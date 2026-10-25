// JP-only game code: the JP CShopMain, the lobby shop's Fresh form
// "shopmain" (an FrTaskForm, JpFrForms.h), which CShopWidget
// (JpShopWidget.h) loads through CTaskWidget::Load. The class is named
// JpShopMain here: the KR CShopMain (yapang/src/client/ProjectG/ShopMain.h,
// a CTaskMain + CHandOwner task of 0x740) is compiled into the port too.
//
// What the JP class is (node scripts/disasm.ts --jp, jp_xrefs.ts --data):
// the KR CShopMain's code moved from a task onto a form. Its member
// functions are the KR ones (jp/pairing.tsv: 46 paired with
// client/ProjectG/ShopMain, 177 more in that unit's run 0xa840b0-0xa936a0),
// but the class is not the KR one:
// - base FrTaskForm (ctor 0x523690) + CHandOwner at +0x120 (KR: CTaskMain +
//   CHandOwner), size 0x6c0 (KR 0x740): every member offset moves;
// - the KR task virtuals (OnInit/OnPreLoadInit/OnLoad/OnProcess/OnDestroy)
//   become form virtuals: [9] OnProc, [32] OnInit, [38] Start, [39]
//   Shutdown, [40] HandleMsg (FrTaskForm's 0x523620 returns false);
// - the message map is the form's own (61 rows, control names without the
//   KR "SHOPMAIN." prefix, base FrForm's map 0xe4343c): the KR rows
//   "SHOPMAIN" INIT / FINISH and the "st.*" stat box are gone (the JP has
//   "stat_group"), and the JP adds exportselectlist, main_bg1..4,
//   item_search_btn and close_wnd.
// So the KR CShopMain cannot be registered for "shopmain": this class is the
// JP one, reusing the KR member names where the JP function is paired.
//
// JP x86 facts: RTTI m_RTTI 0xf02e2c "CShopMain" (static init 0xcea450,
// base FrTaskForm 0xe74f88). Vtables 0xd45b34 (FrForm, 41 slots: the
// overrides below, the rest FrTaskForm's), 0xd45b28 (FrCmdTarget: [0]
// deleting-destructor thunk 0xa91930, [1] GetMessageMap 0xa822c0), 0xd45b20
// ([0] thunk 0xa91940), 0xd45b18 (CHandOwner at +0x120: [0] OnAnotherHand
// 0xa87db0). Message map 0xe348d8 (rows 0xe34908, stride 0x20, handlers
// stored by the static init 0xcea4c0). Registration 0xcea4a0 ("shopmain" ->
// creator 0xa92d70 = CreateForm<CShopMain> 0xa92ca0, which also stores the
// form in the thread-local +0xb280 slot and calls 0x47cc60 with the RTTI).
//
// Layout (x86, from the constructor 0xa91ec0, the destructor 0xa91640,
// InitShopControls 0xa82310 and the INIT handlers' stores): the KR members
// in the KR order, without the KR stat box ("st.*": m_selectedName,
// m_statFrame, m_statText, m_statPart, m_statAll, m_statBar, the selected
// item row) and the card pictures, plus JP-only members after m_cardTypeId.
// The 12 bytes +0x114..+0x120 are FrTaskForm's tail, not ours: CHandOwner is
// the second base, so it follows the whole first base, and the JP
// FrTaskForm is 0x120 bytes (0x5238f0 news 0x120 for a plain FrTaskForm;
// its constructor 0x523690 writes nothing past +0x110; the JP class at
// 0x61f1f0 also has its CHandOwner at +0x120; JpFrForms.h pads it). The
// field checks are off in the 64-bit port (defines.h).
// The std containers are VS 2010's (map/list 0xc: comparator/allocator
// byte, head, size), CAnotherHand is 0x184 in the JP (KR 0x188).
//
// Status: the constructor, the destructor, OnProc, OnInit, Start, Shutdown,
// Create, the control INIT handlers, the side tabs, LoadShopImages,
// OnPetOwnerDraw, OnItemSlotOwnerDraw, ClearCartList, ChangeSkin,
// ResetItemList, RestoreShoppersOldItem, DrawCutinFrame, the cart helpers,
// the small sub-tab helpers, the item list row draw (OnItemListOwnerDraw,
// GetItemIcon, DrawItemPrice), the click (OnItemListBtnDown,
// SelectItemList, UnselectItemList, ShowItemDesc) are written
// (JpShopMain.cpp; each comment lists the parts still missing), the cart /
// checkout part in JpShopMainCart.cpp, the Build_* builders in
// JpShopMainBuild.cpp; HandleMsg is a first pass. Declared only (void, so
// their port stubs do nothing; node scripts/jp_game_progress.ts --declared):
// DrawStatus2, DrawRentalPrice, ShowMainModel (JP: SetLevelBar 0xa8b410),
// PutOnNBackupShoppersOldItem, SetPetCtrl, OnItemListRBtnDown,
// DrawSkinFrame / DrawSkinSlot, BuildMyItemSlot, BuildDisplayRental. The
// form is registered for "shopmain" (JpFrForms.cpp).
#ifndef PG_JP_JPSHOPMAIN_H
#define PG_JP_JPSHOPMAIN_H

