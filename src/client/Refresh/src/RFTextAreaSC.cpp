// Original: client\Refresh\src\RFTextAreaSC.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include "RFTextAreaSC.h"

#include <new>

#include "RFCaret.h"
#include "RFEventArgs.h"
#include "RFEventHandler.h"
#include "RFManager.h"
#include "RFScrollBar.h"
#include "RFText.h"
#include "RFWindowFactoryManager.h"

// lib: the implicit copy constructor 0x415d10 and operator= 0x415dc0, the scalar deleting destructors of
// RFTextAreaSCFactory 0x433990 and (unreferenced, paired) RFTextAreaSC 0x415e60; header
// templates instantiated here, paired: 0x44e500 (the Subscriber template
// constructor for RFTextAreaSC; this in esi), 0x44e550, 0x44e570, 0x44e590,
// 0x44e5a0, 0x44e5b0, 0x44e5c0, 0x44e5f0 (MemberFunctionFunctor<RFTextAreaSC>'s
// constructor, operator(), AddRef, Release, GetRefCount, scalar deleting
// destructor and destructor; the constructor and the destructor take this in
// eax: LTCG)

// The class name, the static const at 0xe46fe4.
std::wstring const RFTextAreaSC::TypeName = L"TextArea";

// 0x44d6a0
RFTextAreaSC::RFTextAreaSC() {
	m_hScrollBar = 0;
	m_vScrollBar = 0;
}

// 0x44d6e0
RFTextAreaSC::~RFTextAreaSC() {
}

// 0x44d6f0
void RFTextAreaSC::Create(WndBaseStruct* wndStruct, RFWnd* parent) {
	RFTextArea::Create(wndStruct, parent);
	m_hScrollBar = 0;
	m_vScrollBar = 0;
	TextAreaSCBase* data = static_cast<TextAreaSCBase*>(wndStruct);
	m_useHScrollBar = data->m_useHScrollBar;
	m_enableVScrollBar = data->m_enableVScrollBar;
	m_unusedScrollFlag = data->m_unusedScrollFlag;
	m_autoShowVScrollBar = data->m_autoShowVScrollBar;
	m_scrollBarRect = data->m_scrollBarRect;

	// The horizontal bar: a control of its compatible type, from its factory,
	// created, initialized and marked as a sub-control of this text area (the
	// original's assert line 0x37 sits two lines below the factory lookup).
	if (data->m_hScrollBar) {
		std::wstring const& name = RFManager::GetInstance().GetCompatibleControlName(data->m_hScrollBar->GetType());
		RFWindowFactory* factory = RFWindowFactoryManager::GetInstance().GetFactory(name);
		assert(factory);
		RFWnd* wnd = factory->Create();
		wnd->Create(data->m_hScrollBar, this);
		wnd->Initialize();
		wnd->SetSubControl(true);
		m_hScrollBar = static_cast<RFScrollBar*>(wnd);
	}

	// The vertical bar, created the same way; it alone scrolls smoothly
	// (the assert lines, 0x37 and 0x44, place the two blocks).
	if (data->m_vScrollBar) {
		std::wstring const& name = RFManager::GetInstance().GetCompatibleControlName(data->m_vScrollBar->GetType());
		RFWindowFactory* factory = RFWindowFactoryManager::GetInstance().GetFactory(name);
		assert(factory);
		RFWnd* wnd = factory->Create();
		wnd->Create(data->m_vScrollBar, this);
		wnd->Initialize();
		wnd->SetSubControl(true);
		m_vScrollBar = static_cast<RFScrollBar*>(wnd);
		m_vScrollBar->EnableSmoothScroll(true);
	}
}

// 0x44d910
void RFTextAreaSC::PreInitialize() {
	RFTextArea::PreInitialize();
	m_stickToBottom = true;
	if (m_hScrollBar) {
		if (!m_useHScrollBar) {
			m_hScrollBar->ShowWindow(false);
		}
		RegisterTo(m_hScrollBar->GetName(), RFScrollBar::EventPosChanged,
			Subscriber(&RFTextAreaSC::OnHScrollPosChanged, this));
		RegisterTo(m_hScrollBar->GetName(), RFWnd::EventDraw,
			Subscriber(&RFTextAreaSC::OnScrollDraw, this));
	}
	if (m_vScrollBar) {
		if (!m_enableVScrollBar) {
			if (m_autoShowVScrollBar == true) {
				if (!IsScrollBarAvailable(RFSCROLLBAR_VERTICAL)) {
					m_vScrollBar->ShowWindow(false);
				}
			} else {
				m_vScrollBar->ShowWindow(false);
			}
		} else if (!IsScrollBarAvailable(RFSCROLLBAR_VERTICAL)) {
			LockScrollbar(RFSCROLLBAR_VERTICAL, true);
		}
		RegisterTo(m_vScrollBar->GetName(), RFScrollBar::EventPosChanged,
			Subscriber(&RFTextAreaSC::OnVScrollPosChanged, this));
		RegisterTo(m_vScrollBar->GetName(), RFWnd::EventDraw,
			Subscriber(&RFTextAreaSC::OnScrollDraw, this));
	}
}

