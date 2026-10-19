// Original: d:\build\custom\temp\code\client\projectg\golfdoc.cpp
#define GOLFDOC_INLINE
#define GOLFDOC_WMATH_INLINE
#include "NetworkSystem.h"
#include "GolfDoc.h"
#include "wmath.h"
#include "wutil.h"
#include "SharedDoc.h"
#include "golfball.h"
#include "wview.h"
#include "wscene.h"
#include "commonutil.h"
#include "scenemanager.h"
#include "polysoup.h"
#include "quadtree.h"
#include "sky.h"
#include "globalgamedefine.h"
#include "wresrcmng.h"
#include "westpak.h"
#include "tinyxml.h"
#include "Exception.h"
#include "fx.h"
#include "CapturedBg.h"
#include "autotarget.h"
#include "PangFBI.h"
#include "hackingmanager.h"
#include "cardsystem.h"
#include "club.h"
#include "InputManager.h"

#include <math.h>
#include <string.h>
#include <stdio.h>

extern LightSet g_lightset;
extern char g_executeDirectory[];
extern WMatrix g_camera;

// The original's max compiles as `a < b ? b : a` (wutil's Max uses >=);
// its header is unknown, as for camerawork.cpp's MaxOf.
template <class T> T MaxOf(T a, T b) { return a < b ? b : a; }

void __cdecl CloseCFile(cFile *file);
// itemmanager.cpp:5935; no header declares it yet (PlayerDlg.cpp declares it locally too).
int CalcPowerPenalty(int power);

// golfdoc.cpp:24 (run-time type and factory registration of CGolfDoc)
const WRTTI CGolfDoc::m_RTTI("CGolfDoc", &IObject::m_RTTI);

IObject *CGolfDocMakeInstance(void) {
	return new CGolfDoc;
}

struct __sCGolfDoc {
	__sCGolfDoc(void) {
		ObjectFactory().AddObjectFunctor(CGolfDocMakeInstance, "CGolfDoc");
	}
};
static __sCGolfDoc s_registerCGolfDoc;

// golfdoc.cpp:27
std::stack<CGolfDoc *, std::list<CGolfDoc *> > CGolfDoc::m_GolfDocInstanceStack;

// golfdoc.cpp:33
CGolfDoc::CGolfDoc(void) : m_pPolySoup(NULL), m_pSky(NULL), m_pAutoTarget(NULL), m_ownedStrings(8, 8), m_players(NULL), m_teams(NULL), m_holeData(NULL), m_gameMode(0), m_netGameEnd(false), m_teeShotReady(false) {
	WSingleton<CSharedDoc>::Instance()->m_pGolfDoc = this;

	m_GolfDocInstanceStack.push(this);

	m_gameState = 15;
	m_currentPlayerNum = 0;
	m_playerNum = 0;
	m_clockHold = 0;
	m_myPlayerIndex = 0;
	m_uiAlpha = 0xFF;
	m_cameraData.m_groundOffset = 0.5f;
	m_cameraData.m_yaw2 = 0.34906584f;
	m_fogBrightness = 1.0f;
	m_chatHistoryIndex = -1;

	m_selectedIndexA = -1;
	m_selectedIndexB = -1;

	m_pause = false;
	m_godMode = false;
	m_debugBar = 0;
	m_report = false;
	m_reportSent = false;
	m_dropPending = false;
	m_maxGrade = 0;
	m_clockStopped = false;
	m_clearedFlag158 = false;
	m_approachMode = false;
	m_showChat = true;
	m_setFlag159 = true;
	m_clearedFlag15A = false;
	m_impactAccuracy = 1.0f;
	m_controlAccuracy = 15.0f;
	m_day = true;
	m_clearedShort194 = 0;
	m_maxPangPerLoser = 0;
	m_totalPang = 0;
	m_skinsPangTotal = 0;
	m_winnerPangBonus = 0;
	m_skinsCarryCount = 0;
	m_clearedFlag1AC = false;
	MakeHoleData();

	CStackableSingleton<CSceneManager>::PushInstance();
	CStackableSingleton<CCapturedBg>::PushInstance();

	new CSceneManager;
	new CCapturedBg;

	if (WSingleton<CFx>::IsInstantiated())
		WSingleton<CSceneManager>::Instance()->RegisterElement((eObjType)6, NULL, WSingleton<CFx>::Instance(), NULL, 0, NULL);

	m_pPolySoup = new WPolySoup(this);
	m_pPolySoup->SetResourceManager(g_resrcmng);

	memset(m_roundResults, 0, sizeof(m_roundResults));
	memset(m_shotResultMsg, 0, sizeof(m_shotResultMsg));
	memset(&m_shotProtocol, 0, sizeof(m_shotProtocol));
	memset(m_zeroedBytes46E, 0, sizeof(m_zeroedBytes46E));

	m_statUpdated = true;

	m_approachReloaded = false;

	m_approachMissionType = (eApproachMissionType)0;
	m_approachPrizeType = (eApproachPrizeType)0;
	m_approachWinnerType = (eApproachWinnerType)0;

	m_approachCurHoleResult.clear();
	m_approachGameResult.clear();
	m_approachTotalPrize = 0;
}

