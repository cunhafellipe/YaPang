// Original: d:\build\custom\temp\code\client\projectg\intrusion.cpp
#include "NetworkSystem.h"
#include "Intrusion.h"
#include "globalgamedefine.h"
#include "Packet.h"
#include "SharedDoc.h"
#include "frform.h"
#include "fresh.h"
#include "TaskManager.h"
#include "ChatMsg.h"
#include "commonutil.h"
#include "GolfDoc.h"
#include "messengerdlg.h"
#include "mousecursor.h"
#include "ClientSetting.h"
#include "icontentsdoc.h"
#include "contentsdoc.h"
#include "user_info.h"
#include "cardsystem.h"
#include "BongdariShopDlg.h"
#include "frwndmanager.h"
#include "frdesktop.h"

// intrusion.cpp:28
unsigned char CIntrusion::GetCalcExpMemberSize(void) {
	return m_calcExpMemberSize;
}

// intrusion.cpp:34
void CIntrusion::FinishGame(void) {
	m_calcExpMemberSize = 0;
}

// intrusion.cpp:41
CIntrusion::CIntrusion(void) {
	m_pDlg = 0;
	m_roomNumber = 0;
	m_calcExpMemberSize = 0;
}

// intrusion.cpp:45
CIntrusion::~CIntrusion(void) {
}

// intrusion.cpp:54
unsigned long CIntrusion::GetActiveColor(void) {
	return 0xC3E5FFD8;
}

// intrusion.cpp:64
bool CIntrusion::OnAccessRoom(sRoomInfo& room) {
	if (room.m_state == 1) {
		if (room.m_intrusionAccess == 0) {
			room.m_intrusionAccess = 1;
			return true;
		}
	}
	return false;
}