#include <stdint.h>

#include <list>
#include <map>

#include "defines.h"
#include "padding.h"
#include "shared/classdefine.h"
#include "frcmdtarget.h"
#include "frform.h"
#include "AnotherHand.h"
#include "JpFrForms.h"
#include "wtypes.h"

class Bitmap;
class CExhibition;
class CPartTidList;
class FrArea;
class FrButton;
class FrComboBox;
class FrEdit;
class FrGraphicInterface;
struct IFF_ITEM_COMMON;
class FrGroupBox;
class FrListBox;
struct FrListItem;
class FrStatic;
class FrTabButton;
class FrTextButton;
class FrUpgradeCaddieDlg;
class WOverlay;
class WTitleFont;
class FrWnd;
class FrWndManager;
class MsgObject;

class JpShopMain : public FrTaskForm, public CHandOwner {
	LAYOUT_CHECK_FRIEND;
public:
	// The KR CShopMain's tab enums (ShopMain.h).
	enum statTab_t {
		STAT_TAB_PART = 0,
		STAT_TAB_ALL = 1
	};
	enum mainTab_t {
		MAIN_TAB_0 = 0
	};
	// The JP sub tab numbers are the KR ones + 1 from the club set on.
	enum subTab_t {
		SUB_TAB_NONE = 0,
		SUB_TAB_CLUBSET = 7,   // ChangeClubset (KR 6)
		SUB_TAB_AZTEC = 8,     // ChangeAztec (KR 7)
		SUB_TAB_CADDIE = 11,   // ChangeCaddie (KR 10)
		SUB_TAB_MASCOT = 13,   // ChangeMascot (KR 12)
		// The highest JP sub tab (KR 34; OnPetOwnerDraw's jump table runs
		// 14..35). It also gives the enum the range the JP numbers use, so
		// clang does not treat the tests of 16..35 as impossible.
		SUB_TAB_LAST = 35
	};

	static const WRTTI m_RTTI;                                        // 0xf02e2c
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0xa91920

	JpShopMain(void);                                                 // 0xa91ec0
	virtual ~JpShopMain(void);                                        // [1] 0xa92390 (deleting), 0xa91640

	// [6] 0xa84790 is not declared (the JP FrWnd slot 6, a bool argument,
	// has no KR slot pinned yet): it sets/clears flag 2 of +0x44, calls the
	// JP FrWnd's 0xbe7a00, then slots 6 and 7 of the child window at +0x15c.
	virtual void OnProc(const float dt);                              // [9] 0xa91950
	// The KR OnShopMainInit's work (0xa90650, 0xa92810, 0xa92590, 0xa8fa30
	// ... in the ShopMain run, the item manager's FindSkin 0xa174d0).
	virtual bool OnInit(void);                                        // [32] 0xa93300
	// LoadShopImages 0xa847e0, the shared doc's 0x63f3f0(-1), then the
	// "LobbyAvatar" actor (GetActor 0x9bac50) through a virtual.
	virtual bool Start(void);                                         // [38] 0xa868e0
	// FrTaskForm::Shutdown 0x523610, then the shared doc's 0x63f3f0 and
	// 0x632df0.
	virtual bool Shutdown(void);                                      // [39] 0xa86960
	// The KR CShopMain::HandleMsg (switch on the message id, 9644 bytes);
	// true when handled.
	virtual bool HandleMsg(const MsgObject& msg);                     // [40] 0xa971f0

	// Its creator, registered for "shopmain" (0xcea4a0); __fastcall in the
	// JP exe: return CreateForm<CShopMain>(pManager, NULL, name, NULL).
	static FrWnd* Create(const char* name, FrWndManager* pManager);   // 0xa92d70

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe34908

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe348d8
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0xa822c0

	// KR CShopMain::OnAnotherHand: work 0 is ShowPet(*(unsigned long*)pData).
	virtual void OnAnotherHand(int nWork, void* pData);               // 0xa87db0