// golfdoc.cpp:125
CGolfDoc::~CGolfDoc(void) {
	ClearApproachCurHoleResult();
	ClearApproachGameResult();

	SendReport();

	m_GolfDocInstanceStack.pop();
	if (m_GolfDocInstanceStack.empty()) {
		WSingleton<CSharedDoc>::Instance()->m_pGolfDoc = NULL;
	} else {
		WSingleton<CSharedDoc>::Instance()->m_pGolfDoc = m_GolfDocInstanceStack.top();
	}

	if (m_pPolySoup) { delete m_pPolySoup; m_pPolySoup = NULL; }
	if (m_pSky) { delete m_pSky; m_pSky = NULL; }
	if (m_pAutoTarget) { delete m_pAutoTarget; m_pAutoTarget = NULL; }

	if (m_players) { delete[] m_players; m_players = NULL; }
	if (m_teams) { delete[] m_teams; m_teams = NULL; }
	if (m_holeData) { delete[] m_holeData; m_holeData = NULL; }

	for (char *str = m_ownedStrings.Start(); str; str = m_ownedStrings.Next()) { if (str) delete[] str; } m_ownedStrings.Reset();

	if (WSingleton<CCapturedBg>::IsInstantiated()) delete WSingleton<CCapturedBg>::Instance();
	if (WSingleton<CSceneManager>::Instance()) delete WSingleton<CSceneManager>::Instance();

	CStackableSingleton<CCapturedBg>::PopInstance();
	CStackableSingleton<CSceneManager>::PopInstance();

	if (WSingleton<CTaskManager>::Instance()->GetPreservedTask() == NULL)
		g_resrcmng->ReleaseAllPets();
}


// golfdoc.h:160
sProperty::sProperty(void) {
	m_name[0] = 0;
	m_desc[0] = 0;
}

// golfdoc.h:165
sProperty::~sProperty(void) {
}

// golfdoc.h (implicit constructor, no line record)
sCameraData::sCameraData() {
}

// golfdoc.h (implicit constructor, no line record)
sHoleData::sHoleData() {
}

// golfdoc.h:488
CGolfDoc::sTarget::sTarget(void) : m_cameraTarget(0), m_unk10(false) {
}

// shareddoc.inl:109, the local player's turn: the current player's UID is
// ours (game mode 5 or online play); a gallery (identity 2) gets the caller's
// default.
bool IsMyTurn(bool galleryTurn) {
	if (WSingleton<CSharedDoc>::Instance()->m_gameMode) {
		if (WSingleton<CSharedDoc>::Instance()->m_gameMode == 5)
			return GOLFDOC()->GetPlayer(GOLFDOC()->m_myPlayerIndex)->m_uid == WSingleton<CSharedDoc>::Instance()->m_userInfo.guid ? true : false;
	} else {
		if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
			if (WSingleton<CSharedDoc>::Instance()->m_userInfo.IsIdentity(2))
				return galleryTurn;

			return GOLFDOC()->GetPlayer(GOLFDOC()->m_myPlayerIndex)->m_uid == WSingleton<CSharedDoc>::Instance()->m_userInfo.guid;
		}
	}

	return true;
}

// shareddoc.inl:363
WVector GetTeeShotPos(void) {
	return GOLFDOC()->GetHoleData().m_teePos;
}

// shareddoc.inl:383
sCameraData &GameCamera(void) {
	return GOLFDOC()->GetCameraData();
}

// golfdoc.cpp:165
void CGolfDoc::UpdateBright(float bright) {
	if (WAbsBySign(m_fogBrightness - bright) < 1e-5f)
		return;

	m_fogBrightness = bright;

	g_lightset.diffuse = ModifyColor(g_lightset.diffuse, GOLFDOC()->m_fogBrightness);
	g_lightset.ambient = ModifyColor(g_lightset.ambient, GOLFDOC()->m_fogBrightness);
	g_lightset.ambient2 = ModifyColor(g_lightset.ambient2, GOLFDOC()->m_fogBrightness);

	if (WSingleton<CSceneManager>::Instance())
		WSingleton<CSceneManager>::Instance()->UpdateBright();

	if (m_pPolySoup)
		m_pPolySoup->UpdateBright();

	if (m_pSky)
		m_pSky->UpdateBright();
}

