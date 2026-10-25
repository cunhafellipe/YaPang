// JP-only game code (no KR counterpart): the character mastery cards
// (R558 screen #4, 0x5b8000-0x5bd3f0), six Refresh forms read from the JP
// 983 exe (node scripts/disasm.ts --jp 0x<va>, jp_hlil.ts): the card row of
// the mastery dialog (RFCharMasteryCardDlg, kam_mastery_card.xml), its ten
// card slots (RFCardIcon, kam_mastery_card_thumb.xml), the card picker
// (RFCharMasteryCardSelectDlg, kam_mastery_card_select_dialog.xml) and its
// columns (RFCharMasteryCardSelectColumn, kam_mastery_card_select_column.xml),
// the card removal dialog (RFCharacterMasteryCardDetachDlg,
// kam_card_deselect_dialog.xml) and a notice (RFCharMasteryCardNoticeDlg,
// mastery_card_notice_dialog.xml). Members at their JP x86 offsets. The
// mastery game state is JP-only: every read of it and every packet goes
// through the declared-only glue at the end (JP VA in each comment).
//
// Child types: JP dynamic_casts the children to the type descriptors
// 0xe462f8 (RFPicture), 0xe46128 (RFButton), 0xe4628c (RFTextArea),
// 0xe46310 (RFListSC, the card list) and 0xe4635c (RFLabel).
#ifndef PG_JP_JPMASTERYCARD_H
#define PG_JP_JPMASTERYCARD_H

#include <stdint.h>

#include <list>

#include "defines.h"
#include "padding.h"
#include "RFFormBase.h"
#include "RFFormModalBase.h"
#include "RFEventArgs.h"
#include "RFSize.h"

class RFButton;
class RFLabel;
class RFListSC;
class RFPicture;
class RFTextArea;
class RFMasteryToolTipWnd;  // JpMasteryTooltipWnd.h (R590)
class RFCardIcon;
struct sItemInfo;
struct sCharacterInfo;
// A JP card record (IFF data, JP-only game state; read through the glue
// at the end).
struct JpCardData;

// An entry of the JP CSharedDoc +0x37e0 card list (std::list nodes): the
// KR sCards of CSharedDoc::m_myCardList (SharedDoc.h). The uses match it
// field for field: m_guid (0x0) is sent with the card packet, m_typeId
// (0x4) is the card's IFF type id (0: none; bits 22-25 the card type),
// m_count (0x14) how many are held ("x%d" on the column; 0: listed nowhere).
struct sCards;
typedef sCards JpCardEntry;

// The card row of the mastery dialog (vtable 0xd0e7f4, 34 slots: no new
// virtuals), size 0x1b4. The constructor keeps the instance in
// g_charMasteryCardDlg (0xe780cc).
class RFCharMasteryCardDlg : public RFFormBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	enum { kCardSlotCount = 10 };

	// 0x5b8c00: RFWndEx::RFWndEx, the instance kept in g_charMasteryCardDlg,
	// the slots and the tooltip cleared.
	RFCharMasteryCardDlg();
	// [1] 0x5b9900 (scalar deleting destructor); ~RFCharMasteryCardDlg
	// 0x5b8c60 clears g_charMasteryCardDlg, then restores RFFormBase<RFWndEx>'s
	// vtable and runs ~RFWndEx.
	virtual ~RFCharMasteryCardDlg();

	// 0xe780c8 (name ours): the parent the last CreateCharMasteryCardDlg
	// was given.
	static RFWnd* s_loadParent;

	// 0x5ba830 (name ours): with show, the tooltip gets text (SetText)
	// and is placed (SetPosition) from the mouse position (0x6038b0 of
	// 0xe73ddc) and its size; then the tooltip's Show(show) ([34]).
	void ShowCardTooltip(bool show, const char* text);

	// 0x5bbdb0 (name ours): with a uid (not -1) whose character data the
	// game state has, each slot gets the character's card in it (by
	// MasteryCardSlotToServerSlot), the lock (IsMasteryCardLockableSlot,
	// hidden unless the character wears a mastery part) and its view.
	void SetCharacterUid(uint32_t uid);

