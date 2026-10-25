// The JP item lookups shared by the JP game code: the JP CItemManager (the
// JP CSharedDoc 0xe47dbc +0x20c) Find* functions mapped onto the port's KR
// CItemManager (ItemManager()), and the JP-only IFF / CSharedDoc values the
// point shop reads. Bodies: JpItemGlue.cpp.
//
// The JP finders are KR's: a std::map lookup by type id returning the
// record (the map node's value, +0x10) or NULL. The JP maps sit at item
// manager +0x30 parts, +0x50 club sets, +0x60 balls, +0x70 items, +0x1b0
// cards and +0x200 the point shop's goods (VS 2010 maps; KR m_partMap 0x30,
// m_clubSetMap 0x48, m_ballMap 0x54, m_itemMap 0x60, m_cardMap 0x12c: the
// same order for the first ones, and KR has no goods table).
//
// Every record starts with the IFF common header (+0x4 the type id); the
// pointers stay opaque (JpIffItemBase) and point at the KR record of the
// type id's kind (typeId >> 26: 2 part, 4 club set, 5 ball, 6 item, 0x1f
// card).
#ifndef PG_JP_JPITEMGLUE_H
#define PG_JP_JPITEMGLUE_H

#include <stdint.h>

struct JpIffItemBase;
const JpIffItemBase* JpItemManager_FindPart(uint32_t typeId);      // 0xa17160 (KR FindPart)
const JpIffItemBase* JpItemManager_FindClubSet(uint32_t typeId);   // 0xa171e0 (KR FindClubSet)
const JpIffItemBase* JpItemManager_FindBall(uint32_t typeId);      // 0xa17220 (KR FindBall)
const JpIffItemBase* JpItemManager_FindItem(uint32_t typeId);      // 0xa17260 (KR FindItem)
const JpIffItemBase* JpItemManager_FindCard(uint32_t typeId);      // 0xa17a10 (KR FindCard)
// 0xa17cf0: the record of any kind (a jump table on typeId >> 26 over the
// finders above and the other kinds' ones, NULL for an unknown kind): KR
// CItemManager::FindCommonItem.
const JpIffItemBase* JpItemManager_FindCommonItem(uint32_t typeId);

// The JP header values the alchemy dialog (JpAlchemyDlg.cpp) reads: the
// 2016 insert at 0x84-0x9c of the JP common header, which the port's KR
// records lack; read from the side table of JpPointShopData.h by the
// record's type id (0 when it has none).
// JP word +0x8c: the MP one item gives (CAlchemyMPBreak::GetItemMP 0x475f80).
int JpIffItemBase_GetBreakMP(const JpIffItemBase* item);
// JP dword +0x94: compared with 3 by the warning check 0x476c70.
int JpIffItemBase_GetBreakClass(const JpIffItemBase* item);
// JP dword +0x98: summed over the selection (CAlchemyMPBreak::
// SumSelectionPrice 0x476b20).
uint32_t JpIffItemBase_GetBreakPrice(const JpIffItemBase* item);
// The type id of a record (+0x4, the common header's).
uint32_t JpIffItemBase_GetTypeId(const JpIffItemBase* item);
// JP card byte +0xc0, the first byte after the JP common header: KR
// sCard::m_grade (+0x90, the first byte after KR's; the warning check
// 0x476e60).
int JpIffCard_GetGrade(const JpIffItemBase* card);
// 0x7e9580 (fastcall, the type id in ecx): a part (typeId >> 26 == 2)
// whose JP record byte +0xe8 is 7, 8 or 9 (the alchemy warning check
// 0x476bf0): KR sPart::slot (+0xb8, the part slot index).
bool JpItem_IsPartClass7to9(uint32_t typeId);

// JP header dword +0x88: the Tiki points the item is worth in the point
// shop's exchange (CPointShop::GetExchangePoint 0x851810).
int JpIffItemBase_GetPointValue(const JpIffItemBase* item);
// JP header dword +0x84: how many of a ball, item or card the exchange
// takes at most (CPointShop::GetMaxCount 0x8518e0).
int JpIffItemBase_GetMaxCount(const JpIffItemBase* item);

