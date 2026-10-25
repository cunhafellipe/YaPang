// JP-only game code: the masked gauge picture (see JpMaskGauge.h). JP VAs in
// comments, in JP address order.
#include "JpMaskGauge.h"

#include "woverlay.h"
#include "wresrcmng.h"
#include "wview.h"

// 0xe428fc: the whole texture (0, 0, 1, 1), the tick marks' source rectangle.
static const _WRECT s_unitRect = { 0.0f, 0.0f, 1.0f, 1.0f };

// 0x923290
JpMaskGauge::~JpMaskGauge(void) {
	for (int i = 0; i < 2; ++i) {
		if (g_resrcmng != 0 && m_pOverlays[i] != 0) {
			g_resrcmng->Release(m_pOverlays[i]);
			m_pOverlays[i] = 0;
		}
	}
	if (g_resrcmng != 0 && m_pTickOverlay != 0) {
		g_resrcmng->Release(m_pTickOverlay);
		m_pTickOverlay = 0;
	}
}

// 0x9232e0
void JpMaskGauge::Reset(void) {
	m_target = 0.0f;
	m_value = 0.0f;
	m_progress = 0.0f;
}

// 0x9232f0
void JpMaskGauge::SetOverlays(const char* mask, const char* gauge) {
	if (mask != 0) {
		if (g_resrcmng != 0 && m_pOverlays[0] != 0) {
			g_resrcmng->Release(m_pOverlays[0]);
			m_pOverlays[0] = 0;
		}
		m_pOverlays[0] = g_resrcmng->GetOverlay(mask, 0);
	}
	if (gauge != 0) {
		if (g_resrcmng != 0 && m_pOverlays[1] != 0) {
			g_resrcmng->Release(m_pOverlays[1]);
			m_pOverlays[1] = 0;
		}
		m_pOverlays[1] = g_resrcmng->GetOverlay(gauge, 0);
	}
}

// 0x923410
JpMaskGauge::JpMaskGauge(void) {
	m_progress = 0.0f;
	m_value = 0.0f;
	m_pTickOverlay = 0;
	m_min = 0.0f;
	m_animating = false;
	m_limit = 0.0f;
	m_speed = 1.0f;
	m_x = 0.0f;
	m_y = 0.0f;
	m_border = 0;
	m_tickCount = 0;
	m_rising = true;
	m_pOverlays[0] = 0;
	m_pOverlays[1] = 0;
	m_drawMask = true;
}

// 0x923450
bool JpMaskGauge::Setup(const char* mask, const char* gauge, JpMaskGaugePlacement placement, int border, bool animating, float speed, bool horizontal, bool drawMask) {
	m_pOverlays[0] = g_resrcmng->GetOverlay(mask, 0);
	m_pOverlays[1] = g_resrcmng->GetOverlay(gauge, 0);
	for (int i = 0; i < 2; ++i) {
		if (m_pOverlays[i] == 0) {
			return false;
		}
	}
	m_limit = placement.limit;
	m_x = placement.x;
	m_animating = animating;
	m_speed = speed;
	m_horizontal = horizontal;
	m_y = placement.y;
	m_fillY = 0.0f;
	m_border = border;
	m_fillX = 0.0f;
	int width = m_pOverlays[1]->GetWidth();
	if (m_pOverlays[0]->GetWidth() > width) {
		width = m_pOverlays[0]->GetWidth();
	}
	m_width = width + border * 2;
	int height = m_pOverlays[1]->GetHeight();
	if (m_pOverlays[0]->GetHeight() > height) {
		height = m_pOverlays[0]->GetHeight();
	}
	m_height = height + border * 2;
	m_drawMask = drawMask;
	return true;
}

// 0x923520
void JpMaskGauge::SetTarget(float target) {
	m_target = target;
	float lo = m_min;
	float hi = m_limit;
	if (target < lo) {
		target = lo;
	} else if (target > hi) {
		target = hi;
	}
	m_target = target;
}

// 0x923580
void JpMaskGauge::SetTargetNow(float target) {
	m_target = target;
	float lo = m_min;
	float hi = m_limit;
	if (target < lo) {
		target = lo;
	} else if (target > hi) {
		target = hi;
	}
	m_target = target;
	m_value = target;
}

// 0x9235e0
void JpMaskGauge::SetTargetFromCurrent(float target) {
	if (m_value != 0.0f) {
		m_progress = m_value / target;
	}
	m_target = target;
	float lo = m_min;
	float hi = m_limit;
	if (target < lo) {
		target = lo;
	} else if (target > hi) {
		target = hi;
	}
	m_target = target;
}

