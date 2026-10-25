// Original: d:\build\custom\temp\code\client\projectg\golfdoc.h
#ifndef PG_CLIENT_PROJECTG_GOLFDOC_H
#define PG_CLIENT_PROJECTG_GOLFDOC_H

#include "defines.h"
#include "padding.h"
#include "wmath.h"
#include "parttidlist.h"
#include "ObjectFactory.h"
#include "encrypttypes.h"
#include "TaskManager.h"
#include "wlist.h"
#include "wflag.h"

#include <list>
#include <map>
#include <stack>
#include <string>
#include <vector>

class WPolySoup;
class CSky;
class CAutoTarget;

// One entry of CPropertyInfo's property vector (CPropertyInfo::GetProperty
// strides 0xE4).
struct sProperty {
	sProperty();
	~sProperty();

	float m_bounceCoeff;    // 0x0, CQuadTree::GetBounceCoeff
	float m_rollCoeff;      // 0x4, CQuadTree::GetRollCoeff
	float m_minPowerOnGround[3];    // 0x8, indexed by sPlayerData::m_powerShotLevel (CQuadTree::GetMinPowerOnGround)
	float m_maxPowerOnGround[3];    // 0x14, indexed by sPlayerData::m_powerShotLevel (CQuadTree::GetMaxPowerOnGround)
	bool m_penetrable : 1;  // 0x20 bit 0, CQuadTree::IsPenetrable
	bool m_outOfBounds : 1; // 0x20 bit 1, CQuadTree::IsOBArea
	unsigned char m_weatherBounceType;  // 0x21, 0..8, CQuadTree::GetWeatherBounceCoeff
	char m_name[0x40];      // 0x22, emptied by the constructor; the bounce sound (CQuadTree::GetBounceSound)
	char m_desc[0x40];      // 0x62, emptied by the constructor; the roll sound (CQuadTree::GetRollSound)
	char m_stepSound[0x40]; // 0xA2, CQuadTree::GetStepSound
	PADDING(0xE2, 0xE4);
};
CHECK_SIZE(sProperty, 0xE4);
CHECK_FIELD(sProperty, m_bounceCoeff, 0x0);
CHECK_FIELD(sProperty, m_rollCoeff, 0x4);
CHECK_FIELD(sProperty, m_stepSound, 0xA2);
CHECK_FIELD(sProperty, m_name, 0x22);
CHECK_FIELD(sProperty, m_desc, 0x62);
CHECK_FIELD(sProperty, m_minPowerOnGround, 0x8);
CHECK_FIELD(sProperty, m_maxPowerOnGround, 0x14);
CHECK_FIELD(sProperty, m_weatherBounceType, 0x21);

class TiXmlDocument;

// Name/value pairs read from a map's XML document (Init). Its constructor
// and destructor are compiler-generated.
class CMapInfo {
	LAYOUT_CHECK_FRIEND;
public:
	void Init(const TiXmlDocument &doc, unsigned char index);
	const char *FindItem(const std::string &name);

private:
	std::map<std::string, std::string> m_items;     // 0x0
};

CHECK_SIZE(CMapInfo, 0xC);
BEGIN_LAYOUT_CHECK(CMapInfo)
	CHECK_FIELD(CMapInfo, m_items, 0x0);
END_LAYOUT_CHECK

// The properties read from an XML document (Init), by index and by name.
class CPropertyInfo {
	LAYOUT_CHECK_FRIEND;
public:
	// golfdoc.h:185
	CPropertyInfo() {
		m_defaultIndex = -1;
	}
	~CPropertyInfo();

	void Init(const TiXmlDocument &doc);
	int GetPropertyIndex(const char *name);
	const sProperty &GetProperty(int index);

private:
	std::vector<sProperty> m_properties;            // 0x0
	std::map<std::string, int> m_indexByName;       // 0x10, GetPropertyIndex
	int m_defaultIndex;                             // 0x1C, GetProperty's fallback for an index out of range
};

CHECK_SIZE(CPropertyInfo, 0x20);
BEGIN_LAYOUT_CHECK(CPropertyInfo)
	CHECK_FIELD(CPropertyInfo, m_properties, 0x0);
	CHECK_FIELD(CPropertyInfo, m_indexByName, 0x10);
	CHECK_FIELD(CPropertyInfo, m_defaultIndex, 0x1C);
END_LAYOUT_CHECK

// The round's default camera (CGolfDoc::m_cameraData, 0xF4..0x148).
// Its default constructor is compiler-generated: the original emits it
// out of line with no line record.
struct sCameraData {
	sCameraData();

	float m_distance;       // 0x0, 15 or 20 by the ball's state (MakeDefaultCamera)
	float m_height;         // 0x4, 7 or 5 by the ball's state (MakeDefaultCamera)
	float m_groundOffset;   // 0x8, added to the ground height under the camera (MakeDefaultCamera)
	WVector m_target;       // 0xC, the ball's position
	float m_yaw;            // 0x18, the RotMat angle of the view direction
	float m_yaw2;           // 0x1C, the RotMat angle turning the view direction off the tee
	WMatrix m_matrix;       // 0x20, the camera matrix (pivot = the eye, za = the view direction)
	float m_pitch;          // 0x50, its tangent raises the look-at point (GetDefaultCamera)
};
CHECK_SIZE(sCameraData, 0x54);
CHECK_FIELD(sCameraData, m_distance, 0x0);
CHECK_FIELD(sCameraData, m_height, 0x4);
CHECK_FIELD(sCameraData, m_groundOffset, 0x8);
CHECK_FIELD(sCameraData, m_target, 0xC);
CHECK_FIELD(sCameraData, m_yaw, 0x18);
CHECK_FIELD(sCameraData, m_yaw2, 0x1C);
CHECK_FIELD(sCameraData, m_matrix, 0x20);
CHECK_FIELD(sCameraData, m_pitch, 0x50);

// Its default constructor is compiler-generated: the original emits it
// out of line with no line record.
struct sHoleData {
	sHoleData();

	WVector m_teePos;       // 0x0
	WVector m_cupPos;       // 0xC, the hole cup's position (CHoleCupInfo::Reset)
	unsigned char m_par;    // 0x18, the hole's par (read by CGolfRuleTeam)
	unsigned char m_kind;   // 0x19, 2-4 (CScore::HandleMsg 0x13a counts the holes of each kind)
	unsigned char m_playerRank[4]; // 0x1A, each player's rank on the hole, by player index (CGolfCaddie::TalkProcess: 1 = first, GetCurrentPlayerNum() = last)
	unsigned char m_mapState; // 0x1E, set to 3 for every hole by the console's "map" command
	bool m_redPang;         // 0x1F, the score board prints the hole's pang in red (FrScoreForm::OnPangListOwnerDraw)
	__int64 m_pang;         // 0x20, the hole's pang (CGolfRule::OnPreLoadInit: GetHolePang)
	__int64 m_basePang;     // 0x28, the hole's pang as the round started (CGolfRule::OnPreLoadInit: GetHolePang)
	unsigned long m_holeKey; // 0x30, sent to the server with the hole's tee and cup positions (CGolfRule::SendHoleData; meaning not known)
	PADDING(0x34, 0x38);
};
CHECK_SIZE(sHoleData, 0x38);
CHECK_FIELD(sHoleData, m_teePos, 0x0);
CHECK_FIELD(sHoleData, m_cupPos, 0xC);
CHECK_FIELD(sHoleData, m_par, 0x18);
CHECK_FIELD(sHoleData, m_playerRank, 0x1A);
CHECK_FIELD(sHoleData, m_mapState, 0x1E);
CHECK_FIELD(sHoleData, m_redPang, 0x1F);
CHECK_FIELD(sHoleData, m_pang, 0x20);
CHECK_FIELD(sHoleData, m_basePang, 0x28);
CHECK_FIELD(sHoleData, m_holeKey, 0x30);

