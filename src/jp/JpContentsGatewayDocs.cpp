// JP-only game code: the gateway documents of contents types 0xad, 0xae and
// 0xaf (JpContentsGatewayDocs.h).
#include "JpContentsGatewayDocs.h"

#include <stddef.h>
#include <string.h>

#include <algorithm>
#include <list>

#include "JpRoomListState.h"
#include "JpLobbyGlue.h"
#include "SharedDoc.h"
#include "globalgamedefine.h"

// JP item-manager lookups (CSharedDoc +0x20c) the entry updates call; the
// first two are defined with their other callers (JpGrandPrixAwardDlg.cpp,
// JpRoomListState.cpp), the inventory item in JpLobbyGlue.cpp
// (JpMasteryCard.h).
const std::vector<sJpGrandPrixRankReward>* JpItemManager_FindGrandPrixRankRewards(uint32_t typeId);
const char* JpItemManager_GetItemDescription(unsigned long typeId);
const sItemInfo* Jp_GameState_FindInventoryItem(uint32_t typeId);
// 0xa17b50: the item manager's grand prix data of a type id (its map
// +0x504). The KR CItemManager has no grand prix table: port glue, false.
static bool JpItemManager_HasGrandPrixData(uint32_t typeId);
// 0xa17b90: the requirement rows of a grand prix type id (a vector of
// 0x210-byte rows), true when there are some. No KR table: port glue, false.
static bool JpItemManager_HasGrandPrixRequirements(uint32_t typeId);

