// JP-only game code (no KR counterpart): the daily-quest window
// (_gateway::_questoneday::FrOnedayQuest, layout "oneday_quest_main") and
// the achievement window (_gateway::_questoneday::FrAchievement, layout
// "ach_main"). Their data is the gateway quest document (document type
// 0xAC, `CQuestDocument`) that `_gateway::_questoneday::CQuestHandler`
// fills from the JP replies 0x0225-0x022E (docs/protocol/quest.md).
//
// Opener chains (JP):
// - Daily quests: the lobby quest button (LobbyMain_OpenQuest 0x593990 /
//   AvatarChatMain_OpenQuest 0x7b0d20) sends 0x0151; the reply 0x0225 goes
//   through the base OnPacket 0x9c20a0 to the gateway actor (message
//   0x753A, key 2), CQuestHandler 0x5ef270 resets the document, rebuilds the
//   shown list (0x5ef100), posts 0x1A3 to "LobbyAvatar" and calls the
//   factory 0x5ee440 (manager = 0xf022d0 +0x40, "oneday_quest_main", no
//   parent); it keeps the form in handler +0x24, moves it (vfn +0x8c with
//   its rect offset by two JP constants), runs vfn +0x78(0), and stores the
//   document in m_pDoc (+0x2bc).
// - Achievements: 0x0157 -> 0x022D records (key 8, 0x5ee6e0) then 0x022C
//   (key 9, 0x5ee910 -> 0x5ee950): the factory 0x5ee5e0 (same manager,
//   "ach_main"), kept in handler +0x2c, document in m_pDoc (+0x79c), vfn
//   +0x78(0), SetTopmost(1), FindNextTopFocus(1), then Open with the result
//   callback 0x5ee800. When "bongdari_shop" is open the document goes to it
//   instead (0x8b4f20).
//
// JP x86 facts:
// - FrOnedayQuest: RTTI 0xe74e5c (GetRTTI 0x51c1e0), size 0x2e0, ctor
//   0x51c0c0, dtor 0x51c210 (deleting 0x51c510; thunks 0x51c200 / 0x51c1f0),
//   base the JP FrForm (ctor 0xbe5e20, dtor 0xbe5e80). Vtables 0xd078bc (38
//   slots: [0] GetRTTI, [1] deleting dtor, [9] OnProc 0x517770, [32] OnInit
//   0x51c610; [6] 0x47ccd0, [10] 0x47ccf0, [14]-[19] 0x47cd00-0x47cd50,
//   [21] 0x47cd60, [31] 0x47cd70 are header-inline copies, the rest
//   FrForm's), 0xd078b0 (FrCmdTarget: [0] 0x51c200, [1] GetMessageMap
//   0x517740), 0xd078a8 ([0] 0x51c1f0). Message map 0xe09b3c (rows
//   0xe09b58, 0x20 bytes each; handlers stored by the static init
//   0xcccb60). Registered as "FrOnedayQuest" (make instance 0x51c480,
//   0x51c4f0, static init 0xcccb40).
// - FrAchievement: RTTI 0xe74d44 (GetRTTI 0x4e9930), size 0x7a0, ctor
//   0x4ea420, dtor 0x4e96d0 (deleting 0x4ea6e0; thunks 0x4e9940 /
//   0x4e9950). Vtables 0xd0504c ([0], [1], [32] OnInit 0x4e9960; [9] is
//   FrForm's), 0xd0503c ([0] 0x4e9940, [1] GetMessageMap 0x4e13a0),
//   0xd05034 ([0] 0x4e9950). Message map 0xe0708c (rows 0xe07098, static
//   init 0xcca4c0). Registered as "FrAchievement" (make instance 0x4ea710,
//   0x4ea780, static init 0xcca4a0).
// The port keeps the KR FrForm layout, so the JP offsets are comments only.
#ifndef PG_JP_JPQUESTFORMS_H
#define PG_JP_JPQUESTFORMS_H

#include <stdint.h>

#include <map>
#include <utility>
#include <list>
#include <string>
#include <vector>

#include <windows.h>

#include "defines.h"
#include "padding.h"
#include "frcmdtarget.h"
#include "frform.h"
#include "GatewayDoc.h"
#include "JpMaskGauge.h"
#include "TitleFontEx.h"
#include "wtypes.h"

class Bitmap;
class FrArea;
class FrButton;
class FrListBox;
struct FrListItem;
class FrStatic;
class FrWnd;
class FrWndManager;
class WOverlay;
class WReceivedPacket;

namespace _gateway {
namespace _questoneday {

class CQuestHandler;

// A player quest item: the value of the CSharedDoc +0x384C list nodes
// (node +0x8), also decoded from the 0x0226 reply (JP size 0x108).
class JpPlayerQuest {
public:
	JpPlayerQuest(void);                                              // 0x5e6080 (declared only)
	~JpPlayerQuest(void);                                             // 0x5e5c20 (declared only)
	// Declared only: reads the item from the packet; copies another
	// item's values into this one (0x0226).
	void Decode(WReceivedPacket& packet);                             // 0x5e62b0
	void Assign(const JpPlayerQuest& other);                          // 0x5e5c30
	// Declared only: takes the progress of the 0x022x key-4 item.
	void Merge(const JpPlayerQuest& other);                           // 0x5e6350
	// Names ours: the 0x0228 removal clears the kept flag; the 0x0227
	// completion (declared only); whether the item stays in the list.
	void ClearKept(void) { m_kept = false; }                          // 0x5e5ee0
	void SetDone(void);                                               // 0x5e63b0
	bool IsKept(void) const { return m_kept == true; }                // 0x5e5ef0
	// The item's unique id (the packets name items by it).
	uint32_t GetUid(void) const { return m_uid; }                     // 0x5e5f10
	uint32_t GetTypeId(void) const { return m_typeId; }               // 0x5e5f00
	bool IsState(int32_t state) const { return m_state == state; }    // 0x5e5f20
	int32_t GetState(void) const { return m_state; }                  // 0x5e5f30
	// Objective `index`'s progress in percent, floored (0x5e6780:
	// progress / goal * 100; 0x5e6800: (progress % 10) / 10 * 100); 0 when
	// the objective is missing or unused.
	float GetObjectivePercent(uint32_t index) const;                  // 0x5e6780
	float GetObjectiveStepPercent(uint32_t index) const;              // 0x5e6800
	// The progress key of objective `index` (the CSharedDoc +0x3840
	// records are keyed by it; no range check).
	uint32_t GetObjectiveKey(uint32_t index) const { return m_objectives[index].m_key; } // 0x5e5f40

