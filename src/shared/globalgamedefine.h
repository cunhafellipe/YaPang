// Original: d:\build\custom\temp\code\shared\globalgamedefine.h
#ifndef PG_SHARED_GLOBALGAMEDEFINE_H
#define PG_SHARED_GLOBALGAMEDEFINE_H

#include <windows.h>

#include "defines.h"
#include "padding.h"

#include <list>

#include <stdlib.h>
#include <string.h>

// Function object for std::for_each over containers of owned pointers.
struct Delete_Object {
	// globalgamedefine.h:307
	template <class T> void operator()(T ptr) const {
		delete ptr;
	}
};

// Function object for std::for_each over maps of owned pointers: deletes
// each element's second and clears it.
struct Delete_SecondObject {
	// globalgamedefine.h:326
	template <class T> void operator()(T& p) const {
		delete p.second;
		p.second = 0;
	}
};

// Level-up notification handed to the level-up reward dialog
// (FrLevelupItemForm::SetData); 3 bytes, cleared by its constructor.
struct sLevelUpDone {
	sLevelUpDone(void) {   // globalgamedefine.h:1749
		m_hasReward = 0;
		m_level = 0;
		m_rewardReceived = 0;
	}

	unsigned char m_hasReward;        // 0x0
	unsigned char m_level;            // 0x1
	unsigned char m_rewardReceived;   // 0x2
};

CHECK_SIZE(sLevelUpDone, 0x3);
CHECK_FIELD(sLevelUpDone, m_hasReward, 0x0);
CHECK_FIELD(sLevelUpDone, m_level, 0x1);
CHECK_FIELD(sLevelUpDone, m_rewardReceived, 0x2);

// Where the player is, sent to the messenger server when a channel is
// entered (0x4B bytes, byte-packed). Constructor out of line
// (globalgamedefine.h:1799, emitted by the SharedDoc unit).
#pragma pack(push, 1)
struct sUserPosition {
	sUserPosition(void);

	unsigned short m_roomNumber;      // 0x00 (0xFFFF: none)
	unsigned long m_value2;           // 0x02 (-1)
	unsigned long m_userValue;        // 0x06 (-1; ConnectToServer's id, CSharedDoc::m_gameServerUID)
	unsigned char m_channelId;        // 0x0A (0xFF)
	char m_channelName[0x40];         // 0x0B
};
#pragma pack(pop)
CHECK_SIZE(sUserPosition, 0x4B);
CHECK_FIELD(sUserPosition, m_roomNumber, 0x00);
CHECK_FIELD(sUserPosition, m_value2, 0x02);
CHECK_FIELD(sUserPosition, m_userValue, 0x06);
CHECK_FIELD(sUserPosition, m_channelId, 0x0A);
CHECK_FIELD(sUserPosition, m_channelName, 0x0B);

// A game server channel, as listed in the server dialog (0x4D bytes,
// byte-packed). Looked up by id (operator==, globalgamedefine.h:3377).
#pragma pack(push, 1)
struct sChannelInfo {
	// globalgamedefine.h:3377 (std::find on the channel list by id)
	bool operator==(const unsigned char &channelId) const {
		return id == channelId;
	}

	char name[0x40];              // 0x00
	unsigned short maxUsers;      // 0x40
	unsigned short curUsers;      // 0x42
	unsigned char id;             // 0x44
	unsigned long flags;          // 0x45, 0x80: tournament rooms (game type 3), 0x4000: game type 7 (FrMakeRoomDlg)
	unsigned long eventCourseMask; // 0x49, bit per course with a pang event (OnRoomList_RoomListOwnerDraw)
};
#pragma pack(pop)
CHECK_SIZE(sChannelInfo, 0x4D);
CHECK_FIELD(sChannelInfo, flags, 0x45);
CHECK_FIELD(sChannelInfo, name, 0x00);
CHECK_FIELD(sChannelInfo, maxUsers, 0x40);
CHECK_FIELD(sChannelInfo, curUsers, 0x42);
CHECK_FIELD(sChannelInfo, id, 0x44);
CHECK_FIELD(sChannelInfo, eventCourseMask, 0x49);

// One server of a server list (0x5C bytes, byte-packed: list<sGameServerInfo>
// nodes are 0x64).
#pragma pack(push, 1)
struct sGameServerInfo {
	// globalgamedefine.h:3442 (ServerDlg constructs its members with it)
	sGameServerInfo(void) {
		name[0] = 0;
		m_address[0] = 0;
	}

	char name[0x28];                     // 0x00
	unsigned long id;                    // 0x28
	int m_maxUsers;                      // 0x2C
	int m_curUsers;                      // 0x30
	char m_address[0x12];                // 0x34
	unsigned long m_port;                // 0x46
	unsigned long flags;                 // 0x4A
	int m_badgeCount;                    // 0x4E (ServerDlg draws it as a number when > 0)
	unsigned long m_flags2;              // 0x52 (ServerListCompare tests bit 0x40 of its low byte)
	unsigned long eventCourseMask;       // 0x56, bit per course with a pang event (CLobbyMain room list draw)
	unsigned short m_iconKey;            // 0x5A (key of FrServerDlg::m_textMap: the name of the server's icon)
};
#pragma pack(pop)
CHECK_SIZE(sGameServerInfo, 0x5C);
CHECK_FIELD(sGameServerInfo, name, 0x00);
CHECK_FIELD(sGameServerInfo, id, 0x28);
CHECK_FIELD(sGameServerInfo, flags, 0x4A);
CHECK_FIELD(sGameServerInfo, m_maxUsers, 0x2C);
CHECK_FIELD(sGameServerInfo, m_curUsers, 0x30);
CHECK_FIELD(sGameServerInfo, m_address, 0x34);
CHECK_FIELD(sGameServerInfo, m_port, 0x46);
CHECK_FIELD(sGameServerInfo, m_badgeCount, 0x4E);
CHECK_FIELD(sGameServerInfo, m_flags2, 0x52);
CHECK_FIELD(sGameServerInfo, eventCourseMask, 0x56);
CHECK_FIELD(sGameServerInfo, m_iconKey, 0x5A);

// An item of the player's inventory (0xA8 bytes; constructor
// globalgamedefine.h:1950). Only the members the transcribed functions read
// are named.
struct sItemInfo {
	sItemInfo(void);
	bool operator==(const unsigned long &typeId) const; // 0x43fbe0 (emitted in GolfRule)

	unsigned long m_serial;       // 0x00
	unsigned long m_typeId;       // 0x04
	int m_remainHours;            // 0x08, hours left of a time-limited item (signed: EquipDlg divides it by 24 with idiv)
	union {
		short m_count;            // 0x0C, how many of a stackable item the player holds (TransferableItem)
		short m_clubStatLevel[5]; // 0x0C, the club's upgrade level of each stat (FrDowngradeDlg)
	};
	unsigned char m_reserved16;   // 0x16, cleared by the constructor, otherwise unused so far
	struct {
		unsigned char flag0 : 1;      // bit 0: cleared by the constructor
		unsigned char expired : 1;    // bit 1: the item's period is over
		unsigned char flags23 : 2;    // bits 2-3: cleared by the constructor
		unsigned char kind : 3;       // bits 4-6: 6 = rental (S5::RENTALPARTS::CollectRentalItem)
		unsigned char flag7 : 1;      // bit 7: kept by the constructor
	} m_stateBits;                // 0x17
	unsigned long m_reserved18[4]; // 0x18, cleared by the constructor, otherwise unused so far
	struct {
		unsigned char flags01 : 2;    // bits 0-1: cleared by the constructor
		unsigned char flags27 : 6;    // bits 2-7: kept by the constructor
	} m_flags28;                  // 0x28
	char m_uccName[0x29];         // 0x29, the UCC's name (CTask::OnPacketCommon 0x71 copies it into sUccClothes)
	char m_uccKey[9];             // 0x52
	unsigned char m_uccStatus;    // 0x5B, the UCC (self-designed item) state (TransferableItem)
	unsigned short m_uccSeq;      // 0x5C, the UCC's sequence number (TransferableItem)
	char m_reserved5E[0x16];      // 0x5E, cleared by the constructor, otherwise unused so far
	int m_cardSlots[12];          // 0x74, the type ids of the cards slotted into the item
	short m_charCardSlotCount;    // 0xA4, the character-card slots the part opens (sPart 0x1F8; FrCardRemoveDlg)
	short m_caddieCardSlotCount;  // 0xA6, the caddie-card slots the part opens (sPart 0x1FA; FrCardRemoveDlg)
};
CHECK_SIZE(sItemInfo, 0xA8);
CHECK_FIELD(sItemInfo, m_remainHours, 0x08);
CHECK_FIELD(sItemInfo, m_reserved18, 0x18);
CHECK_FIELD(sItemInfo, m_flags28, 0x28);
CHECK_FIELD(sItemInfo, m_uccName, 0x29);
CHECK_FIELD(sItemInfo, m_reserved5E, 0x5E);
CHECK_FIELD(sItemInfo, m_serial, 0x00);
CHECK_FIELD(sItemInfo, m_typeId, 0x04);
CHECK_FIELD(sItemInfo, m_count, 0x0C);
CHECK_FIELD(sItemInfo, m_clubStatLevel, 0x0C);
CHECK_FIELD(sItemInfo, m_stateBits, 0x17);
CHECK_FIELD(sItemInfo, m_uccKey, 0x52);
CHECK_FIELD(sItemInfo, m_uccStatus, 0x5B);
CHECK_FIELD(sItemInfo, m_uccSeq, 0x5C);
CHECK_FIELD(sItemInfo, m_cardSlots, 0x74);
CHECK_FIELD(sItemInfo, m_charCardSlotCount, 0xA4);
CHECK_FIELD(sItemInfo, m_caddieCardSlotCount, 0xA6);

