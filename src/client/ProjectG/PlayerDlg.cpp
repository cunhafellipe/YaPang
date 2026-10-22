// Original: d:\build\custom\temp\code\client\projectg\playerdlg.cpp
#include "playerdlg.h"
#include "ObjectFactory.h"
#include "fresh.h"
#include "frbutton.h"
#include "fredit.h"
#include "exhibition.h"
#include "commonutil.h"
#include "frarea.h"
#include "frgaugebar.h"
#include "frlistbox.h"
#include "parttidlist.h"
#include "SharedDoc.h"
#include "cardsystem.h"
#include "TaskManager.h"
#include "actor.h"
#include "shared/classdefine.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "InputManager.h"
#include "bitmap.h"
#include "wview.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

// The type id of an equipment entry, for CEquipHelper::MapFindEquipTid
// (playerdlg.h): our structs name it differently (sItemInfo::m_typeId).
static unsigned long EquipTypeId(const sCharacterInfo& info) { return info.typeId; }
static unsigned long EquipTypeId(const sCaddieInfo& info) { return info.typeId; }
static unsigned long EquipTypeId(const sItemInfo& info) { return info.m_typeId; }
static unsigned long EquipTypeId(const sMascotInfo& info) { return info.typeId; }

// playerdlg.cpp:40
CStatBar::CStatBar(float barGap, float barHeight) : m_barGap(barGap), m_barHeight(barHeight) {
	m_bars.reserve(5);

	for (int i = 0; i < 5; i++) {
		sBar bar(MakeStr("stat_gauge_%d", i));
		if (i == 0)
			bar.m_rangeLimit = 50.0f;
		else
			bar.m_rangeLimit = 30.0f;

		m_bars.push_back(bar);
	}

	m_destTime = 0.0f;
}

// playerdlg.cpp:58
CStatBar::~CStatBar(void) {
}

// playerdlg.cpp:64
void CStatBar::SetRange(eLvlType type, int pos, int range, int maxValue) {
	m_bars[type].m_destPos = 0.0f > (float)pos ? 0.0f : (float)pos;
	m_bars[type].m_curPos = 0.0f;
	m_bars[type].m_rangePos = (float)range < m_bars[type].m_rangeLimit ? (float)range : m_bars[type].m_rangeLimit;
	m_bars[type].m_maxValue = maxValue;
}

// playerdlg.cpp:120
void CStatBar::Render(const WPoint& pos) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0)
		return;

	for (int i = 0; i < 5; i++) {
		float index = (float)i;

		float w = m_bars[i].m_curPos / m_bars[i].m_rangeLimit * 100.0f;
		pGDI->DrawTexture(m_bars[i].m_pBitmap, WRect(pos.x, (m_barHeight + m_barGap) * index + pos.y, w, m_barHeight), 0xFFFFFFFF, 0);

		pGDI->SetTextStyle(0);
		pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);

		pGDI->Print(WPoint(pos.x + 100.0f, (m_barHeight + m_barGap) * index + pos.y - 2.0f), 0, "%d/%d", (int)m_bars[i].m_curPos, (int)m_bars[i].m_rangePos);
	}
}

// playerdlg.cpp:72
void CStatBar::SetDestTime(float destTime) {
	m_destTime = destTime;
}

// playerdlg.cpp:77
void CStatBar::Process(float dt) {
	for (int i = 0; i < 5; i++) {
		if (m_bars[i].m_curPos < m_bars[i].m_destPos) {
			if (m_destTime > FLT_EPSILON) {
				m_bars[i].m_curPos += m_bars[i].m_destPos / m_destTime * dt;
			} else {
				m_bars[i].m_curPos = m_bars[i].m_destPos;
			}

			if (m_bars[i].m_curPos > m_bars[i].m_destPos)
				m_bars[i].m_curPos = m_bars[i].m_destPos;
		}

		if (m_bars[i].m_curPos > m_bars[i].m_destPos) {
			if (m_destTime > FLT_EPSILON) {
				m_bars[i].m_curPos -= m_bars[i].m_destPos / m_destTime * dt;
			} else {
				m_bars[i].m_curPos = m_bars[i].m_destPos;
			}

			if (m_bars[i].m_curPos < m_bars[i].m_destPos)
				m_bars[i].m_curPos = m_bars[i].m_destPos;
		}
	}
}

// playerdlg.cpp:113
void CStatBar::SetPos(eLvlType type, int pos, int range, int maxValue) {
	m_bars[type].m_destPos = 0.0f > (float)pos ? 0.0f : (float)pos;
	m_bars[type].m_rangePos = (float)range;
	m_bars[type].m_maxValue = maxValue;
}

// playerdlg.cpp:146 (run-time type and factory registration of FrSubEquipBar)
const WRTTI FrSubEquipBar::m_RTTI("FrSubEquipBar", &FrForm::m_RTTI);

IObject* FrSubEquipBarMakeInstance(void) {
	return new FrSubEquipBar;
}

struct __sFrSubEquipBar {
	__sFrSubEquipBar(void) {
		ObjectFactory().AddObjectFunctor(FrSubEquipBarMakeInstance, "FrSubEquipBar");
	}
};
static __sFrSubEquipBar s_registerFrSubEquipBar;

// playerdlg.cpp:147
const sFRESH_MSGMAP* FrSubEquipBar::GetMessageMap(void) const {
	return &FrSubEquipBar::_MsgMap;
}

sFRESH_MSGMAP FrSubEquipBar::_MsgMap = { &FrForm::_MsgMap, &FrSubEquipBar::_MsgEntries[0] };