protected:
	// [10] 0x5bbe70: CreateCardSlots, then m_tooltip = the mastery
	// tooltip loaded under the super window (RFMasteryToolTipWnd::Load).
	virtual void OnInitialUpdate();

	// 0x5bb880 (name ours): for "slot_wnd1" .. "slot_wnd10", a card icon
	// (CreateCardIcon 0x5bb5a0) placed at the placeholder's position and
	// shown (ShowForm), then the placeholder deleted.
	void CreateCardSlots();

	UNKNOWN(0x184, 0x188);                          // not written by the constructor
	RFCardIcon* m_cardIcons[kCardSlotCount];        // 0x188 one per "slot_wnd%d"
	RFMasteryToolTipWnd* m_tooltip;                 // 0x1b0 created by OnInitialUpdate
};
CHECK_SIZE(RFCharMasteryCardDlg, 0x1b4);
BEGIN_LAYOUT_CHECK(RFCharMasteryCardDlg)
	CHECK_FIELD(RFCharMasteryCardDlg, m_cardIcons, 0x188);
	CHECK_FIELD(RFCharMasteryCardDlg, m_tooltip, 0x1b0);
END_LAYOUT_CHECK

// One card slot (vtable 0xd0e764, 34 slots: no new virtuals), size 0x19c.
class RFCardIcon : public RFFormBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5b8b80: RFWndEx::RFWndEx, m_slotIndex = -1, no card, locked
	// cleared and m_lockInfoHidden set.
	RFCardIcon();
	// [1] 0x5b8bd0 (scalar deleting destructor, ~RFCardIcon inlined:
	// RFFormBase<RFWndEx>'s vtable, then ~RFWndEx).
	virtual ~RFCardIcon();

	// 0xe780c0 (name ours): the parent the last CreateMasteryCardIcon was
	// given (the card row); the detach dialog loads under its super window.
	static RFWnd* s_loadParent;

	// 0x5b8420 (name ours): the slot's card (NULL: none) and its index.
	void SetCard(const JpCardData* card, int slotIndex);
	// 0x5b8440 (name ours): m_locked = locked; the lock info is hidden
	// when unlocked, else as given (locked ? hideLockInfo : true).
	void SetLocked(bool locked, bool hideLockInfo);
	// 0x5ba670 (name ours): card_img gets the card's picture
	// ("<image>.png", Shift-JIS; none without a card); card_img_not is
	// shown for a card with the lock info shown, card_blank for no card
	// with the lock info hidden.
	void UpdateView();

protected:
	// [10] 0x5bccc0: OnClicked, OnMouseEnter and OnMouseLeave on
	// "frame_btn"; then the "card_img", "card_img_not" and "card_blank"
	// pictures.
	virtual void OnInitialUpdate();

	// 0x5bc880 (name ours), frame_btn RFButton::EventClicked: with a card,
	// the detach dialog (when none is open) under s_loadParent's super
	// window, given the card and the slot (0x5ba570), shown modal;
	// without a card, a locked slot opens the common notice dialog
	// (CreateNoticeDialog 0x5cdd60, SetMessage 0x5cd610) with text 0x1884f, another the card picker (when none is open) for the slot
	// (0x5bc080), shown modal.
	void OnClicked(EventArgs& args);
	// 0x5baad0 (name ours), frame_btn EventMouseEnter: a locked slot shows
	// the card row's tooltip with text 0x18850.
	void OnMouseEnter(EventArgs& args);
	// 0x5baba0 (name ours), frame_btn EventMouseLeave: a locked slot hides
	// the card row's tooltip.
	void OnMouseLeave(EventArgs& args);

	int m_slotIndex;            // 0x184 -1 at construction; given to the picker and the detach dialog
	const JpCardData* m_card;   // 0x188 the card in the slot, NULL: none
	RFPicture* m_cardImg;       // 0x18c "card_img"
	RFPicture* m_cardImgNot;    // 0x190 "card_img_not"
	RFPicture* m_cardBlank;     // 0x194 "card_blank"
	bool m_locked;              // 0x198 false at construction
	bool m_lockInfoHidden;      // 0x199 true at construction; the lock tooltip and use-item dialog need it clear
	UNKNOWN(0x19a, 0x19c);
};
CHECK_SIZE(RFCardIcon, 0x19c);
BEGIN_LAYOUT_CHECK(RFCardIcon)
	CHECK_FIELD(RFCardIcon, m_slotIndex, 0x184);
	CHECK_FIELD(RFCardIcon, m_card, 0x188);
	CHECK_FIELD(RFCardIcon, m_cardImg, 0x18c);
	CHECK_FIELD(RFCardIcon, m_cardImgNot, 0x190);
	CHECK_FIELD(RFCardIcon, m_cardBlank, 0x194);
	CHECK_FIELD(RFCardIcon, m_locked, 0x198);
	CHECK_FIELD(RFCardIcon, m_lockInfoHidden, 0x199);
