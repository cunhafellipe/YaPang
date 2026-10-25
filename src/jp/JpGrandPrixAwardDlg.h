// JP-only game code (no KR counterpart): FrGrandPrixAwardDlg, the grand
// prix award screen ("GrandPrix_Award"), the only subclass of FrFormBase.
// It is also a CTimerEvent (the "GrandPrixAward" base R577 saw at +0x150:
// sub_99be90 is CTimerEvent::CTimerEvent, vtable 0xd3e98c, RTTI
// ".?AVCTimerEvent@@" 0xe0bc00; its other JP subclass is
// CGPRoomTimerSteward, ctor 0x55d180, vtable 0xd0a1a4). Its lua script is
// "grandprix_award_ui.lua" (tables "grandprix_award_ui_main" and
// "grandprix_award_ui_cutscene": bg_file_count, bg_file_name, bg_pos,
// char%d_pic_pos, char%d_name_pos ... read by IsReady 0x54a9f0).
//
// JP x86 facts: RTTI 0xe75130 ".?AVFrGrandPrixAwardDlg@@" (base FrFormBase
// 0xe74d94), size 0x2e0, vtables 0xd09c8c (IObject/FrWnd, 42 slots),
// 0xd09c7c (FrCmdTarget), 0xd09c74, 0xd09c68 (CTimerEvent at +0x150);
// message map 0xe0b564 (base FrFormBase 0xe08570, rows 0xe0b588 stride
// 0x20, ~45 rows: most are "INIT" rows on FrFormBase::OnControlInit
// 0x4fedd0, the handlers stored by the static init 0xccdfc0; later rows
// point at 0x548da0, 0x547b40 ...). Timer name: the static std::string
// 0xe0b56c = "grandprix_award_ui" (static init 0xcce470).
//
// Opener chain: the grand prix award scene step 0x6a55b0 (sets up "GolfBg",
// "Score", the "DummyPlayer" actor and the award camera) creates the dialog
// when s_pAwardDlg (0xe75128) is NULL (OpenGrandPrixAwardDlg 0x54f470),
// then calls 0x548d10 (camera time, "") on it and opens it (slot 25).
// No packet handler opens it directly.
#ifndef PG_JP_JPGRANDPRIXAWARDDLG_H
#define PG_JP_JPGRANDPRIXAWARDDLG_H

#include <stdint.h>
#include <string>
#include <vector>

#include "JpFormBase.h"
#include "RefreshSharedPtr.h"
#include "defines.h"
#include "padding.h"
#include "wtimer.h"

class Bitmap;
class FrArea;
class FrStatic;
class FrForm;
class CSimpleUI;
class FrGraphicInterface;
class WTitleFontEx;
struct sRoomInfo;

// One reward of the award (OnInit 0x54d110 fills them from the grand prix
// data; CGrandPrixRewardList::Draw shows them).
struct sAwardReward {
	enum {
		SHOW_COUNT = 0,         // Draw prints count with the slot's font
		SHOW_BITMAP = 1         // Draw draws the slot's item bitmap
	};
	unsigned int typeId;    // 0x0 (Draw: the item-class offset for 0x18000000 ids)
	int kind;               // 0x4 (SHOW_COUNT / SHOW_BITMAP)
	int count;              // 0x8 (printed "%d" for SHOW_COUNT)
};
CHECK_SIZE(sAwardReward, 0xc);

// The 0x288-byte reward list the dialog owns (ctor 0x54be50; class name
// invented, no RTTI read yet). Draw shows up to m_itemCount slots in three
// steps: 0 the card backs, 1 the reward items fading in (m_colors), 2 the
// rewards turned over (count UI rolling, count text or item bitmap).
class CGrandPrixRewardList {
	LAYOUT_CHECK_FRIEND;
public:
	CGrandPrixRewardList(void);                                       // 0x54be50

	// Draws the list into gdi, advanced by dt (the "rewardlist" owner draw).
	void Draw(FrGraphicInterface* gdi, float dt);                     // 0x547ca0
	// Moves the list's origin to (x, y) and lays the item boxes
	// ("th_item_box_n") out from it, five to a row.
	void SetPosition(int x, int y);                                   // 0x547390
	// Copies the rewards in (5 boxes, 10 for more than 5), reads the item
	// layout, builds the rolling count UI and gives every slot its card,
	// its item bitmap (fitted into the item size) and its count font or
	// period mark.
	void SetRewards(const std::vector<sAwardReward>* rewards);        // 0x54cab0
	// Reads "grandprix_award_ui_rewardbox_item" of grandprix_award_ui.lua:
	// the gaps, the offsets, the two step delays and the item size.
	void LoadItemLayout(void);                                        // 0x549410