// 0x44dc10
bool RFTextAreaSC::HitTest(RFVector2 const& pos, bool hitTestThrough) {
	if (m_visible && m_hitTestVisible && (hitTestThrough || m_enabled)) {
		if (m_vScrollBar && m_vScrollBar->HitTest(pos, hitTestThrough) == true) {
			return true;
		}
		RFRect rect = ClientToScreen();
		if (rect.PtInRect(pos)) {
			return true;
		}
		return false;
	}
	return false;
}

// 0x44dc90
void RFTextAreaSC::OnLayoutChanged(uint32_t flags) {
	RFTextArea::OnLayoutChanged(flags);
	if ((flags & 2) || (flags & 0x10000000)) {
		UpdateScrollbar();
	}
}

// 0x44dcc0
void RFTextAreaSC::OnChar(CharEventArgs& e) {
	RFTextArea::OnChar(e);
	StickToBottomProcess();
}

// 0x44dce0
void RFTextAreaSC::OnKeyDown(KeyEventArgs& e) {
	RFTextArea::OnKeyDown(e);
	if (m_showCaret) {
		StickToBottomProcess();
		if (m_vScrollBar) {
			int32_t pos = m_vScrollBar->GetScrollPos();
			int32_t caretY = m_text->m_currentParagraph->m_caretY;
			RFRect rect = ClientToScreen();
			rect.top += m_padding.top;
			rect.bottom -= m_padding.bottom;
			float y = static_cast<float>(caretY);
			if (y < rect.top) {
				pos += static_cast<int32_t>(y - rect.top);
			} else if (rect.bottom <= y) {
				pos += static_cast<int32_t>(y - rect.bottom + m_lineHeight);
			}
			m_vScrollBar->SetScrollPos(pos);
		}
	}
}

// 0x44dde0
void RFTextAreaSC::OnUpdate(UpdateEventArgs& e) {
	RFWnd::OnUpdate(e);
	if (m_caret) {
		m_caret->Update(e.elapsed);
	}
}

// 0x44de30
void RFTextAreaSC::OnMouseWheel(MouseEventArgs& e) {
	if (m_vScrollBar && IsScrollBarAvailable(RFSCROLLBAR_VERTICAL)) {
		RFTextArea::OnMouseWheel(e);
		if (m_vScrollBar && IsScrollBarAvailable(RFSCROLLBAR_VERTICAL)) {
			float step = m_lineHeight * 2.0f;
			int32_t pos = m_vScrollBar->GetScrollPos();
			RFPoint const& range = m_vScrollBar->GetScrollRange();
			if (e.wheelDelta < 0.0f) {
				if (pos < range.y) {
					m_vScrollBar->MoveToScroll(pos + static_cast<int32_t>(step));
					e.handled = true;
				}
			} else if (pos > range.x) {
				m_vScrollBar->MoveToScroll(pos - static_cast<int32_t>(step));
				e.handled = true;
			}
		}
	}
}

// 0x44df00
void RFTextAreaSC::SetText(std::wstring const& text) {
	ClearText();
	AddText(text);
	StickToBottomProcess();
}

// 0x44df30
void RFTextAreaSC::AddText(std::wstring const& text) {
	RFTextArea::AddText(text);
	StickToBottomProcess();
}

// 0x44df70
void RFTextAreaSC::AddPlainText(std::wstring const& text) {
	RFTextArea::AddPlainText(text);
	StickToBottomProcess();
}

// 0x44dfb0
void RFTextAreaSC::ClearText() {
	RFTextArea::ClearText();
	UpdateScrollbar();
}

