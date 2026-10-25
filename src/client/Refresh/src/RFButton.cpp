// Original: client\Refresh\src\RFButton.cpp (JP 983 exe, unit bounds in
// jp/units.tsv; the file name is assumed from the class)
#include <new>

#include "RFButton.h"
#include "RFEventArgs.h"
#include "RFInterface.h"
#include "RFManager.h"
#include "RFRenderer.h"

#include <windows.h>

// lib: 0x402ac0, 0x402c20 (the implicit copy constructor and operator=),
// 0x402d40 (vector deleting destructor ??_E), 0x402d10 (scalar deleting
// destructor ??_G: not emitted by this TU, unpaired); header templates
// instantiated here, paired: 0x421680 / 0x421b70 (WindowEventArgs_<RFButton>
// destructor / constructor), 0x421c20, 0x421c40, 0x421c50, 0x421c60, 0x421c70
// (MemberFunctionFunctor<RFButton> operator(), AddRef, Release, GetRefCount,
// scalar deleting destructor), 0x421c00 (MemberFunctionFunctor<RFButton>'s
// constructor, unreferenced LTCG copy: this in eax, the object in ecx, the
// function DefaultButtonEventHandler propagated; paired)

// 0xe46cb8
std::wstring const RFButton::EventClicked = L"Clicked";
// The class name, the static const at 0xe46cd4.
std::wstring const RFButton::TypeName = L"Button";

// 0x420e10
RFButton::RFButton() {
}

// 0x420e70; scalar deleting destructor 0x402d40
RFButton::~RFButton() {
}

// 0x420f30
void RFButton::Create(WndBaseStruct* wndStruct, RFWnd* parent) {
	RFWnd::Create(wndStruct, parent);
	ButtonBase* buttonStruct = static_cast<ButtonBase*>(wndStruct);
	m_stateImages[RFBUTTON_STATE_NORMAL] = buttonStruct->m_stateImages[RFBUTTON_STATE_NORMAL];
	m_stateImages[RFBUTTON_STATE_DOWN] = buttonStruct->m_stateImages[RFBUTTON_STATE_DOWN];
	m_stateImages[RFBUTTON_STATE_OVER] = buttonStruct->m_stateImages[RFBUTTON_STATE_OVER];
	m_stateImages[RFBUTTON_STATE_DISABLE] = buttonStruct->m_stateImages[RFBUTTON_STATE_DISABLE];
	m_pushSound = buttonStruct->m_pushSound;
	m_overSound = buttonStruct->m_overSound;
	m_releaseSound = buttonStruct->m_releaseSound;
	m_defaultHandler = buttonStruct->m_defaultHandler.GetValue();
}

// 0x420fd0 (the split cuts it at 224 bytes: its tail 0x4210b0-0x4211bd is
// the inventory's sub_421094. RFWnd::RegisterTo 0x406430, with GetOverlapped,
// GetWindow_'s name test and RFEventHandler::SubscribeEvent, is inlined by
// LTCG in the original)
void RFButton::PreInitialize() {
	RFWnd::PreInitialize();
	m_state = RFBUTTON_STATE_NORMAL;
	m_pushed = false;
	m_mouseOver = false;
	for (int32_t i = 0; i < 4; ++i) {
		m_stateBrushes[i] = 0;
	}
	if (m_defaultHandler) {
		RegisterTo(L"", RFWnd::EventKeyUp, Subscriber(&RFButton::DefaultButtonEventHandler, this));
	}
}

// 0x4211c0
void RFButton::LoadResource() {
	m_stateBrushes[RFBUTTON_STATE_NORMAL] = CreateBrush(m_stateImages[RFBUTTON_STATE_NORMAL]);
	m_stateBrushes[RFBUTTON_STATE_DOWN] = CreateBrush(m_stateImages[RFBUTTON_STATE_DOWN]);
	m_stateBrushes[RFBUTTON_STATE_OVER] = CreateBrush(m_stateImages[RFBUTTON_STATE_OVER]);
	m_stateBrushes[RFBUTTON_STATE_DISABLE] = CreateBrush(m_stateImages[RFBUTTON_STATE_DISABLE]);
}

