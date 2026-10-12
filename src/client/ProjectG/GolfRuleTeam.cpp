// Original: d:\build\custom\temp\code\client\projectg\golfruleteam.cpp
#include "NetworkSystem.h"
#include "GolfRuleTeam.h"

#include "GolfDoc.h"
#include "SharedDoc.h"
#include "GolfRule.h"
#include "ClientSetting.h"
#include "polysoup.h"
#include "shared/Localize.h"
#include "cardsystem.h"
#include "parttidlist.h"
#include "wutil.h"

// golfruleteam.cpp:21
CGolfRuleTeam::CGolfRuleTeam(CGolfRule *pRule) {
	m_pRule = pRule;
}

// golfruleteam.cpp:26
CGolfRuleTeam::~CGolfRuleTeam(void) {
}

// golfruleteam.cpp:31
void CGolfRuleTeam::SetPlayer(void) {
	if (m_pRule == 0) {
		return;
	}

	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
		m_pRule->SendHoleData();

		std::list<sSlotInfo> &players = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList;
		GOLFDOC()->MakeTeam();

		GOLFDOC()->ResetTeamVars();

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

// golfruleteam.cpp:138
void CGolfRuleTeam::SetGrade(void) {
	for (unsigned char i = 0; i < 2; i++) {
		unsigned char sum = 0;

		for (i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			if (GOLFDOC()->GetPlayer(i)->m_attribute != i) {
				continue;
			}

			if (WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_level < 6) {
				sum += 1;
			} else {
				sum += (WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_level - 1) / 5 + 1;
			}
		}

		unsigned char grade = sum / 2;
		if (GOLFDOC()->m_maxGrade < grade) {
			GOLFDOC()->m_maxGrade = grade;
		}
	}
}

// golfruleteam.cpp:164
void CGolfRuleTeam::SetReady(void) {
	CGolfRuleBase::SetReady();
}

// golfruleteam.cpp:178
void CGolfRuleTeam::AddPlayer(unsigned char index, const char *nickname, unsigned long oid, unsigned long uid, unsigned long caddieTypeId) {
	if (index >= GOLFDOC()->GetPlayerNum()) {
		return;
	}

	float cadComboGauge = 0.0f;
	sPlayerData *pPlayer = GOLFDOC()->GetPlayer(index);
	pPlayer->m_partnerIndex = 0xFF;
	for (int i = 0; i < index; i++) {
		if (GOLFDOC()->GetPlayer(i)->m_attribute == pPlayer->m_attribute) {
			GOLFDOC()->GetPlayer(i)->m_partnerIndex = index;
			pPlayer->m_partnerIndex = i;
			break;
		}
	}

	pPlayer->m_color = TEAM(pPlayer->m_attribute)->m_color;
	pPlayer->m_oid = oid;
	pPlayer->m_uid = uid;
	pPlayer->m_state = 2;
	pPlayer->m_chatPenalty = false;
	pPlayer->m_numItems = 0;
	pPlayer->m_shotItemTypeId = 0;
	pPlayer->m_totalScore = 0;
	pPlayer->m_playTime = 0;
	pPlayer->m_joinState = 1;

	if (WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_caddieInfo.cadItemTypeId != 0) {
		IFF_STRUCT::sCadItem *pCadItem = ItemManager()->FindCadItem(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_caddieInfo.cadItemTypeId);
		if (pCadItem != 0) {
			cadComboGauge = (float)pCadItem->m_levels[4];
		}
	}

	cadComboGauge += WSingleton<CSharedDoc>::Instance()->GetWtPepPangyaComboGauge(index);
	if (IsLocalContent((localContentType_t)0x51)) {
		WSingleton<CCardManager>::Instance()->SetPlayerIndex(index);
		cadComboGauge += WSingleton<CCardManager>::Instance()->GetCardPeriodComboGauge();
	}

	if (TEAM(pPlayer->m_attribute)->m_teamRate < cadComboGauge) {
		TEAM(pPlayer->m_attribute)->m_teamRate = Between(0.0f, cadComboGauge, WSingleton<CSharedDoc>::Instance()->GetComboGaugeLimit(pPlayer->m_attribute));
	}

	pPlayer->m_comboGauge = 0.0f;

	if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 8) {
		pPlayer->m_comboGauge = 0.0f;
	}

	pPlayer->m_ghostResult = 0;
	pPlayer->m_ghostAward = 0;
	pPlayer->m_extraPang = 0;
	pPlayer->m_rewardPang = 0;
	pPlayer->m_recordedHoles = 0;

	pPlayer->m_itemPang = 0;
	pPlayer->m_roundPang = 0;

	pPlayer->m_extPrizeCount = 0;
	memset(&pPlayer->m_prizeItems, 0, sizeof(pPlayer->m_prizeItems));
	pPlayer->m_angelWingGrade = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_level;

	for (int hole = 0; hole < 18; hole++) {
		pPlayer->m_holes[hole] = 0;
		pPlayer->m_holeIn[hole] = 0;
		pPlayer->m_holeRecord[hole] = 0;
		pPlayer->m_holeRecord2[hole] = 0;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 8) {
		for (int i = 0; i < 10; i++) {
			WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_equipItem[i] = 0;
		}
	}

	sCharacterInfo *pCharacter = &WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_characterInfo;
	CPartTidList &tids = GOLFDOC()->GetPartTidList(index);
	tids.SetTids(pCharacter, 0xFF);

	pPlayer->m_numItems = WSingleton<CSharedDoc>::Instance()->CollapseItemSlot(index);

	GOLFDOC()->SetPlayerLevel(index);
}

// golfruleteam.cpp:291
int CGolfRuleTeam::GetNumTimeout(void) {
	int numTimeout = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_numTimeout;

	if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_partnerIndex != 0xFF) {
		numTimeout += PLAYER(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_partnerIndex)->m_numTimeout;
	}

	return numTimeout;
}

// golfruleteam.cpp:302
bool CGolfRuleTeam::CheckGiveUp(void) {
	if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_state == 3) {
		return false;
	}

	unsigned char limit = GOLFDOC()->GetHoleData().m_par + 4;
	int state = TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_state;
	unsigned char score = TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1];

	return state == 1 ? score > limit : score >= limit;
}
