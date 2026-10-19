// Original: d:\build\custom\temp\code\client\projectg\shareddoc.cpp
// This unit emits the out-of-line copies of shareddoc.inl's inlines.
#define SHAREDDOC_INLINE
#define SHAREDDOC_GLOBALNETWORKDEFINE_INLINE
#include <atlbase.h>	// stand-in for the precompiled header: _AtlWinModule initializer
#include "SharedDoc.h"
#include "globalgamedefine.h"
#include "GolfDoc.h"
#include "StandingInLine.h"	// its s_standingNames initializer is emitted here
#include "icontentsdoc.h"
#include "contentsdoc.h"
#include "cardsystem.h"
#include "Packet.h"
#include "RealMyRoomData.h"
#include "MascotInfoDlg.h"
#include "shared/s5/SharedUtilities.h"
#include "buddymanager.h"
#include "DetailedRoomInfoDlg.h"
#include "CardDlg.h"
#include "PangGiftDlg.h"
#include "messengerdlg.h"
#include "UccManager.h"
#include "UccLibrary.h"
#include "commonutil.h"
#include "golfball.h"
#include "NetworkSystem.h"
#include "TaskManager.h"
#include "LobbyTask.h"
#include "GolfTask.h"
#include "exhibition.h"
#include "ShopTask.h"
#include "frwnd.h"
#include "frcombobox.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binstr.h"
#include "club.h"
#include "SharedDoc.inl"
#include "client/Wangreal/include/westpak.h"
#include "client/Wangreal/include/wresrcmng.h"
#include "partsinfo.h"

void __fastcall LineParse(const char *line, std::vector<std::string> &tokens);

// The player level table of the exe, 71 records of 5 dwords (the first is the level's exp).
extern "C" const unsigned long data_9e6468[71][5];

// The room titles GetDefaultTitle picks from at random, by game kind. The
// original initializes them dynamically (sub_00989900 to sub_009899f0, one
// per array): each array's first title is a constant, which makes the
// initializer non-constant (docs/tough_problems/010). Battle and approach
// share their first title.
static const char *const s_titleVs = "\xc6\xce\xbe\xdf~ \xc6\xce\xbe\xdf!"; // 팡야~ 팡야!
static const char *const s_titleMass = "\xc8\xa6\xc0\xce\xbf\xf8\xc0\xbb \xb3\xbb \xc7\xb0\xbe\xc8\xbf\xa1"; // 홀인원을 내 품안에
static const char *const s_titleOneRound = "\xbd\xac\xbe\xee\xb0\xa1\xb4\xc2 \xc7\xd1 \xc6\xc7!"; // 쉬어가는 한 판!
static const char *const s_titleChat = "\xbc\xd2\xb3\xe2, \xbc\xd2\xb3\xe0\xb8\xa6 \xb8\xb8\xb3\xaa\xb4\xd9"; // 소년, 소녀를 만나다

// shareddoc.cpp:52
const char *defaultTitles_vs[] = {
	s_titleVs,
	"\xb3\xca\xb6\xfb \xb3\xaa\xb6\xfb \xc6\xce\xbe\xdf~", // 너랑 나랑 팡야~
	"\xb9\xd9\xb6\xf7 \xc1\xc1\xc0\xba \xb9\xe6~", // 바람 좋은 방~
	"\xc6\xce\xbe\xdf\xbf\xcd \xc7\xd4\xb2\xb2\xb6\xf3\xb8\xe9~!", // 팡야와 함께라면~!
	"\xbf\xb9\xbb\xdb \xbd\xba\xc0\xae, \xb8\xda\xc1\xf8 \xbc\xa6!", // 예쁜 스윙, 멋진 샷!
	"\xb3\xaf\xbe\xc6\xb6\xf3 \xbe\xc6\xc1\xee\xc5\xd8~", // 날아라 아즈텍~
	"\xc0\xfa \xc7\xcf\xb4\xc3\xc0\xc7 \xc6\xce\xc0\xcc \xb5\xc7\xbe\xee~", // 저 하늘의 팡이 되어~
	"\xc6\xce\xbe\xdf\xb0\xa1 \xc3\xd6\xb0\xed!", // 팡야가 최고!
	"\xb4\xd9\xc7\xd4\xb2\xb2 \xc6\xae\xb7\xb9\xc0\xfa \xc7\xe5\xc6\xae!", // 다함께 트레저 헌트!
};

// shareddoc.cpp:70
const char *defaultTitles_mass[] = {
	s_titleMass,
	"\xc6\xce\xbe\xdf\xb4\xeb\xc8\xb8\xb7\xce \xb8\xf0\xc0\xcc\xbc\xbc\xbf\xe4~", // 팡야대회로 모이세요~
	"\xb8\xf0\xb5\xce\xb8\xf0\xb5\xce \xc6\xce\xbe\xdf!", // 모두모두 팡야!
	"\xb1\xe2\xb7\xcf \xb0\xbb\xbd\xc5\xc0\xc7 \xb1\xd7\xb3\xaf\xb1\xee\xc1\xf6!", // 기록 갱신의 그날까지!
	"1\xb5\xee \xb1\xe2\xb4\xd9\xb7\xc1~", // 1등 기다려~
	"\xc6\xae\xb7\xce\xc7\xc7\xb4\xc2 \xb3\xbb\xb2\xa8~", // 트로피는 내꺼~
};

// shareddoc.cpp:82
const char *defaultTitles_battle[] = {
	s_titleOneRound,
	"\xc7\xd1\xb9\xe6\xbd\xc2\xba\xce! \xc6\xce\xb9\xe8\xc6\xb2~", // 한방승부! 팡배틀~
	"\xb4\xeb\xb9\xda \xb2\xde, \xbd\xf1\xbe\xc6\xc1\xf6\xb4\xc2 \xc6\xce", // 대박 꿈, 쏟아지는 팡
	"\xbb\xf5\xb7\xce\xbf\xee \xbd\xc3\xc1\xf0 \xc6\xce\xb9\xe8\xc6\xb2 \xc7\xd1 \xc6\xc7!", // 새로운 시즌 팡배틀 한 판!
};

// shareddoc.cpp:91
const char *defaultTitles_approach[] = {
	s_titleOneRound,
	"\xc7\xd1\xb9\xe6\xbd\xc2\xba\xce! \xbe\xee\xc7\xc1\xb7\xce\xc4\xa1~", // 한방승부! 어프로치~
	"\xbd\xc3\xc1\xf0 NEW \xbe\xee\xc7\xc1\xb7\xce\xc4\xa1", // 시즌 NEW 어프로치
};

// shareddoc.cpp:100
const char *defaultTitles_chat[] = {
	s_titleChat,
	"\xbc\xf6\xb4\xd9\xc0\xef\xc0\xcc, \xbb\xf5\xc4\xa7\xb6\xbc\xb1\xe2", // 수다쟁이, 새침떼기
	"\xc0\xce\xbf\xac\xc0\xcc \xb8\xf0\xc0\xcc\xb4\xc2 \xb0\xf7", // 인연이 모이는 곳
	"\xc6\xce\xbe\xdf! \xc1\xf1\xb0\xc5\xbf\xee \xb8\xb8\xb3\xb2~", // 팡야! 즐거운 만남~
	"\xc6\xce\xbe\xdf\xbf\xf9\xb5\xe5\xbf\xcd \xc7\xd4\xb2\xb2!", // 팡야월드와 함께!
	"\xc6\xce\xbe\xdf\xc0\xce\xb5\xe9 \xb4\xd9 \xb8\xf0\xbf\xa9\xb6\xf3~!", // 팡야인들 다 모여라~!
	"\xc4\xf0 \xb0\xa1\xc0\xcc! \xb3\xaa\xc0\xcc\xbd\xba \xb0\xc9!", // 쿨 가이! 나이스 걸!
};

// shareddoc.h:534
void CSharedDoc::HaveAngelWing(bool have) {
	m_haveAngelWing = have;
}

// shareddoc.h:559
eStateTrade CSharedDoc::GetStateTrade(void) {
	return m_stateTrade;
}

// shareddoc.h:564
std::list<sTradeItem> *CSharedDoc::GetListTradeItem(void) {
	return &m_listTradeItem;
}

// shareddoc.h:574
std::list<sStall> *CSharedDoc::GetListStallItem(void) {
	return &m_listStallItem;
}

// shareddoc.h:575
sStall *CSharedDoc::GetStallItem(void) {
	return &m_stallItem;
}

// shareddoc.h:608
void CSharedDoc::SetTradeMode(unsigned char mode) {
	m_tradeMode = mode;
}

// shareddoc.h:1009
void CSharedDoc::SetHaveHalloweenItem(bool have) {
	m_haveHalloweenItem = have;
}

// shareddoc.h:1010
void CSharedDoc::SetWearHalloweenItem(bool wear) {
	m_wearHalloweenItem = wear;
}

// shareddoc.h:1144
void CSharedDoc::SetUserInfoMod(int mod) {
	m_userInfoMod = mod;
}

// globalgamedefine.h:1799
sUserPosition::sUserPosition(void) {
	m_roomNumber = 0xFFFF;
	m_value2 = 0xFFFFFFFF;
	m_userValue = 0xFFFFFFFF;
	m_channelId = 0xFF;
	memset(m_channelName, 0, sizeof(m_channelName));
}

// globalgamedefine.h:3549
void sFriend::Clear(void) {
	memset(nick, 0, sizeof(nick));
	memset(group, 0, sizeof(group));
	uid = 0xFFFFFFFF;

	value25 = 0xFFFFFFFF;
	value29 = 0;
	value2D = 0xFFFFFFFF;

	memset(data31, 0, sizeof(data31));

	memset(extra.data, 0, sizeof(extra.data));
	extra.index = 0xFF;
	extra.code = 0xFFFF;
	extra.id1 = 0xFFFFFFFF;
	extra.id2 = 0xFFFFFFFF;

	status = 5;
	channel = 0xFF;
	value8A = 0;
	flags = 0;
	listFlags = 0;
}

// globalnetworkdefine.h:2837
sStall::sStall(void) {
	AllClear();
}

// shareddoc.cpp:123
CSharedDoc::CSharedDoc(void)
	: m_curMap(0), m_pCurCourse(NULL), m_roomType(0), m_roomName(), m_holes(18), m_weather(0),
	  m_shotTimeLimit(0), m_gameTimeLimit(0), m_value4CE4(8), m_string4D4C(""),
	  m_loginDlgCreated(true) {
	m_pGolfDoc = NULL;
	ClearAll();

	m_value47CC = 0;
	m_flag47D0 = false;

	memset(m_fileName, 0, sizeof(m_fileName));

	m_replayState = 0;
	m_replayPrevAccuracy = 0.0f;
	m_replayStartA = 0;
	m_replayChannelFlags = 0;
	m_replayPlayType = -1;
	m_playingMode = (eReplayModeType)1;
	m_replayStartB = 0;
	m_shotCountKey = 0;

	m_value5BD4 = 6385;
	m_rmrMode = 0;

	m_serverTimeTick = GetTickCount();

	m_directMoveUID = 0xFFFFFFFF;
	m_myCash = 0;
	m_flag5D0C = false;
	m_goStopMode = false;
	m_outsideCash = 0;
	m_myBonusCash = 0;
	m_directMoveServer = 0xFF;
	m_directMoveRoom = 0xFFFF;
}

// shareddoc.cpp (no line marks in the original); defined after the
// CSharedDoc constructor, which calls it rather than inlining it.
sUserInfo::sUserInfo(void)
	: m_characterInfo() {
}

// shareddoc.cpp:271
CSharedDoc::~CSharedDoc(void) {
	for (std::map<unsigned long, sRoomSlot>::iterator it = WSingleton<CSharedDoc>::Instance()->m_roomSlotMap.begin(); it != m_roomSlotMap.end(); ++it) {
		if (it->second.m_pPet != 0) {
			delete it->second.m_pPet;
			it->second.m_pPet = 0;
		}
	}
	m_roomSlotMap.clear();

	ClearRivalVars(false);
	UccManager()->Clear();
}

// shareddoc.cpp:289
void CSharedDoc::ClearAll(void) {
	ClearCommonVars();
	ClearInventory();
	ClearGameVars();
	ClearRivalVars(false);
	InitGameType();
	ClearLobbyStateVars();
	InitTradeData();

	ClearOldSeasonStat();
	ClearServerVars();
	ClearJapanVars();
	memset(m_realEquip, 0, sizeof(m_realEquip));

	if (IsLocalContent((localContentType_t)0x57)) {
		ClearUcc();
	}

	if (IsLocalContent((localContentType_t)0x39)) {
		m_mapEventList.clear();
	}
	if (IsLocalContent((localContentType_t)0x4F)) {
		m_shotDataList.clear();
	}

	m_userInfoMod = 0;
}

// shareddoc.cpp:321
void CSharedDoc::ClearUcc(void) {
	m_myItemList.clear();
	m_myAuxItemList.clear();
	UccManager()->Clear();
}

// shareddoc.cpp:329
void CSharedDoc::ClearJapanVars(void) {
	m_japanState = 0;
	m_billingType = 0xFFFFFFFF;

	m_japanFlag = 0;
	m_japanParam1 = 0;
	m_japanParam2 = 0;

	char commandLine[MAX_PATH] = { 0 };

	// The command line: the exe path, the account, the login server address.
	strcpy(commandLine, GetCommandLineA());

	char args[3][MAX_PATH];
	for (int i = 0; i < 3; i++) {
		args[i][0] = 0;
	}

	char *rest;
	if (commandLine[0] == '"') {
		rest = GetStr(commandLine, args[0]);
	} else {
		sscanf(commandLine, "%s", args[0]);
		rest = commandLine + strlen(args[0]);
	}

	sscanf(rest, "%s %s", args[1], args[2]);

	char unquoted[MAX_PATH];
	if (args[1][0] != 0) {
		int len = strlen(args[1]);
		if (args[1][0] == '"' && args[1][len - 1] == '"') {
			strncpy(unquoted, args[1] + 1, len - 2);
			unquoted[len - 2] = 0;
			strcpy(args[1], unquoted);
			WSingleton<CSharedDoc>::Instance()->m_commandLineAccount = unquoted;
		} else {
			WSingleton<CSharedDoc>::Instance()->m_commandLineAccount = args[1];
		}
	}

	int len = strlen(args[2]);
	if (args[2][0] == '"' && args[2][len - 1] == '"') {
		strncpy(unquoted, args[2] + 1, len - 2);
		unquoted[len - 2] = 0;
		strcpy(args[2], unquoted);
		WSingleton<CSharedDoc>::Instance()->m_loginServerAddress = unquoted;
	} else {
		WSingleton<CSharedDoc>::Instance()->m_loginServerAddress = args[2];
	}
}

