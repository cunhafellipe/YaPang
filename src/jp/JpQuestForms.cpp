// JP-only game code: _gateway::_questoneday::FrOnedayQuest, the daily-quest
// window, and FrAchievement, the achievement window. See JpQuestForms.h.
// JP VAs in comments, in JP address order.
#include "JpQuestForms.h"

#include <algorithm>

#include <math.h>
#include <string.h>

#include "InputManager.h"
#include "ItemManager.h"
#include "JpLobbyGlue.h"
#include "MessageManager.h"
#include "ObjectFactory.h"
#include "Packet.h"
#include "RFTextManager.h"
#include "ScratchDlg.h"
#include "SharedDoc.h"
#include "TaskManager.h"
#include "UccLibrary.h"
#include "actor.h"
#include "bitmap.h"
#include "commonutil.h"
#include "frarea.h"
#include "frbutton.h"
#include "fresh.h"
#include "frgraphicinterface.h"
#include "frlistbox.h"
#include "frstatic.h"
#include "frwnd.h"
#include "shared/luasystem/LuaScript.h"
#include "shared/luasystem/LuaScriptManager.h"
#include "shared/luasystem/luatablehelper.h"
#include "uccmanager.h"
#include "wfont.h"
#include "woverlay.h"
#include "wresrcmng.h"

// lib: 0x51c510, 0x4ea6e0, 0x51b5b0, 0x518f30 (scalar deleting
// destructors), 0x51c200, 0x51c1f0, 0x4e9940, 0x4e9950,
// 0x51b100, 0x5185d0 (this-adjusting thunks), 0xcccb60, 0xcca4c0, 0xcccdd0,
// 0xccced0 (message-map initializers), 0x51b230 (std::vector push_back)