// The item typeids a player's hole result awards (CGolfRule::AddPrize, which
// clears them all once it has used one; CSharedDoc::SetWinningPrize stores
// them). They sit right after a bool, at an odd offset, so the original packs
// them to 1 byte.
#pragma pack(push, 1)
struct sWinningPrize {
	unsigned long m_typeId[6];  // 0x0, [0] hole in one, [1]-[5] the hole's score against par, -3 to +1
};
#pragma pack(pop)
CHECK_SIZE(sWinningPrize, 0x18);

// An extra prize of a player (CSharedDoc::SetExtPrize appends them to
// sPlayerData::m_extPrizes, CGolfRule::AddExtPrize awards them). The array
// sits at an even, not 4-aligned offset: packed to 1 byte.
#pragma pack(push, 1)
struct sPrizeInfo {
	unsigned long m_typeId;     // 0x0, the item awarded (AddExtPrize looks it up with FindCommonItem)
	PADDING(0x4, 0x6);
	unsigned short m_quantity;  // 0x6, AddExtPrize raises it to at least 10 for typeid 0x1A000010
	int m_kind;                 // 0x8, AddExtPrize skips kinds 3 to 5
};
#pragma pack(pop)
CHECK_SIZE(sPrizeInfo, 0xC);
CHECK_FIELD(sPrizeInfo, m_typeId, 0x0);
CHECK_FIELD(sPrizeInfo, m_quantity, 0x6);
CHECK_FIELD(sPrizeInfo, m_kind, 0x8);

// A player of the round (CGolfDoc::MakePlayer allocates them, stride 0xB98).
// Its implicit constructor (0x433fa0, no line info) builds the WCrypticValue
// members in layout order.
// The names of the per-hole records past m_holes and of the single crypted
// values are inferred from their few readers; renaming is welcome.
struct sPlayerData {
	int m_state;            // 0x0, 3 = not playing (RefreshPlayerNum)
	unsigned char m_index;  // 0x4, the player's slot in the room
	PADDING(0x5, 0x8);      // members not transcribed yet
	WVector m_lastShotPos;  // 0x8, where the last shot was played from: CAutoTarget::CalcDropPos drops the ball back here
	WVector m_ballPos;      // 0x14, the player's ball (CHoleCupInfo::Reset measures the cup's height against its y)
	WCrypticValue<unsigned char> m_holes[18];   // 0x20, the scorecard, indexed [hole - 1]
	WCrypticValue<unsigned char> m_holeIn[18];  // 0x140, indexed [hole - 1]: the ball is in the cup (CGolfBall IsHoleIn/SetHoleIn)
	WCrypticValue<int> m_holeRecord[18];        // 0x260, per hole (meaning not known)
	WCrypticValue<int> m_holeRecord2[18];       // 0x380, per hole (meaning not known)
	unsigned long m_color;              // 0x4A0, 0x00RRGGBB: CGolfRuleBase::m_colors[slot] (AddPlayer, AddPlayerOffline)
	unsigned long m_oid;                // 0x4A4, AddPlayer's first number, not set offline (inferred: the server's connection id)
	unsigned long m_uid;                // 0x4A8, sent with message 0x82 when the player drops out (CGolfRule::CheckDropOut; inferred)
	unsigned char m_numItems;           // 0x4AC, the item slots in use (CItemWindow::ProcessItem decrements it when one is used up)
	PADDING(0x4AD, 0x4B0);
	unsigned long m_shotItemTypeId;     // 0x4B0, the typeid of the item used for the current shot, 0 for none (cleared by CCalipers::Reset; CPowerGauge::GetAddLuckyPangYaArea, CItemWindow's help message)
	unsigned char m_turnIndex;          // 0x4B4, 0-based: the playing players added before this one (AddPlayer); CGolfRule::GetFirstTurn picks the player at 0
	PADDING(0x4B5, 0x4B8);
	WCrypticValue<float> m_comboGauge;  // 0x4B8, the combo gauge
	short m_value4C8;               // 0x4C8, set to -1 when a shot starts (ChangeGameMode 0x40; role not known)
	unsigned char m_powerShotLevel;     // 0x4CA, 0 normal, 1-2 power shot grades (inferred): indexes sProperty::m_maxPowerOnGround (CQuadTree::GetMaxPowerOnGround); CCrab::PlayEvent plays only when >= 1
	unsigned char m_numTimeout;         // 0x4CB, the shots the player let the clock run out
	unsigned char m_giveUpCount;        // 0x4CC, the holes the player gave up (CGolfRule::RecordGiveUp increments it)
	PADDING(0x4CD, 0x4D0);
	unsigned long m_bonusFlags;         // 0x4D0, the round's bonus flags: CPangInfo::Record tests 0x1000, 0x4000 and 0x40000-0x800000
	                                    // (bonuses to the other players), PreCalc*HoleInBonus clear 0x1000 and set 0x2000;
	                                    // CGolfRule::SendShotResult sends the whole word (bit meanings not known)
	WFlags m_ghostAwardBase;            // 0x4D4, flag 1: the disappointed putt pose was played (CGolfRule::DoFlyBall_Stopped); reset by ResetData; read by CGhostPlayer::SendAwardResult
	unsigned char m_rivalLevel;         // 0x4D8, the rival's difficulty level
	unsigned char m_attribute;          // 0x4D9, low two bits of the room entry's attribute (the team index in a team game)
	unsigned char m_partnerIndex;       // 0x4DA, the team partner's player index, 0xFF for none
	PADDING(0x4DB, 0x4DC);
	unsigned long m_playTime;           // 0x4DC, in milliseconds (inferred: CGolfRule::SendHoleStat sends it / 1000)
	unsigned char m_turnOrder : 4;      // 0x4E0 bits 0-3, 1-based m_turnIndex (AddPlayer); CHoleInfo::Reset picks the playing player at 1
	unsigned char m_joinState : 4;      // 0x4E0 bits 4-7, set to 1 by AddPlayer and AddPlayerOffline (meaning not known)
	unsigned char m_totalScore;         // 0x4E1, the sum of the holes' scores (CGolfRule::RecordPlayerData)
	PADDING(0x4E2, 0x4E4);
	unsigned long m_ghostResult;        // 0x4E4, copied into FrGhostAward::sResultInfo +0x40 (FrGhostAward::SetResultInfo; meaning not known)
	WCrypticValue<int> m_ghostAward;    // 0x4E8, the player's game pang (CBonusInfo::m_awardPang; CGhostPlayer::SendAwardResult)
	WCrypticValue<int> m_extraPang;     // 0x4F8, the round's extra pang (CBonusInfo::m_extraPang)
	int m_recordedHoles;                // 0x508, the holes already added to m_totalScore (CGolfRule::RecordPlayerData)
	// The five shot stats, set by CGolfDoc::SetPlayerLevel in the order
	// power, control, accuracy, spin, curve (golfdoc.cpp:592-596); CClub::SetAccuracy reads 0x51C.
	WCrypticValue<int> m_power;         // 0x50C
	WCrypticValue<int> m_accuracy;      // 0x51C
	WCrypticValue<int> m_control;       // 0x52C
	WCrypticValue<int> m_spin;          // 0x53C
	WCrypticValue<int> m_curve;         // 0x54C
	unsigned char m_chatPenalty;        // 0x55C, the player is under a chat penalty: cleared by ChatManager::CheckPenaltyTime when its time is up (HeadIcon shows it)
	sWinningPrize m_prizeItems;         // 0x55D, the items AddPrize awards (CSharedDoc::SetWinningPrize)
	unsigned char m_extPrizeCount;      // 0x575, the m_extPrizes in use (CSharedDoc::SetExtPrize adds to it; AddPlayer clears it)
	sPrizeInfo m_extPrizes[128];        // 0x576, the extra prizes (CSharedDoc::SetExtPrize, CGolfRule::AddExtPrize)
	// The player's copies of sTeamData's bytes at 0x3B1/0x3B2 (fontinfo
	// tests the kind for 9 and 10 and multiplies the pang by 2^count).
	unsigned char m_holeInKind;         // 0xB76
	unsigned char m_holeInCount;        // 0xB77
	__int64 m_itemPang;                 // 0xB78, the pang the round's items gave (CBonusInfo::m_itemPang)
	__int64 m_roundPang;                // 0xB80, a pang total AddPlayer zeroes with m_itemPang (meaning not known)
	__int64 m_rewardPang;               // 0xB88, a pang total AddPlayer zeroes with m_itemPang (meaning not known)
	unsigned char m_angelWingGrade;     // 0xB90, the lower, the larger GetAngelBonus
	PADDING(0xB91, 0xB98);
};
CHECK_SIZE(sPlayerData, 0xB98);
CHECK_FIELD(sPlayerData, m_state, 0x0);
CHECK_FIELD(sPlayerData, m_index, 0x4);
CHECK_FIELD(sPlayerData, m_ballPos, 0x14);
CHECK_FIELD(sPlayerData, m_holes, 0x20);
CHECK_FIELD(sPlayerData, m_holeIn, 0x140);
CHECK_FIELD(sPlayerData, m_holeRecord, 0x260);
CHECK_FIELD(sPlayerData, m_holeRecord2, 0x380);
CHECK_FIELD(sPlayerData, m_color, 0x4A0);
CHECK_FIELD(sPlayerData, m_oid, 0x4A4);
CHECK_FIELD(sPlayerData, m_uid, 0x4A8);
CHECK_FIELD(sPlayerData, m_turnIndex, 0x4B4);
CHECK_FIELD(sPlayerData, m_numItems, 0x4AC);
CHECK_FIELD(sPlayerData, m_lastShotPos, 0x8);
CHECK_FIELD(sPlayerData, m_shotItemTypeId, 0x4B0);
CHECK_FIELD(sPlayerData, m_comboGauge, 0x4B8);
CHECK_FIELD(sPlayerData, m_powerShotLevel, 0x4CA);
CHECK_FIELD(sPlayerData, m_numTimeout, 0x4CB);
CHECK_FIELD(sPlayerData, m_bonusFlags, 0x4D0);
CHECK_FIELD(sPlayerData, m_ghostAwardBase, 0x4D4);
CHECK_FIELD(sPlayerData, m_attribute, 0x4D9);
CHECK_FIELD(sPlayerData, m_partnerIndex, 0x4DA);
CHECK_FIELD(sPlayerData, m_playTime, 0x4DC);
CHECK_FIELD(sPlayerData, m_totalScore, 0x4E1);
CHECK_FIELD(sPlayerData, m_recordedHoles, 0x508);
CHECK_FIELD(sPlayerData, m_ghostResult, 0x4E4);
CHECK_FIELD(sPlayerData, m_ghostAward, 0x4E8);
CHECK_FIELD(sPlayerData, m_extraPang, 0x4F8);
CHECK_FIELD(sPlayerData, m_power, 0x50C);
CHECK_FIELD(sPlayerData, m_accuracy, 0x51C);
CHECK_FIELD(sPlayerData, m_control, 0x52C);
CHECK_FIELD(sPlayerData, m_spin, 0x53C);
CHECK_FIELD(sPlayerData, m_curve, 0x54C);
CHECK_FIELD(sPlayerData, m_chatPenalty, 0x55C);
CHECK_FIELD(sPlayerData, m_prizeItems, 0x55D);
CHECK_FIELD(sPlayerData, m_giveUpCount, 0x4CC);
CHECK_FIELD(sPlayerData, m_extPrizeCount, 0x575);
CHECK_FIELD(sPlayerData, m_extPrizes, 0x576);
CHECK_FIELD(sPlayerData, m_holeInKind, 0xB76);
CHECK_FIELD(sPlayerData, m_holeInCount, 0xB77);
CHECK_FIELD(sPlayerData, m_rivalLevel, 0x4D8);
CHECK_FIELD(sPlayerData, m_itemPang, 0xB78);
CHECK_FIELD(sPlayerData, m_roundPang, 0xB80);
CHECK_FIELD(sPlayerData, m_rewardPang, 0xB88);
CHECK_FIELD(sPlayerData, m_angelWingGrade, 0xB90);

