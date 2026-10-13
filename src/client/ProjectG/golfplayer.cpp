// Original: d:\build\custom\temp\code\client\projectg\golfplayer.cpp
#define GOLFPLAYER_GOLFDOC_INLINE
#define GOLFPLAYER_PARTSINFO_INLINE
#define GOLFPLAYER_UCCMANAGER_INLINE
#define GOLFPLAYER_CLUB_INLINE
#include "golfplayer.h"
#include "partsinfo.h"
#include "ObjectFactory.h"
#include "NetworkSystem.h"
#include "GolfDoc.h"
#include "club.h"
#include "UccManager.h"
#include "TailEffect.h"
#include "scenemanager.h"
#include "texturepart.h"
#include "golfball.h"
#include "SharedDoc.h"
#include "headicon.h"
#include "InputManager.h"
#include "commonutil.h"
#include "quadtree.h"
#include "polysoup.h"
#include "wpuppet.h"
#include "wbone.h"
#include "wview.h"
#include "wutil.h"
#include "fx.h"
#include "parttidlist.h"
#include "SoundManager.h"
#include "Packet.h"
#include "slicestr.h"
#include "QuitWindow.h"
#include "ItemWindow.h"
#include "shared/Localize.h"
#include "HelpMsg.h"
#include "client/Fresh/include/frbutton.h"
#include "ProjectG.h"

#include <algorithm>

const char *GetStrConst(const char *pSrc, char *pDest);
const char *GetCharsTillSpace(const char *pSrc, char *pDest);

// ?g_lightset@@3ULightSet@@A; its header (wscene.h) does not declare it yet.
extern LightSet g_lightset;
// The axis swap between the puppet's and the scene's frames, and its inverse
// (?convMat@@3VWMatrix@@A, ?convMatInv@@3VWMatrix@@A); defined after the
// run-time type registration below.
extern WMatrix convMat;
extern WMatrix convMatInv;
// No symbol: the swing tail's texture for three club sets, the motion names
// (CheckShotScript times entries 0 to 15) and the puppet script commands.
struct sSwingTable {
	struct sTail {
		unsigned long m_clubSetId;  // 0x0, the club set's type id
		const char *m_texName;      // 0x4
	} m_tails[3];                   // 0x00
	const char *m_motionNames[41];  // 0x18
	const char *m_commands[15];     // 0xBC, the script commands ParsePuppetScript matches ("*snd(" ...)
};
extern "C" const sSwingTable data_a804a8;
// No symbol: ParsePuppetScript's buffers: the command, the "*snd(" sound
// name, and the "*playif(" pet conditions and motion.
extern "C" {
	static char data_af30a8[0x40];
	static char data_af3068[0x40];
	static char data_af2c68[0x400];
	static char data_af2b68[0x100];
}
// The camera matrix (camerawork.cpp).
extern WMatrix g_camera;
// Nameless .rdata floats: pi, degrees to radians, FLT_MAX, epsilon (1e-5),
// read through memory rather than literals.
extern "C" const float data_9ef200[4];

// golfplayer.h
CGolfPlayer::Object::Object(void) {
}

// golfplayer.h
CGolfPlayer::sClub::sClub(void) {
}

// golfplayer.h:139
CGolfPlayer::sPetInfo::sPetInfo(void)
	: m_motionTime(0), m_pMotion(0), m_unk38(false), m_unk5C(0), m_unk60(0),
	  m_pTexPart(0), m_pAppliedTexPart(0), m_scale(1.0f), m_unkB0(0) {
	for (int i = 2; i >= 0; --i) { m_worldMatrix.p[i * 4] = 1.0f; m_worldMatrix.p[i * 4 + 1] = m_worldMatrix.p[i * 4 + 2] = m_worldMatrix.p[i * 4 + 3] = 0.0f; }
	m_nextMotion[0] = '\0';
	memset(m_shotTimes, 0, sizeof(m_shotTimes));
}

// golfplayer.h:146
CGolfPlayer::sPetInfo::~sPetInfo(void) {
}

// golfplayer.h:233
int CGolfPlayer::GetAbilityPartsSize(void) {
	return m_abilityParts.size();
}

// golfplayer.cpp:37 (run-time type and factory registration of CGolfPlayer)
const WRTTI CGolfPlayer::m_RTTI("CGolfPlayer", &IActor::m_RTTI);

IObject* CGolfPlayerMakeInstance(void) {
	return new CGolfPlayer;
}

struct __impCGolfPlayer {
	__impCGolfPlayer(void) {
		ObjectFactory().AddObjectFunctor(CGolfPlayerMakeInstance, "CGolfPlayer");
		ActorFactory().AddActorClass("CGolfPlayer");
	}
};
static __impCGolfPlayer s_impCGolfPlayer;

WMatrix convMat(0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
WMatrix convMatInv(0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

// golfplayer.cpp:96
CGolfPlayer::CGolfPlayer(void)
	: m_pClubPuppet(NULL), m_pPetFrames(NULL), m_pPetInfos(NULL), m_pTailEffect(NULL), m_unk15D(true),
	  m_unk2F0(false) {
	m_comboRunning = false; m_comboTime = 0;

	m_unk320 = 0;
	m_powerShotKeyTime = 0;
	m_powerShotOn = false;
	m_unk329 = false;
	m_powerShotKeyState = 0;

	m_pHeadBone = NULL;
	m_pSafeTeeSeq = NULL;


	m_unk330 = false;
}

// golfplayer.cpp:113
void CGolfPlayer::OnDestroy(void) {
	GolfClub().ClearClubSet();

	std::list<sTexturePart *>::iterator it;
	for (it = m_pendingTexParts.begin(); it != m_pendingTexParts.end(); ++it) {
		if (*it) { delete *it; *it = NULL; }
	}
	m_pendingTexParts.clear();

	if (m_pTailEffect) { delete m_pTailEffect; m_pTailEffect = NULL; }

	if (m_pPetFrames) { delete[] m_pPetFrames; m_pPetFrames = NULL; }
	if (m_pPetInfos) { delete[] m_pPetInfos; m_pPetInfos = NULL; }
}

// golfplayer.cpp:130
void CGolfPlayer::RegisterClub(int index) {
	const char *motionNames[4] = {
		"\xBF\xEC\xB5\xE5\xBC\xA6\xC1\xD8\xBA\xF1",
		"\xBE\xC6\xC0\xCC\xBE\xF0\xBC\xA6\xC1\xD8\xBA\xF1",
		"\xBB\xF7\xB5\xE5\x9F\x87\xC1\xF6\xBC\xA6\xC1\xD8\xBA\xF1",
		"\xC6\xDB\xC6\xC3\xBC\xA6\xC1\xD8\xBA\xF1",
	};
	GOLFDOC()->GetPartTidList(GOLFDOC()->m_myPlayerIndex);

	WMatrix mat;
	mat.Reset();
	GolfClub().ClearClubSet();

	for (int player = 0; player < GOLFDOC()->GetPlayerNum(); player++) {
		if (index != -1) {
			if (player != index) {
				continue;
			}
		}
		if (GOLFDOC()->GetPlayer(player)->m_state == 3) {
			continue;
		}

		std::vector<WPuppet *> &pets = m_pPetFrames[player].GetAniPetList();

		sClubSet *pClubSet = GolfClub().AddClubSet(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[player].m_clubSet.typeId);

		CPartTidList &tidList = GOLFDOC()->GetPartTidList(player);
		IFF_STRUCT::sChar *pChar = ItemManager()->FindChar(tidList.m_tidChar);

		std::vector<WPuppet *>::iterator it;
		for (it = pets.begin(); it != pets.end(); ++it) {
			WPuppet *pPuppet = *it;

			for (int type = 0; type < 4; type++) {

				m_clubs[player][type].m_ballPos.Reset();
				m_clubs[player][type].m_gripPos = 0;
				w_motion_data *pMotion = pPuppet->FindMotionData(motionNames[type]);
				if (pMotion == NULL) {
					continue;
				}

				int frame = (int)(pMotion->s * 30.0f + 0.5f);

				int frameIndex = -1, check;
				pPuppet->CheckFrameData(frame, frame, frameIndex, check);
				if (frameIndex <= -1) {
					continue;
				}

				char *frameData = pPuppet->GetFrameData(frameIndex);
				WSliceStr slice(frameData);
				slice.Get("*ball_dist", "%f", &m_clubs[player][type].m_ballPos.x);
				slice.Get("*grip_pos", "%f", &m_clubs[player][type].m_gripPos);
				m_clubs[player][type].m_ballPos.y = 0;
				m_clubs[player][type].m_ballPos.z = 0.4f;
				if (pClubSet && pChar) {
					w_bound_box *pBall = pClubSet->m_puppet[0][type]->m_bbList.Find("Ball");
					w_bound_box *pTarget = pClubSet->m_puppet[0][type]->m_bbList.Find("Ball_Target");

					if (pBall && pTarget) {
						pClubSet->m_puppet[0][type]->ApplyBones(NULL, mat);
						pClubSet->m_puppet[0][type]->Transform(NULL, 1.0f, 0);
						WVector diff;
						diff = (pBall->info->spherePivot * pBall->bone->GetMatrix() - pTarget->info->spherePivot * pTarget->bone->GetMatrix()) * pChar->m_cameraScale;

						m_clubs[player][type].m_ballPos.x += diff.x;
						m_clubs[player][type].m_ballPos.z += diff.z;
					}
				}
			}
		}
	}
}

// golfplayer.cpp:227
void CGolfPlayer::OnPreLoadInit(void) {
	m_lightSet.type = LightSet::W_LIGHT_DIRECTIONAL;

	CSceneManager::Instance()->RegisterElement((eObjType)4, 0, 0, "\xc5\xac\xb7\xb4", 0x28, 0);

	CSceneManager::Instance()->RegisterElement((eObjType)6, 0, m_pTailEffect, "tail", 0, 0);
	CSceneManager::Instance()->SetVisible("tail", false);

	m_pPetInfos = new sPetInfo[4];

	for (int i = 0; i < 4; i++) {
		CSceneManager::Instance()->RegisterElement((eObjType)4, 0, 0, MakeStr("PLAYER_%d", i), 0x18, 0);
		CSceneManager::Instance()->SetVisible(MakeStr("PLAYER_%d", i), false);
	}

	m_pTailEffect = new CTailEffect("~swinglight01.jpg");
}

// golfplayer.cpp:269
void CGolfPlayer::OnLoad(void) {
	if (WSingleton<CHeadIcon>::Instance()) {
		WSingleton<CHeadIcon>::Instance()->SetLevel();
	}

	if (m_pPetFrames) { delete[] m_pPetFrames; m_pPetFrames = NULL; }
	m_pPetFrames = new CPetFrame[GOLFDOC()->GetPlayerNum()];

	for (int i = 0; i < 4; i++) {
		if (i < GOLFDOC()->GetPlayerNum()) {
			if (GOLFDOC()->GetPlayer(i)->m_state != 3) {
				CPartTidList *tidList = &GOLFDOC()->GetPartTidList(i);

				ShowAbilityParts(tidList);

				if (!m_pPetFrames[i].Build(tidList, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_characterInfo.auxParts.tids, true, true, i)) {
					tidList->SetDefaultTids();
					m_pPetFrames[i].Build(tidList, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_characterInfo.auxParts.tids, true, false, i);
				}

				m_pPetFrames[i].CalcBVolume();
				SetMotion("\xB5\xEE\xC0\xE5\xB8\xF0\xBC\xC7", 0.0f, i);

				m_pPetInfos[i].m_unk60 = 0;
				m_pPetInfos[i].m_nextMotion[0] = '\0';

				CSceneManager::Instance()->SetPuppet(MakeStr("PLAYER_%d", i), m_pPetFrames[i].GetBonePet());

				m_extendedEffect.RegistExtenedEffect(&m_pPetFrames[i]);
			}
		} else {
			m_pPetInfos[i].m_unk38 = false;
			CSceneManager::Instance()->SetPuppet(MakeStr("PLAYER_%d", i), 0);
		}
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
		for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			if (i == MyIndex()
				|| (GOLFDOC()->GetPlayer(MyIndex())->m_partnerIndex != 0xFF && i == GOLFDOC()->GetPlayer(MyIndex())->m_partnerIndex)
				|| !WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
				MakeStr("PLAYER_%d", i);
			}
		}
	}

	RegisterClub(-1);
	CheckShotScript();
}

// golfplayer.cpp:340
void CGolfPlayer::OnReload(void) {
	OnLoad();
}

// golfplayer.cpp:346
bool CGolfPlayer::ShowAbilityParts(CPartTidList *pTidList) {
	if (GetAbilityPartsSize() == 0) {
		AddAbilityParts(0x80A2800, 0x80A4000);
		AddAbilityParts(0x80A2801, 0x80A4001);
		AddAbilityParts(0x80A2802, 0x80A4002);

		AddAbilityParts(0x8122800, 0x8124000);
		AddAbilityParts(0x8122801, 0x8124001);
		AddAbilityParts(0x8122802, 0x8124002);

		AddAbilityParts(0x8026800, 0x8024800);
		AddAbilityParts(0x8026801, 0x8024801);
		AddAbilityParts(0x8026802, 0x8024802);

		AddAbilityParts(0x8064800, 0x8066800);
		AddAbilityParts(0x8064801, 0x8066801);
		AddAbilityParts(0x8064802, 0x8066802);
		AddAbilityParts(0x8064803, 0x8066803);

		AddAbilityParts(0x81EA800, 0x81EC800);

		AddAbilityParts(0x80E4800, 0x80E6800);
		AddAbilityParts(0x80E4801, 0x80E6801);
		AddAbilityParts(0x80E4802, 0x80E6802);

		AddAbilityParts(0x81A4800, 0x81A6800);
		AddAbilityParts(0x81A4800, 0x81A8800);
		AddAbilityParts(0x81A4801, 0x81A6801);
		AddAbilityParts(0x81A4801, 0x81A8801);

		AddAbilityParts(0x816E801, 0x8152801);
		AddAbilityParts(0x816E802, 0x8152802);
		AddAbilityParts(0x816E803, 0x8152803);

		AddAbilityParts(0x8228800, 0x8226800);
		AddAbilityParts(0x8228801, 0x8226801);
		AddAbilityParts(0x8228802, 0x8226802);
		AddAbilityParts(0x8228803, 0x8226803);
	}

	bool shown = false;
	int count = GetAbilityPartsSize();

	for (int i = 0; i < count; i++) {
		std::pair<unsigned long, unsigned long> part = m_abilityParts[i];
		if (((part.first >> 18) & 0xFF) == (pTidList->m_tidChar & 0x3FFFFFF)) {
			if (pTidList->IsEquipped(part.first, 0) == true) {
				pTidList->SetTid(part.second, 0, 0, 0);
				shown = true;
			}
		}
	}

	return shown;
}

// golfplayer.cpp:413
void CGolfPlayer::CheckShotScript(void) {
	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (GOLFDOC()->GetPlayer(i)->m_state == 3) {
			continue;
		}
		for (int j = 0; j < 16; j++) {
			w_motion_data *pMotion = m_pPetFrames[i].FindMotionData(data_a804a8.m_motionNames[j], true, NULL);
			if (pMotion == NULL) {
				continue;
			}
			int start = (int)ceil(pMotion->s * 30.0f);
			float endFrame = pMotion->e * 30.0f;
			int end = WAbsBySign(floor(endFrame) - endFrame) < data_9ef200[3] ? -1 - (int)(pMotion->e * -30.0f) : (int)floor(pMotion->e * 30.0f);
			for (int frame = start; frame < end; frame++) {
				int index;
				if (m_pPetFrames[i].GetCurAniPet()->CheckFrameData(frame, frame, index, index)) {
					char *data = m_pPetFrames[i].GetCurAniPet()->GetFrameData(index);
					if (strstr(data, "*shot")) {
						m_pPetInfos[i].m_shotTimes[j] = (frame - start) * (1.0f / 30.0f);
						break;
					}
				}
			}
		}
	}
}

