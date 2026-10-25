// The JP 983 character mastery's stat detail window and its two modal
// dialogs (JP-only, no KR counterpart; R558 screen "Mastery stat",
// 0x5c6400-0x5ccbd0): RFCharMasteryStatDetailWnd (kam_status_detail_wnd.xml),
// RFMasteryStatUpDnDlg (kam_status_updown_dialog.xml) and
// RFMasteryStatNoticeDlg (mastery_stat_notice_dialog.xml). RTTI names read
// from their vtables' complete object locators; declarations and the JP x86
// layout from the JP exe's bodies (node scripts/disasm.ts --jp 0x<va>).
// The mastery game state is JP-only: functions that reach it are declared,
// not written (never emulated).
#ifndef PG_JP_JPMASTERYSTATDETAIL_H
#define PG_JP_JPMASTERYSTATDETAIL_H

#include <stdint.h>
#include <vector>

#include "defines.h"
#include "padding.h"
#include "JpMasteryTooltipWnd.h"
#include "RFFormBase.h"
#include "RFFormModalBase.h"

struct JpIffPart;
struct JpIffAuxPart;
struct JpIffCaddie;
struct JpIffMascot;
struct JpIffClubSet;
struct JpItemData;

class RFButton;
class RFLabel;
class RFPicture;
class RFTextArea;
class KeyEventArgs;