// intrusion.cpp:90
void CIntrusion::DoIntrusionAccess(WReceivedPacket& packet, eIntrusionPermitCode code) {
	switch (code) {
	case 0:
		CSharedDoc::Instance()->m_userInfo.identityBits.gallery = 0;
		CSharedDoc::Instance()->m_userInfo.galleryGuid = 0xFFFFFFFF;

		CSharedDoc::Instance()->m_roomPlayerList.clear();
		CSharedDoc::Instance()->m_gameResultList.clear();
		CSharedDoc::Instance()->m_vector4524.clear();
		CSharedDoc::Instance()->m_gameOver = false;

		packet.DecodeBuffer(&CSharedDoc::Instance()->m_roomInfo, sizeof(sRoomInfo));

		CSharedDoc::Instance()->m_chatManager.FilteringHack(CSharedDoc::Instance()->m_roomInfo.m_title, false);
		CSharedDoc::Instance()->m_mySlot = CSharedDoc::Instance()->m_roomInfo.m_roomNumber;

		if (WSingleton<WNetworkSystem>::Instance()->IsConnected((WNetworkSystem::eNetUnit)1) && CMessengerInfo::Instance()->GetComplete() == CONNECT_STATUS_COMPLETE) {
			// Tells the messenger where the player is.
#pragma pack(push, 1)
			struct {
				unsigned short roomNumber;  // 0x0
				unsigned long gameType;     // 0x2
				unsigned long serverUID;    // 0x6
				unsigned char channelId;    // 0xa
				char channelName[0x40];     // 0xb
			} location;
#pragma pack(pop)
			memset(&location, 0, sizeof(location));

			location.serverUID = CSharedDoc::Instance()->m_gameServerUID;
			location.channelId = CSharedDoc::Instance()->m_currentChannel.id;
			location.roomNumber = CSharedDoc::Instance()->m_roomInfo.m_roomNumber;
			location.gameType = CSharedDoc::Instance()->m_roomInfo.m_gameType;
			wsprintf(location.channelName, "%s", CSharedDoc::Instance()->m_currentChannel.name);

			WSendPacket locationPacket(0x23);
			locationPacket.EncodeBuffer(&location, sizeof(location));
			locationPacket.Send(SEND_TO_MSN);
		}
		break;
	case 1: {
		AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 0x2E, 0, 0, 0, 0, 0));

		CSharedDoc::Instance()->m_chatLineList.clear();

		std::list<sSlotInfo>& players = CSharedDoc::Instance()->m_roomPlayerList;
		players.clear();

		int count = packet.Decode1();
		for (int i = 0; i < count; i++) {
			sSlotInfo slot;
			packet.DecodeBuffer(&slot, sizeof(slot));

			if (slot.m_attributeBits.roomMaster) {
				slot.m_flags55.flag1 = 1;
			} else {
				slot.m_flags55.flag1 = 0;
			}

			players.push_back(slot);
		}
		break;
	}
	case 2:
		CSharedDoc::Instance()->m_pangPercent = packet.Decode4();
		break;
	case 3: {
		CSharedDoc::Instance()->ClearRivalVars(false);

		// Bits 3-5 of the event flags.
		unsigned char flag = packet.Decode1();
		CSharedDoc::Instance()->m_userInfo.eventFlags ^= ((flag << 3) ^ CSharedDoc::Instance()->m_userInfo.eventFlags) & 0x08;
		flag = packet.Decode1();
		CSharedDoc::Instance()->m_userInfo.eventFlags ^= ((flag << 4) ^ CSharedDoc::Instance()->m_userInfo.eventFlags) & 0x10;
		flag = packet.Decode1();
		CSharedDoc::Instance()->m_userInfo.eventFlags ^= ((flag << 5) ^ CSharedDoc::Instance()->m_userInfo.eventFlags) & 0x20;

		if (IsLocalContent((localContentType_t)0x51)) {
			SYSTEMTIME serverTime;
			packet.DecodeBuffer(&serverTime, sizeof(serverTime));
			CSharedDoc::Instance()->SetServerTime(serverTime);

			CCardManager::Instance()->m_playerIndexMap[0] = 0;

			std::map<unsigned int, sCharacterInfo>::iterator it = CSharedDoc::Instance()->m_characterMap.find(CSharedDoc::Instance()->m_myCharacterId);
			if (it != CSharedDoc::Instance()->m_characterMap.end()) {
				CCardManager::Instance()->m_characterInfo[0] = it->second;
			}
		}

		CSharedDoc::Instance()->SetEquipCharCaddieInfoFromMyInfo();
		break;
	}
	case 4: {
		FrWnd* pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop");
		FrBongdariShopDlg* pShop = (FrBongdariShopDlg*)(pWnd != 0 ? (void*)pWnd : (__rtti_obj = pWnd));
		if (pShop) {
			pShop->Clear();
		}

		if (CUserInfo::Instance()) {
			CUserInfo::Instance()->Clear();
		}

		SetCurMap(packet.Decode1());
		SetGameType(packet.Decode1());
		unsigned char gameKind = packet.Decode1();
		CSharedDoc::Instance()->m_gameKind = gameKind;
		unsigned char holes = packet.Decode1();
		CSharedDoc::Instance()->m_holes = holes;
		CSharedDoc::Instance()->m_matchTypeId = packet.Decode4();
		unsigned long shotTimeLimit = packet.Decode4();
		CSharedDoc::Instance()->m_shotTimeLimit = shotTimeLimit;
		unsigned long gameTimeLimit = packet.Decode4();
		CSharedDoc::Instance()->m_gameTimeLimit = gameTimeLimit;

		if (!IsMassGame()) {
			unsigned long myUid = CSharedDoc::Instance()->m_userInfo.guid;
			for (int player = 0; player < 4; player++) {
				if (CSharedDoc::Instance()->m_roomPlayers[player].m_guid == myUid) {
					break;
				}
			}
		}

		for (int i = 0; i < 18; i++) {
			CSharedDoc::Instance()->m_holeRandom[i] = packet.Decode4();
			CSharedDoc::Instance()->m_holePin[i] = CSharedDoc::Instance()->m_holeRandom[i] % 3;
			CSharedDoc::Instance()->m_holeOrder[i] = packet.Decode1();
		}

		CGimmickContainer* pGimmick = NULL;
		CContentsDoc::Instance()->GetContainer((localContentType_t)0xA3, pGimmick);
		pGimmick->LoadFromPacket(packet);

		memset(CSharedDoc::Instance()->m_courseHoleOn, 0, sizeof(CSharedDoc::Instance()->m_courseHoleOn));

		CSharedDoc::Instance()->m_value4D74 = 0;
		CSharedDoc::Instance()->LogGstUpdateHole(0xFFFFFFFF, 0xFF, -1, -1);

		if (IsLocalContent((localContentType_t)0x17)) {
			CSharedDoc::Instance()->SetInitWearHalloweenItem();
		}

		if (CMessengerInfo::Instance()->m_myStatus != 3) {
			CMessengerInfo::Instance()->SetMyStatus((MSN::eStatusType)0);
		}

		CSharedDoc::Instance()->m_gameOver = false;

		CMouseCursor::Instance()->m_mode = 1;
		CMouseCursor::Instance()->SetActive(false);
		COption::Instance()->vSetMinimumScreenSize(0, 800, 600);
		CTaskManager::Instance()->ChangeTask("CGolfTask", "CGolfDoc", false);

		IActor* pScore = AfxGetTask()->GetActor("Score");
		if (pScore) {
			pScore << MsgObject(0, 0x131, 0, 0, 0, 0, 0);
			pScore << MsgObject(0, 0x2A, 0, 0, 0, 0, 0);
		}
		break;
	}
	}
}