namespace _gateway {
namespace _controller {

// 0x4ba7e0
CControllerDoc::CControllerDoc(int contentType)
	: m_gatewayFlags(0), m_contentType(contentType), m_pGatewayHandler(0) {
}

// 0x4baac0: the factory's handler is not ported (NULL); the two untraced
// helpers 0x4ba550 / 0x4b0e20 are not called.
bool CControllerDoc::Initialize(void) {
	m_pGatewayHandler = 0;
	return true;
}

// 0x4b9e70: the two untraced helpers 0x4b79a0 / 0x4b9cf0 are not called.
bool CControllerDoc::Release(void) {
	return true;
}

} // namespace _controller

// 0x920280
CWorkShop2013Doc::CWorkShop2013Doc(int contentType)
	: m_gatewayFlags(0), m_contentType(contentType), m_pGatewayHandler(0) {
}

// 0x9202e0: the handler (0x82e7d0) is not ported: NULL.
bool CWorkShop2013Doc::Initialize(void) {
	m_pGatewayHandler = 0;
	return true;
}

// 0x920030: the JP deletes the handler through its slot 0; the port never
// creates one (CGatewayHandler has no definition in the port's headers).
bool CWorkShop2013Doc::Release(void) {
	m_pGatewayHandler = 0;
	return true;
}

namespace _grandprix {

// 0x559b00
CGrandPrixDocument::CGrandPrixDocument(int contentType)
	: m_gatewayFlags(0), m_contentType(contentType), m_pGatewayHandler(0),
	  m_pObject50(0), m_value54(0), m_value58(0), m_value5c(0.0f), m_sort(0) {
}

// 0x555070: the factory's "CGrandPrixHandler" and the 0x55d180 object are
// not ported (NULL).
bool CGrandPrixDocument::Initialize(void) {
	m_pGatewayHandler = 0;
	m_pObject50 = 0;
	m_value54 = -1;
	m_value5c = 0.0f;
	m_value58 = 0;
	return true;
}

// 0x559700
void CGrandPrixDocument::SetSort(int sort) {
	m_sort = sort;
	RebuildEntries();
	FlagListedEntries();
	ApplyRemainingCounts();
	std::vector<const sRoomInfo*> rooms;
	const std::list<sRoomInfo>& roomList = JpSharedDoc_GetRoomList();  // JP CSharedDoc +0x109e4
	for (std::list<sRoomInfo>::const_iterator i = roomList.begin(); i != roomList.end(); ++i) {
		if (JpRoomInfo_HasSortFlag(&*i)) {
			rooms.push_back(&*i);
		}
	}
	if (!rooms.empty()) {
		MatchRooms(rooms);
	}
}

// One entry (its record, flag40 set, the rest zero; 0x557d50
// builds the pair) per table record whose sort (bits 19..25 of +0x8)
// equals m_sort.
// 0x559540
void CGrandPrixDocument::RebuildEntries(void) {
	m_entries.clear();
	const JpGrandPrixTable& table = JpSharedDoc_GetGrandPrixTable();
	for (JpGrandPrixTable::const_iterator i = table.begin(); i != table.end(); ++i) {
		if (((i->second.flags >> 19) & 0x7f) == (uint32_t)m_sort) {
			sJpGrandPrixEntry entry = sJpGrandPrixEntry();
			entry.record = const_cast<sJpGrandPrixName*>(&i->second);
			entry.flag40 = true;
			m_entries.insert(ENTRY_MAP::value_type(i->second.id, entry));
		}
	}
}

// An entry whose record names a listed id (+0xc, 0 none) is
// enabled only while that id is in m_listedIds.
// 0x555260
void CGrandPrixDocument::FlagListedEntries(void) {
	for (ENTRY_MAP::iterator i = m_entries.begin(); i != m_entries.end(); ++i) {
		uint32_t listedId = i->second.record->listedId;
		if (listedId != 0) {
			i->second.flag40 = std::find(m_listedIds.begin(), m_listedIds.end(), listedId) != m_listedIds.end();
		}
	}
}

// 0x5552f0
void CGrandPrixDocument::ApplyRemainingCounts(void) {
	for (std::vector<std::pair<uint32_t, uint32_t> >::const_iterator i = m_remainingCounts.begin();
		i != m_remainingCounts.end(); ++i) {
		for (ENTRY_MAP::iterator j = m_entries.begin(); j != m_entries.end(); ++j) {
			if (j->second.record->flags == i->first) {
				j->second.remainingCount = (int)i->second;
			}
		}
	}
}

// Hands each GP room (JP +0xcd, 0 none) to the entry of its
// grand prix id.
// 0x556620
void CGrandPrixDocument::MatchRooms(const std::vector<const sRoomInfo*>& rooms) {
	for (std::vector<const sRoomInfo*>::const_iterator i = rooms.begin(); i != rooms.end(); ++i) {
		const sRoomInfo* room = *i;
		uint32_t id = JpRoomInfo_GetGrandPrixId(room);
		if (id != 0) {
			ENTRY_MAP::iterator found = m_entries.find(id);
			if (found != m_entries.end()) {
				found->second.room = room;
			}
		}
	}
}

// The JP reads the +0x50 object's +0x20 unchecked; the port never creates
// that object (Initialize), so it has no current grand prix: 0.
// 0x552740
uint32_t CGrandPrixDocument::GetCurrentEntryId(void) const {
	if (!m_pObject50) {
		return 0;
	}
	uint32_t id;
	memcpy(&id, (const char*)m_pObject50 + 0x20, sizeof(id));
	return id;
}

// The JP keeps three instantiations of std::sort with this predicate (one
// per case: 0x556f70, 0x557060, 0x557150); the order is the entries'
// openTime.
struct JpGrandPrixEntryOpenTimeLess {
	bool operator()(const sJpGrandPrixEntry* a, const sJpGrandPrixEntry* b) const {
		return a->openTime < b->openTime;
	}
};

// Sort 0x63's std::stable_sort (0x557a60): within one openTime, by the type
// id bits 8..18.
struct JpGrandPrixEntryTypeLess {
	bool operator()(const sJpGrandPrixEntry* a, const sJpGrandPrixEntry* b) const {
		return a->openTime == b->openTime
			&& (a->record->flags & 0x7ff00) < (b->record->flags & 0x7ff00);
	}
};

// 0x558ef0
void CGrandPrixDocument::CollectEntries(std::vector<sJpGrandPrixEntry*>& entries, const uint64_t& clockTime) {
	for (ENTRY_MAP::iterator i = m_entries.begin(); i != m_entries.end(); ++i) {
		sJpGrandPrixEntry* entry = &i->second;
		if (!entry->flag40) {
			continue;
		}
		entry->flags = 0;
		JpGrandPrixEntry_CheckLevel(entry);
		JpGrandPrixEntry_CheckValue(entry, m_value5c);
		JpGrandPrixEntry_CheckRequirements(entry);
		JpGrandPrixEntry_UpdateDataInfo(entry);
		JpGrandPrixEntry_UpdateRankRewardsInfo(entry);
		JpGrandPrixEntry_BuildDescription(entry);
		if (((entry->record->flags >> 19) & 0x7f) == 0) {
			entry->state = sJpGrandPrixEntry::STATE_OPEN;
		} else {
			JpGrandPrixEntry_Update(entry, clockTime, GetCurrentEntryId());
		}
		entries.push_back(entry);
	}
	switch (m_sort) {
	case 0:
		std::sort(entries.begin(), entries.end(), JpGrandPrixEntryOpenTimeLess());
		// An entry whose record names a prerequisite (typeId) is marked
		// when the entry of that type id has no remaining count.
		for (size_t i = 0; i < entries.size(); ++i) {
			sJpGrandPrixEntry* entry = entries[i];
			uint32_t prerequisite = entry->record->typeId;
			if (prerequisite == 0) {
				entry->value20 &= ~sJpGrandPrixEntry::INFO_PREREQUISITE_DONE;
			} else {
				std::vector<sJpGrandPrixEntry*>::const_iterator j = entries.begin();
				for (; j != entries.end(); ++j) {
					if ((*j)->record->flags == prerequisite) {
						break;
					}
				}
				if (j != entries.end() && (*j)->remainingCount == 0) {
					entry->value20 |= sJpGrandPrixEntry::INFO_PREREQUISITE_DONE;
				}
			}
		}
		break;
	case 1:
	case 2:
	case 3:
		std::sort(entries.begin(), entries.end(), JpGrandPrixEntryOpenTimeLess());
		break;
	case 0x63:
		std::sort(entries.begin(), entries.end(), JpGrandPrixEntryOpenTimeLess());
		std::stable_sort(entries.begin(), entries.end(), JpGrandPrixEntryTypeLess());
		break;
	default:
		break;
	}
}

// 0x5551f0
sJpGrandPrixEntry* CGrandPrixDocument::FindEntry(uint32_t key) {
	ENTRY_MAP::iterator found = m_entries.find(key);
	if (found == m_entries.end()) {
		return NULL;
	}
	return &found->second;
}

// 0x553dd0
sJpGrandPrixEntry* CGrandPrixDocument::FindEntryByType(uint32_t typeId) {
	for (ENTRY_MAP::iterator i = m_entries.begin(); i != m_entries.end(); ++i) {
		if (typeId == i->second.record->typeId) {
			return &i->second;
		}
	}
	return NULL;
}

} // namespace _grandprix
} // namespace _gateway

