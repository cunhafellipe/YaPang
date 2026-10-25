// The JP 983 lobby avatar IDummyPlayer (JpLobbyAvatar.h), from the JP exe's
// bodies (node scripts/disasm.ts --jp 0x<va>), and the IPetScriptObject
// helpers 0x5ac900..0x5acbf0. The script parser 0x5ac510 and the motion
// command reader 0x5ace20 are declared JP dependencies; the effect host is
// JpFxInterpreter (JpFxInterpreter.cpp).
#include "JpLobbyAvatar.h"
#include "JpPetFrameGlue.h"
#include "JpTanLines.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <vector>

#include "SceneManager.h"
#include "SoundManager.h"
#include "GolfDoc.h"
#include "ItemManager.h"
#include "SharedDoc.h"
#include "UccLibrary.h"
#include "UccManager.h"
#include "classdefine.h"
#include "commonutil.h"
#include "fx.h"
#include "partsinfo.h"
#include "parttidlist.h"
#include "quadtree.h"
#include "wbone.h"
#include "wpuppet.h"
#include "wview.h"
#include "shared/luasystem/LuaScript.h"
#include "shared/luasystem/LuaScriptManager.h"
#include "shared/luasystem/luatablehelper.h"

// KR commonutil (?GetStrConst@@YIPBDPBDPAD@Z, JP 0x98c310); no KR header
// declares it (npc.cpp declares it the same way).
const char* GetStrConst(const char* pSrc, char* pDest);
// ?g_lightset@@3ULightSet@@A (JP 0xe33764); wscene.h does not declare it.
extern LightSet g_lightset;

// JP 0xf02454: the position sounds are played relative to (the listener).
extern WVector g_jpSoundListenerPos;
// JP 0x6620b0 / 0x661fb0 (CGolfDoc, on [CSharedDoc +0x11024]): fill an
// outside part list (the KR SetPartTidList fills the doc's own list by
// index); the second variant differs only by the CPartTidList setter it
// calls (0x8b32e0 instead of 0x8b3330).
// 0x8b3330 is the KR CPartTidList::SetTids(sCharacterInfo*, level) with a
// JP pre-pass over the 24 slots (the item manager's parts, +0xec slot masks
// against the later slots) that is not decompiled: both variants call the
// KR SetTids (which is 0x8b32e0) until it is. The record is the KR
// sCharacterInfo (the JP one has the parts at +0xc and the ids at +0x6c);
// the arrays are copied slot by slot (unsigned long is 8 bytes on LP64).
static void Jp_GolfDoc_FillCharInfo(sCharacterInfo& charInfo, uint32_t tidChar, uint8_t hairColor, uint8_t shirtColor, const DWORD* partTids, const DWORD* partIds) {
	charInfo.typeId = tidChar;
	charInfo.hairColor = hairColor;
	charInfo.shirtColor = shirtColor;
	for (int i = 0; i < 24; i++) {
		charInfo.partTypeIds[i] = partTids[i];
	}
	if (partIds != NULL) {
		for (int i = 0; i < 24; i++) {
			charInfo.partIds[i] = partIds[i];
		}
	}
}

// 0x6620b0
void Jp_GolfDoc_SetPartTidList(CPartTidList* list, uint32_t tidChar, uint8_t hairColor, uint8_t shirtColor, DWORD* partTids, DWORD* partIds) {
	sCharacterInfo charInfo;
	Jp_GolfDoc_FillCharInfo(charInfo, tidChar, hairColor, shirtColor, partTids, partIds);
	if (!list->SetTids(&charInfo, 0xff)) {
		list->SetDefaultTids();
	}
	if (!list->IsComboValid()) {
		list->SetDefaultTids();
	}
}

