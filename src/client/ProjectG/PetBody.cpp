// Original: d:\build\custom\temp\code\client\projectg\petbody.cpp
#include "petbody.h"

#include <windows.h>
#include <math.h>
#include <string.h>
#include <algorithm>

#include "wbone.h"
#include "wview.h"
#include "wpuppet.h"
#include "wresrcmng.h"
#include "texturepart.h"
#include "frwnd.h"
#include "GolfTask.h"
#include "TaskManager.h"
#include "commonutil.h"
#include "quadtree.h"
#include "slicestr.h"
#include "Shadow.h"
#include "wutil.h"
#include "golfball.h"
#include "fx.h"
#include "actor.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "SoundManager.h"

extern LightSet g_lightset;
extern WMatrix g_camera;

// The by-value min/max SetMotion inlines (the argument is materialized in a
// stack slot). wutil.h's Min/Max compare with <=/>= and give jp/jnp where the
// original has je/jnp; the original helper's header is not identified yet.
template <class T> T MinOf(T a, T b) { return a > b ? b : a; }
template <class T> T MaxOf(T a, T b) { return b < a ? a : b; }

// wmath.h:86 and wmath.inl:328, emitted out of line in other units
// (ClientSetting, GolfRule); not transcribed into the Wangreal headers yet.
int WisZero(const float &f, float eps);
WVector RotVec(const WVector &v, const WMatrix &m);

// A nameless WMatrix in .bss that GetMatrix puts between the look rotation
// and the head bone.
extern WMatrix data_a7fb50;

// commonutil.cpp
const char *GetStrConst(const char *pSrc, char *pDest);

// This unit's .rdata copies of wmath.h's g_DEGTORAD, g_HUGE and g_EPSILON
// ([0], [1], [2]). The split names the block _data_9ed708, so a reference to
// the constants themselves (or to an equal literal) does not pair; same
// stand-in as golfcadie.cpp's data_9eee9c.
extern "C" const float data_9ed708[3];

// The commands ParsePuppetScript recognizes, by switch index (.data
// 0xa7fb30). *lookup and *dontlookup have no case.
static char *s_scriptCommands[8] = {
	"*ptex(", "*snd(", "*fly", "*hide", "*show", "*fx(", "*lookup", "*dontlookup"
};

// petbody.cpp:42
CPetBody::CPetBody(const char *script, bool arg) : m_scriptHandles(8, 0) {
	Init();
	InitScript(script, arg);
}

// petbody.cpp:48
void CPetBody::Init(void) {
	m_pMotion = NULL;
	m_pPet = NULL;
	m_lookTarget = NULL;
	m_motionTime = 0;
	m_scale = 1.0f;
	m_aniWeight = 1.0f;
	m_aniSpeed = 1.0f;
	m_flightState = 2;
	m_bFlying = false;
	m_nextMotion[0] = '\0';
	m_customGround = "";

	m_rotation.Reset();
	m_pos.Reset();
	m_moveDelta.Reset();
	m_rotMatrix.Reset();
	m_matrix.Reset();

	m_light.type = LightSet::W_LIGHT_DIRECTIONAL; m_bUseLight = m_bUseLocalLights = true;

	m_lookAngle.Reset();
	m_lookAngleTarget.Reset();
}

// petbody.cpp:81
CPetBody::~CPetBody(void) {
	for (ScriptHandle *handle = m_scriptHandles.Start(); handle; handle = m_scriptHandles.Next()) delete handle; m_scriptHandles.Reset();


	// The pending texture parts.
	for (std::list<sTexturePart *>::iterator it = m_pendingTexParts.begin(); it != m_pendingTexParts.end(); ++it)
		delete *it;

	m_pendingTexParts.clear();

	if (m_pPet) {

		g_resrcmng->Release(m_pPet, false);
		m_pPet = NULL;
	}

	PG_VECTOR_FREE_STORAGE(m_localLights);
}