sFRESH_ENTRY FrSubEquipBar::_MsgEntries[] = {
	// playerdlg.cpp:149
	{ "sub_equip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSubEquipBar::OnInitSubEquipList },
	// playerdlg.cpp:150
	{ "sub_equip", FRCMD_LBUTTONDOWN, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSubEquipBar::OnDownSubEquipList },
	// playerdlg.cpp:151
	{ "sub_equip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSubEquipBar::OnDrawSubEquipList },
	// playerdlg.cpp:153
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// playerdlg.cpp:156
FrSubEquipBar::FrSubEquipBar(void) {
	m_result = 0;
}

// playerdlg.cpp:161
FrSubEquipBar::~FrSubEquipBar(void) {
}

// playerdlg.cpp:168
void FrSubEquipBar::Initialize(void) {
	if (m_pSubEquipList == NULL)
		return;

	switch (m_subType) {
	case 0:
		m_pSubEquipList->SetItemSize(48, 52);
		break;
	case 1:
		m_pSubEquipList->SetItemSize(40, 40);
		break;
	case 2:
		m_pSubEquipList->SetItemSize(26, 28);
		break;
	case 3:
		m_pSubEquipList->SetItemSize(30, 30);
		break;
	}

	int count = 0;
	float extra = 0.0f;
	switch (m_category) {
	case 1:
		count = WSingleton<CSharedDoc>::Instance()->m_characterMap.size();
		break;
	case 7:
		count = WSingleton<CSharedDoc>::Instance()->m_caddieMap.size() + 1;
		extra = 2.0f;
		break;
	case 4:
		count = WSingleton<CSharedDoc>::Instance()->m_myItemMap.size();
		break;
	case 5:
		count = WSingleton<CSharedDoc>::Instance()->m_myGearList.size();
		break;
	case 16:
		count = WSingleton<CSharedDoc>::Instance()->m_myMascotMap.size() + 1;
		extra = 2.0f;
		break;
	}

	if (m_category == 1) {
		WRect rect = m_rect;
		extra = count * 3.0f + 10.0f;
		rect.w = (float)(m_pSubEquipList->GetItemWidth() * count) + extra + 0.5f;
		rect.h = (float)m_pSubEquipList->GetItemHeight() + 13.0f;
		SetRect(rect);

		rect.x += 8.0f;
		rect.y += 5.0f;
		rect.w = (float)(m_pSubEquipList->GetItemWidth() * count) + extra + 0.5f;
		rect.h = (float)m_pSubEquipList->GetItemHeight();
		m_pSubEquipList->SetRect(rect);
		m_pSubEquipList->Resize(m_pSubEquipList->GetItemWidth() + 2, m_pSubEquipList->GetItemHeight());
	} else {
		if (count > 6) {
			WRect rect = m_rect;
			rect.w = (float)(m_pSubEquipList->GetItemWidth() * 6) + 41.0f;
			rect.h = (float)m_pSubEquipList->GetItemHeight() + 13.0f;
			SetRect(rect);

			rect.x += 5.0f;
			rect.y += 5.0f;
			rect.w = (float)(m_pSubEquipList->GetItemWidth() * 6) + 28.0f;
			rect.h = (float)m_pSubEquipList->GetItemHeight();
			m_pSubEquipList->SetRect(rect);
			m_pSubEquipList->Resize(m_pSubEquipList->GetItemWidth() + 2, m_pSubEquipList->GetItemHeight());
		} else {
			WRect rect = m_rect;
			extra = count * 3.0f + extra + 10.0f;
			rect.w = (float)(m_pSubEquipList->GetItemWidth() * count) + extra + 0.5f;
			rect.h = (float)m_pSubEquipList->GetItemHeight() + 13.0f;
			SetRect(rect);

			rect.x += 8.0f;
			rect.y += 5.0f;
			rect.w = (float)(m_pSubEquipList->GetItemWidth() * count) + extra + 0.5f;
			rect.h = (float)m_pSubEquipList->GetItemHeight();
			m_pSubEquipList->SetRect(rect);
			m_pSubEquipList->Resize(m_pSubEquipList->GetItemWidth() + 2, m_pSubEquipList->GetItemHeight());
		}
	}

	switch (m_category) {
	case 1: {
		std::map<unsigned int, sCharacterInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin();
		while (it != WSingleton<CSharedDoc>::Instance()->m_characterMap.end()) {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.typeId);
			if (pItem != NULL) {
				m_sortList.push_back(_sortitem(pItem, it->first));
			}
			++it;
		}
		break;
	}
	case 7: {
		if (WSingleton<CSharedDoc>::Instance()->m_caddieMap.size() == 0)
			break;

		std::map<unsigned int, sCaddieInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_caddieMap.begin();
		while (it != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end()) {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.typeId);
			if (pItem != NULL) {
				m_sortList.push_back(_sortitem(pItem, it->second.id));
			}
			++it;
		}
		break;
	}
	case 4: {
		std::map<unsigned int, sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myItemMap.begin();
		while (it != WSingleton<CSharedDoc>::Instance()->m_myItemMap.end()) {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.m_typeId);
			if (pItem != NULL) {
				if (((pItem->m_level & 0x7F) >= WSingleton<CSharedDoc>::Instance()->m_myLevel && (pItem->m_level & 0x80)) || !(pItem->m_level & 0x80))
					m_sortList.push_back(_sortitem(pItem, it->second.m_serial));
			}
			++it;
		}
		break;
	}
	case 5: {
		std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myGearList.begin();
		while (it != WSingleton<CSharedDoc>::Instance()->m_myGearList.end()) {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->m_typeId);
			if (pItem != NULL) {
				m_sortList.push_back(_sortitem(pItem, 0));
			}
			++it;
		}
		break;
	}
	case 16: {
		if (WSingleton<CSharedDoc>::Instance()->m_myMascotMap.size() == 0)
			break;

		std::map<unsigned int, sMascotInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myMascotMap.begin();
		while (it != WSingleton<CSharedDoc>::Instance()->m_myMascotMap.end()) {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.typeId);
			if (pItem != NULL) {
				_sortitem item(pItem, it->second.id);
				m_sortList.push_back(item);
			}
			++it;
		}
		break;
	}
	}

	m_sortList.sort();
	std::list<_sortitem>::iterator it = m_sortList.begin();
	while (it != m_sortList.end()) {
		m_pSubEquipList->AddItem(&(*it));
		++it;
	}

	if (m_category == 16 || m_category == 7)
		m_pSubEquipList->AddItem(NULL);
}

// playerdlg.cpp:410
bool FrSubEquipBar::SetClientEquipInfo(PANGYA_ITEM_GROUP group, unsigned long guid) {
	bool result = false;

	switch (group) {
	case 1: {   // character
		std::map<unsigned int, sCharacterInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_characterMap.find(guid);
		if (it != WSingleton<CSharedDoc>::Instance()->m_characterMap.end())
			result = true;
		break;
	}
	case 5: {   // ball
		std::list<sItemInfo>::iterator it = std::find(WSingleton<CSharedDoc>::Instance()->m_myGearList.begin(), WSingleton<CSharedDoc>::Instance()->m_myGearList.end(), guid);
		if (it != WSingleton<CSharedDoc>::Instance()->m_myGearList.end())
			result = true;
		break;
	}
	case 4: {   // club
		std::map<unsigned int, sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myItemMap.find(guid);
		if (it != WSingleton<CSharedDoc>::Instance()->m_myItemMap.end())
			result = true;
		break;
	}
	case 0x10: {   // mascot
		std::map<unsigned int, sMascotInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myMascotMap.find(guid);
		if (it != WSingleton<CSharedDoc>::Instance()->m_myMascotMap.end())
			result = true;
		break;
	}
	case 7: {   // caddie
		std::map<unsigned int, sCaddieInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_caddieMap.find(guid);
		if (it != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end())
			result = true;
		break;
	}
	}

	return result;
}

// playerdlg.cpp:471
void FrSubEquipBar::OnInitSubEquipList(intptr_t param) {
	m_pSubEquipList = DYNAMIC_CAST(FrListBox, param);
	m_pSubEquipList->SetVisible(true);
}

// playerdlg.cpp:478
void FrSubEquipBar::OnDownSubEquipList(PG_MSGARG param) {
	FrListItem* pSelected = m_pSubEquipList->GetSelected();
	if (pSelected == NULL) {
		return;
	}

	_sortitem* pItem = (_sortitem*)pSelected->pData;
	if (pItem != NULL) {
		m_result = pItem->m_pItem;
		m_lastSelectedGuid = pItem->m_guid;
	} else {
		if (m_category != 7 && m_category != 0x10) {
			return;
		}
		m_result = NULL;
		m_lastSelectedGuid = 0;
	}

	FrForm::OnOK();
}

// playerdlg.cpp:507
void FrSubEquipBar::OnDrawSubEquipList(PG_MSGARG param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	FrListItem* pListItem = (FrListItem*)param;
	if (pListItem == NULL || pGDI == NULL)
		return;

	_sortitem* pSort = (_sortitem*)pListItem->pData;
	IFF_ITEM_COMMON* pItem = pSort ? pSort->m_pItem : NULL;
	const Bitmap* pBitmap = NULL;
	if (pItem != NULL) {
		switch (m_subType) {
		case 0:
			pBitmap = g_pFresh->GetBitmap(pItem->m_icon);
			break;
		case 1:
			pBitmap = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->m_icon));
			break;
		case 2:
			pBitmap = g_pFresh->GetBitmap(MakeStr("s_%s", pItem->m_icon));
			break;
		case 3:
			pBitmap = g_pFresh->GetBitmap(MakeStr("bar_%s", pItem->m_icon));
			break;
		}
	} else {
		switch (m_subType) {
		case 0:
			pBitmap = g_pFresh->GetBitmap("caddie_no.tga");
			break;
		case 1:
			pBitmap = g_pFresh->GetBitmap("w_caddie_no");
			break;
		case 2:
			pBitmap = g_pFresh->GetBitmap("s_no");
			break;
		case 3:
			pBitmap = g_pFresh->GetBitmap("bar_no");
			break;
		}
	}

	if (pBitmap == NULL)
		return;

	if (pListItem->idx % m_pSubEquipList->GetColCapacity() == 0) {
		const Bitmap* pBar = g_pFresh->GetBitmap("select_bar_02");
		if (pBar != NULL)
			pGDI->DrawTexture(pBar, m_rect, 0xFFFFFFFF, 0);
	}

	WRect rect(pListItem->pos.x, pListItem->pos.y, (float)pBitmap->Width(), (float)pBitmap->Height());
	pGDI->DrawTexture(pBitmap, rect, 0xFFFFFFFF, 0);

	if (pItem != NULL && (pItem->m_typeId & 0xFC000000) == 0x1C000000) {
		std::map<unsigned int, sCaddieInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_caddieMap.begin();
		std::map<unsigned int, sCaddieInfo>::iterator end = WSingleton<CSharedDoc>::Instance()->m_caddieMap.end();
		for (; it != end; ++it) {
			if (pItem->m_typeId == it->second.typeId) {
				IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(pItem->m_typeId);
				if (pCaddie != NULL) {
					if ((it->second.flags & 2) && it->second.days <= 0 && it->second.cadItemTypeId == 0 && pCaddie->fee != 0) {
						g_view->DrawLine2D(WPoint(rect.x, rect.y + rect.h * 0.5f - 1.0f), WPoint(rect.x + rect.w, rect.y + rect.h * 0.5f - 1.0f), 0xFFFF0000, 0xFFFF0000, 0);
						g_view->DrawLine2D(WPoint(rect.x, rect.y + rect.h * 0.5f), WPoint(rect.x + rect.w, rect.y + rect.h * 0.5f), 0xFFFF0000, 0xFFFF0000, 0);
						g_view->DrawLine2D(WPoint(rect.x, rect.y + rect.h * 0.5f + 1.0f), WPoint(rect.x + rect.w, rect.y + rect.h * 0.5f + 1.0f), 0xFFFF0000, 0xFFFF0000, 0);
					}
				}
				break;
			}
		}
	}

	WPoint pt(g_input->GetMousePoint().x, g_input->GetMousePoint().y);
	if (rect.IsInRect(pt)) {
		const Bitmap* pFrame = g_pFresh->GetBitmap("select_frame");
		if (pFrame != NULL)
			pGDI->DrawTexture(pFrame, WRect(rect.x - 3.0f, rect.y - 3.0f, rect.w + 6.0f, rect.h + 6.0f), 0xFFFFFFFF, 0);
	}
}