// A rival's per-hole shots, scores and pang (0xEA bytes, packed), as
// RivalDataUpdate decodes them and PacketAnalysis (0x9A reply) sends them.
// The original names are not known.
#pragma pack(push, 1)
struct sRivalHoleData {
	unsigned char holeShots[18];    // 0x0
	int holeScore[18];              // 0x12
	__int64 holePang[18];           // 0x5a
};
struct sRivalHoles {
	sRivalHoleData data;            // 0x0
	unsigned long uid;              // 0xea, the rival asked about
	unsigned long ownerUid;         // 0xee, the local player's UID, 0xFFFFFFFF when the rival is not found
};
#pragma pack(pop)
CHECK_SIZE(sRivalHoleData, 0xEA);
CHECK_SIZE(sRivalHoles, 0xF2);
CHECK_FIELD(sRivalHoles, uid, 0xEA);
CHECK_FIELD(sRivalHoles, ownerUid, 0xEE);

// intrusion.cpp:287
void CIntrusion::PacketAnalysis(WReceivedPacket& packet) {
	if (m_pDlg != NULL) {
		return;
	}

	unsigned char type = packet.Decode1();
	unsigned char code = packet.Decode1();

	switch (type) {
	case 3: {
		m_roomNumber = packet.Decode2();
		unsigned long elapsed = packet.Decode4();
		unsigned long limit = packet.Decode4();

		packet.DecodeBuffer(&CSharedDoc::Instance()->m_roomInfo, sizeof(sRoomInfo));
		CSharedDoc::Instance()->m_chatManager.FilteringHack(CSharedDoc::Instance()->m_roomInfo.m_title, false);
		CSharedDoc::Instance()->m_mySlot = CSharedDoc::Instance()->m_roomInfo.m_roomNumber;

		m_pDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "IntrusionAlarm", 0);
		m_pDlg->SetCaption("30\xC0\xCE \xB4\xEB\xC8\xB8 \xB0\xD4\xC0\xD3\xC1\xDF \xC0\xD4\xC0\xE5 \xC8\xAE\xC0\xCE"); // "30-player tournament: confirm joining in progress"
		m_pDlg->SetDesc(" \xA1\xD8 '\xBF\xB9'\xB8\xA6 \xB4\xA9\xB8\xA3\xB8\xE9 \xC1\xEF\xBD\xC3 \xB0\xD4\xC0\xD3\xC0\xCC \xBD\xC3\xC0\xDB\xB5\xCB\xB4\xCF\xB4\xD9."); // " * 'Yes' starts the game at once."

		sInfoTime info;
		GetInfoTime(&info, elapsed, limit);

		// "This game started '%2dmin%2dsec' ago and has '%2dmin%2dsec' left.
		// Join the room in progress?"
		char* message = MakeStr("\xC7\xD8\xB4\xE7 \xB0\xD4\xC0\xD3\xC0\xBA \xB0\xD4\xC0\xD3 \xBD\xC3\xC0\xDB \xC8\xC4\n\\c0xffff0000\\c'%2d\xBA\xD0%2d\xC3\xCA'\\c0xff000000\\c\xB0\xA1 \xC1\xF6\xB3\xB5\xB0\xED \\c0xffff0000\\c'%2d\xBA\xD0%2d\xC3\xCA'\\c0xff000000\\c\xB0\xA1\n\xB3\xB2\xBE\xC6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.\n\n\xC1\xF8\xC7\xE0\xC1\xDF\xC0\xCE \xB9\xE6\xBF\xA1 \xC0\xD4\xC0\xE5\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?",
			info.m_elapsedMin, info.m_elapsedSec, info.m_remainMin, info.m_remainSec);

		m_pDlg->SetMessage(message, false);
		m_pDlg->Open((FRESH_RESULT_CALLBACK)&CIntrusion::OnAlarmDlgResult, 1);
		break;
	}
	case 7: {
		std::string nickname = packet.DecodeStr();
		// "(Notice) <nickname> joined the game in progress."
		CChatMsg::Instance()->AddChatMsg(MakeStr("(\xBE\xCB\xB8\xB2) \\c0xffffffff\\c%s\\c0xffff7878\\c\xB4\xD4\xC0\xCC \xB0\xD4\xC0\xD3\xC1\xDF\xBF\xA1 \xC0\xD4\xC0\xE5\xC7\xCF\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.", nickname.c_str()), 0xFFFF7878, true, false);

		packet.DecodeBuffer(&CSharedDoc::Instance()->m_roomInfo, sizeof(sRoomInfo));
		CSharedDoc::Instance()->m_chatManager.FilteringHack(CSharedDoc::Instance()->m_roomInfo.m_title, false);

		unsigned long matchTypeId = CSharedDoc::Instance()->m_roomInfo.m_trophy;
		if (CSharedDoc::Instance()->m_matchTypeId != matchTypeId) {
			IFF_STRUCT::sMatch* pMatch = ItemManager()->FindMatch(matchTypeId);
			if (pMatch) {
				CSharedDoc::Instance()->m_matchTypeId = matchTypeId;
				// "(Notice) The tournament grade changed to [ <match> ]."
				CChatMsg::Instance()->AddChatMsg(MakeStr("(\xBE\xCB\xB8\xB2) \xB4\xEB\xC8\xB8 \xB5\xEE\xB1\xDE\xC0\xCC \\c0xffffffff\\c[ %s ]\\c0xffff7878\\c\xC0\xB8\xB7\xCE \xBA\xAF\xB0\xE6 \xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.", pMatch->m_name), 0xFFFF7878, true, false);
			}
		}

		std::list<sSlotInfo>& players = CSharedDoc::Instance()->m_roomPlayerList;
		players.clear();

		int count = packet.Decode1();
		int param = packet.Decode4();

		for (int i = 0; i < count; i++) {
			sSlotInfo slot;
			packet.DecodeBuffer(&slot, sizeof(slot));

			if (slot.m_attributeBits.roomMaster) {
				slot.m_flags55.flag1 = 1;
			} else {
				slot.m_flags55.flag1 = 0;
			}

			players.push_back(slot);
		}

		PlayersUpdate(param);
		break;
	}
	case 4:
		DoIntrusionAccess(packet, (eIntrusionPermitCode)code);
		break;
	case 5:
		m_pDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "IntrusionAlarm", 0);
		m_pDlg->SetCaption("30\xC0\xCE \xB4\xEB\xC8\xB8 \xB0\xD4\xC0\xD3\xC1\xDF \xC0\xD4\xC0\xE5"); // "30-player tournament: join in progress"
		m_pDlg->SetDesc(" \xA1\xD8 '\xBF\xB9'\xB8\xA6 \xB4\xA9\xB8\xA3\xB8\xE9 \xB4\xD9\xBD\xC3 \xC0\xD4\xC0\xE5\xC0\xBB \xBD\xC3\xB5\xB5\xC7\xD5\xB4\xCF\xB4\xD9."); // " * 'Yes' tries to join again."
		// "Too many players, joining failed. Try again?"
		m_pDlg->SetMessage("\xC0\xAF\xC0\xFA\xB0\xA1 \xB8\xB9\xBE\xC6 \xC0\xD4\xC0\xE5\xBF\xA1 \xBD\xC7\xC6\xD0 \xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9. \xB4\xD9\xBD\xC3 \xBD\xC3\xB5\xB5 \xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);
		m_pDlg->Open((FRESH_RESULT_CALLBACK)&CIntrusion::OnDelayDlgResult, 1);
		break;
	case 6:
		DenyAnlaysis((eIntrusionDenyCode)code);
		break;
	case 9: {
		unsigned long uid = packet.Decode4();
		unsigned long sortGroup = packet.Decode4();
		unsigned long myUid = CSharedDoc::Instance()->GetPangyaUserInfo().guid;

		if (uid == myUid) {
			for (std::vector<sGameResultInfo>::iterator it = CSharedDoc::Instance()->m_gameResultList.begin(); it != CSharedDoc::Instance()->m_gameResultList.end(); ++it) {
				it->m_sortGroup = (unsigned char)sortGroup;
			}
			break;
		}

		sRivalHoles holes;
		memset(&holes, 0, sizeof(holes));

		std::vector<sGameResultInfo>::iterator it = std::find_if(CSharedDoc::Instance()->m_gameResultList.begin(), CSharedDoc::Instance()->m_gameResultList.end(), sFindRival(myUid));
		if (it != CSharedDoc::Instance()->m_gameResultList.end()) {
			sRivalHoleData data;
			memset(&data, 0, sizeof(data));

			memcpy(data.holeScore, it->m_holeScore, sizeof(data.holeScore));
			memcpy(data.holeShots, it->m_holeShots, sizeof(data.holeShots));
			memcpy(data.holePang, it->m_holePang, sizeof(data.holePang));

			holes.data = data;
			holes.uid = uid;
			holes.ownerUid = CSharedDoc::Instance()->GetPangyaUserInfo().guid;
		} else {
			holes.uid = uid;
			holes.ownerUid = 0xFFFFFFFF;
		}

		WSendPacket reply((enumClientPacket)0x9A);
		reply.Encode1(2);
		reply.EncodeBuffer(&holes, sizeof(holes));
		reply.Send(SEND_TO_DEFAULT);
		break;
	}
	case 10:
		RivalDataUpdate(packet);
		break;
	case 8:
		m_calcExpMemberSize = packet.Decode1();
		break;
	}
}

