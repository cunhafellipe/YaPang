// Original: d:\build\custom\temp\code\client\projectg\uccmanager.h
#ifndef PG_CLIENT_PROJECTG_UCCMANAGER_H
#define PG_CLIENT_PROJECTG_UCCMANAGER_H

#include <windows.h>
#include <string.h>
#include <list>
#include <map>
#include <string>

#include "defines.h"
#include "shared/classdefine.h"
#include "bitmap.h"

// The common head of every IFF item record. Its original header is not
// identified yet and only the members this project's code uses are named.
// The size is the span the UCC display record clears (uccmanager.h:114).
struct IFF_ITEM_COMMON {
	unsigned char m_active;     // 0x00
	PADDING(0x01, 0x04);
	unsigned long m_typeId;     // 0x04
	char m_name[0x28];          // 0x08
	unsigned char m_level;      // 0x30, & 0x7F the level, bit 7 a flag
	char m_icon[0x28];          // 0x31
	PADDING(0x59, 0x5C);
	unsigned long m_price;      // 0x5C
	unsigned long m_discountPrice; // 0x60, replaces m_price in the fortune list when non-zero
	unsigned long m_refundPrice; // 0x64, "refund price" of FrRepayDlg (cookies or pang by m_flags0 & 1)
	unsigned char m_flags0;     // 0x68
	unsigned char m_flags1;     // 0x69
	unsigned char m_timeType;   // 0x6A
	UNKNOWN(0x6B, 0x70);
	SYSTEMTIME m_saleStart;     // 0x70
	SYSTEMTIME m_saleEnd;       // 0x80
};
CHECK_SIZE(IFF_ITEM_COMMON, 0x90);
CHECK_FIELD(IFF_ITEM_COMMON, m_active, 0x00);
CHECK_FIELD(IFF_ITEM_COMMON, m_typeId, 0x04);
CHECK_FIELD(IFF_ITEM_COMMON, m_name, 0x08);
CHECK_FIELD(IFF_ITEM_COMMON, m_level, 0x30);
CHECK_FIELD(IFF_ITEM_COMMON, m_icon, 0x31);
CHECK_FIELD(IFF_ITEM_COMMON, m_price, 0x5C);
CHECK_FIELD(IFF_ITEM_COMMON, m_discountPrice, 0x60);
CHECK_FIELD(IFF_ITEM_COMMON, m_refundPrice, 0x64);
CHECK_FIELD(IFF_ITEM_COMMON, m_flags0, 0x68);
CHECK_FIELD(IFF_ITEM_COMMON, m_flags1, 0x69);
CHECK_FIELD(IFF_ITEM_COMMON, m_timeType, 0x6A);
CHECK_FIELD(IFF_ITEM_COMMON, m_saleStart, 0x70);
CHECK_FIELD(IFF_ITEM_COMMON, m_saleEnd, 0x80);

// A UCC (user-created) texture set owned by an item: the common record of
// the UCC kinds (sUccClothes).
struct sUccItem {
	// uccmanager.h:32
	sUccItem() {
		m_typeId = 0;
		m_texture[0] = 0;
		m_texture[1] = 0;
		m_texture[2] = 0;
		m_downloadCount = 0;
		m_pIcon = 0;
		m_uccKey[0] = 0;
		m_uccName[0] = 0;
	}

	unsigned long m_serial;         // 0x00, item serial (-1: none)
	char m_uccKey[9];               // 0x04
	PADDING(0x0D, 0x10);
	unsigned long m_typeId;         // 0x10
	char m_uccName[0x29];           // 0x14
	PADDING(0x3D, 0x40);
	unsigned long m_texture[3];     // 0x40, cleared by the constructor; use not seen yet
	unsigned long m_period[4];      // 0x4C, copied from sItemInfo 0x18 (CTask::OnPacketCommon 0x71)
	const Bitmap* m_pIcon;          // 0x5C, the icon GetUccItemIcon returns
	unsigned char m_downloadCount;  // 0x60
	PADDING(0x61, 0x64);
};
CHECK_SIZE(sUccItem, 0x64);
CHECK_FIELD(sUccItem, m_serial, 0x00);
CHECK_FIELD(sUccItem, m_uccKey, 0x04);
CHECK_FIELD(sUccItem, m_typeId, 0x10);
CHECK_FIELD(sUccItem, m_uccName, 0x14);
CHECK_FIELD(sUccItem, m_texture, 0x40);
CHECK_FIELD(sUccItem, m_period, 0x4C);
CHECK_FIELD(sUccItem, m_pIcon, 0x5C);
CHECK_FIELD(sUccItem, m_downloadCount, 0x60);