// globalgamedefine.h:1950
inline sItemInfo::sItemInfo(void) {
	m_stateBits.flag0 = 0;
	m_stateBits.expired = 0;
	m_stateBits.flags23 = 0;
	m_stateBits.kind = 0;
	m_flags28.flags01 = 0;
	m_serial = 0;
	m_typeId = 0;
	m_remainHours = 0;
	m_reserved16 = 0;
	memset(m_clubStatLevel, 0, sizeof(m_clubStatLevel));
	memset(m_reserved18, 0, sizeof(m_reserved18));
	memset(m_uccName, 0, sizeof(m_uccName));
	memset(m_uccKey, 0, sizeof(m_uccKey));
	memset(m_reserved5E, 0, sizeof(m_reserved5E));
	memset(m_cardSlots, 0, sizeof(m_cardSlots));
	m_charCardSlotCount = 0;
	m_caddieCardSlotCount = 0;
	m_uccStatus = 0;
	m_uccSeq = 0;
}

// GolfRule.cpp defines GOLFRULE_GLOBALGAMEDEFINE_INLINE empty before its
// includes, so its object holds the out-of-line copy of sItemInfo::operator==
// (0x43fbe0) every unit can link against. EquipDlg.cpp defines
// EQUIPDLG_GLOBALGAMEDEFINE_DECL_ONLY: the original's equipdlg object saw
// only the declaration, so cl kept std::find over the item list out of line
// there (FrEquipDlg::OpenInformation calls GolfRule's copy, 0x446620).
#ifndef GOLFRULE_GLOBALGAMEDEFINE_INLINE
#define GOLFRULE_GLOBALGAMEDEFINE_INLINE inline
#endif

#ifndef EQUIPDLG_GLOBALGAMEDEFINE_DECL_ONLY
// globalgamedefine.h:1978
GOLFRULE_GLOBALGAMEDEFINE_INLINE bool sItemInfo::operator==(const unsigned long &typeId) const {
	return m_typeId == typeId;
}
#endif

// A caddie the player owns (CSharedDoc keeps them in a std::map keyed by
// id); 0x19 bytes, byte-packed (FrCaddieInfoDlg::GetCaddieInfo copies it with
// `rep movsd` x6 + `movsb`). Only the fields the transcribed functions use
// are named.
#pragma pack(push, 1)
struct sCaddieInfo {
	unsigned long id;           // 0x0 (key of CSharedDoc's caddie map)
	unsigned long typeId;       // 0x4 (sCaddie typeid, IFF_STRUCT; passed to GetIconName(const unsigned long&))
	unsigned long cadItemTypeId; // 0x8 (the sCadItem typeid of the caddie's item)
	unsigned char level;        // 0xC
	unsigned long experience;   // 0xD
	unsigned char flags;        // 0x11 (bit 2: on vacation)
	unsigned short days;        // 0x12 (contract days left; EndContract sets 30)
	unsigned short hours;       // 0x14 (the caddie item's time left, in hours)
	unsigned char reserved;     // 0x16 (no transcribed function reads it; a scalar, not a char[1], or /GS guards every local copy)
	unsigned char contract;     // 0x17 (1: the sign-up button is disabled)
	unsigned char checkEquip;   // 0x18, nonzero: CTask::OnPacketCommon 0x6F re-checks the equipped caddie
};
#pragma pack(pop)
CHECK_SIZE(sCaddieInfo, 0x19);
CHECK_FIELD(sCaddieInfo, id, 0x0);
CHECK_FIELD(sCaddieInfo, typeId, 0x4);
CHECK_FIELD(sCaddieInfo, cadItemTypeId, 0x8);
CHECK_FIELD(sCaddieInfo, hours, 0x14);
CHECK_FIELD(sCaddieInfo, level, 0xC);
CHECK_FIELD(sCaddieInfo, experience, 0xD);
CHECK_FIELD(sCaddieInfo, flags, 0x11);
CHECK_FIELD(sCaddieInfo, days, 0x12);
CHECK_FIELD(sCaddieInfo, reserved, 0x16);
CHECK_FIELD(sCaddieInfo, contract, 0x17);
CHECK_FIELD(sCaddieInfo, checkEquip, 0x18);

// One entry of the guild history (0x34 bytes from the list node allocation,
// packed; packet 0x1BC sends 0x31 bytes each). CGuildMainForm::
// RefreshMyGuildHistory prints the year, month and day of the date and the
// message of the state.
#pragma pack(push, 1)
struct GUILD_HISTORY {
	// globalgamedefine.h:111
	GUILD_HISTORY(void) : data0(-1), data4(0), state(0) {
		memset(data8, 0, sizeof(data8));
	}

	int data0;                  // 0x0: -1 by default (meaning not yet known)
	int data4;                  // 0x4
	char data8[0x15];           // 0x8: a name-sized string, zeroed by the constructor
	int state;                  // 0x1D: a GUILD_STATE_IDX (guildformcontroller.h)
	_SYSTEMTIME date;           // 0x21: wYear 0x21, wMonth 0x23, wDay 0x27 read; not initialised
	UNKNOWN(0x31, 0x34);
};
#pragma pack(pop)

CHECK_SIZE(GUILD_HISTORY, 0x34);
BEGIN_LAYOUT_CHECK(GUILD_HISTORY)
	CHECK_FIELD(GUILD_HISTORY, data0, 0x0);
	CHECK_FIELD(GUILD_HISTORY, data4, 0x4);
	CHECK_FIELD(GUILD_HISTORY, data8, 0x8);
	CHECK_FIELD(GUILD_HISTORY, state, 0x1D);
	CHECK_FIELD(GUILD_HISTORY, date, 0x21);
END_LAYOUT_CHECK

#pragma pack(push, 1)
// A guild member's view of his guild (0x119 bytes, packed: the copy in
// CGuildInfo::SetMyGuildUserInfo moves 0x46 dwords + 1 byte).
struct GUILD_USER_INFO {
	// globalgamedefine.h:137
	GUILD_USER_INFO(void) {
		guildNumber = 0;
		value19 = 0;
		value1D = 0;
		value21 = 0;
		classIndex = 0;
		valueFF = -1;
		memset(name, 0, sizeof(name));
		memset(emblem, 0, sizeof(emblem));
		memset(notice, 0, sizeof(notice));
		memset(introduce, 0, sizeof(introduce));
		memset(text103, 0, sizeof(text103));
	}

	int guildNumber;            // 0x0
	char name[0x15];            // 0x4
	int value19;                // 0x19
	int value1D;                // 0x1D
	int value21;                // 0x21
	char emblem[0xC];           // 0x25
	char notice[0x65];          // 0x31: SetGuildNotice
	char introduce[0x65];       // 0x96: SetGuildIntroduce
	int classIndex;             // 0xFB: a GUILD_CLASS_IDX
	int valueFF;                // 0xFF: -1 by default
	char text103[0x16];         // 0x103
};

// The player's guild (0x129 bytes, packed: CGuildInfo::SetMyGuildInfo moves
// 0x4A dwords + 1 byte).
struct GUILD_INFO : GUILD_USER_INFO {
	// globalgamedefine.h:151
	GUILD_INFO(void) {
	}

	char data119[0x10];         // 0x119: 16 bytes CGuildInfo::ClearAllData zeroes (meaning not yet known)
};
#pragma pack(pop)

BEGIN_LAYOUT_CHECK(GUILD_USER_INFO)
	CHECK_SIZE(GUILD_USER_INFO, 0x119);
	CHECK_FIELD(GUILD_USER_INFO, name, 0x4);
	CHECK_FIELD(GUILD_USER_INFO, emblem, 0x25);
	CHECK_FIELD(GUILD_USER_INFO, notice, 0x31);
	CHECK_FIELD(GUILD_USER_INFO, introduce, 0x96);
	CHECK_FIELD(GUILD_USER_INFO, classIndex, 0xFB);
	CHECK_FIELD(GUILD_USER_INFO, valueFF, 0xFF);
	CHECK_FIELD(GUILD_USER_INFO, text103, 0x103);
END_LAYOUT_CHECK
CHECK_SIZE(GUILD_INFO, 0x129);
CHECK_FIELD(GUILD_INFO, data119, 0x119);

// The rank of a player in his guild (only 1-3 are members; names
// invented).
enum GUILD_CLASS_IDX {
	GUILD_CLASS_NONE = 0,
	GUILD_CLASS_MASTER = 1      // the rank handed over by the master delegation
};

// One member of a guild member page (0x54 bytes from the list node
// allocation; packet 0x1C4 sends 0x51 bytes each).
struct GUILD_USER_LIST {
	// globalgamedefine.h:207
	GUILD_USER_LIST(void) : data0(0), uid(-1), m_classIdx(GUILD_CLASS_NONE), online(0) {
		memset(nickname, 0, sizeof(nickname));
		memset(data25, 0, sizeof(data25));
		memset(text, 0, sizeof(text));
	}

	int data0;                  // 0x0 (meaning not yet known)
	unsigned long uid;          // 0x4: the member's user id
	GUILD_CLASS_IDX m_classIdx; // 0x8: the member's rank
	char text[0x19];            // 0xC: the guild talk, passed through ExchangeQuotation
	char data25[0x15];          // 0x25: a name-sized string, zeroed by the constructor
	char nickname[0x16];        // 0x3A: shown by the user info window
	unsigned char online;       // 0x50: 1 online, 0 offline (the member row's state icon)
	UNKNOWN(0x51, 0x54);
};

CHECK_SIZE(GUILD_USER_LIST, 0x54);
BEGIN_LAYOUT_CHECK(GUILD_USER_LIST)
	CHECK_FIELD(GUILD_USER_LIST, data0, 0x0);
	CHECK_FIELD(GUILD_USER_LIST, uid, 0x4);
	CHECK_FIELD(GUILD_USER_LIST, m_classIdx, 0x8);
	CHECK_FIELD(GUILD_USER_LIST, text, 0xC);
	CHECK_FIELD(GUILD_USER_LIST, data25, 0x25);
	CHECK_FIELD(GUILD_USER_LIST, nickname, 0x3A);
	CHECK_FIELD(GUILD_USER_LIST, online, 0x50);