// shareddoc.cpp:418
void CSharedDoc::ClearCommonVars(void) {
	m_loginConnectValue = 0;
	m_gameOver = false;
	m_gameKind = 0;
	m_gameMode = 0;
	m_matchTypeId = 0;
	m_value456C = 0;
	m_winningTeam = 2;
	m_pangRate = 1.0f;
	m_pangPercent = 100;
	m_offlinePlay = 0;
	m_quitGamePang = 0;
	m_value4D7E = 0xFF;
	m_value4D70 = 0;
	m_value4D74 = 0;

	m_newMapEvent = false;
	m_newMapEventFlags = 0;
	m_flag4D84 = false;
	m_value4D88 = -1;

	memset(m_tutorialData, 0, sizeof(m_tutorialData));
	m_value4D98 = 15;
	m_gameServerPort = 0;
	m_gameServerUID = 0;
	m_value80 = 0;
	m_value88 = 0;

	LoadLevelTable();

	// The hole order runs 1 to 18 and ends with 19 in the byte after the array.
	unsigned char i;
	for (i = 0; i < 18; i++) {
		m_holeRandom[i] = rand();
		m_holePin[i] = (unsigned char)(m_holeRandom[i] % 3);
		m_holeOrder[i] = i + 1;
	}
	m_holeOrder[i] = 19;

	m_noteList.clear();

	memset(&m_serverTime, 0, sizeof(m_serverTime));

	m_value4D04 = 0;

	m_logoutPending = false;
	memset(m_myAuxPartTidList, 0, sizeof(m_myAuxPartTidList));
	m_value5018 = 0;
	m_matchGameCount = 0;
	m_flag5020 = false;
	m_haveHalloweenItem = false;
	m_wearHalloweenItem = false;
	m_lastGiftReceiverNick = "";

	m_lastGiftReceiverUID = -1;

	m_tutorialFinish = 0;
	m_value4DBD = 0;
	m_value4DBE = 0;
	m_friendMap.clear();
	memset(&m_myFriendInfo, 0, sizeof(m_myFriendInfo));
	m_value94 = 0;
	m_controlServerService = 0;
	m_serverProperty.Reset();
	m_flagAbilityItem = 0;
	m_numBonusPangItem = 0;
	m_value5044 = 0;
	m_value5048 = 0;
	m_value504C = 0;
	m_value5050 = 1;
	m_value5055 = 0;
	m_value5056 = 0;
	m_value5058 = 0;
	m_value5054 = 0;
	m_value505A = 0;
	m_value5064 = 0;
	m_pointEventState = 0;
	m_pointEventFlag = false;
	m_value506C = 0;
	IsLocalContent((localContentType_t)0x39);
	m_value5D08 = 0;
	m_flag50F4 = false;
	m_flag50F6 = false;
	m_flag5BDC = false;
	m_myRoomOwnerUID = 0xFFFFFFFF;
	if (IsLocalContent((localContentType_t)0x81)) {
		m_forceNicknameChange = 0;
	}
	m_pPeriodDoc = NULL;
}

// shareddoc.cpp:560
void CSharedDoc::ClearInventory(void) {
	// The local player's block from 0x3E0 to 0xD4C (one struct in the original).
	memset(&m_mySlot, 0, 0xD4C - 0x3E0); m_myCash = 0;

	m_outsideCash = 0;
	m_myBonusCash = 0;

	m_stringMap.clear();
	m_characterMap.clear();
	m_caddieMap.clear();
	m_myMascotMap.clear();
	m_myPartsList.clear();
	m_myItemMap.clear();
	m_myGearList.clear();
	m_itemInfoList.clear();
	m_replayFileList.clear();
	m_myExtraItemList.clear();
	m_myExtraItemList2.clear();
	m_giftList.clear();
	m_mySkinList.clear();
	m_mySpecialTrophies.clear();
	m_myGuildTrophies.clear();
	m_myExtraItemList3.clear();
	m_myExtraItemList4.clear();
	m_questList.clear();
	m_furnitureList.clear();
	m_list1D0.clear();
	m_list1DC.clear();

	ClearUserInfoTimeMap(0xFFFFFFFF);
}

// shareddoc.cpp:595
void CSharedDoc::ClearGameMode(void) {
	m_gameMode = 0;
}

// shareddoc.cpp:601
void CSharedDoc::ClearUserInfoTimeMap(unsigned long uid) {
	if (uid == 0xFFFFFFFF) {
		std::map<unsigned long, sUserInfoTime>::iterator it = m_userInfoTimeMap.begin();
		for (; it != m_userInfoTimeMap.end(); ++it) {
			sUserInfoTime &info = (*it).second;
			if (info.m_pArray1 != NULL) {
				delete[] info.m_pArray1;
				info.m_pArray1 = NULL;
			}
			if (info.m_pArray2 != NULL) {
				delete[] info.m_pArray2;
				info.m_pArray2 = NULL;
			}
			if (info.m_pArray3 != NULL) {
				delete[] info.m_pArray3;
				info.m_pArray3 = NULL;
			}
			if (info.m_pArray4 != NULL) {
				delete[] info.m_pArray4;
				info.m_pArray4 = NULL;
			}
		}
		m_userInfoTimeMap.clear();
	} else {
		std::map<unsigned long, sUserInfoTime>::iterator it;
		it = m_userInfoTimeMap.find(uid);
		if (it != m_userInfoTimeMap.end()) {
			sUserInfoTime &info = (*it).second;
			if (info.m_pArray1 != NULL) {
				delete[] info.m_pArray1;
				info.m_pArray1 = NULL;
			}
			if (info.m_pArray2 != NULL) {
				delete[] info.m_pArray2;
				info.m_pArray2 = NULL;
			}
			if (info.m_pArray3 != NULL) {
				delete[] info.m_pArray3;
				info.m_pArray3 = NULL;
			}
			if (info.m_pArray4 != NULL) {
				delete[] info.m_pArray4;
				info.m_pArray4 = NULL;
			}
			m_userInfoTimeMap.erase(it);
		}
	}
}

// shareddoc.cpp:635
void CSharedDoc::ClearGameVars(void) {
	m_value90 = 0;
	m_value4843 = 0;
	m_value4844 = 0;
	memset(m_gameStatistics, 0, sizeof(m_gameStatistics));
	memset(m_teamResult, 0, sizeof(m_teamResult));
	memset(m_teamPoint, 0, sizeof(m_teamPoint));

	memset(m_value47B4, 0, sizeof(m_value47B4));

	// Written out, not as a loop: a loop leaves cl enough inline budget to
	// inline _Erase into the clear() below (docs/tough_problems/028).
	m_gameStatistics[3].exp = 0xFFFFFFFF;
	m_gameStatistics[2].exp = 0xFFFFFFFF;
	m_gameStatistics[1].exp = 0xFFFFFFFF;
	m_gameStatistics[0].exp = 0xFFFFFFFF;
	m_value4D7C = 0;
	m_value456C = 0;
	m_winningTeam = 2;
	m_indexMap.clear();
}

// shareddoc.cpp:657
void CSharedDoc::ClearRivalVars(bool keepPlayers) {
	if (!keepPlayers)
		memset(m_roomPlayers, 0, sizeof(m_roomPlayers));

	memset(m_trophyUID, 0, sizeof(m_trophyUID));

	m_gameResultList.clear();
	m_vector4524.clear();
	m_guildMatchFlags.clear();
}

// shareddoc.cpp:670
int CSharedDoc::GetBuffValue(int target, int type) {
	if (type != 0x14 && type != 0x10)
		return -1;

	return 0;
}

// shareddoc.cpp:692
void CSharedDoc::ClearOldSeasonStat(void) {
	memset(&m_oldSeasonStatistics, 0, sizeof(m_oldSeasonStatistics));
	memset(&m_oldSeasonTrophy, 0, sizeof(m_oldSeasonTrophy));

	m_specialTrophies.clear();
	m_guildTrophies.clear();
}

// shareddoc.cpp:701
void CSharedDoc::ClearStatistics(void) {
}

// shareddoc.cpp:705
void CSharedDoc::ClearCurrentChannel(bool notify) {
	memset(&m_currentChannel, 0, sizeof(m_currentChannel));
	m_currentChannel.id = 0xFF;
}

// shareddoc.cpp:711
void CSharedDoc::ClearServerVars(void) {
	memset(&m_currentChannel, 0, sizeof(m_currentChannel)); m_currentChannel.id = 0xFF;
	memset(&m_curServer, 0, sizeof(m_curServer));
	m_gameServerList.clear();
	m_channelList.clear();
}

// shareddoc.cpp:719
void CSharedDoc::ClearLobbyStateVars(void) {
	m_lobbyMessage = "";
	m_paranNickChanged = false;
	m_noticeShown = true;
	m_lobbyStateValue = 0;
	m_3DBackgroundOff = false;
	m_lobbyStateFlag = false;
	m_lobbyStateFlag2 = false;

	memset(&m_roomInfo, 0, sizeof(m_roomInfo));

	sRoomInfo roomInfo;
	m_roomInfo = roomInfo;

	m_userMap.clear();
	m_roomList.clear();
	m_roomPlayerList.clear();
	m_roomPlayerList2.clear();
	m_roomUserList.clear();
	m_guildMembers.clear();
}

// shareddoc.cpp:742
void CSharedDoc::ClearRealEquip(void) {
	memset(m_realEquip, 0, sizeof(m_realEquip));
}

// shareddoc.cpp:747
// The game types' room names (cp949), player limits and hole counts.
void CSharedDoc::InitGameType(void) {
	// "스트로크" (stroke)
	m_gameTypeNames[0].maxPlayers = 4;
	m_gameTypeNames[0].defaultPlayers = 2;
	m_gameTypeNames[0].defaultHoles = 3;
	m_gameTypeNames[0].name = "\xBD\xBA\xC6\xAE\xB7\xCE\xC5\xA9";

	// "매치" (match)
	m_gameTypeNames[1].maxPlayers = 4;
	m_gameTypeNames[1].defaultPlayers = 2;
	m_gameTypeNames[1].defaultHoles = 6;
	m_gameTypeNames[1].name = "\xB8\xC5\xC4\xA1";

	// "개인 래더" (ladder)
	m_gameTypeNames[3].maxPlayers = 2;
	m_gameTypeNames[3].defaultPlayers = 2;
	m_gameTypeNames[3].defaultHoles = 9;
	m_gameTypeNames[3].name = "\xB0\xB3\xC0\xCE \xB7\xA1\xB4\xF5";

	// "30인 대회" (30-player tourney)
	m_gameTypeNames[4].maxPlayers = 30;
	m_gameTypeNames[4].defaultPlayers = 10;
	m_gameTypeNames[4].defaultHoles = 9;
	m_gameTypeNames[4].name = "30\xC0\xCE \xB4\xEB\xC8\xB8";

	// "30인 대회 팀전" (30-player team tourney)
	m_gameTypeNames[5].maxPlayers = 30;
	m_gameTypeNames[5].defaultPlayers = 10;
	m_gameTypeNames[5].defaultHoles = 9;
	m_gameTypeNames[5].name = "30\xC0\xCE \xB4\xEB\xC8\xB8 \xC6\xC0\xC0\xFC";

	// "길드 대전" (guild battle)
	m_gameTypeNames[6].maxPlayers = 30;
	m_gameTypeNames[6].defaultPlayers = 10;
	m_gameTypeNames[6].defaultHoles = 9;
	m_gameTypeNames[6].name = "\xB1\xE6\xB5\xE5 \xB4\xEB\xC0\xFC";

	// "공개 마이룸" (open my room)
	m_gameTypeNames[8].maxPlayers = 30;
	m_gameTypeNames[8].defaultPlayers = 1;
	m_gameTypeNames[8].defaultHoles = 1;
	m_gameTypeNames[8].name = "\xB0\xF8\xB0\xB3 \xB8\xB6\xC0\xCC\xB7\xEB";

	// "팡배틀" (pang battle)
	m_gameTypeNames[7].maxPlayers = 4;
	m_gameTypeNames[7].defaultPlayers = 2;
	m_gameTypeNames[7].defaultHoles = 6;
	m_gameTypeNames[7].name = "\xC6\xCE\xB9\xE8\xC6\xB2";

	// "어프로치" (approach)
	m_gameTypeNames[9].maxPlayers = 30;
	m_gameTypeNames[9].defaultPlayers = 10;
	m_gameTypeNames[9].defaultHoles = 1;
	m_gameTypeNames[9].name = "\xBE\xEE\xC7\xC1\xB7\xCE\xC4\xA1";

	m_gameTypeNames[10].maxPlayers = 30;
	m_gameTypeNames[10].defaultPlayers = 6;
	m_gameTypeNames[10].defaultHoles = 3;
	m_gameTypeNames[10].name = "\xBE\xEE\xC7\xC1\xB7\xCE\xC4\xA1";

	// "대화방" (chat room)
	m_gameTypeNames[2].maxPlayers = 30;
	m_gameTypeNames[2].defaultPlayers = 10;
	m_gameTypeNames[2].defaultHoles = 1;
	m_gameTypeNames[2].name = "\xB4\xEB\xC8\xAD\xB9\xE6";

	// "처음 시작하기" (first steps)
	m_gameTypeNames[11].maxPlayers = 1;
	m_gameTypeNames[11].defaultPlayers = 1;
	m_gameTypeNames[11].defaultHoles = 1;
	m_gameTypeNames[11].name = "\xC3\xB3\xC0\xBD \xBD\xC3\xC0\xDB\xC7\xCF\xB1\xE2";

	// "캐디와 배우기" (learn with the caddie)
	m_gameTypeNames[12].maxPlayers = 1;
	m_gameTypeNames[12].defaultPlayers = 1;
	m_gameTypeNames[12].defaultHoles = 1;
	m_gameTypeNames[12].name = "\xC4\xB3\xB5\xF0\xBF\xCD \xB9\xE8\xBF\xEC\xB1\xE2";

	// "스트로크" (stroke)
	m_gameTypeNames[13].maxPlayers = 0xFF;
	m_gameTypeNames[13].defaultPlayers = 1;
	m_gameTypeNames[13].defaultHoles = 18;
	m_gameTypeNames[13].name = "\xBD\xBA\xC6\xAE\xB7\xCE\xC5\xA9";

	// "불가사의한 혼돈" (mysterious chaos)
	m_gameTypeNames[14].maxPlayers = 30;
	m_gameTypeNames[14].defaultPlayers = 30;
	m_gameTypeNames[14].defaultHoles = 18;
	m_gameTypeNames[14].name = "\xBA\xD2\xB0\xA1\xBB\xE7\xC0\xC7\xC7\xD1 \xC8\xA5\xB5\xB7";
}