// petbody.cpp:101
void CPetBody::InitScript(const char *script, bool arg) {
	WSliceStr slice(script);
	char puppetName[256];
	int shadowType = 0;

	if (slice.Get("*pet", "%s", puppetName))
		LoadPuppet(puppetName, false, arg);

	if (slice.Get("*shadow", "%d", &shadowType))
		SetShadowType(shadowType);
}

// petbody.cpp:114
void CPetBody::Reset(void) {
	m_flags.Reset();
	m_lookTarget = NULL;
	m_lookAngleTarget.Reset();
	m_lookAngle.Reset();

	m_aniWeight = 1.0f;
	m_aniSpeed = 1.0f;
	m_nextMotion[0] = '\0';
}

// petbody.cpp:126
void CPetBody::SetCustomGround(char *groundName) {
	m_customGround = groundName;
}

// petbody.cpp:131
void CPetBody::Display(void) {
	if (ShouldDisplay()) {

		if (m_flags.GetFlag(0x14)) {

			m_pPet->ApplyBones(m_pPet->GetBones(NULL, m_motionTime, m_flags.GetFlag(2) ? 1 : 0, NULL, false), m_matrix);
			m_pPet->Transform(NULL, 1.0f, 0);
			m_pPet->UpdateBSphere();
		}























		const WSphere &sphere = m_pPet->GetBoundSphere(); bool inFrustum = g_view->InFrustum(sphere);





		if (inFrustum) {

			if (m_flags.GetFlag(0x34)) {

				if (m_pPet->FindBoneByTail("Head"))
					m_pPet->FindBoneByTail("Head")->SetMatrix(~data_a7fb50 * GetMatrix(BODY_HEAD), NULL);
			}

			if (m_flags.GetFlag(0x14)) {


				if (m_bUseLight)
					m_pPet->UpdateLightSource(&m_light, false, NULL);


				if (m_bUseLocalLights) {


					for (std::vector<sLightSet>::iterator it = m_localLights.begin(); it != m_localLights.end(); ++it) {

						it->light.nearone = RotVec(it->pos, m_matrix);
						m_pPet->UpdateLightSource(&it->light, true, NULL);
					}
				}
			}



			m_pPet->Render(g_view, true, m_scale, g_view->InFrustumSafe(sphere), false, false);
		}



		m_flags.Disable(0x30);
	}
}

// petbody.cpp:203
void CPetBody::DisplayShadow(char *name) {
	if (ShouldDisplay()) {

		if (m_flags.GetFlag(0x14)) {

			m_pPet->ApplyBones(m_pPet->GetBones(NULL, m_motionTime, m_flags.GetFlag(2) ? 1 : 0, NULL, false), m_matrix);
			m_pPet->Transform(NULL, 1.0f, 0);
			m_pPet->UpdateBSphere();
		}

		if (m_flags.GetFlag(0x34)) {

			if (m_pPet->FindBoneByTail("Head"))
				m_pPet->FindBoneByTail("Head")->SetMatrix(~data_a7fb50 * GetMatrix(BODY_HEAD), NULL);
		}


		CShadowManager::Instance()->AddCastPet_New(name, false, m_pPet);
	}
}

// petbody.cpp:224
void CPetBody::SetLight(void) {
	m_pPet->UpdateBound(false, false);
	m_bUseLocalLights = true;
	if ((__rtti_obj = AfxGetTask()) != 0 && ((IObject *)__rtti_obj)->IsKindOf(&CGolfTask::m_RTTI)) {

		m_light.diffuse = ModifyColor(g_lightset.diffuse, GetPVS().GetShadowIntensity(WVector(m_pos.x, m_pPet->GetBoundBox().max.y, m_pos.z)));
		m_light.ambient = g_lightset.ambient;
		m_light.nearone = g_lightset.nearone;
	}
	else {





		m_light.diffuse = 0xffffff;
		m_light.ambient = 0xffffff;

		m_light.type = LightSet::W_LIGHT_DIRECTIONAL; m_light.nearone = WVector(0.0f, -1.0f, 1.0f);
		m_light.nearone.Normalize();
	}
}