// 0x661fb0
void Jp_GolfDoc_SetPartTidListAlt(CPartTidList* list, uint32_t tidChar, uint8_t hairColor, uint8_t shirtColor, DWORD* partTids, DWORD* partIds) {
	sCharacterInfo charInfo;
	Jp_GolfDoc_FillCharInfo(charInfo, tidChar, hairColor, shirtColor, partTids, partIds);
	if (!list->SetTids(&charInfo, 0xff)) {
		list->SetDefaultTids();
	}
	if (!list->IsComboValid()) {
		list->SetDefaultTids();
	}
}
// JP 0x8b0960 (PartsInfo unit, not in KR): applies a UCC clothes record to
// the frame's parts.
void Jp_PetFrame_ApplyUccClothes(CPetFrame* frame, sUccClothes* clothes);
// JP 0x641240 (CSharedDoc, on 0xe47dbc): the named bone group, NULL
// without one. It is the KR CSharedDoc::GetVisGroup (0x41a6c0): the same
// find on the std::map<std::string, std::list<std::string> > (JP +0x11414,
// KR m_visGroups).
// 0x641240
const std::list<std::string>* Jp_SharedDoc_FindBoneGroup(const char* name) {
	return WSingleton<CSharedDoc>::Instance()->GetVisGroup(name);
}
// JP 0x8af620 (PartsInfo unit): whether one of the frame's loaded pets
// (count +0x6c, records +0x74, name +0x2c) is named so (case-insensitive),
// then the same test over the JP frame's second puppet container (+0x334,
// 0x8af2d0, a std::map walk). The first part is the KR CPetFrame::HasMPet;
// the KR frame has no second container.
// 0x8af620
bool Jp_PetFrame_HasPet(CPetFrame* frame, const char* name) {
	return frame->HasMPet(const_cast<char*>(name));
}

// JP 0xe74cc8: PlaySound's resolved name.
static char s_soundName[0x40];

// 0x4d4940
void IDummyPlayer::ParsePuppetScript(const char* script, bool flag) {
	ParseScript(script, flag);
}

// 0x4d5500
void IDummyPlayer::PlaySound(const char*& name) {
	name = GetStrConst(name, s_soundName);
	size_t length = strlen(s_soundName);
	if (length == 0 || length >= 0x40) {
		return;
	}
	WVector position;
	position.x = m_matrix.pivot.x - g_jpSoundListenerPos.x;
	position.y = m_matrix.pivot.y - g_jpSoundListenerPos.y;
	position.z = m_matrix.pivot.z - g_jpSoundListenerPos.z;
	g_audio->PlaySfx(s_soundName, &position, 5, NULL, NULL, 0.5f, 200.0f);
}

// 0x4d55a0
void IDummyPlayer::HideClub(const char*& script) {
	(void)script;
	CSceneManager::Instance()->SetVisible("\xc5\xac\xb7\xb4", false);
}

// 0x4d55c0
void IDummyPlayer::SetPosition(const WVector* position) {
	m_matrix.xa = WVector(1.0f, 0.0f, 0.0f);
	m_matrix.ya = WVector(0.0f, 1.0f, 0.0f);
	m_matrix.za = WVector(0.0f, 0.0f, 1.0f);
	m_matrix.pivot = WVector(0.0f, 0.0f, 0.0f);
	m_matrix.ya = WVector::UNIT_POS_Y;
	m_matrix.xa = WCrossProduct(m_matrix.ya, m_matrix.za);
	m_matrix.pivot = GetPVS().GetGroundPoint(*position, false, NULL, false);
	m_position = *position;
}

// 0x4d5ed0
void IDummyPlayer::Display() {
	if (!m_motionHold) {
		return;
	}
	CPetFrame* frame = m_petFrame.get();
	if (frame == NULL) {
		return;
	}
	JpDummyPlayerFaceAnim* face = m_pendingFace;
	if (face == NULL || face == m_appliedFace) {
		return;
	}
	m_appliedFace = face;
	// JP 0xb69e20 / 0xb69e30 are the KR WPuppet::ChangeTexturePart pair.
	if (face->m_bitmap == NULL) {
		frame->GetBonePet()->ChangeTexturePart(frame->GetFaceTex(), face->m_name, &face->m_bitmap, &face->m_rect);
	} else {
		frame->GetBonePet()->ChangeTexturePart(frame->GetFaceTex(), face->m_bitmap, face->m_rect);
	}
	m_petFrame.get()->SetMakeup();
}

// 0x4d5f60
void IDummyPlayer::ApplyFrameScript(const char*& script, int kind) {
	FrameScript(m_petFrame.get(), kind, script);
}

// 0x4d5f80
void IDummyPlayer::ApplyFrameKind(const char*& script, int kind) {
	FrameKind(m_petFrame.get(), kind == 13, script);
}

