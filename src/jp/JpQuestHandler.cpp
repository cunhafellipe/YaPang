// Original: (JP 983 game code, no PDB) _gateway::_questoneday::CQuestHandler
// lib: 0x5eff40 (deleting destructor), 0x5eec30 (FunctionMapper::Exec
// instance), 0x5eff20, 0xcd2050 (CObjectFactory registration), 0x5efeb0
// (creator), 0x5efb60 (GetRTTI), 0x5e9130, 0x5e9190, 0x5e90e0 (std::find_if
// instances over the CSharedDoc lists), 0x5ebad0 (the record vector's
// push_back)
#include "JpQuestHandler.h"

#include <math.h>
#include <string.h>

#include <algorithm>
#include <list>
#include <vector>

#include "JpLobbyGlue.h"
#include "JpQuestForms.h"
#include "MessageManager.h"
#include "Packet.h"
#include "TaskManager.h"
#include "actor.h"
#include "fresh.h"
#include "frdesktop.h"
#include "frform.h"
#include "frwndmanager.h"
#include "wtypes.h"

namespace _gateway {
namespace _questoneday {

// The JP-only CSharedDoc state (declared only): the player's quest list
// (+0x384C, 0x5e5f10 id, 0x5e5ee0 / 0x5e5ef0 on the item, the index
// +0x3854 by 0x5889d0) and the achievement-pending byte +0x11411.
void JpSharedDoc_RefreshPlayerQuest(unsigned long id);
bool JpSharedDoc_GetAchievementPending(void);
void JpSharedDoc_SetAchievementPending(bool pending);
// The player's quest list (CSharedDoc +0x384C, a std::list; names ours;
// first / next in JpQuestForms.h): the item whose
// unique id is `uid` (NULL when missing); removing an item unlinks its
// node, drops it from the index (+0x3854, 0x5889d0), deletes it and
// decrements the size (+0x3850).
JpPlayerQuest* JpSharedDoc_FindPlayerQuest(uint32_t uid);
void JpSharedDoc_RemovePlayerQuest(JpPlayerQuest* quest);
// Appends a copy of the item (0x642940).
void JpSharedDoc_PushPlayerQuest(const JpPlayerQuest& quest);
// The quest data's int at +0xc8 (sOnedayQuestInfo::m_showFlag; 1: shown in
// the daily list).
int32_t JpQuestData_GetShowFlag(const void* data);

// 0x5e6050 (name ours; LTCG: the id in eax): whether a quest type id is a
// type-0x78 quest of subtype 2 (bits 22..25), whose completion records its
// objectives' progress (JpPlayerQuest::SetDone).
static bool IsCountedQuestType(uint32_t typeId) {
	if ((typeId & 0xFC000000) != 0x78000000) {
		return false;
	}
	return ((typeId >> 22) & 0xF) == 2;
}

// 0x5ee310 (name ours; LTCG: the id in eax): whether a quest type id is a
// type-0x78 quest of subtype 1 or 2 (bits 22..25), kept out of the shown
// list.
static bool IsSpecialQuestType(uint32_t typeId) {
	if ((typeId & 0xFC000000) != 0x78000000) {
		return false;
	}
	return ((typeId >> 22) & 0xF) - 1 <= 1;
}

// --- JpAchievementRecord -------------------------------------------------------

// 0x47a060 (the JP stores its vtable 0xcfe704)
JpAchievementRecord::~JpAchievementRecord(void) {
}

// 0x47a080
uint32_t JpAchievementRecord::GetCounterId(uint32_t index) const {
	return m_counters[index].m_id;
}

// 0x47a0c0
int32_t JpAchievementRecord::GetCounterValue(uint32_t index) const {
	return (int32_t)m_counters[index].m_typeId;
}

// 0x47a0d0
const uint64_t* JpAchievementRecord::GetCounterCount(uint32_t index) const {
	return &m_counters[index].m_count;
}

// 0x47a150
int32_t JpAchievementRecord::GetFirstOpenIndex(void) const {
	for (uint32_t i = 0; i < 10; i++) {
		if (m_counters[i].m_id != 0 && m_counters[i].m_count == 0) {
			return (int32_t)i;
		}
	}
	return -1;
}

// 0x47a170
int32_t JpAchievementRecord::GetLastCountedIndex(void) const {
	for (int32_t i = 9; i > -1; i--) {
		if (m_counters[i].m_count != 0 && m_counters[i].m_id != 0) {
			return i;
		}
	}
	return -1;
}

// 0x47a200
JpAchievementRecord::sCounter* JpAchievementRecord::GetCounter(uint32_t index) {
	if (index >= 10) {
		return 0;
	}
	return &m_counters[index];
}

// 0x47a640 (the JP stores its vtable 0xcfe704 first)
JpAchievementRecord::JpAchievementRecord(void) : m_id(0), m_typeId(0) {
	for (int i = 0; i < 10; i++) {
		m_counters[i].m_id = 0;
		m_counters[i].m_typeId = 0;
		m_counters[i].m_count = 0;
	}
}

// 0x47a720
void JpAchievementRecord::Decode(WReceivedPacket& packet) {
	m_id = packet.Decode4();
	m_typeId = (uint32_t)packet.Decode4();
	unsigned long count = packet.Decode4();
	for (unsigned long i = 0; i < count; i++) {
		sCounter* counter = GetCounter((uint32_t)i);
		if (counter) {
			counter->m_id = (uint32_t)packet.Decode4();
			counter->m_typeId = (uint32_t)packet.Decode4();
			counter->m_count = (uint32_t)packet.Decode4();
		}
	}
}

// --- JpPlayerQuest -------------------------------------------------------------

// GetLatestCounterIndex's order (the predicate of the JP std::sort
// 0x47c2b0): the later count first.
static bool IsLaterCounter(const JpAchievementRecord::sCounter* a, const JpAchievementRecord::sCounter* b) {
	return (int64_t)a->m_count > (int64_t)b->m_count;
}

// 0x47c5b0: the vector of the ten counters' addresses (0x47c180).
int32_t JpAchievementRecord::GetLatestCounterIndex(void) const {
	std::vector<const sCounter*> counters;
	counters.reserve(10);
	for (const sCounter* it = m_counters; it != m_counters + 10; ++it) {
		counters.push_back(it);
	}
	std::sort(counters.begin(), counters.end(), IsLaterCounter);
	if (counters[0]->m_count == 0) {
		return -1;
	}
	int32_t result = 0;
	for (int32_t i = 0; i < 10; i++) {
		if (counters[0] == &m_counters[i]) {
			result = i;
			break;
		}
	}
	return result;
}

// 0x5e5c20 (the JP stores its vtable 0xd0b40c)
JpPlayerQuest::~JpPlayerQuest(void) {
}

// 0x5e5c30: the item's values, and each objective the other item uses.
void JpPlayerQuest::Assign(const JpPlayerQuest& other) {
	m_kept = other.m_kept;
	m_typeId = other.m_typeId;
	m_uid = other.m_uid;
	m_state = other.m_state;
	for (int i = 0; i < 10; i++) {
		if (other.m_objectives[i].m_typeId != 0) {
			m_objectives[i] = other.m_objectives[i];
		}
	}
}

// 0x5e5fb0
JpPlayerQuest::sObjective* JpPlayerQuest::GetObjective(uint32_t index) {
	if (index >= 10) {
		return 0;
	}
	return &m_objectives[index];
}

// 0x5e5fd0
JpPlayerQuest::sObjective* JpPlayerQuest::FindObjective(uint32_t id) {
	for (int i = 0; i < 10; i++) {
		if (m_objectives[i].m_id == id) {
			return &m_objectives[i];
		}
	}
	return 0;
}

// 0x5e6080 (the JP stores its vtable 0xd0b40c first)
JpPlayerQuest::JpPlayerQuest(void) : m_kept(false), m_typeId(0), m_uid(0), m_state(0) {
	for (int i = 0; i < 10; i++) {
		m_objectives[i].m_id = 0;
		m_objectives[i].m_typeId = 0;
		m_objectives[i].m_key = 0;
		m_objectives[i].m_progress = 0;
	}
}

// 0x5e62b0
void JpPlayerQuest::Decode(WReceivedPacket& packet) {
	m_kept = packet.Decode1() != 0;
	m_typeId = (uint32_t)packet.Decode4();
	m_uid = (uint32_t)packet.Decode4();
	m_state = (int32_t)packet.Decode4();
	unsigned long count = packet.Decode4();
	for (unsigned long i = 0; i < count; i++) {
		sObjective* objective = GetObjective((uint32_t)i);
		if (objective) {
			objective->m_id = (uint32_t)packet.Decode4();
			objective->m_typeId = (uint32_t)packet.Decode4();
			objective->m_key = (uint32_t)packet.Decode4();
			objective->m_progress = (uint32_t)packet.Decode4();
		}
	}
}

// 0x5e6350: the other item's state, and the progress of each of its
// objectives that has some.
void JpPlayerQuest::Merge(const JpPlayerQuest& other) {
	m_state = other.m_state;
	for (int i = 0; i < 10; i++) {
		uint64_t progress = other.m_objectives[i].m_progress;
		if (progress != 0) {
			sObjective* objective = FindObjective(other.m_objectives[i].m_id);
			if (objective) {
				objective->m_progress = progress;
			}
		}
	}
}

// 0x5e63b0: a quest of a counted type (IsCountedQuestType) goes to state 3
// and records the progress of each used objective; any other to state 5,
// no longer kept.
void JpPlayerQuest::SetDone(void) {
	if (IsCountedQuestType(m_typeId)) {
		m_state = 3;
		for (int i = 0; i < 10; i++) {
			if (m_objectives[i].m_id != 0) {
				sQuestProgress record;
				record.m_active = true;
				record.m_typeId = m_objectives[i].m_typeId;
				record.m_key = m_objectives[i].m_key;
				record.m_count = 0;
				JpSharedDoc_PushQuestProgress(record);
			}
		}
		return;
	}
	m_state = 5;
	m_kept = false;
}

// --- CQuestDocument -----------------------------------------------------------

// 0x5e6420
int32_t JpPlayerQuest::GetObjectiveGoal(const sObjective* objective) const {
	const int32_t* pGoal = JpSharedDoc_FindAchievementGoal(objective->m_id);
	if (pGoal == 0) {
		return 0;
	}
	return *pGoal;
}

// 0x5e6680: the JP walks the list itself (node +0x10 the key, +0x8 the
// active flag, +0x14 the count).
int32_t JpPlayerQuest::GetObjectiveProgress(const sObjective* objective) const {
	if (objective->m_key == 0) {
		return 0;
	}
	const sQuestProgress* pProgress = JpSharedDoc_FindQuestProgress(objective->m_key);
	if (pProgress == 0 || pProgress->m_active != true) {
		return 0;
	}
	return pProgress->m_count;
}

// 0x5e6780: 100.0 is the double 0xd66ca8.
float JpPlayerQuest::GetObjectivePercent(uint32_t index) const {
	const sObjective* objective = const_cast<JpPlayerQuest*>(this)->GetObjective(index);
	if (objective == 0 || objective->m_typeId == 0) {
		return 0.0f;
	}
	double progress = (double)GetObjectiveProgress(objective);
	float percent = (float)(progress / (double)GetObjectiveGoal(objective) * 100.0);
	return (float)floor(percent);
}

// 0x5e6800: 10.0 is the double 0xcff068.
float JpPlayerQuest::GetObjectiveStepPercent(uint32_t index) const {
	const sObjective* objective = const_cast<JpPlayerQuest*>(this)->GetObjective(index);
	if (objective == 0 || objective->m_typeId == 0) {
		return 0.0f;
	}
	float percent = (float)((double)(GetObjectiveProgress(objective) % 10) / 10.0 * 100.0);
	return (float)floor(percent);
}

// 0x5e7250
bool CQuestDocument::Release(void) {
	return true;
}

// 0x5e7260
bool CQuestDocument::Reset(void) {
	memset(m_dates, 0, sizeof(m_dates));
	return true;
}

// 0x5e7290
void CQuestDocument::SetDate(const SYSTEMTIME& date, int index) {
	if (index < 2) {
		m_dates[index] = date;
	}
}

// 0x5e7fd0
bool CQuestDocument::HasQuests(void) const {
	return m_shownQuests.begin() != m_shownQuests.end();
}

// 0x5e9620
sQuestDocRecord* CQuestDocument::FindShownQuest(unsigned long uid) {
	std::vector<sQuestDocRecord>::iterator it = m_shownQuests.begin();
	for (; it != m_shownQuests.end(); ++it) {
		if (it->m_pPlayerQuest != 0 && it->m_pPlayerQuest->GetUid() == uid) {
			break;
		}
	}
	if (it == m_shownQuests.end()) {
		return 0;
	}
	return &*it;
}

// 0x5ea470 (the map lookup 0x5e9ea0)
const std::vector<const sAchievementStepData*>* CQuestDocument::FindCategorySteps(uint32_t id) {
	std::map<unsigned long, sAchievementCategory>::iterator it = m_categories.find(id);
	if (it == m_categories.end()) {
		return 0;
	}
	return &it->second.m_steps;
}

// 0x5eb9f0
void CQuestDocument::AddPendingAchievement(const JpAchievementRecord& record) {
	if (m_pendingAchievements.find(record.GetId()) == m_pendingAchievements.end()) {
		m_pendingAchievements.insert(std::pair<const unsigned long, JpAchievementRecord>(record.GetId(), record));
	}
}

// 0x5ebee0 (LTCG: the vector in ecx, the item in edx): the item's record
// when it is in state `kind` or 4.
void CQuestDocument::AddShownQuest(std::vector<sQuestDocRecord>& records, const JpPlayerQuest* quest, int kind) {
	if (!quest->IsState(kind) && !quest->IsState(4)) {
		return;
	}

	sQuestDocRecord record;
	record.m_pQuestData = JpSharedDoc_FindQuestData(quest->GetTypeId());
	record.m_pPlayerQuest = quest;
	// 0x5e90e0: the progress record of the item's first objective.
	record.m_pProgress = JpSharedDoc_FindQuestProgress(quest->GetObjectiveKey(0));
	records.push_back(record);
}

// 0x5ebf80 (LTCG: the vector in ecx, the state in edx)
void CQuestDocument::AddQuestRecord(std::vector<sQuestDocRecord>& records, unsigned long id, bool noProgress, int kind) {
	// 0x5e9130: the first player quest item of this type id in state `kind`.
	const JpPlayerQuest* quest = JpSharedDoc_FirstPlayerQuest();
	while (quest && !(quest->IsState(kind) && quest->GetTypeId() == id)) {
		quest = JpSharedDoc_NextPlayerQuest(quest);
	}
	if (quest == 0) {
		return;
	}
	if (!quest->IsState(kind)) {
		return;
	}

	sQuestDocRecord record;
	record.m_pQuestData = JpSharedDoc_FindQuestData(quest->GetTypeId());
	record.m_pPlayerQuest = quest;
	if (noProgress) {
		record.m_pProgress = 0;
	} else {
		// The JP leaves the pointer unset when the item has no progress
		// record (an uninitialized read): NULL here.
		record.m_pProgress = JpSharedDoc_FindQuestProgress(quest->GetUid());
	}
	records.push_back(record);
}

// 0x5ec050
void CQuestDocument::SetQuestIds(const std::vector<unsigned long>& ids) {
	m_questRecords.clear();
	for (std::vector<unsigned long>::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		AddQuestRecord(m_questRecords, *it, false, 1);
	}
}

// 0x5ecd80
void CQuestDocument::SetShownQuests(const std::vector<const JpPlayerQuest*>& quests) {
	m_shownQuests.clear();
	for (std::vector<const JpPlayerQuest*>::const_iterator it = quests.begin(); it != quests.end(); ++it) {
		AddShownQuest(m_shownQuests, *it, 3);
	}
}

// 0x5ecde0
void CQuestDocument::FinishAchievements(void) {
	m_achievements.swap(m_pendingAchievements);
	m_pendingAchievements.clear();
}

// Inlined in the JP (0x5ef8a0).
void CQuestDocument::ClearQuestRecords(void) {
	m_questRecords.clear();
}

// Inlined in the JP (the size of the map +0x70, read at +0x78).
int CQuestDocument::GetAchievementCount(void) const {
	return (int)m_achievements.size();
}

// 0x5ed3d0: each record whose counters reached a step (0x47a170) adds
// the points (+0xf4) of the steps 0..that one of its category entry
// (+0x60 by the record's id). The port: the +0x60 map is filled by
// 0x5ed780 from the item manager's achievement table, neither of which
// is ported, so an entry may have fewer steps than the index; the walk
// stays inside them.
int CQuestDocument::GetAchievementPoints(void) {
	if (m_achievements.size() == 0) {
		return 0;
	}
	int points = 0;
	for (std::map<unsigned long, JpAchievementRecord>::const_iterator it = m_achievements.begin(); it != m_achievements.end(); ++it) {
		int32_t last = it->second.GetLastCountedIndex();
		if (last == -1) {
			continue;
		}
		const sAchievementCategory& category = m_categories[it->second.GetId()];
		for (int32_t i = 0; i <= last && i < (int32_t)category.m_steps.size(); i++) {
			points += category.m_steps[i]->m_rewardCounts[0];
		}
	}
	return points;
}

// 0x5ed500: the data of the category (std::equal_range 0x5e9a30 on +0xc4
// over the +0x50 vector), each found in the achievement map by its id
// (+0x4); the port's +0x60 walk stays inside the steps (see
// GetAchievementPoints).
int CQuestDocument::GetCategoryPoints(int category) {
	if (category >= 8) {
		return 0;
	}
	int points = 0;
	for (std::vector<const sAchievementData*>::const_iterator it = m_achievementData.begin(); it != m_achievementData.end(); ++it) {
		if ((*it)->m_category != category) {
			continue;
		}
		std::map<unsigned long, JpAchievementRecord>::const_iterator found = m_achievements.find((*it)->m_id);
		if (found == m_achievements.end()) {
			continue;
		}
		int32_t last = found->second.GetLastCountedIndex();
		if (last == -1) {
			continue;
		}
		const sAchievementCategory& entry = m_categories[found->second.GetId()];
		for (int32_t i = 0; i <= last && i < (int32_t)entry.m_steps.size(); i++) {
			points += entry.m_steps[i]->m_rewardCounts[0];
		}
	}
	return points;
}

// 0x5ed6c0 (the +0x60 lookup 0x5ed150, the map's operator[]); the port's
// walk stays inside the steps (see GetAchievementPoints).
int CQuestDocument::GetRecordPoints(const JpAchievementRecord* record) {
	if (record == 0) {
		return 0;
	}
	int32_t last = record->GetLastCountedIndex();
	if (last == -1) {
		return 0;
	}
	const sAchievementCategory& entry = m_categories[record->GetId()];
	int points = 0;
	for (int32_t i = 0; i <= last && i < (int32_t)entry.m_steps.size(); i++) {
		points += entry.m_steps[i]->m_rewardCounts[0];
	}
	return points;
}

// 0x5edaf0 (member destruction not traced further)
CQuestDocument::~CQuestDocument(void) {
}

// 0x5edc30
CGatewayHandler* CQuestDocument::GetGatewayHandler(void) {
	return m_pGatewayHandler;
}

// 0x5edc40: the handler is made by the CObjectFactory ("CQuestHandler",
// the name *0xe12f44; the port's KR CGatewayHandler is no IObject, so
// `new`), and its Initialize is called here because the KR QueryHandler
// does not (the JP one calls the handler's vfn [9] 0x4bb3d0). The JP
// passes 10 of the 11 rows to InsertMessageGroup (0x919260 on the
// CMessageManager 0xe74e58), so 0x2c6 is not registered. The texts go
// through Jp_LookupText 0xb4eb80 in the JP: not applied here.
// Then (not decompiled): the vector +0x50 is cleared, +0x90 (0x20 bytes)
// zeroed, 0xa20180(CSharedDoc +0x20c, +0x50), 0x5ec0d0 and 0x5ed780.
bool CQuestDocument::Initialize(void) {
	m_pGatewayHandler = new CQuestHandler;
	if (m_pGatewayHandler) {
		m_pGatewayHandler->Initialize();
	}

	_systemmsg::sMsgInfo msgInfo[] = {
		{ (_systemmsg::eMsgIdentifier)0x2bc, "\xbf\xc0\xb4\xc3\xc0\xc7 \xc4\xf9\xbd\xba\xc6\xae\xb8\xa6 \xb9\xde\xc0\xb8\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?" },
		{ (_systemmsg::eMsgIdentifier)0x2bd, "\xc4\xf9\xbd\xba\xc6\xae\xb0\xa1 \xb0\xa1\xb5\xe6\xc2\xf7 \xb9\xde\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9." },
		{ (_systemmsg::eMsgIdentifier)0x2be, "\xc4\xf9\xbd\xba\xc6\xae\xb8\xa6 \xc6\xf7\xb1\xe2\xc7\xcf\xb8\xe9, \xb9\xde\xc1\xf6 \xbe\xca\xc0\xba \xc4\xf9\xbd\xba\xc6\xae \xba\xb8\xbb\xf3\xc0\xcc \xb8\xf0\xb5\xce \xbb\xe7\xb6\xf3\xc1\xfd\xb4\xcf\xb4\xd9.\n\xc4\xf9\xbd\xba\xc6\xae\xb8\xa6 \xc0\xfc\xba\xce \xc6\xf7\xb1\xe2\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?" },
		{ (_systemmsg::eMsgIdentifier)0x2bf, "%d\xb0\xb3 \xc4\xf9\xbd\xba\xc6\xae\xb8\xa6 \xbf\xcf\xb7\xe1\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9." },
		{ (_systemmsg::eMsgIdentifier)0x2c0, "\xbc\xba\xb0\xf8" },
		{ (_systemmsg::eMsgIdentifier)0x2c1, "\xc4\xf9\xbd\xba\xc6\xae \xc7\xd8\xb0\xe1" },
		{ (_systemmsg::eMsgIdentifier)0x2c2, "\xc4\xf9\xbd\xba\xc6\xae \xbe\xf8\xc0\xbd" },
		{ (_systemmsg::eMsgIdentifier)0x2c3, "\xc4\xf9\xbd\xba\xc6\xae \xba\xb8\xbb\xf3\xc0\xbb \xb9\xde\xc0\xb8\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?" },
		{ (_systemmsg::eMsgIdentifier)0x2c4, "\xc6\xaf\xba\xb0 \xc4\xf9\xbd\xba\xc6\xae \xba\xb8\xbb\xf3\xc0\xbb \xb9\xde\xc0\xb8\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?" },
		{ (_systemmsg::eMsgIdentifier)0x2c5, "%d\xb0\xb3 \xbe\xf7\xc0\xfb\xc0\xbb \xbf\xcf\xb7\xe1\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9." },
		{ (_systemmsg::eMsgIdentifier)0x2c6, "\xbe\xf7\xc0\xfb \xc1\xf8\xc7\xe0 \xb1\xe2\xb7\xcf\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9" },
	};
	WSingleton<_systemmsg::CMessageManager>::Instance()->InsertMessageGroup(msgInfo, 10);

	return true;
}

// Not located in the JP (the constructor is inlined into the creator).
CQuestDocument::CQuestDocument(void) : m_pGatewayHandler(0) {
	memset(m_dates, 0, sizeof(m_dates));
	memset(m_categoryTotals, 0, sizeof(m_categoryTotals));
}

IContentsDataContainer* JpCreateQuestDocument(void) {
	return new CQuestDocument;
}

// --- CQuestHandler -------------------------------------------------------------

// 0x5efaf0
CQuestHandler::CQuestHandler(void) : m_pOnedayQuest(0) {
}

// 0x5efb90
CQuestHandler::~CQuestHandler(void) {
}

// 0x5ee910
void CQuestHandler::OnAchievementEnd(void) {
	CQuestDocument* pDoc = GetDocument<CQuestDocument*>();
	pDoc->FinishAchievements();
	if (pDoc->GetAchievementCount() == 0) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)WSingleton<_systemmsg::CMessageManager>::Instance()->GetMsg((_systemmsg::eMsgIdentifier)0x2c6), 0, 0, 0, 0));
		JpSharedDoc_SetAchievementPending(false);
		return;
	}

	if (JpSharedDoc_GetAchievementPending()) {
		JpSharedDoc_SetAchievementPending(false);
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x356, 0, 0, 0, 0, 0));
		return;
	}

	// GetActor("LobbyAvatar"), then its HandleMsg (vfn +0x2c).
	AfxGetTask()->PassMsgToActor("LobbyAvatar", MsgObject(0, 0x1a3, 0, 0, 0, 0, 0));

	FrWnd* pShop = g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop");
	if (pShop) {
		// 0x8b4f20 (the shop takes the document's records, 0x5ed3d0):
		// not decompiled.
		return;
	}

	m_pAchievement = CreateAchievement(g_pFresh->GetManager(), this, "ach_main", 0);
	m_pAchievement->m_pDoc = pDoc;
	// JP vfn +0x78(0): not mapped to a KR FrWnd slot yet.
	m_pAchievement->SetTopmost(true);
	m_pAchievement->FindNextTopFocus(true);
	m_pAchievement->Open((FRESH_RESULT_CALLBACK)&CQuestHandler::OnAchievementResult, 1);
	// The JP CreateForm's TLS slot (+0xb0 of 0xe73e10 when 0xe78c3c == 1)
	// has no port counterpart.
}

