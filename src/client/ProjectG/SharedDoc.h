// Original: d:\build\custom\temp\code\client\projectg\shareddoc.h
#ifndef PG_CLIENT_PROJECTG_SHAREDDOC_H
#define PG_CLIENT_PROJECTG_SHAREDDOC_H

#include "MascotInfo.h"
#include <windows.h>

#include <list>
#include <map>
#include <string>
#include <vector>

#include "defines.h"
#include "padding.h"
#include "singleton.h"
#include "shared/globalgamedefine.h"
#include "shared/globalnetworkdefine.h"
#include "shared/ItemManager.h"
#include "shared/Localize.h"
#include "shared/classdefine.h"
#include "globalgamedefine.h"
#include "parttidlist.h"
#include "hatManager.h"
#include "wmath.h"
#include "wflag.h"
#include "buddymanager.h"

class CGolfDoc;
class CExhibition;
class FrArea;
class FrComboBox;
class WVector;
class CPeriodContents;
struct sPlayerData;
struct sFurniture_List;  // realmyroomdata.h
struct sGiftInfo;        // panggiftdlg.h
struct sGameServerInfo;  // messengerdlg.h
struct sWinningPrize;    // sPlayerData::m_prizeItems' type (GolfDoc.h names it sPrizeItems)
struct sPrizeInfo;       // 12 bytes, sPlayerData's extra prizes at 0x576

namespace _guild {
class CGuildActor;
}

// The personal shop's state (m_stateTrade). Only the values the code
// tests are named; 1 is not seen yet.
enum eStateTrade {
	STATE_TRADE_NONE = 0,  // no shop: SendTradeCloseShop does nothing
	STATE_TRADE_READY = 2, // the shop is set up: SendTradeOpenShop may open it
	STATE_TRADE_OPEN = 3,  // the own shop is open (FrTradeDlg::OnCloseBnUp closes it)
	STATE_TRADE_EDIT = 4   // the shop's items are being edited (CheckTradeItem)
};

// The replay mode of the current game (GetPlayingMode). The PDB has no
// record of the enumerators: names from usage.
enum eReplayModeType {
	REPLAY_MODE_GAME = 1,     // a normal game (the constructor's value; TaskMain sends packet 0xAD when it leaves 3 for it)
	REPLAY_MODE_PLAYBACK = 2, // a replay is playing (m_replayState drives it)
	REPLAY_MODE_RETURN = 3,   // the replay was closed from its dialog, back to the my room (CGolfRule::OnReplayModeDlgResult)
};

// Who a chat line can be addressed to, as bit flags (InitReservedChatList
// adds the "to team" entry for the first bit and the "to guild" entry for
// the second).
enum eReservedChatPartner {
	RESERVED_CHAT_PARTNER_TEAM = 0x1,
	RESERVED_CHAT_PARTNER_GUILD = 0x2,
};

#pragma pack(push, 1)
// The local player's medals per course (CSharedDoc 0x5DA), 0x4E bytes:
// gold, silver and bronze counts for each of the 13 courses
// (CheckGoldMedal/CheckSilverMedal/CheckBronzeMedal sum one column).
struct sTrophyStatistics {
	unsigned short m_medals[13][3]; // 0x0, [course][gold, silver, bronze]
};

// One course's record of a player, 0x2B bytes (the 0x14-entry arrays of
// sUserInfo at 0x2B4/0x610 and of sUserInfoTime at 0xFF5).
struct sMapStatistics {
	sMapStatistics(void) {
		m_course = 0xFF;
	}

	unsigned char m_course;         // 0x0, the course index; 0xFF: empty
	PADDING(0x1, 0x1D);
	char m_bestScore;               // 0x1D, the best score; 0x7F when there is no record
	__int64 m_bestPang;             // 0x1E, the most pang earned (FrUserInfoForm::OnScoreListOwnerDraw prints it "%I64d")
	unsigned long m_itemTypeId;     // 0x26, an item's type id (OnScoreListOwnerDraw draws its icon, FindCommonItem)
	PADDING(0x2A, 0x2B);
};

// sPangYaUserInfo::eventFlags as sUserInfo keeps it (0x73): one-bit flags
// (the ghost converters copy bits 1, 3, 4 and 5 one by one).
struct sUserEventFlags {
	unsigned char : 1;              // bit 0
	unsigned char gender : 1;       // bit 1, the player's sex (MessengerDlg's sex icon; the tutorial's character 0x4000000 + gender)
	unsigned char : 1;              // bit 2
	unsigned char eventFlag3 : 1;   // bit 3 (CIntrusion::DoIntrusionAccess)
	unsigned char eventFlag4 : 1;   // bit 4 (CIntrusion::DoIntrusionAccess; FamilyMain tests mask 0x10)
	unsigned char eventFlag5 : 1;   // bit 5 (CIntrusion::DoIntrusionAccess)
	unsigned char : 2;              // bits 6-7
};

// The equipped club set's record of a sUserInfo (0xB41, 0x12 bytes): the
// item id, its type id and the upgrade level of each stat.
struct sClubSetRecord {
	unsigned long id;           // 0x0, the club set's item id (sItemInfo::m_serial)
	unsigned long typeId;       // 0x4, its type id
	short statLevel[5];         // 0x8, sItemInfo::m_clubStatLevel
};

// A player's full information, 0xB92 bytes, byte-packed. CSharedDoc keeps
// the room's players in an array of four (0x12D0, the constructor calls
// ??0sUserInfo@@QAE@XZ for each, 0xB92 apart; SetIndex copies the local
// player's 0x10D-byte user info over the first one) and each sUserInfoTime
// holds one at 0x4. The local player's members of CSharedDoc from 0x3E0 on
// (level 0x539, pang 0x53A, character 0x62C, club 0x630, equipped items
// 0x638) follow the same layout. The constructor (0x40fa70, emitted by
// shareddoc.cpp, no line marks) fills two arrays of 0x14 0x2B-byte records
// at 0x2B4 and 0x610 and zeroes the character. Only the members the
// transcribed callers use are named.
struct sUserInfo {
	sUserInfo(void);

	unsigned short m_slot;          // 0x0, callers set it to 0xFFFF (meaning not traced)
	// 0x2-0x10F: a copy of the player's sPangYaUserInfo (0x10D bytes, SetIndex
	// copies it as one; the members below mirror its layout).
	union {
		struct {
			char m_nickname[0x16];          // 0x2, the player's name (FrPlayerDlg::SetState shows it); sPangYaUserInfo::id
			char m_name[0x16];              // 0x18, the second name (sPangYaUserInfo::nickname; FrPlayerDlg::SetName, the ghost converters)
			char m_guildName[0x15];         // 0x2E, sPangYaUserInfo::guildName
			char m_guildMark[0xC];          // 0x43, sPangYaUserInfo::guildMark
			PADDING(0x4F, 0x53);
			unsigned long m_identity;       // 0x53, sPangYaUserInfo::identity
			PADDING(0x57, 0x5B);
			unsigned long m_guid;           // 0x5B, sPangYaUserInfo::guid
			PADDING(0x5F, 0x6B);
			unsigned long m_guildUID;       // 0x6B, sPangYaUserInfo::guildUID (the login reply requests its RSS)
			PADDING(0x6F, 0x73);
			sUserEventFlags m_eventFlags;   // 0x73, sPangYaUserInfo::eventFlags
			PADDING(0x74, 0x10B);
			unsigned long m_uid;            // 0x10B, the player's UID (sPangYaUserInfo + 0x109)
		};
		sPangYaUserInfo m_pangyaInfo;   // 0x2, the same block as one struct (FrUserInfoForm::SetContent)
	};
	// 0x10F-0x1FA: the player's sPangYaUserStatistics (0xEB bytes, byte-packed;
	// the ghost converters copy it as one).
	union {
		struct {
			unsigned long m_statShots;      // 0x10F, the statistics' first dword (CTask::OnPacketCommon copies the block from here)
			PADDING(0x113, 0x155);
			unsigned long m_value155;       // 0x155
			unsigned char m_level;          // 0x159, the grade of the skins/match rules derives from it
			__int64 m_pang;                 // 0x15A
			PADDING(0x162, 0x1AC);
			unsigned long m_ladderPoint;    // 0x1AC, the ladder channel's points (CHeadIcon::SetLevel shows ladder_<points/100>)
			PADDING(0x1B0, 0x1C0);
			unsigned long m_statCombos;     // 0x1C0, statistics +0xB1 (sPangYaUserStatistics::combos); CHoleInfo::Display prints it by the "complete" overlay
			PADDING(0x1C4, 0x1FA);
		};
		sPangYaUserStatistics m_statistics; // 0x10F, the same block as one struct
	};
	sTrophyStatistics m_trophy;     // 0x1FA, the player's medals per course
	// 0x248-0x2B4: the player's equipment, one sUserEquip (like
	// CSharedDoc::m_myEquip and m_realEquip); callers copy it as one.
	union {
		struct {
			unsigned long m_equipHead;      // 0x248, the equipped caddie (sUserEquip::caddieId)
			unsigned long m_characterId;    // 0x24C, the equipped character (CSharedDoc::m_myCharacterId)
			unsigned long m_clubId;         // 0x250, the equipped club set (CSharedDoc::m_myClubId)
			unsigned long m_ballId;         // 0x254, the equipped ball's type id (0x14xxxxxx; CGallery::HandleMsg)
			unsigned long m_equipItem[10];  // 0x258, type ids of the equipped (consumable) items (CGolfRule::CanCancelShot searches the first player's)
			PADDING(0x280, 0x298);
			unsigned long m_skinTypeIds[5]; // 0x298, the skin type ids (FrUserInfoForm::OnCustomProc animates when the first is set; user_info OnPetOwnerDraw)
			unsigned long m_skinTypeId;     // 0x2AC, the equipped skin's type id (HeadIcon SetLevel)
			unsigned long m_mascotId;       // 0x2B0, the equipped mascot's id (the block's end; OnToppage_FamilyUp copies CSharedDoc::m_myMascotId)
		};
		sUserEquip m_equip;                 // 0x248, the same block as one struct
	};
	sMapStatistics m_mapStatistics[20];  // 0x2B4, by course (the constructor marks them empty)
	sMapStatistics m_mapStatistics2[20]; // 0x610, likewise
	sCharacterInfo m_characterInfo; // 0x96C, the equipped character (RequestUccInfo; zeroed by the constructor)
	sCaddieInfo m_caddieInfo;       // 0xB28, the equipped caddie (typeId: CGolfCaddie::OnInit, the last argument of CGolfRule::AddPlayer; cadItemTypeId: CGolfCaddie::sFindCaddie)
	sClubSetRecord m_clubSet;       // 0xB41, the equipped club set (id: FrPlayerDlg::SetClubControl; typeId 0x10xxxxxx: CPowerGauge::GetAddAirnightLuckyArea, CSoundItemManager)
	sMascotInfo m_mascot;           // 0xB53, the equipped mascot (0x3F bytes; CMascot::HandleMsg message 0x4E fills it)
};

// A player's information with the time it was received (CSharedDoc keeps
// them in a std::map keyed by UID at 0x5CB4; FrUserInfoForm::SetInfo shows
// one). 0x13FE bytes, byte-packed (the map's pair copy is `rep movsd` 0x4FF +
// `movsw`). The implicit constructor (0x4116a0) calls sUserInfo's and fills
// two more arrays of 0x14 0x2B-byte records at 0xC99 and 0xFF5 and the
// members from 0x13AF on.
struct sUserInfoTime {
	unsigned long m_time;           // 0x0
	sUserInfo m_userInfo;           // 0x4
	int m_specialTrophyCount;       // 0xB96, the elements of m_pArray1 (FrUserInfoForm::SetInfo)
	sSpecialTrophy *m_pArray1;      // 0xB9A, heap array of the special trophies (ClearUserInfoTimeMap deletes it; SetInfo lists it)
	int m_guildTrophyCount;         // 0xB9E, the elements of m_pArray2 (FrUserInfoForm::SetInfo)
	sGuildTrophy *m_pArray2;        // 0xBA2, heap array of the guild trophies
	unsigned long m_guildPang;      // 0xBA6, FrUserInfoForm::SetInfo copies it to FrUserInfoForm::m_guildPang
	unsigned long m_guildMyPercent; // 0xBAA, FrUserInfoForm::SetInfo copies it to FrUserInfoForm::m_guildMyPercent
	sPangYaUserStatistics m_seasonStatistics;  // 0xBAE, the season's statistics (FrUserInfoForm::SetInfo copies it, the constructor leaves it)
	sMapStatistics m_mapStatistics[20];        // 0xC99, the player's course records (FrUserInfoForm::SetInfo)
	sMapStatistics m_classicMapStatistics[20]; // 0xFF5, the classic courses' records (FrUserInfoForm::SetInfo)
	sTrophyStatistics m_trophyStatistics;      // 0x1351, the medals per course (FrUserInfoForm::SetInfo)
	int m_seasonSpecialTrophyCount; // 0x139F, the elements of m_pArray3 (FrUserInfoForm::SetInfo, the second record)
	sSpecialTrophy *m_pArray3;      // 0x13A3, heap array of special trophies, like m_pArray1
	int m_seasonGuildTrophyCount;   // 0x13A7, the elements of m_pArray4
	sGuildTrophy *m_pArray4;        // 0x13AB, heap array of guild trophies, like m_pArray2
	sFriendExtra m_friendExtra;     // 0x13AF, the friend record (the constructor resets it; FrUserInfoForm::SetInfo copies it)
	PADDING(0x13FA, 0x13FE);
};

// The real header of sTrophyStatistics and sMapStatistics is not known
// (shared code: CheckGoldMedal and CheckCourseMaster of sharedtitledoc.inl
// take them); they live above until it is found.
CHECK_SIZE(sTrophyStatistics, 0x4E);
CHECK_SIZE(sMapStatistics, 0x2B);
CHECK_FIELD(sMapStatistics, m_course, 0x0);
CHECK_FIELD(sMapStatistics, m_bestScore, 0x1D);
CHECK_FIELD(sMapStatistics, m_bestPang, 0x1E);
CHECK_FIELD(sMapStatistics, m_itemTypeId, 0x26);

// A player slot of the room (CSharedDoc::m_roomSlotMap, keyed by UID). The
// original header and the size are not known yet (GolfTask emits its
// constructor; UccLibrary reads an sCharacterInfo at 0xD0). Only the members
// the transcribed callers use are named.
struct sRoomSlot {
	sRoomSlot(void);                // shareddoc.h:239 (GolfTask emits it)

	FrArea *m_pPetArea;             // 0x0, the lobby's pet area of the slot (CLobbyMain msg 0x5A)
	CExhibition *m_pPet;            // 0x4, the slot's pet exhibition (CLobbyMain::HidePet deletes it; its CPetFrame* at +0xA4)
	CPartTidList m_partTidList;     // 0x8, the equipped parts' type and item ids (NetResourceManager::ClearGarbage)
	sCharacterInfo m_characterInfo; // 0xD0, the player's character (RequestUccInfo(sRoomSlot&))
	union {
		unsigned char m_angelFlag;  // 0x28C, bits 0-1: the angel wing bits (sSlotInfo::m_angelFlag; CLobbyMain msg 0x09)
		struct {
			unsigned char flag0 : 1; // bit 0, copied from sSlotInfo::m_flags57.flag0
			unsigned char flag1 : 1; // bit 1, copied from sSlotInfo::m_flags57.flag1
			unsigned char : 6;
		} m_angelFlags;             // 0x28C, the bitfield view of m_angelFlag (CGolfTask, CLobbyMain msg 0x5A)
	};
	PADDING(0x28D, 0x290);
};
CHECK_FIELD(sRoomSlot, m_pPetArea, 0x0);
CHECK_FIELD(sRoomSlot, m_pPet, 0x4);
CHECK_FIELD(sRoomSlot, m_partTidList, 0x8);
CHECK_FIELD(sRoomSlot, m_characterInfo, 0xD0);
CHECK_FIELD(sRoomSlot, m_angelFlag, 0x28C);
CHECK_SIZE(sRoomSlot, 0x290);

// shareddoc.h:239
inline sRoomSlot::sRoomSlot(void) {
	m_pPetArea = 0;
	m_pPet = 0;
}

// A saved replay of the replay file list (CSharedDoc::m_replayFileList),
// 0xDD bytes (list<sRecordedItemInfo>::_Buynode copies 0x37 dwords and a
// byte). The name is the PDB's; its implicit constructor (shareddoc.cpp,
// no line record) only constructs the sItemInfo at 0x0. The original header
// is not known. FrReplayModeDlg::OnOkBtnUp searches it by file name.
struct sRecordedItemInfo : sItemInfo {
	unsigned long m_course;         // 0xA8, the course byte of the save file
	SYSTEMTIME m_date;              // 0xAC, when the replay was recorded
	char m_fileName[0x20];          // 0xBC, compared (_stricmp) with CSharedDoc::m_fileName
	unsigned char m_playable;       // 0xDC, 1 when the replay can be played (CRealMyRoomMain)
};

// The name the callers transcribed first.
typedef sRecordedItemInfo sReplayFileInfo;

// One player's line of the game's result list (CSharedDoc::m_gameResultList),
// 0x190 bytes; FrFinalRankDlg lists and sorts them, GUILDMATCHUP returns one.
// The name is the PDB's (std::vector<sRivalData>, sRivalData::operator=,
// emitted by shareddoc.cpp, copies it member by member: the gaps at 0x37,
// 0x92, 0x142, 0x155 and 0x15D it skips are natural alignment). Only the
// members the transcribed functions read are named. sGameResultInfo is the
// name the callers transcribed first.
#pragma pack(push, 8)
struct sRivalData {
	unsigned long m_id;             // 0x0, the player's id (FrRankForm::OnRankRBtnUp hands it to CUserInfo::SetInfo)
	unsigned long m_uid;            // 0x4
	char m_nickname[0x16];          // 0x8
	char m_guildName[0x15];         // 0x1E, the player's guild (FrResultTeamDlg::OnInit names the winning guild)
	unsigned char m_team;           // 0x33, 0: red, 1: blue (FrResultDlg::FindWinner sums the team results by it)
	unsigned short m_level;         // 0x34, the player's level (FrDemonProcessRank)
	unsigned char m_state;          // 0x36, 2: finished, 3: skipped
	int m_holeScore[18];            // 0x38, one per hole (operator= copies 0x12 dwords; CLobbyTask::OnPacket 0x6B)
	unsigned char m_holeShots[18];  // 0x80, one per hole (FrScoreDlg GetGuildVersus/GetMyCurHoleInGuildMatch)
	__int64 m_holePang[18];         // 0x98, one per hole (operator= copies them as dword pairs; CLobbyTask::OnPacket 0x6B)
	__int64 m_pang;                 // 0x128, third sort key (FinalRankCompare)
	__int64 m_extraPang;            // 0x130, added to m_pang for the result's bonus (FrResultDlg::OnBonusInit)
	int m_score;                    // 0x138, second sort key
	unsigned char m_totalShots;     // 0x13C, the shots so far (CLobbyTask::OnPacket 0x6B)
	unsigned char m_hole;           // 0x13D
	unsigned char m_rank;           // 0x13E, 0x10 and above are not listed
	unsigned char m_value13F;       // 0x13F
	unsigned char m_sortGroup;      // 0x140, first sort key (FinalRankCompare)
	unsigned char m_finishOrder;    // 0x141, the lobby's finish sort key (FinishUserCompare)
	WVector m_ballPos;              // 0x144, the player's ball position (CSpot::UpdateRivals copies it)
	int m_joined;                   // 0x150, set to 1 when SetPlayer adds the player to the result list
	unsigned char m_present;        // 0x154, set to 1 when SetPlayer adds the player to the result list
	unsigned long m_titleTypeId;    // 0x158, the equipped title's type id (FrDemonProcessRank::GetTitleBitmap)
	unsigned char m_characterType;  // 0x15C, sSlotInfo::m_attribute >> 5 (GolfRuleTimeattack SetPlayer)
	int m_hasEmblem;                // 0x160, the player's guild UID (GolfRuleTimeattack SetPlayer; rename pending, FinalRankDlg/ResultDlg test it)
	char m_emblemName[0xC];         // 0x164 (operator= copies 0xC bytes)
	unsigned long m_uniteGuildIndex; // 0x170, second sort key of the unite guild result, higher first (UniteGuildIndexCompare)
	unsigned int m_approachDistanceCopy; // 0x174, CLobbyTask::OnPacket 0x6C stores the approach distance here and at 0x178
	unsigned int m_approachDistance; // 0x178, second sort key of the approach ranking, lower first (ApproachRankUserCompare)
	int m_approachPoint;            // 0x17C, third sort key of the approach ranking, higher first
	unsigned long m_identity;       // 0x180, sPangYaUserInfo::identity flags (GolfRuleTimeattack SetPlayer, FrScoreDlg::OnGolferListOwnerDraw)
	unsigned long m_mascotTypeId;   // 0x184, sSlotInfo::m_mascotTypeId (SetPlayer)
	unsigned char m_itemBoostFlags; // 0x188, sSlotInfo::m_itemBoostFlags (SetPlayer)
	unsigned char m_extraFlags;     // 0x189, sSlotInfo::m_extraFlags (SetPlayer)
};
#pragma pack(pop)