namespace _gateway {
namespace _questoneday {

// --- FrAchievement ------------------------------------------------------------

// 0xe74d44 (static init not traced)
const WRTTI FrAchievement::m_RTTI("FrAchievement", &FrForm::m_RTTI);

// 0x4e1290 (deleting destructor)
FrAchievement::CAhievementPainter::~CAhievementPainter(void) {
}

// 0x4e12c0 (deleting destructor)
FrAchievement::CAhievementMainContents::~CAhievementMainContents(void) {
}

// 0x4e12f0 (deleting destructor)
FrAchievement::CAhievementDetailInfo::~CAhievementDetailInfo(void) {
}

// 0x4e1320 (deleting destructor)
FrAchievement::CAhievementPopupInfo::~CAhievementPopupInfo(void) {
}

// 0x4e13a0
const sFRESH_MSGMAP* FrAchievement::GetMessageMap(void) const {
	return &FrAchievement::_MsgMap;
}

sFRESH_MSGMAP FrAchievement::_MsgMap = { &FrForm::_MsgMap, &FrAchievement::_MsgEntries[0] };

sFRESH_ENTRY FrAchievement::_MsgEntries[] = {
	{ "btn_summary", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAchievement::OnBtnSummaryInit },
	{ "btn_char", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAchievement::OnBtnCharInit },
	{ "btn_gamemode", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAchievement::OnBtnGameModeInit },
	{ "btn_adventure", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAchievement::OnBtnAdventureInit },
	{ "btn_statistics", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAchievement::OnBtnStatisticsInit },
	{ "btn_item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAchievement::OnBtnItemInit },
	{ "btn_system", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAchievement::OnBtnSystemInit },
	{ "btn_challenge", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAchievement::OnBtnChallengeInit },
	{ "btn_event", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAchievement::OnBtnEventInit },
	{ "btn_summary", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAchievement::OnBtnSummaryClicked },
	{ "btn_char", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAchievement::OnBtnCharClicked },
	{ "btn_gamemode", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAchievement::OnBtnGameModeClicked },
	{ "btn_adventure", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAchievement::OnBtnAdventureClicked },
	{ "btn_statistics", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAchievement::OnBtnStatisticsClicked },
	{ "btn_item", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAchievement::OnBtnItemClicked },
	{ "btn_system", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAchievement::OnBtnSystemClicked },
	{ "btn_challenge", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAchievement::OnBtnChallengeClicked },
	{ "btn_event", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAchievement::OnBtnEventClicked },
	{ "area_summary", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAchievement::OnAreaSummaryInit },
	{ "area_summary", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAchievement::OnAreaSummaryDraw },
	{ "area_element_layer", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAchievement::OnAreaElementLayerInit },
	{ "area_element_layer", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAchievement::OnAreaElementLayerDraw },
	{ "list_ach_recent", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAchievement::OnListAchRecentInit },
	{ "list_ach_recent", FRCMD_MOUSEMOVE, FrFuncVV, (FRESH_PMSG)&FrAchievement::OnListAchRecentMouseMove },
	{ "list_ach_element", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAchievement::OnListAchElementInit },
	{ "list_ach_element", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAchievement::OnListAchElementDraw },
	{ "list_ach_element", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAchievement::OnListAchElementClicked },
	{ "list_ach_item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAchievement::OnListAchItemInit },
	{ "list_ach_item", FRCMD_MOUSEMOVE, FrFuncVV, (FRESH_PMSG)&FrAchievement::OnListAchItemMouseMove },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x4e1450: 100.0 is the double 0xd66ca8.
void FrAchievement::UpdateCategoryGauges(void) {
	for (int i = 0; i < 8; i++) {
		int32_t points = m_pDoc->GetCategoryPoints(i);
		m_categoryPoints[i] = points;
		int32_t total = m_pDoc->GetCategoryTotal(i);
		float value;
		if (total == 0) {
			value = 0.0f;
		} else {
			float ratio = (float)points / (float)total;
			float percent = (float)(ratio * 100.0);
			value = (float)floor(percent);
		}
		m_categoryGauges[i].SetTargetNow(value);
	}
}

// 0x4e14f0
int FrAchievement::GetRecentRowAt(const WPoint& point) {
	for (int i = 0; i < m_recentSpanCount; ++i) {
		if (point.x > m_recentSpans[i][0] && point.x < m_recentSpans[i][1]) {
			return i;
		}
	}
	return -1;
}

// 0x4e15a0 (a helper on the address of an element item pointer; name
// ours): the name of a step that has an id, NULL otherwise.
static const char* AchElement_GetStepName(const sAchElementItem* const& element, int index) {
	const sAchievementData* data = element->m_pData;
	if (data->m_stepIds[index] != 0) {
		return data->m_names[index];
	}
	return 0;
}

// 0x4e2930
void FrAchievement::SetCategory(int category) {
	m_category = category;
	if (category == -1) {
		m_pageMode = 0;
	} else if (category >= 0 && category <= 7) {
		m_pageMode = 1;
	}
	if (m_pageMode == 0) {
		m_pages[0]->m_enabled = true;
		m_pages[1]->m_enabled = false;
		m_pages[2]->m_enabled = false;
		return;
	}
	m_pages[0]->m_enabled = false;
	m_pages[1]->m_enabled = true;
	m_pages[2]->m_enabled = true;
}

// 0x4e29b0
void FrAchievement::ReleaseElement(FrListItem* item) {
	((sAchElementItem*)item->pData)->m_shown = 0;
	m_pAchItemList->EnableToolTip(false);
	m_pAchItemList->ClearItem();
	m_pAchItemList->Enable(false);
}

// 0x4e28d0 (name ours): the x of list_ach_item past an element item with
// that many steps: (626 - 66 per step) / 2, 0 once the steps fill the
// row (the doubles 0xd04f48 626.0, 0xd04f50 63.0, 0xd66cf8 3.0, 0xd66ca0
// 0.5).
static float GetStepListX(int stepCount) {
	float count = (float)stepCount;
	float x = (float)(626.0 - (float)(63.0 * count + count * 3.0));
	if (x <= 0.0) {
		x = 0.0f;
	}
	return (float)(x * 0.5);
}

// 0x4e29f0: 9.0 and 70.0 are the doubles 0xd04f60 / 0xd04f58.
void FrAchievement::PlaceStepList(FrListItem* item) {
	const sAchElementItem* element = (const sAchElementItem*)item->pData;
	WRect rect;
	rect.x = (float)(item->pos.x + element->m_stepListX + 9.0);
	rect.y = (float)(item->pos.y + 70.0);
	float count = (float)(int)element->m_steps.size();
	rect.w = (float)m_achBitmaps[16]->Width() * count + count + count;
	rect.h = (float)m_achBitmaps[16]->Height();
	m_pAchItemList->SetRect(rect);
	m_pAchItemList->SetItemSize((int)rect.w, (int)rect.h);
}

// 0x4e2ac0: the first column whose right edge passes the point's x, then
// the point inside it and inside the list's height.
int FrAchievement::GetStepAt(float width, const WPoint& point) {
	const WRect& rect = m_pAchItemList->GetRect();
	int column = 1;
	while (point.x >= (float)(column * width + rect.x)) {
		column++;
		if (column > 10) {
			return -1;
		}
	}
	float left = (float)((column - 1) * width + rect.x);
	if (left <= point.x && point.x <= width + left && rect.y <= point.y && point.y <= rect.y + rect.h) {
		return column - 1;
	}
	return -1;
}

// 0x4e2b80
void FrAchievement::OnBtnSummaryInit(intptr_t param) {
	m_pCategoryBtn[0] = DYNAMIC_CAST(FrButton, param);
}

// 0x4e2bf0
void FrAchievement::OnBtnCharInit(intptr_t param) {
	m_pCategoryBtn[1] = DYNAMIC_CAST(FrButton, param);
}

// 0x4e2c60
void FrAchievement::OnBtnGameModeInit(intptr_t param) {
	m_pCategoryBtn[2] = DYNAMIC_CAST(FrButton, param);
}

// 0x4e2cd0
void FrAchievement::OnBtnAdventureInit(intptr_t param) {
	m_pCategoryBtn[3] = DYNAMIC_CAST(FrButton, param);
}

// 0x4e2d40
void FrAchievement::OnBtnStatisticsInit(intptr_t param) {
	m_pCategoryBtn[4] = DYNAMIC_CAST(FrButton, param);
}

// 0x4e2db0
void FrAchievement::OnBtnItemInit(intptr_t param) {
	m_pCategoryBtn[5] = DYNAMIC_CAST(FrButton, param);
}

// 0x4e2e20
void FrAchievement::OnBtnSystemInit(intptr_t param) {
	m_pCategoryBtn[6] = DYNAMIC_CAST(FrButton, param);
}

// 0x4e2e90
void FrAchievement::OnBtnChallengeInit(intptr_t param) {
	m_pCategoryBtn[7] = DYNAMIC_CAST(FrButton, param);
}

// 0x4e2f00
void FrAchievement::OnBtnEventInit(intptr_t param) {
	m_pCategoryBtn[8] = DYNAMIC_CAST(FrButton, param);
	m_pCategoryBtn[8]->Enable(false);
}

// 0x4e2f70
void FrAchievement::OnAreaSummaryInit(intptr_t param) {
	m_pSummaryArea = DYNAMIC_CAST(FrArea, param);
}

// 0x4e2fe0
void FrAchievement::OnAreaSummaryDraw(intptr_t param) {
	CAhievementPainter* pPainter = m_pages[0];
	if (pPainter->m_enabled) {
		pPainter->Draw(param);
	}
}

// 0x4e3000
void FrAchievement::OnAreaElementLayerInit(intptr_t param) {
	m_pElementLayerArea = DYNAMIC_CAST(FrArea, param);
}

// 0x4e3070
void FrAchievement::OnListAchRecentInit(intptr_t param) {
	m_pAchRecentList = DYNAMIC_CAST(FrListBox, param);
}

// 0x4e30e0
void FrAchievement::OnListAchElementInit(intptr_t param) {
	m_pAchElementList = DYNAMIC_CAST(FrListBox, param);
}

// 0x4e3150
void FrAchievement::OnListAchElementDraw(intptr_t param) {
	m_pages[1]->Draw(param);
}

// 0x4e3170
void FrAchievement::OnListAchItemInit(intptr_t param) {
	m_pAchItemList = DYNAMIC_CAST(FrListBox, param);
	m_pAchItemList->Enable(false);
}

// 0x4e31e0: three rows of ach_bitmap entries side by side, each row at
// its ach_position past the form's rect (entries 6, 7 at position 0;
// 4, 5 at 1; 1, 2, 3 at 2), then the eight category gauges.
void FrAchievement::CAhievementMainContents::DrawBase(void) {
	static const int s_rowFirst[3] = { 5, 3, 0 };
	static const int s_rowCount[3] = { 2, 2, 3 };
	FrAchievement* owner = m_pOwner;
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	for (int row = 0; row < 3; row++) {
		float x = owner->m_achPositions[row].x + owner->GetRect().x;
		float y = owner->m_achPositions[row].y + owner->GetRect().y;
		for (int k = 0; k < s_rowCount[row]; k++) {
			const Bitmap* pBitmap = m_pOwner->m_achBitmaps[s_rowFirst[row] + k];
			WRect dest;
			dest.x = x;
			dest.y = y;
			dest.w = (float)pBitmap->Width();
			dest.h = (float)pBitmap->Height();
			pGDI->DrawTexture(pBitmap, dest, 0xffffffff, 0);
			x = (float)pBitmap->Width() + x;
		}
	}
	for (int i = 0; i < 8; i++) {
		m_pOwner->m_categoryGauges[i].Draw();
	}
}

// CAhievementDetailInfo::Draw's bitmaps: at their own size, from (x, y).
static void DrawAchBitmap(FrGraphicInterface* pGDI, const Bitmap* bitmap, float x, float y) {
	WRect dest;
	dest.x = x;
	dest.y = y;
	dest.w = (float)bitmap->Width();
	dest.h = (float)bitmap->Height();
	pGDI->DrawTexture(bitmap, dest, 0xffffffff, 0);
}

// 0x4e3550: param 0 = the category page's head (ach_bitmap entries 7, 8, 9
// side by side from ach_position 3 past the form, the total gauge, and for
// a category its points ("%d" at ach_position 13, white, text style 1)
// and its challenge text at ach_position 15); else a list_ach_element item:
// entries 10, 11, 12 from ach_position 4, then for a current step its name
// (black, style 1, ach_position 5 + 2.0), its counter value ("%d", or "0"
// without a record; white at 510.0 / 51.0 into the item), the step
// record's name (style 0, + 22.0) and the popup button (entry 24 when
// open, else 23; its right edge on ach_position 6, + 12.0 down); the icon
// at ach_position 8; past the first step entry 22 at ach_position 9 and
// entry 20 at ach_position 6 with the points (font +0x778, 40.0 / 15.0
// into it) and the date ("%d.%d.%d", font +0x77c, 37.0 / 40.0), else entry
// 21 at ach_position 9; last the item's row gauge (m_summaryGauges[idx -
// the list's top index], reset and set at once to m_percent). The port
// skips the gauge for a row outside the four (JpListBox_GetTopItemIndex is
// a placeholder returning -1).
void FrAchievement::CAhievementDetailInfo::Draw(intptr_t param) {
	if (!m_enabled) {
		return;
	}
	FrGraphicInterface* pGDI = GetGDI();
	FrAchievement* owner = m_pOwner;
	FrListItem* item = (FrListItem*)param;
	if (item == 0) {
		float x = owner->m_achPositions[3].x + owner->GetRect().x;
		float y = owner->m_achPositions[3].y + owner->GetRect().y;
		DrawAchBitmap(pGDI, owner->m_achBitmaps[7], x, y);
		x = (float)owner->m_achBitmaps[7]->Width() + x;
		DrawAchBitmap(pGDI, owner->m_achBitmaps[8], x, y);
		x = (float)owner->m_achBitmaps[8]->Width() + x;
		DrawAchBitmap(pGDI, owner->m_achBitmaps[9], x, y);
		owner->m_totalGauge.Draw();
		int category = owner->m_category;
		if (category == -1) {
			return;
		}
		pGDI->SetTextColor(0xffffffff, 0xffffffff);
		pGDI->SetTextStyle(1);
		WPoint pos(owner->GetRect().x + owner->m_achPositions[13].x, owner->m_achPositions[13].y + owner->GetRect().y);
		pGDI->PrintText(pos, 1, MakeStr("%d", owner->m_categoryPoints[category]), 0);
		pGDI->SetTextStyle(0);
		DrawAchBitmap(pGDI, owner->m_challengeTexts[category],
			owner->m_achPositions[15].x + owner->GetRect().x, owner->m_achPositions[15].y + owner->GetRect().y);
		return;
	}

	float itemX = item->pos.x;
	float itemY = item->pos.y;
	float x = owner->m_achPositions[4].x + itemX;
	DrawAchBitmap(pGDI, owner->m_achBitmaps[10], x, itemY);
	x = (float)owner->m_achBitmaps[10]->Width() + x;
	DrawAchBitmap(pGDI, owner->m_achBitmaps[11], x, itemY);
	x = (float)owner->m_achBitmaps[11]->Width() + x;
	DrawAchBitmap(pGDI, owner->m_achBitmaps[12], x, itemY);

	const sAchElementItem* element = (const sAchElementItem*)item->pData;
	const sAchievementData* data = element->m_pData;
	int32_t current = element->m_currentStep;
	if (current > -1) {
		float textX = owner->m_achPositions[5].x + itemX;
		float textY = owner->m_achPositions[5].y + itemY;
		pGDI->SetTextStyle(1);
		pGDI->SetTextColor(0xff000000, 0xffffffff);
		WPoint pos(textX, (float)(textY + 2.0));
		pGDI->PrintText(pos, 0, data->m_names[current], 0);
		pGDI->SetTextColor(0xffffffff, 0xffffffff);
		pos.x = (float)(itemX + 510.0);
		pos.y = (float)(itemY + 51.0);
		if (element->m_pRecord) {
			pGDI->PrintText(pos, 1, MakeStr("%d", element->m_pRecord->GetCounterValue(current)), 0);
		} else {
			pGDI->PrintText(pos, 1, "0", 0);
		}
		pGDI->SetTextStyle(0);
		pGDI->SetTextColor(0xff000000, 0xffffffff);
		pos.x = textX;
		pos.y = (float)(textY + 22.0);
		if (element->m_pStep) {
			pGDI->PrintText(pos, 0, element->m_pStep->m_name, 0);
		}
		const Bitmap* button = element->m_shown ? owner->m_achBitmaps[24] : owner->m_achBitmaps[23];
		DrawAchBitmap(pGDI, button, owner->m_achPositions[6].x + itemX - (float)button->Width(),
			(float)(owner->m_achPositions[6].y + itemY + 12.0));
	}

	const Bitmap* icon = element->m_pIcon;
	if (current > 0) {
		if (icon) {
			DrawAchBitmap(pGDI, icon, owner->m_achPositions[8].x + itemX, owner->m_achPositions[8].y + itemY);
		}
		DrawAchBitmap(pGDI, owner->m_achBitmaps[22], owner->m_achPositions[9].x + itemX, owner->m_achPositions[9].y + itemY);
		float unitX = owner->m_achPositions[6].x + itemX;
		float unitY = owner->m_achPositions[6].y + itemY;
		DrawAchBitmap(pGDI, owner->m_achBitmaps[20], unitX, unitY);
		pGDI->SetTextColor(0xff000000, 0xffffffff);
		owner->m_pOwned[1]->Print((float)(unitX + 40.0), (float)(unitY + 15.0),
			MakeStr("%d", element->m_points), (eTITLEFONTALINE)1, -5.0f, 1.0f);
		owner->m_pOwned[2]->Print((float)(unitX + 37.0), (float)(unitY + 40.0),
			MakeStr("%d.%d.%d", element->m_date.wYear, element->m_date.wMonth, element->m_date.wDay), (eTITLEFONTALINE)1, -0.5f, 1.0f);
	} else {
		if (icon) {
			DrawAchBitmap(pGDI, icon, owner->m_achPositions[8].x + itemX, owner->m_achPositions[8].y + itemY);
		}
		DrawAchBitmap(pGDI, owner->m_achBitmaps[21], owner->m_achPositions[9].x + itemX, owner->m_achPositions[9].y + itemY);
	}

	int row = item->idx - JpListBox_GetTopItemIndex(owner->m_pAchElementList);
	if (row < 0 || row >= 4) {
		return;
	}
	JpMaskGauge& gauge = owner->m_summaryGauges[row];
	gauge.Reset();
	gauge.SetTargetNow(element->m_percent);
	gauge.Draw();
}

// 0x4e3e10: an open element's step popup (param: its list_ach_element
// item). The box (ach_bitmap 14, 15, 16 side by side from ach_position 14
// past the item), then from the step list's corner (PlaceStepList's 9.0 /
// 70.0) each reached step (ach_bitmap 17, or 19 for a special one) with
// its value ("%d", 32.0 / 47.0 into it, white, text style 1) and, unless
// every step is reached, the others from m_currentStep (ach_bitmap 18 or
// 20), each 3.0 past the previous.
void FrAchievement::CAhievementPopupInfo::Draw(intptr_t param) {
	FrListItem* item = (FrListItem*)param;
	if (!m_enabled || item == 0 || item->pData == 0) {
		return;
	}
	FrGraphicInterface* pGDI = GetGDI();
	const sAchElementItem* element = (const sAchElementItem*)item->pData;
	if (element->m_shown == 0) {
		return;
	}
	float itemX = item->pos.x;
	float itemY = item->pos.y;
	float x = m_pOwner->m_achPositions[14].x + itemX;
	float y = m_pOwner->m_achPositions[14].y + itemY;
	for (int b = 13; b < 16; b++) {
		const Bitmap* pBox = m_pOwner->m_achBitmaps[b];
		WRect dest;
		dest.x = x;
		dest.y = y;
		dest.w = (float)pBox->Width();
		dest.h = (float)pBox->Height();
		pGDI->DrawTexture(pBox, dest, 0xffffffff, 0);
		x = (float)pBox->Width() + x;
	}

	x = (float)(element->m_stepListX + itemX + 9.0);
	y = (float)(itemY + 70.0);
	pGDI->SetTextColor(0xffffffff, 0xffffffff);
	pGDI->SetTextStyle(1);
	bool last = false;
	int reached;
	if (element->m_reachedStep != (int)element->m_steps.size() - 1) {
		reached = element->m_currentStep;
	} else {
		last = true;
		reached = element->m_reachedStep + 1;
	}
	if (reached > 0) {
		float textY = (float)(y + 47.0);
		for (int k = 0; k < reached; k++) {
			const sAchStepSlot& slot = element->m_steps[k];
			const Bitmap* pStep = slot.m_special != 0 ? m_pOwner->m_achBitmaps[18] : m_pOwner->m_achBitmaps[16];
			WRect dest;
			dest.x = x;
			dest.y = y;
			dest.w = (float)pStep->Width();
			dest.h = (float)pStep->Height();
			pGDI->DrawTexture(pStep, dest, 0xffffffff, 0);
			WPoint pos((float)(x + 32.0), textY);
			pGDI->Print11(pos, 1, MakeStr("%d", slot.m_value));
			x = (float)(pStep->Width() + 3.0 + x);
		}
	}
	pGDI->SetTextStyle(0);
	if (last) {
		return;
	}
	for (int k = element->m_currentStep; k < (int)element->m_steps.size(); k++) {
		const Bitmap* pStep = element->m_steps[k].m_special != 0 ? m_pOwner->m_achBitmaps[19] : m_pOwner->m_achBitmaps[17];
		WRect dest;
		dest.x = x;
		dest.y = y;
		dest.w = (float)pStep->Width();
		dest.h = (float)pStep->Height();
		pGDI->DrawTexture(pStep, dest, 0xffffffff, 0);
		x = (float)(pStep->Width() + 3.0 + x);
	}
}

// 0x4e5230: the summary page. The total points (m_text, font +0x774,
// centred) at ach_position 7, the six recent icons from ach_position 10
// (each 16.0 past the previous, under ach_bitmap 23 drawn 14.0 up-left),
// then each category's points ("%d", 76.0 apart) from ach_position 12; the
// positions are past area_summary's rect.
void FrAchievement::CAhievementMainContents::Draw(intptr_t param) {
	if (!m_enabled) {
		return;
	}
	DrawBase();
	FrAchievement* owner = m_pOwner;
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	const WRect& area = owner->m_pSummaryArea->GetRect();
	owner->m_pOwned[0]->Print(owner->m_achPositions[7].x + area.x, owner->m_achPositions[7].y + area.y,
		owner->m_text.c_str(), (eTITLEFONTALINE)2, 1.0f, 1.0f);

	float x = owner->m_achPositions[10].x + area.x;
	float y = owner->m_achPositions[10].y + area.y;
	for (int i = 0; i < 6; i++) {
		if (m_pOwner->m_recent[i].m_pData == 0) {
			continue;
		}
		const Bitmap* pIcon = m_pOwner->m_recent[i].m_pIcon;
		if (pIcon == 0) {
			continue;
		}
		WRect dest;
		dest.x = x;
		dest.y = y;
		dest.w = (float)pIcon->Width();
		dest.h = (float)pIcon->Height();
		pGDI->DrawTexture(pIcon, dest, 0xffffffff, 0);
		const Bitmap* pFrame = m_pOwner->m_achBitmaps[22];
		if (pFrame != 0) {
			WRect frame;
			frame.x = (float)(x - 14.0);
			frame.y = (float)(y - 14.0);
			frame.w = (float)pFrame->Width();
			frame.h = (float)pFrame->Height();
			pGDI->DrawTexture(pFrame, frame, 0xffffffff, 0);
		}
		x = (float)(pIcon->Width() + 16.0 + x);
	}

	WPoint pos(area.x + m_pOwner->m_achPositions[12].x, m_pOwner->m_achPositions[12].y + area.y);
	pGDI->SetTextColor(0xff000000, 0xffffffff);
	for (int k = 0; k < 8; k++) {
		pGDI->PrintText(pos, 1, MakeStr("%d", m_pOwner->m_categoryPoints[k]), 0);
		pos.x = (float)(pos.x + 76.0);
	}
}

// Port placeholder (G7): the JP 0xbef700 returns -1 for an empty list,
// else the idx of the item at FrListBox::m_itemTop (protected in the KR
// frlistbox.h, which has no accessor); -1 (no element drawn) until that
// header gains one under JP_PAKS.
int JpListBox_GetTopItemIndex(FrListBox* list) {
	(void)list;
	return -1;
}

// 0x4e6810
void FrAchievement::OnAreaElementLayerDraw(intptr_t param) {
	int top = JpListBox_GetTopItemIndex(m_pAchElementList);
	if (top == -1) {
		return;
	}

	std::list<FrListItem*>::iterator it = m_elements.begin();
	while (it != m_elements.end()) {
		FrListItem* item = *it;
		if (item->idx < top || item->idx >= top + 4) {
			ReleaseElement(item);
			it = m_elements.erase(it);
		} else {
			m_pages[2]->Draw((intptr_t)item);
			++it;
		}
	}
}

// The element list's walk in OnListAchElementClicked (the JP walks the
// list box's items, +0xd0): every element's popup flag cleared.
struct ClearAchElementShown {
	void operator()(void* data) const {
		((sAchElementItem*)data)->m_shown = 0;
	}
};

// 0x4e68b0
void FrAchievement::OnListAchRecentMouseMove(void) {
	WVector mouse = g_input->GetMousePoint();
	int row = GetRecentRowAt(WPoint(mouse.x, mouse.y));
	if (row != -1 && row < 6) {
		const sAchRecentRecord& record = m_recent[row];
		std::string text = MakeStr("%s\n%d.%d.%d", record.m_pData->m_names[record.m_nameIndex],
			record.m_date.wYear, record.m_date.wMonth, record.m_date.wDay);
		m_pAchRecentList->SetToolTipText(text);
	}
}

// 0x4e69d0: the tooltip of the step under the mouse in list_ach_item.
// Reached steps (below m_currentStep, or every step once the last is
// reached): "name\ndescription\nY.M.D" (the record's counter date), plus
// the bonus item's name when the step has one; frame style 0. The others:
// "name\ndescription\n", or with "추가보상 있음" (through Jp_LookupText)
// for a bonus step; frame style 1.
void FrAchievement::OnListAchItemMouseMove(void) {
	FrListItem* item = m_pAchItemList->GetItemUnderCursor();
	if (item == 0) {
		return;
	}
	WVector mouse = g_input->GetMousePoint();
	WPoint point(mouse.x, mouse.y);
	int index = GetStepAt((float)m_achBitmaps[16]->Width(), point);
	if (index == -1 || m_hoverStep == index) {
		return;
	}
	const sAchElementItem* element = (const sAchElementItem*)item->pData;
	if ((unsigned int)index >= element->m_steps.size()) {
		return;
	}
	const std::vector<const sAchievementStepData*>* steps = m_pDoc->FindCategorySteps(element->m_pData->m_id);
	if (steps == 0) {
		return;
	}
	m_hoverStep = index;
	const sAchievementStepData* step = steps->at(index);
	bool last = false;
	if (element->m_reachedStep == (int)element->m_steps.size() - 1) {
		last = true;
	}
	int reached = last ? element->m_reachedStep + 1 : element->m_currentStep;
	unsigned long style;
	if (index < reached) {
		SYSTEMTIME date;
		if (element->m_pRecord == 0) {
			memset(&date, 0, sizeof(date));
		} else {
			Jp_SystemTimeFromTime(&date, (int64_t)*element->m_pRecord->GetCounterCount(index));
		}
		std::string text;
		if (element->m_steps[index].m_hasBonus != 0) {
			const IFF_ITEM_COMMON* bonus = ItemManager()->FindCommonItem(step->m_rewardTypeIds[1]);
			if (bonus != 0) {
				text = MakeStr("%s\n%s\n%d.%d.%d\n%s", AchElement_GetStepName(element, index), step->m_name,
					date.wYear, date.wMonth, date.wDay, bonus->m_name);
			} else {
				text = MakeStr("%s\n%s\n%d.%d.%d", AchElement_GetStepName(element, index), step->m_name,
					date.wYear, date.wMonth, date.wDay);
			}
		} else {
			text = MakeStr("%s\n%s\n%d.%d.%d", AchElement_GetStepName(element, index), step->m_name,
				date.wYear, date.wMonth, date.wDay);
		}
		m_pAchItemList->SetToolTipText(text);
		style = 0;
	} else {
		std::string text;
		if (element->m_steps[index].m_hasBonus != 0) {
			text = MakeStr("%s\n%s\n%s", AchElement_GetStepName(element, index), step->m_name,
				Jp_LookupText("\xc3\xdf\xb0\xa1\xba\xb8\xbb\xf3 \xc0\xd6\xc0\xbd"));
		} else {
			text = MakeStr("%s\n%s\n", AchElement_GetStepName(element, index), step->m_name);
		}
		m_pAchItemList->SetToolTipText(text);
		style = 1;
	}
	m_pAchItemList->SetToolTipFrameStyle(style);
}

// 0x4e6ed0 (and its tail 0x4e7063): a click on an open element closes its
// step list; on another one, every element closes, this one opens (the
// only entry of m_elements) and, when it has a step list, list_ach_item
// is reset (tooltip on, empty text, no step hovered), placed under it
// and given the element as its one item. The JP tests no item under the
// cursor.
void FrAchievement::OnListAchElementClicked(void) {
	FrListItem* item = m_pAchElementList->GetItemUnderCursor();
	sAchElementItem* element = (sAchElementItem*)item->pData;
	if (element->m_shown == 1) {
		ReleaseElement(item);
		for (std::list<FrListItem*>::iterator it = m_elements.begin(); it != m_elements.end(); ++it) {
			if (*it == item) {
				m_elements.erase(it);
				break;
			}
		}
		return;
	}

	m_pAchElementList->OrderItem(ClearAchElementShown());
	element->m_shown = 1;
	m_elements.clear();
	m_elements.push_back(item);
	if (element->m_currentStep == -1) {
		return;
	}
	m_pAchItemList->EnableToolTip(true);
	m_pAchItemList->ClearItem();
	m_pAchItemList->Enable(true);
	m_pAchItemList->SetToolTipText(std::string(""));
	m_hoverStep = -1;
	PlaceStepList(item);
	m_pAchItemList->AddItem(element);
}

// FillRecentRecords' order (the predicate of the JP std::partial_sort
// 0x4e7330): the later counter count first.
static bool IsLaterRecentPair(const std::pair<const JpAchievementRecord*, int32_t>& a, const std::pair<const JpAchievementRecord*, int32_t>& b) {
	return (int64_t)*a.first->GetCounterCount(a.second) > (int64_t)*b.first->GetCounterCount(b.second);
}

// 0x4e7bb0: the pairs {record, counter index} sort only when there are
// more than six. 50.0 and 66.0 are the doubles 0xd05020 / 0xd05028; the
// list's rect starts at ach_position 11 (JP +0x228 / +0x22c) plus the
// form's.
void FrAchievement::FillRecentRecords(void) {
	std::vector<std::pair<const JpAchievementRecord*, int32_t> > pairs;
	const std::map<unsigned long, JpAchievementRecord>& achievements = m_pDoc->GetAchievements();
	for (std::map<unsigned long, JpAchievementRecord>::const_iterator it = achievements.begin(); it != achievements.end(); ++it) {
		int32_t index = it->second.GetLatestCounterIndex();
		if (index != -1) {
			pairs.push_back(std::make_pair(&it->second, index));
		}
	}

	int count = (int)pairs.size();
	if (count > 6) {
		std::partial_sort(pairs.begin(), pairs.begin() + 6, pairs.end(), IsLaterRecentPair);
		count = 6;
	}
	for (int k = 0; k < count; k++) {
		const JpAchievementRecord* record = pairs[k].first;
		const sAchievementData* data = JpSharedDoc_FindAchievementData(record->GetId());
		if (data == 0) {
			continue;
		}
		uint32_t counterId = record->GetCounterId(pairs[k].second);
		for (int32_t i = 0; i < 10; i++) {
			if (data->m_stepIds[i] == counterId) {
				m_recent[k].m_pData = data;
				m_recent[k].m_nameIndex = i;
				m_recent[k].m_pIcon = g_pFresh->GetBitmap(data->m_icon);
				Jp_SystemTimeFromTime(&m_recent[k].m_date, (int64_t)*record->GetCounterCount(pairs[k].second));
			}
		}
	}

	float left = m_achPositions[11].x + GetRect().x;
	float top = m_achPositions[11].y + GetRect().y;
	float height = m_pAchRecentList->GetRect().h;
	int shown = 0;
	for (int k = 0; k < 6; k++) {
		if (m_recent[k].m_pData) {
			shown++;
		}
	}
	float x = left;
	for (int j = 0; j < shown; j++) {
		m_recentSpans[j][0] = x;
		m_recentSpans[j][1] = (float)(x + 50.0);
		x = (float)(x + 66.0);
	}
	m_recentSpanCount = shown;
	WRect rect;
	rect.x = left;
	rect.y = top;
	rect.w = (float)(66.0 * shown);
	rect.h = height;
	m_pAchRecentList->SetRect(rect);
}

// FillCategoryElements' std::equal_range order (0x4e6150; the JP compares
// with a cleared sAchievementData (0x4e2820) holding the category).
struct AchDataCategoryLess {
	bool operator()(const sAchievementData* a, int category) const {
		return a->m_category < category;
	}
	bool operator()(int category, const sAchievementData* a) const {
		return category < a->m_category;
	}
	bool operator()(const sAchievementData* a, const sAchievementData* b) const {
		return a->m_category < b->m_category;
	}
};

// 0x4e8100
void FrAchievement::FillCategoryElements(int category) {
	typedef std::vector<const sAchievementData*>::const_iterator DataIt;
	const std::vector<const sAchievementData*>& data = m_pDoc->GetAchievementData();
	std::pair<DataIt, DataIt> range = std::equal_range(data.begin(), data.end(), category, AchDataCategoryLess());
	m_categoryData.clear();
	if (range.first != range.second) {
		m_categoryData.assign(range.first, range.second);
	}
}

// 0x4e8c20 (the finds of the document's +0x60 and the form's +0x63c maps
// inlined; the achievement map's find 0x4e5d30, the step slots' push_back
// 0x4e49f0 / 0x4e5800, the insert 0x4e8a10). The JP divides by the step's
// goal without a check, and takes the first step of an empty list: the
// port leaves m_pStep NULL and the percent 0 then.
void FrAchievement::LayoutCategoryElements(void) {
	CQuestDocument* doc = m_pDoc;
	m_elementItems.clear();
	const std::map<unsigned long, sAchievementCategory>& categories = doc->GetCategories();
	const std::map<unsigned long, JpAchievementRecord>& achievements = doc->GetAchievements();
	for (std::vector<const sAchievementData*>::const_iterator it = m_categoryData.begin(); it != m_categoryData.end(); ++it) {
		const sAchievementData* data = *it;
		uint32_t id = data->m_id;
		std::map<unsigned long, sAchievementCategory>::const_iterator category = categories.find(id);
		if (category == categories.end()) {
			return;
		}
		if (m_elementItems.find(id) != m_elementItems.end()) {
			return;
		}
		const std::vector<const sAchievementStepData*>& steps = category->second.m_steps;
		int stepCount = (int)steps.size();

		const JpAchievementRecord* record = 0;
		std::map<unsigned long, JpAchievementRecord>::const_iterator found = achievements.find(id);
		if (found != achievements.end()) {
			record = &found->second;
		}

		sAchElementItem item;
		item.m_pRecord = 0;
		item.m_pData = 0;
		item.m_pStep = 0;
		item.m_shown = 0;
		item.m_points = 0;
		item.m_total = 0;
		item.m_percent = 0.0f;
		item.m_stepListX = 0.0f;
		memset(&item.m_date, 0, sizeof(item.m_date));
		item.m_pIcon = 0;
		item.m_reachedStep = -1;
		if (record) {
			item.m_reachedStep = record->GetLastCountedIndex();
		}
		item.m_currentStep = -1;
		uint32_t stepId;
		if (stepCount - 1 == item.m_reachedStep) {
			item.m_currentStep = item.m_reachedStep;
			stepId = record ? record->GetCounterId(item.m_currentStep) : 0;
		} else if (record == 0) {
			stepId = 0;
		} else {
			int32_t open = record->GetFirstOpenIndex();
			if (open == -1) {
				continue;
			}
			if (item.m_reachedStep >= open) {
				open = item.m_reachedStep;
			}
			item.m_currentStep = open;
			stepId = record->GetCounterId(open);
		}

		std::vector<sAchStepSlot> slots;
		for (int i = 0; i < stepCount; i++) {
			const sAchievementStepData* step = steps[i];
			sAchStepSlot slot;
			int rewards = 0;
			slot.m_value = step->m_rewardCounts[0];
			if (slot.m_value != 0) {
				rewards = 1;
			}
			slot.m_hasBonus = step->m_rewardCounts[1];
			if (slot.m_hasBonus != 0) {
				rewards++;
			}
			slot.m_rewardCount3 = step->m_rewardCounts[2];
			if (slot.m_rewardCount3 != 0) {
				rewards++;
			}
			slot.m_special = rewards > 1;
			slots.push_back(slot);
		}
		item.m_steps.swap(slots);

		item.m_pRecord = record;
		item.m_pData = data;
		item.m_shown = 0;
		item.m_points = m_pDoc->GetRecordPoints(record);
		item.m_total = category->second.m_total;
		if (stepId == 0) {
			if (!steps.empty()) {
				item.m_pStep = steps[0];
			}
		} else {
			for (std::vector<const sAchievementStepData*>::const_iterator s = steps.begin(); s != steps.end(); ++s) {
				if ((*s)->m_id == stepId) {
					item.m_pStep = *s;
					break;
				}
			}
		}
		if (record == 0) {
			item.m_percent = 0.0f;
		} else {
			int32_t latest = record->GetLatestCounterIndex();
			if (latest != -1) {
				Jp_SystemTimeFromTime(&item.m_date, (int64_t)*record->GetCounterCount(latest));
			}
			float percent = 0.0f;
			if (item.m_pStep && item.m_pStep->m_goal != 0) {
				float value = (float)record->GetCounterValue(item.m_currentStep);
				percent = (float)(value / item.m_pStep->m_goal * 100.0);
			}
			if (percent < 0.0f) {
				percent = 0.0f;
			} else if (percent > 100.0f) {
				percent = 100.0f;
			}
			item.m_percent = percent;
		}
		item.m_stepListX = GetStepListX((int)item.m_steps.size());
		item.m_pIcon = g_pFresh->GetBitmap(data->m_icon);
		m_elementItems.insert(m_elementItems.end(), std::pair<const uint32_t, sAchElementItem>(data->m_id, item));
	}
}

// The JP's scope object around SelectCategory (name ours): messages 1
// and 2 to the main actor (the current task's vfn +0x30).
class CAchUpdateScope {
public:
	// 0x4e5170
	CAchUpdateScope(void) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
	}
	// 0x4e51d0
	~CAchUpdateScope(void) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
	}
};

// 0x4e92d0: after the element items are added to list_ach_element
// (FrListBox::AddItem 0xbf0b60) the JP calls 0xbf6ae0 on the list's +0x90
// (a JP FrListBox member, not traced): not written. The JP leaves the category unset for a button past 8 (no caller).
void FrAchievement::SelectCategory(int category) {
	CAchUpdateScope scope;
	for (std::list<FrListItem*>::iterator it = m_elements.begin(); it != m_elements.end(); ++it) {
		ReleaseElement(*it);
	}
	m_pAchElementList->ClearItem();
	m_elements.clear();
	for (int i = 0; i < 9; i++) {
		m_pCategoryBtn[i]->SetStatus(FrButton::NORMAL);
	}
	m_pCategoryBtn[category]->SetStatus(FrButton::PRESSED);

	int page = -1;
	switch (category) {
	case 0:
		page = -1;
		break;
	case 1:
		page = 1;
		break;
	case 2:
		page = 0;
		break;
	case 3:
		page = 3;
		break;
	case 4:
		page = 2;
		break;
	case 5:
		page = 4;
		break;
	case 6:
		page = 6;
		break;
	case 7:
		page = 5;
		break;
	case 8:
		page = 7;
		break;
	}
	if (category == 0) {
		m_pAchRecentList->EnableToolTip(true);
		m_pAchRecentList->Enable(true);
	} else {
		m_pAchRecentList->EnableToolTip(false);
		m_pAchRecentList->Enable(false);
	}
	SetCategory(page);
	if (page == -1) {
		return;
	}

	FillCategoryElements(page);
	LayoutCategoryElements();
	for (std::map<uint32_t, sAchElementItem>::iterator it = m_elementItems.begin(); it != m_elementItems.end(); ++it) {
		m_pAchElementList->AddItem(&it->second);
	}

	int32_t total = m_pDoc->GetCategoryTotal(page);
	float value;
	if (total == 0) {
		value = 0.0f;
	} else {
		float ratio = (float)m_categoryPoints[page] / (float)total;
		float percent = (float)(ratio * 100.0);
		value = (float)floor(percent);
	}
	m_totalGauge.SetTargetNow(value);
}

// 0x4e9580
void FrAchievement::OnBtnSummaryClicked(void) {
	SelectCategory(0);
}

// 0x4e9590
void FrAchievement::OnBtnCharClicked(void) {
	SelectCategory(1);
}

// 0x4e95a0
void FrAchievement::OnBtnGameModeClicked(void) {
	SelectCategory(2);
}

// 0x4e95b0
void FrAchievement::OnBtnAdventureClicked(void) {
	SelectCategory(3);
}

// 0x4e95c0
void FrAchievement::OnBtnStatisticsClicked(void) {
	SelectCategory(4);
}

// 0x4e95d0
void FrAchievement::OnBtnItemClicked(void) {
	SelectCategory(5);
}

// 0x4e95e0
void FrAchievement::OnBtnSystemClicked(void) {
	SelectCategory(6);
}

// 0x4e95f0
void FrAchievement::OnBtnChallengeClicked(void) {
	SelectCategory(7);
}

// 0x4e9600
void FrAchievement::OnBtnEventClicked(void) {
	SelectCategory(8);
}

// 0x4e96d0: then the members' implicit destruction (m_text, the list,
// m_pages, the map, m_categoryData, the gauges).
FrAchievement::~FrAchievement(void) {
	for (int i = 2; i >= 0; --i) {
		if (m_pOwned[i] != 0) {
			delete m_pOwned[i];
			m_pOwned[i] = 0;
		}
	}
	for (std::vector<CAhievementPainter*>::iterator it = m_pages.begin(); it != m_pages.end(); ++it) {
		delete *it;
	}
	if (CTaskManager::Instance() != 0) {
		CTaskManager::Instance()->PostMsg(0, "AvatarChat", 0x58, 0, 0, 0, 0);
	}
}

// 0x4e9960: the JP reads each "achievement.lua" array entry with the
// inlined lua_table_helper::GetValueByIndex (0xc24b30 .. 0xc24e60, then
// the string pop 0x483530 or the float pop 0x46e1f0); the JP FrWnd rect
// (+0x34 / +0x38) is the KR GetRect(). The three CAchieve* fonts
// (0x6f4dc0, 0x6f4e80, 0x6f4f40 into m_pOwned) follow the pages.
bool FrAchievement::OnInit(void) {
	m_pages.push_back(new CAhievementMainContents(this));
	m_pages.push_back(new CAhievementDetailInfo(this));
	m_pages.push_back(new CAhievementPopupInfo(this));
	m_pOwned[0] = new CAchieveTotalPointFont;
	m_pOwned[1] = new CAchieveUnitEarnPoint;
	m_pOwned[2] = new CAchievementDate;

	lua_system::lua_script* script = lua_system::lua_script_manger::GetInstance()->CreateScript(false);
	if (script) {
		script->RunFile("achievement.lua");
	}
	{
		lua_system::lua_table_helper table = script->GetTable("ach_bitmap_list");
		int count = table.GetArraySize();
		for (int i = 1; i <= count; ++i) {
			m_achBitmaps[i - 1] = g_pFresh->GetBitmap(table.GetValueByIndex<const char*>(i));
		}
	}
	for (int i = 0; i < 8; ++i) {
		m_challengeTexts[i] = g_pFresh->GetBitmap(MakeStr("Challenge_text_%d", i));
	}
	{
		lua_system::lua_table_helper table = script->GetTable("ach_position");
		int count = table.GetArraySize();
		WPoint* pPos = m_achPositions;
		for (int i = 1; i <= count; i += 2) {
			float x = table.GetValueByIndex<float>(i);
			float y = table.GetValueByIndex<float>(i + 1);
			pPos->x = x;
			pPos->y = y;
			++pPos;
		}
	}
	{
		lua_system::lua_table_helper table = script->GetTable("ach_ui_etc");
		JpMaskGaugePlacement placement;
		placement.unused8 = 0.0f;
		placement.limit = 100.0f;

		float categoryX = table.GetValueByIndex<float>(1);
		float categoryY = table.GetValueByIndex<float>(2);
		float categoryStep = table.GetValueByIndex<float>(3);
		for (int i = 0; i < 8; ++i) {
			placement.x = (float)i * categoryStep + categoryX + GetRect().x;
			placement.y = categoryY + GetRect().y;
			m_categoryGauges[i].Setup("challenges_guage1_mask.tga", "challenges_guage1.tga", placement, 0, true, 1.0f, true, true);
			m_categoryGauges[i].SetValue(0.0f);
			m_categoryGauges[i].SetAnimating(false);
		}

		float summaryX = table.GetValueByIndex<float>(4);
		float summaryY = table.GetValueByIndex<float>(5);
		float summaryStep = table.GetValueByIndex<float>(6);
		for (int i = 0; i < 4; ++i) {
			placement.x = summaryX + GetRect().x;
			placement.y = (float)i * summaryStep + summaryY + GetRect().y;
			m_summaryGauges[i].Setup("challenges_guage3_mask.tga", "challenges_guage3.tga", placement, 0, true, 1.0f, true, true);
			m_summaryGauges[i].SetValue(0.0f);
			m_summaryGauges[i].SetAnimating(false);
		}

		float totalX = table.GetValueByIndex<float>(7);
		float totalY = table.GetValueByIndex<float>(8);
		placement.x = totalX + GetRect().x;
		placement.y = totalY + GetRect().y;
		m_totalGauge.Setup("challenges_guage2_mask.tga", "challenges_guage2.tga", placement, 0, true, 1.0f, true, true);
	}
	lua_system::lua_script_manger::GetInstance()->ReleaseScript(script);

	FillRecentRecords();
	UpdateCategoryGauges();
	m_text = MakeStr("%d", m_pDoc->GetAchievementPoints());
	SelectCategory(0);
	return FrForm::OnInit();
}

// 0x4ea420: the gauges, vectors, map, list and m_text are the members'
// constructors (the JP vector constructor 0xc3c3e5 builds the gauge
// arrays with 0x923410 / 0x923290).
FrAchievement::FrAchievement(void) {
	for (int i = 0; i < 9; ++i) {
		m_pCategoryBtn[i] = 0;
	}
	m_pAchElementList = 0;
	m_pAchItemList = 0;
	m_pAchRecentList = 0;
	for (int i = 0; i < 25; ++i) {
		m_achBitmaps[i] = 0;
	}
	for (int i = 0; i < 8; ++i) {
		m_challengeTexts[i] = 0;
	}
	for (int i = 0; i < 16; ++i) {
		m_achPositions[i].x = 0.0f;
		m_achPositions[i].y = 0.0f;
	}
	memset(m_recent, 0, sizeof(m_recent));
	memset(m_recentSpans, 0, sizeof(m_recentSpans));
	for (int i = 0; i < 8; ++i) {
		m_categoryPoints[i] = 0;
	}
	m_unknown748 = 0;
	m_hoverStep = 0;
	m_recentSpanCount = 0;
	m_pOwned[0] = 0;
	m_pOwned[1] = 0;
	m_pOwned[2] = 0;
	m_pDoc = 0;
}

// 0x4ea710
IObject* FrAchievementMakeInstance(void) {
	return new FrAchievement;
}

// 0x4ea780 (static init 0xcca4a0)
struct __sFrAchievement {
	__sFrAchievement(void) {
		ObjectFactory().AddObjectFunctor(FrAchievementMakeInstance, "FrAchievement");
	}
};
static __sFrAchievement s_registerFrAchievement;

// --- FrOnedayQuest ----------------------------------------------------------

// 0xe74e5c (static init not traced)
const WRTTI FrOnedayQuest::m_RTTI("FrOnedayQuest", &FrForm::m_RTTI);

// 0x517740
const sFRESH_MSGMAP* FrOnedayQuest::GetMessageMap(void) const {
	return &FrOnedayQuest::_MsgMap;
}

sFRESH_MSGMAP FrOnedayQuest::_MsgMap = { &FrForm::_MsgMap, &FrOnedayQuest::_MsgEntries[0] };

sFRESH_ENTRY FrOnedayQuest::_MsgEntries[] = {
	{ "futurework", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOnedayQuest::OnFutureWorkInit },
	{ "futurework", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOnedayQuest::OnFutureWorkDraw },
	{ "currentwork", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOnedayQuest::OnCurrentWorkInit },
	{ "currentwork", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOnedayQuest::OnCurrentWorkDraw },
	{ "req_today_quest", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOnedayQuest::OnReqTodayQuestInit },
	{ "req_today_quest", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOnedayQuest::OnReqTodayQuestClicked },
	{ "giveup_quest", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOnedayQuest::OnGiveUpQuestInit },
	{ "giveup_quest", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOnedayQuest::OnGiveUpQuestClicked },
	{ "quest_btn1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOnedayQuest::OnQuestBtn1Init },
	{ "quest_btn2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOnedayQuest::OnQuestBtn2Init },
	{ "quest_btn3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOnedayQuest::OnQuestBtn3Init },
	{ "quest_btn1", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOnedayQuest::OnQuestBtn1Clicked },
	{ "quest_btn2", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOnedayQuest::OnQuestBtn2Clicked },
	{ "quest_btn3", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOnedayQuest::OnQuestBtn3Clicked },
	{ "last_gift", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOnedayQuest::OnLastGiftInit },
	{ "last_gift", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOnedayQuest::OnLastGiftClicked },
	{ "close", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOnedayQuest::OnCloseClicked },
	{ "draw_gauge", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOnedayQuest::OnDrawGauge },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x517770
void FrOnedayQuest::OnProc(const float dt) {
	for (int i = 0; i < 4; ++i) {
		m_gauges[i].Update(dt);
	}
}

// 0x517950
void FrOnedayQuest::AddEmptyRows(int list) {
	if (list >= 2) {
		return;
	}
	FrListBox* pList = (list == 0) ? m_pFutureWork : m_pCurrentWork;
	for (int i = 0; i < 3; ++i) {
		pList->AddItem(0);
	}
}

// 0x5179e0
bool FrOnedayQuest::OnScratchResultClosed(int, FrForm*) {
	return true;
}

// 0x5179f0
void FrOnedayQuest::OnCloseClicked(void) {
	// JP vfn +0x68 (slot 26) with (1, 1): the JP FrForm has one virtual
	// more than KR's before OnInit (slot 32 against 31), which puts
	// Close(eFormRet, bool) at slot 26.
	Close(FrOK, true);
}

// 0xe74e68 (static init not traced)
const WRTTI FrOnedayQuestComplete::m_RTTI("FrOnedayQuestComplete", &FrForm::m_RTTI);

// 0x517a00
const sFRESH_MSGMAP* FrOnedayQuestComplete::GetMessageMap(void) const {
	return &FrOnedayQuestComplete::_MsgMap;
}

sFRESH_MSGMAP FrOnedayQuestComplete::_MsgMap = { &FrForm::_MsgMap, &FrOnedayQuestComplete::_MsgEntries[0] };

sFRESH_ENTRY FrOnedayQuestComplete::_MsgEntries[] = {
	{ "subject", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOnedayQuestComplete::OnSubjectInit },
	{ "gift", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOnedayQuestComplete::OnGiftInit },
	{ "bg1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOnedayQuestComplete::OnBg1Init },
	{ "bg1", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOnedayQuestComplete::OnBg1Draw },
	{ "bg2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOnedayQuestComplete::OnBg2Init },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0xe74e70 (static init not traced)
const WRTTI FrOnedayMentionSelect::m_RTTI("FrOnedayMentionSelect", &FrForm::m_RTTI);

// 0x517a40
const sFRESH_MSGMAP* FrOnedayMentionSelect::GetMessageMap(void) const {
	return &FrOnedayMentionSelect::_MsgMap;
}

sFRESH_MSGMAP FrOnedayMentionSelect::_MsgMap = { &FrForm::_MsgMap, &FrOnedayMentionSelect::_MsgEntries[0] };

sFRESH_ENTRY FrOnedayMentionSelect::_MsgEntries[] = {
	{ "arin_yes", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOnedayMentionSelect::OnYesInit },
	{ "arin_no", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOnedayMentionSelect::OnNoInit },
	{ "arin_yes", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOnedayMentionSelect::OnYesClicked },
	{ "arin_no", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOnedayMentionSelect::OnNoClicked },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x517a50
FrOnedayMentionSelect::FrOnedayMentionSelect(void) {
	m_pYes = 0;
	m_pNo = 0;
}

// 0x517a90
FrOnedayMentionSelect::~FrOnedayMentionSelect(void) {
}

// 0x517ab0
bool FrOnedayMentionSelect::OnInit(void) {
	return FrForm::OnInit();
}

// 0x517ac0
void FrOnedayMentionSelect::OnYesClicked(void) {
	Close(FrOK, true);
}

// 0x517ad0
void FrOnedayMentionSelect::OnNoClicked(void) {
	Close(FrNO, true);
}

// 0x5181c0: the std::for_each functor of SendQuestRequest (a lambda
// capturing the packet by reference in the JP): writes the uid of each
// record holding a player quest item.
struct sEncodeQuestUid {
	WSendPacket* m_pPacket;

	explicit sEncodeQuestUid(WSendPacket& packet) : m_pPacket(&packet) {}
	void operator()(const sQuestDocRecord& record) const {
		if (record.m_pPlayerQuest != 0) {
			m_pPacket->Encode4(record.m_pPlayerQuest->GetUid());
		}
	}
};

// 0x518290: the first kept player quest item of that type id.
static std::vector<JpPlayerQuest*>::iterator FindKeptQuest(std::vector<JpPlayerQuest*>::iterator first,
	std::vector<JpPlayerQuest*>::iterator last, unsigned int typeId) {
	for (; first != last; ++first) {
		JpPlayerQuest* quest = *first;
		if (quest->GetTypeId() == typeId && quest->IsKept()) {
			break;
		}
	}
	return first;
}

// 0x5185f0 (name ours; LTCG: the id in eax): a daily quest type id (kind
// 0x1E, subkind 1 or 2).
static bool IsDailyQuestType(uint32_t typeId) {
	if ((typeId & 0xFC000000) != 0x78000000) {
		return false;
	}
	return ((typeId >> 22) & 0xF) - 1 <= 1;
}

// 0x518630
void FrOnedayQuest::OnFutureWorkInit(intptr_t param) {
	m_pFutureWork = DYNAMIC_CAST(FrListBox, param);
}

// 0x5186a0: the row's quest name, or "퀘스트 없음" (no quest, through
// Jp_LookupText) for an empty row. JP +0x34 / +0x38 of the list: its
// rect's origin; JP +0xe4: the row height.
void FrOnedayQuest::OnFutureWorkDraw(intptr_t param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == 0) {
		return;
	}
	FrGraphicInterface* pGDI = GetGDI();
	sOnedayQuestRow* pRow = (sOnedayQuestRow*)pItem->pData;
	FrListBox* pList = m_pFutureWork;
	WPoint pos;
	pos.x = pList->GetRect().x;
	if (pRow != 0) {
		pos.y = (float)((float)(pItem->idx * pList->GetItemHeight()) + pList->GetRect().y + 5.0);
		pGDI->PrintText(pos, 0, pRow->m_pInfo->m_name, 0);
		return;
	}
	pos.y = (float)((float)(pItem->idx * pList->GetItemHeight()) + pList->GetRect().y + 5.0);
	pGDI->PrintText(pos, 0, Jp_LookupText("\xc4\xf9\xbd\xba\xc6\xae \xbe\xf8\xc0\xbd"), 0);
}

// 0x518750
void FrOnedayQuest::OnCurrentWorkInit(intptr_t param) {
	m_pCurrentWork = DYNAMIC_CAST(FrListBox, param);
}

// 0x5187c0: the quest name while in progress, "퀘스트 해결" (solved)
// otherwise, "퀘스트 없음" for an empty row (both through Jp_LookupText).
void FrOnedayQuest::OnCurrentWorkDraw(intptr_t param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == 0) {
		return;
	}
	FrGraphicInterface* pGDI = GetGDI();
	sOnedayQuestRow* pRow = (sOnedayQuestRow*)pItem->pData;
	FrListBox* pList = m_pCurrentWork;
	WPoint pos;
	pos.x = pList->GetRect().x;
	const char* key;
	if (pRow == 0) {
		key = "\xc4\xf9\xbd\xba\xc6\xae \xbe\xf8\xc0\xbd";
	} else {
		if (pRow->m_inProgress != 0) {
			pos.y = (float)((float)(pItem->idx * pList->GetItemHeight()) + pList->GetRect().y + 5.0);
			pGDI->PrintText(pos, 0, pRow->m_pInfo->m_name, 0);
			return;
		}
		key = "\xc4\xf9\xbd\xba\xc6\xae \xc7\xd8\xb0\xe1";
	}
	pos.y = (float)((float)(pItem->idx * pList->GetItemHeight()) + pList->GetRect().y + 5.0);
	pGDI->PrintText(pos, 0, Jp_LookupText(key), 0);
}

// 0x518890
void FrOnedayQuest::OnReqTodayQuestInit(intptr_t param) {
	m_pReqTodayQuest = DYNAMIC_CAST(FrButton, param);
}

// 0x518900
void FrOnedayQuest::OnGiveUpQuestInit(intptr_t param) {
	m_pGiveUpQuest = DYNAMIC_CAST(FrButton, param);
}

// 0x518970
void FrOnedayQuest::OnQuestBtn1Init(intptr_t param) {
	m_pQuestBtn[0] = DYNAMIC_CAST(FrButton, param);
	m_pQuestBtn[0]->Enable(false);
}

// 0x5189e0
void FrOnedayQuest::OnQuestBtn2Init(intptr_t param) {
	m_pQuestBtn[1] = DYNAMIC_CAST(FrButton, param);
	m_pQuestBtn[1]->Enable(false);
}

// 0x518a50
void FrOnedayQuest::OnQuestBtn3Init(intptr_t param) {
	m_pQuestBtn[2] = DYNAMIC_CAST(FrButton, param);
	m_pQuestBtn[2]->Enable(false);
}

// 0x518ac0
void FrOnedayQuest::OnLastGiftInit(intptr_t param) {
	m_pLastGift = DYNAMIC_CAST(FrButton, param);
	m_pLastGift->Enable(false);
}

// --- FrOnedayQuestComplete (its run sits inside FrOnedayQuest's) ---------------

// 0x518b30
bool FrOnedayQuestComplete::HasRewards(void) const {
	return m_rewards.begin() != m_rewards.end();
}

// 0x518b40: 75.0 is the double 0xd076a8. The JP moves through vfn +0x8c
// with the whole rect; KR's MoveWindow takes its origin.
void FrOnedayQuestComplete::ShiftUp(void) {
	WRect rect = GetRect();
	rect.y = (float)(rect.y - 75.0);
	MoveWindow(WPoint(rect.x, rect.y));
}

// 0x518b90
void FrOnedayQuestComplete::ShiftDown(void) {
	WRect rect = GetRect();
	rect.y = (float)(rect.y + 75.0);
	MoveWindow(WPoint(rect.x, rect.y));
}

// 0x518be0
void FrOnedayQuestComplete::OnSubjectInit(intptr_t param) {
	m_pQuestText = DYNAMIC_CAST(FrStatic, param);
}

// 0x518c50
void FrOnedayQuestComplete::OnGiftInit(intptr_t param) {
	m_pRewardText = DYNAMIC_CAST(FrStatic, param);
	m_pRewardText->SetVisible(false);
}

// 0x518cc0
void FrOnedayQuestComplete::OnBg1Init(intptr_t param) {
	m_pBg1 = DYNAMIC_CAST(FrArea, param);
}

// 0x518d30: 50.0 is the double 0xd05020, 70.0 the double 0xd04f58; the JP
// GDI is the manager's +0x84 (0x7e9040).
void FrOnedayQuestComplete::OnBg1Draw(intptr_t) {
	if (m_icons.begin() == m_icons.end()) {
		return;
	}
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	WRect rect = GetRect();
	int count = (int)m_icons.size();
	float x = (float)((rect.x + rect.x + rect.w) * 0.5 - (double)count * 50.0 * 0.5);
	for (int i = 0; i < count; ++i) {
		const Bitmap* pIcon = m_icons[i];
		WRect dest;
		dest.x = x;
		dest.y = (float)(GetRect().y + 70.0);
		dest.w = (float)pIcon->Width();
		dest.h = (float)pIcon->Height();
		pGDI->DrawTexture(pIcon, dest, 0xffffffff, 0);
		x += (float)pIcon->Width();
	}
}

// 0x518e30
void FrOnedayQuestComplete::OnBg2Init(intptr_t param) {
	m_pBg2 = DYNAMIC_CAST(FrArea, param);
}

// 0x518f60
void FrOnedayMentionSelect::OnYesInit(intptr_t param) {
	m_pYes = DYNAMIC_CAST(FrButton, param);
}

// 0x518fd0
void FrOnedayMentionSelect::OnNoInit(intptr_t param) {
	m_pNo = DYNAMIC_CAST(FrButton, param);
}

// --- FrOnedayQuest (continued) ----------------------------------------------

// Inlined in the JP by 0x519810 and 0x51a860: the JP moves through vfn
// +0x8c with the whole rect (y minus the double 0xd04f58, 70.0) and shows
// the items through 0x8eb2e0 (KR FrScratchResultDlg::SetResultItem).
void FrOnedayQuest::OpenRewardResult(unsigned long typeId, int count) {
	sBonusBall items[5];
	memset(items, 0, sizeof(items));
	items[0].m_typeId = typeId;
	items[0].m_count = count;
	items[0].m_kind = 2;
	FrScratchResultDlg* pDlg = CreateForm<FrScratchResultDlg>(g_pFresh->GetManager(), this, "scratchresultdlg", 0);
	WRect rect = pDlg->GetRect();
	rect.y = (float)(rect.y - 70.0);
	pDlg->MoveWindow(WPoint(rect.x, rect.y));
	pDlg->Open((FRESH_RESULT_CALLBACK)&FrOnedayQuest::OnScratchResultClosed, 0xc2);
	pDlg->SetFadeout(false);
	pDlg->SetResultItem(items, 1);
}

// 0x5194b0
FrOnedayMentionSelect* CreateOnedayMentionSelect(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent) {
	return CreateForm<FrOnedayMentionSelect>(manager, owner, name, parent);
}

// 0x519810: the row's gauge becomes the full reward picture, its quest
// button is disabled and the reward is shown; with every quest button
// disabled, giveup_quest is disabled too.
void FrOnedayQuest::OnQuestDone(JpPlayerQuest* quest) {
	sQuestDocRecord* pRecord = m_pDoc->FindShownQuest(quest->GetUid());
	if (pRecord == 0) {
		return;
	}
	FrListItem* pItem = m_pCurrentWork->FindItem(pRecord);
	if (pItem == 0) {
		return;
	}
	int row = pItem->idx;
	JpMaskGauge& gauge = m_gauges[row];
	gauge.Reset();
	gauge.SetOverlays("quest_daily_reward.tga", "quest_gauge1.tga");
	gauge.SetRising(false);
	gauge.SetTarget(100.0f);
	int button = row;
	if (button < 0) {
		button = 0;
	} else if (button > 2) {
		button = 2;
	}
	m_pQuestBtn[button]->Enable(false);
	pRecord->m_pPlayerQuest = 0;
	const sOnedayQuestInfo* pInfo = (const sOnedayQuestInfo*)pRecord->m_pQuestData;
	OpenRewardResult(pInfo->m_rewardTypeId, pInfo->m_rewardCount);
	if (!m_pQuestBtn[0]->IsEnabled() && !m_pQuestBtn[1]->IsEnabled() && !m_pQuestBtn[2]->IsEnabled()) {
		m_pGiveUpQuest->Enable(false);
	}
}

// 0x5199e0
void FrOnedayQuest::SendSelectQuest(int row) {
	FrListItem* pItem = m_pCurrentWork->GetItem(row);
	if (pItem == 0) {
		return;
	}
	const sQuestDocRecord* pRecord = (const sQuestDocRecord*)pItem->pData;
	if (pRecord->m_pPlayerQuest == 0) {
		return;
	}
	WSendPacket packet(0x153);
	packet.Encode4(1);
	packet.Encode4(pRecord->m_pPlayerQuest->GetUid());
	packet.Send(SEND_TO_DEFAULT);
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
}

// 0x519ad0: without a selected row, message 0x23 with "잘못된 아이템 정보를
// 요청하였습니다." (wrong item information requested, through
// Jp_LookupText). Then, while last_gift is disabled, a full reward gauge
// goes back to 0.
bool FrOnedayQuest::OnSelectQuestResult(int ret, FrForm*) {
	if (ret != 1) {
		m_selected = -1;
		return true;
	}
	if (m_selected != -1) {
		SendSelectQuest(m_selected);
		m_selected = -1;
	} else {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText(
			"\xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb \xc1\xa4\xba\xb8\xb8\xa6 "
			"\xbf\xe4\xc3\xbb\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."), 0, 0, 0, 0));
	}
	if (m_pLastGift != 0 && !m_pLastGift->IsEnabled() && m_gauges[3].GetValue() == 100.0) {
		m_gauges[3].SetTarget(0.0f);
	}
	return true;
}

// 0x519bc0: the gauges, then the dates (white, black outline, style 2)
// at the form's origin; the progress date only while the document holds
// quests.
void FrOnedayQuest::OnDrawGauge(intptr_t) {
	for (int i = 0; i < 4; ++i) {
		m_gauges[i].Draw();
	}
	FrGraphicInterface* pGDI = GetGDI();
	unsigned long oldColor = pGDI->GetTextColor();
	pGDI->SetTextColor(0xffffffff, 0xff000000);
	pGDI->SetTextStyle(2);
	WPoint pos(m_dateTodayPos.x + GetRect().x, m_dateTodayPos.y + GetRect().y);
	pGDI->PrintText(pos, 0, m_dateToday.c_str(), 0);
	if (m_pDoc->HasQuests()) {
		pos.x = m_dateProcPos.x + GetRect().x;
		pos.y = m_dateProcPos.y + GetRect().y;
		pGDI->PrintText(pos, 0, m_dateProc.c_str(), 0);
	}
	pGDI->SetTextColor(oldColor, 0xffffffff);
	pGDI->SetTextStyle(0);
}

// 0x51a1c0: the document's quest records in futurework, its shown quest
// records in currentwork; without records req_today_quest is disabled
// and futurework gets empty rows; without shown records giveup_quest and
// the quest buttons are disabled, currentwork gets empty rows and the
// three row gauges go back to the plain picture.
void FrOnedayQuest::RefreshQuests(void) {
	CQuestDocument* pDoc = m_pDoc;
	FrListBox* pFutureWork = m_pFutureWork;
	pFutureWork->ClearItem();
	const std::vector<sQuestDocRecord>& records = pDoc->GetQuestRecords();
	for (std::vector<sQuestDocRecord>::const_iterator it = records.begin(); it != records.end(); ++it) {
		pFutureWork->AddItem((void*)&*it);
	}
	FrListBox* pCurrentWork = m_pCurrentWork;
	pCurrentWork->ClearItem();
	const std::vector<sQuestDocRecord>& shown = pDoc->GetShownQuests();
	for (std::vector<sQuestDocRecord>::const_iterator it = shown.begin(); it != shown.end(); ++it) {
		pCurrentWork->AddItem((void*)&*it);
	}
	if (records.empty()) {
		m_pReqTodayQuest->Enable(false);
		AddEmptyRows(0);
	} else {
		m_pReqTodayQuest->Enable(true);
	}
	if (!shown.empty()) {
		m_pGiveUpQuest->Enable(true);
		return;
	}
	m_pGiveUpQuest->Enable(false);
	m_pQuestBtn[0]->Enable(false);
	m_pQuestBtn[1]->Enable(false);
	m_pQuestBtn[2]->Enable(false);
	AddEmptyRows(1);
	m_gauges[0].Reset();
	m_gauges[1].Reset();
	m_gauges[2].Reset();
	m_gauges[0].SetOverlays("quest_gauge1_mask.tga", "quest_gauge1.tga");
	m_gauges[1].SetOverlays("quest_gauge1_mask.tga", "quest_gauge1.tga");
	m_gauges[2].SetOverlays("quest_gauge1_mask.tga", "quest_gauge1.tga");
}

// 0x51a320: 100.0 is the double 0xd66ca8 and the float 0xcfeec0.
void FrOnedayQuest::RefreshProgress(void) {
	const std::vector<sQuestDocRecord>& shown = m_pDoc->GetShownQuests();
	for (std::vector<sQuestDocRecord>::const_iterator it = shown.begin(); it != shown.end(); ++it) {
		if (it->m_pPlayerQuest == 0) {
			continue;
		}
		FrListItem* pItem = m_pCurrentWork->FindItem((void*)&*it);
		if (pItem == 0) {
			continue;
		}
		int button = pItem->idx;
		if (it->m_pPlayerQuest->GetState() == 4) {
			if (button < 0) {
				button = 0;
			} else if (button > 2) {
				button = 2;
			}
			m_pQuestBtn[button]->Enable(true);
		}
	}
	for (std::vector<sQuestDocRecord>::const_iterator it = shown.begin(); it != shown.end(); ++it) {
		if (it->m_pProgress == 0 || it->m_pPlayerQuest == 0) {
			continue;
		}
		float percent = it->m_pPlayerQuest->GetObjectivePercent(0);
		if (percent < 0.0f) {
			percent = 0.0f;
		} else if (percent > 100.0) {
			percent = 100.0f;
		}
		FrListItem* pItem = m_pCurrentWork->FindItem((void*)&*it);
		if (pItem != 0) {
			m_gauges[pItem->idx].SetTarget(percent);
		}
	}
	if (m_shownQuests.empty()) {
		return;
	}
	std::vector<JpPlayerQuest*>::iterator quest = m_shownQuests.begin();
	for (; quest != m_shownQuests.end(); ++quest) {
		if ((*quest)->GetTypeId() == 0x78800001) {
			break;
		}
	}
	if (quest == m_shownQuests.end()) {
		return;
	}
	float percent = (*quest)->GetObjectiveStepPercent(0);
	for (std::vector<std::pair<unsigned int, FrButton*> >::iterator it = m_typeIds.begin(); it != m_typeIds.end(); ++it) {
		if (it->second == 0) {
			return;
		}
		if (it->second->IsEnabled()) {
			percent = 100.0f;
		}
	}
	if (percent < 0.0f) {
		percent = 0.0f;
	} else if (percent > 100.0) {
		percent = 100.0f;
	}
	m_gauges[3].SetTarget(percent);
}

// 0x51a4e0: a finished quest that is not kept disables its gift button
// and shows it hovered.
void FrOnedayQuest::OnTypedQuestDone(JpPlayerQuest* quest) {
	for (std::vector<std::pair<unsigned int, FrButton*> >::iterator it = m_typeIds.begin(); it != m_typeIds.end(); ++it) {
		if (it->first == quest->GetTypeId()) {
			if (!quest->IsKept()) {
				FrButton* pButton = it->second;
				pButton->Enable(false);
				pButton->SetButtonImg("quest_daily_btn_gift_Hover", FrButton::NORMAL);
			}
			return;
		}
	}
}

// 0x51a550
void FrOnedayQuest::SendQuestRequest(int id, bool questRecords) {
	const std::vector<sQuestDocRecord>& records = questRecords ? m_pDoc->GetQuestRecords() : m_pDoc->GetShownQuests();
	WSendPacket packet(id);
	if (id != 0x154) {
		packet.Encode4((unsigned long)records.size());
		for (std::vector<sQuestDocRecord>::const_iterator it = records.begin(); it != records.end(); ++it) {
			packet.Encode4(it->m_pPlayerQuest->GetUid());
		}
	} else {
		unsigned long count = 0;
		for (std::vector<sQuestDocRecord>::const_iterator it = records.begin(); it != records.end(); ++it) {
			if (it->m_pPlayerQuest != 0) {
				++count;
			}
		}
		packet.Encode4(count);
		std::for_each(records.begin(), records.end(), sEncodeQuestUid(packet));
	}
	packet.Send(SEND_TO_DEFAULT);
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
}

// 0x51a690
void FrOnedayQuest::SendTypedQuestSelect(unsigned int typeId) {
	std::vector<JpPlayerQuest*>::iterator it = m_shownQuests.begin();
	for (; it != m_shownQuests.end(); ++it) {
		if (typeId == (*it)->GetTypeId()) {
			break;
		}
	}
	if (it == m_shownQuests.end()) {
		return;
	}
	WSendPacket packet(0x153);
	packet.Encode4(1);
	packet.Encode4((*it)->GetUid());
	packet.Send(SEND_TO_DEFAULT);
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
}

// 0x51a790
void FrOnedayQuest::SelectQuest(void) {
	FrForm* pForm = CreateOnedayMentionSelect(g_pFresh->GetManager(), this, "oneday_quest_mention", 0);
	pForm->SetMessage(WSingleton<_systemmsg::CMessageManager>::Instance()->GetMsg((_systemmsg::eMsgIdentifier)0x2c3), false);
	pForm->Open((FRESH_RESULT_CALLBACK)&FrOnedayQuest::OnSelectQuestResult, 3);
}

// 0x51a810
bool FrOnedayQuest::IsShownQuest(unsigned int typeId) {
	return FindKeptQuest(m_shownQuests.begin(), m_shownQuests.end(), typeId) != m_shownQuests.end();
}

// 0x51a840: give up the shown quests.
bool FrOnedayQuest::OnGiveUpQuestResult(int ret, FrForm*) {
	if (ret == 1) {
		SendQuestRequest(0x154, false);
	}
	return true;
}

// 0x51a860: with a reward type, the reward gauge goes back to 0 and that
// quest is selected; without one, message 0x23 with "잘못된 아이템 정보를
// 요청하였습니다." (through Jp_LookupText). Then the reward 0x1a0001c4 x1
// is shown.
bool FrOnedayQuest::OnRewardResult(int ret, FrForm*) {
	if (ret != 1) {
		m_rewardTypeId = 0;
		return true;
	}
	if (m_rewardTypeId != 0) {
		m_gauges[3].SetTarget(0.0f);
		SendTypedQuestSelect(m_rewardTypeId);
		m_rewardTypeId = 0;
	} else {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText(
			"\xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb \xc1\xa4\xba\xb8\xb8\xa6 "
			"\xbf\xe4\xc3\xbb\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."), 0, 0, 0, 0));
	}
	OpenRewardResult(0x1a0001c4, 1);
	return true;
}

// 0x51a9d0
void FrOnedayQuest::OnGiveUpQuestClicked(void) {
	FrForm* pForm = CreateOnedayMentionSelect(g_pFresh->GetManager(), this, "oneday_quest_mention", 0);
	pForm->SetMessage(WSingleton<_systemmsg::CMessageManager>::Instance()->GetMsg((_systemmsg::eMsgIdentifier)0x2be), false);
	pForm->Open((FRESH_RESULT_CALLBACK)&FrOnedayQuest::OnGiveUpQuestResult, 3);
}

// 0x51aa50
void FrOnedayQuest::OnQuestBtn1Clicked(void) {
	m_selected = 0;
	SelectQuest();
}

// 0x51aa60
void FrOnedayQuest::OnQuestBtn2Clicked(void) {
	m_selected = 1;
	SelectQuest();
}

// 0x51aa70
void FrOnedayQuest::OnQuestBtn3Clicked(void) {
	m_selected = 2;
	SelectQuest();
}

// 0x51ad00: the JP walks the CSharedDoc +0x3840 list itself (the record
// of objective 0's key; 0x519ee0 is the std::find_if on m_typeIds).
void FrOnedayQuest::EnableReachedGifts(void) {
	for (std::vector<JpPlayerQuest*>::iterator it = m_shownQuests.begin(); it != m_shownQuests.end(); ++it) {
		JpPlayerQuest* quest = *it;
		const sQuestProgress* pProgress = (const sQuestProgress*)JpSharedDoc_FindQuestProgress(quest->GetObjectiveKey(0));
		if (pProgress == 0 || !pProgress->m_active) {
			continue;
		}
		const sOnedayQuestInfo* pInfo = (const sOnedayQuestInfo*)JpSharedDoc_FindQuestData(quest->GetTypeId());
		if (pInfo == 0) {
			continue;
		}
		const int32_t* pGoal = JpSharedDoc_FindAchievementGoal(pInfo->m_achievementId);
		if (pGoal != 0 && pProgress->m_count >= *pGoal && quest->GetState() == 4) {
			unsigned int typeId = quest->GetTypeId();
			for (std::vector<std::pair<unsigned int, FrButton*> >::iterator gift = m_typeIds.begin(); gift != m_typeIds.end(); ++gift) {
				if (gift->first == typeId) {
					gift->second->Enable(true);
					break;
				}
			}
		}
	}
}

// 0x51ae10
void FrOnedayQuest::SendRewardRequest(void) {
	FrForm* pForm = CreateOnedayMentionSelect(g_pFresh->GetManager(), this, "oneday_quest_mention", 0);
	pForm->SetMessage(WSingleton<_systemmsg::CMessageManager>::Instance()->GetMsg((_systemmsg::eMsgIdentifier)0x2c3), false);
	pForm->Open((FRESH_RESULT_CALLBACK)&FrOnedayQuest::OnRewardResult, 3);
}

// 0x51ae90
void FrOnedayQuest::RequestReward(unsigned int typeId) {
	if (!IsShownQuest(typeId)) {
		return;
	}
	m_pLastGift->Enable(false);
	SendRewardRequest();
}

// 0x51b050
void FrOnedayQuest::OnLastGiftClicked(void) {
	m_rewardTypeId = 0x78800001;
	RequestReward(0x78800001);
}

// 0x51b070
FrOnedayQuestComplete::FrOnedayQuestComplete(void) {
	m_questCount = 0;
	m_pQuestText = 0;
	m_pRewardText = 0;
	m_isChallenge = 0;
	m_pBg1 = 0;
	m_pBg2 = 0;
}

// 0x51b120: the members' implicit destruction only.
FrOnedayQuestComplete::~FrOnedayQuestComplete(void) {
}

// 0x51b3f0: request today's quests (the document's quest records); the
// progress date becomes today's.
bool FrOnedayQuest::OnReqTodayQuestResult(int ret, FrForm*) {
	if (ret == 1) {
		SendQuestRequest(0x152, true);
		m_dateProc = m_dateToday;
	}
	return true;
}

// 0x51b430
void FrOnedayQuest::OnReqTodayQuestClicked(void) {
	if (m_pDoc->HasQuests()) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)WSingleton<_systemmsg::CMessageManager>::Instance()->GetMsg((_systemmsg::eMsgIdentifier)0x2bd), 0, 0, 0, 0));
		return;
	}
	FrForm* pForm = CreateOnedayMentionSelect(g_pFresh->GetManager(), this, "oneday_quest_mention", 0);
	pForm->SetMessage(WSingleton<_systemmsg::CMessageManager>::Instance()->GetMsg((_systemmsg::eMsgIdentifier)0x2bc), false);
	pForm->Open((FRESH_RESULT_CALLBACK)&FrOnedayQuest::OnReqTodayQuestResult, 3);
}

// --- FrOnedayQuestComplete (continued) -----------------------------------------

// 0x51b5e0: 5.0f is 0xcfea7c (SetTimeLimit, vfn +0x7c), 15.0 the double
// 0xd04f88, 18.0 the double 0xd07848; the JP moves both texts with
// SetRect (0xbe86e0).
bool FrOnedayQuestComplete::OnInit(void) {
	SetTimeLimit(5.0f);
	if (m_rewards.begin() != m_rewards.end()) {
		if ((m_rewards.front() & 0xFC000000) == 0x4C000000) {
			m_isChallenge = 1;
		}
		if (m_isChallenge == 1) {
			m_pBg1->SetBgImg("challenge_complete_bg1");
			m_pBg2->SetBgImg("challenge_complete_bg2");
			for (std::vector<uint32_t>::iterator it = m_rewards.begin(); it != m_rewards.end(); ++it) {
				const sAchievementData* pData = JpSharedDoc_FindAchievementData(*it);
				if (pData != 0) {
					const Bitmap* pIcon = g_pFresh->GetBitmap(pData->m_icon);
					if (pIcon != 0) {
						m_icons.push_back(pIcon);
					}
				}
			}
			WRect rect = m_pQuestText->GetRect();
			rect.y = (float)(rect.y + 15.0);
			m_pQuestText->SetRect(rect);
			m_pRewardText->SetVisible(true);
			rect = m_pRewardText->GetRect();
			rect.y = (float)(rect.y + 18.0);
			m_pRewardText->SetRect(rect);
		}
	}
	return FrForm::OnInit();
}

// 0x51b760: one line per finished quest ("  <name> <text 0x2c0>"), stopping
// at the first quest without data (the text is then not set); with more
// than one, the text 0x2bf formatted with the count.
void FrOnedayQuestComplete::SetQuests(const std::vector<JpPlayerQuest*>& quests) {
	for (std::vector<JpPlayerQuest*>::const_iterator it = quests.begin(); it != quests.end(); ++it) {
		JpPlayerQuest* quest = *it;
		const sOnedayQuestInfo* pInfo = (const sOnedayQuestInfo*)JpSharedDoc_FindQuestData(quest->GetTypeId());
		if (pInfo == 0) {
			return;
		}
		++m_questCount;
		m_text += MakeStr("  %s %s", pInfo->m_name,
			WSingleton<_systemmsg::CMessageManager>::Instance()->GetMsg((_systemmsg::eMsgIdentifier)0x2c0));
		m_rewards.push_back(quest->GetTypeId());
	}
	if (m_questCount > 1) {
		m_text += MakeStr(WSingleton<_systemmsg::CMessageManager>::Instance()->GetMsg((_systemmsg::eMsgIdentifier)0x2bf), m_questCount);
	}
	m_pQuestText->SetWindowTextA(m_text.c_str());
}

// 0x51b950: the reward formats are the binary's CP949 literals 0xd07894
// ("%s %d<gae> <hoekdeuk>", "<count> acquired"), 0xd07888 ("%d <jeom>",
// "points") and the separator 0xd07890 (two spaces). The type id
// 0x6C000001 is the achievement points: its record name once, the counts
// summed. CItemManager::FindCommonItem's result is not tested.
void FrOnedayQuestComplete::SetRewards(const std::vector<sOnedayQuestReward>& rewards) {
	std::vector<uint32_t> hiddenIds;
	JpSharedDoc_GetHiddenRewardIds(hiddenIds);
	if (rewards.begin() == rewards.end()) {
		return;
	}

	std::string pointName;
	std::string items;
	int32_t points = 0;
	for (std::vector<sOnedayQuestReward>::const_iterator it = rewards.begin(); it != rewards.end(); ++it) {
		sOnedayQuestReward reward = *it;
		if ((reward.first & 0xFC000000) == 0x78000000) {
			continue;
		}
		const sAchievementStepData* pStep = JpSharedDoc_FindAchievementStep(reward.second);
		if (pStep == 0) {
			return;
		}
		++m_questCount;
		m_text += MakeStr("  %s %s", pStep->m_name,
			WSingleton<_systemmsg::CMessageManager>::Instance()->GetMsg((_systemmsg::eMsgIdentifier)0x2c0));
		for (int i = 0; i < 3; i++) {
			uint32_t typeId = pStep->m_rewardTypeIds[i];
			if (typeId == 0) {
				continue;
			}
			const IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(typeId);
			if (typeId == 0x6C000001) {
				if (points == 0) {
					pointName.assign(pItem->m_name);
				}
				points += pStep->m_rewardCounts[i];
			} else if (std::find(hiddenIds.begin(), hiddenIds.end(), typeId) == hiddenIds.end()) {
				items += MakeStr("%s %d\xb0\xb3 \xc8\xb9\xb5\xe6", pItem->m_name, pStep->m_rewardCounts[i]);
				items += "  ";
			}
		}
		m_rewards.push_back(reward.first);
	}

	if (m_questCount > 1) {
		m_text += MakeStr(WSingleton<_systemmsg::CMessageManager>::Instance()->GetMsg((_systemmsg::eMsgIdentifier)0x2c5), m_questCount);
	}
	if (m_rewards.begin() != m_rewards.end()) {
		m_pQuestText->SetWindowTextA(m_text.c_str());
	}

	std::string gift;
	if (!pointName.empty()) {
		pointName += " ";
		pointName += MakeStr("%d \xc1\xa1", points);
		gift.append(pointName);
	}
	if (!items.empty()) {
		if (!gift.empty()) {
			gift += "  ";
		}
		gift.append(items);
	}
	if (m_pRewardText != 0) {
		m_pRewardText->SetWindowTextA(gift.c_str());
	}
}

// --- FrOnedayQuest (continued)----------------------------------------------

// 0x51c0c0
FrOnedayQuest::FrOnedayQuest(void) {
	m_pFutureWork = 0;
	m_pCurrentWork = 0;
	m_pQuestBtn[0] = 0;
	m_pQuestBtn[1] = 0;
	m_pQuestBtn[2] = 0;
	m_pReqTodayQuest = 0;
	m_pGiveUpQuest = 0;
	m_pLastGift = 0;
	m_unknown130 = 0;
	m_unknown268 = 0;
	m_pRewardOverlay = 0;
	m_pDoc = 0;
	m_selected = -1;
	m_rewardTypeId = 0;
}

// 0x51c210
FrOnedayQuest::~FrOnedayQuest(void) {
	if (g_resrcmng != 0 && m_pRewardOverlay != 0) {
		g_resrcmng->Release(m_pRewardOverlay);
		m_pRewardOverlay = 0;
	}
	if (CTaskManager::Instance() != 0) {
		CTaskManager::Instance()->PostMsg(0, "AvatarChat", 0x58, 0, 0, 0, 0);
	}
}

// 0x51c480
IObject* FrOnedayQuestMakeInstance(void) {
	return new FrOnedayQuest;
}

// 0x51c4f0 (static init 0xcccb40)
struct __sFrOnedayQuest {
	__sFrOnedayQuest(void) {
		ObjectFactory().AddObjectFunctor(FrOnedayQuestMakeInstance, "FrOnedayQuest");
	}
};
static __sFrOnedayQuest s_registerFrOnedayQuest;

// 0x51bec0 (name ours; the JP walks the list nodes itself): each daily
// player quest item whose type id is one of the `count` ids.
static void CollectPlayerQuests(std::vector<JpPlayerQuest*>& quests, const uint32_t* typeIds, uint32_t count) {
	for (JpPlayerQuest* quest = JpSharedDoc_FirstPlayerQuest(); quest != 0; quest = JpSharedDoc_NextPlayerQuest(quest)) {
		if (IsDailyQuestType(quest->GetTypeId())) {
			for (uint32_t i = 0; i < count; i++) {
				if (quest->GetTypeId() == typeIds[i]) {
					quests.push_back(quest);
					break;
				}
			}
		}
	}
}

// 0x51c610: the JP reads the integers and floats with its
// lua_table_helper::GetValueByName instances 0x470e60 / 0x470f20 (they
// print "not found '%s' value." for a missing key); the JP FrWnd rect
// (+0x34 / +0x38) is the KR GetRect(). The row gauges are 26 and 52 (the
// doubles 0xd079a8, 0xd079a0) below the first; 100.0f is 0xcfeec0.
bool FrOnedayQuest::OnInit(void) {
	lua_system::lua_script* script = lua_system::lua_script_manger::GetInstance()->CreateScript(false);
	if (script) {
		script->RunFile("onedayquest.lua");
	}
	{
		lua_system::lua_table_helper table = script->GetTable("quest_ui_config");
		float procX = (float)table.GetValueByName<int>("procgaugex");
		float procY = (float)table.GetValueByName<int>("procgaugey");
		float totalX = (float)table.GetValueByName<int>("totalgaugex");
		float totalY = (float)table.GetValueByName<int>("totalgaugey");
		procX = GetRect().x + procX;
		procY = procY + GetRect().y;
		totalX = GetRect().x + totalX;
		totalY = totalY + GetRect().y;
		m_dateTodayPos.x = table.GetValueByName<float>("datetodayx");
		m_dateTodayPos.y = table.GetValueByName<float>("datetodayy");
		m_dateProcPos.x = table.GetValueByName<float>("dateprocx");
		m_dateProcPos.y = table.GetValueByName<float>("dateprocy");
		m_typeIds.push_back(std::pair<unsigned int, FrButton*>(0x78800001, m_pLastGift));
		lua_system::lua_script_manger::GetInstance()->ReleaseScript(script);

		JpMaskGaugePlacement placement;
		placement.unused8 = 0.0f;
		placement.limit = 100.0f;
		placement.x = procX;
		placement.y = procY;
		m_gauges[0].Setup("quest_gauge1_mask.tga", "quest_gauge1.tga", placement, 0, true, 1.0f, true, true);
		placement.x = procX;
		placement.y = (float)(procY + 26.0);
		m_gauges[1].Setup("quest_gauge1_mask.tga", "quest_gauge1.tga", placement, 0, true, 1.0f, true, true);
		placement.x = procX;
		placement.y = (float)(procY + 52.0);
		m_gauges[2].Setup("quest_gauge1_mask.tga", "quest_gauge1.tga", placement, 0, true, 1.0f, true, true);
		placement.x = totalX;
		placement.y = totalY;
		m_gauges[3].Setup("quest_gauge2_mask.tga", "quest_gauge2.tga", placement, 0, true, 1.0f, true, true);
	}
	m_gauges[0].Reset();
	m_gauges[1].Reset();
	m_gauges[2].Reset();
	m_gauges[3].Reset();
	m_pRewardOverlay = g_resrcmng->GetOverlay("quest_daily_reward.tga", 0);

	if (m_pDoc != 0) {
		const SYSTEMTIME& today = m_pDoc->GetDate(0);
		if (today.wYear != 0) {
			m_dateToday.assign(MakeStr("%d-%d-%d", today.wYear, today.wMonth, today.wDay));
		}
		const SYSTEMTIME& proc = m_pDoc->GetDate(1);
		if (proc.wYear != 0) {
			m_dateProc.assign(MakeStr("%d-%d-%d", proc.wYear, proc.wMonth, proc.wDay));
		}
	}
	uint32_t rewardTypeId = 0x78800001;
	CollectPlayerQuests(m_shownQuests, &rewardTypeId, 1);
	if (m_pDoc != 0) {
		RefreshQuests();
		EnableReachedGifts();
		RefreshProgress();
	}
	return FrForm::OnInit();
}

// 0x5ee440
FrOnedayQuest* CreateOnedayQuest(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent) {
	return CreateForm<FrOnedayQuest>(manager, owner, name, parent);
}

// 0x5ee510 (the JP also stores the form in the TLS slot +0xb0)
FrOnedayQuestComplete* CreateOnedayQuestComplete(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent) {
	return CreateForm<FrOnedayQuestComplete>(manager, owner, name, parent);
}

// 0x5ee5e0
FrAchievement* CreateAchievement(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent) {
	return CreateForm<FrAchievement>(manager, owner, name, parent);
}

// 0x6f4dc0 (in the JP TitleFontEx unit)
static char* s_achieveTotalPointFontFile[] = {
	"[challenges_total_score.jpg"
};

CAchieveTotalPointFont::CAchieveTotalPointFont(void) {
	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = &s_achieveTotalPointFontFile[0];
	info.numPages = 1;
	info.fontw = 27;
	info.fonth = 33;
	info.texw = 135;
	info.texh = 66;
	info.pCharSet = "1234567890";

	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

// 0x6f4e80 (in the JP TitleFontEx unit)
static char* s_achieveUnitEarnPointFile[] = {
	"[font_challenges_point.jpg"
};

CAchieveUnitEarnPoint::CAchieveUnitEarnPoint(void) {
	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = &s_achieveUnitEarnPointFile[0];
	info.numPages = 1;
	info.fontw = 24;
	info.fonth = 24;
	info.texw = 240;
	info.texh = 24;
	info.pCharSet = "1234567890";

	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

// 0x6f4f40 (in the JP TitleFontEx unit)
static char* s_achievementDateFile[] = {
	"[font_challenges_date.jpg"
};

CAchievementDate::CAchievementDate(void) {
	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = &s_achievementDateFile[0];
	info.numPages = 1;
	info.fontw = 8;
	info.fonth = 10;
	info.texw = 88;
	info.texh = 10;
	info.pCharSet = "1234567890.";

	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

} // namespace _questoneday
} // namespace _gateway
