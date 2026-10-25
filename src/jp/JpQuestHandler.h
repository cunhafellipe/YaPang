// Original: (JP 983 game code, no PDB) _gateway::_questoneday::CQuestHandler
// The gateway handler of the quest document (document type 0xAC, gateway
// message 0x753A): JP x86 run 0x5ee440-0x5f01c0. It binds the keys the base
// OnPacket 0x9c20a0 posts for the replies 0x0225-0x022E
// (docs/protocol/quest.md) and opens the daily-quest form
// ("oneday_quest_main") and the achievement form ("ach_main").
//
// JP x86 facts:
// - RTTI 0xe78c0c (GetRTTI 0x5efb60), size 0x30 (new in the factory
//   creator 0x5efeb0, registered in the CObjectFactory 0xf00718 as
//   "CQuestHandler" by 0x5eff20 / static init 0xcd2050), ctor 0x5efaf0,
//   dtor 0x5efb90 (deleting 0x5eff40). The port's KR CGatewayHandler is
//   not an IObject: the creator is not written (lib: 0x5eff20, 0xcd2050).
// - Bases: the JP _gateway::CGatewayHandler (ctor 0x4bd460, vtables
//   0xd026fc / 0xd026f4 / 0xd026ec): IObject at 0, the FunctionMapper at +4
//   (its map at +8, the current key at +0x18), the document at +0x1c; the
//   handler adds FrCmdTarget at +0x20 (vtable 0xd12e44).
// - Vtable 0xd12e58: [0] GetRTTI 0x5efb60, [1] deleting dtor 0x5eff40,
//   [2] Initialize 0x5eff70, [3] Execute 0x5eed70, [4]-[8] 0x4bd520-0x4bd560
//   and [10] 0x4bd570 (the base's empty virtuals), [9] 0x4bb3d0 (the base:
//   Initialize when the function map is empty; called by the actor's
//   QueryHandler 0x91e850). Vtable at +4: 0xd12e50; FrCmdTarget's: 0xd12e44.
// - Execute dispatches through 0x5eec30 (the FunctionMapper's Exec on +4).
// The port derives from the KR CGatewayHandler (gatewayinterface.h: the
// same FunctionMapper machinery, FrCmdTarget base); JP offsets are comments.
#ifndef PG_JP_JPQUESTHANDLER_H
#define PG_JP_JPQUESTHANDLER_H

#include <stdint.h>

#include "gatewayinterface.h"

class FrForm;

namespace _gateway {
namespace _questoneday {

class FrOnedayQuest;
class FrOnedayQuestComplete;
class FrAchievement;

class CQuestHandler : public CGatewayHandler {
public:
	CQuestHandler(void);
	virtual ~CQuestHandler(void);

	virtual bool Initialize(void);                // [2] 0x5eff70
	virtual bool Execute(const MsgObject& msg);   // [3] 0x5eed70

private:
	// The bound members (key: JP address). param is the MsgObject's
	// address; the packet is its m_param2.
	void OnOnedayQuestList(intptr_t param);       // key 2: 0x5ef270 (0x0225)
	void OnOnedayQuestUpdate(intptr_t param);     // key 3: 0x5ef8a0 (0x0226)
	void OnOnedayQuestItems(intptr_t param);      // key 4: 0x5eed90
	void OnOnedayQuestDone(intptr_t param);       // key 5: 0x5ef020 (0x0227)
	void OnOnedayQuestRemoved(intptr_t param);    // key 6: 0x5ef640 (0x0228)
	void OnAchievementRecords(intptr_t param);    // key 8: 0x5ee6e0 (0x022D)
	void OnAchievementEnd(void);                  // key 9: 0x5ee910 (0x022C)
	void OnOnedayQuestReward(intptr_t param);     // key 10: 0x5efc40 (0x022E)

	// 0x5ef100: rebuilds the document's shown quest list
	// from the player's quests (CSharedDoc +0x384C) into 0x5ecd80.
	void RebuildShownQuests(void);

	// The forms' result callbacks.
	bool OnOnedayQuestResult(int ret, FrForm* pForm);    // 0x5ee790
	bool OnAchievementResult(int ret, FrForm* pForm);    // 0x5ee800
	bool OnOnedayQuestCompleteResult(int ret, FrForm* pForm); // 0x5ee160

	FrOnedayQuest* m_pOnedayQuest;    // JP +0x24
	FrOnedayQuestComplete* m_pOnedayQuestComplete; // JP +0x28 (not set by the ctor; 0x5efc40, cleared by 0x5ee160)
	FrAchievement* m_pAchievement;    // JP +0x2c
};
// size 0x30 (x86)

} // namespace _questoneday
} // namespace _gateway

// The JP base OnPacket's login-list readers (JpQuestHandler.cpp), called
// from CTask::OnPacketCommon under JP_PAKS: 0x021D, the quest progress
// list (CSharedDoc +0x3840), and 0x021E, the player's quest items
// (CSharedDoc +0x384C).
class WReceivedPacket;
void JpQuest_OnQuestProgressListPacket(WReceivedPacket& packet);  // JP case 0x9d4b1c
void JpQuest_OnPlayerQuestListPacket(WReceivedPacket& packet);    // JP case 0x9d4bb8

#endif // PG_JP_JPQUESTHANDLER_H
