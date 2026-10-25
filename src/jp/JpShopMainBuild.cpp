// JP-only game code: the JP CShopMain's item list builders (JpShopMain,
// JpShopMain.h), from the JP exe (node scripts/disasm.ts --jp 0x<va>,
// node scripts/jp_hlil.ts 0x<va>). BuildDisplayStand calls one per sub tab.
//
// The JP builders are not the KR ones (yapang/src/client/ProjectG/
// ShopMain.cpp): they walk the JP item manager's maps (the JP CSharedDoc
// 0xe47dbc +0x20c; map n at +0x20 + 0x10 * n, the KR maps' order: +0x20
// characters, +0x30 parts, +0x50 club sets, +0x60 balls, +0x70 items, +0x90
// caddie items, +0xa0 set items, +0x110 hair styles, +0x130 mascots, +0x170
// furniture, +0x1b0 cards), here the port's KR CItemManager maps; they hide
// the items out of the player's level range (0x9d8a80), list the items
// whose JP header dword +0x9c is 2 even without a shop flag or sale period,
// and sort with the JP compares (0xa85400, 0xa82470).
//
// The JP records are the KR ones with a longer name (0x18 bytes) and the
// 2016 insert at 0x84 (cmake/iff_us.cpp cuts both off): JP +0x48 is the KR
// m_level (0x30), +0x80/+0x81 the KR m_flags0/m_flags1 (0x68/0x69), +0x9c
// the KR u32 at 0x6c (unnamed in IFF_ITEM_COMMON).
//
// Same functions as KR (called): 0xa18780 CItemManager::IsInSalePeriod,
// 0x6333b0 CSharedDoc::GetServerTime, 0xa8c110 AddSellMiniButton, 0xa82460
// ShopEmptyCompare, 0x5b6500 FrListItem::AddNoRBtnIdx (inline).
// lib: 0xa908c0 (std::map<unsigned int, IFF_STRUCT::sChar>::operator[] on
// the character map), 0x9eb9b0 (the set item map's iterator ++)
//
// Not written yet: Build_Parts 0xa8e210 (also walks a JP-only CSharedDoc
// list at +0x1147c, sort 0xa89000), Build_Decorate 0xa8f390 and Build_Cutin
// 0xa8f520 (the KR m_skinMap is private to CItemManager: friend CShopMain
// only), Build_Hair 0xa8e0b0 (needs the IsItemInCart 0xa88c00 member).
#include <string.h>

#include "JpShopMain.h"
#include "ItemManager.h"
#include "SharedDoc.h"
#include "UccManager.h"
#include "frlistbox.h"

// KR ShopMain.cpp's free functions (the same JP code).
void AddSellMiniButton(IFF_ITEM_COMMON &item, FrListItem *&listItem);   // 0xa8c110
bool ShopEmptyCompare(const void *lhs, const void *rhs);                // 0xa82460

namespace {

// The JP header dword +0x9c (the KR record's u32 at 0x6c, not named in
// IFF_ITEM_COMMON): 2 lists an item without a shop flag or sale period.
uint32_t GetListKind(const IFF_ITEM_COMMON &item) {
	uint32_t kind;
	memcpy(&kind, reinterpret_cast<const unsigned char *>(&item) + 0x6c, sizeof(kind));
	return kind;
}

// The player's level (JP CSharedDoc byte +0x91d: the KR m_myLevel).
unsigned int GetMyLevel(void) {
	return WSingleton<CSharedDoc>::Instance()->m_myLevel;
}

bool IsInSalePeriod(unsigned long typeId) {
	return ItemManager()->IsInSalePeriod(typeId, WSingleton<CSharedDoc>::Instance()->GetServerTime());
}

} // namespace

// 0x9d8a80 (JP item manager unit; name ours): an item of shop flag 3 out of
// the player's level range. Its level byte's bit 7 set: the level & 0x7f is
// the highest level it is sold to; clear: it is sold up to one level below
// it.
static bool IsOutOfShopLevel(const IFF_ITEM_COMMON &item, const unsigned int *level) {
	if ((item.m_flags0 & 0xE0) == 0x60) {
		unsigned int itemLevel = item.m_level & 0x7F;
		if ((item.m_level & 0x80) != 0) {
			if (itemLevel < *level) {
				return true;
			}
		} else if (itemLevel > *level + 1) {
			return true;
		}
	}
	return false;
}

