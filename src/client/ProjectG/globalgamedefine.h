// Original: d:\build\custom\temp\code\client\projectg\globalgamedefine.h
#ifndef PG_CLIENT_PROJECTG_GLOBALGAMEDEFINE_H
#define PG_CLIENT_PROJECTG_GLOBALGAMEDEFINE_H

#include "defines.h"

#include <stdlib.h>
#include <string.h>

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
		memset(m_guildUID, 0, sizeof(m_guildUID)); memset(m_guildName, 0, sizeof(m_guildName)); memset(m_guildMark, 0, sizeof(m_guildMark));
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
	unsigned long m_guildUID[2];     // 0x5B (the two guilds of a guild match; FrDtRoomDlg::OnJoinBtnUp checks the player's guild against them)
	char m_guildName[2][21];         // 0x63 (cleared with m_guildUID)
	char m_guildMark[2][12];         // 0x8D (cleared with m_guildUID)
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
CHECK_FIELD(sRoomInfo, m_guildUID, 0x5B);
CHECK_FIELD(sRoomInfo, m_guildName, 0x63);
CHECK_FIELD(sRoomInfo, m_guildMark, 0x8D);
CHECK_FIELD(sRoomInfo, m_masterUID, 0xAD);
CHECK_FIELD(sRoomInfo, m_galleryCount, 0x45);
CHECK_FIELD(sRoomInfo, m_galleryLimit, 0x46);
CHECK_FIELD(sRoomInfo, m_value4B, 0x4B);
CHECK_FIELD(sRoomInfo, m_value4D, 0x4D);
CHECK_FIELD(sRoomInfo, m_value51, 0x51);
CHECK_FIELD(sRoomInfo, m_roomMode, 0xB1);

// A player of the room the local player is in (CSharedDoc::m_roomPlayerList
// is a std::list<sSlotInfo>, whose node copies 0x54 dwords + 1 word: 0x152
// bytes, byte-packed). conv_ghostdata_to_slotinfo fills one for a ghost;
// ntAvatarManager::CreateAvatar / CRealMyRoom_Avatar place the avatar from it.
#pragma pack(push, 1)
struct sSlotInfo {
	// globalgamedefine.h:3257
	sSlotInfo(void) {
		m_ruleUID = 0;
		m_uid = 0;
		m_level = 0;
		m_characterTypeId = 0;
		m_connection = 0;
		m_attribute = 0;
		m_identity = 0;
		m_valueCD = 0; m_titleTypeId = 0x30000000; m_flags55.flag0 = 0;
		m_flags151.flag = 0;
		memset(m_blockD1, 0, sizeof(m_blockD1));
		memset(m_emblemName, 0, sizeof(m_emblemName));
		memset(m_block3C, 0, sizeof(m_block3C));
	}

	// globalgamedefine.h:3287
	bool operator==(const unsigned long &uid) const {
		return m_uid == uid;
	}

	// globalgamedefine.h:3291
	bool IsIdentity(unsigned long identityFlag) {
		return (m_identity & identityFlag) ? true : false;
	}

