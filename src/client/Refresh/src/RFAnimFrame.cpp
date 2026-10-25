// Original: client\Refresh\src\RFAnimFrame.cpp (JP 983 exe; VS 2010 SP1)
#include <new>
#include <cmath>

#include "RFAnimFrame.h"
#include "RFInterface.h"
#include "RFInterpolation.h"
#include "RFManager.h"
#include "RFRenderer.h"
#include "RFSingleton.h"

// lib: 0x403c90 (RFAnimFrame scalar deleting destructor ??_G, unreferenced),
// 0x42eab0 (_Copy_backward wrapper of 0x42ebe0), 0x42ec80 (_Ptr_cat-style
// return 0)
// lib: 0x42e3a0 (std::vector<RFAnimationFrame>::push_back), 0x42e540 (insert),
// 0x42e8d0 (reserve), 0x42ed30 (_Uninitialized_copy), the implicit copy
// constructor 0x403a80 and operator= 0x403bb0, the vector deleting destructor
// 0x403cc0, RFAnimationFrame's implicit destructor 0x403a40, copy constructor
// 0x403d20 and operator= 0x403db0; std::vector<RFAnimationFrame> internals
// 0x42e9d0 (_Grow_to), 0x42ea10 (_Ufill), 0x42ea30 / 0x42ea40 / 0x42ea50 /
// 0x42eb00 (pointer += n), 0x42ea60 / 0x42ea90 / 0x42eaf0 / 0x42eb30 (empty),
// 0x42ea70 / 0x42eb10 (to _Uninitialized_copy 0x42ed30), 0x42eaa0 (to fill
// 0x42eb40), 0x42ead0 (to _Uninitialized_fill_n 0x42ec90), 0x42ebe0
// (copy_backward)

// 0x42d100
static void DeleteInterpolations(RFAnimationFrame& frame) {
	if (frame.m_x.m_interpolation != 0) {
		delete frame.m_x.m_interpolation;
		frame.m_x.m_interpolation = 0;
	}
	if (frame.m_y.m_interpolation != 0) {
		delete frame.m_y.m_interpolation;
		frame.m_y.m_interpolation = 0;
	}
	if (frame.m_centerX.m_interpolation != 0) {
		delete frame.m_centerX.m_interpolation;
		frame.m_centerX.m_interpolation = 0;
	}
	if (frame.m_centerY.m_interpolation != 0) {
		delete frame.m_centerY.m_interpolation;
		frame.m_centerY.m_interpolation = 0;
	}
	if (frame.m_scaleX.m_interpolation != 0) {
		delete frame.m_scaleX.m_interpolation;
		frame.m_scaleX.m_interpolation = 0;
	}
	if (frame.m_scaleY.m_interpolation != 0) {
		delete frame.m_scaleY.m_interpolation;
		frame.m_scaleY.m_interpolation = 0;
	}
	if (frame.m_rotation.m_interpolation != 0) {
		delete frame.m_rotation.m_interpolation;
		frame.m_rotation.m_interpolation = 0;
	}
	if (frame.m_alpha.m_interpolation != 0) {
		delete frame.m_alpha.m_interpolation;
		frame.m_alpha.m_interpolation = 0;
	}
}

// The value between two keyframe values along the curve to the second one
// (inlined into InterpolateFrame).
static RFFloatAnimationFrame Lerp(float from, float to, RFInterpolation const* curve, float const& t) {
	RFFloatAnimationFrame value(from);
	value.m_value += (to - from) * curve->GetValue(t);
	return value;
}

// 0x42d1a0: the in-between keyframe of FrameMove and FrameMovePendulum; each
// value follows the curve of the keyframe it moves to.
static void InterpolateFrame(RFAnimationFrame* out, RFAnimationFrame from, RFAnimationFrame to, float t) {
	out->m_x = Lerp(from.m_x.m_value, to.m_x.m_value, to.m_x.m_interpolation, t);
	out->m_y = Lerp(from.m_y.m_value, to.m_y.m_value, to.m_y.m_interpolation, t);
	out->m_centerX = Lerp(from.m_centerX.m_value, to.m_centerX.m_value, to.m_centerX.m_interpolation, t);
	out->m_centerY = Lerp(from.m_centerY.m_value, to.m_centerY.m_value, to.m_centerY.m_interpolation, t);
	out->m_scaleX = Lerp(from.m_scaleX.m_value, to.m_scaleX.m_value, to.m_scaleX.m_interpolation, t);
	out->m_scaleY = Lerp(from.m_scaleY.m_value, to.m_scaleY.m_value, to.m_scaleY.m_interpolation, t);
	out->m_rotation = Lerp(from.m_rotation.m_value, to.m_rotation.m_value, to.m_rotation.m_interpolation, t);
	out->m_alpha = Lerp(from.m_alpha.m_value, to.m_alpha.m_value, to.m_alpha.m_interpolation, t);
}