// golfdoc.cpp:186
void CGolfDoc::SendReport(void) {
	if (m_report) {
		char dir[MAX_PATH] = "";
		strcpy(dir, g_executeDirectory);

		char buf[0x400] = "";
		FILE *fp = fopen(MakeStr("%s\\chat_history.txt", dir), "wt");
		if (fp) {
			// "report time : "
			SYSTEMTIME time;
			GetLocalTime(&time);

			sprintf(buf, "\xbd\xc5\xb0\xed\xbd\xc3\xb0\xa3 : %d:%02d:%02d %02d:%02d:%02d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
			fwrite(buf, strlen(buf), 1, fp);

			sprintf(buf, "-------------------------------------------------------------\n");
			fwrite(buf, strlen(buf), 1, fp);

			// "id : nickname"
			sprintf(buf, "\xbe\xc6\xc0\xcc\xb5\xf0 : \xb4\xeb\xc8\xad\xb8\xed\n");
			fwrite(buf, strlen(buf), 1, fp);

			if (IsMassGame()) {
				std::vector<sGameResultInfo> &results = WSingleton<CSharedDoc>::Instance()->m_gameResultList;
				for (std::vector<sGameResultInfo>::iterator it = results.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
					sprintf(buf, "%s\n", it->m_nickname);
					fwrite(buf, strlen(buf), 1, fp);
				}
			} else {
				for (unsigned char i = 0; i < m_playerNum; i++) {
					sprintf(buf, "\t%s\n", WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_name);
					fwrite(buf, strlen(buf), 1, fp);
				}
			}

			sprintf(buf, "-------------------------------------------------------------\n");
			fwrite(buf, strlen(buf), 1, fp);

			for (std::list<sChatLine>::iterator line = WSingleton<CSharedDoc>::Instance()->m_chatLineList.begin(); line != WSingleton<CSharedDoc>::Instance()->m_chatLineList.end(); ++line) {
				fwrite(line->m_text.c_str(), strlen(line->m_text.c_str()), 1, fp);
				fwrite("\n", 1, 1, fp);
			}

			fclose(fp);

			_snprintf(buf, 0x3FF, "%s %d %s\\chat_history.txt", WSingleton<CSharedDoc>::Instance()->m_userInfo.nickname, 4, dir);

			WSingleton<CPangFBI>::Instance()->ProcessReport("crime_log", buf);
		}
	}
}

// The current hole's tee position by value: the original copies the vector
// into a temporary at each use (MakeTopViewCameraMatrix), as the cup's
// shareddoc.inl GetHoleCupPos() does. Its name and home header are not known
// (no line record, never emitted out of line).
static WVector TeePosOf(CGolfDoc *doc) { return doc->GetHoleData().m_teePos; }

// golfdoc.cpp:250
WMatrix CGolfDoc::MakeTopViewCameraMatrix(bool keepCamera) {
	WMatrix mat;

	mat.za = WVector::UNIT_NEG_Y;
	mat.ya = GOLFDOC()->m_cameraData.m_matrix.za;
	mat.ya.Normalize();
	mat.xa = WCrossProduct(mat.ya, mat.za);

	WVector toCup = GetHoleCupPos() - PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos;
	float dist = (float)sqrt(toCup.z * toCup.z + toCup.x * toCup.x);

	if (g_input->Get("LCONTROL", true) || g_input->Get("RCONTROL", true)) {
		// the whole hole: halfway between the tee and the cup, high enough to see both
		mat.pivot = (GetHoleCupPos() + TeePosOf(GOLFDOC())) * 0.5f;
		mat.pivot.y = Max(GetHoleCupPos().y, TeePosOf(GOLFDOC()).y)
			+ (Abs(GetHoleCupPos().z - TeePosOf(GOLFDOC()).z) + 480.0f) * 0.5f
			/ ((float)tan(g_view->GetFOV() * 0.5f) * g_view->GetRatio());
	} else if ((g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true) || g_input->Get(1)) && IsMyTurn(true)) {
		// over the club's landing point
		mat.pivot = (GolfClub().GetRange(0) * 3.2f) * GOLFDOC()->m_cameraData.m_matrix.za + GolfBall().m_pos;
		int found;
		mat.pivot.y = GetPVS().GetGroundPoint(mat.pivot, true, &found, true).y + 64.0f;

		if (found < 0) {
			mat.pivot.y = GolfBall().m_pos.y + 64.0f;
		}
	} else if (GolfBall().m_groundType == 2 && GolfClub().GetRange(0) * 3.2f > dist) {
		if (dist < 16.0f)
			mat.pivot = (GOLFDOC()->m_cameraData.m_matrix.za * 0.2f + WVector::UNIT_POS_Y) * 32.0f + GolfBall().m_pos;
		else
			mat.pivot = (GOLFDOC()->m_cameraData.m_matrix.za * 0.4f + WVector(0.0f, 2.0f, 0.0f)) * dist + GolfBall().m_pos;
	} else if (GolfClub().GetApproachMode() <= 3) {
		mat.pivot = (GOLFDOC()->m_cameraData.m_matrix.za * 0.5f + WVector(0.0f, 1.4f, 0.0f)) * Max(32.0f, dist) + GolfBall().m_pos;
	} else {
		mat.pivot = (GOLFDOC()->m_cameraData.m_matrix.za * 0.5f + WVector(0.0f, 1.4f, 0.0f)) * (GolfClub().GetRange(0) * 3.2f) + GolfBall().m_pos;
	}

	if (keepCamera) {
		if (mat.pivot.y < g_camera.pivot.y || (mat.pivot - g_camera.pivot).Magnitude() > 32.0f)
			return g_camera;
	}

	return mat;
}

// golfdoc.cpp:325
WMatrix CGolfDoc::GetDefaultCamera(void) {
	WMatrix camera;

	WVector eye = m_cameraData.m_matrix.pivot, at; at = m_cameraData.m_matrix.pivot + m_cameraData.m_matrix.za;

	at.y += tan(m_cameraData.m_pitch); camera = MakeCameraMatrix(eye, at);

	return camera;
}

