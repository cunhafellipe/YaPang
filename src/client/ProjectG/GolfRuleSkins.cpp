// Original: d:\build\custom\temp\code\client\projectg\golfruleskins.cpp
#include "NetworkSystem.h"
#include "GolfRuleSkins.h"

#include "GolfDoc.h"
#include "SharedDoc.h"
#include "GolfRule.h"
#include "ClientSetting.h"
#include "polysoup.h"

// golfruleskins.cpp:19
CGolfRuleSkins::CGolfRuleSkins(CGolfRule *pRule) {
	m_pRule = pRule;
}

// golfruleskins.cpp:24
CGolfRuleSkins::~CGolfRuleSkins(void) {
}

// golfruleskins.cpp:29
void CGolfRuleSkins::SetPlayer(void) {
	if (m_pRule == 0) {
		return;
	}

	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
		std::list<sSlotInfo> &players = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList;
		m_pRule->SendHoleData();

		GOLFDOC()->MakePlayer(players.size());

		unsigned char index = 0;
		for (std::list<sSlotInfo>::iterator it = players.begin(); it != players.end(); ++it, index++) {
			if ((*it).m_uid == 0xFFFFFFFF) {
				PLAYER(index)->m_state = 3;
				continue;
			}

			WSingleton<CSharedDoc>::Instance()->SetIndex((*it).m_uid);

			PLAYER(index)->m_index = index;
			PLAYER(index)->m_attribute = (*it).m_attribute & 3;
			m_pRule->AddPlayer(index, (*it).m_nickname, (*it).m_ruleUID, (*it).m_uid, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_caddieInfo.typeId);
		}

		GOLFDOC()->RefreshPlayerNum();
	} else {
		WSingleton<CSharedDoc>::Instance()->m_shotTimeLimit = 0;
		WSingleton<COption>::Instance()->dSetNumPlayers(1);
		GOLFDOC()->MakePlayer(1);

		m_pRule->LoadShot(WSingleton<CSharedDoc>::Instance()->m_fileName);

		GOLFDOC()->m_pPolySoup->LoadMapCheckData(m_pRule->m_hole);
	}
}

// golfruleskins.cpp:125
void CGolfRuleSkins::SetGrade(void) {
	for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (GOLFDOC()->GetPlayer(i)->m_state == 3) {
			continue;
		}

		unsigned char grade;
		if (WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_level < 6) {
			grade = 1;
		} else {
			grade = (WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_level - 1) / 5 + 1;
		}

		if (GOLFDOC()->m_maxGrade < grade) {
			GOLFDOC()->m_maxGrade = grade;
		}
	}
}

// golfruleskins.cpp:147
void CGolfRuleSkins::SetReady(void) {
	CGolfRuleBase::SetReady();
}
