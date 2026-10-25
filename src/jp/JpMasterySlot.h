// The JP 983 character mastery's slot group (JP-only, no KR counterpart;
// R558 screen #5, 0x5c0d20-0x5c5130): the tile dialog
// RFCharMasteryTileDlg (slot_mastery_wnd.xml, created by the mastery main
// dialog's 0x5be940), its columns RFMasteryTileColumn
// (slot_mastery_column.xml), and the two modal dialogs
// RFMasteryslotConfirmDlg (mastery_slot_confirm_dialog.xml) and
// RFMasteryslotNoticeDlg (mastery_slot_notice_dlg.xml). RTTI names read from
// their vtables' complete object locators; declarations and the JP x86
// layout from the JP exe's bodies (node scripts/disasm.ts --jp 0x<va>).
// The mastery game state is JP-only: every access to it goes through the
// declared-only JpMastery_* glue below (JP VA in its comment), never
// emulated.
#ifndef PG_JP_JPMASTERYSLOT_H
#define PG_JP_JPMASTERYSLOT_H

#include <vector>

#include "defines.h"
#include "padding.h"
#include "JpMasteryTooltipWnd.h"
#include "RFFormBase.h"
#include "RFFormModalBase.h"
#include "RFLabel.h"
#include "RFList.h"
#include "RFPicture.h"
#include "RFTextArea.h"

class IContentsDataContainer;
class RFButton;
class RFCheck;
class RFMasteryTileColumn;
struct sCharacterInfo;
namespace _gateway {
namespace _questoneday {
class CQuestDocument;
} // namespace _questoneday
} // namespace _gateway

// The JP mastery slot record a column carries (JP game data, one 0x3c-byte
// element of the JP CSharedDoc +0x744 vector; only the fields read here).
struct JpMasterySlotRecord {
	UNKNOWN(0x0, 0x4);
	uint32_t characterTid;  // 0x4 the character type id it belongs to (GetSlotRecords)
	int32_t slotNumber;  // 0x8 1-based: open once the selected slot number reaches it (UpdateStatCounts), selected when it is that number + 1 (BuildSlotColumns)
	int32_t stat;  // 0xc 1-based stat (1 power .. 5 curve): the slot animation plays stat - 1, slot_image_<stat - 1>_1.tga
	int32_t requiredLevel;  // 0x10 the player's level it needs (0: none)
	UNKNOWN(0x14, 0x18);
	int32_t requirement;    // 0x18 the achievement id it needs, given to RFCharMasteryTileDlg::MeetsRequirement (0: none)
	UNKNOWN(0x1c, 0x28);
	int32_t needTp;         // 0x28 the TP (item 0x1a0002a6) it costs (SelectColumn)
	UNKNOWN(0x2c, 0x3c);
};
CHECK_SIZE(JpMasterySlotRecord, 0x3c);
BEGIN_LAYOUT_CHECK(JpMasterySlotRecord)
	CHECK_FIELD(JpMasterySlotRecord, characterTid, 0x4);
	CHECK_FIELD(JpMasterySlotRecord, slotNumber, 0x8);
	CHECK_FIELD(JpMasterySlotRecord, stat, 0xc);
	CHECK_FIELD(JpMasterySlotRecord, requiredLevel, 0x10);
	CHECK_FIELD(JpMasterySlotRecord, requirement, 0x18);
	CHECK_FIELD(JpMasterySlotRecord, needTp, 0x28);
END_LAYOUT_CHECK

// The JP mastery stat rules (vtable 0xd12c0c, one slot; nothing but the
// vptr; name ours), built on the stack by the tile dialog's handlers.
class JpMasteryStatInfo {
public:
	// 0x5e3430 / 0x5e3440 (declared, outside this group): the vptr only;
	// [0] 0x5e3710 (its scalar deleting destructor).
	JpMasteryStatInfo();
	virtual ~JpMasteryStatInfo();

	// 0x5c0840 (name ours): stat 0..4's name through Jp_LookupText
	// (Korean keys power, control, accuracy, spin, curve); "" past 4.
	const char* GetStatName(uint32_t stat);
	// 0x5c02d0 (uncalled; name ours, class by GetStatName's range): stat
	// <= 4.
	bool IsValidStat(uint32_t stat);
	// 0x5e43c0 (JpMasteryState.cpp; name ours): records cleared, then the
	// slot records of characterTid (0: the equipped character's) whose slot
	// number masterySlot reaches (below 0: all of them), in table order (the
	// tile dialog's LoadSlotRecords passes 0, -1).
	void GetSlotRecords(std::vector<const JpMasterySlotRecord*>& records, uint32_t characterTid, int32_t masterySlot);
	// 0x5e39d0 (declared; name ours): the slots stat has open at level (-1:
	// the player's level, Jp_GameState_MyLevel): (level - 1) / 5 for stat 0
	// from level 6, else 0.
	int GetOpenSlotCount(uint32_t stat, int level);
	// 0x5e3970 (JpMasteryState.cpp; name ours): 0x9d9c50 on the player's
	// level, + 0x14.
	int GetPenaltyValue();