	// An item box's top-left corner (also the offsets read from the lua).
	struct sItemPos {
		float x;            // 0x0
		float y;            // 0x4
	};
	// A slot's bitmap for each of the three steps.
	struct sSlotBitmaps {
		const Bitmap* step[3];  // 0x0
	};

protected:
	// A slot was drawn: when last, starts the step delay (m_step0Delay in
	// step 0, m_step1Delay in step 1, unchanged in step 2) unless pending.
	void OnSlotDrawn(bool last);                                      // 0x546d20

public:
	bool m_shown;           // 0x0 (Draw draws only while set; SetListRankVisible)
	bool m_ready;           // 0x1 (OnProc waits for it before showing the next winner)
	UNKNOWN(0x2, 0x4);
	RF_SHARED_PTR<CSimpleUI> m_pCountUI;   // 0x4 (x86 0x8: ptr, ref block; step 2: the rolling "%d" count, SetRewards)
	const Bitmap* m_pBox;           // 0xc (a slot's box)
	const Bitmap* m_pRewardBox;     // 0x10 (the box of a slot holding a reward, step 2)
	sSlotBitmaps m_slotBitmaps[10]; // 0x14
	const Bitmap* m_itemBitmaps[10];        // 0x8c (sAwardReward::SHOW_BITMAP)
	RF_SHARED_PTR<WTitleFontEx> m_countFonts[10];   // 0xb4 (x86 0x8 each; sAwardReward::SHOW_COUNT)
	// 0x104: three float pairs (roles not traced). The constructor's loop
	// writes a pair for each of its ten iterations: an overrun of the
	// original into +0x11c..+0x150 (it leaves the -1 only in the dwords at
	// +0x148 / +0x14c). The port does not reproduce it.
	UNKNOWN(0x104, 0x11c);
	int m_step;             // 0x11c (Draw: 0..2, advanced when m_stepDelay runs out)
	bool m_stepPending;     // 0x120 (Draw counts m_stepDelay down while set)
	float m_stepDelay;      // 0x124
	unsigned long m_colors[10];     // 0x128 (a slot's diffuse colour)
	int m_widths[10];       // 0x150 (a slot's item size, centred in the box)
	int m_heights[10];      // 0x178
	float m_boxGapX;        // 0x1a0 (SetPosition: the gap before and between the boxes; lua RewardItemPaddingX)
	float m_boxGapY;        // 0x1a4 (lua RewardItemPaddingY)
	sItemPos m_itemOffset;      // 0x1a8 (the centred reward item, steps 1-2; lua ItemPosX/Y)
	sItemPos m_classOffset;     // 0x1b0 (added for 0x18000000 type ids; lua ItemOffsetX/Y)
	sItemPos m_cardOffset;      // 0x1b8 (step 0 and the cards not turned yet; lua ReadyPosX/Y)
	sItemPos m_emptyOffset;     // 0x1c0 (the slots past the rewards; lua EmptyPosX/Y)
	sItemPos m_countUIOffset;   // 0x1c8 (lua SmokePosX/Y)
	sItemPos m_countTextOffset; // 0x1d0 (lua ItemDigitPos)
	sItemPos m_itemBitmapOffset;    // 0x1d8 (lua ItemPeriodImgPos)
	int m_boxWidth;         // 0x1e0 (lua ItemWidth: SetRewards fits the item bitmaps into it)
	int m_boxHeight;        // 0x1e4 (lua ItemHeight)
	float m_step0Delay;     // 0x1e8 (OnSlotDrawn; lua ReadyPlayTime)
	float m_step1Delay;     // 0x1ec (lua MaskPlayTime)
	float m_x;              // 0x1f0 (SetPosition)
	float m_y;              // 0x1f4
	sItemPos m_itemPos[10];     // 0x1f8 (Draw walks it up to 0x248)
	float m_counts[10];     // 0x248 (step 2: the rolling count, +15 per second up to 8)
	float m_stepTime;       // 0x270 (Draw adds dt each frame; 0 when m_step advances)
	std::vector<sAwardReward> m_rewards;    // 0x274 (x86 0x10; Draw divides end - begin by 12)
	int m_itemCount;        // 0x284 (the boxes SetPosition lays out and Draw draws)
};
CHECK_SIZE(CGrandPrixRewardList, 0x288);
BEGIN_LAYOUT_CHECK(CGrandPrixRewardList)
	CHECK_FIELD(CGrandPrixRewardList, m_shown, 0x0);
	CHECK_FIELD(CGrandPrixRewardList, m_ready, 0x1);
	CHECK_FIELD(CGrandPrixRewardList, m_pCountUI, 0x4);
	CHECK_FIELD(CGrandPrixRewardList, m_pBox, 0xc);
	CHECK_FIELD(CGrandPrixRewardList, m_pRewardBox, 0x10);
	CHECK_FIELD(CGrandPrixRewardList, m_slotBitmaps, 0x14);
	CHECK_FIELD(CGrandPrixRewardList, m_itemBitmaps, 0x8c);
	CHECK_FIELD(CGrandPrixRewardList, m_countFonts, 0xb4);
	CHECK_FIELD(CGrandPrixRewardList, m_step, 0x11c);
	CHECK_FIELD(CGrandPrixRewardList, m_stepPending, 0x120);
	CHECK_FIELD(CGrandPrixRewardList, m_stepDelay, 0x124);
	CHECK_FIELD(CGrandPrixRewardList, m_colors, 0x128);
	CHECK_FIELD(CGrandPrixRewardList, m_widths, 0x150);
	CHECK_FIELD(CGrandPrixRewardList, m_heights, 0x178);
	CHECK_FIELD(CGrandPrixRewardList, m_boxGapX, 0x1a0);
	CHECK_FIELD(CGrandPrixRewardList, m_boxGapY, 0x1a4);
	CHECK_FIELD(CGrandPrixRewardList, m_itemOffset, 0x1a8);
	CHECK_FIELD(CGrandPrixRewardList, m_classOffset, 0x1b0);
	CHECK_FIELD(CGrandPrixRewardList, m_cardOffset, 0x1b8);
	CHECK_FIELD(CGrandPrixRewardList, m_emptyOffset, 0x1c0);
	CHECK_FIELD(CGrandPrixRewardList, m_countUIOffset, 0x1c8);
	CHECK_FIELD(CGrandPrixRewardList, m_countTextOffset, 0x1d0);
	CHECK_FIELD(CGrandPrixRewardList, m_itemBitmapOffset, 0x1d8);
	CHECK_FIELD(CGrandPrixRewardList, m_boxWidth, 0x1e0);
	CHECK_FIELD(CGrandPrixRewardList, m_boxHeight, 0x1e4);
	CHECK_FIELD(CGrandPrixRewardList, m_step0Delay, 0x1e8);
	CHECK_FIELD(CGrandPrixRewardList, m_step1Delay, 0x1ec);
	CHECK_FIELD(CGrandPrixRewardList, m_x, 0x1f0);
	CHECK_FIELD(CGrandPrixRewardList, m_y, 0x1f4);
	CHECK_FIELD(CGrandPrixRewardList, m_itemPos, 0x1f8);
	CHECK_FIELD(CGrandPrixRewardList, m_counts, 0x248);
	CHECK_FIELD(CGrandPrixRewardList, m_stepTime, 0x270);
	CHECK_FIELD(CGrandPrixRewardList, m_rewards, 0x274);
	CHECK_FIELD(CGrandPrixRewardList, m_itemCount, 0x284);
