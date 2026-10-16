// Original: d:\build\custom\temp\code\client\projectg\polysoup.cpp
#include "NetworkSystem.h"
#include "polysoup.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>

#include "ClientSetting.h"
#include "GolfBall.h"
#include "GolfDoc.h"
#include "SharedDoc.h"
#include "commonutil.h"
#include "contentsdoc.h"
#include "fx.h"
#include "fxsequence.h"
#include "fxspray.h"
#include "quadtree.h"
#include "slicestr.h"
#include "shared/Localize.h"
#include "shared/classdefine.h"
#include "wbone.h"
#include "westpak.h"
#include "wind.h"
#include "wlist.h"
#include "wpuppet.h"
#include "wpuppetutil.h"
#include "wresrcmng.h"
#include "wscene.h"
#include "wutil.h"
#include "wview.h"
#include "wxpuppet.h"

#ifdef PG_MATERIAL_HINTS
// The port's renderer learns which meshes are water (port/compat).
#include "pg_material_hints.h"
#endif

// ?g_lightset@@3ULightSet@@A; its header (wscene.h) does not declare it yet.
extern LightSet g_lightset;

// ?g_camera@@3VWMatrix@@A; no header declares it yet.
extern WMatrix g_camera;

// A nameless .rdata float of this object: the epsilon of the vertex merge
// and of the base-mesh test.
extern "C" const float data_a111e4;

// ?CloseCFile@@YAXPAVcFile@@@Z; no header declares it yet.
void __cdecl CloseCFile(cFile *file);

// polysoup.cpp:57
// The additional models SetAdditionModel loads with the puppet flag set
// (sub_006b8690). Emitted once SetAdditionModel, its only caller, is written.
static bool IsSpecialAdditionModel(const char *name) {
	static const char *s_names[] = {
		"un_sea.pet",
		"silv_dn.pet",
		"ice_unsea.pet",
		"panda_sea.pet",
	};

	for (int i = 0; i < sizeof(s_names) / sizeof(s_names[0]); i++) {
		if (stricmp(name, s_names[i]) == 0) {
			return true;
		}
	}
	return false;
}

#ifdef PG_MATERIAL_HINTS
// Port only (port/compat/pg_material_hints.h): a placed model's hint bits,
// decided once when it loads: water for a sea model, and its shadow rules
// (port/metal/pg_shadow_rules.cpp). Written into each mesh (w_mesh::pgHint),
// which every draw then reads.
// A bone rule ("blue_house02_lamp.pet:Beam") covers the bone and its children.
static void PgSetMeshHints(WBone *pBone, const char *name, int hint) {
	for (; pBone != 0; pBone = pBone->GetNext()) {
		int boneHint = hint | PgShadowRuleForBone(name, pBone->GetBoneName());
		if (boneHint != 0) {
			for (w_mesh *pMesh = pBone->GetMesh(); pMesh != 0; pMesh = pMesh->next) {
				pMesh->pgHint |= boneHint;
			}
		}
		PgSetMeshHints(pBone->GetChild(), name, boneHint);
	}
}

// The port's lamps (port/compat/pg_material_hints.h): once a placed model
// stands where it belongs (ApplyBones), the world centre of each of its meshes
// tagged light, registered for the course (not the lighthouse's beam: no cast
// and no receive). The port lights them whether or not they are drawn.
static void PgRegisterLamps(WBone *pBone) {
	for (; pBone != 0; pBone = pBone->GetNext()) {
		for (w_mesh *pMesh = pBone->GetMesh(); pMesh != 0; pMesh = pMesh->next) {
			if (!(pMesh->pgHint & PG_HINT_LIGHT) || (pMesh->pgHint & (PG_HINT_NOCAST | PG_HINT_NORECEIVE)) == (PG_HINT_NOCAST | PG_HINT_NORECEIVE)
					|| pMesh->vtxNum <= 0 || pMesh->vecList == 0) {
				continue;
			}
			// World positions as WBone's lighting computes them: a rigid
			// vertex (below rigidNum) by its own bone (boneList: a tall lamp's
			// head), the others by the mesh's bone.
			WVector sum(0.0f, 0.0f, 0.0f);
			for (int v = 0; v < pMesh->vtxNum; v++) {
				if (v < pMesh->rigidNum && pMesh->boneList != 0 && pMesh->boneList[v] != 0) {
					sum += pMesh->boneList[v]->Transform(pMesh->vecList[v]);
				} else {
					sum += pMesh->vecList[v] * pBone->GetMatrix();
				}
			}
			sum = sum * (1.0f / (float)pMesh->vtxNum);
			PgLampCourseAdd(sum.x, sum.y, sum.z);
		}
		PgRegisterLamps(pBone->GetChild());
	}
}

// Whether text contains word, any case.
static bool PgContainsNoCase(const char *text, const char *word) {
	size_t len = strlen(word);
	for (; *text; text++) {
		if (_strnicmp(text, word, len) == 0) {
			return true;
		}
	}
	return false;
}

// A placed model's lamps: each bound box attaching a light effect (its *fx
// sequence or the box named lamp, light or glow: blue_lamp02.pet's "LHglow",
// "*fx Blue_StreetLamp"), at the box's centre by its bone (as
// WPuppet::UpdateBSphere); a model without one, its light-tagged meshes
// (PgRegisterLamps). The glow meshes of a lamp can lie on the ground.
static void PgRegisterModelLamps(WPuppet *pPuppet) {
	int boxes = 0;
	for (w_bound_box *pBox = pPuppet->m_bbList.Start(); pBox; pBox = pPuppet->m_bbList.Next()) {
		if (pBox->bone == 0 || pBox->info == 0) {
			continue;
		}
		const char *pOption = pBox->info->option;
		const char *pFx = pOption ? strstr(pOption, "*fx") : 0;
		bool light = PgContainsNoCase(pBox->info->name, "glow") || PgContainsNoCase(pBox->info->name, "lamp") || PgContainsNoCase(pBox->info->name, "light");
		if (pFx != 0) {
			char key[8] = "", fxName[0x20] = "";
			sscanf(pFx, "%7s %31s", key, fxName);
			light = light || PgContainsNoCase(fxName, "glow") || PgContainsNoCase(fxName, "lamp") || PgContainsNoCase(fxName, "light");
		} else {
			light = false;
		}
		if (!light) {
			continue;
		}
		// The box's centre in the world, as WPuppet::UpdateBSphere places a box.
		WVector pos = ((pBox->info->aabb.min + pBox->info->aabb.max) * 0.5f) * pBox->bone->GetMatrix();
		PgLampCourseAdd(pos.x, pos.y, pos.z);
		boxes++;
	}
	if (boxes == 0) {
		PgRegisterLamps(pPuppet->GetRootBone());
	}
}

static void PgSetModelHints(WPuppet *pPuppet, const char *name) {
	if (pPuppet == 0) {
		return;
	}
	int hint = PgShadowRuleForModel(name) | (IsSpecialAdditionModel(name) ? PG_HINT_WATER : 0);
	PgSetMeshHints(pPuppet->GetRootBone(), name, hint);
}
#endif

// polysoup.cpp:85
WPolySoup::WPolySoup(CGolfDoc *pDoc)
	: m_pTextures(0), m_numOldTextures(0), m_pOldTextures(0), m_pCameras(0),
	  m_pAreas(0), m_pPoints(0), m_pBgModel(0), m_bPendingOption(false), m_bUseTnL(false),
	  m_numSmapMeshes(0), m_pSmapDrawFlags(0), m_pSmapMeshes(0), m_bNoShadowmap(false),
	  m_holecupTexture(0) {
	m_pGolfDoc = pDoc;
	m_mapName[0] = 0;
	memset(m_numOtherRecords, 0, 0x28);
	m_numObjects = 0;

	m_cullDistance = 0;
	m_defaultNear = 800.0f;
	m_defaultFar = 960.0f;

	m_models.clear();
	m_numStructuredObjects = 0;
	m_extraBox.Reset();
}

// polysoup.cpp:103
WPolySoup::~WPolySoup(void) {
	Clear();
	ClearOldTextures();

	if (g_resrcmng && m_holecupTexture) { g_resrcmng->Release(m_holecupTexture); m_holecupTexture = 0; }
}

// polysoup.cpp:111
// Releases everything the soup loaded: its models, cameras, points, areas,
// textures, background effects, shadow-map meshes and static puppet groups.
void WPolySoup::Clear(void) {
	memset(m_mapName, 0, sizeof(m_mapName));

	if (m_models.size() != 0) {
		for (int i = 0; i < m_numObjects; i++) {
			sModel *pModel = m_models[i];

			if (pModel->m_pPuppet) {
				g_resrcmng->Release(pModel->m_pPuppet, false);
				pModel->m_pPuppet = 0;
			}
			if (pModel->m_pQuads) {
				delete[] pModel->m_pQuads;
				pModel->m_pQuads = 0;
			}
			if (pModel->m_pTriangles) {
				delete[] pModel->m_pTriangles;
				pModel->m_pTriangles = 0;
			}
			delete pModel;
		}
	}

	if (m_pBgModel) {
		if (m_pBgModel->m_pTriangles) {
			delete[] m_pBgModel->m_pTriangles;
			m_pBgModel->m_pTriangles = 0;
		}
		if (m_pBgModel->m_pPuppet) {
			g_resrcmng->Release(m_pBgModel->m_pPuppet, true);
			m_pBgModel->m_pPuppet = 0;
		}
		delete m_pBgModel;
		m_pBgModel = 0;
	}

	BackupTextures();

	m_models.clear();

	if (m_pCameras) {
		delete[] m_pCameras;
		m_pCameras = 0;
	}
	if (m_pPoints) {
		delete[] m_pPoints;
		m_pPoints = 0;
	}
	if (m_pAreas) {
		delete[] m_pAreas;
		m_pAreas = 0;
	}

	m_boundAreas.clear();
	m_texIndexMap.clear();

	m_lightSets.clear();
	m_ulongList.clear();
	m_soundBoxes.clear();

	memset(m_bgFx, 0, sizeof(m_bgFx));
	m_numBgFx = 0;

	for (int i = 0; i < m_numSmapMeshes; i++) {
		g_resrcmng->Release(m_pSmapDrawFlags[i]);
	}
	m_numSmapMeshes = 0;
	if (m_pSmapDrawFlags) {
		delete[] m_pSmapDrawFlags;
		m_pSmapDrawFlags = 0;
	}
	m_pSmapMeshes = 0;

	m_staticPetGrp.Clear();

	if (WSingleton<CSceneManager>::Instance()) {
		WSingleton<CSceneManager>::Instance()->ReleaseElement();
	}
	if (WSingleton<CFx>::Instance()) {
		WSingleton<CFx>::Instance()->ClearAll(false);
	}

	m_triArrays.clear();
}

// polysoup.cpp:234
void WPolySoup::BackupTextures(void) {
	ClearOldTextures();

	m_numOldTextures = m_numTextures;
	m_pOldTextures = m_pTextures;
	m_pTextures = 0;
}

// polysoup.cpp:246
void WPolySoup::ClearOldTextures(void) {
	for (int i = 0; i < m_numOldTextures; i++) {
		if (g_resrcmng && m_pOldTextures[i].m_handle) {
			g_resrcmng->Release(m_pOldTextures[i].m_handle);
			m_pOldTextures[i].m_handle = 0;
		}
	}
	if (m_pOldTextures) {
		delete[] m_pOldTextures;
		m_pOldTextures = 0;
	}
	m_numOldTextures = 0;
}

#ifdef PG_7XX
// PG_7XX: the .gbin version 0x72 of the later clients (the JP 983 exe's
// readers, 0x9271C0 and the record readers it calls). Against 0x71: the
// header has an eleventh word (the count of a section after the models,
// which this client does not read), each camera 0x20 more bytes (a second
// eye and target, two words), each point 0x14 (three floats, two words)
// after its description, each area 0x20 (a second box, two words), each
// model record 0x38 (twelve floats, two words). The file ends with the par
// alone: the tees and the pins are the points named start point (½ÃÀÛÁ¡, and
// ½ÃÀÛÁ¡2 for the course kind 2) and end point (³¡Á¡1_1..1_3, ³¡Á¡2_1..2_3,
// ³¡Á¡ for any missing one), as the JP exe's sub_926F00 finds them.
static const int kGbin72HeaderExtra = 4;
static const int kGbin72CameraExtra = 0x20;
static const int kGbin72PointExtra = 0x14;
static const int kGbin72AreaExtra = 0x20;
static const int kGbin72ModelExtra = 0x38;

// The background model's vertex colours of a 0x72 .gbin, read after its
// record (the JP exe's sub_948BF0): groups of a bone name and its meshes in
// order, each mesh a count and one colour per vertex. 0x71 has one colour per
// triangle corner instead, read in Load's triangle loop. Colours are applied
// as that loop does (fog brightness, an alpha made opaque).
static void ReadGbin72Colors(cFile *pFile, WPuppet *pPuppet, float fogBrightness) {
	int numGroups = 0;
	pFile->Read(&numGroups, 4);
	for (int g = 0; g < numGroups; g++) {
		int nameLength = 0;
		pFile->Read(&nameLength, 4);
		char name[0x101];
		int stored = nameLength < 0x100 ? nameLength : 0x100;
		pFile->Read(name, stored);
		name[stored] = 0;
		if (nameLength > stored) {
			pFile->Seek(nameLength - stored, 1);
		}

		WBone *pBone = pPuppet ? pPuppet->FindBone(name) : 0;
		w_mesh *pMesh = pBone ? pBone->GetMesh() : 0;

		int numMeshes = 0;
		pFile->Read(&numMeshes, 4);
		for (int m = 0; m < numMeshes; m++) {
			int numColors = 0;
			pFile->Read(&numColors, 4);
			for (int c = 0; c < numColors; c++) {
				unsigned long color = 0;   // 4 bytes read: 8-byte long on LP64 (macOS)
				pFile->Read(&color, 4);
				if (pMesh && c < pMesh->vtxNum && pMesh->vtxColorList) {
					pMesh->vtxColorList[c] = ModifyColor(color, fogBrightness);
					if (color >> 24) {
						pMesh->vtxColorList[c] |= 0xFF000000;
					}
				}
			}
			if (pMesh) {
				pMesh = pMesh->next;
			}
		}
	}
}

// The tee (slot 0) and pins (1..3) of the course kinds other than 2, and
// those of kind 2 (4..7), from the points of a 0x72 .gbin; false when one is
// missing. Leaves the file after the points.
static bool ReadGbin72HolePoints(cFile *pFile, const int *header, WVector *slots) {
	pFile->Seek(0x28 + kGbin72HeaderExtra + header[5] * (0x40 + kGbin72CameraExtra), 0);

	bool found[8] = { false, false, false, false, false, false, false, false };
	bool anyEnd = false;
	WVector endPoint;
	static const char *const kNames[8] = {
		"\xBD\xC3\xC0\xDB\xC1\xA1",
		"\xB3\xA1\xC1\xA1" "1_1", "\xB3\xA1\xC1\xA1" "1_2", "\xB3\xA1\xC1\xA1" "1_3",
		"\xBD\xC3\xC0\xDB\xC1\xA1" "2",
		"\xB3\xA1\xC1\xA1" "2_1", "\xB3\xA1\xC1\xA1" "2_2", "\xB3\xA1\xC1\xA1" "2_3",
	};
	for (int i = 0; i < header[6]; i++) {
		unsigned char type;
		char name[0x21];
		WVector pos;
		pFile->Read(&type, 1);
		pFile->Read(name, 0x20);
		name[0x20] = 0;
		pFile->Read(&pos, 0xC);
		pFile->Seek((type != 0 ? 0x40 : 0) + kGbin72PointExtra, 1);

		if (strcmp(name, "\xB3\xA1\xC1\xA1") == 0) {
			endPoint = pos;
			anyEnd = true;
			continue;
		}
		for (int s = 0; s < 8; s++) {
			if (strcmp(name, kNames[s]) == 0) {
				slots[s] = pos;
				found[s] = true;
				break;
			}
		}
	}

	if (found[0] && !found[4]) {
		slots[4] = slots[0];
		found[4] = true;
	}
	for (int s = 0; s < 8; s++) {
		if (!found[s] && s != 0 && s != 4 && anyEnd) {
			slots[s] = endPoint;
			found[s] = true;
		}
		if (!found[s]) {
			return false;
		}
	}
	return true;
}

// What LoadMapCheckData sets from a hole's check data (CGolfDoc's hole
// array is WPolySoup's to write).
struct sGbin72HoleCheck {
	unsigned char m_par;
	WVector m_teePos;
	WVector m_cupPos;
	unsigned long m_holeKey;
};

// LoadMapCheckData for a 0x72 .gbin (header already read): par, tee, cup,
// key and the approach mode's "*extra" area, as the 0x71 path below sets them.
static bool LoadMapCheckData72(cFile *pFile, const int *header, unsigned char hole, Waabb &extraBox, sGbin72HoleCheck &holeData) {

	WVector slots[8];
	if (!ReadGbin72HolePoints(pFile, header, slots)) {
		return false;
	}

	// The areas follow the points: the approach mode's "*extra" box.
	Waabb box;
	int j = header[7];
	if (GOLFDOC()->m_approachMode) {
		for (j = 0; j < header[7]; j++) {
			unsigned char type;
			char name[0x41];
			WVector v;
			pFile->Read(&type, 1);
			pFile->Read(name, 0x40);
			name[0x40] = 0;
			pFile->Read(&v, 0xC);
			box.min = v;
			pFile->Read(&v, 0xC);
			box.max = v;
			pFile->Seek(kGbin72AreaExtra, 1);
			if (strstr(name, "*extra")) {
				break;
			}
		}
	}

	pFile->Seek(-1, 2);
	pFile->Read(&holeData.m_par, 1);

	unsigned char index = GetHoleIndex(hole) - 1;
	int set = WSingleton<CSharedDoc>::Instance()->m_pCurCourse != 0 && WSingleton<CSharedDoc>::Instance()->m_pCurCourse->courseFlags >> 4 == 2 ? 4 : 0;
	int pin = WSingleton<CSharedDoc>::Instance()->m_holePin[index];
	if (pin < 0 || pin > 2) {
		pin = 0;
	}
	holeData.m_teePos.x = slots[set].x;
	holeData.m_teePos.z = slots[set].z;
	holeData.m_cupPos.x = slots[set + 1 + pin].x;
	holeData.m_cupPos.z = slots[set + 1 + pin].z;

	pFile->Seek(0, 0);
	holeData.m_holeKey = 0;
	int size = pFile->GetByte();
	for (int n = 0; n < size; n++) {
		unsigned char c;
		pFile->Read(&c, 1);
		holeData.m_holeKey += c;
	}

	if (GOLFDOC()->m_approachMode) {
		if (j == header[7]) {
			if (!WSingleton<WNetworkSystem>::Instance()->IsConnected((WNetworkSystem::eNetUnit)0)) {
				MessageBoxA(0, MakeStr("%d\xC8\xA6\xBF\xA1 \xBE\xEE\xC7\xC1\xB7\xCE\xC4\xA1 \xBF\xB5\xBF\xAA\xC0\xCC \xBC\xB3\xC1\xA4\xB5\xC7\xBE\xEE \xC0\xD6\xC1\xF6 \xBE\xCA\xBD\xC0\xB4\xCF\xB4\xD9", hole), "Warning", 0);
			}
		} else {
			if (GOLFDOC()->m_currentHole == hole) {
				extraBox = box;
			}
			holeData.m_teePos.x = (float)(WSingleton<CSharedDoc>::Instance()->m_holeRandom[0] & 0x7F) * (box.max.x - box.min.x) / 128.0f + box.min.x;
			holeData.m_teePos.z = (float)(WSingleton<CSharedDoc>::Instance()->m_holeRandom[1] & 0x7F) * (box.max.z - box.min.z) / 128.0f + box.min.z;
		}
	}
	return true;
}
#endif