	// An objective of the quest (0x18 bytes; ten at +0x18).
	struct sObjective {
		uint32_t m_id;               // +0x0 (Merge finds the objective by it)
		uint32_t m_typeId;           // +0x4 (0: unused; Assign copies the used ones)
		uint32_t m_key;              // +0x8 (the CSharedDoc +0x3840 record's key)
		// +0xc: alignment padding (not cleared by the ctor)
		uint64_t m_progress;         // +0x10 (Decode reads the low dword)
	};

private:
	// Objective `index`, NULL past the ten.
	sObjective* GetObjective(uint32_t index);                         // 0x5e5fb0
	// The objective whose id is `id`, NULL when none.
	sObjective* FindObjective(uint32_t id);                           // 0x5e5fd0
	// 0x5e6420 (name ours): the goal of the objective's achievement
	// (0xa176b0 with its id, the record's +0xd4), 0 when missing.
	int32_t GetObjectiveGoal(const sObjective* objective) const;
	// 0x5e6680 (name ours): the count of the objective's progress record
	// (CSharedDoc +0x3840 by its key) while that record is active, else
	// 0 (and 0 without a key).
	int32_t GetObjectiveProgress(const sObjective* objective) const;

	// +0x0: a vtable (the dtor stores 0xd0b40c); not declared yet.
	UNKNOWN(0x0, 0x8);
	bool m_kept;                     // +0x8
	uint32_t m_typeId;               // +0xc (the quest's type id)
	uint32_t m_uid;                  // +0x10 (the item's unique id)
	int32_t m_state;                 // +0x14
	sObjective m_objectives[10];     // +0x18
};

// A quest record of the document (0xc bytes; the vectors +0x10 and +0x20).
// Built by 0x5ebf80 / 0x5ebee0 from a node of the player's quest list
// (CSharedDoc +0x384C): types of the pointees not traced.
struct sQuestDocRecord {
	const void* m_pQuestData;        // +0x0 (0xa17700 on CSharedDoc +0x20c with the item's id)
	const void* m_pProgress;         // +0x4 (the CSharedDoc +0x3840 entry of the item, or NULL)
	const JpPlayerQuest* m_pPlayerQuest; // +0x8 (the CSharedDoc +0x384C list item, node +0x8)
};

// An achievement record of the document (the value of the maps +0x70 /
// +0x80; read from the 0x022D reply). The same entries the mastery tile
// dialog walks (RFCharMasteryTileDlg::MeetsRequirement 0x5c1c70, the map
// at +0x70 of the type-0xAC data). Only its id is traced.
class JpAchievementRecord {
public:
	JpAchievementRecord(void);                                        // 0x47a640
	~JpAchievementRecord(void);                                       // 0x47a060
	// Reads the record from the packet.
	void Decode(WReceivedPacket& packet);                             // 0x47a720
	unsigned long GetId(void) const { return m_id; }                  // 0x47a070
	// Counter `index`'s id and count (no bounds check, as in the JP).
	uint32_t GetCounterId(uint32_t index) const;                      // 0x47a080
	const uint64_t* GetCounterCount(uint32_t index) const;            // 0x47a0d0
	// The highest counter index whose id and count are both non-zero, -1
	// when none (name ours).
	int32_t GetLastCountedIndex(void) const;                          // 0x47a170
	// The first counter index with an id and a zero count, -1 when none
	// (name ours).
	int32_t GetFirstOpenIndex(void) const;                            // 0x47a150
	// Counter `index`'s second dword (+0x4; the progress value
	// LayoutCategoryElements divides by the step's goal; name ours, no
	// bounds check).
	int32_t GetCounterValue(uint32_t index) const;                    // 0x47a0c0
	// The index of the counter with the latest count (a time; the
	// counters sorted by count, descending, signed), -1 when that count
	// is 0 (name ours).
	int32_t GetLatestCounterIndex(void) const;                        // 0x47c5b0

	// A counter of the record (0x10 bytes; ten at +0x10, read by
	// 0x47a080 / 0x47a0d0).
	struct sCounter {
		uint32_t m_id;               // +0x0
		uint32_t m_typeId;           // +0x4
		uint64_t m_count;            // +0x8 (Decode reads the low dword)
	};

private:
	// Counter `index`, NULL past the ten.
	sCounter* GetCounter(uint32_t index);                             // 0x47a200

	// +0x0: a vtable (the dtor stores 0xcfe704); not declared yet.
	UNKNOWN(0x0, 0x8);
	unsigned long m_id;              // +0x8
	uint32_t m_typeId;               // +0xc (read with the id; use not traced)
	sCounter m_counters[10];         // +0x10
};

struct sAchievementData;

// An achievement record of the item manager's table (CSharedDoc +0x20c,
// 0xa176b0 by id; 0x5e6420 reads its goal, 0x51b950 its name and
// rewards). Only the traced fields.
struct sAchievementStepData {
	UNKNOWN(0x0, 0x4);
	uint32_t m_id;                   // +0x4 (the counter id: JpAchievementRecord::GetCounterId)
	char m_name[0xcc];               // +0x8 (NUL-terminated; its length is not traced, bounded here by the goal)
	int32_t m_goal;                  // +0xd4
	UNKNOWN(0xd8, 0xe8);
	uint32_t m_rewardTypeIds[3];     // +0xe8 (0: none; 0x6C000001 the achievement points)
	// The count of each reward; the first is the points 0x5ed3d0 sums.
	int32_t m_rewardCounts[3];       // +0xf4
};
CHECK_FIELD(sAchievementStepData, m_id, 0x4);
CHECK_FIELD(sAchievementStepData, m_name, 0x8);
CHECK_FIELD(sAchievementStepData, m_goal, 0xd4);
CHECK_FIELD(sAchievementStepData, m_rewardTypeIds, 0xe8);
CHECK_FIELD(sAchievementStepData, m_rewardCounts, 0xf4);

// The value of the quest document's category map (+0x60, keyed by the
// achievement data id; 0x1c bytes), built by 0x5ed780 from the vector
// +0x50: the data's id (+0x4), a total (0x5e9670 over the steps), the
// data, and the records of its steps (0xa176b0 of each non-zero id at the
// data's +0x5d4, ten of them; 0x5eac20).
struct sAchievementCategory {
	uint32_t m_id;                   // +0x0
	int32_t m_total;                 // +0x4
	const void* m_pData;             // +0x8
	std::vector<const sAchievementStepData*> m_steps; // +0xc
};

// The gateway quest document (document type 0xAC; JP vtable [0] deleting
// dtor 0x5ee010 (dtor 0x5edaf0), [1] Initialize 0x5edc40, [2] Release
// 0x5e7250, [3] Reset 0x5e7260, [4] GetGatewayHandler 0x5edc30). The JP
// container base keeps the gateway task flags at +0x4 (stored by the
// actor's QueryHandler 0x91e850); the KR base has no such member.
// JP layout (VS 2010 vectors and maps, 0x10 bytes each): +0xc the handler;
// +0x10 the quest records (SetQuestIds); +0x20 the shown quest records
// (SetShownQuests, HasQuests); +0x30 / +0x40 the SYSTEMTIMEs of dates
// 0 / 1; +0x50 a vector (filled by 0xa20180 from CSharedDoc +0x20c); +0x70
// the achievement map (head +0x74, size +0x78), +0x80 the pending
// achievement map (0x022D records); +0x90 0x20 bytes cleared by
// Initialize. Members with untraced element types are not declared.
class CQuestDocument : public CGatewayControlDoc {
public:
	CQuestDocument(void);
	virtual ~CQuestDocument(void);                                    // [0] 0x5ee010 (deleting), 0x5edaf0