// 0x5ee160
bool CQuestHandler::OnOnedayQuestCompleteResult(int ret, FrForm* pForm) {
	m_pOnedayQuestComplete = 0;
	return true;
}

// 0x5ee6e0
void CQuestHandler::OnAchievementRecords(intptr_t param) {
	CQuestDocument* pDoc = GetDocument<CQuestDocument*>();
	WReceivedPacket* pPacket = (WReceivedPacket*)((const MsgObject*)param)->m_param2;
	pPacket->Decode4();
	unsigned long count = pPacket->Decode4();
	for (unsigned long i = count; i != 0; i--) {
		JpAchievementRecord record;
		record.Decode(*pPacket);
		pDoc->AddPendingAchievement(record);
	}
}

// 0x5ee790
bool CQuestHandler::OnOnedayQuestResult(int ret, FrForm* pForm) {
	m_pOnedayQuest = 0;
	// GetActor("LobbyAvatar"), then its HandleMsg (vfn +0x2c).
	AfxGetTask()->PassMsgToActor("LobbyAvatar", MsgObject(0, 0x1a3, 1, 0, 0, 0, 0));
	return true;
}

// 0x5ee800
bool CQuestHandler::OnAchievementResult(int ret, FrForm* pForm) {
	m_pAchievement = 0;
	// GetActor("LobbyAvatar"), then its HandleMsg (vfn +0x2c).
	AfxGetTask()->PassMsgToActor("LobbyAvatar", MsgObject(0, 0x1a3, 1, 0, 0, 0, 0));
	return true;
}

