// Original: d:\build\custom\temp\code\client\projectg\fxspray.cpp
#include "FxBox.h"
#include "fxspray.h"
#include "wscene.h"
#include "petbody.h"
#include "wpuppet.h"
#include "SoundManager.h"
#include "wutil.h"
#include "wview.h"
#include "fxsequence.h"
#include "binstr.h"
#include "westpak.h"
#include "wresrcmng.h"
#include "w3danispr.h"
#include <float.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef PG_MATERIAL_HINTS
// Port only (port/compat/pg_material_hints.h): the glow of a lamp or a
// lighthouse casts no sun shadow in the port's renderer. Told by the file
// name of the spray or of the sequence that opened it (bg/blue's
// blue_lighthouse_glow.seq, blue_streetlamp.seq); smoke keeps its shadow.
#include <ctype.h>
#include "pg_material_hints.h"

static bool PgFxNamedLight(const char *name) {
	static const char *s_words[] = { "glow", "lamp", "light", "beam" };
	if (name == 0) {
		return false;
	}
	char lower[0x104];
	size_t n = 0;
	for (; name[n] != 0 && n < sizeof(lower) - 1; n++) {
		lower[n] = (char)tolower((unsigned char)name[n]);
	}
	lower[n] = 0;
	for (size_t i = 0; i < sizeof(s_words) / sizeof(s_words[0]); i++) {
		if (strstr(lower, s_words[i]) != 0) {
			return true;
		}
	}
	return false;
}
#endif

// TODO: m_box.m_originalSprays / m_pResourceManager / m_pCamera once
// FxBox.h befriends CFxSpray (FxBox.h is not this unit's header).
// The original spells these as 32-bit member offsets (0x128/0x140); the
// members themselves are the same access on the match and the only correct
// form on LP64, where those offsets no longer hold (docs/PORTABILITY.md).
#define FXBOX_ORIGINAL_SPRAYS(box) ((box).m_originalSprays)
#define FXBOX_RESOURCE_MANAGER(box) ((box).m_pResourceManager)
#define FXBOX_CAMERA(box) (*(const WMatrix**)((char*)&(box) + 0x168))

// Wangreal's WVector division (wmath.inl), not declared in wmath.h yet.
WVector operator/(const WVector& v, float f);
// Wangreal (wmath), not declared in wmath.h yet.
int WisZero(const float& f, float eps);

// fxspray.cpp:20
CFxParticle::CFxParticle(void) {
	m_pSprite = 0;
	m_pPetBody = 0;
	m_fWindScale = 0.0f;
	m_fScale = 1.0f;
}

// fxspray.cpp:28
CFxParticle::~CFxParticle(void) {
	if (m_pPetBody) { delete m_pPetBody; m_pPetBody = 0; }
}

// fxspray.cpp:33
// Modulates two 0xRRGGBB colours channel by channel (the alpha byte is dropped).
static unsigned long __stdcall ModulateColor(unsigned long color, unsigned long light) {
	unsigned long r = ((color >> 16) & 0xff) * ((light >> 16) & 0xff) >> 8;
	unsigned long g = ((color >> 8) & 0xff) * ((light >> 8) & 0xff) >> 8;
	unsigned long b = (color & 0xff) * (light & 0xff) >> 8;
	return (((r << 8) + g) << 8) + b;
}

// fxspray.cpp:45
// A colour with its alpha byte scaled by fScale.
static unsigned long ScaleAlpha(unsigned long color, float fScale) {
	return ((unsigned char)((color >> 24) * fScale) << 24) | (color & 0xffffff);
}

// fxspray.cpp:51
// The generation area scaled about its center (x and z by fScaleXZ, y by
// fScaleY, only where a scale is below 1). A round area keeps its center in
// min and its radii in max.x/max.y.
static void ScaleArea(Waabb& out, const Waabb& in, bool bRound, float fScaleXZ, float fScaleY) {
	if (!bRound) {
		if (fScaleXZ < 1.0f || fScaleY < 1.0f) {
			WVector center = (in.max + in.min) * 0.5f;
			WVector half = (in.max - in.min) * 0.5f;

			if (fScaleXZ < 1.0f) {
				out.min.x = (center.x - half.x) * fScaleXZ;
				out.min.z = (center.z - half.z) * fScaleXZ;
				out.max.x = (half.x + center.x) * fScaleXZ;
				out.max.z = (center.z + half.z) * fScaleXZ;
			}
			else {
				out.min.x = in.min.x; out.max.x = in.max.x;
				out.min.z = in.min.z; out.max.z = in.max.z;
			}

			if (fScaleY < 1.0f) {
				out.min.y = (center.y - half.y) * fScaleY;
				out.max.y = (center.y + half.y) * fScaleY;
			}
			else {
				out.min.y = in.min.y; out.max.y = in.max.y;
			}
		}
		else {
			out = in;
		}
	}
	else {
		out.min = in.min;
		out.max.x = fScaleXZ * in.max.y;
		out.max.y = fScaleY * in.max.y;
		out.max.z = 0.0f;
	}
}

// fxspray.cpp:96
// The opacity factor of a particle at pos (in the spray's space) inside the
// generation area: 1 in the core (inner, ScaleArea's result), falling to 0
// at the edge of the whole area (outer).
static float GetVolumeFactor(const Waabb& inner, const WVector& pos, const Waabb& outer, float fScaleXZ, float fScaleY, bool bRound) {
	float factor = 1.0f;

	if (!bRound) {
		if (fScaleXZ < 1.0f) {
			if (pos.x < inner.min.x)
				factor = 1.0f - (pos.x - inner.min.x) / (outer.min.x - inner.min.x);
			else if (inner.max.x < pos.x)
				factor = 1.0f - (pos.x - inner.max.x) / (outer.max.x - inner.max.x);

			if (pos.z < inner.min.z)
				factor *= 1.0f - (pos.z - inner.min.z) / (outer.min.z - inner.min.z);
			else if (inner.max.z < pos.z)
				factor *= 1.0f - (pos.z - inner.max.z) / (outer.max.z - inner.max.z);
		}

		if (fScaleY < 1.0f) {
			if (pos.y < inner.min.y)
				factor *= 1.0f - (pos.y - inner.min.y) / (outer.min.y - inner.min.y);
			else if (inner.max.y < pos.y)
				factor *= 1.0f - (pos.y - inner.max.y) / (outer.max.y - inner.max.y);
		}
	}
	else {
		WVector d = pos - outer.min;
		float dist = sqrtf(d.y * d.y + d.x * d.x + d.z * d.z);

		if (dist <= inner.max.x) {
			if (!(WAbsBySign(inner.max.x) < WMATH_EPSILON))
				factor = dist / inner.max.x;
		}
		else if (inner.max.y <= dist) {
			float range = outer.max.y - inner.max.y;
			if (!(WAbsBySign(range) < WMATH_EPSILON))
				factor = 1.0f - (dist - inner.max.y) / (outer.max.y - inner.max.y);
		}
	}

	return factor;
}

// fxspray.cpp:143
// The angle between two vectors, 0 when either is (nearly) null.
static float GetAngle(const WVector& a, const WVector& b) {
	float lenA = a.Magnitude();
	if (WAbsBySign(lenA) < WMATH_EPSILON)
		return 0.0f;
	float lenB = b.Magnitude();
	if (WAbsBySign(lenB) < WMATH_EPSILON)
		return 0.0f;
	float c = (a.x * b.x + a.z * b.z + a.y * b.y) / (lenA * lenB);
	return acosf(Between(-1.0f, c, 1.0f));
}

// fxspray.cpp:162
CFxSpray::CFxSpray(CFxBox& box) : m_box(box) {
	Reset();
	m_fAttachLimitB = m_fAttachLimitA = FLT_MAX; memset(m_pMatrix, 0, sizeof(m_pMatrix));
	memset(m_pOffset, 0, sizeof(m_pOffset));
}

// fxspray.cpp:171
CFxSpray::~CFxSpray(void) {
	for (std::list<CFxParticle*>::iterator it = m_particles.begin(); it != m_particles.end(); ++it)
		delete *it;
	m_particles.clear();
}

// fxspray.cpp:179
// Puts the spray back to its defaults: deletes its particles, clears the
// parameter keys and the defines table.
void CFxSpray::Reset(void) {
	m_bAlive = true; m_bActive = true; m_pAttacher = 0;
	m_pParent = 0;
	m_fTime = 0;

	m_pos = WVector::ZERO;
	m_spawnDir = WVector::ZERO;
	m_matrix.Reset();
	m_baseMatrix.Reset();

	m_outputDir.Reset();
	m_outputPos.Reset();

	m_outputAlpha = 0;
	m_pSpawner = 0;
	for (std::list<CFxParticle*>::iterator it = m_particles.begin(); it != m_particles.end(); ++it)
		delete *it;
	m_particles.clear();

	m_param.m_bRoundArea = false;
	m_param.m_area.min = m_param.m_area.max = WVector::ZERO;
	m_param.m_bRegenAtSamePos = false;
	m_param.m_fCamOffset = 0.0f;
	m_param.m_bBillboard = true;
	m_param.m_pSprite = 0;
	m_param.m_sourceName[0] = 0;
	m_param.m_bPetLight = false;
	m_param.m_bSurvivesSequence = false;
	m_param.m_bTransformPos = false;
	m_param.m_flags = 0;

	m_param.m_fAlphaScale = 1.0f;
	m_param.m_fAreaScaleXZ = m_param.m_fAreaScaleY = 1.0f;
	m_param.m_fFriction = 0.0f;
	m_param.m_fGroundFriction = 0.0f; m_param.m_bScaleArea = m_param.m_iLifetimeMax = 1000;
	m_param.m_gravity = WVector::ZERO;
	m_param.m_fGroundHeight = -1000.0f;
	m_param.m_fGroundBounce = 0.0f;
	m_param.m_fGroundRadius = 0.0f;

	m_param.m_lightSource = 0;
	m_param.m_lightColor = 0xFFFFFF; m_param.m_dstBlend = m_param.m_srcBlend = 0x800000;

	m_param.m_initialVel.min = m_param.m_initialVel.max = WVector::ZERO;
	m_param.m_fWindScaleA = m_param.m_fWindScaleB = 0.0f;
	m_param.m_wind.x = m_param.m_wind.y = m_param.m_wind.z = 0.0f;
	m_param.m_extOrientation = 0;
	m_param.m_initAngle.min = m_param.m_initAngle.max = WVector::ZERO;
	m_param.m_initRotation.min = m_param.m_initRotation.max = WVector::ZERO;
	m_param.m_initFrame[0] = m_param.m_initFrame[1] = 0;
	m_param.m_initSize[0] = m_param.m_initSize[1] = 1.0f;
	m_param.m_initSize[2] = 1.0f;
	m_param.m_tailBytes[0] = 0;
	m_param.m_bTail = 0;
	m_param.m_fTailInterval = 0.0f;
	m_param.m_tailTexture[0] = 0;
	m_param.m_hTexture = 0;
	m_param.m_fBandanaWidth = 50.0f;
	m_param.m_bMonolith = false;
	m_param.m_bBandanaFaceCamera = false; m_param.m_tailBytes[3] = m_param.m_tailBytes[2] = m_param.m_tailBytes[1] = 255;

	m_param.m_rots.clear();
	m_param.m_nozzleRots.clear();
	m_param.m_axisRots.clear();
	m_param.m_revols.clear();
	m_param.m_gens.clear();
	m_param.m_gravities.clear();
	m_param.m_fades.clear();
	m_param.m_sizes.clear();
	m_param.m_frames.clear();
	m_param.m_fDistLimitA = m_param.m_fDistLimitB = 0.0f;
	m_param.m_bSphereKind2 = false;
	m_fSphereAngle = 0;
	memset(&m_defines, 0, sizeof(m_defines));
}