	// The stat sums (JpMasteryState.cpp; names ours; a stat past 4 gives 0).
	// 0x5e3850: the base stat of the character IFF characterTid.
	int GetCharacterBaseStatFor(uint32_t stat, uint32_t characterTid);
	// 0x5e3890: GetEquipmentStat for explicit parts, club set, caddie and
	// mascot type ids (no card manager bonus).
	int GetEquipmentStatFor(uint32_t stat, const uint32_t* partTids, const uint32_t* auxPartTids, uint32_t clubTid, uint32_t caddieTid, uint32_t mascotTid);
	// 0x5e3c00: the mastery points in stat of the first owned character of
	// type characterTid (0 when none).
	int GetMasteryPointsOfType(uint32_t stat, uint32_t characterTid);
	// 0x5e3cc0 (also called by 0xa8b410): GetStatValue for an explicit
	// character type id and equipment.
	int GetStatValueFor(uint32_t stat, uint32_t characterTid, const uint32_t* partTids, const uint32_t* auxPartTids, uint32_t clubTid, uint32_t caddieTid, uint32_t mascotTid);
	// 0x5e3d40: GetPowerPenalty on GetStatValueFor.
	int GetPowerPenaltyFor(uint32_t stat, uint32_t characterTid, const uint32_t* partTids, const uint32_t* auxPartTids, uint32_t clubTid, uint32_t caddieTid, uint32_t mascotTid);
	// 0x5e3f60: the equipped character's IFF base stat.
	int GetCharacterBaseStat(uint32_t stat);
	// 0x5e3fc0: the equipped club set, part level, caddie and mascot
	// bonuses, plus the card manager's two.
	int GetEquipmentStat(uint32_t stat);
	// 0x5e4180: the stat the 24 parts partTids (NULL: the equipped
	// character's), the equipped character's 5 aux parts and their set give.
	int GetPartsStatFor(uint32_t stat, const uint32_t* partTids);
	// 0x5e4260: the same for the equipped character's parts (its own loops,
	// no call of 0x5e4180).
	int GetPartsStat(uint32_t stat);
	// 0x5e4340: the equipped character's 12 cards' stat (JpMasteryState.cpp).
	int GetCardStat(uint32_t stat);
	// 0x5e44c0: the slot records (0x5e43c0) of character (NULL: the
	// equipped one) at its mastery slot number that raise stat, counted.
	int GetSlotPoints(uint32_t stat, const sCharacterInfo* character);
	// 0x5e45c0: the same for a character type id and mastery slot number.
	int GetSlotPointsFor(uint32_t stat, uint32_t characterTid, int32_t masterySlot);
	// 0x5e4690: the points stat can take: slots, parts, cards and open
	// slots.
	int GetMaxMasteryPoints(uint32_t stat);
	// 0x5e46e0: GetMaxMasteryPoints for explicit parts, character type id
	// and mastery slot number.
	int GetMaxMasteryPointsFor(uint32_t stat, const uint32_t* partTids, uint32_t characterTid, int32_t masterySlot);
	// 0x5e4740: character's (NULL: the equipped one) points in stat; not
	// raw, at most GetMaxMasteryPointsFor; 0 without a character.
	int GetMasteryPoints(uint32_t stat, bool raw, const sCharacterInfo* character);
	// 0x5e47e0: the equipped character's points in stat past
	// GetMaxMasteryPoints (never below 0; 0 without a character).
	int GetExcessMasteryPoints(uint32_t stat);
	// 0x5e48a0: the points stat can still take (never below 0).
	int GetFreeMasteryPoints(uint32_t stat);
	// 0x5e48e0: base + equipment + mastery points - the power penalty.
	int GetStatValue(uint32_t stat);
	// 0x5e4930: for stats 1 and 2, the power value past GetPenaltyValue
	// (never below 0); else 0.
	int GetPowerPenalty(uint32_t stat);
};