	virtual bool Initialize(void);                                    // [1] 0x5edc40
	virtual bool Release(void);                                       // [2] 0x5e7250
	// Clears both dates.
	virtual bool Reset(void);                                         // [3] 0x5e7260
	virtual CGatewayHandler* GetGatewayHandler(void);                 // [4] 0x5edc30

	// Stores date 0 or 1 (other indices are ignored).
	void SetDate(const SYSTEMTIME& date, int index);                  // 0x5e7290
	// Date 0 or 1 (read inline by FrOnedayQuest::OnInit; name ours).
	const SYSTEMTIME& GetDate(int index) const { return m_dates[index]; }
	// Clears the quest records (+0x10), then adds one per id.
	void SetQuestIds(const std::vector<unsigned long>& ids);          // 0x5ec050
	// Clears the shown quest records (+0x20), then adds one (kind 3) per
	// player quest item.
	void SetShownQuests(const std::vector<const JpPlayerQuest*>& quests); // 0x5ecd80
	// Adds the pending achievement record (keyed by its id) unless one
	// with that id is already pending.
	void AddPendingAchievement(const JpAchievementRecord& record);    // 0x5eb9f0
	// Swaps the achievement map (+0x70) with the pending one (+0x80),
	// then clears the pending one.
	void FinishAchievements(void);                                    // 0x5ecde0
	// Clears the quest records (+0x10; inlined in the JP, 0x0226).
	void ClearQuestRecords(void);
	// The achievement map's size (+0x78; inlined in the JP).
	int GetAchievementCount(void) const;
	// The achievement map (+0x70; inlined in the JP, 0x5c1c70 reads +0x74).
	const std::map<unsigned long, JpAchievementRecord>& GetAchievements(void) const { return m_achievements; }
	// 0x5ed3d0 (name ours): the points of the steps reached over the
	// achievement map, 0 when it is empty; FrAchievement shows it with
	// "%d".
	int GetAchievementPoints(void);
	// 0x5ed500 (name ours): the same over the achievement data of one
	// category (0..7; the +0x50 range whose +0xc4 is it, 0x5e9a30), 0 for
	// another category.
	int GetCategoryPoints(int category);
	// 0x5ed6c0 (name ours): the points (+0xf4) of the steps 0..the last
	// counted one (0x47a170) of the record's category entry (+0x60 by
	// its id), 0 without a record or a counted step.
	int GetRecordPoints(const JpAchievementRecord* record);
	// The category map (+0x60; FrAchievement 0x4e8c20 inlines its find).
	const std::map<unsigned long, sAchievementCategory>& GetCategories(void) const { return m_categories; }
	// The category's step points total (+0x90; inlined in the JP).
	int32_t GetCategoryTotal(int category) const { return m_categoryTotals[category]; }
	// Whether the shown quest records (+0x20) are not empty (the player
	// holds today's quests).
	bool HasQuests(void) const;                                       // 0x5e7fd0
	// The shown quest record (+0x20) whose player quest item has that
	// uid, NULL when none.
	sQuestDocRecord* FindShownQuest(unsigned long uid);               // 0x5e9620
	// 0x5ea470 (name ours): the step records of the category map's (+0x60)
	// entry of that achievement data id, NULL when missing.
	const std::vector<const sAchievementStepData*>* FindCategorySteps(uint32_t id);
	// The +0x50 data, sorted by category (inlined by FrAchievement 0x4e8100).
	const std::vector<const sAchievementData*>& GetAchievementData(void) const { return m_achievementData; }
	// The quest records (+0x10) and the shown quest records (+0x20), read
	// in place by FrOnedayQuest's packet sender 0x51a550 (inlined in the
	// JP).
	const std::vector<sQuestDocRecord>& GetQuestRecords(void) const { return m_questRecords; }
	const std::vector<sQuestDocRecord>& GetShownQuests(void) const { return m_shownQuests; }

private:
	// 0x5ebf80 (declared only; LTCG register arguments, called on the
	// vector): the record of the player quest item with this id
	// (CSharedDoc +0x384C, 0x5e9130), when its kind (0x5e5f30) is `kind`;
	// with `noProgress` the progress pointer is NULL, else the item's
	// CSharedDoc +0x3840 entry (0x5e9190). Pushed by 0x5ebad0.
	static void AddQuestRecord(std::vector<sQuestDocRecord>& records, unsigned long id, bool noProgress, int kind);
	// 0x5ebee0 (declared only; LTCG register arguments, called on the
	// vector): the same from a player quest item.
	static void AddShownQuest(std::vector<sQuestDocRecord>& records, const JpPlayerQuest* quest, int kind);

