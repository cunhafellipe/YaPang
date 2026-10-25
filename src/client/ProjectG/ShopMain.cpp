// Original: d:\build\custom\temp\code\client\projectg\shopmain.cpp
#include <stdio.h>

#include "ShopMain.h"
#include "TaskMain.h"
#include "RTTI.h"
#include "frscrollbar.h"
#include "UccManager.h"
#include "SharedDoc.h"
#include "frlistbox.h"
#include "Packet.h"
#include "ObjectFactory.h"
#include "actor.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "frarea.h"
#include "frform.h"
#include "RepayDlg.h"
#include "partsinfo.h"
#include "fredit.h"
#include "frcombobox.h"
#include "frtextbutton.h"
#include "frdesktop.h"
#include "woverlay.h"
#include "bitmap.h"
#include "wresrcmng.h"
#include "wview.h"
#include "CaddieInfoDlg.h"
#include "GiftDlg.h"
#include "UpgradeCaddieDlg.h"
#include "commonutil.h"
#include "mascot.h"
#include "ItemManager.h"
#include "RankingDlg.h"
#include "user_info.h"
#include "s5/Utilities.h"
#include "messengerdlg.h"
#include "CheckoutDlg.h"
#include "SetItemDlg.h"
#include "InputManager.h"
#include "TaskManager.h"
#include "globalgamedefine.h"
#include "LobbyTask.h"
#include "MascotInfo.h"
#include "cardsystem.h"
#include "UccLibrary.h"
#include "Localize.h"
#include "SoundManager.h"
#include "RealMyRoomData.h"
#include "TitleManager.h"
#include "Titles_Client.h"
#include "ToolTip.h"
#include <math.h>

// shopmain.h: no line record (empty constructor of a nested helper struct)
CShopMain::miniBtn_t::miniBtn_t(void) {
}

bool SpecialHitNewCompare(const void *lhs, const void *rhs);
bool PartCompare(const void *lhs, const void *rhs);
bool IsAngelWing(unsigned long typeId);
bool IsMannerPlayer(void);
void SetSubTabImg(const char *imgs[], int index);

// The ability pictures LoadShopImages registers: the bar and its slot, per
// stat (power, control, accuracy, spin, curve).
const char *dot_files[5][2] = {
	{ "ab_power", "ab_power_slot" },
	{ "ab_control", "ab_control_slot" },
	{ "ab_accuracy", "ab_accuracy_slot" },
	{ "ab_spin", "ab_spin_slot" },
	{ "ab_curve", "ab_curve_slot" },
};

#include <algorithm>

// wtemplate.h:460-465 (not yet in src/client/ProjectG/wtemplate.h): a list
// finder by the element's m_guid (same as CardBoxDlg.cpp's copy).
struct _FindByUid {
	unsigned long m_uid;

	_FindByUid(unsigned long uid) : m_uid(uid) {}
	template <class T>
	bool operator()(const T& item) const { return m_uid == item.m_guid; }
};

// shopmain.cpp:62
void AddSellMiniButton(IFF_ITEM_COMMON &item, FrListItem *&listItem) {
	if ((item.m_flags0 >> 5) == 1 || (item.m_flags0 >> 5) == 3)
		listItem->noRBtnIdxList.push_back(1);
	if ((item.m_flags0 >> 5) == 1 || (item.m_flags0 >> 5) == 2)
		listItem->noRBtnIdxList.push_back(2);
}

// shopmain.cpp:170 (run-time type of CShopMain)
const WRTTI CShopMain::m_RTTI("CShopMain", &CTaskMain::m_RTTI);

IObject* CShopMainMakeInstance(void) {
	return new CShopMain;
}

struct __impCShopMain {
	__impCShopMain(void) {
		ObjectFactory().AddObjectFunctor(CShopMainMakeInstance, "CShopMain");
		ActorFactory().AddActorClass("CShopMain");
	}
};
static __impCShopMain s_impCShopMain;

// shopmain.cpp:172
const sFRESH_MSGMAP *CShopMain::GetMessageMap(void) const {
	return &_MsgMap;
}

sFRESH_MSGMAP CShopMain::_MsgMap = { &CTaskMain::_MsgMap, &CShopMain::_MsgEntries[0] };