	// Message-map handlers, in row order (KR names where the KR row of the
	// same control exists; the JP-only rows' names are ours).
	void OnSortOptionComboInit(intptr_t param);                       // 0xa82db0 shopview
	void OnSortOptionComboDown(void);                                 // 0xa822d0
	void OnSearchGoodsButtonInit(intptr_t param);                     // 0xa82e40 searchgoods
	void OnSearchGoodsLButtonDown(void);                              // 0xa822e0
	void OnExportSelectListInit(intptr_t param);                      // 0xa82ed0 exportselectlist (JP)
	void OnExportSelectListUp(void);                                  // 0xa822f0
	void OnMainBg1Init(intptr_t param);                               // 0xa82f40 main_bg1 (JP)
	void OnMainBg2Init(intptr_t param);                               // 0xa82fb0 main_bg2 (JP)
	void OnMainBg3Init(intptr_t param);                               // 0xa83020 main_bg3 (JP)
	void OnMainBg4Init(intptr_t param);                               // 0xa83090 main_bg4 (JP)
	void OnItemListInit(intptr_t param);                              // 0xa83950 item_list
	void OnItemListOwnerDraw(intptr_t param);                         // 0xa936a0
	void OnItemListBtnDown(void);                                     // 0xa90bf0
	void OnItemListRBtnDown(void);                                    // 0xa91340
	void OnTabMainInit(intptr_t param);                               // 0xa83790 tab_main
	void OnTabMainButtonUp(void);                                     // 0xa92e20
	void OnTabSubInit(intptr_t param);                                // 0xa838e0 tab_sub
	void OnTabSubButtonUp(void);                                      // 0xa92f30
	void OnItemSearchBtnInit(intptr_t param);                         // 0xa844e0 item_search_btn (JP)
	void OnItemSearchBtnUp(void);                                     // 0xa86740
	void OnBuyAllInit(intptr_t param);                                // 0xa83d00 buy_all
	void OnBuyAllUp(void);                                            // 0xa914c0
	void OnBuyHoverOn(intptr_t param);                                // 0xa89640 buy_all, buy_reset
	void OnBuyHoverOff(intptr_t param);                               // 0xa863d0 buy_all, buy_reset
	void OnBuyResetInit(intptr_t param);                              // 0xa83d80 buy_reset
	void OnBuyResetUp(void);                                          // 0xa92fa0
	void OnSideTabBuyInit(intptr_t param);                            // 0xa83e00 sidetab_buy
	void OnSideTabBuyUp(void);                                        // 0xa93270
	void OnSideTabRentalInit(intptr_t param);                         // 0xa83e90 sidetab_rental
	void OnSideTabRentalUp(void);                                     // 0xa915b0
	void OnCloseWndUp(void);                                          // 0xa824e0 close_wnd (JP)
	// The KR rows bind tabtxt2 twice too (OnTabTxt2Init, OnTabTxt3Init).
	void OnTabTxt2Init(intptr_t param);                               // 0xa836b0 tabtxt2
	void OnTabTxt3Init(intptr_t param);                               // 0xa83720 tabtxt2
	void OnPetInit(intptr_t param);                                   // 0xa83170 pet
	void OnPetOwnerDraw(intptr_t param);                              // 0xa90aa0
	void OnSkinPetInit(intptr_t param);                               // 0xa831e0 skin_pet
	void OnSkinPet2Init(intptr_t param);                              // 0xa83250 skin_pet2
	void OnItemDescBgInit(intptr_t param);                            // 0xa832c0 item_desc_bg
	void OnItemDescBgOwnerDraw(intptr_t param);                       // 0xa83330
	void OnItemDescInit(intptr_t param);                              // 0xa83460 item_desc
	void OnPartCartListInit(intptr_t param);                          // 0xa83f50 part_cart
	void OnPartCartListOwnerDraw(intptr_t param);                     // 0xa8aa50
	void OnAuxPartCartListInit(intptr_t param);                       // 0xa84040 auxpart_cart
	void OnAuxPartCartListOwnerDraw(intptr_t param);                  // 0xa8b040
	void OnScrollUpInit(intptr_t param);                              // 0xa84220 scroll_up
	void OnScrollUpUp(void);                                          // 0xa842a0
	void OnScrollDownInit(intptr_t param);                            // 0xa842c0 scroll_down
	void OnScrollDownUp(void);                                        // 0xa84340
	void OnItemSlotFrame1Init(intptr_t param);                        // 0xa84360 item_slot_frame1
	void OnItemSlotFrame2Init(intptr_t param);                        // 0xa843e0 item_slot_frame2
	void OnItemSlotInit(intptr_t param);                              // 0xa84460 item_slot
	void OnItemSlotOwnerDraw(intptr_t param);                         // 0xa86530
	void OnStatGroupInit(intptr_t param);                             // 0xa835d0 stat_group (JP)
	void OnStatGroupArrowInit(intptr_t param);                        // 0xa83640 stat_group.arrow_m (JP)
	void OnPowerEditInit(intptr_t param);                             // 0xa84550 stat_group.epower
	void OnControlEditInit(intptr_t param);                           // 0xa845c0 stat_group.econtrol
	void OnImpactEditInit(intptr_t param);                            // 0xa84630 stat_group.eimpact
	void OnSpinEditInit(intptr_t param);                              // 0xa846a0 stat_group.espin
	void OnCurveEditInit(intptr_t param);                             // 0xa84710 stat_group.ecurve

