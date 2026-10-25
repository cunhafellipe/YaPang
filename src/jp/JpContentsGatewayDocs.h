// JP-only game code: three gateway documents the JP CContentsDoc::Initialize
// 0x915da0 inserts through its creator vector 0xe7e140 (run last, without
// the IsLocalContent test): contents types 0xad (CControllerDoc, creator
// 0x4baa50), 0xae (CWorkShop2013Doc, creator 0x9202c0) and 0xaf
// (CGrandPrixDocument, creator 0x559c00; the room-sort container of
// JpRoomListState.h). Their handlers come from the object factory
// (0xa49100 / 0xa48210) or a `new` the port has not ported: the port keeps
// them NULL, so CGatewayActor::QueryHandler must not be asked for these
// types yet (it throws "Need Gateway Handler!!").
//
// The JP container base keeps the gateway task flags at +0x4 and the type
// at +0x8 (as CHoleInOneGameDoc, JpChipInContainer.h); the KR base the port
// derives from has neither, so each class declares them. Offsets are the
// JP x86 ones (VS 2010 containers); the checks are off in the 64-bit port.
#ifndef PG_JP_JPCONTENTSGATEWAYDOCS_H
#define PG_JP_JPCONTENTSGATEWAYDOCS_H

#include <stdint.h>

#include <map>
#include <utility>
#include <vector>

#include "defines.h"
#include "padding.h"
#include "GatewayDoc.h"
#include "JpGrandPrixAwardDlg.h"

struct sRoomInfo;

namespace _gateway {
namespace _controller {

// Contents type 0xad, RTTI ".?AVCControllerDoc@_controller@_gateway@@",
// vtable 0xd026c8, `new` 0x40 bytes. Its three maps (+0x10, +0x20, +0x30;
// nodes 0x24, 0x24 and 0x30 bytes) are untraced: not declared.
class CControllerDoc : public CGatewayControlDoc {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x4ba7e0: stores the type (0xad from the creator 0x4baa50) and builds
	// the three maps.
	explicit CControllerDoc(int contentType);
	virtual ~CControllerDoc(void) {}                                  // [0] 0x4baa20 (deleting)

	// [1] 0x4baac0: clears the handler, takes the factory's handler (not
	// ported: NULL), then 0x4ba550 and 0x4b0e20 (untraced, not called).
	virtual bool Initialize(void);
	// [2] 0x4b9e70: 0x4b79a0 and 0x4b9cf0 (untraced, not called).
	virtual bool Release(void);
	virtual bool Reset(void) { return true; }                         // [3] 0x4b51e0
	virtual CGatewayHandler* GetGatewayHandler(void) { return m_pGatewayHandler; } // [4] 0x4ba940

private:
	uint32_t m_gatewayFlags;                               // 0x4
	int m_contentType;                                     // 0x8
	UNKNOWN(0xc, 0x3c);                                    // the three maps
	CGatewayHandler* m_pGatewayHandler;                    // 0x3c
};
// size 0x40 (x86)
BEGIN_LAYOUT_CHECK(CControllerDoc)
	CHECK_FIELD(CControllerDoc, m_contentType, 0x8);
	CHECK_FIELD(CControllerDoc, m_pGatewayHandler, 0x3c);
END_LAYOUT_CHECK
CHECK_SIZE(CControllerDoc, 0x40);

} // namespace _controller

// Contents type 0xae, RTTI ".?AVCWorkShop2013Doc@_gateway@@", vtable
// 0xd3c7b4, `new` 0x10 bytes.
class CWorkShop2013Doc : public CGatewayControlDoc {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x920280: stores the type (0xae from the creator 0x9202c0).
	explicit CWorkShop2013Doc(int contentType);
	virtual ~CWorkShop2013Doc(void) {}                                // [0] 0x920400 (deleting)

