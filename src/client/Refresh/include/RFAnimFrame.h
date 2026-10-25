// Original: client\Refresh\include\RFAnimFrame.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFANIMFRAME_H
#define PG_CLIENT_REFRESH_INCLUDE_RFANIMFRAME_H

#include <string>
#include <vector>

#include "defines.h"
#include "RFExport.h"
#include "RFMatrix.h"
#include "RFRect.h"

class RFInterpolation;
class RFTexture;

// One animated value of a keyframe, 0x8 (the type RFAnimationObject::GetXmlValue
// 0x42c890 reads an attribute into): the value and the interpolation that
// leads to it, owned (~RFAnimFrame deletes it through 0x42d100). The
// keyframe's constructor zeroes both inline; RFAnimationFrame::Clear
// (0x42af20) and InterpolateFrame (0x42d1a0) assign each value through a
// stack temporary (value stored, then dword-copied, no interpolation).
struct RFFloatAnimationFrame {
	RFFloatAnimationFrame() : m_value(0.0f), m_interpolation(0) {}  // 0x42aea0
	RFFloatAnimationFrame(float value) : m_value(value), m_interpolation(0) {}  // 0x42aeb0

	float m_value;                     // 0x0
	RFInterpolation* m_interpolation;  // 0x4
};
CHECK_SIZE(RFFloatAnimationFrame, 0x8);
CHECK_FIELD(RFFloatAnimationFrame, m_value, 0x0);
CHECK_FIELD(RFFloatAnimationFrame, m_interpolation, 0x4);

// A keyframe of RFAnimFrame, 0x64 (the vector's element size). GenAnimMatrix
// 0x42dfb0 maps x/y onto the canvas, scales the center by the canvas size,
// turns the rotation from degrees and Render 0x42e2e0 multiplies the alpha;
// Render skips an invisible frame. Clear (0x42af20) resets the defaults
// (time 0, visible, no sound, position 0, scale 1, center 0.5, rotation 0,
// alpha 1); its copy (0x403db0) is the implicit operator=.
struct RFAnimationFrame {
	RFAnimationFrame() { Clear(); }

	// 0x42af20 (with RFAnimationObject's code).
	void Clear();

	unsigned int m_time;               // 0x0, FrameMove's tick
	bool m_visible;                    // 0x4
	std::wstring m_sfx;                // 0x8, PlaySfx
	RFFloatAnimationFrame m_x;         // 0x24
	RFFloatAnimationFrame m_y;         // 0x2c
	RFFloatAnimationFrame m_scaleX;    // 0x34
	RFFloatAnimationFrame m_scaleY;    // 0x3c
	RFFloatAnimationFrame m_centerX;   // 0x44
	RFFloatAnimationFrame m_centerY;   // 0x4c
	RFFloatAnimationFrame m_rotation;  // 0x54, degrees
	RFFloatAnimationFrame m_alpha;     // 0x5c
};
CHECK_SIZE(RFAnimationFrame, 0x64);
CHECK_FIELD(RFAnimationFrame, m_time, 0x0);
CHECK_FIELD(RFAnimationFrame, m_visible, 0x4);
CHECK_FIELD(RFAnimationFrame, m_sfx, 0x8);
CHECK_FIELD(RFAnimationFrame, m_x, 0x24);
CHECK_FIELD(RFAnimationFrame, m_y, 0x2c);
CHECK_FIELD(RFAnimationFrame, m_scaleX, 0x34);
CHECK_FIELD(RFAnimationFrame, m_scaleY, 0x3c);
CHECK_FIELD(RFAnimationFrame, m_centerX, 0x44);
CHECK_FIELD(RFAnimationFrame, m_centerY, 0x4c);
CHECK_FIELD(RFAnimationFrame, m_rotation, 0x54);
CHECK_FIELD(RFAnimationFrame, m_alpha, 0x5c);

// A keyframe animation of a texture: vtable 0xd630f0, 1 slot, size 0x19c.
// Play interpolates from m_fromFrame to m_toFrame into m_nowFrame and
// GenAnimMatrix turns m_nowFrame into m_animMatrix for Render.
class RF_API RFAnimFrame {
	LAYOUT_CHECK_FRIEND;

public:
	virtual ~RFAnimFrame();  // [0] scalar deleting destructor 0x403cc0, ~RFAnimFrame 0x42d570

	RFMatrix const& GetAnimMatrix() const { return m_animMatrix; }  // 0x403a70
	RFAnimFrame();                          // 0x42d440
	void SetCanvas(RFRect const&);          // 0x42d810
	void AddFrame(RFAnimationFrame const&); // 0x42d840
	bool InsertFrame(RFAnimationFrame const&);  // 0x42d860
	void GetNowFrame(RFAnimationFrame*) const;  // 0x42d8e0
	void Stop();                            // 0x42d900
	void End(RFRect const&);                // 0x42d930
	void Play();                            // 0x42d990
	void PlayPendulum();                    // 0x42da40
	bool IsPlaying() const;                 // 0x42db30
	bool IsIdle() const;                    // 0x42db40
	void FrameMovePendulum(unsigned int, RFRect const&);  // 0x42db50
	void FrameMove(unsigned int, RFRect const&);          // 0x42dd80
	void GenAnimMatrix(RFRect const&);      // 0x42dfb0
	void Render(RFTexture*, float);         // 0x42e2e0

protected:
	void PlaySfx(std::wstring const&);      // 0x42d740

private:
	std::vector<RFAnimationFrame> m_frames;  // 0x4
	RFAnimationFrame m_fromFrame;            // 0x14
	RFAnimationFrame m_toFrame;              // 0x78
	RFAnimationFrame m_nowFrame;             // 0xdc, GetNowFrame
	int m_frameIndex;                        // 0x140, index of m_fromFrame in m_frames
	bool m_playing;                          // 0x144, IsPlaying
	bool m_changed;                          // 0x145, cleared by FrameMove; Render draws an invisible frame when set
	bool m_idle;                             // 0x146, IsIdle; Render draws nothing
	bool m_forward;                          // 0x147, set by Play
	bool m_holdFrame;                        // 0x148, Play of a single frame; FrameMove does not interpolate
	RFRect m_canvas;                         // 0x14c, SetCanvas
	RFMatrix m_animMatrix;                   // 0x15c, GetAnimMatrix
};
CHECK_SIZE(RFAnimFrame, 0x19c);
BEGIN_LAYOUT_CHECK(RFAnimFrame)
	CHECK_FIELD(RFAnimFrame, m_frames, 0x4);
	CHECK_FIELD(RFAnimFrame, m_fromFrame, 0x14);
	CHECK_FIELD(RFAnimFrame, m_toFrame, 0x78);
	CHECK_FIELD(RFAnimFrame, m_nowFrame, 0xdc);
	CHECK_FIELD(RFAnimFrame, m_frameIndex, 0x140);
	CHECK_FIELD(RFAnimFrame, m_playing, 0x144);
	CHECK_FIELD(RFAnimFrame, m_changed, 0x145);
	CHECK_FIELD(RFAnimFrame, m_idle, 0x146);
	CHECK_FIELD(RFAnimFrame, m_forward, 0x147);
	CHECK_FIELD(RFAnimFrame, m_holdFrame, 0x148);
	CHECK_FIELD(RFAnimFrame, m_canvas, 0x14c);
	CHECK_FIELD(RFAnimFrame, m_animMatrix, 0x15c);
END_LAYOUT_CHECK

#endif
