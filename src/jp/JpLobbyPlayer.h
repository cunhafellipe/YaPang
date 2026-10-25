// The JP 983 lobby's 3D character actor: WRTTI "LobbyPlayer" (0xd0a680),
// added by the JP CLobbyTask::Register (0xabfcc0) as "LobbyAvatar"
// (docs/JP_CLIENT.md, "LobbyPlayer"). KR has no part of it.
// Declarations and the JP x86 layout, from the JP exe's bodies
// (node scripts/disasm.ts --jp 0x<va>); the members not written in
// JpLobbyPlayer.cpp are declared JP dependencies (not decompiled yet).
#ifndef PG_JP_JPLOBBYPLAYER_H
#define PG_JP_JPLOBBYPLAYER_H

#include <functional>
#include <map>
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

class CPartTidList;
class CPetFrame;
class CSharedDoc;
struct sUccClothes;

// JP CSharedDoc +0x8cb (a dword): the first %d of the avatar's scene name
// "LOBBY_PLAYER_%d_%d" (the builders 0x567a20 / 0x567e10): the KR MyUID()
// (JP +0x8cb is MyUID() in the other ports of JP code). Defined in
// JpLobbyPlayer.cpp.
int Jp_GameState_AvatarNameKey();
// RFLobbyDesktop::s_instance->m_unknown244 (0x244, protected): OnProcess
// stores the mouse handler 0x566cd0's result there. Defined in
// JpLobbyPlayer.cpp (an RFLobbyDesktop friend).
void Jp_LobbyDesktop_SetShopCameraZoom(bool value);
// RFLobbyDesktop::s_instance->m_avatarPending (0x258, protected) = value:
// set by the aux part query 0x5668a0. Defined in JpLobbyPlayer.cpp (an
// RFLobbyDesktop friend).
void Jp_LobbyDesktop_SetAvatarPending(bool value);
// 0x565730 (the result in ecx, the position in edx): the avatar's screen
// rect: on the Refresh root window (RFSceneManager 0xe73ddc, its manager's
// m_root, RFWnd::GetSize; 800 x 600 without one), the first and last pixel
// columns the hit test 0x565620 accepts along the middle row, then the rows
// along the middle column; the zero rect (0xf145f8) when either span is
// empty. Defined in JpLobbyPlayer.cpp.
WRect Jp_LobbyPlayerPointFromPosition(const WVector& position);
// 0xb55a00, thiscall on g_resrcmng (0xf023f0): 0xbc6df0(threadId) on its
// four 0x20-byte locks from +0x164 (each left when the thread owns it): the
// KR WResourceManager::UnlockAllByThread. Defined in JpLobbyPlayer.cpp.
void Jp_ReleaseThreadResources(unsigned long threadId);
// 0x4db4b0 (unplaced): from + (to - from) * t^4, t = time / duration (the
// avatar's fade). Defined in JpLobbyPlayer.cpp.
float Jp_EaseInQuartic(float from, float to, float time, float duration);
// The JP CSharedDoc +0x10c24 motion table (hatManager unit; JP 0x9a3c80 /
// 0x9a3c00, defined in JpAvatarMotionDlg.cpp): the number of motions of a
// character type, and the motion key of one of them.
int JpMotionTable_GetCount(const int& charTypeId);
const char* JpMotionTable_GetMotion(const int& charTypeId, int& index);
// The motion part rules of the JP object at 0xe73e14 (its tables are in
// JpAvatarMotionDlg.cpp). JpPartMotions_Contains (JP 0x4d0cf0, defined in
// JpAvatarMotionDlg.cpp): the slot (0-4) whose motion key is `motion`.
bool JpPartMotions_Contains(const char* motion, unsigned int& slot);
// JP 0x4d0bd0 (same object, unplaced; defined in JpAvatarMotionDlg.cpp
// next to its table): the part typeid of the first
// row of the character type (tid & 0x3ffffff) that gives `slot` and whose
// part is one of the list's 24 m_partTids (the JP passes &parts->m_partTids,
// +0x8); 0 when none.
uint32_t Jp_MotionPartRule_Check(uint32_t characterTid, const CPartTidList* parts, unsigned int slot);
// 0x4cef20
const char* Jp_CharacterBoneName(uint32_t typeIndex);
// 0x8b28b0 (JP CPetFrame, unplaced): the JP SetPart(parts, tid, id,
// keepTid): one argument more than the KR CPetFrame::SetPart(CPartTidList*,
// unsigned long, unsigned long); it ends with the JP SetHairClr(NULL,
// parts->m_hairClr, keepTid ? tid : 0, true). Defined in JpLobbyPlayer.cpp
// on the KR SetPart.
unsigned long Jp_PetFrame_SetPart(CPetFrame* frame, CPartTidList* parts, unsigned long tid, unsigned long id, bool keepTid);
// The mouse rotation 0x566cd0's Refresh side (RFSceneManager, instance
// 0xe73ddc):
// RefreshInputHandler::PollMouse (0x5f3b90) on RFSceneManager's m_input
// (+0x34): the mouse button flags (1 pressed, 2 released, 4 double click,
// 0x100 dragging); 0 without the scene manager or the game's
// CInputManager. Defined in JpLobbyPlayer.cpp.
int Jp_RefreshScene_GetMouseButtons();
// RFSceneManager's m_input (+0x34) is set (inline in 0x566cd0). Defined
// in JpLobbyPlayer.cpp.
bool Jp_RefreshScene_HasInput();
// Whether a Refresh window other than the desktop is under the screen point
// (JP 0x604320, defined in RefreshInputHandler.cpp).
bool Jp_RefreshScene_IsOverWindow(float x, float y);
// RFSceneManager::SetMouseCursor (0x603710) on the instance. Defined in
// JpLobbyPlayer.cpp.
void Jp_RefreshScene_SetMouseCursor(int cursor, bool lock);
// RFLobbyDesktop::s_instance->m_unknown245 (0x245, protected): the caddie
// camera zoom flag.
bool Jp_LobbyDesktop_GetCaddieCameraZoom();
// 0x5659b0
bool Jp_LobbyPlayer_IsMouseFree(const WVector& mouse);
// 0x565620
bool Jp_LobbyPlayer_HitTest(const WVector& mouse, const WVector& position);
// Jp_PetFrame_SetUccClothes (JP CPetFrame 0x8b0960): JpPetFrameGlue.h.