// fxspray.cpp:282
// Reads the spray's ".spr" file the first time; later sprays of the same
// file copy the parameters of the box's original (bOriginal) or parse the
// file again.
bool CFxSpray::ParseFile(const char* name, bool bOriginal) {
	strcpy(m_name, name);

	std::map<std::string, CFxSpray*>::iterator it = FXBOX_ORIGINAL_SPRAYS(m_box).find(name);
	if (it == FXBOX_ORIGINAL_SPRAYS(m_box).end()) {

		cFile* file = FXBOX_RESOURCE_MANAGER(m_box)->GetCFile(name, 0x2000);

		if (file == 0)
			return false;

		char line[1024];
		while (file->Scan("%n", line)) {
			if (!ParseLine(TrimA(line, " \t\n\r"))) {
				CloseCFile(file);
				return false;
			}
		}
		CloseCFile(file);

		CFxSpray* pOriginal = new CFxSpray(m_box);
		pOriginal->m_param = m_param;
		FXBOX_ORIGINAL_SPRAYS(m_box)[name] = pOriginal;

	} else {

		if (bOriginal) {

			m_param = it->second->m_param;

		} else {

			cFile* file = FXBOX_RESOURCE_MANAGER(m_box)->GetCFile(name, 0x2000);

			if (file == 0)
				return false;

			char line[1024];
			while (file->Scan("%n", line)) {
				if (!ParseLine(TrimA(line, " \t\n\r"))) {
					CloseCFile(file);
					return false;
				}
			}
			CloseCFile(file);
		}
	}

	return true;
}

// fxspray.cpp:350
// Reads one "key = value, value, ..." line of a ".spr" file into m_param.
// false when the line is not understood.
bool CFxSpray::ParseLine(const char* line) {
	if (*line == 0 || *line == ';')
		return true;

	if (m_box.PreprocessDefines(m_defines, line))
		return true;

	cTokenV tokens;
	tokens.Init(line, (int)strlen(line));

	char key[0x104];
	char a[16][0x104];

	tokens.GetToken(key, "=", 1);
	TrimA(key, " \t\n\r\0");

	for (int i = 0; i < 16; i++) {
		tokens.GetToken(a[i], ",", 1);
		TrimA(a[i], " \t()%");
		m_box.ProcessDefines(m_defines, a[i]);
	}

	if (!strcmpi(key, "source")) {
		cTokenV words;
		words.Init(a[0], (int)strlen(a[0]));

		char* name;
		words.GetTokenWithoutFail(&name, " \t", 2);

		if (!strcmpi(name, "*pet")) {
			words.GetTokenWithoutFail(&name, " \t", 2);

			if (FXBOX_RESOURCE_MANAGER(m_box) == 0)
				return false;
			const char* match = FXBOX_RESOURCE_MANAGER(m_box)->FindMatchFile(name);
			if (!strcmpi(match, name))
				return false;

			strcpy(m_param.m_sourceName, a[0]);
			m_param.m_bPetLight = !strcmpi(a[1], "petlight");
		} else {
			m_param.m_pSprite = m_box.LoadSprite(a[0], atoi(a[1]), atoi(a[2]));
			if (m_param.m_pSprite == 0)
				return false;

			m_param.m_bBillboard = strlen(a[3]) == 0 || !strcmpi(a[3], "billboard");
		}
	}
	else if (!strcmpi(key, "tail")) {
		m_param.m_bTail = atoi(a[0]);
		m_param.m_fTailInterval = m_box.ParseNumber(a[1]);
		m_param.m_tailBytes[0] = (unsigned char)atoi(a[2]);
		m_param.m_tailBytes[1] = (unsigned char)atoi(a[3]);
		m_param.m_tailBytes[2] = (unsigned char)atoi(a[4]);
		m_param.m_tailBytes[3] = (unsigned char)atoi(a[5]);
		strcpy(m_param.m_tailTexture, a[6]);

		if (strlen(m_param.m_tailTexture) != 0) {
			if (FXBOX_RESOURCE_MANAGER(m_box) == 0)
				return false;
			m_param.m_hTexture = FXBOX_RESOURCE_MANAGER(m_box)->LoadTexture(m_param.m_tailTexture, 0, 0, 0);
			if (m_param.m_hTexture == 0)
				return false;

			m_param.m_fBandanaWidth = (float)atof(a[7]);
			m_param.m_bMonolith = !strcmpi(a[8], "monolith");
			m_param.m_bBandanaFaceCamera = !strcmpi(a[9], "cameraside");
		}
	}
	else if (!strcmpi(key, "issphere")) {
		int kind = atoi(a[0]);
		m_param.m_bRoundArea = kind != 0;
		if (kind == 2)
			m_param.m_bSphereKind2 = true;
	}
	else if (!strcmpi(key, "genpos")) {
		m_param.m_area.min = WVector((float)atof(a[0]), (float)atof(a[1]), (float)atof(a[2]));
		if (!m_param.m_bRoundArea)
			m_param.m_area.max = WVector((float)atof(a[3]), (float)atof(a[4]), (float)atof(a[5]));
		else
			m_param.m_area.max = WVector((float)atof(a[3]), (float)atof(a[4]), m_box.ParseNumber(a[5]));
	}
	else if (!strcmpi(key, "regenatsamepos")) {
		m_param.m_bRegenAtSamePos = atoi(a[0]) != 0;
	}
	else if (!strcmpi(key, "camoffset")) {
		m_param.m_fCamOffset = (float)atof(a[0]);
	}
	else if (!strcmpi(key, "add_generation")) {
		sGen gen;
		gen.t0 = (int)m_box.ParseNumber(a[0]);
		gen.t1 = (int)m_box.ParseNumber(a[1]);
		gen.count = atoi(a[2]);
		gen.spread = m_box.ParseNumber(a[3]);
		gen.generated = 0;
		gen.lastTime = 0.0f;
		m_param.m_gens.push_back(gen);
	}
	else if (!strcmpi(key, "lifetime")) {
		m_param.m_bScaleArea = (int)m_box.ParseNumber(a[0]);
		m_param.m_iLifetimeMax = (int)m_box.ParseNumber(a[1]);

		if (m_param.m_bScaleArea == 0) {
			for (std::list<sFade>::iterator it = m_param.m_fades.begin(); it != m_param.m_fades.end(); ++it) {
				if (it->t0 == 0.0f) {
					it->start[0] = it->end[0] = 255;
					break;
				}
			}
		} else {
			m_param.m_fAreaScaleXZ = m_param.m_fAreaScaleY = 1.0f;
			m_param.m_fAlphaScale = 1.0f;
		}
	}
	else if (!strcmpi(key, "vol_max_opacity")) {
		m_param.m_fAlphaScale = Between<float>(0.0f, (float)atof(a[0]), 1.0f);
		if (m_param.m_bScaleArea != 0)
			m_param.m_fAlphaScale = 1.0f;
	}
	else if (!strcmpi(key, "vol_core")) {
		if (m_param.m_bScaleArea != 0) {
			m_param.m_fAreaScaleXZ = m_param.m_fAreaScaleY = 1.0f;
		} else {
			m_param.m_fAreaScaleXZ = Between<float>(0.0f, (float)atof(a[0]), 1.0f);
			m_param.m_fAreaScaleY = Between<float>(0.0f, (float)atof(a[1]), 1.0f);
		}
	}
	else if (!strcmpi(key, "init_velocity")) {
		m_param.m_initialVel.min = WVector((float)atof(a[0]), (float)atof(a[1]), (float)atof(a[2]));
		m_param.m_initialVel.max = WVector((float)atof(a[3]), (float)atof(a[4]), (float)atof(a[5]));
	}
	else if (!strcmpi(key, "add_velocity")) {
		sVel vel;
		vel.t0 = (int)m_box.ParseNumber(a[0]);
		vel.v0 = (float)atof(a[1]);
		vel.t1 = (int)m_box.ParseNumber(a[2]);
		vel.v1 = (float)atof(a[3]);
		m_param.m_vels.push_back(vel);
	}
	else if (!strcmpi(key, "add_wind")) {
		m_param.m_fWindScaleA = (float)atof(a[0]);
		m_param.m_fWindScaleB = (float)atof(a[1]);
	}
	else if (!strcmpi(key, "ext_wind")) {
		m_param.m_wind = WVector((float)atof(a[0]), (float)atof(a[1]), (float)atof(a[2]));
	}
	else if (!strcmpi(key, "ext_orientation")) {
		int bit1 = 0;
		int bit2 = 0;
		int bit4 = 0;
		sscanf(a[0], "%d", &bit1);
		sscanf(a[1], "%d", &bit2);
		sscanf(a[2], "%d", &bit4);

		m_param.m_extOrientation = 0;
		if (bit1)
			m_param.m_extOrientation = 1;
		if (bit2)
			m_param.m_extOrientation |= 2;
		if (bit4)
			m_param.m_extOrientation |= 4;
	}
	else if (!strcmpi(key, "init_angle")) {
		m_param.m_initAngle.min.x = m_box.ParseNumber(a[0]);
		m_param.m_initAngle.min.y = m_box.ParseNumber(a[1]);
		m_param.m_initAngle.min.z = m_box.ParseNumber(a[2]);
		m_param.m_initAngle.max.x = m_box.ParseNumber(a[3]);
		m_param.m_initAngle.max.y = m_box.ParseNumber(a[4]);
		m_param.m_initAngle.max.z = m_box.ParseNumber(a[5]);
	}
	else if (!strcmpi(key, "init_rotation")) {
		m_param.m_initRotation.min.x = m_box.ParseNumber(a[0]);
		m_param.m_initRotation.min.y = m_box.ParseNumber(a[1]);
		m_param.m_initRotation.min.z = m_box.ParseNumber(a[2]);
		m_param.m_initRotation.max.x = m_box.ParseNumber(a[3]);
		m_param.m_initRotation.max.y = m_box.ParseNumber(a[4]);
		m_param.m_initRotation.max.z = m_box.ParseNumber(a[5]);
	}
	else if (!strcmpi(key, "add_rotation")) {
		sRot rot;
		rot.m_v[0] = (float)atof(a[0]);
		rot.m_v[1] = m_box.ParseNumber(a[1]);
		rot.m_v[2] = (float)atof(a[2]);
		rot.m_v[3] = m_box.ParseNumber(a[3]);
		m_param.m_rots.push_back(rot);
	}
	else if (!strcmpi(key, "friction")) {
		m_param.m_fFriction = (float)atof(a[0]);
	}
	else if (!strcmpi(key, "groundfriction")) {
		m_param.m_fGroundFriction = (float)atof(a[0]);
	}
	else if (!strcmpi(key, "gravity")) {
		m_param.m_gravity = WVector((float)atof(a[0]), (float)atof(a[1]), (float)atof(a[2]));
	}
	else if (!strcmpi(key, "add_gravitypoint")) {
		sGravity gravity;
		gravity.m_pos.x = (float)atof(a[0]);
		gravity.m_pos.y = (float)atof(a[1]);
		gravity.m_pos.z = (float)atof(a[2]);
		gravity.m_fInnerForce = (float)atof(a[3]);
		gravity.m_fRadius = (float)atof(a[4]);
		gravity.m_fOuterForce = (float)atof(a[5]);
		m_param.m_gravities.push_back(gravity);
	}
	else if (!strcmpi(key, "ground")) {
		m_param.m_fGroundHeight = (float)atof(a[0]);
		m_param.m_fGroundBounce = (float)atof(a[1]);
		m_param.m_fGroundRadius = (float)atof(a[2]);
	}
	else if (!strcmpi(key, "blend")) {
		if (!strcmpi(a[0], "normal"))
			m_param.m_srcBlend = 0;
		else if (!strcmpi(a[0], "add"))
			m_param.m_srcBlend = 0x800000;
		else if (!strcmpi(a[0], "multiply"))
			m_param.m_srcBlend = 0x1000000;
		else if (!strcmpi(a[0], "invmultiply"))
			m_param.m_srcBlend = 0x1800000;
		else
			return false;

		if (!strcmpi(a[1], "normal"))
			m_param.m_dstBlend = 0;
		else if (!strcmpi(a[1], "add"))
			m_param.m_dstBlend = 0x800000;
		else if (!strcmpi(a[1], "multiply"))
			m_param.m_dstBlend = 0x1000000;
		else if (!strcmpi(a[1], "invmultiply"))
			m_param.m_dstBlend = 0x1800000;
		else
			return false;
	}
	else if (!strcmpi(key, "lighting")) {
		if (!strcmpi(a[0], "off"))
			m_param.m_lightSource = 0;
		else if (!strcmpi(a[0], "diffuse"))
			m_param.m_lightSource = 1;
		else if (!strcmpi(a[0], "ambient"))
			m_param.m_lightSource = 2;
		else if (!strcmpi(a[0], "ambient2"))
			m_param.m_lightSource = 3;
		else
			return false;
	}
	else if (!strcmpi(key, "add_fade")) {
		sFade fade;
		fade.t0 = (float)atof(a[0]);
		fade.start[0] = (unsigned char)atoi(a[1]);
		fade.start[1] = (unsigned char)atoi(a[2]);
		fade.start[2] = (unsigned char)atoi(a[3]);
		fade.start[3] = (unsigned char)atoi(a[4]);
		fade.t1 = (float)atof(a[5]);
		fade.end[0] = (unsigned char)atoi(a[6]);
		fade.end[1] = (unsigned char)atoi(a[7]);
		fade.end[2] = (unsigned char)atoi(a[8]);
		fade.end[3] = (unsigned char)atoi(a[9]);

		if (m_param.m_bScaleArea == 0 && fade.t0 == 0.0f) {
			fade.end[0] = 255;
			fade.start[0] = 255;
		}
		m_param.m_fades.push_back(fade);
	}
	else if (!strcmpi(key, "init_size")) {
		sscanf(a[0], "%f", &m_param.m_initSize[0]);
		sscanf(a[1], "%f", &m_param.m_initSize[1]);
		sscanf(a[2], "%f", &m_param.m_initSize[2]);
	}
	else if (!strcmpi(key, "add_size")) {
		sSize size;
		sscanf(a[0], "%f", &size.m_v[0]);
		sscanf(a[1], "%f", &size.m_v[1]);
		sscanf(a[2], "%f", &size.m_v[2]);
		sscanf(a[3], "%f", &size.m_v[3]);
		m_param.m_sizes.push_back(size);
	}
	else if (!strcmpi(key, "init_frame")) {
		sscanf(a[0], "%d", &m_param.m_initFrame[0]);
		sscanf(a[1], "%d", &m_param.m_initFrame[1]);
	}
	else if (!strcmpi(key, "add_frame")) {
		sFrame frame;
		sscanf(a[0], "%f", &frame.m_f0);
		sscanf(a[1], "%f", &frame.m_f1);
		sscanf(a[2], "%d", &frame.m_i2);
		sscanf(a[3], "%d", &frame.m_i3);
		frame.m_i4 = (int)m_box.ParseNumber(a[4]);
		m_param.m_frames.push_back(frame);
	}
	else if (!strcmpi(key, "add_quake")) {
		sQuake quake;
		quake.m_keys[0].p0 = (float)atof(a[0]);
		quake.m_keys[0].p1 = (float)atof(a[1]);
		quake.m_keys[0].p2 = (float)atof(a[2]);
		quake.m_keys[0].t = (int)m_box.ParseNumber(a[3]);
		quake.m_keys[1].p0 = (float)atof(a[4]);
		quake.m_keys[1].p1 = (float)atof(a[5]);
		quake.m_keys[1].p2 = (float)atof(a[6]);
		quake.m_keys[1].t = (int)m_box.ParseNumber(a[7]);
		quake.m_keys[2].p0 = (float)atof(a[8]);
		quake.m_keys[2].p1 = (float)atof(a[9]);
		quake.m_keys[2].p2 = (float)atof(a[10]);
		quake.m_keys[2].t = (int)m_box.ParseNumber(a[11]);
		quake.m_fLastTime = 0;
		m_param.m_quakes.push_back(quake);
	}
	else if (!strcmpi(key, "add_flash")) {
		sFlash flash;
		flash.m_v[0] = (int)m_box.ParseNumber(a[0]);
		flash.m_v[1] = (int)m_box.ParseNumber(a[1]);
		flash.m_v[2] = (int)m_box.ParseNumber(a[2]);
		m_param.m_flashes.push_back(flash);
	}
	else if (!strcmpi(key, "dist_limit")) {
		m_param.m_fDistLimitA = Max<float>(0.0f, (float)atof(a[0]));
		m_param.m_fDistLimitB = Max<float>(0.0f, (float)atof(a[1]));
		if (m_param.m_fDistLimitB < m_param.m_fDistLimitA)
			m_param.m_fDistLimitB = m_param.m_fDistLimitA;
	}
	else if (!strcmpi(key, "add_bubble")) {
		sBubble bubble;
		bubble.m_min.x = (float)atof(a[0]);
		bubble.m_max.x = (float)atof(a[1]);
		bubble.m_min.y = (float)atof(a[2]);
		bubble.m_max.y = (float)atof(a[3]);
		bubble.m_min.z = (float)atof(a[4]);
		bubble.m_max.z = (float)atof(a[5]);
		m_param.m_bubbles.push_back(bubble);
	}
	else if (!strcmpi(key, "sound")) {
		sSound sound;
		sound.m_fVolume = -1.0f;
		strcpy(sound.m_name, a[0]);

		char* ext = strstr(sound.m_name, ".wav");
		if (ext)
			*ext = 0;

		sscanf(a[1], "%f", &sound.m_fVolume);
		m_param.m_sounds.push_back(sound);
	}
	else {
		return false;
	}

	return true;
}