// shopmain.cpp:175
sFRESH_ENTRY CShopMain::_MsgEntries[] = {
	{ "SHOPMAIN", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnShopMainInit },
	{ "SHOPMAIN", FRCMD_FINISH, FrFuncVV, (FRESH_PMSG)&CShopMain::OnShopMainFinish },
	{ "SHOPMAIN.obar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_Init },
	{ "SHOPMAIN.utab", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderTab_Init },
	{ "SHOPMAIN.ubar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBarInit },
	{ "SHOPMAIN.shopview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnSortOptionComboInit },
	{ "SHOPMAIN.shopview", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CShopMain::OnSortOptionComboDown },
	{ "SHOPMAIN.searchgoods", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnSearchGoodsButtonInit },
	{ "SHOPMAIN.searchgoods", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnSearchGoodsLButtonDown },
	{ "SHOPMAIN.item_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnItemListInit },
	{ "SHOPMAIN.item_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnItemListOwnerDraw },
	{ "SHOPMAIN.item_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CShopMain::OnItemListBtnDown },
	{ "SHOPMAIN.item_list", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CShopMain::OnItemListRBtnDown },
	{ "SHOPMAIN.tab_main", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnTabMainInit },
	{ "SHOPMAIN.tab_main", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnTabMainButtonUp },
	{ "SHOPMAIN.tab_sub", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnTabSubInit },
	{ "SHOPMAIN.tab_sub", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnTabSubButtonUp },
	{ "SHOPMAIN.buy_all", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBuyAllInit },
	{ "SHOPMAIN.buy_all", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnBuyAllUp },
	{ "SHOPMAIN.buy_all", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBuyHoverOn },
	{ "SHOPMAIN.buy_all", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBuyHoverOff },
	{ "SHOPMAIN.buy_reset", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBuyResetInit },
	{ "SHOPMAIN.buy_reset", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnBuyResetUp },
	{ "SHOPMAIN.buy_reset", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBuyHoverOn },
	{ "SHOPMAIN.buy_reset", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBuyHoverOff },
	{ "SHOPMAIN.sidetab_buy", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnSideTabBuyInit },
	{ "SHOPMAIN.sidetab_buy", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CShopMain::OnSideTabBuyUp },
	{ "SHOPMAIN.sidetab_rental", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnSideTabRentalInit },
	{ "SHOPMAIN.sidetab_rental", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CShopMain::OnSideTabRentalUp },
	{ "SHOPMAIN.tabtxt1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnTabTxt1Init },
	{ "SHOPMAIN.tabtxt2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnTabTxt2Init },
	{ "SHOPMAIN.tabtxt2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnTabTxt3Init },
	{ "SHOPMAIN.st.selected_name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnSelectedNameInit },
	{ "SHOPMAIN.pet", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnPetInit },
	{ "SHOPMAIN.pet", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnPetOwnerDraw },
	{ "SHOPMAIN.skin_pet", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnSkinPetInit },
	{ "SHOPMAIN.skin_pet2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnSkinPet2Init },
	{ "SHOPMAIN.item_desc_bg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnItemDescBgInit },
	{ "SHOPMAIN.item_desc_bg", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnItemDescBgOwnerDraw },
	{ "SHOPMAIN.item_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnItemDescInit },
	{ "SHOPMAIN.part_cart", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnPartCartListInit },
	{ "SHOPMAIN.part_cart", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnPartCartListOwnerDraw },
	{ "SHOPMAIN.auxpart_cart", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnAuxPartCartListInit },
	{ "SHOPMAIN.auxpart_cart", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnAuxPartCartListOwnerDraw },
	{ "SHOPMAIN.scroll_up", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnScrollUpInit },
	{ "SHOPMAIN.scroll_up", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnScrollUpUp },
	{ "SHOPMAIN.scroll_down", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnScrollDownInit },
	{ "SHOPMAIN.scroll_down", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnScrollDownUp },
	{ "SHOPMAIN.item_slot_frame1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnItemSlotFrame1Init },
	{ "SHOPMAIN.item_slot_frame2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnItemSlotFrame2Init },
	{ "SHOPMAIN.item_slot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnItemSlotInit },
	{ "SHOPMAIN.item_slot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnItemSlotOwnerDraw },
	{ "SHOPMAIN.st", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnStatBoxInit },
	{ "SHOPMAIN.st.selframe", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnSelFrameInit },
	{ "SHOPMAIN.st.selitemicon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnSelItemIconInit },
	{ "SHOPMAIN.st.selitem", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnSelItemInit },
	{ "SHOPMAIN.st.selitem", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnSelItemUp },
	{ "SHOPMAIN.st.selitem", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnSelItemOwnerDraw },
	{ "SHOPMAIN.st.selitemicon", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnSelItemIconOwnerDraw },
	{ "SHOPMAIN.st.selitem_prev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnSelItemPrevInit },
	{ "SHOPMAIN.st.selitem_prev", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnSelItemPrevUp },
	{ "SHOPMAIN.st.selitem_next", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnSelItemNextInit },
	{ "SHOPMAIN.st.selitem_next", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnSelItemNextUp },
	{ "SHOPMAIN.st.stat_frame", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnStatFrameInit },
	{ "SHOPMAIN.st.stat_text", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnStatTextInit },
	{ "SHOPMAIN.st.stat_part", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnStatPartInit },
	{ "SHOPMAIN.st.stat_part", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnStatPartUp },
	{ "SHOPMAIN.st.stat_part", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnStatPartOwnerDraw },
	{ "SHOPMAIN.st.stat_all", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnStatAllInit },
	{ "SHOPMAIN.st.stat_all", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnStatAllUp },
	{ "SHOPMAIN.st.stat_all", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnStatAllOwnerDraw },
	{ "SHOPMAIN.st.powerbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnPowerBarInit },
	{ "SHOPMAIN.st.controlbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnControlBarInit },
	{ "SHOPMAIN.st.impactbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnImpactBarInit },
	{ "SHOPMAIN.st.spinbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnSpinBarInit },
	{ "SHOPMAIN.st.curvebar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnCurveBarInit },
	{ "SHOPMAIN.st.epower", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnPowerEditInit },
	{ "SHOPMAIN.st.econtrol", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnControlEditInit },
	{ "SHOPMAIN.st.eimpact", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnImpactEditInit },
	{ "SHOPMAIN.st.espin", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnSpinEditInit },
	{ "SHOPMAIN.st.ecurve", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnCurveEditInit },
	{ "SHOPMAIN.ubar.back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBar_BackInit },
	{ "SHOPMAIN.ubar.back", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnUnderBar_BackUp },
	{ "SHOPMAIN.ubar.back", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.ubar.back", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.ubar.front", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBar_FrontInit },
	{ "SHOPMAIN.ubar.front", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnUnderBar_FrontUp },
	{ "SHOPMAIN.ubar.front", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.ubar.front", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.ubar.shop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBar_ShopInit },
	{ "SHOPMAIN.ubar.shop", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ShopUp },
	{ "SHOPMAIN.ubar.shop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.ubar.shop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.ubar.myroom", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBar_MyRoomInit },
	{ "SHOPMAIN.ubar.myroom", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MyRoomUp },
	{ "SHOPMAIN.ubar.myroom", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.ubar.myroom", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.ubar.itemstorage", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBar_ItemStorageInit },
	{ "SHOPMAIN.ubar.itemstorage", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnUnderBar_ItemStorageUp },
	{ "SHOPMAIN.ubar.itemstorage", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.ubar.itemstorage", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.ubar.gift", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBar_GiftInit },
	{ "SHOPMAIN.ubar.gift", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnUnderBar_GiftUp },
	{ "SHOPMAIN.ubar.gift", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.ubar.gift", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.ubar.cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBar_CookieInit },
	{ "SHOPMAIN.ubar.cookie", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_CookieUp },
	{ "SHOPMAIN.ubar.cookie", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.ubar.cookie", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.ubar.game", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBar_GameInit },
	{ "SHOPMAIN.ubar.game", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnUnderBar_GameUp },
	{ "SHOPMAIN.ubar.game", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.ubar.game", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.ubar.server", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBar_ServerInit },
	{ "SHOPMAIN.ubar.server", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnUnderBar_ServerUp },
	{ "SHOPMAIN.ubar.server", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.ubar.server", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.ubar.exit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBar_ExitInit },
	{ "SHOPMAIN.ubar.exit", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnUnderBar_ExitUp },
	{ "SHOPMAIN.ubar.exit", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.ubar.exit", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.ubar.tabbtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBar_TabBtnInit },
	{ "SHOPMAIN.ubar.tabbtn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnUnderBar_TabBtnUp },
	{ "SHOPMAIN.utab.magicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBar_MagicBoxInit },
	{ "SHOPMAIN.utab.magicbox", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnUnderBar_MagicBoxUp },
	{ "SHOPMAIN.utab.magicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.utab.magicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.utab.tikimagicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBar_TikiMagicBoxInit },
	{ "SHOPMAIN.utab.tikimagicbox", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnUnderBar_TikiMagicBoxUp },
	{ "SHOPMAIN.utab.tikimagicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.utab.tikimagicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.utab.scratch", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBar_ScratchInit },
	{ "SHOPMAIN.utab.scratch", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnUnderBar_ScratchUp },
	{ "SHOPMAIN.utab.scratch", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.utab.scratch", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.utab.bongdarishop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBar_BongdariShopInit },
	{ "SHOPMAIN.utab.bongdarishop", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CShopMain::OnUnderBar_BongdariShopUp },
	{ "SHOPMAIN.utab.bongdarishop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.utab.bongdarishop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.utab.ranking", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBar_RankingInit },
	{ "SHOPMAIN.utab.ranking", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnUnderBar_RankingUp },
	{ "SHOPMAIN.utab.ranking", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.utab.ranking", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.utab.guild", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBar_GuildInit },
	{ "SHOPMAIN.utab.guild", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GuildUp },
	{ "SHOPMAIN.utab.guild", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.utab.guild", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.utab.notice", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBar_NoticeInit },
	{ "SHOPMAIN.utab.notice", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnUnderBar_NoticeUp },
	{ "SHOPMAIN.utab.notice", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.utab.notice", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.utab.maket", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnUnderBar_MaketInit },
	{ "SHOPMAIN.utab.maket", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnUnderBar_MaketUp },
	{ "SHOPMAIN.utab.maket", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.utab.maket", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.obar.title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_TitleInit },
	{ "SHOPMAIN.obar.site", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_SiteInit },
	{ "SHOPMAIN.obar.char", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_CharInit },
	{ "SHOPMAIN.obar.char", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CShopMain::OnOverBar_CharDown },
	{ "SHOPMAIN.obar.char", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_CharOwnerDraw },
	{ "SHOPMAIN.obar.caddie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_CaddieInit },
	{ "SHOPMAIN.obar.caddie", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CShopMain::OnOverBar_CaddieDown },
	{ "SHOPMAIN.obar.caddie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_CaddieOwnerDraw },
	{ "SHOPMAIN.obar.club", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_ClubInit },
	{ "SHOPMAIN.obar.club", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CShopMain::OnOverBar_ClubDown },
	{ "SHOPMAIN.obar.club", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_ClubOwnerDraw },
	{ "SHOPMAIN.obar.aztec", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_AztecInit },
	{ "SHOPMAIN.obar.aztec", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CShopMain::OnOverBar_AztecDown },
	{ "SHOPMAIN.obar.aztec", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_AztecOwnerDraw },
	{ "SHOPMAIN.obar.mascot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_MascotInit },
	{ "SHOPMAIN.obar.mascot", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CShopMain::OnOverBar_MascotDown },
	{ "SHOPMAIN.obar.mascot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_MascotOwnerDraw },
	{ "SHOPMAIN.obar.char_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_CharSelInit },
	{ "SHOPMAIN.obar.char_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_CharSelOwnerDraw },
	{ "SHOPMAIN.obar.caddie_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_CaddieSelInit },
	{ "SHOPMAIN.obar.caddie_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_CaddieSelOwnerDraw },
	{ "SHOPMAIN.obar.club_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_ClubSelInit },
	{ "SHOPMAIN.obar.club_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_ClubSelOwnerDraw },
	{ "SHOPMAIN.obar.aztec_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_AztecSelInit },
	{ "SHOPMAIN.obar.aztec_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_AztecSelOwnerDraw },
	{ "SHOPMAIN.obar.mascot_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_MascotSelInit },
	{ "SHOPMAIN.obar.mascot_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_MascotSelOwnerDraw },
	{ "SHOPMAIN.obar.pang_cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_PangCookieInit },
	{ "SHOPMAIN.obar.pang_cookie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_PangCookieOwnerDraw },
	{ "SHOPMAIN.obar.onelinemsg_req", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_OneLineReqBtnInit },
	{ "SHOPMAIN.obar.onelinemsg_req", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnOverBar_OneLineReqBtnUp },
	{ "SHOPMAIN.obar.onelineview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_OneLineViewInit },
	{ "SHOPMAIN.obar.onelineview", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_OneLineViewOwnerDraw },
	{ "SHOPMAIN.obar.spcard", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_SpecialCardInit },
	{ "SHOPMAIN.obar.spcard", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnOverBar_SpecialCardUp },
	{ "SHOPMAIN.obar.spcard", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.obar.spcard", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.obar.messenger", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_MessengerInit },
	{ "SHOPMAIN.obar.messenger", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnOverBar_MessengerUp },
	{ "SHOPMAIN.obar.messenger", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.obar.messenger", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.obar.myinfo", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_MyInfoInit },
	{ "SHOPMAIN.obar.myinfo", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnOverBar_MyInfoUp },
	{ "SHOPMAIN.obar.myinfo", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.obar.myinfo", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.obar.option", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnOverBar_OptionInit },
	{ "SHOPMAIN.obar.option", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnOverBar_OptionUp },
	{ "SHOPMAIN.obar.option", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOn },
	{ "SHOPMAIN.obar.option", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarHoverOff },
	{ "SHOPMAIN.obar.messenger_bubble", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnMessengerBubbleInit },
	{ "SHOPMAIN.obar.messenger_bubble", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnMessengerBubbleOwnerDraw },
	{ "SHOPMAIN.obar.messenger_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnMessengerBubbleEditInit },
	{ "SHOPMAIN.obar.messenger_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnMessengerAlarmBtnInit },
	{ "SHOPMAIN.obar.messenger_alarm", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnMessengerAlarmBtnUp },
	{ "SHOPMAIN.obar.gift_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnMessengerGiftAlarmBtnInit },
	{ "SHOPMAIN.obar.gift_alarm", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CShopMain::OnMessengerGiftAlarmBtnUp },
	{ "SHOPMAIN.obar.tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CShopMain::OnBarTooltipOwnerDraw },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// shopmain.cpp:477
void CShopMain::OnShopMainInit(PG_MSGARG) {
	SetTimeVariableBGM();
	TurnOff3DBackground();
}

// shopmain.cpp:483
void CShopMain::OnShopMainFinish(void) {
	m_titleArea->SetBgImg("title_shop");

	m_petArea = m_pet;

	CSharedDoc *doc = WSingleton<CSharedDoc>::Instance();
	unsigned long caddieTypeId = doc->m_myCaddieId == 0 ? 0x1C000000 : doc->m_caddieMap[doc->m_myCaddieId].typeId;
	unsigned long mascotTypeId = WSingleton<CSharedDoc>::Instance()->m_myMascotId == 0 ? 0x40000000 : WSingleton<CSharedDoc>::Instance()->m_myMascotMap[WSingleton<CSharedDoc>::Instance()->m_myMascotId].typeId;

	ChangeCharacter(WSingleton<CSharedDoc>::Instance()->m_characterMap[WSingleton<CSharedDoc>::Instance()->m_myCharacterId].typeId);

	ChangeCaddie(caddieTypeId);
	ChangeClubset(WSingleton<CSharedDoc>::Instance()->m_myItemMap[WSingleton<CSharedDoc>::Instance()->m_myClubId].m_typeId);
	ChangeAztec(WSingleton<CSharedDoc>::Instance()->m_myBallId);
	ChangeMascot(mascotTypeId);
	memcpy(m_equipSkin, WSingleton<CSharedDoc>::Instance()->m_equipSkin, sizeof(m_equipSkin));

	ChangeMainTab((mainTab_t)m_shopTab, true);
	ChangeSubTab(m_subTab, true);
	m_statTab = (statTab_t)(m_shopTab > 2);

	memcpy(m_equipSkin, WSingleton<CSharedDoc>::Instance()->m_equipSkin, sizeof(m_equipSkin));
	for (int i = 0; i < 6; i++) {
		if (m_equipSkin[i]) {
			IFF_STRUCT::sSkin *skin = ItemManager()->FindSkin(m_equipSkin[i]);
			if (skin && !(m_equipSkin[i] & 0x3C00000)) {
				m_sideDir[0] = skin->m_sideSlideMode[0];
				m_sideDir[1] = skin->m_sideSlideMode[1];
				m_sidePos[0] = 0;
				m_sidePos[1] = 0;
			}
		}
	}

	if (WSingleton<CSharedDoc>::Instance()->m_tutorialFinish) {
		this << MsgObject(0, 0xAA, 1, 0, 0, 0, 0);
	}
}

// shopmain.cpp:624
void CShopMain::OnSortOptionComboInit(PG_MSGARG param) {
	FrComboBox *wnd = DYNAMIC_CAST(FrComboBox, param);
	m_sortOptionCombo = wnd;
	if (wnd) {
		wnd->SetVisible(false); wnd->Enable(false);
	}
}

// shopmain.cpp:633
void CShopMain::OnSortOptionComboDown(void) {
}

// shopmain.cpp:638
void CShopMain::OnSearchGoodsButtonInit(PG_MSGARG param) {
	FrTextButton *wnd = DYNAMIC_CAST(FrTextButton, param);
	m_searchGoodsButton = wnd;
	if (wnd) {
		wnd->SetVisible(false); wnd->Enable(false);
	}
}

// shopmain.cpp:647
void CShopMain::OnSearchGoodsLButtonDown(void) {
}

// shopmain.cpp:699
void CShopMain::InitShopDialogs(void) {
	m_buyDlg = NULL;
	m_optionDlg = NULL;
	m_kbdManualDlg = NULL;
	m_serverDlg = NULL;
	m_giftDlg = NULL;
	m_waitDlg = NULL;
	m_notifyOneDlg = NULL;
	m_waitDlgTime = -10.0f;
}

// shopmain.cpp:712
void CShopMain::InitShopControls(void) {
	m_selectedName = NULL; m_statFrame = NULL; m_statText = NULL; m_statPart = NULL; m_statAll = NULL; m_tabMainButton = NULL; m_tabSubButton = NULL; m_buyAll = NULL; m_buyReset = NULL; m_itemListBox = NULL; m_cartList = NULL; m_auxCartList = NULL; m_tabTxt1 = NULL; m_tabTxt2 = NULL; m_tabTxt3 = NULL; m_shopControl228 = NULL; m_petArea = NULL; m_petExhibition = NULL; m_selItemIcon = NULL; m_statBox = NULL; m_selItem = NULL; m_selFrame = NULL; memset(m_statBar, 0, sizeof(m_statBar));
	memset(m_statEdit, 0, sizeof(m_statEdit));
	m_selItemPrev = NULL;
	m_selItemNext = NULL;
	m_hintWnd = NULL;
	m_hintTime = -10.0f;
	memset(m_equipSkin, 0, sizeof(m_equipSkin));
	memset(m_sideDir, 0, sizeof(m_sideDir));
	memset(m_sidePos, 0, sizeof(m_sidePos));
	m_pet = NULL;
	m_skinPet = NULL;
	m_itemSlotFrame1 = NULL;
	m_itemSlotFrame2 = NULL;
	m_statPartKind = 0;

	m_selectionArrow = NULL;
	m_blinkValue = 0.0f;
	m_blinkTime = 0.0f;

	memset(m_charPartIds, 0, sizeof(m_charPartIds));

	m_markBegin = NULL;
}

// shopmain.cpp:761
void CShopMain::CloseAllDialogs(void) {
	FrWnd *desktop = g_pFresh->GetManager()->GetDesktop();
	FrWnd *shop = DYNAMIC_CAST(FrWnd, desktop->FindChildByName("bongdari_shop"));
	if (shop)
		shop->Close(true);
}

// shopmain.cpp:770
void CShopMain::TurnOnItemStatus(void) {
	m_statBox->SetVisible(true);
	SetLevelBar();
}

// shopmain.cpp:776
void CShopMain::TurnOffItemStatus(void) {
	m_statBox->SetVisible(false);
	SetLevelBar();
}

// shopmain.cpp:787
// Sort of the part list: displayed UCC clothes first (by rank, then type id),
// then the items on sale, the new and hot ones, the parts by slot, the rest
// by type id descending.
bool PartCompare(const void *lhs, const void *rhs) {
	const IFF_ITEM_COMMON *left = (const IFF_ITEM_COMMON *)lhs;
	const IFF_ITEM_COMMON *right = (const IFF_ITEM_COMMON *)rhs;

	if (left == NULL) {
		return false;
	}
	if (right == NULL) {
		return true;
	}

	if (UccManager()->IsDisplayUccClothes(left) && !UccManager()->IsDisplayUccClothes(right)) {
		return true;
	}
	if (UccManager()->IsDisplayUccClothes(right) && !UccManager()->IsDisplayUccClothes(left)) {
		return false;
	}
	if (UccManager()->IsDisplayUccClothes(left) && UccManager()->IsDisplayUccClothes(right)) {
		const sDisplayUccClothes *leftUcc = (const sDisplayUccClothes *)lhs;
		const sDisplayUccClothes *rightUcc = (const sDisplayUccClothes *)rhs;

		if (leftUcc->m_rank < rightUcc->m_rank) {
			return true;
		}
		if (leftUcc->m_rank == rightUcc->m_rank) {
			return leftUcc->m_typeId < rightUcc->m_typeId ? true : false;
		}
		if (leftUcc->m_rank > rightUcc->m_rank) {
			return false;
		}
	}

	_SYSTEMTIME *leftBegin = NULL, *leftEnd = NULL, *rightBegin = NULL, *rightEnd = NULL;
	bool leftHasPeriod = ItemManager()->HasSalePeriod(left->m_typeId, &leftBegin, &leftEnd);
	bool rightHasPeriod = ItemManager()->HasSalePeriod(right->m_typeId, &rightBegin, &rightEnd);

	if (leftHasPeriod || rightHasPeriod) {
		if (!rightHasPeriod) {
			return true;
		}
		if (!leftHasPeriod) {
			return false;
		}

		if ((leftBegin->wYear != 0 || leftEnd->wYear != 0) && rightBegin->wYear == 0 && rightEnd->wYear == 0) {
			return true;
		}
		if (leftBegin->wYear == 0 && leftEnd->wYear == 0 && (rightBegin->wYear != 0 || rightEnd->wYear != 0)) {
			return false;
		}
	}

	if ((left->m_flags1 & 1) && !(right->m_flags1 & 1)) {
		return true;
	}
	if ((right->m_flags1 & 1) && !(left->m_flags1 & 1)) {
		return false;
	}

	if (((left->m_flags1 >> 1) & 1) && !(right->m_flags1 & 2)) {
		return true;
	}
	if ((right->m_flags1 & 2) && !((left->m_flags1 >> 1) & 1)) {
		return false;
	}

	if ((left->m_typeId & 0xfc000000) == 0x8000000 && (right->m_typeId & 0xfc000000) == (left->m_typeId & 0xfc000000)) {
		const IFF_STRUCT::sPart *leftPart = (const IFF_STRUCT::sPart *)lhs;
		const IFF_STRUCT::sPart *rightPart = (const IFF_STRUCT::sPart *)rhs;

		if (leftPart->slot < rightPart->slot) {
			return true;
		}
		if (leftPart->slot > rightPart->slot) {
			return false;
		}
	}

	return right->m_typeId < left->m_typeId;
}

// shopmain.cpp:912
bool SpecialHitNewCompare(const void *lhs, const void *rhs) {
	const IFF_ITEM_COMMON *left = (const IFF_ITEM_COMMON *)lhs;
	const IFF_ITEM_COMMON *right = (const IFF_ITEM_COMMON *)rhs;

	_SYSTEMTIME *leftBegin = NULL, *leftEnd = NULL, *rightBegin = NULL, *rightEnd = NULL;
	bool leftHasPeriod = ItemManager()->HasSalePeriod(left->m_typeId, &leftBegin, &leftEnd);
	bool rightHasPeriod = ItemManager()->HasSalePeriod(right->m_typeId, &rightBegin, &rightEnd);

	if (leftHasPeriod || rightHasPeriod) {
		if (!rightHasPeriod) {
			return true;
		}
		if (!leftHasPeriod) {
			return false;
		}

		if ((leftBegin->wYear != 0 || leftEnd->wYear != 0) && rightBegin->wYear == 0 && rightEnd->wYear == 0) {
			return true;
		}
		if (leftBegin->wYear == 0 && leftEnd->wYear == 0 && (rightBegin->wYear != 0 || rightEnd->wYear != 0)) {
			return false;
		}
	}

	if ((left->m_flags1 & 1) && !(right->m_flags1 & 1)) {
		return true;
	}
	if ((right->m_flags1 & 1) && !(left->m_flags1 & 1)) {
		return false;
	}

	if (((left->m_flags1 >> 1) & 1) && !(right->m_flags1 & 2)) {
		return true;
	}
	if ((right->m_flags1 & 2) && !((left->m_flags1 >> 1) & 1)) {
		return false;
	}

	return left->m_typeId > right->m_typeId;
}

// shopmain.cpp:957
// Sort of the skin list: by type id, empty slots last.
bool SkinCompare(const void *lhs, const void *rhs) {
	const IFF_ITEM_COMMON *left = (const IFF_ITEM_COMMON *)lhs;
	const IFF_ITEM_COMMON *right = (const IFF_ITEM_COMMON *)rhs;

	if (left == NULL) {
		return false;
	}
	if (right == NULL) {
		return true;
	}

	return left->m_typeId < right->m_typeId;
}

// shopmain.cpp:974
bool ShopEmptyCompare(const void *lhs, const void *rhs) {
	if (lhs) {
		if (rhs == NULL) {
			return true;
		}
	}
	return false;
}

// shopmain.cpp:985
bool CaddieCompare(const void *lhs, const void *rhs) {
	const IFF_ITEM_COMMON *left = (const IFF_ITEM_COMMON *)lhs;
	const IFF_ITEM_COMMON *right = (const IFF_ITEM_COMMON *)rhs;

	if (left == NULL || right == NULL) {
		return false;
	}

	return left->m_typeId < right->m_typeId;
}

// shopmain.cpp:996
void CShopMain::OnPetInit(PG_MSGARG param) {
	m_pet = DYNAMIC_CAST(FrArea, param);
}

// shopmain.cpp:1001
void CShopMain::OnSkinPetInit(PG_MSGARG param) {
	m_skinPet = DYNAMIC_CAST(FrArea, param);
}

// shopmain.cpp:1006
void CShopMain::OnSkinPet2Init(PG_MSGARG param) {
	m_skinPet2 = DYNAMIC_CAST(FrArea, param);
}

// shopmain.cpp:1011
void CShopMain::DrawSkinFrame(void) {
	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	// The background skin, tiled over the pet area and scrolled by m_sidePos.
	if (m_equipSkin[0]) {
		IFF_STRUCT::sSkin *skin = ItemManager()->FindSkin(m_equipSkin[0]);
		if (skin) {
			const Bitmap *bmp = g_pFresh->GetBitmap(skin->m_cutinImage);
			if (bmp) {
				WRect rc = m_petArea->GetRect();
				float w = (float)bmp->Width();
				float h = (float)bmp->Height();

				if (m_sidePos[0] > w)
					m_sidePos[0] -= w;
				else if (m_sidePos[0] < 0.0f)
					m_sidePos[0] += w;

				if (m_sidePos[1] > h)
					m_sidePos[1] -= h;
				else if (m_sidePos[1] < 0.0f)
					m_sidePos[1] += h;

				WRect src(m_sidePos[0], m_sidePos[1], w - m_sidePos[0], h - m_sidePos[1]);
				WRect dst;
				dst.x = rc.x;
				dst.y = rc.y;
				dst.w = src.w;
				dst.h = src.h;

				float bottom = rc.Bottom();
				while (dst.y < bottom) {
					float right = rc.Right();
					while (dst.x < right) {
						pGDI->DrawTexture(bmp, src, dst, 0xFFFFFFFF, 0);

						dst.x += dst.w;
						dst.w = w > rc.Right() - dst.x ? rc.Right() - dst.x : w;

						src.x = 0.0f;
						src.w = dst.w;
					}

					dst.x = rc.x;
					dst.y += dst.h;
					dst.w = w - m_sidePos[0];
					dst.h = h > rc.Bottom() - dst.y ? rc.Bottom() - dst.y : h;

					src.x = m_sidePos[0];
					src.w = dst.w;
					src.y = 0.0f;
					src.h = dst.h;
				}
			}
		}
	}

	if (!m_anotherHand.IsWorking(0)) {
		m_anotherHand.Enter();
		if (m_petExhibition)
			m_petExhibition->Display(0.0f, 0.0f);
		m_anotherHand.Leave();
	}

	if (m_equipSkin[1]) {
		IFF_STRUCT::sSkin *skin = ItemManager()->FindSkin(m_equipSkin[1]);
		if (skin) {
			const Bitmap *bmp = g_pFresh->GetBitmap(skin->m_cutinImage);
			if (bmp)
				pGDI->DrawTexture(bmp, m_petArea->GetRect(), 0xFFFFFFFF, 0);
		}
	}

	if (m_equipSkin[2]) {
		IFF_STRUCT::sSkin *skin = ItemManager()->FindSkin(m_equipSkin[2]);
		if (skin) {
			const Bitmap *bmp = g_pFresh->GetBitmap(skin->m_cutinImage);
			if (bmp)
				pGDI->DrawTexture(bmp, m_petArea->GetRect(), 0xFFFFFFFF, 0);
		}
	}
}

// shopmain.cpp:1110
void CShopMain::DrawSkinSlot(void) {
	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	pGDI->DrawTexture(m_teamBase, WRect(116.0f, 308.0f, (float)m_teamBase->Width(), (float)m_teamBase->Height()), 0xFFFFFFFF, 0);

	if (m_equipSkin[3]) {
		IFF_STRUCT::sSkin *skin = ItemManager()->FindSkin(m_equipSkin[3]);
		if (skin) {
			const Bitmap *bmp = g_pFresh->GetBitmap(skin->m_cutinImage);
			if (bmp)
				pGDI->DrawTexture(bmp, WRect(116.0f, 308.0f, (float)bmp->Width(), (float)bmp->Height()), 0xFFFFFFFF, 0);
		}
	}

	IFF_STRUCT::sChar *character = ItemManager()->FindChar(m_standPetTypeId);
	if (character) {
		const Bitmap *bmp = g_pFresh->GetBitmap(character->m_icon);
		if (bmp)
			pGDI->DrawTexture(bmp, WRect(124.0f, 344.0f, (float)bmp->Width(), (float)bmp->Height()), 0xFFFFFFFF, 0);
	}

	if (m_equipSkin[4]) {
		IFF_STRUCT::sSkin *skin = ItemManager()->FindSkin(m_equipSkin[4]);
		if (skin) {
			const Bitmap *bmp = g_pFresh->GetBitmap(skin->m_cutinImage);
			if (bmp)
				pGDI->DrawTexture(bmp, WRect(124.0f, 344.0f, (float)bmp->Width(), (float)bmp->Height()), 0xFFFFFFFF, 0);
		}
	}

	const Bitmap *sexIcon = m_iconSex[(WSingleton<CSharedDoc>::Instance()->m_userInfo.eventFlags >> 1) & 1];
	pGDI->DrawTexture(sexIcon, WRect(121.0f, 406.0f, (float)sexIcon->Width(), (float)sexIcon->Height()), 0xFFFFFFFF, 0);

	const Bitmap *levelBmp;
	if (m_equipSkin[5]) {
		IFF_STRUCT::sSkin *skin = ItemManager()->FindSkin(m_equipSkin[5]);
		if (skin)
			levelBmp = g_pFresh->GetBitmap(skin->m_icon);
		else
			levelBmp = g_pFresh->GetBitmap(MakeStr("level_%03d", WSingleton<CSharedDoc>::Instance()->m_myLevel + 1));
	} else {
		levelBmp = g_pFresh->GetBitmap(MakeStr("level_%03d", WSingleton<CSharedDoc>::Instance()->m_myLevel + 1));
	}

	if (levelBmp)
		pGDI->DrawTexture(levelBmp, WRect(206.0f - (int)((float)levelBmp->Width() * 0.5f), 386.0f - (int)((float)levelBmp->Height() * 0.5f), (float)levelBmp->Width(), (float)levelBmp->Height()), 0xFFFFFFFF, 0);

	pGDI->DrawTexture(m_masterSign, WRect((float)(int)(179.5f - (float)m_masterSign->Width() * 0.5f), 313.0f, (float)m_masterSign->Width(), (float)m_masterSign->Height()), 0xFFFFFFFF, 0);

	pGDI->SetTextColor(0xFFFFFFFF, 0xFF808080);
	pGDI->SetTextStyle(2);
	g_pFresh->GetManager()->PrintText(WPoint(139.0f, 415.0f), 0, MyNick(), -1.0f, 0xFFFFFFFF);
}

// shopmain.cpp:1192
void CShopMain::DrawCutinFrame(void) {
	if (m_subTab == 33) {
		FrListItem *listItem = m_itemListBox->GetSelected();
		if (listItem == NULL)
			return;
		IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)listItem->pData;
		if (item == NULL)
			return;

		WOverlay *cutin = GetCutinImg(item->m_typeId);
		if (cutin == NULL)
			return;

		WRect rect;
		rect.x = 72.0f;
		rect.y = 67.0f;
		rect.w = (float)cutin->GetWidth();
		rect.h = (float)cutin->GetHeight();
		cutin->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), rect, 0, 0xFFFFFFFF, 0.0f, 0);
	}
}

// shopmain.cpp:1230
WOverlay *CShopMain::GetCutinImg(unsigned long typeId) {
	WOverlay *overlay;
	std::map<unsigned int, WOverlay *>::iterator it = m_cutinImgs.find(typeId);
	if (it == m_cutinImgs.end()) {

		IFF_STRUCT::sSkin *skin = ItemManager()->FindSkin(typeId);

		if (skin) {
			char path[64];
			sprintf(path, "%s.jpg", skin->m_cutinImage);
			overlay = g_resrcmng->GetOverlay(path, 0);
			if (overlay) {
				m_cutinImgs.insert(std::map<unsigned int, WOverlay *>::value_type(skin->m_typeId, overlay));
			}
		} else {
			return NULL;
		}
	} else {
		overlay = it->second;
	}

	return overlay;
}

// shopmain.cpp:1269
void CShopMain::DeleteCutinImages(void) {
	for (std::map<unsigned int, WOverlay *>::iterator it = m_cutinImgs.begin(); it != m_cutinImgs.end(); ++it) {

		if (g_resrcmng && it->second) { g_resrcmng->Release(it->second); it->second = 0; }
	}
}

// shopmain.cpp:1277
void CShopMain::OnPetOwnerDraw(PG_MSGARG) {
	if (m_shopTab == 4) {
		switch (m_subTab) {
		case 13:
		case 14:
		case 15:
			DrawSkinFrame();
			break;
		case 16:
		case 34:
			DrawSkinSlot();
			break;
		case 33:
			DrawCutinFrame();
			break;
		}
	} else {
		if (!m_anotherHand.IsWorking(0)) {
			if (!m_bongdariShop[2] && !WSingleton<CRankingInfo>::Instance()->GetDlg() && !USERINFODLG()) {
				m_anotherHand.Enter();
				if (m_petExhibition)
					m_petExhibition->Display(0.0f, 0.0f);
				m_anotherHand.Leave();
			}
		}
	}
}

// shopmain.cpp:1309
void CShopMain::OnItemDescBgInit(PG_MSGARG param) {
	m_itemDescBg = DYNAMIC_CAST(FrArea, param);
}

// shopmain.cpp:1316
void CShopMain::OnItemDescBgOwnerDraw(PG_MSGARG) {
	FrGraphicInterface *graphic = g_pFresh->GetManager()->GetGDI();
	if (graphic == NULL)
		return;

	int lineNum = m_itemDesc->GetLineNum();
	if (lineNum <= 0)
		return;

	float width[6], height[6]; // six slots in the original frame, two used
	for (int i = 0; i < 2; i++) {
		width[i] = (float)m_itemDescBitmap[i]->Width();
		height[i] = (float)m_itemDescBitmap[i]->Height();
	}

	float y = m_itemDescOffset.y + 320.0f;
	graphic->DrawTexture(m_itemDescBitmap[0], WRect(m_itemDescOffset.x + 6.0f, y, width[0], height[0]), 0xffffffff, 0);

	graphic->DrawTexture(m_itemDescBitmap[1], WRect(m_itemDescOffset.x + 262.0f, y, width[1], height[1]), 0xffffffff, 0);
}

// shopmain.cpp:1341
void CShopMain::OnItemDescInit(PG_MSGARG param) {
	m_itemDesc = DYNAMIC_CAST(FrEdit, param);
	if (m_itemDesc) {
		m_itemDesc->SetVisible(false);
		m_itemDescPos = WPoint(m_itemDesc->GetRect().x, m_itemDesc->GetRect().y + m_itemDesc->GetRect().h);
	}
}

// shopmain.cpp:1351
void CShopMain::SetItemDescRect(void) {
	m_itemDesc->ClearLine();

	switch (m_subTab) {
	case SUB_TAB_CLUBSET:
	case 10:
		m_itemDescOffset = WPoint(0.0f, 30.0f);
		break;
	case SUB_TAB_AZTEC:
	case SUB_TAB_MASCOT:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
		m_itemDescOffset = WPoint(0.0f, 120.0f);
		break;
	default:
		m_itemDescOffset = WPoint(0.0f, 0.0f);
		break;
	}
}

// shopmain.cpp:1377
void CShopMain::ShowItemDesc(unsigned long typeId) {
	IFF_STRUCT::sDesc *desc = ItemManager()->FindDesc(typeId);
	if (desc) {
		m_itemDesc->ClearLine();
		m_itemDesc->AddText(desc->m_text, false, true);

		int lineNum = m_itemDesc->GetLineNum();

		// Keep at most two lines.
		if (lineNum > 2) {
			int extra = lineNum - 2;
			lineNum -= extra;
			do {
				m_itemDesc->DeleteEndLine();
			} while (--extra);
		}

		float height = (float)m_itemDesc->GetLineHeight() * lineNum;
		m_itemDesc->SetRect(WRect(m_itemDescPos.x + m_itemDescOffset.x, m_itemDescPos.y + m_itemDescOffset.y - height, m_itemDesc->GetRect().w, height));

		m_itemDesc->GetScrollBar()->ScrollToFirst();
	}
}

// shopmain.cpp:1401
void CShopMain::OnPowerBarInit(intptr_t param) {
	m_statBar[0] = DYNAMIC_CAST(FrGaugeBar, param);
	m_statBar[0]->SetRange(0, 50, 0);
}

// shopmain.cpp:1407
void CShopMain::OnControlBarInit(intptr_t param) {
	m_statBar[1] = DYNAMIC_CAST(FrGaugeBar, param);
	m_statBar[1]->SetRange(0, 30, 0);
}

// shopmain.cpp:1413
void CShopMain::OnImpactBarInit(intptr_t param) {
	m_statBar[2] = DYNAMIC_CAST(FrGaugeBar, param);
	m_statBar[2]->SetRange(0, 30, 0);
}

// shopmain.cpp:1419
void CShopMain::OnSpinBarInit(intptr_t param) {
	m_statBar[3] = DYNAMIC_CAST(FrGaugeBar, param);
	m_statBar[3]->SetRange(0, 30, 0);
}

// shopmain.cpp:1425
void CShopMain::OnCurveBarInit(intptr_t param) {
	m_statBar[4] = DYNAMIC_CAST(FrGaugeBar, param);
	m_statBar[4]->SetRange(0, 30, 0);
}

// shopmain.cpp:1431
void CShopMain::DrawItemPrice(FrGraphicInterface *graphic, float x, float y, IFF_ITEM_COMMON &item, FrListItem *listItem) {
	if (listItem == NULL)
		return;

	if ((item.m_flags0 & 0xE0) == 0x80)
		return;

	if (item.m_discountPrice > 0 && item.m_discountPrice < item.m_price) {

		graphic->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
		graphic->Print11(WPoint(x, y) + listItem->pos, 2, ((item.m_flags0 & 1) ? MakeStr("%d\xC4\xED\xC5\xB0", item.m_discountPrice) : MakeStr("%d\xC6\xCE", item.m_discountPrice)));
		graphic->SetTextColor(0xFF000000, 0xFFFFFFFF);

		x -= 40.0f;

		graphic->SetTextColor(0xFF000000, 0xFFFFFFFF);
		int width = float2int(graphic->Print11(WPoint(x, y) + listItem->pos, 2, ((item.m_flags0 & 1) ? MakeStr("%d\xC4\xED\xC5\xB0", item.m_price) : MakeStr("%d\xC6\xCE", item.m_price))));
		graphic->SetTextColor(0xFF000000, 0xFFFFFFFF);

		g_view->DrawLine2D(WPoint(x - width - 1.0f, y + float2int(graphic->GetFontHeight11() * 0.5f)) + listItem->pos,
			WPoint(x + 1.0f, y + float2int(graphic->GetFontHeight11() * 0.5f)) + listItem->pos, 0xFF000000, 0xFF000000, 0);
	} else if (item.m_price) {
		graphic->Print11(WPoint(x, y) + listItem->pos, 2, ((item.m_flags0 & 1) ? MakeStr("%d\xC4\xED\xC5\xB0", item.m_price) : MakeStr("%d\xC6\xCE", item.m_price)));
	}
}

// shopmain.cpp:1515
void CShopMain::OnSelFrameInit(PG_MSGARG param) {
	m_selFrame = DYNAMIC_CAST(FrArea, param);
}

// shopmain.cpp:1522
void CShopMain::OnStatBoxInit(PG_MSGARG param) {
	m_statBox = DYNAMIC_CAST(FrGroupBox, param);
}

// shopmain.cpp:1529
void CShopMain::OnSelItemIconInit(PG_MSGARG param) {
	m_selItemIcon = DYNAMIC_CAST(FrArea, param);
}

// shopmain.cpp:1536
void CShopMain::OnSelItemIconOwnerDraw(PG_MSGARG) {
	FrGraphicInterface *graphic = g_pFresh->GetManager()->GetGDI();
	if (graphic == NULL)
		return;

	switch (m_subTab) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 26:
		DrawSelItem(graphic, m_standPetTypeId);
		DrawCharacterList(graphic, m_standPetTypeId);
		break;
	case SUB_TAB_CLUBSET:
		DrawSelItem(graphic, m_clubsetTypeId);
		break;
	case SUB_TAB_AZTEC:
		DrawSelItem(graphic, m_aztecTypeId);
		break;
	case 10:
		DrawSelItem(graphic, m_caddieTypeId);
		DrawCaddieList(graphic, m_caddieTypeId);
		break;
	case SUB_TAB_MASCOT:
		DrawSelItem(graphic, m_mascotTypeId);
		break;
	}
}

// shopmain.cpp:1577
void CShopMain::OnSelItemInit(PG_MSGARG param) {
	m_selItem = DYNAMIC_CAST(FrButton, param);
	if (m_selItem) {
		m_selItem->EnableBlink(true);
	}
}

// shopmain.cpp:1586
void CShopMain::OnSelItemOwnerDraw(PG_MSGARG itemIndex) {
}

// shopmain.cpp:1591
void CShopMain::OnTabTxt1Init(PG_MSGARG param) {
	m_tabTxt1 = DYNAMIC_CAST(FrArea, param);
}

// shopmain.cpp:1598
void CShopMain::OnTabTxt2Init(PG_MSGARG param) {
	m_tabTxt2 = DYNAMIC_CAST(FrArea, param);
}

// shopmain.cpp:1605
void CShopMain::OnTabTxt3Init(PG_MSGARG param) {
	m_tabTxt3 = DYNAMIC_CAST(FrArea, param);
}

// shopmain.cpp:1612
void CShopMain::OnSelectedNameInit(PG_MSGARG param) {
	m_selectedName = DYNAMIC_CAST(FrStatic, param);
}

// shopmain.cpp:1619
void CShopMain::OnSelItemUp(void) {
	m_selItemExpanded = !m_selItemExpanded;
}

// shopmain.cpp:1629
void CShopMain::OnSelItemPrevInit(PG_MSGARG param) {
	m_selItemPrev = DYNAMIC_CAST(FrButton, param);
	if (m_selItemPrev) {
		m_selItemPrev->SetPushDelay(0.0f);
	}
}

// shopmain.cpp:1638
void CShopMain::OnSelItemPrevUp(void) {
	switch (m_shopTab) {
	case 0:
		for (std::map<unsigned int, IFF_STRUCT::sChar>::reverse_iterator it = ItemManager()->m_charMap.rbegin(), end = ItemManager()->m_charMap.rend(); it != end; ++it) {
			if ((*it).second.m_typeId == m_standPetTypeId) {
				if (it++ != end) {
					ChangeCharacter((*it).second.m_typeId);
					break;
				}
			}
		}
		break;
	case 2:
		for (std::map<unsigned int, IFF_STRUCT::sCaddie>::reverse_iterator it = m_visibleCaddies.rbegin(); it != m_visibleCaddies.rend(); ++it) {
			if ((*it).second.m_typeId == m_caddieTypeId) {
				while (it++ != m_visibleCaddies.rend()) {
					const IFF_STRUCT::sCaddie &item = (*it).second;
					if ((item.flags >> 5) != 0 && (item.flags >> 5) != 5 && (item.flags >> 5) != 6
						&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
						ChangeCaddie((*it).second.m_typeId);
						break;
					}
				}
			}
		}
		break;
	case 3:
		for (std::map<unsigned int, IFF_STRUCT::sMascot>::reverse_iterator it = ItemManager()->m_mascotMap.rbegin(); it != ItemManager()->m_mascotMap.rend(); ++it) {
			if ((*it).second.m_typeId == m_mascotTypeId) {
				while (it++ != ItemManager()->m_mascotMap.rend()) {
					const IFF_STRUCT::sMascot &item = (*it).second;
					if ((item.m_flags >> 5) != 0 && (item.m_flags >> 5) != 5 && (item.m_flags >> 5) != 6
						&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
						ChangeMascot((*it).second.m_typeId);
						break;
					}
				}
			}
		}
		break;
	}
}

// shopmain.cpp:1680
void CShopMain::OnSelItemNextInit(PG_MSGARG param) {
	m_selItemNext = DYNAMIC_CAST(FrButton, param);
	if (m_selItemNext) {
		m_selItemNext->SetPushDelay(0.0f);
	}
}

// shopmain.cpp:1689
void CShopMain::OnSelItemNextUp(void) {
	switch (m_shopTab) {
	case 0:
		for (std::map<unsigned int, IFF_STRUCT::sChar>::iterator it = ItemManager()->m_charMap.begin(), end = ItemManager()->m_charMap.end(); it != end; ++it) {
			if ((*it).second.m_typeId == m_standPetTypeId) {
				if (it++ != end) {
					ChangeCharacter((*it).second.m_typeId);
					break;
				}
			}
		}
		break;
	case 2:
		for (std::map<unsigned int, IFF_STRUCT::sCaddie>::iterator it = m_visibleCaddies.begin(); it != m_visibleCaddies.end(); ++it) {
			if ((*it).second.m_typeId == m_caddieTypeId) {
				while (it++ != m_visibleCaddies.end()) {
					const IFF_STRUCT::sCaddie &item = (*it).second;
					if ((item.flags >> 5) != 0 && (item.flags >> 5) != 5 && (item.flags >> 5) != 6
						&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
						ChangeCaddie((*it).second.m_typeId);
						break;
					}
				}
			}
		}
		break;
	case 3:
		for (std::map<unsigned int, IFF_STRUCT::sMascot>::iterator it = ItemManager()->m_mascotMap.begin(); it != ItemManager()->m_mascotMap.end(); ++it) {
			if ((*it).second.m_typeId == m_mascotTypeId) {
				while (it++ != ItemManager()->m_mascotMap.end()) {
					const IFF_STRUCT::sMascot &item = (*it).second;
					if ((item.m_flags >> 5) != 0 && (item.m_flags >> 5) != 5 && (item.m_flags >> 5) != 6
						&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
						ChangeMascot((*it).second.m_typeId);
						break;
					}
				}
			}
		}
		break;
	}
}

// shopmain.cpp:1737
void CShopMain::OnStatFrameInit(PG_MSGARG param) {
	m_statFrame = DYNAMIC_CAST(FrArea, param);
}

// shopmain.cpp:1742
void CShopMain::OnStatTextInit(PG_MSGARG param) {
	m_statText = DYNAMIC_CAST(FrArea, param);
}

// shopmain.cpp:1747
void CShopMain::OnStatPartInit(PG_MSGARG param) {
	m_statPart = DYNAMIC_CAST(FrViewer, param);
}

// shopmain.cpp:1754
void CShopMain::OnStatPartUp(void) {
	m_statTab = STAT_TAB_PART;
}

// shopmain.cpp:1759
void CShopMain::OnStatPartOwnerDraw(PG_MSGARG) {
	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI) {
		pGDI->DrawTexture(m_statPartBitmap[m_statPartKind][2], m_statPart->GetRect(), 0xFFFFFFFF, 0);

		WRect rect = m_statAll->GetRect();
		pGDI->DrawTexture(m_statAllBitmap[0], rect, 0xFFFFFFFF, 0);
	}
}

// shopmain.cpp:1772
void CShopMain::OnStatAllInit(PG_MSGARG param) {
	m_statAll = DYNAMIC_CAST(FrViewer, param);
	if (m_statAll) {
		m_statAll->SetVisible(false);
	}
}

// shopmain.cpp:1782
void CShopMain::OnStatAllUp(void) {
	m_statTab = STAT_TAB_ALL;
}

// shopmain.cpp:1787
void CShopMain::OnStatAllOwnerDraw(PG_MSGARG) {
	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI) {
		int state = 0;
		WRect rect = m_statAll->GetRect();
		if (m_statAll->IsVisible()) {
			if (m_statTab == STAT_TAB_ALL) {
				state = 2;
			} else if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
				state = 1;
			}
			pGDI->DrawTexture(m_statAllBitmap[state], rect, 0xFFFFFFFF, 0);
		}
	}
}

// shopmain.cpp:1810
void CShopMain::OnTabMainInit(PG_MSGARG param) {
	m_tabMainButton = DYNAMIC_CAST(FrTabButton, param);
	if (m_tabMainButton) {

		const char *names[3] = { NULL, };

		for (int i = 0; i < 5; i++) {

			names[0] = MakeStr("stab_%d_n", i + 1); names[1] = MakeStr("stab_%d_d", i + 1); names[2] = MakeStr("stab_%d_d", i + 1);
			m_tabMainButton->AddTab(names, i, NULL);
		}

		names[0] = MakeStr("stab_%d_n", 7); names[1] = MakeStr("stab_%d_d", 7); names[2] = MakeStr("stab_%d_d", 7);
		m_tabMainButton->AddTab(names, 5, NULL);
	}
}

// shopmain.cpp:1846
void CShopMain::OnTabMainButtonUp(void) {
	TButtonItem *item = m_tabMainButton->GetSelected();
	if (item) {

		if (item->index != m_shopTab) {

			ChangeMainTab((mainTab_t)item->index, false);

			switch (item->index) {
			case 0:
				ChangeSubTab((subTab_t)0, false);
				ChangeCharacter(m_standPetTypeId);
				break;

			case 1:
				ChangeSubTab((subTab_t)6, false);
				ChangeClubset(m_clubsetTypeId);
				break;

			case 2:
				ChangeSubTab((subTab_t)10, false);
				ChangeCaddie(m_caddieTypeId);
				break;

			case 3:
				ChangeSubTab((subTab_t)12, false);
				ChangeMascot(m_mascotTypeId);
				break;

			case 4:
				ChangeSubTab((subTab_t)13, false);
				ChangeSkin(m_standPetTypeId);
				break;

			case 5:
				ChangeSubTab((subTab_t)32, false);
				m_anotherHand.WaitUntilWorkDone(9999);
				if (m_petExhibition) { delete m_petExhibition; m_petExhibition = NULL; }
				break;
			}
		}
	}
}

// shopmain.cpp:1898
void CShopMain::OnTabSubInit(PG_MSGARG param) {
	m_tabSubButton = DYNAMIC_CAST(FrTabButton, param);
}

// shopmain.cpp:1906
void CShopMain::OnTabSubButtonUp(void) {
	TButtonItem *selected = m_tabSubButton->GetSelected();
	if (selected) {
		ChangeSubTab((subTab_t)selected->index, false);
	}
}

// shopmain.cpp:1915
void CShopMain::OnItemListInit(intptr_t param) {
	m_itemListBox = DYNAMIC_CAST(FrListBox, param);
	if (m_itemListBox == 0)
		return;

	m_itemListBox->UseRightButton(true);
	m_itemListBox->UseDummy(true);
	m_itemListBox->SetPushSound(0);
	m_itemListBox->AddNoRButtonRect(m_miniBtns[1].rect, 1);
	m_itemListBox->AddNoRButtonRect(m_miniBtns[2].rect, 2);
}

// shopmain.cpp:1928
void CShopMain::DrawStatus(FrGraphicInterface *graphic, FrListItem *listItem, short *const status, short *const slotStatus, float x) {
	float posY = 4.0f;

	for (int i = 0; i < 5; i++) {
		const Bitmap *dots[2] = { m_dotBitmaps[i][0], m_dotBitmaps[i][1] };

		int statusCount = status[i];
		int slotCount = slotStatus[i];

		if (slotCount != 0 || statusCount != 0) {
			float posX = 86.0f;
			for (int j = 0; j < statusCount; j++) {
				graphic->DrawTexture(dots[0], WRect(posX + x + listItem->pos.x, posY + listItem->pos.y + x, 5.0f, 6.0f), 0xFFFFFFFF, 0);
				posX -= 6.0f;
			}

			for (int j = 0; j < slotCount; j++) {
				graphic->DrawTexture(dots[1], WRect(posX + x + listItem->pos.x, posY + listItem->pos.y + x, 5.0f, 6.0f), 0xFFFFFFFF, 0);
				posX -= 6.0f;
			}

			posY += 6.0f;
		}
	}
}

// shopmain.cpp:1962
void CShopMain::DrawStatus2(FrGraphicInterface *graphic, FrListItem *listItem, short *const status, short *const slotStatus, float x) {
	float offsetY = 0.0f;
	for (int i = 0; i < 5; i++) {
		if (status[i] != 0 || slotStatus[i] != 0) {
			graphic->SetTextColor(0xFF0000FF, 0xFFFFFFFF);
			graphic->Print11(WPoint(x + 182.0f, offsetY + x + 36.0f) + listItem->pos, 2, MakeStatusString(i, status[i], slotStatus[i]));
			offsetY += 14.0f;
		}
	}
}

// shopmain.cpp:1986
void CShopMain::DrawStatusCard(FrGraphicInterface *graphic, FrListItem *listItem, short *const cardStatus, float x) {
	float offsetY = 0.0f;
	for (int i = 0; i < 5; i++) {
		if (cardStatus[i] != 0) {
			graphic->SetTextColor(0xFF0000FF, 0xFFFFFFFF);
			graphic->Print11(WPoint(x + 182.0f, offsetY + x + 36.0f) + listItem->pos, 2, MakeCardStatusString(i, cardStatus[i]));
			offsetY += 14.0f;
		}
	}
}

// shopmain.cpp:2000
void CShopMain::DrawPartsSlot(FrGraphicInterface *graphic, FrListItem *listItem) {
	IFF_STRUCT::sPart *part = (IFF_STRUCT::sPart *)listItem->pData;
	if (part == NULL)
		return;

	if (graphic == NULL) {
		graphic = g_pFresh->GetManager()->GetGDI();
		if (graphic == NULL)
			return;
	}

	float offsetX = 5.0f;
	if (part->m_shopMark && m_markBegin)
		graphic->DrawTexture(m_markBegin, WRect(listItem->pos.x + 5.0f, listItem->pos.y + 65.0f, (float)m_markBegin->Width(), (float)m_markBegin->Height()), 0xFFFFFFFF, 0);

	for (int i = 0; i < part->m_charCardSlots; i++) {
		if (m_cardCharacterEmpty)
			m_cardCharacterEmpty->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(offsetX + listItem->pos.x, listItem->pos.y + 54.0f, 12.0f, 12.0f), 0, 0xFFFFFFFF, 0.0f, 0);
		offsetX += 16.0f;
	}

	offsetX = 5.0f;
	for (int j = 0; j < part->m_caddieCardSlots; j++) {
		if (m_cardCaddieEmpty)
			m_cardCaddieEmpty->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(offsetX + listItem->pos.x, listItem->pos.y + 70.0f, 12.0f, 12.0f), 0, 0xFFFFFFFF, 0.0f, 0);
		offsetX += 16.0f;
	}
}

extern TitleManager *g_TitleManager;   // defined in ProjectG.cpp

// shopmain.cpp:2042
void CShopMain::OnItemListOwnerDraw(PG_MSGARG param) {
	FrListItem *listItem = (FrListItem *)param;
	if (listItem == NULL)
		return;

	FrGraphicInterface *graphic = g_pFresh->GetManager()->GetGDI();

	IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)listItem->pData;

	const Bitmap *bitmap;
	if (item == NULL) {
		bitmap = m_itemBaseEmpty;
	} else {
		if (listItem->selected) {
			bitmap = m_itemBaseDown;
			graphic->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
		} else if (listItem->underCursor) {
			bitmap = m_itemBaseOver;
			graphic->SetTextColor(0xFF000000, 0xFFFFFFFF);
		} else {
			bitmap = m_itemBaseNormal;
			graphic->SetTextColor(0xFF000000, 0xFFFFFFFF);
		}
		graphic->SetTextStyle(0);
	}

	if (bitmap)
		graphic->DrawTexture(bitmap, WRect(listItem->pos.x, listItem->pos.y, (float)bitmap->Width(), (float)bitmap->Height()), 0xFFFFFFFF, 0);

	if (item == NULL)
		return;

	if ((item->m_typeId & 0xFC000000) == 0x38000000 && (item->m_typeId & 0x3C00000) == 0x1800000) {
		const Bitmap *titleBg = g_pFresh->GetBitmap("Title_BG");
		graphic->DrawTexture(titleBg, WRect(listItem->pos.x + 7.0f, listItem->pos.y + 7.0f, (float)titleBg->Width(), (float)titleBg->Height()), 0xFFFFFFFF, 0);
	}

	const Bitmap *icon = GetItemIcon(item->m_typeId);
	if (UccManager()->IsDisplayUccClothes(item)) {
		sDisplayUccClothes *display = (sDisplayUccClothes *)listItem->pData;
		icon = GetUccItemIcon(display->m_serial);
	}

	if (icon) {
		float offsetX;
		float offsetY;
		switch (m_subTab) {
		case 0:
			if ((item->m_typeId & 0xFC000000) == 0x24000000) {
				offsetX = 7.0f;
				offsetY = 2.0f;
			} else {
				offsetX = 16.0f;
				offsetY = 27.0f;
			}
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 12:
		case 26:
			offsetX = 7.0f;
			offsetY = 2.0f;
			break;
		case 10:
			if ((item->m_typeId & 0xFC000000) == 0x20000000) {
				offsetX = 7.0f;
				offsetY = 2.0f;
			} else {
				offsetX = 16.0f;
				offsetY = 27.0f;
			}
			break;
		case 15:
			offsetX = 7.0f;
			offsetY = 12.0f;
			break;
		case 27:
		case 33:
			offsetX = 16.0f;
			offsetY = 12.0f;
			break;
		case 28:
		case 29:
		case 30:
		case 31:
		case 32:
			offsetX = 21.0f;
			offsetY = 12.0f;
			break;
		default:
			offsetX = 16.0f;
			offsetY = 27.0f;
			break;
		}
		graphic->DrawTexture(icon, WRect(offsetX + listItem->pos.x, offsetY + listItem->pos.y, (float)icon->Width(), (float)icon->Height()), 0xFFFFFFFF, 0);
	}

	if (UccManager()->IsDisplayUccClothes(item) && m_itemBaseAward)
		graphic->DrawTexture(m_itemBaseAward, WRect(listItem->pos.x, listItem->pos.y, (float)m_itemBaseAward->Width(), (float)m_itemBaseAward->Height()), 0xFFFFFFFF, 0);

	graphic->SetTextStyle(0);
	switch (m_subTab) {
	case SUB_TAB_AZTEC: {
		IFF_STRUCT::sBall *ball = (IFF_STRUCT::sBall *)item;
		if ((item->m_typeId & 0xFC000000) == 0x24000000) {
			graphic->Print11(WPoint(listItem->pos.x + 182.0f, listItem->pos.y + 7.0f), 2, "%s", item->m_name);
		} else {
			graphic->Print11(WPoint(listItem->pos.x + 182.0f, listItem->pos.y + 7.0f), 2, "%s(%d\xB0\xB3)", item->m_name, ball->quantity);
		}
		break;
	}
	case 8:
	case 9: {
		IFF_STRUCT::sItem *usable = (IFF_STRUCT::sItem *)item;
		if (usable->quantity > 0) {
			graphic->Print11(WPoint(listItem->pos.x + 182.0f, listItem->pos.y + 7.0f), 2, "%s(%d\xB0\xB3)", item->m_name, usable->quantity);
		} else {
			graphic->Print11(WPoint(listItem->pos.x + 182.0f, listItem->pos.y + 7.0f), 2, item->m_name);
		}
		break;
	}
	case 27:
		if (IsLocalContent((localContentType_t)0x51)) {
			graphic->Print11(WPoint(listItem->pos.x + 182.0f, listItem->pos.y + 7.0f), 2, item->m_name);
		}
		break;
	case 28:
	case 29:
	case 30:
	case 32:
		if (IsLocalContent((localContentType_t)0x56)) {
			IFF_STRUCT::sFurniture *furniture = (IFF_STRUCT::sFurniture *)item;
			short count = furniture->m_dayPrices[0];
			if (count > 0) {
				graphic->Print11(WPoint(listItem->pos.x + 182.0f, listItem->pos.y + 7.0f), 2, "%s(%d\xB0\xB3)", item->m_name, count);
			} else {
				graphic->Print11(WPoint(listItem->pos.x + 182.0f, listItem->pos.y + 7.0f), 2, item->m_name);
			}
		}
		break;
	case 31:
		if ((item->m_typeId & 0xFC000000) == 0x48000000) {
			IFF_STRUCT::sFurniture *furniture = (IFF_STRUCT::sFurniture *)item;
			short count = furniture->m_dayPrices[0];
			if (count > 0) {
				graphic->Print11(WPoint(listItem->pos.x + 182.0f, listItem->pos.y + 7.0f), 2, "%s(%d\xB0\xB3)", item->m_name, count);
			} else {
				graphic->Print11(WPoint(listItem->pos.x + 182.0f, listItem->pos.y + 7.0f), 2, item->m_name);
			}
		}
		break;
	default:
		if (UccManager()->IsDisplayUccClothes(item)) {
			sDisplayUccClothes *display = (sDisplayUccClothes *)listItem->pData;
			graphic->Print11(WPoint(listItem->pos.x + 182.0f, listItem->pos.y + 7.0f), 2, GetUccItemName(display->m_serial));
			g_pFresh->GetManager()->PrintText(WPoint(listItem->pos.x + 182.0f, listItem->pos.y + 23.0f), 2, display->m_uccKey.c_str(), -1.0f, 0xFFFFFFFF);
		} else {
			graphic->Print11(WPoint(listItem->pos.x + 182.0f, listItem->pos.y + 7.0f), 2, item->m_name);
		}
		break;
	}

	if (m_shopTab == 0 && m_sideTabRental && m_sideTabRental->GetStatus() == FrButton::PRESSED) {
		DrawRentalPrice(182.0f, 23.0f, *item, listItem);
	} else {
		DrawItemPrice(graphic, 182.0f, 23.0f, *item, listItem);
	}

	if (m_subTab > 0 && (m_subTab <= 4 || m_subTab == 26)) {
		IFF_STRUCT::sPart *part = (IFF_STRUCT::sPart *)listItem->pData;
		DrawStatus2(graphic, listItem, part->m_baseStat, part->m_slotStat, 0.0f);
		if (IsLocalContent((localContentType_t)0x51))
			DrawPartsSlot(graphic, listItem);
	}

	if (listItem->selected && (item->m_level & 0x7F) > WSingleton<CSharedDoc>::Instance()->m_myLevel) {
		const Bitmap *level = g_pFresh->GetBitmap(MakeStr("level_%03d", (item->m_level & 0x7F) + 1));
		if (level)
			graphic->DrawTexture(level, WRect(listItem->pos.x + 10.0f, listItem->pos.y + 4.0f, (float)level->Width(), (float)level->Height()), 0xFFFFFFFF, 0);
	}

	if (m_markLimited) {
		if (item->m_saleStart.wYear != 0 || item->m_saleEnd.wYear != 0)
			graphic->DrawTexture(m_markLimited, WRect(listItem->pos.x, listItem->pos.y, (float)m_markLimited->Width(), (float)m_markLimited->Height()), 0xFFFFFFFF, 0);
	}

	if ((item->m_flags1 & 1) && m_markNew)
		graphic->DrawTexture(m_markNew, WRect(listItem->pos.x, listItem->pos.y, (float)m_markNew->Width(), (float)m_markNew->Height()), 0xFFFFFFFF, 0);

	if ((item->m_flags1 & 2) && m_markHot)
		graphic->DrawTexture(m_markHot, WRect(listItem->pos.x, listItem->pos.y, (float)m_markHot->Width(), (float)m_markHot->Height()), 0xFFFFFFFF, 0);

	const WPoint &mousePos = g_pFresh->GetManager()->GetMousePos();

	bool canBuy = ((item->m_flags0 >> 5) == 1 || (item->m_flags0 >> 5) == 3) ? true : false;

	if ((item->m_typeId & 0xFC000000) == 0x38000000 && (item->m_typeId & 0x3C00000) == 0x1800000) {
		Title *title = g_TitleManager->GetTitle((eTitle)(item->m_typeId & 0x3FFFFF));
		if (title) {
			if (!title->CanEquip()) {
				const Bitmap *noBuy = g_pFresh->GetBitmap("do not purchase");
				if (noBuy) {
					graphic->DrawTexture(noBuy, WRect(WPoint(listItem->pos.x + 1.0f, listItem->pos.y + 1.0f), noBuy->Width(), noBuy->Height()), 0xFFFFFFFF, 0);
					canBuy = false;
				}
			}
		}
	}

	if (canBuy) {
		if (!m_miniBtns[1].enable) {
			graphic->DrawTexture(m_miniBtns[1].normal, WRect(m_miniBtns[1].rect + listItem->pos), 0x88FFFFFF, 0);
		} else if (listItem->underCursor && (m_miniBtns[1].rect + listItem->pos).IsInRect(mousePos)) {
			graphic->DrawTexture(m_miniBtns[1].over, m_miniBtns[1].rect + listItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x16);
		} else {
			WRect rc = m_miniBtns[1].rect + listItem->pos;
			graphic->DrawTexture(m_miniBtns[1].normal, WRect(rc.x, rc.y, rc.w, rc.h), 0xFFFFFFFF, 0);
		}
	}

	bool canGift = ((item->m_flags0 >> 5) == 1 || (item->m_flags0 >> 5) == 2) ? true : false;

	if (m_sideTabRental && m_sideTabRental->GetStatus() == FrButton::PRESSED && m_shopTab == 0)
		canGift = false;

	if (canGift) {
		if (!m_miniBtns[2].enable) {
			graphic->DrawTexture(m_miniBtns[2].normal, WRect(m_miniBtns[2].rect.x + listItem->pos.x, m_miniBtns[2].rect.y + listItem->pos.y, m_miniBtns[2].rect.w, m_miniBtns[2].rect.h), 0x88FFFFFF, 0);
		} else if (listItem->underCursor && (m_miniBtns[2].rect + listItem->pos).IsInRect(mousePos)) {
			graphic->DrawTexture(m_miniBtns[2].over, m_miniBtns[2].rect + listItem->pos, 0xFFFFFFFF, 0);
			CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x17);
		} else {
			graphic->DrawTexture(m_miniBtns[2].normal, m_miniBtns[2].rect + listItem->pos, 0xFFFFFFFF, 0);
		}
	}
}

// The gateway actor's name (a pointer variable in the original's data).
extern "C" { static const char* data_aa6254 = "Gateway"; }

// shopmain.cpp:2528
void CShopMain::OnItemListBtnDown(void) {
	FrListItem *listItem = m_itemListBox->GetItemUnderCursor();
	if (listItem) {

		if (m_anotherHand.IsWorking(0)) {
			m_itemListBox->ToggleItem(listItem);
			return;
		}

		IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)listItem->pData;
		CPartTidList &tidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

		if (UccManager()->IsDisplayUccClothes(item)) {
			// The list entry is a part record followed by the UCC ids (sDisplayUccClothes).
			sDisplayUccClothes *display = (sDisplayUccClothes *)listItem->pData;
			IFF_STRUCT::sPart *base = ItemManager()->FindPart(display->m_typeId);
			sUccClothes *clothes = UccManager()->FindClothes(display->m_serial);
			if (base && clothes) {
				IFF_STRUCT::sPart *part = (IFF_STRUCT::sPart *)display;
				part->hideMask = base->hideMask;
				part->meshHideMask = base->meshHideMask;
				strcpy(part->model, base->model);
				strcpy(part->texture[0], GenerateTextureName_F(clothes->m_typeId, clothes->m_uccKey).c_str());
				strcpy(part->texture[1], GenerateTextureName_B(clothes->m_typeId, clothes->m_uccKey).c_str());
				strcpy(part->texture2[0], base->texture[0]);
				strcpy(part->texture2[1], base->texture[1]);

				memcpy(part->m_subParts, base->m_subParts, sizeof(part->m_subParts));
			}
		}

		const WPoint &mousePos = g_pFresh->GetManager()->GetMousePos();
		if ((m_miniBtns[1].rect + listItem->pos).IsInRect(mousePos)) {

			bool canBuy = ((item->m_flags0 >> 5) == 1 || (item->m_flags0 >> 5) == 3) ? true : false;

			if (m_miniBtns[1].enable && canBuy) {

				ResetItemList(false);
				ClearCartList(false, &tidList);
				m_shoppersOldItems.clear();

				m_itemListBox->SelectItem(listItem, true);

				if ((item->m_typeId >> 26) == 15 && !HaveCharacter(m_standPetTypeId)) {
					this << MsgObject(0, 0x23, (intptr_t)"\xC7\xD8\xB4\xE7 \xC4\xB3\xB8\xAF\xC5\xCD\xB8\xA6 \xB0\xA1\xC1\xF6\xB0\xED \xC0\xD6\xC1\xF6 \xBE\xCA\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
					return;
				}
				if ((item->m_typeId >> 26) == 1 && HaveCharacter(m_standPetTypeId)) {
					this << MsgObject(0, 0x23, (intptr_t)"\xC7\xD8\xB4\xE7 \xC4\xB3\xB8\xAF\xC5\xCD\xB8\xA6 \xC0\xCC\xB9\xCC \xB0\xA1\xC1\xF6\xB0\xED \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
					return;
				}

				if (!SelectItemList(listItem))
					return;

				bool result = false;
				IActor *actor = AfxGetTask()->GetActor(data_aa6254);
				actor << MsgObject(0, 0x26D, 0x10, (intptr_t)&result, 0, 0, 0);

				if (result == true) {

					if ((item->m_typeId & 0xFC000000) == 0x20000000 && (item->m_typeId & 0x1FE000) == 0x6000) {
						UpgradeCaddie(m_caddieTypeId);
						return;
					}

					Checkout();
					return;
				}

				actor << MsgObject(0, 0x26D, 0x11, 0, 0, 0, 0);
				return;
			}
		} else if ((m_miniBtns[2].rect + listItem->pos).IsInRect(mousePos)) {

			bool canGift = ((item->m_flags0 >> 5) == 1 || (item->m_flags0 >> 5) == 2) ? true : false;

			if (m_miniBtns[2].enable && canGift) {

				ResetItemList(false);
				m_itemListBox->SelectItem(listItem, true);

				if (!SelectItemList(listItem))
					return;

				bool result = false;
				IActor *actor = AfxGetTask()->GetActor(data_aa6254);
				actor << MsgObject(0, 0x26D, 0x10, (intptr_t)&result, 0, 0, 0);

				if (result == true) {
					SendGift(*item);
					return;
				}

				actor << MsgObject(0, 0x26D, 0x11, 0, 0, 0, 0);
				return;
			}
		}

		if (!listItem->selected) {

			switch (item->m_typeId >> 26) {
			case 2:
			case 14:
			case 15:
				UnselectItemList(item->m_typeId);
				break;
			default:
				m_itemListBox->ToggleItem(listItem);
				break;
			}
		} else {

			if ((item->m_typeId >> 26) == 0x1F) {
				m_cardTypeId = item->m_typeId;
			} else if ((item->m_typeId >> 26) == 9 && (item->m_typeId & 0x1FE000) == 0x1FA000) {
				m_cardTypeId = item->m_typeId;
			} else {
				m_cardTypeId = 0;
			}

			SelectItemList(listItem);
		}

		if (m_shopTab == 0 || m_shopTab == 4) {

			m_buyAll->Enable(!m_cartList->m_itemList.empty() || !m_auxCartList->m_itemList.empty());
			m_buyReset->Enable(!m_cartList->m_itemList.empty() || !m_auxCartList->m_itemList.empty());
		}
	}
}

// shopmain.cpp:2829
void CShopMain::OnItemListRBtnDown(void) {
	if (m_itemListBox) {

		FrListItem *listItem = m_itemListBox->GetItemUnderCursor();
		if (listItem) {

			if (m_anotherHand.IsWorking(0)) {

				m_itemListBox->ToggleItem(listItem);
				return;
			}

			if (listItem->selected)
				SelectItemList(listItem);

			m_itemListBox->SelectItem(listItem, true);

			IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)listItem->pData;
			if (item) {

				if (!UccManager()->IsDisplayUccClothes(item)) {

					OpenInformation(item->m_typeId);

					if (m_shopTab == 0 || m_shopTab == 4) {

						m_buyAll->Enable(!m_cartList->m_itemList.empty() || !m_auxCartList->m_itemList.empty());
						m_buyReset->Enable(!m_cartList->m_itemList.empty() || !m_auxCartList->m_itemList.empty());
					}
				}
			}
		}
	}
}

// shopmain.cpp:2886
void CShopMain::OnBuyAllInit(PG_MSGARG param) {
	m_buyAll = DYNAMIC_CAST(FrButton, param);
	if (m_buyAll) {
		m_buyAll->EnableHover(true, 0.0f);
	}
}

// shopmain.cpp:2895
void CShopMain::OnBuyAllUp(void) {
	FrListBox::ITEM_LIST::iterator it;
	for (it = m_cartList->m_itemList.begin(); it != m_cartList->m_itemList.end(); ++it) {
		IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)(*it)->pData;

		if ((item->m_typeId >> 26) == 1) {
			if (HaveCharacter(item->m_typeId))
				it = m_cartList->_DelItem(it);
			break;
		} else if ((item->m_typeId >> 26) == 9) {
			break;
		}
	}

	if (!HaveCharacter(m_standPetTypeId) && it == m_cartList->m_itemList.end()) {
		this << MsgObject(0, 0x23, (intptr_t)"\xC7\xD8\xB4\xE7 \xC4\xB3\xB8\xAF\xC5\xCD\xB8\xA6 \xB0\xA1\xC1\xF6\xB0\xED \xC0\xD6\xC1\xF6 \xBE\xCA\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
		return;
	}

	Checkout();
	OnBuyHoverOff(0);
}

// shopmain.cpp:2958
void CShopMain::OnBuyResetInit(PG_MSGARG param) {
	m_buyReset = DYNAMIC_CAST(FrButton, param);
	if (m_buyReset) {
		m_buyReset->EnableHover(true, 0.0f);
	}
}

// shopmain.cpp:2967
void CShopMain::OnBuyResetUp(void) {
	g_audio->PlaySfx("cloth_off", 0, 0, 0, 0, 0.5f, 200.0f);

	if (m_shopTab == 0) {
		std::map<unsigned int, sCharacterInfo>::iterator end = WSingleton<CSharedDoc>::Instance()->m_characterMap.end();
		std::map<unsigned int, sCharacterInfo>::iterator it;
		for (it = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin(); it != end; ++it) {
			if (it->second.typeId == m_standPetTypeId)
				break;
		}

		if (m_subTab == 0 || m_subTab == 1 || m_subTab == 2 || m_subTab == 3 || m_subTab == 4 || m_subTab == 5 || m_subTab == 0x1A) {

			if (it != end) {
				WSingleton<CSharedDoc>::Instance()->BuildMyPartTidList(it->second);

				m_charParts = it->second.partTypeIds;
				m_charCards = it->second.auxParts.tids;
				m_charLevels = (const char *)&it->second.unknown6A;
				m_hairColor = it->second.hairColor;
			} else {
				WSingleton<CSharedDoc>::Instance()->BuildMyPartTidListDefault(m_standPetTypeId);
			}

			ClearCartList(true, &WSingleton<CSharedDoc>::Instance()->GetMyPartTidList());
			BuildDisplayStand(m_subTab);
			m_anotherHand.Work(this, 0, &m_standPetTypeId, true, -1);
			TurnOnItemStatus();
		}
	} else if (m_shopTab == 4) {

		ResetItemList(false);
		ClearCartList(false, NULL);

		memcpy(m_equipSkin, WSingleton<CSharedDoc>::Instance()->m_equipSkin, sizeof(m_equipSkin));
		for (int i = 0; i < 6; i++) {
			if (m_equipSkin[i]) {
				IFF_STRUCT::sSkin *skin = ItemManager()->FindSkin(m_equipSkin[i]);
				if (skin && !(m_equipSkin[i] & 0x3C00000)) {
					m_sideDir[0] = skin->m_sideSlideMode[0];
					m_sideDir[1] = skin->m_sideSlideMode[1];
					m_sidePos[0] = 0;
					m_sidePos[1] = 0;
				}
			}
		}
	}

	m_buyAll->Enable(false);
	m_buyReset->Enable(false);

	OnBuyHoverOff(0);
}

// shopmain.cpp:3059
void CShopMain::OnBuyHoverOn(PG_MSGARG param) {
	FrButton *button = DYNAMIC_CAST(FrButton, param);
	if (button == m_buyAll) {
		CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x18);
	} else if (button == m_buyReset) {
		CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x19);
	}
}

// shopmain.cpp:3072
void CShopMain::OnBuyHoverOff(PG_MSGARG param) {
	FrButton *button = DYNAMIC_CAST(FrButton, param);
	CIconToolTip *toolTip = CIconToolTip::GetInstance();
	bool clear = false;

	switch (toolTip->GetCurTipIndex()) {
	case 0x18:

		if (button == m_buyAll) {

			clear = true;
		}
		break;

	case 0x19:

		if (button == m_buyReset) {

			clear = true;
		}
		break;
	}

	if (!button || clear) {

		CIconToolTip::GetInstance()->ClearToolTip();
	}
}

// shopmain.cpp:3107
void CShopMain::OnSideTabBuyInit(PG_MSGARG param) {
	m_sideTabBuy = DYNAMIC_CAST(FrButton, param);
	if (m_sideTabBuy) {
		m_sideTabBuy->SetPushDelay(0.2f);
		m_sideTabBuy->SetStatus(FrButton::PRESSED);
	}
}

// shopmain.cpp:3118
void CShopMain::OnSideTabBuyUp(void) {
	if (m_sideTabBuy && m_sideTabRental) {
		if (m_sideTabBuy->GetStatus() != FrButton::PRESSED) {
			m_sideTabBuy->SetStatus(FrButton::PRESSED);
			m_sideTabRental->SetStatus(FrButton::NORMAL);

			m_miniBtns[2].enable = true;

			ClearCartList(true, &WSingleton<CSharedDoc>::Instance()->GetMyPartTidList());
			SetLevelBar();
			BuildDisplayStand(m_subTab);
			m_anotherHand.Work(this, 0, &m_standPetTypeId, true, -1);
		}
	}
}

// shopmain.cpp:3137
void CShopMain::OnSideTabRentalInit(PG_MSGARG param) {
	m_sideTabRental = DYNAMIC_CAST(FrButton, param);
	if (m_sideTabRental) {
		m_sideTabRental->SetPushDelay(0.2f);
		m_sideTabRental->SetStatus(FrButton::NORMAL);
	}
}

// shopmain.cpp:3147
void CShopMain::OnSideTabRentalUp(void) {
	if (m_sideTabBuy && m_sideTabRental) {
		if (m_sideTabRental->GetStatus() != FrButton::PRESSED) {
			m_sideTabRental->SetStatus(FrButton::PRESSED);
			m_sideTabBuy->SetStatus(FrButton::NORMAL);

			m_miniBtns[2].enable = false;

			ClearCartList(false, &WSingleton<CSharedDoc>::Instance()->GetMyPartTidList());
			SetLevelBar();
			BuildDisplayRental(m_subTab);
			m_anotherHand.Work(this, 0, &m_standPetTypeId, true, -1);
		}
	}
}

// shopmain.cpp:3177
void CShopMain::SetVisibleSideTab(bool bVisible) {
	if (m_sideTabBuy) m_sideTabBuy->SetVisible(bVisible);
	if (m_sideTabRental) m_sideTabRental->SetVisible(bVisible);
}

// shopmain.cpp:3182
void CShopMain::BuildDisplayRental(subTab_t tab) {
	m_itemListBox->ClearItem();

	if (tab > 0 && tab <= 4) {
		unsigned long categoryFlag = GetCategoryFlag(tab);

		for (std::map<unsigned int, IFF_STRUCT::sPart>::iterator it = ItemManager()->m_partMap.begin();
			it != ItemManager()->m_partMap.end(); ++it) {
			IFF_STRUCT::sPart &part = it->second;
			if (part.m_rentalPrice > 0
				&& S5::UnderCategory(m_standPetTypeId, part.m_typeId, categoryFlag)
				&& (part.m_flags & 0xE0) != 0
				&& ItemManager()->IsInSalePeriod(part.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
				if (!IsAngelWing(part.m_typeId) || (IsMannerPlayer() && WSingleton<CSharedDoc>::Instance()->m_myLevel >= 0x15)) {
					FrListItem *listItem = m_itemListBox->AddItem(&part);
					if (listItem && IsItemInCart(listItem)) {
						m_itemListBox->SelectItem(listItem, true);
					}

					unsigned char shopFlag = part.m_flags >> 5;
					if (shopFlag == 1 || shopFlag == 3) {
						listItem->AddNoRBtnIdx(1);
					}
				}
			}
		}

		m_itemListBox->SortItem(PartCompare);
	}

	int count = m_itemListBox->GetCurrentItemSize(true);
	for (int i = count; i < 8; i++) {
		m_itemListBox->AddItem(NULL);
	}
}

// shopmain.cpp:3231
void CShopMain::DrawRentalPrice(float x, float y, IFF_ITEM_COMMON &item, FrListItem *listItem) {
	if (listItem == NULL)
		return;

	if ((item.m_flags0 & 0xE0) == 0x80)
		return;

	if ((item.m_typeId & 0xFC000000) != 0x08000000)
		return;

	IFF_STRUCT::sPart *part = ItemManager()->FindPart(item.m_typeId);
	if (part == NULL)
		return;

	// One original line (3256): the GDI is fetched before the text and the
	// position after it.
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	const char* text = MakeStr("%d\xc6\xce(%d\xc0\xcf)", part->m_rentalPrice, 7);
	gdi->Print11(WPoint(x, y) + listItem->pos, 2, text);
}

// shopmain.cpp:3260
bool CShopMain::IsSelectedRentalTab(void) {
	if (m_shopTab == 0 && m_sideTabRental) {
		if (m_sideTabRental->GetStatus() == FrButton::PRESSED) {
			return true;
		}
	}
	return false;
}

// shopmain.cpp:3271
void CShopMain::OnPartCartListInit(PG_MSGARG param) {
	m_cartList = DYNAMIC_CAST(FrListBox, param);
}

// The cart row each card-slot kind (typeId bits 18-25) sits in.
extern "C" { extern int data_aa7d28[10]; }

// shopmain.cpp:3279
void CShopMain::OnPartCartListOwnerDraw(PG_MSGARG param) {
	FrListItem *listItem = (FrListItem *)param;
	if (listItem == NULL)
		return;

	FrGraphicInterface *graphic = g_pFresh->GetManager()->GetGDI();
	if (graphic == NULL)
		return;

	IFF_STRUCT::sPart *part = (IFF_STRUCT::sPart *)listItem->pData;
	if (part == NULL)
		return;

	bool hasStatus = false;
	short status[5];
	short slotStatus[5];
	memset(status, 0, sizeof(status));
	memset(slotStatus, 0, sizeof(slotStatus));
	int row;
	switch (part->m_typeId >> 26) {
	case 2:
		for (int i = 0; i < 5; i++) {
			if (part->m_baseStat[i] != 0 || part->m_slotStat[i] != 0) {
				memcpy(status, part->m_baseStat, sizeof(status));
				memcpy(slotStatus, part->m_slotStat, sizeof(slotStatus));
				hasStatus = true;
				break;
			}
		}

		switch (part->slot) {
		case 0:
			row = 1;
			break;
		case 1:
			row = 2;
			break;
		case 4:
			row = 3;
			break;
		case 7:
		case 8:
		case 9: {
			int kind = (part->m_typeId >> 18) & 0xFF;
			if (kind < 10) {
				if (((part->m_typeId >> 13) & 0x1F) == data_aa7d28[kind])
					row = 1;
				else
					row = 2;
			} else {
				row = 0;
			}
			break;
		}
		}
		break;
	case 15:
		row = 0;
		break;
	case 9: {
		int kind = (part->m_typeId >> 21) & 0x1F;
		if (kind == 6 || kind == 5 || kind == 4 || kind == 8)
			return;
		row = 1;
		break;
	}
	case 14:
		row = (part->m_typeId >> 22) & 0xF;
		break;
	default:
		return;
	}

	if (row < 0)
		return;

	float rowW = (float)m_itFa02->Width();
	float rowH = (float)m_itFa02->Height();
	float iconW = (float)m_sicons[0]->Width();
	float iconH = (float)m_sicons[0]->Height();
	float cancelW = (float)m_cancelBtnNormal->Width();
	float cancelH = (float)m_cancelBtnNormal->Height();

	float x = row > 3 ? 250.0f : 8.0f;
	int col = row % 4;
	int icon = row > 3 ? 4 : row;

	graphic->DrawTexture(m_itFa02, WRect(x, (float)col * rowH + 129.0f, rowW, rowH), 0xFFFFFFFF, 0);

	if ((part->m_typeId & 0xFC000000) == 0x38000000)
		graphic->DrawTexture(m_sicons[4], WRect(x + 4.0f, (float)col * rowH + 134.0f, iconW, iconH), 0xFFFFFFFF, 0);
	else
		graphic->DrawTexture(m_sicons[icon], WRect(x + 4.0f, (float)col * rowH + 134.0f, iconW, iconH), 0xFFFFFFFF, 0);

	graphic->SetTextColor(0xFFFFFFFF, 0xFF36779A);
	graphic->SetTextStyle(4);
	float textX = x + 8.0f;
	graphic->Print11(WPoint(textX, (float)col * rowH + 156.0f), 0, part->m_name);
	graphic->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
	graphic->SetTextStyle(0);
	if (hasStatus)
		DrawStatus3(graphic, textX, (float)col * rowH + 170.0f, status, slotStatus);

	WRect cancelRect(x + 104.0f, (float)col * rowH + 141.0f, cancelW, cancelH);
	if (cancelRect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
		graphic->DrawTexture(m_cancelBtnOver, cancelRect, 0xFFFFFFFF, 0);
		if (g_input->GetButton(LEFT_BUTTON) == 2) {
			unsigned long typeId = part->m_typeId;
			m_rbuttonPartTypeIds.push_back(typeId);
		}
	} else {
		graphic->DrawTexture(m_cancelBtnNormal, cancelRect, 0xFFFFFFFF, 0);
	}
}

// shopmain.cpp:3471
void CShopMain::DrawStatus3(FrGraphicInterface *graphic, float x, float y, short *const status, short *const slotStatus) {
	float offsetY = 0.0f;
	for (int i = 0; i < 5; i++) {
		if (status[i] != 0 || slotStatus[i] != 0) {
			graphic->Print11(WPoint(x, offsetY + y), 0, MakeStatusString(i, status[i], slotStatus[i]));
			offsetY += 12.0f;
		}
	}
}

// shopmain.cpp:3484
void CShopMain::OnAuxPartCartListInit(PG_MSGARG param) {
	m_auxCartList = DYNAMIC_CAST(FrListBox, param);
}

// shopmain.cpp:3492
void CShopMain::OnAuxPartCartListOwnerDraw(PG_MSGARG param) {
	FrListItem *listItem = (FrListItem *)param;
	if (listItem == NULL)
		return;

	FrGraphicInterface *graphic = g_pFresh->GetManager()->GetGDI();
	if (graphic == NULL)
		return;

	IFF_STRUCT::sPart *part = (IFF_STRUCT::sPart *)listItem->pData;
	if (part == NULL)
		return;

	bool hasStatus = false;
	short status[5];
	short slotStatus[5];
	memset(status, 0, sizeof(status));
	memset(slotStatus, 0, sizeof(slotStatus));
	for (int i = 0; i < 5; i++) {
		if (part->m_baseStat[i] != 0 || part->m_slotStat[i] != 0) {
			memcpy(status, part->m_baseStat, sizeof(status));
			memcpy(slotStatus, part->m_slotStat, sizeof(slotStatus));
			hasStatus = true;
			break;
		}
	}

	int index = listItem->no;

	const Bitmap *rowBg = m_itFa02;
	float rowH = (float)rowBg->Height();
	unsigned int siconW = m_sicons[0]->Width();
	unsigned int siconH = m_sicons[0]->Height();
	float cancelW = (float)m_cancelBtnNormal->Width();
	float cancelH = (float)m_cancelBtnNormal->Height();

	float rowY = (float)index * rowH;
	float y = rowY + 129.0f;
	graphic->DrawTexture(rowBg, WRect(250.0f, y, (float)rowBg->Width(), rowH), 0xFFFFFFFF, 0);
	graphic->DrawTexture(m_sicons[4], WRect(254.0f, y + 5.0f, (float)siconW, (float)siconH), 0xFFFFFFFF, 0);

	graphic->SetTextColor(0xFFFFFFFF, 0xFF36779A);
	graphic->SetTextStyle(4);
	graphic->Print11(WPoint(258.0f, y + 27.0f), 0, part->m_name);
	graphic->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
	graphic->SetTextStyle(0);
	if (hasStatus)
		DrawStatus3(graphic, 258.0f, y + 41.0f, status, slotStatus);

	WRect cancelRect(354.0f, rowY + 141.0f, cancelW, cancelH);
	if (cancelRect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
		graphic->DrawTexture(m_cancelBtnOver, cancelRect, 0xFFFFFFFF, 0);
		if (g_input->GetButton(LEFT_BUTTON) == 2) {
			unsigned long typeId = part->m_typeId;
			m_rbuttonPartTypeIds.push_back(typeId);
		}
	} else {
		graphic->DrawTexture(m_cancelBtnNormal, cancelRect, 0xFFFFFFFF, 0);
	}
}

// shopmain.cpp:3575
void CShopMain::CheckAuxCartScroll(void) {
	FrScrollBar *scrollBar = m_auxCartList->GetScrollBar();
	int top = scrollBar->GetCurTopRow_Int();

	if (scrollBar->GetRowCapacity() < scrollBar->GetItemNum()) {
		m_scrollUp->SetVisible(true);
		m_scrollDown->SetVisible(true);

		if (top == 0) {
			m_scrollUp->Enable(false);
			m_scrollDown->Enable(true);
		} else {
			m_scrollUp->Enable(true);
			m_scrollDown->Enable(false);
		}

		if (m_buyReset) {
			WRect resetRect, downRect, upRect;
			m_buyReset->GetClientRect(resetRect);
			m_scrollDown->GetClientRect(downRect);
			m_scrollUp->GetClientRect(upRect);

			downRect.x = resetRect.x - downRect.w;
			upRect.x = downRect.x - upRect.w - 2.0f;
			upRect.y = (resetRect.h - downRect.h) * 0.5f + resetRect.y;
			downRect.y = upRect.y;

			m_scrollUp->SetClientRect(upRect);
			m_scrollDown->SetClientRect(downRect);
		}
	} else {
		m_scrollUp->SetVisible(false);
		m_scrollDown->SetVisible(false);
	}
}

// shopmain.cpp:3619
void CShopMain::TurnOffAuxCartScroll(void) {
	m_scrollUp->SetVisible(false);
	m_scrollDown->SetVisible(false);
}

// shopmain.cpp:3625
void CShopMain::TurnOnAuxCartScroll(void) {
	m_scrollUp->SetVisible(true);
	m_scrollDown->SetVisible(true);
}

// shopmain.cpp:3631
void CShopMain::OnScrollUpInit(PG_MSGARG param) {
	m_scrollUp = DYNAMIC_CAST(FrButton, param);
	if (m_scrollUp) {
		m_scrollUp->SetVisible(false);
	}
}

// shopmain.cpp:3640
void CShopMain::OnScrollUpUp(void) {
	m_auxCartList->GetScrollBar()->ScrollUp(-1);
	CheckAuxCartScroll();
}

// shopmain.cpp:3646
void CShopMain::OnScrollDownInit(PG_MSGARG param) {
	m_scrollDown = DYNAMIC_CAST(FrButton, param);
	if (m_scrollDown) {
		m_scrollDown->SetVisible(false);
	}
}

// shopmain.cpp:3655
void CShopMain::OnScrollDownUp(void) {
	m_auxCartList->GetScrollBar()->ScrollDown(-1);
	CheckAuxCartScroll();
}

// shopmain.cpp:3661
void CShopMain::OnItemSlotFrame1Init(PG_MSGARG param) {
	m_itemSlotFrame1 = DYNAMIC_CAST(FrArea, param);
	if (m_itemSlotFrame1) {
		m_itemSlotFrame1->SetVisible(false);
	}
}

// shopmain.cpp:3670
void CShopMain::OnItemSlotFrame2Init(PG_MSGARG param) {
	m_itemSlotFrame2 = DYNAMIC_CAST(FrArea, param);
	if (m_itemSlotFrame2) {
		m_itemSlotFrame2->SetVisible(false);
	}
}

// shopmain.cpp:3679
void CShopMain::BuildMyItemSlot(bool bMarked) {
	m_itemSlot->ClearItem();

	for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {
		sItemInfo &info = *it;

		if (ItemManager()->GetCouponKind(info.m_typeId) != 0) {
			continue;
		}

		if (ItemManager()->IsNonVisibleItem(info.m_typeId) == true) {
			continue;
		}

		if (info.m_typeId == 0x1A000042) {
			continue;
		}

		int equipped = 0;

		if (bMarked) {
			if (!(info.m_typeId & 0x2000000)) {
				continue;
			}

			for (int i = 0; i < 10; i++) {
				if (info.m_typeId == WSingleton<CSharedDoc>::Instance()->m_equipItem[i]) {
					equipped++;
				}
			}

			if (info.m_count - equipped > 0) {
				m_itemSlot->AddItem(&info);
			}
		} else if (IsLocalContent((localContentType_t)0x84)) {
			WSingleton<CCardManager>::Instance()->SetPlayerIndex(0xFF);
			WSingleton<CCardManager>::Instance()->CalcCardPeriodAndStatus();

			int slotCount = WSingleton<CSharedDoc>::Instance()->GetNumAddItemSlotByMascot() + 8;

			int cardSlot = WSingleton<CCardManager>::Instance()->GetCardPeriodSlot();

			if (cardSlot > 0) {
				slotCount += cardSlot;
			}

			if (info.m_typeId & 0x2000000) {
				continue;
			}

			for (int i = 0; i < 10; i++) {
				if (i < slotCount) {
					if (info.m_typeId == WSingleton<CSharedDoc>::Instance()->m_equipItem[i]) {
						equipped++;
					}
				} else if (info.m_typeId == WSingleton<CSharedDoc>::Instance()->m_equipItem[i]) {
					// An item left in a slot the shopper no longer has.
					WSingleton<CSharedDoc>::Instance()->m_equipItem[i] = 0;
				}
			}

			if (info.m_count - equipped > 0) {
				m_itemSlot->AddItem(&info);
			}
		} else {
			if (info.m_typeId & 0x2000000) {
				continue;
			}

			for (int i = 0; i < 10; i++) {
				if (info.m_typeId == WSingleton<CSharedDoc>::Instance()->m_equipItem[i]) {
					equipped++;
				}
			}

			if (info.m_count - equipped > 0) {
				m_itemSlot->AddItem(&info);
			}
		}
	}
}

// shopmain.cpp:3802
void CShopMain::OnItemSlotInit(intptr_t param) {
	m_itemSlot = DYNAMIC_CAST(FrListBox, param);
	if (m_itemSlot) {
		m_itemSlot->SetVisible(false);
		m_itemSlot->SetMouseEvent(false);
	}
}

// shopmain.cpp:3812
void CShopMain::OnItemSlotOwnerDraw(PG_MSGARG param) {
	FrListItem *listItem = (FrListItem *)param;
	if (listItem == NULL)
		return;

	FrGraphicInterface *graphic = g_pFresh->GetManager()->GetGDI();
	if (graphic == NULL)
		return;

	const Bitmap *slot = m_itemSlotNormal;
	if (slot) {
		graphic->DrawTexture(slot, WRect(listItem->pos.x, listItem->pos.y, (float)m_itemSlotNormal->Width(), (float)m_itemSlotNormal->Height()), 0xFFFFFFFF, 0);
	}

	sItemInfo *itemInfo = (sItemInfo *)listItem->pData;
	if (itemInfo == NULL)
		return;

	IFF_STRUCT::sItem *item = ItemManager()->FindItem(itemInfo->m_typeId);
	if (item == NULL)
		return;

	const Bitmap *icon = g_pFresh->GetBitmap(item->m_icon);
	if (icon) {
		graphic->DrawTexture(icon,
			WRect(listItem->pos.x + 2.0f, listItem->pos.y + 4.0f, (float)icon->Width(), (float)icon->Height()), 0xFFFFFFFF, 0);
	}

	int equipped = 0;
	if ((itemInfo->m_typeId & 0x2000000) == 0) {
		for (int i = 0; i < 10; i++) {
			if (itemInfo->m_typeId == WSingleton<CSharedDoc>::Instance()->m_equipItem[i])
				equipped++;
		}
	}

	int count = itemInfo->m_count - equipped;
	Print(listItem->pos.x + 45.0f, listItem->pos.y + 40.0f, MakeStr("%d", count), 2.0f, 2, 1.0f, 0xFFFFFFFF);
}

// shopmain.cpp:3863
void CShopMain::OnPowerEditInit(PG_MSGARG param) {
	m_statEdit[0] = DYNAMIC_CAST(FrStatic, param);
}

// shopmain.cpp:3868
void CShopMain::OnControlEditInit(PG_MSGARG param) {
	m_statEdit[1] = DYNAMIC_CAST(FrStatic, param);
}

// shopmain.cpp:3873
void CShopMain::OnImpactEditInit(PG_MSGARG param) {
	m_statEdit[2] = DYNAMIC_CAST(FrStatic, param);
}

// shopmain.cpp:3878
void CShopMain::OnSpinEditInit(PG_MSGARG param) {
	m_statEdit[3] = DYNAMIC_CAST(FrStatic, param);
}

// shopmain.cpp:3883
void CShopMain::OnCurveEditInit(PG_MSGARG param) {
	m_statEdit[4] = DYNAMIC_CAST(FrStatic, param);
}

// shopmain.cpp:3893
bool CShopMain::OnBuyDlgResult(int result, FrForm *form) {
	if (result == 1) {

		for (FrListBox::ITEM_LIST::iterator i = m_cartList->m_itemList.begin(); i != m_cartList->m_itemList.end(); ++i) {

			IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)(*i)->pData;

			if ((item->m_typeId & 0xFC000000) == 0x08000000 && IsUccClothes(item->m_typeId)) {

				RestoreShoppersOldItem(item->m_typeId, WSingleton<CSharedDoc>::Instance()->GetMyPartTidList());
			}
		}

		ResetItemList(false);
		ClearCartList(false, NULL);
		m_shoppersOldItems.clear();
	}

	m_buyDlg = NULL;
	return true;
}

// shopmain.cpp:3917
bool CShopMain::OnGiftDlgResult(int result, FrForm *form) {
	m_giftDlg = NULL;
	return true;
}

// shopmain.cpp:3931
bool CShopMain::OnSellDlgResult(int result, FrForm *form) {
	if (result == 1) {
		FrRepayDlg *dlg = DYNAMIC_CAST(FrRepayDlg, form);
		if (dlg) {
			MsgObject msg(NULL, 1, 0, 0, 0, 0, 0);
			if (this) {
				HandleMsg(msg);
			}

			int kind;
			switch (dlg->GetTid() >> 26) {
			case 1:
				kind = 0;
				break;
			case 7:
				kind = 1;
				break;
			default:
				kind = 2;
				break;
			}

			WSendPacket packet(GCT_REQUEST_SELL_ITEM);
			packet.Encode1(kind);
			packet.Encode1(1);
			packet.Encode4(dlg->GetTid());
			packet.Encode4(dlg->GetGuid());
			packet.Send(SEND_TO_DEFAULT);
		}
	}

	return true;
}

// shopmain.cpp:3961
bool CShopMain::OnReemployeeResult(int result, FrForm *form) {
	if (result == 1) {
		CSharedDoc *doc = WSingleton<CSharedDoc>::Instance();
		for (std::map<unsigned int, sCaddieInfo>::iterator i = doc->m_caddieMap.begin(); i != doc->m_caddieMap.end(); ++i) {
		}
	}

	return true;
}

// shopmain.cpp:3987
CShopMain::CShopMain(void) {
	if (IsLocalContent((localContentType_t)0x51)) {
		m_cardTypeId = 0;
		LoadCardImages();
	}

	m_cutinImgs.clear();
	InitalizeUpgradeCaddieInfo();
	BuildVisibleCaddieList();
}

// shopmain.cpp:4007
CShopMain::~CShopMain(void) {
	if (IsLocalContent((localContentType_t)0x51)) {
		DeleteCardImages();
	}

	DeleteCutinImages();

	HidePet(true);

	g_resrcmng->UnlockAllByThread(m_anotherHand.GetThreadID());
	m_anotherHand.BreakWorkingThread();
	this << MsgObject(0, 2, 0, 0, 0, 0, 0);
}

// shopmain.cpp:4027
void CShopMain::OnPreLoadInit(void) {
	CTaskMain::OnPreLoadInit();

	InitShopControls();
	InitShopDialogs();

	m_selItemExpanded = false;
	m_caddieTypeId = 0x1C000000;
	m_standPetTypeId = 0;
	m_mascotTypeId = 0x40000000;
	m_clubsetTypeId = 0x0C000000;
	m_aztecTypeId = 0x14000000;
	m_charLevels = NULL;
	m_charParts = NULL;
	m_charCards = NULL;
	m_hairColor = 0;
	m_rbuttonPartTypeIds.clear();
	m_statTab = STAT_TAB_PART;
	m_shopTab = 0;
	m_subTab = (subTab_t)1;

	m_sortOptionCombo = NULL;
	m_searchGoodsButton = NULL;
}

// shopmain.cpp:4070
void CShopMain::OnLoad(void) {
	CTaskMain::OnLoad();
	LoadShopImages();
}

// shopmain.cpp:4077
void CShopMain::OnInit(void) {
	CTaskMain::OnInit();
}

// shopmain.cpp:4085
void CShopMain::OnDestroy(void) {
	CTaskMain::OnDestroy();
}

// shopmain.cpp:4091
void CShopMain::OnProcess(float dt) {
	CTaskMain::OnProcess(dt);

	if (m_anotherHand.IsWorking(0)) {
		Sleep(10);
	} else {
		m_anotherHand.Enter();

		bool flag;
		switch (m_shopTab) {
		case 1:
		case 4:
			flag = false;
			break;
		default:
			flag = true;
			break;
		}

		if (m_petExhibition) {
			m_petExhibition->Process(dt, flag);
		}

		m_anotherHand.Leave();
	}

	if (m_hintTime > 0.0f) {
		m_hintTime -= dt;

		if (m_hintTime < 0.1f) {
			m_hintTime = -10.0f;
			if (m_hintWnd) {
				m_hintWnd->SetVisible(false);
			}
		}
	}

	if (m_buyDlg) {
		((FrCheckoutDlg *)m_buyDlg)->Process();
	}

	if (m_shopTab == 4 || m_shopTab == 5) {
		for (int i = 0; i < 2; i++) {
			float speed = (i == 0) ? 138.0f : 228.0f;

			switch (m_sideDir[i]) {
			case 0:
				m_sidePos[i] = 0.0f;
				break;
			case 1:
				m_sidePos[i] = m_sidePos[i] - speed * dt * 0.25f;
				break;
			case 2:
				m_sidePos[i] = m_sidePos[i] + speed * dt * 0.25f;
				break;
			}
		}
	}

	if (g_input->GetDown("ESCAPE", true) == 1 && !g_pFresh->GetManager()->HasEscKeyWindow()) {
		CMessengerInfo::Instance()->Close(false);
	}

	if (!m_rbuttonPartTypeIds.empty()) {
		WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

		for (std::list<unsigned long>::iterator i = m_rbuttonPartTypeIds.begin(); i != m_rbuttonPartTypeIds.end(); ++i) {
			UnselectItemList(*i);
		}
		m_rbuttonPartTypeIds.clear();

		if (m_shopTab == 0 || m_shopTab == 4) {
			m_buyAll->Enable(!m_cartList->m_itemList.empty() || !m_auxCartList->m_itemList.empty());
			m_buyReset->Enable(!m_cartList->m_itemList.empty() || !m_auxCartList->m_itemList.empty());
		}
	}

	int tip = CIconToolTip::GetInstance()->GetCurTipIndex();
	if (tip == 0x16 || tip == 0x17) {
		OnBarHoverOff(0);
	}

	m_blinkTime += dt;
	m_blinkValue = (float)fabs(sin(m_blinkTime * 4.0f)) * 5.0f;
}

// shopmain.cpp:4199
void CShopMain::HandleMsg(const MsgObject &msg) {
	switch (msg.m_msg) {
	case 0x6A: {
		if (m_shopTab != msg.m_param1)
			break;

		std::map<unsigned int, sCharacterInfo>::const_iterator it = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin();
		for (; it != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++it) {
			if (m_standPetTypeId == it->second.typeId) {
				if (WSingleton<CSharedDoc>::Instance()->m_myCharacterId != it->second.id) {
					WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
					packet.Encode1(5);
					packet.Encode4(it->second.id);
					packet.Send(SEND_TO_DEFAULT);
				}
				break;
			}
		}

		if (it == WSingleton<CSharedDoc>::Instance()->m_characterMap.end())
			break;

		IFF_STRUCT::sChar *character = ItemManager()->FindChar(m_standPetTypeId);
		if (!character)
			break;

		sCharacterInfo info = it->second;
		bool changed = false;

		for (int i = 0; i < character->m_numParts + character->m_numExtraParts; i++) {
			if (info.partTypeIds[i] == m_charParts[i])
				continue;

			IFF_STRUCT::sPart *part = ItemManager()->FindPart(m_charParts[i]);
			if (part) {
				unsigned char level = WSingleton<CSharedDoc>::Instance()->m_myLevel;

				if ((part->m_level >> 7) == 0 && (part->m_level & 0x7F) > level)
					continue;
				if ((part->m_level >> 7) == 1 && (part->m_level & 0x7F) < level)
					continue;
			}

			if (IsUccClothes(m_charParts[i]))
				continue;

			info.partTypeIds[i] = m_charParts[i];
			info.partIds[i] = m_charPartIds[i];
			changed = true;
		}

		for (int i = character->m_numParts + character->m_numExtraParts; i < 24; i++)
			info.partTypeIds[i] = 0;

		for (int i = 0; i < 5; i++) {
			if (info.auxParts.tids[i] == m_charCards[i])
				continue;

			IFF_STRUCT::sAuxPart *auxPart = ItemManager()->FindAuxPart(m_charCards[i]);
			if (auxPart && (auxPart->m_level & 0x7F) > WSingleton<CSharedDoc>::Instance()->m_myLevel)
				continue;

			info.auxParts.tids[i] = m_charCards[i];
			changed = true;
		}

		if (info.hairColor != m_hairColor) {
			info.hairColor = m_hairColor;
			changed = true;
		}

		if (changed) {
			WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
			packet.Encode1(0);
			packet.EncodeBuffer(&info, sizeof(info));
			packet.Send(SEND_TO_DEFAULT);
		}
		break;
	}

	case 0xC4:
		if (msg.m_param1 < 6) {
			m_shopTab = msg.m_param1;
			if (msg.m_param2 < 0x24)
				m_subTab = (subTab_t)msg.m_param2;
			if (m_shopTab == 0 && m_subTab > 5)
				m_subTab = SUB_TAB_NONE;
		} else if (msg.m_param1 == 6) {
			m_shopTab = 5;
			m_subTab = (subTab_t)0x20;
		} else {
			m_shopTab = 0;
			m_subTab = SUB_TAB_NONE;
		}
		break;

	case 0xC3:
		*(int *)msg.m_param1 = m_shopTab;
		*(subTab_t *)msg.m_param2 = m_subTab;
		break;

	case 0:
		OpenLayout((const char *)msg.m_param1);
		break;

	case 0xC6:
		if (msg.m_param1 == 3) {
			m_shopTab = 0;
			m_subTab = (subTab_t)0x1A;
			ChangeMainTab((mainTab_t)0, true);
			ChangeSubTab((subTab_t)0x1A, true);

			unsigned long typeId = msg.m_param2;
			if (!typeId)
				typeId = m_standPetTypeId;

			ChangeCharacter(typeId);
		} else if (msg.m_param1 == 4) {
			if (IsLocalContent((localContentType_t)0x51)) {
				m_shopTab = 1;
				m_subTab = (subTab_t)0x1B;
				ChangeMainTab((mainTab_t)1, true);
				ChangeSubTab((subTab_t)0x1B, true);
				m_anotherHand.WaitUntilWorkDone(9999); if (m_petExhibition) { delete m_petExhibition; m_petExhibition = NULL; }
			}
		}
		break;

	case 0x49: {
		const sCharacterInfo *character = (const sCharacterInfo *)msg.m_param2;
		CTask *task = WSingleton<CTaskManager>::Instance()->GetPreservedTask();
		if (task && task->GetActor("AvatarChat")) {
			task->GetActor("AvatarChat") << MsgObject(0, 0x49, msg.m_param1, msg.m_param2, 0, 0, 0);
		} else {
			WSingleton<CSharedDoc>::Instance()->m_myCharacterId = character->id;
		}
		ChangeCharacter(character->typeId);
		break;
	}

	case 0x4A: {
		const sCaddieInfo *caddie = (const sCaddieInfo *)msg.m_param2;
		if (caddie->typeId)
			ChangeCaddie(caddie->typeId);
		WSingleton<CSharedDoc>::Instance()->m_myCaddieId = caddie->id;
		break;
	}

	case 0x4B: {
		const sItemInfo *clubSet = (const sItemInfo *)msg.m_param2;
		ChangeClubset(clubSet->m_typeId);
		WSingleton<CSharedDoc>::Instance()->m_myClubId = clubSet->m_serial;
		break;
	}

	case 0x4C:
		ChangeAztec(msg.m_param2);
		WSingleton<CSharedDoc>::Instance()->m_myBallId = msg.m_param2;
		break;

	case 0x4E: {
		const sMascotInfo *mascot = (const sMascotInfo *)msg.m_param2;
		if (mascot->typeId)
			ChangeMascot(mascot->typeId);
		WSingleton<CSharedDoc>::Instance()->m_myMascotId = mascot->id;
		break;
	}

	case 0xB5: {
		unsigned long id = msg.m_param1;
		unsigned long typeId = msg.m_param2;
		sCharacterInfo info;
		memset(&info, 0, sizeof(info));
		info.id = id;
		info.typeId = typeId;
		ItemManager()->GetDefCombo(info.typeId, info.partTypeIds);

		WSingleton<CSharedDoc>::Instance()->m_characterMap.insert(std::map<unsigned int, sCharacterInfo>::value_type(info.id, info));
		WSingleton<CSharedDoc>::Instance()->m_myCharacterId = info.id;
		break;
	}

	case 0xB6: {
		sItemInfo item;
		memset(&item, 0, sizeof(item));
		item.m_serial = msg.m_param1;
		item.m_typeId = msg.m_param2;
		if (msg.m_param4 == 6 || msg.m_param4 == 2) {
			item.m_remainHours = msg.m_param3 * 24;
			item.m_stateBits.kind = msg.m_param4;
		}

		int slot = (item.m_typeId >> 13) & 0x1F;
		m_charPartIds[slot] = item.m_serial;

		if (IsUccClothes(item.m_typeId)) {
			strncpy(item.m_uccKey, ((const sBoughtItem *)msg.m_param5)->uccKey, 9);
			item.m_uccSeq = 1;
			WSingleton<CSharedDoc>::Instance()->m_myItemList.push_back(item);
		} else {
			WSingleton<CSharedDoc>::Instance()->AddMyPartsList(item);
		}
		break;
	}

	case 0xB7: {
		unsigned long id = msg.m_param1;
		unsigned long typeId = msg.m_param2;
		sCaddieInfo caddie;
		memset(&caddie, 0, sizeof(caddie));
		caddie.id = id;
		caddie.typeId = typeId;
		caddie.flags |= 2;
		caddie.days = 30;

		if (caddie.typeId >= 0x1C00000C && caddie.typeId <= 0x1C00000E) {
			caddie.level = (unsigned char)msg.m_param4;
			caddie.experience = msg.m_param3;
		}

		WSingleton<CSharedDoc>::Instance()->m_caddieMap.insert(std::map<unsigned int, sCaddieInfo>::value_type(caddie.id, caddie));

		m_caddieTypeId = caddie.typeId;
		BuildVisibleCaddieList();
		break;
	}

	case 0xB8: {
		unsigned int caddieId = msg.m_param1;
		unsigned long cadItemTypeId = msg.m_param2;
		unsigned long hours = msg.m_param3;
		std::map<unsigned int, sCaddieInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_caddieMap.find(caddieId);
		if (it == WSingleton<CSharedDoc>::Instance()->m_caddieMap.end())
			break;

		if (!ItemManager()->FindCadItem(cadItemTypeId))
			break;

		sCaddieInfo &info = it->second;
		unsigned char days = (unsigned short)hours / 24;
		info.cadItemTypeId = cadItemTypeId;
		info.hours = (unsigned short)hours;
		if (info.days < days)
			info.days = days;
		break;
	}

	case 0xB9: {
		sItemInfo item;
		memset(&item, 0, sizeof(item));
		item.m_serial = msg.m_param1;
		item.m_typeId = msg.m_param2;
		int period = msg.m_param3;
		item.m_stateBits.kind = msg.m_param4;

		item.m_remainHours = period;
		item.m_count = ((const sBoughtItem *)msg.m_param5)->count;

		ItemManager()->FindItem(item.m_typeId);

		bool found = false;
		if (item.m_stateBits.kind) {
			switch (item.m_stateBits.kind) {
			case 3:
				period *= 1;
				break;
			case 4:
				period *= 24;
				item.m_remainHours = period;
				break;
			case 5:
				period *= 720;
				item.m_remainHours = period;
				break;
			}
		}

		if (!item.m_stateBits.kind) {
			for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {
				if (it->m_typeId == item.m_typeId) {
					it->m_remainHours += period;
					it->m_count = item.m_count;
					found = true;
				}
			}
		}

		if (item.m_stateBits.kind && period > 0)
			Net_StartPeriodItem(item.m_serial);

		if (!found)
			WSingleton<CSharedDoc>::Instance()->m_itemInfoList.push_back(item);
	}
	// fall through: 0xC5 is the tail of 0xB9
	case 0xC5:
		SetMyItemSlot();
		break;

	case 0xBA: {
		sItemInfo item;
		memset(&item, 0, sizeof(item));
		item.m_serial = msg.m_param1;
		item.m_typeId = msg.m_param2;

		WSingleton<CSharedDoc>::Instance()->m_myItemMap.insert(std::map<unsigned int, sItemInfo>::value_type(item.m_serial, item));
		break;
	}

	case 0xBB: {
		unsigned long id = msg.m_param1;
		const sBoughtItem *record = (const sBoughtItem *)msg.m_param5;
		unsigned long typeId = msg.m_param2;

		IFF_STRUCT::sBall *ball = ItemManager()->FindBall(typeId);
		if (!ball)
			break;

		std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myGearList.begin();
		for (; it != WSingleton<CSharedDoc>::Instance()->m_myGearList.end(); ++it) {
			if (it->m_typeId == typeId) {
				if (record->count > 0)
					it->m_count = record->count;
				else
					it->m_count += ball->quantity;
				break;
			}
		}

		if (it != WSingleton<CSharedDoc>::Instance()->m_myGearList.end())
			break;

		sItemInfo item;
		memset(&item, 0, sizeof(item));
		item.m_serial = id;
		item.m_typeId = typeId;
		item.m_count = record->count > 0 ? record->count : ball->quantity;

		WSingleton<CSharedDoc>::Instance()->m_myGearList.push_back(item);
		break;
	}

	case 0xBC: {
		sItemInfo item;
		memset(&item, 0, sizeof(item));
		item.m_serial = msg.m_param1;
		item.m_typeId = msg.m_param2;
		item.m_stateBits.kind = msg.m_param4;
		item.m_remainHours = msg.m_param3 * 24;

		WSingleton<CSharedDoc>::Instance()->m_mySkinList.push_back(item);

		std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_mySkinList.begin();
		for (; it != WSingleton<CSharedDoc>::Instance()->m_mySkinList.end(); ++it) {
		}
		break;
	}

	case 0xB0:
		break;

	case 3:
		if (!m_hintWnd)
			break;

		if (!m_hintWnd->IsVisible()) {
			g_audio->PlaySfx("notice_dingdong", 0, 0, 0, 0, 0.5f, 200.0f);
			m_hintWnd->SetVisible(true);
		}

		m_hintTime = 7.0f;
		m_hintWnd->AddLine((const char *)msg.m_param1, WSingleton<CChatMsg>::Instance()->ConvertToUIColor(msg.m_param2), false);
		break;

	case 0x241: {
		FrWnd *notify = g_pFresh->GetManager()->GetDesktop()->FindChildForm("notify");
		if (!notify)
			break;

		((FrForm *)notify)->SetMessage((const char *)msg.m_param1, false);
		break;
	}

	case 0xC7:
		if (m_notifyOneDlg)
			m_notifyOneDlg->Close(FrOK, true);
		break;

	case 0xBD: {
		IFF_STRUCT::sHairStyle *hair = ItemManager()->FindHairStyle(msg.m_param2);
		if (!hair)
			break;

		for (std::map<unsigned int, sCharacterInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++it) {
			if ((it->second.typeId & 0x3FFFFFF) == hair->m_style) {
				it->second.hairColor = hair->m_color;
				break;
			}
		}
		break;
	}

	case 0xBE: {
		sItemInfo item;
		memset(&item, 0, sizeof(item));
		item.m_serial = msg.m_param1;
		item.m_typeId = msg.m_param2;
		int period = msg.m_param3;
		item.m_stateBits.kind = msg.m_param4;

		item.m_remainHours = period;
		item.m_count = ((const sBoughtItem *)msg.m_param5)->count;

		ItemManager()->FindItem(item.m_typeId);

		bool found = false;
		if (item.m_stateBits.kind) {
			switch (item.m_stateBits.kind) {
			case 3:
				period *= 1;
				break;
			case 4:
				period *= 24;
				item.m_remainHours = period;
				break;
			case 5:
				period *= 720;
				item.m_remainHours = period;
				break;
			}
		}

		if (!item.m_stateBits.kind) {
			for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3.end(); ++it) {
				if (it->m_typeId == item.m_typeId) {
					it->m_remainHours += period;
					it->m_count = item.m_count;
					found = true;
				}
			}
		}

		if (item.m_stateBits.kind && period > 0)
			Net_StartPeriodItem(item.m_serial);

		if (!found)
			WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3.push_back(item);
		break;
	}

	case 0xBF: {
		sMascotInfo mascot;
		memset(&mascot, 0, sizeof(mascot));
		mascot.id = msg.m_param1;
		mascot.typeId = msg.m_param2;
		strcpy(mascot.message, "PANGYA!");
		const sBoughtItem *bought = (const sBoughtItem *)msg.m_param5;
		mascot.remainHours = bought->hours;
		mascot.endTime = bought->endTime;

		WSingleton<CSharedDoc>::Instance()->AddMascot(&mascot);
		break;
	}

	case 0x218: {
		unsigned long typeId = msg.m_param2;
		unsigned long uid = msg.m_param1;

		const sBoughtItem *bought = (const sBoughtItem *)msg.m_param5;
		int count = bought->count > 0 ? bought->count : 1;

		if (!ItemManager()->FindCard(typeId))
			break;

		sCards card;
		memset(&card, 0, sizeof(card));
		card.m_guid = uid;
		card.m_typeId = typeId;
		card.m_count = count;
		std::list<sCards>::iterator it = std::find_if(WSingleton<CSharedDoc>::Instance()->m_myCardList.begin(), WSingleton<CSharedDoc>::Instance()->m_myCardList.end(), _FindByUid(uid));
		if (it == WSingleton<CSharedDoc>::Instance()->m_myCardList.end()) {
			WSingleton<CSharedDoc>::Instance()->m_myCardList.push_back(card);
			break;
		}

		it->m_count = count;
		break;
	}

	case 0x1FE:
		this << MsgObject(NULL, 2, 0, 0, 0, 0, 0);
		break;

	case 0x1FF:
		if (m_giftDlg)
			m_giftDlg->GiftProgress();
		break;

	case 0x217: {
		if (!IsLocalContent((localContentType_t)0x56))
			break;

		unsigned long id = msg.m_param1;
		unsigned long typeId = msg.m_param2;

		IFF_STRUCT::sFurniture *furniture = ItemManager()->FindFurniture(typeId);
		if (!furniture)
			break;

		sFurniture_List item;
		memset(&item, 0, sizeof(item));
		item.m_id = id;
		item.m_typeId = typeId;
		item.m_pos.x = furniture->m_pos[0];
		item.m_pos.y = furniture->m_pos[1];
		item.m_pos.z = furniture->m_pos[2];
		item.m_angle = furniture->m_angle;
		item.m_flags &= ~1;

		WSingleton<CSharedDoc>::Instance()->AddFurniture(item);
		break;
	}

	case 0x24F: {
		FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		pForm->SetMessage((const char *)msg.m_param1, false);
		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
		pForm->SetTopmost(true);
		pForm->FindNextTopFocus(false);

		BuildVisibleCaddieList();
		ChangeCaddie(msg.m_param2);
		SetLevelBar();
		break;
	}

	case 0xC0: {
		// Sent with 0 when a purchase is sent (FrCheckoutDlg::OnConfirmResult)
		// and with 1 when its answer arrives (CShopTask::OnPacket 0x66, 0x68).
		bool enable = msg.m_param1 != 0;
		for (int i = 0; i < 7; i++)
			m_miniBtns[i].enable = enable;

		if (m_sideTabRental) {
			if (m_sideTabRental->GetStatus() == FrButton::PRESSED)
				m_miniBtns[2].enable = false;
		}
		break;
	}

	default:
		HandleMsgCommon(msg);
		break;
	}
}

// shopmain.cpp:5261
void CShopMain::OnAnotherHand(int nWork, void *pData) {
	if (nWork == 0) {
		ShowPet(*(unsigned long *)pData);
	}
}

// shopmain.cpp:5268
void CShopMain::OpenLayout(const char *layoutName) {
	WSendPacket packet(GCT_REQUEST_SERVER_TIME);
	packet.Send(SEND_TO_DEFAULT);

	WSendPacket packet2(GCT_REQUEST_CASH);
	packet2.Send(SEND_TO_DEFAULT);

	g_pFresh->OpenLayout(layoutName, this, false);
}

// shopmain.cpp:5288
void CShopMain::SetPetCtrl(unsigned long typeId) {
	WRect rect = m_petArea->GetRect();

	CExhibition *pet = NULL;
	bool loaded;
	switch (typeId >> 26) {
	case 1:
		if (m_subTab == 13 || m_subTab == 14 || m_subTab == 15) {
			pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.35f, true);
			pet->SetArea(rect.w, rect.h);

			pet->SetDistanceRange(16.0f, 20.0f); pet->SetDistance(18.0f, true, false);
			pet->SetControlFlag(15);
		} else {
			pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f + 15.0f, 0.35f, true);
			pet->SetArea(rect.w, rect.h);

			pet->SetDistanceRange(8.0f, 12.0f); pet->SetDistance(10.0f, true, false);
		}

		loaded = pet->SetModel(WSingleton<CSharedDoc>::Instance()->GetMyPartTidList(), WSingleton<CSharedDoc>::Instance()->GetMyAuxPartTidList(), NULL, 3.0f, 0);
		break;

	case 7:
	case 8:
		pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f + 15.0f, 0.35f, true);
		pet->SetArea(rect.w, rect.h);

		pet->SetDistanceRange(9.0f, 13.0f); pet->SetDistance(11.0f, true, false);
		loaded = pet->SetModel(typeId);
		break;

	case 5:
		pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.0f, true);
		pet->SetArea(rect.w, rect.h);
		pet->SetDistance(1.0f, true, false);
		loaded = pet->SetModel(typeId);
		break;

	case 4:
		pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.0f, true);
		pet->SetArea(rect.w, rect.h);
		pet->SetDistance(6.0f, true, false);
		loaded = pet->SetModel(typeId);
		break;

	case 16:
		pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f + 15.0f, 0.35f, true);
		pet->SetArea(rect.w, rect.h);

		pet->SetDistanceRange(4.0f, 8.0f); pet->SetDistance(6.0f, true, false);
		loaded = pet->SetModel(typeId);

		{
			IFF_STRUCT::sMascot *mascot = ItemManager()->FindMascot(typeId);
			CMascot::ChangeMascotMsgInTex("\xB8\xDE\xBC\xBC\xC1\xF6", mascot, pet->GetModel(), "default");
		}
		break;

	case 6:
		pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.0f, true);
		pet->SetArea(rect.w, rect.h);
		pet->SetDistance(6.0f, true, false);
		loaded = pet->SetModel(typeId);
		break;

	case 31: {
		IFF_STRUCT::sCard *card = ItemManager()->FindCard(typeId);
		if (card) {
			if ((typeId & 0x3C00000) == 0x1000000) {
				pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f - 150.0f, 0.35f, true);
				pet->SetArea(rect.w, rect.h);

				pet->SetDistanceRange(4.0f, 8.0f); pet->SetDistance(6.0f, true, false);
			} else {
				pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.0f, true);
				pet->SetArea(rect.w, rect.h);

				pet->SetDistanceRange(4.0f, 8.0f); pet->SetDistance(5.0f, true, false);
			}
			loaded = pet->SetModel(MakeStr("%s", card->m_model), (eExhibitType)12);

			pet->SetMotion("default", true, 0.0f);
		}
		break;
	}

	case 9: {
		unsigned long kind = (typeId >> 21) & 0x1F;
		if (kind == 6 || kind == 5 || kind == 4 || kind == 8) {
			pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.0f, true);
			pet->SetArea(rect.w, rect.h);
			pet->SetDistance(6.0f, true, false);
			loaded = pet->SetModel(typeId);
		}
		break;
	}

	case 18:
		if (IsLocalContent((localContentType_t)0x56)) {
			pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.0f, true);
			pet->SetArea(rect.w * 0.5f, rect.h * 0.5f);
			pet->SetDistance(15.0f, true, false);
			loaded = pet->SetModel(typeId);
		}
		break;

	default:
		return;
	}

	if (!loaded) {
		delete pet;
		return;
	}

	m_petExhibition = pet;
}

// shopmain.cpp:5419
void CShopMain::ShowPet(unsigned long typeId) {
	if (typeId) {
		m_anotherHand.Enter();

		if (m_petArea == NULL) {
			m_anotherHand.Leave();
			return;
		}

		if (m_petExhibition) { delete m_petExhibition; m_petExhibition = NULL; }

		SetPetCtrl(typeId);

		m_anotherHand.Leave();
	}
}

// shopmain.cpp:5439
// 98%: cl puts this in edi and the exhibition in esi (the original swaps
// them); a temporary, a do-while(0) guard and `== true` did not move it.
void CShopMain::HidePet(bool bWait) {
	if (bWait) {
		m_anotherHand.WaitUntilWorkDone(9999);
	}

	if (m_petExhibition) { delete m_petExhibition; m_petExhibition = NULL; }
}

// shopmain.cpp:5459
void CShopMain::LoadItemImages(void) {
	for (std::map<unsigned int, IFF_STRUCT::sChar>::const_iterator it = ItemManager()->m_charMap.begin(); it != ItemManager()->m_charMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIcons.find(typeId) == m_itemIcons.end()) {
			const Bitmap *bitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", it->second.m_icon);
			if (bitmap) {
				m_itemIcons.insert(std::map<unsigned long, const Bitmap *>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sPart>::iterator it = ItemManager()->m_partMap.begin(); it != ItemManager()->m_partMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIcons.find(typeId) == m_itemIcons.end() && !(typeId & 0x600)) {
			const Bitmap *bitmap = g_pFresh->GetManager()->GetBitmap("ITEMS_FASHION", it->second.m_icon);
			if (bitmap) {
				m_itemIcons.insert(std::map<unsigned long, const Bitmap *>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sCaddie>::iterator it = ItemManager()->m_caddieMap.begin(); it != ItemManager()->m_caddieMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIcons.find(typeId) == m_itemIcons.end()) {
			const Bitmap *bitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", it->second.m_icon);
			if (bitmap) {
				m_itemIcons.insert(std::map<unsigned long, const Bitmap *>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sCadItem>::iterator it = ItemManager()->m_cadItemMap.begin(); it != ItemManager()->m_cadItemMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIcons.find(typeId) == m_itemIcons.end()) {
			const Bitmap *bitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", it->second.m_icon);
			if (bitmap) {
				m_itemIcons.insert(std::map<unsigned long, const Bitmap *>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sItem>::iterator it = ItemManager()->m_itemMap.begin(); it != ItemManager()->m_itemMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIcons.find(typeId) == m_itemIcons.end()) {
			const Bitmap *bitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", it->second.m_icon);
			if (bitmap) {
				m_itemIcons.insert(std::map<unsigned long, const Bitmap *>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sClubSet>::iterator it = ItemManager()->m_clubSetMap.begin(); it != ItemManager()->m_clubSetMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIcons.find(typeId) == m_itemIcons.end()) {
			const Bitmap *bitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", it->second.m_icon);
			if (bitmap) {
				m_itemIcons.insert(std::map<unsigned long, const Bitmap *>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sBall>::iterator it = ItemManager()->m_ballMap.begin(); it != ItemManager()->m_ballMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIcons.find(typeId) == m_itemIcons.end()) {
			const Bitmap *bitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", it->second.m_icon);
			if (bitmap) {
				m_itemIcons.insert(std::map<unsigned long, const Bitmap *>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sSetItem>::iterator it = ItemManager()->m_setItemMap.begin(); it != ItemManager()->m_setItemMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIcons.find(typeId) == m_itemIcons.end()) {
			const Bitmap *bitmap = g_pFresh->GetManager()->GetBitmap("ITEMS_FASHION", it->second.m_icon);
			if (bitmap) {
				m_itemIcons.insert(std::map<unsigned long, const Bitmap *>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sSkin>::iterator it = ItemManager()->m_skinMap.begin(); it != ItemManager()->m_skinMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIcons.find(typeId) == m_itemIcons.end()) {
			const Bitmap *bitmap = g_pFresh->GetManager()->GetBitmap("SKINS", it->second.m_icon);
			if (bitmap) {
				m_itemIcons.insert(std::map<unsigned long, const Bitmap *>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sHairStyle>::iterator it = ItemManager()->m_hairStyleMap.begin(); it != ItemManager()->m_hairStyleMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIcons.find(typeId) == m_itemIcons.end()) {
			const Bitmap *bitmap = g_pFresh->GetManager()->GetBitmap("HAIRS", it->second.m_icon);
			if (bitmap) {
				m_itemIcons.insert(std::map<unsigned long, const Bitmap *>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sMascot>::iterator it = ItemManager()->m_mascotMap.begin(); it != ItemManager()->m_mascotMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIcons.find(typeId) == m_itemIcons.end()) {
			const Bitmap *bitmap = g_pFresh->GetManager()->GetBitmap("MASCOTS", it->second.m_icon);
			if (bitmap) {
				m_itemIcons.insert(std::map<unsigned long, const Bitmap *>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}
}

// shopmain.cpp:5595
void CShopMain::LoadShopImages(void) {
	m_markNew = g_pFresh->RegisterBitmap("mark_new");
	m_markHot = g_pFresh->RegisterBitmap("mark_hot");
	m_markLimited = g_pFresh->RegisterBitmap("mark_lim");
	m_markBegin = g_pFresh->RegisterBitmap("mark_begin");

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 2; j++) {
			m_dotBitmaps[i][j] = g_pFresh->RegisterBitmap(dot_files[i][j]);
		}
	}

	m_selectUp = g_pFresh->RegisterBitmap("select_up");
	m_selectMid = g_pFresh->RegisterBitmap("select_mid");
	m_selectLow = g_pFresh->RegisterBitmap("select_low");
	m_selectEmpty = g_pFresh->RegisterBitmap("select_empty");

	m_selectCursor1 = g_pFresh->RegisterBitmap("select_cursor1");
	m_selectCursor0 = g_pFresh->RegisterBitmap("select_cursor0");

	m_statPartBitmap[0][0] = g_pFresh->RegisterBitmap("stat_char_n");
	m_statPartBitmap[0][1] = g_pFresh->RegisterBitmap("stat_char_o");
	m_statPartBitmap[0][2] = g_pFresh->RegisterBitmap("stat_char_d");
	m_statPartBitmap[1][0] = g_pFresh->RegisterBitmap("stat_club_n");
	m_statPartBitmap[1][1] = g_pFresh->RegisterBitmap("stat_club_o");
	m_statPartBitmap[1][2] = g_pFresh->RegisterBitmap("stat_club_d");
	m_statPartBitmap[2][0] = g_pFresh->RegisterBitmap("stat_caddie_n");
	m_statPartBitmap[2][1] = g_pFresh->RegisterBitmap("stat_caddie_o");
	m_statPartBitmap[2][2] = g_pFresh->RegisterBitmap("stat_caddie_d");
	m_statAllBitmap[0] = g_pFresh->RegisterBitmap("stat_all_none");

	m_statAllBitmap[1] = g_pFresh->RegisterBitmap("stat_all_o");
	m_statAllBitmap[2] = g_pFresh->RegisterBitmap("stat_all_d");

	for (int i = 0; i < 5; i++) {
		m_sicons[i] = g_pFresh->RegisterBitmap(MakeStr("sicon%d", i + 1));
	}
	m_itFa02 = g_pFresh->RegisterBitmap("it_fa02");
	m_cancelBtnNormal = g_pFresh->RegisterBitmap("cancel_btn_n");
	m_cancelBtnOver = g_pFresh->RegisterBitmap("cancel_btn_o");

	m_miniBtns[1].normal = g_pFresh->RegisterBitmap("buy_btn_n");
	m_miniBtns[1].over = g_pFresh->RegisterBitmap("buy_btn_o");

	m_miniBtns[1].rect = WRect(144.0f, 66.0f, m_miniBtns[1].normal->Width(), m_miniBtns[1].normal->Height());

	m_miniBtns[1].enable = true;

	m_miniBtns[2].normal = g_pFresh->RegisterBitmap("gf_btn_n");
	m_miniBtns[2].over = g_pFresh->RegisterBitmap("gf_btn_o");

	m_miniBtns[2].rect = WRect(103.0f, 66.0f, m_miniBtns[2].normal->Width(), m_miniBtns[2].normal->Height());

	m_miniBtns[2].enable = true;

	m_itemBaseNormal = g_pFresh->RegisterBitmap("item_base_n");
	m_itemBaseOver = g_pFresh->RegisterBitmap("item_base_o");
	m_itemBaseDown = g_pFresh->RegisterBitmap("item_base_d");
	m_itemBaseEmpty = g_pFresh->RegisterBitmap("item_base_empty");
	m_itemBaseAward = g_pFresh->RegisterBitmap("item_base_award");

	m_iconSex[0] = g_pFresh->RegisterBitmap("i_male");
	m_iconSex[1] = g_pFresh->RegisterBitmap("i_female");
	m_masterSign = g_pFresh->RegisterBitmap("master_sign");
	m_teamBase = g_pFresh->RegisterBitmap("team_base");

	m_selectionArrow = g_pFresh->RegisterBitmap("selection_arrow");

	m_itemSlotNormal = g_pFresh->RegisterBitmap("item_slot_n");

	m_itemDescBitmap[0] = g_pFresh->RegisterBitmap("itemdesc_01");
	m_itemDescBitmap[1] = g_pFresh->RegisterBitmap("itemdesc_02");
}

// shopmain.cpp:5685
bool CShopMain::HaveCharacter(unsigned long typeId) {
	const std::map<unsigned int, sCharacterInfo> &characters = WSingleton<CSharedDoc>::Instance()->m_characterMap;
	for (std::map<unsigned int, sCharacterInfo>::const_iterator i = characters.begin(); i != characters.end(); ++i) {
		if (typeId == i->second.typeId) {
			return true;
		}
	}

	return false;
}

// shopmain.cpp:5698
void CShopMain::BuildDisplayStand(subTab_t tab) {
	if (m_shopTab == 0) {
		if (m_sideTabBuy) m_sideTabBuy->SetVisible(true); if (m_sideTabRental) m_sideTabRental->SetVisible(true);
		if (m_shopTab == 0 && m_sideTabRental && m_sideTabRental->GetStatus() == FrButton::PRESSED) {
			BuildDisplayRental(tab);
			return;
		}
	} else {
		if (m_sideTabBuy) m_sideTabBuy->SetVisible(false); if (m_sideTabRental) m_sideTabRental->SetVisible(false);
	}

	m_itemListBox->ClearItem();

	switch (tab) {
	case 0:
		Build_Package(tab, 0);
		break;
	case 5:
		Build_Hair(tab, 0);
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 0x1A:
		Build_Parts(tab, 0);
		break;
	case 6:
		Build_Clubset(tab, 0);
		break;
	case 7:
		Build_Aztec(tab, 0);
		break;
	case 8:
		Build_Equip(tab, 0);
		break;
	case 9:
		Build_NoEquip(tab, 0);
		break;
	case 0x1B:
		Build_Card(tab, 0);
		break;
	case 0x1C:
	case 0x1D:
	case 0x1E:
		Build_Interior(tab, 0);
		break;
	case 0x1F:
		Build_Furniture(tab, 0);
		break;
	case 0xD:
	case 0xE:
	case 0xF:
	case 0x10:
	case 0x22:
		Build_Decorate(tab, 0);
		break;
	case 0x21:
		Build_Cutin(tab, 0);
		break;
	case 0xA:
		Build_Caddie(tab, 0);
		break;
	case 0xC:
		Build_Mascot(tab, 0);
		break;
	}

	int count = m_itemListBox->GetCurrentItemSize(true);
	for (int i = count; i < 8; i++) {
		m_itemListBox->AddItem(NULL);
	}
}

// shopmain.cpp:6075
void CShopMain::Net_StartPeriodItem(unsigned long typeId) {
	WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
	packet.Encode1(7);
	packet.Encode4(typeId);
	packet.Send(SEND_TO_DEFAULT);
}

// shopmain.cpp:6083
void CShopMain::SetLevelBar(void) {
	for (int i = 0; i < 5; i++) {
		if (m_statBar[i]) {
			m_statBar[i]->SetDestPos(0, false);
			m_statBar[i]->SetExpand(0);
		}
	}

	switch (m_subTab) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 26:
		SetLevelBar_Char();
		break;
	case SUB_TAB_CLUBSET:
		SetLevelBar_Clubset();
		break;
	case 10: {
		IFF_STRUCT::sCaddie *caddie = ItemManager()->FindCaddie(m_caddieTypeId);
		if (caddie) {
			for (int i = 0; i < 5; i++) {
				unsigned char value = (unsigned char)caddie->m_levels[i];

				if (m_statBar[i]) {
					m_statBar[i]->SetDestPos(value, false);
				}

				if (m_statEdit[i]) {
					m_statEdit[i]->SetCaption11(MakeStatusString2(value, 0, 0));
				}
			}
		}

		m_statPartKind = 2;
		break;
	}
	default:
		break;
	}
}

// shopmain.cpp:6155
void CShopMain::OpenInformation(unsigned long typeId) {
	char text[32]; // unused: the original's frame carries a /GS cookie with no array

	if (FrStaticCast<FrWnd>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("information")))
		return;

	if (FrStaticCast<FrWnd>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("information_setitem")))
		return;

	IFF_ITEM_COMMON *item = ItemManager()->FindCommonItem(typeId);
	if (item == NULL)
		return;

	if ((item->m_typeId & 0xFC000000) == 0x24000000) {
		FrSetItemDlg *setDlg = CreateForm<FrSetItemDlg>(g_pFresh->GetManager(), this, "information_setitem", 0);
		if (setDlg == NULL)
			return;

		setDlg->SetSetItem(item->m_typeId);
		setDlg->Open(0, 3);
		return;
	}

	FrForm *form = CreateForm<FrForm>(g_pFresh->GetManager(), this, "information", 0);
	if (form == NULL)
		return;

	FrArea *portrait = DYNAMIC_CAST(FrArea, form->FindChildByName("portrait"));
	if (portrait) {
		portrait->SetBgImg(item->m_icon);
	}

	FrEdit *name = DYNAMIC_CAST(FrEdit, form->FindChildByName("name"));
	FrArea *level = DYNAMIC_CAST(FrArea, form->FindChildByName("level"));

	if (name && level) {
		const char *saleKind = "";
		if (item->m_typeId != 0x1C000080) {
			switch (item->m_flags0 >> 5) {
			case 2:
				saleKind = "(\xBC\xB1\xB9\xB0\xC0\xFC\xBF\xEB)";
				break;
			case 3:
				saleKind = "(\xB1\xB8\xC0\xD4\xC0\xFC\xBF\xEB)";
				break;
			case 4:
				saleKind = "(\xC0\xFC\xBD\xC3\xBF\xEB)";
				break;
			}
		}

		name->AddText(MakeStr("\xC0\xCC\xB8\xA7: %s\\c0xffff0000\\c%s\\c0xff000000\\c", item->m_name, saleKind), false, true);

		_SYSTEMTIME *saleBegin = NULL;
		_SYSTEMTIME *saleEnd = NULL;
		if (ItemManager()->HasSalePeriod(item->m_typeId, &saleBegin, &saleEnd)) {
			if (saleBegin->wYear != 0 && saleEnd->wYear != 0) {
				name->AddText(MakeStr("\\c0xffff0000\\c\xC6\xC7\xB8\xC5\xB1\xE2\xB0\xA3: %d\xBF\xF9 %d\xC0\xCF ~ %d\xBF\xF9 %d\xC0\xCF\\c0xff000000\\c",
					saleBegin->wMonth, saleBegin->wDay, saleEnd->wMonth, saleEnd->wDay), false, true);
			} else {
				name->AddText("\n", false, true);
			}
		} else {
			name->AddText("\n", false, true);
		}

		if (item->m_level & 0x80) {
			level->SetBgImg(MakeStr("level_%03d", (item->m_level & 0x7F) + 1));
			name->AddText(MakeStr("\xB7\xB9\xBA\xA7:                \xC0\xCC\xC7\xCF", item->m_name), false, true);
		} else {
			level->SetBgImg(MakeStr("level_%03d", (item->m_level & 0x7F) + 1));
			name->AddText(MakeStr("\xB7\xB9\xBA\xA7:                \xC0\xCC\xBB\xF3", item->m_name), false, true);
		}
	}

	if (IsLocalContent((localContentType_t)0x51)) {
		FrEdit *ability = DYNAMIC_CAST(FrEdit, form->FindChildByName("avility"));
		if (ability) {
			IFF_STRUCT::sPart *part = ItemManager()->FindPart(typeId);
			if (part) {
				for (int i = 0; i < 5; i++) {
					if (part->m_baseStat[i] != 0 || part->m_slotStat[i] != 0)
						ability->AddText(MakeStr("\\c0xff0000ff\\c%s\\c0xff000000\\c", MakeStatusString(i, part->m_baseStat[i], part->m_slotStat[i])), false, true);
				}
			}
		}
	}

	IFF_STRUCT::sDesc *desc = ItemManager()->FindDesc(item->m_typeId);
	if (desc) {
		form->SetMessage(desc->m_text, false);
	}

	form->Open(0, 3);
}

// shopmain.cpp:6393
void CShopMain::OpenCaddieInfo(const sCaddieInfo &info) {
	FrWnd *wnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName("caddie_info");
	if (wnd != NULL) {
		return;
	}

	FrCaddieInfoDlg *dlg = CreateForm<FrCaddieInfoDlg>(g_pFresh->GetManager(), this, "caddie_info", DYNAMIC_CAST(FrWnd, wnd));
	if (dlg == NULL) {
		return;
	}

	dlg->SetCaddieInfo(info);
	dlg->Open((FRESH_RESULT_CALLBACK)&CShopMain::OnReemployeeResult, 3);
}

// shopmain.cpp:6407
void CShopMain::ResetItemList(bool bKeepType1) {
	if (m_itemListBox) {
		for (FrListBox::ITEM_LIST::iterator i = m_itemListBox->m_itemList.begin(); i != m_itemListBox->m_itemList.end(); ++i) {
			FrListItem *listItem = *i;
			if (listItem->selected) {
				if (bKeepType1) {
					if ((((IFF_ITEM_COMMON *)listItem->pData)->m_typeId & 0xFC000000) == 0x04000000) {
						continue;
					}
				}

				m_itemListBox->UnselectItem(listItem);
			}
		}
	}
}

// shopmain.cpp:6439
int CShopMain::GetItemNumInMyPocket(unsigned long typeId, bool skipFlagged) {
	int count = 0;

	if (typeId == 0 || (typeId & 0xFC000000) == 0x04000000) {
		for (std::map<unsigned int, sCharacterInfo>::const_iterator it = CSharedDoc::Instance()->m_characterMap.begin(); it != CSharedDoc::Instance()->m_characterMap.end(); ++it) {
			IFF_ITEM_COMMON *item = ItemManager()->FindCommonItem(it->second.typeId);
			if (item) {
				if ((typeId == 0 || it->second.typeId == typeId) && (!skipFlagged || !(item->m_flags0 & 1)))
					count++;
			}
		}
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x08000000) {
		for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_myPartsList.begin(); it != CSharedDoc::Instance()->m_myPartsList.end(); ++it) {
			IFF_ITEM_COMMON *item = ItemManager()->FindCommonItem(it->m_typeId);
			if (item) {
				if ((typeId == 0 || it->m_typeId == typeId) && (!skipFlagged || !(item->m_flags0 & 1)))
					count++;
			}
		}
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x18000000) {
		for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_itemInfoList.begin(); it != CSharedDoc::Instance()->m_itemInfoList.end(); ++it) {
			IFF_ITEM_COMMON *item = ItemManager()->FindCommonItem(it->m_typeId);

			if (item) {
				if ((typeId == 0 || it->m_typeId == typeId) && (!skipFlagged || !(item->m_flags0 & 1))) {
					if (it->m_stateBits.kind) {
						count = 100000;
					} else {
						count++;
					}
				}
			}
		}
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x1C000000) {
		for (std::map<unsigned int, sCaddieInfo>::iterator it = CSharedDoc::Instance()->m_caddieMap.begin(); it != CSharedDoc::Instance()->m_caddieMap.end(); ++it) {
			IFF_ITEM_COMMON *item = ItemManager()->FindCommonItem(it->second.typeId);
			if (item) {
				if ((typeId == 0 || it->second.typeId == typeId) && (!skipFlagged || !(item->m_flags0 & 1)))
					count++;
			}
		}
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x14000000) {
		for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_myGearList.begin(); it != CSharedDoc::Instance()->m_myGearList.end(); ++it) {
			IFF_ITEM_COMMON *item = ItemManager()->FindCommonItem(it->m_typeId);
			if (item) {
				if ((typeId == 0 || it->m_typeId == typeId) && (!skipFlagged || !(item->m_flags0 & 1)))
					count++;
			}
		}
	}

	unsigned long type = typeId >> 26;
	if (type == 9 && (typeId & 0x3E00000) == 0x200000) {
		IFF_STRUCT::sSetItem *setItem = ItemManager()->FindSetItem(typeId);

		for (unsigned char i = 0; i < setItem->m_itemCount; i++) {
			if (typeId == 0 || (setItem->m_itemTypeIds[i] & 0xFC000000) != 0x04000000) {
				for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_myPartsList.begin(); it != CSharedDoc::Instance()->m_myPartsList.end(); ++it) {
					IFF_ITEM_COMMON *item = ItemManager()->FindCommonItem(it->m_typeId);
					if (item) {
						if ((typeId == 0 || it->m_typeId == setItem->m_itemTypeIds[i]) && (!skipFlagged || !(item->m_flags0 & 1)))
							count++;
					}
				}
			}
		}
	}

	if (typeId == 0 || type == 14) {
		for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_mySkinList.begin(); it != CSharedDoc::Instance()->m_mySkinList.end(); ++it) {
			IFF_ITEM_COMMON *item = ItemManager()->FindCommonItem(it->m_typeId);
			if (item) {
				if ((typeId == 0 || it->m_typeId == typeId) && (!skipFlagged || !(item->m_flags0 & 1)))
					count++;
			}
		}
	}

	if (typeId == 0 || type == 15) {
		count = 1;
	}

	if (type == 4) {
		for (std::map<unsigned int, sItemInfo>::iterator it = CSharedDoc::Instance()->m_myItemMap.begin(); it != CSharedDoc::Instance()->m_myItemMap.end(); ++it) {
			IFF_ITEM_COMMON *item = ItemManager()->FindCommonItem(it->second.m_typeId);
			if (item) {
				if ((typeId == 0 || it->second.m_typeId == typeId) && (!skipFlagged || !(item->m_flags0 & 1)))
					count++;
			}
		}
	}

	return count;
}

// shopmain.cpp:6596
void CShopMain::Checkout(void) {
	if (WSingleton<CSharedDoc>::Instance()->IsControlServerService(4)) {
		this << MsgObject(this, 0x23, (intptr_t)"\xBB\xF3\xC1\xA1 \xB0\xFC\xB7\xC3 \xBA\xCE\xBA\xD0 \xC1\xA1\xB0\xCB\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
		return;
	}

	int count = m_cartList->m_itemList.size();
	count += m_auxCartList->m_itemList.size();

	if (count == 0) {
		this << MsgObject(0, 0x23, (intptr_t)"\xB1\xB8\xB8\xC5\xC7\xD2 \xBB\xF3\xC7\xB0\xC0\xBB \xBC\xB1\xC5\xC3\xC7\xD8 \xC1\xD6\xBD\xCA\xBD\xC3\xBF\xC0", 0, 0, 0, 0);
		return;
	}

	bool bNotForSale = false;
	FrListBox::ITEM_LIST::iterator it;
	for (it = m_cartList->m_itemList.begin(); it != m_cartList->m_itemList.end(); ++it) {
		IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)(*it)->pData;

		unsigned char shopFlag = item->m_flags0 >> 5;
		if (shopFlag == 0 || shopFlag == 2 || shopFlag == 4) {
			bNotForSale = true;
			break;
		}
	}

	if (!bNotForSale) {
		for (it = m_auxCartList->m_itemList.begin(); it != m_auxCartList->m_itemList.end(); ++it) {
			IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)(*it)->pData;

			unsigned char shopFlag = item->m_flags0 >> 5;
			if (shopFlag == 0 || shopFlag == 2 || shopFlag == 4) {
				bNotForSale = true;
				break;
			}
		}
	}

	if (bNotForSale) {
		this << MsgObject(0, 0x23, (intptr_t)"\xB1\xB8\xB8\xC5\xC7\xD2 \xBC\xF6 \xBE\xF8\xB4\xC2 \xBB\xF3\xC7\xB0\xC0\xCC \xC6\xF7\xC7\xD4\xB5\xC7\xBE\xEE \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
		return;
	}

	for (it = m_cartList->m_itemList.begin(); it != m_cartList->m_itemList.end(); ++it) {
		IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)(*it)->pData;

		if ((item->m_typeId & 0xFC000000) == 0x48000000) {
			std::list<sFurniture_List> &furnitures = WSingleton<CSharedDoc>::Instance()->m_furnitureList;
			for (std::list<sFurniture_List>::iterator fit = furnitures.begin(); fit != WSingleton<CSharedDoc>::Instance()->m_furnitureList.end(); ++fit) {
				IFF_STRUCT::sFurniture *furniture = ItemManager()->FindFurniture(fit->m_typeId);

				if (!furniture)
					break;

				if (furniture->m_placeOnModel == 0 && fit->m_typeId == item->m_typeId) {
					this << MsgObject(0, 0x23, (intptr_t)"\xC0\xCC\xB9\xCC \xB0\xA1\xC1\xF6\xB0\xED \xC0\xD6\xB4\xC2 \xB9\xB0\xB0\xC7\xC0\xBB \xB1\xB8\xC0\xD4\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
					return;
				}
			}
		}
	}

	m_buyDlg = CreateForm<FrCheckoutDlg>(g_pFresh->GetManager(), this, "checkout", 0);

	for (it = m_cartList->m_itemList.begin(); it != m_cartList->m_itemList.end(); ++it) {
		FrCheckoutDlg::CartItem cartItem;
		cartItem.m_pItem = (IFF_ITEM_COMMON *)(*it)->pData;

		if ((cartItem.m_pItem->m_typeId & 0xFC000000) == 0x24000000) {
			IFF_STRUCT::sSetItem *setItem = ItemManager()->FindSetItem(cartItem.m_pItem->m_typeId);
			if (setItem) {
				cartItem.m_option = 0;
				for (int i = 0; i < setItem->m_itemCount; i++) {
					unsigned long typeId = setItem->m_itemTypeIds[i];
					if (typeId && (typeId >> 26) != 1 && (typeId >> 26) != 6)
						cartItem.m_option += GetItemNumInMyPocket(typeId, false);
				}
			}
		} else {
			cartItem.m_option = GetItemNumInMyPocket(cartItem.m_pItem->m_typeId, false);
		}

		cartItem.m_days = 5;
		((FrCheckoutDlg *)m_buyDlg)->Cart().push_back(cartItem);
	}

	for (it = m_auxCartList->m_itemList.begin(); it != m_auxCartList->m_itemList.end(); ++it) {
		FrCheckoutDlg::CartItem cartItem;
		cartItem.m_pItem = (IFF_ITEM_COMMON *)(*it)->pData;
		cartItem.m_option = GetItemNumInMyPocket(cartItem.m_pItem->m_typeId, false);

		cartItem.m_days = 5;
		((FrCheckoutDlg *)m_buyDlg)->Cart().push_back(cartItem);
	}

	bool bRental = m_shopTab == 0 && m_sideTabRental && m_sideTabRental->GetStatus() == 2;
	((FrCheckoutDlg *)m_buyDlg)->Checkout(WSingleton<CSharedDoc>::Instance()->m_myPang, WSingleton<CSharedDoc>::Instance()->m_myCash, bRental);
	m_buyDlg->Open((FRESH_RESULT_CALLBACK)&CShopMain::OnBuyDlgResult, 0x81);
}

// shopmain.cpp:6750
void CShopMain::SendGift(const IFF_ITEM_COMMON &item) {
	if (WSingleton<CSharedDoc>::Instance()->IsControlServerService(0xC)) {
		this << MsgObject(this, 0x23, (intptr_t)"\xBC\xB1\xB9\xB0\xC7\xD4 \xB0\xFC\xB7\xC3 \xBA\xCE\xBA\xD0 \xC1\xA1\xB0\xCB\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
		return;
	}

	if (GiftForbiddenMode()) {
		this << MsgObject(0, 0x23, (intptr_t)"\xBC\xB1\xB9\xB0\xC7\xCF\xB1\xE2\xB8\xA6 \xC7\xD2 \xBC\xF6 \xBE\xF8\xB4\xC2 \xB0\xE8\xC1\xA4\xC0\xD4\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
		return;
	}

	if (!m_cartList)
		return;

	if (m_cartList->m_itemList.size() == 0 && m_auxCartList->m_itemList.size() == 0) {
		this << MsgObject(0, 0x23, (intptr_t)"\xB1\xB8\xB8\xC5\xC7\xD2 \xBB\xF3\xC7\xB0\xC0\xBB \xBC\xB1\xC5\xC3\xC7\xD8 \xC1\xD6\xBD\xCA\xBD\xC3\xBF\xC0", 0, 0, 0, 0);
		return;
	}

	unsigned char shopFlag = item.m_flags0 >> 5;
	if (shopFlag == 0 || shopFlag == 3 || shopFlag == 4) {
		this << MsgObject(0, 0x23, (intptr_t)"\xBC\xB1\xB9\xB0\xC7\xD2 \xBC\xF6 \xBE\xF8\xB4\xC2 \xBB\xF3\xC7\xB0\xC0\xD4\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
	}

	m_giftDlg = CreateForm<FrGiftDlg>(g_pFresh->GetManager(), this, "send_gift", NULL);
	m_giftDlg->SendGift(item, WSingleton<CSharedDoc>::Instance()->m_myPang, WSingleton<CSharedDoc>::Instance()->m_myCash);

	m_giftDlg->Open((FRESH_RESULT_CALLBACK)&CShopMain::OnGiftDlgResult, 0x81);
}

// shopmain.cpp:6827
void CShopMain::Repay(const IFF_ITEM_COMMON &item, unsigned long guid) {
	FrRepayDlg *dlg = CreateForm<FrRepayDlg>(g_pFresh->GetManager(), this, "repay", NULL);
	if (dlg) {
		CSharedDoc *doc = WSingleton<CSharedDoc>::Instance();
		dlg->Repay(item, guid, doc->m_myPang, doc->m_myCash);
		dlg->Open((FRESH_RESULT_CALLBACK)&CShopMain::OnSellDlgResult, 0x81);
	}
}

// shopmain.cpp:6847
void CShopMain::ClearCartList(bool bKeepCharacters, CPartTidList *pTidList) {
	if (!m_cartList) {
		return;
	}

	FrListBox::ITEM_LIST::iterator i = m_cartList->m_itemList.begin();
	while (i != m_cartList->m_itemList.end()) {
		IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)(*i)->pData;

		if ((item->m_typeId >> 26) == 1) {
			if (bKeepCharacters) {
				++i;
				continue;
			}
			i = m_cartList->_DelItem(i);
			continue;
		} else if ((item->m_typeId >> 26) == 2 && pTidList) {
			if (pTidList->IsEquipped(item->m_typeId, 0) && !m_anotherHand.IsWorking(0)) {
				m_petExhibition->GetPetFrame()->SetPart(pTidList, item->m_typeId, 0);

				RestoreShoppersOldItem(item->m_typeId, *pTidList);
			}
		} else if ((item->m_typeId >> 26) == 0xF && pTidList) {
			if (!m_anotherHand.IsWorking(0)) {
				bool found = false;
				const std::map<unsigned int, sCharacterInfo> &characters = WSingleton<CSharedDoc>::Instance()->m_characterMap;
				unsigned long tidChar = m_petExhibition->GetPetFrame()->GetTidChar();
				for (std::map<unsigned int, sCharacterInfo>::const_iterator it = characters.begin(); it != characters.end(); ++it) {
					if (it->second.typeId == tidChar) {
						m_hairColor = it->second.hairColor;

						CPartTidList &tidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();
						m_petExhibition->GetPetFrame()->SetHairClr(&tidList, m_hairColor);
						found = true;
						break;
					}
				}
				if (!found) {
					m_hairColor = 0;
				}
			}
		}

		i = m_cartList->_DelItem(i);
	}

	FrListBox::ITEM_LIST::iterator j = m_auxCartList->m_itemList.begin();
	while (j != m_auxCartList->m_itemList.end()) {
		IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)(*j)->pData;
		if ((item->m_typeId & 0xFC000000) == 0x08000000 && pTidList
			&& pTidList->IsEquipped(item->m_typeId, 0) && !m_anotherHand.IsWorking(0)) {
			m_petExhibition->GetPetFrame()->SetPart(pTidList, item->m_typeId, 0);

			RestoreShoppersOldItem(item->m_typeId, *pTidList);
		}

		j = m_auxCartList->_DelItem(j);
	}

	CheckAuxCartScroll();
}

// shopmain.cpp:6931
FrListBox::ITEM_LIST::iterator CShopMain::DeleteItemFromCartList(unsigned long typeId) {
	for (FrListBox::ITEM_LIST::iterator i = m_cartList->m_itemList.begin(); i != m_cartList->m_itemList.end(); ++i) {
		IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)(*i)->pData;
		if (item->m_typeId == typeId) {
			return m_cartList->_DelItem(i);
		}
	}

	return m_cartList->m_itemList.end();
}

// shopmain.cpp:6944
FrListBox::ITEM_LIST::iterator CShopMain::DeleteItemFromAuxCartList(unsigned long typeId) {
	FrListBox::ITEM_LIST::iterator end = m_auxCartList->m_itemList.end();
	for (FrListBox::ITEM_LIST::iterator i = m_auxCartList->m_itemList.begin(); i != end; ++i) {
		IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)(*i)->pData;
		if (item->m_typeId == typeId) {
			FrListBox::ITEM_LIST::iterator next = m_auxCartList->_DelItem(i);
			CheckAuxCartScroll();
			return next;
		}
	}

	return end;
}

// shopmain.cpp:6959
bool CShopMain::SelectItemList(unsigned long typeId) {
	if (!m_itemListBox) {
		return false;
	}

	FrListItem *found = NULL;
	for (FrListBox::ITEM_LIST::iterator i = m_itemListBox->m_itemList.begin(); i != m_itemListBox->m_itemList.end(); ++i) {
		IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)(*i)->pData;
		if (item) {
			if (item->m_typeId == typeId) {
				found = *i;
				break;
			}
		}
	}

	if (!found) {
		return false;
	}

	SelectItemList(found);
	return true;
}

// shopmain.cpp:6986
bool CShopMain::SelectItemList(FrListItem *pItem) {
	IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)pItem->pData;

	switch (item->m_typeId >> 26) {
	case 1: {
		WSingleton<CSharedDoc>::Instance()->BuildMyPartTidListDefault(item->m_typeId);
		CPartTidList &tidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

		tidList.m_hairClr = m_hairColor;

		m_standPetTypeId = item->m_typeId;

		m_charParts = tidList.m_partTids;
		SafeLoadPet(&m_standPetTypeId, true);

		// Keep the hair style of the cart.
		IFF_ITEM_COMMON *hairItem = NULL;
		for (FrListBox::ITEM_LIST::iterator it = m_cartList->m_itemList.begin(); it != m_cartList->m_itemList.end(); ++it) {
			hairItem = (IFF_ITEM_COMMON *)(*it)->pData;
			if ((hairItem->m_typeId & 0xFC000000) == 0x3C000000) {
				break;
			}

			hairItem = NULL;
		}

		m_auxCartList->ClearItem();
		m_cartList->ClearItem();
		m_cartList->AddItem(item);
		if (hairItem) {
			m_cartList->AddItem(hairItem);
		}
		TurnOnItemStatus();
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		break;
	}

	case 2: {
		IFF_ITEM_COMMON *part = (IFF_ITEM_COMMON *)pItem->pData;
		CPartTidList &tidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

		unsigned long overlapped = tidList.GetOverlapped(part->m_typeId);
		if (!tidList.CanSetPart(part->m_typeId)) {
			m_itemListBox->UnselectItem(pItem);
			char text[256];
			if (((IFF_STRUCT::sPart *)part)->m_wearRestricted) {
				sprintf(text, "\xBA\xBB \xBB\xF3\xC7\xB0\xC0\xBA \xC7\xF6\xC0\xE7 \xC0\xC7\xBB\xF3\xBF\xA1\xB4\xC2 \xC2\xF8\xBF\xEB\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.");
				this << MsgObject(0, 0x23, (intptr_t)text, 0, 0, 0, 0);
				return false;
			}

			this << MsgObject(0, 0x23, (intptr_t)"\\c0xffff0000\\c\xBB\xF3\xC7\xB0\xC0\xBB \xC2\xF8\xBF\xEB\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
			return false;
		}

		TakeOff_PartNCartList(tidList, overlapped);

		TakeOff_AuxPartNCartList(tidList, overlapped);

		unsigned long serial = 0;
		IFF_ITEM_COMMON *partItem = (IFF_ITEM_COMMON *)pItem->pData;
		if (UccManager()->IsDisplayUccClothes(part)) {
			// A UCC part replaces the old items and the selection of its slot.
			unsigned long typeId = item->m_typeId;
			for (std::list<oldItem_t>::iterator it = m_shoppersOldItems.begin(); it != m_shoppersOldItems.end();) {
				if (((it->typeId >> 13) & 0x1F) == ((typeId >> 13) & 0x1F)) {
					UnselectItemFromItemList(it->typeId);
					it = m_shoppersOldItems.erase(it);
				} else {
					++it;
				}
			}

			for (FrListBox::ITEM_LIST::iterator i = m_itemListBox->m_itemList.begin(); i != m_itemListBox->m_itemList.end(); ++i) {
				if (*i != pItem) {
					IFF_ITEM_COMMON *other = (IFF_ITEM_COMMON *)(*i)->pData;
					if (other) {
						if (((other->m_typeId ^ typeId) & 0x3E000) == 0) {
							m_itemListBox->UnselectItem(*i);
						}
					}
				}
			}

			serial = ((sDisplayUccClothes *)partItem)->m_serial;
		}

		PutOnNBackupShoppersOldItem(part->m_typeId, tidList, serial);

		if (!UccManager()->IsDisplayUccClothes(partItem)) {
			unsigned char slot = ((IFF_STRUCT::sPart *)part)->slot;
			if (slot == 0 || slot == 1 || slot == 4 || slot == 7 || slot == 8 || slot == 9) {
				m_cartList->AddItem(part);
			} else {
				m_auxCartList->AddItem(part);
				CheckAuxCartScroll();
			}
		}

		m_charParts = tidList.m_partTids;

		ShowMainModel(tidList);
		if (m_hairColor != tidList.m_hairClr) {
			m_petExhibition->GetPetFrame()->SetHairClr(&tidList, m_hairColor);
		}
		TurnOnItemStatus();
		g_audio->PlaySfx("item_select_cloth", 0, 0, 0, 0, 0.5f, 200.0f);
		break;
	}

	case 9: {
		unsigned long kind = (item->m_typeId >> 21) & 0x1F;
		if (kind == 6 || kind == 5 || kind == 4 || kind == 8) {
			m_cartList->ClearItem();
			m_cartList->AddItem(item);

			SafeLoadPet(&item->m_typeId, false);
			TurnOffItemStatus();
			g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
			ShowItemDesc(item->m_typeId);
		} else {
			m_cartList->ClearItem();

			IFF_STRUCT::sSetItem *setItem = (IFF_STRUCT::sSetItem *)pItem->pData;
			CPartTidList &tidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

			if ((setItem->m_typeId & 0x3E00000) == 0x200000
				|| (setItem->m_itemTypeIds[0] & 0xFC000000) == 0x4000000) {
				IFF_STRUCT::sChar *character;
				if ((setItem->m_itemTypeIds[0] & 0xFC000000) == 0x4000000) {
					character = ItemManager()->FindChar(setItem->m_itemTypeIds[0]);
				} else {
					character = ItemManager()->FindChar(((setItem->m_typeId >> 13) & 0xFF) | 0x4000000);
				}

				if (character) {
					for (int i = 0; i < character->m_numParts + character->m_numExtraParts; i++) {
						if (tidList.m_partTids[i] && !(tidList.m_partTids[i] & 0x600)) {
							m_petExhibition->GetPetFrame()->SetPart(&tidList, tidList.m_partTids[i], 0);
						}
					}
				}
			}

			for (int i = 0; i < setItem->m_itemCount; i++) {
				if ((setItem->m_itemTypeIds[i] & 0xFC000000) == 0x08000000) {
					PutOnNBackupShoppersOldItem(setItem->m_itemTypeIds[i], tidList, 0);
				}
			}

			m_charParts = tidList.m_partTids; m_petExhibition->GetPetFrame()->SetHairClr(&tidList, m_hairColor);

			// Keep the hair style of the cart.
			IFF_ITEM_COMMON *hairItem = NULL;
			for (FrListBox::ITEM_LIST::iterator it = m_cartList->m_itemList.begin(); it != m_cartList->m_itemList.end(); ++it) {
				hairItem = (IFF_ITEM_COMMON *)(*it)->pData;
				if ((hairItem->m_typeId & 0xFC000000) == 0x3C000000) {
					break;
				}

				hairItem = NULL;
			}

			ShowMainModel(tidList);

			m_auxCartList->ClearItem();
			m_cartList->ClearItem();
			m_cartList->AddItem(setItem);
			if (hairItem) {
				m_cartList->AddItem(hairItem);
			}
			TurnOnItemStatus();
			g_audio->PlaySfx("item_select_cloth", 0, 0, 0, 0, 0.5f, 200.0f);
		}
		break;
	}

	case 0xF: {
		CPartTidList &tidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

		m_petExhibition->GetPetFrame()->SetHairClr(&tidList, ((IFF_STRUCT::sHairStyle *)item)->m_color);

		// Only one hair style in the cart.
		for (FrListBox::ITEM_LIST::iterator it = m_cartList->m_itemList.begin(); it != m_cartList->m_itemList.end();) {
			IFF_ITEM_COMMON *cartItem = (IFF_ITEM_COMMON *)(*it)->pData;

			if ((cartItem->m_typeId & 0xFC000000) == 0x3C000000) {
				UnselectItemFromItemList(cartItem->m_typeId);
				it = DeleteItemFromCartList(cartItem->m_typeId);
			} else {
				++it;
			}
		}

		m_cartList->AddItem(item);

		m_hairColor = ((IFF_STRUCT::sHairStyle *)item)->m_color;

		TurnOnItemStatus();
		g_audio->PlaySfx("item_select_hair", 0, 0, 0, 0, 0.5f, 200.0f);
		break;
	}

	case 4:
		m_cartList->ClearItem();
		m_cartList->AddItem(item);

		m_clubsetTypeId = item->m_typeId;
		SafeLoadPet(&m_clubsetTypeId, false);
		TurnOnItemStatus();
		g_audio->PlaySfx("item_select_club", 0, 0, 0, 0, 0.5f, 200.0f);
		ShowItemDesc(m_clubsetTypeId);
		break;

	case 5:
		m_cartList->ClearItem();
		m_cartList->AddItem(item);

		m_aztecTypeId = item->m_typeId;
		SafeLoadPet(&m_aztecTypeId, false);
		TurnOffItemStatus();
		g_audio->PlaySfx("item_select_aztec", 0, 0, 0, 0, 0.5f, 200.0f);
		ShowItemDesc(m_aztecTypeId);
		break;

	case 7:
		m_cartList->ClearItem();
		m_cartList->AddItem(item);

		m_caddieTypeId = item->m_typeId;
		SafeLoadPet(&m_caddieTypeId, false);
		TurnOnItemStatus();
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		ShowItemDesc(item->m_typeId);
		break;

	case 8:
		if ((item->m_typeId & 0x1FE000) != 0x6000) {
			m_cartList->ClearItem();
			m_cartList->AddItem(item);
			SafeLoadPet(&item->m_typeId, false);
			TurnOnItemStatus();
		}
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		ShowItemDesc(item->m_typeId);
		break;

	case 0x10:
		m_cartList->ClearItem();
		m_cartList->AddItem(item);

		m_mascotTypeId = item->m_typeId;
		SafeLoadPet(&m_mascotTypeId, false);
		TurnOffItemStatus();
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		ShowItemDesc(item->m_typeId);
		break;

	case 0xE: {
		eEquipSkin skin;
		if (S5::UTIL::GetEquipSkinIndex(skin, item->m_typeId)) {
			if (m_equipSkin[skin]) {
				DeleteItemFromCartList(m_equipSkin[skin]);
			}

			m_equipSkin[skin] = item->m_typeId;
			IFF_STRUCT::sSkin *skinItem = (IFF_STRUCT::sSkin *)pItem->pData;
			if (!(item->m_typeId & 0x3C00000)) {
				m_sideDir[0] = skinItem->m_sideSlideMode[0];
				m_sideDir[1] = skinItem->m_sideSlideMode[1];
				m_sidePos[0] = 0;
				m_sidePos[1] = 0;
			}
		} else {
			m_cartList->ClearItem();
		}

		m_cartList->AddItem(item);
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		break;
	}

	// Three identical bodies, which cl merges into one block: a single body
	// with three labels is hot enough for cl to inline TurnOffItemStatus,
	// which the original calls.
	case 6:
		m_cartList->ClearItem();
		m_cartList->AddItem(item);

		SafeLoadPet(&item->m_typeId, false);
		TurnOffItemStatus();
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		ShowItemDesc(item->m_typeId);
		break;

	case 0x12:
		m_cartList->ClearItem();
		m_cartList->AddItem(item);

		SafeLoadPet(&item->m_typeId, false);
		TurnOffItemStatus();
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		ShowItemDesc(item->m_typeId);
		break;

	case 0x1F:
		m_cartList->ClearItem();
		m_cartList->AddItem(item);

		SafeLoadPet(&item->m_typeId, false);
		TurnOffItemStatus();
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		ShowItemDesc(item->m_typeId);
		break;
	}

	pItem->selected = true;

	return true;
}

// shopmain.cpp:7467
void CShopMain::UnselectItemList(unsigned long typeId) {
	CPartTidList &tidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

	switch (typeId >> 26) {
	case 2: {
		DeleteItemFromCartList(typeId);
		DeleteItemFromAuxCartList(typeId);
		UnselectItemFromItemList(typeId);

		// The parts worn before taking this one off.
		unsigned long oldParts[24];
		memcpy(oldParts, tidList.m_partTids, sizeof(oldParts));

		if (tidList.IsEquipped(typeId, 0)) {
			m_petExhibition->GetPetFrame()->SetPart(&tidList, typeId, 0);
			RestoreShoppersOldItem(typeId, tidList);
			m_charParts = tidList.m_partTids;
		}

		for (int i = 0; i < 24; i++) {
			if (oldParts[i] && typeId != oldParts[i] && tidList.m_partTids[i] != oldParts[i]) {
				if (oldParts[i] == 2 || oldParts[i] == 3) {
					continue;
				}

				UnselectItemFromItemList(oldParts[i]);
				DeleteItemFromCartList(oldParts[i]);
				DeleteItemFromAuxCartList(oldParts[i]);
			}
		}

		tidList.m_hairClr = m_hairColor;
		ShowMainModel(tidList);
		break;
	}

	case 7:
	case 31:
		break;

	case 9: {
		DeleteItemFromCartList(typeId);
		UnselectItemFromItemList(typeId);

		IFF_STRUCT::sSetItem *setItem = ItemManager()->FindSetItem(typeId);

		for (int i = 0; i < setItem->m_itemCount; i++) {
			unsigned long partTypeId = setItem->m_itemTypeIds[i];
			if (tidList.IsEquipped(partTypeId, 0)) {
				m_petExhibition->GetPetFrame()->SetPart(&tidList, partTypeId, 0);
				RestoreShoppersOldItem(partTypeId, tidList);
				m_charParts = tidList.m_partTids;
			}
		}

		// Select the character of the item list back.
		for (FrListBox::ITEM_LIST::iterator it = m_itemListBox->m_itemList.begin(); it != m_itemListBox->m_itemList.end(); ++it) {
			IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)(*it)->pData;

			if (item) {

				if ((item->m_typeId & 0xFC000000) == 0x04000000) {

					SelectItemList(*it);
					m_itemListBox->SelectItem(*it, true);
					break;
				}
			}
		}

		tidList.m_hairClr = m_hairColor;
		ShowMainModel(tidList);
		break;
	}

	case 0xF: {
		DeleteItemFromCartList(typeId);
		UnselectItemFromItemList(typeId);

		unsigned char hairColor = 0;
		CPartTidList &myTidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

		const std::map<unsigned int, sCharacterInfo> &characters = WSingleton<CSharedDoc>::Instance()->m_characterMap;
		std::map<unsigned int, sCharacterInfo>::const_iterator it = characters.begin(), end = characters.end();
		if (it != end) {
			unsigned long tidChar = m_petExhibition->GetPetFrame()->GetTidChar();

			for (; it != end; ++it) {
				if (it->second.typeId == tidChar) {
					hairColor = it->second.hairColor;
					break;
				}
			}
		}

		m_hairColor = hairColor;

		myTidList.m_hairClr = hairColor;
		ShowMainModel(myTidList);
		break;
	}

	case 0xE: {
		DeleteItemFromCartList(typeId);
		UnselectItemFromItemList(typeId);

		eEquipSkin skin;
		if (S5::UTIL::GetEquipSkinIndex(skin, typeId)) {

			m_equipSkin[skin] = WSingleton<CSharedDoc>::Instance()->m_equipSkin[skin];
			if (!(typeId & 0x3C00000)) {
				if (m_equipSkin[skin] == 0) {
					m_sideDir[0] = 0;
					m_sideDir[1] = 0;
				} else {
					IFF_STRUCT::sSkin *skinItem = ItemManager()->FindSkin(m_equipSkin[skin]);
					m_sideDir[0] = skinItem->m_sideSlideMode[0];
					m_sideDir[1] = skinItem->m_sideSlideMode[1];
				}

				m_sidePos[0] = 0;
				m_sidePos[1] = 0;
			}
		}
		break;
	}

	default:
		UnselectItemFromItemList(typeId);
		m_statBox->SetVisible(false); SetLevelBar();
		break;
	}
}

// shopmain.cpp:7628
bool CShopMain::UnselectItemFromItemList(unsigned long typeId) {
	bool unselected = false;
	if (!m_itemListBox) {
		return unselected;
	}

	for (FrListBox::ITEM_LIST::iterator i = m_itemListBox->m_itemList.begin(); i != m_itemListBox->m_itemList.end(); ++i) {
		IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)(*i)->pData;
		if (item) {
			if (item->m_typeId == typeId) {
				m_itemListBox->UnselectItem(*i);
				unselected = true;
			}
		}
	}

	return unselected;
}

// shopmain.cpp:8464
void CShopMain::TakeOff_PartNCartList(CPartTidList &tidList, unsigned long overlapMask) {
	for (FrListBox::ITEM_LIST::iterator i = m_cartList->m_itemList.begin(); i != m_cartList->m_itemList.end();) {
		IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)(*i)->pData;

		if ((item->m_typeId >> 26) == 2) {

			unsigned long overlapped = tidList.GetOverlapped(item->m_typeId);
			if (overlapMask & overlapped) {

				UnselectItemFromItemList(item->m_typeId);
				i = DeleteItemFromCartList(item->m_typeId);

				if (tidList.IsEquipped(item->m_typeId, 0)) {

					m_petExhibition->GetPetFrame()->SetPart(&tidList, item->m_typeId, 0);
				}

				RestoreShoppersOldItem(item->m_typeId, tidList);
				UnselectItemList(item->m_typeId);

				continue;
			}

			++i;

			continue;
		} else if ((item->m_typeId >> 26) == 9) {
			IFF_STRUCT::sSetItem *setItem = (IFF_STRUCT::sSetItem *)item;

			for (int j = 0; j < setItem->m_itemCount; j++) {

				unsigned long typeId = setItem->m_itemTypeIds[j];

				if (tidList.IsEquipped(typeId, 0)) {

					m_petExhibition->GetPetFrame()->SetPart(&tidList, typeId, 0);
				}

				RestoreShoppersOldItem(typeId, tidList);
			}

			UnselectItemFromItemList(item->m_typeId);
			i = DeleteItemFromCartList(item->m_typeId);

			continue;
		}

		++i;
	}
}

// shopmain.cpp:8515
void CShopMain::TakeOff_AuxPartNCartList(CPartTidList &tidList, unsigned long overlapMask) {
	for (FrListBox::ITEM_LIST::iterator i = m_auxCartList->m_itemList.begin(); i != m_auxCartList->m_itemList.end();) {
		IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)(*i)->pData;

		if ((item->m_typeId & 0xFC000000) == 0x08000000) {

			unsigned long overlapped = tidList.GetOverlapped(item->m_typeId);
			if (overlapMask & overlapped) {

				UnselectItemFromItemList(item->m_typeId);
				i = DeleteItemFromAuxCartList(item->m_typeId);

				if (tidList.IsEquipped(item->m_typeId, 0)) {

					m_petExhibition->GetPetFrame()->SetPart(&tidList, item->m_typeId, 0);
				}

				RestoreShoppersOldItem(item->m_typeId, tidList);

				continue;
			}

			++i;

			continue;
		}

		++i;
	}
}

// shopmain.cpp:8548
void CShopMain::SetLevelBar_Char(void) {
	IFF_STRUCT::sChar *character = ItemManager()->FindChar(m_standPetTypeId);
	if (character) {

		for (int i = 0; i < 5; i++) {

			unsigned char itemLevel = ItemManager()->GetItemLevel((eLvlType)i, m_charParts, m_charCards);
			unsigned char maxValue = ItemManager()->GetCharCapacity((eLvlType)i, m_standPetTypeId, m_charParts, m_charCards, WSingleton<CSharedDoc>::Instance()->m_myLevel) + itemLevel;
			unsigned char value = ItemManager()->GetCharLevel((eLvlType)i, m_standPetTypeId, m_charLevels) + itemLevel;

			if (m_statBar[i]) {

				m_statBar[i]->SetDestPos(value, false);
				m_statBar[i]->SetExpand(maxValue - value);
			}

			if (m_statEdit[i]) {

				m_statEdit[i]->SetCaption11(MakeStatusString2(value, maxValue, 0));
			}
		}
	}

	m_statPartKind = 0;
}

// shopmain.cpp:8577
void CShopMain::SetLevelBar_Clubset(void) {
	IFF_STRUCT::sClubSet *clubSet = ItemManager()->FindClubSet(m_clubsetTypeId);
	if (clubSet) {

		for (int i = 0; i < 5; i++) {

			unsigned char value = (unsigned char)clubSet->m_baseStat[i], maxValue = (unsigned char)clubSet->m_maxStat[i];

			if (m_statBar[i]) {

				m_statBar[i]->SetDestPos(value, false);
				m_statBar[i]->SetExpand(maxValue - value);
			}

			if (m_statEdit[i]) {

				m_statEdit[i]->SetCaption11(MakeStatusString2(value, maxValue, 0));
			}
		}
	}

	m_statPartKind = 1;
}

// shopmain.cpp:8604
void CShopMain::Build_Package(subTab_t tab, int) {
	IFF_ITEM_COMMON &charItem = (IFF_ITEM_COMMON &)ItemManager()->m_charMap[m_standPetTypeId];
	if ((charItem.m_flags0 >> 5) != 0 && ItemManager()->IsInSalePeriod(charItem.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {

		FrListItem *listItem = m_itemListBox->AddItem(&charItem);
		AddSellMiniButton(charItem, listItem);

		if (IsItemInCart(listItem))
			m_itemListBox->SelectItem(listItem, true);

		typedef std::map<unsigned int, IFF_STRUCT::sSetItem> SetItemMap;
		for (SetItemMap::iterator it = ItemManager()->m_setItemMap.begin(); it != ItemManager()->m_setItemMap.end(); ++it) {
			IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
			if ((item.m_flags0 >> 5) != 0 && (item.m_typeId & 0x3E00000) == 0x200000
				&& ((item.m_typeId >> 13) & 0xFF) == (m_standPetTypeId & 0x3FFFFFF)
				&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
				listItem = m_itemListBox->AddItem(&item);
				AddSellMiniButton(item, listItem);
			}
		}
	}

	m_itemListBox->SortItem(SpecialHitNewCompare);
}

// shopmain.cpp:8654
void CShopMain::Build_Hair(subTab_t tab, int) {
	typedef std::map<unsigned int, IFF_STRUCT::sHairStyle> HairStyleMap;
	for (HairStyleMap::iterator it = ItemManager()->m_hairStyleMap.begin(); it != ItemManager()->m_hairStyleMap.end(); ++it) {
		IFF_STRUCT::sHairStyle &hair = it->second;
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)hair;
		if ((item.m_flags0 >> 5) != 0 && (m_standPetTypeId & 0x3FFFFFF) == hair.m_style
			&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
			FrListItem *listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
			if (IsItemInCart(listItem)) {
				m_itemListBox->SelectItem(listItem, true);
			}
		}
	}

	m_itemListBox->SortItem(SpecialHitNewCompare);
}

// shopmain.cpp:8690
void CShopMain::Build_Parts(subTab_t tab, int) {
	unsigned long categoryFlag = GetCategoryFlag(tab);

	if (tab == 0x1A) {
		std::list<sDisplayUccClothes> &clothesList = UccManager()->m_displayClothesList;

		for (std::list<sDisplayUccClothes>::iterator it = clothesList.begin(); it != clothesList.end(); ++it) {
			sDisplayUccClothes &clothes = *it;
			if (((clothes.m_typeId >> 18) & 0xFF) == (m_standPetTypeId & 0x3FFFFFF)) {
				m_itemListBox->AddItem(&clothes);
			}
		}
	}

	typedef std::map<unsigned int, IFF_STRUCT::sPart> PartMap;
	for (PartMap::iterator it = ItemManager()->m_partMap.begin(); it != ItemManager()->m_partMap.end(); ++it) {
		IFF_STRUCT::sPart &part = it->second;
		if (S5::UnderCategory(m_standPetTypeId, part.m_typeId, categoryFlag)) {
			if ((part.m_flags & 0xE0) != 0
				&& ItemManager()->IsInSalePeriod(part.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
				if (!IsAngelWing(part.m_typeId) || (IsMannerPlayer() && WSingleton<CSharedDoc>::Instance()->m_myLevel >= 0x15)) {
					FrListItem *listItem = m_itemListBox->AddItem(&part);
					AddSellMiniButton((IFF_ITEM_COMMON &)part, listItem);
					if (listItem && IsItemInCart(listItem)) {
						m_itemListBox->SelectItem(listItem, true);
					}
				}
			}
		}
	}

	m_itemListBox->SortItem(PartCompare);
}

// shopmain.cpp:8794
void CShopMain::Build_Clubset(subTab_t tab, int) {
	FrListItem *listItem;

	typedef std::map<unsigned int, IFF_STRUCT::sClubSet> ClubSetMap;
	for (ClubSetMap::iterator it = ItemManager()->m_clubSetMap.begin(); it != ItemManager()->m_clubSetMap.end(); ++it) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
		if ((item.m_flags0 >> 5) != 0 && ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
			listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	typedef std::map<unsigned int, IFF_STRUCT::sSetItem> SetItemMap;
	for (SetItemMap::iterator it2 = ItemManager()->m_setItemMap.begin(); it2 != ItemManager()->m_setItemMap.end(); ++it2) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it2->second;
		if ((item.m_flags0 >> 5) != 0 && (item.m_typeId & 0x3E00000) == 0x800000
			&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
			listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	m_itemListBox->SortItem(ShopEmptyCompare);
	m_itemListBox->SortItem(SpecialHitNewCompare);
}

// shopmain.cpp:8833
void CShopMain::Build_Aztec(subTab_t tab, int) {
	FrListItem *listItem;

	typedef std::map<unsigned int, IFF_STRUCT::sBall> BallMap;
	for (BallMap::iterator it = ItemManager()->m_ballMap.begin(); it != ItemManager()->m_ballMap.end(); ++it) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
		if ((item.m_flags0 >> 5) != 0 && ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
			listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	typedef std::map<unsigned int, IFF_STRUCT::sSetItem> SetItemMap;
	for (SetItemMap::iterator it2 = ItemManager()->m_setItemMap.begin(); it2 != ItemManager()->m_setItemMap.end(); ++it2) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it2->second;
		if ((item.m_flags0 >> 5) != 0 && (item.m_typeId & 0x3E00000) == 0xA00000
			&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
			listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	m_itemListBox->SortItem(ShopEmptyCompare);
	m_itemListBox->SortItem(SpecialHitNewCompare);
}

// shopmain.cpp:8870
void CShopMain::Build_Equip(subTab_t tab, int) {
	FrListItem *listItem;

	typedef std::map<unsigned int, IFF_STRUCT::sItem> ItemMap;
	for (ItemMap::iterator it = ItemManager()->m_itemMap.begin(); it != ItemManager()->m_itemMap.end(); ++it) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
		if ((item.m_flags0 >> 5) != 0 && !(item.m_typeId & 0x2000000)
			&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
			listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	typedef std::map<unsigned int, IFF_STRUCT::sSetItem> SetItemMap;
	for (SetItemMap::iterator it2 = ItemManager()->m_setItemMap.begin(); it2 != ItemManager()->m_setItemMap.end(); ++it2) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it2->second;
		if ((item.m_flags0 >> 5) != 0 && (item.m_typeId & 0x3E00000) == 0xC00000 && (item.m_typeId & 0x1FE000) == 0x1FC000
			&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
			listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	m_itemListBox->SortItem(ShopEmptyCompare);
	m_itemListBox->SortItem(SpecialHitNewCompare);
}

// shopmain.cpp:8910
void CShopMain::Build_NoEquip(subTab_t tab, int) {
	FrListItem *listItem;

	typedef std::map<unsigned int, IFF_STRUCT::sItem> ItemMap;
	for (ItemMap::iterator it = ItemManager()->m_itemMap.begin(); it != ItemManager()->m_itemMap.end(); ++it) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;

		if ((item.m_flags0 >> 5) != 0 && (item.m_typeId & 0x2000000)
			&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {

			listItem = m_itemListBox->AddItem(&item);
			if ((item.m_flags0 >> 5) == 1 || (item.m_flags0 >> 5) == 3) {

				if ((item.m_flags0 >> 5) == 7) {

					listItem->AddNoRBtnIdx(1);
				}
			}

			if ((item.m_flags0 >> 5) == 1 || (item.m_flags0 >> 5) == 2) {

				if ((item.m_flags0 & 1) && (item.m_flags0 >> 5) == 7) {

					listItem->AddNoRBtnIdx(2);
				}
			}
		}
	}

	typedef std::map<unsigned int, IFF_STRUCT::sSetItem> SetItemMap;
	for (SetItemMap::iterator it2 = ItemManager()->m_setItemMap.begin(); it2 != ItemManager()->m_setItemMap.end(); ++it2) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it2->second;
		if ((item.m_flags0 >> 5) != 0 && (item.m_typeId & 0x3E00000) == 0xC00000 && (item.m_typeId & 0x1FE000) == 0x1FE000
			&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
			listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	m_itemListBox->SortItem(ShopEmptyCompare);
	m_itemListBox->SortItem(SpecialHitNewCompare);
}

// shopmain.cpp:9007
void CShopMain::Build_Card(subTab_t tab, int) {
	FrListItem *listItem;

	typedef std::map<unsigned int, IFF_STRUCT::sCard> CardMap;
	for (CardMap::iterator it = ItemManager()->m_cardMap.begin(); it != ItemManager()->m_cardMap.end(); ++it) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
		if ((item.m_flags0 >> 5) != 0 && !(item.m_typeId & 0x2000000)
			&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
			listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	typedef std::map<unsigned int, IFF_STRUCT::sSetItem> SetItemMap;
	for (SetItemMap::iterator it2 = ItemManager()->m_setItemMap.begin(); it2 != ItemManager()->m_setItemMap.end(); ++it2) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it2->second;
		if ((item.m_flags0 >> 5) != 0 && (item.m_typeId & 0x3E00000) == 0x1000000
			&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
			listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	m_itemListBox->SortItem(ShopEmptyCompare);
	m_itemListBox->SortItem(SpecialHitNewCompare);
}

// shopmain.cpp:9115
void CShopMain::Build_Interior(subTab_t tab, int) {
	unsigned long category = GetCategoryFlag(tab);
	typedef std::map<unsigned int, IFF_STRUCT::sFurniture> FurnitureMap;
	for (FurnitureMap::iterator it = ItemManager()->m_furnitureMap.begin(); it != ItemManager()->m_furnitureMap.end(); ++it) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
		if ((item.m_flags0 >> 5) != 0 && !(item.m_typeId & 0x2000000)
			&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
			if (((item.m_typeId >> 11) & 0x7FF) == category) {
				FrListItem *listItem = m_itemListBox->AddItem(&item);
				AddSellMiniButton(item, listItem);
			}
		}
	}

	m_itemListBox->SortItem(ShopEmptyCompare);
	m_itemListBox->SortItem(SpecialHitNewCompare);
}

// shopmain.cpp:9136
void CShopMain::Build_Furniture(subTab_t tab, int) {
	typedef std::map<unsigned int, IFF_STRUCT::sFurniture> FurnitureMap;
	for (FurnitureMap::iterator it = ItemManager()->m_furnitureMap.begin(); it != ItemManager()->m_furnitureMap.end(); ++it) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
		if ((item.m_flags0 >> 5) != 0 && !(item.m_typeId & 0x2000000)
			&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
			int category = (item.m_typeId >> 11) & 0x7FF;

			if ((category >= 0 && category < 7) || category >= 10) {

				FrListItem *listItem = m_itemListBox->AddItem(&item);
				if (listItem) {

					unsigned char shopFlag = item.m_flags0 >> 5;
					if (shopFlag == 1 || shopFlag == 3)
						listItem->AddNoRBtnIdx(1);
					shopFlag = item.m_flags0 >> 5;
					if (shopFlag == 1 || shopFlag == 2)
						listItem->AddNoRBtnIdx(2);
				}
			}
		}
	}

	m_itemListBox->SortItem(ShopEmptyCompare);
	m_itemListBox->SortItem(SpecialHitNewCompare);
}

// shopmain.cpp:9165
void CShopMain::Build_Decorate(subTab_t tab, int) {
	unsigned long category = GetCategoryFlag(tab);

	typedef std::map<unsigned int, IFF_STRUCT::sSkin> SkinMap;
	for (SkinMap::iterator it = ItemManager()->m_skinMap.begin(); it != ItemManager()->m_skinMap.end(); ++it) {
		if (category == 5 || ((it->second.m_typeId >> 22) & 0xF) == category) {
			if ((((IFF_ITEM_COMMON &)it->second).m_flags0 >> 5) != 0) {
				FrListItem *listItem = m_itemListBox->AddItem(&it->second);
				AddSellMiniButton((IFF_ITEM_COMMON &)it->second, listItem);
				if (IsItemInCart(listItem)) {
					m_itemListBox->SelectItem(listItem, true);

					if (!(it->second.m_typeId & 0x3C00000) && m_equipSkin[0]) {
						m_sideDir[0] = it->second.m_sideSlideMode[0];
						m_sideDir[1] = it->second.m_sideSlideMode[1];
						m_sidePos[0] = 0;
						m_sidePos[1] = 0;
					}
				}
			}
		}
	}

	m_itemListBox->SortItem(SkinCompare);
	m_itemListBox->SortItem(SpecialHitNewCompare);
}

// shopmain.cpp:9199
void CShopMain::Build_Cutin(subTab_t tab, int) {
	typedef std::map<unsigned int, IFF_STRUCT::sSkin> SkinMap;
	for (SkinMap::iterator it = ItemManager()->m_skinMap.begin(); it != ItemManager()->m_skinMap.end(); ++it) {
		if (S5::ISCutinItem(it->second.m_typeId)) {
			if ((((IFF_ITEM_COMMON &)it->second).m_flags0 >> 5) != 0) {
				IFF_STRUCT::S5::sCutinInformation *info = ItemManager()->FindCutinInformation(it->second.m_typeId);
				if (info) {
					if ((m_standPetTypeId & 0x3FFFFFF) == info->m_charSerial || info->m_charSerial >= 100) {
						FrListItem *listItem = m_itemListBox->AddItem(&it->second);
						AddSellMiniButton((IFF_ITEM_COMMON &)it->second, listItem);
						if (IsItemInCart(listItem))
							m_itemListBox->SelectItem(listItem, true);
					}
				}
			}
		}
	}

	m_itemListBox->SortItem(SkinCompare);
	m_itemListBox->SortItem(SpecialHitNewCompare);
}

// shopmain.cpp:9235
void CShopMain::Build_Caddie(subTab_t tab, int) {
	FrListItem *listItem;

	typedef std::map<unsigned int, IFF_STRUCT::sCaddie> CaddieMap;
	for (CaddieMap::iterator it = ItemManager()->m_caddieMap.begin(); it != ItemManager()->m_caddieMap.end(); ++it) {

		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
		unsigned char shopFlag = item.m_flags0 >> 5;
		if (shopFlag != 0 && shopFlag != 5 && shopFlag != 6
			&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())
			&& item.m_typeId == m_caddieTypeId) {
			listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	typedef std::map<unsigned int, IFF_STRUCT::sCadItem> CadItemMap;
	for (CadItemMap::iterator it2 = ItemManager()->m_cadItemMap.begin(); it2 != ItemManager()->m_cadItemMap.end(); ++it2) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it2->second;
		if ((item.m_flags0 >> 5) != 0 && ((item.m_typeId >> 21) & 0x1F) == (m_caddieTypeId & 0x3FFFFFF)
			&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
			listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	m_itemListBox->SortItem(CaddieCompare);
	m_itemListBox->SortItem(SpecialHitNewCompare);
}

// shopmain.cpp:9262
void CShopMain::Build_Mascot(subTab_t tab, int) {
	typedef std::map<unsigned int, IFF_STRUCT::sMascot> MascotMap;
	for (MascotMap::iterator it = ItemManager()->m_mascotMap.begin(); it != ItemManager()->m_mascotMap.end(); ++it) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
		unsigned char shopFlag = item.m_flags0 >> 5;
		if (shopFlag != 0 && shopFlag != 5 && shopFlag != 6
			&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
			FrListItem *listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	m_itemListBox->SortItem(ShopEmptyCompare);
	m_itemListBox->SortItem(SpecialHitNewCompare);
}

// shopmain.cpp:9279
bool CShopMain::IsItemInCart(FrListItem *listItem) {
	IFF_ITEM_COMMON *item = (IFF_ITEM_COMMON *)listItem->pData;

	for (FrListBox::ITEM_LIST::iterator i = m_cartList->m_itemList.begin(); i != m_cartList->m_itemList.end(); ++i) {
		IFF_ITEM_COMMON *cartItem = (IFF_ITEM_COMMON *)(*i)->pData;
		if (cartItem == NULL)
			continue;

		if (item->m_typeId == cartItem->m_typeId)
			return true;
	}

	for (FrListBox::ITEM_LIST::iterator i = m_auxCartList->m_itemList.begin(); i != m_auxCartList->m_itemList.end(); ++i) {
		IFF_ITEM_COMMON *cartItem = (IFF_ITEM_COMMON *)(*i)->pData;
		if (cartItem == NULL)
			continue;

		if (item->m_typeId == cartItem->m_typeId)
			return true;
	}

	return false;
}

// shopmain.cpp:9306
void CShopMain::PutOnNBackupShoppersOldItem(unsigned long tid, CPartTidList &tidList, unsigned long id) {
	CPartTidList oldList = tidList;

	if (m_petExhibition && m_petExhibition->GetPetFrame()) {

		if (tidList.IsEquipped(tid, 0)) {

			m_petExhibition->GetPetFrame()->SetPart(&tidList, tid, 0);
		}

		m_petExhibition->ResetCameraRecoverTime();
		unsigned long changed = m_petExhibition->GetPetFrame()->SetPart(&tidList, tid, id);

		for (int i = 0; i < tidList.m_numTotalParts; i++, changed >>= 1) {

			if (changed & 1) {

				unsigned long oldTid = oldList.m_partTids[i];
				if (!(oldTid & 0x600) && (oldTid & 0xFC000000) == 0x8000000) {

					oldItem_t oldItem;
					oldItem.typeId = tid;
					oldItem.tid = oldTid;
					oldItem.id = oldList.m_partIds[i];
					m_shoppersOldItems.push_back(oldItem);
				}
			}
		}
	}
}

// shopmain.cpp:9339
void CShopMain::RestoreShoppersOldItem(unsigned long typeId, CPartTidList &tidList) {
	for (std::list<oldItem_t>::iterator it = m_shoppersOldItems.begin(); it != m_shoppersOldItems.end();) {
		if (it->typeId == typeId) {
			m_petExhibition->GetPetFrame()->SetPart(&tidList, it->tid, it->id);
			it = m_shoppersOldItems.erase(it);
		} else {
			++it;
		}
	}
}

// shopmain.cpp:9353
void CShopMain::ChangeSubTab(subTab_t tab, bool bForce) {
	if (!bForce && m_subTab == tab) {
		return;
	}

	// Whether the shopper's character has to be loaded again.
	bool reload = false;
	if (((m_subTab <= 5 || m_subTab == 16) && (tab == 13 || tab == 14 || tab == 15))
		|| (m_subTab == 33 && (tab == 13 || tab == 14 || tab == 15))
		|| ((m_subTab == 13 || m_subTab == 14 || m_subTab == 15 || m_subTab == 33) && m_shopTab == 0)
		|| (m_shopTab == 0 && m_subTab == 16 && tab == SUB_TAB_NONE)) {
		reload = true;
	}

	m_subTab = tab; m_selItemExpanded = false; m_itemListBox->SetMultiSelect(tab == 4 || tab == 26);

	BuildDisplayStand(m_subTab);

	m_tabSubButton->Select(m_subTab);

	switch (m_subTab) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 5:
	case 26:
		m_statBox->SetVisible(true); SetLevelBar();
		if (reload) {
			SafeLoadPet(&m_standPetTypeId, reload);
		}
		break;

	case 4:
		TurnOnItemStatus();
		if (reload) {
			SafeLoadPet(&m_standPetTypeId, reload);
		}
		break;

	case 6:
		m_selectedName->SetVisible(true);

		m_selectedName->SetCaption11("\xbc\xb1\xc5\xc3\xc7\xd1 \xc5\xac\xb7\xb4");

		TurnOnItemStatus();
		SafeLoadPet(&m_clubsetTypeId, false);
		break;

	case 7:
		TurnOffItemStatus();
		SafeLoadPet(&m_aztecTypeId, false);
		break;

	case 27:
		if (IsLocalContent((localContentType_t)0x51)) {
			TurnOffItemStatus();
			m_anotherHand.WaitUntilWorkDone(9999); if (m_petExhibition) { delete m_petExhibition; m_petExhibition = NULL; }
		}
		break;

	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
		if (IsLocalContent((localContentType_t)0x56)) {
			if (IsLocalContent((localContentType_t)0x61)) {
				TurnOffItemStatus();
				m_anotherHand.WaitUntilWorkDone(9999); if (m_petExhibition) { delete m_petExhibition; m_petExhibition = NULL; }
			}
		}
		break;

	case 10:
		TurnOnItemStatus();
		break;

	case 12:
		TurnOffItemStatus();
		break;

	case 13:
		TurnOffItemStatus();

		SafeLoadPet(&m_standPetTypeId, reload);
		break;

	case 14:
		TurnOffItemStatus();
		SafeLoadPet(&m_standPetTypeId, reload);
		break;

	case 15:
		TurnOffItemStatus();

		SafeLoadPet(&m_standPetTypeId, reload);
		break;

	case 16:
		TurnOffItemStatus();

		SafeLoadPet(&m_standPetTypeId, reload);
		break;

	case 8:
	case 9:
	case 34:
		TurnOffItemStatus();

		m_anotherHand.WaitUntilWorkDone(9999); if (m_petExhibition) { delete m_petExhibition; m_petExhibition = NULL; }
		break;
	}

	SetMyItemSlot();
	SetItemDescRect();
	SetPetAreaRect();
	SetAutoScroll();
}

// shopmain.cpp:9515
void CShopMain::SetPetAreaRect(void) {
	switch (m_subTab) {
	case 13:
	case 14:
	case 15:
		m_petArea = m_skinPet;
		break;
	case 16:
	case 33:
		m_petArea = m_skinPet2;
		break;
	default:
		m_petArea = m_pet;
		break;
	}
}

// shopmain.cpp:9538
void CShopMain::SetMyItemSlot(void) {
	switch (m_subTab) {
	case 9:
		BuildMyItemSlot(true);
		m_itemSlotFrame1->SetVisible(true);
		m_itemSlotFrame2->SetVisible(true);
		m_itemSlot->SetVisible(true);
		break;
	case 8:
		BuildMyItemSlot(false);
		m_itemSlotFrame1->SetVisible(true);
		m_itemSlotFrame2->SetVisible(true);
		m_itemSlot->SetVisible(true);
		break;
	default:
		m_itemSlotFrame1->SetVisible(false);
		m_itemSlotFrame2->SetVisible(false);
		m_itemSlot->SetVisible(false);
		break;
	}
}

// shopmain.cpp:9562
void CShopMain::SetAutoScroll(void) {
	FrListItem *selected = m_itemListBox->GetSelected();

	int index = 0;
	if (selected) {
		for (FrListBox::ITEM_LIST::iterator i = m_itemListBox->m_itemList.begin(); i != m_itemListBox->m_itemList.end(); ++i) {
			if (*i == selected) break;
			index++;
		}
	}

	m_itemListBox->ResetTopRow(); m_itemListBox->GetScrollBar()->SetCurTopRow(index / 2);
}

// shopmain.cpp:9583
void CShopMain::ChangeMainTab(mainTab_t tab, bool bForce) {
	if (bForce) {
		m_tabMainButton->Select(m_shopTab);
	} else if (m_shopTab == tab) {
		return;
	}

	m_shopTab = tab;
	if (tab != MAIN_TAB_0) {
		if (!m_sideTabBuy || !m_sideTabRental)
			return;

		if (m_sideTabBuy->GetStatus() != FrButton::PRESSED) {
			m_sideTabBuy->SetStatus(FrButton::PRESSED);
			m_sideTabRental->SetStatus(FrButton::NORMAL);

			m_miniBtns[2].enable = true;
		}
	}

	m_tabSubButton->ClearTab();

	const char *imgs[3] = { NULL, };
	int i;
	switch (m_shopTab) {
	case 0:
		m_tabTxt1->SetBgImg("mt_character");
		m_buyAll->SetVisible(true);
		m_buyAll->Enable(false);
		m_buyReset->SetVisible(true);
		m_buyReset->Enable(false);

		for (i = 0; i <= 5; i++) {
			SetSubTabImg(imgs, i + 1);
			m_tabSubButton->AddTab(imgs, i, NULL);
		}

		imgs[0] = MakeStr("stext_%d_n", 27); imgs[1] = MakeStr("stext_%d_n", 27); imgs[2] = MakeStr("stext_%d_n", 27);
		m_tabSubButton->AddTab(imgs, 26, NULL);
		break;

	case 2:
		m_tabTxt1->SetBgImg("mt_caddie");
		m_buyAll->SetVisible(false);
		m_buyReset->SetVisible(false);

		SetSubTabImg(imgs, 11);
		m_tabSubButton->AddTab(imgs, 10, NULL);
		break;

	case 1:
		m_tabTxt1->SetBgImg("mt_item");
		m_selectedName->SetVisible(false);
		m_buyAll->SetVisible(false);
		m_buyReset->SetVisible(false);

		for (i = 6; i <= 9; i++) {
			SetSubTabImg(imgs, i + 1);
			m_tabSubButton->AddTab(imgs, i, NULL);
		}

		if (IsLocalContent((localContentType_t)0x51)) {
			SetSubTabImg(imgs, 28);
			m_tabSubButton->AddTab(imgs, 27, NULL);
		}
		break;

	case 3:
		m_tabTxt1->SetBgImg("mt_mascot");
		m_buyAll->SetVisible(false);
		m_buyReset->SetVisible(false);

		SetSubTabImg(imgs, 13);
		m_tabSubButton->AddTab(imgs, 12, NULL);
		break;

	case 4:
		m_tabTxt1->SetBgImg("mt_skin");
		m_buyAll->SetVisible(true);
		m_buyReset->SetVisible(true);

		for (i = 13; i <= 16; i++) {
			SetSubTabImg(imgs, i + 1);
			m_tabSubButton->AddTab(imgs, i, NULL);
		}

		imgs[0] = MakeStr("stext_%d_n", 34); imgs[1] = MakeStr("stext_%d_n", 34); imgs[2] = MakeStr("stext_%d_n", 34);
		m_tabSubButton->AddTab(imgs, 33, NULL);

		imgs[0] = MakeStr("stext_%d_n", 35); imgs[1] = MakeStr("stext_%d_n", 35); imgs[2] = MakeStr("stext_%d_n", 35);
		m_tabSubButton->AddTab(imgs, 34, NULL);
		break;

	case 5:
		m_tabTxt1->SetBgImg("mt_interior");
		m_buyAll->SetVisible(false);
		m_buyReset->SetVisible(false);

		SetSubTabImg(imgs, 33);
		m_tabSubButton->AddTab(imgs, 32, NULL);
		for (i = 28; i < 32; i++) {
			SetSubTabImg(imgs, i + 1);
			m_tabSubButton->AddTab(imgs, i, NULL);
		}
		break;
	}

	switch (m_shopTab) {
	case 0:
	case 2:
		m_selItem->SetVisible(true); m_selItemPrev->SetVisible(true); m_selItemNext->SetVisible(true);
		break;
	default:
		m_selItem->SetVisible(false); m_selItemPrev->SetVisible(false); m_selItemNext->SetVisible(false);
		break;
	}

	m_itemDesc->SetVisible(false);
	m_itemDescBg->SetVisible(false);

	m_statBox->SetVisible(false); SetLevelBar();
	m_scrollUp->SetVisible(false); m_scrollDown->SetVisible(false);

	ClearCartList(false, NULL);
}

// shopmain.cpp:9758
void CShopMain::ChangeStatTab(statTab_t tab) {
	m_statTab = tab;
}

// shopmain.cpp:9763
void CShopMain::ChangeCharacter(unsigned long typeId) {
	typedef std::map<unsigned int, sCharacterInfo> CharacterMap;
	CharacterMap::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++it) {
		if (it->second.typeId == typeId)
			break;
	}

	m_buyAll->Enable(false);
	m_buyReset->Enable(false);

	if (m_standPetTypeId != typeId) {
		m_standPetTypeId = typeId;
		m_cartList->ClearItem();
		m_auxCartList->ClearItem();
	}

	if (m_subTab == 0 || m_subTab == 1 || m_subTab == 2 || m_subTab == 3 || m_subTab == 4 || m_subTab == 5 || m_subTab == 0x1A) {
		bool bUccWinner = false;

		if (m_subTab == 0x1A && typeId == UccManager()->GetWinnerCharacterTypeId()
			&& ((__rtti_obj = AfxGetPreservedTask()) != 0 ? ((IObject*)__rtti_obj)->IsExactKindOf(&CLobbyTask::m_RTTI) : false))
			bUccWinner = true;

		if (it != WSingleton<CSharedDoc>::Instance()->m_characterMap.end()) {
			WSingleton<CSharedDoc>::Instance()->BuildMyPartTidList(it->second);

			m_charParts = it->second.partTypeIds;
			m_charCards = it->second.auxParts.tids;
			m_charLevels = (const char *)&it->second.unknown6A;
			m_hairColor = it->second.hairColor;
			if (m_subTab == 0) {
				ChangeSubTab((subTab_t)1, false);
				m_tabSubButton->Select(m_subTab);
			}
		} else {
			WSingleton<CSharedDoc>::Instance()->BuildMyPartTidListDefault(typeId);
			if (!bUccWinner)
				ChangeSubTab((subTab_t)0, false);
			m_tabSubButton->Select(m_subTab);
			m_hairColor = 0;
		}

		ClearCartList(true, &WSingleton<CSharedDoc>::Instance()->GetMyPartTidList());
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_standPetTypeId, false);
		m_statBox->SetVisible(true); SetLevelBar();

		m_selItemExpanded = false;

		typedef std::map<unsigned int, IFF_STRUCT::sChar> CharMap;
		for (CharMap::iterator cit = ItemManager()->m_charMap.begin(); cit != ItemManager()->m_charMap.end(); ++cit) {
			if (cit->second.m_typeId == typeId) {
				++cit;
				if (cit == ItemManager()->m_charMap.end())
					m_selItemNext->Enable(false);
				else
					m_selItemNext->Enable(true);
				break;
			}
		}

		for (CharMap::reverse_iterator rit = ItemManager()->m_charMap.rbegin(); rit != ItemManager()->m_charMap.rend(); ++rit) {
			if (rit->second.m_typeId == typeId) {
				++rit;
				if (rit == ItemManager()->m_charMap.rend())
					m_selItemPrev->Enable(false);
				else
					m_selItemPrev->Enable(true);
				break;
			}
		}

		if (typeId) {
			const char *name = ItemManager()->GetItemName(typeId);
			if (name)
				m_selectedName->SetCaption11(name);
		}
	} else if (m_shopTab == 4) {
		WSingleton<CSharedDoc>::Instance()->BuildMyPartTidList(it->second);
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_standPetTypeId, false);
	}

	SetAutoScroll();
}

// shopmain.cpp:9877
void CShopMain::ChangeCaddie(unsigned long typeId) {
	m_caddieTypeId = typeId;

	if (m_subTab == 10) {
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_caddieTypeId, false);
		m_statBox->SetVisible(true); SetLevelBar();

		m_selItemExpanded = false;

		typedef std::map<unsigned int, IFF_STRUCT::sCaddie> CaddieMap;
		for (CaddieMap::iterator it = ItemManager()->m_caddieMap.begin(); it != ItemManager()->m_caddieMap.end(); ++it) {
			if (it->second.m_typeId == typeId) {
				while (++it != ItemManager()->m_caddieMap.end()) {
					const IFF_STRUCT::sCaddie &caddie = it->second;
					if ((caddie.flags >> 5) != 0 && (caddie.flags >> 5) != 5 && (caddie.flags >> 5) != 6
						&& ItemManager()->IsInSalePeriod(caddie.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime()))
						break;
				}
				if (it == ItemManager()->m_caddieMap.end())
					m_selItemNext->Enable(false);
				else
					m_selItemNext->Enable(true);
				break;
			}
		}

		for (CaddieMap::reverse_iterator rit = ItemManager()->m_caddieMap.rbegin(); rit != ItemManager()->m_caddieMap.rend(); ++rit) {
			if (rit->second.m_typeId == typeId) {
				while (++rit != ItemManager()->m_caddieMap.rend()) {
					const IFF_STRUCT::sCaddie &caddie = rit->second;
					if ((caddie.flags >> 5) != 0 && (caddie.flags >> 5) != 5 && (caddie.flags >> 5) != 6
						&& ItemManager()->IsInSalePeriod(caddie.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime()))
						break;
				}
				if (rit == ItemManager()->m_caddieMap.rend())
					m_selItemPrev->Enable(false);
				else
					m_selItemPrev->Enable(true);
				break;
			}
		}

		if (typeId) {
			const char *name = ItemManager()->GetItemName(typeId);
			if (name)
				m_selectedName->SetCaption11(name);
		}
	}
}

// shopmain.cpp:9932
void CShopMain::ChangeMascot(unsigned long typeId) {
	m_mascotTypeId = typeId;

	if (m_subTab == SUB_TAB_MASCOT) {
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_mascotTypeId, false);
		TurnOffItemStatus();
	}
}

// shopmain.cpp:9978
void CShopMain::ChangeSkin(unsigned long typeId) {
	m_standPetTypeId = typeId;

	SafeLoadPet(&m_standPetTypeId, false);

	memcpy(m_equipSkin, WSingleton<CSharedDoc>::Instance()->m_equipSkin, sizeof(m_equipSkin));

	for (int i = 0; i < 6; i++) {
		if (m_equipSkin[i]) {
			IFF_STRUCT::sSkin *skin = ItemManager()->FindSkin(m_equipSkin[i]);
			if (skin && !(m_equipSkin[i] & 0x3C00000)) {
				m_sideDir[0] = skin->m_sideSlideMode[0];
				m_sideDir[1] = skin->m_sideSlideMode[1];
				m_sidePos[0] = 0;
				m_sidePos[1] = 0;
			}
		}
	}
}

// shopmain.cpp:9998
void CShopMain::ChangeClubset(unsigned long typeId) {
	m_clubsetTypeId = typeId;

	if (m_subTab == SUB_TAB_CLUBSET) {
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_clubsetTypeId, false);
		TurnOnItemStatus();
	}
}

// shopmain.cpp:10009
void CShopMain::ChangeAztec(unsigned long typeId) {
	m_aztecTypeId = typeId;

	if (m_subTab == SUB_TAB_AZTEC) {
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_aztecTypeId, false);
		TurnOffItemStatus();
	}
}

// shopmain.cpp:10020
void CShopMain::TurnOnItemSelect(void) {
	m_selItem->SetVisible(true);
	m_selItemPrev->SetVisible(true);
	m_selItemNext->SetVisible(true);
}

// shopmain.cpp:10027
void CShopMain::TurnOffItemSelect(void) {
	m_selItem->SetVisible(false);
	m_selItemPrev->SetVisible(false);
	m_selItemNext->SetVisible(false);
}

// shopmain.cpp:10034
const Bitmap *CShopMain::GetItemIcon(unsigned long typeId) {
	const Bitmap *bitmap = NULL;

	std::map<unsigned long, const Bitmap *>::iterator i = m_itemIcons.find(typeId);
	if (i == m_itemIcons.end()) {
		IFF_ITEM_COMMON *item = ItemManager()->FindCommonItem(typeId);
		if (item) {
			bitmap = g_pFresh->RegisterBitmap(item->m_icon);
			if (bitmap) {
				m_itemIcons.insert(std::map<unsigned long, const Bitmap *>::value_type(item->m_typeId, bitmap));
			}
		}
	} else {
		bitmap = i->second;
	}

	return bitmap;
}

// shopmain.cpp:10053
void CShopMain::DrawSelItem(FrGraphicInterface *graphic, unsigned long typeId) {
	WRect rect = m_selItemIcon->GetRect();

	const Bitmap *bitmap = GetItemIcon(typeId);
	if (bitmap == NULL)
		return;

	unsigned long color = 0xffffffff;

	switch (typeId >> 26) {
	case 1: {
		std::map<unsigned int, sCharacterInfo>::const_iterator it;
		for (it = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++it) {
			if (it->second.typeId == typeId)
				break;
		}
		if (it == WSingleton<CSharedDoc>::Instance()->m_characterMap.end())
			color = 0xff808080;
		break;
	}
	case 4: {
		std::map<unsigned int, sCaddieInfo>::const_iterator it;
		for (it = WSingleton<CSharedDoc>::Instance()->m_caddieMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end(); ++it) {
			if (it->second.typeId == typeId)
				break;
		}
		if (it == WSingleton<CSharedDoc>::Instance()->m_caddieMap.end())
			color = 0xff808080;
		break;
	}
	case 5: {
		std::list<sItemInfo>::iterator it;
		for (it = WSingleton<CSharedDoc>::Instance()->m_myGearList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myGearList.end(); ++it) {
			if (it->m_typeId == typeId)
				break;
		}
		if (it == WSingleton<CSharedDoc>::Instance()->m_myGearList.end())
			color = 0xff808080;
		break;
	}
	case 7: {
		std::map<unsigned int, sItemInfo>::const_iterator it;
		for (it = WSingleton<CSharedDoc>::Instance()->m_myItemMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myItemMap.end(); ++it) {
			if (it->second.m_typeId == typeId)
				break;
		}
		if (it == WSingleton<CSharedDoc>::Instance()->m_myItemMap.end())
			color = 0xff808080;
		break;
	}
	case 16: {
		std::map<unsigned int, sMascotInfo>::const_iterator it;
		for (it = WSingleton<CSharedDoc>::Instance()->m_myMascotMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myMascotMap.end(); ++it) {
			if (it->second.typeId == typeId)
				break;
		}
		if (it == WSingleton<CSharedDoc>::Instance()->m_myMascotMap.end())
			color = 0xff808080;
		break;
	}
	case 18: {
		std::list<sFurniture_List>::iterator it;
		for (it = WSingleton<CSharedDoc>::Instance()->m_furnitureList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_furnitureList.end(); ++it) {
			if (it->m_typeId == typeId)
				break;
		}
		if (it == WSingleton<CSharedDoc>::Instance()->m_furnitureList.end())
			color = 0xff808080;
		break;
	}
	}

	graphic->DrawTexture(bitmap, rect, color, 0);
}

// shopmain.cpp:10187
void CShopMain::DrawCharacterList(FrGraphicInterface *graphic, unsigned long typeId) {
	if (m_selItemExpanded && ItemManager()->m_charMap.size() != 0) {
		typedef std::map<unsigned int, IFF_STRUCT::sChar> CharMap;
		int count = 0;
		int drawn = 0;
		for (CharMap::const_iterator cit = ItemManager()->m_charMap.begin(); cit != ItemManager()->m_charMap.end(); ++cit) {
			unsigned char shopFlag = ((const IFF_ITEM_COMMON &)cit->second).m_flags0 >> 5;
			if (shopFlag != 0 && shopFlag != 5 && shopFlag != 6
				&& ItemManager()->IsInSalePeriod(cit->second.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime()))
				count++;
		}

		int cols = count > 16 ? 4 : count > 4 ? 3 : (count > 1) + 1;
		int rows = count / cols + (count % cols != 0);

		float x = 18.0f;
		float height = rows * 50.0f + (rows - 1);
		float y = 419.0f - height;

		graphic->DrawTexture(m_selectUp, WRect(13.0f, y - 5.0f, cols * 46.0f + (cols - 1) + 10.0f, height + 10.0f), 0xFFFFFFFF, 0);

		WRect selectedRect(0.0f, 0.0f, 0.0f, 0.0f);
		WRect hoverRect(0.0f, 0.0f, 0.0f, 0.0f);

		for (CharMap::iterator it = ItemManager()->m_charMap.begin(); it != ItemManager()->m_charMap.end(); ++it) {
			unsigned char shopFlag = ((const IFF_ITEM_COMMON &)it->second).m_flags0 >> 5;
			if (shopFlag != 0 && shopFlag != 5 && shopFlag != 6
				&& ItemManager()->IsInSalePeriod(it->second.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
				unsigned long charTypeId = it->second.m_typeId;
				const Bitmap *bitmap = g_pFresh->GetBitmap(it->second.m_icon);
				if (bitmap) {
					WRect rect(x, y, 46.0f, 50.0f);

					drawn++;
					unsigned long color = 0xFFFFFFFF;
					std::map<unsigned int, sCharacterInfo>::const_iterator mit;
					for (mit = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin(); mit != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++mit) {
						if (mit->second.typeId == charTypeId)
							break;
					}
					if (mit == WSingleton<CSharedDoc>::Instance()->m_characterMap.end())
						color = 0xFF808080;

					graphic->DrawTexture(bitmap, rect, color, 0);

					if (drawn % cols) {
						x += 47.0f;
					} else {
						x = 18.0f;
						y += 51.0f;
					}

					if (m_standPetTypeId == charTypeId)
						selectedRect = WRect(rect.x - 4.0f, rect.y - 4.0f, rect.w + 8.0f, rect.h + 8.0f);

					if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
						hoverRect = WRect(rect.x - 4.0f, rect.y - 4.0f, rect.w + 8.0f, rect.h + 8.0f);
						if (g_input->GetButton(LEFT_BUTTON) == 1) {
							if (m_standPetTypeId != charTypeId)
								ChangeCharacter(charTypeId);
						}
					}
				}
			}
		}

		if (selectedRect.w != 0.0f)
			graphic->DrawTexture(m_selectCursor1, selectedRect, 0xFFFFFFFF, 0);

		if (hoverRect.w != 0.0f)
			graphic->DrawTexture(m_selectCursor0, hoverRect, 0xFFFFFFFF, 0);
	}

	if (m_selItemExpanded && g_input->GetButton(LEFT_BUTTON) == 1) {
		WRect rect = m_selItem->GetRect();
		if (!rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
			m_selItemExpanded = false;
	}
}

// shopmain.cpp:10300
void CShopMain::DrawCaddieList(FrGraphicInterface *graphic, unsigned long typeId) {
	if (m_selItemExpanded && ItemManager()->m_caddieMap.size() != 0) {
		typedef std::map<unsigned int, IFF_STRUCT::sCaddie> CaddieMap;
		int count = 0;
		int drawn = 0;
		for (CaddieMap::const_iterator cit = m_visibleCaddies.begin(); cit != m_visibleCaddies.end(); ++cit) {
			unsigned char shopFlag = ((const IFF_ITEM_COMMON &)cit->second).m_flags0 >> 5;
			if (shopFlag != 0 && shopFlag != 5 && shopFlag != 6
				&& ItemManager()->IsInSalePeriod(cit->second.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime()))
				count++;
		}

		int cols = count > 16 ? 4 : count > 4 ? 3 : (count > 1) + 1;
		int rows = count / cols + (count % cols != 0);

		float x = 18.0f;
		float height = rows * 50.0f + (rows - 1);
		float y = 419.0f - height;

		graphic->DrawTexture(m_selectUp, WRect(13.0f, y - 5.0f, cols * 46.0f + (cols - 1) + 10.0f, height + 10.0f), 0xFFFFFFFF, 0);

		WRect selectedRect(0.0f, 0.0f, 0.0f, 0.0f);
		WRect hoverRect(0.0f, 0.0f, 0.0f, 0.0f);

		for (CaddieMap::iterator it = m_visibleCaddies.begin(); it != m_visibleCaddies.end(); ++it) {
			unsigned char shopFlag = ((const IFF_ITEM_COMMON &)it->second).m_flags0 >> 5;
			if (shopFlag != 0 && shopFlag != 5 && shopFlag != 6
				&& ItemManager()->IsInSalePeriod(it->second.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
				unsigned long caddieTypeId = it->second.m_typeId;
				const Bitmap *bitmap = g_pFresh->GetBitmap(it->second.m_icon);
				if (bitmap) {
					WRect rect(x, y, 46.0f, 50.0f);

					drawn++;
					unsigned long color = 0xFFFFFFFF;
					std::map<unsigned int, sCaddieInfo>::iterator mit = WSingleton<CSharedDoc>::Instance()->m_caddieMap.begin();
					std::map<unsigned int, sCaddieInfo>::iterator mend = WSingleton<CSharedDoc>::Instance()->m_caddieMap.end();
					for (; mit != mend; ++mit) {
						if (mit->second.typeId == caddieTypeId)
							break;
					}
					if (mit == mend)
						color = 0xFF808080;

					graphic->DrawTexture(bitmap, rect, color, 0);

					if (drawn % cols) {
						x += 47.0f;
					} else {
						x = 18.0f;
						y += 51.0f;
					}

					if (m_caddieTypeId == caddieTypeId)
						selectedRect = WRect(rect.x - 4.0f, rect.y - 4.0f, rect.w + 8.0f, rect.h + 8.0f);

					const WPoint &mousePos = g_pFresh->GetManager()->GetMousePos();
					if (mousePos.x >= rect.x && mousePos.x <= rect.x + rect.w && mousePos.y >= rect.y && mousePos.y <= rect.y + rect.h) {
						hoverRect = WRect(rect.x - 4.0f, rect.y - 4.0f, rect.w + 8.0f, rect.h + 8.0f);
						if (g_input->GetButton(LEFT_BUTTON) == 1)
							ChangeCaddie(caddieTypeId);
					}
				}
			}
		}

		if (selectedRect.w != 0.0f)
			graphic->DrawTexture(m_selectCursor1, selectedRect, 0xFFFFFFFF, 0);

		if (hoverRect.w != 0.0f)
			graphic->DrawTexture(m_selectCursor0, hoverRect, 0xFFFFFFFF, 0);
	}

	if (m_selItemExpanded && g_input->GetButton(LEFT_BUTTON) == 1) {
		WRect rect = m_selItem->GetRect();
		if (!rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
			m_selItemExpanded = false;
	}
}

// shopmain.cpp:10417
void CShopMain::DrawMascotList(FrGraphicInterface *graphic, unsigned long typeId) {
	if (m_selItemExpanded && ItemManager()->m_mascotMap.size() != 0) {
		int count = ItemManager()->m_mascotMap.size();

		int drawn = 0;
		int cols = count > 16 ? 4 : count > 4 ? 3 : (count > 1) + 1;
		int rows = count / cols + (count % cols != 0);

		float width = cols * 46.0f + (cols - 1) + 10.0f;
		float height = rows * 50.0f + (rows - 1);
		float y = 419.0f - height;
		float top = y - 5.0f;
		float x = 18.0f;

		graphic->DrawTexture(m_selectUp, WRect(13.0f, top, width, height + 10.0f), 0xFFFFFFFF, 0);

		WRect selectedRect(0.0f, 0.0f, 0.0f, 0.0f);
		WRect hoverRect(0.0f, 0.0f, 0.0f, 0.0f);
		graphic->DrawTexture(m_selectUp, WRect(13.0f, top, width, height + 25.0f), 0xFFFFFFFF, 0);

		typedef std::map<unsigned int, IFF_STRUCT::sMascot> MascotMap;
		for (MascotMap::const_iterator it = ItemManager()->m_mascotMap.begin(); it != ItemManager()->m_mascotMap.end(); ++it) {
			unsigned char shopFlag = ((const IFF_ITEM_COMMON &)it->second).m_flags0 >> 5;
			if (shopFlag != 0 && shopFlag != 5 && shopFlag != 6
				&& ItemManager()->IsInSalePeriod(it->second.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
				unsigned long mascotTypeId = it->second.m_typeId;
				const Bitmap *bitmap = g_pFresh->GetBitmap(it->second.m_icon);
				if (bitmap) {
					WRect rect(x, y, 46.0f, 50.0f);

					drawn++;
					unsigned long color = 0xFFFFFFFF;
					std::map<unsigned int, sMascotInfo>::const_iterator mit;
					for (mit = WSingleton<CSharedDoc>::Instance()->m_myMascotMap.begin(); mit != WSingleton<CSharedDoc>::Instance()->m_myMascotMap.end(); ++mit) {
						if (mit->second.typeId == mascotTypeId)
							break;
					}
					if (mit == WSingleton<CSharedDoc>::Instance()->m_myMascotMap.end())
						color = 0xFF808080;

					graphic->DrawTexture(bitmap, rect, color, 0);

					if (drawn % cols) {
						x += 47.0f;
					} else {
						y += 51.0f;
						x = 18.0f;
					}

					if (m_mascotTypeId == mascotTypeId)
						selectedRect = WRect(rect.x - 4.0f, rect.y - 4.0f, rect.w + 8.0f, rect.h + 8.0f);

					const WPoint &mousePos = g_pFresh->GetManager()->GetMousePos();
					if (mousePos.x >= rect.x && mousePos.x <= rect.x + rect.w && mousePos.y >= rect.y && mousePos.y <= rect.y + rect.h) {
						hoverRect = WRect(rect.x - 4.0f, rect.y - 4.0f, rect.w + 8.0f, rect.h + 8.0f);
						if (g_input->GetButton(LEFT_BUTTON) == 1)
							ChangeMascot(mascotTypeId);
					}
				}
			}
		}

		if (selectedRect.w != 0.0f)
			graphic->DrawTexture(m_selectCursor1, selectedRect, 0xFFFFFFFF, 0);

		if (hoverRect.w != 0.0f)
			graphic->DrawTexture(m_selectCursor0, hoverRect, 0xFFFFFFFF, 0);
	}

	if (m_selItemExpanded && g_input->GetButton(LEFT_BUTTON) == 1) {
		WRect rect = m_selItem->GetRect();
		if (!rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
			m_selItemExpanded = false;
	}
}

// shopmain.cpp:10529
void CShopMain::DrawSelArrow(FrGraphicInterface *graphic) {
}

// shopmain.cpp:10534
bool CShopMain::IsVisibleInStock(IFF_ITEM_COMMON &item) {
	unsigned char shopFlag = item.m_flags0 >> 5;
	if (shopFlag != 0 && shopFlag != 5 && shopFlag != 6
		&& ItemManager()->IsInSalePeriod(item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
		return true;
	}
	return false;
}

// shopmain.cpp:10542
void CShopMain::SafeLoadPet(unsigned long *pTypeId, bool bForce) {
	m_anotherHand.WaitUntilWorkDone(9999);

	if (!bForce) {
		if (m_petExhibition) {
			if (m_petExhibition->GetTypeId() == *pTypeId) {
				return;
			}
		}
	}

	m_anotherHand.Work(this, 0, pTypeId, true, -1);
}

// shopmain.cpp:10562
void CShopMain::ShowMainModel(CPartTidList &tidList) {
	SetLevelBar();

	m_anotherHand.Enter();
	if (m_petExhibition && !m_anotherHand.IsWorking(0)) {
		m_petExhibition->ResetCameraRecoverTime();
		m_petExhibition->ParseBoundingBoxScript();
		m_petExhibition->GetPetFrame()->Refresh(&tidList);
	}
	m_anotherHand.Leave();
}

// shopmain.cpp:10584
void CShopMain::LoadCardImages(void) {
	for (std::map<unsigned int, IFF_STRUCT::sCard>::const_iterator it = ItemManager()->m_cardMap.begin(); it != ItemManager()->m_cardMap.end(); ++it) {

		const IFF_STRUCT::sCard &card = it->second;
		if (m_cardImgs.find(card.m_typeId) == m_cardImgs.end()) {

			char path[64];
			sprintf(path, "%s.png", card.m_model);
			WOverlay *overlay = g_resrcmng->GetOverlay(path, 0x80000000);

			if (overlay) {
				m_cardImgs.insert(std::map<unsigned int, WOverlay *>::value_type(card.m_typeId, overlay));
			}
		}
	}

	m_cardCharacterEmpty = g_resrcmng->GetOverlay("card_character_empty.tga", 0);
	m_cardCaddieEmpty = g_resrcmng->GetOverlay("card_caddie_empty.tga", 0);
}

// shopmain.cpp:10605
void CShopMain::DeleteCardImages(void) {
	for (std::map<unsigned int, WOverlay *>::iterator it = m_cardImgs.begin(); it != m_cardImgs.end(); ++it) {

		if (g_resrcmng && it->second) { g_resrcmng->Release(it->second); it->second = 0; }
	}

	if (g_resrcmng && m_cardCharacterEmpty) { g_resrcmng->Release(m_cardCharacterEmpty); m_cardCharacterEmpty = 0; }
	if (g_resrcmng && m_cardCaddieEmpty) { g_resrcmng->Release(m_cardCaddieEmpty); m_cardCaddieEmpty = 0; }
}

// shopmain.cpp:10616
WOverlay *CShopMain::GetCardImg(unsigned long typeId) {
	WOverlay *overlay = NULL;
	std::map<unsigned int, WOverlay *>::iterator it = m_cardImgs.find(typeId);
	if (it == m_cardImgs.end()) {

		IFF_STRUCT::sCard *card = ItemManager()->FindCard(typeId);

		if (card) {
			char path[64];
			sprintf(path, "%s.png", card->m_model);
			overlay = g_resrcmng->GetOverlay(path, 0);
			if (overlay) {
				m_cardImgs.insert(std::map<unsigned int, WOverlay *>::value_type(card->m_typeId, overlay));
			}
		}
	} else {
		overlay = it->second;
	}

	return overlay;
}

// shopmain.cpp:10641
void CShopMain::DrawCard(void) {
	if (m_cardTypeId) {

		IFF_STRUCT::sCard *card;
		unsigned long cardTypeId;
		if ((m_cardTypeId >> 26) == 0x1F) {

			card = ItemManager()->FindCard(m_cardTypeId);
			cardTypeId = m_cardTypeId;
		} else if ((m_cardTypeId >> 26) == 9 && (m_cardTypeId & 0x1FE000) == 0x1FA000) {

			IFF_STRUCT::sSetItem *setItem = ItemManager()->FindSetItem(m_cardTypeId);
			if (!setItem)
				return;

			cardTypeId = setItem->m_itemTypeIds[0];
			card = ItemManager()->FindCard(cardTypeId);
		} else {
			return;
		}

		if (card && cardTypeId) {

			WOverlay *img = GetCardImg(cardTypeId);
			if (img) {
				img->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(92.0f, 117.0f, 248.0f, 499.0f), 0, 0xFFFFFFFF, 0.0f, 0);
			}
		}
	}
}

// shopmain.cpp:10753
void CShopMain::InitalizeUpgradeCaddieInfo(void) {
	m_upgradeCaddies.insert(std::map<unsigned long, unsigned long>::value_type(0x1C000001, 0x1C000010));
	m_upgradeCaddies.insert(std::map<unsigned long, unsigned long>::value_type(0x1C000002, 0x1C000011));
	m_upgradeCaddies.insert(std::map<unsigned long, unsigned long>::value_type(0x1C000003, 0x1C000012));
	m_upgradeCaddies.insert(std::map<unsigned long, unsigned long>::value_type(0x1C000004, 0x1C000013));
	m_upgradeCaddies.insert(std::map<unsigned long, unsigned long>::value_type(0x1C000005, 0x1C000014));
	m_upgradeCaddies.insert(std::map<unsigned long, unsigned long>::value_type(0x1C000006, 0x1C000015));
	m_upgradeCaddies.insert(std::map<unsigned long, unsigned long>::value_type(0x1C000007, 0x1C000016));
}

// shopmain.cpp:10765
void CShopMain::BuildVisibleCaddieList(void) {
	// A base caddie of m_upgradeCaddies shows only when the player owns it; its
	// upgrade shows unless the owned list meets the base caddie before the
	// upgrade. Every other caddie shows.
	m_visibleCaddies.clear();
	std::map<unsigned int, IFF_STRUCT::sCaddie>::iterator it = ItemManager()->m_caddieMap.begin();
	for (; it != ItemManager()->m_caddieMap.end(); ++it) {

		bool upgraded = false;
		const IFF_STRUCT::sCaddie &caddie = it->second;
		std::map<unsigned long, unsigned long>::iterator up;
		unsigned long baseTypeId;
		unsigned long upgradeTypeId;
		for (up = m_upgradeCaddies.begin(); up != m_upgradeCaddies.end(); ++up) {
			baseTypeId = up->first;
			upgradeTypeId = up->second;

			if (caddie.m_typeId == baseTypeId || caddie.m_typeId == upgradeTypeId) {
				break;
			}
		}

		if (up != m_upgradeCaddies.end()) {
			std::map<unsigned int, sCaddieInfo>::iterator owned = WSingleton<CSharedDoc>::Instance()->m_caddieMap.begin();
			if (caddie.m_typeId == baseTypeId) {
				bool ownsBase = false;
				for (; owned != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end(); ++owned) {

					if (owned->second.typeId == baseTypeId) {
						ownsBase = true;
						break;
					}
				}
				if (!ownsBase) {
					continue;
				}
			}
			else if (caddie.m_typeId == upgradeTypeId) {
				for (; owned != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end(); ++owned) {

					if (owned->second.typeId == baseTypeId) {
						break;
					}
					if (owned->second.typeId == upgradeTypeId) {
						upgraded = true;
						break;
					}
				}

				if (owned != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end() && !upgraded) {
					continue;
				}
			}
			else {
				continue;
			}
		}

		m_visibleCaddies.insert(std::map<unsigned int, IFF_STRUCT::sCaddie>::value_type(it->first, caddie));
	}
}

// shopmain.cpp:10840
void CShopMain::UpgradeCaddie(unsigned long caddieTypeId) {
	unsigned long upgradeTypeId = 0;
	std::map<unsigned long, unsigned long>::iterator up = m_upgradeCaddies.find(caddieTypeId);
	if (up != m_upgradeCaddies.end()) {
		upgradeTypeId = up->second;
	} else {
		this << MsgObject(0, 0x23, (intptr_t)"\xC7\xD8\xB4\xE7 \xC4\xB3\xB5\xF0\xB4\xC2 \xBE\xF7\xB1\xD7\xB7\xB9\xC0\xCC\xB5\xE5\xB0\xA1 \xBA\xD2\xB0\xA1\xB4\xC9 \xC7\xD5\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
	}

	short beforeLevels[5];
	short afterLevels[5];
	int beforeFee;
	int afterFee;

	typedef std::map<unsigned int, IFF_STRUCT::sCaddie> CaddieMap;
	CaddieMap::iterator before;
	CaddieMap::iterator after;
	before = ItemManager()->m_caddieMap.find(caddieTypeId);
	after = ItemManager()->m_caddieMap.find(upgradeTypeId);

	if (before != ItemManager()->m_caddieMap.end()) {
		memcpy(beforeLevels, before->second.m_levels, sizeof(beforeLevels));
		beforeFee = before->second.fee;
	}

	if (after != ItemManager()->m_caddieMap.end()) {
		memcpy(afterLevels, after->second.m_levels, sizeof(afterLevels));
		afterFee = after->second.fee;
	}

	int stats[5];
	stats[0] = afterLevels[0] - beforeLevels[0];
	stats[1] = afterLevels[1] - beforeLevels[1];
	stats[2] = afterLevels[2] - beforeLevels[2];
	stats[3] = afterLevels[3] - beforeLevels[3];
	stats[4] = afterLevels[4] - beforeLevels[4];

	m_upgradeCaddieDlg = CreateForm<FrUpgradeCaddieDlg>(g_pFresh->GetManager(), this, "upgradecaddie", NULL);
	if (m_upgradeCaddieDlg) {
		m_upgradeCaddieDlg->SetUpgradeInfo(caddieTypeId, stats, beforeFee, afterFee);
		m_upgradeCaddieDlg->Open(0, 3);
	}
}
