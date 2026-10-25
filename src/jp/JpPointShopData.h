// The JP item manager's point shop data the port's KR CItemManager does not
// hold, read by the port from the same IFF zip at first use (JpItemGlue.cpp):
// - the goods (JP CItemManager +0x200, a std::map by the type id sold, filled
//   from PointShop.iff by 0xa2f610, which the JP CItemManager::Load 0xa3de50
//   calls; KR has no such table);
// - the 0x18 bytes at 0x84 of every JP common item header (the 2016 insert of
//   docs/PAK_FORMAT.md "JP tables"), which the JP client reads in the record
//   itself and the port's IFF converter (cmake/iff_us.cpp) drops when it cuts
//   the records to the KR layout: a side table by type id.
// Layouts: docs/protocol/jp_items.md (the offline server reads the same
// data, offline/domain/JpTiki.h).
#ifndef PG_JP_JPPOINTSHOPDATA_H
#define PG_JP_JPPOINTSHOPDATA_H

#include <stdint.h>

#include "defines.h"

// A PointShop.iff record (0x14 bytes), the value of the JP goods map.
struct JpPointShopGoods {
	uint8_t active;             // 0x0 (the client tests the byte)
	uint8_t pad1[3];
	uint32_t typeId;            // 0x4, the item sold (the map key)
	int32_t price;              // 0x8, the price in Tiki points
	uint32_t count;             // 0xc, the units one purchase gives
	uint8_t flags;              // 0x10, bit 0: listed first
	uint8_t pad11[3];
};
CHECK_SIZE(JpPointShopGoods, 0x14);
CHECK_FIELD(JpPointShopGoods, typeId, 0x4);
CHECK_FIELD(JpPointShopGoods, price, 0x8);
CHECK_FIELD(JpPointShopGoods, count, 0xc);
CHECK_FIELD(JpPointShopGoods, flags, 0x10);

// The 0x18 bytes at 0x84 of a JP common item header.
struct JpIffTikiData {
	uint32_t maxCount;          // 0x84, the units one exchange takes at most (CPointShop::GetMaxCount 0x8518e0)
	int32_t pointValue;         // 0x88, the Tiki points it gives (CPointShop::GetExchangePoint 0x851810)
	uint16_t mileage;           // 0x8c, the Tiki shop mileage per unit (the MP CAlchemyMPBreak::GetItemMP 0x475f80 reads)
	uint16_t bonusChance;       // 0x8e
	uint16_t bonusMin;          // 0x90
	uint16_t bonusMax;          // 0x92
	uint32_t kind;              // 0x94, 1 normal, 2 cookie, 3 rare (compared with 3 by 0x476c70)
	uint32_t pang;              // 0x98, the pang of the exchange (summed by CAlchemyMPBreak::SumSelectionPrice 0x476b20)
};
CHECK_SIZE(JpIffTikiData, 0x18);
CHECK_FIELD(JpIffTikiData, pointValue, 0x4);
CHECK_FIELD(JpIffTikiData, mileage, 0x8);
CHECK_FIELD(JpIffTikiData, kind, 0x10);
CHECK_FIELD(JpIffTikiData, pang, 0x14);

// The goods by type id (NULL when not sold), and in key order.
const JpPointShopGoods* JpPointShopData_FindGoods(uint32_t typeId);
int JpPointShopData_GetGoodsCount(void);
const JpPointShopGoods* JpPointShopData_GetGoodsAt(int index);
// The JP header bytes 0x84-0x9c of the record with this type id (NULL when
// no common-header table has it).
const JpIffTikiData* JpPointShopData_FindTiki(uint32_t typeId);

#endif // PG_JP_JPPOINTSHOPDATA_H
