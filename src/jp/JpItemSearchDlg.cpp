// JP-only game code: FrItemSearchDlg, the lobby's item search dialog, and
// the JP CTaskMain's item search members (0x651060, 0x655d10). See
// JpItemSearchDlg.h.
#include "JpItemSearchDlg.h"
#include "JpLobbyGlue.h"

#include "ObjectFactory.h"
#include "TaskMain.h"
#include "TaskManager.h"
#include "fresh.h"
#include "frbutton.h"
#include "frcombobox.h"
#include "fredit.h"
#include "frlistbox.h"
#include "frwnd.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "SharedDoc.h"
#include "UccManager.h"
#include "RFTextManager.h"

// The JP game text of a text key is Jp_LookupText 0xb4eb80
// (RFTextManager.h); the index of a combo string is the JP FrComboBox's
// FindString 0xbff0f0 (the KR FrComboBox::FindString: 1-based, 0 when
// missing); the selection by text is the JP FrComboBox::SelectString 0xbff1d0
// (frcombobox.h, JP_PAKS).

// 0xf145f8: the rect ClearResults resets both icon rects to: a Wangreal WRect
// global, 16 zero bytes in the file (also copied by JpFormBase.cpp and
// RefreshRenderer.cpp); its KR name is not known.
extern const WRect data_f145f8;
const WRect data_f145f8(0.0f, 0.0f, 0.0f, 0.0f);

// lib: 0x584140 (std::list clear), 0x584390, 0x5843a0 (this-adjusting
// deleting-destructor thunks), 0xccf070, 0xccf200, 0xccf330 (static
// initializers)

const WRTTI FrItemSearchDlg::m_RTTI("FrItemSearchDlg", &FrForm::m_RTTI);

// 0xe0cd64 (initializer 0xccf070, name ours): the text keys of the six item
// groups (CP949: clothes, item, caddie, mascot, decoration, room decoration).
static std::string s_itemGroupKeys[6] = {
	"\xc0\xc7\xbb\xf3",
	"\xbe\xc6\xc0\xcc\xc5\xdb",
	"\xc4\xb3\xb5\xf0",
	"\xb8\xb6\xbd\xba\xc4\xda\xc6\xae",
	"\xb2\xd9\xb9\xcc\xb1\xe2",
	"\xb9\xe6\xb2\xd9\xb9\xcc\xb1\xe2",
};

// 0xe75c88 (initializer 0xccf330, name ours): "No item was found." (the
// game text of the CP949 key).
static std::string s_noResultText = Jp_LookupText(
	"\xb0\xcb\xbb\xf6\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");

// 0x5832d0
const sFRESH_MSGMAP* FrItemSearchDlg::GetMessageMap(void) const {
	return &FrItemSearchDlg::_MsgMap;
}

sFRESH_MSGMAP FrItemSearchDlg::_MsgMap = { &FrForm::_MsgMap, &FrItemSearchDlg::_MsgEntries[0] };

sFRESH_ENTRY FrItemSearchDlg::_MsgEntries[] = {
	{ "combo_SearchItemGroup", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemSearchDlg::OnGroupComboInit },
	{ "combo_SearchItemGroup", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrItemSearchDlg::OnGroupComboLBtnDown },
	{ "edit_SearchWord", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemSearchDlg::OnSearchEditInit },
	{ "edit_SearchWord", FRCMD_ENTERKEY, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemSearchDlg::OnSearchEditEnterKey },
	{ "btn_Search", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemSearchDlg::OnSearchButtonInit },
	{ "btn_Search", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrItemSearchDlg::OnSearchButtonLBtnUp },
	{ "list_Result", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemSearchDlg::OnResultListInit },
	{ "list_Result", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrItemSearchDlg::OnResultListLBtnUp },
	{ "list_Result", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemSearchDlg::OnResultListOwnerDraw },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x5833a0
void FrItemSearchDlg::OnGroupComboInit(intptr_t param) {
	m_pGroupCombo = DYNAMIC_CAST(FrComboBox, param);
}

// 0x583410
void FrItemSearchDlg::OnSearchEditInit(intptr_t param) {
	m_pSearchEdit = DYNAMIC_CAST(FrEdit, param);
}

// 0x583480
void FrItemSearchDlg::OnSearchButtonInit(intptr_t param) {
	m_pSearchButton = DYNAMIC_CAST(FrButton, param);
}

// 0x5834f0
void FrItemSearchDlg::OnResultListInit(intptr_t param) {
	m_pResultList = DYNAMIC_CAST(FrListBox, param);
}

// 0x583630
bool FrItemSearchDlg::OnInit(void) {
	m_pSelectedBase = g_pFresh->RegisterBitmap("30_noplayer_base");
	if (m_pGroupCombo) {
		m_pGroupCombo->ClearList();
	}
	for (int i = 0; i < 6; i++) {
		m_pGroupCombo->AddString(Jp_LookupText(s_itemGroupKeys[i].c_str()));
	}
	m_pGroupCombo->SelectString(Jp_LookupText(s_itemGroupKeys[0].c_str()));
	return FrForm::OnInit();
}

// 0x5836c0
void FrItemSearchDlg::SendResultSelected(const sItemSearchResult* pResult) {
	if (!pResult) {
		return;
	}
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x2d6, pResult->typeId, pResult->value4,
		pResult->linkedTypeId, 0, 0));
}

