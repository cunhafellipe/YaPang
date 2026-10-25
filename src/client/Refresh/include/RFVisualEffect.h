// Original: client\Refresh\include\RFVisualEffect.h (JP 983 exe; path from
// the library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFVISUALEFFECT_H
#define PG_CLIENT_REFRESH_INCLUDE_RFVISUALEFFECT_H

#include <stdint.h>

#include "RFExport.h"
#include "defines.h"

class RFWnd;

// The base of the per-window visual effects (RFVisualEffectDefault,
// …Fade, …Anim): vtable 0xd630c4, 4 slots, size 0x4 (the ctors write only
// the vptr and the copy ctor copies nothing: no members). The three effect
// virtuals are pure here; the destructor, constructors and assignment are
// header inlines (their kept copies 0x404490-0x4044c0 lie in the shared
// header-inline run 0x4037e0-0x404f40, with RFInterpolation's and
// RFDirector's, not in this class's .cpp run at 0x42cbc0).
// RFWnd::SetVisualEffect (0x407240) deletes the effect it replaces through
// slot 3 and keeps the Clone() of the new one at +0x7c. Update advances the effect on a window by a frame time; Reset
// rewinds it (the fade writes the window's +0x3c alpha and reads its
// +0x78 visual state). Reset returns nothing: none of the three overrides
// sets eax deliberately (Anim's tail-calls RFDirector::FrameMove).
class RF_API RFVisualEffect {
	LAYOUT_CHECK_FRIEND;

public:
	virtual RFVisualEffect* Clone() = 0;      // [0]
	virtual void Reset(RFWnd*) = 0;           // [1]
	virtual void Update(RFWnd*, float) = 0;   // [2]
	// [3] vector deleting destructor 0x404500
	// 0x404490
	virtual ~RFVisualEffect() {}

	// 0x4044a0
	RFVisualEffect() {}
	// 0x4044b0
	RFVisualEffect(RFVisualEffect const&) {}
	// 0x4044c0
	RFVisualEffect& operator=(RFVisualEffect const&) { return *this; }
};
CHECK_SIZE(RFVisualEffect, 0x4);

// The three effects are not exported (no unit yet). Their destructors are
// implicit: slot 3 of each (0x42cc30, 0x42cdd0, 0x42cf60) is a scalar
// deleting destructor that inlines ~RFVisualEffect. The selector 0x42cfa0
// (called by RFWnd::SetVisualEffect, RFWnd::LoadUIAnimation, RFManager)
// keeps one function-local static of each and returns the Clone() of the
// one its index names (0 Default, 1 Fade, 2 Anim).

// The window's open/close visual-state step without animation: vtable
// 0xd64cd4, size 0x4 (Clone 0x42cbc0 allocates 4 bytes and writes only the
// vptr).
class RFVisualEffectDefault : public RFVisualEffect {
public:
	virtual RFVisualEffect* Clone();          // [0] 0x42cbc0
	virtual void Reset(RFWnd*);               // [1] 0x42cbe0
	virtual void Update(RFWnd*, float);       // [2] 0x42cc10 (empty)
	                                          // [3] 0x42cc30 scalar deleting destructor
};
CHECK_SIZE(RFVisualEffectDefault, 0x4);

// The alpha fade: vtable 0xd64ce8, size 0x8 (Clone 0x42cc80 allocates 8
// bytes and zeroes +0x4). Update adds the frame time to m_elapsed, clamps it
// to the fade's duration, and writes the window's +0x3c alpha from
// elapsed/duration (rising while opening, falling while closing).
class RFVisualEffectFade : public RFVisualEffect {
	LAYOUT_CHECK_FRIEND;

public:
	virtual RFVisualEffect* Clone();          // [0] 0x42cc80
	virtual void Reset(RFWnd*);               // [1] 0x42cca0
	virtual void Update(RFWnd*, float);       // [2] 0x42ccc0
	                                          // [3] 0x42cdd0 scalar deleting destructor

	RFVisualEffectFade() : m_elapsed(0.0f) {}

private:
	float m_elapsed;  // 0x4
};
CHECK_SIZE(RFVisualEffectFade, 0x8);
BEGIN_LAYOUT_CHECK(RFVisualEffectFade)
	CHECK_FIELD(RFVisualEffectFade, m_elapsed, 0x4);
END_LAYOUT_CHECK

// Plays the window director's (+0x150) AnimOpening / AnimClosing
// animation: vtable 0xd64cfc, size 0x4 (Clone 0x42ce10 allocates 4 bytes).
class RFVisualEffectAnim : public RFVisualEffect {
public:
	virtual RFVisualEffect* Clone();          // [0] 0x42ce10
	virtual void Reset(RFWnd*);               // [1] 0x42ce30
	virtual void Update(RFWnd*, float);       // [2] 0x42cef0
	                                          // [3] 0x42cf60 scalar deleting destructor
};
CHECK_SIZE(RFVisualEffectAnim, 0x4);

// 0x42cfa0: the effect an index names (0 Default, 1 Fade, 2 Anim; any other
// gives 0), as a new Clone() of a function-local static of each.
// RFWnd::SetVisualEffect and RFManager::SetVisualEffect take a pointer
// below 0x10 as such an index; RFWnd::LoadUIAnimation asks for the Anim one.
// Its callers push the index and it pops it (ret 4) with ecx unset: a free
// __stdcall function.
RFVisualEffect* __stdcall CreateVisualEffect(int32_t index);

#endif