// petbody.cpp:246
void CPetBody::Process(float delta) {
	if (m_flags.GetFlag(4)) {

		Animation(delta * m_aniSpeed);

		if (m_flags.GetFlag(2)) {

			if (m_flags.GetFlag(1)) {

				m_flags.Disable(1);
				m_moveDelta = m_aniWeight * RotVec(m_accumDelta, m_rotMatrix);
				m_pos += m_moveDelta;
			}

			FlightProcess(delta);

			m_rotMatrix.pivot = m_pos;
			m_matrix.pivot = m_pos;
		}
	}

	if (m_pPet->FindBoneByTail("Head"))
		LookUpProcess(delta);
}

// petbody.cpp:272
void CPetBody::Animation(float delta) {
	while (m_pMotion && m_pMotion->s != m_pMotion->e) {





		float moved = Move(delta);

		if (!(moved > 0.0f))
			return;

		ScriptProcess(m_pMotion, m_motionTime, m_motionTime + moved);
		m_motionTime += moved;

		if (!(m_motionTime >= m_pMotion->e))
			return;

		if (m_nextMotion[0]) {

			SetMotion(m_nextMotion, m_bNextMotionLoop, delta - moved, false);
			m_nextMotion[0] = 0;
		}
		else {


			SetMotion(m_pMotion->nextmotion, m_flags.GetFlag(2), delta - moved, true);
		}


		delta -= moved;
	}

	m_flags.Disable(6);
}

// petbody.cpp:305
float CPetBody::Move(float delta) {

	float time = MinOf(m_pMotion->end, delta + m_motionTime);

	if (m_flags.GetFlag(2)) {



		if (!m_flags.GetFlag(1)) {
			m_accumDelta = m_pPet->GetDeltaVec(m_pMotion->name, m_motionTime - m_pMotion->s, time - m_pMotion->s);
			m_flags.Enable(1);
		}
		else {

			m_accumDelta += m_pPet->GetDeltaVec(m_pMotion->name, m_motionTime - m_pMotion->s, time - m_pMotion->s);
		}
	}

	return time - m_motionTime;
}

// petbody.cpp:326
void CPetBody::SetPos(const WVector &pos) {
	m_pos = pos;
	m_rotMatrix.pivot = m_pos;
	m_matrix.pivot = m_pos;
	m_flags.Enable(0x10);
	m_flags.Disable(2);
}

// petbody.cpp:335
void CPetBody::SetRotate(const WVector &rotation) {
	m_rotation = rotation;
	m_rotMatrix = RotMat(m_rotation.x, 0) * RotMat(m_rotation.y, 1) * RotMat(m_rotation.z, 2);
	m_rotMatrix.pivot = m_pos;
	m_matrix = m_rotMatrix;

	m_flags.Enable(0x10);
}

// petbody.cpp:345
void CPetBody::SetRotate_Lean(const WVector &rotation) {
	m_rotation = rotation;
	m_matrix = RotMat(m_rotation.x, 0) * RotMat(m_rotation.y, 1) * RotMat(m_rotation.z, 2);
	m_matrix.pivot = m_pos;

	m_flags.Enable(0x10);
}

// petbody.cpp:354
void CPetBody::SetRotate(float angle) {
	WVector pivot = m_rotMatrix.pivot;
	m_rotation = WVector(0.0f, angle, 0.0f);
	m_rotMatrix = RotMat(angle, 1);
	m_rotMatrix.pivot = pivot;
	m_matrix = m_rotMatrix;

	m_flags.Enable(0x10);
}