// polysoup.cpp:257
// Reads a hole's check data (<course>_<hole>.gbin): the hole's par, its tee
// and cup positions (the cup of the hole's pin), a key summed over the file's
// bytes, and in approach mode the "*extra" area the tee is placed in.
bool WPolySoup::LoadMapCheckData(unsigned char hole) {
	cFile *pFile = GetResrcManager()->GetCFile(MakeStr("%s_%02d.gbin", WSingleton<CSharedDoc>::Instance()->m_pCurCourse ? WSingleton<CSharedDoc>::Instance()->m_pCurCourse->m_mapName : 0, hole), -1);

	if (pFile == 0) {
		return false;
	}

	int header[10];
	pFile->Read(header, 0x28);

#ifdef PG_7XX
	if (header[1] == 0x72) {
		sGbin72HoleCheck check;
		bool loaded = LoadMapCheckData72(pFile, header, hole, m_extraBox, check);
		CloseCFile(pFile);
		if (loaded) {
			sHoleData &holeData = GOLFDOC()->m_holeData[hole - 1];
			holeData.m_par = check.m_par;
			holeData.m_teePos.x = check.m_teePos.x;
			holeData.m_teePos.z = check.m_teePos.z;
			holeData.m_cupPos.x = check.m_cupPos.x;
			holeData.m_cupPos.z = check.m_cupPos.z;
			holeData.m_holeKey = check.m_holeKey;
			WSingleton<CSharedDoc>::Instance()->m_courseHoleOn[hole - 1] = check.m_par;
		}
		return loaded;
	}
#endif
	if (header[1] != 0x71) {
		MessageBoxA(0, MakeStr("%d\xB9\xF8\xC8\xA6 \xB9\xE8\xB0\xE6 \xC6\xC4\xC0\xCF\xC0\xC7 \xB9\xF6\xC1\xAF\xC0\xCC \xB8\xC2\xC1\xF6 \xBE\xCA\xBD\xC0\xB4\xCF\xB4\xD9.", hole), "\xB9\xE8\xB0\xE6\xC6\xC4\xC0\xCF \xB9\xF6\xC1\xAF", 0);
		return false;
	}

	pFile->Seek(-0x41, 2);

	pFile->Read(&GOLFDOC()->m_holeData[hole - 1].m_par, 1);
	WSingleton<CSharedDoc>::Instance()->m_courseHoleOn[hole - 1] = GOLFDOC()->m_holeData[hole - 1].m_par;

	unsigned char index = GetHoleIndex(hole) - 1;

	if (WSingleton<CSharedDoc>::Instance()->m_pCurCourse != 0 && WSingleton<CSharedDoc>::Instance()->m_pCurCourse->courseFlags >> 4 == 2) {
		pFile->Seek(8, 1);

		pFile->Read(&GOLFDOC()->m_holeData[hole - 1].m_teePos.x, 4);
		pFile->Read(&GOLFDOC()->m_holeData[hole - 1].m_teePos.z, 4);

		pFile->Seek(WSingleton<CSharedDoc>::Instance()->m_holePin[index] * 8 + 0x18, 1);
		pFile->Read(&GOLFDOC()->m_holeData[hole - 1].m_cupPos.x, 4);
		pFile->Read(&GOLFDOC()->m_holeData[hole - 1].m_cupPos.z, 4);
		pFile->Seek((2 - WSingleton<CSharedDoc>::Instance()->m_holePin[index]) * 8, 1);
	} else {
		pFile->Read(&GOLFDOC()->m_holeData[hole - 1].m_teePos.x, 4);
		pFile->Read(&GOLFDOC()->m_holeData[hole - 1].m_teePos.z, 4);

		pFile->Seek(8, 1);

		pFile->Seek(WSingleton<CSharedDoc>::Instance()->m_holePin[index] * 8, 1);
		pFile->Read(&GOLFDOC()->m_holeData[hole - 1].m_cupPos.x, 4);
		pFile->Read(&GOLFDOC()->m_holeData[hole - 1].m_cupPos.z, 4);
		pFile->Seek((5 - WSingleton<CSharedDoc>::Instance()->m_holePin[index]) * 8, 1);
	}

	pFile->Seek(0, 0);
	GOLFDOC()->m_holeData[hole - 1].m_holeKey = 0;
	int size = pFile->GetByte();
	for (int n = 0; n < size; n++) {
		unsigned char c;
		pFile->Read(&c, 1);
		GOLFDOC()->m_holeData[hole - 1].m_holeKey += c;
	}

	if (GOLFDOC()->m_approachMode) {
		pFile->Seek(0x28, 0);

		int numPoints = header[5];
		pFile->Seek(numPoints * 0x40, 1);

		for (int i = 0; i < header[6]; i++) {
			unsigned char kind;
			pFile->Read(&kind, 1);

			int recordSize = 0x2C;
			if (kind != 0) {
				recordSize = 0x6C;
			}
			pFile->Seek(recordSize, 1);
		}

		Waabb box;
		int j = 0;
		for (; j < header[7]; j++) {
			struct {
				unsigned char m_type;
				char m_name[0x40];
			} area;
			WVector v;

			pFile->Read(&area.m_type, 1);
			pFile->Read(area.m_name, 0x40);
			pFile->Read(&v, 0xC);
			box.min = v;
			pFile->Read(&v, 0xC);
			box.max = v;

			if (strstr(area.m_name, "*extra")) {
				if (GOLFDOC()->m_currentHole == hole) {
					m_extraBox = box;
				}
				break;
			}
		}

		if (j == header[7]) {
			if (!WSingleton<WNetworkSystem>::Instance()->IsConnected((WNetworkSystem::eNetUnit)0)) {
				MessageBoxA(0, MakeStr("%d\xC8\xA6\xBF\xA1 \xBE\xEE\xC7\xC1\xB7\xCE\xC4\xA1 \xBF\xB5\xBF\xAA\xC0\xCC \xBC\xB3\xC1\xA4\xB5\xC7\xBE\xEE \xC0\xD6\xC1\xF6 \xBE\xCA\xBD\xC0\xB4\xCF\xB4\xD9", hole), "Warning", 0);
			}
		} else {
			GOLFDOC()->m_holeData[hole - 1].m_teePos.x = (float)(WSingleton<CSharedDoc>::Instance()->m_holeRandom[0] & 0x7F) * (box.max.x - box.min.x) / 128.0f + box.min.x;
			GOLFDOC()->m_holeData[hole - 1].m_teePos.z = (float)(WSingleton<CSharedDoc>::Instance()->m_holeRandom[1] & 0x7F) * (box.max.z - box.min.z) / 128.0f + box.min.z;
		}
	}

	CloseCFile(pFile);
	return true;
}

// polysoup.cpp:390
// qsort comparator of the models: by name, then by m_order, then by
// m_subOrder.
int __cdecl WPolySoup::CompareModel(const void *a, const void *b) {
	const sModel *pA = *(const sModel *const *)a;
	const sModel *pB = *(const sModel *const *)b;

	int result = _strcmpi(pA->m_name, pB->m_name);

	if (result != 0) {
		return result;
	}
	return pA->m_order < pB->m_order ? -1 : pA->m_subOrder > pB->m_subOrder;
}

// polysoup.cpp:408
// Whether a "*fx" record of the soup file places the silvia cannon sequence
// (sub_006ba430, called by Load). A record starts with its type byte (6).
static bool __stdcall IsSilviaCannonFx(const char *record) {
	if (*record != 6) {
		return false;
	}

	char name[0x40];
	WVector pos;
	WSliceStr slice = record + 1;

	slice.Get("*fx", "%s %f %f %f", name, &pos.x, &pos.y, &pos.z);

	return strcmpi(name, "silviacannon.seq") == 0 ? true : false;
}

// polysoup.cpp:468
// Loads the course's soup file: header, cameras, points, areas, textures,
// bound polygons, the background model with its collision triangles, the
// placed models, and the effects of the type-6 areas.
bool WPolySoup::Load(const char *name, bool bPlaceHoleCup) {
	int tnlSupported;
	g_resrcmng->VideoReference()->Command((wVDevMessage)0x20, 1, (intptr_t)&tnlSupported);

	if (tnlSupported) {
		m_bUseTnL = true;
	}
	Clear();

	strncpy(m_mapName, name, 0x3F);
	m_mapName[0x3F] = 0;

	IFF_STRUCT::sCourse *pCourse = WSingleton<CSharedDoc>::Instance()->m_pCurCourse;
	unsigned char courseKind = pCourse == 0 ? 0 : pCourse->courseFlags >> 4;

	cFile *pFile = GetResrcManager()->GetCFile(name, -1);
	if (pFile == 0) {
		MessageBoxA(0, MakeStr("\xB9\xE8\xB0\xE6 \xC6\xC4\xC0\xCF(%s)\xC0\xBB \xC0\xD0\xBE\xEE\xBF\xC0\xC1\xF6 \xB8\xF8\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.", name), "\xC6\xC4\xC0\xCF \xC0\xD0\xB1\xE2 \xBD\xC7\xC6\xD0", 0);
		return false;
	}

	pFile->Read(m_numOtherRecords, 0x28);

#ifdef PG_7XX
	bool gbin72 = m_numOtherRecords[1] == 0x72;
	if (gbin72) {
		pFile->Seek(kGbin72HeaderExtra, 1);
	}
	if (m_numOtherRecords[1] != 0x71 && !gbin72) {
#else
	if (m_numOtherRecords[1] != 0x71) {
#endif
		MessageBoxA(0, "\xB9\xE8\xB0\xE6 \xC6\xC4\xC0\xCF\xC0\xC7 \xB9\xF6\xC1\xAF\xC0\xCC \xB8\xC2\xC1\xF6 \xBE\xCA\xBD\xC0\xB4\xCF\xB4\xD9.", "\xB9\xE8\xB0\xE6\xC6\xC4\xC0\xCF \xB9\xF6\xC1\xAF", 0);
		return false;
	}

	switch (courseKind) {
	case 0:
		m_numObjects = m_numOtherRecords[2];
		break;
	case 1:
		m_numObjects = m_numOtherRecords[3] + m_numOtherRecords[2];
		break;
	case 2:
		m_numObjects = m_numOtherRecords[4] + m_numOtherRecords[2];
		break;
	default:
		m_numObjects = m_numOtherRecords[4] + m_numOtherRecords[3] + m_numOtherRecords[2];
		break;
	}

	m_pTextures = new sTexture[m_numTextures];

	sModel **ppModels = new sModel *[m_numObjects];
	for (int i = 0; i < m_numObjects; i++) {
		ppModels[i] = new sModel;
	}

	m_pPoints = new sPoint[m_numPoints];
	m_pAreas = new sArea[m_numAreas];
	m_pCameras = new sCamera[m_numCameras];

	m_pBgModel = new sModel;

	for (int i = 0; i < m_numCameras; i++) {
		sCamera *pCamera = &m_pCameras[i];

		pFile->Read(pCamera->m_name, 0x20);
		WVector pos;
		pFile->Read(&pos, 0xC);
		WVector at;
		pFile->Read(&at, 0xC);
		pCamera->m_matrix = MakeCameraMatrix(WVector(pos.p), WVector(at.p));

		pFile->Read(&pCamera->m_fov, 4);
		pFile->Read(&pCamera->m_param, 4);
#ifdef PG_7XX
		if (gbin72) {
			pFile->Seek(kGbin72CameraExtra, 1);
		}
#endif
	}

	g_lightset.type = LightSet::W_LIGHT_DIRECTIONAL;
	g_lightset.diffuse = 0xFFFFFF;
	g_lightset.ambient = 0x808080;
	g_lightset.ambient2 = 0xFFFFFF;

	for (int i = 0; i < m_numPoints; i++) {
		sPoint *pPoint = &m_pPoints[i];

		pFile->Read(&pPoint->m_type, 1);
		pFile->Read(pPoint->m_name, 0x20);
		WVector vec;
		pFile->Read(&vec, 0xC);
		pPoint->m_pos = vec;

		if (pPoint->m_type != 0) {
			char desc[0x40];
			pFile->Read(desc, 0x40);
			pPoint->m_desc = desc;
		}
#ifdef PG_7XX
		if (gbin72) {
			pFile->Seek(kGbin72PointExtra, 1);
		}
#endif

		switch (pPoint->m_type) {
		case 1:
			sscanf(pPoint->m_desc.c_str(), "%x %x %x", &g_lightset.diffuse, &g_lightset.ambient, &g_lightset.ambient2);
			g_lightset.nearone = pPoint->m_pos;
			break;
		case 3:
			GOLFDOC()->SetDay(false);
			break;
		}
	}

	g_lightset.diffuse = ModifyColor(g_lightset.diffuse, GOLFDOC()->m_fogBrightness);
	g_lightset.ambient = ModifyColor(g_lightset.ambient, GOLFDOC()->m_fogBrightness);
	g_lightset.ambient2 = ModifyColor(g_lightset.ambient2, GOLFDOC()->m_fogBrightness);

	for (int i = 0; i < m_numAreas; i++) {
		sArea *pArea = &m_pAreas[i];

		pFile->Read(&pArea->m_type, 1);
		pFile->Read(pArea->m_name, 0x40);
		WVector vec;
		pFile->Read(&vec, 0xC);
		pArea->m_aabb.min = vec;
		pFile->Read(&vec, 0xC);
		pArea->m_aabb.max = vec;
#ifdef PG_7XX
		if (gbin72) {
			pFile->Seek(kGbin72AreaExtra, 1);
		}
#endif

		if (pArea->m_type == 5) {
			sSoundBox soundBox;
			soundBox.m_interval = -1.0f;
			soundBox.m_range = 60.0f;
			soundBox.m_elapsed = 0.0f;
			sscanf(pArea->m_name, "%s %f %f", soundBox.m_name, &soundBox.m_range, &soundBox.m_interval);
			soundBox.m_box = pArea->m_aabb;
			m_soundBoxes.push_back(soundBox);
		}

		if (strstr(pArea->m_name, "*extra")) {
			m_extraBox = pArea->m_aabb;
		}
	}

	int curMap = GetCurMap() > 0x7F && GetCurMap() != 0xFD ? GetCurMap() - 0x80 : GetCurMap();
	IFF_STRUCT::sCourse *pMapCourse = ItemManager()->FindCourse(curMap | 0x28000000);

	if (GOLFDOC()->m_currentHole != 0x13) {
		if (pMapCourse && pMapCourse->m_propertyFile[0]) {
			m_pGolfDoc->LoadXml(pMapCourse->m_propertyFile, GOLFDOC()->m_currentHole - 1, false);
		} else {
			m_pGolfDoc->LoadXml(MakeStr("%s_property.xml", GetCurMapName()), GOLFDOC()->m_currentHole - 1, false);
		}
	}

	for (int i = 0; i < m_numTextures; i++) {
		pFile->Read(m_pTextures[i].m_name, 0x20);
		m_pTextures[i].m_handle = GetResrcManager()->LoadTexture(m_pTextures[i].m_name, 0, 0, 0);
		m_pTextures[i].m_param = m_pGolfDoc->m_propertyInfo.GetPropertyIndex(m_pTextures[i].m_name);
		m_texIndexMap[m_pTextures[i].m_handle] = i;
	}

	m_bAltCameraBounds = false;
	for (int i = 0; i < m_numBoundAreas; i++) {
		sBoundAreaRecord areaRecord;
		pFile->Read(&areaRecord, 0x18);

		CNodeContainer node(std::string(areaRecord.m_name));

		if (strstr(areaRecord.m_name, "outline")) {
			node.SetType((CNodeContainer::eType)1);
		} else if (strstr(areaRecord.m_name, "ib")) {
			node.SetType((CNodeContainer::eType)2);
		} else if (strstr(areaRecord.m_name, "ob")) {
			node.SetType((CNodeContainer::eType)3);
		} else if (strstr(areaRecord.m_name, "auto")) {
			node.SetType((CNodeContainer::eType)4);
		} else if (strstr(areaRecord.m_name, "beach")) {
			node.SetType((CNodeContainer::eType)5);
		} else if (strstr(areaRecord.m_name, "riverex")) {
			node.SetType((CNodeContainer::eType)8);
		} else if (strstr(areaRecord.m_name, "waterfall")) {
			node.SetType((CNodeContainer::eType)9);
		} else if (strstr(areaRecord.m_name, "river")) {
			node.SetType((CNodeContainer::eType)7);
		} else if (strstr(areaRecord.m_name, "lava")) {
			node.SetType((CNodeContainer::eType)0xD);
		} else if (strstr(areaRecord.m_name, "sea")) {
			node.SetType((CNodeContainer::eType)0xA);
		} else if (strstr(areaRecord.m_name, "wave")) {
			node.SetType((CNodeContainer::eType)0xB);
		} else if (strstr(areaRecord.m_name, "npc")) {
			node.SetType((CNodeContainer::eType)0xC);
		} else if (strstr(areaRecord.m_name, "camera")) {
			m_bAltCameraBounds = true;
			node.SetType((CNodeContainer::eType)6);
		}

		for (int j = 0; j < areaRecord.m_numPoints; j++) {
			WVector vec;
			pFile->Read(&vec, 0xC);
			WVector point = vec;
			node.Add(point);
		}
		m_boundAreas.push_back(node);
	}

	if (g_lightset.type == LightSet::W_LIGHT_DIRECTIONAL) {
		g_lightset.nearone.x *= -1.0f;
		g_lightset.nearone.y *= -1.0f;
		g_lightset.nearone.z *= -1.0f;
		g_lightset.nearone.Normalize();
	}

	sModelRecord record;
	record.m_flags = 0;
	pFile->Read(&record, 0xAC);
#ifdef PG_7XX
	if (gbin72) {
		pFile->Seek(kGbin72ModelExtra, 1);
	}
#endif

	m_pBgModel->m_subOrder = record.m_flags & 7;
	m_pBgModel->m_order = record.m_flags & 0xF8;
	m_pBgModel->m_aabb = record.m_aabb;
	m_pBgModel->m_matrix = record.m_matrix;
	m_pBgModel->m_basePos = (m_pBgModel->m_aabb.max + m_pBgModel->m_aabb.min) * 0.5f;
	m_pBgModel->m_center = (m_pBgModel->m_aabb.max + m_pBgModel->m_aabb.min) * 0.5f;
	m_pBgModel->m_radius = (m_pBgModel->m_aabb.max - m_pBgModel->m_aabb.min).Magnitude() * 0.5f;
	m_pBgModel->m_scale = sqrtf(record.m_matrix.xx * record.m_matrix.xx + record.m_matrix.zx * record.m_matrix.zx + record.m_matrix.yx * record.m_matrix.yx);
	strncpy(m_pBgModel->m_name, record.m_name, 0x1F);

	m_pBgModel->m_pPuppet = g_resrcmng->GetPuppet(m_pBgModel->m_name, false, false, false);
	m_pBgModel->m_pPuppet->m_lightmode = 0x1000000;
	m_pBgModel->m_pPuppet->ApplyBones(0, m_pBgModel->m_matrix);
	m_pBgModel->m_pPuppet->UpdateBound(true, false);
	m_pBgModel->m_pPuppet->UpdateLightSource(&g_lightset, false, 0);

	m_pBgModel->m_pTriangles = new sTriangle[m_pBgModel->m_pPuppet->GetFaceNum()];

	WBone *pRoot = m_pBgModel->m_pPuppet->GetRootBone();
	bool bBaseMeshTest = WSingleton<COption>::Instance()->dIsBaseMeshTestEnabled();

	int triIndex = 0;
	for (w_mesh *pMesh = pRoot->GetMesh(); pMesh; pMesh = pMesh->next) {
		for (int i = 0; i < pMesh->indexNum; i += 3) {
			for (int j = 0; j < 3; j++) {
				int v = pMesh->indexList[i + j];
#ifdef PG_7XX
				if (!gbin72) {   // 0x72: per-vertex colours after this loop
#endif
				unsigned long color;
				pFile->Read(&color, 4);

				pMesh->vtxColorList[v] = ModifyColor(color, GOLFDOC()->m_fogBrightness);
				if (color >> 24) {
					pMesh->vtxColorList[v] |= 0xFF000000;
				}
#ifdef PG_7XX
				}
#endif
				m_pBgModel->m_pTriangles[triIndex].m_vertex[j] = pMesh->vecList[v] * pRoot->GetMatrix();
			}

			WVector *pVertex = m_pBgModel->m_pTriangles[triIndex].m_vertex;

			m_pBgModel->m_pTriangles[triIndex].m_plane.normal = WCrossProduct(pVertex[2] - pVertex[1], pVertex[0] - pVertex[1]);
			m_pBgModel->m_pTriangles[triIndex].m_plane.normal.Normalize();
			m_pBgModel->m_pTriangles[triIndex].m_plane.dis = m_pBgModel->m_pTriangles[triIndex].m_plane.normal.z * -1.0f * pVertex[0].z + m_pBgModel->m_pTriangles[triIndex].m_plane.normal.y * -1.0f * pVertex[0].y + m_pBgModel->m_pTriangles[triIndex].m_plane.normal.x * -1.0f * pVertex[0].x;
			m_pBgModel->m_pTriangles[triIndex].m_doubleSided = (pMesh->xiDrawFlag2 & 0x400) != 0;
			m_pBgModel->m_pTriangles[triIndex].m_propIndex = m_pTextures[m_texIndexMap[pMesh->texHandle]].m_param;
			m_pBgModel->m_pTriangles[triIndex].m_texIndex = m_pTextures[m_texIndexMap[pMesh->texHandle]].m_handle;

			CGimmickContainer *pGimmick = 0;
			IContentsDataContainer *pContainer = WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0xA3);
			if (pContainer) {
				pGimmick = (CGimmickContainer *)pContainer;
			}

			w_texlist *pTexList = g_resrcmng->FindTexture(m_pBgModel->m_pTriangles[triIndex].m_texIndex);
			if (pGimmick->NeedToMakeTriPtArray(pTexList->texname, 0)) {
				std::map<int, std::vector<sTriangle *> >::iterator it = m_triArrays.find(pMesh->texHandle);
				if (it == m_triArrays.end()) {
					std::vector<sTriangle *> tris;
					tris.push_back(&m_pBgModel->m_pTriangles[triIndex]);
					m_triArrays[pMesh->texHandle] = tris;
				} else {
					it->second.push_back(&m_pBgModel->m_pTriangles[triIndex]);
				}
			}

			if (bBaseMeshTest) {
				if (m_pBgModel->m_pTriangles[triIndex].m_plane.normal.y < 0.0f) {
					*(unsigned short *)&pMesh->vtxColorList[pMesh->indexList[i]] = 0;
					*(unsigned short *)&pMesh->vtxColorList[pMesh->indexList[i + 1]] = 0;
					*(unsigned short *)&pMesh->vtxColorList[pMesh->indexList[i + 2]] = 0;
				} else if (WisZero(m_pBgModel->m_pTriangles[triIndex].m_plane.normal.y, data_a111e4)) {
					pMesh->vtxColorList[pMesh->indexList[i]] &= 0xFF0000FF;
					pMesh->vtxColorList[pMesh->indexList[i + 1]] &= 0xFF0000FF;
					pMesh->vtxColorList[pMesh->indexList[i + 2]] &= 0xFF0000FF;
				}
			}
			triIndex++;
		}
	}

#ifdef PG_7XX
	if (gbin72) {
		ReadGbin72Colors(pFile, m_pBgModel->m_pPuppet, GOLFDOC()->m_fogBrightness);
	}
#endif

	w_bound_box *pCenter = m_pBgModel->m_pPuppet->m_bbList.Find("center");
	if (pCenter && pCenter->info->option) {
		WSliceStr slice = pCenter->info->option;
		slice.Get("*wind", "%f", &m_pBgModel->m_windFactor);
	}

	if (m_pBgModel->m_pPuppet->m_bbList.Find("npc")) {
		m_ulongList.push_back(0xFFFF);
	}

	WSingleton<CSceneManager>::Instance()->RegisterElement((eObjType)0, m_pBgModel->m_pPuppet, 0, m_pBgModel->m_name, 3, 0);

#ifdef PG_MATERIAL_HINTS
	// A new course: the last one's lamps go (PgRegisterLamps).
	PgLampCourseReset();
#endif
	int numInstances = 0;
	int numRecords = m_numOtherRecords[4] + m_numOtherRecords[3] + m_numOtherRecords[2];
	int modelIndex = 0;
	for (int i = 0; i < numRecords; i++) {
		pFile->Read(&record, 0xAC);
#ifdef PG_7XX
		if (gbin72) {
			pFile->Seek(kGbin72ModelExtra, 1);
		}
#endif

		if ((record.m_kind == 1 && courseKind == 2) || (record.m_kind == 2 && courseKind == 1)) {
			continue;
		}

		sModel *pModel = ppModels[modelIndex++];
		pModel->m_subOrder = record.m_flags & 7;
		pModel->m_order = record.m_flags & 0xF8;
		pModel->m_aabb = record.m_aabb;
		pModel->m_sceneAabb = record.m_sceneAabb;
		pModel->m_matrix = record.m_matrix;
		pModel->m_basePos = (pModel->m_aabb.max + pModel->m_aabb.min) * 0.5f;
		pModel->m_center = (pModel->m_aabb.min + pModel->m_aabb.max) * 0.5f;
		pModel->m_radius = (pModel->m_aabb.max - pModel->m_aabb.min).Magnitude() * 0.5f;
		pModel->m_scale = sqrtf(record.m_matrix.xx * record.m_matrix.xx + record.m_matrix.zx * record.m_matrix.zx + record.m_matrix.yx * record.m_matrix.yx);
		strncpy(pModel->m_name, record.m_name, 0x1F);
		strncpy(pModel->m_baseName, record.m_baseName, 0x1F);

		pModel->m_pPuppet = g_resrcmng->GetPuppet(pModel->m_name, IsSpecialAdditionModel(pModel->m_name), pModel->m_subOrder != 0, false);
#ifdef PG_MATERIAL_HINTS
		PgSetModelHints(pModel->m_pPuppet, pModel->m_name);
#endif
		if (pModel->m_subOrder) {
			numInstances++;
		}

		if (pModel->m_pPuppet == 0) {
			if (m_pGolfDoc->m_ownedStrings.Find(pModel->m_name) == 0) {
				char *pName = new char[strlen(pModel->m_name) + 1];
				strcpy(pName, pModel->m_name);
				m_pGolfDoc->m_ownedStrings.AddItem(pName, pName, false);
			}
			continue;
		}

		pModel->m_pPuppet->ApplyBones(0, pModel->m_matrix);
		pModel->m_pPuppet->UpdateBound(true, false);
		pModel->m_pPuppet->UpdateLightSource(&g_lightset, false, 0);

		ProcessPostLoadPet(pModel);
#ifdef PG_MATERIAL_HINTS
		PgRegisterModelLamps(pModel->m_pPuppet);
#endif

		unsigned long flags = 0;
		if (pModel->m_subOrder) {
			flags = 4;
		}
		if ((pModel->m_order & 0x10) || pModel->m_pPuppet->GetBoundSphere().radius > 128.0f) {
			flags |= 2;
		}
		if (pModel->m_order & 8) {
			flags |= 1;
		}
		WSingleton<CSceneManager>::Instance()->RegisterElement((eObjType)((pModel->m_subOrder != 0) + 2), pModel->m_pPuppet, 0, pModel->m_name, flags, &pModel->m_sceneAabb);
	}

	if (m_numObjects > 1) {
		qsort(ppModels, m_numObjects, sizeof(sModel*), CompareModel);
	}
	for (int i = 0; i < m_numObjects; i++) {
		m_models.push_back(ppModels[i]);
	}
	delete[] ppModels;

	GetPVS().Reserve(numInstances);

	m_numBgFx = 0;
	for (int i = 0; i < m_numAreas; i++) {
		sArea *pArea = &m_pAreas[i];

		if (IsSilviaCannonFx((const char *)pArea)) {
			continue;
		}
		if (pArea->m_type != 6) {
			continue;
		}

		Attacher attacher;
		char fxName[0x40];
		WVector angles;
		WSliceStr slice = pArea->m_name;
		slice.Get("*fx", "%s %f %f %f", fxName, &angles.x, &angles.y, &angles.z);

		if (bPlaceHoleCup) {
			if (GetCurMap() == 4 || GetCurMap() == 5) {
				if (stricmp(fxName, "CL_Default.spr") == 0) {
					continue;
				}
			}
		}

		m_bgFx[m_numBgFx].m_pArea = pArea;
		m_bgFx[m_numBgFx].m_matrix.Reset();
		m_bgFx[m_numBgFx].m_matrix.Rotate(angles);
		m_bgFx[m_numBgFx].m_matrix.pivot = (pArea->m_aabb.min + pArea->m_aabb.max) * 0.5f;
		m_bgFx[m_numBgFx].m_offset.Reset();
		attacher.m_pMatrix[0] = &m_bgFx[m_numBgFx].m_matrix;
		attacher.m_pOffset[0] = &m_bgFx[m_numBgFx].m_offset;
		void *pFx = CFx::Instance()->Attach(attacher, fxName, false, 0);

		if (pFx == 0) {
			continue;
		}

		char *pExt = strrchr(fxName, '.');
		if (pExt == 0 || strlen(pExt) < 4) {
			continue;
		}
		pExt++;

		if (stricmp(pExt, "spr") == 0) {
			((CFxSpray *)pFx)->SetLight(g_lightset);
			m_bgFx[m_numBgFx].m_pSpray = (CFxSpray *)pFx;
		} else if (stricmp(pExt, "seq") == 0) {
			((CFxSequence *)pFx)->SetLight(g_lightset);
			m_bgFx[m_numBgFx].m_pSequence = (CFxSequence *)pFx;
		} else {
			continue;
		}

		m_numBgFx++;

		if (m_numBgFx >= 0x100) {
			break;
		}
	}

	m_bNoShadowmap = false;
	if (WSingleton<COption>::Instance()->vIsShadowmapEnabled()) {
		m_bNoShadowmap = !LoadShadowmapInfo();
	}

	CFx::Instance()->SetPvsFxBoxGroundHeight(m_pBgModel->m_aabb.max.y, m_pBgModel->m_aabb.min.y);

	CloseCFile(pFile);
	return true;
}