END_LAYOUT_CHECK

// The card picker (modal; vtable 0xd0e63c, 35 slots: [34] 0x5b8320 new;
// the RFModalRenderer sub-object at +0x180 has vtable 0xd0e630), size
// 0x1b4. The constructor keeps the instance in g_masteryCardSelectDlg
// (0xe780b4).
class RFCharMasteryCardSelectDlg : public RFFormModalBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5b8930: RFWndEx::RFWndEx, RFModalRenderer, the children cleared,
	// m_slotIndex = -1, the instance kept in g_masteryCardSelectDlg.
	RFCharMasteryCardSelectDlg();
	// [1] 0x5b9850 (scalar deleting destructor); ~RFCharMasteryCardSelectDlg
	// 0x5b8a00 clears g_masteryCardSelectDlg, then restores
	// RFFormModalBase<RFWndEx>'s vtables and runs ~RFModalRenderer
	// (0x5e5200) and ~RFWndEx.
	virtual ~RFCharMasteryCardSelectDlg();

	// 0xe780b0 (name ours): the parent the last
	// CreateMasteryCardSelectDialog was given.
	static RFWnd* s_loadParent;

	// [34] 0x5b8320 (name ours): Escape closes the dialog (state 2),
	// args handled.
	virtual void OnDialogKey(KeyEventArgs& args);

	// 0x5bb200 (name ours), from a column's click: the card becomes
	// m_selectedCard; with a card picture and a card, the picture gets the
	// card's image ("<image>.png", Shift-JIS) and "_open_card_img_" plays.
	void SelectCard(const JpCardEntry* card);

	// 0x5bc080 (name ours): m_slotIndex = slotIndex, cardType =
	// MasteryCardSlotToCardType; a column per held card of that type
	// (std::for_each 0x5bb9e0 over the card list with AddCardColumn);
	// then SetCardTypeTitle, UpdateUseItem, SetNoticeText and
	// UpdateOkButton.
	void SetSlot(int slotIndex);

	// The std::for_each functor of SetSlot (name ours; its operator() is
	// inlined into 0x5bb9e0): each held card (type id != 0, count > 0)
	// whose type id bits 22-25 are *cardType gets a column under
	// m_cardList, shown and inserted into the list.
	struct AddCardColumn {
		const int* cardType;
		RFCharMasteryCardSelectDlg* dlg;
		void operator()(const JpCardEntry& entry) const;
	};