// The two std::map members at +0x2c and +0x3c (VS 2010: node 0x1c, head at
// +0x4, size at +0x8). The value is 12 bytes: a 4-byte key and a shared_ptr
// (the node destroyer 0x568820 releases the count at the value's +0x8); the
// key's and the pointee's types are not known yet (nothing inserts in the
// code read so far).
struct JpLobbyPlayerTable {
	// 0x568cc0 (thiscall on the map): erases every node (message 0x189).
	void Clear();

	std::map<uint32_t, RF_SHARED_PTR<void> > m_map;  // 0x0
};

// One of the two avatar build requests (0x28 bytes; the array's element
// constructor 0x567530 and destructor 0x567540 are the implicit ones: the
// std::tr1::function constructor / _Tidy): slot 0 is the local player (OnPreLoadInit binds the
// loader 0x568860), slot 1 another player / the shop preview (0x5688e0).
// RequestBuild 0x566000 records a request; OnAnotherHand 0x565f10 runs the
// loader on the worker thread and sets m_built.
struct JpLobbyPlayerBuildRequest {
	std::function<bool()> m_load;  // 0x0 (VS 2010 std::tr1::function; its implementation pointer at +0x10)
	bool m_built;                  // 0x18
	bool m_requested;              // 0x19
	int m_arg;                     // 0x1c, message 0x185's param2 / 0x187's param1
	int m_characterId;             // 0x20
	UNKNOWN(0x24, 0x28);
};

// 0x569310: the object factory's creator (new 0x438).
IObject* LobbyPlayerMakeInstance(void);

// Size 0x438 (LobbyPlayerMakeInstance 0x569310), vtable 0xd0a644 (IActor's 15 slots),
// CHandOwner vtable 0xd0a63c at +0x18.
class LobbyPlayer : public IActor, public CHandOwner {
	LAYOUT_CHECK_FRIEND;
	friend IObject* LobbyPlayerMakeInstance(void);
public:
	static const WRTTI m_RTTI;  // 0xe751c0, name "LobbyPlayer" (0xd0a680)

	// 0x5690c0
	LobbyPlayer();
	// IActor, in vtable order
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }  // [0] 0x569040
	virtual ~LobbyPlayer();                       // [1] 0x5692e0 (scalar deleting destructor)
	virtual void OnPreLoadInit(void);             // [2] 0x568c10
	virtual void OnLoad(void);                    // [3] 0x565b40
	// [4] 0x4912c0 OnInit, [5] 0x4750d0 OnReload: inherited
	virtual void OnProcess(float dt);             // [6] 0x568600
	virtual void OnDisplay(void);                 // [7] 0x566330
	virtual void OnDestroy(void);                 // [8] 0x568dc0
	// [9] 0x4750f0, [10] 0x475100: inherited

	// CHandOwner
	virtual void OnAnotherHand(int nWork, void* pData);  // [0] 0x565f10

protected:
	virtual void HandleMsg(const MsgObject& msg);  // [11] 0x5693c0
	// [12] 0xa46310, [13] 0x475110, [14] 0xa46320: inherited

