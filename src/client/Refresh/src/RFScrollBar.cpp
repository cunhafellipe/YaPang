// Original: client\Refresh\src\RFScrollBar.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include <new>

#include "RFScrollBar.h"

#include <stdlib.h>

#include <functional>

#include "RFEventArgs.h"

// lib: 0x415450, 0x4151d0, 0x4152d0, 0x428dd0, 0x42a680, 0x42a690, 0x42a6a0
// The vector deleting destructor ??_ERFScrollBar (paired); the implicit copy
// constructor and operator=; the scalar deleting destructor of
// ScrollBarPosChangedEventArgs; the vcall thunks of slots 35, 36 and 34 (??_9RFScrollBar@@$BIM/$BJA/$BII@AE, the
// member-function pointers PreInitialize subscribes, paired). The scalar
// deleting destructors of WindowEventArgs_<RFScrollBar> 0x42a120 and
// MemberFunctionFunctor<RFScrollBar> 0x42a2a0.
// lib: 0x428e00, 0x428e10, 0x42a030, 0x42a2d0
// The implicit destructors of ScrollBarPosChangedEventArgs 0x428e00 and
// WindowEventArgs_<RFScrollBar> 0x428e10 (this in eax), the closure
// constructor of LevelToThumbPosition's lambda 0x42a030 (this in eax, captures
// in ecx/edx) and ~MemberFunctionFunctor<RFScrollBar> 0x42a2d0 (paired).
// lib: 0x4153c0, 0x42a0c0, 0x42a230
// Unreferenced copies, paired: the scalar deleting destructor ??_GRFScrollBar
// 0x4153c0 (the vtable holds ??_E), the constructors of
// WindowEventArgs_<RFScrollBar> 0x42a0c0 and MemberFunctionFunctor<RFScrollBar>
// 0x42a230 (this in eax, arguments in ecx/edx).
// lib: 0x42a0d0, 0x42a0f0, 0x42a150, 0x42a170, 0x42a1b0, 0x42a210, 0x42a2e0, 0x42a310, 0x42a340, 0x42a360, 0x42a370
// lib: 0x42a420, 0x42a430, 0x42a440, 0x42a470, 0x42a480, 0x42a490, 0x42a4d0, 0x42a500, 0x42a510, 0x42a530, 0x42a540
// lib: 0x42a550, 0x42a560, 0x42a570, 0x42a580, 0x42a590, 0x42a5e0, 0x42a5f0, 0x42a640, 0x42a650, 0x42a660, 0x42a670
// lib: 0x42a300, 0x42a330
// The std::tr1::function<void(float)> machinery of LevelToThumbPosition's
// lambda (_Tidy, operator(), _Local, _Reset, _Impl_no_alloc1 /
// _Impl_base1 members with vtables 0xd6476c / 0xd64738, _Callable_obj,
// the allocator members).

// The event raised when the position changes, the static const at 0xe46e5c.
std::wstring const RFScrollBar::EventPosChanged = L"PosChanged";

// The class name, the static const at 0xe46e78.
std::wstring const RFScrollBar::TypeName = L"ScrollBar";

// 0x415170
wchar_t const* RFScrollBar::ToString() const {
	return L"RFScrollBar";
}

// 0x415180
std::wstring const& RFScrollBar::GetTypeName() {
	return TypeName;
}

// 0x428c00 (this in esi: LTCG's register convention)
RFScrollAnimation::RFScrollAnimation() {
	Stop();
	m_spline = new RFBezierSpline(1024, RFVector2(0.3f, 0.7f), RFVector2(0.5f, 1.0f));
}

// 0x428cd0 (this in eax: LTCG's register convention)
void RFScrollAnimation::Stop() {
	m_elapsed = 0.0f;
	m_active = false;
	m_progress = 0.0f;
	m_duration = 600;
	m_from = 0;
	m_to = 0;
}