// A UCC clothes texture set.
struct sUccClothes : sUccItem {
	// uccmanager.h:47
	sUccClothes() {
	}
};
CHECK_SIZE(sUccClothes, 0x64);

// A UCC aztec (ball/accessory) texture, same shape as sUccClothes.
struct sUccAztec {
	unsigned long m_serial;         // 0x00
	char m_uccKey[9];               // 0x04
	PADDING(0x0D, 0x10);
	unsigned long m_typeId;         // 0x10
	UNKNOWN(0x14, 0x5C);
	const Bitmap* m_pIcon;          // 0x5C, the icon _RealDownLoad attaches
	unsigned char m_downloadCount;  // 0x60
	PADDING(0x61, 0x64);
};
CHECK_SIZE(sUccAztec, 0x64);
CHECK_FIELD(sUccAztec, m_serial, 0x00);
CHECK_FIELD(sUccAztec, m_uccKey, 0x04);
CHECK_FIELD(sUccAztec, m_typeId, 0x10);
CHECK_FIELD(sUccAztec, m_pIcon, 0x5C);
CHECK_FIELD(sUccAztec, m_downloadCount, 0x60);

// A UCC texture previewed before it is saved (two bitmaps).
struct sUccTempClothes {
	// uccmanager.h:84
	sUccTempClothes() {
		m_typeId = 0;
		memset(m_uccKey, 0, sizeof(m_uccKey));
		m_pFront = new Bitmap;
		m_pBack = new Bitmap;
	}

	unsigned long m_typeId;         // 0x00
	char m_uccKey[9];               // 0x04
	PADDING(0x0D, 0x10);
	Bitmap *m_pFront;               // 0x10
	Bitmap *m_pBack;                // 0x14
};
CHECK_SIZE(sUccTempClothes, 0x18);
CHECK_FIELD(sUccTempClothes, m_typeId, 0x00);
CHECK_FIELD(sUccTempClothes, m_uccKey, 0x04);
CHECK_FIELD(sUccTempClothes, m_pFront, 0x10);
CHECK_FIELD(sUccTempClothes, m_pBack, 0x14);

// A UCC clothes shown on a display (ranking) character: a part record the
// shop lists like an IFF part ("UCCDISPLAY" icon), followed by the UCC ids.
struct sDisplayUccClothes {
	// uccmanager.h:101
	sDisplayUccClothes() {
		m_typeId = 0;
		m_serial = 0;
		m_rank = 0;

		m_part.hideMask = 0;
		m_part.meshHideMask = 0;
		m_part.m_charCardSlots = 0;
		m_part.m_caddieCardSlots = 0; m_part.slot = 2;
		memset(m_part.m_baseStat, 0, sizeof(m_part.m_baseStat));
		memset(m_part.m_slotStat, 0, sizeof(m_part.m_slotStat));

		memset(&m_part, 0, sizeof(IFF_ITEM_COMMON));
		strcpy(m_part.m_icon, "UCCDISPLAY");






		m_part.m_flags1 &= 0xFC; m_part.m_typeId = 0; m_part.m_active = 1; m_part.m_flags = (m_part.m_flags & 0x1E) | 0x81; m_part.m_price = 1000000;
	}

	IFF_STRUCT::sPart m_part;       // 0x000, the part record the shop lists (its head cleared like an IFF_ITEM_COMMON)
	unsigned long m_typeId;         // 0x204
	unsigned long m_serial;         // 0x208
	std::string m_uccKey;           // 0x20C
	int m_rank;                     // 0x228
};
CHECK_SIZE(sDisplayUccClothes, 0x22C);
CHECK_FIELD(sDisplayUccClothes, m_part, 0x000);
CHECK_FIELD(sDisplayUccClothes, m_typeId, 0x204);
CHECK_FIELD(sDisplayUccClothes, m_serial, 0x208);
CHECK_FIELD(sDisplayUccClothes, m_uccKey, 0x20C);
CHECK_FIELD(sDisplayUccClothes, m_rank, 0x228);

// An item whose UCC texture must be refreshed.
struct sUccRefreshItem {
	sUccRefreshItem(void);          // uccmanager.h:70

	unsigned long m_typeId;         // 0x00
	char m_uccKey[9];               // 0x04
	PADDING(0x0D, 0x10);
};
CHECK_SIZE(sUccRefreshItem, 0x10);
CHECK_FIELD(sUccRefreshItem, m_typeId, 0x00);
CHECK_FIELD(sUccRefreshItem, m_uccKey, 0x04);