// petbody.cpp:365
void CPetBody::SetRotateSlope(float angle, WVector normal) {

	WMatrix rot;
	WVector pivot = m_rotMatrix.pivot;
	WVector axis = normal;
	axis.y = 0.0f;

	normal = WCrossProduct(normal, axis);
	normal.y *= -1.0f;

	rot = m_rotMatrix;
	m_rotMatrix = RotMat(normal, angle * 0.0034377468f);
	m_rotMatrix = rot * m_rotMatrix;

	m_rotMatrix.pivot = pivot;
	m_matrix = m_rotMatrix;

	m_flags.Enable(0x10);
}

// petbody.cpp:384
void CPetBody::MultRotate(WMatrix *rot) {
	if (rot) {

		WVector pivot = m_matrix.pivot;

		WMatrix cur;
		m_matrix.GetRotMatrix(&cur);
		m_matrix = cur * *rot;
		m_matrix.pivot = pivot;
		m_flags.Enable(0x10);
	}
}

// petbody.cpp:398
void CPetBody::Rotate(float angle) {
	m_rotation.y += angle;
	m_rotMatrix *= RotMat(angle, 1);
	m_rotMatrix.pivot = m_pos;
	m_matrix = m_rotMatrix;

	m_flags.Enable(0x10);
}

// petbody.cpp:408
void CPetBody::AddScriptHandle(const char *name, IActor *actor, int id) {
	ScriptHandle *handle = new ScriptHandle;


	handle->name = name;
	handle->actor = actor;
	handle->id = id;

	m_scriptHandles += handle;
}

// petbody.cpp:419
void CPetBody::AddLocalLight(const LightSet &light, WVector pos) {
	sLightSet lightSet; lightSet.light = light; lightSet.pos = pos;
	m_localLights.push_back(lightSet);
}

// petbody.cpp:427
void CPetBody::TakeOff(WVector dir) {

	m_flightState = 0; m_takeOffPos = m_pos;
	m_takeOffDir = dir;
	m_takeOffAngle = -0.78539819f;
	m_takeOffHeight = 0.0f;
	m_takeOffSpeed = 50.0f;
}

// petbody.cpp:441
void CPetBody::LoadPuppet(const char *name, bool optimizeBone, bool arg3) {
	m_pPet = g_resrcmng->GetPuppet(name, false, true, arg3);
	if (optimizeBone && m_pPet)
		m_pPet->OptimizeBone();

	SetScale(1.0f);
	SetPivot(NULL, WVector(0.0f, 0.0f, 0.0f));
}

// petbody.cpp:451
void CPetBody::ScriptProcess(const w_motion_data *motion, float prevTime, float time) {


	if (!(prevTime <= motion->e) || prevTime == time)
		return;





	prevTime *= 30.0f;
	time = MinOf(motion->e, time) * 30.0f;
	int start = Ceil(prevTime, 0.03f);
	int end = time == (float)floor(time) ? (int)time - 1 : (int)floor(time);

	if (start <= end) {
		int first, last;
		if (m_pPet->CheckFrameData(start, end, first, last)) {

			for (int i = first; i <= last; i++)
				ParsePuppetScript(m_pPet->GetFrameData(i));
		}
	}
}