	unsigned long m_uid;             // 0x00, the player's connection id (0xFFFFFFFE: a ghost, 0xFFFFFFFF: an empty slot); operator==
	char m_nickname[0x16];           // 0x04
	char m_guildName[0x15];          // 0x1A
	unsigned char m_connection;      // 0x2F, the lobby's connection sort key (ConnectionUserCompare; 2 for a ghost)
	unsigned long m_identity;        // 0x30, sPangYaUserInfo::identity flags (IsIdentity)
	unsigned long m_titleTypeId;     // 0x34, the equipped title's type id (0x30000000 after construction)
	unsigned long m_characterTypeId; // 0x38, the sChar typeid (GHOST_USER_DATA::characterTypeId)
	union {
		char m_block3C[0x18];        // 0x3C, cleared by the constructor
		struct {
			UNKNOWN(0x3C, 0x48);
			unsigned long m_roomCardSkinTypeId; // 0x48, nonzero: FindSkin()->m_cutinImage is drawn over the room card (CLobbyMain::OnGameRoom_RoomUserOwnerDraw)
			unsigned long m_iconSkinTypeId;     // 0x4C, nonzero: FindSkin()->m_cutinImage replaces the character icon on the room card
			UNKNOWN(0x50, 0x54);
		};
	};
	union {
		unsigned char m_attribute;   // 0x54, the low two bits go to sPlayerData (team)
		struct {
			unsigned char team : 2;  // bits 0-1, the team (sPlayerData)
			unsigned char dive : 1;  // bit 2, set or cleared by CLobbyMain::HandleMsg 0x3D
			unsigned char roomMaster : 2; // bits 3-4, 1: the room master (Avatar_Main sets and clears them)
			unsigned char userFlag : 3; // bits 5-7 (conv_ghostdata_to_slotinfo, GHOST_USER_DATA 0x24C)
		} m_attributeBits;           // 0x54
	};
	union {
		unsigned char m_mannerFlag;  // 0x55, bit 0: manner player
		struct {
			unsigned char flag0 : 1; // bit 0, cleared by the constructor
			unsigned char flag1 : 1; // bit 1, set by conv_ghostdata_to_slotinfo
			unsigned char : 6;
		} m_flags55;                 // 0x55
	};
	unsigned char m_level;           // 0x56, sUserInfo::m_level
	union {
		unsigned char m_angelFlag;   // 0x57, bits 0-1: the angel wing bits
		struct {
			unsigned char flag0 : 1; // bit 0 (GHOST_USER_DATA 0x24D)
			unsigned char flag1 : 1; // bit 1 (GHOST_USER_DATA 0x24E)
			unsigned char : 6;
		} m_flags57;                 // 0x57
	};
	unsigned char m_ladderGrade;     // 0x58, ladder grade icon index ("ladder_%03d", CLobbyMain::OnGameRoomExt_RoomUserOwnerDraw)
	unsigned long m_guildUID;        // 0x59, nonzero: ntAvatar shows the m_emblemName emblem
	char m_emblemName[0xC];          // 0x5D, the guild emblem's name
	unsigned long m_ruleUID;         // 0x69, passed to CGolfRule::AddPlayer
	unsigned long m_avatarState;     // 0x6D, the chat-room avatar's ntAvatar state
	PADDING(0x71, 0x73);
	unsigned long m_avatarAction;    // 0x73, the chat-room avatar's ntAvatar action
	float m_x;                       // 0x77, the avatar's position (CreateAvatar)
	float m_z;                       // 0x7B
	float m_angle;                   // 0x7F, the avatar's heading
	PADDING(0x83, 0x87);
	char m_tradeTitle[0x40];         // 0x87, the chat-room avatar's trade title
	unsigned long m_mascotTypeId;    // 0xC7, the equipped mascot's type id (GHOST_USER_DATA 0x17C, sUserInfo 0xB57)
	unsigned char m_itemBoostFlags;  // 0xCB, copied into sRivalData by SetPlayer
	unsigned char m_extraFlags;      // 0xCC, copied into sRivalData by SetPlayer
	unsigned long m_valueCD;         // 0xCD, cleared by the constructor
	char m_blockD1[0x80];            // 0xD1, cleared by the constructor
	union {
		unsigned char m_banishFlags; // 0x151, bit 0: the player cannot be banished (CLobbyMain::OnGameRoom_RoomUserLBtnUp)
		struct {
			unsigned char flag : 1;  // bit 0, cleared by the constructor
			unsigned char : 7;
		} m_flags151;                // 0x151
	};
};
#pragma pack(pop)