// shareddoc.h:211, finds a rival by UID (std::find_if).
struct sFindRival {
	// shareddoc.h:212
	sFindRival(unsigned long uid) : m_uid(uid) {}

	// shareddoc.h:214
	bool operator()(sRivalData &rival) {
		return rival.m_uid == m_uid;
	}

	unsigned long m_uid;            // 0x0
};
CHECK_SIZE(sFindRival, 0x4);
CHECK_FIELD(sFindRival, m_uid, 0x0);

// The two guilds of a guild match (CSharedDoc 0x49F3, 0x4A bytes;
// FrResultTeamDlg::OnInit copies it as one into its own). The original name
// is not known.
struct sGuildMatchInfo {
	PADDING(0x0, 0x8);
	char m_guildName[2][0x15];      // 0x8, the red and blue guild's names (FrResultTeamDlg::OnRedOwnerDraw/OnBlueOwnerDraw)
	char m_guildMark[2][0xC];       // 0x32, the red and blue guild's emblem names (NetResourceManager::GetEmblemByName)
};
CHECK_SIZE(sGuildMatchInfo, 0x4A);
CHECK_FIELD(sGuildMatchInfo, m_guildName, 0x8);

// A player of the award ceremony (CSharedDoc 0x456E, twelve, 8 bytes;
// FrAwardDlg looks them up by UID). The original name is not known.
struct sAwardPlayer {
	unsigned long m_uid;            // 0x0
	int m_pang;                     // 0x4, the pang won (FrResultDlg::OnPangInit prints it "%d")
};
CHECK_SIZE(sAwardPlayer, 0x8);

// A second, 12-byte view of the award players from 0x459E: FrAwardDlg::
// GetMedalIndex compares the UID of six of them with a 12-byte stride (the
// static inits walk the same area as sAwardPlayer). The original name is not
// known.
struct sMedalWinner {
	unsigned long m_uid;            // 0x0
	PADDING(0x4, 0xC);
};
CHECK_SIZE(sMedalWinner, 0xC);

// The exp of a caddie level (CSharedDoc 0x4FF8, three of them, 8 bytes;
// FrCaddieInfoDlg::OnInit reads the first dword by the caddie's level). The
// original name is not known.
struct sCaddieLevelExp {
	unsigned long m_exp;            // 0x0
	unsigned long m_value4;         // 0x4, set by LoadLevelTable (0x208, 0x4B0, 0x7F8)
};
CHECK_SIZE(sCaddieLevelExp, 0x8);

// A player level of the level table (CSharedDoc 0x4DC0, 71 of them, 8 bytes;
// LoadLevelTable fills it from the exe's table). The original name is not known.
struct sLevelExp {
	unsigned long m_exp;            // 0x0, exp of this level
	unsigned long m_totalExp;       // 0x4, exp of all the levels up to and including this one
};
CHECK_SIZE(sLevelExp, 0x8);
CHECK_FIELD(sAwardPlayer, m_uid, 0x0);
CHECK_FIELD(sAwardPlayer, m_pang, 0x4);

CHECK_SIZE(sRivalData, 0x190);
CHECK_FIELD(sRivalData, m_level, 0x34);
CHECK_FIELD(sRivalData, m_state, 0x36);
CHECK_FIELD(sRivalData, m_holeShots, 0x80);
CHECK_FIELD(sRivalData, m_pang, 0x128);
CHECK_FIELD(sRivalData, m_extraPang, 0x130);
CHECK_FIELD(sRivalData, m_titleTypeId, 0x158);
CHECK_FIELD(sRivalData, m_hasEmblem, 0x160);
CHECK_FIELD(sRivalData, m_uniteGuildIndex, 0x170);
CHECK_FIELD(sRivalData, m_identity, 0x180);
CHECK_FIELD(sRivalData, m_joined, 0x150);
CHECK_FIELD(sRivalData, m_present, 0x154);
CHECK_FIELD(sRivalData, m_mascotTypeId, 0x184);
CHECK_FIELD(sRivalData, m_itemBoostFlags, 0x188);
CHECK_FIELD(sRivalData, m_extraFlags, 0x189);
CHECK_FIELD(sRivalData, m_characterType, 0x15C);
CHECK_FIELD(sGuildMatchInfo, m_guildMark, 0x32);

// One team's sums of the finished team game (CSharedDoc::m_teamResult,
// FrResultDlg::FindWinner clears and fills them). The original name is not
// known.
struct sTeamResult {
	unsigned long m_score;          // 0x0, the sum of the members' m_rank (shown when m_roomType is 5)
	PADDING(0x4, 0x8);
	__int64 m_pang;                 // 0x8, the sum of the members' m_pang
};
CHECK_SIZE(sTeamResult, 0x10);
CHECK_FIELD(sTeamResult, m_score, 0x0);
CHECK_FIELD(sTeamResult, m_pang, 0x8);
#pragma pack(pop)

// One player's line of a "tiki" (caddie) report the server sends
// (CSharedDoc::m_tikiReportList, 0x7F bytes); BuildTikiReportList turns each
// into an sRivalData of the result list. The original name and header are not
// known.
#pragma pack(push, 1)
struct sCaddieReportData {
	unsigned long m_uid;            // 0x0, the player's UID (sRivalData::m_id and m_uid)
	__int64 m_pang;                 // 0x4, sRivalData::m_pang
	__int64 m_extraPang;            // 0xC, sRivalData::m_extraPang
	unsigned long m_matchTypeId;    // 0x14, CSharedDoc::m_matchTypeId for the local player
	unsigned long m_exp;            // 0x18, the local player's statistics exp
	unsigned long m_mascotTypeId;   // 0x1C, sRivalData::m_mascotTypeId
	unsigned char m_isGM;           // 0x20, non-zero sets identity flag 0x80
	signed char m_boostKind;        // 0x21, 1/2/3: item boost, extra, both
	unsigned short m_level;         // 0x22, sRivalData::m_level
	PADDING(0x24, 0x26);
	signed char m_score;            // 0x26, sRivalData::m_score
	unsigned char m_awardFlags;     // 0x27, bits 0-5: award ceremony slots 0-5
	unsigned char m_value28;        // 0x28, sRivalData::m_holeShots[0], CSharedDoc::m_value456C
	PADDING(0x29, 0x3F);
	char m_nickname[0x16];          // 0x3F, sRivalData::m_nickname
	PADDING(0x55, 0x59);
	int m_guildUID;                 // 0x59, sRivalData::m_hasEmblem
	char m_emblemName[0xC];         // 0x5D, sRivalData::m_emblemName
	unsigned char m_roomType;       // 0x69, CSharedDoc::m_roomType for the local player
	PADDING(0x6A, 0x6D);
	unsigned char m_flags;          // 0x6D, the team; bit 2: skipped (sRivalData::m_state 3)
	unsigned char m_winningTeam;    // 0x6E, CSharedDoc::m_winningTeam for the local player
	PADDING(0x6F, 0x7F);
};
#pragma pack(pop)
CHECK_SIZE(sCaddieReportData, 0x7F);
CHECK_FIELD(sCaddieReportData, m_pang, 0x4);
CHECK_FIELD(sCaddieReportData, m_extraPang, 0xC);
CHECK_FIELD(sCaddieReportData, m_matchTypeId, 0x14);
CHECK_FIELD(sCaddieReportData, m_exp, 0x18);
CHECK_FIELD(sCaddieReportData, m_mascotTypeId, 0x1C);
CHECK_FIELD(sCaddieReportData, m_isGM, 0x20);
CHECK_FIELD(sCaddieReportData, m_boostKind, 0x21);
CHECK_FIELD(sCaddieReportData, m_level, 0x22);
CHECK_FIELD(sCaddieReportData, m_score, 0x26);
CHECK_FIELD(sCaddieReportData, m_awardFlags, 0x27);
CHECK_FIELD(sCaddieReportData, m_value28, 0x28);
CHECK_FIELD(sCaddieReportData, m_nickname, 0x3F);
CHECK_FIELD(sCaddieReportData, m_guildUID, 0x59);
CHECK_FIELD(sCaddieReportData, m_emblemName, 0x5D);
CHECK_FIELD(sCaddieReportData, m_roomType, 0x69);
CHECK_FIELD(sCaddieReportData, m_flags, 0x6D);
CHECK_FIELD(sCaddieReportData, m_winningTeam, 0x6E);

// An item of the offline shop's sale list (CSharedDoc::m_maketList), 0x5E
// bytes (list<sSaleItem>::_Buynode copies 0x17 dwords and a word). The
// original header is not known; FrListBox items point at it
// (COfflineTradeDlg::SetListItem, OnMaketListOwnerDraw).
#pragma pack(push, 1)
struct sSaleItem {
	unsigned long m_ownerUID;       // 0x0, the shop owner's UID (OnMaketListBtnDown)
	char m_ownerNick[22];           // 0x4
	unsigned long m_typeId;         // 0x1A, FindCommonItem
	__int64 m_price;                // 0x1E
	unsigned short m_count;         // 0x26
	unsigned char m_public;         // 0x28, 1 "public", else "private" (OnMaketListOwnerDraw)
	PADDING(0x29, 0x5E);
};
#pragma pack(pop)
CHECK_SIZE(sSaleItem, 0x5E);
CHECK_FIELD(sSaleItem, m_ownerUID, 0x0);
CHECK_FIELD(sSaleItem, m_ownerNick, 0x4);
CHECK_FIELD(sSaleItem, m_typeId, 0x1A);
CHECK_FIELD(sSaleItem, m_price, 0x1E);
CHECK_FIELD(sSaleItem, m_count, 0x26);
CHECK_FIELD(sSaleItem, m_public, 0x28);

// An owned card stack (CSharedDoc::m_myCardList), 0x3A bytes
// (list<sCards>::_Node copies 0xE dwords and a word). The name is the PDB's
// (CCardSlot::GetCards, sWarpItem::sWarpItem(const sCards&)); the original
// header is not known (no inline of it carries line info).
#pragma pack(push, 1)
struct sCards {
	unsigned long m_guid;           // 0x0, matched by _FindByGuid / _FindByUid
	unsigned long m_typeId;         // 0x4
	// Not identified; dword-typed (a char block would give CSharedDoc::AddCard's
	// zeroed local a /GS cookie the original lacks).
	unsigned long unknown8[3];      // 0x8
	int m_count;                    // 0x14, the cards in the stack (traded by CTransferableItem; signed compares in CTaskManager and CFrCardBoxDlg)
	unsigned long unknown18[8];     // 0x18
	unsigned short unknown38;       // 0x38
};
#pragma pack(pop)
CHECK_SIZE(sCards, 0x3A);
CHECK_FIELD(sCards, m_guid, 0x0);
CHECK_FIELD(sCards, m_typeId, 0x4);
CHECK_FIELD(sCards, m_count, 0x14);

// A line of the chat view (CSharedDoc::m_chatLineList). The name is the
// PDB's (ChatMsg emits its implicit constructors); the original header is
// not known.
struct sChatLine {
	std::string m_text;             // 0x0
	unsigned long m_color;          // 0x1C
};
CHECK_SIZE(sChatLine, 0x20);
CHECK_FIELD(sChatLine, m_text, 0x0);
CHECK_FIELD(sChatLine, m_color, 0x1C);

typedef sRivalData sGameResultInfo;
CHECK_SIZE(sGameResultInfo, 0x190);
CHECK_FIELD(sGameResultInfo, m_team, 0x33);
CHECK_FIELD(sGameResultInfo, m_id, 0x0);
CHECK_FIELD(sGameResultInfo, m_uid, 0x4);
CHECK_FIELD(sGameResultInfo, m_nickname, 0x8);
CHECK_FIELD(sGameResultInfo, m_state, 0x36);
CHECK_FIELD(sGameResultInfo, m_pang, 0x128);
CHECK_FIELD(sGameResultInfo, m_score, 0x138);
CHECK_FIELD(sGameResultInfo, m_hole, 0x13D);
CHECK_FIELD(sGameResultInfo, m_rank, 0x13E);
CHECK_FIELD(sGameResultInfo, m_value13F, 0x13F);
CHECK_FIELD(sGameResultInfo, m_sortGroup, 0x140);
CHECK_FIELD(sGameResultInfo, m_ballPos, 0x144);
CHECK_FIELD(sGameResultInfo, m_hasEmblem, 0x160);
CHECK_FIELD(sGameResultInfo, m_emblemName, 0x164);
CHECK_FIELD(sGameResultInfo, m_uniteGuildIndex, 0x170);
CHECK_FIELD(sGameResultInfo, m_approachDistanceCopy, 0x174);
CHECK_FIELD(sGameResultInfo, m_holeScore, 0x38);
CHECK_FIELD(sGameResultInfo, m_holePang, 0x98);
CHECK_FIELD(sGameResultInfo, m_totalShots, 0x13C);
CHECK_FIELD(sGameResultInfo, m_approachDistance, 0x178);
CHECK_FIELD(sGameResultInfo, m_approachPoint, 0x17C);
CHECK_SIZE(sRecordedItemInfo, 0xDD);
CHECK_FIELD(sRecordedItemInfo, m_fileName, 0xBC);
CHECK_FIELD(sRecordedItemInfo, m_playable, 0xDC);
CHECK_SIZE(sUserInfo, 0xB92);
CHECK_FIELD(sUserInfo, m_slot, 0x0);
CHECK_FIELD(sUserInfo, m_skinTypeIds, 0x298);
CHECK_FIELD(sUserInfo, m_skinTypeId, 0x2AC);
CHECK_FIELD(sUserInfo, m_nickname, 0x2);
CHECK_FIELD(sUserInfo, m_pangyaInfo, 0x2);
CHECK_FIELD(sUserInfo, m_name, 0x18);
CHECK_FIELD(sUserInfo, m_guildName, 0x2E);
CHECK_FIELD(sUserInfo, m_guildMark, 0x43);
CHECK_FIELD(sUserInfo, m_identity, 0x53);
CHECK_FIELD(sUserInfo, m_guid, 0x5B);
CHECK_FIELD(sUserInfo, m_uid, 0x10B);
CHECK_FIELD(sUserInfo, m_trophy, 0x1FA);
CHECK_FIELD(sUserInfo, m_value155, 0x155);
CHECK_FIELD(sUserInfo, m_level, 0x159);
CHECK_FIELD(sUserInfo, m_pang, 0x15A);
CHECK_FIELD(sUserInfo, m_ladderPoint, 0x1AC);
CHECK_FIELD(sUserInfo, m_statCombos, 0x1C0);
CHECK_FIELD(sUserInfo, m_equipHead, 0x248);
CHECK_FIELD(sUserInfo, m_equip, 0x248);
CHECK_FIELD(sUserInfo, m_characterId, 0x24C);
CHECK_FIELD(sUserInfo, m_clubId, 0x250);
CHECK_FIELD(sUserInfo, m_ballId, 0x254);
CHECK_FIELD(sUserInfo, m_equipItem, 0x258);
CHECK_FIELD(sUserInfo, m_mapStatistics, 0x2B4);
CHECK_FIELD(sUserInfo, m_mapStatistics2, 0x610);
CHECK_FIELD(sUserInfo, m_characterInfo, 0x96C);
CHECK_FIELD(sUserInfo, m_eventFlags, 0x73);
CHECK_FIELD(sUserInfo, m_caddieInfo, 0xB28);
CHECK_FIELD(sUserInfo, m_clubSet, 0xB41);
CHECK_FIELD(sUserInfo, m_mascot, 0xB53);
CHECK_SIZE(sUserEventFlags, 0x1);
CHECK_SIZE(sClubSetRecord, 0x12);
CHECK_FIELD(sClubSetRecord, id, 0x0);
CHECK_FIELD(sClubSetRecord, typeId, 0x4);
CHECK_FIELD(sClubSetRecord, statLevel, 0x8);
CHECK_FIELD(sUserInfo, m_statShots, 0x10F);
CHECK_FIELD(sUserInfo, m_statistics, 0x10F);
CHECK_FIELD(sUserInfo, m_guildUID, 0x6B);

// The login reply's player information (globalgamedefine.h:1699 in the
// original; it lives here, after sUserInfo, until that moves): an sUserInfo
// and a 0x400-byte block the server sends when local content 0x70 is on
// (CTask::OnPacketCommon 0x42 copies it to CSharedDoc::m_userInfoExtBlock).
// The constructor is emitted by taskmanager.cpp.
#pragma pack(push, 1)
struct sUserInfoExt : sUserInfo {
	// globalgamedefine.h:1699
	sUserInfoExt(void) {
		memset(&m_characterInfo, 0, sizeof(m_characterInfo));
		memset(&m_caddieInfo, 0, sizeof(m_caddieInfo));
		memset(&m_clubSet, 0, sizeof(m_clubSet));
		memset(&m_mascot, 0, sizeof(m_mascot));
		memset(m_nickname, 0, 0x10D);
		memset(&m_statShots, 0, 0xEB);
		memset(&m_trophy, 0, sizeof(m_trophy));
		memset(&m_equipHead, 0, 0x6C);
		memset(m_mapStatistics, 0, sizeof(m_mapStatistics));
		memset(m_extBlock, 0, sizeof(m_extBlock));
	}

	unsigned char m_extBlock[0x400]; // 0xB92
};
#pragma pack(pop)
CHECK_SIZE(sUserInfoExt, 0xF92);
CHECK_FIELD(sUserInfoExt, m_extBlock, 0xB92);
CHECK_SIZE(sUserInfoTime, 0x13FE);
CHECK_FIELD(sUserInfoTime, m_time, 0x0);
CHECK_FIELD(sUserInfoTime, m_userInfo, 0x4);
CHECK_FIELD(sUserInfoTime, m_specialTrophyCount, 0xB96);
CHECK_FIELD(sUserInfoTime, m_pArray1, 0xB9A);
CHECK_FIELD(sUserInfoTime, m_guildTrophyCount, 0xB9E);
CHECK_FIELD(sUserInfoTime, m_pArray2, 0xBA2);
CHECK_FIELD(sUserInfoTime, m_guildPang, 0xBA6);
CHECK_FIELD(sUserInfoTime, m_seasonSpecialTrophyCount, 0x139F);
CHECK_FIELD(sUserInfoTime, m_pArray3, 0x13A3);
CHECK_FIELD(sUserInfoTime, m_seasonGuildTrophyCount, 0x13A7);
CHECK_FIELD(sUserInfoTime, m_pArray4, 0x13AB);
CHECK_FIELD(sUserInfoTime, m_seasonStatistics, 0xBAE);
CHECK_FIELD(sUserInfoTime, m_mapStatistics, 0xC99);
CHECK_FIELD(sUserInfoTime, m_classicMapStatistics, 0xFF5);
CHECK_FIELD(sUserInfoTime, m_trophyStatistics, 0x1351);
CHECK_FIELD(sUserInfoTime, m_friendExtra, 0x13AF);
CHECK_FIELD(sUserInfoTime, m_guildMyPercent, 0xBAA);
CHECK_FIELD(sUserInfoTime, m_userInfo.m_characterInfo, 0x970);
CHECK_FIELD(sUserInfoTime, m_userInfo.m_characterInfo.auxParts, 0x9E0);