// The tile dialog (vtable 0xd0f67c, 34 slots: no new virtuals), size 0x1ec.
// The constructor keeps the instance in s_instance (0xe78120), which the
// columns and the confirm dialog read.
class RFCharMasteryTileDlg : public RFFormBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5c2190: RFWndEx::RFWndEx, s_instance = this, 0x184-0x1d7 zeroed,
	// m_slotRecords cleared, m_character, m_tooltip and m_masteryData NULL.
	RFCharMasteryTileDlg();
	// [1] 0x5c2990 (scalar deleting destructor); ~RFCharMasteryTileDlg
	// 0x5c2280 clears s_instance, frees m_slotRecords' storage, then restores
	// RFFormBase<RFWndEx>'s vtable and runs ~RFWndEx.
	virtual ~RFCharMasteryTileDlg();

	// 0x5c16c0 (name ours): sends the slot request for m_character
	// (RFMasteryslotConfirmDlg's Enter key with its key args, and the confirm
	// dialog's ok_btn click; the args are not read).
	void RequestSelectedSlot(EventArgs& args);
	// 0x5c17a0 (name ours): every column of both slot lists hides its
	// selected picture (the column's click handler calls it after
	// SelectColumn, then shows its own).
	void ClearSelectedMarks();
	// 0x5c0450 (name ours): m_powerSlotLabel the open power slots,
	// m_penaltyPowerLabel the stat info's penalty value + 1.
	void UpdateSlotLabels();
	// 0x5c1840 (name ours): the record of slot number (1-based, clamped to
	// the records); NULL without records.
	const JpMasterySlotRecord* GetSlotRecord(int number);
	// 0x5c1c70 (name ours): whether the achievement requirement (a slot
	// record's requirement) is cleared in m_masteryData's entries.
	bool MeetsRequirement(int32_t requirement);
	// 0x5c4e70 (name ours; RFCharMasteryMainDlg::CreateTileDlg calls it):
	// the labels, the slot columns (0x5c4810), the stat
	// counts (0x5c2320), then the slot after m_character's open slot selected.
	void Setup();
	// 0x5c4810 (name ours): with records, ClearSelectedMarks, then each
	// record's column (std::for_each 0x5c3020: slot list 1 for the first
	// five, list 2 after, item index % 5) refreshed for the selected slot
	// number and marked selected when the record's slotNumber is the one
	// after it.
	void BuildSlotColumns();
	// 0x5c2320 (name ours): the open slots per stat (std::for_each
	// 0x5c0730 over m_slotRecords) written "x%d" into m_statCountLabels.
	void UpdateStatCounts();
	// 0x5c2a40 (name ours): the column's click handler and Setup give it a
	// slot record: m_currentColumn shows it, the expand panel's labels and
	// checks show its TP, level and achievement conditions, and the expand
	// button is enabled when they are all met.
	void SelectColumn(const JpMasterySlotRecord* record);
	// 0x5c5000 (name ours; ~RFSoltAniWnd calls it on s_instance): without a
	// notice dialog yet, the notice dialog (CreateMasteryslotNoticeDialog
	// under this) given the current column's record and the table text
	// 0x18846 on its stat name, shown modal; then the set-up 0x5c4e70.
	void OnSlotAnimationDone();
	// 0x5c59e0 (name ours): the slot animation window (CreateSoltAniWnd,
	// no parent) shown modal, playing the current column's stat.
	void PlaySlotAnimation();

	// 0xe78120 (name ours): the instance, set by the constructor.
	static RFCharMasteryTileDlg* s_instance;
	// 0xe7811c (name ours): the parent the last CreateMasteryTileDialog
	// was given.
	static RFWnd* s_loadParent;
	// 0x5c02a0 / 0x5c02b0 / 0x5c02c0 (uncalled; names ours): s_instance's
	// get, set and clear.
	static RFCharMasteryTileDlg* GetInstance();
	static void SetInstance(RFCharMasteryTileDlg* instance);
	static void ClearInstance();
	// 0x5c0510 (uncalled; name ours): the stat count label of a 1-based
	// stat: 1 -> 0, 2 -> 1, 4 -> 2, 5 -> 3; any other stat (3, accuracy,
	// included) gives stat - 1.
	int GetStatCountIndex(int32_t stat);
	// 0x5c0700 (uncalled; name ours): the child name as an RFList.
	RFList* GetChildList(const std::wstring& name);