// A team of a team game (CGolfDoc::MakeTeam allocates two, stride 0x3B4). Its
// implicit constructor (0x433f10, no line info) builds WCrypticValue<unsigned char>[18] at 0x4 and
// 0x124, WCrypticValue<int>[18] at 0x244, WCrypticValue<int> at 0x36C and
// 0x37C and WCrypticValue<float> at 0x39C.
struct sTeamData {
	int m_state;                    // 0x0, compared with 1 by CGolfRuleTeam
	WCrypticValue<unsigned char> m_holes[18];   // 0x4, the team's scorecard, indexed [hole - 1]
	WCrypticValue<unsigned char> m_holeIn[18];  // 0x124, per hole, like sPlayerData::m_holeIn
	WCrypticValue<int> m_holeRecord[18];        // 0x244, per hole (meaning not known)
	unsigned char m_totalScore;     // 0x364, the sum of the holes' GetScore (CGolfRule::RecordTeamData)
	PADDING(0x365, 0x368);
	int m_resetCount;               // 0x368, zeroed by CGolfDoc::ResetTeamVars (meaning not known)
	WCrypticValue<int> m_teamValue; // 0x36C, reset by CGolfDoc::ResetTeamVars (meaning not known)
	WCrypticValue<int> m_teamValue2;    // 0x37C (meaning not known)
	WVector m_ballPos;              // 0x38C, the team's ball (CFallingSpot::Display marks it in team games; CGolfRule::ResetData)
	unsigned long m_color;          // 0x398, set by CGolfDoc::ResetTeamVars by team (0xFF60FF / 0xFFFF00 low 24 bits; inferred)
	WCrypticValue<float> m_teamRate;    // 0x39C, the team's combo gauge: CGolfRuleTeam::AddPlayer raises it to a joining player's (reset by CGolfDoc::ResetTeamVars)
	unsigned char m_powerShotLevel; // 0x3AC, the power shot level of the team's ball (CGolfRule::CorrectShotResult copies the shooter's sPlayerData::m_powerShotLevel); cleared per hole by CGolfRule::ResetData
	// Zeroed one by one by CGolfDoc::ResetTeamVars. m_resetBytes keeps the
	// array name until every user reads the named bytes.
	union {
		unsigned char m_resetBytes[3];  // 0x3AD
		struct {
			unsigned char m_clubHolder;     // 0x3AD, the team member whose club set is shown (CGolfPlayer: the others hide theirs)
			unsigned char m_clubHolderClub; // 0x3AE, the club type of m_clubHolder's set (index into its puppets)
			unsigned char m_nextTurnSlot;   // 0x3AF, the team's slot (0 or 1) that plays next: CGolfRule flips it (1 - slot) and adds team * 2
		};
	};
	unsigned char m_firstTurnSlot;  // 0x3B0, the team's player slot that tees off first (CGolfRule::GetFirstTurn adds team * 2)
	// Zeroed one by one by CGolfDoc::ResetTeamVars; the players' copies are
	// sPlayerData::m_holeInKind/m_holeInCount (fontinfo tests the kind for 9
	// and 10 and multiplies the pang by 2^count).
	unsigned char m_holeInKind;     // 0x3B1
	unsigned char m_holeInCount;    // 0x3B2
	PADDING(0x3B3, 0x3B4);
};
CHECK_SIZE(sTeamData, 0x3B4);
CHECK_FIELD(sTeamData, m_state, 0x0);
CHECK_FIELD(sTeamData, m_holes, 0x4);
CHECK_FIELD(sTeamData, m_holeIn, 0x124);
CHECK_FIELD(sTeamData, m_holeRecord, 0x244);
CHECK_FIELD(sTeamData, m_totalScore, 0x364);
CHECK_FIELD(sTeamData, m_resetCount, 0x368);
CHECK_FIELD(sTeamData, m_teamValue, 0x36C);
CHECK_FIELD(sTeamData, m_teamValue2, 0x37C);
CHECK_FIELD(sTeamData, m_color, 0x398);
CHECK_FIELD(sTeamData, m_teamRate, 0x39C);
CHECK_FIELD(sTeamData, m_resetBytes, 0x3AD);
CHECK_FIELD(sTeamData, m_firstTurnSlot, 0x3B0);
CHECK_FIELD(sTeamData, m_holeInKind, 0x3B1);
CHECK_FIELD(sTeamData, m_holeInCount, 0x3B2);
CHECK_FIELD(sTeamData, m_ballPos, 0x38C);
CHECK_FIELD(sTeamData, m_powerShotLevel, 0x3AC);
CHECK_FIELD(sTeamData, m_clubHolder, 0x3AD);
CHECK_FIELD(sTeamData, m_clubHolderClub, 0x3AE);
CHECK_FIELD(sTeamData, m_nextTurnSlot, 0x3AF);

