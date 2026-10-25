// JP-only game code: the JP CShopMain (JpShopMain, JpShopMain.h), the lobby
// shop's "shopmain" form, from the JP exe (node scripts/disasm.ts --jp
// 0x<va>). The KR CShopMain bodies (yapang/src/client/ProjectG/ShopMain.cpp)
// with the JP members and the JP differences; what is still declared only:
// JpShopMain.h, "Status".
#include <math.h>
#include <string.h>

#include "JpShopMain.h"
#include "JpLobbyGlue.h"
#include "JpItemGlue.h"
#include "ItemManager.h"
#include "RFLobbyDesktop.h"
#include "SharedDoc.h"
#include "TaskManager.h"
#include "CheckoutDlg.h"
#include "InputManager.h"
#include "ToolTip.h"
#include "actor.h"
#include "avatar_avatar.h"
#include "RankingDlg.h"
#include "user_info.h"
#include "exhibition.h"
#include "mascot.h"
#include "RTTI.h"
#include "frarea.h"
#include "frbutton.h"
#include "frcombobox.h"
#include "fredit.h"
#include "frgroupbox.h"
#include "frstatic.h"
#include "frtabbutton.h"
#include "frtextbutton.h"
#include "fresh.h"
#include "frlistbox.h"
#include "frscrollbar.h"
#include "frwndmanager.h"
#include "messengerdlg.h"
#include "wfont.h"
#include "wresrcmng.h"
#include "UccManager.h"
#include "frgraphicinterface.h"
#include "bitmap.h"
#include "RFTextManager.h"
#include "Localize.h"
#include "LobbyTask.h"
#include "SoundManager.h"
#include "Packet.h"
#include "RFTooltipHelper.h"
#include "UccLibrary.h"
#include "TitleManager.h"
#include "Titles_Client.h"
#include "wview.h"
#include "commonutil.h"

// The title manager (defined in ProjectG.cpp; JP data_f022c8).
extern TitleManager* g_TitleManager;

// The Wangreal zero rect at JP 0xf145f8 (defined in JpItemSearchDlg.cpp).
extern const WRect data_f145f8;

// The KR sub tab picture names (KR RealMyRoomMainUI.cpp; JP 0x604b00).
void SetSubTabImg(const char* imgs[], int index);

// 0xbed090: the JP FrGraphicInterface's title-font print (defined in
// JpRoomListState.cpp, not in a header yet).
void JpGdi_PrintTitleFont(FrGraphicInterface* gdi, WTitleFont* font, float x, float y,
	const char* text, float spacing, int align, float scale, unsigned long color);

// The number font's page list (JP 0xe3511c: "[font_wind.jpg").
static char* s_shopNumberFontFiles[] = { const_cast<char*>("[font_wind.jpg") };

// lib: 0xcea450 (m_RTTI's dynamic initializer, compiler-generated from this
// definition: WRTTI::WRTTI 0xa4c140, base FrTaskForm), 0xcea4c0 (the
// message map's initializer, storing the handlers), 0xa91930, 0xa91940
// (this-adjusting deleting-destructor thunks)
const WRTTI JpShopMain::m_RTTI("CShopMain", &FrTaskForm::m_RTTI);

// 0xa822c0
const sFRESH_MSGMAP* JpShopMain::GetMessageMap(void) const {
	return &JpShopMain::_MsgMap;
}

sFRESH_MSGMAP JpShopMain::_MsgMap = { &FrForm::_MsgMap, &JpShopMain::_MsgEntries[0] };