private:
	// 0x565b50
	WRect GetPoint() const;
	// 0x566000
	void RequestBuild(int slot, int arg, int characterId);
	// 0x5665c0: runs while m_fadeIn (0x3d5) is set: the avatar's alpha
	// eases between 0 and 255 over 0.7 s; at the end m_fadeOut flips and
	// m_fadeIn clears (one word store at 0x3d4).
	bool ProcessFade(float dt);
	// 0x566390: attaches the new avatar to the scene ("기본자세" pose,
	// position, alpha, "DefaultCamera" to LobbyBg, RegisterElement and
	// SetPuppet); false without an avatar or its pose.
	bool AttachAvatar();
	// 0x566c40: turns the avatar by `angle` (RotMat about axis 1, before
	// its matrix).
	void RotateAvatar(float angle);
	// 0x566cd0: the mouse rotation of the avatar (CInputManager mouse delta
	// -> 0x566c40) while the button drags it, the cursor; its result goes to
	// the lobby desktop's +0x244.
	bool ProcessMouse(float dt);
	// 0x567050: tells "Lobby" (message 0x195) whether the avatar's motion
	// is over (not playing, or its name ends with "끝").
	bool NotifyMotionEnd();
	// 0x567140: with m_motionFlag, once the motion ended ("끝"), a random
	// idle motion of the character (when its part rule allows it).
	bool PlayIdleMotion();
	// 0x567280: whether a new avatar waits in m_newAvatar (under m_lock).
	bool HasNewAvatar();
	// 0x567a20: builds the local player's avatar: the owned character
	// characterId (-1: the equipped one), else the character type id tid
	// from the my-parts lists; the random dress-up when offline.
	bool BuildSelf(int characterId, uint32_t tid);
	// 0x567e10: builds another character (the preview) from its type id.
	bool BuildOther(uint32_t tid);
	// 0x568480: mode 0 BuildSelf(a, b), mode 1 BuildOther(b).
	bool Build(int mode, int a, uint32_t b);
	// 0x5684b0: takes the new avatar (m_newAvatar -> m_avatar, the built
	// ids and flags -> the attached ones) when the worker made one.
	bool TakeNewAvatar();
	// 0x568730: a loader's body: message 0x53 to "Lobby", Build(slot, ...),
	// the request's reset, message 0x54.
	void LoadSlot(int slot);
	// 0x568860: slot 0's loader (local player), then message 0x57 to "Lobby"
	// (JpLobbyLoading.h); false while slot 0 is already built.
	bool LoadSelf();
	// 0x5688e0: slot 1's loader (another player / the preview).
	bool LoadOther();
	// 0x566350: m_position = the scene's poly-soup point `name`
	// (WPolySoup::FindPoint on CSharedDoc +0x11024).
	void SetPositionFromScenePoint(const char* name);
	// 0x5666d0 (message 0x18a): the JP SetPart(parts, tid, id, keepTid) on
	// the avatar's frame, then the part refresh, the hair colour and the
	// character's bone made visible; the result of SetPart.
	unsigned long SetAvatarPart(CPartTidList* parts, unsigned long tid, unsigned long id, bool keepTid);
	// 0x5668a0 (message 0x18b): puts the aux part tid (0: none) in the
	// avatar's aux slot index (the TID bits 21-25); false for an unknown
	// aux part or a bad slot.
	bool SetAvatarAuxPart(unsigned char index, int tid);
	// 0x5669f0 (message 0x18c): with an avatar, param3 != 0
	// must be a hair style (CItemManager::FindHairStyle, JP +0x20c); then
	// message 0x53 to "Lobby", the JP CPetFrame::SetHairClr 0x8af6b0
	// (param1, param2, 0, true: two more arguments than the KR
	// SetHairClr(CPartTidList*, unsigned char)), on success the avatar's
	// RefreshAfterPartChange(0, 0, 0, true) and CPetFrame::Refresh(param1)
	// 0x8b1030, message 0x54, the desktop's m_avatarPending = 1.
	bool SetAvatarHairColor(CPartTidList* parts, unsigned char color, unsigned long hairStyleTid);
	// 0x566b30 (message 0x18d): dresses the avatar in the UCC clothes of
	// serial `serial` (CUccManager::FindClothes); `unused` is not read.
	bool SetAvatarUccClothes(unsigned long serial, int unused);
	// 0x5680b0: drops the avatar (scene element, m_avatar), clears both ids
	// and m_rebuild[0], flags the slot-0 build done and waits for the slot-0
	// worker.
	void ResetAvatar();
	// 0x568230: drops the avatar, restores the my-parts list of
	// m_avatarTid (CSharedDoc::BuildMyPartTidListDefault), clears
	// both ids and resets the slot-1 worker.
	void ClearPreview();

	// 0x1c: the same shared_ptr type as m_avatar (both set up by 0x4d7490,
	// count vtable 0xd04b60): where the builders put the new avatar
	// (docs/JP_CLIENT.md: "the list goes into the container at this+0x1C").
	RF_SHARED_PTR<IDummyPlayer> m_newAvatar;  // 0x1c
	RF_SHARED_PTR<IDummyPlayer> m_avatar;     // 0x24, the attached avatar
	JpLobbyPlayerTable m_table2c;               // 0x2c
	UNKNOWN(0x38, 0x3c);
	JpLobbyPlayerTable m_table3c;               // 0x3c, cleared by message 0x189
	UNKNOWN(0x48, 0x4c);
	int m_builtCharacterUid;                   // 0x4c, the built character's uid (the record's +0x4; 0 from the part lists)
	uint32_t m_builtCharacterTid;              // 0x50, the built character's type id (| 0x4000000 on the random path)
	int m_characterId;                         // 0x54, the attached avatar's character uid (0x5684b0: from m_builtCharacterUid)
	int m_avatarTid;                           // 0x58, the attached avatar's type id (0x5684b0: from m_builtCharacterTid; message 0x190 reads it)
	WVector m_position;                        // 0x5c, WVector::ZERO (0xf04154); message 0x185 sets the scene point "시작점" while still zero
	bool m_avatarBuilt[2];                     // 0x68 local player, 0x69 another player (docs/JP_CLIENT.md; loop at the end of the ctor)
	bool m_rebuild[2];                         // 0x6a, OnProcess re-attaches while set
	UNKNOWN(0x6c, 0x70);
	JpLobbyPlayerBuildRequest m_requests[2];   // 0x70
	CAnotherHand m_hands[2];                   // 0xc0 (ctor 0x98ad90, dtor 0x98add0)
	float m_fadeTime;                          // 0x3d0, cleared by the attach
	bool m_fadeOut;                            // 0x3d4, the attach: !m_unknown40d
	bool m_fadeIn;                             // 0x3d5, the attach: m_unknown40d; OnProcess runs ProcessFade while set
	int m_loadingSlot;                         // 0x3d8, the slot LoadSlot builds
	WMatrix m_matrix;                          // 0x3dc, from 0xf04160 (the avatar's matrix)
	bool m_unknown40c;                         // 0x40c, true
	bool m_unknown40d;                         // 0x40d, true; 0x185: param3 == 0, 0x187: param2 == 0 (0x85 sends 0x185 with param3 = its param1 != 0)
	int m_motionParam;                         // 0x410, message 0x199's param2
	bool m_motionFlag;                         // 0x414, message 0x199's param3 != 0
	bool m_paused;                             // 0x415, message 0x2ae sets, 0x2af clears (its reader not traced yet)
	bool m_dragging;                           // 0x416, ProcessMouse: the button went down on the avatar
	// 0x418: vtable 0xd0a5ec + the CRITICAL_SECTION (InitializeCriticalSectionAndSpinCount 2000)
	UNKNOWN(0x418, 0x41c);
	CRITICAL_SECTION m_lock;                   // 0x41c
	UNKNOWN(0x434, 0x438);
};