// petbody.cpp:477
void CPetBody::ParsePuppetScript(const char *script) {
	static char s_command[64];
	static char s_name[64];

	if (script == NULL) {
		return;
	}

	const char *p = script;
	int len;

	while (p) {
		p = strchr(p, '*');

		if (p == NULL) {
			break;
		}

		int cmdLen = 0;

		while (p[cmdLen] != '\0') {
			if (p[++cmdLen] == ' ') {
				break;
			}
			if (p[cmdLen] == '(') {
				cmdLen++;
				break;
			}
		}

		if (cmdLen <= 1) {
			break;
		}

		strncpy(s_command, p, cmdLen);
		s_command[cmdLen] = '\0';
		p += cmdLen;

		for (unsigned int command = 0; command < sizeof(s_scriptCommands) / sizeof(s_scriptCommands[0]); command++) {
			if (strcmpi(s_command, s_scriptCommands[command]) != 0) {
				continue;
			}

			switch (command) {
			case 0: {   // *ptex(
				char token[64];
				char texture[64];

				token[0] = '\0';
				p = GetStrConst(p, token);

				if ((len = strlen(token)) == 0 || strlen(token) >= sizeof(token)) {
					break;
				}

				if (token[0] == '@') {
					if (m_pMotion == NULL || strcmpi(token + 1, m_pMotion->name) != 0) {
						break;
					}

					p = GetStrConst(p, s_name);

					if ((len = strlen(s_name)) == 0 || strlen(s_name) >= sizeof(s_name)) {
						break;
					}
				}
				else {
					strcpy(s_name, token);
				}

				p = GetStrConst(p, texture);

				if ((len = strlen(texture)) == 0 || strlen(texture) >= sizeof(texture)) {
					break;
				}

				SetPendingTexPart(s_name, texture);
				break;
			}
			case 1:     // *snd(
				p = GetStrConst(p, s_name);

				if ((len = strlen(s_name)) == 0 || strlen(s_name) >= sizeof(s_name)) {
					break;
				}

				PlaySfx(s_name);
				break;
			case 2:     // *fly
				m_bFlying = true;
				break;
			case 3:     // *hide
				m_flags.Enable(0x2000);
				break;
			case 4:     // *show
				m_flags.Disable(0x2000);
				break;
			case 5: {   // *fx(
				char token[64];
				char fxName[64];
				char boneName[64];

				token[0] = '\0';
				p = GetStrConst(p, token);

				if ((len = strlen(token)) == 0 || strlen(token) >= sizeof(token)) {
					break;
				}

				if (token[0] == '@') {
					if (m_pMotion == NULL || strcmpi(token + 1, m_pMotion->name) != 0) {
						break;
					}

					p = GetStrConst(p, fxName);

					if ((len = strlen(fxName)) == 0 || strlen(fxName) >= sizeof(fxName)) {
						break;
					}
				}
				else {
					strcpy(fxName, token);
				}

				Attacher attacher;

				unsigned int i;
				for (i = 0; i < 4; i++) {
					p = GetStrConst(p, boneName);

					if ((len = strlen(boneName)) == 0 || strlen(boneName) >= sizeof(boneName)) {
						break;
					}

					WBone *pBone = m_pPet->FindBone(boneName);
					if (pBone) {
						attacher.m_pMatrix[i] = &pBone->GetMatrix();
					}
					else {
						w_bound_box *pBox = m_pPet->m_bbList.Find(boneName);
						if (pBox) {
							if (pBox->bone) {
								attacher.m_pMatrix[i] = &pBox->bone->GetMatrix();
							}
							attacher.m_pOffset[i] = &pBox->info->spherePivot;
						}
					}
				}

				if ((int)i > 0) {
					WSingleton<CFx>::Instance()->Attach(attacher, fxName, true, NULL);
				}
				break;
			}
			}
			break;
		}
	}

	ScriptHandle *handle = m_scriptHandles.Start();

	while (handle) {
		const char *found = strstr(script, handle->name);

		if (found) {
			MsgObject msg;
			msg.m_msg = handle->id;

			msg.m_pSender = NULL; msg.m_reserved = 0; msg.m_param1 = (intptr_t)found; handle->actor << msg;
		}

		handle = m_scriptHandles.Next();
	}
}

// petbody.cpp:717
sTexturePart *CPetBody::FindTexturePart(char *animName, char *texName) {
	sTexturePart *pTexPart;
	std::list<sTexturePart *>::iterator it;

	it = std::find_if(m_pendingTexParts.begin(), m_pendingTexParts.end(), sFindTexPart(texName));

	if (it != m_pendingTexParts.end()) {
		pTexPart = *it;
	}
	else {
		pTexPart = new sTexturePart;

		strcpy(pTexPart->m_partName, animName);
		strcpy(pTexPart->m_texName, texName);
		m_pendingTexParts.push_back(pTexPart);
	}

	return pTexPart;
}