// shareddoc.cpp:827
void CSharedDoc::LoadLevelTable(void) {
	for (int i = 0; i < 71; i++) {
		m_levelExp[i].m_exp = data_9e6468[i][0];
		m_levelExp[i].m_totalExp = m_levelExp[i].m_exp + (i == 0 ? 0 : m_levelExp[i - 1].m_totalExp);
	}

	m_caddieLevelExp[0].m_exp = 0x208;
	m_caddieLevelExp[0].m_value4 = 0x208;
	m_caddieLevelExp[1].m_exp = 0x2A8;
	m_caddieLevelExp[1].m_value4 = 0x4B0;
	m_caddieLevelExp[2].m_exp = 0x348;
	m_caddieLevelExp[2].m_value4 = 0x7F8;
}

// shareddoc.cpp:843
void CSharedDoc::LoadItemDb(void) {
	m_itemManager.Load();
	CPartTidList::SetItemManager(&m_itemManager);
}

// shareddoc.cpp:850
void CSharedDoc::InitGolfDoc(void) {
	IFF_STRUCT::sCourse *course = ItemManager()->FindCourse(0x28000000);
	if (course != 0) {
		WSingleton<CSharedDoc>::Instance()->m_curMap = 0;
		WSingleton<CSharedDoc>::Instance()->m_pCurCourse = course;
	}
	WSingleton<CSharedDoc>::Instance()->m_roomType = 0;
	strcpy(WSingleton<CSharedDoc>::Instance()->m_roomName, WSingleton<CSharedDoc>::Instance()->m_gameTypeNames[0].name);
	WSingleton<CSharedDoc>::Instance()->m_gameKind = 0;
	WSingleton<CSharedDoc>::Instance()->m_holes = 18;
	WSingleton<CSharedDoc>::Instance()->m_weather = 0;
	WSingleton<CSharedDoc>::Instance()->m_shotTimeLimit = 40000;
	WSingleton<CSharedDoc>::Instance()->m_gameTimeLimit = 0;
}

// shareddoc.cpp:861
void CSharedDoc::LoadChatFilter(void) {
	m_chatManager.Load();
}

// shareddoc.cpp:866, whether the player with the UID is in the current game
// (the golf doc's players) or in its results (the result kinds).
bool CSharedDoc::AreWePlayingTogether(unsigned long uid) {
	switch (m_roomType) {
	case 0:
	case 1:
	case 3:
	case 7:
		if (GOLFDOC() == 0)
			break;

		for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			if (GOLFDOC()->GetPlayer(i)->m_state == 3)
				continue;

			if (GOLFDOC()->GetPlayer(i)->m_uid == uid)
				return true;
		}
		break;

	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
		for (std::vector<sGameResultInfo>::iterator it = m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
			if (it->m_uid == uid)
				return it->m_state != 3;
		}
		break;
	}

	return false;
}

// shareddoc.cpp:913, stores the hole result prizes of the player with the UID
bool CSharedDoc::SetWinningPrize(unsigned long uid, const sWinningPrize &prize) {
	if (GOLFDOC() == 0)
		return false;

	for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (PLAYER(i)->m_uid == uid) {
			PLAYER(i)->m_prizeItems = prize;
			return true;
		}
	}

	return false;
}

// shareddoc.cpp:962, appends count extra prizes to the player with the UID
bool CSharedDoc::SetExtPrize(unsigned long uid, int count, sPrizeInfo *pPrizes) {
	if (GOLFDOC() == 0)
		return false;

	for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (PLAYER(i)->m_uid == uid) {
			if (count > 0) {
				int added = 0;
				for (unsigned int slot = 0; slot < 128; slot++) {
					if (slot >= PLAYER(i)->m_extPrizeCount && added < count) {
						PLAYER(i)->m_extPrizes[slot] = pPrizes[added++];
					}
				}
			}
			PLAYER(i)->m_extPrizeCount += count;
			return true;
		}
	}

	return false;
}

// shareddoc.cpp:1021, whether the player holds every material of the compound
// recipe in the needed count.
bool CSharedDoc::CanCompound(unsigned long typeId) {
	IFF_STRUCT::sQuest *pQuest = m_itemManager.FindQuest(typeId);
	if (pQuest == 0) {
		return false;
	}

	for (int i = 0; i < 5; i++) {
		unsigned long materialId = pQuest->m_materialIds[i];
		if (materialId == 0) {
			continue;
		}

		std::list<sItemInfo>::iterator it;
		for (it = m_myExtraItemList4.begin(); it != m_myExtraItemList4.end(); ++it) {
			if (materialId == it->m_typeId) {
				if (it->m_count < pQuest->m_materialCounts[i]) {
					return false;
				}
				break;
			}
		}

		if (it == m_myExtraItemList4.end()) {
			return false;
		}
	}

	return true;
}

// shareddoc.cpp:1054, the sum (property 0) or product (properties 1-3, in
// percent) of what the aux parts a player wears add to a property.
float CSharedDoc::GetAuxPartProperty(unsigned char team, unsigned char property) {
	if (m_pGolfDoc != 0) {
		if (WSingleton<CSharedDoc>::Instance()->m_pGolfDoc->GetPlayerNum() <= team) {
			if (property == 0) {
				return 0.0f;
			}

			return 1.0f;
		}
	}

	unsigned long *pTids = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[team].m_characterInfo.auxParts.tids;

	float value = (property == 0) ? 0.0f : 1.0f;
	for (int i = 0; i < 5; i++) {
		if (pTids[i] == 0) {
			continue;
		}

		IFF_STRUCT::sAuxPart *pAuxPart = ItemManager()->FindAuxPart(pTids[i]);
		if (pAuxPart == 0) {
			continue;
		}

		switch (property) {
		case 0:
			value += (float)pAuxPart->m_propertyAdd;
			break;
		case 1:
			if (pAuxPart->m_propertyRate[0] > 0) {
				value = (float)pAuxPart->m_propertyRate[0] * value * 0.01f;
			}
			break;
		case 2:
			if (pAuxPart->m_propertyRate[1] > 0) {
				value = (float)pAuxPart->m_propertyRate[1] * value * 0.01f;
			}
			break;
		case 3:
			if (pAuxPart->m_propertyRate[2] > 0) {
				value = (float)pAuxPart->m_propertyRate[2] * value * 0.01f;
			}
			break;
		case 4:
			if ((pAuxPart->m_typeId & 0xFFFF) == 9) {
				value = 9.0f;
			} else if (IsLocalContent((localContentType_t)0x4B) && (pAuxPart->m_typeId & 0xFFFF) == 0x0B) {
				value = 11.0f;
			}
			break;
		}
	}

	return value;
}

// shareddoc.cpp:1117, the starting combo gauge a team's player gets from
// wearing one of the Pangya Peppermint parts.
float CSharedDoc::GetWtPepPangyaComboGauge(unsigned char team) {
	unsigned long pepPangyaParts[20] = {
		0x8022808, 0x8022809, 0x8062808, 0x8062809, 0x80A6808,
		0x80A6809, 0x80E2808, 0x80E2809, 0x8128808, 0x8128809,
		0x816A808, 0x816A809, 0x81A0808, 0x81A0809, 0x81E6808,
		0x81E6809, 0x8222808, 0x8222809, 0x8262808, 0x8262809
	};
	float gauge = 0.0f;

	for (int i = 0; i < 24; i++) {
		unsigned long typeId = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[team].m_characterInfo.partTypeIds[i];

		for (int j = 0; j < sizeof(pepPangyaParts) / sizeof(pepPangyaParts[0]); j++) {
			if (pepPangyaParts[j] == typeId) {
				gauge = 16.0f;
				break;
			}
		}
		if (gauge != 0.0f) {
			break;
		}
	}

	return gauge;
}

// shareddoc.cpp:1167, whether the player of a team (every player of the team
// in a team room) wears the part.
bool CSharedDoc::FindEquipPart(unsigned char team, unsigned long typeId) {
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
		for (int i = 0; i < WSingleton<CSharedDoc>::Instance()->m_pGolfDoc->GetPlayerNum(); i++) {
			if (WSingleton<CSharedDoc>::Instance()->m_pGolfDoc->GetPlayer(i)->m_attribute == team) {
				for (int j = 0; j < 24; j++) {
					if (typeId == WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_characterInfo.partTypeIds[j]) {
						return true;
					}
				}
			}
		}

		return false;
	}

	for (int j = 0; j < 24; j++) {
		if (typeId == WSingleton<CSharedDoc>::Instance()->m_roomPlayers[team].m_characterInfo.partTypeIds[j]) {
			return true;
		}
	}

	return false;
}

// shareddoc.cpp:1212, the combo gauge's limit for a team: 99, 33 more when one
// of its players wears one of the parts below.
float CSharedDoc::GetComboGaugeLimit(unsigned char team) {
	static unsigned long comboParts[] = {
		0x08008816, 0x08008817,
		0x08046822, 0x08046823,
		0x08088816, 0x08088817,
		0x080C8027, 0x080C8028,
		0x08106019, 0x0810601A,
		0x08148025, 0x08148026,
		0x0818601D, 0x0818601E,
		0x081C801A, 0x081C801B,
		0x08208026, 0x08208027,
		0x08248026, 0x08248027,
		0x08008818, 0x08008819,
		0x08046828, 0x08046829,
		0x0808881B, 0x0808881C,
		0x080C802E, 0x080C802F,
		0x0810601C, 0x0810601D,
		0x0814802D, 0x0814802E,
		0x08186027, 0x08186028,
		0x081C801C, 0x081C801D,
		0x0820802F, 0x08208030,
		0x0824802B, 0x0824802C,
	};

	float bonus = 0.0f;
	for (unsigned int i = 0; i < 40; i++) {
		if (FindEquipPart(team, comboParts[i])) {
			bonus = 33.0f;
			break;
		}
	}

	return bonus + 99.0f;
}

// shareddoc.cpp:1249, adds an owned character; a part combination the
// character cannot wear is replaced by the default parts.
void CSharedDoc::AddChar(sCharacterInfo *pChar) {
	CPartTidList tidList;

	if (!tidList.SetTids(pChar, 0xFF) || !tidList.IsComboValid()) {
		tidList.SetDefaultTids();
		memcpy(pChar->partTypeIds, tidList.m_partTids, sizeof(pChar->partTypeIds));
	}

	m_characterMap.insert(std::map<unsigned int, sCharacterInfo>::value_type(pChar->id, *pChar));
}

// shareddoc.cpp:1277
void CSharedDoc::AddCaddie(sCaddieInfo *pCaddie) {
	m_caddieMap.insert(std::map<unsigned int, sCaddieInfo>::value_type(pCaddie->id, *pCaddie));
}

// shareddoc.cpp:1282, adds an owned mascot or updates the one with its id.
void CSharedDoc::AddMascot(sMascotInfo *pMascot) {
	std::map<unsigned int, sMascotInfo>::iterator it = m_myMascotMap.find(pMascot->id);

	if (it != m_myMascotMap.end()) {
		it->second = *pMascot;
	} else {
		m_myMascotMap.insert(std::map<unsigned int, sMascotInfo>::value_type(pMascot->id, *pMascot));
	}
}

// shareddoc.cpp:1297
sCardStack *CSharedDoc::GetCardInfoToUID(unsigned long uid) {
	std::list<sCardStack>::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_cardList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_cardList.end(); ++it) {
		sCardStack *pCard = &*it;
		if (uid == pCard->uid && pCard->valid == 1) {
			return pCard;
		}
	}
	return 0;
}

// shareddoc.cpp:1312
sCardStack *CSharedDoc::GetCardInfoToTID(unsigned long typeId) {
	std::list<sCardStack>::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_cardList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_cardList.end(); ++it) {
		sCardStack *pCard = &*it;
		if (typeId == pCard->typeId && pCard->valid == 1) {
			return pCard;
		}
	}
	return 0;
}

// shareddoc.cpp:1327
void CSharedDoc::ReplaceCardCountToTID(unsigned long typeId, int count) {
	std::list<sCardStack>::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_cardList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_cardList.end(); ++it) {
		if (typeId == it->typeId && it->valid == 1) {
			it->count = count;
			if (count <= 0) it->valid = 0;
			return;
		}
	}
}

// shareddoc.cpp:1338
void CSharedDoc::ReplaceCardCountToUID(unsigned long uid, int count) {
	std::list<sCardStack>::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_cardList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_cardList.end(); ++it) {
		if (uid == it->uid && it->valid == 1) {
			it->count = count;
			if (count <= 0) it->valid = 0;
			return;
		}
	}
}

