// The JP 983 lobby pet IDummyPet (JpDummyPet.h), from the JP exe's bodies
// (node scripts/disasm.ts --jp 0x<va>). The puppet calls are the KR WPuppet
// ones; the JP WPuppet's 0xb73670 is the KR FindMotionData, 0xb86f10 the KR
// CalcBound and 0xb86c10 (bones of a motion time applied on the puppet's
// own matrix) what KR CalcBound spells ApplyBones(GetBones(..)).
#include "wmath.h"

#include "JpDummyPet.h"
#include "JpFxInterpreter.h"
#include "JpTanLines.h"

#include <ctype.h>
#include <math.h>
#include <string.h>

#include <algorithm>
#include <iterator>
#include <map>

#include "scenemanager.h"
#include "SoundManager.h"
#include "ItemManager.h"
#include "SharedDoc.h"
#include "MascotInfo.h"
#include "mascot.h"
#include "classdefine.h"
#include "commonutil.h"
#include "wbone.h"
#include "wpuppet.h"
#include "wresrcmng.h"
#include "wview.h"

// KR commonutil (?GetStrConst@@YIPBDPBDPAD@Z, JP 0x98c310); no KR header
// declares it (npc.cpp declares it the same way).
const char* GetStrConst(const char* pSrc, char* pDest);
// ?g_lightset@@3ULightSet@@A (JP 0xe33764); wscene.h does not declare it.
extern LightSet g_lightset;
// JP 0xf02454: the position sounds are played relative to (the listener).
extern WVector g_jpSoundListenerPos;

// lib: 0x4d3d40 (scalar deleting destructor), 0x4d27d0 (std::find_if on
// m_texParts by texture name), 0x4d2b40 (list node), 0x4d2770 (the records'
// delete loop), 0x4d2110 (map iterator ++), 0x4d3e60 (std::transform with
// tolower), 0x4d3f80 (string assign of a range), 0x4d22d0 (string
// operator[]), 0x45d5b0 (string == literal)

// JP 0x641240 (JpLobbyAvatar.cpp): the shared doc's named bone group, NULL
// without one.
const std::list<std::string>* Jp_SharedDoc_FindBoneGroup(const char* name);

// JP 0xe74c80: PlaySound's resolved name.
static char s_petSoundName[0x40];

// The caddie type id of a caddie item's type id (its caddie's group bits).
static uint32_t CadItemCaddieTid(uint32_t cadItemTid) {
	return (((cadItemTid >> 21) & 0x1f) + ((cadItemTid >> 16) & 0x1f)) | 0x1c000000;
}

// Shows the puppet's bones the script names (up to 64, until ')'), keeping
// them hidden-flagged unless kind is 9 (IPetScriptObject::FrameScript on a
// bare puppet).
// 0x5ac9b0
void Jp_PetScript_PuppetFrameScript(WPuppet* puppet, int kind, const char*& script) {
	char name[64] = "";
	for (int i = 0; i < 0x40; i++) {
		script = GetStrConst(script, name);
		size_t length = strlen(name);
		if (length == 0 || length >= 0x40) {
			break;
		}
		WBone* bone = puppet->FindBone(name);
		if (bone != NULL) {
			bone->SetInvisible(false, kind != 9);
		}
		if (script == NULL || *script == ')') {
			break;
		}
	}
}

// The same for the shared doc's bone groups the script names
// (IPetScriptObject::FrameKind on a bare puppet).
// 0x5acd10
void Jp_PetScript_PuppetFrameKind(WPuppet* puppet, bool kindIs13, const char*& script) {
	if (puppet == NULL) {
		return;
	}
	char name[256] = "";
	for (int i = 0; i < 0x40; i++) {
		script = GetStrConst(script, name);
		size_t length = strlen(name);
		if (length == 0 || length >= 0x100) {
			break;
		}
		const std::list<std::string>* group = Jp_SharedDoc_FindBoneGroup(name);
		if (group != NULL) {
			for (std::list<std::string>::const_iterator it = group->begin(); it != group->end(); ++it) {
				WBone* bone = puppet->FindBone(it->c_str());
				if (bone != NULL) {
					bone->SetInvisible(false, !kindIs13);
				}
			}
		}
		if (script == NULL || *script == ')') {
			break;
		}
	}
}