// petbody.cpp:740
bool CPetBody::ShouldDisplay(void) {
	return !m_flags.GetFlag(0x2000) ? true : false;
}

// petbody.cpp:745
void CPetBody::SetPendingTexPart(char *animName, char *texName) {

	sTexturePart *part = FindTexturePart(animName, texName);

	if (part->m_pBitmap == NULL) {

		m_pPet->ChangeTexturePart(part->m_partName, part->m_texName, &part->m_pBitmap, &part->m_rect);
	} else {

		m_pPet->ChangeTexturePart(part->m_partName, part->m_pBitmap, part->m_rect);
	}
}

// petbody.cpp:760
void CPetBody::PlaySfx(char *name) {
	char buffer[64];
	char *p;
	int len;
	int diff;

	if (name == NULL) {
		return;
	}

	WVector toCamera;
	toCamera = m_pos - g_camera.pivot;
	p = strchr(name, '-');

	if (p != NULL) {

		len = ++p - name;

		// CP949 tag bytes C2 F9 BD BA: the sound has birdie/eagle variants.
		if (strncmp(p, "\xc2\xf9\xbd\xba", 4) == 0) { // "찬스" (chance)

			CSharedDoc *sharedDoc = WSingleton<CSharedDoc>::Instance();

			if (sharedDoc->m_roomType == 1) {
				diff = (unsigned char)TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] - GOLFDOC()->GetHoleData().m_par;
			}
			else {
				diff = (int)PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] - GOLFDOC()->GetHoleData().m_par;
			}

			if (GolfBall().m_groundType == 2 && (diff == -2 || diff == -1)) {

				strncpy(buffer, name, len);

				switch (diff) {
				case -2:
					strcpy(buffer + len, "\xc0\xcc\xb1\xdb"); // "이글" (eagle)
					break;
				case -1:
					strcpy(buffer + len, "\xb9\xf6\xb5\xf0"); // "버디" (birdie)
					break;
				}

				strcat(buffer, name + len);
				strcpy(name, buffer);
			}
		}
	}

	if ((__rtti_obj = AfxGetTask()) != 0 && ((IObject *)__rtti_obj)->IsKindOf(&CGolfTask::m_RTTI)) {
		g_audio->PlaySfx(name, &toCamera, 5, NULL, NULL, 0.5f, 200.0f);
	}
	else {
		g_audio->PlaySfx(name, NULL, 0, NULL, NULL, 0.5f, 200.0f);
	}
}

// petbody.cpp:821
void CPetBody::SetPivot(const char *boneName, WVector offset) {
	if (m_pPet) {

		WVector pivot = m_pPet->GetPivot(boneName);
		pivot += offset;
		pivot *= m_scale;

		m_pos -= pivot;
	}
}

// petbody.cpp:832
void CPetBody::SetShadowType(int type) {
	m_shadowType = type;
}

// petbody.cpp:837
void CPetBody::SetMotion(const char *name, bool loop, float time, bool applyDelta) {




	const w_motion_data *prevMotion = m_pMotion;
	m_pMotion = m_pPet->FindMotionData(name);

	if (m_pMotion) {

		m_bFlying = false;
		m_motionTime = MinOf(time + m_pMotion->s, m_pMotion->e);
		m_motionTime = MaxOf(m_motionTime, 0.0f);




		if (m_motionTime > m_pMotion->s)
			ScriptProcess(m_pMotion, m_pMotion->s, m_motionTime);

		m_flags.Enable(4);

		if (loop) {
			m_flags.Enable(2);
		}
		else {
			if (prevMotion && applyDelta && !WisZero(m_pMotion->e - m_pMotion->s, 1e-5f)) {



				WVector delta;
				delta = m_pPet->GetDeltaVec(prevMotion->name, prevMotion->e - prevMotion->s);
				delta = m_aniWeight * RotVec(delta, m_rotMatrix);
				SetPos(delta + m_pos);
			}

			m_flags.Disable(2);
		}
	}
}

