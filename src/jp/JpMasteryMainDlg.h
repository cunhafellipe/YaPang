// JP-only game code (no KR counterpart): the character mastery screen's
// root (R558 screen #2, source run 0x5bd3f0-0x5c0d20). RFCharMasteryMainDlg
// (kam_mastery_main_dialog.xml) is opened by CreateCharMasteryMainDlg
// 0x5be450 (callers: the card box handler 0x6142e0 and the CTaskMain
// message handler case at 0x65f60e); its OnInitialUpdate creates the
// character picture (RFCharImgWnd), the status, card and slot dialogs over
// placeholder children, and help_button opens RFCharMasteryAboutDlg.
// Declarations and JP x86 layout from the JP exe's bodies
// (node scripts/disasm.ts --jp 0x<va>). The mastery game state is JP-only:
// every JP game-state read and every JP callee outside this run is a glue
// function below (one per JP callee, JP VA in its comment), declared only
// unless its JP field is proven to be a KR member (JpLobbyGlue.cpp).
#ifndef PG_JP_JPMASTERYMAINDLG_H
#define PG_JP_JPMASTERYMAINDLG_H

#include <stdint.h>
#include <string>

#include "defines.h"
#include "padding.h"
#include "RFFormBase.h"
#include "RFFormModalBase.h"

struct sCharacterInfo;

class RFCheck;
class RFLabelButton;
class RFPicture;
class RFWnd;

// The mastery dialogs created from this screen (R558_screens.tsv), declared
// in their own sources by their owners, with their creators and instances.
class RFCharMasteryStatusDlg;   // JpMasteryStatus.h: kam_mastery_status.xml, 0x188, CreateCharMasteryStatusDlg 0x5c99c0, g_charMasteryStatusDlg 0xe78158
class RFCharMasteryCardDlg;     // JpMasteryCard.h: kam_mastery_card.xml, 0x1b4, CreateCharMasteryCardDlg 0x5bb6e0, g_charMasteryCardDlg 0xe780cc
class RFCharMasteryTileDlg;     // JpMasterySlot.h: slot_mastery_wnd.xml, 0x1ec, CreateMasteryTileDialog 0x5c3db0, RFCharMasteryTileDlg::s_instance 0xe78120

// The mastery character picture (RTTI RFCharImgWnd : RFFormBase<RFWndEx>,
// vtable 0xd0edbc, 34 slots: no new virtuals), loaded from
// kam_char_img_wnd.xml by CreateCharImgWnd 0x5be260. Size 0x188.
class RFCharImgWnd : public RFFormBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5bd530: RFWndEx::RFWndEx, m_charImg = NULL.
	RFCharImgWnd();
	// [1] 0x5bdb40 (scalar deleting destructor); ~RFCharImgWnd 0x5bd550
	// restores RFFormBase<RFWndEx>'s vtable and runs ~RFWndEx.
	virtual ~RFCharImgWnd();

	// 0x5bdd50 (name ours): with a picture and a non-zero type id, the
	// picture "mastery_char_img_<typeId & 0x3ffffff>.png".
	void SetCharacter(uint32_t typeId);

	// 0xe780d8 (name ours): the parent the last CreateCharImgWnd was given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5be3a0: m_charImg = the RFPicture child "char_img".
	virtual void OnInitialUpdate();

	// 0x0-0x184: RFFormBase<RFWndEx> / RFWndEx.
	RFPicture* m_charImg;  // 0x184 child "char_img"
};
CHECK_SIZE(RFCharImgWnd, 0x188);
BEGIN_LAYOUT_CHECK(RFCharImgWnd)
	CHECK_FIELD(RFCharImgWnd, m_charImg, 0x184);
END_LAYOUT_CHECK

