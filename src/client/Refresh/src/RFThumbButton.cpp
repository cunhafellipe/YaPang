// Original: client\Refresh\src\RFThumbButton.cpp (JP 983 exe, object
// 0x45a480-0x45a7f0; the file name is assumed from the class)
// lib: 0x45a510 (scalar deleting destructor; the class is not exported)
#include <new>

#include "RFThumbButton.h"

#include "RFEventArgs.h"

// 0x45a480
wchar_t const* RFThumbButton::ToString() const {
	return L"RFThumbButton";
}

// 0x45a490
RFThumbButton::RFThumbButton() : m_horizontal(true) {
}

// 0x45a540
RFThumbButton::~RFThumbButton() {
}

// 0x45a550
void RFThumbButton::Create(WndBaseStruct* createStruct, RFWnd* parent) {
	RFButton::Create(createStruct, parent);
}

// 0x45a560
void RFThumbButton::PreInitialize() {
	RFButton::PreInitialize();
	m_dragable = true;
}

// 0x45a580
void RFThumbButton::SetTrack(bool horizontal, RFRect const& track) {
	m_horizontal = horizontal;
	m_trackRect = track;
}

// 0x45a5b0
void RFThumbButton::OnMouseMove(MouseEventArgs& args) {
	RFRect screen = m_super->GetScreenRect();
	if (m_horizontal) {
		screen.left += m_trackRect.left;
		screen.right = screen.left + m_trackRect.Width();
		float x = (float)(int)(args.pos.x - m_dragOffset.x);
		if (x < screen.left) {
			args.pos.x = screen.left + m_dragOffset.x;
		} else if (x + GetRect().Width() > screen.right) {
			args.pos.x = m_dragOffset.x + screen.right - GetRect().Width();
		}
		args.pos.y = m_dragOffset.y + GetScreenPos().y;
	} else {
		screen.top += m_trackRect.top;
		screen.bottom = screen.top + m_trackRect.Height();
		float y = (float)(int)(args.pos.y - m_dragOffset.y);
		if (y < screen.top) {
			args.pos.y = screen.top + m_dragOffset.y;
		} else if (y + GetRect().Height() > screen.bottom) {
			args.pos.y = m_dragOffset.y + screen.bottom - GetRect().Height();
		}
		args.pos.x = m_dragOffset.x + GetScreenPos().x;
	}
	RFWnd::OnMouseMove(args);
	args.handled = true;
}

// 0x45a7d0
bool RFThumbButton::HasNinePatchImage() const {
	return m_stateImages[0]->GetType() == ImageSetTypes::NinePatch;
}