// Port bodies of the JP helpers this file reaches and does not decompile
// (G7; the VAs are on the declarations in JpDummyPet.h).
int Jp_FxInterpreter_AttachPuppet(JpFxInterpreter* fx, WPuppet* puppet) {
	(void)fx;
	(void)puppet;
	return 0;
}

// 0x4d1190
void IDummyPet::AttachScriptEffect(const char*& script, bool skip, int kind) {
	(void)script;
	(void)skip;
	(void)kind;
}

// 0x4d11a0
void IDummyPet::ApplyFrameScript(const char*& script, int kind) {
	Jp_PetScript_PuppetFrameScript(m_puppet, kind, script);
}

// 0x4d11c0
void IDummyPet::PlayScriptMotion(const char*& script) {
	(void)script;
}

// 0x4d11d0
void IDummyPet::ApplyFrameKind(const char*& script, int kind) {
	Jp_PetScript_PuppetFrameKind(m_puppet, kind == 13, script);
}

// 0x4d1200
void IDummyPet::ResetMotion(int param1, int param2, bool flag) {
	(void)param2;
	(void)flag;
	if (param1 == 0) {
		m_effectHost->StopEffect(m_effectId);
	} else {
		m_effectHost->StopAllEffects(-1);
	}
	m_effectId = Jp_FxInterpreter_AttachPuppet(m_effectHost, m_puppet);
}

// 0x4d1be0
void IDummyPet::PlaySound(const char*& name) {
	name = GetStrConst(name, s_petSoundName);
	size_t length = strlen(s_petSoundName);
	if (length == 0 || length >= 0x40) {
		return;
	}
	WVector position;
	position.x = m_matrix.pivot.x - g_jpSoundListenerPos.x;
	position.y = m_matrix.pivot.y - g_jpSoundListenerPos.y;
	position.z = m_matrix.pivot.z - g_jpSoundListenerPos.z;
	g_audio->PlaySfx(s_petSoundName, &position, 5, NULL, NULL, 0.5f, 200.0f);
}

// 0x4d1c80: *hideclub, as IDummyPlayer's 0x4d55a0 (0x9b0e70 = KR
// CSceneManager::SetVisible on 0xe741c0): the club element ("클럽") hidden.
void IDummyPet::PlaySceneSound(const char*& script) {
	(void)script;
	CSceneManager::Instance()->SetVisible("\xc5\xac\xb7\xb4", false);
}

// 0x4d1ca0
void IDummyPet::ScriptProcess(const w_motion_data* data, float from, float to, bool all, bool flag) {
	if (data == NULL) {
		return;
	}
	if ((!(from <= data->e) || from == to) && !all) {
		return;
	}
	int start;
	int end;
	from *= 30.0f;
	if (all) {
		start = Ceil(from, 0.03f);
		end = start;
	} else {
		to = (data->e <= to ? data->e : to) * 30.0f;
		start = Ceil(from, 0.03f);
		end = WAbsBySign((float)floor(to) - to) < g_EPSILON ? (int)to - 1 : (int)floor(to);
		if (start == (int)floor(data->s * 30.0f + 0.5f)) {
			start++;
		}
		if (start > end) {
			return;
		}
	}
	int first;
	int last;
	if (m_puppet->CheckFrameData(start, end, first, last)) {
		for (int i = first; i <= last; i++) {
			ParseScript(m_puppet->GetFrameData(i), flag);
		}
	}
}

// 0x4d1e50
void IDummyPet::SetPosition(const WVector* position) {
	m_matrix.xa = WVector(1.0f, 0.0f, 0.0f);
	m_matrix.ya = WVector(0.0f, 1.0f, 0.0f);
	m_matrix.za = WVector(0.0f, 0.0f, 1.0f);
	m_matrix.pivot = WVector(0.0f, 0.0f, 0.0f);
	m_matrix.ya = WVector::UNIT_POS_Y;
	m_matrix.xa = WCrossProduct(m_matrix.ya, m_matrix.za);
	m_matrix.pivot = *position;
	m_position = *position;
}

