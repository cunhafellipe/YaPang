// JP-only game code (no KR counterpart): the two room list forms of the JP
// lobby, both derived from FrRoomListDlg (JpRoomListDlg.h). They are opened
// by one case (0xabc986) of the JP lobby's message switch 0xabc060 (10617
// bytes, a switch on the message id: CLobbyMain's HandleMsg, which
// jp/pairing.tsv pairs by string with KR OnRoomList_MakeRoomUp). The case
// calls the lobby's vtable +0x3c, returns when the shared doc's room tab
// (0x62bf50) already equals the message's +0xc and a room list form
// (CLobbyMain +0x77c) is open, else closes that form (vtable +0x68 (1, 1))
// and the form at +0x780 when 0x62bf90 == 8, stores the message's +0xc as
// the room tab (shared doc 0x62bf40), then switches on the room tab:
//   0: FrMultiplayRoomDlg, layout "roomlist" (factory 0xab4990), then
//      0x62bf80(1);
//   1: plays "bgm_grandprix_lobby.mp3" (CLobbyMain 0x653f60), creates
//      FrGrandPrixRoomDlg, layout "grandprix_list" (factory 0xab4a60),
//      calls FrRoomListDlg 0x51ce50(+0x794, +0x784, +0x79c of the lobby)
//      on it, then 0x62bf80(2).
// Both are created with the manager (*0xf022d0)->+0x40, the owner
// &lobby->+0x18 and no parent; the form goes to CLobbyMain +0x77c and is
// moved by its MoveWindow (vtable +0x8c).
//
// jp/pairing.tsv pairs the factories wrongly: 0xab4990 with KR
// CreateForm<FrDtRoomDlg> and 0xab4a60 with KR CreateForm<FrPointEventDlg>;
// they build FrMultiplayRoomDlg (new 0x300) and FrGrandPrixRoomDlg
// (new 0x310).
//
// Both message maps chain to FrRoomListDlg's (0xe09fdc); handlers are
// stored by static initializers with this adjustment -4 (FrCmdTarget base).
#ifndef PG_JP_JPMAKEROOMDLG_H
#define PG_JP_JPMAKEROOMDLG_H

#include "defines.h"
#include "padding.h"
#include "frcmdtarget.h"
#include "frform.h"
#include "JpRoomListDlg.h"
#include "singleton.h"

class Bitmap;
class FrArea;
class FrGraphicInterface;
class FrWnd;
class FrWndManager;
class WTitleFontEx;

// CGrandPrixClock (JP-only, name from the RTTI 0xe0bf6c
// ".?AV?$WSingleton@VCGrandPrixClock@@@@" of the vtable 0xd0a1b0): the GP
// lobby's clock, a WSingleton (WSingleton<CGrandPrixClock> ctor 0x55d1b0,
// BaseObject ctor 0xb67ba0; m_pInstance 0xe74e1c, Instance 0x50f250,
// IsInstantiated 0x50f260). Declared only as far as FrGrandPrixRoomDlg uses
// it; size not traced.
class CGrandPrixClock : public WSingleton<CGrandPrixClock> {
	LAYOUT_CHECK_FRIEND;
public:
	UNKNOWN(0x28, 0x3c);
	// The two numbers Draw prints with two digits (left at m_x, right 82
	// further).
	unsigned short m_leftNumber;               // 0x3c
	UNKNOWN(0x3e, 0x44);
	// Picks the image Draw puts 59 right of m_x: m_pSetImage when set,
	// else m_pUnsetImage.
	int m_flag44;                              // 0x44
	UNKNOWN(0x48, 0x50);
	unsigned short m_rightNumber;              // 0x50
	UNKNOWN(0x52, 0x58);
	// m_pBlinkImage is drawn while it is even.
	unsigned short m_blinkCounter;             // 0x58
	UNKNOWN(0x5a, 0x64);
	// Set by SetupGpRoomList, cleared by StopLobbyAmbience; Draw draws
	// nothing while 0.
	int m_active;                              // 0x64
	// Placed by OnGpStateInit (the gpstate area's position plus two
	// constants), read by OnGpStateOwnerDraw.
	float m_x;                                 // 0x68
	float m_y;                                 // 0x6c
	// Prints the two numbers (none while NULL).
	WTitleFontEx* m_pFont;                     // 0x70
	const Bitmap* m_pUnsetImage;               // 0x74
	const Bitmap* m_pSetImage;                 // 0x78
	const Bitmap* m_pBlinkImage;               // 0x7c