// The mastery screen (RTTI RFCharMasteryMainDlg : RFFormModalBase<RFWndEx>,
// vtable 0xd0ee54, 35 slots: [34] new; the RFModalRenderer sub-object at
// +0x180 has vtable 0xd0ee48, its [0] 0x5bd610 adjusts this by -0x180),
// loaded from kam_mastery_main_dialog.xml. Size 0x1a8. Two tabs: status
// (the status and card dialogs) and mastery (the slot dialog).
class RFCharMasteryMainDlg : public RFFormModalBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5bd560: RFWndEx::RFWndEx, RFModalRenderer (0x5e51f0),
	// m_updateRegistered cleared, the instance kept in g_charMasteryMainDlg,
	// the members cleared, m_tab = -1.
	RFCharMasteryMainDlg();
	// [1] 0x5bdb70 (scalar deleting destructor); ~RFCharMasteryMainDlg
	// 0x5bd620 clears g_charMasteryMainDlg, then restores
	// RFFormModalBase<RFWndEx>'s vtables and runs ~RFModalRenderer
	// (0x5e5200) and ~RFWndEx.
	virtual ~RFCharMasteryMainDlg();

protected:
	// [10] 0x5c0160: the handlers (0x5bec60), the character picture, card,
	// status and slot dialogs, title_mastery brought to the top, tab 0.
	virtual void OnInitialUpdate();

public:
	// [34] 0x5bd290 (name ours): on Escape (key code 0x1b), the
	// "ui_dialog_close" sound, Close(2), args handled.
	virtual void OnKeyDownEscape(EventArgs& args);

	// 0xe780e0 (name ours): the parent the last CreateCharMasteryMainDlg
	// was given.
	static RFWnd* s_loadParent;

protected:
	// 0x5bd2f0 (name ours), exit_button RFButton::EventClicked: the
	// "ui_dialog_close" sound, Close(2).
	void OnExitClicked(EventArgs& args);
	// 0x5bd3c0 (name ours), its own EventKeyDown: [34] with the args (a
	// tail jump through the vtable).
	void OnKeyDownForm(EventArgs& args);
	// 0x5bdcc0 (name ours): when tab differs from m_tab (which it does not
	// store), the tab checks set (status for 0, mastery otherwise), the
	// status and card dialogs shown for 0, the slot dialog otherwise.
	void SelectTab(int tab);
	// 0x5bde50 (name ours), status_mgr_tap_check RFCheck::EventChanged:
	// SelectTab(0), the "ui_icon_click" sound.
	void OnStatusTabChanged(EventArgs& args);
	// 0x5bde90 (name ours), mastery_mgr_tap_check RFCheck::EventChanged:
	// SelectTab(1), the "ui_icon_click" sound.
	void OnMasteryTabChanged(EventArgs& args);
	// 0x5be590 (name ours): the character picture over the placeholder
	// char_img_wnd (deleted afterwards), showing the character's type id.
	void CreateCharImg();
	// 0x5be6c0 (name ours): without a status dialog yet, the status dialog
	// over the placeholder status_wnd (deleted afterwards).
	void CreateStatusDlg();
	// 0x5be7e0 (name ours): without a card dialog yet, the card dialog over
	// the placeholder card_wnd (deleted afterwards), given the character's
	// uid.
	void CreateCardDlg();
	// 0x5be920 (name ours): without a slot dialog yet, the slot dialog
	// over the placeholder slot_mastery_wnd (deleted afterwards), set up
	// (0x5c4e70) before it is placed.
	void CreateTileDlg();
	// 0x5bea50 (name ours), help_button RFButton::EventClicked: the about
	// dialog (CreateCharMasteryAboutDlg) shown modal.
	void OnHelpClicked(EventArgs& args);
	// 0x5bec60 (name ours): the handlers, the two tab checks, and
	// m_character (the equipped character's data).
	void SetupHandlers();

	// 0x0-0x184: RFFormModalBase<RFWndEx> (RFModalRenderer at 0x180,
	// m_updateRegistered at 0x184).
	RFCheck* m_statusTab;                  // 0x188 child "status_mgr_tap_check"
	RFCheck* m_masteryTab;                 // 0x18c child "mastery_mgr_tap_check"
	RFCharImgWnd* m_charImgWnd;            // 0x190 CreateCharImg
	RFCharMasteryStatusDlg* m_statusDlg;   // 0x194 CreateStatusDlg
	RFCharMasteryCardDlg* m_cardDlg;       // 0x198 (CreateCardDlg)
	RFCharMasteryTileDlg* m_tileDlg;       // 0x19c CreateTileDlg
	const sCharacterInfo* m_character;     // 0x1a0 the equipped character's data: +0 type id, +4 uid
	int m_tab;                             // 0x1a4 -1 at construction; SelectTab compares, never stores
};
CHECK_SIZE(RFCharMasteryMainDlg, 0x1a8);
BEGIN_LAYOUT_CHECK(RFCharMasteryMainDlg)
	CHECK_FIELD(RFCharMasteryMainDlg, m_statusTab, 0x188);
	CHECK_FIELD(RFCharMasteryMainDlg, m_masteryTab, 0x18c);
	CHECK_FIELD(RFCharMasteryMainDlg, m_charImgWnd, 0x190);
	CHECK_FIELD(RFCharMasteryMainDlg, m_statusDlg, 0x194);
	CHECK_FIELD(RFCharMasteryMainDlg, m_cardDlg, 0x198);
	CHECK_FIELD(RFCharMasteryMainDlg, m_tileDlg, 0x19c);
	CHECK_FIELD(RFCharMasteryMainDlg, m_character, 0x1a0);
	CHECK_FIELD(RFCharMasteryMainDlg, m_tab, 0x1a4);