END_LAYOUT_CHECK

// A guild member as the messenger server lists him (0x37 bytes, packed: CTask
// decodes 0x37 bytes per member in packets 0x38 and 0x39 and copies it into an
// sFriend).
#pragma pack(push, 1)
struct GUILD_USER_MSN_LIST {
	// globalgamedefine.h:230
	GUILD_USER_MSN_LIST(void) : uid(0xFFFFFFFF), value4(0), sex(0), value35(0), online(0) {
		// globalgamedefine.h:231
		memset(nick, 0, sizeof(nick));
		// globalgamedefine.h:232
		memset(name9, 0, sizeof(name9));
	}

	unsigned long uid;      // 0x0, sFriend::uid
	unsigned long value4;   // 0x4, copied to sFriend::value2D (meaning not known)
	unsigned char sex;      // 0x8, bit 0 copied to sFriend::flags bit 0 (female icon)
	char name9[0x16];       // 0x9, cleared (meaning not known)
	char nick[0x16];        // 0x1F, sFriend::nick
	unsigned char value35;  // 0x35, cleared (meaning not known)
	unsigned char online;   // 0x36, bit 0 copied to sFriend::flags bit 1 (logged on)
};
#pragma pack(pop)
CHECK_SIZE(GUILD_USER_MSN_LIST, 0x37);
CHECK_FIELD(GUILD_USER_MSN_LIST, value4, 0x4);
CHECK_FIELD(GUILD_USER_MSN_LIST, sex, 0x8);
CHECK_FIELD(GUILD_USER_MSN_LIST, name9, 0x9);
CHECK_FIELD(GUILD_USER_MSN_LIST, nick, 0x1F);
CHECK_FIELD(GUILD_USER_MSN_LIST, value35, 0x35);
CHECK_FIELD(GUILD_USER_MSN_LIST, online, 0x36);

// A treasure-hunter gauge entry of packet 0x129 (5 bytes, packed: CTask
// decodes 5 bytes into it and hands both members to CTHunter::SetTHunterGauge).
#pragma pack(push, 1)
struct sTreasureItem {
	// globalgamedefine.h:544
	sTreasureItem(void) : type(0), gauge(0) {}

	unsigned char type;   // 0x0, first argument of CTHunter::SetTHunterGauge
	unsigned long gauge;  // 0x1, second argument of CTHunter::SetTHunterGauge
};
#pragma pack(pop)
CHECK_SIZE(sTreasureItem, 5);
CHECK_FIELD(sTreasureItem, gauge, 0x1);

// A character the player owns (CSharedDoc keeps them in a
// std::map<unsigned int, sCharacterInfo>; 0x1BC bytes). The constructor
// (globalgamedefine.h:1575) clears it member by member, which gives the
// member boundaries: the 4-byte store at 0x6A makes the struct byte-packed. The members at 0x6A, 0x70 and 0x84 are structures of their
// own: CFamilyMain::cFamily's constructor zeroes each through its own address
// (lea + stores), and without a class-type member cl zeroes the whole
// structure with one rep stosd. CPartTidList::SetTids reads the character,
// colours and parts.
#pragma pack(push, 1)
struct sCharacterInfo {
	// globalgamedefine.h:1575
	sCharacterInfo(void) {
		typeId = 0;
		id = 0;
		hairColor = 0;
		shirtColor = 0;
		unknown6F = 0;
		memset(partTypeIds, 0, sizeof(partTypeIds));
		memset(&unknown6A, 0, sizeof(unknown6A));
		memset(&auxParts, 0, sizeof(auxParts));
		memset(partIds, 0, sizeof(partIds));
		memset(&unknown84, 0, sizeof(unknown84));
	}

	unsigned long typeId;               // 0x00, the sChar typeid
	unsigned long id;                   // 0x04
	unsigned long partTypeIds[24];      // 0x08, the sPart typeid worn in each slot
	unsigned char hairColor;            // 0x68
	unsigned char shirtColor;           // 0x69, low nibble: the shirt colour
	struct {
		unsigned char level[5];         // +0x0, the upgrade level of each stat (CRealMyRoomTask::OnPacket 0xA3 adds 1 or -1 at the stat's index)
	} unknown6A;                        // 0x6A, 5 bytes
	unsigned char unknown6F;            // 0x6F
	struct {
		unsigned long tids[5];          // +0x0
	} auxParts;                         // 0x70, five ulong slots (the aux parts, by their count; meaning unconfirmed)
	struct {
		UNKNOWN(0x00, 0xD8);
	} unknown84;                        // 0x84, 0x36 dwords zeroed as one block
	unsigned long partIds[24];          // 0x15C, the item id worn in each slot
};
#pragma pack(pop)
CHECK_SIZE(sCharacterInfo, 0x1BC);
CHECK_FIELD(sCharacterInfo, typeId, 0x00);
CHECK_FIELD(sCharacterInfo, id, 0x04);
CHECK_FIELD(sCharacterInfo, partTypeIds, 0x08);
CHECK_FIELD(sCharacterInfo, hairColor, 0x68);
CHECK_FIELD(sCharacterInfo, shirtColor, 0x69);
CHECK_FIELD(sCharacterInfo, unknown6A, 0x6A);
CHECK_FIELD(sCharacterInfo, unknown6F, 0x6F);
CHECK_FIELD(sCharacterInfo, auxParts, 0x70);
CHECK_FIELD(sCharacterInfo, unknown84, 0x84);
CHECK_FIELD(sCharacterInfo, partIds, 0x15C);

// The items a player has equipped (0x6C bytes, byte-packed:
// CSharedDoc::SetCurEquipInfo copies it with 0x1B dwords; CFamilyMain::cFamily
// keeps one at 0x42D).
#pragma pack(push, 1)
struct sUserEquip {
	unsigned long caddieId;             // 0x00, 0 or -1: none (SetCurEquipInfo looks it up in the caddie map)
	unsigned long characterId;          // 0x04, key of CSharedDoc::m_characterMap (CEquipHelper::GetItemCommon)
	unsigned long clubSetId;            // 0x08, key of CSharedDoc::m_myItemMap (CEquipHelper::GetItemCommon)
	unsigned long ballTypeId;           // 0x0C, the ball's item type id (CEquipHelper::GetItemCommon)
	unsigned long itemSlots[10];        // 0x10, the equipped item slots (FrEquipDlg::GetMyItemSlot)
	UNKNOWN(0x38, 0x68);
	unsigned long mascotId;             // 0x68, 0 or -1: none (SetCurEquipInfo looks it up in the mascot map)
};
#pragma pack(pop)
CHECK_SIZE(sUserEquip, 0x6C);
CHECK_FIELD(sUserEquip, caddieId, 0x00);
CHECK_FIELD(sUserEquip, characterId, 0x04);
CHECK_FIELD(sUserEquip, clubSetId, 0x08);
CHECK_FIELD(sUserEquip, ballTypeId, 0x0C);
CHECK_FIELD(sUserEquip, itemSlots, 0x10);
CHECK_FIELD(sUserEquip, mascotId, 0x68);

// One game of a player's match history (0x34 bytes; constructor
// globalgamedefine.h:1731). CMatchHistory keeps five (0x41 dwords).
// FrUserInfoForm::MatchHistoryUserBtnUp sends uid and opens the nickname.
struct sUserMatchHistory {
	// globalgamedefine.h:1731
	sUserMatchHistory(void) : sex(0), uid(0) {
		memset(nickname, 0, sizeof(nickname));
		memset(id, 0, sizeof(id));
	}

	unsigned long sex;            // 0x00 (name from the record's packet layout; no transcribed reader)
	char nickname[0x16];          // 0x04, given to CUserInfo::m_nick
	char id[0x16];                // 0x1A, the account id (same pair of arrays as sPangYaUserInfo)
	unsigned long uid;            // 0x30, sent by MatchHistoryUserBtnUp
};
CHECK_SIZE(sUserMatchHistory, 0x34);
CHECK_FIELD(sUserMatchHistory, sex, 0x00);
CHECK_FIELD(sUserMatchHistory, nickname, 0x04);
CHECK_FIELD(sUserMatchHistory, id, 0x1A);
CHECK_FIELD(sUserMatchHistory, uid, 0x30);

// Who may visit a My Room (0x6B bytes, byte-packed): the block 0x0123
// carries after its result (CTask::OnPacketCommon decodes it whole, then
// copies it member by member into CSharedDoc 0x47D8) and 0x00B0 sends.
// Name and members from externs/pangea (Structs.g.cs); not in the PDB.
#pragma pack(push, 1)
struct sRealMyRoomAuthority {
	unsigned long ownerUID;       // 0x00, the room owner's uid
	unsigned short reserved;      // 0x04, copied and sent back; no reader found
	bool bPrivate;                // 0x06, a visitor needs the password
	char password[100];           // 0x07, NUL-terminated
};
#pragma pack(pop)
CHECK_SIZE(sRealMyRoomAuthority, 0x6B);
CHECK_FIELD(sRealMyRoomAuthority, ownerUID, 0x00);
CHECK_FIELD(sRealMyRoomAuthority, reserved, 0x04);
CHECK_FIELD(sRealMyRoomAuthority, bPrivate, 0x06);
CHECK_FIELD(sRealMyRoomAuthority, password, 0x07);