// The 61 rows of 0xe34908 (name, command and function type from the file,
// the handler from 0xcea4c0), in order.
sFRESH_ENTRY JpShopMain::_MsgEntries[] = {
	{ "shopview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnSortOptionComboInit },
	{ "shopview", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&JpShopMain::OnSortOptionComboDown },
	{ "searchgoods", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnSearchGoodsButtonInit },
	{ "searchgoods", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&JpShopMain::OnSearchGoodsLButtonDown },
	{ "exportselectlist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnExportSelectListInit },
	{ "exportselectlist", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&JpShopMain::OnExportSelectListUp },
	{ "main_bg1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnMainBg1Init },
	{ "main_bg2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnMainBg2Init },
	{ "main_bg3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnMainBg3Init },
	{ "main_bg4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnMainBg4Init },
	{ "item_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnItemListInit },
	{ "item_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnItemListOwnerDraw },
	{ "item_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&JpShopMain::OnItemListBtnDown },
	{ "item_list", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&JpShopMain::OnItemListRBtnDown },
	{ "tab_main", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnTabMainInit },
	{ "tab_main", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&JpShopMain::OnTabMainButtonUp },
	{ "tab_sub", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnTabSubInit },
	{ "tab_sub", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&JpShopMain::OnTabSubButtonUp },
	{ "item_search_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnItemSearchBtnInit },
	{ "item_search_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&JpShopMain::OnItemSearchBtnUp },
	{ "buy_all", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnBuyAllInit },
	{ "buy_all", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&JpShopMain::OnBuyAllUp },
	{ "buy_all", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnBuyHoverOn },
	{ "buy_all", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnBuyHoverOff },
	{ "buy_reset", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnBuyResetInit },
	{ "buy_reset", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&JpShopMain::OnBuyResetUp },
	{ "buy_reset", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnBuyHoverOn },
	{ "buy_reset", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnBuyHoverOff },
	{ "sidetab_buy", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnSideTabBuyInit },
	{ "sidetab_buy", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&JpShopMain::OnSideTabBuyUp },
	{ "sidetab_rental", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnSideTabRentalInit },
	{ "sidetab_rental", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&JpShopMain::OnSideTabRentalUp },
	{ "close_wnd", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&JpShopMain::OnCloseWndUp },
	{ "tabtxt2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnTabTxt2Init },
	{ "tabtxt2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnTabTxt3Init },
	{ "pet", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnPetInit },
	{ "pet", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnPetOwnerDraw },
	{ "skin_pet", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnSkinPetInit },
	{ "skin_pet2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnSkinPet2Init },
	{ "item_desc_bg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnItemDescBgInit },
	{ "item_desc_bg", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnItemDescBgOwnerDraw },
	{ "item_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnItemDescInit },
	{ "part_cart", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnPartCartListInit },
	{ "part_cart", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnPartCartListOwnerDraw },
	{ "auxpart_cart", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnAuxPartCartListInit },
	{ "auxpart_cart", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnAuxPartCartListOwnerDraw },
	{ "scroll_up", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnScrollUpInit },
	{ "scroll_up", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&JpShopMain::OnScrollUpUp },
	{ "scroll_down", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnScrollDownInit },
	{ "scroll_down", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&JpShopMain::OnScrollDownUp },
	{ "item_slot_frame1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnItemSlotFrame1Init },
	{ "item_slot_frame2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnItemSlotFrame2Init },
	{ "item_slot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnItemSlotInit },
	{ "item_slot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnItemSlotOwnerDraw },
	{ "stat_group", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnStatGroupInit },
	{ "stat_group.arrow_m", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnStatGroupArrowInit },
	{ "stat_group.epower", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnPowerEditInit },
	{ "stat_group.econtrol", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnControlEditInit },
	{ "stat_group.eimpact", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnImpactEditInit },
	{ "stat_group.espin", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnSpinEditInit },
	{ "stat_group.ecurve", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpShopMain::OnCurveEditInit },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0xa822d0
void JpShopMain::OnSortOptionComboDown(void) {
}

// 0xa822e0
void JpShopMain::OnSearchGoodsLButtonDown(void) {
}

// 0xa822f0
void JpShopMain::OnExportSelectListUp(void) {
}

// 0xa82300 (the KR InitShopDialogs; the JP has only the buy dialog)
void JpShopMain::InitShopDialogs(void) {
	m_buyDlg = NULL;
}

// 0xa82310 (the KR InitShopControls without the stat box members)
void JpShopMain::InitShopControls(void) {
	m_tabMainButton = NULL;
	m_tabSubButton = NULL;
	m_buyAll = NULL;
	m_buyReset = NULL;
	m_itemListBox = NULL;
	m_cartList = NULL;
	m_auxCartList = NULL;
	m_tabTxt2 = NULL;
	m_tabTxt3 = NULL;
	m_shopControl154 = NULL;
	m_petArea = NULL;
	m_petExhibition = NULL;
	m_statBox = NULL;
	m_statArrow = NULL;
	memset(m_statEdit, 0, sizeof(m_statEdit));
	m_hintWnd = NULL;
	m_hintTime = -10.0f;
	memset(m_equipSkin, 0, sizeof(m_equipSkin));
	memset(m_sideDir, 0, sizeof(m_sideDir));
	memset(m_sidePos, 0, sizeof(m_sidePos));
	m_blinkValue = 0.0f;
	m_blinkTime = 0.0f;
	m_pet = NULL;
	m_skinPet = NULL;
	m_itemSlotFrame1 = NULL;
	m_itemSlotFrame2 = NULL;
	m_statPartKind = 0;
	m_selectionArrow = NULL;
	memset(m_charPartIds, 0, sizeof(m_charPartIds));
	m_markBegin = NULL;
	m_levelPlusOne = 0;
	m_level = 0;
	m_selectItemMark = NULL;
}

// 0xa824e0 (JP): the form closes itself.
void JpShopMain::OnCloseWndUp(void) {
	Close(true);
}

// 0xa824f0 (JP)
void JpShopMain::SetSideTabsVisible(bool visible) {
	if (m_sideTabBuy) {
		m_sideTabBuy->SetVisible(visible);
	}
	if (m_sideTabRental) {
		m_sideTabRental->SetVisible(visible);
	}
}

// 0xa82530 (JP; the KR ChangeMainTab's two SetVisible calls)
void JpShopMain::HideScrollButtons(void) {
	m_scrollUp->SetVisible(false);
	m_scrollDown->SetVisible(false);
}

// 0xa82610 (the KR HidePet; WaitUntilWorkDone is 0x98ad40)
void JpShopMain::HidePet(bool bWait) {
	if (bWait) {
		m_anotherHand.WaitUntilWorkDone(9999);
	}

	if (m_petExhibition) {
		delete m_petExhibition;
		m_petExhibition = NULL;
	}
}

// 0xa82800 (the KR SetPetAreaRect on the JP sub tabs, KR + 1; the JP adds
// sub tab 35 to the second skin pet area)
void JpShopMain::SetPetAreaRect(void) {
	switch (m_subTab) {
	case 14:
	case 15:
	case 16:
		m_petArea = m_skinPet;
		break;
	case 17:
	case 34:
	case 35:
		m_petArea = m_skinPet2;
		break;
	default:
		m_petArea = m_pet;
		break;
	}
}

// 0xa82870
void JpShopMain::ChangeStatTab(statTab_t tab) {
	m_statTab = tab;
}

// 0xa82880 (the KR SafeLoadPet)
void JpShopMain::SafeLoadPet(unsigned long* pTypeId, bool bForce) {
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

// 0xa82db0
void JpShopMain::OnSortOptionComboInit(intptr_t param) {
	FrComboBox* wnd = DYNAMIC_CAST(FrComboBox, param);
	m_sortOptionCombo = wnd;
	if (wnd) {
		wnd->SetVisible(false);
		wnd->Enable(false);
	}
}

// 0xa82e40
void JpShopMain::OnSearchGoodsButtonInit(intptr_t param) {
	FrTextButton* wnd = DYNAMIC_CAST(FrTextButton, param);
	m_searchGoodsButton = wnd;
	if (wnd) {
		wnd->SetVisible(false);
		wnd->Enable(false);
	}
}

// 0xa82ed0 (JP): the button is hidden and disabled, not kept.
void JpShopMain::OnExportSelectListInit(intptr_t param) {
	FrButton* wnd = DYNAMIC_CAST(FrButton, param);
	if (wnd) {
		wnd->SetVisible(false);
		wnd->Enable(false);
	}
}

// 0xa82f40
void JpShopMain::OnMainBg1Init(intptr_t param) {
	m_mainBg[0] = DYNAMIC_CAST(FrArea, param);
}

// 0xa82fb0
void JpShopMain::OnMainBg2Init(intptr_t param) {
	m_mainBg[1] = DYNAMIC_CAST(FrArea, param);
}

// 0xa83020
void JpShopMain::OnMainBg3Init(intptr_t param) {
	m_mainBg[2] = DYNAMIC_CAST(FrArea, param);
}

// 0xa83090
void JpShopMain::OnMainBg4Init(intptr_t param) {
	m_mainBg[3] = DYNAMIC_CAST(FrArea, param);
}

// 0xa83170
void JpShopMain::OnPetInit(intptr_t param) {
	m_pet = DYNAMIC_CAST(FrArea, param);
}

// 0xa831e0
void JpShopMain::OnSkinPetInit(intptr_t param) {
	m_skinPet = DYNAMIC_CAST(FrArea, param);
}

// 0xa83250
void JpShopMain::OnSkinPet2Init(intptr_t param) {
	m_skinPet2 = DYNAMIC_CAST(FrArea, param);
}

// 0xa832c0
void JpShopMain::OnItemDescBgInit(intptr_t param) {
	m_itemDescBg = DYNAMIC_CAST(FrArea, param);
}

// 0xa83330 (the KR OnItemDescBgOwnerDraw)
void JpShopMain::OnItemDescBgOwnerDraw(intptr_t) {
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (graphic == NULL) {
		return;
	}

	int lineNum = m_itemDesc->GetLineNum();
	if (lineNum <= 0) {
		return;
	}

	float width[2], height[2];
	for (int i = 0; i < 2; i++) {
		width[i] = (float)m_itemDescBitmap[i]->Width();
		height[i] = (float)m_itemDescBitmap[i]->Height();
	}

	float y = m_itemDescOffset.y + 320.0f;
	graphic->DrawTexture(m_itemDescBitmap[0], WRect(m_itemDescOffset.x + 6.0f, y, width[0], height[0]), 0xffffffff, 0);
	graphic->DrawTexture(m_itemDescBitmap[1], WRect(m_itemDescOffset.x + 262.0f, y, width[1], height[1]), 0xffffffff, 0);
}

// 0xa83460
void JpShopMain::OnItemDescInit(intptr_t param) {
	m_itemDesc = DYNAMIC_CAST(FrEdit, param);
	if (m_itemDesc) {
		m_itemDesc->SetVisible(false);
		m_itemDescPos = WPoint(m_itemDesc->GetRect().x, m_itemDesc->GetRect().y + m_itemDesc->GetRect().h);
	}
}

// 0xa83500 (the KR SetItemDescRect on the JP sub tabs, KR + 1)
void JpShopMain::SetItemDescRect(void) {
	m_itemDesc->ClearLine();

	switch (m_subTab) {
	case SUB_TAB_CLUBSET:
	case 11:
		m_itemDescOffset = WPoint(0.0f, 30.0f);
		break;
	case SUB_TAB_AZTEC:
	case SUB_TAB_MASCOT:
	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
	case 33:
		m_itemDescOffset = WPoint(0.0f, 120.0f);
		break;
	default:
		m_itemDescOffset = WPoint(0.0f, 0.0f);
		break;
	}
}

// 0xa835d0
void JpShopMain::OnStatGroupInit(intptr_t param) {
	m_statBox = DYNAMIC_CAST(FrGroupBox, param);
}

// 0xa83640
void JpShopMain::OnStatGroupArrowInit(intptr_t param) {
	m_statArrow = DYNAMIC_CAST(FrArea, param);
}

// 0xa836b0
void JpShopMain::OnTabTxt2Init(intptr_t param) {
	m_tabTxt2 = DYNAMIC_CAST(FrArea, param);
}

// 0xa83720
void JpShopMain::OnTabTxt3Init(intptr_t param) {
	m_tabTxt3 = DYNAMIC_CAST(FrArea, param);
}

// 0xa83790 (JP): six main tabs, stab_1, stab_7 then stab_2..stab_5 (the
// KR adds stab_1..stab_5 then stab_7).
void JpShopMain::OnTabMainInit(intptr_t param) {
	m_tabMainButton = DYNAMIC_CAST(FrTabButton, param);
	if (m_tabMainButton) {
		const char* names[3] = { NULL, };

		names[0] = MakeStr("stab_%d_n", 1); names[1] = MakeStr("stab_%d_d", 1); names[2] = MakeStr("stab_%d_d", 1);
		m_tabMainButton->AddTab(names, 0, NULL);

		names[0] = MakeStr("stab_%d_n", 7); names[1] = MakeStr("stab_%d_d", 7); names[2] = MakeStr("stab_%d_d", 7);
		m_tabMainButton->AddTab(names, 1, NULL);

		for (int i = 2; i < 6; i++) {
			names[0] = MakeStr("stab_%d_n", i); names[1] = MakeStr("stab_%d_d", i); names[2] = MakeStr("stab_%d_d", i);
			m_tabMainButton->AddTab(names, i, NULL);
		}
	}
}

// 0xa838e0
void JpShopMain::OnTabSubInit(intptr_t param) {
	m_tabSubButton = DYNAMIC_CAST(FrTabButton, param);
}

// 0xa83950 (the KR OnItemListInit)
void JpShopMain::OnItemListInit(intptr_t param) {
	m_itemListBox = DYNAMIC_CAST(FrListBox, param);
	if (m_itemListBox == 0) {
		return;
	}

	m_itemListBox->UseRightButton(true);
	m_itemListBox->UseDummy(true);
	m_itemListBox->SetPushSound(0);
	m_itemListBox->AddNoRButtonRect(m_miniBtns[1].rect, 1);
	m_itemListBox->AddNoRButtonRect(m_miniBtns[2].rect, 2);
}

// 0xa83d00
void JpShopMain::OnBuyAllInit(intptr_t param) {
	m_buyAll = DYNAMIC_CAST(FrButton, param);
	if (m_buyAll) {
		m_buyAll->EnableHover(true, 0.0f);
	}
}

// 0xa83d80
void JpShopMain::OnBuyResetInit(intptr_t param) {
	m_buyReset = DYNAMIC_CAST(FrButton, param);
	if (m_buyReset) {
		m_buyReset->EnableHover(true, 0.0f);
	}
}

// 0xa83e00
void JpShopMain::OnSideTabBuyInit(intptr_t param) {
	m_sideTabBuy = DYNAMIC_CAST(FrButton, param);
	if (m_sideTabBuy) {
		m_sideTabBuy->SetPushDelay(0.2f);
		m_sideTabBuy->SetStatus(FrButton::PRESSED);
	}
}

// 0xa83e90
void JpShopMain::OnSideTabRentalInit(intptr_t param) {
	m_sideTabRental = DYNAMIC_CAST(FrButton, param);
	if (m_sideTabRental) {
		m_sideTabRental->SetPushDelay(0.2f);
		m_sideTabRental->SetStatus(FrButton::NORMAL);
	}
}

// 0xa83f20 (JP)
bool JpShopMain::IsRentalSideTab(void) {
	return m_shopTab == 1 && m_sideTabRental && m_sideTabRental->GetStatus() == FrButton::PRESSED;
}

// 0xa83f50
void JpShopMain::OnPartCartListInit(intptr_t param) {
	m_cartList = DYNAMIC_CAST(FrListBox, param);
}

// 0xa84040
void JpShopMain::OnAuxPartCartListInit(intptr_t param) {
	m_auxCartList = DYNAMIC_CAST(FrListBox, param);
}

// 0xa840b0 (the KR CheckAuxCartScroll; the JP places the arrows from the
// aux cart's rect, not the reset button's)
void JpShopMain::CheckAuxCartScroll(void) {
	FrScrollBar* scrollBar = m_auxCartList->GetScrollBar();
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
			WRect cartRect, downRect, upRect;
			m_auxCartList->GetClientRect(cartRect);
			m_scrollDown->GetClientRect(downRect);
			m_scrollUp->GetClientRect(upRect);

			downRect.x = cartRect.x - downRect.w + 118.0f;
			upRect.x = downRect.x - upRect.w - 2.0f;
			upRect.y = (cartRect.h - downRect.h) * 0.5f + cartRect.y - 156.0f;
			downRect.y = upRect.y;

			m_scrollUp->SetClientRect(upRect);
			m_scrollDown->SetClientRect(downRect);
		}
	} else {
		m_scrollUp->SetVisible(false);
		m_scrollDown->SetVisible(false);
	}
}

// 0xa84220
void JpShopMain::OnScrollUpInit(intptr_t param) {
	m_scrollUp = DYNAMIC_CAST(FrButton, param);
	if (m_scrollUp) {
		m_scrollUp->SetVisible(false);
	}
}

// 0xa842a0
void JpShopMain::OnScrollUpUp(void) {
	m_auxCartList->GetScrollBar()->ScrollUp(-1);
	CheckAuxCartScroll();
}

// 0xa842c0
void JpShopMain::OnScrollDownInit(intptr_t param) {
	m_scrollDown = DYNAMIC_CAST(FrButton, param);
	if (m_scrollDown) {
		m_scrollDown->SetVisible(false);
	}
}

// 0xa84340
void JpShopMain::OnScrollDownUp(void) {
	m_auxCartList->GetScrollBar()->ScrollDown(-1);
	CheckAuxCartScroll();
}

// 0xa84360
void JpShopMain::OnItemSlotFrame1Init(intptr_t param) {
	m_itemSlotFrame1 = DYNAMIC_CAST(FrArea, param);
	if (m_itemSlotFrame1) {
		m_itemSlotFrame1->SetVisible(false);
	}
}

// 0xa843e0
void JpShopMain::OnItemSlotFrame2Init(intptr_t param) {
	m_itemSlotFrame2 = DYNAMIC_CAST(FrArea, param);
	if (m_itemSlotFrame2) {
		m_itemSlotFrame2->SetVisible(false);
	}
}

// 0xa84460
void JpShopMain::OnItemSlotInit(intptr_t param) {
	m_itemSlot = DYNAMIC_CAST(FrListBox, param);
	if (m_itemSlot) {
		m_itemSlot->SetVisible(false);
		m_itemSlot->SetMouseEvent(false);
	}
}

// 0xa844e0
void JpShopMain::OnItemSearchBtnInit(intptr_t param) {
	m_itemSearchBtn = DYNAMIC_CAST(FrButton, param);
}

// 0xa84550
void JpShopMain::OnPowerEditInit(intptr_t param) {
	m_statEdit[0] = DYNAMIC_CAST(FrStatic, param);
}

// 0xa845c0
void JpShopMain::OnControlEditInit(intptr_t param) {
	m_statEdit[1] = DYNAMIC_CAST(FrStatic, param);
}

// 0xa84630
void JpShopMain::OnImpactEditInit(intptr_t param) {
	m_statEdit[2] = DYNAMIC_CAST(FrStatic, param);
}

// 0xa846a0
void JpShopMain::OnSpinEditInit(intptr_t param) {
	m_statEdit[3] = DYNAMIC_CAST(FrStatic, param);
}

// 0xa84710
void JpShopMain::OnCurveEditInit(intptr_t param) {
	m_statEdit[4] = DYNAMIC_CAST(FrStatic, param);
}

// 0xe348e0: the stat dots' pictures, by stat (power, control, accuracy,
// spin, curve), dot then slot (the KR dot_files).
static const char* s_shopDotFiles[5][2] = {
	{ "ab_power", "ab_power_slot" },
	{ "ab_control", "ab_control_slot" },
	{ "ab_accuracy", "ab_accuracy_slot" },
	{ "ab_spin", "ab_spin_slot" },
	{ "ab_curve", "ab_curve_slot" },
};

// 0xa847e0 (the KR LoadShopImages; the JP adds the limited / surprise
// sale marks, the "state_view" mini button and "SelectItem_Mark", and has
// no "stat_all" pictures)
void JpShopMain::LoadShopImages(void) {
	m_markNew = g_pFresh->RegisterBitmap("mark_new");
	m_markHot = g_pFresh->RegisterBitmap("mark_hot");
	m_markLimited = g_pFresh->RegisterBitmap("mark_lim");
	m_markLimitedTag = g_pFresh->RegisterBitmap("mark_limitetag");
	for (int i = 0; i < 3; i++) {
		m_markLimitedType[i] = g_pFresh->RegisterBitmap(MakeStr("mark_limite_type_%02d", i + 1));
	}
	m_markSurpriseSale = g_pFresh->RegisterBitmap("mark_surprise_sale");
	m_markBegin = g_pFresh->RegisterBitmap("mark_begin");

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 2; j++) {
			m_dotBitmaps[i][j] = g_pFresh->RegisterBitmap(s_shopDotFiles[i][j]);
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

	m_miniBtns[7].normal = g_pFresh->RegisterBitmap("state_view_n");
	m_miniBtns[7].over = g_pFresh->RegisterBitmap("state_view_o");
	m_miniBtns[7].rect = WRect(103.0f, 35.0f, m_miniBtns[7].normal->Width(), m_miniBtns[7].normal->Height());

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
	m_selectItemMark = g_pFresh->RegisterBitmap("SelectItem_Mark");
}

// 0xa863d0 (JP): the KR tooltip test is gone; the Refresh tooltip helper
// (0xe74da8) hides its tooltip (0x5dfad0). The JP still casts param and
// builds CIconToolTip's instance, and uses neither.
void JpShopMain::OnBuyHoverOff(intptr_t) {
	CIconToolTip::GetInstance();
	if (RFTooltipHelper::Instance()) {
		RFTooltipHelper::Instance()->HideTooltip();
	}
}

// 0xa86530 (the KR OnItemSlotOwnerDraw; the count is printed with the
// number font m_titleFont, 0xbed090)
void JpShopMain::OnItemSlotOwnerDraw(intptr_t param) {
	FrListItem* listItem = (FrListItem*)param;
	if (listItem == NULL) {
		return;
	}

	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (graphic == NULL) {
		return;
	}

	const Bitmap* slot = m_itemSlotNormal;
	if (slot) {
		graphic->DrawTexture(slot, WRect(listItem->pos.x, listItem->pos.y, (float)m_itemSlotNormal->Width(), (float)m_itemSlotNormal->Height()), 0xFFFFFFFF, 0);
	}

	sItemInfo* itemInfo = (sItemInfo*)listItem->pData;
	if (itemInfo == NULL) {
		return;
	}

	IFF_STRUCT::sItem* item = ItemManager()->FindItem(itemInfo->m_typeId);
	if (item == NULL) {
		return;
	}

	const Bitmap* icon = g_pFresh->GetBitmap(item->m_icon);
	if (icon) {
		graphic->DrawTexture(icon,
			WRect(listItem->pos.x + 2.0f, listItem->pos.y + 4.0f, (float)icon->Width(), (float)icon->Height()), 0xFFFFFFFF, 0);
	}

	int equipped = 0;
	if ((itemInfo->m_typeId & 0x2000000) == 0) {
		for (int i = 0; i < 10; i++) {
			if (itemInfo->m_typeId == WSingleton<CSharedDoc>::Instance()->m_equipItem[i]) {
				equipped++;
			}
		}
	}

	int count = itemInfo->m_count - equipped;
	JpGdi_PrintTitleFont(graphic, m_titleFont, listItem->pos.x + 45.0f, listItem->pos.y + 40.0f, MakeStr("%d", count), 1.0f, 2, 1.0f, 0xFFFFFFFF);
}

// 0xa86740 (JP): the main actor opens the item search (message 0x2D8).
void JpShopMain::OnItemSearchBtnUp(void) {
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x2D8, 0, 0, 0, 0, 0));
}

// 0xa868e0 (FrTaskForm [38]; the KR OnLoad's LoadShopImages): the shown
// character goes to the "LobbyAvatar" actor (message 0x187).
bool JpShopMain::Start(void) {
	LoadShopImages();
	sCharacterInfo* info = Jp_GameState_GetCharacterInfo(-1);
	if (info) {
		MsgObject msg(0, 0x187, (intptr_t)info, 1, 0, 0, 0);
		IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
		if (actor) {
			actor << msg;
		}
	}
	return true;
}

// 0xa86960 (FrTaskForm [39]): the equipped character is shown again.
bool JpShopMain::Shutdown(void) {
	FrTaskForm::Shutdown();
	sCharacterInfo* info = Jp_GameState_GetCharacterInfo(-1);
	if (info) {
		Jp_GameState_SetShownCharacter(info);
	}
	return true;
}

// JP CSharedDoc +0x11498 (the +0x1C of the object at +0x1147C; name ours):
// the key of the shop data the client holds, which 0x00FE sends and the
// 0x01B1 reader (JP 0x9d3354) updates. The port's CSharedDoc has no such
// member yet (REQUESTed as a JP_PAKS member): the JP's initial 0.
static uint32_t Jp_GameState_ShopDataKey(void) {
	return 0;
}

// 0xa86990 (the KR OpenLayout, shopmain.cpp:5268): 0x005C, 0x003D, then
// the JP 0x00FE with the shop data key, then the Fresh layout (0xbe9ca0).
void JpShopMain::OpenLayout(const char* layoutName) {
	WSendPacket packet(GCT_REQUEST_SERVER_TIME);
	packet.Send(SEND_TO_DEFAULT);

	WSendPacket packet2(GCT_REQUEST_CASH);
	packet2.Send(SEND_TO_DEFAULT);

	unsigned long key = Jp_GameState_ShopDataKey();
	WSendPacket packet3(0xFE);
	packet3.Encode4(key);
	packet3.Send(SEND_TO_DEFAULT);

	g_pFresh->OpenLayout(layoutName, this, false);
}

// 0xa86a80 (the KR SetPetCtrl on the JP shop; the JP CExhibition ctor
// 0x4ff080 has a last int argument, -1, the KR one does not). Category 1
// outside the skin sub tabs previews on LobbyAvatar (message 0x187, the type
// id's address); 7 and 8 have no exhibition; 4 sits at a third of the area's
// height; 18 is local content 0x57 (KR 0x56 + 1). Not written yet, no
// exhibition (as before): category 1 on the sub tabs 14-16 (the JP
// exhibition subclass 0x604750 with motion 0x62b750), 9 (the JP set item
// branch at 0xa871a4) and 31 (0xa86f38).
void JpShopMain::SetPetCtrl(unsigned long typeId) {
	WRect rect = m_petArea->GetRect();

	CExhibition* pet = NULL;
	bool loaded;
	switch (typeId >> 26) {
	case 1:
		if (m_subTab != 14 && m_subTab != 15 && m_subTab != 16) {
			m_petExhibition = NULL;
			IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
			if (actor) {
				actor << MsgObject(0, 0x187, (intptr_t)&typeId, 0, 0, 0, 0);
			}
		}
		return;

	case 4:
		pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h / 3.0f, 0.0f, true);
		pet->SetArea(rect.w, rect.h);
		pet->SetDistance(7.0f, true, false);
		loaded = pet->SetModel(typeId);
		break;

	case 5:
		pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.0f, true);
		pet->SetArea(rect.w, rect.h);
		pet->SetDistance(1.0f, true, false);
		loaded = pet->SetModel(typeId);
		break;

	case 6:
		pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.0f, true);
		pet->SetArea(rect.w, rect.h);
		pet->SetDistance(6.0f, true, false);
		loaded = pet->SetModel(typeId);
		break;

	case 7:
	case 8:
		m_petExhibition = NULL;
		return;

	case 16: {
		pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f + 15.0f, 0.35f, true);
		pet->SetArea(rect.w, rect.h);
		pet->SetDistanceRange(4.0f, 8.0f);
		pet->SetDistance(6.0f, true, false);
		loaded = pet->SetModel(typeId);
		IFF_STRUCT::sMascot* mascot = ItemManager()->FindMascot(typeId);
		CMascot::ChangeMascotMsgInTex(Jp_LookupText("\xB8\xDE\xBC\xBC\xC1\xF6"), mascot, pet->GetModel(), "default");  // 0xd458e0
		break;
	}

	case 18:
		if (!IsLocalContent((localContentType_t)0x57)) {
			return;
		}
		pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.0f, true);
		pet->SetArea(rect.w * 0.5f, rect.h * 0.5f);
		pet->SetDistance(15.0f, true, false);
		loaded = pet->SetModel(typeId);
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

// 0xa87370 (the KR ShowPet: Enter/Leave inlined, HidePet(false))
void JpShopMain::ShowPet(unsigned long typeId) {
	if (typeId) {
		m_anotherHand.Enter();

		if (m_petArea) {
			HidePet(false);
			SetPetCtrl(typeId);
		}

		m_anotherHand.Leave();
	}
}

// 0xa876f0 (JP): the CSharedDoc +0x91d byte is the KR m_myLevel.
void JpShopMain::InitLevelFields(void) {
	m_levelPlusOne = WSingleton<CSharedDoc>::Instance()->m_myLevel + 1;
	m_level = WSingleton<CSharedDoc>::Instance()->m_myLevel;
}

// 0xa87980 (JP)
void JpShopMain::RequestShopEnter(void) {
	WSendPacket packet(0x140);
	packet.Send(SEND_TO_DEFAULT);

	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
}

// 0xa87a30 (the KR ChangeSkin)
void JpShopMain::ChangeSkin(unsigned long typeId) {
	m_standPetTypeId = typeId;

	SafeLoadPet(&m_standPetTypeId, false);

	memcpy(m_equipSkin, WSingleton<CSharedDoc>::Instance()->m_equipSkin, sizeof(m_equipSkin));

	for (int i = 0; i < 6; i++) {
		if (m_equipSkin[i]) {
			IFF_STRUCT::sSkin* skin = ItemManager()->FindSkin(m_equipSkin[i]);
			if (skin && !(m_equipSkin[i] & 0x3C00000)) {
				m_sideDir[0] = skin->m_sideSlideMode[0];
				m_sidePos[0] = 0;
				m_sidePos[1] = 0;
				m_sideDir[1] = skin->m_sideSlideMode[1];
			}
		}
	}
}

// 0xa87db0 (reached through CHandOwner at +0x120)
void JpShopMain::OnAnotherHand(int nWork, void* pData) {
	if (nWork == 0) {
		ShowPet(*(unsigned long*)pData);
	}
}

// 0xa88120
void JpShopMain::DeleteCutinImages(void) {
	for (std::map<unsigned int, WOverlay*>::iterator it = m_cutinImgs.begin(); it != m_cutinImgs.end(); ++it) {
		if (g_resrcmng && it->second) {
			g_resrcmng->Release(it->second);
			it->second = 0;
		}
	}
}

// 0xa88450 (the KR HaveCharacter)
bool JpShopMain::HaveCharacter(unsigned long typeId) {
	const std::map<unsigned int, sCharacterInfo>& characters = WSingleton<CSharedDoc>::Instance()->m_characterMap;
	for (std::map<unsigned int, sCharacterInfo>::const_iterator i = characters.begin(); i != characters.end(); ++i) {
		if (typeId == i->second.typeId) {
			return true;
		}
	}

	return false;
}

// 0xa88540 (the KR ResetItemList)
void JpShopMain::ResetItemList(bool bKeepType1) {
	if (m_itemListBox) {
		for (FrListBox::ITEM_LIST::iterator i = m_itemListBox->m_itemList.begin(); i != m_itemListBox->m_itemList.end(); ++i) {
			FrListItem* listItem = *i;
			if (listItem->selected) {
				if (bKeepType1) {
					if ((((IFF_ITEM_COMMON*)listItem->pData)->m_typeId & 0xFC000000) == 0x04000000) {
						continue;
					}
				}

				m_itemListBox->UnselectItem(listItem);
			}
		}
	}
}

// 0xa88a80 (the KR DeleteItemFromCartList)
std::list<FrListItem*>::iterator JpShopMain::DeleteItemFromCartList(unsigned long typeId) {
	for (FrListBox::ITEM_LIST::iterator i = m_cartList->m_itemList.begin(); i != m_cartList->m_itemList.end(); ++i) {
		IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)(*i)->pData;
		if (item->m_typeId == typeId) {
			return m_cartList->_DelItem(i);
		}
	}

	return m_cartList->m_itemList.end();
}

// 0xa88ad0 (the KR DeleteItemFromAuxCartList)
std::list<FrListItem*>::iterator JpShopMain::DeleteItemFromAuxCartList(unsigned long typeId) {
	FrListBox::ITEM_LIST::iterator end = m_auxCartList->m_itemList.end();
	for (FrListBox::ITEM_LIST::iterator i = m_auxCartList->m_itemList.begin(); i != end; ++i) {
		IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)(*i)->pData;
		if (item->m_typeId == typeId) {
			FrListBox::ITEM_LIST::iterator next = m_auxCartList->_DelItem(i);
			CheckAuxCartScroll();
			return next;
		}
	}

	return end;
}

// 0xa88b90 (the KR UnselectItemFromItemList)
bool JpShopMain::UnselectItemFromItemList(unsigned long typeId) {
	bool unselected = false;
	if (!m_itemListBox) {
		return unselected;
	}

	for (FrListBox::ITEM_LIST::iterator i = m_itemListBox->m_itemList.begin(); i != m_itemListBox->m_itemList.end(); ++i) {
		IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)(*i)->pData;
		if (item) {
			if (item->m_typeId == typeId) {
				m_itemListBox->UnselectItem(*i);
				unselected = true;
			}
		}
	}

	return unselected;
}

// 0xa88c00 (the KR IsItemInCart)
bool JpShopMain::IsItemInCart(FrListItem* listItem) {
	IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)listItem->pData;

	for (FrListBox::ITEM_LIST::iterator i = m_cartList->m_itemList.begin(); i != m_cartList->m_itemList.end(); ++i) {
		IFF_ITEM_COMMON* cartItem = (IFF_ITEM_COMMON*)(*i)->pData;
		if (cartItem == NULL) {
			continue;
		}

		if (item->m_typeId == cartItem->m_typeId) {
			return true;
		}
	}

	for (FrListBox::ITEM_LIST::iterator i = m_auxCartList->m_itemList.begin(); i != m_auxCartList->m_itemList.end(); ++i) {
		IFF_ITEM_COMMON* cartItem = (IFF_ITEM_COMMON*)(*i)->pData;
		if (cartItem == NULL) {
			continue;
		}

		if (item->m_typeId == cartItem->m_typeId) {
			return true;
		}
	}

	return false;
}

// 0xa88c80 (the KR SetMyItemSlot on the JP sub tabs, KR + 1)
void JpShopMain::SetMyItemSlot(void) {
	switch (m_subTab) {
	case 10:
		BuildMyItemSlot(true);
		m_itemSlotFrame1->SetVisible(true);
		m_itemSlotFrame2->SetVisible(true);
		m_itemSlot->SetVisible(true);
		break;
	case 9:
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

// 0xa88d00 (the KR SetAutoScroll)
void JpShopMain::SetAutoScroll(void) {
	FrListItem* selected = m_itemListBox->GetSelected();

	int index = 0;
	if (selected) {
		for (FrListBox::ITEM_LIST::iterator i = m_itemListBox->m_itemList.begin(); i != m_itemListBox->m_itemList.end(); ++i) {
			if (*i == selected) break;
			index++;
		}
	}

	m_itemListBox->ResetTopRow();
	m_itemListBox->GetScrollBar()->SetCurTopRow(index / 2);
}

// 0xa88dd0 (the KR one without the two empty-card pictures)
void JpShopMain::DeleteCardImages(void) {
	for (std::map<unsigned int, WOverlay*>::iterator it = m_cardImgs.begin(); it != m_cardImgs.end(); ++it) {
		if (g_resrcmng && it->second) {
			g_resrcmng->Release(it->second);
			it->second = 0;
		}
	}
}

// 0xa89640 (JP): the Refresh tooltip helper shows the JP text of
// "\xB8\xF0\xB5\xCE\xB1\xB8\xB8\xC5" (buy all) or "\xBF\xF8\xB7\xA1\xB4\xEB\xB7\xCE" (reset), option 1,
// style 0xb, in place of the KR CIconToolTip 0x18 / 0x19.
void JpShopMain::OnBuyHoverOn(intptr_t param) {
	FrButton* button = DYNAMIC_CAST(FrButton, param);
	if (button == m_buyAll) {
		if (RFTooltipHelper::Instance()) {
			RFTooltipHelper::Instance()->ShowTooltip(NULL, std::string(Jp_LookupText("\xB8\xF0\xB5\xCE\xB1\xB8\xB8\xC5")), 1, 0xb);
		}
	} else if (button == m_buyReset) {
		if (RFTooltipHelper::Instance()) {
			RFTooltipHelper::Instance()->ShowTooltip(NULL, std::string(Jp_LookupText("\xBF\xF8\xB7\xA1\xB4\xEB\xB7\xCE")), 1, 0xb);
		}
	}
}

// 0xa8a390 (the KR RestoreShoppersOldItem: the JP dresses the "LobbyAvatar"
// actor, message 0x18A with the tid list and the old part's tid and id,
// instead of the KR pet frame's SetPart)
void JpShopMain::RestoreShoppersOldItem(unsigned long typeId, CPartTidList& tidList) {
	for (std::list<oldItem_t>::iterator it = m_shoppersOldItems.begin(); it != m_shoppersOldItems.end();) {
		if (it->typeId == typeId) {
			MsgObject msg(0, 0x18A, (intptr_t)&tidList, (intptr_t)&it->tid, (intptr_t)&it->id, 0, 0);
			IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
			if (actor) {
				actor << msg;
			}
			it = m_shoppersOldItems.erase(it);
		} else {
			++it;
		}
	}
}

// 0xa8fa30 (the KR ChangeMainTab on the JP tabs: the side tabs belong to
// shop tab 1, each sub tab's index is its picture number (stext_<n>), there
// is no tab title, selected-item row or stat box; on the lobby desktop the
// bar selection and the "LobbyAvatar" / "LobbyCaddie" actors' visibility
// (messages 0x1CE / 0x1CF) follow the tab)
void JpShopMain::ChangeMainTab(mainTab_t tab, bool bForce) {
	if (bForce) {
		m_tabMainButton->Select(m_shopTab);
	} else if (m_shopTab == tab) {
		return;
	}

	m_shopTab = tab;
	if (tab != 1) {
		if (!m_sideTabBuy || !m_sideTabRental)
			return;

		if (m_sideTabBuy->GetStatus() != FrButton::PRESSED) {
			m_sideTabBuy->SetStatus(FrButton::PRESSED);
			m_sideTabRental->SetStatus(FrButton::NORMAL);

			m_miniBtns[2].enable = true;
		}
	}

	m_tabSubButton->ClearTab();

	const char* imgs[3] = { NULL, };
	int i;
	switch (m_shopTab) {
	case 0:
		m_buyAll->SetVisible(false);
		m_buyReset->SetVisible(false);

		SetSubTabImg(imgs, 12);
		m_tabSubButton->AddTab(imgs, 0, NULL);
		break;

	case 1:
		m_buyAll->SetVisible(true);
		m_buyAll->Enable(false);
		m_buyReset->SetVisible(true);
		m_buyReset->Enable(false);

		for (i = 1; i <= 6; i++) {
			SetSubTabImg(imgs, i);
			m_tabSubButton->AddTab(imgs, i, NULL);
		}

		SetSubTabImg(imgs, 27);
		m_tabSubButton->AddTab(imgs, 27, NULL);
		break;

	case 2:
		m_buyAll->SetVisible(false);
		m_buyReset->SetVisible(false);

		for (i = 7; i <= 10; i++) {
			SetSubTabImg(imgs, i);
			m_tabSubButton->AddTab(imgs, i, NULL);
		}

		if (IsLocalContent((localContentType_t)0x52)) {
			SetSubTabImg(imgs, 28);
			m_tabSubButton->AddTab(imgs, 28, NULL);
		}
		break;

	case 3:
		m_buyAll->SetVisible(false);
		m_buyReset->SetVisible(false);

		SetSubTabImg(imgs, 11);
		m_tabSubButton->AddTab(imgs, 11, NULL);

		SetSubTabImg(imgs, 39);
		m_tabSubButton->AddTab(imgs, 12, NULL);
		break;

	case 4:
		m_buyAll->SetVisible(false);
		m_buyReset->SetVisible(false);

		SetSubTabImg(imgs, 13);
		m_tabSubButton->AddTab(imgs, 13, NULL);
		break;

	case 5:
		m_buyAll->SetVisible(true);
		m_buyReset->SetVisible(true);

		for (i = 14; i <= 17; i++) {
			SetSubTabImg(imgs, i);
			m_tabSubButton->AddTab(imgs, i, NULL);
		}

		SetSubTabImg(imgs, 34);
		m_tabSubButton->AddTab(imgs, 34, NULL);

		SetSubTabImg(imgs, 35);
		m_tabSubButton->AddTab(imgs, 35, NULL);
		break;
	}

	m_itemDesc->SetVisible(false);
	m_itemDescBg->SetVisible(false);

	if (RFLobbyDesktop::s_instance) {
		int avatarMsg;
		int caddieMsg;
		if (m_shopTab == 0 || m_shopTab == 1) {
			JpLobbyDesktop_SetBarSelection(2);
			avatarMsg = 0x1CE;
			caddieMsg = 0x1CF;
		} else if (m_shopTab == 3) {
			JpLobbyDesktop_SetBarSelection(3);
			avatarMsg = 0x1CF;
			caddieMsg = 0x1CE;
		} else {
			JpLobbyDesktop_SetBarSelection(4);
			avatarMsg = 0x1CF;
			caddieMsg = 0x1CF;
		}

		MsgObject avatar(0, avatarMsg, 1, 0, 0, 0, 0);
		IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
		if (actor) {
			actor << avatar;
		}

		MsgObject caddie(0, caddieMsg, 1, 0, 0, 0, 0);
		actor = AfxGetTask()->GetActor("LobbyCaddie");
		if (actor) {
			actor << caddie;
		}

		JpLobbyDesktop_UpdateLobbyCamera();
	}

	m_tab490 = m_shopTab;
	TurnOffItemStatus();
	HideScrollButtons();

	ClearCartList(false, NULL);
}

// 0xa90520 (the KR InitalizeUpgradeCaddieInfo)
void JpShopMain::InitalizeUpgradeCaddieInfo(void) {
	m_upgradeCaddies.insert(std::map<unsigned long, unsigned long>::value_type(0x1C000001, 0x1C000010));
	m_upgradeCaddies.insert(std::map<unsigned long, unsigned long>::value_type(0x1C000002, 0x1C000011));
	m_upgradeCaddies.insert(std::map<unsigned long, unsigned long>::value_type(0x1C000003, 0x1C000012));
	m_upgradeCaddies.insert(std::map<unsigned long, unsigned long>::value_type(0x1C000004, 0x1C000013));
	m_upgradeCaddies.insert(std::map<unsigned long, unsigned long>::value_type(0x1C000005, 0x1C000014));
	m_upgradeCaddies.insert(std::map<unsigned long, unsigned long>::value_type(0x1C000006, 0x1C000015));
	m_upgradeCaddies.insert(std::map<unsigned long, unsigned long>::value_type(0x1C000007, 0x1C000016));
}

// 0xa90650 (the KR BuildVisibleCaddieList)
void JpShopMain::BuildVisibleCaddieList(void) {
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

// 0xa909d0 (the KR DrawCutinFrame on the JP sub tab 0x22, KR 33; the
// picture sits at (72, 66) of the task's page form, JP FrWnd rect +0x34)
void JpShopMain::DrawCutinFrame(void) {
	if (m_subTab == 0x22) {
		FrListItem* listItem = m_itemListBox->GetSelected();
		if (listItem == NULL) {
			return;
		}
		IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)listItem->pData;
		if (item == NULL) {
			return;
		}

		WOverlay* cutin = GetCutinImg(item->m_typeId);
		if (cutin == NULL) {
			return;
		}

		WRect rect;
		rect.x = 72.0f;
		rect.y = 66.0f;
		FrForm* pageForm = Task_GetWidgetForm(AfxGetTask());
		if (pageForm) {
			rect.x = pageForm->GetRect().x + rect.x;
			rect.y = pageForm->GetRect().y + rect.y;
		}
		rect.w = (float)cutin->GetWidth();
		rect.h = (float)cutin->GetHeight();
		cutin->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), rect, 0, 0xFFFFFFFF, 0.0f, 0);
	}
}

// 0xa90aa0 (the KR OnPetOwnerDraw on the JP tabs: shop tab 5 is the KR 4,
// the sub tabs are KR + 1; the JP draws nothing while the lobby desktop is
// busy, and the KR task's m_bongdariShop[2] is the shared doc's +0x11410)
void JpShopMain::OnPetOwnerDraw(intptr_t) {
	if (RFLobbyDesktop::s_instance && RFLobbyDesktop::s_instance->IsBusy()) {
		return;
	}

	if (m_shopTab == 5) {
		switch (m_subTab) {
		case 14:
		case 15:
		case 16:
			DrawSkinFrame();
			break;
		case 17:
		case 35:
			DrawSkinSlot();
			break;
		case 34:
			DrawCutinFrame();
			break;
		}
	} else {
		if (!m_anotherHand.IsWorking(0)) {
			if (!Jp_GameState_11410() && !WSingleton<CRankingInfo>::Instance()->GetDlg() && !USERINFODLG()) {
				m_anotherHand.Enter();
				if (m_petExhibition)
					m_petExhibition->Display(0.0f, 0.0f);
				m_anotherHand.Leave();
			}
		}
	}
}

// 0xa914c0 (the KR OnBuyAllUp; the message is the JP text of the Korean
// key 0xd45ad4, sent to the main actor)
void JpShopMain::OnBuyAllUp(void) {
	FrListBox::ITEM_LIST::iterator it;
	for (it = m_cartList->m_itemList.begin(); it != m_cartList->m_itemList.end(); ++it) {
		IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)(*it)->pData;

		if ((item->m_typeId >> 26) == 1) {
			if (HaveCharacter(item->m_typeId)) {
				it = m_cartList->_DelItem(it);
			}
			break;
		} else if ((item->m_typeId >> 26) == 9) {
			break;
		}
	}

	if (!HaveCharacter(m_standPetTypeId) && it == m_cartList->m_itemList.end()) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText("\xC7\xD8\xB4\xE7 \xC4\xB3\xB8\xAF\xC5\xCD\xB8\xA6 \xB0\xA1\xC1\xF6\xB0\xED \xC0\xD6\xC1\xF6 \xBE\xCA\xBD\xC0\xB4\xCF\xB4\xD9"), 0, 0, 0, 0));
		return;
	}

	Checkout();
	OnBuyHoverOff(0);
}

// 0xa915b0 (the KR OnSideTabRentalUp)
void JpShopMain::OnSideTabRentalUp(void) {
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

// 0xa8b690 (the KR ClearCartList; the JP dresses the "LobbyAvatar" actor
// instead of the pet frame: message 0x18A (the tid list, the part's type id)
// for a part, 0x18C (the player's part tid list, the hair colour) for the
// hair, whose character is the lobby carousel's, not the pet frame's)
void JpShopMain::ClearCartList(bool bKeepCharacters, CPartTidList* pTidList) {
	if (!m_cartList) {
		return;
	}

	FrListBox::ITEM_LIST::iterator i = m_cartList->m_itemList.begin();
	while (i != m_cartList->m_itemList.end()) {
		IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)(*i)->pData;

		if ((item->m_typeId >> 26) == 1) {
			if (bKeepCharacters) {
				++i;
				continue;
			}
			i = m_cartList->_DelItem(i);
			continue;
		} else if ((item->m_typeId >> 26) == 2 && pTidList) {
			if (pTidList->IsEquipped(item->m_typeId, 0) && !m_anotherHand.IsWorking(0)) {
				MsgObject msg(0, 0x18A, (intptr_t)pTidList, (intptr_t)&item->m_typeId, 0, 0, 0);
				IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
				if (actor) {
					actor << msg;
				}

				RestoreShoppersOldItem(item->m_typeId, *pTidList);
			}
		} else if ((item->m_typeId >> 26) == 0xF && pTidList && !m_anotherHand.IsWorking(0)) {
			unsigned long tidChar = 0;
			if (RFLobbyDesktop::s_instance) {
				tidChar = JpLobbyDesktop_GetSelectedCharacterTid();
			}

			const std::map<unsigned int, sCharacterInfo>& characters = WSingleton<CSharedDoc>::Instance()->m_characterMap;
			std::map<unsigned int, sCharacterInfo>::const_iterator it;
			for (it = characters.begin(); it != characters.end(); ++it) {
				if (it->second.typeId == tidChar) {
					break;
				}
			}

			if (it == characters.end()) {
				m_hairColor = 0;
			} else {
				m_hairColor = it->second.hairColor;

				MsgObject msg(0, 0x18C, (intptr_t)&WSingleton<CSharedDoc>::Instance()->GetMyPartTidList(), (intptr_t)&m_hairColor, 0, 0, 0);
				IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
				if (actor) {
					actor << msg;
				}
			}
		}

		i = m_cartList->_DelItem(i);
	}

	FrListBox::ITEM_LIST::iterator j = m_auxCartList->m_itemList.begin();
	while (j != m_auxCartList->m_itemList.end()) {
		IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)(*j)->pData;
		if ((item->m_typeId & 0xFC000000) == 0x08000000 && pTidList
			&& pTidList->IsEquipped(item->m_typeId, 0) && !m_anotherHand.IsWorking(0)) {
			MsgObject msg(0, 0x18A, (intptr_t)pTidList, (intptr_t)&item->m_typeId, 0, 0, 0);
			IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
			if (actor) {
				actor << msg;
			}

			RestoreShoppersOldItem(item->m_typeId, *pTidList);
		}

		j = m_auxCartList->_DelItem(j);
	}

	CheckAuxCartScroll();
}

// 0xa8c1d0 (the KR TurnOnItemStatus, with a null test)
void JpShopMain::TurnOnItemStatus(void) {
	if (m_statBox) {
		m_statBox->SetVisible(true);
	}
	SetLevelBar();
}

// 0xa8c200 (the KR TurnOffItemStatus, with a null test)
void JpShopMain::TurnOffItemStatus(void) {
	if (m_statBox) {
		m_statBox->SetVisible(false);
	}
	SetLevelBar();
}

// 0xa8c230 (the KR GetCutinImg)
WOverlay* JpShopMain::GetCutinImg(unsigned long typeId) {
	WOverlay* overlay;
	std::map<unsigned int, WOverlay*>::iterator it = m_cutinImgs.find(typeId);
	if (it == m_cutinImgs.end()) {
		IFF_STRUCT::sSkin* skin = ItemManager()->FindSkin(typeId);

		if (skin) {
			char path[64];
			sprintf(path, "%s.jpg", skin->m_cutinImage);
			overlay = g_resrcmng->GetOverlay(path, 0);
			if (overlay) {
				m_cutinImgs.insert(std::map<unsigned int, WOverlay*>::value_type(skin->m_typeId, overlay));
			}
		} else {
			return NULL;
		}
	} else {
		overlay = it->second;
	}

	return overlay;
}

// 0xa91640 (the KR destructor, plus the JP avatar-chat steps)
JpShopMain::~JpShopMain(void) {
	DeleteCardImages();
	DeleteCutinImages();

	HidePet(true);

	g_resrcmng->UnlockAllByThread(m_anotherHand.GetThreadID());
	m_anotherHand.BreakWorkingThread();

	CTask* task = AfxGetTask();
	if (task) {
		task->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
	}
	AfxPostMsg(NULL, "AvatarChat", 0x58, 0, 0, 0, 0);

	if (RFLobbyDesktop::s_mode == 1) {
		// Not written yet (REQUEST): the JP first clears the desktop's +0x184
		// (RFLobbyDesktop::s_instance, a member RFLobbyDesktop.h does not
		// model). Then the player's avatar drops state 4 (ntAvatar::SetState
		// 0x7a4820).
		ntAvatarManager* manager = WSingleton<ntAvatarManager>::Instance();
		if (manager) {
			ntAvatar* myAvatar = manager->m_pMyAvatar;
			if (myAvatar) {
				myAvatar->SubState(4);
			}
		}
	}

	if (g_resrcmng && m_titleFont) {
		g_resrcmng->Release(m_titleFont);
		m_titleFont = NULL;
	}
}

// 0xa91950 (FrForm [9]; the KR OnProcess without the base call: the pet
// keeps its idle motion on the tabs 2 and 5 (KR 1 and 4), the side panels
// slide on tab 5 only, the buy buttons follow the carts on tabs 1 and 5
// (KR 0 and 4), and the tooltips 0x1b / 0x1c (KR 0x16 / 0x17) are cleared)
void JpShopMain::OnProc(const float dt) {
	if (m_anotherHand.IsWorking(0)) {
		Sleep(10);
	} else {
		m_anotherHand.Enter();

		bool flag;
		switch (m_shopTab) {
		case 2:
		case 5:
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

	// 0xa9db40 (unplaced in the JP): the KR FrCheckoutDlg::Process.
	if (m_buyDlg) {
		((FrCheckoutDlg*)m_buyDlg)->Process();
	}

	if (m_shopTab == 5) {
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

		if (m_shopTab == 1 || m_shopTab == 5) {
			m_buyAll->Enable(!m_cartList->m_itemList.empty() || !m_auxCartList->m_itemList.empty());
			m_buyReset->Enable(!m_cartList->m_itemList.empty() || !m_auxCartList->m_itemList.empty());
		}
	}

	int tip = CIconToolTip::GetInstance()->GetCurTipIndex();
	if (tip == 0x1b || tip == 0x1c) {
		CIconToolTip::GetInstance()->ClearToolTip();
	}

	m_blinkTime += dt;
	m_blinkValue = (float)fabs(sin(m_blinkTime * 4.0f)) * 5.0f;
}

// 0xa91ec0 (the KR constructor and OnPreLoadInit; the JP moved
// BuildVisibleCaddieList to OnInit and adds the number font)
JpShopMain::JpShopMain(void) {
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));

	m_cardTypeId = 0;
	m_cutinImgs.clear();

	InitShopControls();
	InitShopDialogs();

	m_caddieTypeId = 0;
	m_standPetTypeId = 0;
	m_mascotTypeId = 0x40000000;
	m_clubsetTypeId = 0x10000000;
	m_aztecTypeId = 0x14000000;
	m_charLevels = NULL;
	m_charParts = NULL;
	m_charCards = NULL;
	m_hairColor = 0;
	m_rbuttonPartTypeIds.clear();
	m_statTab = STAT_TAB_PART;
	m_shopTab = 0;
	m_subTab = SUB_TAB_NONE;
	m_tab490 = 0;
	m_rect6b0 = data_f145f8;

	m_sortOptionCombo = NULL;
	m_searchGoodsButton = NULL;
	m_itemSearchBtn = NULL;

	InitLevelFields();
	InitalizeUpgradeCaddieInfo();

	m_index6a8 = -1;
	m_selectItemMarkTypeId = 0;

	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));
	info.filename = s_shopNumberFontFiles;
	info.numPages = 1;
	info.fontw = 16;
	info.fonth = 16;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = const_cast<char*>("1234567890ym%-.?/:,");
	m_titleFont = g_resrcmng->GetTitleFont();
	m_titleFont->Create(&info);
}

// 0xa923c0 (the KR BuildDisplayStand on the JP tab numbers: shop tab 1 is
// the KR 0, the sub tabs are the KR ones + 1 from 1 on; 1 and 12 are
// JP-only lists)
void JpShopMain::BuildDisplayStand(subTab_t tab) {
	if (m_shopTab == 1) {
		SetSideTabsVisible(true);
		if (IsRentalSideTab()) {
			BuildDisplayRental(tab);
			return;
		}
	} else {
		SetSideTabsVisible(false);
	}

	m_itemListBox->ClearItem();

	switch (tab) {
	case 0:
		Build_Package((subTab_t)0, 0);
		break;
	case 1:
		Build_CharacterItems((subTab_t)1, 0);
		break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 0x1B:
		Build_Parts(tab, 0);
		break;
	case 6:
		Build_Hair((subTab_t)6, 0);
		break;
	case 7:
		Build_Clubset((subTab_t)7, 0);
		break;
	case 8:
		Build_Aztec((subTab_t)8, 0);
		break;
	case 9:
		Build_Equip((subTab_t)9, 0);
		break;
	case 10:
		Build_NoEquip((subTab_t)10, 0);
		break;
	case 0x1C:
		Build_Card((subTab_t)0x1C, 0);
		break;
	case 0x1D:
	case 0x1E:
	case 0x1F:
		Build_Interior(tab, 0);
		break;
	case 0x20:
		Build_Furniture((subTab_t)0x20, 0);
		break;
	case 0xE:
	case 0xF:
	case 0x10:
	case 0x11:
	case 0x23:
		Build_Decorate(tab, 0);
		break;
	case 0x22:
		Build_Cutin((subTab_t)0x22, 0);
		break;
	case 0xB:
		Build_Caddie((subTab_t)0xB, 0);
		break;
	case 0xC:
		Build_CaddieItems((subTab_t)0xC, 0);
		break;
	case 0xD:
		Build_Mascot((subTab_t)0xD, 0);
		break;
	}

	int count = m_itemListBox->GetCurrentItemSize(true);
	for (int i = count; i < 8; i++) {
		m_itemListBox->AddItem(NULL);
	}
}

// 0xa92590 (the KR ChangeSubTab on the JP sub tabs, KR + 1 from 1 on;
// the JP has no m_selItemExpanded and no "selected club" caption, and the
// local content ids are the KR ones + 1)
void JpShopMain::ChangeSubTab(subTab_t tab, bool bForce) {
	if (!bForce && m_subTab == tab) {
		return;
	}

	// Whether the shopper's character has to be loaded again.
	bool reload = false;
	if (((m_subTab <= 6 || m_subTab == 17) && (tab == 14 || tab == 15 || tab == 16))
		|| (m_subTab == 34 && (tab == 14 || tab == 15 || tab == 16))
		|| ((m_subTab == 14 || m_subTab == 15 || m_subTab == 16 || m_subTab == 34) && m_shopTab == 0)
		|| (m_shopTab == 1 && m_subTab == 17 && tab == 1)) {
		reload = true;
	}

	m_subTab = tab;
	m_itemListBox->SetMultiSelect(tab == 5 || tab == 27);

	BuildDisplayStand(m_subTab);

	m_tabSubButton->Select(m_subTab);

	switch (m_subTab) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 6:
	case 27:
		TurnOnItemStatus();
		if (reload) {
			SafeLoadPet(&m_standPetTypeId, reload);
		}
		break;

	case 5:
		TurnOnItemStatus();
		if (reload) {
			SafeLoadPet(&m_standPetTypeId, reload);
		}
		break;

	case 7:
		TurnOnItemStatus();
		SafeLoadPet(&m_clubsetTypeId, false);
		break;

	case 8:
		TurnOffItemStatus();
		SafeLoadPet(&m_aztecTypeId, false);
		break;

	case 28:
		if (IsLocalContent((localContentType_t)0x52)) {
			TurnOffItemStatus();
			HidePet(true);
		}
		break;

	case 29:
	case 30:
	case 31:
	case 32:
	case 33:
		if (IsLocalContent((localContentType_t)0x57)) {
			if (IsLocalContent((localContentType_t)0x62)) {
				TurnOffItemStatus();
				HidePet(true);
			}
		}
		break;

	case 11:
		TurnOnItemStatus();
		HidePet(true);
		break;

	case 13:
		TurnOffItemStatus();
		break;

	case 14:
	case 17:
		TurnOffItemStatus();
		SafeLoadPet(&m_standPetTypeId, reload);
		break;

	case 15:
	case 35:
		TurnOffItemStatus();
		SafeLoadPet(&m_standPetTypeId, reload);
		break;

	case 16:
	case 34:
		TurnOffItemStatus();
		SafeLoadPet(&m_standPetTypeId, reload);
		break;

	case 9:
	case 10:
		TurnOffItemStatus();
		HidePet(true);
		break;
	}

	SetMyItemSlot();
	SetItemDescRect();
	SetPetAreaRect();
	SetAutoScroll();
}

// 0xa92810 (the KR ChangeCharacter on the JP sub tabs; no selected-name
// caption, no prev/next buttons; the shop tab 5 (KR 4) also shows the
// skins and checks the aux cart's arrows)
void JpShopMain::ChangeCharacter(unsigned long typeId) {
	typedef std::map<unsigned int, sCharacterInfo> CharacterMap;
	CharacterMap::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++it) {
		if (it->second.typeId == typeId) {
			break;
		}
	}

	m_buyAll->Enable(false);
	m_buyReset->Enable(false);

	if (m_standPetTypeId != typeId) {
		m_standPetTypeId = typeId;
		m_cartList->ClearItem();
		m_auxCartList->ClearItem();
	}

	if (m_subTab == 1 || m_subTab == 2 || m_subTab == 3 || m_subTab == 4 || m_subTab == 5 || m_subTab == 6 || m_subTab == 0x1B) {
		bool bUccWinner = false;

		if (m_subTab == 0x1B && typeId == UccManager()->GetWinnerCharacterTypeId()) {
			CTask* task = AfxGetPreservedTask();
			if (task && task->IsExactKindOf(&CLobbyTask::m_RTTI)) {
				bUccWinner = true;
			}
		}

		if (it == WSingleton<CSharedDoc>::Instance()->m_characterMap.end()) {
			WSingleton<CSharedDoc>::Instance()->BuildMyPartTidListDefault(typeId);
			if (!bUccWinner) {
				ChangeSubTab((subTab_t)1, false);
			}
			m_tabSubButton->Select(m_subTab);
			m_hairColor = 0;
		} else {
			Jp_GameState_SetShownCharacter(&it->second);

			m_charParts = it->second.partTypeIds;
			m_charCards = it->second.auxParts.tids;
			m_charLevels = (const char*)&it->second.unknown6A;
			m_hairColor = it->second.hairColor;
			if (m_subTab == 1) {
				ChangeSubTab((subTab_t)2, false);
				m_tabSubButton->Select(m_subTab);
			}
		}

		ClearCartList(true, &WSingleton<CSharedDoc>::Instance()->GetMyPartTidList());
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_standPetTypeId, m_standPetTypeId == typeId);
		TurnOnItemStatus();
	} else if (m_subTab == 0) {
		if (it == WSingleton<CSharedDoc>::Instance()->m_characterMap.end()) {
			WSingleton<CSharedDoc>::Instance()->BuildMyPartTidListDefault(typeId);
			m_hairColor = 0;
		} else {
			Jp_GameState_SetShownCharacter(&it->second);

			m_charParts = it->second.partTypeIds;
			m_charCards = it->second.auxParts.tids;
			m_charLevels = (const char*)&it->second.unknown6A;
			m_hairColor = it->second.hairColor;
		}

		ChangeSubTab((subTab_t)0, false);
		m_tabSubButton->Select(m_subTab);

		ClearCartList(true, &WSingleton<CSharedDoc>::Instance()->GetMyPartTidList());
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_standPetTypeId, m_standPetTypeId == typeId);
		TurnOnItemStatus();
	} else if (m_shopTab == 5) {
		ChangeSkin(m_standPetTypeId);
		CheckAuxCartScroll();
		Jp_GameState_SetShownCharacter(&it->second);
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_standPetTypeId, false);
	}

	SetAutoScroll();
}