// shareddoc.cpp:1349
void CSharedDoc::DeleteCardToPartsAttach(unsigned long cardId) {
	for (std::list<sSCardAvilityPeriodInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_cardPeriodList[0].begin(); it != WSingleton<CSharedDoc>::Instance()->m_cardPeriodList[0].end(); ++it) {
		if (it->partsId == cardId) {
			WSingleton<CSharedDoc>::Instance()->m_cardPeriodList[0].erase(it);
			break;
		}
	}
}

// shareddoc.cpp:1364, adds a card stack, or sets (replace) or adds the
// count of the valid stack of the same type.
void CSharedDoc::AddCardStack(sCardStack cardStack, bool replace) {
	std::list<sCardStack>::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_cardList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_cardList.end(); ++it) {
		if (cardStack.typeId == (*it).typeId && cardStack.valid == 1) {
			if (replace == true) {
				(*it).count = cardStack.count;
			} else {
				(*it).count += cardStack.count;
			}

			if ((*it).count <= 0) {
				(*it).valid = 0;
			}

			return;
		}
	}

	if (it == WSingleton<CSharedDoc>::Instance()->m_cardList.end()) {
		WSingleton<CSharedDoc>::Instance()->m_cardList.push_back(cardStack);
	}
}

// shareddoc.cpp:1395
void CSharedDoc::DeleteCardStack(unsigned long uid, int count) {
	std::list<sCardStack>::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_cardList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_cardList.end(); ++it) {
		sCardStack *pCard = &*it;
		if (uid == pCard->uid) {
			pCard->count -= count;
			if (pCard->count < 1) {
				pCard->count = 0;
				pCard->valid = 0;
			}
		}
	}
}

// shareddoc.cpp:1413, adds count cards of the type, as a new entry when the
// player has none yet.
void CSharedDoc::AddCard(unsigned long typeId, unsigned long guid, unsigned short count) {
	std::list<sCards>::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_myCardList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myCardList.end(); ++it) {
		if ((*it).m_typeId == typeId) {
			(*it).m_count += count;
			return;
		}
	}

	if (it == WSingleton<CSharedDoc>::Instance()->m_myCardList.end()) {
		sCards card;
		memset(&card, 0, sizeof(card));

		card.m_typeId = typeId;
		card.m_guid = guid;
		card.m_count = count;
		WSingleton<CSharedDoc>::Instance()->m_myCardList.push_back(card);
	}
}

// shareddoc.cpp:1440
void CSharedDoc::AddFurniture(const sFurniture_List &furniture) {
	m_furnitureList.push_back(furniture);
}

// shareddoc.cpp:1445
void CSharedDoc::AddMyPartsList(const sItemInfo &item) {
	m_myPartsList.push_back(item);
}

// shareddoc.cpp:1459
sMascotInfo *CSharedDoc::GetMyMascotInfo(unsigned long id) {
	if (id) {
		std::map<unsigned int, sMascotInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myMascotMap.find(id);
		if (it != WSingleton<CSharedDoc>::Instance()->m_myMascotMap.end()) {
			return &it->second;
		}
	}
	return 0;
}

// shareddoc.cpp:1474
sMascotInfo *CSharedDoc::GetMyEquippedMascotInfo(void) {
	if (m_myMascotId) {
		std::map<unsigned int, sMascotInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myMascotMap.find(m_myMascotId);
		if (it != WSingleton<CSharedDoc>::Instance()->m_myMascotMap.end()) {
			return &it->second;
		}
	}
	return 0;
}

// shareddoc.cpp:1492
int CSharedDoc::GetNumAddItemSlotByMascot(unsigned char player) {
	unsigned long typeId = m_roomPlayers[player].m_mascot.typeId;
	int num = 0;
	IFF_STRUCT::sMascot *pMascot = ItemManager()->FindMascot(typeId);

	if (pMascot) {
		unsigned char slots = pMascot->m_addItemSlots;
		if (slots <= 1) {
			num = slots;
		}
	}
	return num;
}

// shareddoc.cpp:1510, the extra item slots the local player's equipped,
// unexpired mascot gives (0 or 1).
int CSharedDoc::GetNumAddItemSlotByMascot(void) {
	int num = 0;
	sMascotInfo *pMascotInfo = GetMyEquippedMascotInfo();

	if (pMascotInfo != NULL) {
		if (pMascotInfo->remainHours > num) {
			if (ItemManager()->CompareSystemTime(pMascotInfo->endTime, WSingleton<CSharedDoc>::Instance()->GetServerTime()) > 0) {
				IFF_STRUCT::sMascot *pMascot = ItemManager()->FindMascot(pMascotInfo->typeId);

				if (pMascot != NULL) {
					if (pMascot->m_addItemSlots <= 1) {
						num = pMascot->m_addItemSlots;
					}
				}
			}
		}
	}

	return num;
}

// shareddoc.cpp:1534, sends the equipped items without the mascot's extra
// slot when the equipped mascot no longer grants it (none, no extra slot, or
// expired).
void CSharedDoc::SendEquipItemSlotByMascot(void) {
	sMascotInfo *pMascotInfo = GetMyEquippedMascotInfo();

	if (pMascotInfo != NULL) {
		IFF_STRUCT::sMascot *pMascot = ItemManager()->FindMascot(pMascotInfo->typeId);

		if (pMascot != NULL) {
			if (pMascot->m_addItemSlots > 0) {
				if (ItemManager()->CompareSystemTime(pMascotInfo->endTime, WSingleton<CSharedDoc>::Instance()->GetServerTime()) > 0) {
					return;
				}
			}
		}
	}

	if (m_equipItem[9] == 0) {
		return;
	}

	unsigned long equipItem[10] = { 0 };

	memcpy(equipItem, WSingleton<CSharedDoc>::Instance()->m_equipItem, sizeof(equipItem));
	equipItem[9] = 0;

	WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
	packet.Encode1(2);
	packet.EncodeBuffer(equipItem, sizeof(equipItem));
	packet.Send(SEND_TO_DEFAULT);
}

// shareddoc.cpp:1570, sends the equipped items, unless an unexpired mascot
// slot and a card slot are both available or the last two slots are empty.
void CSharedDoc::SendEquipItemSlot(void) {
	sMascotInfo *pMascotInfo = GetMyEquippedMascotInfo();
	WSingleton<CCardManager>::Instance()->SetPlayerIndex(0xFF);
	WSingleton<CCardManager>::Instance()->CalcCardPeriodAndStatus();
	int cardSlot = WSingleton<CCardManager>::Instance()->GetCardPeriodSlot();

	if (pMascotInfo != NULL) {
		IFF_STRUCT::sMascot *pMascot = ItemManager()->FindMascot(pMascotInfo->typeId);

		if (pMascot != NULL) {
			if (pMascot->m_addItemSlots > 0) {
				if (ItemManager()->CompareSystemTime(pMascotInfo->endTime, WSingleton<CSharedDoc>::Instance()->GetServerTime()) > 0 && cardSlot > 0) {
					return;
				}
			}
		}
	}

	if (m_equipItem[9] == 0 && m_equipItem[8] == 0) {
		return;
	}

	unsigned long equipItem[10];
	memcpy(equipItem, m_equipItem, sizeof(equipItem));

	WSendPacket packet(0x20);
	packet.Encode1(2);
	packet.EncodeBuffer(equipItem, sizeof(equipItem));
	packet.Send(SEND_TO_DEFAULT);
}

// shareddoc.cpp:1610, takes the local player's equipment; a caddie or mascot
// the player does not own is replaced (the first owned caddie) or cleared.
void CSharedDoc::SetCurEquipInfo(sUserEquip *equip) {
	m_myEquip = *equip;
	BuildMyPartTidList();

	if (m_myCaddieId != 0) {
		if (m_myCaddieId != 0xFFFFFFFF) {
			if (m_caddieMap.find(m_myCaddieId) == m_caddieMap.end()) {
				if (m_caddieMap.size() > 0) {
					m_myCaddieId = m_caddieMap.begin()->first;
				}
			}
		}
	}

	if (m_myMascotId != 0) {
		if (m_myMascotId != 0xFFFFFFFF) {
			if (m_myMascotMap.find(m_myMascotId) == m_myMascotMap.end()) {
				m_myMascotId = 0;
			}
		}
	}
}

