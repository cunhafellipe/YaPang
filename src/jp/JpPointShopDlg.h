// JP-only game code (no KR counterpart): the Tiki point shop.
//
// FrPointShopDlg, layout "PointShopDlg": the point shop window with three
// pages (intro, exchange, shop; the page number at JP +0x1b8, 3 at OnInit),
// the player's points ("MyPoint", "PrePoint"), the exchange lists (owned
// items "Own_item" / items to exchange "Exchange_item", six row backgrounds
// each) and the shop list ("Shop_item"). CTaskMain 0x659360 opens it (the
// JP names.tsv calls that VA SendRoomListExitPacket, KR's name for the slot):
// it sends packet 0x127, switches the current task's page (task vtable +0x30,
// page 2), creates it through CreatePointShopDlg(manager, "PointShopDlg"),
// opens it modal (form vtable +0x64, result callback 0x651260, flags 0x11),
// centres it on CTaskMain +0x78 (form vtable +0x58), runs 0x656a80 and clears
// the JP CSharedDoc byte +0x10b25. The exchange confirmation (0x84dac0 ->
// CPointShop::Send 0x851e90 on FrPointShopDlg +0x1a0) sends packet 0x128 (mode 1: per entry
// typeId, itemId, count and GetExchangePoint(typeId)) or 0x129 (mode 2: per
// entry typeId, count and the goods' price, JpItemManager_FindGoods).
//
// FrPointShopMessageDlg: the shop's result popup (a background bitmap
// "tikipoint_popup_bg" on success or "tikipoint_popup_fail", and a text).
// Opened by CreatePointShopMessageDlg (0x655680), called from 0x65f2be in
// the CTaskMain lobby code (function start not identified; Binary Ninja
// shows the piece at 0x65f1a0 as CTaskMain::OnUnderBar_ShopInit's tail).
//
// JP x86 facts: base the JP FrForm (ctor 0xbe5e20, dtor 0xbe5e80, 38 slots,
// size 0x110 like KR's). The port keeps the KR FrForm layout, so the JP
// offsets are comments only (no CHECK_FIELD), like JpRoomListSelector.h.
#ifndef PG_JP_JPPOINTSHOPDLG_H
#define PG_JP_JPPOINTSHOPDLG_H

#include "defines.h"
#include "JpItemGlue.h"
#include "frcmdtarget.h"
#include "frform.h"
#include "wtypes.h"

#include <list>
#include <map>
#include <string>

class Bitmap;
class CWindFont;
class FrArea;
class FrButton;
class FrListBox;
struct FrListItem;
class FrViewer;
class FrWnd;
class FrWndManager;

// RTTI 0xe22340 ".?AVCPointShop@@", vtable 0xd318e0 (1 slot: the deleting
// destructor 0x8526d0). The point shop's selection, embedded in
// FrPointShopDlg at JP +0x1a0 and handed to FrPointShopNoticeDlg: the mode
// (1 exchange, 2 shop, 3 intro: FrPointShopDlg's page) and the selected
// entries by type id. Size 0x14 (x86).
class CPointShop {
public:
	// A selected entry (the map's value; names ours).
	struct Entry {
		uint32_t itemId;                    // +0x0 (the owned item's id, exchange mode)
		uint32_t typeId;                    // +0x4
		uint32_t count;                     // +0x8
	};
	typedef std::map<uint32_t, Entry> EntryMap;

	CPointShop(void);                                                 // 0x852650
	virtual ~CPointShop(void);                                        // [0] 0x8526d0 (deleting), 0x8524d0

