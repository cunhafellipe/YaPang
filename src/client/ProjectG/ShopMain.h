// Original: d:\build\custom\temp\code\client\projectg\shopmain.h
#ifndef PG_CLIENT_PROJECTG_SHOPMAIN_H
#define PG_CLIENT_PROJECTG_SHOPMAIN_H

#include <list>
#include <map>

#include "defines.h"
#include "padding.h"
#include "client/Fresh/include/frwnd.h"
#include "client/Fresh/include/frbutton.h"
#include "client/Fresh/include/frtabbutton.h"
#include "client/Fresh/include/frlistbox.h"
#include "client/Fresh/include/frarea.h"
#include "client/Fresh/include/frstatic.h"
#include "client/Fresh/include/frviewer.h"
#include "client/Fresh/include/frgroupbox.h"
#include "client/Fresh/include/frgaugebar.h"
#include "TaskMain.h"
#include "AnotherHand.h"
#include "exhibition.h"
#include "shared/classdefine.h"

class FrArea;
class MsgObject;
class CPartTidList;
class Bitmap;
class FrForm;
class FrGiftDlg;
class FrGraphicInterface;
class WOverlay;
class WRTTI;
class FrEdit;
class FrComboBox;
class FrTextButton;
class FrUpgradeCaddieDlg;
struct IFF_ITEM_COMMON;
struct sCaddieInfo;
struct sFRESH_MSGMAP;

// The shop task. Its size (0x740) is the `new` of CShopMainMakeInstance;
// CHandOwner (0x1E0) receives the pet model loaded by m_anotherHand
// (OnAnotherHand adjusts `this` by -0x1E0). Only the members the
// transcribed handlers touch are named.
// The server's record of a bought item, byte-packed, passed as param5 of the
// shop's item messages 0xB5-0xBC. Only the members HandleMsg reads are named.
#pragma pack(push, 1)
struct sBoughtItem {
	unsigned long typeId;      // 0x0
	unsigned long id;          // 0x4
	unsigned short hours;      // 0x8
	unsigned char flag;        // 0xA
	unsigned short count;      // 0xB, the stack size; 0 keeps the item's default quantity
	_SYSTEMTIME endTime;       // 0xD, the expiry (CShopMain::HandleMsg 0xBF copies it into sMascotInfo::endTime)
	char uccKey[9];            // 0x1D, the self-design key of a UCC item
};
#pragma pack(pop)
CHECK_FIELD(sBoughtItem, count, 0xB);
CHECK_FIELD(sBoughtItem, uccKey, 0x1D);
CHECK_FIELD(sBoughtItem, endTime, 0xD);

class CShopMain : public CTaskMain, public CHandOwner {
public:
	CShopMain(void);
	virtual ~CShopMain(void);

	static const WRTTI m_RTTI;
	virtual const WRTTI *GetRTTI(void) const { return &m_RTTI; } // shopmain.h:38

	// A small button drawn on each item of the item list (0x1C bytes): its
	// rectangle relative to the item, its pictures and whether it is active
	// (inactive buttons are drawn dimmed and do not react). HandleMsg 0xC0
	// switches all of them while a purchase is pending.
	struct miniBtn_t {
		miniBtn_t(void);

		WRect rect;              // 0x00
		const Bitmap *normal;    // 0x10
		const Bitmap *over;      // 0x14
		bool enable;             // 0x18
	};

	// Main tab of the shop (ChangeMainTab); values known by number only.
	enum mainTab_t {
		MAIN_TAB_0 = 0
	};

	// Tab of the status box: per-part stats or the whole set's.
	enum statTab_t {
		STAT_TAB_PART = 0,
		STAT_TAB_ALL = 1
	};

	// Sub tab of the shop (ChangeSubTab); values known by number only.
	enum subTab_t {
		SUB_TAB_NONE = 0,
		SUB_TAB_CLUBSET = 6,   // ChangeClubset
		SUB_TAB_AZTEC = 7,     // ChangeAztec
		SUB_TAB_MASCOT = 12    // ChangeMascot
	};