	CQuestHandler* m_pGatewayHandler;   // JP +0xc
	std::vector<sQuestDocRecord> m_questRecords;   // JP +0x10
	std::vector<sQuestDocRecord> m_shownQuests;    // JP +0x20
	SYSTEMTIME m_dates[2];              // JP +0x30 (date 0), +0x40 (date 1)
	// The item manager's achievement data, sorted by category (filled by
	// 0xa20180 from CSharedDoc +0x20c, not ported: empty in the port).
	std::vector<const sAchievementData*> m_achievementData;            // JP +0x50
	std::map<unsigned long, sAchievementCategory> m_categories;        // JP +0x60 (0x5ed780; empty in the port)
	std::map<unsigned long, JpAchievementRecord> m_achievements;        // JP +0x70
	std::map<unsigned long, JpAchievementRecord> m_pendingAchievements; // JP +0x80
	// The points of each category's steps (0x5ed780: the 0x5e9670 totals
	// of the +0x50 data of that category); zeroed by Initialize.
	int32_t m_categoryTotals[8];        // JP +0x90
};

// 0xa17700 on CSharedDoc +0x20c (name ours; defined in JpQuestHandler.cpp):
// the quest data (sOnedayQuestInfo) of a type id, NULL when missing.
const void* JpSharedDoc_FindQuestData(uint32_t typeId);

// A record of the quest progress list (CSharedDoc +0x3840, a std::list;
// 0x10 bytes; name ours). Appended by 0x641470 from JpPlayerQuest::SetDone
// ({true, typeId, key, 0}), from 0x5861a0 and from a packet case of the
// JP protocol dispatcher (0x9d4b62: a count, then per record the reader
// 0x9b90e0: u8, u32, u32, u32).
struct sQuestProgress {
	bool m_active;                   // +0x0 (true when appended)
	uint32_t m_typeId;               // +0x4
	uint32_t m_key;                  // +0x8
	int32_t m_count;                 // +0xc (0 when appended)
};
// 0x641470 (a CSharedDoc member; name ours): appends a copy of the record
// to that list. The port keeps the list in JpQuestHandler.cpp (the KR
// CSharedDoc has no +0x3840).
void JpSharedDoc_PushQuestProgress(const sQuestProgress& record);
// The record (node +0x8) whose key (+0x8) is `key`, NULL when missing
// (the JP std::find_if instances 0x5e9190 and the walk in 0x5e6680).
const sQuestProgress* JpSharedDoc_FindQuestProgress(uint32_t key);
// The player's quest list (CSharedDoc +0x384C, a std::list; names ours;
// defined in JpQuestHandler.cpp): the first and next item (node +0x8),
// NULL past the last.
JpPlayerQuest* JpSharedDoc_FirstPlayerQuest(void);
JpPlayerQuest* JpSharedDoc_NextPlayerQuest(const JpPlayerQuest* quest);
struct sAchievementData;
// 0xa176b0 on CSharedDoc +0x20c (name ours; defined in JpQuestHandler.cpp):
// the +0xd4 goal of the achievement record of that id, NULL when the
// record is missing.
const int32_t* JpSharedDoc_FindAchievementGoal(uint32_t id);
// 0xa176b0 on CSharedDoc +0x20c, the record itself (name ours; defined in
// JpQuestHandler.cpp): NULL when missing.
const sAchievementStepData* JpSharedDoc_FindAchievementStep(uint32_t id);
// 0xa238f0 on CSharedDoc +0x20c (name ours; defined in JpQuestHandler.cpp):
// appends the +0x18 value of every node of the item manager's list +0x34c
// (the reward type ids SetRewards leaves out of the gift line).
void JpSharedDoc_GetHiddenRewardIds(std::vector<uint32_t>& ids);
// 0xa17750 on CSharedDoc +0x20c (name ours; defined in JpQuestHandler.cpp):
// the achievement data of a type id (the JP item manager's map at
// +0x180), NULL when missing.
const sAchievementData* JpSharedDoc_FindAchievementData(uint32_t typeId);

// The container the content document inserts for type 0xAC (the JP
// CContentsDoc's insertion is not located; the KR CContentsDoc::Initialize
// calls this under JP_PAKS).
IContentsDataContainer* JpCreateQuestDocument(void);

// The confirmation form "oneday_quest_mention" (JP size 0x120). Vtables
// 0xd075f4 ([0] GetRTTI 0x517a80, [1] deleting dtor 0x518f30, [32]
// OnInit 0x517ab0, the rest FrForm's and the shared header inlines),
// 0xd075e8 (FrCmdTarget: [0] 0x5185d0, [1] GetMessageMap 0x517a40),
// 0xd075e0. Message map 0xe09b4c (rows 0xe09e78, handlers stored by the
// static init 0xccced0).
class FrOnedayMentionSelect : public FrForm {
public:
	static const WRTTI m_RTTI;                                        // 0xe74e70
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x517a80

	FrOnedayMentionSelect(void);                                      // 0x517a50
	virtual ~FrOnedayMentionSelect(void);                             // [1] 0x518f30 (deleting), 0x517a90

	// FrForm::OnInit only.
	virtual bool OnInit(void);                                        // [32] 0x517ab0

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe09e78

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe09b4c
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // FrCmdTarget [1] 0x517a40

	// Close(FrOK, true) / Close(FrNO, true) (JP vfn +0x68).
	void OnYesClicked(void);                                          // 0x517ac0
	void OnNoClicked(void);                                           // 0x517ad0
	void OnYesInit(intptr_t param);                                   // 0x518f60
	void OnNoInit(intptr_t param);                                    // 0x518fd0

private:
	FrButton* m_pYes;                // JP +0x110 ("arin_yes")
	FrButton* m_pNo;                 // JP +0x114 ("arin_no")
	UNKNOWN(0x118, 0x120);
};
// size 0x120 (x86)

// 0x5194b0: the JP CreateForm<FrOnedayMentionSelect> (the JP also stores
// the form in the TLS slot +0x64).
FrOnedayMentionSelect* CreateOnedayMentionSelect(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent);

// The quest data a document record points to (sQuestDocRecord::m_pQuestData;
// only the name and the reward are traced).
struct sOnedayQuestInfo {
	UNKNOWN(0x0, 0x8);
	char m_name[0xc0];               // +0x8 (NUL-terminated; its length is not traced, bounded here by the flag)
	int32_t m_showFlag;              // +0xc8 (1: shown in the daily list; JpQuestData_GetShowFlag)
	uint32_t m_achievementId;        // +0xcc (its goal: JpSharedDoc_FindAchievementGoal)
	UNKNOWN(0xd0, 0xf4);
	uint32_t m_rewardTypeId;         // +0xf4 (OnQuestDone shows it in "scratchresultdlg")
	UNKNOWN(0xf8, 0xfc);
	uint32_t m_rewardCount;          // +0xfc
};

// FrListItem::pData of a futurework / currentwork row (built by OnInit):
// a document record (sQuestDocRecord: +0x8 is the player quest item, read
// by 0x5199e0 as one).
struct sOnedayQuestRow {
	sOnedayQuestInfo* m_pInfo;       // +0x0
	UNKNOWN(0x4, 0x8);
	int m_inProgress;                // +0x8 (currentwork: 0 draws "solved" instead of the name)
};

class FrOnedayQuest : public FrForm {
public:
	static const WRTTI m_RTTI;                                        // 0xe74e5c
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x51c1e0

	FrOnedayQuest(void);                                              // 0x51c0c0
	virtual ~FrOnedayQuest(void);                                     // [1] 0x51c510 (deleting), 0x51c210

	// Updates the four gauges.
	virtual void OnProc(const float dt);                              // [9] 0x517770
	// Reads
	// "quest_ui_config" of "onedayquest.lua" (procgaugex/y, totalgaugex/y
	// offset by the form's origin; datetodayx/y -> m_dateTodayPos,
	// dateprocx/y -> m_dateProcPos), pushes typeid 0x78800001 to
	// m_typeIds, sets up the gauges (three "quest_gauge1" stacked, one
	// "quest_gauge2"), loads the overlay "quest_daily_reward.tga", formats
	// the document's dates 0 / 1 as "%d-%d-%d" (year, month, day) into
	// m_dateToday / m_dateProc, fills m_shownQuests from CSharedDoc
	// +0x384C (0x51bec0), and with a document runs 0x51a1c0, 0x51ad00 and
	// 0x51a320 before FrForm::OnInit.
	virtual bool OnInit(void);                                        // [32] 0x51c610

	// Opens "oneday_quest_mention" with the text 0x2c3 and the result
	// callback OnSelectQuestResult.
	void SelectQuest(void);                                           // 0x51a790

	// Called by CQuestHandler (names ours): RefreshQuests
	// after the shown list is rebuilt (0x0226 / 0x0228; also OnInit with a
	// document); OnQuestDone / OnTypedQuestDone for a player quest item the
	// 0x0227 reply finished, by its type id: (typeId & 0x3C00000) == 0, else
	// != 0.
	void RefreshQuests(void);                                         // 0x51a1c0
	void OnQuestDone(JpPlayerQuest* quest);                           // 0x519810
	void OnTypedQuestDone(JpPlayerQuest* quest);                      // 0x51a4e0
	// 0x51a320 (name ours): enables the quest button of each shown quest in
	// state 4, sets the row gauges to the first objective's percent, and
	// the reward gauge to the 0x78800001 quest's step percent (100 while a
	// gift button is enabled).
	void RefreshProgress(void);
	// 0x51ad00 (name ours): enables the gift button (m_typeIds) of each
	// shown quest in state 4 whose first progress record is active and
	// has reached its achievement's goal.
	void EnableReachedGifts(void);