protected:
	// [10] 0x5bc0f0: 0x5b8790, 0x5bbab0 and 0x5b8340 on the buttons
	// (ok_btn, cancel_btn), then the children below; plays the
	// "ui_dialog_open" sound (not written yet).
	virtual void OnInitialUpdate();

	// 0x5b8790 (shared with the detach dialog): calls [34] with the args.
	void OnKey(EventArgs& args);
	// 0x5b9880 (name ours): for slots 9 and 10, ok_btn is disabled unless
	// the item 0x1a00018f is held.
	void UpdateOkButton();
	// 0x5bb360 (name ours): the card type's title picture (0 character,
	// 1 caddie, 5 NPC; another leaves it).
	void SetCardTypeTitle(int cardType);
	// 0x5bbab0 (name ours), ok_btn RFButton::EventClicked: once (m_selectState),
	// with a selected card, the task waits and the card goes to the server
	// (packet 0x18a, or 0x18b for slots 9 and 10); then "ui_button_ok_click".
	void OnOkClicked(EventArgs& args);
	// 0x5b8340 (name ours), cancel_btn RFButton::EventClicked:
	// "ui_button_cancel_click", Close(2).
	void OnCancelClicked(EventArgs& args);
	// 0x5ba2c0 (name ours): with the use item's IFF record (0x1a00018f),
	// its icon into use_item_img, "<name> x<held>" into use_item_text and
	// the warning (table text 0x18849 for slots 9 and 10, else 0x1884a)
	// into warnning_text, shown.
	void UpdateUseItem();
	// 0x5ba8e0 (name ours): table text 0x1884c formatted with the card
	// type's quoted name (preceded by table text 0x1884b for slots 9 and
	// 10) into notice_text.
	void SetNoticeText(int cardType);

	// 0x0-0x188: RFFormModalBase<RFWndEx> (its ShowModal flag at 0x184).
	RFListSC* m_cardList;       // 0x188 "card_list"
	RFPicture* m_cardTypeTitle; // 0x18c "title_card_type_img"
	RFPicture* m_cardImg;       // 0x190 "card_img"
	RFPicture* m_useItemImg;    // 0x194 "use_item_img"
	RFTextArea* m_useItemText;  // 0x198 "use_item_text"
	RFTextArea* m_noticeText;   // 0x19c "notice_text"
	RFTextArea* m_warningText;  // 0x1a0 "warnning_text"
	RFButton* m_okButton;       // 0x1a4 "ok_btn"
	const JpCardEntry* m_selectedCard; // 0x1a8 zeroed by the constructor; SelectCard
	int m_slotIndex;            // 0x1ac -1 at construction; SetSlot
	bool m_selectState;         // 0x1b0 cleared by the constructor; set once OnOkClicked sent the card
	UNKNOWN(0x1b1, 0x1b4);
};
CHECK_SIZE(RFCharMasteryCardSelectDlg, 0x1b4);
BEGIN_LAYOUT_CHECK(RFCharMasteryCardSelectDlg)
	CHECK_FIELD(RFCharMasteryCardSelectDlg, m_cardList, 0x188);
	CHECK_FIELD(RFCharMasteryCardSelectDlg, m_okButton, 0x1a4);
	CHECK_FIELD(RFCharMasteryCardSelectDlg, m_selectedCard, 0x1a8);
	CHECK_FIELD(RFCharMasteryCardSelectDlg, m_slotIndex, 0x1ac);
	CHECK_FIELD(RFCharMasteryCardSelectDlg, m_selectState, 0x1b0);
END_LAYOUT_CHECK

// One column of the card picker (vtable 0xd0e5a4, 34 slots: no new
// virtuals), size 0x194.
class RFCharMasteryCardSelectColumn : public RFFormBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5b88f0: RFWndEx::RFWndEx, the four members cleared.
	RFCharMasteryCardSelectColumn();
	// [1] 0x5b9820 (scalar deleting destructor); ~RFCharMasteryCardSelectColumn
	// 0x5b8920: RFFormBase<RFWndEx>'s vtable, then ~RFWndEx.
	virtual ~RFCharMasteryCardSelectColumn();

	// 0xe780a8 (name ours): the parent the last
	// CreateMasteryCardSelectColumn was given.
	static RFWnd* s_loadParent;

	// 0x5ba0f0 (name ours): with the entry's card record, the entry
	// becomes m_card, card_img gets "<image>.png" and card_name
	// "x<count> <name>".
	void SetCard(const JpCardEntry* entry);