	// The KR member OnAnotherHand calls (paired 0xa87370).
	void ShowPet(unsigned long typeId);                               // 0xa87370
	// KR CShopMain members the JP calls (KR names, not decompiled yet).
	void SetPetCtrl(unsigned long typeId);                            // 0xa86a80
	void ChangeSkin(unsigned long typeId);                            // 0xa87a30
	bool HaveCharacter(unsigned long typeId);                         // 0xa88450
	void Checkout(void);                                              // 0xa8c570
	void ResetItemList(bool bKeepType1);                              // 0xa88540
	// The KR cart / item-list helpers (paired; the KR bodies).
	std::list<FrListItem*>::iterator DeleteItemFromCartList(unsigned long typeId);    // 0xa88a80
	std::list<FrListItem*>::iterator DeleteItemFromAuxCartList(unsigned long typeId); // 0xa88ad0
	bool UnselectItemFromItemList(unsigned long typeId);              // 0xa88b90
	// JP only (names ours): the side tabs' SetVisible(visible), each when
	// set; true on the tab 1 with the rental side tab pressed.
	void SetSideTabsVisible(bool visible);                            // 0xa824f0
	bool IsRentalSideTab(void);                                       // 0xa83f20
	// JP only (name ours): hides the scroll buttons (ChangeMainTab's tail).
	void HideScrollButtons(void);                                     // 0xa82530
	// The item list builders BuildDisplayStand calls (KR names; tab, 0).
	void Build_Package(subTab_t tab, int);                            // 0xa8df90
	void Build_Hair(subTab_t tab, int);                               // 0xa8e0b0
	void Build_Parts(subTab_t tab, int);                              // 0xa8e210
	void Build_Clubset(subTab_t tab, int);                            // 0xa8e520
	void Build_Aztec(subTab_t tab, int);                              // 0xa8e750
	void Build_Equip(subTab_t tab, int);                              // 0xa8e950
	void Build_NoEquip(subTab_t tab, int);                            // 0xa8eb60
	void Build_Card(subTab_t tab, int);                               // 0xa8ee00
	void Build_Interior(subTab_t tab, int);                           // 0xa8f010
	void Build_Furniture(subTab_t tab, int);                          // 0xa8f170
	void Build_Decorate(subTab_t tab, int);                           // 0xa8f390
	void Build_Cutin(subTab_t tab, int);                              // 0xa8f520
	void Build_Caddie(subTab_t tab, int);                             // 0xa8f6a0
	void Build_Mascot(subTab_t tab, int);                             // 0xa8f8c0
	// The KR ChangeSubTab tail (KR names, not decompiled yet).
	void SetMyItemSlot(void);                                         // 0xa88c80
	// The KR BuildMyItemSlot (the my-item slot list; not decompiled yet).
	void BuildMyItemSlot(bool);                                       // 0xa881b0
	void SetItemDescRect(void);                                       // 0xa83500
	void SetPetAreaRect(void);                                        // 0xa82800
	void SetAutoScroll(void);                                         // 0xa88d00
	// The skin tab's pet-area draws (KR names, not decompiled yet).
	void DrawSkinFrame(void);                                         // 0xa85610
	void DrawSkinSlot(void);                                          // 0xa85a00
	void DrawCutinFrame(void);                                        // 0xa909d0
	// JP only (names ours): the shop items (CSharedDoc +0x2b0 list) made
	// for the shown character, and those (+0x2a0) for the shown caddie.
	void Build_CharacterItems(subTab_t tab, int);                     // 0xa91d20
	void Build_CaddieItems(subTab_t tab, int);                        // 0xa8f790
	// The KR IsItemInCart: the list item's type id is in the cart or the aux
	// cart list.
	bool IsItemInCart(FrListItem* listItem);                          // 0xa88c00
	// JP only (names ours): the item list's level gates. An item whose level
	// byte has bit 7 clear and whose level is at least m_levelPlusOne, or
	// with bit 7 set and a level below m_level, is drawn greyed out and has
	// no buy button.
	bool IsAboveShopLevel(const IFF_ITEM_COMMON* item);               // 0xa826d0
	bool IsBelowShopLevel(const IFF_ITEM_COMMON* item);               // 0xa82700
	// The KR item list draw helpers (KR names).
	const Bitmap* GetItemIcon(unsigned long typeId);                  // 0xa90120
	void DrawItemPrice(FrGraphicInterface* graphic, float x, float y, IFF_ITEM_COMMON& item, FrListItem* listItem); // 0xa85fe0
	void DrawRentalPrice(float x, float y, IFF_ITEM_COMMON& item, FrListItem* listItem);  // 0xa86480
	void DrawStatus2(FrGraphicInterface* graphic, FrListItem* listItem, short* const status, short* const slotStatus, float x); // 0xa83ba0
	// The KR item selection (the JP previews on the "LobbyAvatar" /
	// "LobbyCaddie" actors); true when the item was put in the cart.
	bool SelectItemList(FrListItem* pItem);                           // 0xa8ca90
	// The KR TakeOff_PartNCartList / TakeOff_AuxPartNCartList (the cart
	// parts the try-on overlaps leave the carts; JP: the "LobbyAvatar" is
	// dressed through message 0x18A; JpShopMainCart.cpp).
	void TakeOff_PartNCartList(CPartTidList& tidList, unsigned long overlapMask);    // 0xa8b9e0
	void TakeOff_AuxPartNCartList(CPartTidList& tidList, unsigned long overlapMask); // 0xa8bbb0
	// KR CShopMain members SelectItemList / OnItemListBtnDown call (KR
	// names, not decompiled yet).
	void PutOnNBackupShoppersOldItem(unsigned long tid, CPartTidList& tidList, unsigned long id); // 0xa8be80
	void ShowMainModel(CPartTidList& tidList);                        // 0xa8c080
	void ShowItemDesc(unsigned long typeId);                          // 0xa85ee0