// shareddoc.cpp:1662, re-validates the parts every owned character wears
// (default parts when the set is invalid, dropping parts the player no
// longer owns), sends each changed character and re-equips the current one.
void CSharedDoc::CheckCurrEquipParts(void) {
	CPartTidList tidList;
	CPartList partList;
	bool bChangedCurChar = false;

	for (std::map<unsigned int, sCharacterInfo>::iterator it = m_characterMap.begin(); it != m_characterMap.end(); ++it) {
		bool bChanged = false;
		sCharacterInfo *pCharInfo = &(*it).second;

		if (!tidList.SetTids(pCharInfo, m_myLevel)) {
			tidList.SetDefaultTids();
		} else {
			partList.Build(&tidList);

			unsigned long *pTid = pCharInfo->partTypeIds;
			for (int i = 24; i != 0; i--, pTid++) {
				unsigned long typeId = *pTid;
				if (typeId == 0 || (typeId & 0x600) != 0)
					continue;

				std::list<sItemInfo>::iterator itPart;
				for (itPart = m_myPartsList.begin(); itPart != m_myPartsList.end(); ++itPart) {
					if ((*itPart).m_typeId == typeId)
						break;
				}

				if (itPart == m_myPartsList.end()) {
					bChanged = true;
					partList.SetPart(&tidList, typeId, 0);
				}
			}

			if (!bChanged)
				continue;
		}

		memcpy(pCharInfo->partTypeIds, tidList.m_partTids, sizeof(pCharInfo->partTypeIds));
		if (pCharInfo->id == m_myCharacterId)
			bChangedCurChar = true;

		WSendPacket packet(0x20);
		packet.Encode1(0);
		packet.EncodeBuffer(pCharInfo, sizeof(sCharacterInfo));
		packet.Send(SEND_TO_DEFAULT);
	}

	if (bChangedCurChar) {
		WSendPacket packet(0x20);
		packet.Encode1(5);
		packet.Encode4(m_myCharacterId);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// shareddoc.cpp:1740, builds the local player's part list from the default
// parts of character typeId; false when no character has that typeid.
bool CSharedDoc::BuildMyPartTidListDefault(unsigned long typeId) {
	IFF_STRUCT::sChar *pChar = m_itemManager.FindChar(typeId);
	if (pChar) {
		m_myPartTidList.m_tidChar = typeId;
		m_myPartTidList.m_unk04 = pChar->m_numParts;
		m_myPartTidList.m_numTotalParts = pChar->m_numParts + pChar->m_numExtraParts;
		m_myPartTidList.m_hairClr = 0;
		m_myPartTidList.m_shirtsClr = 0;
		m_myPartTidList.SetDefaultTids();

		memset(m_myAuxPartTidList, 0, sizeof(m_myAuxPartTidList));
		return true;
	}

	return false;
}

// shareddoc.cpp:1787, builds the local player's part list from the equipped
// character; with no owned character of that id, from the default parts of
// character typeid 0x4000000 | (eventFlags bit 1).
void CSharedDoc::BuildMyPartTidList(void) {
	sCharacterInfo defaultChar = sCharacterInfo();
	sCharacterInfo *pChar;
	std::map<unsigned int, sCharacterInfo>::iterator it;
	it = m_characterMap.find(m_myCharacterId);

	if (it == m_characterMap.end()) {
		memset(&defaultChar, 0, sizeof(defaultChar));
		defaultChar.typeId = 0x4000000 | (WSingleton<CSharedDoc>::Instance()->m_userInfo.eventFlags >> 1 & 1) % 2;
		ItemManager()->GetDefCombo(defaultChar.typeId, defaultChar.partTypeIds);
		pChar = &defaultChar;
	} else {
		pChar = &it->second;
	}

	if (!m_myPartTidList.SetTids(pChar, WSingleton<CSharedDoc>::Instance()->m_myLevel)) {
		m_myPartTidList.SetDefaultTids();
	}

	if (!m_myPartTidList.IsComboValid()) {
		m_myPartTidList.SetDefaultTids();
	}

	memcpy(m_myAuxPartTidList, &pChar->auxParts, sizeof(m_myAuxPartTidList));
}

// shareddoc.cpp:1820, builds the local player's part list from characterInfo,
// falling back to the default parts when its parts are not a valid combination.
void CSharedDoc::BuildMyPartTidList(sCharacterInfo &characterInfo) {
	if (!m_myPartTidList.SetTids(&characterInfo, WSingleton<CSharedDoc>::Instance()->m_myLevel)) {
		m_myPartTidList.SetDefaultTids();
	}

	if (!m_myPartTidList.IsComboValid()) {
		m_myPartTidList.SetDefaultTids();
	}

	memcpy(m_myAuxPartTidList, &characterInfo.auxParts, sizeof(m_myAuxPartTidList));
}

// shareddoc.cpp:1843, rebuilds the result list from the tiki report: the
// local player's line also sets the match, exp, room type and winning team.
void CSharedDoc::BuildTikiReportList(void) {
	if (!IsLocalContent((localContentType_t)0x14)) {
		return;
	}

	int count = m_tikiReportList.size();
	if (count == 0) {
		return;
	}

	WSingleton<CSharedDoc>::Instance()->m_gameResultList.~vector();
	WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_userLimit = 30;

	for (std::vector<sCaddieReportData>::iterator it = m_tikiReportList.begin(); it != m_tikiReportList.end(); ++it) {
		sRivalData rival;
		memset(&rival, 0, sizeof(rival));
		rival.m_id = it->m_uid;
		rival.m_uid = it->m_uid;
		strcpy(rival.m_nickname, it->m_nickname);
		rival.m_level = it->m_level;
		rival.m_score = it->m_score;
		rival.m_pang = it->m_pang;
		rival.m_extraPang = it->m_extraPang;
		rival.m_hasEmblem = it->m_guildUID;
		strcpy(rival.m_emblemName, it->m_emblemName);
		rival.m_mascotTypeId = it->m_mascotTypeId;
		rival.m_holeShots[0] = it->m_value28;
		unsigned char flags = it->m_flags;
		if (flags & 4) {
			flags ^= 4;
			rival.m_state = 3;
		}
		rival.m_team = flags;

		if (strcmp(rival.m_nickname, WSingleton<CSharedDoc>::Instance()->m_userInfo.nickname) == 0) {
			WSingleton<CSharedDoc>::Instance()->m_value456C = it->m_value28;
			WSingleton<CSharedDoc>::Instance()->m_gameStatistics[0].exp = it->m_exp;
			WSingleton<CSharedDoc>::Instance()->m_matchTypeId = it->m_matchTypeId;
			WSingleton<CSharedDoc>::Instance()->m_roomType = it->m_roomType;
			WSingleton<CSharedDoc>::Instance()->m_winningTeam = it->m_winningTeam;
		}

		rival.m_identity = it->m_isGM ? 0x80 : 0;
		rival.m_itemBoostFlags = 0;
		rival.m_extraFlags = 0;

		if (it->m_awardFlags & 1) {
			m_awardPlayers[0].m_uid = rival.m_id;
		}
		if (it->m_awardFlags & 2) {
			m_awardPlayers[1].m_uid = rival.m_id;
		}
		if (it->m_awardFlags & 4) {
			m_awardPlayers[2].m_uid = rival.m_id;
		}
		if (it->m_awardFlags & 8) {
			m_awardPlayers[3].m_uid = rival.m_id;
		}
		if (it->m_awardFlags & 0x10) {
			m_awardPlayers[4].m_uid = rival.m_id;
		}
		if (it->m_awardFlags & 0x20) {
			m_awardPlayers[5].m_uid = rival.m_id;
		}

		switch (it->m_boostKind) {
		case 1:
			rival.m_itemBoostFlags = 1;
			break;
		case 2:
			rival.m_extraFlags = 1;
			break;
		case 3:
			rival.m_extraFlags = 1;
			rival.m_itemBoostFlags = 1;
			break;
		}

		m_gameResultList.push_back(rival);
	}
}

// shareddoc.cpp:1972
CPartTidList &CSharedDoc::GetMyPartTidList(void) {
	return m_myPartTidList;
}

// shareddoc.cpp:1976
unsigned long *CSharedDoc::GetMyAuxPartTidList(void) {
	return m_myAuxPartTidList;
}

// shareddoc.cpp:1981
void CSharedDoc::SetIndex(unsigned long uid) {
	if (m_indexMap.find(uid) == m_indexMap.end()) {
		unsigned char index = (unsigned char)m_indexMap.size();

		if (OnlinePlay()) {
			if (IsMassGame()) {
				if (uid == m_userInfo.guid)
					memcpy(m_roomPlayers[0].m_nickname, &m_userInfo, sizeof(m_userInfo));
			}
		}

		m_indexMap[uid] = index;
	}
}

// shareddoc.cpp:2009
bool CompareItemInfo(const sItemInfo &a, const sItemInfo &b) {
	return a.m_typeId < b.m_typeId;
}

// shareddoc.cpp:2014
void CSharedDoc::SortAllMyItemList(void) {
	m_myPartsList.sort(CompareItemInfo);
	m_myGearList.sort(CompareItemInfo);
	m_itemInfoList.sort(CompareItemInfo);
	m_myExtraItemList.sort(CompareItemInfo);
	m_myExtraItemList2.sort(CompareItemInfo);
}

// shareddoc.cpp:2024, whether the player can receive count more of the item:
// a stackable item (type class 5 or 6) holds at most 20000.
bool CSharedDoc::ConfirmInsertItem(unsigned long typeId, unsigned long count) const {
	unsigned long typeClass = typeId >> 26;

	if (typeClass >= 5 && typeClass <= 6 && typeId != 0x1A000010) {
		std::list<sItemInfo>::const_iterator it;
		for (it = m_itemInfoList.begin(); it != m_itemInfoList.end(); ++it) {
			if (sItemInfo(*it).m_typeId == typeId) {
				return sItemInfo(*it).m_count + count <= 20000 ? true : false;
			}
		}

		return true;
	}

	return true;
}

// shareddoc.cpp:2085
void CSharedDoc::AddMyItem(const sItemInfo &item) {
	WSingleton<CSharedDoc>::Instance()->m_itemInfoList.push_back(item);
}

// shareddoc.cpp:2092
bool CSharedDoc::DecreaseMyItem(std::list<sItemInfo>::iterator &it) {
	sItemInfo *pItem = &(*it);
	pItem->m_count--;
	if (pItem->m_count <= 0)
		m_itemInfoList.erase(it);

	return true;
}

// shareddoc.cpp:2103
bool CSharedDoc::DeleteMyItem(unsigned long typeId) {
	std::list<sItemInfo>::iterator it;
	for (it = m_itemInfoList.begin(); it != m_itemInfoList.end(); ++it) {
		if ((*it).m_typeId == typeId)
			break;
	}

	return DeleteMyItem(it);
}

// shareddoc.cpp:2117
bool CSharedDoc::DeleteMyItem(std::list<sItemInfo>::iterator &it) {
	if (it == m_itemInfoList.end()) {
		return false;
	}

	m_itemInfoList.erase(it);
	return true;
}

// shareddoc.cpp:2128
sItemInfo *CSharedDoc::FindMyItemByGuid(unsigned long guid) {
	for (std::list<sItemInfo>::iterator it = m_itemInfoList.begin(); it != m_itemInfoList.end(); ++it) {
		if ((*it).m_serial == guid)
			return &*it;
	}

	return NULL;
}

// shareddoc.cpp:2146
void CSharedDoc::ClearMyGuildInfo(void) {
	m_userInfo.guildUID = 0;
	m_userInfo.guildPosition = 0;
	memset(m_userInfo.guildName, 0, sizeof(m_userInfo.guildName));
	memset(m_userInfo.guildMark, 0, sizeof(m_userInfo.guildMark));
}

// shareddoc.cpp:2156, fills the first room player from the local player's
// info and owned character, caddie, mascot and club set (a club set the
// player's level does not allow is replaced by the first basic one).
void CSharedDoc::SetEquipCharCaddieInfoFromMyInfo(void) {
	memset(&m_roomPlayers[0].m_characterInfo, 0, sizeof(m_roomPlayers[0].m_characterInfo));
	memset(&m_roomPlayers[0].m_caddieInfo, 0, sizeof(m_roomPlayers[0].m_caddieInfo));
	memset(&m_roomPlayers[0].m_clubSet, 0, sizeof(m_roomPlayers[0].m_clubSet));
	memset(&m_roomPlayers[0].m_mascot, 0, sizeof(m_roomPlayers[0].m_mascot));

	memcpy(m_roomPlayers[0].m_nickname, &m_userInfo, 0x10D); // the whole sPangYaUserInfo
	m_roomPlayers[0].m_statistics = m_myStatistics;
	m_roomPlayers[0].m_equip = m_myEquip;

	std::map<unsigned int, sCharacterInfo>::iterator itChar;
	itChar = m_characterMap.find(m_myCharacterId);
	if (itChar != m_characterMap.end()) {
		m_roomPlayers[0].m_characterInfo = (*itChar).second;
	}

	std::map<unsigned int, sCaddieInfo>::iterator itCaddie;
	itCaddie = m_caddieMap.find(m_myCaddieId);
	if (itCaddie != m_caddieMap.end()) {
		m_roomPlayers[0].m_caddieInfo = (*itCaddie).second;

		IFF_STRUCT::sCaddie *pCaddie = ItemManager()->FindCaddie(m_roomPlayers[0].m_caddieInfo.typeId);

		if ((pCaddie != NULL && m_myLevel < (pCaddie->m_level & 0x7F)) ||
			((WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_caddieInfo.flags & 2) &&
			 WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_caddieInfo.days == 0 && pCaddie->fee > 0))
			m_roomPlayers[0].m_caddieInfo.typeId = 0;
	}

	std::map<unsigned int, sMascotInfo>::iterator itMascot;
	itMascot = m_myMascotMap.find(m_myMascotId);
	if (itMascot != m_myMascotMap.end()) {
		m_roomPlayers[0].m_mascot = (*itMascot).second;

		ItemManager()->FindMascot(m_roomPlayers[0].m_mascot.typeId);
	}

	std::map<unsigned int, sItemInfo>::iterator itClub;
	itClub = m_myItemMap.find(m_myClubId);
	if (itClub != m_myItemMap.end()) {
		IFF_STRUCT::sClubSet *pClubSet = ItemManager()->FindClubSet((*itClub).second.m_typeId);

		if (pClubSet != NULL &&
			((m_myLevel < (pClubSet->m_level & 0x7F) && !(pClubSet->m_level & 0x80)) ||
			 (m_myLevel > (pClubSet->m_level & 0x7F) && (pClubSet->m_level & 0x80)))) {
			for (std::map<unsigned int, sItemInfo>::iterator it = m_myItemMap.begin(); it != m_myItemMap.end(); ++it) {
				if ((*it).second.m_typeId == 0x10000000) {
					itClub = it;
					break;
				}
			}
		} else {
			itClub = m_myItemMap.find(m_myClubId);
		}

		if (itClub != m_myItemMap.end()) {
			m_roomPlayers[0].m_clubSet.typeId = (*itClub).second.m_typeId;
			m_roomPlayers[0].m_clubSet.id = (*itClub).second.m_serial;
			memcpy(m_roomPlayers[0].m_clubSet.statLevel, (*itClub).second.m_clubStatLevel, sizeof(m_roomPlayers[0].m_clubSet.statLevel));
		}
	}
}

// shareddoc.cpp:2233, the game result of the guild match opponent of the
// player with the UID.
sRivalData *CSharedDoc::GetRival(unsigned long uid) {
	for (std::vector<sGuildMatchup>::iterator it = m_vector4524.begin(); it != m_vector4524.end(); ++it) {
		if (uid == (*it).m_guidA) {
			return &m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex((*it).m_guidB)];
		}

		if (uid == (*it).m_guidB) {
			return &m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex((*it).m_guidA)];
		}
	}

	return NULL;
}

// shareddoc.cpp:2256, moves the player's equipped items to the front of the
// usable slots (10, or 8 plus the mascot's and the cards' extra slots); the
// number of equipped items.
int CSharedDoc::CollapseItemSlot(unsigned char player) {
	if (player >= 4) {
		return 0;
	}

	unsigned long *pItems = m_roomPlayers[player].m_equipItem;
	int slotNum = 10;

	if (IsLocalContent((localContentType_t)3)) {
		slotNum = GetNumAddItemSlotByMascot(player) + 8;
	}

	if (IsLocalContent((localContentType_t)0x51)) {
		WSingleton<CCardManager>::Instance()->SetPlayerIndex(player);
		slotNum += WSingleton<CCardManager>::Instance()->GetCardPeriodSlot();
	}

	int count = 0;
	for (int i = 0; i < slotNum; i++) {
		if (pItems[i] != 0)
			count++;
	}

	for (int i = 0; i < count; i++) {
		if (pItems[i] == 0) {
			int j;
			for (j = i + 1; j < slotNum; j++) {
				if (pItems[j] != 0) {
					pItems[i] = pItems[j];
					pItems[j] = 0;
					break;
				}
			}

			if (j == slotNum)
				break;
		}
	}

	return count;
}

// shareddoc.cpp:2312
const char *CSharedDoc::GetDefaultTitle(unsigned char gameType) {
	switch (gameType) {
	case 0:
	case 1:
		return defaultTitles_vs[rand() % 9];

	case 4:
	case 5:
	case 6:
	case 0xE:
		return defaultTitles_mass[rand() % 6];

	case 7:
		return defaultTitles_battle[rand() % 4];

	case 0xA:
		return defaultTitles_approach[rand() % 3];

	case 2:
	case 8:
		return defaultTitles_chat[rand() % 7];

	default:
		return "No Title!";
	}
}

// <direct.h>'s mkdir (the toolchain has no direct.h yet).
extern "C" __declspec(dllimport) int __cdecl mkdir(const char* dirname);

