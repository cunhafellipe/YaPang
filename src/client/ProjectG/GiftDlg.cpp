// Original: d:\build\custom\temp\code\client\projectg\giftdlg.cpp
#include "NetworkSystem.h"
#include "GiftDlg.h"
#include "ObjectFactory.h"
#include "EmoticonDlg.h"
#include "FriendDlg.h"
#include "Packet.h"
#include "buddymanager.h"
#include "shared/globalgamedefine.h"
#include "commonutil.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frlistbox.h"
#include "bitmap.h"
#include "frbutton.h"
#include "frcombobox.h"
#include "fredit.h"
#include "frstatic.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "binstr.h"
#include "Browser.h"
#include "logininfo.h"
#include "TaskManager.h"
#include "actor.h"
#include "InputManager.h"
#include "SharedDoc.h"
#include "commonutil.h"
#include "frgraphicinterface.h"
#include "wview.h"
#include "shared/s5/SharedUtilities.h"

// The project's SDK has no <wininet.h>; the function is declared locally.
extern "C" __declspec(dllimport) BOOL __stdcall InternetSetCookieA(LPCSTR lpszUrl, LPCSTR lpszCookieName, LPCSTR lpszCookieData);

// The default rental periods (defined with the checkout dialog)
struct days_t {
	const char* m_name;   // 0x0
	int m_days;           // 0x4
};

extern days_t days[];

// One item of a gift request (packet 0x1F, EncodeBuffer of 0x10 bytes; FrGiftDlg::GiftProgress)
struct sGiftBuyItem {
	unsigned long m_serial;      // 0x0, -1
	unsigned long m_typeId;      // 0x4, the item's type id
	unsigned short m_period;     // 0x8, the rental length (days table or sPeriodPrice::altPeriod), 0: none
	unsigned short m_daysIndex;  // 0xA, the selected period, 0xFFFF: none
	int m_count;                 // 0xC, the item count
};
CHECK_SIZE(sGiftBuyItem, 0x10);
CHECK_FIELD(sGiftBuyItem, m_typeId, 0x4);
CHECK_FIELD(sGiftBuyItem, m_period, 0x8);
CHECK_FIELD(sGiftBuyItem, m_daysIndex, 0xA);
CHECK_FIELD(sGiftBuyItem, m_count, 0xC);

// giftdlg.cpp:26 (run-time type and factory registration of FrGiftDlg)
const WRTTI FrGiftDlg::m_RTTI("FrGiftDlg", &FrForm::m_RTTI);

IObject* FrGiftDlgMakeInstance(void) {
	return new FrGiftDlg;
}

struct __sFrGiftDlg {
	__sFrGiftDlg(void) {
		ObjectFactory().AddObjectFunctor(FrGiftDlgMakeInstance, "FrGiftDlg");
	}
};
static __sFrGiftDlg s_registerFrGiftDlg;

// giftdlg.cpp:28
const sFRESH_MSGMAP* FrGiftDlg::GetMessageMap(void) const {
	return &FrGiftDlg::_MsgMap;
}

sFRESH_MSGMAP FrGiftDlg::_MsgMap = { &FrForm::_MsgMap, &FrGiftDlg::_MsgEntries[0] };