// 0x5eed70
bool CQuestHandler::Execute(const MsgObject& msg) {
	SetFunctionType(msg.m_param1);
	return Exec(this, (intptr_t)&msg);
}

// 0x5eed90
void CQuestHandler::OnOnedayQuestItems(intptr_t param) {
	WReceivedPacket* pPacket = (WReceivedPacket*)((const MsgObject*)param)->m_param2;
	std::vector<JpPlayerQuest*> finished;
	unsigned long count = pPacket->Decode4();
	for (unsigned long i = count; i != 0; i--) {
		JpPlayerQuest update;
		update.Decode(*pPacket);
		JpPlayerQuest* quest = JpSharedDoc_FindPlayerQuest(update.GetUid());
		if (quest) {
			quest->Merge(update);
			if ((quest->GetTypeId() & 0x3C00000) != 0x400000 && update.IsState(4)
				&& (update.GetTypeId() & 0xFC000000) != 0x4C000000) {
				finished.push_back(quest);
			}
		}
	}

	if (finished.begin() == finished.end()) {
		return;
	}
	for (std::vector<JpPlayerQuest*>::iterator it = finished.begin(); it != finished.end(); ++it) {
		if (((*it)->GetTypeId() & 0xFC000000) == 0x4C000000) {
			return;
		}
	}

	m_pOnedayQuestComplete = CreateOnedayQuestComplete(g_pFresh->GetManager(), this, "oneday_quest_complete", 0);
	m_pOnedayQuestComplete->SetQuests(finished);
	if (!m_pOnedayQuestComplete->HasRewards()) {
		return;
	}
	// JP vfn +0x78(0): not mapped to a KR FrWnd slot yet.
	m_pOnedayQuestComplete->ShiftUp();
	m_pOnedayQuestComplete->SetTopmost(true);
	m_pOnedayQuestComplete->FindNextTopFocus(true);
	m_pOnedayQuestComplete->Open((FRESH_RESULT_CALLBACK)&CQuestHandler::OnOnedayQuestCompleteResult, 1);
}

