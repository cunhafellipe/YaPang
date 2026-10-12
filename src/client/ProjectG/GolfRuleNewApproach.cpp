// Original: d:\build\custom\temp\code\client\projectg\golfrulenewapproach.cpp
#include <math.h>
#include "NetworkSystem.h"
#include "GolfRuleNewApproach.h"

#include "GolfDoc.h"
#include "SharedDoc.h"
#include "GolfRule.h"
#include "ClientSetting.h"
#include "polysoup.h"

// golfrulenewapproach.cpp:16
CGolfRuleNewApproach::CGolfRuleNewApproach(CGolfRule *pRule) {
	m_pRule = pRule;

	GOLFDOC()->m_approachMode = true;
}

// golfrulenewapproach.cpp:23
CGolfRuleNewApproach::~CGolfRuleNewApproach(void) {
}

// golfrulenewapproach.cpp:30
void CGolfRuleNewApproach::SetReady(void) {
	CGolfRuleBase::SetReady();

	GOLFDOC()->ApproachDataAllClear();
}

// golfrulenewapproach.cpp:47
void CGolfRuleNewApproach::SetGrade(void) {
}

// The CSharedDoc member this file reads that SharedDoc.h does not declare yet
// (its PADDING(0x4604, 0x4650) covers it).
struct CSharedDocRandoms {
	UNKNOWN(0x0, 0x4608);
	unsigned long m_randoms[18];    // 0x4608
};

// golfrulenewapproach.cpp:55
WVector CGolfRuleNewApproach::GetRandomStartPos(unsigned long seed) {
	// WPolySoup 0xEC: the course's bounding box (a Waabb polysoup.h does not
	// declare yet).
	Waabb box = *(Waabb *)((char *)GOLFDOC()->m_pPolySoup + 0xEC);
	const CSharedDocRandoms *doc = (const CSharedDocRandoms *)WSingleton<CSharedDoc>::Instance();

	WVector pos;
	pos.x = (box.max.x - box.min.x) * ((doc->m_randoms[seed % 18] + seed) & 0x7F) / 128.0f + box.min.x;

	pos.y = 0.0f; pos.z = (box.max.z - box.min.z) * ((doc->m_randoms[17 - seed % 18] + seed) & 0x7F) / 128.0f + box.min.z;

	return pos;
}

// golfrulenewapproach.cpp:71
void CGolfRuleNewApproach::SetPlayer(void) {
	if (m_pRule == 0) {
		return;
	}

	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
		GOLFDOC()->MakePlayer(1);

		std::list<sSlotInfo> &players = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList;
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
			data.m_ballPos = GetRandomStartPos((*it).m_uid);

			data.m_titleTypeId = (*it).m_titleTypeId;
			data.m_characterType = (*it).m_attribute >> 5;

			data.m_team = (*it).m_attribute & 3; data.m_state = 0; data.m_totalShots = 0; data.m_rank = 1; data.m_score = 0; data.m_pang = 0; data.m_extraPang = 0; memset(data.m_holeShots, 0, sizeof(data.m_holeShots)); data.m_joined = 1; data.m_present = 1;
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

			WVector cup = GOLFDOC()->GetHoleData().m_cupPos; WVector diff; diff = cup - data.m_ballPos;

			float dist = sqrtf(diff.x * diff.x + diff.z * diff.z) * 10.0f; data.m_approachDistanceCopy = (int)floor(dist * 0.3125f);
			data.m_approachDistance = 0xFFFFFFFF;
			data.m_approachPoint = -1;
			data.m_identity = (*it).m_identity;
			data.m_mascotTypeId = (*it).m_mascotTypeId;
			data.m_itemBoostFlags = (*it).m_itemBoostFlags;
			data.m_extraFlags = (*it).m_extraFlags;
			data.m_uniteGuildIndex = 0;
			WSingleton<CSharedDoc>::Instance()->m_gameResultList.push_back(data);

			if ((*it).m_uid == MyGuid(true)) {
				PLAYER(0)->m_attribute = (*it).m_attribute & 3;
				m_pRule->AddPlayer(0, (*it).m_nickname, (*it).m_ruleUID, (*it).m_uid, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_caddieInfo.typeId);

				GOLFDOC()->GetHoleData().m_teePos = data.m_ballPos;
			}

			WSingleton<CSharedDoc>::Instance()->ClearUserInfoTimeMap((*it).m_ruleUID);
		}

		GOLFDOC()->RefreshPlayerNum();

		m_pRule->SendHoleData();
	} else {
		WSingleton<CSharedDoc>::Instance()->m_shotTimeLimit = 0;
		WSingleton<COption>::Instance()->dSetNumPlayers(1);
		GOLFDOC()->MakePlayer(1);

		m_pRule->LoadShot("save/pangya_000.sav");

		GOLFDOC()->m_pPolySoup->LoadMapCheckData(m_pRule->m_hole);
	}

	WSingleton<CSharedDoc>::Instance()->m_gameStartTime = 0;
}