// 0x4d5fa0
void IDummyPlayer::RefreshAfterPartChange(int keepEffects, int attachFrame, int param3, bool flag) {
	(void)param3;
	if (keepEffects == 0) {
		m_effectHost->StopEffect(m_effectId);
	} else {
		m_effectHost->StopAllEffects(-1);
	}
	if (attachFrame == 0) {
		float range[2];
		range[0] = 3.402823466e+38f;
		range[1] = 3.402823466e+38f;
		m_effectId = m_effectHost->AttachRange(m_petFrame.get(), range, 1, 0);
	} else {
		m_effectId = m_effectHost->AttachFrame(m_petFrame.get(), NULL, NULL, flag, 0);
	}
	m_effectHost->BindOwner(this, "\xb4", NULL);
	m_effectHost->BindOwner(this, "\xbf", NULL);
}

// 0x4d6040
void IDummyPlayer::ScriptProcess(const w_motion_data* data, float from, float to, bool all, bool flag) {
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
	if (m_petFrame.get()->GetCurAniPet()->CheckFrameData(start, end, first, last)) {
		for (int i = first; i <= last; i++) {
			ParsePuppetScript(m_petFrame.get()->GetCurAniPet()->GetFrameData(i), flag);
		}
	}
}

// 0x4d6aa0
bool IDummyPlayer::PlayMotion(const char* name) {
	m_motionState = m_petFrame.get()->FindMotionData(name, true, NULL);
	if (m_motionState == NULL) {
		m_motionPlaying = false;
		return false;
	}
	float start = m_motionState->s;
	float end = m_motionState->e;
	m_motionEnd = end < start ? end : start;
	ScriptProcess(m_motionState, m_motionState->s, m_motionState->s, true, false);
	return true;
}

// 0x4d6f80
bool IDummyPlayer::SetMotionByName(const char* name, int param) {
	if (name == NULL) {
		return false;
	}
	m_lightAngle = 3.1415927f;
	m_light.type = LightSet::W_LIGHT_DIRECTIONAL;
	m_light.ambient = ModifyColor(g_lightset.ambient, 1.0f);
	m_light.nearone = g_lightset.nearone;
	m_motionEnd = 0.0f;
	m_matrix.xa = WVector(1.0f, 0.0f, 0.0f);
	m_matrix.ya = WVector(0.0f, 1.0f, 0.0f);
	m_matrix.za = WVector(0.0f, 0.0f, 1.0f);
	m_matrix.pivot = WVector(0.0f, 0.0f, 0.0f);
	m_pendingFace = NULL;
	m_appliedFace = NULL;
	m_motionHold = false;
	m_effectFlag = param != 0;
	if (m_effectHost == NULL) {
		m_effectHost = new JpFxInterpreter(NULL, 0);
	}
	PlayMotion(name);
	RefreshAfterPartChange(0, param, 0, true);
	m_motionReady = true;
	return true;
}

// 0x4d70c0
void IDummyPlayer::Process(float dt) {
	if (!m_motionPlaying) {
		return;
	}
	w_motion_data* motion = m_motionState;
	if (motion == NULL || !m_motionHold) {
		return;
	}
	float time = dt + m_motionEnd;
	m_motionEnd = time;
	ScriptProcess(motion, time - dt, time, false, false);
	motion = m_motionState;
	if (m_motionEnd <= motion->e) {
		// JP 0x4d18f0 is the bone puppet's matrix store (WPuppet::GetMatrix() = m).
		m_petFrame.get()->GetBonePet()->GetMatrix() = m_matrix;
		Jp_PetFrame_Animate(m_petFrame.get(), NULL, NULL, m_motionEnd);
		m_petFrame.get()->GetBonePet()->UpdateBound(true, true);
	} else if (motion->s == motion->e) {
		m_motionPlaying = false;
	} else {
		PlayMotion(motion->nextmotion);
	}
	if (!g_view->InFrustum(m_petFrame.get()->GetBonePet()->GetBoundBox())) {
		return;
	}
	// The JP reads the head bone's position at +0x164 (the JP WBone).
	WBone* head = m_petFrame.get()->GetBonePet()->FindBoneByTail("Head");
	m_light.diffuse = ModifyColor(g_lightset.diffuse, GetPVS().GetShadowIntensity(head->GetMatrix().pivot));
	m_petFrame.get()->UpdateLightSource(&m_light, false, NULL);
}