// The stat detail window (vtable 0xd0ff34, 34 slots: no new virtuals), size
// 0x1e0: the stat up/down buttons, six stat labels, the mastery tooltip
// shown over the four "egg" areas and the cap button, and an open/closed
// state toggled by the cap button's animations.
class RFCharMasteryStatDetailWnd : public RFFormBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5c7a70: RFWndEx::RFWndEx, 0x188-0x1bb zeroed, the second vector
	// cleared, m_statType = 5, 0x1d0-0x1dc zeroed.
	RFCharMasteryStatDetailWnd();
	// [1] 0x5c80a0 (scalar deleting destructor); ~RFCharMasteryStatDetailWnd
	// 0x5c7b10 frees both vectors' storage, then restores
	// RFFormBase<RFWndEx>'s vtable and runs ~RFWndEx.
	virtual ~RFCharMasteryStatDetailWnd();

	// 0x5c9160 (name ours): cap_img's picture
	// "button_mastery_stat_<index>_cap.tga" (the status column's
	// UpdateDetailStat calls it).
	void SetStat(int32_t index);
	// 0x5cb950 (name ours): m_statType = statType; for a stat 0..4 the
	// five value labels from a JpMasteryStatInfo (base, "+" equipment, "+"
	// mastery points, "-" power penalty, stat value), the total label
	// black while the stat has excess points (else white), the free points
	// label, the up / down buttons enabled by the free / mastery points,
	// then UpdateEntries (the status column's UpdateDetailValues calls it).
	void UpdateValues(int32_t statType);

	// m_detailOpen as the status column reads and writes it (its button
	// handlers 0x5c96e0 / 0x5c9790 / 0x5c9840; names ours).
	int32_t GetDetailOpen() const { return m_detailOpen; }
	void SetDetailOpen(int32_t open) { m_detailOpen = open; }

	// 0xe78144 (name ours): the parent the last
	// CreateMasteryStatDetailWnd was given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5cbcd0: the cap and stat up/down click handlers, the mouse
	// enter/leave handlers of egg1-egg4 and cap_btn, the controls looked
	// up, then the mastery tooltip under the third super window.
	virtual void OnInitialUpdate();

	// cap_btn's EventClicked (0x5c92a0): opens ("_push_status_detail_"
	// animation, "mastery_stat_open") or closes ("_close_status_detail_",
	// "mastery_stat_closed") the detail.
	void OnCapClicked(EventArgs& args);
	// 0x5cbc30 / 0x5cbc70: button_stat_up / button_stat_dn's EventClicked:
	// unless an up/down dialog is open, creates one, sets it up for
	// m_statType with +1 / -1 and shows it modal.
	void OnStatUpClicked(EventArgs& args);
	void OnStatDownClicked(EventArgs& args);
	// EventMouseEnter of the areas: the tooltip gets a text from the
	// mastery stat state and is shown.
	void OnEgg1MouseEnter(EventArgs& args);     // 0x5ca030 table text 0x18836 at label 0
	void OnEgg2MouseEnter(EventArgs& args);     // 0x5ca0f0 "%s : %d" per part, aux part, caddie, mascot, club set, then table texts 0x18837/0x18838 (card bonuses) at label 1
	void OnEgg3MouseEnter(EventArgs& args);     // 0x5ca410 "[%s : %d/%d]" (stat name, mastery points, max; red past the max) plus table texts 0x18839-0x1883b (slot, parts, card points) at label 2
	void OnEgg4MouseEnter(EventArgs& args);     // 0x5ca640 with a power penalty in m_statType, table text 0x1883c (power stat value, penalty) at label 3
	// 0x5ca790: cap_btn's EventMouseEnter: while closed the
	// "_over_status_detail_" animation; with excess points in m_statType,
	// table text 0x1883d formatted with them at label 4.
	void OnCapMouseEnter(EventArgs& args);
	// 0x5ca8e0: cap_btn's EventMouseLeave: "_out_status_detail_"
	// animation, the tooltip hidden.
	void OnCapMouseLeave(EventArgs& args);
	// 0x5ca770: EventMouseLeave of egg1-egg4: the tooltip hidden.
	void OnEggMouseLeave(EventArgs& args);

	// 0x5ca9b0 (name ours): with an equipped character, refills m_parts /
	// m_auxParts (0x5c7ea0) for m_statType and looks up m_caddie, m_mascot,
	// m_clubSet and m_club: what OnEgg2MouseEnter lists.
	void UpdateEntries();

	// 0x5c9400 (name ours): with a tooltip, hides it (show false), or gives
	// it the Shift-JIS text in colour, places it at the right edge of the
	// stat label labelIndex and shows it.
	void ShowTooltip(bool show, char const* text, int32_t labelIndex, uint32_t color);

	// 0x184-0x1e0 (0x0-0x184: RFFormBase<RFWndEx> / RFWndEx).
	int32_t m_statType;                     // 0x184 5 at construction; given to the up/down dialog
	RFButton* m_statUpButton;               // 0x188 "button_stat_up"
	RFButton* m_statDownButton;             // 0x18c "button_stat_dn"
	// 0x190 the stat labels, indexed by ShowTooltip: char_stat_num_label,
	// parts_stat_num_label, slot_stat_num_label, penalty_stat_num_label,
	// total_stat_num_label, extra_slot_num_label.
	RFLabel* m_statLabels[6];
	RFMasteryToolTipWnd* m_tooltip;         // 0x1a8 RFMasteryToolTipWnd::Load
	int32_t m_detailOpen;                   // 0x1ac cap button's open state
	// 0x1b0 / 0x1c0 (UpdateEntries): the equipped character's parts and
	// aux parts with a positive stat in m_statType, freed by the destructor.
	std::vector<const JpIffPart*> m_parts;          // 0x1b0
	std::vector<const JpIffAuxPart*> m_auxParts;    // 0x1c0
	// 0x1d0-0x1dc (UpdateEntries; zeroed by the constructor): the equipped
	// caddie's, mascot's and club set's IFF records and the club item.
	const JpIffCaddie* m_caddie;            // 0x1d0
	const JpIffMascot* m_mascot;            // 0x1d4
	const JpIffClubSet* m_clubSet;          // 0x1d8
	const JpItemData* m_club;               // 0x1dc
};
CHECK_SIZE(RFCharMasteryStatDetailWnd, 0x1e0);
BEGIN_LAYOUT_CHECK(RFCharMasteryStatDetailWnd)
	CHECK_FIELD(RFCharMasteryStatDetailWnd, m_statType, 0x184);
	CHECK_FIELD(RFCharMasteryStatDetailWnd, m_statUpButton, 0x188);
	CHECK_FIELD(RFCharMasteryStatDetailWnd, m_statDownButton, 0x18c);
	CHECK_FIELD(RFCharMasteryStatDetailWnd, m_statLabels, 0x190);
	CHECK_FIELD(RFCharMasteryStatDetailWnd, m_tooltip, 0x1a8);
	CHECK_FIELD(RFCharMasteryStatDetailWnd, m_detailOpen, 0x1ac);
	CHECK_FIELD(RFCharMasteryStatDetailWnd, m_parts, 0x1b0);
	CHECK_FIELD(RFCharMasteryStatDetailWnd, m_auxParts, 0x1c0);
	CHECK_FIELD(RFCharMasteryStatDetailWnd, m_caddie, 0x1d0);
	CHECK_FIELD(RFCharMasteryStatDetailWnd, m_mascot, 0x1d4);
	CHECK_FIELD(RFCharMasteryStatDetailWnd, m_clubSet, 0x1d8);
	CHECK_FIELD(RFCharMasteryStatDetailWnd, m_club, 0x1dc);