END_LAYOUT_CHECK

// A control the rank steps show or fade (the constructor's tables): its
// name and when it applies.
struct sAwardControl {
	enum {
		SHOW_ALWAYS = 0,
		SHOW_TOP3 = 1,          // the player's rank is 1-3
		SHOW_OTHER = 2          // the player's rank is above 3
	};
	sAwardControl(const std::string& name_, int condition_) : name(name_), condition(condition_) {}

	std::string name;       // 0x0 (x86 0x1c)
	int condition;          // 0x1c
};
CHECK_SIZE(sAwardControl, 0x20);

// The JP record CItemManager::FindMatch (CSharedDoc +0x20c, map +0xc0)
// returns to OnInit 0x54d110 is the KR IFF_STRUCT::sMatch (classdefine.h:
// name +0x8, trophy icon +0x59, the two strings ShowNextWinner reads).
namespace IFF_STRUCT {
struct sMatch;
}

// The grand prix record an entry points at (the JP grand prix table; name
// ours): its name, conditions, schedule, the five rewards every player
// gets and its type id.
struct sJpGrandPrixName {
	UNKNOWN(0x0, 0x4);
	uint32_t id;                // 0x4 (the room list's main-actor message 0x34; JpRoomListState.md, R921)
	uint32_t flags;             // 0x8 (the grand prix type id; bits 19..25: the sort)
	uint32_t listedId;          // 0xc (0 none; else enabled only while in CGrandPrixDocument's listed ids, 0x555260)
	UNKNOWN(0x10, 0x12);
	char name[1];               // 0x12 (length not traced)
	UNKNOWN(0x13, 0x54);
	uint32_t requiredItemTypeId;    // 0x54 (0x55e310: the inventory item 0x63b570 looks up)
	int requiredItemCount;      // 0x58 (0x55e310: that item's count must reach it)
	UNKNOWN(0x5c, 0x86);
	uint8_t hasNote;            // 0x86 (0x55e890: appends text 0x18769)
	uint8_t noteKind;           // 0x87 (0x55e890: 1 appends text 0x7548, 2 text 0x7549)
	uint32_t noteItemTypeId;    // 0x88 (0x55e890: the item description 0xa19320 appended)
	UNKNOWN(0x8c, 0x95);
	uint8_t minLevel;           // 0x95 (0x55e160: a lower own level sets entry flags 1)
	uint8_t maxLevel;           // 0x96 (0 none; a higher own level sets entry flags 2)
	UNKNOWN(0x97, 0x98);
	float minValue;             // 0x98 (0x55dff0: CGrandPrixDocument +0x5c below it sets entry flags 4)
	float maxValue;             // 0x9c (0 none; +0x5c above it sets entry flags 8)
	UNKNOWN(0xa0, 0xb4);
	uint32_t rewardTypeIds[5];  // 0xb4 (OnInit skips a 0)
	int rewardCounts[5];        // 0xc8 (sAwardReward::count)
	int rewardKinds[5];         // 0xdc (sAwardReward::kind)
	// The schedule (0x55e1f0): times of day, put on the server time's date
	// (0xb30790). The room list's double click on sort 0x63 wants all three
	// set (first dword non-zero).
	SYSTEMTIME openTime;        // 0xf0 (sJpGrandPrixEntry::openTime; state 2 from it)
	SYSTEMTIME closeTime;       // 0x100 (state 3 from it)
	SYSTEMTIME endTime;         // 0x110 (state 4 from it; before closeTime: the minutes of the two)
	UNKNOWN(0x120, 0x124);
	uint32_t typeId;            // 0x124 (0x553dd0 compares it; CollectEntries: the entry of this type id)
};
BEGIN_LAYOUT_CHECK(sJpGrandPrixName)
	CHECK_FIELD(sJpGrandPrixName, id, 0x4);
	CHECK_FIELD(sJpGrandPrixName, flags, 0x8);
	CHECK_FIELD(sJpGrandPrixName, listedId, 0xc);
	CHECK_FIELD(sJpGrandPrixName, name, 0x12);
	CHECK_FIELD(sJpGrandPrixName, requiredItemTypeId, 0x54);
	CHECK_FIELD(sJpGrandPrixName, requiredItemCount, 0x58);
	CHECK_FIELD(sJpGrandPrixName, hasNote, 0x86);
	CHECK_FIELD(sJpGrandPrixName, noteKind, 0x87);
	CHECK_FIELD(sJpGrandPrixName, noteItemTypeId, 0x88);
	CHECK_FIELD(sJpGrandPrixName, minLevel, 0x95);
	CHECK_FIELD(sJpGrandPrixName, maxLevel, 0x96);
	CHECK_FIELD(sJpGrandPrixName, minValue, 0x98);
	CHECK_FIELD(sJpGrandPrixName, maxValue, 0x9c);
	CHECK_FIELD(sJpGrandPrixName, rewardTypeIds, 0xb4);
	CHECK_FIELD(sJpGrandPrixName, openTime, 0xf0);
	CHECK_FIELD(sJpGrandPrixName, closeTime, 0x100);
	CHECK_FIELD(sJpGrandPrixName, endTime, 0x110);
	CHECK_FIELD(sJpGrandPrixName, typeId, 0x124);
