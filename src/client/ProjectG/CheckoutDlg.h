// Original: d:\build\custom\temp\code\client\projectg\checkoutdlg.h
#ifndef PG_CLIENT_PROJECTG_CHECKOUTDLG_H
#define PG_CLIENT_PROJECTG_CHECKOUTDLG_H

#include <list>
#include <map>

#include "defines.h"
#include "padding.h"
#include "frform.h"

class Bitmap;
class FrButton;
class FrComboBox;
class FrEdit;
class FrListBox;
class FrStatic;
struct sCaddieInfo;
struct IFF_ITEM_COMMON;

// Shop checkout dialog (cart, prices in pang and cookies, rental days); its
// size (0x3d0) is the `new` of FrCheckoutDlgMakeInstance. Only the members
// this unit's written functions use are named.
class FrCheckoutDlg : public FrForm {
	LAYOUT_CHECK_FRIEND;
public:
	struct CartItem;

	// Rental-days selector of a cart line (two per dialog, 12 bytes).
	class MiniBtn {
	public:
		MiniBtn(void);

		FrComboBox* m_pDays;        // 0x0
		CartItem* m_pCartItem;      // 0x4, cart line the selector edits
		bool m_bChanged;            // 0x8
	};

	// One line of the cart (std::list node value, 0x14 bytes).
	struct CartItem {
		IFF_ITEM_COMMON* m_pItem;   // 0x0, shop item of the line
		int m_option;               // 0x4, nonzero sets m_flag16a (Checkout)
		int m_unused;               // 0x8, never read or written by the client (Checkout leaves it uninitialized)
		int m_days;                 // 0xc, index of the rental period
		MiniBtn* m_pMiniBtn;        // 0x10
	};

	// One rental period offered for the item of a cart line, filled by
	// FixCartItem from each IFF_STRUCT::sChildItem of the item (same shape
	// as FrGiftDlg::sPeriodPrice).
	struct sPeriodPrice {
		char name[0x20];              // 0x0, the text shown in the days combo box
		bool isCash;                  // 0x20, priced in cookies (sChildItem::isCash)
		UNKNOWN(0x21, 0x28);
		__int64 price;                // 0x28, sChildItem::m_price
		bool hasPrice;                // 0x30, sChildItem::m_hasPrice
		UNKNOWN(0x31, 0x32);
		unsigned short quantity;      // 0x32, sChildItem::m_quantity
		unsigned int altPrice;        // 0x34, sChildItem::m_altPrice
		unsigned char altPeriodUnit;  // 0x38, sChildItem::m_altPeriodUnit
		UNKNOWN(0x39, 0x3A);
		unsigned short altPeriod;     // 0x3A, sChildItem::m_altPeriod
		UNKNOWN(0x3C, 0x40);
	};

	// A discount coupon of the player (std::map value, 0x10 bytes).
	struct sCoupon {
#ifdef JP_PAKS
		// The JP coupon list builder 0xaa2f30 copies them from the player's
		// item list (CSharedDoc +0x375c, node value +0x4 / +0x0).
		unsigned int m_typeId;      // 0x0, the coupon item's type id
		unsigned int m_guid;        // 0x4, the player's coupon item id, sent by GetGuidSelectedCoupon
#else
		UNKNOWN(0x0, 0x8);
#endif
		bool m_bUsable;             // 0x8
		int m_period;               // 0xc
	};

	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }   // checkoutdlg.h:40

	FrCheckoutDlg(void);

	bool IsUnderLevel(void);
	bool IsOverLevel(void);
	std::list<CartItem>& Cart(void);
	void Process(void);
	void Checkout(__int64 pang, __int64 cookie, bool b);

private:
	static sFRESH_ENTRY _MsgEntries[];