END_LAYOUT_CHECK

// The stat up/down dialog (vtable 0xd0fd24, 35 slots: [34] new; the
// RFModalRenderer sub-object at +0x180 has vtable 0xd0fd1c), size 0x1b8.
// The constructor keeps the instance in s_instance (0xe78140), which the
// detail window's up/down handlers test.
class RFMasteryStatUpDnDlg : public RFFormModalBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5c6540: RFWndEx::RFWndEx, RFModalRenderer, 0x188-0x1af zeroed,
	// m_statType = 5, m_requestSent cleared, s_instance = this.
	RFMasteryStatUpDnDlg();
	// [1] 0x5c71e0 (scalar deleting destructor; the sub-object's [0]
	// 0x5c6600 adjusts this by -0x180 and jumps to it); ~RFMasteryStatUpDnDlg
	// 0x5c6610 clears s_instance, then restores RFFormModalBase<RFWndEx>'s
	// vtables and runs ~RFModalRenderer (0x5e5200) and ~RFWndEx.
	virtual ~RFMasteryStatUpDnDlg();

	// [34] 0x5cb910 (name ours): Enter is OnConfirmClicked, Escape
	// Close(2); either way the args are handled.
	virtual void OnDialogKey(KeyEventArgs& args);

	// 0x5cb260 (name ours): for a stat 0..4, keeps the stat and the
	// direction (+1 up, -1 down) and fills the dialog (the three steps
	// below); a JpMasteryStatInfo lives on the stack throughout.
	void SetUp(uint32_t statType, int32_t direction);

	// 0xe78140 (name ours): the instance, set by the constructor.
	static RFMasteryStatUpDnDlg* s_instance;
	// 0xe7813c (name ours): the parent the last
	// CreateMasteryStatUpDnDialog was given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5cb2f0: OnKey on its own EventKeyDown, the confirm and cancel
	// clicks, nine controls looked up, then "ui_dialog_open".
	virtual void OnInitialUpdate();

	// SetUp's steps (names ours):
	// 0x5c8d90: with a direction, the arrow and title pictures (up unless
	// the direction is -1), then stat_bg1 / stat_bg2's
	// "button_mastery_stat_<stat>_n.tga" / "..._cap.tga".
	void UpdateImages();
	// 0x5c5c10: with a direction, the total label shows the stat value
	// one step up or down, the before label the free points, the after
	// label the free points one fewer (up), none while the stat has excess
	// points, else one more (down).
	void UpdateNumbers();
	// 0x5c9df0: the message text (the stat name, the step's pang cost) and
	// the power warning.
	void UpdateMessage();

	// 0x5c6340 (shared with the notice dialog): calls [34] with the args.
	void OnKey(EventArgs& args);
	// 0x5c9bc0 (name ours): button_confirm's
	// EventClicked: once (m_requestSent), "ui_button_ok_click", then either
	// a notice (CreateNoticeDialog) or the stat request is sent.
	void OnConfirmClicked(EventArgs& args);
	// 0x5c5bd0: button_cancel's EventClicked: "ui_button_cancel_click",
	// Close(2).
	void OnCancelClicked(EventArgs& args);

	// 0x0-0x184: RFFormModalBase<RFWndEx> (its m_updateRegistered at 0x184).
	RFPicture* m_arrowImg;          // 0x188 "arrow_img"
	RFPicture* m_titleImg;          // 0x18c "title_img"
	RFPicture* m_statBg1Img;        // 0x190 "stat_bg1_img"
	RFPicture* m_statBg2Img;        // 0x194 "stat_bg2_img"
	RFLabel* m_totalStatLabel;      // 0x198 "total_stat_num"
	RFLabel* m_beforeSlotLabel;     // 0x19c "before_slot_num"
	RFLabel* m_afterSlotLabel;      // 0x1a0 "after_slot_num"
	RFTextArea* m_message;          // 0x1a4 "message"
	RFTextArea* m_warningText;      // 0x1a8 "warnning_text"
	int32_t m_direction;            // 0x1ac +1 / -1 (0: none; OnConfirmClicked tests it)
	int32_t m_statType;             // 0x1b0 5 at construction
	bool m_requestSent;             // 0x1b4 set once the request is sent
	PADDING(0x1b5, 0x1b8);
};
CHECK_SIZE(RFMasteryStatUpDnDlg, 0x1b8);
BEGIN_LAYOUT_CHECK(RFMasteryStatUpDnDlg)
	CHECK_FIELD(RFMasteryStatUpDnDlg, m_arrowImg, 0x188);
	CHECK_FIELD(RFMasteryStatUpDnDlg, m_totalStatLabel, 0x198);
	CHECK_FIELD(RFMasteryStatUpDnDlg, m_message, 0x1a4);
	CHECK_FIELD(RFMasteryStatUpDnDlg, m_warningText, 0x1a8);
	CHECK_FIELD(RFMasteryStatUpDnDlg, m_direction, 0x1ac);
	CHECK_FIELD(RFMasteryStatUpDnDlg, m_statType, 0x1b0);
	CHECK_FIELD(RFMasteryStatUpDnDlg, m_requestSent, 0x1b4);