// fxspray.cpp:847
// Advances the spray by fDelta milliseconds: moves and turns it, runs its
// camera quake and flash keys, emits the due particles, then moves, fades,
// sizes, animates and spawns from the live ones (respawning or deleting the
// dead). false once no particle is left and no generation key is pending.
bool CFxSpray::Process(float fDelta) {
	float fScale = fDelta * 0.06f;

	m_pos += m_spawnDir * fScale;

	if (m_param.m_bTransformPos && m_pParent) {
		WMatrix bone;
		m_pParent->GetBoneMatrix(&bone);
		bone.GetRotMatrix(&m_matrix);

		if (m_param.m_flags != 0 && m_param.m_flags != 7) {
			WVector angles(0.0f, 0.0f, 0.0f);

			if (m_param.m_flags & 1) {
				WVector axis = WVector::UNIT_POS_Y;
				WVector v;
				v = axis * m_matrix;
				v.x = 0.0f;
				angles.x = GetAngle(axis, v);
				if (!WisZero(v.z, WMATH_EPSILON))
					angles.x *= v.z < 0.0f ? -1.0f : 1.0f;
			}

			if (m_param.m_flags & 2) {
				WVector axis = WVector::UNIT_POS_Z;
				WVector v;
				v = axis * m_matrix;
				v.y = 0.0f;
				angles.y = GetAngle(axis, v);
				if (!WisZero(v.x, WMATH_EPSILON))
					angles.y *= v.x < 0.0f ? -1.0f : 1.0f;
			}

			if (m_param.m_flags & 4) {
				WVector axis = WVector::UNIT_POS_X;
				WVector v;
				v = axis * m_matrix;
				v.z = 0.0f;
				angles.z = GetAngle(axis, v);
				if (!WisZero(v.y, WMATH_EPSILON))
					angles.z *= v.y > 0.0f ? -1.0f : 1.0f;
			}

			m_matrix = ~RotMat(angles);
		}
	}

	WMatrix nozzle = RotateNozzle(m_fTime, fScale);
	m_baseMatrix = m_baseMatrix * nozzle;

	WMatrix rot = Rotate(m_fTime, fScale);
	Revolute(m_fTime, fScale);
	RevoluteGrand(m_fTime, fScale);

	// Camera quake keys: a random shake of m_outputDir, m_outputPos the
	// amplitude-weighted average of the keys' directions.
	m_outputPos.Reset();
	bool bQuake = false;
	std::list<sQuake>::iterator quake = m_param.m_quakes.begin();
	WVector ampSum;
	ampSum.Reset();
	for (; quake != m_param.m_quakes.end(); ++quake) {
		// m_keys[0]: the direction and t0, m_keys[1]: the amplitude at t0 and
		// t1, m_keys[2]: the amplitude at t1 and the shake interval,
		// m_fLastTime: the time of the last shake.
		// The key times are read once: the shake below takes t1 - t0 from the
		// values the tests loaded.
		int k0 = quake->m_keys[0].t;
		float t0 = (float)k0;
		if (m_fTime < t0) {
			quake->m_fLastTime = m_fTime;
		}
		else {
			int k1 = quake->m_keys[1].t;
			if ((float)k1 >= m_fTime) {
				bQuake = true;
				if (m_fTime - quake->m_fLastTime >= (float)quake->m_keys[2].t) {
					quake->m_fLastTime = m_fTime;

					// TODO: fxspray.h's sQuake::sKey should hold a WVector (p) instead of
					// p0..p2; the original reads the keys as WVector lvalues.
					WVector amp = k1 - k0 > 0
						? (*(const WVector*)&quake->m_keys[2].p0 - *(const WVector*)&quake->m_keys[1].p0) * (m_fTime - t0) / (float)(k1 - k0)
							+ *(const WVector*)&quake->m_keys[1].p0
						: *(const WVector*)&quake->m_keys[1].p0;

					m_outputDir.Reset();
					m_outputDir.x += (float)(rand() % 20001 - 10000) * amp.x * 0.00005f;
					m_outputDir.y += (float)(rand() % 20001 - 10000) * amp.y * 0.00005f;
					m_outputDir.z += (float)(rand() % 20001 - 10000) * amp.z * 0.00005f;

					m_outputPos.x += amp.x * quake->m_keys[0].p0;
					m_outputPos.y += amp.y * quake->m_keys[0].p1;
					m_outputPos.z += amp.z * quake->m_keys[0].p2;

					ampSum += amp;
				}
			}
		}
	}

	if (!bQuake) {
		m_outputDir.Reset();
	}
	else {
		if (!(WAbsBySign(ampSum.x) < WMATH_EPSILON))
			m_outputPos.x /= ampSum.x;
		if (!(WAbsBySign(ampSum.y) < WMATH_EPSILON))
			m_outputPos.y /= ampSum.y;
		if (!(WAbsBySign(ampSum.z) < WMATH_EPSILON))
			m_outputPos.z /= ampSum.z;
	}

	// Flash keys: m_v[0] start, m_v[1] full flash, m_v[2] end.
	m_outputAlpha = 0;
	for (std::list<sFlash>::iterator flash = m_param.m_flashes.begin(); flash != m_param.m_flashes.end(); ++flash) {
		float t0 = (float)flash->m_v[0];
		if (m_fTime >= t0) {
			if ((float)flash->m_v[1] >= m_fTime) {
				int alpha = flash->m_v[1] - flash->m_v[0] > 0
					? (int)(m_fTime - t0) * 255 / (flash->m_v[1] - flash->m_v[0])
					: 0;
				alpha += m_outputAlpha;
				if (alpha > 255)
					alpha = 255;
				m_outputAlpha = (unsigned char)alpha;
			}
			else if ((float)flash->m_v[2] >= m_fTime) {
				int alpha = flash->m_v[2] - flash->m_v[1] > 0
					? 255 - (int)(m_fTime - (float)flash->m_v[1]) * 255 / (flash->m_v[2] - flash->m_v[1])
					: 0;
				alpha += m_outputAlpha;
				if (alpha > 255)
					alpha = 255;
				m_outputAlpha = (unsigned char)alpha;
			}
		}
	}

	bool bGenerating = Generate(m_fTime);

	Waabb area;
	bool bRegenerated = false;
	ScaleArea(area, m_param.m_area, m_param.m_bRoundArea, m_param.m_fAreaScaleXZ, m_param.m_fAreaScaleY);

	std::list<CFxParticle*>::iterator it = m_particles.begin();
	while (it != m_particles.end()) {
		CFxParticle* particle = *it;

		WVector bgPos = GetBgPos(particle);
		WVector pos = bgPos + particle->m_basePos * GetBgRot(particle);

		// The particle's age in percent of its life.
		float fAge = particle->m_fLifeTime > 0.0f ? (particle->m_fLifeTime - particle->m_fLifeLeft) / particle->m_fLifeTime * 100.0f : 100.0f;

		MakeTail(m_fTime, particle, pos + particle->m_bubbleOffset);

		if (!particle->m_bDead) {
			WVector oldPos = pos;
			WVector wind = m_param.m_wind * particle->m_fWindScale;
			pos += (particle->m_dir * GetBgRot(particle) + wind) * fScale;

			WVector friction = particle->m_dir;
			friction = friction.Normalize() * fScale
				* (particle->m_bGrounded ? m_param.m_fGroundFriction : m_param.m_fFriction);

			// A component the friction would carry past zero stops.
			if ((particle->m_dir.x > 0.0f && particle->m_dir.x < friction.x) || (particle->m_dir.x < 0.0f && particle->m_dir.x > friction.x))
				particle->m_dir.x = 0.0f;
			else
				particle->m_dir.x -= friction.x;

			if ((particle->m_dir.y > 0.0f && particle->m_dir.y < friction.y) || (particle->m_dir.y < 0.0f && particle->m_dir.y > friction.y))
				particle->m_dir.y = 0.0f;
			else
				particle->m_dir.y -= friction.y;

			if ((particle->m_dir.z > 0.0f && particle->m_dir.z < friction.z) || (particle->m_dir.z < 0.0f && particle->m_dir.z > friction.z))
				particle->m_dir.z = 0.0f;
			else
				particle->m_dir.z -= friction.z;

			if (!particle->m_bGrounded) {
				// Gravity points: m_v[0..2] the point, m_v[3] the strength
				// outside m_v[4] (falling with the squared distance), m_v[5]
				// the strength inside.
				for (std::list<sGravity>::iterator gravity = m_param.m_gravities.begin(); gravity != m_param.m_gravities.end(); ++gravity) {
					WVector d = GetBgPos() + gravity->m_pos - pos;
					float dist = d.Magnitude();
					if (dist < gravity->m_fRadius) {
						float sq = dist * dist;
						if (sq < 0.5f)
							sq = 0.5f;
						particle->m_dir += d.Normalize() * fScale * gravity->m_fInnerForce * (1.0f / sq);
					}
					else {
						particle->m_dir += d.Normalize() * fScale * gravity->m_fOuterForce;
					}
				}

				WVector bg = GetBgPos(particle);
				float height = bg.y + m_param.m_fGroundHeight;
				if (m_box.CheckGroundCollision(oldPos, pos, height, m_param.m_fGroundRadius)) {
					float depth = pos.y - m_param.m_fGroundRadius;
					depth = height - depth > 0.0f ? height - depth : depth - height;

					pos.y = oldPos.y < pos.y ? height - m_param.m_fGroundRadius : height + m_param.m_fGroundRadius;
					particle->m_dir.y = m_param.m_fGroundBounce * particle->m_dir.y * -0.01f;
					float speedY = Abs(particle->m_dir.y);

					if ((depth < 0.03f && speedY < depth) || speedY < 0.01f
						|| WDotProduct(m_param.m_gravity + particle->m_dir, particle->m_dir) < 0.0f) {
						particle->m_dir.y = 0.0f;
						particle->m_bGrounded = true;
					}

					for (std::list<sSpawn>::iterator spawn = m_param.m_spawns.begin(); spawn != m_param.m_spawns.end(); ++spawn) {
						if (spawn->m_type == 3)
							Spawn(*spawn, particle);
					}
				}
				else {
					particle->m_dir += m_param.m_gravity * fScale;
				}

				if (m_param.m_axisRots.size() != 0)
					pos = (pos - GetBgPos()) * rot + GetBgPos();
			}
		}

		// Fade keys: the colour bytes interpolated over the age.
		for (std::list<sFade>::iterator fade = m_param.m_fades.begin(); fade != m_param.m_fades.end(); ++fade) {
			const sFade& f = *fade;
			if (fAge >= f.t0 && fAge <= f.t1) {
				for (int k = 0; k < 4; k++) {
					particle->m_color.bytes[3 - k] = f.t1 - f.t0 > 0.0f
						? (unsigned char)((float)(f.end[k] - f.start[k]) * (fAge - f.t0) / (f.t1 - f.t0) + f.start[k])
						: f.start[k];
				}
			}
		}

		if (particle->m_pSprite != 0 || particle->m_pPetBody != 0) {
			// Rotation keys (m_v: t0, speed at t0, t1, speed at t1).
			for (std::list<sRot>::iterator r = m_param.m_rots.begin(); r != m_param.m_rots.end(); ++r) {
				if (fAge >= r->m_v[0] && fAge <= r->m_v[2]) {
					const sRot& key = *r;
					float speed = key.m_v[2] - key.m_v[0] > 0.0f
						? (key.m_v[3] - key.m_v[1]) * (fAge - key.m_v[0]) / (key.m_v[2] - key.m_v[0]) + key.m_v[1]
						: key.m_v[1];
					particle->m_rotation += fScale * particle->m_rotSpeed * speed * 0.01f;
				}
			}

			// Size keys (m_v: t0, size at t0, t1, size at t1, in percent).
			for (std::list<sSize>::iterator size = m_param.m_sizes.begin(); size != m_param.m_sizes.end(); ++size) {
				if (fAge >= size->m_v[0] && fAge <= size->m_v[2]) {
					const sSize& key = *size;
					float scale = key.m_v[2] - key.m_v[0] > 0.0f
						? (key.m_v[3] - key.m_v[1]) * (fAge - key.m_v[0]) / (key.m_v[2] - key.m_v[0]) + key.m_v[1]
						: key.m_v[1];
					particle->m_fSize = scale * particle->m_fBaseSize * 0.01f;
				}
			}

			if (!particle->m_bGrounded) {
				// Frame keys (m_f0..m_f1 the age range, m_i2..m_i3 the
				// frames, -1 for a random one below m_i3, m_i4 the interval).
				for (std::list<sFrame>::iterator frame = m_param.m_frames.begin(); frame != m_param.m_frames.end(); ++frame) {
					if (fAge >= frame->m_f0 && fAge <= frame->m_f1) {
						if (m_fTime - particle->m_fFrameTime >= (float)frame->m_i4) {
							particle->m_fFrameTime = m_fTime;
							if (frame->m_i2 == -1) {
								particle->m_frame = rand() % frame->m_i3;
							}
							else {
								particle->m_frame++;
								if (particle->m_frame > frame->m_i3)
									particle->m_frame = frame->m_i2;
							}
						}
					}
				}
			}

			if (m_param.m_bScaleArea == 0 && fDelta != 0.0f) {
				// Outside the generation area the particle starts again;
				// inside, it fades towards the area's edge.
				WMatrix bone;
				GetBoneMatrix(&bone);
				WMatrix boneRot;
				bone.GetRotMatrix(&boneRot);

				WVector local;
				local = (pos - GetBgPos(particle)) * ~boneRot;

				bool bInside;
				if (!m_param.m_bRoundArea) {
					WVector vmin(m_param.m_area.min.x - WMATH_EPSILON, m_param.m_area.min.y - WMATH_EPSILON, m_param.m_area.min.z - WMATH_EPSILON);
					WVector vmax(m_param.m_area.max.x + WMATH_EPSILON, m_param.m_area.max.y + WMATH_EPSILON, m_param.m_area.max.z + WMATH_EPSILON);
					Waabb box(vmin, vmax);
					bInside = box.IsInclude(local);
				}
				else {
					WVector d = local - m_param.m_area.min;
					float dist = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
					bInside = dist < m_param.m_area.max.y + 0.0001f;
				}

				if (bInside) {
					particle->m_fScale = GetVolumeFactor(area, local, m_param.m_area, m_param.m_fAreaScaleXZ, m_param.m_fAreaScaleY, m_param.m_bRoundArea);
				}
				else {
					InitParticle(m_param.m_bRegenAtSamePos, particle, 100.0f, 0.0f, area, true);
					bRegenerated = false;
					++it;
					continue;
				}
			}

			if (particle->m_pPetBody) {
				particle->m_pPetBody->SetAlpha((unsigned char)(particle->m_color.bytes[3] * particle->m_fScale * m_param.m_fAlphaScale));
				particle->m_pPetBody->SetLight();
				particle->m_pPetBody->SetPos(pos);
				particle->m_pPetBody->SetRotate(particle->m_rotation);
				particle->m_pPetBody->MultRotate(&m_matrix);
				particle->m_pPetBody->SetScale(particle->m_fSize);
				particle->m_pPetBody->Process(fDelta * 0.001f);
			}
		}

		// Age and interval spawns.
		for (std::list<sSpawn>::iterator spawn = m_param.m_spawns.begin(); spawn != m_param.m_spawns.end(); ++spawn) {
			const sSpawn& key = *spawn;
			if (key.m_type == 1 && particle->m_spawnCount == 0 && fAge + 1.0f > key.m_fParam)
				Spawn(key, particle);

			if (key.m_type == 0) {
				if (m_fTime - particle->m_fSpawnTime > key.m_fParam && key.m_intervalCount > particle->m_spawnCount) {
					particle->m_fSpawnTime = m_fTime;
					Spawn(key, particle);
				}
			}
		}

		if (particle->m_fLifeLeft > 0.0f && !bRegenerated) {
			particle->m_fLifeLeft -= fDelta;

			if (particle->m_fLifeLeft <= 0.0f) {
				for (std::list<sSpawn>::iterator spawn = m_param.m_spawns.begin(); spawn != m_param.m_spawns.end(); ++spawn) {
					if (spawn->m_type == 2)
						Spawn(*spawn, particle);
				}

				if (m_param.m_bRegenAtSamePos) {
					// Processed again as a new particle, its life untouched.
					bRegenerated = true;
					InitParticle(m_param.m_bRegenAtSamePos, particle, 100.0f, 0.0f, area, true);
					continue;
				}

				it = m_particles.erase(it);
				delete particle;
				continue;
			}
		}

		// Bubble keys: a wobble across the particle's direction (m_min the
		// amplitude, m_max the frequency per axis, a random phase per key in
		// the particle's m_points).
		if (m_param.m_bubbles.size() != 0) {
			std::list<WVector>::iterator phase = particle->m_points.begin();
			WVector wobble;
			wobble.Reset();

			for (std::list<sBubble>::iterator bubble = m_param.m_bubbles.begin(); bubble != m_param.m_bubbles.end(); ++bubble) {
				if (phase == particle->m_points.end()) {
					particle->m_points.push_back(WVector((float)(rand() % 1001) * 0.0062831859f,
						(float)(rand() % 1001) * 0.0062831859f, (float)(rand() % 1001) * 0.0062831859f));
					--phase;
				}

				std::list<WVector>::iterator p = phase++;

				wobble.x += cosf(bubble->m_max.x * m_fTime * 0.0062831859f + p->x) * bubble->m_min.x;
				wobble.y += cosf(bubble->m_max.y * m_fTime * 0.0062831859f + p->y) * bubble->m_min.y;
				wobble.z += cosf(bubble->m_max.z * m_fTime * 0.0062831859f + p->z) * bubble->m_min.z;
			}

			WVector dir = particle->m_dir;
			WVector side = IsEqual(particle->m_dir.x, 0.0f) && IsEqual(particle->m_dir.z, 0.0f)
				? WVector::UNIT_POS_Z
				: WCrossProduct(WVector::UNIT_POS_Y, particle->m_dir);
			WVector up = WCrossProduct(dir, side);

			particle->m_bubbleOffset += (side.Normalize() * wobble.z + up.Normalize() * wobble.y + dir.Normalize() * wobble.x) * fScale;
		}

		// Sounds play once, at the particle (relative to the camera) when
		// they have a volume.
		if (m_param.m_sounds.size() != 0) {
			for (std::list<sSound>::iterator sound = m_param.m_sounds.begin(); sound != m_param.m_sounds.end(); ++sound) {
				const sSound& s = *sound;
				if (s.m_fVolume < 0.0f) {
					g_audio->PlaySfx(s.m_name, 0, 0, 0, 0, 0.5f, 200.0f);
				}
				else {
					WVector rel = particle->m_basePos - m_box.m_pCamera->pivot;
					g_audio->PlaySfx(s.m_name, &rel, 5, 0, 0, 0.0f, s.m_fVolume);
				}
			}
			m_param.m_sounds.clear();
		}

		particle->m_basePos = (pos - GetBgPos(particle)) * ~GetBgRot(particle);

		bRegenerated = false;
		++it;
	}

	m_fTime += fDelta;

	return m_particles.size() > 0 || bGenerating;
}

