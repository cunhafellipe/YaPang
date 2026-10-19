// Original: d:\build\custom\temp\code\client\projectg\checkoutdlg.cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CheckoutDlg.h"
#include "ObjectFactory.h"
#include "CaddieInfoDlg.h"
#include "bitmap.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frbutton.h"
#include "frcombobox.h"
#include "frdesktop.h"
#include "fredit.h"
#include "frlistbox.h"
#include "frstatic.h"
#include "Browser.h"
#include "logininfo.h"
#include "TaskManager.h"
#include "actor.h"
#include "InputManager.h"
#include "SharedDoc.h"
#include "commonutil.h"
#include "binstr.h"
#include "UccManager.h"
#include "ItemManager.h"
#include "Packet.h"
#include "shared/s5/SharedUtilities.h"
#include "frgraphicinterface.h"
#include "wview.h"
#include "Localize.h"
#include "TitleManager.h"
#include "Titles_Client.h"

// The project's SDK has no <wininet.h>; the function is declared locally.
extern "C" __declspec(dllimport) BOOL __stdcall InternetSetCookieA(LPCSTR lpszUrl, LPCSTR lpszCookieName, LPCSTR lpszCookieData);

// checkoutdlg.h:77
bool FrCheckoutDlg::IsUnderLevel(void) {
	return m_bUnderLevel;
}

// checkoutdlg.h:78
bool FrCheckoutDlg::IsOverLevel(void) {
	return m_bOverLevel;
}

// checkoutdlg.h: no line record (empty constructor of a nested helper class)
FrCheckoutDlg::MiniBtn::MiniBtn(void) {
}

// checkoutdlg.cpp:32 (run-time type and factory registration of FrCheckoutDlg)
const WRTTI FrCheckoutDlg::m_RTTI("FrCheckoutDlg", &FrForm::m_RTTI);

IObject* FrCheckoutDlgMakeInstance(void) {
	return new FrCheckoutDlg;
}

struct __sFrCheckoutDlg {
	__sFrCheckoutDlg(void) {
		ObjectFactory().AddObjectFunctor(FrCheckoutDlgMakeInstance, "FrCheckoutDlg");
	}
};
static __sFrCheckoutDlg s_registerFrCheckoutDlg;

// checkoutdlg.cpp:34
const sFRESH_MSGMAP* FrCheckoutDlg::GetMessageMap(void) const {
	return &FrCheckoutDlg::_MsgMap;
}

sFRESH_MSGMAP FrCheckoutDlg::_MsgMap = { &FrForm::_MsgMap, &FrCheckoutDlg::_MsgEntries[0] };