// 0xa92b20 (the KR one without the expanded-list walk)
void JpShopMain::ChangeCaddie(unsigned long typeId) {
	m_caddieTypeId = typeId;

	if (m_subTab == SUB_TAB_CADDIE) {
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_caddieTypeId, false);
		TurnOnItemStatus();
	}
}

// 0xa92b60
void JpShopMain::ChangeMascot(unsigned long typeId) {
	m_mascotTypeId = typeId;

	if (m_subTab == SUB_TAB_MASCOT) {
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_mascotTypeId, false);
		TurnOffItemStatus();
	}
}

// 0xa92ba0
void JpShopMain::ChangeClubset(unsigned long typeId) {
	m_clubsetTypeId = typeId;

	if (m_subTab == SUB_TAB_CLUBSET) {
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_clubsetTypeId, false);
		TurnOnItemStatus();
	}
}

// 0xa92be0
void JpShopMain::ChangeAztec(unsigned long typeId) {
	m_aztecTypeId = typeId;

	if (m_subTab == SUB_TAB_AZTEC) {
		BuildDisplayStand(m_subTab);
		SafeLoadPet(&m_aztecTypeId, false);
		TurnOffItemStatus();
	}
}

// 0xa92d70 (__fastcall in the JP: CreateForm<CShopMain> 0xa92ca0, which
// also keeps the form in the thread-local +0xb280 slot)
FrWnd* JpShopMain::Create(const char* name, FrWndManager* pManager) {
	return CreateForm<JpShopMain>(pManager, NULL, name, NULL);
}