// A player's game statistics (0xEB bytes, byte-packed). sUserInfo holds one
// at 0x10F and CSharedDoc the local player's at 0x4EF (flattened there as
// m_myStat*, whose names these follow). IsNothingRecord/IsHideDisconnectRate
// read the level and the games played.
#pragma pack(push, 1)
struct sPangYaUserStatistics {
	unsigned long shots;            // 0x00
	unsigned long puttAttempts;     // 0x04
	unsigned long totalPlayTime;    // 0x08, in seconds, printed as days, hours and minutes (FrUserInfoForm::SetContent)
	unsigned long playTime;         // 0x0C, in seconds (inferred: CGolfRule::SendHoleStat stores the player's milliseconds / 1000)
	float bestDrive;                // 0x10, the longest shot (inferred: CGolfCaddie::TalkProcess compares it, above 960.0f, with the last shot's length)
	unsigned long pangyas;          // 0x14
	unsigned long timeouts;         // 0x18, the shots the clock ran out on (CGolfRule::ChangeGameMode(0x4000) counts the local player's)
	unsigned long obCount;          // 0x1C, out-of-bounds shots; the OB rate is obCount / (shots + puttAttempts) (FrUserInfoForm::UpdateEditSpecialInfo)
	unsigned long totalDistance;    // 0x20, printed "%dy" as the "total distance" of the room types 4-6, 9, 10 (CMiniScore::Display)
	unsigned long drives;           // 0x24
	unsigned long par3Drives;       // 0x28
	unsigned short holeInOnes;      // 0x2C
	unsigned short bunkerShots;     // 0x2E, shots that stopped on ground type 3 (inferred: the bunker; CGolfRule::UpdateShotResultStat)
	unsigned long fairways;         // 0x30
	unsigned short albatrosses;     // 0x34, printed "%d" into the albatross edit (FrUserInfoForm::SetContent)
	unsigned long holedShots;       // 0x36, shots that went in the cup, putts or not (CGolfRule::UpdateShotResultStat)
	unsigned long puttsSunk;        // 0x3A
	float longestPutt;              // 0x3E, printed "%.2fy" as the "longest putt" (CMiniScore::Display)
	float longestChipIn;            // 0x42, printed "%.2fy" as the "longest chip-in" (CMiniScore::Display)
	unsigned long exp;              // 0x46
	unsigned char level;            // 0x4A
	__int64 pang;                   // 0x4B
	int scoreToPar;                 // 0x53, signed; the average score is 72 + scoreToPar * 18 / (drives - par3Drives) (FrUserInfoForm::UpdateEditSpecialInfo)
	signed char bestScore[6];       // 0x57, per course; 0x7F: none (FrUserInfoForm::GetBestScoreColor)
	__int64 maxPang[6];             // 0x5D, per course (FrUserInfoForm::GetMaxPangColor)
	unsigned long gamesPlayed;      // 0x8D
	UNKNOWN(0x91, 0x95);
	unsigned long rateCount;        // 0x95, shown as rateCount * 100 / rateTotal, "%.1f %" (FrUserInfoForm::UpdateEditSpecialInfo)
	unsigned long rateTotal;        // 0x99, the rate's denominator; no record shown when 0
	unsigned long ladderPoint;      // 0x9D, the ladder channel's points (sUserInfo::m_ladderPoint)
	UNKNOWN(0xA1, 0xB1);
	unsigned long combos;           // 0xB1, shown as "combos / totalCombos" in the combo edit (FrUserInfoForm::UpdateEditExpAndMannerInfo)
	unsigned long totalCombos;      // 0xB5
	unsigned long noMannerGames;    // 0xB9
	__int64 totalSkinsPang;         // 0xBD, printed "%I64d" (FrUserInfoForm::UpdateEditSpecialInfo)
	UNKNOWN(0xC5, 0xD7);
	unsigned long holesPlayed;      // 0xD7 (CSharedDoc::m_myStatHolesPlayed)
	unsigned long recordCount;      // 0xDB (CSharedDoc::m_myStatRecordCount; CLobbyMain::OpenNewRecordForm)
	UNKNOWN(0xDF, 0xEB);
};
#pragma pack(pop)
CHECK_SIZE(sPangYaUserStatistics, 0xEB);
CHECK_FIELD(sPangYaUserStatistics, shots, 0x00);
CHECK_FIELD(sPangYaUserStatistics, puttAttempts, 0x04);
CHECK_FIELD(sPangYaUserStatistics, totalPlayTime, 0x08);
CHECK_FIELD(sPangYaUserStatistics, playTime, 0x0C);
CHECK_FIELD(sPangYaUserStatistics, bestDrive, 0x10);
CHECK_FIELD(sPangYaUserStatistics, pangyas, 0x14);
CHECK_FIELD(sPangYaUserStatistics, timeouts, 0x18);
CHECK_FIELD(sPangYaUserStatistics, obCount, 0x1C);
CHECK_FIELD(sPangYaUserStatistics, totalDistance, 0x20);
CHECK_FIELD(sPangYaUserStatistics, drives, 0x24);
CHECK_FIELD(sPangYaUserStatistics, par3Drives, 0x28);
CHECK_FIELD(sPangYaUserStatistics, holeInOnes, 0x2C);
CHECK_FIELD(sPangYaUserStatistics, bunkerShots, 0x2E);
CHECK_FIELD(sPangYaUserStatistics, fairways, 0x30);
CHECK_FIELD(sPangYaUserStatistics, albatrosses, 0x34);
CHECK_FIELD(sPangYaUserStatistics, holedShots, 0x36);
CHECK_FIELD(sPangYaUserStatistics, puttsSunk, 0x3A);
CHECK_FIELD(sPangYaUserStatistics, longestPutt, 0x3E);
CHECK_FIELD(sPangYaUserStatistics, longestChipIn, 0x42);
CHECK_FIELD(sPangYaUserStatistics, exp, 0x46);
CHECK_FIELD(sPangYaUserStatistics, level, 0x4A);
CHECK_FIELD(sPangYaUserStatistics, pang, 0x4B);
CHECK_FIELD(sPangYaUserStatistics, scoreToPar, 0x53);
CHECK_FIELD(sPangYaUserStatistics, bestScore, 0x57);
CHECK_FIELD(sPangYaUserStatistics, maxPang, 0x5D);
CHECK_FIELD(sPangYaUserStatistics, gamesPlayed, 0x8D);
CHECK_FIELD(sPangYaUserStatistics, rateCount, 0x95);
CHECK_FIELD(sPangYaUserStatistics, rateTotal, 0x99);
CHECK_FIELD(sPangYaUserStatistics, ladderPoint, 0x9D);
CHECK_FIELD(sPangYaUserStatistics, combos, 0xB1);
CHECK_FIELD(sPangYaUserStatistics, totalCombos, 0xB5);
CHECK_FIELD(sPangYaUserStatistics, noMannerGames, 0xB9);
CHECK_FIELD(sPangYaUserStatistics, totalSkinsPang, 0xBD);
CHECK_FIELD(sPangYaUserStatistics, holesPlayed, 0xD7);
CHECK_FIELD(sPangYaUserStatistics, recordCount, 0xDB);

// An element of a player's special trophy list (0xC bytes: the list node
// copies three dwords). FrUserInfoForm::OnListBoxOwnerDraw_TRI_SPECIALBOX
// draws the trophy's icon and count.
struct sSpecialTrophy {
	unsigned long m_id;             // 0x0, decoded from the packet, not read by the client
	unsigned long m_typeId;         // 0x4, the trophy item's type id
	unsigned long m_count;          // 0x8
};
CHECK_SIZE(sSpecialTrophy, 0xC);
CHECK_FIELD(sSpecialTrophy, m_id, 0x0);
CHECK_FIELD(sSpecialTrophy, m_typeId, 0x4);
CHECK_FIELD(sSpecialTrophy, m_count, 0x8);

// An element of a player's guild trophy list (4 bytes: the list node copies
// one dword). FrUserInfoForm::OnGuildTrophyListOwnerDraw draws its icon.
struct sGuildTrophy {
	unsigned long m_typeId;         // 0x0, the trophy item's type id
};
CHECK_SIZE(sGuildTrophy, 0x4);
CHECK_FIELD(sGuildTrophy, m_typeId, 0x0);

// A lobby player as the user list holds it (0xC4 bytes, byte-packed: the
// map node of CSharedDoc's user map copies 0x32 dwords of key and value).
#pragma pack(push, 1)
struct sBriefUserInfo {
	// globalgamedefine.h:1786
	bool IsIdentity(unsigned long identityFlag) {
		return (this->identity & identityFlag) ? true : false;
	}

	unsigned long id;             // 0x00, FrUserListDlg::m_selectedID
	unsigned long uid;            // 0x04, FrUserListDlg::m_selectedUID
	unsigned short roomNumber;    // 0x08, 0xFFFF: not in a room (invite enabled)
	char nick[0x16];              // 0x0A
	unsigned char level;          // 0x20
	union {
		unsigned long identity;   // 0x21, flags (IsIdentity; 2: gallery, 4: administrator)
		struct {
			unsigned long : 1;
			unsigned long gallery : 1;        // bit 1
			unsigned long administrator : 1;  // bit 2 (ntAvatarManager::AvatarByRayIntersect)
			unsigned long : 29;
		} identityBits;           // 0x21, the same flags as bits
	};
	unsigned long titleTypeId;    // 0x25, the equipped title's type id (CTask::OnPacketCommon 0x44 copies it to sSlotInfo)
	unsigned long rankValue;      // 0x29, sorted instead of the level in channels with flag 0x80
	unsigned char unknownBit : 1; // 0x2D, bit 0
	unsigned char charType : 3;   // 0x2D, bits 1-3; even male, odd female (the sex sort)
	unsigned char manner : 1;     // 0x2D, bit 4: the manner badge on the lobby user list
	unsigned char angel : 1;      // 0x2D, bit 5: the angel badge on the lobby user list (drawn before manner)
	unsigned long guildUID;       // 0x2E, the guild sort key
	char guildMark[0xC];          // 0x32, the guild emblem (CGuildActor::ChangeGuildEmblem)
	unsigned short invisible : 1;   // 0x3E, bit 0: an administrator hidden from picking (AvatarByRayIntersect skips them)
	unsigned short : 9;
	unsigned short stayInLobby : 1; // 0x3E, bit 10: a hidden administrator stays in the lobby's user map (CTask::OnPacketCommon 0x44)
	unsigned short : 5;
	unsigned long nickColorType;  // 0x40, 1: the nick is drawn in 0xFF33DBFF instead of white (OnRoomList_UserListOwnerDraw)
	PADDING(0x44, 0xC4);
};
#pragma pack(pop)
CHECK_SIZE(sBriefUserInfo, 0xC4);
CHECK_FIELD(sBriefUserInfo, id, 0x00);
CHECK_FIELD(sBriefUserInfo, uid, 0x04);
CHECK_FIELD(sBriefUserInfo, roomNumber, 0x08);
CHECK_FIELD(sBriefUserInfo, nick, 0x0A);
CHECK_FIELD(sBriefUserInfo, guildMark, 0x32);
CHECK_FIELD(sBriefUserInfo, level, 0x20);
CHECK_FIELD(sBriefUserInfo, identity, 0x21);
CHECK_FIELD(sBriefUserInfo, identityBits, 0x21);
CHECK_FIELD(sBriefUserInfo, rankValue, 0x29);
CHECK_FIELD(sBriefUserInfo, guildUID, 0x2E);
CHECK_FIELD(sBriefUserInfo, titleTypeId, 0x25);
CHECK_FIELD(sBriefUserInfo, nickColorType, 0x40);