protected:
	static sFRESH_MSGMAP _MsgMap;
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;

	void FixCartItem(CartItem& item);
	void SetPrice(int num);
	void RecalculateTotalCookie(__int64 pang, __int64 cookie, bool b);
	unsigned long GetGuidSelectedCoupon(void);
	void OnDaysBtnDown(void);
	void OnDays2BtnDown(void);
	bool OnNumEnterKey(intptr_t key);
	void OnNumPrevLBtnUp(void);
	void OnNumNextLBtnUp(void);
	bool OnChargeResult(int ret, FrForm* pForm);
	bool OnConfirmResult(int ret, FrForm* pForm);
	void OnCheckoutInit(intptr_t param);
	void OnDaysInit(intptr_t param);
	void OnDays2Init(PG_MSGARG param);
	void OnCartInit(intptr_t param);
	void OnPriceInit(intptr_t param);
	void OnMyMoneyInit(intptr_t param);
	void OnMoneyLeftInit(intptr_t param);
	void OnPrice_CookieInit(intptr_t param);
	void OnMyMoney_CookieInit(intptr_t param);
	void OnMoneyLeft_CookieInit(intptr_t param);
	void OnNumStaticInit(intptr_t param);
	void OnExplaneStaticInit(intptr_t param);
	void OnNumInit(intptr_t param);
	void OnNumPrevInit(intptr_t param);
	void OnNumNextInit(intptr_t param);
	void OnCartOwnerDraw(PG_MSGARG param);
	void OnCheckoutLBtnUp(void);
	void OpenCaddieInfo(const sCaddieInfo& info);

	FrButton* m_pCheckout;               // 0x110
	FrListBox* m_pCart;                  // 0x114
	FrEdit* m_pPrice;                    // 0x118
	FrEdit* m_pMyMoney;                  // 0x11c
	FrEdit* m_pMoneyLeft;                // 0x120
	FrEdit* m_pPrice_Cookie;             // 0x124
	FrEdit* m_pMyMoney_Cookie;           // 0x128
	FrEdit* m_pMoneyLeft_Cookie;         // 0x12c
	FrStatic* m_pNumStatic;              // 0x130
	FrStatic* m_pExplaneStatic;          // 0x134
	FrEdit* m_pNum;                      // 0x138
	FrButton* m_pNumPrev;                // 0x13c
	FrButton* m_pNumNext;                // 0x140
	MiniBtn m_miniBtn[2];                // 0x144
	std::list<CartItem> m_cart;          // 0x15c
	bool m_bUnderLevel;                  // 0x168
	bool m_bOverLevel;                   // 0x169
	bool m_flag16a;                      // 0x16a
	bool m_bCheckoutFlag;                // 0x16b, third argument of Checkout
	PADDING(0x16c, 0x170);               // FrCheckoutDlg members
	__int64 m_unitPang;                  // 0x170, pang price of one unit (SetPrice multiplies it by m_num)
	__int64 m_unitCookie;                // 0x178, cookie price
	bool m_bEnoughPang;                  // 0x180, cleared by SetPrice when the player lacks pang
	bool m_bEnoughCookie;                // 0x181, cleared by SetPrice when the player lacks cookies
	int m_value184;                      // 0x184
	int m_num;                           // 0x188, purchase count (m_pNum)
	PADDING(0x18c, 0x190);               // FrCheckoutDlg members
	__int64 m_pang;                      // 0x190
	__int64 m_cookie;                    // 0x198
	__int64 m_totalCookie;               // 0x1a0, CSharedDoc::RemainOwnCash of the cookie price (RecalculateTotalCookie)
	bool m_bTotalCookieSet;              // 0x1a8, false when only pang is spent
	std::map<unsigned int, sCoupon> m_coupons;   // 0x1ac
	bool m_bCouponFlag;                  // 0x1b8, cleared by GetGuidSelectedCoupon
	int m_selectedCoupon;                // 0x1bc
	int m_value1c0;                      // 0x1c0
	PADDING(0x1c4, 0x1c8);               // FrCheckoutDlg members
	__int64 m_pangDiscount;              // 0x1c8, subtracted from the pang price (SetPrice)
	__int64 m_cookieDiscount;            // 0x1d0, subtracted from the cookie price (SetPrice)
	int m_value1d8;                      // 0x1d8
	bool m_flag1dc;                      // 0x1dc
	PADDING(0x1dd, 0x270);               // FrCheckoutDlg members
	WRect m_clockRect;                   // 0x270, where the "clock" icon is drawn
	const Bitmap* m_pClockIcon;          // 0x280
	PADDING(0x284, 0x288);               // FrCheckoutDlg members
	int m_dayNameCount;                  // 0x288, entries of m_dayNames in use
	PADDING(0x28c, 0x290);               // FrCheckoutDlg members
	sPeriodPrice m_dayNames[5];          // 0x290, rental periods of the item (FixCartItem)
#ifdef JP_PAKS
	// The JP coupon picker: four icons paged by prev / next, the discounts of
	// the selected coupon. Its 8 control pointers sit at JP +0x15c..+0x178,
	// before the cart (later JP offsets = KR + 0x20). JP +0x1dc m_bCouponFlag,
	// +0x1e0 m_selectedCoupon, +0x1e8 / +0x1f0 / +0x1f8 m_pangDiscount /
	// m_cookieDiscount / m_value1d8 (the kind); the JP constructor 0xaa1af0.
protected:
	void BuildCouponList(void);
	void UpdateCouponControls(void);
	void UpdateCouponButtons(void);
	void SelectCoupon(int index);
	void RecalculateCouponDiscount(void);
	void ShowCouponInformation(const sCoupon* coupon);
	void DrawCouponLine(void);
	void DrawCouponIcons(void);
	void OnCouponPangInit(intptr_t param);
	void OnCouponCookieInit(intptr_t param);
	void OnCouponIcon1Init(intptr_t param);
	void OnCouponIcon2Init(intptr_t param);
	void OnCouponIcon3Init(intptr_t param);
	void OnCouponIcon4Init(intptr_t param);
	void OnCouponIconPrevInit(intptr_t param);
	void OnCouponIconNextInit(intptr_t param);
	void OnCouponIcon1LBtnUp(void);
	void OnCouponIcon2LBtnUp(void);
	void OnCouponIcon3LBtnUp(void);
	void OnCouponIcon4LBtnUp(void);
	void OnCouponIcon1RBtnUp(void);
	void OnCouponIcon2RBtnUp(void);
	void OnCouponIcon3RBtnUp(void);
	void OnCouponIcon4RBtnUp(void);
	void OnCouponIconPrevLBtnUp(void);
	void OnCouponIconNextLBtnUp(void);

	FrEdit* m_pCouponPang;               // JP 0x15c, "coupon_pang": the pang discount
	FrEdit* m_pCouponCookie;             // JP 0x160, "coupon_cookie": the cookie discount
	FrButton* m_pCouponIcon[4];          // JP 0x164, "coupon_icon_01".."coupon_icon_04"
	FrButton* m_pCouponPrev;             // JP 0x174, "coupon_icon_prev"
	FrButton* m_pCouponNext;             // JP 0x178, "coupon_icon_next"
	int m_couponPage;                    // JP 0x1e4 (KR m_value1c0's slot), m_coupons key on the first icon