// polysoup.cpp:1069
// Applies the options of a placed model's bound boxes: day/night-only
// bones, the center box's wind and flags, ambient sound boxes, "abv" bones
// and the "*fx" effects attached to the boxes.
void WPolySoup::ProcessPostLoadPet(sModel *pModel) {
	for (w_bound_box *pBox = pModel->m_pPuppet->m_bbList.Start(); pBox; pBox = pModel->m_pPuppet->m_bbList.Next()) {
		char *pOption = pBox->info->option;

		if (_stricmp(pBox->info->name, "night_only") == 0) {
			if (GOLFDOC()->m_day) {
				if (pBox->bone && pBox->bone->GetChild()) {
					pBox->bone->GetChild()->SetInvisible(true, true);
				}
			}
		} else if (_stricmp(pBox->info->name, "day_only") == 0) {
			if (!GOLFDOC()->m_day) {
				if (pBox->bone && pBox->bone->GetChild()) {
					pBox->bone->GetChild()->SetInvisible(true, true);
				}
			}
		} else if (_stricmp(pBox->info->name, "center") == 0) {
			WSliceStr slice = pOption;
			slice.Get("*wind", "%f", &pModel->m_windFactor);
			slice.Get("*nocol", "%d", &pModel->m_reservedByte);

			if (pOption) {
				if (strstr(pOption, "*nofog")) {
					pModel->m_pPuppet->DisableFog();
				}
				if (strstr(pOption, "*blend")) {
					pModel->m_pPuppet->EnableAlphaBlend();
				}
			}
		} else if (_stricmp(pBox->info->name, "npc") != 0) {
			if (strstr(pBox->info->name, "sound")) {
				sSoundBox soundBox;
				soundBox.m_interval = -1.0f;
				soundBox.m_range = 60.0f;
				soundBox.m_elapsed = 0.0f;
				sscanf(pOption, "%s %f %f", soundBox.m_name, &soundBox.m_range, &soundBox.m_interval);

				soundBox.m_box = pBox->bone->GetMatrix();
				soundBox.m_box *= (pBox->info->aabb.max - pBox->info->aabb.min) * 0.5f;
				soundBox.m_box.center = pBox->info->spherePivot * pBox->bone->GetMatrix();

				m_soundBoxes.push_back(soundBox);
			} else if (strstr(_strlwr(pBox->info->name), "abv")) {
				pBox->bone->HideBone(false);
			}
		}

		if (pOption == 0) {
			continue;
		}
		char *pFx = strstr(pOption, "*fx");
		if (pFx == 0) {
			continue;
		}

		Attacher unusedAttacher;
		char key[8];
		char fxName[0x20] = "";
		char kind[0x20] = "";
		sscanf(pFx, "%s %s %s", key, fxName, kind);

		if (pBox->bone == 0 || pBox->bone->IsInvisible()) {
			continue;
		}

		char *pExt = strrchr(fxName, '.');
		if (pExt == 0 || strlen(pExt) < 4) {
			continue;
		}
		pExt++;
		if (_stricmp(pExt, "spr") != 0 && _stricmp(pExt, "seq") != 0) {
			continue;
		}

		if (_strcmpi(kind, "tree") == 0) {
			ApplyFx(pModel, pBox->bone->GetChild(), fxName);
			continue;
		}

		Attacher attacher;
		attacher.m_pMatrix[0] = &pBox->bone->GetMatrix();
		attacher.m_pOffset[0] = &pBox->info->spherePivot;

		defines_t defines;
		strcpy(defines.m_names[0], "pet_scale");
		defines.m_values[0] = pModel->m_scale;
		defines.m_count = 1;

		if (_stricmp(pExt, "spr") == 0) {
			CFx::Instance()->Attach(attacher, fxName, false, &defines);
		} else {
			CFx::Instance()->Attach(attacher, fxName, false, &defines);
		}
	}
}

// polysoup.cpp:1185
bool WPolySoup::IsResisteredBgSequence(const std::string &name) {
	for (int i = 0; i < m_numBgFx; i++) {
		if (m_bgFx[i].m_pSequence) {
			if (m_bgFx[i].m_pSequence->m_name == name) {
				return true;
			}
		}
	}
	return false;
}

// polysoup.cpp:1198
void WPolySoup::ApplyFx(sModel *pModel, WBone *pBone, const char *name) {
	w_bound_box *pBox = pModel->m_pPuppet->m_bbList.Find(pBone->GetBoneName());

	if (pBox) {
		Attacher attacher;
		attacher.m_pMatrix[0] = &pBox->bone->GetMatrix();
		attacher.m_pOffset[0] = &pBox->info->spherePivot;

		defines_t defines;
		strcpy(defines.m_names[0], "pet_scale");
		defines.m_values[0] = pModel->m_scale;
		defines.m_count = 1;

		if (stricmp(name, ".spr") == 0) {
			CFx::Instance()->Attach(attacher, name, false, &defines);
		} else {
			CFx::Instance()->Attach(attacher, name, false, &defines);
		}
	}

	if (pBone->GetChild()) {
		ApplyFx(pModel, pBone->GetChild(), name);
	}
	if (pBone->GetNext()) {
		ApplyFx(pModel, pBone->GetNext(), name);
	}
}

// polysoup.cpp:1226
void WPolySoup::MakeDataStructure(void) {
	GetPVS().SetPolySoup(this);

	GetPVS().MakeBaseStructure(m_pBgModel, false);

	for (short i = 0; i < m_numObjects; i++) {
		GetPVS().MakeObjStructure(i, -1, false);
	}
	m_numStructuredObjects = m_numObjects;
}

// polysoup.cpp:1238
void WPolySoup::CreateDataModel(sModel *pModel) {
	if (IsLocalContent((localContentType_t)0x56)) {
		pModel->m_objIndex = m_numStructuredObjects;

		m_models.push_back(pModel);

		GetPVS().MakeObjStructure(pModel->m_objIndex, m_numObjects, false);

		m_numStructuredObjects += 1;

		m_numObjects += 1;
	}
}

// polysoup.cpp:1254
void WPolySoup::ChangeDataStructure(sModel *pModel) {
	int index = 0;

	if (IsLocalContent((localContentType_t)0x56)) {
		for (std::vector<sModel*>::iterator it = m_models.begin(); it != m_models.end(); ++it, index++) {
			if (_stricmp((*it)->m_name, pModel->m_name) == 0) {
				*it = pModel;

				GetPVS().MakeObjStructure(pModel->m_objIndex, index, true);

				for (int i = 0; i < m_numObjects; i++) {
					GetPVS().MakeObjStructure(m_models[i]->m_objIndex, i, true);
				}
				return;
			}
		}
		CreateDataModel(pModel);
	}
}

// polysoup.cpp:1307
// Removes a model (found by name) from the quadtree and from the soup.
bool WPolySoup::DeleteDataStructure(sModel *pModel) {
	int numObjects = m_numObjects;
	int index = 0;
	bool result;
	const char *name;

	if (IsLocalContent((localContentType_t)0x56)) {
		std::vector<sModel*>::iterator it;
		for (it = m_models.begin(); it != m_models.end(); ++it, ++index) {
			name = pModel->m_name;
			if (stricmp((*it)->m_name, name) == 0) {
				break;
			}
		}
		if (it == m_models.end()) {
			result = false;
		} else {
			result = true;
			(*it)->m_pPuppet = 0;

			GetPVS().MakeBaseStructure(m_pBgModel, true);

			GetPVS().MakeObjStructure((*it)->m_objIndex, index, true);

			for (int i = 0; i < numObjects; i++) {
				GetPVS().MakeObjStructure(m_models[i]->m_objIndex, i, true);
			}

			for (it = m_models.begin(); it != m_models.end(); ++it) {
				if (stricmp((*it)->m_name, name) == 0) {
					break;
				}
			}
			if (it != m_models.end()) {
				if ((*it)->m_pTriangles != 0) {
					delete[] (*it)->m_pTriangles;
					(*it)->m_pTriangles = 0;
				}
				if ((*it)->m_pQuads != 0) {
					delete[] (*it)->m_pQuads;
					(*it)->m_pQuads = 0;
				}

				m_models.erase(it);

				m_numObjects--;
			}
		}
	} else {
		result = false;
	}
	return result;
}

// polysoup.cpp:1372
const sProperty &WPolySoup::GetProperty(int index) {
	return m_pGolfDoc->m_propertyInfo.GetProperty(index);
}

// polysoup.cpp:1377
int WPolySoup::GetPropertyIndex(const char *name) {
	return m_pGolfDoc->m_propertyInfo.GetPropertyIndex(name);
}

// polysoup.cpp:1382
void WPolySoup::EndLoad(bool bPlaceHoleCup) {
	ClearOldTextures();

	if (bPlaceHoleCup) {
		PlaceHoleCup();
	}
	xBuildTnLBuffers();
}

// polysoup.cpp:1395
void WPolySoup::ChangeFloorTexture(const char *oldName, const char *newName) {
	if (!IsLocalContent((localContentType_t)0x56)) {
		return;
	}

	if (oldName != 0 && newName != 0) {
		int oldHandle = g_resrcmng->FindTexture(oldName)->texhandle;
		int newHandle = g_resrcmng->LoadTexture(newName, 0, 0, 0);

		if (m_pBgModel->m_pPuppet->GetRootBone()->GetMesh() != 0) {
			m_pBgModel->m_pPuppet->ChangeTextureByHandle(newHandle, oldHandle, m_pBgModel->m_pPuppet);
		}
	}
}

// polysoup.cpp:1433
void WPolySoup::ChangeObjectTexture(const char *oldName, const char *newName, int index) {
	if (!IsLocalContent((localContentType_t)0x56)) {
		return;
	}

	if (oldName == 0 || newName == 0)
		return;

	int oldHandle;
	int newHandle;
	w_mesh *pMesh;

	if (index == -1) {
		oldHandle = g_resrcmng->FindTexture(oldName)->texhandle;
		newHandle = g_resrcmng->LoadTexture(newName, 0, 0, 0);
		pMesh = m_models[0]->m_pPuppet->GetRootBone()->GetMesh();

		if (pMesh == 0) {
			return;
		}
		pMesh = pMesh->next;
		if (pMesh == 0) {
			return;
		}
		m_models[0]->m_pPuppet->ChangeTextureByHandle(newHandle, oldHandle, m_models[0]->m_pPuppet);
	} else {
		int modelIndex = index;

		for (int i = 0; i < m_numObjects; i++) {
			if (m_models[i]->m_objIndex == index) {
				modelIndex = i;
				break;
			}
		}

		oldHandle = g_resrcmng->FindTexture(oldName)->texhandle;
		newHandle = g_resrcmng->LoadTexture(newName, 0, 0, 0);
		pMesh = m_models[modelIndex]->m_pPuppet->GetRootBone()->GetMesh();

		if (pMesh == 0) {
			return;
		}
		pMesh = pMesh->next;
		m_models[modelIndex]->m_pPuppet->ChangeTextureByHandle(newHandle, oldHandle, m_models[modelIndex]->m_pPuppet);
	}
}

// polysoup.cpp:1527
void WPolySoup::xBuildTnLBuffers(void) {
	if (m_bUseTnL) {
		m_pBgModel->m_pPuppet->xUpdateTnLBuffers(true);
		m_staticPetGrp.Build(m_models, m_numObjects);
	}
}

// polysoup.cpp:1536
// How opaque a model stays in front of the camera: 255 unless the camera is
// close to its box relative to its size (the ratio is clamped to [0.25, 1]).
unsigned char WPolySoup::OverlapCamera(int index) {
	if (m_models[index]->m_order & 8) {
		return 0xFF;
	}
	Waabb box = m_models[index]->m_aabb;

	float dx = box.max.x - box.min.x;
	float dy = box.max.y - box.min.y;
	float dz = box.max.z - box.min.z;
	float radius = sqrtf(dz * dz + dy * dy + dx * dx) * 0.5f;
	float maxRadius = radius > 96.0f ? 96.0f : radius;
	float distSq = 0.0f;

	for (int i = 0; i < 3; i++) {
		float d;

		if (g_camera.pivot.p[i] < box.min.p[i]) {
			d = g_camera.pivot.p[i] - box.min.p[i];
			distSq += d * d;
		} else if (g_camera.pivot.p[i] > box.max.p[i]) {
			d = g_camera.pivot.p[i] - box.max.p[i];
			distSq += d * d;
		}
	}

	float ratio = sqrtf(distSq) / maxRadius;
	return (unsigned char)((ratio < 0.25f ? 0.25f : (ratio > 1.0f ? 1.0f : ratio)) * 255.0f);
}

