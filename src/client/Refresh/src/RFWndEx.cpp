// Original: client\Refresh\src\RFWndEx.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include "RFWndEx.h"

#include "RFManager.h"
#include "RFRect.h"
#include "RFRenderer.h"
#include "RFVector2.h"

// lib: 0x414270, 0x4141d0, 0x414210, 0x433b10, 0x414240
// (the vector deleting destructor, the implicit copy constructor and copy
// assignment, RFWndExFactory's scalar deleting destructor, RFWndEx's
// unreferenced scalar deleting destructor ??_GRFWndEx, paired)

// The class name, the static const at 0xe46c64.
std::wstring const RFWndEx::TypeName = L"Window";

// 0x41d980
RFWndEx::RFWndEx()
	: m_brush(0)
	, m_brushDesc(0) {
}

// 0x41d9a0
RFWndEx::~RFWndEx() {
}

// 0x41d9b0
void RFWndEx::Create(WndBaseStruct* wndStruct, RFWnd* parent) {
	RFWnd::Create(wndStruct, parent);
	m_brushDesc = static_cast<WndExBase*>(wndStruct)->m_brushDesc;
}

// 0x41d9e0
void RFWndEx::PreInitialize() {
	RFWnd::PreInitialize();
}

// 0x41da40
void RFWndEx::LoadResource() {
	m_brush = CreateBrush(m_brushDesc);
}

// 0x41da60
void RFWndEx::OnDestroy() {
	if (m_brush) {
		delete m_brush;
		m_brush = 0;
	}
}

// 0x41da90 (the original inlines RFWnd::GetAlpha 0x4075c0 and GetScreenPos
// 0x406670 and realigns its frame, LTCG)
void RFWndEx::OnDraw() {
	RFRect screenRect = ClientToScreen();
	RFRect clip;
	clip.IntersectRect(m_rc.m_clipRect, screenRect);
	if (m_rc.m_clip) {
		RFManager::s_renderer->PushClipRect(clip);
	}
	float depth = m_rc.m_depth;
	float alpha = GetAlpha(false);
	RFVector2 pos = GetScreenPos();
	m_brush->Draw(pos, GetSize(), alpha, depth);
	RFRect finalScreenRect = ClientToScreen();
	RFRect screenClip;
	screenClip.IntersectRect(m_rc.m_clipRect, finalScreenRect);
	m_rc.m_clipRect = screenClip;
	if (m_rc.m_clip) {
		RFManager::s_renderer->PopClipRect();
	}
}