// intrusion.cpp:509
void CIntrusion::RivalDataUpdate(WReceivedPacket& packet) {
	unsigned long uid = packet.Decode4();
	unsigned char totalShots = packet.Decode1();
	unsigned char hole = packet.Decode1();
	int score = packet.Decode4();
	__int64 pang = packet.Decode8();

	sRivalHoles holes;
	memset(&holes, 0, sizeof(holes));
	packet.DecodeBuffer(&holes, sizeof(holes));

	if (uid == CSharedDoc::Instance()->GetPangyaUserInfo().guid) {
		return;
	}

	std::vector<sGameResultInfo>::iterator it = std::find_if(CSharedDoc::Instance()->m_gameResultList.begin(), CSharedDoc::Instance()->m_gameResultList.end(), sFindRival(uid));
	if (it == CSharedDoc::Instance()->m_gameResultList.end()) {
		return;
	}

	it->m_totalShots = totalShots;
	it->m_hole = hole;
	it->m_score = score;
	it->m_pang = pang;

	memcpy(it->m_holeShots, holes.data.holeShots, sizeof(it->m_holeShots));
	memcpy(it->m_holeScore, holes.data.holeScore, sizeof(it->m_holeScore));
	memcpy(it->m_holePang, holes.data.holePang, sizeof(it->m_holePang));

	IActor* pScore = AfxGetTask()->GetActor("Score");
	if (pScore) {
		pScore << MsgObject(0, 0x13C, 0, 0, 0, 0, 0);
	}
	IActor* pSpot = AfxGetTask()->GetActor("Spot");
	if (pSpot) {
		pSpot << MsgObject(0, 0xFB, 0, 0, 0, 0, 0);
	}
}