// golfdoc.cpp:339
void CGolfDoc::MakeDefaultCamera(bool setCamera) {
	if (GolfBall().m_groundType == 2) {
		m_cameraData.m_distance = 15.0f;
		m_cameraData.m_height = 7.0f;
	} else {
		m_cameraData.m_distance = 20.0f;
		m_cameraData.m_height = 5.0f;
	}

	WVector dir;
	dir = RotMat(m_cameraData.m_yaw, 1).za;

	dir.y = 0.0f; dir.Normalize();

	if (IsMyTurn(true) || (IsLocalContent((localContentType_t)0x4F) && WSingleton<CSharedDoc>::Instance()->m_playingMode == 2)) {
		m_cameraData.m_target = GolfBall().m_pos;
	} else {
		m_cameraData.m_target = dir * 15.0f + GolfBall().m_pos;
		m_cameraData.m_distance += 20.0f;

		if (GolfBall().m_groundType == 2)
			m_cameraData.m_height += 3.0f;

		dir = dir * RotMat(m_cameraData.m_yaw2, 1);
	}

	IFF_STRUCT::sChar *pChar = ItemManager()->FindChar(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_characterInfo.typeId);
	if (pChar) {
		m_cameraData.m_distance *= pChar->m_cameraScale;
		m_cameraData.m_height *= pChar->m_cameraScale;
	}

	WVector at; at = dir + m_cameraData.m_target;

	m_cameraData.m_matrix = MakeCameraMatrix(m_cameraData.m_target, at);
	m_cameraData.m_matrix.pivot -= m_cameraData.m_matrix.za * m_cameraData.m_distance;
	m_cameraData.m_matrix.pivot.y = MaxOf(GetPVS().GetGroundPoint(m_cameraData.m_matrix.pivot, false, 0, false).y + m_cameraData.m_groundOffset, m_cameraData.m_target.y + m_cameraData.m_height);

	if (setCamera) {
		at.y += tan(m_cameraData.m_pitch);
		g_camera = MakeCameraMatrix(m_cameraData.m_target, at);
		g_camera.pivot = m_cameraData.m_matrix.pivot;
	}

	g_view->SetFOV(55.0f * g_DEGTORAD);
}

// golfdoc.cpp:397
sPlayerData *CGolfDoc::GetPlayer(unsigned char index) {
	if (m_players == NULL) {
		return NULL;
	}

	switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
		return m_players;
	}
	if (index >= m_playerNum)
		return NULL;

	return &m_players[index];
}

// golfdoc.cpp:414
sTeamData *CGolfDoc::GetTeam(unsigned char index) {
	if (m_teams == NULL) {
		return NULL;
	}

	return &m_teams[index];
}

// golfdoc.cpp:425
void CGolfDoc::MakeTeam(void) {
	m_teamGameWinner = (eTeamName)2;

	if (m_teams) { delete[] m_teams; m_teams = NULL; }
	m_teams = new sTeamData[2];
	memset(m_teams, 0, sizeof(sTeamData) * 2);
}

// golfdoc.cpp:435
void CGolfDoc::MakePlayer(unsigned char num) {
	m_currentPlayerNum = num;
	m_playerNum = num;

	if (m_players) { delete[] m_players; m_players = NULL; }

	m_players = new sPlayerData[m_playerNum];
	memset(m_players, 0, sizeof(sPlayerData) * m_playerNum);
}

// golfdoc.cpp:446 (qsort comparator: the players in score order of the current hole)
int __cdecl CompareRank(const void *a, const void *b) {
	sPlayerData *player1 = (sPlayerData *)a;
	sPlayerData *player2 = (sPlayerData *)b;

	unsigned char hole = GOLFDOC()->m_currentHole;

	if ((unsigned char)player1->m_holes[hole - 1] < (unsigned char)player2->m_holes[hole - 1])
		return 0;
	if (player1->m_holes[hole - 1] == player2->m_holes[hole - 1] && player1->m_state == 1 && player2->m_state != 1)
		return 0;

	return 1;
}

// golfdoc.cpp:461
void CGolfDoc::ResetTeamVars(void) {
	if (m_teams == NULL)
		return;

	for (int i = 0; i < 2; i++) {
		m_teams[i].m_color = (i ? 0xFF0060FF : 0xFFFF0000) & 0xFFFFFF;
		m_teams[i].m_state = 2;
		m_teams[i].m_teamRate = 0.0f;
		m_teams[i].m_totalScore = 0;
		m_teams[i].m_resetCount = 0;
		m_teams[i].m_teamValue = 0;
		m_teams[i].m_teamValue2 = 0;
		m_teams[i].m_clubHolder = 0;
		m_teams[i].m_clubHolderClub = 0;
		m_teams[i].m_nextTurnSlot = 0;
		m_teams[i].m_firstTurnSlot = 0;
		m_teams[i].m_holeInKind = 0;
		m_teams[i].m_holeInCount = 0;

		int hole;
		for (hole = 0; hole < 18; hole++) {
			m_teams[i].m_holes[hole] = 0;
			m_teams[i].m_holeIn[hole] = 0;
			m_teams[i].m_holeRecord[hole] = 0;
		}

		if (OnlinePlay() && WSingleton<HackingManager>::IsInstantiated()) {
			for (hole = 0; hole < 18; hole++) {
				WSingleton<HackingManager>::Instance()->AddCrypticByteValue(&m_teams[i].m_holes[hole]);
				WSingleton<HackingManager>::Instance()->AddCrypticByteValue(&m_teams[i].m_holeIn[hole]);
				WSingleton<HackingManager>::Instance()->AddCrypticIntValue(&m_teams[i].m_holeRecord[hole]);
			}

			WSingleton<HackingManager>::Instance()->AddCrypticIntValue(&m_teams[i].m_teamValue);
			WSingleton<HackingManager>::Instance()->AddCrypticIntValue(&m_teams[i].m_teamValue2);
			WSingleton<HackingManager>::Instance()->AddCrypticFloatValue(&m_teams[i].m_teamRate);
		}
	}
}