// 0xa92e20 (the KR OnTabMainButtonUp with the JP tab order: 1 is the
// KR 5 tab's sub tab 1; JP sub tabs are the KR ones + 1 from the club set)
void JpShopMain::OnTabMainButtonUp(void) {
	TButtonItem* item = m_tabMainButton->GetSelected();
	if (item) {
		if ((int)item->index != m_shopTab) {
			ChangeMainTab((mainTab_t)item->index, false);

			switch (item->index) {
			case 0:
				ChangeSubTab((subTab_t)0, false);
				ChangeCharacter(m_standPetTypeId);
				break;
			case 1:
				ChangeSubTab((subTab_t)1, false);
				ChangeCharacter(m_standPetTypeId);
				break;
			case 2:
				ChangeSubTab(SUB_TAB_CLUBSET, false);
				ChangeClubset(m_clubsetTypeId);
				break;
			case 3:
				ChangeSubTab(SUB_TAB_CADDIE, false);
				ChangeCaddie(m_caddieTypeId);
				break;
			case 4:
				ChangeSubTab(SUB_TAB_MASCOT, false);
				ChangeMascot(m_mascotTypeId);
				break;
			case 5:
				ChangeSubTab((subTab_t)14, false);
				ChangeSkin(m_standPetTypeId);
				break;
			}
		}
	}
}