// golfplayer.cpp:467
void CGolfPlayer::OnInit(void) {
	m_pFootBones[0] = NULL;
	m_pFootBones[1] = NULL;
	m_shotMotionName[0] = '\0';
	m_swingTypeName[0] = '\0';
	m_unk15C = false;

	m_unk140 = 3.14159265f;
	m_displayMatrix.Reset();
	m_lightSet.ambient = ModifyColor(g_lightset.ambient, 1.0f);
	m_lightSet.nearone = g_lightset.nearone;
}

// golfplayer.cpp:484
void CGolfPlayer::InputProcess(float dt) {
	if (g_input->Get(0x45)) {
		if (m_headAngle < 1.0471976f) {
			m_headAngle += dt * 12.566371f;
			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C |= 0x200;
		}
	}

	if (g_input->Get(0x51)) {
		if (m_headAngle > -1.0471976f) {
			m_headAngle -= dt * 12.566371f;
			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C |= 0x200;
		}
	}
}

// golfplayer.cpp:508
void CGolfPlayer::OnProcess(float dt) {
	switch (m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk60) {
	case 3:
		if (m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_pMotion == NULL) {
			break;
		}

		{
			float ratio = (dt + m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_motionTime - m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_pMotion->s) /
				(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_pMotion->e - m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_pMotion->s);

			if (ratio > 1.0f) {
				ratio = 1.0f;
				m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk60 = 4;
				SendMsg(this, "Spot", 0xF5, 0, 0, 0, 0);

				if (OnlinePlay()) {
					int powerShot = GolfClub().PowerShotType();
					WSendPacket packet(GCT_POWER_SHOT);
					packet.Encode1(powerShot);
					packet.Send(SEND_TO_DEFAULT);
				}
			}

			SendMsg(this, "Screen", 0x1AA, (intptr_t)&ratio, 0, 0, 0);
		}
		break;
	case 0:
	case 4:
		if (GOLFDOC()->GameMode() & 0xA4000) {
			m_unk18F = false;
		} else {
			if (m_unk18F && !m_unk190) {
				if (m_swingTypeName[0] != '\0') {
					char *motion;

					switch (GolfClub().PowerShotType()) {
					case 0:
						motion = MakeStr("%s\xC1\xD8\xBA\xF1", m_swingTypeName);
						break;
					case 1:
						motion = MakeStr("%s\xC6\xC4\xBF\xF6\xC1\xD8\xBA\xF1\xB3\xA1", m_swingTypeName);
						break;
					case 2:
					case 3:
						motion = MakeStr("%s\xC6\xC4\xBF\xF6" "2\xC1\xD8\xBA\xF1\xB3\xA1", m_swingTypeName);
						break;
					}

					SetMotion(motion, 0.0f, -1);
				}
			}

			m_unk18F = m_unk190;
		}
		m_unk190 = false;
		break;
	}

	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (!m_pPetInfos[i].m_unk38 || GOLFDOC()->GetPlayer(i)->m_state == 3) {
			continue;
		}

		m_pPetInfos[i].m_motionTime += dt;

		if (Abs(m_pPetInfos[i].m_unkB0) > 0.0f) {
			m_pPetInfos[i].m_scale = Between(0.0f, m_pPetInfos[i].m_scale + dt * m_pPetInfos[i].m_unkB0, 1.0f);
		}

		ScriptProcess(m_pPetInfos[i].m_pMotion, m_pPetInfos[i].m_motionTime - dt, m_pPetInfos[i].m_motionTime, i, false, false);

		if (m_pPetInfos[i].m_pMotion != NULL && m_pPetInfos[i].m_motionTime > m_pPetInfos[i].m_pMotion->e) {
			if (m_pPetInfos[i].m_pMotion->s != m_pPetInfos[i].m_pMotion->e) {
				if (m_pPetInfos[i].m_unk60 == 2) {
					m_unk15C = false;
					CSceneManager::Instance()->SetVisible("tail", false);
				}

				if (m_pPetInfos[i].m_nextMotion[0] != '\0') {
					if (m_unk304) {
						m_unk304 = false;
						this << MsgObject(0, 0x10E, 0, 0, 0, 0, 0);
						continue;
					}

					SetMotion(m_pPetInfos[i].m_nextMotion, m_pPetInfos[i].m_motionTime - m_pPetInfos[i].m_pMotion->e, i);
					m_pPetInfos[i].m_nextMotion[0] = '\0';
					continue;
				}

				char name[0x40];
				sprintf(name, "%s\xC1\xD8\xBA\xF1", m_swingTypeName);

				// "등장모션" (entrance motion)
				if (strcmp(m_pPetInfos[i].m_pMotion->name, "\xB5\xEE\xC0\xE5\xB8\xF0\xBC\xC7") == 0 && m_swingTypeName[0] != '\0') {
					if (strcmp(name, m_pPetInfos[i].m_pMotion->nextmotion) != 0) {
						SetMotion(name, m_pPetInfos[i].m_motionTime - m_pPetInfos[i].m_pMotion->e, i);
					} else {
						SetMotion(m_pPetInfos[i].m_pMotion->nextmotion, m_pPetInfos[i].m_motionTime - m_pPetInfos[i].m_pMotion->e, i);
					}
				} else if (i == GOLFDOC()->m_myPlayerIndex && strcmp(m_pPetInfos[i].m_pMotion->name, name) == 0 &&
					(m_unk190 || GolfClub().PowerShotType() || GOLFDOC()->GameMode() != 8 || m_unk2F4 < 9.6f)) {
					SetMotion(m_pPetInfos[i].m_pMotion->name, m_pPetInfos[i].m_motionTime - m_pPetInfos[i].m_pMotion->e, i);
				} else {
					SetMotion(m_pPetInfos[i].m_pMotion->nextmotion, m_pPetInfos[i].m_motionTime - m_pPetInfos[i].m_pMotion->e, i);
				}
			} else {
				m_pPetInfos[i].m_motionTime = m_pPetInfos[i].m_pMotion->s;
				m_pPetInfos[i].m_unk5C &= ~2;
			}
		}
	}

	if (GolfClub().TypeUpdated()) {
		ParseBoundingBoxScript(GOLFDOC()->m_myPlayerIndex, true, false, true);
		SetSwingType();
		SearchGround();
		m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C |= 0x21;
	}

	if (m_unk15C) {
		m_pTailEffect->Process(dt);

		if (m_pTailSpots[0] && m_pTailSpots[1]) {
			WVector from = m_pTailSpots[0]->info->spherePivot * m_pTailSpots[0]->bone->GetMatrix();
			WVector to = m_pTailSpots[1]->info->spherePivot * m_pTailSpots[1]->bone->GetMatrix();

			m_pTailEffect->PutSpot(from, to);
		}
	}

	if (GolfClub().GetType() == 3) {
		if (GolfBall().m_groundType == 2 && GOLFDOC()->m_gameState >= 4 && GolfBall().m_preDataIndex < 0) {
			if (g_input->GetDown("LALT", true) || g_input->GetDown("RALT", true)) {
				m_unk2A0 = !m_unk2A0;
			}
		}
	} else {
		PowerShotInput();
	}

	if (IsLocalContent((localContentType_t)0x57)) {
		RefreshAllUccClothes();
	}
}

// golfplayer.cpp:717
void CGolfPlayer::OnDisplay(void) {
	if ((GOLFDOC()->GameMode() & 0x20000) ||
		((WSingleton<CSharedDoc>::Instance()->m_roomType == 1 || WSingleton<CSharedDoc>::Instance()->m_roomType == 3 ||
		  WSingleton<CSharedDoc>::Instance()->m_roomType == 7) && (GOLFDOC()->GameMode() & 0x3000))) {
		ScoreEvent();
		return;
	}

	if (!m_unk15D) {
		return;
	}

	if (m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C & 1) {
		if (GOLFDOC()->GameMode() & 0x3000) {
			WMatrix &mat = m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_worldMatrix;

			mat.za = m_displayMatrix.za;
			mat.ya = WVector(0.0f, 1.0f, 0.0f);
			mat.xa = WCrossProduct(mat.ya, mat.za);
			mat.pivot = GetPVS().GetGroundPoint(m_position * m_displayMatrix, false, NULL, false);
		} else if (GOLFDOC()->GameMode() & 0x4000) {
			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_worldMatrix.za = m_displayMatrix.za;
			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_worldMatrix.ya = WVector::UNIT_POS_Y;
			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_worldMatrix.xa = WCrossProduct(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_worldMatrix.ya, m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_worldMatrix.za);
			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_worldMatrix.pivot = GetPVS().GetGroundPoint(m_position + m_clubs[GOLFDOC()->m_myPlayerIndex][GolfClub().GetType()].m_ballPos * m_displayMatrix, false, NULL, false);
		} else {
			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_worldMatrix = m_displayMatrix;
			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_worldMatrix.pivot = m_position + m_clubs[GOLFDOC()->m_myPlayerIndex][GolfClub().GetType()].m_ballPos * m_displayMatrix;
		}

		const char *texName = GetPVS().GetTextureName(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_worldMatrix.pivot);

		if (texName && (stricmp(texName, "igrass05.dds") == 0 || stricmp(texName, "igrass04_01.dds") == 0)) {
			// __real@3ea3d70b, one ulp above 0.32f
			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_worldMatrix.pivot.y -= 0.32000002f;
		}
	}

	if (m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C & 3) {
		m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetBonePet()->ApplyBones(m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetCurAniPet()->GetBones(NULL, m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_motionTime, 0, NULL, false), m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_worldMatrix);
		m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetBonePet()->UpdateBound(true, true);
	}

	if (GOLFDOC()->GameMode() & 0x1002) {
		WVector pos = GetHeadMatrix().pivot;

		pos.y = 0.0f;
		pos = GetPVS().GetGroundPoint(pos, false, NULL, false);
		m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_worldMatrix.pivot.y = pos.y;
	}

	if (!(GOLFDOC()->GameMode() & 0x4000) && (m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C & 0xB)) {
		for (int i = 0; i < 2; i++) {
			if (m_pFootBones[i]) {
				m_pFootBones[i]->SetMatrix(convMatInv * GetFootMatrix(i), NULL);
			}
		}
	}

	if (m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C & 0x203) {
		if (m_pHeadBone) {
			m_pHeadBone->SetMatrix(convMatInv * GetHeadMatrix(), NULL);
		}
	}

	if (WSingleton<CHeadIcon>::Instance()) {
		WBone *pHead = m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetBonePet()->FindBone("Bip01 Head");
		WVector pos = pHead->GetMatrix().pivot;
		WSingleton<CHeadIcon>::Instance()->SetPlayerPos(GOLFDOC()->m_myPlayerIndex, pos);
	}

	if (m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C & 3) {
		m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C |= 0x30;
	}

	WPuppet *pPet = m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetBonePet();

	sClubSet *pClubSet = GolfClub().FindClubSet(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId);

	WPuppet *pClub = pClubSet->m_puppet[0][GolfClub().GetType()];

	if (g_view->InFrustum(pPet->GetBoundSphere())) {
		if ((m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C >> 4) & 1) {
			if ((m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C >> 6) & 1) {
				SetLight();
			}

			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C &= ~0x10;
			m_pPetFrames[GOLFDOC()->m_myPlayerIndex].UpdateLightSource(&m_lightSet, false, NULL);
		}

		if (m_standBlocked || m_unk2A0) {
			m_pPetFrames[GOLFDOC()->m_myPlayerIndex].SetAlpha((unsigned char)(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_scale * 64.0f));
			pClub->SetAlpha((unsigned char)(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_scale * 64.0f));
		} else {
			m_pPetFrames[GOLFDOC()->m_myPlayerIndex].SetAlpha((unsigned char)(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_scale * 255.0f));
			pClub->SetAlpha((unsigned char)(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_scale * 255.0f));
		}

		if (!(WAbsBySign(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_scale) < data_9ef200[3])) {
			pClub->GetAlpha(NULL);
		}

		sPetInfo *pInfo = &m_pPetInfos[GOLFDOC()->m_myPlayerIndex];

		if (pInfo->m_pTexPart && pInfo->m_pTexPart != pInfo->m_pAppliedTexPart) {
			sTexturePart *pPart = pInfo->m_pTexPart;
			pInfo->m_pAppliedTexPart = pPart;

			if (pPart->m_pBitmap == NULL) {
				m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetBonePet()->ChangeTexturePart(m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetFaceTex(), pPart->m_texName, &pPart->m_pBitmap, &pPart->m_rect);
			} else {
				m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetBonePet()->ChangeTexturePart(m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetFaceTex(), pPart->m_pBitmap, pPart->m_rect);
			}

			m_pPetFrames[GOLFDOC()->m_myPlayerIndex].SetMakeup();
		}
	}

	if (!((m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C >> 7) & 1)) {
		CPartTidList &tidList = GOLFDOC()->GetPartTidList(GOLFDOC()->m_myPlayerIndex);
		IFF_STRUCT::sChar *pChar = ItemManager()->FindChar(tidList.m_tidChar);
		float scale;

		if (pChar) {
			scale = pChar->m_cameraScale;
		} else {
			scale = 1.0f;
		}

		WMatrix mat;

		if ((m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C >> 8) & 1) {
			mat.xa = m_fixedClubMatrix.xa * scale;
			mat.ya = m_fixedClubMatrix.ya * scale;
			mat.za = m_fixedClubMatrix.za * scale;
			mat.pivot = m_fixedClubMatrix.pivot;
		} else {
			w_bound_box *pBox = m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetBonePet()->FindBoundBox("club");

			if (pBox) {
				mat = pBox->bone->GetMatrix();
				mat.pivot = pBox->info->spherePivot * pBox->bone->GetMatrix();
				mat.pivot += mat.xa * m_clubs[GOLFDOC()->m_myPlayerIndex][GolfClub().GetType()].m_gripPos;
				mat.xa *= scale;
				mat.ya *= scale;
				mat.za *= scale;
			} else {
				mat.Reset();
			}
		}

		pClub->ApplyBones(NULL, mat);
		pClub->Transform(NULL, 1.0f, 0);
		pClub->UpdateBSphere();

		if (GolfClub().TypeUpdated()) {
			m_pClubPuppet = pClub;
		}

		if ((m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C >> 5) & 1) {
			if ((m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C >> 6) & 1) {
				SetLight();
			}

			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C &= ~0x20;
		}

		pClub->UpdateLightSource(&m_lightSet, false, NULL);
		// "club" in Korean
		CSceneManager::Instance()->SetPuppet("\xC5\xAC\xB7\xB4", pClub);
	}

	if (m_unk15C) {
		m_pTailEffect->Display();
	}

	m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C &= ~0x209;

	m_extendedEffect.Update(&m_pPetFrames[GOLFDOC()->m_myPlayerIndex]);
}