// golfdoc.cpp:510
void CGolfDoc::MakeHoleData(void) {
	if (m_holeData) { delete[] m_holeData; m_holeData = NULL; }
	m_holeData = new sHoleData[18];
	memset(m_holeData, 0, sizeof(sHoleData) * 18);
}

// golfdoc.cpp:517
void CGolfDoc::LoadXml(const char *filename, unsigned char index, bool reload) {
	TiXmlDocument doc;

	cFile *file = g_resrcmng->GetCFile(filename, 0xFFFF);
	if (file) {
		char *buffer = new char[file->Length() + 1];
		file->Read(buffer, file->Length());
		buffer[file->Length()] = 0;
		CloseCFile(file);

		if (doc.LoadFileFromMemoryEx(buffer)) {
			// a reload, or another course: read its properties again
			if (reload || m_xmlName.empty() || m_xmlName != filename)
				m_propertyInfo.Init(doc);
			m_mapInfo.Init(doc, index);
			m_xmlName = filename;

			delete[] buffer;
			return;
		}

		delete[] buffer;
	}

	// "the map's property file failed to load" (CP949)
	throw WAppException("\xc5\xd8\xbd\xba\xc3\xc4 \xbc\xd3\xbc\xba \xc6\xc4\xc0\xcf\xc0\xbb \xb7\xce\xb5\xf9 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9");
}

// golfdoc.cpp:550
void CGolfDoc::RefreshPlayerNum(void) {
	m_currentPlayerNum = 0;

	for (unsigned char i = 0; i < m_playerNum; i++) {
		if (m_players[i].m_state == 3)
			continue;

		m_currentPlayerNum++;
	}
}

// golfdoc.cpp:564
void CGolfDoc::SetCurrentPlayerNum(int num) {
	m_currentPlayerNum = num;
}

// golfdoc.cpp:570
unsigned char CGolfDoc::GetCurrentPlayerNum(void) {
	return m_currentPlayerNum;
}

// golfdoc.cpp:576
void CGolfDoc::SetPlayerLevel(unsigned char index) {
	sPlayerData *pPlayer = GetPlayer(index);

	unsigned long charTypeId = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_characterInfo.typeId;
	const char *charLevels = (const char *)&WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_characterInfo.unknown6A;
	const unsigned long *parts = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_characterInfo.partTypeIds;
	const unsigned long *cards = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_characterInfo.auxParts.tids;
	unsigned long clubTypeId = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_clubSet.typeId;
	const short *clubLevels = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_clubSet.statLevel;
	unsigned long caddieTypeId = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_caddieInfo.typeId;
	unsigned char level = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_level;

	if (WSingleton<CSharedDoc>::Instance()->m_offlinePlay) {
		pPlayer->m_power = rand() % 20 + 10;
		pPlayer->m_control = rand() % 10 + 15;
		pPlayer->m_accuracy = rand() % 5 + 5;
		pPlayer->m_spin = rand() % 5 + 5;
		pPlayer->m_curve = rand() % 5 + 5;
	} else {
		if (IsLocalContent((localContentType_t)0x51))
			WSingleton<CCardManager>::Instance()->SetPlayerIndex(index);

		pPlayer->m_power = ItemManager()->GetLevel((eLvlType)0, charTypeId, charLevels, parts, cards, clubTypeId, clubLevels, caddieTypeId, level);
		pPlayer->m_control = ItemManager()->GetLevel((eLvlType)1, charTypeId, charLevels, parts, cards, clubTypeId, clubLevels, caddieTypeId, level);
		pPlayer->m_accuracy = ItemManager()->GetLevel((eLvlType)2, charTypeId, charLevels, parts, cards, clubTypeId, clubLevels, caddieTypeId, level);
		pPlayer->m_spin = ItemManager()->GetLevel((eLvlType)3, charTypeId, charLevels, parts, cards, clubTypeId, clubLevels, caddieTypeId, level);
		pPlayer->m_curve = ItemManager()->GetLevel((eLvlType)4, charTypeId, charLevels, parts, cards, clubTypeId, clubLevels, caddieTypeId, level);
	}

	pPlayer->m_power = Between<int>(0, pPlayer->m_power, 100) - 15;

	pPlayer->m_control -= Max<int>(0, pPlayer->m_power - (CalcPowerPenalty(level) + 5));
	pPlayer->m_accuracy -= Max<int>(0, pPlayer->m_power - (CalcPowerPenalty(level) + 5));

	pPlayer->m_accuracy = Between<int>(0, pPlayer->m_accuracy, 30);
	pPlayer->m_control = Between<int>(0, pPlayer->m_control, 30);
	pPlayer->m_spin = Between<int>(1, pPlayer->m_spin, 30);
	pPlayer->m_curve = Between<int>(1, pPlayer->m_curve, 30);
}

// golfdoc.cpp:625
CPartTidList &CGolfDoc::GetPartTidList(unsigned char index) {
	return m_partTidList[index];
}