// 0xa92f30
void JpShopMain::OnTabSubButtonUp(void) {
	TButtonItem* selected = m_tabSubButton->GetSelected();
	if (selected) {
		ChangeSubTab((subTab_t)selected->index, false);
	}
}

// 0xa92fa0 (the KR OnBuyResetUp on the JP tabs: shop tab 1 (KR 0) sends
// the "LobbyAvatar" actor 0x188 and 0x187 (the shown character's type id)
// in place of the KR pet reload; shop tab 5 is the KR 4)
void JpShopMain::OnBuyResetUp(void) {
	g_audio->PlaySfx("cloth_off", 0, 0, 0, 0, 0.5f, 200.0f);

	if (m_shopTab == 1) {
		std::map<unsigned int, sCharacterInfo>::iterator end = WSingleton<CSharedDoc>::Instance()->m_characterMap.end();
		std::map<unsigned int, sCharacterInfo>::iterator it;
		for (it = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin(); it != end; ++it) {
			if (it->second.typeId == m_standPetTypeId) {
				break;
			}
		}

		if (m_subTab == 1 || m_subTab == 2 || m_subTab == 3 || m_subTab == 4 || m_subTab == 5 || m_subTab == 6 || m_subTab == 0x1B) {
			if (it != end) {
				Jp_GameState_SetShownCharacter(&it->second);

				m_charParts = it->second.partTypeIds;
				m_charCards = it->second.auxParts.tids;
				m_charLevels = (const char*)&it->second.unknown6A;
				m_hairColor = it->second.hairColor;
			} else {
				WSingleton<CSharedDoc>::Instance()->BuildMyPartTidListDefault(m_standPetTypeId);
			}

			ClearCartList(true, &WSingleton<CSharedDoc>::Instance()->GetMyPartTidList());
			BuildDisplayStand(m_subTab);

			MsgObject reset(0, 0x188, 0, 0, 0, 0, 0);
			IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
			if (actor) {
				actor << reset;
			}

			MsgObject show(0, 0x187, (intptr_t)&m_standPetTypeId, 1, 0, 0, 0);
			actor = AfxGetTask()->GetActor("LobbyAvatar");
			if (actor) {
				actor << show;
			}

			TurnOnItemStatus();
		}
	} else if (m_shopTab == 5) {
		ResetItemList(false);
		ClearCartList(false, NULL);

		memcpy(m_equipSkin, WSingleton<CSharedDoc>::Instance()->m_equipSkin, sizeof(m_equipSkin));
		for (int i = 0; i < 6; i++) {
			if (m_equipSkin[i]) {
				IFF_STRUCT::sSkin* skin = ItemManager()->FindSkin(m_equipSkin[i]);
				if (skin && !(m_equipSkin[i] & 0x3C00000)) {
					m_sideDir[0] = skin->m_sideSlideMode[0];
					m_sidePos[0] = 0;
					m_sidePos[1] = 0;
					m_sideDir[1] = skin->m_sideSlideMode[1];
				}
			}
		}
	}

	m_buyAll->Enable(false);
	m_buyReset->Enable(false);

	OnBuyHoverOff(0);
}

// 0xa93270 (the KR OnSideTabBuyUp; Work is 0x98afb0)
void JpShopMain::OnSideTabBuyUp(void) {
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

// 0xa93300 (FrForm [32]; the KR OnShopMainFinish without the title area,
// with the JP tutorial message 0xCE and the avatar-chat step)
bool JpShopMain::OnInit(void) {
	BuildVisibleCaddieList();

	m_petArea = m_pet;

	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	unsigned long caddieTypeId = doc->m_myCaddieId == 0 ? 0 : doc->m_caddieMap[doc->m_myCaddieId].typeId;
	unsigned long mascotTypeId = doc->m_myMascotId == 0 ? 0x40000000 : doc->m_myMascotMap[doc->m_myMascotId].typeId;

	if (m_standPetTypeId) {
		ChangeCharacter(m_standPetTypeId);
	} else {
		ChangeCharacter(doc->m_characterMap[doc->m_myCharacterId].typeId);
	}

	ChangeCaddie(caddieTypeId);
	ChangeClubset(doc->m_myItemMap[doc->m_myClubId].m_typeId);
	ChangeAztec(doc->m_myBallId);
	ChangeMascot(mascotTypeId);
	memcpy(m_equipSkin, doc->m_equipSkin, sizeof(m_equipSkin));

	ChangeMainTab((mainTab_t)m_shopTab, true);
	ChangeSubTab(m_subTab, true);
	ChangeStatTab((statTab_t)(m_shopTab > 3));

	memcpy(m_equipSkin, WSingleton<CSharedDoc>::Instance()->m_equipSkin, sizeof(m_equipSkin));
	for (int i = 0; i < 6; i++) {
		if (m_equipSkin[i]) {
			IFF_STRUCT::sSkin* skin = ItemManager()->FindSkin(m_equipSkin[i]);
			if (skin && !(m_equipSkin[i] & 0x3C00000)) {
				m_sideDir[0] = skin->m_sideSlideMode[0];
				m_sidePos[0] = 0;
				m_sidePos[1] = 0;
				m_sideDir[1] = skin->m_sideSlideMode[1];
			}
		}
	}

	if (Jp_GameState_10d80()) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xCE, 1, 0, 0, 0, 0));
	}

	JpFrWnd_RaiseFlagF1aac8();

	if (RFLobbyDesktop::s_mode == 1) {
		// The player's avatar gains state 4 (ntAvatar::SetState 0x7a4820).
		ntAvatarManager* manager = WSingleton<ntAvatarManager>::Instance();
		if (manager) {
			ntAvatar* myAvatar = manager->m_pMyAvatar;
			if (myAvatar) {
				myAvatar->AddState(4);
			}
		}
	}

	return FrForm::OnInit();
}

