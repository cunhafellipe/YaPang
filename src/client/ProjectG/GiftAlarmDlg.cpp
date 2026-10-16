// Original: d:\build\custom\temp\code\client\projectg\giftalarmdlg.cpp
#include <windows.h>
#include <string.h>
#include "GiftAlarmDlg.h"
#include "ObjectFactory.h"
#include "frlistbox.h"
#include "frstatic.h"
#include "commonutil.h"
#include "fresh.h"
#include "SharedDoc.h"
#include "UccManager.h"
#include "PangGiftDlg.h"
#include "bitmap.h"
#include "frgraphicinterface.h"
#include "frwndmanager.h"
#include "Treasure.h"

// Item data of a gift-list row: the received gift and the sender message
// split over two lines (new char[0x50] each).
struct sGiftAlarmItem {
	void* pGift;       // 0x0, element of the shared doc's gift list
	char* pLines[2];   // 0x4
};
CHECK_SIZE(sGiftAlarmItem, 0xC);

// Item data of a treasure-gift row (new 0xC8).
struct sTreasureAlarmItem {
	unsigned long typeId;    // 0x0
	unsigned short count;    // 0x4
	char name[0x40];         // 0x6
	char desc[0x80];         // 0x46
};
CHECK_SIZE(sTreasureAlarmItem, 0xC8);

// Blink state of a dialog's caption (OnProc): set when the caption must be
// rewritten, and whether it is shown.
struct sCaptionBlink {
	bool update;   // 0x0
	bool show;     // 0x1
};

// The OnProc blink states (no symbol): FrGiftAlarmDlg's, CTreasureAlarmDlg's.
extern "C" {
sCaptionBlink data_af7531;
sCaptionBlink data_af7533;
}

// giftalarmdlg.cpp:19 (run-time type and factory registration of FrGiftAlarmDlg)
const WRTTI FrGiftAlarmDlg::m_RTTI("FrGiftAlarmDlg", &FrForm::m_RTTI);

IObject* FrGiftAlarmDlgMakeInstance(void) {
	return new FrGiftAlarmDlg;
}

struct __sFrGiftAlarmDlg {
	__sFrGiftAlarmDlg(void) {
		ObjectFactory().AddObjectFunctor(FrGiftAlarmDlgMakeInstance, "FrGiftAlarmDlg");
	}
};
static __sFrGiftAlarmDlg s_registerFrGiftAlarmDlg;

// giftalarmdlg.cpp:21
const sFRESH_MSGMAP* FrGiftAlarmDlg::GetMessageMap(void) const {
	return &FrGiftAlarmDlg::_MsgMap;
}

sFRESH_MSGMAP FrGiftAlarmDlg::_MsgMap = { &FrForm::_MsgMap, &FrGiftAlarmDlg::_MsgEntries[0] };