	virtual void OnInit(void);
	virtual void OnDestroy(void);
	virtual void OnLoad(void);
	virtual void OnPreLoadInit(void);
	virtual void OnProcess(float dt);
	virtual void HandleMsg(const MsgObject &msg);

protected:
	static sFRESH_MSGMAP _MsgMap;
	virtual const sFRESH_MSGMAP *GetMessageMap(void) const;

	virtual void OnAnotherHand(int nWork, void *pData);

	void OnShopMainInit(int);
	void LoadItemImages(void);
	void LoadShopImages(void);
	void ShowPet(unsigned long typeId);
	void SetPetCtrl(unsigned long typeId);
	void RestoreShoppersOldItem(unsigned long typeId, CPartTidList &tidList);
	void PutOnNBackupShoppersOldItem(unsigned long tid, CPartTidList &tidList, unsigned long id);
	void ShowMainModel(CPartTidList &tidList);
	bool HaveCharacter(unsigned long typeId);
	void OnStatPartOwnerDraw(int);
	void OnPetInit(int);
	void OnPowerBarInit(intptr_t);
	void OnControlBarInit(intptr_t);
	void OnImpactBarInit(intptr_t);
	void OnSpinBarInit(intptr_t);
	void OnCurveBarInit(intptr_t);
	void OnScrollUpInit(int);
	void OnScrollDownInit(int);
	void OnItemSlotFrame1Init(int);
	void OnItemSlotFrame2Init(int);
	void OnItemSlotInit(intptr_t);
	void OnStatAllInit(int);
	void OnStatAllOwnerDraw(int);
	void OnBuyAllInit(int);
	void OnBuyResetInit(int);
	void OnSideTabBuyInit(int);
	void OnSideTabRentalInit(int);
	void OnSideTabBuyUp(void);
	void ResetItemList(bool bKeepType1);
	bool SelectItemList(unsigned long typeId);
	bool SelectItemList(FrListItem *pItem);
	bool UnselectItemFromItemList(unsigned long typeId);
	const Bitmap *GetItemIcon(unsigned long typeId);
	void Repay(const IFF_ITEM_COMMON &item, unsigned long guid);
	bool OnSellDlgResult(int result, FrForm *form);
	bool OnBuyDlgResult(int result, FrForm *form);
	void OnItemDescBgOwnerDraw(int);
	void SetMyItemSlot(void);
	void BuildMyItemSlot(bool);
	void OnSideTabRentalUp(void);
	void ClearCartList(bool, CPartTidList *);
	void BuildDisplayRental(subTab_t tab);
	void OnSelectedNameInit(int);
	void OnStatFrameInit(int);
	void OnStatTextInit(int);
	void OnStatPartInit(int);
	void OnTabTxt1Init(int);
	void OnTabTxt2Init(int);
	void OnTabTxt3Init(int);
	void OnSelItemIconInit(int);
	void OnStatBoxInit(int);
	void OnSelFrameInit(int);
	void OnPowerEditInit(int);
	void OnControlEditInit(int);
	void OnImpactEditInit(int);
	void OnSpinEditInit(int);
	void OnCurveEditInit(int);
	void OnSelItemInit(int);
	void OnSelItemPrevInit(int);
	void OnSelItemNextInit(int);
	void OnItemDescBgInit(int);
	void OnTabSubInit(int);
	void OnPartCartListInit(int);
	void OnAuxPartCartListInit(int);
	void OnSkinPetInit(int);
	void OnSkinPet2Init(int);
	void BuildDisplayStand(subTab_t tab);
	void SafeLoadPet(unsigned long *pTypeId, bool bForce);
	void ChangeMascot(unsigned long typeId);
	void ChangeSkin(unsigned long typeId);
	void ChangeClubset(unsigned long typeId);
	void ChangeAztec(unsigned long typeId);
	void ChangeCharacter(unsigned long typeId);
	void ChangeCaddie(unsigned long typeId);
	void BuildVisibleCaddieList(void);