// petbody.cpp:881
void CPetBody::SetScale(float scale) {
	m_scale = scale;
	{ WVector pg_scale(scale, scale, scale); m_matrix.AxisScale(pg_scale); }

	if (m_pPet) {


		{ WVector pg_scale(scale, scale, scale); m_pPet->GetMatrix().AxisScale(pg_scale); }
	}
}

// petbody.cpp:893
void CPetBody::ChangeTexture(char *name, int index) {
	m_pPet->ChangeTexture(name, index);
}

// petbody.cpp:898
WMatrix CPetBody::GetMatrix(eBody body) {
	WMatrix mat;

	switch (body) {
	case BODY_HEAD:

		mat = RotMat(m_lookAngle.x, 0) * RotMat(m_lookAngle.y, 1) * data_a7fb50 * m_pPet->FindBoneByTail("Head")->GetMatrix();
		break;
	case BODY_PUPPET:

		mat = m_pPet->GetMatrix();
		break;
	case BODY_PUPPET_AT_POS:
		mat = m_pPet->GetMatrix();
		mat.pivot = m_pos;
		break;
	}


	return mat;
}

// petbody.cpp:927
void CPetBody::LookUpProcess(float delta) {
	if (m_lookTarget) {


		WVector toTarget = *m_lookTarget - m_pPet->FindBoneByTail("Head")->GetMatrix().pivot;
		float pitch = (float)atan(toTarget.y / sqrt(toTarget.x * toTarget.x + toTarget.z * toTarget.z));
		m_lookAngleTarget.x = pitch < -0.34906584f ? -0.34906584f : (pitch > 0.78539819f ? 0.78539819f : pitch);


		float yaw = CalcDeltaAngle(*m_lookTarget - m_rotMatrix.pivot, m_rotMatrix.za * -1.0f);
		m_lookAngleTarget.y = yaw;

		if (m_bLookImmediate && Abs(yaw) > 0.78539819f) {

			float over = Abs(yaw) - 0.78539819f;
			SetRotate(WVector(m_rotation.x, m_rotation.y + (yaw > 0.0f ? over : -over), m_rotation.z));
		}


		float yawTarget = m_lookAngleTarget.y;
		m_lookAngleTarget.y = yawTarget < -0.78539819f ? -0.78539819f : (yawTarget > 0.78539819f ? 0.78539819f : yawTarget);
	}
	else {
		m_lookAngleTarget.y = 0.0f;
		m_lookAngleTarget.x = 0.0f;
	}

	WVector2D diff = m_lookAngleTarget - m_lookAngle;


	if (Abs(diff.x) > data_9ed708[0]) {


		m_flags.Enable(0x20);
		m_lookAngle.x += MinOf(delta / diff.Magnitude() * 1.5707964f, 1.0f) * diff.x;
	}

	if (Abs(diff.y) > data_9ed708[0]) {


		m_flags.Enable(0x20);
		m_lookAngle.y += MinOf(delta / diff.Magnitude() * 1.5707964f, 1.0f) * diff.y;
	}
}

