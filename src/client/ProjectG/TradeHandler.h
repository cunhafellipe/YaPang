// Original: d:\build\custom\temp\code\client\projectg\tradehandler.h
#ifndef PG_CLIENT_PROJECTG_TRADEHANDLER_H
#define PG_CLIENT_PROJECTG_TRADEHANDLER_H

#include "defines.h"
#include "gatewayinterface.h"
#include "padding.h"
#include "shared/globalnetworkdefine.h"

#include <string>
#include <vector>

namespace _gateway {

#pragma pack(push, 1)

// An item bought in a trade: the traded item (copied out by
// sTransformHelper), then the inventory item it becomes.
struct sTradeBuyBase {
	sTradeItem item;            // 0x00
	sItemInfo info;             // 0xAA: the bought item as it enters the inventory (UpdateBuyItem)
	unsigned char countType;    // 0x152: 2 or 3; anything else is an invalid counting type
};
CHECK_SIZE(sTradeBuyBase, 0x153);
CHECK_FIELD(sTradeBuyBase, info, 0xAA);
CHECK_FIELD(sTradeBuyBase, countType, 0x152);

#pragma pack(pop)

// m_param2 of the "trade buy item" message, filled from packet 0x1D6.
struct sTradeBuyParam {
	std::vector<sTradeBuyBase> itemList; // 0x00
	int result;                          // 0x10, passed to SetTradeResult
	PADDING(0x14, 0x18);
	__int64 pang;                        // 0x18, the local player's new pang
};
CHECK_FIELD(sTradeBuyParam, result, 0x10);
CHECK_FIELD(sTradeBuyParam, pang, 0x18);

// m_param2 of the "trade sell item" message, filled from packet 0x1D7.
struct sTradeSellParam {
	std::vector<sTradeItem> itemList; // 0x00
	int result;                       // 0x10, 1 once the packet is read
	PADDING(0x14, 0x18);
	__int64 pang;                     // 0x18, the pang earned
};
CHECK_FIELD(sTradeSellParam, result, 0x10);
CHECK_FIELD(sTradeSellParam, pang, 0x18);

// m_param2 of the "trade minus item" message, filled from packet 0x1D8:
// items taken off a shop's list.
struct sTradeMinusParam {
	std::vector<sTradeItem> itemList; // 0x00, removed with CSharedDoc::MinusListTradeItem
	std::string ownerName;            // 0x10, the shop owner's nickname
	int shopState;                    // 0x2C, an eStateTrade; 3 is copied to CSharedDoc::SetStateTrade
	unsigned long shopUID;            // 0x30, compared with CSharedDoc::GetTradeUID
};
CHECK_FIELD(sTradeMinusParam, ownerName, 0x10);
CHECK_FIELD(sTradeMinusParam, shopState, 0x2C);
CHECK_FIELD(sTradeMinusParam, shopUID, 0x30);

// tradehandler.cpp:272
struct sTransformHelper {
	const sTradeItem operator()(const sTradeBuyBase& buyItem);
};

// Handler of the trade gateway content: CGatewayHandler (0x1C bytes), no
// member of its own. Its document is a CTradeDoc.
class CTradeHandler : public CGatewayHandler {
public:
	CTradeHandler(void);
	virtual ~CTradeHandler(void);

	virtual bool Initialize(void);
	virtual bool Execute(const MsgObject& msg);

private:
	void OnSetEnablePackageSale(void);
	void OnSetDisablePackageSale(void);
	void OnGetPackageSale(intptr_t param);
	void OnNotifyTradeErrorCode(intptr_t param);
	void OnUpdateTradeMinusItem(int param);
	void OnUpdateTradeSellItem(intptr_t param);
	void OnUpdateTradeBuyItem(intptr_t param);
};

CHECK_SIZE(CTradeHandler, 0x1C);

}

#endif // PG_CLIENT_PROJECTG_TRADEHANDLER_H
