// Original: client\Refresh\src\RFProgress.cpp (JP 983 exe, unit bounds in
// jp/units.tsv; the file name is assumed from the class)
#include <new>

#include "RFProgress.h"

#include "RFManager.h"
#include "RFRect.h"
#include "RFRenderer.h"
#include "RFSize.h"
#include "RFVector2.h"

// lib: 0x416e70 (the vector deleting destructor), the implicit copy
// constructor 0x416d50 and operator= 0x416dd0, the scalar deleting destructor
// 0x416e40 (??_GRFProgress, paired with map_symbol.ts)

// The class name, the static const at 0xe46fc8.
std::wstring const RFProgress::TypeName = L"RFProgress";

// 0x44d2b0
RFProgress::RFProgress() {
}

// 0x44d2d0
RFProgress::~RFProgress() {
}

// 0x44d2e0
void RFProgress::Create(WndBaseStruct* wndStruct, RFWnd* parent) {
	RFWnd::Create(wndStruct, parent);
	m_brushDesc = static_cast<ProgressBase*>(wndStruct)->m_brushDesc;
}

// 0x44d310
void RFProgress::PreInitialize() {
	RFWnd::PreInitialize();
	m_brush = 0;
	m_min = 0;
	m_horizontal = true;
	m_max = 100;
	m_progress = 50;
	m_unknown188 = true;
}

// 0x44d3a0
void RFProgress::LoadResource() {
	m_brush = CreateBrush(m_brushDesc);
}

// 0x44d3c0
void RFProgress::OnDestroy() {
	if (m_brush != 0) {
		delete m_brush;
		m_brush = 0;
	}
}

// 0x44d3f0 (the original inlines RFWnd::GetScreenPos 0x406670 and GetAlpha
// 0x4075c0 and realigns its frame, LTCG)
void RFProgress::OnDraw() {
	RFRect screenRect = ClientToScreen();
	RFRect clip;
	clip.IntersectRect(m_rc.m_clipRect, screenRect);
	if (m_rc.m_clip) {
		RFManager::s_renderer->PushClipRect(clip);
	}
	float width = GetSize().width;
	float height = GetSize().height;
	RFVector2 pos = GetScreenPos();
	// The filled share of the bar, 0 to 1.
	float ratio = static_cast<float>(m_progress - m_min) / (m_max - m_min);
	RFSize size;
	if (m_horizontal) {
		size = RFSize(width * ratio, height);
	} else {
		// A vertical bar fills from the bottom.
		pos.y += (1.0f - ratio) * GetSize().height;
		size = RFSize(width, height * ratio);
	}
	float depth = m_rc.m_depth;
	m_brush->Draw(pos, size, GetAlpha(false), depth);
	RFRect finalScreenRect = ClientToScreen();
	RFRect screenClip;
	screenClip.IntersectRect(m_rc.m_clipRect, finalScreenRect);
	m_rc.m_clipRect = screenClip;
	if (m_rc.m_clip) {
		RFManager::s_renderer->PopClipRect();
	}
}

// 0x44d620
void RFProgress::SetRange(int min, int max) {
	m_min = min;
	m_max = max;
	if (m_progress < min) {
		m_progress = min;
	} else if (m_progress > max) {
		m_progress = max;
	}
}

// 0x44d660
void RFProgress::SetProgress(int progress) {
	m_progress = progress;
	if (progress < m_min) {
		m_progress = m_min;
	} else if (progress > m_max) {
		m_progress = m_max;
	}
}

// 0x44d690
int RFProgress::GetProgress() const {
	return m_progress;
}