// 0x421210
void RFButton::OnDestroy() {
	for (int32_t i = 0; i < 4; ++i) {
		if (m_stateBrushes[i] != 0) {
			delete m_stateBrushes[i];
			m_stateBrushes[i] = 0;
		}
	}
}

// 0x421240
void RFButton::ButtonReset() {
	m_lButtonDown = false;
	m_state = RFBUTTON_STATE_NORMAL;
	m_pushed = false;
}

// 0x421260
void RFButton::OnDraw() {
	RFRect screenRect = ClientToScreen();
	RFRect clip;
	clip.IntersectRect(m_rc.m_clipRect, screenRect);
	if (m_rc.m_clip) {
		RFManager::s_renderer->PushClipRect(clip);
	}
	RFBUTTON_STATE state = IsWindowEnabled(false) ? m_state : RFBUTTON_STATE_DISABLE;
	float depth = m_rc.m_depth;
	// RFWnd::IsWindowEnabled, GetAlpha and GetScreenPos are inlined by LTCG
	// in the original.
	float alpha = GetAlpha(false);
	RFVector2 pos = GetScreenPos();
	m_stateBrushes[state]->Draw(pos, GetSize(), alpha, depth);
	RFRect finalScreenRect = ClientToScreen();
	RFRect screenClip;
	screenClip.IntersectRect(m_rc.m_clipRect, finalScreenRect);
	m_rc.m_clipRect = screenClip;
	if (m_rc.m_clip) {
		RFManager::s_renderer->PopClipRect();
	}
}

// 0x421420
void RFButton::OnEnabled(bool enabled) {
	m_state = enabled ? RFBUTTON_STATE_NORMAL : RFBUTTON_STATE_DISABLE;
}

// 0x421440 (RFWnd::OnShow is inlined by LTCG in the original)
void RFButton::OnShow(bool show) {
	RFWnd::OnShow(show);
	if (!show) {
		m_lButtonDown = false;
	}
}

// 0x4214d0
void RFButton::OnLButtonDown(MouseEventArgs& args) {
	RFManager::GetInstance().SetCapturedWindow(this);
	RFWnd::OnLButtonDown(args);
	PlaySfx(m_pushSound);
	m_state = RFBUTTON_STATE_DOWN;
	RFManager::s_interface->OnButtonStateChanged(this, 2);
	args.handled = true;
}

// 0x421540 (RFManager::GetInstance and SetCapturedWindow, RFWnd::SendEvent
// are inlined by LTCG in the original)
void RFButton::OnLButtonUp(MouseEventArgs& args) {
	RFManager::GetInstance().SetCapturedWindow(0);
	bool wasDown = m_lButtonDown;
	RFWnd::OnLButtonUp(args);
	args.handled = true;
	if (wasDown) {
		if (!HitTest(args.pos, false)) {
			RFManager::s_interface->OnButtonStateChanged(this, 0);
			if (!m_pushed) {
				m_state = RFBUTTON_STATE_NORMAL;
			}
		} else {
			if (!m_pushed) {
				m_state = RFBUTTON_STATE_OVER;
			}
			RFManager::s_interface->OnButtonStateChanged(this, 1);
			PlaySfx(m_releaseSound);
			WindowEventArgs_<RFButton> clicked(this);
			SendEvent(EventClicked, clicked);
		}
	}
}

// 0x421690 (RFWnd::OnMouseEnter is inlined by LTCG in the original)
void RFButton::OnMouseEnter() {
	MouseEnter_Impl();
	RFWnd::OnMouseEnter();
}

// 0x421710 (MouseLeave_Impl is inlined, RFWnd::OnMouseLeave by LTCG, in the
// original)
void RFButton::OnMouseLeave() {
	MouseLeave_Impl();
	RFWnd::OnMouseLeave();
}