// 0x4d2500
void IDummyPet::Display() {
	if (!m_motionPlaying) {
		return;
	}
	WPuppet* puppet = m_puppet;
	if (puppet == NULL) {
		return;
	}
	puppet->Transform(g_view, 1.0f, 0);
	m_puppet->UpdateBSphere();
	if (!g_view->InFrustum(m_puppet->GetBoundSphere())) {
		return;
	}
	m_puppet->UpdateLightSource(&g_lightset, false, NULL);
	m_puppet->Render(g_view, true, 1.0f, g_view->InFrustumSafe(m_puppet->GetBoundBox()), false, false);
}

// 0x4d25a0
bool IDummyPet::PlayMotion(const char* name) {
	if (m_puppet != NULL && m_puppet->GetRootBone() != NULL) {
		m_motion = m_puppet->FindMotionData(name);
		if (m_motion == NULL) {
			m_motionHold = false;
			return false;
		}
		m_puppet->GetRootBone()->SetInvisible(true, false);
		float start = m_motion->s;
		float end = m_motion->e;
		m_motionTime = end < start ? end : start;
		ScriptProcess(m_motion, m_motion->s, m_motion->s, true, false);
		return true;
	}
	return false;
}

// 0x4d2890
bool IDummyPet::SetMotionByName(const char* name) {
	m_lightAngle = 3.1415927f;
	m_motionTime = 0.0f;
	m_matrix.xa = WVector(1.0f, 0.0f, 0.0f);
	m_matrix.ya = WVector(0.0f, 1.0f, 0.0f);
	m_matrix.za = WVector(0.0f, 0.0f, 1.0f);
	m_matrix.pivot = WVector(0.0f, 0.0f, 0.0f);
	m_curTexPart = NULL;
	m_unknownAC = 0;
	m_motionPlaying = false;
	m_motionHold = false;
	m_motionReady = false;
	if (m_effectHost == NULL) {
		m_effectHost = new JpFxInterpreter(NULL, 0);
	}
	if (name != NULL) {
		PlayMotion(name);
	}
	ResetMotion(0, 0, true);
	m_motionReady = true;
	return true;
}

// 0x4d2980
void IDummyPet::Process(float dt) {
	if (!m_motionHold) {
		return;
	}
	w_motion_data* motion = m_motion;
	if (motion == NULL) {
		return;
	}
	float time = dt + m_motionTime;
	m_motionTime = time;
	ScriptProcess(motion, time - dt, time, false, false);
	if (!(m_motion->e < m_motionTime)) {
		m_puppet->GetMatrix() = m_matrix;
		m_puppet->ApplyBones(m_puppet->GetBones(NULL, m_motionTime, 0, NULL, false));
		m_puppet->UpdateBound(true, true);
		return;
	}
	if (m_motion->s == m_motion->e) {
		m_motionHold = false;
		return;
	}
	PlayMotion(m_motion->nextmotion);
}

// 0x4d3150
bool IDummyPet::BuildMascot(uint32_t typeId, bool skipRegister) {
	if (m_kind != 5) {
		return false;
	}
	IFF_STRUCT::sMascot* mascot = ItemManager()->FindMascot(typeId);
	if (mascot == NULL) {
		return false;
	}
	m_puppet = g_resrcmng->GetPuppet(MakeStr("%s.pet", mascot->m_model), false, true, false);
	if (m_puppet == NULL) {
		return false;
	}
	// The owned mascot of that type (JP CSharedDoc +0x3728, the KR
	// m_myMascotMap) and its index: its message goes into the texture.
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	int index = 0;
	for (std::map<unsigned int, sMascotInfo>::iterator it = doc->m_myMascotMap.begin(); it != doc->m_myMascotMap.end(); ++it, index++) {
		if (it->second.typeId == typeId) {
			IFF_STRUCT::sMascot* owned = ItemManager()->FindMascot(typeId);
			if (owned != NULL) {
				if (owned->m_msgInTexture) {
					m_msgTexHandle = CMascot::ChangeMascotMsgInTex(it->second.message, owned, m_puppet, MakeStr("ui%02d", index));
				}
				break;
			}
		}
	}
	m_puppet->CalcBound(NULL, true);
	if (!skipRegister) {
		CSceneManager::Instance()->RegisterElement((eObjType)4, NULL, NULL, m_name.c_str(), 0x18, NULL);
		CSceneManager::Instance()->SetPuppet(m_name.c_str(), m_puppet);
	}
	return true;
}