// golfplayer.cpp:936
void CGolfPlayer::ScoreEvent(void) {
	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (!m_pPetInfos[i].m_unk38 || GOLFDOC()->GetPlayer(i)->m_state == 3) {
			continue;
		}

		m_pPetFrames[i].GetBonePet()->ApplyBones(m_pPetFrames[i].GetCurAniPet()->GetBones(NULL, m_pPetInfos[i].m_motionTime, 0, NULL, false));
		m_pPetFrames[i].GetBonePet()->UpdateBound(true, false);

		if (g_view->InFrustum(m_pPetFrames[i].GetBonePet()->GetBoundSphere())) {
			m_pPetFrames[i].UpdateLightSource(&m_lightSet, false, NULL);
		}

		if (WSingleton<CHeadIcon>::Instance()) {
			WBone *pHead = m_pPetFrames[i].GetBonePet()->FindBone("Bip01 Head");
			WVector pos = pHead->GetMatrix().pivot;
			WSingleton<CHeadIcon>::Instance()->SetPlayerPos(i, pos);
		}

		sPetInfo *pInfo = &m_pPetInfos[i];

		if (pInfo->m_pTexPart && pInfo->m_pTexPart != pInfo->m_pAppliedTexPart) {
			sTexturePart *pPart = pInfo->m_pTexPart;
			pInfo->m_pAppliedTexPart = pPart;
			if (pPart->m_pBitmap == NULL) {
				m_pPetFrames[i].GetBonePet()->ChangeTexturePart(m_pPetFrames[i].GetFaceTex(), pPart->m_texName, &pPart->m_pBitmap, &pPart->m_rect);
			} else {
				m_pPetFrames[i].GetBonePet()->ChangeTexturePart(m_pPetFrames[i].GetFaceTex(), pPart->m_pBitmap, pPart->m_rect);
			}

			m_pPetFrames[i].SetMakeup();
		}

		if (m_pClubPuppet == NULL) {
			return;
		}

		if (!(GOLFDOC()->GameMode() & 0x3000)) {
			continue;
		}

		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
			if (i != TEAM(PLAYER(MyIndex())->m_attribute)->m_clubHolder) {
				continue;
			}

			if ((m_pPetInfos[i].m_unk5C >> 7) & 1) {
				continue;
			}
		}

		if ((m_pPetInfos[i].m_unk5C >> 8) & 1) {
			m_pClubPuppet->ApplyBones(NULL, m_fixedClubMatrix);
		} else {
			w_bound_box *pBox = m_pPetFrames[i].GetBonePet()->FindBoundBox("club");
			WMatrix mat;

			mat = pBox->bone->GetMatrix();
			mat.pivot = pBox->info->spherePivot * pBox->bone->GetMatrix();
			float grip = m_clubs[i][GolfClub().GetType()].m_gripPos;
			mat.pivot += mat.xa * grip;
			m_pClubPuppet->ApplyBones(NULL, mat);
		}

		m_pClubPuppet->Transform(NULL, 1.0f, 0);
		m_pClubPuppet->UpdateBSphere();
		m_pClubPuppet->UpdateLightSource(&m_lightSet, false, NULL);
	}
}

// golfplayer.cpp:1016
WMatrix CGolfPlayer::GetHeadMatrix(void) {
	WMatrix mat;
	WBone *pBone = m_pHeadBone;

	if (pBone == NULL) {
		mat.Reset();
		return mat;
	}

	if (WAbsBySign(m_headAngle) < data_9ef200[3]) {
		mat = convMat * pBone->GetMatrix();
	} else {
		mat = RotMat(m_headAngle, 1);
		mat = mat * convMat * pBone->GetMatrix();
	}

	return mat;
}

// golfplayer.cpp:1040
WMatrix CGolfPlayer::GetFootMatrix(int foot) {
	WMatrix mat;
	WBone *pBone = m_pFootBones[foot];

	if (pBone == NULL) {
		mat.Reset();
		return mat;
	}

	if (WAbsBySign(m_footAngle) < data_9ef200[3]) {
		mat = convMat * pBone->GetMatrix();
	} else {
		mat = RotMat(m_footAngle, 0);
		mat = mat * convMat * pBone->GetMatrix();
	}

	return mat;
}

// golfplayer.cpp:1064
void CGolfPlayer::ParseBoundingBoxScript(int player, bool clear, bool, bool) {
	char fxTag[] = "*fx(";
	sClubSet *pClubSet = GolfClub().FindClubSet(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[player].m_clubSet.typeId);
	Attacher attacher;

	if (clear) {
		for (unsigned int i = 0; i < m_bbSprays.size(); i++) {
			CFx::Instance()->CloseSpray(m_bbSprays[i]);
		}
		for (unsigned int j = 0; j < m_bbSequences.size(); j++) {
			CFx::Instance()->CloseSequence(m_bbSequences[j], true);
		}
		m_bbSprays.clear();
		m_bbSequences.clear();
	}

	WList<w_bound_box *> &bbList = m_pPetFrames[player].GetBonePet()->m_bbList;
	w_bound_box *pBox;

	for (pBox = bbList.Start(); pBox; pBox = bbList.Next()) {
		if (pBox->info->option == NULL || pBox->bone == NULL || pBox->bone->IsInvisible()) {
			continue;
		}

		const char *pFound = strstr(pBox->info->option, fxTag);
		const char *cur;

		for (; pFound; pFound = strstr(cur, fxTag)) {
			char fxName[0x40];
			cur = GetStrConst(pFound + strlen(fxTag), fxName);

			int len = strlen(fxName);

			if (len == 0 || len >= 0x40) {
				continue;
			}

			char *ext = strrchr(fxName, '.');

			if (ext == NULL || strlen(ext) < 4) {
				continue;
			}

			char token[0x40];
			cur = GetStrConst(cur, token);
			len = strlen(token);

			if (len == 0 || len >= 0x40) {
				continue;
			}

			bool needPart = false;

			while (token[0] == '&') {
				needPart = true;
				unsigned long tid = atoi(&token[1]);

				if (m_pPetFrames[player].GetPartList().FindPart(tid)) {
					needPart = false;
					break;
				}

				unsigned long *pAuxTids = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_characterInfo.auxParts.tids;
				int k;

				for (k = 0; k < 5; k++) {
					if (pAuxTids[k] == tid) {
						needPart = false;
						break;
					}
				}

				if (!needPart) {
					break;
				}

				if (pClubSet && pClubSet->m_pItem && (pClubSet->m_pItem->m_clubTypeIds[GolfClub().GetType()] == tid || pClubSet->m_pItem->m_typeId == tid)) {
					needPart = false;
					break;
				}

				cur = GetStrConst(cur, token);
				len = strlen(token);

				if (len == 0 || len >= 0x40) {
					break;
				}
			}

			if (needPart) {
				continue;
			}

			attacher.m_pMatrix[0] = &pBox->bone->GetMatrix();
			attacher.m_pOffset[0] = &pBox->info->spherePivot;

			if (stricmp(ext + 1, "spr") == 0) {
				CFxSpray *pSpray = (CFxSpray *)CFx::Instance()->Attach(attacher, fxName, false, NULL);
				m_bbSprays.push_back(pSpray);
			} else if (stricmp(ext + 1, "seq") == 0) {
				CFxSequence *pSequence = (CFxSequence *)CFx::Instance()->Attach(attacher, fxName, false, NULL);
				m_bbSequences.push_back(pSequence);
			}
		}
	}

	if (pClubSet) {
		WList<w_bound_box *> &clubBBList = pClubSet->m_puppet[0][GolfClub().GetType()]->m_bbList;
		char clubTag[] = "*fx";

		for (pBox = clubBBList.Start(); pBox; pBox = clubBBList.Next()) {
			if (pBox->info->option == NULL || pBox->bone == NULL || pBox->bone->IsInvisible()) {
				continue;
			}

			const char *pFound = strstr(pBox->info->option, clubTag);
			const char *cur;

			for (; pFound; pFound = strstr(cur, clubTag)) {
				char fxName[0x40];
				cur = GetCharsTillSpace(pFound + strlen(clubTag), fxName);

				int len = strlen(fxName);

				if (len == 0 || len >= 0x40) {
					continue;
				}

				char *ext = strrchr(fxName, '.');

				if (ext == NULL || strlen(ext) < 4) {
					continue;
				}

				attacher.m_pMatrix[0] = &pBox->bone->GetMatrix();
				attacher.m_pOffset[0] = &pBox->info->spherePivot;

				if (stricmp(ext + 1, "spr") == 0) {
					CFxSpray *pSpray = (CFxSpray *)CFx::Instance()->Attach(attacher, fxName, false, NULL);
					m_bbSprays.push_back(pSpray);
				} else if (stricmp(ext + 1, "seq") == 0) {
					CFxSequence *pSequence = (CFxSequence *)CFx::Instance()->Attach(attacher, fxName, false, NULL);
					m_bbSequences.push_back(pSequence);
				}
			}
		}
	}
}