// intrusion.cpp:559
void CIntrusion::PlayersUpdate(int param) {
	std::list<sSlotInfo>& players = CSharedDoc::Instance()->m_roomPlayerList;
	std::vector<sRivalData>& rivals = CSharedDoc::Instance()->m_gameResultList;

	for (std::list<sSlotInfo>::iterator it = players.begin(); it != players.end(); ++it) {
		unsigned long uid = it->m_uid;
		std::vector<sRivalData>::iterator end = rivals.end();
		std::vector<sRivalData>::iterator found;
		for (found = rivals.begin(); found != end; ++found) {
			if (found->m_uid == uid) {
				break;
			}
		}

		if (found != rivals.end()) {
			// Already in the result list.
			found->m_sortGroup = (unsigned char)param;
			continue;
		}

		if (uid == 0xFFFFFFFF) {
			CSharedDoc::Instance()->m_pGolfDoc->GetPlayer(0)->m_state = 3;
			continue;
		}

		CSharedDoc::Instance()->SetIndex(uid);

		sRivalData rival;
		memset(&rival, 0, sizeof(rival));
		strcpy(rival.m_nickname, it->m_nickname);
		strcpy(rival.m_guildName, it->m_guildName);

		rival.m_id = it->m_ruleUID;
		rival.m_value13F = (unsigned char)param;
		rival.m_sortGroup = (unsigned char)param;
		rival.m_identity = it->m_identity;

		strcpy(rival.m_emblemName, it->m_emblemName);
		rival.m_finishOrder = (unsigned char)players.size();

		rival.m_characterType = it->m_attribute >> 5;

		rival.m_uid = it->m_uid;
		rival.m_hasEmblem = it->m_guildUID;
		rival.m_hole = CSharedDoc::Instance()->m_holeOrder[0];

		rival.m_level = it->m_level;
		rival.m_itemBoostFlags = it->m_itemBoostFlags;
		rival.m_extraFlags = it->m_extraFlags;
		rival.m_ballPos = CSharedDoc::Instance()->m_pGolfDoc->GetHoleData().m_teePos;

		rival.m_team = it->m_attribute & 3;
		rival.m_joined = 1;
		rival.m_rank = 1;
		rival.m_present = 1;
		rival.m_state = 0;
		rival.m_mascotTypeId = it->m_mascotTypeId;
		rival.m_titleTypeId = it->m_titleTypeId;

		rivals.push_back(rival);
	}

	IActor* pScore = AfxGetTask()->GetActor("Score");
	if (pScore) {
		pScore << MsgObject(0, 0x13C, 0, 0, 0, 0, 0);
	}
	IActor* pSpot = AfxGetTask()->GetActor("Spot");
	if (pSpot) {
		pSpot << MsgObject(0, 0xFB, 0, 0, 0, 0, 0);
	}
}