	// KR CShopMain members (KR names; JP offsets).
	// The KR OnBuyDlgResult (the checkout dialog's result callback).
	bool OnBuyDlgResult(int result, FrForm* form);                    // 0xa8c490
	// The KR GetItemNumInMyPocket (JP: a 0x18 item of sub kind 1 always counts).
	int GetItemNumInMyPocket(unsigned long typeId, bool skipFlagged); // 0xa885a0
	// The KR SendGift (JP: the main actor opens the gift page, message 0x4D).
	void SendGift(const IFF_ITEM_COMMON& item);                       // 0xa873d0
	void UpgradeCaddie(unsigned long caddieTypeId);                   // 0xa8a460
	// The KR DrawStatus3 (the cart rows' stat lines).
	void DrawStatus3(FrGraphicInterface* graphic, float x, float y, short* const status, short* const slotStatus); // 0xa83fc0
	// JP only (name ours): places the cart / aux cart lists beside the avatar
	// rect m_rect6b0, then CheckAuxCartScroll.
	void PlaceCartLists(void);                                        // 0xa84da0
	void InitShopDialogs(void);                                       // 0xa82300
	void InitShopControls(void);                                      // 0xa82310
	void HidePet(bool bWait);                                         // 0xa82610
	void SafeLoadPet(unsigned long* pTypeId, bool bForce);            // 0xa82880
	void ChangeStatTab(statTab_t tab);                                // 0xa82870
	void CheckAuxCartScroll(void);                                    // 0xa840b0
	void LoadShopImages(void);                                        // 0xa847e0
	// The KR OpenLayout plus the JP 0x00FE request (docs/protocol/shop.md,
	// "JP shop page opening"); HandleMsg 0xa97888 calls it.
	void OpenLayout(const char* layoutName);                          // 0xa86990
	// JP only (name ours): m_levelPlusOne / m_level from the JP CSharedDoc
	// byte +0x91d (+1 / as is); no KR counterpart.
	void InitLevelFields(void);                                       // 0xa876f0
	// JP only (name ours): the shop-enter request 0x0140 again
	// (docs/protocol/lobby.md), then main-actor message 1; HandleMsg's
	// notices end with it.
	void RequestShopEnter(void);                                      // 0xa87980
	void DeleteCutinImages(void);                                     // 0xa88120
	// The KR GetCutinImg (the cut-in picture cache).
	WOverlay* GetCutinImg(unsigned long typeId);                      // 0xa8c230
	void DeleteCardImages(void);                                      // 0xa88dd0
	void SetLevelBar(void);                                           // 0xa8b410
	void ClearCartList(bool bKeepCharacters, CPartTidList* pTidList); // 0xa8b690
	// The KR RestoreShoppersOldItem (not decompiled yet).
	void RestoreShoppersOldItem(unsigned long typeId, CPartTidList& tidList); // 0xa8a390
	void TurnOnItemStatus(void);                                      // 0xa8c1d0
	void BuildDisplayRental(subTab_t tab);                            // 0xa8c310
	void TurnOffItemStatus(void);                                     // 0xa8c200
	void UnselectItemList(unsigned long typeId);                      // 0xa8da20
	void ChangeMainTab(mainTab_t tab, bool bForce);                   // 0xa8fa30
	void InitalizeUpgradeCaddieInfo(void);                            // 0xa90520
	void BuildVisibleCaddieList(void);                                // 0xa90650
	void BuildDisplayStand(subTab_t tab);                             // 0xa923c0
	void ChangeSubTab(subTab_t tab, bool bForce);                     // 0xa92590
	void ChangeCharacter(unsigned long typeId);                       // 0xa92810
	void ChangeCaddie(unsigned long typeId);                          // 0xa92b20
	void ChangeMascot(unsigned long typeId);                          // 0xa92b60
	void ChangeClubset(unsigned long typeId);                         // 0xa92ba0
	void ChangeAztec(unsigned long typeId);                           // 0xa92be0

	// The item list's mini buttons (the KR CShopMain::miniBtn_t; the JP
	// constructor builds nothing there).
	struct miniBtn_t {
		WRect rect;              // 0x00
		const Bitmap* normal;    // 0x10
		const Bitmap* over;      // 0x14
		bool enable;             // 0x18
	};

	// A part the shopper wore before trying an item on (the KR
	// CShopMain::oldItem_t; JP list node 0x14).
	struct oldItem_t {
		unsigned long typeId;      // 0x0
		unsigned long tid;         // 0x4
		unsigned long id;          // 0x8
	};