protected:
	// [10] 0x5bbec0 (body 0x5bbf08): OnClicked on "column_body", then
	// "card_img" and "card_name".
	virtual void OnInitialUpdate();

	// 0x5bba60 (name ours), column_body RFButton::EventClicked: the
	// picker's SelectCard with the column's card, then "ui_icon_click".
	void OnClicked(EventArgs& args);

	const JpCardEntry* m_card;  // 0x184 the column's card, given to SelectCard
	UNKNOWN(0x188, 0x18c);      // zeroed by the constructor
	RFPicture* m_cardImg;       // 0x18c "card_img"
	RFLabel* m_cardName;        // 0x190 "card_name"
};
CHECK_SIZE(RFCharMasteryCardSelectColumn, 0x194);
BEGIN_LAYOUT_CHECK(RFCharMasteryCardSelectColumn)
	CHECK_FIELD(RFCharMasteryCardSelectColumn, m_card, 0x184);
	CHECK_FIELD(RFCharMasteryCardSelectColumn, m_cardImg, 0x18c);
	CHECK_FIELD(RFCharMasteryCardSelectColumn, m_cardName, 0x190);
END_LAYOUT_CHECK

// The card removal dialog (modal; vtable 0xd0e6d4, 35 slots: [34]
// 0x5b8380 new; sub-object vtable 0xd0e6cc), size 0x190. The constructor
// keeps the instance in g_masteryCardDetachDlg (0xe780bc).
class RFCharacterMasteryCardDetachDlg : public RFFormModalBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5b8a70: RFWndEx::RFWndEx, RFModalRenderer, no picture,
	// m_slotIndex = -1, the instance kept in g_masteryCardDetachDlg.
	RFCharacterMasteryCardDetachDlg();
	// [1] 0x5b98d0 (scalar deleting destructor);
	// ~RFCharacterMasteryCardDetachDlg 0x5b8b10 clears
	// g_masteryCardDetachDlg, then ~RFModalRenderer and ~RFWndEx.
	virtual ~RFCharacterMasteryCardDetachDlg();

	// 0xe780b8 (name ours): the parent the last
	// CreateMasteryCardDetachDialog was given.
	static RFWnd* s_loadParent;

	// [34] 0x5b8380 (name ours): Escape plays "ui_dialog_close" and
	// closes the dialog (state 2), args handled.
	virtual void OnDialogKey(KeyEventArgs& args);

	// 0x5ba570 (name ours): with a card, its picture ("<image>.png",
	// Shift-JIS) into m_cardOriginImg when there is one, then m_slotIndex =
	// slotIndex.
	void SetCard(const JpCardData* card, int slotIndex);

protected:
	// [10] 0x5bc9a0: OnKey on its own EventKeyDown, OnDetachClicked on
	// detach_btn, OnCloseClicked on close_btn, then "card_origin_img";
	// plays "ui_dialog_open".
	virtual void OnInitialUpdate();

	// 0x5b8790 (shared with the picker): calls [34] with the args.
	void OnKey(EventArgs& args);
	// 0x5bc6b0 (name ours), detach_btn RFButton::EventClicked: with a slot
	// and a picture, the detach item (IFF 0x1a0000c2); held, the use-item
	// dialog with it, text 0x1884d and OnUseItemOkClicked on its ok_btn;
	// otherwise the notice dialog with text 0x1884e; shown modal.
	void OnDetachClicked(EventArgs& args);
	// 0x5bbc50 (name ours), the use-item dialog's ok_btn: with the detach
	// item held, the task waits and packet 0x18c goes to the server.
	void OnUseItemOkClicked(EventArgs& args);
	// 0x5b83e0 (name ours), close_btn RFButton::EventClicked:
	// "ui_dialog_close", Close(2).
	void OnCloseClicked(EventArgs& args);

	// 0x0-0x188: RFFormModalBase<RFWndEx> (its ShowModal flag at 0x184).
	RFPicture* m_cardOriginImg; // 0x188 "card_origin_img"
	int m_slotIndex;            // 0x18c -1 at construction; SetCard
};
CHECK_SIZE(RFCharacterMasteryCardDetachDlg, 0x190);
BEGIN_LAYOUT_CHECK(RFCharacterMasteryCardDetachDlg)
	CHECK_FIELD(RFCharacterMasteryCardDetachDlg, m_cardOriginImg, 0x188);
	CHECK_FIELD(RFCharacterMasteryCardDetachDlg, m_slotIndex, 0x18c);
END_LAYOUT_CHECK

