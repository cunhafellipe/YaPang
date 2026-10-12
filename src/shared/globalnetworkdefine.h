// Original: d:\build\custom\temp\code\shared\globalnetworkdefine.h
#ifndef PG_SHARED_GLOBALNETWORKDEFINE_H
#define PG_SHARED_GLOBALNETWORKDEFINE_H

#include "defines.h"
#include "padding.h"
#include "shared/globalgamedefine.h"

#include <string.h>

// Network structures are byte-packed (sTradeItem is 0xAA bytes, its words and
// dwords sit at odd offsets).
#pragma pack(push, 1)

// One item of a purchase reply (packets 0x67 and 0x79 result 5, 0x26 bytes).
// The type id and the guid are sent to the RealMyRoom actor; the names past
// `guid` are inferred from the member sizes (the period, flag and count words
// of an item, a 16-byte end date, the 9-byte UCC index as in sTradeItem).
struct sBuyItemResult {
	// globalnetworkdefine.h:2059
	sBuyItemResult(void) {
		typeId = 0;
		guid = 0;
		period = 0;
		flag = 0;
		count = 0;
		memset(&endDate, 0, sizeof(endDate));
		memset(uccIdx, 0, sizeof(uccIdx));
	}

	unsigned long typeId;         // 0x00, its group is typeId >> 26
	unsigned long guid;           // 0x04
	unsigned short period;        // 0x08
	unsigned char flag;           // 0x0A
	unsigned short count;         // 0x0B
	SYSTEMTIME endDate;           // 0x0D
	char uccIdx[9];               // 0x1D
};
CHECK_SIZE(sBuyItemResult, 0x26);
CHECK_FIELD(sBuyItemResult, typeId, 0x00);
CHECK_FIELD(sBuyItemResult, guid, 0x04);
CHECK_FIELD(sBuyItemResult, period, 0x08);
CHECK_FIELD(sBuyItemResult, flag, 0x0A);
CHECK_FIELD(sBuyItemResult, count, 0x0B);
CHECK_FIELD(sBuyItemResult, endDate, 0x0D);
CHECK_FIELD(sBuyItemResult, uccIdx, 0x1D);

struct sTradeItem {
	sTradeItem(void);

	// Every member is cleared by the constructor; the names past `price`
	// are inferred from the sizes (UCC data of a self-design item).
	unsigned long index;          // 0x00
	unsigned long typeId;         // 0x04
	unsigned long guid;           // 0x08
	unsigned long count;          // 0x0C
	unsigned char kind;           // 0x10
	unsigned short flag;          // 0x11
	__int64 price;                // 0x13, pang (summed by SumForTradeItemList)
	unsigned long period;         // 0x1B
	unsigned short stats[5];      // 0x1F
	unsigned short statFlag;      // 0x29
	char uccIdx[9];               // 0x2B
	unsigned short uccSeq;        // 0x34
	unsigned char uccStatus;      // 0x36
	union {                       // 0x37
		char uccData[0x30];
		// The type ids of the cards put on a part: character card slots
		// first, caddie cards from index 4 (TransferableItem).
		unsigned long m_cardTypeIds[12];
	};
	unsigned short m_charCardSlotCount;    // 0x67, card slots of a character part (not cleared by the constructor)
	unsigned short m_caddieCardSlotCount;  // 0x69, card slots of a caddie part (not cleared by the constructor)
	char uccName[0x29];           // 0x6B
	char uccCopierNick[0x16];     // 0x94
};
CHECK_SIZE(sTradeItem, 0xAA);
CHECK_FIELD(sTradeItem, index, 0x00);
CHECK_FIELD(sTradeItem, typeId, 0x04);
CHECK_FIELD(sTradeItem, guid, 0x08);
CHECK_FIELD(sTradeItem, count, 0x0C);
CHECK_FIELD(sTradeItem, kind, 0x10);
CHECK_FIELD(sTradeItem, flag, 0x11);
CHECK_FIELD(sTradeItem, price, 0x13);
CHECK_FIELD(sTradeItem, period, 0x1B);
CHECK_FIELD(sTradeItem, stats, 0x1F);
CHECK_FIELD(sTradeItem, statFlag, 0x29);
CHECK_FIELD(sTradeItem, uccIdx, 0x2B);
CHECK_FIELD(sTradeItem, uccSeq, 0x34);
CHECK_FIELD(sTradeItem, uccStatus, 0x36);
CHECK_FIELD(sTradeItem, uccData, 0x37);
CHECK_FIELD(sTradeItem, m_cardTypeIds, 0x37);
CHECK_FIELD(sTradeItem, m_charCardSlotCount, 0x67);
CHECK_FIELD(sTradeItem, m_caddieCardSlotCount, 0x69);
CHECK_FIELD(sTradeItem, uccName, 0x6B);
CHECK_FIELD(sTradeItem, uccCopierNick, 0x94);