// 0x4d7200
void IDummyPlayer::PlayScriptMotion(const char*& script) {
	const char* name = FindScriptMotion(m_petFrame.get(), script);
	if (name != NULL) {
		PlayMotion(name);
	}
}

// 0x4d78f0
IDummyPlayer::IDummyPlayer(int kind)
	: m_petFrame(static_cast<CPetFrame*>(NULL)),
	  m_kind(kind),
	  m_name(),
	  m_lightAngle(0.0f),
	  m_light(),
	  m_position(WVector::ZERO),
	  m_motionState(NULL),
	  m_faceAnims(),
	  m_pendingFace(NULL),
	  m_effectFlag(true),
	  m_effectHost(NULL),
	  m_effectId(0) {
	m_unknown8c[0] = 0;
	m_unknown8c[1] = 0;
	m_unknown8c[2] = 0;
	m_unknown9c[0] = 0;
	m_unknown9c[1] = 0;
	m_unknown9c[2] = 0;
}

// 0x4d7a30 (the vtable's 0x4d8ef0 is the scalar deleting destructor)
IDummyPlayer::~IDummyPlayer() {
	m_faceAnims.clear();
	delete m_effectHost;
	m_effectHost = NULL;
	if (m_petFrame.get() != NULL) {
		m_petFrame.reset();
		m_petFrame = RF_SHARED_PTR<CPetFrame>(static_cast<CPetFrame*>(NULL));
	}
	m_motionReady = false;
}

// 0x4d7c60
bool IDummyPlayer::BuildParts(DWORD* partTids, DWORD* auxTids, DWORD* partIds, uint8_t hairColor, uint8_t shirtColor, bool skipRegister, bool altList) {
	if (m_petFrame.get() == NULL) {
		m_petFrame = RF_SHARED_PTR<CPetFrame>(new CPetFrame());
	}
	CPartTidList list;
	if (altList) {
		Jp_GolfDoc_SetPartTidListAlt(&list, m_typeId | 0x4000000, hairColor, shirtColor, partTids, partIds);
		m_altPartList = true;
	} else {
		Jp_GolfDoc_SetPartTidList(&list, m_typeId | 0x4000000, hairColor, shirtColor, partTids, partIds);
		m_altPartList = false;
	}
	// The JP CPetFrame::Build takes a sixth argument, this avatar (its
	// IPetScriptObject); the KR Build has five.
	m_petFrame.get()->Build(&list, auxTids, true, true, -1);
	m_petFrame.get()->CalcBVolume();
	if (!skipRegister) {
		CSceneManager::Instance()->RegisterElement((eObjType)4, NULL, NULL, m_name.c_str(), 0x18, NULL);
		CSceneManager::Instance()->SetPuppet(m_name.c_str(), m_petFrame.get()->GetBonePet());
	}
	if (partIds != NULL) {
		for (int i = 0; i < 24; i++) {
			uint32_t id = partIds[i];
			if (id != 0 && id != 0xffffffff && IsUccClothes(partTids[i])) {
				sUccClothes* clothes = UccManager()->FindClothes(partIds[i]);
				if (clothes != NULL) {
					Jp_PetFrame_ApplyUccClothes(m_petFrame.get(), clothes);
				}
			}
		}
	}
	uint32_t faceTid = list.m_partTids[1];
	if (faceTid != 0 && (faceTid & 0x1800) == 0x1000) {
		IFF_STRUCT::sChar* character = ItemManager()->FindChar(m_typeId | 0x4000000);
		if (character != NULL) {
			char faceTexName[256];
			strcpy(faceTexName, character->m_faceTexture);
			sprintf(strrchr(faceTexName, '.'), "_%d%s", 1, strrchr(character->m_faceTexture, '.'));
			int faceTex = g_resrcmng->LoadTexture(character->m_faceTexture, 0, 0, faceTexName);
			w_texlist* texture = g_resrcmng->FindTexture(character->m_faceTexture);
			WPuppet* bonePet = m_petFrame.get()->GetBonePet();
			if (texture != NULL && bonePet != NULL) {
				bonePet->ChangeTextureByHandle(faceTex, texture->texhandle, NULL);
				CPetFrame* frame = m_petFrame.get();
				if (frame != NULL) {
					bonePet->ChangeTexturePart(faceTex, frame->GetMakeupTexture(), NULL, NULL);
				}
			}
		}
	}
	return true;
}