	CQuestDocument* m_pDoc;          // JP +0x2bc (set by CQuestHandler 0x5ef270)

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe09b58

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe09b3c
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // FrCmdTarget [1] 0x517740

	void OnFutureWorkInit(intptr_t param);                            // 0x518630
	void OnFutureWorkDraw(intptr_t param);                            // 0x5186a0
	void OnCurrentWorkInit(intptr_t param);                           // 0x518750
	void OnCurrentWorkDraw(intptr_t param);                           // 0x5187c0
	void OnReqTodayQuestInit(intptr_t param);                         // 0x518890
	// Opens "oneday_quest_mention" (text 0x2bc, OnReqTodayQuestResult)
	// unless the document already holds quests: then message 0x23 with
	// the text 0x2bd to the current task's main actor.
	void OnReqTodayQuestClicked(void);                                // 0x51b430
	void OnGiveUpQuestInit(intptr_t param);                           // 0x518900
	// Opens "oneday_quest_mention" (text 0x2be, OnGiveUpQuestResult).
	void OnGiveUpQuestClicked(void);                                  // 0x51a9d0
	// The cast, then Enable(false) (vfn +0x18).
	void OnQuestBtn1Init(intptr_t param);                             // 0x518970
	void OnQuestBtn2Init(intptr_t param);                             // 0x5189e0
	void OnQuestBtn3Init(intptr_t param);                             // 0x518a50
	void OnQuestBtn1Clicked(void);                                    // 0x51aa50
	void OnQuestBtn2Clicked(void);                                    // 0x51aa60
	void OnQuestBtn3Clicked(void);                                    // 0x51aa70
	// The cast, then Enable(false) (vfn +0x18).
	void OnLastGiftInit(intptr_t param);                              // 0x518ac0
	void OnLastGiftClicked(void);                                     // 0x51b050
	// When IsShownQuest(typeId), disables last_gift and runs
	// SendRewardRequest.
	void RequestReward(unsigned int typeId);                          // 0x51ae90
	// 0x51a810: whether a kept player quest of that type id is shown
	// (the find 0x518290 in m_shownQuests).
	bool IsShownQuest(unsigned int typeId);
	// 0x51ae10 (name kept): opens "oneday_quest_mention" (text 0x2c3) with
	// the result callback OnRewardResult.
	void SendRewardRequest(void);
	// The "oneday_quest_mention" result callbacks: 0x519ad0 (SelectQuest),
	// 0x51a840 (giveup_quest), 0x51b3f0 (req_today_quest).
	bool OnSelectQuestResult(int ret, FrForm* form);
	bool OnGiveUpQuestResult(int ret, FrForm* form);
	bool OnReqTodayQuestResult(int ret, FrForm* form);
	// 0x51a860 (SendRewardRequest's callback): resets the reward gauge and
	// runs SendTypedQuestSelect(m_rewardTypeId), else message 0x23, then
	// OpenRewardResult(0x1a0001c4, 1).
	bool OnRewardResult(int ret, FrForm* form);
	// 0x5199e0 (name ours): sends 0x153 {1, uid} for the player quest item
	// of that currentwork row, then message 1 to the main actor.
	void SendSelectQuest(int row);
	// 0x51a550 (name ours): sends `id` with the uids of the document's quest
	// records (questRecords) or shown quest records; 0x154 (give up) counts
	// and writes only the records with a player quest item. Then message 1
	// to the main actor.
	void SendQuestRequest(int id, bool questRecords);
	// 0x5179f0: the form's JP vfn +0x68(1, 1) (slot 26).
	void OnCloseClicked(void);
	void OnDrawGauge(intptr_t param);                                 // 0x519bc0
	// 0x517950 (name ours): three empty rows (no data) in futurework (0)
	// or currentwork (1).
	void AddEmptyRows(int list);
	// 0x5179e0 (name ours): the "scratchresultdlg" result callback.
	bool OnScratchResultClosed(int ret, FrForm* form);
	// 0x51a690 (name ours): sends 0x153 {1, uid} for the shown player quest
	// of that type id, then message 1 to the main actor.
	void SendTypedQuestSelect(unsigned int typeId);
	// Inlined in the JP by OnQuestDone and OnRewardResult (name ours):
	// "scratchresultdlg" (CreateForm 0x5193e0) 70 up, Open(OnScratchResultClosed,
	// 0xc2), no fade-out, showing one rare item (kind 2).
	void OpenRewardResult(unsigned long typeId, int count);

private:
	FrListBox* m_pFutureWork;        // JP +0x110 ("futurework"; cast to RTTI 0xf1aae0 FrListBox)
	FrListBox* m_pCurrentWork;       // JP +0x114 ("currentwork"; FrListBox)
	FrButton* m_pQuestBtn[3];        // JP +0x118 ("quest_btn1".."quest_btn3")
	FrButton* m_pReqTodayQuest;      // JP +0x124 ("req_today_quest")
	FrButton* m_pGiveUpQuest;        // JP +0x128 ("giveup_quest")
	FrButton* m_pLastGift;           // JP +0x12c ("last_gift")
	int m_unknown130;                // JP +0x130 (cleared by the ctor; use not traced)
	JpMaskGauge m_gauges[4];         // JP +0x134 (4 x 0x4c)
	WOverlay* m_pRewardOverlay;      // JP +0x264 ("quest_daily_reward.tga")
	int m_unknown268;                // JP +0x268 (cleared by the ctor; use not traced)
	WPoint m_dateTodayPos;           // JP +0x26c
	WPoint m_dateProcPos;            // JP +0x274
	std::string m_dateToday;         // JP +0x27c ("%d-%d-%d" of document date 0)
	std::string m_dateProc;          // JP +0x298 ("%d-%d-%d" of document date 1)
	int m_selected;                  // JP +0x2b4 (-1; quest_btn1..3 set 0..2)
	unsigned int m_rewardTypeId;     // JP +0x2b8 (0; last_gift sets 0x78800001)
	// JP +0x2bc: m_pDoc (public above).
	// The player quest items shown (built by 0x51bec0 from CSharedDoc
	// +0x384C).
	std::vector<JpPlayerQuest*> m_shownQuests; // JP +0x2c0
	// {type id, its gift button} (8-byte elements, walked by
	// OnTypedQuestDone; OnInit pushes 0x78800001).
	std::vector<std::pair<unsigned int, FrButton*> > m_typeIds; // JP +0x2d0
};
// size 0x2e0 (x86)

// A step slot of an element (0x10-byte elements of sAchElementItem +0x3c;
// LayoutCategoryElements copies the step record's three reward counts,
// sAchievementStepData +0xf4).
struct sAchStepSlot {
	int32_t m_value;                 // +0x0 (reward count 0, the points; printed "%d" on a reached step)
	int32_t m_hasBonus;              // +0x4 (reward count 1; non-zero: the step gives an item besides the points)
	int32_t m_rewardCount3;          // +0x8 (reward count 2)
	int32_t m_special;               // +0xc (1: more than one non-zero count; drawn with the other bitmap of the pair)
};

// The data of a list_ach_element item (FrListItem::pData; the values of
// the form's map +0x63c, built by LayoutCategoryElements 0x4e8c20 from
// m_categoryData). The element list's items themselves are
// the entries the form keeps in m_elements (their FrListItem::idx is the
// row).
struct sAchElementItem {
	const JpAchievementRecord* m_pRecord; // +0x0 (the player's record, NULL when none: the dates)
	const sAchievementData* m_pData; // +0x4 (step names and ids)
	// The step record of the current step's counter id (the first step
	// without a record).
	const sAchievementStepData* m_pStep; // +0x8
	int32_t m_shown;                 // +0xc (1 while its step popup is open; 0 once ReleaseElement dropped it)
	int32_t m_points;                // +0x10 (CQuestDocument::GetRecordPoints of the record)
	int32_t m_total;                 // +0x14 (the category entry's total, sAchievementCategory +0x4)
	// The step in progress (the steps below it are reached; -1: no step
	// list to show).
	int32_t m_currentStep;           // +0x18
	int32_t m_reachedStep;           // +0x1c (the last step reached; the last one: every step is)
	float m_percent;                 // +0x20 (the current step's counter value / goal * 100, 0..100)
	float m_stepListX;               // +0x24 (list_ach_item's x past the item's)
	SYSTEMTIME m_date;               // +0x28 (the latest counter's time; zero without one)
	const Bitmap* m_pIcon;           // +0x38 (Fresh::GetBitmap of m_pData->m_icon)
	std::vector<sAchStepSlot> m_steps; // +0x3c
};
// size 0x4c (x86; the map +0x63c's 0x60-byte nodes)

// The achievement data a recent record points to (CSharedDoc +0x20c
// records; the clearing ctor 0x4e2820 gives the arrays' sizes).
struct sAchievementData {
	UNKNOWN(0x0, 0x4);
	uint32_t m_id;                   // +0x4 (the key of the quest document's +0x60 map and achievement map)
	UNKNOWN(0x8, 0x49);
	char m_icon[0x77];               // +0x49 (a Fresh bitmap name, NUL-terminated)
	int32_t m_unknownC0;             // +0xc0 (0 by 0x4e2820; use not traced)
	int32_t m_category;              // +0xc4 (-1 by 0x4e2820; FrAchievement's category 0..7; the +0x50 vector's sort key)
	char m_names[10][0x81];          // +0xc8 (one per step, NUL-terminated)
	UNKNOWN(0x5d2, 0x5d4);
	uint32_t m_stepIds[10];          // +0x5d4 (the counter id of each step; 0: none)
};

// A row of list_ach_recent (6 rows at FrAchievement +0x670, 0x1c bytes each).
struct sAchRecentRecord {
	const sAchievementData* m_pData; // +0x0
	int32_t m_nameIndex;             // +0x4 (index into m_pData->m_names)
	const Bitmap* m_pIcon;           // +0x8 (Fresh::GetBitmap of m_pData->m_icon)
	SYSTEMTIME m_date;               // +0xc (when it was achieved)
};

// 0xbef700 (JP FrListBox, no KR accessor; name ours): the index of the
// top item, -1 when the list is empty.
int JpListBox_GetTopItemIndex(FrListBox* list);

class FrAchievement : public FrForm {
public:
	static const WRTTI m_RTTI;                                        // 0xe74d44
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x4e9930