// 0x5ef020
void CQuestHandler::OnOnedayQuestDone(intptr_t param) {
	WReceivedPacket* pPacket = (WReceivedPacket*)((const MsgObject*)param)->m_param2;
	pPacket->Decode4();
	uint32_t uid = (uint32_t)pPacket->Decode4();
	JpPlayerQuest* quest = JpSharedDoc_FindPlayerQuest(uid);
	if (quest == 0) {
		return;
	}

	quest->SetDone();
	if (m_pOnedayQuest) {
		if ((quest->GetTypeId() & 0x3C00000) == 0) {
			m_pOnedayQuest->OnQuestDone(quest);
		} else {
			m_pOnedayQuest->OnTypedQuestDone(quest);
		}
	}
	if (!quest->IsKept()) {
		JpSharedDoc_RemovePlayerQuest(quest);
	}
}

// 0x5ef100
void CQuestHandler::RebuildShownQuests(void) {
	std::vector<const JpPlayerQuest*> quests;
	for (const JpPlayerQuest* quest = JpSharedDoc_FirstPlayerQuest(); quest; quest = JpSharedDoc_NextPlayerQuest(quest)) {
		if (quest->IsState(1)) {
			continue;
		}
		if (IsSpecialQuestType(quest->GetTypeId())) {
			continue;
		}
		const void* data = JpSharedDoc_FindQuestData(quest->GetTypeId());
		if (data && JpQuestData_GetShowFlag(data) == 1) {
			quests.push_back(quest);
		}
	}
	GetDocument<CQuestDocument*>()->SetShownQuests(quests);
}