END_LAYOUT_CHECK

// The mastery help (RTTI RFCharMasteryAboutDlg : RFFormModalBase<RFWndEx>,
// vtable 0xd0ed2c, 34 slots: no new virtuals; the RFModalRenderer
// sub-object at +0x180 has vtable 0xd0ed24, its [0] 0x5bd4b0), loaded from
// kam_mastery_about_dialog.xml by CreateCharMasteryAboutDlg 0x5bded0: nine
// page buttons, forward/backward and confirm; the page shows
// "char_master_help_<page>.jpg". Size 0x1b8.
class RFCharMasteryAboutDlg : public RFFormModalBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	enum {
		PageCount = 9
	};

	// 0x5bd3f0: RFWndEx::RFWndEx, RFModalRenderer (0x5e51f0),
	// m_updateRegistered cleared, the instance kept in
	// g_charMasteryAboutDlg, the page buttons cleared, m_page = 1.
	RFCharMasteryAboutDlg();
	// [1] 0x5bdb10 (scalar deleting destructor); ~RFCharMasteryAboutDlg
	// 0x5bd4c0 clears g_charMasteryAboutDlg, then restores
	// RFFormModalBase<RFWndEx>'s vtables and runs ~RFModalRenderer
	// (0x5e5200) and ~RFWndEx.
	virtual ~RFCharMasteryAboutDlg();

	// 0xe780d0 (name ours): the parent the last CreateCharMasteryAboutDlg
	// was given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5bf150: the button handlers, the page buttons, their labels
	// (the JP texts of the Korean keys "1. ..." to "9. ..."), centred,
	// page 1, the "ui_dialog_open" sound.
	virtual void OnInitialUpdate();

	// 0x5bd250 (name ours), btn_confirm RFButton::EventClicked: the
	// "ui_dialog_close" sound, Close(2).
	void OnConfirmClicked(EventArgs& args);
	// 0x5be010 (name ours): every page button enabled but page's, m_page =
	// page, pic_help_mastery_01 given "char_master_help_<page>.jpg".
	void ShowPage(int page);
	// 0x5be1a0 (name ours): the page before (at least 1), the
	// "ui_icon_click" sound.
	void PreviousPage();
	// 0x5be200 (name ours): the page after (at most 9), the
	// "ui_icon_click" sound.
	void NextPage();
	// 0x5bea70 (name ours; name in edx under LTCG): the RFLabelButton
	// child name's label set to text.
	void SetButtonLabel(const wchar_t* name, const std::wstring& text);
	// label_btn_about_0<n> RFButton::EventClicked: ShowPage(n).
	void OnPage1Clicked(EventArgs& args);  // 0x5bebb0
	void OnPage2Clicked(EventArgs& args);  // 0x5bebc0
	void OnPage3Clicked(EventArgs& args);  // 0x5bebd0
	void OnPage4Clicked(EventArgs& args);  // 0x5bebe0
	void OnPage5Clicked(EventArgs& args);  // 0x5bebf0
	void OnPage6Clicked(EventArgs& args);  // 0x5bec00
	void OnPage7Clicked(EventArgs& args);  // 0x5bec10
	void OnPage8Clicked(EventArgs& args);  // 0x5bec20
	void OnPage9Clicked(EventArgs& args);  // 0x5bec30
	// 0x5bec40 (name ours), btn_forward RFButton::EventClicked: NextPage.
	void OnForwardClicked(EventArgs& args);
	// 0x5bec50 (name ours), btn_backward RFButton::EventClicked:
	// PreviousPage.
	void OnBackwardClicked(EventArgs& args);

	// 0x0-0x184: RFFormModalBase<RFWndEx> (RFModalRenderer at 0x180,
	// m_updateRegistered at 0x184).
	RFLabelButton* m_pageButtons[PageCount];  // 0x188 children "label_btn_about_01".."_09"
	UNKNOWN(0x1ac, 0x1b4);                    // not read in this run
	int m_page;                               // 0x1b4 1..9, 1 at construction
};
CHECK_SIZE(RFCharMasteryAboutDlg, 0x1b8);
BEGIN_LAYOUT_CHECK(RFCharMasteryAboutDlg)
	CHECK_FIELD(RFCharMasteryAboutDlg, m_pageButtons, 0x188);
	CHECK_FIELD(RFCharMasteryAboutDlg, m_page, 0x1b4);