sFRESH_ENTRY FrGiftAlarmDlg::_MsgEntries[] = {
	// giftalarmdlg.cpp:23
	{ "giftlist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftAlarmDlg::OnGiftListInit },
	{ "giftlist", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftAlarmDlg::OnGiftListOwnerDraw },
	{ "infomsg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftAlarmDlg::OnInfoMsgInit },

	// giftalarmdlg.cpp:27
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// giftalarmdlg.cpp:32
FrGiftAlarmDlg::FrGiftAlarmDlg(void) {
	m_pGiftList = 0;
	m_curGift = 0;
	m_giftCount = 0;
	m_blinkTime = 0.0f;

	WSingleton<CSharedDoc>::Instance()->m_tutorialFinish = 0;
	m_pArrivalMailImage = g_pFresh->RegisterBitmap("rmr_Arrival_mail");
	m_pOpenMailImage = g_pFresh->RegisterBitmap("rmr_Open_mail");
}

// giftalarmdlg.cpp:44
FrGiftAlarmDlg::~FrGiftAlarmDlg(void) {
	for (FrListBox::ITEM_LIST::iterator it = m_pGiftList->m_itemList.begin();
			it != m_pGiftList->m_itemList.end(); ++it) {
		FrListItem* pItem = *it;
		sGiftAlarmItem* pData = (sGiftAlarmItem*)pItem->pData;

		if (pData) {

			delete[] pData->pLines[0];
			delete[] pData->pLines[1];
		}

		delete pData;
	}
}

// giftalarmdlg.cpp:64
void FrGiftAlarmDlg::OnInfoMsgInit(intptr_t param) {
	m_pInfoMsg = DYNAMIC_CAST(FrStatic, param);


}

// giftalarmdlg.cpp:72
void FrGiftAlarmDlg::MakeSenderMsg(const char* pSender, char* pLines[]) {
	int len = (int)strlen(pSender);
	if (len > 20) {

		int dbcsCount = 0, sbcsCount = 0;
		for (int i = len - 1; i >= 20; i--) {

			if (IsDBCSLeadByte(pSender[i])) {
				dbcsCount++;
			} else {
				sbcsCount++;
			}
		}

		if (dbcsCount % 2) {
			dbcsCount++;
		}
		int cut = len - sbcsCount - dbcsCount;

		memcpy(pLines[0], pSender, cut);
		pLines[0][cut] = 0;


		for (int j = 0; j < 20; j++) {

			if (pSender[cut] != ' ') {
				break;
			}
			cut++;
		}


		strcpy(pLines[1], pSender + cut);
	} else {


		strcpy(pLines[0], pSender);
		pLines[1][0] = 0;
	}
}

// giftalarmdlg.cpp:112
void FrGiftAlarmDlg::OnGiftListInit(intptr_t param) {

	char message[128] = "\0";
	m_pGiftList = DYNAMIC_CAST(FrListBox, param);
	if (m_pGiftList) {


		if (IsLocalContent((localContentType_t)0x60)) {


			for (std::list<sMailInfoBrief>::iterator it = WSingleton<CSharedDoc>::Instance()->m_mailList.begin();
					it != WSingleton<CSharedDoc>::Instance()->m_mailList.end(); ++it) {



				sGiftAlarmItem* pData = new sGiftAlarmItem;
				pData->pGift = &*it;


				pData->pLines[0] = new char[0x50];
				pData->pLines[1] = new char[0x50];














				MakeSenderMsg(message, pData->pLines);

				m_pGiftList->AddItem(pData);
			}
			m_giftCount = (int)WSingleton<CSharedDoc>::Instance()->m_mailList.size();
		} else {



			for (std::list<sGiftInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_giftList2.begin();
					it != WSingleton<CSharedDoc>::Instance()->m_giftList2.end(); ++it) {



				sGiftAlarmItem* pData = new sGiftAlarmItem;
				sGiftInfo& gift = *it;
				pData->pGift = &gift;
				pData->pLines[0] = new char[0x50];
				pData->pLines[1] = new char[0x50];

				MakeSenderMsg(gift.m_message, pData->pLines);

				m_pGiftList->AddItem(pData);
			}
			m_giftCount = (int)WSingleton<CSharedDoc>::Instance()->m_giftList2.size();
		}
	
	}
}

// giftalarmdlg.cpp:174
void FrGiftAlarmDlg::OnGiftListOwnerDraw(int param) {
	FrListItem* pItem = (FrListItem*)param;

	if (pItem) {


		FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
		if (pGDI) {


			char sender[128] = "\0";
			char line1[80] = "\0";
			char line2[80] = "\0";
			int timeUnit;
			int count;
			unsigned long typeId;

			if (IsLocalContent((localContentType_t)0x60)) {

				sGiftAlarmItem* pData = (sGiftAlarmItem*)pItem->pData;
				if (!pData) {
					return;
				}

				sMailInfoBrief* pMail = (sMailInfoBrief*)pData->pGift;
				wsprintfA(sender, "%s", pMail->m_sender);
				wsprintfA(line1, "%s", pData->pLines[0]);
				wsprintfA(line2, "%s", pData->pLines[1]);

				typeId = pMail->m_item.typeId;
				if ((int)pMail->m_item.flag > 0) {

					timeUnit = 2;
					count = pMail->m_item.flag;

				} else {


					timeUnit = 0;
					count = pMail->m_item.count;
				}
			} else {


				sGiftAlarmItem* pData = (sGiftAlarmItem*)pItem->pData;
				if (!pData) {
					return;
				}

				sGiftInfo* pGift = (sGiftInfo*)pData->pGift;
				wsprintfA(sender, "%s", pGift->m_sender);
				wsprintfA(line1, "%s", pData->pLines[0]);
				wsprintfA(line2, "%s", pData->pLines[1]);

				typeId = pGift->m_typeId;
				timeUnit = pGift->m_timeUnit;
				count = pGift->m_pang;
			}

			m_curGift = pItem->idx;

			pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
			pGDI->SetTextStyle(0);


			g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 10.0f, pItem->pos.y + 5.0f), 0, sender, -1.0f, 0xFFFFFFFF);


			const Bitmap* pMsgBitmap = g_pFresh->GetManager()->GetBitmap("FRAMES8", "text_message");
			if (pMsgBitmap) {
				pGDI->DrawTexture(pMsgBitmap, WRect(pItem->pos.x + 24.0f, pItem->pos.y + 27.0f, (float)pMsgBitmap->Width(), (float)pMsgBitmap->Height()), 0xFFFFFFFF, 0);
			}

			pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);

			g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 28.0f, pItem->pos.y + 56.0f), 0, line1, -1.0f, 0xFFFFFFFF);
			g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 28.0f, pItem->pos.y + 74.0f), 0, line2, -1.0f, 0xFFFFFFFF);

			char name[80] = "\0";
			const Bitmap* pIcon;
			IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);
			if (!pCommon) {

				pIcon = m_pArrivalMailImage;
				wsprintfA(name, "\xc6\xed\xc1\xf6");

			} else {

				pIcon = g_pFresh->GetBitmap(pCommon->m_icon);
				wsprintfA(name, "%s", pCommon->m_name);
			}

			if (pIcon) {
				float offX, offY;

				switch (typeId >> 26) {
				case 6:

					offX = 0.0f;
					offY = 0.0f;
					break;
				case 5:

					offX = -10.0f;
					offY = -8.0f;
					break;
				case 2:
				case 0x1C:

					offX = -10.0f;
					offY = -27.0f;
					break;
				default:

					offX = -10.0f;
					offY = -27.0f;
					break;
				}

				pGDI->DrawTexture(pIcon, WRect(offX + pItem->pos.x + 36.0f, offY + pItem->pos.y + 121.0f, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);
			}

			pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

			pGDI->SetTextStyle(1);
			pGDI->Print(WPoint(pItem->pos.x + 181.0f, pItem->pos.y + 96.0f), 2, "%s", name);

			pGDI->SetTextStyle(0);

			char* pText = 0;
			if (timeUnit < 3 || timeUnit > 5) {

				if ((typeId >> 26) == 0x1C) {

					if (count > 0) {
						pText = MakeStr("%d\xb0\xb3", count);
					} else {

						IFF_STRUCT::sAuxPart* pAux = ItemManager()->FindAuxPart(typeId);
						if (pAux && pAux->quantity > 0) {
							pText = MakeStr("%d\xb0\xb3", pAux->quantity);
						}
					}
				} else if ((typeId >> 26) == 5) {

					if (count > 0) {

						pText = MakeStr("%d\xb0\xb3", count);
					} else {

						IFF_STRUCT::sBall* pBall = ItemManager()->FindBall(typeId);
						if (pBall && pBall->quantity > 0) {
							pText = MakeStr("%d\xb0\xb3", pBall->quantity);
						}
					}
				} else if ((typeId >> 26) == 6) {

					if (typeId == 0x1A000010) {

						pText = MakeStr("%d\xc6\xce", count);
					} else {

						pText = MakeStr("%d\xb0\xb3", count);
					}
				} else if ((typeId >> 26) == 0xE) {
					pText = MakeStr("%d\xc0\xcf", count);
				} else if ((typeId >> 26) == 2) {
					pText = MakeStr("%d\xb0\xb3", 1);
				}
			} else {

				if (timeUnit == 3) {
					pText = MakeStr("%d\xbd\xc3\xb0\xa3", count);
				} else if (timeUnit == 4) {
					pText = MakeStr("%d\xc0\xcf", count);
				} else {
					pText = MakeStr("%d\xb0\xb3\xbf\xf9", count);
				}
			}

			if (pText) {
				pGDI->Print(WPoint(pItem->pos.x + 181.0f, pItem->pos.y + 116.0f), 2, pText);
			}
		}
	}
}