	void OnSortOptionComboDown(void);
	void OnSearchGoodsLButtonDown(void);
	void OnSelItemOwnerDraw(int itemIndex);
	void DrawSelArrow(FrGraphicInterface *graphic);
	void OnSelItemUp(void);
	void OnStatPartUp(void);
	void OnStatAllUp(void);
	bool OnGiftDlgResult(int result, FrForm *form);
	void ChangeStatTab(statTab_t tab);
	void ChangeMainTab(mainTab_t tab, bool bForce);
	void ChangeSubTab(subTab_t tab, bool bForce);
	void OnTabSubButtonUp(void);
	bool IsSelectedRentalTab(void);
	void SetAutoScroll(void);
	void Net_StartPeriodItem(unsigned long typeId);
	FrListBox::ITEM_LIST::iterator DeleteItemFromCartList(unsigned long typeId);
	FrListBox::ITEM_LIST::iterator DeleteItemFromAuxCartList(unsigned long typeId);
	bool IsVisibleInStock(IFF_ITEM_COMMON &item);
	void InitShopDialogs(void);
	void TurnOnItemStatus(void);
	void TurnOffItemStatus(void);
	void SetLevelBar(void);
	void SetVisibleSideTab(bool bVisible);
	void TurnOffAuxCartScroll(void);
	void TurnOnAuxCartScroll(void);
	void CheckAuxCartScroll(void);
	void OnScrollUpUp(void);
	void OnScrollDownUp(void);
	void TurnOnItemSelect(void);
	void TurnOffItemSelect(void);
	void HidePet(bool bWait);
	void SetPetAreaRect(void);
	virtual void CloseAllDialogs(void);
	void DrawCutinFrame(void);
	void SetItemDescRect(void);
	void ShowItemDesc(unsigned long typeId);
	void DrawStatus(FrGraphicInterface *graphic, FrListItem *listItem, short *const status, short *const slotStatus, float x);
	void DrawStatus2(FrGraphicInterface *graphic, FrListItem *listItem, short *const status, short *const slotStatus, float x);
	void DrawStatusCard(FrGraphicInterface *graphic, FrListItem *listItem, short *const cardStatus, float x);
	void DrawPartsSlot(FrGraphicInterface *graphic, FrListItem *listItem);
	void DrawRentalPrice(float x, float y, IFF_ITEM_COMMON &item, FrListItem *listItem);
	void DrawStatus3(FrGraphicInterface *graphic, float x, float y, short *const status, short *const slotStatus);
	void OpenLayout(const char *layoutName);
	bool OnReemployeeResult(int result, FrForm *form);
	void OpenCaddieInfo(const sCaddieInfo &info);
	WOverlay *GetCardImg(unsigned long typeId);
	WOverlay *GetCutinImg(unsigned long typeId);
	void InitalizeUpgradeCaddieInfo(void);
	void OnSortOptionComboInit(int);
	void OnSearchGoodsButtonInit(int);
	void OnItemDescInit(int);
	bool IsItemInCart(FrListItem *listItem);
	void InitShopControls(void);
	void LoadCardImages(void);
	void DeleteCardImages(void);
	void DeleteCutinImages(void);
	void DrawSkinFrame(void);
	void DrawSkinSlot(void);
	void DrawSelItem(FrGraphicInterface *graphic, unsigned long typeId);
	void DrawMascotList(FrGraphicInterface *graphic, unsigned long typeId);
	void DrawCharacterList(FrGraphicInterface *graphic, unsigned long typeId);
	void DrawCaddieList(FrGraphicInterface *graphic, unsigned long typeId);
	void OnPetOwnerDraw(int);
	void OnSelItemIconOwnerDraw(int);
	void OnItemListInit(intptr_t);
	void OnBuyHoverOn(int);
	void OnBuyHoverOff(int);
	void OnTabMainInit(int);
	void DrawCard(void);
	void OnTabMainButtonUp(void);
	void OnItemListRBtnDown(void);
	void OpenInformation(unsigned long typeId);
	int GetItemNumInMyPocket(unsigned long typeId, bool skipFlagged);
	void UnselectItemList(unsigned long typeId);
	void OnShopMainFinish(void);
	void SendGift(const IFF_ITEM_COMMON &item);
	void UpgradeCaddie(unsigned long caddieTypeId);
	void DrawItemPrice(FrGraphicInterface *graphic, float x, float y, IFF_ITEM_COMMON &item, FrListItem *listItem);
	void OnItemSlotOwnerDraw(int);
	void OnPartCartListOwnerDraw(int);
	void OnAuxPartCartListOwnerDraw(int);
	void OnItemListOwnerDraw(int);
	void OnBuyAllUp(void);
	void Checkout(void);
	void OnItemListBtnDown(void);
	void OnSelItemPrevUp(void);
	void OnSelItemNextUp(void);
	void OnBuyResetUp(void);

private:
	LAYOUT_CHECK_FRIEND;
	static sFRESH_ENTRY _MsgEntries[];