protected:
	// [10] 0x5c5270: the mouse enter/leave handlers on bg_rank,
	// bg_power_slot and bg_penalty_power, the five set-up steps below, the
	// tooltip (CreateMasteryToolTip 0x5d76c0 under the super window) and
	// the mastery data (JpMastery_FindData 0xac).
	virtual void OnInitialUpdate();

	// EventMouseEnter / EventMouseLeave of the three areas: the tooltip
	// gets a table text and is placed at the area's right edge, or hidden.
	void OnRankMouseEnter(EventArgs& args);          // 0x5c2460 bg_rank, text 0x18843
	void OnTooltipMouseLeave(EventArgs& args);       // 0x5c0610 all three: m_tooltip [34](false)
	void OnPowerSlotMouseEnter(EventArgs& args);     // 0x5c2580 bg_power_slot, text 0x18844 on the next slot's level name
	void OnPenaltyPowerMouseEnter(EventArgs& args);  // 0x5c2730 bg_penalty_power, text 0x18845

	// The set-up steps OnInitialUpdate calls in this order (names ours).
	// 0x5c4860: m_slotRecords from the stat info, m_character.
	void LoadSlotRecords();
	// 0x5c3ef0: rank_img (level_%03d.tga, sized to its texture) and the
	// two rank labels.
	void InitRankWidgets();
	// 0x5c4eb0: with records, the two slot lists, then one column per
	// record (lambda 0x5c4540 in std::for_each).
	void InitSlotLists();
	// 0x5c4190: the four total_*_slot_num_label labels.
	void InitStatCountLabels();
	// 0x5c48f0: button_slot_expand's click, the current column in place of
	// the selected_slot_wnd placeholder (deleted), then the expand panel's
	// labels, checks, picture and button.
	void InitExpandPanel();
	// 0x5c4370 (name ours): button_slot_expand's click: the confirm dialog
	// (CreateMasteryslotConfirmDialog under this) given the current
	// column's record (SetSlotRecord) and table text 0x18842 on its need TP
	// (SetMessage), ok_btn wired to RequestSelectedSlot, shown modal.
	void OnExpandClicked(EventArgs& args);

	// 0x0-0x184: RFFormBase<RFWndEx> / RFWndEx.
	RFMasteryTileColumn* m_currentColumn;         // 0x184 SelectColumn gives it the clicked column's data (SetColumnData)
	RFPicture* m_rankImg;                         // 0x188 "rank_img" (bg_rank's tooltip anchor)
	RFLabel* m_powerSlotLabel;                    // 0x18c "rank_power_slot_num_label": the open power slots (UpdateSlotLabels; bg_power_slot's tooltip anchor)
	RFLabel* m_penaltyPowerLabel;                 // 0x190 "rank_penalty_power_num_label": the penalty power (UpdateSlotLabels; bg_penalty_power's tooltip anchor)
	RFList* m_slotList1;                          // 0x194 items' data are RFMasteryTileColumn* (ClearSelectedMarks)
	RFList* m_slotList2;                          // 0x198 items' data are RFMasteryTileColumn* (ClearSelectedMarks)
	RFLabel* m_statCountLabels[4];                // 0x19c "total_<power|control|spin|curve>_slot_num_label": "x<count>" per stat (UpdateStatCounts)
	RFTextArea* m_needTpLabel;                    // 0x1ac "need_tp_label": the record's TP cost (SelectColumn)
	RFTextArea* m_currentTpLabel;                 // 0x1b0 "current_tp_label": the TP held, red when short (SelectColumn)
	RFTextArea* m_levelLimitLabel;                // 0x1b4 "level_limit_label": the required level's name (SelectColumn)
	RFTextArea* m_achievementClearLabel;          // 0x1b8 "achivement_clear_label": the required achievement's name (SelectColumn)
	RFCheck* m_levelLimitCheck;                   // 0x1bc "level_limit_check": shown with a level requirement, checked when met
	RFCheck* m_achievementClearCheck;             // 0x1c0 "achivement_clear_check": shown with an achievement requirement, checked when met
	RFPicture* m_slotExpandedImg;                 // 0x1c4 "slot_expanded_img": shown when the record's slot is open
	RFButton* m_expandButton;                     // 0x1c8 "button_slot_expand": enabled when the record can be opened
	std::vector<const JpMasterySlotRecord*> m_slotRecords;  // 0x1cc the slots' records (GetSlotRecord); cleared by the constructor, freed by the destructor
	UNKNOWN(0x1d8, 0x1dc);
	const sCharacterInfo* m_character;            // 0x1dc the equipped character's JP data (LoadSlotRecords): its open slot number, and the two dwords RequestSelectedSlot sends
	RFMasteryToolTipWnd* m_tooltip;               // 0x1e0 from RFMasteryToolTipWnd::Load
	// 0x1e4 the gateway quest document (contents container 0xac,
	// JpQuestForms.h), JpMastery_FindData(0xac) when found; its achievement
	// map (+0x70) is what MeetsRequirement walks.
	_gateway::_questoneday::CQuestDocument* m_masteryData;
	UNKNOWN(0x1e8, 0x1ec);
};

// One column of the tile dialog (vtable 0xd0f54c, 34 slots: no new
// virtuals), size 0x198.
class RFMasteryTileColumn : public RFFormBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
	// The tile dialog reads m_columnData (0x5c5000, 0x5c59e0).
	friend class RFCharMasteryTileDlg;
public:
	// 0x5c0f60: RFWndEx::RFWndEx, the pictures and m_columnData NULL,
	// m_state = 3.
	RFMasteryTileColumn();
	// [1] 0x5c1690 (scalar deleting destructor; runs ~RFWndEx).
	virtual ~RFMasteryTileColumn();

	// 0x5c0420 (name ours): m_columnData = data.
	void SetColumnData(const JpMasterySlotRecord* data);
	// 0x5c0290 (uncalled; name ours): +0x190. The column, the confirm and
	// the notice dialogs all keep a record there; the column's is assumed.
	const JpMasterySlotRecord* GetColumnData();
	// 0x5c0430 (name ours): m_selectedImg shown or hidden.
	void ShowSelected(bool show);
	// 0x5c2970 (name ours): 0x5c1fc0(selectedSlotNumber) then 0x5c2050.
	void Refresh(int32_t selectedSlotNumber);
	// 0x5c1fc0 (name ours): with a record, m_state 2 (open) when the
	// selected slot number reaches its slotNumber; 1 (next) when it is the
	// one after and its requirement and required level are met; else 0.
	void UpdateSlotState(int32_t selectedSlotNumber);
	// 0x5c2050 (name ours): with a record, m_slotImg
	// slot_image_<stat - 1>_<open>.tga and m_lockedImg shown for state 0.
	void UpdateSlotImage();

	// 0xe78114 (name ours): the parent the last CreateMasteryTileColumn
	// was given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5c45c0: OnColumnClicked on slot_btn's click, the three
	// pictures looked up (RFPicture casts), locked and selected
	// hidden.
	virtual void OnInitialUpdate();

	// 0x5c3d40 (name ours): slot_btn's click; through the tile dialog's
	// SelectColumn and ClearSelectedMarks, then m_selectedImg shown and
	// "ui_icon_click" played.
	void OnColumnClicked(EventArgs& args);

	// 0x0-0x184: RFFormBase<RFWndEx> / RFWndEx.
	RFPicture* m_slotImg;    // 0x184 "slot_img"
	RFPicture* m_lockedImg;  // 0x188 "locked_img", hidden by OnInitialUpdate
	RFPicture* m_selectedImg;  // 0x18c "selected_img", hidden by OnInitialUpdate, shown by OnColumnClicked
	const JpMasterySlotRecord* m_columnData;  // 0x190 given to RFCharMasteryTileDlg::SelectColumn
	int32_t m_state;         // 0x194 3 at construction; UpdateSlotState: 0 locked, 1 next to open, 2 open
};