// 0xa1f570 (a JP CItemManager member, on the item manager; name ours): a
// record sold now, the KR Build_Caddie / Build_Mascot test (shop flag not
// 0, 5 or 6, in its sale period), or listed (+0x9c == 2).
static bool IsOnSale(const IFF_ITEM_COMMON &item, _SYSTEMTIME &time) {
	unsigned char shopFlag = item.m_flags0 >> 5;
	bool inSale = false;
	if (shopFlag != 0 && shopFlag != 5 && shopFlag != 6) {
		inSale = ItemManager()->IsInSalePeriod(item.m_typeId, time);
	}
	if ((shopFlag == 0 || shopFlag == 5 || shopFlag == 6 || !inSale) && GetListKind(item) != 2) {
		return false;
	}
	return true;
}

// 0xa82470 (name ours): the caddie list's order: the listed items (+0x9c ==
// 2) first. The original's last test, (left m_flags1 & 1) != 2, is always
// true: two listed items, or a listed right one, compare false.
static bool CaddieListCompare(const void *lhs, const void *rhs) {
	const IFF_ITEM_COMMON *left = (const IFF_ITEM_COMMON *)lhs;
	const IFF_ITEM_COMMON *right = (const IFF_ITEM_COMMON *)rhs;

	if (GetListKind(*left) == 2) {
		if (GetListKind(*right) != 2) {
			return true;
		}
		return false;
	}
	if (GetListKind(*right) == 2) {
		return false;
	}
	return left->m_typeId < right->m_typeId;
}

// 0xa85400 (the KR SpecialHitNewCompare with the JP listed items and level
// tests; name ours): items with a sale period first (a listed item's only
// while it runs), dated before undated; then m_flags1 bit 0, then bit 1;
// then by level range; then by type id, highest first. A listed right item
// in its sale period compares false.
static bool JpSpecialHitNewCompare(const void *lhs, const void *rhs) {
	const IFF_ITEM_COMMON *left = (const IFF_ITEM_COMMON *)lhs;
	const IFF_ITEM_COMMON *right = (const IFF_ITEM_COMMON *)rhs;

	bool rightListed = false;
	if (GetListKind(*right) == 2) {
		rightListed = IsInSalePeriod(right->m_typeId);
	}
	if (GetListKind(*right) == 2 && rightListed) {
		return false;
	}

	_SYSTEMTIME *leftBegin = NULL, *leftEnd = NULL, *rightBegin = NULL, *rightEnd = NULL;
	bool leftHasPeriod = ItemManager()->HasSalePeriod(left->m_typeId, &leftBegin, &leftEnd);
	bool rightHasPeriod = ItemManager()->HasSalePeriod(right->m_typeId, &rightBegin, &rightEnd);

	if (leftHasPeriod && GetListKind(*left) == 2 && !IsInSalePeriod(left->m_typeId)) {
		leftHasPeriod = false;
	}
	if (rightHasPeriod && GetListKind(*right) == 2 && !IsInSalePeriod(right->m_typeId)) {
		rightHasPeriod = false;
	}

	if (leftHasPeriod || rightHasPeriod) {
		if (!leftHasPeriod) {
			return false;
		}
		if (!rightHasPeriod) {
			return true;
		}
		bool leftDated = leftBegin->wYear != 0 || leftEnd->wYear != 0;
		bool rightDated = rightBegin->wYear != 0 || rightEnd->wYear != 0;
		if (leftDated && !rightDated) {
			return true;
		}
		if (!leftDated && rightDated) {
			return false;
		}
	}

	bool leftHit = (left->m_flags1 & 1) != 0;
	if (leftHit && (right->m_flags1 & 1) == 0) {
		return true;
	}
	if ((right->m_flags1 & 1) != 0 && !leftHit) {
		return false;
	}
	bool leftNew = ((left->m_flags1 >> 1) & 1) != 0;
	if (leftNew && (right->m_flags1 & 2) == 0) {
		return true;
	}
	if ((right->m_flags1 & 2) != 0 && !leftNew) {
		return false;
	}

	unsigned int myLevel = GetMyLevel();
	if ((left->m_level & 0x80) != 0) {
		if ((right->m_level & 0x80) == 0 && (unsigned int)(right->m_level & 0x7F) > myLevel) {
			return true;
		}
		if ((unsigned int)(left->m_level & 0x7F) < myLevel) {
			return false;
		}
		return left->m_typeId > right->m_typeId;
	}
	if ((unsigned int)(left->m_level & 0x7F) > myLevel) {
		return false;
	}
	if ((right->m_level & 0x80) != 0 && (unsigned int)(right->m_level & 0x7F) < myLevel) {
		return true;
	}
	return left->m_typeId > right->m_typeId;
}