bool IDummyPlayer::BuildParts(const sCharacterInfo& info, bool skipRegister, bool altList) {
	sCharacterInfo& record = const_cast<sCharacterInfo&>(info);
	return BuildParts(record.partTypeIds, record.auxParts.tids, record.partIds, record.hairColor, record.shirtColor, skipRegister, altList);
}

// 0x4d7fb0
bool IDummyPlayer::BuildFromPartTidList(CPartTidList& parts, DWORD* auxTids, bool flag) {
	if (m_petFrame.get() == NULL) {
		// The JP CPetFrame is 0x358 bytes (ctor 0x8b2290).
		m_petFrame = RF_SHARED_PTR<CPetFrame>(new CPetFrame());
	}
	// The JP CPetFrame::Build takes a sixth argument, this avatar (its
	// IPetScriptObject); the KR Build has five.
	m_petFrame.get()->Build(&parts, auxTids, true, true, -1);
	m_petFrame.get()->CalcBVolume();
	if (!flag) {
		CSceneManager::Instance()->RegisterElement((eObjType)4, NULL, NULL, m_name.c_str(), 0x18, NULL);
		CSceneManager::Instance()->SetPuppet(m_name.c_str(), m_petFrame.get()->GetBonePet());
	}
	return true;
}

// 0x4d80f0
bool IDummyPlayer::RandomDressUp(lua_system::lua_script* script, DWORD* partTids, DWORD* auxTids) {
	uint32_t typeId = m_typeId;
	bool ownScript = false;
	if (script == NULL) {
		script = lua_system::lua_script_manger::GetInstance()->CreateScript(false);
		if (script != NULL) {
			script->RunFile("grandprix_ceremony.lua");
		}
		ownScript = true;
	}
	std::vector<int> positions;
	lua_system::lua_table_helper table = script->GetTable("essentialPosition");
	int count = lua_system::lua_table_helper(table.GetValueByIndex<lua_tinker::table>(typeId + 1)).GetArraySize();
	for (int i = 1; i <= count; i++) {
		positions.push_back(lua_system::lua_table_helper(table.GetValueByIndex<lua_tinker::table>(typeId + 1)).GetValueByIndex<int>(i));
	}
	memset(partTids, 0, 0x60);
	memset(auxTids, 0, 0x14);
	ItemManager()->GetDefCombo(m_typeId | 0x4000000, partTids);
	CItemManager* items = ItemManager();
	if (items->m_partMap.lower_bound((typeId | 0x200) << 18) == items->m_partMap.upper_bound((typeId << 18) | 0x802e0ff)) {
		return false;
	}
	CPartTidList list;
	Jp_GolfDoc_SetPartTidListAlt(&list, m_typeId | 0x4000000, 0, 0, partTids, NULL);
	if (ownScript && script != NULL) {
		lua_system::lua_script_manger::GetInstance()->ReleaseScript(script);
	}
	return true;
}

void IDummyPlayer::RandomDressUp(sCharacterInfo* info) {
	RandomDressUp(NULL, info->partTypeIds, info->auxParts.tids);
}