// One entry of CSharedDoc::m_gameTypeNames (8 bytes: the stride of
// FrDtRoomDlg::OnGameDescInit's index).
struct sGameTypeInfo {
	const char *name;            // 0x0, the room name SetGameType copies into m_roomName
	unsigned char maxPlayers;    // 0x4, 4 for stroke/match, 2 for the ladder, 30 for lobbies and tourneys (InitGameType)
	unsigned char defaultPlayers; // 0x5, 2 for stroke/match, 10 for lobbies and tourneys (InitGameType)
	unsigned char defaultHoles;  // 0x6, 3 for stroke, 9 for the ladder and tourneys, 18 for the last two types (InitGameType)
	UNKNOWN(0x7, 0x8);
};
CHECK_FIELD(sGameTypeInfo, name, 0x0);
CHECK_FIELD(sGameTypeInfo, maxPlayers, 0x4);
CHECK_FIELD(sGameTypeInfo, defaultPlayers, 0x5);
CHECK_FIELD(sGameTypeInfo, defaultHoles, 0x6);
CHECK_SIZE(sGameTypeInfo, 0x8);

// A map event (the element of CSharedDoc::m_mapEventList; its list node is
// 0x30 bytes, hence sizeof 0x28). The members are not known yet.
struct sMapEventInfo {
	UNKNOWN(0x0, 0x28);
};
CHECK_SIZE(sMapEventInfo, 0x28);

struct sCardStack;
struct sRoomUserInfo;    // detailedroominfodlg.h
struct sItemAttachCard;  // carddlg.h

// The element types below are known only by their size (the constructor's
// list and map nodes); their members are not transcribed yet.
#pragma pack(push, 1)
// A guild match flag (CSharedDoc::m_guildMatchFlags; list node 0x38 bytes).
struct sGuildMatchFlagInfo {
	UNKNOWN(0x0, 0x30);
};
// A guild match pairing the server sends (CLobbyTask::OnPacket 0xBD,
// CSharedDoc::m_guildMatchups).
struct sGuildMatchup {
	bool m_kind;               // 0x0, copied to CSharedDoc::m_flag5020
	unsigned long m_guidA;     // 0x1, a player of the match
	unsigned long m_guidB;     // 0x5, his opponent
};
// A guild member (CSharedDoc::m_guildMembers, keyed by int; tree node 0x54 bytes).
struct GuildMemberInfo_t {
	UNKNOWN(0x0, 0x40);
};
// A quest of the player (CSharedDoc::m_questList; list node 0x18 bytes),
// not IFF_STRUCT::sQuest.
struct sQuest {
	unsigned long m_typeId;      // 0x0, the quest's IFF type id (CLobbyTask::OnPacket 0xCD)
	int m_state;                 // 0x4, a byte the server sends with it
	IFF_STRUCT::sQuest *m_iff;   // 0x8, CItemManager::FindQuest(m_typeId)
	bool m_flagC;                // 0xC, false when received
	UNKNOWN(0xD, 0x10);
};
// A burning special card (CSharedDoc::m_burningSpCards; list node 0x1C bytes).
struct sBurnningSpCard {
	UNKNOWN(0x0, 0x14);
};
// Stand-in for CGolfRule::sShotData (golfrule.h, 0x88 bytes: list node 0x90),
// the element of CSharedDoc::m_shotDataList: CGolfRule cannot be included here.
struct sShotDataStorage {
	UNKNOWN(0x0, 0x88);
};
// The game settings of the room FrDtRoomDlg describes (OnGameDescInit),
// decoded as one 12-byte block (CSharedDoc::m_gameDesc).
struct sRoomGameDesc {
	unsigned char m_descHoleCount;  // 0x0, number of holes
	unsigned long m_descGameTime;   // 0x1, time limit in milliseconds
	unsigned char m_descCourse;     // 0x5, course index, 0xFF: random
	unsigned char m_descGameType;   // 0x6, game type (index of CSharedDoc::m_gameTypeNames)
	unsigned char m_descMode;       // 0x7, hole order mode
	unsigned long m_descMatchId;    // 0x8, the match (CItemManager::FindMatch)
};
#pragma pack(pop)
CHECK_SIZE(sGuildMatchFlagInfo, 0x30);
CHECK_SIZE(sGuildMatchup, 0x9);
CHECK_FIELD(sGuildMatchup, m_guidA, 0x1);
CHECK_FIELD(sGuildMatchup, m_guidB, 0x5);
CHECK_SIZE(GuildMemberInfo_t, 0x40);
CHECK_SIZE(sBurnningSpCard, 0x14);
CHECK_SIZE(sQuest, 0x10);
CHECK_FIELD(sQuest, m_state, 0x4);
CHECK_FIELD(sQuest, m_iff, 0x8);
CHECK_FIELD(sQuest, m_flagC, 0xC);
CHECK_SIZE(sShotDataStorage, 0x88);
CHECK_SIZE(sRoomGameDesc, 0xC);
CHECK_FIELD(sRoomGameDesc, m_descHoleCount, 0x0);
CHECK_FIELD(sRoomGameDesc, m_descGameTime, 0x1);
CHECK_FIELD(sRoomGameDesc, m_descCourse, 0x5);
CHECK_FIELD(sRoomGameDesc, m_descGameType, 0x6);
CHECK_FIELD(sRoomGameDesc, m_descMode, 0x7);
CHECK_FIELD(sRoomGameDesc, m_descMatchId, 0x8);

// The client's shared document (the WSingleton<CSharedDoc> instance). Only
// the members the transcribed functions touch are named. The byte-packed
// sPangYaUserInfo at 0x3E2 puts the following members at odd offsets, hence
// the packing. Size 0x5D30 (CProjectG's constructor allocates it).
#pragma pack(push, 1)
class CSharedDoc : public WSingleton<CSharedDoc> {
	LAYOUT_CHECK_FRIEND;
	friend class CProjectG; // WinProc reconnects through the server list
	friend class CGolfDoc;  // its constructor registers itself as m_pGolfDoc
	friend class CSky;      // CSky::RenderBillboardCloud reads m_pGolfDoc
	friend class FrServerDlg; // ConnectToServer stores the chosen server id
	friend class CLobbyTask;  // OnPacket stores the login result in the protected user info
	friend class CTask;       // OnPacketCommon reads and fills the protected login state
	friend class ntAvatarChatTask; // OnPacket 0x3E reads the user info, m_pGolfDoc and the ignore list
	friend class CGolfRule;   // SendHoleStat reads the user info and fills the game statistics
	friend class CRealMyRoomMain; // InitMyRoomVariables reads m_value5050 and m_value5055
	friend class FrRankForm3;     // OnRankInit lists the guild match pairings
	friend class FrResultDlg;     // its handlers read the match result state
	friend class FrResultTeamDlg; // OnMatchInit reads m_matchTypeId and m_matchGameCount
	friend class FrNewRecordDlg;  // OnCourseListOwnerDraw reads the local player's map statistics
	friend class CHeadIcon;       // DisplayOverPlayer reads m_flag5020
	friend class FrDtRoomDlg;     // OnUserListInit lists m_roomUserList
	friend class CIntrusion;      // OnPacket's room update reads and sets m_matchTypeId
	friend class FrGiftAlarmDlg;  // OnGiftListInit reads m_giftList2 and m_mailList
	friend class FrGiftDlg;       // GiftProgress filters through m_chatManager and keeps the last receiver
	friend class FrScoreDlg;      // SetGuildPlayer lists the guild match pairings
	friend class FrUniteResultDlg; // SetDataToRankListBox tests the protected user info's identity
	friend class CHoleInfo;       // Display prints the local player's best score (m_myMapStatistics/2)
	friend class FrGhostGameMapDlg; // Init finds the map courses through m_itemManager
protected:
	void ClearGameMode(void);
	void ClearStatistics(void);
	void ClearCommonVars(void);
	void LoadLevelTable(void);
	void InitGameType(void);
	void ClearLobbyStateVars(void);
	void ClearServerVars(void);

public:
	// The application gift download state (m_value5D08).
	enum eAppGiftDownloadState {
		APP_GIFT_DOWNLOAD_NONE = 0,  // cleared by ClearCommonVars
		APP_GIFT_DOWNLOAD_READY = 2  // CLobbyMain::ProcessLobbyUI opens the treasure approach gift (CTaskManager packet 0x12B)
	};

	CSharedDoc(void);
	virtual ~CSharedDoc(void);

	// shareddoc.h:1169 (GolfTask emits the out-of-line copy)
	void SetAppGiftDownloadState(eAppGiftDownloadState state) {
		m_value5D08 = state;
	}

	int GetBuffValue(int target, int type);
	bool AddWhisperPartner(FrComboBox* combo, const char* name);
	void ClearCurrentChannel(bool notify);
	void ClearRealEquip(void);
	unsigned long *GetMyAuxPartTidList(void);
	CPartTidList &GetMyPartTidList(void);
	void ClearMyGuildInfo(void);
	void ClearUserInfoTimeMap(unsigned long uid);
	void LoadSchoolName(char* const szName, unsigned long schoolNo);
	void ClearAll(void);
	void ClearInventory(void);
	void ClearGameVars(void);
	void ClearRivalVars(bool);
	void InitTradeData(void);
	void ClearOldSeasonStat(void);
	void ClearJapanVars(void);
	void ClearUcc(void);
	void LogGstUpdateHole(unsigned long uid, unsigned char hole, int score, __int64 pang);
	int IsControlServerService(int service);
	void SetTradeUID(unsigned long uid);
	unsigned long GetTradeUID(void);               // shareddoc.h:568, inline below (Avatar_MainUI's copy)
	unsigned short GetSecurityKey(void);
	void LoadItemDb(void);
	void AddChar(sCharacterInfo *pChar);
	void AddCaddie(sCaddieInfo *pCaddie);
	void AddMascot(sMascotInfo *pMascot);
	void AddMyPartsList(const sItemInfo &item);
	void AddMyItem(const sItemInfo &item);
	bool InitRecentWhisperList(FrComboBox *pComboBox);
	bool InitReservedChatList(FrComboBox *pComboBox, eReservedChatPartner partner);
	bool UpdateChatTarget(FrComboBox *pComboBox);
	const char *GetPrevChatTarget(const char *target);
	const char *GetNextChatTarget(const char *target);
	std::string GetLoginAuthKey(void);
	void SetLoginAuthKey(std::string key);
	void SetSecurityKey(unsigned short key);
	void SetFileName(char *const fileName);
	void InitMapEvent(void);
	bool IsMapEventActive(int map);
	unsigned long GetMapEventPangRate(int map);
	unsigned long GetMapEventExpRate(int map);
	bool CanUseRookieChannelMap(unsigned int map);
	bool CanCompound(unsigned long typeId);
	void SetIdentity(int identity);
	void AccumulateIdentity(int identity);
	_SYSTEMTIME &GetServerTime(void);
	void SetServerTime(_SYSTEMTIME &time);
	void SetCurEquipInfo(sUserEquip *equip);
	void BuildMyPartTidList(void);
	bool BuildMyPartTidListDefault(unsigned long typeId);          // 0x407350
	void BuildMyPartTidList(sCharacterInfo &characterInfo);       // 0x410360
	void SetInitHaveHalloweenItem(void);
	void SetDirectMoveRoomInfo(unsigned char server, unsigned short room, unsigned long uid);
	bool GetDirectMoveRoomInfo(unsigned char *server, unsigned short *room, unsigned long *uid);
	void ClearDirectMoveRoomInfo(void);
	CPeriodContents *GetPeriodDoc(void);
	void SetPeriodDoc(void);
	void LoadChatFilter(void);
	void InitGolfDoc(void);                         // 0x413ed0
	void LoadVisGroup(const char *fileName);        // 0x420a60
	sItemInfo *FindMyItemByGuid(unsigned long guid);
	bool IsOverlapPart(unsigned long typeId);
	sCardStack *GetCardInfoToUID(unsigned long uid);
	sCardStack *GetCardInfoToTID(unsigned long typeId);
	void ReplaceCardCountToTID(unsigned long typeId, int count);  // 0x414620
	void ReplaceCardCountToUID(unsigned long uid, int count);     // 0x414660
	void DeleteCardStack(unsigned long uid, int count);           // 0x4146a0
	// shareddoc.h:467, in the class body in the original. sCardStack is
	// incomplete here (cardsystem.h), so taskmanager.cpp, which emitted the
	// copy, defines it out of line: m_cardList.clear().
	void ClearCardStack(void);
	sMascotInfo *GetMyMascotInfo(unsigned long id);
	sMascotInfo *GetMyEquippedMascotInfo(void);
	int GetNumAddItemSlotByMascot(unsigned char player);
	int GetNumAddItemSlotByMascot(void);
	bool DecreaseMyItem(std::list<sItemInfo>::iterator &it);
	bool IsEquipParts(sCharacterInfo &characterInfo, unsigned long typeId, unsigned long partsId);
	int IsEquipParts(unsigned long serial, unsigned long typeId); // 0x41a870
	__int64 RemainOwnCash(unsigned long buyType, __int64 price);
	void SetIndex(unsigned long uid);
	bool IsCurrentMissionBlind(void);
	int FindUserInfoTimeUID(unsigned long uid, std::map<unsigned long, sUserInfoTime>::iterator &it);
	int FindUserInfoTimeGUID(unsigned long guid, std::map<unsigned long, sUserInfoTime>::iterator &it);
	int FindUserInfoTimeUserID(const char *userId, std::map<unsigned long, sUserInfoTime>::iterator &it); // 0x414bf0
	void CheckAngelWing(void);                       // 0x4147a0
	bool SendOfflineTradeEditShop(void);             // 0x410560
	unsigned long GetMyDisconPangPenalty(void);      // 0x410ab0
	void Log30sScore(void);                          // 0x41a0a0
	int FindUserInfo(unsigned long key, std::map<unsigned long, sBriefUserInfo>::iterator &it);
	int FindUserInfoUID(unsigned long uid, std::map<unsigned long, sBriefUserInfo>::iterator &it);
	int InsertUserInfoTime(unsigned long uid, sUserInfoTime &info);

	// shareddoc.inl:389 (inline; SharedDoc emitted the copy the linker kept,
	// not transcribed yet).
	unsigned char GetIndex(unsigned long uid);
	bool AreWePlayingTogether(unsigned long uid);
	const char *GetDefaultTitle(unsigned char gameType);
	std::map<unsigned char, sMapStatistics> &GetMyPastMapStat(unsigned char course);

	// shareddoc.h:502 (GolfItemHandler's copy)
	const std::list<sItemInfo> &GetConstMyItemList(void) { return m_itemInfoList; }

	// shareddoc.h:505 (inline; GuildActor emitted the copy the linker kept)
	const sPangYaUserInfo &GetPangyaUserInfo(void) {
		return m_userInfo;
	}

	// Defined in the class body in the original (shareddoc.h line marks);
	// this unit emits them, so they are defined out of line in shareddoc.cpp.
	void HaveAngelWing(bool have);
	eStateTrade GetStateTrade(void);                // shareddoc.h:559, inline below
	std::list<sTradeItem> *GetListTradeItem(void);  // shareddoc.h:564, inline below
	std::list<sStall> *GetListStallItem(void);
	sStall *GetStallItem(void);
	// shareddoc.h:579 (TaskManager's copy)
	void SetStallItem(sStall stall) {
		m_stallItem = stall;
	}
	void SetTradeMode(unsigned char mode);
	unsigned char GetTradeMode(void);               // shareddoc.h:609, inline below (OfflineTradeDlg's copy)
	std::list<sSaleItem> *GetMaketList(void);       // shareddoc.h:583, inline below (OfflineTradeDlg's copy)
	void SetHaveHalloweenItem(bool have);
	void SetWearHalloweenItem(bool wear);
	void SetUserInfoMod(int mod);                   // shareddoc.h:1144, inline below
	void SetCountVisitor(int count);                // shareddoc.h:571, inline below (TaskManager's copy)
	void SetTradeIncome(__int64 income);            // shareddoc.h:588, inline below (TaskManager's copy)
	int GetUserInfoMod(void);

	// Bongdari shop play counts (shareddoc.h:550-554, inline below).
	short GetBsUsableTimes(void);
	short GetBsUsableBonusTimes(void);
	short GetBsRemainedBonusTimes(void);

	// Emitted by taskmain.cpp, defined out of line there.
	void SetRmrMode(unsigned char mode);            // shareddoc.h:616
	unsigned char GetRmrMode(void);                 // shareddoc.h:617
	unsigned long GetFlagBlock(void);               // shareddoc.h:620, inline below (TaskMain's copy)
	bool IsHaveHalloweenItem(void);                 // shareddoc.h:1011
	eReplayModeType GetPlayingMode(void);           // shareddoc.h:1024, inline below (TaskMain's copy)
	std::list<std::string> *GetVisGroup(const char *name); // 0x41a6c0, out of line in shareddoc.cpp (CGolfPlayer::ParseScript_ShowVis)
	void SetPlayingMode(eReplayModeType mode);      // shareddoc.h:1025
	int GetNowGoWithMode(void);                     // shareddoc.h:1137
	int GetGoWithFriendUID(void);                   // shareddoc.h:1138
	void SetGoWithModeStart(int friendUID);          // shareddoc.h:1132, inline below (MessengerDlg's copy)
	void SetGoWithModeEnd(void);                    // shareddoc.h:1136, inline below (MessengerDlg's copy)
	void SetGoWithMode(int mode);                   // shareddoc.h:1140, inline below (LobbyMain's copy)

	// Emitted by taskmanager.cpp, defined out of line there.
	void SetBsUsableTimes(short times);             // shareddoc.h:551
	void SetBsUsableBonusTimes(short times);        // shareddoc.h:553
	void SetBsRemainedBonusTimes(short times);      // shareddoc.h:555
	void SetFlagBlock(unsigned long flagBlock);     // shareddoc.h:622

	// Emitted by lobbytask.cpp, defined out of line there.
	void SetTimeBlock(int timeBlock);               // shareddoc.h:623

	// shareddoc.h:621 (ServerDlg emitted the copy the linker kept)
	int GetTimeBlock(void) {
		return m_userInfo.timeBlock;
	}

	// shareddoc.h:565 (TaskManager's copy)
	int GetSizeTradeItem(void) { return m_listTradeItem.size(); }

	// shareddoc.h:563 (TaskManager's copy)
	void ClearListTradeItem(void) { m_listTradeItem.clear(); }
	// shareddoc.h:569 (TaskManager's copy)
	void SetTradeTitle(std::string title) { m_tradeTitle = title; }
	// shareddoc.h:584 (TaskManager's copy)
	void ClearMaketList(void) { m_maketList.clear(); }
	// shareddoc.h:585 (TaskManager's copy)
	void AddMaketListItem(sSaleItem item) { m_maketList.push_back(item); }

	unsigned long GetFriendGameSvrUID(void);        // shareddoc.h:1058
	void SetForceNicknameChange(unsigned char force); // shareddoc.h:1074
	// shareddoc.h:1075 (LobbyMain's copy)
	unsigned char GetForceNicknameChange(void) const { return m_forceNicknameChange; }

	// shareddoc.h:1173 (LobbyMain's copy)
	eAppGiftDownloadState GetAppGiftDownLoadState(void) const
	{
		return (eAppGiftDownloadState)m_value5D08;
	}