// intrusion.cpp:637
void CIntrusion::GetInfoTime(sInfoTime* pInfo, unsigned long elapsed, unsigned long limit) {
	int elapsedSec = elapsed / 1000;
	int remainSec = limit / 1000 - elapsedSec;

	pInfo->m_elapsedMin = elapsedSec / 60;
	pInfo->m_elapsedSec = elapsedSec - pInfo->m_elapsedMin * 60;
	pInfo->m_remainMin = remainSec / 60;
	pInfo->m_remainSec = remainSec - pInfo->m_remainMin * 60;
}

// intrusion.cpp:651
bool CIntrusion::OnAlarmDlgResult(int result, FrForm* pForm) {
	if (result == 1) {
		WSendPacket packet(GCT_INTRUSION);
		packet.Encode1(1);
		packet.Encode2(m_roomNumber);
		packet.Send(SEND_TO_DEFAULT);

		unsigned long caddieId = CSharedDoc::Instance()->m_myCaddieId;

		std::map<unsigned int, sCaddieInfo>& caddies = CSharedDoc::Instance()->m_caddieMap;
		std::map<unsigned int, sCaddieInfo>::iterator it = caddies.find(caddieId);
		if (it != caddies.end()) {
			// A caddie on vacation with no contract days left is unequipped.
			IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(it->second.typeId);
			if ((it->second.flags & 2) && it->second.days == 0 && pCaddie && pCaddie->fee > 0) {
				caddieId = 0;
			}
		}

		// The equipped character, caddie, club set and ball.
		struct {
			unsigned long characterId;  // 0x0
			unsigned long caddieId;     // 0x4
			unsigned long clubId;       // 0x8
			unsigned long ballId;       // 0xc
		} equip;
		memset(&equip, 0, sizeof(equip));
		equip.characterId = CSharedDoc::Instance()->m_myCharacterId;
		CSharedDoc::Instance()->m_myCaddieId = caddieId;
		equip.caddieId = caddieId;
		equip.clubId = CSharedDoc::Instance()->m_myClubId;
		equip.ballId = CSharedDoc::Instance()->m_myBallId;

		WSendPacket equipPacket((enumClientPacket)0x0C);
		equipPacket.Encode1(7);
		equipPacket.EncodeBuffer(&equip, sizeof(equip));
		equipPacket.Send(SEND_TO_DEFAULT);
	} else {
		AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 0x3B, 0, 0, 0, 0, 0));
		CSharedDoc::Instance()->m_mySlot = 0xFFFF;
	}

	m_pDlg = 0;
	return true;
}

