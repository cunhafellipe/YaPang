// JP-only game code: the JP CRealMyRoomMain, the lobby My Room page's Fresh
// form "realmyroom" (an FrTaskForm, JpFrForms.h), which CMyRoomWidget
// (JpMyRoomWidget.h) loads through CTaskWidget::Load. Named
// JpRealMyRoomMain here: the KR CRealMyRoomMain
// (yapang/src/client/ProjectG/RealMyRoomMain.h, a CTaskMain task of the KR
// REALMYROOM layout) is compiled into the port too, and the JP class is its
// descendant moved onto a form, like the JP CShopMain (JpShopMain.h).
//
// JP x86 facts (node scripts/disasm.ts --jp 0x<va>):
// - creator 0x620b90 (registered for "realmyroom" by the static init
//   0xcd2220) -> CreateForm<CRealMyRoomMain> 0x620a00: new 0xae0, ctor
//   0x61f1f0, Fresh create 0xbe6940, the thread-local +0xb4 slot, 0x47cc60
//   with the RTTI;
// - RTTI 0xe78c5c (filled at static init), base FrTaskForm;
// - vtables 0xd1497c (FrForm, 41 slots: [0] GetRTTI 0x61d770, [1] deleting
//   destructor 0x61f500, [6] 0x606e40, [9] OnProc 0x60ca50, [32] OnInit
//   0x620c30, [38] Start 0x60ca40, [39] Shutdown 0x6165d0, [40] HandleMsg
//   0x623500, the rest FrTaskForm's 0xd0877c), 0xd1496c (FrCmdTarget: [0]
//   thunk 0x61d780, [1] GetMessageMap 0x604b50), 0xd14964, 0xd1495c
//   (CHandOwner at +0x120: [0] OnAnotherHand 0x611f50);
// - message map 0xe1347c (base FrForm's map 0xe4343c, rows 0xe13488).
//
// Layout (x86, from the constructor 0x61f1f0, OnInit 0x620c30 and Shutdown
// 0x6165d0): FrTaskForm 0x120, CHandOwner +0x120, then the KR members moved:
// three std::maps (heads +0x454, +0x464, +0x474; 0x18-byte nodes), an object
// at +0x588 (ctor 0x98ad90, Shutdown 0x98af10; +0x594 read by Shutdown), the
// shared doc's +0x3704 copy at +0x710 (a std::map, heads +0x714, nodes
// 0x218), 0x74 bytes copied from the shared doc +0xa26 at +0x720,
// +0x52c = 0 / +0x530 = 0x16 (OnInit), a std::list at +0x954, +0x98c, an
// object of 0x94 at +0xa78 (ctor 0x7f6970, OnInit's 0x7f3860), +0xaa0, an
// object at +0xac0 (ctor 0x4797a0), +0xab8, the byte +0xabc, +0xad0 = -1.
// The controls the INIT handlers store (+0x144, +0x1dc..+0x1ec ...) are in
// the padding until the message map's rows are written.
//
// Status: GetRTTI, GetMessageMap, the creator, Start, SelectMenu's case 9,
// SetMainTab 0x61e4e0, SetSubTab 0x61d7a0, OnTabMainUp 0x61f530,
// RefreshCharacter 0x61eaa0, ShowUpgradeButtons 0x605010 and most INIT
// handlers are the JP ones; the constructor, the destructor, OnInit,
// Shutdown, HandleMsg, OnAnotherHand, the preview helpers 0x605530 /
// 0x605640 (the +0x588 job is not modelled), BuildDisplayStand 0x61af80
// (sub tab 0x16 only), OnItemListOwnerDraw 0x6166b0 (frame, icon, name),
// OnItemListBtnDown 0x620e00 / SelectItemList 0x61f8b0 (the parts and
// card cases in the lobby) and the INIT handlers whose comment says "Port
// body" leave out what the port does not model (the shared-doc copies,
// the 0xe7688c tooltips, the card page). The message map (176 JP rows,
// list with handler VAs in temps/orchestrator/refresh/R990_rows.txt) has
// its INIT rows, item_list OWNERDRAW / LBUTTONDOWN, tab_main / tab_sub
// LBUTTONUP, close_wnd (0x604eb0) and storage_open_btn (0x6229b0); the
// other owner draws and clicks (item_list RBUTTONDOWN 0x622330 / MOUSEMOVE
// 0x607010 / DBLCLICK 0x614be0, equip_cancel 0x61a640, equip_reset
// 0x61a6c0, item_slot 0x607d30 / 0x61a710, UnselectItemList 0x61dfb0)
// are not written.
#ifndef PG_JP_JPREALMYROOMMAIN_H
#define PG_JP_JPREALMYROOMMAIN_H

#include <map>
#include <stdint.h>

#include "defines.h"
#include "padding.h"
#include "frcmdtarget.h"
#include "frform.h"
#include "AnotherHand.h"
#include "JpFrForms.h"
#include "wtypes.h"
#include "shared/globalgamedefine.h"