// golfplayer.cpp:1326
void CGolfPlayer::HandleMsg(const MsgObject &msg) {
	if ((unsigned long)msg.m_reserved > g_CurrentTime) {
		StoreDelayedMsg(msg);
		return;
	}

	switch (msg.m_msg) {
	case 0x1A1:
		if (!m_unk18F) {
			if (GolfClub().PowerShotType() && GolfClub().GetType() != 3) {
				if (GolfClub().PowerShotType() == 1 || GolfClub().GetType() == 0) {
					SetMotion(MakeStr("%s\xB0\xD4\xB0\xC9\xC0\xBD\xC6\xC4\xBF\xF6", m_swingTypeName), 0.0f, -1);
				} else {
					SetMotion(MakeStr("%s\xB0\xD4\xB0\xC9\xC0\xBD\xC6\xC4\xBF\xF6" "2", m_swingTypeName), 0.0f, -1);
				}
			} else {
				float speed = 0.0f;
				SendMsg(this, "Screen", 0x1AA, (intptr_t)&speed, 0, 0, 0);
				m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk60 = 0;
				SetMotion(MakeStr("%s\xB0\xD4\xB0\xC9\xC0\xBD", m_swingTypeName), 0.0f, -1);
			}
		}

		if (IsLocalContent((localContentType_t)0x2F)) {
			if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000028) {
				WVector pos = GetPVS().GetGroundPoint(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_worldMatrix.pivot, false, NULL, false);
				if (m_pSafeTeeSeq) {
					m_pSafeTeeSeq->m_pos = pos;
				}
			}
		}

		m_unk190 = true;
		{
			float angle = *(float *)msg.m_param1;
			m_displayMatrix = RotMat(m_unk140 = angle + m_unk140, 1);
		}
		SearchGround();

		m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C |= 0x40;

		if (IsMyTurn(false)) {
			if (m_standBlocked) {
				int zero = 0;
				SendMsg(this, "Letter", 0x18D, (intptr_t)"NoPlay", (intptr_t)&zero, 1, 0);
				SendMsg(this, "Caddie", 0x1E1, 1, 0, 0, 0);
			} else {
				SendMsg(this, "Letter", 0x18C, (intptr_t)"NoPlay", 0, 0, 0);
				SendMsg(this, "Caddie", 0x1E1, 0, 0, 0, 0);
			}
		}
		break;
	case 0x114:
		m_unk2A0 = msg.m_param1 == 1;
		break;
	case 0x110:
		*(float *)msg.m_param1 = m_unk140 - data_9ef200[0];
		break;
	case 0x2715:
		*(const char **)msg.m_param1 = m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_pMotion->name;
		break;
	case 0x2716:
		m_unk330 = msg.m_param3 == 1;
		SetMotion((const char *)msg.m_param1, (float)msg.m_param2, -1);
		m_unk330 = false;
		break;
	case 0x10E:
		if (!m_unk304) {
			if (m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk60 == 2) {
				break;
			}
			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk60 = 2;
			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_nextMotion[0] = '\0';
			SetMotion(m_shotMotionName, 0.0f, -1);

			if ((GolfClub().GetType() == 0 || GolfClub().GetType() == 1) && GOLFDOC()->GetBar().m_accuracyGrade < 2) {
				break;
			}

			WVector delta;
			delta = GolfBall().PreData(GolfBall().m_preDataCount)->m_pos - GetHoleCupPos();
			delta.y = 0.0f;
			float endDist = delta.Magnitude();

			delta = GolfBall().PreData(0)->m_pos - GetHoleCupPos();
			delta.y = 0.0f;
			float startDist = delta.Magnitude();

			switch (GolfClub().GetType()) {
			case 0:
				if (!GolfBall().m_landed && endDist > 416.0f) {
					SendMsg(this, "Player", 0x123, 0, 0, 0, 0);
				}
				break;
			case 3:
				if ((WSingleton<CSharedDoc>::Instance()->m_roomType == 7 && GOLFDOC()->m_clearedShort194 == 2)
					|| (GOLFDOC()->m_clearedFlag1AC && GOLFDOC()->m_clearedShort194 == 3)) {
					if (!(startDist > 32.0f) || !(endDist < 3.2f)) {
						break;
					}
				} else {
					if (!(startDist > 32.0f)) {
						break;
					}
					int chance;
					if (startDist > 96.0f) {
						chance = rand() % 4;
					} else if (startDist > 64.0f) {
						chance = rand() % 3;
					} else {
						chance = rand() % 2;
					}
					if (!(endDist < 3.2f) || chance < 1) {
						break;
					}
				}
				SendMsg(this, "GolfRule", 0x9A, 0, 0, 0, 0);
				strcpy(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_nextMotion, "\xC6\xDB\xC6\xC3\xC8\xC4\xB4\xEB\xB1\xE2");
				break;
			}
		}
		break;
	case 0x10F:
		SetSwingType();
		break;
	case 0x2718: {
		m_unk190 = false;
		m_unk18F = false;
		float angle = *(float *)msg.m_param1;
		m_displayMatrix = RotMat(m_unk140 = angle + data_9ef200[0], 1);
		SearchGround();
		m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C |= 0x248;
		break;
	}
	case 0x2719:
		m_position = *(WVector *)msg.m_param1;
		m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C |= 0x249;
		break;
	case 0x127:
		if (msg.m_param2 == 1) {
			*(WVector *)msg.m_param1 = m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_worldMatrix.pivot;
		} else {
			*(WVector *)msg.m_param1 = m_position;
		}
		break;
	case 0x11D: {
		SetMotion(MakeStr("%s\xC6\xC4\xBF\xF6\xC1\xD8\xBA\xF1", m_swingTypeName), 0.0f, -1);
		m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk60 = 4;
		GolfClub().SetPowerShot(1);
		SendMsg(this, "Spot", 0xF5, 0, 0, 0, 0);
		m_powerShotOn = true;
		m_powerShotKeyState = 0;
		m_unk329 = true;
		float speed = 1.0f;
		SendMsg(this, "Screen", 0x1AA, (intptr_t)&speed, 0, 0, 0);
		break;
	}
	case 0x11E:
		if (IsLocalContent((localContentType_t)0x31)) {
			SetMotion(MakeStr("%s\xC6\xC4\xBF\xF6" "2" "\xC1\xD8\xBA\xF1", m_swingTypeName), 0.0f, -1);
			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk60 = 4;
			GolfClub().SetPowerShot(3);
			SendMsg(this, "Spot", 0xF5, 0, 0, 0, 0);
			m_powerShotOn = true;
			m_powerShotKeyState = 0;
			m_unk329 = true;
			float speed = 1.0f;
			SendMsg(this, "Screen", 0x1AA, (intptr_t)&speed, 0, 0, 0);
		}
		break;
	case 0x124:
		*(WMatrix *)msg.m_param2 = m_pPetFrames[msg.m_param1].GetBonePet()->FindBone("Bip01 Head")->GetMatrix();
		break;
	case 0xE2: {
		for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			const char *puppetName = MakeStr("PLAYER_%d", i);

			if (PLAYER(i)->m_state == 3) {
				if (m_pPetInfos[i].m_unk38) {
					m_pPetInfos[i].m_unk38 = false;
					m_pPetFrames[i].GetBonePet()->HideBone(NULL);
					CSceneManager::Instance()->SetVisible(puppetName, false);
				}
			} else {
				m_pPetInfos[i].m_unk38 = i == GOLFDOC()->m_myPlayerIndex;

				if (m_pPetInfos[i].m_unk38) {
					m_pPetFrames[i].GetBonePet()->ShowBone(NULL);
				} else {
					m_pPetFrames[i].GetBonePet()->HideBone(NULL);
				}

				CSceneManager::Instance()->SetVisible(puppetName, m_pPetInfos[i].m_unk38);

				if (IsCurMapSnow()) {
					Attacher attacher;

					w_bound_box *pBox = m_pPetFrames[i].GetBonePet()->FindBoundBox("Breathe");

					if (pBox) {
						WBone *pBone = pBox->bone;

						attacher.m_pMatrix[0] = &pBone->GetMatrix(); attacher.m_pOffset[0] = &pBox->info->spherePivot; CFx::Instance()->Attach(attacher, "breath.seq", true, NULL);
					}
				}
			}

			if (m_pPetInfos[i].m_unk38 && !WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_eventFlags.eventFlag3 && WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_eventFlags.eventFlag4) {
				Attacher attacher;

				WBone *pBone = m_pPetFrames[i].GetBonePet()->FindBone("Bip01 R Finger1");

				if (pBone) {
					attacher.m_pMatrix[0] = &pBone->GetMatrix();
					CFx::Instance()->Attach(attacher, "High-Rank.seq", true, NULL);
				}

				pBone = m_pPetFrames[i].GetBonePet()->FindBone("Bip01 L Finger1");

				if (pBone) {
					attacher.m_pMatrix[0] = &pBone->GetMatrix();
					CFx::Instance()->Attach(attacher, "High-Rank.seq", true, NULL);
				}
			}
		}

		m_unk2F0 = false;
		m_unk2A0 = false;
		m_unk15C = false;
		m_unk304 = false;
		CSceneManager::Instance()->SetVisible("tail", false);

		SetSwingType();

		float radius = GolfBall().m_radius; const WVector &ballPos = GolfBall().m_pos; m_position = WVector(ballPos.x, ballPos.y - radius, ballPos.z);

		const WVector &minPos = GolfBall().m_pos; WVector boxMin(minPos.x - 16.0f, minPos.y, minPos.z - 16.0f);
		const WVector &maxPos = GolfBall().m_pos; WVector boxMax(maxPos.x + 16.0f, maxPos.y, maxPos.z + 16.0f);
		GetPVS().GetObjList(Waabb(boxMin, boxMax));

		m_unk190 = false; m_unk18F = false;
		float angle = GOLFDOC()->GetCameraData().m_yaw;
		m_displayMatrix = RotMat(m_unk140 = angle + data_9ef200[0], 1);
		SearchGround();

		m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C |= 0x40;
		m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C &= ~0x180;

		m_pTailSpots[0] = m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetBonePet()->FindBoundBox("club_near");
		m_pTailSpots[1] = m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetBonePet()->FindBoundBox("club_far");

		m_pTailEffect->Reset();

		m_pFootBones[0] = m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetBonePet()->FindBoneByTail("L Foot");
		m_pFootBones[1] = m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetBonePet()->FindBoneByTail("R Foot");

		m_headAngle = 0.0f;
		m_pHeadBone = m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetBonePet()->FindBone("Bip01 Head");

		m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_nextMotion[0] = '\0';

		WVector delta = GetHoleCupPos() - PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos;

		delta.y = 0.0f; m_unk2F4 = delta.Magnitude();

		sClubSet *pClubSet = GolfClub().FindClubSet(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId);

		m_pClubPuppet = pClubSet->m_puppet[0][GolfClub().GetType()];
		CSceneManager::Instance()->SetVisible("\xC5\xAC\xB7\xB4", true);

		ParseBoundingBoxScript(GOLFDOC()->m_myPlayerIndex, true, false, true);

		m_powerShotOn = false;
		m_unk329 = false;
		break;
	}
	case 0x2717:
		if ((const char *)msg.m_param1) {
			strcpy(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_nextMotion, (const char *)msg.m_param1);
		} else {
			strcpy(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_nextMotion, MakeStr("%s\xC1\xD8\xBA\xF1", m_swingTypeName));
		}
		break;
	case 0x111:
		*(WBone **)msg.m_param1 = m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetBonePet()->FindBone((const char *)msg.m_param2);
		break;
	case 0x113:
		*(float *)msg.m_param1 = Between(-0.34906584f, m_footAngle, 0.34906584f);
		break;
	case 0x11F:
		if (msg.m_param1) {
			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk60 = 4;
			float speed = 1.0f;
			GolfClub().SetPowerShot(msg.m_param1);
			SendMsg(this, "Spot", 0xF5, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x1AA, (intptr_t)&speed, 0, 0, 0);
			char *motion;
			if (GolfClub().PowerShotType() == 1) {
				motion = MakeStr("%s\xC6\xC4\xBF\xF6\xC1\xD8\xBA\xF1\xB3\xA1", m_swingTypeName);
			} else {
				motion = MakeStr("%s\xC6\xC4\xBF\xF6" "2" "\xC1\xD8\xBA\xF1\xB3\xA1", m_swingTypeName);
			}
			SetMotion(motion, 0.0f, -1);
		} else {
			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk60 = 0;
			float speed = 0.0f;
			GolfClub().SetPowerShot(0);
			SendMsg(this, "Spot", 0xF5, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x1AA, (intptr_t)&speed, 0, 0, 0);
			SetMotion(MakeStr("%s\xC1\xD8\xBA\xF1", m_swingTypeName), 0.0f, -1);
		}
		break;
	case 0x115:
		GolfBall().m_bounceCount = HasStandGround(*(WVector *)msg.m_param1);
		break;
	case 0x126:
		if ((GolfClub().GetType() == 0 || GolfClub().GetType() == 1) && GOLFDOC()->GetBar().m_accuracyGrade < 2) {
			if (GOLFDOC()->GetBar().m_position > GOLFDOC()->GetBar().m_startPosition) {
				strcpy(m_shotMotionName, MakeStr("%s%s\xBA\xFC\xB8\xA5\xC7\xEA\xBD\xBA\xC0\xAE", m_swingTypeName, GolfClub().PowerShotType() ? "\xC6\xC4\xBF\xF6" : ""));
			} else {
				strcpy(m_shotMotionName, MakeStr("%s%s\xB4\xCA\xC0\xBA\xC7\xEA\xBD\xBA\xC0\xAE", m_swingTypeName, GolfClub().PowerShotType() ? "\xC6\xC4\xBF\xF6" : ""));
			}
		} else if (GolfClub().PowerShotType()) {
			strcpy(m_shotMotionName, MakeStr("%s\xC6\xC4\xBF\xF6", m_swingTypeName));
		} else if (GolfClub().GetType() == 3 && m_unk2F4 < 3.2f &&
			Abs(CalcDeltaAngle(GolfBall().m_pos - GetHoleCupPos(), m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_worldMatrix.za)) < 0.5235988f) {
			strcpy(m_shotMotionName, "\xC2\xAA\xC0\xBA\xC6\xDB\xC6\xC3");
		} else {
			strcpy(m_shotMotionName, m_swingTypeName);
		}

		for (unsigned int i = 0; i < 16; i++) {
			if (strcmp(data_a804a8.m_motionNames[i], m_shotMotionName) == 0) {
				*(float *)msg.m_param1 = m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_shotTimes[i];
				break;
			}
		}
		break;
	case 0x77: {
		if ((GolfClub().GetType() == 0 || GolfClub().GetType() == 1) && GOLFDOC()->GetBar().m_accuracyGrade < 2) {
			if (GOLFDOC()->GetBar().m_position > GOLFDOC()->GetBar().m_startPosition) {
				strcpy(m_shotMotionName, MakeStr("%s%s\xBA\xFC\xB8\xA5\xC7\xEA\xBD\xBA\xC0\xAE", m_swingTypeName, GolfClub().PowerShotType() ? "\xC6\xC4\xBF\xF6" : ""));
			} else {
				strcpy(m_shotMotionName, MakeStr("%s%s\xB4\xCA\xC0\xBA\xC7\xEA\xBD\xBA\xC0\xAE", m_swingTypeName, GolfClub().PowerShotType() ? "\xC6\xC4\xBF\xF6" : ""));
			}
		} else if (GolfClub().PowerShotType()) {
			if (GolfClub().PowerShotType() == 1 || GolfClub().GetType() == 0) {
				strcpy(m_shotMotionName, MakeStr("%s\xC6\xC4\xBF\xF6", m_swingTypeName));
			} else {
				strcpy(m_shotMotionName, MakeStr("%s\xC6\xC4\xBF\xF6" "2", m_swingTypeName));
			}
		} else if (GolfClub().GetType() == 3 && m_unk2F4 < 3.2f &&
			Abs(CalcDeltaAngle(GolfBall().m_pos - GetHoleCupPos(), m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_worldMatrix.za)) < 0.5235988f) {
			strcpy(m_shotMotionName, "\xC2\xAA\xC0\xBA\xC6\xDB\xC6\xC3");
		} else {
			strcpy(m_shotMotionName, m_swingTypeName);
		}

		char *motion;
		if (GolfClub().PowerShotType()) {
			if (GolfClub().PowerShotType() == 1) {
				motion = MakeStr("%s\xC6\xC4\xBF\xF6\xC1\xD8\xBA\xF1\xB3\xA1", m_swingTypeName);
			} else {
				motion = MakeStr("%s\xC6\xC4\xBF\xF6" "2" "\xC1\xD8\xBA\xF1\xB3\xA1", m_swingTypeName);
			}
		} else {
			motion = MakeStr("%s\xC1\xD8\xBA\xF1", m_swingTypeName);
		}
		w_motion_data *pMotion = m_pPetFrames[GOLFDOC()->m_myPlayerIndex].FindMotionData(motion, true, NULL);

		if (pMotion->e - pMotion->s > 1.0f) {
			m_unk304 = true;
			SetMotion(pMotion->name, pMotion->e - pMotion->s - 1.0f, -1);
			strcpy(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_nextMotion, m_shotMotionName);
		} else {
			m_unk304 = false;
		}
		break;
	}
	case 0x117: {
		unsigned long tid = msg.m_param1;
		if ((tid & 0xFC000000) == 0x70000000) {
			unsigned long &auxTid = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_characterInfo.auxParts.tids[(tid >> 21) & 0x1F];
			auxTid = auxTid != tid ? tid : 0;

			GOLFDOC()->SetPlayerLevel(GOLFDOC()->m_myPlayerIndex);
			SendMsg(this, "Spot", 0xF5, 0, 0, 0, 0);
			ParseBoundingBoxScript(GOLFDOC()->m_myPlayerIndex, true, false, true);
		} else {
			CPartTidList &tidList = GOLFDOC()->GetPartTidList(GOLFDOC()->m_myPlayerIndex);

			if (ItemManager()->FindPart(tid)) {
				unsigned long changed = m_pPetFrames[GOLFDOC()->m_myPlayerIndex].SetPart(&tidList, tid, 0);

				if (ShowAbilityParts(&tidList)) {
					m_pPetFrames[GOLFDOC()->m_myPlayerIndex].Build(&tidList, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_characterInfo.auxParts.tids, true, true, GOLFDOC()->m_myPlayerIndex);
				}
				m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetBonePet()->xUpdateTnLBuffers(true);

				if (changed) {
					m_pPetFrames[GOLFDOC()->m_myPlayerIndex].CalcBVolume();
					m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C |= 0x13;
				}

				ParseBoundingBoxScript(GOLFDOC()->m_myPlayerIndex, true, false, true);
			}
		}
		break;
	}
	case 0x163:
		m_unk15D = true;
		m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk38 = true;
		m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetBonePet()->ShowBone(NULL);
		CSceneManager::Instance()->SetVisible(MakeStr("PLAYER_%d", GOLFDOC()->m_myPlayerIndex), true);
		CSceneManager::Instance()->SetVisible("\xC5\xAC\xB7\xB4", true);
		break;
	case 0x164:
		m_unk15D = false;
		m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk38 = false;
		m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetBonePet()->HideBone(NULL);
		CSceneManager::Instance()->SetVisible(MakeStr("PLAYER_%d", GOLFDOC()->m_myPlayerIndex), false);
		CSceneManager::Instance()->SetVisible("\xC5\xAC\xB7\xB4", false);

		for (unsigned int i = 0; i < m_bbSprays.size(); i++) {
			CFx::Instance()->CloseSpray(m_bbSprays[i]);
		}
		for (unsigned int j = 0; j < m_bbSequences.size(); j++) {
			CFx::Instance()->CloseSequence(m_bbSequences[j], true);
		}
		m_bbSprays.clear();
		m_bbSequences.clear();
		break;
	case 0x118:
		for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			if (PLAYER(i)->m_state == 3) {
				m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetBonePet()->SetRenderMode(msg.m_param1);
			}
		}
		break;
	case 0x119: {
		std::string motionName;
		char charName[16];

		for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			if (WSingleton<CSharedDoc>::Instance()->m_roomType != 1 && PLAYER(i)->m_state == 3) {
				continue;
			}

			const char *pointName;
			if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
				if (PLAYER(i)->m_partnerIndex != 0xFF) {
					if (GOLFDOC()->GetTeamGameWinner() == 2) {
						if (PLAYER(i)->m_attribute == 0) {
							pointName = i < PLAYER(i)->m_partnerIndex ? "\xC4\xBF\xC7\xC3_1\xB5\xEE" : "\xC4\xBF\xC7\xC3_2\xB5\xEE";
						} else {
							pointName = i < PLAYER(i)->m_partnerIndex ? "\xC4\xBF\xC7\xC3_3\xB5\xEE" : "\xC4\xBF\xC7\xC3_4\xB5\xEE";
						}
					} else if (PLAYER(i)->m_attribute == (unsigned char)GOLFDOC()->GetTeamGameWinner()) {
						pointName = i < PLAYER(i)->m_partnerIndex ? "\xC4\xBF\xC7\xC3_1\xB5\xEE" : "\xC4\xBF\xC7\xC3_2\xB5\xEE";
					} else {
						pointName = i < PLAYER(i)->m_partnerIndex ? "\xC4\xBF\xC7\xC3_3\xB5\xEE" : "\xC4\xBF\xC7\xC3_4\xB5\xEE";
					}
				} else if (GOLFDOC()->GetTeamGameWinner() == 2) {
					if (PLAYER(i)->m_attribute == 0) {
						pointName = "\xC4\xBF\xC7\xC3_1\xB5\xEE";
					} else {
						pointName = "\xC4\xBF\xC7\xC3_3\xB5\xEE";
					}
				} else {
					if (PLAYER(i)->m_attribute == (unsigned char)GOLFDOC()->GetTeamGameWinner()) {
						pointName = "\xC4\xBF\xC7\xC3_1\xB5\xEE";
					} else {
						pointName = "\xC4\xBF\xC7\xC3_3\xB5\xEE";
					}
				}
			} else {
				pointName = MakeStr("%d\xC0\xCE_%d\xB5\xEE", GOLFDOC()->GetCurrentPlayerNum(), PLAYER(i)->m_turnOrder);
			}

			WPolySoup::sPoint *pPoint = GOLFDOC()->m_pPolySoup->FindPoint(pointName);

			m_pPetFrames[i].GetBonePet()->ShowBone(NULL);
			m_pPetFrames[i].GetBonePet()->HideBone("Bone01");
			m_pPetFrames[i].SetAlpha(0xFF);
			CSceneManager::Instance()->SetVisible(MakeStr("PLAYER_%d", i), true);

			m_pPetInfos[i].m_worldMatrix.Reset();
			m_pPetInfos[i].m_worldMatrix.pivot = GetPVS().GetGroundPoint(pPoint->m_pos, false, NULL, false);

			m_pPetInfos[i].m_unk60 = 0;
			m_pPetInfos[i].m_nextMotion[0] = '\0';

			if (WSingleton<CSharedDoc>::Instance()->m_roomType != 1) {
				if (PLAYER(i)->m_joinState == 1) {
					motionName = "1\xB5\xEE\xB8\xF0\xBC\xC7";
				} else if (PLAYER(i)->m_joinState == GOLFDOC()->GetPlayerNum()) {
					motionName = "3\xB5\xEE\xB8\xF0\xBC\xC7";
				} else if (PLAYER(i)->m_joinState == 2) {
					motionName = "2\xB5\xEE\xB8\xF0\xBC\xC7";
				} else {
					motionName = "3\xB5\xEE\xB8\xF0\xBC\xC7";
				}
			} else {
				if (PLAYER(i)->m_attribute == (unsigned char)GOLFDOC()->GetTeamGameWinner() || GOLFDOC()->GetTeamGameWinner() == 2) {
					motionName = "1\xB5\xEE\xB8\xF0\xBC\xC7";
				} else {
					motionName = "3\xB5\xEE\xB8\xF0\xBC\xC7";
				}
			}

			m_pPetInfos[i].m_pMotion = m_pPetFrames[i].FindMotionData(motionName.c_str(), true, NULL);
			m_pPetInfos[i].m_motionTime = m_pPetInfos[i].m_pMotion->s;
			m_pPetInfos[i].m_unk38 = true;

			m_pPetFrames[i].GetBonePet()->ApplyBones(m_pPetFrames[i].GetCurAniPet()->GetBones(NULL, m_pPetInfos[i].m_motionTime, 0, NULL, false), m_pPetInfos[i].m_worldMatrix);

			m_lightSet.nearone = g_lightset.nearone;
			m_lightSet.ambient = ModifyColor(g_lightset.ambient, 1.0f);
			SetLight();

			sprintf(charName, "char%d", i);

			m_pPetFrames[i].SetInvisible(false);

			ScriptProcess(m_pPetInfos[i].m_pMotion, m_pPetInfos[i].m_pMotion->s, m_pPetInfos[i].m_pMotion->s, i, true, false);
		}

		CSceneManager::Instance()->SetPuppet("\xC5\xAC\xB7\xB4", NULL);
		break;
	}
	case 0x11C: {
		unsigned long typeId = msg.m_param1;
		sClubSet *pClubSet = GolfClub().AddClubSet(typeId);

		if (pClubSet) {
			WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId = typeId;
			m_pClubPuppet = pClubSet->m_puppet[0][GolfClub().GetType()];

			if (msg.m_param2) {
				RegisterClub(0);
			}
			CSceneManager::Instance()->SetPuppet("\xC5\xAC\xB7\xB4", m_pClubPuppet);

			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C |= 0x269;

			m_ballDist = m_clubs[GOLFDOC()->m_myPlayerIndex][GolfClub().GetType()].m_ballPos.x;
		}
		break;
	}
	case 0x49: {
		CPartTidList &tidList = GOLFDOC()->GetPartTidList(0);
		tidList.SetTids(&WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_characterInfo, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_level);
		if (!m_pPetFrames->Build(&tidList, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_characterInfo.auxParts.tids, true, true, -1)) {
			tidList.SetDefaultTids();
			m_pPetFrames->Build(&tidList, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_characterInfo.auxParts.tids, true, false, -1);
		}
		m_pPetFrames->CalcBVolume();
		CSceneManager::Instance()->SetPuppet("PLAYER_0", m_pPetFrames->GetBonePet());
		RegisterClub(0);
		break;
	}
	case 0x199:
		*(int *)msg.m_param1 = m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk60;
		break;
	case 0x121: {
		m_pPetFrames[MyIndex()].GetBonePet()->ShowBone(NULL);
		m_pPetFrames[MyIndex()].GetBonePet()->HideBone("Bone01");
		m_pPetFrames[MyIndex()].SetAlpha(0xFF);

		m_pPetInfos[MyIndex()].m_worldMatrix.Reset();
		m_pPetInfos[MyIndex()].m_worldMatrix.pivot = GetPVS().GetGroundPoint(*(WVector *)msg.m_param1, false, NULL, false);

		m_pPetInfos[MyIndex()].m_unk60 = 0;
		m_pPetInfos[MyIndex()].m_nextMotion[0] = '\0';

		m_pPetInfos[MyIndex()].m_pMotion = m_pPetFrames[MyIndex()].FindMotionData((const char *)msg.m_param2, true, NULL);

		if (m_pPetInfos[MyIndex()].m_pMotion) {
			m_pPetInfos[MyIndex()].m_motionTime = m_pPetInfos[MyIndex()].m_pMotion->s;
		} else {
			m_pPetInfos[MyIndex()].m_motionTime = 0.0f;
		}

		m_pPetInfos[MyIndex()].m_unk38 = true;

		m_pPetFrames[MyIndex()].GetBonePet()->ApplyBones(m_pPetFrames[MyIndex()].GetCurAniPet()->GetBones(NULL, m_pPetInfos[MyIndex()].m_motionTime, 0, NULL, false), m_pPetInfos[MyIndex()].m_worldMatrix);

		m_pPetInfos[MyIndex()].m_unk5C |= 0x48;

		CSceneManager::Instance()->SetVisible(MakeStr("PLAYER_%d", MyIndex()), true);

		sClubSet *pClubSet = GolfClub().FindClubSet(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[MyIndex()].m_clubSet.typeId);

		WPuppet *pClub = pClubSet->m_puppet[0][GolfClub().GetType()];

		CSceneManager::Instance()->SetVisible("\xC5\xAC\xB7\xB4", true);
		CSceneManager::Instance()->SetPuppet("\xC5\xAC\xB7\xB4", pClub);

		m_pClubPuppet = pClub;
		m_pPetFrames[MyIndex()].SetInvisible(false);

		ScriptProcess(m_pPetInfos[MyIndex()].m_pMotion, m_pPetInfos[MyIndex()].m_pMotion->s, m_pPetInfos[MyIndex()].m_pMotion->s, MyIndex(), true, false);

		m_pPetInfos[1 - MyIndex()].m_unk38 = false;
		CSceneManager::Instance()->SetVisible(MakeStr("PLAYER_%d", 1 - MyIndex()), false);

		m_lightSet.nearone = g_lightset.nearone;
		m_lightSet.ambient = ModifyColor(g_lightset.ambient, 1.0f);
		SetLight(MyIndex());

		ParseBoundingBoxScript(MyIndex(), true, false, true);
		break;
	}
	case 0x122: {
		MyIndex();
		bool showClub = msg.m_param1 != 0;

		m_pPetFrames[MyIndex()].GetBonePet()->ShowBone(NULL);
		m_pPetFrames[MyIndex()].GetBonePet()->HideBone("Bone01");
		m_pPetFrames[MyIndex()].SetAlpha(0xFF);

		m_pPetInfos[MyIndex()].m_worldMatrix.Reset();
		m_pPetInfos[MyIndex()].m_worldMatrix.pivot = GetPVS().GetGroundPoint(*(WVector *)msg.m_param1, false, NULL, false);

		m_pPetInfos[MyIndex()].m_unk60 = 0;
		m_pPetInfos[MyIndex()].m_nextMotion[0] = '\0';

		m_pPetInfos[MyIndex()].m_pMotion = m_pPetFrames[MyIndex()].FindMotionData((const char *)msg.m_param2, true, NULL);

		if (m_pPetInfos[MyIndex()].m_pMotion) {
			m_pPetInfos[MyIndex()].m_motionTime = m_pPetInfos[MyIndex()].m_pMotion->s;
		} else {
			m_pPetInfos[MyIndex()].m_motionTime = 0.0f;
		}

		m_pPetInfos[MyIndex()].m_unk38 = true;

		m_pPetFrames[MyIndex()].GetBonePet()->ApplyBones(m_pPetFrames[MyIndex()].GetCurAniPet()->GetBones(NULL, m_pPetInfos[MyIndex()].m_motionTime, 0, NULL, false), m_pPetInfos[MyIndex()].m_worldMatrix);

		m_pPetInfos[MyIndex()].m_unk5C |= 0x48;

		for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			m_pPetInfos[i].m_unk38 = i == MyIndex();
			CSceneManager::Instance()->SetVisible(MakeStr("PLAYER_%d", i), i == MyIndex());
		}

		CSceneManager::Instance()->SetVisible("\xC5\xAC\xB7\xB4", showClub);

		if (showClub) {
			sClubSet *pClubSet = GolfClub().FindClubSet(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[MyIndex()].m_clubSet.typeId);

			if (pClubSet) {
				WPuppet *pClub = pClubSet->m_puppet[0][GolfClub().GetType()];

				if (pClub) {
					CSceneManager::Instance()->SetPuppet("\xC5\xAC\xB7\xB4", pClub);
					m_pClubPuppet = pClub;
				} else {
					m_pClubPuppet = NULL;
				}
			} else {
				m_pClubPuppet = NULL;
			}
		} else {
			m_pClubPuppet = NULL;
		}

		m_pPetFrames[MyIndex()].SetInvisible(false);

		ScriptProcess(m_pPetInfos[MyIndex()].m_pMotion, m_pPetInfos[MyIndex()].m_pMotion->s, m_pPetInfos[MyIndex()].m_pMotion->s, MyIndex(), true, false);

		m_lightSet.nearone = g_lightset.nearone;
		m_lightSet.ambient = ModifyColor(g_lightset.ambient, 1.0f);
		SetLight(MyIndex());

		ParseBoundingBoxScript(MyIndex(), true, false, true);
		break;
	}
	case 0x120: {
		// Both team partners: param1 and param2 are their motions, param3 the
		// point they stand around.
		const char *motionNames[2] = { (const char *)msg.m_param1, (const char *)msg.m_param2 };
		WVector pos;
		pos = *(WVector *)msg.m_param3;

		for (int i = 0; i < 2; i++) {
			int player;

			if (PLAYER(MyIndex())->m_partnerIndex != 0xFF) {
				player = i != 0 ? PLAYER(MyIndex())->m_partnerIndex : MyIndex();
			} else {
				if (i == 1) {
					break;
				}

				player = MyIndex();
			}

			CPetFrame &frame = m_pPetFrames[player];
			sPetInfo &info = m_pPetInfos[player];

			frame.GetBonePet()->ShowBone(NULL);
			frame.GetBonePet()->HideBone("Bone01");
			frame.SetAlpha(0xFF);

			info.m_worldMatrix.Reset();
			info.m_worldMatrix.pivot = GetPVS().GetGroundPoint(WVector((i == 0 ? -3.0f : 3.0f) + pos.x, pos.y, pos.z), false, NULL, false);

			info.m_unk60 = 0;
			info.m_nextMotion[0] = '\0';
			info.m_pMotion = frame.FindMotionData(motionNames[i], true, NULL);

			if (info.m_pMotion) {
				info.m_motionTime = info.m_pMotion->s;
			} else {
				info.m_motionTime = 0.0f;
			}

			info.m_unk38 = true;
			frame.GetBonePet()->ApplyBones(frame.GetCurAniPet()->GetBones(NULL, info.m_motionTime, 0, NULL, false), info.m_worldMatrix);

			m_pPetInfos[player].m_unk5C |= 0x48;

			CSceneManager::Instance()->SetVisible(MakeStr("PLAYER_%d", player), true);

			// m_clubHolder: the team member holding the club, m_clubHolderClub: its club type.
			if (player == TEAM(PLAYER(MyIndex())->m_attribute)->m_clubHolder) {
				sClubSet *pClubSet = GolfClub().FindClubSet(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[player].m_clubSet.typeId);

				if (pClubSet == NULL) {
					continue;
				}

				WPuppet *pClub;

				if (i == GOLFDOC()->m_myPlayerIndex) {
					pClub = pClubSet->m_puppet[0][GolfClub().GetType()];
				} else {
					pClub = pClubSet->m_puppet[0][TEAM(PLAYER(MyIndex())->m_attribute)->m_clubHolderClub];
				}

				if (pClub == NULL) {
					continue;
				}

				CSceneManager::Instance()->SetVisible("\xC5\xAC\xB7\xB4", true);
				CSceneManager::Instance()->SetPuppet("\xC5\xAC\xB7\xB4", pClub);

				m_pClubPuppet = pClub;
			}

			m_pPetFrames[player].SetInvisible(false);

			ScriptProcess(m_pPetInfos[player].m_pMotion, m_pPetInfos[player].m_pMotion->s, m_pPetInfos[player].m_pMotion->s, player, true, false);
		}

		for (int j = 0; j < GOLFDOC()->GetPlayerNum(); j++) {
			if (j != MyIndex()) {
				if ((PLAYER(MyIndex())->m_partnerIndex != 0xFF && j != PLAYER(MyIndex())->m_partnerIndex) || PLAYER(MyIndex())->m_partnerIndex == 0xFF) {
					m_pPetInfos[j].m_unk38 = false;
					CSceneManager::Instance()->SetVisible(MakeStr("PLAYER_%d", j), false);
				}
			}
		}

		m_lightSet.nearone = g_lightset.nearone;
		m_lightSet.ambient = ModifyColor(g_lightset.ambient, 1.0f);
		SetLight(MyIndex());

		ParseBoundingBoxScript(MyIndex(), true, false, true);

		if (PLAYER(MyIndex())->m_partnerIndex != 0xFF) {
			ParseBoundingBoxScript(PLAYER(MyIndex())->m_partnerIndex, false, false, false);
		}
		break;
	}
	case 0x123: {
		CPartTidList &tidList = GOLFDOC()->GetPartTidList(GOLFDOC()->m_myPlayerIndex);
		if (GolfClub().PowerShotType()) {
			if ((tidList.m_tidChar & 0x3FFFFFF) == 4) {
				if (GolfClub().PowerShotType() == 1) {
					strcpy(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_nextMotion, "\xBF\xEC\xB5\xE5\xBC\xA6\xC6\xC4\xBF\xF6\xC8\xC4\xB8\xF0\xBC\xC7");
				} else {
					strcpy(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_nextMotion, "\xBF\xEC\xB5\xE5\xBC\xA6\xC6\xC4\xBF\xF6" "2" "\xC8\xC4\xB8\xF0\xBC\xC7");
				}
			} else if (GolfClub().PowerShotType() == 1) {
				strcpy(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_nextMotion, "\xBF\xEC\xB5\xE5\xBC\xA6\xC8\xC4\xB8\xF0\xBC\xC7");
			} else {
				strcpy(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_nextMotion, "\xBF\xEC\xB5\xE5\xBC\xA6\xC6\xC4\xBF\xF6" "2" "\xC8\xC4\xB8\xF0\xBC\xC7");
			}
		} else {
			strcpy(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_nextMotion, "\xBF\xEC\xB5\xE5\xBC\xA6\xC8\xC4\xB8\xF0\xBC\xC7");
		}
		break;
	}
	case 0x125:
		if (msg.m_param1 == 0) {
			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C |= 0x80;
			CSceneManager::Instance()->SetVisible("\xC5\xAC\xB7\xB4", false);
		} else {
			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C &= ~0x80;
			CSceneManager::Instance()->SetVisible("\xC5\xAC\xB7\xB4", true);
		}
		break;
	case 0x128:
		if (m_pPetInfos && m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_pMotion &&
			(strstr(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_pMotion->name, "\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE") ||
			strstr(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_pMotion->name, "\xBD\xC7\xB8\xC1\xC6\xF7\xC1\xEE") ||
			strstr(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_pMotion->name, "\xB5\xEE\xC0\xE5\xB8\xF0\xBC\xC7") ||
			strstr(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_pMotion->name, "\xB5\xEE\xB8\xF0\xBC\xC7"))) {
			*(WVector *)msg.m_param1 = WVector::ZERO;
		} else {
			*(WVector *)msg.m_param1 = m_clubs[GOLFDOC()->m_myPlayerIndex][GolfClub().m_pClub->m_type].m_ballPos;
		}
		break;
	case 0x129:
		*(int *)msg.m_param1 = m_comboRunning;
		*(float *)msg.m_param2 = m_comboTime;
		break;
	case 0x1F8:
		if (msg.m_param1 < GOLFDOC()->GetPlayerNum()) {
			CFxSequence *pSeq = CFx::Instance()->OpenSequence("Talk_Room.seq", true);
			if (pSeq) {
				const WVector &pivot = m_pPetInfos[msg.m_param1].m_worldMatrix.pivot;
				pSeq->m_pos = WVector(pivot.x, pivot.y + 5.0f, pivot.z);
			}
		}
		break;
	case 0x157:
		if (msg.m_param1 != -1) {
			PLAYER(0)->m_power = msg.m_param1;
		}
		if (msg.m_param2 != -1) {
			PLAYER(0)->m_control = msg.m_param2;
		}
		if (msg.m_param3 != -1) {
			PLAYER(0)->m_curve = msg.m_param3;
		}
		break;
	case 0x203:
		if (IsLocalContent((localContentType_t)0x2F)) {
			if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000028) {
				if (msg.m_param1) {
					m_pSafeTeeSeq = CFx::Instance()->OpenSequence("safetee_Avatar.seq", true);

					WVector pos;
					pos = GetPVS().GetGroundPoint(m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_worldMatrix.pivot, false, NULL, false);
					if (m_pSafeTeeSeq) {
						m_pSafeTeeSeq->m_pos = pos;
					}
				} else {
					CFx::Instance()->CloseSequence(m_pSafeTeeSeq, false);
				}
			} else {
				CFx::Instance()->CloseSequence(m_pSafeTeeSeq, false);
			}
		}
		break;
	}
}