	// The page painters (RTTI .?AVCAhievementPainter@FrAchievement@...,
	// vtable 0xd04f18, size 0xc; the ctor is inlined by OnInit).
	class CAhievementPainter {
	public:
		CAhievementPainter(FrAchievement* owner) : m_pOwner(owner), m_enabled(false) {}
		virtual ~CAhievementPainter(void);                            // [0] 0x4e1290 (deleting)
		virtual void Draw(intptr_t param) = 0;                        // [1] __purecall

		FrAchievement* m_pOwner;     // +0x4
		bool m_enabled;              // +0x8 (area_summary draws page 0 only when set)
	};
	// Page 0, 1, 2 of m_pages (all three Draws written).
	class CAhievementMainContents : public CAhievementPainter {
	public:
		CAhievementMainContents(FrAchievement* owner) : CAhievementPainter(owner) {}
		virtual ~CAhievementMainContents(void);         // [0] 0x4e12c0 (deleting)
		virtual void Draw(intptr_t param);                            // [1] 0x4e5230 (vtable 0xd04f24)
		// 0x4e31e0 (name ours): the summary page's base, then the
		// category gauges.
		void DrawBase(void);
	};
	class CAhievementDetailInfo : public CAhievementPainter {
	public:
		CAhievementDetailInfo(FrAchievement* owner) : CAhievementPainter(owner) {}
		virtual ~CAhievementDetailInfo(void);           // [0] 0x4e12f0 (deleting)
		virtual void Draw(intptr_t param);                            // [1] 0x4e3550 (vtable 0xd04f30)
	};
	class CAhievementPopupInfo : public CAhievementPainter {
	public:
		CAhievementPopupInfo(FrAchievement* owner) : CAhievementPainter(owner) {}
		virtual ~CAhievementPopupInfo(void);            // [0] 0x4e1320 (deleting)
		virtual void Draw(intptr_t param);                            // [1] 0x4e3e10 (vtable 0xd04f3c)
	};

	// 0x4ea420: FrForm, the gauge arrays, the vector, map (head node 0x60
	// bytes, sentinel colour bytes +0x5c/+0x5d = 1), vector and list heads
	// (`new`, std::bad_alloc on failure), m_text, then clears +0x114..+0x144
	// (not the two areas), +0x14c..+0x250, m_recent, +0x718..+0x780 and
	// m_pDoc.
	FrAchievement(void);
	// Deletes m_pOwned[2], [1], [0], each m_pages entry, posts message
	// 0x58 to "AvatarChat" (0x4e96d0).
	virtual ~FrAchievement(void);                                     // [1] 0x4ea6e0 (deleting)

	// 0x4e9960: the three pages, then "achievement.lua" (bitmaps,
	// positions, gauge placements), the recent records and the category
	// gauges, m_text, category 0, then FrForm::OnInit.
	virtual bool OnInit(void);                                        // [32]

	CQuestDocument* m_pDoc;          // JP +0x79c (set by CQuestHandler 0x5ee950)

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe07098

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe0708c
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // FrCmdTarget [1] 0x4e13a0