// The round's result of one player (CGolfDoc keeps four, 0x21 bytes apart:
// the original packs it to 1 byte).
#pragma pack(push, 1)
struct sRoundResult {
	unsigned long m_uid;    // 0x0, the player the result is about: CGolfTask's game-end messages (0x64, 0x8F, 0xB1) store each result at CSharedDoc::GetIndex(m_uid)
	// CGolfRule::HandleMsg 0x80 (game end) copies the next five into the
	// player's sPlayerData members of the same name.
	unsigned char m_joinState;  // 0x4, the low nibble goes into sPlayerData::m_joinState
	signed char m_ghostResult;  // 0x5, sign-extended into sPlayerData::m_ghostResult
	unsigned char m_totalScore; // 0x6
	short m_exp;            // 0x7, the exp the round gave (FrAwardForm::CalcResultMatch adds it to the player's exp)
	int m_ghostAward;       // 0x9, the player's game pang (sPlayerData::m_ghostAward)
	PADDING(0xD, 0x11);
	int m_extraPang;        // 0x11, sPlayerData::m_extraPang
	PADDING(0x15, 0x19);
	__int64 m_itemPang;     // 0x19, copied into sPlayerData::m_itemPang in room type 7
};
#pragma pack(pop)
CHECK_SIZE(sRoundResult, 0x21);
CHECK_FIELD(sRoundResult, m_uid, 0x0);
CHECK_FIELD(sRoundResult, m_joinState, 0x4);
CHECK_FIELD(sRoundResult, m_ghostResult, 0x5);
CHECK_FIELD(sRoundResult, m_totalScore, 0x6);
CHECK_FIELD(sRoundResult, m_exp, 0x7);
CHECK_FIELD(sRoundResult, m_ghostAward, 0x9);
CHECK_FIELD(sRoundResult, m_extraPang, 0x11);
CHECK_FIELD(sRoundResult, m_itemPang, 0x19);

// The result of one player's shot (CGolfDoc keeps four, 0x25 bytes apart:
// the original packs it to 1 byte). CGolfRule::SendShotResult sends the
// local player's to the server (packet 0x1B), which relays it to every
// player (CGolfTask's message 0x62).
// A position as three plain floats: sShotResult is copied with rep movsd /
// movsb, so its position is not a WVector (whose operator= is out of line).
// Same member names as WVector (x, y, z and p[]).
struct sPos3 {
	union {
		struct { float x, y, z; };
		float p[3];
	};
};
CHECK_SIZE(sPos3, 0xC);

#pragma pack(push, 1)
struct sShotResult {
	unsigned long m_uid;    // 0x0, the player the result is about: CGolfTask's message 0x62 stores it at CSharedDoc::GetIndex(m_uid)
	sPos3 m_pos;            // 0x4, where the ball stopped (sPlayerData::m_ballPos); reset to the tee by InitShotResultMsg
	unsigned char m_resultType;     // 0x10, how the shot ended: 3 the ball's m_landed is set, 4 holed, 2 stopped after a bounce, 5 stopped without one
	unsigned char m_powerShotLevel; // 0x11, sPlayerData::m_powerShotLevel; InitShotResultMsg clears it
	unsigned char m_replayMask; // 0x12, the sender's CGolfRule::m_replayFlags; CGolfRule's message 0x79 ANDs it into m_replayFlags
	int m_pang;             // 0x13, sPlayerData::m_ghostAward
	int m_clearBonus;       // 0x17, sPlayerData::m_extraPang; CGolfRule's message 0x79 adds the round's clear bonus
	unsigned long m_bonusFlags;     // 0x1B, sPlayerData::m_bonusFlags; bit 0x2000: the round was cleared (message 0x79 adds the clear bonus)
	unsigned long m_shotFlags;      // 0x1F, the ball's m_flags recoded: 2/4/8 from bits 0x10/0x40/0x20, 0x10/0x20 from bits 1/2, 0x40/0x80 from bits 4/8
	short m_preDataCount;   // 0x23, the ball's m_preDataCount
};
#pragma pack(pop)
CHECK_SIZE(sShotResult, 0x25);
CHECK_FIELD(sShotResult, m_uid, 0x0);
CHECK_FIELD(sShotResult, m_pos, 0x4);
CHECK_FIELD(sShotResult, m_resultType, 0x10);
CHECK_FIELD(sShotResult, m_powerShotLevel, 0x11);
CHECK_FIELD(sShotResult, m_replayMask, 0x12);
CHECK_FIELD(sShotResult, m_pang, 0x13);
CHECK_FIELD(sShotResult, m_clearBonus, 0x17);
CHECK_FIELD(sShotResult, m_bonusFlags, 0x1B);
CHECK_FIELD(sShotResult, m_shotFlags, 0x1F);
CHECK_FIELD(sShotResult, m_preDataCount, 0x23);

// Positions on the shot bar (sBar::Persent reads BAR_START from .rdata).
// The original initializes BAR_LENGTH (360.0f) dynamically at golfdoc.h:282
// in fontinfo, GolfRule, gui_control, rival, SharedDoc, ShotPreview, stdafx
// and videoin. The value of other const variables is not a constant
// expression for cl, so it is dynamic where <atlbase.h> (the PCH stand-in,
// _AtlWinModule's initializer) precedes this header, folded elsewhere
// (docs/tough_problems/010).
const float BAR_START = 140.0f;
const float BAR_END = 500.0f;
// golfdoc.h:282
const float BAR_LENGTH = BAR_END - BAR_START;
// golfdoc.h:283: the middle of the bar (320.0f), initialized dynamically only
// in stdafx and videoin, the objects defining PG_GOLFDOC_BAR_CENTER; the
// other includers of the original have no initializer for it.
#ifdef PG_GOLFDOC_BAR_CENTER
const float BAR_CENTER = (BAR_START + BAR_END) * 0.5f;
#endif

// The power/accuracy bar of a shot; sBar::Reset (golfdoc.cpp:1146) writes
// every member, CGolfDoc's next member at 0xF4 bounds the size.
struct sBar {
	void Reset(void);
	// golfdoc.h:298
	float Persent(void) const;
	// golfdoc.h:300
	float Delta(void) const;