// 0x428cf0 (this in esi: LTCG's register convention)
int32_t RFScrollAnimation::Update(float elapsed) {
	unsigned int ms = static_cast<unsigned int>(m_elapsed * 1000.0 + 0.5);
	m_elapsed += elapsed;
	// The original subtracts a start time of 0 (its fsub of the constant 0.0):
	// the progress is (now - start) / duration with the animation starting at 0.
	float progress = static_cast<float>((static_cast<float>(ms) - 0.0) / static_cast<float>(m_duration));
	m_progress = progress;
	if (progress < 1.0f) {
		float pos = m_spline->GetValue(progress) * (m_to - m_from) + m_from;
		return static_cast<int32_t>(pos + 0.5);
	}
	m_active = false;
	return m_to;
}

// 0x428e20 (this in eax: LTCG's register convention)
bool RFThumbButton::IsLButtonDown() const {
	return m_lButtonDown;
}

// 0x428e30
RFScrollBar::RFScrollBar() {
}

// 0x428ec0
RFScrollBar::~RFScrollBar() {
}

// 0x428f30
void RFScrollBar::Create(WndBaseStruct* data, RFWnd* parent) {
	RFWnd::Create(data, parent);
	m_ltButton = 0;
	m_rbButton = 0;
	m_thumb = 0;
	ScrollbarBase* scrollData = dynamic_cast<ScrollbarBase*>(data);
	m_trackRect = scrollData->m_trackRect;
	m_horizontal = scrollData->m_horizontal;

	RFButton* ltButton = new RFButton;
	ltButton->Create(scrollData->m_ltButton, this);
	ltButton->Initialize();
	ltButton->SetSubControl(true);
	m_ltButton = ltButton;

	RFButton* rbButton = new RFButton;
	rbButton->Create(scrollData->m_rbButton, this);
	rbButton->Initialize();
	rbButton->SetSubControl(true);
	m_rbButton = rbButton;

	RFThumbButton* thumb = new RFThumbButton;
	thumb->Create(scrollData->m_thumb, this);
	thumb->Initialize();
	thumb->SetSubControl(true);
	m_thumb = thumb;
	m_thumbSize = m_thumb->GetSize();
}

// 0x429110 (RFWnd::PreInitialize is inlined across units by LTCG)
void RFScrollBar::PreInitialize() {
	RFWnd::PreInitialize();
	m_scrollRange.x = 0;
	m_scrollRange.y = 100;
	m_scrollPos = 0;
	m_scrollSize = 1;
	m_smoothScroll = false;
	m_thumb->SetTrack(m_horizontal, m_trackRect);
	RegisterTo(m_ltButton->GetName(), RFButton::EventClicked,
		Subscriber(&RFScrollBar::OnLTButClicked, this));
	RegisterTo(m_rbButton->GetName(), RFButton::EventClicked,
		Subscriber(&RFScrollBar::OnRBButClicked, this));
	RegisterTo(m_thumb->GetName(), RFWnd::EventMouseMove,
		Subscriber(&RFScrollBar::OnThumbMouseMove, this));
}

// 0x429390 (RFWnd::OnLayoutChanged is inlined across units by LTCG)
void RFScrollBar::OnLayoutChanged(uint32_t flags) {
	RFWnd::OnLayoutChanged(flags);
	RFSize size = GetSize();
	if (flags & 2) {
		if (m_horizontal) {
			m_trackRect.right = size.width - m_trackRect.left;
		} else {
			m_trackRect.bottom = size.height - m_trackRect.top;
		}
		m_thumb->SetTrack(m_horizontal, m_trackRect);
	}
}

// 0x429430
void RFScrollBar::OnEnabled(bool enabled) {
	m_thumb->ShowWindow(enabled);
}

// 0x429440
void RFScrollBar::OnLButtonUp(MouseEventArgs& args) {
	RFWnd::OnLButtonUp(args);
	args.handled = true;
}

// 0x429460
void RFScrollBar::OnRButtonUp(MouseEventArgs& args) {
	RFWnd::OnRButtonUp(args);
	args.handled = true;
}