// 0x4d34b0
bool Jp_DummyPet_ParseTexturePart(const w_motion_data* motion, std::string& part, std::string& texture, const char*& script) {
	char token[0x40];
	char name[0x40];
	char path[0x400];
	token[0] = '\0';
	path[0] = '\0';
	script = GetStrConst(script, token);
	size_t length = strlen(token);
	if (length == 0 || length >= 0x40) {
		return false;
	}
	if (token[0] != '@') {
		strcpy(name, token);
	} else {
		// "@motion name path": only for the current motion.
		if (motion == NULL) {
			return false;
		}
		if (_stricmp(token + 1, motion->name) != 0) {
			return false;
		}
		script = GetStrConst(script, name);
		length = strlen(name);
		if (length == 0 || length >= 0x40) {
			return false;
		}
	}
	script = GetStrConst(script, path);
	length = strlen(path);
	if (length == 0 || length >= 0x400) {
		return false;
	}
	std::string found;
	JpTanLines::Instance()->FindTexture(name, path, found, NULL);
	part = name;
	texture = found.c_str();
	return true;
}

// 0x4d37d0
IDummyPet::IDummyPet(int kind)
	: m_puppet(NULL),
	  m_kind(kind) {
	m_msgTexHandle = 0;
	m_motion = NULL;
	m_curTexPart = NULL;
	m_motionReady = false;
	m_effectHost = NULL;
	m_effectId = 0;
}

// 0x4d38c0
IDummyPet::~IDummyPet() {
	for (std::list<JpPetTexturePart*>::iterator it = m_texParts.begin(); it != m_texParts.end(); ++it) {
		delete *it;
	}
	if (m_effectHost != NULL) {
		delete m_effectHost;
		m_effectHost = NULL;
	}
	if (m_puppet != NULL && g_resrcmng != NULL) {
		g_resrcmng->Release(m_puppet, false);
		m_puppet = NULL;
	}
	m_motionReady = false;
	m_effectId = 0;
	if (g_resrcmng != NULL && m_msgTexHandle != 0) {
		g_resrcmng->Release(m_msgTexHandle);
		m_msgTexHandle = 0;
	}
	m_texParts.clear();
}

// 0x4d3a50
void IDummyPet::SetTexturePart(const char* part, const char* texture) {
	std::list<JpPetTexturePart*>::iterator it = m_texParts.begin();
	for (; it != m_texParts.end(); ++it) {
		if (_stricmp((*it)->m_texName, texture) == 0) {
			break;
		}
	}
	if (it == m_texParts.end()) {
		JpPetTexturePart* record = new JpPetTexturePart;
		memset(record->m_animName, 0, sizeof(record->m_animName));
		memset(record->m_texName, 0, sizeof(record->m_texName));
		record->m_bitmap = NULL;
		memset(&record->m_rect, 0, sizeof(record->m_rect));
		strcpy(record->m_animName, part);
		strcpy(record->m_texName, texture);
		m_texParts.push_back(record);
		m_curTexPart = record;
	} else if ((*it) != m_curTexPart) {
		m_curTexPart = *it;
	}
	JpPetTexturePart* record = m_curTexPart;
	if (record == NULL) {
		return;
	}
	if (m_kind != 12) {
		if (m_puppet != NULL) {
			if (record->m_bitmap != NULL) {
				m_puppet->ChangeTexturePart(record->m_animName, record->m_bitmap, record->m_rect);
			} else {
				m_puppet->ChangeTexturePart(record->m_animName, record->m_texName, &record->m_bitmap, &record->m_rect);
			}
		}
	} else if (m_puppet != NULL) {
		int original = g_resrcmng->LoadTexture(part, 0, 0, NULL);
		if (original != 0) {
			int replacement = g_resrcmng->LoadTexture(texture, 0, 0, NULL);
			if (replacement != 0) {
				m_puppet->ChangeTextureByHandle(replacement, original, m_puppet);
			}
		}
	}
}