// shareddoc.cpp defines SHAREDDOC_GLOBALNETWORKDEFINE_INLINE empty before its
// includes: the original holds the out-of-line copy of the constructor in
// shareddoc's object.
#ifndef SHAREDDOC_GLOBALNETWORKDEFINE_INLINE
#define SHAREDDOC_GLOBALNETWORKDEFINE_INLINE inline
#endif

// globalnetworkdefine.h:2481
SHAREDDOC_GLOBALNETWORKDEFINE_INLINE sTradeItem::sTradeItem(void) {
	index = 0;
	typeId = 0;
	guid = 0;
	count = 0;
	kind = 0;
	flag = 0;
	price = 0;
	period = 0;
	memset(stats, 0, sizeof(stats));
	statFlag = 0;

	memset(uccIdx, 0, sizeof(uccIdx));
	uccSeq = 0;
	uccStatus = 0;

	memset(uccData, 0, sizeof(uccData));
	memset(uccName, 0, sizeof(uccName));
	memset(uccCopierNick, 0, sizeof(uccCopierNick));
}

// The accessors are defined in the class body in the original (lines
// 2981-2988); the units that emitted them define them out of line.
struct sStoredItemInfo {
	unsigned long GetTypeID(void) const;
	void SetTypeID(unsigned long);
	unsigned long GetGuid(void) const;
	void SetGuid(unsigned long);
	unsigned long GetCount(void) const;
	void SetCount(unsigned long);
	sStoredItemInfo& operator=(const sStoredItemInfo& rhs);   // globalnetworkdefine.h:2991

	unsigned long slot; // 0x00
	sTradeItem item;    // 0x04
};
CHECK_SIZE(sStoredItemInfo, 0xAE);
CHECK_FIELD(sStoredItemInfo, slot, 0x00);
CHECK_FIELD(sStoredItemInfo, item, 0x04);

// One slot of a personal shop (stall).
struct sStallSlot {
	// globalnetworkdefine.h:2807
	sStallSlot(void) {
		clear();
	}

	// globalnetworkdefine.h:2813
	void clear(void) {
		index = 0;
		used = 0;
		memset(&item, 0, sizeof(item));
		memset(buyerNick, 0, sizeof(buyerNick));
		sold = 0;
		reserved = 0;
	}

	unsigned long index;          // 0x00
	unsigned char used;           // 0x04
	sTradeItem item;              // 0x05
	union {                       // 0xAF
		char buyerNick[0x10];
		SYSTEMTIME expireTime;    // the visitor's slots (OfflineTradeDlg)
	};
	unsigned char sold;           // 0xBF
	unsigned char reserved;       // 0xC0
};
CHECK_SIZE(sStallSlot, 0xC1);
CHECK_FIELD(sStallSlot, index, 0x00);
CHECK_FIELD(sStallSlot, used, 0x04);
CHECK_FIELD(sStallSlot, item, 0x05);
CHECK_FIELD(sStallSlot, buyerNick, 0xAF);
CHECK_FIELD(sStallSlot, expireTime, 0xAF);
CHECK_FIELD(sStallSlot, sold, 0xBF);
CHECK_FIELD(sStallSlot, reserved, 0xC0);

// A personal shop (stall): its owner state and twelve slots.
struct sStall {
	sStall(void);

	// globalnetworkdefine.h:2843
	void SlotClear(void) {
		memset(slots, 0, sizeof(slots));
	}

	// globalnetworkdefine.h:2848
	void AllClear(void) {
		ownerUID = 0;
		open = 0;
		visible = 0;
		memset(name, 0, sizeof(name));
		state = 0;
		SlotClear();
	}

	unsigned long uid;            // 0x00, the shop id (OfflineTradeDlg SendOfflineTradeVisitor)
	unsigned long ownerUID;       // 0x04
	unsigned char state;          // 0x08
	unsigned char open;           // 0x09
	unsigned char visible;        // 0x0A
	sStallSlot slots[12];         // 0x0B
	char name[0x10];              // 0x917
};
CHECK_SIZE(sStall, 0x927);
CHECK_FIELD(sStall, uid, 0x00);
CHECK_FIELD(sStall, ownerUID, 0x04);
CHECK_FIELD(sStall, state, 0x08);
CHECK_FIELD(sStall, open, 0x09);
CHECK_FIELD(sStall, visible, 0x0A);
CHECK_FIELD(sStall, slots, 0x0B);
CHECK_FIELD(sStall, name, 0x917);

#pragma pack(pop)

#endif // PG_SHARED_GLOBALNETWORKDEFINE_H