// giftalarmdlg.cpp:446
void FrGiftAlarmDlg::OnProc(const float dt) {
	m_blinkTime += dt;

	if (m_blinkTime > 1.0f) {

		data_af7531.show ^= true;

		m_blinkTime = 0; data_af7531.update = true;
	}

	if (data_af7531.show) {

		if (data_af7531.update) {

			m_pInfoMsg->SetCaption(MakeStr("\xbf\xec\xc6\xed\xc7\xd4\xc0\xbb \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4   (%d/%d)", m_curGift + 1, m_giftCount));

			data_af7531.update = false;
		}
	} else {

		if (data_af7531.update) {

			m_pInfoMsg->SetCaption("");
			data_af7531.update = false;
		}
	}
}

// giftalarmdlg.cpp:485 (run-time type and factory registration of CTreasureAlarmDlg)
const WRTTI CTreasureAlarmDlg::m_RTTI("CTreasureAlarmDlg", &FrForm::m_RTTI);

IObject* CTreasureAlarmDlgMakeInstance(void) {
	return new CTreasureAlarmDlg;
}

struct __sCTreasureAlarmDlg {
	__sCTreasureAlarmDlg(void) {
		ObjectFactory().AddObjectFunctor(CTreasureAlarmDlgMakeInstance, "CTreasureAlarmDlg");
	}
};
static __sCTreasureAlarmDlg s_registerCTreasureAlarmDlg;

