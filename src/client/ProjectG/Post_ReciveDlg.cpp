// Original: d:\build\custom\temp\code\client\projectg\post_recivedlg.cpp
#include "post_recivedlg.h"
#include "ObjectFactory.h"
#include "commonutil.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frbutton.h"
#include "frarea.h"
#include "fredit.h"
#include "frlistbox.h"
#include "frscrollbar.h"
#include "frgraphicinterface.h"
#include "bitmap.h"
#include "ToolTip.h"
#include "wfont.h"
#include "wresrcmng.h"
#include "shared/globalgamedefine.h"
#include "Packet.h"
#include "actor.h"
#include "TaskManager.h"
#include "TransferableItem.h"
#include "SharedDoc.h"
#include "shared/Localize.h"
#include "ReturnGiftDlg.h"
#include "UccLibrary.h"
#include "UccManager.h"
#include "wview.h"
#include <new>
#include <list>
#include <algorithm>

// post_recivedlg.h: no line record (empty constructor of a nested helper struct)
CPost_ReciveDlg::miniBtn_t::miniBtn_t(void) {
}

// post_recivedlg.cpp:33 (run-time type and factory registration of CPost_ReciveDlg)
const WRTTI CPost_ReciveDlg::m_RTTI("CPost_ReciveDlg", &FrForm::m_RTTI);

IObject* CPost_ReciveDlgMakeInstance(void) {
	return new CPost_ReciveDlg;
}

struct __sCPost_ReciveDlg {
	__sCPost_ReciveDlg(void) {
		ObjectFactory().AddObjectFunctor(CPost_ReciveDlgMakeInstance, "CPost_ReciveDlg");
	}
};
static __sCPost_ReciveDlg s_registerCPost_ReciveDlg;

// post_recivedlg.cpp:35
const sFRESH_MSGMAP* CPost_ReciveDlg::GetMessageMap(void) const {
	return &CPost_ReciveDlg::_MsgMap;
}

sFRESH_MSGMAP CPost_ReciveDlg::_MsgMap = { &FrForm::_MsgMap, &CPost_ReciveDlg::_MsgEntries[0] };