	float m_power;          // 0x0
	float m_position;       // 0x4
	float m_accuracy;       // 0x8
	float m_prevAccuracy;   // 0xC
	int m_accuracyGrade;    // 0x10, 1..4 from the accuracy error (CRival::Shot)
	bool m_stopped;         // 0x14
	int m_state;            // 0x18
	float m_startPosition;  // 0x1C
	float m_impact;         // 0x20
};
CHECK_SIZE(sBar, 0x24);
CHECK_FIELD(sBar, m_power, 0x0);
CHECK_FIELD(sBar, m_position, 0x4);
CHECK_FIELD(sBar, m_accuracy, 0x8);
CHECK_FIELD(sBar, m_prevAccuracy, 0xC);
CHECK_FIELD(sBar, m_accuracyGrade, 0x10);
CHECK_FIELD(sBar, m_stopped, 0x14);
CHECK_FIELD(sBar, m_state, 0x18);
CHECK_FIELD(sBar, m_startPosition, 0x1C);
CHECK_FIELD(sBar, m_impact, 0x20);

enum eApproachMissionType {
};

enum eApproachPrizeType {
};

enum eApproachWinnerType {
};

// Defined in shared/globalgamedefine.h.
struct sApproachResultData;

enum eTeamName {
};

// A shot as the network protocol carries it (CGolfDoc keeps one, cleared by
// its constructor; GhostManager converts it from/to GHOST_SHOT_PROTOCOL
// field by field, which gives the packed size 0x2E). Declared here until
// its original header is known.
#pragma pack(push, 1)
// CGolfRule::HandleMsg (golfrule.cpp:4988-5011) applies a received shot: each
// member's comment names where it goes.
struct sShotProtocol {
	float power;                    // 0x00, CGolfDoc::m_bar.m_power
	float accuracy;                 // 0x04, CGolfDoc::m_bar.m_position, m_accuracy and m_prevAccuracy
	float lieSlope;                 // 0x08, CGolfBall::m_lieSlope
	float lieAngle;                 // 0x0C, CGolfBall::m_lieAngle
	unsigned char accuracyGrade;    // 0x10, CGolfDoc::m_bar.m_accuracyGrade
	unsigned long powerFlags;       // 0x11, CGolfBall::m_powerFlags
	unsigned long frame;            // 0x15, CQuadTree::m_frame (GetPVS)
	float angle;                    // 0x19, CGolfDoc 0x10C (CGolfRule::SetAngle)
	int shotElapsed;                // 0x1D, CGolfDoc::m_shotElapsed
	float barStartPosition;         // 0x21, CGolfDoc::m_bar.m_startPosition
	unsigned char club;             // 0x25, CClub::SetClub
	float randomPower;              // 0x26, CClub::m_randomPower
	float spot;                     // 0x2A, CGolfRule 0x278, the value ChangeGameMode hands the "Spot" actor (message 0xFA)
};
#pragma pack(pop)
CHECK_SIZE(sShotProtocol, 0x2E);
CHECK_FIELD(sShotProtocol, power, 0x00);
CHECK_FIELD(sShotProtocol, accuracy, 0x04);
CHECK_FIELD(sShotProtocol, lieSlope, 0x08);
CHECK_FIELD(sShotProtocol, lieAngle, 0x0C);
CHECK_FIELD(sShotProtocol, accuracyGrade, 0x10);
CHECK_FIELD(sShotProtocol, powerFlags, 0x11);
CHECK_FIELD(sShotProtocol, frame, 0x15);
CHECK_FIELD(sShotProtocol, angle, 0x19);
CHECK_FIELD(sShotProtocol, shotElapsed, 0x1D);
CHECK_FIELD(sShotProtocol, barStartPosition, 0x21);
CHECK_FIELD(sShotProtocol, club, 0x25);
CHECK_FIELD(sShotProtocol, randomPower, 0x26);
CHECK_FIELD(sShotProtocol, spot, 0x2A);

// Another player's shot-clock state, as packet 0x55 carries it (5 bytes,
// copied whole into CGolfDoc::m_shotClock). Declared here until its original
// header is known.
#pragma pack(push, 1)
struct sShotClock {
	unsigned char kind;             // 0x0, 1: the clock is held, 2: released with the bar's accuracy
	float accuracy;                 // 0x1, CGolfDoc::m_bar.m_accuracy and m_prevAccuracy
};
#pragma pack(pop)
CHECK_SIZE(sShotClock, 5);
CHECK_FIELD(sShotClock, accuracy, 0x1);

// A shore wave of the course (CWaveInfo::Load reads them from a file).
struct sWave {
	char m_texture[0x20];   // 0x0, the texture's name
	float m_width;          // 0x20, the second number of the line ("%d %s %f %f"; GetWidth)
	float m_freq;           // 0x24, the third number, scaled by 0.001 on load (GetFreq)
};
CHECK_SIZE(sWave, 0x28);
CHECK_FIELD(sWave, m_texture, 0x0);
CHECK_FIELD(sWave, m_width, 0x20);
CHECK_FIELD(sWave, m_freq, 0x24);

// The course's shore waves, read from a file by Load (golfdoc.cpp:753; Load
// clears the map with _Tree::_Erase).
class CWaveInfo {
	LAYOUT_CHECK_FRIEND;
public:
	CWaveInfo(void) {}
	// golfdoc.h:237
	~CWaveInfo(void) { m_waves.clear(); }
	void Load(const char *name);
	// golfdoc.h:241 (out of line in beach)
	char *GetTexture(int key) { return m_waves[key].m_texture; }
	// golfdoc.h:242
	float GetWidth(int key) { return m_waves[key].m_width; }
	// golfdoc.h:243
	float GetFreq(int key) { return m_waves[key].m_freq; }

private:
	std::map<int, sWave> m_waves;   // 0x0
};
CHECK_SIZE(CWaveInfo, 0xC);

// A river of the course: the four numbers of a line of the file
// ("%d %f %f %f %f", the first being the map key; CRiverInfo::Load).
struct sRiver {
	float m_scrollVelocity; // 0x0
	float m_headTile;       // 0x4
	float m_sideTile;       // 0x8
	float m_depth;          // 0xC
};
CHECK_SIZE(sRiver, 0x10);
CHECK_FIELD(sRiver, m_scrollVelocity, 0x0);
CHECK_FIELD(sRiver, m_headTile, 0x4);
CHECK_FIELD(sRiver, m_sideTile, 0x8);
CHECK_FIELD(sRiver, m_depth, 0xC);

// The course's rivers, read from a file by Load; same shape as CWaveInfo.
class CRiverInfo {
	LAYOUT_CHECK_FRIEND;
public:
	CRiverInfo(void) {}
	// golfdoc.h:263
	~CRiverInfo(void) { m_rivers.clear(); }
	void Load(const char *name);
	// golfdoc.h:267 (out of line in river)
	float GetScrollVelocity(int key) { return m_rivers[key].m_scrollVelocity; }
	// golfdoc.h:268
	float GetHeadTile(int key) { return m_rivers[key].m_headTile; }
	// golfdoc.h:269
	float GetSideTile(int key) { return m_rivers[key].m_sideTile; }
	// golfdoc.h:270
	float GetDepth(int key) { return m_rivers[key].m_depth; }

private:
	std::map<int, sRiver> m_rivers; // 0x0
};
CHECK_SIZE(CRiverInfo, 0xC);

#include "padding.h"