// 0x540a10's text for the description: the JP appends it unchecked; the
// port's table can be missing (Jp_GetTableText NULL), then "".
static const char* JpGrandPrix_Text(int id) {
	const char* text = Jp_GetTableText(id);
	return text ? text : "";
}

// 0xb306d0 (unplaced JP time helper; name ours): a FILETIME in seconds
// since 1970.
static int64_t JpFileTimeToTime(const FILETIME& fileTime) {
	int64_t ticks = (int64_t)(((uint64_t)fileTime.dwHighDateTime << 32) | fileTime.dwLowDateTime);
	return (ticks - 116444736000000000LL) / 10000000LL;
}

// 0xb30750 (unplaced; name ours): a local SYSTEMTIME in seconds since 1970.
static int64_t JpLocalSystemTimeToTime(const SYSTEMTIME* time) {
	FILETIME localTime;
	SystemTimeToFileTime(time, &localTime);
	FILETIME fileTime;
	LocalFileTimeToFileTime(&localTime, &fileTime);
	return JpFileTimeToTime(fileTime);
}

// 0xb30790 (unplaced; name ours): puts time on the date of date (year,
// month and day; time is changed in place), then in seconds since 1970.
static int64_t JpTimeOnDate(SYSTEMTIME* time, const SYSTEMTIME& date) {
	time->wYear = date.wYear;
	time->wMonth = date.wMonth;
	time->wDay = date.wDay;
	return JpLocalSystemTimeToTime(time);
}

