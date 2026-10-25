// The JP 983 lobby's 3D mascot actor: WRTTI "LobbyMascot" (0xd0a598),
// added by the JP CLobbyTask::Register (0xabfcc0) as "LobbyMascot". It shows
// a JP IDummyPet (kind 5, JpDummyPet.h). KR has no part of it.
// Declarations and the JP x86 layout, from the JP exe's bodies
// (node scripts/disasm.ts --jp 0x<va>).
#ifndef PG_JP_JPLOBBYMASCOT_H
#define PG_JP_JPLOBBYMASCOT_H

#include <functional>
#include <stdint.h>

#include "defines.h"
#include "padding.h"
#include "wmath.h"
#include "wtypes.h"
#include "actor.h"
#include "AnotherHand.h"
#include "JpLobbyCaddie.h"
#include "JpDummyPet.h"

// The mascot's build request (0x20 bytes; LobbyPlayer's and LobbyCaddie's
// carry two more ints).
struct JpLobbyMascotBuildRequest {
	std::function<bool()> m_load;  // 0x0 (VS 2010 std::tr1::function; its implementation pointer at +0x10)
	bool m_built;                  // 0x18
	bool m_requested;              // 0x19
	int m_mascotId;                // 0x1c, message 0x1aa's param1 (0: the equipped mascot)
};

// 0x5650e0: the object factory's creator (new 0x210).
IObject* LobbyMascotMakeInstance(void);

// Size 0x210 (LobbyMascotMakeInstance 0x5650e0), vtable 0xd0a55c (IActor's
// 15 slots), CHandOwner vtable 0xd0a554 at +0x18.
class LobbyMascot : public IActor, public CHandOwner {
	LAYOUT_CHECK_FRIEND;
	friend IObject* LobbyMascotMakeInstance(void);
public:
	static const WRTTI m_RTTI;  // 0xe751a4, name "LobbyMascot" (0xd0a598), base 0xf006cc (IActor)

	// 0x564fb0
	LobbyMascot();
	// IActor, in vtable order
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }  // [0] 0x564f30
	virtual ~LobbyMascot();                       // [1] 0x5650b0 (scalar deleting destructor; body 0x564e60)
	virtual void OnPreLoadInit(void);             // [2] 0x5648f0
	virtual void OnLoad(void);                    // [3] 0x563860
	// [4] 0x4912c0 OnInit, [5] 0x4750d0 OnReload: inherited
	virtual void OnProcess(float dt);             // [6] 0x564340
	virtual void OnDisplay(void);                 // [7] 0x563870
	virtual void OnDestroy(void);                 // [8] 0x5643e0
	// [9] 0x4750f0, [10] 0x475100: inherited

	// CHandOwner
	virtual void OnAnotherHand(int nWork, void* pData);  // [0] 0x563c10

protected:
	virtual void HandleMsg(const MsgObject& msg);  // [11] 0x565180
	// [12] 0xa46310, [13] 0x475110, [14] 0xa46320: inherited

private:
	// 0x563ef0: m_position = the scene's poly-soup point `name` ("mascot").
	void SetPositionFromScenePoint(const char* name);
	// 0x563f30: attaches the new mascot to the scene ("Default" motion,
	// position, RegisterElement and SetPuppet); false without a mascot or
	// its motion.
	bool AttachMascot();
	// 0x563fc0: drops the mascot (scene element, deleted), clears the id,
	// flags the build done and waits for the worker.
	void ResetMascot();
	// 0x564610: builds the owned mascot mascotId (0: the equipped one, KR
	// CSharedDoc::GetMyMascotInfo).
	bool Build(int mascotId);
	// 0x564820: the loader's body: Build, m_attachPending and m_mascotId.
	bool LoadMascot();
	// 0x564890: the request's loader (bound by OnPreLoadInit); false while
	// the request is already built.
	bool Load();

	IDummyPet* m_mascot;                // 0x1c, owned (ResetMascot deletes it; the destructor does not)
	// 0x20: a std::map (VS 2010: head 0x24 allocated by the ctor, size 0x28;
	// 12-byte values) destroyed by the destructor (0x564ce0); its use is
	// not traced.
	UNKNOWN(0x20, 0x2c);
	UNKNOWN(0x2c, 0x30);
	int m_mascotId;                     // 0x30, the built mascot's id; message 0x1aa compares it
	WVector m_position;                 // 0x34, WVector::ZERO; message 0x1aa sets the scene point "mascot" while still zero
	bool m_attachPending;               // 0x40, true from the ctor and OnDestroy, the build's result; OnProcess attaches while set
	UNKNOWN(0x41, 0x48);
	int m_unknown48;                    // 0x48, zeroed by the ctor
	UNKNOWN(0x4c, 0x50);
	JpLobbyMascotBuildRequest m_request; // 0x50
	CAnotherHand m_hand;                // 0x70 (ctor 0x98ad90, dtor 0x98add0)
	CRITICAL_SECTION m_lock;            // 0x1f8 (InitializeCriticalSection)
};

CHECK_SIZE(JpLobbyMascotBuildRequest, 0x20);
CHECK_SIZE(LobbyMascot, 0x210);
BEGIN_LAYOUT_CHECK(LobbyMascot)
	CHECK_FIELD(LobbyMascot, m_mascot, 0x1c);
	CHECK_FIELD(LobbyMascot, m_mascotId, 0x30);
	CHECK_FIELD(LobbyMascot, m_position, 0x34);
	CHECK_FIELD(LobbyMascot, m_attachPending, 0x40);
	CHECK_FIELD(LobbyMascot, m_request, 0x50);
	CHECK_FIELD(LobbyMascot, m_hand, 0x70);
	CHECK_FIELD(LobbyMascot, m_lock, 0x1f8);
END_LAYOUT_CHECK

#endif // PG_JP_JPLOBBYMASCOT_H
