// Original: client\Refresh\src\RFSlider.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
// lib: 0x4169e0 (vector deleting destructor, ??_E of the dllexport class),
// 0x44ed10 (SliderPosChangedEventArgs scalar deleting destructor), 0x44ff20
// (WindowEventArgs_<RFSlider> scalar deleting destructor), the implicit copy
// constructor 0x416830 and operator= 0x4168f0, MemberFunctionFunctor<RFSlider>'s
// scalar deleting destructor 0x450010, the class's scalar deleting destructor
// 0x4169b0, the destructors of SliderPosChangedEventArgs 0x44ed40 and
// WindowEventArgs_<RFSlider> 0x44ed50, WindowEventArgs_<RFSlider>'s constructor
// 0x44ff10, the Subscriber<RFSlider> constructor 0x44ff50,
// MemberFunctionFunctor<RFSlider>'s constructor 0x44ffa0, operator() 0x44ffc0,
// AddRef 0x44ffe0, Release 0x44fff0, GetRefCount 0x450000 and destructor
// 0x450040 (template and implicit members, all paired)
#include <new>
#include <string>

#include "RFSlider.h"

#include "RFButton.h"
#include "RFEventArgs.h"
#include "RFManager.h"
#include "RFSize.h"
#include "RFVector2.h"

// The event raised when the position changes, the static const at 0xe4701c.
std::wstring const RFSlider::EventPosChanged = L"PosChanged";

// The class name, the static const at 0xe47038.
std::wstring const RFSlider::TypeName = L"Slider";

// 0x416810
wchar_t const* RFSlider::ToString() const {
	return L"RFSlider";
}

// 0x416820
std::wstring const& RFSlider::GetTypeName() {
	return TypeName;
}

// 0x44ed60
RFSlider::RFSlider() {
	m_dragging = false;
}

// 0x44eda0
RFSlider::~RFSlider() {
}

// 0x44edb0
void RFSlider::Create(WndBaseStruct* data, RFWnd* parent) {
	RFWnd::Create(data, parent);
	m_ltButton = 0;
	m_rbButton = 0;
	m_thumb = 0;
	SliderBase* sliderData = dynamic_cast<SliderBase*>(data);
	m_trackRect = sliderData->m_trackRect;
	m_horizontal = sliderData->m_horizontal;

	RFButton* ltButton = new RFButton;
	ltButton->Create(sliderData->m_ltButton, this);
	ltButton->Initialize();
	ltButton->SetSubControl(true);
	m_ltButton = ltButton;

	RFButton* rbButton = new RFButton;
	rbButton->Create(sliderData->m_rbButton, this);
	rbButton->Initialize();
	rbButton->SetSubControl(true);
	m_rbButton = rbButton;

	RFThumbButton* thumb = new RFThumbButton;
	thumb->Create(sliderData->m_thumb, this);
	thumb->Initialize();
	thumb->SetSubControl(true);
	m_thumb = thumb;
}

// 0x44ef60 (RFWnd::GetName is inlined across units by LTCG)
void RFSlider::PreInitialize() {
	RFWnd::PreInitialize();
	m_slideRange.x = 0;
	m_slideRange.y = 5;
	m_slidePos = 0;
	m_slideSize = 1;
	m_thumb->m_horizontal = m_horizontal;
	m_thumb->m_trackRect = m_trackRect;
	RegisterTo(m_ltButton->GetName(), RFButton::EventClicked,
		Subscriber(&RFSlider::OnLTButClicked, this));
	RegisterTo(m_rbButton->GetName(), RFButton::EventClicked,
		Subscriber(&RFSlider::OnRBButClicked, this));
	RegisterTo(m_thumb->GetName(), RFWnd::EventMouseMove,
		Subscriber(&RFSlider::OnThumbMouseMove, this));
}