	// shareddoc.h:1205 (LobbyMain's copy)
	bool IsOpenFirstLoginAlarm(void) const { return m_loginDlgCreated; }
	// shareddoc.h:1206 (LobbyMain's copy)
	void SetNeedFirstLoginAlarm(bool need) { m_loginDlgCreated = need; }

	// Emitted by messengerdlg.cpp (inline below, MESSENGERDLG_SHAREDDOC_INLINE).
	void SetFriendGameSvrUID(unsigned long uid);    // shareddoc.h:1057
	void SetInviteMode(int mode);                   // shareddoc.h:1060
	int IsInviteMode(void);                         // shareddoc.h:1061

	// Class-body inlines in the original. GolfRule emitted the copies the
	// linker kept: GolfRule.cpp defines GOLFRULE_SHAREDDOC_INLINE before its
	// first include and defines them out of line.
#ifndef GOLFRULE_SHAREDDOC_INLINE
	// shareddoc.h:1018
	void SetReplayState(int state) { m_replayState = state; }
	// shareddoc.h:1019
	int GetReplayState(void) { return m_replayState; }
#else
	void SetReplayState(int state);                 // shareddoc.h:1018
	int GetReplayState(void);                       // shareddoc.h:1019
#endif

	// Emitted by golfrule.cpp, defined out of line there.
	char *GetFileName(void);                        // shareddoc.h:1021
	int GetReplayPlayType(void);                    // shareddoc.h:1022
	void SetReplayPlayType(int type);               // shareddoc.h:1023
	void SetGoStopMode(bool goStop);                // shareddoc.h:1150
	bool GetGoStopMode(void);                       // shareddoc.h:1151

	// Emitted by fontinfo.cpp, defined out of line there.
	unsigned long GetFlagAbilityItem(void);         // shareddoc.h:642
	unsigned char GetNumBonusPangItem(void);        // shareddoc.h:644
	bool IsWearHalloweenItem(void);                 // shareddoc.h:1012

	// Emitted by tradedlg.cpp, defined out of line there.
	const char *GetTradeTitle(void); // shareddoc.h:570
	int GetCountVisitor(void);      // shareddoc.h:572
	__int64 GetTradeIncome(void);   // shareddoc.h:590

	// Emitted by tradehandler.cpp, defined out of line there.
	void SetStateTrade(eStateTrade state); // shareddoc.h:560
	void AddTradeIncome(__int64 income);   // shareddoc.h:589

	bool SendTradeOpenShop(void);
	bool SendTradeTitle(std::string title);          // 0x40c990
	bool SendTradeCloseShop(void);
	bool SendTradeEditShop(void);                   // 0x40c820
	bool SendTradeEnterShop(unsigned long uid);     // 0x40c890
	void SendEquipItemSlotByMascot(void);           // 0x419750
	void SendEquipItemSlot(void);                   // 0x419880
	bool SendTradeExitShop(unsigned long uid);      // 0x40c910
	bool SendTradeShowVisitor(void);                // 0x40ca40
	bool SendTradeIncome(void);                     // 0x40cac0
	bool SendTradeBuyItem(unsigned long uid, sTradeItem item); // 0x40cb40
	float GetWtPepPangyaComboGauge(unsigned char team); // 0x410210
	int CollapseItemSlot(unsigned char player);     // 0x410460
	void DeleteCardToPartsAttach(unsigned long cardId); // 0x4195a0
	float GetAuxPartProperty(unsigned char team, unsigned char property); // 0x4142d0
	bool ConfirmInsertItem(unsigned long typeId, unsigned long count) const; // 0x4103d0
	bool FindEquipPart(unsigned char team, unsigned long typeId); // 0x414470
	float GetComboGaugeLimit(unsigned char team);   // 0x414540
	bool CheckUnderLevelClubSet(unsigned long typeId); // 0x410b70
	void RefreshAllCookie(void);                    // 0x410be0
	bool DeleteMyItem(std::list<sItemInfo>::iterator &it); // 0x419dc0
	bool DeleteMyItem(unsigned long typeId);        // 0x41c8f0
	sRivalData *GetRival(unsigned long uid);        // 0x41fcd0
	void SetInitAbilityItem(void);                  // 0x410620
	void SetInitWearHalloweenItem(void);            // 0x41a4b0
	void AddCard(unsigned long typeId, unsigned long guid, unsigned short count); // 0x41c680
	void AddCardStack(sCardStack cardStack, bool replace); // 0x41c5f0
	bool MinusListTradeItem(sTradeItem item);       // 0x41d100
	void ChageListStallitem(sStall *pStall);        // 0x4148f0
	void AddFurniture(const sFurniture_List &furniture); // 0x41c720
	bool UTIL_SendChatMessage(FrComboBox *pComboBox, const char *message, bool useSendChatMessage2); // 0x41ce70
	void SortAllMyItemList(void);                   // 0x4200b0
	int GetIndexPangyaLogoImage(void);              // 0x4106a0
	bool SetWinningPrize(unsigned long uid, const sWinningPrize &prize); // 0x414080
	bool SetExtPrize(unsigned long uid, int count, sPrizeInfo *pPrizes); // 0x414130
	bool SendOfflineTradeVisitor(unsigned long uid); // 0x414970
	void CheckCurrEquipParts(void);                 // 0x4199e0
	void SetEquipCharCaddieInfoFromMyInfo(void);    // 0x419e00
	void RefreshItemListFromGuidList(unsigned char type); // 0x41a530
	bool AddListTradeItem(sTradeItem item);         // 0x41cf90
	void UpdateGachaTickets(void);                  // 0x41d1a0
	void ReplaySavFileCheck(void);                  // 0x41d300
	void BuildTikiReportList(void);                 // 0x41f430

	// shareddoc.inl's free functions read these.
	friend void SetHoles(unsigned char holes);
	friend void SetWeather(unsigned char weather);
	friend CItemManager *ItemManager(void);
	friend unsigned long MyUID(void);
	friend __int64 MyBonusCash(void);
	friend int IsGuildGame(void);
	friend char *MyNick(void);
	friend char *MyId(void);
	friend unsigned char GetPlayerNum(void);
	friend sPlayerData *PLAYER(unsigned char index);
	friend unsigned char GetCurMap(void);
	friend bool SetCurMap(unsigned char map);
	friend int OfflinePlay(void);
	friend __int64 MyOutsideCash(void);
	friend void SetMyOutsideCash(__int64 cash);
	friend unsigned char GetHoleType(void);
	friend int GalleryMode(void);
	friend int AdminMode(void);
	friend int GiftForbiddenMode(void);
	friend int ItemDropMode(void);
	friend unsigned long MyGuid(bool galleryGuid);
	friend unsigned char GetHoles(void);
	friend unsigned char GetStartHole(void);
	friend unsigned char GetEndHole(void);
	friend unsigned char GetWeather(void);
	friend unsigned long GetShotTimeLimit(void);
	friend void SetGameOver(bool gameOver);
	friend bool IsGameOver(void);
	friend unsigned long GetGameTimeLimit(void);
	friend unsigned char MyIndex(void);
	friend bool IsMyTurn(bool galleryTurn);
	friend int IsObserver(unsigned long identity);
	friend int IsAdministrator(unsigned long identity);
	friend CGolfDoc *GOLFDOC(void);

	// TurnOn3DBackground / TurnOff3DBackground write the weather and the
	// 3D background switch directly.
	friend class CTaskMain;

	// CGMToolkit::IsAdministrator reads m_userInfo.identity directly.
	friend class CGMToolkit;

	// CClock::HandleMsg tests bit 1 of m_userInfo.identity directly.
	friend class CClock;

	// _guild::CGuildActor::ChangeGuildName copies the new name into
	// m_userInfo.guildName directly.
	friend class _guild::CGuildActor;

	// FrAwardDlg's handlers read the local player's and the room's state
	// directly.
	friend class FrAwardDlg;

	// FrPrivateTrade::OnInit reads the local player's nickname directly.
	friend class FrPrivateTrade;

	// CGolfBg::OnLoad/DoReLoad read the room's course state directly.
	friend class CGolfBg;
	// CLobbyMain::OnToppage_Finish tests and clears m_flag50F6.
	friend class CLobbyMain;
	// COfflineTradeDlg reads the item manager and sets m_rmrMode to visit a shop owner's room.
	friend class COfflineTradeDlg;
	// CScreen's handlers look up the room's match through m_matchTypeId.
	friend class CScreen;
	// CSpot tests the local player's m_userInfo.guildUID.
	friend class CSpot;
	// CShopMain tests bit 1 of m_userInfo.eventFlags.
	friend class CShopMain;
	// CGolfTask's packet handlers read m_matchTypeId and m_userInfoMod.
	friend class CGolfTask;
	// CPangInfo tests bit 0 of m_value4D74.
	friend class CPangInfo;
	// FrLoginDlg::OnProc shows the login block reason (m_string4D14).
	friend class FrLoginDlg;
	// CShopTask::OnPacket stores m_myBonusCash and sends and resets the last gift receiver.
	friend class CShopTask;
	// CMiniScore::Display prints the players' m_gameStatistics distances.
	friend class CMiniScore;
	// CGolfCaddie::TalkProcess reads m_gameStatistics and m_roomPlayers.
	friend class CGolfCaddie;
	// CRealMyRoomTask's packet handlers read the protected room state.
	friend class CRealMyRoomTask;

protected:

public:
	std::string m_loginServerAddress;                // 0x28, the login server's dotted address (GameUnit::Connect hands it to SetMoveLoginServer; MSNUnit::OnConnect)
	std::string m_gameServerAddress;                 // 0x44, the game server's dotted address (GameUnit::Connect, inet_addr)

protected:
	std::string m_string60;                          // 0x60 (only constructed so far)

public:
	unsigned long m_gameServerPort;                  // 0x7C, the game server's port, host order (GameUnit::Connect, htons)

protected:
	unsigned long m_value80;                         // 0x80, cleared by ClearCommonVars

public:
	unsigned long m_gameServerUID;                   // 0x84, the id of the game server connected to (FrServerDlg::ConnectToServer; FrUserListDlg::OnInviteBtnUp), cleared by ClearCommonVars

protected:
	unsigned long m_value88;                         // 0x88, likewise

public:
	unsigned long m_loginConnectValue;               // 0x8C, a dword LoginUnit::OnConnect decodes from the login server's reply
	unsigned char m_value90;                         // 0x90, cleared by ClearGameVars
	PADDING(0x91, 0x94);
	unsigned long m_value94;                         // 0x94, cleared by ClearCommonVars
	std::string m_rankServerAddress;                 // 0x98, the rank server's dotted address (RankUnit::Connect, inet_addr)
	unsigned short m_rankServerPort;                 // 0xB4, the rank server's port, host order (RankUnit::Connect, htons)

protected:
	PADDING(0xB6, 0xC8);
	unsigned long m_loginTime;                       // 0xC8, timeGetTime() when the login server accepted the login (CLobbyTask::OnPacket)

public:
	sRoomGameDesc m_gameDesc;                        // 0xCC, the room FrDtRoomDlg describes (CLobbyTask::OnPacket 0x84)
	// The room name of each game type (SetGameType copies the entry into
	// m_roomName; FrDtRoomDlg::OnGameDescInit shows it).
	sGameTypeInfo m_gameTypeNames[15];               // 0xD8, indexed by game type (m_roomType)

protected:
	PADDING(0x150, 0x158);
	_SYSTEMTIME m_serverTime;                        // 0x158
	std::string m_string168;                         // 0x168 (only constructed so far)
	unsigned long m_japanState;                      // 0x184, cleared by ClearJapanVars
	std::string m_string188;                         // 0x188 (only constructed so far)

public:
	unsigned long m_billingType;                     // 0x1A4, the billing service: 2 gametree, 4 hangame (FrGiftDlg/FrCheckoutDlg::OnChargeResult)

protected:
	std::string m_commandLineAccount;                // 0x1A8, the first command-line argument after the exe path, quotes stripped (ClearJapanVars)
	unsigned char m_japanFlag;                       // 0x1C4, cleared by ClearJapanVars
	PADDING(0x1C5, 0x1C8);
	unsigned long m_japanParam1;                     // 0x1C8, likewise
	unsigned long m_japanParam2;                     // 0x1CC, likewise
	std::list<unsigned long> m_list1D0;              // 0x1D0 (only constructed so far)
	std::list<unsigned short> m_list1DC;             // 0x1DC (only constructed so far)
	CItemManager m_itemManager;                      // 0x1E8, constructed by CSharedDoc's constructor (ItemManager)
public:
	unsigned short m_mySlot;                         // 0x3E0, the local player's slot in the room (sUserInfo::m_slot), 0xFFFF outside one (CTaskMain::SendRoomExitPacket)

	// 0x3E2-0x4EF: the local player's sPangYaUserInfo (0x10D bytes, SetIndex
	// copies it whole); m_loginId and m_myUID name two of its members.
	union {
		struct {
			PADDING(0x3E2, 0x46B);
			char m_loginId[0x16];                    // 0x46B, sPangYaUserInfo::loginId; a web login whose nickname still equals it must pick one (CLobbyMain::NeedsChangeNick)
			PADDING(0x481, 0x4EB);
			unsigned long m_myUID;                   // 0x4EB, sPangYaUserInfo::uid, the local player's UID (MSNUnit::OnConnect)
		};
		sPangYaUserInfo m_userInfo;                  // 0x3E2, the local player's (GetPangyaUserInfo)
	};
	// The local player's statistics (sPangYaUserStatistics at 0x4EF; the flat
	// names after the Titles_Client checks, m_myStatistics the same block).
	union {
		struct {
			unsigned long m_myStatShots;             // 0x4EF, statistics +0x00
			unsigned long m_myStatPuttAttempts;      // 0x4F3, statistics +0x04 (CheckPuttinPercent's attempts)
			PADDING(0x4F7, 0x503);
			unsigned long m_myStatPangyas;           // 0x503, statistics +0x14
			PADDING(0x507, 0x513);
			unsigned long m_myStatDrives;            // 0x513, statistics +0x24 (CheckFairawayPercent's drives)
			unsigned long m_myStatPar3Drives;        // 0x517, statistics +0x28 (CheckFairawayPercent's par 3s)
			unsigned short m_myStatHoleInOnes;       // 0x51B, statistics +0x2C
			PADDING(0x51D, 0x51F);
			unsigned long m_myStatFairways;          // 0x51F, statistics +0x30 (CheckFairawayPercent's fairways)
			PADDING(0x523, 0x529);
			unsigned long m_myStatPuttsSunk;         // 0x529, statistics +0x3A (CheckPuttinPercent's successes)
			PADDING(0x52D, 0x535);
			unsigned long m_myExp;                   // 0x535, the local player's experience (CFamilyTask::OnPacket)
			unsigned char m_myLevel;                 // 0x539, the local player's level
			__int64 m_myPang;                        // 0x53A, the local player's pang (FrPangGiftDlg, "%I64d")
			PADDING(0x542, 0x57C);
			unsigned long m_myStatGamesPlayed;       // 0x57C, statistics +0x8D (CheckNoMannerPercent's games)
			unsigned long m_myStatTeamHoles;         // 0x580, statistics +0x91 (name inferred from the statistics layout); CTask::OnPacketCommon 0xC4 decrements it while > 0
			PADDING(0x584, 0x5A8);
			unsigned long m_myStatNoMannerGames;     // 0x5A8, statistics +0xB9 (CheckNoMannerPercent's count)
			PADDING(0x5AC, 0x5BC);
			unsigned long m_myStatCoins;             // 0x5BC, statistics +0xCD; CHoleInfo::Display draws count % 5 coins and a full row at 5
			PADDING(0x5C0, 0x5C6);
			unsigned long m_myStatHolesPlayed;       // 0x5C6, the local player's holes played (ValentineEventDlg)
			unsigned long m_myStatRecordCount;       // 0x5CA, statistics +0xDB; CLobbyMain::OpenNewRecordForm opens the new record form when it differs from m_value4D88, then copies it there
			PADDING(0x5CE, 0x5DA);
		};
		sPangYaUserStatistics m_myStatistics;        // 0x4EF, the same block as one struct
	};

public:
	sTrophyStatistics m_myTrophy;                    // 0x5DA, the local player's medals (Title25-27, CheckGoldMedal)

public:
	// 0x628-0x694: the local player's equipment, one sUserEquip (SetCurEquipInfo
	// copies it whole; CRealMyRoomMain keeps a copy at 0x974).
	union {
		struct {
			unsigned long m_myCaddieId;              // 0x628, the local player's equipped caddie (sUserInfo::m_equipHead; Net_EquipCaddie, cCaddieTalkTeeShot::FindMsg)
			unsigned long m_myCharacterId;           // 0x62C, the local player's equipped character
			unsigned long m_myClubId;                // 0x630, the local player's equipped club set
			unsigned long m_myBallId;                // 0x634, the local player's equipped ball (sUserInfo::m_ballId; Net_EquipGear)
			unsigned long m_equipItem[10];           // 0x638, type ids of the equipped (consumable) items
			PADDING(0x660, 0x678);
			unsigned long m_equipSkin[6];            // 0x678, type ids of the equipped skins (CTaskMain::HandleMsgCommon)
			unsigned long m_myMascotId;              // 0x690, the local player's equipped mascot (GetMyEquippedMascotInfo)
		};
		sUserEquip m_myEquip;                        // 0x628, the same block as one struct
	};

protected:
	sMapStatistics m_myMapStatistics[20];            // 0x694, by course (the constructor marks them empty)
	sMapStatistics m_myMapStatistics2[20];           // 0x9F0, likewise
	unsigned char m_userInfoExtBlock[0x400];         // 0xD4C, sUserInfoExt::m_extBlock of the login reply (CTask::OnPacketCommon 0x42)
	std::map<unsigned int, std::string> m_stringMap; // 0x114C (only constructed so far)

public:
	std::map<unsigned int, sCharacterInfo> m_characterMap; // 0x1158, the characters the player owns, keyed by id

protected:

public:
	std::map<unsigned int, sCaddieInfo> m_caddieMap; // 0x1164, the caddies the player owns, keyed by id

public:
	std::map<unsigned int, sMascotInfo> m_myMascotMap; // 0x1170, the mascots the player owns, keyed by id

public:
	std::list<sItemInfo> m_myPartsList;             // 0x117C, AddMyPartsList appends to it

protected:

public:
	std::map<unsigned int, sItemInfo> m_myItemMap;   // 0x1188

public:
	std::list<sItemInfo> m_myGearList;               // 0x1194, sorted by SortAllMyItemList; FrPlayerDlg cycles balls/clubs through it (typeId +4)

public:
	std::list<sItemInfo> m_itemInfoList;            // 0x11A0, AddMyItem appends to it (elements read by type id +4 and the short at +0xC)

protected:

public:
	std::list<sRecordedItemInfo> m_replayFileList;   // 0x11AC, the saved replays (FrReplayModeDlg deletes from it)

public:
	// Two more owned-item lists (SortAllMyItemList sorts them, UccLibrary's
	// GetMyItemInfo searches them); their item classes are not identified.
	std::list<sItemInfo> m_myExtraItemList;          // 0x11B8
	std::list<sItemInfo> m_myExtraItemList2;         // 0x11C4
	std::list<sGiftInfo> m_giftList;                 // 0x11D0, the received gifts (CTask::_ReFreshMyRoomGift)

protected:
	std::list<sGiftInfo> m_giftList2;                // 0x11DC (only constructed so far)
	std::list<sMailInfoBrief> m_mailList;            // 0x11E8 (only constructed so far)

public:
	std::list<sItemInfo> m_mySkinList;              // 0x11F4, the owned items of type class 14 (skins; FrRecycleDlg::IsHaveItem reads the type id +4)

protected:
	std::list<sSpecialTrophy> m_mySpecialTrophies;   // 0x1200 (only constructed so far)
	std::list<sGuildTrophy> m_myGuildTrophies;       // 0x120C (only constructed so far)

public:
	// Two more owned-item lists, searched last by UccLibrary's GetMyItemInfo;
	// CTransferableItem trades the first by count (m_count, +0xC). Their item
	// classes are not identified.
	std::list<sItemInfo> m_myExtraItemList3;         // 0x1218 (PrivateTradeDlg finds by guid)
	std::list<sItemInfo> m_myExtraItemList4;         // 0x1224

public:
	std::list<sFurniture_List> m_furnitureList;      // 0x1230, the my-room furniture (AddFurniture appends; element flag byte +0x1A)

public:
	std::list<sCards> m_myCardList;                  // 0x123C, the owned cards (CTransferableItem, CCardBook, PrivateTradeDlg)

public:
	// The period cards in use, by kind (sSCardAvilityPeriodInfo::kind; [0] is
	// what CItemBuff::UpdateBuffDlg shows). CCardManager::CalcCardPeriodAndStatus
	// walks the four lists.
	std::list<sSCardAvilityPeriodInfo> m_cardPeriodList[4]; // 0x1248

protected:
	std::list<sQuest> m_questList;                   // 0x1278 (only constructed so far)
	PADDING(0x1284, 0x1288);

public:
	__int64 m_myCash;                                // 0x1288, the local player's cash (FrOnelineReqChkOutDlg::OnYesInit)

protected:
	__int64 m_outsideCash;                           // 0x1290
	__int64 m_myBonusCash;                           // 0x1298 (MyBonusCash)

public:
	std::list<sItemInfo> m_myItemList;               // 0x12A0
	std::list<sItemInfo> m_myAuxItemList;            // 0x12AC, searched for UCC aztecs

public:
	std::map<unsigned long, sRoomSlot> m_roomSlotMap; // 0x12B8, the room's player slots by UID (CLobbyMain::HidePet deletes a slot's pet exhibition)

public:
	std::map<unsigned long, unsigned char> m_indexMap; // 0x12C4, a player's index by UID (SetIndex, GetIndex)

public:
	sUserInfo m_roomPlayers[4];                      // 0x12D0, indexed by player number

protected:
	sPangYaUserStatistics m_gameStatistics[4];      // 0x4118, by player number; ClearGameVars zeroes them and sets exp to -1

public:
	std::vector<unsigned long> m_gainedItemList[4];  // 0x44C4, type ids of the items won in the game (FrUniteResultDlg lists [0])

public:
	std::vector<sRivalData> m_gameResultList;        // 0x4504, the finished game's results (FrFinalRankDlg)

public:
	std::vector<sCaddieReportData> m_tikiReportList; // 0x4514, the server's report lines (BuildTikiReportList)

protected:
	std::vector<sGuildMatchup> m_vector4524;         // 0x4524, the guild match pairings (CLobbyTask::OnPacket 0xBD)
	std::list<sGuildMatchFlagInfo> m_guildMatchFlags; // 0x4534 (only constructed so far)
	unsigned char m_block4540[0x1C];                 // 0x4540, a block the server sends whole (CLobbyTask::OnPacket 0xBE, then CUserInfo::Refresh)

public:
	bool m_gameOver;                                 // 0x455C

protected:
	PADDING(0x455D, 0x4560);

public:
	int m_gameMode;                                  // 0x4560
	unsigned char m_gameKind;                        // 0x4564, 4: no ghost recording (CGhostRecorder::IsSaveAvailable, FamilyMain, ClientSetting, TrainingDlg)

protected:
	PADDING(0x4565, 0x4568);
	unsigned long m_matchTypeId;                     // 0x4568, the match's type id (CItemManager::FindMatch; FrResultTeamDlg::OnMatchInit)
	unsigned char m_value456C;                       // 0x456C, cleared by ClearGameVars

public:
	unsigned char m_winningTeam;                     // 0x456D, the team game's winner: 0 red, 1 blue, 2 draw (FrResultDlg)

protected:
	// 0x456E, the award ceremony's players: one array of twelve
	// (FrResultDlg::OnPangInit walks 6-11; LobbyTask decodes 0x60 bytes).
	// m_winnerPlayers names its second half for FrAwardDlg until AwardDlg.cpp
	// indexes m_awardPlayers[6 + i].
	union {
		sAwardPlayer m_awardPlayers[12];
		struct {
			sAwardPlayer m_awardPlayersFirstHalf[6];
			union {
				sAwardPlayer m_winnerPlayers[6];     // 0x459E, the winners (FrAwardDlg)
				sMedalWinner m_medalWinners[4];     // 0x459E, the same in 12-byte records (GetMedalIndex reads six)
			};
		};
	};

public:
	unsigned char m_holePin[18];                     // 0x45CE, the pin position (0-2) of each played hole (TrainingDlg draws it with rand() % 3)

protected:
	unsigned char m_holeCourse[18];                  // 0x45E0, course map of each played hole (CGolfBg::OnLoad/DoReLoad)

public:
	unsigned char m_holeOrder[18];                   // 0x45F2, course hole of each played hole

protected:
	PADDING(0x4604, 0x4608);

public:
	unsigned long m_holeRandom[18];                  // 0x4608, the per-hole random table (GolfRuleApproach::GetRandomStartPos)
	unsigned long m_gameStartTime;                   // 0x4650, g_CurrentTime when the game started (CClock::OnProcess)