// 0x5ef270
void CQuestHandler::OnOnedayQuestList(intptr_t param) {
	const MsgObject* pMsg = (const MsgObject*)param;
	WReceivedPacket* pPacket = (WReceivedPacket*)pMsg->m_param2;
	CQuestDocument* pDoc = GetDocument<CQuestDocument*>();
	pDoc->Reset();

	unsigned long date0 = pPacket->Decode4();
	unsigned long date1 = pPacket->Decode4();
	SYSTEMTIME date;
	if (date0) {
		Jp_SystemTimeFromTime(&date, date0);   // 0xb30710
		pDoc->SetDate(date, 0);
	}
	if (date1) {
		Jp_SystemTimeFromTime(&date, date1);
		pDoc->SetDate(date, 1);
	}

	std::vector<unsigned long> ids;
	unsigned long count = pPacket->Decode4();
	for (unsigned long i = 0; i < count; i++) {
		ids.push_back(pPacket->Decode4());
	}

	unsigned long removed = pPacket->Decode4();
	for (unsigned long i = 0; i < removed; i++) {
		JpSharedDoc_RefreshPlayerQuest(pPacket->Decode4());
	}

	pDoc->SetQuestIds(ids);
	RebuildShownQuests();

	// GetActor("LobbyAvatar"), then its HandleMsg (vfn +0x2c).
	AfxGetTask()->PassMsgToActor("LobbyAvatar", MsgObject(0, 0x1a3, 0, 0, 0, 0, 0));

	m_pOnedayQuest = CreateOnedayQuest(g_pFresh->GetManager(), this, "oneday_quest_main", 0);
	WRect rect = m_pOnedayQuest->GetRect();
	rect.y = (float)(rect.y + 60.0);
	rect.x = (float)(rect.x + 30.0);
	m_pOnedayQuest->MoveWindow(WPoint(rect.x, rect.y));
	// JP vfn +0x78(0): not mapped to a KR FrWnd slot yet.
	m_pOnedayQuest->m_pDoc = pDoc;
	m_pOnedayQuest->Open((FRESH_RESULT_CALLBACK)&CQuestHandler::OnOnedayQuestResult, 1);
	// The JP CreateForm's TLS slot (+0xb0 of 0xe73e10 when 0xe78c3c == 1)
	// has no port counterpart.
}

