// JP-only CSharedDoc state of the shop (no KR counterpart), kept as port
// storage: the KR CSharedDoc of the port has no room for the JP members and
// no friend for JP glue. Bodies: JpShopGlue.cpp. KR callers under JP_PAKS
// (TaskManager.cpp) declare the functions they call instead of including
// this header.
#ifndef PG_JP_JPSHOPGLUE_H
#define PG_JP_JPSHOPGLUE_H

// The shop-entry pairs of the 0x20E reply (TaskManager.cpp), a std::map at
// JP CSharedDoc +0x113e0 keyed by first (meaning not known).
// 0x9be750: clears the map (0x63d250 on +0x113e0).
void JpSharedDoc_ClearShopEnterPairs(void);
// 0x642990: inserts (first, second) unless first is 0; an existing key
// keeps its value (std::map::insert with a lower_bound hint, 0x640020 /
// 0x63cfe0).
void JpSharedDoc_AddShopEnterPair(unsigned int first, unsigned int second);

// JP CSharedDoc +0x106fc (a byte): set to 1 after a successful purchase
// (CShopWidget::OnPacket 0x5e0ff0, packet 0x68 result 0).
void JpSharedDoc_SetShopPurchased(bool purchased);

#endif