	void SetLevelBar_Char(void);
	void SetLevelBar_Clubset(void);
	void Build_Hair(subTab_t tab, int);
	void Build_Parts(subTab_t tab, int);
	void Build_Clubset(subTab_t tab, int);
	void Build_Aztec(subTab_t tab, int);
	void Build_Equip(subTab_t tab, int);
	void Build_NoEquip(subTab_t tab, int);
	void Build_Card(subTab_t tab, int);
	void Build_Interior(subTab_t tab, int);
	void Build_Furniture(subTab_t tab, int);
	void Build_Decorate(subTab_t tab, int);
	void Build_Cutin(subTab_t tab, int);
	void Build_Caddie(subTab_t tab, int);
	void Build_Mascot(subTab_t tab, int);
	void Build_Package(subTab_t tab, int);
	void TakeOff_PartNCartList(CPartTidList &tidList, unsigned long overlapMask);
	void TakeOff_AuxPartNCartList(CPartTidList &tidList, unsigned long overlapMask);

	FrEdit *m_hintWnd;        // 0x1e4, the hint line box, hidden when m_hintTime runs out (OnProcess)
	float m_hintTime;         // 0x1e8, counts down while positive (OnProcess)
	FrStatic *m_selectedName; // 0x1ec (OnSelectedNameInit)
	FrArea *m_statFrame;      // 0x1f0 (OnStatFrameInit)
	FrArea *m_statText;       // 0x1f4 (OnStatTextInit)
	FrViewer *m_statPart;     // 0x1f8 (OnStatPartInit), owner-drawn by OnStatPartOwnerDraw
	FrViewer *m_statAll;      // 0x1fc (OnStatAllInit), its rect is OnStatPartOwnerDraw's second picture
	FrTabButton *m_tabMainButton; // 0x200, the main tabs (OnTabMainInit)
	FrTabButton *m_tabSubButton; // 0x204, the sub tabs (OnTabSubButtonUp)
	FrButton *m_buyAll;       // 0x208 (OnBuyAllInit)
	FrButton *m_buyReset;     // 0x20c (OnBuyResetInit)
	FrListBox *m_itemListBox; // 0x210, the goods of the current tab (SetAutoScroll, SortItem)
	FrListBox *m_cartList;    // 0x214, the cart (DeleteItemFromCartList)
	FrListBox *m_auxCartList; // 0x218, the second cart, scrolled by OnScrollUpUp/OnScrollDownUp
	FrArea *m_tabTxt1;        // 0x21c (OnTabTxt1Init)
	FrArea *m_tabTxt2;        // 0x220 (OnTabTxt2Init)
	FrArea *m_tabTxt3;        // 0x224 (OnTabTxt3Init)
	FrWnd *m_shopControl228;  // 0x228, a control cleared by InitShopControls; its init handler is not identified
	FrArea *m_petArea;        // 0x22c, one of the three pet areas (SetPetAreaRect)
	FrArea *m_selItemIcon;    // 0x230 (OnSelItemIconInit)
	FrButton *m_selItem;      // 0x234 (OnSelItemInit)
	FrGroupBox *m_statBox;    // 0x238 (OnStatBoxInit)
	FrArea *m_selFrame;       // 0x23c (OnSelFrameInit)
	FrGaugeBar *m_statBar[5];  // 0x240, power/control/impact/spin/curve (On<Stat>BarInit)
	FrStatic *m_statEdit[5];   // 0x254, power/control/impact/spin/curve (On<Stat>EditInit)
	FrButton *m_selItemPrev;  // 0x268 (OnSelItemPrevInit)
	FrButton *m_selItemNext;  // 0x26c (OnSelItemNextInit)
	FrButton *m_scrollUp;     // 0x270 (OnScrollUpInit)
	FrButton *m_scrollDown;   // 0x274 (OnScrollDownInit)
	FrArea *m_itemDescBg;     // 0x278 (OnItemDescBgInit)
	FrEdit *m_itemDesc;       // 0x27c, the description of the selected item (SetItemDescRect)
	FrArea *m_skinPet;        // 0x280 (OnSkinPetInit)
	FrArea *m_skinPet2;       // 0x284 (OnSkinPet2Init)
	FrArea *m_pet;            // 0x288 (OnPetInit)
	FrArea *m_itemSlotFrame1; // 0x28c (OnItemSlotFrame1Init)
	FrArea *m_itemSlotFrame2; // 0x290 (OnItemSlotFrame2Init)
	FrListBox *m_itemSlot;    // 0x294 (OnItemSlotInit)
	FrButton *m_sideTabBuy;   // 0x298 (OnSideTabBuyInit)
	FrButton *m_sideTabRental; // 0x29c (OnSideTabRentalInit)
	FrForm *m_buyDlg;         // 0x2a0, cleared by OnBuyDlgResult
	FrUpgradeCaddieDlg *m_upgradeCaddieDlg; // 0x2a4, created by UpgradeCaddie
	std::map<unsigned long, unsigned long> m_upgradeCaddies; // 0x2a8, caddie type id -> its upgraded type id (InitalizeUpgradeCaddieInfo)
	std::map<unsigned int, IFF_STRUCT::sCaddie> m_visibleCaddies; // 0x2b4, built by BuildVisibleCaddieList
	const Bitmap *m_itemBaseNormal;   // 0x2c0, item_base_n (LoadShopImages)
	const Bitmap *m_itemBaseOver;     // 0x2c4, item_base_o
	const Bitmap *m_itemBaseDown;     // 0x2c8, item_base_d
	const Bitmap *m_itemBaseEmpty;    // 0x2cc, item_base_empty
	const Bitmap *m_itemBaseAward;    // 0x2d0, item_base_award
	PADDING(0x2d4, 0x2dc);
	const Bitmap *m_markNew;          // 0x2dc, mark_new (LoadShopImages)
	const Bitmap *m_markHot;          // 0x2e0, mark_hot
	const Bitmap *m_markLimited;      // 0x2e4, mark_lim
	const Bitmap *m_markBegin;        // 0x2e8, mark_begin, cleared by InitShopControls
	PADDING(0x2ec, 0x2f8);
	const Bitmap *m_dotBitmaps[5][2]; // 0x2f8, the pictures named by dot_files (LoadShopImages)
	const Bitmap *m_selectUp;         // 0x320, select_up (LoadShopImages)
	const Bitmap *m_selectMid;        // 0x324, select_mid
	const Bitmap *m_selectLow;        // 0x328, select_low
	const Bitmap *m_selectEmpty;      // 0x32c, select_empty
	const Bitmap *m_selectCursor1;    // 0x330, select_cursor1
	const Bitmap *m_selectCursor0;    // 0x334, select_cursor0
	const Bitmap *m_statPartBitmap[3][3]; // 0x338, stat_char/club/caddie _n/_o/_d; OnStatPartOwnerDraw draws [m_statPartKind][2]
	const Bitmap *m_statAllBitmap[3]; // 0x35c, the stat-all frame: normal / under the cursor / selected (OnStatAllOwnerDraw)
	const Bitmap *m_sicons[5];        // 0x368, sicon1..sicon5 (LoadShopImages)
	const Bitmap *m_itFa02;           // 0x37c, it_fa02
	const Bitmap *m_cancelBtnNormal;  // 0x380, cancel_btn_n
	const Bitmap *m_cancelBtnOver;    // 0x384, cancel_btn_o
	PADDING(0x388, 0x38c);
	const Bitmap *m_iconSex[2];       // 0x38c, i_male / i_female, indexed by bit 1 of the user info flags (DrawSkinSlot)
	const Bitmap *m_masterSign;       // 0x394, master_sign
	const Bitmap *m_teamBase;         // 0x398, team_base
	const Bitmap *m_itemSlotNormal;   // 0x39c, item_slot_n
	const Bitmap *m_selectionArrow;   // 0x3a0, selection_arrow, cleared by InitShopControls
	const Bitmap *m_itemDescBitmap[2]; // 0x3a4, itemdesc_01 / itemdesc_02 (OnItemDescBgOwnerDraw)
	PADDING(0x3ac, 0x3bc);
	// The item list's mini buttons; [1] buy (buy_btn_n/o) and [2] gift
	// (gf_btn_n/o) are set by LoadShopImages and are the item list's
	// no-right-button rectangles 1 and 2 (OnItemListInit). The gift button is
	// inactive while the Rental side tab is selected (OnSideTabRentalUp).
	miniBtn_t m_miniBtns[7];  // 0x3bc
	std::map<unsigned long, const Bitmap *> m_itemIcons; // 0x480, icon cache by type id (GetItemIcon)
	std::map<unsigned int, WOverlay *> m_cardImgs;  // 0x48c, card picture cache by type id (GetCardImg)
	std::map<unsigned int, WOverlay *> m_cutinImgs; // 0x498, cut-in picture cache by type id (GetCutinImg)
	PADDING(0x4a4, 0x4b0);
	bool m_selItemExpanded;   // 0x4b0, toggled by OnSelItemUp
	PADDING(0x4b1, 0x4b4);
	const char *m_charLevels;          // 0x4b4, the shown character's levels (SetLevelBar_Char: GetCharLevel)
	const unsigned long *m_charParts;  // 0x4b8, the shown character's parts (SetLevelBar_Char: GetItemLevel)
	unsigned long m_charPartIds[24];   // 0x4bc, the item id worn in each of m_charParts' slots (HandleMsg 0x6A)
	const unsigned long *m_charCards;  // 0x51c, the shown character's cards (SetLevelBar_Char: GetItemLevel)
	unsigned char m_hairColor;         // 0x520, passed to CPetFrame::SetHairClr (ClearCartList)
	PADDING(0x521, 0x524);
	unsigned long m_standPetTypeId; // 0x524, the shown character's type id, loaded on a side tab switch (OnSideTabBuyUp, ChangeSkin)
	unsigned long m_caddieTypeId;  // 0x528, shown on the display stand (OnSelItemIconOwnerDraw)
	unsigned long m_mascotTypeId;  // 0x52c, shown on the display stand (ChangeMascot)
	unsigned long m_aztecTypeId;   // 0x530, shown on the display stand (ChangeAztec)
	unsigned long m_clubsetTypeId; // 0x534, shown on the display stand (ChangeClubset)
	statTab_t m_statTab;      // 0x538
	int m_shopTab;            // 0x53c, the side tabs apply only while it is 0 (IsSelectedRentalTab)
	subTab_t m_subTab;        // 0x540
	float m_blinkValue;       // 0x544, |sin(m_blinkTime * 4)| * 5 (OnProcess)
	float m_blinkTime;        // 0x548, grows by the frame time (OnProcess)
	WPoint m_itemDescOffset;  // 0x54c, depends on the sub tab (SetItemDescRect)
	WPoint m_itemDescPos;     // 0x554, below the description box (OnItemDescInit)
	int m_statPartKind;       // 0x55c, selects the m_statPartBitmap triple