// polysoup.cpp:1579
// Whether pos (XZ) lies inside the polygon, by its winding number counted in
// quadrant steps (4 = once around).
bool WPolySoup::IsPointInPoly(const std::vector<WVector> &points, const WVector &pos) {
	int num = points.size();
	if (num < 3) {
		return false;
	}

	int winding = 0;

	for (int i = 0; i < num; i++) {
		WVector v1 = points[i];
		WVector v2 = points[(i + 1) % num];
		int q1 = v1.x > pos.x ? (v1.z > pos.z ? 1 : 4) : (v1.z > pos.z ? 2 : 3);
		int q2 = v2.x > pos.x ? (v2.z > pos.z ? 1 : 4) : (v2.z > pos.z ? 2 : 3);
		int delta = q2 - q1;

		switch (delta) {
		case 2:
		case -2:
			if (v2.x - (v1.x - v2.x) * (v2.z - pos.z) / (v1.z - v2.z) > pos.x) {
				delta = -delta;
			}
			break;
		case 3:
			delta = -1;
			break;
		case -3:
			delta = 1;
			break;
		}
		winding += delta;
	}

	return (winding > 0 ? winding : -winding) == 4;
}

// polysoup.cpp:1616
bool WPolySoup::IsOutOfBound(const WVector &pos) {
	for (std::vector<CNodeContainer>::iterator it = m_boundAreas.begin(); it != m_boundAreas.end(); ++it) {
		int type = it->m_type;

		if (type == 3 || type == 1) {
			if (!IsPointInPoly(it->m_points, pos)) {
				return true;
			}
		} else if (type == 2) {
			if (IsPointInPoly(it->m_points, pos)) {
				return true;
			}
		}
	}

	return GetPVS().IsOBArea(pos) ? true : false;
}

// polysoup.cpp:1641
WVector WPolySoup::GetCameraPos(const WVector &pos, const WVector &dir) {
	WVector result = pos;

	for (std::vector<CNodeContainer>::iterator it = m_boundAreas.begin(); it != m_boundAreas.end(); ++it) {
		int type = it->m_type;

		if ((HaveCameraArea() && type == 6) || (!HaveCameraArea() && type == 1)) {
			if (!IsPointInPoly(it->m_points, pos)) {
				result = PosInWorld(it->m_points, pos, dir);
			}
			break;
		}
	}

	return result;
}

// polysoup.cpp:1662
// Pulls the camera back inside the bound area: the distance from dir towards
// pos is clamped by the radius at the first edge the XZ direction crosses.
WVector WPolySoup::PosInWorld(const std::vector<WVector> &points, const WVector &pos, const WVector &dir) {
	int numPoints = points.size();

	WVector toPos;
	toPos = pos - dir;
	toPos.y = 0.0f;
	float dist = sqrtf(WDotProduct(toPos, toPos));

	if (IsClockWise(points, dir)) {
		for (int i = 0; i < numPoints; i++) {
			WVector cur;
			cur = points[i] - dir;
			cur.y = 0.0f;
			if (WDotProduct(cur, toPos) < 0.0f)
				continue;
			WVector crossCur;
			crossCur = WCrossProduct(toPos, cur);

			WVector next;
			next = points[(i + 1) % numPoints] - dir;
			next.y = 0.0f;
			if (WDotProduct(next, toPos) < 0.0f)
				continue;
			WVector crossNext;
			crossNext = WCrossProduct(toPos, next);

			if (crossCur.y < 0.0f && crossNext.y > 0.0f) {
				float radius = GetCameraRadius(cur, next, toPos);
				if (dist > radius)
					dist = radius;
				break;
			}
		}
	} else {
		for (int i = 0; i < numPoints; i++) {
			WVector cur;
			cur = points[i] - dir;

			if (WDotProduct(cur, toPos) < 0.0f)
				continue;

			WVector crossCur;
			crossCur = WCrossProduct(toPos, cur);

			WVector next;
			next = points[(i + 1) % numPoints] - dir;
			next.y = 0.0f;

			if (WDotProduct(next, toPos) < 0.0f)
				continue;

			WVector crossNext;
			crossNext = WCrossProduct(toPos, next);

			if (crossCur.y > 0.0f && crossNext.y < 0.0f) {
				float radius = GetCameraRadius(cur, next, toPos);
				if (dist > radius)
					dist = radius;
				break;
			}
		}
	}

	toPos.Normalize();
	toPos *= dist;

	return toPos + dir;
}

// polysoup.cpp:1739
// The radius of the circle through the camera's corner on the XZ plane: a
// and b are the sides from the corner, c the direction between them.
float WPolySoup::GetCameraRadius(WVector a, WVector b, WVector c) {
	a.y = 0.0f;
	b.y = 0.0f;
	c.y = 0.0f;

	float angle[2];
	float lenA = sqrtf(a.x * a.x + a.z * a.z);
	float lenB = sqrtf(b.x * b.x + b.z * b.z);
	a.Normalize();
	b.Normalize();
	c.Normalize();

	float cosA = a.x * c.x + a.y * c.y + a.z * c.z;
	angle[0] = acosf(cosA < -1.0f ? -1.0f : (cosA > 1.0f ? 1.0f : cosA));
	float cosB = b.x * c.x + b.y * c.y + b.z * c.z;
	angle[1] = acosf(cosB < -1.0f ? -1.0f : (cosB > 1.0f ? 1.0f : cosB));

	float sinSum = sinf(angle[0] + angle[1]);

	return sinSum * lenB * lenA / (sinf(angle[0]) * lenA + sinf(angle[1]) * lenB);
}

// polysoup.cpp:1766
bool WPolySoup::IsClockWise(const std::vector<WVector> &points, const WVector &pos) {
	WVector2D a(points[1].x - pos.x, points[1].z - pos.z), b(points[0].x - pos.x, points[0].z - pos.z);

	if (a.x * b.y - b.x * a.y > 0.0f) {
		return true;
	}
	return false;
}

// polysoup.cpp:1776
WPolySoup::sCamera *WPolySoup::FindCamera(const char *name) {
	if (name == 0 || name[0] == '\0') {
		return 0;
	}
	for (int i = 0; i < m_numCameras; i++) {
		if (_strcmpi(m_pCameras[i].m_name, name) == 0) {
			return &m_pCameras[i];
		}
	}
	return 0;
}

// polysoup.cpp:1790
WPolySoup::sPoint *WPolySoup::FindPoint(const char *name) {
	if (name == 0 || name[0] == '\0') {
		return 0;
	}
	for (int i = 0; i < m_numPoints; i++) {
		if (_strcmpi(m_pPoints[i].m_name, name) == 0) {
			return &m_pPoints[i];
		}
	}
	return 0;
}

// polysoup.cpp:1804
WPolySoup::sArea *WPolySoup::FindArea(const char *name) {
	if (name == 0 || name[0] == '\0') {
		return 0;
	}
	for (int i = 0; i < m_numAreas; i++) {
		if (strstr(m_pAreas[i].m_name, name) != 0) {
			return &m_pAreas[i];
		}
	}
	return 0;
}

// polysoup.cpp:1818
WPolySoup::sModel *WPolySoup::FindModel(const char *name) {
	if (name == 0 || name[0] == '\0') {
		return m_pBgModel;
	}
	for (int i = 0; i < GetModelNum(); i++) {
		if (_strcmpi(m_models[i]->m_name, name) == 0) {
			return m_models[i];
		}
	}
	return 0;
}

// polysoup.cpp:1837
CNodeContainer *WPolySoup::FindNode(const char *name) {
	if (name == 0 || *name == 0) {
		return 0;
	}

	for (std::vector<CNodeContainer>::iterator it = m_boundAreas.begin(); it != m_boundAreas.end(); ++it) {
		if (strcmpi(it->GetName().c_str(), name) == 0) {
			return &*it;
		}
	}

	return 0;
}

// polysoup.cpp:1852
WPolySoup::sArea *WPolySoup::GetAreaByPos(const WVector &pos) {
	for (int i = 0; i < m_numAreas; i++) {
		if (strstr(m_pAreas[i].m_name, "*cam") != 0) {
			if (m_pAreas[i].m_aabb.IsInclude(pos)) {
				return &m_pAreas[i];
			}
		}
	}

	return 0;
}

// polysoup.cpp:1867
WPolySoup::sCamera *WPolySoup::GetCameraByName(const char *name) {
	for (int i = 0; i < m_numCameras; i++) {
		if (_stricmp(m_pCameras[i].m_name, name) == 0) {
			return &m_pCameras[i];
		}
	}
	return 0;
}

// polysoup.cpp:1879
w_bound_box *WPolySoup::GetObjBoundBox(int index, const char *name) {
	if (index == 0xFFFF) {
		return m_pBgModel->m_pPuppet->m_bbList.Find(name);
	}
	return m_models[index]->m_pPuppet->m_bbList.Find(name);
}

// polysoup.cpp:1892
int WPolySoup::GetObjAlpha(int index) {
	if (index == 0xFFFF) {
		return 0xFF;
	}
	return m_models[index]->m_alpha;
}

// polysoup.cpp:1905
float WPolySoup::GetObjWindFactor(int index) {
	if (index == 0xFFFF) {
		return m_pBgModel->m_windFactor;
	}
	return m_models[index]->m_windFactor;
}

// polysoup.cpp:1919
void WPolySoup::UpdateBgFxWind(void) {
	CFx::Instance()->UpdateExtWind(Wind().GetGlobalWind());

	for (int i = 0; i < m_numBgFx; i++) {
		if (m_bgFx[i].m_pSpray) {
			m_bgFx[i].m_pSpray->SetWind(Wind().GetWind(m_bgFx[i].m_pSpray->m_pos));
		}
		if (m_bgFx[i].m_pSequence) {
			if (CFx::Instance()->FindSequence(m_bgFx[i].m_pSequence)) {
				m_bgFx[i].m_pSequence->SetWind(Wind().GetWind(m_bgFx[i].m_pSequence->Pos()));
			}
		}
	}
}

// polysoup.cpp:1938
// Places an additional model in the next free slot of m_models, taking its
// orientation, scale and base name from the fifth model.
void WPolySoup::SetAdditionModel(const char *name, const WVector &pos) {
	if (!IsLocalContent((localContentType_t)0x56)) {
		return;
	}

	if (name == 0) {
		return;
	}

	sModel *pModel = m_models[m_numObjects++];

	pModel->m_subOrder = 0;
	pModel->m_order = 0;

	pModel->m_matrix = m_models[4]->m_matrix;

	pModel->m_matrix.xm = pos.x;
	pModel->m_matrix.ym = pos.y;
	pModel->m_matrix.zm = pos.z;

	pModel->m_basePos = (pModel->m_aabb.min + pModel->m_aabb.max) * 0.5f;
	pModel->m_center = pos * 0.5f;
	pModel->m_radius = (pModel->m_aabb.max - pModel->m_aabb.min).Magnitude() * 0.5f;
	pModel->m_scale = m_models[4]->m_scale;
	strncpy(pModel->m_name, name, 0x1F);
	strncpy(pModel->m_baseName, m_models[4]->m_baseName, 0x1F);

	pModel->m_pPuppet = g_resrcmng->GetPuppet(pModel->m_name, IsSpecialAdditionModel(pModel->m_name), pModel->m_subOrder != 0, false);
#ifdef PG_MATERIAL_HINTS
	PgSetModelHints(pModel->m_pPuppet, pModel->m_name);
#endif

	w_bound_box *pBox = pModel->m_pPuppet->m_bbList.Find("char0");
	if (pBox) {
		pModel->m_obb = pBox->bone->GetMatrix();
		pModel->m_obb *= (pBox->info->aabb.max - pBox->info->aabb.min) * 0.5f + WVector::ONE * 1.6f;
		pModel->m_obb.center = pBox->info->spherePivot * pBox->bone->GetMatrix();
		g_view->DrawOBB(pModel->m_obb, 0xFF00FF00);
	}

	if (m_pGolfDoc->m_ownedStrings.Find(pModel->m_name) == 0) {
		char *pName = new char[strlen(pModel->m_name) + 1];
		strcpy(pName, pModel->m_name);
		m_pGolfDoc->m_ownedStrings.AddItem(pName, pName, false);
	}

	pModel->m_pPuppet->ApplyBones(0, pModel->m_matrix);
	pModel->m_pPuppet->UpdateBound(true, false);
	pModel->m_pPuppet->UpdateLightSource(&g_lightset, false, 0);

	ProcessPostLoadPet(pModel);
#ifdef PG_MATERIAL_HINTS
	PgRegisterModelLamps(pModel->m_pPuppet);
#endif

	unsigned long flags = 0;
	if (pModel->m_subOrder) {
		flags = 4;
	}
	if ((pModel->m_order & 0x10) || pModel->m_pPuppet->GetBoundSphere().radius > 128.0f) {
		flags |= 2;
	}
	if (pModel->m_order & 8) {
		flags |= 1;
	}
	WSingleton<CSceneManager>::Instance()->RegisterElement((eObjType)((pModel->m_subOrder != 0) + 2), pModel->m_pPuppet, 0, pModel->m_name, flags, &pModel->m_sceneAabb);
}

// polysoup.cpp:2075
void WPolySoup::SetRenderMode(int mode) {
	for (int i = 0; i < m_numObjects; i++) {
		sModel *pModel = m_models[i];

		if (pModel->m_pPuppet) {
			pModel->m_pPuppet->SetRenderMode(mode);
		}
	}
	m_pBgModel->m_pPuppet->SetRenderMode(mode);
}

// direct.h is not in the toolchain's include directory; the CRT export.
extern "C" __declspec(dllimport) int __cdecl mkdir(const char *dirname);

// polysoup.cpp:2095
// Writes the face-count distribution of the course's objects to
// statistics/<map>_<hole>_stat.txt.
void WPolySoup::MakeStatistics(void) {
	mkdir("statistics");
	FILE *fp = fopen(MakeStr("statistics/%s_%02d_stat.txt", GetCurMapName(), GOLFDOC()->m_currentHole), "wt");

	if (fp == 0)
		return;

	int total = 0;
	int maxFaces = 0;
	int minFaces = 1000;
	int counts[11];

	memset(counts, 0, sizeof(counts));

	int i;
	for (i = 0; i < m_numObjects; i++) {
		sModel *model = m_models[i];
		int faces = model->m_pPuppet->GetFaceNum();
		int slot = faces / 100;
		if (slot > 10)
			slot = 10;
		int &count = counts[slot];
		count++;

		total += model->m_pPuppet->GetFaceNum();
		if (model->m_pPuppet->GetFaceNum() >= maxFaces) maxFaces = model->m_pPuppet->GetFaceNum();
		if (faces <= minFaces) minFaces = faces;
	}

	fprintf(fp, "*[%s_%02d]\xC0\xC7 \xBF\xC0\xBA\xEA\xC1\xA7\xC6\xAE \xB8\xDE\xBD\xAC \xBA\xD0\xC6\xF7\xB5\xB5\n\n", GetCurMapName(), GOLFDOC()->m_currentHole);

	for (i = 0; i < 11; i++) {
		if (i == 10) {
			fprintf(fp, "%3d~    : ", 1000);
		} else {
			fprintf(fp, "%3d~%3d : ", i * 100, i * 100 + 100);
		}

		for (int j = 0; j < counts[i]; j++)
			fprintf(fp, "*");

		fprintf(fp, "(%d)\n", counts[i]);
	}

	int average = total / m_numObjects;

	int sumSquares = 0;

	for (i = 0; i < m_numObjects; i++) {
		int faces = m_models[i]->m_pPuppet->GetFaceNum();
		sumSquares += faces * faces;
	}

	int variance = (sumSquares - m_numObjects * average * average) / (m_numObjects - 1);

	fprintf(fp, "\n\xC7\xD5\xB0\xE8   : %d(%d)\n", total + m_pBgModel->m_pPuppet->GetFaceNum(), m_numObjects);
	fprintf(fp, "\xC6\xF2\xB1\xD5\xB0\xAA : %d\n", average);
	fprintf(fp, "\xC3\xD6\xBC\xD2\xB0\xAA : %d\n", minFaces);
	fprintf(fp, "\xC3\xD6\xB4\xEB\xB0\xAA : %d\n", maxFaces);
	fprintf(fp, "\xC7\xA5\xC1\xD8\xC6\xED\xC2\xF7 : %.0f\n", sqrt((double)variance));
	fprintf(fp, "\xC5\xD8\xBD\xBA\xC3\xC4\xB0\xB9\xBC\xF6 : %d\n", m_numTextures);

	fclose(fp);
}

// polysoup.cpp:2166
// Interpolates a value over a triangle from its barycentric weights
// (sub_006b8aa0). The original has no reference to it; cl drops it until a
// caller is found.
static float InterpolateTriangle(float v0, float v1, float v2, float u, float v) {
	float value = (v1 - v0) * u + (v2 - v0) * v;

	return value + v0;
}

// polysoup.cpp:2175
// The barycentric weights of pos in the triangle (a, b, c), on the XZ plane
// (sub_006ba820). Inlined in GenNewHolecupVtx; smBuildOutFragMeshes calls it
// only from a branch it never takes (see there). pt[3][2] gives the original's 0x1C frame (the whole array is
// homed once pt[0][0] spills off the FPU stack).
static void GetBarycentric(const WVector &a, const WVector &b, const WVector &c, const WVector &pos, float &u, float &v) {
	float pt[3][2];

	pt[0][0] = a.x;
	pt[0][1] = a.z;
	pt[0][0] -= pos.x;
	pt[0][1] -= pos.z;
	pt[1][0] = b.x - pos.x;
	pt[1][1] = b.z - pos.z;
	pt[2][0] = c.x - pos.x;
	pt[2][1] = c.z - pos.z;

	u = (pt[0][0] * pt[2][1] - pt[2][0] * pt[0][1]) / (pt[1][0] * pt[2][1] - pt[2][0] * pt[1][1]);
	v = (pt[1][0] * pt[0][1] - pt[1][1] * pt[0][0]) / (pt[1][0] * pt[2][1] - pt[2][0] * pt[1][1]);
}

// polysoup.cpp:2193
// The barycentric weights of pos in the triangle (a, b, c), projected on the
// plane the triangle's normal is closest to (sub_006ba880, register
// arguments). u weighs c, v weighs b. Taking c before b reproduces the
// original's registers (ecx a, edx b, esi c, edi pos). Emitted once its callers
// smBuildOutFragMeshes and smAddSmapPack are written.
static void GetBarycentricProjected(const WVector &a, const WVector &c, const WVector &b, const WVector &pos, float &u, float &v) {
	WVector normal;
	normal = WCrossProduct(c - a, b - a);
	float px, py, cx, cy, bx, by;

	float absX = Abs(normal.x);
	float absY = Abs(normal.y);
	float absZ = Abs(normal.z);

	if (absX >= absY && absX >= absZ) {
		px = pos.z;
		py = pos.y;
		px -= a.z;
		py -= a.y;
		cx = c.z - a.z;
		cy = c.y - a.y;
		bx = b.z - a.z;
		by = b.y - a.y;
	} else if (absY >= absX && absY >= absZ) {
		px = pos.x;
		py = pos.z;
		px -= a.x;
		py -= a.z;
		cx = c.x - a.x;
		cy = c.z - a.z;
		bx = b.x - a.x;
		by = b.z - a.z;
	} else {
		px = pos.x;
		py = pos.y;
		px -= a.x;
		py -= a.y;
		cx = c.x - a.x;
		cy = c.y - a.y;
		bx = b.x - a.x;
		by = b.y - a.y;
	}

	u = (by * px - bx * py) / (by * cx - bx * cy);
	v = (cx * py - cy * px) / (by * cx - bx * cy);
}

#ifdef PG_7XX
// PG_7XX: the bone whose meshes the shadow-map packs belong to while a
// later client's .sbin is read (PgLoadShadowmapBone); 0: the root, as in the
// 2011 client, where PG_SMAP_BONE() is the root bone itself.
static WBone *s_pgSmapBone = 0;
#define PG_SMAP_BONE() (s_pgSmapBone ? s_pgSmapBone : m_pBgModel->m_pPuppet->GetRootBone())
#else
#define PG_SMAP_BONE() m_pBgModel->m_pPuppet->GetRootBone()
#endif