	// Clears the selection on a mode change: 0 when the mode is the same,
	// 1 when changed (-1 is unreachable).
	int SetMode(int mode);                                            // 0x852080
	int GetMode(void) const { return m_mode; }
	void Clear(void);                                                 // 0x8520c0
	// A copy of the selection.
	EntryMap GetEntries(void) const;                                  // 0x84fba0
	// The selection's price: per entry the item's point value (exchange)
	// or the goods' price (shop) times the count; 0 on the intro page.
	int GetTotal(void);                                               // 0x8520d0
	// The Tiki point value of an item by its type id's kind (0 when the
	// kind has none or the record is missing).
	int GetExchangePoint(uint32_t typeId);                            // 0x851810
	// How many of a type id the selection may hold: 1 for a part or a
	// club set that exists, the IFF record's maximum count for a ball, an
	// item or a card, 0 otherwise (the record missing or another kind).
	int GetMaxCount(uint32_t typeId);                                 // 0x8518e0
	// The selection's key of an entry: the owned item's id (exchange) or
	// the type id (shop); 0 on the intro page.
	uint32_t GetKey(const Entry& entry) const;                        // 0x8515c0
	// The selected entry with the same key as `entry`, NULL when none (or
	// on the intro page).
	Entry* Find(const Entry& entry);                                  // 0x852450
	// Selects one more of `entry`: an entry already selected counts one
	// more (not a part nor a club set: NULL); a new one is stored with
	// one more than its count while at most 4 are selected (5 entries at
	// most). Returns the stored entry, NULL when nothing was added (also
	// on the intro page or with a zero key).
	Entry* Add(const Entry& entry);                                   // 0x852700
	// Selects one less of `entry` (the entry is removed at 0); returns
	// the count left, 0 when not selected (or on the intro page).
	int Remove(const Entry& entry);                                   // 0x8523e0
	// Sends the selection: packet 0x128 (exchange: the entry count byte,
	// per entry typeId, itemId, count and GetExchangePoint(typeId)) or
	// 0x129 (shop: the entry count byte, per entry with goods typeId,
	// count and the goods' price).
	void Send(void);                                                  // 0x851e90

private:
	int m_mode;                             // +0x4 (not set by the constructor)
	EntryMap m_entries;                     // +0x8 (VS 2010 map: head +0xc, size +0x10)
};

// RTTI 0xe7c6e8 "FrPointShopDlg" (registered also as "PointShopDlg"),
// vtables 0xd3171c (38 slots: [0] GetRTTI, [1] the deleting destructor,
// [9] OnProc, [32] OnInit; the rest FrForm's), 0xd31710 (FrCmdTarget: [0]
// thunk 0x850a10, [1] GetMessageMap 0x84ba80), 0xd31708 ([0] thunk
// 0x850a20). Message map 0xe21bf0 (rows 0xe21d58, 42 rows + terminator,
// handlers stored by the static init 0xcde0b0, base 0xe4343c). Size 0x1c0.
class FrPointShopDlg : public FrForm {
public:
	static const WRTTI m_RTTI;                                        // 0xe7c6e8
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x850a00

	FrPointShopDlg(void);                                             // 0x850850
	virtual ~FrPointShopDlg(void);                                    // [1] 0x850cd0 (deleting), 0x850a30

	// With "Exchange_ok" made and the exchange page up (page 1), enables
	// the button when the selection is not empty.
	virtual void OnProc(const float dt);                              // [9] 0x850200
	virtual bool OnInit(void);                                        // [32] 0x84ba90

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe21d58

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe21bf0
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0x84ba80