	// Category buttons (declared only).
	void OnBtnSummaryInit(intptr_t param);                            // 0x4e2b80
	void OnBtnCharInit(intptr_t param);                               // 0x4e2bf0
	void OnBtnGameModeInit(intptr_t param);                           // 0x4e2c60
	void OnBtnAdventureInit(intptr_t param);                          // 0x4e2cd0
	void OnBtnStatisticsInit(intptr_t param);                         // 0x4e2d40
	void OnBtnItemInit(intptr_t param);                               // 0x4e2db0
	void OnBtnSystemInit(intptr_t param);                             // 0x4e2e20
	void OnBtnChallengeInit(intptr_t param);                          // 0x4e2e90
	void OnBtnEventInit(intptr_t param);                              // 0x4e2f00
	void OnBtnSummaryClicked(void);                                   // 0x4e9580
	void OnBtnCharClicked(void);                                      // 0x4e9590
	void OnBtnGameModeClicked(void);                                  // 0x4e95a0
	void OnBtnAdventureClicked(void);                                 // 0x4e95b0
	void OnBtnStatisticsClicked(void);                                // 0x4e95c0
	void OnBtnItemClicked(void);                                      // 0x4e95d0
	void OnBtnSystemClicked(void);                                    // 0x4e95e0
	void OnBtnChallengeClicked(void);                                 // 0x4e95f0
	void OnBtnEventClicked(void);                                     // 0x4e9600
	// 0x4e92d0: shows button 0 (summary) .. 8 (event): releases the shown
	// elements, presses that button, maps it to a category (summary -1,
	// then 1, 0, 3, 2, 4, 6, 5, 7), shows list_ach_recent only for the
	// summary, and for a category fills its elements and sets the total
	// gauge.
	void SelectCategory(int category);
	// 0x4e2930 (name ours): the category (-1: the summary) and the page
	// mode (0: summary, 1: a category 0..7), then page 0 enabled for the
	// summary, pages 1 and 2 for a category.
	void SetCategory(int category);
	// 0x4e8100 (name ours): m_categoryData = the document's achievement
	// data (+0x50, sorted by category) of that category.
	void FillCategoryElements(int category);
	// 0x4e8c20 (name ours): clears m_elementItems, then one item per
	// m_categoryData entry (stops at the first one missing from the
	// document's category map or already present).
	void LayoutCategoryElements(void);
	void OnAreaSummaryInit(intptr_t param);                           // 0x4e2f70
	void OnAreaSummaryDraw(intptr_t param);                           // 0x4e2fe0
	void OnAreaElementLayerInit(intptr_t param);                      // 0x4e3000
	// Drops the entries outside the four rows from the top one (calling
	// ReleaseElement), draws the others with page 2.
	void OnAreaElementLayerDraw(intptr_t param);                      // 0x4e6810
	void OnListAchRecentInit(intptr_t param);                         // 0x4e3070
	// The tooltip of the recent row under the mouse: "%s\n%d.%d.%d"
	// (name, year, month, day).
	void OnListAchRecentMouseMove(void);                              // 0x4e68b0
	// 0x4e14f0 (name ours): the list_ach_recent row whose x span holds the
	// point's x (both bounds excluded), -1 when none.
	int GetRecentRowAt(const WPoint& point);
	void OnListAchElementInit(intptr_t param);                        // 0x4e30e0
	void OnListAchElementDraw(intptr_t param);                        // 0x4e3150
	void OnListAchElementClicked(void);                               // 0x4e6ed0
	void OnListAchItemInit(intptr_t param);                           // 0x4e3170
	void OnListAchItemMouseMove(void);                                // 0x4e69d0
	// 0x4e29b0 (name ours): clears the item's m_shown, then
	// list_ach_item: EnableToolTip(false) (JP flags +0x1c bit 0, KR
	// m_nFlags), ClearItem (0xbf0e50) and Enable(false) (vfn +0x18).
	void ReleaseElement(FrListItem* item);
	// 0x4e29f0 (name ours): list_ach_item over the element item: x = the
	// item's + its m_stepListX + 9.0, y = the item's + 70.0, w = n steps x
	// (ach_bitmap 17's width + 2), h = its height; the same as item size.
	void PlaceStepList(FrListItem* item);
	// 0x4e2ac0 (name ours): the step (0..9) of list_ach_item whose column
	// (`width` wide from the list's x) holds the point, -1 when none.
	int GetStepAt(float width, const WPoint& point);
	// 0x4e1450 (name ours): each category gauge's value from the
	// document (0x5ed500(category) / its total * 100, floored; 0 without a
	// total).
	void UpdateCategoryGauges(void);
	// 0x4e7bb0 (name ours): the six latest reached steps of the
	// document's achievement map (0x47c5b0 per record, the lookup 0xa17750)
	// into the recent rows, then their x spans and list_ach_recent's rect.
	void FillRecentRecords(void);

private:
	// JP +0x110: not initialized by the ctor.
	UNKNOWN(0x110, 0x114);
	// JP +0x114: btn_summary, btn_char, btn_gamemode, btn_adventure,
	// btn_statistics, btn_item, btn_system, btn_challenge, btn_event (the
	// SelectCategory index).
	FrButton* m_pCategoryBtn[9];
	FrListBox* m_pAchElementList;    // JP +0x138 ("list_ach_element")
	FrListBox* m_pAchItemList;       // JP +0x13c ("list_ach_item")
	FrListBox* m_pAchRecentList;     // JP +0x140 ("list_ach_recent")
	FrArea* m_pSummaryArea;          // JP +0x144 ("area_summary")
	FrArea* m_pElementLayerArea;     // JP +0x148 ("area_element_layer")
	// Zeroed by the ctor, filled by OnInit.
	const Bitmap* m_achBitmaps[25];  // JP +0x14c ("ach_bitmap_list" entries, from 1)
	const Bitmap* m_challengeTexts[8]; // JP +0x1b0 ("Challenge_text_0".."_7")
	WPoint m_achPositions[16];       // JP +0x1d0 ("ach_position" x/y pairs)
	JpMaskGauge m_categoryGauges[8]; // JP +0x250 (8 x 0x4c)
	JpMaskGauge m_summaryGauges[4];  // JP +0x4b0 (4 x 0x4c)
	JpMaskGauge m_totalGauge;        // JP +0x5e0
	std::vector<const sAchievementData*> m_categoryData; // JP +0x62c (FillCategoryElements)
	// The element items by achievement data id (0x60-byte nodes, destroyed
	// by 0x4e8b50; built by LayoutCategoryElements, added to
	// list_ach_element by SelectCategory).
	std::map<uint32_t, sAchElementItem> m_elementItems; // JP +0x63c
	std::vector<CAhievementPainter*> m_pages; // JP +0x64c (`new`ed 0xc objects, deleted through vfn 0)
	std::list<FrListItem*> m_elements; // JP +0x65c (head), +0x660 (size): the element popups shown (list_ach_element items)
	int32_t m_category;              // JP +0x668 (SetCategory; -1: the summary)
	int32_t m_pageMode;              // JP +0x66c (0: summary page, 1: category pages)
	sAchRecentRecord m_recent[6];    // JP +0x670 (zeroed by the ctor)
	// JP +0x718..+0x780 zeroed by the ctor; +0x774, +0x778, +0x77c are
	// `new`ed objects deleted (vfn 0, flag 1) by the dtor.
	// The x spans {left, right} of the list_ach_recent rows (filled by
	// FillRecentRecords), m_recentSpanCount of them.
	float m_recentSpans[6][2];       // JP +0x718
	int32_t m_unknown748;            // JP +0x748 (zeroed by the ctor; use not traced)
	int32_t m_hoverStep;             // JP +0x74c (the step whose tooltip is set; -1: none)
	int32_t m_recentSpanCount;       // JP +0x750
	int32_t m_categoryPoints[8];     // JP +0x754 (zeroed by the ctor; UpdateCategoryGauges: the document's 0x5ed500)
	// JP +0x774 CAchieveTotalPointFont, +0x778 CAchieveUnitEarnPoint,
	// +0x77c CAchievementDate (`new`ed by OnInit after the pages), deleted
	// in reverse order by the dtor.
	WTitleFontEx* m_pOwned[3];
	std::string m_text;              // JP +0x780 ("%d" of the document's GetAchievementPoints)
	// JP +0x79c: m_pDoc (public above).
};
// size 0x7a0 (x86)

// The achievement form's digit fonts (JP TitleFontEx unit; 0xc bytes,
// WTitleFontEx subclasses; the font file entries 0xe16904, 0xe16930,
// 0xe16958).
// "[challenges_total_score.jpg": "1234567890", 27 x 33 in a 135 x 66 page.
class CAchieveTotalPointFont : public WTitleFontEx {
public:
	CAchieveTotalPointFont(void);                                     // 0x6f4dc0 (vtable 0xd1c178)
};
CHECK_SIZE(CAchieveTotalPointFont, 0xc);
// "[font_challenges_point.jpg": "1234567890", 24 x 24 in a 240 x 24 page.
class CAchieveUnitEarnPoint : public WTitleFontEx {
public:
	CAchieveUnitEarnPoint(void);                                      // 0x6f4e80 (vtable 0xd1c19c)
};
CHECK_SIZE(CAchieveUnitEarnPoint, 0xc);
// "[font_challenges_date.jpg": "1234567890.", 8 x 10 in an 88 x 10 page.
class CAchievementDate : public WTitleFontEx {
public:
	CAchievementDate(void);                                           // 0x6f4f40 (vtable 0xd1c1cc)
};
CHECK_SIZE(CAchievementDate, 0xc);

// A daily-quest reward of the 0x022E reply: {type id listed (a challenge
// when (id & 0xFC000000) == 0x4C000000), achievement record id (0xa176b0)}.
typedef std::pair<uint32_t, uint32_t> sOnedayQuestReward;

// The quest-complete form ("oneday_quest_complete", opened by
// CQuestHandler 0x5efc40; JP size 0x170). Vtables 0xd07794 ([0] GetRTTI
// 0x51b0f0, [1] deleting dtor 0x51b5b0, [32] OnInit 0x51b5e0, the rest
// FrForm's and the shared header inlines), 0xd07788 (FrCmdTarget: [0]
// 0x51b100, [1] GetMessageMap 0x517a00), 0xd07780. Message map 0xe09b44
// (rows 0xe09db8, handlers stored by the static init 0xcccdd0).
class FrOnedayQuestComplete : public FrForm {
public:
	static const WRTTI m_RTTI;                                        // 0xe74e68
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x51b0f0

