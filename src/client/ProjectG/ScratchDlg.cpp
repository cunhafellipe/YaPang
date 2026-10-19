// Original: d:\build\custom\temp\code\client\projectg\scratchdlg.cpp
#include "NetworkSystem.h"
#include "ScratchDlg.h"
#include "RealMyRoomData.h"
#include "SharedDoc.h"
#include "commonutil.h"
#include "UccManager.h"
#include "ObjectFactory.h"
#include "Packet.h"
#include "shared/Localize.h"
#include "wfont.h"
#include "wresrcmng.h"
#include "fresh.h"
#include "frbutton.h"
#include "fredit.h"
#include "frarea.h"
#include "frgraphicinterface.h"
#include "frtextbutton.h"
#include "frviewer.h"
#include "frdesktop.h"
#include "frwndmanager.h"
#include <string.h>

// Offset of each item rect from the dialog's centre (defined at the end).
extern WPoint g_ptScratchItem[];

// Title font file names ("[font_wind.jpg", ...).
extern "C" char* data_a9bea0[];

// scratchdlg.cpp:18 (run-time type and factory registration of FrScratchDlg)
const WRTTI FrScratchDlg::m_RTTI("FrScratchDlg", &FrForm::m_RTTI);

IObject* FrScratchDlgMakeInstance(void) {
	return new FrScratchDlg;
}

struct __sFrScratchDlg {
	__sFrScratchDlg(void) {
		ObjectFactory().AddObjectFunctor(FrScratchDlgMakeInstance, "FrScratchDlg");
	}
};
static __sFrScratchDlg s_registerFrScratchDlg;

// scratchdlg.cpp:20
const sFRESH_MSGMAP* FrScratchDlg::GetMessageMap(void) const {
	return &FrScratchDlg::_MsgMap;
}

sFRESH_MSGMAP FrScratchDlg::_MsgMap = { &FrForm::_MsgMap, &FrScratchDlg::_MsgEntries[0] };