// golfplayer.cpp:2726
void CGolfPlayer::SetMotion(const char *name, float time, int player) {
	if (player < 0) {
		player = GOLFDOC()->m_myPlayerIndex;
	}

	if (IsLocalContent((localContentType_t)0x6D)) {
		int count = m_pPetFrames[player].GetMotionCountIncludeName(name, data_a804a8.m_motionNames[16]);
		if (count > 1) {
			char motionName[0x80] = "";
			sprintf(motionName, "%s%02d", name, rand() % count + 1);

			m_pPetInfos[player].m_pMotion = m_pPetFrames[player].FindMotionData(motionName, true, NULL);

			if (m_pPetInfos[player].m_pMotion == NULL) {
				m_pPetInfos[player].m_pMotion = m_pPetFrames[player].FindMotionData(name, true, NULL);
			}
		} else {
			m_pPetInfos[player].m_pMotion = m_pPetFrames[player].FindMotionData(name, true, NULL);
		}
	} else {
		m_pPetInfos[player].m_pMotion = m_pPetFrames[player].FindMotionData(name, true, NULL);
	}

	if (m_pPetInfos[player].m_pMotion == NULL) {
		return;
	}

	m_pPetInfos[player].m_scale = 1.0f;
	m_pPetInfos[player].m_unkB0 = 0;

	if (time < 0.0f) {
		m_pPetInfos[player].m_motionTime = Max(m_pPetInfos[player].m_pMotion->s, time + m_pPetInfos[player].m_pMotion->e);
	} else {
		m_pPetInfos[player].m_motionTime = Min(time + m_pPetInfos[player].m_pMotion->s, m_pPetInfos[player].m_pMotion->e);
	}

	m_pPetInfos[player].m_unk5C |= 0x33;
	m_unk2F0 = ((m_pPetInfos[player].m_unk5C >> 8) & 1) != 0;
	m_pPetInfos[player].m_unk5C &= ~0x180;

	m_pPetFrames[player].SetInvisible(false);

	ScriptProcess(m_pPetInfos[player].m_pMotion, m_pPetInfos[player].m_pMotion->s, m_pPetInfos[player].m_pMotion->s, player, true, time < 0.0f);

	if ((m_pPetFrames[player].GetTidChar() & 0x3FFFFFF) == 6 && !(WAbsBySign(time) < data_9ef200[3]) && strcmpi(name, "\xB5\xEE\xC0\xE5\xB8\xF0\xBC\xC7") == 0) {
		WBone *pBone = m_pPetFrames[player].GetBonePet()->FindBone("G_Bird");
		if (pBone != NULL) {
			pBone->SetInvisible(false, true);
		}
	}

	m_animFx[player].~vector();
}