// 0x55dff0
void JpGrandPrixEntry_CheckValue(sJpGrandPrixEntry* entry, float value) {
	const sJpGrandPrixName* record = entry->record;
	if (value < record->minValue) {
		entry->flags |= sJpGrandPrixEntry::FLAG_VALUE_LOW;
	}
	float maxValue = record->maxValue;
	if (maxValue < value && maxValue != 0.0f) {
		entry->flags |= sJpGrandPrixEntry::FLAG_VALUE_HIGH;
	}
}

// 0x55e160
void JpGrandPrixEntry_CheckLevel(sJpGrandPrixEntry* entry) {
	unsigned char level = WSingleton<CSharedDoc>::Instance()->m_myLevel;  // JP +0x91d
	const sJpGrandPrixName* record = entry->record;
	if (level < record->minLevel) {
		entry->flags |= sJpGrandPrixEntry::FLAG_LEVEL_LOW;
		return;
	}
	if (record->maxLevel != 0 && level > record->maxLevel) {
		entry->flags |= sJpGrandPrixEntry::FLAG_LEVEL_HIGH;
	}
}

// 0x55e190
void JpGrandPrixEntry_UpdateDataInfo(sJpGrandPrixEntry* entry) {
	if (JpItemManager_HasGrandPrixData(entry->record->flags)) {
		entry->value20 |= sJpGrandPrixEntry::INFO_HAS_DATA;
	} else {
		entry->value20 &= ~sJpGrandPrixEntry::INFO_HAS_DATA;
	}
}

// 0x55e1c0
void JpGrandPrixEntry_UpdateRankRewardsInfo(sJpGrandPrixEntry* entry) {
	if (JpItemManager_FindGrandPrixRankRewards(entry->record->flags)) {
		entry->value20 |= sJpGrandPrixEntry::INFO_HAS_RANK_REWARDS;
	} else {
		entry->value20 &= ~sJpGrandPrixEntry::INFO_HAS_RANK_REWARDS;
	}
}

// The schedule's times on the server time's date (JP CSharedDoc +0x180, the
// KR m_serverTime). When endTime is not after closeTime, the end is
// closeTime plus the minutes of closeTime minus those of endTime (ended
// at once when negative).
// 0x55e1f0
void JpGrandPrixEntry_Update(sJpGrandPrixEntry* entry, const uint64_t& clockTime, uint32_t currentId) {
	const SYSTEMTIME& serverTime = WSingleton<CSharedDoc>::Instance()->GetServerTime();
	sJpGrandPrixName* record = entry->record;
	entry->openTime = JpTimeOnDate(&record->openTime, serverTime);
	int64_t closeTime = JpTimeOnDate(&entry->record->closeTime, serverTime);
	int64_t endTime = JpTimeOnDate(&entry->record->endTime, serverTime);
	int64_t now = (int64_t)clockTime;
	if (closeTime < endTime) {
		if (now >= endTime) {
			entry->state = sJpGrandPrixEntry::STATE_ENDED;
			return;
		}
	} else {
		int minutes = (int)entry->record->closeTime.wMinute - (int)entry->record->endTime.wMinute;
		if (minutes < 0 || now >= closeTime + (int64_t)(minutes * 60)) {
			entry->state = sJpGrandPrixEntry::STATE_ENDED;
			return;
		}
	}
	if (now >= closeTime) {
		entry->state = sJpGrandPrixEntry::STATE_CLOSED;
		return;
	}
	if (now < entry->openTime) {
		entry->state = entry->record->id == currentId ? sJpGrandPrixEntry::STATE_CURRENT : sJpGrandPrixEntry::STATE_WAITING;
	} else {
		entry->state = sJpGrandPrixEntry::STATE_OPEN;
	}
}