// The round's document, a run-time-typed IObject created through the object
// factory (golfdoc.cpp:24); some members are not transcribed yet.
class CGolfDoc : public CTaskDoc {
	friend IObject *CGolfDocMakeInstance(void);
	friend class CTutorial;     // CheckClubTarget/CheckCameraTarget read m_cameraData
	friend class WPolySoup;     // Load records the missing model files in m_ownedStrings
	friend class CPetFrame;     // SetPartPets records a missing pet model's name in m_ownedStrings
	friend class CConsole;      // the debug commands god, bar and unbalance
	friend class CGolfTask;     // reads m_selectedIndexA, m_clearedShort194 and m_shotResultMsg
	friend class CGolfPlayer;   // HandleMsg 0x10E reads m_clearedShort194 and m_clearedFlag1AC
	friend class CGolfRule;     // HandleMsg 0xA3 toggles m_clearedFlag15A
	friend class CHoleInfo;     // Display prints m_skinsPangTotal and m_skinsCarryCount
	friend class FrAwardForm;   // OnInit reads the room type 7 winner from m_selectedIndexB
	LAYOUT_CHECK_FRIEND;
public:
	static const WRTTI m_RTTI;
	// golfdoc.h:459
	virtual const WRTTI *GetRTTI(void) const { return &m_RTTI; }

	virtual ~CGolfDoc(void);

	// The documents pushed aside while another round's document is current
	// (golfdoc.cpp:27).
	static std::stack<CGolfDoc *, std::list<CGolfDoc *> > m_GolfDocInstanceStack;

protected:
	CGolfDoc(void);

public:
	// golfdoc.h:488
	struct sTarget {
		sTarget();

		class WPuppet *m_cameraTarget; // 0x0, the "camera_target.pet" puppet CTutorial::OnLoad gets (cleared by the constructor)
		WVector m_pos;          // 0x4, the point CTutorial::CheckClubTarget measures the club's reach against
		bool m_unk10;           // 0x10, cleared by the constructor
	};

	// golfdoc.h:517 (defined below the class)
	struct sCameraData &GetCameraData(void);
	WMatrix GetDefaultCamera(void);
	WMatrix MakeTopViewCameraMatrix(bool);
	void MakeDefaultCamera(bool);
	sApproachResultData *GetAppMyCurHoleResult(void);
	sApproachResultData *GetAppMyGameResult(void);
	void SetCurrentPlayerNum(int num);
	unsigned char GetCurrentPlayerNum(void);
	void ClearShotResultMsg(void);
	void LogStrokeScore(void);
	void SetApproachMissionDesc(const char *desc);
	char *GetApproachMissionDesc(void);
	void SetApproachMissionType(eApproachMissionType type);
	void SetApproachPrizeType(eApproachPrizeType type);
	void SetApproachWinnerType(eApproachWinnerType type);
	eApproachMissionType GetApproachMissionType(void) const;
	eApproachPrizeType GetApproachPrizeType(void) const;
	eApproachWinnerType GetApproachWinnerType(void) const;
	std::list<sApproachResultData *> &GetApproachCurHoleResult(void);
	std::list<sApproachResultData *> &GetApproachGameResult(void);
	void AddApproachTotalPrize(unsigned long prize);
	unsigned long GetApproachTotalPrize(void) const;
	void ApproachDataAllClear(void);
	void ClearApproachCurHoleResult(void);
	void SetApproachCurHoleResult(sApproachResultData &data);
	void SetApproachGameResult(sApproachResultData &data);
	void ClearApproachGameResult(void);
	void SendReport(void);
	eTeamName GetTeamGameWinner(void);
	struct sHoleData &GetHoleData(unsigned char hole);
	struct sPlayerData *GetPlayer(unsigned char index);
	struct sTeamData *GetTeam(unsigned char index);
	void MakeTeam(void);
	void ResetTeamVars(void);
	void MakePlayer(unsigned char num);
	void MakeHoleData(void);
	void RefreshPlayerNum(void);
	CPartTidList &GetPartTidList(unsigned char index);
	void InitShotResultMsg(void);
	void UpdateBright(float bright);
	void LoadXml(const char *filename, unsigned char index, bool reload);
	void SetPlayerLevel(unsigned char index);
	void SetPartTidList(unsigned char index, unsigned long tidChar, unsigned char hairClr, unsigned char shirtsClr, unsigned long *partTids, unsigned long *partIds);
	// golfdoc.h:506
	unsigned char GetPlayerNum(void) { return m_playerNum; }
	// golfdoc.h:511
	struct sHoleData &GetHoleData(void) { return m_holeData[m_currentHole - 1]; }
	unsigned char &GetUIAlpha(void);
	struct sBar &GetBar(void);
	unsigned long &GameMode(void);
	void NetGameEnd(void);
	bool IsNetGameEnd(void);
	void SetTeeShotReady(bool ready);
	bool IsTeeShotReady(void);
	void SetPause(bool pause);
	bool IsPause(void);
	void SetTeamGameWinner(eTeamName team);
	void SetReport(bool report);
	bool IsReported(void);
	void ShowChat(bool show);
	bool IsShowChat(void);
	void SetDay(bool day);
	bool GetDay(void);
	void SetSaveChatNum(int num);
	int GetSaveChatNum(void);
	void SetStatUpdated(bool updated);
	bool GetStatUpdated(void) const;
	bool GetApproachReloaded(void);
	void SetApproachReloaded(bool reloaded);

public:
	WPolySoup *m_pPolySoup;                          // 0x8, the course geometry
	CSky *m_pSky;                                    // 0xC, null when the course has no sky (CSceneManager::Render_RenderSky)
	CAutoTarget *m_pAutoTarget;                      // 0x10, owned (deleted by the destructor)
	CPropertyInfo m_propertyInfo;                    // 0x14, the course's properties (WPolySoup::GetProperty)
	CMapInfo m_mapInfo;                              // 0x34, the course's map items (LoadXml)
	CWaveInfo m_waveInfo;                            // 0x40, loaded with the course (ntLobbyBg::OnLoad)
	CRiverInfo m_riverInfo;                          // 0x4C
private:
	WList<char *> m_ownedStrings;                    // 0x58, its items are delete[]d by the destructor
public:
	int m_gameState;                                 // 0x78, the round's progress (CFontOverlay shows overlays from 4)
private:
public:
	std::list<unsigned long> m_commandArrows;        // 0x7C, the arrow commands CScreen::DrawCommandArrow draws (0x10 entries skipped)
	sTarget m_target;                                // 0x88
private:
	std::string m_xmlName;                           // 0x9C, the last course XML LoadXml read (its properties are read again only for another file)
	sPlayerData *m_players;                          // 0xB8, m_playerNum of them
	sTeamData *m_teams;                              // 0xBC, two
	eTeamName m_teamGameWinner;                      // 0xC0
	unsigned char m_currentPlayerNum;                // 0xC4
	unsigned char m_playerNum;                       // 0xC5
	PADDING(0xC6, 0xC8);
	sHoleData *m_holeData;                           // 0xC8
	unsigned char m_uiAlpha;                         // 0xCC
	PADDING(0xCD, 0xD0);
	sBar m_bar;                                      // 0xD0
	sCameraData m_cameraData;                        // 0xF4
	unsigned long m_gameMode;                        // 0x148, flag bits (GameMode); IActor::m_pLayer points at it, bit 0x100 is the byte at 0x149
	bool m_netGameEnd;                               // 0x14C
public:
	bool m_pause;                                    // 0x14D, read directly by CClock::OnProcess
private:
	bool m_teeShotReady;                             // 0x14E
	bool m_report;                                   // 0x14F
public:
	bool m_day;                                      // 0x150, true by default (GetDay/SetDay)
private:
	bool m_showChat;                                 // 0x151
	PADDING(0x152, 0x154);
public:
	int m_chatHistoryIndex;                          // 0x154, ChatManager::SaveChatHistory's in/out history index (the chat dialogs' "savechat" buttons)
public:
	bool m_clearedFlag158;                           // 0x158, cleared by the constructor (role not traced yet)
	bool m_setFlag159;                               // 0x159, set by the constructor (role not traced yet)
private:
	bool m_clearedFlag15A;                           // 0x15A, cleared by the constructor (role not traced yet)
	PADDING(0x15B, 0x15C);
public:
	int m_value15C;                                  // 0x15C, an integer CTutorial::HandleMsg (0x155) compares and copies into m_bar.m_accuracy (role not traced yet)
private:
	bool m_godMode;                                  // 0x160, the console's "god" toggle (then PerformPowerBarSetting)
	float m_debugBar;                                // 0x164, set by the console's "bar" command (138-142, 0 = off)
public:
	unsigned char m_currentHole;                     // 0x168, 1-based (CGolfRuleTeam::CheckGiveUp)
private:
	PADDING(0x169, 0x16C);
public:
	unsigned long m_remainShotTime;                  // 0x16C, the shot clock's remaining milliseconds (CClock)
	int m_shotElapsed;                               // 0x170, the milliseconds accumulated by CClock::OnProcess
	unsigned char m_clockHold;                       // 0x174, nonzero: no warning tick and no timeout (CClock)
	unsigned char m_myPlayerIndex;                   // 0x175, this client's player
private:
	PADDING(0x176, 0x178);
public:
	float m_fogBrightness;                           // 0x178, scales the fog colour read from <map>_fog.txt (CSceneManager::UpdateFog, ModifyColor)
	bool m_dropPending;                              // 0x17C, cleared when the drop dialog is confirmed (CGolfRule::CancelDropDlgResult)
	bool m_reportSent;                               // 0x17D, the round's report was already sent (CGolfRule::OnReportBtnUp)

