// Original: d:\build\custom\temp\code\client\projectg\carddlg.cpp
#include "CardDlg.h"
#include "ObjectFactory.h"
#include "SharedDoc.h"
#include "NetResourceManager.h"
#include "UccLibrary.h"
#include "UccManager.h"
#include "bitmap.h"
#include "commonutil.h"
#include "frgraphicinterface.h"
#include "frarea.h"
#include "frbutton.h"
#include "fredit.h"
#include "fresh.h"
#include "frlistbox.h"
#include "frstatic.h"
#include "frwndmanager.h"
#include "woverlay.h"
#include "wresrcmng.h"
#include "wview.h"

#include <stdio.h>
#include <string.h>
#include <algorithm>

extern unsigned long g_CurrentTime;

// carddlg.cpp:16 (run-time type and factory registration of FrSpCardUseDlg)
const WRTTI FrSpCardUseDlg::m_RTTI("FrSpCardUseDlg", &FrForm::m_RTTI);

IObject* FrSpCardUseDlgMakeInstance(void) {
	return new FrSpCardUseDlg;
}

struct __sFrSpCardUseDlg {
	__sFrSpCardUseDlg(void) {
		ObjectFactory().AddObjectFunctor(FrSpCardUseDlgMakeInstance, "FrSpCardUseDlg");
	}
};
static __sFrSpCardUseDlg s_registerFrSpCardUseDlg;

// carddlg.cpp:17
const sFRESH_MSGMAP* FrSpCardUseDlg::GetMessageMap(void) const {
	return &FrSpCardUseDlg::_MsgMap;
}

sFRESH_MSGMAP FrSpCardUseDlg::_MsgMap = { &FrForm::_MsgMap, &FrSpCardUseDlg::_MsgEntries[0] };

