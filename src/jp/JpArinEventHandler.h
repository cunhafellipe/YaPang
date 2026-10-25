// Original: (JP 983 game code, no PDB) _gateway::CArinBirthDay2014Handler
// The gateway handler of the 2014 Arin birthday event (JpArinEvent.h): JP
// x86 run 0x477760-0x478cc2 (its bound members and the function-map
// helpers), constructor and destructor in the event document's run
// (0x920fd0-0x921240).
//
// JP x86 facts:
// - Size 0x28 (new in the factory creator 0x9211c0), ctor 0x920fd0 (the
//   JP CGatewayHandler ctor 0x4bd460, FrCmdTarget 0xc08500 at +0x20,
//   +0x24 cleared), dtor 0x921060, deleting dtor 0x921110.
// - Bases as CQuestHandler's (JpQuestHandler.h): IObject at 0, the
//   FunctionMapper at +4 (map +8, current key +0x18), the document at +0x1c
//   (the event state, CArinBirthDay2014Doc), FrCmdTarget at +0x20
//   (vtable 0xd3c8f4).
// - Vtable 0xd3c908: [0] 0x4bd590, [1] deleting dtor 0x921110, [2]
//   Initialize 0x478ab0, [3] Execute 0x478a30, [4]-[8] 0x4bd520-0x4bd560,
//   [9] 0x4bb3d0, [10] 0x4bd570. Vtable at +4: 0xd3c900.
// - Execute dispatches through 0x4788e0 (the FunctionMapper's Exec on +4);
//   Initialize's AddFunctionType is 0x478870 + 0x478600 (map insert).
// The port derives from the KR CGatewayHandler (gatewayinterface.h); JP
// offsets are comments. How the JP gateway reaches it and the KR hooks the
// port needs: temps/orchestrator/refresh_notes/JpArinEvent.md.
#ifndef PG_JP_JPARINEVENTHANDLER_H
#define PG_JP_JPARINEVENTHANDLER_H

#include <stdint.h>

#include "gatewayinterface.h"

namespace _gateway {

struct CArinBirthDay2014Doc;

class CArinBirthDay2014Handler : public CGatewayHandler {
public:
	CArinBirthDay2014Handler(void);             // 0x920fd0
	virtual ~CArinBirthDay2014Handler(void);    // 0x921060

	virtual bool Initialize(void);              // [2] 0x478ab0
	virtual bool Execute(const MsgObject& msg); // [3] 0x478a30

private:
	// The bound members (key: JP address; names ours). param is the
	// MsgObject's address; the packet is its m_param2.
	// key 0 (VV) 0x478720: the main dialog (when none is open), its stage
	// from the event items held (0x1be00013 -> 9, else 0x1a0002b8 -> 4,
	// else 0; 0x1be00014 -> 0xb), the document as its state, UpdateStage,
	// centred, ShowModal.
	void OpenDialog(void);
	// key 1 (VP) 0x4787c0: the document reads the state from the packet
	// (0x920cc0); with five missions, OpenDialog.
	void OnEventState(intptr_t param);
	// key 2 (VV) 0x477bf0: the open dialog to stage 3, UpdateStage.
	void OnTicketsDone(void);
	// key 3 (VV) 0x477c10: the open dialog to stage 5, UpdateStage.
	void OnStage5(void);
	// key 4 (VP) 0x477c30: a u32 reward id from the packet (0xa4ded0); the
	// open dialog's ShowRewardNotify, ShowText(10), ApplyReward(id - 0x15).
	void OnRewardTaken(intptr_t param);
	// key 5 (VV) 0x477c80: the open dialog's ShowFinalNotify,
	// ApplyFinalReward.
	void OnFinalRewardTaken(void);
	// key 6 (VP) 0x477780: empty.
	void OnIgnored(intptr_t param);

	uint32_t m_unknown24;    // JP +0x24: cleared by the ctor; no reader seen
};
// size 0x28 (x86)

} // namespace _gateway

#endif // PG_JP_JPARINEVENTHANDLER_H