// playerdlg.cpp:629
void FrSubEquipBar::OnDraw(void) {
	FrForm::OnDraw();
}

// playerdlg.cpp:634
void FrSubEquipBar::OnProc(const float dt) {
	FrForm::OnProc(dt);
	if (g_input->GetButton(LEFT_BUTTON) == 1) {
		WRect rect = m_rect;
		WPoint pt(g_input->GetMousePoint().x, g_input->GetMousePoint().y);
		if (!(pt.x >= rect.x && pt.x <= rect.x + rect.w && pt.y >= rect.y && rect.y + rect.h >= pt.y))
			FrForm::OnCancel();
	}
}

// playerdlg.cpp:647
void FrSubEquipBar::OnOK(void) {
	FrForm::OnOK();
}

// playerdlg.cpp:652
void FrSubEquipBar::OnCancel(void) {
	FrForm::OnCancel();
}

// playerdlg.cpp:657
void FrSubEquipBar::Open(FRESH_RESULT_CALLBACK pfnResult, eCategory category, const WPoint& pt, eSubType subType) {
	m_category = category;
	m_subType = subType;
	Initialize();
	FrForm::Open(pfnResult, pt, 1);
}

// playerdlg.cpp:741
CEquipHelper::CEquipHelper(void) {
}

// playerdlg.cpp:746
CEquipHelper::~CEquipHelper(void) {
}

// playerdlg.cpp:751
IFF_ITEM_COMMON* CEquipHelper::GetItemCommon(eCategory category, const sUserEquip& equip) {
	unsigned long tid = 0;

	switch (category) {
	case eCategoryCharacter: {
		std::map<unsigned int, sCharacterInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_characterMap.find(equip.characterId);
		if (it != WSingleton<CSharedDoc>::Instance()->m_characterMap.end())
			tid = it->second.typeId;
		break;
	}

	case eCategoryCaddie: {
		std::map<unsigned int, sCaddieInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_caddieMap.find(equip.caddieId);
		if (it != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end())
			tid = it->second.typeId;
		break;
	}

	case eCategoryBall:
		tid = equip.ballTypeId;
		break;

	case eCategoryClub: {
		std::map<unsigned int, sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myItemMap.find(equip.clubSetId);
		if (it != WSingleton<CSharedDoc>::Instance()->m_myItemMap.end())
			tid = it->second.m_typeId;
		break;
	}

	case eCategoryMascot: {
		std::map<unsigned int, sMascotInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myMascotMap.find(equip.mascotId);
		if (it != WSingleton<CSharedDoc>::Instance()->m_myMascotMap.end())
			tid = it->second.typeId;
		break;
	}
	}

	return ItemManager()->FindCommonItem(tid);
}

// playerdlg.cpp:799
unsigned long CEquipHelper::GetEquipItemGuid(eCategory category, bool random) {
	unsigned long guid = 0;

	switch (category) {
	case eCategoryCharacter:
		if (random)
			guid = RandSelectGuid(WSingleton<CSharedDoc>::Instance()->m_characterMap);
		else
			return WSingleton<CSharedDoc>::Instance()->m_myCharacterId;
		break;

	case eCategoryCaddie:
		if (random) {
			unsigned int count = WSingleton<CSharedDoc>::Instance()->m_caddieMap.size();
			if (count == 0)
				break;

			if (rand() % count == 0)
				break;

			guid = RandSelectGuid(WSingleton<CSharedDoc>::Instance()->m_caddieMap);
		} else {
			return WSingleton<CSharedDoc>::Instance()->m_myCaddieId;
		}
		break;

	case eCategoryBall:
		if (random) {
			int count = WSingleton<CSharedDoc>::Instance()->m_myGearList.size();
			if (count == 0)
				return 0;

			std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myGearList.begin();

			for (int skip = rand() % count; skip > 0; skip--)
				++it;

			guid = it->m_typeId;
		} else {
			guid = WSingleton<CSharedDoc>::Instance()->m_myBallId;
		}
		break;

	case eCategoryClub:
		if (random)
			guid = RandSelectGuid(WSingleton<CSharedDoc>::Instance()->m_myItemMap);
		else
			return WSingleton<CSharedDoc>::Instance()->m_myClubId;
		break;

	case eCategoryMascot:
		if (random) {
			unsigned int count = WSingleton<CSharedDoc>::Instance()->m_myMascotMap.size();
			if (count == 0)
				break;

			if (rand() % count == 0)
				break;

			guid = RandSelectGuid(WSingleton<CSharedDoc>::Instance()->m_myMascotMap);
		} else {
			return WSingleton<CSharedDoc>::Instance()->m_myMascotId;
		}
		break;
	}

	return guid;
}

// playerdlg.cpp:885
unsigned long CEquipHelper::GetEquipItemTid(eCategory category, bool random) {
	unsigned long tid = 0;

	switch (category) {
	case eCategoryCharacter:
		if (random)
			tid = MapFindEquipTid(WSingleton<CSharedDoc>::Instance()->m_characterMap, 0);
		else
			tid = MapFindEquipTid(WSingleton<CSharedDoc>::Instance()->m_characterMap, WSingleton<CSharedDoc>::Instance()->m_myCharacterId);
		break;

	case eCategoryCaddie:
		if (random)
			tid = MapFindEquipTid(WSingleton<CSharedDoc>::Instance()->m_caddieMap, 0);
		else
			tid = MapFindEquipTid(WSingleton<CSharedDoc>::Instance()->m_caddieMap, WSingleton<CSharedDoc>::Instance()->m_myCaddieId);
		break;

	case eCategoryBall:
		if (random) {
			int count = WSingleton<CSharedDoc>::Instance()->m_myGearList.size();
			if (count == 0)
				break;

			std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myGearList.begin();

			for (int skip = rand() % count; skip > 0; skip--)
				++it;

			tid = it->m_typeId;
		} else {
			std::list<sItemInfo>& gearList = WSingleton<CSharedDoc>::Instance()->m_myGearList;
			for (std::list<sItemInfo>::iterator it = gearList.begin(); it != gearList.end(); ++it) {
				if (it->m_typeId == WSingleton<CSharedDoc>::Instance()->m_myBallId) {
					tid = it->m_typeId;
					break;
				}
			}
		}
		break;

	case eCategoryClub:
		if (random)
			tid = MapFindEquipTid(WSingleton<CSharedDoc>::Instance()->m_myItemMap, 0);
		else
			tid = MapFindEquipTid(WSingleton<CSharedDoc>::Instance()->m_myItemMap, WSingleton<CSharedDoc>::Instance()->m_myClubId);
		break;

	case eCategoryMascot:
		if (random)
			tid = MapFindEquipTid(WSingleton<CSharedDoc>::Instance()->m_myMascotMap, 0);
		else
			tid = MapFindEquipTid(WSingleton<CSharedDoc>::Instance()->m_myMascotMap, WSingleton<CSharedDoc>::Instance()->m_myMascotId);
		break;
	}

	return tid;
}