// shareddoc.cpp:2378, writes the 30s game's result to
// score/score_30s_<date>_<time>.txt (the result room kinds only).
void CSharedDoc::Log30sScore(void) {
	switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
		break;
	default:
		if (WSingleton<CSharedDoc>::Instance()->m_roomType != 6)
			return;
	}

	mkdir("score");

	SYSTEMTIME time;
	char fileName[128];

	GetLocalTime(&time);

	sprintf(fileName, "score/score_30s_%d%02d%02d_%02d%02d%02d.txt", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

	FILE *fp = fopen(fileName, "wt");
	if (fp != NULL) {
		fprintf(fp, "## 30s Game Result ##\n\n");
		fprintf(fp, "* room name : %s\n", &WSingleton<CSharedDoc>::Instance()->m_roomInfo);
		fprintf(fp, "* map no. : %d\n", (unsigned char)((WSingleton<CSharedDoc>::Instance()->m_curMap > 0x7F && WSingleton<CSharedDoc>::Instance()->m_curMap != 0xFD) ? WSingleton<CSharedDoc>::Instance()->m_curMap - 0x80 : WSingleton<CSharedDoc>::Instance()->m_curMap));
		fprintf(fp, "* grade : %d\n", (unsigned char)(WSingleton<CSharedDoc>::Instance()->m_matchTypeId >> 16));
		fprintf(fp, "* game finished at : %d-%02d-%02d %02d:%02d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
		fprintf(fp, "* num of players : %d\n\n", m_gameResultList.size());

		fprintf(fp, "nick / id                                rank  score  pang \n");
		fprintf(fp, "-----------------------------------------------------------\n");

		for (int i = 0; i < (int)m_gameResultList.size(); i++) {
			sGameResultInfo *pResult = &m_gameResultList[i];

			if (pResult != NULL) {
				fprintf(fp, "%18s   %2d    ", pResult->m_nickname, pResult->m_rank);

				if (pResult->m_score == 0)
					fprintf(fp, "  0   %I64d\n", pResult->m_pang);
				else
					fprintf(fp, "%+2d   %I64d\n", pResult->m_score, pResult->m_pang);
			}
		}

		fclose(fp);
	}
}

// shareddoc.cpp:2632
void CSharedDoc::LogGstUpdateHole(unsigned long uid, unsigned char hole, int score, __int64 pang) {
}

// shareddoc.cpp:2839, the name of school schoolNo from school.txt (each line:
// the number, a space, then the name up to a '('); "" when it is not listed.
void CSharedDoc::LoadSchoolName(char *const szName, unsigned long schoolNo) {
	char line[0x2000];

	cFile *file = g_resrcmng->GetCFile("school.txt", 0xFFFF);

	if (file == NULL) {
		szName[0] = '\0';
		return;
	}

	while (file->Scan("%n", line)) {
		int len = strlen(line);
		cTokenV token;
		token.Init(line, len);

		char number[0x40];
		token.GetToken(number, " ", 1);
		token.GetToken(szName, "(\r\n", 3);

		if (schoolNo == atoi(number)) {
			CloseCFile(file);
			return;
		}
	}

	CloseCFile(file);
	szName[0] = '\0';
}

// shareddoc.cpp:2893
bool CSharedDoc::InitRecentWhisperList(FrComboBox *pComboBox) {
	m_recentWhisperList.clear();
	return UpdateChatTarget(pComboBox);
}

// shareddoc.cpp:2899, the chat target names are "to team", "to all" and "to
// guild" (cp949).
bool CSharedDoc::InitReservedChatList(FrComboBox *pComboBox, eReservedChatPartner partner) {
	if (pComboBox == 0) {
		return false;
	}

	m_list4CD8.clear();
	if (m_string4CE8.size() > 0) {
		pComboBox->SetLine(1, m_string4CE8.c_str(), 0, false, 0);
	}

	if (partner & RESERVED_CHAT_PARTNER_TEAM) {
		m_list4CD8.push_back("\xC6\xC0\xBF\xA1\xB0\xD4");
	} else if (strcmpi(m_string4CE8.c_str(), "\xC6\xC0\xBF\xA1\xB0\xD4") == 0) {
		m_string4CE8 = "\xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4";
		pComboBox->SetLine(1, "\xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4", 0, false, 0);
	}

	if (partner & RESERVED_CHAT_PARTNER_GUILD) {
		m_list4CD8.push_back("\xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4");
	}

	if (WSingleton<CSharedDoc>::Instance()->m_userInfo.guildUID != 0) {
		m_list4CD8.push_back("\xB1\xE6\xB5\xE5\xBF\xA1\xB0\xD4");
	}

	if (strlen(pComboBox->GetLine(1, false)) == 0) {
		pComboBox->SetLine(1, "\xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4", 0, false, 0);
	}

	return UpdateChatTarget(pComboBox);
}

// shareddoc.cpp:2946, makes name (or the combo box's text when name is 0)
// the newest whisper partner: a reserved partner only becomes the current
// target, "to team" is refused, and the list keeps at most m_value4CE4
// names, the oldest dropped.
bool CSharedDoc::AddWhisperPartner(FrComboBox *pComboBox, const char *name) {
	if (pComboBox == 0) {
		return false;
	}

	bool fromName = false;
	if (name != 0) {
		fromName = true;
	} else {
		if (strlen(pComboBox->GetLine(1, false)) == 0 || strcmp(pComboBox->GetLine(1, false), WSingleton<CSharedDoc>::Instance()->m_userInfo.nickname) == 0) {
			return false;
		}

		name = pComboBox->GetLine(1, false);
	}

	if (name == 0) {
		return false;
	}

	std::list<std::string>::iterator it;
	it = std::find(m_list4CD8.begin(), m_list4CD8.end(), name);
	if (it != m_list4CD8.end()) {
		if (!fromName) {
			m_string4CE8 = name;
		}
		return true;
	}

	if (strcmpi("\xC6\xC0\xBF\xA1\xB0\xD4", name) == 0) {
		return false;
	}

	it = std::find(m_recentWhisperList.begin(), m_recentWhisperList.end(), name);
	if (it != m_recentWhisperList.end()) {
		if (*it == m_recentWhisperList.back()) {
			return true;
		}

		if (!fromName) {
			m_string4CE8 = name;
		}
		m_recentWhisperList.push_back(name);
		m_recentWhisperList.erase(it);

		return UpdateChatTarget(pComboBox);
	}

	if (m_recentWhisperList.size() == m_value4CE4) {
		if (!fromName) {
			m_string4CE8 = name;
		}
		m_recentWhisperList.push_back(name);
		m_recentWhisperList.erase(m_recentWhisperList.begin());
		return UpdateChatTarget(pComboBox);
	}

	if (!fromName) {
		m_string4CE8 = name;
	}
	m_recentWhisperList.push_back(name);
	return UpdateChatTarget(pComboBox);
}

// shareddoc.cpp:3024, refills the combo box with the recent whispers, then
// the reserved partners.
bool CSharedDoc::UpdateChatTarget(FrComboBox *pComboBox) {
	if (pComboBox == 0) {
		return false;
	}

	pComboBox->ClearList();

	for (std::list<std::string>::iterator it = m_recentWhisperList.begin(); it != m_recentWhisperList.end(); ++it) {
		pComboBox->AddString((*it).c_str());
	}

	for (std::list<std::string>::iterator it = m_list4CD8.begin(); it != m_list4CD8.end(); ++it) {
		pComboBox->AddString((*it).c_str());
	}

	return true;
}

// shareddoc.cpp:3047, sends message to the combo box's target ("to all"
// when empty): "to team" sends to the team, a reserved partner to everyone,
// any other name is whispered.
bool CSharedDoc::UTIL_SendChatMessage(FrComboBox *pComboBox, const char *message, bool useSendChatMessage2) {
	if (pComboBox == 0) {
		return false;
	}

	if (strlen(pComboBox->GetLine(1, false)) == 0) {
		pComboBox->SetLine(1, "\xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4", 0, false, 0);
	}

	if (!AddWhisperPartner(pComboBox, 0)) {
		return false;
	}

	const char *target = 0;
	bool team = false;
	if (strcmpi(pComboBox->GetLine(1, false), "\xC6\xC0\xBF\xA1\xB0\xD4") == 0) {
		team = true;
	} else if (strcmpi(pComboBox->GetLine(1, false), "\xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4") != 0 && strcmpi(pComboBox->GetLine(1, false), "\xC6\xC0\xBF\xA1\xB0\xD4") != 0) {
		target = pComboBox->GetLine(1, false);
	}

	if (useSendChatMessage2) {
		m_chatManager.SendChatMessage2(message, target, team);
	} else {
		m_chatManager.SendChatMessage(message, target, team);
	}

	return true;
}

// shareddoc.cpp:3085, the chat target before target: the reserved partners
// follow the recent whispers, and the first whisper wraps to the last
// reserved partner.
const char *CSharedDoc::GetPrevChatTarget(const char *target) {
	std::list<std::string>::iterator it = std::find(m_list4CD8.begin(), m_list4CD8.end(), target);

	if (it != m_list4CD8.end()) {
		if (it != m_list4CD8.begin()) {
			--it;
			return it->c_str();
		}

		if (m_recentWhisperList.size() > 0) {
			it = m_recentWhisperList.end();
			--it;
			return it->c_str();
		}
	} else if (m_recentWhisperList.size() > 0) {
		it = std::find(m_recentWhisperList.begin(), m_recentWhisperList.end(), target);

		if (it != m_recentWhisperList.end()) {
			if (it != m_recentWhisperList.begin()) {
				--it;
				return it->c_str();
			}

			return m_recentWhisperList.front().c_str();
		}
	}

	it = m_list4CD8.end();
	--it;
	return it->c_str();
}

// shareddoc.cpp:3122, the chat target after target ("" when none follows).
const char *CSharedDoc::GetNextChatTarget(const char *target) {
	std::list<std::string>::iterator it = std::find(m_recentWhisperList.begin(), m_recentWhisperList.end(), target);

	if (it != m_recentWhisperList.end()) {
		++it;
		if (it != m_recentWhisperList.end()) {
			return it->c_str();
		}

		if (m_list4CD8.size() > 0) {
			it = m_list4CD8.begin();
			return it->c_str();
		}
	} else if (m_list4CD8.size() > 0) {
		it = std::find(m_list4CD8.begin(), m_list4CD8.end(), target);

		if (it != m_list4CD8.end()) {
			++it;
			if (it != m_list4CD8.end()) {
				return it->c_str();
			}
		}
	}

	return "";
}

// shareddoc.cpp:3157, whether the local player owns one of the angel wing parts
void CSharedDoc::CheckAngelWing(void) {


	m_haveAngelWing = false;

	for (std::list<sItemInfo>::iterator it = m_myPartsList.begin(); it != m_myPartsList.end(); ++it) {

		switch ((*it).m_typeId) {
		case 0x8016800:
		case 0x8058800:
		case 0x8098800:
		case 0x80DC800:
		case 0x8118800:
		case 0x8160800:
		case 0x8190800:
		case 0x81E2800:
		case 0x8214800:
		case 0x8254800:
			m_haveAngelWing = true;
			return;
		}
	}
}

// shareddoc.cpp:3175
void CSharedDoc::SetInitHaveHalloweenItem(void) {
	m_haveHalloweenItem = false;
	for (std::list<sItemInfo>::iterator it = m_myPartsList.begin(); it != m_myPartsList.end(); ++it) {
		if (((*it).m_typeId & 0x1800) == 0x1800) {
			m_haveHalloweenItem = true;
			break;
		}
	}
}

// shareddoc.cpp:3192, whether the local player's character wears a
// halloween part.
void CSharedDoc::SetInitWearHalloweenItem(void) {
	m_wearHalloweenItem = false;

	for (std::map<unsigned int, sCharacterInfo>::const_iterator it = m_characterMap.begin(); it != m_characterMap.end(); ++it) {
		const sCharacterInfo *pChar = &(*it).second;

		if (pChar == NULL) continue;
		if (pChar->id != m_myCharacterId) continue;

		for (int i = 0; i < 24; i++) {
			if ((pChar->partTypeIds[i] & 0x1800) == 0x1800) {
				m_wearHalloweenItem = true;
				break;
			}
		}

		break;
	}
}

// shareddoc.cpp:3225
int CSharedDoc::IsControlServerService(int service) {
	return m_controlServerService & service;
}

// shareddoc.cpp:3254
void CSharedDoc::InitTradeData(void) {
	m_stateTrade = STATE_TRADE_NONE;
	m_listTradeItem.clear();


	m_tradeUID = 0xFFFFFFFF; m_tradeTitle = "";
	m_countVisitor = 0;
	m_tradeIncome = 0;


	m_tradeMode = 0;
}

// shareddoc.cpp:3269
void CSharedDoc::SetTradeUID(unsigned long uid) {
	if (uid != 0xFFFFFFFF) {
		m_tradeUID = uid;
	}
}

// shareddoc.cpp:3278, adds item to the own shop's list (at most 6): a second
// item of the same GUID only for the stackable kinds; a self-design item asks
// the UCC manager for its clothes.
bool CSharedDoc::AddListTradeItem(sTradeItem item) {
	if (m_listTradeItem.size() >= 6) return false;
	if (item.guid == 0) return false;
	if (item.typeId == 0) return false;
	if ((long)item.count <= 0) return false;
	if (item.price <= 0) return false;

	for (std::list<sTradeItem>::iterator it = m_listTradeItem.begin(); it != m_listTradeItem.end(); ++it) {
		if ((*it).guid == item.guid) {
			unsigned long type = item.typeId >> 26;
			if (type < 5 || (type > 6 && type != 0x1F))
				return false;
		}
	}

	m_listTradeItem.push_back(item);
	if (IsLocalContent((localContentType_t)0x57)) {
		if (strlen(item.uccIdx) != 0 && item.uccSeq > 0 && (item.uccStatus & 1) && IsUccClothes(item.typeId)) {
			sUccClothes clothes;
			clothes.m_serial = item.guid;
			clothes.m_typeId = item.typeId;
			strncpy(clothes.m_uccKey, item.uccIdx, 9);
			strncpy(clothes.m_uccName, "\xc1\xa4\xba\xb8\xb8\xa6 \xb9\xde\xbe\xc6\xbf\xc0\xb4\xc2 \xc1\xdf...", 0x29); // 정보를 받아오는 중...
			clothes.m_downloadCount = 0;
			UccManager()->AddClothes(0, clothes, true, false);
		}
	}

	return true;
}

// shareddoc.cpp:3325, takes item's count off the own shop's matching item
// (removing it when nothing is left).
bool CSharedDoc::MinusListTradeItem(sTradeItem item) {
	for (std::list<sTradeItem>::iterator it = m_listTradeItem.begin(); it != m_listTradeItem.end(); ++it) {
		if ((*it).index == item.index) {
			if ((*it).guid != item.guid) break;
			if ((*it).typeId != item.typeId) break;

			if ((*it).count > item.count) {
				(*it).count -= item.count;
				return true;
			} else if ((*it).count == item.count) {
				m_listTradeItem.erase(it);
				return true;
			}

			break;
		}
	}

	return false;
}

// shareddoc.cpp:3362, refreshes the visited shops' entry of the stall's
// owner (the own stall for none).
void CSharedDoc::ChageListStallitem(sStall *pStall) {
	std::list<sStall> *pList = WSingleton<CSharedDoc>::Instance()->GetListStallItem();

	if (pStall == NULL) {
		sStall *pMyStall = GetStallItem();
		for (std::list<sStall>::iterator it = pList->begin(); it != pList->end(); ++it) {
			if (pMyStall->ownerUID == (*it).ownerUID) {
				*it = *pMyStall;
			}
		}
	} else {
		for (std::list<sStall>::iterator it = pList->begin(); it != pList->end(); ++it) {
			if (pStall->ownerUID == (*it).ownerUID) {
				*it = *pStall;
			}
		}
	}
}

// shareddoc.cpp:3393, asks the main actor to edit the own offline shop, or
// tells the player the offline shop was not bought.
bool CSharedDoc::SendOfflineTradeEditShop(void) {
	if (IsLocalContent((localContentType_t)0x66)) {
		if (WSingleton<CSharedDoc>::Instance()->GetStallItem() == NULL || WSingleton<CSharedDoc>::Instance()->GetStallItem()->ownerUID == 0) {
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xC1\xF8\xBF\xAD\xC0\xE5 \xB1\xB8\xB8\xC5\xB8\xA6 \xC7\xCF\xC1\xF6\xBE\xCA\xC0\xB8\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9. \xB1\xB8\xB8\xC5\xC7\xCF\xB0\xED \xC0\xCC\xBF\xEB\xC7\xD8 \xC1\xD6\xBC\xBC\xBF\xE4.~", 0, 0, 0, 0));
		} else {
			WSingleton<CSharedDoc>::Instance()->m_tradeMode = 5;
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x232, 0, 0, 0, 0, 0));
		}
	}

	return true;
}