END_LAYOUT_CHECK
// A grand prix entry of the contents container 0xaf (the value of its map
// at +0x30, node +0x18: 0x5551f0 finds one by key, 0x553dd0 by its
// record's type id). 0x48 bytes, 8-aligned (openTime).
struct sJpGrandPrixEntry {
	// flags bits (0x55e160, 0x55dff0, 0x55ec80, 0x55e310).
	enum {
		FLAG_LEVEL_LOW = 0x1,
		FLAG_LEVEL_HIGH = 0x2,
		FLAG_VALUE_LOW = 0x4,
		FLAG_VALUE_HIGH = 0x8,
		FLAG_REQUIREMENT = 0x10,
		FLAG_REQUIRED_ITEM = 0x80
	};
	// state (0x55e1f0).
	enum {
		STATE_WAITING = 0,      // before openTime
		STATE_CURRENT = 1,      // before openTime, the document's current grand prix
		STATE_OPEN = 2,         // openTime reached (also a sort-0 record)
		STATE_CLOSED = 3,       // closeTime reached
		STATE_ENDED = 4         // endTime reached
	};
	// value20 bits (0x55e190, 0x55e1c0, CollectEntries).
	enum {
		INFO_HAS_DATA = 0x1,            // 0xa17b50 has the type id
		INFO_HAS_RANK_REWARDS = 0x2,    // 0xa17bd0 has the type id
		INFO_PREREQUISITE_DONE = 0x4    // the entry of record typeId has no remaining count
	};
	sJpGrandPrixName* record;   // 0x0
	const sRoomInfo* room;      // 0x4 (NULL: no room; the room list sends packet 0x2d with its +0x59 room number)
	uint32_t flags;             // 0x8 (FLAG_*; CollectEntries clears it; the room list's double click clears 0x180)
	int remainingCount;         // 0xc (OnInit looks the next entry up only while <= 0; role not traced further)
	int state;                  // 0x10 (STATE_*; the room list selects the first entry with 2)
	UNKNOWN(0x14, 0x18);
	int64_t openTime;           // 0x18 (0x55e1f0: record openTime in seconds since 1970; the sort key)
	uint16_t value20;           // 0x20 (INFO_*; the room list's bit test 0x598960(v, 4))
	UNKNOWN(0x22, 0x24);
	std::string description;    // 0x24 (x86 0x1c; 0x55e890)
	bool flag40;                // 0x40 (enabled: CollectEntries updates and lists only these)
};
BEGIN_LAYOUT_CHECK(sJpGrandPrixEntry)
	CHECK_FIELD(sJpGrandPrixEntry, record, 0x0);
	CHECK_FIELD(sJpGrandPrixEntry, room, 0x4);
	CHECK_FIELD(sJpGrandPrixEntry, flags, 0x8);
	CHECK_FIELD(sJpGrandPrixEntry, remainingCount, 0xc);
	CHECK_FIELD(sJpGrandPrixEntry, state, 0x10);
	CHECK_FIELD(sJpGrandPrixEntry, openTime, 0x18);
	CHECK_FIELD(sJpGrandPrixEntry, value20, 0x20);
	CHECK_FIELD(sJpGrandPrixEntry, description, 0x24);
	CHECK_FIELD(sJpGrandPrixEntry, flag40, 0x40);