// 0x63b960 (a JP CSharedDoc member over its +0x11408 list; name ours): the
// item a lobby link asks the shop to show (a part: the shop's character
// tab, kind 6: the caddie tab). Not decompiled yet: 0, the JP's answer
// without that list (G7 placeholder).
static unsigned long Jp_SharedDoc_GetShopLinkItem(void);

static unsigned long Jp_SharedDoc_GetShopLinkItem(void) {
	return 0;
}

// 0xa971f0 (FrTaskForm [40]; the KR HandleMsg on the JP message ids and
// tabs). First pass: the cases below are the JP ones; the JP also handles
// 3 (the hint window), 0x5a, 0x5b (a mascot, then CSharedDoc 0x63de40),
// 0x75 (CSharedDoc 0x62fd70 / 0x62b750), 0xd7..0xe2, 0xec, 0xee..0xf5 (the
// CSharedDoc item, caddie and card containers at their JP offsets),
// 0x105, 0x27b, 0x294, 0x295, 0x2bc, 0x2cb, 0x2d3 (0xa96f00), 0x2d6 and
// 0x2d7, which are not decompiled yet and return false here (as
// FrTaskForm::HandleMsg does).
bool JpShopMain::HandleMsg(const MsgObject& msg) {
	switch (msg.m_msg) {
	case 0:
		OpenLayout((const char*)msg.m_param1);
		return true;

	case 0x48: {
		const sCharacterInfo* character = (const sCharacterInfo*)msg.m_param2;
		CTask* task = WSingleton<CTaskManager>::Instance()->GetPreservedTask();
		if (task && task->GetActor("AvatarChat")) {
			task->GetActor("AvatarChat") << MsgObject(0, 0x48, msg.m_param1, msg.m_param2, 0, 0, 0);
		} else {
			WSingleton<CSharedDoc>::Instance()->m_myCharacterId = character->id;
		}
		ChangeCharacter(character->typeId);
		return true;
	}

	case 0x49: {
		const sCaddieInfo* caddie = (const sCaddieInfo*)msg.m_param2;
		if (caddie->typeId) {
			ChangeCaddie(caddie->typeId);
		}
		WSingleton<CSharedDoc>::Instance()->m_myCaddieId = caddie->id;
		return true;
	}

	case 0x4A: {
		const sItemInfo* clubSet = (const sItemInfo*)msg.m_param2;
		ChangeClubset(clubSet->m_typeId);
		WSingleton<CSharedDoc>::Instance()->m_myClubId = clubSet->m_serial;
		return true;
	}

	case 0x4B:
		ChangeAztec(msg.m_param2);
		WSingleton<CSharedDoc>::Instance()->m_myBallId = msg.m_param2;
		return true;

	case 0x78: {
		if (m_shopTab != msg.m_param1) {
			return true;
		}

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

		if (it == WSingleton<CSharedDoc>::Instance()->m_characterMap.end()) {
			return true;
		}

		IFF_STRUCT::sChar* character = ItemManager()->FindChar(m_standPetTypeId);
		if (!character) {
			return true;
		}

		sCharacterInfo info = it->second;
		bool changed = false;

		for (int i = 0; i < character->m_numParts + character->m_numExtraParts; i++) {
			if (info.partTypeIds[i] == m_charParts[i]) {
				continue;
			}

			IFF_STRUCT::sPart* part = ItemManager()->FindPart(m_charParts[i]);
			if (part) {
				unsigned char level = WSingleton<CSharedDoc>::Instance()->m_myLevel;

				if ((part->m_level >> 7) == 0 && (part->m_level & 0x7F) > level) {
					continue;
				}
				if ((part->m_level >> 7) == 1 && (part->m_level & 0x7F) < level) {
					continue;
				}
			}

			// 0x7e9580: the KR IsUccClothes on the JP part record.
			if (JpItem_IsPartClass7to9(m_charParts[i])) {
				continue;
			}

			info.partTypeIds[i] = m_charParts[i];
			info.partIds[i] = m_charPartIds[i];
			changed = true;
		}

		for (int i = character->m_numParts + character->m_numExtraParts; i < 24; i++) {
			info.partTypeIds[i] = 0;
		}

		for (int i = 0; i < 5; i++) {
			if (info.auxParts.tids[i] == m_charCards[i]) {
				continue;
			}

			IFF_STRUCT::sAuxPart* auxPart = ItemManager()->FindAuxPart(m_charCards[i]);
			if (auxPart && (auxPart->m_level & 0x7F) > WSingleton<CSharedDoc>::Instance()->m_myLevel) {
				continue;
			}

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
		return true;
	}

	case 0xD4:
		return true;

	case 0xE5:
		*(int*)msg.m_param1 = m_shopTab;
		*(subTab_t*)msg.m_param2 = m_subTab;
		return true;

	case 0xE6:
		if (msg.m_param1 < 6) {
			m_shopTab = msg.m_param1;
			if (msg.m_param2 < 0x25) {
				m_subTab = (subTab_t)msg.m_param2;
			}
			if (m_shopTab == 1 && m_subTab > 6) {
				m_subTab = (subTab_t)1;
			}
		} else {
			m_shopTab = 0;
			m_subTab = SUB_TAB_NONE;
		}
		return true;

	case 0xE7:
		SetMyItemSlot();
		return true;

	case 0xE8:
		BuildDisplayStand(m_subTab);
		return true;

	case 0xE9:
		// The JP server's shop result codes: the limited item sold out, its
		// buy limit reached, check the price again (Korean keys of the JP
		// text table), else the code itself (message 0x27).
		switch (msg.m_param1) {
		case 0x2F4D77:
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_GetTableText(0x13886), 0, 0, 0, 0));
			break;
		case 0x301881:
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText("\xb1\xdd\xc0\xcf(\xc8\xb8) \xc1\xd8\xba\xf1\xb5\xc8 \xbc\xf6\xb7\xae\xc0\xcc \xb8\xf0\xb5\xce \xb8\xc5\xc1\xf8\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9."), 0, 0, 0, 0));
			break;
		case 0x301882:
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText("\xb1\xdd\xc0\xcf(\xc8\xb8) \xb1\xb8\xb8\xc5\xc7\xd2 \xbc\xf6 \xc0\xd6\xb4\xc2 \xc7\xd1\xb5\xb5\xb0\xa1 \xb8\xf0\xb5\xce \xbc\xd2\xc1\xf8\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9."), 0, 0, 0, 0));
			break;
		case 0x301883:
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText("\xb0\xa1\xb0\xdd\xc0\xbb \xb4\xd9\xbd\xc3 \xc8\xae\xc0\xce\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4."), 0, 0, 0, 0));
			break;
		default:
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x27, msg.m_param1, 0, 0, 0, 0));
			break;
		}
		RequestShopEnter();
		return true;

	case 0xEA:
		RequestShopEnter();
		return true;

	case 0xEB:
		if (msg.m_param1 == 3) {
			m_shopTab = 1;
			m_subTab = (subTab_t)0x1B;
			ChangeMainTab((mainTab_t)1, true);
			ChangeSubTab((subTab_t)0x1B, true);

			unsigned long typeId = msg.m_param2;
			if (!typeId) {
				typeId = m_standPetTypeId;
			}

			ChangeCharacter(typeId);
		} else if (msg.m_param1 == 4) {
			if (IsLocalContent((localContentType_t)0x52)) {
				m_shopTab = 2;
				m_subTab = (subTab_t)0x1C;
				ChangeMainTab((mainTab_t)2, true);
				ChangeSubTab((subTab_t)0x1C, true);
				HidePet(true);
			}
		} else if (msg.m_param1 == 5) {
			unsigned long typeId = Jp_SharedDoc_GetShopLinkItem();
			if (typeId) {
				unsigned long kind = typeId >> 26;
				if (kind == 2) {
					m_shopTab = 1;
					m_subTab = (subTab_t)2;
				} else if (kind == 6) {
					m_shopTab = 2;
					m_subTab = (subTab_t)10;
				} else {
					m_subTab = (subTab_t)2;
					m_shopTab = 1;
					ChangeCharacter(0x4000000);
				}
				if (kind == 2) {
					m_standPetTypeId = ((typeId >> 18) & 0xFF) | 0x4000000;
				}
			}
		}
		return true;

	case 0x27C:
		return true;

	default:
		return false;
	}
}

// ---------------------------------------------------------------------------
// The item list's row draw (OnItemListOwnerDraw 0xa936a0) and its helpers.

// The JP header dword +0x9c (the KR record's u32 at 0x6c, not named in
// IFF_ITEM_COMMON; JpShopMainBuild.cpp reads it too): 2 lists an item
// without a shop flag or sale period, and it shows the surprise-sale mark
// while the item is in its sale period.
static uint32_t ShopListKind(const IFF_ITEM_COMMON& item) {
	uint32_t kind;
	memcpy(&kind, reinterpret_cast<const unsigned char*>(&item) + 0x6c, sizeof(kind));
	return kind;
}

static bool ShopIsInSalePeriod(unsigned long typeId) {
	return ItemManager()->IsInSalePeriod(typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime());
}

// 0x63f280 (CSharedDoc, JP only; name ours): the value of the JP CSharedDoc
// map<unsigned long, unsigned long> at +0x113e0 for a type id (0..3, 0 when
// absent; filled by a JP shop packet not identified yet). The port's
// CSharedDoc has no such map: a G7 placeholder returning 0 (no limited-type
// mark; the buy and gift buttons stay, they are hidden on 3).
static unsigned long Jp_SharedDoc_GetItemLimitKind(unsigned long typeId);

static unsigned long Jp_SharedDoc_GetItemLimitKind(unsigned long) {
	return 0;
}

// The 35 type ids of data_d45760 (JP only).
static const unsigned long s_shopSpecialItems[35] = {
	0x8006055, 0x800a035, 0x8044085, 0x8048025, 0x80440b3, 0x8084044, 0x808a025,
	0x80c408b, 0x80ca023, 0x80c40b2, 0x810404f, 0x8108030, 0x8144084, 0x814a02c,
	0x81440bf, 0x8184087, 0x8188020, 0x81840c7, 0x81c404b, 0x81ca01f, 0x820404a,
	0x820a00e, 0x8204091, 0x8244064, 0x824a004, 0x824408f, 0x8284026, 0x8284027,
	0x828a012, 0x830401b, 0x830401e, 0x830a00c, 0x838400e, 0x8384011, 0x838a006
};

// 0xa82ce0 (JP only; name ours): the type id is one of data_d45760's parts,
// which the list shows with their UCC picture and name and without a gift
// button.
static bool IsShopSpecialItem(unsigned long typeId) {
	for (unsigned int i = 0; i < sizeof(s_shopSpecialItems) / sizeof(s_shopSpecialItems[0]); i++) {
		if (s_shopSpecialItems[i] == typeId) {
			return true;
		}
	}
	return false;
}

// 0xa826d0
bool JpShopMain::IsAboveShopLevel(const IFF_ITEM_COMMON* item) {
	if (item) {
		if ((item->m_level & 0x80) == 0 && (unsigned int)(item->m_level & 0x7F) >= (unsigned int)m_levelPlusOne) {
			return true;
		}
	}
	return false;
}

// 0xa82700
bool JpShopMain::IsBelowShopLevel(const IFF_ITEM_COMMON* item) {
	if (item) {
		if ((item->m_level & 0x80) != 0 && (unsigned int)(item->m_level & 0x7F) < (unsigned int)m_level) {
			return true;
		}
	}
	return false;
}

// 0xa85fe0 (the KR DrawItemPrice with the JP sale-period kind; the JP
// formats go through Jp_LookupText. Not written: the decoration skins'
// per-period price list, kind 0x38 except sub kinds 5 and 6, from the JP
// sSkin's shorts +0xea and the period names data_f02e64; their row shows no
// price here)
void JpShopMain::DrawItemPrice(FrGraphicInterface* graphic, float x, float y, IFF_ITEM_COMMON& item, FrListItem* listItem) {
	if (listItem == NULL || (item.m_flags0 & 0xE0) == 0x80) {
		return;
	}

	bool discount;
	if (ShopListKind(item) == 2) {
		discount = ShopIsInSalePeriod(item.m_typeId);
	} else {
		discount = item.m_discountPrice != 0 && item.m_discountPrice < item.m_price;
	}

	if (discount) {
		graphic->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
		graphic->Print11(WPoint(x, y) + listItem->pos, 2,
			MakeStr(Jp_LookupText((item.m_flags0 & 1) ? "%d\xC4\xED\xC5\xB0" : "%d\xC6\xCE"), item.m_discountPrice));
		graphic->SetTextColor(0xFF000000, 0xFFFFFFFF);

		x -= 40.0f;

		graphic->SetTextColor(0xFF000000, 0xFFFFFFFF);
		int width = float2int(graphic->Print11(WPoint(x, y) + listItem->pos, 2,
			MakeStr(Jp_LookupText((item.m_flags0 & 1) ? "%d\xC4\xED\xC5\xB0" : "%d\xC6\xCE"), item.m_price)));
		graphic->SetTextColor(0xFF000000, 0xFFFFFFFF);

		g_view->DrawLine2D(WPoint(x - width - 1.0f, y + float2int(graphic->GetFontHeight11() * 0.5f)) + listItem->pos,
			WPoint(x + 1.0f, y + float2int(graphic->GetFontHeight11() * 0.5f)) + listItem->pos, 0xFF000000, 0xFF000000, 0);
		return;
	}

	unsigned long typeId = item.m_typeId;
	unsigned long skinKind = (typeId >> 22) & 0xF;
	if ((typeId & 0xFC000000) != 0x38000000 || skinKind == 6 || skinKind == 5) {
		graphic->Print(WPoint(x, y) + listItem->pos, 2,
			MakeStr(Jp_LookupText((item.m_flags0 & 1) ? "%d\xC4\xED\xC5\xB0" : "%d\xC6\xCE"), item.m_price));
	}
}

// 0xa90120 (the KR GetItemIcon)
const Bitmap* JpShopMain::GetItemIcon(unsigned long typeId) {
	std::map<unsigned long, const Bitmap*>::iterator i = m_itemIcons.find(typeId);
	if (i != m_itemIcons.end()) {
		return i->second;
	}

	IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(typeId);
	if (item == NULL) {
		return NULL;
	}

	const Bitmap* bitmap = g_pFresh->RegisterBitmap(item->m_icon);
	if (bitmap == NULL) {
		return bitmap;
	}

	m_itemIcons.insert(std::map<unsigned long, const Bitmap*>::value_type(item->m_typeId, bitmap));
	return bitmap;
}