	// Message-map handlers (map row in brackets).
	void OnMyPointInit(PG_MSGARG param);                              // [0] 0x84c530
	void OnMyPointOwnerDraw(void);                                    // [1] 0x84dbc0
	void OnExitInit(PG_MSGARG param);                                 // [2] 0x84c6f0
	// [3] PointShop_Exit LBUTTONUP: FrForm::OnFreshOkay (0x84bb20, a jump).
	void OnMainInit(PG_MSGARG param);                                 // [4] 0x84c5a0
	void OnMainClick(void);                                           // [5] 0x851570 SetPage(3)
	void OnHelpInit(PG_MSGARG param);                                 // [6] 0x84c650
	// When the help button is visible, creates "PointShopHelp" and opens it.
	void OnHelpClick(void);                                           // [7] 0x84dc30
	void OnGotoMainBgInit(PG_MSGARG param);                           // [8] 0x84c790
	void OnIntroBg1Init(PG_MSGARG param);                             // [9] 0x84c810
	void OnIntroBg2Init(PG_MSGARG param);                             // [10] 0x84c890
	void OnIntroNoticeInit(PG_MSGARG param);                          // [11] 0x84c910
	// While the notice viewer is visible, opens "pointshop_notice.jpg".
	void OnIntroNoticeOwnerDraw(void);                                // [12] 0x84c9b0
	void OnExchangeInit(PG_MSGARG param);                             // [13] 0x84c9d0
	void OnExchangeClick(void);                                       // [14] 0x851580 SetPage(1)
	void OnBuyInit(PG_MSGARG param);                                  // [15] 0x84ca80
	void OnBuyClick(void);                                            // [16] 0x851590 SetPage(2)
	void OnExchangeListBg1Init(PG_MSGARG param);                      // [17] 0x84cb20
	void OnExchangeListBg2Init(PG_MSGARG param);                      // [18] 0x84cba0
	void OnExchangeListBg3Init(PG_MSGARG param);                      // [19] 0x84cc20
	void OnExchangeListBg4Init(PG_MSGARG param);                      // [20] 0x84cca0
	void OnExchangeListBg5Init(PG_MSGARG param);                      // [21] 0x84cd20
	void OnExchangeListBg6Init(PG_MSGARG param);                      // [22] 0x84cda0
	void OnPrePointInit(PG_MSGARG param);                             // [23] 0x84ce20
	void OnPrePointOwnerDraw(void);                                   // [24] 0x84cea0
	void OnExchangeOkInit(PG_MSGARG param);                           // [25] 0x84cf20
	// When the selection is not empty, opens "PointShopNoticeDlg" on it;
	// its Apply (0x84dac0) sends it (CPointShop::Send, packets 0x128 /
	// 0x129).
	void OnExchangeOkClick(void);                                     // [26] 0x8502b0
	void OnOwnItemInit(PG_MSGARG param);                              // [27] 0x84cfa0
	void OnOwnItemOwnerDraw(PG_MSGARG param);                         // [28] 0x84f700
	// Moves one of the selected owned row (its first owned item) to the
	// selection: the row counts the maximum count less, leaves "Own_item"
	// when fewer are left, and the entry shows in "Exchange_item".
	void OnOwnItemClick(void);                                        // [29] 0x84e620
	void OnExchangeItemInit(PG_MSGARG param);                         // [30] 0x84d030
	void OnExchangeItemOwnerDraw(PG_MSGARG param);                    // [31] 0x84f7c0
	// The reverse: one less of the selected exchange entry (the row leaves
	// "Exchange_item" at 0), its owned row counts the maximum count more
	// and shows in "Own_item".
	void OnExchangeItemClick(void);                                   // [32] 0x84e6e0
	void OnShopListBg1Init(PG_MSGARG param);                          // [33] 0x84d0b0
	void OnShopListBg2Init(PG_MSGARG param);                          // [34] 0x84d130
	void OnShopListBg3Init(PG_MSGARG param);                          // [35] 0x84d1b0
	void OnShopListBg4Init(PG_MSGARG param);                          // [36] 0x84d230
	void OnShopListBg5Init(PG_MSGARG param);                          // [37] 0x84d2b0
	void OnShopListBg6Init(PG_MSGARG param);                          // [38] 0x84d330
	void OnShopItemInit(PG_MSGARG param);                             // [39] 0x84d3b0
	void OnShopItemOwnerDraw(PG_MSGARG param);                        // [40] 0x84f8d0
	// Selects the clicked goods alone (count 0) and opens
	// "PointShopNoticeDlg" on the selection.
	void OnShopItemClick(void);                                       // [41] 0x8503a0

	// Draws `bmp` (when the GDI and the bitmap exist) over `rect`, at the
	// bitmap's size when `bitmapSize`, with the text style 0 first.
	void DrawBitmap(const Bitmap* bmp, WRect rect, uint32_t color, bool bitmapSize); // 0x84c480
	// Draws a list row: a part's character base image, the item's icon,
	// its name (cut to `column` - 1 characters and "..." when longer; with
	// " (n)" the exchange's maximum count on page 1), then "x<count>"
	// (page 1 `count` / the maximum count, page 2 the goods' count), the
	// point value (page 2 the goods' price) and "TP" at 11 and 13 times
	// `column`. `height` is the row's height (the icons are scaled to it),
	// `column` 0x10 on the exchange lists and 0x1e on the shop list.
	void DrawItemRow(FrListItem* item, float height, int column, uint32_t count); // 0x84eda0

	// Shows page `page`: 1 exchange, 2 shop, 3 main/intro.
	void SetPage(int page);                                           // 0x851310
	// A row of the page's item list (the +0x110 list's element, 0x14
	// bytes; names ours): a type id, the count summed over the adds
	// (0x850da0), and per owned item id (non-zero) the count added with
	// it.
	struct ListedItem {
		uint32_t typeId;                    // +0x0 (the finder's key)
		uint32_t count;                     // +0x4 (compared unsigned)
		std::map<uint32_t, int32_t> itemCounts; // +0x8 (VS 2010 map: head +0xc, size +0x10)
	};