// 0x44f1e0
void RFSlider::OnEnabled(bool enable) {
	m_thumb->EnableWindow(enable);
	m_ltButton->EnableWindow(enable);
	m_rbButton->EnableWindow(enable);
}

// 0x44f270 (RFWnd::GetRect 0x406810 and RFManager::GetInstance are inlined
// across units by LTCG)
void RFSlider::OnLButtonDown(MouseEventArgs& args) {
	RFWnd::OnLButtonDown(args);
	RFVector2 point = args.pos;
	point.x -= m_thumb->GetRect().Width() * 0.5f;
	point.y -= m_thumb->GetRect().Height() * 0.5f;
	PositionToLevel(point);
	int32_t half = m_slideSize / 2;
	if (m_slidePos <= half) {
		m_slidePos = m_slideRange.x;
	} else if (m_slidePos + half >= m_slideRange.y) {
		m_slidePos = m_slideRange.y;
	}
	LevelToThumbPosition();
	SliderPosChangedEventArgs posArgs(this, m_slidePos);
	SendEvent(EventPosChanged, posArgs);
	RFManager::GetInstance().SetCapturedWindow(this);
	m_dragging = true;
	args.handled = true;
}

// 0x44f420
void RFSlider::OnLButtonUp(MouseEventArgs& args) {
	RFWnd::OnLButtonUp(args);
	m_dragging = false;
	RFManager::GetInstance().SetCapturedWindow(0);
	args.handled = true;
}

// 0x44f470
void RFSlider::SetSlideSize(int32_t size) {
	m_slideSize = size;
}

// 0x44f480
void RFSlider::SetSlideRange(int32_t minimum, int32_t maximum) {
	m_slideRange = RFPoint(minimum, maximum);
	LevelToThumbPosition();
	SliderPosChangedEventArgs args(this, m_slidePos);
	SendEvent(EventPosChanged, args);
}

// 0x44f530
void RFSlider::SetSlidePos(int32_t pos) {
	m_slidePos = pos;
	if (m_slidePos < m_slideRange.x) {
		m_slidePos = m_slideRange.x;
	} else if (m_slidePos > m_slideRange.y) {
		m_slidePos = m_slideRange.y;
	}
	LevelToThumbPosition();
	SliderPosChangedEventArgs args(this, m_slidePos);
	SendEvent(EventPosChanged, args);
}

// 0x44f5f0
int32_t RFSlider::GetSlidePos() const {
	return m_slidePos;
}

// 0x44f600
void RFSlider::Next() {
	EventArgs args;
	OnRBButClicked(args);
}

// 0x44f660
void RFSlider::Prev() {
	EventArgs args;
	OnLTButClicked(args);
}

// 0x44f6c0
void RFSlider::SlideToTop() {
	SetSlidePos(m_slideRange.x);
}

// 0x44f6d0
void RFSlider::SlideToBottom() {
	SetSlidePos(m_slideRange.y);
}

// 0x44f6e0 (RFWnd::GetRect 0x406810 is inlined across units by LTCG)
void RFSlider::OnMouseMove(MouseEventArgs& args) {
	RFWnd::OnMouseMove(args);
	if (m_dragging) {
		RFVector2 point = args.pos;
		point.x -= m_thumb->GetRect().Width() * 0.5f;
		point.y -= m_thumb->GetRect().Height() * 0.5f;
		PositionToLevel(point);
		int32_t half = m_slideSize / 2;
		if (m_slidePos <= half) {
			m_slidePos = m_slideRange.x;
		} else if (m_slidePos + half >= m_slideRange.y) {
			m_slidePos = m_slideRange.y;
		}
		LevelToThumbPosition();
		SliderPosChangedEventArgs posArgs(this, m_slidePos);
		SendEvent(EventPosChanged, posArgs);
	}
	args.handled = true;
}