// golfdoc.cpp:632
void CGolfDoc::SetPartTidList(unsigned char index, unsigned long tidChar, unsigned char hairClr, unsigned char shirtsClr, unsigned long *partTids, unsigned long *partIds) {
	sCharacterInfo charInfo;

	charInfo.typeId = tidChar; charInfo.hairColor = hairClr; charInfo.shirtColor ^= (charInfo.shirtColor ^ shirtsClr) & 0xF; memcpy(charInfo.partTypeIds, partTids, sizeof(charInfo.partTypeIds));
	if (partIds) {
		memcpy(charInfo.partIds, partIds, sizeof(charInfo.partIds));
	}

	CPartTidList &list = m_partTidList[index];
	if (!list.SetTids(&charInfo, 0xFF)) {
		list.SetDefaultTids();
	}

	if (!list.IsComboValid()) {
		list.SetDefaultTids();
	}
}

// golfdoc.cpp:663
void CGolfDoc::ClearShotResultMsg(void) {
	memset(m_shotResultMsg, 0, sizeof(m_shotResultMsg));
}

// golfdoc.cpp:668
void CGolfDoc::InitShotResultMsg(void) {
	float radius = GolfBall().GetRadius(); WVector pos = GetHoleData().m_teePos + WVector(0.0f, radius, 0.0f);

	for (int i = 0; i < m_playerNum; i++) {
		m_shotResultMsg[i].m_pos.x = pos.x;
		m_shotResultMsg[i].m_pos.y = pos.y;
		m_shotResultMsg[i].m_pos.z = pos.z;
		m_shotResultMsg[i].m_powerShotLevel = 0;
	}
}

// golfdoc.cpp:681
void CGolfDoc::LogStrokeScore(void) {
}

// golfdoc.cpp:753
void CWaveInfo::Load(const char *name) {
	cFile *file = g_resrcmng->GetCFile(name, 0xFFFF);
	if (file == NULL)
		return;

	m_waves.clear();

	char buffer[0x400];
	int length = file->Length();
	file->Read(buffer, length);
	buffer[length] = 0;

	char *next = buffer;
	char *line = buffer;
	while (*line != 0) {
		while (*next != 0 && *next != '\n')
			next++;
		if (*next != 0)
			next++;

		if (line[0] == '/' && line[1] == '/') {
			line = next;
			continue;
		}

		int id;
		sWave wave;
		if (sscanf(line, "%d %s %f %f", &id, wave.m_texture, &wave.m_width, &wave.m_freq) == 4) {
			std::map<int, sWave>::iterator it = m_waves.find(id);
			if (it == m_waves.end()) {
				wave.m_freq *= 0.001f;
				m_waves[id] = wave;
			}
		}

		line = next;
	}

	CloseCFile(file);
}

// golfdoc.cpp:812
void CRiverInfo::Load(const char *name) {
	cFile *file = g_resrcmng->GetCFile(name, 0xFFFF);
	if (file == NULL)
		return;

	m_rivers.clear();

	char buffer[0x400];
	int length = file->Length();
	file->Read(buffer, length);
	buffer[length] = 0;

	char *next = buffer;
	char *line = buffer;
	while (*line != 0) {
		while (*next != 0 && *next != '\n')
			next++;
		if (*next != 0)
			next++;

		if (line[0] == '/' && line[1] == '/') {
			line = next;
			continue;
		}

		int id;
		sRiver river;
		if (sscanf(line, "%d %f %f %f %f", &id, &river.m_scrollVelocity, &river.m_headTile, &river.m_sideTile, &river.m_depth) == 5) {
			std::map<int, sRiver>::iterator it = m_rivers.find(id);
			if (it == m_rivers.end())
				m_rivers[id] = river;
		}

		line = next;
	}

	CloseCFile(file);
}

// golfdoc.cpp:867
void CMapInfo::Init(const TiXmlDocument &doc, unsigned char index) {
	TiXmlNode *project = doc.FirstChild("round")->FirstChild("project");

	if (project == NULL)
		return;

	m_items.clear();
	TiXmlNode *wep = project->FirstChild("wep");
	for (unsigned char i = 0; i < index; i++)
		wep = wep->NextSibling();

	std::string name = wep->ToElement()->Attribute("name");
	TiXmlNode *item = wep->FirstChild();
	while (item) {
		std::string key = item->ToElement()->Value();
		std::string value = item->FirstChild()->Value();
		m_items[key] = value;
		item = item->NextSibling();
	}
}

// golfdoc.cpp:892
const char *CMapInfo::FindItem(const std::string &name) {
	if (m_items.find(name) == m_items.end())
		return NULL;
	return m_items[name].c_str();
}

// golfdoc.cpp:904
CPropertyInfo::~CPropertyInfo() {
}