sFRESH_ENTRY CPost_ReciveDlg::_MsgEntries[] = {
	// post_recivedlg.cpp:37
	{ "recive_post", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_ReciveDlg::OnRecivePostInit },
	// post_recivedlg.cpp:38
	{ "recive_post", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CPost_ReciveDlg::OnRecivePostBtnUp },
	// post_recivedlg.cpp:39
	{ "send_post", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_ReciveDlg::OnSendPostInit },
	// post_recivedlg.cpp:40
	{ "send_post", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CPost_ReciveDlg::OnSendPostBtnUp },
	// post_recivedlg.cpp:41
	{ "delete_letter", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_ReciveDlg::OnDeleteAllLetterInit },
	// post_recivedlg.cpp:42
	{ "delete_letter", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CPost_ReciveDlg::OnDeleteAllLetterBtnUp },
	// post_recivedlg.cpp:43
	{ "write_letter", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_ReciveDlg::OnWriteLetterInit },
	// post_recivedlg.cpp:44
	{ "write_letter", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CPost_ReciveDlg::OnWriteLetterBtnUp },
	// post_recivedlg.cpp:45
	{ "post_slot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_ReciveDlg::OnPostListInit },
	// post_recivedlg.cpp:46
	{ "post_slot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_ReciveDlg::OnPostListOwnerDraw },
	// post_recivedlg.cpp:47
	{ "post_slot", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CPost_ReciveDlg::OnPostListBtnDown },
	// post_recivedlg.cpp:48
	{ "post_slot", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CPost_ReciveDlg::OnPostListRBtnDown },
	// post_recivedlg.cpp:49
	{ "post_slot", FRCMD_MOUSEMOVE, FrFuncVV, (FRESH_PMSG)&CPost_ReciveDlg::OnPostListMouseMove },
	// post_recivedlg.cpp:50
	{ "post_slot", FRCMD_DBLCLICK, FrFuncVV, (FRESH_PMSG)&CPost_ReciveDlg::OnPostListDClick },
	// post_recivedlg.cpp:52
	{ "page_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_ReciveDlg::OnPageListInit },
	// post_recivedlg.cpp:53
	{ "page_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_ReciveDlg::OnPageListOwnerDraw },
	// post_recivedlg.cpp:54
	{ "page_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CPost_ReciveDlg::OnPageListBtnDown },
	// post_recivedlg.cpp:60
	{ "btn_minus", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_ReciveDlg::OnPageDownInit },
	// post_recivedlg.cpp:61
	{ "btn_minus", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CPost_ReciveDlg::OnPageDownBtnUp },
	// post_recivedlg.cpp:62
	{ "btn_pluse", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_ReciveDlg::OnPageUpInit },
	// post_recivedlg.cpp:63
	{ "btn_pluse", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CPost_ReciveDlg::OnPageUpBtnUp },
	// post_recivedlg.cpp:67
	{ "Edit_Page", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&CPost_ReciveDlg::OnEditPageEndterKey },
	// post_recivedlg.cpp:69
	{ "tooltip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_ReciveDlg::OnToolTipInit },
	// post_recivedlg.cpp:70
	{ "tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_ReciveDlg::OnToolTipOnwerDraw },
	// post_recivedlg.cpp:73
	{ "index_area", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_ReciveDlg::OnIndexAreaInit },
	// post_recivedlg.cpp:74
	{ "index_area", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&CPost_ReciveDlg::OnIndexAreaOwnerDraw },
	// post_recivedlg.cpp:76
	{ "index_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_ReciveDlg::OnIndexListInit },
	// post_recivedlg.cpp:77
	{ "index_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CPost_ReciveDlg::OnIndexListLBDown },
	// post_recivedlg.cpp:78
	{ "index_list", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&CPost_ReciveDlg::OnIndexListMouseMove },

	// post_recivedlg.cpp:80
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// Title font file lists (nameless data): [0] the letter contents, [1] the
// mail box.
extern "C" char* data_a8f108[];

// post_recivedlg.cpp:84
CPost_ReciveDlg::CPost_ReciveDlg(void) {
	m_pContentsDlg = 0;
	m_curPage = 1;
	m_requestPage = 1;
	m_pQuestionItem = 0;
	m_questionType = -1;
	m_pageListStep = 1;

	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = data_a8f108 + 1;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:,";
	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&info);

	m_pIndexArea = 0;
	m_pIndexList = 0;
	m_indexPageCount = 0;
	m_indexPage = 0;
	m_indexSelected = 0;
	m_indexHover = 0;
	m_indexItemCount = 0;
	m_pIndexRects = 0;
	m_pDeleteAllLetterBtn = 0;
	m_pWriteLetterBtn = 0;
	m_questionIndex = -1;
	OnLoadImage();
	OnLoadPostList();
}

// post_recivedlg.cpp:130
CPost_ReciveDlg::~CPost_ReciveDlg(void) {
	if (m_pTitleFont) {
		m_pTitleFont->Erase();
	}

	if (g_resrcmng && m_pTitleFont) {
		g_resrcmng->Release(m_pTitleFont);
		m_pTitleFont = 0;
	}

	if (m_pIndexRects) {
		delete[] m_pIndexRects;
		m_pIndexRects = 0;
	}
}

// post_recivedlg.cpp:152
void CPost_ReciveDlg::OnRecivePostInit(intptr_t param) {
	m_pRecivePostBtn = DYNAMIC_CAST(FrButton, param);
}

// post_recivedlg.cpp:159
void CPost_ReciveDlg::OnRecivePostBtnUp(void) {
}

// post_recivedlg.cpp:164
void CPost_ReciveDlg::OnSendPostInit(intptr_t param) {
	m_pSendPostBtn = DYNAMIC_CAST(FrButton, param);
}

// post_recivedlg.cpp:171
void CPost_ReciveDlg::OnSendPostBtnUp(void) {
	FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
	// "Under construction!!! Please use it later~"
	pForm->SetMessage("\xB0\xF8\xBB\xE7\xC1\xDF!!! \xB3\xAA\xC1\xDF\xBF\xA1 \xC0\xCC\xBF\xEB\xC7\xD8 \xC1\xD6\xBC\xBC\xBF\xE4~", false);
	pForm->Open(0, 3);
}

// post_recivedlg.cpp:178
void CPost_ReciveDlg::OnDeleteAllLetterInit(intptr_t param) {
	m_pDeleteAllLetterBtn = DYNAMIC_CAST(FrButton, param);
}

// post_recivedlg.cpp:185: functor collecting the indices of the unread
// letters to delete, at most 100.
struct delete_letter_check {
	delete_letter_check(void);
	void operator()(const sMailInfoBrief& info);

	unsigned long m_count;                  // 0x0
	std::list<unsigned long> m_indexList;   // 0x4
};

// post_recivedlg.cpp:188
delete_letter_check::delete_letter_check(void) : m_count(0) {
}

// post_recivedlg.cpp:189
void delete_letter_check::operator()(const sMailInfoBrief& info) {
	if (info.m_hasItem == 0 && m_count < 100) {
		m_count++;
		m_indexList.push_back(info.m_mailIndex);
	}
}

// post_recivedlg.cpp:199
void CPost_ReciveDlg::OnDeleteAllLetterBtnUp(void) {
	sMailBoxListForPage* pPageList = WSingleton<CTransferableItem>::Instance()->GetMailBoxPageList();

	delete_letter_check check = std::for_each(pPageList->mailList.begin(), pPageList->mailList.end(), delete_letter_check());
	if (check.m_count != 0) {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);
		// "Delete all %d letters of the current page that hold no item?
		// (Note: unread letters are deleted too.)"
		pForm->SetMessage(MakeStr("\xBE\xC6\xC0\xCC\xC5\xDB\xC0\xCC \xC3\xB7\xBA\xCE\xB5\xC7\xC1\xF6 \xBE\xCA\xC0\xBA \xC7\xF6\xC0\xE7 \xC6\xE4\xC0\xCC\xC1\xF6\xC0\xC7 \xC6\xED\xC1\xF6(%d\xB0\xB3)\xB8\xA6 \xB8\xF0\xB5\xCE \xBB\xE8\xC1\xA6 \xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?\n\n\\c0xffff0000\\c(\xC1\xD6\xC0\xC7 : \xB3\xBB\xBF\xEB\xC0\xBB \xC0\xD0\xC1\xF6 \xBE\xCA\xC0\xBA \xC6\xED\xC1\xF6\xB5\xB5 \xB8\xF0\xB5\xCE \xBB\xE8\xC1\xA6 \xB5\xCB\xB4\xCF\xB4\xD9.)\\c0xff000000\\c", check.m_count), false);
		pForm->Open((FRESH_RESULT_CALLBACK)&CPost_ReciveDlg::OnDeleteAllLetterResult, 3);
	} else {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "There is no letter to delete."
		pForm->SetMessage("\xBB\xE8\xC1\xA6\xC7\xD2 \xC6\xED\xC1\xF6\xB0\xA1 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
		pForm->Open(0, 3);
	}

	m_questionIndex = -1;
}

// post_recivedlg.cpp:221
bool CPost_ReciveDlg::OnDeleteAllLetterResult(int ret, FrForm *pForm) {
	if (ret == FrOK) {
		sMailBoxListForPage* pPageList = WSingleton<CTransferableItem>::Instance()->GetMailBoxPageList();

		delete_letter_check check = std::for_each(pPageList->mailList.begin(), pPageList->mailList.end(), delete_letter_check());

		WSendPacket packet(GCT_REQUEST_MAILBOX_DELETE_MAIL);
		packet.Encode4(check.m_count);
		std::list<unsigned long>::iterator it = check.m_indexList.begin();
		while (it != check.m_indexList.end()) {
			packet.Encode4(*it);
			++it;
		}

		packet.Encode4(m_requestPage);
		packet.Send(SEND_TO_DEFAULT);

		AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 1, 0, 0, 0, 0, 0);
		EnableMiniBtns(false);

		m_pQuestionItem = 0;
	}

	return true;
}

// post_recivedlg.cpp:249
void CPost_ReciveDlg::OnWriteLetterInit(intptr_t param) {
	m_pWriteLetterBtn = DYNAMIC_CAST(FrButton, param);
}

// post_recivedlg.cpp:256
void CPost_ReciveDlg::OnWriteLetterBtnUp(void) {
	if (IsLocalContent((localContentType_t)8)) {
		if (WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().flagBlock & 0x80) {
			// "Letter sending is blocked. Please contact customer support."
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\\c0xffff0000\\c\xC6\xED\xC1\xF6\xBE\xB2\xB1\xE2 \xBA\xED\xB7\xB0\\c0xff000000\\c \xBB\xF3\xC5\xC2\xC0\xD4\xB4\xCF\xB4\xD9.\n\n\xB0\xED\xB0\xB4\xBC\xBE\xC5\xCD\xB7\xCE \xB9\xAE\xC0\xC7\xC7\xCF\xBF\xA9 \xC1\xD6\xBD\xC3\xB1\xE2 \xB9\xD9\xB6\xF8\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
			return;
		}
	}

	AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x221, 0, 0, 0, 0, 0);

	SetFadeout(true);
	Close(true);
}

// post_recivedlg.cpp:275
void CPost_ReciveDlg::OnPageListInit(intptr_t param) {
	m_pPageList = DYNAMIC_CAST(FrListBox, param);
	if (m_pPageList) {
		m_pPageList->ClearItem();

		for (int i = 1; i <= 11; i++) {
			FrListItem* pItem = m_pPageList->AddItem((void*)(m_pageListStep * i));

			if (i == 1) {
				m_pPageList->SelectItem(pItem, true);
			}
		}

		m_pPageList->SetMultiSelect(false);

		m_pPageList->SetVisible(false);
	}
}

// post_recivedlg.cpp:301
void CPost_ReciveDlg::OnPageListOwnerDraw(intptr_t param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == 0)
		return;

	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	float offsetX = 7.5f;
	float offsetY = 0.0f;

	int page = (intptr_t)pItem->pData;
	if (page == 0)
		return;

	if (page > m_totalPage)
		pItem->selected = false;

	pGI->SetTextColor(0xFF959494, 0xFFFFFFFF);

	if (pItem->selected) {
		pGI->SetTextStyle(1);
		pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	} else if (pItem->underCursor) {
		offsetY = -0.5f;
		offsetX = 7.0f;
	} else {
		pGI->SetTextStyle(0);
	}

	if (page != m_totalPage) {
		pGI->Print(WPoint(offsetX + pItem->pos.x, offsetY + pItem->pos.y), 1, "%d ", page);

		pGI->SetTextStyle(0);
		pGI->SetTextColor(0xFF959494, 0xFFFFFFFF);

		pGI->Print(WPoint(pItem->pos.x + 12.0f, pItem->pos.y), 1, "|");
	} else {
		pGI->Print(WPoint(offsetX + pItem->pos.x, offsetY + pItem->pos.y), 1, "%d", page);
	}
}

// post_recivedlg.cpp:349
void CPost_ReciveDlg::OnPageListBtnDown(void) {
	FrListItem* pItem = m_pPageList->GetItemUnderCursor();

	if (pItem) {
		int page = (intptr_t)pItem->pData;

		int maxPage = m_totalPage;
		m_curPage = page < 1 ? 1 : (page > maxPage ? maxPage : page);
		OnLoadPostList();
	}
}

// post_recivedlg.cpp:374
void CPost_ReciveDlg::OnPostListInit(intptr_t param) {
	m_pPostList = DYNAMIC_CAST(FrListBox, param);
	if (m_pPostList) {
		m_pPostList->SetMultiSelect(false);
		m_pPostList->UseDummy(true);
		m_pPostList->SetPushSound(0);

		m_pPostList->AddNoRButtonRect(m_miniBtns[1].m_rect, 1);
		m_pPostList->AddNoRButtonRect(m_miniBtns[0].m_rect, 0);
	}
}

// post_recivedlg.cpp:389
void CPost_ReciveDlg::OnPostListOwnerDraw(intptr_t param) {
	int i = 0;
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem)
		return;

	const Bitmap* pBase;
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (!pItem->pData) {
		pBase = m_pItemBaseEmptyImg;
	} else if (pItem->selected) {
		pBase = m_pItemBaseDownImg;
		pGI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	} else if (pItem->underCursor) {
		pBase = m_pItemBaseOverImg;
		pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	} else {
		pBase = m_pItemBaseNormalImg;
		pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	}

	pGI->SetTextStyle(0);

	if (pBase)
		pGI->DrawTexture(pBase, WRect(pItem->pos.x, pItem->pos.y, (float)pBase->Width(), (float)pBase->Height()), 0xFFFFFFFF, 0);

	sMailInfoBrief* pBrief = (sMailInfoBrief*)pItem->pData;
	if (!pBrief)
		return;

	float offsetX;
	float offsetY;
	int count;
	const Bitmap* pIcon;
	const char* name = "\xC6\xED\xC1\xF6";   // "Letter"
	unsigned long typeId = pBrief->m_item.typeId;
	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);
	if (pCommon && pBrief->m_hasItem != 0) {
		name = pCommon->m_name;
		count = pBrief->m_item.count;
		pIcon = g_pFresh->GetBitmap(pCommon->m_icon);
	} else {
		if (!pBrief->m_read)
			pIcon = m_pArrivalMailImg;
		else
			pIcon = m_pOpenMailImg;
		count = 0;
	}

	if (pIcon) {
		switch (typeId >> 26) {
		case 1:
		case 2:
		case 8:
		case 15:
		case 16:
		case 28:
			offsetX = 7.0f;
			offsetY = 2.0f;
			break;
		case 11:
			offsetX = 9.0f;
			offsetY = 12.0f;
			break;
		case 14:
			if ((typeId & 0x3C00000) == 0x800000) {
				offsetX = 7.0f;
				offsetY = 0.0f;
			} else {
				offsetX = 16.0f;
				offsetY = 27.0f;
			}
			break;
		case 31:
			offsetX = 16.0f;
			offsetY = 12.0f;
			break;
		default:
			offsetX = 16.0f;
			offsetY = 12.0f;
			break;
		}
	}

	if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(typeId) && pBrief->m_hasItem > 0)
		pIcon = GetUccItemIcon(pBrief->m_item.typeId, pBrief->m_item.uccKey);

	if (pIcon && pGI)
		pGI->DrawTexture(pIcon, WRect(offsetX + pItem->pos.x, offsetY + pItem->pos.y, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);

	const WPoint& mousePos = g_pFresh->GetManager()->GetMousePos();

	miniBtn_t btn;
	if (pBrief->m_hasItem > 0)
		btn = m_miniBtns[0];
	else
		btn = m_miniBtns[1];

	WRect rc = m_miniBtns[1].m_rect + pItem->pos;
	if (!btn.m_enabled) {
		pGI->DrawTexture(btn.m_pNormalImg, WRect(rc), 0x88FFFFFF, 0);
	} else if (pItem->underCursor && rc.IsInRect(mousePos)) {
		pGI->DrawTexture(btn.m_pOverImg, rc + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
		CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x40);
	} else {
		pGI->DrawTexture(btn.m_pNormalImg, WRect(rc), 0xFFFFFFFF, 0);
	}

	if (pBrief->m_item.kind && (int)pBrief->m_item.flag > 0) {
		pGI->SetTextStyle(2);
		pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);

		const char* fmt = NULL;
		switch (typeId & 0xFC000000) {
		case 0x40000000: {
			IFF_ITEM_COMMON* pTime = ItemManager()->FindCommonItem(typeId);
			if (pTime) {
				// "%d days" / "Unlimited"
				fmt = pTime->m_timeType == 1 ? "%d\xC0\xCF" : "\xB9\xAB\xC1\xA6\xC7\xD1";
			}
			break;
		}
		default:
			fmt = "%d\xC0\xCF";   // "%d days"
			break;
		}

		if (fmt)
			pGI->Print(WPoint(pItem->pos.x + 72.0f, pItem->pos.y + 72.0f), 2, fmt, pBrief->m_item.flag);
	} else if (count > 1) {
		const char* text = MakeStr("%d", count);
		float x = pItem->pos.x + 72.0f;

		for (i = 0; i < strlen(text); i++)
			x -= m_pTitleFont->GetCharWidthA(g_view, text[i] - '0');

		m_pTitleFont->Print(g_view, x, pItem->pos.y + 42.0f, text, 0, 0xFFFFFFFF, 0);
	}


	if (pBrief->m_hasItem == -1)
		pGI->Print(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 23.0f), 2, "%d", typeId);
	else if (pBrief->m_hasItem > 1)
		pGI->Print(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 23.0f), 2, " + %d\xB0\xB3", pBrief->m_hasItem - 1);   // " + %d more"

	pGI->SetTextStyle(0);
	pGI->Print(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 7.0f), 2, "%s", name);

	if (IsLocalContent((localContentType_t)0x51)) {
		if ((pBrief->m_item.typeId & 0xFC000000) == 0x8000000 && pBrief->m_hasItem > 0)
			DrawPartsSlot(pGI, pItem);
	}
}

