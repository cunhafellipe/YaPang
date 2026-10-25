// Original: client\Refresh\src\RFCheck.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include <new>

#include "RFCheck.h"

#include "RFEventArgs.h"
#include "RFManager.h"
#include "RFRenderer.h"

// lib: 0x414c50 (vector deleting destructor ??_E, vtable slot 1), 0x414bf0
// (scalar deleting destructor); event args, paired: 0x425a30 / 0x425a60
// (CheckChangedEventArgs scalar deleting destructor / destructor), 0x425f10 /
// 0x425a70 / 0x425f20 (WindowEventArgs_<RFCheck> constructor / destructor /
// scalar deleting destructor)

// The event raised when the check state changes, the static const at 0xe46d98.
std::wstring const RFCheck::EventChanged = L"Changed";

// The class name, the static const at 0xe46db4.
std::wstring const RFCheck::TypeName = L"CheckBox";

// 0x425a80
RFCheck::RFCheck() {
}

// 0x425ab0
RFCheck::~RFCheck() {
}

// 0x425b00
void RFCheck::Create(WndBaseStruct* wndStruct, RFWnd* parent) {
	RFWnd::Create(wndStruct, parent);
	CheckBase* checkStruct = static_cast<CheckBase*>(wndStruct);
	m_brushDescs[0] = checkStruct->m_brushDescs[0];
	m_brushDescs[1] = checkStruct->m_brushDescs[1];
	m_clickSound = checkStruct->m_clickSound.GetValue();
}

// 0x425b50
void RFCheck::PreInitialize() {
	RFWnd::PreInitialize();
	for (int32_t i = 0; i < 2; ++i) {
		m_brushes[i] = 0;
	}
	m_check = false;
}

// 0x425bc0
void RFCheck::LoadResource() {
	m_brushes[0] = CreateBrush(m_brushDescs[0]);
	m_brushes[1] = CreateBrush(m_brushDescs[1]);
}

// 0x425bf0
void RFCheck::OnDestroy() {
	for (int32_t i = 0; i < 2; ++i) {
		if (m_brushes[i] != 0) {
			delete m_brushes[i];
			m_brushes[i] = 0;
		}
	}
}

// 0x425c20
bool RFCheck::GetCheck() const {
	return m_check;
}

// 0x425c30
void RFCheck::SetCheck(bool check) {
	m_check = check;
}

// 0x425c40
void RFCheck::OnLButtonDown(MouseEventArgs& args) {
	// RFManager::GetInstance 0x417450 (src\RFManager.cpp, assert line 58) in
	// the original, inlined across units by LTCG.
	RFManager::GetInstance().SetCapturedWindow(this);
	RFWnd::OnLButtonDown(args);
	args.handled = true;
}

// 0x425c90
void RFCheck::OnLButtonUp(MouseEventArgs& args) {
	// RFManager::GetInstance 0x417450 inlined by LTCG, as in OnLButtonDown.
	RFManager::GetInstance().SetCapturedWindow(0);
	bool wasDown = m_lButtonDown;
	RFWnd::OnLButtonUp(args);
	PlaySfx(m_clickSound);
	args.handled = true;
	if (wasDown && HitTest(args.pos, false)) {
		m_check = !m_check;
		CheckChangedEventArgs changed(this, m_check);
		SendEvent(EventChanged, changed);
	}
}

// 0x425d80
void RFCheck::OnDraw() {
	RFRect clip;
	clip.IntersectRect(m_rc.m_clipRect, ClientToScreen());
	if (m_rc.m_clip) {
		RFManager::s_renderer->PushClipRect(clip);
	}
	float depth = m_rc.m_depth;
	// RFWnd::GetAlpha and GetScreenPos are inlined by LTCG in the original.
	float alpha = GetAlpha(false);
	RFVector2 pos = GetScreenPos();
	m_brushes[m_check ? 1 : 0]->Draw(pos, GetSize(), alpha, depth);
	RFRect screenClip;
	screenClip.IntersectRect(m_rc.m_clipRect, ClientToScreen());
	m_rc.m_clipRect = screenClip;
	if (m_rc.m_clip) {
		RFManager::s_renderer->PopClipRect();
	}
}