// 0x44dfe0
void RFTextAreaSC::LockScrollbar(RFSCROLLBAR_TYPE type, bool lock) {
	switch (type) {
	case RFSCROLLBAR_HORIZONTAL:
		if (m_hScrollBar) {
			m_hScrollBar->EnableWindow(!lock);
		}
		break;
	case RFSCROLLBAR_VERTICAL:
		if (m_vScrollBar) {
			m_vScrollBar->EnableWindow(!lock);
		}
		break;
	case RFSCROLLBAR_BOTH:
		if (m_hScrollBar) {
			m_hScrollBar->EnableWindow(!lock);
		}
		if (m_vScrollBar) {
			m_vScrollBar->EnableWindow(!lock);
		}
		break;
	}
}

// 0x44e070
bool RFTextAreaSC::IsScrollBarAvailable(RFSCROLLBAR_TYPE type) {
	if (type == RFSCROLLBAR_VERTICAL) {
		RFRect rect = ClientToScreen();
		rect.top += m_padding.top;
		rect.bottom -= m_padding.bottom;
		float height = rect.Height();
		if (height < static_cast<float>(m_text->GetTotalHeight())) {
			return true;
		}
	}
	return false;
}

// 0x44e130
void RFTextAreaSC::ScrollToTop() {
	RFScrollBar* scrollBar = m_vScrollBar;
	if (scrollBar) {
		scrollBar->SetScrollPos(scrollBar->GetScrollRange().x);
	}
}

// 0x44e150
void RFTextAreaSC::ScrollToBottom() {
	RFScrollBar* scrollBar = m_vScrollBar;
	if (scrollBar) {
		scrollBar->SetScrollPos(scrollBar->GetScrollRange().y);
	}
}

// 0x44e170
void RFTextAreaSC::StickToBottomProcess() {
	if (m_stickToBottom && m_vScrollBar) {
		bool atBottom = false;
		if (!IsScrollBarAvailable(RFSCROLLBAR_VERTICAL) || m_vScrollBar->IsBottom()) {
			atBottom = true;
		}
		UpdateScrollbar();
		if (atBottom) {
			m_vScrollBar->SetScrollPos(m_vScrollBar->GetScrollRange().y);
		} else if (m_text->GetLineLimit() <= static_cast<uint32_t>(m_text->GetNumOfLine())) {
			m_vScrollBar->SetScrollPos(m_vScrollBar->GetScrollPos() - m_lineHeight);
		}
	} else {
		UpdateScrollbar();
	}
}

// 0x44e220
// Shows (or enables) the vertical bar when the text is taller than the
// area, hides (or disables) it otherwise, and sets its range to the text
// height left over.
void RFTextAreaSC::UpdateScrollbar() {
	if (!m_vScrollBar) {
		return;
	}
	if (IsScrollBarAvailable(RFSCROLLBAR_VERTICAL) == true) {
		if (m_enableVScrollBar == true) {
			LockScrollbar(RFSCROLLBAR_VERTICAL, false);
		} else if (m_autoShowVScrollBar == true && !m_vScrollBar->IsWindowVisible(false)) {
			SetRect(GetRect() + m_scrollBarRect);
			m_vScrollBar->ShowWindow(true);
		}
		RFRect rect = ClientToScreen();
		rect.top += m_padding.top;
		rect.bottom -= m_padding.bottom;
		m_vScrollBar->SetScrollRange(0, m_text->GetTotalHeight() - static_cast<int32_t>(rect.Height()));
		m_vScrollBar->SetScrollSize(m_lineHeight * 2);
		return;
	}
	if (m_enableVScrollBar == true) {
		LockScrollbar(RFSCROLLBAR_VERTICAL, true);
	} else if (m_autoShowVScrollBar == true && m_vScrollBar->IsWindowVisible(false)) {
		SetRect(GetRect() - m_scrollBarRect);
		m_vScrollBar->ShowWindow(false);
	}
	m_vScrollBar->SetScrollPos(0);
	m_scrollY = 0;
}

// 0x44e460
void RFTextAreaSC::OnHScrollPosChanged(EventArgs&) {
}

// 0x44e470
void RFTextAreaSC::OnVScrollPosChanged(EventArgs& e) {
	if (m_vScrollBar && IsScrollBarAvailable(RFSCROLLBAR_VERTICAL)) {
		m_scrollY = -static_cast<ScrollBarPosChangedEventArgs&>(e).pos;
		Invaildate();
	}
}

// 0x44e4b0
// A scroll bar draws clipped to the text area's parent (the original
// inlines RFWnd::GetRC 0x401fe0 across units).
void RFTextAreaSC::OnScrollDraw(EventArgs& e) {
	WindowEventArgs_<RFWnd>& args = static_cast<WindowEventArgs_<RFWnd>&>(e);
	args.sender->GetRC()->m_clipRect = m_super->GetRC()->m_clipRect;
	args.handled = true;
}