// A mastery card notice (modal; vtable 0xd0e514, 34 slots; sub-object
// vtable 0xd0e508), size 0x190.
class RFCharMasteryCardNoticeDlg : public RFFormModalBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5b87f0: RFWndEx::RFWndEx, RFModalRenderer, the two children
	// cleared.
	RFCharMasteryCardNoticeDlg();
	// [1] 0x5b97f0 (scalar deleting destructor); ~RFCharMasteryCardNoticeDlg
	// 0x5b8880: ~RFModalRenderer, then ~RFWndEx.
	virtual ~RFCharMasteryCardNoticeDlg();

	// 0xe780a0 (name ours): the parent the last
	// CreateMasteryCardNoticeDialog was given.
	static RFWnd* s_loadParent;

	// 0x5b9e90 (name ours): the card's picture ("<image>.png") into
	// card_img; table text 0x18847 (attached) or 0x18848 formatted with the
	// card's name into message.
	void SetCard(uint32_t typeId, bool attached);

protected:
	// [10] 0x5bad10: OnOkClicked on ok_btn, then "card_img" and
	// "message"; plays "ui_dialog_open".
	virtual void OnInitialUpdate();

	// 0x5b82c0 (name ours), ok_btn RFButton::EventClicked:
	// "ui_button_ok_click", Close(1).
	void OnOkClicked(EventArgs& args);

	// 0x0-0x188: RFFormModalBase<RFWndEx> (its ShowModal flag at 0x184).
	RFPicture* m_cardImg;       // 0x188 "card_img"
	RFTextArea* m_message;      // 0x18c "message"
};
CHECK_SIZE(RFCharMasteryCardNoticeDlg, 0x190);
BEGIN_LAYOUT_CHECK(RFCharMasteryCardNoticeDlg)
	CHECK_FIELD(RFCharMasteryCardNoticeDlg, m_cardImg, 0x188);
	CHECK_FIELD(RFCharMasteryCardNoticeDlg, m_message, 0x18c);
END_LAYOUT_CHECK

// Instances (kept by the constructors, cleared by the destructors).
extern RFCharMasteryCardDlg* g_charMasteryCardDlg;                    // 0xe780cc
extern RFCharMasteryCardSelectDlg* g_masteryCardSelectDlg;        // 0xe780b4
extern RFCharacterMasteryCardDetachDlg* g_masteryCardDetachDlg;   // 0xe780bc
// 0xe780a4 (name ours): tested by ShowMasteryCardNotice; nothing but its
// initializers (0x5b81a0, 0x5b87a0) writes it.
extern RFCharMasteryCardNoticeDlg* g_masteryCardNoticeDlg;

// Creators (name ours): g_jpWndCreateMode's three ways (CreateWnd under
// parent, CreateWnd without one, CreateDesktopWnd), then the class's
// s_loadParent = parent.
RFWnd* CreateCharMasteryCardDlg(RFWnd* parent);                               // 0x5bb6e0 (also declared by JpMasteryMainDlg.h)
RFCardIcon* CreateMasteryCardIcon(RFWnd* parent);                             // 0x5bb5a0
RFCharMasteryCardSelectDlg* CreateMasteryCardSelectDialog(RFWnd* parent);     // 0x5bb060
RFCharacterMasteryCardDetachDlg* CreateMasteryCardDetachDialog(RFWnd* parent);// 0x5bb460
RFCharMasteryCardSelectColumn* CreateMasteryCardSelectColumn(RFWnd* parent);  // 0x5baf20
RFCharMasteryCardNoticeDlg* CreateMasteryCardNoticeDialog(RFWnd* parent);     // 0x5babd0

// 0x5bb820 (name ours; called twice by the JP packet handler 0x9d0b00):
// closes the detach dialog and the picker (state 2), then, unless
// g_masteryCardNoticeDlg is set, the card notice under parent for the
// card, shown modal.
void ShowMasteryCardNotice(RFWnd* parent, uint32_t typeId, bool attached);

// 0x5b8120 (name ours; JpMasteryCard.cpp): a card slot's card type (0
// character, 1 caddie, 5 NPC, 6 none); SetSlot's argument to
// SetCardTypeTitle, the list filter and the notice text.
int MasteryCardSlotToCardType(int slot);
// 0x5b8010 (name ours; JpMasteryCard.cpp): slot 6 is the one card slot
// that can be locked (0x5bbdb0 gives it to RFCardIcon::SetLocked).
bool IsMasteryCardLockableSlot(int slot);

