// The JP 983 mastery slot animation (RTTI RFSoltAniWnd :
// RFFormModalBase<RFWndEx>, vtable 0xd0fb6c, 34 slots: no new virtuals; the
// RFModalRenderer sub-object at +0x180 has vtable 0xd0fb64), loaded from
// kam_mastery_slotani.xml by CreateSoltAniWnd 0x5c5870. The mastery slot
// window (RFCharMasteryTileDlg, 0x5c59e0) creates it, shows it modal and
// plays the stat's effect (PlayStatEffect); the window draws a CPvsFxBox of
// its own and closes itself once the effect has finished.
// Declarations and the JP x86 layout, from the JP exe's bodies
// (node scripts/disasm.ts --jp 0x<va>). m_fxBox is the KR CPvsFxBox (JP size
// 0x43c, KR 0x364): the members after it keep their JP offsets in comments
// only (no CHECK_FIELD past m_fxBox).
#ifndef PG_JP_JPMASTERYSLOTANI_H
#define PG_JP_JPMASTERYSLOTANI_H

#include "defines.h"
#include "padding.h"
#include "RFFormModalBase.h"

#include "fx.h"
#include "wmath.h"

// Size 0x5f8.
class RFSoltAniWnd : public RFFormModalBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5c5130: RFWndEx::RFWndEx, RFModalRenderer (0x5e51f0),
	// m_updateRegistered cleared, then m_fxBox constructed (CPvsFxBox
	// 0x48e9b0).
	RFSoltAniWnd();
	// [1] 0x5c59b0 (scalar deleting destructor; the sub-object's [0]
	// 0x5c51b0 adjusts this by -0x180 and jumps to it); ~RFSoltAniWnd
	// 0x5c51c0: the effects cleared, the mastery slot window told
	// (RFCharMasteryTileDlg::OnSlotAnimationDone 0x5c5000), then
	// ~CPvsFxBox (0x48eae0), RFFormModalBase<RFWndEx>'s vtables,
	// ~RFModalRenderer (0x5e5200) and ~RFWndEx.
	virtual ~RFSoltAniWnd();

	// 0x5c08b0 (name ours): centres the effect box on the window and, for
	// stat 0 (power), 1 (control), 3 (spin) or 4 (curve), loads
	// mastery_eff_<stat>.seq, places it on the window's centre and plays
	// "mastery_stat_extend"; stat 2 (and anything past 4) plays nothing.
	void PlayStatEffect(int stat);

	// 0xe780fc (name ours): the parent the last CreateSoltAniWnd was given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5c1620 (a jump to 0x5c0c30): the effect box on the resource
	// manager and g_camera, its frame time 33.333, and m_camera the
	// identity placed at (0, 0, -30).
	virtual void OnInitialUpdate();
	// [31] 0x5c02e0: closes the window once the effects have finished;
	// otherwise runs them for the frame (milliseconds).
	virtual void OnUpdate(UpdateEventArgs& args);
	// [32] 0x5c0a80: draws the effects from m_camera with a 90 degree field
	// of view and a 0.5 - 50 perspective clip, then puts g_view back.
	virtual void OnDraw();

	// 0x0-0x184: RFFormModalBase<RFWndEx> (RFWndEx, RFModalRenderer at
	// 0x180, m_updateRegistered at 0x184).
	PADDING(0x185, 0x188);
	CPvsFxBox m_fxBox;   // 0x188 (JP size 0x43c: its two attacher maps at JP 0x420 / 0x430)
	UNKNOWN(0x5c4, 0x5c8);
	WMatrix m_camera;    // 0x5c8 the camera the effects are drawn from (OnInitialUpdate, OnDraw)
};
BEGIN_LAYOUT_CHECK(RFSoltAniWnd)
	CHECK_FIELD(RFSoltAniWnd, m_fxBox, 0x188);
END_LAYOUT_CHECK

// 0x5c5870 (parent in ecx, name ours): the window from
// kam_mastery_slotani.xml, made as g_jpModalCreateMode
// (0xe03260) says; the parent is kept in RFSoltAniWnd::s_loadParent.
RFWnd* CreateSoltAniWnd(RFWnd* parent);

// The JP CFxSequence fields PlayStatEffect sets (JP layout, not the KR
// one): the screen position at +0x18 / +0x1c and the position at +0x110c.
void Jp_FxSequence_SetScreenPos(CFxSequence* sequence, float x, float y);
void Jp_FxSequence_SetPos(CFxSequence* sequence, const WVector& pos);
// 0xf04154 (name ours): the position PlayStatEffect gives the sequence
// (.bss; its initializer is not traced).
extern WVector g_jpSlotAniSequencePos;

#endif // PG_JP_JPMASTERYSLOTANI_H