	// [1] 0x9202e0: `new` 0x28-byte handler (constructor 0x82e7d0, not
	// ported: NULL) into +0xc.
	virtual bool Initialize(void);
	// [2] 0x920030: deletes the handler through its slot 0 (none in the
	// port).
	virtual bool Release(void);
	virtual CGatewayHandler* GetGatewayHandler(void) { return m_pGatewayHandler; } // [4] 0x9202a0

private:
	uint32_t m_gatewayFlags;                               // 0x4
	int m_contentType;                                     // 0x8
	CGatewayHandler* m_pGatewayHandler;                    // 0xc
};
// size 0x10 (x86)
BEGIN_LAYOUT_CHECK(CWorkShop2013Doc)
	CHECK_FIELD(CWorkShop2013Doc, m_contentType, 0x8);
	CHECK_FIELD(CWorkShop2013Doc, m_pGatewayHandler, 0xc);
END_LAYOUT_CHECK
CHECK_SIZE(CWorkShop2013Doc, 0x10);

namespace _grandprix {

// Contents type 0xaf, RTTI ".?AVCGrandPrixDocument@_grandprix@_gateway@@",
// vtable 0xd09ecc, `new` 0x64 bytes: the grand prix document, also the
// room-sort container that JpRoomListState::Sort 0x5a0310 hands the sort
// (SetSort 0x559700).
class CGrandPrixDocument : public CGatewayControlDoc {
	LAYOUT_CHECK_FRIEND;
public:
	// The map +0x30: key = the record's id (sJpGrandPrixName +0x4), value
	// at node +0x18 (0x68-byte nodes: the entry is 8-aligned).
	typedef std::map<uint32_t, sJpGrandPrixEntry> ENTRY_MAP;

	// 0x559b00: stores the type (0xaf from the creator 0x559c00), empties
	// the vectors and builds the map.
	explicit CGrandPrixDocument(int contentType);
	virtual ~CGrandPrixDocument(void) {}                              // [0] 0x559bd0 (deleting)

	// [1] 0x555070: the factory's "CGrandPrixHandler" into +0xc (not
	// ported: NULL), a `new` 0x24-byte object (0x55d180, not ported) into
	// +0x50, then +0x54 = -1, +0x58 = 0, +0x5c = 0.0f.
	virtual bool Initialize(void);
	virtual bool Release(void) { return true; }                       // [2] 0x54f4a0
	virtual bool Reset(void) { return true; }                         // [3] 0x54f4b0
	virtual CGatewayHandler* GetGatewayHandler(void) { return m_pGatewayHandler; } // [4] 0x559a80

	// 0x559700: stores the sort, rebuilds the entries for it, flags the
	// listed ones, applies the remaining counts, then matches the shared
	// doc's rooms that carry the JP room flag (names ours).
	void SetSort(int sort);
	void RebuildEntries(void);                                        // 0x559540
	void FlagListedEntries(void);                                     // 0x555260
	void ApplyRemainingCounts(void);                                  // 0x5552f0
	void MatchRooms(const std::vector<const sRoomInfo*>& rooms);      // 0x556620
	// 0x552740: the +0x50 object's +0x20 (the current grand prix id; the
	// room list's Attach selects the entry of that record id).
	uint32_t GetCurrentEntryId(void) const;
	// 0x558ef0: updates every enabled entry (flag40) for clockTime and
	// collects it, sorts the list by the entries' openTime for sorts 0-3
	// (sort 0x63: then stable by the type id bits 8..18 within one
	// openTime), and for sort 0 marks the entries whose prerequisite (the
	// record's typeId) has no remaining count.
	void CollectEntries(std::vector<sJpGrandPrixEntry*>& entries, const uint64_t& clockTime);
	// 0x5551f0: the entry of key in m_entries, NULL for none.
	sJpGrandPrixEntry* FindEntry(uint32_t key);
	// 0x553dd0: the entry whose record has typeId (record +0x124), NULL
	// for none.
	sJpGrandPrixEntry* FindEntryByType(uint32_t typeId);

private:
	uint32_t m_gatewayFlags;                               // 0x4
	int m_contentType;                                     // 0x8
	CGatewayHandler* m_pGatewayHandler;                    // 0xc
	// Record +0xc ids that enable an entry (flag40; FlagListedEntries).
	std::vector<uint32_t> m_listedIds;                     // 0x10
	// (record type id +0x8, remaining count) pairs (ApplyRemainingCounts).
	std::vector<std::pair<uint32_t, uint32_t> > m_remainingCounts; // 0x20
	ENTRY_MAP m_entries;                                   // 0x30
	UNKNOWN(0x3c, 0x50);                                   // a vector +0x40 (results, 0xe4 each)
	// 0x55d180's object (deleted through slot 0; not ported: NULL).
	void* m_pObject50;                                     // 0x50
	int m_value54;                                         // 0x54 (Initialize: -1)
	int m_value58;                                         // 0x58 (Initialize: 0)
	float m_value5c;                                       // 0x5c (Initialize: 0; CollectEntries checks it against the records' minValue / maxValue, 0x55dff0)
	int m_sort;                                            // 0x60
};
// size 0x64 (x86)
BEGIN_LAYOUT_CHECK(CGrandPrixDocument)
	CHECK_FIELD(CGrandPrixDocument, m_contentType, 0x8);
	CHECK_FIELD(CGrandPrixDocument, m_pGatewayHandler, 0xc);
	CHECK_FIELD(CGrandPrixDocument, m_listedIds, 0x10);
	CHECK_FIELD(CGrandPrixDocument, m_remainingCounts, 0x20);
	CHECK_FIELD(CGrandPrixDocument, m_entries, 0x30);
	CHECK_FIELD(CGrandPrixDocument, m_pObject50, 0x50);
	CHECK_FIELD(CGrandPrixDocument, m_sort, 0x60);
END_LAYOUT_CHECK
CHECK_SIZE(CGrandPrixDocument, 0x64);

} // namespace _grandprix
} // namespace _gateway