// The slot confirm dialog (vtable 0xd0f4bc, 35 slots: [34] new; the
// RFModalRenderer sub-object at +0x180 has vtable 0xd0f4b4, [0] 0x5c0ee0),
// size 0x194.
class RFMasteryslotConfirmDlg : public RFFormModalBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5c0e60: RFWndEx::RFWndEx, RFModalRenderer, then 0x184-0x190 zeroed.
	RFMasteryslotConfirmDlg();
	// [1] 0x5c1660 (scalar deleting destructor).
	virtual ~RFMasteryslotConfirmDlg();

	// 0xe7810c (name ours): the parent the last
	// CreateMasteryslotConfirmDialog was given.
	static RFWnd* s_loadParent;

	// [34] 0x5c1b70 (name ours): Enter sends the tile dialog's slot request
	// and closes with state 1; Escape closes with state 2; both mark the
	// args handled.
	virtual void OnDialogKey(KeyEventArgs& args);

	// 0x5c1ec0 (name ours): keeps record and shows its
	// slot_image_<stat - 1>_0.tga in m_slotImg.
	void SetSlotRecord(const JpMasterySlotRecord* record);
	// 0x5c3740 (name ours): m_message's text, widened from Shift-JIS.
	void SetMessage(const char* text);

protected:
	// [10] 0x5c3880: message and slot_img looked up, OnKey on its own
	// EventKeyDown, ok_btn / cancel_btn clicks, then "ui_dialog_open".
	virtual void OnInitialUpdate();

	// 0x5c0820 (shared with the notice dialog): calls [34] with the args.
	void OnKey(EventArgs& args);
	// 0x5c03a0: "ui_button_ok_click", Close(2).
	void OnOkClicked(EventArgs& args);
	// 0x5c03e0: "ui_button_cancel_click", Close(2).
	void OnCancelClicked(EventArgs& args);

	// 0x0-0x184: RFFormModalBase<RFWndEx> (its m_updateRegistered at 0x184).
	RFTextArea* m_message;    // 0x188 "message"
	RFPicture* m_slotImg;    // 0x18c "slot_img"
	const JpMasterySlotRecord* m_slotRecord;  // 0x190 SetSlotRecord's record (OnExpandClicked)
};

// The slot notice dialog (vtable 0xd0f424, 35 slots: [34] new; the
// RFModalRenderer sub-object at +0x180 has vtable 0xd0f41c, [0] 0x5c0db0),
// size 0x194. The constructor keeps the instance in s_instance (0xe78108).
class RFMasteryslotNoticeDlg : public RFFormModalBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5c0d20: RFWndEx::RFWndEx, RFModalRenderer, s_instance = this, then
	// 0x188-0x190 zeroed.
	RFMasteryslotNoticeDlg();
	// [1] 0x5c1630 (scalar deleting destructor).
	virtual ~RFMasteryslotNoticeDlg();

	// [34] 0x5c0e30 (name ours): Enter or Escape is OnOkClicked, args
	// handled.
	virtual void OnDialogKey(KeyEventArgs& args);

	// 0x5c1dc0 (name ours): keeps record and shows its
	// slot_image_<stat - 1>_1.tga in m_slotImg.
	void SetSlotRecord(const JpMasterySlotRecord* record);
	// 0x5c3200 (name ours): m_message's text, widened from Shift-JIS.
	void SetMessage(const char* text);

	// 0xe78108 (name ours): the instance, set by the constructor.
	static RFMasteryslotNoticeDlg* s_instance;
	// 0x5c0260 / 0x5c0270 / 0x5c0280 / 0x5c0830 (uncalled; names ours):
	// s_instance's get, set, clear and != NULL.
	static RFMasteryslotNoticeDlg* GetInstance();
	static void SetInstance(RFMasteryslotNoticeDlg* instance);
	static void ClearInstance();
	static bool HasInstance();
	// 0xe78104 (name ours): the parent the last
	// CreateMasteryslotNoticeDialog was given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5c3340: OnKey on its own EventKeyDown, ok_btn click, slot_img
	// and message looked up, then "ui_dialog_open".
	virtual void OnInitialUpdate();

	// 0x5c0820 (shared with the confirm dialog): calls [34] with the args.
	void OnKey(EventArgs& args);
	// 0x5c0360: "ui_button_ok_click", Close(2).
	void OnOkClicked(EventArgs& args);

	// 0x0-0x184: RFFormModalBase<RFWndEx> (its m_updateRegistered at 0x184).
	RFPicture* m_slotImg;    // 0x188 "slot_img"
	RFTextArea* m_message;    // 0x18c "message"
	const JpMasterySlotRecord* m_slotRecord;  // 0x190 SetSlotRecord's record
};

