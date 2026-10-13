// Original: d:\build\custom\temp\code\client\projectg\golfruletimeattack.cpp
#include "NetworkSystem.h"
#include "GolfRuleTimeattack.h"

#include "GolfDoc.h"
#include "SharedDoc.h"
#include "GolfRule.h"
#include "ClientSetting.h"
#include "polysoup.h"
#include "shared/Localize.h"

// golfruletimeattack.cpp:19
CGolfRuleTimeattack::CGolfRuleTimeattack(CGolfRule *pRule) {
	m_pRule = pRule;
}

// golfruletimeattack.cpp:24
CGolfRuleTimeattack::~CGolfRuleTimeattack(void) {
}

// golfruletimeattack.cpp:29
void CGolfRuleTimeattack::SetPlayer(void) {
	if (m_pRule == 0) {
		return;
	}

	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
		m_pRule->SendHoleData();

		std::list<sSlotInfo> &players = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList;
		GOLFDOC()->MakePlayer(1);
		WSingleton<CSharedDoc>::Instance()->m_gameResultList.reserve(players.size());

		for (std::list<sSlotInfo>::iterator it = players.begin(); it != players.end(); ++it) {
			if ((*it).m_uid == 0xFFFFFFFF) {
				PLAYER(0)->m_state = 3;
				continue;
			}

			WSingleton<CSharedDoc>::Instance()->SetIndex((*it).m_uid);

			sRivalData data;
			data.m_hole = WSingleton<CSharedDoc>::Instance()->m_holeOrder[0];

			data.m_id = (*it).m_ruleUID;
			data.m_uid = (*it).m_uid;
			data.m_ballPos = GOLFDOC()->GetHoleData().m_teePos;

			data.m_titleTypeId = (*it).m_titleTypeId;
			data.m_team = (*it).m_attribute & 3;
			data.m_characterType = (*it).m_attribute >> 5;

			data.m_rank = 1; data.m_state = 0; data.m_totalShots = 0; data.m_score = 0; data.m_pang = 0; data.m_extraPang = 0; memset(data.m_holeShots, 0, sizeof(data.m_holeShots)); data.m_joined = 1; data.m_present = 1;
			memset(data.m_holeScore, 0, sizeof(data.m_holeScore));
			memset(data.m_holePang, 0, sizeof(data.m_holePang));

			data.m_value13F = (*it).m_connection;
			data.m_sortGroup = players.size();
			data.m_finishOrder = players.size();

			data.m_level = (*it).m_level;
			strcpy(data.m_nickname, (*it).m_nickname);
			strcpy(data.m_guildName, (*it).m_guildName);
			data.m_hasEmblem = (*it).m_guildUID;

			strcpy(data.m_emblemName, (*it).m_emblemName);

			data.m_identity = (*it).m_identity;
			data.m_mascotTypeId = (*it).m_mascotTypeId;
			data.m_itemBoostFlags = (*it).m_itemBoostFlags;
			data.m_uniteGuildIndex = 0;
			data.m_extraFlags = (*it).m_extraFlags;
			WSingleton<CSharedDoc>::Instance()->m_gameResultList.push_back(data);

			if ((*it).m_uid == MyGuid(true)) {
				PLAYER(0)->m_attribute = (*it).m_attribute & 3;
				m_pRule->AddPlayer(0, (*it).m_nickname, (*it).m_ruleUID, (*it).m_uid, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_caddieInfo.typeId);
			}

			WSingleton<CSharedDoc>::Instance()->ClearUserInfoTimeMap((*it).m_ruleUID);
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

// golfruletimeattack.cpp:212
void CGolfRuleTimeattack::SetGrade(void) {
}

// golfruletimeattack.cpp:217
void CGolfRuleTimeattack::SetReady(void) {
	if (IsLocalContent((localContentType_t)0x30)) {
		CheckCalipersCount();
	}
	m_pRule->ChangeGameMode(1);
}