// Port glue (the JP creators), for the KR IContentsDoc.cpp to call under
// JP_PAKS without including this header.
IContentsDataContainer* JpCreateControllerDoc(void);       // 0x4baa50
IContentsDataContainer* JpCreateWorkShop2013Doc(void);     // 0x9202c0
IContentsDataContainer* JpCreateGrandPrixDocument(void);   // 0x559c00

// The JP CSharedDoc +0x704: the grand prix table (a std::map, the record at
// node +0x10; RebuildEntries walks it). Not in the port's CSharedDoc: the
// port glue (JpContentsGatewayDocs.cpp) hands back an empty table.
typedef std::map<uint32_t, sJpGrandPrixName> JpGrandPrixTable;
const JpGrandPrixTable& JpSharedDoc_GetGrandPrixTable(void);
// The grand prix entry updates CollectEntries runs (thiscall on the entry in
// the JP; names ours). Each sets sJpGrandPrixEntry flags / value20 bits.
// 0x55e160: the own level (JP CSharedDoc +0x91d, KR m_myLevel) against the
// record's minLevel / maxLevel.
void JpGrandPrixEntry_CheckLevel(sJpGrandPrixEntry* entry);
// 0x55dff0: value (CGrandPrixDocument +0x5c) against minValue / maxValue.
void JpGrandPrixEntry_CheckValue(sJpGrandPrixEntry* entry, float value);
// 0x55ec80: with requirements for the type id (0xa17b90), FLAG_REQUIREMENT
// unless the player meets them (0x55ea60).
void JpGrandPrixEntry_CheckRequirements(sJpGrandPrixEntry* entry);
// 0x55e190 / 0x55e1c0: INFO_HAS_DATA (0xa17b50) and INFO_HAS_RANK_REWARDS
// (0xa17bd0) from the item manager's grand prix tables.
void JpGrandPrixEntry_UpdateDataInfo(sJpGrandPrixEntry* entry);
void JpGrandPrixEntry_UpdateRankRewardsInfo(sJpGrandPrixEntry* entry);
// 0x55e890: builds the entry's description (texts 0x7547, 0x18769, the
// item description of noteItemTypeId, texts 0x7548 / 0x7549).
void JpGrandPrixEntry_BuildDescription(sJpGrandPrixEntry* entry);
// 0x55e1f0: openTime and the state for clockTime (STATE_*; STATE_CURRENT
// when the record's id is currentId).
void JpGrandPrixEntry_Update(sJpGrandPrixEntry* entry, const uint64_t& clockTime, uint32_t currentId);
// 0x55e310: whether the player holds requiredItemCount of the record's
// requiredItemTypeId; otherwise sets FLAG_REQUIRED_ITEM.
bool JpGrandPrixEntry_HasRequiredItemCount(sJpGrandPrixEntry* entry);
// 0x55ea60: whether the player meets the requirements the item manager
// keeps for the grand prix type id (0xa17b90: 0x210-byte rows, a switch on
// their type class). The KR CItemManager has no such table: port glue,
// false.
bool JpGrandPrix_HasRequiredItem(uint32_t typeId);

// The JP room record's +0xcd (the grand prix id of a GP room; JP-only, not
// in the port's KR sRoomInfo): 0.
uint32_t JpRoomInfo_GetGrandPrixId(const sRoomInfo* room);

#endif // PG_JP_JPCONTENTSGATEWAYDOCS_H