sFRESH_ENTRY FrCheckoutDlg::_MsgEntries[] = {
	// checkoutdlg.cpp:36
	{ "checkout", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCheckoutDlg::OnCheckoutInit },
	// checkoutdlg.cpp:37
	{ "checkout", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCheckoutDlg::OnCheckoutLBtnUp },
	// checkoutdlg.cpp:38
	{ "cart", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCheckoutDlg::OnCartInit },
	// checkoutdlg.cpp:39
	{ "cart", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCheckoutDlg::OnCartOwnerDraw },
	// checkoutdlg.cpp:40
	{ "price", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCheckoutDlg::OnPriceInit },
	// checkoutdlg.cpp:41
	{ "my_money", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCheckoutDlg::OnMyMoneyInit },
	// checkoutdlg.cpp:42
	{ "money_left", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCheckoutDlg::OnMoneyLeftInit },
	// checkoutdlg.cpp:43
	{ "price_cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCheckoutDlg::OnPrice_CookieInit },
	// checkoutdlg.cpp:44
	{ "my_money_cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCheckoutDlg::OnMyMoney_CookieInit },
	// checkoutdlg.cpp:45
	{ "money_left_cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCheckoutDlg::OnMoneyLeft_CookieInit },
	// checkoutdlg.cpp:46
	{ "num_static", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCheckoutDlg::OnNumStaticInit },
	// checkoutdlg.cpp:48
	{ "explane", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCheckoutDlg::OnExplaneStaticInit },
	// checkoutdlg.cpp:50
	{ "num", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCheckoutDlg::OnNumInit },
	// checkoutdlg.cpp:51
	{ "num", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&FrCheckoutDlg::OnNumEnterKey },
	// checkoutdlg.cpp:52
	{ "num_prev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCheckoutDlg::OnNumPrevInit },
	// checkoutdlg.cpp:53
	{ "num_prev", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCheckoutDlg::OnNumPrevLBtnUp },
	// checkoutdlg.cpp:54
	{ "num_next", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCheckoutDlg::OnNumNextInit },
	// checkoutdlg.cpp:55
	{ "num_next", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCheckoutDlg::OnNumNextLBtnUp },
	// checkoutdlg.cpp:56
	{ "days", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCheckoutDlg::OnDaysInit },
	// checkoutdlg.cpp:57
	{ "days2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCheckoutDlg::OnDays2Init },
	// checkoutdlg.cpp:58
	{ "days", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrCheckoutDlg::OnDaysBtnDown },
	// checkoutdlg.cpp:59
	{ "days2", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrCheckoutDlg::OnDays2BtnDown },
	// checkoutdlg.cpp:82
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// checkoutdlg.cpp:84
// A rental period offered by the shop (8 bytes).
struct days_t {
	const char* const m_name;   // 0x0
	const int m_days;           // 0x4
};

days_t days[] = {
	{ "1\xc0\xcf", 1 },
	{ "7\xc0\xcf", 7 },
	{ "15\xc0\xcf", 15 },
	{ "30\xc0\xcf", 30 },
	{ "365\xc0\xcf", 365 },
	{ "\xb9\xab\xc1\xa6\xc7\xd1", 0 },
};

// checkoutdlg.cpp:96
FrCheckoutDlg::FrCheckoutDlg(void) {
	m_pCheckout = 0;
	m_pCart = 0;
	m_pPrice = 0;
	m_pMyMoney = 0;
	m_pMoneyLeft = 0;
	m_pPrice_Cookie = 0;
	m_pMyMoney_Cookie = 0;
	m_pMoneyLeft_Cookie = 0;
	m_pNumStatic = 0;

	m_pExplaneStatic = 0;

	m_pNum = 0;
	m_pNumPrev = 0;
	m_pNumNext = 0;

	m_flag16a = false;
	m_bUnderLevel = false;
	m_bOverLevel = false;
	m_bEnoughPang = false;
	m_bEnoughCookie = false;
	m_bCheckoutFlag = false;
	m_value184 = 0;

	m_num = 1;

	m_pang = 0;
	m_cookie = 0;

	m_totalCookie = 0;
	m_bTotalCookieSet = false;

	m_coupons.clear();
	m_bCouponFlag = true;
	m_selectedCoupon = -1;
	m_value1c0 = 0;

	m_pangDiscount = 0;
	m_cookieDiscount = 0;

	m_value1d8 = 0;

	m_dayNameCount = 0;
	memset(m_dayNames, 0, sizeof(m_dayNames));

	m_miniBtn[0].m_pCartItem = 0;
	m_miniBtn[1].m_pCartItem = 0;
	m_flag1dc = false;
}

// checkoutdlg.cpp:167
std::list<FrCheckoutDlg::CartItem>& FrCheckoutDlg::Cart(void) {
	return m_cart;
}

// checkoutdlg.cpp:172
void FrCheckoutDlg::Process(void) {
	if (m_pNum) {
		const char* text = m_pNum->GetLine(1, false);
		int num = atoi(text);

		if (num != m_num) {
			SetPrice(num);
		}
	}
}

// checkoutdlg.cpp:189
void FrCheckoutDlg::RecalculateTotalCookie(__int64 pang, __int64 cookie, bool update) {
	m_unitPang = 0;
	m_unitCookie = 0;

	for (std::list<CartItem>::iterator it = m_cart.begin(); it != m_cart.end(); ++it) {
		CartItem& item = *it;
		__int64 price = 0xffffffff;

		if (m_bCheckoutFlag && (item.m_pItem->m_typeId & 0xFC000000) == 0x08000000) {
			IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(item.m_pItem->m_typeId);
			if (pPart && pPart->m_rentalPrice > 0) {
				m_unitPang += pPart->m_rentalPrice;
			}
		} else if ((item.m_pItem->m_typeId >> 26) == 0xE) {
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(item.m_pItem->m_typeId);

			unsigned long subType = (item.m_pItem->m_typeId >> 22) & 0xF;
			if (subType == 5) {
				if (item.m_pItem->m_discountPrice > 0 && item.m_pItem->m_discountPrice < item.m_pItem->m_price)
					price = item.m_pItem->m_discountPrice;
				else
					price = item.m_pItem->m_price;

				if (item.m_pItem->m_flags0 & 1)
					m_unitCookie += price;
				else
					m_unitPang += price;
			} else if (subType == 6) {
				if (item.m_pItem->m_discountPrice > 0 && item.m_pItem->m_discountPrice < item.m_pItem->m_price)
					price = item.m_pItem->m_discountPrice;
				else
					price = item.m_pItem->m_price;

				if (item.m_pItem->m_flags0 & 1)
					m_unitCookie += price;
				else
					m_unitPang += price;
			} else {
				if (pSkin->m_flags & 1)
					m_unitCookie += pSkin->m_periodPrices[item.m_days];
				else
					m_unitPang += pSkin->m_periodPrices[item.m_days];
			}
		} else if ((item.m_pItem->m_typeId >> 26) == 8) {
			IFF_STRUCT::sCadItem* pCadItem = ItemManager()->FindCadItem(item.m_pItem->m_typeId);

			if (pCadItem->m_flags & 1)
				m_unitCookie += pCadItem->m_levels[item.m_days];
			else
				m_unitPang += pCadItem->m_levels[item.m_days];
		} else if ((item.m_pItem->m_typeId >> 26) == 0x10) {
			IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(item.m_pItem->m_typeId);

			if (pMascot->m_flags & 1)
				m_unitCookie += pMascot->m_periodPrices[item.m_days];
			else
				m_unitPang += pMascot->m_periodPrices[item.m_days];
		} else if (m_dayNameCount > 0) {
			if (m_dayNames[item.m_days].altPeriodUnit)
				price = m_dayNames[item.m_days].altPrice;
			else if (m_dayNames[item.m_days].hasPrice)
				price = m_dayNames[item.m_days].price;

			if (m_dayNames[item.m_days].isCash)
				m_unitCookie += price;
			else
				m_unitPang += price;
		} else {
			if (item.m_pItem->m_discountPrice > 0 && item.m_pItem->m_discountPrice < item.m_pItem->m_price)
				price = item.m_pItem->m_discountPrice;
			else
				price = item.m_pItem->m_price;

			if (item.m_pItem->m_flags0 & 1)
				m_unitCookie += price;
			else
				m_unitPang += price;
		}
	}

	if (update) {
		char text[32];
		bool bPang = false;
		bool bCash = false;

		if (m_pPrice) {
			__int64 price = m_unitPang - m_pangDiscount;
			MakePriceStrA(text, price < 0 ? 0 : price);
			strcat(text, "\xc6\xce");
			m_pPrice->SetLine(1, text, 0, false, 0);

			if (m_unitPang != 0) {
				bPang = true;
			}
		}

		if (m_pPrice_Cookie) {
			__int64 price = m_unitCookie - m_cookieDiscount;
			sprintf(text, "%I64d\xc4\xed\xc5\xb0", price < 0 ? 0 : price);
			m_pPrice_Cookie->SetLine(1, text, 0, false, 0);

			if (price != 0) {
				bCash = true;
				m_totalCookie = WSingleton<CSharedDoc>::Instance()->RemainOwnCash(WSingleton<CSharedDoc>::Instance()->m_billingType, price);
			}
		}

		if (bPang == true && bCash == false) {
			m_bTotalCookieSet = false;
			m_totalCookie = 0;
		} else {
			m_bTotalCookieSet = true;
		}

		if (m_pMyMoney) {
			MakePriceStrA(text, pang);
			strcat(text, "\xc6\xce");
			m_pMyMoney->SetLine(1, text, 0, false, 0);
		}

		if (m_pMyMoney_Cookie) {
			sprintf(text, "%I64d\xc4\xed\xc5\xb0", cookie);
			m_pMyMoney_Cookie->SetLine(1, text, 0, false, 0);
		}
	}

	SetPrice(1);
}

// checkoutdlg.cpp:402
void FrCheckoutDlg::Checkout(__int64 pang, __int64 cookie, bool b) {
	m_bCheckoutFlag = b;
	m_flag16a = false;
	m_bUnderLevel = false;
	m_bOverLevel = false;
	m_bEnoughPang = false;
	m_bEnoughCookie = false;
	m_dayNameCount = 0;
	memset(m_dayNames, 0, sizeof(m_dayNames));

	if (m_pCart) {
		m_pCart->ClearItem();

		for (std::list<CartItem>::iterator it = m_cart.begin(); it != m_cart.end(); ++it) {
			CartItem& item = *it;
			FixCartItem(item);
			m_pCart->AddItem(&item);

			if (item.m_pItem->m_flags0 & 0xE0) {
				if ((item.m_pItem->m_level >> 7) && (item.m_pItem->m_level & 0x7F) < WSingleton<CSharedDoc>::Instance()->m_myLevel) {
					m_bOverLevel = true;
				} else if (!(item.m_pItem->m_level >> 7) && (item.m_pItem->m_level & 0x7F) > WSingleton<CSharedDoc>::Instance()->m_myLevel) {
					m_bUnderLevel = true;
				}
			}

			if (item.m_option) {
				m_flag16a = true;
			}
		}
	}

	m_miniBtn[0].m_bChanged = true;
	m_miniBtn[0].m_pDays->SetVisible(false);
	m_miniBtn[1].m_bChanged = true;
	m_miniBtn[1].m_pDays->SetVisible(false);

	m_pang = pang;
	m_cookie = cookie;
	RecalculateTotalCookie(pang, cookie, true);

	std::list<CartItem>::iterator first = m_cart.begin();

	bool bQuantity = false;
	if ((first->m_pItem->m_typeId & 0xFC000000) == 0x18000000 && !(first->m_pItem->m_typeId & 0x2000000)) {
		IFF_STRUCT::sItem* pItem = ItemManager()->FindItem(first->m_pItem->m_typeId);

		if (pItem->quantity > 0) {
			bQuantity = true;
			if (m_pNum) {
				m_pNum->SetLine(1, MakeStr("%d", pItem->quantity), 0, false, 0);
			}
		}
	}

	if (((first->m_pItem->m_typeId & 0x2000000) || (first->m_pItem->m_typeId & 0xFC000000) != 0x18000000 || bQuantity || m_cart.size() > 1)
		&& first->m_pItem->m_typeId != 0x1A000041 && first->m_pItem->m_typeId != 0x1A0000CC) {
		if (m_pNumStatic) {
			m_pNumStatic->SetVisible(false);
		}

		if (m_pNum) {
			m_pNum->SetVisible(false);
		}

		if (m_pNumPrev) {
			m_pNumPrev->SetVisible(false);
		}

		if (m_pNumNext) {
			m_pNumNext->SetVisible(false);
		}
	} else {
		if (m_pNum) {
			WRect rect = m_pNum->GetRect();
			m_pNum->MoveWindow(WPoint(rect.x + ((first->m_pItem->m_flags0 & 1) ? 50 : -50), rect.y));
		}

		if (m_pNumPrev) {
			WRect rect = m_pNumPrev->GetRect();
			m_pNumPrev->MoveWindow(WPoint(rect.x + ((first->m_pItem->m_flags0 & 1) ? 50 : -50), rect.y));
		}

		if (m_pNumNext) {
			WRect rect = m_pNumNext->GetRect();
			m_pNumNext->MoveWindow(WPoint(rect.x + ((first->m_pItem->m_flags0 & 1) ? 50 : -50), rect.y));
		}
	}
}

// checkoutdlg.cpp:524
void FrCheckoutDlg::FixCartItem(CartItem& item) {
	switch (item.m_pItem->m_typeId >> 26) {
	case 2: {
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(item.m_pItem->m_typeId);
		if (pPart && m_bCheckoutFlag) {
			if (pPart->m_rentalPrice > 0) {
				item.m_days = 7;
			}
		}
		break;
	}

	case 0xE: {
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(item.m_pItem->m_typeId);

		if (pSkin && !S5::ISCutinItem(item.m_pItem->m_typeId)) {
			for (int i = 0; i < 5; i++) {
				if (pSkin->m_periodPrices[i] != 0) {
					item.m_days = i;
					break;
				}
			}
		}
		break;
	}

	case 8: {
		IFF_STRUCT::sCadItem* pCadItem = ItemManager()->FindCadItem(item.m_pItem->m_typeId);
		if (pCadItem) {
			for (int i = 0; i < 5; i++) {
				if (pCadItem->m_levels[i] != 0) {
					item.m_days = i;
					break;
				}
			}
		}
		break;
	}

	case 0x10: {
		IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(item.m_pItem->m_typeId);
		if (pMascot) {
			for (int i = 0; i < 5; i++) {
				if (pMascot->m_periodPrices[i] != 0) {
					item.m_days = i;
					break;
				}
			}
		}
		break;
	}

	default:
		m_dayNameCount = ItemManager()->FindChildItemNumber(item.m_pItem->m_typeId);
		if (m_dayNameCount > 0) {
			memset(m_dayNames, 0, sizeof(m_dayNames));

			if (m_dayNameCount > 5) {
				m_dayNameCount = 5;
			}

			for (int i = 0; i < m_dayNameCount; i++) {
				IFF_STRUCT::sChildItem* pChild = ItemManager()->FindChildItem(item.m_pItem->m_typeId, i);
				if (pChild == 0 || !pChild->m_enabled)
					continue;

				if (pChild->m_hasPrice) {
					sprintf(m_dayNames[i].name, "%d\xb0\xb3", pChild->m_quantity);
					m_dayNames[i].isCash = pChild->isCash;
					m_dayNames[i].price = pChild->m_price;
					m_dayNames[i].hasPrice = pChild->m_hasPrice;
					m_dayNames[i].quantity = pChild->m_quantity;
				} else if (pChild->m_altPeriodUnit) {
					switch (pChild->m_altPeriodUnit) {
					case 0: break;
					case 3: sprintf(m_dayNames[i].name, "%d\xbd\xc3\xb0\xa3", pChild->m_altPeriod); break;
					case 4: sprintf(m_dayNames[i].name, "%d\xc0\xcf", pChild->m_altPeriod); break;
					case 5: sprintf(m_dayNames[i].name, "%d\xb0\xb3\xbf\xf9", pChild->m_altPeriod); break;
					default: sprintf(m_dayNames[i].name, "%d\xc0\xcf", pChild->m_altPeriod); break;
					}

					m_dayNames[i].isCash = pChild->isCash;
					m_dayNames[i].altPrice = pChild->m_altPrice;
					m_dayNames[i].altPeriodUnit = pChild->m_altPeriodUnit;
					m_dayNames[i].altPeriod = pChild->m_altPeriod;
				}
			}
		}
		break;
	}
}

// checkoutdlg.cpp:641
void FrCheckoutDlg::SetPrice(int num) {
	char text[32];

	m_bEnoughPang = true;
	m_bEnoughCookie = true;

	m_num = num < 1 ? 1 : (num > 99 ? 99 : num);

	if (m_pPrice && m_pMyMoney && m_unitPang != 0) {
		__int64 price = m_num * m_unitPang - m_pangDiscount;

		MakePriceStrA(text, price < 0 ? 0 : price);
		strcat(text, "\xc6\xce");
		m_pPrice->SetLine(1, text, 0, false, 0);

		__int64 money = WSingleton<CSharedDoc>::Instance()->m_myPang;

		if (money - (m_num * m_unitPang - m_pangDiscount) < 0) {
			if (m_pMoneyLeft) {
				MakePriceStrA(text, m_num * m_unitPang - m_pangDiscount - money);
				strcat(text, "\xc6\xce \xba\xce\xc1\xb7");
				m_pMoneyLeft->SetLine(1, text, 0xFFFF0000, false, 0);
			}

			m_bEnoughPang = false;
		} else if (m_pMoneyLeft) {
			__int64 total = m_num * m_unitPang;
			__int64 discount = m_pangDiscount;
			if (discount > total) {
				discount = total;
			}

			MakePriceStrA(text, money - (total - discount));
			strcat(text, "\xc6\xce");
			m_pMoneyLeft->SetLine(1, text, 0, false, 0);
		}
	} else if (m_pMoneyLeft) {
		m_pMoneyLeft->SetLine(1, m_pMyMoney->GetLine(1, false), 0, false, 0);
	}

	if (m_pPrice_Cookie && m_pMyMoney_Cookie) {
		__int64 price = m_unitCookie - m_cookieDiscount;

		sprintf(text, "%I64d\xc4\xed\xc5\xb0", price < 0 ? 0 : price);
		m_pPrice_Cookie->SetLine(1, text, 0, false, 0);

		__int64 money = WSingleton<CSharedDoc>::Instance()->m_myCash;

		if (money - (m_unitCookie - m_cookieDiscount) < 0) {
			if (m_pMoneyLeft_Cookie) {
				MakePriceStrA(text, m_unitCookie - m_cookieDiscount - money);
				strcat(text, "\xc4\xed\xc5\xb0 \xba\xce\xc1\xb7");
				m_pMoneyLeft_Cookie->SetLine(1, text, 0xFFFF0000, false, 0);
			}

			m_bEnoughCookie = false;
		} else if (m_pMoneyLeft_Cookie) {
			__int64 discount = m_cookieDiscount;
			if (discount > m_unitCookie) {
				discount = m_unitCookie;
			}

			sprintf(text, "%d\xc4\xed\xc5\xb0", money - (m_unitCookie - discount));
			m_pMoneyLeft_Cookie->SetLine(1, text, 0, false, 0);
		}
	} else if (m_pMoneyLeft_Cookie) {
		m_pMoneyLeft_Cookie->SetLine(1, m_pMyMoney_Cookie->GetLine(1, false), 0, false, 0);
	}

	if (m_pNum) {
		m_pNum->SetLine(1, MakeStr("%d", m_num), 0, false, 0);
	}
}

// checkoutdlg.cpp:744
bool FrCheckoutDlg::OnChargeResult(int ret, FrForm*) {
	if (ret == FrOK) {
		WSingleton<CBrowser>::Instance()->InitializePostArgument();

		switch (WSingleton<CSharedDoc>::Instance()->m_billingType) {
		case 2:
			WSingleton<CBrowser>::Instance()->AddPostArgument("sn", "9");
			WSingleton<CBrowser>::Instance()->AddPostArgument("mn", MakeStr("%d", LOGININFO()->AuthUid()));
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

		Close(FrOK, true);
	}

	return true;
}

// One item of the buy request packet 0x1D as OnConfirmResult fills it (0x10
// bytes, the layout of sBuyItem, globalnetworkdefine.h). Not sBuyItem itself:
// the original zeroes it in line (checkoutdlg.cpp:864) with no line of
// sBuyItem's constructor, so its type has no constructor. Name provisional.
struct sBuyRequestItem {
	int m_uid;                  // 0x0, -1 for a new item
	unsigned long m_typeId;     // 0x4
	unsigned short m_period;    // 0x8, rental period (sPeriodPrice::altPeriod or days_t::m_days)
	short m_periodIndex;        // 0xa, index of the rental period, -1 for none
	int m_count;                // 0xc
};

// checkoutdlg.cpp:857
bool FrCheckoutDlg::OnConfirmResult(int ret, FrForm*) {
	if (ret == FrOK && !m_flag1dc) {
		FrListBox::ITEM_LIST& items = m_pCart->m_itemList;

		const char* text = m_pNum->GetLine(1, false);
		int count = atoi(text);
		sBuyRequestItem buy;
		memset(&buy, 0, sizeof(buy));
		buy.m_uid = -1;

		FrListBox::ITEM_LIST::iterator it = items.begin();
		CartItem& item = *(CartItem*)(*it)->pData;

		if (IsLocalContent((localContentType_t)0x17) && WSingleton<CSharedDoc>::Instance()->IsHaveHalloweenItem()
			&& (item.m_pItem->m_typeId & 0xFC000000) == 0x8000000 && (item.m_pItem->m_typeId & 0x1800) == 0x1800) {
			// "This item was already bought."
			AfxGetTask()->GetActor("Shop") << MsgObject(0, 0x23, (intptr_t)"\xC0\xCC\xB9\xCC \xB1\xB8\xB8\xC5\xC7\xD1 \xB9\xB0\xB0\xC7\xC0\xD4\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
			return true;
		}

		if (m_dayNameCount > 0) {
			if ((item.m_pItem->m_typeId & 0xFC000000) == 0x18000000) {
				WSendPacket packet((enumClientPacket)0x1D);

				ItemManager()->FindItem(item.m_pItem->m_typeId);

				if (m_dayNames[item.m_days].hasPrice) {
					count = m_dayNames[item.m_days].quantity;
				}

				buy.m_typeId = item.m_pItem->m_typeId;
				buy.m_period = m_dayNames[item.m_days].altPeriod;
				buy.m_periodIndex = (short)item.m_days;
				buy.m_count = count;

				packet.Encode1(0);
				packet.Encode2(1);
				packet.EncodeBuffer(&buy, sizeof(buy));
				packet.Send(SEND_TO_DEFAULT);
			} else {
				int n = items.size();
				WSendPacket packet((enumClientPacket)0x1D);

				packet.Encode1(0);
				packet.Encode2(n);

				for (it = items.begin(); it != items.end(); ++it) {
					item = *(CartItem*)(*it)->pData;

					buy.m_typeId = item.m_pItem->m_typeId;
					buy.m_period = m_dayNames[item.m_days].altPeriod;
					buy.m_periodIndex = (short)item.m_days;
					buy.m_count = count;

					packet.EncodeBuffer(&buy, sizeof(buy));
				}

				packet.Send(SEND_TO_DEFAULT);
			}
		} else {
			unsigned long typeId = item.m_pItem->m_typeId;
			if (typeId >> 26 == 5) {
				IFF_STRUCT::sBall* pBall = ItemManager()->FindBall(typeId);
				if (pBall) {
					count = pBall->quantity;
				}
			} else if (typeId >> 26 == 6 && (typeId & 0x2000000) && typeId != 0x1A000041 && typeId != 0x1A0000CC) {
				IFF_STRUCT::sItem* pItem = ItemManager()->FindItem(typeId);
				if (pItem) {
					count = pItem->quantity;
				}
			}

			if (!WSingleton<CSharedDoc>::Instance()->ConfirmInsertItem(item.m_pItem->m_typeId, count)) {
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				WRect rect = pForm->GetRect();
				pForm->SetRect(WRect(rect.x, rect.y, rect.w, rect.h));
				pForm->SetTimeLimit(12.0f);
				// "You already hold 20,000 of this item, so it cannot be bought or moved."
				pForm->SetMessage("\xC7\xF6\xC0\xE7 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xC7 \xBA\xB8\xC0\xAF\xB0\xB3\xBC\xF6\xB0\xA1 20,000\xB0\xB3\xC0\xCC\xB9\xC7\xB7\xCE \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xB1\xB8\xC0\xD4 \xB6\xC7\xB4\xC2 \xBF\xC5\xB1\xE6 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
				pForm->Open((FRESH_RESULT_CALLBACK)0, 0);
				return true;
			}

			if (m_bCheckoutFlag) {
				for (it = items.begin(); it != items.end(); ++it) {
					CartItem* pCartItem = (CartItem*)(*it)->pData;
					if (!S5::RENTALPARTS::IsRentableParts(pCartItem->m_pItem->m_typeId)) {
						// "Purchase error. Please start again from the beginning."
						MsgObject msg(0, 0x23, (intptr_t)"\xB1\xB8\xB8\xC5 \xC3\xB3\xB8\xAE \xBF\xC0\xB7\xF9. \xC3\xB3\xC0\xBD\xBA\xCE\xC5\xCD \xB4\xD9\xBD\xC3 \xBD\xC3\xC0\xDB\xC7\xD8 \xC1\xD6\xBC\xBC\xBF\xE4.", 0, 0, 0, 0);
						(!CTaskManager::Instance() ? 0 : CTaskManager::Instance()->GetCurrentTask())->GetActor("Shop") << msg;
						return true;
					}
				}
			}

			if (items.size() > 1) {
				std::list<unsigned long> typeIds;
				for (it = items.begin(); it != items.end(); ++it) {
					CartItem* pCartItem = (CartItem*)(*it)->pData;
					unsigned long typeId = pCartItem->m_pItem->m_typeId;
					typeIds.push_back(typeId);
				}

				typeIds.sort();
				typeIds.unique();

				if (typeIds.size() != items.size()) {
					FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
					WRect rect = pForm->GetRect();
					pForm->SetRect(WRect(rect.x, rect.y, rect.w, rect.h));
					pForm->SetTimeLimit(12.0f);
					// "A problem occurred while buying the cart, the items cannot be bought. Please buy them one by one."
					pForm->SetMessage("\xC4\xAB\xC6\xAE \xB1\xB8\xB8\xC5 \xB0\xFA\xC1\xA4\xBF\xA1 \xB9\xAE\xC1\xA6\xB0\xA1 \xB9\xDF\xBB\xFD\xC7\xCF\xBF\xA9 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xBB\xEC \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9. \xB4\xDC\xC7\xB0\xC0\xB8\xB7\xCE \xB1\xB8\xB8\xC5\xB8\xA6 \xBD\xC3\xB5\xB5\xC7\xD8 \xC1\xD6\xBC\xBC\xBF\xE4.", false);
					pForm->Open((FRESH_RESULT_CALLBACK)0, 0);
					return true;
				}
			}

			int n = items.size();
			WSendPacket packet((enumClientPacket)0x1D);

			packet.Encode1(m_bCheckoutFlag);
			packet.Encode2(n);

			for (it = items.begin(); it != items.end(); ++it) {
				CartItem* pCartItem = (CartItem*)(*it)->pData;

				buy.m_typeId = pCartItem->m_pItem->m_typeId;
				buy.m_period = (unsigned short)days[pCartItem->m_days].m_days;
				buy.m_periodIndex = -1;
				buy.m_count = count;

				packet.EncodeBuffer(&buy, sizeof(buy));
			}

			packet.Send(SEND_TO_DEFAULT);
		}

		m_flag1dc = true;

		MsgObject msg(0, 0xC0, 0, 0, 0, 0, 0);
		(!CTaskManager::Instance() ? 0 : CTaskManager::Instance()->GetCurrentTask())->GetActor("Shop") << msg;
		MsgObject msg2(0, 1, 0, 0, 0, 0, 0);
		(!CTaskManager::Instance() ? 0 : CTaskManager::Instance()->GetCurrentTask())->GetActor("Shop") << msg2;
		Close(FrOK, true);
	}

	return true;
}

// checkoutdlg.cpp:1085
bool CompareUsableCoupon(const FrCheckoutDlg::sCoupon& a, const FrCheckoutDlg::sCoupon& b) {
	return a.m_bUsable > b.m_bUsable;
}

// checkoutdlg.cpp:1091
bool ComparePeriodCoupon(const FrCheckoutDlg::sCoupon& a, const FrCheckoutDlg::sCoupon& b) {
	return a.m_period < b.m_period;
}

// checkoutdlg.cpp:1775
unsigned long FrCheckoutDlg::GetGuidSelectedCoupon(void) {
	m_bCouponFlag = false;

	return 0;
}

// checkoutdlg.cpp:1802
void FrCheckoutDlg::OnCheckoutInit(intptr_t param) {
	m_pCheckout = DYNAMIC_CAST(FrButton, param);

	m_pClockIcon = g_pFresh->GetManager()->GetBitmap("ICONS", "clock");
	m_clockRect = WRect(40.0f, 35.0f, m_pClockIcon->Width(), m_pClockIcon->Height());
}

extern TitleManager* g_TitleManager;   // defined in ProjectG.cpp

// checkoutdlg.cpp:1812
void FrCheckoutDlg::OnCheckoutLBtnUp(void) {
	if (IsUnderLevel() || IsOverLevel()) {
		// "You cannot buy it at your current level"
		MsgObject msg(0, 0x23, (intptr_t)"\xC7\xF6\xC0\xE7 \xB7\xB9\xBA\xA7\xB7\xCE\xB4\xC2 \xB1\xB8\xC0\xD4\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
		(!CTaskManager::Instance() ? 0 : CTaskManager::Instance()->GetCurrentTask())->GetActor("Shop") << msg;
		return;
	}

	int ownedCount = WSingleton<CSharedDoc>::Instance()->m_myItemMap.size() + WSingleton<CSharedDoc>::Instance()->m_myPartsList.size();

	FrListBox::ITEM_LIST& items = m_pCart->m_itemList;
	unsigned long characterTypeId = 0;
	unsigned long hairStyleTypeId = 0;
	for (FrListBox::ITEM_LIST::iterator it = items.begin(); it != items.end(); ++it) {
		CartItem* pCartItem = (CartItem*)(*it)->pData;
		if (ownedCount > 1000) {
			switch (pCartItem->m_pItem->m_typeId >> 26) {
			case 2:
			case 4:
				// "You cannot buy any more items."
				MsgObject msg(0, 0x23, (intptr_t)"\xB4\xF5 \xC0\xCC\xBB\xF3 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xB1\xB8\xB8\xC5\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
				(!CTaskManager::Instance() ? 0 : CTaskManager::Instance()->GetCurrentTask())->GetActor("Shop") << msg;
				return;
			}
		}

		if ((pCartItem->m_pItem->m_typeId & 0xFC000000) == 0x20000000) {
			IFF_STRUCT::sCadItem* pCadItem = ItemManager()->FindCadItem(pCartItem->m_pItem->m_typeId);

			bool found = false;
			std::map<unsigned int, sCaddieInfo>::const_iterator end = WSingleton<CSharedDoc>::Instance()->m_caddieMap.end();
			for (std::map<unsigned int, sCaddieInfo>::const_iterator caddie = WSingleton<CSharedDoc>::Instance()->m_caddieMap.begin(); caddie != end; ++caddie) {
				const sCaddieInfo& info = caddie->second;
				if (info.typeId == (((pCadItem->m_typeId >> 21) & 0x1F) | 0x1C000000)) {
					IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(info.typeId);
					if ((info.flags & 2) && info.days == 0 && pCaddie && pCaddie->fee > 0) {
						OpenCaddieInfo(info);
						return;
					}
					found = true;
					break;
				}
			}

			if (!found) {
				// "You do not have a caddie to decorate."
				MsgObject msg(0, 0x23, (intptr_t)"\xB2\xD9\xB9\xCC\xB1\xE2\xC7\xD2 \xC4\xB3\xB5\xF0\xB8\xA6 \xB0\xA1\xC1\xF6\xB0\xED \xC0\xD6\xC1\xF6 \xBE\xCA\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
				(!CTaskManager::Instance() ? 0 : CTaskManager::Instance()->GetCurrentTask())->GetActor("Shop") << msg;
				return;
			}
		}

		if ((pCartItem->m_pItem->m_typeId & 0xFC000000) == 0x38000000 && (pCartItem->m_pItem->m_typeId & 0x3C00000) == 0x1800000) {
			Title* pTitle = g_TitleManager->GetTitle((eTitle)(pCartItem->m_pItem->m_typeId & 0x3FFFFF));
			if (pTitle) {
				if (!pTitle->CanEquip()) {
					// "You do not meet the purchase conditions"
					MsgObject msg(0, 0x23, (intptr_t)"\xB1\xB8\xC0\xD4\xB0\xA1\xB4\xC9\xC7\xD1 \xC1\xB6\xB0\xC7\xC0\xBB \xB0\xAE\xC3\xDF\xC1\xF6 \xBE\xCA\xBE\xD2\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
					(!CTaskManager::Instance() ? 0 : CTaskManager::Instance()->GetCurrentTask())->GetActor("Shop") << msg;
					return;
				}
			}
		}

		if (pCartItem->m_option) {
			unsigned long type = pCartItem->m_pItem->m_typeId >> 26;
			if (type == 6 || type == 5 || (type == 9 && (((pCartItem->m_pItem->m_typeId >> 21) & 0x1F) == 6 || ((pCartItem->m_pItem->m_typeId >> 21) & 0x1F) == 5))) {
				if (pCartItem->m_option < 100000) {
					if (m_dayNameCount > 0) {
						if (m_dayNames[pCartItem->m_days].altPeriodUnit != 0) {
							// "You cannot buy an item you already have"
							MsgObject msg(0, 0x23, (intptr_t)"\xC0\xCC\xB9\xCC \xB0\xA1\xC1\xF6\xB0\xED \xC0\xD6\xB4\xC2 \xB9\xB0\xB0\xC7\xC0\xBB \xB1\xB8\xC0\xD4\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
							(!CTaskManager::Instance() ? 0 : CTaskManager::Instance()->GetCurrentTask())->GetActor("Shop") << msg;
							return;
						}
					} else if (ItemManager()->IsDisableDuplicateItem(pCartItem->m_pItem->m_typeId)) {
						MsgObject msg(0, 0x23, (intptr_t)"\xC0\xCC\xB9\xCC \xB0\xA1\xC1\xF6\xB0\xED \xC0\xD6\xB4\xC2 \xB9\xB0\xB0\xC7\xC0\xBB \xB1\xB8\xC0\xD4\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
						(!CTaskManager::Instance() ? 0 : CTaskManager::Instance()->GetCurrentTask())->GetActor("Shop") << msg;
						return;
					}
				} else {
					MsgObject msg(0, 0x23, (intptr_t)"\xC0\xCC\xB9\xCC \xB0\xA1\xC1\xF6\xB0\xED \xC0\xD6\xB4\xC2 \xB9\xB0\xB0\xC7\xC0\xBB \xB1\xB8\xC0\xD4\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
					(!CTaskManager::Instance() ? 0 : CTaskManager::Instance()->GetCurrentTask())->GetActor("Shop") << msg;
					return;
				}
			} else {
				switch (pCartItem->m_pItem->m_typeId >> 26) {
				case 2:
					if (pCartItem->m_pItem->m_flags0 & 1) {
						if (m_bCheckoutFlag) {
							// "Clothes already rented cannot be rented again."
							AfxGetTask()->GetActor("Shop") << MsgObject(0, 0x23, (intptr_t)"\xC0\xCC\xB9\xCC \xB4\xEB\xBF\xA9\xC1\xDF\xC0\xCE \xC0\xC7\xBB\xF3\xC0\xBA \xC3\xDF\xB0\xA1\xB7\xCE \xB4\xEB\xBF\xA9\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
							return;
						}
						// "Cookie clothes you already have or rent cannot be bought"
						AfxGetTask()->GetActor("Shop") << MsgObject(0, 0x23, (intptr_t)"\xC4\xED\xC5\xB0 \xC0\xC7\xBB\xF3\xC0\xBA \xC0\xCC\xB9\xCC \xB0\xA1\xC1\xF6\xB0\xED \xC0\xD6\xB0\xC5\xB3\xAA \xB4\xEB\xBF\xA9\xC1\xDF\xBF\xA1\xB4\xC2 \xB1\xB8\xC0\xD4\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
						return;
					}
					m_value184 = 4;
					break;

				case 15:
					break;

				default:
					MsgObject msg(0, 0x23, (intptr_t)"\xC0\xCC\xB9\xCC \xB0\xA1\xC1\xF6\xB0\xED \xC0\xD6\xB4\xC2 \xB9\xB0\xB0\xC7\xC0\xBB \xB1\xB8\xC0\xD4\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
					(!CTaskManager::Instance() ? 0 : CTaskManager::Instance()->GetCurrentTask())->GetActor("Shop") << msg;
					return;
				}
			}
		}

		unsigned long typeId = pCartItem->m_pItem->m_typeId;
		if ((typeId >> 26) == 15) {
			IFF_STRUCT::sHairStyle* pHairStyle = ItemManager()->FindHairStyle(typeId);
			m_value184 = 1;
			for (std::map<unsigned int, sCharacterInfo>::const_iterator character = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin(); character != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++character) {
				if ((character->second.typeId & 0x3FFFFFF) == pHairStyle->m_style && character->second.hairColor == pHairStyle->m_color) {
					// "This character already has the hair color of the selected dye"
					MsgObject msg(0, 0x23, (intptr_t)"\xC7\xD8\xB4\xE7\xC4\xB3\xB8\xAF\xC5\xCD\xB4\xC2 \xBC\xB1\xC5\xC3\xC7\xD1 \xBF\xB0\xBB\xF6\xBE\xE0\xB0\xFA \xB0\xB0\xC0\xBA \xBB\xF6\xC0\xC7 \xBF\xB0\xBB\xF6\xC0\xBB \xC7\xCF\xB0\xED \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
					(!CTaskManager::Instance() ? 0 : CTaskManager::Instance()->GetCurrentTask())->GetActor("Shop") << msg;
					return;
				}
			}
		} else if ((typeId >> 26) == 8) {
			ItemManager()->FindCadItem(typeId);
			m_value184 = 2;
			for (std::map<unsigned int, sCaddieInfo>::const_iterator caddie = WSingleton<CSharedDoc>::Instance()->m_caddieMap.begin(); caddie != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end(); ++caddie) {
				if (caddie->second.typeId == (((pCartItem->m_pItem->m_typeId >> 21) & 0x1F) | 0x1C000000)) {
					m_value184 = 3;
				}

				if (caddie->second.hours >= 17520) {
					// "No more can be bought."
					MsgObject msg(0, 0x23, (intptr_t)"\xB4\xF5\xC0\xCC\xBB\xF3 \xB1\xB8\xC0\xD4\xC0\xCC \xBA\xD2\xB0\xA1\xB4\xC9\xC7\xD5\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
					(!CTaskManager::Instance() ? 0 : CTaskManager::Instance()->GetCurrentTask())->GetActor("Shop") << msg;
					return;
				}
			}
		} else if ((typeId >> 26) == 16) {
			for (std::map<unsigned int, sMascotInfo>::const_iterator mascot = WSingleton<CSharedDoc>::Instance()->m_myMascotMap.begin(); mascot != WSingleton<CSharedDoc>::Instance()->m_myMascotMap.end(); ++mascot) {
				if (mascot->second.typeId == typeId) {
					m_value184 = 3;
					if (mascot->second.remainHours >= 17520) {
						MsgObject msg(0, 0x23, (intptr_t)"\xB4\xF5\xC0\xCC\xBB\xF3 \xB1\xB8\xC0\xD4\xC0\xCC \xBA\xD2\xB0\xA1\xB4\xC9\xC7\xD5\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
						(!CTaskManager::Instance() ? 0 : CTaskManager::Instance()->GetCurrentTask())->GetActor("Shop") << msg;
						return;
					}
					break;
				}
			}
		}

		if (!ItemManager()->IsInSalePeriod(pCartItem->m_pItem->m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
			// "Please check the sale period."
			MsgObject msg(0, 0x23, (intptr_t)"\xC6\xC7\xB8\xC5\xB1\xE2\xB0\xA3\xC0\xBB \xC8\xAE\xC0\xCE\xC7\xD8 \xC1\xD6\xBC\xBC\xBF\xE4.", 0, 0, 0, 0);
			(!CTaskManager::Instance() ? 0 : CTaskManager::Instance()->GetCurrentTask())->GetActor("Shop") << msg;
			return;
		}

		if ((pCartItem->m_pItem->m_typeId >> 26) == 1) {
			characterTypeId = pCartItem->m_pItem->m_typeId;
		} else if ((pCartItem->m_pItem->m_typeId >> 26) == 15) {
			hairStyleTypeId = pCartItem->m_pItem->m_typeId;
		}
	}

	if (characterTypeId != 0 && hairStyleTypeId != 0) {
		// "A character and a hair dye cannot be bought together."
		MsgObject msg(0, 0x23, (intptr_t)"\xC4\xB3\xB8\xAF\xC5\xCD\xBF\xCD \xBF\xB0\xBB\xF6\xBE\xE0\xC0\xBA \xB5\xBF\xBD\xC3\xBF\xA1 \xB1\xB8\xB8\xC5\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
		(!CTaskManager::Instance() ? 0 : CTaskManager::Instance()->GetCurrentTask())->GetActor("Shop") << msg;
		return;
	}

	if (!m_bEnoughPang) {
		// "Not enough pang. Collect pang by playing games."
		MsgObject msg(0, 0x23, (intptr_t)"\xC6\xCE\xC0\xCC \xB8\xF0\xC0\xDA\xB6\xF8\xB4\xCF\xB4\xD9.\n\xB0\xD4\xC0\xD3\xBF\xA1 \xC2\xFC\xBF\xA9\xC7\xD8\xBC\xAD \xC6\xCE\xC0\xBB \xB8\xF0\xC0\xB8\xBC\xBC\xBF\xE4.", 0, 0, 0, 0);
		(!CTaskManager::Instance() ? 0 : CTaskManager::Instance()->GetCurrentTask())->GetActor("Shop") << msg;
		return;
	}

	if (!m_bEnoughCookie) {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_okcancel", 0);
		// "Not enough cookies. Charge cookies now?"
		pForm->SetMessage("\\c0xffff0000\\c\xC4\xED\xC5\xB0\\c0xff000000\\c\xB0\xA1 \xB8\xF0\xC0\xDA\xB6\xF8\xB4\xCF\xB4\xD9.\n\xC1\xF6\xB1\xDD \xC4\xED\xC5\xB0\xB8\xA6 \xC3\xE6\xC0\xFC\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);
		pForm->Open((FRESH_RESULT_CALLBACK)&FrCheckoutDlg::OnChargeResult, 3);
		return;
	}

	std::string message;

	if (m_bCouponFlag && m_selectedCoupon >= 0) {
		CartItem* pFirst = (CartItem*)(*m_pCart->m_itemList.begin())->pData;
		char price[16] = "";
		if (m_unitPang > 0) {
			__int64 total = 0;
			__int64 full = m_num * m_unitPang;
			if (m_pangDiscount < full) {
				total = full - m_pangDiscount;
			}
			sprintf(price, "%I64d\xC6\xCE", total);
		} else if (m_unitCookie > 0) {
			__int64 total = 0;
			if (m_cookieDiscount < m_unitCookie) {
				total = m_unitCookie - m_cookieDiscount;
			}
			sprintf(price, "%I64d\xC4\xED\xC5\xB0", total);
		} else {
			sprintf(price, "0\xC4\xED\xC5\xB0");
		}

		char text[128];
		// "You are buying %s and it costs %s. Really buy it?"
		sprintf(text, "\\c0xffff0000\\c%s\xC0\xBB(\xB8\xA6) \xB1\xB8\xC0\xD4\xC7\xCF\xBD\xC3\xB4\xC2\xB5\xA5 %s\xC0\xCC(\xB0\xA1) \xBC\xD2\xBF\xE4\xB5\xCB\xB4\xCF\xB4\xD9.\n\xC1\xA4\xB8\xBB \xB1\xB8\xC0\xD4\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", pFirst->m_pItem->m_name, price);
		message = text;
	} else {
		switch (m_value184) {
		case 1:
			message = "\\c0xffff0000\\c\xBF\xB0\xBB\xF6\xBE\xE0\xC0\xBA \xB1\xB8\xB8\xC5\xBF\xCD \xB5\xBF\xBD\xC3\xBF\xA1 \xBB\xE7\xBF\xEB\xC0\xCC \xB5\xCB\xB4\xCF\xB4\xD9.\n\xC7\xD8\xB4\xE7 \xC4\xB3\xB8\xAF\xC5\xCD\xC0\xC7 \xB8\xD3\xB8\xAE\xBB\xF6\xC0\xBB \xBC\xB1\xC5\xC3\xC7\xD1 \xBB\xF6\xC0\xB8\xB7\xCE \xBF\xB0\xBB\xF6\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?";
			break;

		case 2:
			message = "\\c0xffff0000\\c\xB1\xE2\xC1\xB8\xC0\xC7 \xC4\xB3\xB5\xF0\xB2\xD9\xB9\xCC\xB1\xE2 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xCC \xC0\xD6\xB4\xD9\xB8\xE9 \xB5\xA4\xBE\xEE\xBE\xB2\xB0\xD4 \xB5\xCB\xB4\xCF\xB4\xD9.\n \xB1\xB8\xC0\xD4\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?";
			break;

		case 3:
			message = "\\c0xffff0000\\c\xBC\xB1\xC5\xC3\xC7\xCF\xBD\xC5 \xC0\xCF\xBC\xF6\xB8\xB8\xC5\xAD \xB1\xE2\xB0\xA3\xC0\xCC \xBF\xAC\xC0\xE5\xB5\xCB\xB4\xCF\xB4\xD9.\n \xB1\xB8\xC0\xD4\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?";
			break;

		case 4:
			message = "\\c0xffff0000\\c\xC0\xCC\xB9\xCC \xB0\xA1\xC1\xF6\xB0\xED \xC0\xD6\xB4\xC2 \xC0\xC7\xBB\xF3\xC0\xD4\xB4\xCF\xB4\xD9.\n \xC1\xA4\xB8\xBB \xB1\xB8\xC0\xD4\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?";
			break;

		case 0:
			message = "\\c0xffff0000\\c\xC1\xA4\xB8\xBB \xB1\xB8\xC0\xD4\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?";
			break;

		default:
			message = "\\c0xffff0000\\c\xC1\xA4\xB8\xBB \xB1\xB8\xC0\xD4\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?";
			break;
		}
	}

	message += "\n\n\xB1\xB8\xC0\xD4\xC7\xCF\xBD\xC3\xB1\xE2 \xC0\xFC\xBF\xA1 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xC7 \xB1\xE2\xC7\xD1\xB0\xFA \xBC\xF6\xB7\xAE\xC0\xBB \xB2\xC0 \xC8\xAE\xC0\xCE\xC7\xCF\xBF\xA9 \xC1\xD6\xBD\xCA\xBD\xC3\xBF\xC0.\n\xC0\xCC \xBB\xF3\xC7\xB0\xC0\xBA \xC3\xBB\xBE\xE0\xC3\xB6\xC8\xB8(\xB1\xB8\xB8\xC5\xC3\xEB\xBC\xD2)\xB0\xA1 \xBA\xD2\xB0\xA1\xB4\xC9\xC7\xD1 \xBB\xF3\xC7\xB0\xC0\xD4\xB4\xCF\xB4\xD9.";

	FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_okcancel_big", 0);
	pForm->SetMessage(message.c_str(), false);
	pForm->Open((FRESH_RESULT_CALLBACK)&FrCheckoutDlg::OnConfirmResult, 3);
}

// checkoutdlg.cpp:2192
void FrCheckoutDlg::OpenCaddieInfo(const sCaddieInfo& info) {
	FrWnd* pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName("caddie_info");
	if (pWnd == 0) {
		// The parent goes through a type cast of a null window: its __rtti_obj store remains.
		FrCaddieInfoDlg* pDlg = CreateForm<FrCaddieInfoDlg>(g_pFresh->GetManager(), this, "caddie_info", DYNAMIC_CAST(FrWnd, 0));
		if (pDlg) {
			pDlg->SetCaddieInfo(info);
			pDlg->Open(0, 3);
		}
	}
}

// checkoutdlg.cpp:2206
void FrCheckoutDlg::OnDaysInit(intptr_t param) {
	m_miniBtn[0].m_pDays = DYNAMIC_CAST(FrComboBox, param);
}

// checkoutdlg.cpp:2211
void FrCheckoutDlg::OnDays2Init(PG_MSGARG param) {
	m_miniBtn[1].m_pDays = DYNAMIC_CAST(FrComboBox, param);
}

// checkoutdlg.cpp:2216
void FrCheckoutDlg::OnDaysBtnDown(void) {
	if (m_dayNameCount > 0) {
		const char* text = m_miniBtn[0].m_pDays->GetLine(1, false);

		for (int i = 0; i < m_dayNameCount; ++i) {
			if (_stricmp(m_dayNames[i].name, text) == 0) {
				m_miniBtn[0].m_pCartItem->m_days = i;
			}
		}
	} else {
		const char* text = m_miniBtn[0].m_pDays->GetLine(1, false);

		for (int i = 0; i < 5; ++i) {
			if (_stricmp(days[i].m_name, text) == 0) {
				m_miniBtn[0].m_pCartItem->m_days = i;
			}
		}
	}

	RecalculateTotalCookie(m_pang, m_cookie, true);
}

// checkoutdlg.cpp:2247
void FrCheckoutDlg::OnDays2BtnDown(void) {
	if (m_dayNameCount > 0) {
		const char* text = m_miniBtn[1].m_pDays->GetLine(1, false);

		for (int i = 0; i < m_dayNameCount; ++i) {
			if (_stricmp(m_dayNames[i].name, text) == 0) {
				m_miniBtn[1].m_pCartItem->m_days = i;
			}
		}
	} else {
		const char* text = m_miniBtn[1].m_pDays->GetLine(1, false);

		for (int i = 0; i < 5; ++i) {
			if (_stricmp(days[i].m_name, text) == 0) {
				m_miniBtn[1].m_pCartItem->m_days = i;
			}
		}
	}

	RecalculateTotalCookie(m_pang, m_cookie, true);
}

// checkoutdlg.cpp:2278
void FrCheckoutDlg::OnCartInit(intptr_t param) {
	m_pCart = DYNAMIC_CAST(FrListBox, param);
}

// checkoutdlg.cpp:2286
void FrCheckoutDlg::OnCartOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem) {
		FrGraphicInterface* pGDI = m_pCart->WndManager()->GetGDI();
		if (pGDI) {
			CartItem* pCartItem = (CartItem*)pItem->pData;

			const Bitmap* pBmp;
			if ((pCartItem->m_pItem->m_typeId >> 26) == 14) {
				pBmp = m_pCart->WndManager()->GetBitmap("SKINS", pCartItem->m_pItem->m_icon);
			}
			else if ((pCartItem->m_pItem->m_typeId >> 26) == 15) {
				pBmp = m_pCart->WndManager()->GetBitmap("HAIRS", pCartItem->m_pItem->m_icon);
			}
			else {
				pBmp = m_pCart->WndManager()->GetBitmap("ITEMS", pCartItem->m_pItem->m_icon);

				if (pBmp == 0)
					pBmp = m_pCart->WndManager()->GetBitmap("ITEMS_FASHION", pCartItem->m_pItem->m_icon);
			}

			if (pBmp) {
				WRect rc(pItem->pos.x, pItem->pos.y, (float)pBmp->Width(), (float)pBmp->Height());

				if ((pCartItem->m_pItem->m_typeId & 0xFC000000) != 0x08000000) {
					rc.x += 10.0f;
					rc.y += 20.0f;
				}

				pGDI->DrawTexture(pBmp, rc, 0xFFFFFFFF, 0);

				if (m_dayNameCount > 0) {
					if (m_dayNames[pCartItem->m_days].altPeriodUnit) {
						WPoint pos(pItem->pos.x, pItem->pos.y + 10.0f);
						pGDI->DrawTexture(m_pClockIcon, m_clockRect + pos, 0xFFFFFFFF, 0);
					}
				}
			}

			pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
			pGDI->SetTextStyle(1);

			// "(purchase only)"
			float width = pGDI->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 14.0f), 0, "%s%s", pCartItem->m_pItem->m_name, (pCartItem->m_pItem->m_flags0 >> 5) == 3 ? "(\xb1\xb8\xc0\xd4\xc0\xfc\xbf\xeb)" : "");

			if (!ItemManager()->IsInSalePeriod(pCartItem->m_pItem->m_typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
				pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
				// "(sale period over)"
				pGDI->Print(WPoint((int)width + 90.0f, 14.0f) + pItem->pos, 0, "(\xc6\xc7\xb8\xc5\xb1\xe2\xb0\xa3 \xc1\xbe\xb7\xe1)");
				pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
			}

			pGDI->SetTextStyle(0);

			// "price:", the unit "cookie" or "pang"
			int labelWidth;
			char unit[8];
			if (m_dayNameCount > 0) {
				BOOL bCookie = m_bCheckoutFlag ? FALSE : m_dayNames[pCartItem->m_days].isCash != false;
				if (bCookie) {
					labelWidth = float2int(pGDI->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 34.0f), 0, "\xb0\xa1    \xb0\xdd:   "));
					const char* text = "\xc4\xed\xc5\xb0";
					strcpy(unit, text);
				}
				else {
					labelWidth = float2int(pGDI->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 34.0f), 0, "\xb0\xa1    \xb0\xdd:   "));
					const char* text = "\xc6\xce";
					strcpy(unit, text);
				}
			}
			else {
				BOOL bCookie = m_bCheckoutFlag ? FALSE : pCartItem->m_pItem->m_flags0 & 1;
				if (bCookie) {
					labelWidth = float2int(pGDI->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 34.0f), 0, "\xb0\xa1    \xb0\xdd:   "));
					const char* text = "\xc4\xed\xc5\xb0";
					strcpy(unit, text);
				}
				else {
					labelWidth = float2int(pGDI->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 34.0f), 0, "\xb0\xa1    \xb0\xdd:   "));
					const char* text = "\xc6\xce";
					strcpy(unit, text);
				}
			}

			if ((pCartItem->m_pItem->m_typeId >> 26) == 14) {
				if (S5::ISCutinItem(pCartItem->m_pItem->m_typeId)) {
					IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pCartItem->m_pItem->m_typeId);
					// sSkin lacks the IFF common price member at 0x5C (classdefine.h).
					pGDI->Print(WPoint(labelWidth + 90.0f, 34.0f) + pItem->pos, 0, "%d%s", ((IFF_ITEM_COMMON*)pSkin)->m_price, unit);
				}
				else {
					unsigned long typeId = pCartItem->m_pItem->m_typeId;

					if ((typeId & 0x3C00000) == 0x1800000) {
						IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(typeId);
						pGDI->Print(WPoint(labelWidth + 90.0f, 34.0f) + pItem->pos, 0, "%d%s", ((IFF_ITEM_COMMON*)pSkin)->m_price, unit);
					}
					else {
						IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(typeId);
						pGDI->Print(WPoint(labelWidth + 90.0f, 34.0f) + pItem->pos, 0, "%d%s", pSkin->m_periodPrices[pCartItem->m_days], unit);
					}
				}
			}
			else if ((pCartItem->m_pItem->m_typeId >> 26) == 8) {
				IFF_STRUCT::sCadItem* pCad = ItemManager()->FindCadItem(pCartItem->m_pItem->m_typeId);
				pGDI->Print(WPoint(labelWidth + 90.0f, 34.0f) + pItem->pos, 0, "%d%s", pCad->m_levels[pCartItem->m_days], unit);
			}
			else if ((pCartItem->m_pItem->m_typeId >> 26) == 16) {
				IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(pCartItem->m_pItem->m_typeId);
				pGDI->Print(WPoint(labelWidth + 90.0f, 34.0f) + pItem->pos, 0, "%d%s", pMascot->m_periodPrices[pCartItem->m_days], unit);
			}
			else if (m_dayNameCount > 0) {
				__int64 price = 0;
				if (m_dayNames[pCartItem->m_days].hasPrice)
					price = m_dayNames[pCartItem->m_days].price;
				else if (m_dayNames[pCartItem->m_days].altPeriodUnit)
					price = m_dayNames[pCartItem->m_days].altPrice;

				pGDI->Print(WPoint(labelWidth + 90.0f, 34.0f) + pItem->pos, 0, "%I64d%s", price, unit);
			}
			else if (m_bCheckoutFlag) {
				int rentalPrice = S5::RENTALPARTS::GetRentalPrice(pCartItem->m_pItem->m_typeId);

				pGDI->Print(WPoint(labelWidth + 90.0f, 34.0f) + pItem->pos, 0, "%d%s", rentalPrice, unit);
			}
			else {
				float priceX = (float)labelWidth;
				float textX = priceX + 90.0f;
				int priceWidth = float2int(pGDI->Print(WPoint(textX, 34.0f) + pItem->pos, 0, "%d%s", pCartItem->m_pItem->m_price, unit));

				if (pCartItem->m_pItem->m_discountPrice > 0 && pCartItem->m_pItem->m_discountPrice < pCartItem->m_pItem->m_price) {
					priceX += priceWidth;
					g_view->DrawLine2D(WPoint(textX - 2.0f, float2int(pGDI->GetFontHeight() * 0.5f) + 34.0f) + pItem->pos,
						WPoint(priceX + 92.0f, float2int(pGDI->GetFontHeight() * 0.5f) + 34.0f) + pItem->pos, 0xFF000000, 0xFF000000, 0);

					pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
					pGDI->Print(WPoint(priceX + 90.0f, 34.0f) + pItem->pos, 0, " %d%s", pCartItem->m_pItem->m_discountPrice, unit);
					pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
				}
			}

			float lineY = 54.0f;
			_SYSTEMTIME* pBegin = 0;
			_SYSTEMTIME* pEnd = 0;
			if (ItemManager()->HasSalePeriod(pCartItem->m_pItem->m_typeId, &pBegin, &pEnd)) {
				if (pBegin->wYear != 0 && pEnd->wYear != 0) {
					pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
					// "sale period: %d/%d ~ %d/%d"
					pGDI->Print(WPoint(pItem->pos.x + 90.0f, pItem->pos.y + 54.0f), 0, MakeStr("\xc6\xc7\xb8\xc5\xb1\xe2\xb0\xa3: %d\xbf\xf9 %d\xc0\xcf ~ %d\xbf\xf9 %d\xc0\xcf", pBegin->wMonth, pBegin->wDay, pEnd->wMonth, pEnd->wDay));
					pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

					lineY += 20.0f;
					for (int i = 0; i < 2; i++) {
						static WRect s_rcDays = m_miniBtn[i].m_pDays->GetRect();
						WRect rcDays = s_rcDays;
						rcDays.y = s_rcDays.y + 18.0f;
						m_miniBtn[i].m_pDays->SetRect(rcDays);
					}
				}
			}

			const Bitmap* pLevel = g_pFresh->GetManager()->GetBitmap("LEVELS", MakeStr("level_%03d", (pCartItem->m_pItem->m_level & 0x7F) + 1));
			if (pLevel) {
				// "level:"
				float levelX = pGDI->GetTextExtend("\xb7\xb9    \xba\xa7:") + pItem->pos.x;
				WRect rc(levelX + 102.0f, lineY + pItem->pos.y - 3.0f, (float)pLevel->Width(), (float)pLevel->Height());
				pGDI->DrawTexture(pLevel, rc, 0xFFFFFFFF, 0);

				pGDI->Print(WPoint(pItem->pos.x + 90.0f, lineY + pItem->pos.y), 0, "\xb7\xb9    \xba\xa7:");

				// level bit 7: "or lower" instead of "or higher"
				bool bOrLower = pCartItem->m_pItem->m_level >> 7;
				const char* text = "\xc0\xcc\xbb\xf3";
				if (bOrLower)
					text = "\xc0\xcc\xc7\xcf";

				if ((!bOrLower && (pCartItem->m_pItem->m_level & 0x7F) > WSingleton<CSharedDoc>::Instance()->m_myLevel)
					|| (bOrLower && (pCartItem->m_pItem->m_level & 0x7F) < WSingleton<CSharedDoc>::Instance()->m_myLevel)) {
					pGDI->Print(WPoint(rc.w + rc.x + 5.0f, lineY + pItem->pos.y), 0, text);

					pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
					// "(cannot buy)"
					pGDI->Print(WPoint(pGDI->GetTextExtend(text) + rc.w + rc.x + 10.0f, lineY + pItem->pos.y), 0, "(\xb1\xb8\xc0\xd4\xba\xd2\xb0\xa1)");
				}
				else {
					pGDI->Print(WPoint(rc.w + rc.x + 5.0f, lineY + pItem->pos.y), 0, text);
				}
			}

			lineY += 20.0f;
			// "period of use:"
			if ((pCartItem->m_pItem->m_typeId >> 26) == 7) {
				pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
				pGDI->Print(WPoint(pItem->pos.x + 90.0f, lineY + pItem->pos.y), 0, "\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:");
				pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);

				IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(pCartItem->m_pItem->m_typeId);
				if (pCaddie == 0 || pCaddie->fee > 0) {
					// "re-hire %d days after the purchase"
					pGDI->Print(WPoint(pGDI->GetTextExtend("\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:") + 95.0f, lineY) + pItem->pos, 0, "\xb1\xb8\xc0\xd4\xc0\xcf\xb7\xce\xba\xce\xc5\xcd %d\xc0\xcf \xc8\xc4 \xc0\xe7\xb0\xed\xbf\xeb", 30);
				}
				else {
					// "unlimited"
					pGDI->Print(WPoint(pGDI->GetTextExtend("\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:") + 95.0f, lineY) + pItem->pos, 0, "\xb9\xab\xc1\xa6\xc7\xd1");
				}

				if (pCartItem->m_option) {
					pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
					// " (items already owned: %d)"
					pGDI->Print(WPoint(pItem->pos.x + 90.0f, ((pCartItem->m_pItem->m_typeId & 0xFC000000) == 0x1C000000 ? lineY + 20.0f : lineY) + pItem->pos.y), 0, " (\xc0\xcc\xb9\xcc \xb0\xa1\xc1\xf8 \xbe\xc6\xc0\xcc\xc5\xdb \xbc\xf6 %d)", pCartItem->m_option);
					pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
				}
			}
			else if ((pCartItem->m_pItem->m_typeId >> 26) == 8) {
				pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
				pGDI->Print(WPoint(pItem->pos.x + 90.0f, lineY + pItem->pos.y), 0, "\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:");
				pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);

				if (pCartItem->m_option) {
					pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
					// "(already owned)"
					pGDI->Print(WPoint(pItem->pos.x + 230.0f, lineY + pItem->pos.y), 0, "(\xc0\xcc\xb9\xcc \xbc\xd2\xc0\xaf\xc1\xdf)");
					pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
				}

				IFF_STRUCT::sCadItem* pCad = ItemManager()->FindCadItem(pCartItem->m_pItem->m_typeId);

				int line = pItem->no % 2;
				pCartItem->m_pMiniBtn = &m_miniBtn[line != 0 ? 1 : 0];

				if (pCartItem->m_pMiniBtn->m_bChanged) {
					pCartItem->m_pMiniBtn->m_pDays->ClearList();
					for (int i = 0; i < 4; i++) {
						if (pCad->m_levels[i] == 0)
							continue;

						if (i == 1)
							continue;

						pCartItem->m_pMiniBtn->m_pDays->AddString(days[i].m_name);
					}

					pCartItem->m_pMiniBtn->m_bChanged = false;
					pCartItem->m_days = 0;
					pCartItem->m_pMiniBtn->m_pDays->SetLine(1, days[0].m_name, 0, false, 0);
					pCartItem->m_pMiniBtn->m_pDays->SetVisible(true);
					RecalculateTotalCookie(0, 0, false);
				}

				pCartItem->m_pMiniBtn->m_pCartItem = pCartItem;
			}
			else if ((pCartItem->m_pItem->m_typeId >> 26) == 14) {
				if (!S5::ISCutinItem(pCartItem->m_pItem->m_typeId) && (pCartItem->m_pItem->m_typeId & 0x3C00000) != 0x1800000) {
					pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
					pGDI->Print(WPoint(pItem->pos.x + 90.0f, lineY + pItem->pos.y), 0, "\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:");
					pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);

					if (pCartItem->m_option) {
						pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
						pGDI->Print(WPoint(pItem->pos.x + 230.0f, lineY + pItem->pos.y), 0, "(\xc0\xcc\xb9\xcc \xbc\xd2\xc0\xaf\xc1\xdf)");
						pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
					}
				}

				IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pCartItem->m_pItem->m_typeId);

				int line = pItem->no % 2 != 0 ? 1 : 0;
				if (m_miniBtn[line].m_pCartItem) {
					m_miniBtn[line].m_bChanged = pCartItem->m_pItem->m_typeId != m_miniBtn[line].m_pCartItem->m_pItem->m_typeId;
				}
				else {
					m_miniBtn[line].m_bChanged = true;
				}

				pCartItem->m_pMiniBtn = &m_miniBtn[line];

				if (pCartItem->m_pMiniBtn->m_bChanged) {
					pCartItem->m_pMiniBtn->m_pDays->ClearList();
					for (int i = 0; i < 5; i++) {
						if (pSkin->m_periodPrices[i])
							pCartItem->m_pMiniBtn->m_pDays->AddString(days[i].m_name);
					}
					pCartItem->m_pMiniBtn->m_bChanged = false;
				}

				pCartItem->m_pMiniBtn->m_pDays->SetLine(1, days[pCartItem->m_days].m_name, 0, false, 0);
				if (S5::ISCutinItem(pCartItem->m_pItem->m_typeId))
					pCartItem->m_pMiniBtn->m_pDays->SetVisible(false);
				else if ((pCartItem->m_pItem->m_typeId & 0x3C00000) == 0x1800000)
					pCartItem->m_pMiniBtn->m_pDays->SetVisible(false);
				else
					pCartItem->m_pMiniBtn->m_pDays->SetVisible(true);

				pCartItem->m_pMiniBtn->m_pCartItem = pCartItem;
			}
			else if ((pCartItem->m_pItem->m_typeId >> 26) == 16) {
				pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
				pGDI->Print(WPoint(pItem->pos.x + 90.0f, lineY + pItem->pos.y), 0, "\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3:");
				pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);

				if (pCartItem->m_option) {
					pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
					pGDI->Print(WPoint(pItem->pos.x + 230.0f, lineY + pItem->pos.y), 0, "(\xc0\xcc\xb9\xcc \xbc\xd2\xc0\xaf\xc1\xdf)");
					pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
				}

				IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(pCartItem->m_pItem->m_typeId);

				int line = pItem->no % 2;
				pCartItem->m_pMiniBtn = &m_miniBtn[line != 0 ? 1 : 0];

				if (pCartItem->m_pMiniBtn->m_bChanged) {
					pCartItem->m_pMiniBtn->m_pDays->ClearList();
					for (int i = 0; i < 5; i++) {
						if (pMascot->m_periodPrices[i])
							pCartItem->m_pMiniBtn->m_pDays->AddString(days[i].m_name);
					}
					pCartItem->m_pMiniBtn->m_bChanged = false;
				}

				pCartItem->m_pMiniBtn->m_pDays->SetLine(1, days[pCartItem->m_days].m_name, 0, false, 0);
				pCartItem->m_pMiniBtn->m_pDays->SetVisible(true);

				pCartItem->m_pMiniBtn->m_pCartItem = pCartItem;
			}
			else if (m_dayNameCount > 0) {
				pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
				// "purchase method:"
				pGDI->Print(WPoint(pItem->pos.x + 90.0f, lineY + pItem->pos.y), 0, "\xb1\xb8\xc0\xd4\xb9\xe6\xb9\xfd:");
				pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);

				if (pCartItem->m_option) {
					pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
					pGDI->Print(WPoint(pItem->pos.x + 230.0f, lineY + pItem->pos.y), 0, "(\xc0\xcc\xb9\xcc \xbc\xd2\xc0\xaf\xc1\xdf)");
					pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
				}

				int line = pItem->no % 2;
				pCartItem->m_pMiniBtn = &m_miniBtn[line != 0 ? 1 : 0];

				if (pCartItem->m_pMiniBtn->m_bChanged) {
					pCartItem->m_pMiniBtn->m_pDays->ClearLine();

					for (int i = 0; i < m_dayNameCount; i++)
						pCartItem->m_pMiniBtn->m_pDays->AddString(m_dayNames[i].name);

					pCartItem->m_pMiniBtn->m_bChanged = false;
				}

				pCartItem->m_pMiniBtn->m_pDays->SetLine(1, m_dayNames[pCartItem->m_days].name, 0, false, 0);
				pCartItem->m_pMiniBtn->m_pDays->SetVisible(true);

				pCartItem->m_pMiniBtn->m_pCartItem = pCartItem;
			}

			if (m_pExplaneStatic && m_bTotalCookieSet) {
				pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
				pGDI->SetTextStyle(1);

				const char* cashName = 0;
				switch (WSingleton<CSharedDoc>::Instance()->m_billingType) {
				case 2:
					cashName = "\xc6\xae\xb8\xae\xc4\xb3\xbd\xac";
					break;
				case 4:
					cashName = "\xc7\xd1\xc4\xda\xc0\xce";
					break;
				}

				if (cashName) {
					const WRect* pRect = &m_pExplaneStatic->GetRect();
					if (m_totalCookie < 0) {
						// "%s balance is insufficient."
						pGDI->Print(WPoint(pRect->x - 70.0f, pRect->y - 20.0f), 0, MakeStr("%s \xc0\xdc\xbe\xd7\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.", cashName));
					}
					else {
						__int64 cash = MyOutsideCash();
						__int64 cookie = MyBonusCash() - m_unitCookie + m_cookieDiscount;
						__int64 exchanged = S5::ExchangeOwnCookieToCash(cookie, 100);
						if (exchanged < 0)
							cash = MyOutsideCash() + exchanged;

						// "%s balance after the purchase: %I64d won."
						pGDI->Print(WPoint(pRect->x - 140.0f, pRect->y - 20.0f), 0, MakeStr("\xb1\xb8\xb8\xc5 \xc8\xc4 \xc3\xd6\xc1\xbe %s \xc0\xdc\xbe\xd7\xc0\xba %I64d\xbf\xf8 \xc0\xd4\xb4\xcf\xb4\xd9.", cashName, cash > 0 ? cash : 0));
					}
				}

				pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
				pGDI->SetTextStyle(0);
			}

			if (IsLocalContent((localContentType_t)0x17) && !WSingleton<CSharedDoc>::Instance()->IsHaveHalloweenItem()) {
				unsigned long typeId = pCartItem->m_pItem->m_typeId;
				if ((typeId & 0xFC000000) == 0x08000000 && (typeId & 0x1800) == 0x1800) {
					pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
					// "*the pumpkin hat can be bought once per account"
					pGDI->Print(WPoint(pItem->pos.x + 20.0f, pItem->pos.y + 330.0f), 0, "*\xc8\xa3\xb9\xda\xb8\xf0\xc0\xda\xb4\xc2 \xb0\xe8\xc1\xa4\xb4\xe7 \xc7\xd1\xb9\xf8 \xb9\xdb\xbf\xa1 \xb1\xb8\xb8\xc5\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
					pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
				}
			}
		}
	}
}

// checkoutdlg.cpp:2872
void FrCheckoutDlg::OnPriceInit(intptr_t param) {
	m_pPrice = DYNAMIC_CAST(FrEdit, param);
}

// checkoutdlg.cpp:2878
void FrCheckoutDlg::OnMyMoneyInit(intptr_t param) {
	m_pMyMoney = DYNAMIC_CAST(FrEdit, param);
}

// checkoutdlg.cpp:2884
void FrCheckoutDlg::OnMoneyLeftInit(intptr_t param) {
	m_pMoneyLeft = DYNAMIC_CAST(FrEdit, param);
}

// checkoutdlg.cpp:2890
void FrCheckoutDlg::OnPrice_CookieInit(intptr_t param) {
	m_pPrice_Cookie = DYNAMIC_CAST(FrEdit, param);
}

// checkoutdlg.cpp:2898
void FrCheckoutDlg::OnMyMoney_CookieInit(intptr_t param) {
	m_pMyMoney_Cookie = DYNAMIC_CAST(FrEdit, param);
}

// checkoutdlg.cpp:2906
void FrCheckoutDlg::OnMoneyLeft_CookieInit(intptr_t param) {
	m_pMoneyLeft_Cookie = DYNAMIC_CAST(FrEdit, param);
}

// checkoutdlg.cpp:2914
void FrCheckoutDlg::OnNumStaticInit(intptr_t param) {
	m_pNumStatic = DYNAMIC_CAST(FrStatic, param);
}

// checkoutdlg.cpp:2920
void FrCheckoutDlg::OnExplaneStaticInit(intptr_t param) {
	m_pExplaneStatic = DYNAMIC_CAST(FrStatic, param);
}

// checkoutdlg.cpp:2926
void FrCheckoutDlg::OnNumInit(intptr_t param) {
	m_pNum = DYNAMIC_CAST(FrEdit, param);
}

// checkoutdlg.cpp:2931
bool FrCheckoutDlg::OnNumEnterKey(intptr_t key) {
	return false;
}

// checkoutdlg.cpp:2937
void FrCheckoutDlg::OnNumPrevInit(intptr_t param) {
	m_pNumPrev = DYNAMIC_CAST(FrButton, param);
	if (m_pNumPrev) {
		m_pNumPrev->SetPushDelay(0.0f);
	}
}

// checkoutdlg.cpp:2945
void FrCheckoutDlg::OnNumPrevLBtnUp(void) {
	if (m_pNum) {
		const char* text = m_pNum->GetLine(1, false);
		int num = atoi(text);

		if (num > 1) {
			SetPrice(num - 1);
		}
	}
}

// checkoutdlg.cpp:2963
void FrCheckoutDlg::OnNumNextInit(intptr_t param) {
	m_pNumNext = DYNAMIC_CAST(FrButton, param);
	if (m_pNumNext) {
		m_pNumNext->SetPushDelay(0.0f);
	}
}

// checkoutdlg.cpp:2971
void FrCheckoutDlg::OnNumNextLBtnUp(void) {
	if (m_pNum) {
		const char* text = m_pNum->GetLine(1, false);
		int num = atoi(text);

		if (num < 99) {
			SetPrice(num + 1);
		}
	}
}
