// The JP 983 game's two "common" modal dialogs (RTTI names read from their
// vtables' complete object locators), both : RFFormModalBase<RFWndEx>:
// RFUseItemDlg (common_use_item_dialog.xml, CreateUseItemDialog 0x5cd6f0)
// and RFNoticeDlg (common_notice_dialog.xml, CreateNoticeDialog 0x5cdd60).
// Their source file is the run 0x5ccba0-0x5ce010. Despite the names only
// the character mastery code uses them (0x5bc6b0 and 0x5bc880 of the
// mastery cards, 0x5c9bc0 of the mastery status): each caller creates the
// dialog, fills it, then shows it with RFFormModalBase::ShowModal
// (0x4783e0). Declarations and the JP x86 layout, from the JP exe's bodies
// (node scripts/disasm.ts --jp 0x<va>).
#ifndef PG_JP_JPCOMMONDIALOGS_H
#define PG_JP_JPCOMMONDIALOGS_H

#include <string>

#include "defines.h"
#include "padding.h"
#include "RFFormModalBase.h"

class RFLabel;
class RFPicture;
class RFTextArea;
struct sItemInfo;
namespace IFF_STRUCT {
struct sItem;
}

// The "use this item?" dialog (vtable 0xd106ac, 34 slots: no new virtuals;
// the RFModalRenderer sub-object at +0x180 has vtable 0xd106a0), size 0x194:
// an item picture with its count, a message and the ok/cancel buttons,
// which close it with state 1 (ok) or 2 (cancel).
class RFUseItemDlg : public RFFormModalBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5ccbd0: RFWndEx::RFWndEx, RFModalRenderer (0x5e51f0),
	// m_updateRegistered cleared, then the instance is kept in g_useItemDlg
	// and the controls cleared.
	RFUseItemDlg();
	// [1] 0x5cd140 (scalar deleting destructor; the sub-object's [0]
	// 0x5ccc60 adjusts this by -0x180 and jumps to it); ~RFUseItemDlg
	// 0x5ccc70 clears g_useItemDlg, then restores RFFormModalBase<RFWndEx>'s
	// vtables and runs ~RFModalRenderer (0x5e5200) and ~RFWndEx.
	virtual ~RFUseItemDlg();

	// 0x5cd2c0 (name ours): with an inventory item, the picture gets its
	// IFF record's icon ("<icon>.tga", the record from CItemManager::FindItem
	// 0xa17260 on the JP CSharedDoc +0x20c, used unchecked) and the label
	// "x<count>" (JP +0x4 type id, +0xc count, as the KR sItemInfo).
	void SetItem(sItemInfo const* item);
	// 0x5cd470 (name ours; no caller in the exe): the same from an IFF item
	// record (JP +0x49 icon name and +0xec count: the KR sItem's m_icon and
	// quantity, after the JP record's longer name and model strings).
	void SetItem(IFF_STRUCT::sItem const* item);
	// 0x5cd830 (name ours; no caller in the exe): the message text area's
	// [34] SetText with a copy of the text.
	void SetMessage(std::wstring const& text);
	// 0x5cd900 (name ours): the same with a Shift-JIS text (0x5e4ef0).
	void SetMessage(char const* text);

protected:
	// [10] 0x5cda40: OnOkClicked and OnCancelClicked on the buttons'
	// RFButton::EventClicked, the three controls looked up, then the
	// "ui_dialog_open" sound.
	virtual void OnInitialUpdate();

	// 0x5ccba0 (name ours), ok_btn's EventClicked: Close(1).
	void OnOkClicked(EventArgs& args);
	// 0x5ccbb0 (name ours), cancel_btn's EventClicked: Close(2).
	void OnCancelClicked(EventArgs& args);

	// 0x0-0x188: RFFormModalBase<RFWndEx> (RFWndEx, RFModalRenderer at
	// +0x180, m_updateRegistered +0x184).
	RFPicture* m_itemPicture;       // 0x188 "item_img"
	RFLabel* m_itemCountLabel;      // 0x18c "item_num_label"
	RFTextArea* m_messageText;      // 0x190 "message"
};
CHECK_SIZE(RFUseItemDlg, 0x194);
BEGIN_LAYOUT_CHECK(RFUseItemDlg)
	CHECK_FIELD(RFUseItemDlg, m_itemPicture, 0x188);
	CHECK_FIELD(RFUseItemDlg, m_itemCountLabel, 0x18c);
	CHECK_FIELD(RFUseItemDlg, m_messageText, 0x190);
END_LAYOUT_CHECK

// The notice dialog (vtable 0xd10744, 34 slots: no new virtuals; the
// RFModalRenderer sub-object at +0x180 has vtable 0xd10738), size 0x18c: a
// message label and an ok button, which closes it with state 1. Unlike
// RFUseItemDlg it keeps no instance.
class RFNoticeDlg : public RFFormModalBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5ccce0: RFWndEx::RFWndEx, RFModalRenderer (0x5e51f0),
	// m_updateRegistered cleared, m_messageLabel cleared.
	RFNoticeDlg();
	// [1] 0x5cd170 (scalar deleting destructor; the sub-object's [0]
	// 0x5ccd60 adjusts this by -0x180 and jumps to it); ~RFNoticeDlg
	// 0x5ccd70 restores RFFormModalBase<RFWndEx>'s vtables and runs
	// ~RFModalRenderer (0x5e5200) and ~RFWndEx.
	virtual ~RFNoticeDlg();

	// 0x5cd610 (name ours): the message label gets the Shift-JIS text
	// (0x5e4ef0).
	void SetMessage(char const* text);

protected:
	// [10] 0x5cdea0: OnOkClicked on ok_btn's RFButton::EventClicked, then
	// the label looked up.
	virtual void OnInitialUpdate();

	// 0x5ccbc0 (name ours), ok_btn's EventClicked: Close(1).
	void OnOkClicked(EventArgs& args);

	// 0x0-0x188: RFFormModalBase<RFWndEx>.
	RFLabel* m_messageLabel;        // 0x188 "message"
};
CHECK_SIZE(RFNoticeDlg, 0x18c);
BEGIN_LAYOUT_CHECK(RFNoticeDlg)
	CHECK_FIELD(RFNoticeDlg, m_messageLabel, 0x188);
END_LAYOUT_CHECK

// 0xe78174 (name ours): the RFUseItemDlg instance (its constructor sets
// it, its destructor clears it).
extern RFUseItemDlg* g_useItemDlg;
// 0xe78170 / 0xe78178 (names ours): the parent the last
// CreateUseItemDialog / CreateNoticeDialog was given.
extern RFWnd* g_useItemDlgParent;
extern RFWnd* g_noticeDlgParent;

// 0x5cd6f0 / 0x5cdd60 (parent in ecx): the dialog from
// common_use_item_dialog.xml / common_notice_dialog.xml, made as
// g_jpModalCreateMode (0xe03260, JpLobbyDialogs.h) says; the parent is kept in
// g_useItemDlgParent / g_noticeDlgParent.
RFWnd* CreateUseItemDialog(RFWnd* parent);
RFWnd* CreateNoticeDialog(RFWnd* parent);

#endif // PG_JP_JPCOMMONDIALOGS_H
