// JP-only game code (no KR counterpart): the masked gauge picture shared by
// the daily-quest / achievement forms (JpQuestForms.h) and the alchemy
// dialog (JpAlchemyDlg.h). No RTTI, no vtable, size 0x4c (x86): constructor
// 0x923410, destructor 0x923290, reset 0x9232e0, overlay swap 0x9232f0, set-up 0x923450, target
// setters 0x923520 / 0x9235e0, the per-frame update 0x923650 and the draw
// 0x9239f0 with its horizontal / vertical fills 0x923700 / 0x923860.
#ifndef PG_JP_JPMASKGAUGE_H
#define PG_JP_JPMASKGAUGE_H

#include "defines.h"
#include "padding.h"
#include "wtypes.h"

class WOverlay;

// The 16-byte placement the set-up takes by value (the callers copy four
// floats with `sub esp,10h`): +0x0 x, +0x4 y, +0x8 not read by 0x923450,
// +0xc the upper limit of the value (1000.0f in the alchemy dialog).
// size 0x10 (x86)
struct JpMaskGaugePlacement {
	float x;          // 0x0
	float y;          // 0x4
	float unused8;    // 0x8 (0.0f at every traced caller)
	float limit;      // 0xc
};

class JpMaskGauge {
public:
	JpMaskGauge(void);
	~JpMaskGauge(void);

	// 0x9232e0: the progress, the shown value and the target back to 0.
	void Reset(void);
	// 0x9232f0: for each non-null name, releases the old overlay and loads
	// the new one (WResrcMng::GetOverlay type 0) into m_pOverlays[0] / [1].
	void SetOverlays(const char* mask, const char* gauge);   // 0x9232f0
	// 0x923450: loads the mask and the gauge overlays (WResrcMng::GetOverlay
	// with type 0 on g_resrcmng); false when one is missing. The size is
	// the larger of the two textures plus twice the border.
	bool Setup(const char* mask, const char* gauge, JpMaskGaugePlacement placement, int border, bool animating, float speed, bool horizontal, bool drawMask);
	// 0x923520: the target, clamped to [m_min, m_limit].
	void SetTarget(float target);
	// 0x923580 (name ours): the target, clamped to [m_min, m_limit], also
	// shown at once.
	void SetTargetNow(float target);
	// 0x9235e0: the same, the progress first rescaled so that the shown
	// value keeps its place on the way to the new target.
	void SetTargetFromCurrent(float target);
	// 0x923650
	void Update(const float& dt);
	// 0x9239f0: the fill (DrawHorizontal, else DrawVertical), then the
	// m_tickCount - 1 tick marks of m_pTickOverlay spread over the gauge.
	void Draw(void);
	// 0x4f30d0 (header inline, copy in the alchemy dialog's run): the shown
	// value reached the target.
	bool IsFinished(void) { if (m_value == m_target) return true; return false; }   // 0x4f30d0
	// 0x4f30f0 (header inline, the same run): the shown value reached the
	// upper limit.
	// The shown value (read inline by the alchemy dialog's OnPrePointDraw).
	float GetValue(void) const { return m_value; }
	bool IsFull(void) { if (m_value == m_limit) return true; return false; }   // 0x4f30f0
	// Inlined by FrOnedayQuest::OnQuestDone (0x519810, a byte store at
	// +0x49): false shows 1 - m_progress of the target.
	void SetRising(bool rising) { m_rising = rising; }
	// Inlined by FrAchievement::OnInit (0x4e9960) after each Setup: a
	// float store at +0x4 and a byte store at +0x20 (names ours).
	void SetValue(float value) { m_value = value; }
	void SetAnimating(bool animating) { m_animating = animating; }

private:
	// 0x923700: the mask (when m_drawMask), then the gauge clipped to the
	// left part (m_value - m_min) / (m_limit - m_min) of its inner width.
	void DrawHorizontal(void);
	// 0x923860: the same from the bottom up; it draws the mask whatever
	// m_drawMask.
	void DrawVertical(void);

	float m_progress;                     // 0x0 (0..1, grows by dt * 0.5 * m_speed while animating)
	float m_value;                        // 0x4 (shown value, in [m_min, m_target])
	float m_target;                       // 0x8 (the value the gauge runs to; clamped to [m_min, m_limit])
	float m_min;                          // 0xc
	float m_limit;                        // 0x10 (upper limit of the target, set by Setup)
	WOverlay* m_pOverlays[2];             // 0x14 (mask, gauge; set by Setup, released by the dtor)
	WOverlay* m_pTickOverlay;             // 0x1c (the tick mark drawn by Draw; released by the dtor)
	bool m_animating;                     // 0x20 (0: Update shows m_target at once)
	float m_speed;                        // 0x24 (1.0)
	float m_x;                            // 0x28
	float m_y;                            // 0x2c
	int m_border;                         // 0x30
	int m_tickCount;                      // 0x34 (not set by Setup; use not traced)
	int m_width;                          // 0x38 (larger texture width + 2 * border)
	int m_height;                         // 0x3c (larger texture height + 2 * border)
	float m_fillX;                        // 0x40 (the fill's last drawn x; 0.0f by Setup)
	float m_fillY;                        // 0x44 (the fill's last drawn y; 0.0f by Setup)
	bool m_horizontal;                    // 0x48 (Draw: DrawHorizontal, else DrawVertical)
	bool m_rising;                        // 0x49 (1; 0 shows 1 - m_progress)
	bool m_drawMask;                      // 0x4a (1; DrawHorizontal draws the mask only when set)
	UNKNOWN(0x4b, 0x4c);
};
// size 0x4c (x86)

#endif // PG_JP_JPMASKGAUGE_H