// fxspray.cpp:1388
// Draws the particles (sprite, tail, bandana) with the screen centred on the
// attachment's limits; in the box's debug mode also the generation area and
// the gravity points.
void CFxSpray::Display(WView* pView) {
	if (!m_bActive)
		return;

	pView->SetScreenCenter(m_fAttachLimitA == g_HUGE ? pView->GetWidth() * 0.5f : m_fAttachLimitA,
		m_fAttachLimitB == g_HUGE ? pView->GetHeight() * 0.5f : m_fAttachLimitB);
	pView->UpdateCamera();

	m_displayedCount2 = m_displayedCount = 0;

	if (m_box.m_flag124) {
		if (!m_param.m_bRoundArea) {
			pView->DrawAABB(m_param.m_area + GetBgPos(), 0xFFFF0000, false, 0);
			pView->DrawSphere(GetBgPos() + (m_param.m_area.min + m_param.m_area.max) * 0.5f, 0.5f, 0xFFFF0000, 10, 0);
		}
		else {
			WSphere sphere(m_param.m_area.min, m_param.m_area.max.x);
			pView->DrawSphere(sphere, 0xFFFF0000, 0x40, 0);
			sphere = WSphere(m_param.m_area.min, m_param.m_area.max.y);
			pView->DrawSphere(sphere, 0xFFFF0000, 0x40, 0);
		}

		for (std::list<sGravity>::iterator g = m_param.m_gravities.begin(); g != m_param.m_gravities.end(); ++g)
			pView->DrawSphere(GetBgPos() + g->m_pos, 0.3f, 0xFFFFFF00, 10, 0);
	}

#ifdef PG_MATERIAL_HINTS
	int pgHint = PgMaterialHintGet();
	if (PgFxNamedLight(m_name) || (m_pParent != 0 && PgFxNamedLight(m_pParent->m_name.c_str()))) {
		PgMaterialHint(pgHint | PG_HINT_LIGHT | PG_HINT_EFFECT);
	}
#endif
	for (std::list<CFxParticle*>::iterator it = m_particles.begin(); it != m_particles.end(); ++it) {
		m_displayedCount++;
		CFxParticle* particle = *it;
		WVector pos;
		pos = (particle->m_basePos + particle->m_bubbleOffset) * GetBgRot(particle) + GetBgPos(particle);
		m_drawBgPos = GetBgPos(particle);
		DrawSprite(pView, particle, pos);
		DrawTail(pView, particle, pos);
		DrawBandana(pView, particle, pos);
	}
#ifdef PG_MATERIAL_HINTS
	PgMaterialHint(pgHint);
#endif

	pView->ResetScreenCenter();
	pView->UpdateCamera();
}