	CExhibition *m_petExhibition; // 0x560, the shown pet model (HidePet)
	CAnotherHand m_anotherHand;   // 0x564, loads the pet model off the main thread
	std::list<unsigned long> m_rbuttonPartTypeIds; // 0x6ec, cart parts right-clicked (OnPartCartListOwnerDraw push_back)
	// A part the shopper wore before trying an item on
	// (PutOnNBackupShoppersOldItem, RestoreShoppersOldItem).
	struct oldItem_t {
		unsigned long typeId;      // 0x0, the item it was replaced by
		unsigned long tid;         // 0x4
		unsigned long id;          // 0x8
	};
	std::list<oldItem_t> m_shoppersOldItems; // 0x6f8, parts to put back on the pet (RestoreShoppersOldItem)
	unsigned long m_equipSkin[6]; // 0x704, copy of CSharedDoc::m_equipSkin taken by ChangeSkin
	float m_sidePos[2];       // 0x71c, slide of the two side areas (OnProcess)
	int m_sideDir[2];         // 0x724, 0 hold at zero, 1 slide back, 2 slide out (OnProcess)
	unsigned long m_cardTypeId; // 0x72c, the card (or set item holding one) whose picture DrawCard shows; cleared before LoadCardImages (ctor)
	WOverlay *m_cardCharacterEmpty; // 0x730, card_character_empty.tga (LoadCardImages)
	WOverlay *m_cardCaddieEmpty;    // 0x734, card_caddie_empty.tga (LoadCardImages)
	FrComboBox *m_sortOptionCombo;     // 0x738 (OnSortOptionComboInit)
	FrTextButton *m_searchGoodsButton; // 0x73c (OnSearchGoodsButtonInit)
};
CHECK_SIZE(CShopMain, 0x740);