// golfplayer.obj emits the sUccRefreshItem constructor out of line:
// golfplayer.cpp defines GOLFPLAYER_UCCMANAGER_INLINE empty before its
// includes and gets the out-of-line copy from here; every other unit inlines it.
#ifndef GOLFPLAYER_UCCMANAGER_INLINE
#define GOLFPLAYER_UCCMANAGER_INLINE inline
#endif

// uccmanager.h:70
GOLFPLAYER_UCCMANAGER_INLINE sUccRefreshItem::sUccRefreshItem(void) {
	m_typeId = 0;
	memset(m_uccKey, 0, sizeof(m_uccKey));
}

class CUccManager {
	friend class NetResourceManager; // _RealDownLoad fills m_iconMap
	friend class FrUccDrawDlg;       // Upload inserts the new icon into m_iconMap
	friend class CShopMain;          // Build_Parts lists m_displayClothesList
	friend class CRealMyRoomMain;    // HandleMsg 0x214 looks the new icon up in m_iconMap
public:
	CUccManager(void);
	~CUccManager(void);

	void AddClothes(unsigned char slot, unsigned long serial, unsigned long typeId, const char *uccKey, bool send, bool request);
	void AddClothes(unsigned char slot, sUccClothes &clothes, bool send, bool request);
	void AddAztec(sUccAztec &aztec, bool request);
	sUccClothes *FindClothes(unsigned long typeId, const char *uccKey);
	sUccAztec *FindAztec(unsigned long typeId, const char *uccKey);
	sUccClothes *FindClothes(unsigned long serial);
	sUccAztec *FindAztec(unsigned long serial);
	bool IsDownloadComplete(unsigned long typeId, const char *uccKey);
	bool SetDownloadComplete(unsigned long typeId, const char *uccKey);
	bool IsDownloadComplete(sUccClothes *clothes);
	bool IsDownloadComplete(sUccAztec *aztec);
	bool SetDownloadComplete(sUccClothes *clothes);
	bool SetDownloadComplete(sUccAztec *aztec);
	void Clear(void);
	void ClearTexture(void);
	void ClearIcon(void);
	void ClearTempClothes(void);
	void AddRefreshItem(unsigned long typeId, const char *uccKey);
	bool GetRefreshItem(sUccRefreshItem &item);
	void SetUccTempClothes(unsigned long typeId, const char *uccKey, Bitmap *front, Bitmap *back);
	bool GetUccTempClothes(unsigned long typeId, const char *uccKey, Bitmap &front, Bitmap &back);
	void AddDisplayClothes(unsigned long typeId, unsigned long serial, std::string uccKey, int rank);
	void BuildDisplayClothesSet(void);
	bool IsDisplayUccClothes(const IFF_ITEM_COMMON *item);
	unsigned long GetWinnerCharacterTypeId(void);

	// uccmanager.h:142
	void ClearClothes(void) { m_clothesList.clear(); }
	// uccmanager.h:143
	void ClearAztec(void) { m_aztecList.clear(); }

private:
	sUccClothes *_GetClothes(unsigned long serial);
	sUccAztec *_GetAztec(unsigned long serial);
	void RequestClothes(unsigned long typeId, const char *uccKey, bool force);
	void RequestAztec(unsigned long typeId, const char *uccKey, bool force);

	LAYOUT_CHECK_FRIEND;
	std::list<sUccClothes> m_clothesList;               // 0x00
	std::list<sUccAztec> m_aztecList;                   // 0x0C
	std::map<std::string, Bitmap *> m_iconMap;          // 0x18
	std::list<sUccTempClothes> m_tempClothesList;       // 0x24
	std::list<sDisplayUccClothes> m_displayClothesList; // 0x30
	std::list<sUccRefreshItem> m_refreshList;           // 0x3C
	CRITICAL_SECTION m_refreshLock;                     // 0x48
};
CHECK_SIZE(CUccManager, 0x60);
BEGIN_LAYOUT_CHECK(CUccManager)
	CHECK_FIELD(CUccManager, m_clothesList, 0x00);
	CHECK_FIELD(CUccManager, m_aztecList, 0x0C);
	CHECK_FIELD(CUccManager, m_iconMap, 0x18);
	CHECK_FIELD(CUccManager, m_tempClothesList, 0x24);
	CHECK_FIELD(CUccManager, m_displayClothesList, 0x30);
	CHECK_FIELD(CUccManager, m_refreshList, 0x3C);
	CHECK_FIELD(CUccManager, m_refreshLock, 0x48);
END_LAYOUT_CHECK

CUccManager *UccManager(void);

#endif // PG_CLIENT_PROJECTG_UCCMANAGER_H