// JP game glue (declared only; JP-only state and windows owned elsewhere).
// The card record's image name (char[] at +0xc1).
const char* JpCardData_GetImageName(const JpCardData* card);
// The card record's name (char[] at +0x8).
const char* JpCardData_GetName(const JpCardData* card);
// The card record's stats (short[5] at +0xea, JpMasteryStatInfo::GetCardStat
// 0x5e4340; name ours): KR sCard::m_stat (0xba), the JP card record being
// 0x30 bytes longer than the KR one before it (JpMasteryGlue.cpp).
int16_t JpCardData_GetStat(const JpCardData* card, uint32_t stat);
// The JP CSharedDoc (0xe47dbc) +0x37e0 card list (its head node pointer
// there): the held mastery cards, the KR CSharedDoc::m_myCardList.
std::list<JpCardEntry>& Jp_GameState_CardList();
// The std::map operator[] 0x50cf40 on the JP CSharedDoc (0xe47dbc)
// +0x3704 (the owned characters by uid; JP value 0x204 bytes): the
// character data of a uid, inserted when missing.
const sCharacterInfo* Jp_GameState_CharacterData(uint32_t uid);
// The JP character data's card type id of a server card slot (dwords at
// the unaligned JP +0x1d1; no KR sCharacterInfo counterpart).
uint32_t JpCharacterData_GetCardTypeId(const sCharacterInfo* data, int serverSlot);
// The 12 card type ids of the JP +0x1d1 by server card slot, decoded with
// the JP character record (0x9c6024): port storage (JpMasteryGlue.cpp).
void JpCharacterData_SetCardTypeIds(const sCharacterInfo* data, const uint32_t cardTypeIds[12]);
// 0x6332f0 (thiscall on the JP CSharedDoc; name ours): one of the
// character's 24 part type ids (JP +0xc) has an IFF part record
// (0xa17160 on +0x20c) whose word +0x22a is at least 1.
bool Jp_GameState_CharacterHasMasteryPart(const sCharacterInfo* data);
// CItemManager::FindCard on the JP CSharedDoc (0xe47dbc) +0x20c: the JP
// card record of a card type id.
const JpCardData* Jp_FindCard(uint32_t typeId);
// 0x63b570 on the JP CSharedDoc (thiscall): the inventory item of a type
// id, NULL when not held.
const sItemInfo* Jp_GameState_FindInventoryItem(uint32_t typeId);
// The JP CSharedDoc +0x3c0 container is not empty (its head's first link
// is not the head; which container is not read yet). 0x5bbc50 tests it.
bool JpMasteryCard_StateListNotEmpty();
// The current task's wait message (CTaskManager 0xe73df0 GetCurrentTask,
// vfn +0x30 with {0, 1, 0...}), posted before a mastery card packet.
void JpMastery_PostTaskWait();
// WSendPacket(packetId): the equipped character's JP data [0] (0x50cf40),
// CSharedDoc +0xa2a, the item's type id, its serial and slot, then Send(0).
void JpMasteryCard_SendCardPacket(uint16_t packetId, uint32_t typeId, uint32_t serial, int32_t slot);

// The JP card manager (0xe74210) card stat appliers that
// Jp_CardManager_RefreshMasteryStats 0x80aac0 calls for a card held in a
// character record's slot, by the card kind ((typeId >> 22) & 0xf), with
// the player index (thiscall on the instance; names ours; no KR
// counterpart: the KR CCardManager applies sSCardAvilityPeriodInfo
// records, these the card IFF record): 0x8093d0 kind 0 (character card),
// 0x809570 kind 1 (caddie card), 0x8095e0 kind 5.
void Jp_CardManager_ApplyCharacterCard(const JpCardData* card, int playerIndex);
void Jp_CardManager_ApplyCaddieCard(const JpCardData* card, int playerIndex);
void Jp_CardManager_ApplyKind5Card(const JpCardData* card, int playerIndex);

#endif // PG_JP_JPMASTERYCARD_H