END_LAYOUT_CHECK

// The creators (parent in ecx): the window made as the create mode says
// (0 CreateWnd under the parent, 1 CreateWnd with no parent, 2
// CreateDesktop); the parent is kept in the class's s_loadParent.
// 0x5be450: kam_mastery_main_dialog.xml, mode 0xe03260
// (g_jpModalCreateMode).
RFWnd* CreateCharMasteryMainDlg(RFWnd* parent);
// 0x5bded0: kam_mastery_about_dialog.xml, mode 0xe03260.
RFCharMasteryAboutDlg* CreateCharMasteryAboutDlg(RFWnd* parent);
// 0x5be260: kam_char_img_wnd.xml, mode g_jpWndCreateMode (0xe73dd8).
RFCharImgWnd* CreateCharImgWnd(RFWnd* parent);

// 0xe780e4, 0xe780d4 (names ours): the open mastery screen and help, set
// by their constructors and cleared by their destructors.
extern RFCharMasteryMainDlg* g_charMasteryMainDlg;
extern RFCharMasteryAboutDlg* g_charMasteryAboutDlg;

// --- Declared-only glue (JP callees outside this run; never emulated) ---

// The std::map operator[] 0x50cf40 on the JP CSharedDoc (0xe47dbc) +0x3704
// (KR m_characterMap), key +0xa2a (KR m_myCharacterId) (name ours): the
// equipped character's data (JP 0x204 bytes, KR sCharacterInfo: +0 type id,
// +4 uid), inserted when missing (JpLobbyGlue.cpp).
const sCharacterInfo* Jp_GameState_EquippedCharacterData();
#endif // PG_JP_JPMASTERYMAINDLG_H