// giftdlg.cpp:30
sFRESH_ENTRY FrGiftDlg::_MsgEntries[] = {
	{ "checkout", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnCheckoutInit },
	{ "checkout", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrGiftDlg::OnCheckoutBtnUp },
	{ "friend", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnFriendBtnInit },
	{ "friend", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrGiftDlg::OnFriendBtnUp },
	{ "cart", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnCartInit },
	{ "cart", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnCartOwnerDraw },
	{ "sxid", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnSxIdInit },
	{ "edid", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnEdIdInit },
	{ "edid", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&FrGiftDlg::OnEdIdEnterKey },
	{ "edid", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrGiftDlg::OnEdIdDown },
	{ "id_check", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnBtIdCheckInit },
	{ "id_check", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrGiftDlg::OnBtIdCheckUp },
	{ "sxnick", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnSxNickInit },
	{ "ednick", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnEdNickInit },
	{ "ednick", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&FrGiftDlg::OnEdNickEnterKey },
	{ "ednick", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrGiftDlg::OnEdNickDown },
	{ "emoticon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnEmoticonBtnInit },
	{ "emoticon", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrGiftDlg::OnEmoticonBtnUp },
	{ "nick_check", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnBtNickCheckInit },
	{ "nick_check", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrGiftDlg::OnBtNickCheckUp },
	{ "message", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnMsgInit },
	{ "message", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&FrGiftDlg::OnMsgEnterKey },
	{ "confirm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnEdConfirmInit },
	{ "price", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnPriceInit },
	{ "my_money", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnMyMoneyInit },
	{ "money_left", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnMoneyLeftInit },
	{ "price_cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnPrice_CookieInit },
	{ "my_money_cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnMyMoney_CookieInit },
	{ "money_left_cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnMoneyLeft_CookieInit },
	{ "num_static", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnNumStaticInit },
	{ "num", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnNumInit },
	{ "num_prev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnNumPrevInit },
	{ "num_prev", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrGiftDlg::OnNumPrevBtnUp },
	{ "num_next", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnNumNextInit },
	{ "num_next", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrGiftDlg::OnNumNextBtnUp },
	{ "days", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnDaysInit },
	{ "days", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrGiftDlg::OnDaysBtnDown },
	{ "warn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnWarnInit },
	{ "explane", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGiftDlg::OnExplaneStaticInit },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// giftdlg.h: no line record (empty constructor of a nested helper class)
FrGiftDlg::MiniBtn::MiniBtn(void) {
}

// giftdlg.cpp:77
FrGiftDlg::FrGiftDlg(void) {
	memset(&m_item, 0, sizeof(m_item));
	m_receiverUID = 0xffffffff;
	m_receiverNick = "";
	m_bPayPang = false;
	m_bPayCookie = false;

	m_pFriendBtn = 0;
	m_pCheckoutBtn = 0;
	m_pCart = 0;
	m_pPrice = 0;
	m_pMyMoney = 0;
	m_pMoneyLeft = 0;
	m_pPriceCookie = 0;
	m_pMyMoneyCookie = 0;
	m_pMoneyLeftCookie = 0;
	m_pNumStatic = 0;
	m_pNum = 0;
	m_pNumPrev = 0;
	m_pNumNext = 0;
	m_pFriendListDlg = 0;
	m_pBtNickCheck = 0;
	m_pWarn = 0;

	m_pExplaneStatic = 0;

	m_pSxNick = 0;

	m_pEmoticonBtn = 0;
	m_pEmoticonDlg = 0;


	m_pangAmount = 0;
	m_cookieAmount = 0;


	m_totalCookie = 0;
	m_bTotalCookieSet = false;


	m_periodCount = 0;
	memset(m_periods, 0, sizeof(m_periods));
	m_receiverMode = -1;
}

// giftdlg.cpp:124
void FrGiftDlg::SendGift(const IFF_ITEM_COMMON& item, __int64 pang, __int64 cookie) {
	m_item = item;

	m_pangPrice = 0;
	m_cookiePrice = 0;
	m_bPayPang = false;
	m_bPayCookie = false;

	if (m_item.m_flags0 & 1) {
		if (m_pWarn)
			m_pWarn->SetVisible(false);
	}
	else {
		if (m_pSxId)
			m_pSxId->SetVisible(false);

		if (m_pEdId)
			m_pEdId->SetVisible(false);

		if (m_pBtIdCheck)
			m_pBtIdCheck->SetVisible(false);

		if (m_pSxNick)
			m_pSxNick->SetVisible(false);

		if (m_pEdNick)
			m_pEdNick->SetVisible(false);

		if (m_pBtNickCheck)
			m_pBtNickCheck->SetVisible(false);

		if (m_pMsg)
			m_pMsg->SetKeyFocus(true);

		if (m_pWarn)
			m_pWarn->SetVisible(true);

		if (m_pEmoticonBtn)
			m_pEmoticonBtn->SetVisible(false);
	}

	unsigned long group = m_item.m_typeId >> 26;

	m_daysIndex = 1;
	if (group == 0xE) {
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(item.m_typeId);

		if (pSkin && !S5::ISCutinItem(item.m_typeId)) {
			for (int i = 0; i < 5; i++) {
				if (pSkin->m_periodPrices[i] != 0) {
					m_daysIndex = i;
					break;
				}
			}
		}
	}
	else if (group == 8) {
		IFF_STRUCT::sCadItem* pCadItem = ItemManager()->FindCadItem(item.m_typeId);
		for (int i = 0; i < 4; i++) {
			if (pCadItem->m_levels[i] != 0) {
				m_daysIndex = i;
				break;
			}
		}
	}
	else if (group == 0x10) {
		IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(item.m_typeId);
		for (int i = 0; i < 4; i++) {
			if (pMascot->m_periodPrices[i] != 0) {
				m_daysIndex = i;
				break;
			}
		}
	}
	else {
		m_periodCount = ItemManager()->FindChildItemNumber(item.m_typeId);

		if (m_periodCount > 0) {
			memset(m_periods, 0, sizeof(m_periods));

			if (m_periodCount > 5)
				m_periodCount = 5;

			for (int i = 0; i < m_periodCount; i++) {
				IFF_STRUCT::sChildItem* pChild = ItemManager()->FindChildItem(item.m_typeId, i);

				if (pChild == 0)
					continue;
				if (!pChild->m_enabled)
					continue;

				if (pChild->m_hasPrice) {
					sprintf(m_periods[i].name, "%d\xb0\xb3", pChild->m_quantity);
					m_periods[i].isCash = pChild->isCash;
					m_periods[i].price = pChild->m_price;
					m_periods[i].hasPrice = pChild->m_hasPrice;
					m_periods[i].quantity = pChild->m_quantity;
				}
				else if (pChild->m_altPeriodUnit) {
					switch (pChild->m_altPeriodUnit) {
					case 0:
						break;
					case 3:
						sprintf(m_periods[i].name, "%d\xbd\xc3\xb0\xa3", pChild->m_altPeriod);
						break;
					case 4:
						sprintf(m_periods[i].name, "%d\xc0\xcf", pChild->m_altPeriod);
						break;
					case 5:
						sprintf(m_periods[i].name, "%d\xb0\xb3\xbf\xf9", pChild->m_altPeriod);
						break;
					default:
						sprintf(m_periods[i].name, "%d\xc0\xcf", pChild->m_altPeriod);
						break;
					}

					m_periods[i].isCash = pChild->isCash;
					m_periods[i].altPrice = pChild->m_altPrice;
					m_periods[i].altPeriodUnit = pChild->m_altPeriodUnit;
					m_periods[i].altPeriod = pChild->m_altPeriod;
				}
			}
		}
	}

	m_pangAmount = pang;
	m_cookieAmount = cookie;
	RecalculateTotalCookie(pang, cookie, true);

	bool bQuantity = false;
	if ((m_item.m_typeId & 0xFC000000) == 0x18000000 && !(m_item.m_typeId & 0x2000000)) {
		IFF_STRUCT::sItem* pItem = ItemManager()->FindItem(m_item.m_typeId);

		if (pItem->quantity > 0) {
			bQuantity = true;
			if (m_pNum)
				m_pNum->SetLine(1, MakeStr("%d", pItem->quantity), 0, false, 0);
		}
	}

	if ((m_item.m_typeId & 0x2000000) || (m_item.m_typeId & 0xFC000000) != 0x18000000 || bQuantity) {
		if (m_pNumStatic)
			m_pNumStatic->SetVisible(false);

		if (m_pNum)
			m_pNum->SetVisible(false);

		if (m_pNumPrev)
			m_pNumPrev->SetVisible(false);

		if (m_pNumNext)
			m_pNumNext->SetVisible(false);
	}
	else {
		if (m_pNum) {
			WPoint pt(m_pNum->GetRect().x, m_pNum->GetRect().y);
			pt.x += (float)((m_item.m_flags0 & 1) ? 50 : -50);
			m_pNum->MoveWindow(pt);
		}

		if (m_pNumPrev) {
			WPoint pt(m_pNumPrev->GetRect().x, m_pNumPrev->GetRect().y);
			pt.x += (float)((m_item.m_flags0 & 1) ? 50 : -50);
			m_pNumPrev->MoveWindow(pt);
		}

		if (m_pNumNext) {
			WPoint pt(m_pNumNext->GetRect().x, m_pNumNext->GetRect().y);
			pt.x += (float)((m_item.m_flags0 & 1) ? 50 : -50);
			m_pNumNext->MoveWindow(pt);
		}
	}

	m_bItemSet = true;
	m_pDays->SetVisible(false);
}

// giftdlg.cpp:326
void FrGiftDlg::GiftProgress(void) {
	FrListBox::ITEM_LIST& cart = m_pCart->m_itemList;

	const char* msg = m_pMsg->GetLine(1, false);
	WSingleton<CSharedDoc>::Instance()->m_chatManager.FilteringHack(msg, true);

	const char* numText = m_pNum->GetLine(1, false);
	int num = atoi(numText);
	sGiftBuyItem buy = { 0 };

	buy.m_serial = 0xffffffff;
	if (m_periodCount > 0) {
		if ((m_item.m_typeId & 0xFC000000) == 0x18000000) {
			// packet 0x1F has no enumerator in packet.h yet
			WSendPacket packet((enumClientPacket)0x1F);
			packet.EncodeStr(m_message);
			packet.Encode4(m_receiverUID);
			packet.EncodeStr(std::string(msg));
			packet.Encode1(1);

			if (m_item.m_typeId & 0x2000000) {
				int count;
				if (m_periods[m_daysIndex].hasPrice)
					count = m_periods[m_daysIndex].quantity;
				else
					count = 1;

				buy.m_count = count;
			}
			else {
				buy.m_count = num;
			}

			packet.Encode2(1);
			buy.m_typeId = m_item.m_typeId;
			buy.m_period = m_periods[m_daysIndex].altPeriod;
			buy.m_daysIndex = (unsigned short)m_daysIndex;
			packet.EncodeBuffer(&buy, sizeof(buy));
			packet.Send(SEND_TO_DEFAULT);
		}
		else {
			unsigned short count = (unsigned short)cart.size();

			WSendPacket packet((enumClientPacket)0x1F);
			packet.EncodeStr(m_message);
			packet.Encode4(m_receiverUID);
			packet.EncodeStr(std::string(msg));
			packet.Encode1(0);
			packet.Encode2(count);

			for (FrListBox::ITEM_LIST::iterator it = cart.begin(); it != cart.end(); ++it) {
				IFF_ITEM_COMMON* pItem = (IFF_ITEM_COMMON*)(*it)->pData;

				buy.m_typeId = pItem->m_typeId;
				buy.m_period = m_periods[m_daysIndex].altPeriod;
				buy.m_daysIndex = (unsigned short)m_daysIndex;

				packet.EncodeBuffer(&buy, sizeof(buy));
			}

			packet.Send(SEND_TO_DEFAULT);
		}
	}
	else {
		unsigned short count = (unsigned short)cart.size();

		num = 1;
		WSendPacket packet((enumClientPacket)0x1F);
		packet.EncodeStr(m_message);
		packet.Encode4(m_receiverUID);
		packet.EncodeStr(std::string(msg));
		packet.Encode1(0);
		packet.Encode2(count);

		for (FrListBox::ITEM_LIST::iterator it = cart.begin(); it != cart.end(); ++it) {
			IFF_ITEM_COMMON* pItem = (IFF_ITEM_COMMON*)(*it)->pData;

			buy.m_typeId = pItem->m_typeId;

			buy.m_daysIndex = 0xffff;
			if ((m_item.m_typeId >> 26) == 5) {
				IFF_STRUCT::sBall* pBall = ItemManager()->FindBall(m_item.m_typeId);
				if (pBall)
					num = pBall->quantity;
			}
			else if ((m_item.m_typeId >> 26) == 6 && (m_item.m_typeId & 0x2000000)) {
				IFF_STRUCT::sItem* pIff = ItemManager()->FindItem(m_item.m_typeId);
				if (pIff)
					num = pIff->quantity;
			}
			else if ((m_item.m_typeId >> 26) == 6 && !(m_item.m_typeId & 0x2000000) && (m_item.m_flags0 & 1) && count == 1) {
				IFF_STRUCT::sItem* pIff = ItemManager()->FindItem(m_item.m_typeId);
				if (pIff)
					num = pIff->quantity;
			}

			if ((m_item.m_typeId >> 26) == 0xE || (m_item.m_typeId >> 26) == 8 || (m_item.m_typeId >> 26) == 0x10) {
				if (!S5::ISCutinItem(m_item.m_typeId))
					buy.m_period = (unsigned short)days[m_daysIndex].m_days;
			}
			else {
				buy.m_period = 0;
			}

			buy.m_count = num;

			packet.EncodeBuffer(&buy, sizeof(buy));
		}

		packet.Send(SEND_TO_DEFAULT);
	}

	WSingleton<CSharedDoc>::Instance()->m_lastGiftReceiverNick = std::string(m_pEdNick->GetLine(1, false));

	WSingleton<CSharedDoc>::Instance()->m_lastGiftReceiverUID = m_receiverUID;

	AfxGetTask()->GetActor("Shop") << MsgObject(0, 0xC0, 0, 0, 0, 0, 0);
	AfxGetTask()->GetActor("Shop") << MsgObject(0, 1, 0, 0, 0, 0, 0);
	Close(FrOK, true);
}

// giftdlg.cpp:476
void FrGiftDlg::SetReceiverInfo(unsigned long uid, const sPangYaUserInfo& info) {
	char text[32];   // unused; its /GS cookie survives

	m_receiverUID = uid;

	if (m_pEdNick) {
		m_pEdNick->SetLine(1, info.nickname, 0, false, 0);
	}

	if (m_pEdConfirm) {
		switch (m_receiverMode) {
		case 1:
			m_pEdConfirm->SetLine(1, info.nickname, 0, false, 0);
			m_receiverNick = info.nickname;
			break;
		case 0: {
			if (m_pEdId) {
				m_pEdId->ClearLine();
			}

			std::string id(info.id);
			unsigned int len = id.length();
			id.erase(id.begin() + len / 2, id.end());
			len -= id.length();
			id.insert(id.end(), len, '*');

			if (m_pEdConfirm) {
				m_pEdConfirm->SetLine(1, id.c_str(), 0, false, 0);
			}

			m_receiverNick = info.id;
			break;
		}
		}
	}
}

// giftdlg.cpp:533
void FrGiftDlg::SetPrice(int num) {
	m_bPayPang = true;
	m_bPayCookie = true;

	if (m_pPrice && m_pMyMoney && m_pangPrice) {
		char buf[32] = { 0 }; MakePriceStrA(buf, num * m_pangPrice);

		strcat(buf, "\xc6\xce");
		m_pPrice->SetLine(1, buf, 0, false, 0);

		__int64 myPang = WSingleton<CSharedDoc>::Instance()->m_myPang;

		if (myPang - num * m_pangPrice < 0) {
			if (m_pMoneyLeft) {
				MakePriceStrA(buf, num * m_pangPrice - myPang);
				strcat(buf, "\xc6\xce \xba\xce\xc1\xb7");
				m_pMoneyLeft->SetLine(1, buf, 0xFFFF0000, false, 0);
			}

			m_bPayPang = false;
		}
		else {
			if (m_pMoneyLeft) {
				MakePriceStrA(buf, myPang - num * m_pangPrice);
				strcat(buf, "\xc6\xce");
				m_pMoneyLeft->SetLine(1, buf, 0, false, 0);
			}
		}
	}
	else if (m_pMoneyLeft) {
		m_pMoneyLeft->SetLine(1, m_pMyMoney->GetLine(1, false), 0, false, 0);
	}

	if (m_pPriceCookie && m_pMyMoneyCookie && m_cookiePrice) {
		char buf[32] = { 0 };
		sprintf(buf, "%d\xc4\xed\xc5\xb0", m_cookiePrice);

		m_pPriceCookie->SetLine(1, buf, 0, false, 0);

		__int64 myCash = WSingleton<CSharedDoc>::Instance()->m_myCash;

		if (myCash - m_cookiePrice < 0) {
			if (m_pMoneyLeftCookie) {
				MakePriceStrA(buf, m_cookiePrice - myCash);
				strcat(buf, "\xc4\xed\xc5\xb0 \xba\xce\xc1\xb7");
				m_pMoneyLeftCookie->SetLine(1, buf, 0xFFFF0000, false, 0);
			}

			m_bPayCookie = false;
		}
		else {
			if (m_pMoneyLeftCookie) {
				sprintf(buf, "%d\xc4\xed\xc5\xb0", myCash - m_cookiePrice);
				m_pMoneyLeftCookie->SetLine(1, buf, 0, false, 0);
			}
		}
	}
	else if (m_pMoneyLeftCookie) {
		m_pMoneyLeftCookie->SetLine(1, m_pMyMoneyCookie->GetLine(1, false), 0, false, 0);
	}

	if (m_pNum)
		m_pNum->SetLine(1, MakeStr("%d", num), 0, false, 0);
}

// giftdlg.cpp:618
bool FrGiftDlg::OnFriendListResult(int ret, FrForm* pForm) {
	if (ret == FrOK) {
		m_receiverMode = 0; m_receiverUID = m_pFriendListDlg->GetUid();
		m_receiverNick = m_pEdConfirm->GetLine(1, false);
	}

	m_pFriendListDlg = 0;

	return true;
}

// giftdlg.cpp:653
bool FrGiftDlg::OnChargeResult(int ret, FrForm*) {
	if (ret == FrOK) {
		WSingleton<CBrowser>::Instance()->InitializePostArgument();

		switch (WSingleton<CSharedDoc>::Instance()->m_billingType) {
		case 2:
			WSingleton<CBrowser>::Instance()->AddPostArgument("sn", "9");
			WSingleton<CBrowser>::Instance()->AddPostArgument("mn", MakeStr("%d", CLoginInfo::Instance()->AuthUid()));
			WSingleton<CBrowser>::Instance()->OpenPost("https://qa.www.gametree.co.kr/InGame/Gateway.aspx", false, true);
			break;

		case 4:
			if (!InternetSetCookieA("https://alpha-bill.hangame.com/", NULL, ((NhnLogin*)CLoginInfo::Instance()->QueryInterface())->GetSiteCookie())) {
				GetLastError();
				MessageBoxA(NULL, "Cookie Setting Fail!", "Error", MB_OK);
				return false;
			}

			WSingleton<CBrowser>::Instance()->AddPostArgument("CHNL", "HG");
			WSingleton<CBrowser>::Instance()->OpenPost("https://alpha-bill.hangame.com/pay/charge.nhn", false, true);
			break;

		default:
			return false;
		}

		AfxGetTask()->GetActor("Shop") << MsgObject(0, 0xC2, 0, 0, 0, 0, 0);
		g_input->Reset();

		Close(FrCANCEL, true);
	}

	return true;
}

// giftdlg.cpp:763
bool FrGiftDlg::OnConfirmResult(int ret, FrForm*) {
	if (ret == FrOK) {
		GiftProgress();
	}

	return true;
}

// giftdlg.cpp:789
void FrGiftDlg::OnCheckoutInit(intptr_t param) {
	m_pCheckoutBtn = DYNAMIC_CAST(FrButton, param);
}

// giftdlg.cpp:794
void FrGiftDlg::OnCheckoutBtnUp(void) {
	if (m_receiverMode == -1 || m_receiverUID == 0xffffffff) {
		// "Before sending, make sure the receiver is right:\npress the 'Confirm' button to check"
		AfxGetTask()->GetActor("Shop") << MsgObject(0, 0x23, (intptr_t)"\xba\xb8\xb3\xbb\xb1\xe2 \xc0\xfc\xbf\xa1 \xb9\xde\xb4\xc2 \xc0\xcc\xb0\xa1 \xc8\xae\xbd\xc7\xc8\xf7 \xb8\xc2\xb4\xc2\xc1\xf6\n\xb2\xc0 \\c0xffff0000\\c'\xc8\xae\xc0\xce'\\c0xff000000\\c \xb9\xf6\xc6\xb0\xc0\xbb \xb4\xad\xb7\xaf \xc8\xae\xc0\xce\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4", 0, 0, 0, 0);
		return;
	}

	if (GiftForbiddenMode()) {
		// "This account cannot send gifts."
		AfxGetTask()->GetActor("Shop") << MsgObject(0, 0x23, (intptr_t)"\xbc\xb1\xb9\xb0\xc7\xcf\xb1\xe2\xb8\xa6 \xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb0\xe8\xc1\xa4\xc0\xd4\xb4\xcf\xb4\xd9.", 0, 0, 0, 0);
		return;
	}

	if ((m_receiverMode == 0 && m_receiverNick == WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().nickname)
		|| (m_receiverMode == 1 && m_receiverNick == WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().nickname)) {
		// "You cannot send a gift to yourself."
		AfxGetTask()->GetActor("Shop") << MsgObject(0, 0x23, (intptr_t)"\xc0\xda\xb1\xe2 \xc0\xda\xbd\xc5\xbf\xa1\xb0\xd4\xb4\xc2 \xbc\xb1\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0);
		return;
	}

	if (!m_bPayPang) {
		// "Not enough pang.\nPlay games to collect pang."
		AfxGetTask()->GetActor("Shop") << MsgObject(0, 0x23, (intptr_t)"\xc6\xce\xc0\xcc \xb8\xf0\xc0\xda\xb6\xf8\xb4\xcf\xb4\xd9.\n\xb0\xd4\xc0\xd3\xbf\xa1 \xc2\xfc\xbf\xa9\xc7\xd8\xbc\xad \xc6\xce\xc0\xbb \xb8\xf0\xc0\xb8\xbc\xbc\xbf\xe4.", 0, 0, 0, 0);
		return;
	}

	if (!m_bPayCookie) {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_okcancel", 0);
		// "Not enough cookies.\nCharge cookies?"
		pForm->SetMessage("\\c0xffff0000\\c\xc4\xed\xc5\xb0\\c0xff000000\\c\xb0\xa1 \xb8\xf0\xc0\xda\xb6\xf8\xb4\xcf\xb4\xd9.\n\xc4\xed\xc5\xb0\xb8\xa6 \xc3\xe6\xc0\xfc\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?", false);
		pForm->Open((FRESH_RESULT_CALLBACK)&FrGiftDlg::OnChargeResult, 3);
		return;
	}

	if (!ItemManager()->IsInSalePeriod(m_item.m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
		// "Please check the sale period."
		AfxGetTask()->GetActor("Shop") << MsgObject(0, 0x23, (intptr_t)"\xc6\xc7\xb8\xc5\xb1\xe2\xb0\xa3\xc0\xbb \xc8\xae\xc0\xce\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.", 0, 0, 0, 0);
		return;
	}

	FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_okcancel_big", 0);
	// "Send the gift to '%s'?"
	std::string text(MakeStr("'\\c0xffff0000\\c%s\\c0xff000000\\c'\xb4\xd4\xbf\xa1\xb0\xd4 \xbc\xb1\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbb\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?", m_pEdNick->GetLine(1, false)));

	text += " ";
	text += "\n\\c0xffff0000\\c\xbc\xb1\xb9\xb0\xc7\xcf\xbd\xc3\xb1\xe2 \xc0\xfc\xbf\xa1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xb1\xe2\xc7\xd1\xb0\xfa \xbc\xf6\xb7\xae\xc0\xbb \xb2\xc0 \xc8\xae\xc0\xce\xc7\xcf\xbf\xa9 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0.\\c0xff000000\\c";
	text += "\n\\c0xffff0000\\c\xc0\xcc \xbb\xf3\xc7\xb0\xc0\xba \xc3\xbb\xbe\xe0\xc3\xb6\xc8\xb8(\xb1\xb8\xb8\xc5\xc3\xeb\xbc\xd2)\xb0\xa1 \xba\xd2\xb0\xa1\xb4\xc9\xc7\xd1 \xbb\xf3\xc7\xb0\xc0\xd4\xb4\xcf\xb4\xd9.\\c0xff000000\\c";

	pForm->SetMessage(text.c_str(), false);

	pForm->Open((FRESH_RESULT_CALLBACK)&FrGiftDlg::OnConfirmResult, 1);
}

// giftdlg.cpp:863
void FrGiftDlg::OnFriendBtnInit(intptr_t param) {
	m_pFriendBtn = DYNAMIC_CAST(FrButton, param);
}

// giftdlg.cpp:869
void FrGiftDlg::OnFriendBtnUp(void) {
	if (BuddyMGR->GetBuddySize() == 0) {
		if (!WSingleton<WNetworkSystem>::Instance()->IsConnected((WNetworkSystem::eNetUnit)1)) {
			WSendPacket packet(GCT_MSN_REQUEST);
			packet.Encode2(0x11f);
			packet.Send(SEND_TO_DEFAULT);
		} else {
			WSendPacket packet(GCR_MESSENGER);
			packet.Encode2(0x11f);
			packet.Send(SEND_TO_MSN);
		}
	}

	if (!m_pFriendListDlg) {
		m_pFriendListDlg = CreateForm<FrFriendDlg>(g_pFresh->GetManager(), this, "friend_list_form", 0);
		if (m_pFriendListDlg) {
			m_pEdId->ClearLine();

			m_pFriendListDlg->SetParent(this);
			m_pFriendListDlg->EnableDrag(false);
			m_pFriendListDlg->Open((FRESH_RESULT_CALLBACK)&FrGiftDlg::OnFriendListResult, WPoint(582.0f, 35.0f), 1);
		}
	}
}

// giftdlg.cpp:901
void FrGiftDlg::OnCartInit(intptr_t param) {
	m_pCart = DYNAMIC_CAST(FrListBox, param);

	m_pClock = g_pFresh->GetManager()->GetBitmap("ICONS", "clock");
	m_rcClock = WRect(40.0f, 35.0f, m_pClock->Width(), m_pClock->Height());
}

// giftdlg.cpp:910
void FrGiftDlg::OnCartOwnerDraw(intptr_t param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem) {
		FrGraphicInterface* pGDI = m_pCart->WndManager()->GetGDI();

		IFF_ITEM_COMMON* pData = (IFF_ITEM_COMMON*)pItem->pData;

		const Bitmap* pBmp = m_pCart->WndManager()->GetBitmap("ITEMS", pData->m_icon);
		if ((pData->m_typeId >> 26) == 14) {
			pBmp = m_pCart->WndManager()->GetBitmap("SKINS", pData->m_icon);
		}
		else if ((pData->m_typeId >> 26) == 15) {
			pBmp = m_pCart->WndManager()->GetBitmap("HAIRS", pData->m_icon);
		}
		else {
			pBmp = m_pCart->WndManager()->GetBitmap("ITEMS", pData->m_icon);

			if (pBmp == 0)
				pBmp = m_pCart->WndManager()->GetBitmap("ITEMS_FASHION", pData->m_icon);
		}

		if (pBmp) {
			WRect rc(pItem->pos.x, pItem->pos.y, (float)pBmp->Width(), (float)pBmp->Height());

			if ((pData->m_typeId >> 26) != 2 && (pData->m_typeId >> 26) != 15) {
				rc.x += 10.0f;
				rc.y += 20.0f;
			}

			pGDI->DrawTexture(pBmp, rc, 0xFFFFFFFF, 0);

			if (m_periodCount > 0) {
				if (m_periods[m_daysIndex].altPeriodUnit) {
					WPoint pos(pItem->pos.x, pItem->pos.y + 10.0f);
					pGDI->DrawTexture(m_pClock, WRect(pos.x + m_rcClock.x, pos.y + m_rcClock.y, m_rcClock.w, m_rcClock.h), 0xFFFFFFFF, 0);
				}
			}
		}

		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGDI->SetTextStyle(1);

		// "(gift only)"
		float width = pGDI->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 14.0f), 0, "%s%s", pData->m_name, (pData->m_flags0 >> 5) == 2 ? "(\xbc\xb1\xb9\xb0\xc0\xfc\xbf\xeb)" : "");

		if (!ItemManager()->IsInSalePeriod(pData->m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
			pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
			// "(sale period over)"
			pGDI->Print(WPoint((int)width + 90.0f + pItem->pos.x, pItem->pos.y + 14.0f), 0, "(\xc6\xc7\xb8\xc5\xb1\xe2\xb0\xa3 \xc1\xbe\xb7\xe1)");
			pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		}

		pGDI->SetTextStyle(0);

		if (m_periodCount > 0) {
			__int64 price = 0;
			if (m_periods[m_daysIndex].hasPrice)
				price = m_periods[m_daysIndex].price;
			else if (m_periods[m_daysIndex].altPeriodUnit)
				price = m_periods[m_daysIndex].altPrice;

			if (m_periods[m_daysIndex].isCash) {
				// "price:"
				float labelWidth = pGDI->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 34.0f), 0, "\xb0\xa1    \xb0\xdd:   ");
				pGDI->Print(WPoint((int)labelWidth + 90.0f + pItem->pos.x, pItem->pos.y + 34.0f), 0, "%I64d\xc4\xed\xc5\xb0", price);
			}
			else {
				float labelWidth = pGDI->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 34.0f), 0, "\xb0\xa1    \xb0\xdd:   ");
				pGDI->Print(WPoint((int)labelWidth + 90.0f + pItem->pos.x, pItem->pos.y + 34.0f), 0, "%I64d\xc6\xce", price);
			}
		}
		else if (pData->m_flags0 & 1) {
			int labelWidth = (int)pGDI->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 34.0f), 0, "\xb0\xa1    \xb0\xdd:   ");

			if ((pData->m_typeId >> 26) == 14) {
				IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pData->m_typeId);
				if (!S5::ISCutinItem(pData->m_typeId)) {
					if (pSkin)
						pGDI->Print(WPoint(labelWidth + 90.0f + pItem->pos.x, pItem->pos.y + 34.0f), 0, "%d\xc4\xed\xc5\xb0", pSkin->m_periodPrices[m_daysIndex]);
				}
				else if (pSkin) {
					if (pData->m_discountPrice > 0 && pData->m_discountPrice < pData->m_price)
						pGDI->Print(WPoint(labelWidth + 90.0f + pItem->pos.x, pItem->pos.y + 34.0f), 0, "%d\xc4\xed\xc5\xb0", pData->m_discountPrice);
					else
						pGDI->Print(WPoint(labelWidth + 90.0f + pItem->pos.x, pItem->pos.y + 34.0f), 0, "%d\xc4\xed\xc5\xb0", pData->m_price);
				}
			}
			else if ((pData->m_typeId >> 26) == 8) {
				IFF_STRUCT::sCadItem* pCad = ItemManager()->FindCadItem(pData->m_typeId);
				pGDI->Print(WPoint(labelWidth + 90.0f + pItem->pos.x, pItem->pos.y + 34.0f), 0, "%d\xc4\xed\xc5\xb0", pCad->m_levels[m_daysIndex]);
			}
			else {
				float priceX = (float)labelWidth;
				int priceWidth = float2int(pGDI->Print(WPoint(priceX + 90.0f + pItem->pos.x, pItem->pos.y + 34.0f), 0, "%d\xc4\xed\xc5\xb0", pData->m_price));

				if (pData->m_discountPrice > 0 && pData->m_discountPrice < pData->m_price) {
					float startX = priceX + 90.0f - 2.0f;
					priceX += priceWidth;
					g_view->DrawLine2D(WPoint(startX + pItem->pos.x, float2int(pGDI->GetFontHeight() * 0.5f) + 34.0f + pItem->pos.y),
						WPoint(priceX + 92.0f + pItem->pos.x, float2int(pGDI->GetFontHeight() * 0.5f) + 34.0f + pItem->pos.y), 0xFF000000, 0xFF000000, 0);

					pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
					pGDI->Print(WPoint(priceX + 90.0f + pItem->pos.x, pItem->pos.y + 34.0f), 0, " %d\xc4\xed\xc5\xb0", pData->m_discountPrice);
					pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
				}
			}
		}
		else {
			int labelWidth = (int)pGDI->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 34.0f), 0, "\xb0\xa1    \xb0\xdd:   ");

			// "%d pang + %d pang (fee)"
			if ((pData->m_typeId >> 26) == 14) {
				if (!S5::ISCutinItem(pData->m_typeId)) {
					IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pData->m_typeId);
					if (pSkin)
						pGDI->Print(WPoint(labelWidth + 90.0f + pItem->pos.x, pItem->pos.y + 34.0f), 0, "%d\xc6\xce + %d\xc6\xce(\xbc\xf6\xbc\xf6\xb7\xe1)", pSkin->m_periodPrices[m_daysIndex], pSkin->m_periodPrices[m_daysIndex] * 5 / 100);
				}
				else if (pData->m_discountPrice > 0 && pData->m_discountPrice < pData->m_price)
					pGDI->Print(WPoint(labelWidth + 90.0f + pItem->pos.x, pItem->pos.y + 34.0f), 0, "%d\xc6\xce + %d\xc6\xce(\xbc\xf6\xbc\xf6\xb7\xe1)", pData->m_discountPrice, pData->m_discountPrice * 5 / 100);
				else
					pGDI->Print(WPoint(labelWidth + 90.0f + pItem->pos.x, pItem->pos.y + 34.0f), 0, "%d\xc6\xce + %d\xc6\xce(\xbc\xf6\xbc\xf6\xb7\xe1)", pData->m_price, pData->m_price * 5 / 100);
			}
			else if ((pData->m_typeId >> 26) == 8) {
				IFF_STRUCT::sCadItem* pCad = ItemManager()->FindCadItem(pData->m_typeId);
				pGDI->Print(WPoint(labelWidth + 90.0f + pItem->pos.x, pItem->pos.y + 34.0f), 0, "%d\xc6\xce + %d\xc6\xce(\xbc\xf6\xbc\xf6\xb7\xe1)", pCad->m_levels[m_daysIndex], pCad->m_levels[m_daysIndex] * 5 / 100);
			}
			else if ((pData->m_typeId >> 26) == 16) {
				IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(pData->m_typeId);
				pGDI->Print(WPoint(labelWidth + 90.0f + pItem->pos.x, pItem->pos.y + 34.0f), 0, "%d\xc6\xce + %d\xc6\xce(\xbc\xf6\xbc\xf6\xb7\xe1)", pMascot->m_periodPrices[m_daysIndex], pMascot->m_periodPrices[m_daysIndex] * 5 / 100);
			}
			else {
				float priceX = (float)labelWidth;
				int priceWidth = float2int(pGDI->Print(WPoint(priceX + 90.0f + pItem->pos.x, pItem->pos.y + 34.0f), 0, "%d\xc6\xce + %d\xc6\xce(\xbc\xf6\xbc\xf6\xb7\xe1)", pData->m_price, pData->m_price * 5 / 100));

				if (pData->m_discountPrice > 0 && pData->m_discountPrice < pData->m_price) {
					float startX = priceX + 90.0f - 2.0f;
					priceX += priceWidth;
					g_view->DrawLine2D(WPoint(startX + pItem->pos.x, float2int(pGDI->GetFontHeight() * 0.5f) + 34.0f + pItem->pos.y),
						WPoint(priceX + 92.0f + pItem->pos.x, float2int(pGDI->GetFontHeight() * 0.5f) + 34.0f + pItem->pos.y), 0xFF000000, 0xFF000000, 0);

					pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
					pGDI->Print(WPoint(priceX + 90.0f + pItem->pos.x, pItem->pos.y + 34.0f), 0, " %d\xc6\xce + %d\xc6\xce(\xbc\xf6\xbc\xf6\xb7\xe1)", pData->m_discountPrice, pData->m_discountPrice * 5 / 100);
					pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
				}
			}
		}

		float lineY = 54.0f;
		_SYSTEMTIME* pBegin = 0;
		_SYSTEMTIME* pEnd = 0;
		if (ItemManager()->HasSalePeriod(pData->m_typeId, &pBegin, &pEnd)) {
			if (pBegin->wYear != 0 && pEnd->wYear != 0) {
				pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
				// "sale period: %d/%d ~ %d/%d"
				pGDI->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 54.0f), 0, MakeStr("\xc6\xc7\xb8\xc5\xb1\xe2\xb0\xa3: %d\xbf\xf9 %d\xc0\xcf ~ %d\xbf\xf9 %d\xc0\xcf", pBegin->wMonth, pBegin->wDay, pEnd->wMonth, pEnd->wDay));
				pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

				lineY += 20.0f;
				static WRect s_rcDays = m_pDays->GetRect();
				WRect rcDays = s_rcDays;
				rcDays.y = s_rcDays.y + 18.0f;
				m_pDays->SetRect(rcDays);
			}
		}

		const Bitmap* pLevel = g_pFresh->GetManager()->GetBitmap("LEVELS", MakeStr("level_%03d", (pData->m_level & 0x7F) + 1));
		if (pLevel) {
			// "level:"
			float levelX = pGDI->GetTextExtend("\xb7\xb9    \xba\xa7:") + pItem->pos.x;
			WRect rc(levelX + 102.0f, lineY + pItem->pos.y - 3.0f, (float)pLevel->Width(), (float)pLevel->Height());
			pGDI->DrawTexture(pLevel, rc, 0xFFFFFFFF, 0);

			pGDI->Print(WPoint(pItem->pos.x + 90.0f, lineY + pItem->pos.y), 0, "\xb7\xb9    \xba\xa7:");
			// "or higher"
			pGDI->Print(WPoint(rc.w + rc.x + 5.0f, lineY + pItem->pos.y), 0, "\xc0\xcc\xbb\xf3");
		}

		lineY += 20.0f;
		// "period of use:"
		if ((pData->m_typeId >> 26) == 7) {
			pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
			pGDI->Print(WPoint(pItem->pos.x + 90.0f, lineY + pItem->pos.y), 0, "\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:");
			pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);

			// "unlimited" / "monthly"
			const char* text = (pData->m_flags0 & 1) ? "\xb9\xab\xc1\xa6\xc7\xd1" : "\xbf\xf9\xb1\xde\xc1\xa6";
			pGDI->Print(WPoint(pGDI->GetTextExtend("\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:") + 95.0f + pItem->pos.x, lineY + pItem->pos.y), 0, text);
		}
		else if ((pData->m_typeId >> 26) == 8) {
			pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
			pGDI->Print(WPoint(pItem->pos.x + 90.0f, lineY + pItem->pos.y), 0, "\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:");
			pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);

			if (m_bItemSet) {
				m_pDays->SetVisible(true);

				IFF_STRUCT::sCadItem* pCad = ItemManager()->FindCadItem(pData->m_typeId);
				for (int i = 0; i < 4; i++) {
					if (pCad->m_levels[i])
						m_pDays->AddString(days[i].m_name);
				}

				m_pDays->SetLine(1, days[m_daysIndex].m_name, 0, false, 0);

				m_bItemSet = false;
			}
		}
		else if ((pData->m_typeId >> 26) == 14 && !S5::ISCutinItem(pData->m_typeId)) {
			pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
			pGDI->Print(WPoint(pItem->pos.x + 90.0f, lineY + pItem->pos.y), 0, "\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:");
			pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);

			if (m_bItemSet) {
				m_pDays->SetVisible(true);

				IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pData->m_typeId);
				for (int i = 0; i < 5; i++) {
					if (pSkin->m_periodPrices[i])
						m_pDays->AddString(days[i].m_name);
				}

				m_pDays->SetLine(1, days[m_daysIndex].m_name, 0, false, 0);

				m_bItemSet = false;
			}
		}
		else if ((pData->m_typeId >> 26) == 16) {
			pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
			pGDI->Print(WPoint(pItem->pos.x + 90.0f, lineY + pItem->pos.y), 0, "\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:");
			pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);

			if (m_bItemSet) {
				m_pDays->SetVisible(true);

				IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(pData->m_typeId);
				for (int i = 0; i < 5; i++) {
					if (pMascot->m_periodPrices[i])
						m_pDays->AddString(days[i].m_name);
				}

				m_pDays->SetLine(1, days[m_daysIndex].m_name, 0, false, 0);

				m_bItemSet = false;
			}
		}
		else if (m_periodCount > 0) {
			IFF_STRUCT::sChildItem* pChild = ItemManager()->FindChildItem(pData->m_typeId, m_daysIndex);

			if (pChild && pChild->m_hasPrice) {
				pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
				// "count:"
				pGDI->Print(WPoint(pItem->pos.x + 90.0f, lineY + pItem->pos.y), 0, "\xb0\xb3    \xbc\xf6:");
			}
			else {
				pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
				pGDI->Print(WPoint(pItem->pos.x + 90.0f, lineY + pItem->pos.y), 0, "\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:");
			}
			pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);

			if (m_bItemSet) {
				m_pDays->SetVisible(true);

				for (int i = 0; i < m_periodCount; i++) {
					pChild = ItemManager()->FindChildItem(pData->m_typeId, i);
					if (pChild) {
						if (pChild->m_enabled)
							m_pDays->AddString(m_periods[i].name);
					}
				}

				m_pDays->SetLine(1, m_periods[m_daysIndex].name, 0, false, 0);
				m_bItemSet = false;
			}
		}

		if (m_pExplaneStatic && m_bTotalCookieSet) {
			pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
			pGDI->SetTextStyle(1);

			const char* cashName;
			switch (WSingleton<CSharedDoc>::Instance()->m_billingType) {
			case 2: cashName = "\xc6\xae\xb8\xae\xc4\xb3\xbd\xac"; break;   // "tree cash"
			case 4: cashName = "\xc7\xd1\xc4\xda\xc0\xce"; break;   // "han coin"
			default:
				pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
				pGDI->SetTextStyle(0);
				return;
			}

			const WRect& rc = m_pExplaneStatic->GetRect();
			if (m_totalCookie < 0) {
				// "Not enough %s balance."
				pGDI->Print(WPoint(rc.x - 70.0f, rc.y - 20.0f), 0, MakeStr("%s \xc0\xdc\xbe\xd7\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.", cashName));
			}
			else {
				__int64 balance = WSingleton<CSharedDoc>::Instance()->m_outsideCash;
				__int64 bonus = WSingleton<CSharedDoc>::Instance()->m_myBonusCash - m_cookiePrice;
				__int64 cash = S5::ExchangeOwnCookieToCash(bonus, 100);
				if (cash < 0)
					balance = WSingleton<CSharedDoc>::Instance()->m_outsideCash + cash;

				// "Your final %s balance after the purchase is %I64d won."
				pGDI->Print(WPoint(rc.x - 140.0f, rc.y - 20.0f), 0, MakeStr("\xb1\xb8\xb8\xc5 \xc8\xc4 \xc3\xd6\xc1\xbe %s \xc0\xdc\xbe\xd7\xc0\xba %I64d\xbf\xf8 \xc0\xd4\xb4\xcf\xb4\xd9.", cashName, balance > 0 ? balance : (__int64)0));
			}

			pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
			pGDI->SetTextStyle(0);
		}
	}
}

