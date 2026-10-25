// Original: client\Refresh\src\RFListSC.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; name from the assert paths, docs/JP_REFRESH_UI.md)
#include <new>

#include "RFEventArgs.h"
#include "RFListSC.h"
#include "RFManager.h"
#include "RFScrollBar.h"
#include "RFWindowFactoryManager.h"

// lib: 0x415620 (??_ERFListSC, paired), 0x433110 (??_GRFListSCFactory, paired;
// emitted only by a TU that constructs the factory, none here)

// RFListSC is an RFList with an optional horizontal and vertical scroll bar,
// created from the ListSCBase creation struct as sub-controls. The bars move
// the list's scroll offset (OnH/VScrollPosChanged) and are enabled only while
// the items overflow the padded client area (UpdateScrollbar). m_scrollSize is
// the wheel and arrow step; PAGE selects the item width or height instead.
//
// The two `assert(factory)` in Create must stay on the original's lines 45
// and 58 (their __LINE__ is pushed to _wassert): keep this file head's line
// count when editing above Create.

std::wstring const RFListSC::TypeName = L"RFListSC"; // the static at 0xe47054

uint32_t const RFListSC::PAGE; // 0xd64cc8

// 0x450080
RFListSC::RFListSC() {
}

// 0x4500a0
RFListSC::~RFListSC() {
}