#endif
};

#ifndef JP_PAKS
CHECK_SIZE(FrCheckoutDlg, 0x3d0);
#endif
BEGIN_LAYOUT_CHECK(FrCheckoutDlg)
	CHECK_FIELD(FrCheckoutDlg, m_pCheckout, 0x110);
	CHECK_FIELD(FrCheckoutDlg, m_pNumNext, 0x140);
	CHECK_FIELD(FrCheckoutDlg, m_miniBtn, 0x144);
	CHECK_FIELD(FrCheckoutDlg, m_cart, 0x15c);
	CHECK_FIELD(FrCheckoutDlg, m_bUnderLevel, 0x168);
	CHECK_FIELD(FrCheckoutDlg, m_bCheckoutFlag, 0x16b);
	CHECK_FIELD(FrCheckoutDlg, m_unitPang, 0x170);
	CHECK_FIELD(FrCheckoutDlg, m_unitCookie, 0x178);
	CHECK_FIELD(FrCheckoutDlg, m_bEnoughPang, 0x180);
	CHECK_FIELD(FrCheckoutDlg, m_bEnoughCookie, 0x181);
	CHECK_FIELD(FrCheckoutDlg, m_value184, 0x184);
	CHECK_FIELD(FrCheckoutDlg, m_num, 0x188);
	CHECK_FIELD(FrCheckoutDlg, m_pang, 0x190);
	CHECK_FIELD(FrCheckoutDlg, m_cookie, 0x198);
	CHECK_FIELD(FrCheckoutDlg, m_totalCookie, 0x1a0);
	CHECK_FIELD(FrCheckoutDlg, m_bTotalCookieSet, 0x1a8);
	CHECK_FIELD(FrCheckoutDlg, m_coupons, 0x1ac);
	CHECK_FIELD(FrCheckoutDlg, m_bCouponFlag, 0x1b8);
	CHECK_FIELD(FrCheckoutDlg, m_selectedCoupon, 0x1bc);
	CHECK_FIELD(FrCheckoutDlg, m_value1c0, 0x1c0);
	CHECK_FIELD(FrCheckoutDlg, m_pangDiscount, 0x1c8);
	CHECK_FIELD(FrCheckoutDlg, m_cookieDiscount, 0x1d0);
	CHECK_FIELD(FrCheckoutDlg, m_value1d8, 0x1d8);
	CHECK_FIELD(FrCheckoutDlg, m_flag1dc, 0x1dc);
	CHECK_FIELD(FrCheckoutDlg, m_clockRect, 0x270);
	CHECK_FIELD(FrCheckoutDlg, m_pClockIcon, 0x280);
	CHECK_FIELD(FrCheckoutDlg, m_dayNameCount, 0x288);
	CHECK_FIELD(FrCheckoutDlg, m_dayNames, 0x290);
END_LAYOUT_CHECK
CHECK_SIZE(FrCheckoutDlg::CartItem, 0x14);
CHECK_FIELD(FrCheckoutDlg::CartItem, m_option, 0x4);
CHECK_FIELD(FrCheckoutDlg::CartItem, m_unused, 0x8);
CHECK_FIELD(FrCheckoutDlg::CartItem, m_days, 0xc);
CHECK_SIZE(FrCheckoutDlg::sPeriodPrice, 0x40);
CHECK_FIELD(FrCheckoutDlg::sPeriodPrice, isCash, 0x20);
CHECK_FIELD(FrCheckoutDlg::sPeriodPrice, price, 0x28);
CHECK_FIELD(FrCheckoutDlg::sPeriodPrice, hasPrice, 0x30);
CHECK_FIELD(FrCheckoutDlg::sPeriodPrice, quantity, 0x32);
CHECK_FIELD(FrCheckoutDlg::sPeriodPrice, altPrice, 0x34);
CHECK_FIELD(FrCheckoutDlg::sPeriodPrice, altPeriodUnit, 0x38);
CHECK_FIELD(FrCheckoutDlg::sPeriodPrice, altPeriod, 0x3A);

#endif // PG_CLIENT_PROJECTG_CHECKOUTDLG_H