// 0x44f870 (RFWnd::OnMouseWheel, which raises EventMouseWheel, is inlined
// across units by LTCG)
void RFSlider::OnMouseWheel(MouseEventArgs& args) {
	RFWnd::OnMouseWheel(args);
	if (args.wheelDelta < 0.0f) {
		OnRBButClicked(args);
	} else {
		OnLTButClicked(args);
	}
	args.handled = true;
}

// 0x44f8e0 (0.0%: LTCG `and esp,-8` frame and RFWnd::GetScreenPos 0x406670 inlined)
void RFSlider::OnThumbMouseMove(EventArgs&) {
	if (m_thumb->m_lButtonDown) {
		RFVector2 point = m_thumb->GetScreenPos();
		PositionToLevel(point);
		LevelToThumbPosition();
		SliderPosChangedEventArgs args(this, m_slidePos);
		SendEvent(EventPosChanged, args);
	}
}

// 0x44f9d0
void RFSlider::OnLTButClicked(EventArgs&) {
	m_slidePos -= m_slideSize;
	if (m_slidePos < m_slideRange.x) {
		m_slidePos = m_slideRange.x;
	}
	LevelToThumbPosition();
	SliderPosChangedEventArgs args(this, m_slidePos);
	SendEvent(EventPosChanged, args);
}

// 0x44fa80
void RFSlider::OnRBButClicked(EventArgs&) {
	m_slidePos += m_slideSize;
	if (m_slidePos > m_slideRange.y) {
		m_slidePos = m_slideRange.y;
	}
	LevelToThumbPosition();
	SliderPosChangedEventArgs args(this, m_slidePos);
	SendEvent(EventPosChanged, args);
}

// 0x44fb30 (RFWnd::GetRect 0x406810 is inlined across units by LTCG)
void RFSlider::PositionToLevel(RFVector2 const& point) {
	if (m_slideRange.Length() > 0) {
		float pos;
		float length;
		if (m_horizontal) {
			pos = point.x - GetScreenPos().x - m_trackRect.left;
			float trackLength = m_trackRect.Width();
			length = trackLength - m_thumb->GetRect().Width();
		} else {
			pos = point.y - GetScreenPos().y - m_trackRect.top;
			float trackLength = m_trackRect.Height();
			length = trackLength - m_thumb->GetRect().Height();
		}
		int32_t minimum = m_slideRange.x;
		float step = length / m_slideRange.Length();
		int32_t level = static_cast<int32_t>(static_cast<float>(pos / step + 0.5)) + minimum;
		if (level < minimum) {
			level = minimum;
		} else if (level > m_slideRange.y) {
			level = m_slideRange.y;
		}
		m_slidePos = level;
	}
}

// 0x44fcb0 (RFWnd::GetRect 0x406810 is inlined across units by LTCG)
void RFSlider::LevelToThumbPosition() {
	if (m_slideRange.Length() > 0) {
		if (m_horizontal) {
			float trackLength = m_trackRect.Width();
			float thumbLength = m_thumb->GetRect().Width();
			float length = trackLength - thumbLength;
			float step = length / m_slideRange.Length();
			float offset = step * (m_slidePos - m_slideRange.x) + 0.5;
			RFVector2 screenPos(GetScreenPos().x, GetScreenPos().y + m_thumb->GetPos().y);
			RFVector2 pos(m_trackRect.left + screenPos.x, m_trackRect.top + screenPos.y);
			m_thumb->SetScreenPos(pos.x + offset, pos.y);
		} else {
			float trackLength = m_trackRect.Height();
			float thumbLength = m_thumb->GetRect().Height();
			float length = trackLength - thumbLength;
			float step = length / m_slideRange.Length();
			float offset = step * (m_slidePos - m_slideRange.x) + 0.5;
			RFVector2 screenPos(GetScreenPos().x + m_thumb->GetPos().x, GetScreenPos().y);
			RFVector2 pos(m_trackRect.left + screenPos.x, m_trackRect.top + screenPos.y);
			m_thumb->SetScreenPos(pos.x, pos.y + offset);
		}
	}
}
