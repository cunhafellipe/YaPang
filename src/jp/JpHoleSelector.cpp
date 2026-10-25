// JP-only game code: FrHoleSelector, the game room's hole picker
// (JpHoleSelector.h), and its CreateForm instance.
#include "JpHoleSelector.h"

#include "ObjectFactory.h"
#include "SharedDoc.h"
#include "bitmap.h"
#include "commonutil.h"
#include "frarea.h"
#include "fresh.h"
#include "frgraphicinterface.h"
#include "frlistbox.h"
#include "frwnd.h"
#include "frwndmanager.h"
#include "s5/Utilities.h"
#include "woverlay.h"
#include "wresrcmng.h"
#include "wview.h"

// 0xd06bd0: the list's items, one byte each (the hole number); the list
// compares one byte to find a hole's row.
static const unsigned char s_holeIds[18] = {
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18
};

// 0xe09348, 0xe0934c: the size of a preview cell in the sheets.
static const float s_previewCellWidth = 140.0f;
static const float s_previewCellHeight = 93.0f;

// 0xe428fc: the whole texture.
static const WRect s_fullUV(0.0f, 0.0f, 1.0f, 1.0f);

// lib: 0xccc380 (m_RTTI's dynamic initializer, compiler-generated from
// this definition: WRTTI::WRTTI 0xa4c140, base FrForm); 0xccc3a0 is the
// factory registration below.
const WRTTI FrHoleSelector::m_RTTI("FrHoleSelector", &FrForm::m_RTTI);

// 0x5107a0
IObject* FrHoleSelectorMakeInstance(void) {
	return new FrHoleSelector;
}

struct __sFrHoleSelector {
	__sFrHoleSelector(void) {
		ObjectFactory().AddObjectFunctor(FrHoleSelectorMakeInstance, "FrHoleSelector");
	}
};
static __sFrHoleSelector s_registerFrHoleSelector;

// 0x5100c0
const sFRESH_MSGMAP* FrHoleSelector::GetMessageMap(void) const {
	return &FrHoleSelector::_MsgMap;
}

sFRESH_MSGMAP FrHoleSelector::_MsgMap = { &FrForm::_MsgMap, &FrHoleSelector::_MsgEntries[0] };