class Bitmap;
class FrArea;
class FrButton;
class FrEdit;
class FrGaugeBar;
class FrGroupBox;
struct FrListItem;
class FrListBox;
class FrTabButton;
class FrViewer;
class FrWnd;
class FrWndManager;
class MsgObject;

// The JP CCardBook (new 0x94 in the constructor 0x61f1f0, ctor 0x7f6970,
// vtable 0xd2ad08): the my-room card album. Its JP layout is not the KR
// CCardBook's (CCardBook.h: seven tab buttons here, the KR has five), so
// only the controls the "realmyroom" INIT handlers store are declared.
// The card page (+0x34, its CCardSlot array at +0x4, 0x3c each) is not
// ported: it stays NULL.
class JpCardBook {
public:
	// Port body (JP 0x7f6970: ClearVariables, LoadImages, the card pack /
	// card pages): the controls start NULL.
	JpCardBook(void);                                                 // 0x7f6970
	virtual ~JpCardBook(void);

	PADDING(0x4, 0x34);
	void* m_cardPage;                // 0x34, the card page (not ported: NULL)
	PADDING(0x38, 0x68);
	FrArea* m_bookWnd;               // 0x68 "card_book"
	FrArea* m_propertyWnd;           // 0x6c "card_property"
	// 0x70 "card_pack", 0x74..0x84 "card_volume1".."card_volume5",
	// 0x88 "card_sp"
	FrButton* m_tabButtons[7];       // 0x70
	FrButton* m_useButton;           // 0x8c "card_use"
	FrListBox* m_pageIndex;          // 0x90 "cardpage_index"
};
// size 0x94 (x86)

class JpRealMyRoomMain : public FrTaskForm, public CHandOwner {
	LAYOUT_CHECK_FRIEND;
public:
	// The KR CRealMyRoomMain's nested types (same layout): an item list
	// row's button area (LoadMyRoomImages, OnItemListOwnerDraw) and a
	// select icon pair.
	struct sItemListArea {
		WRect rect;                    // 0x0
		const Bitmap* normal;          // 0x10
		const Bitmap* over;            // 0x14
		UNKNOWN(0x18, 0x1C);
	};
	struct sSelectIcon {
		const Bitmap* normal;          // 0x0
		const Bitmap* over;            // 0x4
		UNKNOWN(0x8, 0xC);
	};

	static const WRTTI m_RTTI;                                        // 0xe78c5c
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x61d770

	// Port bodies (JP 0x61f1f0, 0x61f500 deleting); see the class comment.
	JpRealMyRoomMain(void);                                           // 0x61f1f0
	virtual ~JpRealMyRoomMain(void);                                  // [1] 0x61f500 (deleting)

	// Port bodies (Start is the JP one): the JP OnInit 0x620c30 fills the page from the shared
	// doc and the controls of the INIT handlers (not written yet), then
	// calls FrForm::OnInit (0xbe6570); Shutdown 0x6165d0 tears the page
	// down (the equip senders 0x613590 ... 0x60e810, 0x60e6c0, the shared
	// doc 0x63de40, 0x605530(1), the +0x588 and +0xa78 objects, 0x614530,
	// 0x614410, the 0xe741e4 singleton), then FrTaskForm::Shutdown and closes the RFWnd 0xe780e4; HandleMsg
	// 0x623500 is the KR CRealMyRoomMain::HandleMsg.
	virtual bool OnInit(void);                                        // [32] 0x620c30
	virtual bool Start(void);                                         // [38] 0x60ca40
	virtual bool Shutdown(void);                                      // [39] 0x6165d0
	virtual bool HandleMsg(const MsgObject& msg);                     // [40] 0x623500

	// Its creator, registered for "realmyroom" (0xcd2220); __fastcall in
	// the JP exe: return CreateForm<CRealMyRoomMain>(pManager, NULL, name,
	// NULL) (0x620a00).
	static FrWnd* Create(const char* name, FrWndManager* pManager);   // 0x620b90

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe13488

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe1347c
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0x604b50

	// Port body (the JP 0x611f50 runs the KR CRealMyRoomMain's
	// another-hand jobs): nothing is queued while OnInit is a port body.
	virtual void OnAnotherHand(int nWork, void* pData);               // 0x611f50