// The game style of a room; its enumerators are not transcribed yet.
PG_ENUM_FWD(eGameStyle)

// A room's game class: its style and a byte value (8 bytes; the constructor
// stores a dword at 0x0 and a byte at 0x4). Its out-of-line constructor is
// the stdafx unit's.
struct stGameClass {
	// globalgamedefine.h:663
	stGameClass(eGameStyle style, unsigned char value) {
		m_style = style; m_value = value;
	}

	eGameStyle m_style;     // 0x0
	unsigned char m_value;  // 0x4, name provisional
};
CHECK_SIZE(stGameClass, 0x8);
CHECK_FIELD(stGameClass, m_style, 0x0);
CHECK_FIELD(stGameClass, m_value, 0x4);

// The two guilds of a guild-match room (0x4A bytes, byte-packed).
#pragma pack(push, 1)
struct sGuildRoomInfo {
	// globalgamedefine.h:3058 (the out-of-line copy is SharedDoc's)
	void Reset(void) {
		memset(m_guildUID, 0, sizeof(m_guildUID));
		memset(m_guildName, 0, sizeof(m_guildName));
		memset(m_guildMark, 0, sizeof(m_guildMark));
	}

	// globalgamedefine.h:3072 (the out-of-line copy is LobbyMain's)
	void Reset(int team) {
		if (team < 2) {
			m_guildUID[team] = 0; m_guildName[team][0] = 0;
			m_guildMark[team][0] = 0;
		}
	}

	unsigned long m_guildUID[2];  // 0x00
	char m_guildName[2][0x15];    // 0x08
	char m_guildMark[2][0xC];     // 0x32
};
#pragma pack(pop)
CHECK_SIZE(sGuildRoomInfo, 0x4A);
CHECK_FIELD(sGuildRoomInfo, m_guildUID, 0x00);
CHECK_FIELD(sGuildRoomInfo, m_guildName, 0x08);
CHECK_FIELD(sGuildRoomInfo, m_guildMark, 0x32);

struct EncryptKey {
public:
	// globalgamedefine.h:1116
	EncryptKey(void) {
		memset(m_key, 0, sizeof(m_key));
		for (int i = 0; i < 16; i++)
			m_key[i] = (unsigned char)(rand() % 255);
	}
	~EncryptKey(void) {}               // globalgamedefine.h:1122

	unsigned char GetKey(int index);   // globalgamedefine.h:1132

private:
	unsigned char m_key[16]; // 0x0
};

CHECK_SIZE(EncryptKey, 0x10);

// Packed: m_roomNumber sits at the odd offset 0x49. 0xB2 bytes
// (FrDtRoomDlg::SetRoomInfo copies 0x2C dwords + 1 word). The destructor is
// implicit: the original's 1-byte ??1sRoomInfo has no line info.
#pragma pack(push, 1)
struct sRoomInfo {
	// globalgamedefine.h:3163
	sRoomInfo(void) {
		m_title[0] = 0;
		m_password[0] = 0;
		m_intrusionAccess = 0;
		m_trophy = 0;
		m_option[0] = 0;
		m_option[1] = 0;
		m_roomMode = 0;
		m_roomNumber = 0xFFFF;
		m_isPublic = 1;
		m_guild.Reset();
	}

	// globalgamedefine.h:3186 (std::find on the room list by room number)
	bool operator==(const unsigned long &roomNumber) const {
		return m_roomNumber == roomNumber;
	}

	char m_title[0x20];              // 0x00 (FrChangeRoomInfoDlg::OnTitleInit shows it)
	char m_password[0x10];           // 0x20 (FrChangeRoomInfoDlg::OnPasswordInit shows it)
	unsigned char m_isPublic;        // 0x30 (0: FrDtRoomDlg::OnJoinBtnUp asks for the password)
	unsigned char m_intrusionAccess; // 0x31 (set once CIntrusion::OnAccessRoom lets a player join the game in progress)
	bool m_state;                    // 0x32 (true = game in progress, returned by CIntrusion::IsIntrusionGame)
	unsigned char m_userLimit;       // 0x33 (100 for a lounge-sized room: the lobby's room list sorts those first)
	unsigned char m_userCount;       // 0x34 (second key of RoomUserCompare)
	EncryptKey m_key;                // 0x35
	unsigned char m_galleryCount;    // 0x45 (spectators in the room; FrDtRoomDlg::OnGalleryBtnUp refuses a join when it reaches m_galleryLimit)
	unsigned char m_galleryLimit;    // 0x46 (CLobbyTask::OnPacket 0x48 updates it; the spectator capacity, FrDtRoomDlg::OnGalleryBtnUp)
	unsigned char m_holeCount;       // 0x47 (RoomHoleCompare)
	unsigned char m_gameType;        // 0x48 (RoomGameTypeCompare)
	unsigned short m_roomNumber;     // 0x49 (matched by CIntrusion::IsIntrusionGame)
	unsigned char m_value4B;         // 0x4B (CLobbyTask::OnPacket 0x48 updates it and sends it to the lobby with message 0x12)
	unsigned char m_course;          // 0x4C (RoomCourseCompare)
	unsigned long m_value4D;         // 0x4D (CLobbyTask::OnPacket 0x48; one of the two time limits of the packet layout)
	unsigned long m_value51;         // 0x51 (likewise, the other time limit)
	unsigned long m_trophy;          // 0x55 (cleared by the constructor; name from the packet layout: mode, course, two time limits, trophy)
	unsigned char m_option[2];       // 0x59 (two bytes the constructor clears; meaning not yet known)
	sGuildRoomInfo m_guild;          // 0x5B (the two guilds of a guild match; FrDtRoomDlg::OnJoinBtnUp checks the player's guild against them)
	PADDING(0xA5, 0xAD);            // untranscribed room fields
	unsigned long m_masterUID;       // 0xAD (compared with CSharedDoc::m_myUID by CLobbyTask::OnPacket 0x47: the local player owns the room)
	unsigned char m_roomMode;        // 0xB1 (the constructor clears it; 14 shows the blank score panels)
};
#pragma pack(pop)

CHECK_SIZE(sRoomInfo, 0xB2);
CHECK_FIELD(sRoomInfo, m_title, 0x00);
CHECK_FIELD(sRoomInfo, m_password, 0x20);
CHECK_FIELD(sRoomInfo, m_isPublic, 0x30);
CHECK_FIELD(sRoomInfo, m_intrusionAccess, 0x31);
CHECK_FIELD(sRoomInfo, m_state, 0x32);
CHECK_FIELD(sRoomInfo, m_userLimit, 0x33);
CHECK_FIELD(sRoomInfo, m_userCount, 0x34);
CHECK_FIELD(sRoomInfo, m_key, 0x35);
CHECK_FIELD(sRoomInfo, m_holeCount, 0x47);
CHECK_FIELD(sRoomInfo, m_gameType, 0x48);
CHECK_FIELD(sRoomInfo, m_roomNumber, 0x49);
CHECK_FIELD(sRoomInfo, m_course, 0x4C);
CHECK_FIELD(sRoomInfo, m_trophy, 0x55);
CHECK_FIELD(sRoomInfo, m_option, 0x59);
CHECK_FIELD(sRoomInfo, m_guild, 0x5B);
CHECK_FIELD(sRoomInfo, m_masterUID, 0xAD);
CHECK_FIELD(sRoomInfo, m_galleryCount, 0x45);
CHECK_FIELD(sRoomInfo, m_galleryLimit, 0x46);
CHECK_FIELD(sRoomInfo, m_value4B, 0x4B);
CHECK_FIELD(sRoomInfo, m_value4D, 0x4D);
CHECK_FIELD(sRoomInfo, m_value51, 0x51);
CHECK_FIELD(sRoomInfo, m_roomMode, 0xB1);

// An item attached to a mail (0x35 bytes, byte-packed: the list node's
// copy moves 0xD dwords + 1 byte; constructor globalgamedefine.h:4416).
// Filled from a sTradeItem by CPost_SendDlg::GetSendList.
#pragma pack(push, 1)
struct sMailIncludeItem {
	// globalgamedefine.h:4416 (the out-of-line copy is RealMyRoomMainUI's)
	sMailIncludeItem(void) {
		guid = (unsigned long)-1;
		typeId = (unsigned long)-1;
		count = 0;
		flag = 0;
		stats[4] = 0;
		stats[3] = 0;
		stats[2] = 0;
		stats[1] = 0;
		stats[0] = 0;
		charCardSlots = 0;
		caddieCardSlots = 0;
		extraId = (unsigned long)-1; // stored with guid/typeId: the -1 register stays live past the zeros
		extraValue = 0;
		memset(uccKey, 0, sizeof(uccKey));
		uccValue = 0;
		uccStatus = 0;
	}