// 0x4d8580
void IDummyPlayer::AttachScriptEffect(const char*& script, bool skip, int kind) {
	if (skip) {
		return;
	}
	std::vector<int> partTids;
	char token[64];
	token[0] = '\0';
	script = GetStrConst(script, token);
	if (strlen(token) - 1 > 0x3e) {
		return;
	}
	char name[64];
	if (token[0] == '@') {
		if (m_motionState == NULL || strcmp(token + 1, m_motionState->name) != 0) {
			return;
		}
		script = GetStrConst(script, name);
		int length = (int)strlen(name);
		if (length < 1 || length >= 0x40) {
			return;
		}
	} else {
		strcpy(name, token);
		token[0] = '\0';
	}
	char part[64];
	script = GetStrConst(script, part);
	int length = (int)strlen(part);
	if (length == 0 || length >= 0x40) {
		return;
	}
	if (part[0] == '&') {
		bool found = false;
		int tid;
		do {
			tid = atoi(part + 1);
			if (!found && m_petFrame.get()->GetPartList().FindPart(tid)) {
				found = true;
				partTids.push_back(tid);
			}
			script = GetStrConst(script, part);
			length = (int)strlen(part);
		} while (length != 0 && length < 0x40 && part[0] == '&');
		if (!found) {
			return;
		}
		m_effectHost->ResetPartEffect(0, tid);
		partTids.push_back(tid);
	}
	char bone[64];
	strcpy(bone, part);
	length = (int)strlen(bone);
	if (length == 0 || length >= 0x40) {
		return;
	}
	Attacher attacher;
	int count;
	for (count = 0; count < 4; count++) {
		if (count != 0) {
			script = GetStrConst(script, bone);
			length = (int)strlen(bone);
			if (length == 0 || length >= 0x40) {
				break;
			}
		}
		WBone* found = m_petFrame.get()->GetBonePet()->FindBone(bone);
		if (found != NULL) {
			attacher.m_pMatrix[count] = &found->GetMatrix();
		} else {
			w_bound_box* box = m_petFrame.get()->GetBonePet()->FindBoundBox(bone);
			if (box != NULL) {
				if (box->bone != NULL) {
					attacher.m_pMatrix[count] = &box->bone->GetMatrix();
				}
				attacher.m_pOffset[count] = &box->info->spherePivot;
			}
		}
	}
	if (count <= 0) {
		return;
	}
	if (strcmp("i_tak.seq", name) == 0) {
		return;
	}
	char* ext = strrchr(name, '.');
	if (ext == NULL || strlen(ext) < 4) {
		return;
	}
	if (_stricmp(ext + 1, "spr") == 0) {
		if (partTids.size() == 0) {
			JpDummyPlayerSprayEffect* spray = static_cast<JpDummyPlayerSprayEffect*>(m_effectHost->FindSpray(name, token, &attacher, 0, 0, 0));
			if (spray != NULL) {
				spray->m_kind7 = kind == 7;
			} else {
				m_effectHost->AddEffect(name, token, &attacher, 2, 1, 0, 0, 0);
			}
		} else {
			for (std::vector<int>::iterator it = partTids.begin(); it != partTids.end(); ++it) {
				JpDummyPlayerSprayEffect* spray = static_cast<JpDummyPlayerSprayEffect*>(m_effectHost->FindSpray(name, token, &attacher, *it, 0, 0));
				if (spray != NULL) {
					spray->m_kind7 = kind == 7;
				} else {
					m_effectHost->AddEffect(name, token, &attacher, 2, 1, *it, 0, 0);
				}
			}
		}
	} else if (partTids.size() == 0) {
		JpDummyPlayerSequenceEffect* sequence = static_cast<JpDummyPlayerSequenceEffect*>(m_effectHost->FindSequence(name, token, &attacher, 0, 0, 0));
		if (sequence != NULL) {
			bool flag = kind == 7;
			if (sequence->m_kind7 != flag) {
				sequence->m_kind7 = flag;
			}
		} else {
			m_effectHost->AddEffect(name, token, &attacher, 1, 1, 0, 0, 0);
		}
	} else {
		for (std::vector<int>::iterator it = partTids.begin(); it != partTids.end(); ++it) {
			JpDummyPlayerSequenceEffect* sequence = static_cast<JpDummyPlayerSequenceEffect*>(m_effectHost->FindSequence(name, token, &attacher, *it, 0, 0));
			if (sequence != NULL) {
				bool flag = kind == 7;
				if (sequence->m_kind7 != flag) {
					sequence->m_kind7 = flag;
				}
			} else {
				m_effectHost->AddEffect(name, token, &attacher, 1, 1, *it, 0, 0);
			}
		}
	}
}

// 0x4d8bf0
void IDummyPlayer::RunScriptCommand(const char* command) {
	// 0x4d27d0: the std::find_if over m_faceAnims by name (its predicate
	// is not traced; strcmp on m_name assumed).
	std::list<JpDummyPlayerFaceAnim*>::iterator it = m_faceAnims.begin();
	while (it != m_faceAnims.end() && strcmp((*it)->m_name, command) != 0) {
		++it;
	}
	if (it == m_faceAnims.end()) {
		JpDummyPlayerFaceAnim* face = new JpDummyPlayerFaceAnim;
		memset(face, 0, sizeof(JpDummyPlayerFaceAnim));
		strcpy(face->m_name, command);
		m_faceAnims.push_back(face);
		m_pendingFace = face;
	} else if (*it != m_pendingFace) {
		m_pendingFace = *it;
	}
}