// post_recivedlg.cpp:615
void CPost_ReciveDlg::DrawPartsSlot(FrGraphicInterface* pGI, FrListItem* pItem) {
	sMailInfoBrief* pMail = (sMailInfoBrief*)pItem->pData;
	if (!pMail)
		return;

	if (!pGI) {
		pGI = g_pFresh->GetManager()->GetGDI();
		if (!pGI)
			return;
	}

	int i;
	float charX = 5.0f;
	float caddieX = 5.0f;
	if (!((unsigned short)pMail->m_item.charCardSlots > -1 && (unsigned short)pMail->m_item.charCardSlots < 5)) {
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pMail->m_item.typeId);
		if (pPart) {
			pMail->m_item.charCardSlots = pPart->m_charCardSlots;
			if (!((unsigned short)pMail->m_item.charCardSlots > -1 && (unsigned short)pMail->m_item.charCardSlots < 5)) {
				pMail->m_item.charCardSlots = 0;
			}
		} else {
			pMail->m_item.charCardSlots = 0;
		}
	}

	for (i = 1; i < (unsigned short)pMail->m_item.charCardSlots + 1; i++) {
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it = CSharedDoc::Instance()->m_cardPeriodList[0].begin(); it != CSharedDoc::Instance()->m_cardPeriodList[0].end(); ++it) {
			if (pMail->m_item.typeId == it->partsTypeId && pMail->m_item.guid == it->partsId) {
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

	if (!((unsigned short)pMail->m_item.caddieCardSlots > -1 && (unsigned short)pMail->m_item.caddieCardSlots < 5)) {
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pMail->m_item.typeId);
		if (pPart) {
			pMail->m_item.caddieCardSlots = pPart->m_caddieCardSlots;
			if (!((unsigned short)pMail->m_item.caddieCardSlots > -1 && (unsigned short)pMail->m_item.caddieCardSlots < 5)) {
				pMail->m_item.caddieCardSlots = 0;
			}
		} else {
			pMail->m_item.caddieCardSlots = 0;
		}
	}

	for (i = 1; i < (unsigned short)pMail->m_item.caddieCardSlots + 1; i++) {
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it = CSharedDoc::Instance()->m_cardPeriodList[0].begin(); it != CSharedDoc::Instance()->m_cardPeriodList[0].end(); ++it) {
			if (pMail->m_item.typeId == it->partsTypeId && pMail->m_item.guid == it->partsId) {
				IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(it->typeId);
				if (pCard) {
					if (i == it->slot) {
						if (it->kind == 1) {
							found = true;
							const Bitmap* pBitmap = g_pFresh->GetBitmap(MakeStr(pCard->m_texture));
							if (pBitmap) {
								pGI->DrawTexture(pBitmap, WRect(caddieX + pItem->pos.x, pItem->pos.y + 70.0f, (float)pBitmap->Width(), (float)pBitmap->Height()), 0xFFFFFFFF, 0);
								caddieX += 16.0f;
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

// post_recivedlg.cpp:758
void CPost_ReciveDlg::OnReset(void) {
	m_curPage = 1;
	OnLoadPostList();
}

// post_recivedlg.cpp:764
void CPost_ReciveDlg::DrawStatus2(FrGraphicInterface* pGI, FrListItem* pItem, short status[5], short slot[5], float x) {
	float y = 0.0f;
	for (int i = 0; i < 5; i++) {
		if (status[i] != 0 || slot[i] != 0) {
			pGI->SetTextColor(0xFF0000FF, 0xFFFFFFFF);
			pGI->Print(WPoint(x + 182.0f, y + x + 36.0f) + pItem->pos, 2, MakeStatusString(i, status[i], slot[i]));
			y += 14.0f;
		}
	}
}

// post_recivedlg.cpp:777
const char* CPost_ReciveDlg::MakeStatusString(int statusType, int statusValue, int slotValue) {
	const char* statusNames[5] = {
		"\xC6\xC4\xBF\xF6",           // "Power"
		"\xC4\xC1\xC6\xAE\xB7\xD1",   // "Control"
		"\xC1\xA4\xC8\xAE\xB5\xB5",   // "Accuracy"
		"\xBD\xBA\xC7\xC9",           // "Spin"
		"\xC4\xBF\xBA\xEA"            // "Curve"
	};
	if (statusValue != 0 && slotValue != 0) {
		return MakeStr("%s+%d/%s+%d", statusNames[statusType], statusValue, "\xBD\xBD\xB7\xD4", slotValue);   // "Slot"
	} else if (statusValue != 0) {
		return MakeStr("%s+%d", statusNames[statusType], statusValue);
	} else if (slotValue != 0) {
		return MakeStr("%s%s+%d", statusNames[statusType], "\xBD\xBD\xB7\xD4", slotValue);   // "Slot"
	} else {
		return "";
	}
}

// post_recivedlg.cpp:799
void CPost_ReciveDlg::OnPostListBtnDown(void) {
	FrListItem* pItem = m_pPostList->GetItemUnderCursor();
	if (pItem == 0 || pItem->pData == 0)
		return;

	m_pQuestionItem = pItem;
	sMailInfoBrief* pBrief = (sMailInfoBrief*)pItem->pData;

	if (pBrief == 0)
		return;

	if ((m_miniBtns[1].m_rect + pItem->pos).IsInRect(g_pFresh->GetManager()->GetMousePos())) {
		if (pBrief->m_hasItem > 0) {
			IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(pBrief->m_item.typeId);
			if (pCommon) {
				if (m_miniBtns[0].m_enabled)
					Question(0);
			} else {
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				// "The items attached to the letter cannot be moved now."
				pForm->SetMessage("\xBF\xEC\xC6\xED\xB9\xB0\xBF\xA1 \xC3\xB7\xBA\xCE\xB5\xC8 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBA \xC7\xF6\xC0\xE7 \xBF\xC5\xB1\xE6\xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
				pForm->Open(0, 3);
			}
		} else {
			if (m_miniBtns[1].m_enabled)
				Question(1);
		}

		m_questionIndex = pItem->idx;
	} else {
		m_questionType = -1;
		ToServerSendData(pItem, 0xBD);
	}
}

// post_recivedlg.cpp:849
void CPost_ReciveDlg::OnPostListRBtnDown(void) {
}

// post_recivedlg.cpp:866
void CPost_ReciveDlg::Question(int type) {
	if (type == -1)
		return;

	m_questionType = type;
	FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);
	if (type) {
		// "Do you want to delete the letter?"
		pForm->SetMessage("\xC6\xED\xC1\xF6\xB8\xA6 \xBB\xE8\xC1\xA6\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);
	} else {
		// "Do you want to move the gift to My Room?"
		pForm->SetMessage("\xBC\xB1\xB9\xB0\xC0\xBB \xB8\xB6\xC0\xCC\xB7\xEB\xC0\xB8\xB7\xCE \xBF\xC5\xB1\xE2\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);
	}

	pForm->Open((FRESH_RESULT_CALLBACK)&CPost_ReciveDlg::OnPostQuestionResult, 2);
}

// post_recivedlg.cpp:885
bool CPost_ReciveDlg::OnPostQuestionResult(int ret, FrForm *pForm) {
	if (ret == FrOK) {
		if (m_questionType) {
			ToServerSendData(m_pQuestionItem, 0xBE);
		} else {
			ToServerSendData(m_pQuestionItem, 0xBF);
		}
	} else {
		m_pQuestionItem = 0;
		m_questionIndex = -1;
	}
	return true;
}

// post_recivedlg.cpp:904
void CPost_ReciveDlg::ToServerSendData(FrListItem *pItem, unsigned long type) {
	if (pItem == 0 || pItem->pData == 0) {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "The selected item is an invalid item."
		pForm->SetMessage("\xBC\xB1\xC5\xC3\xC7\xD1 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBA \xC0\xDF\xB8\xF8\xB5\xC8 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xD4\xB4\xCF\xB4\xD9.", false);
		pForm->Open(0, 3);
		return;
	}

	sMailInfoBrief* pBrief = (sMailInfoBrief*)pItem->pData;

	if (type == 0xBE) {
		WSendPacket packet((enumClientPacket)type);
		packet.Encode4(1);
		packet.Encode4(pBrief->m_mailIndex);
		packet.Encode4(m_requestPage);
		packet.Send(SEND_TO_DEFAULT);
	} else {
		WSendPacket packet((int)type);
		packet.Encode4(pBrief->m_mailIndex);
		packet.Send(SEND_TO_DEFAULT);
	}

	AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 1, 0, 0, 0, 0, 0);
	EnableMiniBtns(false);

	if (type != 0xBD) {
		m_pQuestionItem = 0;
	}
}

// post_recivedlg.cpp:948
void CPost_ReciveDlg::OnPostListMouseMove(void) {
}

// post_recivedlg.cpp:952
void CPost_ReciveDlg::OnPostDescDlgCreate(void) {
	sMailInfo* pMailInfo = WSingleton<CTransferableItem>::Instance()->GetMailInfo();

	if (pMailInfo == 0)
		return;

	m_pContentsDlg = CreateForm<CPost_Contents>(g_pFresh->GetManager(), this, "post_contents", 0);
	if (m_pContentsDlg) {
		FrForm::EnableTail(false);
		m_pContentsDlg->SetData(pMailInfo);
		m_pContentsDlg->Open((FRESH_RESULT_CALLBACK)&CPost_ReciveDlg::OnPostMessageResult, 1);
		m_pContentsDlg->SetFadeout(true);
	}
}

// post_recivedlg.cpp:968
bool CPost_ReciveDlg::OnPostMessageResult(int ret, FrForm *pForm) {
	if (m_pQuestionItem == 0 || m_pQuestionItem->pData == 0)
		return true;

	sMailInfoBrief* pInfo = (sMailInfoBrief*)m_pQuestionItem->pData;
	m_pQuestionItem->selected = false;
	pInfo->m_read = true;

	m_pQuestionItem = 0;
	return true;
}

// post_recivedlg.cpp:985
void CPost_ReciveDlg::OnPageDownInit(intptr_t param) {
	m_pPageDownBtn = DYNAMIC_CAST(FrButton, param);
}

// post_recivedlg.cpp:993
void CPost_ReciveDlg::OnPageDownBtnUp(void) {
	if (m_indexPage > 0) {
		m_indexPage--;

		BuildIndexPage(m_indexPage);

		unsigned int page = m_indexPage;
		m_indexSelected = 0;
		WSendPacket packet(GCT_REQUEST_MAILBOX_GET_MAILBOX);
		packet.Encode4(page * 5 + 1);
		packet.Send(SEND_TO_DEFAULT);

		AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 1, 0, 0, 0, 0, 0);
		EnableMiniBtns(false);
	}
}

// post_recivedlg.cpp:1055
void CPost_ReciveDlg::OnPageUpInit(intptr_t param) {
	m_pPageUpBtn = DYNAMIC_CAST(FrButton, param);
}

// post_recivedlg.cpp:1061
void CPost_ReciveDlg::OnPageUpBtnUp(void) {
	if (m_indexPage + 1 < m_indexPageCount) {
		m_indexPage++;

		BuildIndexPage(m_indexPage);

		unsigned int page = m_indexPage;
		m_indexSelected = 0;
		WSendPacket packet(GCT_REQUEST_MAILBOX_GET_MAILBOX);
		packet.Encode4(page * 5 + 1);
		packet.Send(SEND_TO_DEFAULT);

		AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 1, 0, 0, 0, 0, 0);
		EnableMiniBtns(false);
	}
}

// post_recivedlg.cpp:1123
void CPost_ReciveDlg::OnPostListDClick(void) {
}

// post_recivedlg.cpp:1129
bool CPost_ReciveDlg::OnEditPageEndterKey(PG_MSGARG) {
	return true;
}

// post_recivedlg.cpp:1135
int CPost_ReciveDlg::GetItemNumInMyPocket(unsigned long typeId) {
	int count = 0;

	if (typeId == 0 || (typeId & 0xFC000000) == 0x04000000) {
		for (std::map<unsigned int, sCharacterInfo>::iterator it = CSharedDoc::Instance()->m_characterMap.begin(); it != CSharedDoc::Instance()->m_characterMap.end(); it++) {

			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.typeId);
			if (pItem) {

				if (typeId == 0 || it->second.typeId == typeId)
					count++;
			}
		}
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x08000000) {
		for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_myPartsList.begin(); it != CSharedDoc::Instance()->m_myPartsList.end(); ++it) {

			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->m_typeId);
			if (pItem) {

				if (typeId == 0 || it->m_typeId == typeId)
					count++;
			}
		}
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x18000000) {
		for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_itemInfoList.begin(); it != CSharedDoc::Instance()->m_itemInfoList.end(); ++it) {

			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->m_typeId);
			if (pItem) {

				if (typeId == 0 || it->m_typeId == typeId)
					count++;
			}
		}
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x1C000000) {
		for (std::map<unsigned int, sCaddieInfo>::iterator it = CSharedDoc::Instance()->m_caddieMap.begin(); it != CSharedDoc::Instance()->m_caddieMap.end(); ++it) {

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
		for (std::map<unsigned int, sCharacterInfo>::iterator it = CSharedDoc::Instance()->m_characterMap.begin(); it != CSharedDoc::Instance()->m_characterMap.end(); ++it) {

			IFF_STRUCT::sHairStyle* pHair = ItemManager()->FindHairStyle(typeId);
			if (pHair) {

				if (pHair->m_color == it->second.hairColor && (pHair->m_style | 0x04000000) == it->second.typeId)
					count++;
			}
		}
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x10000000) {
		for (std::map<unsigned int, sItemInfo>::iterator it = CSharedDoc::Instance()->m_myItemMap.begin(); it != CSharedDoc::Instance()->m_myItemMap.end(); ++it) {

			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.m_typeId);
			if (pItem) {

				if (typeId == 0 || it->second.m_typeId == typeId)
					count++;
			}
		}
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x14000000) {
		for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_myGearList.begin(); it != CSharedDoc::Instance()->m_myGearList.end(); ++it) {

			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->m_typeId);
			if (pItem) {

				if (typeId == 0 || it->m_typeId == typeId)
					count++;
			}
		}
	}

	if ((typeId >> 26) == 9 && (typeId & 0x03E00000) == 0x00200000) {

		IFF_STRUCT::sSetItem* pSetItem = ItemManager()->FindSetItem(typeId);

		for (unsigned char i = 0; i < pSetItem->m_itemCount; i++) {

			if (typeId == 0 || (pSetItem->m_itemTypeIds[i] & 0xFC000000) != 0x04000000) {

				for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_myPartsList.begin(); it != CSharedDoc::Instance()->m_myPartsList.end(); ++it) {

					IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->m_typeId);
					if (pItem) {

						if (typeId == 0 || it->m_typeId == pSetItem->m_itemTypeIds[i])
							count++;
					}
				}
			}
		}
	}

	if (typeId == 0 || (typeId >> 26) == 14) {
		for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_mySkinList.begin(); it != CSharedDoc::Instance()->m_mySkinList.end(); ++it) {

			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->m_typeId);
			if (pItem) {

				if (typeId == 0 || it->m_typeId == typeId)
					count++;
			}
		}
	}

	return count;
}