	// Message-map handlers (the rows written so far; the row list with
	// the handler VAs: temps/orchestrator/refresh/R990_rows.txt).
	void OnChatBgInit(intptr_t param);                                // 0x607270
	void OnMiddleUiInit(intptr_t param);                              // 0x6074b0
	void OnMainBg1Init(intptr_t param);                               // 0x6072f0
	void OnMainBg2Init(intptr_t param);                               // 0x607360
	void OnMainBg3Init(intptr_t param);                               // 0x6073d0
	void OnMainBg4Init(intptr_t param);                               // 0x607440
	void OnItemListInit(intptr_t param);                              // 0x606e90
	void OnTabMainInit(intptr_t param);                               // 0x607080
	void OnTabMainUp(void);                                           // 0x61f530
	void OnTabSubInit(intptr_t param);                                // 0x607190
	void OnTabSubUp(void);                                            // 0x61f6b0
	void OnItemSearchBtnInit(intptr_t param);                         // 0x607200
	void OnCloseWndUp(void);                                          // 0x604eb0 close_wnd
	void OnStorageOpenBtnUp(void);                                    // 0x6229b0 storage_open_btn
	void OnTabTxt2Init(intptr_t param);                               // 0x607520
	void OnTabTxt3Init(intptr_t param);                               // 0x607590
	void OnPetInit(intptr_t param);                                   // 0x607600
	void OnSkinPetInit(intptr_t param);                               // 0x607670
	void OnSkinPet2Init(intptr_t param);                              // 0x6076e0
	void OnItemDescBgInit(intptr_t param);                            // 0x607750
	void OnItemDescInit(intptr_t param);                              // 0x6078f0
	// Owner draws (JpRealMyRoomMainDraw.cpp; the FRCMD_OWNERDRAW rows of
	// 0xe13488 not written yet: pet 0x61f6d0, fitem_list 0x60fb50,
	// card_book_back_bg 0x6056e0, card_book
	// 0x605700, cardpacklist 0x60ff10, item_storage_bgi 0x60a3e0,
	// item_storage_list 0x60a6b0, item_storage_index 0x60a8d0,
	// item_storage_dragdrop 0x60abf0 are not written yet).
	void OnItemListOwnerDraw(intptr_t param);                         // 0x6166b0 middle_ui.item_list
	void OnItemListBtnDown(void);                                     // 0x620e00 middle_ui.item_list LBUTTONDOWN
	// 0x61f8b0 (KR name, from the JP's decorated name): wears the row's
	// item (the parts case only, JpRealMyRoomMainDraw.cpp).
	bool SelectItemList(FrListItem* pItem);                           // 0x61f8b0
	void OnItemDescBgOwnerDraw(intptr_t param);                       // 0x6077c0 middle_ui.item_desc_bg
	void OnItemSlotOwnerDraw(intptr_t param);                         // 0x60d0d0 middle_ui.sbox.item_slot
	void OnItemHideDescBgOwnerDraw(intptr_t param);                   // 0x6093d0 item_hide_desc_bg
	void OnItemSlotBoxInit(intptr_t param);                           // 0x607ac0
	void OnEquipCancelInit(intptr_t param);                           // 0x607b60
	void OnEquipResetInit(intptr_t param);                            // 0x607c10
	void OnItemSlotInit(intptr_t param);                              // 0x607cc0
	void OnGiftBgInit(intptr_t param);                                // 0x607d40
	void OnGiftDescInit(intptr_t param);                              // 0x607dc0
	void OnStatGroupInit(intptr_t param);                             // 0x607e40
	void OnStatTextInit(intptr_t param);                              // 0x607ed0
	void OnPowerBarInit(intptr_t param);                              // 0x607fa0
	void OnControlBarInit(intptr_t param);                            // 0x608010
	void OnImpactBarInit(intptr_t param);                             // 0x608080
	void OnSpinBarInit(intptr_t param);                               // 0x6080f0
	void OnCurveBarInit(intptr_t param);                              // 0x608160
	void OnUpDownGroupInit(intptr_t param);                           // 0x6081d0
	void OnUpgradeInit(intptr_t param);                               // 0x608240
	void OnDowngradeInit(intptr_t param);                             // 0x6082c0
	void OnMasteryInit(intptr_t param);                               // 0x6083c0
	void OnClubWorkInit(intptr_t param);                              // 0x608340
	void OnFurnitureTabMainInit(intptr_t param);                      // 0x6091c0
	void OnFurnitureListInit(intptr_t param);                         // 0x6092c0
	void OnItemHideDescBgInit(intptr_t param);                        // 0x609350
	void OnItemHideDescInit(intptr_t param);                          // 0x607990
	void OnItemHideNameInit(intptr_t param);                          // 0x607a40
	void OnCardBookInit(intptr_t param);                              // 0x6098a0
	void OnCardPackListInit(intptr_t param);                          // 0x614260
	void OnCardPageIndexInit(intptr_t param);                         // 0x609910
	void OnCardPropertyInit(intptr_t param);                          // 0x6099d0
	void OnCardPackInit(intptr_t param);                              // 0x609a40
	void OnCardUseInit(intptr_t param);                               // 0x609ad0
	void OnCardVolume1Init(intptr_t param);                           // 0x609b50
	void OnCardVolume2Init(intptr_t param);                           // 0x609be0
	void OnCardVolume3Init(intptr_t param);                           // 0x609c70
	void OnCardVolume4Init(intptr_t param);                           // 0x609d00
	void OnCardVolume5Init(intptr_t param);                           // 0x609e20
	void OnCardSpInit(intptr_t param);                                // 0x609d90
	void OnCardPackSlot1Init(intptr_t param);                         // 0x609eb0
	void OnCardPackSlot2Init(intptr_t param);                         // 0x609f20
	void OnCardPackSlot3Init(intptr_t param);                         // 0x609f90
	void OnCardPackSlot4Init(intptr_t param);                         // 0x60a000
	void OnCardPackSlot5Init(intptr_t param);                         // 0x60a070
	void OnCardPackSlot6Init(intptr_t param);                         // 0x60a0e0
	void OnCardPackSlot7Init(intptr_t param);                         // 0x60a150
	void OnCardPackSlot8Init(intptr_t param);                         // 0x60a1c0
	void OnCardSlot1Init(intptr_t param);                             // 0x6102b0
	void OnCardSlot2Init(intptr_t param);                             // 0x610320
	void OnCardSlot3Init(intptr_t param);                             // 0x610390
	void OnCardSlot4Init(intptr_t param);                             // 0x610400
	void OnCardSlot5Init(intptr_t param);                             // 0x610470
	void OnCardSlot6Init(intptr_t param);                             // 0x6104e0
	void OnCardSlot7Init(intptr_t param);                             // 0x610550
	void OnCardSlot8Init(intptr_t param);                             // 0x6105c0
	void OnCardSlot9Init(intptr_t param);                             // 0x610630
	void OnCardSlot10Init(intptr_t param);                            // 0x6106a0
	void OnCardSlot11Init(intptr_t param);                            // 0x610710
	void OnCardSlot12Init(intptr_t param);                            // 0x610780
	void OnItemStoragePangInit(intptr_t param);                       // 0x60a420
	void OnPangMinusInit(intptr_t param);                             // 0x60a4a0
	void OnPangPlusInit(intptr_t param);                              // 0x60a560
	void OnItemStorageListInit(intptr_t param);                       // 0x60a620
	void OnItemStorageIndexInit(intptr_t param);                      // 0x60a850
	void OnItemStoragePageNextInit(intptr_t param);                   // 0x60a940
	void OnItemStoragePagePrevInit(intptr_t param);                   // 0x60aa00
	void OnItemStorageCloseInit(intptr_t param);                      // 0x60aac0
	void OnItemStorageDragDropInit(intptr_t param);                   // 0x60ab70