// 0x4d3d70
void IDummyPet::RunMotionScript(const char*& script) {
	std::string part;
	std::string texture;
	if (Jp_DummyPet_ParseTexturePart(m_motion, part, texture, script)) {
		SetTexturePart(part.c_str(), texture.c_str());
	}
}

// 0x4d3fc0
bool IDummyPet::BuildCaddie(uint32_t typeId, uint32_t cadItemTypeId, bool skipRegister) {
	if (m_kind != 0) {
		return false;
	}
	IFF_STRUCT::sCaddie* caddie = ItemManager()->FindCaddie(typeId);
	if (caddie == NULL) {
		return false;
	}
	if (cadItemTypeId == 0) {
		m_puppet = g_resrcmng->GetPuppet(MakeStr("%s.pet", caddie->m_model), false, true, false);
	} else {
		IFF_STRUCT::sCadItem* item = ItemManager()->FindCadItem(cadItemTypeId);
		if (item == NULL) {
			return false;
		}
		const char* bodyTexture = item->m_bodyTexture;
		std::string model(bodyTexture);
		std::string lower;
		std::transform(model.begin(), model.end(), std::back_inserter(lower), ::tolower);
		size_t exPos = lower.find("_ex_", 0, 4);
		uint32_t itemCaddie = CadItemCaddieTid(item->m_typeId);
		bool exModel = false;
		// The bodies with a model of their own; the others fall to the
		// "_ex_" rule (or keep the texture name, which loads nothing).
		if (lower == "caddie_quma_ex_01.jpg") {
			model.assign("quma_ex_02.pet", 0xe);
		} else if (lower == "caddie_dol_ex_01.jpg") {
			model.assign("dol_ex_02.pet", 0xd);
		} else if (lower == "pipin3_bd_01-01.jpg") {
			model.assign("pipin_ex_02.pet", 0xf);
		} else if (lower == "[2#caddie_cadie_004.jpg") {
			model.assign("cadie_ex_02.pet", 0xf);
		} else if (lower == "[2#ho_body_001_02.jpg") {
			model.assign("ho_mesh_ex02.pet", 0x10);
		} else if (lower == "[2#ho_body_002_04.jpg") {
			model.assign("ho_mesh_ex03.pet", 0x10);
		} else if (lower == "caddie_maid_body_001.jpg") {
			if (itemCaddie != 0x1c00001f) {
				if ((item->m_typeId & 0x1fff) >= 3) {
					model.assign("maid_12.pet", 0xb);
				} else {
					model.assign("maid.pet", 8);
				}
			} else if ((item->m_typeId & 0x1fff) >= 3) {
				model.assign("maid_13.pet", 0xb);
			} else {
				model.assign("maid_1.pet", 0xa);
			}
		} else if (lower == "caddie_maid_body_002.jpg") {
			if (itemCaddie != 0x1c00001f) {
				if ((item->m_typeId & 0x1fff) >= 3) {
					model.assign("maid2_2.pet", 0xb);
				} else {
					model.assign("maid2.pet", 9);
				}
			} else if ((item->m_typeId & 0x1fff) >= 3) {
				model.assign("maid2_3.pet", 0xb);
			} else {
				model.assign("maid2_1.pet", 0xb);
			}
		} else if (lower == "caddie_maid_body_003.jpg") {
			if (itemCaddie != 0x1c00001f) {
				if ((item->m_typeId & 0x1fff) >= 3) {
					model.assign("maid3_2.pet", 0xb);
				} else {
					model.assign("maid3.pet", 9);
				}
			} else if ((item->m_typeId & 0x1fff) >= 3) {
				model.assign("maid3_3.pet", 0xb);
			} else {
				model.assign("maid3_1.pet", 0xb);
			}
		} else if (lower == "[2#cien_body-01.jpg") {
			if (itemCaddie == 0x1c000021) {
				model.assign("Cien_Armor.pet", 0xe);
			} else if (itemCaddie == 0x1c000022) {
				model.assign("Cien_Secret.pet", 0xf);
			} else if (itemCaddie == 0x1c000023) {
				model.assign("Cien_Rare.pet", 0xd);
			}
		} else if (lower == "[2#cien_body-02.jpg") {
			if (itemCaddie == 0x1c000021) {
				model.assign("Cien_Armor_W.pet", 0x10);
			} else if (itemCaddie == 0x1c000022) {
				model.assign("Cien_Secret_W.pet", 0x11);
			} else if (itemCaddie == 0x1c000023) {
				model.assign("Cien_Rare_W.pet", 0xf);
			}
		} else if (lower == "[2#cien_body-03.jpg") {
			if (itemCaddie == 0x1c000023) {
				model.assign("Cien_Rare_S.pet", 0xf);
			}
		} else if (lower == "[2#cien_body-04.jpg") {
			if (itemCaddie == 0x1c000021) {
				model.assign("Cien_Armor_WS.pet", 0x11);
			} else if (itemCaddie == 0x1c000022) {
				model.assign("Cien_Secret_WS.pet", 0x12);
			}
		} else if (lower == "[2#cien_elemental_01.jpg") {
			if (itemCaddie == 0x1c000021) {
				model.assign("Cien_Armor_e.pet", 0x10);
			} else if (itemCaddie == 0x1c000022) {
				model.assign("Cien_Secret_e.pet", 0x11);
			} else if (itemCaddie == 0x1c000023) {
				model.assign("Cien_Rare_e.pet", 0xf);
			}
		} else if (lower == "[2#karen_003.jpg") {
			// Jump table 0x4d47b0 on the item's caddie 0x1c000025..0x1c00002c.
			switch (itemCaddie) {
			case 0x1c000025:
			case 0x1c000029:
				model.assign("karen.pet", 9);
				break;
			case 0x1c000027:
			case 0x1c00002b:
				model.assign("karen_ex_n.pet", 0xe);
				break;
			case 0x1c000026:
			case 0x1c000028:
			case 0x1c00002a:
			case 0x1c00002c:
				model.assign("karen_r_n.pet", 0xd);
				break;
			}
		} else if (lower == "2#karen_p_001.jpg" || lower == "2#karen_r_p_001.jpg" || lower == "2#karen_ex_p_001.jpg") {
			// Jump table 0x4d47d0.
			switch (itemCaddie) {
			case 0x1c000025:
			case 0x1c000029:
				model.assign("karen_p.pet", 0xb);
				break;
			case 0x1c000027:
			case 0x1c00002b:
				model.assign("karen_ex_p.pet", 0xe);
				break;
			case 0x1c000026:
			case 0x1c000028:
			case 0x1c00002a:
			case 0x1c00002c:
				model.assign("karen_r_p.pet", 0xd);
				break;
			}
		} else if (exPos == std::string::npos) {
			exModel = true;
		} else {
			// "<name>_ex" from the first lower-case letter, plus ".pet".
			size_t first = 0;
			while (first < lower.size()) {
				char c = lower[first];
				if (c >= 'a' && c <= 'z') {
					break;
				}
				first++;
			}
			model.assign(lower.begin() + first, lower.begin() + exPos + 3);
			model.append(".pet", 4);
		}
		if (exModel) {
			m_puppet = g_resrcmng->GetPuppet(MakeStr("%s_ex.pet", caddie->m_model), false, true, false);
		} else {
			m_puppet = g_resrcmng->GetPuppet(model.c_str(), false, true, false);
		}
		if (m_puppet == NULL) {
			return false;
		}
		if (strlen(item->m_faceTexture) != 0) {
			SetTexturePart("face", item->m_faceTexture);
		}
		if (strlen(bodyTexture) != 0) {
			SetTexturePart("body", bodyTexture);
		}
	}
	m_puppet->CalcBound(NULL, true);
	if (!skipRegister) {
		CSceneManager::Instance()->RegisterElement((eObjType)4, NULL, NULL, m_name.c_str(), 0x18, NULL);
		CSceneManager::Instance()->SetPuppet(m_name.c_str(), m_puppet);
	}
	return true;
}