// fxspray.cpp:1451
// The rotation of the keys of m_axisRots active at fTime, their angles
// interpolated and scaled by fScale (the last active key wins).
WMatrix CFxSpray::Rotate(float fTime, float fScale) {
	WMatrix mat;
	mat.Reset();
	std::list<sNozzleRot>::iterator end = m_param.m_axisRots.end();
	for (std::list<sNozzleRot>::iterator it = m_param.m_axisRots.begin(); it != end; ++it) {
		const sNozzleRot& rot = *it;
		if (fTime >= (float)rot.t0 && (float)rot.t1 >= fTime) {
			float angle = (fTime - rot.t0) > 0.0f ? (fTime - rot.t0) * (rot.a1 - rot.a0) / (rot.t1 - rot.t0) + rot.a0 : rot.a0;
			mat = RotMat(it->axis, angle * fScale);
		}
	}
	return mat;
}

// fxspray.cpp:1474
// Rotate, for the nozzle keys (m_nozzleRots).
WMatrix CFxSpray::RotateNozzle(float fTime, float fScale) {
	WMatrix mat;
	mat.Reset();
	std::list<sNozzleRot>::iterator end = m_param.m_nozzleRots.end();
	for (std::list<sNozzleRot>::iterator it = m_param.m_nozzleRots.begin(); it != end; ++it) {
		const sNozzleRot& rot = *it;
		if (fTime >= (float)rot.t0 && (float)rot.t1 >= fTime) {
			float angle = (fTime - rot.t0) > 0.0f ? (fTime - rot.t0) * (rot.a1 - rot.a0) / (rot.t1 - rot.t0) + rot.a0 : rot.a0;
			mat = RotMat(it->axis, angle * fScale);
		}
	}
	return mat;
}

// fxspray.cpp:1498
// Turns the spray's position, its direction and its rotation axes about the
// active revolution keys (m_revols).
void CFxSpray::Revolute(float fTime, float fScale) {
	for (std::list<sRevol>::iterator it = m_param.m_revols.begin(); it != m_param.m_revols.end(); ++it) {
		if (fTime >= it->t0 && fTime <= it->t1) {
			float speed = (it->t1 - it->t0 > 0) ? (fTime - it->t0) * (it->speed1 - it->speed0) / (it->t1 - it->t0) + it->speed0 : it->speed0;

			WMatrix rot = RotMat(it->axis, speed * fScale);

			WVector prev = m_pos;
			m_pos = (m_pos - it->center) * rot + it->center;

			m_spawnDir += prev;
			m_spawnDir = (m_spawnDir - it->center) * rot + it->center;
			m_spawnDir -= m_pos;

			for (std::list<sNozzleRot>::iterator r = m_param.m_axisRots.begin(); r != m_param.m_axisRots.end(); ++r) {
				WVector& axis = r->axis;
				axis += prev;
				axis = (axis - it->center) * rot + it->center;
				axis -= m_pos;
			}
		}
	}
}