// giftdlg.cpp:1353
void FrGiftDlg::OnSxIdInit(intptr_t param) {
	m_pSxId = DYNAMIC_CAST(FrStatic, param);
	if (m_pSxId) {
		m_pSxId->SetVisible(false);
	}
}

// giftdlg.cpp:1365
void FrGiftDlg::OnEdIdInit(intptr_t param) {
	m_pEdId = DYNAMIC_CAST(FrEdit, param);
	if (m_pEdId) {
		m_pEdId->SetKeyFocus(true);
	}

	if (m_pEdId) {
		m_pEdId->SetVisible(false);
	}
}

// giftdlg.cpp:1380
bool FrGiftDlg::OnEdIdEnterKey(PG_MSGARG) {
	return false;
}

// giftdlg.cpp:1387
void FrGiftDlg::OnEdIdDown(void) {
}

// giftdlg.cpp:1394
void FrGiftDlg::OnEdNickDown(void) {
	m_receiverMode = -1;
}

// giftdlg.cpp:1401
void FrGiftDlg::OnBtIdCheckInit(intptr_t param) {
	m_pBtIdCheck = DYNAMIC_CAST(FrButton, param);
	if (m_pBtIdCheck) {
		m_pBtIdCheck->SetVisible(false);
	}
}

// giftdlg.cpp:1413
void FrGiftDlg::OnBtIdCheckUp(void) {
}