// giftalarmdlg.cpp:487
const sFRESH_MSGMAP* CTreasureAlarmDlg::GetMessageMap(void) const {
	return &CTreasureAlarmDlg::_MsgMap;
}

sFRESH_MSGMAP CTreasureAlarmDlg::_MsgMap = { &FrForm::_MsgMap, &CTreasureAlarmDlg::_MsgEntries[0] };

sFRESH_ENTRY CTreasureAlarmDlg::_MsgEntries[] = {
	// giftalarmdlg.cpp:489
	{ "giftlist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTreasureAlarmDlg::OnListBox_Gift_Init },
	{ "giftlist", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTreasureAlarmDlg::OnListBox_Gift_OwnerDraw },
	{ "infomsg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTreasureAlarmDlg::OnStatic_Msg_Init },

	// giftalarmdlg.cpp:493
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// giftalarmdlg.cpp:498
CTreasureAlarmDlg::CTreasureAlarmDlg(void) {
	m_pGiftList = 0;
	m_pMsg = 0;
}

// giftalarmdlg.cpp:504
CTreasureAlarmDlg::~CTreasureAlarmDlg(void) {


	for (FrListBox::ITEM_LIST::iterator it = m_pGiftList->m_itemList.begin();
			it != m_pGiftList->m_itemList.end(); ++it) {
		FrListItem* pItem = *it;


		delete (sTreasureAlarmItem*)pItem->pData;
	}
}

// giftalarmdlg.cpp:518
void CTreasureAlarmDlg::OnListBox_Gift_Init(int param) {
	m_pGiftList = DYNAMIC_CAST(FrListBox, param);
	if (m_pGiftList) {





		CTHunter::Instance()->ReAdjustRepository();
		unsigned char size = CTHunter::Instance()->GetRepositorySize();

		for (unsigned char i = 0; i < size; i++) {

			sTreasureHunt info;
			info = CTHunter::Instance()->GetGiftInfo(i);

			sTreasureAlarmItem* pData = new sTreasureAlarmItem;
			pData->typeId = info.m_typeId;
			pData->count = info.m_count;
			strncpy(pData->name, "\xc6\xae\xb7\xb9\xc1\xae \xbc\xb1\xb9\xb0", 0x40);
			strncpy(pData->desc, "", 0x80);

			m_pGiftList->AddItem(pData);
		}

		m_giftCount = size;
	}
}

// giftalarmdlg.cpp:545
void CTreasureAlarmDlg::OnListBox_Gift_OwnerDraw(int param) {
	FrListItem* pItem = (FrListItem*)param;

	if (pItem) {


		FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
		if (pGDI) {


			sTreasureAlarmItem* pData = (sTreasureAlarmItem*)pItem->pData;
			if (pData) {


				IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(pData->typeId);
				if (pCommon) {


					m_drawItemIdx = pItem->idx;

					pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
					pGDI->SetTextStyle(0);


					g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 10.0f, pItem->pos.y + 5.0f), 0, pData->name, -1.0f, 0xFFFFFFFF);


					const Bitmap* pMsgBitmap = g_pFresh->RegisterBitmap("text_message_treasure");
					if (pMsgBitmap) {
						pGDI->DrawTexture(pMsgBitmap, WRect(pItem->pos.x + 24.0f, pItem->pos.y + 27.0f, (float)pMsgBitmap->Width(), (float)pMsgBitmap->Height()), 0xFFFFFFFF, 0);
					}


					pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
					g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 28.0f, pItem->pos.y + 56.0f), 0, pData->desc, -1.0f, 0xFFFFFFFF);





					const Bitmap* pIcon = g_pFresh->GetBitmap(pCommon->m_icon);
					if (pIcon) {
						float offX, offY;

						switch (pCommon->m_typeId >> 26) {
						case 6:

							offX = 0.0f;
							offY = 0.0f;
							break;
						case 5:

							offX = -10.0f;
							offY = -8.0f;
							break;
						default:


							offX = -10.0f;
							offY = -27.0f;
							break;
						}

						pGDI->DrawTexture(pIcon, WRect(offX + pItem->pos.x + 36.0f, offY + pItem->pos.y + 121.0f, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);
					}



					pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
					pGDI->SetTextStyle(1);
					pGDI->Print(WPoint(pItem->pos.x + 181.0f, pItem->pos.y + 96.0f), 2, "%s", pCommon->m_name);
					pGDI->SetTextStyle(0);



					char* pText = 0;
					unsigned long typeId = pCommon->m_typeId;
					if ((typeId >> 26) == 0x1C) {

						if (pData->count > 0) {
							pText = MakeStr("%d\xb0\xb3", pData->count);
						} else {

							IFF_STRUCT::sAuxPart* pAux = ItemManager()->FindAuxPart(typeId);
							if (pAux && pAux->quantity > 0) {
								pText = MakeStr("%d\xb0\xb3", pAux->quantity);
							}
						}
					} else if ((typeId >> 26) == 5) {

						if (pData->count > 0) {

							pText = MakeStr("%d\xb0\xb3", pData->count);
						} else {

							IFF_STRUCT::sBall* pBall = ItemManager()->FindBall(typeId);
							if (pBall && pBall->quantity > 0) {
								pText = MakeStr("%d\xb0\xb3", pBall->quantity);
							}
						}
					} else if ((typeId >> 26) == 6) {

						if (typeId == 0x1A000010) {

							pText = MakeStr("%d\xc6\xce", pData->count);
						} else {

							pText = MakeStr("%d\xb0\xb3", pData->count);
						}
					} else if ((typeId >> 26) == 0xE) {
						pText = MakeStr("%d\xc0\xcf", pData->count);
					} else {
						pText = MakeStr("%d\xb0\xb3", 1);
					}

					if (pText) {

						pGDI->Print(WPoint(pItem->pos.x + 181.0f, pItem->pos.y + 116.0f), 2, pText);
					}
				}
			}
		}
	}
}

// giftalarmdlg.cpp:676
void CTreasureAlarmDlg::OnStatic_Msg_Init(intptr_t param) {
	m_pMsg = DYNAMIC_CAST(FrStatic, param);


}

// giftalarmdlg.cpp:683
void CTreasureAlarmDlg::OnProc(const float dt) {
	m_blinkTime += dt;

	if (m_blinkTime > 1.0f) {

		data_af7533.show ^= true;
		m_blinkTime = 0;
		data_af7533.update = true;
	}

	if (data_af7533.show) {

		if (data_af7533.update) {

			m_pMsg->SetCaption("\xb8\xb6\xc0\xcc\xb7\xeb\xbf\xa1\xbc\xad \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4");

			data_af7533.update = false;
		}
	} else {

		if (data_af7533.update) {

			m_pMsg->SetCaption("");
			data_af7533.update = false;
		}
	}
}