// shareddoc.cpp:3411, asks to visit the offline shop of the visited-shops
// entry with this id, or tells the player the own offline shop is not set up.
bool CSharedDoc::SendOfflineTradeVisitor(unsigned long uid) {
	if (IsLocalContent((localContentType_t)0x66)) {
		if (WSingleton<CSharedDoc>::Instance()->GetStallItem() == NULL || WSingleton<CSharedDoc>::Instance()->GetStallItem()->ownerUID == 0) {
			// The literal is 62 bytes; only its first 32 are known (from its
			// mangled name ??_C@_0DP@HEPMFBFC@): "방문하신 방의 유저는 좌판을 배치"...
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xB9\xE6\xB9\xAE\xC7\xCF\xBD\xC5 \xB9\xE6\xC0\xC7 \xC0\xAF\xC0\xFA\xB4\xC2 \xC1\xC2\xC6\xC7\xC0\xBB \xB9\xE8\xC4\xA1", 0, 0, 0, 0));
		} else {
			std::list<sStall> *pList = WSingleton<CSharedDoc>::Instance()->GetListStallItem();
			sStall stall;
			for (std::list<sStall>::iterator it = pList->begin(); it != pList->end(); ++it) {
				if ((*it).uid == uid) {
					WSendPacket packet(GCT_OFFLINESHOP_REQUEST_VISIT);
					packet.Encode4(WSingleton<CSharedDoc>::Instance()->m_myRoomOwnerUID);
					packet.Encode4((*it).ownerUID);
					packet.Encode1(1);
					packet.Send(SEND_TO_DEFAULT);

					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
					break;
				}
			}
		}
	}

	return true;
}

// shareddoc.cpp:3453
bool CSharedDoc::SendTradeOpenShop(void) {
	if (GetStateTrade() != STATE_TRADE_READY) return false;
	if (GetListTradeItem()->size() <= 0) return false;

	WSendPacket packet(GCT_TRADE_OPEN_SHOP);
	packet.Send(SEND_TO_DEFAULT);

	return true;
}

// shareddoc.cpp:3465
bool CSharedDoc::SendTradeCloseShop(void) {
	if (GetStateTrade() == STATE_TRADE_NONE) return false;

	WSendPacket packet(GCT_TRADE_CLOSE_SHOP);
	packet.Send(SEND_TO_DEFAULT);

	return true;
}

// shareddoc.cpp:3476
bool CSharedDoc::SendTradeEditShop(void) {
	WSendPacket packet(GCT_TRADE_EDIT_SHOP);
	packet.Send(SEND_TO_DEFAULT);

	return true;
}

// shareddoc.cpp:3487
bool CSharedDoc::SendTradeEnterShop(unsigned long uid) {
	if (uid == 0xFFFFFFFF) return false;

	WSendPacket packet(GCT_TRADE_ENTER_SHOP);
	packet.Encode4(uid);
	packet.Send(SEND_TO_DEFAULT);

	return true;
}

// shareddoc.cpp:3499
bool CSharedDoc::SendTradeExitShop(unsigned long uid) {
	if (uid == 0xFFFFFFFF) return false;

	WSendPacket packet(GCT_TRADE_EXIT_SHOP);
	packet.Encode4(uid);
	packet.Send(SEND_TO_DEFAULT);

	return true;
}

// shareddoc.cpp:3511
bool CSharedDoc::SendTradeTitle(std::string title) {
	if (title.length() <= 0) return false;

	WSendPacket packet(GCT_TRADE_EDIT_TITLE);
	packet.EncodeStr(title);
	packet.Send(SEND_TO_DEFAULT);

	return true;
}

// shareddoc.cpp:3523, asks for the visitors of the own shop (state 1 or open).
bool CSharedDoc::SendTradeShowVisitor(void) {
	switch (m_stateTrade) {
	default:
		return false;
	case 1:
	case STATE_TRADE_OPEN:
		WSendPacket packet(GCT_TRADE_SHOW_VISITOR);
		packet.Send(SEND_TO_DEFAULT);
	}

	return true;
}

// shareddoc.cpp:3541, asks for the income of the own shop (state 1 or open).
bool CSharedDoc::SendTradeIncome(void) {
	switch (m_stateTrade) {
	default:
		return false;
	case 1:
	case STATE_TRADE_OPEN:
		WSendPacket packet(GCT_TRADE_INCOME);
		packet.Send(SEND_TO_DEFAULT);
	}

	return true;
}

// shareddoc.cpp:3559, buys item from the shop of the player with the UID
// (only while the own shop is being edited).
bool CSharedDoc::SendTradeBuyItem(unsigned long uid, sTradeItem item) {
	if (m_stateTrade != STATE_TRADE_EDIT) return false;
	if (uid == 0 || uid == 0xFFFFFFFF) return false;
	if (item.guid == 0) return false;
	if (item.typeId == 0) return false;
	if (item.price == 0) return false;
	if ((int)item.count <= 0) return false; // signed in the original (jle)

	WSendPacket packet((enumClientPacket)0x7D);
	packet.Encode4(uid);
	packet.EncodeBuffer(&item, 0xAA);
	packet.Send(SEND_TO_DEFAULT);

	return true;
}

// shareddoc.cpp:3577, collects the ability flags of the local player's
// parts and counts the bonus pang ones (flag bit 0).
void CSharedDoc::SetInitAbilityItem(void) {
	m_flagAbilityItem = 0;
	m_numBonusPangItem = 0;

	for (int i = 0; i < 24; i++) {
		unsigned long flag = ItemManager()->GetIndexAbilityItem(m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_characterInfo.partTypeIds[i]);
		m_flagAbilityItem |= flag;

		if (flag & 1) {
			m_numBonusPangItem++;
		}
	}
}

// shareddoc.cpp:3597, the logo image index of the local player's club set
// (by its type id's serial, one also by the character's; 0 for none).
int CSharedDoc::GetIndexPangyaLogoImage(void) {
	int index = 0;
	switch (m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId & 0x3FFFFFF) {
	case 12: index = 1; break;
	case 128: index = 2; break;
	case 17: index = 3; break;
	case 16: index = 4; break;
	case 20: index = 6; break;
	case 19: index = 8; break;
	case 24: index = 11; break;
	case 25: index = 13; break;
	case 27: index = 16; break;
	case 39: index = 19; break;
	case 43: index = 20; break;
	case 34:
	case 47:
	case 48: index = 21; break;
	case 38: index = 23; break;
	case 44: index = 26; break;
	case 51: index = 27; break;
	case 53: index = 28; break;
	case 52: index = 29; break;
	case 58: index = 31; break;
	case 63:
	case 64:
		if ((m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_characterInfo.typeId & 0x3FFFFFF) == 9)
			index = 58;
		break;
	case 62: index = 62; break;
	case 67: index = 67; break;
	case 68:
	case 69: index = 68; break;
	}

	return index;
}

// shareddoc.cpp:3748
bool CSharedDoc::IsOverlapPart(unsigned long typeId) {
	for (std::list<sItemInfo>::iterator it = m_myPartsList.begin(); it != m_myPartsList.end(); ++it) {
		if ((*it).m_typeId == typeId)
			return true;
	}

	return false;
}

// shareddoc.cpp:3763
int OnlinePlay(void) {
	if (!WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT))
		return 0;

	if (((__rtti_obj = AfxGetTask()) != 0 ? ((IObject*)__rtti_obj)->IsKindOf(&CLobbyTask::m_RTTI) : false)
		|| ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject*)__rtti_obj)->IsKindOf(&CShopTask::m_RTTI) : false))
		return 1;

	if (WSingleton<CSharedDoc>::Instance()->m_gameMode != 0
		|| WSingleton<CSharedDoc>::Instance()->m_roomType == 0x0B || WSingleton<CSharedDoc>::Instance()->m_roomType == 0x0C)
		return 0;

	return 1;
}

// shareddoc.cpp:3777
unsigned char GetCurMap(void) {
	unsigned char map = WSingleton<CSharedDoc>::Instance()->m_curMap;
	return (map > 0x7F && map != 0xFD) ? map - 0x80 : map;
}

// shareddoc.cpp:3796
bool SetCurMap(unsigned char map) {
	IFF_STRUCT::sCourse *course = ItemManager()->FindCourse(0x28000000 | ((map > 0x7F && map != 0xFD) ? map - 0x80 : map));

	if (!course)
		return false;
	WSingleton<CSharedDoc>::Instance()->m_curMap = map;
	WSingleton<CSharedDoc>::Instance()->m_pCurCourse = course;
	return true;
}

// shareddoc.cpp:3782
bool IsCurMapSnow(void) {
	unsigned char map = GetCurMap();

	if (map == 8)
		return true;
	else if (map == 14)
		return true;
	else if (map == 17)
		return true;

	return false;
}

// shareddoc.cpp:3810
std::string CSharedDoc::GetLoginAuthKey(void) {
	return WSingleton<CSharedDoc>::Instance()->m_loginAuthKey;
}

// shareddoc.cpp:3814
unsigned short CSharedDoc::GetSecurityKey(void) {
	return Instance()->m_securityKey;
}

// shareddoc.cpp:3819
void CSharedDoc::SetLoginAuthKey(std::string key) {
	WSingleton<CSharedDoc>::Instance()->m_loginAuthKey = key;
}

// shareddoc.cpp:3824
void CSharedDoc::SetSecurityKey(unsigned short key) {
	Instance()->m_securityKey = key;
}

// shareddoc.cpp:3846, keeps only the items whose GUID the server listed in
// m_list1D0 (with their counts, in step, in m_list1DC); type 2 also takes the
// counts and filters m_myExtraItemList4. Both lists are emptied afterwards.
void CSharedDoc::RefreshItemListFromGuidList(unsigned char type) {
	for (std::list<sItemInfo>::iterator it = m_itemInfoList.begin(); it != m_itemInfoList.end();) {
		std::list<unsigned long>::iterator guidIt = m_list1D0.begin();
		std::list<unsigned short>::iterator countIt = m_list1DC.begin();
		for (; guidIt != m_list1D0.end(); ++guidIt, ++countIt) {
			if ((*it).m_serial == *guidIt)
				break;
		}

		if (guidIt == m_list1D0.end()) {
			it = m_itemInfoList.erase(it);
			continue;
		}

		if (type == 2 && (*it).m_count != *countIt) {
			(*it).m_count = *countIt;
		}

		++it;
	}

	if (type == 2) {
		for (std::list<sItemInfo>::iterator it = m_myExtraItemList4.begin(); it != m_myExtraItemList4.end();) {
			std::list<unsigned long>::iterator guidIt = m_list1D0.begin();
			std::list<unsigned short>::iterator countIt = m_list1DC.begin();
			for (; guidIt != m_list1D0.end(); ++guidIt, ++countIt) {
				if ((*it).m_serial == *guidIt)
					break;
			}

			if (guidIt == m_list1D0.end()) {
				it = m_myExtraItemList4.erase(it);
				continue;
			}

			if ((*it).m_count != *countIt) {
				(*it).m_count = *countIt;
			}

			++it;
		}
	}

	m_list1D0.clear();
	m_list1DC.clear();
}