	// Helpers of the INIT handlers and the tabs (names ours).
	// 0x6059a0: hides and disables a card-pack slot button.
	static void HideCardPackSlot(FrButton* pButton, int slot);        // 0x6059a0
	// 0x60a330: binds a card slot button to the card page's slot.
	void BindCardSlot(FrButton* pButton, unsigned char slot);         // 0x60a330
	// 0x605010: the mastery button, and the club-work / upgrade /
	// downgrade buttons.
	void ShowUpgradeButtons(bool showMastery, bool showClubWork);     // 0x605010
	// 0x61e4e0: selects a main tab and fills tab_sub with its sub tabs.
	void SetMainTab(int tab, bool force);                             // 0x61e4e0
	// 0x61d7a0 (1195 bytes; a port body without the list builders):
	// selects a sub tab and builds its item list (BuildDisplayStand ...).
	void SetSubTab(int subTab, bool force);                           // 0x61d7a0
	// 0x6056a0 / 0x6056c0: shows / hides stat_group.
	void ShowStatGroup(void);                                         // 0x6056a0
	void HideStatGroup(void);                                         // 0x6056c0
	// 0x61eaa0 (the KR ChangeCharacter): shows the owned character `id`
	// (with onlyCurrent, only when it is the equipped one) and rebuilds the
	// character tabs' list.
	void RefreshCharacter(unsigned long id, bool onlyCurrent);        // 0x61eaa0
	// 0x61af80 (KR name, from the JP's decorated name): fills item_list
	// for a sub tab (jump table 0x61c82c over 0x61c88c).
	void BuildDisplayStand(int subTab);                               // 0x61af80
	// 0x605530 / 0x605640 (names ours): drop the character preview (after
	// waiting for the +0x588 job when wait), or queue its build for the
	// type id at typeId unless the preview already shows it.
	void ReleasePreview(bool wait);                                   // 0x605530
	void LoadPreview(const uint32_t* typeId, bool force);             // 0x605640
	// 0x61a750 (the KR SetLevelBar, not written): the five stat bars'
	// "%d/%d" captions.
	void SetLevelBar(void);                                           // 0x61a750

	// The KR CRealMyRoomMain::SelectMenu with the JP menu numbers (jump
	// table 0x622914, cases 0..9): only case 9 (close) is written.
	void SelectMenu(unsigned char menu);                              // 0x622630