// polysoup.cpp:2248
// Loads the shadow map of the background (<map>.sbin): the file must match
// the background's meshes (their count, vertex and index counts).
bool WPolySoup::LoadShadowmapInfo(void) {
	char name[0x40] = { 0 };

	char *pExt = strrchr(m_mapName, '.');
	if (pExt == 0 || pExt - m_mapName >= 0x3A) {
		return false;
	}
	strncpy(name, m_mapName, pExt - m_mapName);
	strcat(name, ".sbin");

	cFile *pFile = GetResrcManager()->GetCFile(name, -1);
	if (pFile == 0) {
		return false;
	}

#ifdef PG_7XX
	// The later clients' .sbin (the JP 983 exe's reader, 0x9473D0): a tag
	// 0xFFFE<major><minor>, from 1.0 on followed by groups of a bone name
	// and that bone's shadow map in the 2011 layout (sub_946CD0, the body
	// below with the bone in place of the root). Without the tag, the 2011
	// file. A name no bone has ends the groups, keeping what was loaded.
	unsigned int tag = 0;
	pFile->Read(&tag, 4);
	if ((tag & 0xFFFF0000) == 0xFFFE0000 && tag >= 0xFFFE0100) {
		bool loaded = true;
		int numGroups = 0;
		pFile->Read(&numGroups, 4);
		for (int group = 0; group < numGroups && loaded; group++) {
			int nameLength = 0;
			pFile->Read(&nameLength, 4);
			char boneName[0x104];
			int stored = nameLength < 0x103 ? nameLength : 0x103;
			pFile->Read(boneName, stored);
			boneName[stored] = 0;
			if (nameLength > stored) {
				pFile->Seek(nameLength - stored, 1);
			}

			WBone *pBone = m_pBgModel->m_pPuppet->FindBone(boneName);
			if (pBone == 0 || pBone->GetMesh() == 0) {
				break;
			}
			// m_pSmapMeshes and its draw flags keep the last group's packs.
			if (group > 0) {
				delete[] m_pSmapDrawFlags;
				m_pSmapDrawFlags = 0;
			}
			loaded = PgLoadShadowmapBone(pFile, pBone);
		}
		CloseCFile(pFile);
		return loaded;
	}
	pFile->Seek(-4, 1);
#endif

	w_mesh *pFirst = m_pBgModel->m_pPuppet->GetRootBone()->GetMesh();
	w_mesh *pMesh;
	int numMeshes = 0;
	for (pMesh = pFirst; pMesh; pMesh = pMesh->next) {
		numMeshes++;
	}

	w_mesh **ppMeshes = new w_mesh*[numMeshes];

	int i = 0;
	for (pMesh = pFirst; pMesh; pMesh = pMesh->next, i++) {
		ppMeshes[i] = pMesh;
	}

	pFile->Read(&i, 4);

	if (i != numMeshes) {
		delete[] ppMeshes;
		CloseCFile(pFile);
		return false;
	}

	if (numMeshes == 0) {
		return true;
	}

	for (i = 0; i < numMeshes; i++) {
		int vtxNum;
		int indexNum;
		pFile->Read(&vtxNum, 4);
		pFile->Read(&indexNum, 4);

		if (vtxNum != ppMeshes[i]->vtxNum || indexNum != ppMeshes[i]->indexNum) {
			delete[] ppMeshes;
			CloseCFile(pFile);
			return false;
		}
	}

	int numUV = 0;
	int numVtx = 0;
	pFile->Read(&numVtx, 4);
	pFile->Read(&numUV, 4);

	if (numVtx == 0 || numUV == 0) {
		delete[] ppMeshes;
		CloseCFile(pFile);
		return false;
	}

	WVector *pVtx = new WVector[numVtx];
	pFile->Read(pVtx, numVtx * sizeof(WVector));

	float (*pUV)[2] = new float[numUV][2];
	pFile->Read(pUV, numUV * sizeof(float[2]));

	SmCase cases[2];

	smLoadSmapMeshes(cases, pFile, ppMeshes, pVtx, pUV);
	smLoadOutFrags(pFile, numMeshes, ppMeshes, pVtx, cases);

	if (pVtx) {
		delete[] pVtx;
	}
	if (pUV) {
		delete[] pUV;
	}

	delete[] ppMeshes;

	int maxVtx = 0;
	for (pMesh = pFirst; pMesh; pMesh = pMesh->next) {
		if (maxVtx < pMesh->vtxNum) {
			maxVtx = pMesh->vtxNum;
		}
	}

	m_pBgModel->m_pPuppet->GetRootBone()->AllocVTX(maxVtx);

	CloseCFile(pFile);

	return true;
}

#ifdef PG_7XX
// LoadShadowmapInfo's body for one bone of a later client's .sbin: that
// bone's meshes must match, the packs join its mesh list. The file stays open.
bool WPolySoup::PgLoadShadowmapBone(cFile *pFile, WBone *pBone) {
	w_mesh *pFirst = pBone->GetMesh();
	w_mesh *pMesh;
	int numMeshes = 0;
	for (pMesh = pFirst; pMesh; pMesh = pMesh->next) {
		numMeshes++;
	}

	w_mesh **ppMeshes = new w_mesh*[numMeshes];

	int i = 0;
	for (pMesh = pFirst; pMesh; pMesh = pMesh->next, i++) {
		ppMeshes[i] = pMesh;
	}

	pFile->Read(&i, 4);

	if (i != numMeshes) {
		delete[] ppMeshes;
		return false;
	}

	if (numMeshes == 0) {
		return true;
	}

	for (i = 0; i < numMeshes; i++) {
		int vtxNum;
		int indexNum;
		pFile->Read(&vtxNum, 4);
		pFile->Read(&indexNum, 4);

		if (vtxNum != ppMeshes[i]->vtxNum || indexNum != ppMeshes[i]->indexNum) {
			delete[] ppMeshes;
			return false;
		}
	}

	int numUV = 0;
	int numVtx = 0;
	pFile->Read(&numVtx, 4);
	pFile->Read(&numUV, 4);

	if (numVtx == 0 || numUV == 0) {
		delete[] ppMeshes;
		return false;
	}

	WVector *pVtx = new WVector[numVtx];
	pFile->Read(pVtx, numVtx * sizeof(WVector));

	float (*pUV)[2] = new float[numUV][2];
	pFile->Read(pUV, numUV * sizeof(float[2]));

	SmCase cases[2];

	s_pgSmapBone = pBone;
	smLoadSmapMeshes(cases, pFile, ppMeshes, pVtx, pUV);
	smLoadOutFrags(pFile, numMeshes, ppMeshes, pVtx, cases);
	s_pgSmapBone = 0;

	if (pVtx) {
		delete[] pVtx;
	}
	if (pUV) {
		delete[] pUV;
	}

	delete[] ppMeshes;

	int maxVtx = 0;
	for (pMesh = pFirst; pMesh; pMesh = pMesh->next) {
		if (maxVtx < pMesh->vtxNum) {
			maxVtx = pMesh->vtxNum;
		}
	}

	pBone->AllocVTX(maxVtx);

	return true;
}
#endif

// polysoup.cpp:2392
// Reads the shadow-map packs of the two cases and appends their meshes to
// the background's mesh list; the second case and the skipped blocks depend
// on the current course's kind.
void WPolySoup::smLoadSmapMeshes(SmCase *pCases, cFile *pFile, w_mesh **ppMeshes, WVector *pVtx, float (*pUV)[2]) {
	int i;
	int size;

	w_mesh *pLast = PG_SMAP_BONE()->GetMesh();

	while (pLast && pLast->next) {
		pLast = pLast->next;
	}

	pFile->Read(&size, 4);

	w_mesh *pFirst = 0;
	if (size > 0) {
		pFile->Read(&pCases[0].m_numLists, 4);

		if (pCases[0].m_numLists > 0) {
			pCases[0].m_pLists = new SmInFragList[pCases[0].m_numLists];
		}

		for (i = 0; i < pCases[0].m_numLists; i++) {
			w_mesh *pNew = smAddSmapPack(&pCases[0].m_pLists[i], pFile, 0, i, ppMeshes, pVtx, pUV);
			pLast->next = pNew;
			pLast = pNew;
			if (i == 0) {
				pFirst = pNew;
			}
		}
	}

	switch (CSharedDoc::Instance()->m_pCurCourse == 0 ? (unsigned char)0 : (unsigned char)(CSharedDoc::Instance()->m_pCurCourse->courseFlags >> 4)) {
	case 0:
		pFile->Read(&size, 4);
		pFile->Seek(size, 1);
		pFile->Read(&size, 4);
		pFile->Seek(size, 1);
		break;
	case 2:
		pFile->Read(&size, 4);
		pFile->Seek(size, 1);
	case 1:
		pFile->Read(&size, 4);
		if (size > 0) {
			pFile->Read(&pCases[1].m_numLists, 4);
		}
		break;
	}

	// The original sizes the second case's lists by the first case's count.
	if (pCases[1].m_numLists > 0) {
		pCases[1].m_pLists = new SmInFragList[pCases[0].m_numLists];
	}

	for (i = 0; i < pCases[1].m_numLists; i++) {
		w_mesh *pNew = smAddSmapPack(&pCases[1].m_pLists[i], pFile, CSharedDoc::Instance()->m_pCurCourse == 0 ? (unsigned char)0 : (unsigned char)(CSharedDoc::Instance()->m_pCurCourse->courseFlags >> 4), i, ppMeshes, pVtx, pUV);
		pLast->next = pNew;
		pLast = pNew;
	}

	if ((CSharedDoc::Instance()->m_pCurCourse == 0 ? (unsigned char)0 : (unsigned char)(CSharedDoc::Instance()->m_pCurCourse->courseFlags >> 4)) == 1) {
		pFile->Read(&size, 4);
		pFile->Seek(size, 1);
	}

	m_pSmapMeshes = pFirst;

	for (m_numSmapMeshes = 0; pFirst; pFirst = pFirst->next) {
		m_numSmapMeshes++;
	}

	m_pSmapDrawFlags = new int[m_numSmapMeshes];

	i = 0;
	for (w_mesh *pMesh = m_pSmapMeshes; pMesh; pMesh = pMesh->next) {
		m_pSmapDrawFlags[i++] = pMesh->drawFlag & 0x7FF;
	}
}

// The header of a DDS file (the layout of DirectDraw's DDSURFACEDESC2, size
// 0x7C) smAddSmapPack builds in front of a shadow-map texture.
struct SmDdsHeader {
	unsigned long m_size;                 // 0x00
	unsigned long m_flags;                // 0x04
	unsigned long m_height;               // 0x08
	unsigned long m_width;                // 0x0C
	unsigned long m_linearSize;           // 0x10
	UNKNOWN(0x14, 0x18);
	unsigned long m_mipMapCount;          // 0x18
	UNKNOWN(0x1C, 0x48);
	unsigned long m_formatSize;           // 0x48, the pixel format's size
	unsigned long m_formatFlags;          // 0x4C
	unsigned long m_fourCC;               // 0x50
	UNKNOWN(0x54, 0x68);
	unsigned long m_caps;                 // 0x68
	UNKNOWN(0x6C, 0x7C);
};
CHECK_SIZE(SmDdsHeader, 0x7C);
CHECK_FIELD(SmDdsHeader, m_size, 0x00);
CHECK_FIELD(SmDdsHeader, m_flags, 0x04);
CHECK_FIELD(SmDdsHeader, m_height, 0x08);
CHECK_FIELD(SmDdsHeader, m_width, 0x0C);
CHECK_FIELD(SmDdsHeader, m_linearSize, 0x10);
CHECK_FIELD(SmDdsHeader, m_mipMapCount, 0x18);
CHECK_FIELD(SmDdsHeader, m_formatSize, 0x48);
CHECK_FIELD(SmDdsHeader, m_formatFlags, 0x4C);
CHECK_FIELD(SmDdsHeader, m_fourCC, 0x50);
CHECK_FIELD(SmDdsHeader, m_caps, 0x68);

// polysoup.cpp:2496
// Reads a shadow-map pack: its DXT1 texture, then its fragments, and builds
// a mesh of them; vertices equal in position, alpha and UV are shared.
w_mesh *WPolySoup::smAddSmapPack(SmInFragList *pList, cFile *pFile, int kind, int index, w_mesh **ppMeshes, WVector *pVtx, float (*pUV)[2]) {
	int width;
	int height;
	pFile->Read(&width, 4);
	pFile->Read(&height, 4);

	SmDdsHeader desc;
	memset(&desc, 0, sizeof(desc));
	desc.m_size = sizeof(desc);
	desc.m_flags = 0x80000;               // DDSD_LINEARSIZE
	desc.m_height = height;
	desc.m_width = width;
	desc.m_linearSize = height * width / 2;
	desc.m_mipMapCount = 1;
	desc.m_formatSize = 0x20;
	desc.m_formatFlags = 4;               // DDPF_FOURCC
	desc.m_fourCC = 0x31545844;           // "DXT1"
	desc.m_caps = 0x1000;                 // DDSCAPS_TEXTURE

	int size = desc.m_linearSize + 0x80;
	unsigned char *pBuf = new unsigned char[size];
	memcpy(pBuf + 4, &desc, sizeof(desc));
	pBuf[0] = 'D';
	pBuf[1] = 'D';
	pBuf[2] = 'S';
	pBuf[3] = ' ';
	pFile->Read(pBuf + 0x80, desc.m_linearSize);

	char name[0x20];
	sprintf(name, "%s-%d-%d.dds", m_pBgModel->m_name, kind, index);
	int texHandle = g_resrcmng->LoadTexture_DDS(pBuf, size, name, 0, 0);
	delete[] pBuf;

	int numFrags;
	pFile->Read(&numFrags, 4);
	SmInFrag *pFrags = new SmInFrag[numFrags];
	pList->m_numFrags = numFrags;
	pList->m_pFrags = pFrags;
	pFile->Read(pFrags, numFrags * sizeof(SmInFrag));

	w_mesh *pMesh = new w_mesh;
	memset(pMesh, 0, sizeof(w_mesh));

	pMesh->texHandle = texHandle;

	pMesh->drawFlag = (texHandle & 0x7FF) | 0x21100000;
	pMesh->diffuse = 0xFFFFFF;
	pMesh->alpha = 1.0f;

	pMesh->indexNum = numFrags * 3;
	pMesh->indexList = new unsigned short[pMesh->indexNum];
	WVector *pVerts = new WVector[pMesh->indexNum];
	float (*pUVs)[2] = new float[pMesh->indexNum][2];
	unsigned long *pColors = new unsigned long[pMesh->indexNum];
	int numVtx = 0;

	for (int i = 0; i < numFrags; i++) {
		for (int j = 0; j < 3; j++) {
			int vtx = pFrags[i].m_vtx[j];
			int uv = pFrags[i].m_uv[j];
			int k;

			if (vtx < 0x8000) {
				w_mesh *pSrc = ppMeshes[pFrags[i].m_meshIndex];
				unsigned short *pIdx = &pSrc->indexList[pFrags[i].m_triIndex * 3];
				unsigned short i0 = pIdx[0];
				unsigned short i1 = pIdx[1];
				unsigned short i2 = pIdx[2];
				float u, v;
				GetBarycentricProjected(pSrc->vecList[i0], pSrc->vecList[i1], pSrc->vecList[i2], pVtx[vtx], u, v);
				unsigned long color = ((unsigned char)InterpolateTriangle((float)(pSrc->vtxColorList[i0] >> 24), (float)(pSrc->vtxColorList[i1] >> 24), (float)(pSrc->vtxColorList[i2] >> 24), u, v) << 24) | 0xFFFFFF;

				for (k = 0; k < numVtx; k++) {
					if (WisEqual(pVerts[k].x, pVtx[vtx].x, data_a111e4) && WisEqual(pVerts[k].y, pVtx[vtx].y, data_a111e4) && WisEqual(pVerts[k].z, pVtx[vtx].z, data_a111e4) && pColors[k] == color && WisEqual(pUVs[k][0], pUV[uv][0], data_a111e4) && WisEqual(pUVs[k][1], pUV[uv][1], data_a111e4)) {
						break;
					}
				}

				if (k == numVtx) {
					pVerts[numVtx] = pVtx[vtx];
					pColors[numVtx] = color;
					pUVs[numVtx][0] = pUV[uv][0];
					pUVs[numVtx][1] = pUV[uv][1];
					pMesh->indexList[i * 3 + j] = numVtx;
					numVtx++;
				} else {
					pMesh->indexList[i * 3 + j] = k;
				}
			} else {
				w_mesh *pSrc = ppMeshes[pFrags[i].m_meshIndex];
				vtx -= 0x8000;

				for (k = 0; k < numVtx; k++) {
					if (WisEqual(pVerts[k].x, pSrc->vecList[vtx].x, data_a111e4) && WisEqual(pVerts[k].y, pSrc->vecList[vtx].y, data_a111e4) && WisEqual(pVerts[k].z, pSrc->vecList[vtx].z, data_a111e4) && pColors[k] == (pSrc->vtxColorList[vtx] | 0xFFFFFF) && WisEqual(pUVs[k][0], pUV[uv][0], data_a111e4) && WisEqual(pUVs[k][1], pUV[uv][1], data_a111e4)) {
						break;
					}
				}

				if (k == numVtx) {
					pVerts[numVtx] = pSrc->vecList[vtx];
					pUVs[numVtx][0] = pUV[uv][0];
					pUVs[numVtx][1] = pUV[uv][1];
					pColors[numVtx] = pSrc->vtxColorList[vtx] | 0xFFFFFF;
					pMesh->indexList[i * 3 + j] = numVtx;
					numVtx++;
				} else {
					pMesh->indexList[i * 3 + j] = k;
				}
			}
		}
	}

	pMesh->vtxNum = numVtx;
	pMesh->vecList = new WVector[numVtx];
	memcpy(pMesh->vecList, pVerts, numVtx * sizeof(WVector));
	pMesh->vtxColorList = new unsigned long[numVtx];
	memcpy(pMesh->vtxColorList, pColors, numVtx * sizeof(unsigned long));
	pMesh->originalVtxColorList = new unsigned long[numVtx];
	memcpy(pMesh->originalVtxColorList, pColors, numVtx * sizeof(unsigned long));
	pMesh->uvData = new float[numVtx][2];
	memcpy(pMesh->uvData, pUVs, numVtx * sizeof(float[2]));

	WBone::CalcMeshAABB(*PG_SMAP_BONE(), *pMesh);

	delete[] pVerts;
	delete[] pUVs;
	delete[] pColors;

	return pMesh;
}

