// Original: d:\build\custom\temp\code\client\projectg\golfrulestroke.cpp
#include "NetworkSystem.h"
#include "GolfRuleStroke.h"

#include "GolfDoc.h"
#include "SharedDoc.h"
#include "GolfRule.h"
#include "ClientSetting.h"
#include "polysoup.h"
#include "shared/Localize.h"

// golfrulestroke.cpp:20
CGolfRuleStroke::CGolfRuleStroke(CGolfRule *pRule) {
	m_pRule = pRule;
}

// golfrulestroke.cpp:25
CGolfRuleStroke::~CGolfRuleStroke(void) {
}

// golfrulestroke.cpp:30
void CGolfRuleStroke::SetPlayer(void) {
	if (m_pRule == 0) {
		return;
	}

	if (IsLocalContent((localContentType_t)0x4F) && WSingleton<CSharedDoc>::Instance()->m_playingMode == (eReplayModeType)2) {
		return;
	}

	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
		m_pRule->SendHoleData();

		std::list<sSlotInfo> &players = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList;
		GOLFDOC()->MakePlayer(WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.size());

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

// golfrulestroke.cpp:149
void CGolfRuleStroke::SetGrade(void) {
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

// golfrulestroke.cpp:171
void CGolfRuleStroke::SetReady(void) {
	CGolfRuleBase::SetReady();
}