// post_recivedlg.cpp:1294
bool CPost_ReciveDlg::OnReturnGiftDlgResult(int ret, FrForm *pForm) {
	if (ret != FrOK)
		return true;

	FrReturnGiftDlg* pDlg = DYNAMIC_CAST(FrReturnGiftDlg, pForm);
	if (pDlg == 0)
		return true;

	WSendPacket packet(GCT_REQUEST_DRAWBACK_GIFT);
	packet.Encode4(pDlg->m_requestIds[0]);
	packet.Encode4(pDlg->m_requestIds[1]);
	packet.Send(SEND_TO_DEFAULT);

	AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 1, 0, 0, 0, 0, 0);
	EnableMiniBtns(false);

	return true;
}

// post_recivedlg.cpp:1314
bool CPost_ReciveDlg::OnMoveGiftDlgResult(int ret, FrForm *pForm) {
	if (ret == FrOK && m_pMoveGiftIds) {
		FrForm* pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "Requested moving the gift."
		pNotify->SetMessage("\xBC\xB1\xB9\xB0 \xBF\xC5\xB1\xE2\xB1\xE2\xB8\xA6 \xBF\xE4\xC3\xBB\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", false);
		pNotify->Open(0, 3);

		WSendPacket packet(GCT_MOVE_GIFT);
		packet.Encode4(m_pMoveGiftIds[0]);
		packet.Encode4(m_pMoveGiftIds[1]);
		packet.Send(SEND_TO_DEFAULT);

		AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 1, 0, 0, 0, 0, 0);
		EnableMiniBtns(false);
	}

	return true;
}