// The point shop's goods (JP item manager +0x200, PointShop.iff; 0xa17570,
// no KR counterpart; the record: JpPointShopData.h): +0x8 is the price in
// Tiki points.
struct JpPointShopGoods;
const JpPointShopGoods* JpItemManager_FindGoods(uint32_t typeId);  // 0xa17570
int JpPointShopGoods_GetPrice(const JpPointShopGoods* goods);      // reads +0x8
// The goods in the table's key order (FrPointShopDlg::BuildItemList 0x850f10
// walks the JP map): the count and the goods at an index.
int JpItemManager_GetGoodsCount(void);
const JpPointShopGoods* JpItemManager_GetGoodsAt(int index);
bool JpPointShopGoods_IsActive(const JpPointShopGoods* goods);       // reads the byte +0x0
uint32_t JpPointShopGoods_GetTypeId(const JpPointShopGoods* goods);  // reads +0x4 (the item sold)
uint32_t JpPointShopGoods_GetCount(const JpPointShopGoods* goods);   // reads +0xc
// Bit 0 of the byte +0x10: the goods are listed first.
bool JpPointShopGoods_IsListedFirst(const JpPointShopGoods* goods);

// Whether an owned item id is in the JP CSharedDoc +0x37ec list (std::list,
// the element's +0xc dword compared with the id; FrPointShopDlg 0x84e350
// skips those items). The list right after the owned cards (+0x37e0,
// KR m_myCardList), where KR has m_cardPeriodList[0]: role not proven,
// declared only.
bool JpSharedDoc_IsInList37ec(uint32_t itemId);

// The player's Tiki points (JP CSharedDoc +0x3860, a dword; no KR member).
int JpSharedDoc_GetTikiPoints(void);
void JpSharedDoc_SetTikiPoints(int points);

// The JP CSharedDoc item-change appliers of the 0x0216 record kinds (the
// item apply 0x58c160 on its +0x11628 member; TaskManager.cpp case 0x216).
// Each returns the JP refusal code (0: applied), which the caller shows with
// main actor message 0x27. The JP maps: +0x3740 the club sets by serial (KR
// m_myItemMap, sItemInfo), +0x3734 the parts (KR m_myPartsList), +0x3714
// the caddies (KR m_caddieMap).
// Kind 0xC9 (0x588ff0, none 0x1E8553): the 5 stat levels (record +0x1c) into
// the club set's +0xc (KR sItemInfo::m_clubStatLevel). Kind 0xCB (0x589070,
// none 0x1E8554, slot 0 0x1E8555): the card type id (record +0x4c) into the
// slot dword +0x70 + 4 x slot (KR m_cardSlots[slot - 1]), then the slot's
// entry of the equipped-card list (JP +0x37ec, KR m_cardPeriodList[0])
// replaced. Kind 0xCC (0x5891c0, none 0x1E8556): the 23 bytes after record
// +0x54 into club set +0xaa..+0xc0, past the KR sItemInfo (0xa8 bytes): no
// KR store. Any other kind: 0.
uint32_t JpClubSet_ApplyItemChange(unsigned char kind, uint32_t typeId, uint32_t serial, const unsigned short stats[5], unsigned char slot, uint32_t cardTypeId, const unsigned char extra[23]);
// 0x588d60 (kind 0xCB on a part; none 0x1E8551, slot 0 0x1E8552): as the
// club set's card slot, on the part found by its serial (0x588230).
uint32_t JpPart_SetCardSlot(uint32_t typeId, uint32_t serial, unsigned char slot, uint32_t cardTypeId);
// 0x589460 (kind 0xCE; none 0x1E856F): record +0x74 byte into caddie +0xc
// (KR sCaddieInfo::level), +0x78 dword into +0xd (KR experience).
uint32_t JpCaddie_ApplyItemChange(uint32_t serial, unsigned char level, uint32_t experience);

#endif // PG_JP_JPITEMGLUE_H