	unsigned long guid;           // 0x00
	unsigned long typeId;         // 0x04
	unsigned char kind;           // 0x08
	unsigned long count;          // 0x09
	unsigned long flag;           // 0x0D, sTradeItem::flag
	short stats[5];               // 0x11, the item's five stat values (cleared by the constructor)
	short charCardSlots;          // 0x1B, the part's sPart::m_charCardSlots (CPost_ReciveDlg::SetLoadPostList)
	short caddieCardSlots;        // 0x1D, the part's sPart::m_caddieCardSlots
	unsigned long extraId;        // 0x1F, -1 (none) by the constructor; name provisional
	unsigned long extraValue;     // 0x23, name provisional
	char uccKey[9];               // 0x27, the UCC's key (RequestUccInfo)
	unsigned long uccValue;       // 0x30, name provisional
	unsigned char uccStatus;      // 0x34, name provisional
};
#pragma pack(pop)
CHECK_SIZE(sMailIncludeItem, 0x35);
CHECK_FIELD(sMailIncludeItem, guid, 0x00);
CHECK_FIELD(sMailIncludeItem, typeId, 0x04);
CHECK_FIELD(sMailIncludeItem, kind, 0x08);
CHECK_FIELD(sMailIncludeItem, count, 0x09);
CHECK_FIELD(sMailIncludeItem, flag, 0x0D);
CHECK_FIELD(sMailIncludeItem, charCardSlots, 0x1B);
CHECK_FIELD(sMailIncludeItem, caddieCardSlots, 0x1D);
CHECK_FIELD(sMailIncludeItem, stats, 0x11);
CHECK_FIELD(sMailIncludeItem, extraId, 0x1F);
CHECK_FIELD(sMailIncludeItem, extraValue, 0x23);
CHECK_FIELD(sMailIncludeItem, uccKey, 0x27);
CHECK_FIELD(sMailIncludeItem, uccValue, 0x30);
CHECK_FIELD(sMailIncludeItem, uccStatus, 0x34);

// One line of the mailbox list (0xC4 bytes, byte-packed: an int sits at
// 0x89; the std::list node holds 0xCC).
#pragma pack(push, 1)
struct sMailInfoBrief {
	unsigned long m_mailIndex;   // 0x00, the letter's index sent to the server (delete_letter_check)
	char m_sender[0x16];         // 0x04, the sender's nickname (FrGiftAlarmDlg prints it; size as sMailInfo::senderNick)
	UNKNOWN(0x1A, 0x88);
	bool m_read;                 // 0x88, set once the letter is opened (CPost_ReciveDlg::OnPostMessageResult)
	int m_hasItem;               // 0x89, nonzero: items are attached (delete-all skips the letter; name inferred)
	sMailIncludeItem m_item;     // 0x8D, the first attached item (the implicit constructor runs sMailIncludeItem's: -1 at 0x8D/0x91/0xAC)
	UNKNOWN(0xC2, 0xC4);
};
#pragma pack(pop)
CHECK_SIZE(sMailInfoBrief, 0xC4);
CHECK_FIELD(sMailInfoBrief, m_mailIndex, 0x00);
CHECK_FIELD(sMailInfoBrief, m_read, 0x88);
CHECK_FIELD(sMailInfoBrief, m_sender, 0x04);
CHECK_FIELD(sMailInfoBrief, m_hasItem, 0x89);
CHECK_FIELD(sMailInfoBrief, m_item, 0x8D);

// One page of the mailbox (CTransferableItem::SetMailInfoPage). The three
// counters are copied as they are; their names are inferred.
struct sMailBoxListForPage {
	std::list<sMailInfoBrief> mailList;   // 0x00
	int page;                             // 0x0C
	int totalPage;                        // 0x10
	int totalMail;                        // 0x14
};
CHECK_SIZE(sMailBoxListForPage, 0x18);
CHECK_FIELD(sMailBoxListForPage, mailList, 0x00);
CHECK_FIELD(sMailBoxListForPage, page, 0x0C);
CHECK_FIELD(sMailBoxListForPage, totalPage, 0x10);
CHECK_FIELD(sMailBoxListForPage, totalMail, 0x14);

// A mail opened from the mailbox (0x368 bytes, byte-packed: the item list
// sits at 0x35B). The constructor (globalgamedefine.h:4443) is emitted by
// realmyroomtask.cpp; the member names are inferred from the sizes.
#pragma pack(push, 1)
struct sMailInfo {
	// globalgamedefine.h:4443
	sMailInfo(void) {
		mailIndex = -1;
		memset(senderNick, 0, sizeof(senderNick));
		memset(date, 0, sizeof(date));
		memset(message, 0, sizeof(message));
		isRead = 0;
		itemList.clear();
	}

	int mailIndex;                          // 0x000, -1 when empty
	char senderNick[0x16];                  // 0x004
	char date[0x20];                        // 0x01A
	char message[0x320];                    // 0x03A
	unsigned char isRead;                   // 0x35A
	std::list<sMailIncludeItem> itemList;   // 0x35B
	unsigned char state;                    // 0x367, not cleared by the constructor
};
#pragma pack(pop)
CHECK_SIZE(sMailInfo, 0x368);
CHECK_FIELD(sMailInfo, mailIndex, 0x000);
CHECK_FIELD(sMailInfo, senderNick, 0x004);
CHECK_FIELD(sMailInfo, date, 0x01A);
CHECK_FIELD(sMailInfo, message, 0x03A);
CHECK_FIELD(sMailInfo, isRead, 0x35A);
CHECK_FIELD(sMailInfo, itemList, 0x35B);
CHECK_FIELD(sMailInfo, state, 0x367);

// A player's public profile, byte-packed, 0x10D bytes (CSharedDoc keeps the
// local player's at 0x3E2, GetPangyaUserInfo; CSharedDoc::SetIndex copies it
// whole into sUserInfo). Only the members the transcribed functions read are
// named.
#pragma pack(push, 1)
struct sPangYaUserInfo {
	// globalgamedefine.h:1354 (inline; emitted by taskmain.cpp)
	bool IsIdentity(unsigned long identityFlag) {
		return (this->identity & identityFlag) ? true : false;
	}

	char id[0x16];                // 0x00
	char nickname[0x16];          // 0x16
	char guildName[0x15];         // 0x2C
	char guildMark[0xC];          // 0x41
	unsigned long schoolNo;       // 0x4D, the player's school (FrOptionDlg::SetGameColtrols, CSharedDoc::LoadSchoolName)
	union {
		unsigned long identity;   // 0x51, flags (2: gallery, 4: administrator)
		struct {
			unsigned long : 1;
			unsigned long gallery : 1;        // bit 1 (CClock::HandleMsg)
			unsigned long administrator : 1;  // bit 2 (CGMToolkit::IsAdministrator)
			unsigned long : 29;
		} identityBits;           // 0x51, the same flags as bits
	};
	unsigned long galleryGuid;    // 0x55, MyGuid in gallery mode
	unsigned long guid;           // 0x59, MyGuid
	UNKNOWN(0x5D, 0x69);
	unsigned long guildUID;       // 0x69
	unsigned long guildPosition;  // 0x6D
	union {
		unsigned char eventFlags; // 0x71, bit 1 (mask 2): the recycle event is on (FrRecycleDlg::SetEventMessage)
		struct {
			unsigned char eventFlag0 : 1;
			unsigned char gender : 1;      // bit 1
			unsigned char eventFlag2 : 1;
			unsigned char eventFlag3 : 1;  // bit 3 (CTask::OnPacketCommon, CIntrusion::DoIntrusionAccess)
			unsigned char eventFlag4 : 1;  // bit 4
			unsigned char eventFlag5 : 1;  // bit 5
			unsigned char : 2;
		} eventBits;              // 0x71, the same flags as bits
	};
	UNKNOWN(0x72, 0x73);
	short bsUsableTimes;          // 0x73, Bongdari shop: plays usable (CSharedDoc::GetBsUsableTimes)
	short bsUsableBonusTimes;     // 0x75, Bongdari shop: bonus plays usable (CSharedDoc::GetBsUsableBonusTimes)
	short bsRemainedBonusTimes;   // 0x77, Bongdari shop: bonus plays remaining (CSharedDoc::GetBsRemainedBonusTimes)
	unsigned long pointEventPoints; // 0x79, the point event's points (FrPointEventDlg::OnProc)
	unsigned long flagBlock;      // 0x7D
	int timeBlock;                // 0x81, CSharedDoc::SetTimeBlock
	int nameByEdit;               // 0x85, 1: mail is addressed by the name typed in the receiver edit
	char loginId[0x16];           // 0x89, the account's login id (CSharedDoc::m_loginId)
	UNKNOWN(0x9F, 0x109);
	unsigned long uid;            // 0x109, the player's UID (CSharedDoc::m_myUID, sUserInfo::m_uid)
};
#pragma pack(pop)
CHECK_SIZE(sPangYaUserInfo, 0x10D);
CHECK_FIELD(sPangYaUserInfo, id, 0x00);
CHECK_FIELD(sPangYaUserInfo, nickname, 0x16);
CHECK_FIELD(sPangYaUserInfo, guildName, 0x2C);
CHECK_FIELD(sPangYaUserInfo, guildMark, 0x41);
CHECK_FIELD(sPangYaUserInfo, schoolNo, 0x4D);
CHECK_FIELD(sPangYaUserInfo, identity, 0x51);
CHECK_FIELD(sPangYaUserInfo, identityBits, 0x51);
CHECK_FIELD(sPangYaUserInfo, galleryGuid, 0x55);
CHECK_FIELD(sPangYaUserInfo, guid, 0x59);
CHECK_FIELD(sPangYaUserInfo, guildUID, 0x69);
CHECK_FIELD(sPangYaUserInfo, guildPosition, 0x6D);
CHECK_FIELD(sPangYaUserInfo, eventFlags, 0x71);
CHECK_FIELD(sPangYaUserInfo, bsUsableTimes, 0x73);
CHECK_FIELD(sPangYaUserInfo, bsUsableBonusTimes, 0x75);
CHECK_FIELD(sPangYaUserInfo, bsRemainedBonusTimes, 0x77);
CHECK_FIELD(sPangYaUserInfo, pointEventPoints, 0x79);
CHECK_FIELD(sPangYaUserInfo, flagBlock, 0x7D);
CHECK_FIELD(sPangYaUserInfo, timeBlock, 0x81);
CHECK_FIELD(sPangYaUserInfo, nameByEdit, 0x85);
CHECK_FIELD(sPangYaUserInfo, loginId, 0x89);
CHECK_FIELD(sPangYaUserInfo, uid, 0x109);