	// Draws the clock (called by OnGpStateOwnerDraw with the GDI; name
	// ours).
	void Draw(FrGraphicInterface* pGDI);                              // 0x55d2e0
};
BEGIN_LAYOUT_CHECK(CGrandPrixClock)
	CHECK_FIELD(CGrandPrixClock, m_leftNumber, 0x3c);
	CHECK_FIELD(CGrandPrixClock, m_flag44, 0x44);
	CHECK_FIELD(CGrandPrixClock, m_rightNumber, 0x50);
	CHECK_FIELD(CGrandPrixClock, m_blinkCounter, 0x58);
	CHECK_FIELD(CGrandPrixClock, m_active, 0x64);
	CHECK_FIELD(CGrandPrixClock, m_x, 0x68);
	CHECK_FIELD(CGrandPrixClock, m_y, 0x6c);
	CHECK_FIELD(CGrandPrixClock, m_pFont, 0x70);
	CHECK_FIELD(CGrandPrixClock, m_pBlinkImage, 0x7c);
END_LAYOUT_CHECK

// 0x55cd50 (fastcall, name ours): writes value as two digits into
// out[0..1] ("%d" with a leading '0' below 10; out[2] is not written). Also
// called by 0x55b1f0 and 0x55d030.
void JpFormatTwoDigits(unsigned short value, char* out);

// 0x4ce760 (thiscall on the JP object at 0xe73e14, which it does not read;
// declared only, name ours): true for the part type ids a no-manner player
// may not wear in a room (the KR OnRoomList_Init switch lists ten of them;
// the JP list differs, 0x8314800 and 0x839c800 among them).
bool Jp_IsNoMannerRestrictedPart(uint32_t typeId);

// FrMultiplayRoomDlg: RTTI 0xe74e4c "FrMultiplayRoomDlg" (static init
// 0xccca20, base FrRoomListDlg 0xe74f58), registered by name at static init
// 0xccca40 (make instance 0x517660). Vtables 0xd07524 (38 slots, overrides
// [0] GetRTTI, [1] deleting destructor, [9] OnProc, [32] OnInit),
// 0xd07514 (FrCmdTarget: [0] thunk 0x5171c0, [1] GetMessageMap 0x517140),
// 0xd0750c ([0] thunk 0x5171d0). Message map 0xe09a38 (5 rows at 0xe09a40,
// handlers stored by 0xccca60). Size 0x300: no members of its own.
class FrMultiplayRoomDlg : public FrRoomListDlg {
	LAYOUT_CHECK_FRIEND;
public:
	static const WRTTI m_RTTI;                                        // 0xe74e4c
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x517170

	FrMultiplayRoomDlg(void);                                         // 0x5175f0
	virtual ~FrMultiplayRoomDlg(void);                                // [1] 0x5171e0 (deleting), 0x517150

	// FrRoomListDlg::OnProc, then (with m_pRoomList and no m_value14c)
	// two key tests (CInputManager::GetDown 0x9a6ce0 on g_input 0xf00510,
	// key names 0xd075c0 / 0xd075bc) that scroll the room list's scroll
	// bar (JP FrWnd +0x90, ScrollUp 0xbf6b50(1) / ScrollDown 0xbf6bb0(1)),
	// then FrRoomListDlg::UpdateRoomList 0x520520(dt).
	virtual void OnProc(const float dt);                              // [9] 0x517210
	// FrRoomListDlg::OnInit; then the room state (+0x140) gets kind 0,
	// shared doc 0x62bf40(0), room state 0x5a01c0(this), RefreshRoomList,
	// FrRoomListDlg 0x521470(0), m_pMtRoomTab->SetStatus(2).
	virtual bool OnInit(void);                                        // [32] 0x517360

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe09a40

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe09a38
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // FrCmdTarget [1] 0x517140