// 0x923650
void JpMaskGauge::Update(const float& dt) {
	if (!m_animating) {
		m_value = m_target;
		return;
	}
	m_progress = (float)(dt * 0.5 * m_speed + m_progress);
	if (m_progress > 1.0f) {
		m_progress = 1.0f;
	}
	float t = m_progress;
	if (!m_rising) {
		t = 1.0f - t;
	}
	m_value = t * m_target;
	if (m_value < m_min) {
		m_value = m_min;
	} else if (m_value > m_target) {
		m_value = m_target;
	}
}

// 0x923700: the overlays' slot +0x8 (WOverlay::Render) takes the texture
// rectangle (0..1) before the screen one, as in Draw.
void JpMaskGauge::DrawHorizontal(void) {
	_WRECT src;
	src.x = 0.0f;
	src.y = 0.0f;
	src.w = 1.0f;
	src.h = 1.0f;
	_WRECT dst;
	dst.x = m_x;
	dst.y = m_y;
	dst.w = (float)m_pOverlays[0]->GetWidth();
	dst.h = (float)m_pOverlays[0]->GetHeight();
	if (m_drawMask) {
		m_pOverlays[0]->Render(g_view, src, dst, 0, 0xffffffff, 0.0f, 0);
	}
	float borderU = 0.0f;
	int border = m_border;
	if (border > 0) {
		borderU = (float)border / m_pOverlays[1]->GetWidth();
	}
	int width = m_pOverlays[1]->GetWidth();
	float fill = (m_value - m_min) / (m_limit - m_min) * (width - border * 2);
	float u = fill / width;
	src.x = 0.0f;
	src.y = 0.0f;
	src.w = (m_value == 0.0f) ? 0.0f : u + borderU;
	src.h = 1.0f;
	dst.x = m_x;
	dst.y = m_y;
	dst.w = (m_value == 0.0f) ? 0.0f : u * m_pOverlays[1]->GetWidth() + border;
	dst.h = (float)m_pOverlays[1]->GetHeight();
	m_fillX = dst.x;
	m_fillY = dst.y;
	m_pOverlays[1]->Render(g_view, src, dst, 0, 0xffffffff, 0.0f, 0);
}

// 0x923860: the gauge's bottom part, (m_value - m_min) / (m_limit - m_min)
// of its inner height, ending at the mask's bottom edge.
void JpMaskGauge::DrawVertical(void) {
	_WRECT src;
	src.x = 0.0f;
	src.y = 0.0f;
	src.w = 1.0f;
	src.h = 1.0f;
	_WRECT dst;
	dst.x = m_x;
	dst.y = m_y;
	dst.w = (float)m_pOverlays[0]->GetWidth();
	dst.h = (float)m_pOverlays[0]->GetHeight();
	int height = m_pOverlays[1]->GetHeight();
	m_pOverlays[0]->Render(g_view, src, dst, 0, 0xffffffff, 0.0f, 0);
	float borderV = 0.0f;
	int border = m_border;
	if (border > 0) {
		borderV = (float)border / height;
	}
	float fill = (m_value - m_min) / (m_limit - m_min) * (height - border * 2);
	float v = fill / height;
	src.x = 0.0f;
	src.y = (m_value == 0.0f) ? 0.0f : 1.0f - (v + borderV);
	src.w = 1.0f;
	src.h = (m_value == 0.0f) ? 0.0f : v + borderV;
	dst.x = m_x;
	dst.y = m_y;
	float borderF = (float)border;
	dst.y = m_pOverlays[0]->GetHeight() + m_y - (m_pOverlays[1]->GetHeight() * v + borderF);
	dst.w = (float)m_pOverlays[1]->GetWidth();
	dst.h = (m_value == 0.0f) ? 0.0f : v * m_pOverlays[1]->GetHeight() + borderF;
	m_fillX = dst.x;
	m_fillY = dst.y;
	m_pOverlays[1]->Render(g_view, src, dst, 0, 0xffffffff, 0.0f, 0);
}

// 0x9239f0
void JpMaskGauge::Draw(void) {
	if (m_horizontal) {
		DrawHorizontal();
	} else {
		DrawVertical();
	}
	if (m_pTickOverlay == 0) {
		return;
	}
	for (int i = 0; i < m_tickCount - 1; ++i) {
		int x = (int)((float)(m_pOverlays[1]->GetWidth() - m_border * 2) / m_tickCount * (i + 1) + m_x + m_border);
		_WRECT rect;
		rect.x = (float)x;
		rect.y = m_y;
		rect.w = (float)m_pTickOverlay->GetWidth();
		rect.h = (float)m_pTickOverlay->GetHeight();
		// The vtable slot +0x8 (WOverlay::Render): the texture rectangle
		// (0..1), then the screen one.
		m_pTickOverlay->Render(g_view, s_unitRect, rect, 0, 0xffffffff, 0.0f, 0);
	}
}