// 0x5ef640
void CQuestHandler::OnOnedayQuestRemoved(intptr_t param) {
	WReceivedPacket* pPacket = (WReceivedPacket*)((const MsgObject*)param)->m_param2;
	unsigned long count = pPacket->Decode4();
	for (unsigned long i = 0; i < count; i++) {
		JpPlayerQuest* quest = JpSharedDoc_FindPlayerQuest((uint32_t)pPacket->Decode4());
		if (quest == 0) {
			break;
		}
		quest->ClearKept();
		if (!quest->IsKept()) {
			JpSharedDoc_RemovePlayerQuest(quest);
		}
	}

	RebuildShownQuests();
	if (m_pOnedayQuest) {
		m_pOnedayQuest->RefreshQuests();
	}
}

// 0x5ef8a0
void CQuestHandler::OnOnedayQuestUpdate(intptr_t param) {
	WReceivedPacket* pPacket = (WReceivedPacket*)((const MsgObject*)param)->m_param2;
	CQuestDocument* pDoc = GetDocument<CQuestDocument*>();
	unsigned long count = pPacket->Decode4();
	for (unsigned long i = 0; i < count; i++) {
		JpPlayerQuest update;
		update.Decode(*pPacket);
		JpPlayerQuest* quest = JpSharedDoc_FindPlayerQuest(update.GetUid());
		if (quest == 0) {
			break;
		}
		quest->Assign(update);
	}
	pDoc->ClearQuestRecords();

	RebuildShownQuests();
	if (m_pOnedayQuest) {
		m_pOnedayQuest->RefreshQuests();
	}
}