BEGIN_LAYOUT_CHECK(CShopMain)
	CHECK_FIELD(CShopMain, m_hintWnd, 0x1E4);
	CHECK_FIELD(CShopMain, m_hintTime, 0x1E8);
	CHECK_FIELD(CShopMain, m_blinkValue, 0x544);
	CHECK_FIELD(CShopMain, m_blinkTime, 0x548);
	CHECK_FIELD(CShopMain, m_sidePos, 0x71C);
	CHECK_FIELD(CShopMain, m_sideDir, 0x724);
	CHECK_FIELD(CShopMain, m_selItem, 0x234);
	CHECK_FIELD(CShopMain, m_statBox, 0x238);
	CHECK_FIELD(CShopMain, m_selItemPrev, 0x268);
	CHECK_FIELD(CShopMain, m_selItemNext, 0x26c);
	CHECK_FIELD(CShopMain, m_scrollUp, 0x270);
	CHECK_FIELD(CShopMain, m_scrollDown, 0x274);
	CHECK_FIELD(CShopMain, m_sideTabBuy, 0x298);
	CHECK_FIELD(CShopMain, m_sideTabRental, 0x29c);
	CHECK_FIELD(CShopMain, m_buyDlg, 0x2a0);
	CHECK_FIELD(CShopMain, m_selItemExpanded, 0x4b0);
	CHECK_FIELD(CShopMain, m_statTab, 0x538);
	CHECK_FIELD(CShopMain, m_subTab, 0x540);
	CHECK_FIELD(CShopMain, m_petExhibition, 0x560);
	CHECK_FIELD(CShopMain, m_anotherHand, 0x564);
	CHECK_FIELD(CShopMain, m_shoppersOldItems, 0x6F8);
	CHECK_SIZE(CShopMain::oldItem_t, 0xC);
	CHECK_SIZE(CShopMain::miniBtn_t, 0x1C);
	CHECK_FIELD(CShopMain::miniBtn_t, rect, 0x00);
	CHECK_FIELD(CShopMain::miniBtn_t, normal, 0x10);
	CHECK_FIELD(CShopMain::miniBtn_t, over, 0x14);
	CHECK_FIELD(CShopMain::miniBtn_t, enable, 0x18);
	CHECK_FIELD(CShopMain, m_tabSubButton, 0x204);
	CHECK_FIELD(CShopMain, m_itemListBox, 0x210);
	CHECK_FIELD(CShopMain, m_cartList, 0x214);
	CHECK_FIELD(CShopMain, m_auxCartList, 0x218);
	CHECK_FIELD(CShopMain, m_shopTab, 0x53c);
	CHECK_FIELD(CShopMain, m_petArea, 0x22c);
	CHECK_FIELD(CShopMain, m_skinPet, 0x280);
	CHECK_FIELD(CShopMain, m_skinPet2, 0x284);
	CHECK_FIELD(CShopMain, m_pet, 0x288);
	CHECK_FIELD(CShopMain, m_statPart, 0x1F8);
	CHECK_FIELD(CShopMain, m_statAll, 0x1FC);
	CHECK_FIELD(CShopMain, m_statPartBitmap, 0x338);
	CHECK_FIELD(CShopMain, m_statAllBitmap, 0x35C);
	CHECK_FIELD(CShopMain, m_statPartKind, 0x55C);
	CHECK_FIELD(CShopMain, m_mascotTypeId, 0x52C);
	CHECK_FIELD(CShopMain, m_aztecTypeId, 0x530);
	CHECK_FIELD(CShopMain, m_clubsetTypeId, 0x534);
	CHECK_FIELD(CShopMain, m_selectedName, 0x1EC);
	CHECK_FIELD(CShopMain, m_statFrame, 0x1F0);
	CHECK_FIELD(CShopMain, m_statText, 0x1F4);
	CHECK_FIELD(CShopMain, m_tabTxt1, 0x21C);
	CHECK_FIELD(CShopMain, m_tabTxt2, 0x220);
	CHECK_FIELD(CShopMain, m_tabTxt3, 0x224);
	CHECK_FIELD(CShopMain, m_selItemIcon, 0x230);
	CHECK_FIELD(CShopMain, m_selFrame, 0x23C);
	CHECK_FIELD(CShopMain, m_statBar, 0x240);
	CHECK_FIELD(CShopMain, m_statEdit, 0x254);
	CHECK_FIELD(CShopMain, m_itemDescBg, 0x278);
	CHECK_FIELD(CShopMain, m_itemSlotFrame1, 0x28C);
	CHECK_FIELD(CShopMain, m_itemSlotFrame2, 0x290);
	CHECK_FIELD(CShopMain, m_itemSlot, 0x294);
	CHECK_FIELD(CShopMain, m_buyAll, 0x208);
	CHECK_FIELD(CShopMain, m_miniBtns, 0x3BC);
	CHECK_FIELD(CShopMain, m_itemIcons, 0x480);
	CHECK_FIELD(CShopMain, m_cardImgs, 0x48C);
	CHECK_FIELD(CShopMain, m_cutinImgs, 0x498);
	CHECK_FIELD(CShopMain, m_standPetTypeId, 0x524);
	CHECK_FIELD(CShopMain, m_buyReset, 0x20C);
	CHECK_FIELD(CShopMain, m_itemDesc, 0x27C);
	CHECK_FIELD(CShopMain, m_upgradeCaddies, 0x2A8);
	CHECK_FIELD(CShopMain, m_itemDescOffset, 0x54C);
	CHECK_FIELD(CShopMain, m_itemDescPos, 0x554);
	CHECK_FIELD(CShopMain, m_sortOptionCombo, 0x738);
	CHECK_FIELD(CShopMain, m_searchGoodsButton, 0x73C);
	CHECK_FIELD(CShopMain, m_caddieTypeId, 0x528);
	CHECK_FIELD(CShopMain, m_charLevels, 0x4B4);
	CHECK_FIELD(CShopMain, m_charParts, 0x4B8);
	CHECK_FIELD(CShopMain, m_charPartIds, 0x4BC);
	CHECK_FIELD(CShopMain, m_charCards, 0x51C);
	CHECK_FIELD(CShopMain, m_hairColor, 0x520);
	CHECK_FIELD(CShopMain, m_rbuttonPartTypeIds, 0x6EC);
	CHECK_FIELD(CShopMain, m_tabMainButton, 0x200);
	CHECK_FIELD(CShopMain, m_shopControl228, 0x228);
	CHECK_FIELD(CShopMain, m_itemBaseNormal, 0x2C0);
	CHECK_FIELD(CShopMain, m_itemBaseAward, 0x2D0);
	CHECK_FIELD(CShopMain, m_markNew, 0x2DC);
	CHECK_FIELD(CShopMain, m_markBegin, 0x2E8);
	CHECK_FIELD(CShopMain, m_dotBitmaps, 0x2F8);
	CHECK_FIELD(CShopMain, m_selectUp, 0x320);
	CHECK_FIELD(CShopMain, m_selectCursor0, 0x334);
	CHECK_FIELD(CShopMain, m_sicons, 0x368);
	CHECK_FIELD(CShopMain, m_itFa02, 0x37C);
	CHECK_FIELD(CShopMain, m_cancelBtnOver, 0x384);
	CHECK_FIELD(CShopMain, m_iconSex, 0x38C);
	CHECK_FIELD(CShopMain, m_selectionArrow, 0x3A0);
	CHECK_FIELD(CShopMain, m_itemDescBitmap, 0x3A4);
	CHECK_FIELD(CShopMain, m_equipSkin, 0x704);
END_LAYOUT_CHECK

#endif // PG_CLIENT_PROJECTG_SHOPMAIN_H