// intrusion.cpp:697
bool CIntrusion::OnDelayDlgResult(int result, FrForm* pForm) {
	if (result != 1) {
		AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 0x3B, 0, 0, 0, 0, 0));
	}
	m_pDlg = 0;
	return true;
}

// intrusion.cpp:714
bool CIntrusion::OnDenyDlgResult(int result, FrForm* pForm) {
	AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 0x3B, 0, 0, 0, 0, 0));
	m_pDlg = 0;
	return true;
}

// intrusion.cpp:721
void CIntrusion::DenyAnlaysis(eIntrusionDenyCode code) {
	if (m_pDlg == NULL) {
		m_pDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "IntrusionDeny", 0);
		m_pDlg->SetCaption("30\xC0\xCE \xB4\xEB\xC8\xB8 \xB0\xD4\xC0\xD3\xC1\xDF \xC0\xD4\xC0\xE5"); // "30-player tournament: join in progress"
		m_pDlg->SetDesc(" \xA1\xD8 \xB0\xD4\xC0\xD3\xC1\xDF \xC0\xD4\xC0\xE5 \xB0\xE1\xB0\xFA"); // " * join-in-progress result"

		switch (code) {
		case 1:
			// "The room is full, you cannot enter."
			m_pDlg->SetMessage("\xC0\xCE\xBF\xF8\xC0\xCC \xB0\xA1\xB5\xE6 \xC2\xF7\xBC\xAD \xB9\xE6\xBF\xA1 \xC0\xD4\xC0\xE5 \xC7\xCF\xBD\xC7 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			break;
		case 2:
			// "You cannot enter a room whose game started more than 5 minutes ago."
			m_pDlg->SetMessage("\xB0\xD4\xC0\xD3 \xBD\xC3\xC0\xDB \xC8\xC4 5\xBA\xD0\xC0\xCC \xC1\xF6\xB3\xAD \xB9\xE6\xBF\xA1\xB4\xC2 \xC0\xD4\xC0\xE5 \xC7\xCF\xBD\xC7 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			break;
		case 3:
			// "You cannot join a private room's game in progress."
			m_pDlg->SetMessage("\xBA\xF1\xB0\xF8\xB0\xB3 \xB9\xE6\xBF\xA1\xB4\xC2 \xB0\xD4\xC0\xD3\xC1\xDF \xC0\xD4\xC0\xE5 \xC7\xCF\xBD\xC7 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			break;
		case 4:
			// "You joined a game in progress before; you cannot enter for a while."
			m_pDlg->SetMessage("\xC0\xCC\xC0\xFC\xBF\xA1 \xB0\xD4\xC0\xD3\xC1\xDF \xC0\xD4\xC0\xE5\xC7\xD1 \xB1\xE2\xB7\xCF\xC0\xCC \xC0\xD6\xBE\xEE\xBC\xAD \xB4\xE7\xBA\xD0\xB0\xA3 \xC0\xD4\xC0\xE5\xC7\xCF\xBD\xC7 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			break;
		case 5:
			// "This room does not allow joining a game in progress."
			m_pDlg->SetMessage("\xC7\xF6\xC0\xE7 \xB0\xD4\xC0\xD3\xC1\xDF \xC0\xD4\xC0\xE5\xC0\xCC \xBA\xD2\xB0\xA1\xB4\xC9\xC7\xD1 \xB9\xE6\xC0\xD4\xB4\xCF\xB4\xD9.", false);
			break;
		case 6:
			// "You played in this room before; you cannot join its game in progress."
			m_pDlg->SetMessage("\xC7\xD8\xB4\xE7 \xB9\xE6\xBF\xA1\xBC\xAD \xC7\xC3\xB7\xB9\xC0\xCC\xC7\xD1 \xB1\xE2\xB7\xCF\xC0\xCC \xC0\xD6\xBE\xEE\xBC\xAD \xB0\xD4\xC0\xD3\xC1\xDF \xC0\xD4\xC0\xE5\xC7\xCF\xBD\xC7 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			break;
		case 7:
			// "You were kicked from this room before; you cannot enter."
			m_pDlg->SetMessage("\xC7\xD8\xB4\xE7 \xB9\xE6\xBF\xA1\xBC\xAD \xB0\xAD\xC1\xA6 \xC5\xF0\xC0\xE5\xB4\xE7\xC7\xD1 \xB1\xE2\xB7\xCF\xC0\xCC \xC0\xD6\xBE\xEE\xBC\xAD \xC0\xD4\xC0\xE5\xC7\xCF\xBD\xC7 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			break;
		}

		m_pDlg->Open((FRESH_RESULT_CALLBACK)&CIntrusion::OnDenyDlgResult, 3);
	}
}

// intrusion.cpp:758
bool CIntrusion::IsIntrusionGame(unsigned short roomNumber) {
	bool result = false;
	std::list<sRoomInfo>& rooms = CSharedDoc::Instance()->m_roomList;
	for (std::list<sRoomInfo>::iterator it = rooms.begin(); it != rooms.end(); ++it) {
		if (it->m_roomNumber == roomNumber) {
			result = it->m_state;
			break;
		}
	}
	return result;
}

// intrusion.cpp:777
void CIntrusion::DoJoinRoom(unsigned short roomNumber) {
	m_roomNumber = roomNumber;

	WSendPacket packet(GCT_INTRUSION);
	packet.Encode1(0);
	packet.Encode2(m_roomNumber);
	packet.Send(SEND_TO_DEFAULT);
}