sFRESH_ENTRY FrSpCardUseDlg::_MsgEntries[] = {
	// carddlg.cpp:19
	{ "item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSpCardUseDlg::OnItemInit },
	// carddlg.cpp:20
	{ "item", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSpCardUseDlg::OnItemOwnerDraw },
	// carddlg.cpp:21
	{ "text1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSpCardUseDlg::OnText1Init },
	// carddlg.cpp:22
	{ "text2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSpCardUseDlg::OnText2Init },

	// carddlg.cpp:24
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// carddlg.cpp:29
FrSpCardUseDlg::FrSpCardUseDlg(void) {
	m_spCardTid = 0;
	m_overlappedCard = 0;
}

// carddlg.cpp:35
FrSpCardUseDlg::~FrSpCardUseDlg(void) {
}

// carddlg.cpp:40
void FrSpCardUseDlg::SetSpCardInfo(unsigned long& spCardTid) {
	m_spCardTid = spCardTid;
	m_overlappedCard = GetOverlappedCard();
}

// carddlg.cpp:46
unsigned long FrSpCardUseDlg::GetOverlappedCard(void) {
	// A special card of the same effect already running (not this card,
	// not expired) is the one the new card would overlap.
	for (std::list<sSCardAvilityPeriodInfo>::iterator it = CSharedDoc::Instance()->m_cardPeriodList[0].begin(); it != CSharedDoc::Instance()->m_cardPeriodList[0].end(); ++it) {
		if (it->typeId == m_spCardTid) {
			continue;
		}

		if (it->kind != 2) {
			continue;
		}

		if (ItemManager()->CompareSystemTime(CSharedDoc::Instance()->GetServerTime(), it->endTime) == 1) {
			continue;
		}

		IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(m_spCardTid);

		if (it->effect == pCard->m_effectType) {
			return it->typeId;
		}
	}

	return 0;
}

// carddlg.cpp:74
void FrSpCardUseDlg::OnItemInit(intptr_t param) {
	m_pItem = DYNAMIC_CAST(FrArea, param);
}

// carddlg.cpp:79
void FrSpCardUseDlg::OnText1Init(intptr_t param) {
	m_pText1 = DYNAMIC_CAST(FrStatic, param);

	if (m_pText1) {
		m_pText1->Enable(false);
		m_pText1->SetVisible(false);
	}
}

// carddlg.cpp:89
void FrSpCardUseDlg::OnText2Init(intptr_t param) {
	m_pText2 = DYNAMIC_CAST(FrStatic, param);

	if (m_pText2) {
		m_pText2->Enable(false);
		m_pText2->SetVisible(false);
	}
}

// carddlg.cpp:99
void FrSpCardUseDlg::OnItemOwnerDraw(intptr_t nIndex) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0) {
		return;
	}

	char desc[512] = { 0 };

	if (m_overlappedCard != 0) {
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGDI->Print(WPoint(m_pItem->GetRect().x + 20.0f, m_pItem->GetRect().y + 11.0f), 0, "\xc7\xf6\xc0\xe7 \xc0\xfb\xbf\xeb\xc1\xdf\xc0\xce \xc4\xab\xb5\xe5 \xc8\xbf\xb0\xfa\xb0\xa1 \xbb\xe8\xc1\xa6\xb5\xc7\xb0\xed");
		pGDI->Print(WPoint(m_pItem->GetRect().x + 20.0f, m_pItem->GetRect().y + 28.0f), 0, "\xbb\xf5\xb7\xce\xbf\xee \xc4\xab\xb5\xe5 \xc8\xbf\xb0\xfa\xb0\xa1 \xc0\xfb\xbf\xeb\xb5\xcb\xb4\xcf\xb4\xd9.");

		IFF_STRUCT::sCard* pOldCard = ItemManager()->FindCard(m_overlappedCard);
		if (pOldCard == 0) {
			return;
		}

		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGDI->SetTextStyle(1);
		pGDI->Print(WPoint(m_pItem->GetRect().x + 20.0f, m_pItem->GetRect().y + 50.0f), 0, "%s", pOldCard->m_name);
		pGDI->SetTextStyle(0);

		IFF_STRUCT::sDesc* pOldDesc = ItemManager()->FindDesc(m_overlappedCard);
		if (pOldDesc != 0) {
			strcpy(desc, pOldDesc->m_text);

			for (int i = 0; i < 512; i++) {
				if (desc[i] == '\n') {
					desc[i] = 0;
					break;
				}
			}

			pGDI->Print(WPoint(m_pItem->GetRect().x + 20.0f, m_pItem->GetRect().y + 70.0f), 0, "%s", desc);
		}

		IFF_STRUCT::sCard* pNewCard = ItemManager()->FindCard(m_spCardTid);
		if (pNewCard == 0) {
			return;
		}

		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGDI->SetTextStyle(1);
		pGDI->Print(WPoint(m_pItem->GetRect().x + 20.0f, m_pItem->GetRect().y + 110.0f), 0, "%s", pNewCard->m_name);
		pGDI->SetTextStyle(0);

		IFF_STRUCT::sDesc* pNewDesc = ItemManager()->FindDesc(m_spCardTid);
		if (pNewDesc != 0) {
			strcpy(desc, pNewDesc->m_text);

			for (int i = 0; i < 512; i++) {
				if (desc[i] == '\n') {
					desc[i] = 0;
					break;
				}
			}

			pGDI->Print(WPoint(m_pItem->GetRect().x + 20.0f, m_pItem->GetRect().y + 130.0f), 0, "%s", desc);
		}

		const Bitmap* pArrow = g_pFresh->GetBitmap("Overlapping_Arrow");
		if (pArrow != 0) {
			WRect rect(m_pItem->GetRect().x + 90.0f, m_pItem->GetRect().y + 90.0f, (float)pArrow->Width(), (float)pArrow->Height());
			pGDI->DrawTexture(pArrow, rect, 0xFFFFFFFF, 0);
		}
	} else {
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGDI->Print(WPoint(m_pItem->GetRect().x + 20.0f, m_pItem->GetRect().y + 11.0f), 0, "\xc0\xcc \xc4\xab\xb5\xe5\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xd8\xbc\xad \xbe\xc6\xc0\xcc\xc5\xdb\xbf\xa1");
		pGDI->Print(WPoint(m_pItem->GetRect().x + 20.0f, m_pItem->GetRect().y + 28.0f), 0, "\xb4\xd9\xc0\xbd \xc6\xaf\xbc\xf6\xc8\xbf\xb0\xfa\xb8\xa6 \xbe\xf2\xc0\xb8\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?");

		IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(m_spCardTid);
		if (pCard == 0) {
			return;
		}

		const Bitmap* pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", pCard->m_image);
		if (pBitmap != 0) {
			WRect rect(m_pItem->GetRect().x + 55.0f, m_pItem->GetRect().y + 100.0f, (float)pBitmap->Width(), (float)pBitmap->Height());
			pGDI->DrawTexture(pBitmap, rect, 0xFFFFFFFF, 0);
		}

		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGDI->SetTextStyle(1);
		pGDI->Print(WPoint(m_pItem->GetRect().x + 20.0f, m_pItem->GetRect().y + 50.0f), 0, "%s", pCard->m_name);
		pGDI->SetTextStyle(0);

		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGDI->SetTextStyle(0);

		IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(m_spCardTid);
		if (pDesc != 0) {
			strcpy(desc, pDesc->m_text);

			for (int i = 0; i < 512; i++) {
				if (desc[i] == '\n') {
					desc[i] = 0;
					break;
				}
			}

			pGDI->Print(WPoint(m_pItem->GetRect().x + 20.0f, m_pItem->GetRect().y + 70.0f), 0, "%s", desc);
		}
	}

	const Bitmap* pTiki = g_pFresh->GetManager()->GetBitmap("ITEMS", "card_tiki1");
	if (pTiki != 0) {
		WRect rect(m_pItem->GetRect().x + 200.0f, m_pItem->GetRect().y + 45.0f, (float)pTiki->Width(), (float)pTiki->Height());
		pGDI->DrawTexture(pTiki, rect, 0xFFFFFFFF, 0);
	}
}

// carddlg.cpp:238 (run-time type and factory registration of FrCardUseDlg)
const WRTTI FrCardUseDlg::m_RTTI("FrCardUseDlg", &FrForm::m_RTTI);

IObject* FrCardUseDlgMakeInstance(void) {
	return new FrCardUseDlg;
}

struct __sFrCardUseDlg {
	__sFrCardUseDlg(void) {
		ObjectFactory().AddObjectFunctor(FrCardUseDlgMakeInstance, "FrCardUseDlg");
	}
};
static __sFrCardUseDlg s_registerFrCardUseDlg;

// carddlg.cpp:239
const sFRESH_MSGMAP* FrCardUseDlg::GetMessageMap(void) const {
	return &FrCardUseDlg::_MsgMap;
}

sFRESH_MSGMAP FrCardUseDlg::_MsgMap = { &FrForm::_MsgMap, &FrCardUseDlg::_MsgEntries[0] };

sFRESH_ENTRY FrCardUseDlg::_MsgEntries[] = {
	// carddlg.cpp:241
	{ "item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardUseDlg::OnItemInit },
	// carddlg.cpp:242
	{ "item", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardUseDlg::OnItemOwnerDraw },

	// carddlg.cpp:244
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// carddlg.cpp:248
FrCardUseDlg::FrCardUseDlg(void) {
	m_cardTypeId = 0;
	m_cardId = 0;

	m_value118 = 0;
	m_value11c = 0;
}

// carddlg.cpp:257
FrCardUseDlg::~FrCardUseDlg(void) {
}

// carddlg.cpp:262
void FrCardUseDlg::OnItemInit(intptr_t param) {
	m_pItem = DYNAMIC_CAST(FrArea, param);
}

// carddlg.cpp:267
void FrCardUseDlg::OnItemOwnerDraw(intptr_t nIndex) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0) {
		return;
	}

	char desc[512] = { 0 };
	IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(m_cardTypeId);
	if (pCard == 0) {
		return;
	}

	int cardType = (m_cardTypeId >> 22) & 0xF;
	const Bitmap* pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", pCard->m_image);
	if (pBitmap != 0) {
		WRect rect(m_pItem->GetRect().x + 55.0f, m_pItem->GetRect().y + 100.0f, (float)pBitmap->Width(), (float)pBitmap->Height());
		pGDI->DrawTexture(pBitmap, rect, 0xFFFFFFFF, 0);
	}

	const Bitmap* pTiki = g_pFresh->GetManager()->GetBitmap("ITEMS", "card_tiki1");
	if (pTiki != 0) {
		WRect rect(m_pItem->GetRect().x + 200.0f, m_pItem->GetRect().y + 45.0f, (float)pTiki->Width(), (float)pTiki->Height());
		pGDI->DrawTexture(pTiki, rect, 0xFFFFFFFF, 0);
	}

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	pGDI->SetTextStyle(1);
	pGDI->Print(WPoint(m_pItem->GetRect().x + 20.0f, m_pItem->GetRect().y + 50.0f), 0, "%s", pCard->m_name);
	pGDI->SetTextStyle(0);

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	pGDI->SetTextStyle(0);

	if (cardType == 2 || cardType == 1) {
		IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(m_cardTypeId);
		if (pDesc == 0) {
			return;
		}

		strcpy(desc, pDesc->m_text);

		for (int i = 0; i < 512; i++) {
			if (desc[i] == '\n') {
				desc[i] = 0;
				break;
			}
		}

		pGDI->Print(WPoint(m_pItem->GetRect().x + 20.0f, m_pItem->GetRect().y + 70.0f), 0, "%s", desc);
	} else if (cardType == 0) {
		float offset = 0.0f;
		for (int i = 0; i < 5; i++) {
			if (pCard->m_stat[i] > 0) {
				if (i == 0) {
					sprintf(desc, "%s + %d", "\xc6\xc4\xbf\xf6\xbd\xbd\xb7\xd4", pCard->m_stat[0]);
				} else if (i == 1) {
					sprintf(desc, "%s + %d", "\xc4\xc1\xc6\xae\xb7\xd1\xbd\xbd\xb7\xd4", pCard->m_stat[1]);
				} else if (i == 2) {
					sprintf(desc, "%s + %d", "\xc1\xa4\xc8\xae\xb5\xb5\xbd\xbd\xb7\xd4", pCard->m_stat[2]);
				} else if (i == 3) {
					sprintf(desc, "%s + %d", "\xbd\xba\xc7\xc9\xbd\xbd\xb7\xd4", pCard->m_stat[3]);
				} else if (i == 4) {
					sprintf(desc, "%s + %d", "\xc4\xbf\xba\xea\xbd\xbd\xb7\xd4", pCard->m_stat[4]);
				}

				pGDI->Print(WPoint(m_pItem->GetRect().x + 20.0f, m_pItem->GetRect().y + (offset + 70.0f)), 0, "%s", desc);
				offset += 15.0f;
			}
		}
	}
}

// carddlg.cpp:428 (run-time type and factory registration of FrOpenCardPackDlg)
const WRTTI FrOpenCardPackDlg::m_RTTI("FrOpenCardPackDlg", &FrForm::m_RTTI);

IObject* FrOpenCardPackDlgMakeInstance(void) {
	return new FrOpenCardPackDlg;
}

struct __sFrOpenCardPackDlg {
	__sFrOpenCardPackDlg(void) {
		ObjectFactory().AddObjectFunctor(FrOpenCardPackDlgMakeInstance, "FrOpenCardPackDlg");
	}
};
static __sFrOpenCardPackDlg s_registerFrOpenCardPackDlg;

// carddlg.cpp:429
const sFRESH_MSGMAP* FrOpenCardPackDlg::GetMessageMap(void) const {
	return &FrOpenCardPackDlg::_MsgMap;
}

sFRESH_MSGMAP FrOpenCardPackDlg::_MsgMap = { &FrForm::_MsgMap, &FrOpenCardPackDlg::_MsgEntries[0] };

sFRESH_ENTRY FrOpenCardPackDlg::_MsgEntries[] = {
	// carddlg.cpp:431
	{ "item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOpenCardPackDlg::OnItemInit },
	// carddlg.cpp:432
	{ "item", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOpenCardPackDlg::OnItemOwnerDraw },
	// carddlg.cpp:433
	{ "levelImg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOpenCardPackDlg::OnLevelImgInit },

	// carddlg.cpp:435
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// carddlg.cpp:439
FrOpenCardPackDlg::FrOpenCardPackDlg(void) {
	m_cardTypeId = 0;
	m_cardId = 0;
}

// carddlg.cpp:445
FrOpenCardPackDlg::~FrOpenCardPackDlg(void) {
}

// carddlg.cpp:450
void FrOpenCardPackDlg::OnLevelImgInit(intptr_t param) {
	m_pLevelImg = DYNAMIC_CAST(FrArea, param);
}

// carddlg.cpp:455
void FrOpenCardPackDlg::OnItemInit(intptr_t param) {
	m_pItem = DYNAMIC_CAST(FrArea, param);
}

// carddlg.cpp:460
void FrOpenCardPackDlg::OnItemOwnerDraw(intptr_t nIndex) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0) {
		return;
	}

	char desc[512] = { 0 };
	IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(m_cardTypeId);
	if (pCard == 0) {
		return;
	}

	int cardType = (m_cardTypeId >> 22) & 0xF;
	const Bitmap* pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", pCard->m_icon);
	if (pBitmap != 0) {
		WRect rect(m_pItem->GetRect().x + 20.0f, m_pItem->GetRect().y + 50.0f, (float)pBitmap->Width(), (float)pBitmap->Height());
		pGDI->DrawTexture(pBitmap, rect, 0xFFFFFFFF, 0);
	}

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	pGDI->SetTextStyle(1);
	pGDI->Print(WPoint(m_pItem->GetRect().x + 210.0f, m_pItem->GetRect().y + 55.0f), 0, "%s", pCard->m_name);
	pGDI->SetTextStyle(0);

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	pGDI->SetTextStyle(1);
	pGDI->Print(WPoint(m_pItem->GetRect().x + 160.0f, m_pItem->GetRect().y + 55.0f), 0, "%s", "\xC0\xCC\xB8\xA7: ");
	pGDI->SetTextStyle(0);

	char text[100] = { 0 };

	// The level text goes through a pointer (the original copies it byte by byte).
	if (pCard->m_level & 0x80) {
		const char* levelText = "\xB7\xB9\xBA\xA7:                \xC0\xCC\xC7\xCF";
		strcpy(text, levelText);
	} else {
		const char* levelText = "\xB7\xB9\xBA\xA7:                \xC0\xCC\xBB\xF3";
		strcpy(text, levelText);
	}

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	pGDI->SetTextStyle(1);
	pGDI->Print(WPoint(m_pItem->GetRect().x + 160.0f, m_pItem->GetRect().y + 75.0f), 0, "%s", text);
	pGDI->SetTextStyle(0);

	if (m_pLevelImg != 0) {
		m_pLevelImg->SetBgImg(MakeStr("level_%03d", (pCard->m_level & 0x7F) + 1));
	}

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	pGDI->SetTextStyle(0);

	if (cardType == 3) {
		IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(m_cardTypeId);
		if (pDesc == 0) {
			return;
		}

		strcpy(desc, pDesc->m_text);

		for (int i = 0; i < 512; i++) {
			if (desc[i] == '\n') {
				desc[i] = 0;
				break;
			}
		}

		pGDI->Print(WPoint(m_pItem->GetRect().x + 25.0f, m_pItem->GetRect().y + 155.0f), 0, "%s", desc);
	}
}

// carddlg.cpp:536 (run-time type and factory registration of FrOpenCardPackResultDlg)
const WRTTI FrOpenCardPackResultDlg::m_RTTI("FrOpenCardPackResultDlg", &FrForm::m_RTTI);

IObject* FrOpenCardPackResultDlgMakeInstance(void) {
	return new FrOpenCardPackResultDlg;
}

struct __sFrOpenCardPackResultDlg {
	__sFrOpenCardPackResultDlg(void) {
		ObjectFactory().AddObjectFunctor(FrOpenCardPackResultDlgMakeInstance, "FrOpenCardPackResultDlg");
	}
};
static __sFrOpenCardPackResultDlg s_registerFrOpenCardPackResultDlg;

// carddlg.cpp:537
const sFRESH_MSGMAP* FrOpenCardPackResultDlg::GetMessageMap(void) const {
	return &FrOpenCardPackResultDlg::_MsgMap;
}

sFRESH_MSGMAP FrOpenCardPackResultDlg::_MsgMap = { &FrForm::_MsgMap, &FrOpenCardPackResultDlg::_MsgEntries[0] };

sFRESH_ENTRY FrOpenCardPackResultDlg::_MsgEntries[] = {
	// carddlg.cpp:539
	{ "item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOpenCardPackResultDlg::OnItemInit },
	// carddlg.cpp:540
	{ "item", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOpenCardPackResultDlg::OnItemOwnerDraw },
	// carddlg.cpp:541
	{ "item", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOpenCardPackResultDlg::OnItemOwnerDraw },
	// carddlg.cpp:542
	{ "cardpack_frm_num", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOpenCardPackResultDlg::OnCardFrmNumInit },

	// carddlg.cpp:544
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// carddlg.cpp:548
FrOpenCardPackResultDlg::FrOpenCardPackResultDlg(void) {

	for (int i = 0; i < 3; i++) {
		m_values[i] = 0;
	}
}

// carddlg.cpp:554
FrOpenCardPackResultDlg::~FrOpenCardPackResultDlg(void) {
}

// carddlg.cpp:559
void FrOpenCardPackResultDlg::OnItemInit(intptr_t param) {
	m_pItem = DYNAMIC_CAST(FrArea, param);
}

// carddlg.cpp:564
void FrOpenCardPackResultDlg::OnCardFrmNumInit(intptr_t param) {
	FrArea* pFrmNum = DYNAMIC_CAST(FrArea, param);
	m_pCardFrmNum = pFrmNum;

	if (m_values[1] && m_values[2] && m_value11c) {
		pFrmNum->SetBgImg("cardpack_frm_num_3");
	} else {
		pFrmNum->SetBgImg("cardpack_frm_num_1");
	}
}

// carddlg.cpp:574
void FrOpenCardPackResultDlg::OnItemOwnerDraw(intptr_t nIndex) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0) {
		return;
	}

	char buffer[32];   // unused; the original's frame keeps its /GS cookie

	if (m_values[0] != 0) {
		IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(m_values[0]);
		if (pCard == 0) {
			return;
		}

		const Bitmap* pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", pCard->m_icon);
		if (pBitmap != 0) {
			WRect rect(m_pItem->GetRect().x + 30.0f, m_pItem->GetRect().y + 85.0f, (float)pBitmap->Width(), (float)pBitmap->Height());
			pGDI->DrawTexture(pBitmap, rect, 0xFFFFFFFF, 0);
		}

		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGDI->PrintText(WPoint(m_pItem->GetRect().x + 16.0f, m_pItem->GetRect().y + 160.0f), 0, MakeStr("%s", pCard->m_name), 0);
	}

	if (m_values[1] != 0) {
		IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(m_values[1]);
		if (pCard == 0) {
			return;
		}

		const Bitmap* pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", pCard->m_icon);
		if (pBitmap != 0) {
			WRect rect(m_pItem->GetRect().x + 126.0f, m_pItem->GetRect().y + 94.0f, (float)pBitmap->Width(), (float)pBitmap->Height());
			pGDI->DrawTexture(pBitmap, rect, 0xFFFFFFFF, 0);
		}
	}

	if (m_values[2] != 0 && m_value11c != 0) {
		IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(m_values[2]);
		if (pCard == 0) {
			return;
		}

		const Bitmap* pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", pCard->m_icon);
		if (pBitmap != 0) {
			WRect rect(m_pItem->GetRect().x + 178.0f, m_pItem->GetRect().y + 94.0f, (float)pBitmap->Width(), (float)pBitmap->Height());
			pGDI->DrawTexture(pBitmap, rect, 0xFFFFFFFF, 0);
		}

		pCard = ItemManager()->FindCard(m_value11c);
		if (pCard == 0) {
			return;
		}

		pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", pCard->m_icon);
		if (pBitmap != 0) {
			WRect rect(m_pItem->GetRect().x + 230.0f, m_pItem->GetRect().y + 94.0f, (float)pBitmap->Width(), (float)pBitmap->Height());
			pGDI->DrawTexture(pBitmap, rect, 0xFFFFFFFF, 0);
		}
	}
}

// carddlg.cpp:644 (run-time type and factory registration of FrCardAttachDlg)
const WRTTI FrCardAttachDlg::m_RTTI("FrCardAttachDlg", &FrForm::m_RTTI);

IObject* FrCardAttachDlgMakeInstance(void) {
	return new FrCardAttachDlg;
}

struct __sFrCardAttachDlg {
	__sFrCardAttachDlg(void) {
		ObjectFactory().AddObjectFunctor(FrCardAttachDlgMakeInstance, "FrCardAttachDlg");
	}
};
static __sFrCardAttachDlg s_registerFrCardAttachDlg;

// carddlg.cpp:645
const sFRESH_MSGMAP* FrCardAttachDlg::GetMessageMap(void) const {
	return &FrCardAttachDlg::_MsgMap;
}

sFRESH_MSGMAP FrCardAttachDlg::_MsgMap = { &FrForm::_MsgMap, &FrCardAttachDlg::_MsgEntries[0] };

sFRESH_ENTRY FrCardAttachDlg::_MsgEntries[] = {
	// carddlg.cpp:647
	{ "item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardAttachDlg::OnItemInit },
	// carddlg.cpp:648
	{ "item", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardAttachDlg::OnItemOwnerDraw },
	// carddlg.cpp:649
	{ "cardavility", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardAttachDlg::OnCardAvilityInit },
	// carddlg.cpp:650
	{ "cardavilityold", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardAttachDlg::OnCardAvilityOldInit },

	// carddlg.cpp:652
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// carddlg.cpp:656
FrCardAttachDlg::FrCardAttachDlg(void) {
	m_cardTypeId = 0;
	m_cardId = 0;

	m_value128 = 0;
	m_value12c = 0;

	m_charSlot = 0;
	m_caddieSlot = 0;

	m_pCardAvility = 0;
	m_pCardAvilityOld = 0;
}

// carddlg.cpp:671
FrCardAttachDlg::~FrCardAttachDlg(void) {
}

// carddlg.cpp:676
bool FrCardAttachDlg::OnSetDlgTxt(void) {
	if (m_cardTypeId == 0)
		return false;

	IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(m_cardTypeId);
	if (!pCard)
		return false;

	char text[512] = { 0 };
	int cardType = (m_cardTypeId >> 22) & 0xF;
	if (cardType == 0) {
		for (int i = 0; i < 5; i++) {
			if (pCard->m_stat[i] > 0) {
				if (i == 0) {
					sprintf(text, "%s + %d", "\xC6\xC4\xBF\xF6\xBD\xBD\xB7\xD4", pCard->m_stat[0]);
				} else if (i == 1) {
					sprintf(text, "%s + %d", "\xC4\xC1\xC6\xAE\xB7\xD1\xBD\xBD\xB7\xD4", pCard->m_stat[1]);
				} else if (i == 2) {
					sprintf(text, "%s + %d", "\xC1\xA4\xC8\xAE\xB5\xB5\xBD\xBD\xB7\xD4", pCard->m_stat[2]);
				} else if (i == 3) {
					sprintf(text, "%s + %d", "\xBD\xBA\xC7\xC9\xBD\xBD\xB7\xD4", pCard->m_stat[3]);
				} else if (i == 4) {
					sprintf(text, "%s + %d", "\xC4\xBF\xBA\xEA\xBD\xBD\xB7\xD4", pCard->m_stat[4]);
				}

				m_pCardAvility->AddText(MakeStr("\\cT\\c%s\\cN\\c", text), false, true);
			}
		}
	} else if (cardType == 1) {
		IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(m_cardTypeId);
		if (pDesc) {
			strcpy(text, pDesc->m_text);

			for (int i = 0; i < 512; i++) {
				if (text[i] == '\n') {
					text[i] = 0;
					break;
				}
			}

			m_pCardAvility->AddText(MakeStr("\\cT\\c%s\\cN\\c", text), false, true);
		}
	}

	if (m_charSlot > 0 || m_caddieSlot > 0) {
		for (std::list<sSCardAvilityPeriodInfo>::iterator it = CSharedDoc::Instance()->m_cardPeriodList[0].begin(); it != CSharedDoc::Instance()->m_cardPeriodList[0].end(); ++it) {
			unsigned long typeId = it->typeId;
			int kind = (typeId >> 22) & 0xF;

			if (it->partsTypeId == m_itemTypeId && it->partsId == m_itemId) {
				if (kind == 0 && m_charSlot == it->slot) {
					IFF_STRUCT::sCard* pOldCard = ItemManager()->FindCard(typeId);
					if (pOldCard) {
						for (int i = 0; i < 5; i++) {
							if (pOldCard->m_stat[i] > 0) {
								memset(text, 0, sizeof(text));
								if (i == 0) {
									sprintf(text, "%s + %d", "\xC6\xC4\xBF\xF6\xBD\xBD\xB7\xD4", pOldCard->m_stat[0]);
								} else if (i == 1) {
									sprintf(text, "%s + %d", "\xC4\xC1\xC6\xAE\xB7\xD1\xBD\xBD\xB7\xD4", pOldCard->m_stat[1]);
								} else if (i == 2) {
									sprintf(text, "%s + %d", "\xC1\xA4\xC8\xAE\xB5\xB5\xBD\xBD\xB7\xD4", pOldCard->m_stat[2]);
								} else if (i == 3) {
									sprintf(text, "%s + %d", "\xBD\xBA\xC7\xC9\xBD\xBD\xB7\xD4", pOldCard->m_stat[3]);
								} else if (i == 4) {
									sprintf(text, "%s + %d", "\xC4\xBF\xBA\xEA\xBD\xBD\xB7\xD4", pOldCard->m_stat[4]);
								}

								m_pCardAvilityOld->AddText(MakeStr("\\cT\\c%s\\cN\\c", text), false, true);
							}
						}
					}
				} else if (kind == 1 && m_caddieSlot == it->slot) {
					IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(typeId);
					if (pDesc) {
						strcpy(text, pDesc->m_text);

						for (int i = 0; i < 512; i++) {
							if (text[i] == '\n') {
								text[i] = 0;
								break;
							}
						}

						m_pCardAvilityOld->AddText(MakeStr("\\cT\\c%s\\cN\\c", text), false, true);
					}
				}
			}
		}
	}

	return true;
}

// carddlg.cpp:805
void FrCardAttachDlg::OnCardAvilityInit(intptr_t param) {
	m_pCardAvility = DYNAMIC_CAST(FrEdit, param);
	m_pCardAvility->SetVisible(true);
}

// carddlg.cpp:811
void FrCardAttachDlg::OnCardAvilityOldInit(intptr_t param) {
	m_pCardAvilityOld = DYNAMIC_CAST(FrEdit, param);
	m_pCardAvilityOld->SetVisible(true);
}

// carddlg.cpp:817
void FrCardAttachDlg::OnItemInit(intptr_t param) {
	m_pItem = DYNAMIC_CAST(FrArea, param);
}

// carddlg.cpp:822
void FrCardAttachDlg::OnItemOwnerDraw(intptr_t nIndex) {
}

// carddlg.cpp:846
CTFrameMove::CTFrameMove(void)
	: m_count(3) {

	m_frames.reserve(m_count);
}

// carddlg.cpp:857
CTFrameMove::CTFrameMove(unsigned char count)
	: m_count(count) {

	m_frames.reserve(count);
}

// carddlg.cpp:867
CTFrameMove::~CTFrameMove(void) {
	Reset();
}

// carddlg.cpp:872
void CTFrameMove::AddFrame(FrArea* pFrame) {
	if (pFrame != 0) {
		m_frames.push_back(pFrame);
	}
}

// carddlg.cpp:878
bool CTFrameMove::SetFrameWidth(float width) {
	unsigned char start;
	unsigned char end;

	if (0.0f == width) {
		return false;
	}

	unsigned char count = (unsigned char)m_frames.size();

	if (count == 3) {
		start = count;
		end = 6;
	} else if (count == 9) {
		start = 0;
		end = 3;
	}

	for (unsigned char i = start; i < end; i++) {
		WRect rect;
		rect = m_frames[i]->GetRect();
		rect.w += width;
		m_frames[i]->SetRect(rect);
	}

	return true;
}

// carddlg.cpp:911
bool CTFrameMove::SetFrameHeight(float height) {
	unsigned char start;
	unsigned char middle;   // end of the stretched frames, start of the moved ones
	unsigned char end;

	if (0.0f == height) {
		return false;
	}

	unsigned char count = (unsigned char)m_frames.size();

	if (count == 6) {
		start = 0;
		middle = 3;
		end = 6;
	}

	unsigned char i;
	for (i = start; i < middle; i++) {
		WRect rect;
		rect = m_frames[i]->GetRect();
		rect.h += height;
		m_frames[i]->SetRect(rect);
	}

	for (i = middle; i < end; i++) {
		WRect rect;
		rect = m_frames[i]->GetRect();
		rect.y += height;
		m_frames[i]->SetRect(rect);
	}

	return true;
}

// carddlg.cpp:948 (run-time type and factory registration of FrItemAttachCardDlg)
const WRTTI FrItemAttachCardDlg::m_RTTI("FrItemAttachCardDlg", &FrForm::m_RTTI);

IObject* FrItemAttachCardDlgMakeInstance(void) {
	return new FrItemAttachCardDlg;
}

struct __sFrItemAttachCardDlg {
	__sFrItemAttachCardDlg(void) {
		ObjectFactory().AddObjectFunctor(FrItemAttachCardDlgMakeInstance, "FrItemAttachCardDlg");
	}
};
static __sFrItemAttachCardDlg s_registerFrItemAttachCardDlg;

// carddlg.cpp:950
const sFRESH_MSGMAP* FrItemAttachCardDlg::GetMessageMap(void) const {
	return &FrItemAttachCardDlg::_MsgMap;
}

sFRESH_MSGMAP FrItemAttachCardDlg::_MsgMap = { &FrForm::_MsgMap, &FrItemAttachCardDlg::_MsgEntries[0] };

sFRESH_ENTRY FrItemAttachCardDlg::_MsgEntries[] = {
	// carddlg.cpp:952
	{ "attach_item_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnMyItemListInit },
	// carddlg.cpp:953
	{ "attach_item_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnMyItemListLClick },
	// carddlg.cpp:954
	{ "attach_item_list", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnMyItemListRClick },
	// carddlg.cpp:955
	{ "attach_item_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnMyItemListOwnerDraw },
	// carddlg.cpp:956
	{ "attach_card_icon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnAttachCardIconInit },
	// carddlg.cpp:957
	{ "attach_card_icon", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnAttachCardIconOwnerDraw },
	// carddlg.cpp:958
	{ "attach_card_name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnAttachCardNameInit },
	// carddlg.cpp:959
	{ "attach_card_ability", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnAttachCardAbilityInit },
	// carddlg.cpp:960
	{ "attach_item_icon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnAttachItemIconInit },
	// carddlg.cpp:961
	{ "attach_item_icon", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnAttachItemIconOwnerDraw },
	// carddlg.cpp:962
	{ "attach_item_name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnAttachItemNameInit },
	// carddlg.cpp:963
	{ "attach_item_ability", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnAttachItemAbilityInit },
	// carddlg.cpp:964
	{ "attach_char_img", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnAreaCharImgInit },
	// carddlg.cpp:965
	{ "foldbtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnItemListFoldInit },
	// carddlg.cpp:966
	{ "foldbtn", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnItemListFoldLClick },
	// carddlg.cpp:967
	{ "unfoldbtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnItemListUnfoldInit },
	// carddlg.cpp:968
	{ "unfoldbtn", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnItemListUnfoldLClick },
	// carddlg.cpp:969
	{ "card_frm_04", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnDlgFrm_04Init },
	// carddlg.cpp:970
	{ "card_frm_05", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnDlgFrm_05Init },
	// carddlg.cpp:971
	{ "card_frm_06", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnDlgFrm_06Init },
	// carddlg.cpp:972
	{ "card_frm_07", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnDlgFrm_07Init },
	// carddlg.cpp:973
	{ "card_frm_08", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnDlgFrm_08Init },
	// carddlg.cpp:974
	{ "card_frm_09", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnDlgFrm_09Init },
	// carddlg.cpp:975
	{ "charbtn_1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnCharSlotBtnInit_1 },
	// carddlg.cpp:976
	{ "charbtn_1", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnCharSlotBtnDown_1 },
	// carddlg.cpp:977
	{ "charbtn_1", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnCharSlotBtnOwnerDraw_1 },
	// carddlg.cpp:978
	{ "charbtn_2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnCharSlotBtnInit_2 },
	// carddlg.cpp:979
	{ "charbtn_2", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnCharSlotBtnDown_2 },
	// carddlg.cpp:980
	{ "charbtn_2", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnCharSlotBtnOwnerDraw_2 },
	// carddlg.cpp:981
	{ "charbtn_3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnCharSlotBtnInit_3 },
	// carddlg.cpp:982
	{ "charbtn_3", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnCharSlotBtnDown_3 },
	// carddlg.cpp:983
	{ "charbtn_3", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnCharSlotBtnOwnerDraw_3 },
	// carddlg.cpp:984
	{ "charbtn_4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnCharSlotBtnInit_4 },
	// carddlg.cpp:985
	{ "charbtn_4", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnCharSlotBtnDown_4 },
	// carddlg.cpp:986
	{ "charbtn_4", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnCharSlotBtnOwnerDraw_4 },
	// carddlg.cpp:987
	{ "caddiebtn_1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnCaddieSlotBtnInit_1 },
	// carddlg.cpp:988
	{ "caddiebtn_1", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnCaddieSlotBtnDown_1 },
	// carddlg.cpp:989
	{ "caddiebtn_1", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnCaddieSlotBtnOwnerDraw_1 },
	// carddlg.cpp:990
	{ "caddiebtn_2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnCaddieSlotBtnInit_2 },
	// carddlg.cpp:991
	{ "caddiebtn_2", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnCaddieSlotBtnDown_2 },
	// carddlg.cpp:992
	{ "caddiebtn_2", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnCaddieSlotBtnOwnerDraw_2 },
	// carddlg.cpp:993
	{ "caddiebtn_3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnCaddieSlotBtnInit_3 },
	// carddlg.cpp:994
	{ "caddiebtn_3", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnCaddieSlotBtnDown_3 },
	// carddlg.cpp:995
	{ "caddiebtn_3", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnCaddieSlotBtnOwnerDraw_3 },
	// carddlg.cpp:996
	{ "caddiebtn_4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemAttachCardDlg::OnCaddieSlotBtnInit_4 },
	// carddlg.cpp:997
	{ "caddiebtn_4", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnCaddieSlotBtnDown_4 },
	// carddlg.cpp:998
	{ "caddiebtn_4", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrItemAttachCardDlg::OnCaddieSlotBtnOwnerDraw_4 },

	// carddlg.cpp:1000
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// carddlg.cpp:1003
FrItemAttachCardDlg::FrItemAttachCardDlg(void) {
	m_pMyItemList = 0;
	m_pAttachCardIcon = 0;
	m_pAttachCardName = 0;
	m_pAttachCardAbility = 0;
	m_pAttachItemIcon = 0;
	m_pAttachItemName = 0;
	m_pAttachItemAbility = 0;

	for (int i = 0; i < 4; i++) {
		m_pCharSlotBtn[i] = 0;
		m_pCaddieSlotBtn[i] = 0;
	}

	m_itemTypeId = 0;
	m_itemId = 0;

	m_pLineOverlay = g_resrcmng->GetOverlay("line.tga", 0);

	m_pCardAttachDlg = 0;

	m_pMyItemBaseClickBmp = g_pFresh->GetBitmap("trade_myitem_base_click");
	m_pMyItemBaseOverBmp = g_pFresh->GetBitmap("trade_myitem_base_over");

	m_myItemList.clear();

	m_pSelectedItem = 0;
	initCardSlotBtnTxt();

	m_selectedSlot = 0xFF;

	m_bItemListFolded = false;

	m_pEmptySlotBmp = g_pFresh->GetBitmap("card_character_empty");
	m_pEmptyCaddieSlotBmp = g_pFresh->GetBitmap("card_caddie_empty");

	m_pUccStateBmp[0] = g_pFresh->RegisterBitmap("ucc_state_original");
	m_pUccStateBmp[1] = g_pFresh->RegisterBitmap("ucc_state_copied");
	m_pUccStateBmp[2] = g_pFresh->RegisterBitmap("ucc_state_temp_saved");
}

// carddlg.cpp:1048
FrItemAttachCardDlg::~FrItemAttachCardDlg(void) {
	if (g_resrcmng && m_pLineOverlay) { g_resrcmng->Release(m_pLineOverlay); m_pLineOverlay = 0; }
}

// carddlg.cpp:1068
bool FrItemAttachCardDlg::OnInit(void) {
	EnableDrag(false);

	SetListCardUsableMyItem();
	SetListMyItem();
	SetSelectMyItem(0);
	SetCardInfo(m_cardTypeId);

	return true;
}

// carddlg.cpp:1080
void FrItemAttachCardDlg::initCardSlotBtnTxt(void) {
	memset(m_charSlotBtnTxt[0], 0, sizeof(m_charSlotBtnTxt[0]));
	memset(m_charSlotBtnTxt[1], 0, sizeof(m_charSlotBtnTxt[1]));
	memset(m_charSlotBtnTxt[2], 0, sizeof(m_charSlotBtnTxt[2]));
	memset(m_charSlotBtnTxt[3], 0, sizeof(m_charSlotBtnTxt[3]));
	memset(m_caddieSlotBtnTxt[0], 0, sizeof(m_caddieSlotBtnTxt[0]));
	memset(m_caddieSlotBtnTxt[1], 0, sizeof(m_caddieSlotBtnTxt[1]));
	memset(m_caddieSlotBtnTxt[2], 0, sizeof(m_caddieSlotBtnTxt[2]));
	memset(m_caddieSlotBtnTxt[3], 0, sizeof(m_caddieSlotBtnTxt[3]));

	for (int i = 0; i < 4; i++) {
		m_charSlotAttached[i] = false;
		m_caddieSlotAttached[i] = false;

		m_charSlotCard[i] = 0;
		m_caddieSlotCard[i] = 0;
	}
}

// carddlg.cpp:1101
void FrItemAttachCardDlg::OnAttachCardIconInit(PG_MSGARG param) {
	m_pAttachCardIcon = DYNAMIC_CAST(FrArea, param);
}

// carddlg.cpp:1106
void FrItemAttachCardDlg::OnAttachCardIconOwnerDraw(void) {
	if (m_cardTypeId) {
		WPoint pt(m_pAttachCardIcon->GetRect().x, m_pAttachCardIcon->GetRect().y);

		DrawItem(pt, m_cardTypeId, m_cardId);
	}
}

// carddlg.cpp:1117
void FrItemAttachCardDlg::OnAttachCardNameInit(PG_MSGARG param) {
	m_pAttachCardName = DYNAMIC_CAST(FrEdit, param);
}

// carddlg.cpp:1123
void FrItemAttachCardDlg::OnAttachCardAbilityInit(PG_MSGARG param) {
	m_pAttachCardAbility = DYNAMIC_CAST(FrEdit, param);
}

// carddlg.cpp:1128
void FrItemAttachCardDlg::OnAttachItemIconInit(PG_MSGARG param) {
	m_pAttachItemIcon = DYNAMIC_CAST(FrArea, param);
}

// carddlg.cpp:1133
void FrItemAttachCardDlg::OnAttachItemIconOwnerDraw(void) {
	if (m_pSelectedItem) {
		WPoint pt(m_pAttachItemIcon->GetRect().x, m_pAttachItemIcon->GetRect().y);

		DrawItem(pt, m_pSelectedItem->typeId, m_pSelectedItem->id);
		DrawPartsSlot(pt, m_pSelectedItem->typeId, m_pSelectedItem->id, 0);
	}
}

// carddlg.cpp:1145
void FrItemAttachCardDlg::OnAttachItemNameInit(PG_MSGARG param) {
	m_pAttachItemName = DYNAMIC_CAST(FrEdit, param);
}

// carddlg.cpp:1151
void FrItemAttachCardDlg::OnAttachItemAbilityInit(PG_MSGARG param) {
	m_pAttachItemAbility = DYNAMIC_CAST(FrEdit, param);
}

// carddlg.cpp:1156
void FrItemAttachCardDlg::OnAreaCharImgInit(PG_MSGARG param) {
	m_pAreaCharImg = DYNAMIC_CAST(FrArea, param);
}

// carddlg.cpp:1161
void FrItemAttachCardDlg::OnItemListFoldInit(PG_MSGARG param) {
	m_pItemListFold = DYNAMIC_CAST(FrButton, param);
	m_pItemListFold->SetVisible(true);
}

// carddlg.cpp:1167
void FrItemAttachCardDlg::OnItemListUnfoldInit(PG_MSGARG param) {
	m_pItemListUnfold = DYNAMIC_CAST(FrButton, param);

	m_pItemListUnfold->SetVisible(false);
}

// carddlg.cpp:1174
void FrItemAttachCardDlg::OnItemListFoldLClick(void) {
	WRect rect;
	m_pMyItemList->GetClientRect(rect);
	rect.h += 100.0f;
	m_pMyItemList->SetClientRect(rect);

	m_cardSlotMove.SetFrameHeight(100.0f);

	m_pItemListFold->SetVisible(false);
	m_pItemListUnfold->SetVisible(true);

	m_bItemListFolded = true;
}

// carddlg.cpp:1189
void FrItemAttachCardDlg::OnItemListUnfoldLClick(void) {
	WRect rect;
	m_pMyItemList->GetClientRect(rect);
	rect.h -= 100.0f;
	m_pMyItemList->SetClientRect(rect);

	m_cardSlotMove.SetFrameHeight(-100.0f);

	m_pItemListFold->SetVisible(true);
	m_pItemListUnfold->SetVisible(false);

	m_bItemListFolded = false;
}

// carddlg.cpp:1203
void FrItemAttachCardDlg::OnDlgFrm_04Init(PG_MSGARG param) {
	m_cardSlotMove.AddFrame(DYNAMIC_CAST(FrArea, param));
}

// carddlg.cpp:1208
void FrItemAttachCardDlg::OnDlgFrm_05Init(PG_MSGARG param) {
	m_cardSlotMove.AddFrame(DYNAMIC_CAST(FrArea, param));
}

// carddlg.cpp:1213
void FrItemAttachCardDlg::OnDlgFrm_06Init(PG_MSGARG param) {
	m_cardSlotMove.AddFrame(DYNAMIC_CAST(FrArea, param));
}

// carddlg.cpp:1218
void FrItemAttachCardDlg::OnDlgFrm_07Init(PG_MSGARG param) {
	m_cardSlotMove.AddFrame(DYNAMIC_CAST(FrArea, param));
}

// carddlg.cpp:1223
void FrItemAttachCardDlg::OnDlgFrm_08Init(PG_MSGARG param) {
	m_cardSlotMove.AddFrame(DYNAMIC_CAST(FrArea, param));
}

// carddlg.cpp:1228
void FrItemAttachCardDlg::OnDlgFrm_09Init(PG_MSGARG param) {
	m_cardSlotMove.AddFrame(DYNAMIC_CAST(FrArea, param));
}

// carddlg.cpp:1233
bool FrItemAttachCardDlg::OnCardAttachResult(int ret, FrForm* pForm) {
	if (ret == FrOK) {

		ResetCardSlotInfo();
	} else if (ret == FrCANCEL) {

		SetPartsSlotBtnTxt();
		m_selectedSlot = 0xff;
	}

	m_pCardAttachDlg = 0;
	return true;
}

// carddlg.cpp:1312
void FrItemAttachCardDlg::OnOpenAttachDlg(int charSlot, int caddieSlot) {
	m_pCardAttachDlg = CreateForm<FrCardAttachDlg>(g_pFresh->GetManager(), this, "attach_card", 0);

	m_pCardAttachDlg->m_cardTypeId = m_cardTypeId;
	m_pCardAttachDlg->m_cardId = m_cardId;
	m_pCardAttachDlg->m_itemTypeId = m_itemTypeId;
	m_pCardAttachDlg->m_itemId = m_itemId;
	m_pCardAttachDlg->m_charSlot = charSlot;
	m_pCardAttachDlg->m_caddieSlot = caddieSlot;
	m_pCardAttachDlg->OnSetDlgTxt();

	m_pCardAttachDlg->Open((FRESH_RESULT_CALLBACK)&FrItemAttachCardDlg::OnAgreeAttachingResult, 1);
}

// carddlg.cpp:1331
void FrItemAttachCardDlg::SetCharacterAttatchBtnTxt(unsigned long typeId, char* text) {
	if (m_cardTypeId == 0) {
		strcpy(text, "Card Info error...");
	}

	char result[50] = { 0 };
	char stat[255] = { 0 };
	int count = 0;

	IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(typeId);
	if (pCard != 0) {
		for (int i = 0; i < 5; i++) {
			if (pCard->m_stat[i] > 0) {
				if (count == 1) {
					strcat(result, ",");
				}

				memset(stat, 0, sizeof(stat));
				if (i == 0) {
					sprintf(stat, "%s + %d", "\xC6\xC4\xBF\xF6\xBD\xBD\xB7\xD4", pCard->m_stat[0]);
					strcat(result, stat);
					count++;
				} else if (i == 1) {
					sprintf(stat, "%s + %d", "\xC4\xC1\xC6\xAE\xB7\xD1\xBD\xBD\xB7\xD4", pCard->m_stat[1]);
					strcat(result, stat);
					count++;
				} else if (i == 2) {
					sprintf(stat, "%s + %d", "\xC1\xA4\xC8\xAE\xB5\xB5\xBD\xBD\xB7\xD4", pCard->m_stat[2]);
					strcat(result, stat);
					count++;
				} else if (i == 3) {
					sprintf(stat, "%s + %d", "\xBD\xBA\xC7\xC9\xBD\xBD\xB7\xD4", pCard->m_stat[3]);
					strcat(result, stat);
					count++;
				} else if (i == 4) {
					sprintf(stat, "%s + %d", "\xC4\xBF\xBA\xEA\xBD\xBD\xB7\xD4", pCard->m_stat[4]);
					strcat(result, stat);
					count++;
				}
			}
		}
	}

	strcpy(text, result);
}

// carddlg.cpp:1388
void FrItemAttachCardDlg::SetCaddieAttatchBtnTxt(unsigned long typeId, char* text) {
	if (m_cardTypeId == 0) {
		strcpy(text, "Card Info error...");
	}

	char desc[512] = { 0 };

	IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(typeId);
	if (pDesc) {

		strcpy(desc, pDesc->m_text);

		for (int i = 0; i < 512; i++) {

			if (desc[i] == '\n') {

				desc[i] = 0;
				break;
			}
		}
	}

	strncpy(text, desc, 0x31);
}

// carddlg.cpp:1414
void FrItemAttachCardDlg::DrawBtnCharSlotimg(float x, float y, unsigned long cardTypeId) {
	IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(cardTypeId);
	if (pCard) {

		FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
		if (pGDI) {

			const Bitmap* pBitmap = g_pFresh->GetBitmap(pCard->m_texture);
			if (pBitmap) {
				pGDI->DrawTexture(pBitmap, WRect(x + 5.0f, y + 4.0f, pBitmap->Width(), pBitmap->Height()), 0xFFFFFFFF, 0);
			}
		}
	}
}

// carddlg.cpp:1436
void FrItemAttachCardDlg::DrawBtnCaddieSlotimg(float x, float y, unsigned long cardTypeId) {
	IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(cardTypeId);
	if (pCard) {

		FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
		if (pGDI) {

			const Bitmap* pBitmap = g_pFresh->GetBitmap(MakeStr(pCard->m_texture));
			if (pBitmap) {
				pGDI->DrawTexture(pBitmap, WRect(x + 5.0f, y + 4.0f, pBitmap->Width(), pBitmap->Height()), 0xFFFFFFFF, 0);
			}
		}
	}
}

// carddlg.cpp:1458
void FrItemAttachCardDlg::OnCharSlotBtnInit_1(PG_MSGARG param) {
	m_pCharSlotBtn[0] = DYNAMIC_CAST(FrButton, param);
	m_pCharSlotBtn[0]->Enable(false);
}

// carddlg.cpp:1464
void FrItemAttachCardDlg::OnCharSlotBtnDown_1(void) {
	if (m_cardKind == 0) {
		OnOpenAttachDlg(1, 0);
		m_selectedSlot = 1;
	} else {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "Only character cards can be used."
		pForm->SetMessage("\xC4\xB3\xB8\xAF\xC5\xCD \xC4\xAB\xB5\xE5\xB8\xB8 \xBB\xE7\xBF\xEB\xC7\xD2 \xBC\xF6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.", false);
		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
	}
}

// carddlg.cpp:1482
void FrItemAttachCardDlg::OnCharSlotBtnOwnerDraw_1(void) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	const WRect& rect = m_pCharSlotBtn[0]->GetRect();

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	pGDI->Print11(WPoint(rect.x + 20.0f, rect.y + 6.0f), 0, "%s", m_charSlotBtnTxt[0]);

	if (!m_charSlotAttached[0]) {

		const Bitmap* pBitmap = m_pEmptySlotBmp;
		if (pBitmap) {
			pGDI->DrawTexture(pBitmap, WRect(rect.x + 5.0f, rect.y + 4.0f, pBitmap->Width(), pBitmap->Height()), 0xFFFFFFFF, 0);
		}
	} else {
		DrawBtnCharSlotimg(rect.x, rect.y, m_charSlotCard[0]);
	}
}

// carddlg.cpp:1505
void FrItemAttachCardDlg::OnCharSlotBtnInit_2(PG_MSGARG param) {
	m_pCharSlotBtn[1] = DYNAMIC_CAST(FrButton, param);
	m_pCharSlotBtn[1]->Enable(false);
}

// carddlg.cpp:1511
void FrItemAttachCardDlg::OnCharSlotBtnDown_2(void) {
	if (m_cardKind == 0) {
		OnOpenAttachDlg(2, 0);
		m_selectedSlot = 2;
	} else {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "Only character cards can be used."
		pForm->SetMessage("\xC4\xB3\xB8\xAF\xC5\xCD \xC4\xAB\xB5\xE5\xB8\xB8 \xBB\xE7\xBF\xEB\xC7\xD2 \xBC\xF6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.", false);
		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
	}
}

// carddlg.cpp:1529
void FrItemAttachCardDlg::OnCharSlotBtnOwnerDraw_2(void) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	const WRect& rect = m_pCharSlotBtn[1]->GetRect();

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	pGDI->Print11(WPoint(rect.x + 20.0f, rect.y + 6.0f), 0, "%s", m_charSlotBtnTxt[1]);

	if (!m_charSlotAttached[1]) {

		const Bitmap* pBitmap = m_pEmptySlotBmp;
		if (pBitmap) {
			pGDI->DrawTexture(pBitmap, WRect(rect.x + 5.0f, rect.y + 4.0f, pBitmap->Width(), pBitmap->Height()), 0xFFFFFFFF, 0);
		}
	} else {
		DrawBtnCharSlotimg(rect.x, rect.y, m_charSlotCard[1]);
	}
}

// carddlg.cpp:1552
void FrItemAttachCardDlg::OnCharSlotBtnInit_3(PG_MSGARG param) {
	m_pCharSlotBtn[2] = DYNAMIC_CAST(FrButton, param);
	m_pCharSlotBtn[2]->Enable(false);
}

// carddlg.cpp:1558
void FrItemAttachCardDlg::OnCharSlotBtnDown_3(void) {
	if (m_cardKind == 0) {
		OnOpenAttachDlg(3, 0);
		m_selectedSlot = 3;
	} else {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "Only character cards can be used."
		pForm->SetMessage("\xC4\xB3\xB8\xAF\xC5\xCD \xC4\xAB\xB5\xE5\xB8\xB8 \xBB\xE7\xBF\xEB\xC7\xD2 \xBC\xF6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.", false);
		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
	}
}

// carddlg.cpp:1576
void FrItemAttachCardDlg::OnCharSlotBtnOwnerDraw_3(void) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	const WRect& rect = m_pCharSlotBtn[2]->GetRect();

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	pGDI->Print11(WPoint(rect.x + 20.0f, rect.y + 6.0f), 0, "%s", m_charSlotBtnTxt[2]);

	if (!m_charSlotAttached[2]) {

		const Bitmap* pBitmap = m_pEmptySlotBmp;
		if (pBitmap) {
			pGDI->DrawTexture(pBitmap, WRect(rect.x + 5.0f, rect.y + 4.0f, pBitmap->Width(), pBitmap->Height()), 0xFFFFFFFF, 0);
		}
	} else {
		DrawBtnCharSlotimg(rect.x, rect.y, m_charSlotCard[2]);
	}
}

// carddlg.cpp:1599
void FrItemAttachCardDlg::OnCharSlotBtnInit_4(PG_MSGARG param) {
	m_pCharSlotBtn[3] = DYNAMIC_CAST(FrButton, param);
	m_pCharSlotBtn[3]->Enable(false);
}

// carddlg.cpp:1605
void FrItemAttachCardDlg::OnCharSlotBtnDown_4(void) {
	if (m_cardKind == 0) {
		OnOpenAttachDlg(4, 0);
		m_selectedSlot = 4;
	} else {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "Only character cards can be used."
		pForm->SetMessage("\xC4\xB3\xB8\xAF\xC5\xCD \xC4\xAB\xB5\xE5\xB8\xB8 \xBB\xE7\xBF\xEB\xC7\xD2 \xBC\xF6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.", false);
		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
	}
}

// carddlg.cpp:1623
void FrItemAttachCardDlg::OnCharSlotBtnOwnerDraw_4(void) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	const WRect& rect = m_pCharSlotBtn[3]->GetRect();

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	pGDI->Print11(WPoint(rect.x + 20.0f, rect.y + 6.0f), 0, "%s", m_charSlotBtnTxt[3]);

	if (!m_charSlotAttached[3]) {

		const Bitmap* pBitmap = m_pEmptySlotBmp;
		if (pBitmap) {
			pGDI->DrawTexture(pBitmap, WRect(rect.x + 5.0f, rect.y + 4.0f, pBitmap->Width(), pBitmap->Height()), 0xFFFFFFFF, 0);
		}
	} else {
		DrawBtnCharSlotimg(rect.x, rect.y, m_charSlotCard[3]);
	}
}

// carddlg.cpp:1646
void FrItemAttachCardDlg::OnCaddieSlotBtnInit_1(PG_MSGARG param) {
	m_pCaddieSlotBtn[0] = DYNAMIC_CAST(FrButton, param);
	m_pCaddieSlotBtn[0]->Enable(false);
}

// carddlg.cpp:1652
void FrItemAttachCardDlg::OnCaddieSlotBtnDown_1(void) {
	if (m_cardKind == 1) {
		OnOpenAttachDlg(0, 1);
		m_selectedSlot = 5;
	} else {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "Only caddie cards can be used."
		pForm->SetMessage("\xC4\xB3\xB5\xF0 \xC4\xAB\xB5\xE5\xB8\xB8 \xBB\xE7\xBF\xEB\xC7\xD2 \xBC\xF6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.", false);
		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
	}
}

// carddlg.cpp:1670
void FrItemAttachCardDlg::OnCaddieSlotBtnOwnerDraw_1(void) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	const WRect& rect = m_pCaddieSlotBtn[0]->GetRect();

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	pGDI->Print11(WPoint(rect.x + 20.0f, rect.y + 6.0f), 0, "%s", m_caddieSlotBtnTxt[0]);

	if (!m_caddieSlotAttached[0]) {

		const Bitmap* pBitmap = m_pEmptyCaddieSlotBmp;
		if (pBitmap) {
			pGDI->DrawTexture(pBitmap, WRect(rect.x + 5.0f, rect.y + 4.0f, pBitmap->Width(), pBitmap->Height()), 0xFFFFFFFF, 0);
		}
	} else {
		DrawBtnCaddieSlotimg(rect.x, rect.y, m_caddieSlotCard[0]);
	}
}

// carddlg.cpp:1692
void FrItemAttachCardDlg::OnCaddieSlotBtnInit_2(PG_MSGARG param) {
	m_pCaddieSlotBtn[1] = DYNAMIC_CAST(FrButton, param);
	m_pCaddieSlotBtn[1]->Enable(false);
}

// carddlg.cpp:1698
void FrItemAttachCardDlg::OnCaddieSlotBtnDown_2(void) {
	if (m_cardKind == 1) {
		OnOpenAttachDlg(0, 2);
		m_selectedSlot = 6;
	} else {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "Only caddie cards can be used."
		pForm->SetMessage("\xC4\xB3\xB5\xF0 \xC4\xAB\xB5\xE5\xB8\xB8 \xBB\xE7\xBF\xEB\xC7\xD2 \xBC\xF6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.", false);
		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
	}
}

// carddlg.cpp:1716
void FrItemAttachCardDlg::OnCaddieSlotBtnOwnerDraw_2(void) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	const WRect& rect = m_pCaddieSlotBtn[1]->GetRect();

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	pGDI->Print11(WPoint(rect.x + 20.0f, rect.y + 6.0f), 0, "%s", m_caddieSlotBtnTxt[1]);

	if (!m_caddieSlotAttached[1]) {

		const Bitmap* pBitmap = m_pEmptyCaddieSlotBmp;
		if (pBitmap) {
			pGDI->DrawTexture(pBitmap, WRect(rect.x + 5.0f, rect.y + 4.0f, pBitmap->Width(), pBitmap->Height()), 0xFFFFFFFF, 0);
		}
	} else {
		DrawBtnCaddieSlotimg(rect.x, rect.y, m_caddieSlotCard[1]);
	}
}

// carddlg.cpp:1738
void FrItemAttachCardDlg::OnCaddieSlotBtnInit_3(PG_MSGARG param) {
	m_pCaddieSlotBtn[2] = DYNAMIC_CAST(FrButton, param);
	m_pCaddieSlotBtn[2]->Enable(false);
}

// carddlg.cpp:1744
void FrItemAttachCardDlg::OnCaddieSlotBtnDown_3(void) {
	if (m_cardKind == 1) {
		OnOpenAttachDlg(0, 3);
		m_selectedSlot = 7;
	} else {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "Only caddie cards can be used."
		pForm->SetMessage("\xC4\xB3\xB5\xF0 \xC4\xAB\xB5\xE5\xB8\xB8 \xBB\xE7\xBF\xEB\xC7\xD2 \xBC\xF6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.", false);
		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
	}
}

// carddlg.cpp:1762
void FrItemAttachCardDlg::OnCaddieSlotBtnOwnerDraw_3(void) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	const WRect& rect = m_pCaddieSlotBtn[2]->GetRect();

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	pGDI->Print11(WPoint(rect.x + 20.0f, rect.y + 6.0f), 0, "%s", m_caddieSlotBtnTxt[2]);

	if (!m_caddieSlotAttached[2]) {

		const Bitmap* pBitmap = m_pEmptyCaddieSlotBmp;
		if (pBitmap) {
			pGDI->DrawTexture(pBitmap, WRect(rect.x + 5.0f, rect.y + 4.0f, pBitmap->Width(), pBitmap->Height()), 0xFFFFFFFF, 0);
		}
	} else {
		DrawBtnCaddieSlotimg(rect.x, rect.y, m_caddieSlotCard[2]);
	}
}

// carddlg.cpp:1785
void FrItemAttachCardDlg::OnCaddieSlotBtnInit_4(PG_MSGARG param) {
	m_pCaddieSlotBtn[3] = DYNAMIC_CAST(FrButton, param);
	m_pCaddieSlotBtn[3]->Enable(false);
}

// carddlg.cpp:1791
void FrItemAttachCardDlg::OnCaddieSlotBtnDown_4(void) {
	if (m_cardKind == 1) {
		OnOpenAttachDlg(0, 4);
		m_selectedSlot = 8;
	} else {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "Only caddie cards can be used."
		pForm->SetMessage("\xC4\xB3\xB5\xF0 \xC4\xAB\xB5\xE5\xB8\xB8 \xBB\xE7\xBF\xEB\xC7\xD2 \xBC\xF6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.", false);
		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
	}
}

// carddlg.cpp:1809
void FrItemAttachCardDlg::OnCaddieSlotBtnOwnerDraw_4(void) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	const WRect& rect = m_pCaddieSlotBtn[3]->GetRect();

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	pGDI->Print11(WPoint(rect.x + 20.0f, rect.y + 6.0f), 0, "%s", m_caddieSlotBtnTxt[3]);

	if (!m_caddieSlotAttached[3]) {

		const Bitmap* pBitmap = m_pEmptyCaddieSlotBmp;
		if (pBitmap) {
			pGDI->DrawTexture(pBitmap, WRect(rect.x + 5.0f, rect.y + 4.0f, pBitmap->Width(), pBitmap->Height()), 0xFFFFFFFF, 0);
		}
	} else {
		DrawBtnCaddieSlotimg(rect.x, rect.y, m_caddieSlotCard[3]);
	}
}

// carddlg.cpp:1833
void FrItemAttachCardDlg::OnProc(float fDelta) {
}

// carddlg.cpp:1839
void FrItemAttachCardDlg::SetListMyItem(void) {
	if (m_pMyItemList) {

		m_pMyItemList->ClearItem();

		std::list<sItemAttachCard>::iterator it;
		for (it = m_myItemList.begin(); it != m_myItemList.end(); ++it) {

			m_pMyItemList->AddItem(&*it);
		}
	}
}

// carddlg.cpp:1855
void FrItemAttachCardDlg::OnMyItemListInit(intptr_t param) {
	m_pMyItemList = DYNAMIC_CAST(FrListBox, param);

	if (m_pMyItemList) {

		m_pMyItemList->UseRightButton(true);
	}

	// "Used a card on the clothes."
	SetDesc("\xC0\xC7\xBB\xF3\xBF\xA1 \xC4\xAB\xB5\xE5\xB8\xA6 \xBB\xE7\xBF\xEB\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.");
}

// carddlg.cpp:1871
void FrItemAttachCardDlg::OnMyItemListLClick(void) {
	FrListItem* pItem = m_pMyItemList->GetItemUnderCursor();

	if (pItem) {
		m_selectedSlot = 0xff;

		if (!pItem->selected) {
			m_pMyItemList->ToggleItem(pItem);
		} else {
			SetSelectMyItem((sItemAttachCard*)pItem->pData);
		}
	}
}

// carddlg.cpp:1893
void FrItemAttachCardDlg::OnMyItemListRClick(void) {
}

// carddlg.cpp:1900
void FrItemAttachCardDlg::OnMyItemListOwnerDraw(intptr_t nIndex) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0) {
		return;
	}

	FrListItem* pItem = (FrListItem*)nIndex;
	if (pItem == 0) {
		return;
	}

	sItemAttachCard* pData = (sItemAttachCard*)pItem->pData;
	const Bitmap* pBaseBmp = 0;

	if (pItem->selected) {
		pBaseBmp = m_pMyItemBaseClickBmp;
		pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	} else if (pItem->underCursor) {
		pBaseBmp = m_pMyItemBaseOverBmp;
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	} else {
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	}

	if (pBaseBmp != 0) {
		WRect rect(pItem->pos.x + 2.0f, pItem->pos.y, (float)pBaseBmp->Width(), (float)pBaseBmp->Height());

		pGDI->DrawTexture(pBaseBmp, rect, 0xFFFFFFFF, 0);
	}

	if ((pData->typeId & 0xFC000000) == 0x08000000) {
		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(((pData->typeId >> 18) & 0xFF) | 0x04000000);
		if (pChar != 0) {
			const Bitmap* pCharBmp = g_pFresh->GetBitmap(MakeStr("s_%s", pChar->m_icon));
			if (pCharBmp != 0) {
				WRect rect(pItem->pos.x + 7.0f, pItem->pos.y + 5.0f, (float)pCharBmp->Width(), (float)pCharBmp->Height());
				pGDI->DrawTexture(pCharBmp, rect, 0xFFFFFFFF, 0);
			}
		}
	}

	WPoint pt(pItem->pos.x + 13.0f, pItem->pos.y + 20.0f);
	DrawItem(pt, pData->typeId, pData->id);
	DrawPartsSlot(pt, pData->typeId, pData->id, pGDI);
	DrawPartsLine();
}

// carddlg.cpp:1963
void FrItemAttachCardDlg::SetListCardUsableMyItem(void) {
	m_myItemList.clear();

	sItemAttachCard item;
	item.typeId = 0;
	item.id = 0;

	for (int partType = 0; partType < 10; partType++) {
		std::list<sItemInfo>::iterator it;

		std::list<sItemInfo>& partsList = CSharedDoc::Instance()->m_myPartsList;
		for (it = partsList.begin(); it != partsList.end(); ++it) {
			if (it->m_stateBits.kind == 6 && it->m_stateBits.expired) {
				continue;
			}

			if (partType != ((it->m_typeId >> 18) & 0xFF)) {
				continue;
			}

			IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(it->m_typeId);

			if (!pPart) {
				continue;
			}

			if (pPart->m_caddieCardSlots > 0 || pPart->m_charCardSlots > 0) {
				item.typeId = it->m_typeId;
				item.id = it->m_serial;

				m_myItemList.push_back(item);
			}
		}

		std::list<sItemInfo>& itemList = CSharedDoc::Instance()->m_myItemList;
		for (it = itemList.begin(); it != itemList.end(); ++it) {
			if (partType != ((it->m_typeId >> 18) & 0xFF)) {
				continue;
			}

			IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(it->m_typeId);

			if (!pPart) {
				continue;
			}

			if ((it->m_uccStatus & 1) && (!UccManager()->FindClothes(it->m_typeId, it->m_uccKey) || UccManager()->IsDownloadComplete(it->m_typeId, it->m_uccKey) || NetResourceManager::Instance()->IsDownloadRequested(RESOURCE_TYPE_UCC_CLOTHES, it->m_typeId, it->m_uccKey))) {
				if (it->m_uccStatus & 1) {
					item.typeId = it->m_typeId;
					item.id = it->m_serial;

					m_myItemList.push_back(item);
				}
			}
		}
	}

	SetListEquipParts();

	std::list<sItemAttachCard>::iterator itMy = m_myItemList.begin();
	while (itMy != m_myItemList.end()) {
		if (IsEquipParts(itMy->typeId, itMy->id)) {
			itMy = m_myItemList.erase(itMy);
		} else {
			++itMy;
		}
	}
}

// wtemplate.h (not yet in src/client/ProjectG/wtemplate.h): list finder by the element's guid.
struct _FindByGuid {
	unsigned long m_guid;

	_FindByGuid(unsigned long guid) : m_guid(guid) {}
	template <class T>
	bool operator()(const T& item) const { return m_guid == item.m_serial; }
};

// carddlg.cpp:2053
void FrItemAttachCardDlg::DrawItem(WPoint pt, unsigned long typeId, unsigned long id) {
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (!pGI)
		return;
	if (!typeId)
		return;

	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);
	if (!pCommon)
		return;

	float offX = 0.0f;
	float offY = 0.0f;
	const Bitmap* pIcon = g_pFresh->GetBitmap(pCommon->m_icon);
	if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(typeId))
		pIcon = GetUccItemIcon(id);

	unsigned long kind = typeId >> 26;
	if (kind == 2) {
		offX = -12.0f;
		offY = -20.0f;
	}

	if (pIcon) {
		WRect rc(pt.x + offX + 14.0f, pt.y + offY + 11.0f, (float)pIcon->Width(), (float)pIcon->Height());

		pGI->DrawTexture(pIcon, rc, 0xFFFFFFFF, 0);
		pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGI->SetTextStyle(4);
	}

	if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(typeId)) {
		sItemInfo* pInfo = GetMyItemInfo(id);

		if (pInfo) {
			WRect rc(pt.x + 47.0f, pt.y + 43.0f, (float)m_pUccStateBmp[0]->Width(), (float)m_pUccStateBmp[0]->Height());

			if ((pInfo->m_uccStatus & 1) && pInfo->m_uccSeq == 1 && m_pUccStateBmp[0]) {
				pGI->DrawTexture(m_pUccStateBmp[0], rc, 0xFFFFFFFF, 0);
			} else if ((pInfo->m_uccStatus & 1) && pInfo->m_uccSeq > 1 && m_pUccStateBmp[1]) {
				pGI->DrawTexture(m_pUccStateBmp[1], rc, 0xFFFFFFFF, 0);
			} else if ((pInfo->m_uccStatus & 2) && m_pUccStateBmp[2]) {
				pGI->DrawTexture(m_pUccStateBmp[2], rc, 0xFFFFFFFF, 0);
			}
		}
	}

	switch (kind) {
	case 2: {
		const Bitmap* pLevel = g_pFresh->GetBitmap(MakeStr("level_%03d", (pCommon->m_level & 0x7F) + 1));
		if (pLevel) {
			WRect rc(pt.x + 38.0f - float2int(pLevel->Width() * 0.5f), pt.y + 35.0f - float2int(pLevel->Height() * 0.5f), (float)pLevel->Width(), (float)pLevel->Height());

			pGI->DrawTexture(pLevel, rc, 0xFFFFFFFF, 0);
		}

		std::list<sItemInfo>::iterator it = std::find_if(CSharedDoc::Instance()->m_myPartsList.begin(), CSharedDoc::Instance()->m_myPartsList.end(), _FindByGuid(id));
		if (it != CSharedDoc::Instance()->m_myPartsList.end()) {
			if (it->m_stateBits.kind == 6) {
				const Bitmap* pClock = g_pFresh->GetBitmap("clock");
				pGI->DrawTexture(pClock, WRect(pt.x + 60.0f, pt.y - 15.0f, pClock->Width(), pClock->Height()), 0xFFFFFFFF, 0);
			}
		}
		break;
	}
	}
}

// carddlg.cpp:2151
void FrItemAttachCardDlg::SetCardInfo(unsigned long cardTypeId) {
	if (cardTypeId == 0) {
		return;
	}

	IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(cardTypeId);
	if (pCard == 0 || (cardTypeId & 0xFC000000) != 0x7C000000) {
		return;
	}

	if (m_pAttachCardName != 0) {
		m_pAttachCardName->AddText(MakeStr("\\c0xff000000\\c%s", pCard->m_name), false, true);
	}

	if (m_pAttachCardAbility != 0) {
		for (int i = 0; i < 5; i++) {
			if (pCard->m_stat[i] > 0) {
				char text[512];
				memset(text, 0, sizeof(text));

				if (i == 0) {
					sprintf(text, "%s + %d", "\xC6\xC4\xBF\xF6\xBD\xBD\xB7\xD4", pCard->m_stat[0]);
				} else if (i == 1) {
					sprintf(text, "%s + %d", "\xC4\xC1\xC6\xAE\xB7\xD1\xBD\xBD\xB7\xD4", pCard->m_stat[1]);
				} else if (i == 2) {
					sprintf(text, "%s + %d", "\xC1\xA4\xC8\xAE\xB5\xB5\xBD\xBD\xB7\xD4", pCard->m_stat[2]);
				} else if (i == 3) {
					sprintf(text, "%s + %d", "\xBD\xBA\xC7\xC9\xBD\xBD\xB7\xD4", pCard->m_stat[3]);
				} else if (i == 4) {
					sprintf(text, "%s + %d", "\xC4\xBF\xBA\xEA\xBD\xBD\xB7\xD4", pCard->m_stat[4]);
				}

				m_pAttachCardAbility->AddText(MakeStr("\\c0xff0000ff\\c%s", text), false, true);
			}
		}
	}
}

// carddlg.cpp:2198
void FrItemAttachCardDlg::SetListEquipParts(void) {
	m_equipParts.clear();

	for (std::map<unsigned int, sCharacterInfo>::iterator it = CSharedDoc::Instance()->m_characterMap.begin(); it != CSharedDoc::Instance()->m_characterMap.end(); ++it) {
		for (int i = 0; i < 24; i++) {
			if (it->second.partTypeIds[i] != 0) {
				sItemAttachCard part;
				part.typeId = it->second.partTypeIds[i];
				part.id = it->second.partIds[i];
				m_equipParts.push_back(part);
			}
		}
	}
}

// carddlg.cpp:2221
bool FrItemAttachCardDlg::IsEquipParts(unsigned long typeId, unsigned long id) {
	bool bFound = false;

	std::list<sItemAttachCard>::iterator it;
	for (it = m_equipParts.begin(); it != m_equipParts.end(); ++it) {

		if (typeId == it->typeId && id == it->id) {

			m_equipParts.erase(it);
			bFound = true;
			break;
		}
	}

	return bFound;
}

// carddlg.cpp:2241
void FrItemAttachCardDlg::SetSelectMyItem(sItemAttachCard* pItem) {
	m_pAttachItemName->ClearLine();
	m_pAttachItemAbility->ClearLine();
	m_pSelectedItem = pItem;

	if (pItem == 0) {
		return;
	}

	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(pItem->typeId);
	if (pCommon == 0) {
		return;
	}

	if (m_pAttachItemName == 0) {
		return;
	}

	m_itemTypeId = pItem->typeId;
	m_itemId = pItem->id;

	if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(pCommon->m_typeId)) {
		m_pAttachItemName->AddText(MakeStr("\\c0xff000000\\c%s", GetUccItemName(pItem->id)), false, true);
	} else {
		m_pAttachItemName->AddText(MakeStr("\\c0xff000000\\c%s", pCommon->m_name), false, true);
	}

	switch (pItem->typeId >> 26) {
	case 2: {
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pItem->typeId);
		if (pPart != 0) {
			short baseStat[5];
			memcpy(baseStat, pPart->m_baseStat, sizeof(baseStat));
			short slotStat[5];
			memcpy(slotStat, pPart->m_slotStat, sizeof(slotStat));

			const char* statName = "";
			for (int i = 0; i < 5; i++) {
				if (baseStat[i] != 0 || slotStat[i] != 0) {
					switch (i) {
					case 0: statName = "\xC6\xC4\xBF\xF6"; break;
					case 1: statName = "\xC4\xC1\xC6\xAE\xB7\xD1"; break;
					case 2: statName = "\xC1\xA4\xC8\xAE\xB5\xB5"; break;
					case 3: statName = "\xBD\xBA\xC7\xC9"; break;
					case 4: statName = "\xC4\xBF\xBA\xEA"; break;
					}

					if (baseStat[i] > 0 || slotStat[i] > 0) {
						if (baseStat[i] == 0) {
							m_pAttachItemAbility->AddText(MakeStr("\\c0xff0000ff\\c%s\xBD\xBD\xB7\xD4%+d\\c0xff000000\\c", statName, slotStat[i]), false, true);
						} else if (slotStat[i] == 0) {
							m_pAttachItemAbility->AddText(MakeStr("\\c0xff0000ff\\c%s%+d\\c0xff000000\\c", statName, baseStat[i]), false, true);
						} else {
							m_pAttachItemAbility->AddText(MakeStr("\\c0xff0000ff\\c%s%+d/\xBD\xBD\xB7\xD4%+d\\c0xff000000\\c", statName, baseStat[i], slotStat[i]), false, true);
						}
					}
				}
			}
		}
		break;
	}
	}

	if ((pItem->typeId & 0xFC000000) == 0x08000000) {
		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(((pItem->typeId >> 18) & 0xFF) | 0x04000000);
		if (pChar != 0) {
			m_pAreaCharImg->SetBgImg(MakeStr("s_%s", pChar->m_icon));
		}
	}

	SetPartsSlot(pItem->typeId);
	SetPartsSlotBtnTxt();
}

// carddlg.cpp:2346
void FrItemAttachCardDlg::SetPartsSlot(unsigned long typeId) {
	IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(typeId);

	int i;
	for (i = 0; i < 4; i++) {

		m_pCharSlotBtn[i]->SetButtonImg("cd_slot_d", FrButton::NORMAL);
		m_pCharSlotBtn[i]->Enable(false);
		m_pCaddieSlotBtn[i]->SetButtonImg("cd_slot_d", FrButton::NORMAL);
		m_pCaddieSlotBtn[i]->Enable(false);
	}

	for (i = 0; i < pPart->m_charCardSlots; i++) {

		m_pCharSlotBtn[i]->SetButtonImg("cd_slot_n", FrButton::NORMAL);
		m_pCharSlotBtn[i]->Enable(true);
	}

	for (i = 0; i < pPart->m_caddieCardSlots; i++) {

		m_pCaddieSlotBtn[i]->SetButtonImg("cd_slot_n", FrButton::NORMAL);
		m_pCaddieSlotBtn[i]->Enable(true);
	}
}

// carddlg.cpp:2374
void FrItemAttachCardDlg::SetPartsSlotBtnTxt(void) {
	char buffer[8];   // unused: only its /GS cookie remains

	initCardSlotBtnTxt();

	for (std::list<sSCardAvilityPeriodInfo>::iterator it = CSharedDoc::Instance()->m_cardPeriodList[0].begin(); it != CSharedDoc::Instance()->m_cardPeriodList[0].end(); ++it) {
		if (it->partsId != m_itemId || it->partsTypeId != m_itemTypeId) {
			continue;
		}

		int slot = it->slot;
		IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(it->typeId);
		if (!pCard) {
			continue;
		}

		if (it->kind == 0) {
			for (int i = 0; i < 5; i++) {
				if (pCard->m_stat[i] > 0) {
					switch (slot) {
					case 1:
						m_charSlotAttached[0] = true;
						m_charSlotCard[0] = it->typeId;
						SetCharacterAttatchBtnTxt(it->typeId, m_charSlotBtnTxt[0]);
						break;
					case 2:
						m_charSlotAttached[1] = true;
						m_charSlotCard[1] = it->typeId;
						SetCharacterAttatchBtnTxt(it->typeId, m_charSlotBtnTxt[1]);
						break;
					case 3:
						m_charSlotAttached[2] = true;
						m_charSlotCard[2] = it->typeId;
						SetCharacterAttatchBtnTxt(it->typeId, m_charSlotBtnTxt[2]);
						break;
					case 4:
						m_charSlotAttached[3] = true;
						m_charSlotCard[3] = it->typeId;
						SetCharacterAttatchBtnTxt(it->typeId, m_charSlotBtnTxt[3]);
						break;
					}
				}
			}
		} else if (it->kind == 1) {
			switch (slot) {
			case 1:
				m_caddieSlotAttached[0] = true;
				m_caddieSlotCard[0] = it->typeId;
				SetCaddieAttatchBtnTxt(it->typeId, m_caddieSlotBtnTxt[0]);
				break;
			case 2:
				m_caddieSlotAttached[1] = true;
				m_caddieSlotCard[1] = it->typeId;
				SetCaddieAttatchBtnTxt(it->typeId, m_caddieSlotBtnTxt[1]);
				break;
			case 3:
				m_caddieSlotAttached[2] = true;
				m_caddieSlotCard[2] = it->typeId;
				SetCaddieAttatchBtnTxt(it->typeId, m_caddieSlotBtnTxt[2]);
				break;
			case 4:
				m_caddieSlotAttached[3] = true;
				m_caddieSlotCard[3] = it->typeId;
				SetCaddieAttatchBtnTxt(it->typeId, m_caddieSlotBtnTxt[3]);
				break;
			}
		}
	}
}

// carddlg.cpp:2467
void FrItemAttachCardDlg::DrawPartsSlot(WPoint pt, unsigned long typeId, unsigned long id, FrGraphicInterface* pGraphic) {
	IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(typeId);
	if (!pPart)
		return;

	if (!pGraphic) {
		pGraphic = g_pFresh->GetManager()->GetGDI();
		if (!pGraphic)
			return;
	}

	int i;
	float charX = 5.0f;
	float caddieX = 5.0f;
	for (i = 1; i < pPart->m_charCardSlots + 1; i++) {
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it = CSharedDoc::Instance()->m_cardPeriodList[0].begin(); it != CSharedDoc::Instance()->m_cardPeriodList[0].end(); ++it) {
			if (typeId == it->partsTypeId && id == it->partsId) {
				IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(it->typeId);
				if (pCard) {
					if (i == it->slot) {
						if (it->kind == 0) {
							found = true;
							const Bitmap* pBitmap = g_pFresh->GetBitmap(pCard->m_texture);
							if (pBitmap) {
								pGraphic->DrawTexture(pBitmap, WRect(pt.x + charX, pt.y + 54.0f, (float)pBitmap->Width(), (float)pBitmap->Height()), 0xFFFFFFFF, 0);
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
				pGraphic->DrawTexture(pBitmap, WRect(pt.x + charX, pt.y + 54.0f, (float)pBitmap->Width(), (float)pBitmap->Height()), 0xFFFFFFFF, 0);
				charX += 16.0f;
			}
		}
	}

	for (i = 1; i < pPart->m_caddieCardSlots + 1; i++) {
		bool found = false;
		for (std::list<sSCardAvilityPeriodInfo>::iterator it = CSharedDoc::Instance()->m_cardPeriodList[0].begin(); it != CSharedDoc::Instance()->m_cardPeriodList[0].end(); ++it) {
			if (typeId == it->partsTypeId && id == it->partsId) {
				IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(it->typeId);
				if (pCard) {
					if (i == it->slot) {
						if (it->kind == 1) {
							found = true;
							const Bitmap* pBitmap = g_pFresh->GetBitmap(MakeStr(pCard->m_texture));
							if (pBitmap) {
								pGraphic->DrawTexture(pBitmap, WRect(pt.x + caddieX, pt.y + 70.0f, (float)pBitmap->Width(), (float)pBitmap->Height()), 0xFFFFFFFF, 0);
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
				pGraphic->DrawTexture(pBitmap, WRect(pt.x + caddieX, pt.y + 70.0f, (float)pBitmap->Width(), (float)pBitmap->Height()), 0xFFFFFFFF, 0);
				caddieX += 16.0f;
			}
		}
	}
}

// carddlg.cpp:2566
void FrItemAttachCardDlg::DrawPartsLine(void) {
	float x = 0.0f;

	for (int i = 0; i < 3; i++) {

		if (m_pLineOverlay) {

			m_pLineOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(x + 307.0f, 155.0f, 1.0f, 75.0f), 0, 0xffffffff, 0.0f, 0);

			if (m_bItemListFolded == true) {
				m_pLineOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(x + 307.0f, 255.0f, 1.0f, 75.0f), 0, 0xffffffff, 0.0f, 0);
			}
		}

		x += 91.0f;
	}
}

// carddlg.cpp:2593
void FrItemAttachCardDlg::AgreeAttaching(void) {
	int nIndex = (m_selectedSlot > 4) ? m_selectedSlot - 5 : m_selectedSlot - 1;

	unsigned long cardTypeId = (m_selectedSlot > 4) ? m_caddieSlotCard[nIndex] : m_charSlotCard[nIndex];

	FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);

	// "The effect of the card you were using (%s) is removed. Use the card anyway?"
	pForm->SetMessage(MakeStr("\xB1\xE2\xC1\xB8\xBF\xA1 \xBB\xE7\xBF\xEB\xC7\xCF\xBD\xC3\xB4\xF8 \xC4\xAB\xB5\xE5(%s)\xC0\xC7 \xC8\xBF\xB0\xFA\xB0\xA1 \xBB\xE8\xC1\xA6\xB5\xCB\xB4\xCF\xB4\xD9. \xC1\xA4\xB8\xBB\xB7\xCE \xC4\xAB\xB5\xE5\xB8\xA6 \xBB\xE7\xBF\xEB\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", ItemManager()->GetItemName(cardTypeId)), false);

	pForm->Open((FRESH_RESULT_CALLBACK)&FrItemAttachCardDlg::OnCardAttachResult, 1);
}

// carddlg.cpp:2607
bool FrItemAttachCardDlg::OnAgreeAttachingResult(int ret, FrForm* pForm) {
	if (ret == FrOK) {
		int nIndex = (m_selectedSlot > 4) ? m_selectedSlot - 5 : m_selectedSlot - 1;

		bool bAttached = (m_selectedSlot > 4) ? m_caddieSlotAttached[nIndex] : m_charSlotAttached[nIndex];

		if (bAttached) {
			AgreeAttaching();
		} else {
			ResetCardSlotInfo();
		}
	} else if (ret == FrCANCEL) {

		SetPartsSlotBtnTxt();
		m_selectedSlot = 0xff;
	}

	m_pCardAttachDlg = 0;
	return true;
}

// carddlg.cpp:2628
void FrItemAttachCardDlg::ResetCardSlotInfo(void) {

	SetPartsSlotBtnTxt();

	switch (m_selectedSlot) {
	case 1:
		m_charSlotAttached[0] = true;
		m_charSlotCard[0] = m_cardTypeId;
		SetCharacterAttatchBtnTxt(m_cardTypeId, m_charSlotBtnTxt[0]);
		break;

	case 2:
		m_charSlotAttached[1] = true;
		m_charSlotCard[1] = m_cardTypeId;
		SetCharacterAttatchBtnTxt(m_cardTypeId, m_charSlotBtnTxt[1]);
		break;

	case 3:
		m_charSlotAttached[2] = true;
		m_charSlotCard[2] = m_cardTypeId;
		SetCharacterAttatchBtnTxt(m_cardTypeId, m_charSlotBtnTxt[2]);
		break;

	case 4:
		m_charSlotAttached[3] = true;
		m_charSlotCard[3] = m_cardTypeId;
		SetCharacterAttatchBtnTxt(m_cardTypeId, m_charSlotBtnTxt[3]);
		break;

	case 5:
		m_caddieSlotAttached[0] = true;
		m_caddieSlotCard[0] = m_cardTypeId;
		SetCaddieAttatchBtnTxt(m_cardTypeId, m_caddieSlotBtnTxt[0]);
		break;

	case 6:
		m_caddieSlotAttached[1] = true;
		m_caddieSlotCard[1] = m_cardTypeId;
		SetCaddieAttatchBtnTxt(m_cardTypeId, m_caddieSlotBtnTxt[1]);
		break;

	case 7:
		m_caddieSlotAttached[2] = true;
		m_caddieSlotCard[2] = m_cardTypeId;
		SetCaddieAttatchBtnTxt(m_cardTypeId, m_caddieSlotBtnTxt[2]);
		break;

	case 8:
		m_caddieSlotAttached[3] = true;
		m_caddieSlotCard[3] = m_cardTypeId;
		SetCaddieAttatchBtnTxt(m_cardTypeId, m_caddieSlotBtnTxt[3]);
		break;
	}
}

// carddlg.cpp:2694 (run-time type and factory registration of FrSPCardBuffDlg)
const WRTTI FrSPCardBuffDlg::m_RTTI("FrSPCardBuffDlg", &FrForm::m_RTTI);

IObject* FrSPCardBuffDlgMakeInstance(void) {
	return new FrSPCardBuffDlg;
}

struct __sFrSPCardBuffDlg {
	__sFrSPCardBuffDlg(void) {
		ObjectFactory().AddObjectFunctor(FrSPCardBuffDlgMakeInstance, "FrSPCardBuffDlg");
	}
};
static __sFrSPCardBuffDlg s_registerFrSPCardBuffDlg;

// carddlg.cpp:2696
const sFRESH_MSGMAP* FrSPCardBuffDlg::GetMessageMap(void) const {
	return &FrSPCardBuffDlg::_MsgMap;
}

sFRESH_MSGMAP FrSPCardBuffDlg::_MsgMap = { &FrForm::_MsgMap, &FrSPCardBuffDlg::_MsgEntries[0] };

sFRESH_ENTRY FrSPCardBuffDlg::_MsgEntries[] = {
	// carddlg.cpp:2698
	{ "spcardlist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSPCardBuffDlg::OnSPCardListInit },
	// carddlg.cpp:2699
	{ "spcardlist", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSPCardBuffDlg::OnSPCardListOwnerDraw },

	// carddlg.cpp:2701
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// carddlg.cpp:2707
FrSPCardBuffDlg::FrSPCardBuffDlg(void) {
	m_pSPCardList = 0;

	if (m_Pos.x < 0.0f) {

		m_Pos = WPoint(g_view->GetWidth() - 155.0f, 48.0f);
	}
}

// carddlg.cpp:2718
FrSPCardBuffDlg::~FrSPCardBuffDlg(void) {
}

// carddlg.cpp:2722
bool FrSPCardBuffDlg::OnInit(void) {
	FrForm::OnInit();

	// Registers every card's bitmap up front.
	for (std::map<unsigned int, IFF_STRUCT::sCard>::const_iterator it = ItemManager()->m_cardMap.begin(); it != ItemManager()->m_cardMap.end(); ++it) {

		g_pFresh->RegisterBitmap(it->second.m_bitmap);
	}

	return true;
}

// carddlg.cpp:2736
bool FrSPCardBuffDlg::Close(eFormRet ret, bool bDestroy) {
	if (g_view) {

		WPoint pos;
		pos = GetRect().TopLeft();
		float rx = pos.x / g_view->GetWidth();

		SetPosition(WPoint(rx * 800.0f, pos.y / g_view->GetHeight() * 600.0f));
	}

	return FrForm::Close(ret, bDestroy);
}

// carddlg.cpp:2751
void FrSPCardBuffDlg::OnProc(float fDelta) {
}

// carddlg.cpp:2756
void FrSPCardBuffDlg::SetBuffItem(const std::list<sSCardAvilityPeriodInfo>& buffList, bool clear) {
	if (!m_pSPCardList) {
		return;
	}

	if (clear) {
		m_pSPCardList->ClearItem();
	}

	for (std::list<sSCardAvilityPeriodInfo>::const_iterator it = buffList.begin(); it != buffList.end(); ++it) {
		const sSCardAvilityPeriodInfo* pInfo = &*it;
		bool bSPCard = (pInfo->typeId & 0xFC000000) == 0x7C000000 && pInfo->kind == 2;

		if (bSPCard || ItemManager()->IsItemBuff(pInfo->typeId)) {
			m_pSPCardList->AddItem((void*)pInfo);
		}
	}

	int rows = (GetSpecialCardNum() - 1) / 4;
	if (rows > 0) {
		WRect rect;
		m_pSPCardList->GetClientRect(rect);
		int height = m_pSPCardList->GetItemHeight() * rows;
		rect.h += height;
		m_pSPCardList->SetClientRect(rect);

		rect = GetRect();
		height = m_pSPCardList->GetItemHeight() * rows;
		rect.h += height;
		SetRect(rect);
	}
}

// carddlg.cpp:2801
int FrSPCardBuffDlg::GetSpecialCardNum(void) {
	if (!m_pSPCardList) {
		return 0;
	}

	return m_pSPCardList->GetCurrentItemSize(false);
}

// carddlg.cpp:2811
void FrSPCardBuffDlg::OnSPCardListInit(intptr_t param) {
	m_pSPCardList = DYNAMIC_CAST(FrListBox, param);
}

// carddlg.cpp:2821
void FrSPCardBuffDlg::OnSPCardListOwnerDraw(intptr_t nIndex) {
	FrListItem* pItem = (FrListItem*)nIndex;
	if (pItem == 0) {
		return;
	}

	const sSCardAvilityPeriodInfo* pInfo = (const sSCardAvilityPeriodInfo*)pItem->pData;
	if (pInfo == 0) {
		return;
	}

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0) {
		return;
	}

	const char* bitmapName;
	if ((pInfo->typeId & 0xFC000000) == 0x7C000000) {
		IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(pInfo->typeId);
		if (pCard == 0) {
			return;
		}

		bitmapName = pCard->m_bitmap;
	} else {
		IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(pInfo->typeId);
		if (pCommon == 0) {
			return;
		}

		bitmapName = MakeStr("%s_buff", pCommon->m_icon);
	}

	const Bitmap* pBitmap = g_pFresh->GetBitmap(bitmapName);
	if (pBitmap == 0) {
		return;
	}

	unsigned long textColor = 0xFFFF4949;
	unsigned long diffuse = 0xFF494949;
	char text[32] = "-:-";
	int remainMin = GetRemainedSec(pInfo) / 60;

	int hour = remainMin / 60;
	int min = remainMin % 60;

	if (hour >= 1 || min >= 1) {
		if (hour < 0) {
			hour = 0;
		}
		if (min < 0) {
			min = 0;
		}

		sprintf(text, "%.2d:%.2d", hour, min);

		textColor = 0xFF494949;
		diffuse = 0xFFFFFFFF;
	}

	pGDI->DrawTexture(pBitmap, WRect(pItem->pos, (float)pBitmap->Width(), (float)pBitmap->Height()), diffuse, 0);

	unsigned long oldColor = pGDI->GetTextColor();
	pGDI->SetTextColor(textColor, 0xFFFFFFFF);
	pGDI->SetTextStyle(2);

	pGDI->Print11(pItem->pos + WPoint((float)pBitmap->Width(), (float)pBitmap->Height()), 2, text);

	pGDI->SetTextColor(oldColor, 0xFFFFFFFF);
	pGDI->SetTextStyle(0);
}

// carddlg.cpp:2917
int GetTotalDay_S(_SYSTEMTIME time) {
	int monthDays[12] = { 31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	int total = 0;

	if (time.wYear < 2 || time.wMonth < 1 || time.wMonth > 12 || time.wDay > 31) {
		return 0;
	}

	for (int year = 1; year < time.wYear; year++) {
		if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
			total += 366;
		} else {
			total += 365;
		}
	}

	if ((time.wYear % 4 == 0 && time.wYear % 100 != 0) || time.wYear % 400 == 0) {
		monthDays[1] = 29;
	} else {
		monthDays[1] = 28;
	}

	for (int month = 0; month < time.wMonth - 1; month++) {
		total += monthDays[month];
	}

	if (monthDays[time.wMonth - 1] < time.wDay) {
		time.wDay = monthDays[time.wMonth - 1];
	}

	return total + time.wDay;
}

// carddlg.cpp:2945
int FrSPCardBuffDlg::GetRemainedSec(const sSCardAvilityPeriodInfo* pInfo) {
	int remain;
	unsigned long elapsed;

	bool bSPCard = (pInfo->typeId & 0xFC000000) == 0x7C000000;

	if (!bSPCard) {
		remain = MAKELONG(pInfo->endTime.wMilliseconds, pInfo->endTime.wSecond);
		elapsed = g_CurrentTime - MAKELONG(pInfo->startTime.wMilliseconds, pInfo->startTime.wSecond);
	} else {
		_SYSTEMTIME serverTime = WSingleton<CSharedDoc>::Instance()->GetServerTime();

		int hour = pInfo->endTime.wHour;
		int minute = pInfo->endTime.wMinute;
		int second = pInfo->endTime.wSecond;
		int day = pInfo->endTime.wDay;

		if (second < serverTime.wSecond) {
			minute -= 1;
			second += 60;
		}
		second -= serverTime.wSecond;

		if (minute < serverTime.wMinute) {
			hour -= 1;
			minute += 60;
		}
		minute -= serverTime.wMinute;

		if (hour < serverTime.wHour) {
			day -= 1;
			hour += 24;
		}
		hour -= serverTime.wHour;

		if (day != serverTime.wDay) {
			_SYSTEMTIME endTime = pInfo->endTime;

			if (day < 1) {
				endTime.wMonth = endTime.wMonth - 1;
				if (endTime.wMonth < 1) {
					endTime.wMonth = 12;
				}
			}

			int days;
			if (endTime.wMonth != serverTime.wMonth) {
				days = GetTotalDay_S(endTime) - serverTime.wDay + GetTotalDay_S(serverTime);
			} else {
				days = day - serverTime.wDay;
			}

			hour += days * 24;
		}

		remain = (hour * 60 + minute) * 60 + second;
		elapsed = g_CurrentTime - m_SpecialBuffStartTick;
	}

	return remain - elapsed / 1000;
}

// The special buff dialog's position (x < 0: not placed yet, GetPosition
// places it), and when the special buffs were last reset. Defined after the
// unit's run-time initializers: WPoint's constructor makes m_Pos a dynamic
// initializer, whose `_$E` number would shift theirs.
WPoint FrSPCardBuffDlg::m_Pos(-1.0f, 0.0f);
unsigned long FrSPCardBuffDlg::m_SpecialBuffStartTick;