// 0x4d8f20
void IDummyPlayer::RunMotionScript(const char*& script) {
	std::string command;
	if (NextMotionCommand(m_motionState, command, script, m_kind, m_altPartList)) {
		RunScriptCommand(command.c_str());
	}
}

// JP 0xe11820: ParseScript's keywords, in its switch's case order.
static const char* const s_scriptKeywords[16] = {
	"*snd(", "*shot", "*tail", "*showclub", "*hideclub", "*swing", "*lockclub", "*fx(",
	"*ptex(", "*showbone(", "*hidebone(", "*playif(", "*opa(", "*showvis(", "*hidevis(", "*fxstop(",
};
// JP 0xe76c40: the keyword ParseScript is matching.
static char s_scriptKeyword[0x400];

// 0x5ac510
void IPetScriptObject::ParseScript(const char* script, bool flag) {
	if (script == NULL) {
		return;
	}
	bool featherDone = false;
	char buffer[0x400];
	sprintf(buffer, "%s", script);
	const char* cursor = buffer;
	while (cursor != NULL) {
		cursor = strchr(cursor, '*');
		if (cursor == NULL || *cursor == '\0') {
			return;
		}
		int length = 0;
		while (true) {
			char c = cursor[++length];
			if (c == ' ') {
				break;
			}
			if (c == '(') {
				length++;
				break;
			}
			if (c == '\0') {
				break;
			}
		}
		if (length <= 1) {
			return;
		}
		strncpy(s_scriptKeyword, cursor, length);
		cursor += length;
		s_scriptKeyword[length] = '\0';
		for (int i = 0; i < 16; i++) {
			if (_stricmp(s_scriptKeyword, s_scriptKeywords[i]) != 0) {
				continue;
			}
			switch (i) {
			case 0:
				PlaySound(cursor);
				break;
			case 1:
				ScriptEvent2(cursor);
				break;
			case 2:
				ScriptEvent3(cursor);
				break;
			case 3:
				ScriptEvent4(cursor);
				break;
			case 4:
				HideClub(cursor);
				break;
			case 5:
				ScriptEvent6(cursor);
				break;
			case 6:
				ScriptEvent7(cursor);
				break;
			case 7:
			case 15:
				// The original sprintfs the buffer into itself (cursor
				// points into it), as here.
				if (i == 7 && !featherDone) {
					featherDone = true;
					if (strstr(cursor, "TF_Feather.seq") != NULL) {
						if (strstr(cursor, "head_wood") != NULL) {
							sprintf(buffer, "%s *fx(\"plume_twinkle.seq\" \"&268435587\" \"&201327102\" \"&201327103\" \"&201327104\" \"&201327105\" \"head_wood\") ", cursor);
							sprintf(buffer, "%s *fx(\"plume_twinkle.seq\" \"&268435588\" \"&201327106\" \"&201327107\" \"&201327108\" \"&201327109\" \"head_wood\") ", cursor);
							sprintf(buffer, "%s *fx(\"plume_twinkle.seq\" \"&268435589\" \"&201327110\" \"&201327111\" \"&201327112\" \"&201327113\" \"head_wood\") ", cursor);
						} else {
							sprintf(buffer, "%s *fx(\"plume_twinkle.seq\" \"&268435587\" \"&201327102\" \"&201327103\" \"&201327104\" \"&201327105\" \"head_iron\") ", cursor);
							sprintf(buffer, "%s *fx(\"plume_twinkle.seq\" \"&268435588\" \"&201327106\" \"&201327107\" \"&201327108\" \"&201327109\" \"head_iron\") ", cursor);
							sprintf(buffer, "%s *fx(\"plume_twinkle.seq\" \"&268435589\" \"&201327110\" \"&201327111\" \"&201327112\" \"&201327113\" \"head_iron\") ", cursor);
						}
						cursor = buffer;
					}
				}
				AttachScriptEffect(cursor, flag, i);
				break;
			case 8:
				RunMotionScript(cursor);
				break;
			case 9:
			case 10:
				ApplyFrameScript(cursor, i);
				break;
			case 11:
				PlayScriptMotion(cursor);
				break;
			case 12:
				ScriptEvent12(cursor);
				break;
			case 13:
			case 14:
				ApplyFrameKind(cursor, i);
				break;
			}
		}
	}
}