// shareddoc.cpp:3907, mirrors the two gacha ticket counts of the Japan
// state (m_japanParam1/2) into the item list: updates, removes or adds them.
void CSharedDoc::UpdateGachaTickets(void) {
	unsigned long typeIds[2] = { 0x1A000080, 0x1A000083 };
	unsigned short counts[2];
	counts[0] = (unsigned short)m_japanParam1;
	counts[1] = (unsigned short)m_japanParam2;

	sItemInfo item;
	memset(&item, 0, sizeof(item));

	for (int i = 0; i < 2; i++) {
		std::list<sItemInfo>::iterator it;
		for (it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {
			if ((*it).m_typeId == typeIds[i]) {
				if (counts[i] == 0)
					WSingleton<CSharedDoc>::Instance()->m_itemInfoList.erase(it);
				else
					(*it).m_count = counts[i];
				break;
			}
		}

		if (counts[i] != 0) {
			if (it == WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end()) {
				item.m_typeId = typeIds[i];
				item.m_count = counts[i];
				WSingleton<CSharedDoc>::Instance()->m_itemInfoList.push_back(item);
			}
		}
	}
}

// shareddoc.cpp:3954, the pang the local player loses for leaving a game:
// by room type, raised with the level and the no-manner rate
unsigned long CSharedDoc::GetMyDisconPangPenalty(void) {
	unsigned long penalty = 0;




	switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {


	case 0:
		penalty = 100;
		break;


	case 3:
	case 7:
		penalty = 350;
		break;


	case 1:
		penalty = 300;
		break;


	case 4:
		penalty = 150;
		break;


	case 5:
		penalty = 200;
		break;
	}




	if (m_myLevel >= 6) {

		penalty += (m_myLevel - 1) / 5 * 100;


		if (m_myStatGamesPlayed > 50)
			penalty += (m_myStatNoMannerGames * 100 / m_myStatGamesPlayed - 10) * 10;
	}


	return penalty;
}

// shareddoc.cpp:3996, reads the visibility groups: each line of visgroup.def
// (the original ignores fileName) is a group name followed by the names of
// its members.
void CSharedDoc::LoadVisGroup(const char *fileName) {
	cFile *file = g_resrcmng->GetCFile("visgroup.def", 0xFFFF);
	if (file == 0) {
		return;
	}

	std::vector<std::string> tokens;
	int length = file->Length();
	char *data = new char[length + 1];
	file->Read(data, length);
	CloseCFile(file);

	data[length] = 0;
	char *cursor = data;
	for (;;) {
		while (*cursor == '\n' || *cursor == '\r') {
			cursor++;
		}
		if (*cursor == 0) {
			break;
		}

		char line[0x100];
		char *dest = line;
		while (*cursor != '\n' && *cursor != '\r' && *cursor != 0) {
			*dest++ = *cursor++;
		}
		*dest = 0;

		LineParse(line, tokens);
		if (tokens.size() > 1) {
			std::string &groupName = tokens[0];

			for (size_t i = 1; i < tokens.size(); i++) {
				m_visGroups[groupName].push_back(tokens[i]);
			}
		}
	}
	delete[] data;
}

// shareddoc.cpp:4048
void CSharedDoc::SetFileName(char *const fileName) {
	strcpy(m_fileName, fileName);
}

// shareddoc.cpp:4053, lists the replays of save/ recorded by this player
// with this game version, ten at most
void CSharedDoc::ReplaySavFileCheck(void) {
	WSingleton<CSharedDoc>::Instance()->m_replayFileList.clear();

	char fileName[32] = {0};
	for (int i = 0; i < 100; ++i) {
		sprintf(fileName, "save/pangya_%03d.sav", i);
		FILE *fp = fopen(fileName, "rb");

		if (fp == NULL) {
			continue;
		}

		int version = 0;
		unsigned long uid = 0xFFFFFFFF;
		unsigned char course = 0;
		SYSTEMTIME date;
		int mode = 0;

		fread(&version, 4, 1, fp);
		fread(&uid, 4, 1, fp);
		fread(&course, 1, 1, fp);
		fread(&date, 16, 1, fp);
		fread(&mode, 4, 1, fp);

		if (version == m_value5BD4 && uid == WSingleton<CSharedDoc>::Instance()->m_myUID) {
			sRecordedItemInfo info;

			if (mode == 1) {
				info.m_typeId = 0x1A000043;
			} else if (mode == 2) {
				info.m_typeId = 0x1A000050;
			}
			info.m_course = course;
			info.m_count = 1;
			info.m_date = date;
			info.m_playable = 1;
			strcpy(info.m_fileName, fileName);

			WSingleton<CSharedDoc>::Instance()->m_replayFileList.push_back(info);
		}
		fclose(fp);

		if (WSingleton<CSharedDoc>::Instance()->m_replayFileList.size() >= 10) {
			break;
		}
	}
}

// shareddoc.cpp:4118
std::list<std::string> *CSharedDoc::GetVisGroup(const char *name) {
	std::map<std::string, std::list<std::string> >::iterator it = m_visGroups.find(name);
	if (it == m_visGroups.end()) {
		return 0;
	}

	return &it->second;
}

// shareddoc.cpp:4129, false when the club set needs a level above the
// local player's.
bool CSharedDoc::CheckUnderLevelClubSet(unsigned long typeId) {
	bool result = true;

	IFF_STRUCT::sClubSet *pClubSet = WSingleton<CSharedDoc>::Instance()->m_itemManager.FindClubSet(typeId);

	if (pClubSet->m_level & 0x80) {
		if ((pClubSet->m_level & 0x7F) < m_myLevel)
			result = false;
	}

	return result;
}

// shareddoc.cpp:4154
void CSharedDoc::InitMapEvent(void) {
}

// shareddoc.cpp:4173
bool CSharedDoc::IsMapEventActive(int map) {
	return false;
}

// shareddoc.cpp:4190
unsigned long CSharedDoc::GetMapEventPangRate(int map) {
	return 100;
}

// shareddoc.cpp:4204
unsigned long CSharedDoc::GetMapEventExpRate(int map) {
	return 100;
}

// shareddoc.cpp:4245
void CSharedDoc::SetIdentity(int identity) {
	m_userInfo.identity = identity;
}

// shareddoc.cpp:4250
void CSharedDoc::AccumulateIdentity(int identity) {
	m_userInfo.identity |= identity;
}

// shareddoc.cpp:4256
__int64 CSharedDoc::RemainOwnCash(unsigned long buyType, __int64 price) {
	__int64 remain = WSingleton<CSharedDoc>::Instance()->m_myCash;
	switch (buyType) {
	case 2:
	case 4:
		remain -= price;
		break;
	}

	return remain;
}

// shareddoc.cpp:4301
void CSharedDoc::RefreshAllCookie(void) {
	WSingleton<CSharedDoc>::Instance()->m_myCash = S5::ExchangeOwnCashToCookie(WSingleton<CSharedDoc>::Instance()->m_outsideCash, WSingleton<CSharedDoc>::Instance()->m_myBonusCash, 100);
}

// shareddoc.cpp:4307
bool CSharedDoc::CanUseRookieChannelMap(unsigned int map) {
	bool canUse = true;
	IFF_STRUCT::sCourse *pCourse = ItemManager()->FindCourse(0x28000000 | ((map > 0x7F && map != 0xFD) ? map - 0x80 : map));
	if (pCourse) {
		if ((pCourse->courseFlags & 0xF) > 3) {
			return false;
		}
	}

	if (map == 8 || map == 1 || map == 9 || map == 0xE || map == 6)
		canUse = false;

	return canUse;
}

// shareddoc.cpp:4330
void CSharedDoc::SetServerTime(_SYSTEMTIME &time) {
	WSingleton<CSharedDoc>::Instance()->m_serverTime = time;

	if (IsLocalContent((localContentType_t)0x51)) {
		if (WSingleton<CCardManager>::IsInstantiated())
			WSingleton<CCardManager>::Instance()->ResetSpecialBuffTick();
	}

	WSingleton<CSharedDoc>::Instance()->m_serverTimeTick = GetTickCount();
}

// shareddoc.cpp:4344
_SYSTEMTIME &CSharedDoc::GetServerTime(void) {
	return Instance()->m_serverTime;
}

// shareddoc.cpp:4350
void CSharedDoc::SetDirectMoveRoomInfo(unsigned char server, unsigned short room, unsigned long uid) {
	m_directMoveServer = server;
	m_directMoveRoom = room;
	m_directMoveUID = uid;
}

// shareddoc.cpp:4357
bool CSharedDoc::GetDirectMoveRoomInfo(unsigned char *server, unsigned short *room, unsigned long *uid) {
	if (m_directMoveUID == 0xFFFFFFFF || m_directMoveRoom == 0xFFFF)
		return false;

	*server = m_directMoveServer;
	*room = m_directMoveRoom;
	*uid = m_directMoveUID;

	return true;
}

// shareddoc.cpp:4370
void CSharedDoc::ClearDirectMoveRoomInfo(void) {
	m_directMoveServer = 0xFF;
	m_directMoveRoom = 0xFFFF;
	m_directMoveUID = 0xFFFFFFFF;
}

// shareddoc.cpp:4377
int CSharedDoc::FindUserInfoTimeUID(unsigned long uid, std::map<unsigned long, sUserInfoTime>::iterator &it) {
	it = m_userInfoTimeMap.find(uid);
	return it == m_userInfoTimeMap.end() ? FALSE : TRUE;
}

// shareddoc.cpp:4386, the map has no GUID key: a linear search.
int CSharedDoc::FindUserInfoTimeGUID(unsigned long guid, std::map<unsigned long, sUserInfoTime>::iterator &it) {
	if (!m_userInfoTimeMap.empty()) {
		for (std::map<unsigned long, sUserInfoTime>::iterator i = m_userInfoTimeMap.begin(); i != m_userInfoTimeMap.end(); ++i) {
			if (i->second.m_userInfo.m_guid == guid) {
				it = i;
				return TRUE;
			}
		}
	}

	it = m_userInfoTimeMap.end();
	return FALSE;
}

// shareddoc.cpp:4405
int CSharedDoc::FindUserInfo(unsigned long key, std::map<unsigned long, sBriefUserInfo>::iterator &it) {
	int count = m_userMap.size();

	if (count) {
		std::map<unsigned long, sBriefUserInfo>::iterator found = m_userMap.find(key);
		if (found != m_userMap.end()) {
			it = found;
			return TRUE;
		}
	}

	return FALSE;
}

// shareddoc.cpp:4423
int CSharedDoc::FindUserInfoUID(unsigned long uid, std::map<unsigned long, sBriefUserInfo>::iterator &it) {
	if (m_userMap.size() == 0)
		return FALSE;

	std::map<unsigned long, sBriefUserInfo>::iterator i = m_userMap.begin();
	for (; i != m_userMap.end(); ++i) {
		if (uid == (*i).second.id) {
			it = i;
			return TRUE;
		}
	}

	return FALSE;
}

// shareddoc.cpp:4444, the map has no user id key: a linear search, case
// insensitive (both names are lowered in place).
int CSharedDoc::FindUserInfoTimeUserID(const char *userId, std::map<unsigned long, sUserInfoTime>::iterator &it) {
	if (m_userInfoTimeMap.size() != 0) {
		char *pUserId = strlwr((char *)userId);

		for (std::map<unsigned long, sUserInfoTime>::iterator i = m_userInfoTimeMap.begin(); i != m_userInfoTimeMap.end(); ++i) {
			char *pNickname = strlwr(i->second.m_userInfo.m_nickname);
			if (strcmp(pUserId, pNickname) == 0) {
				it = i;
				return TRUE;
			}
		}
	}

	it = m_userInfoTimeMap.end();
	return FALSE;
}

// shareddoc.cpp:4466
int CSharedDoc::InsertUserInfoTime(unsigned long uid, sUserInfoTime &info) {
	m_userInfoTimeMap[uid] = info;
	return TRUE;
}

// shareddoc.cpp:4488
CPeriodContents *CSharedDoc::GetPeriodDoc(void) {
	return m_pPeriodDoc;
}

// shareddoc.cpp:4499
void CSharedDoc::SetPeriodDoc(void) {
	m_pPeriodDoc = NULL;
	WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0x92, m_pPeriodDoc);
}

// The approach missions played blind (IsCurrentMissionBlind).
static const int s_blindMissions[] = { 4, 0x15 };

// shareddoc.cpp:4506
bool CSharedDoc::IsCurrentMissionBlind(void) {
	bool blind = false;
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 10) {
		for (unsigned int i = 0; i < sizeof(s_blindMissions) / sizeof(s_blindMissions[0]); i++) {
			if (s_blindMissions[i] == GOLFDOC()->GetApproachMissionType()) {
				blind = true;
				break;
			}
		}
	}

	return blind;
}

// shareddoc.cpp:4527, whether one of the owned characters wears the part
// (item id serial) or the aux part (type id typeId).
int CSharedDoc::IsEquipParts(unsigned long serial, unsigned long typeId) {
	if ((typeId >> 26) == 2) {
		for (std::map<unsigned int, sCharacterInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++it) {
			for (int i = 0; i < 24; i++) {
				if (it->second.partTypeIds[i] != 0) {
					if (serial == it->second.partIds[i])
						return TRUE;
				}
			}
		}
	} else if ((typeId >> 26) == 0x1C) {
		for (std::map<unsigned int, sCharacterInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++it) {
			for (int i = 0; i < 5; i++) {
				if (it->second.auxParts.tids[i] != 0) {
					if (typeId == it->second.auxParts.tids[i])
						return TRUE;
				}
			}
		}
	}

	return FALSE;
}

// shareddoc.cpp:4565
bool CSharedDoc::IsEquipParts(sCharacterInfo &characterInfo, unsigned long itemId, unsigned long typeId) {
	if ((typeId >> 26) == 2) {
		for (int i = 0; i < 24; i++) {
			if (characterInfo.partTypeIds[i] != 0) {
				if (itemId == characterInfo.partIds[i])
					return true;
			}
		}
	} else if ((typeId >> 26) == 0x1C) {
		for (int i = 0; i < 5; i++) {
			if (characterInfo.auxParts.tids[i] != 0) {
				if (typeId == characterInfo.auxParts.tids[i])
					return true;
			}
		}
	}

	return false;
}

// shareddoc.cpp:4598, the past season's map statistics of a course.
std::map<unsigned char, sMapStatistics> &CSharedDoc::GetMyPastMapStat(unsigned char course) {
	unsigned char index = 0;

	switch (course) {
	case 0:
		index = 0;
		break;

	case 10:
		index = 1;
		break;

	case 51:
		index = 2;
		break;

	case 5:
		index = 3;
		break;
	}

	return m_myPastMapStat[index];
}

// The power bar's scale (no symbol; set elsewhere at run time).
extern "C" {
static float data_af1958;
}

// shareddoc.cpp:5040, the power bar's position for the distance from the
// local player's ball to the pin: 500 when the pin is out of the club's range.
float CalcPowerBarPosition(void) {
	WVector pinPos = GOLFDOC()->GetHoleData().m_cupPos;
	CGolfBall &ball = GolfBall();

	WVector diff = ball.m_pos - pinPos;
	diff.y = 0.0f;
	float range = GolfClub().GetRange(0) * 3.2f;

	if (diff.Magnitude() > range) {
		return 500.0f;
	}

	char text[32] = "";
	sPlayerData *pPlayer = PLAYER(GOLFDOC()->m_myPlayerIndex);
	pinPos = GOLFDOC()->GetHoleData().m_cupPos;
	WVector toPin = pinPos - pPlayer->m_ballPos;
	toPin.y = 0.0f;

	float yard = sqrtf(toPin * toPin) * 0.3125f;
	if (yard < 100.0f) {
		sprintf(text, "%.1f", yard);
	} else {
		sprintf(text, "%.0f", yard);
	}

	float value = (float)atof(text);
	value *= 3.2f;
	return value / range * data_af1958 + 140.0f;
}

// shareddoc.cpp:5076
void PerformPowerBarSetting(void) {
	float position = CalcPowerBarPosition();

	IActor *pScreen = AfxGetTask()->GetActor("Screen");
	if (pScreen != 0) {
		pScreen << MsgObject(0, 0xA1, (intptr_t)&position, 0, 0, 0, 0);
	}
}