// 0x55e310
bool JpGrandPrixEntry_HasRequiredItemCount(sJpGrandPrixEntry* entry) {
	const sItemInfo* item = Jp_GameState_FindInventoryItem(entry->record->requiredItemTypeId);
	if (item && item->m_count >= entry->record->requiredItemCount) {
		return true;
	}
	entry->flags |= sJpGrandPrixEntry::FLAG_REQUIRED_ITEM;
	return false;
}

// 0x55e890
void JpGrandPrixEntry_BuildDescription(sJpGrandPrixEntry* entry) {
	std::string& text = entry->description;
	text.clear();
	const char* prefix = JpGrandPrix_Text(0x7547);
	const char* itemDescription = JpItemManager_GetItemDescription(entry->record->noteItemTypeId);
	if (itemDescription) {
		text.assign(prefix);
		if (entry->record->hasNote) {
			text.append(JpGrandPrix_Text(0x18769));
		}
		text.append(itemDescription);
	} else if (entry->record->hasNote) {
		text.assign(prefix);
		text.append(JpGrandPrix_Text(0x18769));
	}
	switch (entry->record->noteKind) {
	case 1:
		if (text.size() == 0) {
			text.assign(prefix);
		} else {
			text.append(", ");
		}
		text.append(JpGrandPrix_Text(0x7548));
		break;
	case 2:
		if (text.size() == 0) {
			text.assign(prefix);
		} else {
			text.append(", ");
		}
		text.append(JpGrandPrix_Text(0x7549));
		break;
	}
}

bool JpGrandPrix_HasRequiredItem(uint32_t typeId) {
	(void)typeId;
	return false;
}

// 0x55ec80
void JpGrandPrixEntry_CheckRequirements(sJpGrandPrixEntry* entry) {
	if (JpItemManager_HasGrandPrixRequirements(entry->record->flags)
		&& !JpGrandPrix_HasRequiredItem(entry->record->flags)) {
		entry->flags |= sJpGrandPrixEntry::FLAG_REQUIREMENT;
	}
}

static bool JpItemManager_HasGrandPrixData(uint32_t typeId) {
	(void)typeId;
	return false;
}

static bool JpItemManager_HasGrandPrixRequirements(uint32_t typeId) {
	(void)typeId;
	return false;
}

// The JP CSharedDoc +0x704 has no port member: an empty table.
const JpGrandPrixTable& JpSharedDoc_GetGrandPrixTable(void) {
	static const JpGrandPrixTable s_table;
	return s_table;
}

// No port room record has the JP +0xcd.
uint32_t JpRoomInfo_GetGrandPrixId(const sRoomInfo* room) {
	(void)room;
	return 0;
}

// 0x4baa50
IContentsDataContainer* JpCreateControllerDoc(void) {
	return new _gateway::_controller::CControllerDoc(0xad);
}

// 0x9202c0
IContentsDataContainer* JpCreateWorkShop2013Doc(void) {
	return new _gateway::CWorkShop2013Doc(0xae);
}

// 0x559c00
IContentsDataContainer* JpCreateGrandPrixDocument(void) {
	return new _gateway::_grandprix::CGrandPrixDocument(0xaf);
}