// golfplayer.cpp:2808
void CGolfPlayer::SetSwingType(void) {
	switch (GolfClub().GetType()) {
	case 0:
		strcpy(m_swingTypeName, "\xBF\xEC\xB5\xE5\xBC\xA6");
		break;
	case 1:
		strcpy(m_swingTypeName, "\xBE\xC6\xC0\xCC\xBE\xF0\xBC\xA6");
		break;
	case 2:
		strcpy(m_swingTypeName, "\xBB\xF7\xB5\xE5\x9F\x87\xC1\xF6\xBC\xA6");
		break;
	case 3:
		strcpy(m_swingTypeName, "\xC6\xDB\xC6\xC3\xBC\xA6");
		break;
	}

	char *motion;
	if (GolfClub().PowerShotType()) {
		if (GolfClub().PowerShotType() == 1) {
			motion = MakeStr("%s\xC6\xC4\xBF\xF6\xC1\xD8\xBA\xF1\xB3\xA1", m_swingTypeName);
		} else {
			motion = MakeStr("%s\xC6\xC4\xBF\xF6" "2\xC1\xD8\xBA\xF1\xB3\xA1", m_swingTypeName);
		}
	} else {
		motion = MakeStr("%s\xC1\xD8\xBA\xF1", m_swingTypeName);
	}

	SetMotion(motion, 0.0f, -1);

	m_ballDist = m_clubs[GOLFDOC()->m_myPlayerIndex][GolfClub().GetType()].m_ballPos.x;
	m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk60 = 0;
}

// golfplayer.cpp:2845
void CGolfPlayer::ScriptProcess(w_motion_data *data, float from, float to, int player, bool all, bool skip) {
	if (data == NULL) {
		return;
	}

	if ((!(from <= data->e) || from == to) && !all) {
		return;
	}

	int start, end;
	from *= 30.0f;
	if (all) {
		start = Ceil(from, 0.03f);
		end = start;
	} else {
		to = Min(data->e, to) * 30.0f;
		start = Ceil(from, 0.03f);
		end = WAbsBySign((float)floor(to) - to) < data_9ef200[3] ? (int)to - 1 : (int)floor(to);

		if (start == (int)floor(data->s * 30.0f + 0.5f)) {
			start++;
		}

		if (start > end) {
			return;
		}
	}

	int first, last;
	if (m_pPetFrames[player].GetCurAniPet()->CheckFrameData(start, end, first, last)) {
		for (int i = first; i <= last; i++) {
			ParsePuppetScript(m_pPetFrames[player].GetCurAniPet()->GetFrameData(i), player, skip);
		}
	}
}

// golfplayer.cpp:2887
void CGolfPlayer::CloseBoundingBoxFx(int slot, unsigned long fxId) {
	if (fxId == 0 || slot > 4) {
		return;
	}

	std::vector<sCondBBSpray>::iterator spray;
	for (spray = m_condBBSprays[slot].begin(); spray != m_condBBSprays[slot].end(); ) {
		if (spray->m_fxId == fxId) {
			CFx::Instance()->CloseSpray(spray->m_pSpray);
			m_condBBSprays[slot].erase(spray);
		} else {
			++spray;
		}
	}

	std::vector<sCondBBSequence>::iterator sequence;
	for (sequence = m_condBBSequences[slot].begin(); sequence != m_condBBSequences[slot].end(); ) {
		if (sequence->m_fxId == fxId) {
			CFx::Instance()->CloseSequence(sequence->m_pSequence, true);
			m_condBBSequences[slot].erase(sequence);
		} else {
			++sequence;
		}
	}
}

// golfplayer.cpp:2924
void CGolfPlayer::ParsePuppetScript(char *script, int player, bool skip) {
	if (script == NULL) {
		return;
	}

	const char *cur = script;
	unsigned long value;

	do {
		const char *cmd = strchr(cur, '*');

		if (cmd == NULL) {
			break;
		}

		int len = 0;

		while (cmd[len] != '\0') {
			len++;
			if (cmd[len] == ' ') {
				break;
			}
			if (cmd[len] == '(') {
				len++;
				break;
			}
		}

		if (len <= 1) {
			break;
		}

		strncpy(data_af30a8, cmd, len);
		data_af30a8[len] = '\0';
		cur = cmd + len;

		for (int i = 0; i < sizeof(data_a804a8.m_commands) / sizeof(data_a804a8.m_commands[0]); i++) {
			if (strcmpi(data_af30a8, data_a804a8.m_commands[i]) != 0) {
				continue;
			}

			switch (i) {
			case 0: {
				cur = GetStrConst(cur, data_af3068);

				if ((value = strlen(data_af3068)) == 0 || strlen(data_af3068) >= 0x40) {
					break;
				}

				WVector pos = m_position - g_camera.pivot;
				g_audio->PlaySfx(data_af3068, &pos, 5, 0, 0, 0.5f, 200.0f);
				break;
			}
			case 1:
				PlayShotSfx(player);

				if (GOLFDOC()->GameMode() & 0x40) {
					SendMsg(this, "GolfRule", 0x78, 0x80, 0, 0, 0);
				}
				break;
			case 2: {
				int j;

				for (j = 0; j < 3; j++) {
					if (WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId == data_a804a8.m_tails[j].m_clubSetId) {
						m_pTailEffect->ChangeTexture(std::string(data_a804a8.m_tails[j].m_texName));
						break;
					}
				}

				if (j == 3) {
					m_pTailEffect->ChangeTexture(std::string("~swinglight01.jpg"));
				}

				m_unk15C = true;
				CSceneManager::Instance()->SetVisible("tail", true);
				break;
			}
			case 3:
				m_pPetInfos[player].m_unk5C &= ~0x80;
				// "club" in Korean
				CSceneManager::Instance()->SetVisible("\xC5\xAC\xB7\xB4", true);
				break;
			case 4:
				m_pPetInfos[player].m_unk5C |= 0x80;
				CSceneManager::Instance()->SetVisible("\xC5\xAC\xB7\xB4", false);
				break;
			case 5:
				break;
			case 6:
				if (!m_unk2F0) {
					w_bound_box *pBox = m_pPetFrames[player].GetBonePet()->FindBoundBox("club");
					m_fixedClubMatrix = pBox->bone->GetMatrix();
					m_fixedClubMatrix.pivot = pBox->info->spherePivot * pBox->bone->GetMatrix();
					m_fixedClubMatrix.pivot += m_clubs[player][GolfClub().GetType()].m_gripPos * m_fixedClubMatrix.xa;
				}

				m_pPetInfos[player].m_unk5C |= 0x100;
				break;
			case 7: {
				if (skip) {
					break;
				}

				char fxName[0x80];
				fxName[0] = '\0';
				cur = GetStrConst(cur, fxName);

				if ((value = strlen(fxName)) == 0 || strlen(fxName) >= 0x40) {
					break;
				}

				char bbName[0x100];

				if (fxName[0] == '@') {
					if (m_pPetInfos[player].m_pMotion == NULL || strcmp(fxName + 1, m_pPetInfos[player].m_pMotion->name) != 0) {
						break;
					}

					cur = GetStrConst(cur, bbName);

					if ((value = strlen(bbName)) == 0 || strlen(bbName) >= 0x40) {
						break;
					}
				} else {
					strcpy(bbName, fxName);
				}

				if (stricmp(bbName, "*playbbfx") == 0) {
					ParseBoundingBoxScript(player, true, true, true);
					break;
				}

				char token[0x40];
				cur = GetStrConst(cur, token);

				int tokenLen = strlen(token);
				if (tokenLen == 0 || tokenLen >= 0x40) {
					break;
				}

				sClubSet *pClubSet = GolfClub().FindClubSet(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[player].m_clubSet.typeId);
				bool found = false;

				if (token[0] == '&') {
					while (token[0] == '&') {
						value = atoi(token + 1);

						if (!found) {
							if (m_pPetFrames[player].GetPartList().FindPart(value)
								|| (pClubSet && pClubSet->m_pItem && pClubSet->m_pItem->m_clubTypeIds[GolfClub().GetType()] == value)) {
								found = true;
							}
						}

						cur = GetStrConst(cur, token);

						tokenLen = strlen(token);
						if (tokenLen == 0 || tokenLen >= 0x40) {
							break;
						}
					}

					if (!found) {
						break;
					}

					CloseBoundingBoxFx(player, value);
					m_animFx[player].push_back(value);
				}

				char boneName[0x40];
				strcpy(boneName, token);

				if ((value = strlen(boneName)) == 0 || strlen(boneName) >= 0x40) {
					break;
				}

				Attacher attacher;
				int k;

				for (k = 0; k < sizeof(attacher.m_pMatrix) / sizeof(attacher.m_pMatrix[0]); k++) {
					if (k != 0) {
						cur = GetStrConst(cur, boneName);

						if ((value = strlen(boneName)) == 0 || strlen(boneName) >= 0x40) {
							break;
						}
					}

					WBone *pBone = m_pPetFrames[player].GetBonePet()->FindBone(boneName);

					if (pBone) {
						attacher.m_pMatrix[k] = &pBone->GetMatrix();
						continue;
					}

					w_bound_box *pBox = m_pPetFrames[player].GetBonePet()->FindBoundBox(boneName);

					if (pBox) {
						if (pBox->bone) {
							attacher.m_pMatrix[k] = &pBox->bone->GetMatrix();
						}
						attacher.m_pOffset[k] = &pBox->info->spherePivot;
						continue;
					}

					if (pClubSet && pClubSet->m_puppet[0][GolfClub().GetType()]) {
						WBone *pClubBone = pClubSet->m_puppet[0][GolfClub().GetType()]->FindBone(boneName);

						if (pClubBone) {
							attacher.m_pMatrix[k] = &pClubBone->GetMatrix();
						} else {
							pBox = pClubSet->m_puppet[0][GolfClub().GetType()]->m_bbList.Find(boneName);

							if (pBox) {
								if (pBox->bone) {
									attacher.m_pMatrix[k] = &pBox->bone->GetMatrix();
								}
								attacher.m_pOffset[k] = &pBox->info->spherePivot;
							}
						}
					}
				}

				if (k > 0) {
					CFx::Instance()->Attach(attacher, bbName, true, NULL);
				}
				break;
			}
			case 8: {
				char texName[0x40];
				char texPath[0x400];

				texName[0] = '\0';
				texPath[0] = '\0';
				cur = GetStrConst(cur, texName);

				if ((value = strlen(texName)) == 0 || strlen(texName) >= 0x40) {
					break;
				}

				if (texName[0] == '@') {
					if (m_pPetInfos[player].m_pMotion == NULL || strcmpi(texName + 1, m_pPetInfos[player].m_pMotion->name) != 0) {
						break;
					}

					cur = GetStrConst(cur, texPath);

					if ((value = strlen(texPath)) == 0 || strlen(texPath) >= 0x400) {
						break;
					}
				} else {
					strcpy(texPath, cur);
				}

				cur = GetStrConst(cur, texPath);

				if ((value = strlen(texPath)) == 0 || strlen(texPath) >= 0x400) {
					break;
				}

				SetPendingTexPart(player, texPath);
				break;
			}
			case 9:
			case 10: {
				char boneName[0x40] = "";

				for (int k = 0; k < 0x40; k++) {
					cur = GetStrConst(cur, boneName);

					if ((value = strlen(boneName)) == 0 || strlen(boneName) >= 0x40) {
						break;
					}

					WBone *pBone = m_pPetFrames[player].GetBonePet()->FindBone(boneName);

					if (pBone) {
						pBone->SetInvisible(false, i != 9);
					}

					if (cur == NULL || *cur == ')') {
						break;
					}
				}
				break;
			}
			case 11: {
				int offset = 0;

				data_af2b68[0] = '\0';
				data_af2c68[0] = '\0';
				cur = GetStrConst(cur, data_af2c68);
				cur = GetStrConst(cur, data_af2b68);

				if (data_af2c68[0] == '\0' || data_af2b68[0] == '\0') {
					break;
				}

				if (m_unk330) {
					break;
				}

				while (true) {
					char *pCond = data_af2c68 + offset;
					char *pSemi = strchr(pCond, ';');
					int n;

					if (pSemi) {
						n = pSemi - (data_af2c68 + offset);
					} else {
						n = strlen(pCond);

						if (n == 0) {
							break;
						}
					}

					char petName[0x28];
					strncpy(petName, pCond, n);
					petName[n] = '\0';
					strcat(petName, ".mpet");

					if (m_pPetFrames[player].HasMPet(petName)) {
						SetMotion(data_af2b68, 0.0f, player);
						m_pPetInfos[player].m_nextMotion[0] = '\0';
						break;
					}

					if (pSemi == NULL) {
						break;
					}

					offset += n + 1;
				}
				break;
			}
			case 12: {
				char name[0x80];
				char args[0x40];

				name[0] = '\0';
				args[0] = '\0';
				cur = GetStrConst(cur, name);

				if ((value = strlen(name)) == 0 || strlen(name) >= 0x40) {
					break;
				}

				if (name[0] == '@') {
					if (m_pPetInfos[player].m_pMotion == NULL || strcmpi(name + 1, m_pPetInfos[player].m_pMotion->name) != 0) {
						break;
					}

					cur = GetStrConst(cur, args);

					if ((value = strlen(args)) == 0 || strlen(args) >= 0x40) {
						break;
					}
				} else {
					strcpy(args, name);
				}

				if ((value = strlen(args)) == 0 || strlen(args) >= 0x40) {
					break;
				}

				float from, to;
				float time = 0.0f;

				sscanf(args, "%f %f %f", &from, &to, &time);
				m_pPetInfos[player].m_scale = from;
				time *= 1.0f / 30.0f;

				if (time > 0.0f) {
					m_pPetInfos[player].m_unkB0 = (to - from) / time;
				} else {
					m_pPetInfos[player].m_unkB0 = 0.0f;
				}
				break;
			}
			case 13:
			case 14:
				ParseScript_ShowVis(player, i == 13, cur);
				break;
			}
		}
	} while (cur);

	m_unk2F0 = false;
}

