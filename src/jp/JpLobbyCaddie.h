// The JP 983 lobby's 3D caddie actor: WRTTI "LobbyCaddie" (0xd0a45c),
// added by the JP CLobbyTask::Register (0xabfcc0) as "LobbyCaddie". The 3D
// pet object it shows (LobbyMascot too) is IDummyPet (JpDummyPet.h). KR has
// neither.
// Declarations and the JP x86 layout, from the JP exe's bodies
// (node scripts/disasm.ts --jp 0x<va>); members not written in
// JpLobbyCaddie.cpp are declared JP dependencies (not decompiled yet).
#ifndef PG_JP_JPLOBBYCADDIE_H
#define PG_JP_JPLOBBYCADDIE_H

#include <list>
#include <string>
#include <stdint.h>

#include "defines.h"
#include "padding.h"
#include "wmath.h"
#include "wtypes.h"
#include "actor.h"
#include "AnotherHand.h"
#include "RefreshSharedPtr.h"
#include "JpLobbyAvatar.h"
#include "JpLobbyPlayer.h"
#include "JpDummyPet.h"

struct sCaddieInfo;

// RFLobbyDesktop::s_instance->m_unknown245 (0x245, protected) = value: the
// caddie mouse handler 0x5604b0's result (LobbyCaddie::OnProcess).
// Declared only: needs an RFLobbyDesktop friend (REQUEST).
void Jp_LobbyDesktop_SetCaddieCameraZoom(bool value);
// RFLobbyDesktop::s_instance->m_unknown244 (0x244, protected), the shop
// camera zoom flag LobbyPlayer sets; the caddie mouse handler resets the
// cursor while it is clear. Declared only: needs an RFLobbyDesktop friend
// (REQUEST).
bool Jp_LobbyDesktop_GetShopCameraZoom();
// 0x63f390 (thiscall on the shared doc, unplaced): the owned caddie `id`
// (-1: the equipped one, KR m_myCaddieId at JP +0xa26) in the caddie map,
// when its type id is a caddie's (0x1c000000 group); NULL otherwise.
// Defined in JpLobbyCaddie.cpp on the KR m_caddieMap.
sCaddieInfo* Jp_SharedDoc_FindMyCaddie(unsigned long id);

// 0x561600: the object factory's creator (new 0x218).
IObject* LobbyCaddieMakeInstance(void);

// Size 0x218 (LobbyCaddieMakeInstance 0x561600), vtable 0xd0a40c (IActor's
// 15 slots), CHandOwner vtable 0xd0a404 at +0x18. One build request and one
// worker (LobbyPlayer has two).
class LobbyCaddie : public IActor, public CHandOwner {
	LAYOUT_CHECK_FRIEND;
	friend IObject* LobbyCaddieMakeInstance(void);
public:
	static const WRTTI m_RTTI;  // 0xe7518c, name "LobbyCaddie" (0xd0a45c), base 0xf006cc (IActor)

	// 0x561180
	LobbyCaddie();
	// IActor, in vtable order
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }  // [0] 0x560d60
	virtual ~LobbyCaddie();                       // [1] 0x560ff0 (scalar deleting destructor; body 0x560c90)
	virtual void OnPreLoadInit(void);             // [2] 0x5617b0
	virtual void OnLoad(void);                    // [3] 0x55fee0
	// [4] 0x4912c0 OnInit, [5] 0x4750d0 OnReload: inherited
	virtual void OnProcess(float dt);             // [6] 0x560d70
	virtual void OnDisplay(void);                 // [7] 0x560290
	virtual void OnDestroy(void);                 // [8] 0x5616a0
	// [9] 0x4750f0, [10] 0x475100: inherited

	// CHandOwner
	virtual void OnAnotherHand(int nWork, void* pData);  // [0] 0x560080

protected:
	virtual void HandleMsg(const MsgObject& msg);  // [11] 0x561820
	// [12] 0xa46310, [13] 0x475110, [14] 0xa46320: inherited