// 0xe09350 (the handlers are stored by the static init 0xccc3c0)
sFRESH_ENTRY FrHoleSelector::_MsgEntries[] = {
	{ "hole_preview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrHoleSelector::OnPreviewInit },
	{ "hole_preview", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrHoleSelector::OnPreviewDraw },
	{ "holeIds", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrHoleSelector::OnHoleListInit },
	{ "holeIds", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrHoleSelector::OnHoleListClick },
	{ "holeIds", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrHoleSelector::OnHoleListDraw },
	{ "holeIds", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrHoleSelector::OnHoleListHoverOff },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x5100d0 (scalar deleting destructor 0x510360)
FrHoleSelector::~FrHoleSelector(void) {
	if (g_resrcmng != 0) {
		if (m_pPreviewSheets[0] != 0) {
			g_resrcmng->Release(m_pPreviewSheets[0]);
			m_pPreviewSheets[0] = 0;
		}
		if (g_resrcmng != 0 && m_pPreviewSheets[1] != 0) {
			g_resrcmng->Release(m_pPreviewSheets[1]);
			m_pPreviewSheets[1] = 0;
		}
	}
}

// 0x510190
bool FrHoleSelector::Close(eFormRet ret, bool bDestroy) {
	return FrForm::Close(ret, false);
}

// 0x5102a0
FrHoleSelector::FrHoleSelector(void) {
	m_pHoleList = 0;
	m_pPreviewArea = 0;
	m_pPreview = 0;
	m_pPreviewSheets[0] = 0;
	m_pPreviewSheets[1] = 0;
	m_hole = 19;
	m_pButtonNormalImg = g_pFresh->GetBitmap("single_btn_hole_Normal");
	m_pButtonHoverImg = g_pFresh->GetBitmap("single_btn_hole_Hover");
	m_pButtonSelectImg = g_pFresh->GetBitmap("single_btn_hole_Select");
}

// 0x510390
void FrHoleSelector::OnPreviewInit(PG_MSGARG param) {
	m_pPreviewArea = DYNAMIC_CAST(FrArea, param);
}

// 0x510400
void FrHoleSelector::OnPreviewDraw(PG_MSGARG param) {
	if (m_pPreviewArea != 0 && m_pPreview != 0) {
		const WRect& area = m_pPreviewArea->GetRect();
		WRect rect(area.x, area.y, s_previewCellWidth, s_previewCellHeight);
		unsigned int color = ((unsigned int)(GetAlpha() * 255.0) << 24) | 0xffffff;
		m_pPreview->Render(g_view, m_previewUV, rect, 0, color, 0.0f, 0);
	}
}

// 0x5104d0
void FrHoleSelector::OnHoleListInit(PG_MSGARG param) {
	m_pHoleList = DYNAMIC_CAST(FrListBox, param);
	if (m_pHoleList != 0) {
		for (int i = 0; i < 18; i++) {
			m_pHoleList->AddItem((void*)&s_holeIds[i]);
		}
		m_textOffset.x = (float)(m_pHoleList->GetItemWidth() / 2);
		m_textOffset.y = (float)(m_pHoleList->GetItemHeight() / 2) - 5.0;
	}
}

// 0x5105a0
void FrHoleSelector::OnHoleListDraw(PG_MSGARG param) {
	FrListItem* item = (FrListItem*)param;
	if (item == 0 || item->pData == 0) {
		return;
	}
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0) {
		return;
	}
	const Bitmap* image = m_pButtonNormalImg;
	unsigned int textColor = 0xff000000;
	if (item->selected) {
		image = m_pButtonSelectImg;
		textColor = 0xffffffff;
	} else if (item->underCursor) {
		image = m_pButtonHoverImg;
	}
	if (image != 0) {
		pGDI->DrawTexture(image, WRect(item->pos.x, item->pos.y, image->Width(), image->Height()), 0xffffffff, 0);
	}
	if (item->pData != 0) {
		unsigned char hole = *(const unsigned char*)item->pData;
		unsigned long oldColor = pGDI->GetTextColor();
		unsigned long oldOutlineColor = pGDI->GetTextOutlineColor();
		pGDI->SetTextColor(FrALPHA(textColor, GetAlpha()), oldOutlineColor);
		WPoint pos(m_textOffset.x + item->pos.x, m_textOffset.y + item->pos.y);
		pGDI->PrintText11(pos, 1, MakeStr("%d", hole), (Bitmap*)0);
		pGDI->SetTextColor(oldColor, oldOutlineColor);
	}
}

// 0x5106f0
void FrHoleSelector::ShowPreview(unsigned char hole) {
	int cell = hole;
	int sheet = 0;
	m_pPreview = 0;
	if (cell > 10) {
		sheet = 1;
		cell = cell <= 19 ? cell - 10 : 9;
	} else if (cell < 1) {
		cell = 1;
	}
	m_pPreview = m_pPreviewSheets[sheet];
	if (m_pPreview != 0) {
		S5::UTIL::CalculateUVPos(m_previewUV, m_pPreview->GetWidth(), m_pPreview->GetHeight(), s_previewCellWidth, s_previewCellHeight, cell - 1);
		return;
	}
	m_previewUV = s_fullUV;
}

// 0x510830
void FrHoleSelector::SetCourse(unsigned char course, unsigned char hole) {
	int clamped = hole;
	if (clamped >= 19) {
		clamped = 19;
	}
	m_hole = (unsigned char)clamped;
	// Course ids above 0x7f (except 0xfd) carry a 0x80 flag.
	unsigned int courseId = (course <= 0x7f || course == 0xfd) ? course : course - 0x80;
	IFF_STRUCT::sCourse* record = ItemManager()->FindCourse(0x28000000 | courseId);
	if (record != 0 && course < 0x16) {
		if (m_pPreviewSheets[0] == 0) {
			m_pPreviewSheets[0] = g_resrcmng->GetOverlay(MakeStr("%s_1.png", record->m_name), 0);
		}
		if (m_pPreviewSheets[1] == 0) {
			m_pPreviewSheets[1] = g_resrcmng->GetOverlay(MakeStr("%s_2.png", record->m_name), 0);
		}
	}
	if (m_pHoleList != 0) {
		m_pHoleList->SelectItem(m_pHoleList->FindItem((void*)&s_holeIds[m_hole - 1], 1), true);
	}
	ShowPreview(m_hole);
}

// 0x510930
void FrHoleSelector::OnMouseMove(const WPoint& pt) {
	FrForm::OnMouseMove(pt);
	unsigned char hole = m_hole;
	// The row under the cursor (JP FrListBox +0x104).
	if (m_pHoleList != 0 && m_pHoleList->GetItemUnderCursor() != 0 && m_pHoleList->GetItemUnderCursor()->pData != 0) {
		hole = *(const unsigned char*)m_pHoleList->GetItemUnderCursor()->pData;
	}
	ShowPreview(hole);
}

// 0x510980
void FrHoleSelector::OnHoleListClick(void) {
	// The selected row (JP FrListBox +0x108).
	FrListItem* selected = m_pHoleList->GetSelected();
	if (selected != 0 && selected->pData != 0) {
		m_hole = *(const unsigned char*)selected->pData;
		ShowPreview(m_hole);
		Close(FrOK, true);
		return;
	}
	if (m_pHoleList != 0) {
		m_pHoleList->SelectItem(m_pHoleList->FindItem((void*)&s_holeIds[m_hole - 1], 1), true);
	}
	Close(FrOK, true);
}

// 0x5109f0
void FrHoleSelector::OnHoleListHoverOff(PG_MSGARG param) {
	ShowPreview(m_hole);
}

// 0x5227d0
FrForm* CreateHoleSelector(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent) {
	return CreateForm<FrHoleSelector>(manager, owner, name, parent);
}