// golfplayer.cpp:3474
void CGolfPlayer::ParseScript_ShowVis(int player, bool show, const char *script) {
	char token[0x100] = "";
	int count = 0;

	do {
		script = GetStrConst(script, token);

		if (strlen(token) == 0 || strlen(token) >= sizeof(token)) {
			break;
		}

		std::list<std::string> *pGroup = WSingleton<CSharedDoc>::Instance()->GetVisGroup(token);

		if (pGroup) {
			for (std::list<std::string>::iterator it = pGroup->begin(); it != pGroup->end(); ++it) {
				WBone *pBone = m_pPetFrames[player].GetBonePet()->FindBone(it->c_str());

				if (pBone) {
					pBone->SetInvisible(false, !show);
				}
			}
		}

		if (script == NULL || *script == ')') {
			break;
		}
	} while (++count < 64);
}

// golfplayer.cpp:3507
void CGolfPlayer::PlayShotSfx(int player) {
	if (player < 0 || player >= GOLFDOC()->GetPlayerNum()) {
		return;
	}

	// "헛스윙" (whiff)
	if (strstr(m_pPetInfos[player].m_pMotion->name, "\xC7\xEA\xBD\xBA\xC0\xAE")) {
		if (GolfClub().GetType() == 0) {
			// "빠른헛스윙" (fast whiff)
			if (strstr(m_shotMotionName, "\xBA\xFC\xB8\xA5\xC7\xEA\xBD\xBA\xC0\xAE") == NULL) {
				g_audio->PlaySfx("swing_vain", 0, 0, 0, 0, 0.5f, 200.0f);
			} else {
				g_audio->PlaySfx("swing_miss_wood", 0, 0, 0, 0, 0.5f, 200.0f);
			}
		} else if (GolfClub().GetType() == 1) {
			g_audio->PlaySfx("swing_miss_iron", 0, 0, 0, 0, 0.5f, 200.0f);
		}

		if ((GolfClub().GetType() == 0 || GolfClub().GetType() == 1) && GOLFDOC()->GetBar().m_accuracyGrade < 2) {
			SendMsg(this, "Gallery", 0x103, 0, 0, 0, 0);
		}
		return;
	}

	int score;
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
		score = TEAM(GOLFDOC()->GetPlayer(player)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1];
	} else {
		score = GOLFDOC()->GetPlayer(player)->m_holes[GOLFDOC()->m_currentHole - 1];
	}

	if (GolfClub().PowerShotType()) {
		g_audio->PlaySfx("swing_powershot_effect", 0, 0, 0, 0, 0.5f, 200.0f);
	}

	float accuracy = 0.0f;
	SendMsg(this, "GolfRule", 0x8B, (intptr_t)&accuracy, 0, 0, 0);
	int strength = 1;
	if (!(accuracy > 0.9f)) {
		strength = 2;
	}

	IFF_STRUCT::sClubSet *pClubSet = ItemManager()->FindClubSet(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId);
	int special = 0;

	if (pClubSet) {
		if ((pClubSet->m_typeId & 0x3FFFFFF) == 8) {
			special = 1;
		}
	}

	if (GolfBall().m_powerFlags & 0x10) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId == 0x1400000B) {
			g_audio->PlaySfx("HG_SpecialShot_1", 0, 0, 0, 0, 0.5f, 200.0f);
		} else {
			g_audio->PlaySfx("swing_tomahawk", 0, 0, 0, 0, 0.5f, 200.0f);
		}
		g_audio->PlaySfx("fly_tomahawk", 0, 0, 0, 0, 0.5f, 200.0f);
	} else if (GolfBall().m_powerFlags & 0x20) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId == 0x1400000B) {
			g_audio->PlaySfx("HG_SpecialShot_1", 0, 0, 0, 0, 0.5f, 200.0f);
		} else {
			g_audio->PlaySfx("swing_cobra", 0, 0, 0, 0, 0.5f, 200.0f);
		}
		g_audio->PlaySfx("fly_cobra", 0, 0, 0, 0, 0.5f, 200.0f);
	}

	switch (GolfClub().GetType()) {
	case 0:
		if (special == 0) {
			if (score == 1) {
				if (strength == 1) {
					g_audio->PlaySfx("swing_tee_s", 0, 0, 0, 0, 0.5f, 200.0f);
				} else {
					g_audio->PlaySfx("swing_tee_w", 0, 0, 0, 0, 0.5f, 200.0f);
				}
			} else {
				if (strength == 1) {
					g_audio->PlaySfx("swing_drive_s", 0, 0, 0, 0, 0.5f, 200.0f);
				} else {
					g_audio->PlaySfx("swing_drive_w", 0, 0, 0, 0, 0.5f, 200.0f);
				}
			}
		} else if (special == 1) {
			g_audio->PlaySfx("picohan_wood", 0, 0, 0, 0, 0.5f, 200.0f);
		}

		if (GolfBall().m_powerFlags & 0x10) {
			g_audio->PlaySfx("swing_tomahawk", 0, 0, 0, 0, 0.5f, 200.0f);
			g_audio->PlaySfx("fly_tomahawk", 0, 0, 0, 0, 0.5f, 200.0f);
		} else if ((GolfBall().m_powerFlags & 0x20) ||
			(GolfBall().m_powerFlags & 0x40)) {
			g_audio->PlaySfx("swing_cobra", 0, 0, 0, 0, 0.5f, 200.0f);
			g_audio->PlaySfx("fly_cobra", 0, 0, 0, 0, 0.5f, 200.0f);
		}
		break;
	case 1:
		if (special == 0) {
			if (strength == 1) {
				g_audio->PlaySfx("swing_normal_s", 0, 0, 0, 0, 0.5f, 200.0f);
			} else {
				g_audio->PlaySfx("swing_normal_w", 0, 0, 0, 0, 0.5f, 200.0f);
			}
		} else if (special == 1) {
			g_audio->PlaySfx("picohan_iron", 0, 0, 0, 0, 0.5f, 200.0f);
		}

		if (GolfBall().m_powerFlags & 0x10) {
			g_audio->PlaySfx("swing_tomahawk", 0, 0, 0, 0, 0.5f, 200.0f);
			g_audio->PlaySfx("fly_tomahawk", 0, 0, 0, 0, 0.5f, 200.0f);
		}

		// The ground sounds below are tail-merged with case 2's in the original.
		switch (GolfBall().m_groundType) {
		case 1:
			// "아이언_페어웨이%d" (iron, fairway)
			g_audio->PlaySfx(MakeStr("\xBE\xC6\xC0\xCC\xBE\xF0_\xC6\xE4\xBE\xEE\xBF\xFE\xC0\xCC%d", strength), 0, 0, 0, 0, 0.5f, 200.0f);
			break;
		case 3:
			// "아이언_모래밭%d" (iron, sand), "벙커모래날림" (bunker sand spray)
			g_audio->PlaySfx(MakeStr("\xBE\xC6\xC0\xCC\xBE\xF0_\xB8\xF0\xB7\xA1\xB9\xE7%d", strength), 0, 0, 0, 0, 0.5f, 200.0f);
			g_audio->PlaySfx("\xBA\xA1\xC4\xBF\xB8\xF0\xB7\xA1\xB3\xAF\xB8\xB2", 0, 0, 0, 0, 0.5f, 200.0f);
			break;
		case 4:
			// "아이언_나뭇잎%d" (iron, leaves)
			g_audio->PlaySfx(MakeStr("\xBE\xC6\xC0\xCC\xBE\xF0_\xB3\xAA\xB9\xB5\xC0\xD9%d", strength), 0, 0, 0, 0, 0.5f, 200.0f);
			break;
		}
		break;
	case 2:
		if (special == 0) {
			g_audio->PlaySfx("swing_putting", 0, 0, 0, 0, 0.5f, 200.0f);
		} else if (special == 1) {
			g_audio->PlaySfx("mogura", 0, 0, 0, 0, 0.5f, 200.0f);
		}

		if (GolfBall().m_powerFlags & 0x10) {
			g_audio->PlaySfx("swing_tomahawk", 0, 0, 0, 0, 0.5f, 200.0f);
			g_audio->PlaySfx("fly_tomahawk", 0, 0, 0, 0, 0.5f, 200.0f);
		}

		switch (GolfBall().m_groundType) {
		case 1:
			g_audio->PlaySfx(MakeStr("\xBE\xC6\xC0\xCC\xBE\xF0_\xC6\xE4\xBE\xEE\xBF\xFE\xC0\xCC%d", strength), 0, 0, 0, 0, 0.5f, 200.0f);
			break;
		case 3:
			g_audio->PlaySfx(MakeStr("\xBE\xC6\xC0\xCC\xBE\xF0_\xB8\xF0\xB7\xA1\xB9\xE7%d", strength), 0, 0, 0, 0, 0.5f, 200.0f);
			g_audio->PlaySfx("\xBA\xA1\xC4\xBF\xB8\xF0\xB7\xA1\xB3\xAF\xB8\xB2", 0, 0, 0, 0, 0.5f, 200.0f);
			break;
		case 4:
			g_audio->PlaySfx(MakeStr("\xBE\xC6\xC0\xCC\xBE\xF0_\xB3\xAA\xB9\xB5\xC0\xD9%d", strength), 0, 0, 0, 0, 0.5f, 200.0f);
			break;
		}
		break;
	case 3:
		if (special == 0) {
			g_audio->PlaySfx("swing_putting", 0, 0, 0, 0, 0.5f, 200.0f);
		} else if (special == 1) {
			g_audio->PlaySfx("harisen_putt", 0, 0, 0, 0, 0.5f, 200.0f);
		}
		break;
	}
}

// golfplayer.cpp:3752
void CGolfPlayer::SetPendingTexPart(int player, char *texName) {
	std::list<sTexturePart *>::iterator it;

	it = std::find_if(m_pendingTexParts.begin(), m_pendingTexParts.end(), sFindTexPart(texName));

	if (it != m_pendingTexParts.end()) {
		if (*it != m_pPetInfos[player].m_pTexPart) {
			m_pPetInfos[player].m_pTexPart = *it;
		}
		return;
	}

	sTexturePart *pTexPart = new sTexturePart;

	strcpy(pTexPart->m_texName, texName);
	m_pendingTexParts.push_back(pTexPart);

	m_pPetInfos[player].m_pTexPart = pTexPart;
}