// 0xa936a0 (the KR OnItemListOwnerDraw on the JP sub tabs (KR + 1 from the
// club set on) and records: a greyed frame and no buy button out of the
// level range, the JP marks (limited tag, surprise sale), the buy / gift
// mini buttons [1] / [2] hidden for limit kind 3. Not written yet: the 35
// data_d45760 parts' UCC name and picture (a temporary sItemInfo from the
// JP record +0x238 / +0x23c), the club set / caddie / mascot stat rows (sub
// tabs 7, 11, 13: 0xa89380 on the 0xa171e0 / 0xa179d0 records), the
// limited-type picture (Jp_SharedDoc_GetItemLimitKind is 0 in the port)
// and the m_selectItemMark row mark (its position comes from the list
// box's +0xe0 / +0xe4))
void JpShopMain::OnItemListOwnerDraw(intptr_t param) {
	FrListItem* listItem = (FrListItem*)param;
	if (listItem == NULL) {
		return;
	}

	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)listItem->pData;

	const Bitmap* bitmap;
	if (item == NULL) {
		bitmap = m_itemBaseEmpty;
	} else {
		unsigned long color;
		if (listItem->selected) {
			bitmap = m_itemBaseDown;
			color = 0xFFFF0000;
		} else {
			color = 0xFF000000;
			if (listItem->underCursor) {
				bitmap = m_itemBaseOver;
			} else {
				bitmap = m_itemBaseNormal;
			}
		}
		graphic->SetTextColor(color, 0xFFFFFFFF);
		graphic->SetTextStyle(0);
	}

	bool buyable = true;
	if (bitmap) {
		if (IsAboveShopLevel(item) || IsBelowShopLevel(item)) {
			graphic->DrawTexture(bitmap, WRect(listItem->pos.x, listItem->pos.y, (float)bitmap->Width(), (float)bitmap->Height()), 0x50B0B0B0, 0);
			buyable = false;
		} else {
			graphic->DrawTexture(bitmap, WRect(listItem->pos.x, listItem->pos.y, (float)bitmap->Width(), (float)bitmap->Height()), 0xFFFFFFFF, 0);
		}
	}

	if (item == NULL) {
		return;
	}

	unsigned long typeId = item->m_typeId;
	if ((typeId & 0xFC000000) == 0x38000000 && (typeId & 0x3C00000) == 0x1800000) {
		const Bitmap* titleBg = g_pFresh->GetBitmap("Title_BG");
		graphic->DrawTexture(titleBg, WRect(listItem->pos.x + 7.0f, listItem->pos.y + 7.0f, (float)titleBg->Width(), (float)titleBg->Height()), 0xFFFFFFFF, 0);
	}

	const Bitmap* icon = GetItemIcon(item->m_typeId);
	if (UccManager()->IsDisplayUccClothes(item)) {
		sDisplayUccClothes* display = (sDisplayUccClothes*)listItem->pData;
		icon = GetUccItemIcon(display->m_serial);
	}

	typeId = item->m_typeId;
	bool uccPart = (typeId & 0x3E00000) == 0x800000 && (typeId & 0x1FE000) == 0x1F8000;

	if (icon) {
		float offsetX;
		float offsetY;
		switch ((int)m_subTab) {
		case 0:
		case 1:
			if ((typeId & 0xFC000000) == 0x24000000) {
				offsetX = 7.0f;
				offsetY = 2.0f;
			} else {
				offsetX = 16.0f;
				offsetY = 27.0f;
			}
			break;
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 13:
		case 27:
			offsetX = 7.0f;
			offsetY = 2.0f;
			break;
		case 11:
		case 12:
			if ((typeId & 0xFC000000) == 0x20000000) {
				offsetX = 7.0f;
				offsetY = 2.0f;
			} else {
				offsetX = 16.0f;
				offsetY = 27.0f;
			}
			break;
		case 16:
			offsetX = 7.0f;
			offsetY = 12.0f;
			break;
		case 28:
		case 34:
			offsetX = 16.0f;
			offsetY = 12.0f;
			break;
		case 29:
		case 30:
		case 31:
		case 32:
		case 33:
			offsetX = 21.0f;
			offsetY = 12.0f;
			break;
		case 35: {
			// The title's icon centred on "Title_BG".
			const Bitmap* titleBg = g_pFresh->GetBitmap("Title_BG");
			offsetX = (float)titleBg->Width() * 0.5f + 7.0f - (float)icon->Width() * 0.5f;
			offsetY = 27.0f;
			break;
		}
		default:
			if ((typeId & 0xFC000000) == 0x24000000 && !uccPart) {
				offsetX = 7.0f;
				offsetY = 2.0f;
			} else {
				offsetX = 16.0f;
				offsetY = 27.0f;
			}
			break;
		}
		graphic->DrawTexture(icon, WRect(listItem->pos.x + offsetX, listItem->pos.y + offsetY, (float)icon->Width(), (float)icon->Height()), 0xFFFFFFFF, 0);
	}

	if (UccManager()->IsDisplayUccClothes(item) && m_itemBaseAward) {
		graphic->DrawTexture(m_itemBaseAward, WRect(listItem->pos.x, listItem->pos.y, (float)m_itemBaseAward->Width(), (float)m_itemBaseAward->Height()), 0xFFFFFFFF, 0);
	}

	graphic->SetTextStyle(0);
	WPoint namePos(listItem->pos.x + 182.0f, listItem->pos.y + 7.0f);
	switch ((int)m_subTab) {
	case SUB_TAB_AZTEC: {
		IFF_STRUCT::sBall* ball = (IFF_STRUCT::sBall*)item;
		if ((typeId & 0xFC000000) == 0x24000000) {
			graphic->Print11(namePos, 2, Jp_LookupText("%s"), item->m_name);
		} else {
			graphic->Print11(namePos, 2, Jp_LookupText("%s(%d\xB0\xB3)"), item->m_name, ball->quantity);
		}
		break;
	}
	case 9:
	case 10: {
		IFF_STRUCT::sItem* usable = (IFF_STRUCT::sItem*)item;
		const short* counts = &usable->quantity;
		if ((typeId & 0xFC000000) == 0x18000000 && (typeId & 0x1F00000) == 0x100000) {
			graphic->Print11(namePos, 2, item->m_name);
		} else if ((typeId & 0x1FE000) == 0x1FC000) {
			graphic->Print11(namePos, 2, item->m_name);
		} else if (counts[0] > 0 && counts[1] <= 0) {
			graphic->Print11(namePos, 2, Jp_LookupText("%s(%d\xB0\xB3)"), item->m_name, counts[0]);
		} else {
			graphic->Print11(namePos, 2, item->m_name);
		}
		break;
	}
	case 28:
		if (IsLocalContent((localContentType_t)0x52)) {
			graphic->Print11(namePos, 2, item->m_name);
		}
		break;
	case 29:
	case 30:
	case 31:
	case 33:
		if (IsLocalContent((localContentType_t)0x57)) {
			IFF_STRUCT::sFurniture* furniture = (IFF_STRUCT::sFurniture*)item;
			short count = furniture->m_dayPrices[0];
			if (count > 0) {
				graphic->Print11(namePos, 2, Jp_LookupText("%s(%d\xB0\xB3)"), item->m_name, count);
			} else {
				graphic->Print11(namePos, 2, item->m_name);
			}
		}
		break;
	case 32:
		if ((typeId & 0xFC000000) == 0x48000000) {
			IFF_STRUCT::sFurniture* furniture = (IFF_STRUCT::sFurniture*)item;
			short count = furniture->m_dayPrices[0];
			if (count > 0) {
				graphic->Print11(namePos, 2, Jp_LookupText("%s(%d\xB0\xB3)"), item->m_name, count);
			} else {
				graphic->Print11(namePos, 2, item->m_name);
			}
		}
		break;
	default:
		if (UccManager()->IsDisplayUccClothes(item)) {
			sDisplayUccClothes* display = (sDisplayUccClothes*)listItem->pData;
			graphic->Print11(namePos, 2, GetUccItemName(display->m_serial));
			g_pFresh->GetManager()->PrintText(WPoint(listItem->pos.x + 182.0f, listItem->pos.y + 23.0f), 2, display->m_uccKey.c_str(), -1.0f, 0xFFFFFFFF);
		} else {
			graphic->Print11(namePos, 2, item->m_name);
		}
		break;
	}

	if (IsRentalSideTab()) {
		DrawRentalPrice(182.0f, 23.0f, *item, listItem);
	} else {
		DrawItemPrice(graphic, 182.0f, 23.0f, *item, listItem);
	}

	switch ((int)m_subTab) {
	case 2:
	case 3:
	case 4:
	case 5:
	case 27: {
		// The part's stats (not written: the slot mark m_markBegin the JP
		// draws after them when the JP record's byte +0x234 is set).
		IFF_STRUCT::sPart* part = (IFF_STRUCT::sPart*)listItem->pData;
		if (part) {
			DrawStatus2(graphic, listItem, part->m_baseStat, part->m_slotStat, 0.0f);
		}
		break;
	}
	default:
		break;
	}

	if (listItem->selected && (unsigned int)(item->m_level & 0x7F) > (unsigned int)WSingleton<CSharedDoc>::Instance()->m_myLevel) {
		const Bitmap* level = g_pFresh->GetBitmap(MakeStr("level_%03d", (item->m_level & 0x7F) + 1));
		if (level) {
			graphic->DrawTexture(level, WRect(listItem->pos.x + 10.0f, listItem->pos.y + 4.0f, (float)level->Width(), (float)level->Height()), 0xFFFFFFFF, 0);
		}
	}

	if (ShopListKind(*item) == 2) {
		if (ShopIsInSalePeriod(item->m_typeId) && m_markSurpriseSale) {
			graphic->DrawTexture(m_markSurpriseSale, WRect(listItem->pos.x, listItem->pos.y, (float)m_markSurpriseSale->Width(), (float)m_markSurpriseSale->Height()), 0xFFFFFFFF, 0);
		}
	} else if (Jp_SharedDoc_GetItemLimitKind(item->m_typeId) != 0 && m_markLimitedTag) {
		graphic->DrawTexture(m_markLimitedTag, WRect(listItem->pos.x, listItem->pos.y, (float)m_markLimitedTag->Width(), (float)m_markLimitedTag->Height()), 0xFFFFFFFF, 0);
	} else {
		if (m_markLimited && (item->m_saleStart.wYear != 0 || item->m_saleEnd.wYear != 0)) {
			graphic->DrawTexture(m_markLimited, WRect(listItem->pos.x, listItem->pos.y, (float)m_markLimited->Width(), (float)m_markLimited->Height()), 0xFFFFFFFF, 0);
		}
		if ((item->m_flags1 & 1) && m_markNew) {
			graphic->DrawTexture(m_markNew, WRect(listItem->pos.x, listItem->pos.y, (float)m_markNew->Width(), (float)m_markNew->Height()), 0xFFFFFFFF, 0);
		}
		if ((item->m_flags1 & 2) && m_markHot) {
			graphic->DrawTexture(m_markHot, WRect(listItem->pos.x, listItem->pos.y, (float)m_markHot->Width(), (float)m_markHot->Height()), 0xFFFFFFFF, 0);
		}
	}

	const WPoint& mousePos = g_pFresh->GetManager()->GetMousePos();

	unsigned int sellKind = item->m_flags0 >> 5;
	bool canBuy = sellKind == 1 || sellKind == 3;
	bool showBuy = IsShopSpecialItem(item->m_typeId) || canBuy || sellKind == 7;

	typeId = item->m_typeId;
	if ((typeId & 0xFC000000) == 0x38000000 && (typeId & 0x3C00000) == 0x1800000) {
		Title* title = g_TitleManager->GetTitle((eTitle)(typeId & 0x3FFFFF));
		if (title && !title->CanEquip()) {
			const Bitmap* noBuy = g_pFresh->GetBitmap("do not purchase");
			if (noBuy) {
				graphic->DrawTexture(noBuy, WRect(WPoint(listItem->pos.x + 1.0f, listItem->pos.y + 1.0f), noBuy->Width(), noBuy->Height()), 0xFFFFFFFF, 0);
				showBuy = false;
			}
		}
	} else if (!buyable) {
		const Bitmap* noBuy = g_pFresh->GetBitmap("do not purchase");
		if (noBuy) {
			graphic->DrawTexture(noBuy, WRect(WPoint(listItem->pos.x + 1.0f, listItem->pos.y + 1.0f), noBuy->Width(), noBuy->Height()), 0xFFFFFFFF, 0);
			showBuy = false;
		}
	}

	if (Jp_SharedDoc_GetItemLimitKind(item->m_typeId) != 3 && showBuy) {
		WRect rc = m_miniBtns[1].rect + listItem->pos;
		if (!m_miniBtns[1].enable) {
			graphic->DrawTexture(m_miniBtns[1].normal, rc, 0x88FFFFFF, 0);
		} else if (listItem->underCursor && rc.IsInRect(mousePos)) {
			graphic->DrawTexture(m_miniBtns[1].over, rc, 0xFFFFFFFF, 0);
			CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x1b);
		} else {
			graphic->DrawTexture(m_miniBtns[1].normal, rc, 0xFFFFFFFF, 0);
		}
	}

	sellKind = item->m_flags0 >> 5;
	bool canGift = sellKind == 1 || sellKind == 2;
	bool showGift = (item->m_flags0 & 1) != 0 && (canGift || sellKind == 7);
	if (m_sideTabRental && m_sideTabRental->GetStatus() == FrButton::PRESSED && m_shopTab == 1) {
		showGift = false;
	}
	if (IsShopSpecialItem(item->m_typeId)) {
		showGift = false;
	}

	if (Jp_SharedDoc_GetItemLimitKind(item->m_typeId) != 3 && showGift) {
		WRect rc = m_miniBtns[2].rect + listItem->pos;
		if (!m_miniBtns[2].enable) {
			graphic->DrawTexture(m_miniBtns[2].normal, rc, 0x88FFFFFF, 0);
		} else if (listItem->underCursor && rc.IsInRect(mousePos)) {
			graphic->DrawTexture(m_miniBtns[2].over, rc, 0xFFFFFFFF, 0);
			CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x1c);
		} else {
			graphic->DrawTexture(m_miniBtns[2].normal, rc, 0xFFFFFFFF, 0);
		}
	}
}