// 0x583730
void FrItemSearchDlg::SendResultChosen(unsigned int typeId, unsigned int value4) {
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x2d7, typeId, value4, 0, 0, 0));
}

// 0x583810
void FrItemSearchDlg::OnResultListLBtnUp(void) {
	FrListItem* pItem = m_pResultList->GetSelected();
	if (!pItem) {
		return;
	}
	sItemSearchResult* pResult = (sItemSearchResult*)pItem->pData;
	SendResultSelected(pResult);
	SendResultChosen(pResult->typeId, pResult->value4);
}

// 0x583a00
void FrItemSearchDlg::AddResults(std::list<sItemSearchResult>& results) {
	if (!m_pResultList) {
		return;
	}
	for (std::list<sItemSearchResult>::iterator it = results.begin(); it != results.end(); ++it) {
		m_pResultList->AddItem(&*it);
	}
}

// 0x583a40
void FrItemSearchDlg::DrawItemIcon(unsigned int typeId, const WPoint* pos) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!typeId) {
		return;
	}
	IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(typeId);
	if (!pItem) {
		return;
	}
	std::string baseName = "";
	// JP IFF +0x49, the item's icon name.
	std::string iconName = pItem->m_icon;
	unsigned int kind = typeId >> 26;
	int characterId = 0xf;
	switch (kind) {
	case 1:
		characterId = typeId & 0x3ffffff;
		break;
	case 2:
		characterId = (typeId >> 18) & 0xff;
		break;
	case 9:
		characterId = (typeId >> 13) & 0xff;
		break;
	case 15:
		characterId = Jp_ItemManager_GetCharacterId(typeId);
		break;
	}
	// Ours: slot 13 of the JP table is NULL and the port's lookup returns
	// NULL for ids past it: no base image then.
	const char* pBaseName = Jp_GetCharacterIconBaseName(characterId);
	if (pBaseName) {
		baseName = pBaseName;
	}

	if (!baseName.empty()) {
		const Bitmap* pBase = g_pFresh->GetBitmap(baseName.c_str());
		if (pBase) {
			m_iconRect.x = pos->x;
			m_iconRect.y = pos->y + 2.0;
			float scale = (float)m_pResultList->GetItemWidth() / ((float)pBase->Height() * 1.4f);
			if (!(scale < 1.0f)) {
				scale = 1.0f;
			}
			m_iconRect.w = (float)pBase->Width() * scale;
			m_iconRect.h = (float)pBase->Height() * scale;
			pGDI->DrawTexture(pBase, m_iconRect, 0xffffffff, 0);
		}
	}

	if (!iconName.empty()) {
		const Bitmap* pIcon = g_pFresh->GetBitmap(iconName.c_str());
		if (pIcon) {
			m_linkedIconRect.x = pos->x;
			m_linkedIconRect.y = pos->y + 2.0;
			float scale = (float)m_pResultList->GetItemWidth() / ((float)pIcon->Height() * 1.4f);
			scale = scale + (kind == 6 ? scale * 0.5 : 0.0);
			if (!(scale < 1.0f)) {
				scale = 1.0f;
			}
			m_linkedIconRect.x = 2.0 * scale + m_iconRect.w + m_linkedIconRect.x;
			m_linkedIconRect.w = (float)pIcon->Width() * scale;
			m_linkedIconRect.h = (float)pIcon->Height() * scale;
			pGDI->DrawTexture(pIcon, m_linkedIconRect, 0xffffffff, 0);
		}
	}
}