// petbody.cpp:967
void CPetBody::FlightProcess(float delta) {
	CGolfBall &ball = GolfBall();



	WVector pos = m_pos;
	float takeOffDist = (m_takeOffDir - m_takeOffPos).Magnitude();
	float dist = (m_takeOffDir - WVector(pos.x, 0.0f, pos.z)).Magnitude();

	switch (m_flightState) {
	case 0:
		if (m_bFlying) {

			if (!(WAbsBySign(m_takeOffAngle) < data_9ed708[2])) {

				float angle = m_takeOffAngle + delta * 0.4f;
				if (angle < 0.0f) {
					m_takeOffAngle = angle;
				}
				else {
					m_takeOffAngle = 0.0f;
				}

				float yaw = CalcDeltaAngle(GetDirection(), WVector::UNIT_NEG_Z);
				SetRotate_Lean(WVector(m_takeOffAngle, yaw, 0.0f));
			}

			if (ball.m_pos.y > m_takeOffHeight)
				m_takeOffHeight = ball.m_pos.y;

			if (m_pos.y < m_takeOffHeight) {
				m_pos.y += delta * m_takeOffSpeed;

				if (m_takeOffSpeed > 0.0f)
					m_takeOffSpeed -= delta * 25.0f;
			}

			if (m_takeOffHeight + 10.0f < m_pos.y)
				m_pos.y -= delta * 10.0f;

			if (dist * 1.5f <= takeOffDist) {
				m_takeOffAngle = 0.0f;
				m_flightState = 1;
				RestoreLeanMat();
			}
		}
		else {
			m_takeOffAngle = 0.0f;
			m_flightState = 1;
			RestoreLeanMat();
		}
		break;

	case 1:
		if (!m_bFlying || dist * 3.0f <= takeOffDist)
			m_flightState = 2;
		break;

	case 2: {
		float height = 0.0f;
		if (!m_customGround.empty()) {
			w_bound_box *box = m_pPet->m_bbList.Find(m_customGround.c_str());
			if (box) {
				Waabb aabb = box->info->aabb;
				aabb.min = aabb.min * box->bone->GetMatrix();
				aabb.max = aabb.max * box->bone->GetMatrix();
				aabb.min.x = (aabb.min.x + aabb.max.x) * 0.5f;
				aabb.min.z = (aabb.min.z + aabb.max.z) * 0.5f;
				height = aabb.min.y - GetPVS().GetGroundPoint(aabb.min, true, NULL, true).y;
			}
		}

		if (m_bFlying) {
			if (!(WAbsBySign(m_takeOffAngle - -0.39269908f) < data_9ed708[2])) {
				float angle = m_takeOffAngle - delta * 0.3f;
				if (angle > -0.39269908f) {
					m_takeOffAngle = angle;
				}
				else {
					m_takeOffAngle = -0.39269908f;
				}

				float yaw = CalcDeltaAngle(GetDirection(), WVector::UNIT_NEG_Z);
				SetRotate_Lean(WVector(m_takeOffAngle, yaw, 0.0f));
			}
		}
		else {
			m_takeOffAngle = 0.0f;
			RestoreLeanMat();
		}

		height = MinOf(delta * 36.0f, height);
		m_pos.y -= height;
		break;
	}
	}

	if (m_bFlying && !(WAbsBySign(ball.m_lieSlope) < data_9ed708[2])) {
		WVector side = WCrossProduct(WVector::UNIT_POS_Y, m_moveDelta);
		side.Normalize();

		float offset = WDotProduct(ball.m_pos - m_pos, side);
		m_pos += side * offset * 1.2f * delta;
	}
}

// petbody.cpp:1123
void CPetBody::LookUp(WVector *target, bool immediate) {
	if (m_pPet->FindBoneByTail("Head") == NULL)
		return;

	m_lookTarget = target;
	m_bLookImmediate = immediate;
}

// petbody.cpp:1132
float CPetBody::GetAnimationLength(const char *motionName) {
	w_motion_data *md = m_pPet->FindMotionData(motionName);

	return md->e - md->s;
}

// petbody.cpp:1139
void CPetBody::SetNextMotion(const char *motionName, bool loop) {
	m_bNextMotionLoop = loop;
	strcpy(m_nextMotion, motionName);
}

// petbody.cpp:1145
void CPetBody::Update(void) {
	m_pPet->ApplyBones(m_pPet->GetBones(NULL, m_motionTime, m_flags.GetFlag(2) ? 1 : 0, NULL, false), m_matrix);
	m_pPet->Transform(NULL, 1.0f, 0);
	m_pPet->UpdateBSphere();
}

// petbody.cpp:1152
WPuppet *CPetBody::GetPet(void) {
	return m_pPet;
}