private:
	// 0x5600b0: records the build request (the caddie's type id for a
	// preview, else the owned caddie's id) unless the worker is busy.
	void RequestBuild(uint32_t typeId, int caddieId);
	// 0x5602a0: m_position = the scene's poly-soup point `name` ("Caddie").
	void SetPositionFromScenePoint(const char* name);
	// 0x560330: turns the caddie by `angle` (RotMat about axis 1, before its
	// matrix).
	void RotateCaddie(float angle);
	// 0x5603c0: the caddie's alpha eases between 0 and 255 (m_fadeOut picks
	// the direction) over the 0xd0a3ac duration; run while m_fadeIn.
	bool ProcessFade(float dt);
	// 0x5604b0: the mouse rotation of the caddie (like LobbyPlayer's
	// 0x566cd0); its result goes to the lobby desktop's +0x245.
	bool ProcessMouse(float dt);
	// 0x560a60: attaches the new caddie to the scene ("기다리기" motion,
	// "Talk_Room.seq" out of the shop states, position, turn of 0.6,
	// RegisterElement and SetPuppet); false without a caddie or its motion.
	bool AttachCaddie();
	// 0x561240: builds the caddie: the owned caddie caddieId (-1 / 0 with
	// no type id: the equipped one), else the caddie type id typeId (a
	// caddie item's type id gives its caddie).
	bool Build(int caddieId, uint32_t typeId);
	// 0x561490: drops the caddie (scene element, m_caddie), clears the ids,
	// flags the build done and waits for the worker. `unused` is not read.
	void ResetCaddie(int unused);
	// 0x5616d0: the loader's body: message 0x53 to "Lobby", Build, message
	// 0x54.
	void LoadCaddie();
	// 0x561790: the request's loader (bound by OnPreLoadInit); false while
	// the request is already built.
	bool Load();

	RF_SHARED_PTR<IDummyPet> m_caddie;          // 0x1c (ctor 0x560ed0, assign 0x561040, dtor 0x560b40)
	WVector m_position;                          // 0x24, WVector::ZERO; messages 0x1a5 set the scene point "Caddie" while still zero
	bool m_attachPending;                        // 0x30, true from the ctor, the build and OnDestroy; OnProcess attaches while set
	UNKNOWN(0x31, 0x34);
	uint32_t m_caddieTid;                        // 0x34, the built caddie's type id
	int m_caddieId;                              // 0x38, the built caddie's id (0 for a preview); message 0x1a7 compares it
	bool m_fadeOut;                              // 0x3c, the attach sets it
	bool m_fadeIn;                               // 0x3d, the attach clears it; OnProcess runs ProcessFade while set
	UNKNOWN(0x3e, 0x40);
	float m_fadeTime;                            // 0x40, cleared by the attach
	bool m_paused;                               // 0x44, message 0x2ae sets, 0x2af clears
	bool m_dragging;                             // 0x45, ProcessMouse: the button went down on the caddie
	UNKNOWN(0x46, 0x48);
	int m_unknown48;                             // 0x48, zeroed by the ctor
	UNKNOWN(0x4c, 0x50);
	JpLobbyPlayerBuildRequest m_request;         // 0x50 (m_arg 0x6c: the caddie id, m_characterId 0x70: the type id)
	CAnotherHand m_hand;                         // 0x78 (ctor 0x98ad90, dtor 0x98add0)
	CRITICAL_SECTION m_lock;                     // 0x200 (InitializeCriticalSection)
};

CHECK_SIZE(LobbyCaddie, 0x218);
BEGIN_LAYOUT_CHECK(LobbyCaddie)
	CHECK_FIELD(LobbyCaddie, m_caddie, 0x1c);
	CHECK_FIELD(LobbyCaddie, m_position, 0x24);
	CHECK_FIELD(LobbyCaddie, m_caddieTid, 0x34);
	CHECK_FIELD(LobbyCaddie, m_fadeTime, 0x40);
	CHECK_FIELD(LobbyCaddie, m_paused, 0x44);
	CHECK_FIELD(LobbyCaddie, m_request, 0x50);
	CHECK_FIELD(LobbyCaddie, m_hand, 0x78);
	CHECK_FIELD(LobbyCaddie, m_lock, 0x200);
END_LAYOUT_CHECK

#endif // PG_JP_JPLOBBYCADDIE_H