	// The KR equip senders (KR names, the KR SelectMenu case 11 order; not
	// written yet: they read the edited equipment members in the padding).
	void Net_EquipDresserParts(void);                                 // 0x613590
	void Net_EquipCharacter(void);                                    // 0x60e4b0
	void Net_EquipCaddie(void);                                       // 0x614d20
	void Net_EquipItem(void);                                         // 0x60e550
	void Net_EquipGear(void);                                         // 0x60e600
	void Net_EquipSkin(void);                                         // 0x613930
	void Net_EquipMascot(void);                                       // 0x614de0
	// JP only (name ours): C->S 0x20 kind 9 with the shared doc's 0x63f3f0
	// record (+0x4, the 16 bytes at +0x1b8) when there is one (SelectMenu
	// case 9, Shutdown).
	void Net_SendEquipKind9(void);                                    // 0x60e810
	// KR name (the bitmaps of the page, from +0x270: Fresh::RegisterBitmap
	// over the name table 0xe348e0..0xe34908 ...; 3428 bytes, not written).
	void LoadMyRoomImages(void);                                      // 0x608430

	PADDING(0x124, 0x128);
	FrGroupBox* m_middleUi;              // 0x128 "middle_ui"
	PADDING(0x12c, 0x134);
	FrTabButton* m_mainTab;              // 0x134 "middle_ui.tab_main"
	FrTabButton* m_subTab;               // 0x138 "middle_ui.tab_sub"
	PADDING(0x13c, 0x144);
	FrListBox* m_itemList;               // 0x144 "middle_ui.item_list"
	FrArea* m_tabText[2];                // 0x148 the two "middle_ui.tabtxt2" rows
	PADDING(0x150, 0x158);
	FrGroupBox* m_statGroup;             // 0x158 "stat_group"
	// 0x15c "epower", "econtrol", "eimpact", "espin", "ecurve"
	FrGaugeBar* m_statBar[5];            // 0x15c
	FrArea* m_itemDescBg;                // 0x170 "middle_ui.item_desc_bg"
	FrEdit* m_itemDesc;                  // 0x174 "middle_ui.item_desc"
	FrViewer* m_itemHideDescBg;          // 0x178 "item_hide_desc_bg"
	FrEdit* m_itemHideDesc;              // 0x17c "item_hide_desc"
	FrEdit* m_itemHideName;              // 0x180 "item_hide_name"
	FrArea* m_skinPet[2];                // 0x184 "middle_ui.skin_pet", "skin_pet2"
	FrArea* m_pet;                       // 0x18c "middle_ui.pet"
	FrListBox* m_itemSlot;               // 0x190 "middle_ui.sbox.item_slot"
	FrGroupBox* m_slotBox;               // 0x194 "middle_ui.sbox"
	FrGroupBox* m_upDownGroup;           // 0x198 "updown_group"
	FrButton* m_equipCancel;             // 0x19c "middle_ui.sbox.equip_cancel"
	FrButton* m_equipReset;              // 0x1a0 "middle_ui.sbox.equip_reset"
	FrButton* m_upgradeButton;           // 0x1a4 "updown_group.upgrade"
	FrButton* m_downgradeButton;         // 0x1a8 "updown_group.downgrade"
	PADDING(0x1ac, 0x1b0);
	FrButton* m_clubWorkButton;          // 0x1b0 "updown_group.clubwork_m"
	FrButton* m_masteryButton;           // 0x1b4 "updown_group.mastery_m"
	FrViewer* m_giftBg;                  // 0x1b8 "middle_ui.gift_bg"
	FrEdit* m_giftDesc;                  // 0x1bc "middle_ui.gift_desc"
	FrArea* m_statText;                  // 0x1c0 "stat_group.stat_text"
	FrTabButton* m_furnitureTab;         // 0x1c4 "ftab_main"
	FrListBox* m_furnitureList;          // 0x1c8 "fitem_list"
	PADDING(0x1cc, 0x1dc);
	FrArea* m_mainBg[4];                 // 0x1dc "middle_ui.main_bg1".."4"
	FrViewer* m_chatBg;                  // 0x1ec "chat_bg"
	PADDING(0x1f0, 0x248);
	FrButton* m_itemSearchButton;        // 0x248 "item_search_btn"
	// The page's pictures (LoadMyRoomImages 0x608430; the KR members, KR
	// offset - 0x230 unless noted).
	const Bitmap* m_pSelectItemMark;       // 0x24c "SelectItem_Mark" (JP only)
	PADDING(0x250, 0x254);
	const Bitmap* m_cardPackItemBmp[4];    // 0x254 "item_base_n/o/d/empty"
	PADDING(0x264, 0x270);
	const Bitmap* m_pStatPointBitmap[5][2]; // 0x270 dot_files
	const Bitmap* m_pListBgBitmap[3];      // 0x298 "select_up/mid/low"
	const Bitmap* m_pListEmptyBitmap;      // 0x2a4 "select_empty"
	const Bitmap* m_pListSelBitmap;        // 0x2a8 "select_cursor1"
	const Bitmap* m_pListOverBitmap;       // 0x2ac "select_cursor0"
	const Bitmap* m_pStatPartBitmap[3][3]; // 0x2b0 "stat_char/club/caddie_n/o/d"
	const Bitmap* m_pStatAllBitmap[3];     // 0x2d4 "stat_all_n/o/d"
	sSelectIcon m_selectIcons[5];          // 0x2e0 "select_1_n" .. "select_5_o"
	const Bitmap* m_pSelectDivide;         // 0x31c "select_divide"
	const Bitmap* m_pSkinLevelBitmap[2];   // 0x320 "i_male", "i_female"
	const Bitmap* m_pSkinNameBitmap;       // 0x328 "master_sign"
	const Bitmap* m_pSkinSlotBitmap;       // 0x32c "team_base"
	const Bitmap* m_pItemSlotBitmap[3];    // 0x330 "item_slot_n/o/d" (OnItemSlotOwnerDraw)
	const Bitmap* m_pItemSlotEmpty;        // 0x33c "item_slot_empty"
	const Bitmap* m_pItemDescBottom[2];    // 0x340 "itemdesc_01/02" (OnItemDescBgOwnerDraw)
	PADDING(0x348, 0x358);
	const Bitmap* m_pItemDescFrame[9];     // 0x358 "rmr_tip_frm_01".."09" (item_hide_desc_bg's draw 0x6093d0)
	const Bitmap* m_pUccStateBitmap[3];    // 0x37c (KR - 0x244) "ucc_state_original/copied/temp_saved"
	PADDING(0x388, 0x3ec);
	const Bitmap* m_pFurnitureItemBg;      // 0x3ec (KR - 0x254) "rmr_arg_item_base_n"
	const Bitmap* m_pFurnitureItemDownBg;  // 0x3f0 "rmr_arg_item_base_d"
	const Bitmap* m_pFurnitureSelFrame;    // 0x3f4 "rmr_arg_item_base_s"
	const Bitmap* m_pFurnitureEmptyBg;     // 0x3f8 "rmr_arg_item_base_empty"
	const Bitmap* m_pMarkBegin;            // 0x3fc "mark_begin" (JP only)
	PADDING(0x400, 0x448);
	float m_itemHideDescLeft;            // 0x448 (item_hide_desc's left)
	float m_itemHideDescBottom;          // 0x44c (its top + height)
	PADDING(0x450, 0x48c);
	// The shown character (RefreshCharacter 0x61eaa0, the KR
	// ChangeCharacter members): pointers into its record in CSharedDoc's
	// character map (the port keeps the KR sCharacterInfo there).
	const unsigned char* m_pCharLevels;  // 0x48c (the record's stat levels)
	const DWORD* m_pCharParts;           // 0x490 (its part type ids, 24)
	uint32_t m_charPartIds[24];          // 0x494 (its part item ids, copied under local content 0x58)
	DWORD* m_pCharCards;                 // 0x4f4 (its card / aux part type ids, 5)
	uint32_t m_charTypeId;               // 0x4f8 (the preview job's argument)
	uint32_t m_charSerial;               // 0x4fc (its id, the doc's m_myCharacterId)
	PADDING(0x500, 0x52c);
	int m_mainTabIndex;                  // 0x52c (SetMainTab; OnInit 0)
	int m_subTabIndex;                   // 0x530 (SetSubTab; OnInit 0x16)
	PADDING(0x534, 0x53c);
	// The KR m_itemDescPos: item_desc's left and bottom, where
	// OnItemDescBgOwnerDraw puts the bottom pictures (+6 / +262, +320).
	WPoint m_itemDescPos;                // 0x53c
	float m_itemDescLeft;                // 0x544 (item_desc's left)
	float m_itemDescBottom;              // 0x548 (its top + height)
	float m_itemHideDescLeft2;           // 0x54c (item_hide_desc again)
	float m_itemHideDescBottom2;         // 0x550
	int m_furnitureTabIndex;             // 0x554 (ftab_main's selection)
	PADDING(0x558, 0x584);
	// 0x584: the character preview the +0x588 another-hand job builds
	// (0x605640; +0x55c its character id; deleted through its vtable [0]
	// by 0x605530); while there is one SetMainTab leaves the sub tab alone.
	void* m_preview;                     // 0x584
	PADDING(0x588, 0x710);
	// A copy of CSharedDoc's owned characters (OnInit; the JP doc +0x3704),
	// compared by Net_EquipDresserParts.
	std::map<unsigned int, sCharacterInfo> m_characterMap; // 0x710
	PADDING(0x71c, 0x720);
	// 0x720-0x794: the edited equipment, the 0x74 bytes OnInit copies from
	// the JP doc +0xa26 (the KR CSharedDoc::m_myEquip block: m_myCaddieId
	// ...), compared and sent by the Net_Equip* senders.
	uint32_t m_caddieId;                 // 0x720
	uint32_t m_characterId;              // 0x724
	uint32_t m_clubId;                   // 0x728
	uint32_t m_ballId;                   // 0x72c
	uint32_t m_equipItem[10];            // 0x730
	UNKNOWN(0x758, 0x770);
	uint32_t m_equipSkin[6];             // 0x770
	uint32_t m_mascotId;                 // 0x788
	UNKNOWN(0x78c, 0x794);
	// The item list's button areas (KR m_itemListArea[14] at 0x9e0; the JP
	// adds [14] "ucc_exhibit_btn" and [15] "state_view", and [10] is
	// "poster_btn"; LoadMyRoomImages).
	sItemListArea m_itemListArea[16];    // 0x794
	PADDING(0x954, 0xa12);
	// SetSubTab rebuilds the current sub tab once when set (cleared there).
	bool m_subTabDirty;                  // 0xa12
	PADDING(0xa13, 0xa14);
	WRect m_uccDrawRects[2];             // 0xa14 (KR 0xc28) sized after "ucc_draw_n" at (103, 66) and (144, 66)
	PADDING(0xa34, 0xa3c);
	// SetSubTab's case 0 keeps the preview when it is 1 and nothing forces
	// a reload (the KR ChangeSubTab's m_uiState test).
	int m_uiState;                       // 0xa3c
	PADDING(0xa40, 0xa78);
	JpCardBook* m_cardBook;              // 0xa78 (new in the constructor)
	PADDING(0xa7c, 0xa84);
	// SelectMenu case 9 sends the main actor 0x2af instead of leaving when
	// set (the KR case 11 tests IsItemStorageVisible at the same place).
	bool m_itemStorageOpen; // 0xa84
	PADDING(0xa85, 0xa90);
	FrArea* m_itemStorageBg;             // 0xa90 "item_storage_bgi"
	FrArea* m_itemStoragePang;           // 0xa94 "item_storage_pang"
	FrButton* m_pangMinus;               // 0xa98 "pang_minus"
	FrButton* m_pangPlus;                // 0xa9c "pang_plus"
	FrListBox* m_itemStorageList;        // 0xaa0 "item_storage_list"
	FrListBox* m_itemStorageIndex;       // 0xaa4 "item_storage_index"
	FrButton* m_itemStorageNext;         // 0xaa8 "item_storage_page_next"
	FrButton* m_itemStoragePrev;         // 0xaac "item_storage_page_prev"
	FrButton* m_itemStorageClose;        // 0xab0 "item_storage_close"
	FrArea* m_itemStorageDragDrop;       // 0xab4 "item_storage_dragdrop"
	FrListBox* m_cardPackList;           // 0xab8 "cardpacklist"
	PADDING(0xabc, 0xae0);
};
// size 0xae0 (x86); the field checks are off in the 64-bit port (defines.h)
CHECK_SIZE(JpRealMyRoomMain, 0xae0);
BEGIN_LAYOUT_CHECK(JpRealMyRoomMain)
	CHECK_FIELD(JpRealMyRoomMain, m_middleUi, 0x128);
	CHECK_FIELD(JpRealMyRoomMain, m_mainTab, 0x134);
	CHECK_FIELD(JpRealMyRoomMain, m_subTab, 0x138);
	CHECK_FIELD(JpRealMyRoomMain, m_itemList, 0x144);
	CHECK_FIELD(JpRealMyRoomMain, m_tabText, 0x148);
	CHECK_FIELD(JpRealMyRoomMain, m_statGroup, 0x158);
	CHECK_FIELD(JpRealMyRoomMain, m_statBar, 0x15c);
	CHECK_FIELD(JpRealMyRoomMain, m_itemDescBg, 0x170);
	CHECK_FIELD(JpRealMyRoomMain, m_itemDesc, 0x174);
	CHECK_FIELD(JpRealMyRoomMain, m_itemHideDescBg, 0x178);
	CHECK_FIELD(JpRealMyRoomMain, m_itemHideDesc, 0x17c);
	CHECK_FIELD(JpRealMyRoomMain, m_itemHideName, 0x180);
	CHECK_FIELD(JpRealMyRoomMain, m_skinPet, 0x184);
	CHECK_FIELD(JpRealMyRoomMain, m_pet, 0x18c);
	CHECK_FIELD(JpRealMyRoomMain, m_itemSlot, 0x190);
	CHECK_FIELD(JpRealMyRoomMain, m_slotBox, 0x194);
	CHECK_FIELD(JpRealMyRoomMain, m_upDownGroup, 0x198);
	CHECK_FIELD(JpRealMyRoomMain, m_equipCancel, 0x19c);
	CHECK_FIELD(JpRealMyRoomMain, m_equipReset, 0x1a0);
	CHECK_FIELD(JpRealMyRoomMain, m_upgradeButton, 0x1a4);
	CHECK_FIELD(JpRealMyRoomMain, m_downgradeButton, 0x1a8);
	CHECK_FIELD(JpRealMyRoomMain, m_clubWorkButton, 0x1b0);
	CHECK_FIELD(JpRealMyRoomMain, m_masteryButton, 0x1b4);
	CHECK_FIELD(JpRealMyRoomMain, m_giftBg, 0x1b8);
	CHECK_FIELD(JpRealMyRoomMain, m_giftDesc, 0x1bc);
	CHECK_FIELD(JpRealMyRoomMain, m_statText, 0x1c0);
	CHECK_FIELD(JpRealMyRoomMain, m_furnitureTab, 0x1c4);
	CHECK_FIELD(JpRealMyRoomMain, m_furnitureList, 0x1c8);
	CHECK_FIELD(JpRealMyRoomMain, m_mainBg, 0x1dc);
	CHECK_FIELD(JpRealMyRoomMain, m_chatBg, 0x1ec);
	CHECK_FIELD(JpRealMyRoomMain, m_itemSearchButton, 0x248);
	CHECK_FIELD(JpRealMyRoomMain, m_pSelectItemMark, 0x24c);
	CHECK_FIELD(JpRealMyRoomMain, m_cardPackItemBmp, 0x254);
	CHECK_FIELD(JpRealMyRoomMain, m_pStatPointBitmap, 0x270);
	CHECK_FIELD(JpRealMyRoomMain, m_selectIcons, 0x2e0);
	CHECK_FIELD(JpRealMyRoomMain, m_pSelectDivide, 0x31c);
	CHECK_FIELD(JpRealMyRoomMain, m_pItemSlotBitmap, 0x330);
	CHECK_FIELD(JpRealMyRoomMain, m_pItemDescBottom, 0x340);
	CHECK_FIELD(JpRealMyRoomMain, m_pItemDescFrame, 0x358);
	CHECK_FIELD(JpRealMyRoomMain, m_pUccStateBitmap, 0x37c);
	CHECK_FIELD(JpRealMyRoomMain, m_pFurnitureItemBg, 0x3ec);
	CHECK_FIELD(JpRealMyRoomMain, m_pMarkBegin, 0x3fc);
	CHECK_FIELD(JpRealMyRoomMain, m_itemHideDescLeft, 0x448);
	CHECK_FIELD(JpRealMyRoomMain, m_pCharLevels, 0x48c);
	CHECK_FIELD(JpRealMyRoomMain, m_pCharParts, 0x490);
	CHECK_FIELD(JpRealMyRoomMain, m_charPartIds, 0x494);
	CHECK_FIELD(JpRealMyRoomMain, m_pCharCards, 0x4f4);
	CHECK_FIELD(JpRealMyRoomMain, m_charTypeId, 0x4f8);
	CHECK_FIELD(JpRealMyRoomMain, m_charSerial, 0x4fc);
	CHECK_FIELD(JpRealMyRoomMain, m_mainTabIndex, 0x52c);
	CHECK_FIELD(JpRealMyRoomMain, m_subTabIndex, 0x530);
	CHECK_FIELD(JpRealMyRoomMain, m_itemDescPos, 0x53c);
	CHECK_FIELD(JpRealMyRoomMain, m_itemDescLeft, 0x544);
	CHECK_FIELD(JpRealMyRoomMain, m_furnitureTabIndex, 0x554);
	CHECK_FIELD(JpRealMyRoomMain, m_preview, 0x584);
	CHECK_FIELD(JpRealMyRoomMain, m_caddieId, 0x720);
	CHECK_FIELD(JpRealMyRoomMain, m_equipItem, 0x730);
	CHECK_FIELD(JpRealMyRoomMain, m_equipSkin, 0x770);
	CHECK_FIELD(JpRealMyRoomMain, m_mascotId, 0x788);
	CHECK_FIELD(JpRealMyRoomMain, m_itemListArea, 0x794);
	CHECK_FIELD(JpRealMyRoomMain, m_subTabDirty, 0xa12);
	CHECK_FIELD(JpRealMyRoomMain, m_uccDrawRects, 0xa14);
	CHECK_FIELD(JpRealMyRoomMain, m_uiState, 0xa3c);
	CHECK_FIELD(JpRealMyRoomMain, m_cardBook, 0xa78);
	CHECK_FIELD(JpRealMyRoomMain, m_itemStorageOpen, 0xa84);
	CHECK_FIELD(JpRealMyRoomMain, m_itemStorageBg, 0xa90);
	CHECK_FIELD(JpRealMyRoomMain, m_cardPackList, 0xab8);
END_LAYOUT_CHECK

// Keeps JpRealMyRoomMain.o (and its "realmyroom" registrar) linked (G11):
// JpMyRoomWidget.cpp references it.
FrWnd* JpRealMyRoomMain_Create(const char* name, FrWndManager* pManager);

#endif