END_LAYOUT_CHECK
CHECK_SIZE(sJpGrandPrixEntry, 0x48);

// One result row of the contents container 0xaf (its vector at +0x40 /
// +0x44, 0xe4 bytes each): the grand prix ranking OnInit walks.
struct sJpGrandPrixResult {
	uint32_t uid;               // 0x0 (a room player's uid, or the NPC id when !isRoomPlayer)
	int rank;                   // 0x4 (1-3 get a podium picture)
	int isRoomPlayer;           // 0x8 (0: an NPC of the grand prix, 0xa17cb0)
	UNKNOWN(0xc, 0xe4);
};
CHECK_SIZE(sJpGrandPrixResult, 0xe4);

// A player of the JP room's result list (CSharedDoc +0x1042c / +0x10430,
// 0x1b8 bytes each) is the KR sRivalData (CSharedDoc::m_gameResultList,
// the pairing of JpRoomTypeDlgs.cpp): JP +0x0 uid = KR m_id (compared with
// MyUID, as KR FrResultDlg does), +0x8 nickname = m_nickname, +0x152 rank =
// m_rank. JP +0x1ac (the character type id OnInit hands to FindChar) has
// no KR member.
struct sRivalData;

// A grand prix NPC (0xa17cb0 on CSharedDoc +0x20c, by the low 16 bits of
// its id: the value of the JP CItemManager map +0x4c4).
struct sJpGrandPrixNpc {
	UNKNOWN(0x0, 0x8);
	char name[0x28];            // 0x8 (length not traced past 0x30)
	uint32_t characterId;       // 0x30 (OR 0x4000000 = the character type id)
};

// The rewards of one rank (0xa17bd0 on CSharedDoc +0x20c: a vector of
// 0x4c-byte rows; OnInit takes the row of the player's rank).
struct sJpGrandPrixRankReward {
	UNKNOWN(0x0, 0x8);
	int rank;                   // 0x8
	uint32_t typeIds[5];        // 0xc
	int counts[5];              // 0x20
	int kinds[5];               // 0x34
	UNKNOWN(0x48, 0x4c);
};
CHECK_SIZE(sJpGrandPrixRankReward, 0x4c);

// A position read from grandprix_award_ui.lua.
struct sAwardPos {
	float x;        // 0x0
	float y;        // 0x4
};
CHECK_SIZE(sAwardPos, 0x8);