	unsigned char m_maxGrade;                        // 0x17E, the highest grade of the players (SetGrade)
	bool m_clockStopped;                             // 0x17F, CClock::OnProcess does nothing while set
	bool m_approachMode;                             // 0x180, set by the approach rules' constructors
private:
	PADDING(0x181, 0x184);
	float m_impactAccuracy;                          // 0x184, 1.0 by default; the console's "unbalance" impact (then CClub::SetAccuracy)
	float m_controlAccuracy;                         // 0x188, 15.0 by default; the console's "unbalance" control
	int m_selectedIndexA;                            // 0x18C, -1 (none) by default
	int m_selectedIndexB;                            // 0x190, -1 (none) by default; CGolfRule::HandleMsg 0x80 compares it with the user's guid in room type 7
	short m_clearedShort194;                         // 0x194, cleared by the constructor
	PADDING(0x196, 0x198);
public:
	__int64 m_maxPangPerLoser;                       // 0x198, caps what each other player's pang adds to the winner's (CGolfRule::CalcWinnersPang)
private:
	__int64 m_skinsPangTotal;                        // 0x1A0, pang total CHoleInfo::Display prints in room type 7
	unsigned long m_skinsCarryCount;                 // 0x1A8, count CHoleInfo::Display prints "x%d" when above 1
	unsigned char m_clearedFlag1AC;                  // 0x1AC, cleared by the constructor; a count (SetHoleInPose_Skins compares it with 2, 3 and 4)
	PADDING(0x1AD, 0x1B0);
public:
	__int64 m_totalPang;                             // 0x1B0, the round's total pang (FrScoreForm::OnPangListOwnerDraw)
	__int64 m_winnerPangBonus;                       // 0x1B8, added to the winner's pang (CGolfRule::CalcWinnersPang)
private:
	PADDING(0x1C0, 0x328);
public:
	sRoundResult m_roundResults[4];                  // 0x328, by player slot, zeroed by the constructor
private:
	sShotResult m_shotResultMsg[4];                  // 0x3AC
public:
	sShotProtocol m_shotProtocol;                    // 0x440, zeroed by the constructor; filled by CGhostPlayer::DoShot, applied by CGolfRule::HandleMsg
	sShotClock m_shotClock;                          // 0x46E, zeroed by the constructor; GolfTask packet 0x55 fills it, CGolfRule::HandleMsg 0x7B applies it
private:
	PADDING(0x473, 0x474);
	CPartTidList m_partTidList[4];                   // 0x474

	bool m_statUpdated;                              // 0x794
	bool m_approachReloaded;                         // 0x795
	char m_approachMissionDesc[0x80];                // 0x796
	PADDING(0x816, 0x818);
	eApproachMissionType m_approachMissionType;      // 0x818
	eApproachPrizeType m_approachPrizeType;          // 0x81C
	eApproachWinnerType m_approachWinnerType;        // 0x820
	std::list<sApproachResultData *> m_approachCurHoleResult; // 0x824
	std::list<sApproachResultData *> m_approachGameResult;    // 0x830
	unsigned long m_approachTotalPrize;              // 0x83C
};

BEGIN_LAYOUT_CHECK(CGolfDoc)
	CHECK_FIELD(CGolfDoc, m_gameState, 0x78);
	CHECK_FIELD(CGolfDoc, m_myPlayerIndex, 0x175);
	CHECK_FIELD(CGolfDoc, m_players, 0xB8);
	CHECK_FIELD(CGolfDoc, m_teams, 0xBC);
	CHECK_FIELD(CGolfDoc, m_partTidList, 0x474);
	CHECK_FIELD(CGolfDoc, m_pPolySoup, 0x8);
	CHECK_FIELD(CGolfDoc, m_pSky, 0xC);
	CHECK_FIELD(CGolfDoc, m_pAutoTarget, 0x10);
	CHECK_FIELD(CGolfDoc, m_propertyInfo, 0x14);
	CHECK_FIELD(CGolfDoc, m_mapInfo, 0x34);
	CHECK_FIELD(CGolfDoc, m_xmlName, 0x9C);
	CHECK_FIELD(CGolfDoc, m_commandArrows, 0x7C);
	CHECK_FIELD(CGolfDoc, m_target, 0x88);
	CHECK_SIZE(CGolfDoc::sTarget, 0x14);
	CHECK_FIELD(CGolfDoc::sTarget, m_pos, 0x4);
	CHECK_FIELD(CGolfDoc::sTarget, m_unk10, 0x10);
	CHECK_FIELD(CGolfDoc, m_waveInfo, 0x40);
	CHECK_FIELD(CGolfDoc, m_riverInfo, 0x4C);
	CHECK_FIELD(CGolfDoc, m_shotProtocol, 0x440);
	CHECK_FIELD(CGolfDoc, m_fogBrightness, 0x178);
	CHECK_FIELD(CGolfDoc, m_dropPending, 0x17C);
	CHECK_FIELD(CGolfDoc, m_reportSent, 0x17D);
	CHECK_FIELD(CGolfDoc, m_maxGrade, 0x17E);
	CHECK_FIELD(CGolfDoc, m_clockStopped, 0x17F);
	CHECK_FIELD(CGolfDoc, m_remainShotTime, 0x16C);
	CHECK_FIELD(CGolfDoc, m_shotElapsed, 0x170);
	CHECK_FIELD(CGolfDoc, m_clockHold, 0x174);
	CHECK_FIELD(CGolfDoc, m_approachMode, 0x180);
	CHECK_FIELD(CGolfDoc, m_teamGameWinner, 0xC0);
	CHECK_FIELD(CGolfDoc, m_currentPlayerNum, 0xC4);
	CHECK_FIELD(CGolfDoc, m_playerNum, 0xC5);
	CHECK_FIELD(CGolfDoc, m_currentHole, 0x168);
	CHECK_FIELD(CGolfDoc, m_holeData, 0xC8);
	CHECK_FIELD(CGolfDoc, m_uiAlpha, 0xCC);
	CHECK_FIELD(CGolfDoc, m_bar, 0xD0);
	CHECK_FIELD(CGolfDoc, m_cameraData, 0xF4);
	CHECK_FIELD(CGolfDoc, m_gameMode, 0x148);
	CHECK_FIELD(CGolfDoc, m_netGameEnd, 0x14C);
	CHECK_FIELD(CGolfDoc, m_pause, 0x14D);
	CHECK_FIELD(CGolfDoc, m_teeShotReady, 0x14E);
	CHECK_FIELD(CGolfDoc, m_report, 0x14F);
	CHECK_FIELD(CGolfDoc, m_showChat, 0x151);
	CHECK_FIELD(CGolfDoc, m_chatHistoryIndex, 0x154);
	CHECK_FIELD(CGolfDoc, m_statUpdated, 0x794);
	CHECK_FIELD(CGolfDoc, m_approachReloaded, 0x795);
	CHECK_FIELD(CGolfDoc, m_shotResultMsg, 0x3AC);
	CHECK_FIELD(CGolfDoc, m_approachMissionDesc, 0x796);
	CHECK_FIELD(CGolfDoc, m_approachMissionType, 0x818);
	CHECK_FIELD(CGolfDoc, m_approachPrizeType, 0x81C);
	CHECK_FIELD(CGolfDoc, m_approachWinnerType, 0x820);
	CHECK_FIELD(CGolfDoc, m_approachCurHoleResult, 0x824);
	CHECK_FIELD(CGolfDoc, m_approachGameResult, 0x830);
	CHECK_FIELD(CGolfDoc, m_approachTotalPrize, 0x83C);
	CHECK_FIELD(CGolfDoc, m_ownedStrings, 0x58);
	CHECK_FIELD(CGolfDoc, m_day, 0x150);
	CHECK_FIELD(CGolfDoc, m_clearedFlag158, 0x158);
	CHECK_FIELD(CGolfDoc, m_setFlag159, 0x159);
	CHECK_FIELD(CGolfDoc, m_clearedFlag15A, 0x15A);
	CHECK_FIELD(CGolfDoc, m_value15C, 0x15C);
	CHECK_FIELD(CGolfDoc, m_godMode, 0x160);
	CHECK_FIELD(CGolfDoc, m_debugBar, 0x164);
	CHECK_FIELD(CGolfDoc, m_impactAccuracy, 0x184);
	CHECK_FIELD(CGolfDoc, m_controlAccuracy, 0x188);
	CHECK_FIELD(CGolfDoc, m_selectedIndexA, 0x18C);
	CHECK_FIELD(CGolfDoc, m_selectedIndexB, 0x190);
	CHECK_FIELD(CGolfDoc, m_clearedShort194, 0x194);
	CHECK_FIELD(CGolfDoc, m_maxPangPerLoser, 0x198);
	CHECK_FIELD(CGolfDoc, m_skinsPangTotal, 0x1A0);
	CHECK_FIELD(CGolfDoc, m_skinsCarryCount, 0x1A8);
	CHECK_FIELD(CGolfDoc, m_clearedFlag1AC, 0x1AC);
	CHECK_FIELD(CGolfDoc, m_totalPang, 0x1B0);
	CHECK_FIELD(CGolfDoc, m_winnerPangBonus, 0x1B8);
	CHECK_FIELD(CGolfDoc, m_roundResults, 0x328);
	CHECK_FIELD(CGolfDoc, m_shotClock, 0x46E);