// 0x4217c0
void RFButton::MouseEnter_Impl() {
	m_mouseOver = true;
	if (!m_pushed) {
		m_state = RFBUTTON_STATE_OVER;
	}
	if (m_lButtonDown) {
		m_state = RFBUTTON_STATE_DOWN;
	} else {
		int32_t cursor = 1;
		if (m_state == RFBUTTON_STATE_DISABLE) {
			cursor = 3;
		}
		RFManager::s_interface->OnButtonStateChanged(this, cursor);
		PlaySfx(m_overSound);
	}
}

// 0x421820
void RFButton::MouseLeave_Impl() {
	m_mouseOver = false;
	if (!m_lButtonDown) {
		RFManager::s_interface->OnButtonStateChanged(this, 0);
	}
	if (!m_pushed && m_enabled) {
		if (m_super == 0 || m_super->IsWindowEnabled(false)) {
			m_state = RFBUTTON_STATE_NORMAL;
		}
	}
}

// 0x421870 (RFEventHandler::SendEvent is inlined by LTCG in the original)
void RFButton::DefaultButtonEventHandler(EventArgs& args) {
	KeyEventArgs& keyArgs = static_cast<KeyEventArgs&>(args);
	if (keyArgs.keyCode == VK_RETURN) {
		args.handled = true;
		WindowEventArgs_<RFButton> clicked(this);
		SendEvent(EventClicked, clicked);
	}
}

// 0x4218f0
void RFButton::ChangeNormalButtonImage(std::wstring fileName) {
	if (m_stateBrushes[RFBUTTON_STATE_NORMAL] != 0) {
		delete m_stateBrushes[RFBUTTON_STATE_NORMAL];
		m_stateBrushes[RFBUTTON_STATE_NORMAL] = 0;
	}
	if (m_stateImages[RFBUTTON_STATE_NORMAL] != 0) {
		static_cast<SingleImageSet*>(m_stateImages[RFBUTTON_STATE_NORMAL])->m_fileName = fileName;
	}
	m_stateBrushes[RFBUTTON_STATE_NORMAL] = CreateBrush(m_stateImages[RFBUTTON_STATE_NORMAL]);
}

// 0x421990
void RFButton::ChangeOverButtonImage(std::wstring fileName) {
	if (m_stateBrushes[RFBUTTON_STATE_OVER] != 0) {
		delete m_stateBrushes[RFBUTTON_STATE_OVER];
		m_stateBrushes[RFBUTTON_STATE_OVER] = 0;
	}
	if (m_stateImages[RFBUTTON_STATE_OVER] != 0) {
		static_cast<SingleImageSet*>(m_stateImages[RFBUTTON_STATE_OVER])->m_fileName = fileName;
	}
	m_stateBrushes[RFBUTTON_STATE_OVER] = CreateBrush(m_stateImages[RFBUTTON_STATE_OVER]);
}

// 0x421a30
void RFButton::ChangeDownButtonImage(std::wstring fileName) {
	if (m_stateBrushes[RFBUTTON_STATE_DOWN] != 0) {
		delete m_stateBrushes[RFBUTTON_STATE_DOWN];
		m_stateBrushes[RFBUTTON_STATE_DOWN] = 0;
	}
	if (m_stateImages[RFBUTTON_STATE_DOWN] != 0) {
		static_cast<SingleImageSet*>(m_stateImages[RFBUTTON_STATE_DOWN])->m_fileName = fileName;
	}
	m_stateBrushes[RFBUTTON_STATE_DOWN] = CreateBrush(m_stateImages[RFBUTTON_STATE_DOWN]);
}

// 0x421ad0
void RFButton::ChangeDisableButtonImage(std::wstring fileName) {
	if (m_stateBrushes[RFBUTTON_STATE_DISABLE] != 0) {
		delete m_stateBrushes[RFBUTTON_STATE_DISABLE];
		m_stateBrushes[RFBUTTON_STATE_DISABLE] = 0;
	}
	if (m_stateImages[RFBUTTON_STATE_DISABLE] != 0) {
		static_cast<SingleImageSet*>(m_stateImages[RFBUTTON_STATE_DISABLE])->m_fileName = fileName;
	}
	m_stateBrushes[RFBUTTON_STATE_DISABLE] = CreateBrush(m_stateImages[RFBUTTON_STATE_DISABLE]);
}