// golfdoc.cpp:922
void CPropertyInfo::Init(const TiXmlDocument &doc) {
	// One value of a <class> element: its child element's name, the sscanf
	// format and where the value goes.
	struct sPropertyField {
		const char *name;
		const char *format;
		void *dest;
	};

	TiXmlNode *round = doc.FirstChild("round");
	if (round == NULL)
		throw WAppException(std::string("app exception"));

	TiXmlNode *declare = round->FirstChild("declare");
	if (declare == NULL)
		return;

	m_properties.clear();
	m_indexByName.clear();

	int count = 0;
	declare->ToElement()->Attribute("count", &count);
	if (count) {
		m_properties.reserve(count);
	}

	m_defaultIndex = -1;
	int index = 0;

	sProperty prop;
	char typeName[128];

	sPropertyField fields[12] = {
		{ "bound", "%f", &prop.m_bounceCoeff },
		{ "roll", "%f", &prop.m_rollCoeff },
		{ "min", "%f", &prop.m_minPowerOnGround[0] },
		{ "max", "%f", &prop.m_maxPowerOnGround[0] },
		{ "min2", "%f", &prop.m_minPowerOnGround[1] },
		{ "max2", "%f", &prop.m_maxPowerOnGround[1] },
		{ "min3", "%f", &prop.m_minPowerOnGround[2] },
		{ "max3", "%f", &prop.m_maxPowerOnGround[2] },
		// an int written over the flag byte at 0x20 (m_penetrable, m_outOfBounds)
		{ "pass", "%d", (char *)&prop + 0x20 },
		{ "pet", "%s", typeName },
		{ "bound_sound", "%s", prop.m_name },
		{ "roll_sound", "%s", prop.m_desc },
	};

	// the "default" class supplies the values a class leaves out
	TiXmlNode *defaultClass = declare->FirstChild("class");
	while (defaultClass) {
		m_defaultIndex++;
		std::string name = defaultClass->ToElement()->Attribute("name");
		if (name == "default")
			break;

		defaultClass = defaultClass->NextSibling("class");
	}

	TiXmlNode *cls = declare->FirstChild("class");
	do {
		TiXmlElement *element = cls->ToElement();
		if (stricmp(element->Value(), "class") != 0)
			return;

		std::string name = element->Attribute("name");
		if (name.empty())
			break;

		memset(typeName, 0, sizeof(typeName));

		for (int i = 0; i < 12; i++) {
			TiXmlNode *node = cls->FirstChild(fields[i].name);
			if (node == NULL) {
				// not given: the default class's value
				node = defaultClass->FirstChild(fields[i].name);
			}

			std::string value = node->ToElement()->FirstChild()->Value();
			sscanf(value.c_str(), fields[i].format, fields[i].dest);
		}

		if (strstr(typeName, "green"))
			prop.m_weatherBounceType = 2;
		else if (strstr(typeName, "fairway"))
			prop.m_weatherBounceType = 1;
		else if (strstr(typeName, "bunker"))
			prop.m_weatherBounceType = 3;
		else if (strstr(typeName, "rough"))
			prop.m_weatherBounceType = 4;
		else if (strstr(typeName, "snow"))
			prop.m_weatherBounceType = 5;
		else if (strstr(typeName, "road"))
			prop.m_weatherBounceType = 6;
		else if (strstr(typeName, "ice"))
			prop.m_weatherBounceType = 7;
		else if (strstr(typeName, "water"))
			prop.m_weatherBounceType = 9;
		else if (strstr(typeName, "sand"))
			prop.m_weatherBounceType = 11;
		else if (strstr(typeName, "booster"))
			prop.m_weatherBounceType = 13;
		else if (strstr(typeName, "special"))
			prop.m_weatherBounceType = 12;
		else if (strstr(typeName, "wb"))
			prop.m_weatherBounceType = 14;
		else if (strstr(typeName, "vector"))
			prop.m_weatherBounceType = 8;

		// the sounds are named without their extension
		char *extension = strstr(prop.m_name, ".wav");
		if (extension)
			*extension = 0;

		extension = strstr(prop.m_desc, ".wav");
		if (extension)
			*extension = 0;

		unsigned char ground = prop.m_weatherBounceType;
		int type = ground;
		if (ground == 0 || ground == 4) {
			if (GetCurMap() == 10)
				type = 11;
			else
				type = IsCurMapSnow() ? 5 : 2;
		}

		// the footstep sound by the ground ("\xb9\xdf\xc0\xda\xb1\xb9" = "footstep", CP949)
		switch (type) {
		case 5:
			strcpy(prop.m_stepSound, "\xb9\xdf\xc0\xda\xb1\xb9_snow");
			break;
		case 7:
			strcpy(prop.m_stepSound, "\xb9\xdf\xc0\xda\xb1\xb9_ice");
			break;
		case 3:
		case 11:
			strcpy(prop.m_stepSound, "\xb9\xdf\xc0\xda\xb1\xb9_sand");
			break;
		case 9:
			strcpy(prop.m_stepSound, "\xb9\xdf\xc0\xda\xb1\xb9_water");
			break;
		default:
			strcpy(prop.m_stepSound, "\xb9\xdf\xc0\xda\xb1\xb9_green");
			break;
		}

		m_properties.push_back(prop);

		TiXmlNode *textures = cls->FirstChild("textures");
		if (textures) {
			TiXmlNode *item = textures->FirstChild("item");
			while (item) {
				std::string texture = item->ToElement()->FirstChild()->Value();
				item = item->NextSibling();

				m_indexByName[texture] = index;
			}
		}

		index++;

		cls = cls->NextSibling("class");
	} while (cls);
}

// golfdoc.cpp:1122
int CPropertyInfo::GetPropertyIndex(const char *name) {
	std::map<std::string, int>::iterator it = m_indexByName.find(name);
	if (it == m_indexByName.end()) {
		// unknown name

		return m_defaultIndex;
	}

	return m_indexByName[name];
}

// golfdoc.cpp:1134
const sProperty &CPropertyInfo::GetProperty(int index) {
	if (index < 0 || index >= m_properties.size())
		index = m_defaultIndex;

	return m_properties[index];
}

// golfdoc.cpp:1146
void sBar::Reset(void) {
	m_accuracy = 105.0f;
	m_prevAccuracy = 105.0f;
	m_position = m_power = m_startPosition = BAR_START; m_stopped = false;
	m_state = 0;
	m_impact = BAR_START;
}