// 0xb2e230 (fastcall, the sub tab in ecx; name ours): the KR CTaskMain::
// GetCategoryFlag with the JP sub tab numbers (a jump table over 1..35).
static unsigned long GetCategoryFlag(int tab) {
	switch (tab) {
	case 2:
	case 15:
		return 1;
	case 3:
	case 16:
		return 2;
	case 4:
		return 0x10;
	case 5:
		return 0x2C;
	case 6:
		return 0x40;
	case 17:
		return 3;
	case 22:
		return 0x3BF;
	case 27:
		return 0x380;
	case 29:
		return 7;
	case 30:
		return 9;
	case 31:
		return 8;
	case 35:
		return 6;
	}
	return 0;
}

// 0xa8df90: the default tab (0): every character sold, listed or in the
// player's level range (the KR one lists the shown character and its sets).
void JpShopMain::Build_Package(subTab_t, int) {
	typedef std::map<unsigned int, IFF_STRUCT::sChar> CharMap;
	for (CharMap::iterator it = ItemManager()->m_charMap.begin(); it != ItemManager()->m_charMap.end(); ++it) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
		unsigned int level = GetMyLevel();
		if (!IsOutOfShopLevel(item, &level)) {
			if (((item.m_flags0 & 0xE0) != 0 && IsInSalePeriod(item.m_typeId)) || GetListKind(item) == 2) {
				FrListItem *listItem = m_itemListBox->AddItem(&item);
				AddSellMiniButton(item, listItem);
			}
		}
	}

	m_itemListBox->SortItem(ShopEmptyCompare);
}

// 0xa8e520
void JpShopMain::Build_Clubset(subTab_t, int) {
	FrListItem *listItem;

	typedef std::map<unsigned int, IFF_STRUCT::sClubSet> ClubSetMap;
	for (ClubSetMap::iterator it = ItemManager()->m_clubSetMap.begin(); it != ItemManager()->m_clubSetMap.end(); ++it) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
		unsigned int level = GetMyLevel();
		if (!IsOutOfShopLevel(item, &level)) {
			if (((item.m_flags0 & 0xE0) != 0 && IsInSalePeriod(item.m_typeId)) || GetListKind(item) == 2) {
				listItem = m_itemListBox->AddItem(&item);
				AddSellMiniButton(item, listItem);
			}
		}
	}

	typedef std::map<unsigned int, IFF_STRUCT::sSetItem> SetItemMap;
	for (SetItemMap::iterator it2 = ItemManager()->m_setItemMap.begin(); it2 != ItemManager()->m_setItemMap.end(); ++it2) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it2->second;
		unsigned int level = GetMyLevel();
		if (!IsOutOfShopLevel(item, &level) && (item.m_flags0 & 0xE0) != 0 && (item.m_typeId & 0x3E00000) == 0x800000
			&& IsInSalePeriod(item.m_typeId)) {
			listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	m_itemListBox->SortItem(ShopEmptyCompare);
	m_itemListBox->SortItem(JpSpecialHitNewCompare);
}

