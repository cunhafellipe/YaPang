// Original: d:\build\custom\temp\code\client\projectg\golfrulebase.cpp
#include "NetworkSystem.h"
#include "GolfRuleBase.h"

#include "GolfDoc.h"
#include "SharedDoc.h"
#include "GolfRule.h"
#include "TaskManager.h"
#include "actor.h"
#include "shared/Localize.h"
#include "ClientSetting.h"
#include "polysoup.h"
#include "cardsystem.h"
#include "parttidlist.h"
#include <algorithm>

// Actor name pointer the ready message 0x26F goes to (no symbol; not cut yet).
extern "C" const char *data_a7e3c8;

// golfrulebase.cpp:24
CGolfRuleBase::CGolfRuleBase(void) {
	m_pRule = 0;

	m_colors[0] = 0xFD0002;
	m_colors[1] = 0xDC00FD;
	m_colors[2] = 0x27D628;
	m_colors[3] = 0x2778D6;
}

// golfrulebase.cpp:34
CGolfRuleBase::~CGolfRuleBase(void) {
}

// golfrulebase.cpp:38
void CGolfRuleBase::CheckCalipersCount(void) {
	AfxGetTask()->GetActor(data_a7e3c8) << MsgObject(0, 0x26F, 0, 0, 0, 0, 0);

	std::list<sItemInfo>::iterator it = std::find(WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(), WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(), (unsigned long)0x1A000040);
	if (it == WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end()) {
		return;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_gameMode == 1) {
		return;
	}

	IActor *pActor = AfxGetTask()->GetActor(data_a7e3c8);
	pActor << MsgObject(0, 0x26F, 5, (*it).m_count, 0, 0, 0);
	pActor << MsgObject(0, 0x26F, 2, 0, 0, 0, 0);
}

// golfrulebase.cpp:68
void CGolfRuleBase::SetPlayer(void) {
	if (m_pRule == 0) {
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

		m_pRule->LoadShot("save/pangya_000.sav");

		GOLFDOC()->m_pPolySoup->LoadMapCheckData(m_pRule->m_hole);
	}
}