// 0x5c3db0 (name ours): creates the tile dialog from slot_mastery_wnd.xml
// by g_jpWndCreateMode (0xe73dd8, JpTooltipWnd.h), keeps the parent in
// RFCharMasteryTileDlg::s_loadParent.
RFWnd* CreateMasteryTileDialog(RFWnd* parent);
// 0x5c30c0 / 0x5c3600 (names ours): the notice and confirm dialogs, by
// g_jpModalCreateMode (0xe03260, the modal forms' mode);
// the parent is kept in s_loadParent.
RFWnd* CreateMasteryslotNoticeDialog(RFWnd* parent);
RFWnd* CreateMasteryslotConfirmDialog(RFWnd* parent);
// 0x5c3c00 (name ours): one column, from slot_mastery_column.xml by
// g_jpWndCreateMode; the parent is kept in s_loadParent.
RFWnd* CreateMasteryTileColumn(RFWnd* parent);

// --- JP game-state glue (declared only) ---
// The JP character record's mastery points (+0x1c8, one signed byte per
// stat) and its 24 part type ids (+0xc) (names ours; the JP record is not
// the KR sCharacterInfo layout past +0x8).
int8_t JpCharacter_GetMasteryPoints(const sCharacterInfo* character, uint32_t stat);
const uint32_t* JpCharacter_GetPartTypeIds(const sCharacterInfo* character);
// The dword at +0x1cd (unaligned) of the equipped character's JP data
// (name ours): its open mastery slot number, which Setup and SelectColumn
// read.
int32_t JpCharacter_GetMasterySlotNumber(const sCharacterInfo* character);
// The JP character list packet (case 0x6e of 0x9c20a0, 0x9c6024: 0x201-byte
// records, each passed to the CSharedDoc AddChar 0x642730) fills both with
// the record: +0x1c8 the 5 mastery points, +0x1cd the open slot number. The
// KR record has neither: the setter (name ours) fills the port storage of
// the KR record the character map holds.
void JpCharacter_SetMasteryData(const sCharacterInfo* character, const int8_t points[5], int32_t slotNumber);
// The port's wire form of that JP tail after a KR sCharacterInfo (u8
// points[5], u32 slot number, u32 card type ids[12]; name ours, no JP
// function): read and stored for `character` (NULL: dropped).
class WReceivedPacket;
void JpCharacter_DecodeMasteryTail(WReceivedPacket& packet, const sCharacterInfo* character);
// The JP character record's 5 aux part type ids (+0x1a4; name ours).
const uint32_t* JpCharacter_GetAuxPartTypeIds(const sCharacterInfo* character);
// The JP CSharedDoc (0xe47dbc) +0x744 vector (name ours): every mastery
// slot record, 0x3c bytes each.
const std::vector<JpMasterySlotRecord>& Jp_GameState_MasterySlotRecords();
// The IFF manager (JP CSharedDoc +0x20c) lookups the stat sums read (names
// ours; NULL when the type id is missing) and the fields they read.
struct JpIffCharacter;
struct JpIffPart;
struct JpIffAuxPart;
// 0xa17120 (CItemManager::FindChar).
const JpIffCharacter* JpIff_FindCharacter(uint32_t typeId);
// The character's base stats (short[5] at +0x160).
int16_t JpIffCharacter_GetStat(const JpIffCharacter* character, uint32_t stat);
// The part (JpItemManager_FindPart 0xa17160, JpItemGlue.h: its opaque
// JpIffItemBase* is the KR sPart, reinterpreted as JpIffPart) stats (short[5] at +0x1ee).
int16_t JpIffPart_GetStat(const JpIffPart* part, uint32_t stat);
// 0xa178e0 (CItemManager::FindAuxPart).
const JpIffAuxPart* JpIff_FindAuxPart(uint32_t typeId);
// The aux part's stats (signed char[5] at +0xcf).
int8_t JpIffAuxPart_GetStat(const JpIffAuxPart* auxPart, uint32_t stat);
// 0xa1fad0 (thiscall on the IFF manager; name ours): the stat bonus of the
// part set the 24 parts and 5 aux parts form (0 for a stat past 4 or
// without both arrays): 0x9d9af0 keeps the arrays at manager +0x578/+0x57c,
// 0xa18a80 picks the first record of the manager's +0x1a0 map whose five
// type ids (+0x1c..+0x2c) are all worn (0x9e9bf0 by kind) and keeps it at
// +0x590, 0x9d9a90 returns its short +0x40[stat] plus one per +0x4/+0x8/+0xc
// equal to 6. The +0x1a0 map is not the KR m_setItemMap (that one is the
// JP +0xa0 by the maps' order, and its records list 10 type ids at 0x94):
// a JP-only table the port does not have.
uint8_t JpIff_GetPartSetStat(uint32_t stat, const uint32_t* partTids, const uint32_t* auxPartTids);
struct JpIffCaddie;
struct JpIffMascot;
struct JpIffClubSet;
// 0xa172a0 / 0xa179d0 (CItemManager::FindCaddie, FindMascot). The club
// set is JpItemManager_FindClubSet 0xa171e0 (JpItemGlue.h: its opaque
// JpIffItemBase* is the KR sClubSet, reinterpreted as JpIffClubSet).
const JpIffCaddie* JpIff_FindCaddie(uint32_t typeId);
const JpIffMascot* JpIff_FindMascot(uint32_t typeId);
// The fields the stat detail window's entries read (names ours; every JP
// record's name is the common header's char[0x28] at +0x8): the part's
// short +0x1e4[stat] (KR sPart::m_baseStat), the aux part's signed byte
// +0xca[stat] (KR sAuxPart::m_levels), the caddie's short +0xec[stat] (KR
// sCaddie::m_levels), the mascot's signed byte +0x115[stat] (KR sMascot
// 0xe5, unnamed in KR) and the club set's short +0xd0[stat] (KR
// sClubSet::m_baseStat).
const char* JpIffPart_GetName(const JpIffPart* part);
int16_t JpIffPart_GetBaseStat(const JpIffPart* part, uint32_t stat);
const char* JpIffAuxPart_GetName(const JpIffAuxPart* auxPart);
int8_t JpIffAuxPart_GetLevel(const JpIffAuxPart* auxPart, uint32_t stat);
const char* JpIffCaddie_GetName(const JpIffCaddie* caddie);
int16_t JpIffCaddie_GetStat(const JpIffCaddie* caddie, uint32_t stat);
const char* JpIffMascot_GetName(const JpIffMascot* mascot);
int8_t JpIffMascot_GetStat(const JpIffMascot* mascot, uint32_t stat);
const char* JpIffClubSet_GetName(const JpIffClubSet* clubSet);
int16_t JpIffClubSet_GetBaseStat(const JpIffClubSet* clubSet, uint32_t stat);
// 0xa17480 (CItemManager::FindEnchant; the JP manager's +0xe0 map is the
// KR m_enchantMap by the maps' order): the enchant record, keyed here by
// ((stat | 0x340) << 20) | the stat's mastery points; NULL when missing.
// Its +0x8 (KR sEnchant::m_value) is the pang the next point costs (the
// up/down dialog compares it with the player's pang).
struct JpIffEnchant;
const JpIffEnchant* JpIff_FindEnchant(uint32_t typeId);
uint32_t JpIffEnchant_GetValue(const JpIffEnchant* enchant);
// The selected character's JP record (the +0x3704 map's value) as the
// stat request sends it: WSendPacket EncodeBuffer 0xa4ddc0 of 0x201 bytes
// (the character list packet's record; names ours). Its layout beyond the
// fields JpMasteryGlue.cpp maps is not traced.
class WSendPacket;
void JpCharacter_EncodeRecord(WSendPacket& packet, const sCharacterInfo* character);
// The same record with the JP tail the port keeps for `tailOwner` (an
// edited copy of a shared doc character is sent with the doc record's tail).
void JpCharacter_EncodeRecordWithTailOf(WSendPacket& packet, const sCharacterInfo* character, const sCharacterInfo* tailOwner);
// 0xa189c0 (CItemManager::GetItemLevel, thiscall on the IFF manager): the
// stat the parts' item levels give.
uint8_t JpIff_GetItemLevelStat(uint32_t stat, const uint32_t* partTids, const uint32_t* auxPartTids);
// 0x9e9cc0 (name ours): the club set's stat (short +0xd0[stat]) plus its
// upgrades (short[5], may be NULL), capped by 0x9e9c80 on extra (the club
// item's +0xaa, type not traced; may be NULL).
uint8_t Jp_GetClubSetStat(uint32_t stat, const JpIffClubSet* clubSet, const int16_t* upgrades, const void* extra);
// 0x9d9ce0 / 0x9d9d10 (names ours): the caddie's and the mascot's stat
// bonus at the player's level (the IFF entry may be NULL).
uint8_t Jp_GetCaddieStat(uint32_t stat, const JpIffCaddie* caddie, uint8_t level);
uint8_t Jp_GetMascotStat(uint32_t stat, const JpIffMascot* mascot, uint8_t level);
// 0x80a620 / 0x809f40 on the card manager 0xe74210 (names ours): the
// card ability bonus (stat 1 only) and the card effect bonus of a stat for
// the card manager's player.
int JpCardManager_GetStatBonus(uint32_t stat);
int JpCardManager_GetEffectStatBonus(uint32_t stat);
// 0x80a000 on the card manager 0xe74210 (thiscall, the stat; name ours):
// 0x9e9c80's card term, the club stat limit bonus of the manager's player.
uint8_t JpCardManager_GetClubStatCapBonus(uint32_t stat);
// The equipped caddie (+0x3714 map by +0xa26), mascot (+0x3724 by +0xa8e)
// and club (item map +0x3740 by +0xa2e) of the JP CSharedDoc (names ours;
// NULL when the map has no such entry) and the fields read from them.
struct JpCaddieData;
struct JpMascotData;
struct JpItemData;
const JpCaddieData* Jp_GameState_EquippedCaddie();
const JpMascotData* Jp_GameState_EquippedMascot();
const JpItemData* Jp_GameState_EquippedClub();
uint32_t JpCaddieData_GetTypeId(const JpCaddieData* caddie);  // +0x4
uint32_t JpMascotData_GetTypeId(const JpMascotData* mascot);  // +0x4
uint32_t JpItemData_GetTypeId(const JpItemData* item);        // +0x4
const int16_t* JpItemData_GetClubUpgrades(const JpItemData* item);  // +0xc, short[5]
const void* JpItemData_GetClubExtra(const JpItemData* item);        // +0xaa
// The JP CSharedDoc +0x3704 character map in key order (names ours): the
// first and next character record, NULL past the last.
const sCharacterInfo* Jp_GameState_FirstCharacter();
const sCharacterInfo* Jp_GameState_NextCharacter(const sCharacterInfo* character);
// 0xe47dbc +0x91d (the JP CSharedDoc; KR CSharedDoc::m_myLevel): the
// player's level (JpMasteryStatInfo::GetOpenSlotCount's -1).
uint8_t Jp_GameState_MyLevel();
// 0x8329d0 on the level table 0xe78124 (name ours): level's record, by
// value ("dummy" with value 0 when missing); its name is a Jp_LookupText key.
struct JpLevelRecord {
	char name[0x10];  // 0x0
	int32_t value;    // 0x10
};
JpLevelRecord JpLevelTable_Find(int level);
// 0x9137e0 on the game object 0xe47db4 (thiscall, id 0xac): KR
// CContentsDoc::GetContainer; for 0xac the gateway quest document
// (CQuestDocument, created by 0x5ee040) the tile dialog keeps in
// m_masteryData; NULL when missing.
IContentsDataContainer* JpMastery_FindData(int id);
// 0x5c06d0 (uncalled; name ours; thiscall on that game object): data =
// JpMastery_FindData(id) when found; whether it was.
bool Jp_FindMasteryData(int id, IContentsDataContainer** data);
// 0xa176b0 on the JP CSharedDoc +0x20c (name ours): the achievement record
// of an id, its name a Jp_LookupText key at +8; NULL when missing.
const char* Jp_FindAchievementName(int32_t id);