// polysoup.cpp:2701
// Loads the output fragments of the .sbin file: per mesh, the fragments the
// file stores for the current course case and the input fragments of the
// two cases, merged, rid of their duplicates (same vertices, any rotation),
// sorted and built into the shadow-map meshes (smBuildOutFragMeshes).
void WPolySoup::smLoadOutFrags(cFile *pFile, int numMeshes, w_mesh **ppMeshes, WVector *pVtx, SmCase *pCases) {
	int numInFrags = 0;
	int c, i, j, k, r;

	for (c = 0; c < 2; c++) {
		if (pCases[c].m_pLists) {
			for (j = 0; j < pCases[c].m_numLists; j++) {
				SmInFragList *pLists = pCases[c].m_pLists;
				for (k = 0; k < pLists[j].m_numFrags; k++) {
					if (pLists[j].m_pFrags[k].m_triIndex != 0xFFFF) {
						numInFrags++;
					}
				}
			}
		}
	}

	int numOutA;
	pFile->Read(&numOutA, 4);

	int numOutB = 0;
	int numSkipAfter = 0;
	int numSkipBefore = 0;
	switch (CSharedDoc::Instance()->m_pCurCourse == 0 ? (unsigned char)0 : (unsigned char)(CSharedDoc::Instance()->m_pCurCourse->courseFlags >> 4)) {
	case 0:
		pFile->Read(&numSkipBefore, 4);
		pFile->Read(&numSkipAfter, 4);
		break;
	case 1:
		pFile->Read(&numOutB, 4);
		pFile->Read(&numSkipAfter, 4);
		break;
	case 2:
		pFile->Read(&numSkipBefore, 4);
		pFile->Read(&numOutB, 4);
		break;
	}

	// Four runs of numMeshes counters: the first fragment of each mesh in
	// pOut, the file's fragments A and B of each mesh, its input fragments.
	int *pCounts = new int[numMeshes * 4];
	memset(pCounts, 0, numMeshes * 16);
	SmOutFrag *pOut = new SmOutFrag[numOutA + numOutB + numInFrags];

	if (numOutA > 0) {
		pFile->Read(&pCounts[numMeshes], numMeshes * 4);
	}
	if (numSkipBefore > 0) {
		pFile->Seek(numMeshes * 4, 1);
	}
	if (numOutB > 0) {
		pFile->Read(&pCounts[numMeshes * 2], numMeshes * 4);
	}
	if (numSkipAfter > 0) {
		pFile->Seek(numMeshes * 4, 1);
	}

	for (c = 0; c < 2; c++) {
		if (pCases[c].m_pLists) {
			for (j = 0; j < pCases[c].m_numLists; j++) {
				SmInFragList *pLists = pCases[c].m_pLists;
				if (pLists) {
					for (k = 0; k < pLists[j].m_numFrags; k++) {
						SmInFrag *pFrag = &pLists[j].m_pFrags[k];
						if (pFrag->m_triIndex != 0xFFFF) pCounts[numMeshes * 3 + pFrag->m_meshIndex]++;
					}
				}
			}
		}
	}

	pCounts[0] = 0;
	for (i = 1; i < numMeshes; i++) {
		pCounts[i] = pCounts[i - 1] + pCounts[numMeshes * 3 + i - 1] + pCounts[numMeshes + i - 1] + pCounts[numMeshes * 2 + i - 1];
	}

	memset(&pCounts[numMeshes * 3], 0, numMeshes * 4);

	for (i = 0; i < numMeshes; i++) {
		if (pCounts[numMeshes + i] > 0) {
			pFile->Read(&pOut[pCounts[i]], pCounts[numMeshes + i] * 8);
		}
	}
	if (numSkipBefore) {
		pFile->Seek(numSkipBefore * 8, 1);
	}

	if (numOutB > 0) {
		for (i = 0; i < numMeshes; i++) {
			if (pCounts[numMeshes * 2 + i] > 0) {
				pFile->Read(&pOut[pCounts[i] + pCounts[numMeshes + i]], pCounts[numMeshes * 2 + i] * 8);
			}
		}
	}

	for (c = 0; c < 2; c++) {
		if (pCases[c].m_pLists) {
			for (j = 0; j < pCases[c].m_numLists; j++) {
				SmInFragList *pLists = pCases[c].m_pLists;
				if (pLists) {
					for (k = 0; k < pLists[j].m_numFrags; k++) {
						SmInFrag *pFrag = &pLists[j].m_pFrags[k];
						if (pFrag->m_triIndex != 0xFFFF) {
							int mesh = pFrag->m_meshIndex;
							int *pCount = &pCounts[numMeshes * 3 + mesh];
							SmOutFrag *pDst = &pOut[pCounts[numMeshes * 2 + mesh] + pCounts[numMeshes + mesh] + pCounts[mesh] + *pCount];
							pDst->m_triIndex = pFrag->m_triIndex;
							memcpy(pDst->m_vtx, pFrag->m_vtx, 6);
							(*pCount)++;
						}
					}
				}
			}
		}
	}

	if (numSkipAfter) {
		pFile->Seek(numSkipAfter * 8, 1);
	}

	for (i = 0; i < numMeshes; i++) {
		if (pCounts[numMeshes * 2 + i] > 0) {
			pFile->Read(&pOut[pCounts[i] + pCounts[numMeshes + i]], pCounts[numMeshes * 2 + i] * 8);
		}
		pCounts[numMeshes + i] += pCounts[numMeshes * 3 + i] + pCounts[numMeshes * 2 + i];
	}

	for (i = 0; i < numMeshes; i++) {
		if (pCounts[numMeshes + i] > 0) {
			// The fragments with no later duplicate.
			int numUnique = 0;
			for (j = 0; j < pCounts[numMeshes + i] - 1; j++) {
				for (k = j + 1; k < pCounts[numMeshes + i]; k++) {
					for (r = 0; r < 3; r++) {
						if (pOut[pCounts[i] + j].m_vtx[0] == pOut[pCounts[i] + k].m_vtx[r] && pOut[pCounts[i] + j].m_vtx[1] == pOut[pCounts[i] + k].m_vtx[(r + 1) % 3] && pOut[pCounts[i] + j].m_vtx[2] == pOut[pCounts[i] + k].m_vtx[(r + 2) % 3]) {
							break;
						}
					}
					if (r < 3) {
						break;
					}
				}
				if (k == pCounts[numMeshes + i]) {
					numUnique++;
				}
			}

			if (numUnique + 1 != pCounts[numMeshes + i]) {
				SmOutFrag *pKept = new SmOutFrag[numUnique + 1];
				numUnique = 0;
				for (j = 0; j < pCounts[numMeshes + i] - 1; j++) {
					for (k = j + 1; k < pCounts[numMeshes + i]; k++) {
						for (r = 0; r < 3; r++) {
							if (pOut[pCounts[i] + j].m_vtx[0] == pOut[pCounts[i] + k].m_vtx[r] && pOut[pCounts[i] + j].m_vtx[1] == pOut[pCounts[i] + k].m_vtx[(r + 1) % 3] && pOut[pCounts[i] + j].m_vtx[2] == pOut[pCounts[i] + k].m_vtx[(r + 2) % 3]) {
								break;
							}
						}
						if (r < 3) {
							break;
						}
					}
					if (k == pCounts[numMeshes + i]) {
						memcpy(&pKept[numUnique++], &pOut[pCounts[i] + j], sizeof(SmOutFrag));
					}
				}
				memcpy(&pKept[numUnique++], &pOut[pCounts[i] + j], sizeof(SmOutFrag));
				qsort(pKept, numUnique, 8, smCompareOutFrag);
				smBuildOutFragMeshes(ppMeshes[i], pKept, numUnique, pVtx);
				delete[] pKept;
			} else {
				qsort(&pOut[pCounts[i]], pCounts[numMeshes + i], 8, smCompareOutFrag);
				smBuildOutFragMeshes(ppMeshes[i], &pOut[pCounts[i]], pCounts[numMeshes + i], pVtx);
			}
		}
	}

	delete[] pCounts;
	delete[] pOut;
}

// polysoup.cpp:2967
// qsort comparator of the output fragments, ascending by their leading
// 16-bit key.
int __cdecl WPolySoup::smCompareOutFrag(const void *a, const void *b) {
	unsigned short keyA = *(const unsigned short *)a;
	unsigned short keyB = *(const unsigned short *)b;

	if (keyA < keyB) {
		return -1;
	}
	if (keyA > keyB) {
		return 1;
	}
	return 0;
}

// polysoup.cpp:2982
// Splits the triangles of pMesh named by the output fragments (sorted by
// triangle) into the fragments' triangles. A fragment vertex below 0x8000 is
// a new vertex at pVtx[vtx], its normal, color and UV interpolated over the
// split triangle; from 0x8000 up it is the mesh's own vertex vtx - 0x8000.
// The split triangles' indices are replaced by the fragments' ones, appended
// after the indices of the triangles that are kept.
void WPolySoup::smBuildOutFragMeshes(w_mesh *pMesh, SmOutFrag *pFrags, int numFrags, WVector *pVtx) {
	if (numFrags == 0) {
		return;
	}

	unsigned short *pOldIdx = pMesh->indexList;
	WVector *pOldVec = pMesh->vecList;
	WVector *pOldNorm = pMesh->normList;
	float (*pOldUV)[2] = pMesh->uvData;
	unsigned long *pOldColors = pMesh->vtxColorList;
	int numTris = 1;
	int i, j, k;

	for (i = 1; i < numFrags; i++) {
		if (pFrags[i].m_triIndex != pFrags[i - 1].m_triIndex) {
			numTris++;
		}
	}

	int numKeptIdx = pMesh->indexNum - numTris * 3;
	int newIndexNum = numKeptIdx + numFrags * 3;
	unsigned short *pNewIdx = new unsigned short[newIndexNum];
	WVector *pNewVec = new WVector[numFrags * 3];
	WVector *pNewNorm = new WVector[numFrags * 3];
	float (*pNewUV)[2] = new float[numFrags * 3][2];
	unsigned long *pNewColors = new unsigned long[numFrags * 3];
	int numNew = 0;

	for (i = 0; i < numFrags; i++) {
		int tri = pFrags[i].m_triIndex * 3;
		int i0 = pOldIdx[tri];
		int i1 = pOldIdx[tri + 1];
		int i2 = pOldIdx[tri + 2];

		for (j = 0; j < 3; j++) {
			int vtx = pFrags[i].m_vtx[j];

			if (vtx < 0x8000) {
				WVector *pPos = &pVtx[vtx];
				float u, v;
				// The XZ-plane weights are never selected: c2 removes the
				// branch only after GetBarycentric was compiled (lines
				// 3051-3054 have no code), which is why the original keeps
				// an unreferenced GetBarycentric with register arguments.
				bool bProjected = true;
				if (bProjected) {
					GetBarycentricProjected(pOldVec[i0], pOldVec[i1], pOldVec[i2], *pPos, u, v);
				} else {
					GetBarycentric(pOldVec[i0], pOldVec[i1], pOldVec[i2], *pPos, u, v);
				}

				WVector normal;
				normal.x = InterpolateTriangle(pOldNorm[i0].x, pOldNorm[i1].x, pOldNorm[i2].x, u, v);
				normal.y = InterpolateTriangle(pOldNorm[i0].y, pOldNorm[i1].y, pOldNorm[i2].y, u, v);
				normal.z = InterpolateTriangle(pOldNorm[i0].z, pOldNorm[i1].z, pOldNorm[i2].z, u, v);

				float a0 = (float)((pOldColors[i0] >> 24) & 0xFF);
				float r0 = (float)((pOldColors[i0] >> 16) & 0xFF);
				float g0 = (float)((pOldColors[i0] >> 8) & 0xFF);
				float b0 = (float)(pOldColors[i0] & 0xFF);
				unsigned long color = (unsigned char)(int)InterpolateTriangle(a0, (float)((pOldColors[i1] >> 24) & 0xFF), (float)((pOldColors[i2] >> 24) & 0xFF), u, v);
				color = color << 8 | (unsigned char)(int)InterpolateTriangle(r0, (float)((pOldColors[i1] >> 16) & 0xFF), (float)((pOldColors[i2] >> 16) & 0xFF), u, v);
				color = color << 8 | (unsigned char)(int)InterpolateTriangle(g0, (float)((pOldColors[i1] >> 8) & 0xFF), (float)((pOldColors[i2] >> 8) & 0xFF), u, v);
				color = color << 8 | (unsigned char)(int)InterpolateTriangle(b0, (float)(pOldColors[i1] & 0xFF), (float)(pOldColors[i2] & 0xFF), u, v);

				float uv[2];
				uv[0] = InterpolateTriangle(pOldUV[i0][0], pOldUV[i1][0], pOldUV[i2][0], u, v);
				uv[1] = InterpolateTriangle(pOldUV[i0][1], pOldUV[i1][1], pOldUV[i2][1], u, v);

				for (k = 0; k < numNew; k++) {
					if (WisEqual(pNewVec[k].x, pPos->x, data_a111e4) && WisEqual(pNewVec[k].y, pPos->y, data_a111e4) && WisEqual(pNewVec[k].z, pPos->z, data_a111e4) && WisEqual(pNewNorm[k].x, normal.x, data_a111e4) && WisEqual(pNewNorm[k].y, normal.y, data_a111e4) && WisEqual(pNewNorm[k].z, normal.z, data_a111e4) && WisEqual(pNewUV[k][0], uv[0], data_a111e4) && WisEqual(pNewUV[k][1], uv[1], data_a111e4)) {
						break;
					}
				}

				if (k == numNew) {
					pNewVec[numNew] = *pPos;
					pNewNorm[numNew] = normal;
					memcpy(pNewUV[numNew], uv, sizeof(uv));
					pNewColors[numNew] = color;
					pNewIdx[numKeptIdx + i * 3 + j] = (unsigned short)(pMesh->vtxNum + numNew);
					numNew++;
				} else {
					pNewIdx[numKeptIdx + i * 3 + j] = (unsigned short)(pMesh->vtxNum + k);
				}
			} else {
				pNewIdx[numKeptIdx + i * 3 + j] = (unsigned short)(vtx - 0x8000);
			}

		}
	}

	// The indices of the triangles before, between and after the split ones.
	int numIdx = 0;

	if (pFrags[0].m_triIndex > 0) {
		memcpy(pNewIdx, pOldIdx, pFrags[0].m_triIndex * 3 * sizeof(unsigned short));
		numIdx = pFrags[0].m_triIndex * 3;
	}

	for (i = 1; i < numFrags; i++) {
		int prev = pFrags[i - 1].m_triIndex * 3;
		int cur = pFrags[i].m_triIndex * 3;

		if (cur - prev > 3) {
			memcpy(&pNewIdx[numIdx], &pOldIdx[prev + 3], (cur - prev - 3) * sizeof(unsigned short));
			numIdx += cur - prev - 3;
		}
	}

	int last = pFrags[i - 1].m_triIndex * 3;
	int rest = pMesh->indexNum - last;

	if (rest > 3) {
		memcpy(&pNewIdx[numIdx], &pOldIdx[last + 3], (rest - 3) * sizeof(unsigned short));
	}

	WVector *pVec = new WVector[pMesh->vtxNum + numNew];
	memcpy(pVec, pMesh->vecList, pMesh->vtxNum * sizeof(WVector));
	memcpy(&pVec[pMesh->vtxNum], pNewVec, numNew * sizeof(WVector));

	WVector *pNorm = new WVector[pMesh->vtxNum + numNew];
	memcpy(pNorm, pMesh->normList, pMesh->vtxNum * sizeof(WVector));
	memcpy(&pNorm[pMesh->vtxNum], pNewNorm, numNew * sizeof(WVector));

	unsigned long *pColors = new unsigned long[pMesh->vtxNum + numNew];
	memcpy(pColors, pMesh->vtxColorList, pMesh->vtxNum * sizeof(unsigned long));
	memcpy(&pColors[pMesh->vtxNum], pNewColors, numNew * sizeof(unsigned long));

	unsigned long *pOrigColors = new unsigned long[pMesh->vtxNum + numNew];
	memcpy(pOrigColors, pColors, (pMesh->vtxNum + numNew) * sizeof(unsigned long));

	float (*pUV)[2] = new float[pMesh->vtxNum + numNew][2];
	memcpy(pUV, pMesh->uvData, pMesh->vtxNum * sizeof(float[2]));
	memcpy(&pUV[pMesh->vtxNum], pNewUV, numNew * sizeof(float[2]));

	delete[] pNewVec;
	delete[] pNewNorm;
	delete[] pNewUV;
	delete[] pNewColors;

	delete[] pMesh->vecList;
	delete[] pMesh->normList;
	delete[] pMesh->vtxColorList;
	delete[] pMesh->originalVtxColorList;
	delete[] pMesh->uvData;
	delete[] pMesh->indexList;

	pMesh->vtxNum += numNew;
	pMesh->indexNum = newIndexNum;
	pMesh->vecList = pVec;
	pMesh->normList = pNorm;
	pMesh->vtxColorList = pColors;
	pMesh->originalVtxColorList = pOrigColors;
	pMesh->uvData = pUV;
	pMesh->indexList = pNewIdx;
}

// polysoup.cpp:3204
#ifdef PG_7XX
// The shadow-map meshes (drawFlag 0x21100000) of every bone from pBone on:
// a later client's .sbin puts packs on several bones, m_pSmapMeshes holds
// only the last one's.
static void PgSetSmapAlpha(WBone *pBone, float alpha) {
	for (; pBone; pBone = pBone->GetNext()) {
		for (w_mesh *pMesh = pBone->GetMesh(); pMesh; pMesh = pMesh->next) {
			if ((pMesh->drawFlag & 0x21100000) == 0x21100000) {
				pMesh->alpha = alpha;
			}
		}
		PgSetSmapAlpha(pBone->GetChild(), alpha);
	}
}
#endif

void WPolySoup::EnableShadowmap(bool enable) {
	w_mesh *pMesh;

	if (enable) {
		if (m_pSmapMeshes) {
			for (pMesh = m_pSmapMeshes; pMesh; pMesh = pMesh->next) {
				if ((pMesh->drawFlag & 0x21100000) == 0x21100000) {
					pMesh->alpha = 1.0f;
				}
			}
		} else if (!m_bNoShadowmap) {
			ReloadBasePet();
		}
	} else {
		for (pMesh = m_pSmapMeshes; pMesh; pMesh = pMesh->next) {
			if ((pMesh->drawFlag & 0x21100000) == 0x21100000) {
				pMesh->alpha = 0.0f;
			}
		}
	}
#ifdef PG_7XX
	if (m_pSmapMeshes && m_pBgModel && m_pBgModel->m_pPuppet) {
		PgSetSmapAlpha(m_pBgModel->m_pPuppet->GetRootBone(), enable ? 1.0f : 0.0f);
	}
#endif
}

// polysoup.cpp:3234
// Reloads the background puppet and its vertex colours from the map file,
// then the shadow map and the hole cup.
void WPolySoup::ReloadBasePet(void) {
	sBgModel();   // a discarded temporary: the original emits sBgModel's constructor with no caller left
	if (g_resrcmng == 0) {
		return;
	}

	if (m_pBgModel->m_pPuppet) {
		g_resrcmng->Release(m_pBgModel->m_pPuppet, true);
		m_pBgModel->m_pPuppet = 0;
	}

	m_pBgModel->m_pPuppet = g_resrcmng->GetPuppet(m_pBgModel->m_name, false, false, false);

	m_pBgModel->m_pPuppet->m_lightmode = 0x1000000;
	m_pBgModel->m_pPuppet->ApplyBones(0, m_pBgModel->m_matrix);
	m_pBgModel->m_pPuppet->UpdateBound(true, false);
	m_pBgModel->m_pPuppet->UpdateLightSource(&g_lightset, false, 0);

	cFile *pFile = GetResrcManager()->GetCFile(m_mapName, -1);

	int offset = m_numTextures * 0x20 + m_numCameras * 0x40 + m_numAreas * 0x59 + m_numPoints * 0x2D + 0x28;

	for (int i = 0; i < m_numPoints; i++) {
		if (m_pPoints[i].m_type) {
			offset += 0x40;
		}
	}

	for (std::vector<CNodeContainer>::iterator it = m_boundAreas.begin(); it != m_boundAreas.end(); ++it) {
		offset += it->GetList().size() * 12 + 0x18;
	}

#ifdef PG_7XX
	// 0x72: the longer header and records before the colours, which are
	// grouped per bone and vertex (ReadGbin72Colors).
	bool gbin72 = m_numOtherRecords[1] == 0x72;
	if (gbin72) {
		offset += kGbin72HeaderExtra + m_numCameras * kGbin72CameraExtra + m_numPoints * kGbin72PointExtra + m_numAreas * kGbin72AreaExtra;
	}
#endif
	pFile->Seek(offset + 0xAC, 1);

#ifdef PG_7XX
	if (gbin72) {
		pFile->Seek(kGbin72ModelExtra, 1);
		ReadGbin72Colors(pFile, m_pBgModel->m_pPuppet, GOLFDOC()->m_fogBrightness);
	} else
#endif
	for (w_mesh *pMesh = m_pBgModel->m_pPuppet->GetRootBone()->GetMesh(); pMesh; pMesh = pMesh->next) {
		for (int i = 0; i < pMesh->indexNum; i += 3) {
			for (int j = 0; j < 3; j++) {
				pFile->Read(&pMesh->vtxColorList[pMesh->indexList[i + j]], 4);
			}
		}
	}

	CloseCFile(pFile);

	m_bNoShadowmap = !LoadShadowmapInfo();

	if (strstr(m_mapName, "rank") == 0) {
		PlaceHoleCup();
	}
	m_pBgModel->m_pPuppet->xUpdateTnLBuffers(true);

	CSceneManager::Instance()->SetPuppet(m_pBgModel->m_name, m_pBgModel->m_pPuppet);
}

// polysoup.cpp:3341
// Digs the hole cup into the background: a 32-gon of the cup's radius around
// the current hole's cup position (taken into the root bone's space) is cut
// out of the meshes it lies on.
void WPolySoup::PlaceHoleCup(void) {
	int i;
	for (i = 0; i < m_numPoints; i++) {
		if (strstr(m_pPoints[i].m_name, "\xB3\xA1\xC1\xA1")) {
			break;
		}
	}
	if (i == m_numPoints) {
		return;
	}

	std::vector<sCupVtx> cupVtxs;
	std::vector<sHoleTri> tris;
	WVector center(0.0f, 0.0f, 0.0f);
	std::vector<sHoleMesh> meshes;

	WMatrix inv;
	inv = ~m_pBgModel->m_pPuppet->GetRootBone()->GetMatrix();
	WVector cupPos = GOLFDOC()->GetHoleData().m_cupPos;
	center = cupPos * inv;

	float radius = GolfBall().m_radius * 3.0f;

	if (IsLocalContent((localContentType_t)0x3A)) {
		CSharedDoc *pDoc = WSingleton<CSharedDoc>::Instance();
		if ((pDoc->m_currentChannel.flags & 0x800) && pDoc->m_roomType != 0x0B && pDoc->m_roomType != 0x0C) {
			radius = GolfBall().m_radius * 6.0f;
		}
	}

	for (int n = 0; n < 0x21; n++) {
		float angle = (float)n * 0.19634955f;
		sCupVtx vtx;
		vtx.m_pos.x = cosf(angle) * radius + center.x;
		vtx.m_pos.z = center.z - sinf(angle) * radius;
		cupVtxs.push_back(vtx);
	}

	GatherHolecupTris(tris, cupVtxs, center);

	if (tris.size() == 0) {
		return;
	}

	CreateNewHolecupMeshes(meshes, cupVtxs, tris);
	AddHolecup(meshes, cupVtxs, tris);

	WBone *pBone = m_pBgModel->m_pPuppet->GetRootBone();
	int maxVtx = 0;
	for (w_mesh *pMesh = pBone->GetMesh(); pMesh; pMesh = pMesh->next) {
		if (maxVtx < pMesh->vtxNum) {
			maxVtx = pMesh->vtxNum;
		}
	}
	pBone->AllocVTX(maxVtx);
}