// A course of the game (the map id the server sends).
enum eMapType {
	MAP_BLUE_LAGOON = 0,
	MAP_BLUE_WATER = 1,
	MAP_SEPIA_WIND = 2,
	MAP_WIND_HILL = 3,
	MAP_WIZ_WIZ = 4,
	MAP_WEST_WIZ = 5,
	MAP_BLUE_MOON = 6,
	MAP_SILVIA_CANNON = 7,
	MAP_ICE_CANNON = 8,
	MAP_WHITE_WIZ = 9,
	MAP_SHINING_SAND = 10,
	MAP_PINK_WIND = 11,
	MAP_COURSE_12 = 12,          // has a slot in GetMapIndex; name unknown
	MAP_DEEP_INFERNO = 13,
	MAP_ICE_SPA = 14,
	MAP_LOST_SEAWAY = 15,
	MAP_EASTERN_VALLEY = 16,
	MAP_ICE_INFERNO = 18,
	MAP_WIZ_CITY = 19,
#ifdef JP_PAKS
	// JP data: the JP Course.iff's later courses (docs/JP_CLIENT.md)
	MAP_CHRONICLE_1_CHAOS = 17,
	MAP_ABBOT_MINE = 20,
	MAP_MYSTIC_RUINS = 21,
	MAP_GRAND_ZODIAC = 0x40,
#endif
	MAP_RANDOM = 0x7F            // the room's "random course" id
};

// Conversions between a course id, its slot in per-course tables and its bit
// in a course mask.
class CMapTypeAdapter {
public:
	enum eMapIndex {
		MAP_INDEX_NONE = -1,
		MAP_INDEX_BLUE_LAGOON = 0,
		MAP_INDEX_BLUE_WATER = 1,
		MAP_INDEX_SEPIA_WIND = 2,
		MAP_INDEX_WIND_HILL = 3,
		MAP_INDEX_WIZ_WIZ = 4,
		MAP_INDEX_WEST_WIZ = 5,
		MAP_INDEX_BLUE_MOON = 6,
		MAP_INDEX_SILVIA_CANNON = 7,
		MAP_INDEX_ICE_CANNON = 8,
		MAP_INDEX_WHITE_WIZ = 9,
		MAP_INDEX_SHINING_SAND = 10,
		MAP_INDEX_PINK_WIND = 11,
		MAP_INDEX_COURSE_12 = 12,
		MAP_INDEX_DEEP_INFERNO = 13,
		MAP_INDEX_ICE_SPA = 14,
		MAP_INDEX_LOST_SEAWAY = 15,
		MAP_INDEX_EASTERN_VALLEY = 16,
		MAP_INDEX_WIZ_CITY = 19,
#ifdef JP_PAKS
		// JP data: slots for the JP courses with a button in the JP
		// "treasuremap" form (Ice Inferno, Abbot Mine, Mystic Ruins) and
		// for Chronicle 1 Chaos; the per-course tables they index have
		// MAP_INDEX_COUNT_JP rows in this build. Grand Zodiac (0x40) has none.
		MAP_INDEX_CHRONICLE_1_CHAOS = 17,
		MAP_INDEX_ICE_INFERNO = 18,
		MAP_INDEX_ABBOT_MINE = 20,
		MAP_INDEX_MYSTIC_RUINS = 21,
		MAP_INDEX_COUNT_JP = 22,
#endif
		MAP_INDEX_RANDOM = 0x7F
	};

	enum eMapFlag {
		MAP_FLAG_NONE = 0,
		MAP_FLAG_BLUE_LAGOON = 0x1,
		MAP_FLAG_BLUE_WATER = 0x2,
		MAP_FLAG_SEPIA_WIND = 0x4,
		MAP_FLAG_WIND_HILL = 0x8,
		MAP_FLAG_WIZ_WIZ = 0x10,
		MAP_FLAG_WEST_WIZ = 0x20,
		MAP_FLAG_BLUE_MOON = 0x40,
		MAP_FLAG_SILVIA_CANNON = 0x80,
		MAP_FLAG_ICE_CANNON = 0x100,
		MAP_FLAG_WHITE_WIZ = 0x200,
		MAP_FLAG_SHINING_SAND = 0x400,
		MAP_FLAG_PINK_WIND = 0x800,
		MAP_FLAG_DEEP_INFERNO = 0x1000,
		MAP_FLAG_ICE_SPA = 0x2000,
		MAP_FLAG_LOST_SEAWAY = 0x4000,
		MAP_FLAG_EASTERN_VALLEY = 0x8000,
		MAP_FLAG_WIZ_CITY = 0x40000
	};

	// globalgamedefine.h:4574
	static eMapFlag GetMapFlag(eMapType type) {
		eMapFlag flag;

		switch (type) {
		case MAP_BLUE_LAGOON:
			return MAP_FLAG_BLUE_LAGOON;
		case MAP_BLUE_WATER:
			return MAP_FLAG_BLUE_WATER;
		case MAP_SEPIA_WIND:
			return MAP_FLAG_SEPIA_WIND;
		case MAP_WIND_HILL:
			return MAP_FLAG_WIND_HILL;
		case MAP_WIZ_WIZ:
			return MAP_FLAG_WIZ_WIZ;
		case MAP_WEST_WIZ:
			return MAP_FLAG_WEST_WIZ;
		case MAP_BLUE_MOON:
			return MAP_FLAG_BLUE_MOON;
		case MAP_SILVIA_CANNON:
			return MAP_FLAG_SILVIA_CANNON;
		case MAP_ICE_CANNON:
			return MAP_FLAG_ICE_CANNON;
		case MAP_WHITE_WIZ:
			return MAP_FLAG_WHITE_WIZ;
		case MAP_SHINING_SAND:
			return MAP_FLAG_SHINING_SAND;
		case MAP_PINK_WIND:
			return MAP_FLAG_PINK_WIND;
		case MAP_DEEP_INFERNO:
			return MAP_FLAG_DEEP_INFERNO;
		case MAP_ICE_SPA:
			return MAP_FLAG_ICE_SPA;
		case MAP_LOST_SEAWAY:
			return MAP_FLAG_LOST_SEAWAY;
		case MAP_EASTERN_VALLEY:
			return MAP_FLAG_EASTERN_VALLEY;
		case MAP_WIZ_CITY:
			flag = MAP_FLAG_WIZ_CITY;
			break;
		}

		return flag;
	}

	// globalgamedefine.h:4627
	static eMapFlag GetMapFlag(eMapIndex index) {
		eMapFlag flag = MAP_FLAG_NONE;

		switch (index) {
		case MAP_INDEX_BLUE_LAGOON:
			return MAP_FLAG_BLUE_LAGOON;
		case MAP_INDEX_BLUE_WATER:
			return MAP_FLAG_BLUE_WATER;
		case MAP_INDEX_SEPIA_WIND:
			return MAP_FLAG_SEPIA_WIND;
		case MAP_INDEX_WIND_HILL:
			return MAP_FLAG_WIND_HILL;
		case MAP_INDEX_WIZ_WIZ:
			return MAP_FLAG_WIZ_WIZ;
		case MAP_INDEX_WEST_WIZ:
			return MAP_FLAG_WEST_WIZ;
		case MAP_INDEX_BLUE_MOON:
			return MAP_FLAG_BLUE_MOON;
		case MAP_INDEX_SILVIA_CANNON:
			return MAP_FLAG_SILVIA_CANNON;
		case MAP_INDEX_ICE_CANNON:
			return MAP_FLAG_ICE_CANNON;
		case MAP_INDEX_WHITE_WIZ:
			return MAP_FLAG_WHITE_WIZ;
		case MAP_INDEX_SHINING_SAND:
			return MAP_FLAG_SHINING_SAND;
		case MAP_INDEX_PINK_WIND:
			return MAP_FLAG_PINK_WIND;
		case MAP_INDEX_COURSE_12:
			return MAP_FLAG_NONE;
		case MAP_INDEX_DEEP_INFERNO:
			return MAP_FLAG_DEEP_INFERNO;
		case MAP_INDEX_ICE_SPA:
			return MAP_FLAG_ICE_SPA;
		case MAP_INDEX_LOST_SEAWAY:
			return MAP_FLAG_LOST_SEAWAY;
		case MAP_INDEX_EASTERN_VALLEY:
			return MAP_FLAG_EASTERN_VALLEY;
		case MAP_INDEX_WIZ_CITY:
			flag = MAP_FLAG_WIZ_CITY;
			break;
		}

		return flag;
	}