// 0x429480 (RFWnd::GetRect is inlined across units by LTCG)
void RFScrollBar::OnLButtonDown(MouseEventArgs& args) {
	RFWnd::OnLButtonDown(args);
	// The click position, moved back by half the thumb so the thumb centres
	// on it.
	RFVector2 pos = args.pos;
	pos.x -= m_thumb->GetRect().Width() * 0.5f;
	pos.y -= m_thumb->GetRect().Height() * 0.5f;
	int32_t oldPos = m_scrollPos;
	PositionToLevel(pos);
	m_animation.Stop();
	if (m_smoothScroll) {
		int32_t newPos = m_scrollPos;
		m_scrollPos = oldPos;
		MoveToScroll(newPos);
	} else {
		LevelToThumbPosition();
		ScrollBarPosChangedEventArgs posArgs(this, m_scrollPos);
		SendEvent(EventPosChanged, posArgs);
	}
	args.handled = true;
}

// 0x429620 (SmoothScrolling inlined)
void RFScrollBar::OnUpdate(UpdateEventArgs& args) {
	SmoothScrolling(args.elapsed);
}

// 0x429660
void RFScrollBar::SetScrollSize(int32_t size) {
	m_scrollSize = size;
}

// 0x429670 (RFThumbButton::HasNinePatchImage and RFWnd::SetSize are inlined
// across units by LTCG)
void RFScrollBar::SetScrollRange(int32_t minimum, int32_t maximum) {
	m_scrollRange.y = maximum;
	m_scrollRange.x = minimum;
	SetScrollPos(m_scrollPos);
	// A nine-patch thumb stretches: the track less one pixel a level, but not
	// below its size at creation.
	if (m_thumb->HasNinePatchImage()) {
		float height = m_trackRect.Height() - (m_scrollRange.y - m_scrollRange.x);
		if (height < m_thumbSize.height) {
			height = m_thumbSize.height;
		}
		RFSize size = m_thumb->GetSize();
		size.height = height;
		m_thumb->SetSize(size);
	}
}

// 0x429770
void RFScrollBar::SetScrollPos(int32_t pos) {
	m_scrollPos = pos;
	if (m_scrollPos < m_scrollRange.x) {
		m_scrollPos = m_scrollRange.x;
	} else if (m_scrollPos > m_scrollRange.y) {
		m_scrollPos = m_scrollRange.y;
	}
	LevelToThumbPosition();
	ScrollBarPosChangedEventArgs args(this, m_scrollPos);
	SendEvent(EventPosChanged, args);
}

// 0x429830
int32_t RFScrollBar::GetScrollPos() const {
	return m_scrollPos;
}

// 0x429840
void RFScrollBar::Next() {
	EventArgs args;
	OnRBButClicked(args);
}

// 0x4298a0
void RFScrollBar::Prev() {
	EventArgs args;
	OnLTButClicked(args);
}

// 0x429900
void RFScrollBar::ScrollToTop() {
	SetScrollPos(m_scrollRange.x);
}

// 0x429910
void RFScrollBar::ScrollToBottom() {
	SetScrollPos(m_scrollRange.y);
}

// 0x429920
bool RFScrollBar::IsBottom() {
	if (m_scrollPos == m_scrollRange.y) {
		return true;
	}
	return false;
}

// 0x429930
void RFScrollBar::MoveToScroll(int32_t pos) {
	if (m_animation.m_active) {
		++m_smoothSpeed;
		if (m_smoothSpeed > 5) {
			m_smoothSpeed = 5;
		}
		m_animation.m_from = m_scrollPos;
		int32_t to;
		if (pos > m_scrollPos) {
			to = (pos - m_scrollPos) * m_smoothSpeed + pos;
		} else {
			to = pos - (m_scrollPos - pos) * m_smoothSpeed;
		}
		m_animation.m_elapsed = 0.0f;
		m_animation.m_to = to;
		return;
	}
	m_smoothSpeed = 1;
	m_animation.Start(m_scrollPos, pos);
	// 4 ms a step, 400 to 1000 ms.
	uint32_t duration = abs(m_scrollPos - pos) * 4;
	if (duration < 400) {
		duration = 400;
	} else if (duration > 1000) {
		duration = 1000;
	}
	m_animation.m_duration = duration;
}