// 0x42d440
RFAnimFrame::RFAnimFrame()
	: m_frameIndex(0),
	  m_playing(false),
	  m_changed(false),
	  m_idle(false),
	  m_forward(true),
	  m_holdFrame(false),
	  m_canvas(0.0f, 0.0f, 0.0f, 0.0f) {
}

// 0x42d570
RFAnimFrame::~RFAnimFrame() {
	for (unsigned int i = 0; i < m_frames.size(); ++i) {
		DeleteInterpolations(m_frames[i]);
	}
	m_frames.clear();
}

// 0x42d740
void RFAnimFrame::PlaySfx(std::wstring const& sfx) {
	std::wstring name(sfx.c_str());
	RFManager::GetInstance().GetInterface()->PlaySfx(name);
}

// 0x42d810
void RFAnimFrame::SetCanvas(RFRect const& canvas) {
	m_canvas = canvas;
}

// 0x42d840
void RFAnimFrame::AddFrame(RFAnimationFrame const& frame) {
	m_frames.push_back(frame);
}

// 0x42d860
bool RFAnimFrame::InsertFrame(RFAnimationFrame const& frame) {
	if (!m_frames.empty()) {
		for (unsigned int i = 0; i < m_frames.size(); ++i) {
			if (frame.m_time < m_frames[i].m_time) {
				m_frames.insert(m_frames.begin() + i, frame);
				return true;
			}
			if (frame.m_time == m_frames[i].m_time) {
				return false;
			}
		}
	}
	m_frames.push_back(frame);
	return true;
}

// 0x42d8e0
void RFAnimFrame::GetNowFrame(RFAnimationFrame* frame) const {
	*frame = m_nowFrame;
}

// 0x42d900
void RFAnimFrame::Stop() {
	m_frameIndex = 0;
	m_nowFrame.Clear();
	m_playing = false;
}

// 0x42d930
void RFAnimFrame::End(RFRect const& canvas) {
	m_frameIndex = m_frames.size() - 1;
	m_nowFrame.Clear();
	m_nowFrame = m_frames[m_frameIndex];
	GenAnimMatrix(canvas);
}

// 0x42d990
void RFAnimFrame::Play() {
	m_frameIndex = 0;
	m_nowFrame.Clear();
	if (m_frames.size() == 1) {
		m_fromFrame = m_toFrame = m_nowFrame = m_frames[m_frameIndex];
		m_forward = true;
		m_holdFrame = true;
		m_playing = true;
		return;
	}
	m_fromFrame = m_nowFrame = m_frames[m_frameIndex];
	m_toFrame = m_frames[m_frameIndex + 1];
	m_forward = true;
	m_holdFrame = false;
	m_playing = true;
}

// 0x42da40
void RFAnimFrame::PlayPendulum() {
	m_frameIndex = m_frames.size() - 1;
	m_nowFrame.Clear();
	if (m_frameIndex < 1) {
		m_fromFrame = m_toFrame = m_nowFrame = m_frames[m_frameIndex];
		m_forward = true;
		m_holdFrame = true;
		m_playing = true;
		return;
	}
	m_fromFrame = m_frames[m_frameIndex - 1];
	m_toFrame = m_frames[m_frameIndex];
	m_toFrame.m_visible = m_fromFrame.m_visible;
	if (m_frameIndex - 2 >= 0) {
		m_fromFrame.m_visible = m_frames[m_frameIndex - 2].m_visible;
	} else if (m_fromFrame.m_time > 0) {
		m_fromFrame.m_visible = false;
	}
	m_nowFrame = m_toFrame;
	m_forward = true;
	m_holdFrame = false;
	m_playing = true;
}

// 0x42db30
bool RFAnimFrame::IsPlaying() const {
	return m_playing;
}

// 0x42db40
bool RFAnimFrame::IsIdle() const {
	return m_idle;
}

