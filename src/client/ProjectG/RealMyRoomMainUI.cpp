// Original: d:\build\custom\temp\code\client\projectg\realmyroommainui.cpp
#define REALMYROOMMAINUI_EXHIBITION_INLINE
#define REALMYROOMMAINUI_CCARDBOOK_INLINE
#define REALMYROOMMAINUI_ITEMSTORAGE_INLINE inline
#define REALMYROOMMAINUI_SHAREDDOC_INLINE
#include "NetworkSystem.h"
#include "RealMyRoomMain.h"
#include "StandingInLine.h"
#include "LevelUpItemDlg.h"
#include "guilddefine.h"
#include "ClientSetting.h"
#include "RealMyRoom_Avatar.h"
#include "RealMyRoomData.h"
#include "InputManager.h"
#include "mousecursor.h"
#include "exhibition.h"
#include "client/Wangreal/include/wpuppet.h"
#include "client/Wangreal/include/wbone.h"
#include "RankingDlg.h"
#include "user_info.h"
#include "Post_SendDlg.h"
#include "Post_ReciveDlg.h"
#include "UccDrawDlg.h"
#include "ItemStorage.h"
#include "globalnetworkdefine.h"
#include "Packet.h"
#include "FriendDlg.h"
#include "FortuneDlg.h"
#include "FortuneOpenDlg.h"
#include "PangGiftDlg.h"
#include "ReturnGiftDlg.h"
#include "CaddieInfoDlg.h"
#include "EnchantDlg.h"
#include "DowngradeDlg.h"
#include "FrRandomBoxProgress.h"
#include "CCardBook.h"
#include "s5/RentalExtensionDlg.h"
#include "s5/Utilities.h"
#include "classdefine.h"
#include "frwnd.h"
#include "frcontextmenuctrl.h"
#include "frform.h"
#include "frarea.h"
#include "frbutton.h"
#include "frlistbox.h"
#include "frgaugebar.h"
#include "commonutil.h"
#include "Localize.h"
#include "ToolTip.h"
#include "frtabbutton.h"
#include "frscrollbar.h"
#include "fresh.h"
#include "SharedDoc.h"
#include "shared/classdefine.h"
#include "client/Wangreal/include/wview.h"
#include "client/Wangreal/include/woverlay.h"
#include "frgraphicinterface.h"
#include "shared/globalgamedefine.h"
#include "frdesktop.h"
#include "CardBoxDlg.h"
#include "CardDlg.h"
#include "shared/TitleManager.h"
#include "Titles_Client.h"
#include "Furniture_Ability.h"
#include "cardsystem.h"
#include "contentsdoc.h"
#include "frgroupbox.h"
#include "frstatic.h"
#include "SetItemDlg.h"
#include "frviewer.h"
#include "avatar_task.h"
#include "fredit.h"
#include "NoteDlg.h"
#include "MascotInfoDlg.h"
#include "UccManager.h"
#include "UccLibrary.h"
#include "SoundManager.h"
#include "wutil.h"
#include "partsinfo.h"
#include "RealMyRoom_AuthorityDlg.h"
#include "CardRemove.h"
#include "s5/SupplyPack.h"
#include "RealMyRoomTask.h"
#include "fx.h"
#include "fxsequence.h"
#include "mascot.h"
#include "ProjectG.h"
#include "ShopMain.h"
#include "GolfDoc.h"
#include "quadtree.h"
#include "polysoup.h"
#include "logininfo.h"
#include "UccBase.h"
#include "ResultDlg.h"
#include "IndexControl.h"
#include "CodeTinker.h"
#include "UccCopyDlg.h"
#include "DeleteItem.h"
#include "ReplayModeDlg.h"

extern WMatrix g_camera;
extern const char *dot_files[5][2];   // defined in ShopMain.cpp
extern TitleManager* g_TitleManager;   // defined in ProjectG.cpp

// Actor names in .data with no public symbol (the same pair as TaskManager.cpp,
// header of origin not recovered yet).
static const char* s_guildActorName = "GuildActor";
static const char* s_gatewayActorName = "Gateway";

// realmyroommain.h:1404
bool CRealMyRoomMain::IsItemStorageVisible(void) const {
	return m_itemStorageVisible;
}

// realmyroom_avatar.h:80
CPetFrame* CRealMyRoom_Avatar::GetPetFrame() {
	return m_pPetFrame;
}

// realmyroom_avatar.h:84
const char* CRealMyRoom_Avatar::GetIDName() const {
	return m_idName;
}

// realmyroom_avatar.h:85
const char* CRealMyRoom_Avatar::GetNickName() const {
	return m_nickName;
}

// realmyroom_avatar.h:94
const WVector& CRealMyRoom_Avatar::GetTargetPosition() const {
	return m_targetPosition;
}

// realmyroom_avatar.h:120
void CRealMyRoom_Avatar::SubState(unsigned long state) {
	SetState(m_state & ~state);
}

// fortunedlg.h:16
unsigned long FrFortuneDlg::GetTypeId(void) {
	return m_typeId;
}

// fortuneopendlg.h:99
unsigned long FrFortuneOpenDlg::GetTypeId(void) {
	return m_typeId;
}

// enchantdlg.h:24
void FrEnchantDlg::SetResultCode(unsigned char code) { m_resultCode = code; }

// panggiftdlg.h:17
sGiftInfo* FrPangGiftDlg::GetItemInfo(void) {
	return m_itemInfo;
}

// ccardbook.h:130
bool ICardBookPage::BoundSlotControl(unsigned char slot, FrButton* pButton) {
	bool result = false;
	if (m_pCardSlots != NULL) {
		result = m_pCardSlots[slot].BoundControl(pButton);
	}
	return result;
}

// ccardbook.h:324
CCardPackPage* CCardBook::GetCardPackPage(void) {
	return m_pCardPackPage;
}

// ccardbook.h:329
CCardPage* CCardBook::GetCardPage(void) {
	return m_pCardPage;
}

// ccardbook.h:334
void CCardBook::SetSelectedSlot(unsigned char slot) {
	m_selectedSlot = slot;
}

// ccardbook.h:350
bool CCardBook::IsShow(void) const {
	return m_show;
}

// ccardbook.h:313 (the instantiations this unit emits)
template bool CCardBook::BoundControl<FrArea*>(FrArea*& rControl, FrArea* pControl);
template bool CCardBook::BoundControl<FrListBox*>(FrListBox*& rControl, FrListBox* pControl);
template bool CCardBook::BoundControl<FrButton*>(FrButton*& rControl, FrButton* pControl);

// post_senddlg.h:35
void CPost_SendDlg::UpdateFriendList(void) {
	if (m_pFriendDlg) {
		m_pFriendDlg->UpdateFriendList();
	}
}

// post_senddlg.h:42
unsigned long CPost_SendDlg::GetSendUID(void) {
	return m_sendUID;
}

// uccdrawdlg.h:72
bool FrUccDrawDlg::IsInitialized(void) {
	return m_initialized;
}

// itemstorage.h:130
FrItemStorageAccessPanel::eFormType FrItemStorageAccessPanel::GetFormType(void) const {
	return m_formType;
}

// itemstorage.h:451
bool CItemStorage::IsLock(void) const {
	return m_lock;
}

// itemstorage.h:489
unsigned long CItemStorage::GetTotalPage(void) const {
	return m_totalPage;
}

// itemstorage.h:497
unsigned long CItemStorage::GetCurPage(void) const {
	return m_curPage;
}

// itemstorage.h:501
CIndexControl* CItemStorage::GetIndexControl(void) {
	return m_indexControl;
}

// itemstorage.h:541
void CItemStorage::SetItemStorageArea(WRect rect) {
	m_itemStorageArea = rect;
}

// itemstorage.h:542
void CItemStorage::SetItemListArea(WRect rect) {
	m_itemListArea = rect;
}

// rentalextensiondlg.h:22
const sItemInfo* S5::FrRentalExtensionDlg::GetItemInfo(void) {
	return m_itemInfo;
}

// frrandomboxprogress.h:37
void FrRandomBoxProgress::SetBoxTid(unsigned long tid) { m_boxTid = tid; }

// classdefine.h:478
bool IFF_STRUCT::sItem::IsRandomBox(void) const {
	return isRandomBox;
}

// globalnetworkdefine.h:2991
sStoredItemInfo& sStoredItemInfo::operator=(const sStoredItemInfo& rhs) {
	slot = rhs.slot;
	item = rhs.item;

	return *this;
}

// clientsetting.h:420
void COption::mSetPCBangMascotMsg(const char* msg) {
	strncpy(m_option.m_pcBangMascotMsg, msg, sizeof(m_option.m_pcBangMascotMsg));
}


// realmyroommainui.cpp:108
void ReviseItemIconPoint(unsigned long typeId, WPoint& pt) {
	switch (typeId >> 26) {
	case 1:
	case 2:
	case 8:
	case 9:
	case 15:
	case 16:
	case 28:
		pt.x += 7.0f;
		pt.y += 2.0f;
		break;
	case 11:
		pt.x += 9.0f;
		pt.y += 12.0f;
		break;
	case 14:
		if ((typeId & 0x3C00000) == 0x800000) {
			pt.x += 7.0f;
			pt.y += 12.0f;
		} else {
			pt.x += 16.0f;
			pt.y += 27.0f;
		}
		break;
	case 31:
		pt.x += 16.0f;
		pt.y += 12.0f;
		break;
	case 18:
		pt.x += 21.0f;
		pt.y += 12.0f;
		break;
	default:
		pt.x += 16.0f;
		pt.y += 27.0f;
		break;
	}
}

// realmyroommainui.cpp:153
template <class T>
T* FindFrForm(const char* name) {
	FrWnd* pWnd = g_pFresh->GetDesktop()->FindChildForm(name);
	if (pWnd != NULL) {
		return DYNAMIC_CAST(T, pWnd);
	}

	return NULL;
}

// The instantiation OnCardUseLBDown uses (not written yet).
template FrCardBoxDlg* FindFrForm<FrCardBoxDlg>(const char* name);

// realmyroommainui.cpp:171
const sFRESH_MSGMAP* CRealMyRoomMain::GetMessageMap(void) const {
	return &CRealMyRoomMain::_MsgMap;
}

sFRESH_MSGMAP CRealMyRoomMain::_MsgMap = { &CTaskMain::_MsgMap, &CRealMyRoomMain::_MsgEntries[0] };

// realmyroommainui.cpp:174
sFRESH_ENTRY CRealMyRoomMain::_MsgEntries[] = {
	{ "REALMYROOM", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnMyRoomInit },
	{ "REALMYROOM", FRCMD_FINISH, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnMyRoomFinish },
	{ "REALMYROOMVISITOR", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnMyRoomInit },
	{ "REALMYROOMVISITOR", FRCMD_FINISH, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnMyRoomFinish },
	{ "REALMYROOM.obar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_Init },
	{ "REALMYROOM.utab", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderTab_Init },
	{ "REALMYROOM.ubar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBarInit },
	{ "REALMYROOMVISITOR.obar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_Init },
	{ "REALMYROOMVISITOR.utab", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderTab_Init },
	{ "REALMYROOMVISITOR.ubar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBarInit },
	{ "REALMYROOM.middle_ui.item_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnItemListInit },
	{ "REALMYROOM.middle_ui.item_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnItemListOwnerDraw },
	{ "REALMYROOM.middle_ui.item_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnItemListBtnDown },
	{ "REALMYROOM.middle_ui.item_list", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnItemListRBtnDown },
	{ "REALMYROOM.middle_ui.item_list", FRCMD_MOUSEMOVE, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnItemListMouseMove },
	{ "REALMYROOM.middle_ui.item_list", FRCMD_DBLCLICK, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnItemListDClick },
	{ "REALMYROOM.middle_ui.tab_main", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTabMainInit },
	{ "REALMYROOM.middle_ui.tab_main", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnTabMainButtonUp },
	{ "REALMYROOM.middle_ui.tab_sub", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTabSubInit },
	{ "REALMYROOM.middle_ui.tab_sub", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnTabSubButtonUp },
	{ "REALMYROOM.middle_ui.gift_page", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnGiftPageOwnerDraw },
	{ "REALMYROOM.middle_ui.click_area_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnGiftPageLBDown },
	{ "REALMYROOM.middle_ui", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnMiddleUIInit },
	{ "REALMYROOMVISITOR.middle_ui", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnMiddleUIInit },
	{ "REALMYROOM.middle_ui.tabtxt1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTabTxt1Init },
	{ "REALMYROOM.middle_ui.tabtxt2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTabTxt2Init },
	{ "REALMYROOM.middle_ui.tabtxt2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTabTxt3Init },
	{ "REALMYROOM.middle_ui.st.selected_name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSelectedNameInit },
	{ "REALMYROOMVISITOR.middle_ui.tabtxt1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTabTxt1Init },
	{ "REALMYROOMVISITOR.middle_ui.tabtxt2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTabTxt2Init },
	{ "REALMYROOMVISITOR.middle_ui.tabtxt2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTabTxt3Init },
	{ "REALMYROOMVISITOR.middle_ui.st.selected_name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSelectedNameInit },
	{ "REALMYROOM.middle_ui.pet", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnPetInit },
	{ "REALMYROOM.middle_ui.pet", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnPetOwnerDraw },
	{ "REALMYROOM.middle_ui.skin_pet", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSkinPetInit },
	{ "REALMYROOM.middle_ui.skin_pet2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSkinPet2Init },
	{ "REALMYROOM.middle_ui.item_desc_bg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnItemDescBgInit },
	{ "REALMYROOM.middle_ui.item_desc_bg", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnItemDescBgOwnerDraw },
	{ "REALMYROOM.middle_ui.item_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnItemDescInit },
	{ "REALMYROOM.middle_ui.sbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnItemSlotBoxInit },
	{ "REALMYROOM.middle_ui.sbox.equip_cancel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnEquipCancelInit },
	{ "REALMYROOM.middle_ui.sbox.equip_cancel", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnEquipCancelUp },
	{ "REALMYROOM.middle_ui.sbox.equip_cancel", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTooltipHoverOn },
	{ "REALMYROOM.middle_ui.sbox.equip_cancel", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTooltipHoverOff },
	{ "REALMYROOM.middle_ui.sbox.equip_reset", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnEquipResetInit },
	{ "REALMYROOM.middle_ui.sbox.equip_reset", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnEquipResetUp },
	{ "REALMYROOM.middle_ui.sbox.equip_reset", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTooltipHoverOn },
	{ "REALMYROOM.middle_ui.sbox.equip_reset", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTooltipHoverOff },
	{ "REALMYROOM.middle_ui.sbox.item_slot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnItemSlotInit },
	{ "REALMYROOM.middle_ui.sbox.item_slot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnItemSlotOwnerDraw },
	{ "REALMYROOM.middle_ui.sbox.item_slot", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnItemSlotBtnDown },
	{ "REALMYROOM.middle_ui.sbox.item_slot", FRCMD_DBLCLICK, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnItemSlotDClick },
	{ "REALMYROOM.middle_ui.gift_bg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnGiftBgInit },
	{ "REALMYROOM.middle_ui.gift_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnGiftDescInit },
	{ "REALMYROOM.middle_ui.furniture_arrange", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureArrangeInit },
	{ "REALMYROOM.middle_ui.furniture_arrange", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnFurnitureArrangeButtonUP },
	{ "REALMYROOM.middle_ui.st", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnStatBoxInit },
	{ "REALMYROOM.middle_ui.st.selframe", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSelFrameInit },
	{ "REALMYROOM.middle_ui.st.selitemicon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSelItemIconInit },
	{ "REALMYROOM.middle_ui.st.selitem", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSelItemInit },
	{ "REALMYROOM.middle_ui.st.selitem", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnSelItemUp },
	{ "REALMYROOM.middle_ui.st.selitem", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSelItemOwnerDraw },
	{ "REALMYROOM.middle_ui.st.selitemicon", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSelItemIconOwnerDraw },
	{ "REALMYROOM.middle_ui.st.selitem_prev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSelItemPrevInit },
	{ "REALMYROOM.middle_ui.st.selitem_prev", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnSelItemPrevUp },
	{ "REALMYROOM.middle_ui.st.selitem_next", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSelItemNextInit },
	{ "REALMYROOM.middle_ui.st.selitem_next", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnSelItemNextUp },
	{ "REALMYROOM.middle_ui.st.stat_part", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnStatPartInit },
	{ "REALMYROOM.middle_ui.st.stat_part", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnStatPartUp },
	{ "REALMYROOM.middle_ui.st.stat_part", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnStatPartOwnerDraw },
	{ "REALMYROOM.middle_ui.st.stat_all", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnStatAllInit },
	{ "REALMYROOM.middle_ui.st.stat_all", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnStatAllUp },
	{ "REALMYROOM.middle_ui.st.stat_all", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnStatAllOwnerDraw },
	{ "REALMYROOM.middle_ui.st.stat_frame", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnStatFrameInit },
	{ "REALMYROOM.middle_ui.st.stat_text", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnStatTextInit },
	{ "REALMYROOM.middle_ui.st.powerbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnPowerBarInit },
	{ "REALMYROOM.middle_ui.st.controlbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnControlBarInit },
	{ "REALMYROOM.middle_ui.st.impactbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnImpactBarInit },
	{ "REALMYROOM.middle_ui.st.spinbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSpinBarInit },
	{ "REALMYROOM.middle_ui.st.curvebar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCurveBarInit },
	{ "REALMYROOM.middle_ui.st.epower", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnPowerEditInit },
	{ "REALMYROOM.middle_ui.st.econtrol", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnControlEditInit },
	{ "REALMYROOM.middle_ui.st.eimpact", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnImpactEditInit },
	{ "REALMYROOM.middle_ui.st.espin", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSpinEditInit },
	{ "REALMYROOM.middle_ui.st.ecurve", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCurveEditInit },
	{ "REALMYROOM.middle_ui.upgrade", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUpgradeInit },
	{ "REALMYROOM.middle_ui.upgrade", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUpgradeUp },
	{ "REALMYROOM.middle_ui.upgrade", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTooltipHoverOn },
	{ "REALMYROOM.middle_ui.upgrade", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTooltipHoverOff },
	{ "REALMYROOM.middle_ui.downgrade", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnDowngradeInit },
	{ "REALMYROOM.middle_ui.downgrade", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnDowngradeUp },
	{ "REALMYROOM.middle_ui.downgrade", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTooltipHoverOn },
	{ "REALMYROOM.middle_ui.downgrade", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTooltipHoverOff },
	{ "REALMYROOM.rmenu", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnRightMenuInit },
	{ "REALMYROOM.rmenu", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnRightMenuLBtnDown },
	{ "REALMYROOM.rmenu", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnRightMenuRBtnDown },
	{ "REALMYROOM.ubar.back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_BackInit },
	{ "REALMYROOM.ubar.back", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_BackUp },
	{ "REALMYROOM.ubar.back", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.ubar.back", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.ubar.front", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_FrontInit },
	{ "REALMYROOM.ubar.front", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_FrontUp },
	{ "REALMYROOM.ubar.front", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.ubar.front", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.ubar.shop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_ShopInit },
	{ "REALMYROOM.ubar.shop", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ShopUp },
	{ "REALMYROOM.ubar.shop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.ubar.shop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.ubar.myroom", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_MyRoomInit },
	{ "REALMYROOM.ubar.myroom", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MyRoomUp },
	{ "REALMYROOM.ubar.myroom", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.ubar.myroom", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.ubar.itemstorage", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_ItemStorageInit },
	{ "REALMYROOM.ubar.itemstorage", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_ItemStorageUp },
	{ "REALMYROOM.ubar.itemstorage", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.ubar.itemstorage", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.ubar.itemstorage", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_ItemStorageUp },
	{ "REALMYROOM.ubar.gift", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_GiftInit },
	{ "REALMYROOM.ubar.gift", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_GiftUp },
	{ "REALMYROOM.ubar.gift", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.ubar.gift", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.ubar.cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_CookieInit },
	{ "REALMYROOM.ubar.cookie", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_CookieUp },
	{ "REALMYROOM.ubar.cookie", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.ubar.cookie", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.ubar.game", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_GameInit },
	{ "REALMYROOM.ubar.game", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_GameUp },
	{ "REALMYROOM.ubar.game", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.ubar.game", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.ubar.server", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_ServerInit },
	{ "REALMYROOM.ubar.server", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_ServerUp },
	{ "REALMYROOM.ubar.server", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.ubar.server", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.ubar.exit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_ExitInit },
	{ "REALMYROOM.ubar.exit", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_ExitUp },
	{ "REALMYROOM.ubar.exit", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.ubar.exit", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.ubar.tabbtn_b", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_TabBtnInit },
	{ "REALMYROOM.ubar.tabbtn_b", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_TabBtnUp },
	{ "REALMYROOM.utab", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderTab_Init },
	{ "REALMYROOM.utab.magicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_MagicBoxInit },
	{ "REALMYROOM.utab.magicbox", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_MagicBoxUp },
	{ "REALMYROOM.utab.magicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.utab.magicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.utab.tikimagicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_TikiMagicBoxInit },
	{ "REALMYROOM.utab.tikimagicbox", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_TikiMagicBoxUp },
	{ "REALMYROOM.utab.tikimagicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.utab.tikimagicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.utab.scratch", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_ScratchInit },
	{ "REALMYROOM.utab.scratch", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_ScratchUp },
	{ "REALMYROOM.utab.scratch", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.utab.scratch", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.utab.bongdarishop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_BongdariShopInit },
	{ "REALMYROOM.utab.bongdarishop", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_BongdariShopUp },
	{ "REALMYROOM.utab.bongdarishop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.utab.bongdarishop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.utab.ranking", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_RankingInit },
	{ "REALMYROOM.utab.ranking", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_RankingUp },
	{ "REALMYROOM.utab.ranking", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.utab.ranking", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.utab.guild", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_GuildInit },
	{ "REALMYROOM.utab.guild", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GuildUp },
	{ "REALMYROOM.utab.guild", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.utab.guild", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.utab.notice", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_NoticeInit },
	{ "REALMYROOM.utab.notice", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_NoticeUp },
	{ "REALMYROOM.utab.notice", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.utab.notice", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.utab.maket", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_MaketInit },
	{ "REALMYROOM.utab.maket", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_MaketUp },
	{ "REALMYROOM.utab.maket", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.utab.maket", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.middle_ui.chatbg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnEffectViewInit },
	{ "REALMYROOM.middle_ui.chatbg", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnEffectViewOnwnerDraw },
	{ "REALMYROOM.obar.title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_TitleInit },
	{ "REALMYROOM.obar.site", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_SiteInit },
	{ "REALMYROOM.obar.char", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_CharInit },
	{ "REALMYROOM.obar.char", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_CharDown },
	{ "REALMYROOM.obar.char", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_CharOwnerDraw },
	{ "REALMYROOM.obar.caddie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_CaddieInit },
	{ "REALMYROOM.obar.caddie", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_CaddieDown },
	{ "REALMYROOM.obar.caddie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_CaddieOwnerDraw },
	{ "REALMYROOM.obar.club", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_ClubInit },
	{ "REALMYROOM.obar.club", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_ClubDown },
	{ "REALMYROOM.obar.club", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_ClubOwnerDraw },
	{ "REALMYROOM.obar.aztec", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_AztecInit },
	{ "REALMYROOM.obar.aztec", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_AztecDown },
	{ "REALMYROOM.obar.aztec", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_AztecOwnerDraw },
	{ "REALMYROOM.obar.mascot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_MascotInit },
	{ "REALMYROOM.obar.mascot", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_MascotDown },
	{ "REALMYROOM.obar.mascot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_MascotOwnerDraw },
	{ "REALMYROOM.obar.char_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_CharSelInit },
	{ "REALMYROOM.obar.char_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_CharSelOwnerDraw },
	{ "REALMYROOM.obar.caddie_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_CaddieSelInit },
	{ "REALMYROOM.obar.caddie_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_CaddieSelOwnerDraw },
	{ "REALMYROOM.obar.club_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_ClubSelInit },
	{ "REALMYROOM.obar.club_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_ClubSelOwnerDraw },
	{ "REALMYROOM.obar.aztec_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_AztecSelInit },
	{ "REALMYROOM.obar.aztec_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_AztecSelOwnerDraw },
	{ "REALMYROOM.obar.mascot_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_MascotSelInit },
	{ "REALMYROOM.obar.mascot_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_MascotSelOwnerDraw },
	{ "REALMYROOM.obar.pang_cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_PangCookieInit },
	{ "REALMYROOM.obar.pang_cookie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_PangCookieOwnerDraw },
	{ "REALMYROOM.obar.onelinemsg_req", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_OneLineReqBtnInit },
	{ "REALMYROOM.obar.onelinemsg_req", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_OneLineReqBtnUp },
	{ "REALMYROOM.obar.onelineview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_OneLineViewInit },
	{ "REALMYROOM.obar.onelineview", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_OneLineViewOwnerDraw },
	{ "REALMYROOM.obar.spcard", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_SpecialCardInit },
	{ "REALMYROOM.obar.spcard", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_SpecialCardUp },
	{ "REALMYROOM.obar.spcard", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.obar.spcard", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.obar.messenger", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_MessengerInit },
	{ "REALMYROOM.obar.messenger", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_MessengerUp },
	{ "REALMYROOM.obar.messenger", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.obar.messenger", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.obar.myinfo", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_MyInfoInit },
	{ "REALMYROOM.obar.myinfo", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_MyInfoUp },
	{ "REALMYROOM.obar.myinfo", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.obar.myinfo", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.obar.option", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_OptionInit },
	{ "REALMYROOM.obar.option", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_OptionUp },
	{ "REALMYROOM.obar.option", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOM.obar.option", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOM.obar.messenger_bubble", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnMessengerBubbleInit },
	{ "REALMYROOM.obar.messenger_bubble", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnMessengerBubbleOwnerDraw },
	{ "REALMYROOM.obar.messenger_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnMessengerBubbleEditInit },
	{ "REALMYROOM.obar.messenger_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnMessengerAlarmBtnInit },
	{ "REALMYROOM.obar.messenger_alarm", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnMessengerAlarmBtnUp },
	{ "REALMYROOM.obar.gift_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnMessengerGiftAlarmBtnInit },
	{ "REALMYROOM.obar.gift_alarm", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnMessengerGiftAlarmBtnUp },
	{ "REALMYROOM.obar.tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarTooltipOwnerDraw },
	{ "REALMYROOM.ftab_main", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureTabMainInit },
	{ "REALMYROOM.ftab_main", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnFurnitureTabMainButtonUp },
	{ "REALMYROOM.fitem_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureListInit },
	{ "REALMYROOM.fitem_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureListOwnerDraw },
	{ "REALMYROOM.fitem_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnFurnitureListBtnDown },
	{ "REALMYROOM.farrorup", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurniturePupInit },
	{ "REALMYROOM.farrorup", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnFurniturePupButtonUp },
	{ "REALMYROOM.farrordown", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurniturePdownInit },
	{ "REALMYROOM.farrordown", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnFurniturePdownButtonUp },
	{ "REALMYROOM.flist_num", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureListNumInit },
	{ "REALMYROOM.flist_num", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnFurnitureListNumBtnUp },
	{ "REALMYROOM.flist_num", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&CRealMyRoomMain::OnFurnitureListEnterKey },
	{ "REALMYROOM.flist_maxnum", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureListMaxNumInit },
	{ "REALMYROOM.Btn_flistnum", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurniture_BtnListNumInit },
	{ "REALMYROOM.Btn_flistnum", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnFurniture_BtnListNumBtnUp },
	{ "REALMYROOM.freset", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureResetInit },
	{ "REALMYROOM.freset", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnFurnitureResetButtonUp },
	{ "REALMYROOM.fback", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureBackInit },
	{ "REALMYROOM.fback", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnFurnitureBackButtonUp },
	{ "REALMYROOM.fsave", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureSaveInit },
	{ "REALMYROOM.fsave", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnFurnitureSaveButtonUp },
	{ "REALMYROOM.fout", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureOutInit },
	{ "REALMYROOM.fout", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnFurnitureOutButtonUp },
	{ "REALMYROOM.ftab_background01", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureBackGroundInit1 },
	{ "REALMYROOM.ftab_background02", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureBackGroundInit2 },
	{ "REALMYROOM.ftab_background03", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureBackGroundInit3 },
	{ "REALMYROOM.ftab_background04", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureBackGroundInit4 },
	{ "REALMYROOM.ftab_background05", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureBackGroundInit5 },
	{ "REALMYROOM.ftab_background06", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureBackGroundInit6 },
	{ "REALMYROOM.ftab_background07", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureBackGroundInit7 },
	{ "REALMYROOM.ftab_background08", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureBackGroundInit8 },
	{ "REALMYROOM.ftab_scrollimg01", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureScrollInit1 },
	{ "REALMYROOM.ftab_scrollimg02", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureScrollInit2 },
	{ "REALMYROOM.ftab_scrollimg03", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnFurnitureScrollInit3 },
	{ "REALMYROOM.Btn_Menu", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBtnMenuInit },
	{ "REALMYROOM.Btn_Menu", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OOnBtnMenuButtonUP },
	{ "REALMYROOM.Btn_MenuList", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBtn_MenuListInit },
	{ "REALMYROOM.Btn_MenuList", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBtn_MenuListOwnerDraw },
	{ "REALMYROOM.Btn_MenuList", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnBtn_MenuListBtnDown },
	{ "REALMYROOM.Btn_MenuList", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnBtn_MenuListRBtnDown },
	{ "REALMYROOM.Btn_MenuList", FRCMD_MOUSEMOVE, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnBtn_MenuListMouseMove },
	{ "REALMYROOM.Btn_MenuList", FRCMD_DBLCLICK, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnBtn_MenuListDClick },
	{ "REALMYROOM.item_hide_desc_bg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnItemDescBgInit2 },
	{ "REALMYROOM.item_hide_desc_bg", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnItemDescBgOwnerDraw2 },
	{ "REALMYROOM.item_hide_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnItemDescInit2 },
	{ "REALMYROOM.item_hide_name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnItemNameInit },
	{ "REALMYROOM.card_book", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardBookInit },
	{ "REALMYROOM.card_book", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardBookOwnerDraw },
	{ "REALMYROOM.cardpacklist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardPackListInit },
	{ "REALMYROOM.cardpacklist", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardPackListDraw },
	{ "REALMYROOM.cardpacklist", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardPackListDown },
	{ "REALMYROOM.cardpacklist", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardPackListRbUp },
	{ "REALMYROOM.cardpage_index", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardBookIndexInit },
	{ "REALMYROOM.cardpage_index", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardBookIndexLBDown },
	{ "REALMYROOM.cardpage_index", FRCMD_MOUSEMOVE, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardBookIndexMouseOver },
	{ "REALMYROOM.card_property", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardPropertyInit },
	{ "REALMYROOM.card_pack", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTabCardPackInit },
	{ "REALMYROOM.card_pack", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnTabCardPackLBDown },
	{ "REALMYROOM.card_use", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardUseInit },
	{ "REALMYROOM.card_use", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardUseLBDown },
	{ "REALMYROOM.card_volume1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTabCardVolume1Init },
	{ "REALMYROOM.card_volume1", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnTabCardVolume1LBDown },
	{ "REALMYROOM.card_volume2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTabCardVolume2Init },
	{ "REALMYROOM.card_volume2", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnTabCardVolume2LBDown },
	{ "REALMYROOM.card_volume3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTabCardVolume3Init },
	{ "REALMYROOM.card_volume3", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnTabCardVolume3LBDown },
	{ "REALMYROOM.card_volume4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTabCardVolume4Init },
	{ "REALMYROOM.card_volume4", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnTabCardVolume4LBDown },
	{ "REALMYROOM.cardpack_slot1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardPackInit1 },
	{ "REALMYROOM.cardpack_slot2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardPackInit2 },
	{ "REALMYROOM.cardpack_slot3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardPackInit3 },
	{ "REALMYROOM.cardpack_slot4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardPackInit4 },
	{ "REALMYROOM.cardpack_slot5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardPackInit5 },
	{ "REALMYROOM.cardpack_slot6", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardPackInit6 },
	{ "REALMYROOM.cardpack_slot7", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardPackInit7 },
	{ "REALMYROOM.cardpack_slot8", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardPackInit8 },
	{ "REALMYROOM.cardpack_slot1", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardPackLBDown1 },
	{ "REALMYROOM.cardpack_slot2", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardPackLBDown2 },
	{ "REALMYROOM.cardpack_slot3", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardPackLBDown3 },
	{ "REALMYROOM.cardpack_slot4", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardPackLBDown4 },
	{ "REALMYROOM.cardpack_slot5", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardPackLBDown5 },
	{ "REALMYROOM.cardpack_slot6", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardPackLBDown6 },
	{ "REALMYROOM.cardpack_slot7", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardPackLBDown7 },
	{ "REALMYROOM.cardpack_slot8", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardPackLBDown8 },
	{ "REALMYROOM.cardpack_slot1", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardPackRBDown1 },
	{ "REALMYROOM.cardpack_slot2", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardPackRBDown2 },
	{ "REALMYROOM.cardpack_slot3", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardPackRBDown3 },
	{ "REALMYROOM.cardpack_slot4", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardPackRBDown4 },
	{ "REALMYROOM.cardpack_slot5", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardPackRBDown5 },
	{ "REALMYROOM.cardpack_slot6", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardPackRBDown6 },
	{ "REALMYROOM.cardpack_slot7", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardPackRBDown7 },
	{ "REALMYROOM.cardpack_slot8", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardPackRBDown8 },
	{ "REALMYROOM.card_slot1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardInit1 },
	{ "REALMYROOM.card_slot2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardInit2 },
	{ "REALMYROOM.card_slot3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardInit3 },
	{ "REALMYROOM.card_slot4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardInit4 },
	{ "REALMYROOM.card_slot5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardInit5 },
	{ "REALMYROOM.card_slot6", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardInit6 },
	{ "REALMYROOM.card_slot7", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardInit7 },
	{ "REALMYROOM.card_slot8", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardInit8 },
	{ "REALMYROOM.card_slot9", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardInit9 },
	{ "REALMYROOM.card_slot10", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardInit10 },
	{ "REALMYROOM.card_slot11", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardInit11 },
	{ "REALMYROOM.card_slot12", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCardInit12 },
	{ "REALMYROOM.card_slot1", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardLBDown1 },
	{ "REALMYROOM.card_slot2", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardLBDown2 },
	{ "REALMYROOM.card_slot3", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardLBDown3 },
	{ "REALMYROOM.card_slot4", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardLBDown4 },
	{ "REALMYROOM.card_slot5", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardLBDown5 },
	{ "REALMYROOM.card_slot6", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardLBDown6 },
	{ "REALMYROOM.card_slot7", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardLBDown7 },
	{ "REALMYROOM.card_slot8", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardLBDown8 },
	{ "REALMYROOM.card_slot9", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardLBDown9 },
	{ "REALMYROOM.card_slot10", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardLBDown10 },
	{ "REALMYROOM.card_slot11", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardLBDown11 },
	{ "REALMYROOM.card_slot12", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardLBDown12 },
	{ "REALMYROOM.card_slot1", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardRBDown1 },
	{ "REALMYROOM.card_slot2", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardRBDown2 },
	{ "REALMYROOM.card_slot3", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardRBDown3 },
	{ "REALMYROOM.card_slot4", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardRBDown4 },
	{ "REALMYROOM.card_slot5", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardRBDown5 },
	{ "REALMYROOM.card_slot6", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardRBDown6 },
	{ "REALMYROOM.card_slot7", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardRBDown7 },
	{ "REALMYROOM.card_slot8", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardRBDown8 },
	{ "REALMYROOM.card_slot9", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardRBDown9 },
	{ "REALMYROOM.card_slot10", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardRBDown10 },
	{ "REALMYROOM.card_slot11", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardRBDown11 },
	{ "REALMYROOM.card_slot12", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnCardRBDown12 },
	{ "REALMYROOMVISITOR.obar.title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_TitleInit },
	{ "REALMYROOMVISITOR.obar.site", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_SiteInit },
	{ "REALMYROOMVISITOR.obar.char", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_CharInit },
	{ "REALMYROOMVISITOR.obar.char", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_CharDown },
	{ "REALMYROOMVISITOR.obar.char", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_CharOwnerDraw },
	{ "REALMYROOMVISITOR.obar.caddie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_CaddieInit },
	{ "REALMYROOMVISITOR.obar.caddie", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_CaddieDown },
	{ "REALMYROOMVISITOR.obar.caddie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_CaddieOwnerDraw },
	{ "REALMYROOMVISITOR.obar.club", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_ClubInit },
	{ "REALMYROOMVISITOR.obar.club", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_ClubDown },
	{ "REALMYROOMVISITOR.obar.club", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_ClubOwnerDraw },
	{ "REALMYROOMVISITOR.obar.aztec", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_AztecInit },
	{ "REALMYROOMVISITOR.obar.aztec", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_AztecDown },
	{ "REALMYROOMVISITOR.obar.aztec", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_AztecOwnerDraw },
	{ "REALMYROOMVISITOR.obar.mascot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_MascotInit },
	{ "REALMYROOMVISITOR.obar.mascot", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_MascotDown },
	{ "REALMYROOMVISITOR.obar.mascot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_MascotOwnerDraw },
	{ "REALMYROOMVISITOR.obar.char_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_CharSelInit },
	{ "REALMYROOMVISITOR.obar.char_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_CharSelOwnerDraw },
	{ "REALMYROOMVISITOR.obar.caddie_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_CaddieSelInit },
	{ "REALMYROOMVISITOR.obar.caddie_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_CaddieSelOwnerDraw },
	{ "REALMYROOMVISITOR.obar.club_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_ClubSelInit },
	{ "REALMYROOMVISITOR.obar.club_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_ClubSelOwnerDraw },
	{ "REALMYROOMVISITOR.obar.aztec_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_AztecSelInit },
	{ "REALMYROOMVISITOR.obar.aztec_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_AztecSelOwnerDraw },
	{ "REALMYROOMVISITOR.obar.mascot_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_MascotSelInit },
	{ "REALMYROOMVISITOR.obar.mascot_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_MascotSelOwnerDraw },
	{ "REALMYROOMVISITOR.obar.pang_cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_PangCookieInit },
	{ "REALMYROOMVISITOR.obar.pang_cookie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_PangCookieOwnerDraw },
	{ "REALMYROOMVISITOR.obar.onelinemsg_req", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_OneLineReqBtnInit },
	{ "REALMYROOMVISITOR.obar.onelinemsg_req", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_OneLineReqBtnUp },
	{ "REALMYROOMVISITOR.obar.onelineview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_OneLineViewInit },
	{ "REALMYROOMVISITOR.obar.onelineview", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_OneLineViewOwnerDraw },
	{ "REALMYROOMVISITOR.obar.spcard", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_SpecialCardInit },
	{ "REALMYROOMVISITOR.obar.spcard", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_SpecialCardUp },
	{ "REALMYROOMVISITOR.obar.spcard", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.obar.spcard", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.obar.messenger", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_MessengerInit },
	{ "REALMYROOMVISITOR.obar.messenger", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_MessengerUp },
	{ "REALMYROOMVISITOR.obar.messenger", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.obar.messenger", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.obar.myinfo", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_MyInfoInit },
	{ "REALMYROOMVISITOR.obar.myinfo", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_MyInfoUp },
	{ "REALMYROOMVISITOR.obar.myinfo", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.obar.myinfo", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.obar.option", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOverBar_OptionInit },
	{ "REALMYROOMVISITOR.obar.option", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnOverBar_OptionUp },
	{ "REALMYROOMVISITOR.obar.option", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.obar.option", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.obar.messenger_bubble", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnMessengerBubbleInit },
	{ "REALMYROOMVISITOR.obar.messenger_bubble", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnMessengerBubbleOwnerDraw },
	{ "REALMYROOMVISITOR.obar.messenger_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnMessengerBubbleEditInit },
	{ "REALMYROOMVISITOR.obar.messenger_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnMessengerAlarmBtnInit },
	{ "REALMYROOMVISITOR.obar.messenger_alarm", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnMessengerAlarmBtnUp },
	{ "REALMYROOMVISITOR.obar.gift_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnMessengerGiftAlarmBtnInit },
	{ "REALMYROOMVISITOR.obar.gift_alarm", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnMessengerGiftAlarmBtnUp },
	{ "REALMYROOMVISITOR.obar.tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarTooltipOwnerDraw },
	{ "REALMYROOMVISITOR.ubar.back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_BackInit },
	{ "REALMYROOMVISITOR.ubar.back", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_BackUp },
	{ "REALMYROOMVISITOR.ubar.back", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.ubar.back", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.ubar.front", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_FrontInit },
	{ "REALMYROOMVISITOR.ubar.front", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_FrontUp },
	{ "REALMYROOMVISITOR.ubar.front", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.ubar.front", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.ubar.shop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_ShopInit },
	{ "REALMYROOMVISITOR.ubar.shop", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ShopUp },
	{ "REALMYROOMVISITOR.ubar.shop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.ubar.shop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.ubar.myroom", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_MyRoomInit },
	{ "REALMYROOMVISITOR.ubar.myroom", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MyRoomUp },
	{ "REALMYROOMVISITOR.ubar.myroom", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.ubar.myroom", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.ubar.itemstorage", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_ItemStorageInit },
	{ "REALMYROOMVISITOR.ubar.itemstorage", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_ItemStorageUp },
	{ "REALMYROOMVISITOR.ubar.itemstorage", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.ubar.itemstorage", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.ubar.gift", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_GiftInit },
	{ "REALMYROOMVISITOR.ubar.gift", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_GiftUp },
	{ "REALMYROOMVISITOR.ubar.gift", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.ubar.gift", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.ubar.cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_CookieInit },
	{ "REALMYROOMVISITOR.ubar.cookie", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_CookieUp },
	{ "REALMYROOMVISITOR.ubar.cookie", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.ubar.cookie", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.ubar.game", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_GameInit },
	{ "REALMYROOMVISITOR.ubar.game", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_GameUp },
	{ "REALMYROOMVISITOR.ubar.game", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.ubar.game", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.ubar.server", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_ServerInit },
	{ "REALMYROOMVISITOR.ubar.server", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_ServerUp },
	{ "REALMYROOMVISITOR.ubar.server", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.ubar.server", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.ubar.exit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_ExitInit },
	{ "REALMYROOMVISITOR.ubar.exit", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_ExitUp },
	{ "REALMYROOMVISITOR.ubar.exit", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.ubar.exit", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.ubar.tabbtn_b", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_TabBtnInit },
	{ "REALMYROOMVISITOR.ubar.tabbtn_b", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_TabBtnUp },
	{ "REALMYROOMVISITOR.middle_ui.st", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnStatBoxInit },
	{ "REALMYROOMVISITOR.middle_ui.st.selframe", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSelFrameInit },
	{ "REALMYROOMVISITOR.middle_ui.st.selitemicon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSelItemIconInit },
	{ "REALMYROOMVISITOR.middle_ui.st.selitem", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSelItemInit },
	{ "REALMYROOMVISITOR.middle_ui.st.selitem", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnSelItemUp },
	{ "REALMYROOMVISITOR.middle_ui.st.selitem", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSelItemOwnerDraw },
	{ "REALMYROOMVISITOR.middle_ui.st.selitemicon", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSelItemIconOwnerDraw },
	{ "REALMYROOMVISITOR.middle_ui.st.selitem_prev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSelItemPrevInit },
	{ "REALMYROOMVISITOR.middle_ui.st.selitem_prev", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnSelItemPrevUp },
	{ "REALMYROOMVISITOR.middle_ui.st.selitem_next", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSelItemNextInit },
	{ "REALMYROOMVISITOR.middle_ui.st.selitem_next", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnSelItemNextUp },
	{ "REALMYROOMVISITOR.middle_ui.st.stat_part", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnStatPartInit },
	{ "REALMYROOMVISITOR.middle_ui.st.stat_part", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnStatPartUp },
	{ "REALMYROOMVISITOR.middle_ui.st.stat_part", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnStatPartOwnerDraw },
	{ "REALMYROOMVISITOR.middle_ui.st.stat_all", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnStatAllInit },
	{ "REALMYROOMVISITOR.middle_ui.st.stat_all", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnStatAllUp },
	{ "REALMYROOMVISITOR.middle_ui.st.stat_all", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnStatAllOwnerDraw },
	{ "REALMYROOMVISITOR.middle_ui.st.stat_frame", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnStatFrameInit },
	{ "REALMYROOMVISITOR.middle_ui.st.stat_text", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnStatTextInit },
	{ "REALMYROOMVISITOR.middle_ui.st.powerbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnPowerBarInit },
	{ "REALMYROOMVISITOR.middle_ui.st.controlbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnControlBarInit },
	{ "REALMYROOMVISITOR.middle_ui.st.impactbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnImpactBarInit },
	{ "REALMYROOMVISITOR.middle_ui.st.spinbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSpinBarInit },
	{ "REALMYROOMVISITOR.middle_ui.st.curvebar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCurveBarInit },
	{ "REALMYROOMVISITOR.middle_ui.st.epower", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnPowerEditInit },
	{ "REALMYROOMVISITOR.middle_ui.st.econtrol", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnControlEditInit },
	{ "REALMYROOMVISITOR.middle_ui.st.eimpact", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnImpactEditInit },
	{ "REALMYROOMVISITOR.middle_ui.st.espin", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnSpinEditInit },
	{ "REALMYROOMVISITOR.middle_ui.st.ecurve", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnCurveEditInit },
	{ "REALMYROOMVISITOR.middle_ui.upgrade", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUpgradeInit },
	{ "REALMYROOMVISITOR.middle_ui.upgrade", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUpgradeUp },
	{ "REALMYROOMVISITOR.middle_ui.upgrade", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTooltipHoverOn },
	{ "REALMYROOMVISITOR.middle_ui.upgrade", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTooltipHoverOff },
	{ "REALMYROOMVISITOR.middle_ui.downgrade", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnDowngradeInit },
	{ "REALMYROOMVISITOR.middle_ui.downgrade", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnDowngradeUp },
	{ "REALMYROOMVISITOR.middle_ui.downgrade", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTooltipHoverOn },
	{ "REALMYROOMVISITOR.middle_ui.downgrade", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnTooltipHoverOff },
	{ "REALMYROOMVISITOR.utab", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderTab_Init },
	{ "REALMYROOMVISITOR.utab.magicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_MagicBoxInit },
	{ "REALMYROOMVISITOR.utab.magicbox", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_MagicBoxUp },
	{ "REALMYROOMVISITOR.utab.magicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.utab.magicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.utab.tikimagicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_TikiMagicBoxInit },
	{ "REALMYROOMVISITOR.utab.tikimagicbox", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_TikiMagicBoxUp },
	{ "REALMYROOMVISITOR.utab.tikimagicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.utab.tikimagicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.utab.scratch", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_ScratchInit },
	{ "REALMYROOMVISITOR.utab.scratch", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_ScratchUp },
	{ "REALMYROOMVISITOR.utab.scratch", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.utab.scratch", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.utab.bongdarishop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_BongdariShopInit },
	{ "REALMYROOMVISITOR.utab.bongdarishop", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_BongdariShopUp },
	{ "REALMYROOMVISITOR.utab.bongdarishop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.utab.bongdarishop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.utab.maket", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_MaketInit },
	{ "REALMYROOMVISITOR.utab.maket", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_MaketUp },
	{ "REALMYROOMVISITOR.utab.maket", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.utab.maket", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.utab.ranking", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_RankingInit },
	{ "REALMYROOMVISITOR.utab.ranking", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_RankingUp },
	{ "REALMYROOMVISITOR.utab.ranking", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.utab.ranking", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.utab.guild", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_GuildInit },
	{ "REALMYROOMVISITOR.utab.guild", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GuildUp },
	{ "REALMYROOMVISITOR.utab.guild", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.utab.guild", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.utab.notice", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnUnderBar_NoticeInit },
	{ "REALMYROOMVISITOR.utab.notice", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnUnderBar_NoticeUp },
	{ "REALMYROOMVISITOR.utab.notice", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOn },
	{ "REALMYROOMVISITOR.utab.notice", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnBarHoverOff },
	{ "REALMYROOMVISITOR.rmenu", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnRightMenuInit },
	{ "REALMYROOMVISITOR.rmenu", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnRightMenuLBtnDown },
	{ "REALMYROOMVISITOR.rmenu", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnRightMenuRBtnDown },
	{ "REALMYROOM.item_storage_bgi", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnInitItemStorageBGI },
	{ "REALMYROOM.item_storage_bgi", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOwnerDrawItemStorageBGI },
	{ "REALMYROOM.item_storage_pang", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnInitItemStoragePang },
	{ "REALMYROOM.item_storage_pang", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOwnerDrawItemStoragePang },
	{ "REALMYROOM.pang_minus", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnInitItemStoragePangMinus },
	{ "REALMYROOM.pang_minus", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnLBDownItemStoragePangMinus },
	{ "REALMYROOM.pang_plus", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnInitItemStoragePangPlus },
	{ "REALMYROOM.pang_plus", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnLBDownItemStoragePangPlus },
	{ "REALMYROOM.item_storage_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnInitItemStorageList },
	{ "REALMYROOM.item_storage_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOwnerDrawItemStorageList },
	{ "REALMYROOM.item_storage_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnLBDownItemStorageList },
	{ "REALMYROOM.item_storage_list", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnRBDownItemStorageList },
	{ "REALMYROOM.item_storage_list", FRCMD_DBLCLICK, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnDBClickItemStorageList },
	{ "REALMYROOM.item_storage_list", FRCMD_MOUSEMOVE, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnMouseMoveItemStorageList },
	{ "REALMYROOM.item_storage_index", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnInitItemStorageIndexMsg },
	{ "REALMYROOM.item_storage_index", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOwnerDrawItemStorageIndexMsg },
	{ "REALMYROOM.item_storage_index", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnLBDownItemStorageIndexMsg },
	{ "REALMYROOM.item_storage_page_next", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnInitItemStoragePageNext },
	{ "REALMYROOM.item_storage_page_next", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnLBDownItemStoragePageNext },
	{ "REALMYROOM.item_storage_page_prev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnInitItemStoragePagePrev },
	{ "REALMYROOM.item_storage_page_prev", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnLBDownItemStoragePagePrev },
	{ "REALMYROOM.item_storage_close", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnInitItemStorageClose },
	{ "REALMYROOM.item_storage_close", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CRealMyRoomMain::OnLBDownItemStorageClose },
	{ "REALMYROOM.item_storage_dragdrop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnInitItemStorageDragDrop },
	{ "REALMYROOM.item_storage_dragdrop", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CRealMyRoomMain::OnOwnerDrawItemStorageDragDrop },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// realmyroommainui.cpp:976
void SetMainTabImg(const char* imgs[], int index) {
	imgs[0] = MakeStr("stab_%d_n", index);
	imgs[1] = MakeStr("stab_%d_d", index);
	imgs[2] = MakeStr("stab_%d_d", index);
}

// realmyroommainui.cpp:983
void SetSubTabImg(const char* imgs[], int index) {
	imgs[0] = MakeStr("stext_%d_n", index);
	imgs[1] = MakeStr("stext_%d_n", index);
	imgs[2] = MakeStr("stext_%d_n", index);
}

// realmyroommainui.cpp:991
bool EmptyCompares(const void* p1, const void* p2) {
	if (p1 != NULL) {
		if (p2 == NULL) {
			return true;
		}
	}

	return false;
}

// realmyroommainui.cpp:1258
void CRealMyRoomMain::OnAnotherHand(int nWork, void* pData) {
	if (nWork == 0) {
		ShowPet(*(unsigned long*)pData);
	}
}

// realmyroommainui.cpp:1265
void CRealMyRoomMain::OpenLayout(const char* layout) {
	WSendPacket packet(GCT_REQUEST_SERVER_TIME);
	packet.Send(SEND_TO_DEFAULT);

	g_pFresh->OpenLayout(layout, this, false);
}

// realmyroommainui.cpp:1273
void CRealMyRoomMain::CloseLayout(void) {
	if (m_pLayoutWnd) {
		m_pLayoutWnd->Close(true);
	}

	g_pFresh->CloseLayout();
}

// realmyroommainui.cpp:1281
void CRealMyRoomMain::ShowPet(unsigned long typeId) {
	if (typeId) {
		m_petLoader.Enter();

		if (m_pPetArea == NULL) {
			m_petLoader.Leave();
			return;
		}

		if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = 0; }

		WRect rect = m_pPetArea->GetRect();

		CExhibition* pet = NULL;
		bool loaded;
		switch (typeId >> 26) {
		case 1:
			if (m_roomMode == 13 || m_roomMode == 14 || m_roomMode == 15) {
				pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.35f, true);
				pet->SetArea(rect.w, rect.h);

				pet->SetDistanceRange(16.0f, 20.0f); pet->SetDistance(18.0f, true, false);
			} else {
				pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f + 15.0f, 0.35f, true);
				pet->SetArea(rect.w, rect.h);

				pet->SetDistanceRange(8.0f, 12.0f); pet->SetDistance(10.0f, true, false);
			}

			// While a UCC is being drawn, show it on the character.
			if (IsLocalContent((localContentType_t)0x57) && m_pUccDrawDlg && m_pUccDrawDlg->m_pItemInfo) {
				char uccKey[9];
				char partTextures[24][9];

				unsigned long uccTypeId = m_pUccDrawDlg->m_pItemInfo->m_typeId;
				strncpy(uccKey, m_pUccDrawDlg->m_pItemInfo->m_uccKey, 9);

				sItemInfo* info = GetMyItemInfo(uccTypeId, uccKey, 0);

				pet->SetAutoRecoverCamera(false); strcpy(partTextures[(uccTypeId >> 13) & 0x1F], uccKey);
				CPartTidList parts =
					WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();
				parts.SetTid(uccTypeId, info ? info->m_serial : 0, uccKey, info ? info->m_uccSeq : 0);
				loaded = pet->SetModel(parts, WSingleton<CSharedDoc>::Instance()->GetMyAuxPartTidList(), NULL, 3.0f, 0);
			} else {
				CSharedDoc* doc = WSingleton<CSharedDoc>::Instance(); loaded = pet->SetModel(doc->GetMyPartTidList(), doc->GetMyAuxPartTidList(), NULL, 3.0f, 0);
			}
			break;

		case 7:
		case 8:
			pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f + 15.0f, 0.35f, true);
			pet->SetArea(rect.w, rect.h);

			pet->SetDistanceRange(9.0f, 13.0f); pet->SetDistance(11.0f, true, false);
			loaded = pet->SetModel(typeId);
			break;

		case 16:
			pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f + 15.0f, 0.35f, true);
			pet->SetArea(rect.w, rect.h);
			pet->SetDistanceRange(4.0f, 8.0f);
			pet->SetDistance(6.0f, true, false);
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

		case 11:
			pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.0f, true);
			pet->SetArea(rect.w, rect.h);
			pet->SetDistance(18.0f, true, false);
			loaded = pet->SetModel(typeId);
			break;

		case 6:
			pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.0f, true);
			pet->SetArea(rect.w, rect.h);
			pet->SetDistance(6.0f, true, false);

			pet->SetDistanceRange(5.0f, 7.0f); loaded = pet->SetModel(typeId);
			break;

		case 18:
			if (IsLocalContent((localContentType_t)0x56)) {
				pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.0f, true);
				pet->SetArea(rect.w * 0.5f, rect.h * 0.5f);
				pet->SetDistance(15.0f, true, false);
				loaded = pet->SetModel(typeId);
			}
			break;

		case 31:
			pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.0f, true);
			pet->SetArea(rect.w, rect.h);

			pet->SetDistanceRange(5.0f, 7.0f);
			if ((typeId & 0x3C00000) == 0xC00000) {
				pet->SetDistance(5.0f, true, false);
			} else {
				pet->SetDistance(6.0f, true, false);
			}

			loaded = pet->SetModel(typeId);
			break;

		default:
			m_petLoader.Leave();
			return;
		}

		if (!loaded) {
			delete pet;
		} else {
			pet->SetControlFlag(15);
			m_pPetExhibition = pet;
		}

		m_petLoader.Leave();
	}
}

// realmyroommainui.cpp:1443
void CRealMyRoomMain::HidePet(bool bWait) {
	if (bWait) {
		m_petLoader.WaitUntilWorkDone(9999);
	}

	if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = 0; }
}

// realmyroommainui.cpp:1456
void CRealMyRoomMain::OnMyRoomInit(PG_MSGARG param) {
	m_initList.clear();

	SetConstantBGM("myroom.mp3", false, false);
	TurnOff3DBackground();
}

// realmyroommainui.cpp:1464
void CRealMyRoomMain::OnMyRoomFinish(void) {
	m_titleArea->SetBgImg("title_myroom");

	m_pPetArea = m_pPetAreaWnd[2];

	Reinitialize(false);

	if (WSingleton<CSharedDoc>::Instance()->m_tutorialFinish) {
		this << MsgObject(0, 0xAA, 1, 0, 0, 0, 0);
	}
}

// realmyroommainui.cpp:1478
void CRealMyRoomMain::Reinitialize(bool bForce) {
	if (MyUID() == WSingleton<CSharedDoc>::Instance()->m_myRoomOwnerUID) {
		ChangeCharacter(WSingleton<CSharedDoc>::Instance()->m_myCharacterId);
		ChangeClubset(WSingleton<CSharedDoc>::Instance()->m_myClubId);
		ChangeAztec(WSingleton<CSharedDoc>::Instance()->m_myBallId);

		if (bForce == false) {
			ChangeCaddie(WSingleton<CSharedDoc>::Instance()->m_myCaddieId);
			ChangeMascot(WSingleton<CSharedDoc>::Instance()->m_myMascotId);
		}

		ChangeMainTab(m_mainTab, true);
		ChangeSubTab((subTab_t)m_roomMode, true);
		m_statTab = (statTab_t)(m_mainTab > 2);
		SetLevelBar();
		m_selTab = (selTab_t)0;

		memcpy(m_displaySkin, WSingleton<CSharedDoc>::Instance()->m_equipSkin, sizeof(m_displaySkin));

		for (int i = 0; i < 6; i++) {
			if (m_displaySkin[i] != 0) {
				IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(m_displaySkin[i]);
				if (pSkin != 0 && (m_displaySkin[i] & 0x3C00000) == 0) {
					m_skinParam[0] = *((unsigned char*)pSkin + 0xB8);
					m_skinParam[1] = *((unsigned char*)pSkin + 0xB9);
					m_skinSlot[0] = 0;
					m_skinSlot[1] = 0;
				}
			}
		}
	}
}

// realmyroommainui.cpp:1517
void CRealMyRoomMain::ReinitializePC(ePCBangItem item) {
	if (item == PCBANG_ITEM_CADDIE)
		ChangeCaddie(CSharedDoc::Instance()->m_realEquip[0]);
	else if (item == PCBANG_ITEM_MASCOT)
		ChangeMascot(CSharedDoc::Instance()->m_realEquip[0x1A]);
}

// realmyroommainui.cpp:1526
void CRealMyRoomMain::InitMyRoomDialogs(void) {
	m_pMenuDlg = NULL;
	m_optionDlg = NULL;
	m_kbdManualDlg = NULL;
	m_serverDlg = NULL;
	m_giftDlg = NULL;
	m_waitDlg = NULL;
	m_notifyOneDlg = NULL;
	m_waitDlgTime = -10.0f;
	m_pEnchantDlg = NULL;
	m_pDowngradeDlg = NULL;
	m_pUpgradeButton = NULL;
	m_pUpgradeDlg = NULL;
	m_pPangGiftDlg = NULL;
	m_pMascotInfoDlg = NULL;
	m_pNoteDlg = NULL;
	m_pFortuneDlg = NULL;
	m_pGiftOpenDlg = NULL;
	m_pReplayDelDlg = NULL;
	m_pPostDlg = NULL;
	m_pPostSendDlg = NULL;
	m_pAuthorityDlg = NULL;
	if (IsLocalContent((localContentType_t)0x76)) {
		m_pFurnitureAbilityDlg = NULL;
	}

	m_dialogsReady = true;
	m_pUccDrawDlg = NULL;
	m_pUccCopyDlg = NULL;
	m_pReportDlg = NULL;
	m_pCardUseDlg = NULL;
	m_pCardRemoveDlg = NULL;
	m_pItemStorageNotice = NULL;
	m_pItemStorageAccessPanel = NULL;
	m_itemStorageVisible = false;
	m_itemStorageDlgPos = WPoint(0, 0);
	m_pItemStorageDlg = NULL;
	m_pItemStoragePang = NULL;
	m_pItemStoragePangMinus = NULL;
	m_pItemStoragePangPlus = NULL;
	m_pItemStorageList = NULL;
	m_pItemStorageIndexMsg = NULL;
	m_pItemStoragePageNext = NULL;
	m_pItemStoragePagePrev = NULL;
	m_pItemStorageClose = NULL;
	m_pItemStorageDragDrop = NULL;
	SetItemStorageVisible(false);
}

// realmyroommainui.cpp:1589
void CRealMyRoomMain::InitMyRoomControls(void) {
	m_pMiddleUIWnd = NULL;
	m_pSelectedName = NULL;
	m_pItemStatusWnd = NULL;
	m_pStatAllViewer = NULL;
	m_pMainTab = NULL;
	m_pSubTab = NULL;
	m_pSubTabWndA = NULL;
	m_pSubTabWndB = NULL;
	m_pUccCopyHideWnd = NULL;
	m_pTabTxt[0] = NULL;
	m_pTabTxt[1] = NULL;
	m_pTabTxt[2] = NULL;
	m_pPetArea = NULL;
	m_pPetExhibition = NULL;
	m_pItemSelectWnd = NULL;
	memset(&m_pPowerBar, 0, sizeof(FrGaugeBar*) * 5);
	memset(&m_pPowerEdit, 0, sizeof(FrStatic*) * 5);
	m_pSelItemPrevButton = NULL;
	m_pSelItemNextButton = NULL;
	memset(m_displaySkin, 0, sizeof(m_displaySkin));
	memset(m_skinParam, 0, sizeof(m_skinParam));
	memset(m_skinSlot, 0, sizeof(m_skinSlot));
	m_pPetAreaWnd[2] = NULL;
	m_pPetAreaWnd[0] = NULL;
	m_pItemSlotList = NULL;
	m_pEquipCancelButton = NULL;
	m_pEquipResetButton = NULL;
	m_pItemSlotBox = NULL;
	m_controlFlag = false;
	m_pCaddieInfoWnd = NULL;
	m_pSelFrameWnd = NULL;
	m_pSelItemWnd = NULL;
	m_pStatWnd = NULL;
	m_pSelFrame = NULL;
	m_pListEmptyBitmap = NULL;
	m_pSelectDivide = NULL;
	m_pSkinNameBitmap = NULL;
	m_pSkinSlotBitmap = NULL;
	m_pDowngradeButton = NULL;

	m_pItemDescBg = NULL;
	m_pItemDescEdit = NULL;
	m_pItemDescBg2 = NULL;
	m_pItemDescEdit2 = NULL;
	m_pGiftBgViewer = NULL;
	m_pGiftDescEdit = NULL;

	m_pFurnitureArrangeButton = NULL;

	m_pEffectViewWnd = NULL;
	m_pEffectViewWnd2 = NULL;

	m_pPetAreaWnd[1] = NULL;
	m_pStatFrame = NULL;
	m_pStatText = NULL;


	m_pFurnitureWndA = NULL;
	m_pFurnitureWndB = NULL;
	m_pFurnitureWndC = NULL;
	m_pFurnitureWndD = NULL;
	m_pFurnitureWndE = NULL;

	m_pFurnitureWndF = NULL;

	m_pFurnitureLeftWnd[0] = NULL;
	m_pFurnitureLeftWnd[1] = NULL;
	m_pFurnitureLeftWnd[2] = NULL;
	m_pFurnitureLeftWnd[3] = NULL;
	m_pFurnitureLeftWnd[4] = NULL;


	m_pRightMenuWnd = NULL;



	m_pFurnitureMainTab = NULL;
	m_pFurnitureList = NULL;

	m_uiState = 0;
	m_prevUiState = 0;
	memset(m_flags, 1, sizeof(m_flags));
	m_flags[3] = false;


	m_pFurnitureResetButton = NULL;
	m_pFurnitureBackButton = NULL;
	m_pFurnitureSaveButton = NULL;
	m_pFurnitureOutButton = NULL;


	for (int i = 0; i < 8; ++i) m_pFurnitureBackGround[i] = NULL;


	for (int j = 0; j < 3; ++j) m_pFurnitureScroll[j] = NULL;



	m_pFurnitureListNumWnd = NULL;
	m_pFurniturePageCountWnd = NULL;
	m_pFurnitureListNumButton = NULL;
	m_pFurniturePageUpButton = NULL;
	m_pFurniturePageDownButton = NULL;
	m_pMenuList = NULL;



	m_pFurnitureCtrl[0] = NULL;
	m_pFurnitureCtrl[1] = NULL;
	m_pFurnitureCtrl[2] = NULL;
	m_pFurnitureCtrl[3] = NULL;
	m_pFurnitureCtrl[4] = NULL;
	m_pFurnitureCtrl[5] = NULL;


	m_uiFlagA = false;
	m_bHideMiddleUI = true;
	m_uiFlagB = false;


	m_offlineShopEditState = 0;

}

// realmyroommainui.cpp:1718
void CRealMyRoomMain::InitMyRoomVariables(void) {
	m_selItemOn = false;
	m_caddieTypeId = 0;
	m_caddieValueA = 0;
	m_caddieValueB = 0;
	m_mascotTypeId = 0;
	m_mascotSerial = 0;
	m_charTypeId = 0;
	m_charSerial = 0;
	m_clubSerial = 0;
	m_aztecTypeId = 0;
	m_pCharLevels = NULL;
	m_pCharParts = NULL;
	if (IsLocalContent((localContentType_t)0x57))
		memset(m_localContentData, 0, sizeof(m_localContentData));

	m_pCharCards = NULL;
	m_statTab = (statTab_t)0;
	m_mainTab = (mainTab_t)0;
	m_roomMode = 0x15;
	m_pClubLevels = NULL;
	m_selTab = (selTab_t)0;
	m_statSource = 0;

	m_giftPageCount = 0;
	m_giftPageShownCount = 0;
	m_giftPageCurrent = CSharedDoc::Instance()->m_value5050;
	m_giftPageCurrent = CSharedDoc::Instance()->m_value5055;
	m_giftPageGroup = 0;

	for (int i = 0; i < 7; i++) {
		m_areaRects[i] = WRect(0, 0, 0, 0);
	}

	m_pEffectViewWnd = NULL;
	m_pEffectViewWnd2 = NULL;
	m_pPetAreaWnd[1] = NULL;
	m_pStatFrame = NULL;
	m_pStatText = NULL;
	m_pFurnitureWndA = NULL;
	m_pFurnitureWndB = NULL;
	m_pFurnitureWndC = NULL;
	m_pFurnitureWndD = NULL;
	m_pFurnitureWndE = NULL;
	m_pFurnitureWndF = NULL;
	m_pFurnitureLeftWnd[0] = NULL;
	m_pFurnitureLeftWnd[1] = NULL;
	m_pFurnitureLeftWnd[2] = NULL;
	m_pFurnitureLeftWnd[3] = NULL;
	m_pFurnitureLeftWnd[4] = NULL;
	m_areaFlagC = false;
	m_areaFlagA = false;

	m_furnitureMainTab = (eFurniture_Tab)0;
	m_furniturePage = 0;

	m_pShopMarkBitmap = NULL;
	m_areaFlagB = true;
	m_furniturePageCount = 1;
}

// realmyroommainui.cpp:1791
void CRealMyRoomMain::LoadMyRoomImages(void) {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 2; j++) {
			m_pStatPointBitmap[i][j] = g_pFresh->RegisterBitmap(dot_files[i][j]);
		}
	}

	m_pListBgBitmap[0] = g_pFresh->RegisterBitmap("select_up");
	m_pListBgBitmap[1] = g_pFresh->RegisterBitmap("select_mid");
	m_pListBgBitmap[2] = g_pFresh->RegisterBitmap("select_low");
	m_pListEmptyBitmap = g_pFresh->RegisterBitmap("select_empty");

	for (int k = 0; k < 5; k++) {
		m_selectIcons[k].normal = g_pFresh->RegisterBitmap(MakeStr("select_%d_n", k + 1));
		m_selectIcons[k].over = g_pFresh->RegisterBitmap(MakeStr("select_%d_o", k + 1));
	}
	m_pSelectDivide = g_pFresh->RegisterBitmap(MakeStr("select_divide"));

	m_pListSelBitmap = g_pFresh->RegisterBitmap("select_cursor1");
	m_pListOverBitmap = g_pFresh->RegisterBitmap("select_cursor0");

	m_pStatPartBitmap[0][0] = g_pFresh->RegisterBitmap("stat_char_n");
	m_pStatPartBitmap[0][1] = g_pFresh->RegisterBitmap("stat_char_o");
	m_pStatPartBitmap[0][2] = g_pFresh->RegisterBitmap("stat_char_d");
	m_pStatPartBitmap[1][0] = g_pFresh->RegisterBitmap("stat_club_n");
	m_pStatPartBitmap[1][1] = g_pFresh->RegisterBitmap("stat_club_o");
	m_pStatPartBitmap[1][2] = g_pFresh->RegisterBitmap("stat_club_d");
	m_pStatPartBitmap[2][0] = g_pFresh->RegisterBitmap("stat_caddie_n");
	m_pStatPartBitmap[2][1] = g_pFresh->RegisterBitmap("stat_caddie_o");
	m_pStatPartBitmap[2][2] = g_pFresh->RegisterBitmap("stat_caddie_d");

	m_pStatAllBitmap[0] = g_pFresh->RegisterBitmap("stat_all_n");
	m_pStatAllBitmap[1] = g_pFresh->RegisterBitmap("stat_all_o");
	m_pStatAllBitmap[2] = g_pFresh->RegisterBitmap("stat_all_d");

	m_cardPackItemBmp[0] = g_pFresh->RegisterBitmap("item_base_n");
	m_cardPackItemBmp[1] = g_pFresh->RegisterBitmap("item_base_o");
	m_cardPackItemBmp[2] = g_pFresh->RegisterBitmap("item_base_d");
	m_cardPackItemBmp[3] = g_pFresh->RegisterBitmap("item_base_empty");

	m_itemListArea[0].normal = g_pFresh->RegisterBitmap("giftmove_btn_n");
	m_itemListArea[0].over = g_pFresh->RegisterBitmap("giftmove_btn_o");
	m_itemListArea[0].rect = WRect(103.0f, 66.0f, m_itemListArea[0].normal->Width(), m_itemListArea[0].normal->Height());

	m_itemListArea[4].normal = g_pFresh->RegisterBitmap("answer_btn_n");
	m_itemListArea[4].over = g_pFresh->RegisterBitmap("answer_btn_o");
	m_itemListArea[4].rect = WRect(144.0f, 66.0f, m_itemListArea[4].normal->Width(), m_itemListArea[4].normal->Height());

	m_itemListArea[1].normal = g_pFresh->RegisterBitmap("equip_btn_n");
	m_itemListArea[1].over = g_pFresh->RegisterBitmap("equip_btn_o");
	m_itemListArea[1].rect = WRect(103.0f, 66.0f, m_itemListArea[1].normal->Width(), m_itemListArea[1].normal->Height());

	m_itemListArea[2].normal = g_pFresh->RegisterBitmap("delete_btn_n");
	m_itemListArea[2].over = g_pFresh->RegisterBitmap("delete_btn_o");
	m_itemListArea[2].rect = WRect(144.0f, 66.0f, m_itemListArea[2].normal->Width(), m_itemListArea[2].normal->Height());

	m_itemListArea[3].normal = g_pFresh->RegisterBitmap("reemploy_btn_n");
	m_itemListArea[3].over = g_pFresh->RegisterBitmap("reemploy_btn_o");
	m_itemListArea[3].rect = WRect(144.0f, 66.0f, m_itemListArea[3].normal->Width(), m_itemListArea[3].normal->Height());

	m_itemListArea[5].normal = g_pFresh->RegisterBitmap("clock");
	m_itemListArea[5].over = g_pFresh->RegisterBitmap("clock");
	m_itemListArea[5].rect = WRect(32.0f, 30.0f, m_itemListArea[5].normal->Width(), m_itemListArea[5].normal->Height());

	if (IsLocalContent((localContentType_t)0x4F)) {
		m_itemListArea[7].normal = g_pFresh->RegisterBitmap("replaymode_btn_n");
		m_itemListArea[7].over = g_pFresh->RegisterBitmap("replaymode_btn_o");
		m_itemListArea[7].rect = WRect(144.0f, 66.0f, m_itemListArea[7].normal->Width(), m_itemListArea[7].normal->Height());

		m_itemListArea[8].normal = g_pFresh->RegisterBitmap("replaydel_btn_n");
		m_itemListArea[8].over = g_pFresh->RegisterBitmap("replaydel_btn_o");
		m_itemListArea[8].rect = WRect(103.0f, 66.0f, m_itemListArea[8].normal->Width(), m_itemListArea[8].normal->Height());
	}

	if (IsLocalContent((localContentType_t)0x51)) {
		m_itemListArea[9].normal = g_pFresh->RegisterBitmap("card_btn_n");
		m_itemListArea[9].over = g_pFresh->RegisterBitmap("card_btn_o");
		m_itemListArea[9].rect = WRect(144.0f, 66.0f, m_itemListArea[9].normal->Width(), m_itemListArea[9].normal->Height());
	}

	if (IsLocalContent((localContentType_t)0x57)) {
		m_itemListArea[11].normal = g_pFresh->RegisterBitmap("ucc_draw_n");
		m_itemListArea[11].over = g_pFresh->RegisterBitmap("ucc_draw_o");

		m_itemListArea[13].normal = g_pFresh->RegisterBitmap("ucc_redraw_btn_n");
		m_itemListArea[13].over = g_pFresh->RegisterBitmap("ucc_redraw_btn_o");

		m_itemListArea[12].normal = g_pFresh->RegisterBitmap("ucc_copy_n");
		m_itemListArea[12].over = g_pFresh->RegisterBitmap("ucc_copy_o");

		m_uccDrawRects[0] = WRect(103.0f, 66.0f, m_itemListArea[11].normal->Width(), m_itemListArea[11].normal->Height());
		m_uccDrawRects[1] = WRect(144.0f, 66.0f, m_itemListArea[11].normal->Width(), m_itemListArea[11].normal->Height());

		m_pUccStateBitmap[0] = g_pFresh->RegisterBitmap("ucc_state_original");
		m_pUccStateBitmap[1] = g_pFresh->RegisterBitmap("ucc_state_copied");
		m_pUccStateBitmap[2] = g_pFresh->RegisterBitmap("ucc_state_temp_saved");
	}

	if (IsLocalContent((localContentType_t)0x56)) {
		m_itemListArea[10].normal = g_pFresh->RegisterBitmap("rmr_arrange_btn_n");
		m_itemListArea[10].over = g_pFresh->RegisterBitmap("rmr_arrange_btn_o");

		m_itemListArea[10].rect = WRect(144.0f, 66.0f, m_itemListArea[10].normal->Width(), m_itemListArea[10].normal->Height());
	}

	m_pSkinLevelBitmap[0] = g_pFresh->RegisterBitmap("i_male");
	m_pSkinLevelBitmap[1] = g_pFresh->RegisterBitmap("i_female");
	m_pSkinNameBitmap = g_pFresh->RegisterBitmap("master_sign");
	m_pSkinSlotBitmap = g_pFresh->RegisterBitmap("team_base");

	m_pItemSlotBitmap[0] = g_pFresh->RegisterBitmap("item_slot_n");
	m_pItemSlotBitmap[1] = g_pFresh->RegisterBitmap("item_slot_o");
	m_pItemSlotBitmap[2] = g_pFresh->RegisterBitmap("item_slot_d");
	m_pItemSlotEmpty = g_pFresh->RegisterBitmap("item_slot_empty");

	m_pItemDescBottom[0] = (Bitmap*)g_pFresh->RegisterBitmap("itemdesc_01");
	m_pItemDescBottom[1] = (Bitmap*)g_pFresh->RegisterBitmap("itemdesc_02");

	m_pGiftPageBitmap[0] = g_pFresh->RegisterBitmap("gift_arrow_white_2");
	m_pGiftPageBitmap[1] = g_pFresh->RegisterBitmap("gift_number_01");
	m_pGiftPageBitmap[2] = g_pFresh->RegisterBitmap("gift_number_02");
	m_pGiftPageBitmap[3] = g_pFresh->RegisterBitmap("gift_number_03");
	m_pGiftPageBitmap[4] = g_pFresh->RegisterBitmap("gift_arrow_white");

	m_pFurnitureItemBg = g_pFresh->RegisterBitmap("rmr_arg_item_base_n");
	m_pFurnitureItemDownBg = g_pFresh->RegisterBitmap("rmr_arg_item_base_d");
	m_pFurnitureSelFrame = g_pFresh->RegisterBitmap("rmr_arg_item_base_s");
	m_pFurnitureEmptyBg = g_pFresh->RegisterBitmap("rmr_arg_item_base_empty");

	m_pItemDescFrame[0] = g_pFresh->RegisterBitmap("rmr_tip_frm_01");
	m_pItemDescFrame[1] = g_pFresh->RegisterBitmap("rmr_tip_frm_02");
	m_pItemDescFrame[2] = g_pFresh->RegisterBitmap("rmr_tip_frm_03");
	m_pItemDescFrame[3] = g_pFresh->RegisterBitmap("rmr_tip_frm_04");
	m_pItemDescFrame[4] = g_pFresh->RegisterBitmap("rmr_tip_frm_05");
	m_pItemDescFrame[5] = g_pFresh->RegisterBitmap("rmr_tip_frm_06");
	m_pItemDescFrame[6] = g_pFresh->RegisterBitmap("rmr_tip_frm_07");
	m_pItemDescFrame[7] = g_pFresh->RegisterBitmap("rmr_tip_frm_08");
	m_pItemDescFrame[8] = g_pFresh->RegisterBitmap("rmr_tip_frm_09");

	m_pMenuIcon[0] = g_pFresh->RegisterBitmap("rmr_f_wardrobe");
	m_pMenuIcon[1] = g_pFresh->RegisterBitmap("rmr_f_post");
	m_pMenuIcon[2] = g_pFresh->RegisterBitmap("rmr_f_arrange");
	m_pMenuIcon[3] = g_pFresh->RegisterBitmap("rmr_f_shop");
	m_pMenuIcon[4] = g_pFresh->RegisterBitmap("rmr_f_shop");
	m_pMenuIcon[5] = g_pFresh->RegisterBitmap("rmr_f_album");
	m_pMenuIcon[6] = g_pFresh->RegisterBitmap("rmr_f_tv");
	m_pMenuIcon[7] = g_pFresh->RegisterBitmap("rmr_f_visitors_book");
	m_pMenuIcon[8] = g_pFresh->RegisterBitmap("rmr_f_authority");
	m_pMenuIcon[12] = g_pFresh->RegisterBitmap("rmr_f_close");
	m_pMenuIcon[9] = g_pFresh->RegisterBitmap("rmr_f_camera");
	m_pMenuIcon[10] = g_pFresh->RegisterBitmap("rmr_f_hide");

	m_pMenuText[0] = g_pFresh->RegisterBitmap("rmr_f_wardrobe_text");
	m_pMenuText[1] = g_pFresh->RegisterBitmap("rmr_f_post_text");
	m_pMenuText[2] = g_pFresh->RegisterBitmap("rmr_f_arrange_text");
	m_pMenuText[3] = g_pFresh->RegisterBitmap("rmr_f_shop_text");
	m_pMenuText[4] = g_pFresh->RegisterBitmap("rmr_f_shop_text");
	m_pMenuText[5] = g_pFresh->RegisterBitmap("rmr_f_album_text");
	m_pMenuText[6] = g_pFresh->RegisterBitmap("rmr_f_tv_text");
	m_pMenuText[7] = g_pFresh->RegisterBitmap("rmr_f_visitors_book_text");
	m_pMenuText[8] = g_pFresh->RegisterBitmap("rmr_f_authority_text");
	m_pMenuText[12] = g_pFresh->RegisterBitmap("rmr_f_close_text");

	m_pMenuText[9] = g_pFresh->RegisterBitmap("rmr_f_camera_text");
	m_pMenuText[10] = g_pFresh->RegisterBitmap("rmr_f_hide_text");

	if (IsLocalContent((localContentType_t)0x71) == true) {
		m_pMenuIcon[11] = g_pFresh->RegisterBitmap("rmr_f_secbox");
		m_pMenuText[11] = g_pFresh->RegisterBitmap("rmr_f_secbox_text");
	}

	m_pMenuOver = g_pFresh->RegisterBitmap("rmr_menu_o");
	m_pMenuDown = g_pFresh->RegisterBitmap("rmr_menu_d");

	m_pShopMarkBitmap = g_pFresh->RegisterBitmap("mark_begin");
}

// realmyroommainui.cpp:2006
void CRealMyRoomMain::LoadItemImages(void) {
	for (std::map<unsigned int, IFF_STRUCT::sChar>::const_iterator it = ItemManager()->m_charMap.begin(); it != ItemManager()->m_charMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIconMap.find(typeId) == m_itemIconMap.end()) {
			const Bitmap* bitmap = g_pFresh->RegisterBitmap(it->second.m_icon);
			if (bitmap) {
				m_itemIconMap.insert(std::map<unsigned long, const Bitmap*>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sPart>::iterator it = ItemManager()->m_partMap.begin(); it != ItemManager()->m_partMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIconMap.find(typeId) == m_itemIconMap.end() && !(typeId & 0x600)) {
			const Bitmap* bitmap = g_pFresh->RegisterBitmap(it->second.m_icon);
			if (bitmap) {
				m_itemIconMap.insert(std::map<unsigned long, const Bitmap*>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sCaddie>::iterator it = ItemManager()->m_caddieMap.begin(); it != ItemManager()->m_caddieMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIconMap.find(typeId) == m_itemIconMap.end()) {
			const Bitmap* bitmap = g_pFresh->RegisterBitmap(it->second.m_icon);
			if (bitmap) {
				m_itemIconMap.insert(std::map<unsigned long, const Bitmap*>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sCadItem>::iterator it = ItemManager()->m_cadItemMap.begin(); it != ItemManager()->m_cadItemMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIconMap.find(typeId) == m_itemIconMap.end()) {
			const Bitmap* bitmap = g_pFresh->RegisterBitmap(it->second.m_icon);
			if (bitmap) {
				m_itemIconMap.insert(std::map<unsigned long, const Bitmap*>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sItem>::iterator it = ItemManager()->m_itemMap.begin(); it != ItemManager()->m_itemMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIconMap.find(typeId) == m_itemIconMap.end()) {
			const Bitmap* bitmap = g_pFresh->RegisterBitmap(it->second.m_icon);
			if (bitmap) {
				m_itemIconMap.insert(std::map<unsigned long, const Bitmap*>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sClubSet>::iterator it = ItemManager()->m_clubSetMap.begin(); it != ItemManager()->m_clubSetMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIconMap.find(typeId) == m_itemIconMap.end()) {
			const Bitmap* bitmap = g_pFresh->RegisterBitmap(it->second.m_icon);
			if (bitmap) {
				m_itemIconMap.insert(std::map<unsigned long, const Bitmap*>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sBall>::iterator it = ItemManager()->m_ballMap.begin(); it != ItemManager()->m_ballMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIconMap.find(typeId) == m_itemIconMap.end()) {
			const Bitmap* bitmap = g_pFresh->RegisterBitmap(it->second.m_icon);
			if (bitmap) {
				m_itemIconMap.insert(std::map<unsigned long, const Bitmap*>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sSetItem>::iterator it = ItemManager()->m_setItemMap.begin(); it != ItemManager()->m_setItemMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIconMap.find(typeId) == m_itemIconMap.end()) {
			const Bitmap* bitmap = g_pFresh->RegisterBitmap(it->second.m_icon);
			if (bitmap) {
				m_itemIconMap.insert(std::map<unsigned long, const Bitmap*>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sSkin>::iterator it = ItemManager()->m_skinMap.begin(); it != ItemManager()->m_skinMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIconMap.find(typeId) == m_itemIconMap.end()) {
			const Bitmap* bitmap = g_pFresh->RegisterBitmap(it->second.m_icon);
			if (bitmap) {
				m_itemIconMap.insert(std::map<unsigned long, const Bitmap*>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sHairStyle>::iterator it = ItemManager()->m_hairStyleMap.begin(); it != ItemManager()->m_hairStyleMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIconMap.find(typeId) == m_itemIconMap.end()) {
			const Bitmap* bitmap = g_pFresh->RegisterBitmap(it->second.m_icon);
			if (bitmap) {
				m_itemIconMap.insert(std::map<unsigned long, const Bitmap*>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sAuxPart>::iterator it = ItemManager()->m_auxPartMap.begin(); it != ItemManager()->m_auxPartMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIconMap.find(typeId) == m_itemIconMap.end()) {
			const Bitmap* bitmap = g_pFresh->RegisterBitmap(it->second.m_icon);
			if (bitmap) {
				m_itemIconMap.insert(std::map<unsigned long, const Bitmap*>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sQuestDrop>::iterator it = ItemManager()->m_questDropMap.begin(); it != ItemManager()->m_questDropMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIconMap.find(typeId) == m_itemIconMap.end()) {
			const Bitmap* bitmap = g_pFresh->RegisterBitmap(it->second.m_icon);
			if (bitmap) {
				m_itemIconMap.insert(std::map<unsigned long, const Bitmap*>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sQuest>::iterator it = ItemManager()->m_questMap.begin(); it != ItemManager()->m_questMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIconMap.find(typeId) == m_itemIconMap.end()) {
			const Bitmap* bitmap = g_pFresh->RegisterBitmap(it->second.m_icon);
			if (bitmap) {
				m_itemIconMap.insert(std::map<unsigned long, const Bitmap*>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}

	for (std::map<unsigned int, IFF_STRUCT::sMascot>::iterator it = ItemManager()->m_mascotMap.begin(); it != ItemManager()->m_mascotMap.end(); ++it) {
		unsigned long typeId = it->second.m_typeId;
		if (m_itemIconMap.find(typeId) == m_itemIconMap.end()) {
			const Bitmap* bitmap = g_pFresh->RegisterBitmap(it->second.m_icon);
			if (bitmap) {
				m_itemIconMap.insert(std::map<unsigned long, const Bitmap*>::value_type(it->second.m_typeId, bitmap));
			}
		}
	}
}

// realmyroommainui.cpp:2178
void CRealMyRoomMain::CloseAllDialogs(void) {
	FrWnd* pWnd = DYNAMIC_CAST(FrWnd, g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop"));
	if (pWnd) {
		pWnd->Close(true);
	}

	CTaskMain::CloseAllDialogs();
}

// realmyroommainui.cpp:2189
void CRealMyRoomMain::OnMiddleUIInit(PG_MSGARG param) {
	m_pMiddleUIWnd = DYNAMIC_CAST(FrGroupBox, param);
}

// realmyroommainui.cpp:2196
void CRealMyRoomMain::OnTabTxt1Init(PG_MSGARG param) {
	m_pTabTxt[0] = DYNAMIC_CAST(FrArea, param);
}

// realmyroommainui.cpp:2203
void CRealMyRoomMain::OnTabTxt2Init(PG_MSGARG param) {
	m_pTabTxt[1] = DYNAMIC_CAST(FrArea, param);
}

// realmyroommainui.cpp:2210
void CRealMyRoomMain::OnTabTxt3Init(PG_MSGARG param) {
	m_pTabTxt[2] = DYNAMIC_CAST(FrArea, param);
}

// realmyroommainui.cpp:2217
void CRealMyRoomMain::OnSelectedNameInit(PG_MSGARG param) {
	m_pSelectedName = DYNAMIC_CAST(FrStatic, param);
}

// realmyroommainui.cpp:2224
void CRealMyRoomMain::ChangeSubTab(subTab_t tab, bool bForce) {
	if (!bForce && m_roomMode == tab)
		return;

	// Leaving a character page for a pet page (or back) reloads the pet.
	bool bReloadPet = false;
	if (((m_roomMode == 0x15 || m_roomMode <= 4 || m_roomMode == 0x16 || m_roomMode == 0x1A || m_roomMode == 0x10 || m_roomMode == 0x21)
			&& (tab == 0xD || tab == 0xE || tab == 0xF))
		|| ((m_roomMode == 0xD || m_roomMode == 0xE || m_roomMode == 0xF || m_roomMode == 0x21) && m_mainTab == 0)
		|| (m_mainTab == 0 && m_roomMode == 0x10)) {
		bReloadPet = true;
	}

	if (tab == 5)
		tab = (subTab_t)1;

	m_roomMode = tab;

	m_selItemOn = false;
	BuildDisplayStand(tab);
	SetLevelBar();

	m_pItemDescEdit->ClearLine();
	m_pSubTab->Select(m_roomMode);

	if (m_pGiftBgViewer)
		m_pGiftBgViewer->SetVisible(false);
	if (m_pGiftDescEdit)
		m_pGiftDescEdit->SetVisible(false);

	m_pStatWnd->SetVisible(false);

	if (!m_pUccCopyHideWnd->IsVisible()) {
		m_pUccCopyHideWnd->SetVisible(true);
		SetCardBookVisible(false);
	}

	switch (m_roomMode) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 21:
	case 22:
	case 26:
		m_pUccCopyHideWnd->SetMultiSelect(true);
		if (bReloadPet || m_uiState != 1)
			SafeLoadPet(&m_charTypeId, bReloadPet);

		m_pStatWnd->SetVisible(true);
		break;

	case 6:
		if (m_pSelectedName) {
			m_pSelectedName->SetVisible(true);
			// "Selected club"
			m_pSelectedName->SetCaption11("\xBC\xB1\xC5\xC3\xC7\xD1 \xC5\xAC\xB7\xB4");
		}

		m_pUccCopyHideWnd->SetMultiSelect(false);
		ChangeClubset(m_clubSerial);
		m_pStatWnd->SetVisible(true);
		break;

	case 7:
		m_pUccCopyHideWnd->SetMultiSelect(false);
		ChangeAztec(m_aztecTypeId);
		break;

	case 8:
		m_pUccCopyHideWnd->SetMultiSelect(false);
		m_petLoader.WaitUntilWorkDone(9999); if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = 0; }
		break;

	case 9:
		m_pUccCopyHideWnd->SetMultiSelect(false);
		m_petLoader.WaitUntilWorkDone(9999); if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = 0; }
		break;

	case 10:
		m_pUccCopyHideWnd->SetMultiSelect(false);
		m_pStatWnd->SetVisible(true);
		break;

	case 12:
	case 19:
		m_pUccCopyHideWnd->SetMultiSelect(false);
		break;

	case 13:
	case 14:
	case 15:
	case 16:
		m_pUccCopyHideWnd->SetMultiSelect(false);
		SafeLoadPet(&m_charTypeId, bReloadPet);
		break;

	case 34:
		TurnOffItemStatus();
		m_petLoader.WaitUntilWorkDone(9999); if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = 0; }
		break;

	case 17:
		if (IsLocalContent((localContentType_t)0x60))
			break;

		m_pUccCopyHideWnd->SetMultiSelect(false);
		BuildDisplayStand((subTab_t)0x11);
		m_petLoader.WaitUntilWorkDone(9999); if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = 0; }
		if (m_pGiftBgViewer)
			m_pGiftBgViewer->SetVisible(true);
		if (m_pGiftDescEdit)
			m_pGiftDescEdit->SetVisible(true);

		if (IsLocalCountry(0x5B)) {
			m_giftPageCount = WSingleton<CSharedDoc>::Instance()->m_value5056;
			m_giftPageCurrent = WSingleton<CSharedDoc>::Instance()->m_value5055;
			break;
		}

		{
			unsigned long perPage = WSingleton<CSharedDoc>::Instance()->m_value5044;
			unsigned long total = WSingleton<CSharedDoc>::Instance()->m_value504C;
			if (perPage != 0 && total != 0) {
				m_giftPageCount = total / perPage;
				if (total % perPage != 0)
					m_giftPageCount++;
			}

			m_giftPageCurrent = WSingleton<CSharedDoc>::Instance()->m_value5050;
		}
		break;

	case 18:
		m_pUccCopyHideWnd->SetMultiSelect(false);
		break;

	case 20:
		m_pUccCopyHideWnd->SetMultiSelect(false);
		break;

	case 23:
		if (!IsLocalContent((localContentType_t)0x4F))
			break;

		m_pUccCopyHideWnd->SetMultiSelect(false);
		m_petLoader.WaitUntilWorkDone(9999); if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = 0; }
		break;

	case 27:
		if (!IsLocalContent((localContentType_t)0x51))
			break;

		m_pUccCopyHideWnd->SetMultiSelect(false);
		m_pUccCopyHideWnd->SetVisible(false);
		m_petLoader.WaitUntilWorkDone(9999); if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = 0; }
		SetCardBookVisible(true);
		break;

	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
		if (!IsLocalContent((localContentType_t)0x56))
			break;

		m_pUccCopyHideWnd->SetMultiSelect(false);
		if (m_pFurnitureArrangeButton)
			m_pFurnitureArrangeButton->SetVisible(true);

		m_petLoader.WaitUntilWorkDone(9999); if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = 0; }
		break;
	}

	switch (m_roomMode) {
	case 8:
		if (m_pItemSlotBox)
			m_pItemSlotBox->SetVisible(true);
		break;
	default:
		if (m_pItemSlotBox)
			m_pItemSlotBox->SetVisible(false);
		break;
	}

	switch (m_roomMode) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 21:
	case 22:
	case 26:
		m_pUpgradeButton->SetVisible(true);
		m_pDowngradeButton->SetVisible(true);
		break;

	default:
		m_pUpgradeButton->SetVisible(false);
		m_pDowngradeButton->SetVisible(false);
		break;
	}

	SetPetAreaRect();
	SetItemDesc();
	if (m_pUccCopyHideWnd && m_pUccCopyHideWnd->GetScrollBar())
		m_pUccCopyHideWnd->GetScrollBar()->SetCurTopRow(0);
}

// realmyroommainui.cpp:2501
void CRealMyRoomMain::SetPetAreaRect(void) {
	switch (m_roomMode) {
	case 13:
	case 14:
	case 15:
		m_pPetArea = m_pPetAreaWnd[0];
		break;
	case 16:
		m_pPetArea = m_pPetAreaWnd[1];
		break;
	default:
		m_pPetArea = m_pPetAreaWnd[2];
		break;
	}
}

// realmyroommainui.cpp:2524
void CRealMyRoomMain::ChangeMainTab(mainTab_t tab, bool bForce) {
	if (bForce) {
		m_pMainTab->Select(m_mainTab);
	} else if (m_mainTab == tab) {
		return;
	}

	m_mainTab = tab;
	m_pSubTab->ClearTab();

	const char* imgs[3] = { NULL };

	if (m_pEffectViewWnd)
		m_pEffectViewWnd->SetVisible(true);

	if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar)
		WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetHide(false);

	switch (m_mainTab) {
	case 0: {
		m_pTabTxt[0]->SetBgImg("mt_character");
		TurnOnItemSelect();

		SetSubTabImg(imgs, 0x16);
		m_pSubTab->AddTab(imgs, 0x15, NULL);

		for (int i = 1; i < 5; i++) {
			SetSubTabImg(imgs, i + 1);
			m_pSubTab->AddTab(imgs, i, NULL);
		}

		if (IsLocalContent((localContentType_t)0x57)) {
			SetSubTabImg(imgs, 0x1B);
			m_pSubTab->AddTab(imgs, 0x1A, NULL);
		}

		SetSubTabImg(imgs, 0x17);
		m_pSubTab->AddTab(imgs, 0x16, NULL);
		break;
	}

	case 2:
		m_pTabTxt[0]->SetBgImg("mt_caddie");
		TurnOffItemSelect();

		for (int i = 0xA; i < 0xB; i++) {
			SetSubTabImg(imgs, i + 1);
			m_pSubTab->AddTab(imgs, i, NULL);
		}
		break;

	case 1: {
		m_pTabTxt[0]->SetBgImg("mt_item");
		if (m_pSelectedName)
			m_pSelectedName->SetVisible(false);
		TurnOffItemSelect();

		for (int i = 6; i < 10; i++) {
			SetSubTabImg(imgs, i + 1);
			m_pSubTab->AddTab(imgs, i, NULL);
		}

		if (IsLocalContent((localContentType_t)0x51)) {
			SetSubTabImg(imgs, 0x1C);
			m_pSubTab->AddTab(imgs, 0x1B, NULL);
		}
		break;
	}

	case 3:
		m_pTabTxt[0]->SetBgImg("mt_mascot");
		TurnOffItemSelect();

		for (int i = 0xC; i < 0xD; i++) {
			SetSubTabImg(imgs, i + 1);
			m_pSubTab->AddTab(imgs, i, NULL);
		}
		break;

	case 4: {
		m_pTabTxt[0]->SetBgImg("mt_skin");
		TurnOffItemSelect();

		for (int i = 0xD; i < 0x11; i++) {
			SetSubTabImg(imgs, i + 1);
			m_pSubTab->AddTab(imgs, i, NULL);
		}

		SetSubTabImg(imgs, 0x22);
		m_pSubTab->AddTab(imgs, 0x21, NULL);
		SetSubTabImg(imgs, 0x23);
		m_pSubTab->AddTab(imgs, 0x22, NULL);
		break;
	}

	case 5:
		if (IsLocalContent((localContentType_t)0x5B)) {
			if (!WSingleton<CSharedDoc>::Instance()->m_value505A) {
				// The gift list is not loaded yet: ask for its first page.
				WSendPacket packet(GCT_GIFT_LIST_PAGE);
				packet.Encode4(1);
				packet.Send(SEND_TO_DEFAULT);
				break;
			}

			m_pTabTxt[0]->SetBgImg("mt_mybox");
			TurnOffItemSelect();

			// The first gift tab is left out where local content 0x60 is on.
			int first = IsLocalContent((localContentType_t)0x60) ? 1 : 0;
			for (int i = first + 0x11; i <= 0x14; i++) {
				SetSubTabImg(imgs, i + 1);
				m_pSubTab->AddTab(imgs, i, NULL);
			}

			if (IsLocalContent((localContentType_t)0x14)) {
				SetSubTabImg(imgs, 0x19);
				m_pSubTab->AddTab(imgs, 0x18, NULL);
			}

			if (IsLocalContent((localContentType_t)0x4F)) {
				SetSubTabImg(imgs, 0x18);
				m_pSubTab->AddTab(imgs, 0x17, NULL);
			}
		} else {
			m_pTabTxt[0]->SetBgImg("mt_mybox");
			TurnOffItemSelect();

			// The first gift tab is left out where local content 0x60 is on.
			int first = IsLocalContent((localContentType_t)0x60) ? 1 : 0;
			for (int i = first + 0x11; i <= 0x14; i++) {
				SetSubTabImg(imgs, i + 1);
				m_pSubTab->AddTab(imgs, i, NULL);
			}

			if (IsLocalContent((localContentType_t)0x14)) {
				SetSubTabImg(imgs, 0x19);
				m_pSubTab->AddTab(imgs, 0x18, NULL);
			}

			if (IsLocalContent((localContentType_t)0x4F)) {
				SetSubTabImg(imgs, 0x18);
				m_pSubTab->AddTab(imgs, 0x17, NULL);
			}
		}
		break;

	case 6: {
		m_pTabTxt[0]->SetBgImg("mt_interior");
		TurnOffItemSelect();

		SetSubTabImg(imgs, 0x16);
		m_pSubTab->AddTab(imgs, 0x20, NULL);

		for (int i = 0x1C; i < 0x20; i++) {
			SetSubTabImg(imgs, i + 1);
			m_pSubTab->AddTab(imgs, i, NULL);
		}
		break;
	}
	}

	m_pItemDescEdit->SetVisible(false);
	m_pItemDescBg->SetVisible(false);

	m_barButton[3]->Enable(false);

	if (m_pPetExhibition == NULL)
		ChangeSubTab((subTab_t)m_roomMode, true);
}

// realmyroommainui.cpp:2755
void CRealMyRoomMain::ChangeStatTab(statTab_t tab) {
	m_statTab = tab;
	SetLevelBar();
}

// realmyroommainui.cpp:2761
void CRealMyRoomMain::ChangeSelTab(selTab_t tab) {
	m_selTab = tab;
}

// realmyroommainui.cpp:2780
void CRealMyRoomMain::CharacterViewControl(bool bShow) {
	if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar != NULL) {
		if (bShow == true) {
			WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetState(2);
			WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetHide(false);
		} else {
			WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->ChangeCharacterInfo();
			WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetState(0);
			WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetHide(!bShow);
		}
	}
}

// realmyroommainui.cpp:2823
void CRealMyRoomMain::ChangeCharacter(unsigned long typeId) {
	std::map<unsigned int, sCharacterInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_characterMap.find(typeId);
	if (it == WSingleton<CSharedDoc>::Instance()->m_characterMap.end())
		return;

	m_charTypeId = it->second.typeId;
	m_pCharParts = it->second.partTypeIds;
	if (IsLocalContent((localContentType_t)0x57)) {
		memcpy(m_localContentData, it->second.partIds, sizeof(m_localContentData));
	}
	m_pCharCards = it->second.auxParts.tids;
	m_pCharLevels = (const char*)&it->second.unknown6A;
	m_charSerial = it->second.id;
	WSingleton<CSharedDoc>::Instance()->m_myCharacterId = it->second.id;

	if (MyUID() != WSingleton<CSharedDoc>::Instance()->m_myRoomOwnerUID) {
		if (m_mainTab == 0) {
			WSingleton<CSharedDoc>::Instance()->BuildMyPartTidList(it->second);

			sCharacterInfo& charInfo = WSingleton<CSharedDoc>::Instance()->m_characterMap[WSingleton<CSharedDoc>::Instance()->m_myCharacterId];
			CRealMyRoom_Avatar* pAvatar = WSingleton<CRealMyRoom_AvatarManager>::Instance()->FindAvatarByGuid(WSingleton<CSharedDoc>::Instance()->m_userInfo.guid);
			if (pAvatar && pAvatar->m_charInfo.id != charInfo.id) {
				static_cast<CRealMyRoomTask*>(GetTask())->RemoveAvatarBurden(CRealMyRoom_AvatarBurden(pAvatar->m_guid, pAvatar->m_charInfo, pAvatar->m_pPetFrame));

				pAvatar->SetCharactorInfo(charInfo);

				static_cast<CRealMyRoomTask*>(GetTask())->AddAvatarBurden(CRealMyRoom_AvatarBurden(pAvatar->m_guid, charInfo, 0));

				if (!m_bHideMiddleUI) {
					CFxSequence* pSeq = CFx::Instance()->OpenSequence("Talk_Room.seq", true);
					if (pSeq) {
						pSeq->Pos() = WVector(pAvatar->m_position.x, pAvatar->m_position.y + 5.0f, pAvatar->m_position.z);
					}
				}
			}
		}
	} else {
		if (m_mainTab == 0) {
			WSingleton<CSharedDoc>::Instance()->BuildMyPartTidList(it->second);

			BuildDisplayStand((subTab_t)m_roomMode);
			m_petLoader.WaitUntilWorkDone(9999); if (m_pPetExhibition == NULL || m_pPetExhibition->GetTypeId() != m_charTypeId) m_petLoader.Work(this, 0, &m_charTypeId, true, -1);

			sCharacterInfo& charInfo = WSingleton<CSharedDoc>::Instance()->m_characterMap[WSingleton<CSharedDoc>::Instance()->m_myCharacterId];
			CRealMyRoom_Avatar* pAvatar = WSingleton<CRealMyRoom_AvatarManager>::Instance()->FindAvatarByGuid(WSingleton<CSharedDoc>::Instance()->m_userInfo.guid);
			if (pAvatar && pAvatar->m_charInfo.id != charInfo.id) {
				static_cast<CRealMyRoomTask*>(GetTask())->RemoveAvatarBurden(CRealMyRoom_AvatarBurden(pAvatar->m_guid, pAvatar->m_charInfo, pAvatar->m_pPetFrame));

				pAvatar->SetCharactorInfo(charInfo);

				static_cast<CRealMyRoomTask*>(GetTask())->AddAvatarBurden(CRealMyRoom_AvatarBurden(pAvatar->m_guid, charInfo, 0));

				if (!m_bHideMiddleUI) {
					CFxSequence* pSeq = CFx::Instance()->OpenSequence("Talk_Room.seq", true);
					if (pSeq) {
						pSeq->Pos() = WVector(pAvatar->m_position.x, pAvatar->m_position.y + 5.0f, pAvatar->m_position.z);
					}
				}
			}

			TurnOnItemStatus();

			m_selItemOn = false;
			if (m_pSelItemNextButton && m_pSelItemPrevButton) {
				m_pSelItemNextButton->Enable(false);
				m_pSelItemPrevButton->Enable(false);

				std::map<unsigned int, sCharacterInfo>& characters = WSingleton<CSharedDoc>::Instance()->m_characterMap;
				std::map<unsigned int, sCharacterInfo>::iterator it2;
				for (it2 = characters.begin(); it2 != characters.end(); ++it2) {
					if (it2->second.id == typeId) {
						if (++it2 != characters.end()) {
							m_pSelItemNextButton->Enable(true);
						}
						break;
					}
				}

				std::map<unsigned int, sCharacterInfo>& characters2 = WSingleton<CSharedDoc>::Instance()->m_characterMap;
				std::map<unsigned int, sCharacterInfo>::reverse_iterator rit;
				for (rit = characters2.rbegin(); rit != characters2.rend(); ++rit) {
					if (rit->second.id == typeId) {
						if (++rit != characters2.rend()) {
							m_pSelItemPrevButton->Enable(true);
						}
						break;
					}
				}
			}

			if (m_charTypeId) {
				const char* name = ItemManager()->GetItemName(m_charTypeId);
				if (name && m_pSelectedName) {
					m_pSelectedName->SetCaption11(name);
				}
			}
		} else if (m_mainTab == 4) {
			WSingleton<CSharedDoc>::Instance()->BuildMyPartTidList(it->second);
			BuildDisplayStand((subTab_t)m_roomMode);
			SafeLoadPet(&m_charTypeId, false);
		} else if (m_statTab == 1) {
			SetLevelBar();
		}

		AutoScroll();
	}
}

// realmyroommainui.cpp:2956
bool CRealMyRoomMain::ChangeCaddie(unsigned long id) {
	bool result = false;
	std::map<unsigned int, sCaddieInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_caddieMap.find(id);

	if (it == WSingleton<CSharedDoc>::Instance()->m_caddieMap.end()) {
		m_caddieTypeId = 0;
		m_caddieValueA = 0;
		WSingleton<CSharedDoc>::Instance()->m_myCaddieId = 0;

		if (m_roomMode == 10) {
			HidePet(true);
			m_pUccCopyHideWnd->UnselectAllItem();

			TurnOnItemStatus();
			if (m_pSelectedName) {
				// "Caddie"
				m_pSelectedName->SetCaption11("\xC4\xB3\xB5\xF0");
			}
		}

		if (id == 0) {
			WSingleton<CSharedDoc>::Instance()->m_realEquip[0] = 0;
		}
	} else {
		m_caddieValueA = it->second.id;
		m_caddieTypeId = it->second.typeId;

		if (it->second.cadItemTypeId != 0)
			m_caddieValueB = it->second.cadItemTypeId;
		else
			m_caddieValueB = it->second.typeId;

		IFF_STRUCT::sCaddie* pCaddie = WSingleton<CSharedDoc>::Instance()->m_itemManager.FindCaddie(m_caddieTypeId);
		if (pCaddie) {
			unsigned char level = pCaddie->m_level & 0x7F;

			if (level <= WSingleton<CSharedDoc>::Instance()->m_myLevel) {
				WSingleton<CSharedDoc>::Instance()->m_myCaddieId = it->second.id;
				WSingleton<CSharedDoc>::Instance()->m_realEquip[0] = it->second.id;
				result = true;
			}
		}

		if (m_roomMode == 10) {
			BuildDisplayStand((subTab_t)10);
			SafeLoadPet(&m_caddieValueB, false);
			TurnOnItemStatus();

			m_selItemOn = false;
			m_pSelItemNextButton->Enable(false);
			m_pSelItemPrevButton->Enable(false);

			std::map<unsigned int, sCaddieInfo>& caddies = WSingleton<CSharedDoc>::Instance()->m_caddieMap;
			std::map<unsigned int, sCaddieInfo>::iterator it2;
			for (it2 = caddies.begin(); it2 != caddies.end(); ++it2) {
				if (it2->second.id == id) {
					if (++it2 != caddies.end()) {
						m_pSelItemNextButton->Enable(true);
					}
					break;
				}
			}

			std::map<unsigned int, sCaddieInfo>& caddies2 = WSingleton<CSharedDoc>::Instance()->m_caddieMap;
			std::map<unsigned int, sCaddieInfo>::reverse_iterator rit;
			for (rit = caddies2.rbegin(); rit != caddies2.rend(); ++rit) {
				if (rit->second.id == id) {
					if (++rit != caddies2.rend()) {
						m_pSelItemPrevButton->Enable(true);
					}
					break;
				}
			}

			if (result) {
				const char* name = ItemManager()->GetItemName(m_caddieTypeId);
				if (name && m_pSelectedName) {
					m_pSelectedName->SetCaption11(name);
				}
			}
		}
	}

	return result;
}

// realmyroommainui.cpp:3069
void CRealMyRoomMain::ChangeMascot(unsigned long id) {
	std::map<unsigned int, sMascotInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myMascotMap.find(id);

	if (it == WSingleton<CSharedDoc>::Instance()->m_myMascotMap.end()) {
		m_mascotTypeId = 0;
		m_mascotSerial = 0;
		WSingleton<CSharedDoc>::Instance()->m_myMascotId = 0;

		if (m_roomMode == 12) {
			HidePet(true);
			m_pUccCopyHideWnd->UnselectAllItem();
		}

		if (id == 0) {
			WSingleton<CSharedDoc>::Instance()->m_realEquip[0x1A] = 0;
		}
	} else {
		m_mascotTypeId = it->second.typeId;
		m_mascotSerial = it->second.id;
		WSingleton<CSharedDoc>::Instance()->m_myMascotId = it->second.id;
		WSingleton<CSharedDoc>::Instance()->m_realEquip[0x1A] = it->second.id;

		if (m_roomMode == 12) {
			BuildDisplayStand((subTab_t)12);
			SafeLoadPet(&m_mascotTypeId, false);
			TurnOffItemStatus();

			m_selItemOn = false;
			m_pSelItemPrevButton->Enable(false);
			m_pSelItemNextButton->Enable(false);

			std::map<unsigned int, sMascotInfo>& mascots = WSingleton<CSharedDoc>::Instance()->m_myMascotMap;
			std::map<unsigned int, sMascotInfo>::iterator it2;
			for (it2 = mascots.begin(); it2 != mascots.end(); ++it2) {
				if (it2->second.id == id) {
					if (++it2 != mascots.end()) {
						m_pSelItemNextButton->Enable(true);
					}
					break;
				}
			}

			std::map<unsigned int, sMascotInfo>& mascots2 = WSingleton<CSharedDoc>::Instance()->m_myMascotMap;
			std::map<unsigned int, sMascotInfo>::reverse_iterator rit;
			for (rit = mascots2.rbegin(); rit != mascots2.rend(); ++rit) {
				if (rit->second.id == id) {
					if (++rit != mascots2.rend()) {
						m_pSelItemPrevButton->Enable(true);
					}
					break;
				}
			}
		}
	}

	if (IsLocalContent((localContentType_t)0x51)) {
		WSingleton<CSharedDoc>::Instance()->SendEquipItemSlot();
	} else {
		WSingleton<CSharedDoc>::Instance()->SendEquipItemSlotByMascot();
	}
}

// realmyroommainui.cpp:3135
int CRealMyRoomMain::ChangeClubset(unsigned long serial) {
	std::map<unsigned int, sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myItemMap.find(serial);
	if (it == WSingleton<CSharedDoc>::Instance()->m_myItemMap.end()) {
		return 0;
	}

	m_clubTypeId = it->second.m_typeId;
	m_clubSerial = it->second.m_serial;
	m_pClubLevels = it->second.m_clubStatLevel;

	int result = 0;
	IFF_STRUCT::sClubSet* pClubSet = WSingleton<CSharedDoc>::Instance()->m_itemManager.FindClubSet(m_clubTypeId);
	if (pClubSet) {
		int level = pClubSet->m_level & 0x7f;

		int myLevel = WSingleton<CSharedDoc>::Instance()->m_myLevel;
		if (pClubSet->m_level & 0x80) {
			if (level >= myLevel) {
				WSingleton<CSharedDoc>::Instance()->m_myClubId = it->second.m_serial;
				result = 1;
			} else {
				std::map<unsigned int, sItemInfo>::iterator it2;
				for (it2 = WSingleton<CSharedDoc>::Instance()->m_myItemMap.begin(); it2 != WSingleton<CSharedDoc>::Instance()->m_myItemMap.end(); ++it2) {
					if (it2->second.m_typeId == 0x10000000) {
						WSingleton<CSharedDoc>::Instance()->m_myClubId = it2->second.m_serial;
						m_clubTypeId = it2->second.m_typeId;
						m_clubSerial = it2->second.m_serial;
						m_pClubLevels = it2->second.m_clubStatLevel;
						break;
					}
				}
				result = 2;
			}
		} else if (level <= myLevel) {
			WSingleton<CSharedDoc>::Instance()->m_myClubId = it->second.m_serial;
			result = 1;
		}
	}

	if (m_roomMode == 6) {
		SafeLoadPet(&m_clubTypeId, false);
		TurnOnItemStatus();

		m_selItemOn = false;
		m_pSelItemPrevButton->Enable(false);
		m_pSelItemNextButton->Enable(false);

		std::map<unsigned int, sItemInfo>& items = WSingleton<CSharedDoc>::Instance()->m_myItemMap;
		std::map<unsigned int, sItemInfo>::iterator it3;
		for (it3 = items.begin(); it3 != items.end(); ++it3) {
			if (it3->second.m_serial == serial) {
				if (++it3 != items.end()) {
					m_pSelItemNextButton->Enable(true);
				}
				break;
			}
		}

		std::map<unsigned int, sItemInfo>& items2 = WSingleton<CSharedDoc>::Instance()->m_myItemMap;
		std::map<unsigned int, sItemInfo>::reverse_iterator rit;
		for (rit = items2.rbegin(); rit != items2.rend(); ++rit) {
			if (rit->second.m_serial == serial) {
				if (++rit != items2.rend()) {
					m_pSelItemPrevButton->Enable(true);
				}
				break;
			}
		}
	}

	return result;
}

// realmyroommainui.cpp:3235
void CRealMyRoomMain::ChangeAztec(unsigned long typeId) {
	m_aztecTypeId = typeId;
	WSingleton<CSharedDoc>::Instance()->m_myBallId = typeId;

	if (m_roomMode == 7) {
		SafeLoadPet(&m_aztecTypeId, false);
		TurnOffItemStatus();

		m_selItemOn = false;
		m_pSelItemPrevButton->Enable(false);
		m_pSelItemNextButton->Enable(false);

		std::list<sItemInfo>& gears = WSingleton<CSharedDoc>::Instance()->m_myGearList;
		std::list<sItemInfo>::iterator it;
		for (it = gears.begin(); it != gears.end(); ++it) {
			if (it->m_typeId == typeId) {
				if (++it != gears.end()) {
					m_pSelItemNextButton->Enable(true);
				}
				break;
			}
		}

		std::list<sItemInfo>& gears2 = WSingleton<CSharedDoc>::Instance()->m_myGearList;
		std::list<sItemInfo>::reverse_iterator rit;
		for (rit = gears2.rbegin(); rit != gears2.rend(); ++rit) {
			if (rit->m_typeId == typeId) {
				if (++rit != gears2.rend()) {
					m_pSelItemPrevButton->Enable(true);
				}
				break;
			}
		}
	}
}

// realmyroommainui.cpp:3274
const Bitmap* CRealMyRoomMain::GetItemIcon(unsigned long typeId) {
	const Bitmap* pIcon = NULL;

	std::map<unsigned long, const Bitmap*>::iterator it = m_itemIconMap.find(typeId);
	if (it == m_itemIconMap.end()) {
		IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(typeId);
		if (pItem != NULL) {
			pIcon = g_pFresh->RegisterBitmap(pItem->m_icon);
			if (pIcon != NULL) {
				m_itemIconMap.insert(std::map<unsigned long, const Bitmap*>::value_type(pItem->m_typeId, pIcon));
			}
		}
	} else {
		pIcon = it->second;
	}

	return pIcon;
}

// realmyroommainui.cpp:3304
void CRealMyRoomMain::DrawCharacterList(FrGraphicInterface* pGI, unsigned long typeId) {
	if (m_selItemOn && CSharedDoc::Instance()->m_characterMap.size() != 0) {
		int count = CSharedDoc::Instance()->m_characterMap.size();
		int index = 0;

		int cols = count > 16 ? 4 : (count > 4 ? 3 : (count > 1 ? 2 : 1));

		int rows = count / cols + (count % cols != 0 ? 1 : 0);

		float listH = (rows - 1) + rows * 50.0f;
		float posY = 419.0f - listH;
		float posX = 18.0f;
		pGI->DrawTexture(m_pListBgBitmap[0], WRect(13.0f, posY - 5.0f, (cols - 1) + cols * 46.0f + 10.0f, listH + 10.0f), 0xFFFFFFFF, 0);

		WRect selRect(0, 0, 0, 0);
		WRect overRect(0, 0, 0, 0);
		std::map<unsigned int, sCharacterInfo>::const_iterator it;
		for (it = CSharedDoc::Instance()->m_characterMap.begin(); it != CSharedDoc::Instance()->m_characterMap.end(); ++it) {
			const Bitmap* pIcon = GetItemIcon(it->second.typeId);
			if (pIcon == NULL) {
				continue;
			}

			index++;
			WRect rect(posX, posY, 46.0f, 50.0f);
			pGI->DrawTexture(pIcon, rect, 0xFFFFFFFF, 0);
			if (index % cols != 0) {
				posX += 47.0f;
			} else {
				posY += 51.0f;
				posX = 18.0f;
			}

			if (m_charSerial == it->second.id) {
				selRect = WRect(rect.x - 4.0f, rect.y - 4.0f, rect.w + 8.0f, rect.h + 8.0f);
			}

			const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
			if (rect.IsInRect(mouse)) {
				overRect = WRect(rect.x - 4.0f, rect.y - 4.0f, rect.w + 8.0f, rect.h + 8.0f);

				if (g_input->GetButton(LEFT_BUTTON) == 1) {
					if (!m_petLoader.IsWorking(0)) {
						ChangeCharacter(it->second.id);
					}
				}
			}
		}

		if (selRect.w != 0.0f) {
			pGI->DrawTexture(m_pListSelBitmap, selRect, 0xFFFFFFFF, 0);
		}

		if (overRect.w != 0.0f) {
			pGI->DrawTexture(m_pListOverBitmap, overRect, 0xFFFFFFFF, 0);
		}
	}

	if (m_selItemOn && g_input->GetButton(LEFT_BUTTON) == 1) {
		WRect wndRect = m_pItemSelectWnd->GetRect();
		if (!wndRect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
			m_selItemOn = false;
		}
	}
}

// realmyroommainui.cpp:3391
void CRealMyRoomMain::DrawCaddieList(FrGraphicInterface* pGI, unsigned long typeId) {
	if (m_selItemOn && CSharedDoc::Instance()->m_caddieMap.size() != 0) {
		int count = CSharedDoc::Instance()->m_caddieMap.size();
		int index = 0;

		int cols = count > 16 ? 4 : (count > 4 ? 3 : (count > 1 ? 2 : 1));

		int rows = count / cols + (count % cols != 0 ? 1 : 0);

		float listH = (rows - 1) + rows * 50.0f;
		float posY = 419.0f - listH;
		float posX = 18.0f;
		pGI->DrawTexture(m_pListBgBitmap[0], WRect(13.0f, posY - 5.0f, (cols - 1) + cols * 46.0f + 10.0f, listH + 10.0f), 0xFFFFFFFF, 0);

		WRect selRect(0, 0, 0, 0);
		WRect overRect(0, 0, 0, 0);
		std::map<unsigned int, sCaddieInfo>::const_iterator it;
		for (it = CSharedDoc::Instance()->m_caddieMap.begin(); it != CSharedDoc::Instance()->m_caddieMap.end(); ++it) {
			const Bitmap* pIcon = GetItemIcon(it->second.typeId);
			if (pIcon == NULL) {
				continue;
			}

			index++;
			WRect rect(posX, posY, 46.0f, 50.0f);
			pGI->DrawTexture(pIcon, rect, 0xFFFFFFFF, 0);
			if (index % cols != 0) {
				posX += 47.0f;
			} else {
				posY += 51.0f;
				posX = 18.0f;
			}

			if (m_caddieValueA == it->second.id) {
				selRect = WRect(rect.x - 4.0f, rect.y - 4.0f, rect.w + 8.0f, rect.h + 8.0f);
			}

			const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
			if (rect.IsInRect(mouse)) {
				overRect = WRect(rect.x - 4.0f, rect.y - 4.0f, rect.w + 8.0f, rect.h + 8.0f);

				if (g_input->GetButton(LEFT_BUTTON) == 1) {
					if (!m_petLoader.IsWorking(0)) {
						ChangeCaddie(it->second.id);
					}
				}
			}
		}

		if (selRect.w != 0.0f) {
			pGI->DrawTexture(m_pListSelBitmap, selRect, 0xFFFFFFFF, 0);
		}

		if (overRect.w != 0.0f) {
			pGI->DrawTexture(m_pListOverBitmap, overRect, 0xFFFFFFFF, 0);
		}
	}

	if (m_selItemOn && g_input->GetButton(LEFT_BUTTON) == 1) {
		WRect rect = m_pItemSelectWnd->GetRect();
		if (!rect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
			m_selItemOn = false;
		}
	}
}

// realmyroommainui.cpp:3478
void CRealMyRoomMain::DrawMascotList(FrGraphicInterface* pGI, unsigned long typeId) {
	if (m_selItemOn && CSharedDoc::Instance()->m_myMascotMap.size() != 0) {
		int count = CSharedDoc::Instance()->m_myMascotMap.size();
		int index = 0;

		int cols = count > 16 ? 4 : (count > 4 ? 3 : (count > 1 ? 2 : 1));

		int rows = count / cols + (count % cols != 0 ? 1 : 0);

		float listH = (rows - 1) + rows * 50.0f;
		float posY = 419.0f - listH;
		float posX = 18.0f;
		pGI->DrawTexture(m_pListBgBitmap[0], WRect(13.0f, posY - 5.0f, (cols - 1) + cols * 46.0f + 10.0f, listH + 10.0f), 0xFFFFFFFF, 0);

		WRect selRect(0, 0, 0, 0);
		WRect overRect(0, 0, 0, 0);
		std::map<unsigned int, sMascotInfo>::const_iterator it;
		for (it = CSharedDoc::Instance()->m_myMascotMap.begin(); it != CSharedDoc::Instance()->m_myMascotMap.end(); ++it) {
			const Bitmap* pIcon = GetItemIcon(it->second.typeId);
			if (pIcon == NULL) {
				continue;
			}

			index++;
			WRect rect(posX, posY, 46.0f, 50.0f);
			pGI->DrawTexture(pIcon, rect, 0xFFFFFFFF, 0);
			if (index % cols != 0) {
				posX += 47.0f;
			} else {
				posY += 51.0f;
				posX = 18.0f;
			}

			if (m_mascotSerial == it->second.id) {
				selRect = WRect(rect.x - 4.0f, rect.y - 4.0f, rect.w + 8.0f, rect.h + 8.0f);
			}

			const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
			if (rect.IsInRect(mouse)) {
				overRect = WRect(rect.x - 4.0f, rect.y - 4.0f, rect.w + 8.0f, rect.h + 8.0f);

				if (g_input->GetButton(LEFT_BUTTON) == 1) {
					if (!m_petLoader.IsWorking(0)) {
						ChangeMascot(it->second.id);
					}
				}
			}
		}

		if (selRect.w != 0.0f) {
			pGI->DrawTexture(m_pListSelBitmap, selRect, 0xFFFFFFFF, 0);
		}

		if (overRect.w != 0.0f) {
			pGI->DrawTexture(m_pListOverBitmap, overRect, 0xFFFFFFFF, 0);
		}
	}

	if (m_selItemOn && g_input->GetButton(LEFT_BUTTON) == 1) {
		WRect rect = m_pItemSelectWnd->GetRect();
		if (!rect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
			m_selItemOn = false;
		}
	}
}

// realmyroommainui.cpp:3565
void CRealMyRoomMain::DrawAztecList(FrGraphicInterface* pGI, unsigned long typeId) {
	if (!m_selItemOn) {
		return;
	}

	int count = CSharedDoc::Instance()->m_myGearList.size();
	if (count != 0) {
		int index = 0;
		int line = 1;
		int rows = count / 4 + (count % 4 != 0 ? 1 : 0);

		WPoint pos(18.0f, 407.0f - rows * 50.0f + (rows - 1) * 5.0f);
		float top = pos.y - 12.0f;
		float midH = rows * 50.0f + 15.0f;
		pGI->DrawTexture(m_pListBgBitmap[0], WRect(6.0f, top, 223.0f, 12.0f), 0xFFFFFFFF, 0);
		pGI->DrawTexture(m_pListBgBitmap[1], WRect(6.0f, top + 12.0f, 223.0f, midH), 0xFFFFFFFF, 0);
		pGI->DrawTexture(m_pListBgBitmap[2], WRect(6.0f, midH + top + 12.0f, 223.0f, 20.0f), 0xFFFFFFFF, 0);

		for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_myGearList.begin(); it != CSharedDoc::Instance()->m_myGearList.end(); ++it) {
			unsigned long aztecTypeId = it->m_typeId;
			const Bitmap* pIcon = GetItemIcon(aztecTypeId);
			if (pIcon == NULL) {
				continue;
			}

			index++;
			WRect iconRect(pos.x, pos.y, 46.0f, 50.0f);
			pGI->DrawTexture(pIcon, iconRect, 0xFFFFFFFF, 0);
			if (index % 4 != 0) {
				pos.x += 51.0f;
			} else {
				pos.y += 55.0f;
				pos.x = 18.0f;
				line++;
			}

			WRect frame(iconRect.x - 6.0f, iconRect.y - 6.0f, iconRect.w + 12.0f, iconRect.h + 12.0f);

			if (m_aztecTypeId == aztecTypeId) {
				pGI->DrawTexture(m_pListSelBitmap, frame, 0xFFFFFFFF, 0);
			}

			const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
			if (iconRect.IsInRect(mouse)) {
				pGI->DrawTexture(m_pListOverBitmap, frame, 0xFFFFFFFF, 0);

				if (g_input->GetButton(LEFT_BUTTON) == 1) {
					if (!m_petLoader.IsWorking(0)) {
						ChangeAztec(aztecTypeId);
					}
				}
			}
		}

		if (rows == line && 4 - index % 4 != 0) {
			for (int i = 0; i < 4 - index % 4; i++) {
				pGI->DrawTexture(m_pListEmptyBitmap, WRect(pos.x, pos.y, 46.0f, 50.0f), 0xFFFFFFFF, 0);
				pos.x += 51.0f;
			}
		}
	}

	if (m_selItemOn && g_input->GetButton(LEFT_BUTTON) == 1) {
		WRect selRect = m_pItemSelectWnd->GetRect();
		if (!selRect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
			m_selItemOn = false;
		}
	}
}

// realmyroommainui.cpp:3653
void CRealMyRoomMain::DrawClubList(FrGraphicInterface* pGI, unsigned long typeId) {
	if (!m_selItemOn) {
		return;
	}

	int count = CSharedDoc::Instance()->m_myItemMap.size();
	if (count != 0) {
		int index = 0;
		int line = 1;
		int rows = count / 4 + (count % 4 != 0 ? 1 : 0);

		WPoint pos(18.0f, 407.0f - rows * 50.0f + (rows - 1) * 5.0f);
		float top = pos.y - 12.0f;
		float midH = rows * 50.0f + 15.0f;
		pGI->DrawTexture(m_pListBgBitmap[0], WRect(6.0f, top, 223.0f, 12.0f), 0xFFFFFFFF, 0);
		pGI->DrawTexture(m_pListBgBitmap[1], WRect(6.0f, top + 12.0f, 223.0f, midH), 0xFFFFFFFF, 0);
		pGI->DrawTexture(m_pListBgBitmap[2], WRect(6.0f, midH + top + 12.0f, 223.0f, 20.0f), 0xFFFFFFFF, 0);

		std::map<unsigned int, sItemInfo>::const_iterator it;
		for (it = CSharedDoc::Instance()->m_myItemMap.begin(); it != CSharedDoc::Instance()->m_myItemMap.end(); ++it) {
			const Bitmap* pIcon = GetItemIcon(it->second.m_typeId);
			if (pIcon == NULL) {
				continue;
			}

			index++;
			WRect iconRect(pos.x, pos.y, 46.0f, 50.0f);
			pGI->DrawTexture(pIcon, iconRect, 0xFFFFFFFF, 0);
			if (index % 4 != 0) {
				pos.x += 51.0f;
			} else {
				pos.y += 55.0f;
				pos.x = 18.0f;
				line++;
			}

			WRect frame(iconRect.x - 6.0f, iconRect.y - 6.0f, iconRect.w + 12.0f, iconRect.h + 12.0f);

			if (m_clubSerial == it->second.m_serial) {
				pGI->DrawTexture(m_pListSelBitmap, frame, 0xFFFFFFFF, 0);
			}

			const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
			if (iconRect.IsInRect(mouse)) {
				pGI->DrawTexture(m_pListOverBitmap, frame, 0xFFFFFFFF, 0);

				if (g_input->GetButton(LEFT_BUTTON) == 1) {
					if (!m_petLoader.IsWorking(0)) {
						ChangeClubset(it->second.m_serial);
					}
				}
			}
		}

		if (rows == line && 4 - index % 4 != 0) {
			for (int i = 0; i < 4 - index % 4; i++) {
				pGI->DrawTexture(m_pListEmptyBitmap, WRect(pos.x, pos.y, 46.0f, 50.0f), 0xFFFFFFFF, 0);
				pos.x += 51.0f;
			}
		}
	}

	if (m_selItemOn && g_input->GetButton(LEFT_BUTTON) == 1) {
		WRect selRect = m_pItemSelectWnd->GetRect();
		if (!selRect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
			m_selItemOn = false;
		}
	}
}

// realmyroommainui.cpp:3739
void CRealMyRoomMain::OnPetInit(PG_MSGARG param) {
	m_pPetAreaWnd[2] = DYNAMIC_CAST(FrArea, param);
}

// realmyroommainui.cpp:3744
void CRealMyRoomMain::OnSkinPetInit(PG_MSGARG param) {
	m_pPetAreaWnd[0] = DYNAMIC_CAST(FrArea, param);
}

// realmyroommainui.cpp:3749
void CRealMyRoomMain::OnSkinPet2Init(PG_MSGARG param) {
	m_pPetAreaWnd[1] = DYNAMIC_CAST(FrArea, param);
}

// realmyroommainui.cpp:3754
void CRealMyRoomMain::DrawSkinFrame(void) {
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == NULL) {
		return;
	}

	if (m_displaySkin[0] != 0) {
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(m_displaySkin[0]);
		if (pSkin != NULL) {
			const Bitmap* pBmp = g_pFresh->GetBitmap(pSkin->m_cutinImage);
			if (pBmp != NULL) {
				WRect area = m_pPetArea->GetRect();
				float bw = (float)pBmp->Width();
				float bh = (float)pBmp->Height();

				if (m_skinSlot[0] > bw) {
					m_skinSlot[0] -= bw;
				} else if (m_skinSlot[0] < 0.0f) {
					m_skinSlot[0] = bw + m_skinSlot[0];
				}

				if (m_skinSlot[1] > bh) {
					m_skinSlot[1] -= bh;
				} else if (m_skinSlot[1] < 0.0f) {
					m_skinSlot[1] = bh + m_skinSlot[1];
				}

				WRect src(m_skinSlot[0], m_skinSlot[1], bw - m_skinSlot[0], bh - m_skinSlot[1]);

				WRect dst;
				dst.x = area.x;
				dst.y = area.y;
				dst.w = src.w;
				dst.h = src.h;

				while (dst.y < area.y + area.h) {
					while (dst.x < area.x + area.w) {
						pGI->DrawTexture(pBmp, src, dst, 0xFFFFFFFF, 0);

						dst.x += dst.w;
						dst.w = bw > area.x + area.w - dst.x ? area.x + area.w - dst.x : bw;

						src.x = 0.0f;
						src.w = dst.w;
					}

					dst.x = area.x;
					dst.y += dst.h;
					dst.w = bw - m_skinSlot[0];
					dst.h = bh > area.y + area.h - dst.y ? area.y + area.h - dst.y : bh;

					src.x = m_skinSlot[0];

					src.w = dst.w;
					src.y = 0.0f;
					src.h = dst.h;
				}
			}
		}
	}

	if (!m_petLoader.IsWorking(0)) {
		if (!m_bongdariShop[2] && WSingleton<CRankingInfo>::Instance()->GetDlg() == NULL && USERINFODLG() == NULL) {
			m_petLoader.Enter();
			if (m_pPetExhibition != NULL) {
				m_pPetExhibition->Display(0.0f, 0.0f);
			}
			m_petLoader.Leave();
		}
	}

	if (m_displaySkin[1] != 0) {
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(m_displaySkin[1]);
		if (pSkin != NULL) {
			const Bitmap* pBmp = g_pFresh->GetBitmap(pSkin->m_cutinImage);
			if (pBmp != NULL) {
				pGI->DrawTexture(pBmp, m_pPetArea->GetRect(), 0xFFFFFFFF, 0);
			}
		}
	}

	if (m_displaySkin[2] != 0) {
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(m_displaySkin[2]);
		if (pSkin != NULL) {
			const Bitmap* pBmp = g_pFresh->GetBitmap(pSkin->m_cutinImage);
			if (pBmp != NULL) {
				pGI->DrawTexture(pBmp, m_pPetArea->GetRect(), 0xFFFFFFFF, 0);
			}
		}
	}
}

// realmyroommainui.cpp:3855
void CRealMyRoomMain::DrawSkinSlot(void) {
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == NULL) {
		return;
	}

	pGI->DrawTexture(m_pSkinSlotBitmap, WRect(116.0f, 308.0f, (float)m_pSkinSlotBitmap->Width(), (float)m_pSkinSlotBitmap->Height()), 0xFFFFFFFF, 0);

	if (m_displaySkin[3] != 0) {
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(m_displaySkin[3]);
		if (pSkin != NULL) {
			const Bitmap* pBmp = g_pFresh->GetBitmap(pSkin->m_cutinImage);
			if (pBmp != NULL) {
				pGI->DrawTexture(pBmp, WRect(116.0f, 308.0f, (float)pBmp->Width(), (float)pBmp->Height()), 0xFFFFFFFF, 0);
			}
		}
	}

	IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(m_charTypeId);
	if (pChar != NULL) {
		const Bitmap* pBmp = g_pFresh->GetBitmap(pChar->m_icon);
		if (pBmp != NULL) {
			pGI->DrawTexture(pBmp, WRect(124.0f, 344.0f, (float)pBmp->Width(), (float)pBmp->Height()), 0xFFFFFFFF, 0);
		}
	}

	if (m_displaySkin[4] != 0) {
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(m_displaySkin[4]);
		if (pSkin != NULL) {
			const Bitmap* pBmp = g_pFresh->GetBitmap(pSkin->m_cutinImage);
			if (pBmp != NULL) {
				pGI->DrawTexture(pBmp, WRect(124.0f, 344.0f, (float)pBmp->Width(), (float)pBmp->Height()), 0xFFFFFFFF, 0);
			}
		}
	}

	const Bitmap* pLevelBmp = m_pSkinLevelBitmap[(WSingleton<CSharedDoc>::Instance()->m_userInfo.eventFlags >> 1) & 1];
	pGI->DrawTexture(pLevelBmp, WRect(121.0f, 406.0f, (float)pLevelBmp->Width(), (float)pLevelBmp->Height()), 0xFFFFFFFF, 0);

	const char* levelName;
	if (m_displaySkin[5] != 0) {
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(m_displaySkin[5]);
		if (pSkin != NULL) {
			levelName = pSkin->m_icon;
		} else {
			levelName = MakeStr("level_%03d", WSingleton<CSharedDoc>::Instance()->m_myLevel + 1);
		}
	} else {
		levelName = MakeStr("level_%03d", WSingleton<CSharedDoc>::Instance()->m_myLevel + 1);
	}
	const Bitmap* pBmp = g_pFresh->GetBitmap(levelName);

	if (pBmp != NULL) {
		pGI->DrawTexture(pBmp, WRect(206.0f - (float)(int)((float)pBmp->Width() * 0.5f), 386.0f - (float)(int)((float)pBmp->Height() * 0.5f), (float)pBmp->Width(), (float)pBmp->Height()), 0xFFFFFFFF, 0);
	}

	pGI->DrawTexture(m_pSkinNameBitmap, WRect((float)(int)(179.5f - (float)m_pSkinNameBitmap->Width() * 0.5f), 313.0f, (float)m_pSkinNameBitmap->Width(), (float)m_pSkinNameBitmap->Height()), 0xFFFFFFFF, 0);

	pGI->SetTextColor(0xFFFFFFFF, 0xFF808080);
	pGI->SetTextStyle(2);
	g_pFresh->GetManager()->PrintText(WPoint(139.0f, 415.0f), 0, WSingleton<CSharedDoc>::Instance()->m_userInfo.nickname, -1.0f, 0xFFFFFFFF);
}

// realmyroommainui.cpp:3927
void CRealMyRoomMain::OnPetOwnerDraw(PG_MSGARG param) {
	if (m_mainTab == 4) {
		switch (m_roomMode) {
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
		if (m_roomMode == 27) {
			FrListItem* pItem = m_pCardPackListBox->GetSelected();
			if (pItem != NULL) {
				sCards* pCards = (sCards*)pItem->pData;
				if (pCards != NULL && m_pPetExhibition != NULL) {
					if ((pCards->m_typeId & 0x3C00000) == 0x1000000 && (int)pCards->m_count < 10) {
						WBone* pRoot = m_pPetExhibition->GetModel()->GetRootBone();
						WBone* pBone = pRoot->FindBone("card_pack_Dummy", 0);
						if (pBone != NULL) {
							pBone->SetInvisible(false, true);
						}

						for (int i = 1; i < (int)(10 - pCards->m_count); i++) {
							pBone = pRoot->FindBone(MakeStr("card_pack%02d_Dummy", i), 0);
							if (pBone != NULL) {
								pBone->SetInvisible(false, true);
							}
						}
					}
				}
			}
		}

		if (!m_petLoader.IsWorking(0)) {
			if (!m_bongdariShop[2] && WSingleton<CRankingInfo>::Instance()->GetDlg() == NULL && USERINFODLG() == NULL) {
				m_petLoader.Enter();
				if (m_pPetExhibition != NULL) {
					m_pPetExhibition->Display(0.0f, 0.0f);
				}
				m_petLoader.Leave();
			}
		}
	}
}

// realmyroommainui.cpp:3985
void CRealMyRoomMain::OnFurnitureArrangeInit(PG_MSGARG param) {
	m_pFurnitureArrangeButton = DYNAMIC_CAST(FrButton, param);
	if (m_pFurnitureArrangeButton) {
		m_pFurnitureArrangeButton->SetVisible(false);
	}
}

// realmyroommainui.cpp:3994
void CRealMyRoomMain::OnFurnitureArrangeButtonUP(void) {
	if (!m_flags[3]) {
		ControlUiHideMode(4);
	} else {
		ControlUiHideMode(2);
	}
}

// realmyroommainui.cpp:4003
void CRealMyRoomMain::OnItemDescBgInit2(PG_MSGARG param) {
	m_pItemDescBg2 = DYNAMIC_CAST(FrViewer, param);
	if (m_pItemDescBg2) {
		m_pItemDescBg2->SetVisible(false);
	}
}

// realmyroommainui.cpp:4013
void CRealMyRoomMain::OnItemDescBgOwnerDraw2(PG_MSGARG param) {
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == NULL) {
		return;
	}

	float w[9];
	float h[9];
	for (int i = 0; i < 9; i++) {
		w[i] = (float)m_pItemDescFrame[i]->Width();
		h[i] = (float)m_pItemDescFrame[i]->Height();
	}

	float nameH = m_pItemNameEdit->GetRect().h;
	int lineHeight = m_pItemDescEdit2->GetLineHeight();
	int lineNum = m_pItemDescEdit2->GetLineNum();

	float textH = (float)(lineNum + 1) * lineHeight;
	float editW = m_pItemDescEdit2->GetRect().w;
	float totalH = textH + nameH;
	float left = m_itemDescEdit2InitPos.x - m_pItemDescBg2->GetRect().w;
	float top = 488.0f - totalH;
	m_pItemNameEdit->SetRect(WRect(left + 10.0f, top + 15.0f, editW, nameH));
	m_pItemDescEdit2->SetRect(WRect(left + 10.0f, top + 45.0f, editW, textH));

	float frameW = editW + 10.0f;

	pGI->DrawTexture(m_pItemDescFrame[0], WRect(left, top, w[0], h[0]), 0xFFFFFFFF, 0);
	pGI->DrawTexture(m_pItemDescFrame[1], WRect(w[0] + left, top, frameW - (w[2] + w[0]), h[1]), 0xFFFFFFFF, 0);
	pGI->DrawTexture(m_pItemDescFrame[2], WRect(frameW - w[2] + left, top, w[2], h[2]), 0xFFFFFFFF, 0);
	pGI->DrawTexture(m_pItemDescFrame[3], WRect(left, h[1] + top, w[3], totalH - (h[6] + h[0])), 0xFFFFFFFF, 0);
	pGI->DrawTexture(m_pItemDescFrame[4], WRect(w[3] + left, h[1] + top, frameW - (w[5] + w[3]), totalH - (h[7] + h[1])), 0xFFFFFFFF, 0);
	pGI->DrawTexture(m_pItemDescFrame[5], WRect(frameW - w[5] + left, h[2] + top, w[5], totalH - (h[8] + h[2])), 0xFFFFFFFF, 0);
	pGI->DrawTexture(m_pItemDescFrame[6], WRect(left, totalH - h[6] + top, w[6], h[6]), 0xFFFFFFFF, 0);
	pGI->DrawTexture(m_pItemDescFrame[7], WRect(w[6] + left, totalH - h[7] + top, frameW - (w[8] + w[6]), h[7]), 0xFFFFFFFF, 0);
	pGI->DrawTexture(m_pItemDescFrame[8], WRect(frameW - w[8] + left, totalH - h[8] + top, w[8], h[8]), 0xFFFFFFFF, 0);
}

// realmyroommainui.cpp:4058
void CRealMyRoomMain::OnItemDescBgInit(PG_MSGARG param) {
	m_pItemDescBg = DYNAMIC_CAST(FrArea, param);
}

// realmyroommainui.cpp:4065
void CRealMyRoomMain::OnItemDescBgOwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == NULL) {
		return;
	}

	int lines = m_pItemDescEdit->GetLineNum();
	if (lines <= 0) {
		return;
	}

	float w[6];
	float h[6];
	for (int i = 0; i < 2; i++) {
		w[i] = (float)m_pItemDescBottom[i]->Width();
		h[i] = (float)m_pItemDescBottom[i]->Height();
	}

	float y = m_itemDescPos.y + 320.0f;

	pGI->DrawTexture(m_pItemDescBottom[0], WRect(m_itemDescPos.x + 6.0f, y, w[0], h[0]), 0xFFFFFFFF, 0);

	pGI->DrawTexture(m_pItemDescBottom[1], WRect(m_itemDescPos.x + 262.0f, y, w[1], h[1]), 0xFFFFFFFF, 0);
}

// realmyroommainui.cpp:4091
void CRealMyRoomMain::OnItemDescInit2(PG_MSGARG param) {
	m_pItemDescEdit2 = DYNAMIC_CAST(FrEdit, param);
	if (m_pItemDescEdit2) {
		m_pItemDescEdit2->SetVisible(false);
		m_itemDescEdit2Pos = WPoint(m_pItemDescEdit2->GetRect().x, m_pItemDescEdit2->GetRect().y + m_pItemDescEdit2->GetRect().h);
		m_itemDescEdit2InitPos = m_itemDescEdit2Pos;
	}
}

// realmyroommainui.cpp:4103
void CRealMyRoomMain::OnItemNameInit(PG_MSGARG param) {
	m_pItemNameEdit = DYNAMIC_CAST(FrEdit, param);
	if (m_pItemNameEdit) {
		m_pItemNameEdit->SetVisible(false);
	}
}

// realmyroommainui.cpp:4113
void CRealMyRoomMain::OnItemDescInit(PG_MSGARG param) {
	m_pItemDescEdit = DYNAMIC_CAST(FrEdit, param);
	if (m_pItemDescEdit) {
		m_pItemDescEdit->SetVisible(false);
		m_itemDescEditPos = WPoint(m_pItemDescEdit->GetRect().x, m_pItemDescEdit->GetRect().y + m_pItemDescEdit->GetRect().h);
	}
}

// realmyroommainui.cpp:4123
void CRealMyRoomMain::SetItemDesc(void) {
	m_pItemDescEdit->ClearLine();

	switch (m_roomMode) {
	case 6:
	case 10:
		m_itemDescPos = WPoint(0.0f, 30.0f);
		break;
	case 7:
	case 9:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 23:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
		m_itemDescPos = WPoint(0.0f, 120.0f);
		break;
	default:
		m_itemDescPos = WPoint(0.0f, 0.0f);
		break;
	}
}

// realmyroommainui.cpp:4160
void CRealMyRoomMain::OnItemSlotBoxInit(PG_MSGARG param) {
	m_pItemSlotBox = DYNAMIC_CAST(FrGroupBox, param);
	if (m_pItemSlotBox) {
		m_pItemSlotBox->SetVisible(false);

		if (IsLocalContent((localContentType_t)0x71) == true) {
			CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
			if (pItemStorage != NULL) {
				pItemStorage->AddBackGroundUI(m_pItemSlotBox, false);
			}
		}
	}
}

// realmyroommainui.cpp:4178
void CRealMyRoomMain::OnEquipCancelInit(PG_MSGARG param) {
	m_pEquipCancelButton = DYNAMIC_CAST(FrButton, param);
	m_pEquipCancelButton->SetPushDelay(0.0f);
	m_pEquipCancelButton->EnableHover(true, 0.0f);

	if (IsLocalContent((localContentType_t)0x71) == true) {
		CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
		if (pItemStorage != NULL) {
			pItemStorage->AddBackGroundUI(m_pEquipCancelButton, false);
		}
	}
}

// realmyroommainui.cpp:4194
void CRealMyRoomMain::OnEquipCancelUp(void) {
	if (m_pItemSlotList != NULL) {
		FrListItem* pSelected = m_pItemSlotList->GetSelected();

		if (pSelected != NULL) {
			FrListBox::ITEM_LIST::iterator it;
			for (it = m_pItemSlotList->m_itemList.begin(); it != m_pItemSlotList->m_itemList.end(); ++it) {
				if (*it == pSelected) {
					break;
				}
			}

			for (++it; it != m_pItemSlotList->m_itemList.end(); ++it) {
				unsigned long* pItemId = (unsigned long*)(*it)->pData;
				if (pItemId != NULL) {
					if (*pItemId != 0) {
						m_pItemSlotList->SelectItem(*it, true);
						break;
					}
				}
			}

			if (pSelected->pData != NULL) {
				UnequipItem((unsigned long*)pSelected->pData);
			}
		}
	}

	OnTooltipHoverOff(0);
}

// realmyroommainui.cpp:4256
void CRealMyRoomMain::OnEquipResetInit(PG_MSGARG param) {
	m_pEquipResetButton = DYNAMIC_CAST(FrButton, param);
	m_pEquipResetButton->SetPushDelay(0.0f);
	m_pEquipResetButton->EnableHover(true, 0.0f);

	if (IsLocalContent((localContentType_t)0x71) == true) {
		CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
		if (pItemStorage != NULL) {
			pItemStorage->AddBackGroundUI(m_pEquipResetButton, false);
		}
	}
}

// realmyroommainui.cpp:4272
void CRealMyRoomMain::OnEquipResetUp(void) {
	if (m_pItemSlotList != NULL) {
		for (FrListBox::ITEM_LIST::iterator it = m_pItemSlotList->m_itemList.begin(); it != m_pItemSlotList->m_itemList.end(); ++it) {
			FrListItem* pItem = *it;
			unsigned long* pItemId = (unsigned long*)pItem->pData;
			if (pItemId != NULL) {
				UnequipItem(pItemId);
			}
		}
	}

	OnTooltipHoverOff(0);
}

// realmyroommainui.cpp:4288
void CRealMyRoomMain::OnItemSlotInit(intptr_t param) {
	m_pItemSlotList = DYNAMIC_CAST(FrListBox, param);
}

// realmyroommainui.cpp:4293
void CRealMyRoomMain::OnItemSlotOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem != NULL) {

		FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();

		unsigned long* pItemId = (unsigned long*)pItem->pData;
		if (pItemId != NULL) {

			unsigned long typeId = *pItemId;

			int state = 0;
			if (pItem->selected) {
				state = 2;
			} else if (pItem->underCursor) {
				state = 1;
			}
			if (typeId == 0) {
				state = 0;
			}

			const Bitmap* pFrame = m_pItemSlotBitmap[state];
			if (pFrame != NULL) {
				pGI->DrawTexture(pFrame, WRect(pItem->pos.x, pItem->pos.y, (float)pFrame->Width(), (float)pFrame->Height()), 0xFFFFFFFF, 0);
			}

			IFF_STRUCT::sItem* pIff = ItemManager()->FindItem(typeId);
			if (pIff != NULL) {

				const Bitmap* pIcon = g_pFresh->GetBitmap(pIff->m_icon);
				if (pIcon != NULL) {
					pGI->DrawTexture(pIcon, WRect(pItem->pos.x + 2.0f, pItem->pos.y + 4.0f, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);
				}
			}
		}
	}
}

// realmyroommainui.cpp:4338
void CRealMyRoomMain::OnItemSlotBtnDown(void) {
}

// realmyroommainui.cpp:4345
void CRealMyRoomMain::OnItemSlotDClick(void) {
	if (IsLocalContent((localContentType_t)0x71) && m_itemStorageVisible) {
		return;
	}

	FrListItem* pItem = m_pItemSlotList->GetItemUnderCursor();
	if (pItem == 0) {
		return;
	}

	unsigned long* pItemId = (unsigned long*)pItem->pData;
	if (pItemId == 0) {
		return;
	}

	UnequipItem(pItemId);
}

// realmyroommainui.cpp:4361
void CRealMyRoomMain::OnGiftBgInit(PG_MSGARG param) {
	m_pGiftBgViewer = DYNAMIC_CAST(FrViewer, param);
	if (m_pGiftBgViewer) {
		m_pGiftBgViewer->SetVisible(false);
		m_pGiftBgViewer->SetMouseEvent(false);
	}
}

// realmyroommainui.cpp:4371
void CRealMyRoomMain::OnGiftDescInit(PG_MSGARG param) {
	m_pGiftDescEdit = DYNAMIC_CAST(FrEdit, param);
	if (m_pGiftDescEdit) {
		m_pGiftDescEdit->SetVisible(false);
	}
}

// realmyroommainui.cpp:3293
void CRealMyRoomMain::DrawSelItem(FrGraphicInterface* pGI, unsigned long typeId) {
	WRect rect = m_pSelItemWnd->GetRect();

	const Bitmap* pIcon = GetItemIcon(typeId);
	if (pIcon != NULL) {
		pGI->DrawTexture(pIcon, rect, 0xFFFFFFFF, 0);
	}
}

// realmyroommainui.cpp:4380
void CRealMyRoomMain::SetGiftDescString(const char* pSender, const char* pMessage, const char* pDate, const char* pTime) {
	m_pGiftDescEdit->ClearLine();
	if (pSender == NULL) {
		m_pGiftDescEdit->AddText(MakeStr("\xB5\xB5\xC2\xF8\xC7\xD1 \xBC\xB1\xB9\xB0\xC0\xCC \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", pSender), false, true);
		return;
	}

	m_pGiftDescEdit->AddText(MakeStr("\\c0xffffffff,t\\c\xB9\xDE\xB4\xC2\xC0\xCC\\c0xffffffff,n\\c : %s\xB4\xD4\xB2\xB2\xBC\xAD \xBC\xB1\xB9\xB0\xC0\xBB \xBA\xB8\xB3\xBB\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.", pSender), false, true);

	m_pGiftDescEdit->AddText(MakeStr("\\c0xffffffff,t\\c\xB3\xBB\xBF\xEB\\c0xffffffff,n\\c : %s", pMessage), false, true);

	m_pGiftDescEdit->AddText(MakeStr("\\c0xffffffff,t\\c\xBA\xB8\xB3\xBD\xB3\xAF\xC2\xA5\\c0xffffffff,n\\c : %s", pDate), false, true);
	m_pGiftDescEdit->AddText(MakeStr("\\c0xffffffff,t\\c\xBA\xB8\xB3\xBD\xBD\xC3\xB0\xA3\\c0xffffffff,n\\c : %s", pTime), false, true);
}

// realmyroommainui.cpp:4414
void CRealMyRoomMain::OnStatFrameInit(PG_MSGARG param) {
	m_pStatFrame = DYNAMIC_CAST(FrArea, param);
}

// realmyroommainui.cpp:4422
void CRealMyRoomMain::OnStatTextInit(PG_MSGARG param) {
	m_pStatText = DYNAMIC_CAST(FrArea, param);
}

// realmyroommainui.cpp:4430
void CRealMyRoomMain::OnPowerBarInit(intptr_t param) {
	m_pPowerBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_pPowerBar->SetRange(0, 50, 0);
}

// realmyroommainui.cpp:4436
void CRealMyRoomMain::OnControlBarInit(intptr_t param) {
	m_pControlBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_pControlBar->SetRange(0, 30, 0);
}

// realmyroommainui.cpp:4442
void CRealMyRoomMain::OnImpactBarInit(intptr_t param) {
	m_pImpactBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_pImpactBar->SetRange(0, 30, 0);
}

// realmyroommainui.cpp:4448
void CRealMyRoomMain::OnSpinBarInit(intptr_t param) {
	m_pSpinBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_pSpinBar->SetRange(0, 30, 0);
}

// realmyroommainui.cpp:4454
void CRealMyRoomMain::OnCurveBarInit(intptr_t param) {
	m_pCurveBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_pCurveBar->SetRange(0, 30, 0);
}

// realmyroommainui.cpp:4460
void CRealMyRoomMain::TurnOnItemSelect(void) {
	m_pItemSelectWnd->SetVisible(true);
	m_pSelItemPrevButton->SetVisible(true);
	m_pSelItemNextButton->SetVisible(true);
}

// realmyroommainui.cpp:4467
void CRealMyRoomMain::TurnOffItemSelect(void) {
	m_pItemSelectWnd->SetVisible(false);
	m_pSelItemPrevButton->SetVisible(false);
	m_pSelItemNextButton->SetVisible(false);
}

// realmyroommainui.cpp:4474
void CRealMyRoomMain::TurnOnStat(void) {
	m_pStatWnd->SetVisible(true);
}

// realmyroommainui.cpp:4479
void CRealMyRoomMain::TurnOffStat(void) {
	m_pStatWnd->SetVisible(false);
}

// realmyroommainui.cpp:4484
void CRealMyRoomMain::OnSelFrameInit(PG_MSGARG param) {
	m_pSelFrame = DYNAMIC_CAST(FrArea, param);
}

// realmyroommainui.cpp:4491
void CRealMyRoomMain::OnStatBoxInit(PG_MSGARG param) {
	m_pStatWnd = DYNAMIC_CAST(FrGroupBox, param);
}

// realmyroommainui.cpp:4498
void CRealMyRoomMain::OnSelItemIconInit(PG_MSGARG param) {
	m_pSelItemWnd = DYNAMIC_CAST(FrArea, param);
}

// realmyroommainui.cpp:4505
void CRealMyRoomMain::OnSelItemIconOwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == NULL) {
		return;
	}

	switch (m_roomMode) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 21:
	case 22:
	case 26:
		DrawSelItem(pGI, m_charTypeId);
		DrawCharacterList(pGI, m_charTypeId);
		break;
	case 6:
		DrawSelItem(pGI, m_clubTypeId);
		DrawClubList(pGI, m_clubTypeId);
		break;
	case 7:
		DrawSelItem(pGI, m_aztecTypeId);
		DrawAztecList(pGI, m_aztecTypeId);
		break;
	case 10:
		DrawSelItem(pGI, m_caddieTypeId);
		DrawCaddieList(pGI, m_caddieTypeId);
		break;
	case 12:
		DrawSelItem(pGI, m_mascotTypeId);
		DrawMascotList(pGI, m_mascotTypeId);
		break;
	}
}

// realmyroommainui.cpp:4543
void CRealMyRoomMain::OnSelItemInit(PG_MSGARG param) {
	m_pItemSelectWnd = DYNAMIC_CAST(FrButton, param);
	if (m_pItemSelectWnd) {
		m_pItemSelectWnd->EnableBlink(true);
	}
}

// realmyroommainui.cpp:4558
void CRealMyRoomMain::OnSelItemOwnerDraw(PG_MSGARG param) {
}

// realmyroommainui.cpp:4564
void CRealMyRoomMain::OnSelItemUp(void) {
	m_selItemOn = !m_selItemOn;
}

// realmyroommainui.cpp:4574
void CRealMyRoomMain::OnSelItemPrevInit(PG_MSGARG param) {
	m_pSelItemPrevButton = DYNAMIC_CAST(FrButton, param);
	if (m_pSelItemPrevButton) {
		m_pSelItemPrevButton->SetPushDelay(0.1f);
	}
}

// realmyroommainui.cpp:4583
void CRealMyRoomMain::OnSelItemPrevUp(void) {
	if (m_petLoader.IsWorking(0)) {
		return;
	}

	switch (m_roomMode) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 21:
	case 22:
	case 26: {
		std::map<unsigned int, sCharacterInfo>::reverse_iterator it = WSingleton<CSharedDoc>::Instance()->m_characterMap.rbegin();
		for (; it != WSingleton<CSharedDoc>::Instance()->m_characterMap.rend(); ++it) {
			if (it->second.id == m_charSerial) {
				if (++it != WSingleton<CSharedDoc>::Instance()->m_characterMap.rend()) {
					ChangeCharacter(it->second.id);
					return;
				}
			}
		}
		break;
	}

	case 10: {
		std::map<unsigned int, sCaddieInfo>::reverse_iterator it = WSingleton<CSharedDoc>::Instance()->m_caddieMap.rbegin();
		for (; it != WSingleton<CSharedDoc>::Instance()->m_caddieMap.rend(); ++it) {
			if (it->second.id == m_caddieValueA) {
				if (++it != WSingleton<CSharedDoc>::Instance()->m_caddieMap.rend()) {
					ChangeCaddie(it->second.id);
					return;
				}
			}
		}
		break;
	}

	case 12: {
		std::map<unsigned int, sMascotInfo>::reverse_iterator it = WSingleton<CSharedDoc>::Instance()->m_myMascotMap.rbegin();
		for (; it != WSingleton<CSharedDoc>::Instance()->m_myMascotMap.rend(); ++it) {
			if (it->second.id == m_mascotSerial) {
				if (++it != WSingleton<CSharedDoc>::Instance()->m_myMascotMap.rend()) {
					ChangeMascot(it->second.id);
					return;
				}
			}
		}
		break;
	}

	case 6: {
		std::map<unsigned int, sItemInfo>::reverse_iterator it = WSingleton<CSharedDoc>::Instance()->m_myItemMap.rbegin();
		for (; it != WSingleton<CSharedDoc>::Instance()->m_myItemMap.rend(); ++it) {
			if (it->second.m_serial == m_clubSerial) {
				if (++it != WSingleton<CSharedDoc>::Instance()->m_myItemMap.rend()) {
					ChangeClubset(it->second.m_serial);
					return;
				}
			}
		}
		break;
	}

	case 7: {
		std::list<sItemInfo>::reverse_iterator it = WSingleton<CSharedDoc>::Instance()->m_myGearList.rbegin();
		for (; it != WSingleton<CSharedDoc>::Instance()->m_myGearList.rend(); ++it) {
			if (it->m_typeId == m_aztecTypeId) {
				if (++it != WSingleton<CSharedDoc>::Instance()->m_myGearList.rend()) {
					ChangeAztec(it->m_typeId);
					return;
				}
			}
		}
		break;
	}
	}
}

// realmyroommainui.cpp:4662
void CRealMyRoomMain::OnSelItemNextInit(PG_MSGARG param) {
	m_pSelItemNextButton = DYNAMIC_CAST(FrButton, param);
	if (m_pSelItemNextButton) {
		m_pSelItemNextButton->SetPushDelay(0.1f);
	}
}

// realmyroommainui.cpp:4671
void CRealMyRoomMain::OnSelItemNextUp(void) {
	if (m_petLoader.IsWorking(0)) {
		return;
	}

	switch (m_roomMode) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 21:
	case 22:
	case 26: {
		std::map<unsigned int, sCharacterInfo>::const_iterator it = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin();
		for (; it != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++it) {
			if (it->second.id == m_charSerial) {
				if (++it != WSingleton<CSharedDoc>::Instance()->m_characterMap.end()) {
					ChangeCharacter(it->second.id);
					return;
				}
			}
		}
		break;
	}

	case 10: {
		std::map<unsigned int, sCaddieInfo>::const_iterator it = WSingleton<CSharedDoc>::Instance()->m_caddieMap.begin();
		for (; it != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end(); ++it) {
			if (it->second.id == m_caddieValueA) {
				if (++it != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end()) {
					ChangeCaddie(it->second.id);
					return;
				}
			}
		}
		break;
	}

	case 12: {
		std::map<unsigned int, sMascotInfo>::const_iterator it = WSingleton<CSharedDoc>::Instance()->m_myMascotMap.begin();
		for (; it != WSingleton<CSharedDoc>::Instance()->m_myMascotMap.end(); ++it) {
			if (it->second.id == m_mascotSerial) {
				if (++it != WSingleton<CSharedDoc>::Instance()->m_myMascotMap.end()) {
					ChangeMascot(it->second.id);
					return;
				}
			}
		}
		break;
	}

	case 6: {
		std::map<unsigned int, sItemInfo>::const_iterator it = WSingleton<CSharedDoc>::Instance()->m_myItemMap.begin();
		for (; it != WSingleton<CSharedDoc>::Instance()->m_myItemMap.end(); ++it) {
			if (it->second.m_serial == m_clubSerial) {
				if (++it != WSingleton<CSharedDoc>::Instance()->m_myItemMap.end()) {
					ChangeClubset(it->second.m_serial);
					return;
				}
			}
		}
		break;
	}

	case 7: {
		std::list<sItemInfo>::const_iterator it = WSingleton<CSharedDoc>::Instance()->m_myGearList.begin();
		for (; it != WSingleton<CSharedDoc>::Instance()->m_myGearList.end(); ++it) {
			if (it->m_typeId == m_aztecTypeId) {
				if (++it != WSingleton<CSharedDoc>::Instance()->m_myGearList.end()) {
					ChangeAztec(it->m_typeId);
					return;
				}
			}
		}
		break;
	}
	}
}

// realmyroommainui.cpp:4750
void CRealMyRoomMain::OnStatPartInit(PG_MSGARG param) {
	m_pItemStatusWnd = DYNAMIC_CAST(FrViewer, param);
}

// realmyroommainui.cpp:4757
void CRealMyRoomMain::OnStatPartUp(void) {
	ChangeStatTab(STAT_TAB_PART);
}

// realmyroommainui.cpp:4762
void CRealMyRoomMain::OnStatPartOwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == NULL) {
		return;
	}

	int state = 0;
	WRect rect = m_pItemStatusWnd->GetRect();
	if (m_pItemStatusWnd->IsVisible()) {
		if (m_statTab == STAT_TAB_PART) {
			state = 2;
		} else {
			const WPoint& pt = g_pFresh->GetManager()->GetMousePos();
			if (rect.IsInRect(pt)) {
				state = 1;
			}
		}

		if (m_statSource < 3) {
			pGI->DrawTexture(m_pStatPartBitmap[m_statSource][state], rect, 0xFFFFFFFF, 0);
		}
	}
}

// realmyroommainui.cpp:4787
void CRealMyRoomMain::OnStatAllInit(PG_MSGARG param) {
	m_pStatAllViewer = DYNAMIC_CAST(FrViewer, param);
}

// realmyroommainui.cpp:4794
void CRealMyRoomMain::OnStatAllUp(void) {
	ChangeStatTab(STAT_TAB_ALL);
}

// realmyroommainui.cpp:4799
void CRealMyRoomMain::OnStatAllOwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == NULL) {
		return;
	}

	int state = 0;
	WRect rect = m_pStatAllViewer->GetRect();
	if (m_pStatAllViewer->IsVisible()) {
		if (m_statTab == STAT_TAB_ALL) {
			state = 2;
		} else {
			const WPoint& pt = g_pFresh->GetManager()->GetMousePos();
			if (rect.IsInRect(pt)) {
				state = 1;
			}
		}

		pGI->DrawTexture(m_pStatAllBitmap[state], rect, 0xFFFFFFFF, 0);
	}
}

// realmyroommainui.cpp:4822
void CRealMyRoomMain::OnTabMainInit(PG_MSGARG param) {
	m_pMainTab = (FrTabButton*)((__rtti_obj = (void*)param) != 0 ? (((IObject*)__rtti_obj)->IsKindOf(&FrTabButton::m_RTTI) ? (void*)param : 0) : 0);
	if (m_pMainTab) {
		// The normal, down and disabled images of each tab.
		const char* names[3] = { NULL };

		for (int i = 0; i < 5; i++) {
			names[0] = MakeStr("stab_%d_n", i + 1); names[1] = MakeStr("stab_%d_d", i + 1); names[2] = MakeStr("stab_%d_d", i + 1);
			m_pMainTab->AddTab(names, i, NULL);
		}

		names[0] = MakeStr("stab_%d_n", 7); names[1] = MakeStr("stab_%d_d", 7); names[2] = MakeStr("stab_%d_d", 7);
		m_pMainTab->AddTab(names, 6, NULL);

		names[0] = MakeStr("stab_%d_n", 6); names[1] = MakeStr("stab_%d_d", 6); names[2] = MakeStr("stab_%d_d", 6);
		m_pMainTab->AddTab(names, 5, NULL);
	}
}

// realmyroommainui.cpp:4847
void CRealMyRoomMain::OnTabMainButtonUp(void) {
	TButtonItem* pSelected = m_pMainTab->GetSelected();

	if (pSelected) {
		mainTab_t tab = (mainTab_t)pSelected->index;

		if (m_petLoader.IsWorking(0)) {
			m_pMainTab->Select(m_mainTab);
			return;
		}

		ChangeMainTab(tab, false);

		switch (tab) {
		case 0:
			ChangeSubTab((subTab_t)0x15, false);
			ChangeCharacter(WSingleton<CSharedDoc>::Instance()->m_myCharacterId);
			break;
		case 1:
			ChangeSubTab((subTab_t)6, false);
			break;
		case 2:
			ChangeSubTab((subTab_t)0xA, false);
			ChangeCaddie(WSingleton<CSharedDoc>::Instance()->m_myCaddieId);
			break;
		case 3:
			ChangeSubTab((subTab_t)0xC, false);
			ChangeMascot(WSingleton<CSharedDoc>::Instance()->m_myMascotId);
			break;
		case 4:
			ChangeSubTab((subTab_t)0xD, false);
			TurnOffItemStatus();
			break;
		case 5:
			if (IsLocalContent((localContentType_t)0x60)) {
				ChangeSubTab((subTab_t)0x12, false);
			} else {
				ChangeSubTab((subTab_t)0x11, false);
			}

			TurnOffItemStatus();
			m_petLoader.WaitUntilWorkDone(9999); if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = 0; }
			break;
		case 6:
			ChangeSubTab((subTab_t)0x20, false);
			TurnOffItemStatus();
			break;
		}

		if (tab == 6) {
			m_pFurnitureArrangeButton->SetVisible(true);
		} else {
			m_pFurnitureArrangeButton->SetVisible(false);
		}

		Net_EquipDresserParts();
	}
}

// realmyroommainui.cpp:4924
void CRealMyRoomMain::OnTabSubInit(PG_MSGARG param) {
	m_pSubTab = DYNAMIC_CAST(FrTabButton, param);
}

// realmyroommainui.cpp:4932
void CRealMyRoomMain::OnTabSubButtonUp(void) {
	TButtonItem* pItem = m_pSubTab->GetSelected();
	if (pItem == NULL) {
		return;
	}

	ChangeSubTab(static_cast<subTab_t>(pItem->index), false);
}

// realmyroommainui.cpp:4943
void CRealMyRoomMain::OnGiftPageOwnerDraw(PG_MSGARG param) {
	if (m_roomMode != 0x11) {
		return;
	}

	for (int i = 0; i < 5; i++) {
		if (m_pGiftPageBitmap[i] == NULL) {
			return;
		}
	}

	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();

	if (pGI == NULL) {
		return;
	}

	unsigned int firstPage = m_giftPageGroup * 5;
	unsigned int shown = 0;

	if (IsLocalContent((localContentType_t)0x5B)) {
		m_giftPageCount = WSingleton<CSharedDoc>::Instance()->m_value5056;
		shown = WSingleton<CSharedDoc>::Instance()->m_value5055;

		if (firstPage < m_giftPageCount) {
			if (m_giftPageCount - firstPage >= 5) {
				shown = 5;
			} else {
				shown = m_giftPageCount - firstPage;
			}
		} else if (firstPage == m_giftPageCount) {
			shown = 1;
		} else if (firstPage > m_giftPageCount) {
			shown = m_giftPageCount % 5;
		}
	} else {
		unsigned long perPage = WSingleton<CSharedDoc>::Instance()->m_value5044;
		unsigned long total = WSingleton<CSharedDoc>::Instance()->m_value504C;

		if (perPage != 0 && total != 0) {
			m_giftPageCount = total / perPage;
			if (m_giftPageCount * perPage < total) {
				m_giftPageCount++;
			}
		}

		if (firstPage < m_giftPageCount) {
			if (m_giftPageCount - firstPage >= 5) {
				shown = 5;
			} else {
				shown = m_giftPageCount - firstPage;
			}
		} else if (firstPage == m_giftPageCount) {
			shown = 1;
		} else if (firstPage > m_giftPageCount) {
			shown = m_giftPageCount % 5;
		}
	}

	if (shown == 1) {
		if (m_giftPageGroup == 0) {
			return;
		}
	}

	m_giftPageShownCount = shown;

	WRect rect(0.0f, 0.0f, 0.0f, 0.0f);
	float x = 577.0f - (float)((shown + 2) / 2 * m_pGiftPageBitmap[2]->Width());
	if (shown % 2) {
		x -= (float)(m_pGiftPageBitmap[2]->Width() / 2);
	}
	x -= (float)m_pGiftPageBitmap[1]->Width();

	pGI->DrawTexture(m_pGiftPageBitmap[1], WRect(x, 517.0f, (float)m_pGiftPageBitmap[1]->Width(), (float)m_pGiftPageBitmap[1]->Height()), 0xFFFFFFFF, 0);
	x += (float)m_pGiftPageBitmap[1]->Width();

	rect.x = x;
	rect.y = 517.0f;
	rect.w = (float)m_pGiftPageBitmap[2]->Width();
	rect.h = (float)m_pGiftPageBitmap[2]->Height();
	pGI->DrawTexture(m_pGiftPageBitmap[2], rect, 0xFFFFFFFF, 0);

	rect.x = rect.x + (float)(m_pGiftPageBitmap[2]->Width() / 2) - (float)(m_pGiftPageBitmap[0]->Width() / 2);
	rect.y = rect.y + (float)(m_pGiftPageBitmap[2]->Height() / 2) - (float)(m_pGiftPageBitmap[0]->Height() / 2);
	rect.w = (float)m_pGiftPageBitmap[0]->Width();
	rect.h = (float)m_pGiftPageBitmap[0]->Height();

	rect.y -= 0.5f;
	m_areaRects[0] = rect;
	if (m_giftPageGroup > 0) {
		pGI->DrawTexture(m_pGiftPageBitmap[0], rect, 0xFFFFFFFF, 0);
	} else {
		pGI->DrawTexture(m_pGiftPageBitmap[0], rect, 0xFF999999, 0);
	}

	x += (float)m_pGiftPageBitmap[2]->Width();

	unsigned int i;
	unsigned int areaIndex = 1;
	for (i = 0; i < shown; i++) {
		rect.x = x;
		rect.y = 517.0f;
		rect.w = (float)m_pGiftPageBitmap[2]->Width();
		rect.h = (float)m_pGiftPageBitmap[2]->Height();
		pGI->DrawTexture(m_pGiftPageBitmap[2], rect, 0xFFFFFFFF, 0);

		rect.x += (float)(m_pGiftPageBitmap[2]->Width() / 2);
		rect.y += 3.0f;

		m_areaRects[areaIndex] = rect;
		m_areaRects[areaIndex].x -= 5.0f;
		m_areaRects[areaIndex].y -= 5.0f;
		areaIndex++;

		pGI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);

		if (m_giftPageShownCount < m_giftPageCurrent) {
			m_giftPageCurrent = m_giftPageShownCount;
			if (IsLocalContent((localContentType_t)0x5B)) {
				WSingleton<CSharedDoc>::Instance()->m_value5055 = (unsigned char)m_giftPageCurrent;
			} else {
				WSingleton<CSharedDoc>::Instance()->m_value5050 = (unsigned char)m_giftPageCurrent;
			}
		}

		if (i + 1 == m_giftPageCurrent) {
			pGI->SetTextStyle(1);
		} else {
			pGI->SetTextColor(0xFF99CCFF, 0xFFFFFFFF);
		}

		pGI->Print(WPoint(rect.x, rect.y), 1, "%d", m_giftPageGroup * 5 + i + 1);

		pGI->SetTextStyle(0);

		x += (float)m_pGiftPageBitmap[2]->Width();
	}

	rect.x = x;
	rect.y = 517.0f;
	rect.w = (float)m_pGiftPageBitmap[2]->Width();
	rect.h = (float)m_pGiftPageBitmap[2]->Height();
	pGI->DrawTexture(m_pGiftPageBitmap[2], WRect(x, 517.0f, (float)m_pGiftPageBitmap[2]->Width(), (float)m_pGiftPageBitmap[2]->Height()), 0xFFFFFFFF, 0);

	rect.x = rect.x + (float)(m_pGiftPageBitmap[2]->Width() / 2) - (float)(m_pGiftPageBitmap[4]->Width() / 2);
	rect.y = rect.y + (float)(m_pGiftPageBitmap[2]->Height() / 2) - (float)(m_pGiftPageBitmap[4]->Height() / 2);
	rect.w = (float)m_pGiftPageBitmap[4]->Width();
	rect.h = (float)m_pGiftPageBitmap[4]->Height();

	rect.y -= 0.5f;
	m_areaRects[areaIndex] = rect;

	if (m_giftPageGroup * 5 < m_giftPageCount) {
		if (m_giftPageCount - m_giftPageGroup * 5 > 5) {
			pGI->DrawTexture(m_pGiftPageBitmap[4], rect, 0xFFFFFFFF, 0);
		} else {
			pGI->DrawTexture(m_pGiftPageBitmap[4], rect, 0xFF999999, 0);
		}
	} else {
		pGI->DrawTexture(m_pGiftPageBitmap[4], rect, 0xFF999999, 0);
	}

	pGI->DrawTexture(m_pGiftPageBitmap[3], WRect(x + (float)m_pGiftPageBitmap[2]->Width(), 517.0f, (float)m_pGiftPageBitmap[3]->Width(), (float)m_pGiftPageBitmap[3]->Height()), 0xFFFFFFFF, 0);

	if (IsLocalContent((localContentType_t)0x5B)) {
		if (WSingleton<CSharedDoc>::Instance()->m_value5054 == 1) {
			WSingleton<CSharedDoc>::Instance()->m_value5054 = 0;
			BuildDisplayStand((subTab_t)0x11);
		}
	} else {
		if (WSingleton<CSharedDoc>::Instance()->m_value5054 == 1) {
			WSingleton<CSharedDoc>::Instance()->m_value5054 = 0;
			BuildDisplayStand((subTab_t)0x11);
		}
	}

	float mouseY = g_input->GetMousePoint().y, mouseX = g_input->GetMousePoint().x;
	bool hover = false;
	int area = 0;

	if (m_areaRects[0].IsInRect(mouseX, mouseY)) {
		area = 1;
		hover = true;
	}

	for (unsigned int j = 0; j < m_giftPageShownCount; j++) {
		const WRect& rc = m_areaRects[j + 1];
		if (mouseX >= rc.x && mouseX <= rc.x + rc.w && mouseY >= rc.y && mouseY <= rc.h + rc.y) {
			hover = true;
		}
		area++;
	}

	if (m_areaRects[area + 1].IsInRect(mouseX, mouseY)) {
		hover = true;
	}

	if (hover) {
		WSingleton<CMouseCursor>::Instance()->SetCursor(1);
	}
}

// realmyroommainui.cpp:5211
void CRealMyRoomMain::OnGiftPageLBDown(void) {
	if (m_roomMode != 0x11) {
		return;
	}

	if (!IsLocalContent((localContentType_t)0x5B)) {
		if (WSingleton<CSharedDoc>::Instance()->m_value504C <= WSingleton<CSharedDoc>::Instance()->m_value5044) {
			return;
		}
	}

	static DWORD s_lastClickTime = GetTickCount();

	if (GetTickCount() - s_lastClickTime > 350) {
		s_lastClickTime = GetTickCount();

		const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
		unsigned long page = 0;
		int area = 0;

		if (m_areaRects[0].IsInRect(mouse)) {
			if (m_giftPageGroup > 0) {
				m_giftPageGroup--;
				m_giftPageCurrent = 1;
				page = m_giftPageGroup * 5 + 1;
			}
			area = 1;
		}

		for (unsigned int i = 0; i < m_giftPageShownCount; i++) {
			const WRect& rc = m_areaRects[i + 1];
			if (mouse.x >= rc.x && rc.x + rc.w >= mouse.x && mouse.y >= rc.y && rc.y + rc.Size().h >= mouse.y) {
				if (m_giftPageCurrent != i + 1) {
					page = m_giftPageGroup * 5 + (m_giftPageCurrent = i + 1);
				}
			}
			area++;
		}

		const WRect& next = m_areaRects[(unsigned short)area + 1];
		if (mouse.x >= next.x && next.x + next.w >= mouse.x && mouse.y >= next.y && next.y + next.h >= mouse.y) {
			if (m_giftPageGroup * 5 < m_giftPageCount) {
				if (m_giftPageCount - m_giftPageGroup * 5 > 5) {
					m_giftPageGroup++;
					m_giftPageCurrent = 1;
					page = m_giftPageGroup * 5 + 1;
				}
			}
		}

		if (page != 0) {
			WSendPacket packet(GCT_GIFT_LIST_PAGE);
			packet.Encode4(page);
			packet.Send(SEND_TO_DEFAULT);
		}
	}

	if (!IsLocalContent((localContentType_t)0x5B)) {
		WSingleton<CSharedDoc>::Instance()->m_value5050 = m_giftPageCurrent;
	}
}

// realmyroommainui.cpp:5301
void CRealMyRoomMain::OnCardBookInit(PG_MSGARG param) {
	FrArea* p = DYNAMIC_CAST(FrArea, param);

	if (p != NULL && m_pCardBook != NULL) {
		m_pCardBook->BoundControl(m_pCardBook->m_pBookWnd, p);
	}
}

// realmyroommainui.cpp:5311
void CRealMyRoomMain::OnCardBookOwnerDraw(PG_MSGARG param) {
	if (m_pCardBook) {
		m_pCardBook->Draw();
	}
}

// realmyroommainui.cpp:5319
void CRealMyRoomMain::SetCardBookVisible(bool bVisible) {
	if (m_pCardBook) {
		m_pCardBook->Show(bVisible);
		m_cardBookVisible = bVisible;
	}

	if (m_pCardPackListBox) {
		m_pCardPackListBox->SetVisible(bVisible);
		m_pCardPackListBox->UnselectAllItem();
		if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = NULL; }
	}
}

// realmyroommainui.cpp:5335
void CRealMyRoomMain::BuildCardPackList(void) {
	if (m_pCardPackListBox) {
		m_pCardPackListBox->ClearItem();

		std::list<sCards>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myCardList.begin();
		while (it != WSingleton<CSharedDoc>::Instance()->m_myCardList.end()) {
			sCards* pCards = &(*it);

			// Card packs only (item groups 3 and 4).
			if (((pCards->m_typeId >> 22) & 0xF) == 4 || ((pCards->m_typeId >> 22) & 0xF) == 3) {
				m_pCardPackListBox->AddItem(pCards);
			}
			++it;
		}

		if (WSingleton<CSharedDoc>::Instance()->m_myCardList.size() % 2) {
			m_pCardPackListBox->AddItem(NULL);
		}

		int count = 6 - m_pCardPackListBox->GetCurrentItemSize(true);
		for (int i = 0; i < count; i++) {
			m_pCardPackListBox->AddItem(NULL);
		}

		m_pCardPackListBox->UseRightButton(true);
		m_pCardPackListBox->UseDummy(true);
		if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = NULL; }
	}
}

// realmyroommainui.cpp:5374
void CRealMyRoomMain::OnCardPackListInit(PG_MSGARG param) {
	m_pCardPackListBox = DYNAMIC_CAST(FrListBox, param);
	BuildCardPackList();

	if (m_pCardPackListBox) {
		m_pCardPackListBox->SetVisible(false);
	}
}

// realmyroommainui.cpp:5385
void CRealMyRoomMain::OnCardPackListDraw(PG_MSGARG param) {
	if (m_pCardPackListBox == NULL || m_roomMode != 27) {
		return;
	}

	FrListItem* pItem = (FrListItem*)param;
	if (pItem == NULL) {
		return;
	}

	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	pGI->SetTextStyle(0);
	const Bitmap* pFrame;
	if (pItem->pData == NULL) {
		pFrame = m_cardPackItemBmp[3];
	} else if (pItem->selected) {
		pFrame = m_cardPackItemBmp[2];
		pGI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	} else if (pItem->underCursor) {
		pFrame = m_cardPackItemBmp[1];
		pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	} else {
		pFrame = m_cardPackItemBmp[0];
		pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	}

	if (pFrame != NULL) {
		pGI->DrawTexture(pFrame, WRect(0.0f, 0.0f, (float)pFrame->Width(), (float)pFrame->Height()),
			WRect(pItem->pos.x, pItem->pos.y, (float)m_pCardPackListBox->GetItemWidth(), (float)m_pCardPackListBox->GetItemHeight()), 0xFFFFFFFF, 0);
	}

	sCards* pCards = (sCards*)pItem->pData;
	if (pCards == NULL) {
		return;
	}

	const char* name = "\xBE\xCB \xBC\xF6 \xBE\xF8\xB4\xC2 \xBE\xC6\xC0\xCC\xC5\xDB";
	const char* icon = "caddie_no";
	IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(pCards->m_typeId);
	if (pCard != NULL) {
		name = pCard->m_name;
		icon = pCard->m_icon;
	}

	const Bitmap* pIcon = g_pFresh->GetBitmap(icon);

	float x = (float)m_pCardPackListBox->GetItemWidth();
	if (pIcon != NULL) {
		pGI->DrawTexture(pIcon, WRect(pItem->pos.x + 12.0f, pItem->pos.y + 12.0f, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);
	}

	pGI->Print11(WPoint((x -= 12.0f) + pItem->pos.x, pItem->pos.y + 12.0f), 2, "%s", name);

	float fontHeight = (float)pGI->GetFontHeight11();
	if ((pCards->m_typeId & 0x3C00000) == 0x1000000) {
		pGI->Print11(WPoint(x, fontHeight + 17.0f) + pItem->pos, 2, MakeStr("\xB3\xB2\xC0\xBA \xC4\xAB\xB5\xE5 \xC6\xD1 %d\xB0\xB3", pCards->m_count));
	} else {
		pGI->Print11(WPoint(x, fontHeight + 17.0f) + pItem->pos, 2, MakeStr("%d\xB0\xB3", pCards->m_count));
	}

	if (m_pCardBook != NULL && pItem == m_pCardPackListBox->GetSelected()) {
		m_pCardBook->DrawCardPackHelp(pCards->m_typeId);
	}
}

// realmyroommainui.cpp:5459
void CRealMyRoomMain::OnCardPackListDown(void) {
	if (m_pCardPackListBox && m_roomMode == 27) {
		FrListItem* pItem = m_pCardPackListBox->GetItemUnderCursor();
		if (pItem == NULL || pItem->pData == NULL) {
			return;
		}

		if (m_petLoader.IsWorking(0)) {
			m_pCardPackListBox->ToggleItem(pItem);
			return;
		}

		if (pItem->selected) {
			g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);

			sItemInfo* pInfo = (sItemInfo*)pItem->pData;
			if (m_pCardBook) {
				m_pCardBook->m_pPageWnd->SetVisible(true);
			}

			ShowPet(pInfo->m_typeId);
			return;
		}

		m_pCardPackListBox->SelectItem(pItem, true);
	}
}

// realmyroommainui.cpp:5494
void CRealMyRoomMain::OnCardPackListRbUp(void) {
	if (m_pCardPackListBox && m_roomMode == 27) {
		FrListItem* pItem = m_pCardPackListBox->GetItemUnderCursor();
		if (pItem == NULL || pItem->pData == NULL) {
			return;
		}

		if (m_petLoader.IsWorking(0)) {
			m_pCardPackListBox->ToggleItem(pItem);
			return;
		}

		if (pItem->selected) {
			OnCardPackListDown();
		} else {
			m_pCardPackListBox->ToggleItem(pItem);
		}

		sItemInfo* pInfo = (sItemInfo*)pItem->pData;
		OpenInformation(&pInfo->m_typeId, NULL, NULL);
	}
}

// realmyroommainui.cpp:5523
void CRealMyRoomMain::OnCardBookIndexInit(PG_MSGARG param) {
	FrListBox* p = DYNAMIC_CAST(FrListBox, param);

	if (p != NULL && m_pCardBook != NULL) {
		m_pCardBook->BoundControl(m_pCardBook->m_pSubWnd, p);
	}
}

// realmyroommainui.cpp:5533
void CRealMyRoomMain::OnCardBookIndexLBDown(void) {
	if (m_pCardBook) {
		const WPoint& pt = g_pFresh->GetManager()->GetMousePos();
		m_pCardBook->PageIndexBehavior(pt, 1);
	}
}

// realmyroommainui.cpp:5543
void CRealMyRoomMain::OnCardBookIndexMouseOver(void) {
	if (m_pCardBook) {
		const WPoint& pt = g_pFresh->GetManager()->GetMousePos();
		m_pCardBook->PageIndexBehavior(pt, 2);
	}
}

// realmyroommainui.cpp:5553
void CRealMyRoomMain::OnCardUseInit(PG_MSGARG param) {
	FrButton* p = DYNAMIC_CAST(FrButton, param);

	if (p != NULL) {
		if (m_pCardBook != NULL) {
			m_pCardBook->BoundControl(m_pCardBook->m_pPageWnd, p);
		}
	}
}

// realmyroommainui.cpp:5566
void CRealMyRoomMain::OnCardUseLBDown(void) {
	if (m_pCardPackListBox->IsVisible()) {
		FrListItem* pItem = m_pCardPackListBox->GetSelected();

		if (pItem != NULL) {
			sCards* pCards = (sCards*)pItem->pData;

			if (pCards != NULL) {
				if (FindFrForm<FrCardBoxDlg>("FrCardBoxDlg") == NULL) {
					FrCardBoxDlg* pDlg = CreateForm<FrCardBoxDlg>(g_pFresh->GetManager(), this, "FrCardBoxDlg", 0);
					pDlg->Init(pCards->m_typeId, pCards->m_guid, pCards->m_count);
					pDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnCardBoxDlgResult, 1);

					if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = NULL; }
				}
			}
		}
	} else {
		if (m_pCardBook != NULL) {
			m_pCardBook->UseCard();
		}
	}
}

// realmyroommainui.cpp:5594
bool CRealMyRoomMain::OnCardBoxDlgResult(int result, FrForm* pForm) {
	return true;
}

// realmyroommainui.cpp:5604
void CRealMyRoomMain::OnCardPropertyInit(PG_MSGARG param) {
	FrArea* p = DYNAMIC_CAST(FrArea, param);

	if (p != NULL && m_pCardBook != NULL) {
		m_pCardBook->BoundControl(m_pCardBook->m_pPropertyWnd, p);
	}
}

// realmyroommainui.cpp:5614
void CRealMyRoomMain::OnTabCardPackInit(PG_MSGARG param) {
	FrButton* pButton = DYNAMIC_CAST(FrButton, param);

	if (pButton != NULL && m_pCardBook != NULL) {
		m_pCardBook->BoundControl(m_pCardBook->m_pTabButtons[0], pButton);
		pButton->SetPushDelay(0.0f);
	}
}

// realmyroommainui.cpp:5625
void CRealMyRoomMain::OnTabCardPackLBDown(void) {
	if (m_pCardBook) {
		m_pCardBook->ButtonTabControl(0);
		m_pCardBook->ShowSlotControl(true);
	}

	if (m_pCardPackListBox) {
		m_pCardPackListBox->SetVisible(true);
		m_pCardPackListBox->UnselectAllItem();
	}
}

// realmyroommainui.cpp:5641
void CRealMyRoomMain::OnTabCardVolume1Init(PG_MSGARG param) {
	FrButton* pButton = DYNAMIC_CAST(FrButton, param);

	if (pButton != NULL && m_pCardBook != NULL) {
		m_pCardBook->BoundControl(m_pCardBook->m_pTabButtons[1], pButton);
		pButton->SetPushDelay(0.0f);
	}
}

// realmyroommainui.cpp:5653
void CRealMyRoomMain::OnTabCardVolume1LBDown(void) {
	if (m_pCardBook) {
		m_pCardBook->ButtonTabControl(1);
		m_pCardBook->ShowSlotControl(true);
	}

	if (m_pCardPackListBox) {
		m_pCardPackListBox->SetVisible(false);
	}

	if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = NULL; }
}

// realmyroommainui.cpp:5667
void CRealMyRoomMain::OnTabCardVolume2Init(PG_MSGARG param) {
	FrButton* pButton = DYNAMIC_CAST(FrButton, param);

	if (pButton != NULL && m_pCardBook != NULL) {
		m_pCardBook->BoundControl(m_pCardBook->m_pTabButtons[2], pButton);
		pButton->SetPushDelay(0.0f);
	}
}

// realmyroommainui.cpp:5678
void CRealMyRoomMain::OnTabCardVolume2LBDown(void) {
	if (m_pCardBook) {
		m_pCardBook->ButtonTabControl(2);
		m_pCardBook->ShowSlotControl(true);
	}

	if (m_pCardPackListBox) {
		m_pCardPackListBox->SetVisible(false);
	}

	if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = NULL; }
}

// realmyroommainui.cpp:5692
void CRealMyRoomMain::OnTabCardVolume3Init(PG_MSGARG param) {
	FrButton* pButton = DYNAMIC_CAST(FrButton, param);

	if (pButton != NULL && m_pCardBook != NULL) {
		m_pCardBook->BoundControl(m_pCardBook->m_pTabButtons[3], pButton);
		pButton->SetPushDelay(0.0f);
	}
}

// realmyroommainui.cpp:5703
void CRealMyRoomMain::OnTabCardVolume3LBDown(void) {
	if (m_pCardBook) {
		m_pCardBook->ButtonTabControl(3);
		m_pCardBook->ShowSlotControl(true);
	}

	if (m_pCardPackListBox) {
		m_pCardPackListBox->SetVisible(false);
	}

	if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = NULL; }
}

// realmyroommainui.cpp:5717
void CRealMyRoomMain::OnTabCardVolume4Init(PG_MSGARG param) {
	FrButton* pButton = DYNAMIC_CAST(FrButton, param);

	if (pButton != NULL && m_pCardBook != NULL) {
		m_pCardBook->BoundControl(m_pCardBook->m_pTabButtons[4], pButton);
		pButton->SetPushDelay(0.0f);
	}
}

// realmyroommainui.cpp:5728
void CRealMyRoomMain::OnTabCardVolume4LBDown(void) {
	if (m_pCardBook) {
		m_pCardBook->ButtonTabControl(4);
		m_pCardBook->ShowSlotControl(true);
	}

	if (m_pCardPackListBox) {
		m_pCardPackListBox->SetVisible(false);
	}

	if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = NULL; }
}

// realmyroommainui.cpp:5744
void CRealMyRoomMain::BoundCardPackPage(FrButton* pButton, unsigned char slot) {
	pButton->SetVisible(false);
	pButton->Enable(false);
}

// realmyroommainui.cpp:5749
void CRealMyRoomMain::OnCardPackInit1(PG_MSGARG param) { BoundCardPackPage(DYNAMIC_CAST(FrButton, param), 0); }

// realmyroommainui.cpp:5750
void CRealMyRoomMain::OnCardPackInit2(PG_MSGARG param) { BoundCardPackPage(DYNAMIC_CAST(FrButton, param), 1); }

// realmyroommainui.cpp:5751
void CRealMyRoomMain::OnCardPackInit3(PG_MSGARG param) { BoundCardPackPage(DYNAMIC_CAST(FrButton, param), 2); }

// realmyroommainui.cpp:5752
void CRealMyRoomMain::OnCardPackInit4(PG_MSGARG param) { BoundCardPackPage(DYNAMIC_CAST(FrButton, param), 3); }

// realmyroommainui.cpp:5753
void CRealMyRoomMain::OnCardPackInit5(PG_MSGARG param) { BoundCardPackPage(DYNAMIC_CAST(FrButton, param), 4); }

// realmyroommainui.cpp:5754
void CRealMyRoomMain::OnCardPackInit6(PG_MSGARG param) { BoundCardPackPage(DYNAMIC_CAST(FrButton, param), 5); }

// realmyroommainui.cpp:5755
void CRealMyRoomMain::OnCardPackInit7(PG_MSGARG param) { BoundCardPackPage(DYNAMIC_CAST(FrButton, param), 6); }

// realmyroommainui.cpp:5756
void CRealMyRoomMain::OnCardPackInit8(PG_MSGARG param) { BoundCardPackPage(DYNAMIC_CAST(FrButton, param), 7); }

// realmyroommainui.cpp:5760
void CRealMyRoomMain::CardPackBehavior(unsigned char slot, unsigned char button) {
	if (m_pCardBook) {
		if (!m_itemStorageVisible) {
			CCardPackPage* pPage = m_pCardBook->GetCardPackPage();

			if (pPage) {
				m_pCardBook->SetSelectedSlot(slot);
				pPage->SelectSlot(button, slot);

				if (m_pCardBook->m_pPageWnd) {
					if (pPage->IsSelectedSlot()) {
						m_pCardBook->m_pPageWnd->SetVisible(true);
					} else {
						m_pCardBook->m_pPageWnd->SetVisible(false);
					}
				}
			}
		}
	}
}

// realmyroommainui.cpp:5822
void CRealMyRoomMain::OnCardPackLBDown1(void) { CardPackBehavior(0, 1); }

// realmyroommainui.cpp:5823
void CRealMyRoomMain::OnCardPackLBDown2(void) { CardPackBehavior(1, 1); }

// realmyroommainui.cpp:5824
void CRealMyRoomMain::OnCardPackLBDown3(void) { CardPackBehavior(2, 1); }

// realmyroommainui.cpp:5825
void CRealMyRoomMain::OnCardPackLBDown4(void) { CardPackBehavior(3, 1); }

// realmyroommainui.cpp:5826
void CRealMyRoomMain::OnCardPackLBDown5(void) { CardPackBehavior(4, 1); }

// realmyroommainui.cpp:5827
void CRealMyRoomMain::OnCardPackLBDown6(void) { CardPackBehavior(5, 1); }

// realmyroommainui.cpp:5828
void CRealMyRoomMain::OnCardPackLBDown7(void) { CardPackBehavior(6, 1); }

// realmyroommainui.cpp:5829
void CRealMyRoomMain::OnCardPackLBDown8(void) { CardPackBehavior(7, 1); }

// realmyroommainui.cpp:5831
void CRealMyRoomMain::OnCardPackRBDown1(void) { CardPackBehavior(0, 2); }

// realmyroommainui.cpp:5832
void CRealMyRoomMain::OnCardPackRBDown2(void) { CardPackBehavior(1, 2); }

// realmyroommainui.cpp:5833
void CRealMyRoomMain::OnCardPackRBDown3(void) { CardPackBehavior(2, 2); }

// realmyroommainui.cpp:5834
void CRealMyRoomMain::OnCardPackRBDown4(void) { CardPackBehavior(3, 2); }

// realmyroommainui.cpp:5835
void CRealMyRoomMain::OnCardPackRBDown5(void) { CardPackBehavior(4, 2); }

// realmyroommainui.cpp:5836
void CRealMyRoomMain::OnCardPackRBDown6(void) { CardPackBehavior(5, 2); }

// realmyroommainui.cpp:5837
void CRealMyRoomMain::OnCardPackRBDown7(void) { CardPackBehavior(6, 2); }

// realmyroommainui.cpp:5838
void CRealMyRoomMain::OnCardPackRBDown8(void) { CardPackBehavior(7, 2); }

// realmyroommainui.cpp:5842
void CRealMyRoomMain::BoundCardPage(FrButton* pButton, unsigned char slot) {
	if (pButton && m_pCardBook) {
		CCardPage* pPage = m_pCardBook->GetCardPage();

		if (pPage) {
			pPage->BoundSlotControl(slot, pButton);
		}

		pButton->UseRightButton(true);
		if (IsLocalContent(static_cast<localContentType_t>(0x71)) == true) {
			CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
			if (pItemStorage) {
				pItemStorage->AddBackGroundUI(pButton, false);
			}
		}
	}
}

// realmyroommainui.cpp:5865
void CRealMyRoomMain::OnCardInit1(PG_MSGARG param) { BoundCardPage(DYNAMIC_CAST(FrButton, param), 0); }

// realmyroommainui.cpp:5866
void CRealMyRoomMain::OnCardInit2(PG_MSGARG param) { BoundCardPage(DYNAMIC_CAST(FrButton, param), 1); }

// realmyroommainui.cpp:5867
void CRealMyRoomMain::OnCardInit3(PG_MSGARG param) { BoundCardPage(DYNAMIC_CAST(FrButton, param), 2); }

// realmyroommainui.cpp:5868
void CRealMyRoomMain::OnCardInit4(PG_MSGARG param) { BoundCardPage(DYNAMIC_CAST(FrButton, param), 3); }

// realmyroommainui.cpp:5869
void CRealMyRoomMain::OnCardInit5(PG_MSGARG param) { BoundCardPage(DYNAMIC_CAST(FrButton, param), 4); }

// realmyroommainui.cpp:5870
void CRealMyRoomMain::OnCardInit6(PG_MSGARG param) { BoundCardPage(DYNAMIC_CAST(FrButton, param), 5); }

// realmyroommainui.cpp:5871
void CRealMyRoomMain::OnCardInit7(PG_MSGARG param) { BoundCardPage(DYNAMIC_CAST(FrButton, param), 6); }

// realmyroommainui.cpp:5872
void CRealMyRoomMain::OnCardInit8(PG_MSGARG param) { BoundCardPage(DYNAMIC_CAST(FrButton, param), 7); }

// realmyroommainui.cpp:5873
void CRealMyRoomMain::OnCardInit9(PG_MSGARG param) { BoundCardPage(DYNAMIC_CAST(FrButton, param), 8); }

// realmyroommainui.cpp:5874
void CRealMyRoomMain::OnCardInit10(PG_MSGARG param) { BoundCardPage(DYNAMIC_CAST(FrButton, param), 9); }

// realmyroommainui.cpp:5875
void CRealMyRoomMain::OnCardInit11(PG_MSGARG param) { BoundCardPage(DYNAMIC_CAST(FrButton, param), 10); }

// realmyroommainui.cpp:5876
void CRealMyRoomMain::OnCardInit12(PG_MSGARG param) { BoundCardPage(DYNAMIC_CAST(FrButton, param), 11); }

// realmyroommainui.cpp:5879
void CRealMyRoomMain::CardBehavior(unsigned char slot, unsigned char button) {
	if (m_pCardBook) {
		if (!m_itemStorageVisible) {
			CCardPage* pPage = m_pCardBook->GetCardPage();

			if (pPage) {
				m_pCardBook->SetSelectedSlot(slot);
				pPage->SelectSlot(button, slot);

				if (m_pCardBook->m_pPageWnd) {
					if (pPage->IsSelectedSlot()) {
						if (pPage->GetCardSlot()[slot].GetCards()->m_typeId == 0x7C800026) {
							m_pCardBook->m_pPageWnd->SetVisible(false);
						} else {
							m_pCardBook->m_pPageWnd->SetVisible(true);
						}
					} else {
						m_pCardBook->m_pPageWnd->SetVisible(false);
					}
				}
			}
		}
	}
}

// realmyroommainui.cpp:5941
void CRealMyRoomMain::OnCardLBDown1(void) { CardBehavior(0, 1); }

// realmyroommainui.cpp:5942
void CRealMyRoomMain::OnCardLBDown2(void) { CardBehavior(1, 1); }

// realmyroommainui.cpp:5943
void CRealMyRoomMain::OnCardLBDown3(void) { CardBehavior(2, 1); }

// realmyroommainui.cpp:5944
void CRealMyRoomMain::OnCardLBDown4(void) { CardBehavior(3, 1); }

// realmyroommainui.cpp:5945
void CRealMyRoomMain::OnCardLBDown5(void) { CardBehavior(4, 1); }

// realmyroommainui.cpp:5946
void CRealMyRoomMain::OnCardLBDown6(void) { CardBehavior(5, 1); }

// realmyroommainui.cpp:5947
void CRealMyRoomMain::OnCardLBDown7(void) { CardBehavior(6, 1); }

// realmyroommainui.cpp:5948
void CRealMyRoomMain::OnCardLBDown8(void) { CardBehavior(7, 1); }

// realmyroommainui.cpp:5949
void CRealMyRoomMain::OnCardLBDown9(void) { CardBehavior(8, 1); }

// realmyroommainui.cpp:5950
void CRealMyRoomMain::OnCardLBDown10(void) { CardBehavior(9, 1); }

// realmyroommainui.cpp:5951
void CRealMyRoomMain::OnCardLBDown11(void) { CardBehavior(10, 1); }

// realmyroommainui.cpp:5952
void CRealMyRoomMain::OnCardLBDown12(void) { CardBehavior(11, 1); }

// realmyroommainui.cpp:5954
void CRealMyRoomMain::OnCardRBDown1(void) { CardBehavior(0, 2); }

// realmyroommainui.cpp:5955
void CRealMyRoomMain::OnCardRBDown2(void) { CardBehavior(1, 2); }

// realmyroommainui.cpp:5956
void CRealMyRoomMain::OnCardRBDown3(void) { CardBehavior(2, 2); }

// realmyroommainui.cpp:5957
void CRealMyRoomMain::OnCardRBDown4(void) { CardBehavior(3, 2); }

// realmyroommainui.cpp:5958
void CRealMyRoomMain::OnCardRBDown5(void) { CardBehavior(4, 2); }

// realmyroommainui.cpp:5959
void CRealMyRoomMain::OnCardRBDown6(void) { CardBehavior(5, 2); }

// realmyroommainui.cpp:5960
void CRealMyRoomMain::OnCardRBDown7(void) { CardBehavior(6, 2); }

// realmyroommainui.cpp:5961
void CRealMyRoomMain::OnCardRBDown8(void) { CardBehavior(7, 2); }

// realmyroommainui.cpp:5962
void CRealMyRoomMain::OnCardRBDown9(void) { CardBehavior(8, 2); }

// realmyroommainui.cpp:5963
void CRealMyRoomMain::OnCardRBDown10(void) { CardBehavior(9, 2); }

// realmyroommainui.cpp:5964
void CRealMyRoomMain::OnCardRBDown11(void) { CardBehavior(10, 2); }

// realmyroommainui.cpp:5965
void CRealMyRoomMain::OnCardRBDown12(void) { CardBehavior(11, 2); }

// realmyroommainui.cpp:5972
void CRealMyRoomMain::OnItemListInit(intptr_t param) {
	m_pUccCopyHideWnd = DYNAMIC_CAST(FrListBox, param);
	if (m_pUccCopyHideWnd == NULL) {
		return;
	}

	m_pUccCopyHideWnd->UseRightButton(true);
	m_pUccCopyHideWnd->UseDummy(true);
	m_pUccCopyHideWnd->SetPushSound(NULL);
	m_pUccCopyHideWnd->AddNoRButtonRect(m_itemListArea[0].rect, 0);
	m_pUccCopyHideWnd->AddNoRButtonRect(m_itemListArea[4].rect, 4);
	m_pUccCopyHideWnd->AddNoRButtonRect(m_itemListArea[1].rect, 1);
	m_pUccCopyHideWnd->AddNoRButtonRect(m_itemListArea[2].rect, 2);
	m_pUccCopyHideWnd->AddNoRButtonRect(m_itemListArea[3].rect, 3);
	if (IsLocalContent((localContentType_t)0x4F)) {
		m_pUccCopyHideWnd->AddNoRButtonRect(m_itemListArea[7].rect, 7);
		m_pUccCopyHideWnd->AddNoRButtonRect(m_itemListArea[8].rect, 8);
	}

	if (IsLocalContent((localContentType_t)0x51)) {
		m_pUccCopyHideWnd->AddNoRButtonRect(m_itemListArea[9].rect, 9);
	}

	if (IsLocalContent((localContentType_t)0x56)) {
		m_pUccCopyHideWnd->AddNoRButtonRect(m_itemListArea[10].rect, 10);
	}

	if (IsLocalContent((localContentType_t)0x71) == true) {
		CItemStorage* pStorage = CItemStorage::Instance();
		if (pStorage) {
			pStorage->m_itemListArea = WRect(m_pUccCopyHideWnd->GetRect());
		}
	}
}

// realmyroommainui.cpp:6011
void CRealMyRoomMain::DrawStatus(FrGraphicInterface* pGI, FrListItem* pItem, short* const status, short* const slotStatus) {
	float y = 4.0f;

	for (int i = 0; i < 5; i++) {
		const Bitmap* pPoint = m_pStatPointBitmap[i][0];
		const Bitmap* pSlotPoint = m_pStatPointBitmap[i][1];

		int count = status[i];
		int slotCount = slotStatus[i];

		if (slotCount != 0 || count != 0) {
			float x = 86.0f;
			for (int j = 0; j < count; j++) {
				pGI->DrawTexture(pPoint, WRect(x + pItem->pos.x, y + pItem->pos.y, 5.0, 6.0), 0xFFFFFFFF, 0);
				x -= 6.0f;
			}

			for (int j = 0; j < slotCount; j++) {
				pGI->DrawTexture(pSlotPoint, WRect(x + pItem->pos.x, y + pItem->pos.y, 5.0, 6.0), 0xFFFFFFFF, 0);
				x -= 6.0f;
			}

			y += 6.0f;
		}
	}
}

// realmyroommainui.cpp:6046
void CRealMyRoomMain::DrawStatus2(FrGraphicInterface* pGI, FrListItem* pItem, short* const status, short* const slotStatus, float x) {
	float offsetY = 0.0f;
	for (int i = 0; i < 5; i++) {
		if (status[i] != 0 || slotStatus[i] != 0) {
			pGI->SetTextColor(0xFF0000FF, 0xFFFFFFFF);
			pGI->Print11(WPoint(x + 182.0f, offsetY + x + 36.0f) + pItem->pos, 2, MakeStatusString(i, status[i], slotStatus[i]));
			offsetY += 14.0f;
		}
	}
}

// realmyroommainui.cpp:6077
void CRealMyRoomMain::DrawStatusCard(FrGraphicInterface* pGI, FrListItem* pItem, short* const cardStatus, float x) {
	float offsetY = 0.0f;
	for (int i = 0; i < 5; i++) {
		if (cardStatus[i] != 0) {
			pGI->SetTextColor(0xFF0000FF, 0xFFFFFFFF);
			pGI->Print11(WPoint(x + 182.0f, offsetY + x + 36.0f) + pItem->pos, 2, MakeCardStatusString(i, cardStatus[i]));
			offsetY += 14.0f;
		}
	}
}

// realmyroommainui.cpp:6091
void CRealMyRoomMain::OnItemListOwnerDraw(intptr_t param) {
	FrListItem *pItem = (FrListItem *)param;
	// realmyroommainui.cpp:6093
	if (pItem == NULL)
		return;

	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();
	const Bitmap *pBg;
	if (pItem->pData == NULL) {
		// realmyroommainui.cpp:6102
		pBg = m_cardPackItemBmp[3];
	} else {
		// realmyroommainui.cpp:6105
		if (pItem->selected) {
			pBg = m_cardPackItemBmp[2];
			// realmyroommainui.cpp:6106
			pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
		} else if (pItem->underCursor) {
			// realmyroommainui.cpp:6107
			pBg = m_cardPackItemBmp[1];
			// realmyroommainui.cpp:6108
			pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		} else {
			// realmyroommainui.cpp:6110
			pBg = m_cardPackItemBmp[0];
			// realmyroommainui.cpp:6111
			pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		}
	}
	// realmyroommainui.cpp:6113
	pGDI->SetTextStyle(0);

	// realmyroommainui.cpp:6132
	if (pBg)
		// realmyroommainui.cpp:6133
		pGDI->DrawTexture(pBg, WRect(pItem->pos.x, pItem->pos.y, (float)pBg->Width(), (float)pBg->Height()), 0xFFFFFFFF, 0);

	// realmyroommainui.cpp:6137
	void *pData = pItem->pData;
	if (pData == NULL)
		return;

	unsigned long typeId = 0;
	const char *pName = "\xBE\xCB\xBC\xF6\xBE\xF8\xB4\xC2 \xBE\xC6\xC0\xCC\xC5\xDB";
	const char *pIconName = "caddie_no";
	int count = -1;
	unsigned char canDelete = 0;
	bool caddieRest = false;
	bool hasNote = false;
	unsigned char periodKind = 0;
	int days = 0;
	int hours = 0;
	bool hasCadItem = false;
	int cadDays = 0;
	int cadHours = 0;
	unsigned char pcBang = 0;
	const Bitmap *pUccIcon = NULL;
	unsigned char timeType = 0;
	unsigned long replayCourse = 0;
	SYSTEMTIME replayTime;

	// realmyroommainui.cpp:6165
	switch (m_roomMode) {
	default: {
		// realmyroommainui.cpp:6170
		sItemInfo *pInfo = (sItemInfo *)pData;
		typeId = pInfo->m_typeId;
		// realmyroommainui.cpp:6171
		IFF_ITEM_COMMON *pCommon = ItemManager()->FindCommonItem(typeId);
		// realmyroommainui.cpp:6172
		if (pCommon) {
			// realmyroommainui.cpp:6174
			pName = pCommon->m_name;
			// realmyroommainui.cpp:6175
			pIconName = pCommon->m_icon;
			// realmyroommainui.cpp:6176
			canDelete = ~pCommon->m_flags0 & 1;
			// realmyroommainui.cpp:6177
			count = pInfo->m_count;
			// realmyroommainui.cpp:6178
			periodKind = pInfo->m_stateBits.kind;
			// realmyroommainui.cpp:6179
			days = pInfo->m_remainHours / 24;
			// realmyroommainui.cpp:6180
			hours = pInfo->m_remainHours % 24;
		}
		// realmyroommainui.cpp:6184
		if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(pInfo->m_typeId)) {
			// realmyroommainui.cpp:6187
			pName = GetUccItemName(pInfo);
			// realmyroommainui.cpp:6188
			pUccIcon = GetUccItemIcon(pInfo);
			// realmyroommainui.cpp:6191
			if (m_roomMode == 0x15 && !(pInfo->m_uccStatus & 1)) {
				// realmyroommainui.cpp:6193
				const WPoint &mousePos = g_pFresh->GetManager()->GetMousePos();
				// realmyroommainui.cpp:6196
				sItemInfo *pUcc = (sItemInfo *)pItem->pData;
				// realmyroommainui.cpp:6197
				IFF_STRUCT::sPart *pPart = ItemManager()->FindPart(pUcc->m_typeId);
				// realmyroommainui.cpp:6200
				if (pPart) {
					if (pPart->slot == 7 || pPart->slot == 9) {
						// realmyroommainui.cpp:6207
						int rect = pPart->slot != 7;
						if (pItem->underCursor && (m_uccDrawRects[rect] + pItem->pos).IsInRect(mousePos)) {
							// realmyroommainui.cpp:6209
							pGDI->DrawTexture(m_itemListArea[12].over, m_uccDrawRects[rect] + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
							// realmyroommainui.cpp:6210
							CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x2E);
							// realmyroommainui.cpp:6212
						} else {
							// realmyroommainui.cpp:6214
							pGDI->DrawTexture(m_itemListArea[12].normal, m_uccDrawRects[rect] + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
						}
					}
					// realmyroommainui.cpp:6219
					if (pPart->slot == 7 || pPart->slot == 8) {
						int area;
						int toolTip;
						// realmyroommainui.cpp:6225
						if (pUcc->m_uccStatus & 2) {
							// realmyroommainui.cpp:6227
							area = 0xD;
							// realmyroommainui.cpp:6228
							toolTip = 0x3C;
							// realmyroommainui.cpp:6230
						} else {
							// realmyroommainui.cpp:6232
							area = 0xB;
							// realmyroommainui.cpp:6233
							toolTip = 0x2D;
						}
						// realmyroommainui.cpp:6237
						if (pItem->underCursor && (m_uccDrawRects[1] + pItem->pos).IsInRect(mousePos)) {
							// realmyroommainui.cpp:6239
							pGDI->DrawTexture(m_itemListArea[area].over, m_uccDrawRects[1] + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
							// realmyroommainui.cpp:6240
							CIconToolTip::GetInstance()->SetToolTip((eBarButton)toolTip);
							// realmyroommainui.cpp:6242
						} else {
							// realmyroommainui.cpp:6244
							pGDI->DrawTexture(m_itemListArea[area].normal, m_uccDrawRects[1] + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
						}
					}
				}
			}
		}
		break;
	}

	case 8: {
		// realmyroommainui.cpp:6255
		sItemInfo *pInfo = (sItemInfo *)pData;
		typeId = pInfo->m_typeId;
		// realmyroommainui.cpp:6256
		IFF_ITEM_COMMON *pCommon = ItemManager()->FindCommonItem(typeId);
		// realmyroommainui.cpp:6257
		if (pCommon) {
			// realmyroommainui.cpp:6258
			pName = pCommon->m_name;
			// realmyroommainui.cpp:6259
			pIconName = pCommon->m_icon;
			// realmyroommainui.cpp:6260
			canDelete = ~pCommon->m_flags0 & 1;
			// realmyroommainui.cpp:6261
			count = pInfo->m_count;
			// realmyroommainui.cpp:6262
			periodKind = pInfo->m_stateBits.kind;
			// realmyroommainui.cpp:6265
			for (int k = 0; k < 10; k++) {
				if (typeId == WSingleton<CSharedDoc>::Instance()->m_equipItem[k])
					// realmyroommainui.cpp:6266
					count--;
			}
		}
		// realmyroommainui.cpp:6271
		break;
	}

	case 0xA: {
		// realmyroommainui.cpp:6275
		sCaddieInfo *pCaddie = (sCaddieInfo *)pData;
		typeId = pCaddie->typeId;
		// realmyroommainui.cpp:6276
		IFF_STRUCT::sCaddie *pIff = ItemManager()->FindCaddie(typeId);
		// realmyroommainui.cpp:6277
		if (pIff) {
			// realmyroommainui.cpp:6284
			pName = pIff->m_name;
			pIconName = pIff->m_icon;
			count = 1;
			days = pCaddie->days;
			if ((pCaddie->flags & 2) && pIff->fee > 0)
				// realmyroommainui.cpp:6285
				periodKind = 1;
			// realmyroommainui.cpp:6287
			if (pCaddie->cadItemTypeId & 0x1FFF) {
				// realmyroommainui.cpp:6290
				hasCadItem = true;
				cadDays = pCaddie->hours / 24;
				// realmyroommainui.cpp:6291
				cadHours = pCaddie->hours % 24;
			}
		}
		// realmyroommainui.cpp:6296
		if ((pCaddie->flags & 2) && pCaddie->days <= 0 && pCaddie->cadItemTypeId == 0 && pIff->fee)
			// realmyroommainui.cpp:6297
			caddieRest = true;
		// realmyroommainui.cpp:6299
		break;
	}

	case 0xC: {
		// realmyroommainui.cpp:6303
		sMascotInfo *pMascot = (sMascotInfo *)pData;
		typeId = pMascot->typeId;
		// realmyroommainui.cpp:6304
		IFF_ITEM_COMMON *pCommon = ItemManager()->FindCommonItem(typeId);
		// realmyroommainui.cpp:6305
		if (pCommon) {
			// realmyroommainui.cpp:6306
			pName = pCommon->m_name;
			// realmyroommainui.cpp:6307
			pIconName = pCommon->m_icon;
			// realmyroommainui.cpp:6309
			count = 1;
			days = pMascot->remainHours / 24;
			// realmyroommainui.cpp:6310
			hours = pMascot->remainHours % 24;
			// realmyroommainui.cpp:6312
			timeType = pCommon->m_timeType;
		}
		// realmyroommainui.cpp:6340
		pcBang = pMascot->pcBang;
		// realmyroommainui.cpp:6343
		break;
	}

	case 0x11: {
		// realmyroommainui.cpp:6348
		if (IsLocalContent((localContentType_t)0x60))
			break;
		// realmyroommainui.cpp:6350
		sGiftInfo *pGift = (sGiftInfo *)pItem->pData;
		// realmyroommainui.cpp:6351
		typeId = pGift->m_typeId;
		// realmyroommainui.cpp:6352
		IFF_ITEM_COMMON *pCommon = ItemManager()->FindCommonItem(typeId);
		// realmyroommainui.cpp:6353
		if (pCommon) {
			// realmyroommainui.cpp:6356
			count = pGift->m_pang;
			pName = pCommon->m_name;
			pIconName = pCommon->m_icon;
			// realmyroommainui.cpp:6357
			periodKind = pGift->m_timeUnit;
		}
		// realmyroommainui.cpp:6361
		if (pGift->m_sender[0] != '@')
			// realmyroommainui.cpp:6362
			hasNote = true;
		// realmyroommainui.cpp:6366
		break;
	}

	case 0x12: {
		// realmyroommainui.cpp:6373
		unsigned long trophyTypeId = ((sSpecialTrophy *)pData)->m_typeId;
		// realmyroommainui.cpp:6376
		typeId = trophyTypeId & ~0xF;
		int medal = trophyTypeId & 0xF;
		IFF_STRUCT::sMatch *pMatch = ItemManager()->FindMatch(typeId);
		// realmyroommainui.cpp:6377
		if (pMatch) {
			// realmyroommainui.cpp:6378
			pName = pMatch->m_name;
			// realmyroommainui.cpp:6379
			pIconName = pMatch->m_trophyIcon[medal];
			// realmyroommainui.cpp:6380
			count = ((sSpecialTrophy *)pData)->m_count;
		}
		// realmyroommainui.cpp:6383
		break;
	}

	case 0x14: {
		// realmyroommainui.cpp:6387
		typeId = *(unsigned long *)pData;
		// realmyroommainui.cpp:6388
		IFF_STRUCT::sQuest *pQuest = ItemManager()->FindQuest(typeId);
		// realmyroommainui.cpp:6389
		if (pQuest) {
			// realmyroommainui.cpp:6390
			pName = pQuest->m_name;
			// realmyroommainui.cpp:6391
			pIconName = pQuest->m_icon;
			// realmyroommainui.cpp:6392
			count = 1;
		}
		// realmyroommainui.cpp:6395
		break;
	}

	case 0x17:
		// realmyroommainui.cpp:6399
		if (IsLocalContent((localContentType_t)0x4F)) {
			// realmyroommainui.cpp:6401, the replay record: the type id at
			// +4, the course at +0xA8, the date at +0xAC (sReplayFileInfo
			// names none of them yet)
			const char *pReplay = (const char *)pItem->pData;
			// realmyroommainui.cpp:6402
			typeId = *(const unsigned long *)(pReplay + 4);
			// realmyroommainui.cpp:6403
			replayCourse = *(const unsigned long *)(pReplay + 0xA8);
			// realmyroommainui.cpp:6404
			replayTime = *(const SYSTEMTIME *)(pReplay + 0xAC);
			// realmyroommainui.cpp:6405
			IFF_ITEM_COMMON *pCommon = ItemManager()->FindCommonItem(typeId);
			// realmyroommainui.cpp:6407
			if (pCommon) {
				// realmyroommainui.cpp:6409
				pName = pCommon->m_name;
				// realmyroommainui.cpp:6410
				pIconName = pCommon->m_icon;
				// realmyroommainui.cpp:6411
				count = 1;
			}
		}
		// realmyroommainui.cpp:6415
		break;

	case 0x1B:
		// realmyroommainui.cpp:6419
		if (IsLocalContent((localContentType_t)0x51)) {
			// realmyroommainui.cpp:6425
			sCards *pCard = (sCards *)pItem->pData;
			// realmyroommainui.cpp:6427
			if (pCard->m_typeId) {
				// realmyroommainui.cpp:6429
				IFF_STRUCT::sCard *pIff = ItemManager()->FindCard(pCard->m_typeId);
				// realmyroommainui.cpp:6431
				if (pIff) {
					// realmyroommainui.cpp:6433
					typeId = pCard->m_typeId;
					// realmyroommainui.cpp:6436
					pName = pIff->m_name;
					pIconName = pIff->m_icon;
					count = pCard->m_count;
				}
			}
		}
		// realmyroommainui.cpp:6441
		break;

	case 0x1A: {
		// realmyroommainui.cpp:6445
		sItemInfo *pInfo = (sItemInfo *)pData;
		typeId = pInfo->m_typeId;
		// realmyroommainui.cpp:6447
		IFF_ITEM_COMMON *pCommon = ItemManager()->FindCommonItem(typeId);
		// realmyroommainui.cpp:6448
		if (pCommon) {
			// realmyroommainui.cpp:6451
			pIconName = pCommon->m_icon;
			// realmyroommainui.cpp:6452
			canDelete = ~pCommon->m_flags0 & 1;
			// realmyroommainui.cpp:6455
			count = 1;
			periodKind = 0;
			days = 0;
			// realmyroommainui.cpp:6456
			hours = 0;
		}
		// realmyroommainui.cpp:6461
		pName = GetUccItemName(pInfo);
		// realmyroommainui.cpp:6462
		pUccIcon = GetUccItemIcon(pInfo);
		// realmyroommainui.cpp:6465
		if (pInfo->m_uccStatus & 1)
			break;
		// realmyroommainui.cpp:6467
		const WPoint &mousePos = g_pFresh->GetManager()->GetMousePos();
		// realmyroommainui.cpp:6470
		IFF_STRUCT::sPart *pPart = ItemManager()->FindPart(pInfo->m_typeId);
		// realmyroommainui.cpp:6473
		if (pPart == NULL)
			break;
		if (pPart->slot == 7 || pPart->slot == 9) {
			// realmyroommainui.cpp:6475
			int rect = pPart->slot != 7;
			// realmyroommainui.cpp:6480
			if (pItem->underCursor && (m_uccDrawRects[rect] + pItem->pos).IsInRect(mousePos)) {
				// realmyroommainui.cpp:6482
				pGDI->DrawTexture(m_itemListArea[12].over, m_uccDrawRects[rect] + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
				// realmyroommainui.cpp:6483
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x2E);
				// realmyroommainui.cpp:6485
			} else {
				// realmyroommainui.cpp:6487
				pGDI->DrawTexture(m_itemListArea[12].normal, m_uccDrawRects[rect] + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			}
		}
		// realmyroommainui.cpp:6492
		if (pPart->slot == 7 || pPart->slot == 8) {
			int area;
			int toolTip;
			// realmyroommainui.cpp:6498
			if (pInfo->m_uccStatus & 2) {
				// realmyroommainui.cpp:6500
				area = 0xD;
				// realmyroommainui.cpp:6501
				toolTip = 0x3C;
				// realmyroommainui.cpp:6503
			} else {
				// realmyroommainui.cpp:6505
				area = 0xB;
				// realmyroommainui.cpp:6506
				toolTip = 0x2D;
			}
			// realmyroommainui.cpp:6509
			if (pItem->underCursor && (m_uccDrawRects[1] + pItem->pos).IsInRect(mousePos)) {
				// realmyroommainui.cpp:6511
				pGDI->DrawTexture(m_itemListArea[area].over, m_uccDrawRects[1] + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
				// realmyroommainui.cpp:6512
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)toolTip);
				// realmyroommainui.cpp:6514
			} else {
				// realmyroommainui.cpp:6516
				pGDI->DrawTexture(m_itemListArea[area].normal, m_uccDrawRects[1] + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			}
		}
		// realmyroommainui.cpp:6542
		break;
	}

	case 0x1C:
	case 0x1D:
	case 0x1E:
	case 0x1F:
	case 0x20: {
		// realmyroommainui.cpp:6550
		typeId = ((sFurniture_List *)pData)->m_typeId;
		// realmyroommainui.cpp:6552
		if ((typeId & 0xFC000000) != 0x48000000)
			break;
		// realmyroommainui.cpp:6554
		IFF_STRUCT::sFurniture *pFurniture = ItemManager()->FindFurniture(typeId);
		// realmyroommainui.cpp:6555
		if (pFurniture) {
			// realmyroommainui.cpp:6556
			pName = pFurniture->m_name;
			// realmyroommainui.cpp:6557
			pIconName = pFurniture->m_icon;
			// realmyroommainui.cpp:6558
			count = 1;
		}
		// realmyroommainui.cpp:6562
		break;
	}
	}

	// realmyroommainui.cpp:6566
	const Bitmap *pIcon = g_pFresh->GetBitmap(pIconName);
	// realmyroommainui.cpp:6567
	if (pUccIcon)
		// realmyroommainui.cpp:6568
		pIcon = pUccIcon;
	// realmyroommainui.cpp:6570
	CItemStorage *pStorage = CItemStorage::m_pInstance;
	// realmyroommainui.cpp:6571
	if (pStorage) {
		// realmyroommainui.cpp:6573
		if (pStorage->GetDragingItemIndex(sStoredItemDragDrop::MODE_PUSH) == pItem->idx)
			// realmyroommainui.cpp:6574
			pIcon = NULL;
	}

	WPoint iconPos;
	// realmyroommainui.cpp:6581
	if (!S5::ISCutinItem(typeId))
		// realmyroommainui.cpp:6583
		iconPos = WPoint(pItem->pos.x, pItem->pos.y);
	// realmyroommainui.cpp:6585
	else
		// realmyroommainui.cpp:6587
		iconPos = WPoint(pItem->pos.x, pItem->pos.y - 12.0f);
	// realmyroommainui.cpp:6590
	ReviseItemIconPoint(typeId, iconPos);

	// realmyroommainui.cpp:6592
	if (pIcon)
		// realmyroommainui.cpp:6593
		pGDI->DrawTexture(pIcon, WRect(iconPos.x, iconPos.y, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);

	// realmyroommainui.cpp:6601
	if (periodKind >= 2 && typeId != 0x1A000042 && periodKind != 6)
		// realmyroommainui.cpp:6603
		pGDI->DrawTexture(m_itemListArea[5].over, m_itemListArea[5].rect + iconPos, 0xFFFFFFFF, 0);

	// realmyroommainui.cpp:6608
	if (caddieRest) {
		// realmyroommainui.cpp:6609
		const Bitmap *pRest = g_pFresh->GetBitmap("caddie_rest");
		// realmyroommainui.cpp:6610
		pGDI->DrawTexture(pRest, WRect(iconPos.x, iconPos.y, (float)pRest->Width(), (float)pRest->Height()), 0xFFFFFFFF, 0);
	}

	// realmyroommainui.cpp:6613
	pGDI->SetTextStyle(0);

	// realmyroommainui.cpp:6615
	if (m_roomMode == 0x12) {
		// realmyroommainui.cpp:6617
		char text[100] = { 0 };
		// realmyroommainui.cpp:6619
		strcpy(text, pName);
		// realmyroommainui.cpp:6621
		char *pLine = text;
		// realmyroommainui.cpp:6626
		int line = 0;
		char *pBreak = strstr(pLine, "\n");
		// realmyroommainui.cpp:6627
		while (pBreak) {
			// realmyroommainui.cpp:6629
			*pBreak = 0;
			pGDI->Print11(WPoint(pItem->pos.x + 182.0f, line * 15.0f + 7.0f + pItem->pos.y), 2, pLine);
			// realmyroommainui.cpp:6631
			line++;
			pLine = pBreak + 1;
			pBreak = strstr(pLine, "\n");
		}
		// realmyroommainui.cpp:6633
		pGDI->Print11(WPoint(pItem->pos.x + 182.0f, line * 15.0f + 7.0f + pItem->pos.y), 2, pLine);
		// realmyroommainui.cpp:6635
		sprintf(text, "%d", count);
		// realmyroommainui.cpp:6636
		pGDI->Print11(WPoint(pItem->pos.x + 180.0f, pItem->pos.y + 67.0f), 2, text);
		// realmyroommainui.cpp:6641
	} else {
		// realmyroommainui.cpp:6643
		pGDI->Print11(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 7.0f), 2, "%s", pName);
	}

	// realmyroommainui.cpp:6646
	if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(typeId)) {
		// realmyroommainui.cpp:6652
		sItemInfo *pInfo = (sItemInfo *)pItem->pData;
		float stateX = pItem->pos.x + 51.0f;
		float stateY = pItem->pos.y + 50.0f;
		if ((pInfo->m_uccStatus & 1) && pInfo->m_uccSeq == 1 && m_pUccStateBitmap[0]) {
			// realmyroommainui.cpp:6654
			pGDI->DrawTexture(m_pUccStateBitmap[0], WRect(stateX, stateY, (float)m_pUccStateBitmap[0]->Width(), (float)m_pUccStateBitmap[0]->Height()), 0xFFFFFFFF, 0);
			// realmyroommainui.cpp:6656
		} else if ((pInfo->m_uccStatus & 1) && pInfo->m_uccSeq > 1 && m_pUccStateBitmap[1]) {
			// realmyroommainui.cpp:6661
			pGDI->DrawTexture(m_pUccStateBitmap[1], WRect(stateX, stateY, (float)m_pUccStateBitmap[1]->Width(), (float)m_pUccStateBitmap[1]->Height()), 0xFFFFFFFF, 0);
			// realmyroommainui.cpp:6663
		} else if ((pInfo->m_uccStatus & 2) && m_pUccStateBitmap[2]) {
			// realmyroommainui.cpp:6665
			pGDI->DrawTexture(m_pUccStateBitmap[2], WRect(stateX, stateY, (float)m_pUccStateBitmap[2]->Width(), (float)m_pUccStateBitmap[2]->Height()), 0xFFFFFFFF, 0);
		}
		// realmyroommainui.cpp:6669
		if (pInfo->m_uccStatus & 1) {
			// realmyroommainui.cpp:6671
			pGDI->SetTextColor(0xFF9E2D3E, 0xFFFFFFFF);
			// realmyroommainui.cpp:6672
			pGDI->Print11(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 21.0f), 2, pInfo->m_reserved5E);
			// realmyroommainui.cpp:6673
			pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		}
	}

	// realmyroommainui.cpp:6678
	const WPoint &mousePos = g_pFresh->GetManager()->GetMousePos();
	// realmyroommainui.cpp:6679
	int roomMode = m_roomMode;
	// realmyroommainui.cpp:6683
	bool handled = false;
	GetActor("ItemBuff") << MsgObject(0, 0x246, 0, (intptr_t)&handled, (intptr_t)pItem, (intptr_t)&mousePos, typeId);
	int mode;
	// realmyroommainui.cpp:6684
	if (handled)
		// realmyroommainui.cpp:6686
		mode = 0x23;
	else
		mode = roomMode;

	// realmyroommainui.cpp:6689
	WPoint textPos(pItem->pos.x + 182.0f, pItem->pos.y + 23.0f);
	WPoint textPos2(textPos.x, textPos.y + 16.0f);
	// realmyroommainui.cpp:6692
	switch (mode) {
	case 0x11:
		// realmyroommainui.cpp:6697
		if (IsLocalContent((localContentType_t)0x60))
			break;
		// realmyroommainui.cpp:6699
		if (pItem->underCursor && (m_itemListArea[0].rect + pItem->pos).IsInRect(mousePos)) {
			// realmyroommainui.cpp:6700
			pGDI->DrawTexture(m_itemListArea[0].over, m_itemListArea[0].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			// realmyroommainui.cpp:6701
			CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x1B);
			// realmyroommainui.cpp:6702
		} else {
			// realmyroommainui.cpp:6703
			pGDI->DrawTexture(m_itemListArea[0].normal, m_itemListArea[0].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
		}
		// realmyroommainui.cpp:6705
		if (hasNote) {
			// realmyroommainui.cpp:6706
			if (pItem->underCursor && (m_itemListArea[4].rect + pItem->pos).IsInRect(mousePos)) {
				// realmyroommainui.cpp:6707
				pGDI->DrawTexture(m_itemListArea[4].over, m_itemListArea[4].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
				// realmyroommainui.cpp:6708
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x1F);
				// realmyroommainui.cpp:6709
			} else {
				// realmyroommainui.cpp:6710
				pGDI->DrawTexture(m_itemListArea[4].normal, m_itemListArea[4].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			}
		}
		// realmyroommainui.cpp:6715
		break;

	case 8:
		// realmyroommainui.cpp:6717
		if (pItem->underCursor && (m_itemListArea[1].rect + pItem->pos).IsInRect(mousePos)) {
			// realmyroommainui.cpp:6718
			pGDI->DrawTexture(m_itemListArea[1].over, m_itemListArea[1].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			// realmyroommainui.cpp:6719
			CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x1A);
			// realmyroommainui.cpp:6720
		} else {
			// realmyroommainui.cpp:6721
			pGDI->DrawTexture(m_itemListArea[1].normal, m_itemListArea[1].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
		}
		// realmyroommainui.cpp:6723
		if (canDelete) {
			// realmyroommainui.cpp:6724
			if (pItem->underCursor && (m_itemListArea[2].rect + pItem->pos).IsInRect(mousePos)) {
				// realmyroommainui.cpp:6725
				pGDI->DrawTexture(m_itemListArea[2].over, m_itemListArea[2].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
				// realmyroommainui.cpp:6726
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x1C);
				// realmyroommainui.cpp:6727
			} else {
				// realmyroommainui.cpp:6728
				pGDI->DrawTexture(m_itemListArea[2].normal, m_itemListArea[2].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			}
		}
		// realmyroommainui.cpp:6731
		break;

	case 0xA:
		// realmyroommainui.cpp:6733
		if (caddieRest) {
			// realmyroommainui.cpp:6734
			if (pItem->underCursor && (m_itemListArea[3].rect + pItem->pos).IsInRect(mousePos)) {
				// realmyroommainui.cpp:6735
				pGDI->DrawTexture(m_itemListArea[3].over, m_itemListArea[3].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
				// realmyroommainui.cpp:6736
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x1E);
				// realmyroommainui.cpp:6737
			} else {
				// realmyroommainui.cpp:6738
				pGDI->DrawTexture(m_itemListArea[3].normal, m_itemListArea[3].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			}
		}
		// realmyroommainui.cpp:6741
		break;

	case 9:
		// realmyroommainui.cpp:6744
		switch (typeId) {
		case 0x1A00003B:
			// realmyroommainui.cpp:6748
			if (pItem->underCursor && (m_itemListArea[0].rect + pItem->pos).IsInRect(mousePos)) {
				// realmyroommainui.cpp:6749
				pGDI->DrawTexture(m_itemListArea[0].over, m_itemListArea[0].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
				// realmyroommainui.cpp:6750
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x20);
				// realmyroommainui.cpp:6751
			} else {
				// realmyroommainui.cpp:6752
				pGDI->DrawTexture(m_itemListArea[0].normal, m_itemListArea[0].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			}
			// realmyroommainui.cpp:6755
			break;

		case 0x1A000003:
		case 0x1A00003E:
		case 0x1A000048:
		case 0x1A000052:
		case 0x1A000054:
		case 0x1A0000BC:
		case 0x1A0000D6:
		case 0x1A000105:
		case 0x1A00012F:
		case 0x1A000130:
		case 0x1A000131:
		case 0x1A00014B:
			// realmyroommainui.cpp:6842
			if (pItem->underCursor && (m_itemListArea[0].rect + pItem->pos).IsInRect(mousePos)) {
				// realmyroommainui.cpp:6843
				pGDI->DrawTexture(m_itemListArea[0].over, m_itemListArea[0].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
				// realmyroommainui.cpp:6844
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x21);
				// realmyroommainui.cpp:6845
			} else {
				// realmyroommainui.cpp:6846
				pGDI->DrawTexture(m_itemListArea[0].normal, m_itemListArea[0].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			}
			// realmyroommainui.cpp:6849
			break;

		default: {
			// realmyroommainui.cpp:6854
			IFF_STRUCT::sItem *pIff = ItemManager()->FindItem(typeId);
			// realmyroommainui.cpp:6855
			if (pIff) {
				// realmyroommainui.cpp:6857
				if (pIff->isRandomBox) {
					// realmyroommainui.cpp:6859
					if (pItem->underCursor && (m_itemListArea[0].rect + pItem->pos).IsInRect(mousePos)) {
						// realmyroommainui.cpp:6860
						pGDI->DrawTexture(m_itemListArea[0].over, m_itemListArea[0].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
						// realmyroommainui.cpp:6861
						CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x21);
						// realmyroommainui.cpp:6862
					} else {
						// realmyroommainui.cpp:6863
						pGDI->DrawTexture(m_itemListArea[0].normal, m_itemListArea[0].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
					}
				}
			}
			// realmyroommainui.cpp:6870
			if (FrCardRemove::IsCardRemoveItem(typeId)) {
				// realmyroommainui.cpp:6872
				if (pItem->underCursor && (m_itemListArea[0].rect + pItem->pos).IsInRect(mousePos))
					// realmyroommainui.cpp:6873
					pGDI->DrawTexture(m_itemListArea[0].over, m_itemListArea[0].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
				// realmyroommainui.cpp:6874
				else
					// realmyroommainui.cpp:6875
					pGDI->DrawTexture(m_itemListArea[0].normal, m_itemListArea[0].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			}
			// realmyroommainui.cpp:6881
			break;
		}
		}
		break;

	case 0x18:
		// realmyroommainui.cpp:6885
		if (typeId == 0x1A000042) {
			// realmyroommainui.cpp:6887
			if (IsLocalContent((localContentType_t)0x14)) {
				// realmyroommainui.cpp:6889
				if (pItem->underCursor && (m_itemListArea[0].rect + pItem->pos).IsInRect(mousePos)) {
					// realmyroommainui.cpp:6890
					pGDI->DrawTexture(m_itemListArea[0].over, m_itemListArea[0].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
					// realmyroommainui.cpp:6891
					CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x22);
					// realmyroommainui.cpp:6892
				} else {
					// realmyroommainui.cpp:6893
					pGDI->DrawTexture(m_itemListArea[0].normal, m_itemListArea[0].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
				}
			}
		}
		// realmyroommainui.cpp:6900
		break;

	case 0x17:
		// realmyroommainui.cpp:6904
		if (IsLocalContent((localContentType_t)0x4F)) {
			// realmyroommainui.cpp:6906
			if (pItem->underCursor && (m_itemListArea[7].rect + pItem->pos).IsInRect(mousePos))
				// realmyroommainui.cpp:6907
				pGDI->DrawTexture(m_itemListArea[7].over, m_itemListArea[7].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			// realmyroommainui.cpp:6908
			else
				// realmyroommainui.cpp:6909
				pGDI->DrawTexture(m_itemListArea[7].normal, m_itemListArea[7].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			// realmyroommainui.cpp:6912
			if (pItem->underCursor && (m_itemListArea[8].rect + pItem->pos).IsInRect(mousePos))
				// realmyroommainui.cpp:6913
				pGDI->DrawTexture(m_itemListArea[8].over, m_itemListArea[8].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			// realmyroommainui.cpp:6914
			else
				// realmyroommainui.cpp:6915
				pGDI->DrawTexture(m_itemListArea[8].normal, m_itemListArea[8].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
		}
		// realmyroommainui.cpp:6919
		break;

	case 0x1B:
		// realmyroommainui.cpp:6923
		if (IsLocalContent((localContentType_t)0x51)) {
			// realmyroommainui.cpp:6925
			if (pItem->underCursor && (m_itemListArea[9].rect + pItem->pos).IsInRect(mousePos)) {
				// realmyroommainui.cpp:6927
				pGDI->DrawTexture(m_itemListArea[9].over, m_itemListArea[9].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
				// realmyroommainui.cpp:6928
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x27);
				// realmyroommainui.cpp:6930
			} else {
				// realmyroommainui.cpp:6932
				pGDI->DrawTexture(m_itemListArea[9].normal, m_itemListArea[9].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			}
		}
		// realmyroommainui.cpp:6936
		break;

	case 0x1C:
	case 0x1D:
	case 0x1E:
	case 0x1F:
	case 0x20:
		// realmyroommainui.cpp:6943
		if (IsLocalContent((localContentType_t)0x56)) {
			// realmyroommainui.cpp:6945
			if (pItem->underCursor && (m_itemListArea[10].rect + pItem->pos).IsInRect(mousePos)) {
				// realmyroommainui.cpp:6947
				pGDI->DrawTexture(m_itemListArea[10].over, m_itemListArea[10].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
				// realmyroommainui.cpp:6948
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x3E);
				// realmyroommainui.cpp:6949
			} else {
				// realmyroommainui.cpp:6950
				pGDI->DrawTexture(m_itemListArea[10].normal, m_itemListArea[10].rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			}
		}
		break;
	}

	// realmyroommainui.cpp:6958
	if (count == -1) {
		// realmyroommainui.cpp:6960
		pGDI->Print11(textPos, 2, "%d", typeId);
		// realmyroommainui.cpp:6962
		return;
	}

	// realmyroommainui.cpp:6965
	int itemClass = typeId >> 26;
	switch (itemClass) {
	case 2: {
		// realmyroommainui.cpp:6969
		IFF_STRUCT::sPart *pPart = ItemManager()->FindPart(typeId);
		// realmyroommainui.cpp:6970
		if (pPart) {
			// realmyroommainui.cpp:6972
			DrawStatus2(pGDI, pItem, pPart->m_baseStat, pPart->m_slotStat, 0.0f);
			// realmyroommainui.cpp:6973
			if (m_roomMode != 0x11)
				// realmyroommainui.cpp:6975
				DrawPartsSlot(pGDI, pItem);
		}
		// realmyroommainui.cpp:6980
		sItemInfo *pInfo = (sItemInfo *)pItem->pData;
		// realmyroommainui.cpp:6983
		if (pInfo && pInfo->m_stateBits.kind == 6) {
			// realmyroommainui.cpp:6985
			if (pInfo->m_stateBits.expired) {
				// realmyroommainui.cpp:6988
				const Bitmap *pExpired = g_pFresh->GetBitmap("rental_item_expired");
				// realmyroommainui.cpp:6989
				if (pExpired == NULL)
					return;
				// realmyroommainui.cpp:6990
				pGDI->DrawTexture(pExpired, WRect(pItem->pos, pExpired->Width(), pExpired->Height()), 0xFFFFFFFF, 0);
				// realmyroommainui.cpp:6992
				return;
			}
			// realmyroommainui.cpp:6997
			const Bitmap *pClock = g_pFresh->GetBitmap("myroom_rental_clock");
			// realmyroommainui.cpp:6998
			if (pClock)
				// realmyroommainui.cpp:6999
				pGDI->DrawTexture(pClock, WRect(pItem->pos.x + 70.0f, pItem->pos.y + 43.0f, pClock->Width(), pClock->Height()), 0xFFFFFFFF, 0);
			// realmyroommainui.cpp:7003
		} else if (pInfo->m_stateBits.kind < 2) {
			return;
		}
		// realmyroommainui.cpp:7010
		pGDI->SetTextColor(0xFF4D4D4D, 0xFFFFFFFF);
		// realmyroommainui.cpp:7011
		if (days > 0) {
			// realmyroommainui.cpp:7013
			pGDI->Print11(textPos, 2, "\xB3\xB2\xC0\xBA\xB1\xE2\xB0\xA3 : %d\xC0\xCF %d\xBD\xC3\xB0\xA3", days, hours);
			return;
		}
		// realmyroommainui.cpp:7017
		pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
		// realmyroommainui.cpp:7018
		pGDI->Print11(textPos, 2, "\xB3\xB2\xC0\xBA\xB1\xE2\xB0\xA3 : %d\xBD\xC3\xB0\xA3", hours);
		return;
	}

	case 0x1C:
		// realmyroommainui.cpp:7025
		if (typeId & 0x1F0000)
			return;
		// realmyroommainui.cpp:7026
		pGDI->Print11(textPos, 2, "%d\xB0\xB3", count);
		return;

	case 5:
		// realmyroommainui.cpp:7030
		if (ItemManager()->IsBasicAztecFamily(typeId))
			return;
		// realmyroommainui.cpp:7031
		pGDI->Print11(textPos, 2, "%d\xB0\xB3", count);
		return;

	case 6:
		// realmyroommainui.cpp:7035
		if (periodKind >= 2) {
			// realmyroommainui.cpp:7037
			if (typeId == 0x1A000042 && IsLocalContent((localContentType_t)0x14)) {
				// realmyroommainui.cpp:7039, the report's date: words of
				// sItemInfo 0x18 (a SYSTEMTIME)
				sItemInfo *pInfo = (sItemInfo *)pItem->pData;
				// realmyroommainui.cpp:7041
				pGDI->SetTextColor(0xFF4D4D4D, 0xFFFFFFFF);
				// realmyroommainui.cpp:7042
				pGDI->Print11(textPos, 2, "%d\xB3\xE2 %d\xBF\xF9 %d\xC0\xCF", ((const SYSTEMTIME *)pInfo->m_reserved18)->wYear, ((const SYSTEMTIME *)pInfo->m_reserved18)->wMonth, ((const SYSTEMTIME *)pInfo->m_reserved18)->wDay);
				// realmyroommainui.cpp:7043
				pGDI->SetTextColor(0xFF4D4D4D, 0xFFFFFFFF);
				// realmyroommainui.cpp:7045
				pGDI->Print11(textPos2, 2, " %d\xBD\xC3 %d \xBA\xD0", ((const SYSTEMTIME *)pInfo->m_reserved18)->wHour, ((const SYSTEMTIME *)pInfo->m_reserved18)->wMinute);
				return;
			}
			// realmyroommainui.cpp:7048
			if (days == 0 && hours == 0) {
				// realmyroommainui.cpp:7050
				pGDI->SetTextColor(0xFF4D4D4D, 0xFFFFFFFF);
				// realmyroommainui.cpp:7051
				pGDI->Print11(textPos, 2, "\xB3\xB2\xC0\xBA\xBD\xC3\xB0\xA3:");
				// realmyroommainui.cpp:7052
				pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
				// realmyroommainui.cpp:7053
				pGDI->Print11(textPos2, 2, "1\xBD\xC3\xB0\xA3\xB9\xCC\xB8\xB8");
				return;
			}
			// realmyroommainui.cpp:7057
			pGDI->SetTextColor(0xFF4D4D4D, 0xFFFFFFFF);
			// realmyroommainui.cpp:7058
			pGDI->Print11(textPos, 2, "\xB3\xB2\xC0\xBA\xBD\xC3\xB0\xA3:");
			pGDI->Print11(textPos2, 2, "%d\xC0\xCF %d\xBD\xC3\xB0\xA3", days, hours);
			return;
		}
		// realmyroommainui.cpp:7065
		if (IsLocalContent((localContentType_t)0x4F) && m_roomMode == 0x17) {
			// realmyroommainui.cpp:7067
			IFF_STRUCT::sCourse *pCourse = ItemManager()->FindCourse(replayCourse | 0x28000000);
			// realmyroommainui.cpp:7068
			if (pCourse)
				// realmyroommainui.cpp:7069
				pGDI->Print11(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 22.0f), 2, pCourse->m_name);
			// realmyroommainui.cpp:7084
			char date[50] = { 0 };
			char time[50] = { 0 };
			wsprintfA(date, "%d\xB3\xE2%02d\xBF\xF9%02d\xC0\xCF", replayTime.wYear, replayTime.wMonth, replayTime.wDay);
			// realmyroommainui.cpp:7085
			pGDI->Print11(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 33.0f), 2, date);
			// realmyroommainui.cpp:7088
			wsprintfA(time, "%02d\xBD\xC3%02d\xBA\xD0", replayTime.wHour, replayTime.wMinute);
			// realmyroommainui.cpp:7089
			pGDI->Print11(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 44.0f), 2, time);
			return;
		}
		// realmyroommainui.cpp:7093
		if (typeId == 0x1A000006)
			return;
		// realmyroommainui.cpp:7095
		if (typeId == 0x1A000010) {
			// realmyroommainui.cpp:7101
			pGDI->Print11(textPos, 2, "%d\xC6\xCE", count);
			return;
		}
		// realmyroommainui.cpp:7099
		pGDI->Print11(textPos, 2, "%d\xB0\xB3", count);
		return;

	case 0x10:
		// realmyroommainui.cpp:7107
		if (m_roomMode == 0x11)
			return;
		// realmyroommainui.cpp:7109
		if (pcBang)
			return;
		// realmyroommainui.cpp:7134
		if (!timeType) {
			// realmyroommainui.cpp:7136
			pGDI->SetTextColor(0xFF4D4D4D, 0xFFFFFFFF);
			// realmyroommainui.cpp:7137
			pGDI->Print11(textPos, 2, "\xB3\xB2\xC0\xBA\xBD\xC3\xB0\xA3:");
			// realmyroommainui.cpp:7138
			pGDI->Print11(textPos2, 2, "\xB9\xAB\xC1\xA6\xC7\xD1", days, hours);
			return;
		}
		// realmyroommainui.cpp:7143
		if (days == 0 && hours == 0) {
			// realmyroommainui.cpp:7145
			pGDI->SetTextColor(0xFF4D4D4D, 0xFFFFFFFF);
			// realmyroommainui.cpp:7146
			pGDI->Print11(textPos, 2, "\xB3\xB2\xC0\xBA\xBD\xC3\xB0\xA3:");
			// realmyroommainui.cpp:7147
			pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
			// realmyroommainui.cpp:7148
			pGDI->Print11(textPos2, 2, "1\xBD\xC3\xB0\xA3\xB9\xCC\xB8\xB8");
			return;
		}
		// realmyroommainui.cpp:7152
		pGDI->SetTextColor(0xFF4D4D4D, 0xFFFFFFFF);
		// realmyroommainui.cpp:7153
		pGDI->Print11(textPos, 2, "\xB3\xB2\xC0\xBA\xBD\xC3\xB0\xA3:");
		// realmyroommainui.cpp:7154
		pGDI->Print11(textPos2, 2, "%d\xC0\xCF %d\xBD\xC3\xB0\xA3", days, hours);
		return;

	case 7:
		// realmyroommainui.cpp:7161
		if (periodKind == 1) {
			// realmyroommainui.cpp:7163
			if (days == 0 && hours == 0) {
				// realmyroommainui.cpp:7165
				pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
				// realmyroommainui.cpp:7166
				pGDI->Print11(textPos, 2, "\xC8\xDE\xB0\xA1\xC1\xDF");
				return;
			}
			// realmyroommainui.cpp:7170
			pGDI->SetTextColor(0xFF4D4D4D, 0xFFFFFFFF);
			// realmyroommainui.cpp:7171
			pGDI->Print11(textPos, 2, "\xB3\xB2\xC0\xBA \xB0\xED\xBF\xEB\xB1\xE2\xB0\xA3:");
			// realmyroommainui.cpp:7172
			pGDI->Print11(textPos2, 2, "%d\xC0\xCF", days);
			// realmyroommainui.cpp:7174
			if (!hasCadItem)
				return;
			// realmyroommainui.cpp:7176
			pGDI->SetTextColor(0xFF4D4D4D, 0xFFFFFFFF);
			// realmyroommainui.cpp:7177
			pGDI->Print11(WPoint(textPos2.x, textPos2.y + 16.0f), 2, "\xB2\xD9\xB9\xCC\xB1\xE2 \xB3\xB2\xC0\xBA \xB1\xE2\xB0\xA3:");
			// realmyroommainui.cpp:7178
			pGDI->Print11(WPoint(textPos2.x, textPos2.y + 32.0f), 2, "%d\xC0\xCF %d\xBD\xC3\xB0\xA3", cadDays, cadHours);
			return;
		}
		// realmyroommainui.cpp:7184
		if (!hasCadItem)
			return;
		// realmyroommainui.cpp:7186
		pGDI->SetTextColor(0xFF4D4D4D, 0xFFFFFFFF);
		// realmyroommainui.cpp:7187
		pGDI->Print11(textPos, 2, "\xB2\xD9\xB9\xCC\xB1\xE2 \xB3\xB2\xC0\xBA \xB1\xE2\xB0\xA3:");
		// realmyroommainui.cpp:7188
		pGDI->Print11(textPos2, 2, "%d\xC0\xCF %d\xBD\xC3\xB0\xA3", cadDays, cadHours);
		return;

	case 4: {
		// realmyroommainui.cpp:7200
		int enchantPang = 0;
		if (m_roomMode != 0x11) {
			// realmyroommainui.cpp:7202
			short *pLevels = ((sItemInfo *)pItem->pData)->m_clubStatLevel;
			// realmyroommainui.cpp:7204
			for (int stat = 0; stat < 5; stat++) {
				// realmyroommainui.cpp:7206
				for (int level = 0; level < pLevels[stat]; level++) {
					// realmyroommainui.cpp:7208
					IFF_STRUCT::sEnchant *pEnchant = ItemManager()->FindEnchant(((stat | 0x340) << 20) | level);
					// realmyroommainui.cpp:7210
					if (pEnchant)
						// realmyroommainui.cpp:7212
						enchantPang += pEnchant->m_value;
				}
			}
		}
		// realmyroommainui.cpp:7218
		pGDI->SetTextColor(0xFF009000, 0xFFFFFFFF);
		// realmyroommainui.cpp:7219
		pGDI->Print11(textPos, 2, "\xBE\xF7\xB1\xD7\xB7\xB9\xC0\xCC\xB5\xE5\xC7\xD1 \xBA\xF1\xBF\xEB:");
		// realmyroommainui.cpp:7220
		pGDI->Print11(textPos2, 2, "%d\xC6\xCE", enchantPang);
		return;
	}

	case 0x1F: {
		// realmyroommainui.cpp:7225
		if (!IsLocalContent((localContentType_t)0x51))
			return;
		// realmyroommainui.cpp:7227
		pGDI->Print11(textPos, 2, "%d\xB0\xB3", count);
		// realmyroommainui.cpp:7229
		sCards *pCard = (sCards *)pItem->pData;
		// realmyroommainui.cpp:7230
		IFF_STRUCT::sCard *pIff = ItemManager()->FindCard(pCard->m_typeId);
		// realmyroommainui.cpp:7231
		DrawStatusCard(pGDI, pItem, pIff->m_stat, 0.0f);
		return;
	}

	case 0x12: {
		// realmyroommainui.cpp:7237
		if (!IsLocalContent((localContentType_t)0x56))
			return;
		// realmyroommainui.cpp:7239
		if (itemClass != 0x12)
			return;
		// realmyroommainui.cpp:7241
		sFurniture_List *pFurniture = (sFurniture_List *)pItem->pData;
		// realmyroommainui.cpp:7243
		if (!GetArrangement(typeId, pFurniture->m_id))
			return;
		// realmyroommainui.cpp:7245
		const Bitmap *pCheck = g_pFresh->GetBitmap("rmr_arg_item_check");
		// realmyroommainui.cpp:7246
		pGDI->DrawTexture(pCheck, WRect(pItem->pos.x + 8.0f, pItem->pos.y + 62.0f, (float)pCheck->Width(), (float)pCheck->Height()), 0xFFFFFFFF, 0);
		return;
	}

	case 0xE: {
		// realmyroommainui.cpp:7255
		if ((typeId & 0x3C00000) != 0x1800000)
			return;
		// realmyroommainui.cpp:7257
		Title *pTitle = g_TitleManager->GetTitle((eTitle)(typeId & 0x3FFFFF));
		// realmyroommainui.cpp:7258
		if (pTitle == NULL)
			return;
		// realmyroommainui.cpp:7260
		if (pTitle->CanEquip())
			return;
		// realmyroommainui.cpp:7262
		const Bitmap *pNoFit = g_pFresh->GetBitmap("do not fit");
		// realmyroommainui.cpp:7263
		if (pNoFit == NULL)
			return;
		// realmyroommainui.cpp:7264
		pGDI->DrawTexture(pNoFit, WRect(WPoint(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f), pNoFit->Width(), pNoFit->Height()), 0xFFFFFFFF, 0);
		return;
	}
	}
	// realmyroommainui.cpp:7274
}

// realmyroommainui.cpp:7277
void CRealMyRoomMain::MoveGift(sGiftInfo* pGiftInfo) {
	IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(pGiftInfo->m_typeId);

	if (!WSingleton<CSharedDoc>::Instance()->ConfirmInsertItem(pGiftInfo->m_typeId, pGiftInfo->m_pang)) {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		WRect rect = pForm->GetRect();
		pForm->SetRect(WRect(rect.x, rect.y, rect.w, rect.h));
		pForm->SetTimeLimit(12.0f);
		// "You hold 20,000 of this item, so it cannot be bought or moved."
		pForm->SetMessage("\xC7\xF6\xC0\xE7 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xC7 \xBA\xB8\xC0\xAF\xB0\xB3\xBC\xF6\xB0\xA1 20,000\xB0\xB3\xC0\xCC\xB9\xC7\xB7\xCE \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xB1\xB8\xC0\xD4 \xB6\xC7\xB4\xC2 \xBF\xC5\xB1\xE6 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
		pForm->Open(0, 0);
		return;
	}

	unsigned long typeId = pGiftInfo->m_typeId;
	switch (typeId >> 26) {
	default:
		if (pItem->m_flags0 & 1) {
			// The gift's serial leads sGiftInfo (its member at 0x0 is not declared yet).
			if (GetItemNumInMyPocket(typeId)) {
				FrReturnGiftDlg* pDlg = CreateForm<FrReturnGiftDlg>(g_pFresh->GetManager(), this, "notify_yesno", 0);
				// "You already own this cookie item. It can be refunded for %d cookies. Convert to cookies?"
				pDlg->SetMessage(MakeStr("\xC0\xCC\xB9\xCC \xB0\xA1\xC1\xF6\xB0\xED \xC0\xD6\xB4\xC2 \\c0xffff0000\\c\xC4\xED\xC5\xB0\xBE\xC6\xC0\xCC\xC5\xDB\\c0xff000000\\c\xC0\xD4\xB4\xCF\xB4\xD9.\n\\c0xffff0000\\c%d \xC4\xED\xC5\xB0\\c0xff000000\\c\xB7\xCE \xC8\xAF\xBA\xD2\xB9\xDE\xC0\xBB \xBC\xF6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.\n\xC4\xED\xC5\xB0\xB7\xCE \xC0\xFC\xC8\xAF\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", (pItem->m_discountPrice > 0 && pItem->m_discountPrice < pItem->m_price) ? pItem->m_discountPrice : pItem->m_price), false);
				pDlg->m_requestIds[0] = *(const unsigned long*)pGiftInfo;
				pDlg->m_requestIds[1] = pGiftInfo->m_typeId;

				pDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnReturnGiftDlgResult, 0);
				return;
			}
		}
		break;

	case 8: {
		for (std::map<unsigned int, sCaddieInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_caddieMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end(); ++it) {
			if (it->second.typeId == (((typeId >> 21) & 0x1F) | 0x1C000000)) {
				IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(it->second.typeId);
				if (it->second.days == 0 && pCaddie && pCaddie->fee > 0 && !(pCaddie->flags & 1)) {
					// "This caddie is on vacation. Please rehire it before moving the gift."
					this << MsgObject(0, 0x23, (intptr_t)"\xC8\xDE\xB0\xA1\xC1\xDF\xC0\xCE \xC4\xB3\xB5\xF0\xC0\xD4\xB4\xCF\xB4\xD9. \xC0\xE7\xB0\xED\xBF\xEB \xC8\xC4 \xBC\xB1\xB9\xB0 \xBF\xC5\xB1\xE2\xB1\xE2\xB8\xA6 \xC7\xD8\xC1\xD6\xBC\xBC\xBF\xE4", 0, 0, 0, 0);
					return;
				}
				break;
			}
		}
		break;
	}

	case 5:
	case 6:
		if (typeId == 0x1A000010) {
			m_pPangGiftDlg = CreateForm<FrPangGiftDlg>(g_pFresh->GetManager(), this, "panggift", 0);
			m_pPangGiftDlg->SetItemInfo(pGiftInfo);
			m_pPangGiftDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnPangGiftDlgResult, 1);
			return;
		}
		break;

	case 2:
	case 4:
		if (pItem->m_flags0 & 1) {
			if (GetItemNumInMyPocket(typeId)) {
				FrReturnGiftDlg* pDlg = CreateForm<FrReturnGiftDlg>(g_pFresh->GetManager(), this, "notify_yesno", 0);
				// "You already own this cookie item. It can be refunded for %d cookies. Convert to cookies?"
				pDlg->SetMessage(MakeStr("\xC0\xCC\xB9\xCC \xB0\xA1\xC1\xF6\xB0\xED \xC0\xD6\xB4\xC2 \\c0xffff0000\\c\xC4\xED\xC5\xB0\xBE\xC6\xC0\xCC\xC5\xDB\\c0xff000000\\c\xC0\xD4\xB4\xCF\xB4\xD9.\n\\c0xffff0000\\c%d \xC4\xED\xC5\xB0\\c0xff000000\\c\xB7\xCE \xC8\xAF\xBA\xD2\xB9\xDE\xC0\xBB \xBC\xF6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.\n\xC4\xED\xC5\xB0\xB7\xCE \xC0\xFC\xC8\xAF\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", (pItem->m_discountPrice > 0 && pItem->m_discountPrice < pItem->m_price) ? pItem->m_discountPrice : pItem->m_price), false);
				pDlg->m_requestIds[0] = *(const unsigned long*)pGiftInfo;
				pDlg->m_requestIds[1] = pGiftInfo->m_typeId;

				pDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnReturnGiftDlgResult, 0);
				return;
			}
		}
		break;

	case 14:
		break;
	}

	m_pMoveGift = pGiftInfo;
	FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);
	// "Do you want to move the gift?"
	pForm->SetMessage("\xBC\xB1\xB9\xB0\xC0\xBB \xBF\xC5\xB1\xE2\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);
	pForm->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnMoveGiftDlgResult, 1);
}

// realmyroommainui.cpp:7367
bool CRealMyRoomMain::OnCardRemoveDlgResult(int result, FrForm* pForm) {
	m_pCardRemoveDlg = NULL;
	return true;
}

// realmyroommainui.cpp:7377
void CRealMyRoomMain::OpenTikiReport(sItemInfo* pItemInfo) {
	if (!IsLocalContent((localContentType_t)0x14)) {
		return;
	}

	if (pItemInfo == NULL) {
		return;
	}

	if (pItemInfo->m_serial == m_tikiReportSerial) {
		// "This is not a valid tiki report."
		this << MsgObject(0, 0x23, (intptr_t)"\xC0\xAF\xC8\xBF\xC7\xCF\xC1\xF6 \xBE\xCA\xC0\xBA \xC6\xBC\xC5\xB0 \xB8\xAE\xC6\xF7\xC6\xAE \xC0\xD4\xB4\xCF\xB4\xD9. ", 0, 0, 0, 0);
		return;
	}

	m_tikiReportSerial = pItemInfo->m_serial;
	m_tikiReportTypeId = pItemInfo->m_typeId;

	m_tikiReportRecord = (pItemInfo->m_clubStatLevel[1] << 15) + pItemInfo->m_clubStatLevel[2];

	FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);
	// "Open the tiki report of the 30-player tournament record?"
	pForm->SetMessage("30\xC0\xCE \xB4\xEB\xC8\xB8 \xB1\xE2\xB7\xCF\xC0\xCE \xC6\xBC\xC5\xB0 \xB8\xAE\xC6\xF7\xC6\xAE\xB8\xA6 \xBF\xAD\xB6\xF7 \xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);
	pForm->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnOpenTikiReport, 1);
}

// realmyroommainui.cpp:7402
void OnOpenedBoxDlg(unsigned long typeId, FRESH_RESULT_CALLBACK pfnResult, FrCmdTarget* pOwner) {
	FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), pOwner, "notify_yesno", 0);
	pForm->SetMessage("\xBC\xB1\xC5\xC3\xC7\xD1 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xBF\xAD\xBE\xEE \xBA\xB8\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);

	pForm->Open(pfnResult, 1);
}

// realmyroommainui.cpp:7411
void CRealMyRoomMain::OpenNewYearMoney(sItemInfo* pItemInfo) {
	if (pItemInfo != NULL) {
		m_newYearMoneySerial = pItemInfo->m_serial;
		OnOpenedBoxDlg(pItemInfo->m_typeId, (FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnOpenNewYearMoneyDlgResult, this);
	}
}

// realmyroommainui.cpp:7420
void CRealMyRoomMain::OpenEventGiftBox(sItemInfo* pItemInfo) {
	if (pItemInfo != NULL) {
		CGiftBoxEvent* pEvent = NULL;
		if (!WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0x1D, pEvent)) {
			return;
		}

		m_eventGiftBoxSerial = pItemInfo->m_serial;

		pEvent->SetOpenedBoxTID(pItemInfo->m_typeId);

		if (pEvent->GetOpenedBoxTID() != 0) {
			OnOpenedBoxDlg(pItemInfo->m_typeId, (FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnOpenEventGiftDlgResult, this);
		}
	}
}

// realmyroommainui.cpp:7441
void CRealMyRoomMain::OpenS4EventGiftBox(sItemInfo* pItemInfo) {
	if (pItemInfo != NULL) {
		CGiftBoxEvent* pEvent = (CGiftBoxEvent*)WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0x1D);
		if (pEvent == NULL) {
			return;
		}

		m_s4EventGiftBoxTypeId = pItemInfo->m_typeId;

		pEvent->SetOpenedBoxTID(pItemInfo->m_typeId);

		if (pEvent->GetOpenedBoxTID() != 0) {
			OnOpenedBoxDlg(pItemInfo->m_typeId, (FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnOpenS4EventGiftDlgResult, this);
		}
	}
}

// realmyroommainui.cpp:7462
void CRealMyRoomMain::OnItemListBtnDown(void) {
	// realmyroommainui.cpp:7463
	FrListItem *pItem = m_pUccCopyHideWnd->GetItemUnderCursor();
	// realmyroommainui.cpp:7464
	if (pItem == NULL)
		return;

	// realmyroommainui.cpp:7469
	if (m_itemStorageVisible ? m_petLoader.IsWorking(0) : (m_petLoader.IsWorking(0) || m_roomMode == 0x16)) {
		// realmyroommainui.cpp:7516
		m_pUccCopyHideWnd->ToggleItem(pItem);
		return;
	}

	// realmyroommainui.cpp:7478
	if (IsLocalContent((localContentType_t)0x71) == true && m_itemStorageVisible == true) {
		// realmyroommainui.cpp:7481
		m_pUccCopyHideWnd->ToggleItem(pItem);
		// realmyroommainui.cpp:7484
		CItemStorage *pStorage = CItemStorage::m_pInstance;
		// realmyroommainui.cpp:7485
		if (pStorage == NULL)
			return;
		// realmyroommainui.cpp:7487
		sStoredItemInfo stored;
		// realmyroommainui.cpp:7488
		const sItemInfo *pInfo = (const sItemInfo *)pItem->pData;
		// realmyroommainui.cpp:7490
		if (pStorage->CheckStoredItemInfo(stored, *pInfo) != true)
			return;
		// realmyroommainui.cpp:7495
		sStoredItemDragDrop dragDrop;
		memset(&dragDrop, 0, sizeof(dragDrop));
		// realmyroommainui.cpp:7496
		dragDrop.mode = sStoredItemDragDrop::MODE_NONE;
		// realmyroommainui.cpp:7498
		dragDrop.index = pItem->idx;
		dragDrop.pItemInfo = pInfo;
		dragDrop.slot = stored;
		// realmyroommainui.cpp:7499
		memcpy(dragDrop.slot.item.uccIdx, pInfo->m_uccKey, sizeof(dragDrop.slot.item.uccIdx));
		// realmyroommainui.cpp:7501
		pStorage->SetDragDropItem(dragDrop);
		return;
	}

	// realmyroommainui.cpp:7507
	if (m_mainTab == 0) {
		// realmyroommainui.cpp:7514
		if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar == NULL || WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->GetPetFrame() == NULL) {
			// realmyroommainui.cpp:7516
			m_pUccCopyHideWnd->ToggleItem(pItem);
			return;
		}
	}

	// realmyroommainui.cpp:7522
	const WPoint &mousePos = g_pFresh->GetManager()->GetMousePos();
	// realmyroommainui.cpp:7523
	switch (m_roomMode) {
	case 0x11:
		// realmyroommainui.cpp:7527
		if (IsLocalContent((localContentType_t)0x60))
			break;
		// realmyroommainui.cpp:7529
		if ((m_itemListArea[0].rect + pItem->pos).IsInRect(mousePos)) {
			// realmyroommainui.cpp:7531
			sGiftInfo *pGift = (sGiftInfo *)pItem->pData;
			// realmyroommainui.cpp:7532
			if (pGift)
				// realmyroommainui.cpp:7533
				MoveGift(pGift);
		}
		// realmyroommainui.cpp:7536
		if ((m_itemListArea[4].rect + pItem->pos).IsInRect(mousePos)) {
			// realmyroommainui.cpp:7538
			sGiftInfo *pGift = (sGiftInfo *)pItem->pData;
			// realmyroommainui.cpp:7539
			if (pGift && pGift->m_sender[0] != '@') {
				// realmyroommainui.cpp:7541
				m_pNoteDlg = CreateForm<FrNoteDlg>(g_pFresh->GetManager(), this, "note", NULL);
				// realmyroommainui.cpp:7542
				m_pNoteDlg->SetNick(pGift->m_sender, pGift->m_message);
				// realmyroommainui.cpp:7543
				m_pNoteDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnNoteDlgResult, 1);
			}
		}
		// realmyroommainui.cpp:7548
		break;

	case 8:
		// realmyroommainui.cpp:7550
		if ((m_itemListArea[1].rect + pItem->pos).IsInRect(mousePos)) {
			// realmyroommainui.cpp:7552
			sItemInfo *pInfo = (sItemInfo *)pItem->pData;
			// realmyroommainui.cpp:7553
			if (pInfo) {
				// realmyroommainui.cpp:7554
				EquipItem(pInfo);
				return;
			}
		}
		// realmyroommainui.cpp:7559
		if ((m_itemListArea[2].rect + pItem->pos).IsInRect(mousePos)) {
			// realmyroommainui.cpp:7561
			sItemInfo *pInfo = (sItemInfo *)pItem->pData;
			// realmyroommainui.cpp:7562
			if (pInfo) {
				// realmyroommainui.cpp:7563
				IFF_ITEM_COMMON *pCommon = ItemManager()->FindCommonItem(pInfo->m_typeId);
				// realmyroommainui.cpp:7564
				if (!(pCommon->m_flags0 & 1)) {
					int equipped = 0;
					// realmyroommainui.cpp:7569
					if (!(pInfo->m_typeId & 0x2000000)) {
						// realmyroommainui.cpp:7571
						for (int k = 0; k < 10; k++) {
							if (pInfo->m_typeId == WSingleton<CSharedDoc>::Instance()->m_equipItem[k])
								// realmyroommainui.cpp:7572
								equipped++;
						}
					}
					// realmyroommainui.cpp:7576
					int count = pInfo->m_count - equipped;
					// realmyroommainui.cpp:7578
					if (count > 0) {
						// realmyroommainui.cpp:7580
						FrDeleteItemDlg *pDlg = CreateForm<FrDeleteItemDlg>(g_pFresh->GetManager(), this, "delete", NULL);
						// realmyroommainui.cpp:7581
						pDlg->SetDelItem(pInfo->m_typeId, count);
						// realmyroommainui.cpp:7582
						pDlg->Open(NULL, 1);
					}
				}
			}
		}
		// realmyroommainui.cpp:7587
		break;

	case 0xA:
		// realmyroommainui.cpp:7589
		if ((m_itemListArea[3].rect + pItem->pos).IsInRect(mousePos)) {
			// realmyroommainui.cpp:7591
			sCaddieInfo *pCaddie = (sCaddieInfo *)pItem->pData;
			// realmyroommainui.cpp:7592
			if (pCaddie) {
				// realmyroommainui.cpp:7594
				IFF_STRUCT::sCaddie *pIff = ItemManager()->FindCaddie(pCaddie->typeId);
				// realmyroommainui.cpp:7595
				if ((pCaddie->flags & 2) && pCaddie->days <= 0 && pCaddie->cadItemTypeId == 0 && pIff->fee)
					// realmyroommainui.cpp:7600
					OpenCaddieInfo(*pCaddie);
			}
		}
		// realmyroommainui.cpp:7604
		break;

	case 9: {
		// realmyroommainui.cpp:7608
		sItemInfo *pInfo = (sItemInfo *)pItem->pData;
		// realmyroommainui.cpp:7612
		bool handled = false;
		GetActor("ItemBuff") << MsgObject(0, 0x246, 1, (intptr_t)&handled, (intptr_t)pItem, (intptr_t)&mousePos, pInfo->m_typeId);
		// realmyroommainui.cpp:7613
		if (handled)
			break;
		// realmyroommainui.cpp:7617
		if ((m_itemListArea[0].rect + pItem->pos).IsInRect(mousePos))
			// realmyroommainui.cpp:7619
			ExecuteMiniButtonDown(pInfo);
		// realmyroommainui.cpp:7622
		break;
	}

	case 0x18: {
		// realmyroommainui.cpp:7638
		sItemInfo *pInfo = (sItemInfo *)pItem->pData;
		// realmyroommainui.cpp:7640
		if (pInfo && (m_itemListArea[0].rect + pItem->pos).IsInRect(mousePos)) {
			// realmyroommainui.cpp:7642
			if (pInfo->m_typeId == 0x1A000042)
				// realmyroommainui.cpp:7644
				OpenTikiReport(pInfo);
		}
		// realmyroommainui.cpp:7648
		break;
	}

	case 0x17:
		// realmyroommainui.cpp:7652
		if (IsLocalContent((localContentType_t)0x4F)) {
			// realmyroommainui.cpp:7654
			if ((m_itemListArea[7].rect + pItem->pos).IsInRect(mousePos)) {
				// realmyroommainui.cpp:7656
				sReplayFileInfo *pReplay = (sReplayFileInfo *)pItem->pData;
				// realmyroommainui.cpp:7658, byte 0xDC of the record (inside the
				// 0x24 bytes sReplayFileInfo gives m_fileName): 1 when it can be played
				if (((const unsigned char *)pReplay)[0xDC] == 1) {
					// realmyroommainui.cpp:7660
					if (m_pReplayDelDlg == NULL) {
						// realmyroommainui.cpp:7662
						m_pReplayDelDlg = CreateForm<FrReplayModeDlg>(g_pFresh->GetManager(), this, "replaymode", NULL);
						// realmyroommainui.cpp:7663
						WSingleton<CSharedDoc>::Instance()->SetFileName(pReplay->m_fileName);
						// realmyroommainui.cpp:7664
						m_pReplayDelDlg->SetReplayMode(1);
						// realmyroommainui.cpp:7665
						m_pReplayDelDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnReplayModeOpenDlgResult, 1);
					}
					// realmyroommainui.cpp:7668
				} else {
					// realmyroommainui.cpp:7669
					this << MsgObject(0, 0x23, (intptr_t)"\xB9\xF6\xC1\xAF\xC0\xCC \xB3\xB7\xBE\xC6\xBC\xAD \xB8\xAE\xC7\xC3\xB7\xB9\xC0\xCC \xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
				}
			}
			// realmyroommainui.cpp:7672
			if ((m_itemListArea[8].rect + pItem->pos).IsInRect(mousePos)) {
				// realmyroommainui.cpp:7676
				sReplayFileInfo *pReplay = (sReplayFileInfo *)pItem->pData;
				if (m_pReplayDelDlg == NULL) {
					// realmyroommainui.cpp:7678
					m_pReplayDelDlg = CreateForm<FrReplayModeDlg>(g_pFresh->GetManager(), this, "replaytape_del", NULL);
					// realmyroommainui.cpp:7679
					WSingleton<CSharedDoc>::Instance()->SetFileName(pReplay->m_fileName);
					// realmyroommainui.cpp:7680
					m_pReplayDelDlg->SetReplayMode(2);
					// realmyroommainui.cpp:7681
					m_pReplayDelDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnReplayDelOpenDlgResult, 1);
				}
			}
		}
		// realmyroommainui.cpp:7686
		break;

	case 0x1B:
		// realmyroommainui.cpp:7690
		if (IsLocalContent((localContentType_t)0x51)) {
			// realmyroommainui.cpp:7692
			sCards *pCard = (sCards *)pItem->pData;
			// realmyroommainui.cpp:7693
			m_cardTypeId = pCard->m_typeId;
			// realmyroommainui.cpp:7695
			if ((m_itemListArea[9].rect + pItem->pos).IsInRect(mousePos)) {
				// realmyroommainui.cpp:7697
				IFF_STRUCT::sCard *pIff = ItemManager()->FindCard(pCard->m_typeId);
				// realmyroommainui.cpp:7698
				if (pIff == NULL) {
					// realmyroommainui.cpp:7700
					this << MsgObject(0, 0x23, (intptr_t)"\xC0\xAF\xC8\xBF\xC7\xD1 \xC4\xAB\xB5\xE5\xB0\xA1 \xBE\xC6\xB4\xCF\xB9\xC7\xB7\xCE \xBB\xE7\xBF\xEB\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
					// realmyroommainui.cpp:7702
					break;
				}
				// realmyroommainui.cpp:7704
				if (pIff->m_active == 0) {
					// realmyroommainui.cpp:7706
					this << MsgObject(0, 0x23, (intptr_t)"\xC0\xAF\xC8\xBF\xC7\xD1 \xC4\xAB\xB5\xE5\xB0\xA1 \xBE\xC6\xB4\xCF\xB9\xC7\xB7\xCE \xBB\xE7\xBF\xEB\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
					// realmyroommainui.cpp:7708
					break;
				}
				// realmyroommainui.cpp:7711
				if (((pCard->m_typeId >> 22) & 0xF) == 3) {
					// realmyroommainui.cpp:7713
					m_pOpenCardPackDlg = CreateForm<FrOpenCardPackDlg>(g_pFresh->GetManager(), this, "cardpack_open", NULL);
					// realmyroommainui.cpp:7714
					m_pOpenCardPackDlg->m_cardId = pCard->m_guid;
					// realmyroommainui.cpp:7715
					m_pOpenCardPackDlg->m_cardTypeId = pCard->m_typeId;
					// realmyroommainui.cpp:7716
					m_pOpenCardPackDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnCardPackDlgResult, 1);
					// realmyroommainui.cpp:7718
					break;
				}
				// realmyroommainui.cpp:7721
				bool used = false;
				if (((pCard->m_typeId >> 22) & 0xF) == 2) {
					// realmyroommainui.cpp:7723
					for (std::list<sSCardAvilityPeriodInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_cardPeriodList[0].begin(); it != WSingleton<CSharedDoc>::Instance()->m_cardPeriodList[0].end(); ++it) {
						// realmyroommainui.cpp:7725
						if (it->kind == 2) {
							// realmyroommainui.cpp:7727
							IFF_STRUCT::sCard *pUsed = ItemManager()->FindCard(pCard->m_typeId);
							// realmyroommainui.cpp:7728
							if (it->effect == pUsed->m_effectType) {
								// realmyroommainui.cpp:7730
								this << MsgObject(0, 0x23, (intptr_t)"\xC0\xCC \xC4\xAB\xB5\xE5\xC0\xC7 \xC8\xBF\xB0\xFA\xB4\xC2 \xC0\xCC\xB9\xCC \xC0\xFB\xBF\xEB\xB5\xC7\xBE\xEE \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
								// realmyroommainui.cpp:7731
								used = true;
							}
						}
					}
					// realmyroommainui.cpp:7737
					if (used)
						break;
				}
				// realmyroommainui.cpp:7739
				m_pCardUseDlg = CreateForm<FrCardUseDlg>(g_pFresh->GetManager(), this, "usecard", NULL);
				// realmyroommainui.cpp:7740
				m_pCardUseDlg->m_cardId = pCard->m_guid;
				// realmyroommainui.cpp:7741
				m_pCardUseDlg->m_cardTypeId = pCard->m_typeId;
				// realmyroommainui.cpp:7742
				m_pCardUseDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnCardDlgResult, 1);
			}
		}
		// realmyroommainui.cpp:7750
		break;

	case 0x1C:
	case 0x1D:
	case 0x1E:
	case 0x1F:
	case 0x20:
		// realmyroommainui.cpp:7757
		if (IsLocalContent((localContentType_t)0x56)) {
			// realmyroommainui.cpp:7761
			if (pItem->pData && (m_itemListArea[10].rect + pItem->pos).IsInRect(mousePos)) {
				// realmyroommainui.cpp:7764
				if (!m_flags[3])
					// realmyroommainui.cpp:7765
					ControlUiHideMode(4);
				// realmyroommainui.cpp:7766
				else
					// realmyroommainui.cpp:7767
					ControlUiHideMode(2);
			}
		}
		break;
	}

	// realmyroommainui.cpp:7776
	if (IsLocalContent((localContentType_t)0x57)) {
		// realmyroommainui.cpp:7778
		if (m_roomMode == 0x15 || m_roomMode == 0x1A) {
			// realmyroommainui.cpp:7780
			sItemInfo *pInfo = (sItemInfo *)pItem->pData;
			// realmyroommainui.cpp:7782
			if (pInfo) {
				// realmyroommainui.cpp:7784
				IFF_STRUCT::sPart *pPart = ItemManager()->FindPart(pInfo->m_typeId);
				// realmyroommainui.cpp:7787
				if (pPart) {
					if (pPart->slot == 7 || pPart->slot == 8) {
						// realmyroommainui.cpp:7791
						if (!(pInfo->m_uccStatus & 1) && (m_uccDrawRects[1] + pItem->pos).IsInRect(mousePos) && IsUccClothes(pInfo->m_typeId)) {
							// realmyroommainui.cpp:7793
							if (m_pUccDrawDlg == NULL) {
								// realmyroommainui.cpp:7795
								CIconToolTip::GetInstance()->ClearToolTip();
								// realmyroommainui.cpp:7797
								m_pUccDrawDlg = CreateForm<FrUccDrawDlg>(g_pFresh->GetManager(), this, "ucc_draw", NULL);
								// realmyroommainui.cpp:7798
								m_pUccDrawDlg->SetUccDrawInfo((eDrawMode)0, pInfo, false);
								// realmyroommainui.cpp:7799
								if (m_pUccDrawDlg->m_initialized) {
									// realmyroommainui.cpp:7801
									m_pUccDrawDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnUccDrawDlgResult, WPoint(373.0f, 72.0f), 1);
									// realmyroommainui.cpp:7803
									unsigned long tidChar = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList().m_tidChar;
									// realmyroommainui.cpp:7804
									ShowPet(tidChar);
									// realmyroommainui.cpp:7806
									m_pUccCopyHideWnd->SetVisible(false);
									// realmyroommainui.cpp:7808
								} else {
									// realmyroommainui.cpp:7810
									this << MsgObject(0, 0x23, (intptr_t)MakeStr("\xBC\xAD\xB9\xF6\xB7\xCE\xBA\xCE\xC5\xCD \xC7\xCA\xBF\xE4\xC7\xD1 \xC1\xA4\xBA\xB8\xB8\xA6 \xBE\xF2\xC1\xF6 \xB8\xF8\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9(\xC4\xDA\xB5\xE5:%d)", 2), 0, 0, 0, 0);
								}
							}
						}
					}
					// realmyroommainui.cpp:7817
					if (pPart->slot == 7 || pPart->slot == 9) {
						// realmyroommainui.cpp:7820
						int rect = pPart->slot != 7;
						// realmyroommainui.cpp:7825
						if (!(pInfo->m_uccStatus & 1) && (m_uccDrawRects[rect] + pItem->pos).IsInRect(mousePos) && IsUccClothes(pInfo->m_typeId)) {
							// realmyroommainui.cpp:7827
							if (m_pUccCopyDlg == NULL) {
								// realmyroommainui.cpp:7829
								CIconToolTip::GetInstance()->ClearToolTip();
								// realmyroommainui.cpp:7830
								m_pUccCopyHideWnd->SetVisible(false);
								// realmyroommainui.cpp:7831
								m_pUccCopyDlg = CreateForm<FrUccCopyDlg>(g_pFresh->GetManager(), this, "ucc_copy", NULL);
								// realmyroommainui.cpp:7832
								m_pUccCopyDlg->SetItem(pInfo);
								// realmyroommainui.cpp:7833
								m_pUccCopyDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnUccCopyDlgResult, WPoint(359.0f, 71.0f), 1);
							}
						}
					}
				}
				// realmyroommainui.cpp:7838
				if (m_pUccCopyHideWnd) {
					// realmyroommainui.cpp:7841
					if ((!(pInfo->m_uccStatus & 1) && IsUccClothes(pInfo->m_typeId))
						// realmyroommainui.cpp:7848
						|| (IsUccClothes(pInfo->m_typeId) && !UccManager()->IsDownloadComplete(pInfo->m_typeId, pInfo->m_uccKey))) {
						// realmyroommainui.cpp:7850
						m_pUccCopyHideWnd->UnselectItem(pItem);
						return;
					}
				}
			}
		}
	}

	// realmyroommainui.cpp:7904
	if (pItem->selected == false)
		// realmyroommainui.cpp:7906
		UnselectItemList(pItem);
	else
		// realmyroommainui.cpp:7908
		SelectItemList(pItem);
	// realmyroommainui.cpp:7910
}

// realmyroommainui.cpp:7913
void CRealMyRoomMain::OnItemListMouseMove(void) {
	if (m_pUccCopyHideWnd->GetItemUnderCursor() != NULL) {
		if (!(m_itemStorageVisible ? m_petLoader.IsWorking(0) : (m_petLoader.IsWorking(0) || m_roomMode == 22))) {
			if (IsLocalContent(static_cast<localContentType_t>(0x71)) == true) {
				CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
				if (pItemStorage) {
					pItemStorage->SetDrag(sStoredItemDragDrop::MODE_PUSH);
				}
			}
		}
	}
}

// realmyroommainui.cpp:7940
void CRealMyRoomMain::OnItemListRBtnDown(void) {
	if (m_pUccCopyHideWnd == NULL) {
		return;
	}

	FrListItem* pItem = m_pUccCopyHideWnd->GetItemUnderCursor();
	if (pItem == NULL) {
		return;
	}

	if (m_petLoader.IsWorking(0)) {
		m_pUccCopyHideWnd->ToggleItem(pItem);
		return;
	}

	if (m_mainTab == 0) {
		if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar == NULL
			|| WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->GetPetFrame() == NULL) {
			m_pUccCopyHideWnd->ToggleItem(pItem);
			return;
		}
	}

	if (m_itemStorageVisible) {
		m_pUccCopyHideWnd->ToggleItem(pItem);
		OpenInformation(pItem);
		return;
	}

	switch (m_roomMode) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 21:
	case 26: {
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;
		if (pInfo == NULL) {
			return;
		}

		if (pInfo->m_stateBits.kind == 6 && pInfo->m_stateBits.expired) {
			m_pUccCopyHideWnd->ToggleItem(pItem);
			OpenInformation(pItem);
			return;
		}
		break;
	}
	}

	if (m_roomMode == 9) {
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;
		if (pInfo == NULL) {
			return;
		}

		if (pInfo->m_typeId == 0x1A000004) {
			if (m_pFortuneDlg == NULL) {
				IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(0x1A000004);
				m_pFortuneDlg = CreateForm<FrFortuneDlg>(g_pFresh->GetManager(), this, "fortune", 0);
				m_pFortuneDlg->SetMessage(pDesc ? pDesc->m_text : NULL, false);
				m_pFortuneDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnFortuneDlgResult, 1);
			}
		} else {
			OpenInformation(pItem);
		}
	} else {
		OpenInformation(pItem);
	}

	if (m_roomMode != 22) {
		if (IsLocalContent((localContentType_t)0x57)) {
			sItemInfo* pInfo = (sItemInfo*)pItem->pData;
			if (IsUccClothes(pInfo->m_typeId)) {
				if (!(pInfo->m_uccStatus & 1) || !UccManager()->IsDownloadComplete(pInfo->m_typeId, pInfo->m_uccKey)) {
					m_pUccCopyHideWnd->ToggleItem(pItem);
					return;
				}
			}
		}

		if (pItem->selected) {
			SelectItemList(pItem);
		} else {
			m_pUccCopyHideWnd->ToggleItem(pItem);
		}
		return;
	}

	m_pUccCopyHideWnd->ToggleItem(pItem);
}

// realmyroommainui.cpp:8227
void CRealMyRoomMain::OnItemListDClick(void) {
	if (!IsLocalContent(static_cast<localContentType_t>(0x71)) || !m_itemStorageVisible) {
		FrListItem* pItem = m_pUccCopyHideWnd->GetItemUnderCursor();
		if (pItem == NULL) {
			return;
		}

		if (m_roomMode != 8) {
			return;
		}

		EquipItem((sItemInfo*)pItem->pData);
		return;
	}

	if (IsLocalContent(static_cast<localContentType_t>(0x71)) == true && m_itemStorageVisible == true) {
		CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
		if (pItemStorage == NULL) {
			return;
		}

		if (pItemStorage->IsLock()) {
			return;
		}

		FrListItem* pItem = m_pUccCopyHideWnd->GetItemUnderCursor();
		if (pItem == NULL) {
			return;
		}

		pItemStorage->ReqPushStoredItemInfo(*pItem);
	}
}

// realmyroommainui.cpp:8261
void CRealMyRoomMain::OnPowerEditInit(PG_MSGARG param) {
	m_pPowerEdit = DYNAMIC_CAST(FrStatic, param);
}

// realmyroommainui.cpp:8266
void CRealMyRoomMain::OnControlEditInit(PG_MSGARG param) {
	m_pControlEdit = DYNAMIC_CAST(FrStatic, param);
}

// realmyroommainui.cpp:8271
void CRealMyRoomMain::OnImpactEditInit(PG_MSGARG param) {
	m_pImpactEdit = DYNAMIC_CAST(FrStatic, param);
}

// realmyroommainui.cpp:8276
void CRealMyRoomMain::OnSpinEditInit(PG_MSGARG param) {
	m_pSpinEdit = DYNAMIC_CAST(FrStatic, param);
}

// realmyroommainui.cpp:8281
void CRealMyRoomMain::OnCurveEditInit(PG_MSGARG param) {
	m_pCurveEdit = DYNAMIC_CAST(FrStatic, param);
}

// realmyroommainui.cpp:8286
void CRealMyRoomMain::OnUpgradeInit(PG_MSGARG param) {
	m_pUpgradeButton = DYNAMIC_CAST(FrButton, param);
	m_pUpgradeButton->SetVisible(false);

	m_pUpgradeButton->EnableHover(true, 0.0f);
}

// realmyroommainui.cpp:8294
void CRealMyRoomMain::OnUpgradeUp(void) {
	IFF_STRUCT::sClubSet* pClubSet = WSingleton<CSharedDoc>::Instance()->m_itemManager.FindClubSet(m_clubTypeId);
	if (pClubSet != NULL) {
		// The level bit 7 flags a club set usable only up to its level.
		unsigned char level = pClubSet->m_level & 0x7f;
		if ((level > WSingleton<CSharedDoc>::Instance()->m_myLevel && !(pClubSet->m_level & 0x80)) || (level < WSingleton<CSharedDoc>::Instance()->m_myLevel && (pClubSet->m_level & 0x80))) {
			this << MsgObject(0, 0x23, (intptr_t)"\xB7\xB9\xBA\xA7\xC0\xCC \xC3\xE6\xBA\xD0\xC7\xCF\xC1\xF6 \xBE\xCA\xBD\xC0\xB4\xCF\xB4\xD9. \xB7\xB9\xBA\xA7\xBE\xF7 \xC8\xC4\xBF\xA1 \xC0\xCC\xBF\xEB\xC7\xCF\xBD\xCA\xBD\xC3\xBF\xC0.", 0, 0, 0, 0);
			return;
		}
	}

	FrDowngradeDlg* pDlg = CreateForm<FrDowngradeDlg>(g_pFresh->GetManager(), this, "downgrade", 0);
	m_pUpgradeDlg = pDlg;
	if (m_mainTab == 0) {
		pDlg->SetInfo(m_charTypeId, (char*)m_pCharLevels, (unsigned long*)m_pCharParts, (unsigned long*)m_pCharCards, 0);
	} else if (m_mainTab == 1) {
		pDlg->SetInfo(WSingleton<CSharedDoc>::Instance()->m_myClubId, 0);
	}

	m_pUpgradeDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnUpgradeDlgResult, 3);

	OnTooltipHoverOff(0);
}

// realmyroommainui.cpp:8322
void CRealMyRoomMain::OnDowngradeInit(PG_MSGARG param) {
	m_pDowngradeButton = DYNAMIC_CAST(FrButton, param);
	m_pDowngradeButton->SetVisible(false);

	m_pDowngradeButton->EnableHover(true, 0.0f);
}

// realmyroommainui.cpp:8330
void CRealMyRoomMain::OnDowngradeUp(void) {
	IFF_STRUCT::sClubSet* pClubSet = WSingleton<CSharedDoc>::Instance()->m_itemManager.FindClubSet(m_clubTypeId);
	if (pClubSet != NULL) {
		// The level bit 7 flags a club set usable only up to its level.
		unsigned char level = pClubSet->m_level & 0x7f;
		if ((level > WSingleton<CSharedDoc>::Instance()->m_myLevel && !(pClubSet->m_level & 0x80)) || (level < WSingleton<CSharedDoc>::Instance()->m_myLevel && (pClubSet->m_level & 0x80))) {
			this << MsgObject(0, 0x23, (intptr_t)"\xB7\xB9\xBA\xA7\xC0\xCC \xC3\xE6\xBA\xD0\xC7\xCF\xC1\xF6 \xBE\xCA\xBD\xC0\xB4\xCF\xB4\xD9. \xB7\xB9\xBA\xA7\xBE\xF7 \xC8\xC4\xBF\xA1 \xC0\xCC\xBF\xEB\xC7\xCF\xBD\xCA\xBD\xC3\xBF\xC0.", 0, 0, 0, 0);
			return;
		}
	}

	FrDowngradeDlg* pDlg = CreateForm<FrDowngradeDlg>(g_pFresh->GetManager(), this, "downgrade", 0);
	m_pUpgradeDlg = pDlg;
	if (m_mainTab == 0) {
		pDlg->SetInfo(m_charTypeId, (char*)m_pCharLevels, (unsigned long*)m_pCharParts, (unsigned long*)m_pCharCards, 1);
	} else if (m_mainTab == 1) {
		pDlg->SetInfo(WSingleton<CSharedDoc>::Instance()->m_myClubId, 1);
	}

	m_pUpgradeDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnUpgradeDlgResult, 3);

	OnTooltipHoverOff(0);
}

// realmyroommainui.cpp:8357
void CRealMyRoomMain::OnTooltipHoverOn(PG_MSGARG param) {
	FrButton* pButton = (FrButton*)((__rtti_obj = (void*)param) != 0 ? (((IObject*)__rtti_obj)->IsKindOf(&FrButton::m_RTTI) ? (void*)param : 0) : 0);
	if (pButton == m_pUpgradeButton) {
		CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x23);
	} else if (pButton == m_pDowngradeButton) {
		CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x24);
	} else if (pButton == m_pEquipCancelButton) {
		CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x25);
	} else if (pButton == m_pEquipResetButton) {
		CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x26);
	}
}

// realmyroommainui.cpp:8378
void CRealMyRoomMain::OnTooltipHoverOff(PG_MSGARG param) {
	FrButton* pButton = (FrButton*)((__rtti_obj = (void*)param) != 0 ? (((IObject*)__rtti_obj)->IsKindOf(&FrButton::m_RTTI) ? (void*)param : 0) : 0);
	int tipIndex = CIconToolTip::GetInstance()->GetCurTipIndex();

	bool isOwnTip = false;
	switch (tipIndex) {
	case 0x23:
		if (pButton == m_pUpgradeButton) {
			isOwnTip = true;
		}
		break;
	case 0x24:
		if (pButton == m_pDowngradeButton) {
			isOwnTip = true;
		}
		break;
	case 0x25:
		if (pButton == m_pEquipCancelButton) {
			isOwnTip = true;
		}
		break;
	case 0x26:
		if (pButton == m_pEquipResetButton) {
			isOwnTip = true;
		}
		break;
	}

	if (pButton == NULL || isOwnTip) {
		CIconToolTip::GetInstance()->ClearToolTip();
	}
}

// realmyroommainui.cpp:8430
void CRealMyRoomMain::GetTotalStatus(unsigned char* const status, unsigned char* const slotStatus, unsigned char* const cardStatus) {
	if (IsLocalContent(static_cast<localContentType_t>(0x51))) {
		WSingleton<CCardManager>::Instance()->SetPlayerIndex(0xFF);
		WSingleton<CCardManager>::Instance()->CalcCardPeriodAndStatus();
	}

	for (int i = 0; i < 5; i++) {
		status_t level = ItemManager()->GetLevelWithPenalty((eLvlType)i, m_charTypeId, m_pCharLevels, m_pCharParts, m_pCharCards, m_clubTypeId, m_pClubLevels, m_caddieTypeId, CSharedDoc::Instance()->m_myLevel);
		status[i] = level.m_level;
		cardStatus[i] = level.m_penalty;

		slotStatus[i] = ItemManager()->GetCapacity((eLvlType)i, m_charTypeId, m_pCharLevels, m_pCharParts, m_clubTypeId, m_pClubLevels, m_caddieTypeId, CSharedDoc::Instance()->m_myLevel);
	}
}

// realmyroommainui.cpp:8451
void CRealMyRoomMain::GetCharacterStatus(unsigned char* const status, unsigned char* const slotStatus) {
	if (IsLocalContent(static_cast<localContentType_t>(0x51))) {
		WSingleton<CCardManager>::Instance()->SetPlayerIndex(0xFF);
		WSingleton<CCardManager>::Instance()->CalcCardPeriodAndStatus();
	}

	IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(m_charTypeId);
	if (pChar == NULL) {
		return;
	}

	for (int i = 0; i < 5; i++) {
		unsigned char itemLevel = ItemManager()->GetItemLevel((eLvlType)i, m_pCharParts, m_pCharCards);
		slotStatus[i] = itemLevel + ItemManager()->GetCharCapacity((eLvlType)i, m_charTypeId, m_pCharParts, m_pCharCards, CSharedDoc::Instance()->m_myLevel);
		status[i] = itemLevel + ItemManager()->GetCharLevel((eLvlType)i, m_charTypeId, m_pCharLevels);
	}
}

// realmyroommainui.cpp:8472
void CRealMyRoomMain::GetCaddieStatus(unsigned char* const pStatus, unsigned char* const pUnused) {
	IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(m_caddieTypeId);
	if (pCaddie) {
		for (int i = 0; i < 5; i++) pStatus[i] += (unsigned char)pCaddie->m_levels[i];
	}
}

// realmyroommainui.cpp:8483
void CRealMyRoomMain::GetClubSetStatus(unsigned char* const status, unsigned char* const slotStatus) {
	unsigned int serial = m_clubSerial;
	std::map<unsigned int, sItemInfo>::iterator it = CSharedDoc::Instance()->m_myItemMap.find(serial);
	if (it != CSharedDoc::Instance()->m_myItemMap.end()) {
		for (int i = 0; i < 5; i++) {
			status[i] = (unsigned char)it->second.m_clubStatLevel[i];
		}
	}

	IFF_STRUCT::sClubSet* pClubSet = ItemManager()->FindClubSet(m_clubTypeId);
	if (pClubSet) {
		for (int i = 0; i < 5; i++) {
			slotStatus[i] += (unsigned char)pClubSet->m_maxStat[i];
			status[i] += (unsigned char)pClubSet->m_baseStat[i];
		}
	}
}

// realmyroommainui.cpp:8559
void CRealMyRoomMain::BuildDisplayStand(subTab_t tab) {
	// realmyroommainui.cpp:8560
	if (m_pUccCopyHideWnd == NULL)
		return;

	// realmyroommainui.cpp:8563
	m_pUccCopyHideWnd->ClearItem();

	// realmyroommainui.cpp:8565
	switch (tab) {
	case 4: {
		// realmyroommainui.cpp:8568
		unsigned long category = GetCategoryFlag(tab);
		// realmyroommainui.cpp:8571
		for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myPartsList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myPartsList.end(); ++it) {
			sItemInfo *pInfo = &*it;
			// realmyroommainui.cpp:8574
			if (S5::UnderCategory(m_charTypeId, pInfo->m_typeId, category)) {
				// realmyroommainui.cpp:8577
				if (IsAngelWing(pInfo->m_typeId)) {
					// realmyroommainui.cpp:8580
					if (!IsMannerPlayer() || WSingleton<CSharedDoc>::Instance()->m_myLevel < 0x15)
						// realmyroommainui.cpp:8582
						continue;
				}
				// realmyroommainui.cpp:8598
				m_pUccCopyHideWnd->AddItem(pInfo);
			}
		}

		// realmyroommainui.cpp:8603
		for (int i = 0; i < 24; i++) {
			// realmyroommainui.cpp:8605
			for (FrListBox::ITEM_LIST::iterator it = m_pUccCopyHideWnd->m_itemList.begin(); it != m_pUccCopyHideWnd->m_itemList.end(); ++it) {
				// realmyroommainui.cpp:8612
				FrListItem *pItem = *it;
				sItemInfo *pInfo = (sItemInfo *)pItem->pData;
				std::map<unsigned int, sCharacterInfo>::iterator cit = WSingleton<CSharedDoc>::Instance()->m_characterMap.find(WSingleton<CSharedDoc>::Instance()->m_myCharacterId);
				// realmyroommainui.cpp:8613
				if (cit == WSingleton<CSharedDoc>::Instance()->m_characterMap.end())
					continue;
				// realmyroommainui.cpp:8616
				int slot = (pInfo->m_typeId >> 13) & 0x1F;
				// realmyroommainui.cpp:8618
				if (cit->second.partIds[slot] == 0) {
					// realmyroommainui.cpp:8620
					if (pInfo->m_typeId != m_pCharParts[i])
						continue;
				} else {
					// realmyroommainui.cpp:8625
					if (pInfo->m_typeId != m_pCharParts[i] || pInfo->m_serial != m_localContentData[i])
						continue;
				}
				// realmyroommainui.cpp:8632
				IFF_STRUCT::sPart *pPart = ItemManager()->FindPart(pInfo->m_typeId);
				// realmyroommainui.cpp:8633
				if (pPart) {
					// realmyroommainui.cpp:8635
					pInfo->m_charCardSlotCount = pPart->m_charCardSlots;
					// realmyroommainui.cpp:8636
					pInfo->m_caddieCardSlotCount = pPart->m_caddieCardSlots;
				}
				// realmyroommainui.cpp:8639
				m_pUccCopyHideWnd->SelectItem(pItem, false);
				break;
			}
		}

		// realmyroommainui.cpp:8646
		for (std::list<sItemInfo>::iterator it2 = WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3.begin(); it2 != WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3.end(); ++it2) {
			// realmyroommainui.cpp:8648
			m_pUccCopyHideWnd->AddItem(&*it2);
		}

		// realmyroommainui.cpp:8652
		for (int j = 0; j < 5; j++) {
			// realmyroommainui.cpp:8653
			for (FrListBox::ITEM_LIST::iterator it = m_pUccCopyHideWnd->m_itemList.begin(); it != m_pUccCopyHideWnd->m_itemList.end(); ++it) {
				// realmyroommainui.cpp:8656
				FrListItem* pItem = *it;
				if (((sItemInfo *)pItem->pData)->m_typeId == m_pCharCards[j]) {
					// realmyroommainui.cpp:8657
					m_pUccCopyHideWnd->SelectItem(pItem, false);
					break;
				}
			}
		}
		// realmyroommainui.cpp:8663
		break;
	}

	case 1:
	case 2:
	case 3: {
		// realmyroommainui.cpp:8668
		unsigned long category = GetCategoryFlag(tab);
		// realmyroommainui.cpp:8670
		for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myPartsList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myPartsList.end(); ++it) {
			sItemInfo *pInfo = &*it;
			// realmyroommainui.cpp:8673
			if (S5::UnderCategory(m_charTypeId, pInfo->m_typeId, category)) {
				// realmyroommainui.cpp:8675
				IFF_STRUCT::sPart *pPart = ItemManager()->FindPart(pInfo->m_typeId);
				// realmyroommainui.cpp:8676
				if (pPart) {
					// realmyroommainui.cpp:8678
					pInfo->m_charCardSlotCount = pPart->m_charCardSlots;
					// realmyroommainui.cpp:8679
					pInfo->m_caddieCardSlotCount = pPart->m_caddieCardSlots;
				}
				// realmyroommainui.cpp:8681
				m_pUccCopyHideWnd->AddItem(pInfo);
			}
		}

		// realmyroommainui.cpp:8686
		for (int i = 0; i < 24; i++) {
			// realmyroommainui.cpp:8688
			for (FrListBox::ITEM_LIST::iterator it = m_pUccCopyHideWnd->m_itemList.begin(); it != m_pUccCopyHideWnd->m_itemList.end(); ++it) {
				// realmyroommainui.cpp:8694
				FrListItem *pItem = *it;
				sItemInfo *pInfo = (sItemInfo *)pItem->pData;
				std::map<unsigned int, sCharacterInfo>::iterator cit = WSingleton<CSharedDoc>::Instance()->m_characterMap.find(WSingleton<CSharedDoc>::Instance()->m_myCharacterId);
				// realmyroommainui.cpp:8695
				if (cit == WSingleton<CSharedDoc>::Instance()->m_characterMap.end())
					continue;
				// realmyroommainui.cpp:8698
				int slot = (pInfo->m_typeId >> 13) & 0x1F;
				// realmyroommainui.cpp:8699
				if (cit->second.partIds[slot] == 0) {
					// realmyroommainui.cpp:8701
					if (pInfo->m_typeId != m_pCharParts[i])
						continue;
				} else {
					// realmyroommainui.cpp:8706
					if (pInfo->m_typeId != m_pCharParts[i] || pInfo->m_serial != m_localContentData[i])
						continue;
				}
				// realmyroommainui.cpp:8713
				m_pUccCopyHideWnd->SelectItem(pItem, false);
				break;
			}
		}
		// realmyroommainui.cpp:8719
		break;
	}

	case 0x1A: {
		// realmyroommainui.cpp:8723
		unsigned long category = GetCategoryFlag(tab);
		// realmyroommainui.cpp:8725
		for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myItemList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myItemList.end(); ++it) {
			sItemInfo *pInfo = &*it;
			// realmyroommainui.cpp:8728
			if (S5::UnderCategory(m_charTypeId, pInfo->m_typeId, category)) {
				// realmyroommainui.cpp:8730
				if (IsLocalContent((localContentType_t)0x51)) {
					// realmyroommainui.cpp:8732
					IFF_STRUCT::sPart *pPart = ItemManager()->FindPart(pInfo->m_typeId);
					// realmyroommainui.cpp:8733
					if (pPart) {
						// realmyroommainui.cpp:8735
						pInfo->m_charCardSlotCount = pPart->m_charCardSlots;
						// realmyroommainui.cpp:8736
						pInfo->m_caddieCardSlotCount = pPart->m_caddieCardSlots;
					}
				}
				// realmyroommainui.cpp:8739
				m_pUccCopyHideWnd->AddItem(pInfo);
			}
		}

		// realmyroommainui.cpp:8744
		for (int i = 0; i < 24; i++) {
			// realmyroommainui.cpp:8745
			for (FrListBox::ITEM_LIST::iterator it = m_pUccCopyHideWnd->m_itemList.begin(); it != m_pUccCopyHideWnd->m_itemList.end(); ++it) {
				// realmyroommainui.cpp:8746
				FrListItem *pItem = *it;
				// realmyroommainui.cpp:8747
				sItemInfo *pInfo = (sItemInfo *)pItem->pData;
				// realmyroommainui.cpp:8749
				if (IsLocalContent((localContentType_t)0x51)) {
					// realmyroommainui.cpp:8753
					std::map<unsigned int, sCharacterInfo>::iterator cit = WSingleton<CSharedDoc>::Instance()->m_characterMap.find(WSingleton<CSharedDoc>::Instance()->m_myCharacterId);
					// realmyroommainui.cpp:8754
					if (cit == WSingleton<CSharedDoc>::Instance()->m_characterMap.end())
						continue;
					// realmyroommainui.cpp:8757
					int slot = (pInfo->m_typeId >> 13) & 0x1F;
					// realmyroommainui.cpp:8758
					if (cit->second.partIds[slot] == 0) {
						// realmyroommainui.cpp:8760
						if (pInfo->m_typeId != m_pCharParts[i])
							continue;
					} else {
						// realmyroommainui.cpp:8765
						if (pInfo->m_typeId != m_pCharParts[i] || pInfo->m_serial != m_localContentData[i])
							// realmyroommainui.cpp:8776
							continue;
					}
					// realmyroommainui.cpp:8772
					m_pUccCopyHideWnd->SelectItem(pItem, false);
					// realmyroommainui.cpp:8773
					break;
				}
				// realmyroommainui.cpp:8778
				if (pInfo->m_typeId == m_pCharParts[i]) {
					// realmyroommainui.cpp:8780
					m_pUccCopyHideWnd->SelectItem(pItem, false);
					break;
				}
			}
		}
		// realmyroommainui.cpp:8787
		break;
	}

	case 0x15: {
		// realmyroommainui.cpp:8791
		unsigned long category = GetCategoryFlag(tab);
		// realmyroommainui.cpp:8793
		for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myPartsList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myPartsList.end(); ++it) {
			sItemInfo *pInfo = &*it;
			// realmyroommainui.cpp:8795
			if (S5::UnderCategory(m_charTypeId, pInfo->m_typeId, category)) {
				// realmyroommainui.cpp:8797
				if (IsLocalContent((localContentType_t)0x51)) {
					// realmyroommainui.cpp:8799
					IFF_STRUCT::sPart *pPart = ItemManager()->FindPart(pInfo->m_typeId);
					// realmyroommainui.cpp:8800
					if (pPart) {
						// realmyroommainui.cpp:8802
						pInfo->m_charCardSlotCount = pPart->m_charCardSlots;
						// realmyroommainui.cpp:8803
						pInfo->m_caddieCardSlotCount = pPart->m_caddieCardSlots;
					}
				}
				// realmyroommainui.cpp:8806
				m_pUccCopyHideWnd->AddItem(pInfo);
			}
		}

		// realmyroommainui.cpp:8811
		for (int i = 0; i < 24; i++) {
			// realmyroommainui.cpp:8813
			for (FrListBox::ITEM_LIST::iterator it = m_pUccCopyHideWnd->m_itemList.begin(); it != m_pUccCopyHideWnd->m_itemList.end(); ++it) {
				// realmyroommainui.cpp:8820
				FrListItem *pItem = *it;
				sItemInfo *pInfo = (sItemInfo *)pItem->pData;
				std::map<unsigned int, sCharacterInfo>::iterator cit = WSingleton<CSharedDoc>::Instance()->m_characterMap.find(WSingleton<CSharedDoc>::Instance()->m_myCharacterId);
				// realmyroommainui.cpp:8821
				if (cit == WSingleton<CSharedDoc>::Instance()->m_characterMap.end())
					continue;
				// realmyroommainui.cpp:8824
				int slot = (pInfo->m_typeId >> 13) & 0x1F;
				// realmyroommainui.cpp:8826
				if (cit->second.partIds[slot] == 0) {
					// realmyroommainui.cpp:8828
					if (pInfo->m_typeId != m_pCharParts[i])
						continue;
				} else {
					// realmyroommainui.cpp:8833
					if (pInfo->m_typeId != m_pCharParts[i] || pInfo->m_serial != m_localContentData[i])
						continue;
				}
				// realmyroommainui.cpp:8840
				m_pUccCopyHideWnd->SelectItem(pItem, false);
				break;
			}
		}

		// realmyroommainui.cpp:8847
		for (std::list<sItemInfo>::iterator it2 = WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3.begin(); it2 != WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3.end(); ++it2) {
			// realmyroommainui.cpp:8849
			m_pUccCopyHideWnd->AddItem(&*it2);
		}

		// realmyroommainui.cpp:8853
		for (int j = 0; j < 5; j++) {
			// realmyroommainui.cpp:8854
			for (FrListBox::ITEM_LIST::iterator it = m_pUccCopyHideWnd->m_itemList.begin(); it != m_pUccCopyHideWnd->m_itemList.end(); ++it) {
				// realmyroommainui.cpp:8855
				FrListItem *pItem = *it;
				// realmyroommainui.cpp:8856
				if (pItem && pItem->pData) {
					// realmyroommainui.cpp:8857
					if (((sItemInfo *)pItem->pData)->m_typeId == m_pCharCards[j]) {
						// realmyroommainui.cpp:8858
						m_pUccCopyHideWnd->SelectItem(pItem, false);
						break;
					}
				}
			}
		}

		// realmyroommainui.cpp:8865
		if (IsLocalContent((localContentType_t)0x57)) {
			// realmyroommainui.cpp:8867
			for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myItemList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myItemList.end(); ++it) {
				sItemInfo *pInfo = &*it;
				// realmyroommainui.cpp:8870
				if (S5::UnderCategory(m_charTypeId, pInfo->m_typeId, category)) {
					// realmyroommainui.cpp:8872
					if (IsLocalContent((localContentType_t)0x51)) {
						// realmyroommainui.cpp:8874
						IFF_STRUCT::sPart *pPart = ItemManager()->FindPart(pInfo->m_typeId);
						// realmyroommainui.cpp:8875
						if (pPart) {
							// realmyroommainui.cpp:8877
							pInfo->m_charCardSlotCount = pPart->m_charCardSlots;
							// realmyroommainui.cpp:8878
							pInfo->m_caddieCardSlotCount = pPart->m_caddieCardSlots;
						}
					}
					// realmyroommainui.cpp:8881
					m_pUccCopyHideWnd->AddItem(pInfo);
				}
			}

			// realmyroommainui.cpp:8886
			for (int i = 0; i < 24; i++) {
				// realmyroommainui.cpp:8887
				for (FrListBox::ITEM_LIST::iterator it = m_pUccCopyHideWnd->m_itemList.begin(); it != m_pUccCopyHideWnd->m_itemList.end(); ++it) {
					// realmyroommainui.cpp:8888
					FrListItem *pItem = *it;
					// realmyroommainui.cpp:8889
					sItemInfo *pInfo = (sItemInfo *)pItem->pData;
					// realmyroommainui.cpp:8890
					if (IsLocalContent((localContentType_t)0x51)) {
						// realmyroommainui.cpp:8894
						std::map<unsigned int, sCharacterInfo>::iterator cit = WSingleton<CSharedDoc>::Instance()->m_characterMap.find(WSingleton<CSharedDoc>::Instance()->m_myCharacterId);
						// realmyroommainui.cpp:8895
						if (cit == WSingleton<CSharedDoc>::Instance()->m_characterMap.end())
							continue;
						// realmyroommainui.cpp:8898
						int slot = (pInfo->m_typeId >> 13) & 0x1F;
						// realmyroommainui.cpp:8899
						if (cit->second.partIds[slot] == 0) {
							// realmyroommainui.cpp:8901
							if (pInfo->m_typeId != m_pCharParts[i])
								continue;
						} else {
							// realmyroommainui.cpp:8906
							if (pInfo->m_typeId != m_pCharParts[i] || pInfo->m_serial != m_localContentData[i])
								// realmyroommainui.cpp:8917
								continue;
						}
						// realmyroommainui.cpp:8913
						m_pUccCopyHideWnd->SelectItem(pItem, false);
						// realmyroommainui.cpp:8914
						break;
					}
					// realmyroommainui.cpp:8919
					if (pInfo->m_typeId == m_pCharParts[i]) {
						// realmyroommainui.cpp:8921
						m_pUccCopyHideWnd->SelectItem(pItem, false);
						break;
					}
				}
			}
		}
		// realmyroommainui.cpp:8929
		break;
	}

	case 0x16: {
		// realmyroommainui.cpp:8932
		unsigned long category = GetCategoryFlag(tab);
		// realmyroommainui.cpp:8934
		for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myPartsList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myPartsList.end(); ++it) {
			// realmyroommainui.cpp:8937
			if (it->m_typeId & 0x600)
				continue;
			// realmyroommainui.cpp:8943
			std::map<unsigned int, sCharacterInfo>::const_iterator cit;
			for (cit = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin(); cit != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++cit) {
				// realmyroommainui.cpp:8946
				if (((it->m_typeId >> 18) & 0xFF) == (cit->second.typeId & 0x3FFFFFF))
					break;
			}
			// realmyroommainui.cpp:8950
			if (cit != WSingleton<CSharedDoc>::Instance()->m_characterMap.end())
				continue;
			// realmyroommainui.cpp:8952
			if (IsLocalContent((localContentType_t)0x51)) {
				// realmyroommainui.cpp:8954
				IFF_STRUCT::sPart *pPart = ItemManager()->FindPart(it->m_typeId);
				// realmyroommainui.cpp:8955
				if (pPart) {
					// realmyroommainui.cpp:8957
					it->m_charCardSlotCount = pPart->m_charCardSlots;
					// realmyroommainui.cpp:8958
					it->m_caddieCardSlotCount = pPart->m_caddieCardSlots;
				}
			}
			// realmyroommainui.cpp:8961
			m_pUccCopyHideWnd->AddItem(&*it);
		}

		// realmyroommainui.cpp:8966
		if (IsLocalContent((localContentType_t)0x57)) {
			// realmyroommainui.cpp:8968
			for (std::list<sItemInfo>::iterator it2 = WSingleton<CSharedDoc>::Instance()->m_myItemList.begin(); it2 != WSingleton<CSharedDoc>::Instance()->m_myItemList.end(); ++it2) {
				// realmyroommainui.cpp:8970
				sItemInfo *pInfo = &*it2;
				// realmyroommainui.cpp:8971
				if (pInfo->m_typeId & 0x600)
					continue;
				// realmyroommainui.cpp:8977
				std::map<unsigned int, sCharacterInfo>::const_iterator cit;
				for (cit = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin(); cit != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++cit) {
					// realmyroommainui.cpp:8980
					if (((pInfo->m_typeId >> 18) & 0xFF) == (cit->second.typeId & 0x3FFFFFF))
						break;
				}
				// realmyroommainui.cpp:8984
				if (cit != WSingleton<CSharedDoc>::Instance()->m_characterMap.end())
					continue;
				// realmyroommainui.cpp:8986
				m_pUccCopyHideWnd->AddItem(pInfo);
			}
		}
		// realmyroommainui.cpp:8992
		break;
	}

	case 6:
		// realmyroommainui.cpp:8995
		for (std::map<unsigned int, sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myItemMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myItemMap.end(); ++it) {
			// realmyroommainui.cpp:8996
			FrListItem *pItem = m_pUccCopyHideWnd->AddItem(&it->second);
			// realmyroommainui.cpp:8998
			if (m_clubSerial == 0) {
				// realmyroommainui.cpp:9001
				if (WSingleton<CSharedDoc>::Instance()->m_myClubId == it->second.m_serial) {
					// realmyroommainui.cpp:9003
					m_clubSerial = it->second.m_serial;
					m_pUccCopyHideWnd->SelectItem(pItem, true);
				}
				// realmyroommainui.cpp:9007
			} else if (m_clubSerial == it->second.m_serial) {
				// realmyroommainui.cpp:9010
				m_pUccCopyHideWnd->SelectItem(pItem, true);
			}
		}
		// realmyroommainui.cpp:9014
		break;

	case 7:
		// realmyroommainui.cpp:9017
		for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myGearList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myGearList.end(); ++it) {
			// realmyroommainui.cpp:9018
			FrListItem *pItem = m_pUccCopyHideWnd->AddItem(&*it);
			// realmyroommainui.cpp:9019
			if (WSingleton<CSharedDoc>::Instance()->m_myBallId == it->m_typeId)
				// realmyroommainui.cpp:9020
				m_pUccCopyHideWnd->SelectItem(pItem, true);
		}
		// realmyroommainui.cpp:9024
		break;

	case 8: {
		// realmyroommainui.cpp:9030
		for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {
			sItemInfo *pInfo = &*it;
			// realmyroommainui.cpp:9034
			if (ItemManager()->GetCouponKind(pInfo->m_typeId))
				continue;
			// realmyroommainui.cpp:9038
			if (pInfo->m_typeId & 0x2000000)
				continue;

			int equipped = 0;
			// realmyroommainui.cpp:9043
			for (int k = 0; k < 10; k++) {
				if (pInfo->m_typeId == WSingleton<CSharedDoc>::Instance()->m_equipItem[k])
					// realmyroommainui.cpp:9044
					equipped++;
			}
			// realmyroommainui.cpp:9047
			if (pInfo->m_count - equipped > 0) {
				// realmyroommainui.cpp:9049
				FrListItem *pItem = m_pUccCopyHideWnd->AddItem(pInfo);
				// realmyroommainui.cpp:9050
				pItem->AddNoRBtnIdx(1);
				// realmyroommainui.cpp:9052
				IFF_ITEM_COMMON *pCommon = ItemManager()->FindCommonItem(pInfo->m_typeId);
				// realmyroommainui.cpp:9053
				if (pCommon && !(pCommon->m_flags0 & 1))
					// realmyroommainui.cpp:9054
					pItem->AddNoRBtnIdx(2);
			}
		}

		// realmyroommainui.cpp:9061
		if (m_pItemSlotList) {
			// realmyroommainui.cpp:9063
			m_pItemSlotList->ClearItem();
			// realmyroommainui.cpp:9064
			int slotCount = WSingleton<CSharedDoc>::Instance()->GetNumAddItemSlotByMascot() + 8;
			// realmyroommainui.cpp:9066
			if (IsLocalContent((localContentType_t)0x84)) {
				// realmyroommainui.cpp:9068
				if (IsLocalContent((localContentType_t)0x51)) {
					// realmyroommainui.cpp:9070
					CCardManager::Instance()->SetPlayerIndex(0xFF);
					// realmyroommainui.cpp:9071
					CCardManager::Instance()->CalcCardPeriodAndStatus();
					// realmyroommainui.cpp:9073
					int cardSlots = CCardManager::Instance()->GetCardPeriodSlot();
					// realmyroommainui.cpp:9075
					if (cardSlots > 0)
						// realmyroommainui.cpp:9076
						slotCount += cardSlots;
				}

				// realmyroommainui.cpp:9081
				for (int k = 0; k < 10; k++) {
					// realmyroommainui.cpp:9083
					if (k < slotCount)
						// realmyroommainui.cpp:9084
						m_pItemSlotList->AddItem(&WSingleton<CSharedDoc>::Instance()->m_equipItem[k]);
					// realmyroommainui.cpp:9085
					else
						// realmyroommainui.cpp:9091
						UnequipItem(&WSingleton<CSharedDoc>::Instance()->m_equipItem[k]);
				}

				// realmyroommainui.cpp:9096
				for (int n = m_pItemSlotList->GetCurrentItemSize(true); n < 10; n++)
					// realmyroommainui.cpp:9097
					m_pItemSlotList->AddItem(NULL);
				// realmyroommainui.cpp:9100
				break;
			}

			// realmyroommainui.cpp:9102
			if (IsLocalContent((localContentType_t)0x51)) {
				// realmyroommainui.cpp:9104
				CCardManager::Instance()->SetPlayerIndex(0xFF);
				// realmyroommainui.cpp:9105
				CCardManager::Instance()->CalcCardPeriodAndStatus();
				// realmyroommainui.cpp:9106
				slotCount += CCardManager::Instance()->GetCardPeriodSlot();
			}

			// realmyroommainui.cpp:9110
			for (int k = 0; k < slotCount; k++)
				// realmyroommainui.cpp:9111
				m_pItemSlotList->AddItem(&WSingleton<CSharedDoc>::Instance()->m_equipItem[k]);

			// realmyroommainui.cpp:9115
			for (int n = m_pItemSlotList->GetCurrentItemSize(true); n < 10; n++)
				// realmyroommainui.cpp:9116
				m_pItemSlotList->AddItem(NULL);
		}
		// realmyroommainui.cpp:9121
		break;
	}

	case 9:
		// realmyroommainui.cpp:9126
		for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {
			// realmyroommainui.cpp:9127
			sItemInfo *pInfo = &*it;
			// realmyroommainui.cpp:9130
			if (ItemManager()->GetCouponKind(pInfo->m_typeId))
				continue;
			// realmyroommainui.cpp:9153
			if (ItemManager()->IsNonVisibleItem(pInfo->m_typeId) == true)
				continue;
			// realmyroommainui.cpp:9163
			if (pInfo->m_typeId & 0x2000000) {
				// realmyroommainui.cpp:9165
				FrListItem *pItem = m_pUccCopyHideWnd->AddItem(pInfo);
				// realmyroommainui.cpp:9167
				if (CCodeTinker().IsNeedOpenMiniButton(pInfo->m_typeId))
					// realmyroommainui.cpp:9168
					pItem->AddNoRBtnIdx(0);
			}
		}
		// realmyroommainui.cpp:9172
		break;

	case 0x18:
		// realmyroommainui.cpp:9176
		if (IsLocalContent((localContentType_t)0x14)) {
			// realmyroommainui.cpp:9180
			for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {
				// realmyroommainui.cpp:9184
				if (it->m_typeId & 0x2000000) {
					// realmyroommainui.cpp:9186
					if (it->m_typeId == 0x1A000042) {
						// realmyroommainui.cpp:9188
						FrListItem *pItem = m_pUccCopyHideWnd->AddItem(&*it);
						// realmyroommainui.cpp:9189
						pItem->AddNoRBtnIdx(0);
					}
				}
			}
		}
		// realmyroommainui.cpp:9195
		break;

	case 0xD:
	case 0xE:
	case 0xF:
	case 0x10:
	case 0x22: {
		// realmyroommainui.cpp:9204
		unsigned long category = GetCategoryFlag(tab);
		// realmyroommainui.cpp:9205
		for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_mySkinList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_mySkinList.end(); ++it) {
			sItemInfo *pInfo = &*it;
			// realmyroommainui.cpp:9208
			if (category == 5 || ((pInfo->m_typeId >> 22) & 0xF) == category) {
				// realmyroommainui.cpp:9210
				FrListItem *pItem = m_pUccCopyHideWnd->AddItem(pInfo);
				// realmyroommainui.cpp:9214
				eEquipSkin skin;
				if (S5::UTIL::GetEquipSkinIndex(skin, pInfo->m_typeId)) {
					// realmyroommainui.cpp:9216
					if (WSingleton<CSharedDoc>::Instance()->m_equipSkin[skin] == pInfo->m_typeId)
						// realmyroommainui.cpp:9217
						m_pUccCopyHideWnd->SelectItem(pItem, true);
				}
			}
		}
		// realmyroommainui.cpp:9222
		break;
	}

	case 0x21:
		// realmyroommainui.cpp:9225
		for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_mySkinList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_mySkinList.end(); ++it) {
			sItemInfo *pInfo = &*it;
			// realmyroommainui.cpp:9227
			if (!S5::ISCutinItem(pInfo->m_typeId))
				continue;
			// realmyroommainui.cpp:9230
			IFF_STRUCT::S5::sCutinInformation *pCutin = ItemManager()->FindCutinInformation(pInfo->m_typeId);
			// realmyroommainui.cpp:9231
			if (pCutin == NULL)
				continue;
			// realmyroommainui.cpp:9234
			if ((m_charTypeId & 0x3FFFFFF) == pCutin->m_charSerial || pCutin->m_charSerial >= 100) {
				// realmyroommainui.cpp:9236
				FrListItem *pItem = m_pUccCopyHideWnd->AddItem(pInfo);
				// realmyroommainui.cpp:9246
				eEquipSkin skin;
				if (S5::UTIL::GetEquipSkinIndex(skin, pInfo->m_typeId)) {
					// realmyroommainui.cpp:9248
					if (WSingleton<CSharedDoc>::Instance()->m_equipSkin[skin] == pInfo->m_typeId)
						// realmyroommainui.cpp:9249
						m_pUccCopyHideWnd->SelectItem(pItem, true);
				}
			}
		}
		// realmyroommainui.cpp:9256
		break;

	case 0xA:
		// realmyroommainui.cpp:9260
		for (std::map<unsigned int, sCaddieInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_caddieMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end(); ++it) {
			// realmyroommainui.cpp:9261
			FrListItem *pItem = m_pUccCopyHideWnd->AddItem(&it->second);
			// realmyroommainui.cpp:9263
			IFF_STRUCT::sCaddie *pCaddie = ItemManager()->FindCaddie(it->second.typeId);
			// realmyroommainui.cpp:9265
			if ((it->second.flags & 2) && it->second.days <= 0 && it->second.cadItemTypeId == 0 && pCaddie->fee)
				// realmyroommainui.cpp:9266
				pItem->AddNoRBtnIdx(3);
			// realmyroommainui.cpp:9269
			if (m_caddieValueA == 0) {
				// realmyroommainui.cpp:9272
				if (WSingleton<CSharedDoc>::Instance()->m_myCaddieId == it->second.id) {
					// realmyroommainui.cpp:9275
					m_caddieValueA = it->second.id;
					m_pUccCopyHideWnd->SelectItem(pItem, true);
				}
				// realmyroommainui.cpp:9278
			} else if (m_caddieValueA == it->second.id) {
				// realmyroommainui.cpp:9281
				m_pUccCopyHideWnd->SelectItem(pItem, true);
			}
		}
		// realmyroommainui.cpp:9285
		break;

	case 0xC:
		// realmyroommainui.cpp:9288
		for (std::map<unsigned int, sMascotInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myMascotMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myMascotMap.end(); ++it) {
			// realmyroommainui.cpp:9289
			FrListItem *pItem = m_pUccCopyHideWnd->AddItem(&it->second);
			// realmyroommainui.cpp:9290
			if (WSingleton<CSharedDoc>::Instance()->m_myMascotId == it->second.id)
				// realmyroommainui.cpp:9291
				m_pUccCopyHideWnd->SelectItem(pItem, true);
		}
		// realmyroommainui.cpp:9295
		break;

	case 0x11:
		// realmyroommainui.cpp:9300
		if (IsLocalContent((localContentType_t)0x60))
			break;
		// realmyroommainui.cpp:9304
		if (WSingleton<CSharedDoc>::Instance()->m_giftList.size() == 0) {
			// realmyroommainui.cpp:9305
			m_pGiftDescEdit->ClearLine();
			m_pGiftDescEdit->AddText(MakeStr("\xB5\xB5\xC2\xF8\xC7\xD1 \xBC\xB1\xB9\xB0\xC0\xCC \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0), false, true);
			// realmyroommainui.cpp:9306
			break;
		}
		// realmyroommainui.cpp:9310
		for (std::list<sGiftInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_giftList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_giftList.end(); ++it) {
			// realmyroommainui.cpp:9311
			FrListItem *pItem = m_pUccCopyHideWnd->AddItem(&*it);
			// realmyroommainui.cpp:9312
			pItem->AddNoRBtnIdx(0);
			// realmyroommainui.cpp:9315
			if (it->m_sender[0] != '@')
				// realmyroommainui.cpp:9316
				pItem->AddNoRBtnIdx(4);
		}
		// realmyroommainui.cpp:9320
		break;

	case 0x12: {
		// realmyroommainui.cpp:9323
		m_initList.clear();
		// realmyroommainui.cpp:9326
		sTrophyStatistics &trophy = WSingleton<CSharedDoc>::Instance()->m_myTrophy;
		// realmyroommainui.cpp:9328
		sTrophyStatistics &oldTrophy = WSingleton<CSharedDoc>::Instance()->m_oldSeasonTrophy;
		// realmyroommainui.cpp:9330
		for (int course = 0; course < 13; course++) {
			// realmyroommainui.cpp:9332
			for (int medal = 2; medal >= 0; medal--) {
				// realmyroommainui.cpp:9333
				if (trophy.m_medals[course][medal] > 0 || oldTrophy.m_medals[course][medal] > 0) {
					sSpecialTrophy special;
					// realmyroommainui.cpp:9336
					unsigned short oldCount = oldTrophy.m_medals[course][medal];
					// realmyroommainui.cpp:9337
					special.m_typeId = ((course | 0x2C00) << 16) | medal;
					// realmyroommainui.cpp:9340
					special.m_count = trophy.m_medals[course][medal] + oldCount;
					m_initList.push_back(special);
				}
			}
		}

		// realmyroommainui.cpp:9346
		for (std::list<sSpecialTrophy>::iterator it = WSingleton<CSharedDoc>::Instance()->m_mySpecialTrophies.begin(); it != WSingleton<CSharedDoc>::Instance()->m_mySpecialTrophies.end(); ++it)
			// realmyroommainui.cpp:9347
			m_initList.push_back(*it);

		// realmyroommainui.cpp:9351
		for (std::list<sSpecialTrophy>::iterator it2 = WSingleton<CSharedDoc>::Instance()->m_specialTrophies.begin(); it2 != WSingleton<CSharedDoc>::Instance()->m_specialTrophies.end(); ++it2)
			// realmyroommainui.cpp:9352
			m_initList.push_back(*it2);

		// realmyroommainui.cpp:9356
		for (std::list<sSpecialTrophy>::reverse_iterator rit = m_initList.rbegin(); rit != m_initList.rend(); ++rit)
			// realmyroommainui.cpp:9357
			m_pUccCopyHideWnd->AddItem(&*rit);
		// realmyroommainui.cpp:9360
		break;
	}

	case 0x13:
		// realmyroommainui.cpp:9363
		for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {
			// realmyroommainui.cpp:9365
			if (ItemManager()->GetCouponKind(it->m_typeId))
				// realmyroommainui.cpp:9366
				m_pUccCopyHideWnd->AddItem(&*it);
		}
		// realmyroommainui.cpp:9370
		break;

	case 0x14:
		// realmyroommainui.cpp:9373
		for (std::list<sQuest>::iterator it = WSingleton<CSharedDoc>::Instance()->m_questList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_questList.end(); ++it)
			// realmyroommainui.cpp:9374
			m_pUccCopyHideWnd->AddItem(&*it);
		// realmyroommainui.cpp:9377
		break;

	case 0x17:
		// realmyroommainui.cpp:9381
		if (IsLocalContent((localContentType_t)0x4F)) {
			// realmyroommainui.cpp:9383
			WSingleton<CSharedDoc>::Instance()->ReplaySavFileCheck();
			// realmyroommainui.cpp:9385
			for (std::list<sReplayFileInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_replayFileList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_replayFileList.end(); ++it)
				// realmyroommainui.cpp:9387
				m_pUccCopyHideWnd->AddItem(&*it);
		}
		// realmyroommainui.cpp:9391
		break;

	case 0x1B:
		// realmyroommainui.cpp:9394
		if (IsLocalContent((localContentType_t)0x51)) {
			// realmyroommainui.cpp:9396
			for (std::list<sCards>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myCardList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myCardList.end(); ++it)
				// realmyroommainui.cpp:9399
				m_pUccCopyHideWnd->AddItem(&*it);
		}
		// realmyroommainui.cpp:9403
		BuildCardPackList();
		// realmyroommainui.cpp:9405
		if (IsLocalContent((localContentType_t)0x51) == true) {
			// realmyroommainui.cpp:9407
			if (m_pCardBook) {
				// realmyroommainui.cpp:9409
				if (m_pCardBook->IsShow())
					// realmyroommainui.cpp:9411
					m_pCardBook->ReBuildCurrentPage();
			}
		}
		// realmyroommainui.cpp:9416
		break;

	case 0x20:
		// realmyroommainui.cpp:9422
		for (std::list<sFurniture_List>::iterator it = WSingleton<CSharedDoc>::Instance()->m_furnitureList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_furnitureList.end(); ++it)
			// realmyroommainui.cpp:9427
			m_pUccCopyHideWnd->AddItem(&*it);
		// realmyroommainui.cpp:9431
		break;

	case 0x1C:
	case 0x1D:
	case 0x1E:
		// realmyroommainui.cpp:9436
		if (IsLocalContent((localContentType_t)0x56)) {
			// realmyroommainui.cpp:9438
			unsigned long category = GetCategoryFlag(tab);
			// realmyroommainui.cpp:9442
			for (std::list<sFurniture_List>::iterator it = WSingleton<CSharedDoc>::Instance()->m_furnitureList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_furnitureList.end(); ++it) {
				// realmyroommainui.cpp:9444
				if (((it->m_typeId >> 11) & 0x7FF) == category)
					// realmyroommainui.cpp:9445
					m_pUccCopyHideWnd->AddItem(&*it);
			}
		}
		// realmyroommainui.cpp:9450
		break;

	case 0x1F:
		// realmyroommainui.cpp:9453
		if (IsLocalContent((localContentType_t)0x56)) {
			// realmyroommainui.cpp:9463
			for (std::list<sFurniture_List>::iterator it = WSingleton<CSharedDoc>::Instance()->m_furnitureList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_furnitureList.end(); ++it) {
				// realmyroommainui.cpp:9468
				int kind = (it->m_typeId >> 11) & 0x7FF;
				// realmyroommainui.cpp:9469
				if ((kind >= 0 && kind < 7) || kind >= 10)
					// realmyroommainui.cpp:9470
					m_pUccCopyHideWnd->AddItem(&*it);
			}
		}
		break;
	}

	// realmyroommainui.cpp:9478
	m_pUccCopyHideWnd->ResetTopRow();
	// realmyroommainui.cpp:9481
	for (int n = m_pUccCopyHideWnd->GetCurrentItemSize(true); n < 8; n++)
		// realmyroommainui.cpp:9482
		m_pUccCopyHideWnd->AddItem(NULL);

	// realmyroommainui.cpp:9486
	if (tab != 0x12)
		// realmyroommainui.cpp:9487
		m_pUccCopyHideWnd->SortItem(EmptyCompares);
	// realmyroommainui.cpp:9488
}

// realmyroommainui.cpp:9491
void CRealMyRoomMain::ShowMainModel(void) {
	SetLevelBar();

	if (m_pPetExhibition) {
		m_pPetExhibition->ResetCameraRecoverTime();
		m_pPetExhibition->ParseBoundingBoxScript();
	}

	m_petLoader.Enter();
	if (m_pPetExhibition) {
		m_pPetExhibition->GetPetFrame()->Refresh(&WSingleton<CSharedDoc>::Instance()->GetMyPartTidList());
	}

	if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar) {
		WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->ChangeCharacterInfo();

		if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->GetPetFrame()) {
			WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->GetPetFrame()->Refresh(&WSingleton<CSharedDoc>::Instance()->GetMyPartTidList());
		}
	}

	m_petLoader.Leave();
}

// realmyroommainui.cpp:9518
void CRealMyRoomMain::DrawSelectItemList(FrListItem* pItem) {
	sItemInfo* pInfo = (sItemInfo*)pItem->pData;

	if (pInfo != NULL) {
		IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(pInfo->m_typeId);

		m_pItemDescEdit2->SetVisible(true);

		m_itemDescEdit2InitPos = pItem->pos;
		m_itemDescEdit2Pos = pItem->pos;

		m_pItemDescEdit2->ClearLine();
		if (pDesc) {
			m_pItemDescEdit2->AddText(pDesc->m_text, false, true);

			int lines = m_pItemDescEdit2->GetLineNum();
			float h = (float)m_pItemDescEdit2->GetLineHeight() * lines;
			m_pItemDescEdit2->SetRect(WRect(m_itemDescPos.x + m_itemDescEdit2Pos.x, m_itemDescEdit2Pos.y + m_itemDescPos.y - h, m_pItemDescEdit2->GetRect().w, h));

			m_pItemDescEdit2->GetScrollBar()->ScrollToFirst();
		} else {
			m_pItemDescEdit2->SetVisible(false);
			m_pItemDescBg2->SetVisible(false);
		}
	} else {
		m_pItemDescEdit2->SetVisible(false);
	}
}

// realmyroommainui.cpp:9555
bool CRealMyRoomMain::SelectItemList(unsigned long typeId) {
	if (m_pUccCopyHideWnd == NULL) {
		return false;
	}

	FrListItem* pFound = NULL;
	for (FrListBox::ITEM_LIST::iterator it = m_pUccCopyHideWnd->m_itemList.begin(); it != m_pUccCopyHideWnd->m_itemList.end(); ++it) {
		sItemInfo* pInfo = (sItemInfo*)(*it)->pData;
		if (pInfo != NULL) {
			if (pInfo->m_typeId == typeId) {
				pFound = *it;
				break;
			}
		}
	}

	if (pFound == NULL) {
		return false;
	}

	SelectItemList(pFound);
	return true;
}

// realmyroommainui.cpp:9582
bool CRealMyRoomMain::SelectItemList(FrListItem* pItem) {
	switch (m_roomMode) {
	case 4:
	case 21: {
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;
		g_audio->PlaySfx("item_select_cloth", 0, 0, 0, 0, 0.5f, 200.0f);

		if ((pInfo->m_typeId & 0xFC000000) == 0x70000000) {
			unsigned long oldTypeId = m_pCharCards[(pInfo->m_typeId >> 21) & 0x1F];
			m_pCharCards[(pInfo->m_typeId >> 21) & 0x1F] = pInfo->m_typeId;

			if (m_pPetExhibition != NULL) {
				m_pPetExhibition->GetPetFrame()->SetAuxPart((unsigned char)((pInfo->m_typeId >> 21) & 0x1F), pInfo->m_typeId);
			} else if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar != NULL && WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->GetPetFrame() != NULL) {
				WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->GetPetFrame()->SetAuxPart((unsigned char)((pInfo->m_typeId >> 21) & 0x1F), pInfo->m_typeId);
			}

			if (oldTypeId != 0) {
				for (FrListBox::ITEM_LIST::iterator it = m_pUccCopyHideWnd->m_itemList.begin(); it != m_pUccCopyHideWnd->m_itemList.end(); ++it) {
					sItemInfo* pOther = (sItemInfo*)(*it)->pData;
					if (pOther != NULL) {
						if ((*it)->selected && pOther->m_typeId == oldTypeId) {
							m_pUccCopyHideWnd->UnselectItem(*it);
						}
					}
				}
			}

			ShowMainModel();
			return true;
		}
	}
	case 1:
	case 2:
	case 3:
	case 26: {
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pInfo->m_typeId);
		if (pPart == NULL) {
			return false;
		}

		if (pInfo->m_stateBits.kind == 6) {
			if (pInfo->m_stateBits.expired) {
				m_pUccCopyHideWnd->ToggleItem(pItem);

				S5::FrRentalExtensionDlg* pDlg = CreateForm<S5::FrRentalExtensionDlg>(g_pFresh->GetManager(), this, "FrRentalExtensionDlg", 0);
				pDlg->Initialize(pInfo);
				pDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnRentalExtensionDlgResult, 3);
				return false;
			}
		}

		if (IsAngelWing(pInfo->m_typeId) && !IsMannerPlayer()) {
			if (pItem->selected) {
				m_pUccCopyHideWnd->ToggleItem(pItem);
			}

			this << MsgObject(0, 0x23, (intptr_t)"\xc2\xf8\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xc0\xbd", 0, 0, 0, 0);
			return false;
		}

		const char* msg = NULL;
		if ((pPart->m_level >> 7) == 0 && (unsigned char)(pPart->m_level & 0x7F) > WSingleton<CSharedDoc>::Instance()->m_myLevel) {
			msg = "\xb7\xb9\xba\xa7\xc0\xcc \xc3\xe6\xba\xd0\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9. \xb7\xb9\xba\xa7\xbe\xf7 \xc8\xc4\xbf\xa1 \xc2\xf8\xbf\xeb\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.";
		} else if ((pPart->m_level >> 7) == 1 && (unsigned char)(pPart->m_level & 0x7F) < WSingleton<CSharedDoc>::Instance()->m_myLevel) {
			msg = "\xc0\xcc\xb9\xcc \xc2\xf8\xbf\xeb\xb0\xa1\xb4\xc9\xc7\xd1 \xb7\xb9\xba\xa7\xc0\xbb \xb3\xd1\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xbe\xc6\xc0\xcc\xc5\xdb\xbf\xa1 \xc7\xa5\xbd\xc3\xb5\xc8 \xb7\xb9\xba\xa7 \xc0\xcc\xc7\xcf\xb8\xb8 \xc2\xf8\xbf\xeb\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.";
		}

		if (msg != NULL) {
			if (pItem->selected) {
				m_pUccCopyHideWnd->ToggleItem(pItem);
			}

			this << MsgObject(0, 0x23, (intptr_t)msg, 0, 0, 0, 0);
			return false;
		}

		CPartTidList& partList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

		if (!partList.CanSetPart(pInfo->m_typeId)) {
			m_pUccCopyHideWnd->UnselectItem(pItem);

			char text[256];
			if (pPart->m_wearRestricted) {
				sprintf(text, "\xba\xbb \xbb\xf3\xc7\xb0\xc0\xba \xc7\xf6\xc0\xe7 \xc0\xc7\xbb\xf3\xbf\xa1\xb4\xc2 \xc2\xf8\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
				this << MsgObject(0, 0x23, (intptr_t)text, 0, 0, 0, 0);
				return false;
			}

			this << MsgObject(0, 0x23, (intptr_t)"\\c0xffff0000\\c\xbb\xf3\xc7\xb0\xc0\xbb \xc2\xf8\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0);
			return false;
		}

		g_audio->PlaySfx("item_select_cloth", 0, 0, 0, 0, 0.5f, 200.0f);

		unsigned long overlapped = partList.GetOverlapped(pInfo->m_typeId);
		if (overlapped != 0) {
			for (FrListBox::ITEM_LIST::iterator it = m_pUccCopyHideWnd->m_itemList.begin(); it != m_pUccCopyHideWnd->m_itemList.end(); ++it) {
				sItemInfo* pOther = (sItemInfo*)(*it)->pData;
				if (pOther != NULL) {
					if ((*it)->selected && *it != pItem && partList.IsEquipped(pOther->m_typeId, pOther->m_serial)) {
						unsigned long otherOverlapped = partList.GetOverlapped(pOther->m_typeId);
						if (overlapped & otherOverlapped) {
							m_pUccCopyHideWnd->UnselectItem(*it);
							UnselectItemList(*it);
						}
					}
				}
			}
		}

		CPartTidList avatarList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

		unsigned long result = 0;
		if (m_pPetExhibition != NULL && m_pPetExhibition->GetPetFrame() != NULL) {
			result = m_pPetExhibition->GetPetFrame()->SetPart(&partList, pInfo->m_typeId, pInfo->m_serial);
		}

		if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar != NULL && WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->GetPetFrame() != NULL) {
			WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->GetPetFrame()->SetPart(&avatarList, pInfo->m_typeId, pInfo->m_serial);
		}

		if (result != 0) {
			sCharacterInfo& charInfo = WSingleton<CSharedDoc>::Instance()->m_characterMap[WSingleton<CSharedDoc>::Instance()->m_myCharacterId];
			for (int i = 0; i < partList.m_numTotalParts; i++) {
				charInfo.partTypeIds[i] = partList.m_partTids[i];
				charInfo.partIds[i] = partList.m_partIds[i];
			}
		} else {
			this << MsgObject(0, 0x23, (intptr_t)"\xc2\xf8\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9", 0, 0, 0, 0);
		}

		ShowMainModel();

		m_pCharParts = partList.m_partTids;
		memcpy(m_localContentData, partList.m_partIds, sizeof(m_localContentData));
		return true;
	}

	case 8:
	case 9:
	case 23:
	case 24:
	case 28:
	case 29:
	case 30:
	case 31:
	case 32: {
		unsigned long* pTypeId = &((sItemInfo*)pItem->pData)->m_typeId;
		m_petLoader.WaitUntilWorkDone(9999);
		if (m_pPetExhibition == NULL || m_pPetExhibition->GetTypeId() != *pTypeId) {
			m_petLoader.Work(this, 0, pTypeId, true, -1);
		}
		break;
	}

	case 13:
	case 14:
	case 15:
	case 16: {
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		eEquipSkin skin;
		if (S5::UTIL::GetEquipSkinIndex(skin, pInfo->m_typeId)) {
			m_displaySkin[skin] = pInfo->m_typeId;
			g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		}

		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pInfo->m_typeId);
		if (pSkin == NULL) {
			return true;
		}

		if ((pInfo->m_typeId & 0x3C00000) == 0 && m_displaySkin[0] != 0) {
			m_skinParam[0] = *((unsigned char*)pSkin + 0xB8);
			m_skinParam[1] = *((unsigned char*)pSkin + 0xB9);
			m_skinSlot[0] = 0;
			m_skinSlot[1] = 0;
		}

		WSingleton<CSharedDoc>::Instance()->m_equipSkin[skin] = pInfo->m_typeId;
		return true;
	}

	case 34: {
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		Title* pTitle = g_TitleManager->GetTitle((eTitle)(pInfo->m_typeId & 0x3FFFFF));
		if (pTitle != NULL) {
			if (!pTitle->CanEquip()) {
				if (pItem->selected) {
					m_pUccCopyHideWnd->ToggleItem(pItem);
					m_pUccCopyHideWnd->SelectItem(m_pUccCopyHideWnd->GetPreviousSelected(), true);
				}

				this << MsgObject(0, 0x23, (intptr_t)"\xc2\xf8\xbf\xeb\xc7\xd2 \xbc\xf6 \xc0\xd6\xb4\xc2 \xc1\xb6\xb0\xc7\xc0\xbb \xb0\xae\xc3\xdf\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9", 0, 0, 0, 0);
				return false;
			}
		}

		eEquipSkin skin;
		if (S5::UTIL::GetEquipSkinIndex(skin, pInfo->m_typeId)) {
			m_displaySkin[skin] = pInfo->m_typeId;
			g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		}

		if (ItemManager()->FindSkin(pInfo->m_typeId) == NULL) {
			return true;
		}

		WSingleton<CSharedDoc>::Instance()->m_equipSkin[skin] = pInfo->m_typeId;
		return true;
	}

	case 33: {
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		if (!S5::ISCutinItem(pInfo->m_typeId)) {
			return true;
		}

		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		return true;
	}

	case 6: {
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		int result = ChangeClubset(pInfo->m_serial);
		if (result == 0) {
			this << MsgObject(0, 0x23, (intptr_t)"\xb7\xb9\xba\xa7\xc0\xcc \xc3\xe6\xba\xd0\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9. \xb7\xb9\xba\xa7\xbe\xf7 \xc8\xc4\xbf\xa1 \xc2\xf8\xbf\xeb\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.", 0, 0, 0, 0);
		} else if (result == 2) {
			this << MsgObject(0, 0x23, (intptr_t)"\xc2\xf8\xbf\xeb \xb7\xb9\xba\xa7\xc0\xbb \xc3\xca\xb0\xfa\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9. \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0);
		}

		g_audio->PlaySfx("item_select_club", 0, 0, 0, 0, 0.5f, 200.0f);
		return true;
	}

	case 7:
		ChangeAztec(((sItemInfo*)pItem->pData)->m_typeId);
		g_audio->PlaySfx("item_select_aztec", 0, 0, 0, 0, 0.5f, 200.0f);
		return true;

	case 10: {
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		if (!ChangeCaddie(pInfo->m_serial)) {
			this << MsgObject(0, 0x23, (intptr_t)"\xb7\xb9\xba\xa7\xc0\xcc \xc3\xe6\xba\xd0\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9. \xb7\xb9\xba\xa7\xbe\xf7 \xc8\xc4\xbf\xa1 \xc2\xf8\xbf\xeb\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.", 0, 0, 0, 0);
		}

		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		return true;
	}

	case 12: {
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		ChangeMascot(pInfo->m_serial);
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		return true;
	}

	case 18:
	case 19:
		SafeLoadPet(&((sItemInfo*)pItem->pData)->m_typeId, false);
	case 27:
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		return true;

	case 22:
		return true;

	case 17: {
		if (IsLocalContent(static_cast<localContentType_t>(0x60))) {
			return true;
		}

		sGiftInfo* pGift = (sGiftInfo*)pItem->pData;
		char date[32] = { 0 };
		char time[32] = { 0 };

		sscanf(pGift->m_date, "%s %s", date, time);

		char* pDot = strchr(time, '.');
		if (pDot != NULL) {
			*pDot = 0;
		}

		SetGiftDescString(pGift->m_sender, pGift->m_message, date, time);
		break;
	}
	}

	g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
	return true;
}

// realmyroommainui.cpp:10011
bool CRealMyRoomMain::UnselectItemFromItemList(unsigned long typeId) {
	if (m_pUccCopyHideWnd == NULL) {
		return false;
	}

	if ((typeId & 0xFC000000) != 0x08000000) {
		return false;
	}

	for (FrListBox::ITEM_LIST::iterator it = m_pUccCopyHideWnd->m_itemList.begin(); it != m_pUccCopyHideWnd->m_itemList.end(); ++it) {
		sItemInfo* pInfo = (sItemInfo*)(*it)->pData;

		if (pInfo != NULL) {
			if (pInfo->m_typeId == typeId) {
				m_pUccCopyHideWnd->UnselectItem(*it);
				return true;
			}
		}
	}

	return false;
}

// realmyroommainui.cpp:10036
bool CRealMyRoomMain::UnselectItemList(unsigned long typeId) {
	if (m_pUccCopyHideWnd == NULL) {
		return false;
	}

	FrListItem* pFound = NULL;
	for (FrListBox::ITEM_LIST::iterator it = m_pUccCopyHideWnd->m_itemList.begin(); it != m_pUccCopyHideWnd->m_itemList.end(); ++it) {
		sItemInfo* pInfo = (sItemInfo*)(*it)->pData;
		if (pInfo != NULL) {
			if (pInfo->m_typeId == typeId) {
				pFound = *it;
				break;
			}
		}
	}

	if (pFound == NULL) {
		return false;
	}

	UnselectItemList(pFound);
	return true;
}

// realmyroommainui.cpp:10062
void CRealMyRoomMain::UnselectItemList(FrListItem* pItem) {
	switch (m_roomMode) {
	case 4:
	case 21: {
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		if ((pInfo->m_typeId & 0xFC000000) == 0x70000000) {
			m_pCharCards[(pInfo->m_typeId >> 21) & 0x1F] = 0;
			if (m_pPetExhibition != NULL) {
				m_pPetExhibition->GetPetFrame()->SetAuxPart((unsigned char)((pInfo->m_typeId >> 21) & 0x1F), 0);
			} else if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar != NULL && WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->GetPetFrame() != NULL) {
				WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->GetPetFrame()->SetAuxPart((unsigned char)((pInfo->m_typeId >> 21) & 0x1F), 0);
			}

			ShowMainModel();
			break;
		}
	}
	case 1:
	case 2:
	case 3:
	case 26: {
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		CPartTidList& partList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

		CPartTidList avatarList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

		unsigned long oldParts[24];
		memcpy(oldParts, partList.m_partTids, sizeof(oldParts));

		if (partList.IsEquipped(pInfo->m_typeId, pInfo->m_serial)) {
			unsigned long result = 0;
			if (m_pPetExhibition != NULL && m_pPetExhibition->GetPetFrame() != NULL) {
				result = m_pPetExhibition->GetPetFrame()->SetPart(&partList, pInfo->m_typeId, pInfo->m_serial);
			}

			if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar != NULL && WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->GetPetFrame() != NULL) {
				result = WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->GetPetFrame()->SetPart(&avatarList, pInfo->m_typeId, pInfo->m_serial);
			}

			if (result != 0) {
				sCharacterInfo& charInfo = WSingleton<CSharedDoc>::Instance()->m_characterMap[WSingleton<CSharedDoc>::Instance()->m_myCharacterId];
				for (int i = 0; i < partList.m_numTotalParts; i++) {
					charInfo.partTypeIds[i] = partList.m_partTids[i];
					charInfo.partIds[i] = partList.m_partIds[i];
				}

				m_pCharParts = partList.m_partTids;
			}
		} else {
			m_pUccCopyHideWnd->SelectItem(pItem, true);
		}

		for (int i = 0; i < 24; i++) {
			if (oldParts[i] != 0 && pInfo->m_typeId != oldParts[i] && partList.m_partTids[i] != oldParts[i]) {
				if (oldParts[i] != 2 && oldParts[i] != 3) {
					UnselectItemFromItemList(oldParts[i]);
				}
			}
		}

		if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(pInfo->m_typeId)) {
			unsigned char slot = (unsigned char)((pInfo->m_typeId >> 13) & 0x1F);
			unsigned long charTypeId = WSingleton<CSharedDoc>::Instance()->m_characterMap[WSingleton<CSharedDoc>::Instance()->m_myCharacterId].typeId;
			unsigned long typeId = (((charTypeId << 5) | slot) << 13) | 0x8000400;
			if (m_pPetExhibition != NULL && m_pPetExhibition->GetPetFrame() != NULL) {
				m_pPetExhibition->GetPetFrame()->SetPart(&partList, typeId, 0);
			}

			memcpy(m_localContentData, partList.m_partIds, sizeof(m_localContentData));
		}

		ShowMainModel();
		break;
	}

	case 12:
		ChangeMascot(0);
		break;

	case 10:
		ChangeCaddie(0);
		TurnOffItemStatus();
		break;

	case 13:
	case 14:
	case 15:
	case 16:
	case 34: {
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		eEquipSkin skin;
		if (S5::UTIL::GetEquipSkinIndex(skin, pInfo->m_typeId)) {
			m_displaySkin[skin] = 0;
			WSingleton<CSharedDoc>::Instance()->m_equipSkin[skin] = 0;
		}
		break;
	}

	case 22:
	case 33:
		break;

	default:
		m_pUccCopyHideWnd->SelectItem(pItem, true);
		break;
	}
}

// realmyroommainui.cpp:10224
int CRealMyRoomMain::GetItemNumInMyPocket(unsigned long typeId) {
	int count = 0;

	if (typeId == 0 || (typeId & 0xFC000000) == 0x04000000) {
		for (std::map<unsigned int, sCharacterInfo>::const_iterator it = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++it) {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.typeId);
			if (pItem) {
				if (typeId == 0 || it->second.typeId == typeId)
					count++;
			}
		}
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x08000000) {
		for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myPartsList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myPartsList.end(); ++it) {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->m_typeId);
			if (pItem) {
				if (typeId == 0 || it->m_typeId == typeId)
					count++;
			}
		}
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x18000000) {
		for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->m_typeId);
			if (pItem) {
				if (typeId == 0 || it->m_typeId == typeId)
					count++;
			}
		}
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x1C000000) {
		for (std::map<unsigned int, sCaddieInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_caddieMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end(); ++it) {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.typeId);
			if (pItem) {
				if (typeId == 0 || it->second.typeId == typeId)
					count++;
			}
		}
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x20000000) {
		count = 0;
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x3C000000) {
		for (std::map<unsigned int, sCharacterInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++it) {
			IFF_STRUCT::sHairStyle* pHair = ItemManager()->FindHairStyle(typeId);
			if (pHair) {
				if (pHair->m_color == it->second.hairColor && (pHair->m_style | 0x04000000) == it->second.typeId)
					count++;
			}
		}
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x10000000) {
		for (std::map<unsigned int, sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myItemMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myItemMap.end(); ++it) {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.m_typeId);
			if (pItem) {
				if (typeId == 0 || it->second.m_typeId == typeId)
					count++;
			}
		}
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x14000000) {
		for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myGearList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myGearList.end(); ++it) {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->m_typeId);
			if (pItem) {
				if (typeId == 0 || it->m_typeId == typeId)
					count++;
			}
		}
	}

	if (typeId >> 26 == 9 && (typeId & 0x03E00000) == 0x00200000) {
		IFF_STRUCT::sSetItem* pSetItem = ItemManager()->FindSetItem(typeId);
		for (unsigned char i = 0; i < pSetItem->m_itemCount; i++) {
			if (typeId == 0 || (pSetItem->m_itemTypeIds[i] & 0xFC000000) != 0x04000000) {
				for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myPartsList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myPartsList.end(); ++it) {
					IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->m_typeId);
					if (pItem) {
						if (typeId == 0 || it->m_typeId == pSetItem->m_itemTypeIds[i])
							count++;
					}
				}
			}
		}
	}

	if (typeId == 0 || typeId >> 26 == 14) {
		for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_mySkinList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_mySkinList.end(); ++it) {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->m_typeId);
			if (pItem) {
				if (typeId == 0 || it->m_typeId == typeId)
					count++;
			}
		}
	}

	return count;
}

// realmyroommainui.cpp:10383
void CRealMyRoomMain::Net_EquipCharacter(void) {
	if (m_characterId != WSingleton<CSharedDoc>::Instance()->m_myCharacterId) {
		WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
		packet.Encode1(5);
		packet.Encode4(WSingleton<CSharedDoc>::Instance()->m_myCharacterId);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// realmyroommainui.cpp:10394
void CRealMyRoomMain::Net_EquipDresserParts(void) {
	int i;

	std::map<unsigned int, sCharacterInfo>::iterator it = m_characterMap.begin(), docIt = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin();

	for (; it != m_characterMap.end() && docIt != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++it, ++docIt) {
		sCharacterInfo& charInfo = it->second;
		sCharacterInfo& docInfo = docIt->second;

		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(charInfo.typeId);
		if (pChar == NULL) {
			continue;
		}

		int numParts = pChar->m_numParts + pChar->m_numExtraParts;
		bool changed = false;
		for (i = 0; i < numParts; i++) {
			if (charInfo.partTypeIds[i] != docInfo.partTypeIds[i] || charInfo.partIds[i] != docInfo.partIds[i]) {
				changed = true;
				break;
			}
		}

		for (i = 0; i < 5; i++) {
			if (charInfo.auxParts.tids[i] != docInfo.auxParts.tids[i]) {
				changed = true;
				break;
			}
		}

		if (!changed) {
			continue;
		}

		sCharacterInfo info = docInfo;

		for (i = 0; i < numParts; i++) {
			info.partTypeIds[i] = docInfo.partTypeIds[i];
			info.partIds[i] = docInfo.partIds[i];
		}

		for (i = numParts; i < 24; i++) {
			info.partTypeIds[i] = 0;
			info.partIds[i] = 0;
		}

		info.auxParts = docInfo.auxParts;

		WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
		packet.Encode1(0);
		packet.EncodeBuffer(&info, sizeof(info));
		packet.Send(SEND_TO_DEFAULT);
	}
}

// realmyroommainui.cpp:10467
void CRealMyRoomMain::Net_EquipCaddie(void) {
	if (m_caddieId == WSingleton<CSharedDoc>::Instance()->m_myCaddieId) {
		return;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_myCaddieId != 0) {
		std::map<unsigned int, sCaddieInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_caddieMap.find(WSingleton<CSharedDoc>::Instance()->m_myCaddieId);
		if (it == WSingleton<CSharedDoc>::Instance()->m_caddieMap.end()) {
			return;
		}
	}

	WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
	packet.Encode1(1);
	packet.Encode4(WSingleton<CSharedDoc>::Instance()->m_myCaddieId);
	packet.Send(SEND_TO_DEFAULT);
}

// realmyroommainui.cpp:10486
void CRealMyRoomMain::Net_EquipItem(void) {
	int i;
	for (i = 0; i < 10; i++) {
		if (m_equipItem[i] != WSingleton<CSharedDoc>::Instance()->m_equipItem[i]) {
			break;
		}
	}

	if (i == 10) {
		return;
	}

	WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
	packet.Encode1(2);
	packet.EncodeBuffer(WSingleton<CSharedDoc>::Instance()->m_equipItem, 0x28);
	packet.Send(SEND_TO_DEFAULT);
}

// realmyroommainui.cpp:10504
void CRealMyRoomMain::Net_EquipGear(void) {
	if (m_ballId != WSingleton<CSharedDoc>::Instance()->m_myBallId || m_clubId != WSingleton<CSharedDoc>::Instance()->m_myClubId) {
		WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
		packet.Encode1(3);
		packet.Encode4(WSingleton<CSharedDoc>::Instance()->m_myBallId);
		packet.Encode4(WSingleton<CSharedDoc>::Instance()->m_myClubId);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// realmyroommainui.cpp:8502
void CRealMyRoomMain::SetLevelBar(void) {
	unsigned char status[5];
	unsigned char slotStatus[5];
	unsigned char cardStatus[5];
	int i;

	memset(status, 0, sizeof(status));
	memset(slotStatus, 0, sizeof(slotStatus));
	memset(cardStatus, 0, sizeof(cardStatus));

	for (i = 0; i < 5; i++) {
		if ((&m_pPowerBar)[i]) {
			(&m_pPowerBar)[i]->SetDestPos(0, false);
			(&m_pPowerBar)[i]->SetExpand(0);
		}
	}

	switch (m_statTab) {
	case 1:
		GetTotalStatus(status, slotStatus, cardStatus);
		break;
	case 0:
		switch (m_roomMode) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 21:
		case 22:
		case 26:
			GetCharacterStatus(status, slotStatus);
			m_statSource = 0;
			break;
		case 6:
			GetClubSetStatus(status, slotStatus);
			m_statSource = 1;
			break;
		case 10:
			GetCaddieStatus(status, slotStatus);
			m_statSource = 2;
			break;
		}
		break;
	}

	for (i = 0; i < 5; i++) {
		if ((&m_pPowerBar)[i]) {
			(&m_pPowerBar)[i]->SetDestPos(status[i], false);
			(&m_pPowerBar)[i]->SetExpand(slotStatus[i] - status[i]);
		}

		if ((&m_pPowerEdit)[i]) {
			(&m_pPowerEdit)[i]->SetCaption11(MakeStatusString3(status[i], slotStatus[i], cardStatus[i]));
		}
	}
}

// realmyroommainui.cpp:10518
void CRealMyRoomMain::Net_EquipSkin(void) {
	int i;
	for (i = 0; i < 6; i++) {
		if (m_equipSkin[i] != WSingleton<CSharedDoc>::Instance()->m_equipSkin[i]) {
			break;
		}
	}

	if (i == 6) {
		return;
	}

	for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_mySkinList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_mySkinList.end(); ++it) {
		eEquipSkin skin;
		if (!S5::UTIL::GetEquipSkinIndex(skin, it->m_typeId)) {
			continue;
		}

		if (it->m_stateBits.kind == 1 && it->m_typeId == WSingleton<CSharedDoc>::Instance()->m_equipSkin[skin]) {
			it->m_stateBits.kind = 2;
		}
	}

	WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
	packet.Encode1(4);
	packet.EncodeBuffer(WSingleton<CSharedDoc>::Instance()->m_equipSkin, 0x18);
	packet.Send(SEND_TO_DEFAULT);
}

// realmyroommainui.cpp:10557
void CRealMyRoomMain::Net_StartPeriodItem(unsigned long serial) {
	WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
	packet.Encode1(7);
	packet.Encode4(serial);
	packet.Send(SEND_TO_DEFAULT);
}

// realmyroommainui.cpp:10609
bool CRealMyRoomMain::OnEnchantDlgResult(int result, FrForm* pForm) {
	m_pEnchantDlg = NULL;
	return true;
}

// realmyroommainui.cpp:10615
bool CRealMyRoomMain::OnPangGiftDlgResult(int result, FrForm* pForm) {
	if (result == 1) {
		sGiftInfo* pInfo = ((FrPangGiftDlg*)pForm)->GetItemInfo();

		WSendPacket packet(GCT_MOVE_GIFT);
		packet.Encode4(pInfo->m_serial);
		packet.Encode4(pInfo->m_typeId);
		packet.Send(SEND_TO_DEFAULT);

		Sleep(50);
	}

	m_pPangGiftDlg = NULL;
	return true;
}

// realmyroommainui.cpp:10634
bool CRealMyRoomMain::OnReturnGiftDlgResult(int result, FrForm* pForm) {
	if (result != 1)
		return true;

	FrReturnGiftDlg* pDlg = DYNAMIC_CAST(FrReturnGiftDlg, pForm);
	if (pDlg == NULL)
		return true;

	WSendPacket packet(GCT_REQUEST_DRAWBACK_GIFT);
	packet.Encode4(pDlg->m_requestIds[0]);
	packet.Encode4(pDlg->m_requestIds[1]);
	packet.Send(SEND_TO_DEFAULT);

	return true;
}

// realmyroommainui.cpp:10651
bool CRealMyRoomMain::OnMoveGiftDlgResult(int result, FrForm* pForm) {
	if (result == 1 && m_pMoveGift) {
		m_areaFlagB = false;
		// "Requested moving the gift."
		this << MsgObject(0, 0x24, (intptr_t)"\xBC\xB1\xB9\xB0 \xBF\xC5\xB1\xE2\xB1\xE2\xB8\xA6 \xBF\xE4\xC3\xBB\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
		this << MsgObject(0, 1, 0, 0, 0, 0, 0);

		WSendPacket packet(GCT_MOVE_GIFT);
		packet.Encode4(m_pMoveGift->m_serial);
		packet.Encode4(m_pMoveGift->m_typeId);
		packet.Send(SEND_TO_DEFAULT);
	}

	return true;
}

// realmyroommainui.cpp:10675
bool CRealMyRoomMain::OnMascotInfoDlgResult(int result, FrForm* pForm) {
	m_pMascotInfoDlg = NULL;
	return true;
}

// realmyroommainui.cpp:10681
bool CRealMyRoomMain::OnUpgradeDlgResult(int result, FrForm* pForm) {
	m_pUpgradeDlg = NULL;
	return true;
}

// realmyroommainui.cpp:10687
bool CRealMyRoomMain::OnNoteDlgResult(int result, FrForm* pForm) {
	if (result == 1) {
		WSendPacket packet(GCT_MSN_REQUEST);
		packet.Encode2(0x111);

		packet.Encode4(0xFFFFFFFF);
		packet.EncodeStr(std::string(m_pNoteDlg->GetNote()));
		packet.Encode1(1);
		packet.Send(SEND_TO_DEFAULT);
	}

	m_pNoteDlg = NULL;
	return true;
}

// realmyroommainui.cpp:10704
bool CRealMyRoomMain::OnFunritureSaveMsgdlgResult(int result, FrForm* pForm) {
	SetCameraDisplayType(0);
	return true;
}

// realmyroommainui.cpp:10711
bool CRealMyRoomMain::OnOfflineShopEditMsgDlgResult(int result, FrForm* pForm) {
	if (result == 1) {
		m_offlineShopEditState = 1;

		WSendPacket packet(GCT_OFFLINESHOP_REQUEST_LOAD_STALL);
		packet.Encode4(WSingleton<CSharedDoc>::Instance()->m_myRoomOwnerUID);
		packet.Send(SEND_TO_DEFAULT);
	}

	return true;
}

// realmyroommainui.cpp:10731
bool CRealMyRoomMain::OnFurnitrueListDeleteDlgResult(int result, FrForm* pForm) {
	if (result == 1) {
		FrListItem* pItem = m_pFurnitureList->GetItemUnderCursor();
		if (pItem == NULL) {
			FrForm* pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			// "The furniture to delete was not found."
			pNotify->SetMessage("\xBB\xE8\xC1\xA6\xC7\xD2 \xB0\xA1\xB1\xB8\xB8\xA6 \xC3\xA3\xC1\xF6 \xB8\xF8\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			pNotify->Open(0, 3);
		} else {
			sFurniture_List* pFurniture = (sFurniture_List*)pItem->pData;

			pItem->selected = false;

			if (pFurniture != NULL) {
				m_pFurnitureList->UnselectItem(pItem);

				for (std::list<sFurniture_List>::iterator it = WSingleton<CSharedDoc>::Instance()->m_furnitureList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_furnitureList.end(); ++it) {
					if (it->m_id == pFurniture->m_id) {
						it->m_flags &= ~1;
					}
				}

				WSingleton<CRealMyRoomData>::Instance()->ListFurnitureSelect(pFurniture);
				WSingleton<CRealMyRoomData>::Instance()->SetState(6);
			} else {
				FrForm* pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				// "The furniture to delete was not found."
				pNotify->SetMessage("\xBB\xE8\xC1\xA6\xC7\xD2 \xB0\xA1\xB1\xB8\xB8\xA6 \xC3\xA3\xC1\xF6 \xB8\xF8\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.", false);
				pNotify->Open(0, 3);
			}
		}
	}

	return true;
}

// realmyroommainui.cpp:10779
bool CRealMyRoomMain::OnCaddieInfoDlgResult(int result, FrForm* pForm) {
	if (result == 2) {

		if (m_pCaddieInfoWnd != NULL) {

			m_pCaddieInfoWnd->Enable(true);

			if (pForm != NULL) {

				sCaddieInfo info = ((FrCaddieInfoDlg*)pForm)->GetCaddieInfo();
				unsigned long typeId = info.typeId;

				std::map<unsigned int, IFF_STRUCT::sCaddie>& caddies = ItemManager()->m_caddieMap;
				for (std::map<unsigned int, IFF_STRUCT::sCaddie>::const_iterator it = caddies.begin(); it != caddies.end(); ++it) {
					if (it->second.m_typeId == typeId) {
						if ((it->second.m_level & 0x7F) > WSingleton<CSharedDoc>::Instance()->m_myLevel) {
							m_pCaddieInfoWnd->Enable(false);
						}
						break;
					}
				}
			}
		}
	}

	return true;
}

// realmyroommainui.cpp:10816
bool CRealMyRoomMain::OnOpenTikiReport(int result, FrForm* pForm) {
	if (!IsLocalContent((localContentType_t)0x14)) {
		return false;
	}

	if (result == 1) {
		if (m_tikiReportSerial != 0 && m_tikiReportRecord != 0) {
			WSendPacket packet((enumClientPacket)0xA3);
			packet.Encode4(m_tikiReportSerial);
			packet.Encode4(m_tikiReportRecord);
			packet.Send(SEND_TO_DEFAULT);

			this << MsgObject(0, 1, 0, 0, 0, 0, 0);
		}
	} else {
		m_tikiReportSerial = 0;
	}

	m_tikiReportRecord = 0;
	return true;
}

// realmyroommainui.cpp:10844
bool CRealMyRoomMain::OnOpenNewYearMoneyDlgResult(int result, FrForm* pForm) {
	if (result == 1) {
		if (m_newYearMoneySerial != 0) {
			this << MsgObject(0, 1, 0, 0, 0, 0, 0);

			WSendPacket packet(GCT_OPEN_NEWYEAR_MONEY);
			packet.Encode4(m_newYearMoneySerial);
			packet.Send(SEND_TO_DEFAULT);
		}
	}

	m_newYearMoneySerial = 0;

	return true;
}

// realmyroommainui.cpp:10864
bool CRealMyRoomMain::OnOpenEventGiftDlgResult(int result, FrForm* pForm) {
	if (result == 1) {
		if (m_eventGiftBoxSerial != 0) {
			this << MsgObject(0, 1, 0, 0, 0, 0, 0);

			WSendPacket packet(GCT_OPEN_EVENT_GIFTBOX);
			packet.Encode4(m_eventGiftBoxSerial);
			packet.Send(SEND_TO_DEFAULT);
		}
	}

	m_eventGiftBoxSerial = 0;

	return true;
}

// realmyroommainui.cpp:10884
bool CRealMyRoomMain::OnOpenS4EventGiftDlgResult(int result, FrForm* pForm) {
	if (result == 1) {
		if (m_s4EventGiftBoxTypeId != 0) {
			this << MsgObject(0, 1, 0, 0, 0, 0, 0);

			WSendPacket packet(GCT_OPEN_S4_EVENT_GIFTBOX);
			packet.Encode4(m_s4EventGiftBoxTypeId);
			packet.Send(SEND_TO_DEFAULT);
		}
	}

	m_s4EventGiftBoxTypeId = 0;

	return true;
}

// realmyroommainui.cpp:10565
void CRealMyRoomMain::Net_EquipMascot(void) {
	sMascotInfo* pMascot = WSingleton<CSharedDoc>::Instance()->GetMyEquippedMascotInfo();

	if (pMascot) {
		if (ItemManager()->CompareSystemTime(pMascot->endTime, WSingleton<CSharedDoc>::Instance()->GetServerTime()) <= 0 && pMascot->pcBang == 0) {
			WSingleton<CSharedDoc>::Instance()->m_myMascotId = 0;
		}
	}

	CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
	if (m_mascotId != pDoc->m_myMascotId) {
		if (pDoc->m_myMascotId != 0) {
			std::map<unsigned int, sMascotInfo>::iterator it = pDoc->m_myMascotMap.find(pDoc->m_myMascotId);
			if (it == pDoc->m_myMascotMap.end()) {
				return;
			}
		}

		WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
		packet.Encode1(8);
		packet.Encode4(WSingleton<CSharedDoc>::Instance()->m_myMascotId);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// realmyroommainui.cpp:10903
void CRealMyRoomMain::OpenCaddieInfo(const sCaddieInfo& info) {
	FrWnd* pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName("caddie_info");
	if (pWnd == NULL) {
		FrCaddieInfoDlg* pDlg = CreateForm<FrCaddieInfoDlg>(g_pFresh->GetManager(), this, "caddie_info", DYNAMIC_CAST(FrWnd, pWnd));

		if (pDlg) {
			pDlg->SetCaddieInfo(info);
			pDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnCaddieInfoDlgResult, 3);

			if (m_pCaddieInfoWnd)
				m_pCaddieInfoWnd->Enable(false);
		}
	}
}

// realmyroommainui.cpp:10923
void CRealMyRoomMain::OpenInformation(FrListItem* pItem) {
	if (pItem) {
		m_infoDlgPos.x = pItem->pos.x - 300.0f;
		m_infoDlgPos.y = pItem->pos.y - 450.0f;
	}

	switch (m_roomMode) {
	case 17: {
		if (IsLocalContent((localContentType_t)0x60)) {
			break;
		}

		// The gift list's rows: the type id follows the serial like in
		// sItemInfo (sGiftInfo's members at 0x0/0x4 are not declared yet).
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		OpenInformation(&pInfo->m_typeId, (sGiftInfo*)pInfo, NULL);
		break;
	}
	default: {
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		OpenInformation(&pInfo->m_typeId, NULL, &pInfo->m_serial);
		break;
	}
	case 10:
		OpenCaddieInfo(*(sCaddieInfo*)pItem->pData);
		break;
	case 12: {
		sMascotInfo* pInfo = (sMascotInfo*)pItem->pData;

		m_pMascotInfoDlg = CreateForm<FrMascotInfoDlg>(g_pFresh->GetManager(), this, "mascot_info", NULL);

		m_pMascotInfoDlg->SetMascotInfo(*pInfo, m_pPetExhibition ? m_pPetExhibition->GetModel() : NULL);
		m_pMascotInfoDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnMascotInfoDlgResult, 1);
		break;
	}
	}
}

// realmyroommainui.cpp:10989
void CRealMyRoomMain::OpenInformation(unsigned long* pTypeId, sGiftInfo* pGiftInfo, unsigned long* pSerial) {
	if (FrStaticCast<FrWnd>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("information")))
		return;

	if (FrStaticCast<FrWnd>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("information_setitem")))
		return;

	IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(*pTypeId);
	unsigned long serial;

	if (pSerial)
		serial = *pSerial;
	else
		serial = 0;

	if (!pItem)
		return;

	if ((pItem->m_typeId & 0xFC000000) == 0x24000000) {
		FrSetItemDlg* pSetDlg = CreateForm<FrSetItemDlg>(g_pFresh->GetManager(), this, "information_setitem", 0);
		if (!pSetDlg)
			return;

		pSetDlg->SetSetItem(pItem->m_typeId);
		pSetDlg->Open(0, 3);
		return;
	}

	FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "information", 0);
	if (!pForm)
		return;

	FrArea* pPortrait = DYNAMIC_CAST(FrArea, pForm->FindChildByName("portrait"));
	if (pPortrait) {
		if (IsLocalContent((localContentType_t)0x57) && IsLocalContent((localContentType_t)0x5F) && serial && IsUccClothes(*pTypeId)) {
			const Bitmap* pIcon = GetUccItemIcon(serial);
			if (pIcon) {
				pPortrait->SetBgImg(pIcon);
			}
		} else {
			pPortrait->SetBgImg(pItem->m_icon);
		}
	}

	FrEdit* pName = DYNAMIC_CAST(FrEdit, pForm->FindChildByName("name"));
	FrArea* pLevel = DYNAMIC_CAST(FrArea, pForm->FindChildByName("level"));

	bool bPeriod = false;
	if (pName && pLevel) {
		char szName[128] = { 0 };

		if (IsLocalContent((localContentType_t)0x57) && IsLocalContent((localContentType_t)0x5F) && serial && IsUccClothes(*pTypeId)) {
			strncpy(szName, GetUccItemName(serial), 128);
		} else {
			strncpy(szName, pItem->m_name, 128);
		}

		if (IsAngelWing(pItem->m_typeId) && !IsMannerPlayer()) {
			pName->AddText(MakeStr("\xC0\xCC\xB8\xA7: %s \\c0xffff0000\\c(\xC2\xF8\xBF\xEB\xBA\xD2\xB0\xA1)\\c0xff000000\\c\n\n", szName), false, true);
		} else {
			pName->AddText(MakeStr("\xC0\xCC\xB8\xA7: %s\n\n", szName), false, true);
		}

		if (pItem->m_level & 0x80) {
			pName->AddText("\xB7\xB9\xBA\xA7:                \xC0\xCC\xC7\xCF\n", false, true);
		} else {
			pName->AddText("\xB7\xB9\xBA\xA7:                \xC0\xCC\xBB\xF3\n", false, true);
		}

		pLevel->SetBgImg(MakeStr("level_%03d", (pItem->m_level & 0x7F) + 1));

		unsigned char kind = 0;
		int days = 0;
		int hours = 0;
		bool bExpired = false;

		if (pGiftInfo) {
			if (pItem->m_timeType) {
				kind = 1;
				days = pGiftInfo->m_pang;
				hours = 0;
			}
		} else {
			CSharedDoc* pDoc = CSharedDoc::Instance();
			std::list<sItemInfo>::iterator it;

			for (it = pDoc->m_mySkinList.begin(); it != pDoc->m_mySkinList.end(); ++it) {
				if (&it->m_typeId == pTypeId)
					break;
			}

			if (it == pDoc->m_mySkinList.end()) {
				for (it = pDoc->m_itemInfoList.begin(); it != pDoc->m_itemInfoList.end(); ++it) {
					if (&it->m_typeId == pTypeId)
						break;
				}
			}

			bool bFound = true;
			if (it == pDoc->m_itemInfoList.end()) {
				for (it = pDoc->m_myPartsList.begin(); it != pDoc->m_myPartsList.end(); ++it) {
					if (&it->m_typeId == pTypeId)
						break;
				}

				if (it == pDoc->m_myPartsList.end())
					bFound = false;
			}

			if (bFound) {
				kind = it->m_stateBits.kind;
				days = it->m_remainHours / 24;
				hours = it->m_remainHours % 24;
				bExpired = it->m_stateBits.expired;
			}
		}

		if (kind > 0) {
			bPeriod = true;
			pName->AddText("\n", false, true);
		}

		switch (kind) {
		case 1:
			pName->AddText(MakeStr("\xBB\xE7\xBF\xEB\xB1\xE2\xB0\xA3: %d\xC0\xCF %d\xBD\xC3\xB0\xA3", days, hours), false, true);
			break;
		case 2:
		case 3:
		case 4:
		case 5:
			if (bExpired) {
				pName->AddText("\\c0xffff0000\\c\xBB\xE7\xBF\xEB\xB1\xE2\xB0\xA3\\c0xff000000\\c\xC0\xCC \\c0xffff0000\\c\xB8\xB8\xB7\xE1\\c0xff000000\\c\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.\n", false, true);
			} else if (IsLocalContent((localContentType_t)0x14) && pItem->m_typeId == 0x1A000042) {
				pName->AddText(MakeStr("\xBF\xAD\xB6\xF7 \xB0\xA1\xB4\xC9 \xB1\xE2\xB0\xA3: %d\xC0\xCF %d\xBD\xC3\xB0\xA3\n", days, hours), false, true);
			} else if (days == 0 && hours == 0) {
				pName->AddText("\xB3\xB2\xC0\xBA\xBD\xC3\xB0\xA3: 1\xBD\xC3\xB0\xA3\xB9\xCC\xB8\xB8\n", false, true);
			} else {
				pName->AddText(MakeStr("\xB3\xB2\xC0\xBA\xBD\xC3\xB0\xA3: %d\xC0\xCF %d\xBD\xC3\xB0\xA3\n", days, hours), false, true);
			}
			break;
		}
	}

	if (IsLocalContent((localContentType_t)0x51)) {
		char szDesc[512] = { 0 };

		FrEdit* pAbility = DYNAMIC_CAST(FrEdit, pForm->FindChildByName("avility"));
		FrStatic* pStaticAbility = DYNAMIC_CAST(FrStatic, pForm->FindChildByName("static_avility"));

		if (pAbility) {
			WRect rect;
			if (bPeriod == true) {
				pAbility->AddText("\n\n", false, true);

				rect = pStaticAbility->GetRect();
				rect.y += 25.0f;
				pStaticAbility->SetRect(rect);
			} else {
				rect = pStaticAbility->GetRect();
				rect.y -= 3.0f;
				pStaticAbility->SetRect(rect);
			}

			IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(*pTypeId);
			if (pPart) {
				for (int i = 0; i < 5; i++) {
					if (pPart->m_baseStat[i] != 0 || pPart->m_slotStat[i] != 0)
						pAbility->AddText(MakeStr("\\c0xff0000ff\\c%s\\c0xff000000\\c", MakeStatusString(i, pPart->m_baseStat[i], pPart->m_slotStat[i])), false, true);
				}
			}

			for (std::list<sSCardAvilityPeriodInfo>::iterator it = CSharedDoc::Instance()->m_cardPeriodList[0].begin(); it != CSharedDoc::Instance()->m_cardPeriodList[0].end(); ++it) {
				if (it->partsTypeId == *pTypeId && it->partsId == serial) {
					if (it->kind == 0) {
						IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(it->typeId);
						if (pCard) {
							for (int i = 0; i < 5; i++) {
								if (pCard->m_stat[i] > 0)
									pAbility->AddText(MakeStr("\\c0xff60ee0e\\c%s\\c0xff000000\\c", MakeCardStatusString(i, pCard->m_stat[i])), false, true);
							}
						}
					} else if (it->kind == 1) {
						IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(it->typeId);
						if (pDesc) {
							strcpy(szDesc, pDesc->m_text);

							for (int i = 0; i < 512; i++) {
								if (szDesc[i] == '\n') {
									szDesc[i] = 0;
									break;
								}
							}

							pAbility->AddText(MakeStr("\\c0xffff0000\\c%s\\c0xff000000\\c", szDesc), false, true);
						}
					}
				}
			}
		}
	}

	IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(pItem->m_typeId);
	if (pDesc) {
		pForm->SetMessage(pDesc->m_text, false);
	}

	pForm->Open(0, 3);
}

// realmyroommainui.cpp:11599
void CRealMyRoomMain::EquipItem(sItemInfo* pItemInfo) {
	int numSlots = 10;
	int cardSlots = 0;
	int i;

	if (IsLocalContent((localContentType_t)3)) {
		numSlots = WSingleton<CSharedDoc>::Instance()->GetNumAddItemSlotByMascot() + 8;
	}

	if (IsLocalContent((localContentType_t)0x51)) {
		WSingleton<CCardManager>::Instance()->SetPlayerIndex(0xFF);
		WSingleton<CCardManager>::Instance()->CalcCardPeriodAndStatus();
		cardSlots = WSingleton<CCardManager>::Instance()->GetCardPeriodSlot();
		if (cardSlots > 0) {
			numSlots += cardSlots;
		} else if (WSingleton<CSharedDoc>::Instance()->GetNumAddItemSlotByMascot() == 1) {
			WSingleton<CSharedDoc>::Instance()->m_equipItem[9] = 0;
		} else {
			WSingleton<CSharedDoc>::Instance()->m_equipItem[8] = 0;
			WSingleton<CSharedDoc>::Instance()->m_equipItem[9] = 0;
		}
	}

	for (i = 0; i < numSlots; i++) {
		if (WSingleton<CSharedDoc>::Instance()->m_equipItem[i] == 0) {
			break;
		}
	}

	if (i == numSlots) {
		this << MsgObject(0, 0x23, (intptr_t)"\xB4\xF5\x20\xC0\xCC\xBB\xF3\x20\xC0\xE5\xC2\xF8\xC7\xD2\x20\xBC\xF6\x20\xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
		return;
	}

	WSingleton<CSharedDoc>::Instance()->m_equipItem[i] = pItemInfo->m_typeId;

	int equipped = 0;
	for (int j = 0; j < 10; j++) {
		if (pItemInfo->m_typeId == WSingleton<CSharedDoc>::Instance()->m_equipItem[j]) {
			equipped++;
		}
	}

	if (pItemInfo->m_count - equipped <= 0) {
		HidePet(true);

		for (FrListBox::ITEM_LIST::iterator it = m_pUccCopyHideWnd->m_itemList.begin(); it != m_pUccCopyHideWnd->m_itemList.end(); ++it) {
			sItemInfo* pInfo = (sItemInfo*)(*it)->pData;
			if (pInfo == 0) {
				continue;
			}
			if (pItemInfo->m_typeId == pInfo->m_typeId) {
				m_pUccCopyHideWnd->_DelItem(it);
				break;
			}
		}

		m_pUccCopyHideWnd->SortItem(EmptyCompares);
	}
}

// realmyroommainui.cpp:11666
void CRealMyRoomMain::UnequipItem(unsigned long* pItemId) {
	for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {
		if (it->m_typeId == *pItemId) {
			if (!IsInItemList(*pItemId)) {
				m_pUccCopyHideWnd->AddItem(&*it);
				m_pUccCopyHideWnd->SortItem(EmptyCompares);
			}
		}
	}

	*pItemId = 0;
}

// realmyroommainui.cpp:11682
bool CRealMyRoomMain::IsInItemList(unsigned long typeId) {
	for (FrListBox::ITEM_LIST::iterator it = m_pUccCopyHideWnd->m_itemList.begin(); it != m_pUccCopyHideWnd->m_itemList.end(); ++it) {
		FrListItem* pItem = *it;
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;

		if (pInfo == 0) {
			continue;
		}

		if (pInfo->m_typeId == typeId) {
			return true;
		}
	}

	return false;
}

// realmyroommainui.cpp:11699
void CRealMyRoomMain::SafeLoadPet(unsigned long* pTypeId, bool bForce) {
	m_petLoader.WaitUntilWorkDone(9999);

	if (!bForce) {
		if (m_pPetExhibition != NULL) {
			if (m_pPetExhibition->GetTypeId() == *pTypeId) {
				return;
			}
		}
	}

	m_petLoader.Work(this, 0, pTypeId, true, -1);
}

// realmyroommainui.cpp:11719
void CRealMyRoomMain::TurnOnItemStatus(void) {
	if (m_pItemStatusWnd) {
		m_pItemStatusWnd->Enable(true);
	}
	m_statTab = STAT_TAB_PART;
	SetLevelBar();
}

// realmyroommainui.cpp:11727
void CRealMyRoomMain::TurnOffItemStatus(void) {
	if (m_pItemStatusWnd) {
		m_pItemStatusWnd->Enable(false);
	}
	m_statTab = STAT_TAB_ALL;
	SetLevelBar();
}

// realmyroommainui.cpp:11735
void CRealMyRoomMain::AutoScroll(void) {
	if (m_pUccCopyHideWnd && m_pUccCopyHideWnd->GetScrollBar()) {
		m_pUccCopyHideWnd->GetScrollBar()->SetCurTopRow(0);
	}
}

// realmyroommainui.cpp:11741
bool CRealMyRoomMain::OnFortuneOpenDlgResult(int result, FrForm* pForm) {
	std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end();

	if (m_pGiftOpenDlg) {
		unsigned long typeId = m_pGiftOpenDlg->GetTypeId();
		std::list<sItemInfo>::iterator end = it;
		for (it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != end; ++it) {
			if (it->m_typeId == typeId) break;
		}
	}

	m_pGiftOpenDlg = NULL;

	if (it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end()) {
		it->m_count--;
		if (it->m_count <= 0) {
			WSingleton<CSharedDoc>::Instance()->m_itemInfoList.erase(it);
		}
	}

	this << MsgObject(0, 0xE1, 0, 0, 0, 0, 0);

	return true;
}

// realmyroommainui.cpp:11894
bool CRealMyRoomMain::OnFortuneDlgResult(int result, FrForm* pForm) {
	if (result == 1) {
		this << MsgObject(0, 1, 0, 0, 0, 0, 0);

		WSendPacket packet(GCT_OPEN_FORTUNE);
		packet.Encode4(m_pFortuneDlg->GetTypeId());
		packet.Send(SEND_TO_DEFAULT);
	}

	m_pFortuneDlg = NULL;

	return true;
}

// realmyroommainui.cpp:11934
void CRealMyRoomMain::HandleMsg(const MsgObject &msg) {
	// realmyroommainui.cpp:11935
	if ((unsigned long)msg.m_reserved > g_CurrentTime) {
		// realmyroommainui.cpp:11937
		StoreDelayedMsg(msg);
		// realmyroommainui.cpp:11938
		return;
	}

	// realmyroommainui.cpp:11941
	switch (msg.m_msg) {
	case 0x3:
	case 0xAA:
	case 0xC0:
		break;

	case 0xC1:
		break;

	case 0xD5:
	case 0xD6:
	case 0xD7:
	case 0xD8:
		break;

	case 0xD9:
	case 0xDA:
	case 0xDB:
	case 0xDC:
		break;

	case 0xDE:
	case 0xE8:
		break;

	case 0xE9:
	case 0x1FC:
	case 0x211:
	case 0x232:
		break;

	case 0xE4: {
		// realmyroommainui.cpp:11945, a player entered the room: param1 its
		// sSlotInfo, param2 its sCharacterInfo
		const sSlotInfo *pSlotInfo = (const sSlotInfo *)msg.m_param1;
		// realmyroommainui.cpp:11946
		const sCharacterInfo *pCharInfo = (const sCharacterInfo *)msg.m_param2;
		// realmyroommainui.cpp:11948
		CRealMyRoom_Avatar *pAvatar = WSingleton<CRealMyRoom_AvatarManager>::Instance()->CreateAvatar(*pSlotInfo, *pCharInfo, this);
		// realmyroommainui.cpp:11950
		static_cast<CRealMyRoomTask *>(GetTask())->AddAvatarBurden(CRealMyRoom_AvatarBurden(pSlotInfo->m_uid, *pCharInfo, 0));

		// realmyroommainui.cpp:11952
		if (pSlotInfo->m_uid == WSingleton<CSharedDoc>::Instance()->m_userInfo.guid) {
			// realmyroommainui.cpp:11954
			WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar = pAvatar;
			// realmyroommainui.cpp:11955
			WSingleton<CSharedDoc>::Instance()->m_myCharacterId = pCharInfo->id;
			// realmyroommainui.cpp:11956
			WSingleton<CSharedDoc>::Instance()->SendTradeCloseShop();

			// realmyroommainui.cpp:11958
			WPolySoup::sPoint *point = GOLFDOC()->m_pPolySoup->FindPoint(MakeStr("\xb8\xb6\xc0\xcc\xb7\xeb\xbd\xc3\xc0\xdb\xc1\xa1"));
			// realmyroommainui.cpp:11959
			if (point == NULL)
				// realmyroommainui.cpp:11960
				point = GOLFDOC()->m_pPolySoup->FindPoint(MakeStr("\xb4\xeb\xc8\xad\xb9\xe6\xbd\xc3\xc0\xdb"));

			// realmyroommainui.cpp:11964
			WVector pos = point->m_pos;
			// realmyroommainui.cpp:11968
			pos = GetPVS().GetGroundPoint(point->m_pos, false, 0, false);

			// realmyroommainui.cpp:11973
			WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetPosition(pos);
			// realmyroommainui.cpp:11975
			WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetAngle(0.0f);
			// realmyroommainui.cpp:11976
			WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->Stop();

			// realmyroommainui.cpp:11978
			m_avatarViewPos = WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->m_position;
			// realmyroommainui.cpp:11979
			m_avatarViewPos.y += 4.6f;
			m_avatarViewPos.z -= 46.0f;

			// realmyroommainui.cpp:11984
			if (MyUID() == WSingleton<CSharedDoc>::Instance()->m_myRoomOwnerUID) {
				SetUIState(1);
				// realmyroommainui.cpp:11985
				if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar) {
					WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetState(2);
					WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetHide(false);
				}
				// realmyroommainui.cpp:11986
				SetCameraDisplayType(1);
				// realmyroommainui.cpp:11988
				break;
			}

			// realmyroommainui.cpp:11991
			SetUIState(0);
			// realmyroommainui.cpp:11992
			SetCameraDisplayType(4);
			// realmyroommainui.cpp:11993
			if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar) {
				WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->ChangeCharacterInfo();
				WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetState(0);
				WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetHide(true);
			}
			// realmyroommainui.cpp:11997
			break;
		}

		// realmyroommainui.cpp:12000
		WPolySoup::sPoint *point = GOLFDOC()->m_pPolySoup->FindPoint(MakeStr("\xb8\xb6\xc0\xcc\xb7\xeb\xbd\xc3\xc0\xdb\xc1\xa1"));
		// realmyroommainui.cpp:12001
		if (point == NULL)
			// realmyroommainui.cpp:12002
			point = GOLFDOC()->m_pPolySoup->FindPoint(MakeStr("\xb4\xeb\xc8\xad\xb9\xe6\xbd\xc3\xc0\xdb"));

		// realmyroommainui.cpp:12006
		WVector pos = point->m_pos;
		pAvatar->SetPosition(pos);
		// realmyroommainui.cpp:12007
		pAvatar->SetAngle(pSlotInfo->m_angle);
		// realmyroommainui.cpp:12012
		break;
	}

	case 0xE5: {
		// realmyroommainui.cpp:12015, a player left the room: param1 its guid
		CRealMyRoom_Avatar *pAvatar = WSingleton<CRealMyRoom_AvatarManager>::Instance()->FindAvatarByGuid(msg.m_param1);
		// realmyroommainui.cpp:12016
		if (pAvatar == NULL)
			break;

		// realmyroommainui.cpp:12021
		if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar) {
			// realmyroommainui.cpp:12023
			if (strcmp(pAvatar->GetIDName(), WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->GetIDName()) == 0) {
				// realmyroommainui.cpp:12025
				WSingleton<CSharedDoc>::Instance()->SendTradeCloseShop();
				// realmyroommainui.cpp:12026
				WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SubState(0x7E);
				// realmyroommainui.cpp:12027
				WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar = NULL;
			}

			// realmyroommainui.cpp:12030
			if (IsLocalContent((localContentType_t)10)) {
				// realmyroommainui.cpp:12032
				std::map<unsigned long, sBriefUserInfo>::const_iterator it = WSingleton<CSharedDoc>::Instance()->m_userMap.find(pAvatar->m_guid);
				// realmyroommainui.cpp:12035
				if (sBriefUserInfo(it->second).identityBits.administrator) {
					// realmyroommainui.cpp:12037
					if (sBriefUserInfo(it->second).invisible) {
						// realmyroommainui.cpp:12039
						std::string text(MakeStr("%s \xb4\xd4\xc0\xcc \xc5\xf0\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.", pAvatar->GetNickName()));
						// realmyroommainui.cpp:12040
						WSingleton<CChatMsg>::Instance()->AddChatMsg(text, 0xFFFF00, false, false);
					}
				} else {
					// realmyroommainui.cpp:12045
					std::string text(MakeStr("%s \xb4\xd4\xc0\xcc \xc5\xf0\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.", pAvatar->GetNickName()));
					// realmyroommainui.cpp:12046
					WSingleton<CChatMsg>::Instance()->AddChatMsg(text, 0xFFFF00, false, false);
				}
			} else {
				// realmyroommainui.cpp:12049
				std::string text(MakeStr("%s \xb4\xd4\xc0\xcc \xc5\xf0\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.", pAvatar->GetNickName()));
				// realmyroommainui.cpp:12050
				WSingleton<CChatMsg>::Instance()->AddChatMsg(text, 0xFFFF00, false, false);
			}
		}

		// realmyroommainui.cpp:12054
		if (IsLocalContent((localContentType_t)10)) {
			// realmyroommainui.cpp:12059
			std::map<unsigned long, sBriefUserInfo>::const_iterator it = WSingleton<CSharedDoc>::Instance()->m_userMap.find(pAvatar->m_guid);
			// realmyroommainui.cpp:12061
			if (sBriefUserInfo(it->second).identityBits.administrator) {
				// realmyroommainui.cpp:12063
				if (sBriefUserInfo(it->second).invisible) {
					CFxSequence *pSeq = CFx::Instance()->OpenSequence("Talk_Room.seq", true);
					if (pSeq)
						pSeq->Pos() = WVector(pAvatar->m_position.x, pAvatar->m_position.y + 5.0f, pAvatar->m_position.z);
				}
			} else {
				CFxSequence *pSeq = CFx::Instance()->OpenSequence("Talk_Room.seq", true);
				if (pSeq)
					pSeq->Pos() = WVector(pAvatar->m_position.x, pAvatar->m_position.y + 5.0f, pAvatar->m_position.z);
			}
		} else {
			// realmyroommainui.cpp:12079
			CFxSequence *pSeq = CFx::Instance()->OpenSequence("Talk_Room.seq", true);
			// realmyroommainui.cpp:12080
			if (pSeq)
				// realmyroommainui.cpp:12081
				pSeq->Pos() = WVector(pAvatar->m_position.x, pAvatar->m_position.y + 5.0f, pAvatar->m_position.z);
		}

		// realmyroommainui.cpp:12084
		static_cast<CRealMyRoomTask *>(GetTask())->RemoveAvatarBurden(CRealMyRoom_AvatarBurden(pAvatar->m_guid, pAvatar->m_charInfo, pAvatar->m_pPetFrame));
		// realmyroommainui.cpp:12086
		WSingleton<CRealMyRoom_AvatarManager>::Instance()->DeleteAvatar(pAvatar->GetIDName());
		// realmyroommainui.cpp:12093
		break;
	}

	case 0x5B: {
		// realmyroommainui.cpp:12096, another avatar's activity: param1 its
		// guid, param2 the kind
		CRealMyRoom_Avatar *pAvatar = WSingleton<CRealMyRoom_AvatarManager>::Instance()->FindAvatarByGuid(msg.m_param1);
		// realmyroommainui.cpp:12097
		if (pAvatar == NULL || pAvatar == WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar)
			break;

		// realmyroommainui.cpp:12099
		switch (msg.m_param2) {
		case 4: {
			// realmyroommainui.cpp:12103, a warp: param3/param4 the x/z
			// position, param5 the angle
			WVector pos(*(const float *)msg.m_param3, 0.0f, *(const float *)msg.m_param4);
			// realmyroommainui.cpp:12104
			pos = GetPVS().GetGroundPoint(pos, false, 0, false);
			// realmyroommainui.cpp:12105
			pAvatar->SetPosition(pos);
			// realmyroommainui.cpp:12106
			pAvatar->SetAngle(*(const float *)msg.m_param5);

			// realmyroommainui.cpp:12108
			if (IsLocalContent((localContentType_t)10)) {
				// realmyroommainui.cpp:12112
				std::map<unsigned long, sBriefUserInfo>::const_iterator it = WSingleton<CSharedDoc>::Instance()->m_userMap.find(msg.m_param1);
				// realmyroommainui.cpp:12114
				if (sBriefUserInfo(it->second).identityBits.administrator) {
					// realmyroommainui.cpp:12116
					if (sBriefUserInfo(it->second).invisible) {
						// realmyroommainui.cpp:12118
						CFxSequence *pSeq = CFx::Instance()->OpenSequence("Talk_Room.seq", true);
						// realmyroommainui.cpp:12119
						if (pSeq)
							// realmyroommainui.cpp:12120
							pSeq->Pos() = WVector(pAvatar->m_position.x, pAvatar->m_position.y + 5.0f, pAvatar->m_position.z);
					}
				}
				// realmyroommainui.cpp:12124
			} else {
				// realmyroommainui.cpp:12126
				CFxSequence *pSeq = CFx::Instance()->OpenSequence("Talk_Room.seq", true);
				// realmyroommainui.cpp:12127
				if (pSeq)
					// realmyroommainui.cpp:12128
					pSeq->Pos() = WVector(pos.x, pos.y + 5.0f, pos.z);
			}
			// realmyroommainui.cpp:12131
			break;
		}

		case 5:
			// realmyroommainui.cpp:12135
			pAvatar->SetAction(msg.m_param3);
			// realmyroommainui.cpp:12136
			break;

		case 6: {
			// realmyroommainui.cpp:12140, a move: param3/param4 the x/z
			// offset, param5 the target angle
			WVector target = pAvatar->GetTargetPosition();
			// realmyroommainui.cpp:12141
			target.x += *(const float *)msg.m_param3;
			// realmyroommainui.cpp:12142
			target.z += *(const float *)msg.m_param4;
			// realmyroommainui.cpp:12143
			pAvatar->SetTargetPosition(target);
			// realmyroommainui.cpp:12144
			pAvatar->SetPendingTargetAngle(*(const float *)msg.m_param5);
			// realmyroommainui.cpp:12145
			break;
		}

		case 7:
			// realmyroommainui.cpp:12149
			pAvatar->ChangeMotion(4, (const char *)msg.m_param3, 0.2f);
			// realmyroommainui.cpp:12150
			break;

		case 8:
			// realmyroommainui.cpp:12154
			pAvatar->SetState(msg.m_param3);
			break;

		case 9: {
			int unused = msg.m_param3;
			break;
		}
		}
		// realmyroommainui.cpp:12173
		break;
	}

	case 0x0:
		// realmyroommainui.cpp:12191
		if (g_pFresh) {
			// realmyroommainui.cpp:12194
			g_pFresh->OpenLayout((const char*)msg.m_param1, this, false);
		}
		// realmyroommainui.cpp:12197
		break;

	case 0x16:
		// realmyroommainui.cpp:12200
		m_entryFlag = msg.m_param1 != 0;
		// realmyroommainui.cpp:12202
		break;

	case 0x17:
		// realmyroommainui.cpp:12205
		*(int*)msg.m_param1 = m_entryFlag;
		// realmyroommainui.cpp:12207
		break;

	case 0xC4:
		// realmyroommainui.cpp:12213
		if ((int)msg.m_param1 < 7) {
			// realmyroommainui.cpp:12215
			if (msg.m_param1 == 5)
				// realmyroommainui.cpp:12216
				m_mainTab = (mainTab_t)6;
			else
				// realmyroommainui.cpp:12218
				m_mainTab = (mainTab_t)msg.m_param1;
		} else {
			// realmyroommainui.cpp:12222
			m_mainTab = (mainTab_t)0;
			// realmyroommainui.cpp:12223
			m_roomMode = 1;
		}

		// realmyroommainui.cpp:12226
		if (msg.m_param2 == 0)
			// realmyroommainui.cpp:12227
			m_roomMode = 0x15;
		// realmyroommainui.cpp:12231
		else if (msg.m_param1 == 6)
			// realmyroommainui.cpp:12232
			m_roomMode = 0x20;
		else
			// realmyroommainui.cpp:12234
			m_roomMode = msg.m_param2;
		// realmyroommainui.cpp:12237
		break;

	case 0xC3:
		// realmyroommainui.cpp:12241
		*(int*)msg.m_param1 = m_mainTab;
		// realmyroommainui.cpp:12242
		*(int*)msg.m_param2 = m_roomMode;
		// realmyroommainui.cpp:12244
		break;

	case 0xC6: {
		// realmyroommainui.cpp:12251
		int menu = msg.m_param1;
		if (menu == 0) {
			// realmyroommainui.cpp:12253
			if (IsLocalContent((localContentType_t)0x60)) {
				// realmyroommainui.cpp:12255
				SelectMenu(1);
			} else {
				// realmyroommainui.cpp:12262
				m_mainTab = (mainTab_t)5;
				m_roomMode = 0x11;
				ChangeMainTab((mainTab_t)5, true);
				// realmyroommainui.cpp:12263
				ChangeSubTab((subTab_t)0x11, true);
				// realmyroommainui.cpp:12264
				TurnOffItemStatus();
				m_petLoader.WaitUntilWorkDone(9999);
				if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = NULL; }
			}
		} else if (menu == 1) {
			// realmyroommainui.cpp:12271
			m_mainTab = (mainTab_t)0;
			m_roomMode = 0x15;
			ChangeMainTab((mainTab_t)0, true);
			// realmyroommainui.cpp:12272
			ChangeSubTab((subTab_t)0x15, true);
			// realmyroommainui.cpp:12273
			ChangeCharacter(WSingleton<CSharedDoc>::Instance()->m_myCharacterId);
		} else if (menu == 2) {
			// realmyroommainui.cpp:12278
			if (IsLocalContent((localContentType_t)0x4F)) {
				// realmyroommainui.cpp:12282
				m_mainTab = (mainTab_t)5;
				m_roomMode = 0x17;
				ChangeMainTab((mainTab_t)5, true);
				// realmyroommainui.cpp:12283
				ChangeSubTab((subTab_t)0x17, true);
				m_petLoader.WaitUntilWorkDone(9999);
				if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = NULL; }
			}
		} else if (menu == 3) {
			// realmyroommainui.cpp:12290
			if (IsLocalContent((localContentType_t)0x57)) {
				// realmyroommainui.cpp:12294
				m_mainTab = (mainTab_t)0;
				m_roomMode = 0x1A;
				ChangeMainTab((mainTab_t)0, true);
				// realmyroommainui.cpp:12295
				ChangeSubTab((subTab_t)0x1A, true);
				// realmyroommainui.cpp:12296
				ChangeCharacter(WSingleton<CSharedDoc>::Instance()->m_myCharacterId);
			}
		} else if (menu == 4) {
			// realmyroommainui.cpp:12302
			if (IsLocalContent((localContentType_t)0x51)) {
				// realmyroommainui.cpp:12306
				m_mainTab = (mainTab_t)1;
				m_roomMode = 0x1B;
				ChangeMainTab((mainTab_t)1, true);
				// realmyroommainui.cpp:12307
				ChangeSubTab((subTab_t)0x1B, true);
				// realmyroommainui.cpp:12308
				m_petLoader.WaitUntilWorkDone(9999);
				if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = NULL; }
			}
		} else if (menu == 5) {
			// realmyroommainui.cpp:12313
			if (IsLocalContent((localContentType_t)0x56)) {
				// realmyroommainui.cpp:12318
				m_mainTab = (mainTab_t)6;
				m_roomMode = 0x20;
				ChangeMainTab((mainTab_t)6, true);
				// realmyroommainui.cpp:12319
				ChangeSubTab((subTab_t)0x20, true);
				// realmyroommainui.cpp:12320
				m_petLoader.WaitUntilWorkDone(9999);
				if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = NULL; }
				// realmyroommainui.cpp:12321
				OnTabMainButtonUp();
			}
		} else if (menu == 6) {
			// realmyroommainui.cpp:12326
			if (IsLocalContent((localContentType_t)0x71))
				// realmyroommainui.cpp:12328
				SelectMenu(0xB);

			// realmyroommainui.cpp:12332
			m_mainTab = (mainTab_t)0;
			m_roomMode = 0x15;
			ChangeMainTab((mainTab_t)0, true);
			// realmyroommainui.cpp:12333
			ChangeSubTab((subTab_t)0x15, true);
			// realmyroommainui.cpp:12334
			ChangeCharacter(WSingleton<CSharedDoc>::Instance()->m_myCharacterId);
		}
		// realmyroommainui.cpp:12336
		break;
	}

	case 0xC7:
		// realmyroommainui.cpp:12348
		if (m_notifyOneDlg) {
			// realmyroommainui.cpp:12350
			m_notifyOneDlg->Close(FrOK, true);
			// realmyroommainui.cpp:12351
			m_notifyOneDlg = NULL;
		}
		// realmyroommainui.cpp:12353
		break;

	case 0xC8: {
		// realmyroommainui.cpp:12358, a character was bought: param1 its id,
		// param2 its type id
		sCharacterInfo info;
		memset(&info, 0, sizeof(info));
		// realmyroommainui.cpp:12361
		info.id = msg.m_param1;
		info.typeId = msg.m_param2;
		info.shirtColor = (info.shirtColor & 0xF) | 0x10;
		// realmyroommainui.cpp:12362
		ItemManager()->GetDefCombo(info.typeId, info.partTypeIds);
		// realmyroommainui.cpp:12364
		WSingleton<CSharedDoc>::Instance()->m_characterMap.insert(std::map<unsigned int, sCharacterInfo>::value_type(info.id, info));
		// realmyroommainui.cpp:12366
		break;
	}

	case 0xCA: {
		// realmyroommainui.cpp:12371, a part was bought: param1 its serial,
		// param2 its type id, param3 its days, param4 its kind, param5 the
		// sBoughtItem
		sItemInfo item;
		memset(&item, 0, sizeof(item));
		// realmyroommainui.cpp:12372
		item.m_serial = msg.m_param1;
		// realmyroommainui.cpp:12373
		item.m_typeId = msg.m_param2;
		// realmyroommainui.cpp:12376
		item.m_remainHours = msg.m_param3 * 24;
		item.m_stateBits.kind = msg.m_param4;
		item.m_stateBits.flag0 = 1;

		if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(item.m_typeId)) {
			strncpy(item.m_uccKey, ((const sBoughtItem *)msg.m_param5)->uccKey, 9);
			item.m_uccSeq = 1;
			WSingleton<CSharedDoc>::Instance()->m_myItemList.push_back(item);
		} else {
			// realmyroommainui.cpp:12387
			WSingleton<CSharedDoc>::Instance()->AddMyPartsList(item);
		}
		// realmyroommainui.cpp:12390
		break;
	}

	case 0xCB: {
		// realmyroommainui.cpp:12396, a caddie was bought: param1 its id,
		// param2 its type id, param3 its experience, param4 its level
		sCaddieInfo caddie;
		memset(&caddie, 0, sizeof(caddie));
		// realmyroommainui.cpp:12399
		caddie.id = msg.m_param1;
		caddie.typeId = msg.m_param2;
		caddie.flags |= 3;
		caddie.days = 30;

		// realmyroommainui.cpp:12402
		if (caddie.typeId >= 0x1C00000C && caddie.typeId <= 0x1C00000E) {
			// realmyroommainui.cpp:12407
			caddie.level = (unsigned char)msg.m_param4;
			// realmyroommainui.cpp:12408
			caddie.experience = msg.m_param3;
		}

		// realmyroommainui.cpp:12412
		WSingleton<CSharedDoc>::Instance()->m_caddieMap.insert(std::map<unsigned int, sCaddieInfo>::value_type(caddie.id, caddie));
		// realmyroommainui.cpp:12414
		break;
	}

	case 0xCC: {
		// realmyroommainui.cpp:12422, a caddie item was bought: param1 the
		// caddie's id, param2 the item's type id, param3 its hours
		unsigned int caddieId = msg.m_param1;
		CSharedDoc *pDoc = WSingleton<CSharedDoc>::Instance();
		int hours = msg.m_param3;
		std::map<unsigned int, sCaddieInfo>::iterator it = pDoc->m_caddieMap.find(caddieId);
		// realmyroommainui.cpp:12423
		if (it == pDoc->m_caddieMap.end())
			break;

		// realmyroommainui.cpp:12429
		it->second.cadItemTypeId = msg.m_param2;
		it->second.hours = (unsigned short)hours;
		if (it->second.days < hours)
			// realmyroommainui.cpp:12431
			it->second.days = (unsigned short)hours;
		// realmyroommainui.cpp:12435
		break;
	}

	case 0xC9: {
		// realmyroommainui.cpp:12442, a hair colour was bought: param2 its
		// type id
		unsigned long hairTypeId = msg.m_param2;
		sCharacterInfo &myCharacter = WSingleton<CSharedDoc>::Instance()->m_characterMap[WSingleton<CSharedDoc>::Instance()->m_myCharacterId];
		// realmyroommainui.cpp:12444
		IFF_STRUCT::sHairStyle *pHair = ItemManager()->FindHairStyle(hairTypeId);
		// realmyroommainui.cpp:12445
		if (pHair == NULL)
			break;

		// realmyroommainui.cpp:12451
		for (std::map<unsigned int, sCharacterInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++it) {
			// realmyroommainui.cpp:12455
			if (it->second.typeId == (pHair->m_style | 0x4000000)) {
				// realmyroommainui.cpp:12457
				it->second.hairColor = pHair->m_color;
				break;
			}
		}

		// realmyroommainui.cpp:12462
		if (IsLocalContent((localContentType_t)0x60)) {
			// realmyroommainui.cpp:12465
			CPartTidList &partTidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();
			// realmyroommainui.cpp:12466
			m_pPetExhibition->GetPetFrame()->SetHairClr(&partTidList, pHair->m_color);
		}
		// realmyroommainui.cpp:12469
		break;
	}

	case 0xCD: {
		// realmyroommainui.cpp:12476, a mascot was bought: param1 its id,
		// param2 its type id, param5 the sBoughtItem
		sMascotInfo mascot;
		memset(&mascot, 0, sizeof(mascot));
		mascot.id = msg.m_param1;
		mascot.typeId = msg.m_param2;
		strcpy(mascot.message, "PANGYA!");
		// realmyroommainui.cpp:12478
		const sBoughtItem *pBought = (const sBoughtItem *)msg.m_param5;
		// realmyroommainui.cpp:12479
		mascot.remainHours = pBought->hours;
		// realmyroommainui.cpp:12480
		mascot.endTime = pBought->endTime;
		// realmyroommainui.cpp:12482
		WSingleton<CSharedDoc>::Instance()->AddMascot(&mascot);
		// realmyroommainui.cpp:12484
		break;
	}

	case 0xE0: {
		// realmyroommainui.cpp:12489, a card was bought: param1 its id,
		// param2 its type id, param5 the sBoughtItem
		unsigned long id = msg.m_param1;
		unsigned long typeId = msg.m_param2;
		// realmyroommainui.cpp:12492
		const sBoughtItem *pBought = (const sBoughtItem *)msg.m_param5;
		if (ItemManager()->FindCard(typeId) == NULL)
			break;

		// realmyroommainui.cpp:12502
		int count = pBought->count;
		// realmyroommainui.cpp:12505
		sCards card;
		memset(&card, 0, sizeof(card));
		card.m_guid = id;
		card.m_typeId = typeId;
		card.m_count = count;
		for (std::list<sCards>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myCardList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myCardList.end(); ++it) {
			// realmyroommainui.cpp:12508
			if (it->m_typeId == typeId) {
				// realmyroommainui.cpp:12510
				it->m_count += count;
				// realmyroommainui.cpp:12515
				return;
			}
		}
		// realmyroommainui.cpp:12516
		WSingleton<CSharedDoc>::Instance()->m_myCardList.push_back(card);
		// realmyroommainui.cpp:12518
		break;
	}

	case 0xCE: {
		// realmyroommainui.cpp:12523, an item was bought: param1 its serial,
		// param2 its type id, param3 its period, param4 its period kind,
		// param5 the sBoughtItem
		unsigned long typeId = msg.m_param2;
		if (typeId == 0x1A000010) {
			// realmyroommainui.cpp:12525, pang
			WSingleton<CSharedDoc>::Instance()->m_myPang += msg.m_param3;
			// realmyroommainui.cpp:12526
			break;
		}

		// realmyroommainui.cpp:12535
		int period = 0;
		unsigned char kind = 0;
		int count;
		std::list<sItemInfo> *pList = NULL;
		switch (typeId >> 26) {
		case 6:
			// realmyroommainui.cpp:12542
			count = ((const sBoughtItem *)msg.m_param5)->count;
			period = msg.m_param3;
			kind = (unsigned char)msg.m_param4;
			// realmyroommainui.cpp:12544
			pList = &WSingleton<CSharedDoc>::Instance()->m_itemInfoList;
			// realmyroommainui.cpp:12546
			break;

		case 5:
			// realmyroommainui.cpp:12553
			count = ((const sBoughtItem *)msg.m_param5)->count;
			period = msg.m_param3;
			kind = (unsigned char)msg.m_param4;
			// realmyroommainui.cpp:12555
			pList = &WSingleton<CSharedDoc>::Instance()->m_myGearList;
			// realmyroommainui.cpp:12557
			break;

		case 0x1C: {
			// realmyroommainui.cpp:12561
			const sBoughtItem *pBought = (const sBoughtItem *)msg.m_param5;
			// realmyroommainui.cpp:12562
			if (ItemManager()->FindAuxPart(pBought->typeId) == NULL)
				break;
			// realmyroommainui.cpp:12567
			count = pBought->count;
			// realmyroommainui.cpp:12568
			pList = &WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3;
			// realmyroommainui.cpp:12570
			break;
		}

		case 0x1D:
			// realmyroommainui.cpp:12574
			if (ItemManager()->FindQuestDrop(typeId) == NULL)
				break;
			// realmyroommainui.cpp:12580
			count = 1;
			pList = &WSingleton<CSharedDoc>::Instance()->m_myExtraItemList4;
			break;
		}

		// realmyroommainui.cpp:12596
		if (pList == NULL)
			break;

		bool found = false;
		// realmyroommainui.cpp:12616
		if (msg.m_param4 == 0) {
			// realmyroommainui.cpp:12618
			for (std::list<sItemInfo>::iterator it = pList->begin(); it != pList->end(); ++it) {
				// realmyroommainui.cpp:12621
				if (it->m_serial == (unsigned long)msg.m_param1) {
					// realmyroommainui.cpp:12623
					int hours = 0;
					// realmyroommainui.cpp:12624
					if (kind) {
						// realmyroommainui.cpp:12626
						if (kind == 3)
							hours = period;
						// realmyroommainui.cpp:12627
						else if (kind == 4)
							hours = period * 24;
						// realmyroommainui.cpp:12628
						else if (kind == 5)
							hours = period * 720;
						// realmyroommainui.cpp:12630
						Net_StartPeriodItem(it->m_serial);
					}
					// realmyroommainui.cpp:12633
					it->m_remainHours += hours;
					// realmyroommainui.cpp:12635
					it->m_count = count;
					if (it->m_stateBits.kind && it->m_remainHours > 0)
						// realmyroommainui.cpp:12637
						Net_StartPeriodItem(it->m_serial);
					found = true;
					break;
				}
			}
		}

		if (!found) {
			// realmyroommainui.cpp:12649
			sItemInfo item;
			memset(&item, 0, sizeof(item));
			// realmyroommainui.cpp:12651
			item.m_serial = msg.m_param1;
			item.m_typeId = msg.m_param2;
			// realmyroommainui.cpp:12652
			item.m_stateBits.flag0 = 1;
			// realmyroommainui.cpp:12653
			item.m_count = count < 1 ? 1 : count;
			// realmyroommainui.cpp:12654
			item.m_stateBits.kind = kind;
			// realmyroommainui.cpp:12655
			if (kind) {
				// realmyroommainui.cpp:12657
				if (kind == 3)
					item.m_remainHours = period;
				// realmyroommainui.cpp:12658
				else if (kind == 4)
					item.m_remainHours = period * 24;
				// realmyroommainui.cpp:12659
				else if (kind == 5)
					item.m_remainHours = period * 720;
				// realmyroommainui.cpp:12661
				Net_StartPeriodItem(item.m_serial);
				// realmyroommainui.cpp:12662
				item.m_stateBits.kind = 2;
			}
			// realmyroommainui.cpp:12664
			pList->push_back(item);
		}

		// realmyroommainui.cpp:12668
		BuildDisplayStand((subTab_t)m_roomMode);
		// realmyroommainui.cpp:12670
		break;
	}

	case 0xCF: {
		// realmyroommainui.cpp:12675, a club set was bought: param1 its
		// serial, param2 its type id
		sItemInfo item;
		memset(&item, 0, sizeof(item));
		// realmyroommainui.cpp:12677
		item.m_serial = msg.m_param1;
		item.m_typeId = msg.m_param2;
		// realmyroommainui.cpp:12679
		WSingleton<CSharedDoc>::Instance()->m_myItemMap.insert(std::map<unsigned int, sItemInfo>::value_type(item.m_serial, item));
		// realmyroommainui.cpp:12681
		break;
	}

	case 0xD0: {
		// realmyroommainui.cpp:12688, a skin was bought: param1 its serial,
		// param2 its type id, param3 its days, param4 its period kind
		sItemInfo item;
		memset(&item, 0, sizeof(item));
		item.m_serial = msg.m_param1;
		item.m_typeId = msg.m_param2;
		// realmyroommainui.cpp:12689
		item.m_stateBits.kind = msg.m_param4;
		// realmyroommainui.cpp:12690
		item.m_remainHours = msg.m_param3 * 24;
		// realmyroommainui.cpp:12731
		WSingleton<CSharedDoc>::Instance()->m_mySkinList.push_back(item);
		// realmyroommainui.cpp:12734
		break;
	}

	case 0xD1: {
		// realmyroommainui.cpp:12752, a gift was taken out: param1 its serial
		unsigned long serial = msg.m_param1;
		for (FrListBox::ITEM_LIST::iterator it = m_pUccCopyHideWnd->m_itemList.begin(); it != m_pUccCopyHideWnd->m_itemList.end(); ++it) {
			// realmyroommainui.cpp:12753
			FrListItem *pItem = *it;
			// realmyroommainui.cpp:12754
			sGiftInfo *pGift = (sGiftInfo *)pItem->pData;
			// realmyroommainui.cpp:12755
			if (pGift == NULL)
				break;
			// realmyroommainui.cpp:12758
			if (pGift->m_serial == serial) {
				// realmyroommainui.cpp:12759
				m_pUccCopyHideWnd->_DelItem(it);
				break;
			}
		}
		// realmyroommainui.cpp:12763
		m_pUccCopyHideWnd->SortItem(EmptyCompares);

		// realmyroommainui.cpp:12766
		for (std::list<sGiftInfo>::iterator it2 = WSingleton<CSharedDoc>::Instance()->m_giftList.begin(); it2 != WSingleton<CSharedDoc>::Instance()->m_giftList.end(); ++it2) {
			// realmyroommainui.cpp:12768
			if (it2->m_serial == serial) {
				// realmyroommainui.cpp:12769
				WSingleton<CSharedDoc>::Instance()->m_giftList.erase(it2);
				break;
			}
			// realmyroommainui.cpp:12773
		}
		// realmyroommainui.cpp:12777
		m_areaFlagB = true;
		// realmyroommainui.cpp:12779
		break;
	}

	case 0xD2:
		// realmyroommainui.cpp:12782, the enchant's result code: param1
		if (m_pEnchantDlg)
			// realmyroommainui.cpp:12783
			m_pEnchantDlg->SetResultCode((unsigned char)msg.m_param1);
		// realmyroommainui.cpp:12785
		SetLevelBar();
		// realmyroommainui.cpp:12786
		break;

	case 0xD3:
		// realmyroommainui.cpp:12789
		if (m_pUpgradeDlg)
			// realmyroommainui.cpp:12790
			m_pUpgradeDlg->SetLevelBar();
		// realmyroommainui.cpp:12791
		if (m_pDowngradeDlg)
			// realmyroommainui.cpp:12792
			m_pDowngradeDlg->SetLevelBar();
		// realmyroommainui.cpp:12793
		SetLevelBar();
		// realmyroommainui.cpp:12794
		break;

	case 0xD4:
		// realmyroommainui.cpp:12797, open the enchant dialog: param1 its
		// type, param2 the grade
		if (m_pEnchantDlg == NULL) {
			// realmyroommainui.cpp:12799
			m_pEnchantDlg = CreateForm<FrEnchantDlg>(g_pFresh->GetManager(), this, "enchant", NULL);
			// realmyroommainui.cpp:12800
			m_pEnchantDlg->SetType((unsigned char)msg.m_param1, (unsigned char)msg.m_param2);
			// realmyroommainui.cpp:12801
			m_pEnchantDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnEnchantDlgResult, 1);
		}
		// realmyroommainui.cpp:12803
		break;

	case 0x26C:
		// realmyroommainui.cpp:12822, the owned characters changed (TaskManager
		// packet 0x69): refresh the copy
		m_characterMap = CSharedDoc::Instance()->m_characterMap;
		// realmyroommainui.cpp:12824
		break;

	case 0x49:
		// realmyroommainui.cpp:12832, a character was equipped: param2 its
		// sCharacterInfo
		m_petLoader.WaitUntilWorkDone(9999);
		// realmyroommainui.cpp:12835
		ChangeCharacter(((const sCharacterInfo *)msg.m_param2)->id);
		// realmyroommainui.cpp:12856
		break;

	case 0x4A: {
		// realmyroommainui.cpp:12864, a caddie was equipped: param2 its
		// sCaddieInfo
		m_petLoader.WaitUntilWorkDone(9999);
		// realmyroommainui.cpp:12866
		const sCaddieInfo *pCaddie = (const sCaddieInfo *)msg.m_param2;
		// realmyroommainui.cpp:12867
		ChangeCaddie(pCaddie->id);
		// realmyroommainui.cpp:12868
		SetLevelBar();
		// realmyroommainui.cpp:12870
		break;
	}

	case 0x4B: {
		// realmyroommainui.cpp:12878, a club set was equipped: param2 its
		// sClubSetRecord
		m_petLoader.WaitUntilWorkDone(9999);
		// realmyroommainui.cpp:12880
		const sClubSetRecord *pClubSet = (const sClubSetRecord *)msg.m_param2;
		// realmyroommainui.cpp:12881
		ChangeClubset(pClubSet->id);
		// realmyroommainui.cpp:12882
		SetLevelBar();
		// realmyroommainui.cpp:12884
		break;
	}

	case 0x4C:
		// realmyroommainui.cpp:12892, a ball was equipped: param2 its type id
		m_petLoader.WaitUntilWorkDone(9999);
		// realmyroommainui.cpp:12894
		ChangeAztec(msg.m_param2);
		// realmyroommainui.cpp:12896
		break;

	case 0x4E: {
		// realmyroommainui.cpp:12902, a mascot was equipped or its message
		// changed: param2 its sMascotInfo
		const sMascotInfo *pMascot = (const sMascotInfo *)msg.m_param2;
		if (CSharedDoc::Instance()->m_myMascotId != pMascot->id) {
			// realmyroommainui.cpp:12908
			m_petLoader.WaitUntilWorkDone(9999);
			// realmyroommainui.cpp:12910
			ChangeMascot(pMascot->id);
			// realmyroommainui.cpp:12911
			CSharedDoc::Instance()->m_myMascotId = pMascot->id;
		}
		// realmyroommainui.cpp:12914
		if (m_pPetExhibition && m_pPetExhibition->GetModel() && pMascot->typeId) {
			// realmyroommainui.cpp:12916
			IFF_STRUCT::sMascot *pIff = ItemManager()->FindMascot(pMascot->typeId);
			// realmyroommainui.cpp:12917
			if (pIff)
				// realmyroommainui.cpp:12919
				CMascot::ChangeMascotMsgInTex(pMascot->message, pIff, m_pPetExhibition ? m_pPetExhibition->GetModel() : NULL, MakeStr("myroom%02d", pIff->m_typeId & 0x3FFFFFF));
		}
		// realmyroommainui.cpp:12923
		if (pMascot->pcBang)
			// realmyroommainui.cpp:12924
			COption::Instance()->mSetPCBangMascotMsg(pMascot->message);
		// realmyroommainui.cpp:12928
		BuildDisplayStand((subTab_t)m_roomMode);
		// realmyroommainui.cpp:12930
		break;
	}

	case 0x68:
		// realmyroommainui.cpp:12935, re-equip after a PC bang change: param1
		// the PC bang item
		if (CLoginInfo::Instance()->IsPcBang()) {
			// realmyroommainui.cpp:12937
			if (GetFlags(1)) {
				// realmyroommainui.cpp:12939
				if (msg.m_param1 == 0)
					// realmyroommainui.cpp:12940
					Reinitialize(true);
				else
					// realmyroommainui.cpp:12943
					ReinitializePC((ePCBangItem)msg.m_param1);
			}
		} else {
			// realmyroommainui.cpp:12949
			Reinitialize(false);
		}

		// realmyroommainui.cpp:12953
		Net_EquipCharacter();
		// realmyroommainui.cpp:12954
		Net_EquipDresserParts();
		// realmyroommainui.cpp:12955
		Net_EquipCaddie();
		// realmyroommainui.cpp:12956
		Net_EquipItem();
		// realmyroommainui.cpp:12957
		Net_EquipGear();
		// realmyroommainui.cpp:12958
		Net_EquipMascot();
		// realmyroommainui.cpp:12963
		break;

	case 0xDF:
		// realmyroommainui.cpp:12967, send every edited equipment
		Net_EquipDresserParts();
		// realmyroommainui.cpp:12968
		Net_EquipCharacter();
		// realmyroommainui.cpp:12969
		Net_EquipCaddie();
		// realmyroommainui.cpp:12970
		Net_EquipItem();
		// realmyroommainui.cpp:12971
		Net_EquipGear();
		// realmyroommainui.cpp:12972
		Net_EquipSkin();
		// realmyroommainui.cpp:12973
		Net_EquipMascot();
		// realmyroommainui.cpp:12978
		break;

	case 0xE1:
		// realmyroommainui.cpp:12982
		BuildDisplayStand((subTab_t)m_roomMode);
		// realmyroommainui.cpp:12984
		break;

	case 0x204:
		// realmyroommainui.cpp:13077, hand the tiki report serial over:
		// param1 receives it
		if (IsLocalContent((localContentType_t)0x14)) {
			// realmyroommainui.cpp:13079
			*(unsigned long *)msg.m_param1 = m_tikiReportSerial;
			// realmyroommainui.cpp:13080
			m_tikiReportSerial = 0;
		}
		// realmyroommainui.cpp:13083
		break;

	case 0x209:
		// realmyroommainui.cpp:13087, the free camera's custom limits: param1
		// 1 the y limits, 2 the x limits; param2/param3 min/max in degrees
		m_customLookAtLimits = msg.m_param1;
		// realmyroommainui.cpp:13088
		if (m_customLookAtLimits == 1) {
			// realmyroommainui.cpp:13090
			m_lookAtYMinDeg = (float)msg.m_param2;
			// realmyroommainui.cpp:13091
			m_lookAtYMaxDeg = (float)msg.m_param3;
			// realmyroommainui.cpp:13093
		} else if (m_customLookAtLimits == 2) {
			// realmyroommainui.cpp:13095
			m_lookAtXMinDeg = (float)msg.m_param2;
			// realmyroommainui.cpp:13096
			if (m_lookAtXMinDeg < -90.0f)
				// realmyroommainui.cpp:13097
				m_lookAtXMinDeg = -90.0f;
			// realmyroommainui.cpp:13098
			m_lookAtXMaxDeg = (float)msg.m_param3;
			// realmyroommainui.cpp:13099
			if (m_lookAtXMaxDeg > 45.0f)
				// realmyroommainui.cpp:13100
				m_lookAtXMaxDeg = 45.0f;
		}
		// realmyroommainui.cpp:13103
		break;

	case 0x20A: {
		// realmyroommainui.cpp:13111, add a model to the room: param1/param2
		// its x/z, param3 its name
		float x = (float)msg.m_param1;
		float z = (float)msg.m_param2;
		char name[32];
		// realmyroommainui.cpp:13114
		strcpy(name, (const char *)msg.m_param3);
		// realmyroommainui.cpp:13116
		GOLFDOC()->m_pPolySoup->SetAdditionModel(name, WVector(x, 0.0f, z));
		// realmyroommainui.cpp:13117
		GOLFDOC()->m_pPolySoup->EndLoad(false);
		// realmyroommainui.cpp:13120
		break;
	}

	case 0x22B:
		// realmyroommainui.cpp:13125
		g_camera.pivot = WVector((float)msg.m_param1, (float)msg.m_param2, (float)msg.m_param3);
		// realmyroommainui.cpp:13132
		break;

	case 0x20B:
		// realmyroommainui.cpp:13141
		if (IsLocalContent((localContentType_t)0x56)) {
			// realmyroommainui.cpp:13143
			if (m_bHideMiddleUI)
				// realmyroommainui.cpp:13144
				ChangeSubTab((subTab_t)0x15, false);
			// realmyroommainui.cpp:13146
			if (MyUID() == WSingleton<CSharedDoc>::Instance()->m_myRoomOwnerUID)
				// realmyroommainui.cpp:13149
				SetMiddleUIControl(!m_bHideMiddleUI);
			// realmyroommainui.cpp:13155
			CharacterViewControl(m_bHideMiddleUI);
		}
		// fall through: the authority dialog closes too

	case 0x220:
		// realmyroommainui.cpp:13162
		if (IsLocalContent((localContentType_t)0x56)) {
			// realmyroommainui.cpp:13172
			if (m_pAuthorityDlg) {
				// realmyroommainui.cpp:13174
				m_pAuthorityDlg->Close(true);
				// realmyroommainui.cpp:13175
				m_pAuthorityDlg = NULL;
			}
		}
		// realmyroommainui.cpp:13179
		break;

	case 0x212:
		// realmyroommainui.cpp:13195
		if (m_pUccDrawDlg)
			// realmyroommainui.cpp:13197
			m_pUccDrawDlg->Upload();
		// realmyroommainui.cpp:13200
		break;

	case 0x214: {
		// realmyroommainui.cpp:13205, the self-design upload result: param1
		// the result, param2 &serial, param3 &type id, param4 the UCC key,
		// param5 the UCC name
		unsigned char result = (unsigned char)msg.m_param1;
		unsigned long serial = *(const unsigned long *)msg.m_param2;
		// realmyroommainui.cpp:13208
		unsigned long typeId = *(const unsigned long *)msg.m_param3;
		const char *uccKey = (const char *)msg.m_param4;
		const char *uccName = (const char *)msg.m_param5;

		// realmyroommainui.cpp:13210
		if (m_pUccDrawDlg == NULL)
			break;

		// realmyroommainui.cpp:13217
		if (m_pUccDrawDlg->m_pItemInfo->m_typeId != typeId || strcmp(m_pUccDrawDlg->m_pItemInfo->m_uccKey, uccKey) != 0)
			break;

		// realmyroommainui.cpp:13225
		m_pUccDrawDlg->m_bSavingTemporarily = false;
		if (result == 0) {
			// realmyroommainui.cpp:13227
			FrForm *pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
			// realmyroommainui.cpp:13228
			pNotify->SetMessage("\xBC\xBF\xC7\xC1\xB5\xF0\xC0\xDA\xC0\xCE \xC0\xC7\xBB\xF3 \xB5\xEE\xB7\xCF\xBF\xA1 \xBD\xC7\xC6\xD0\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			// realmyroommainui.cpp:13229
			pNotify->Open(NULL, 0);
			// realmyroommainui.cpp:13230
			break;
		}

		// realmyroommainui.cpp:13235
		for (int i = 0; i < 2; i++) {
			// realmyroommainui.cpp:13237
			CUccBaseItemDraw *pBase = (CUccBaseItemDraw *)m_pUccDrawDlg->m_pUccBase[i];
			Bitmap *pTexture = pBase->GetTexture();
			char *pName = pBase->GetTextureName();
			// realmyroommainui.cpp:13240
			w_texlist *pTex = g_resrcmng->FindTexture(pName);
			// realmyroommainui.cpp:13241
			if (pTex == NULL)
				// realmyroommainui.cpp:13243
				g_resrcmng->UploadTexture(pName, pTexture, 0, NULL);
			else
				// realmyroommainui.cpp:13245
				g_resrcmng->FixTexture(pTex->texhandle, pTexture, 0, NULL);
		}

		// realmyroommainui.cpp:13255
		sUccClothes clothes;
		SYSTEMTIME now;
		GetSystemTime(&now);
		// realmyroommainui.cpp:13256
		clothes.m_serial = serial;
		// realmyroommainui.cpp:13257
		memcpy(clothes.m_period, &now, sizeof(now));
		clothes.m_downloadCount = 1;
		clothes.m_typeId = typeId;
		// realmyroommainui.cpp:13260
		strncpy(clothes.m_uccKey, uccKey, 9);
		// realmyroommainui.cpp:13261
		strncpy(clothes.m_uccName, uccName, 0x29);
		// realmyroommainui.cpp:13262
		std::map<std::string, Bitmap *>::iterator icon = UccManager()->m_iconMap.find(GenerateIconFilename(typeId, uccKey));
		// realmyroommainui.cpp:13263
		if (icon != UccManager()->m_iconMap.end())
			// realmyroommainui.cpp:13265
			clothes.m_pIcon = icon->second;
		// realmyroommainui.cpp:13267
		UccManager()->AddClothes(1, clothes, true, false);

		// realmyroommainui.cpp:13274
		for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myItemList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myItemList.end(); ++it) {
			// realmyroommainui.cpp:13276
			sItemInfo *pInfo = &*it;
			// realmyroommainui.cpp:13277
			if (pInfo && pInfo->m_typeId == typeId && strcmp(pInfo->m_uccKey, uccKey) == 0) {
				// realmyroommainui.cpp:13279
				pInfo->m_uccStatus &= ~2;
				// realmyroommainui.cpp:13280
				pInfo->m_uccStatus |= 1;
				break;
			}
		}

		// realmyroommainui.cpp:13287
		FrForm *pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		// realmyroommainui.cpp:13288
		pNotify->SetMessage("\xBC\xBF\xC7\xC1\xB5\xF0\xC0\xDA\xC0\xCE \xC0\xC7\xBB\xF3\xC0\xCC \xBF\xCF\xBC\xBA\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.", false);
		// realmyroommainui.cpp:13289
		pNotify->Open(NULL, 0);
		// realmyroommainui.cpp:13291
		m_pUccDrawDlg->Close(FrOK, true);
		// realmyroommainui.cpp:13293
		break;
	}

	case 0x213: {
		// realmyroommainui.cpp:13297, the self-design upload failed: param1
		// &type id, param2 the UCC key, param3 the error code
		unsigned long typeId = *(const unsigned long *)msg.m_param1;
		// realmyroommainui.cpp:13298
		const char *uccKey = (const char *)msg.m_param2;
		// realmyroommainui.cpp:13299
		const char *errorCode = (const char *)msg.m_param3;

		// realmyroommainui.cpp:13309
		const char *errors[] = { "", "ERR_FILE", "ERR_UCC", "ERR_KEY", "ERR_USER", "ERR_SRVVAR" };
		int error = 0;
		// realmyroommainui.cpp:13312
		for (int i = 0; i < sizeof(errors) / sizeof(errors[0]); i++) {
			if (strcmpi(errors[i], errorCode) == 0) {
				// realmyroommainui.cpp:13314
				error = i;
				break;
			}
		}

		// realmyroommainui.cpp:13319
		if (m_pUccDrawDlg == NULL)
			break;

		// realmyroommainui.cpp:13326
		if (m_pUccDrawDlg->m_pItemInfo->m_typeId != typeId || strcmp(m_pUccDrawDlg->m_pItemInfo->m_uccKey, uccKey) != 0)
			break;

		// realmyroommainui.cpp:13332
		FrForm *pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		// realmyroommainui.cpp:13333
		pNotify->SetMessage(MakeStr("\xBE\xF7\xB7\xCE\xB5\xE5\xB0\xA1 \xBD\xC7\xC6\xD0\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9. \xB4\xD9\xBD\xC3 \xBD\xC3\xB5\xB5\xC7\xD8\xC1\xD6\xBC\xBC\xBF\xE4.(\xC4\xDA\xB5\xE5:%d)", error + 10), false);
		// realmyroommainui.cpp:13334
		pNotify->Open(NULL, 1);
		// realmyroommainui.cpp:13336
		m_pUccDrawDlg->CloseWaitingForm();
		// realmyroommainui.cpp:13338
		break;
	}

	case 0x215: {
		// realmyroommainui.cpp:13341, the self-design temporary save result:
		// param1 the result, param2 &type id, param3 the UCC key
		unsigned char result = (unsigned char)msg.m_param1;
		// realmyroommainui.cpp:13342
		unsigned long typeId = *(const unsigned long *)msg.m_param2;
		// realmyroommainui.cpp:13343
		const char *uccKey = (const char *)msg.m_param3;

		// realmyroommainui.cpp:13345
		if (m_pUccDrawDlg == NULL)
			break;

		// realmyroommainui.cpp:13352
		if (m_pUccDrawDlg->m_pItemInfo->m_typeId != typeId || strcmp(m_pUccDrawDlg->m_pItemInfo->m_uccKey, uccKey) != 0)
			break;

		// realmyroommainui.cpp:13361
		m_pUccDrawDlg->m_bSavingTemporarily = false;
		if (result == 1) {
			// realmyroommainui.cpp:13365
			UccManager()->SetUccTempClothes(typeId, uccKey, m_pUccDrawDlg->m_pUccBase[0]->GetTexture32BPP(true), m_pUccDrawDlg->m_pUccBase[1]->GetTexture32BPP(true));
			// realmyroommainui.cpp:13368
			m_pUccDrawDlg->SetSaved(true);
			// realmyroommainui.cpp:13377
			break;
		}

		// realmyroommainui.cpp:13379
		FrForm *pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		// realmyroommainui.cpp:13380
		pNotify->SetMessage("\xBC\xBF\xC7\xC1\xB5\xF0\xC0\xDA\xC0\xCE \xC0\xC7\xBB\xF3\xC0\xC7 \xC0\xD3\xBD\xC3 \xC0\xFA\xC0\xE5\xBF\xA1 \xBD\xC7\xC6\xD0\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.", false);
		// realmyroommainui.cpp:13381
		pNotify->Open(NULL, 1);
		// realmyroommainui.cpp:13384
		break;
	}

	case 0x219: {
		// realmyroommainui.cpp:13405, a card was used: param1 its type id,
		// param2 1 when it failed
		bool failed = msg.m_param2 == 1;
		// realmyroommainui.cpp:13407
		unsigned long typeId = msg.m_param1;
		if (m_barButton[10] && m_specialCardForm == NULL)
			// realmyroommainui.cpp:13409
			m_barButton[10]->EnableBlink(CCardManager::Instance()->ShowSPCardAlarm());

		// realmyroommainui.cpp:13412
		if (!failed) {
			// realmyroommainui.cpp:13414
			if (m_pCardUseDlg == NULL) {
				// realmyroommainui.cpp:13416
				FrCardUseDlg *pDlg = CreateForm<FrCardUseDlg>(g_pFresh->GetManager(), this, "usecard_result", NULL);
				// realmyroommainui.cpp:13422
				pDlg->m_cardTypeId = typeId;
				pDlg->Open(NULL, 0);
			}
			// realmyroommainui.cpp:13426
			if (m_pCardBook)
				// realmyroommainui.cpp:13428
				m_pCardBook->ReBuildCurrentPage();
		}
		// realmyroommainui.cpp:13432
		break;
	}

	case 0x21A:
		// realmyroommainui.cpp:13436
		if (m_pCardBook) {
			// realmyroommainui.cpp:13438
			if (m_pCardBook->IsShow() == true)
				// realmyroommainui.cpp:13439
				m_pCardBook->ReBuildCurrentPage();
		}
		// realmyroommainui.cpp:13442
		BuildCardPackList();
		// realmyroommainui.cpp:13444
		break;

	case 0x229:
		// realmyroommainui.cpp:13450
		if (m_pCardBook)
			// realmyroommainui.cpp:13451
			m_pCardBook->ReBuildCurrentPage();
		// realmyroommainui.cpp:13453
		BuildCardPackList();
		// realmyroommainui.cpp:13455
		break;

	case 0x22A: {
		// realmyroommainui.cpp:13463, the experience (param1) and level
		// (param2)
		unsigned char level = (unsigned char)msg.m_param2;
		WSingleton<CSharedDoc>::Instance()->m_myExp = msg.m_param1;
		// realmyroommainui.cpp:13464
		WSingleton<CSharedDoc>::Instance()->m_myLevel = level;
		// realmyroommainui.cpp:13466
		break;
	}

	case 0x221:
		// realmyroommainui.cpp:13471, open the post writing form
		if (IsLocalContent((localContentType_t)0x56)) {
			// realmyroommainui.cpp:13473
			m_pPostSendDlg = NULL;
			// realmyroommainui.cpp:13475
			m_pPostSendDlg = CreateForm<CPost_SendDlg>(g_pFresh->GetManager(), this, "write_post", NULL);
			// realmyroommainui.cpp:13477
			FrForm::ms_bHasTail = false;
			m_pPostSendDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnPostSendDlgResult, 0x81);
			// realmyroommainui.cpp:13478
			m_pPostSendDlg->SetFadeout(false);
		}
		// realmyroommainui.cpp:13481
		break;

	case 0x223:
		// realmyroommainui.cpp:13485
		if (IsLocalContent((localContentType_t)0x56)) {
			// realmyroommainui.cpp:13487
			if (m_pPostDlg)
				// realmyroommainui.cpp:13489
				m_pPostDlg->SetLoadPostList();
		}
		// realmyroommainui.cpp:13494
		break;

	case 0x224:
		// realmyroommainui.cpp:13498
		if (IsLocalContent((localContentType_t)0x56)) {
			// realmyroommainui.cpp:13500
			if (m_pPostDlg)
				// realmyroommainui.cpp:13502
				m_pPostDlg->OnPostDescDlgCreate();
		}
		// realmyroommainui.cpp:13506
		break;

	case 0x226:
		// realmyroommainui.cpp:13510
		if (IsLocalContent((localContentType_t)0x56)) {
			// realmyroommainui.cpp:13512
			if (m_pPostDlg)
				// realmyroommainui.cpp:13513
				m_pPostDlg->OnLoadPostList();
		}
		// realmyroommainui.cpp:13516
		break;

	case 0x225:
		// realmyroommainui.cpp:13521, a post was sent: param1 0x13E
		if (msg.m_param1 == 0x13E) {
			// realmyroommainui.cpp:13525
			FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "rmr_notify", NULL);
			// realmyroommainui.cpp:13526
			pForm->SetMessage("\xBF\xEC\xC6\xED\xB9\xB0\xC0\xBB \xBA\xB8\xB3\xC2\xBD\xC0\xB4\xCF\xB4\xD9. \n\xC6\xED\xC1\xF6 \xBE\xB2\xB1\xE2\xB8\xA6 \xB0\xE8\xBC\xD3 \xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);
			// realmyroommainui.cpp:13527
			pForm->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnPostSendDlg2Result, 2);
		}
		// realmyroommainui.cpp:13532
		break;

	case 0x228: {
		// realmyroommainui.cpp:13539, a my-room request failed or finished:
		// param1 the request, param2 its result
		int request = msg.m_param1;
		FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		// realmyroommainui.cpp:13541
		switch (request) {
		case 0x122:
			// realmyroommainui.cpp:13547
			if (msg.m_param2)
				// realmyroommainui.cpp:13548
				pForm->SetMessage("\xB8\xB6\xC0\xCC\xB7\xEB\xC0\xC7 \xB1\xC7\xC7\xD1\xC0\xBB \xBC\xB3\xC1\xA4\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			// realmyroommainui.cpp:13549
			else
				// realmyroommainui.cpp:13550
				pForm->SetMessage("\xB8\xB6\xC0\xCC\xB7\xEB\xC0\xC7 \xB1\xC7\xC7\xD1\xC0\xBB \xBC\xB3\xC1\xA4\xC7\xCF\xB4\xC2\xB5\xA5 \xBD\xC7\xC6\xD0\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			// realmyroommainui.cpp:13552
			break;

		case 0x124: {
			// realmyroommainui.cpp:13557
			int saved = msg.m_param2;
			pForm->Close(true);
			// realmyroommainui.cpp:13558
			FrForm *pSaveForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
			// realmyroommainui.cpp:13559
			if (saved)
				// realmyroommainui.cpp:13560
				pSaveForm->SetMessage("\xC7\xF6\xC0\xE7 \xB9\xE8\xC4\xA1\xB8\xA6 \xC0\xFA\xC0\xE5\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			// realmyroommainui.cpp:13561
			else
				// realmyroommainui.cpp:13562
				pSaveForm->SetMessage("\xB8\xB6\xC0\xCC\xB7\xEB\xBF\xA1 \xB0\xA1\xB1\xB8\xB8\xA6 \xB3\xF5\xC1\xF6 \xB8\xF8\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.. ", false);
			// realmyroommainui.cpp:13564
			pSaveForm->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnFunritureSaveMsgdlgResult, 3);
			// realmyroommainui.cpp:13566
			break;
		}

		case 0x125:
			// realmyroommainui.cpp:13569
			pForm->SetMessage("\xB8\xB6\xC0\xCC\xB7\xEB\xBF\xA1 \xC0\xD6\xB4\xC2 \xB0\xA1\xB1\xB8 \xC1\xA4\xBA\xB8\xB8\xA6 \xB0\xA1\xC1\xAE\xBF\xC0\xC1\xF6 \xB8\xF8\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			break;
		case 0x123:
			// realmyroommainui.cpp:13570
			pForm->SetMessage("\xB8\xB6\xC0\xCC\xB7\xEB\xBF\xA1 \xB5\xE9\xBE\xEE\xB0\xA5 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			break;
		case 0x141:
			// realmyroommainui.cpp:13571
			pForm->SetMessage("\xBF\xEC\xC6\xED\xC7\xD4\xC0\xC7 \xC6\xED\xC1\xF6\xB5\xE9\xC0\xBB \xC0\xD0\xBE\xEE\xBF\xC0\xC1\xF6 \xB8\xF8\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			break;
		case 0x143:
			// realmyroommainui.cpp:13572
			pForm->SetMessage("\xBF\xEC\xC6\xED\xB9\xB0\xC0\xC7 \xB3\xBB\xBF\xEB\xC0\xBB \xC0\xD0\xBE\xEE\xBF\xC0\xC1\xF6 \xB8\xF8\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			break;
		case 0x145:
			// realmyroommainui.cpp:13573
			pForm->SetMessage("\xBF\xEC\xC6\xED\xB9\xB0\xBF\xA1 \xC3\xB7\xBA\xCE\xB5\xC8 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xBF\xC5\xB1\xE2\xC1\xF6 \xB8\xF8\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			break;

		case 0x15D:
			// realmyroommainui.cpp:13576
			if (msg.m_param2 == 9)
				// realmyroommainui.cpp:13577
				pForm->SetMessage("\xBE\xC6\xC0\xCC\xC5\xDB\xC0\xCC \xC6\xF7\xC7\xD4\xB5\xC7\xBE\xEE \xC0\xD6\xBE\xEE \xC1\xF6\xBF\xEC\xC1\xF6 \xB8\xF8\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			// realmyroommainui.cpp:13578
			else
				// realmyroommainui.cpp:13580
				pForm->SetMessage("\xBF\xEC\xC6\xED\xB9\xB0\xC0\xBB \xC1\xF6\xBF\xEC\xC1\xF6 \xB8\xF8\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			// realmyroommainui.cpp:13583
			break;

		case 0x13F: {
			// realmyroommainui.cpp:13620, a post could not be sent: param2 why
			int error = msg.m_param2;
			// realmyroommainui.cpp:13621
			if (error == 1)
				// realmyroommainui.cpp:13622
				pForm->SetMessage("\xBF\xEC\xC6\xED\xB9\xB0\xC0\xBB \xBA\xB8\xB3\xBE \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9. \n\xBB\xF3\xB4\xEB\xB9\xE6\xC0\xCC \xC6\xED\xC1\xF6 \xBC\xF6\xBD\xC5\xC0\xBB \xB0\xC5\xBA\xCE\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			// realmyroommainui.cpp:13623
			else if (error == 2)
				// realmyroommainui.cpp:13624
				pForm->SetMessage("\xBF\xEC\xC6\xED\xB9\xB0\xC0\xBB \xBA\xB8\xB3\xBE \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9. \n\xBF\xEC\xC6\xED\xB9\xB0\xC0\xBB \xBA\xB8\xB3\xBE \xC0\xAF\xC0\xFA\xB8\xA6 \xC3\xA3\xC0\xBB \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			// realmyroommainui.cpp:13625
			else if (error == 3)
				// realmyroommainui.cpp:13626
				pForm->SetMessage("\xBF\xEC\xC6\xED\xB9\xB0\xC0\xBB \xBA\xB8\xB3\xBE \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9. \n\xBB\xF3\xB4\xEB\xB9\xE6\xC0\xCC \xC0\xCC\xB9\xCC \xBC\xD2\xC0\xAF\xC7\xD1 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xD4\xB4\xCF\xB4\xD9.", false);
			// realmyroommainui.cpp:13627
			else if (error == 4)
				// realmyroommainui.cpp:13628
				pForm->SetMessage("\xBF\xEC\xC6\xED\xB9\xB0\xC0\xBB \xBA\xB8\xB3\xBE \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9. \n\xBC\xD2\xC0\xAF\xC7\xD1 \xBE\xC6\xC0\xCC\xC5\xDB \xBC\xF6\xB7\xAE\xBA\xB8\xB4\xD9 \xB4\xF5 \xB8\xB9\xC0\xBA \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xBA\xB8\xB3\xBB\xB7\xC1\xB0\xED \xBD\xC3\xB5\xB5\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9..", false);
			// realmyroommainui.cpp:13629
			else if (error == 5)
				// realmyroommainui.cpp:13630
				pForm->SetMessage("\xBF\xEC\xC6\xED\xB9\xB0\xC0\xBB \xBA\xB8\xB3\xBE \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9. \n\xBC\xD2\xC0\xAF\xC7\xCF\xC1\xF6 \xBE\xCA\xC0\xBA \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xBA\xB8\xB3\xBB\xB7\xC1\xB0\xED \xBD\xC3\xB5\xB5\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			// realmyroommainui.cpp:13631
			else if (error == 6)
				// realmyroommainui.cpp:13632
				pForm->SetMessage("\xBF\xEC\xC6\xED\xB9\xB0\xC0\xBB \xBA\xB8\xB3\xBE \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9. \n\xBB\xF3\xB4\xEB\xB9\xE6\xC0\xCC \xC0\xCC\xB9\xCC \xC6\xED\xC1\xF6\xB7\xCE \xB9\xDE\xC0\xBA \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xD4\xB4\xCF\xB4\xD9.", false);
			// realmyroommainui.cpp:13633
			else if (error == 7)
				// realmyroommainui.cpp:13634
				pForm->SetMessage("\xBF\xEC\xC6\xED\xB9\xB0\xC0\xBB \xBA\xB8\xB3\xBE \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9. \n\xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xC3\xB7\xBA\xCE\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			// realmyroommainui.cpp:13635
			else if (error == 8)
				// realmyroommainui.cpp:13636
				pForm->SetMessage("\xBF\xEC\xC6\xED\xB9\xB0\xC0\xBB \xBA\xB8\xB3\xBE \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9. \n\xC6\xED\xC1\xF6 \xB9\xDF\xBC\xDB\xC0\xBB \xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			// realmyroommainui.cpp:13637
			else if (error == 9)
				// realmyroommainui.cpp:13638
				pForm->SetMessage("\xBF\xEC\xC6\xED\xB9\xB0\xC0\xBB \xBA\xB8\xB3\xBE \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9. \n\xBB\xF3\xB4\xEB\xB9\xE6\xC0\xC7 \xBF\xEC\xC6\xED\xC7\xD4\xC0\xCC \xB0\xA1\xB5\xE6 \xC2\xF7\xBC\xAD \xC7\xF6\xC0\xE7\xB4\xC2 \xBF\xEC\xC6\xED\xB9\xB0\xC0\xBB \xB9\xDE\xC0\xBB \xBC\xF6 \xBE\xF8\xB4\xC2 \xBB\xF3\xC5\xC2\xC0\xD4\xB4\xCF\xB4\xD9.", false);
			// realmyroommainui.cpp:13639
			else if (error == 10)
				// realmyroommainui.cpp:13650
				pForm->SetMessage("\xBF\xEC\xC6\xED\xC7\xD4 \xB1\xE2\xB4\xC9\xC0\xC7 \xBA\xCE\xBA\xD0 \xC1\xA1\xB0\xCB\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9.", false);
			// realmyroommainui.cpp:13652
			else if (error == 0x16)
				// realmyroommainui.cpp:13653
				pForm->SetMessage("\xB8\xDE\xC0\xCF \xC3\xB7\xBA\xCE \xB0\xA1\xB4\xC9\xC7\xD1 \xC3\xD6\xB4\xEB \xB0\xB3\xBC\xF6\xB8\xA6 \xC3\xCA\xB0\xFA\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			// realmyroommainui.cpp:13654
			else
				// realmyroommainui.cpp:13655
				pForm->SetMessage("\xBF\xEC\xC6\xED\xB9\xB0\xC0\xBB \xBA\xB8\xB3\xBB\xC1\xF6 \xB8\xF8\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			break;
		}
		}

		// realmyroommainui.cpp:13661
		if (request != 0x124)
			pForm->Open(NULL, 0);
		// realmyroommainui.cpp:13663
		AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		// realmyroommainui.cpp:13673
		break;
	}

	case 0x48:
		// realmyroommainui.cpp:13677, the friend list changed
		if (IsLocalContent((localContentType_t)0x56)) {
			// realmyroommainui.cpp:13680
			if (m_pPostSendDlg)
				// realmyroommainui.cpp:13681
				m_pPostSendDlg->UpdateFriendList();
		}
		// realmyroommainui.cpp:13684
		break;

	case 0x50: {
		// realmyroommainui.cpp:13689, a user's info arrived: param1 the
		// receiver's uid, param2 his user info
		if (IsLocalContent((localContentType_t)0x56)) {
			// realmyroommainui.cpp:13691
			if (m_pPostSendDlg)
				// realmyroommainui.cpp:13694
				m_pPostSendDlg->SetReceiverInfo((unsigned long)msg.m_param1, *(const sPangYaUserInfo *)msg.m_param2);
		}

		// realmyroommainui.cpp:13699
		FrUserInfoForm *pUserInfo = USERINFODLG();
		// realmyroommainui.cpp:13700
		if (pUserInfo) {
			// realmyroommainui.cpp:13702
			if (CUserInfo::Instance()->IsVisible() == true) {
				// realmyroommainui.cpp:13704
				if (pUserInfo->m_addFriendDlg) {
					// realmyroommainui.cpp:13706
					const sPangYaUserInfo *info = (const sPangYaUserInfo *)msg.m_param2;
					// realmyroommainui.cpp:13707
					pUserInfo->SetRequestFriendData(info->nickname, info->uid);
				}
			}
		}
		// realmyroommainui.cpp:13712
		break;
	}

	case 0x205:
		// realmyroommainui.cpp:13716, the tiki report result: param1 its
		// _SYSTEMTIME
		if (IsLocalContent((localContentType_t)0x14)) {
			// realmyroommainui.cpp:13718
			if (m_pReportDlg == NULL) {
				// realmyroommainui.cpp:13720
				SYSTEMTIME time = *(const SYSTEMTIME *)msg.m_param1;
				// realmyroommainui.cpp:13721
				m_pReportDlg = CreateForm<FrUniteResultDlg>(g_pFresh->GetManager(), this, "uniteresult", NULL);
				// realmyroommainui.cpp:13722
				m_pReportDlg->EnableDrag(false);
				// realmyroommainui.cpp:13723
				m_pReportDlg->SetTikiReportState(time);
				// realmyroommainui.cpp:13724
				m_pReportDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnResultDlg_Report, WPoint(68.0f, 48.0f), 1);
			}
		}
		// realmyroommainui.cpp:13728
		break;

	case 0x22F:
		// realmyroommainui.cpp:13732
		if (!g_pFresh->IsCurrentLayout("REALMYROOM"))
			break;
		// realmyroommainui.cpp:13734
		SetListMenu();
		// fall through: the display stand is rebuilt too

	case 0xDD:
		// realmyroommainui.cpp:13735
		BuildDisplayStand((subTab_t)m_roomMode);
		// realmyroommainui.cpp:13738
		break;

	case 0x233:
		// realmyroommainui.cpp:13744
		if (IsLocalContent((localContentType_t)0x71) == true)
			// realmyroommainui.cpp:13747
			SetItemStorageVisible(true);
		// realmyroommainui.cpp:13750
		break;

	case 0x234:
		// realmyroommainui.cpp:13754
		if (IsLocalContent((localContentType_t)0x71) == true) {
			// realmyroommainui.cpp:13757
			SetItemStorageVisible(false);
			// realmyroommainui.cpp:13760
			if (m_pItemStorageAccessPanel)
				// realmyroommainui.cpp:13762
				m_pItemStorageAccessPanel->Close((eFormRet)2, true);
		}
		// realmyroommainui.cpp:13766
		break;

	case 0x235:
		// realmyroommainui.cpp:13770
		if (IsLocalContent((localContentType_t)0x71) == true)
			// realmyroommainui.cpp:13772
			OpenItemStorageAccessPanel(1);
		// realmyroommainui.cpp:13775
		break;

	case 0x236:
		// realmyroommainui.cpp:13779
		if (IsLocalContent((localContentType_t)0x71) == true)
			// realmyroommainui.cpp:13781
			OpenItemStorageNotice();
		// realmyroommainui.cpp:13784
		break;

	case 0x237:
		// realmyroommainui.cpp:13788
		if (IsLocalContent((localContentType_t)0x71) == true)
			// realmyroommainui.cpp:13790
			OpenItemStorageAccessPanel(2);
		// realmyroommainui.cpp:13793
		break;

	case 0x238:
		// realmyroommainui.cpp:13797
		if (IsLocalContent((localContentType_t)0x71) == true)
			// realmyroommainui.cpp:13799
			OpenItemStorageAccessPanel(3);
		// realmyroommainui.cpp:13802
		break;

	case 0x239:
		// realmyroommainui.cpp:13806
		if (IsLocalContent((localContentType_t)0x71) == true)
			// realmyroommainui.cpp:13808
			OpenItemStorageAccessPanel(4);
		// realmyroommainui.cpp:13811
		break;

	case 0x23A:
		// realmyroommainui.cpp:13815
		if (IsLocalContent((localContentType_t)0x71) == true) {
			// realmyroommainui.cpp:13817
			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x233, 0, 0, 0, 0, 0);
			// realmyroommainui.cpp:13819
			if (m_itemStorageVisible == true)
				// realmyroommainui.cpp:13821
				BuildDisplayStand((subTab_t)m_roomMode);
		}
		// realmyroommainui.cpp:13825
		break;

	case 0x23D:
		// realmyroommainui.cpp:13829, ask for the item storage's current page
		if (IsLocalContent((localContentType_t)0x71) == true) {
			// realmyroommainui.cpp:13831
			if (m_itemStorageVisible == true) {
				// realmyroommainui.cpp:13833
				CItemStorage *pStorage = CItemStorage::m_pInstance;
				// realmyroommainui.cpp:13835
				if (pStorage) {
					// realmyroommainui.cpp:13837
					int page = pStorage->m_curPage;
					// realmyroommainui.cpp:13838
					WSendPacket packet((enumClientPacket)0xCF);
					// realmyroommainui.cpp:13839
					packet.Encode4(0x63);
					// realmyroommainui.cpp:13840
					packet.Encode2((unsigned char)page);
					// realmyroommainui.cpp:13841
					packet.Send(SEND_TO_DEFAULT);
				}
			}
		}
		// realmyroommainui.cpp:13846
		break;

	case 0x23E:
		// realmyroommainui.cpp:13850, the same, at least page 1
		if (IsLocalContent((localContentType_t)0x71) == true) {
			// realmyroommainui.cpp:13852
			if (m_itemStorageVisible == true) {
				// realmyroommainui.cpp:13854
				CItemStorage *pStorage = CItemStorage::m_pInstance;
				// realmyroommainui.cpp:13856
				if (pStorage) {
					// realmyroommainui.cpp:13858
					int page = pStorage->m_curPage;
					// realmyroommainui.cpp:13860
					if (page == 0)
						// realmyroommainui.cpp:13862
						page = 1;
					// realmyroommainui.cpp:13865
					WSendPacket packet((enumClientPacket)0xCF);
					// realmyroommainui.cpp:13866
					packet.Encode4(0x63);
					// realmyroommainui.cpp:13867
					packet.Encode2((unsigned char)page);
					// realmyroommainui.cpp:13868
					packet.Send(SEND_TO_DEFAULT);
				}
			}
		}
		// realmyroommainui.cpp:13873
		break;

	case 0x23F: {
		// realmyroommainui.cpp:13880, an item storage request answered:
		// param1 the request, param2 the error code
		int error = msg.m_param2;
		bool refresh = false;
		// realmyroommainui.cpp:13881
		if (error == 0) {
			// realmyroommainui.cpp:13883
			if (msg.m_param1 == 0x17B)
				// realmyroommainui.cpp:13884
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xC6\xD0\xBD\xBA\xBF\xF6\xB5\xE5\xB0\xA1 \xBC\xB3\xC1\xA4\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
		// realmyroommainui.cpp:13886
		} else if (error == 0x64) {
			// realmyroommainui.cpp:13888
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xBD\xC7\xC6\xD0", 0, 0, 0, 0));
		// realmyroommainui.cpp:13890
		} else if (error == 0x65) {
			// realmyroommainui.cpp:13892
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xBD\xC7\xC6\xD0\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
		// realmyroommainui.cpp:13894
		} else if (error == 0x66) {
			// realmyroommainui.cpp:13896
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xC1\xA1\xB0\xCB\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
		// realmyroommainui.cpp:13898
		} else if (error == 0x67) {
		// realmyroommainui.cpp:13900
		} else if (error == 0x68) {
		// realmyroommainui.cpp:13902
		} else if (error == 0x6A) {
		// realmyroommainui.cpp:13904
		} else if (error == 0x6B) {
			// realmyroommainui.cpp:13907
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xC1\xA4\xBB\xF3 \xC3\xB3\xB8\xAE \xB5\xC7\xC1\xF6 \xBE\xCA\xBE\xD2\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
			refresh = true;
		// realmyroommainui.cpp:13909
		} else if (error == 0x6C) {
			// realmyroommainui.cpp:13911
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xC3\xB3\xB8\xAE\xC7\xD2\xBC\xF6 \xC0\xD6\xB4\xC2 \xBE\xC6\xC0\xCC\xC5\xDB \xC3\xD6\xB4\xEB \xB0\xB3\xBC\xF6\xB8\xA6 \xC3\xCA\xB0\xFA\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
		// realmyroommainui.cpp:13913
		} else if (error == 0x6D) {
			// realmyroommainui.cpp:13915
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xC3\xA2\xB0\xED\xBF\xA1 \xBA\xB8\xB0\xFC\xC7\xD2 \xBC\xF6 \xBE\xF8\xB4\xC2 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xD4\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
		// realmyroommainui.cpp:13917
		} else if (error == 0x6E) {
			// realmyroommainui.cpp:13919
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xB1\xE2\xB0\xA3\xC1\xA6 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBA \xC3\xA2\xB0\xED\xBF\xA1 \xB3\xD6\xC0\xBB \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
		// realmyroommainui.cpp:13921
		} else if (error == 0x6F) {
			// realmyroommainui.cpp:13923
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xC0\xD4\xB7\xC2\xC7\xCF\xBD\xC5 \xB1\xDD\xBE\xD7\xC0\xCC \xBA\xB8\xC0\xAF\xC7\xD1 \xB1\xDD\xBE\xD7\xBA\xB8\xB4\xD9 \xB8\xB9\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
		// realmyroommainui.cpp:13925
		} else if (error == 0x70) {
			// realmyroommainui.cpp:13928
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xBE\xC6\xC0\xCC\xC5\xDB\xC0\xCC \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
			refresh = true;
		// realmyroommainui.cpp:13930
		} else if (error == 0x71) {
			// realmyroommainui.cpp:13932
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xC3\xA2\xB0\xED\xB0\xA1 \xC0\xCC\xB9\xCC \xBA\xB8\xC0\xAF\xC7\xCF\xB0\xED \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
		// realmyroommainui.cpp:13934
		} else if (error == 0x72) {
			// realmyroommainui.cpp:13936
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xBA\xB8\xC0\xAF\xC1\xDF\xC0\xCE \xC3\xA2\xB0\xED\xB0\xA1 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
		// realmyroommainui.cpp:13938
		} else if (error == 0x73) {
			// realmyroommainui.cpp:13940
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xC6\xED\xC1\xFD\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
		// realmyroommainui.cpp:13942
		} else if (error == 0x74) {
			// realmyroommainui.cpp:13945
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xC3\xA2\xB0\xED\xB0\xA1 \xC0\xE1\xB1\xDD\xBB\xF3\xC5\xC2\xC0\xD4\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
			refresh = true;
		// realmyroommainui.cpp:13947
		} else if (error == 0x75) {
			// realmyroommainui.cpp:13949
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xC6\xD0\xBD\xBA\xBF\xF6\xB5\xE5\xB0\xA1 \xC6\xB2\xB7\xC8\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
		// realmyroommainui.cpp:13951
		} else if (error == 0x69) {
			// realmyroommainui.cpp:13953
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xC6\xD0\xBD\xBA\xBF\xF6\xB5\xE5\xB4\xC2 \xBC\xFD\xC0\xDA 4\xC0\xDA\xB8\xAE\xB7\xCE\xB8\xB8 \xBC\xB3\xC1\xA4 \xB0\xA1\xB4\xC9 \xC7\xD5\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
		// realmyroommainui.cpp:13955
		} else if (error == 0x76) {
			// realmyroommainui.cpp:13957
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xBE\xC6\xC0\xCC\xC5\xDB \xBC\xF6\xB7\xAE \xBF\xA1\xB7\xAF", 0, 0, 0, 0));
		// realmyroommainui.cpp:13959
		} else if (error == 0x77) {
			// realmyroommainui.cpp:13963
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xC0\xD4\xB7\xC2\xC7\xCF\xBD\xC5 \xB1\xDD\xBE\xD7\xC0\xCC \xBA\xB8\xC0\xAF\xC7\xD1 \xB1\xDD\xBE\xD7\xBA\xB8\xB4\xD9 \xB8\xB9\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
		} else if (error == 0x78) {
			// realmyroommainui.cpp:13965
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"1\xC8\xB8 \xC0\xD4,\xC3\xE2\xB1\xDD \xC7\xD2\xBC\xF6\xC0\xD6\xB4\xC2 \xB9\xFC\xC0\xA7\xB8\xA6 \xC3\xCA\xB0\xFA\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
		// realmyroommainui.cpp:13967
		} else if (error == 0x79) {
			// realmyroommainui.cpp:13969
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xBF\xAC\xBC\xD3\xB5\xC8 \xBC\xFD\xC0\xDA\xB7\xCE \xBC\xB3\xC1\xA4\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
		// realmyroommainui.cpp:13971
		} else if (error == 0x7A) {
			// realmyroommainui.cpp:13973
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)" \xB8\xB6\xC0\xCC\xB7\xEB \xC3\xD6\xB4\xEB \xBA\xB8\xC0\xAF \xBC\xF6\xB7\xAE\xC0\xBB \xC3\xCA\xB0\xFA\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
		}

		// realmyroommainui.cpp:13976
		if (refresh)
			// realmyroommainui.cpp:13978
			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x234, 0, 0, 0, 0, 0);
		// realmyroommainui.cpp:13981
		break;
	}

	case 0x23C:
		// realmyroommainui.cpp:13985, the item storage list: back to its
		// first index page
		if (IsLocalContent((localContentType_t)0x71) == true) {
			// realmyroommainui.cpp:13987
			CItemStorage *pStorage = CItemStorage::m_pInstance;
			// realmyroommainui.cpp:13989
			if (pStorage && m_pItemStorageList) {
				// realmyroommainui.cpp:13992
				pStorage->InitIndexControl(pStorage->m_totalPage, 5);
				// realmyroommainui.cpp:13994
				CIndexControl *pIndex = pStorage->m_indexControl;
				// realmyroommainui.cpp:13996
				if (pIndex)
					// realmyroommainui.cpp:13998
					pIndex->SelectIndex(0);
			}
		}
		// realmyroommainui.cpp:14003
		break;

	case 0x23B:
		// realmyroommainui.cpp:14007, the same, to its last index page
		if (IsLocalContent((localContentType_t)0x71) == true) {
			// realmyroommainui.cpp:14009
			CItemStorage *pStorage = CItemStorage::m_pInstance;
			// realmyroommainui.cpp:14011
			if (pStorage && m_pItemStorageList) {
				// realmyroommainui.cpp:14014
				pStorage->InitIndexControl(pStorage->m_totalPage, 5);
				// realmyroommainui.cpp:14016
				CIndexControl *pIndex = pStorage->m_indexControl;
				// realmyroommainui.cpp:14018
				if (pIndex) {
					// realmyroommainui.cpp:14020
					unsigned long count = pIndex->GetCurIndexCount();
					// realmyroommainui.cpp:14021
					pIndex->SelectIndex(count);
				}
			}
		}
		// realmyroommainui.cpp:14026
		break;

	case 0x242:
		// realmyroommainui.cpp:14030, param2 receives whether the item
		// (param1) is in the item list
		if (IsInItemList(msg.m_param1)) {
			// realmyroommainui.cpp:14031
			*(bool *)msg.m_param2 = true;
			// realmyroommainui.cpp:14032
			break;
		}
		// realmyroommainui.cpp:14033
		*(bool *)msg.m_param2 = false;
		// realmyroommainui.cpp:14035
		break;

	case 0x244:
		// realmyroommainui.cpp:14040
		if (m_pPostDlg)
			// realmyroommainui.cpp:14041
			m_pPostDlg->EnableMiniBtns(msg.m_param1 != 0);
		// realmyroommainui.cpp:14043
		break;

	case 0x245: {
		// realmyroommainui.cpp:14049, an item was used: param1 its type id,
		// param2 how many are left
		if (!IsLocalContent((localContentType_t)0x76)) {
			// realmyroommainui.cpp:14051
			FrForm *pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
			// realmyroommainui.cpp:14052
			pNotify->SetMessage("\xB3\xAD\xB7\xCE \xB1\xE2\xB4\xC9\xC0\xBA \xC7\xF6\xC0\xE7 \xBB\xE7\xBF\xEB\xC7\xCF\xBD\xC7\xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9. ", false);
			// realmyroommainui.cpp:14053
			pNotify->Open(NULL, 3);
			// realmyroommainui.cpp:14054
			break;
		}

		// realmyroommainui.cpp:14058
		unsigned long typeId = msg.m_param1;
		int count = msg.m_param2;
		// realmyroommainui.cpp:14060
		IFF_ITEM_COMMON *pItem = ItemManager()->FindCommonItem(typeId);
		// realmyroommainui.cpp:14061
		FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_item", NULL);
		// realmyroommainui.cpp:14063
		if (pForm == NULL)
			break;

		// realmyroommainui.cpp:14065
		FrArea *pIcon = DYNAMIC_CAST(FrArea, pForm->FindChildByName("itemicon"));
		// realmyroommainui.cpp:14066
		FrEdit *pName = DYNAMIC_CAST(FrEdit, pForm->FindChildByName("itemname"));

		// realmyroommainui.cpp:14068
		if (pIcon) {
			// realmyroommainui.cpp:14070
			if (pItem) {
				// realmyroommainui.cpp:14072
				pIcon->SetBgImg(pItem->m_icon);
			// realmyroommainui.cpp:14074
			} else if (typeId) {
				// realmyroommainui.cpp:14076
				WRect rect(pIcon->GetRect());
				// realmyroommainui.cpp:14077
				pIcon->SetRect(WRect(rect.x + 15.0f, rect.y + 7.0f, rect.w, rect.h));
				// realmyroommainui.cpp:14078
				pIcon->SetBgImg("item1_16");
			// realmyroommainui.cpp:14080
			} else {
				// realmyroommainui.cpp:14082
				pIcon->SetBgImg("hide");
			}
		}

		// realmyroommainui.cpp:14086
		if (pName) {
			// realmyroommainui.cpp:14088
			if (pItem)
				// realmyroommainui.cpp:14090
				pName->AddText(pItem->m_name, false, true);
			// realmyroommainui.cpp:14092
			else if (typeId)
				// realmyroommainui.cpp:14094
				pName->AddText(MakeStr("%d\xC6\xCE", typeId), false, true);
			// realmyroommainui.cpp:14096
			else
				// realmyroommainui.cpp:14098
				pName->AddText("none", false, true);
			// realmyroommainui.cpp:14101
			pName->AddText("\n", false, true);
		}

		// realmyroommainui.cpp:14104
		pForm->SetMessage(MakeStr("\xB0\xA1\xB1\xB8\xB8\xA6 \xBB\xE7\xBF\xEB\xC7\xCF\xBF\xA9 %s \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB %d\xB0\xB3 \xC8\xB9\xB5\xE6\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", pItem->m_name, count), false);
		// realmyroommainui.cpp:14105
		pForm->Open(NULL, 2);
		// realmyroommainui.cpp:14108
		break;
	}

	case 0x12D: {
		// realmyroommainui.cpp:14113, the level-up item form: param1 the
		// sLevelUpDone
		FrLevelupItemForm *form = CreateForm<FrLevelupItemForm>(g_pFresh->GetManager(), this, "levelupitem", NULL);
		// realmyroommainui.cpp:14115
		if (form == NULL)
			break;
		// realmyroommainui.cpp:14118
		form->SetData(*(const sLevelUpDone *)msg.m_param1);
		// realmyroommainui.cpp:14119
		form->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnLevelupFormResult, 3);
		m_levelupForms.push_back(form);
		break;
	}

	case 0x266:
		// realmyroommainui.cpp:14129
		UnselectItemList((unsigned long)msg.m_param1);
		// realmyroommainui.cpp:14131
		break;

	case 0x267:
		// realmyroommainui.cpp:14135
		SelectItemList((unsigned long)msg.m_param1);
		// realmyroommainui.cpp:14137
		break;

	default:
		// realmyroommainui.cpp:14140
		HandleMsgCommon(msg);
		break;
	}
	// realmyroommainui.cpp:14144
}

// realmyroommainui.cpp:14148
bool CRealMyRoomMain::OnReplayModeOpenDlgResult(int result, FrForm* pForm) {
	m_pReplayDelDlg = NULL;

	if (result == 1) {
		memset(&WSingleton<CSharedDoc>::Instance()->m_roomInfo, 0, sizeof(sRoomInfo));
		WSingleton<CSharedDoc>::Instance()->m_playingMode = (eReplayModeType)2;
		WSingleton<CSharedDoc>::Instance()->m_roomType = 0; strcpy(WSingleton<CSharedDoc>::Instance()->m_roomName, WSingleton<CSharedDoc>::Instance()->m_gameTypeNames[0].name);
		CTaskManager::Instance()->ChangeTask("CGolfTask", "CGolfDoc", false);
	}

	return true;
}

// realmyroommainui.cpp:14164
bool CRealMyRoomMain::OnReplayDelOpenDlgResult(int result, FrForm* pForm) {
	m_pReplayDelDlg = NULL;

	if (result == 1) {
		BuildDisplayStand(static_cast<subTab_t>(0x17));
	}

	return true;
}

// realmyroommainui.cpp:14179
void CRealMyRoomMain::OnFurniturePupInit(PG_MSGARG param) {
	m_pFurniturePageUpButton = DYNAMIC_CAST(FrButton, param);
	if (m_pFurniturePageUpButton) {
		m_pFurniturePageUpButton->SetVisible(false);
	}
}

// realmyroommainui.cpp:14188
void CRealMyRoomMain::OnFurniturePupButtonUp(void) {
	m_furniturePage--;

	int lastPage = m_furniturePageCount - 1;
	m_furniturePage = m_furniturePage < 0 ? 0 : (m_furniturePage > lastPage ? lastPage : m_furniturePage);

	m_pFurnitureListNumWnd->ClearLine();
	m_pFurniturePageCountWnd->ClearLine();
	m_pFurnitureListNumWnd->AddText(MakeStr("%3d", m_furniturePage + 1), false, true);
	m_pFurniturePageCountWnd->AddText(MakeStr("%3d", m_furniturePageCount), false, true);

	BuildDisplayStandFurniture();
}

// realmyroommainui.cpp:14205
void CRealMyRoomMain::OnFurniturePdownInit(PG_MSGARG param) {
	m_pFurniturePageDownButton = DYNAMIC_CAST(FrButton, param);
	if (m_pFurniturePageDownButton) {
		m_pFurniturePageDownButton->SetVisible(false);
	}
}

// realmyroommainui.cpp:14214
void CRealMyRoomMain::OnFurniturePdownButtonUp(void) {
	m_furniturePage++;

	int lastPage = m_furniturePageCount - 1;
	m_furniturePage = m_furniturePage < 0 ? 0 : (m_furniturePage > lastPage ? lastPage : m_furniturePage);

	m_pFurnitureListNumWnd->ClearLine();
	m_pFurniturePageCountWnd->ClearLine();
	m_pFurnitureListNumWnd->AddText(MakeStr("%3d", m_furniturePage + 1), false, true);
	m_pFurniturePageCountWnd->AddText(MakeStr("%3d", m_furniturePageCount), false, true);

	BuildDisplayStandFurniture();
}

// realmyroommainui.cpp:14230
void CRealMyRoomMain::OnFurnitureListNumInit(PG_MSGARG param) {
	m_pFurnitureListNumWnd = DYNAMIC_CAST(FrEdit, param);
	if (m_pFurnitureListNumWnd) {
		m_pFurnitureListNumWnd->SetCharLimit(2, false);
		m_pFurnitureListNumWnd->SetVisible(false);
	}
}

// realmyroommainui.cpp:14243
void CRealMyRoomMain::OnFurnitureListNumBtnUp(void) {
	if (m_pFurnitureListNumWnd->IsVisible())
	{
		m_pFurnitureListNumWnd->SetKeyEvent(true);
	}
}

// realmyroommainui.cpp:14252
bool CRealMyRoomMain::OnFurnitureListEnterKey(PG_MSGARG param) {
	OnFurniture_BtnListNumBtnUp();

	return false;
}

// realmyroommainui.cpp:14259
void CRealMyRoomMain::OnFurnitureListMaxNumInit(PG_MSGARG param) {
	m_pFurniturePageCountWnd = DYNAMIC_CAST(FrEdit, param);
	if (m_pFurniturePageCountWnd) {
		m_pFurniturePageCountWnd->SetVisible(false);
	}
}

// realmyroommainui.cpp:14268
void CRealMyRoomMain::OnFurniture_BtnListNumInit(PG_MSGARG param) {
	m_pFurnitureListNumButton = DYNAMIC_CAST(FrButton, param);
	if (m_pFurnitureListNumButton) {
		m_pFurnitureListNumButton->SetVisible(false);
	}
}

// realmyroommainui.cpp:14278
void CRealMyRoomMain::OnFurniture_BtnListNumBtnUp(void) {
	m_furniturePage = atoi(m_pFurnitureListNumWnd->GetLine(1, false)) - 1;
	m_furniturePage = Between(0, m_furniturePage, m_furniturePageCount - 1);

	m_pFurnitureListNumWnd->ClearLine();
	m_pFurniturePageCountWnd->ClearLine();
	m_pFurnitureListNumWnd->AddText(MakeStr("%3d", m_furniturePage + 1), false, true);
	m_pFurniturePageCountWnd->AddText(MakeStr("%3d", m_furniturePageCount), false, true);

	if (m_pFurnitureListNumWnd->IsVisible()) {
		m_pFurnitureListNumWnd->SetKeyEvent(true);
	}

	BuildDisplayStandFurniture();
}

// realmyroommainui.cpp:14319
void CRealMyRoomMain::OnFurnitureResetInit(PG_MSGARG param) {
	m_pFurnitureResetButton = DYNAMIC_CAST(FrButton, param);
	if (m_pFurnitureResetButton) {
		m_pFurnitureResetButton->SetVisible(false);
	}
}

// realmyroommainui.cpp:14328
void CRealMyRoomMain::OnFurnitureResetButtonUp(void) {
	WSingleton<CRealMyRoomData>::Instance()->ResetList(false);

	for (std::list<sFurniture_List>::iterator it = WSingleton<CSharedDoc>::Instance()->m_furnitureList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_furnitureList.end(); ++it) {
		it->m_flags &= ~1;
	}

	BuildDisplayStandFurniture();
}

// realmyroommainui.cpp:14340
void CRealMyRoomMain::OnFurnitureBackInit(PG_MSGARG param) {
	m_pFurnitureBackButton = DYNAMIC_CAST(FrButton, param);
	if (m_pFurnitureBackButton) {
		m_pFurnitureBackButton->SetVisible(false);
	}
}

// realmyroommainui.cpp:14349
void CRealMyRoomMain::OnFurnitureBackButtonUp(void) {
	std::list<sFurniture_List> furnitureList;

	for (std::list<sFurniture_List>::iterator it = WSingleton<CSharedDoc>::Instance()->m_furnitureList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_furnitureList.end(); ++it) {
		it->m_flags &= ~1;
	}

	WSingleton<CRealMyRoomData>::Instance()->ResotreList();

	BuildDisplayStandFurniture();
}

// realmyroommainui.cpp:14364
void CRealMyRoomMain::OnFurnitureSaveInit(PG_MSGARG param) {
	m_pFurnitureSaveButton = DYNAMIC_CAST(FrButton, param);
	if (m_pFurnitureSaveButton) {
		m_pFurnitureSaveButton->SetVisible(false);
	}
}

// realmyroommainui.cpp:14373
void CRealMyRoomMain::OnFurnitureSaveButtonUp(void) {
	if (MyUID() == WSingleton<CSharedDoc>::Instance()->m_myRoomOwnerUID) {
		m_pNamedCameras[1]->m_matrix = g_camera;
		m_pNamedCameras[1]->m_fov = g_view->GetFOV();
		m_savedClipNear = g_view->GetClipNearValue();
		m_savedClipFar = g_view->GetClipFarValue();
		SetCameraDisplayType(2);

		if (WSingleton<CRealMyRoomData>::Instance()->SaveModel(false)) {
			if (MyUID() == WSingleton<CSharedDoc>::Instance()->m_myRoomOwnerUID) {
				RMR_UpdateDB();
				WSingleton<CRealMyRoomData>::Instance()->SaveModel(true);
			}
		}
	} else {
		FrForm* pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "It cannot be saved right now."
		pNotify->SetMessage(MakeStr("\xC7\xF6\xC0\xE7\xB4\xC2 \xC0\xFA\xC0\xE5\xC7\xD2\xBC\xF6 \xBE\xF8\xB4\xC2 \xBB\xF3\xC5\xC2\xC0\xD4\xB4\xCF\xB4\xD9."), false);
		pNotify->Open(0, 3);
	}
}

// realmyroommainui.cpp:14405
void CRealMyRoomMain::OnFurnitureOutInit(PG_MSGARG param) {
	m_pFurnitureOutButton = DYNAMIC_CAST(FrButton, param);
	if (m_pFurnitureOutButton) {
		m_pFurnitureOutButton->SetVisible(false);
	}
}

// realmyroommainui.cpp:14414
void CRealMyRoomMain::OnFurnitureOutButtonUp(void) {
	// One source line (every instruction is marked 14415).
	m_pRoomFx->SetActive(false, true); if (m_prevUiState == 1) { if (m_mainTab == (mainTab_t)6) ControlUiHideMode(1); else ControlUiHideMode(2); } else ControlUiHideMode(2);
}

// realmyroommainui.cpp:14422
void CRealMyRoomMain::OnFurnitureBackGroundInit1(PG_MSGARG param) {
	m_pFurnitureBackGround[0] = DYNAMIC_CAST(FrArea, param);
	if (m_pFurnitureBackGround[0]) {
		m_pFurnitureBackGround[0]->SetVisible(false);
	}
}

// realmyroommainui.cpp:14430
void CRealMyRoomMain::OnFurnitureBackGroundInit2(PG_MSGARG param) {
	m_pFurnitureBackGround[1] = DYNAMIC_CAST(FrArea, param);
	if (m_pFurnitureBackGround[1]) {
		m_pFurnitureBackGround[1]->SetVisible(false);
	}
}

// realmyroommainui.cpp:14438
void CRealMyRoomMain::OnFurnitureBackGroundInit3(PG_MSGARG param) {
	m_pFurnitureBackGround[2] = DYNAMIC_CAST(FrArea, param);
	if (m_pFurnitureBackGround[2]) {
		m_pFurnitureBackGround[2]->SetVisible(false);
	}
}

// realmyroommainui.cpp:14446
void CRealMyRoomMain::OnFurnitureBackGroundInit4(PG_MSGARG param) {
	m_pFurnitureBackGround[3] = DYNAMIC_CAST(FrArea, param);
	if (m_pFurnitureBackGround[3]) {
		m_pFurnitureBackGround[3]->SetVisible(false);
	}
}

// realmyroommainui.cpp:14454
void CRealMyRoomMain::OnFurnitureBackGroundInit5(PG_MSGARG param) {
	m_pFurnitureBackGround[4] = DYNAMIC_CAST(FrArea, param);
	if (m_pFurnitureBackGround[4]) {
		m_pFurnitureBackGround[4]->SetVisible(false);
	}
}

// realmyroommainui.cpp:14462
void CRealMyRoomMain::OnFurnitureBackGroundInit6(PG_MSGARG param) {
	m_pFurnitureBackGround[5] = DYNAMIC_CAST(FrArea, param);
	if (m_pFurnitureBackGround[5]) {
		m_pFurnitureBackGround[5]->SetVisible(false);
	}
}

// realmyroommainui.cpp:14470
void CRealMyRoomMain::OnFurnitureBackGroundInit7(PG_MSGARG param) {
	m_pFurnitureBackGround[6] = DYNAMIC_CAST(FrArea, param);
	if (m_pFurnitureBackGround[6]) {
		m_pFurnitureBackGround[6]->SetVisible(false);
	}
}

// realmyroommainui.cpp:14478
void CRealMyRoomMain::OnFurnitureBackGroundInit8(PG_MSGARG param) {
	m_pFurnitureBackGround[7] = DYNAMIC_CAST(FrArea, param);
	if (m_pFurnitureBackGround[7]) {
		m_pFurnitureBackGround[7]->SetVisible(false);
	}
}

// realmyroommainui.cpp:14486
void CRealMyRoomMain::OnFurnitureScrollInit1(PG_MSGARG param) {
	m_pFurnitureScroll[0] = DYNAMIC_CAST(FrArea, param);
	if (m_pFurnitureScroll[0]) {
		m_pFurnitureScroll[0]->SetVisible(false);
	}
}

// realmyroommainui.cpp:14494
void CRealMyRoomMain::OnFurnitureScrollInit2(PG_MSGARG param) {
	m_pFurnitureScroll[1] = DYNAMIC_CAST(FrArea, param);
	if (m_pFurnitureScroll[1]) {
		m_pFurnitureScroll[1]->SetVisible(false);
	}
}

// realmyroommainui.cpp:14502
void CRealMyRoomMain::OnFurnitureScrollInit3(PG_MSGARG param) {
	m_pFurnitureScroll[2] = DYNAMIC_CAST(FrArea, param);
	if (m_pFurnitureScroll[2]) {
		m_pFurnitureScroll[2]->SetVisible(false);
	}
}

// realmyroommainui.cpp:14516
void CRealMyRoomMain::BuildDisplayStandFurniture(void) {
	int count = 0;
	int added = 0;
	int category = -1;

	if (m_pFurnitureList == NULL) {
		return;
	}

	m_pFurnitureList->ClearItem();

	if (m_furnitureMainTab == 0) {
		for (std::list<sFurniture_List>::iterator it = WSingleton<CSharedDoc>::Instance()->m_furnitureList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_furnitureList.end(); ++it) {
			if (++count > m_furniturePage * 11) {
				FrListItem* pItem = m_pFurnitureList->AddItem(&*it);
				added++;

				if (it->m_flags & 1) {
					m_pFurnitureList->SelectItem(pItem, true);
				}

				if (added >= 11) {
					break;
				}
			}
		}
	} else if (m_furnitureMainTab == 4) {
		for (std::list<sFurniture_List>::iterator it = WSingleton<CSharedDoc>::Instance()->m_furnitureList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_furnitureList.end(); ++it) {
			int kind = (it->m_typeId >> 11) & 0x7FF;

			if ((kind >= 0 && kind < 7) || kind >= 10) {
				count++;

				if (added < 11) {
					if (count > m_furniturePage * 11) {
						FrListItem* pItem = m_pFurnitureList->AddItem(&*it);
						added++;

						if (it->m_flags & 1) {
							m_pFurnitureList->SelectItem(pItem, true);
						}
					}
				}
			}
		}
	} else {
		if (m_furnitureMainTab == 1) {
			category = 7;
		} else if (m_furnitureMainTab == 2) {
			category = 9;
		} else if (m_furnitureMainTab == 3) {
			category = 8;
		}

		for (std::list<sFurniture_List>::iterator it = WSingleton<CSharedDoc>::Instance()->m_furnitureList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_furnitureList.end(); ++it) {
			if (((it->m_typeId >> 11) & 0x7FF) == category) {
				count++;

				if (added < 10) {
					if (count > m_furniturePage * 11) {
						FrListItem* pItem = m_pFurnitureList->AddItem(&*it);
						added++;

						if (it->m_flags & 1) {
							m_pFurnitureList->SelectItem(pItem, true);
						}
					}
				}
			}
		}
	}

	int pad = 11 - added % 11;
	for (int i = 0; i < pad; i++) {
		m_pFurnitureList->AddItem(NULL);
	}

	if (m_furnitureMainTab == 0) {
		m_furniturePageCount = WSingleton<CSharedDoc>::Instance()->m_furnitureList.size() / 11;
	} else {
		m_furniturePageCount = count / 11;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_furnitureList.size() % 11 != 0) {
		m_furniturePageCount += 1;

		if (m_furniturePageCount == 1) {
			m_pFurniturePageUpButton->Enable(false);
			m_pFurniturePageDownButton->Enable(false);
		} else {
			m_pFurniturePageUpButton->Enable(true);
			m_pFurniturePageDownButton->Enable(true);
		}
	} else if (m_furniturePageCount == 0) {
		m_furniturePageCount = 1;
		m_pFurniturePageUpButton->Enable(false);
		m_pFurniturePageDownButton->Enable(false);
	}

	if (m_furniturePage == 0) {
		m_pFurniturePageUpButton->Enable(false);
	}

	if (m_furniturePage + 1 == m_furniturePageCount) {
		m_pFurniturePageDownButton->Enable(false);
	}

	m_pFurnitureListNumWnd->ClearLine();
	m_pFurnitureListNumWnd->AddText(MakeStr("%3d", m_furniturePage + 1), false, true);
	m_pFurniturePageCountWnd->ClearLine();
	m_pFurniturePageCountWnd->AddText(MakeStr("%3d", m_furniturePageCount), false, true);
}

// realmyroommainui.cpp:14688
void CRealMyRoomMain::OnFurnitureTabMainInit(PG_MSGARG param) {
	m_pFurnitureMainTab = DYNAMIC_CAST(FrTabButton, param);
	if (m_pFurnitureMainTab) {
		for (int i = 0; i < 5; i++) {
			m_pFurnitureMainTab->AddTab(MakeStr("rmr_arg_tab%d", i + 1), i, NULL);
		}
		m_pFurnitureMainTab->Select(m_furnitureMainTab);
		m_pFurnitureMainTab->SetVisible(false);
	}
}

// realmyroommainui.cpp:14709
void CRealMyRoomMain::OnFurnitureTabMainButtonUp(void) {
	TButtonItem* pItem = m_pFurnitureMainTab->GetSelected();
	if (pItem == NULL) {
		return;
	}

	ChangeFMainTab(static_cast<eFurniture_Tab>(pItem->index), false);
}

// realmyroommainui.cpp:14722
void CRealMyRoomMain::ChangeFMainTab(eFurniture_Tab tab, bool bForce) {
	if (m_furnitureMainTab != tab)
		m_furniturePage = 0;

	m_furnitureMainTab = tab;

	if (tab == 2 || tab == 3 || tab == 1)
		m_pFurnitureList->SetMultiSelect(false);
	else
		m_pFurnitureList->SetMultiSelect(true);

	BuildDisplayStandFurniture();
}

// realmyroommainui.cpp:14739
void CRealMyRoomMain::OnFurnitureListInit(PG_MSGARG param) {
	m_pFurnitureList = DYNAMIC_CAST(FrListBox, param);

	if (m_pFurnitureList != NULL) {
		m_pFurnitureList->SetVisible(false);
		m_pFurnitureList->UseDummy(true);
		m_pFurnitureList->SetMultiSelect(true);
	}
}

// realmyroommainui.cpp:14752
void CRealMyRoomMain::OnFurnitureListOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	float offset = 0.0f;
	char text[512];

	if (pItem == NULL) {
		if (m_pItemNameEdit && m_pItemNameEdit->IsVisible()) {
			m_pItemNameEdit->SetElementFadeOut(true);
		}
		if (m_pItemDescEdit2 && m_pItemDescEdit2->IsVisible()) {
			m_pItemDescEdit2->SetElementFadeOut(true);
		}
		if (m_pItemDescBg2 && m_pItemDescBg2->IsVisible()) {
			m_pItemDescBg2->SetElementFadeOut(true);
		}
		return;
	}

	const Bitmap* pBg;
	if (pItem->pData == NULL) {
		pBg = m_pFurnitureEmptyBg;
	} else {
		unsigned long color;
		if (pItem->selected) {
			color = 0xFFFF0000;
		} else if (pItem->underCursor) {
			offset = 3.0f;
			color = 0xFF000000;
		} else {
			color = 0xFF000000;
		}

		pBg = m_pFurnitureItemBg;
		pGI->SetTextColor(color, 0xFFFFFFFF);
	}

	pGI->SetTextStyle(0);

	if (pBg != NULL) {
		pGI->DrawTexture(pBg, WRect(pItem->pos.x, pItem->pos.y - offset, (float)pBg->Width(), (float)pBg->Height()), 0xFFFFFFFF, 0);
	}

	sFurniture_List* pFurniture = (sFurniture_List*)pItem->pData;
	if (pFurniture == NULL) {
		return;
	}

	IFF_STRUCT::sFurniture* pIff = ItemManager()->FindFurniture(pFurniture->m_typeId);
	if (pIff != NULL) {
		const Bitmap* pIcon = g_pFresh->GetBitmap(pIff->m_icon);
		if (pIcon != NULL) {
			pGI->DrawTexture(pIcon, WRect(pItem->pos.x + 3.0f, pItem->pos.y + 3.0f - offset, (float)pIcon->Width() * 0.66f, (float)pIcon->Height() * 0.66f), 0xFFFFFFFF, 0);
		}
	}

	if (pItem->selected) {
		const Bitmap* pSel = m_pFurnitureSelFrame;
		if (pSel != NULL) {
			pGI->DrawTexture(pSel, WRect(pItem->pos.x, pItem->pos.y - offset, (float)pSel->Width(), (float)pSel->Height()), 0xFFFFFFFF, 0);
		}
	}

	if (pItem->underCursor) {
		if (m_pItemNameEdit && !m_pItemNameEdit->IsVisible()) {
			m_pItemNameEdit->SetElementFadeOut(false);
		}
		if (m_pItemDescEdit2 && !m_pItemDescEdit2->IsVisible()) {
			m_pItemDescEdit2->SetElementFadeOut(false);
		}
		if (m_pItemDescBg2 && !m_pItemDescBg2->IsVisible()) {
			m_pItemDescBg2->SetElementFadeOut(false);
		}

		IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(pFurniture->m_typeId);
		sprintf(text, " [%s]", pIff->m_name);

		m_pItemDescEdit2->ClearLine();
		m_pItemNameEdit->ClearLine();

		m_pItemNameEdit->AddText(text, false, true);

		if (pDesc != NULL) {
			m_pItemDescEdit2->AddText(pDesc->m_text, false, true);
		}

		m_itemDescEdit2InitPos = pItem->pos;
	}
}

// realmyroommainui.cpp:14870
void CRealMyRoomMain::OnFurnitureListBtnDown(void) {
	FrListItem* pCursorItem = m_pFurnitureList->GetItemUnderCursor();
	if (pCursorItem == NULL) {
		return;
	}

	sFurniture_List* pFurniture = (sFurniture_List*)pCursorItem->pData;
	if (!pCursorItem->selected && ((pFurniture->m_typeId >> 11) & 0x7FF) != 8 && ((pFurniture->m_typeId >> 11) & 0x7FF) != 9) {
		pCursorItem->selected = true;

		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);
		pForm->SetMessage("\xBC\xB1\xC5\xC3\xC7\xD1 \xB0\xA1\xB1\xB8\xC0\xC7 \xBC\xB3\xC4\xA1\xB8\xA6 \xC1\xA6\xB0\xC5\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);
		pForm->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnFurnitrueListDeleteDlgResult, 0);
		return;
	}

	for (std::list<sFurniture_List>::iterator it = WSingleton<CSharedDoc>::Instance()->m_furnitureList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_furnitureList.end(); ++it) {
		int kind = (it->m_typeId >> 11) & 0x7FF;

		if (kind == 8 || kind == 9) {
			if (((pFurniture->m_typeId >> 11) & 0x7FF) == kind && it->m_id != pFurniture->m_id) {
				it->m_flags &= ~1;
			}
		}
	}

	pFurniture->m_flags |= 1;
	int kind = (pFurniture->m_typeId >> 11) & 0x7FF;

	if (kind == 14) {
		IFF_STRUCT::sFurniture* pIff = ItemManager()->FindFurniture(pFurniture->m_typeId);
		pFurniture->m_pos.x = pIff->m_pos[0];
		pFurniture->m_pos.y = pIff->m_pos[1];
		pFurniture->m_pos.z = pIff->m_pos[2];
		pFurniture->m_angle = pIff->m_angle;
	}

	if (WSingleton<CRealMyRoomData>::Instance()->AddModel(*pFurniture)) {
		pFurniture->m_flags |= 1;
		if (kind == 14 && WSingleton<CRealMyRoomData>::Instance()->m_pSelectModel != NULL) {
			WVector screen = g_view->Projection(WSingleton<CRealMyRoomData>::Instance()->m_pSelectModel->m_pModelData->m_matrix.pivot);
			g_input->SetMousePoint(screen.x, screen.y);
		}

		if (m_furnitureMainTab == 0 && (kind == 8 || kind == 9)) {
			int count = m_pFurnitureList->GetCurrentItemSize(true);

			for (int i = 0; i < count; i++) {
				FrListItem* pItem = m_pFurnitureList->GetItem(i);
				if (pItem == NULL || pItem == pCursorItem) {
					continue;
				}

				sFurniture_List* pOther = (sFurniture_List*)pItem->pData;
				if (pOther != NULL && (int)((pOther->m_typeId >> 11) & 0x7FF) == kind && pItem->selected) {
					m_pFurnitureList->UnselectItem(pItem);
				}
			}
		}
	} else {
		pFurniture->m_flags &= ~1;
		pCursorItem->selected = false;
	}
}

// realmyroommainui.cpp:14981
void CRealMyRoomMain::OnBtnMenuInit(PG_MSGARG param) {
	m_pBtnMenu = DYNAMIC_CAST(FrButton, param);
}

// realmyroommainui.cpp:14988
void CRealMyRoomMain::OOnBtnMenuButtonUP(void) {
	m_pMenuList->SetVisible(true);
}

// realmyroommainui.cpp:14993
void CRealMyRoomMain::OnBtn_MenuListInit(PG_MSGARG param) {
	m_pMenuList = DYNAMIC_CAST(FrListBox, param);
	if (m_pMenuList) {
		m_pMenuList->SetVisible(false);

		m_pMenuList->UseDummy(true);
		m_pMenuList->SetMultiSelect(false);

		SetListMenu();
	}
}

// realmyroommainui.cpp:15007
void CRealMyRoomMain::SetListMenu(void) {
	if (m_pMenuList) {
		m_pMenuList->ClearItem();
	}

	int menu[13];
	menu[0] = 5;
	menu[1] = 6;
	menu[2] = 7;
	menu[3] = 4;
	int count = 4;
	if (!IsLocalContent(static_cast<localContentType_t>(0x66)) || WSingleton<CSharedDoc>::Instance()->IsControlServerService(0x80000)) {
		menu[4] = 3;
		count = 5;
	}

	if (WSingleton<CSharedDoc>::Instance()->IsControlServerService(0x40000)) {
		menu[count++] = 1;
	}

	if (!IsLocalContent(static_cast<localContentType_t>(0x71))) {
		menu[count++] = 11;
	}

	switch (m_uiState) {
	case 0:
	case 1:
		menu[count++] = 9;
		menu[count++] = 10;
		break;
	case 2:
		menu[count++] = 9;
		menu[count++] = 10;
	case 4:
		menu[count++] = 11;
		break;
	}

	bool found = false;
	for (std::list<sFurniture_List>::iterator it = WSingleton<CSharedDoc>::Instance()->m_furnitureList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_furnitureList.end(); ++it) {
		if ((it->m_typeId & 0x3FF800) == 0x2800 && (it->m_flags & 1)) {
			found = true;
			break;
		}
	}

	if (!found) {
		menu[count++] = 3;
	}

	if (m_pMenuList) {
		for (int i = 0; i < 13; i++) {
			if (i == 12) {
				int pad = 2 - m_pMenuList->GetCurrentItemSize(true) % 3;
				for (; pad > 0; pad--) {
					m_pMenuList->AddItem(NULL);
				}

				m_pMenuList->AddItem((void*)13);
				break;
			}

			bool listed = false;
			for (int j = 0; j < count; j++) {
				if (menu[j] == i) {
					listed = true;
					break;
				}
			}

			if (!listed) {
				m_pMenuList->AddItem((void*)(i + 1));
			}
		}
	}
}

// realmyroommainui.cpp:15105
void CRealMyRoomMain::OnBtn_MenuListOwnerDraw(PG_MSGARG param) {
	if (m_pMenuList == NULL) {
		return;
	}

	if (!m_pMenuList->IsVisible()) {
		return;
	}

	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();

	FrListItem* pItem = (FrListItem*)param;
	if (pItem == NULL || pItem->pData == NULL) {
		return;
	}

	int index = (int)pItem->pData - 1;

	const Bitmap* pIcon = m_pMenuIcon[index];

	if (pIcon != NULL) {
		pGI->DrawTexture(pIcon, WRect(pItem->pos.x, pItem->pos.y, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);
	}

	if ((pItem->selected || pItem->underCursor) && index != 12) {
		const Bitmap* pText = m_pMenuText[index];
		if (pText != NULL) {
			pGI->DrawTexture(pText, WRect(pItem->pos.x, pItem->pos.y, (float)pText->Width(), (float)pText->Height()), 0xFFFFFFFF, 0);
		}
	}

	const Bitmap* pFrame;
	if (pItem->selected && index != 12) {
		pFrame = m_pMenuDown;
	} else if (pItem->underCursor) {
		pFrame = m_pMenuOver;
	} else {
		return;
	}

	if (pFrame != NULL) {
		pGI->DrawTexture(pFrame, WRect(pItem->pos.x, pItem->pos.y, (float)pFrame->Width(), (float)pFrame->Height()), 0xFFFFFFFF, 0);
	}
}

// realmyroommainui.cpp:15160
void CRealMyRoomMain::OnBtn_MenuListBtnDown(void) {
	if (m_pMenuList == NULL) {
		return;
	}

	FrListItem* pItem = m_pMenuList->GetItemUnderCursor();
	if (pItem == NULL) {
		return;
	}

	// The item data is the menu number + 1, stored as the pointer value.
	size_t data = (size_t)pItem->pData;
	if (data == 0) {
		return;
	}

	unsigned char menu = (unsigned char)data - 1;

	SelectMenu(menu);
}

// realmyroommainui.cpp:15178
void CRealMyRoomMain::OnBtn_MenuListRBtnDown(void) {
}

// realmyroommainui.cpp:15182
void CRealMyRoomMain::OnBtn_MenuListMouseMove(void) {
}

// realmyroommainui.cpp:15186
void CRealMyRoomMain::OnBtn_MenuListDClick(void) {
}

// realmyroommainui.cpp:15194
void CRealMyRoomMain::SelectMenu(unsigned char menu) {
	switch (menu) {
	case 0:
		if (!GetFlags(1)) {
			ControlUiHideMode(1);
			if (m_cardBookVisible == true) {
				SetCardBookVisible(true);
			}
		} else {
			ControlUiHideMode(2);
			SetCardBookVisible(false);
		}
		break;
	case 1: {
		int inspecting = WSingleton<CSharedDoc>::Instance()->IsControlServerService(0x40000);

		if (inspecting) {
			// "The post office is under inspection."
			this << MsgObject(this, 0x23, (intptr_t)"\xBF\xEC\xC6\xED\xC7\xD4 \xB1\xE2\xB4\xC9\xC0\xC7 \xBA\xCE\xBA\xD0 \xC1\xA1\xB0\xCB\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
		} else if (m_pPostDlg == NULL) {
			m_pPostDlg = CreateForm<CPost_ReciveDlg>(g_pFresh->GetManager(), this, "postbox", NULL);
			FrForm::ms_bHasTail = false;
			m_pPostDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnPostDlgResult, 0x81);
			m_pPostDlg->SetFadeout(false);
		}
		break;
	}
	case 2:
		if (!GetFlags(8)) {
			ControlUiHideMode(4);
			SetCardBookVisible(false);
		} else {
			ControlUiHideMode(2);
			SetCardBookVisible(false);
		}
		m_furnitureMainTab = (eFurniture_Tab)0;
		break;
	case 8:
		if (m_pAuthorityDlg == NULL) {
			m_pAuthorityDlg = CreateForm<CRealMyRoom_AuthorityDlg>(g_pFresh->GetManager(), this, "realmyroom_authority", NULL);
			FrForm::ms_bHasTail = false;
			m_pAuthorityDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnAuthDlgResult, 0x81);
			m_pAuthorityDlg->SetFadeout(false);
		}
		break;
	case 9:
		if (m_flags[3] == true) {
			SetCameraDisplayType(5);
		} else {
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
			// "This can only be used in furniture placement mode."
			pForm->SetMessage("\xB0\xA1\xB1\xB8 \xB9\xE8\xC4\xA1 \xB8\xF0\xB5\xE5\xBF\xA1\xBC\xAD\xB8\xB8 \xBB\xE7\xBF\xEB\xC7\xD2 \xBC\xF6 \xC0\xD6\xB4\xC2 \xB1\xE2\xB4\xC9\xC0\xD4\xB4\xCF\xB4\xD9.", false);
			pForm->Open(NULL, 3);
		}
		break;
	case 11:
		if (!IsLocalContent((localContentType_t)0x71)) {
			break;
		}

		if (!IsItemStorageVisible()) {
			WSendPacket packet((enumClientPacket)0xD5);
			packet.Send(SEND_TO_DEFAULT);

			Net_EquipDresserParts();
			Net_EquipCharacter();
			Net_EquipCaddie();
			Net_EquipItem();
			Net_EquipGear();
			Net_EquipSkin();
			Net_EquipMascot();
			if (IsLocalContent((localContentType_t)0x51)) {
				WSingleton<CSharedDoc>::Instance()->SendEquipItemSlot();
			} else {
				WSingleton<CSharedDoc>::Instance()->SendEquipItemSlotByMascot();
			}
		} else {
			MsgObject msg(0, 0x234, 0, 0, 0, 0, 0);
			AfxGetTask()->GetActor("RealMyRoom") << msg;
		}
		break;
	case 10: {
		bool furnitureMode = GetFlags(8);

		if (furnitureMode == true) {
			if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar == NULL) {
				break;
			}

			if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->m_hide) {
				WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetHide(false);
				WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->m_position = m_cameraDropOrigin;
				WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetPosition(m_cameraDropOrigin);
			} else {
				WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->m_position = m_cameraDropOrigin;
				WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetPosition(m_cameraDropOrigin);
				WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetHide(true);
			}
		} else {
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
			// "This can only be used in furniture placement mode."
			pForm->SetMessage("\xB0\xA1\xB1\xB8 \xB9\xE8\xC4\xA1 \xB8\xF0\xB5\xE5\xBF\xA1\xBC\xAD\xB8\xB8 \xBB\xE7\xBF\xEB\xC7\xD2 \xBC\xF6 \xC0\xD6\xB4\xC2 \xB1\xE2\xB4\xC9\xC0\xD4\xB4\xCF\xB4\xD9.", false);
			pForm->Open(NULL, 3);
		}
		break;
	}
	case 12:
		if (m_pMenuList != NULL) {
			m_pMenuList->SetVisible(false);
		}
		break;
	}
}

// realmyroommainui.cpp:15445
void CRealMyRoomMain::SetUIState(unsigned long state) {
	if (m_uiState & state)
		m_uiState &= ~state;
	else
		m_uiState |= state;

	CRealMyRoomData::Instance()->m_pSelectModel = NULL;

	ChangeFlags(state);
}

// realmyroommainui.cpp:15458
void CRealMyRoomMain::ChangeFlags(unsigned long flag) {
	memset(m_flags, 0, sizeof(m_flags));

	switch (flag) {
	case 1:
		m_flags[0] = !m_flags[0];
		break;
	case 2:
		m_flags[1] = !m_flags[1];
		break;
	case 4:
		m_flags[2] = !m_flags[2];
		m_flags[0] = m_flags[2];
		m_flags[1] = m_flags[2];
		break;
	case 8:
		m_flags[3] = !m_flags[3];
		break;
	}
}

// realmyroommainui.cpp:15497
bool CRealMyRoomMain::GetFlags(unsigned long flag) {
	switch (flag) {
	case 1: return m_flags[0];
	case 2: return m_flags[1];
	case 4: return m_flags[2];
	case 8: return m_flags[3];
	}

	return false;
}

// realmyroommainui.cpp:15518
void CRealMyRoomMain::UIDisplayControl(bool bShow) {
	SetOverAndUnderBarVisible(bShow);

	if (bShow) {
		SetCameraDisplayType(1);
	} else {
		SetCameraDisplayType(4);
	}
}

// realmyroommainui.cpp:15527
void CRealMyRoomMain::FurnitureDisplayControl(bool bShow) {
	if (bShow)
		BuildDisplayStandFurniture();

	bool bFadeOut = !bShow;

	if (m_pFurnitureList)
		m_pFurnitureList->SetVisible(bShow);

	if (m_pFurnitureMainTab) m_pFurnitureMainTab->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureResetButton) m_pFurnitureResetButton->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureBackButton) m_pFurnitureBackButton->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureSaveButton) m_pFurnitureSaveButton->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureOutButton) m_pFurnitureOutButton->SetElementFadeOut(bFadeOut);

	if (m_pFurniturePageUpButton) m_pFurniturePageUpButton->SetElementFadeOut(bFadeOut);
	if (m_pFurniturePageDownButton) m_pFurniturePageDownButton->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureListNumWnd) {
		m_pFurnitureListNumWnd->SetVisible(bShow);
		m_pFurnitureListNumWnd->ClearLine();
		m_pFurnitureListNumWnd->AddText(MakeStr("%3d", m_furniturePage + 1), false, true);
	}
	if (m_pFurniturePageCountWnd) {
		m_pFurniturePageCountWnd->SetVisible(bShow);
		m_pFurniturePageCountWnd->ClearLine();
		m_pFurniturePageCountWnd->AddText(MakeStr("%3d", m_furniturePageCount), false, true);
	}

	if (m_pFurnitureListNumButton) {
		m_pFurnitureListNumButton->SetElementFadeOut(bFadeOut);
	}

	for (int i = 0; i < 8; i++) {
		if (m_pFurnitureBackGround[i]) {
			m_pFurnitureBackGround[i]->SetElementFadeOut(bFadeOut);
		}
	}

	for (int i = 0; i < 3; i++) {
		if (m_pFurnitureScroll[i]) {
			m_pFurnitureScroll[i]->SetElementFadeOut(bFadeOut);
		}
	}

	m_pFurnitureMainTab->Select(m_furnitureMainTab);
}

// realmyroommainui.cpp:15589
void CRealMyRoomMain::PostDlgCreate(void) {
	if (m_pPostDlg == NULL) {
		m_pPostDlg = CreateForm<CPost_ReciveDlg>(g_pFresh->GetManager(), this, "postbox", NULL);

		FrForm::ms_bHasTail = false;
		m_pPostDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnPostDlgResult, 0x81);
		m_pPostDlg->SetFadeout(false);
	} else {
		m_pPostDlg->OnInit();
	}
}

// realmyroommainui.cpp:15605
void CRealMyRoomMain::ControlUiHideMode(int mode) {
	if (mode != m_uiState && mode != 5) {
		m_prevUiState = m_uiState;
	}

	m_pRoomFx->SetActive(false, true);

	switch (mode) {
	case 1:
		SetUIState(1);

		ShowOverAndUnderBar(true);
		for (int i = 0; i < 3; i++) {
			if (m_barFrame[i] && m_barFrame[i]->IsVisible()) {
				m_barFrame[i]->SetElementFadeOut(false);
			}
		}
		SetCameraDisplayType(1);

		m_bHideMiddleUI = true;
		if (m_pMiddleUIWnd) {
			m_pMiddleUIWnd->SetElementFadeOut(false);
		}
		ChangeMainTab(m_mainTab, true);

		FurnitureDisplayControl(false);
		if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar) {
			WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetState(2);
			WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetHide(false);
		}
		break;
	case 2:
		SetUIState(2);

		ShowOverAndUnderBar(true);
		for (int i = 0; i < 3; i++) {
			if (m_barFrame[i] && m_barFrame[i]->IsVisible()) {
				m_barFrame[i]->SetElementFadeOut(false);
			}
		}

		m_bHideMiddleUI = false;
		if (m_pMiddleUIWnd && m_pMiddleUIWnd->IsVisible()) {
			m_pMiddleUIWnd->SetElementFadeOut(true);
		}
		FurnitureDisplayControl(false);

		SetCameraDisplayType(4);
		if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar) {
			WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->ChangeCharacterInfo();
			WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetState(0);
			WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetHide(true);
		}
		break;
	case 4:
		m_pRoomFx->SetActive(true, true);

		SetUIState(8);
		for (int i = 0; i < 3; i++) {
			if (m_barFrame[i] && m_barFrame[i]->IsVisible()) {
				m_barFrame[i]->SetElementFadeOut(true);
			}
		}
		ShowOverAndUnderBar(false);

		m_bHideMiddleUI = false;
		if (m_pMiddleUIWnd && m_pMiddleUIWnd->IsVisible()) {
			m_pMiddleUIWnd->SetElementFadeOut(true);
		}
		FurnitureDisplayControl(true);

		SetCameraDisplayType(4);
		if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar) {
			WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->ChangeCharacterInfo();
			WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetState(0);
			WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->SetHide(true);
		}
		break;
	case 5:
		if (m_prevUiState == 1) {
			ControlUiHideMode((m_mainTab != 6) + 1);
			return;
		}
		ControlUiHideMode(2);
		return;
	}

	m_uiState = mode;
	SetListMenu();
}

// realmyroommainui.cpp:15722
void CRealMyRoomMain::SetOverAndUnderBarVisible(bool bVisible) {
	bool bFadeOut = !bVisible;

	if (bVisible) {
		ShowOverAndUnderBar(true);
	}

	for (int i = 0; i < 3; i++) {
		if (m_barFrame[i] && m_barFrame[i]->IsVisible()) {
			m_barFrame[i]->SetElementFadeOut(bFadeOut);
		}
	}

	if (bFadeOut) {
		ShowOverAndUnderBar(false);
	}
}

// realmyroommainui.cpp:15747
void CRealMyRoomMain::SetMiddleUIControl(bool bShow) {
	if (bShow) {
		SetCameraDisplayType(1);
	} else {
		SetCameraDisplayType(4);
	}
}

// realmyroommainui.cpp:15779
void CRealMyRoomMain::SetMiddleUIVisible(bool bVisible) {
	bool bFadeOut = !bVisible;

	if (bVisible) {
		if (m_pMiddleUIWnd != NULL) {
			m_pMiddleUIWnd->SetElementFadeOut(bFadeOut);
		}

		ChangeMainTab(m_mainTab, true);
	} else {
		if (m_pMiddleUIWnd != NULL && m_pMiddleUIWnd->IsVisible()) {
			m_pMiddleUIWnd->SetElementFadeOut(bFadeOut);
		}
	}
}

// realmyroommainui.cpp:15860
void CRealMyRoomMain::Furniture_ShowUI(bool bShow) {
	bool bFadeOut = !bShow;

	if (m_pFurnitureWndA) m_pFurnitureWndA->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureWndB) m_pFurnitureWndB->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureWndC) m_pFurnitureWndC->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureWndD) m_pFurnitureWndD->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureWndE) m_pFurnitureWndE->SetElementFadeOut(bFadeOut);

	if (m_pFurnitureWndF) m_pFurnitureWndF->SetElementFadeOut(bFadeOut);

	if (m_pFurnitureLeftWnd[0]) m_pFurnitureLeftWnd[0]->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureLeftWnd[1]) m_pFurnitureLeftWnd[1]->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureLeftWnd[2]) m_pFurnitureLeftWnd[2]->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureLeftWnd[3]) m_pFurnitureLeftWnd[3]->SetElementFadeOut(bFadeOut);
	if (m_pFurnitureLeftWnd[4]) m_pFurnitureLeftWnd[4]->SetElementFadeOut(bFadeOut);
}

// realmyroommainui.cpp:15881
void CRealMyRoomMain::Furniture_ShowUI_Left(bool bShow) {
	if (m_pFurnitureLeftWnd[0]) m_pFurnitureLeftWnd[0]->Enable(bShow);
	if (m_pFurnitureLeftWnd[1]) m_pFurnitureLeftWnd[1]->Enable(bShow);
	if (m_pFurnitureLeftWnd[2]) m_pFurnitureLeftWnd[2]->Enable(bShow);
	if (m_pFurnitureLeftWnd[3]) m_pFurnitureLeftWnd[3]->Enable(bShow);
	if (m_pFurnitureLeftWnd[4]) m_pFurnitureLeftWnd[4]->Enable(bShow);
}

// realmyroommainui.cpp:15978
void CRealMyRoomMain::OnEffectViewInit(PG_MSGARG param) {
	m_pEffectViewWnd = DYNAMIC_CAST(FrArea, param);
}

// realmyroommainui.cpp:15983
void CRealMyRoomMain::OnEffectViewOnwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == 0) {
		return;
	}

	pGI->Box(m_pEffectViewWnd->GetRect(), 0x18000000, 0, 0.0f);
}

// realmyroommainui.cpp:16026
void CRealMyRoomMain::OnRightMenuInit(PG_MSGARG param) {
	m_pRightMenuWnd = DYNAMIC_CAST(FrWnd, param);
	if (m_pRightMenuWnd) {
		m_pRightMenuWnd->SetVisible(false);
		m_pRightMenuWnd->SetFixed(true);
	}
}

// realmyroommainui.cpp:16038
void CRealMyRoomMain::OnRightMenuLBtnDown(void) {
	if (IsLocalContent((localContentType_t)0x71) == true && m_itemStorageVisible == true) {
		return;
	}

	const char* text = static_cast<FrContextMenuCtrl*>(m_pRightMenuWnd)->GetSelectMenuText();

	if (IsLocalContent((localContentType_t)0x76)) {
		// "Use furniture"
		if (strcmp(text, "\xB0\xA1\xB1\xB8\xBB\xE7\xBF\xEB") == 0) {
			FurnitureUse();
			return;
		}
		// "Run furniture"
		if (strcmp(text, "\xB0\xA1\xB1\xB8\xB1\xB8\xB5\xBF") == 0) {
			FurnitureRun();
			return;
		}
	}

	// "Select furniture"
	if (strcmp(text, "\xB0\xA1\xB1\xB8\xBC\xB1\xC5\xC3") == 0) {
		FurnitureFunctionChange();
	// "Move furniture"
	} else if (strcmp(text, "\xB0\xA1\xB1\xB8\xC0\xCC\xB5\xBF") == 0) {
		WSingleton<CRealMyRoomData>::Instance()->SetState(2);
	// "Rotate furniture"
	} else if (strcmp(text, "\xB0\xA1\xB1\xB8\xC8\xB8\xC0\xFC") == 0) {
		WSingleton<CRealMyRoomData>::Instance()->SetState(3);
	// "Remove furniture"
	} else if (strcmp(text, "\xB0\xA1\xB1\xB8\xC1\xA6\xB0\xC5") == 0) {
		FrListItem* pItem = m_pFurnitureList->FindItem(WSingleton<CRealMyRoomData>::Instance()->m_pSelectModel->m_pFurniture);

		if (pItem != NULL) {
			m_pFurnitureList->UnselectItem(pItem);
		}

		for (std::list<sFurniture_List>::iterator it = WSingleton<CSharedDoc>::Instance()->m_furnitureList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_furnitureList.end(); ++it) {
			if (it->m_id == WSingleton<CRealMyRoomData>::Instance()->m_pSelectModel->m_pFurniture->m_id) {
				it->m_flags &= ~1;
			}
		}

		WSingleton<CRealMyRoomData>::Instance()->SetState(6);
	}
}

// realmyroommainui.cpp:16105
void CRealMyRoomMain::OnRightMenuRBtnDown(void) {
	if (IsLocalContent(static_cast<localContentType_t>(0x71)) == true) {
		if (m_itemStorageVisible == true) {
			return;
		}
	}

	m_pRightMenuWnd->SetVisible(false);
}

// realmyroommainui.cpp:16119
void CRealMyRoomMain::UpdateRightMenu(void) {
	if (WSingleton<CRealMyRoomData>::Instance()->m_pSelectModel != NULL) {
		int kind = (WSingleton<CRealMyRoomData>::Instance()->m_pSelectModel->m_pFurniture->m_typeId >> 11) & 0x7FF;
		if (kind >= 7 && kind <= 9) {
			static_cast<FrContextMenuCtrl*>(m_pRightMenuWnd)->SetVisible(false);
			return;
		}

		if (MyUID() == WSingleton<CSharedDoc>::Instance()->m_myRoomOwnerUID
			|| !WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
			if (m_flags[3]) {
				static_cast<FrContextMenuCtrl*>(m_pRightMenuWnd)->SetMenuList("\xB0\xA1\xB1\xB8\xBB\xE7\xBF\xEB@rmr_i_use|\xB0\xA1\xB1\xB8\xC0\xCC\xB5\xBF@rmr_i_move|separator|\xB0\xA1\xB1\xB8\xC8\xB8\xC0\xFC@rmr_i_rotate|separator|\xB0\xA1\xB1\xB8\xC1\xA6\xB0\xC5@rmr_i_delete");
			} else {
				IFF_STRUCT::sFurniture* pFurniture = ItemManager()->FindFurniture(WSingleton<CRealMyRoomData>::Instance()->m_pSelectModel->m_pFurniture->m_typeId);
				if (pFurniture->m_rentalFlag != 0) {
					static_cast<FrContextMenuCtrl*>(m_pRightMenuWnd)->SetMenuList("\xB0\xA1\xB1\xB8\xBB\xE7\xBF\xEB@rmr_i_use|\xB0\xA1\xB1\xB8\xBC\xB1\xC5\xC3@rmr_i_select");
				} else {
					static_cast<FrContextMenuCtrl*>(m_pRightMenuWnd)->SetVisible(false);
				}
			}
		} else if (kind == 5 || kind == 0xD) {
			unsigned long typeId = WSingleton<CRealMyRoomData>::Instance()->m_pSelectModel->m_pFurniture->m_typeId;
			if (typeId == 0x48006820 || typeId == 0x48006821) {
				static_cast<FrContextMenuCtrl*>(m_pRightMenuWnd)->SetVisible(false);
			} else {
				static_cast<FrContextMenuCtrl*>(m_pRightMenuWnd)->SetMenuList("\xB0\xA1\xB1\xB8\xBB\xE7\xBF\xEB@rmr_i_use|\xB0\xA1\xB1\xB8\xBC\xB1\xC5\xC3@rmr_i_select");
			}
		} else {
			static_cast<FrContextMenuCtrl*>(m_pRightMenuWnd)->SetVisible(false);
		}

		if (IsLocalContent(static_cast<localContentType_t>(0x76)) && kind == 0xD) {
			if (IsLocalContent(static_cast<localContentType_t>(0x76))) {
				unsigned long typeId = WSingleton<CRealMyRoomData>::Instance()->m_pSelectModel->m_pFurniture->m_typeId;
				if (typeId != 0x48006820 && typeId != 0x48006821) {
					static_cast<FrContextMenuCtrl*>(m_pRightMenuWnd)->UnableMenuItem("\xB0\xA1\xB1\xB8\xBC\xB1\xC5\xC3");
					return;
				}
			} else {
				return;
			}
		}

		static_cast<FrContextMenuCtrl*>(m_pRightMenuWnd)->UnableMenuItem("\xB0\xA1\xB1\xB8\xBB\xE7\xBF\xEB");
	} else {
		m_pRightMenuWnd->SetVisible(false);
	}
}

// realmyroommainui.cpp:16202
void CRealMyRoomMain::PopupRightMenu(int x, int y) {
	if (MyUID() == WSingleton<CSharedDoc>::Instance()->m_myRoomOwnerUID
		|| !WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
		m_pRightMenuWnd->MoveWindow(WPoint((float)x, (float)y));

		m_pRightMenuWnd->SetVisible(true);
		m_pRightMenuWnd->SetTopmost(true);
	} else if (g_pFresh->IsCurrentLayout("REALMYROOMVISITOR")) {
		m_pRightMenuWnd->MoveWindow(WPoint((float)x, (float)y));

		m_pRightMenuWnd->SetVisible(true);
		m_pRightMenuWnd->SetTopmost(true);
	}

	UpdateRightMenu();
}

// realmyroommainui.cpp:16224
void CRealMyRoomMain::ExecuteMiniButtonDown(sItemInfo* pItemInfo) {
	switch (pItemInfo->m_typeId) {
	case 0x1A000003:
		OnOverBar_OptionUp();
		break;
	case 0x1A00003B:
		if (IsLocalContent((localContentType_t)0x40)) {
			OpenNewYearMoney(pItemInfo);
		}
		break;
	case 0x1A00003E:
	case 0x1A000048:
		OpenEventGiftBox(pItemInfo);
		break;
	case 0x1A000054:
	case 0x1A0000BC:
		OpenS4EventGiftBox(pItemInfo);
		break;
	case 0x1A000052:
		if (IsLocalContent((localContentType_t)0x24)) {
			OpenEventXmasGiftBox(pItemInfo);
		}
		break;
	case 0x1A000105:
	case 0x1A00014B:
		if (g_pFresh->GetDesktop()->FindChildForm("FrSupplyPackDlg") == NULL) {
			S5::FrSupplyPackDlg* pDlg = CreateForm<S5::FrSupplyPackDlg>(g_pFresh->GetManager(), NULL, "FrSupplyPackDlg", NULL);
			pDlg->Initialize(pItemInfo->m_typeId);
			pDlg->EnableDrag(false);
			pDlg->Open(NULL, 0);
		}
		break;
	case 0x1A00012F:
	case 0x1A000130:
	case 0x1A000131: {
		IActor* pGuildActor = GetActor(s_guildActorName);
		// The original tests the name, not the actor.
		if (s_guildActorName != NULL) {
			pGuildActor << MsgObject(0, 0x263, 0x1B, pItemInfo->m_typeId, 0, 0, 0);
		}
		break;
	}
	case 0x1A00015B: {
		IActor* pGateway = GetActor(s_gatewayActorName);
		pGateway << MsgObject(0, 0x276, 0, 0, 0, 0, 0);
		break;
	}
	default: {
		IFF_STRUCT::sItem* pIff = ItemManager()->FindItem(pItemInfo->m_typeId);
		if (pIff != NULL) {
			if (pIff->isRandomBox) {
				OpenNewRandomBox(pItemInfo);
			}
		}

		if (FrCardRemove::IsCardRemoveItem(pItemInfo->m_typeId)) {
			if (m_pCardRemoveDlg == NULL) {
				FrCardRemove* pDlg = CreateForm<FrCardRemove>(g_pFresh->GetManager(), this, "card_remove", NULL);
				m_pCardRemoveDlg = pDlg;
				pDlg->SetCardRemoveItem(pItemInfo->m_typeId, pItemInfo->m_serial);
				m_pCardRemoveDlg->SetCaption("\xC4\xAB\xB5\xE5\xC3\xDF\xC3\xE2\xBE\xD7");
				m_pCardRemoveDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnCardRemoveDlgResult, 0);
			}
		}
		break;
	}
	}
}

// realmyroommainui.cpp:16426
void CRealMyRoomMain::OnUnderBar_BackUp(void) {
	if (IsLocalContent(static_cast<localContentType_t>(0x71)) == true) {
		if (m_itemStorageVisible == true) {
			SetItemStorageVisible(false);
		} else {
			CTaskMain::OnUnderBar_BackUp();
		}
	} else {
		CTaskMain::OnUnderBar_BackUp();
	}
}

// realmyroommainui.cpp:16451
void CRealMyRoomMain::FurnitureUse(void) {
	if (!IsLocalContent((localContentType_t)0x76)) {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "The stove function cannot be used at the moment."
		pForm->SetMessage("\xB3\xAD\xB7\xCE \xB1\xE2\xB4\xC9\xC0\xBA \xC7\xF6\xC0\xE7 \xBB\xE7\xBF\xEB\xC7\xCF\xBD\xC7 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9. ", false);
		pForm->Open(0, 3);
		return;
	}

	sFurniture_List* pFurniture = WSingleton<CRealMyRoomData>::Instance()->m_pSelectModel->m_pFurniture;
	unsigned long typeId = pFurniture->m_typeId;

	if ((typeId & 0xFC000000) == 0x48000000 && (typeId & 0x3FF800) == 0x6800) {
		unsigned long id = pFurniture->m_id;

		if (!WSingleton<CRealMyRoomData>::Instance()->IsArragementFurniture(id)) {
			if (MyUID() == WSingleton<CSharedDoc>::Instance()->m_myRoomOwnerUID) {
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				// "This furniture is not placed. Place it, press save, then use it."
				pForm->SetMessage("\xB9\xE8\xC4\xA1\xB5\xC7\xC1\xF6 \xBE\xCA\xC0\xBA \xB0\xA1\xB1\xB8\xC0\xD4\xB4\xCF\xB4\xD9. \xB9\xE8\xC4\xA1\xC7\xD1 \xB4\xD9\xC0\xBD \xC0\xFA\xC0\xE5\xB9\xF6\xC6\xB0\xC0\xBB \xB4\xAD\xB7\xAF \xC0\xFA\xC0\xE5\xC7\xCF\xBD\xC5 \xC8\xC4\xBF\xA1 \xBB\xE7\xBF\xEB\xC7\xCF\xBD\xC7 \xBC\xF6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.", false);
				pForm->Open(0, 3);
			} else {
				WSendPacket packet((enumClientPacket)0xD9);
				packet.Encode4(id);
				packet.Encode4(0xFFFFFFFF);
				packet.Send(SEND_TO_DEFAULT);
			}
		} else {
			WSendPacket packet((enumClientPacket)0xD9);
			packet.Encode4(id);
			packet.Encode4(0xFFFFFFFF);
			packet.Send(SEND_TO_DEFAULT);
		}
	} else {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "This furniture cannot open the use and run menu."
		pForm->SetMessage("\xB0\xA1\xB1\xB8 \xBB\xE7\xBF\xEB \xB9\xD7 \xB1\xB8\xB5\xBF \xB8\xDE\xB4\xBA \xBF\xAD\xB1\xE2\xB0\xA1 \xBA\xD2\xB0\xA1\xB4\xC9\xC7\xD1 \xB0\xA1\xB1\xB8\xC0\xD4\xB4\xCF\xB4\xD9.", false);
		pForm->Open(0, 3);
	}
}

// realmyroommainui.cpp:16513
void CRealMyRoomMain::FurnitureRun(void) {
	if (!IsLocalContent((localContentType_t)0x76)) {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "The stove function cannot be used at the moment."
		pForm->SetMessage("\xB3\xAD\xB7\xCE \xB1\xE2\xB4\xC9\xC0\xBA \xC7\xF6\xC0\xE7 \xBB\xE7\xBF\xEB\xC7\xCF\xBD\xC7 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9. ", false);
		pForm->Open(0, 3);
		return;
	}

	if (m_pFurnitureAbilityDlg == NULL) {
		m_pFurnitureAbilityDlg = CreateForm<CFurniture_AbilityDlg>(g_pFresh->GetManager(), this, "myitem_choice", 0);

		FrForm::ms_bHasTail = false;
		m_pFurnitureAbilityDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnFurniture_AbilityDlgResult, 0x81);
		m_pFurnitureAbilityDlg->SetFadeout(false);
	}
}

// realmyroommainui.cpp:16534
void CRealMyRoomMain::FurnitureFunctionChange(void) {
	IFF_STRUCT::sFurniture* pFurniture = ItemManager()->FindFurniture(WSingleton<CRealMyRoomData>::Instance()->m_pSelectModel->m_pFurniture->m_typeId);

	if (pFurniture->m_rentalFlag > 100) {
		if (MyUID() == WSingleton<CSharedDoc>::Instance()->m_myRoomOwnerUID
			|| !WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
			SelectMenu(3);
		} else {
			SelectMenu(4);
		}
	} else {
		switch (pFurniture->m_rentalFlag) {
		case 1:
			SelectMenu(0);
			break;
		case 2:
			PostDlgCreate();
			break;
		case 5:
			if (IsLocalContent((localContentType_t)0x71) == true) {
				SelectMenu(0xB);
			}
			break;
		}
	}
}

// realmyroommainui.cpp:16575
void CRealMyRoomMain::OnUnderBar_ItemStorageUp(void) {
	if (IsLocalContent(static_cast<localContentType_t>(0x71)) == true) {
		SelectMenu(11);
	}

	CIconToolTip::GetInstance()->ClearToolTip();
}

// realmyroommainui.cpp:16592
void CRealMyRoomMain::OnUnderBar_MyRoomUp(void) {
	if (g_pFresh->IsCurrentLayout("REALMYROOMVISITOR")) {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		pForm->SetMessage("\xB4\xD9\xB8\xA5\xC0\xAF\xC0\xFA\xC0\xC7 \xB9\xE6\xC0\xBB \xB9\xE6\xB9\xAE \xC1\xDF\xC0\xCE \xBB\xF3\xC5\xC2\xBF\xA1\xBC\xAD\xB4\xC2 \xB8\xB6\xC0\xCC\xB7\xEB\xC0\xB8\xB7\xCE \xB0\xA5 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
		pForm->Open(0, 3);
	} else {
		CTaskMain::OnUnderBar_MyRoomUp();
	}
}

// realmyroommainui.cpp:16603
void CRealMyRoomMain::OnUnderBar_GiftUp(void) {
	if (g_pFresh->IsCurrentLayout("REALMYROOMVISITOR")) {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		pForm->SetMessage("\xB4\xD9\xB8\xA5\xC0\xAF\xC0\xFA\xC0\xC7 \xB9\xE6\xC0\xBB \xB9\xE6\xB9\xAE \xC1\xDF\xC0\xCE \xBB\xF3\xC5\xC2\xBF\xA1\xBC\xAD\xB4\xC2 \xBC\xB1\xB9\xB0\xC7\xD4\xC0\xB8\xB7\xCE \xB0\xA5 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
		pForm->Open(0, 3);
	} else {
		SelectMenu(1);
	}
}

// realmyroommainui.cpp:16617
bool CRealMyRoomMain::OnPostDlgResult(int result, FrForm* pForm) {
	if (pForm != NULL) {
		FrForm::ms_bHasTail = true;
	}

	SetListMenu();
	m_pPostDlg = NULL;
	return true;
}

// realmyroommainui.cpp:16636
bool CRealMyRoomMain::OnFurniture_AbilityDlgResult(int result, FrForm* pForm) {
	if (pForm != NULL) {
		FrForm::ms_bHasTail = true;
	}

	if (result == 1) {
		unsigned long typeId = WSingleton<CRealMyRoomData>::Instance()->m_pSelectModel->m_pFurniture->m_typeId;

		if ((typeId & 0xFC000000) == 0x48000000 && (typeId & 0x3FF800) == 0x6800) {
			if (m_pFurnitureAbilityDlg != NULL) {
				static_cast<CFurniture_AbilityDlg*>(m_pFurnitureAbilityDlg)->GetItemInfo(NULL);

				FrForm* pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				// "Could not get the item the furniture needs to run."
				pNotify->SetMessage("\xB0\xA1\xB1\xB8 \xB1\xB8\xB5\xBF\xBF\xA1 \xC7\xCA\xBF\xE4\xC7\xD1 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xBE\xF2\xBE\xEE\xBF\xC0\xC1\xF6 \xB8\xF8\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.", false);
				pNotify->Open(0, 3);
			} else {
				FrForm* pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				// "Could not get the item the furniture needs to run."
				pNotify->SetMessage("\xB0\xA1\xB1\xB8 \xB1\xB8\xB5\xBF\xBF\xA1 \xC7\xCA\xBF\xE4\xC7\xD1 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xBE\xF2\xBE\xEE\xBF\xC0\xC1\xF6 \xB8\xF8\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.", false);
				pNotify->Open(0, 3);
			}
		} else {
			FrForm* pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			// "This furniture cannot open the use and run menu."
			pNotify->SetMessage("\xB0\xA1\xB1\xB8 \xBB\xE7\xBF\xEB \xB9\xD7 \xB1\xB8\xB5\xBF \xB8\xDE\xB4\xBA \xBF\xAD\xB1\xE2\xB0\xA1 \xBA\xD2\xB0\xA1\xB4\xC9\xC7\xD1 \xB0\xA1\xB1\xB8\xC0\xD4\xB4\xCF\xB4\xD9.", false);
			pNotify->Open(0, 3);
		}
	} else {
		FrForm* pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "Adding an item to the furniture was cancelled."
		pNotify->SetMessage("\xB0\xA1\xB1\xB8\xBF\xA1 \xBE\xC6\xC0\xCC\xC5\xDB \xC3\xDF\xB0\xA1\xC7\xCF\xB1\xE2\xB8\xA6 \xC1\xDF\xB4\xDC\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", false);
		pNotify->Open(0, 3);
	}

	m_pFurnitureAbilityDlg = NULL;
	return true;
}

// realmyroommainui.cpp:16717
bool CRealMyRoomMain::OnPostSendDlgResult(int result, FrForm* pForm) {
	if (pForm) {
		FrForm::ms_bHasTail = true;
	}

	if (result == 1) {
		if (m_pPostSendDlg) {
			WSendPacket packet(GCT_REQUEST_MAILBOX_SEND_MAIL);
			packet.Encode4(WSingleton<CSharedDoc>::Instance()->m_myUID);
			packet.Encode4(m_pPostSendDlg->m_sendUID);

			packet.Encode1(m_pPostSendDlg->GetSendListSize());
			packet.Encode8(m_pPostSendDlg->GetSendServiceCharge());
			packet.EncodeStr("");
			packet.EncodeStr(m_pPostSendDlg->GetLetterMsg());

			sMailIncludeItem item;
			std::list<sMailIncludeItem>* pList = m_pPostSendDlg->GetSendList();

			for (std::list<sMailIncludeItem>::iterator it = pList->begin(); it != pList->end(); ++it) {
				item = *it;
				packet.EncodeBuffer(&item, 0x35);
			}

			packet.Send(SEND_TO_DEFAULT);

			m_pPostSendDlg = NULL;
		}
	} else {
		m_pPostDlg = NULL;
		m_pPostSendDlg = NULL;

		m_pPostDlg = CreateForm<CPost_ReciveDlg>(g_pFresh->GetManager(), this, "postbox", NULL);
		FrForm::ms_bHasTail = false;
		m_pPostDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnPostDlgResult, 0x81);
		m_pPostDlg->SetFadeout(false);
	}

	return true;
}

// realmyroommainui.cpp:16771
bool CRealMyRoomMain::OnPostSendDlg2Result(int result, FrForm* pForm) {
	m_pPostSendDlg = NULL;

	if (result == 1) {
		m_pPostSendDlg = CreateForm<CPost_SendDlg>(g_pFresh->GetManager(), this, "write_post", NULL);
		FrForm::ms_bHasTail = false;
		m_pPostSendDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnPostSendDlgResult, 0x81);
		m_pPostSendDlg->SetFadeout(false);
	}

	return true;
}

// realmyroommainui.cpp:16789
bool CRealMyRoomMain::OnAuthDlgResult(int result, FrForm* pForm) {
	if (pForm != NULL) {
		FrForm::ms_bHasTail = true;
	}

	if (result == 1) {

		if (m_pAuthorityDlg != NULL) {

			bool authSetting = m_pAuthorityDlg->GetAuthSetting();
			m_pAuthorityDlg->GetPassWord();

			if (strlen(WSingleton<CSharedDoc>::Instance()->m_authPassword) == 0 && authSetting == true) {

				FrForm* pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "rmr_notify", 0);
				// "No password was entered, so setting the permission failed."
				pNotify->SetMessage("\xBA\xF1\xB9\xD0\xB9\xF8\xC8\xA3 \xC0\xD4\xB7\xC2\xC0\xCC \xB5\xC7\xC1\xF6 \xBE\xCA\xBE\xC6\xBC\xAD \xB1\xC7\xC7\xD1 \xBC\xB3\xC1\xA4\xBF\xA1 \xBD\xC7\xC6\xD0\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", false);
				pNotify->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnAuthDlgFaileResult, 2);

			} else {

				WSingleton<CSharedDoc>::Instance()->m_authSetting = m_pAuthorityDlg->GetAuthSetting();

				WSendPacket packet((enumClientPacket)0xB0);
				packet.EncodeBuffer(&WSingleton<CSharedDoc>::Instance()->m_realMyRoomUID, 0x6B);
				packet.Send(SEND_TO_DEFAULT);
			}
		}
	}

	SetListMenu();

	m_pAuthorityDlg = NULL;
	return true;
}

// realmyroommainui.cpp:16827
bool CRealMyRoomMain::OnAuthDlgFaileResult(int result, FrForm* pForm) {
	if (result == 1) {
		SelectMenu(8);
	}

	return true;
}

// realmyroommainui.cpp:16837
bool CRealMyRoomMain::GetArrangement(unsigned long typeId, unsigned long serial) {
	if ((typeId & 0xFC000000) != 0x48000000) {
		return false;
	}

	for (cModel* pModel = WSingleton<CRealMyRoomData>::Instance()->m_modelList.Start(); pModel; pModel = WSingleton<CRealMyRoomData>::Instance()->m_modelList.Next()) {
		if (pModel != NULL) {
			if (pModel->m_pFurniture->m_id == serial) {
				return true;
			}
		}
	}

	return false;
}

// realmyroommainui.cpp:16864
void CRealMyRoomMain::RMR_UpdateDB(void) {
	std::list<sFurniture_List> changedList;

	if (WSingleton<CSharedDoc>::Instance()->m_furnitureList.size() <= 0) {
		return;
	}

	changedList.clear();

	for (std::list<sFurniture_List>::iterator it = WSingleton<CSharedDoc>::Instance()->m_furnitureList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_furnitureList.end(); ++it) {
		IFF_STRUCT::sFurniture* pIff = ItemManager()->FindFurniture(it->m_typeId);
		if (pIff == NULL) {
			FrForm* pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			pNotify->SetMessage("\xC7\xF6\xC0\xE7\xB4\xC2 \xC0\xFA\xC0\xE5\xC7\xD2\xBC\xF6 \xBE\xF8\xB4\xC2 \xBB\xF3\xC5\xC2\xC0\xD4\xB4\xCF\xB4\xD9.", false);
			pNotify->Open(0, 3);
			continue;
		}

		it->m_flags &= ~1;

		cModel* pModel = WSingleton<CRealMyRoomData>::Instance()->GetModelData(it->m_id, it->m_typeId);

		sFurniture_List furniture;
		if (pModel != NULL && (pModel->m_pFurniture->m_typeId & 0xFC000000) == 0x48000000) {
			pModel->m_pFurniture->m_flags |= 1;
			it->m_flags |= 1;

			memcpy(&furniture, pModel->m_pFurniture, sizeof(furniture));
			if (pModel->m_pModelData != NULL) {
				furniture.m_pos.x = pModel->m_pModelData->m_basePos.x;
				furniture.m_pos.y = pModel->m_pModelData->m_basePos.y;
				furniture.m_pos.z = pModel->m_pModelData->m_basePos.z;
			}

			int change = WSingleton<CRealMyRoomData>::Instance()->IsFunritureValueChange(*pModel);
			if (change == -1 || (change > 0 && change <= 2)) {
				changedList.push_back(furniture);
			}
		} else if (WSingleton<CRealMyRoomData>::Instance()->IsModelData_SaveList(it->m_id, it->m_typeId)) {
			memset(&furniture, 0, sizeof(furniture));
			furniture.m_typeId = it->m_typeId;
			furniture.m_id = it->m_id;
			furniture.m_pos.x = it->m_pos.x;
			furniture.m_pos.y = it->m_pos.y;
			furniture.m_pos.z = it->m_pos.z;
			furniture.m_angle = it->m_angle;
			furniture.m_flags &= ~1;

			changedList.push_back(furniture);
		}
	}

	if (changedList.size() <= 0) {
		FrForm* pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		pNotify->SetMessage("\xBA\xAF\xB0\xE6\xC7\xD1 \xB3\xBB\xBF\xEB\xC0\xCC \xBE\xF8\xC0\xB8\xB9\xC7\xB7\xCE \xC0\xFA\xC0\xE5\xB5\xC7\xC1\xF6 \xBE\xCA\xBE\xD2\xBD\xC0\xB4\xCF\xB4\xD9.", false);
		pNotify->Open(0, 3);
	} else {
		WSendPacket packet((enumClientPacket)0xAE);
		packet.Encode2((unsigned short)changedList.size());

		for (std::list<sFurniture_List>::iterator it = changedList.begin(); it != changedList.end(); ++it) {
			packet.EncodeBuffer(&*it, 0x1B);
		}

		packet.Send(SEND_TO_DEFAULT);
	}
}

// realmyroommainui.cpp:16971
void CRealMyRoomMain::LoadCardImages(void) {
	for (std::map<unsigned int, IFF_STRUCT::sCard>::const_iterator it = ItemManager()->m_cardMap.begin(); it != ItemManager()->m_cardMap.end(); ++it) {

		const IFF_STRUCT::sCard& card = it->second;
		if (m_cardImgMap.find(card.m_typeId) == m_cardImgMap.end()) {

			char path[64];
			sprintf(path, "%s.png", card.m_model);
			WOverlay* overlay = g_resrcmng->GetOverlay(path, 0x80000000);

			if (overlay) {
				m_cardImgMap.insert(std::map<unsigned int, WOverlay*>::value_type(card.m_typeId, overlay));
			}
		}
	}
}

// realmyroommainui.cpp:17011
void CRealMyRoomMain::DeleteCardImages(void) {
	for (std::map<unsigned int, WOverlay*>::iterator it = m_cardImgMap.begin(); it != m_cardImgMap.end(); ++it) {
		if (g_resrcmng && it->second) { g_resrcmng->Release(it->second); it->second = 0; }
	}
}

// realmyroommainui.cpp:17028
WOverlay* CRealMyRoomMain::GetCardImg(unsigned long typeId) {
	std::map<unsigned int, WOverlay*>::iterator it = m_cardImgMap.find(typeId);
	IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(typeId);

	WOverlay* overlay = NULL;
	if (it == m_cardImgMap.end()) {

		pCard = ItemManager()->FindCard(typeId);

		if (pCard != NULL) {

			char path[64];
			sprintf(path, "%s.png", pCard->m_model);
			overlay = g_resrcmng->GetOverlay(path, 0);
			if (overlay) {
				m_cardImgMap.insert(std::map<unsigned int, WOverlay*>::value_type(pCard->m_typeId, overlay));
			}
		}
	} else {
		overlay = it->second;
	}

	return overlay;
}

// realmyroommainui.cpp:17055
void CRealMyRoomMain::DrawCard(void) {
	if (m_cardTypeId != 0) {
		IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(m_cardTypeId);

		if (pCard != NULL) {
			WOverlay* pImg = GetCardImg(m_cardTypeId);
			if (pImg != NULL) {
				WRECT screenRect = { 92.0f, 117.0f, 248.0f, 499.0f };
				WRECT texRect = { 0.0f, 0.0f, 1.0f, 1.0f };
				pImg->Render(g_view, texRect, screenRect, 0, 0xFFFFFFFF, 0.0f, 0);
			}
		}
	}
}

// realmyroommainui.cpp:17076
void CRealMyRoomMain::DrawPartsSlot(FrGraphicInterface* pGI, FrListItem* pItem) {
	sItemInfo* pInfo = (sItemInfo*)pItem->pData;
	if (!pInfo)
		return;

	if (!pGI) {
		pGI = g_pFresh->GetManager()->GetGDI();
		if (!pGI)
			return;
	}

	int i;
	float charX = 5.0f;
	float caddieX = 5.0f;
	if (pInfo->m_charCardSlotCount == 0 && pInfo->m_caddieCardSlotCount == 0 && m_pShopMarkBitmap) {
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pInfo->m_typeId);
		if (pPart) {
			if (pPart->m_shopMark == 1) {
				pGI->DrawTexture(m_pShopMarkBitmap, WRect(pItem->pos.x + 5.0f, pItem->pos.y + 65.0f, (float)m_pShopMarkBitmap->Width(), (float)m_pShopMarkBitmap->Height()), 0xFFFFFFFF, 0);
			}
		}
	}

	if (!((unsigned short)pInfo->m_charCardSlotCount > -1 && (unsigned short)pInfo->m_charCardSlotCount < 5)) {
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pInfo->m_typeId);
		if (pPart) {
			pInfo->m_charCardSlotCount = pPart->m_charCardSlots;
			if (!((unsigned short)pInfo->m_charCardSlotCount > -1 && (unsigned short)pInfo->m_charCardSlotCount < 5)) {
				pInfo->m_charCardSlotCount = 0;
			}
		} else {
			pInfo->m_charCardSlotCount = 0;
		}
	}

	for (i = 1; i < (unsigned short)pInfo->m_charCardSlotCount + 1; i++) {
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it = CSharedDoc::Instance()->m_cardPeriodList[0].begin(); it != CSharedDoc::Instance()->m_cardPeriodList[0].end(); ++it) {
			if (pInfo->m_typeId == it->partsTypeId && pInfo->m_serial == it->partsId) {
				IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(it->typeId);
				if (pCard) {
					if (i == it->slot) {
						if (it->kind == 0) {
							found = true;
							const Bitmap* pBitmap = g_pFresh->GetBitmap(pCard->m_texture);
							if (pBitmap) {
								pGI->DrawTexture(pBitmap, WRect(charX + pItem->pos.x, pItem->pos.y + 54.0f, (float)pBitmap->Width(), (float)pBitmap->Height()), 0xFFFFFFFF, 0);
								charX += 16.0f;
							}
						}
					}
				}
			}
		}
		if (!found) {
			const Bitmap* pBitmap = g_pFresh->GetBitmap("card_character_empty");
			if (pBitmap) {
				pGI->DrawTexture(pBitmap, WRect(charX + pItem->pos.x, pItem->pos.y + 54.0f, (float)pBitmap->Width(), (float)pBitmap->Height()), 0xFFFFFFFF, 0);
				charX += 16.0f;
			}
		}
	}

	if (!((unsigned short)pInfo->m_caddieCardSlotCount > -1 && (unsigned short)pInfo->m_caddieCardSlotCount < 5)) {
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pInfo->m_typeId);
		if (pPart) {
			pInfo->m_caddieCardSlotCount = pPart->m_caddieCardSlots;
			if (!((unsigned short)pInfo->m_caddieCardSlotCount > -1 && (unsigned short)pInfo->m_caddieCardSlotCount < 5)) {
				pInfo->m_caddieCardSlotCount = 0;
			}
		} else {
			pInfo->m_caddieCardSlotCount = 0;
		}
	}

	for (i = 1; i < (unsigned short)pInfo->m_caddieCardSlotCount + 1; i++) {
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it = CSharedDoc::Instance()->m_cardPeriodList[0].begin(); it != CSharedDoc::Instance()->m_cardPeriodList[0].end(); ++it) {
			if (pInfo->m_typeId == it->partsTypeId && pInfo->m_serial == it->partsId) {
				IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(it->typeId);
				if (pCard) {
					if (i == it->slot) {
						if (it->kind == 1) {
							found = true;
							const Bitmap* pBitmap = g_pFresh->GetBitmap(pCard->m_texture);
							if (pBitmap) {
								pGI->DrawTexture(pBitmap, WRect(caddieX + pItem->pos.x, pItem->pos.y + 70.0f, (float)pBitmap->Width(), (float)pBitmap->Height()), 0xFFFFFFFF, 0);
								caddieX += 16.0f;
								break;
							}
						}
					}
				}
			}
		}
		if (!found) {
			const Bitmap* pBitmap = g_pFresh->GetBitmap("card_caddie_empty");
			if (pBitmap) {
				pGI->DrawTexture(pBitmap, WRect(caddieX + pItem->pos.x, pItem->pos.y + 70.0f, (float)pBitmap->Width(), (float)pBitmap->Height()), 0xFFFFFFFF, 0);
				caddieX += 16.0f;
			}
		}
	}
}

// realmyroommainui.cpp:17227
bool CRealMyRoomMain::OnCardDlgResult(int result, FrForm* pForm) {
	FrCardUseDlg* pDlg = (FrCardUseDlg*)pForm;
	if (result == 1 && pDlg != NULL) {
		int cardKind = (pDlg->m_cardTypeId >> 22) & 0xF;
		if (cardKind == 2) {
			CTaskManager* pTaskManager = CTaskManager::Instance();
			(!pTaskManager ? 0 : pTaskManager->GetCurrentTask())->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));

			WSendPacket packet(GCT_USE_CARD_REQUEST);
			packet.Encode4(pDlg->m_cardTypeId);
			packet.Send(SEND_TO_DEFAULT);
		} else if (cardKind == 0 || cardKind == 1) {
			m_pItemAttachCardDlg = CreateForm<FrItemAttachCardDlg>(g_pFresh->GetManager(), this, "itemattachcarddlg", NULL);
			m_pItemAttachCardDlg->m_cardId = pDlg->m_cardId;
			m_pItemAttachCardDlg->m_cardTypeId = pDlg->m_cardTypeId;
			m_pItemAttachCardDlg->m_cardKind = (pDlg->m_cardTypeId >> 22) & 0xF;
			m_pItemAttachCardDlg->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnItemAttachCardDlgResult, 1);
		}
	}

	m_pCardUseDlg = NULL;
	return true;
}

// realmyroommainui.cpp:17257
bool CRealMyRoomMain::OnCardPackDlgResult(int result, FrForm* pForm) {
	if (result == 1) {
		FrOpenCardPackDlg* pDlg = (FrOpenCardPackDlg*)pForm;
		if (pDlg != NULL && (pDlg->m_cardTypeId & 0x3C00000) == 0xC00000) {
			CTaskManager* pTaskManager = CTaskManager::Instance();
			(!pTaskManager ? 0 : pTaskManager->GetCurrentTask())->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));

			WSendPacket packet(GCT_OPEN_CARDPACK);
			packet.Encode4(pDlg->m_cardTypeId);
			packet.Encode4(pDlg->m_cardId);
			packet.Send(SEND_TO_DEFAULT);
		}
	}

	m_pOpenCardPackDlg = NULL;
	return true;
}

// The card-to-item attach request (packet 0xC0), 0x3a bytes.
#pragma pack(push, 1)
struct sMyRoomAttachCardRequest {
	unsigned long cardId;          // 0x0
	unsigned long cardTypeId;      // 0x4
	unsigned long itemTypeId;      // 0x8
	unsigned long itemId;          // 0xc
	int slot;                      // 0x10
	unsigned long reserved14[9];   // 0x14, never filled
	unsigned short reserved38;     // 0x38
};
#pragma pack(pop)
CHECK_SIZE(sMyRoomAttachCardRequest, 0x3A);

// realmyroommainui.cpp:17280
bool CRealMyRoomMain::OnItemAttachCardDlgResult(int result, FrForm* pForm) {
	sMyRoomAttachCardRequest request;
	FrItemAttachCardDlg* pDlg = (FrItemAttachCardDlg*)pForm;
	if (pDlg == NULL) {
		return true;
	}

	if (result != 1) {
		return true;
	}

	if (pDlg->m_selectedSlot == 0xFF) {
		this << MsgObject(0, 0x23,
			(intptr_t)"\xc6\xc4\xc3\xf7\xc0\xc7 \xbd\xbd\xb7\xd4\xc0\xcc \xbc\xb1\xc5\xc3\xb5\xc7\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0);
		return true;
	}

	int cardKind = (pDlg->m_cardTypeId >> 22) & 0xF;
	if ((cardKind == 0 && pDlg->m_selectedSlot > 0 && pDlg->m_selectedSlot < 5)
		|| (cardKind == 1 && pDlg->m_selectedSlot > 4 && pDlg->m_selectedSlot < 9)) {
		request.cardId = pDlg->m_cardId;
		request.cardTypeId = pDlg->m_cardTypeId;
		request.itemTypeId = pDlg->m_itemTypeId;
		request.itemId = pDlg->m_itemId;
		request.slot = pDlg->m_selectedSlot;

		WSendPacket packet(GCT_PARTS_ATTACH_CARD);
		packet.EncodeBuffer(&request, sizeof(request));
		packet.Send(SEND_TO_DEFAULT);

		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
	} else {
		this << MsgObject(0, 0x23,
			(intptr_t)"\xbd\xbd\xb7\xd4\xb0\xfa \xc4\xab\xb5\xe5\xc0\xc7 \xc5\xb8\xc0\xd4\xc0\xcc \xb4\xde\xb6\xf3\xbc\xad \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0);
	}

	return true;
}

// realmyroommainui.cpp:17327
bool CRealMyRoomMain::OnResultDlg_Report(int result, FrForm* pForm) {
	m_pReportDlg = NULL;

	return true;
}

// realmyroommainui.cpp:17335
bool CRealMyRoomMain::OnUccDrawDlgResult(int result, FrForm* pForm) {
	m_pUccDrawDlg = NULL;
	unsigned long tidChar = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList().m_tidChar;
	m_pUccCopyHideWnd->SetVisible(true);
	if (g_view != NULL) {
		ShowPet(tidChar);
	}

	return true;
}

// realmyroommainui.cpp:17347
bool CRealMyRoomMain::OnUccCopyDlgResult(int result, FrForm* pForm) {
	m_pUccCopyDlg = NULL;
	m_pUccCopyHideWnd->SetVisible(true);
	return true;
}

// realmyroommainui.cpp:17355
void CRealMyRoomMain::OpenEventXmasGiftBox(sItemInfo* pItemInfo) {
	if (pItemInfo != NULL) {
		CGiftBoxEvent* pEvent = (CGiftBoxEvent*)WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0x24);
		if (pEvent == NULL) {
			return;
		}

		m_xmasGiftBoxSerial = pItemInfo->m_serial;

		pEvent->SetOpenedBoxTID(pItemInfo->m_typeId);

		if (pEvent->GetOpenedBoxTID() != 0) {
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);
			pForm->SetMessage("\xC5\xA9\xB8\xAE\xBD\xBA\xB8\xB6\xBD\xBA \xBC\xB1\xB9\xB0 \xBB\xF3\xC0\xDA\xB8\xA6 \xBF\xAD\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);
			pForm->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnOpenEventXmasGiftDlgResult, 1);
		}
	}
}

// realmyroommainui.cpp:17379
bool CRealMyRoomMain::OnOpenEventXmasGiftDlgResult(int result, FrForm* pForm) {
	return true;
}

// realmyroommainui.cpp:17400
bool CheckRandomBoxTypeID(unsigned long typeId) {
	IFF_STRUCT::sItem* pItem = ItemManager()->FindItem(typeId);
	if (pItem != NULL) {
		if (pItem->IsRandomBox()) {
			return true;
		}
	}

	return false;
}

// realmyroommainui.cpp:17481
void CRealMyRoomMain::OpenNewRandomBox(sItemInfo* pItemInfo) {
	if (pItemInfo != NULL) {
		if (CheckRandomBoxTypeID(pItemInfo->m_typeId)) {
			m_s4EventGiftBoxTypeId = pItemInfo->m_typeId;
			OnOpenedBoxDlg(pItemInfo->m_typeId, (FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnOpenNewRandomBoxDlgResult, this);
		} else {
			// "This gift box cannot be opened."
			std::string msg = "\xBF\xAD \xBC\xF6 \xBE\xF8\xB4\xC2 \xBC\xB1\xB9\xB0 \xBB\xF3\xC0\xDA\xC0\xD4\xB4\xCF\xB4\xD9.";
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x26, (intptr_t)msg.c_str(), 0, 0, 0, 0));
		}
	}
}

// realmyroommainui.cpp:17499
bool CRealMyRoomMain::OnOpenNewRandomBoxDlgResult(int result, FrForm* pForm) {
	if (result == 1) {

		if (m_s4EventGiftBoxTypeId != 0) {

			FrRandomBoxProgress* pDlg = CreateForm<FrRandomBoxProgress>(g_pFresh->GetManager(), this, "newRandomBox", 0);
			if (pDlg == NULL) {
				return false;
			}

			pDlg->SetBoxTid(m_s4EventGiftBoxTypeId);

			const char* pName = ItemManager()->GetItemName(m_s4EventGiftBoxTypeId);
			if (pName != NULL) {

				pDlg->SetExplane(MakeStr("%\xBC\xB1\xC5\xC3\xC7\xCF\xBD\xC5 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xBF\xA9\xB4\xC2 \xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9.", pName));
			}
			pDlg->SetFixed(true);
			pDlg->Open(0, 1);
		}
	}

	m_s4EventGiftBoxTypeId = 0;

	return true;
}

// realmyroommainui.cpp:17555
void CRealMyRoomMain::OpenItemStorageNotice(void) {
	if (m_pItemStorageNotice == NULL) {
		m_pItemStorageNotice = CreateForm<FrItemStorageNotice>(g_pFresh->GetManager(), this, "item_storage_notice", NULL);
	}

	if (m_pItemStorageNotice != NULL) {
		m_pItemStorageNotice->SetCaption("\xBE\xCB\xB8\xB2");
		m_pItemStorageNotice->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnResultItemStorageNotice, 9);
	}
}

// realmyroommainui.cpp:17570
bool CRealMyRoomMain::OnResultItemStorageNotice(int result, FrForm* pForm) {
	OpenItemStorageAccessPanel(0);
	m_pItemStorageNotice = NULL;
	return true;
}

// realmyroommainui.cpp:17579
void CRealMyRoomMain::OpenItemStorageAccessPanel(unsigned char type) {
	if (m_pItemStorageAccessPanel == NULL) {
		m_pItemStorageAccessPanel = CreateForm<FrItemStorageAccessPanel>(g_pFresh->GetManager(), this, "item_storage_access_panel", NULL);
	}

	if (m_pItemStorageAccessPanel != NULL) {
		WRect rect = m_pItemStorageAccessPanel->GetRect();

		rect.x = 100.0f;
		rect.y = 75.0f;
		m_pItemStorageAccessPanel->SetRect(rect);

		m_pItemStorageAccessPanel->SetFormType((FrItemStorageAccessPanel::eFormType)type);
		m_pItemStorageAccessPanel->Open((FRESH_RESULT_CALLBACK)&CRealMyRoomMain::OnResultItemStorageAccessPanel, 9);
	}
}

// realmyroommainui.cpp:17599
bool CRealMyRoomMain::OnResultItemStorageAccessPanel(int result, FrForm* pForm) {
	bool bClose = true;

	if (pForm) {
		FrItemStorageAccessPanel* pPanel = (FrItemStorageAccessPanel*)pForm;

		if (result == 1) {
			pPanel->SendPasswordToServer();
		} else if (pPanel->GetFormType() == 0) {
			bClose = false;
		}
	}

	m_pItemStorageAccessPanel = 0;

	return bClose;
}

// realmyroommainui.cpp:17631
void CRealMyRoomMain::OnInitItemStorageBGI(PG_MSGARG param) {
	m_pItemStorageDlg = (FrForm*)((__rtti_obj = (void*)param) != 0 ? (((IObject*)__rtti_obj)->IsKindOf(&FrArea::m_RTTI) ? (void*)param : 0) : 0);

	CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();

	if (pItemStorage != NULL) {
		pItemStorage->AddFrontUI(m_pItemStorageDlg);

		for (int i = 0; i < 0x15; i++) {
			if (m_barButton[i] != NULL && i != 3) {
				pItemStorage->AddBackGroundUI(m_barButton[i], false);
			}
		}

		pItemStorage->AddBackGroundUI(m_caddieViewer, false);
		pItemStorage->AddBackGroundUI(m_subSelectionWnd[1], false);
		pItemStorage->AddBackGroundUI(m_aztecViewer, false);
		pItemStorage->AddBackGroundUI(m_subSelectionWnd[2], false);
		pItemStorage->AddBackGroundUI(m_clubViewer, false);
		pItemStorage->AddBackGroundUI(m_subSelectionWnd[3], false);
		pItemStorage->AddBackGroundUI(m_mascotViewer, false);
		pItemStorage->AddBackGroundUI(m_subSelectionWnd[4], false);

		pItemStorage->AddBackGroundUI(m_oneLineReqBtn, false);

		pItemStorage->AddBackGroundUI(m_underBar, false);

		pItemStorage->AddBackGroundUI(m_pUpgradeButton, false);
		pItemStorage->AddBackGroundUI(m_pDowngradeButton, false);

		pItemStorage->AddBackGroundUI(m_pItemSelectWnd, false);

		pItemStorage->AddBackGroundUI(m_pPetAreaWnd[2], true);

		pItemStorage->AddBackGroundUI(m_pFurnitureArrangeButton, false);

		pItemStorage->AddBackGroundUI(m_pBtnMenu, true);
		pItemStorage->AddBackGroundUI(m_pMenuList, true);
	}

	if (!WSingleton<CSharedDoc>::Instance()->m_flag50F4) {
		if (IsLocalContent((localContentType_t)0x71)) {
			if (!m_itemStorageVisible) {
				if (!WSingleton<CSharedDoc>::Instance()->m_flag50F4) {
					WSendPacket packet((enumClientPacket)0xD5);
					packet.Send(SEND_TO_DEFAULT);

					Net_EquipDresserParts();
					Net_EquipCharacter();
					Net_EquipCaddie();
					Net_EquipItem();
					Net_EquipGear();
					Net_EquipSkin();
					Net_EquipMascot();
					if (IsLocalContent((localContentType_t)0x51)) {
						WSingleton<CSharedDoc>::Instance()->SendEquipItemSlot();
					} else {
						WSingleton<CSharedDoc>::Instance()->SendEquipItemSlotByMascot();
					}
				}
			} else {
				MsgObject msg(0, 0x234, 0, 0, 0, 0, 0);
				AfxGetTask()->GetActor("RealMyRoom") << msg;
			}
		}
	}
}

// realmyroommainui.cpp:17728
void CRealMyRoomMain::OnOwnerDrawItemStorageBGI(PG_MSGARG param) {
	CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();

	if (pItemStorage != NULL) {
		pItemStorage->DrawBackgroundImage();
	}
}

// realmyroommainui.cpp:17740
void CRealMyRoomMain::OnInitItemStoragePang(PG_MSGARG param) {
	m_pItemStoragePang = DYNAMIC_CAST(FrArea, param);
	CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
	if (pItemStorage) {
		pItemStorage->AddFrontUI(m_pItemStoragePang);
	}
}

// realmyroommainui.cpp:17754
void CRealMyRoomMain::OnOwnerDrawItemStoragePang(PG_MSGARG param) {
}

// realmyroommainui.cpp:17760
void CRealMyRoomMain::OnInitItemStoragePangMinus(PG_MSGARG param) {
	m_pItemStoragePangMinus = DYNAMIC_CAST(FrButton, param);
	if (m_pItemStoragePangMinus) {
		m_pItemStoragePangMinus->SetPushDelay(0.1f);

		CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
		if (pItemStorage != NULL) {
			pItemStorage->AddFrontUI(m_pItemStoragePangMinus);
		}
	}
}

// realmyroommainui.cpp:17777
void CRealMyRoomMain::OnLBDownItemStoragePangMinus(void) {
	CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
	if (pItemStorage != NULL) {
		if (!pItemStorage->IsLock()) {
			pItemStorage->OpenPangBank(FrItemStoragePangBank::TYPE_WITHDRAW);
		}
	}
}

// realmyroommainui.cpp:17791
void CRealMyRoomMain::OnInitItemStoragePangPlus(PG_MSGARG param) {
	m_pItemStoragePangPlus = DYNAMIC_CAST(FrButton, param);
	if (m_pItemStoragePangPlus) {
		m_pItemStoragePangPlus->SetPushDelay(0.1f);

		CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
		if (pItemStorage != NULL) {
			pItemStorage->AddFrontUI(m_pItemStoragePangPlus);
		}
	}
}

// realmyroommainui.cpp:17808
void CRealMyRoomMain::OnLBDownItemStoragePangPlus(void) {
	CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
	if (pItemStorage != NULL) {
		if (!pItemStorage->IsLock()) {
			pItemStorage->OpenPangBank(FrItemStoragePangBank::TYPE_DEPOSIT);
		}
	}
}

// realmyroommainui.cpp:17822
void CRealMyRoomMain::OnInitItemStorageList(PG_MSGARG param) {
	m_pItemStorageList = DYNAMIC_CAST(FrListBox, param);

	if (m_pItemStorageList) {
		m_pItemStorageList->UseDblClick(true);
		m_pItemStorageList->UseRightButton(true);

		CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
		if (pItemStorage != NULL) {
			pItemStorage->AddFrontUI(m_pItemStorageList);
			pItemStorage->SetItemStorageArea(m_pItemStorageList->GetRect());
		}
	}
}

// realmyroommainui.cpp:17843
void CRealMyRoomMain::OnOwnerDrawItemStorageList(PG_MSGARG param) {
	CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();

	if (pItemStorage != NULL) {
		const FrListItem* pItem = (const FrListItem*)param;

		if (pItem != NULL) {
			pItemStorage->DrawStoredItemInfo(m_pItemStorageList, *pItem);
		}
	}
}

// realmyroommainui.cpp:17860
void CRealMyRoomMain::OnLBDownItemStorageList(void) {
	if (IsLocalContent((localContentType_t)0x71) == true && m_itemStorageVisible == true) {

		if (m_pItemStorageList != NULL) {

			CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
			FrListItem* pItem = m_pItemStorageList->GetItemUnderCursor();

			if (pItemStorage != NULL) {

				if (!pItemStorage->IsLock() && pItem != NULL) {

					sStoredItemInfo* pInfo = (sStoredItemInfo*)pItem->pData;

					if (pInfo != NULL) {

						sStoredItemDragDrop dragDrop;
						memset(&dragDrop, 0, sizeof(dragDrop));

						dragDrop.index = pItem->idx;
						dragDrop.mode = sStoredItemDragDrop::MODE_NONE;

						// 0x14 is an sStoredItemInfo in the original (its operator= is called).
						dragDrop.slot = *pInfo;

						if (WSingleton<CItemStorage>::Instance() != NULL) {
							pItemStorage->SetDragDropItem(dragDrop);
						}
					}

					m_pItemStorageList->SelectItem(pItem, true);
				}
			}
		}
	}
}

// realmyroommainui.cpp:17904
void CRealMyRoomMain::OnRBDownItemStorageList(void) {
	if (IsLocalContent((localContentType_t)0x71) == true && m_itemStorageVisible == true) {

		if (m_pItemStorageList != NULL) {

			CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
			if (pItemStorage != NULL) {

				if (!pItemStorage->IsLock()) {

					FrListItem* pItem = m_pItemStorageList->GetItemUnderCursor();

					if (pItem != NULL) {

						sStoredItemInfo* pInfo = (sStoredItemInfo*)pItem->pData;

						if (pInfo != NULL) {

							unsigned long typeId = pInfo->GetTypeID();
							unsigned long serial = pInfo->GetGuid();

							OpenInformation(&typeId, 0, &serial);
						}

						m_pItemStorageList->SelectItem(pItem, true);
					}
				}
			}
		}
	}
}

// realmyroommainui.cpp:17940
void CRealMyRoomMain::OnDBClickItemStorageList(void) {
	if (IsLocalContent(static_cast<localContentType_t>(0x71)) == true && m_itemStorageVisible == true) {
		if (m_pItemStorageList != NULL) {
			CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
			if (pItemStorage != NULL) {
				if (!pItemStorage->IsLock()) {
					FrListItem* pItem = m_pItemStorageList->GetItemUnderCursor();

					if (pItem != NULL) {
						m_pItemStorageList->SelectItem(pItem, true);

						CItemStorage* pStorage = WSingleton<CItemStorage>::Instance();

						if (pStorage != NULL) {
							pStorage->ReqPopStoredItemInfo(*pItem);
						}
					}
				}
			}
		}
	}
}

// realmyroommainui.cpp:17972
void CRealMyRoomMain::OnMouseMoveItemStorageList(void) {
	CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
	if (pItemStorage != NULL) {
		pItemStorage->SetDrag(static_cast<sStoredItemDragDrop::MODE>(2));
	}
}

// realmyroommainui.cpp:17982
void CRealMyRoomMain::OnInitItemStorageIndexMsg(PG_MSGARG param) {
	m_pItemStorageIndexMsg = DYNAMIC_CAST(FrListBox, param);
	CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
	if (pItemStorage) {
		pItemStorage->AddFrontUI(m_pItemStorageIndexMsg);
	}
}

// realmyroommainui.cpp:17996
void CRealMyRoomMain::OnOwnerDrawItemStorageIndexMsg(PG_MSGARG param) {
	CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();

	if (pItemStorage != NULL) {
		WPoint pt(165.0f, 386.0f);
		pItemStorage->IndexControlDraw(pt);
	}
}

// realmyroommainui.cpp:18008
void CRealMyRoomMain::OnLBDownItemStorageIndexMsg(void) {
	CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();

	if (pItemStorage != NULL) {
		if (!pItemStorage->IsLock()) {
			pItemStorage->IndexControlClick();
		}
	}
}

// realmyroommainui.cpp:18022
void CRealMyRoomMain::OnInitItemStoragePageNext(PG_MSGARG param) {
	m_pItemStoragePageNext = DYNAMIC_CAST(FrButton, param);
	if (m_pItemStoragePageNext) {
		m_pItemStoragePageNext->SetPushDelay(0.1f);
		m_pItemStoragePageNext->SetVisible(false);
	}
}

// realmyroommainui.cpp:18034
void CRealMyRoomMain::OnLBDownItemStoragePageNext(void) {
	CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();

	if (pItemStorage != NULL) {
		if (!pItemStorage->IsLock()) {
			pItemStorage->IndexControlNext();
		}
	}
}

// realmyroommainui.cpp:18049
void CRealMyRoomMain::OnInitItemStoragePagePrev(PG_MSGARG param) {
	m_pItemStoragePagePrev = DYNAMIC_CAST(FrButton, param);
	if (m_pItemStoragePagePrev) {
		m_pItemStoragePagePrev->SetPushDelay(0.1f);
		m_pItemStoragePagePrev->SetVisible(false);
	}
}

// realmyroommainui.cpp:18061
void CRealMyRoomMain::OnLBDownItemStoragePagePrev(void) {
	CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();

	if (pItemStorage != NULL) {
		if (!pItemStorage->IsLock()) {
			pItemStorage->IndexControlPrev();
		}
	}
}

// realmyroommainui.cpp:18076
void CRealMyRoomMain::OnInitItemStorageClose(PG_MSGARG param) {
	m_pItemStorageClose = DYNAMIC_CAST(FrButton, param);
	if (m_pItemStorageClose) {
		m_pItemStorageClose->SetPushDelay(0.1f);
	}

	CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
	if (pItemStorage != NULL) {
		pItemStorage->AddFrontUI(m_pItemStorageClose);
	}
}

// realmyroommainui.cpp:18094
void CRealMyRoomMain::OnLBDownItemStorageClose(void) {
	CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();

	if (pItemStorage != NULL) {
		if (!pItemStorage->IsLock()) {
			SetItemStorageVisible(false);
		}
	}

	if ((__rtti_obj = AfxGetPreservedTask()) != 0 ? ((IObject*)__rtti_obj)->IsKindOf(&ntAvatarChatTask::m_RTTI) : false) {
		m_barButton[7]->Enable(false);
		m_barButton[8]->Enable(false);
	}
}

// realmyroommainui.cpp:18116
void CRealMyRoomMain::OnInitItemStorageDragDrop(PG_MSGARG param) {
	m_pItemStorageDragDrop = DYNAMIC_CAST(FrArea, param);
	CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
	if (pItemStorage) {
		pItemStorage->AddFrontUI(m_pItemStorageDragDrop);
	}
}

// realmyroommainui.cpp:18127
void CRealMyRoomMain::OnOwnerDrawItemStorageDragDrop(PG_MSGARG param) {
	CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();
	if (pItemStorage != NULL) {
		pItemStorage->DragDropItemDraw();
	}
}

// realmyroommainui.cpp:18137
void CRealMyRoomMain::SetItemStorageVisible(bool visible) {
	if (IsLocalContent((localContentType_t)0x71) == true) {
		m_itemStorageVisible = visible;

		CItemStorage* pItemStorage = WSingleton<CItemStorage>::Instance();

		if (pItemStorage != NULL) {
			if (m_itemStorageVisible == true) {
				pItemStorage->SetVisibleFrontUI(true);
				pItemStorage->SetEnableBackGroundUI(false);
				pItemStorage->SetVisibleBackGroundUI(false);

				pItemStorage->InitIndexControl(pItemStorage->GetTotalPage(), 5);

				if (m_pItemStorageList != NULL) {
					pItemStorage->MakeStoredItemInfo(m_pItemStorageList);

					if (m_pItemStoragePageNext != NULL && m_pItemStoragePagePrev != NULL) {
						if (pItemStorage->NeedPageIndexer() == true) {
							m_pItemStoragePageNext->SetVisible(true);
							m_pItemStoragePagePrev->SetVisible(true);
						} else {
							m_pItemStoragePageNext->SetVisible(false);
							m_pItemStoragePagePrev->SetVisible(false);
						}
					}
				}
			} else {
				pItemStorage->SetVisibleFrontUI(false);
				pItemStorage->SetEnableBackGroundUI(true);
				pItemStorage->SetVisibleBackGroundUI(true);

				if (m_pItemStorageList != NULL) {
					m_pItemStorageList->ClearItem();
				}

				pItemStorage->ClearStoredItemInfo();

				if (m_pItemStoragePageNext != NULL && m_pItemStoragePagePrev != NULL) {
					m_pItemStoragePageNext->SetVisible(false);
					m_pItemStoragePagePrev->SetVisible(false);
				}
			}
		}
	}
}

// realmyroommainui.cpp:18214
void CRealMyRoomMain::DrawCutinFrame(void) {
	if (m_roomMode == 0x21) {
		FrListItem* pSelected = m_pUccCopyHideWnd->GetSelected();
		if (pSelected == NULL)
			return;
		sItemInfo* pInfo = (sItemInfo*)pSelected->pData;
		if (pInfo == NULL)
			return;

		WOverlay* pCutin = GetCutinImg(pInfo->m_typeId);
		if (pCutin == NULL)
			return;

		WRect rect;
		rect.x = 72.0f;
		rect.y = 67.0f;
		rect.w = (float)pCutin->GetWidth();
		rect.h = (float)pCutin->GetHeight();
		pCutin->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), rect, 0, 0xFFFFFFFF, 0.0f, 0);
	}
}

// realmyroommainui.cpp:18261
WOverlay* CRealMyRoomMain::GetCutinImg(unsigned long typeId) {
	WOverlay* overlay;
	std::map<unsigned int, WOverlay*>::iterator it = m_cutinImgMap.find(typeId);
	if (it == m_cutinImgMap.end()) {

		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(typeId);

		if (pSkin) {
			char path[64];
			sprintf(path, "%s.jpg", pSkin->m_cutinImage);
			overlay = g_resrcmng->GetOverlay(path, 0);
			if (overlay) {
				m_cutinImgMap.insert(std::map<unsigned int, WOverlay*>::value_type(pSkin->m_typeId, overlay));
			}
		} else {
			return NULL;
		}
	} else {
		overlay = it->second;
	}

	return overlay;
}

// realmyroommainui.cpp:18301
void CRealMyRoomMain::DeleteCutinImages(void) {
	for (std::map<unsigned int, WOverlay*>::iterator it = m_cutinImgMap.begin(); it != m_cutinImgMap.end(); ++it) {
		if (g_resrcmng && it->second) { g_resrcmng->Release(it->second); it->second = 0; }
	}
}

// realmyroommainui.cpp:18313
bool CRealMyRoomMain::OnRentalExtensionDlgResult(int result, FrForm* pForm) {
	S5::FrRentalExtensionDlg* pDlg = (S5::FrRentalExtensionDlg*)((__rtti_obj = (void*)pForm) != 0 ? (((IObject*)__rtti_obj)->IsKindOf(&S5::FrRentalExtensionDlg::m_RTTI) ? (void*)pForm : 0) : 0);
	if (result == 3) {
		this << MsgObject(0, 1, 0, 0, 0, 0, 0);

		const sItemInfo* pInfo = pDlg->GetItemInfo();
		WSendPacket packet((enumClientPacket)0xE8);
		packet.Encode4(pInfo->m_serial);
		packet.Send(SEND_TO_DEFAULT);
	} else if (result == 4) {
		this << MsgObject(0, 1, 0, 0, 0, 0, 0);

		const sItemInfo* pInfo = pDlg->GetItemInfo();
		WSendPacket packet((enumClientPacket)0xE9);
		packet.Encode4(pInfo->m_serial);
		packet.Send(SEND_TO_DEFAULT);
	}

	return true;
}