// playerdlg.cpp:963
unsigned long CEquipHelper::GetGuidFromTid(eCategory category, unsigned long tid) {
	unsigned long guid = 0;

	switch (category) {
	case eCategoryCharacter: {
		std::map<unsigned int, sCharacterInfo>& characterMap = WSingleton<CSharedDoc>::Instance()->m_characterMap;
		std::map<unsigned int, sCharacterInfo>::const_iterator it = characterMap.begin();
		while (it != characterMap.end()) {
			if (it->second.typeId == tid) {
				guid = it->second.id;
				break;
			}

			++it;
		}
		break;
	}

	case eCategoryCaddie: {
		std::map<unsigned int, sCaddieInfo>& caddieMap = WSingleton<CSharedDoc>::Instance()->m_caddieMap;
		std::map<unsigned int, sCaddieInfo>::const_iterator it = caddieMap.begin();
		while (it != caddieMap.end()) {
			if (it->second.typeId == tid) {
				guid = it->second.id;
				break;
			}

			++it;
		}
		break;
	}

	case eCategoryBall: {
		std::list<sItemInfo>& gearList = WSingleton<CSharedDoc>::Instance()->m_myGearList;
		std::list<sItemInfo>::const_iterator it = gearList.begin();
		while (it != gearList.end()) {
			if (it->m_typeId == tid) {
				guid = it->m_serial;
				break;
			}

			++it;
		}
		break;
	}

	case eCategoryClub: {
		std::map<unsigned int, sItemInfo>& itemMap = WSingleton<CSharedDoc>::Instance()->m_myItemMap;
		std::map<unsigned int, sItemInfo>::const_iterator it = itemMap.begin();
		while (it != itemMap.end()) {
			if (it->second.m_typeId == tid) {
				guid = it->second.m_serial;
				break;
			}

			++it;
		}
		break;
	}

	case eCategoryMascot: {
		std::map<unsigned int, sMascotInfo>& mascotMap = WSingleton<CSharedDoc>::Instance()->m_myMascotMap;
		std::map<unsigned int, sMascotInfo>::const_iterator it = mascotMap.begin();
		while (it != mascotMap.end()) {
			if (it->second.typeId == tid) {
				guid = it->second.id;
				break;
			}

			++it;
		}
		break;
	}
	}

	return guid;
}

// playerdlg.cpp:1051 (run-time type and factory registration of FrPlayerDlg)
const WRTTI FrPlayerDlg::m_RTTI("FrPlayerDlg", &FrForm::m_RTTI);

IObject* FrPlayerDlgMakeInstance(void) {
	return new FrPlayerDlg;
}

struct __sFrPlayerDlg {
	__sFrPlayerDlg(void) {
		ObjectFactory().AddObjectFunctor(FrPlayerDlgMakeInstance, "FrPlayerDlg");
	}
};
static __sFrPlayerDlg s_registerFrPlayerDlg;

// playerdlg.cpp:1053
const sFRESH_MSGMAP* FrPlayerDlg::GetMessageMap(void) const {
	return &FrPlayerDlg::_MsgMap;
}

sFRESH_MSGMAP FrPlayerDlg::_MsgMap = { &FrForm::_MsgMap, &FrPlayerDlg::_MsgEntries[0] };