END_LAYOUT_CHECK

// Each macro below is defined empty, before its first include, by the one unit
// whose object kept the out-of-line copy of those inline accessors in the
// original (GOLFDOC_INLINE: GolfDoc.cpp, GOLFRULE_GOLFDOC_INLINE: GolfRule.cpp,
// GOLFPLAYER_GOLFDOC_INLINE: golfplayer.cpp, GOLFTASK_GOLFDOC_INLINE:
// GolfTask.cpp); every other unit inlines them.
#ifndef GOLFDOC_INLINE
#define GOLFDOC_INLINE inline
#endif
#ifndef GOLFRULE_GOLFDOC_INLINE
#define GOLFRULE_GOLFDOC_INLINE inline
#endif
#ifndef GOLFPLAYER_GOLFDOC_INLINE
#define GOLFPLAYER_GOLFDOC_INLINE inline
#endif
// GolfTask.cpp holds IsReported's out-of-line copy.
#ifndef GOLFTASK_GOLFDOC_INLINE
#define GOLFTASK_GOLFDOC_INLINE inline
#endif

// golfdoc.h:514
// golfdoc.h:512
GOLFRULE_GOLFDOC_INLINE sHoleData &CGolfDoc::GetHoleData(unsigned char hole) {
	return m_holeData[hole - 1];
}

GOLFRULE_GOLFDOC_INLINE unsigned char &CGolfDoc::GetUIAlpha(void) {
	return m_uiAlpha;
}

// golfdoc.h:516
GOLFRULE_GOLFDOC_INLINE sBar &CGolfDoc::GetBar(void) {
	return m_bar;
}

// golfdoc.h:517
GOLFDOC_INLINE sCameraData &CGolfDoc::GetCameraData(void) {
	return m_cameraData;
}

// golfdoc.h:519
GOLFRULE_GOLFDOC_INLINE unsigned long &CGolfDoc::GameMode(void) {
	return m_gameMode;
}

// golfdoc.h:526
GOLFRULE_GOLFDOC_INLINE void CGolfDoc::NetGameEnd(void) {
	m_netGameEnd = true;
}

// golfdoc.h:527
GOLFRULE_GOLFDOC_INLINE bool CGolfDoc::IsNetGameEnd(void) {
	return m_netGameEnd;
}

// golfdoc.h:528
GOLFRULE_GOLFDOC_INLINE void CGolfDoc::SetTeeShotReady(bool ready) {
	m_teeShotReady = ready;
}

// golfdoc.h:529
GOLFRULE_GOLFDOC_INLINE bool CGolfDoc::IsTeeShotReady(void) {
	return m_teeShotReady;
}

// golfdoc.h:540
GOLFRULE_GOLFDOC_INLINE void CGolfDoc::SetPause(bool pause) {
	m_pause = pause;
}

// golfdoc.h:541
GOLFRULE_GOLFDOC_INLINE bool CGolfDoc::IsPause(void) {
	return m_pause;
}

// golfdoc.h:542
GOLFRULE_GOLFDOC_INLINE void CGolfDoc::SetTeamGameWinner(eTeamName team) {
	m_teamGameWinner = team;
}

// golfdoc.h:543
GOLFPLAYER_GOLFDOC_INLINE eTeamName CGolfDoc::GetTeamGameWinner(void) {
	return m_teamGameWinner;
}

// golfdoc.h:545
GOLFRULE_GOLFDOC_INLINE void CGolfDoc::SetReport(bool report) {
	m_report = report;
}

// golfdoc.h:546
GOLFTASK_GOLFDOC_INLINE bool CGolfDoc::IsReported(void) {
	return m_report;
}

// golfdoc.h:551
inline void CGolfDoc::SetDay(bool day) {
	m_day = day;
}

// golfdoc.h:552
inline bool CGolfDoc::GetDay(void) {
	return m_day;
}

// golfdoc.h:555
GOLFRULE_GOLFDOC_INLINE void CGolfDoc::ShowChat(bool show) {
	m_showChat = show;
}

// golfdoc.h:556
GOLFRULE_GOLFDOC_INLINE bool CGolfDoc::IsShowChat(void) {
	return m_showChat;
}

// golfdoc.h:559
inline void CGolfDoc::SetSaveChatNum(int num) {
	m_chatHistoryIndex = num;
}

// golfdoc.h:560
inline int CGolfDoc::GetSaveChatNum(void) {
	return m_chatHistoryIndex;
}

// shareddoc.inl's PLAYER and TEAM, defined by SharedDoc.h once CGolfDoc is
// complete.
#include "SharedDoc.h"

#endif // PG_CLIENT_PROJECTG_GOLFDOC_H