// 0xa8ca90 (the KR SelectItemList(FrListItem*) on the JP lobby: the
// character, the hair colour and the parts are previewed on the
// "LobbyAvatar" actor (messages 0x187 the character, 0x18C the tid list and
// a hair colour), the caddies on "LobbyCaddie" (0x1A7), instead of the KR
// pet frame. A UCC set (kind 0x800000, 0x1F8000) is handled as the kind of
// its first component. Not written yet: the set items (kind 9, the JP
// 0xa8d0b6 block), the skins (kind 14, 0xa8d7b7), and on the character
// case the calls 0x5d35b0(2, 0) (when byte +0x184 of the JP global
// 0xe781b8 is set) and CSharedDoc 0x643ed0(m_standPetTypeId), the lobby
// flags data_e78c38 +0x258 / +0x259 the character and caddie cases set,
// and the exhibition call 0x873a30 of the part case)
bool JpShopMain::SelectItemList(FrListItem* pItem) {
	IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)pItem->pData;

	unsigned long typeId = item->m_typeId;
	if ((typeId & 0x3E00000) == 0x800000 && (item->m_typeId & 0x1FE000) == 0x1F8000) {
		IFF_STRUCT::sSetItem* setItem = ItemManager()->FindSetItem(item->m_typeId);
		if (setItem) {
			typeId = setItem->m_itemTypeIds[0];
		}
	}

	switch (typeId >> 26) {
	case 1: {
		WSingleton<CSharedDoc>::Instance()->BuildMyPartTidListDefault(item->m_typeId);
		CPartTidList& tidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();
		tidList.m_hairClr = m_hairColor;
		m_charParts = tidList.m_partTids;
		m_standPetTypeId = item->m_typeId;

		MsgObject msg(0, 0x187, (intptr_t)&m_standPetTypeId, 0, 0, 0, 0);
		IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
		if (actor) {
			actor << msg;
		}

		// Keep the hair style of the cart.
		IFF_ITEM_COMMON* hairItem = NULL;
		for (FrListBox::ITEM_LIST::iterator it = m_cartList->m_itemList.begin(); it != m_cartList->m_itemList.end(); ++it) {
			hairItem = (IFF_ITEM_COMMON*)(*it)->pData;
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
		IFF_ITEM_COMMON* part = (IFF_ITEM_COMMON*)pItem->pData;
		CPartTidList& tidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

		unsigned long overlapped = tidList.GetOverlapped(part->m_typeId);
		if (!tidList.CanSetPart(part->m_typeId)) {
			m_itemListBox->UnselectItem(pItem);
			IFF_STRUCT::sPart* restricted = (IFF_STRUCT::sPart*)part;
			if (restricted->m_wearRestricted) {
				// The JP record holds the name of the outfit the part needs.
				char text[256];
				sprintf(text, Jp_LookupText("\xBA\xBB \xBB\xF3\xC7\xB0\xC0\xBA \\c0xffff0000\\c%s\xC0\xBB(\xB8\xA6) \xC0\xD4\xC0\xBA \xBB\xF3\xC5\xC2\xBF\xA1\xBC\xAD\xB8\xB8\\c0xff000000\\c \xC2\xF8\xBF\xEB\xC7\xD2 \xBC\xF6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9."),
					(const char*)&restricted->m_wearRestricted);
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)text, 0, 0, 0, 0));
				return false;
			}

			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText("\\c0xffff0000\\c\xBB\xF3\xC7\xB0\xC0\xBB \xC2\xF8\xBF\xEB\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9."), 0, 0, 0, 0));
			return false;
		}

		TakeOff_PartNCartList(tidList, overlapped);
		TakeOff_AuxPartNCartList(tidList, overlapped);

		unsigned long serial = 0;
		IFF_ITEM_COMMON* partItem = (IFF_ITEM_COMMON*)pItem->pData;
		if (UccManager()->IsDisplayUccClothes(partItem)) {
			// A UCC part replaces the old items and the selection of its slot.
			unsigned long partTypeId = item->m_typeId;
			for (std::list<oldItem_t>::iterator it = m_shoppersOldItems.begin(); it != m_shoppersOldItems.end();) {
				if (((it->typeId >> 13) & 0x1F) == ((partTypeId >> 13) & 0x1F)) {
					UnselectItemFromItemList(it->typeId);
					it = m_shoppersOldItems.erase(it);
				} else {
					++it;
				}
			}

			for (FrListBox::ITEM_LIST::iterator i = m_itemListBox->m_itemList.begin(); i != m_itemListBox->m_itemList.end(); ++i) {
				if (*i != pItem) {
					IFF_ITEM_COMMON* other = (IFF_ITEM_COMMON*)(*i)->pData;
					if (other && ((other->m_typeId ^ partTypeId) & 0x3E000) == 0) {
						m_itemListBox->UnselectItem(*i);
					}
				}
			}

			serial = ((sDisplayUccClothes*)partItem)->m_serial;
		}

		PutOnNBackupShoppersOldItem(part->m_typeId, tidList, serial);

		if (!UccManager()->IsDisplayUccClothes(partItem)) {
			unsigned char slot = ((IFF_STRUCT::sPart*)part)->slot;
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
			MsgObject msg(0, 0x18C, (intptr_t)&tidList, (intptr_t)&m_hairColor, 0, 0, 0);
			IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
			if (actor) {
				actor << msg;
			}
		}
		TurnOnItemStatus();
		g_audio->PlaySfx("item_select_cloth", 0, 0, 0, 0, 0.5f, 200.0f);
		break;
	}

	case 0xF: {
		IFF_STRUCT::sHairStyle* hair = (IFF_STRUCT::sHairStyle*)pItem->pData;
		CPartTidList& tidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

		MsgObject msg(0, 0x18C, (intptr_t)&tidList, (intptr_t)&hair->m_color, 0, 0, 0);
		IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
		if (actor) {
			actor << msg;
		}

		// Only one hair style in the cart.
		for (FrListBox::ITEM_LIST::iterator it = m_cartList->m_itemList.begin(); it != m_cartList->m_itemList.end();) {
			IFF_ITEM_COMMON* cartItem = (IFF_ITEM_COMMON*)(*it)->pData;
			if ((cartItem->m_typeId & 0xFC000000) == 0x3C000000) {
				UnselectItemFromItemList(cartItem->m_typeId);
				it = DeleteItemFromCartList(cartItem->m_typeId);
			} else {
				++it;
			}
		}

		m_cartList->AddItem(hair);
		m_hairColor = hair->m_color;
		TurnOnItemStatus();
		g_audio->PlaySfx("item_select_hair", 0, 0, 0, 0, 0.5f, 200.0f);
		break;
	}

	case 4:
		m_cartList->ClearItem();
		m_cartList->AddItem(item);
		m_clubsetTypeId = typeId;
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

	case 7: {
		m_cartList->ClearItem();
		m_cartList->AddItem(item);
		m_caddieTypeId = item->m_typeId;

		MsgObject msg(0, 0x1A7, (intptr_t)&m_caddieTypeId, 0, 0, 0, 0);
		IActor* actor = AfxGetTask()->GetActor("LobbyCaddie");
		if (actor) {
			actor << msg;
		}
		TurnOnItemStatus();
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		ShowItemDesc(item->m_typeId);
		break;
	}

	case 8:
		if ((item->m_typeId & 0x1FE000) != 0x6000) {
			m_cartList->ClearItem();
			m_cartList->AddItem(item);

			MsgObject msg(0, 0x1A7, (intptr_t)&item->m_typeId, 0, 0, 0, 0);
			IActor* actor = AfxGetTask()->GetActor("LobbyCaddie");
			if (actor) {
				actor << msg;
			}
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

	case 6:
	case 0x12:
	case 0x1F:
		m_cartList->ClearItem();
		m_cartList->AddItem(item);
		{
			// The port's record type id is 32 bits; SafeLoadPet takes an
			// unsigned long.
			unsigned long petTypeId = item->m_typeId;
			SafeLoadPet(&petTypeId, false);
		}
		TurnOffItemStatus();
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		ShowItemDesc(item->m_typeId);
		break;

	default:
		break;
	}

	pItem->selected = true;
	return true;
}

// 0xa90bf0 (the KR OnItemListBtnDown on the JP: the buy button [1] also
// takes kind 7 and data_d45760's parts, the gift button [2] needs the
// cookie flag; both skip the items of limit kind 3 and the buy button the
// items out of the level range; the character messages go to the current
// task, the selected character is still shown; no Gateway check before the
// checkout; a title that cannot be worn ignores the click; the buy buttons
// follow the cart on shop tab 1, and tab 5 except sub tab 35. Not written
// yet: the 0x1a0000fd check (a held one in the JP CSharedDoc list +0x375c
// opens a "notify" form instead of buying))
void JpShopMain::OnItemListBtnDown(void) {
	FrListItem* listItem = m_itemListBox->GetItemUnderCursor();
	if (listItem == NULL) {
		return;
	}

	if (m_anotherHand.IsWorking(0)) {
		m_itemListBox->ToggleItem(listItem);
		return;
	}

	IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)listItem->pData;
	CPartTidList& tidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

	if (UccManager()->IsDisplayUccClothes(item)) {
		// The list entry is a part record followed by the UCC ids (sDisplayUccClothes).
		sDisplayUccClothes* display = (sDisplayUccClothes*)listItem->pData;
		IFF_STRUCT::sPart* base = ItemManager()->FindPart(display->m_typeId);
		sUccClothes* clothes = UccManager()->FindClothes(display->m_serial);
		if (base && clothes) {
			IFF_STRUCT::sPart* part = (IFF_STRUCT::sPart*)display;
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

	const WPoint& mousePos = g_pFresh->GetManager()->GetMousePos();

	unsigned long typeId = item->m_typeId;
	if ((typeId & 0xFC000000) == 0x38000000 && (typeId & 0x3C00000) == 0x1800000) {
		Title* title = g_TitleManager->GetTitle((eTitle)(typeId & 0x3FFFFF));
		if (title && !title->CanEquip()) {
			return;
		}
	}

	if ((m_miniBtns[1].rect + listItem->pos).IsInRect(mousePos)) {
		unsigned int sellKind = item->m_flags0 >> 5;
		bool canBuy = sellKind == 1 || sellKind == 3 || sellKind == 7;
		if (!m_miniBtns[1].enable) {
			canBuy = false;
		}
		if (IsShopSpecialItem(item->m_typeId)) {
			canBuy = true;
		}

		if (Jp_SharedDoc_GetItemLimitKind(item->m_typeId) != 3 && !IsAboveShopLevel(item) && !IsBelowShopLevel(item) && canBuy) {
			ResetItemList(false);
			ClearCartList(false, &tidList);
			m_shoppersOldItems.clear();
			m_itemListBox->SelectItem(listItem, true);

			unsigned long kind = item->m_typeId >> 26;
			if (kind == 15 && !HaveCharacter(m_standPetTypeId)) {
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText("\xC7\xD8\xB4\xE7 \xC4\xB3\xB8\xAF\xC5\xCD\xB8\xA6 \xB0\xA1\xC1\xF6\xB0\xED \xC0\xD6\xC1\xF6 \xBE\xCA\xBD\xC0\xB4\xCF\xB4\xD9"), 0, 0, 0, 0));
				return;
			}
			if (kind == 1 && HaveCharacter(item->m_typeId)) {
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText("\xC7\xD8\xB4\xE7 \xC4\xB3\xB8\xAF\xC5\xCD\xB8\xA6 \xC0\xCC\xB9\xCC \xB0\xA1\xC1\xF6\xB0\xED \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9"), 0, 0, 0, 0));
				SelectItemList(listItem);
				return;
			}

			if (SelectItemList(listItem)) {
				if ((item->m_typeId & 0xFC000000) == 0x20000000 && (item->m_typeId & 0x1FE000) == 0x6000) {
					UpgradeCaddie(m_caddieTypeId);
					return;
				}
				Checkout();
			}
			return;
		}
	} else if ((m_miniBtns[2].rect + listItem->pos).IsInRect(mousePos)) {
		unsigned int sellKind = item->m_flags0 >> 5;
		bool canGift = (item->m_flags0 & 1) != 0 && (sellKind == 1 || sellKind == 2 || sellKind == 7);
		if (IsShopSpecialItem(item->m_typeId)) {
			canGift = false;
		}
		if (!m_miniBtns[2].enable) {
			canGift = false;
		}

		if (Jp_SharedDoc_GetItemLimitKind(item->m_typeId) != 3 && canGift) {
			ResetItemList(false);
			m_itemListBox->SelectItem(listItem, true);
			if (SelectItemList(listItem)) {
				SendGift(*item);
			}
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

	if (m_shopTab == 1 || (m_shopTab == 5 && m_subTab != 35)) {
		m_buyAll->Enable(!m_cartList->m_itemList.empty() || !m_auxCartList->m_itemList.empty());
		m_buyReset->Enable(!m_cartList->m_itemList.empty() || !m_auxCartList->m_itemList.empty());
	}
}

// 0xa85ee0 (the KR ShowItemDesc)
void JpShopMain::ShowItemDesc(unsigned long typeId) {
	IFF_STRUCT::sDesc* desc = ItemManager()->FindDesc(typeId);
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

// 0xa8da20 (the KR UnselectItemList on the JP: a part taken off undresses
// the "LobbyAvatar" through message 0x18A (the tid list, the type id, a zero
// id byte) instead of the KR pet frame's SetPart. Not written yet: the set
// items (kind 9, 0xa8dbbc), the skins (kind 14, 0xa8de5e) and the hair
// styles (kind 15, 0xa8dd4a): they are left as they are)
void JpShopMain::UnselectItemList(unsigned long typeId) {
	CPartTidList& tidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

	switch (typeId >> 26) {
	case 2: {
		DeleteItemFromCartList(typeId);
		DeleteItemFromAuxCartList(typeId);
		UnselectItemFromItemList(typeId);

		// The parts worn before taking this one off.
		unsigned long oldParts[24];
		memcpy(oldParts, tidList.m_partTids, sizeof(oldParts));

		if (tidList.IsEquipped(typeId, 0)) {
			// The JP passes a zero byte; the actor reads a 32-bit id.
			uint32_t id = 0;
			MsgObject msg(0, 0x18A, (intptr_t)&tidList, (intptr_t)&typeId, (intptr_t)&id, 0, 0);
			IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
			if (actor) {
				actor << msg;
			}
			RestoreShoppersOldItem(typeId, tidList);
			m_charParts = tidList.m_partTids;
		}

		for (int i = 0; i < 24; i++) {
			unsigned long oldPart = oldParts[i];
			if (oldPart && typeId != oldPart && tidList.m_partTids[i] != oldPart && oldPart != 2 && oldPart != 3) {
				UnselectItemFromItemList(oldPart);
				DeleteItemFromCartList(oldPart);
				DeleteItemFromAuxCartList(oldPart);
			}
		}

		tidList.m_hairClr = m_hairColor;
		ShowMainModel(tidList);
		break;
	}

	case 7:
	case 31:
		break;

	case 9:
	case 14:
	case 15:
		break;

	default:
		UnselectItemFromItemList(typeId);
		TurnOffItemStatus();
		break;
	}
}

// 0xa8be80 (the KR PutOnNBackupShoppersOldItem on the JP: the part is put on
// the "LobbyAvatar" through message 0x18A (the tid list, the type id, the
// id, the changed-slot mask back), first taken off when worn; the parts it
// replaces are kept in m_shoppersOldItems for RestoreShoppersOldItem)
void JpShopMain::PutOnNBackupShoppersOldItem(unsigned long tid, CPartTidList& tidList, unsigned long id) {
	CPartTidList oldList = tidList;

	if (tidList.IsEquipped(tid, 0)) {
		// The JP passes a byte 1 as the id of this first message; the actor
		// reads a 32-bit id.
		uint32_t takeOffId = 1;
		MsgObject msg(0, 0x18A, (intptr_t)&tidList, (intptr_t)&tid, (intptr_t)&takeOffId, 0, 0);
		IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
		if (actor) {
			actor << msg;
		}
	}

	int changedSlots = 0;
	{
		MsgObject msg(0, 0x18A, (intptr_t)&tidList, (intptr_t)&tid, (intptr_t)&id, (intptr_t)&changedSlots, 0);
		IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
		if (actor) {
			actor << msg;
		}
	}

	uint32_t changed = (uint32_t)changedSlots;
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