	float m_pangRate;                                // 0x4654, the game's pang multiplier: bonus = (m_pang + m_extraPang) * m_pangPercent * m_pangRate / 100 (FrResultDlg::OnBonusInit)
	unsigned int m_pangPercent;                      // 0x4658, see m_pangRate

	bool m_timeBooster;                              // 0x465C, the player owns the time booster (0x1A000011) and is no gallery viewer (CGolfRule::OnPreLoadInit)

protected:
	PADDING(0x465D, 0x4660);
	unsigned char m_curMap;                          // 0x4660, +0x80 for a special course
	PADDING(0x4661, 0x4664);

public:
	IFF_STRUCT::sCourse *m_pCurCourse;               // 0x4664, the item manager's record of m_curMap (SetCurMap)
	unsigned char m_roomType;                        // 0x4668 (6 guild, 7 shows the shot board)
	char m_roomName[0x104];                          // 0x4669, the game type's room name (SetGameType)
	unsigned char m_holes;                           // 0x476D

protected:
	unsigned char m_weather;                         // 0x476E
	PADDING(0x476F, 0x4770);

public:
	unsigned long m_shotTimeLimit;                   // 0x4770, the rule classes' SetPlayer clears it offline

public:
	unsigned long m_gameTimeLimit;                   // 0x4774

public:
	unsigned char m_courseHoleOn[18];                // 0x4778, per course hole (index course hole - 1): nonzero when the hole is played (FrScoreDlg::OnHoleListOwnerDraw/GetMyCurHoleInGuildMatch)

protected:
	PADDING(0x478A, 0x4790);

public:
	sTeamResult m_teamResult[2];                     // 0x4790, red and blue (FrResultDlg::FindWinner)
	short m_teamPoint[2];                            // 0x47B0, red and blue, shown instead of m_teamResult's score when m_roomType is not 5

protected:
	unsigned long m_value47B4[2];                    // 0x47B4, cleared by ClearGameVars
	PADDING(0x47BC, 0x47C0);

public:
	__int64 m_quitGamePang;                          // 0x47C0, the player's game pang when he leaves the game early, else 0 (CGolfRule::ReturnToLobbyDlgResult)

protected:
	PADDING(0x47C8, 0x47CC);
	unsigned long m_value47CC;                       // 0x47CC (cleared by the constructor)
	bool m_flag47D0;                                 // 0x47D0 (cleared by the constructor)
	PADDING(0x47D1, 0x47D4);

public:
	unsigned long m_myRoomOwnerUID;                  // 0x47D4, set to MyUID() by CTaskMain::OnUnderBar_ItemStorageUp before MakeRealMyRoom

public:
	// 0x47D8..0x4843 hold an sRealMyRoomAuthority member by member: 0x0123
	// fills them (CTask::OnPacketCommon), 0x00B0 sends the 0x6B bytes back.
	unsigned long m_realMyRoomUID;                   // 0x47D8, the uid of the My Room being entered (CTaskMain::HandleMsgCommon)
	unsigned short m_authReserved;                   // 0x47DC, sRealMyRoomAuthority::reserved
	bool m_authSetting;                              // 0x47DE, copied into CRealMyRoom_AuthorityDlg::m_bAuthSetting
	char m_authPassword[0x64];                       // 0x47DF, the room's password (CRealMyRoom_AuthorityDlg::OnAuth_PasswordInit)
	unsigned char m_value4843;                       // 0x4843, cleared by ClearGameVars
	unsigned char m_value4844;                       // 0x4844, cleared by ClearGameVars
	sPangYaUserStatistics m_oldSeasonStatistics;     // 0x4845, the previous season's statistics (ClearOldSeasonStat)
	sTrophyStatistics m_oldSeasonTrophy;             // 0x4930, the previous season's medals (ClearOldSeasonStat)
	PADDING(0x497E, 0x4980);
	std::list<sSpecialTrophy> m_specialTrophies;     // 0x4980 (constructor)
	std::list<sGuildTrophy> m_guildTrophies;         // 0x498C (constructor)

	// The room the player is in: m_holeCount (CLobbyMain::ReLoadHoleItem
	// compares it with 18), m_gameType (6: guild lobby, FrUserListDlg disables
	// the invite button), m_roomNumber (0xFFFF: none, in the lobby;
	// CGMToolkit::CommandDestroy), the guilds of a guild match from m_guildUID
	// on (an sGuildMatchInfo), m_roomMode (14 shows the blank score panels).
	sRoomInfo m_roomInfo;                            // 0x4998

protected:
	PADDING(0x4A4A, 0x4A4C);

public:
	std::map<unsigned long, sBriefUserInfo> m_userMap; // 0x4A4C, the lobby's players by UID (FrUserListDlg)

public:
	std::list<sRoomInfo> m_roomList;                 // 0x4A58, read by CIntrusion::IsIntrusionGame

public:
	std::list<sChannelInfo> m_channelList;           // 0x4A64, the game server's channels (CProjectG::OnGameServerResult clears it)

public:
	std::list<sSlotInfo> m_roomPlayerList;           // 0x4A70, walked by the rule classes' SetPlayer

public:
	std::list<sSlotInfo> m_roomPlayerList2;          // 0x4A7C, a second list of the room's players (CLobbyTask::OnPacket 0xAC)

public:
	std::list<sGameServerInfo> m_gameServerList;     // 0x4A88, the game servers (FrServerDlg::BuildServerList, CProjectG::OnGameServerResult)

protected:
	std::vector<sGameServerInfo> m_vector4A94;       // 0x4A94 (only constructed so far)
	std::list<sRoomUserInfo> m_roomUserList;         // 0x4AA4 (only constructed so far)

public:
	const char* m_lobbyMessage;                      // 0x4AB0, reset to "" by CLobbyMain's constructor (role not known)
	int m_lobbyStateValue;                           // 0x4AB4, zeroed by ClearLobbyStateVars (role not known)

public:
	sChannelInfo m_currentChannel;                   // 0x4AB8, its flags are read by FrMakeRoomDlg and FrReplayModeDlg

	sGameServerInfo m_curServer;                     // 0x4B05, the connected game server (FrServerDlg::ConnectToServer, packet 0xF7 copy it); m_flags2 tested by _util::IsAvatarChatServer

protected:
	PADDING(0x4B61, 0x4B64);
	std::list<unsigned long> m_list4B64;             // 0x4B64 (only constructed so far)
	std::map<int, GuildMemberInfo_t> m_guildMembers; // 0x4B70 (only constructed so far)

public:
	bool m_paranNickChanged;                         // 0x4B7C, tested by LobbyMain OnParanNickChangeResult
	bool m_noticeShown;                              // 0x4B7D, set by CTaskMain::OnUnderBar_NoticeUp
	bool m_3DBackgroundOff;                          // 0x4B7E, TurnOff3DBackground
	bool m_lobbyStateFlag;                           // 0x4B7F, cleared by ClearLobbyStateVars (role not known)
	bool m_lobbyStateFlag2;                          // 0x4B80, cleared by ClearLobbyStateVars (role not known)

protected:
	sChannelInfo m_selectedChannel;                  // 0x4B81, the channel chosen in FrServerDlg::OnChannelListBtnDown
	PADDING(0x4BCE, 0x4BD0);
	std::map<unsigned long, sFriend> m_friendMap;    // 0x4BD0 (only constructed so far)
	sFriend m_myFriendInfo;                          // 0x4BDC (only constructed so far)
	PADDING(0x4C69, 0x4C6C);

public:
	ChatManager m_chatManager;                       // 0x4C6C, chat filters, flood control, chat commands

protected:
	PADDING(0x4C90, 0x4CA4);

public:
	std::list<sChatLine> m_chatLineList;             // 0x4CA4, the chat view's lines (FrChatFuncDlg::OnChatViewInit)
	std::list<std::string> m_chatHistory;            // 0x4CB0, the chat lines already sent (ChatManager::RecordMyChatHistory keeps 20)

public:
	std::list<std::string> m_ignoreList;             // 0x4CBC, nicknames FrFriendInfoDlg::OnIgnoreBtnUp adds/removes

protected:
	int m_leaveReason;                               // 0x4CC8, why the player left the game: 1, 2, 4 or 8 picks CGolfTask::Process's notice

public:
	std::list<std::string> m_recentWhisperList;      // 0x4CCC, the nicknames offered as whisper targets (InitRecentWhisperList)

public:
	std::list<std::string> m_list4CD8;               // 0x4CD8, the chat targets (all, team, guild; CLobbyMain::ProcessDialogKeyboard cycles them with Get{Prev,Next}ChatTarget)

protected:
	int m_value4CE4;                                 // 0x4CE4, 8 at construction
	std::string m_string4CE8;                        // 0x4CE8 (only constructed so far)
	unsigned long m_value4D04;                       // 0x4D04, cleared by ClearCommonVars

public:
	std::list<sNoteInfo> m_noteList;                 // 0x4D08, the received notes (FrMailBoxDlg)

protected:
	std::string m_string4D14;                        // 0x4D14 (only constructed so far)

public:
	std::string m_latestVersion;                     // 0x4D30, the client version the server announced (CLobbyMain::LatestVersion compares it with the own one)

protected:
	std::string m_string4D4C;                        // 0x4D4C, "" at construction
	int m_controlServerService;                      // 0x4D68
	int m_offlinePlay;                               // 0x4D6C
	unsigned long m_value4D70;                       // 0x4D70, cleared by ClearCommonVars
	unsigned long m_value4D74;                       // 0x4D74, likewise

public:
	WFlags m_serverProperty;                         // 0x4D78, eGameServerProperty bits (S5::UTIL::IsEventServerControl)

protected:
	unsigned char m_value4D7C;                       // 0x4D7C, cleared by ClearGameVars

public:
	bool m_logoutPending;                            // 0x4D7D, cleared by FrLogoutDlg::OnLogoutBtnUp when set

protected:
	unsigned char m_value4D7E;                       // 0x4D7E, 0xFF after ClearCommonVars

public:
	bool m_newMapEvent;                              // 0x4D7F, CTaskManager sets it with the flags (ClearCommonVars clears both)
	int m_newMapEventFlags;                          // 0x4D80

protected:
	bool m_flag4D84;                                 // 0x4D84, cleared by ClearCommonVars
	PADDING(0x4D85, 0x4D88);
	int m_value4D88;                                 // 0x4D88, -1 after ClearCommonVars

public:
	unsigned long m_tutorialData[3];                 // 0x4D8C, written by CTutorialTask::OnPacket (one element, or all 12 bytes by DecodeBuffer)

protected:
	unsigned long m_value4D98;                       // 0x4D98, 15 after ClearCommonVars
	std::string m_lastGiftReceiverNick;              // 0x4D9C, the nickname FrGiftDlg::GiftProgress last sent a gift to; "" after ClearCommonVars
	unsigned long m_lastGiftReceiverUID;             // 0x4DB8, that receiver's UID (FrGiftDlg::GiftProgress); -1 after ClearCommonVars

public:
	unsigned char m_tutorialFinish;                  // 0x4DBC, read by TutorialMain OnTutorialMainFinish

	// Messenger alarm states: FrMessengerDlg's Notify3/Notify4 set them,
	// TaskMain and the rule classes test them before showing the alarm button.
	unsigned char m_value4DBD;                       // 0x4DBD, cleared by ClearCommonVars
	unsigned char m_value4DBE;                       // 0x4DBE, likewise

protected:
	PADDING(0x4DBF, 0x4DC0);

public:
	sLevelExp m_levelExp[71];                        // 0x4DC0, filled by LoadLevelTable

public:
	sCaddieLevelExp m_caddieLevelExp[3];             // 0x4FF8, per caddie level below 3 (FrCaddieInfoDlg::OnInit)

public:
	unsigned long m_trophyUID[2];                    // 0x5010, by team, a guild match player: the guild trophy shows when his team won (FrResultTeamDlg::OnStageInit/OnInit)

protected:
	unsigned long m_value5018;                       // 0x5018, cleared by ClearCommonVars
	int m_matchGameCount;                            // 0x501C, the match's game count ("%d" in FrResultTeamDlg::OnMatchInit)
	bool m_flag5020;                                 // 0x5020, cleared by ClearCommonVars
	PADDING(0x5021, 0x5024);
	std::string m_string5024;                        // 0x5024 (only constructed so far)
	CGolfDoc *m_pGolfDoc;                            // 0x5040
	unsigned long m_value5044;                       // 0x5044, cleared by ClearCommonVars
	unsigned long m_value5048;                       // 0x5048, likewise
	unsigned long m_value504C;                       // 0x504C, likewise
	int m_value5050;                                 // 0x5050, 1 after ClearCommonVars
	unsigned char m_value5054;                       // 0x5054, cleared by ClearCommonVars
	unsigned char m_value5055;                       // 0x5055, likewise
	unsigned char m_value5056;                       // 0x5056, likewise
	PADDING(0x5057, 0x5058);
	unsigned short m_value5058;                      // 0x5058, likewise
	unsigned char m_value505A;                       // 0x505A, likewise
	PADDING(0x505B, 0x505C);

public:
	// The top page's point event (CLobbyMain::OnToppage_PointEventDown sets
	// the state to 2 and clears the flag when it requests the event, packet 0x95).
	int m_pointEventState;                           // 0x505C
	unsigned long m_value5060;                       // 0x5060, a dword of the login reply (CTask::OnPacketCommon 0x42)
	int m_value5064;                                 // 0x5064, cleared by ClearCommonVars
	bool m_pointEventFlag;                           // 0x5068

protected:
	PADDING(0x5069, 0x506C);
	unsigned long m_value506C;                       // 0x506C, cleared by ClearCommonVars

public:
	unsigned long m_realEquip[0x1B];                 // 0x5070

protected:
	std::list<sMapEventInfo> m_mapEventList;       // 0x50DC, cleared by ClearAll where local content 0x39 is on
	std::list<sShotDataStorage> m_shotDataList;      // 0x50E8, std::list<CGolfRule::sShotData> in the original; cleared by ClearAll where local content 0x4F is on
	bool m_flag50F4;                                 // 0x50F4, cleared by ClearCommonVars

public:
	bool m_storageLock;                              // 0x50F5, copied into CRealMyRoom_AuthorityDlg::m_bStorageLock

protected:
	bool m_flag50F6;                                 // 0x50F6, cleared by ClearCommonVars
	PADDING(0x50F7, 0x50F8);

public:
	CPartTidList m_myPartTidList;                    // 0x50F8, GetMyPartTidList

protected:
	unsigned long m_myAuxPartTidList[5];             // 0x51C0
	bool m_haveAngelWing;                            // 0x51D4
	PADDING(0x51D5, 0x51D8);
	std::string m_loginAuthKey;                      // 0x51D8, GetLoginAuthKey
	unsigned short m_securityKey;                    // 0x51F4
	PADDING(0x51F6, 0x51F8);
	unsigned long m_flagAbilityItem;                 // 0x51F8
	unsigned char m_numBonusPangItem;                // 0x51FC
	PADDING(0x51FD, 0x5200);
	eStateTrade m_stateTrade;                        // 0x5200
	std::list<sTradeItem> m_listTradeItem;           // 0x5204
	std::list<sItemAttachCard> m_itemAttachCardList; // 0x5210 (only constructed so far)
	unsigned long m_tradeUID;                        // 0x521C
	std::string m_tradeTitle;                        // 0x5220, the personal shop's title
	int m_countVisitor;                              // 0x523C
	unsigned char m_tradeMode;                       // 0x5240
	sStall m_stallItem;                              // 0x5241
	std::list<sStall> m_listStallItem;               // 0x5B68
	std::list<sSaleItem> m_maketList;                // 0x5B74, GetMaketList
	unsigned char m_rmrMode;                         // 0x5B80
	PADDING(0x5B81, 0x5B88);
	__int64 m_tradeIncome;                           // 0x5B88

public:
	bool m_haveHalloweenItem;                        // 0x5B90, read by FrCheckoutDlg::OnConfirmResult

protected:
	bool m_wearHalloweenItem;                        // 0x5B91
	PADDING(0x5B92, 0x5B94);

public:
	int m_replayState;                               // 0x5B94, 14 once FrReplayModeDlg starts a replay
	int m_replayPlayType;                            // 0x5B98, read and written directly by FrReplayDlg

public:
	// Read directly by the rule classes (no getter call in the original).
	eReplayModeType m_playingMode;                   // 0x5B9C
	char m_fileName[0x20];                           // 0x5BA0, the replay file LoadShot opens
	// 0x5BC0, the replayed shot's sBar::m_prevAccuracy: GolfRule's LoadShot
	// restores it, CScreen::DrawHitBarInfo copies it back into the bar; cleared
	// by the constructor.
	float m_replayPrevAccuracy;
	// Set by FrReplayModeDlg::OnOkBtnUp when a replay starts.
	int m_replayStartA;                              // 0x5BC4, cleared
	unsigned long m_replayChannelFlags;              // 0x5BC8, the current channel's flags
	int m_replayStartB;                              // 0x5BCC, cleared