// giftdlg.cpp:1446
void FrGiftDlg::OnSxNickInit(intptr_t param) {
	m_pSxNick = DYNAMIC_CAST(FrStatic, param);
}

// giftdlg.cpp:1451
void FrGiftDlg::OnEdNickInit(intptr_t param) {
	m_pEdNick = DYNAMIC_CAST(FrEdit, param);
}

// giftdlg.cpp:1463
bool FrGiftDlg::OnEdNickEnterKey(PG_MSGARG) {
	OnBtNickCheckUp();
	return false;
}

// giftdlg.cpp:1470
void FrGiftDlg::OnMsgInit(intptr_t param) {
	m_pMsg = DYNAMIC_CAST(FrEdit, param);
}

// giftdlg.cpp:1476
bool FrGiftDlg::OnMsgEnterKey(PG_MSGARG) {
	OnCheckoutBtnUp();
	return false;
}

// giftdlg.cpp:1483
void FrGiftDlg::OnEdConfirmInit(intptr_t param) {
	m_pEdConfirm = DYNAMIC_CAST(FrEdit, param);
	if (m_pEdConfirm) {
		m_pEdConfirm->SetLine(1, "<\xc8\xae\xc0\xce \xbe\xc8\xb5\xca>", 0, false, 0);
	}
}