	// The listed row of a type id, NULL when none.
	ListedItem* FindListedItem(uint32_t typeId);                      // 0x84e320
	// Whether an owned record can be exchanged: its IFF record (by the
	// type id's kind: part, club set, ball, item or card) has a point
	// value; a part, club set, ball or item is not worn (KR
	// CSharedDoc::IsEquipParts) nor carrying a period card (the JP
	// CSharedDoc +0x37ec list = KR m_cardPeriodList[0]),
	// and holds at least the maximum count (an item less its equipped
	// copies; a ball not the equipped one); a card is not in use (its
	// +0x8 and +0xc zero) and holds at least the maximum count.
	// `record` is the sItemInfo (part, club set, ball, item) or the
	// sCards (card) of the type id.
	bool IsExchangeable(uint32_t typeId, const void* record);         // 0x84e350
	// Adds `count` to the row of `typeId` (a new row at the front or the
	// back of the list when none) and, with a non-zero item id, records
	// the count under it; returns the row's count.
	uint32_t AddListedItem(uint32_t typeId, uint32_t itemId, uint32_t count, bool atFront); // 0x850da0
	// Empties and rebuilds the +0x110 item list for the current page:
	// page 1 the exchangeable owned records (JP CSharedDoc +0x3734 parts,
	// +0x3750 gear, +0x375c items less the equipped ones (+0xa36..),
	// +0x3744 the item map, +0x37e0 cards), page 2 the active goods.
	void BuildItemList(void);                                         // 0x850f10
	// Adds every listed row to the page's list box (page 1 "Own_item",
	// page 2 "Shop_item"; the row is the list item's data).
	void FillItemList(void);                                          // 0x84e5d0

private:
	std::list<ListedItem> m_listedItems;    // JP +0x110 (head +0x110, size +0x114, 0x20-byte nodes)
	const Bitmap* m_pPointGetListMyBmp;     // JP +0x11c "tikipoint_btn_pointget_list_my_o"
	const Bitmap* m_pPointGetListUseBmp;    // JP +0x120 "tikipoint_btn_pointget_list_use_o"
	const Bitmap* m_pPointUseListNBmp;      // JP +0x124 "tikipoint_pointuse_list_n"
	const Bitmap* m_pPointUseListOBmp;      // JP +0x128 "tikipoint_pointuse_list_o"
	const Bitmap* m_pBuyBmp;                // JP +0x12c "tikipoint_buy"
	FrArea* m_pGotoMainBg;                  // JP +0x130 "GotoMainBg"
	FrArea* m_pShopListBg[6];               // JP +0x134 "Shop_ListBG1".."Shop_ListBG6"
	FrArea* m_pExchangeListBg[6];           // JP +0x14c "Exchange_ListBG1".."Exchange_ListBG6"
	FrArea* m_pIntroBg1;                    // JP +0x164 "Intro_bg1"
	FrArea* m_pIntroBg2;                    // JP +0x168 "Intro_bg2"
	FrArea* m_pMyPoint;                     // JP +0x16c "MyPoint"
	FrArea* m_pPrePoint;                    // JP +0x170 "PrePoint"
	FrViewer* m_pIntroNotice;               // JP +0x174 "Intro_notice"
	FrButton* m_pMainButton;                // JP +0x178 "PointShop_Main"
	FrButton* m_pHelpButton;                // JP +0x17c "PointShop_Help"
	// JP +0x180: not set by the constructor nor the map's handlers.
	FrButton* m_pExitButton;                // JP +0x184 "PointShop_Exit"
	FrButton* m_pExchangeButton;            // JP +0x188 "Exchange"
	FrButton* m_pBuyButton;                 // JP +0x18c "Buy"
	FrButton* m_pExchangeOkButton;          // JP +0x190 "Exchange_ok"
	FrListBox* m_pOwnItemList;              // JP +0x194 "Own_item"
	FrListBox* m_pExchangeItemList;         // JP +0x198 "Exchange_item"
	FrListBox* m_pShopItemList;             // JP +0x19c "Shop_item"
	CPointShop m_pointShop;                 // JP +0x1a0 .. +0x1b4
	// JP +0x1b4: not touched by the class's functions.
	int m_page;                             // JP +0x1b8 (3 at OnInit)
	CWindFont* m_pWindFont;                 // JP +0x1bc (ctor 0x6f4640, RTTI CWindFont)
};

// 0x654630: CreateForm<FrPointShopDlg>(manager, owner, name, parent),
// which also stores the form in the JP thread-local slot (+0xc0) and casts
// it to FrPointShopDlg.
FrForm* CreatePointShopDlg(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent);