// polysoup.cpp:3407
// Collects the triangles of the course mesh the hole cup cuts: first the
// meshes of the cup's own textures, then the others if none of those were hit.
void WPolySoup::GatherHolecupTris(std::vector<sHoleTri> &tris, std::vector<sCupVtx> &cupVtxs, WVector &center) {
	std::vector<int> cupTextures;
	sHoleTri holeTri;

	int i;
	for (i = 0; i < m_numTextures; i++) {
		const sProperty &prop = GetProperty(m_pTextures[i].m_param);

		if (prop.m_weatherBounceType == 2)
			cupTextures.push_back(m_pTextures[i].m_handle);
	}

	for (int pass = 0; pass < 2; pass++) {
		for (w_mesh *pMesh = m_pBgModel->m_pPuppet->GetRootBone()->GetMesh(); pMesh; pMesh = pMesh->next) {
			if (pass == 0) {
				if ((pMesh->drawFlag & 0x300000) != 0x100000) {
					if (std::find(cupTextures.begin(), cupTextures.end(), pMesh->drawFlag & 0x7FF) == cupTextures.end())
						continue;
				}
			} else {
				if ((pMesh->drawFlag & 0x300000) == 0x100000)
					continue;
				if (std::find(cupTextures.begin(), cupTextures.end(), pMesh->drawFlag & 0x7FF) != cupTextures.end())
					continue;
			}

			for (i = 0; i < pMesh->indexNum; i += 3) {
				holeTri.m_pVertex[0] = &pMesh->vecList[pMesh->indexList[i]];
				holeTri.m_pVertex[1] = &pMesh->vecList[pMesh->indexList[i + 1]];
				holeTri.m_pVertex[2] = &pMesh->vecList[pMesh->indexList[i + 2]];

				WVector normal = WCrossProduct(*holeTri.m_pVertex[1] - *holeTri.m_pVertex[0], *holeTri.m_pVertex[2] - *holeTri.m_pVertex[0]).Normalize();

				if (normal.y < 0.001f)
					continue;

				int k;
				for (k = 0; k < (int)cupVtxs.size() - 1; k++) {
					if (IsInTri(cupVtxs[k].m_pos, holeTri.m_pVertex)) {
						GenHolecupTri(&holeTri, pMesh, i);
						tris.push_back(holeTri);
						break;
					}
				}

				if (k < (int)cupVtxs.size() - 1)
					continue;

				int m;
				for (m = 0; m < 3; m++) {
					if (IsInPoly(*holeTri.m_pVertex[m], cupVtxs)) {
						GenHolecupTri(&holeTri, pMesh, i);
						tris.push_back(holeTri);
						break;
					}
				}

				if (m < 3)
					continue;

				for (k = 0; k < (int)cupVtxs.size() - 1; k++) {
					sCupVtx *pVtx = &cupVtxs[k];

					for (m = 0; m < 3; m++) {
						if (AreCrossed(&pVtx[0].m_pos, &pVtx[1].m_pos, holeTri.m_pVertex[m], holeTri.m_pVertex[(m + 1) % 3])) {
							GenHolecupTri(&holeTri, pMesh, i);
							tris.push_back(holeTri);
							break;
						}
					}

					if (m < 3)
						break;
				}
			}
		}

		bool bFound = false;
		for (int t = 0; t < (int)tris.size(); t++) {
			if ((tris[t].m_pMesh->drawFlag & 0x300000) != 0x100000) {
				bFound = true;
				break;
			}
		}
		if (bFound)
			break;
	}

	for (i = 0; i < (int)cupVtxs.size(); i++) {
		sCupVtx &vtx = cupVtxs[i];

		for (int t = 0; t < (int)tris.size(); t++) {
			sHoleTri &tri = tris[t];
			if (IsInTri(vtx.m_pos, tri.m_pVertex)) {
				vtx.m_pMesh = tri.m_pMesh;
				cupVtxs[i].m_pMesh = tris[t].m_pMesh;
				cupVtxs[i].m_index = tris[t].m_index;
				break;
			}
		}
	}
}

// polysoup.cpp:3557
// Whether pos lies inside the triangle tri on the XZ plane: pos must be on
// the same side of every edge as the triangle's own winding.
bool WPolySoup::IsInTri(WVector &pos, WVector *tri[]) {
	WVector2D e1;
	e1.x = tri[1]->x - tri[0]->x;
	e1.y = tri[1]->z - tri[0]->z;
	e1 /= e1.Magnitude();
	WVector2D e2;
	e2.x = tri[2]->x - tri[0]->x;
	e2.y = tri[2]->z - tri[0]->z;

	WVector2D p(pos.x, pos.z);
	e2 /= e2.Magnitude();

	float winding = e1.x * e2.y - e2.x * e1.y;

	if (Abs(winding) < 0.001f)
		return false;

	for (int i = 0; i < 3; i++) {
		WVector2D edge;
		edge.x = tri[(i + 1) % 3]->x - tri[i]->x;
		edge.y = tri[(i + 1) % 3]->z - tri[i]->z;
		edge /= edge.Magnitude();
		WVector2D toPos;
		toPos.x = p.x - tri[i]->x;
		toPos.y = p.y - tri[i]->z;
		toPos /= toPos.Magnitude();

		if ((edge.x * toPos.y - toPos.x * edge.y) * winding < 0.0f)
			return false;
	}

	return true;
}

// polysoup.cpp:3600
// Whether pos lies inside the closed polygon verts (its last vertex repeats
// the first) on the XZ plane: pos must be on the same side of every edge as
// the polygon's own winding.
bool WPolySoup::IsInPoly(WVector &pos, std::vector<sCupVtx> &verts) {
	if (verts.size() < 4)
		return false;

	WVector2D p(pos.x, pos.z);
	WVector2D v0(verts[0].m_pos.x, verts[0].m_pos.z);
	WVector2D v1(verts[1].m_pos.x, verts[1].m_pos.z);
	WVector2D v2(verts[2].m_pos.x, verts[2].m_pos.z);
	WVector2D e;
	e = v1 - v0;
	e /= e.Magnitude();
	WVector2D d;
	d = v2 - v0;
	d /= d.Magnitude();
	float winding = e.x * d.y - d.x * e.y;

	for (int i = 0; i < (int)verts.size() - 1; i++) {
		WVector2D c(verts[i].m_pos.x, verts[i].m_pos.z);
		WVector2D n(verts[i + 1].m_pos.x, verts[i + 1].m_pos.z);
		WVector2D edge;
		edge = n - c;
		edge /= edge.Magnitude();
		WVector2D toPos;
		toPos = p - c;
		toPos /= toPos.Magnitude();

		if ((edge.x * toPos.y - toPos.x * edge.y) * winding <= 0.0f)
			return false;
	}

	return true;
}

// polysoup.cpp:3653
// Whether the segments (a, b) and (c, d) cross on the XZ plane: each
// segment's ends lie on opposite sides of the other.
bool WPolySoup::AreCrossed(WVector *a, WVector *b, WVector *c, WVector *d) {
	WVector2D e(b->x - a->x, b->z - a->z);
	e /= sqrtf(e.x * e.x + e.y * e.y);
	WVector2D v(c->x - a->x, c->z - a->z);
	v /= sqrtf(v.x * v.x + v.y * v.y);
	float side1 = e.x * v.y - v.x * e.y;
	v = WVector2D(d->x - a->x, d->z - a->z);
	v /= sqrtf(v.x * v.x + v.y * v.y);
	float side2 = e.x * v.y - v.x * e.y;

	if (side2 * side1 > 0.0f)
		return false;

	e = WVector2D(d->x - c->x, d->z - c->z);
	e /= sqrtf(e.x * e.x + e.y * e.y);
	v = WVector2D(a->x - c->x, a->z - c->z);
	v /= sqrtf(v.x * v.x + v.y * v.y);
	side1 = e.x * v.y - v.x * e.y;
	v = WVector2D(b->x - c->x, b->z - c->z);
	v /= sqrtf(v.x * v.x + v.y * v.y);
	side2 = e.x * v.y - v.x * e.y;

	if (side2 * side1 > 0.0f)
		return false;

	return true;
}

// polysoup.cpp:3709
void WPolySoup::GenHolecupTri(sHoleTri *pTri, w_mesh *pMesh, int index) {
	pTri->m_pMesh = pMesh;
	pTri->m_index = index;

	for (int i = 0; i < 3; i++) {
		pTri->m_vtx[i] = pMesh->indexList[index + i];

		pTri->m_pNormal[i] = &pMesh->normList[pTri->m_vtx[i]];
		pTri->m_color[i] = pMesh->vtxColorList[pTri->m_vtx[i]];
		pTri->m_uv[i][0] = pMesh->uvData[pTri->m_vtx[i]][0];
		pTri->m_uv[i][1] = pMesh->uvData[pTri->m_vtx[i]][1];
		if (pMesh->uvBackup) {
			pTri->m_uvBackup[i][0] = pMesh->uvBackup[pTri->m_vtx[i]][0];
			pTri->m_uvBackup[i][1] = pMesh->uvBackup[pTri->m_vtx[i]][1];
		}

		pTri->m_edge[i].z = 0.0f;
		pTri->m_edge[i].x = pTri->m_pVertex[i]->z - pTri->m_pVertex[(i + 1) % 3]->z;
		pTri->m_edge[i].y = pTri->m_pVertex[(i + 1) % 3]->x - pTri->m_pVertex[i]->x;
		pTri->m_edge[i].Normalize();
		pTri->m_edge[i].z = (pTri->m_pVertex[i]->z * pTri->m_edge[i].y + pTri->m_pVertex[i]->x * pTri->m_edge[i].x) * -1.0f;
	}
}

// polysoup.cpp:3739
// The intersection of the segment (a, b) with the XZ line
// line->x * x + line->y * z + line->z = 0, stored in out; true only when it
// also lies strictly between c and d.
bool WPolySoup::GetIntersection(WVector *out, WVector *a, WVector *b, WVector *c, WVector *d, WVector *line) {
	float dist[2];
	dist[0] = a->x * line->x + a->z * line->y + line->z;

	if (WAbsBySign(dist[0]) < 0.001f)
		return false;

	dist[1] = b->x * line->x + b->z * line->y + line->z;

	if (WAbsBySign(dist[1]) < 0.001f)
		return false;

	if (dist[1] * dist[0] > 0.0f)
		return false;

	out->x = (b->x - a->x) * dist[0] / (dist[0] - dist[1]) + a->x;
	out->z = (b->z - a->z) * dist[0] / (dist[0] - dist[1]) + a->z;

	float dx = WAbsBySign(d->x - c->x);
	float dz = WAbsBySign(d->z - c->z);

	float side;
	if (dx > dz) {
		side = (out->x - c->x) * (d->x - out->x);
	} else {
		side = (out->z - c->z) * (d->z - out->z);
	}

	if (side > 0.0f)
		return true;
	return false;
}

// polysoup.cpp:3780
void WPolySoup::CreateNewHolecupMeshes(std::vector<sHoleMesh> &meshes, std::vector<sCupVtx> &cupVtxs, std::vector<sHoleTri> &tris) {
	std::vector<WVector> clipped;

	for (std::vector<sHoleTri>::iterator it = tris.begin(); it != tris.end(); ) {
		if (ClipHolecup(clipped, *it, cupVtxs) >= 3) {
			BuildNewHolecupMesh(meshes, *it, clipped);
			++it;
		} else {
			it = tris.erase(it);
		}
	}
}

// polysoup.cpp:3830
// Clips the cup polygon (cupVtxs) against the three XZ edge lines of tri
// (Sutherland-Hodgman, keeping the side where a line is <= 0) into clipped,
// closing the polygon by repeating its first vertex. Returns the vertex count.
int WPolySoup::ClipHolecup(std::vector<WVector> &clipped, sHoleTri &tri, std::vector<sCupVtx> &cupVtxs) {
	std::vector<WVector> work;
	WVector cross;
	int prevSide;
	int side;

	clipped.clear();
	int count = (int)cupVtxs.size();

	for (int i = 0; i < count; i++)
		work.push_back(cupVtxs[i].m_pos);

	std::vector<WVector> *pIn = &work;
	std::vector<WVector> *pOut = &clipped;

	for (int edge = 0; edge < 3; edge++) {
		float dist = tri.m_edge[edge].x * (*pIn)[0].x + tri.m_edge[edge].y * (*pIn)[0].z + tri.m_edge[edge].z;
		prevSide = dist > 0.001f ? 1 : (dist < -0.001f ? -1 : 0);
		if (prevSide <= 0)
			pOut->push_back((*pIn)[0]);

		for (int j = 1; j < count; j++) {
			dist = tri.m_edge[edge].x * (*pIn)[j].x + tri.m_edge[edge].y * (*pIn)[j].z + tri.m_edge[edge].z;
			side = dist > 0.001f ? 1 : (dist < -0.001f ? -1 : 0);

			if (prevSide == 0) {
				if (side < 0)
					pOut->push_back((*pIn)[j]);
			} else if (side == 0) {
				pOut->push_back((*pIn)[j]);
			} else {
				if (side * prevSide < 0) {
					GetIntersection(&cross, &(*pIn)[j - 1], &(*pIn)[j], tri.m_pVertex[edge], tri.m_pVertex[(edge + 1) % 3], &tri.m_edge[edge]);
					pOut->push_back(cross);
				}
				if (side < 0)
					pOut->push_back((*pIn)[j]);
			}
			prevSide = side;
		}

		if (pOut->size() < 3)
			return (int)pOut->size();

		WVector &first = pOut->front();
		WVector &last = pOut->back();
		if (!WisEqual(first.x, last.x, data_a111e4) || !WisEqual(first.y, last.y, data_a111e4) || !WisEqual(first.z, last.z, data_a111e4))
			pOut->push_back(first);

		std::vector<WVector> *tmp = pIn;
		pIn = pOut;
		pOut = tmp;
		if (edge < 2)
			pOut->clear();

		count = (int)pIn->size();
	}

	return (int)clipped.size();
}

// polysoup.cpp:3919
void WPolySoup::BuildNewHolecupMesh(std::vector<sHoleMesh> &meshes, sHoleTri &tri, std::vector<WVector> &clipped) {
	std::vector<WVector> edges;
	int numClipped = (int)clipped.size();
	int i;

	// The XZ line of each edge of the clipped polygon: (a, b, c), a*x + b*z + c.
	edges.reserve(numClipped);
	for (i = 0; i < numClipped; i++) {
		int next = i + 1;
		if (next >= numClipped)
			next++;
		next %= numClipped;

		WVector &cur = clipped[i];
		WVector line(cur.z - clipped[next].z, clipped[next].x - cur.x, 0.0f);
		line.Normalize();
		line.z = (line.x * cur.x + line.y * cur.z) * -1.0f;
		edges.push_back(line);
	}

	int firstInside = -1;
	bool started = false;
	int prevCount = 0;
	int prevInside[3];
	bool used[3];
	memset(used, 0, sizeof(used));

	for (i = 0; i < numClipped; i++) {
		int inside[3];
		int count = 0;
		int k;

		for (k = 0; k < 3; k++) {
			float dist = tri.m_pVertex[k]->x * edges[i].x + tri.m_pVertex[k]->z * edges[i].y + edges[i].z;
			if (dist > 0.001f)
				inside[count++] = k;
		}

		if (count == 2) {
			if (inside[0] == 0 && inside[1] == 2) {
				inside[1] = 0;
				inside[0] = 2;
			}
			if (i == 0)
				firstInside = inside[0];
		}

		switch (count) {
		case 0:
			started = true;
			break;

		case 1: {
			int cur = inside[0];
			if (prevCount == 1) {
				int other = prevInside[0];
				if (cur != other && !used[other]) {
					used[other] = true;
					AddNewHolecupTri(meshes, tri, tri.m_pVertex[other], tri.m_pVertex[cur], &clipped[i],
						tri.m_vtx[other], tri.m_vtx[cur], -1);
				}
			} else if (prevCount == 2) {
				int other = prevInside[1];
				if (cur != other && !used[other]) {
					used[other] = true;
					AddNewHolecupTri(meshes, tri, tri.m_pVertex[other], tri.m_pVertex[cur], &clipped[i],
						tri.m_vtx[other], tri.m_vtx[cur], -1);
				}
			}

			if (i < numClipped - 1) {
				AddNewHolecupTri(meshes, tri, tri.m_pVertex[cur], &clipped[i + 1], &clipped[i],
					tri.m_vtx[cur], -1, -1);
			}
			started = true;
			break;
		}

		case 2: {
			if (prevCount == 1 && inside[0] != prevInside[0] && !used[prevInside[0]]) {
				int other = prevInside[0];
				used[other] = true;
				started = true;
				AddNewHolecupTri(meshes, tri, tri.m_pVertex[other], tri.m_pVertex[inside[0]], &clipped[i],
					tri.m_vtx[other], tri.m_vtx[inside[0]], -1);
			}

			if (i < numClipped - 1) {
				int cur = inside[0];
				if (!used[cur]) {
					used[cur] = true;
					AddNewHolecupTri(meshes, tri, tri.m_pVertex[cur], tri.m_pVertex[inside[1]], &clipped[i],
						tri.m_vtx[cur], tri.m_vtx[inside[1]], -1);
					AddNewHolecupTri(meshes, tri, tri.m_pVertex[inside[1]], &clipped[i + 1], &clipped[i],
						tri.m_vtx[inside[1]], -1, -1);
				} else if (cur == firstInside && started) {
					AddNewHolecupTri(meshes, tri, tri.m_pVertex[cur], &clipped[i + 1], &clipped[i],
						tri.m_vtx[cur], -1, -1);
				} else {
					AddNewHolecupTri(meshes, tri, tri.m_pVertex[inside[1]], &clipped[i + 1], &clipped[i],
						tri.m_vtx[inside[1]], -1, -1);
				}
			}
			break;
		}
		}

		memcpy(prevInside, inside, count * sizeof(int));
		prevCount = count;
	}
}

// polysoup.cpp:4081
// Appends a triangle to the hole mesh of tri's mesh: a corner whose index is
// -1 is a new vertex, shared with an equal one the mesh already gained.
void WPolySoup::AddNewHolecupTri(std::vector<sHoleMesh> &meshes, sHoleTri &tri, WVector *pPos0, WVector *pPos1, WVector *pPos2, int index0, int index1, int index2) {
	WVector *pPos[3];
	pPos[0] = pPos0;
	pPos[1] = pPos1;
	pPos[2] = pPos2;
	int indices[3];
	indices[0] = index0;
	indices[1] = index1;
	indices[2] = index2;

	sHoleMesh *pMesh = 0;
	std::vector<sHoleMesh>::iterator it;
	for (it = meshes.begin(); it != meshes.end(); ++it) {
		if (it->m_pMesh == tri.m_pMesh) {
			pMesh = &*it;
			break;
		}
	}
	if (it == meshes.end()) {
		sHoleMesh mesh;
		mesh.m_pMesh = tri.m_pMesh;
		meshes.push_back(mesh);
		it = meshes.end();
		pMesh = &*(it - 1);
	}

	std::vector<sHoleVtx> *pVtxs = &pMesh->m_vtxs;
	std::vector<int> *pIndices = &pMesh->m_indices;

	for (int i = 0; i < 3; i++) {
		if (indices[i] == -1) {
			sHoleVtx vtx;
			vtx.m_pos = *pPos[i];
			GenNewHolecupVtx(&vtx, &tri);

			int numVtxs = pVtxs->size();
			int j;
			for (j = 0; j < numVtxs; j++) {
				sHoleVtx &other = (*pVtxs)[j];
				if (WisEqual(other.m_pos.x, vtx.m_pos.x, 0.00001f) && WisEqual(other.m_pos.y, vtx.m_pos.y, 0.00001f) && WisEqual(other.m_pos.z, vtx.m_pos.z, 0.00001f)
					&& WisEqual(other.m_normal.x, vtx.m_normal.x, 0.00001f) && WisEqual(other.m_normal.y, vtx.m_normal.y, 0.00001f) && WisEqual(other.m_normal.z, vtx.m_normal.z, 0.00001f)
					&& WisEqual(other.m_uv[0], vtx.m_uv[0], 0.00001f) && WisEqual(other.m_uv[1], vtx.m_uv[1], 0.00001f)) {
					if (tri.m_pMesh->uvBackup == 0)
						break;

					if (WisEqual(other.m_uvBackup[0], vtx.m_uvBackup[0], 0.00001f) && WisEqual(other.m_uvBackup[1], vtx.m_uvBackup[1], 0.00001f))
						break;
				}
			}

			if (j == numVtxs) {
				pIndices->push_back(tri.m_pMesh->vtxNum + numVtxs);
				pVtxs->push_back(vtx);
			} else {
				pIndices->push_back(tri.m_pMesh->vtxNum + j);
			}
		} else {
			pIndices->push_back(indices[i]);
		}
	}
}