// giftdlg.cpp:1492
void FrGiftDlg::OnBtNickCheckInit(intptr_t param) {
	m_pBtNickCheck = DYNAMIC_CAST(FrButton, param);
}

// giftdlg.cpp:1498
void FrGiftDlg::OnBtNickCheckUp(void) {
	if (m_pEdConfirm)
		m_pEdConfirm->ClearLine();

	if (m_pEdNick) {
		const char* nick = m_pEdNick->GetLine(1, false);
		int len = strlen(nick);
		if (len <= 0) {
			AfxGetTask()->GetActor("Shop") << MsgObject(0, 0x23, (intptr_t)"\xb9\xde\xb4\xc2\xc0\xcc\xc0\xc7 \xb4\xeb\xc8\xad\xb8\xed\xc0\xbb \xb3\xd6\xbe\xee\xc1\xd6\xbc\xbc\xbf\xe4", 0, 0, 0, 0);
		} else if (len >= 0x16) {
			AfxGetTask()->GetActor("Shop") << MsgObject(0, 0x23, (intptr_t)MakeStr("\xb4\xeb\xc8\xad\xb8\xed\xc0\xba %d\xc0\xda \xb1\xee\xc1\xf6 \xc0\xd4\xb7\xc2\xc7\xd2\xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.", 0x16), 0, 0, 0, 0);
		} else if (strcmp(WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().nickname, nick) != 0) {
			WSendPacket packet(GCT_REQUEST_USERINFO_OFFLINE);

			packet.Encode1(0);

			packet.EncodeStr(std::string(nick));
			packet.Send(SEND_TO_DEFAULT);

			m_receiverMode = 0;

			AfxGetTask()->GetActor("Shop") << MsgObject(0, 1, 0, 0, 0, 0, 0);
		} else {
			AfxGetTask()->GetActor("Shop") << MsgObject(0, 0x23, (intptr_t)"\xc0\xda\xb1\xe2 \xc0\xda\xbd\xc5\xbf\xa1\xb0\xd4\xb4\xc2 \xbc\xb1\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0);
		}
	}

	if (m_pMsg)
		m_pMsg->SetKeyFocus(true);
}

// giftdlg.cpp:1552
void FrGiftDlg::OnPriceInit(intptr_t param) {
	m_pPrice = DYNAMIC_CAST(FrEdit, param);
}

// giftdlg.cpp:1558
void FrGiftDlg::OnMyMoneyInit(intptr_t param) {
	m_pMyMoney = DYNAMIC_CAST(FrEdit, param);
}

// giftdlg.cpp:1564
void FrGiftDlg::OnMoneyLeftInit(intptr_t param) {
	m_pMoneyLeft = DYNAMIC_CAST(FrEdit, param);
}

// giftdlg.cpp:1570
void FrGiftDlg::OnPrice_CookieInit(intptr_t param) {
	m_pPriceCookie = DYNAMIC_CAST(FrEdit, param);
}

// giftdlg.cpp:1578
void FrGiftDlg::OnMyMoney_CookieInit(intptr_t param) {
	m_pMyMoneyCookie = DYNAMIC_CAST(FrEdit, param);
}

// giftdlg.cpp:1586
void FrGiftDlg::OnMoneyLeft_CookieInit(intptr_t param) {
	m_pMoneyLeftCookie = DYNAMIC_CAST(FrEdit, param);
}

// giftdlg.cpp:1594
void FrGiftDlg::OnNumStaticInit(intptr_t param) {
	m_pNumStatic = DYNAMIC_CAST(FrStatic, param);
}

// giftdlg.cpp:1600
void FrGiftDlg::OnNumInit(intptr_t param) {
	m_pNum = DYNAMIC_CAST(FrEdit, param);
}

// giftdlg.cpp:1606
void FrGiftDlg::OnNumPrevInit(intptr_t param) {
	m_pNumPrev = DYNAMIC_CAST(FrButton, param);
	if (m_pNumPrev) {
		m_pNumPrev->SetPushDelay(0.0f);
	}
}

// giftdlg.cpp:1614
void FrGiftDlg::OnNumPrevBtnUp(void) {
	if (m_pNum) {
		const char* text = m_pNum->GetLine(1, false);
		int num = atoi(text);

		if (num > 1) {
			SetPrice(num - 1);
		}
	}
}

// giftdlg.cpp:1627
void FrGiftDlg::OnNumNextInit(intptr_t param) {
	m_pNumNext = DYNAMIC_CAST(FrButton, param);
	if (m_pNumNext) {
		m_pNumNext->SetPushDelay(0.0f);
	}
}

// giftdlg.cpp:1635
void FrGiftDlg::OnNumNextBtnUp(void) {
	if (m_pNum) {
		const char* text = m_pNum->GetLine(1, false);
		int num = atoi(text);

		if (num < 10) {
			SetPrice(num + 1);
		}
	}
}

// giftdlg.cpp:1648
void FrGiftDlg::RecalculateTotalCookie(__int64 pang, __int64 cookie, bool update) {
	m_pangPrice = 0;
	m_cookiePrice = 0;

	if (m_pCart) {
		m_pCart->ClearItem();

		if ((m_item.m_typeId >> 26) == 0xE) {
			if ((m_item.m_typeId & 0x3C00000) == 0x1400000) {
				if (m_item.m_flags0 & 1) {
					if (m_item.m_discountPrice > 0 && m_item.m_discountPrice < m_item.m_price)
						m_cookiePrice += m_item.m_discountPrice;
					else
						m_cookiePrice += m_item.m_price;
				}
				else {
					if (m_item.m_discountPrice > 0 && m_item.m_discountPrice < m_item.m_price)
						m_pangPrice += m_item.m_discountPrice + m_item.m_discountPrice * 5 / 100;
					else
						m_pangPrice += m_item.m_price + m_item.m_price * 5 / 100;
				}
			}
			else {
				IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(m_item.m_typeId);

				if (pSkin->m_flags & 1)
					m_cookiePrice += pSkin->m_periodPrices[m_daysIndex];
				else
					m_pangPrice += pSkin->m_periodPrices[m_daysIndex] + pSkin->m_periodPrices[m_daysIndex] * 5 / 100;
			}
		}
		else if ((m_item.m_typeId >> 26) == 8) {
			IFF_STRUCT::sCadItem* pCadItem = ItemManager()->FindCadItem(m_item.m_typeId);

			if (pCadItem->m_flags & 1)
				m_cookiePrice += pCadItem->m_levels[m_daysIndex];
			else
				m_pangPrice += pCadItem->m_levels[m_daysIndex] + pCadItem->m_levels[m_daysIndex] * 5 / 100;
		}
		else if ((m_item.m_typeId >> 26) == 0x10) {
			IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(m_item.m_typeId);

			if (pMascot->m_flags & 1)
				m_cookiePrice += pMascot->m_periodPrices[m_daysIndex];
			else
				m_pangPrice += pMascot->m_periodPrices[m_daysIndex] + pMascot->m_periodPrices[m_daysIndex] * 5 / 100;
		}
		else if (m_periodCount > 0) {
			__int64 price = 0xffffffff;

			if (m_periods[m_daysIndex].altPeriodUnit)
				price = m_periods[m_daysIndex].altPrice;
			else if (m_periods[m_daysIndex].hasPrice)
				price = m_periods[m_daysIndex].price;

			if (m_periods[m_daysIndex].isCash)
				m_cookiePrice += price;
			else
				m_pangPrice += price + price * 5 / 100;
		}
		else if (m_item.m_flags0 & 1) {
			if (m_item.m_discountPrice > 0 && m_item.m_discountPrice < m_item.m_price)
				m_cookiePrice += m_item.m_discountPrice;
			else
				m_cookiePrice += m_item.m_price;
		}
		else {
			if (m_item.m_discountPrice > 0 && m_item.m_discountPrice < m_item.m_price)
				m_pangPrice += m_item.m_discountPrice + m_item.m_discountPrice * 5 / 100;
			else
				m_pangPrice += m_item.m_price + m_item.m_price * 5 / 100;
		}

		m_pCart->AddItem(&m_item);
	}

	if (update) {
		char buf[32];
		bool bPang = false;
		bool bCash = false;

		if (m_pPrice) {
			MakePriceStrA(buf, m_pangPrice);
			strcat(buf, "\xc6\xce");
			m_pPrice->SetLine(1, buf, 0, false, 0);

			if (m_pangPrice > 0)
				bPang = true;
		}

		if (m_pPriceCookie) {
			sprintf(buf, "%I64d\xc4\xed\xc5\xb0", m_cookiePrice);
			m_pPriceCookie->SetLine(1, buf, 0, false, 0);

			if (m_cookiePrice > 0) {
				bCash = true;
				m_totalCookie = WSingleton<CSharedDoc>::Instance()->RemainOwnCash(WSingleton<CSharedDoc>::Instance()->m_billingType, m_cookiePrice);
			}
		}

		if (bPang == true && bCash == false) {
			m_bTotalCookieSet = false;
			m_totalCookie = 0;
		}
		else {
			m_bTotalCookieSet = true;
		}

		if (m_pMyMoney) {
			MakePriceStrA(buf, pang);
			strcat(buf, "\xc6\xce");
			m_pMyMoney->SetLine(1, buf, 0, false, 0);
		}

		if (m_pMyMoneyCookie) {
			sprintf(buf, "%I64d\xc4\xed\xc5\xb0", cookie);
			m_pMyMoneyCookie->SetLine(1, buf, 0, false, 0);
		}
	}

	SetPrice(1);
}

// giftdlg.cpp:1831
void FrGiftDlg::OnDaysInit(intptr_t param) {
	m_pDays = DYNAMIC_CAST(FrComboBox, param);
}

// giftdlg.cpp:1836
void FrGiftDlg::OnDaysBtnDown(void) {
	if (m_periodCount > 0) {
		const char* text = m_pDays->GetLine(1, false);
		for (int i = 0; i < m_periodCount; i++) {
			if (stricmp(m_periods[i].name, text) == 0) {
				m_daysIndex = i;
			}
		}
	} else {
		const char* text = m_pDays->GetLine(1, false);
		for (int i = 0; i < 5; i++) {
			if (stricmp(days[i].m_name, text) == 0) {
				m_daysIndex = i;
			}
		}
	}

	RecalculateTotalCookie(m_pangAmount, m_cookieAmount, true);
}

// giftdlg.cpp:1868
void FrGiftDlg::OnWarnInit(intptr_t param) {
	m_pWarn = DYNAMIC_CAST(FrStatic, param);
	if (m_pWarn) {
		m_pWarn->SetCaption("\xc6\xce \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xc4\xa3\xb1\xb8\xbf\xa1\xb0\xd4\xb8\xb8 \xbc\xb1\xb9\xb0\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
	}
}

// giftdlg.cpp:1878
void FrGiftDlg::OnEmoticonBtnInit(intptr_t param) {
	m_pEmoticonBtn = DYNAMIC_CAST(FrButton, param);
}

// giftdlg.cpp:1883
void FrGiftDlg::OnEmoticonBtnUp(void) {
	if (m_pEmoticonBtn && !m_pEmoticonDlg) {
		m_pEmoticonDlg = CreateForm<FrEmoticonDlg>(g_pFresh->GetManager(), this, "emoticon", 0);
		if (m_pEmoticonDlg) {
			WPoint pt(0, 0);
			const WRect& rect = m_pEmoticonBtn->GetRect();
			pt.x = rect.x - 200.0f;
			pt.y = rect.y + 24.0f;

			m_pEmoticonDlg->Open((FRESH_RESULT_CALLBACK)&FrGiftDlg::OnEmoticonResult, pt, 3);
		}
	}
}

// giftdlg.cpp:1901
void FrGiftDlg::OnExplaneStaticInit(intptr_t param) {
	m_pExplaneStatic = DYNAMIC_CAST(FrStatic, param);
}

// giftdlg.cpp:1907
bool FrGiftDlg::OnEmoticonResult(int ret, FrForm* pForm) {
	if (m_pEmoticonDlg && ret == FrOK) {
		FrEmoticonDlg* pDlg = DYNAMIC_CAST(FrEmoticonDlg, pForm);
		if (pDlg) {
			const char* icon = pDlg->GetSelectedIcon();
			if (icon) {
				FrEdit* pEdit = DYNAMIC_CAST(FrEdit, FindChildByName("id"));
				if (m_pEdNick) {
					const char* front = m_pEdNick->GetEditText_Front();
					const char* comp = m_pEdNick->GetEditText_Comp();
					const char* end = m_pEdNick->GetEditText_End();
					m_pEdNick->SetLine(1, MakeStr("%s%s%s%s", front, comp, icon, end), 0, false, 0);
					m_pEdNick->SetKeyFocus(true);
				}
			}
		}
	}

	m_pEmoticonDlg = 0;
	return true;
}