	// XOR key of the hole records' crypted shot count (CShotCountInfo::Display
	// decodes `~key ^ byte`); cleared by the constructor.
	unsigned char m_shotCountKey;                    // 0x5BD0

protected:
	PADDING(0x5BD1, 0x5BD4);
	int m_value5BD4;                                 // 0x5BD4, 6385 at construction
	unsigned long m_serverTimeTick;                  // 0x5BD8, GetTickCount() when SetServerTime received m_serverTime
	bool m_flag5BDC;                                 // 0x5BDC, cleared by ClearCommonVars
	PADDING(0x5BDD, 0x5BE0);
	CPeriodContents *m_pPeriodDoc;                   // 0x5BE0
	int m_nowGoWithMode;                             // 0x5BE4
	int m_goWithFriendUID;                           // 0x5BE8
	sRoomInfo m_roomInfo2;                           // 0x5BEC (only constructed so far)
	unsigned char m_directMoveServer;                // 0x5C9E
	PADDING(0x5C9F, 0x5CA0);
	unsigned short m_directMoveRoom;                 // 0x5CA0
	PADDING(0x5CA2, 0x5CA4);
	unsigned long m_directMoveUID;                   // 0x5CA4
	unsigned long m_friendGameSvrUID;                // 0x5CA8
	int m_inviteMode;                                // 0x5CAC, SetInviteMode/IsInviteMode; zeroed by CLobbyTask::OnPacket 0x47 (entering a room) when IsLocalContent(0x54)
	PADDING(0x5CB0, 0x5CB4);

public:
	std::map<unsigned long, sUserInfoTime> m_userInfoTimeMap; // 0x5CB4, the players' information by UID

protected:
	int m_userInfoMod;                               // 0x5CC0

public:
	unsigned char m_forceNicknameChange;             // 0x5CC4, tested by CLobbyMain

protected:
	PADDING(0x5CC5, 0x5CC8);
	std::map<unsigned char, sMapStatistics> m_myPastMapStat[4]; // 0x5CC8 (GetMyPastMapStat)
	std::map<std::string, std::list<std::string> > m_visGroups; // 0x5CF8 (GetVisGroup)
	bool m_goStopMode;                               // 0x5D04
	PADDING(0x5D05, 0x5D08);
public:
	unsigned long m_value5D08;                       // 0x5D08, cleared by ClearCommonVars; 2 makes CLobbyMain::ProcessLobbyUI open the treasure approach gift (room type 10)
protected:
	bool m_flag5D0C;                                 // 0x5D0C, cleared by the constructor
	PADDING(0x5D0D, 0x5D10);

public:
	std::list<sCardStack> m_cardList;                // 0x5D10, the card stacks the player owns (GetCardInfoToUID / GetCardInfoToTID)

protected:
	std::list<sBurnningSpCard> m_burningSpCards;     // 0x5D1C (only constructed so far)
public:
	bool m_loginDlgCreated;                          // 0x5D28, set by FrLoginDlg's constructor
	PADDING(0x5D29, 0x5D30);
};
#pragma pack(pop)

CHECK_SIZE(CSharedDoc, 0x5D30);
BEGIN_LAYOUT_CHECK(CSharedDoc)
	CHECK_FIELD(CSharedDoc, m_myMapStatistics, 0x694);
	CHECK_FIELD(CSharedDoc, m_equipSkin, 0x678);
	CHECK_FIELD(CSharedDoc, m_realMyRoomUID, 0x47D8);
	CHECK_FIELD(CSharedDoc, m_stringMap, 0x114C);
	CHECK_FIELD(CSharedDoc, m_userInfoExtBlock, 0xD4C);
	CHECK_FIELD(CSharedDoc, m_value5060, 0x5060);
	CHECK_FIELD(CSharedDoc, m_questList, 0x1278);
	CHECK_FIELD(CSharedDoc, m_gainedItemList, 0x44C4);
	CHECK_FIELD(CSharedDoc, m_guildMatchFlags, 0x4534);
	CHECK_FIELD(CSharedDoc, m_roomInfo, 0x4998);
	CHECK_FIELD(CSharedDoc, m_roomUserList, 0x4AA4);
	CHECK_FIELD(CSharedDoc, m_guildMembers, 0x4B70);
	CHECK_FIELD(CSharedDoc, m_myFriendInfo, 0x4BDC);
	CHECK_FIELD(CSharedDoc, m_string4CE8, 0x4CE8);
	CHECK_FIELD(CSharedDoc, m_lastGiftReceiverNick, 0x4D9C);
	CHECK_FIELD(CSharedDoc, m_string5024, 0x5024);
	CHECK_FIELD(CSharedDoc, m_shotDataList, 0x50E8);
	CHECK_FIELD(CSharedDoc, m_itemAttachCardList, 0x5210);
	CHECK_FIELD(CSharedDoc, m_value5BD4, 0x5BD4);
	CHECK_FIELD(CSharedDoc, m_shotCountKey, 0x5BD0);
	CHECK_FIELD(CSharedDoc, m_roomInfo2, 0x5BEC);
	CHECK_FIELD(CSharedDoc, m_myPastMapStat, 0x5CC8);
	CHECK_FIELD(CSharedDoc, m_visGroups, 0x5CF8);
	CHECK_FIELD(CSharedDoc, m_burningSpCards, 0x5D1C);
	CHECK_FIELD(CSharedDoc, m_loginServerAddress, 0x28);
	CHECK_FIELD(CSharedDoc, m_gameServerAddress, 0x44);
	CHECK_FIELD(CSharedDoc, m_gameServerPort, 0x7C);
	CHECK_FIELD(CSharedDoc, m_loginConnectValue, 0x8C);
	CHECK_FIELD(CSharedDoc, m_rankServerAddress, 0x98);
	CHECK_FIELD(CSharedDoc, m_rankServerPort, 0xB4);
	CHECK_FIELD(CSharedDoc, m_loginTime, 0xC8);
	CHECK_FIELD(CSharedDoc, m_serverTime, 0x158);
	CHECK_FIELD(CSharedDoc, m_itemManager, 0x1E8);
	CHECK_FIELD(CSharedDoc, m_cardPeriodList, 0x1248);
	CHECK_FIELD(CSharedDoc, m_myCash, 0x1288);
	CHECK_FIELD(CSharedDoc, m_myBonusCash, 0x1298);
	CHECK_FIELD(CSharedDoc, m_pCurCourse, 0x4664);
	CHECK_FIELD(CSharedDoc, m_authPassword, 0x47DF);
	CHECK_FIELD(CSharedDoc, m_value90, 0x90);
	CHECK_FIELD(CSharedDoc, m_gameStatistics, 0x4118);
	CHECK_FIELD(CSharedDoc, m_value456C, 0x456C);
	CHECK_FIELD(CSharedDoc, m_value47B4, 0x47B4);
	CHECK_FIELD(CSharedDoc, m_value4843, 0x4843);
	CHECK_FIELD(CSharedDoc, m_value4844, 0x4844);
	CHECK_FIELD(CSharedDoc, m_oldSeasonStatistics, 0x4845);
	CHECK_FIELD(CSharedDoc, m_oldSeasonTrophy, 0x4930);
	CHECK_FIELD(CSharedDoc, m_value4D7C, 0x4D7C);
	CHECK_FIELD(CSharedDoc, m_userInfo, 0x3E2);
	CHECK_FIELD(CSharedDoc, m_userInfo.nickname, 0x3F8);
	CHECK_FIELD(CSharedDoc, m_userInfo.guildName, 0x40E);
	CHECK_FIELD(CSharedDoc, m_userInfo.guildMark, 0x423);
	CHECK_FIELD(CSharedDoc, m_userInfo.identity, 0x433);
	CHECK_FIELD(CSharedDoc, m_userInfo.galleryGuid, 0x437);
	CHECK_FIELD(CSharedDoc, m_userInfo.guid, 0x43B);
	CHECK_FIELD(CSharedDoc, m_userInfo.guildUID, 0x44B);
	CHECK_FIELD(CSharedDoc, m_userInfo.guildPosition, 0x44F);
	CHECK_FIELD(CSharedDoc, m_userInfo.flagBlock, 0x45F);
	CHECK_FIELD(CSharedDoc, m_loginId, 0x46B);
	CHECK_FIELD(CSharedDoc, m_myUID, 0x4EB);
	CHECK_FIELD(CSharedDoc, m_myStatShots, 0x4EF);
	CHECK_FIELD(CSharedDoc, m_myStatistics, 0x4EF);
	CHECK_FIELD(CSharedDoc, m_myStatPuttAttempts, 0x4F3);
	CHECK_FIELD(CSharedDoc, m_myStatPangyas, 0x503);
	CHECK_FIELD(CSharedDoc, m_myStatDrives, 0x513);
	CHECK_FIELD(CSharedDoc, m_myStatPar3Drives, 0x517);
	CHECK_FIELD(CSharedDoc, m_myStatHoleInOnes, 0x51B);
	CHECK_FIELD(CSharedDoc, m_myStatFairways, 0x51F);
	CHECK_FIELD(CSharedDoc, m_myStatPuttsSunk, 0x529);
	CHECK_FIELD(CSharedDoc, m_myExp, 0x535);
	CHECK_FIELD(CSharedDoc, m_myStatGamesPlayed, 0x57C);
	CHECK_FIELD(CSharedDoc, m_myStatNoMannerGames, 0x5A8);
	CHECK_FIELD(CSharedDoc, m_myStatCoins, 0x5BC);
	CHECK_FIELD(CSharedDoc, m_myStatTeamHoles, 0x580);
	CHECK_FIELD(CSharedDoc, m_myStatRecordCount, 0x5CA);
	CHECK_FIELD(CSharedDoc, m_myTrophy, 0x5DA);
	CHECK_FIELD(CSharedDoc, m_myLevel, 0x539);
	CHECK_FIELD(CSharedDoc, m_roomName, 0x4669);
	CHECK_FIELD(CSharedDoc, m_paranNickChanged, 0x4B7C);
	CHECK_FIELD(CSharedDoc, m_tutorialFinish, 0x4DBC);
	CHECK_FIELD(CSharedDoc, m_value80, 0x80);
	CHECK_FIELD(CSharedDoc, m_gameServerUID, 0x84);
	CHECK_FIELD(CSharedDoc, m_value88, 0x88);
	CHECK_FIELD(CSharedDoc, m_value94, 0x94);
	CHECK_FIELD(CSharedDoc, m_value4D04, 0x4D04);
	CHECK_FIELD(CSharedDoc, m_value4D70, 0x4D70);
	CHECK_FIELD(CSharedDoc, m_value4D74, 0x4D74);
	CHECK_FIELD(CSharedDoc, m_value4D7E, 0x4D7E);
	CHECK_FIELD(CSharedDoc, m_flag4D84, 0x4D84);
	CHECK_FIELD(CSharedDoc, m_value4D88, 0x4D88);
	CHECK_FIELD(CSharedDoc, m_value4D98, 0x4D98);
	CHECK_FIELD(CSharedDoc, m_lastGiftReceiverUID, 0x4DB8);
	CHECK_FIELD(CSharedDoc, m_value4DBD, 0x4DBD);
	CHECK_FIELD(CSharedDoc, m_value4DBE, 0x4DBE);
	CHECK_FIELD(CSharedDoc, m_value5018, 0x5018);
	CHECK_FIELD(CSharedDoc, m_flag5020, 0x5020);
	CHECK_FIELD(CSharedDoc, m_value5044, 0x5044);
	CHECK_FIELD(CSharedDoc, m_value5048, 0x5048);
	CHECK_FIELD(CSharedDoc, m_value504C, 0x504C);
	CHECK_FIELD(CSharedDoc, m_value5050, 0x5050);
	CHECK_FIELD(CSharedDoc, m_value5054, 0x5054);
	CHECK_FIELD(CSharedDoc, m_value5055, 0x5055);
	CHECK_FIELD(CSharedDoc, m_value5056, 0x5056);
	CHECK_FIELD(CSharedDoc, m_value5058, 0x5058);
	CHECK_FIELD(CSharedDoc, m_value505A, 0x505A);
	CHECK_FIELD(CSharedDoc, m_value5064, 0x5064);
	CHECK_FIELD(CSharedDoc, m_value506C, 0x506C);
	CHECK_FIELD(CSharedDoc, m_flag50F4, 0x50F4);
	CHECK_FIELD(CSharedDoc, m_flag50F6, 0x50F6);
	CHECK_FIELD(CSharedDoc, m_flag5BDC, 0x5BDC);
	CHECK_FIELD(CSharedDoc, m_value5D08, 0x5D08);
	CHECK_FIELD(CSharedDoc, m_myPang, 0x53A);
	CHECK_FIELD(CSharedDoc, m_myCharacterId, 0x62C);
	CHECK_FIELD(CSharedDoc, m_mySlot, 0x3E0);
	CHECK_FIELD(CSharedDoc, m_myCaddieId, 0x628);
	CHECK_FIELD(CSharedDoc, m_myEquip, 0x628);
	CHECK_FIELD(CSharedDoc, m_myBallId, 0x634);
	CHECK_FIELD(CSharedDoc, m_inviteMode, 0x5CAC);
	CHECK_FIELD(CSharedDoc, m_matchTypeId, 0x4568);
	CHECK_FIELD(CSharedDoc, m_quitGamePang, 0x47C0);
	CHECK_FIELD(CSharedDoc, m_matchGameCount, 0x501C);
	CHECK_FIELD(CSharedDoc, m_myClubId, 0x630);
	CHECK_FIELD(CSharedDoc, m_equipItem, 0x638);
	CHECK_FIELD(CSharedDoc, m_caddieMap, 0x1164);
	CHECK_FIELD(CSharedDoc, m_myMascotId, 0x690);
	CHECK_FIELD(CSharedDoc, m_myMascotMap, 0x1170);
	CHECK_FIELD(CSharedDoc, m_cardList, 0x5D10);
	CHECK_FIELD(CSharedDoc, m_myItemMap, 0x1188);
	CHECK_FIELD(CSharedDoc, m_mySkinList, 0x11F4);
	CHECK_FIELD(CSharedDoc, m_outsideCash, 0x1290);
	CHECK_FIELD(CSharedDoc, m_myItemList, 0x12A0);
	CHECK_FIELD(CSharedDoc, m_myAuxItemList, 0x12AC);
	CHECK_FIELD(CSharedDoc, m_userInfo.timeBlock, 0x463);
	CHECK_FIELD(CSharedDoc, m_friendGameSvrUID, 0x5CA8);
	CHECK_FIELD(CSharedDoc, m_forceNicknameChange, 0x5CC4);
	CHECK_FIELD(CSharedDoc, m_roomPlayers, 0x12D0);
	CHECK_FIELD(CSharedDoc, m_characterMap, 0x1158);
	CHECK_FIELD(CSharedDoc, m_roomSlotMap, 0x12B8);
	CHECK_FIELD(CSharedDoc, m_indexMap, 0x12C4);
	CHECK_FIELD(CSharedDoc, m_userInfoTimeMap, 0x5CB4);
	CHECK_FIELD(CSharedDoc, m_userMap, 0x4A4C);
	CHECK_FIELD(CSharedDoc, m_roomList, 0x4A58);
	CHECK_FIELD(CSharedDoc, m_channelList, 0x4A64);
	CHECK_FIELD(CSharedDoc, m_roomPlayerList, 0x4A70);
	CHECK_FIELD(CSharedDoc, m_roomPlayerList2, 0x4A7C);
	CHECK_FIELD(CSharedDoc, m_gameResultList, 0x4504);
	CHECK_FIELD(CSharedDoc, m_block4540, 0x4540);
	CHECK_FIELD(CSharedDoc, m_gameOver, 0x455C);
	CHECK_FIELD(CSharedDoc, m_gameMode, 0x4560);
	CHECK_FIELD(CSharedDoc, m_holeOrder, 0x45F2);
	CHECK_FIELD(CSharedDoc, m_holePin, 0x45CE);
	CHECK_FIELD(CSharedDoc, m_awardPlayers, 0x456E);
	CHECK_FIELD(CSharedDoc, m_winnerPlayers, 0x459E);
	CHECK_FIELD(CSharedDoc, m_medalWinners, 0x459E);
	CHECK_FIELD(CSharedDoc, m_holeCourse, 0x45E0);
	CHECK_FIELD(CSharedDoc, m_levelExp, 0x4DC0);
	CHECK_FIELD(CSharedDoc, m_caddieLevelExp, 0x4FF8);
	CHECK_FIELD(CSharedDoc, m_pangRate, 0x4654);
	CHECK_FIELD(CSharedDoc, m_pangPercent, 0x4658);
	CHECK_FIELD(CSharedDoc, m_timeBooster, 0x465C);
	CHECK_FIELD(CSharedDoc, m_specialTrophies, 0x4980);
	CHECK_FIELD(CSharedDoc, m_guildTrophies, 0x498C);
	CHECK_FIELD(CSharedDoc, m_courseHoleOn, 0x4778);
	CHECK_FIELD(CSharedDoc, m_gameTypeNames, 0xD8);
	CHECK_FIELD(CSharedDoc, m_gameDesc, 0xCC);
	CHECK_FIELD(CSharedDoc, m_japanState, 0x184);
	CHECK_FIELD(CSharedDoc, m_billingType, 0x1A4);
	CHECK_FIELD(CSharedDoc, m_commandLineAccount, 0x1A8);
	CHECK_FIELD(CSharedDoc, m_japanFlag, 0x1C4);
	CHECK_FIELD(CSharedDoc, m_japanParam1, 0x1C8);
	CHECK_FIELD(CSharedDoc, m_japanParam2, 0x1CC);
	CHECK_FIELD(CSharedDoc, m_gameStartTime, 0x4650);
	CHECK_FIELD(CSharedDoc, m_holeRandom, 0x4608);
	CHECK_FIELD(CSharedDoc, m_myStatHolesPlayed, 0x5C6);
	CHECK_FIELD(CSharedDoc, m_curMap, 0x4660);
	CHECK_FIELD(CSharedDoc, m_roomType, 0x4668);
	CHECK_FIELD(CSharedDoc, m_holes, 0x476D);
	CHECK_FIELD(CSharedDoc, m_weather, 0x476E);
	CHECK_FIELD(CSharedDoc, m_shotTimeLimit, 0x4770);
	CHECK_FIELD(CSharedDoc, m_gameTimeLimit, 0x4774);
	CHECK_FIELD(CSharedDoc, m_gameKind, 0x4564);
	CHECK_FIELD(CSharedDoc, m_winningTeam, 0x456D);
	CHECK_FIELD(CSharedDoc, m_teamResult, 0x4790);
	CHECK_FIELD(CSharedDoc, m_teamPoint, 0x47B0);
	CHECK_FIELD(CSharedDoc, m_myRoomOwnerUID, 0x47D4);
	CHECK_FIELD(CSharedDoc, m_curServer, 0x4B05);
	CHECK_FIELD(CSharedDoc, m_selectedChannel, 0x4B81);
	CHECK_FIELD(CSharedDoc, m_noticeShown, 0x4B7D);
	CHECK_FIELD(CSharedDoc, m_serverProperty, 0x4D78);
	CHECK_FIELD(CSharedDoc, m_logoutPending, 0x4D7D);
	CHECK_FIELD(CSharedDoc, m_serverTimeTick, 0x5BD8);
	CHECK_FIELD(CSharedDoc, m_currentChannel, 0x4AB8);
	CHECK_FIELD(CSharedDoc, m_3DBackgroundOff, 0x4B7E);
	CHECK_FIELD(CSharedDoc, m_ignoreList, 0x4CBC);
	CHECK_FIELD(CSharedDoc, m_chatManager, 0x4C6C);
	CHECK_FIELD(CSharedDoc, m_chatLineList, 0x4CA4);
	CHECK_FIELD(CSharedDoc, m_chatHistory, 0x4CB0);
	CHECK_FIELD(CSharedDoc, m_itemInfoList, 0x11A0);
	CHECK_FIELD(CSharedDoc, m_pointEventState, 0x505C);
	CHECK_FIELD(CSharedDoc, m_pointEventFlag, 0x5068);
	CHECK_FIELD(CSharedDoc, m_myGearList, 0x1194);
	CHECK_FIELD(CSharedDoc, m_myExtraItemList, 0x11B8);
	CHECK_FIELD(CSharedDoc, m_myExtraItemList2, 0x11C4);
	CHECK_FIELD(CSharedDoc, m_giftList, 0x11D0);
	CHECK_FIELD(CSharedDoc, m_myExtraItemList3, 0x1218);
	CHECK_FIELD(CSharedDoc, m_myExtraItemList4, 0x1224);
	CHECK_FIELD(CSharedDoc, m_furnitureList, 0x1230);
	CHECK_FIELD(CSharedDoc, m_myCardList, 0x123C);
	CHECK_FIELD(CSharedDoc, m_gameServerList, 0x4A88);
	CHECK_FIELD(CSharedDoc, m_newMapEvent, 0x4D7F);
	CHECK_FIELD(CSharedDoc, m_newMapEventFlags, 0x4D80);
	CHECK_FIELD(CSharedDoc, m_myPartTidList, 0x50F8);
	CHECK_FIELD(CSharedDoc, m_recentWhisperList, 0x4CCC);
	CHECK_FIELD(CSharedDoc, m_loginAuthKey, 0x51D8);
	CHECK_FIELD(CSharedDoc, m_myPartsList, 0x117C);
	CHECK_FIELD(CSharedDoc, m_noteList, 0x4D08);
	CHECK_FIELD(CSharedDoc, m_latestVersion, 0x4D30);
	CHECK_FIELD(CSharedDoc, m_controlServerService, 0x4D68);
	CHECK_FIELD(CSharedDoc, m_offlinePlay, 0x4D6C);
	CHECK_FIELD(CSharedDoc, m_tutorialData, 0x4D8C);
	CHECK_FIELD(CSharedDoc, m_trophyUID, 0x5010);
	CHECK_FIELD(CSharedDoc, m_pGolfDoc, 0x5040);
	CHECK_FIELD(CSharedDoc, m_realEquip, 0x5070);
	CHECK_FIELD(CSharedDoc, m_mapEventList, 0x50DC);
	CHECK_FIELD(CSharedDoc, m_myAuxPartTidList, 0x51C0);
	CHECK_FIELD(CSharedDoc, m_haveAngelWing, 0x51D4);
	CHECK_FIELD(CSharedDoc, m_securityKey, 0x51F4);
	CHECK_FIELD(CSharedDoc, m_flagAbilityItem, 0x51F8);
	CHECK_FIELD(CSharedDoc, m_numBonusPangItem, 0x51FC);
	CHECK_FIELD(CSharedDoc, m_stateTrade, 0x5200);
	CHECK_FIELD(CSharedDoc, m_listTradeItem, 0x5204);
	CHECK_FIELD(CSharedDoc, m_tradeUID, 0x521C);
	CHECK_FIELD(CSharedDoc, m_tradeTitle, 0x5220);
	CHECK_FIELD(CSharedDoc, m_countVisitor, 0x523C);
	CHECK_FIELD(CSharedDoc, m_tradeMode, 0x5240);
	CHECK_FIELD(CSharedDoc, m_maketList, 0x5B74);
	CHECK_FIELD(CSharedDoc, m_stallItem, 0x5241);
	CHECK_FIELD(CSharedDoc, m_listStallItem, 0x5B68);
	CHECK_FIELD(CSharedDoc, m_rmrMode, 0x5B80);
	CHECK_FIELD(CSharedDoc, m_tradeIncome, 0x5B88);
	CHECK_FIELD(CSharedDoc, m_haveHalloweenItem, 0x5B90);
	CHECK_FIELD(CSharedDoc, m_wearHalloweenItem, 0x5B91);
	CHECK_FIELD(CSharedDoc, m_replayState, 0x5B94);
	CHECK_FIELD(CSharedDoc, m_replayPlayType, 0x5B98);
	CHECK_FIELD(CSharedDoc, m_playingMode, 0x5B9C);
	CHECK_FIELD(CSharedDoc, m_fileName, 0x5BA0);
	CHECK_FIELD(CSharedDoc, m_replayPrevAccuracy, 0x5BC0);
	CHECK_FIELD(CSharedDoc, m_replayStartA, 0x5BC4);
	CHECK_FIELD(CSharedDoc, m_replayChannelFlags, 0x5BC8);
	CHECK_FIELD(CSharedDoc, m_replayStartB, 0x5BCC);
	CHECK_FIELD(CSharedDoc, m_replayFileList, 0x11AC);
	CHECK_FIELD(CSharedDoc, m_authReserved, 0x47DC);
	CHECK_FIELD(CSharedDoc, m_authSetting, 0x47DE);
	CHECK_FIELD(CSharedDoc, m_storageLock, 0x50F5);
	CHECK_FIELD(CSharedDoc, m_pPeriodDoc, 0x5BE0);
	CHECK_FIELD(CSharedDoc, m_nowGoWithMode, 0x5BE4);
	CHECK_FIELD(CSharedDoc, m_goWithFriendUID, 0x5BE8);
	CHECK_FIELD(CSharedDoc, m_directMoveServer, 0x5C9E);
	CHECK_FIELD(CSharedDoc, m_directMoveRoom, 0x5CA0);
	CHECK_FIELD(CSharedDoc, m_directMoveUID, 0x5CA4);
	CHECK_FIELD(CSharedDoc, m_userInfoMod, 0x5CC0);
	CHECK_FIELD(CSharedDoc, m_goStopMode, 0x5D04);
	CHECK_FIELD(CSharedDoc, m_loginDlgCreated, 0x5D28);