	// "roomlist" handlers. The three mouse handlers forward to the room
	// state object at +0x140 (0x5a0280, 0x5a02b0, 0x5a02e0, with this).
	void OnRoomListInit(intptr_t param);                              // 0x5172a0
	void OnRoomListOwnerDraw(intptr_t param);                         // 0x517330
	void OnRoomListLBtnDown(void);                                    // 0x517190
	void OnRoomListRBtnUp(void);                                      // 0x5171a0
	void OnRoomListDblClick(void);                                    // 0x5171b0

	// Tail call of FrRoomListDlg 0x520900 (name ours).
	void RefreshRoomList(void);                                       // 0x517180
	// Run by the constructor (name ours): sets two shared-doc bytes
	// (+0x1094c, +0x109b8) to 0x17, FrRoomListDlg 0x51d6b0, releases and
	// clears the shared doc's tree at +0x3890, resets the 0x58 option
	// (0xa7a8d0), computes a percentage from +0x960 / +0x98c and clears
	// matching slots (0x4ce760), m_value2ec = 6, m_flag2e9 = false.
	void ResetRoomState(void);                                        // 0x5173c0
};
CHECK_SIZE(FrMultiplayRoomDlg, 0x300);

// FrGrandPrixRoomDlg: RTTI 0xe74e20 "FrGrandPrixRoomDlg" (static init
// 0xccc100, base FrRoomListDlg 0xe74f58), registered by name at static init
// 0xccc120 (make instance 0x510000). Vtables 0xd06a9c (38 slots, overrides
// [0] GetRTTI, [1] deleting destructor, [9] OnProc, [32] OnInit),
// 0xd06a90 (FrCmdTarget: [0] thunk 0x50f920, [1] GetMessageMap 0x50f1e0),
// 0xd06a88 ([0] thunk 0x50f930). Message map 0xe09074 (17 rows at
// 0xe09080, handlers stored by 0xccc140). Size 0x310.
class FrGrandPrixRoomDlg : public FrRoomListDlg {
	LAYOUT_CHECK_FRIEND;
public:
	static const WRTTI m_RTTI;                                        // 0xe74e20
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x50f910

	FrGrandPrixRoomDlg(void);                                         // 0x50ff90
	virtual ~FrGrandPrixRoomDlg(void);                                // [1] 0x50faf0 (deleting), 0x50f8a0

	// FrRoomListDlg::OnProc, then FrRoomListDlg 0x520520(dt).
	virtual void OnProc(const float dt);                              // [9] 0x50f1f0
	// FrRoomListDlg::OnInit; then the room state (+0x140) gets kind 1,
	// shared doc 0x62bf40(1), room state 0x5a01c0(this), 0x50fb20,
	// m_pGpRoomTab->SetStatus(2), and m_pUserList is moved to its rect
	// with its height lowered by the double 80.0 at 0xd06b38 (SetRect
	// 0xbe86e0; the JP FrWnd rect is at +0x34).
	virtual bool OnInit(void);                                        // [32] 0x50fc80

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe09080

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe09074
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // FrCmdTarget [1] 0x50f1e0