sFRESH_ENTRY FrPlayerDlg::_MsgEntries[] = {
	// playerdlg.cpp:1055
	{ "base", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnBaseInit },
	// playerdlg.cpp:1056
	{ "base", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPlayerDlg::OnBaseBtnUp },
	// playerdlg.cpp:1057
	{ "select", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnSelectInit },
	// playerdlg.cpp:1058
	{ "pet", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnPetInit },
	// playerdlg.cpp:1059
	{ "pet", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrPlayerDlg::OnPetOwnerDraw },
	// playerdlg.cpp:1060
	{ "charprev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnCharPrevInit },
	// playerdlg.cpp:1061
	{ "charprev", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPlayerDlg::OnCharPrevBtnUp },
	// playerdlg.cpp:1062
	{ "charnext", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnCharNextInit },
	// playerdlg.cpp:1063
	{ "charnext", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPlayerDlg::OnCharNextBtnUp },
	// playerdlg.cpp:1064
	{ "caddie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnCaddieInit },
	// playerdlg.cpp:1065
	{ "caddie_limit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnCaddieLimitInit },
	// playerdlg.cpp:1066
	{ "caddieprev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnCaddiePrevInit },
	// playerdlg.cpp:1067
	{ "caddieprev", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPlayerDlg::OnCaddiePrevBtnUp },
	// playerdlg.cpp:1068
	{ "caddienext", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnCaddieNextInit },
	// playerdlg.cpp:1069
	{ "caddienext", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPlayerDlg::OnCaddieNextBtnUp },
	// playerdlg.cpp:1070
	{ "club", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnClubInit },
	// playerdlg.cpp:1071
	{ "club_limit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnClubLimitInit },
	// playerdlg.cpp:1072
	{ "clubprev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnClubPrevInit },
	// playerdlg.cpp:1073
	{ "clubprev", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPlayerDlg::OnClubPrevBtnUp },
	// playerdlg.cpp:1074
	{ "clubnext", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnClubNextInit },
	// playerdlg.cpp:1075
	{ "clubnext", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPlayerDlg::OnClubNextBtnUp },
	// playerdlg.cpp:1076
	{ "ball", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnBallInit },
	// playerdlg.cpp:1077
	{ "ballprev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnBallPrevInit },
	// playerdlg.cpp:1078
	{ "ballprev", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPlayerDlg::OnBallPrevBtnUp },
	// playerdlg.cpp:1079
	{ "ballnext", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnBallNextInit },
	// playerdlg.cpp:1080
	{ "ballnext", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPlayerDlg::OnBallNextBtnUp },
	// playerdlg.cpp:1081
	{ "name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnNameInit },
	// playerdlg.cpp:1082
	{ "remove", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnRemoveInit },
	// playerdlg.cpp:1083
	{ "remove", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPlayerDlg::OnRemoveBtnUp },
	// playerdlg.cpp:1084
	{ "index", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnIndexInit },
	// playerdlg.cpp:1085
	{ "powerbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnPowerBarInit },
	// playerdlg.cpp:1086
	{ "controlbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnControlBarInit },
	// playerdlg.cpp:1087
	{ "impactbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnImpactBarInit },
	// playerdlg.cpp:1088
	{ "spinbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnSpinBarInit },
	// playerdlg.cpp:1089
	{ "curvebar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnCurveBarInit },
	// playerdlg.cpp:1090
	{ "epower", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnPowerEditInit },
	// playerdlg.cpp:1091
	{ "econtrol", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnControlEditInit },
	// playerdlg.cpp:1092
	{ "eimpact", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnImpactEditInit },
	// playerdlg.cpp:1093
	{ "espin", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnSpinEditInit },
	// playerdlg.cpp:1094
	{ "ecurve", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerDlg::OnCurveEditInit },
	// playerdlg.cpp:1096
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// playerdlg.cpp:1099
FrPlayerDlg::~FrPlayerDlg(void) {
	if (m_pExhibition != NULL) { delete m_pExhibition; m_pExhibition = NULL; } if (m_pPartTidList != NULL) { delete m_pPartTidList; m_pPartTidList = NULL; }
}

// playerdlg.cpp:1104
void FrPlayerDlg::Init(int index) {
	m_playerIndex = index;

	if (m_pIndexArea != NULL)
		m_pIndexArea->SetBgImg(MakeStr("%dp", index + 1));

	if (m_pRemoveBtn != NULL)
		m_pRemoveBtn->SetVisible(m_playerIndex >= 2);

	SetVisible(strlen(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_nickname) != 0);
}

// playerdlg.cpp:1117
void FrPlayerDlg::ShowPet(bool show) {
	HidePet();

	if (m_pPetArea == NULL)
		return;

	if (strlen(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_nickname) == 0)
		return;

	WRect rect = m_pPetArea->GetRect();

	m_pExhibition = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.35f, true);
	m_pExhibition->SetDistanceRange(15.0f, 25.0f);
	m_pExhibition->SetDistance(20.0f, true, false);
	m_pExhibition->SetArea(rect.w, rect.h);

	m_pPartTidList = new CPartTidList;

	if (show)
		SetRandomState();
	else
		SetState();
}

// playerdlg.cpp:1142
void FrPlayerDlg::HidePet(void) {
	if (m_pExhibition != NULL) { delete m_pExhibition; m_pExhibition = NULL; }
	if (m_pPartTidList != NULL) { delete m_pPartTidList; m_pPartTidList = NULL; }
}

// playerdlg.cpp:1148
void FrPlayerDlg::SetState(void) {
	int length = strlen(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_nickname);
	if (length != 0) {
		if (m_pNameEdit)
			m_pNameEdit->SetLine(1, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_nickname, 0, false, 0);

		SetCharControl(eButtonDirCurrent);
		SetCaddieControl(eButtonDirCurrent);
		SetClubControl(eButtonDirCurrent);
		SetBallControl(eButtonDirCurrent);
	}
}

// playerdlg.cpp:1162
void FrPlayerDlg::SetRandomState(void) {
	if (m_pNameEdit)
		m_pNameEdit->SetLine(1, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_nickname, 0, false, 0);

	SetCharControl(eButtonDirRandom);
	SetCaddieControl(eButtonDirRandom);
	SetClubControl(eButtonDirRandom);
	SetBallControl(eButtonDirRandom);
}

// playerdlg.cpp:1173
void FrPlayerDlg::OnCustomProc(float dt) {
	if (m_pExhibition != NULL) {
		m_pExhibition->Process(dt, true);
	}
}

// playerdlg.cpp:1179
void FrPlayerDlg::OnPetOwnerDraw(void) {
	if (m_pExhibition != NULL) {
		m_pExhibition->Display(0, 0);
	}
}

// playerdlg.cpp:1185
void FrPlayerDlg::SetTeam(unsigned char team) {
	m_team = team;

	if (m_pTeamBtn == NULL) {
		return;
	}
	switch (team) {
	case 0:
		m_pTeamBtn->SetButtonImg("team_base_red", FrButton::NORMAL);
		break;
	case 1:
		m_pTeamBtn->SetButtonImg("team_base_blue", FrButton::NORMAL);
		break;
	case 2:
		m_pTeamBtn->SetButtonImg("team_base", FrButton::NORMAL);
		break;
	}
}

// playerdlg.cpp:1208
void FrPlayerDlg::OnBaseInit(intptr_t param) {
	m_pTeamBtn = DYNAMIC_CAST(FrButton, param);
}

// playerdlg.cpp:1213
void FrPlayerDlg::OnBaseBtnUp(void) {
	AfxGetTask()->GetActor("Family") << MsgObject(0, 0xE3, m_playerIndex, 0, 0, 0, 0);
}

// playerdlg.cpp:1218
void FrPlayerDlg::OnSelectInit(intptr_t param) {
	m_pSelectWnd = DYNAMIC_CAST(FrArea, param);
	m_pSelectWnd->SetVisible(false);
}

// playerdlg.cpp:1224
void FrPlayerDlg::Select(void) {
	if (m_pSelectWnd != NULL) {
		m_pSelectWnd->SetVisible(true);
	}
}

// playerdlg.cpp:1230
void FrPlayerDlg::UnSelect(void) {
	if (m_pSelectWnd != NULL) {
		m_pSelectWnd->SetVisible(false);
	}
}

// playerdlg.cpp:1236
void FrPlayerDlg::OnPetInit(PG_MSGARG param) {
	m_pPetArea = DYNAMIC_CAST(FrArea, param);
}

// playerdlg.cpp:1241
void FrPlayerDlg::OnCharPrevInit(PG_MSGARG param) {
	m_pCharPrevBtn = DYNAMIC_CAST(FrButton, param);
	m_pCharPrevBtn->SetPushDelay(0.0f);
}

// playerdlg.cpp:1247
void FrPlayerDlg::OnCharPrevBtnUp(void) {
	std::map<unsigned int, sCharacterInfo>& characterMap = WSingleton<CSharedDoc>::Instance()->m_characterMap;
	if (characterMap.size() != 0) {
		std::map<unsigned int, sCharacterInfo>::const_iterator it = characterMap.find(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_characterId);
		--it;
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_characterId = it->second.id;
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_characterInfo = it->second;
		SetCharacter();
		SetLevelBar();
		if (m_pCharPrevBtn != NULL)
			m_pCharPrevBtn->Enable(it != characterMap.begin());
		if (m_pCharNextBtn != NULL)
			m_pCharNextBtn->Enable(++it != characterMap.end());
	}
}

// playerdlg.cpp:1252
void FrPlayerDlg::OnCharNextInit(PG_MSGARG param) {
	m_pCharNextBtn = DYNAMIC_CAST(FrButton, param);
	m_pCharNextBtn->SetPushDelay(0.0f);
}

// playerdlg.cpp:1258
void FrPlayerDlg::OnCharNextBtnUp(void) {
	std::map<unsigned int, sCharacterInfo>& characterMap = WSingleton<CSharedDoc>::Instance()->m_characterMap;
	if (characterMap.size() != 0) {
		std::map<unsigned int, sCharacterInfo>::const_iterator it = characterMap.find(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_characterId);
		++it;
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_characterId = it->second.id;
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_characterInfo = it->second;
		SetCharacter();
		SetLevelBar();
		if (m_pCharPrevBtn != NULL)
			m_pCharPrevBtn->Enable(it != characterMap.begin());
		if (m_pCharNextBtn != NULL)
			m_pCharNextBtn->Enable(++it != characterMap.end());
	}
}

// playerdlg.cpp:1263
void FrPlayerDlg::OnCaddieInit(PG_MSGARG param) {
	m_pCaddieArea = DYNAMIC_CAST(FrArea, param);
}

// playerdlg.cpp:1268
void FrPlayerDlg::OnCaddieLimitInit(PG_MSGARG param) {
	m_pCaddieLimitArea = DYNAMIC_CAST(FrArea, param);
}

// playerdlg.cpp:1273
void FrPlayerDlg::OnCaddiePrevInit(PG_MSGARG param) {
	m_pCaddiePrevBtn = DYNAMIC_CAST(FrButton, param);
	m_pCaddiePrevBtn->SetPushDelay(0.0f);
}

// playerdlg.cpp:1279
void FrPlayerDlg::OnCaddiePrevBtnUp(void) {
	SetCaddieControl(eButtonDirPrev);
}

// playerdlg.cpp:1284
void FrPlayerDlg::OnCaddieNextInit(PG_MSGARG param) {
	m_pCaddieNextBtn = DYNAMIC_CAST(FrButton, param);
	m_pCaddieNextBtn->SetPushDelay(0.0f);
}

// playerdlg.cpp:1290
void FrPlayerDlg::OnCaddieNextBtnUp(void) {
	SetCaddieControl(eButtonDirNext);
}

// playerdlg.cpp:1295
void FrPlayerDlg::OnClubInit(PG_MSGARG param) {
	m_pClubArea = DYNAMIC_CAST(FrArea, param);
}

// playerdlg.cpp:1300
void FrPlayerDlg::OnClubLimitInit(PG_MSGARG param) {
	m_pClubLimitArea = DYNAMIC_CAST(FrArea, param);
}

// playerdlg.cpp:1305
void FrPlayerDlg::OnClubPrevInit(PG_MSGARG param) {
	m_pClubPrevBtn = DYNAMIC_CAST(FrButton, param);
	m_pClubPrevBtn->SetPushDelay(0.0f);
}

// playerdlg.cpp:1311
void FrPlayerDlg::OnClubPrevBtnUp(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_myItemMap.size() != 0) {
		std::map<unsigned int, sItemInfo>::const_iterator it = WSingleton<CSharedDoc>::Instance()->m_myItemMap.find(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_clubSet.id);
		--it;
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_clubSet.typeId = it->second.m_typeId;
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_clubSet.id = it->second.m_serial;
		memcpy(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_clubSet.statLevel, it->second.m_clubStatLevel, sizeof(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_clubSet.statLevel));
		SetClub(it->second.m_typeId);
		SetLevelBar();
		if (m_pClubPrevBtn != NULL)
			m_pClubPrevBtn->Enable(it != WSingleton<CSharedDoc>::Instance()->m_myItemMap.begin());
		if (m_pClubNextBtn != NULL)
			m_pClubNextBtn->Enable(++it != WSingleton<CSharedDoc>::Instance()->m_myItemMap.end());
	}
}

// playerdlg.cpp:1316
void FrPlayerDlg::OnClubNextInit(PG_MSGARG param) {
	m_pClubNextBtn = DYNAMIC_CAST(FrButton, param);
	m_pClubNextBtn->SetPushDelay(0.0f);
}

// playerdlg.cpp:1322
void FrPlayerDlg::OnClubNextBtnUp(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_myItemMap.size() != 0) {
		std::map<unsigned int, sItemInfo>::const_iterator it = WSingleton<CSharedDoc>::Instance()->m_myItemMap.find(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_clubSet.id);
		++it;
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_clubSet.typeId = it->second.m_typeId;
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_clubSet.id = it->second.m_serial;
		memcpy(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_clubSet.statLevel, it->second.m_clubStatLevel, sizeof(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_clubSet.statLevel));
		SetClub(it->second.m_typeId);
		SetLevelBar();
		if (m_pClubPrevBtn != NULL)
			m_pClubPrevBtn->Enable(it != WSingleton<CSharedDoc>::Instance()->m_myItemMap.begin());
		if (m_pClubNextBtn != NULL)
			m_pClubNextBtn->Enable(++it != WSingleton<CSharedDoc>::Instance()->m_myItemMap.end());
	}
}

// playerdlg.cpp:1327
void FrPlayerDlg::OnBallInit(PG_MSGARG param) {
	m_pBallArea = DYNAMIC_CAST(FrArea, param);
}

// playerdlg.cpp:1332
void FrPlayerDlg::OnBallPrevInit(PG_MSGARG param) {
	m_pBallPrevBtn = DYNAMIC_CAST(FrButton, param);
	m_pBallPrevBtn->SetPushDelay(0.0f);
}

// playerdlg.cpp:1338
void FrPlayerDlg::OnBallPrevBtnUp(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_myGearList.size() != 0) {
		unsigned long& ballId = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_ballId;
		unsigned long typeId = ballId;
		std::list<sItemInfo>& gearList = WSingleton<CSharedDoc>::Instance()->m_myGearList;
		std::list<sItemInfo>::iterator it = gearList.begin();
		while (it != gearList.end() && it->m_typeId != typeId)
			++it;
		--it;
		ballId = it->m_typeId;
		SetBall(it->m_typeId);
		SetLevelBar();
		m_pBallNextBtn->Enable(++it != gearList.end());
		if (m_pBallPrevBtn != NULL)
			m_pBallPrevBtn->Enable(it != gearList.begin());
	}
}

// playerdlg.cpp:1343
void FrPlayerDlg::OnBallNextInit(PG_MSGARG param) {
	m_pBallNextBtn = DYNAMIC_CAST(FrButton, param);
	m_pBallNextBtn->SetPushDelay(0.0f);
}

// playerdlg.cpp:1349
void FrPlayerDlg::OnBallNextBtnUp(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_myGearList.size() != 0) {
		unsigned long& ballId = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_ballId;
		unsigned long typeId = ballId;
		std::list<sItemInfo>& gearList = WSingleton<CSharedDoc>::Instance()->m_myGearList;
		std::list<sItemInfo>::iterator it = gearList.begin();
		while (it != gearList.end() && it->m_typeId != typeId)
			++it;
		++it;
		ballId = it->m_typeId;
		SetBall(it->m_typeId);
		SetLevelBar();
		m_pBallNextBtn->Enable(++it != gearList.end());
		if (m_pBallPrevBtn != NULL)
			m_pBallPrevBtn->Enable(it != gearList.begin());
	}
}

// playerdlg.cpp:1354
void FrPlayerDlg::OnRemoveInit(PG_MSGARG param) {
	m_pRemoveBtn = DYNAMIC_CAST(FrButton, param);
	m_pRemoveBtn->SetPushDelay(0.0f);
}

// playerdlg.cpp:1360
void FrPlayerDlg::OnRemoveBtnUp(void) {
	SetVisible(false);

	if (m_pLinkedWnd) {
		m_pLinkedWnd->SetVisible(true);
	}

	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_nickname[0] = 0;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_name[0] = 0;

	AfxGetTask()->GetActor("Family") << MsgObject(0, 0xE2, m_playerIndex, 0, 0, 0, 0);
}

// playerdlg.cpp:1373
void FrPlayerDlg::OnNameInit(intptr_t param) {
	m_pNameEdit = DYNAMIC_CAST(FrEdit, param);
}

// playerdlg.cpp:1378
const char* FrPlayerDlg::GetName(void) {
	if (m_pNameEdit != NULL) {
		return m_pNameEdit->GetLine(1, false);
	}
	return NULL;
}

// playerdlg.cpp:1386
void FrPlayerDlg::SetName(const char* name) {
	if (m_pNameEdit) {
		m_pNameEdit->SetLine(1, name, 0, false, 0);
	}

	strcpy(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_nickname, name);
	strcpy(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_name, name);
}

// playerdlg.cpp:1395
void FrPlayerDlg::OnIndexInit(intptr_t param) {
	m_pIndexArea = DYNAMIC_CAST(FrArea, param);
}

int CalcPowerPenalty(int power);

template <typename T>
inline T MaxOf(T a, T b) {
	return a < b ? b : a;
}

// playerdlg.cpp:1400
void FrPlayerDlg::SetLevelBar(void) {
	unsigned long charTypeId = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_characterInfo.typeId;
	const char* charLevels = (const char*)&WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_characterInfo.unknown6A;
	// The original recomputes the player's address here (an unsigned index) and reuses the first one for the find below.
	unsigned long clubTypeId = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[(unsigned int)m_playerIndex].m_clubSet.typeId;
	const short* clubLevels = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[(unsigned int)m_playerIndex].m_clubSet.statLevel;
	unsigned long caddieId = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[(unsigned int)m_playerIndex].m_equipHead;
	std::map<unsigned int, sCharacterInfo>::const_iterator charIt = WSingleton<CSharedDoc>::Instance()->m_characterMap.find(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_characterId);
	const unsigned long* parts = charIt->second.partTypeIds;
	const unsigned long* cards = charIt->second.auxParts.tids;
	unsigned long caddieTypeId;
	std::map<unsigned int, sCaddieInfo>::iterator caddieIt;

	if (caddieId == 0) {
		caddieTypeId = caddieId;
	} else {
		caddieIt = WSingleton<CSharedDoc>::Instance()->m_caddieMap.find(caddieId);
		caddieTypeId = caddieIt->second.typeId;
	}

	unsigned char myLevel = WSingleton<CSharedDoc>::Instance()->m_myLevel;
	int penalty = CalcPowerPenalty(myLevel);

	if (IsLocalContent((localContentType_t)0x51)) {
		WSingleton<CCardManager>::Instance()->SetPlayerIndex(0xFF);
		WSingleton<CCardManager>::Instance()->CalcCardPeriodAndStatus();
	}

	for (int i = 0; i < 5; i++) {
		int level = ItemManager()->GetLevel((eLvlType)i, charTypeId, charLevels, parts, cards, clubTypeId, clubLevels, caddieTypeId, myLevel);
		int shown = level;

		switch (i) {
		case 0:
			if (m_pStatGauge[0] != NULL) {
				int overPenalty = level - penalty;
				m_pStatGauge[1]->SetExpand(-MaxOf(0, overPenalty - 20));
				m_pStatGauge[2]->SetExpand(-MaxOf(0, overPenalty - 20));
			}
			break;
		case 1:
		case 2: {
			status_t status = ItemManager()->GetLevelWithPenalty((eLvlType)i, charTypeId, charLevels, parts, cards, clubTypeId, clubLevels, caddieTypeId, myLevel);
			shown = status.m_level;
			break;
		}
		}

		if (m_pLevelEdit[(unsigned char)i] != NULL) {
			m_pLevelEdit[(unsigned char)i]->SetLine(1, MakeStr("\\c0xffff0000\\c%d\\c0xff000000\\c/%d", (unsigned char)shown, (unsigned char)level), 0, false, 0);
		}

		if (m_pStatGauge[i] != NULL) {
			m_pStatGauge[i]->SetDestPos(level, false);
		}
	}
}

// playerdlg.cpp:1484
void FrPlayerDlg::SetLevelNum(unsigned char type, unsigned char level, unsigned char maxLevel) {
	if (m_pLevelEdit[type] != NULL) {
		m_pLevelEdit[type]->SetLine(1, MakeStr("\\c0xffff0000\\c%d\\c0xff000000\\c/%d", level, maxLevel), 0, false, 0);
	}
}

// playerdlg.cpp:1490
void FrPlayerDlg::SetCharControl(eButtonDir dir) {
	std::map<unsigned int, sCharacterInfo>& characterMap = WSingleton<CSharedDoc>::Instance()->m_characterMap;

	if (characterMap.size() == 0)
		return;

	std::map<unsigned int, sCharacterInfo>::const_iterator it;

	if (dir == eButtonDirRandom) {
		unsigned int count = rand() % characterMap.size();

		for (it = characterMap.begin(); it != characterMap.end(); ++it, count--) {
			if (count == 0)
				break;
		}
	} else {
		it = characterMap.find(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_characterId);
	}

	switch (dir) {
	case eButtonDirPrev:
		--it;
		break;
	case eButtonDirNext:
		++it;
		break;
	}

	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_characterId = it->second.id;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_characterInfo = it->second;
	SetCharacter();
	SetLevelBar();

	if (m_pCharPrevBtn != NULL)
		m_pCharPrevBtn->Enable(it != characterMap.begin());

	if (m_pCharNextBtn != NULL)
		m_pCharNextBtn->Enable(++it != characterMap.end());
}

// playerdlg.cpp:1546
void FrPlayerDlg::SetCaddieControl(eButtonDir dir) {
	std::map<unsigned int, sCaddieInfo>& caddieMap = WSingleton<CSharedDoc>::Instance()->m_caddieMap;

	if (caddieMap.size() == 0) {
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_equipHead = 0;
		SetCaddie(0);

		if (m_pCaddiePrevBtn != NULL)
			m_pCaddiePrevBtn->Enable(false);

		if (m_pCaddieNextBtn != NULL)
			m_pCaddieNextBtn->Enable(false);

		return;
	}

	std::map<unsigned int, sCaddieInfo>::const_iterator it;
	unsigned long caddieId;

	if (dir == eButtonDirRandom) {
		caddieId = rand() % caddieMap.size();

		for (it = caddieMap.begin(); it != caddieMap.end(); ++it, caddieId--) {
			if (caddieId == 0)
				break;
		}
	} else {
		caddieId = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_equipHead;
		it = caddieMap.find(caddieId);
	}

	switch (dir) {
	case eButtonDirPrev:
		if (it == caddieMap.begin()) {
			WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_equipHead = 0;
			SetCaddie(0);

			if (m_pCaddiePrevBtn != NULL)
				m_pCaddiePrevBtn->Enable(false);
		} else {
			--it;

			WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_caddieInfo = it->second;
			SetCaddie(it->second.typeId);
		}

		if (m_pCaddieNextBtn != NULL)
			m_pCaddieNextBtn->Enable(true);
		break;

	case eButtonDirNext:
		if (caddieId == 0)
			it = caddieMap.begin();
		else
			++it;

		if (it == caddieMap.end())
			return;

		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_caddieInfo = it->second;
		SetCaddie(it->second.typeId);

		if (m_pCaddiePrevBtn != NULL)
			m_pCaddiePrevBtn->Enable(true);

		if (m_pCaddieNextBtn != NULL)
			m_pCaddieNextBtn->Enable(++it != caddieMap.end());
		break;

	case eButtonDirCurrent:
		if (caddieId == 0) {
			SetCaddie(0);

			if (m_pCaddiePrevBtn != NULL)
				m_pCaddiePrevBtn->Enable(false);

			if (m_pCaddieNextBtn != NULL)
				m_pCaddieNextBtn->Enable(caddieMap.size() > 0);
		} else {
			SetCaddie(it->second.typeId);

			if (m_pCaddiePrevBtn != NULL)
				m_pCaddiePrevBtn->Enable(true);

			if (m_pCaddieNextBtn != NULL)
				m_pCaddieNextBtn->Enable(++it != caddieMap.end());
		}
		break;

	case eButtonDirRandom:
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_caddieInfo = it->second;
		SetCaddie(it->second.typeId);

		if (m_pCaddiePrevBtn != NULL)
			m_pCaddiePrevBtn->Enable(true);

		if (m_pCaddieNextBtn != NULL)
			m_pCaddieNextBtn->Enable(++it != caddieMap.end());
		break;

	default:
		return;
	}

	SetLevelBar();
}

// playerdlg.cpp:1672
void FrPlayerDlg::SetClubControl(eButtonDir dir) {
	if (WSingleton<CSharedDoc>::Instance()->m_myItemMap.size() == 0)
		return;

	std::map<unsigned int, sItemInfo>::iterator it;

	if (dir == eButtonDirRandom) {
		unsigned int count = rand() % WSingleton<CSharedDoc>::Instance()->m_myItemMap.size();

		for (it = WSingleton<CSharedDoc>::Instance()->m_myItemMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myItemMap.end(); ++it, count--) {
			if (count == 0)
				break;
		}
	} else {
		unsigned int clubId = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_clubSet.id;

		it = WSingleton<CSharedDoc>::Instance()->m_myItemMap.find(clubId);
	}

	switch (dir) {
	case eButtonDirPrev:
		--it;
		break;
	case eButtonDirNext:
		++it;
		break;
	}

	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_clubSet.typeId = it->second.m_typeId;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_clubSet.id = it->second.m_serial;
	memcpy(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_clubSet.statLevel, it->second.m_clubStatLevel, sizeof(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_clubSet.statLevel));
	SetClub(it->second.m_typeId);
	SetLevelBar();

	if (m_pClubPrevBtn != NULL)
		m_pClubPrevBtn->Enable(it != WSingleton<CSharedDoc>::Instance()->m_myItemMap.begin());

	if (m_pClubNextBtn != NULL)
		m_pClubNextBtn->Enable(++it != WSingleton<CSharedDoc>::Instance()->m_myItemMap.end());
}

// playerdlg.cpp:1726
void FrPlayerDlg::SetBallControl(eButtonDir dir) {
	std::list<sItemInfo>& gearList = WSingleton<CSharedDoc>::Instance()->m_myGearList;

	if (gearList.size() == 0)
		return;

	std::list<sItemInfo>::iterator it;

	if (dir == eButtonDirRandom) {
		unsigned int count = rand() % gearList.size();

		for (it = gearList.begin(); it != gearList.end(); ++it, count--) {
			if (count == 0)
				break;
		}
	} else {
		unsigned long typeId = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_ballId;

		for (it = gearList.begin(); it != gearList.end(); ++it) {
			if (it->m_typeId == typeId)
				break;
		}
	}

	switch (dir) {
	case eButtonDirPrev:
		--it;
		break;
	case eButtonDirNext:
		++it;
		break;
	}

	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_ballId = it->m_typeId;
	SetBall(it->m_typeId);
	SetLevelBar();

	m_pBallNextBtn->Enable(++it != gearList.end());

	if (m_pBallPrevBtn != NULL)
		m_pBallPrevBtn->Enable(it != gearList.begin());
}

// playerdlg.cpp:1849
void FrPlayerDlg::SetCharacter(void) {
	if (m_pPartTidList != NULL) {
		m_pPartTidList->SetTids(&WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_characterInfo, 0xFF);
	}
	if (m_pExhibition != NULL) {
		m_pExhibition->SetModel(*m_pPartTidList, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_characterInfo.auxParts.tids, 0, 3.0f, m_playerIndex);
	}
}

// playerdlg.cpp:1860
void FrPlayerDlg::SetCaddie(unsigned long typeId) {
	if (m_pCaddieArea == NULL)
		return;

	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_caddieInfo.typeId = typeId;

	IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(typeId);

	if (pCaddie != NULL)
		m_pCaddieArea->SetBgImg(pCaddie->m_icon);
	else
		m_pCaddieArea->SetBgImg("caddie_no");

	if (m_pCaddieLimitArea == NULL)
		return;

	if (pCaddie != NULL) {
		if (WSingleton<CSharedDoc>::Instance()->m_myLevel < (pCaddie->m_level & 0x7F))
			m_pCaddieLimitArea->SetBgImg("unused");
		else
			m_pCaddieLimitArea->SetBgImg("caddie_rest");
	} else {
		m_pCaddieLimitArea->SetVisible(false);
		return;
	}

	unsigned long caddieId = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_caddieInfo.id;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_playerIndex].m_equipHead = caddieId;

	if (caddieId != 0) {
		std::map<unsigned int, sCaddieInfo>& caddieMap = WSingleton<CSharedDoc>::Instance()->m_caddieMap;

		std::map<unsigned int, sCaddieInfo>::const_iterator it = caddieMap.find(caddieId);

		if (it != caddieMap.end()) {
			IFF_STRUCT::sCaddie* pInfo = ItemManager()->FindCaddie(it->second.typeId);

			if (pInfo == NULL)
				return;

			if (((it->second.flags & 2) && it->second.days == 0 && pInfo->fee > 0) || WSingleton<CSharedDoc>::Instance()->m_myLevel < (pInfo->m_level & 0x7F))
				m_pCaddieLimitArea->SetVisible(true);
			else
				m_pCaddieLimitArea->SetVisible(false);
		} else {
			m_pCaddieLimitArea->SetVisible(false);
		}
	} else {
		m_pCaddieLimitArea->SetVisible(false);
	}
}

// playerdlg.cpp:1926
void FrPlayerDlg::SetClub(unsigned long typeId) {
	if (m_pClubArea == NULL) {
		return;
	}

	IFF_STRUCT::sClubSet* pClubSet = ItemManager()->FindClubSet(typeId);

	if (pClubSet == NULL) {
		return;
	}

	m_pClubArea->SetBgImg(pClubSet->m_icon);

	if (m_pClubLimitArea != NULL)
		m_pClubLimitArea->SetVisible(WSingleton<CSharedDoc>::Instance()->m_myLevel < (pClubSet->m_level & 0x7F));
}

// playerdlg.cpp:1942
void FrPlayerDlg::SetBall(unsigned long typeId) {
	if (m_pBallArea == NULL) {
		return;
	}

	IFF_STRUCT::sBall* pBall = ItemManager()->FindBall(typeId);

	if (pBall == NULL) {
		return;
	}

	if (m_pBallArea != NULL) {
		m_pBallArea->SetBgImg(pBall->m_icon);
	}
}

// playerdlg.cpp:1956
void FrPlayerDlg::OnPowerBarInit(intptr_t param) {
	m_pStatGauge[0] = DYNAMIC_CAST(FrGaugeBar, param);
}

// playerdlg.cpp:1961
void FrPlayerDlg::OnControlBarInit(intptr_t param) {
	m_pStatGauge[1] = DYNAMIC_CAST(FrGaugeBar, param);
	m_pStatGauge[1]->SetRange(0, 30, 0);
}

// playerdlg.cpp:1967
void FrPlayerDlg::OnImpactBarInit(intptr_t param) {
	m_pStatGauge[2] = DYNAMIC_CAST(FrGaugeBar, param);
	m_pStatGauge[2]->SetRange(0, 30, 0);
}

// playerdlg.cpp:1973
void FrPlayerDlg::OnSpinBarInit(intptr_t param) {
	m_pStatGauge[3] = DYNAMIC_CAST(FrGaugeBar, param);
	m_pStatGauge[3]->SetRange(0, 30, 0);
}

// playerdlg.cpp:1979
void FrPlayerDlg::OnCurveBarInit(intptr_t param) {
	m_pStatGauge[4] = DYNAMIC_CAST(FrGaugeBar, param);
	m_pStatGauge[4]->SetRange(0, 30, 0);
}

// playerdlg.cpp:1987
void FrPlayerDlg::OnPowerEditInit(PG_MSGARG param) {
	m_pLevelEdit[0] = DYNAMIC_CAST(FrEdit, param);
}

// playerdlg.cpp:1992
void FrPlayerDlg::OnControlEditInit(PG_MSGARG param) {
	m_pLevelEdit[1] = DYNAMIC_CAST(FrEdit, param);
}

// playerdlg.cpp:1997
void FrPlayerDlg::OnImpactEditInit(PG_MSGARG param) {
	m_pLevelEdit[2] = DYNAMIC_CAST(FrEdit, param);
}

// playerdlg.cpp:2002
void FrPlayerDlg::OnSpinEditInit(PG_MSGARG param) {
	m_pLevelEdit[3] = DYNAMIC_CAST(FrEdit, param);
}

// playerdlg.cpp:2007
void FrPlayerDlg::OnCurveEditInit(PG_MSGARG param) {
	m_pLevelEdit[4] = DYNAMIC_CAST(FrEdit, param);
}

// playerdlg.cpp:2014 (run-time type and factory registration of FrPlayerEmptyDlg)
const WRTTI FrPlayerEmptyDlg::m_RTTI("FrPlayerEmptyDlg", &FrForm::m_RTTI);

IObject* FrPlayerEmptyDlgMakeInstance(void) {
	return new FrPlayerEmptyDlg;
}

struct __sFrPlayerEmptyDlg {
	__sFrPlayerEmptyDlg(void) {
		ObjectFactory().AddObjectFunctor(FrPlayerEmptyDlgMakeInstance, "FrPlayerEmptyDlg");
	}
};
static __sFrPlayerEmptyDlg s_registerFrPlayerEmptyDlg;

// playerdlg.cpp:2016
const sFRESH_MSGMAP* FrPlayerEmptyDlg::GetMessageMap(void) const {
	return &FrPlayerEmptyDlg::_MsgMap;
}

sFRESH_MSGMAP FrPlayerEmptyDlg::_MsgMap = { &FrForm::_MsgMap, &FrPlayerEmptyDlg::_MsgEntries[0] };

sFRESH_ENTRY FrPlayerEmptyDlg::_MsgEntries[] = {
	// playerdlg.cpp:2018
	{ "base", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerEmptyDlg::OnBaseInit },
	// playerdlg.cpp:2019
	{ "create", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerEmptyDlg::OnCreateInit },
	// playerdlg.cpp:2020
	{ "create", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPlayerEmptyDlg::OnCreateBtnUp },
	// playerdlg.cpp:2021
	{ "index", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerEmptyDlg::OnIndexInit },
	// playerdlg.cpp:2023
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// playerdlg.cpp:2026
void FrPlayerEmptyDlg::Init(int index) {
	m_index = index;

	if (m_pBgArea != NULL)
		m_pBgArea->SetBgImg(MakeStr("%dp", index + 1));

	SetVisible(strlen(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_index].m_nickname) == 0);
}

// playerdlg.cpp:2036
void FrPlayerEmptyDlg::OnBaseInit(intptr_t param) {
	m_pBaseArea = DYNAMIC_CAST(FrArea, param);
}

// playerdlg.cpp:2041
void FrPlayerEmptyDlg::OnCreateInit(intptr_t param) {
	m_pCreateBtn = DYNAMIC_CAST(FrButton, param);
	m_pCreateBtn->SetPushDelay(0.0f);
}

// playerdlg.cpp:2047
void FrPlayerEmptyDlg::OnCreateBtnUp(void) {
	SetVisible(false);

	if (m_pPlayerDlg != NULL) {
		sprintf(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_index].m_nickname, "PLAYER%d", m_index + 1);
		sprintf(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[m_index].m_name, "PLAYER%d", m_index + 1);
		m_pPlayerDlg->ShowPet(true);
		m_pPlayerDlg->SetVisible(true);
	}

	AfxGetTask()->GetActor("Family") << MsgObject(0, 0xE2, m_index, 1, 0, 0, 0);
	AfxGetTask()->GetActor("Family") << MsgObject(0, 0xE3, m_index, 0, 0, 0, 0);
}

// playerdlg.cpp:2063
void FrPlayerEmptyDlg::OnIndexInit(intptr_t param) {
	m_pBgArea = DYNAMIC_CAST(FrArea, param);
}