END_LAYOUT_CHECK

// shareddoc.h:559/564/1144: defined in the class body in the original. SharedDoc
// emitted the copies the linker kept: shareddoc.cpp defines SHAREDDOC_INLINE
// before its first include and defines them out of line itself.
#ifndef SHAREDDOC_INLINE
inline eStateTrade CSharedDoc::GetStateTrade(void) {
	return m_stateTrade;
}

inline std::list<sTradeItem> *CSharedDoc::GetListTradeItem(void) {
	return &m_listTradeItem;
}

// shareddoc.h:574
inline std::list<sStall> *CSharedDoc::GetListStallItem(void) {
	return &m_listStallItem;
}

// shareddoc.h:575
inline sStall *CSharedDoc::GetStallItem(void) {
	return &m_stallItem;
}

// shareddoc.h:1144
inline void CSharedDoc::SetUserInfoMod(int mod) {
	m_userInfoMod = mod;
}
#endif

// shareddoc.h:1143: defined in the class body in the original. scoreboard
// emitted the copy the linker kept: scoreboard.cpp defines
// SCOREBOARD_SHAREDDOC_INLINE before its first include and defines it out of
// line itself.
#ifndef SCOREBOARD_SHAREDDOC_INLINE
inline int CSharedDoc::GetUserInfoMod(void) {
	return m_userInfoMod;
}
#endif

// shareddoc.h:571, 588: defined in the class body in the original;
// TaskManager emitted the copies the linker kept.
inline void CSharedDoc::SetCountVisitor(int count) {
	m_countVisitor = count;
}

inline void CSharedDoc::SetTradeIncome(__int64 income) {
	m_tradeIncome = income;
}

// shareddoc.h: defined in the class body in the original. OfflineTradeDlg
// emitted the copies the linker kept: OfflineTradeDlg.cpp defines
// OFFLINETRADEDLG_SHAREDDOC_INLINE empty before its first include.
#ifndef OFFLINETRADEDLG_SHAREDDOC_INLINE
#define OFFLINETRADEDLG_SHAREDDOC_INLINE inline
#endif

// shareddoc.h:609
OFFLINETRADEDLG_SHAREDDOC_INLINE unsigned char CSharedDoc::GetTradeMode(void) {
	return m_tradeMode;
}

// shareddoc.h:583
OFFLINETRADEDLG_SHAREDDOC_INLINE std::list<sSaleItem> *CSharedDoc::GetMaketList(void) {
	return &m_maketList;
}

// Avatar_MainUI emitted the copy the linker kept: Avatar_MainUI.cpp defines
// AVATAR_MAINUI_SHAREDDOC_INLINE empty before its first include.
#ifndef AVATAR_MAINUI_SHAREDDOC_INLINE
#define AVATAR_MAINUI_SHAREDDOC_INLINE inline
#endif

// shareddoc.h:568
AVATAR_MAINUI_SHAREDDOC_INLINE unsigned long CSharedDoc::GetTradeUID(void) {
	return m_tradeUID;
}

// shareddoc.h:550-554: defined in the class body in the original.
// BongdariShopDlg emitted the copies the linker kept: BongdariShopDlg.cpp
// defines BONGDARISHOPDLG_SHAREDDOC_INLINE empty before its first include.
#ifndef BONGDARISHOPDLG_SHAREDDOC_INLINE
#define BONGDARISHOPDLG_SHAREDDOC_INLINE inline
#endif

// shareddoc.h:550
BONGDARISHOPDLG_SHAREDDOC_INLINE short CSharedDoc::GetBsUsableTimes(void) {
	return m_userInfo.bsUsableTimes;
}

// shareddoc.h:552
BONGDARISHOPDLG_SHAREDDOC_INLINE short CSharedDoc::GetBsUsableBonusTimes(void) {
	return m_userInfo.bsUsableBonusTimes;
}

// shareddoc.h:554
BONGDARISHOPDLG_SHAREDDOC_INLINE short CSharedDoc::GetBsRemainedBonusTimes(void) {
	return m_userInfo.bsRemainedBonusTimes;
}

// shareddoc.h:1132-1136: defined in the class body in the original.
// MessengerDlg emitted the copies the linker kept: MessengerDlg.cpp defines
// MESSENGERDLG_SHAREDDOC_INLINE empty before its first include.
#ifndef MESSENGERDLG_SHAREDDOC_INLINE
#define MESSENGERDLG_SHAREDDOC_INLINE inline
#endif

// shareddoc.h:1132
MESSENGERDLG_SHAREDDOC_INLINE void CSharedDoc::SetGoWithModeStart(int friendUID) {
	m_goWithFriendUID = friendUID;
}

// shareddoc.h:1136
MESSENGERDLG_SHAREDDOC_INLINE void CSharedDoc::SetGoWithModeEnd(void) {
	m_nowGoWithMode = 0;
	m_goWithFriendUID = -1;
}

// shareddoc.h:1057
MESSENGERDLG_SHAREDDOC_INLINE void CSharedDoc::SetFriendGameSvrUID(unsigned long uid) {
	m_friendGameSvrUID = uid;
}

// shareddoc.h:1060
MESSENGERDLG_SHAREDDOC_INLINE void CSharedDoc::SetInviteMode(int mode) {
	m_inviteMode = mode;
}

// shareddoc.h:1061
MESSENGERDLG_SHAREDDOC_INLINE int CSharedDoc::IsInviteMode(void) {
	return m_inviteMode;
}

// shareddoc.h:616: defined in the class body in the original. TaskMain
// emitted the copy the linker kept: TaskMain.cpp defines
// TASKMAIN_SHAREDDOC_INLINE before its first include and defines it out of
// line itself.
#ifndef TASKMAIN_SHAREDDOC_INLINE
inline void CSharedDoc::SetRmrMode(unsigned char mode) {
	m_rmrMode = mode;
}

// shareddoc.h:620 (same arrangement: TaskMain's copy)
inline unsigned long CSharedDoc::GetFlagBlock(void) {
	return m_userInfo.flagBlock;
}

// shareddoc.h:1011 (same arrangement: TaskMain's copy)
inline bool CSharedDoc::IsHaveHalloweenItem(void) {
	return m_haveHalloweenItem;
}
#endif

// shareddoc.inl: global accessors of the shared document.
void SetHoles(unsigned char holes);
void SetWeather(unsigned char weather);
int IsGuildGame(void);
char *MyNick(void);
unsigned char GetPlayerNum(void);
WVector GetTeeShotPos(void);
struct sCameraData &GameCamera(void);
unsigned char GetCurMap(void);
bool SetCurMap(unsigned char map);
bool IsCurMapSnow(void);
int OnlinePlay(void);
__int64 MyOutsideCash(void);
int GalleryMode(void);
int AdminMode(void);
int GiftForbiddenMode(void);
int ItemDropMode(void);
unsigned long MyGuid(bool galleryGuid);
float CalcPowerBarPosition(void);               // shareddoc.cpp:5077 area, 0x414cb0
void PerformPowerBarSetting(void);
WVector GetHoleCupPos(void);                    // shareddoc.inl:368, inline below with GolfDoc.h (SharedDoc's copy 0x40fc90)
unsigned char GetHoleIndex(unsigned char hole);
int IsMassGame(void);
int IsMassGame(unsigned char gameType);
unsigned char MyIndex(void);
bool IsMyTurn(bool);
bool IsOthersTurn(bool);
unsigned char GetStartHole(void);
unsigned char GetEndHole(void);
void SetGameOver(bool gameOver);
int IsObserver(unsigned long identity);

// Avatar_Avatar.cpp defines AVATAR_AVATAR_SHAREDDOC_INLINE empty before its
// first include, so its object emits the out-of-line copy of IsAdministrator
// (0x537a70), as the original's did.
#ifndef AVATAR_AVATAR_SHAREDDOC_INLINE
#define AVATAR_AVATAR_SHAREDDOC_INLINE inline
#endif

// shareddoc.inl:549. The result goes through a local: written as one
// `return (identity & 4) == 4;` the inlined call (IsAdministrator(0xFFFFFFFF)
// in FrUniteResultDlg::OnInit, FrChangeRoomInfoDlg::OnGameTypeOwnerDraw) folds
// to `test byte ptr [doc+0x433],4`; the original keeps and/cmp/sete/test eax.
AVATAR_AVATAR_SHAREDDOC_INLINE int IsAdministrator(unsigned long identity) {
	if (identity == 0xFFFFFFFF)
		identity = WSingleton<CSharedDoc>::Instance()->m_userInfo.identity;

	int result = (identity & 4) == 4;
	return result;
}

bool IsGameType(unsigned char type);
bool IsAngelWing(unsigned long typeId);         // shareddoc.inl:562 (SharedDoc's copy, defined in SharedDoc.inl)
bool IsMannerPlayer(void);                      // shareddoc.inl:602 (RealMyRoomMainUI's copy)

// shareddoc.inl:602, inline in the original. RealMyRoomMainUI emitted the copy
// the linker kept: RealMyRoomMainUI.cpp defines REALMYROOMMAINUI_SHAREDDOC_INLINE
// before its first include and gets the out-of-line definition here.
#ifdef REALMYROOMMAINUI_SHAREDDOC_INLINE
bool IsMannerPlayer(void) {
	unsigned long games = WSingleton<CSharedDoc>::Instance()->m_myStatGamesPlayed;
	if (games < 1) {
		games = 1;
	}

	if ((float)WSingleton<CSharedDoc>::Instance()->m_myStatNoMannerGames / (float)games * 100.0f < 3.0f) {
		return true;
	}

	return false;
}
#endif
bool IsUsedItembyMe(unsigned long typeId);      // shareddoc.inl:684, inline below with GolfDoc.h (club's copy 0x474ed0)

// shareddoc.cpp:2009, SortAllMyItemList's order: by type id (0x4073f0)
bool CompareItemInfo(const sItemInfo &a, const sItemInfo &b);

// Inline in the original (shareddoc.inl, expanded by their callers; GolfRule
// emitted the copies the linker kept unless noted). SharedDoc.cpp defines
// SHAREDDOC_INLINE empty before its first include, so its object emits the
// out-of-line copies the original's did.
#ifndef SHAREDDOC_INLINE
#define SHAREDDOC_INLINE inline
#endif

// MessengerDlg.cpp defines MESSENGERDLG_SHAREDDOC_DECL_ONLY: the bodies of
// SetHoles and SetWeather, visible there, change two reloads of
// CMessengerInfo::AlarmBtnUp although it calls neither.
#ifndef MESSENGERDLG_SHAREDDOC_DECL_ONLY
// shareddoc.inl:229
SHAREDDOC_INLINE void SetHoles(unsigned char holes) {
	WSingleton<CSharedDoc>::Instance()->m_holes = holes;
}

// shareddoc.inl:293
SHAREDDOC_INLINE void SetWeather(unsigned char weather) {
	WSingleton<CSharedDoc>::Instance()->m_weather = weather;
}
#endif

// shareddoc.inl:492
SHAREDDOC_INLINE char *MyNick(void) {
	return WSingleton<CSharedDoc>::Instance()->m_userInfo.nickname;
}

// shareddoc.inl:437. TaskMain emitted the copy the linker kept; TaskMain.cpp
// (TASKMAIN_SHAREDDOC_INLINE) defines it out of line itself.
#ifndef TASKMAIN_SHAREDDOC_INLINE
inline unsigned long MyGuid(bool galleryGuid) {
	if (WSingleton<CSharedDoc>::Instance()->m_userInfo.IsIdentity(2) && galleryGuid)
		return WSingleton<CSharedDoc>::Instance()->m_userInfo.galleryGuid;

	return WSingleton<CSharedDoc>::Instance()->m_userInfo.guid;
}
#endif

// TaskMain.cpp defines TASKMAIN_SHAREDDOC_INLINE empty before its first
// include, so its object emits the out-of-line copies of OfflinePlay and
// GalleryMode, as the original's did.
#ifndef TASKMAIN_SHAREDDOC_INLINE
#define TASKMAIN_SHAREDDOC_INLINE inline
#endif

