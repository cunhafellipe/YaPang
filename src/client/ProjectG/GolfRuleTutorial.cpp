// Original: d:\build\custom\temp\code\client\projectg\golfruletutorial.cpp
#include "NetworkSystem.h"
#include "GolfRuleTutorial.h"
#include "GolfRule.h"
#include "GolfDoc.h"
#include "SharedDoc.h"
#include "quadtree.h"
#include "golfball.h"
#include "polysoup.h"
#include "TaskManager.h"
#include "actor.h"
#include "ClientSetting.h"
#include "shared/Localize.h"
#include "ItemManager.h"
#include <string.h>

// golfruletutorial.cpp:20
CGolfRuleTutorial::CGolfRuleTutorial(CGolfRule *pRule) {
	m_pRule = pRule;
}

// golfruletutorial.cpp:25
CGolfRuleTutorial::~CGolfRuleTutorial(void) {
}

// golfruletutorial.cpp:30
void CGolfRuleTutorial::SetPlayer(void) {
	if (m_pRule == 0) {
		return;
	}

	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
		m_pRule->SendHoleData();

		GOLFDOC()->MakePlayer(1);
		WSingleton<CSharedDoc>::Instance()->SetIndex(WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().guid);
		SetEquipCharCaddieInfoForTutorial();

		CSharedDoc *pDoc = WSingleton<CSharedDoc>::Instance();
		switch (pDoc->m_roomType) {
		case 0xB:
			m_pRule->AddPlayer(0, pDoc->GetPangyaUserInfo().nickname, pDoc->m_myUID, pDoc->GetPangyaUserInfo().guid, 0x1C000001);
			GOLFDOC()->m_gameState = 2;
			break;
		case 0xC: {
			m_pRule->AddPlayer(0, pDoc->GetPangyaUserInfo().nickname, pDoc->m_myUID, pDoc->GetPangyaUserInfo().guid, 0x1C000001);

			int gameState = 1;
			int result1, result2, result4, result5;
			IActor *pActor = AfxGetPreservedTask()->GetActor("TutorialMain");
			if (pActor) {
				pActor << MsgObject(0, 0x14F, (intptr_t)&result1, (intptr_t)&result2, (intptr_t)&gameState, (intptr_t)&result4, (intptr_t)&result5);
			}
			GOLFDOC()->m_gameState = gameState;
			break;
		}
		}

		GOLFDOC()->RefreshPlayerNum();
	} else {
		WSingleton<CSharedDoc>::Instance()->m_shotTimeLimit = 0;
		WSingleton<COption>::Instance()->dSetNumPlayers(1);
		GOLFDOC()->MakePlayer(1);

		m_pRule->LoadShot("save/pangya_000.sav");

		GOLFDOC()->m_pPolySoup->LoadMapCheckData(m_pRule->m_hole);
	}
}

// golfruletutorial.cpp:133
void CGolfRuleTutorial::SetGrade(void) {
}

// golfruletutorial.cpp:138
void CGolfRuleTutorial::SetReady(void) {
	GetPVS().ResetHoleData();
	int gameState = GOLFDOC()->m_gameState;
	if (gameState != 9 && gameState < 0xF) {
		GolfBall().Reset(GolfBall().m_pos);
	}

	GolfBall().m_groundArea = GetPVS().GetMeshIndex(GolfBall().m_pos, false);
	m_pRule->ResetData(GolfBall().m_pos);

	GOLFDOC()->m_myPlayerIndex = m_pRule->GetFirstTurn();
	PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] = 1;
	PLAYER(GOLFDOC()->m_myPlayerIndex)->m_state = 0;
	m_pRule->SendMsg(m_pRule, "Logo", 0x18B, 0, 0, 0, 0);
}

// golfruletutorial.cpp:156
void CGolfRuleTutorial::SetEquipCharCaddieInfoForTutorial(void) {
	if (IsLocalContent((localContentType_t)0x38)) {
		memset(&WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_characterInfo, 0, sizeof(sCharacterInfo));
		memset(&WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_caddieInfo, 0, sizeof(sCaddieInfo));
		memset(&WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_clubSet, 0, sizeof(sClubSetRecord));
		memset(&WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_mascot, 0, sizeof(sMascotInfo));
		memset(&WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_equipHead, 0, 0x6C);

		memcpy(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_nickname, &WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo(), 0x10D);
		memcpy(&WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_statShots, &WSingleton<CSharedDoc>::Instance()->m_myStatShots, sizeof(sPangYaUserStatistics));
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_skinTypeId = WSingleton<CSharedDoc>::Instance()->m_equipSkin[5];

		CSharedDoc *pDoc = WSingleton<CSharedDoc>::Instance();
		sCharacterInfo *pChar = &pDoc->m_roomPlayers[0].m_characterInfo;
		bool gender = pDoc->m_roomPlayers[0].m_eventFlags.gender % 2;
		pChar->typeId = 0x4000000 + gender;
		ItemManager()->GetDefCombo(pChar->typeId, pChar->partTypeIds);
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_clubSet.typeId = 0x10000000;
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_ballId = 0x14000000;
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_caddieInfo.level = 2;
	}
}