// 0x5efc40
void CQuestHandler::OnOnedayQuestReward(intptr_t param) {
	WReceivedPacket* pPacket = (WReceivedPacket*)((const MsgObject*)param)->m_param2;
	std::vector<sOnedayQuestReward> rewards;
	unsigned long count = pPacket->Decode4();
	for (unsigned long i = 0; i < count; i++) {
		uint32_t typeId = (uint32_t)pPacket->Decode4();
		uint32_t value = (uint32_t)pPacket->Decode4();
		if ((typeId & 0xFC000000) == 0x78000000) {
			return;
		}
		rewards.push_back(sOnedayQuestReward(typeId, value));
	}

	if (rewards.begin() == rewards.end()) {
		return;
	}

	m_pOnedayQuestComplete = CreateOnedayQuestComplete(g_pFresh->GetManager(), this, "oneday_quest_complete", 0);
	m_pOnedayQuestComplete->SetRewards(rewards);
	if (!m_pOnedayQuestComplete->HasRewards()) {
		return;
	}
	// JP vfn +0x78(0): not mapped to a KR FrWnd slot yet.
	m_pOnedayQuestComplete->ShiftDown();
	m_pOnedayQuestComplete->SetTopmost(true);
	m_pOnedayQuestComplete->FindNextTopFocus(true);
	m_pOnedayQuestComplete->Open((FRESH_RESULT_CALLBACK)&CQuestHandler::OnOnedayQuestCompleteResult, 0x801);
}

// 0x5eff70
bool CQuestHandler::Initialize(void) {
	AddFunctionType(2, FUNCTION_TYPE_VP, (FUNCTION_MAPPER_PFN)(void (CQuestHandler::*)(void))&CQuestHandler::OnOnedayQuestList);
	AddFunctionType(3, FUNCTION_TYPE_VP, (FUNCTION_MAPPER_PFN)(void (CQuestHandler::*)(void))&CQuestHandler::OnOnedayQuestUpdate);
	AddFunctionType(4, FUNCTION_TYPE_VP, (FUNCTION_MAPPER_PFN)(void (CQuestHandler::*)(void))&CQuestHandler::OnOnedayQuestItems);
	AddFunctionType(5, FUNCTION_TYPE_VP, (FUNCTION_MAPPER_PFN)(void (CQuestHandler::*)(void))&CQuestHandler::OnOnedayQuestDone);
	AddFunctionType(6, FUNCTION_TYPE_VP, (FUNCTION_MAPPER_PFN)(void (CQuestHandler::*)(void))&CQuestHandler::OnOnedayQuestRemoved);
	AddFunctionType(8, FUNCTION_TYPE_VP, (FUNCTION_MAPPER_PFN)(void (CQuestHandler::*)(void))&CQuestHandler::OnAchievementRecords);
	AddFunctionType(9, FUNCTION_TYPE_VV, (FUNCTION_MAPPER_PFN)&CQuestHandler::OnAchievementEnd);
	AddFunctionType(10, FUNCTION_TYPE_VP, (FUNCTION_MAPPER_PFN)(void (CQuestHandler::*)(void))&CQuestHandler::OnOnedayQuestReward);

	return true;
}

// --- CSharedDoc glue ------------------------------------------------------------

// The quest progress list (JP CSharedDoc +0x3840, a std::list<sQuestProgress>):
// the KR CSharedDoc has no such member, so the port keeps it here.
static std::list<sQuestProgress>& JpQuestProgressList(void) {
	static std::list<sQuestProgress> s_list;
	return s_list;
}

// 0x641470
void JpSharedDoc_PushQuestProgress(const sQuestProgress& record) {
	JpQuestProgressList().push_back(record);
}

// The player's quest list (JP CSharedDoc +0x384C, a std::list of item
// pointers, its size +0x3850) and the achievement-pending byte +0x11411:
// the KR CSharedDoc has neither, so the port keeps them here. The JP's
// filler of +0x384C is not located: the list stays empty until it is.
static std::list<JpPlayerQuest*>& JpPlayerQuestList(void) {
	static std::list<JpPlayerQuest*> s_list;
	return s_list;
}
static bool s_achievementPending = false;

JpPlayerQuest* JpSharedDoc_FirstPlayerQuest(void) {
	std::list<JpPlayerQuest*>& list = JpPlayerQuestList();
	if (list.empty()) {
		return 0;
	}
	return list.front();
}

JpPlayerQuest* JpSharedDoc_NextPlayerQuest(const JpPlayerQuest* quest) {
	std::list<JpPlayerQuest*>& list = JpPlayerQuestList();
	for (std::list<JpPlayerQuest*>::iterator it = list.begin(); it != list.end(); ++it) {
		if (*it == quest) {
			++it;
			if (it == list.end()) {
				return 0;
			}
			return *it;
		}
	}
	return 0;
}

// The JP std::find_if 0x5e9130 by the item's uid (0x5e5f10).
JpPlayerQuest* JpSharedDoc_FindPlayerQuest(uint32_t uid) {
	std::list<JpPlayerQuest*>& list = JpPlayerQuestList();
	for (std::list<JpPlayerQuest*>::iterator it = list.begin(); it != list.end(); ++it) {
		if ((*it)->GetUid() == uid) {
			return *it;
		}
	}
	return 0;
}