	// Message-map handlers, in row order. The three "roomlist" mouse
	// handlers forward to the room state object at +0x140 (0x5a0280,
	// 0x5a02b0, 0x5a02e0, with this).
	void OnRoomListInit(intptr_t param);                              // 0x50f2a0
	void OnRoomListOwnerDraw(intptr_t param);                         // 0x50f330
	void OnRoomListLBtnDown(void);                                    // 0x50f220
	void OnRoomListRBtnUp(void);                                      // 0x50f230
	void OnRoomListDblClick(void);                                    // 0x50f240
	// The five sort buttons are FrRoomListDlg's FrButton* [5] at +0x2a8
	// (event, rookie, c1, c2, c3; cast to RTTI 0xf1ab38 FrButton). Each
	// LBtnUp (sort 0x63 for event, 0..3 for the others) calls 0x50f940(sort)
	// (all five SetStatus(0) 0xbf4fb0, the matching one SetStatus(2), then
	// the shared doc 0x62bf20(sort)) and the room state 0x5a0310(sort, this).
	void OnSortEventInit(intptr_t param);                             // 0x50f360
	void OnSortEventLBtnUp(void);                                     // 0x50fa40
	void OnSortRookieInit(intptr_t param);                            // 0x50f3d0
	void OnSortRookieLBtnUp(void);                                    // 0x50fa60
	void OnSortC1Init(intptr_t param);                                // 0x50f440
	void OnSortC1LBtnUp(void);                                        // 0x50fa80
	void OnSortC2Init(intptr_t param);                                // 0x50f4b0
	void OnSortC2LBtnUp(void);                                        // 0x50faa0
	void OnSortC3Init(intptr_t param);                                // 0x50f520
	void OnSortC3LBtnUp(void);                                        // 0x50fac0
	void OnGpLimitedTooltipInit(intptr_t param);                      // 0x50f590
	void OnGpStateTicketInit(intptr_t param);                         // 0x50f600
	// Places the GP clock (0xe74e1c) at the gpstate area's position (JP
	// FrWnd rect +0x34/+0x38) plus the doubles 10.0 (0xcff068) / 9.0
	// (0xd04f60).
	void OnGpStateInit(intptr_t param);                               // 0x50f670
	void OnGpStateOwnerDraw(intptr_t param);                          // 0x50f700
	// All five sort buttons NORMAL, the one of sort (0x63 event, 0
	// rookie, 1..3 c1..c3) PRESSED, then the shared doc's room sort
	// (0x62bf20; name ours).
	void SelectSortButton(int sort);                                  // 0x50f940

	// Run by the destructor (name ours): g_audio->StopSfx("gp_lobby_amb")
	// (CSoundManager slot 4) and clears +0x64 of the object at 0xe74e1c
	// when there is one (a JP singleton: ctor 0x55d1b0 stores it, vtable
	// 0xd0a1b0, base ctor 0xb67ba0; 0x50f250 returns it, 0x50f260 tests
	// it: CGrandPrixClock above).
	void StopLobbyAmbience(void);                                     // 0x50f270
	// Called by OnInit (name ours): sends packet 0x5c (WSendPacket 0xa4e800,
	// Send 0xa4dab0(0)), the room state (+0x140) 0x5a0310(sort, this) and the
	// sort-button select 0x50f940(sort) with the sort from the shared doc
	// 0x62bf30; clears m_value2f4 and sends a 0x753e/4 message
	// (0x1a000264, &m_value2f4) to the "Gateway" (0xe09064) target of
	// 0xe73df0 (0x9b79c0, 0x9bac50, vtable +0x2c); sets +0x64 of the
	// 0xe74e1c object to 1 and plays "gp_lobby_amb" looped (g_audio slot 3,
	// 0.5f, 200.0f).
	void SetupGpRoomList(void);                                       // 0x50fb20
	// Run by the constructor (name ours): shared-doc bytes +0x1094c and
	// +0x109b8 = 0x17, FrRoomListDlg 0x51ca40, then releases (vtable slot 0,
	// deleting) and clears every node's +0x14 object of the shared doc's
	// std::map at +0x3890 and erases it (0x50fc40); rest not traced yet.
	void InitGpRoomState(void);                                       // 0x50fd90

	FrArea* m_pGpLimitedTooltip;               // 0x300 gp_limited_tooltip
	FrArea* m_pGpStateTicket;                  // 0x304 gpstate_ticket
	UNKNOWN(0x308, 0x310);
};
CHECK_SIZE(FrGrandPrixRoomDlg, 0x310);
BEGIN_LAYOUT_CHECK(FrGrandPrixRoomDlg)
	CHECK_FIELD(FrGrandPrixRoomDlg, m_pGpLimitedTooltip, 0x300);
	CHECK_FIELD(FrGrandPrixRoomDlg, m_pGpStateTicket, 0x304);
END_LAYOUT_CHECK

// The factory 0xab4990 (jp/pairing.tsv: KR CreateForm<FrDtRoomDlg>, wrong).
FrMultiplayRoomDlg* CreateMultiplayRoomDlg(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent);
// The factory 0xab4a60 (jp/pairing.tsv: KR CreateForm<FrPointEventDlg>,
// wrong).
FrGrandPrixRoomDlg* CreateGrandPrixRoomDlg(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent);

#endif