// 0xa8e750
void JpShopMain::Build_Aztec(subTab_t, int) {
	FrListItem *listItem;

	typedef std::map<unsigned int, IFF_STRUCT::sBall> BallMap;
	for (BallMap::iterator it = ItemManager()->m_ballMap.begin(); it != ItemManager()->m_ballMap.end(); ++it) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
		unsigned int level = GetMyLevel();
		if (!IsOutOfShopLevel(item, &level) && (item.m_flags0 & 0xE0) != 0 && IsInSalePeriod(item.m_typeId)) {
			listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	typedef std::map<unsigned int, IFF_STRUCT::sSetItem> SetItemMap;
	for (SetItemMap::iterator it2 = ItemManager()->m_setItemMap.begin(); it2 != ItemManager()->m_setItemMap.end(); ++it2) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it2->second;
		if ((item.m_flags0 & 0xE0) != 0 && (item.m_typeId & 0x3E00000) == 0xA00000 && IsInSalePeriod(item.m_typeId)) {
			listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	m_itemListBox->SortItem(ShopEmptyCompare);
	m_itemListBox->SortItem(JpSpecialHitNewCompare);
}

// 0xa8e950
void JpShopMain::Build_Equip(subTab_t, int) {
	FrListItem *listItem;

	typedef std::map<unsigned int, IFF_STRUCT::sItem> ItemMap;
	for (ItemMap::iterator it = ItemManager()->m_itemMap.begin(); it != ItemManager()->m_itemMap.end(); ++it) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
		unsigned int level = GetMyLevel();
		if (!IsOutOfShopLevel(item, &level) && (item.m_flags0 & 0xE0) != 0 && !(item.m_typeId & 0x2000000)
			&& IsInSalePeriod(item.m_typeId)) {
			listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	typedef std::map<unsigned int, IFF_STRUCT::sSetItem> SetItemMap;
	for (SetItemMap::iterator it2 = ItemManager()->m_setItemMap.begin(); it2 != ItemManager()->m_setItemMap.end(); ++it2) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it2->second;
		if ((item.m_flags0 & 0xE0) != 0 && (item.m_typeId & 0x3E00000) == 0xC00000 && (item.m_typeId & 0x1FE000) == 0x1FC000
			&& IsInSalePeriod(item.m_typeId)) {
			listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	m_itemListBox->SortItem(ShopEmptyCompare);
	m_itemListBox->SortItem(JpSpecialHitNewCompare);
}

// 0xa8eb60: the KR one, without the GM-only items (kind 6, type 0x1f00000)
// for a player without the GM identity bits (JP CSharedDoc +0x812: the KR
// m_userInfo.identity) and without 0x24dfe00a. The original's guard bit of
// a function-local static (0xf02e28) has no object left to build.
void JpShopMain::Build_NoEquip(subTab_t, int) {
	typedef std::map<unsigned int, IFF_STRUCT::sItem> ItemMap;
	for (ItemMap::iterator it = ItemManager()->m_itemMap.begin(); it != ItemManager()->m_itemMap.end(); ++it) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
		if ((item.m_typeId & 0xFC000000) != 0x18000000 || (item.m_typeId & 0x1F00000) != 0x1F00000
			|| (WSingleton<CSharedDoc>::Instance()->m_userInfo.identity & 0x2804) != 0) {
			unsigned int level = GetMyLevel();
			if (!IsOutOfShopLevel(item, &level) && (item.m_flags0 & 0xE0) != 0 && (item.m_typeId & 0x2000000)
				&& IsInSalePeriod(item.m_typeId) && item.m_typeId != 0x24DFE00A) {
				FrListItem *listItem = m_itemListBox->AddItem(&item);
				unsigned char shopFlag = item.m_flags0 >> 5;
				if ((shopFlag == 1 || shopFlag == 3) && shopFlag == 7) {
					listItem->AddNoRBtnIdx(1);
				}
				shopFlag = item.m_flags0 >> 5;
				if ((shopFlag == 1 || shopFlag == 2) && (item.m_flags0 & 1) && shopFlag == 7) {
					listItem->AddNoRBtnIdx(2);
				}
			}
		}
	}

	typedef std::map<unsigned int, IFF_STRUCT::sSetItem> SetItemMap;
	for (SetItemMap::iterator it2 = ItemManager()->m_setItemMap.begin(); it2 != ItemManager()->m_setItemMap.end(); ++it2) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it2->second;
		if ((item.m_flags0 & 0xE0) != 0 && (item.m_typeId & 0x3E00000) == 0xC00000 && (item.m_typeId & 0x1FE000) == 0x1FE000
			&& IsInSalePeriod(item.m_typeId) && item.m_typeId != 0x24DFE00A) {
			FrListItem *listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	m_itemListBox->SortItem(ShopEmptyCompare);
	m_itemListBox->SortItem(JpSpecialHitNewCompare);
}

// 0xa8ee00
void JpShopMain::Build_Card(subTab_t, int) {
	FrListItem *listItem;

	typedef std::map<unsigned int, IFF_STRUCT::sCard> CardMap;
	for (CardMap::iterator it = ItemManager()->m_cardMap.begin(); it != ItemManager()->m_cardMap.end(); ++it) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
		unsigned int level = GetMyLevel();
		if (!IsOutOfShopLevel(item, &level)) {
			if (((item.m_flags0 & 0xE0) != 0 && !(item.m_typeId & 0x2000000) && IsInSalePeriod(item.m_typeId))
				|| GetListKind(item) == 2) {
				listItem = m_itemListBox->AddItem(&item);
				AddSellMiniButton(item, listItem);
			}
		}
	}

	typedef std::map<unsigned int, IFF_STRUCT::sSetItem> SetItemMap;
	for (SetItemMap::iterator it2 = ItemManager()->m_setItemMap.begin(); it2 != ItemManager()->m_setItemMap.end(); ++it2) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it2->second;
		if ((item.m_flags0 & 0xE0) != 0 && (item.m_typeId & 0x3E00000) == 0x1000000 && IsInSalePeriod(item.m_typeId)) {
			listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	m_itemListBox->SortItem(ShopEmptyCompare);
	m_itemListBox->SortItem(JpSpecialHitNewCompare);
}

// 0xa8f010
void JpShopMain::Build_Interior(subTab_t tab, int) {
	unsigned long category = GetCategoryFlag(tab);

	typedef std::map<unsigned int, IFF_STRUCT::sFurniture> FurnitureMap;
	for (FurnitureMap::iterator it = ItemManager()->m_furnitureMap.begin(); it != ItemManager()->m_furnitureMap.end(); ++it) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
		unsigned int level = GetMyLevel();
		if (!IsOutOfShopLevel(item, &level)) {
			if (((item.m_flags0 & 0xE0) != 0 && !(item.m_typeId & 0x2000000) && IsInSalePeriod(item.m_typeId))
				|| GetListKind(item) == 2) {
				if (((item.m_typeId >> 11) & 0x7FF) == category) {
					FrListItem *listItem = m_itemListBox->AddItem(&item);
					AddSellMiniButton(item, listItem);
				}
			}
		}
	}

	m_itemListBox->SortItem(ShopEmptyCompare);
	m_itemListBox->SortItem(JpSpecialHitNewCompare);
}

// 0xa8f170
void JpShopMain::Build_Furniture(subTab_t, int) {
	typedef std::map<unsigned int, IFF_STRUCT::sFurniture> FurnitureMap;
	for (FurnitureMap::iterator it = ItemManager()->m_furnitureMap.begin(); it != ItemManager()->m_furnitureMap.end(); ++it) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
		unsigned int level = GetMyLevel();
		if (!IsOutOfShopLevel(item, &level)) {
			if (((item.m_flags0 & 0xE0) != 0 && !(item.m_typeId & 0x2000000) && IsInSalePeriod(item.m_typeId))
				|| GetListKind(item) == 2) {
				int category = (item.m_typeId >> 11) & 0x7FF;
				if ((category >= 0 && category < 7) || category >= 10) {
					FrListItem *listItem = m_itemListBox->AddItem(&item);
					if (listItem) {
						unsigned char shopFlag = item.m_flags0 >> 5;
						if (shopFlag == 1 || shopFlag == 3)
							listItem->AddNoRBtnIdx(1);
						shopFlag = item.m_flags0 >> 5;
						if (shopFlag == 1 || shopFlag == 2)
							listItem->AddNoRBtnIdx(2);
					}
				}
			}
		}
	}

	m_itemListBox->SortItem(ShopEmptyCompare);
	m_itemListBox->SortItem(JpSpecialHitNewCompare);
}

// 0xa8f6a0: the caddies BuildVisibleCaddieList kept (the KR one walks the
// item manager's caddie map).
void JpShopMain::Build_Caddie(subTab_t, int) {
	typedef std::map<unsigned int, IFF_STRUCT::sCaddie> CaddieMap;
	for (CaddieMap::iterator it = m_visibleCaddies.begin(); it != m_visibleCaddies.end(); ++it) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
		if (IsOnSale(item, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
			FrListItem *listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	m_itemListBox->SortItem(CaddieListCompare);
	m_itemListBox->SortItem(JpSpecialHitNewCompare);
}

// 0xa8f790: the shown caddie's items (sub tab 12; the KR Build_Caddie's
// second loop, the caddie serial being the type id's bits 21.. plus 16..).
void JpShopMain::Build_CaddieItems(subTab_t, int) {
	typedef std::map<unsigned int, IFF_STRUCT::sCadItem> CadItemMap;
	for (CadItemMap::iterator it = ItemManager()->m_cadItemMap.begin(); it != ItemManager()->m_cadItemMap.end(); ++it) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
		if (((item.m_flags0 & 0xE0) != 0
				&& ((item.m_typeId >> 21) & 0x1F) + ((item.m_typeId >> 16) & 0x1F) == (m_caddieTypeId & 0x3FFFFFF)
				&& IsInSalePeriod(item.m_typeId))
			|| GetListKind(item) == 2) {
			FrListItem *listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	m_itemListBox->SortItem(CaddieListCompare);
	m_itemListBox->SortItem(JpSpecialHitNewCompare);
}

// 0xa8f8c0
void JpShopMain::Build_Mascot(subTab_t, int) {
	typedef std::map<unsigned int, IFF_STRUCT::sMascot> MascotMap;
	for (MascotMap::iterator it = ItemManager()->m_mascotMap.begin(); it != ItemManager()->m_mascotMap.end(); ++it) {
		IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
		unsigned int level = GetMyLevel();
		if (!IsOutOfShopLevel(item, &level) && IsOnSale(item, WSingleton<CSharedDoc>::Instance()->GetServerTime())) {
			FrListItem *listItem = m_itemListBox->AddItem(&item);
			AddSellMiniButton(item, listItem);
		}
	}

	m_itemListBox->SortItem(ShopEmptyCompare);
	m_itemListBox->SortItem(JpSpecialHitNewCompare);
}

// 0xa91d20: the shown character's sets (sub tab 1; the KR Build_Package's
// set loop, with the set kinds 1 and 2), when the character is sold.
void JpShopMain::Build_CharacterItems(subTab_t, int) {
	IFF_ITEM_COMMON &charItem = (IFF_ITEM_COMMON &)ItemManager()->m_charMap[m_standPetTypeId];
	if ((charItem.m_flags0 & 0xE0) != 0 && IsInSalePeriod(charItem.m_typeId)) {
		typedef std::map<unsigned int, IFF_STRUCT::sSetItem> SetItemMap;
		for (SetItemMap::iterator it = ItemManager()->m_setItemMap.begin(); it != ItemManager()->m_setItemMap.end(); ++it) {
			IFF_ITEM_COMMON &item = (IFF_ITEM_COMMON &)it->second;
			unsigned int level = GetMyLevel();
			if (!IsOutOfShopLevel(item, &level) && (item.m_flags0 & 0xE0) != 0) {
				unsigned long setKind = (item.m_typeId >> 21) & 0x1F;
				if ((setKind == 1 || setKind == 2) && ((item.m_typeId >> 13) & 0xFF) == (m_standPetTypeId & 0x3FFFFFF)
					&& IsInSalePeriod(item.m_typeId)) {
					FrListItem *listItem = m_itemListBox->AddItem(&item);
					AddSellMiniButton(item, listItem);
				}
			}
		}
	}

	m_itemListBox->SortItem(JpSpecialHitNewCompare);
}