// A text anchor read from grandprix_award_ui.lua: position and scale.
struct sAwardTextPos {
	float x;        // 0x0
	float y;        // 0x4
	float scale;    // 0x8
};
CHECK_SIZE(sAwardTextPos, 0xc);

class FrGrandPrixAwardDlg : public FrFormBase, public CTimerEvent {
	LAYOUT_CHECK_FRIEND;
public:
	static const WRTTI m_RTTI;                                        // 0xe75130
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }   // [0] 0x54dbf0

	// Registers itself as s_pAwardDlg through SetSubForm(this), creates
	// the reward list and builds the control tables m_listControls and
	// m_myRankControls.
	FrGrandPrixAwardDlg(void);                                        // 0x54dc90
	// Clears the "grandprix_award_ui" timer (when the timer manager
	// exists), zeroes the global 0xe7512c, ClearSubForm(), then destroys
	// the members (the vectors at 0x294/0x284 through 0x54d890).
	virtual ~FrGrandPrixAwardDlg(void);                               // [1] 0x54f2e0 (scalar deleting), 0x54da40

	// Reads grandprix_award_ui.lua ("grandprix_award_ui_main" and
	// "_cutscene"): moves the "area_bg%d" backgrounds by bg_pos (the first
	// one's corner is m_bgX/m_bgY) and keeps the pictures', names', rank
	// digits' and trophies' positions. True on every path.
	virtual bool IsReady(void);                                       // [40] 0x54a9f0
	// Keeps the dialog in s_pAwardDlg, then FrFormBase::SetSubForm.
	virtual FrFormBase* SetSubForm(FrFormBase* pSubForm);             // [38] 0x546b60
	// Forgets s_pAwardDlg and the sub-form.
	virtual void ClearSubForm(void);                                  // [39] 0x546b80

	// CTimerEvent: on the "grandprix_award_ui" timer, sets m_state to 2,
	// copies +0x1f8 into +0x2cc, clears the timer and empties m_name.
	virtual void OnTimer(void);                                       // CTimerEvent [1] 0x549210

	// A winner finished coming on (0x671310 calls it on s_pAwardDlg):
	// counts it and waits 0.2 s before the next, m_lastWinnerDelay after
	// the third.
	void OnWinnerShown(void);                                         // 0x546cd0

	// Starts the award timer (CTimerEvent::AddTimer) of time seconds,
	// named name, or "grandprix_award_ui" for an empty name.
	void StartAwardTimer(float time, std::string name);               // 0x548d10

	// The award dialog that is open, NULL for none.
	static FrGrandPrixAwardDlg* s_pAwardDlg;                          // 0xe75128