// 0x42db50
void RFAnimFrame::FrameMovePendulum(unsigned int time, RFRect const& canvas) {
	m_changed = false;
	m_idle = false;
	if (m_holdFrame == true) {
		if (time > m_nowFrame.m_time) {
			return;
		}
		PlaySfx(m_nowFrame.m_sfx);
		m_playing = false;
		if (time != 0) {
			m_idle = true;
		}
		GenAnimMatrix(canvas);
		return;
	}
	float t = ((float)time - (float)m_fromFrame.m_time) / (float)(m_toFrame.m_time - m_fromFrame.m_time);
	if (t > 1.0f) {
		if (m_frames[m_frameIndex].m_visible == true) {
			m_changed = true;
			GenAnimMatrix(canvas);
			return;
		}
		m_idle = true;
		GenAnimMatrix(canvas);
		return;
	}
	if (t <= 0.0f) {
		--m_frameIndex;
		if (m_frameIndex < 1) {
			m_nowFrame = m_fromFrame;
			PlaySfx(m_nowFrame.m_sfx);
			m_playing = false;
			GenAnimMatrix(canvas);
			return;
		}
		m_fromFrame = m_frames[m_frameIndex - 1];
		m_toFrame = m_frames[m_frameIndex];
		m_toFrame.m_visible = m_fromFrame.m_visible;
		if (m_frameIndex - 2 >= 0) {
			m_fromFrame.m_visible = m_frames[m_frameIndex - 2].m_visible;
		} else if (m_fromFrame.m_time > 0) {
			m_fromFrame.m_visible = false;
		}
		m_nowFrame = m_toFrame;
		PlaySfx(m_nowFrame.m_sfx);
		GenAnimMatrix(canvas);
		return;
	}
	InterpolateFrame(&m_nowFrame, m_fromFrame, m_toFrame, t);
	GenAnimMatrix(canvas);
}

// 0x42dd80
void RFAnimFrame::FrameMove(unsigned int time, RFRect const& canvas) {
	m_changed = false;
	m_idle = false;
	if (m_holdFrame == true) {
		if (time < m_nowFrame.m_time) {
			m_idle = true;
			return;
		}
		PlaySfx(m_nowFrame.m_sfx);
		m_playing = false;
		GenAnimMatrix(canvas);
		return;
	}
	float t = ((float)time - (float)m_fromFrame.m_time) / (float)(m_toFrame.m_time - m_fromFrame.m_time);
	if (t < 0.0f) {
		m_idle = true;
		return;
	}
	if (m_forward == true) {
		m_nowFrame = m_fromFrame;
		PlaySfx(m_nowFrame.m_sfx);
		m_forward = false;
		GenAnimMatrix(canvas);
		return;
	}
	if (t >= 1.0f) {
		++m_frameIndex;
		if (m_frameIndex + 1 >= (int)m_frames.size()) {
			m_nowFrame = m_toFrame;
			PlaySfx(m_nowFrame.m_sfx);
			m_playing = false;
			GenAnimMatrix(canvas);
			return;
		}
		m_fromFrame = m_frames[m_frameIndex];
		m_toFrame = m_frames[m_frameIndex + 1];
		m_nowFrame = m_fromFrame;
		PlaySfx(m_nowFrame.m_sfx);
		GenAnimMatrix(canvas);
		return;
	}
	InterpolateFrame(&m_nowFrame, m_fromFrame, m_toFrame, t);
	GenAnimMatrix(canvas);
}

// 0x42dfb0
void RFAnimFrame::GenAnimMatrix(RFRect const& rect) {
	RFMatrixIdentity(&m_animMatrix);
	float x = m_canvas.Width() * m_nowFrame.m_x.m_value + m_canvas.left;
	float y = m_canvas.Height() * m_nowFrame.m_y.m_value + m_canvas.top;

	RFMatrix translation;
	RFMatrixTranslation(&translation,
		-floor(m_nowFrame.m_centerX.m_value * rect.right + 0.5f),
		-floor(m_nowFrame.m_centerY.m_value * rect.bottom + 0.5f), 0.0f);
	RFMatrix rotation;
	RFMatrixRotationZ(&rotation, m_nowFrame.m_rotation.m_value * (3.141592654f / 180.0f));
	RFMatrix scaling;
	RFMatrixScaling(&scaling, m_nowFrame.m_scaleX.m_value, m_nowFrame.m_scaleY.m_value, 0.0f);

	rotation = scaling * rotation;
	m_animMatrix = translation * rotation;
	m_animMatrix(3, 0) += x;
	m_animMatrix(3, 1) += y;
}

// 0x42e2e0
void RFAnimFrame::Render(RFTexture* texture, float alpha) {
	if (m_idle) {
		return;
	}
	if (m_changed != true && !m_nowFrame.m_visible) {
		return;
	}
	alpha *= m_nowFrame.m_alpha.m_value;
	if (alpha < 1.0f / 255.0f) {
		return;
	}
	RFManager::GetInstance().GetRenderer()->Draw(texture, alpha, m_animMatrix);
}