// golfplayer.cpp:3778
void CGolfPlayer::SearchGround(void) {
	WVector dir;
	WVector ground;
	WVector nextGround;

	dir = m_ballDist * RotMat(m_unk140, 1).xa;
	float t = 0.0f;

	do {
		ground = GetPVS().GetGroundPoint(m_position + dir * t, true, NULL, false);
		nextGround = GetPVS().GetGroundPoint(m_position + dir * (t + 0.1f), true, NULL, false);

		if ((WVectorLen(ground - m_position) - m_ballDist) * (WVectorLen(nextGround - m_position) - m_ballDist) <= 0.0f) {
			break;
		}

		t += 0.1f;
	} while (t < 1.0f);

	float end = t + 0.1f;

	for (float s = t; s < end; s += 0.01f) {
		ground = GetPVS().GetGroundPoint(m_position + dir * s, true, NULL, false);
		nextGround = GetPVS().GetGroundPoint(m_position + dir * (s + 0.01f), true, NULL, false);

		if ((WVectorLen(ground - m_position) - m_ballDist) * (WVectorLen(nextGround - m_position) - m_ballDist) <= 0.0f) {
			m_displayMatrix.xa = nextGround - m_position;
			m_displayMatrix.xa.Normalize();

			m_displayMatrix.ya = WCrossProduct(m_displayMatrix.za, m_displayMatrix.xa);

			if (GOLFDOC()->m_approachMode) {
				m_standBlocked = false;
			} else if (Abs(m_displayMatrix.xa.y) > sin(20.0f * g_DEGTORAD)) {
				m_standBlocked = true;
			} else {
				m_standBlocked = GetPVS().OverlappedWithObject(m_position + m_clubs[GOLFDOC()->m_myPlayerIndex][GolfClub().GetType()].m_ballPos * m_displayMatrix, 1.6f);
			}

			SendMsg(this, "GolfRule", 7, m_standBlocked == false, 0, 0, 0);
			SendMsg(this, "GolfRule", 0x77, m_standBlocked == false, 0, 0, 0);
			SendMsg(this, "GolfRule", 7, m_standBlocked == false, 0, 0, 0);
			m_footAngle = asin(m_displayMatrix.xa.y) * -1.0f;
			m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C |= 0xB;
			return;
		}
	}

	m_displayMatrix = RotMat(m_unk140, 1);
	m_standBlocked = true;
	SendMsg(this, "GolfRule", 0x77, 0, 0, 0, 0);
	m_footAngle = 0.0f;

	m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk5C |= 0xB;
}

// golfplayer.cpp:3860
bool CGolfPlayer::SearchGround(const WVector &pos, float angle) {
	int objId;
	WVector ground;
	WVector nextGround;
	WVector dir;

	dir = m_ballDist * RotMat(angle, 1).xa;
	float t = 0.0f;

	do {
		ground = GetPVS().GetGroundPoint(pos + dir * t, true, NULL, false);
		nextGround = GetPVS().GetGroundPoint(pos + dir * (t + 0.1f), true, &objId, false);

		if (objId < 0) {
			return false;
		}

		if ((WVectorLen(ground - pos) - m_ballDist) * (WVectorLen(nextGround - pos) - m_ballDist) <= 0.0f) {
			break;
		}

		t += 0.1f;
	} while (t < 1.0f);

	float end = t + 0.1f;

	for (float s = t; s < end; s += 0.01f) {
		ground = GetPVS().GetGroundPoint(pos + dir * s, true, NULL, false);
		nextGround = GetPVS().GetGroundPoint(pos + dir * (s + 0.01f), true, NULL, false);

		if ((WVectorLen(ground - pos) - m_ballDist) * (WVectorLen(nextGround - pos) - m_ballDist) <= 0.0f) {
			WMatrix mat = RotMat(m_unk140, 1);

			mat.xa = nextGround - pos;
			mat.xa.Normalize();

			mat.ya = WCrossProduct(mat.za, mat.xa);

			const WVector &lastPos = GolfBall().PreData(GolfBall().m_preDataCount)->m_pos; WVector boxMin(lastPos.x - 16.0f, lastPos.y, lastPos.z - 16.0f);
			const WVector &lastPos2 = GolfBall().PreData(GolfBall().m_preDataCount)->m_pos; WVector boxMax(lastPos2.x + 16.0f, lastPos2.y, lastPos2.z + 16.0f);
			GetPVS().GetObjList(Waabb(boxMin, boxMax));

			if (!(Abs(mat.xa.y) > sin(20.0f * g_DEGTORAD))
				&& !GetPVS().OverlappedWithObject(pos + m_clubs[GOLFDOC()->m_myPlayerIndex][GolfClub().GetType()].m_ballPos * mat, 1.6f)) {
				return true;
			}

			return false;
		}
	}

	return false;
}

// golfplayer.cpp:3925
unsigned char CGolfPlayer::HasStandGround(const WVector &pos) {
	float angle = 0.0f; unsigned char count = 0;

	for (int i = 0; i < 8; ++i, angle += 3.14159265f / 4.0f) {
		if (SearchGround(pos - WVector(0.0f, GolfBall().GetRadius(), 0.0f), angle)) {
			++count;
		}
	}
	return count;
}

// golfplayer.cpp:3939
void CGolfPlayer::SetLight(void) {
	WBone *pHead = m_pPetFrames[GOLFDOC()->m_myPlayerIndex].GetBonePet()->FindBoneByTail("Head");
	m_lightSet.diffuse = ModifyColor(g_lightset.diffuse, GetPVS().GetShadowIntensity(pHead->GetMatrix().pivot));
}

// golfplayer.cpp:3945
void CGolfPlayer::SetLight(int index) {
	if (index < 0 || index >= 4) {
		return;
	}

	WBone *pHead = m_pPetFrames[index].GetBonePet()->FindBoneByTail("Head");

	if (pHead) {
		m_lightSet.diffuse = ModifyColor(g_lightset.diffuse, GetPVS().GetShadowIntensity(pHead->GetMatrix().pivot));
	}
}

// golfplayer.cpp:3958
void CGolfPlayer::ResetComboState(void) {
	m_comboRunning = false;
	m_comboTime = 0;
}

// golfplayer.cpp:3965
void CGolfPlayer::StartComboState(void) {
	m_comboRunning = true;
	m_comboTime = 0;
}

// golfplayer.cpp:3972
void CGolfPlayer::EndComboState(void) {
	m_comboCount = (int)(m_comboTime * (1.0f / 33.0f));
	m_comboRunning = false;
	m_comboTime = 0;
}

// golfplayer.cpp:3979
bool CGolfPlayer::Decide_PowerShot(void) {
	if ((m_comboCount == 1 && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_comboGauge >= 33.0f) ||
		(m_comboCount == 2 && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_comboGauge >= 66.0f)) {
		return true;
	}

	if ((m_comboCount == 1 && WSingleton<CSharedDoc>::Instance()->m_roomType == 1 && TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_teamRate >= 33.0f) ||
		(m_comboCount == 2 && WSingleton<CSharedDoc>::Instance()->m_roomType == 1 && TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_teamRate >= 66.0f)) {
		return true;
	}

	if (m_comboCount == 1 && (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000004 ||
		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000010 ||
		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000022)) {
		return true;
	}

	if (m_comboCount == 2 && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000027) {
		return true;
	}

	return false;
}

// golfplayer.cpp:4012
void CGolfPlayer::Shot_PowerShot(void) {
	if ((GOLFDOC()->GameMode() & 0x208) && WSingleton<CSharedDoc>::Instance()->m_playingMode == 1) {

		if (m_unk329 == true && m_comboCount == 2 && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId != 0x18000027) {

			if (CHelpMsg::Instance()) {

				g_audio->PlaySfx("play_unplayable", 0, 0, 0, 0, 0.5f, 200.0f);
				CHelpMsg::Instance()->SetHelpMsg(0x1A, 0);
			}
			return;
		}

		if (Decide_PowerShot()) {

			if (!m_unk190) {

				if (m_swingTypeName[0]) {

					if (m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk60 == 3) {

						g_audio->StopSfx("powershot_ready");
						m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk60 = 0;
						SetMotion(MakeStr("%s\xC1\xD8\xBA\xF1", m_swingTypeName), 0.0f, -1);

						int screenParam = 0;
						SendMsg(this, "Screen", 0x1AA, (intptr_t)&screenParam, 0, 0, 0);
					} else {
						g_audio->PlaySfx("powershot_ready", 0, 0, "powershot_ready", 0, 0.5f, 200.0f);
						m_pPetInfos[GOLFDOC()->m_myPlayerIndex].m_unk60 = 3;

						if (IsLocalContent((localContentType_t)0x31)) {

							if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000027) {

								GolfClub().SetPowerShot(3);
							} else {
								GolfClub().SetPowerShot(m_comboCount);
							}
						} else {
							GolfClub().SetPowerShot(m_comboCount);
						}

						char *motion;
						unsigned char type = GolfClub().PowerShotType();
						if (type == 2 || type == 3) {
							motion = MakeStr("%s\xC6\xC4\xBF\xF6" "2\xC1\xD8\xBA\xF1", m_swingTypeName);
						} else {
							motion = MakeStr("%s\xC6\xC4\xBF\xF6\xC1\xD8\xBA\xF1", m_swingTypeName);
						}
						SetMotion(motion, 0.0f, -1);

						if (WSingleton<CSharedDoc>::Instance()->m_roomType == 3) {

							GolfClub().m_calcRandomPower = false;
						}

						IObject *pSpike;
						IObject *pCobra;
						IObject *pTomahawk;
						IObject *pNormal;
						SendMsg(this, "GolfRule", 0x24A, (intptr_t)"predict_normal", (intptr_t)&pNormal, 0, 0);
						SendMsg(this, "GolfRule", 0x24A, (intptr_t)"predict_tomahwak", (intptr_t)&pTomahawk, 0, 0);
						SendMsg(this, "GolfRule", 0x24A, (intptr_t)"predict_cobra", (intptr_t)&pCobra, 0, 0);
						SendMsg(this, "GolfRule", 0x24A, (intptr_t)"predict_spike", (intptr_t)&pSpike, 0, 0);

						FrButton *pNormalButton = DYNAMIC_CAST(FrButton, pNormal);
						FrButton *pTomahawkButton = DYNAMIC_CAST(FrButton, pTomahawk);
						FrButton *pCobraButton = DYNAMIC_CAST(FrButton, pCobra);
						FrButton *pSpikeButton = DYNAMIC_CAST(FrButton, pSpike);

						if (pNormalButton) pNormalButton->SetStatus(FrButton::NORMAL);
						if (pTomahawkButton) pTomahawkButton->SetStatus(FrButton::PRESSED);
						if (pCobraButton) pCobraButton->SetStatus(FrButton::NORMAL);
						if (pSpikeButton) pSpikeButton->SetStatus(FrButton::NORMAL);

						SendMsg(this, "ShotPreview", -1, (intptr_t)"CHANGE_PREDICT_TYPE", 1, 0, 0);
					}
				}
			}
		} else if (CHelpMsg::Instance()) {

			g_audio->PlaySfx("play_unplayable", 0, 0, 0, 0, 0.5f, 200.0f);
			CHelpMsg::Instance()->SetHelpMsg(1, 0);
		}
	}
}

// golfplayer.cpp:4119
void CGolfPlayer::Cancel_PowerShot(void) {
	if (GolfClub().PowerShotType()) {
		g_audio->StopSfx("powershot_ready");

		g_audio->PlaySfx("powershot_cancel", 0, 0, 0, 0, 0.5f, 200.0f);

		int screenParam = 0;

		GolfClub().SetPowerShot(0);
		SendMsg(this, "Spot", 0xF5, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1AA, (intptr_t)&screenParam, 0, 0, 0);

		SetSwingType();

		if (OnlinePlay()) {
			WSendPacket packet(GCT_POWER_SHOT);
			packet.Encode1(0);
			packet.Send(SEND_TO_DEFAULT);
		}
	}
}

// golfplayer.cpp:4153
void CGolfPlayer::PowerShotInput(void) {
	if (IsMyTurn(false) && !WSingleton<CQuitWindow>::Instance()->IsActive() && CItemWindow::IsClosed()) {
		if (m_powerShotKeyState == 0) {
			if (g_input->GetUp("LALT") || g_input->GetUp("RALT")) {
				if (m_powerShotOn == true) {
					Cancel_PowerShot();
					m_powerShotOn = false;
				} else if (IsLocalContent((localContentType_t)0x31) && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000027) {
					m_powerShotKeyState = 2;
					m_powerShotKeyTime = timeGetTime();
				} else {
					m_powerShotKeyState = 1;
					m_powerShotKeyTime = timeGetTime();
				}
			}
		} else if (m_powerShotKeyState == 1) {
			if (g_input->GetDown("LALT", true) || g_input->GetDown("RALT", true)) {
				if (timeGetTime() < m_powerShotKeyTime + 500) {
					m_powerShotKeyState = 2;
				} else if (m_powerShotOn == true) {
					Cancel_PowerShot();
					m_powerShotOn = false;
					m_powerShotKeyState = 0;
				}
			}
		} else if (m_powerShotKeyState == 2) {
			if (g_input->GetUp("LALT") || g_input->GetUp("RALT")) {
				m_comboCount = 2;
				Shot_PowerShot();
				m_powerShotOn = true;
				m_powerShotKeyState = 0;
			}
		}
	}

	if (m_powerShotKeyState == 1 && !m_powerShotOn && timeGetTime() > m_powerShotKeyTime + 500) {
		m_comboCount = 1;
		Shot_PowerShot();
		m_powerShotOn = true;
		m_powerShotKeyState = 0;
	}
}

// golfplayer.cpp:4256
bool CGolfPlayer::IsAnimFxRunning(int slot, unsigned long fxId) {
	if (fxId == 0 || slot > 4) {
		return false;
	}

	std::vector<unsigned long>::iterator it;
	for (it = m_animFx[slot].begin(); it != m_animFx[slot].end(); ++it) {
		if (*it == fxId) {
			return true;
		}
	}
	return false;
}

// golfplayer.cpp:4273
void CGolfPlayer::RefreshAllUccClothes(void) {
	sUccRefreshItem item;

	for (int i = 0; i < 10; i++) {
		if (!UccManager()->GetRefreshItem(item)) {
			return;
		}

		for (int player = 0; player < GOLFDOC()->GetPlayerNum(); player++) {
			CPartTidList &tidList = GOLFDOC()->GetPartTidList(player);

			unsigned char part = (unsigned char)((item.m_typeId >> 13) & 0x1F);
			if (tidList.m_partTids[part] == item.m_typeId) {
				unsigned long serial = tidList.m_partIds[part];

				sUccClothes *pClothes = UccManager()->FindClothes(serial);
				if (pClothes && !_strcmpi(pClothes->m_uccKey, item.m_uccKey)) {
					m_pPetFrames[player].SetPart(&tidList, pClothes->m_typeId, pClothes->m_serial);
					m_pPetFrames[player].SetPart(&tidList, pClothes->m_typeId, pClothes->m_serial);
					m_pPetFrames[player].GetBonePet()->xUpdateTnLBuffers(true);
				}
			}
		}
	}
}

// golfplayer.cpp:4308
void CGolfPlayer::AddAbilityParts(unsigned long tid, unsigned long value) {
	m_abilityParts.push_back(std::pair<unsigned long, unsigned long>(tid, value));
}