protected:
	virtual void OnProc(const float dt);                              // [9] 0x54a2e0
	// Sends the JP game statistics once (packet 6), puts the three
	// winners' pictures and names up, gathers the rewards and the notices'
	// data, then starts the "my rank" step.
	virtual bool OnInit(void);                                        // [32] 0x54d110

	// The control the message map registered under name, NULL for none
	// ("char%d" pictures / "char%d_name" texts).
	FrArea* FindArea(std::string name);                               // 0x54a530
	FrStatic* FindStatic(std::string name);                           // 0x54a690
	// The same for the "rewardlist" control (its class not traced).
	FrWnd* FindControl(std::string name);                             // 0x54a7f0
	// Reads "grandprix_award_ui_rewardbox" (box_pos, box_width,
	// box_height1/2, padding_w/h, rewardbox_show_delaytime).
	void LoadRewardBoxLayout(void);                                   // 0x549290
	// Places "char%d" / "char%d_name", "myrank123" / "myrank4" (shown for
	// the player's rank), the "outback%d" frame on the reward box, the
	// "fbar0/1" bars and the "inback%d" frame and "rewardlist" inside it.
	void LayoutAwardControls(void);                                   // 0x54c360
	// Lays the nine controls format % 0..8 out as a 3x3 frame of
	// width x height at (x, y): the corners keep the sizes of controls 0
	// and 8, the middle row / column stretch.
	void LayoutNinePatch(const char* format, float x, float y, int width, int height);  // 0x54b900
	// Jumps to 0x54baf0: "trophy" ("gp_awards_winner_%d") centred on the
	// screen and its "trophy_attr%d" pictures for ranks 1-3, the
	// "trophy_other%d" ones above 3. True.
	bool SetupTrophies(void);                                         // 0x54c7d0

	// Countdowns of OnProc, each decremented by dt; true when it runs out
	// (the 0x2c8 one: true while time was left, as the original tests it).
	bool UpdateCloseDelay(float dt);                                  // 0x546ba0 (only while > 0)
	bool UpdateMyRankDelay(float dt);                                 // 0x546bf0 (only while m_drawMyRank)
	bool UpdateStepTime(float dt);                                    // 0x546c40 (only while !m_drawListRank)
	bool UpdateNextDelay(float dt);                                   // 0x546c90
	// Set m_drawListRank / m_drawMyRank to show and update the rank
	// controls: each control of the table that applies to m_myRank is
	// shown / hidden, faded when bFade (one that does not apply is hidden).
	void SetListRankVisible(bool show, bool bFade);                   // 0x549a90
	void SetMyRankVisible(bool show, bool bFade);                     // 0x549c20
	// Brings the next of the three winners on: a "notify_ok2" notice
	// (match, reward rate, grand prix name), or OnWinnerShown at once
	// when there is nothing to tell.
	void ShowNextWinner(void);                                        // 0x549d80
	// The notice's result callback: OnWinnerShown on s_pAwardDlg. The
	// original's class is not traced (its FrCmdTarget sits at +0x18 of
	// it, the 16-byte member pointer's adjustor -0x18); it reads no member.
	bool OnWinnerNoticeResult(int ret, FrForm* form);                 // 0x671310

	// "common_area" owner draw: the player's rank number ("%d") at the
	// list anchor (rank > 3, while m_drawListRank) and at the "myrank"
	// anchor (m_drawMyRank: myrank4's for rank > 3, myrank123's for 1-3).
	void OnCommonAreaDraw(PG_MSGARG param);                           // 0x547b40
	// "rewardlist" owner draw: draws the reward list.
	void OnRewardListDraw(PG_MSGARG param);                           // 0x548da0

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe0b588

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe0b564
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;          // FrCmdTarget [1] 0x546b90

	// +0x150: CTimerEvent (0x20)
	bool m_done;                // 0x170 (the ctor clears it; OnProc sets it)
	UNKNOWN(0x171, 0x174);
	RF_SHARED_PTR<CGrandPrixRewardList> m_pRewardList;   // 0x174 (x86 0x8: ptr, ref block; ctor 0x54be50)
	float m_bgX;                // 0x17c (IsReady: "bg_pos"; LoadRewardBoxLayout places the reward box from it)
	float m_bgY;                // 0x180
	sAwardPos m_charPicPos[3];      // 0x184 (IsReady: char%d_pic_pos + bg; "char%d")
	sAwardPos m_charNamePos[3];     // 0x19c (char%d_name_pos + bg; "char%d_name")
	sAwardPos m_myRankPicPos[4];    // 0x1b4 (myrank%d_pic_pos + bg; "myrank123" by rank 1-3, "myrank4")
	sAwardTextPos m_listRankPos;    // 0x1d4 (rank > 3 in the list)
	float m_rewardBoxX;         // 0x1e0 ("grandprix_award_ui_rewardbox" box_pos + m_bgX; LayoutAwardControls)
	float m_rewardBoxY;         // 0x1e4 (box_pos + m_bgY)
	int m_rewardBoxWidth;       // 0x1e8 (box_width)
	int m_rewardBoxHeight;      // 0x1ec (box_height1, box_height2 for more than 5 rewards)
	int m_rewardBoxPaddingW;    // 0x1f0 (padding_w)
	int m_rewardBoxPaddingH;    // 0x1f4 (padding_h)
	float m_stepDuration;       // 0x1f8 (rewardbox_show_delaytime; OnTimer: copied into m_stepTimeLeft when state 2 starts)
	float m_lastWinnerDelay;    // 0x1fc (OnWinnerShown, after the third winner; lua close_delaytime)
	sAwardPos m_trophyPicPos;       // 0x200 (IsReady: trophy_pos + bg, truncated, + trophy_pic_pos)
	sAwardPos m_trophyAttrPos[2];   // 0x208 (trophy_attr%d_pos)
	sAwardPos m_trophyOtherPos[2];  // 0x218 (trophy_other%d_pos)
	sAwardTextPos m_myRank123Pos;   // 0x228 (ranks 1-3, centred)
	sAwardTextPos m_myRank4Pos;     // 0x234 (rank > 3, left aligned)
	const IFF_STRUCT::sMatch* m_matchRecord;  // 0x240 (OnInit: CItemManager::FindMatch)
	float m_rewardRate;         // 0x244 (OnInit; the second notice shows it x 100)
	int m_rewardGrade;          // 0x248 (OnInit: 1-3, else 0)
	std::string m_grandPrixName;    // 0x24c (x86 0x1c; OnInit)
	sJpGrandPrixEntry* m_grandPrixEntry;    // 0x268 (OnInit 0x553dd0; the third notice)
	RF_SHARED_PTR<WTitleFontEx> m_pRankFont;  // 0x26c (x86 0x8: ptr, ref block)
	std::vector<sAwardReward> m_rewards;      // 0x274 (x86 0x10)
	std::vector<sAwardControl> m_myRankControls;  // 0x284 (x86 0x10; SetMyRankVisible: the trophies)
	std::vector<sAwardControl> m_listControls;    // 0x294 (x86 0x10; SetListRankVisible)
	int m_state;                // 0x2a4 (OnTimer: 2)
	int m_shownWinners;         // 0x2a8 (ctor 0; OnProc: next winner while < 3)
	bool m_nextPending;         // 0x2ac
	UNKNOWN(0x2ad, 0x2b0);
	float m_animTime;           // 0x2b0 (ctor 0; no other reader found)
	float m_animScale;          // 0x2b4 (ctor 1; no other reader found)
	bool m_drawListRank;        // 0x2b8 (also stops the state 2 countdown 0x546c40)
	bool m_drawMyRank;          // 0x2b9 (also gates the countdown 0x546bf0 of +0x2c4)
	UNKNOWN(0x2ba, 0x2bc);
	int m_myRank;               // 0x2bc
	float m_frameDt;            // 0x2c0 (OnProc: the frame's dt)
	float m_myRankDelay;        // 0x2c4 (IsReady: cutscene_playtime)
	float m_nextDelay;          // 0x2c8 (OnProc sets 1)
	float m_stepTimeLeft;       // 0x2cc (state 2 countdown)
	float m_closeDelay;         // 0x2d0
	UNKNOWN(0x2d4, 0x2e0);
};
// size 0x2e0 (x86)
CHECK_SIZE(FrGrandPrixAwardDlg, 0x2e0);
BEGIN_LAYOUT_CHECK(FrGrandPrixAwardDlg)
	CHECK_FIELD(FrGrandPrixAwardDlg, m_done, 0x170);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_pRewardList, 0x174);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_bgX, 0x17c);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_bgY, 0x180);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_charPicPos, 0x184);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_charNamePos, 0x19c);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_myRankPicPos, 0x1b4);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_listRankPos, 0x1d4);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_rewardBoxX, 0x1e0);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_rewardBoxY, 0x1e4);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_rewardBoxWidth, 0x1e8);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_rewardBoxHeight, 0x1ec);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_rewardBoxPaddingW, 0x1f0);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_rewardBoxPaddingH, 0x1f4);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_stepDuration, 0x1f8);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_lastWinnerDelay, 0x1fc);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_trophyPicPos, 0x200);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_trophyAttrPos, 0x208);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_trophyOtherPos, 0x218);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_myRank123Pos, 0x228);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_myRank4Pos, 0x234);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_matchRecord, 0x240);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_rewardRate, 0x244);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_rewardGrade, 0x248);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_grandPrixName, 0x24c);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_grandPrixEntry, 0x268);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_rewards, 0x274);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_myRankControls, 0x284);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_listControls, 0x294);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_animTime, 0x2b0);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_animScale, 0x2b4);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_pRankFont, 0x26c);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_state, 0x2a4);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_shownWinners, 0x2a8);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_nextPending, 0x2ac);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_drawListRank, 0x2b8);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_drawMyRank, 0x2b9);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_myRank, 0x2bc);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_frameDt, 0x2c0);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_myRankDelay, 0x2c4);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_nextDelay, 0x2c8);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_stepTimeLeft, 0x2cc);
	CHECK_FIELD(FrGrandPrixAwardDlg, m_closeDelay, 0x2d0);
END_LAYOUT_CHECK

// 0xe7512c: set by the award scene (0x69cd90), read by OnInit: the
// match's type id (passed to 0xa173e0).
extern uint32_t data_e7512c;

// 0x54f3e0: make instance (new FrGrandPrixAwardDlg).
IObject* FrGrandPrixAwardDlgMakeInstance(void);
// 0x54f310: CreateForm<FrGrandPrixAwardDlg>.
FrGrandPrixAwardDlg* CreateGrandPrixAwardDlg(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent);
// 0x54f470: creates the "GrandPrix_Award" form (no parent). The award
// scene 0x6a55b0 calls it with the Fresh window manager (0xf022d0 +0x40)
// and no owner when s_pAwardDlg is NULL.
FrGrandPrixAwardDlg* OpenGrandPrixAwardDlg(FrWndManager* manager, FrCmdTarget* owner);

#endif // PG_JP_JPGRANDPRIXAWARDDLG_H