// post_recivedlg.cpp:1345
void CPost_ReciveDlg::OnLoadImage(void) {
	m_miniBtns[0].m_pNormalImg = g_pFresh->RegisterBitmap("rmr_giftmove_btn_n");
	m_miniBtns[0].m_pOverImg = g_pFresh->RegisterBitmap("rmr_giftmove_btn_o");
	m_miniBtns[0].m_rect = WRect(103.0f, 66.0f, m_miniBtns[0].m_pNormalImg->Width(), m_miniBtns[0].m_pNormalImg->Height());

	m_miniBtns[0].m_enabled = true;

	m_miniBtns[1].m_pNormalImg = g_pFresh->RegisterBitmap("rmr_delete_btn_n");
	m_miniBtns[1].m_pOverImg = g_pFresh->RegisterBitmap("rmr_delete_btn_o");
	m_miniBtns[1].m_rect = WRect(144.0f, 66.0f, m_miniBtns[1].m_pNormalImg->Width(), m_miniBtns[1].m_pNormalImg->Height());

	m_miniBtns[1].m_enabled = true;

	m_pItemBaseNormalImg = g_pFresh->RegisterBitmap("item_base_n");
	m_pItemBaseOverImg = g_pFresh->RegisterBitmap("item_base_o");
	m_pItemBaseDownImg = g_pFresh->RegisterBitmap("item_base_d");
	m_pItemBaseEmptyImg = g_pFresh->RegisterBitmap("item_base_empty");

	m_pArrivalMailImg = g_pFresh->RegisterBitmap("rmr_Arrival_mail");
	m_pOpenMailImg = g_pFresh->RegisterBitmap("rmr_Open_mail");
}

// post_recivedlg.cpp:1375
void CPost_ReciveDlg::OnToolTipInit(intptr_t param) {
	m_pToolTipArea = DYNAMIC_CAST(FrArea, param);
}

// post_recivedlg.cpp:1381
void CPost_ReciveDlg::OnToolTipOnwerDraw(PG_MSGARG param) {
	m_toolTipManager.Process();

	int tipIndex = CIconToolTip::GetInstance()->GetCurTipIndex();

	if (tipIndex >= 0x40 && tipIndex <= 0x41) {
		CIconToolTip::GetInstance()->ClearToolTip();
	}
}

// post_recivedlg.cpp:1421
void CPost_ReciveDlg::BuildIndexPage(int page) {
	m_indexItemCount = 0;
	if (m_pIndexRects) {
		delete[] m_pIndexRects;
		m_pIndexRects = 0;
	}

	if (m_pageListStep < 1) {
		m_indexPageCount = 0;
		m_indexItemCount = 0;
		return;
	}

	m_indexPageCount = m_totalPage / 5;
	if (m_totalPage % 5 != 0)
		m_indexPageCount++;

	if (m_totalPage - page * 5 >= 5) {
		m_indexItemCount = 5;
	} else {
		if (m_totalPage % 5 != 0)
			m_indexItemCount = m_totalPage % 5;

		if (m_indexItemCount < 1)
			return;
	}

	m_pIndexRects = new WRect[m_indexItemCount];

	float totalWidth = m_indexItemCount * 30.0f;
	float startX = 243.0f - totalWidth * 0.5f;
	if (m_pIndexRects) {
		for (unsigned int i = 0; i < m_indexItemCount; i++) {
			float offset = i * 30.0f;
			m_pIndexRects[i] = WRect(startX + offset, 300.0f, i * 30.0f + startX + 30.0f, 310.0f);
		}
	}
}