	// 0x114-0x120: FrTaskForm's tail (see the file comment); 0x120: CHandOwner.
	FrEdit* m_hintWnd;                      // 0x124, cleared by InitShopControls
	float m_hintTime;                       // 0x128, -10 (InitShopControls)
	PADDING(0x12c, 0x130);
	FrTabButton* m_tabMainButton;           // 0x130 (OnTabMainInit)
	FrTabButton* m_tabSubButton;            // 0x134 (OnTabSubInit)
	FrButton* m_buyAll;                     // 0x138 (OnBuyAllInit)
	FrButton* m_buyReset;                   // 0x13c (OnBuyResetInit)
	FrListBox* m_itemListBox;               // 0x140 (OnItemListInit)
	FrListBox* m_cartList;                  // 0x144 (OnPartCartListInit)
	FrListBox* m_auxCartList;               // 0x148 (OnAuxPartCartListInit)
	FrArea* m_tabTxt2;                      // 0x14c (OnTabTxt2Init)
	FrArea* m_tabTxt3;                      // 0x150 (OnTabTxt3Init)
	FrWnd* m_shopControl154;                // 0x154, cleared by InitShopControls only (the KR m_shopControl228)
	FrArea* m_petArea;                      // 0x158, = m_pet in OnInit
	FrGroupBox* m_statBox;                  // 0x15c, "stat_group" (OnStatGroupInit)
	FrArea* m_statArrow;                    // 0x160, "stat_group.arrow_m" (OnStatGroupArrowInit)
	FrStatic* m_statEdit[5];                // 0x164, power/control/impact/spin/curve (On<Stat>EditInit)
	FrButton* m_scrollUp;                   // 0x178 (OnScrollUpInit)
	FrButton* m_scrollDown;                 // 0x17c (OnScrollDownInit)
	FrArea* m_itemDescBg;                   // 0x180 (OnItemDescBgInit)
	FrEdit* m_itemDesc;                     // 0x184 (OnItemDescInit)
	FrArea* m_skinPet;                      // 0x188 (OnSkinPetInit)
	FrArea* m_skinPet2;                     // 0x18c (OnSkinPet2Init)
	FrArea* m_pet;                          // 0x190 (OnPetInit)
	FrArea* m_itemSlotFrame1;               // 0x194 (OnItemSlotFrame1Init)
	FrArea* m_itemSlotFrame2;               // 0x198 (OnItemSlotFrame2Init)
	FrListBox* m_itemSlot;                  // 0x19c (OnItemSlotInit)
	FrButton* m_sideTabBuy;                 // 0x1a0 (OnSideTabBuyInit)
	FrButton* m_sideTabRental;              // 0x1a4 (OnSideTabRentalInit)
	FrForm* m_buyDlg;                       // 0x1a8, cleared by InitShopDialogs
	FrUpgradeCaddieDlg* m_upgradeCaddieDlg; // 0x1ac (UpgradeCaddie 0xa8a460)
	std::map<unsigned long, unsigned long> m_upgradeCaddies; // 0x1b0, node 0x18 (InitalizeUpgradeCaddieInfo)
	PADDING(0x1bc, 0x1c0);
	std::map<unsigned int, IFF_STRUCT::sCaddie> m_visibleCaddies; // 0x1c0, node 0x10c (BuildVisibleCaddieList)
	// 0x1cc-0x3c8: the pictures LoadShopImages loads (KR names).
	PADDING(0x1cc, 0x1d0);
	const Bitmap* m_itemBaseNormal;         // 0x1d0, "item_base_n"
	const Bitmap* m_itemBaseOver;           // 0x1d4, "item_base_o"
	const Bitmap* m_itemBaseDown;           // 0x1d8, "item_base_d"
	const Bitmap* m_itemBaseEmpty;          // 0x1dc, "item_base_empty"
	const Bitmap* m_itemBaseAward;          // 0x1e0, "item_base_award"
	PADDING(0x1e4, 0x1ec);
	const Bitmap* m_markNew;                // 0x1ec, "mark_new"
	const Bitmap* m_markHot;                // 0x1f0, "mark_hot"
	const Bitmap* m_markLimited;            // 0x1f4, "mark_lim"
	const Bitmap* m_markLimitedTag;         // 0x1f8, "mark_limitetag" (JP)
	const Bitmap* m_markLimitedType[3];     // 0x1fc, "mark_limite_type_01".."03" (JP)
	const Bitmap* m_markSurpriseSale;       // 0x208, "mark_surprise_sale" (JP)
	const Bitmap* m_markBegin;              // 0x20c, "mark_begin", cleared by InitShopControls
	PADDING(0x210, 0x21c);
	const Bitmap* m_dotBitmaps[5][2];       // 0x21c, "ab_<stat>" / "ab_<stat>_slot"
	const Bitmap* m_selectUp;               // 0x244, "select_up"
	const Bitmap* m_selectMid;              // 0x248, "select_mid"
	const Bitmap* m_selectLow;              // 0x24c, "select_low"
	const Bitmap* m_selectEmpty;            // 0x250, "select_empty"
	const Bitmap* m_selectCursor1;          // 0x254, "select_cursor1"
	const Bitmap* m_selectCursor0;          // 0x258, "select_cursor0"
	const Bitmap* m_statPartBitmap[3][3];   // 0x25c, "stat_char/club/caddie" _n/_o/_d
	PADDING(0x280, 0x28c);
	const Bitmap* m_sicons[5];              // 0x28c, "sicon1".."sicon5"
	const Bitmap* m_itFa02;                 // 0x2a0, "it_fa02"
	const Bitmap* m_cancelBtnNormal;        // 0x2a4, "cancel_btn_n"
	const Bitmap* m_cancelBtnOver;          // 0x2a8, "cancel_btn_o"
	PADDING(0x2ac, 0x2b0);
	const Bitmap* m_iconSex[2];             // 0x2b0, "i_male" / "i_female"
	const Bitmap* m_masterSign;             // 0x2b8, "master_sign"
	const Bitmap* m_teamBase;               // 0x2bc, "team_base"
	const Bitmap* m_itemSlotNormal;         // 0x2c0, "item_slot_n"
	const Bitmap* m_selectionArrow;         // 0x2c4, "selection_arrow", cleared by InitShopControls
	const Bitmap* m_itemDescBitmap[2];      // 0x2c8, "itemdesc_01" / "itemdesc_02" (OnItemDescBgOwnerDraw)
	PADDING(0x2d0, 0x2e0);
	const Bitmap* m_selectItemMark;         // 0x2e0, "SelectItem_Mark" (JP), cleared by InitShopControls
	// The type id whose row OnItemListOwnerDraw marks with m_selectItemMark
	// (JP; its setter is not identified), cleared by the constructor.
	unsigned long m_selectItemMarkTypeId;   // 0x2e4
	// [1] buy, [2] gift (OnSideTabBuyUp), [7] "state_view" (JP).
	miniBtn_t m_miniBtns[8];                // 0x2e8
	std::map<unsigned long, const Bitmap*> m_itemIcons; // 0x3c8, icon cache by type id
	PADDING(0x3d4, 0x3d8);
	std::map<unsigned int, WOverlay*> m_cardImgs;       // 0x3d8, card picture cache
	PADDING(0x3e4, 0x3e8);
	std::map<unsigned int, WOverlay*> m_cutinImgs;      // 0x3e8, cut-in picture cache
	PADDING(0x3f4, 0x404);
	const char* m_charLevels;               // 0x404
	const DWORD* m_charParts;               // 0x408
	unsigned long m_charPartIds[24];        // 0x40c
	const DWORD* m_charCards;               // 0x46c
	unsigned char m_hairColor;              // 0x470
	PADDING(0x471, 0x474);
	unsigned long m_standPetTypeId;         // 0x474, the shown character (OnInit prefers it over the equipped one)
	unsigned long m_caddieTypeId;           // 0x478, 0 (KR 0x1C000000)
	unsigned long m_mascotTypeId;           // 0x47c, 0x40000000
	unsigned long m_aztecTypeId;            // 0x480, 0x14000000
	unsigned long m_clubsetTypeId;          // 0x484, 0x10000000 (KR 0x0C000000)
	statTab_t m_statTab;                    // 0x488 (ChangeStatTab)
	int m_shopTab;                          // 0x48c
	int m_tab490;                           // 0x490, 0 at construction, the shop tab ChangeMainTab last set (JP)
	subTab_t m_subTab;                      // 0x494, 0 at construction (KR 1)
	float m_blinkValue;                     // 0x498
	float m_blinkTime;                      // 0x49c
	WPoint m_itemDescOffset;                // 0x4a0
	WPoint m_itemDescPos;                   // 0x4a8 (OnItemDescInit)
	int m_statPartKind;                     // 0x4b0
	CExhibition* m_petExhibition;           // 0x4b4 (HidePet)
	CAnotherHand m_anotherHand;             // 0x4b8 (ctor 0x98ad90, dtor 0x98add0; JP 0x184 bytes)
	std::list<unsigned long> m_rbuttonPartTypeIds; // 0x63c
	std::list<oldItem_t> m_shoppersOldItems;       // 0x648
	PADDING(0x654, 0x658);
	unsigned long m_equipSkin[6];           // 0x658, the CSharedDoc equipped skins (OnInit)
	float m_sidePos[2];                     // 0x670
	int m_sideDir[2];                       // 0x678
	unsigned long m_cardTypeId;             // 0x680
	FrComboBox* m_sortOptionCombo;          // 0x684 (OnSortOptionComboInit)
	FrTextButton* m_searchGoodsButton;      // 0x688 (OnSearchGoodsButtonInit)
	FrButton* m_itemSearchBtn;              // 0x68c, "item_search_btn" (JP)
	FrArea* m_mainBg[4];                    // 0x690, "main_bg1".."main_bg4" (JP)
	int m_levelPlusOne;                     // 0x6a0 (InitLevelFields; JP)
	int m_level;                            // 0x6a4 (InitLevelFields; JP)
	int m_index6a8;                         // 0x6a8, -1 at construction (JP; role not identified)
	WTitleFont* m_titleFont;                // 0x6ac, "[font_wind.jpg" digits 16x16, released by the destructor
	WRect m_rect6b0;                        // 0x6b0, the zero rect data_f145f8 at construction (JP; role not identified)
};
CHECK_SIZE(JpShopMain, 0x6c0);
BEGIN_LAYOUT_CHECK(JpShopMain)
	CHECK_FIELD(JpShopMain, m_hintWnd, 0x124);
	CHECK_FIELD(JpShopMain, m_tabMainButton, 0x130);
	CHECK_FIELD(JpShopMain, m_petArea, 0x158);
	CHECK_FIELD(JpShopMain, m_statEdit, 0x164);
	CHECK_FIELD(JpShopMain, m_sideTabRental, 0x1a4);
	CHECK_FIELD(JpShopMain, m_buyDlg, 0x1a8);
	CHECK_FIELD(JpShopMain, m_upgradeCaddieDlg, 0x1ac);
	CHECK_FIELD(JpShopMain, m_upgradeCaddies, 0x1b0);
	CHECK_FIELD(JpShopMain, m_visibleCaddies, 0x1c0);
	CHECK_FIELD(JpShopMain, m_itemBaseNormal, 0x1d0);
	CHECK_FIELD(JpShopMain, m_markNew, 0x1ec);
	CHECK_FIELD(JpShopMain, m_markBegin, 0x20c);
	CHECK_FIELD(JpShopMain, m_dotBitmaps, 0x21c);
	CHECK_FIELD(JpShopMain, m_selectUp, 0x244);
	CHECK_FIELD(JpShopMain, m_statPartBitmap, 0x25c);
	CHECK_FIELD(JpShopMain, m_sicons, 0x28c);
	CHECK_FIELD(JpShopMain, m_iconSex, 0x2b0);
	CHECK_FIELD(JpShopMain, m_itemSlotNormal, 0x2c0);
	CHECK_FIELD(JpShopMain, m_selectionArrow, 0x2c4);
	CHECK_FIELD(JpShopMain, m_itemDescBitmap, 0x2c8);
	CHECK_FIELD(JpShopMain, m_selectItemMark, 0x2e0);
	CHECK_FIELD(JpShopMain, m_miniBtns, 0x2e8);
	CHECK_SIZE(JpShopMain::miniBtn_t, 0x1c);
	CHECK_FIELD(JpShopMain, m_itemIcons, 0x3c8);
	CHECK_FIELD(JpShopMain, m_cardImgs, 0x3d8);
	CHECK_FIELD(JpShopMain, m_cutinImgs, 0x3e8);
	CHECK_FIELD(JpShopMain, m_charLevels, 0x404);
	CHECK_FIELD(JpShopMain, m_charPartIds, 0x40c);
	CHECK_FIELD(JpShopMain, m_charCards, 0x46c);
	CHECK_FIELD(JpShopMain, m_hairColor, 0x470);
	CHECK_FIELD(JpShopMain, m_standPetTypeId, 0x474);
	CHECK_FIELD(JpShopMain, m_clubsetTypeId, 0x484);
	CHECK_FIELD(JpShopMain, m_statTab, 0x488);
	CHECK_FIELD(JpShopMain, m_subTab, 0x494);
	CHECK_FIELD(JpShopMain, m_itemDescPos, 0x4a8);
	CHECK_FIELD(JpShopMain, m_statPartKind, 0x4b0);
	CHECK_FIELD(JpShopMain, m_petExhibition, 0x4b4);
	CHECK_FIELD(JpShopMain, m_anotherHand, 0x4b8);
	CHECK_FIELD(JpShopMain, m_rbuttonPartTypeIds, 0x63c);
	CHECK_FIELD(JpShopMain, m_shoppersOldItems, 0x648);
	CHECK_FIELD(JpShopMain, m_equipSkin, 0x658);
	CHECK_FIELD(JpShopMain, m_sidePos, 0x670);
	CHECK_FIELD(JpShopMain, m_sideDir, 0x678);
	CHECK_FIELD(JpShopMain, m_cardTypeId, 0x680);
	CHECK_FIELD(JpShopMain, m_sortOptionCombo, 0x684);
	CHECK_FIELD(JpShopMain, m_itemSearchBtn, 0x68c);
	CHECK_FIELD(JpShopMain, m_mainBg, 0x690);
	CHECK_FIELD(JpShopMain, m_levelPlusOne, 0x6a0);
	CHECK_FIELD(JpShopMain, m_index6a8, 0x6a8);
	CHECK_FIELD(JpShopMain, m_titleFont, 0x6ac);
	CHECK_FIELD(JpShopMain, m_rect6b0, 0x6b0);
END_LAYOUT_CHECK

#endif // PG_JP_JPSHOPMAIN_H