sFRESH_ENTRY FrScratchDlg::_MsgEntries[] = {
	// scratchdlg.cpp:22
	{ "scratchdlg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchDlg::OnScratchDlgInit },
	// scratchdlg.cpp:23
	{ "scratch_bottom_02", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchDlg::OnScratchBottomInit },
	// scratchdlg.cpp:24
	{ "scratch_bottom_02", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrScratchDlg::OnScratchBottomOwnerDraw },
	// scratchdlg.cpp:25
	{ "scratch_cover_01", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchDlg::OnScratchCover01Init },
	// scratchdlg.cpp:26
	{ "scratch_cover_01", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrScratchDlg::OnScratchCover01OwnerDraw },
	// scratchdlg.cpp:27
	{ "scratch_cover_02", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchDlg::OnScratchCover02Init },
	// scratchdlg.cpp:28
	{ "scratch_cover_02", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrScratchDlg::OnScratchCover02OwnerDraw },
	// scratchdlg.cpp:29
	{ "scratch_card", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchDlg::OnScratchCardInit },
	// scratchdlg.cpp:30
	{ "scratch_card", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&FrScratchDlg::OnScratchCardEnterKey },
	// scratchdlg.cpp:31
	{ "scratch_nextpage", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchDlg::OnScratchNextPageBnInit },
	// scratchdlg.cpp:32
	{ "scratch_nextpage", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScratchDlg::OnScratchNextPageBnUp },
	// scratchdlg.cpp:33
	{ "serial_number", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchDlg::OnSerialNumberInit },
	// scratchdlg.cpp:34
	{ "serial_number", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&FrScratchDlg::OnSerialNumberEnterKey },
	// scratchdlg.cpp:35
	{ "serial_number_bn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchDlg::OnSerialNumberBnInit },
	// scratchdlg.cpp:36
	{ "serial_number_bn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScratchDlg::OnSerialNumberBnUp },
	// scratchdlg.cpp:37
	{ "about", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchDlg::OnAboutBnInit },
	// scratchdlg.cpp:38
	{ "about", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScratchDlg::OnAboutBnUp },
	// scratchdlg.cpp:39
	{ "close", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchDlg::OnCloseBnInit },
	// scratchdlg.cpp:40
	{ "close", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScratchDlg::OnCloseBnUp },
	// scratchdlg.cpp:42
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// scratchdlg.cpp:45
FrScratchDlg::FrScratchDlg(void) {
	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	m_pScratchCover[0] = 0;
	m_pScratchCover[1] = 0;
	m_pScratchCardEdit = 0;
	m_pNextPageBn = 0;
	m_pSerialNumberEdit = 0;
	m_pSerialNumberBn = 0;

	info.filename = &data_a9bea0[1];
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:";
	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&info);

	m_pCoverAreaBitmap[0] = 0;
	m_pCoverAreaBitmap[1] = 0;
	m_pBmpClosedCover = g_pFresh->GetManager()->GetBitmap("SCRATCHDLG", "scratch_cover_close");
	m_pBmpEraser = g_pFresh->GetManager()->GetBitmap("SCRATCHDLG", "scratch_eraser");
	m_pBmpChangeCardNumber[0] = g_pFresh->GetManager()->GetBitmap("SCRATCHDLG", "scratch_text_minus");
	m_pBmpChangeCardNumber[1] = g_pFresh->GetManager()->GetBitmap("SCRATCHDLG", "scratch_text_plus");
	m_pBmpScratchItem[0] = 0;
	m_pBmpScratchItem[1] = 0;

	memset(m_rectScratchItem, 0, sizeof(m_rectScratchItem));
	m_rectEraseArea = WRect(0.0f, 0.0f, 0.0f, 0.0f);
	m_eraserOffset = WPoint(-10.0f, -10.0f);
	m_bLButtonDown = false;

	m_scratchCardCount = 0;
	memset(m_scratchItem, 0, sizeof(m_scratchItem));
	m_serialNumberResult = -1;
	m_scratchItemCount = 0;
	m_bEnableRequestScratchItem = true;
	m_bReceivedScratchItem = false;
	m_bEnableSerialNumber = true;
	m_bWaitResultText = false;
	m_changeCardNumber[0] = -1;
	m_changeCardNumber[1] = -1;
	m_changeCardTime[0] = 0;
	m_changeCardTime[1] = 0;
	m_changeCardPos[0] = 0;
	m_changeCardPos[1] = 0;
	m_resultTextTime = 0;

	memset(m_bCheckedScratchRect, 0, sizeof(m_bCheckedScratchRect));
	memset(m_checkScratchRect, 0, sizeof(m_checkScratchRect));

	if (IsLocalContent((localContentType_t)0x59)) {
		WSendPacket packet(GCT_UCC_REQUEST);
		packet.Encode1(2);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// scratchdlg.cpp:112
FrScratchDlg::~FrScratchDlg(void) {
	if (IsLocalContent((localContentType_t)0x59)) {
		if (WNetworkSystem::Instance()) {
			WSendPacket packet(GCT_UCC_REQUEST);
			packet.Encode1(1);
			packet.Send(SEND_TO_DEFAULT);
		}
	}

	if (m_pTitleFont) {
		m_pTitleFont->Erase();
	}

	if (g_resrcmng && m_pTitleFont) { g_resrcmng->Release(m_pTitleFont); m_pTitleFont = 0; }

	OpenScratchResultDlg();
}

// scratchdlg.cpp:135
void FrScratchDlg::OnScratchDlgInit(PG_MSGARG) {
}

// scratchdlg.cpp:140
void FrScratchDlg::OnScratchBottomInit(PG_MSGARG param) {
	DYNAMIC_CAST(FrArea, param);
}

// scratchdlg.cpp:146
void FrScratchDlg::OnScratchBottomOwnerDraw(void) {
	DrawScratchItem();
	DrawChangeScratchCardNumber();
}

// scratchdlg.cpp:153
void FrScratchDlg::OnScratchCover01Init(PG_MSGARG param) {
	m_pScratchCover[0] = DYNAMIC_CAST(FrArea, param);

	if (m_pScratchCover[0]) {
		m_pCoverAreaBitmap[0] = m_pScratchCover[0]->GetBgImg();

		ResetBmpScratchCover(0);
	}
}

// scratchdlg.cpp:166
void FrScratchDlg::OnScratchCover01OwnerDraw(void) {
}

// scratchdlg.cpp:171
void FrScratchDlg::OnScratchCover02Init(PG_MSGARG param) {
	m_pScratchCover[1] = DYNAMIC_CAST(FrArea, param);

	if (m_pScratchCover[1]) {
		m_pCoverAreaBitmap[1] = m_pScratchCover[1]->GetBgImg();

		ResetBmpScratchCover(1);
	}
}

// scratchdlg.cpp:184
void FrScratchDlg::OnScratchCover02OwnerDraw(void) {
}

// scratchdlg.cpp:189
void FrScratchDlg::OnScratchCardInit(PG_MSGARG param) {
	m_pScratchCardEdit = DYNAMIC_CAST(FrEdit, param);
}

// scratchdlg.cpp:195
bool FrScratchDlg::OnScratchCardEnterKey(PG_MSGARG) {
	return false;
}

// scratchdlg.cpp:201
void FrScratchDlg::OnScratchNextPageBnInit(PG_MSGARG param) {
	m_pNextPageBn = DYNAMIC_CAST(FrButton, param);

	if (m_pNextPageBn) {
		m_pNextPageBn->SetVisible(false);
	}
}

// scratchdlg.cpp:212
void FrScratchDlg::OnScratchNextPageBnUp(void) {
	if (!m_pNextPageBn) {
		return;
	}

	m_pNextPageBn->Enable(false);

	m_bReceivedScratchItem = false;

	if (m_scratchItem[0].m_typeId == 0) {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);

		if (pForm) {
			pForm->Open((FRESH_RESULT_CALLBACK)&FrScratchDlg::OnConfirmResult, 0xc0);
			pForm->SetFadeout(false);
			// "Next time..."
			pForm->SetMessage("\xB4\xD9\xC0\xBD \xB1\xE2\xC8\xB8\xBF\xA1...", false);
		}

		m_resultTextTime = 0.0f;
		m_bWaitResultText = true;
	}
	else {
		OpenScratchResultDlg();
		m_resultTextTime = 0.0f;
		m_bWaitResultText = true;
		OnLButtonUp(WPoint(0.0f, 0.0f));
	}
}

// scratchdlg.cpp:233
void FrScratchDlg::OnSerialNumberInit(PG_MSGARG param) {
	m_pSerialNumberEdit = DYNAMIC_CAST(FrEdit, param);
}

// scratchdlg.cpp:242
bool FrScratchDlg::OnSerialNumberEnterKey(PG_MSGARG) {
	return false;
}

// scratchdlg.cpp:248
void FrScratchDlg::OnSerialNumberBnInit(PG_MSGARG param) {
	m_pSerialNumberBn = DYNAMIC_CAST(FrButton, param);
}

// scratchdlg.cpp:257
void FrScratchDlg::OnSerialNumberBnUp(void) {
	if (m_pSerialNumberBn) {
		SendScratchSerialNumber();
		EnableScratchSerialNumber(false);
	}
}

// scratchdlg.cpp:267
void FrScratchDlg::OnAboutBnInit(PG_MSGARG param) {
	DYNAMIC_CAST(FrButton, param);
}

// scratchdlg.cpp:273
void FrScratchDlg::OnAboutBnUp(void) {
	FrScratchAboutDlg* pDlg = CreateForm<FrScratchAboutDlg>(g_pFresh->GetManager(), this, "scratch_about", 0);

	if (pDlg) {
		pDlg->Open((FRESH_RESULT_CALLBACK)0, 3);
	}
}

// scratchdlg.cpp:284
void FrScratchDlg::OnCloseBnInit(intptr_t param) {
	DYNAMIC_CAST(FrButton, param);
}

// scratchdlg.cpp:290
void FrScratchDlg::OnCloseBnUp(void) {
	OnFreshOkay();
}

// scratchdlg.cpp:296
bool FrScratchDlg::OnInit(void) {
	if (m_pBmpEraser && m_pScratchCover[0] && m_pScratchCover[1]) {
		m_rectEraseArea.x = (float)(int)(m_pScratchCover[0]->GetRect().x - m_pBmpEraser->Width() / 2);
		m_rectEraseArea.y = (float)(int)(m_pScratchCover[0]->GetRect().y - m_pBmpEraser->Height() / 2);
		m_rectEraseArea.w = (float)(int)(m_pCoverAreaBitmap[0]->Width() + m_pCoverAreaBitmap[1]->Width() + m_pBmpEraser->Width());
		m_rectEraseArea.h = (float)(int)(m_pCoverAreaBitmap[0]->Height() + m_pBmpEraser->Height());
	}

	InitScratchCard();
	SetBmpColsedScratchCover();

	return true;
}

// scratchdlg.cpp:316
void FrScratchDlg::OnProc(const float dt) {
	ProcessChangeScratchCardNumber(dt);
}

// scratchdlg.cpp:324
void FrScratchDlg::OnMouseMove(const WPoint& pt) {
	if (m_bLButtonDown) {
		RequestScratchItem();
		OpenScratchCover(pt);
	}
}

// scratchdlg.cpp:335
bool FrScratchDlg::OnLButtonDown(const WPoint& pt) {
	FrForm::OnLButtonDown(pt);

	if (m_pScratchCover[0] && m_pScratchCover[1]) {
		WRect rect;
		rect.x = m_pScratchCover[0]->GetRect().x + 10.0f;
		rect.y = m_pScratchCover[0]->GetRect().y + 10.0f;
		rect.w = m_bmpScratchCover[1].Width() + m_bmpScratchCover[0].Width() - 20.0f;
		rect.h = m_bmpScratchCover[0].Height() - 20.0f;

		if (rect.IsInRect(pt)) {
			m_bLButtonDown = true;
		}
	}

	return false;
}

// scratchdlg.cpp:356
bool FrScratchDlg::OnLButtonUp(const WPoint& pt) {
	FrForm::OnLButtonUp(pt);

	m_bLButtonDown = false;

	return false;
}

// scratchdlg.cpp:365
void FrScratchDlg::OnCancel(void) {
}

// scratchdlg.cpp:372
void FrScratchDlg::InitScratchCard(void) {
	m_scratchCardCount = 0;

	for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {
		if (ItemManager()->GetCouponKind((*it).m_typeId) == 3) {
			m_scratchCardCount += (*it).m_clubStatLevel[0];
		}
	}

	if (m_pScratchCardEdit) {
		m_pScratchCardEdit->SetLine(1, MakeStr("%d", m_scratchCardCount), 0, false, 0);
	}
}

// scratchdlg.cpp:395
void FrScratchDlg::SetBmpColsedScratchCover(void) {
	if (m_bReceivedScratchItem) {
		return;
	}

	if (m_scratchCardCount == 0) {
		memcpy(m_bmpScratchCover[0].vram, m_pBmpClosedCover->vram, m_pBmpClosedCover->Size());

		m_pScratchCover[0]->SetBgImg(&m_bmpScratchCover[0]);
		GDI()->UpdateTextureCacheInfo(&m_bmpScratchCover[0]);
	}
	else {
		ResetBmpScratchCover(0);
	}

	ResetBmpScratchCover(1);
}

// scratchdlg.cpp:418
void FrScratchDlg::ResetBmpScratchCover(int index) {
	if (m_pCoverAreaBitmap[index]) {
		if (!m_bmpScratchCover[index].bi) {
			m_bmpScratchCover[index].Create(m_pCoverAreaBitmap[index]->Width(), m_pCoverAreaBitmap[index]->Height(), m_pCoverAreaBitmap[index]->BitsPerPixel());
		}

		memcpy(m_bmpScratchCover[index].vram, m_pCoverAreaBitmap[index]->vram, m_pCoverAreaBitmap[index]->Size());
		m_pScratchCover[index]->SetBgImg(&m_bmpScratchCover[index]);
		GDI()->UpdateTextureCacheInfo(&m_bmpScratchCover[index]);
	}
}

// scratchdlg.cpp:437
void FrScratchDlg::OpenScratchCover(const WPoint& pt) {
	if (!m_bReceivedScratchItem) {
		return;
	}
	if (!m_pScratchCover) {
		return;
	}
	if (!m_pBmpEraser) {
		return;
	}

	if (!m_rectEraseArea.IsInRect(pt)) {
		return;
	}

	bool bUpdate[2];
	bUpdate[0] = false;
	bUpdate[1] = false;

	for (unsigned int y = 0; y < m_pBmpEraser->Height(); y++) {
		for (unsigned int x = 0; x < m_pBmpEraser->Width(); x++) {
			unsigned char alpha = const_cast<Bitmap*>(m_pBmpEraser)->GetAlpha(x, y);

			if (alpha) {
				bool bErased = false;
				for (int i = 0; i < 2; i++) {
					if (!bErased) {
						int px = (int)((int)x + pt.x - m_pScratchCover[i]->GetRect().x + m_eraserOffset.x);
						int py = (int)((int)y + pt.y - m_pScratchCover[i]->GetRect().y + m_eraserOffset.y);

						if (px >= 0 && (unsigned int)px < m_bmpScratchCover[i].Width() && py >= 0 && (unsigned int)py < m_bmpScratchCover[i].Height()) {
							m_bmpScratchCover[i].SetAlpha(px, py, 0);
							bUpdate[i] = true;
							bErased = true;
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < 2; i++) {
		if (bUpdate[i]) {
			GDI()->UpdateTextureCacheInfo(&m_bmpScratchCover[i]);
		}
	}
}

// scratchdlg.cpp:496
void FrScratchDlg::EnableNextPageButton(bool bEnable) {
	if (m_pNextPageBn) {
		m_pNextPageBn->SetVisible(bEnable);
		m_pNextPageBn->Enable(true);
	}
}

// scratchdlg.cpp:506
void FrScratchDlg::EnableScratchSerialNumber(bool bEnable) {
	if (m_pSerialNumberBn) {
		m_pSerialNumberBn->Enable(bEnable);
	}

	if (m_pSerialNumberEdit) {
		m_pSerialNumberEdit->ClearLine();
		m_pSerialNumberEdit->Enable(bEnable);
		m_pSerialNumberEdit->SetKeyFocus(bEnable);
		m_pSerialNumberEdit->SetViewFocus(bEnable);
	}

	m_bEnableSerialNumber = bEnable;
}

// scratchdlg.cpp:525
void FrScratchDlg::SendScratchSerialNumber(void) {
	if (m_pSerialNumberEdit) {
		const char* pSerialNumber = m_pSerialNumberEdit->GetLine(1, false);
		int length = strlen(pSerialNumber);

		if (length == 13) {
			WSendPacket packet(GCT_SCRATCH_SERIAL_NUMBER);
			packet.Encode4(13);
			packet.EncodeBuffer(pSerialNumber, 13);
			packet.Send(SEND_TO_DEFAULT);

			m_bEnableSerialNumber = false;
		}
		else {
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);

			if (pForm) {
				pForm->Open((FRESH_RESULT_CALLBACK)&FrScratchDlg::OnConfirmResult, 0xc0);
				pForm->SetFadeout(false);
				// "Serial number" (red) "is not valid." (black)
				pForm->SetMessage("\\c0xffff0000\\c\xBD\xC3\xB8\xAE\xBE\xF3 \xB9\xF8\xC8\xA3\\c0xff000000\\c\xB0\xA1 \xBF\xC3\xB9\xD9\xB8\xA3\xC1\xF6 \xBE\xCA\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			}

			m_resultTextTime = 0.0f;
			m_bWaitResultText = true;
		}
	}
}

// scratchdlg.cpp:550
void FrScratchDlg::ResponseScratchSerialNumber(int result, unsigned long serial) {
	m_serialNumberResult = result;

	switch (result) {
	case 0:
		{
			sBonusBall item = { 0 };
			item.m_typeId = 0x1A000030;
			item.m_serial = serial;
			item.m_count = 1;
			item.m_index = 0;
			item.m_kind = 0;
			item.m_bUnitCount = 0;
			item.m_unitCount = 0;

			UpdateListMyItem(&item);

			InitScratchCard();

			SetBmpColsedScratchCover();
		}
		break;
	case 1:
		{
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);

			if (pForm) {
				pForm->Open((FRESH_RESULT_CALLBACK)&FrScratchDlg::OnConfirmResult, 0xc0);
				pForm->SetFadeout(false);
				// "The serial number has already been used."
				pForm->SetMessage("\xC0\xCC\xB9\xCC \xBB\xE7\xBF\xEB\xC7\xD1 \\c0xffff0000\\c\xBD\xC3\xB8\xAE\xBE\xF3 \xB9\xF8\xC8\xA3\\c0xff000000\\c\xC0\xD4\xB4\xCF\xB4\xD9.", false);
			}

			m_resultTextTime = 0.0f;
			m_bWaitResultText = true;
		}
		break;
	default:
		{
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);

			if (pForm) {
				pForm->Open((FRESH_RESULT_CALLBACK)&FrScratchDlg::OnConfirmResult, 0xc0);
				pForm->SetFadeout(false);
				// "Serial number" (red) "is not valid." (black)
				pForm->SetMessage("\\c0xffff0000\\c\xBD\xC3\xB8\xAE\xBE\xF3 \xB9\xF8\xC8\xA3\\c0xff000000\\c\xB0\xA1 \xBF\xC3\xB9\xD9\xB8\xA3\xC1\xF6 \xBE\xCA\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			}

			m_resultTextTime = 0.0f;
			m_bWaitResultText = true;
		}
		break;
	case 3:
		{
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);

			if (pForm) {
				pForm->Open((FRESH_RESULT_CALLBACK)&FrScratchDlg::OnConfirmResult, 0xc0);
				pForm->SetFadeout(false);
				// "The validity period of this card's serial number has passed."
				pForm->SetMessage("\\c0xffff0000\\c\xC0\xAF\xC8\xBF \xB1\xE2\xB0\xA3\\c0xff000000\\c\xC0\xCC \xC1\xF6\xB3\xAD \xC4\xAB\xB5\xE5\xC0\xC7 \xBD\xC3\xB8\xAE\xBE\xF3 \xB9\xF8\xC8\xA3\xC0\xD4\xB4\xCF\xB4\xD9.", false);
			}

			m_resultTextTime = 0.0f;
			m_bWaitResultText = true;
		}
		break;
	case 4:
		{
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);

			if (pForm) {
				pForm->Open((FRESH_RESULT_CALLBACK)&FrScratchDlg::OnConfirmResult, 0xc0);
				pForm->SetFadeout(false);
				// "Up to 5 event coupons per account within the validity period; all used."
				pForm->SetMessage("\xC0\xCC\xBA\xA5\xC6\xAE\xB7\xCE \xB9\xDE\xC0\xB8\xBD\xC5 \xC4\xED\xC6\xF9\xC0\xBA \xC0\xAF\xC8\xBF\xB1\xE2\xB0\xA3 \xB3\xBB\xBF\xA1 \xC7\xD1 \xB0\xE8\xC1\xA4\xB4\xE7 \xC3\xD6\xB4\xEB 5\xB0\xB3\xB1\xEE\xC1\xF6\xB8\xB8 \xBB\xE7\xBF\xEB\xC0\xCC \xB0\xA1\xB4\xC9\xC7\xD5\xB4\xCF\xB4\xD9. \xC0\xCC\xB9\xCC \xC4\xED\xC6\xF9\xC0\xBB \xB8\xF0\xB5\xCE \xBB\xE7\xBF\xEB\xC7\xCF\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			}

			m_resultTextTime = 0.0f;
			m_bWaitResultText = true;
		}
		break;
	}
}

// scratchdlg.cpp:602
void FrScratchDlg::RequestScratchItem(void) {
	if (m_scratchCardCount == 0) {
		return;
	}

	if (m_bEnableRequestScratchItem) {
		WSendPacket packet(GCT_SCRATCH_ITEM);
		packet.Send(SEND_TO_DEFAULT);

		m_bEnableRequestScratchItem = false;
		EnableScratchSerialNumber(false);
	}
}

// scratchdlg.cpp:619
void FrScratchDlg::SetScratchItem(int count, sBonusBall* pItems) {
	if (!pItems) {
		return;
	}

	memcpy(m_scratchItem, pItems, count * sizeof(sBonusBall));

	m_scratchItemCount = count;
	m_bReceivedScratchItem = true;

	for (int i = 0; i < count; i++) {
		UpdateListMyItem(&pItems[i]);
	}

	InitScratchCard();

	SetBmpScratchItem();

	SetRectScratchItem();

	SetRectCheckScratchItemRect();
}

// scratchdlg.cpp:651
void FrScratchDlg::ClearScratchItem(void) {
	memset(m_scratchItem, 0, sizeof(m_scratchItem));
	m_scratchItemCount = 0;
}

// scratchdlg.cpp:658
void FrScratchDlg::SetBmpResultText(int result) {
	if (result >= 0 && result < 6) {
		if (result == 4) {
			OpenScratchResultDlg();
		}
		else {
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);

			if (pForm) {
				pForm->Open((FRESH_RESULT_CALLBACK)&FrScratchDlg::OnConfirmResult, 0xc0);
				pForm->SetFadeout(false);

				switch (result) {
				case 0:
					// "Serial number" (red) "is not valid." (black)
					pForm->SetMessage("\\c0xffff0000\\c\xBD\xC3\xB8\xAE\xBE\xF3 \xB9\xF8\xC8\xA3\\c0xff000000\\c\xB0\xA1 \xBF\xC3\xB9\xD9\xB8\xA3\xC1\xF6 \xBE\xCA\xBD\xC0\xB4\xCF\xB4\xD9.", false);
					break;
				case 1:
					// "The serial number has already been used."
					pForm->SetMessage("\xC0\xCC\xB9\xCC \xBB\xE7\xBF\xEB\xC7\xD1 \\c0xffff0000\\c\xBD\xC3\xB8\xAE\xBE\xF3 \xB9\xF8\xC8\xA3\\c0xff000000\\c\xC0\xD4\xB4\xCF\xB4\xD9.", false);
					break;
				case 2:
					// "The validity period of this card's serial number has passed."
					pForm->SetMessage("\\c0xffff0000\\c\xC0\xAF\xC8\xBF \xB1\xE2\xB0\xA3\\c0xff000000\\c\xC0\xCC \xC1\xF6\xB3\xAD \xC4\xAB\xB5\xE5\xC0\xC7 \xBD\xC3\xB8\xAE\xBE\xF3 \xB9\xF8\xC8\xA3\xC0\xD4\xB4\xCF\xB4\xD9.", false);
					break;
				case 3:
					// "Up to 5 event coupons per account within the validity period; all used."
					pForm->SetMessage("\xC0\xCC\xBA\xA5\xC6\xAE\xB7\xCE \xB9\xDE\xC0\xB8\xBD\xC5 \xC4\xED\xC6\xF9\xC0\xBA \xC0\xAF\xC8\xBF\xB1\xE2\xB0\xA3 \xB3\xBB\xBF\xA1 \xC7\xD1 \xB0\xE8\xC1\xA4\xB4\xE7 \xC3\xD6\xB4\xEB 5\xB0\xB3\xB1\xEE\xC1\xF6\xB8\xB8 \xBB\xE7\xBF\xEB\xC0\xCC \xB0\xA1\xB4\xC9\xC7\xD5\xB4\xCF\xB4\xD9. \xC0\xCC\xB9\xCC \xC4\xED\xC6\xF9\xC0\xBB \xB8\xF0\xB5\xCE \xBB\xE7\xBF\xEB\xC7\xCF\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.", false);
					break;
				case 5:
					// "Next time..."
					pForm->SetMessage("\xB4\xD9\xC0\xBD \xB1\xE2\xC8\xB8\xBF\xA1...", false);
					break;
				default:
					// "Error"
					pForm->SetMessage("\xBF\xC0\xB7\xF9", false);
					break;
				}
			}
		}

		m_resultTextTime = 0.0f;
		m_bWaitResultText = true;
	}
	else {
		m_bWaitResultText = false;
	}
}

// scratchdlg.cpp:709
void FrScratchDlg::ProcessResultText(float dt) {
	if (m_serialNumberResult == 0) {
		return;
	}

	if (m_bWaitResultText) {
		m_resultTextTime += dt;

		if (m_resultTextTime > 2.0f) {
			m_resultTextTime = 0.0f;
			m_bWaitResultText = false;
			EnableScratchSerialNumber(true);
			EnableNextPageButton(false);

			if (!m_bEnableRequestScratchItem) {
				ClearDataCheckScratchItem();
				ClearScratchItem();
				SetBmpColsedScratchCover();

				m_bEnableRequestScratchItem = true;
			}
		}
	}
}

// scratchdlg.cpp:746
void FrScratchDlg::ClearDataCheckScratchItem(void) {
	memset(m_bCheckedScratchRect, 0, sizeof(m_bCheckedScratchRect));
	memset(m_checkScratchRect, 0, sizeof(m_checkScratchRect));
}

// scratchdlg.cpp:753
void FrScratchDlg::SetBmpScratchItem(void) {
	if (m_scratchItemCount <= 0) {
		m_pBmpScratchItem[0] = g_pFresh->GetBitmap("scratch_text_nothing");
		return;
	}

	for (int i = 0; i < m_scratchItemCount; i++) {
		if (m_scratchItem[i].m_typeId) {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(m_scratchItem[i].m_typeId);

			if (pItem) {
				m_pBmpScratchItem[i] = g_pFresh->GetBitmap(pItem->m_icon);
			}

			if ((m_scratchItem[i].m_typeId & 0xFC000000) == 0x24000000) {
				m_scratchItemCount = 1;
				break;
			}
		} else {
			m_pBmpScratchItem[i] = g_pFresh->GetBitmap("scratch_text_nothing");
			break;
		}
	}
}

// scratchdlg.cpp:801
void FrScratchDlg::SetRectScratchItem(void) {
	if (!m_pBmpScratchItem[0]) {
		return;
	}

	WPoint offset(12.0f, 12.0f);
	if (m_scratchItemCount < 2) {
		if ((m_scratchItem[0].m_typeId & 0xFC000000) != 0x18000000) {
			offset = WPoint(0.0f, 0.0f);
		}

		m_rectScratchItem[2].x = (float)(int)(GetRect().x + GetRect().w * 0.5f + g_ptScratchItem[2].x - m_pBmpScratchItem[0]->Width() / 2 + offset.x);
		m_rectScratchItem[2].y = (float)(int)(GetRect().y + GetRect().h * 0.5f + g_ptScratchItem[2].y - m_pBmpScratchItem[0]->Height() / 2 + offset.y);
		m_rectScratchItem[2].w = (float)(int)m_pBmpScratchItem[0]->Width();
		m_rectScratchItem[2].h = (float)(int)m_pBmpScratchItem[0]->Height();
	}
	else {
		m_rectScratchItem[0].x = (float)(int)(GetRect().x + GetRect().w * 0.5f + g_ptScratchItem[0].x - m_pBmpScratchItem[0]->Width() / 2 + offset.x);
		m_rectScratchItem[0].y = (float)(int)(GetRect().y + GetRect().h * 0.5f + g_ptScratchItem[0].y - m_pBmpScratchItem[0]->Height() / 2 + offset.y);
		m_rectScratchItem[0].w = (float)(int)m_pBmpScratchItem[0]->Width();
		m_rectScratchItem[0].h = (float)(int)m_pBmpScratchItem[0]->Height();

		m_rectScratchItem[1].x = (float)(int)(GetRect().x + GetRect().w * 0.5f + g_ptScratchItem[1].x - m_pBmpScratchItem[1]->Width() / 2 + offset.x);
		m_rectScratchItem[1].y = (float)(int)(GetRect().y + GetRect().h * 0.5f + g_ptScratchItem[1].y - m_pBmpScratchItem[1]->Height() / 2 + offset.y);
		m_rectScratchItem[1].w = (float)(int)m_pBmpScratchItem[1]->Width();
		m_rectScratchItem[1].h = (float)(int)m_pBmpScratchItem[1]->Height();
	}
}

// scratchdlg.cpp:841
void FrScratchDlg::SetRectCheckScratchItemRect(void) {
	if (m_pBmpScratchItem[0]) {
		WPoint offset(24.0f, 24.0f);

		if (m_scratchItemCount < 2) {
			if ((m_scratchItem[0].m_typeId & 0xFC000000) != 0x18000000) {
				offset = WPoint(0.0f, 0.0f);
			}

			SetRectCheckScratchItemRectByIndex(2, offset);
		} else {
			SetRectCheckScratchItemRectByIndex(0, WPoint(24.0f, 24.0f));
			SetRectCheckScratchItemRectByIndex(1, WPoint(24.0f, 24.0f));
		}
	}
}

// scratchdlg.cpp:864
void FrScratchDlg::SetRectCheckScratchItemRectByIndex(int index, WPoint offset) {
	m_checkScratchRect[index][0].w = (float)(int)((m_rectScratchItem[index].w - offset.x) * 0.25f);
	m_checkScratchRect[index][0].h = (float)(int)((m_rectScratchItem[index].h - offset.y) * 0.25f);

	for (int i = 1; i < 4; i++) {
		m_checkScratchRect[index][i].w = m_checkScratchRect[index][0].w;
		m_checkScratchRect[index][i].h = m_checkScratchRect[index][0].h;
	}

	m_checkScratchRect[index][0].x = (float)(int)((GetRect().w * 0.5f + GetRect().x) + g_ptScratchItem[index].x - m_checkScratchRect[index][0].w);
	m_checkScratchRect[index][0].y = (float)(int)((GetRect().h * 0.5f + GetRect().y) + g_ptScratchItem[index].y - m_checkScratchRect[index][0].h);
	m_checkScratchRect[index][1].x = (float)(int)((GetRect().w * 0.5f + GetRect().x) + g_ptScratchItem[index].x);
	m_checkScratchRect[index][1].y = (float)(int)((GetRect().h * 0.5f + GetRect().y) + g_ptScratchItem[index].y - m_checkScratchRect[index][1].h);
	m_checkScratchRect[index][2].x = (float)(int)((GetRect().w * 0.5f + GetRect().x) + g_ptScratchItem[index].x - m_checkScratchRect[index][2].w);
	m_checkScratchRect[index][2].y = (float)(int)((GetRect().h * 0.5f + GetRect().y) + g_ptScratchItem[index].y);
	m_checkScratchRect[index][3].x = (float)(int)((GetRect().w * 0.5f + GetRect().x) + g_ptScratchItem[index].x);
	m_checkScratchRect[index][3].y = (float)(int)((GetRect().h * 0.5f + GetRect().y) + g_ptScratchItem[index].y);
}

// scratchdlg.cpp:890
void FrScratchDlg::CheckScratchItemRect(const WPoint& pt) {
	if (!m_bReceivedScratchItem) {
		return;
	}

	if (m_scratchItemCount < 2) {
		for (int i = 0; i < 4; i++) {
			const WRect& rect = m_checkScratchRect[2][i];
			if (pt.x >= rect.x && pt.x <= rect.x + rect.w && pt.y >= rect.y && pt.y <= rect.y + rect.h) {
				m_bCheckedScratchRect[0][i] = true;
				break;
			}
		}
	} else {
		for (int index = 0; index < m_scratchItemCount; index++) {
			for (int i = 0; i < 4; i++) {
				const WRect& rect = m_checkScratchRect[index][i];
				if (pt.x >= rect.x && pt.x <= rect.x + rect.w && pt.y >= rect.y && pt.y <= rect.y + rect.h) {
					m_bCheckedScratchRect[index][i] = true;
					break;
				}
			}
		}
	}
}

// scratchdlg.cpp:924
void FrScratchDlg::ProcessCheckScratchItemRect(float dt) {
	if (!m_bReceivedScratchItem || m_bWaitResultText) {
		return;
	}

	int count = 0;

	if (m_scratchItemCount < 2) {
		for (int i = 0; i < 4; i++) {
			if (m_bCheckedScratchRect[0][i]) {
				count++;
			}
		}

		if (count < 3) {
			return;
		}
	} else {
		for (int index = 0; index < m_scratchItemCount; index++) {
			for (int i = 0; i < 4; i++) {
				if (m_bCheckedScratchRect[index][i]) {
					count++;
				}
			}
		}

		if (count < m_scratchItemCount * 3) {
			return;
		}
	}

	m_bReceivedScratchItem = false;

	if (m_scratchItem[0].m_typeId == 0) {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);

		if (pForm) {
			pForm->Open((FRESH_RESULT_CALLBACK)&FrScratchDlg::OnConfirmResult, 0xc0);
			pForm->SetFadeout(false);
			// "Next time..."
			pForm->SetMessage("\xB4\xD9\xC0\xBD \xB1\xE2\xC8\xB8\xBF\xA1...", false);
		}

		m_resultTextTime = 0.0f;
		m_bWaitResultText = true;
	}
	else {
		OpenScratchResultDlg();
		m_resultTextTime = 0.0f;
		m_bWaitResultText = true;
		OnLButtonUp(WPoint(0.0f, 0.0f));
	}
}

// scratchdlg.cpp:981
void FrScratchDlg::ProcessChangeScratchCardNumber(float dt) {
	for (int i = 0; i < 2; i++) {
		m_changeCardPos[i] -= (int)(dt * 480.0f);
		m_changeCardTime[i] -= dt * 20.0f;
	}
}

// scratchdlg.cpp:993
void FrScratchDlg::InitChangeScratchCardNumber(int number) {
	if (number < 0 || number >= 2) {
		return;
	}

	int slot = 0;
	if (m_changeCardTime[0] > m_changeCardTime[1]) {
		slot = 1;
	}

	m_changeCardNumber[slot] = number;
	m_changeCardTime[slot] = 0;
	m_changeCardPos[slot] = 0x200;
}

// scratchdlg.cpp:1010
void FrScratchDlg::DrawScratchItem(void) {
	unsigned int j = 0;
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (!pGDI) {
		return;
	}
	if (!m_pBmpScratchItem[0]) {
		return;
	}

	if (m_scratchItemCount < 2) {
		pGDI->DrawTexture(m_pBmpScratchItem[0], m_rectScratchItem[2], 0xFFFFFFFF, 0);

		if (m_scratchItem[0].m_count > 1) {
			char* str = MakeStr("%d", m_scratchItem[0].m_count);
			float x = m_rectScratchItem[2].x + 40.0f;

			for (j = 0; j < strlen(str); j++) {
				x -= m_pTitleFont->GetCharWidthA(g_view, str[j] - '0');
			}

			m_pTitleFont->Print(g_view, x, m_rectScratchItem[2].y + 30.0f, str, 0, 0xFFFFFFFF, 0);
		}
	}
	else {
		for (int i = 0; i < 2; i++) {
			if (m_pBmpScratchItem[i]) {
				pGDI->DrawTexture(m_pBmpScratchItem[i], m_rectScratchItem[i], 0xFFFFFFFF, 0);

				if (m_scratchItem[i].m_count > 1) {
					char* str = MakeStr("%d", m_scratchItem[i].m_count);
					float x = m_rectScratchItem[i].x + 40.0f;

					for (j = 0; j < strlen(str); j++) {
						x -= m_pTitleFont->GetCharWidthA(g_view, str[j] - '0');
					}

					m_pTitleFont->Print(g_view, x, m_rectScratchItem[i].y + 30.0f, str, 0, 0xFFFFFFFF, 0);
				}
			}
		}
	}
}

// scratchdlg.cpp:1066
void FrScratchDlg::DrawChangeScratchCardNumber(void) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI) {
		for (int i = 0; i < 2; i++) {
			if (m_changeCardNumber[i] >= 0
				&& m_pBmpChangeCardNumber[m_changeCardNumber[i]]) {
				unsigned char alpha;

				if (m_changeCardPos[i] < 0) {
					alpha = 0;
					m_changeCardNumber[i] = -1;
				}
				else if (m_changeCardPos[i] > 0xff) {
					alpha = 0xff;
				}
				else {
					alpha = (unsigned char)m_changeCardPos[i];
				}

				WRect rect(m_rect.x + 348.0f, m_changeCardTime[i] + m_rect.y + 113.0f,
					m_pBmpChangeCardNumber[m_changeCardNumber[i]]->Width(), m_pBmpChangeCardNumber[m_changeCardNumber[i]]->Height());

				pGDI->DrawTexture(m_pBmpChangeCardNumber[m_changeCardNumber[i]], rect, (alpha << 24) | 0xffffff, 0);
			}
		}
	}
}

// scratchdlg.cpp:1105
void FrScratchDlg::OpenScratchResultDlg(void) {
	if (m_scratchItem[0].m_typeId) {
		FrScratchResultDlg* pDlg = CreateForm<FrScratchResultDlg>(g_pFresh->GetManager(), this, "scratchresultdlg", 0);

		if (pDlg) {
			pDlg->Open((FRESH_RESULT_CALLBACK)&FrScratchDlg::OnConfirmResult, 0xc0);
			pDlg->SetFadeout(false);
			pDlg->SetResultItem(m_scratchItem, m_scratchItemCount);
		}
	}
}

// scratchdlg.cpp:1120
bool FrScratchDlg::OnConfirmResult(int result, FrForm* pForm) {
	if (result == 1 || result == 2) {
		FrScratchDlg* pDlg = FrStaticCast<FrScratchDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("scratchdlg"));

		if (pDlg) {
			pDlg->EnableScratchSerialNumber(true);
			if (pDlg->m_pNextPageBn) { pDlg->m_pNextPageBn->SetVisible(false); pDlg->m_pNextPageBn->Enable(true); }
			pDlg->ClearDataCheckScratchItem();
			pDlg->ClearScratchItem();
			pDlg->SetBmpColsedScratchCover();
			pDlg->SetEnableRequestScratchItem(true);
		}
	}

	return true;
}

// scratchdlg.cpp:1143
void FrScratchDlg::UpdateListMyItem(sBonusBall* pItem) {
	if (!pItem) {
		return;
	}

	bool bNew = true;
	std::list<sItemInfo>* pList = 0;

	switch (pItem->m_typeId >> 26) {
	case 2:
	case 6:
		pList = &WSingleton<CSharedDoc>::Instance()->m_itemInfoList;
		break;
	case 5:
		pList = &WSingleton<CSharedDoc>::Instance()->m_myGearList;
		break;
	case 8:
		pList = &WSingleton<CSharedDoc>::Instance()->m_myExtraItemList;
		break;
	case 9:
		// The original stores 1 into this dialog's item count here and
		// returns (scratchdlg.cpp:1157).
		m_scratchItemCount = 1;
		return;
	case 0xe:
		pList = &WSingleton<CSharedDoc>::Instance()->m_mySkinList;
		break;
	case 0x1c:
		pList = &WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3;
		break;
	case 0x1d:
		pList = &WSingleton<CSharedDoc>::Instance()->m_myExtraItemList4;
		break;
	}

	if (pList) {
		for (std::list<sItemInfo>::iterator it = pList->begin(); it != pList->end(); ++it) {
			if (it->m_typeId == pItem->m_typeId) {
				if (!it->m_stateBits.kind && !pItem->m_bUnitCount) {
					it->m_count += (short)pItem->m_count;
					bNew = false;
				}
			}
		}
	}

	for (std::map<unsigned int, sCaddieInfo>::iterator itCaddie = WSingleton<CSharedDoc>::Instance()->m_caddieMap.begin(); itCaddie != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end(); ++itCaddie) {
		if (itCaddie->second.typeId == pItem->m_typeId) {
			bNew = false;
			break;
		}
	}

	for (std::map<unsigned int, sItemInfo>::iterator itItem = WSingleton<CSharedDoc>::Instance()->m_myItemMap.begin(); itItem != WSingleton<CSharedDoc>::Instance()->m_myItemMap.end(); ++itItem) {
		if (itItem->second.m_serial == pItem->m_serial) {
			return;
		}
	}

	if (!bNew) {
		return;
	}

	if ((pItem->m_typeId >> 26) == 7) {
		sCaddieInfo caddie = { pItem->m_serial };

		caddie.days = 30;

		caddie.flags &= 0xfc;

		caddie.typeId = pItem->m_typeId;
		caddie.reserved = 0;
		WSingleton<CSharedDoc>::Instance()->m_caddieMap[pItem->m_serial] = caddie;
	} else if ((pItem->m_typeId >> 26) == 4) {
		sItemInfo item;

		item.m_serial = pItem->m_serial;
		item.m_typeId = pItem->m_typeId;

		WSingleton<CSharedDoc>::Instance()->m_myItemMap[pItem->m_serial] = item;
	} else if ((pItem->m_typeId >> 26) == 0x12) {
		unsigned long serial = pItem->m_serial;
		unsigned long typeId = pItem->m_typeId;

		IFF_STRUCT::sFurniture* pFurniture = ItemManager()->FindFurniture(typeId);
		if (!pFurniture) {
			return;
		}

		sFurniture_List furniture;
		memset(&furniture, 0, sizeof(furniture));

		furniture.m_id = serial;
		furniture.m_typeId = typeId;
		furniture.m_pos.x = pFurniture->m_pos[0];
		furniture.m_pos.y = pFurniture->m_pos[1];
		furniture.m_pos.z = pFurniture->m_pos[2];
		furniture.m_angle = pFurniture->m_angle;

		furniture.m_flags &= 0xfe;

		WSingleton<CSharedDoc>::Instance()->AddFurniture(furniture);
	} else {
		sItemInfo item;

		short count[5] = { 1 };
		if (pItem->m_bUnitCount == 0) {
			count[0] = (short)pItem->m_count;
		} else if (pItem->m_bUnitCount == 1 || pItem->m_bUnitCount == 2) {
			count[0] = pItem->m_unitCount;
			count[4] = pItem->m_unitCount;
		}

		item.m_serial = pItem->m_serial;
		item.m_stateBits.flag0 = 0;
		item.m_typeId = pItem->m_typeId;
		item.m_reserved16 = 0;
		memcpy(item.m_clubStatLevel, count, sizeof(count));
		item.m_stateBits.flag0 = 0;
		item.m_stateBits.expired = 0;
		item.m_stateBits.kind = pItem->m_bUnitCount;

		if (item.m_stateBits.kind) {
			item.m_count = 1;
			if (item.m_stateBits.kind == 3) {
				item.m_remainHours = pItem->m_unitCount;
			} else if (item.m_stateBits.kind == 4) {
				item.m_remainHours = pItem->m_unitCount * 24;
			} else if (item.m_stateBits.kind == 5) {
				item.m_remainHours = pItem->m_unitCount * 24 * 30;
			}
		}

		switch (item.m_typeId >> 26) {
		case 2:
			WSingleton<CSharedDoc>::Instance()->AddMyPartsList(item);
			break;
		case 5:
			WSingleton<CSharedDoc>::Instance()->m_myGearList.push_back(item);
			break;
		case 6:
			WSingleton<CSharedDoc>::Instance()->m_itemInfoList.push_back(item);
			break;
		case 8:
			WSingleton<CSharedDoc>::Instance()->m_myExtraItemList.push_back(item);
			break;
		case 9:
			WSingleton<CSharedDoc>::Instance()->m_myExtraItemList2.push_back(item);
			break;
		case 0xe:
			WSingleton<CSharedDoc>::Instance()->m_mySkinList.push_back(item);
			break;
		case 0x1c:
			WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3.push_back(item);
			break;
		case 0x1d:
			WSingleton<CSharedDoc>::Instance()->m_myExtraItemList4.push_back(item);
			break;
		}
	}
}

// scratchdlg.cpp:1311 (run-time type and factory registration of
// FrScratchResultDlg)
const WRTTI FrScratchResultDlg::m_RTTI("FrScratchResultDlg", &FrForm::m_RTTI);

IObject* FrScratchResultDlgMakeInstance(void) {
	return new FrScratchResultDlg;
}

struct __sFrScratchResultDlg {
	__sFrScratchResultDlg(void) {
		ObjectFactory().AddObjectFunctor(FrScratchResultDlgMakeInstance, "FrScratchResultDlg");
	}
};
static __sFrScratchResultDlg s_registerFrScratchResultDlg;

// scratchdlg.cpp:1313
const sFRESH_MSGMAP* FrScratchResultDlg::GetMessageMap(void) const {
	return &FrScratchResultDlg::_MsgMap;
}

sFRESH_MSGMAP FrScratchResultDlg::_MsgMap = { &FrForm::_MsgMap, &FrScratchResultDlg::_MsgEntries[0] };

sFRESH_ENTRY FrScratchResultDlg::_MsgEntries[] = {
	// scratchdlg.cpp:1315
	{ "scratchresultdlg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchResultDlg::OnScratchResultDlgInit },
	// scratchdlg.cpp:1316
	{ "resultitem_icon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchResultDlg::OnResultItemIconInit },
	// scratchdlg.cpp:1317
	{ "resultitem_icon", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrScratchResultDlg::OnResultItemIconOwnerDraw },
	// scratchdlg.cpp:1318
	{ "resultitem_name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchResultDlg::OnResultItemNameInit },
	// scratchdlg.cpp:1319
	{ "resultitem_ability", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchResultDlg::OnResultItemAbilityInit },

	// scratchdlg.cpp:1321
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// scratchdlg.cpp:1324
FrScratchResultDlg::FrScratchResultDlg(void) {
	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	m_pResultItemIcon = 0;
	m_pResultItemName = 0;
	m_pResultItemAbility = 0;

	info.filename = data_a9bea0;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:";
	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&info);

	memset(m_resultItem, 0, sizeof(m_resultItem));
	m_resultItemCount = 0;
	m_bShowItemDetail = false;
	m_pBmpResultItem[0] = 0;
	m_pBmpResultItem[1] = 0;
}

// scratchdlg.cpp:1353
FrScratchResultDlg::~FrScratchResultDlg(void) {
	if (m_pTitleFont) {
		m_pTitleFont->Erase();
	}

	if (g_resrcmng && m_pTitleFont) { g_resrcmng->Release(m_pTitleFont); m_pTitleFont = 0; }
}

// scratchdlg.cpp:1364
void FrScratchResultDlg::OnScratchResultDlgInit(PG_MSGARG) {
}

// scratchdlg.cpp:1369
void FrScratchResultDlg::OnResultItemIconInit(intptr_t param) {
	m_pResultItemIcon = DYNAMIC_CAST(FrArea, param);
}

// scratchdlg.cpp:1375
void FrScratchResultDlg::OnResultItemIconOwnerDraw(void) {
	unsigned int j = 0;
	if (!m_pBmpResultItem[0]) {
		return;
	}
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (!pGDI) {
		return;
	}

	if (m_resultItemCount < 2) {
		WRect rect(m_pResultItemIcon->GetRect().x, m_pResultItemIcon->GetRect().y, m_pBmpResultItem[0]->Width(), m_pBmpResultItem[0]->Height());

		if (m_bShowItemDetail) {
			unsigned long kind = m_resultItem[0].m_typeId >> 26;

			switch (kind) {
			case 2:
			case 7:
			case 8:
				rect.x += 30.0f;
				rect.y += 24.0f;
				break;
			default:
				rect.x = (m_rect.w - m_pBmpResultItem[0]->Width()) * 0.5f + m_rect.x;
				rect.y = m_rect.y + 60.0f;
				break;
			}
		} else {
			rect.x = (m_rect.w - m_pBmpResultItem[0]->Width()) * 0.5f + m_rect.x + 9.0f;
			rect.y = m_rect.y + 60.0f;
		}

		pGDI->DrawTexture(m_pBmpResultItem[0], rect, 0xFFFFFFFF, 0);

		if (m_resultItem[0].m_count > 1) {
			char* str = MakeStr("%d", m_resultItem[0].m_count);
			float x = rect.x + 40.0f;

			for (j = 0; j < strlen(str); j++) {
				x -= m_pTitleFont->GetCharWidthA(g_view, str[j] - '0');
			}

			m_pTitleFont->Print(g_view, x, rect.y + 30.0f, str, 0, 0xFFFFFFFF, 0);
		}
	} else {
		WRect rect((m_rect.w - m_pBmpResultItem[0]->Width()) * 0.5f + m_rect.x, m_rect.y + 60.0f, m_pBmpResultItem[0]->Width(), m_pBmpResultItem[0]->Height());

		for (int i = 0; i < 2; i++) {
			if (m_pBmpResultItem[i]) {
				pGDI->DrawTexture(m_pBmpResultItem[i], WRect(rect.x - 26.0f + i * 70.0f, rect.y, rect.w, rect.h), 0xFFFFFFFF, 0);

				if (m_resultItem[i].m_count > 1) {
					char* str = MakeStr("%d", m_resultItem[i].m_count);
					float x = rect.x + 40.0f - 26.0f + i * 70.0f;

					for (j = 0; j < strlen(str); j++) {
						x -= m_pTitleFont->GetCharWidthA(g_view, str[j] - '0');
					}

					m_pTitleFont->Print(g_view, x, rect.y + 30.0f, str, 0, 0xFFFFFFFF, 0);
				}
			}
		}
	}
}

// scratchdlg.cpp:1468
void FrScratchResultDlg::OnResultItemNameInit(intptr_t param) {
	m_pResultItemName = DYNAMIC_CAST(FrEdit, param);
}

// scratchdlg.cpp:1474
void FrScratchResultDlg::OnResultItemAbilityInit(intptr_t param) {
	m_pResultItemAbility = DYNAMIC_CAST(FrEdit, param);
}

// scratchdlg.cpp:1480
bool FrScratchResultDlg::OnInit(void) {
	return true;
}

// scratchdlg.cpp:1486
void FrScratchResultDlg::OnProc(const float) {
}

// Ten bytes of stat values (five shorts) copied as one unit (the same
// local type as bongdarishopdlg.cpp's).
struct sStatBonus5 {
	short m_value[5];
};

// scratchdlg.cpp:1491
void FrScratchResultDlg::SetResultItem(sBonusBall* pItems, int count) {
	m_pResultItemName->ClearLine();
	m_pResultItemAbility->ClearLine();

	m_pBmpResultItem[0] = 0;
	m_pBmpResultItem[1] = 0;

	if (pItems && count > 0) {
		memcpy(m_resultItem, pItems, sizeof(m_resultItem));
		m_resultItemCount = count;
	} else {
		memset(m_resultItem, 0, sizeof(m_resultItem));
		m_resultItemCount = 0;
		return;
	}

	IFF_ITEM_COMMON* pCommon = 0;

	for (int i = 0; i < m_resultItemCount; i++) {
		if (m_resultItem[i].m_typeId) {
			pCommon = ItemManager()->FindCommonItem(m_resultItem[i].m_typeId);

			if (pCommon) {
				m_pBmpResultItem[i] = g_pFresh->GetManager()->GetBitmap("ITEMS", pCommon->m_icon);
			}
		}
	}

	unsigned long kind = m_resultItem[0].m_typeId >> 26;
	if (kind >= 5 && kind <= 6) {
		return;
	}

	m_bShowItemDetail = true;
	if (!m_pResultItemName) {
		return;
	}
	if (!pCommon) {
		return;
	}

	WRect rect;
	rect = WRect(m_rect.x + 10.0f, m_rect.y + 35.0f, 240.0f, 80.0f);
	m_pResultItemName->SetRect(rect);
	m_pResultItemName->AddText(MakeStr("\\cT\\c%s\\cN\\c", pCommon->m_name), false, true);

	sStatBonus5 slot = { 0 };
	sStatBonus5 base;

	switch (m_resultItem[0].m_typeId >> 26) {
	case 2: {
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(m_resultItem[0].m_typeId);

		if (!pPart)
			return;

		base = *(const sStatBonus5*)pPart->m_baseStat;
		slot = *(const sStatBonus5*)pPart->m_slotStat;
		break;
	}
	case 7: {
		IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(m_resultItem[0].m_typeId);

		if (!pCaddie)
			return;

		base = *(const sStatBonus5*)pCaddie->m_levels;
		break;
	}
	case 8: {
		IFF_STRUCT::sCadItem* pCadItem = ItemManager()->FindCadItem(m_resultItem[0].m_typeId);

		if (!pCadItem)
			return;

		base = *(const sStatBonus5*)pCadItem->m_levels;
		break;
	}
	default:
		return;
	}

	const char* statName = "";

	for (unsigned char j = 0; j < 5; j++) {
		if (base.m_value[j] == 0 && slot.m_value[j] == 0)
			continue;

		switch (j) {
		case 0:
			statName = "\xC6\xC4\xBF\xF6";
			break;
		case 1:
			statName = "\xC4\xC1\xC6\xAE\xB7\xD1";
			break;
		case 2:
			statName = "\xC1\xA4\xC8\xAE\xB5\xB5";
			break;
		case 3:
			statName = "\xBD\xBA\xC7\xC9";
			break;
		case 4:
			statName = "\xC4\xBF\xBA\xEA";
			break;
		}

		if (base.m_value[j] <= 0 && slot.m_value[j] <= 0)
			continue;

		if (base.m_value[j] == 0)
			m_pResultItemAbility->AddText(MakeStr("\\c0xff0000ff\\c%s\xBD\xBD\xB7\xD4%+d\\c0xff000000\\c", statName, slot.m_value[j]), false, true);
		else if (slot.m_value[j] == 0)
			m_pResultItemAbility->AddText(MakeStr("\\c0xff0000ff\\c%s%+d\\c0xff000000\\c", statName, base.m_value[j]), false, true);
		else
			m_pResultItemAbility->AddText(MakeStr("\\c0xff0000ff\\c%s%+d/\xBD\xBD\xB7\xD4%+d\\c0xff000000\\c", statName, base.m_value[j], slot.m_value[j]), false, true);
	}
}

// scratchdlg.cpp:1636 (run-time type and factory registration of
// FrScratchAboutDlg)
const WRTTI FrScratchAboutDlg::m_RTTI("FrScratchAboutDlg", &FrForm::m_RTTI);

IObject* FrScratchAboutDlgMakeInstance(void) {
	return new FrScratchAboutDlg;
}

struct __sFrScratchAboutDlg {
	__sFrScratchAboutDlg(void) {
		ObjectFactory().AddObjectFunctor(FrScratchAboutDlgMakeInstance, "FrScratchAboutDlg");
	}
};
static __sFrScratchAboutDlg s_registerFrScratchAboutDlg;

// scratchdlg.cpp:1638
const sFRESH_MSGMAP* FrScratchAboutDlg::GetMessageMap(void) const {
	return &FrScratchAboutDlg::_MsgMap;
}

sFRESH_MSGMAP FrScratchAboutDlg::_MsgMap = { &FrForm::_MsgMap, &FrScratchAboutDlg::_MsgEntries[0] };

sFRESH_ENTRY FrScratchAboutDlg::_MsgEntries[] = {
	// scratchdlg.cpp:1640
	{ "aboutviewer", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchAboutDlg::OnAboutViewerInit },
	// scratchdlg.cpp:1641
	{ "index01", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchAboutDlg::OnIndex01Init },
	// scratchdlg.cpp:1642
	{ "index01", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScratchAboutDlg::OnIndex01BnUp },
	// scratchdlg.cpp:1643
	{ "index02", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchAboutDlg::OnIndex02Init },
	// scratchdlg.cpp:1644
	{ "index02", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScratchAboutDlg::OnIndex02BnUp },
	// scratchdlg.cpp:1645
	{ "index03", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchAboutDlg::OnIndex03Init },
	// scratchdlg.cpp:1646
	{ "index03", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScratchAboutDlg::OnIndex03BnUp },
	// scratchdlg.cpp:1647
	{ "index04", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchAboutDlg::OnIndex04Init },
	// scratchdlg.cpp:1648
	{ "index04", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScratchAboutDlg::OnIndex04BnUp },
	// scratchdlg.cpp:1649
	{ "index05", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchAboutDlg::OnIndex05Init },
	// scratchdlg.cpp:1650
	{ "index05", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScratchAboutDlg::OnIndex05BnUp },
	// scratchdlg.cpp:1651
	{ "backward", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchAboutDlg::OnBackwardBnInit },
	// scratchdlg.cpp:1652
	{ "backward", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScratchAboutDlg::OnBackwardBnUp },
	// scratchdlg.cpp:1653
	{ "forward", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchAboutDlg::OnForwardBnInit },
	// scratchdlg.cpp:1654
	{ "forward", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScratchAboutDlg::OnForwardBnUp },
	// scratchdlg.cpp:1655
	{ "ok", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScratchAboutDlg::OnOkBnInit },
	// scratchdlg.cpp:1656
	{ "ok", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScratchAboutDlg::OnOkBnUp },
	// scratchdlg.cpp:1658
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// scratchdlg.cpp:1661
FrScratchAboutDlg::FrScratchAboutDlg(void) {
	for (int i = 0; i < 5; i++) {
		m_pIndexBn[i] = 0;
	}

	m_curIndex = 0;
	m_pBackwardBn = 0;
	m_pForwardBn = 0;
}

// scratchdlg.cpp:1675
FrScratchAboutDlg::~FrScratchAboutDlg(void) {
}

// scratchdlg.cpp:1679
void FrScratchAboutDlg::OnAboutViewerInit(intptr_t param) {
	m_pAboutViewer = DYNAMIC_CAST(FrViewer, param);
}

// scratchdlg.cpp:1685
void FrScratchAboutDlg::OnIndex01Init(intptr_t param) {
	m_pIndexBn[0] = DYNAMIC_CAST(FrTextButton, param);
}

// scratchdlg.cpp:1690
void FrScratchAboutDlg::OnIndex01BnUp(void) {
	SetImgAboutIndex(0);
}

// scratchdlg.cpp:1695
void FrScratchAboutDlg::OnIndex02Init(intptr_t param) {
	m_pIndexBn[1] = DYNAMIC_CAST(FrTextButton, param);
}

// scratchdlg.cpp:1700
void FrScratchAboutDlg::OnIndex02BnUp(void) {
	SetImgAboutIndex(1);
}

// scratchdlg.cpp:1705
void FrScratchAboutDlg::OnIndex03Init(intptr_t param) {
	m_pIndexBn[2] = DYNAMIC_CAST(FrTextButton, param);
}

// scratchdlg.cpp:1710
void FrScratchAboutDlg::OnIndex03BnUp(void) {
	SetImgAboutIndex(2);
}

// scratchdlg.cpp:1715
void FrScratchAboutDlg::OnIndex04Init(intptr_t param) {
	m_pIndexBn[3] = DYNAMIC_CAST(FrTextButton, param);
}

// scratchdlg.cpp:1720
void FrScratchAboutDlg::OnIndex04BnUp(void) {
	SetImgAboutIndex(3);
}

// scratchdlg.cpp:1725
void FrScratchAboutDlg::OnIndex05Init(intptr_t param) {
	m_pIndexBn[4] = DYNAMIC_CAST(FrTextButton, param);
}

// scratchdlg.cpp:1730
void FrScratchAboutDlg::OnIndex05BnUp(void) {
	SetImgAboutIndex(4);
}

// scratchdlg.cpp:1735
void FrScratchAboutDlg::OnBackwardBnInit(intptr_t param) {
	m_pBackwardBn = DYNAMIC_CAST(FrButton, param);
}

// scratchdlg.cpp:1740
void FrScratchAboutDlg::OnBackwardBnUp(void) {
	SetImgAboutIndex(--m_curIndex);
	m_pBackwardBn->SetPushDelay(0.0f);
}

// scratchdlg.cpp:1746
void FrScratchAboutDlg::OnForwardBnInit(intptr_t param) {
	m_pForwardBn = DYNAMIC_CAST(FrButton, param);
}

// scratchdlg.cpp:1751
void FrScratchAboutDlg::OnForwardBnUp(void) {
	SetImgAboutIndex(++m_curIndex);
	m_pForwardBn->SetPushDelay(0.0f);
}

// scratchdlg.cpp:1757
void FrScratchAboutDlg::OnOkBnInit(intptr_t param) {
	DYNAMIC_CAST(FrButton, param);
}

// scratchdlg.cpp:1762
void FrScratchAboutDlg::OnOkBnUp(void) {
	OnFreshOkay();
}

// scratchdlg.cpp:1767
bool FrScratchAboutDlg::OnInit(void) {
	SetImgAboutIndex(0);

	return true;
}

// scratchdlg.cpp:1775
void FrScratchAboutDlg::SetImgAboutIndex(int index) {
	for (int i = 0; i < 5; i++) {
		m_pIndexBn[i]->Unselect(0);
	}

	if (m_pBackwardBn) {
		m_pBackwardBn->Enable(true);
	}

	if (m_pForwardBn) {
		m_pForwardBn->Enable(true);
	}

	if (index <= 0) {
		index = 0;
		m_pIndexBn[0]->Select(0);

		if (m_pBackwardBn) {
			m_pBackwardBn->Enable(false);
		}
	}
	else if (index >= 4) {
		index = 4;
		m_pIndexBn[4]->Select(0);

		if (m_pForwardBn) {
			m_pForwardBn->Enable(false);
		}
	}

	m_pIndexBn[index]->Select(0);

	m_curIndex = index;
	if (m_pAboutViewer) {
		m_pAboutViewer->Open(MakeStr("help_scratch_%02d.jpg", index + 1));
	}
}

// Offset of each item rect from the dialog's centre: two-item scratch, then
// single item. Defined after the unit's run-time initializers: WPoint's
// constructor makes it a dynamic initializer, whose `_$E` number would shift
// theirs.
WPoint g_ptScratchItem[] = {
	WPoint(-17.0f, -38.0f), WPoint(23.0f, -38.0f), WPoint(3.0f, -38.0f)
};