// golfdoc.cpp:1159
void CGolfDoc::SetApproachMissionDesc(const char *desc) {
	strcpy(m_approachMissionDesc, desc);
}

// golfdoc.cpp:1164
char *CGolfDoc::GetApproachMissionDesc(void) {
	return m_approachMissionDesc;
}

// golfdoc.cpp:1171
void CGolfDoc::SetApproachMissionType(eApproachMissionType type) {
	m_approachMissionType = type;
}

// golfdoc.cpp:1178
void CGolfDoc::SetApproachPrizeType(eApproachPrizeType type) {
	m_approachPrizeType = type;
}

// golfdoc.cpp:1185
void CGolfDoc::SetApproachWinnerType(eApproachWinnerType type) {
	m_approachWinnerType = type;
}

// golfdoc.cpp:1192
eApproachMissionType CGolfDoc::GetApproachMissionType(void) const {
	return m_approachMissionType;
}

// golfdoc.cpp:1199
eApproachPrizeType CGolfDoc::GetApproachPrizeType(void) const {
	return m_approachPrizeType;
}

// golfdoc.cpp:1206
eApproachWinnerType CGolfDoc::GetApproachWinnerType(void) const {
	return m_approachWinnerType;
}

// golfdoc.cpp:1213
void CGolfDoc::SetApproachCurHoleResult(sApproachResultData &data) {
	sApproachResultData *result = new sApproachResultData;

	if (result) {
		*result = data;
		m_approachCurHoleResult.push_back(result);
	}
}

// golfdoc.cpp:1226
void CGolfDoc::SetApproachGameResult(sApproachResultData &data) {
	sApproachResultData *result = new sApproachResultData;
	memset(result, 0, sizeof(sApproachResultData));

	if (result) {
		*result = data;

		std::list<sApproachResultData *>::iterator it;
		for (it = m_approachGameResult.begin(); it != m_approachGameResult.end(); ++it) {
			if ((*it)->m_uid == data.m_uid)
				break;
		}
		if (it == m_approachGameResult.end()) {
			m_approachGameResult.push_back(result);
		}
		else {
			delete result;

			sApproachResultData *found = *it;
			found->m_unk09 = data.m_unk09;
			found->m_uid = data.m_uid;
			found->m_unk16 += data.m_unk16;
			found->m_unk17 += data.m_unk17;
			found->m_unk0A += data.m_unk0A;
			found->m_unk0E += data.m_unk0E;
			found->m_unk12 += data.m_unk12;
		}
	}
}

// golfdoc.cpp:1270
void CGolfDoc::ClearApproachCurHoleResult(void) {
	std::list<sApproachResultData *>::iterator it;

	for (it = m_approachCurHoleResult.begin(); it != m_approachCurHoleResult.end(); ++it) {
		if (*it) {
			delete *it;
			*it = NULL;
		}
	}

	m_approachCurHoleResult.clear();
}

// golfdoc.cpp:1287
void CGolfDoc::ClearApproachGameResult(void) {
	std::list<sApproachResultData *>::iterator it;

	for (it = m_approachGameResult.begin(); it != m_approachGameResult.end(); ++it) {
		if (*it) {
			delete *it;
			*it = NULL;
		}
	}

	m_approachGameResult.clear();
}

// golfdoc.cpp:1304
std::list<sApproachResultData *> &CGolfDoc::GetApproachCurHoleResult(void) {
	return m_approachCurHoleResult;
}

// golfdoc.cpp:1310
sApproachResultData *CGolfDoc::GetAppMyCurHoleResult(void) {
	sApproachResultData *result = NULL;
	std::list<sApproachResultData *>::iterator it;
	for (it = m_approachCurHoleResult.begin(); it != m_approachCurHoleResult.end(); ++it) {
		sApproachResultData *data = *it;
		if (data != NULL) {
			if (data->m_uid == WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().guid) {
				result = data;
				break;
			}

			result = NULL;
		}
	}

	return result;
}

// golfdoc.cpp:1339
std::list<sApproachResultData *> &CGolfDoc::GetApproachGameResult(void) {
	return m_approachGameResult;
}

// golfdoc.cpp:1345
sApproachResultData *CGolfDoc::GetAppMyGameResult(void) {
	sApproachResultData *result = NULL;
	std::list<sApproachResultData *>::iterator it;
	for (it = m_approachGameResult.begin(); it != m_approachGameResult.end(); ++it) {
		sApproachResultData *data = *it;
		if (data != NULL) {
			if (data->m_uid == WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().guid) {
				result = data;
				break;
			}

			result = NULL;
		}
	}

	return result;
}

// golfdoc.cpp:1373
void CGolfDoc::AddApproachTotalPrize(unsigned long prize) {
	m_approachTotalPrize += prize;
}

// golfdoc.cpp:1380
unsigned long CGolfDoc::GetApproachTotalPrize(void) const {
	return m_approachTotalPrize;
}

// golfdoc.cpp:1387
void CGolfDoc::ApproachDataAllClear(void) {
	m_approachReloaded = false;

	m_approachMissionType = (eApproachMissionType)0;
	m_approachPrizeType = (eApproachPrizeType)0;
	m_approachWinnerType = (eApproachWinnerType)0;

	m_approachCurHoleResult.clear();
	m_approachGameResult.clear();

	m_approachTotalPrize = 0;
}