// RTTI 0xe7c6d8 "FrPointShopMessageDlg", vtables 0xd31594 (38 slots: [0]
// GetRTTI, [1] the deleting destructor, [8] OnDraw; the rest FrForm's),
// 0xd31588 (FrCmdTarget: [0] thunk 0x84dee0, [1] GetMessageMap 0x84b940),
// 0xd31580 ([0] thunk 0x84def0). Message map 0xe21be0 (rows 0xe21c38,
// handler stored by the static init 0xcddf00, base 0xe4343c). Size 0x140.
class FrPointShopMessageDlg : public FrForm {
public:
	static const WRTTI m_RTTI;                                        // 0xe7c6d8
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x84ded0

	FrPointShopMessageDlg(void);                                      // 0x84e820
	virtual ~FrPointShopMessageDlg(void);                             // [1] 0x84e2f0 (deleting), 0x84de70

	// With a GDI: the popup bitmap (success or fail) over the form's rect
	// (the Fresh DrawTexture 0xbec640, colour -1), then the text in black
	// then red (0xbec440 0xff000000 / 0xffff0000, 0xbec460(2)) at the rect's
	// x + 127, y + 116 (0xbed280, KR FrGraphicInterface::Print).
	virtual void OnDraw(void);                                        // [8] 0x84d9d0

	// m_success (which bitmap OnDraw shows).
	void SetSuccess(int success);                                     // 0x84b950
	// m_message = text (a NULL text leaves it).
	void SetMessage(const char* text);                                // 0x84e8d0

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe21c38

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe21be0
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0x84b940

	void OnBackgroundInit(PG_MSGARG param);                           // 0x84c130

private:
	// Which bitmap OnDraw shows: non-zero "tikipoint_popup_bg", zero
	// "tikipoint_popup_fail" (1 from the constructor).
	int m_success;                          // JP +0x110
	std::string m_message;                  // JP +0x114 (the text OnDraw prints)
	const Bitmap* m_pSuccessBg;             // JP +0x130 "tikipoint_popup_bg"
	const Bitmap* m_pFailBg;                // JP +0x134 "tikipoint_popup_fail"
	FrArea* m_pBackground;                  // JP +0x138 "background"
	// JP +0x13c .. +0x140: not touched by the class's functions.
};

// 0x655680: CreateForm<FrPointShopMessageDlg>(manager, owner, name,
// parent), with the thread-local slot (+0xc0) store and the cast.
FrForm* CreatePointShopMessageDlg(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent);

class CTaskMain;

// The JP CTaskMain::HandleMsgCommon case 0x2e1 (0x65f2a3; the point shop
// result the packet readers post, text in param1, success flag in param2):
// "PointShopMessageDlg" (0x655680, owner the task), SetMessage, SetSuccess,
// Open with no result callback and flag 3 (JP FrForm vtable +0x64), then
// FrWnd::SetTopmost(true) (0xbe8500).
void TaskMain_OpenPointShopMessageDlg(CTaskMain* task, const char* text, int success);

class FrEdit;

// RTTI 0xe7c6cc "FrPointShopHelp" (layout "PointShopHelp"): the point
// shop's help page, a viewer showing "pointshop_explanation.jpg". Vtables
// 0xd312cc (38 slots: [0] GetRTTI, [1] the deleting destructor; the rest
// FrForm's), 0xd312c0 (FrCmdTarget: [0] thunk 0x84bf80, [1] GetMessageMap
// 0x84b8d0), 0xd312b8. Message map 0xe21bd8 (rows 0xe21bf8, handler stored
// by the static init 0xcdde70, base 0xe4343c). Size 0x120.
class FrPointShopHelp : public FrForm {
public:
	static const WRTTI m_RTTI;                                        // 0xe7c6cc
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x84b910

	FrPointShopHelp(void);                                            // 0x84b8e0
	virtual ~FrPointShopHelp(void);                                   // [1] 0x84c070 (deleting), 0x84b920

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe21bf8

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe21bd8
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0x84b8d0

	void OnDescViewInit(PG_MSGARG param);                             // 0x84c0a0

private:
	FrViewer* m_pDescView;                  // JP +0x110 "desc_view"
	// JP +0x114 .. +0x120: not touched by the class's functions.
};

// 0x84d790: CreateForm<FrPointShopHelp>(manager, owner, name, parent),
// with the thread-local slot (+0xc0) store and the cast.
FrForm* CreatePointShopHelp(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent);