// 0x4500b0
void RFListSC::Create(WndBaseStruct* wndStruct, RFWnd* parent) {
	RFList::Create(wndStruct, parent);
	m_hScrollBar = 0;
	m_vScrollBar = 0;
	ListSCBase* data = static_cast<ListSCBase*>(wndStruct);
	if (data->m_hScrollBar) {
		std::wstring const& name = RFManager::GetInstance().GetCompatibleControlName(data->m_hScrollBar->GetType());
		RFWindowFactory* factory = RFWindowFactoryManager::GetInstance().GetFactory(name);
		assert(factory);

		RFWnd* wnd = factory->Create();
		wnd->Create(data->m_hScrollBar, this);
		wnd->Initialize();
		wnd->SetSubControl(true);
		m_hScrollBar = static_cast<RFScrollBar*>(wnd);
		m_hScrollBar->EnableSmoothScroll(true);
	}

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

// 0x450270 (RFList::PreInitialize, RFWnd::PreInitialize's inline body, and
// RFWnd::GetName are inlined across units by LTCG)
void RFListSC::PreInitialize() {
	RFList::PreInitialize();
	m_scrollSize = PAGE;
	if (m_hScrollBar) {
		m_hScrollBar->SetScrollRange(0, 0);
		LockScrollbar(RFSCROLLBAR_HORIZONTAL, true);
		RegisterTo(m_hScrollBar->GetName(), RFScrollBar::EventPosChanged,
			Subscriber(&RFListSC::OnHScrollPosChanged, this));
		RegisterTo(m_hScrollBar->GetName(), RFWnd::EventDraw,
			Subscriber(&RFListSC::OnScrollDraw, this));
	}
	if (m_vScrollBar) {
		m_vScrollBar->SetScrollRange(0, 0);
		LockScrollbar(RFSCROLLBAR_VERTICAL, true);
		RegisterTo(m_vScrollBar->GetName(), RFScrollBar::EventPosChanged,
			Subscriber(&RFListSC::OnVScrollPosChanged, this));
		RegisterTo(m_vScrollBar->GetName(), RFWnd::EventDraw,
			Subscriber(&RFListSC::OnScrollDraw, this));
	}
}

// 0x4505a0
bool RFListSC::HitTest(RFVector2 const& point, bool force) {
	if (m_visible && m_hitTestVisible && (force || m_enabled)) {
		if (m_vScrollBar && m_vScrollBar->HitTest(point, force)) {
			return true;
		}
		if (m_hScrollBar && m_hScrollBar->HitTest(point, force)) {
			return true;
		}
		RFRect rect = ClientToScreen();
		if (rect.PtInRect(point)) {
			return true;
		}
	}
	return false;
}

// 0x450640
void RFListSC::LockScrollbar(RFSCROLLBAR_TYPE type, bool lock) {
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

// 0x4506d0
bool RFListSC::IsScrollBarAvailable(RFSCROLLBAR_TYPE type) const {
	RFRect rect(0.0f, 0.0f, GetSize().width, GetSize().height);
	rect.ApplyPadding(m_padding);
	switch (type) {
	case RFSCROLLBAR_HORIZONTAL:
		if (m_hScrollBar && rect.Width() < GetAllItemsWidth()) {
			return true;
		}
		break;
	case RFSCROLLBAR_VERTICAL:
		if (m_vScrollBar && rect.Height() < GetAllItemsHeight()) {
			return true;
		}
		break;
	default:
		break;
	}
	return false;
}

// 0x4507b0
void RFListSC::ScrollToTop() {
	RFScrollBar* vBar = m_vScrollBar;
	if (vBar) {
		vBar->SetScrollPos(vBar->GetScrollRange().x);
	}
	RFScrollBar* hBar = m_hScrollBar;
	if (hBar) {
		hBar->SetScrollPos(hBar->GetScrollRange().x);
	}
}

// 0x4507f0
void RFListSC::ScrollToBottom() {
	RFScrollBar* vBar = m_vScrollBar;
	if (vBar) {
		vBar->SetScrollPos(vBar->GetScrollRange().y);
	}
	RFScrollBar* hBar = m_hScrollBar;
	if (hBar) {
		hBar->SetScrollPos(hBar->GetScrollRange().y);
	}
}

// 0x450830
void RFListSC::ScrollToItem(int32_t index) {
	if (m_items.empty() || index < 0 || index >= GetItemCount()) {
		return;
	}
	RFRect rect = GetScreenRect();
	rect.left += m_padding.left;
	rect.top += m_padding.top;
	if (index < GetItemCount()) {
		if (IsScrollBarAvailable(RFSCROLLBAR_VERTICAL)) {
			m_vScrollBar->SetScrollPos((int32_t)(m_items[index]->GetPos().y - rect.top - m_scrollOffset.y));
		}
		if (IsScrollBarAvailable(RFSCROLLBAR_HORIZONTAL)) {
			m_hScrollBar->SetScrollPos((int32_t)(m_items[index]->GetPos().x - rect.left - m_scrollOffset.x));
		}
	}
}

// 0x450930
void RFListSC::UpdateScrollbar() {
	RFRect rect(0.0f, 0.0f, GetSize().width, GetSize().height);
	rect.ApplyPadding(m_padding);
	if (m_vScrollBar) {
		if (IsScrollBarAvailable(RFSCROLLBAR_VERTICAL)) {
			LockScrollbar(RFSCROLLBAR_VERTICAL, false);
			m_vScrollBar->SetScrollRange(0, (int32_t)GetAllItemsHeight() - (int32_t)rect.Height());
			m_vScrollBar->SetScrollSize((int32_t)GetScrollSize());
		} else {
			LockScrollbar(RFSCROLLBAR_VERTICAL, true);
			m_vScrollBar->SetScrollPos(0);
			m_scrollOffset.y = 0.0f;
		}
	}
	if (m_hScrollBar) {
		if (IsScrollBarAvailable(RFSCROLLBAR_HORIZONTAL)) {
			LockScrollbar(RFSCROLLBAR_HORIZONTAL, false);
			m_hScrollBar->SetScrollRange(0, (int32_t)GetAllItemsWidth() - (int32_t)rect.Width());
			m_hScrollBar->SetScrollSize((int32_t)GetScrollSize());
		} else {
			LockScrollbar(RFSCROLLBAR_HORIZONTAL, true);
			m_hScrollBar->SetScrollPos(0);
			m_scrollOffset.x = 0.0f;
		}
	}
}

// 0x450b20
void RFListSC::OnItemAdded() {
	UpdateItemPosition();
	UpdateScrollbar();
}

// 0x450b30
void RFListSC::OnItemRemoved() {
	UpdateItemPosition();
	UpdateScrollbar();
}

// 0x450b40
void RFListSC::OnUpdate(UpdateEventArgs& e) {
	RFWnd::OnUpdate(e);
	UpdateItemPosition();
	UpdateScrollbar();
}

// 0x450b70
void RFListSC::SetScrollSize(uint32_t size) {
	m_scrollSize = size;
}

// 0x450b80
float RFListSC::GetScrollSize() {
	float size = 0.0f;
	if (m_scrollSize == PAGE) {
		RFRect rect(0.0f, 0.0f, GetSize().width, GetSize().height);
		if (m_orientation == 0) {
			if (m_vScrollBar && IsScrollBarAvailable(RFSCROLLBAR_VERTICAL)) {
				size = m_itemSize.height;
			}
		} else {
			if (m_hScrollBar && IsScrollBarAvailable(RFSCROLLBAR_HORIZONTAL)) {
				size = m_itemSize.width;
			}
		}
	} else {
		size = (float)m_scrollSize;
	}
	return size;
}

// 0x450c30
void RFListSC::OnMouseWheel(MouseEventArgs& args) {
	RFWnd::OnMouseWheel(args);
	if (m_orientation == 0) {
		if (m_vScrollBar && IsScrollBarAvailable(RFSCROLLBAR_VERTICAL)) {
			float step = GetScrollSize();
			int32_t pos = m_vScrollBar->GetScrollPos();
			RFPoint range = m_vScrollBar->GetScrollRange();
			if (args.wheelDelta < 0.0f) {
				if (pos < range.y) {
					m_vScrollBar->MoveToScroll((int32_t)step + pos);
					args.handled = true;
				}
			} else if (pos > range.x) {
				m_vScrollBar->MoveToScroll(pos - (int32_t)step);
				args.handled = true;
			}
		}
	} else {
		if (m_hScrollBar && IsScrollBarAvailable(RFSCROLLBAR_HORIZONTAL)) {
			float step = GetScrollSize();
			int32_t pos = m_hScrollBar->GetScrollPos();
			RFPoint range = m_hScrollBar->GetScrollRange();
			if (args.wheelDelta < 0.0f) {
				if (pos < range.y) {
					m_hScrollBar->MoveToScroll((int32_t)step + pos);
					args.handled = true;
				}
			} else if (pos > range.x) {
				m_hScrollBar->MoveToScroll(pos - (int32_t)step);
				args.handled = true;
			}
		}
	}
}

// 0x450da0
void RFListSC::OnHScrollPosChanged(EventArgs& args) {
	if (m_hScrollBar && IsScrollBarAvailable(RFSCROLLBAR_HORIZONTAL) && !m_items.empty()) {
		m_scrollOffset.x = -(float)static_cast<ScrollBarPosChangedEventArgs&>(args).pos;
	}
}

// 0x450de0
void RFListSC::OnVScrollPosChanged(EventArgs& args) {
	if (m_vScrollBar && IsScrollBarAvailable(RFSCROLLBAR_VERTICAL) && !m_items.empty()) {
		m_scrollOffset.y = -(float)static_cast<ScrollBarPosChangedEventArgs&>(args).pos;
	}
}

// 0x450e20 (RFWnd::GetRC is inlined across units by LTCG)
void RFListSC::OnScrollDraw(EventArgs& args) {
	WindowEventArgs_<RFWnd>& e = static_cast<WindowEventArgs_<RFWnd>&>(args);
	e.sender->GetRC()->m_clipRect = m_super->GetRC()->m_clipRect;
	e.handled = true;
}


// lib: 0x4155f0 (??_GRFListSC, paired), 0x450e70 (the Subscriber template
// constructor for RFListSC, paired; this in esi), 0x450ec0, 0x450ee0,
// 0x450f00, 0x450f10, 0x450f20, 0x450f30, 0x450f60
// (MemberFunctionFunctor<RFListSC>'s constructor, operator(), AddRef,
// Release, GetRefCount, scalar deleting destructor and destructor, paired;
// the constructors and the destructor take this in eax: LTCG)