// 0x583e40
void FrItemSearchDlg::OnResultListOwnerDraw(intptr_t param) {
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem) {
		return;
	}
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI) {
		return;
	}
	sItemSearchResult* pResult = (sItemSearchResult*)pItem->pData;
	unsigned long oldColor = pGDI->GetTextColor();
	if (!pResult->typeId) {
		return;
	}
	if (pItem->underCursor) {
		WRect rect(pItem->pos.x - 0.0f, pItem->pos.y - 0.0f, 298.0f, 31.0f);
		pGDI->DrawTexture(m_pSelectedBase, rect, 0xffffffff, 0);
	}
	DrawItemIcon(pResult->typeId, &pItem->pos);
	pGDI->SetTextColor(0xff000000, 0xffffffff);
	if (pItem->underCursor == true) {
		pGDI->SetTextColor(0xff0000ff, 0xffffffff);
		pGDI->SetTextStyle(2);
	}
	// JP FrListBox +0xe4 (KR's m_itemSize.w).
	int itemSize = m_pResultList->GetItemWidth();
	float textOffset = m_linkedIconRect.w + m_iconRect.w + 0.0f;
	float textX = pItem->pos.x + textOffset;
	float textY = (float)(itemSize / 2 - pGDI->GetFontHeight11() / 2) + pItem->pos.y;
	pGDI->Print11(WPoint(textX, textY), 0, pResult->name.c_str());
	if (pResult->linkedTypeId) {
		IFF_ITEM_COMMON* pLinked = ItemManager()->FindCommonItem(pResult->linkedTypeId);
		if (pLinked) {
			WPoint iconPos(pGDI->GetTextExtend11(pResult->name.c_str()) + textX + 0.0f, pItem->pos.y);
			DrawItemIcon(pLinked->m_typeId, &iconPos);
			pGDI->Print11(WPoint(iconPos.x + textOffset, textY), 0, pLinked->m_name);
		}
	}
	pGDI->SetTextColor(oldColor, 0xffffffff);
	pGDI->SetTextStyle(0);
}

// 0x584190
void FrItemSearchDlg::ClearResults(void) {
	if (!m_results.empty()) {
		m_results.clear();
	}
	if (m_pResultList && m_pResultList->GetCurrentItemSize(false)) {
		m_pResultList->ClearItem();
	}
	m_iconRect = data_f145f8;
	m_linkedIconRect = data_f145f8;
}

// 0x584250
FrItemSearchDlg::FrItemSearchDlg(void) {
	m_pGroupCombo = 0;
	m_pSearchEdit = 0;
	m_pSearchButton = 0;
	m_pResultList = 0;
	m_iconRect = data_f145f8;
	m_linkedIconRect = data_f145f8;
	m_pSelectedBase = 0;
}

// 0x5843c0
FrItemSearchDlg::~FrItemSearchDlg(void) {
	m_pSelectedBase = 0;
}

// 0x584410
void FrItemSearchDlg::OnGroupComboLBtnDown(void) {
	ClearResults();
}

// 0x584420
void FrItemSearchDlg::Search(std::string& word, int group) {
	ClearResults();
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x2d3, group, (intptr_t)&word, (intptr_t)&m_results, 0, 0));
	if (!m_results.empty()) {
		AddResults(m_results);
		return;
	}
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x24, (intptr_t)s_noResultText.c_str(), 0, 0, 0, 0));
}

// 0x584510
IObject* FrItemSearchDlgMakeInstance(void) {
	return new FrItemSearchDlg;
}

// 0x584580 (static init 0xccf1e0): registration with the form factory
struct __sFrItemSearchDlg {
	__sFrItemSearchDlg(void) {
		ObjectFactory().AddObjectFunctor(FrItemSearchDlgMakeInstance, "FrItemSearchDlg");
	}
};
static __sFrItemSearchDlg s_registerFrItemSearchDlg;

// 0x5845d0
void FrItemSearchDlg::OnSearchButtonLBtnUp(void) {
	ClearResults();
	std::string word = m_pSearchEdit->GetLine(1, false);
	if (!word.empty() && word.size() >= 2) {
		Search(word, m_pGroupCombo->FindString(m_pGroupCombo->GetLine(1, false)) - 1);
	}
}

// 0x5846b0
void FrItemSearchDlg::OnSearchEditEnterKey(intptr_t param) {
	OnSearchButtonLBtnUp();
}

// 0x651060
bool CTaskMain::OnItemSearchDlgResult(int, FrForm*) {
	m_itemSearchDlg = NULL;
	return true;
}

// 0x655d10
void CTaskMain::OpenItemSearchDlg(void) {
	if (m_itemSearchDlg) {
		return;
	}
	m_itemSearchDlg = CreateForm<FrItemSearchDlg>(g_pFresh->GetManager(), this, "ItemSearchDlg", 0);
	m_itemSearchDlg->Open((FRESH_RESULT_CALLBACK)&CTaskMain::OnItemSearchDlgResult, 1);
}
