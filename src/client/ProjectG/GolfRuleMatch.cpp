// Original: d:\build\custom\temp\code\client\projectg\golfrulematch.cpp
#include "NetworkSystem.h"
#include "GolfRuleMatch.h"

#include "GolfDoc.h"
#include "SharedDoc.h"
#include "GolfRule.h"
#include "ClientSetting.h"
#include "polysoup.h"
#include "parttidlist.h"

// golfrulematch.cpp:19
CGolfRuleMatch::CGolfRuleMatch(CGolfRule *pRule) {
	m_pRule = pRule;
}

// golfrulematch.cpp:24
CGolfRuleMatch::~CGolfRuleMatch(void) {
}

// golfrulematch.cpp:29
void CGolfRuleMatch::SetPlayer(void) {
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

		m_pRule->LoadShot("save/pangya_000.sav");

		GOLFDOC()->m_pPolySoup->LoadMapCheckData(m_pRule->m_hole);
	}
}

// golfrulematch.cpp:125
void CGolfRuleMatch::SetGrade(void) {
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

// golfrulematch.cpp:147
void CGolfRuleMatch::SetReady(void) {
	CGolfRuleBase::SetReady();
}

// golfrulematch.cpp:161
void CGolfRuleMatch::AddPlayer(unsigned char index, const char *nickname, unsigned long oid, unsigned long uid, unsigned long caddieTypeId) {
	if (index >= GOLFDOC()->GetPlayerNum()) {
		return;
	}

	float cadComboGauge = 0.0f;
	sPlayerData *pPlayer = GOLFDOC()->GetPlayer(index);
	pPlayer->m_turnIndex = 0;
	pPlayer->m_turnOrder = 1;
	for (int i = 0; i < index; i++) {
		if (GOLFDOC()->GetPlayer(i)->m_state == 3) {
			continue;
		}
		pPlayer->m_turnIndex++;
		pPlayer->m_turnOrder++;
	}

	pPlayer->m_color = m_colors[index];
	pPlayer->m_uid = uid;
	pPlayer->m_oid = oid;
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

	pPlayer->m_comboGauge = cadComboGauge;
	pPlayer->m_comboGauge += WSingleton<CSharedDoc>::Instance()->GetWtPepPangyaComboGauge(index);

	if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 8) {
		pPlayer->m_comboGauge = 0.0f;
	}

	pPlayer->m_ghostResult = 0;
	pPlayer->m_ghostAward = 0;
	pPlayer->m_extraPang = 0;

	pPlayer->m_rewardPang = 0;
	pPlayer->m_itemPang = 0;
	pPlayer->m_roundPang = 0;

	pPlayer->m_recordedHoles = 0;

	pPlayer->m_extPrizeCount = 0;
	memset(&pPlayer->m_prizeItems, 0, sizeof(pPlayer->m_prizeItems));
	pPlayer->m_angelWingGrade = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_level;

	for (int hole = 0; hole < 18; hole++) {
		pPlayer->m_holes[hole] = 0;
		pPlayer->m_holeIn[hole] = 0;
		pPlayer->m_holeRecord[hole] = 0;
		pPlayer->m_holeRecord2[hole] = 0;
	}

	for (int i = 0; i < 10; i++) {
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_equipItem[i] = 0x1800000D;
	}

	sCharacterInfo *pCharacter = &WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_characterInfo;
	CPartTidList &tids = GOLFDOC()->GetPartTidList(index);
	tids.SetTids(pCharacter, 0xFF);

	pPlayer->m_numItems = WSingleton<CSharedDoc>::Instance()->CollapseItemSlot(index);

	GOLFDOC()->SetPlayerLevel(index);
}