CHECK_SIZE(sSlotInfo, 0x152);
CHECK_FIELD(sSlotInfo, m_uid, 0x00);
CHECK_FIELD(sSlotInfo, m_nickname, 0x04);
CHECK_FIELD(sSlotInfo, m_ladderGrade, 0x58);
CHECK_FIELD(sSlotInfo, m_guildName, 0x1A);
CHECK_FIELD(sSlotInfo, m_connection, 0x2F);
CHECK_FIELD(sSlotInfo, m_identity, 0x30);
CHECK_FIELD(sSlotInfo, m_titleTypeId, 0x34);
CHECK_FIELD(sSlotInfo, m_characterTypeId, 0x38);
CHECK_FIELD(sSlotInfo, m_block3C, 0x3C);
CHECK_FIELD(sSlotInfo, m_roomCardSkinTypeId, 0x48);
CHECK_FIELD(sSlotInfo, m_iconSkinTypeId, 0x4C);
CHECK_FIELD(sSlotInfo, m_attribute, 0x54);
CHECK_FIELD(sSlotInfo, m_attributeBits, 0x54);
CHECK_FIELD(sSlotInfo, m_mannerFlag, 0x55);
CHECK_FIELD(sSlotInfo, m_flags55, 0x55);
CHECK_FIELD(sSlotInfo, m_level, 0x56);
CHECK_FIELD(sSlotInfo, m_angelFlag, 0x57);
CHECK_FIELD(sSlotInfo, m_flags57, 0x57);
CHECK_FIELD(sSlotInfo, m_guildUID, 0x59);
CHECK_FIELD(sSlotInfo, m_emblemName, 0x5D);
CHECK_FIELD(sSlotInfo, m_avatarState, 0x6D);
CHECK_FIELD(sSlotInfo, m_avatarAction, 0x73);
CHECK_FIELD(sSlotInfo, m_tradeTitle, 0x87);
CHECK_FIELD(sSlotInfo, m_ruleUID, 0x69);
CHECK_FIELD(sSlotInfo, m_x, 0x77);
CHECK_FIELD(sSlotInfo, m_z, 0x7B);
CHECK_FIELD(sSlotInfo, m_angle, 0x7F);
CHECK_FIELD(sSlotInfo, m_mascotTypeId, 0xC7);
CHECK_FIELD(sSlotInfo, m_itemBoostFlags, 0xCB);
CHECK_FIELD(sSlotInfo, m_extraFlags, 0xCC);
CHECK_FIELD(sSlotInfo, m_valueCD, 0xCD);
CHECK_FIELD(sSlotInfo, m_blockD1, 0xD1);
CHECK_FIELD(sSlotInfo, m_banishFlags, 0x151);
CHECK_FIELD(sSlotInfo, m_flags151, 0x151);

// A received note, element of CSharedDoc's note list (m_noteList; its node
// is 0x78 bytes, hence sizeof 0x70). FrMailBoxDlg lists them.
struct sNoteInfo {
	// globalgamedefine.h:3666
	sNoteInfo(void) {
		clear();
	}

	// globalgamedefine.h:3672
	void clear(void) {
		id = 0xFFFFFFFF;
		extra = 0;
		memset(nick, 0, sizeof(nick));
		memset(text, 0, sizeof(text));
		memset(date, 0, sizeof(date));
		read = false;
	}

	unsigned long id;        // 0x00, sent back in the read receipt (FrMailBoxDlg::OnNoteDlgResult); 0xFFFFFFFF once cleared
	unsigned short extra;    // 0x04, cleared by clear(); meaning not known
	char nick[0x16];         // 0x06, the sender ('@' first: an operator)
	char text[0x40];         // 0x1C
	char date[0x10];         // 0x5C
	bool read;               // 0x6C, drawn grey once read
	PADDING(0x6D, 0x70);
};
CHECK_SIZE(sNoteInfo, 0x70);
CHECK_FIELD(sNoteInfo, id, 0x00);
CHECK_FIELD(sNoteInfo, extra, 0x04);
CHECK_FIELD(sNoteInfo, nick, 0x06);
CHECK_FIELD(sNoteInfo, text, 0x1C);
CHECK_FIELD(sNoteInfo, date, 0x5C);
CHECK_FIELD(sNoteInfo, read, 0x6C);

#endif // PG_CLIENT_PROJECTG_GLOBALGAMEDEFINE_H