// fxspray.cpp:1532
// Revolute for the grand revolution keys (m_grandRevols): also turns the
// nozzle rotation axes and the revolution keys themselves.
void CFxSpray::RevoluteGrand(float fTime, float fScale) {
	for (std::list<sRevol>::iterator it = m_param.m_grandRevols.begin(); it != m_param.m_grandRevols.end(); ++it) {
		if (fTime >= it->t0 && fTime <= it->t1) {
			float speed = (it->t1 - it->t0 > 0) ? (fTime - it->t0) * (it->speed1 - it->speed0) / (it->t1 - it->t0) + it->speed0 : it->speed0;

			WMatrix rot = RotMat(it->axis, speed * fScale);

			WVector prev = m_pos;
			m_pos = (m_pos - it->center) * rot + it->center;

			m_spawnDir += prev;
			m_spawnDir = (m_spawnDir - it->center) * rot + it->center;
			m_spawnDir -= m_pos;

			for (std::list<sNozzleRot>::iterator r = m_param.m_nozzleRots.begin(); r != m_param.m_nozzleRots.end(); ++r) {
				WVector& axis = r->axis;
				axis += prev;
				axis = (axis - it->center) * rot + it->center;
				axis -= m_pos;
			}

			for (std::list<sRevol>::iterator v = m_param.m_revols.begin(); v != m_param.m_revols.end(); ++v) {
				WVector& center = v->center;
				v->axis += center;
				v->axis = (v->axis - it->center) * rot + it->center;
				center = (center - it->center) * rot + it->center;
				v->axis -= center;
			}
		}
	}
}

// fxspray.cpp:1576
// Emits the particles of the generation keys due at fTime, their speed
// from the last active speed key; false when no key is active.
bool CFxSpray::Generate(float fTime) {
	float fSpeed = 100.0f;
	for (std::list<sVel>::iterator vel = m_param.m_vels.begin(); vel != m_param.m_vels.end(); ++vel) {
		const sVel& v = *vel;
		if (fTime >= (float)v.t0 && (float)v.t1 >= fTime) {
			if (v.t1 - v.t0 > 0)
				fSpeed = (v.v1 - v.v0) * (fTime - (float)v.t0) / (v.t1 - v.t0) + v.v0;
			else
				fSpeed = v.v0;
		}
	}

	bool bGenerated = false;
	Waabb area;
	if (m_param.m_bScaleArea == 0)
		area = m_param.m_area;
	else
		ScaleArea(area, m_param.m_area, m_param.m_bRoundArea, m_param.m_fAreaScaleXZ, m_param.m_fAreaScaleY);

	for (std::list<sGen>::iterator gen = m_param.m_gens.begin(); gen != m_param.m_gens.end(); ++gen) {
		if (!(fTime <= (float)gen->t1) && gen->generated >= gen->count)
			continue;

		bGenerated = true;
		if (fTime < (float)gen->t0) {
			gen->lastTime = fTime;
			continue;
		}

		int n;
		if (fTime < (float)gen->t1 && gen->t1 - gen->t0 > 0)
			n = (int)((fTime - (float)gen->t0) * gen->count / (gen->t1 - gen->t0) + 0.0001f) - gen->generated + 1;
		else
			n = gen->count - gen->generated;

		if (n > 0)
			gen->lastTime = fTime;

		if (gen->generated + n > gen->count)
			n = gen->count - gen->generated;

		for (int i = 0; i < n; i++) {
			CFxParticle* particle = new CFxParticle;
			InitParticle(false, particle, fSpeed, gen->spread, area, false);
			m_particles.push_back(particle);
			gen->generated++;
		}
	}
	return bGenerated;
}

// fxspray.cpp:1653
// Starts a particle: its puppet (a "source" pet script), its place in the
// generation area (kept when bSpawn), its velocity (the initial velocity
// range scaled by fSpeed percent, spread by fSpread), its wind, rotation,
// size, life, frame, colour and tail buffers. bSecond (a regenerated
// particle) moves it back to the area's edge the gravity or wind comes from.
bool CFxSpray::InitParticle(bool bSpawn, CFxParticle* pParticle, float fSpeed, float fSpread, Waabb& area, bool bSecond) {
	if (m_param.m_sourceName[0] && FXBOX_RESOURCE_MANAGER(m_box)) {
		if (pParticle->m_pPetBody) {
			delete pParticle->m_pPetBody;
			pParticle->m_pPetBody = 0;
		}
		pParticle->m_pPetBody = new CPetBody(m_param.m_sourceName, true);

		if (pParticle->m_pPetBody) {
			if (pParticle->m_pPetBody->GetPet()) {
				w_motion_data* motion = pParticle->m_pPetBody->GetPet()->GetFirstMotionData();
				if (motion)
					pParticle->m_pPetBody->SetMotion(motion->name, false, 0.0f, false);
			}
			pParticle->m_pPetBody->ActiveGlobalLight(m_param.m_bPetLight);
			WPuppet* pet = pParticle->m_pPetBody->GetPet();
			if (pet)
				pet->CalcBound(0, true);
		}
	}

	pParticle->m_bDead = false;
	pParticle->m_pSprite = m_param.m_pSprite;

	if (!bSpawn) {
		if (!m_param.m_bRoundArea) {
			pParticle->m_basePos = m_box.Random(m_param.m_area.min, m_param.m_area.max);
		}
		else if (m_param.m_bSphereKind2) {
			// A ring about the x axis: radius max.x, length max.z, the
			// particles spread evenly over all the generation keys.
			float angle = m_fSphereAngle;
			float radius = m_param.m_area.max.x;
			int total = 0;
			for (std::list<sGen>::iterator gen = m_param.m_gens.begin(); gen != m_param.m_gens.end(); ++gen)
				total += gen->count;

			pParticle->m_basePos.x = m_box.Random(0.0f, m_param.m_area.max.z) - m_param.m_area.max.z * 0.5f;
			pParticle->m_basePos.y = sinf(angle * g_DEGTORAD) * radius;
			pParticle->m_basePos.z = cosf(angle * g_DEGTORAD) * radius;

			m_fSphereAngle += 360.0f / total;
		}
		else {
			pParticle->m_basePos = m_box.Random(m_param.m_area.min, m_param.m_area.max.x, m_param.m_area.max.y, m_param.m_area.max.z);
		}
	}

	pParticle->m_bubbleOffset.Reset();
	pParticle->m_pos = GetBgPos();
	pParticle->m_rot = m_matrix;

	pParticle->m_bGrounded = false;
	WVector vel = m_box.Random(m_param.m_initialVel.min, m_param.m_initialVel.max);
	vel *= fSpeed * 0.01f;
	pParticle->m_dir = vel;

	WVector axis = (vel.x == 0.0f && vel.z == 0.0f) ? WVector::UNIT_POS_Z : WCrossProduct(vel, WVector::UNIT_POS_Y);

	pParticle->m_dir = pParticle->m_dir * RotMat(axis - pParticle->m_basePos, m_box.Random(0.0f, fSpread));
	pParticle->m_dir = pParticle->m_dir * RotMat(vel, m_box.Random(0.0f, g_2_PI));
	pParticle->m_dir = pParticle->m_dir * m_baseMatrix;
	pParticle->m_fWindScale = m_box.Random(m_param.m_fWindScaleA, m_param.m_fWindScaleB);
	pParticle->m_offset = WVector(0.0f, 0.0f, 0.0f);

	WMatrix bone;
	GetBoneMatrix(&bone);
	WMatrix rot;
	bone.GetRotMatrix(&rot);

	// Where the particle is pushed (wind and velocity), in the bone's frame.
	WVector push;
	if (m_param.m_extOrientation) {
		if (m_param.m_extOrientation == 7) {
			push = pParticle->m_dir + pParticle->m_fWindScale * m_param.m_wind * ~rot;
			pParticle->m_dir = pParticle->m_dir * rot;
		}
		else {
			WVector angles;
			WVector up;
			WVector v;
			angles.x = angles.y = angles.z = 0.0f;

			if (m_param.m_extOrientation & 1) {
				up = WVector::UNIT_POS_Y;
				v = up * rot;
				v.x = 0.0f;
				angles.x = GetAngle(up, v);
				if (!(WAbsBySign(v.z) < WMATH_EPSILON))
					angles.x *= v.z < 0.0f ? -1.0f : 1.0f;
			}

			if (m_param.m_extOrientation & 2) {
				up = WVector::UNIT_POS_Z;
				v = up * rot;
				v.y = 0.0f;
				angles.y = GetAngle(up, v);
				if (!(WAbsBySign(v.x) < WMATH_EPSILON))
					angles.y *= v.x < 0.0f ? -1.0f : 1.0f;
			}

			if (m_param.m_extOrientation & 4) {
				up = WVector::UNIT_POS_X;
				v = up * rot;
				v.z = 0.0f;
				angles.z = GetAngle(up, v);
				if (!(WAbsBySign(v.y) < WMATH_EPSILON))
					angles.z *= v.y > 0.0f ? -1.0f : 1.0f;
			}

			WMatrix orient;
			orient = ~RotMat(angles);
			push = (pParticle->m_fWindScale * m_param.m_wind + pParticle->m_dir * orient) * ~rot;
			pParticle->m_dir = pParticle->m_dir * orient;
		}
	}
	else {
		push = (pParticle->m_fWindScale * m_param.m_wind + pParticle->m_dir) * ~rot;
	}

	if (bSecond
		&& !(WAbsBySign(push.x) < WMATH_EPSILON && WAbsBySign(push.y) < WMATH_EPSILON && WAbsBySign(push.z) < WMATH_EPSILON
			&& WAbsBySign(m_param.m_gravity.x) < WMATH_EPSILON
			&& WisZero(m_param.m_gravity.y, WMATH_EPSILON) && WisZero(m_param.m_gravity.z, WMATH_EPSILON))) {
		if (m_param.m_bRoundArea) {
			// Back to the outer shell of the round area.
			WVector n = pParticle->m_basePos - m_param.m_area.min;
			n.Normalize();
			pParticle->m_basePos = n * m_param.m_area.max.y * -0.999f + m_param.m_area.min;
		}
		else {
			WVector g;
			if (WAbsBySign(push.x) < WMATH_EPSILON && WAbsBySign(push.y) < WMATH_EPSILON && WAbsBySign(push.z) < WMATH_EPSILON)
				g = m_param.m_gravity;
			else
				g = push;

			const WVector& lo = m_param.m_area.min;
			const WVector& hi = m_param.m_area.max;

			// The faces of the area, each with a point on it; the particle
			// goes back along g onto the faces g points into.
			WPlane planes[6] = {
				WPlane(0.0f, 0.0f, 1.0f, -lo.z), WPlane(0.0f, 0.0f, -1.0f, hi.z),
				WPlane(0.0f, 1.0f, 0.0f, -lo.y), WPlane(0.0f, -1.0f, 0.0f, hi.y),
				WPlane(1.0f, 0.0f, 0.0f, -lo.x), WPlane(-1.0f, 0.0f, 0.0f, hi.x)
			};
			WVector points[6] = {
				WVector(lo.x, lo.y, lo.z), WVector(hi.x, lo.y, hi.z),
				WVector(hi.x, lo.y, hi.z), WVector(lo.x, hi.y, lo.z),
				WVector(lo.x, lo.y, hi.z), WVector(hi.x, lo.y, lo.z)
			};

			bool bFirst = true;
			int faces = 0;
			if (g.x > 0.0001f)
				faces = 0x10;
			else if (g.x < -0.0001f)
				faces = 0x20;
			if (g.z > 0.0001f)
				faces |= 1;
			else if (g.z < -0.0001f)
				faces |= 2;
			if (g.y > 0.0001f)
				faces |= 4;
			else if (g.y < -0.0001f)
				faces |= 8;

			for (int i = 0; i < 6; i++) {
				if (!(faces & (1 << i)))
					continue;
				if (!bFirst && (pParticle->m_basePos - points[i]) * planes[i].normal > 0.0f)
					continue;

				float dist = planes[i] * pParticle->m_basePos;
				pParticle->m_basePos = g * dist * (1.0f / (planes[i].normal * g)) * -1.0f + pParticle->m_basePos;
				bFirst = false;
			}
		}
	}

	pParticle->m_rotSpeed = m_box.Random(m_param.m_initRotation.min, m_param.m_initRotation.max) * m_baseMatrix;
	pParticle->m_rotation = m_box.Random(m_param.m_initAngle.min, m_param.m_initAngle.max) * m_baseMatrix;

	pParticle->m_fBaseSize = pParticle->m_fSize = m_box.Random(m_param.m_initSize[0], m_param.m_initSize[1]);
	pParticle->m_fSizeParam = m_param.m_initSize[2];

	pParticle->m_color.value = 0xFFFFFFFF;

	for (std::list<sFade>::iterator fade = m_param.m_fades.begin(); fade != m_param.m_fades.end(); ++fade) {
		if (fade->t0 == 0.0f) {
			pParticle->m_color.bytes[3] = fade->start[0];
			pParticle->m_color.bytes[2] = fade->start[1];
			pParticle->m_color.bytes[1] = fade->start[2];
			pParticle->m_color.bytes[0] = fade->start[3];
			break;
		}
	}

	pParticle->m_fScale = GetVolumeFactor(area, pParticle->m_basePos, m_param.m_area, m_param.m_fAreaScaleXZ, m_param.m_fAreaScaleY, m_param.m_bRoundArea);

	pParticle->m_fLifeTime = m_box.Random((float)m_param.m_bScaleArea, (float)m_param.m_iLifetimeMax);
	pParticle->m_fLifeLeft = pParticle->m_fLifeTime;
	pParticle->m_frame = (int)m_box.Random((float)m_param.m_initFrame[0], (float)m_param.m_initFrame[1]);
	pParticle->m_fFrameTime = 0.0f;
	pParticle->m_fTailTime = 0.0f;

	if (m_param.m_bTail > 0) {
		pParticle->m_tails.Init(m_param.m_bTail + 1);
		pParticle->m_bandanas.Init(m_param.m_bTail + 1);
	}

	pParticle->m_spawnCount = 0;
	pParticle->m_fSpawnTime = m_fTime;
	return true;
}