END_LAYOUT_CHECK

// The stat notice dialog (vtable 0xd0fc8c, 35 slots: [34] new; the
// RFModalRenderer sub-object at +0x180 has vtable 0xd0fc80), size 0x19c.
// The constructor keeps the instance in s_instance (0xe78138).
class RFMasteryStatNoticeDlg : public RFFormModalBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5c6400: RFWndEx::RFWndEx, RFModalRenderer, s_instance = this, then
	// 0x188-0x19b zeroed.
	RFMasteryStatNoticeDlg();
	// [1] 0x5c71b0 (scalar deleting destructor; the sub-object's [0]
	// 0x5c6490 adjusts this by -0x180 and jumps to it);
	// ~RFMasteryStatNoticeDlg 0x5c64a0 clears s_instance, then restores
	// RFFormModalBase<RFWndEx>'s vtables and runs ~RFModalRenderer
	// (0x5e5200) and ~RFWndEx.
	virtual ~RFMasteryStatNoticeDlg();

	// [34] 0x5c6510 (name ours): Enter or Escape is OnOkClicked, args
	// handled.
	virtual void OnDialogKey(KeyEventArgs& args);

	// 0xe78138 (name ours): the instance, set by the constructor.
	static RFMasteryStatNoticeDlg* s_instance;
	// 0xe78134 (name ours): the parent the last
	// CreateMasteryStatNoticeDialog was given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5c8870: OnKey on its own EventKeyDown, ok_btn's click, five
	// controls looked up, then "ui_dialog_open".
	virtual void OnInitialUpdate();

	// 0x5c6340 (shared with the up/down dialog): calls [34] with the args.
	void OnKey(EventArgs& args);
	// 0x5c5b90: ok_btn's EventClicked: "ui_button_ok_click", Close(1).
	void OnOkClicked(EventArgs& args);

	// 0x0-0x184: RFFormModalBase<RFWndEx> (its m_updateRegistered at 0x184).
	RFPicture* m_statImg1;          // 0x188 "stat_img_01"
	RFPicture* m_statImg2;          // 0x18c "stat_img_02"
	RFPicture* m_arrowImg;          // 0x190 "arrow_img"
	RFLabel* m_statLabel;           // 0x194 "stat_label"
	RFTextArea* m_message;          // 0x198 "message"
};
CHECK_SIZE(RFMasteryStatNoticeDlg, 0x19c);
BEGIN_LAYOUT_CHECK(RFMasteryStatNoticeDlg)
	CHECK_FIELD(RFMasteryStatNoticeDlg, m_statImg1, 0x188);
	CHECK_FIELD(RFMasteryStatNoticeDlg, m_arrowImg, 0x190);
	CHECK_FIELD(RFMasteryStatNoticeDlg, m_statLabel, 0x194);
	CHECK_FIELD(RFMasteryStatNoticeDlg, m_message, 0x198);
END_LAYOUT_CHECK

// Creators (name ours; parent in ecx): the window from its XML, made as the
// mode says (0 CreateWnd under the parent, 1 CreateWnd with no parent, 2
// CreateDesktopWnd); the parent is kept in the class's s_loadParent.
// 0x5c8400 / 0x5c8c50: g_jpModalCreateMode (0xe03260, JpLobbyDialogs.h).
RFWnd* CreateMasteryStatNoticeDialog(RFWnd* parent);
RFWnd* CreateMasteryStatUpDnDialog(RFWnd* parent);
// 0x5c9020: g_jpWndCreateMode (0xe73dd8); called by the status column's
// OnInitialUpdate 0x5cacd0.
RFWnd* CreateMasteryStatDetailWnd(RFWnd* parent);

#endif // PG_JP_JPMASTERYSTATDETAIL_H