CHECK_SIZE(RFCharMasteryTileDlg, 0x1ec);
BEGIN_LAYOUT_CHECK(RFCharMasteryTileDlg)
	CHECK_FIELD(RFCharMasteryTileDlg, m_currentColumn, 0x184);
	CHECK_FIELD(RFCharMasteryTileDlg, m_rankImg, 0x188);
	CHECK_FIELD(RFCharMasteryTileDlg, m_powerSlotLabel, 0x18c);
	CHECK_FIELD(RFCharMasteryTileDlg, m_slotList1, 0x194);
	CHECK_FIELD(RFCharMasteryTileDlg, m_slotList2, 0x198);
	CHECK_FIELD(RFCharMasteryTileDlg, m_statCountLabels, 0x19c);
	CHECK_FIELD(RFCharMasteryTileDlg, m_needTpLabel, 0x1ac);
	CHECK_FIELD(RFCharMasteryTileDlg, m_currentTpLabel, 0x1b0);
	CHECK_FIELD(RFCharMasteryTileDlg, m_levelLimitLabel, 0x1b4);
	CHECK_FIELD(RFCharMasteryTileDlg, m_achievementClearLabel, 0x1b8);
	CHECK_FIELD(RFCharMasteryTileDlg, m_levelLimitCheck, 0x1bc);
	CHECK_FIELD(RFCharMasteryTileDlg, m_achievementClearCheck, 0x1c0);
	CHECK_FIELD(RFCharMasteryTileDlg, m_slotExpandedImg, 0x1c4);
	CHECK_FIELD(RFCharMasteryTileDlg, m_expandButton, 0x1c8);
	CHECK_FIELD(RFCharMasteryTileDlg, m_penaltyPowerLabel, 0x190);
	CHECK_FIELD(RFCharMasteryTileDlg, m_slotRecords, 0x1cc);
	CHECK_FIELD(RFCharMasteryTileDlg, m_character, 0x1dc);
	CHECK_FIELD(RFCharMasteryTileDlg, m_tooltip, 0x1e0);
	CHECK_FIELD(RFCharMasteryTileDlg, m_masteryData, 0x1e4);