// fxspray.cpp:1948
// The spray's position in the world: m_pos, carried by the sequence that
// opened it (turned by m_matrix first when m_bTransformPos), unless another
// spray's particle spawned it.
WVector CFxSpray::GetBgPos(void) {
	if (m_pSpawner)
		return m_pos;

	if (m_pParent) {
		if (m_param.m_bTransformPos)
			return m_pos * m_matrix + m_pParent->Pos();
		return m_pos + m_pParent->Pos();
	}
	return m_pos;
}

// fxspray.cpp:1970
// Adds a point to the particle's tail every m_fTailInterval: a bandana
// segment (two edges across the particle's direction) when the spray is
// textured, a plain tail point otherwise.
void CFxSpray::MakeTail(float fTime, CFxParticle* pParticle, const WVector& pos) {
	if (m_param.m_bTail && !(fTime - pParticle->m_fTailTime < m_param.m_fTailInterval)) {

		pParticle->m_fTailTime = fTime;

		if (m_param.m_hTexture) {

			CFxParticle::sBandana* pCur = pParticle->m_bandanas.GetTail(0);
			CFxParticle::sBandana* pPrev = pParticle->m_bandanas.GetTail(1);

			WVector side;
			if (WVectorLen(pParticle->m_dir) < 0.0001f && pParticle->m_bandanas.GetHead(0) != pCur) {
				side = (pPrev->a - pPrev->b) / 2.0f;
			} else {
				if (FXBOX_CAMERA(m_box) && m_param.m_bBandanaFaceCamera) {
					side = WCrossProduct(FXBOX_CAMERA(m_box)->za, pParticle->m_dir);
				} else {
					side = WCrossProduct(WVector::UNIT_POS_Y, pParticle->m_dir);
				}
				side = side.Normalize() * pParticle->m_fSize * m_param.m_fBandanaWidth * 0.005f;
			}

			pCur->a = pos + side;
			pCur->b = pos - side;

			if (pParticle->m_bandanas.GetHead(0) == pCur) {
				pCur->fLen = 0;
			} else {
				if (WDotProduct(pCur->a - pCur->b, pPrev->a - pPrev->b) < 0.0f) {
					WVector tmp = pCur->a;
					pCur->a = pCur->b;
					pCur->b = tmp;
				}
				WVector d = pos - (pPrev->a + pPrev->b) * 0.5f;
				pCur->fLen = sqrtf(WDotProduct(d, d));
			}

			if (pParticle->m_bandanas.IsFull())
				pParticle->m_bandanas.StepHead(1);

			pParticle->m_bandanas.StepTail(1);
		} else {

			CFxParticle::sTail* pTail = pParticle->m_tails.GetTail(0);

			pTail->pos = pos;
			pTail->value = pParticle->m_frame;

			if (pParticle->m_tails.IsFull())
				pParticle->m_tails.StepHead(1);

			pParticle->m_tails.StepTail(1);
		}
	}
}

// fxspray.cpp:2037
// Draws the particle itself: its sprite (a billboard, or turned by its
// rotation; pulled towards the camera by the spray's camoffset), its puppet,
// or a short line along its direction.
void CFxSpray::DrawSprite(WView* pView, CFxParticle* pParticle, const WVector& pos) {
	if (pParticle->m_pSprite) {
		unsigned long color = GetParticleColor(pView, pos, pParticle, pParticle->m_color.value);
		if (!(color & 0xFF000000))
			return;

		pParticle->m_pSprite->SetColor(color);
		float width = pParticle->m_fSizeParam * pParticle->m_fSize;
		pParticle->m_pSprite->SetRect(width, pParticle->m_fSize, width * 0.5f, pParticle->m_fSize * 0.5f);

		if (m_param.m_fCamOffset != 0.0f) {
			const WVector& eye = pView->camera.pivot;
			WVector dir(pos.x - eye.x, pos.y - eye.y, pos.z - eye.z);
			float distSq = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;
			dir.Normalize();
			float facing = dir.x * pView->camera.za.x + dir.y * pView->camera.za.y + dir.z * pView->camera.za.z;

			if (facing <= 0.0f)
				return;

			float nearDist = pView->GetClipNearValue() / facing;
			WVector drawPos;
			if (sqrtf(distSq) < nearDist + m_param.m_fCamOffset) {
				drawPos = dir * (nearDist + 0.01f) + eye;
			} else {
				drawPos = pos - dir * m_param.m_fCamOffset;
			}
			pParticle->m_pSprite->SetPos(drawPos);
		} else {
			pParticle->m_pSprite->SetPos(pos);
		}

		if (m_param.m_bBillboard) {
			pParticle->m_pSprite->Render(pView, pParticle->m_rotation.x,
				m_box.m_value114 | m_param.m_srcBlend | 0x20400000, pParticle->m_frame);
		} else {
			pParticle->m_pSprite->Render(pView, RotMat(pParticle->m_rotation) * GetBgRot(pParticle),
				m_box.m_value114 | m_param.m_srcBlend | 0x20000000, pParticle->m_frame);
		}
	} else if (pParticle->m_pPetBody) {
		pParticle->m_pPetBody->Display();
	} else {
		unsigned long color = GetParticleColor(pView, pos, pParticle, pParticle->m_color.value);
		if (!(color & 0xFF000000))
			return;

		WVector dir = pParticle->m_dir;
		dir = dir.Normalize() * 0.03f;
		pView->DrawLine(pos, color, dir + pos, color, 0);
	}
}