// polysoup.cpp:4170
void WPolySoup::GenNewHolecupVtx(sHoleVtx *pVtx, sHoleTri *pTri) {
	// The barycentric coordinates of pVtx->m_pos in the triangle, on the XZ plane.
	float u, v;
	GetBarycentric(pVtx->m_pos, *pTri->m_pVertex[1], *pTri->m_pVertex[2], *pTri->m_pVertex[0], u, v);

	pVtx->m_pos.y = InterpolateTriangle(pTri->m_pVertex[0]->y, pTri->m_pVertex[1]->y, pTri->m_pVertex[2]->y, u, v);

	if (pTri->m_pMesh->normList) {
		pVtx->m_normal.x = InterpolateTriangle(pTri->m_pNormal[0]->x, pTri->m_pNormal[1]->x, pTri->m_pNormal[2]->x, u, v);
		pVtx->m_normal.y = InterpolateTriangle(pTri->m_pNormal[0]->y, pTri->m_pNormal[1]->y, pTri->m_pNormal[2]->y, u, v);
		pVtx->m_normal.z = InterpolateTriangle(pTri->m_pNormal[0]->z, pTri->m_pNormal[1]->z, pTri->m_pNormal[2]->z, u, v);
	}

	// Each byte of the colour, interpolated alone.
	float alpha = ((unsigned char *)&pTri->m_color[0])[3];
	float red = ((unsigned char *)&pTri->m_color[0])[2];
	float green = ((unsigned char *)&pTri->m_color[0])[1];
	float blue = (float)(pTri->m_color[0] & 0xFF);
	pVtx->m_color =
		((((unsigned long)(unsigned char)(int)InterpolateTriangle(alpha, ((unsigned char *)&pTri->m_color[1])[3], ((unsigned char *)&pTri->m_color[2])[3], u, v) << 8
		| (unsigned char)(int)InterpolateTriangle(red, ((unsigned char *)&pTri->m_color[1])[2], ((unsigned char *)&pTri->m_color[2])[2], u, v)) << 8
		| (unsigned char)(int)InterpolateTriangle(green, ((unsigned char *)&pTri->m_color[1])[1], ((unsigned char *)&pTri->m_color[2])[1], u, v)) << 8)
		| (unsigned char)(int)InterpolateTriangle(blue, (float)(pTri->m_color[1] & 0xFF), (float)(pTri->m_color[2] & 0xFF), u, v);

	pVtx->m_uv[0] = InterpolateTriangle(pTri->m_uv[0][0], pTri->m_uv[1][0], pTri->m_uv[2][0], u, v);
	pVtx->m_uv[1] = InterpolateTriangle(pTri->m_uv[0][1], pTri->m_uv[1][1], pTri->m_uv[2][1], u, v);

	if (pTri->m_pMesh->uvBackup) {
		pVtx->m_uvBackup[0] = InterpolateTriangle(pTri->m_uvBackup[0][0], pTri->m_uvBackup[1][0], pTri->m_uvBackup[2][0], u, v);
		pVtx->m_uvBackup[1] = InterpolateTriangle(pTri->m_uvBackup[0][1], pTri->m_uvBackup[1][1], pTri->m_uvBackup[2][1], u, v);
	}
}

// polysoup.cpp:4215
void WPolySoup::AddHolecup(std::vector<sHoleMesh> &meshes, std::vector<sCupVtx> &cupVtxs, std::vector<sHoleTri> &tris) {
	DigHolecup(cupVtxs, tris);

	for (std::vector<sHoleMesh>::iterator it = meshes.begin(); it != meshes.end(); ++it) {
		ApplyHoleMesh(*it, tris);
	}
}

// polysoup.cpp:4227
void WPolySoup::ApplyHoleMesh(sHoleMesh &mesh, std::vector<sHoleTri> &tris) {
	w_mesh *pMesh = mesh.m_pMesh;
	int vtxNum = pMesh->vtxNum + mesh.m_vtxs.size();

	WVector *pVecList = new WVector[vtxNum];
	WVector *pNormList;
	if (pMesh->normList)
		pNormList = new WVector[vtxNum];
	else
		pNormList = 0;
	unsigned long *pVtxColors = new unsigned long[vtxNum];
	unsigned long *pOrigColors = new unsigned long[vtxNum];
	float (*pUV)[2] = new float[vtxNum][2];
	float (*pUVBackup)[2] = pMesh->uvBackup ? new float[vtxNum][2] : 0;

	int removed = 0;
	for (std::vector<sHoleTri>::iterator tri = tris.begin(); tri != tris.end(); ++tri) {
		if (tri->m_pMesh == pMesh)
			removed++;
	}

	int indexNum = mesh.m_indices.size() - removed * 3 + pMesh->indexNum;
	unsigned short *pIndexList = new unsigned short[indexNum];

	memcpy(pVecList, pMesh->vecList, pMesh->vtxNum * sizeof(WVector));

	if (pNormList)
		memcpy(pNormList, pMesh->normList, pMesh->vtxNum * sizeof(WVector));

	memcpy(pVtxColors, pMesh->vtxColorList, pMesh->vtxNum * sizeof(unsigned long));
	memcpy(pOrigColors, pMesh->originalVtxColorList, pMesh->vtxNum * sizeof(unsigned long));
	memcpy(pUV, pMesh->uvData, pMesh->vtxNum * sizeof(float[2]));
	if (pMesh->uvBackup)
		memcpy(pUVBackup, pMesh->uvBackup, pMesh->vtxNum * sizeof(float[2]));

	for (int i = pMesh->vtxNum; i < vtxNum; i++) {
		sHoleVtx &vtx = mesh.m_vtxs[i - pMesh->vtxNum];

		pVecList[i] = vtx.m_pos;

		if (pNormList)
			pNormList[i] = vtx.m_normal;

		pVtxColors[i] = vtx.m_color;
		pOrigColors[i] = vtx.m_color;
		memcpy(pUV[i], vtx.m_uv, sizeof(float[2]));

		if (pMesh->uvBackup) {
			memcpy(pUVBackup[i], vtx.m_uvBackup, sizeof(float[2]));
		}
	}

	for (std::vector<sHoleTri>::iterator it = tris.begin(); it != tris.end(); ++it) {
		if (it->m_pMesh == pMesh) {
			// keep the indices before the first removed triangle
			if (it->m_index > 0)
				memcpy(pIndexList, pMesh->indexList, it->m_index * sizeof(unsigned short));

			int prev = it->m_index;
			int dst = prev;

			// and those between the removed triangles (sorted by index)
			for (++it; it != tris.end(); ++it) {
				if (it->m_pMesh != pMesh)
					break;

				int gap = it->m_index - prev;
				if (gap > 3) {
					memcpy(&pIndexList[dst], &pMesh->indexList[prev + 3], (gap - 3) * sizeof(unsigned short));
					dst += it->m_index - prev - 3;
				}
				prev = it->m_index;
			}

			int gap = pMesh->indexNum - prev;
			if (gap > 3) {
				memcpy(&pIndexList[dst], &pMesh->indexList[prev + 3], (gap - 3) * sizeof(unsigned short));
				dst += pMesh->indexNum - prev - 3;
			}

			// then the hole mesh's own triangles
			for (int k = 0; dst < indexNum; dst++, k++)
				pIndexList[dst] = (unsigned short)mesh.m_indices[k];
			break;
		}
	}

	delete[] pMesh->vecList;
	delete[] pMesh->normList;
	delete[] pMesh->vtxColorList;
	delete[] pMesh->originalVtxColorList;
	delete[] pMesh->uvData;
	if (pMesh->uvBackup)
		delete[] pMesh->uvBackup;
	delete[] pMesh->indexList;

	pMesh->vtxNum = vtxNum;
	pMesh->vecList = pVecList;
	pMesh->normList = pNormList;
	pMesh->vtxColorList = pVtxColors;
	pMesh->originalVtxColorList = pOrigColors;
	pMesh->indexNum = indexNum;
	pMesh->uvData = pUV;
	pMesh->uvBackup = pUVBackup;
	pMesh->indexList = pIndexList;
}

// polysoup.cpp:4353
// Appends to the background's root bone a mesh for the inside of the hole
// cup: a wall strip down from the rim (cupVtxs) to one unit below it, and a
// floor quad, textured with holecup.dds and lit by g_lightset.
void WPolySoup::DigHolecup(std::vector<sCupVtx> &cupVtxs, std::vector<sHoleTri> &tris) {
	FindNewHolecupVtxs(cupVtxs, tris);

	w_mesh *pMesh = m_pBgModel->m_pPuppet->GetRootBone()->GetMesh();

	WVector up(0.0f, 1.0f, 0.0f);
	if (!pMesh)
		return;

	while (pMesh->next)
		pMesh = pMesh->next;

	w_mesh *pCup = new w_mesh;
	memset(pCup, 0, sizeof(w_mesh));
	pMesh->next = pCup;

	if (!m_holecupTexture)
		m_holecupTexture = g_resrcmng->LoadTexture("holecup.dds", 0, 0, 0);
	pCup->texHandle = m_holecupTexture;
	pCup->drawFlag = (m_holecupTexture & 0x7FF) | 0x8000000;
	pCup->diffuse = 0xFFFFFF;
	pCup->alpha = 1.0f;

	int numCupVtxs = cupVtxs.size();
	pCup->vtxNum = numCupVtxs * 2 + 4;
	pCup->indexNum = numCupVtxs * 6;
	pCup->vecList = new WVector[pCup->vtxNum];
	pCup->normList = new WVector[pCup->vtxNum];
	pCup->vtxColorList = new unsigned long[pCup->vtxNum];
	pCup->originalVtxColorList = new unsigned long[pCup->vtxNum];
	pCup->uvData = new float[pCup->vtxNum][2];
	pCup->uvBackup = 0;
	pCup->indexList = new unsigned short[pCup->indexNum];

	float radius = GolfBall().m_radius * 3.0f;

	if (IsLocalContent((localContentType_t)0x3A)) {
		CSharedDoc *pDoc = WSingleton<CSharedDoc>::Instance();
		if (pDoc->m_roomType != 0x0B && pDoc->m_roomType != 0x0C && (pDoc->m_currentChannel.flags & 0x800)) {
			radius = GolfBall().m_radius * 6.0f;
		}
	}

	WMatrix inv;
	inv = ~m_pBgModel->m_pPuppet->GetRootBone()->GetMatrix();
	WVector center = GOLFDOC()->GetHoleData().m_cupPos;
	center = center * inv;
	float bottom = cupVtxs.begin()->m_pos.y - 1.0f;
	// The light direction in the bone's space (rotation only), reversed.
	WVector toLight = WVector(inv.xx * g_lightset.nearone.x + inv.xy * g_lightset.nearone.y + inv.xz * g_lightset.nearone.z,
		inv.yx * g_lightset.nearone.x + inv.yy * g_lightset.nearone.y + inv.yz * g_lightset.nearone.z,
		inv.zx * g_lightset.nearone.x + inv.zy * g_lightset.nearone.y + inv.zz * g_lightset.nearone.z) * -1.0f;
	toLight.Normalize();

	int i;
	for (i = 0; i < numCupVtxs; i++) {
		WVector dir;
		dir = WVector(center.x - cupVtxs[i].m_pos.x, 0.0f, center.z - cupVtxs[i].m_pos.z);
		dir.Normalize();

		pCup->vecList[i * 2] = pCup->vecList[i * 2 + 1] = cupVtxs[i].m_pos;
		pCup->vecList[i * 2 + 1].y = bottom;
		pCup->normList[i * 2] = pCup->normList[i * 2 + 1] = dir;

		float angle;
		if (i == numCupVtxs - 1) {
			angle = 6.2831855f;
		} else {
			float cosine = -dir.x;
			angle = acosf(cosine < -1.0f ? -1.0f : (cosine > 1.0f ? 1.0f : cosine));
			if (dir.z < 0.0f)
				angle = 6.2831855f - angle;
		}
		float u = angle * 0.47746483f;

		pCup->uvData[i * 2][0] = u;
		pCup->uvData[i * 2][1] = 0.015625f;
		pCup->uvData[i * 2 + 1][0] = u;
		pCup->uvData[i * 2 + 1][1] = 0.90625f;

		float lit = (toLight.z * dir.z + toLight.x * dir.x + toLight.y * dir.y) * 255.0f;
		unsigned long color;
		if (lit <= 0.0f)
			color = g_lightset.ambient;
		else
			color = AddDiffuse(g_lightset.ambient, g_lightset.diffuse, (unsigned char)lit);
		color |= 0xFF000000;

		pCup->originalVtxColorList[i * 2 + 1] = color;
		pCup->originalVtxColorList[i * 2] = color;
		pCup->vtxColorList[i * 2 + 1] = color;
		pCup->vtxColorList[i * 2] = color;
	}

	// The floor quad, a square of 2 * radius around the cup's centre.
	unsigned long floorColor = g_lightset.ambient | 0xFF000000;
	for (int k = 0; k < 4; k++) {
		int v = numCupVtxs * 2 + k;
		pCup->vecList[v] = center + WVector((k & 1) ? 1.0f : -1.0f, 0.0f, (k & 2) ? -1.0f : 1.0f) * radius;
		pCup->vecList[v].y = bottom;

		pCup->normList[v] = up;

		pCup->uvData[v][0] = 0.0f;
		pCup->uvData[v][1] = 0.96875f;

		pCup->originalVtxColorList[v] = floorColor;
		pCup->vtxColorList[v] = floorColor;
	}

	// The wall's quads, then the floor's in the last slot.
	for (i = 0; i < numCupVtxs - 1; i++) {
		pCup->indexList[i * 6 + 0] = i * 2;
		pCup->indexList[i * 6 + 1] = i * 2 + 2;
		pCup->indexList[i * 6 + 2] = i * 2 + 1;
		pCup->indexList[i * 6 + 3] = i * 2 + 1;
		pCup->indexList[i * 6 + 4] = i * 2 + 2;
		pCup->indexList[i * 6 + 5] = i * 2 + 3;
	}
	pCup->indexList[i * 6 + 0] = numCupVtxs * 2;
	pCup->indexList[i * 6 + 1] = numCupVtxs * 2 + 1;
	pCup->indexList[i * 6 + 2] = numCupVtxs * 2 + 2;
	pCup->indexList[i * 6 + 3] = numCupVtxs * 2 + 2;
	pCup->indexList[i * 6 + 4] = numCupVtxs * 2 + 1;
	pCup->indexList[i * 6 + 5] = numCupVtxs * 2 + 3;

	WBone::CalcMeshAABB(*m_pBgModel->m_pPuppet->GetRootBone(), *pCup);
}

// polysoup.cpp:4506
void WPolySoup::FindNewHolecupVtxs(std::vector<sCupVtx> &cupVtxs, std::vector<sHoleTri> &tris) {
	for (std::vector<sHoleTri>::iterator tri = tris.begin(); tri != tris.end(); ++tri) {
		std::vector<sCupVtx>::iterator prev = cupVtxs.begin();

		bool prevIn = IsInTri(prev->m_pos, tri->m_pVertex);

		for (std::vector<sCupVtx>::iterator cur = prev + 1; cur != cupVtxs.end(); ++cur) {
			bool curIn = IsInTri(cur->m_pos, tri->m_pVertex);

			if (prevIn != curIn) {
				for (int i = 0; i < 3; i++) {
					sCupVtx vtx;

					if (GetIntersection(&vtx.m_pos, &prev->m_pos, &cur->m_pos, tri->m_pVertex[i], tri->m_pVertex[(i + 1) % 3], &tri->m_edge[i])) {
						vtx.m_pMesh = tri->m_pMesh;
						vtx.m_index = tri->m_index;
						cur = cupVtxs.insert(cur, vtx);
						++cur;
						break;
					}
				}
			}

			prev = cur;
			prevIn = curIn;
		}
	}

	for (std::vector<sCupVtx>::iterator it = cupVtxs.begin(); it != cupVtxs.end(); ++it) {
		sHoleVtx holeVtx;

		holeVtx.m_pos = it->m_pos;

		for (std::vector<sHoleTri>::iterator tri = tris.begin(); tri != tris.end(); ++tri) {
			if (tri->m_pMesh == it->m_pMesh && tri->m_index == it->m_index) {
				GenNewHolecupVtx(&holeVtx, &*tri);
				it->m_pos.y = holeVtx.m_pos.y;
				break;
			}
		}
	}
}

// polysoup.cpp:4585
WPolySoup::CStaticPetGrp::CStaticPetGrp() {
	m_numGroups = 0;
	m_ppGroups = 0;
}

// polysoup.cpp:4589
WPolySoup::CStaticPetGrp::~CStaticPetGrp() {
	Clear();
}

// polysoup.cpp:4597
// Groups the consecutive top-level models sharing a puppet name into one
// static puppet group each, then registers the groups with the scene manager.
void WPolySoup::CStaticPetGrp::Build(std::vector<sModel*> models, int option) {
	Clear();

	int numGroups = 0;
	const char *prevName = "\0";
	m_numGroups = 0;
	for (int i = 0; i < option; i++) {
		if (models[i]->m_pPuppet != 0 && models[i]->m_subOrder == 0 && strcmpi(models[i]->m_name, prevName) != 0) {
			prevName = models[i]->m_name;
			numGroups++;
		}
	}

	if (numGroups == 0) {
		return;
	}

	m_ppGroups = new WxStaticPuppetGrp*[numGroups];
	memset(m_ppGroups, 0, numGroups * sizeof(WxStaticPuppetGrp*));

	prevName = "\0";
	int group = 0;
	int index = 0;
	while (group < numGroups) {
		int count = 1;
		if (models[index]->m_pPuppet != 0 && models[index]->m_subOrder == 0 && strcmpi(models[index]->m_name, prevName) != 0) {
			prevName = models[index]->m_name;
			for (int j = index + 1; j < option; j++) {
				if (models[index]->m_subOrder != 0 || strcmpi(models[j]->m_name, prevName) != 0) {
					break;
				}
				count++;
			}

			WPuppet **ppPuppets = new WPuppet*[count];
			for (int k = 0; k < count; k++) {
				ppPuppets[k] = models[index + k]->m_pPuppet;
			}

			m_ppGroups[group] = g_resrcmng->xGetStaticPuppetGrp(count, models[index]->m_name, ppPuppets);
			index += count;
			group++;
			delete[] ppPuppets;
		} else {
			index++;
		}
	}

	WSingleton<CSceneManager>::Instance()->RegisterElement((eObjType)2, 0, this, "StaticPetGrp", 2, 0);
	m_numGroups = numGroups;
}

// polysoup.cpp:4718
void WPolySoup::CStaticPetGrp::Clear(void) {
	for (int i = 0; i < m_numGroups; i++) {
		g_resrcmng->Release(m_ppGroups[i]);
	}
	if (m_ppGroups) {
		delete[] m_ppGroups;
		m_ppGroups = 0;
	}
	m_numGroups = 0;
}

// polysoup.cpp:4729
void WPolySoup::CStaticPetGrp::Display(void) {
	g_view->xSetLight(0, g_lightset);

	for (int i = 0; i < m_numGroups; i++) {
		if (m_ppGroups[i]) {
			m_ppGroups[i]->xUpdateCullFlag(g_view);
			m_ppGroups[i]->xRender(g_view);
		}
	}
}

// polysoup.cpp:4745
void WPolySoup::UpdateBright(void) {
	if (m_pBgModel->m_pPuppet != 0) {
		m_pBgModel->m_pPuppet->UpdateLightSource(&g_lightset, false, 0);
	}

	for (int i = 0; i < m_numObjects; i++) {
		if (m_models[i]->m_pPuppet != 0) {
			m_models[i]->m_pPuppet->UpdateLightSource(&g_lightset, false, 0);
		}
	}
}

// polysoup.cpp:4762
void WPolySoup::ChangePointType(unsigned char from, unsigned char to) {
	sPoint *pPoint = m_pPoints;
	unsigned int num = m_numPoints;

	for (unsigned int i = 0; i < num; i++, pPoint++) {
		if (pPoint && pPoint->m_type == from) {
			pPoint->m_type = to;
			return;
		}
	}
}

// polysoup.cpp:4778
const std::vector<WPolySoup::sTriangle*> *WPolySoup::GetTriArray(int index) const {
	std::map<int, std::vector<sTriangle*> >::const_iterator it = m_triArrays.find(index);
	if (it == m_triArrays.end()) {
		return 0;
	}
	return &it->second;
}

// polysoup.h (no line marks: out-of-line, destroys m_desc only)
WPolySoup::sPoint::~sPoint() {
}
