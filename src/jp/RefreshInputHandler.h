// The JP 983 game's input bridge to Refresh (RefreshInputHandler, named by
// the RTTI of its shared_ptr control block std::tr1::_Ref_count<
// RefreshInputHandler>, vtable 0xd13d5c). Not polymorphic, size 0x48
// (RFSceneManager::Setup's operator new; constructor 0x5f4da0). Owned by
// RFSceneManager as a std::tr1::shared_ptr. The mouse is polled from the
// game's CInputManager (g_input, 0xf00510) every frame; the window procedure
// only queues the keyboard messages. Bodies in RefreshInputHandler.cpp.
#ifndef PG_JP_REFRESHINPUTHANDLER_H
#define PG_JP_REFRESHINPUTHANDLER_H

#include <stdint.h>
#include <map>

#include <windows.h>

#include "CriticalSection.h"
#include "defines.h"
#include "padding.h"
#include "wmath.h"

class CInputManager;

// The mouse as RefreshInputHandler last saw it (no RTTI, no vtable; size
// 0x10, RefreshInputHandler +0x2c).
class RefreshMouseState {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5f3f20: Reset.
	RefreshMouseState();
	// 0x5f38e0: the position to WVector::ZERO (0xf04154), the wheel to 0.
	void Reset();
	// 0x5f3900: the input's button, wheel and position changes since the
	// last poll, as the flags RefreshInputHandler::DispatchMouse injects:
	// left down 1 (4 on a double click), left up 2, right down 8, right up
	// 0x10, middle down 0x20, middle up 0x40, wheel 0x80; with none of them,
	// the position is refreshed and the result is 0x100 (move). 0 without an
	// input.
	int Poll(CInputManager* input);

	WVector m_pos;                                          // 0x0 the mouse position (CInputManager::GetMousePoint)
	float m_wheel;                                          // 0xc the last wheel delta (GetMouseDelta().z)
};
CHECK_SIZE(RefreshMouseState, 0x10);
BEGIN_LAYOUT_CHECK(RefreshMouseState)
	CHECK_FIELD(RefreshMouseState, m_pos, 0x0);
	CHECK_FIELD(RefreshMouseState, m_wheel, 0xc);
END_LAYOUT_CHECK

class RefreshInputHandler {
	LAYOUT_CHECK_FRIEND;
public:
	RefreshInputHandler();  // 0x5f4da0

	// 0x5f4e60: under the lock, unless m_locked, m_keyMessages[msg] =
	// (wParam, lParam). Called by RFSceneManager::OnWindowMessage
	// (0x603850) for WM_KEYDOWN, WM_KEYUP and WM_LBUTTONUP.
	void QueueMessage(UINT msg, WPARAM wParam, LPARAM lParam);
	// 0x5f4b00: once per frame from RFSceneManager::Update (nothing without
	// the RFSceneManager instance): DispatchKeys, DispatchMouse(dt), clears
	// the queue (std::map::clear, 0x5f4640) and m_locked.
	void Update(float dt);
	// 0x5f4870: unless m_disabled, each queued 0x100 → InjectKeyDown,
	// 0x101 → InjectKeyUp (RFSceneManager 0x603420/0x603440).
	void DispatchKeys();
	// 0x5f3a00: reads the input device (0xf00510) into m_mouse (0x5f3900)
	// and injects the first of L down/up/dblclick, R down/up (flags 1, 2, 4,
	// 8, 0x10), the wheel (flag 0x80, with the repeat delay m_wheelDelay)
	// and the move (flag 0x100) through RFSceneManager 0x6032d0…0x6033f0.
	void DispatchMouse(float dt);
	// 0x5f3b90: the mouse flags since the last poll (m_mouse.Poll(g_input)), 0 without g_input.
	int PollMouse();

	// Ours: RFSceneManager 0x6038f0 / 0x603920 write and read +0x44 and
	// reset +0x2c (0x5f38e0) directly.
	bool IsDisabled() const { return m_disabled; }
	void SetDisabled(bool disabled) { m_disabled = disabled; }
	RefreshMouseState& GetMouse() { return m_mouse; }

protected:
	CCriticalSection m_lock;                                // 0x0  (0x18 on x86)
	bool m_locked;                                          // 0x18
	PADDING(0x19, 0x1c);
	std::map<UINT, std::pair<WPARAM, LPARAM> > m_keyMessages;  // 0x1c (VS 2010: allocator byte, head 0x20, size 0x24)
	PADDING(0x28, 0x2c);
	RefreshMouseState m_mouse;                              // 0x2c
	float m_wheelTimer;                                     // 0x3c counts down by dt; a wheel is injected only at 0
	float m_wheelDelay;                                     // 0x40 0.2f (0xd097cc), the wheel repeat delay
	bool m_disabled;                                        // 0x44 no injection at all when set
	PADDING(0x45, 0x48);
};
CHECK_SIZE(RefreshInputHandler, 0x48);
BEGIN_LAYOUT_CHECK(RefreshInputHandler)
	CHECK_FIELD(RefreshInputHandler, m_locked, 0x18);
	CHECK_FIELD(RefreshInputHandler, m_keyMessages, 0x1c);
	CHECK_FIELD(RefreshInputHandler, m_mouse, 0x2c);
	CHECK_FIELD(RefreshInputHandler, m_wheelTimer, 0x3c);
	CHECK_FIELD(RefreshInputHandler, m_wheelDelay, 0x40);
	CHECK_FIELD(RefreshInputHandler, m_disabled, 0x44);
END_LAYOUT_CHECK

#endif