// golfrulebase.cpp:184
void CGolfRuleBase::SetGrade(void) {
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

// golfrulebase.cpp:206
void CGolfRuleBase::SetReady(void) {
	if (IsLocalContent((localContentType_t)0x30)) {
		CheckCalipersCount();
	}

	AfxGetTask()->GetActor(data_a7e3c8) << MsgObject(0, 0x26F, 10, 0, 0, 0, 0);

	m_pRule->ChangeGameMode(1);
}

// golfrulebase.cpp:220
void CGolfRuleBase::AddPlayer(unsigned char index, const char *nickname, unsigned long oid, unsigned long uid, unsigned long caddieTypeId) {
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

	pPlayer->m_comboGauge = cadComboGauge;
	pPlayer->m_comboGauge += WSingleton<CSharedDoc>::Instance()->GetWtPepPangyaComboGauge(index);
	if (IsLocalContent((localContentType_t)0x51)) {
		WSingleton<CCardManager>::Instance()->SetPlayerIndex(index);
		pPlayer->m_comboGauge += WSingleton<CCardManager>::Instance()->GetCardPeriodComboGauge();
	}

	if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 8) {
		pPlayer->m_comboGauge = 0.0f;
		for (int i = 0; i < 10; i++) {
			WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_equipItem[i] = 0;
		}
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

	sCharacterInfo *pCharacter = &WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_characterInfo;
	CPartTidList &tids = GOLFDOC()->GetPartTidList(index);
	tids.SetTids(pCharacter, 0xFF);

	pPlayer->m_numItems = WSingleton<CSharedDoc>::Instance()->CollapseItemSlot(index);

	GOLFDOC()->SetPlayerLevel(index);
}

// golfrulebase.cpp:323
void CGolfRuleBase::AddPlayerOffline(unsigned char index, const char *nickname, unsigned long uid, unsigned long caddieTypeId) {
	if (index >= GOLFDOC()->GetPlayerNum()) {
		return;
	}

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
	strcpy(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_name, nickname);

	pPlayer->m_uid = uid;
	pPlayer->m_state = 2;
	pPlayer->m_chatPenalty = false;
	pPlayer->m_numItems = 0;
	pPlayer->m_shotItemTypeId = 0;
	pPlayer->m_totalScore = 0;
	pPlayer->m_playTime = 0;
	pPlayer->m_joinState = 1;
	pPlayer->m_comboGauge = 0.0f;

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

	WSingleton<CSharedDoc>::Instance()->SetIndex(index);
	GOLFDOC()->GetPlayer(index)->m_index = index;

	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_equipItem[0] = 0x18000010;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_equipItem[1] = 0x18000010;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_equipItem[2] = 0x18000010;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_equipItem[3] = 0x18000010;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_equipItem[4] = 0x18000010;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_equipItem[5] = 0x18000010;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_equipItem[6] = 0x18000010;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_equipItem[7] = 0x18000010;
	GOLFDOC()->GetPlayer(index)->m_numItems = 10;

	int character;
	if (index == 0) {
		character = WSingleton<COption>::Instance()->dGetDefaultCharacter();
	} else {
		character = rand() % ItemManager()->GetNumItems((PANGYA_ITEM_GROUP)1);
	}

	int club = WSingleton<COption>::Instance()->dGetDefaultClub();
	int ball = WSingleton<COption>::Instance()->dGetDefaultBall();

	unsigned long partTypeIds[24];
	unsigned long characterTypeId = character | 0x4000000;
	ItemManager()->GetDefCombo(characterTypeId, partTypeIds);

	if (character == 0) {
		partTypeIds[3] = 0x8006001;
		partTypeIds[5] = 0x800A000;
		partTypeIds[6] = 0;
		partTypeIds[7] = 0;
		partTypeIds[8] = 0x8010002;
		partTypeIds[19] = 0x8026800;
	} else if (character == 1) {
		partTypeIds[0] = 0x8040802;
		partTypeIds[2] = 0x8044000;
		partTypeIds[4] = 0;
		partTypeIds[5] = 0;
		partTypeIds[7] = 0x804E001;
		partTypeIds[18] = 0x8064800;
	} else if (character == 2) {
		partTypeIds[0] = 0x8080801;
		partTypeIds[1] = 0;
		partTypeIds[2] = 0x8084004;
		partTypeIds[3] = 0;
		partTypeIds[4] = 0x8088801;
		partTypeIds[5] = 0x808A004;
		partTypeIds[6] = 0;
		partTypeIds[7] = 0x808E003;
		partTypeIds[8] = 0x8090800;
		partTypeIds[9] = 0x8083001;
		partTypeIds[17] = 0x80A2800;
	} else if (character == 3) {
		partTypeIds[2] = 0x80C4003;
		partTypeIds[3] = 0;
		partTypeIds[5] = 0x80CA003;
		partTypeIds[7] = 0x80CE003;
		partTypeIds[8] = 0x80D0801;
		partTypeIds[10] = 0x80D4800;
		partTypeIds[18] = 0x80E4800;
	} else if (character == 4) {
		partTypeIds[2] = 0x8104002;
		partTypeIds[3] = 0x8106001;
		partTypeIds[4] = 0x8108002;
		partTypeIds[5] = 0x810A002;
		partTypeIds[9] = 0x8112000;
		partTypeIds[10] = 0x8114000;
		partTypeIds[17] = 0x8122800;
	} else if (character == 5) {
		partTypeIds[23] = 0x816E801;
	} else if (character == 6) {
		partTypeIds[18] = 0x81A4800;
	} else if (character == 7) {
		partTypeIds[21] = 0x81EA800;
	} else if (character == 8) {
		partTypeIds[20] = 0x8228800;
	} else if (character == 9) {
		partTypeIds[2] = 0x824400B;
		partTypeIds[5] = 0;
		partTypeIds[20] = 0x8268800;
	}

	GOLFDOC()->SetPartTidList(index, characterTypeId, 0, 0, partTypeIds, 0);

	if (OfflinePlay()) {
		GOLFDOC()->GetPartTidList(index).SetDefaultTids();
	}

	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_characterId = 0;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_equipHead = 0;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_clubId = 0;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_ballId = ball | 0x14000000;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_mascotId = 0;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_characterInfo.typeId = characterTypeId;
	memcpy(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_characterInfo.partTypeIds, partTypeIds, sizeof(partTypeIds));
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_characterInfo.auxParts.tids[0] = 0x70000080;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_caddieInfo.id = caddieTypeId != 0 ? index : 0;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_caddieInfo.typeId = caddieTypeId;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_clubSet.typeId = club | 0x10000000;

	rand();

	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_level = 0;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_value155 = rand() % WSingleton<CSharedDoc>::Instance()->m_levelExp[0].m_exp;

	GOLFDOC()->SetPlayerLevel(index);
}

// golfrulebase.cpp:515
int CGolfRuleBase::GetNumTimeout(void) {
	return PLAYER(GOLFDOC()->m_myPlayerIndex)->m_numTimeout;
}

// golfrulebase.cpp:521
bool CGolfRuleBase::CheckGiveUp(void) {
	if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_state == 3) {
		return false;
	}

	unsigned char limit = GOLFDOC()->GetHoleData().m_par + 4;
	int state = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_state;
	unsigned char score = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1];

	return state == 1 ? score > limit : score >= limit;
}