// post_recivedlg.cpp:1488
void CPost_ReciveDlg::EnableMiniBtns(bool bEnable) {
	for (int i = 0; i < 2; i++)
		m_miniBtns[i].m_enabled = bEnable;

	if (m_pDeleteAllLetterBtn)
		m_pDeleteAllLetterBtn->Enable(bEnable);

	if (m_pWriteLetterBtn)
		m_pWriteLetterBtn->Enable(bEnable);
}

// post_recivedlg.cpp:1502
void CPost_ReciveDlg::OnIndexAreaInit(intptr_t param) {
	m_pIndexArea = DYNAMIC_CAST(FrArea, param);
}

// post_recivedlg.cpp:1512
void CPost_ReciveDlg::OnIndexAreaOwnerDraw(void) {
	if (m_pIndexRects == 0)
		return;

	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == 0)
		return;

	for (int i = 0; i < m_indexItemCount; i++) {
		WRect rc = m_rect;

		if (i == m_indexSelected) {
			pGI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
			pGI->SetTextStyle(1);
		} else if (i == m_indexHover) {
			pGI->SetTextColor(0xFFAA1111, 0xFFFFFFFF);
			pGI->SetTextStyle(1);
		} else {
			pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);
			pGI->SetTextStyle(0);
		}

		pGI->Print(WPoint(rc.x + m_pIndexRects[i].x + 15.0f, rc.y + m_pIndexRects[i].y), 1, "%d", m_indexPage * 5 + i + 1);

		if (i != 0) {
			pGI->SetTextColor(0xFFAAAAAA, 0xFFFFFFFF);
			pGI->SetTextStyle(0);

			pGI->Print(WPoint(rc.x + m_pIndexRects[i].x, rc.y + m_pIndexRects[i].y), 1, "|");
		}

		pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGI->SetTextStyle(0);
	}
}

// post_recivedlg.cpp:1562
void CPost_ReciveDlg::OnIndexListInit(intptr_t param) {
	m_pIndexList = DYNAMIC_CAST(FrListBox, param);
}

// post_recivedlg.cpp:1568
void CPost_ReciveDlg::OnIndexListLBDown(void) {
	if (m_indexItemCount == 0 || m_pIndexRects == 0)
		return;

	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();

	for (unsigned char i = 0; i < m_indexItemCount; i++) {
		WRect wndRect = GetRect();
		WRect rc = m_pIndexRects[i];

		rc.x += wndRect.x;
		rc.y += wndRect.y;
		rc.w += wndRect.x;
		rc.h += wndRect.y;

		// rc holds left, top, right, bottom
		if (mouse.x >= rc.x && mouse.x <= rc.w && mouse.y >= rc.y && mouse.y <= rc.h) {
			m_indexSelected = i;
			int mailPage = m_indexPage * 5 + i + 1;
			WSendPacket packet(GCT_REQUEST_MAILBOX_GET_MAILBOX);
			packet.Encode4(mailPage);
			packet.Send(SEND_TO_DEFAULT);

			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 1, 0, 0, 0, 0, 0);
			EnableMiniBtns(false);
			return;
		}
	}
}

// post_recivedlg.cpp:1611
void CPost_ReciveDlg::OnIndexListMouseMove(void) {
	if (m_indexItemCount == 0 || m_pIndexRects == 0)
		return;

	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();

	for (unsigned char i = 0; i < m_indexItemCount; i++) {
		WRect wndRect = GetRect();
		WRect rc = m_pIndexRects[i];

		rc.x += wndRect.x;
		rc.y += wndRect.y;
		rc.w += wndRect.x;
		rc.h += wndRect.y;

		// rc holds left, top, right, bottom
		if (mouse.x >= rc.x && mouse.x <= rc.w && mouse.y >= rc.y && mouse.y <= rc.h) {
			m_indexHover = i;
			return;
		}
	}

	m_indexHover = 0xFF;
}

// post_recivedlg.cpp:1652
void CPost_ReciveDlg::OnLoadPostList(void) {
	WSendPacket packet(GCT_REQUEST_MAILBOX_GET_MAILBOX);
	packet.Encode4(m_requestPage);

	packet.Send(SEND_TO_DEFAULT);

	AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 1, 0, 0, 0, 0, 0);
	EnableMiniBtns(false);
}

// post_recivedlg.cpp:1664
void CPost_ReciveDlg::SetLoadPostList(void) {
	sMailBoxListForPage* pPageList = WSingleton<CTransferableItem>::Instance()->GetMailBoxPageList();

	m_totalPage = pPageList->totalPage;
	m_requestPage = pPageList->page;

	m_pPageList->ClearItem();

	for (int i = 1; i <= m_totalPage; i++) {
		FrListItem* pItem = m_pPageList->AddItem((void*)(m_pageListStep * i));
		if (i == m_requestPage)
			m_pPageList->SelectItem(pItem, true);
	}

	SetLoadPostList(&pPageList->mailList);

	BuildIndexPage(m_indexPage);

	unsigned int indexPage = m_indexPage;

	if (indexPage * 5 + m_indexSelected + 1 > (unsigned int)m_totalPage && m_totalPage != 0) {
		m_indexSelected = m_indexItemCount - 1;
		int requestPage = m_indexSelected + 1;

		WSendPacket packet(GCT_REQUEST_MAILBOX_GET_MAILBOX);
		packet.Encode4(requestPage);
		packet.Send(SEND_TO_DEFAULT);

		AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 1, 0, 0, 0, 0, 0);
		EnableMiniBtns(false);
	}
}

// post_recivedlg.cpp:1716
void CPost_ReciveDlg::SetLoadPostList(std::list<sMailInfoBrief>* pList) {
	m_pPostList->ClearItem();

	sMailInfoBrief info;

	if (pList->size() == 0)
		return;

	std::list<sMailInfoBrief>::iterator it;
	for (it = pList->begin(); it != pList->end(); ++it) {
		if (IsLocalContent((localContentType_t)0x51)) {
			if ((it->m_item.typeId & 0xFC000000) == 0x8000000 && it->m_hasItem > 0) {
				IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(it->m_item.typeId);
				if (pPart) {
					it->m_item.charCardSlots = pPart->m_charCardSlots;
					it->m_item.caddieCardSlots = pPart->m_caddieCardSlots;
				} else {
					it->m_item.charCardSlots = 0;
					it->m_item.caddieCardSlots = 0;
				}
			} else {
				it->m_item.charCardSlots = 0;
				it->m_item.caddieCardSlots = 0;
			}
		}

		FrListItem* pItem = m_pPostList->AddItem(&*it);
		pItem->AddNoRBtnIdx(0);
		pItem->AddNoRBtnIdx(1);
	}

	if (pList->size() < 20) {
		for (int i = pList->size(); i < 20; i++) {
			m_pPostList->AddItem(0);
		}
	}

	m_pPostList->ResetTopRow();
	int row = (m_questionIndex - 1) / 2;
	if (m_questionIndex != -1 && row >= 0) {
		m_pPostList->GetScrollBar()->SetCurTopRow(row);
	} else {
		m_pPostList->GetScrollBar()->ScrollToFirst();
	}
	m_questionIndex = -1;
}

// post_recivedlg.cpp:1774
void CPost_ReciveDlg::OnDeleteItem(FrListItem *pItem) {
	ToServerSendData(pItem, 0xBE);
}

// post_recivedlg.cpp:1811 (run-time type and factory registration of CPost_MessgeDlg)
const WRTTI CPost_MessgeDlg::m_RTTI("CPost_MessgeDlg", &FrForm::m_RTTI);

IObject* CPost_MessgeDlgMakeInstance(void) {
	return new CPost_MessgeDlg;
}

struct __sCPost_MessgeDlg {
	__sCPost_MessgeDlg(void) {
		ObjectFactory().AddObjectFunctor(CPost_MessgeDlgMakeInstance, "CPost_MessgeDlg");
	}
};
static __sCPost_MessgeDlg s_registerCPost_MessgeDlg;