CHECK_SIZE(JpLobbyPlayerBuildRequest, 0x28);
CHECK_SIZE(JpLobbyPlayerTable, 0xc);
CHECK_SIZE(LobbyPlayer, 0x438);
BEGIN_LAYOUT_CHECK(LobbyPlayer)
	CHECK_FIELD(LobbyPlayer, m_newAvatar, 0x1c);
	CHECK_FIELD(LobbyPlayer, m_avatar, 0x24);
	CHECK_FIELD(LobbyPlayer, m_table3c, 0x3c);
	CHECK_FIELD(LobbyPlayer, m_characterId, 0x54);
	CHECK_FIELD(LobbyPlayer, m_avatarTid, 0x58);
	CHECK_FIELD(LobbyPlayer, m_position, 0x5c);
	CHECK_FIELD(LobbyPlayer, m_avatarBuilt, 0x68);
	CHECK_FIELD(LobbyPlayer, m_requests, 0x70);
	CHECK_FIELD(LobbyPlayer, m_hands, 0xc0);
	CHECK_FIELD(LobbyPlayer, m_matrix, 0x3dc);
	CHECK_FIELD(LobbyPlayer, m_unknown40d, 0x40d);
	CHECK_FIELD(LobbyPlayer, m_paused, 0x415);
	CHECK_FIELD(LobbyPlayer, m_lock, 0x41c);
END_LAYOUT_CHECK

#endif // PG_JP_JPLOBBYPLAYER_H