// 0x5ac900
void IPetScriptObject::FrameScript(CPetFrame* frame, int kind, const char*& script) {
	char name[64] = "";
	for (int i = 0; i < 0x40; i++) {
		script = GetStrConst(script, name);
		size_t length = strlen(name);
		if (length == 0 || length >= 0x40) {
			break;
		}
		WBone* bone = frame->GetBonePet()->FindBone(name);
		if (bone != NULL) {
			bone->SetInvisible(false, kind != 9);
		}
		if (script == NULL || *script == ')') {
			break;
		}
	}
}

// JP 0xe77040 / 0xe77440: FindScriptMotion's pet list and motion name
// (0xe77440's size is not traced).
static char s_scriptPets[0x400];
static char s_scriptMotion[0x400];

// 0x5aca50
const char* IPetScriptObject::FindScriptMotion(CPetFrame* frame, const char*& script) {
	s_scriptMotion[0] = '\0';
	s_scriptPets[0] = '\0';
	script = GetStrConst(script, s_scriptPets);
	script = GetStrConst(script, s_scriptMotion);
	if (s_scriptPets[0] == '\0' || s_scriptMotion[0] == '\0') {
		return NULL;
	}
	size_t offset = 0;
	while (true) {
		const char* pet = s_scriptPets + offset;
		const char* separator = strchr(pet, ';');
		size_t length;
		if (separator == NULL) {
			length = strlen(pet);
			if (length == 0) {
				return NULL;
			}
		} else {
			length = separator - pet;
		}
		char petName[0x28];
		strncpy(petName, pet, length);
		petName[length] = '\0';
		strcat(petName, ".mpet");
		if (Jp_PetFrame_HasPet(frame, petName)) {
			return s_scriptMotion;
		}
		if (separator == NULL) {
			return NULL;
		}
		offset += length + 1;
	}
}

// 0x5acbf0
void IPetScriptObject::FrameKind(CPetFrame* frame, bool kindIs13, const char*& script) {
	if (frame == NULL) {
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
				WBone* bone = frame->GetBonePet()->FindBone(it->c_str());
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

// 0x5ace20
bool IPetScriptObject::NextMotionCommand(const w_motion_data* motion, std::string& name, const char*& script, int kind, bool extra) {
	char token[0x40];
	char command[0x400];
	token[0] = '\0';
	command[0] = '\0';
	script = GetStrConst(script, token);
	size_t length = strlen(token);
	if (length == 0 || length >= 0x40) {
		return false;
	}
	if (token[0] == '@') {
		if (motion == NULL) {
			return false;
		}
		if (_stricmp(token + 1, motion->name) != 0) {
			return false;
		}
		script = GetStrConst(script, command);
		length = strlen(command);
		if (length == 0 || length >= 0x400) {
			return false;
		}
	} else {
		strcpy(command, script);
	}
	script = GetStrConst(script, command);
	length = strlen(command);
	if (length == 0 || length >= 0x400) {
		return false;
	}
	if (kind == 1) {
		name = command;
		return true;
	}
	JpTanLines* tanLines = JpTanLines::Instance();
	std::string texture;
	if (kind == 2) {
		if (tanLines->HasTanLine()) {
			tanLines->FindTexture(token, command, texture, NULL);
			name.assign(texture, 0, std::string::npos);
		} else {
			name = command;
		}
		return true;
	}
	if (kind == 0) {
		if (extra == true) {
			tanLines->FindTexture(token, command, texture, NULL);
			name.assign(texture, 0, std::string::npos);
			return true;
		}
		name = command;
		return true;
	}
	CPetFrame* frame = m_scriptFrame;
	uint32_t tids[24];
	for (int i = 0; i < 24; i++) {
		if (i < frame->GetPartList().GetNumTotalParts()) {
			tids[i] = static_cast<uint32_t>(frame->GetPartList().GetPart(static_cast<unsigned char>(i))->m_tid);
		} else {
			tids[i] = 0;
		}
	}
	tanLines->SetTids(tids);
	tanLines->FindTexture(token, command, texture, NULL);
	name.assign(texture, 0, std::string::npos);
	return true;
}