// post_recivedlg.cpp:1813
const sFRESH_MSGMAP* CPost_MessgeDlg::GetMessageMap(void) const {
	return &CPost_MessgeDlg::_MsgMap;
}

sFRESH_MSGMAP CPost_MessgeDlg::_MsgMap = { &FrForm::_MsgMap, &CPost_MessgeDlg::_MsgEntries[0] };

sFRESH_ENTRY CPost_MessgeDlg::_MsgEntries[] = {
	// post_recivedlg.cpp:1824
	{ "recive_user", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_MessgeDlg::OnPostMessage_ReciveUserInit },
	// post_recivedlg.cpp:1825
	{ "letter_contents", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_MessgeDlg::OnPostMessage_LetterContentsInit },
	// post_recivedlg.cpp:1826
	{ "letter_date", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_MessgeDlg::OnPostMessage_LetterDateInit },

	// post_recivedlg.cpp:1828
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// post_recivedlg.cpp:1834
CPost_MessgeDlg::CPost_MessgeDlg(void) {
	m_pReciveUserEdit = 0;
	m_pLetterContentsEdit = 0;
	m_pLetterDateEdit = 0;
}

// post_recivedlg.cpp:1846
CPost_MessgeDlg::~CPost_MessgeDlg(void) {
}

// post_recivedlg.cpp:1908
void CPost_MessgeDlg::OnPostMessage_ReciveUserInit(intptr_t param) {
	m_pReciveUserEdit = DYNAMIC_CAST(FrEdit, param);
}

// post_recivedlg.cpp:1917
void CPost_MessgeDlg::OnPostMessage_LetterContentsInit(intptr_t param) {
	m_pLetterContentsEdit = DYNAMIC_CAST(FrEdit, param);
}

// post_recivedlg.cpp:1926
void CPost_MessgeDlg::OnPostMessage_LetterDateInit(intptr_t param) {
	m_pLetterDateEdit = DYNAMIC_CAST(FrEdit, param);
}

// post_recivedlg.cpp:1938
void CPost_MessgeDlg::OnPostDescFrame(bool bShow) {
	m_pReciveUserEdit->SetElementFadeOut(!bShow);
	m_pLetterContentsEdit->SetElementFadeOut(!bShow);
	m_pLetterDateEdit->SetElementFadeOut(!bShow);
}

// post_recivedlg.cpp:1951
void CPost_MessgeDlg::SetGiftDescString(const char* pSender, const char* pContents, const char* pItemName, const char* pItemDesc) {
	if (m_pReciveUserEdit->GetLine(1, false) > 0) m_pReciveUserEdit->ClearLine();
	if (m_pLetterContentsEdit->GetLine(1, false) > 0) m_pLetterContentsEdit->ClearLine();
	if (m_pLetterDateEdit->GetLine(1, false) > 0) m_pLetterDateEdit->ClearLine();

	if (pSender == 0) {
		// "There is no mail that has arrived."
		m_pLetterContentsEdit->AddText(MakeStr("\xB5\xB5\xC2\xF8\xC7\xD1 \xBF\xEC\xC6\xED\xB9\xB0\xC0\xCC \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9."), false, true);
	} else {
		m_pReciveUserEdit->AddText(MakeStr("%s", pSender), false, true);

		if (strlen(pContents) > 2)
			m_pLetterContentsEdit->AddText(MakeStr("%s", pContents), false, true);
		else
			// "The mail that arrived has no contents."
			m_pLetterContentsEdit->AddText(MakeStr("\xB5\xB5\xC2\xF8\xC7\xD1 \xBF\xEC\xC6\xED\xB9\xB0\xBF\xA1 \xB3\xBB\xBF\xEB\xC0\xCC \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9."), false, true);

		m_pLetterDateEdit->AddText(MakeStr("%s \\c0xff7a6625\\c(%s)\\c0xff000000\\c", pItemName, pItemDesc), false, true);
	}
}

// post_recivedlg.cpp:1976
void CPost_MessgeDlg::OnOK(void) {
	SetFadeout(true);
	FrForm::OnOK();
}

// post_recivedlg.cpp:1994 (run-time type and factory registration of CPost_Contents)
const WRTTI CPost_Contents::m_RTTI("CPost_Contents", &FrForm::m_RTTI);

IObject* CPost_ContentsMakeInstance(void) {
	return new CPost_Contents;
}

struct __sCPost_Contents {
	__sCPost_Contents(void) {
		ObjectFactory().AddObjectFunctor(CPost_ContentsMakeInstance, "CPost_Contents");
	}
};
static __sCPost_Contents s_registerCPost_Contents;

// post_recivedlg.cpp:1996
const sFRESH_MSGMAP* CPost_Contents::GetMessageMap(void) const {
	return &CPost_Contents::_MsgMap;
}

sFRESH_MSGMAP CPost_Contents::_MsgMap = { &FrForm::_MsgMap, &CPost_Contents::_MsgEntries[0] };

sFRESH_ENTRY CPost_Contents::_MsgEntries[] = {
	// post_recivedlg.cpp:2000
	{ "recive_user", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_Contents::OnPostMessage_ReciveUserInit },
	// post_recivedlg.cpp:2001
	{ "send_user", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_Contents::OnPostMessage_SendUserInit },
	// post_recivedlg.cpp:2002
	{ "letter_contents", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_Contents::OnPostMessage_LetterContentsInit },
	// post_recivedlg.cpp:2003
	{ "letter_date", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_Contents::OnPostMessage_LetterDateInit },
	// post_recivedlg.cpp:2005
	{ "post_slot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_Contents::OnPostListInit },
	// post_recivedlg.cpp:2006
	{ "post_slot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CPost_Contents::OnPostListOwnerDraw },
	// post_recivedlg.cpp:2007
	{ "post_slot", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CPost_Contents::OnPostListBtnDown },
	// post_recivedlg.cpp:2008
	{ "post_slot", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CPost_Contents::OnPostListRBtnDown },
	// post_recivedlg.cpp:2009
	{ "post_slot", FRCMD_MOUSEMOVE, FrFuncVV, (FRESH_PMSG)&CPost_Contents::OnPostListMouseMove },
	// post_recivedlg.cpp:2010
	{ "post_slot", FRCMD_DBLCLICK, FrFuncVV, (FRESH_PMSG)&CPost_Contents::OnPostListDClick },

	// post_recivedlg.cpp:2014
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// post_recivedlg.cpp:2018
CPost_Contents::CPost_Contents(void) {
	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = data_a8f108;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:,";
	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&info);
}

// post_recivedlg.cpp:2041
CPost_Contents::~CPost_Contents(void) {
	if (m_pTitleFont) {
		m_pTitleFont->Erase();
	}

	if (g_resrcmng && m_pTitleFont) {
		g_resrcmng->Release(m_pTitleFont);
		m_pTitleFont = 0;
	}
}

// post_recivedlg.cpp:2055
void CPost_Contents::OnPostMessage_ReciveUserInit(intptr_t param) {
	m_pReciveUserEdit = DYNAMIC_CAST(FrEdit, param);
}

// post_recivedlg.cpp:2065
void CPost_Contents::OnPostMessage_SendUserInit(intptr_t param) {
	m_pSendUserEdit = DYNAMIC_CAST(FrEdit, param);
}

// post_recivedlg.cpp:2073
void CPost_Contents::OnPostMessage_LetterContentsInit(intptr_t param) {
	m_pLetterContentsEdit = DYNAMIC_CAST(FrEdit, param);
}

// post_recivedlg.cpp:2081
void CPost_Contents::OnPostMessage_LetterDateInit(intptr_t param) {
	m_pLetterDateEdit = DYNAMIC_CAST(FrEdit, param);
	if (m_pLetterDateEdit) {
		m_pLetterDateEdit->AddText("2008-02-01 16:10", false, true);
	}
}

// post_recivedlg.cpp:2090
void CPost_Contents::OnPostListInit(intptr_t param) {
	m_pPostList = DYNAMIC_CAST(FrListBox, param);
}

// post_recivedlg.cpp:2097
void CPost_Contents::OnPostListOwnerDraw(intptr_t param) {
	int i = 0;
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem)
		return;

	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();

	pGI->SetTextStyle(0);

	sMailIncludeItem* pMail = (sMailIncludeItem*)pItem->pData;
	if (!pMail)
		return;

	float offsetX;
	float offsetY;
	int count;
	const char* iconName = "item0_13";
	unsigned long typeId = pMail->typeId;
	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);
	if (pCommon) {
		iconName = pCommon->m_icon;
		count = pMail->count;
	} else {
		count = 0;
	}

	const Bitmap* pIcon = g_pFresh->GetBitmap(iconName);

	if (pIcon) {
		switch (typeId >> 26) {
		case 1:
		case 2:
		case 8:
		case 9:
		case 15:
		case 16:
		case 28:
			offsetX = 7.0f;
			offsetY = 2.0f;
			break;
		case 11:
			offsetX = 9.0f;
			offsetY = 12.0f;
			break;
		case 14:
			if ((typeId & 0x3C00000) == 0x800000) {
				offsetX = 7.0f;
				offsetY = 0.0f;
			} else {
				offsetX = 16.0f;
				offsetY = 27.0f;
			}
			break;
		case 31:
			offsetX = 16.0f;
			offsetY = 12.0f;
			break;
		default:
			offsetX = 16.0f;
			offsetY = 27.0f;
			break;
		}
	}

	unsigned long kind = typeId >> 26;
	if (kind == 2) {
		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(((typeId >> 18) & 0xFF) | 0x4000000);
		if (pChar) {
			const Bitmap* pBitmap = g_pFresh->GetBitmap(MakeStr("s_%s", pChar->m_icon));
			if (pBitmap) {
				WRect rc(pItem->pos.x + 3.0f, pItem->pos.y + 3.0f, (float)pBitmap->Width(), (float)pBitmap->Height());
				pGI->DrawTexture(pBitmap, rc, 0xFFFFFFFF, 0);
			}
		}
	}

	if (IsLocalContent((localContentType_t)0x57) && typeId && IsUccClothes(typeId))
		pIcon = GetUccItemIcon(pMail->typeId, pMail->uccKey);

	if (pIcon && pGI)
		pGI->DrawTexture(pIcon, WRect(offsetX + pItem->pos.x, offsetY + pItem->pos.y, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);

	if (pMail->kind && (int)pMail->flag > 0) {
		pGI->SetTextStyle(2);
		pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGI->Print(WPoint(pItem->pos.x + 72.0f, pItem->pos.y + 72.0f), 2, "%d\xC0\xCF", pMail->flag);   // "%d days"
	} else if (count > 1) {
		const char* text = MakeStr("%d", count);
		float x = pItem->pos.x + 72.0f;

		for (i = 0; i < strlen(text); i++)
			x -= m_pTitleFont->GetCharWidthA(g_view, text[i] - '0');

		m_pTitleFont->Print(g_view, x, pItem->pos.y + 60.0f, text, 0, 0xFFFFFFFF, 0);
	}

	if (IsLocalContent((localContentType_t)0x51)) {
		if (kind == 2)
			DrawPartsSlot(pGI, pItem);
	}
}

// post_recivedlg.cpp:2268
void CPost_Contents::DrawPartsSlot(FrGraphicInterface* pGI, FrListItem* pItem) {
	sMailIncludeItem* pMail = (sMailIncludeItem*)pItem->pData;
	if (!pMail)
		return;

	if (!pGI) {
		pGI = g_pFresh->GetManager()->GetGDI();
		if (!pGI)
			return;
	}

	int i;
	float charX = 5.0f;
	float caddieX = 5.0f;
	if (!((unsigned short)pMail->charCardSlots > -1 && (unsigned short)pMail->charCardSlots < 5)) {
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pMail->typeId);
		if (pPart) {
			pMail->charCardSlots = pPart->m_charCardSlots;
			if (!((unsigned short)pMail->charCardSlots > -1 && (unsigned short)pMail->charCardSlots < 5)) {
				pMail->charCardSlots = 0;
			}
		} else {
			pMail->charCardSlots = 0;
		}
	}

	for (i = 1; i < (unsigned short)pMail->charCardSlots + 1; i++) {
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it = CSharedDoc::Instance()->m_cardPeriodList[0].begin(); it != CSharedDoc::Instance()->m_cardPeriodList[0].end(); ++it) {
			if (pMail->typeId == it->partsTypeId && pMail->guid == it->partsId) {
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

	if (!((unsigned short)pMail->caddieCardSlots > -1 && (unsigned short)pMail->caddieCardSlots < 5)) {
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pMail->typeId);
		if (pPart) {
			pMail->caddieCardSlots = pPart->m_caddieCardSlots;
			if (!((unsigned short)pMail->caddieCardSlots > -1 && (unsigned short)pMail->caddieCardSlots < 5)) {
				pMail->caddieCardSlots = 0;
			}
		} else {
			pMail->caddieCardSlots = 0;
		}
	}

	for (i = 1; i < (unsigned short)pMail->caddieCardSlots + 1; i++) {
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it = CSharedDoc::Instance()->m_cardPeriodList[0].begin(); it != CSharedDoc::Instance()->m_cardPeriodList[0].end(); ++it) {
			if (pMail->typeId == it->partsTypeId && pMail->guid == it->partsId) {
				IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(it->typeId);
				if (pCard) {
					if (i == it->slot) {
						if (it->kind == 1) {
							found = true;
							const Bitmap* pBitmap = g_pFresh->GetBitmap(MakeStr(pCard->m_texture));
							if (pBitmap) {
								pGI->DrawTexture(pBitmap, WRect(caddieX + pItem->pos.x, pItem->pos.y + 70.0f, (float)pBitmap->Width(), (float)pBitmap->Height()), 0xFFFFFFFF, 0);
								caddieX += 16.0f;
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

// post_recivedlg.cpp:2412
void CPost_Contents::OnPostListBtnDown(void) {
}

// post_recivedlg.cpp:2416
void CPost_Contents::OnPostListRBtnDown(void) {
}

// post_recivedlg.cpp:2420
void CPost_Contents::OnPostListMouseMove(void) {
}

// post_recivedlg.cpp:2424
void CPost_Contents::OnPostListDClick(void) {
}

// post_recivedlg.cpp:2428
void CPost_Contents::SetData(sMailInfo* pMailInfo) {
	if (m_pSendUserEdit) {
		m_pSendUserEdit->ClearLine();
		if (strlen(pMailInfo->senderNick) > 0)
			m_pSendUserEdit->AddText(MakeStr("%s", pMailInfo->senderNick), false, true);
	}

	if (m_pReciveUserEdit) {
		m_pReciveUserEdit->ClearLine();
		const char* pMyNick = WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().nickname;
		if (strlen(pMyNick) > 0)
			m_pReciveUserEdit->AddText(MakeStr("%s", pMyNick), false, true);
	}

	if (m_pLetterContentsEdit) {
		m_pLetterContentsEdit->ClearLine();
		if (strlen(pMailInfo->message) > 0)
			m_pLetterContentsEdit->AddText(MakeStr("%s", pMailInfo->message), false, true);
	}

	if (m_pLetterDateEdit) {
		m_pLetterDateEdit->ClearLine();
		if (strlen(pMailInfo->date) > 0)
			m_pLetterDateEdit->AddText(MakeStr("%s", pMailInfo->date), false, true);
	}

	SetData(&pMailInfo->itemList);
}

// post_recivedlg.cpp:2462
void CPost_Contents::SetData(std::list<sMailIncludeItem>* pItemList) {
	// Unused buffer: its initialisation is dropped, the /GS cookie stays.
	char szBuf[8] = "";

	m_pPostList->ClearItem();

	if (pItemList->size() != 0) {
		for (std::list<sMailIncludeItem>::iterator it = pItemList->begin(); it != pItemList->end(); ++it) {
			if (IsLocalContent((localContentType_t)0x51)) {
				if ((it->typeId & 0xFC000000) == 0x08000000) {
					IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(it->typeId);
					if (pPart) {
						it->charCardSlots = pPart->m_charCardSlots;
						it->caddieCardSlots = pPart->m_caddieCardSlots;
					} else {
						it->charCardSlots = 0;
						it->caddieCardSlots = 0;
					}
				}
			}

			m_pPostList->AddItem(&*it);
		}
	}
}