// RTTI 0xe7c6e0 "FrPointShopNoticeDlg" (layout "PointShopNoticeDlg"): the
// confirmation of an exchange or a purchase, opened by
// FrPointShopDlg::OnExchangeOkClick and OnShopItemClick (0x84d860 then
// SetExchange 0x8500a0). Vtables 0xd313c4 (38 slots: [0] GetRTTI, [1] the
// deleting destructor, [8] OnDraw; the rest FrForm's), 0xd313b4
// (FrCmdTarget: [0] thunk 0x84bfa0, [1] GetMessageMap 0x84b960), 0xd313ac.
// Message map 0xe21be8 (rows 0xe21c78, handlers stored by the static init
// 0xcddf90, base 0xe4343c). Size 0x130.
class FrPointShopNoticeDlg : public FrForm {
public:
	static const WRTTI m_RTTI;                                        // 0xe7c6e0
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x84ba20

	FrPointShopNoticeDlg(void);                                       // 0x84b970
	virtual ~FrPointShopNoticeDlg(void);                              // [1] 0x84c230 (deleting), 0x84ba30

	// With a GDI: "tikipoint_popup_bg" over the form's rect when the
	// background area is set, then per the exchange's mode the selected
	// entries (mode 2 each with its item icon from CSharedDoc +0x20c and
	// "%s(%d) : %dTP"; mode 1 "%s(%d) x %d" with the maximum count), the
	// total (0x8520d0) in a confirmation text and, in mode 1, the "cannot
	// be restored" note, in the Fresh text colours 0xffa34706 / 0xff8a19a5.
	virtual void OnDraw(void);                                        // [8] 0x84fbd0

	// Stores the exchange (+0x128); in mode 2 also fills the "desc" edit
	// with each selected goods' description (CItemManager::FindDesc on the
	// JP CSharedDoc +0x20c, text +0x4: ClearLine, the JP 5-argument
	// FrEdit::AddText(text, 0, 1, 0, 0), then the edit's +0x90 scroll bar
	// ScrollToFirst; the KR 3-argument AddText in the port).
	void SetExchange(CPointShop* exchange);                           // 0x8500a0

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe21c78

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe21be8
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0x84b960

	void OnBackgroundInit(PG_MSGARG param);                           // [0] 0x84c260
	void OnTagInit(PG_MSGARG param);                                  // [1] 0x84c2d0
	// With the tag area and the exchange set: "tikipoint_popup_tec1"
	// (mode 1) or "tikipoint_popup_tec2" (mode 2) at the tag's position,
	// at the bitmap's size.
	void OnTagOwnerDraw(void);                                        // [2] 0x84c340
	void OnDescInit(PG_MSGARG param);                                 // [3] 0x84c410
	// Mode 2 with a total (0x8520d0) above the player's points (JP
	// CSharedDoc +0x3860): closes and posts the "not enough points" notice
	// (string 0xd31548, message 0x2e1) through 0x9b79c0's vtable +0x30;
	// otherwise sends the exchange (CPointShop::Send), posts message 1 the
	// same way (the JP 0x20-byte message is KR MsgObject, 0x9b79c0 KR
	// AfxGetTask, vtable +0x30 SendMsgToMainActor; the text is
	// Jp_LookupText 0xb4eb80 of the Korean string 0xd31548), and closes
	// (also without an exchange).
	void OnApplyClick(void);                                          // [4] 0x84dac0
	void OnExitClick(void);                                           // [5] 0x84ba70

private:
	const Bitmap* m_pTec1Bmp;               // JP +0x110 "tikipoint_popup_tec1"
	const Bitmap* m_pTec2Bmp;               // JP +0x114 "tikipoint_popup_tec2"
	const Bitmap* m_pBgBmp;                 // JP +0x118 "tikipoint_popup_bg"
	FrArea* m_pBackground;                  // JP +0x11c "background"
	FrArea* m_pTag;                         // JP +0x120 "tag"
	FrEdit* m_pDesc;                        // JP +0x124 "desc"
	CPointShop* m_pExchange;                // JP +0x128 (SetExchange)
	// JP +0x12c: not touched by the class's functions.
};

// 0x84d860: CreateForm<FrPointShopNoticeDlg>(manager, owner, name,
// parent), with the thread-local slot (+0xc0) store and the cast.
FrForm* CreatePointShopNoticeDlg(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent);

#endif // PG_JP_JPPOINTSHOPDLG_H