// fxspray.cpp:2101
// Draws the particle's tail: its points' sprites, or lines from point to
// point, the colour fading from the particle's to the tail colour.
void CFxSpray::DrawTail(WView* pView, CFxParticle* pParticle, const WVector& pos) {
	if (pParticle->m_tails.IsEmpty())
		return;

	m_displayedCount2++;

	FxColor prevColor;
	prevColor.value = 0xFFFFFFFF;
	int len = pParticle->m_tails.GetLen();
	CFxParticle::sTail* pLast = pParticle->m_tails.GetTail(1);

	if (pParticle->m_pSprite) {
		FxColor color;
		color.bytes[0] = (unsigned char)(pParticle->m_color.bytes[0] * (len - 1) / len + m_param.m_tailBytes[3] / len);
		color.bytes[1] = (unsigned char)(pParticle->m_color.bytes[1] * (len - 1) / len + m_param.m_tailBytes[2] / len);
		color.bytes[2] = (unsigned char)(pParticle->m_color.bytes[2] * (len - 1) / len + m_param.m_tailBytes[1] / len);
		color.bytes[3] = (unsigned char)(pParticle->m_color.bytes[3] * (len - 1) / len + m_param.m_tailBytes[0] / len);
		unsigned long c = GetParticleColor(pView, pLast->pos, pParticle, color.value);
		if (c & 0xFF000000) {
			pParticle->m_pSprite->SetColor(c);
			pParticle->m_pSprite->SetPos(pLast->pos);
			pParticle->m_pSprite->Render(pView, pParticle->m_rotation.x,
				m_box.m_value114 | m_param.m_dstBlend | 0x400000, pLast->value);
		}
	} else {
		prevColor.bytes[0] = (unsigned char)(pParticle->m_color.bytes[0] * (len - 1) / len + m_param.m_tailBytes[3] / len);
		prevColor.bytes[1] = (unsigned char)(pParticle->m_color.bytes[1] * (len - 1) / len + m_param.m_tailBytes[2] / len);
		prevColor.bytes[2] = (unsigned char)(pParticle->m_color.bytes[2] * (len - 1) / len + m_param.m_tailBytes[1] / len);
		prevColor.bytes[3] = (unsigned char)(pParticle->m_color.bytes[3] * (len - 1) / len + m_param.m_tailBytes[0] / len);
		pView->DrawLine(pLast->pos, GetParticleColor(pView, pLast->pos, pParticle, prevColor.value),
			pos, GetParticleColor(pView, pLast->pos, pParticle, pParticle->m_color.value), 0);
	}

	for (int i = len - 2; i >= 0; i--) {
		m_displayedCount2++;

		CFxParticle::sTail* pCur = pParticle->m_tails.GetHead(i);
		CFxParticle::sTail* pNext = pParticle->m_tails.GetHead(i + 1);

		FxColor color;
		color.bytes[0] = (unsigned char)(pParticle->m_color.bytes[0] * i / len + m_param.m_tailBytes[3] * (len - i) / len);
		color.bytes[1] = (unsigned char)(pParticle->m_color.bytes[1] * i / len + m_param.m_tailBytes[2] * (len - i) / len);
		color.bytes[2] = (unsigned char)(pParticle->m_color.bytes[2] * i / len + m_param.m_tailBytes[1] * (len - i) / len);
		color.bytes[3] = (unsigned char)(m_param.m_tailBytes[0] * (len - i) / len + pParticle->m_color.bytes[3] * i / len);

		if (pParticle->m_pSprite) {
			unsigned long c = GetParticleColor(pView, pCur->pos, pParticle, color.value);
			if (!(c & 0xFF000000))
				continue;

			pParticle->m_pSprite->SetColor(c);
			pParticle->m_pSprite->SetPos(pCur->pos);
			pParticle->m_pSprite->Render(pView, pParticle->m_rotation.x,
				m_box.m_value114 | m_param.m_dstBlend | 0x400000, pCur->value);
		} else {
			pView->DrawLine(pCur->pos, color.value, pNext->pos, prevColor.value, 0);
		}

		prevColor.value = color.value;
	}
}

// fxspray.cpp:2176
// Draws the particle's bandana: a textured strip from the particle's
// position through its bandana segments, one quad per segment, its u along
// the strip's length (with "monolith") and its colour fading to the tail
// colour.
void CFxSpray::DrawBandana(WView* pView, CFxParticle* pParticle, const WVector& pos) {
	if (pParticle->m_bandanas.IsEmpty())
		return;

	float total = 0.0f;
	int len = pParticle->m_bandanas.GetLen();
	for (int n = 1; n < len; n++) {
		CFxParticle::sBandana* pSeg = pParticle->m_bandanas.GetHead(n);
		if (pSeg)
			total += pSeg->fLen;
	}

	unsigned long prevColor = pParticle->m_color.value;

	m_displayedCount2++;

	CFxParticle::sBandana* pLast = pParticle->m_bandanas.GetTail(1);
	float dist = (pos - (pLast->a + pLast->b) * 0.5f).Magnitude();
	total += dist;

	// The particle's own edge, across it like the last segment.
	CFxParticle::sBandana edge;
	edge.a = pos + (pLast->a - pLast->b) * 0.5f;
	edge.b = pos - (pLast->a - pLast->b) * 0.5f;

	FxColor color;
	color.bytes[0] = (unsigned char)(pParticle->m_color.bytes[0] * (len - 1) / len + m_param.m_tailBytes[3] / len);
	color.bytes[1] = (unsigned char)(pParticle->m_color.bytes[1] * (len - 1) / len + m_param.m_tailBytes[2] / len);
	color.bytes[2] = (unsigned char)(pParticle->m_color.bytes[2] * (len - 1) / len + m_param.m_tailBytes[1] / len);
	color.bytes[3] = (unsigned char)(pParticle->m_color.bytes[3] < pParticle->m_color.bytes[3] * (len - 1) / len + m_param.m_tailBytes[0] / len
		? pParticle->m_color.bytes[3] : pParticle->m_color.bytes[3] * (len - 1) / len + m_param.m_tailBytes[0] / len);

	// The quad from the particle's position to the last segment.
	int k;
	CFxParticle::sBandana* pSeg;
	unsigned long c;
	for (k = 0; k < 4; k++) {
		if (k & 2) {
			pSeg = pLast;
			m_box.m_corners[k].tu = (m_param.m_bMonolith && total != 0.0f) ? dist / total : 1.0f;
			c = GetParticleColor(pView, (k & 1) ? pSeg->a : pSeg->b, pParticle, color.value);
		} else {
			pSeg = &edge;
			m_box.m_corners[k].tu = 0.0f;
			c = GetParticleColor(pView, (k & 1) ? pSeg->a : pSeg->b, pParticle, pParticle->m_color.value);
		}

		m_box.m_corners[k].diffuse = c;
		m_box.m_corners[k].SetPosition((k & 1) ? pSeg->a : pSeg->b);
	}

	if ((m_box.m_corners[0].diffuse & 0xFF000000) || (m_box.m_corners[1].diffuse & 0xFF000000)
		|| (m_box.m_corners[2].diffuse & 0xFF000000) || (m_box.m_corners[3].diffuse & 0xFF000000)) {
		pView->DrawPolygonFan(m_box.m_pCorners,
			(m_param.m_hTexture & 0x7FF) | m_box.m_value114 | m_param.m_dstBlend | 0x400000, 0x400, false);
	}

	// One quad per pair of segments, from the newest back to the oldest.
	for (int i = len - 2; i >= 0; i--) {
		m_displayedCount2++;

		CFxParticle::sBandana* pCur = pParticle->m_bandanas.GetHead(i);
		CFxParticle::sBandana* pNext = pParticle->m_bandanas.GetHead(i + 1);

		color.bytes[0] = (unsigned char)(pParticle->m_color.bytes[0] * i / len + m_param.m_tailBytes[3] * (len - i) / len);
		color.bytes[1] = (unsigned char)(pParticle->m_color.bytes[1] * i / len + m_param.m_tailBytes[2] * (len - i) / len);
		color.bytes[2] = (unsigned char)(pParticle->m_color.bytes[2] * i / len + m_param.m_tailBytes[1] * (len - i) / len);
		color.bytes[3] = (unsigned char)(pParticle->m_color.bytes[3] < pParticle->m_color.bytes[3] * i / len + m_param.m_tailBytes[0] * (len - i) / len
			? pParticle->m_color.bytes[3] : pParticle->m_color.bytes[3] * i / len + m_param.m_tailBytes[0] * (len - i) / len);

		for (k = 0; k < 4; k++) {
			if (k & 2) {
				pSeg = pCur;
				m_box.m_corners[k].tu = (m_param.m_bMonolith && total != 0.0f) ? (dist + pNext->fLen) / total : 1.0f;
				c = GetParticleColor(pView, (k & 1) ? pSeg->a : pSeg->b, pParticle, color.value);
			} else {
				pSeg = pNext;
				m_box.m_corners[k].tu = (m_param.m_bMonolith && total != 0.0f) ? dist / total : 0.0f;
				c = GetParticleColor(pView, (k & 1) ? pSeg->a : pSeg->b, pParticle, prevColor);
			}

			m_box.m_corners[k].diffuse = c;
			m_box.m_corners[k].SetPosition((k & 1) ? pSeg->a : pSeg->b);
		}

		if ((m_box.m_corners[0].diffuse & 0xFF000000) || (m_box.m_corners[1].diffuse & 0xFF000000)
			|| (m_box.m_corners[2].diffuse & 0xFF000000) || (m_box.m_corners[3].diffuse & 0xFF000000)) {
			pView->DrawPolygonFan(m_box.m_pCorners,
				(m_param.m_hTexture & 0x7FF) | m_box.m_value114 | m_param.m_dstBlend | 0x400000, 0x400, false);
		}

		prevColor = color.value;
		dist += pNext->fLen;
	}
}

// fxspray.cpp:2310
unsigned long CFxSpray::GetParticleColor(const WView* view, const WVector& pos, const CFxParticle* particle, unsigned long color) {
	if (particle == 0) {
		return 0xffffffff;
	}

	float fade;
	if (m_param.m_fDistLimitB > 0.0f) {
		float dist = (view->camera.pivot - pos).Magnitude();
		if (!(dist < m_param.m_fDistLimitA)) {
			if (dist > m_param.m_fDistLimitB)
				return 0;
			fade = 1.0f - Between(0.0f, (dist - m_param.m_fDistLimitA) / (m_param.m_fDistLimitB - m_param.m_fDistLimitA), 1.0f);
		} else {
			fade = 1.0f;
		}
	} else {
		fade = 1.0f;
	}

	unsigned long result = color;
	if (m_param.m_lightSource != 0)
		result = ModulateColor(color, m_param.m_lightColor) | (color & 0xff000000);

	return ScaleAlpha(result, m_param.m_fAlphaScale * particle->m_fScale * fade);
}

// fxspray.cpp:2346
void CFxSpray::SetLight(const LightSet& light) {
	switch (m_param.m_lightSource) {
	case 0:
		m_param.m_lightColor = 0xffffff;
		break;
	case 1:
		m_param.m_lightColor = light.diffuse;
		break;
	case 2:
		m_param.m_lightColor = light.ambient;
		break;
	case 3:
		m_param.m_lightColor = light.ambient2;
		break;
	}
}

// fxspray.cpp:2369
void CFxSpray::SetWind(const WVector& wind) {
	if (m_param.m_fWindScaleA != 0.0f || m_param.m_fWindScaleB != 0.0f)
		m_param.m_wind = wind;
}

// fxspray.cpp:2376
void CFxSpray::SetDistLimit(float limitA, float limitB) {
	m_param.m_fDistLimitA = limitA;
	m_param.m_fDistLimitB = limitB;
}

// fxspray.cpp:2383
void CFxSpray::Spawn(const sSpawn& spawn, CFxParticle* pParticle) {
	pParticle->m_spawnCount++;
	CFxSpray* pSpray = m_box.LoadSpray(spawn.m_name, 0);
	if (pSpray) {
		pSpray->m_param.m_bFollowSpawner = spawn.m_bFollow;
		pSpray->m_pSpawner = pSpray->m_param.m_bFollowSpawner ? this : 0;

		pSpray->m_pParent = m_pParent;
		pSpray->m_fAttachLimitA = m_fAttachLimitA;
		pSpray->m_fAttachLimitB = m_fAttachLimitB;

		if (m_pParent) {
			pSpray->SetWind(m_pParent->m_extWind);
			m_pParent->m_sprays[pSpray] = pSpray;
		}

		pSpray->m_pos = pParticle->m_basePos + pParticle->m_pos;
		pSpray->m_spawnDir = pParticle->m_dir;
		pSpray->m_bActive = m_bActive;
		m_box.AddSpray(pSpray);
	}
}