END_LAYOUT_CHECK
CHECK_SIZE(RFMasteryTileColumn, 0x198);
BEGIN_LAYOUT_CHECK(RFMasteryTileColumn)
	CHECK_FIELD(RFMasteryTileColumn, m_slotImg, 0x184);
	CHECK_FIELD(RFMasteryTileColumn, m_lockedImg, 0x188);
	CHECK_FIELD(RFMasteryTileColumn, m_selectedImg, 0x18c);
	CHECK_FIELD(RFMasteryTileColumn, m_columnData, 0x190);
	CHECK_FIELD(RFMasteryTileColumn, m_state, 0x194);
END_LAYOUT_CHECK
CHECK_SIZE(RFMasteryslotConfirmDlg, 0x194);
BEGIN_LAYOUT_CHECK(RFMasteryslotConfirmDlg)
	CHECK_FIELD(RFMasteryslotConfirmDlg, m_message, 0x188);
	CHECK_FIELD(RFMasteryslotConfirmDlg, m_slotImg, 0x18c);
	CHECK_FIELD(RFMasteryslotConfirmDlg, m_slotRecord, 0x190);
END_LAYOUT_CHECK
CHECK_SIZE(RFMasteryslotNoticeDlg, 0x194);
BEGIN_LAYOUT_CHECK(RFMasteryslotNoticeDlg)
	CHECK_FIELD(RFMasteryslotNoticeDlg, m_slotImg, 0x188);
	CHECK_FIELD(RFMasteryslotNoticeDlg, m_message, 0x18c);
	CHECK_FIELD(RFMasteryslotNoticeDlg, m_slotRecord, 0x190);
END_LAYOUT_CHECK

#endif // PG_JP_JPMASTERYSLOT_H