	FrOnedayQuestComplete(void);                                      // 0x51b070
	virtual ~FrOnedayQuestComplete(void);                             // [1] 0x51b5b0 (deleting), 0x51b120

	// A challenge (the first type id's top six bits 0x13, i.e.
	// (id & 0xFC000000) == 0x4C000000) swaps both backgrounds, lists the
	// type ids' icons (drawn by OnBg1Draw) and moves the text 15 and the
	// gift line 18 down; then FrForm::OnInit.
	virtual bool OnInit(void);                                        // [32] 0x51b5e0

	// Fills the form's vector (+0x144) from the finished player quests
	// (0x022x key 4).
	void SetQuests(const std::vector<JpPlayerQuest*>& quests);        // 0x51b760
	// One line per reward ("  <record name> <text 0x2c0>", then the text
	// 0x2c5 with the count when more than one), stopping at the first
	// reward without a record (no text set then); the gift line lists the
	// records' points (0x6C000001, summed) and their other rewards. The
	// listed type ids go into the form's vector (+0x144).
	void SetRewards(const std::vector<sOnedayQuestReward>& rewards);  // 0x51b950
	// Whether that vector (+0x144 / +0x148) is not empty.
	bool HasRewards(void) const;                                      // 0x518b30
	// Move the form (vfn +0x8c) to its rect with the y offset by minus /
	// plus the double at 0xd076a8 (names ours).
	void ShiftUp(void);                                               // 0x518b40
	void ShiftDown(void);                                             // 0x518b90

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe09db8

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe09b44
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // FrCmdTarget [1] 0x517a00

	void OnSubjectInit(intptr_t param);                               // 0x518be0
	// The cast, then SetVisible(false) (vfn +0x1c).
	void OnGiftInit(intptr_t param);                                  // 0x518c50
	void OnBg1Init(intptr_t param);                                   // 0x518cc0
	// The icons of m_icons in a row centred on the form, 70 below its top,
	// each 50 wide in the centring and advanced by its bitmap's width.
	void OnBg1Draw(intptr_t param);                                   // 0x518d30
	void OnBg2Init(intptr_t param);                                   // 0x518e30

private:
	int32_t m_questCount;            // JP +0x110 (the quests and rewards listed)
	FrStatic* m_pQuestText;          // JP +0x114 ("subject"; its text: m_text; 0xc02420)
	FrStatic* m_pRewardText;         // JP +0x118 ("gift"; hidden until a challenge shows it)
	FrArea* m_pBg1;                  // JP +0x11c ("bg1")
	FrArea* m_pBg2;                  // JP +0x120 ("bg2")
	int32_t m_isChallenge;           // JP +0x124 (1: OnInit found a challenge type id)
	std::string m_text;              // JP +0x128 ("  <name> <text 0x2c0>" per quest)
	// The type ids listed (SetQuests: the quests'; SetRewards: the
	// rewards').
	std::vector<uint32_t> m_rewards; // JP +0x144 (begin), +0x148 (end)
	// The challenge icons (Fresh::GetBitmap of each listed type id's
	// record name +0x49; pushed by 0x51b230, the vector's push_back).
	std::vector<const Bitmap*> m_icons; // JP +0x154
};

// 0x5ee510: the JP CreateForm<FrOnedayQuestComplete>.
FrOnedayQuestComplete* CreateOnedayQuestComplete(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent);

// The factories (JP CreateForm<T> instances; the JP one also stores the
// form in a TLS slot +0xb0 before the DynamicCast).
FrOnedayQuest* CreateOnedayQuest(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent);
FrAchievement* CreateAchievement(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent);

// The CObjectFactory creators ("FrOnedayQuest", "FrAchievement").
IObject* FrOnedayQuestMakeInstance(void);                             // 0x51c480
IObject* FrAchievementMakeInstance(void);                             // 0x4ea710

} // namespace _questoneday
} // namespace _gateway

#endif // PG_JP_JPQUESTFORMS_H