	// globalgamedefine.h:4683
	static eMapIndex GetMapIndex(eMapType type) {
		switch (type) {
		case MAP_BLUE_LAGOON:
			return MAP_INDEX_BLUE_LAGOON;
		case MAP_BLUE_WATER:
			return MAP_INDEX_BLUE_WATER;
		case MAP_SEPIA_WIND:
			return MAP_INDEX_SEPIA_WIND;
		case MAP_WIND_HILL:
			return MAP_INDEX_WIND_HILL;
		case MAP_WIZ_WIZ:
			return MAP_INDEX_WIZ_WIZ;
		case MAP_WEST_WIZ:
			return MAP_INDEX_WEST_WIZ;
		case MAP_BLUE_MOON:
			return MAP_INDEX_BLUE_MOON;
		case MAP_SILVIA_CANNON:
			return MAP_INDEX_SILVIA_CANNON;
		case MAP_ICE_CANNON:
			return MAP_INDEX_ICE_CANNON;
		case MAP_WHITE_WIZ:
			return MAP_INDEX_WHITE_WIZ;
		case MAP_SHINING_SAND:
			return MAP_INDEX_SHINING_SAND;
		case MAP_PINK_WIND:
			return MAP_INDEX_PINK_WIND;
		case MAP_COURSE_12:
			return MAP_INDEX_COURSE_12;
		case MAP_DEEP_INFERNO:
			return MAP_INDEX_DEEP_INFERNO;
		case MAP_ICE_SPA:
			return MAP_INDEX_ICE_SPA;
		case MAP_LOST_SEAWAY:
			return MAP_INDEX_LOST_SEAWAY;
		case MAP_EASTERN_VALLEY:
			return MAP_INDEX_EASTERN_VALLEY;
		case MAP_WIZ_CITY:
			return MAP_INDEX_WIZ_CITY;
#ifdef JP_PAKS
		case MAP_CHRONICLE_1_CHAOS:
			return MAP_INDEX_CHRONICLE_1_CHAOS;
		case MAP_ICE_INFERNO:
			return MAP_INDEX_ICE_INFERNO;
		case MAP_ABBOT_MINE:
			return MAP_INDEX_ABBOT_MINE;
		case MAP_MYSTIC_RUINS:
			return MAP_INDEX_MYSTIC_RUINS;
#endif
		case MAP_RANDOM:
			return MAP_INDEX_RANDOM;
		default:
			return MAP_INDEX_NONE;
		}
	}
};

#ifdef JP_PAKS
// JP data (docs/JP_CLIENT.md): the row of a course in the 2011 code's
// per-course tables of 20 rows indexed by the course id (clear bonus,
// hole-in pangs): a later JP course (Abbot Mine, Mystic Ruins, Grand Zodiac)
// takes Wiz City's instead of reading past the table.
inline unsigned char JpCourseRow(unsigned char course) {
	return course < 20 ? course : (unsigned char)MAP_WIZ_CITY;
}
#endif

// A prize of a pouch opened in My Room (0x14 bytes: CRealMyRoomTask::OnPacket
// 0x121 decodes each with DecodeBuffer(0x14) into a std::list<sPouchPrize>).
struct sPouchPrize {
	// globalgamedefine.h:4816
	sPouchPrize(void) {}

	unsigned long typeId;           // 0x0, the prize item's type id (CItemManager::FindCommonItem)
	UNKNOWN(0x4, 0x8);
	int quantity;                   // 0x8, signed; added to CSharedDoc's pang for a pang prize (typeId 0x1A000010)
	UNKNOWN(0xC, 0x14);
};
CHECK_SIZE(sPouchPrize, 0x14);
CHECK_FIELD(sPouchPrize, typeId, 0x0);
CHECK_FIELD(sPouchPrize, quantity, 0x8);

// Animation style of a cut-in overlay (values 3..7 seen: scaling 3/4,
// alpha 5/6, rotate 7; the others are not named yet).
enum eCUTINANISTLYE {
	CUTIN_ANI_STYLE_NONE = 0
};

// An approach-mode result (CGolfDoc keeps two lists of pointers to them);
// packed to 1 byte: the player's UID sits at 0x1.
#pragma pack(push, 1)
struct sApproachResultData {
	// globalgamedefine.h:4909
	sApproachResultData& operator=(sApproachResultData& rhs) {
		m_unk00 = rhs.m_unk00;
		m_uid = rhs.m_uid;
		m_unk05 = rhs.m_unk05;
		m_unk09 = rhs.m_unk09;
		m_unk0A = rhs.m_unk0A;
		m_unk0E = rhs.m_unk0E;
		m_unk12 = rhs.m_unk12;
		m_unk16 = rhs.m_unk16;
		m_unk17 = rhs.m_unk17;
		return *this;
	}

	unsigned char m_unk00;      // 0x0
	unsigned long m_uid;        // 0x1, compared with the local player's (GetAppMyCurHoleResult)
	unsigned long m_unk05;      // 0x5
	unsigned char m_unk09;      // 0x9
	unsigned long m_unk0A;      // 0xA
	unsigned long m_unk0E;      // 0xE
	unsigned long m_unk12;      // 0x12
	unsigned char m_unk16;      // 0x16
	unsigned char m_unk17;      // 0x17
};
#pragma pack(pop)
CHECK_SIZE(sApproachResultData, 0x18);
CHECK_FIELD(sApproachResultData, m_unk00, 0x0);
CHECK_FIELD(sApproachResultData, m_uid, 0x1);
CHECK_FIELD(sApproachResultData, m_unk05, 0x5);
CHECK_FIELD(sApproachResultData, m_unk09, 0x9);
CHECK_FIELD(sApproachResultData, m_unk0A, 0xA);
CHECK_FIELD(sApproachResultData, m_unk0E, 0xE);
CHECK_FIELD(sApproachResultData, m_unk12, 0x12);
CHECK_FIELD(sApproachResultData, m_unk16, 0x16);
CHECK_FIELD(sApproachResultData, m_unk17, 0x17);

// Animation parameters of one cut-in overlay, 0xB8 bytes. Only the members
// the decompiled code touches are transcribed.
struct CUTIN_ANI_STYPEINFO {
	// globalgamedefine.h:5453
	CUTIN_ANI_STYPEINFO(void) {
		Reset();
	}

	// globalgamedefine.h:5459
	void Reset(void) {
		m_posType = 0;
		m_style0 = CUTIN_ANI_STYLE_NONE;
		m_style1 = CUTIN_ANI_STYLE_NONE;
		m_style2 = CUTIN_ANI_STYLE_NONE;
		m_style3 = CUTIN_ANI_STYLE_NONE;
		m_style = CUTIN_ANI_STYLE_NONE;

		memset(m_image0, 0, sizeof(m_image0));
		memset(m_image1, 0, sizeof(m_image1));
		memset(m_image2, 0, sizeof(m_image2));
		memset(m_image3, 0, sizeof(m_image3));
	}

	int m_posType;              // 0x00, the overlay's position type (0..5)
	eCUTINANISTLYE m_style0;    // 0x04, the style of overlay 0 (CCutin::PrepareCutIn)
	eCUTINANISTLYE m_style1;    // 0x08
	eCUTINANISTLYE m_style2;    // 0x0C
	eCUTINANISTLYE m_style3;    // 0x10
	eCUTINANISTLYE m_style;     // 0x14
	char m_image0[0x28];        // 0x18, overlay 0's image name (CCutin::HandleMsg loads it)
	char m_image1[0x28];        // 0x40, overlay 1's image name
	char m_image2[0x28];        // 0x68, overlay 2's image name
	char m_image3[0x28];        // 0x90, overlay 3's image name
};

CHECK_SIZE(CUTIN_ANI_STYPEINFO, 0xB8);
CHECK_FIELD(CUTIN_ANI_STYPEINFO, m_posType, 0x00);
CHECK_FIELD(CUTIN_ANI_STYPEINFO, m_style0, 0x04);
CHECK_FIELD(CUTIN_ANI_STYPEINFO, m_style1, 0x08);
CHECK_FIELD(CUTIN_ANI_STYPEINFO, m_style2, 0x0C);
CHECK_FIELD(CUTIN_ANI_STYPEINFO, m_style3, 0x10);
CHECK_FIELD(CUTIN_ANI_STYPEINFO, m_style, 0x14);
CHECK_FIELD(CUTIN_ANI_STYPEINFO, m_image0, 0x18);
CHECK_FIELD(CUTIN_ANI_STYPEINFO, m_image1, 0x40);
CHECK_FIELD(CUTIN_ANI_STYPEINFO, m_image2, 0x68);
CHECK_FIELD(CUTIN_ANI_STYPEINFO, m_image3, 0x90);

// A period (time-limited) card in use: the special card buffs and the
// character / caddie cards with an ability period (CCardManager, CItemBuff,
// FrSPCardBuffDlg). 0x41 bytes, byte-packed: the list node copies 0x41
// bytes and sAbilityFinder::operator(), taking it by value, pops 0x44.
// Received whole by CItemBuff::ProcessPacket (DecodeBuffer, 0x41).
#pragma pack(push, 1)
struct sSCardAvilityPeriodInfo {
	unsigned long id;               // 0x00
	unsigned long typeId;           // 0x04, the card's type id
	unsigned long partsTypeId;      // 0x08, the character / caddie type id it is put on (0: none)
	unsigned long partsId;          // 0x0C, the character / caddie id
	int effect;                     // 0x10
	int effectValue;                // 0x14
	int slot;                       // 0x18
	SYSTEMTIME startTime;           // 0x1C
	SYSTEMTIME endTime;             // 0x2C
	int kind;                       // 0x3C, 0 character card, 1 caddie card, 2 special card
	unsigned char use;              // 0x40
};
#pragma pack(pop)
CHECK_SIZE(sSCardAvilityPeriodInfo, 0x41);
CHECK_FIELD(sSCardAvilityPeriodInfo, typeId, 0x04);
CHECK_FIELD(sSCardAvilityPeriodInfo, partsTypeId, 0x08);
CHECK_FIELD(sSCardAvilityPeriodInfo, partsId, 0x0C);
CHECK_FIELD(sSCardAvilityPeriodInfo, slot, 0x18);
CHECK_FIELD(sSCardAvilityPeriodInfo, startTime, 0x1C);
CHECK_FIELD(sSCardAvilityPeriodInfo, endTime, 0x2C);
CHECK_FIELD(sSCardAvilityPeriodInfo, kind, 0x3C);
CHECK_FIELD(sSCardAvilityPeriodInfo, use, 0x40);

// globalgamedefine.h:3965
// Prize kinds 3 to 5, which CGolfRule::AddExtPrize leaves out (they bring no
// item).
static bool IsNoItemPrizeKind(int kind) {
	if (kind >= 3 && kind <= 5)
		return true;
	return false;
}

#endif // PG_SHARED_GLOBALGAMEDEFINE_H