// 0x642940: the JP list holds the items themselves (a 0x110 node built by
// 0x588870, the item copy-constructed at node +0x8 by 0x587da0); the
// port's list holds a copy by pointer.
void JpSharedDoc_PushPlayerQuest(const JpPlayerQuest& quest) {
	JpPlayerQuestList().push_back(new JpPlayerQuest(quest));
}

// The JP unlinks the node, drops the item from the index +0x3854
// (0x5889d0; the port has no index) and deletes it.
void JpSharedDoc_RemovePlayerQuest(JpPlayerQuest* quest) {
	std::list<JpPlayerQuest*>& list = JpPlayerQuestList();
	for (std::list<JpPlayerQuest*>::iterator it = list.begin(); it != list.end(); ++it) {
		if (*it == quest) {
			list.erase(it);
			delete quest;
			return;
		}
	}
}

bool JpSharedDoc_GetAchievementPending(void) {
	return s_achievementPending;
}

void JpSharedDoc_SetAchievementPending(bool pending) {
	s_achievementPending = pending;
}

// The lookup by key (in the JP, the std::find_if instances listed in lib).
const sQuestProgress* JpSharedDoc_FindQuestProgress(uint32_t key) {
	std::list<sQuestProgress>& list = JpQuestProgressList();
	for (std::list<sQuestProgress>::const_iterator it = list.begin(); it != list.end(); ++it) {
		if (it->m_key == key) {
			return &*it;
		}
	}
	return 0;
}

// 0xa17700: the JP item manager's (CSharedDoc +0x20c) quest table, a map
// at +0x338 (past the achievement table +0x328 and every map the KR
// CItemManager has; the KR m_questMap holds the compound recipes, a
// different record) has no KR counterpart and the port none: never found.
const void* JpSharedDoc_FindQuestData(uint32_t typeId) {
	(void)typeId;
	return 0;
}

// The record's goal, through JpSharedDoc_FindAchievementStep.
const int32_t* JpSharedDoc_FindAchievementGoal(uint32_t id) {
	const sAchievementStepData* pStep = JpSharedDoc_FindAchievementStep(id);
	if (pStep == 0) {
		return 0;
	}
	return &pStep->m_goal;
}

// 0xa176b0: the achievement table (the item manager's +0x328; see
// Jp_FindAchievementName) has no KR counterpart and the port none.
const sAchievementStepData* JpSharedDoc_FindAchievementStep(uint32_t id) {
	(void)id;
	return 0;
}

// 0xa238f0: the item manager's list +0x34c has no KR counterpart and the
// port none: no id is hidden.
void JpSharedDoc_GetHiddenRewardIds(std::vector<uint32_t>& ids) {
	(void)ids;
}

// 0xa17750: the same item manager's map at +0x180 (the record is the map
// value, node +0x10); the port has no such table either.
const sAchievementData* JpSharedDoc_FindAchievementData(uint32_t typeId) {
	(void)typeId;
	return 0;
}

} // namespace _questoneday
} // namespace _gateway

// --- The login lists (JP base OnPacket 0x9c20a0, jump table 0x9d8320) --------

namespace {

// 0x9b90e0 (a member of the record in the JP): u8 active, u32 type id,
// u32 key, u32 count.
void ReadQuestProgress(_gateway::_questoneday::sQuestProgress& record, WReceivedPacket& packet) {
	record.m_active = packet.Decode1() != 0;
	record.m_typeId = (uint32_t)packet.Decode4();
	record.m_key = (uint32_t)packet.Decode4();
	record.m_count = (int32_t)packet.Decode4();
}

} // namespace

// JP case 0x9d4b1c ([35], id 0x021D): u32 result (non-zero: main actor
// message 0x27 with the code, nothing else), u32 total, u32 n, n records
// (the record zeroed by 0x585520, then read by 0x9b90e0) appended to
// CSharedDoc +0x3840 (0x641470); when total == n, main actor message 0x2D
// with 0x20.
void JpQuest_OnQuestProgressListPacket(WReceivedPacket& packet) {
	int32_t result = (int32_t)packet.Decode4();
	if (result != 0) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x27, result, 0, 0, 0, 0));
		return;
	}
	uint32_t total = (uint32_t)packet.Decode4();
	uint32_t count = (uint32_t)packet.Decode4();
	for (uint32_t i = 0; i < count; i++) {
		_gateway::_questoneday::sQuestProgress record;
		record.m_active = false;
		record.m_typeId = 0;
		record.m_key = 0;
		record.m_count = 0;
		ReadQuestProgress(record, packet);
		_gateway::_questoneday::JpSharedDoc_PushQuestProgress(record);
	}
	if (total == count) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x2d, 0x20, 0, 0, 0, 0));
	}
}

// JP case 0x9d4bb8 ([36], id 0x021E): u32 result (non-zero: main actor
// message 0x27 with the code), u32 total, u32 n, n player quest items
// (JpPlayerQuest::Decode) appended to CSharedDoc +0x384C (0x642940); when
// total == n, main actor message 0x2D with 0x40.
void JpQuest_OnPlayerQuestListPacket(WReceivedPacket& packet) {
	int32_t result = (int32_t)packet.Decode4();
	if (result != 0) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x27, result, 0, 0, 0, 0));
		return;
	}
	uint32_t total = (uint32_t)packet.Decode4();
	uint32_t count = (uint32_t)packet.Decode4();
	for (uint32_t i = 0; i < count; i++) {
		_gateway::_questoneday::JpPlayerQuest quest;
		quest.Decode(packet);
		_gateway::_questoneday::JpSharedDoc_PushPlayerQuest(quest);
	}
	if (total == count) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x2d, 0x40, 0, 0, 0, 0));
	}
}