// 0x429a30 (RFWnd::OnMouseWheel, which raises EventMouseWheel, is inlined
// across units by LTCG)
void RFScrollBar::OnMouseWheel(MouseEventArgs& args) {
	RFWnd::OnMouseWheel(args);
	if (args.wheelDelta < 0.0f) {
		OnRBButClicked(args);
	} else {
		OnLTButClicked(args);
	}
}

// 0x429a90 (RFWnd::GetScreenPos is inlined across units by LTCG)
void RFScrollBar::OnThumbMouseMove(EventArgs&) {
	if (m_thumb->IsLButtonDown()) {
		RFVector2 pos = m_thumb->GetScreenPos();
		PositionToLevel(pos);
		ScrollBarPosChangedEventArgs args(this, m_scrollPos);
		SendEvent(EventPosChanged, args);
	}
}

// 0x429b80
void RFScrollBar::OnLTButClicked(EventArgs& args) {
	if (m_scrollPos > m_scrollRange.x) {
		if (m_smoothScroll) {
			MoveToScroll(m_scrollPos - m_scrollSize);
		} else {
			SetScrollPos(m_scrollPos - m_scrollSize);
		}
		args.handled = true;
	}
}

// 0x429bd0
void RFScrollBar::OnRBButClicked(EventArgs& args) {
	if (m_scrollPos < m_scrollRange.y) {
		int32_t pos = m_scrollPos + m_scrollSize;
		if (m_smoothScroll) {
			MoveToScroll(pos);
		} else {
			SetScrollPos(pos);
		}
		args.handled = true;
	}
}

// 0x429c20
void RFScrollBar::PositionToLevel(RFVector2 const& pos) {
	if (m_scrollRange.y - m_scrollRange.x > 0) {
		float offset;
		float scrollLength;
		if (m_horizontal) {
			offset = pos.x - GetScreenPos().x - m_trackRect.left;
			float trackLength = m_trackRect.Width();
			scrollLength = trackLength - m_thumb->GetRect().Width();
		} else {
			offset = pos.y - GetScreenPos().y - m_trackRect.top;
			float trackLength = m_trackRect.Height();
			scrollLength = trackLength - m_thumb->GetRect().Height();
		}
		float levelLength = scrollLength / (m_scrollRange.y - m_scrollRange.x);
		float level = offset / levelLength + 0.5;
		int32_t newPos = static_cast<int32_t>(level) + m_scrollRange.x;
		if (newPos < m_scrollRange.x) {
			newPos = m_scrollRange.x;
		} else if (newPos > m_scrollRange.y) {
			newPos = m_scrollRange.y;
		}
		m_scrollPos = newPos;
	}
}

// 0x429da0 (RFWnd::GetRect is inlined across units by LTCG; the lambda's call
// 0x42a3d0 and the rest of its std::function machinery are std:: code outside
// every unit)
void RFScrollBar::LevelToThumbPosition() {
	if (m_scrollRange.y - m_scrollRange.x > 0) {
		// The thumb's offset on the track for the position, the scroll length
		// shared out over the range.
		float offset;
		std::function<void(float)> setOffset = [&offset, this](float scrollLength) {
			float levelLength = scrollLength / (m_scrollRange.y - m_scrollRange.x);
			offset = levelLength * (m_scrollPos - m_scrollRange.x) + 0.5;
		};
		if (m_horizontal) {
			float trackLength = m_trackRect.Width();
			setOffset(trackLength - m_thumb->GetRect().Width());
			float x = GetScreenPos().x;
			float y = m_thumb->GetPos().y + GetScreenPos().y;
			m_thumb->SetScreenPos(m_trackRect.left + x + offset, m_trackRect.top + y);
		} else {
			float trackLength = m_trackRect.Height();
			setOffset(trackLength - m_thumb->GetRect().Height());
			float x = m_thumb->GetPos().x + GetScreenPos().x;
			float y = GetScreenPos().y;
			m_thumb->SetScreenPos(m_trackRect.left + x, m_trackRect.top + y + offset);
		}
	}
}

// 0x42a090
void RFScrollBar::SmoothScrolling(float elapsed) {
	if (m_animation.m_active) {
		SetScrollPos(m_animation.Update(elapsed));
	}
}