// shareddoc.inl:139 (TaskMain emitted the copy the linker kept)
TASKMAIN_SHAREDDOC_INLINE int OfflinePlay(void) {
	return WSingleton<CSharedDoc>::Instance()->m_offlinePlay;
}

// shareddoc.h:1024 (TaskMain emitted the copy the linker kept)
TASKMAIN_SHAREDDOC_INLINE eReplayModeType CSharedDoc::GetPlayingMode(void) {
	return m_playingMode;
}

// shareddoc.inl:145 (pool emitted the copy the linker kept)
inline const char *GetCurMapName(void) {
	IFF_STRUCT::sCourse *course = WSingleton<CSharedDoc>::Instance()->m_pCurCourse;
	if (course)
		return course->m_mapName;

	return 0;
}

// shareddoc.inl:155: the current course's item record
inline IFF_STRUCT::sCourse *GetCurMapProp(void) {
	return WSingleton<CSharedDoc>::Instance()->m_pCurCourse;
}

// shareddoc.inl:184 (SharedDoc emitted the copy the linker kept)
SHAREDDOC_INLINE void SetGameType(unsigned char type) {
	WSingleton<CSharedDoc>::Instance()->m_roomType = type;
	if (type < 15)
		strcpy(WSingleton<CSharedDoc>::Instance()->m_roomName, WSingleton<CSharedDoc>::Instance()->m_gameTypeNames[type].name);
}

// shareddoc.inl:277 (fallingspot emitted the copy the linker kept)
inline bool IsValidHole(unsigned char hole) {
	for (unsigned char i = 0; i < WSingleton<CSharedDoc>::Instance()->m_holes; i++) {
		if (WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] == hole)
			return true;
	}

	return false;
}

// shareddoc.inl:197 (LobbyMain's copy): a game type's info, 0 past the table
inline const sGameTypeInfo *GetGameTypeInfo(unsigned char gameType) {
	if (gameType >= 15)
		return 0;

	return &WSingleton<CSharedDoc>::Instance()->m_gameTypeNames[gameType];
}

// shareddoc.inl:647 (LobbyMain's copy): the course of the current room
inline unsigned char GetCurRoomMapType(void) {
	return WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course;
}

// shareddoc.inl:288
inline unsigned char GetWeather(void) {
	return WSingleton<CSharedDoc>::Instance()->m_weather;
}

// shareddoc.inl:298, the current course's kind (low nibble of its flags), 1
// without a course.
inline unsigned char GetDifficulty(void) {
	IFF_STRUCT::sCourse *course = WSingleton<CSharedDoc>::Instance()->m_pCurCourse;
	if (!course)
		return 1;

	return course->kind;
}

// shareddoc.inl:308 (wind emitted the copy the linker kept): the current
// course's difficulty, the high nibble of its flags (0 normal, 1 hard).
inline unsigned char GetDiffFlag(void) {
	IFF_STRUCT::sCourse *course = WSingleton<CSharedDoc>::Instance()->m_pCurCourse;
	if (!course)
		return 0;

	return course->difficulty;
}

// shareddoc.inl:323
inline unsigned long GetShotTimeLimit(void) {
	return WSingleton<CSharedDoc>::Instance()->m_shotTimeLimit;
}

// shareddoc.inl:328 (SharedDoc emitted the copy the linker kept)
SHAREDDOC_INLINE CItemManager *ItemManager(void) {
	return &WSingleton<CSharedDoc>::Instance()->m_itemManager;
}

// The courses in the order the lobby offers them (FirstMap takes the first
// one that is not a special course, LastMap the last; 0x7F marks an unused
// slot). A file-local table of the original (0xa1ce08, no symbol).
#ifdef JP_PAKS
// JP data (docs/JP_CLIENT.md): the JP exe's own order (the same 21-byte
// header static, one copy per unit, at 0xD05E80, 0xD15FE8, 0xD4362C ...:
// Abbot Mine and Mystic Ruins first, then Ice Inferno before Wiz City), with
// the two courses of the JP Course.iff it leaves out, 0x11 (Chronicle 1
// Chaos) and 0x40 (Grand Zodiac), put before the random slot so that every
// course is offered.
static const unsigned char s_courseOrder[] = {
	0x14, 0x15, 0x12, 0x13, 0x10, 0x0F, 0x0E, 0x0D, 0x0B, 0x08, 0x0A, 0x00,
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x09, 0x11, 0x40, 0x7F,
};
#define JP_COURSE_ORDER_COUNT ((int)(sizeof(s_courseOrder) / sizeof(s_courseOrder[0])))
#else
static const unsigned char s_courseOrder[18] = {
	0x13, 0x10, 0x0F, 0x0E, 0x0D, 0x0B, 0x08, 0x0A, 0x00,
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x09, 0x7F,
};
#endif

// LobbyMain.cpp defines LOBBYMAIN_SHAREDDOC_INLINE empty before its first
// include, so its object emits the out-of-line copies of FirstMap and
// LastMap, as the original's did.
#ifndef LOBBYMAIN_SHAREDDOC_INLINE
#define LOBBYMAIN_SHAREDDOC_INLINE inline
#endif

// shareddoc.inl:59
LOBBYMAIN_SHAREDDOC_INLINE unsigned char FirstMap(void) {
	if (!(WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x800))
#ifdef JP_PAKS
		return s_courseOrder[0];   // JP data: the first of the JP order
#else
		return 0x13;
#endif

	if (IsLocalContent((localContentType_t)0x3A)) {
		if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x800)
			return 0x0B;
	}

	int i;
#ifdef JP_PAKS
	for (i = 0; i < JP_COURSE_ORDER_COUNT - 1; i++) {
#else
	for (i = 0; i < 18; i++) {
#endif
		IFF_STRUCT::sCourse *course = ItemManager()->FindCourse(0x28000000 | s_courseOrder[i]);

		if ((course->courseFlags & 0x0F) < 3)
			break;
	}

	return s_courseOrder[i];
}

// shareddoc.h:1140
LOBBYMAIN_SHAREDDOC_INLINE void CSharedDoc::SetGoWithMode(int mode) {
	m_nowGoWithMode = mode;
}

// shareddoc.inl:83
LOBBYMAIN_SHAREDDOC_INLINE unsigned char LastMap(bool skipUnused) {
	if (!(WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x800))
#ifdef JP_PAKS
		// JP data: the last course of the JP order, or its random slot
		return skipUnused ? s_courseOrder[JP_COURSE_ORDER_COUNT - 2] : s_courseOrder[JP_COURSE_ORDER_COUNT - 1];
#else
		return skipUnused ? 0x09 : 0x7F;
#endif

	int i;
#ifdef JP_PAKS
	for (i = JP_COURSE_ORDER_COUNT - 1; i >= 0; i--) {
#else
	for (i = 17; i >= 0; i--) {
#endif
		if (skipUnused && s_courseOrder[i] == 0x7F)
			continue;

		IFF_STRUCT::sCourse *course = ItemManager()->FindCourse(0x28000000 | s_courseOrder[i]);

		if ((course->courseFlags & 0x0F) < 3)
			break;
	}

	return s_courseOrder[i];
}

// shareddoc.inl:7, the course before map in the selection order (map when
// none is selectable).
LOBBYMAIN_SHAREDDOC_INLINE unsigned char PrevMap(unsigned char map) {
#ifdef JP_PAKS
	int index = JP_COURSE_ORDER_COUNT - 1;
	for (int i = 0; i < JP_COURSE_ORDER_COUNT; i++) {
#else
	int index = 17;
	for (int i = 0; i < 18; i++) {
#endif
		if (s_courseOrder[i] == map) {
			index = i;
			break;
		}
	}

	for (int prev = index - 1; prev >= 0; prev--) {
		if (!IsLocalContent((localContentType_t)0x3A)
			|| !(WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x800)
			|| WSingleton<CSharedDoc>::Instance()->CanUseRookieChannelMap(s_courseOrder[prev]))
			return s_courseOrder[prev];
	}

	return map;
}

// shareddoc.inl:33, the course after map in the selection order (map when
// none is selectable).
LOBBYMAIN_SHAREDDOC_INLINE unsigned char NextMap(unsigned char map, bool) {
	int index = 0;
#ifdef JP_PAKS
	for (int i = 0; i < JP_COURSE_ORDER_COUNT; i++) {
#else
	for (int i = 0; i < 18; i++) {
#endif
		if (s_courseOrder[i] == map) {
			index = i;
			break;
		}
	}

#ifdef JP_PAKS
	for (int next = index + 1; next < JP_COURSE_ORDER_COUNT; next++) {
#else
	for (int next = index + 1; next < 18; next++) {
#endif
		if (!IsLocalContent((localContentType_t)0x3A)
			|| !(WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x800)
			|| WSingleton<CSharedDoc>::Instance()->CanUseRookieChannelMap(s_courseOrder[next]))
			return s_courseOrder[next];
	}

	return map;
}

// shareddoc.inl:356
inline CGolfDoc *GOLFDOC(void) {
	return WSingleton<CSharedDoc>::Instance()->m_pGolfDoc;
}

// shareddoc.inl:431
inline unsigned long GetGameTimeLimit(void) {
	return WSingleton<CSharedDoc>::Instance()->m_gameTimeLimit;
}

// shareddoc.inl:446 (SharedDoc emitted the copy the linker kept)
SHAREDDOC_INLINE unsigned long MyUID(void) {
	return WSingleton<CSharedDoc>::Instance()->m_myUID;
}

// GolfRule.cpp defines GOLFRULE_SHAREDDOC_INLINE empty before its first
// include, so its object emits the out-of-line copy of GUILDMATCHUP
// (0x452e80), as the original's did.
#ifndef GOLFRULE_SHAREDDOC_INLINE
#define GOLFRULE_SHAREDDOC_INLINE inline
#endif

// shareddoc.inl:224
GOLFRULE_SHAREDDOC_INLINE unsigned char GetHoles(void) {
	return WSingleton<CSharedDoc>::Instance()->m_holes;
}

// shareddoc.inl:234
GOLFRULE_SHAREDDOC_INLINE unsigned char GetStartHole(void) {
	return WSingleton<CSharedDoc>::Instance()->m_holeOrder[0];
}

// shareddoc.inl:254, the 1-based play order of a course hole, 19 when it is
// not played. A plain inline even in GolfRule, whose calls cl inlines where the
// original did (DoScoreBoard); the rest still emit its COMDAT there.
inline unsigned char GetHoleIndex(unsigned char hole) {
	for (unsigned char i = 0; i < 18; i++) {
		if (WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] == hole)
			return i + 1;
	}

	return 19;
}

// shareddoc.inl:351, the result line of the player whose UID the team's
// trophy goes to in a guild match.
GOLFRULE_SHAREDDOC_INLINE sRivalData *GUILDMATCHUP(unsigned char team) {
	return &WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(WSingleton<CSharedDoc>::Instance()->m_trophyUID[team])];
}

// shareddoc.inl:421
GOLFRULE_SHAREDDOC_INLINE bool IsGameOver(void) {
	return WSingleton<CSharedDoc>::Instance()->m_gameOver;
}

// ClientSetting.cpp defines CLIENTSETTING_SHAREDDOC_INLINE empty before its
// first include, so its object emits the out-of-line copies of Doc,
// GetGameType, SetHoleType, SetShotTimeLimit, SetGameTimeLimit and MyId, as
// the original's did.
#ifndef CLIENTSETTING_SHAREDDOC_INLINE
#define CLIENTSETTING_SHAREDDOC_INLINE inline
#endif

// GhostDocument.cpp defines GHOSTDOCUMENT_SHAREDDOC_INLINE empty before its
// first include, so its object emits the out-of-line copy of GetHoleType, as
// the original's did.
#ifndef GHOSTDOCUMENT_SHAREDDOC_INLINE
#define GHOSTDOCUMENT_SHAREDDOC_INLINE inline
#endif

// shareddoc.inl:2 (ClientSetting.cpp emits the out-of-line copy)
CLIENTSETTING_SHAREDDOC_INLINE CSharedDoc *Doc(void) {
	return WSingleton<CSharedDoc>::Instance();
}

// shareddoc.inl:162 (ClientSetting.cpp emits the out-of-line copy)
CLIENTSETTING_SHAREDDOC_INLINE unsigned char GetGameType(void) {
	return WSingleton<CSharedDoc>::Instance()->m_roomType;
}

// shareddoc.inl:239
GHOSTDOCUMENT_SHAREDDOC_INLINE unsigned char GetHoleType(void) {
	return WSingleton<CSharedDoc>::Instance()->m_gameKind;
}

// shareddoc.inl:244
CLIENTSETTING_SHAREDDOC_INLINE void SetHoleType(unsigned char type) {
	WSingleton<CSharedDoc>::Instance()->m_gameKind = type;
}

// shareddoc.inl:318
CLIENTSETTING_SHAREDDOC_INLINE void SetShotTimeLimit(unsigned long limit) {
	WSingleton<CSharedDoc>::Instance()->m_shotTimeLimit = limit;
}

// shareddoc.inl:426
CLIENTSETTING_SHAREDDOC_INLINE void SetGameTimeLimit(unsigned long limit) {
	WSingleton<CSharedDoc>::Instance()->m_gameTimeLimit = limit;
}

// shareddoc.inl:487
CLIENTSETTING_SHAREDDOC_INLINE char *MyId(void) {
	return WSingleton<CSharedDoc>::Instance()->m_userInfo.id;
}

// shareddoc.inl:515
inline __int64 MyPang(void) {
	return WSingleton<CSharedDoc>::Instance()->m_myPang;
}

// shareddoc.inl:516
inline __int64 MyCookie(void) {
	return WSingleton<CSharedDoc>::Instance()->m_myCash;
}

// shareddoc.inl:518 (TaskMain emitted the copy the linker kept)
TASKMAIN_SHAREDDOC_INLINE __int64 MyOutsideCash(void) {
	return WSingleton<CSharedDoc>::Instance()->m_outsideCash;
}

// shareddoc.inl:521 (TaskManager emitted the copy the linker kept)
inline void SetMyOutsideCash(__int64 cash) {
	WSingleton<CSharedDoc>::Instance()->m_outsideCash = cash;
}

// shareddoc.inl:519 (OnelineReqDlg emitted the copy the linker kept)
inline __int64 MyBonusCash(void) {
	return WSingleton<CSharedDoc>::Instance()->m_myBonusCash;
}

// shareddoc.inl:527 (TaskMain emitted the copy the linker kept). Callers test
// its int result, not the identity bit: `if (GalleryMode())` compiles to
// mov/shr/test, `!GalleryMode()` to movzx/and 1/sete.
TASKMAIN_SHAREDDOC_INLINE int GalleryMode(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_userInfo.IsIdentity(2))
		return 1;

	return 0;
}

// shareddoc.inl, like GalleryMode for identity bit 4 (the administrator bit
// IsAdministrator tests). The original never emitted it out of line, so its
// real name is unknown. Callers test its int result: mov/shr 2/test
// (FrUserListDlg::MakeUserList, FrDtRoomDlg::OnUserListInit).
inline int AdminMode(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_userInfo.IsIdentity(4))
		return 1;

	return 0;
}

// shareddoc.inl, like GalleryMode for identity bit 0x40: the account may use
// the /itemdrop chat command (hatManager CheckChatCommand: mov/shr 6/test).
// Never emitted out of line, so its real name is unknown.
inline int ItemDropMode(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_userInfo.IsIdentity(0x40))
		return 1;

	return 0;
}

// shareddoc.inl, like GalleryMode for identity bit 0x100: the account may
// not send gifts (FrGiftDlg::OnCheckoutBtnUp refuses the checkout). Never
// emitted out of line, so its real name is unknown.
inline int GiftForbiddenMode(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_userInfo.IsIdentity(0x100))
		return 1;

	return 0;
}

#endif // PG_CLIENT_PROJECTG_SHAREDDOC_H

// shareddoc.inl's inlines that need CGolfDoc complete: defined once both
// headers are in (GolfDoc.h includes this header again at its end), without
// SharedDoc.h pulling GolfDoc.h (and its dynamic initializer) into every
// includer. SharedDoc.cpp defines SHAREDDOC_INLINE empty before its first
// include, so its object emits the out-of-line copy of PLAYER (0x4137a0), as
// the original's did.
#if defined(PG_CLIENT_PROJECTG_GOLFDOC_H) && !defined(PG_CLIENT_PROJECTG_SHAREDDOC_GOLFDOC_INLINES)
#define PG_CLIENT_PROJECTG_SHAREDDOC_GOLFDOC_INLINES

#ifndef SHAREDDOC_INLINE
#define SHAREDDOC_INLINE inline
#endif

// shareddoc.inl:340
SHAREDDOC_INLINE sPlayerData *PLAYER(unsigned char index) {
	CGolfDoc *golfDoc = WSingleton<CSharedDoc>::Instance()->m_pGolfDoc;
	return golfDoc->GetPlayer(index);
}

// shareddoc.inl:368
SHAREDDOC_INLINE WVector GetHoleCupPos(void) {
	return GOLFDOC()->GetHoleData().m_cupPos;
}

// shareddoc.inl:373 (GolfRule emitted the copy the linker kept)
GOLFRULE_SHAREDDOC_INLINE unsigned char &UIAlpha(void) {
	return GOLFDOC()->GetUIAlpha();
}

// shareddoc.inl:378 (GolfRule emitted the copy the linker kept)
GOLFRULE_SHAREDDOC_INLINE sBar &Bar(void) {
	return GOLFDOC()->GetBar();
}

// shareddoc.inl:345 (GolfRule emitted the copy the linker kept)
inline sTeamData *TEAM(unsigned char index) {
	return GOLFDOC()->GetTeam(index);
}

// shareddoc.inl:503 (GolfRule emitted the copy the linker kept). This
// client's player index: a gallery member's by its gallery GUID (in rooms 4
// to 6, 9 and 10) or the golf doc's, else by its own GUID.
GOLFRULE_SHAREDDOC_INLINE unsigned char MyIndex(void) {
	CSharedDoc *doc = WSingleton<CSharedDoc>::Instance();
	if (doc->m_userInfo.IsIdentity(2)) {
		switch (doc->m_roomType) {
		case 4:
		case 5:
		case 6:
		case 9:
		case 10:
			return doc->GetIndex(doc->m_userInfo.galleryGuid);
		default:
			return doc->m_pGolfDoc->m_myPlayerIndex;
		}
	}

	return doc->GetIndex(doc->m_userInfo.guid);
}

// s5/DemonLandDlg.cpp defines DEMONLANDDLG_SHAREDDOC_INLINE empty before its
// first include, so its object emits the out-of-line copy of GetNextHole
// (0x601b70), as the original's did.
#ifndef DEMONLANDDLG_SHAREDDOC_INLINE
#define DEMONLANDDLG_SHAREDDOC_INLINE inline
#endif

// shareddoc.inl:265, the course hole played after the current one, 19 after
// the last.
DEMONLANDDLG_SHAREDDOC_INLINE unsigned char GetNextHole(void) {
	for (unsigned char i = 0; i < 18; i++) {
		if (WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] == GOLFDOC()->m_currentHole)
			return WSingleton<CSharedDoc>::Instance()->m_holeOrder[i + 1];
	}

	return 19;
}

// club.cpp defines CLUB_SHAREDDOC_INLINE empty before its first include, so
// its object emits the out-of-line copy of IsUsedItembyMe (0x474ed0), as the
// original's did.
#ifndef CLUB_SHAREDDOC_INLINE
#define CLUB_SHAREDDOC_INLINE inline
#endif

// shareddoc.inl:684, whether the local player's current shot uses the item
// typeId (false when it is not the local player's turn).
CLUB_SHAREDDOC_INLINE bool IsUsedItembyMe(unsigned long typeId) {
	if (IsMyTurn(false)) {
		return PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == typeId ? true : false;
	}

	return false;
}

#endif
