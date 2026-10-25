// Original: client\Refresh\src\RFStackPanel.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include <new>

#include "RFStackPanel.h"

// lib: 0x416b30, 0x416a70, 0x416ac0, 0x416b00
// The vector deleting destructor ??_E (vtable slot 1), the implicit copy
// constructor and copy assignment, and the scalar deleting destructor ??_G
// (paired with map_symbol.ts).

// The class name, the static const at 0xe47000.
std::wstring const RFStackPanel::TypeName = L"RFStackPanel";

// 0x416a50
wchar_t const* RFStackPanel::ToString() const {
	return L"RFStackPanel";
}

// 0x416a60
std::wstring const& RFStackPanel::GetTypeName() {
	return TypeName;
}

// 0x44e710
RFStackPanel::RFStackPanel() {
}

// 0x44e730
RFStackPanel::~RFStackPanel() {
}

// 0x44e740
void RFStackPanel::Initialize() {
	RFWnd::Initialize();
	StackUp_Impl();
}

// 0x44e780
void RFStackPanel::Create(WndBaseStruct* wndStruct, RFWnd* parent) {
	RFWnd::Create(wndStruct, parent);
	StackPanelBase* panelStruct = static_cast<StackPanelBase*>(wndStruct);
	m_hAlign = panelStruct->m_hAlign.GetValue();
	m_vAlign = panelStruct->m_vAlign.GetValue();
	m_orientation = panelStruct->m_orientation.GetValue();
}

// 0x44e7c0
OrientationTypes::Type RFStackPanel::GetOrientation() const {
	return m_orientation;
}

// 0x44e7d0
void RFStackPanel::SetOrientation(OrientationTypes::Type orientation) {
	m_orientation = orientation;
}

// 0x44e7e0
void RFStackPanel::OnLayoutChanged(uint32_t flags) {
	if (flags & 2) {
		StackUp_Impl();
	}
	std::vector<RFWnd*>::iterator end = _children.end();
	for (std::vector<RFWnd*>::iterator it = _children.begin(); it != end; ++it) {
		(*it)->ApplyAnchor();
	}
}

// 0x44e840
void RFStackPanel::StackUp() {
	StackUp_Impl();
	std::vector<RFWnd*>::iterator end = _children.end();
	for (std::vector<RFWnd*>::iterator it = _children.begin(); it != end; ++it) {
		(*it)->ApplyAnchor();
	}
}

// 0x44e890
void RFStackPanel::StackUp_Impl() {
	if (m_orientation == 0) {
		StackUpVertically();
	} else if (m_orientation == 1) {
		StackUpHorizontally();
	}
}

// 0x44e8c0 (the original inlines RFWnd::SetPos 0x4064c0, whose __ftol2 calls
// truncate the position, and the last GetSize 0x406750, LTCG)
void RFStackPanel::StackUpVertically() {
	float contentHeight = 0.0f;
	std::vector<RFWnd*>::iterator end = _children.end();
	for (std::vector<RFWnd*>::iterator it = _children.begin(); it != end; ++it) {
		contentHeight += (*it)->GetSize().height;
	}
	RFSize panelSize = GetSize();
	float x = 0.0f;
	float y = 0.0f;
	switch (m_vAlign) {
	case 0:
		y = 0.0f;
		break;
	case 1:
		y = (panelSize.height - contentHeight) * 0.5;
		break;
	case 2:
		y = panelSize.height - contentHeight;
		break;
	}
	std::vector<RFWnd*>::reverse_iterator rend = _children.rend();
	for (std::vector<RFWnd*>::reverse_iterator it = _children.rbegin(); it != rend; ++it) {
		switch (m_hAlign) {
		case 0:
			x = 0.0f;
			break;
		case 1: {
			RFSize childSize = (*it)->GetSize();
			x = (panelSize.width - childSize.width) * 0.5;
			break;
		}
		case 2: {
			RFSize childSize = (*it)->GetSize();
			x = panelSize.width - childSize.width;
			break;
		}
		}
		(*it)->SetPos(x, y);
		y += (*it)->GetSize().height;
	}
}

// 0x44eac0 (the original inlines RFWnd::SetPos 0x4064c0, whose __ftol2 calls
// truncate the position, and the last GetSize 0x406750, LTCG)
void RFStackPanel::StackUpHorizontally() {
	float contentWidth = 0.0f;
	std::vector<RFWnd*>::iterator end = _children.end();
	for (std::vector<RFWnd*>::iterator it = _children.begin(); it != end; ++it) {
		contentWidth += (*it)->GetSize().width;
	}
	RFSize panelSize = GetSize();
	float x = 0.0f;
	float y = 0.0f;
	switch (m_hAlign) {
	case 0:
		x = 0.0f;
		break;
	case 1:
		x = (panelSize.width - contentWidth) * 0.5;
		break;
	case 2:
		x = panelSize.width - contentWidth;
		break;
	}
	std::vector<RFWnd*>::reverse_iterator rend = _children.rend();
	for (std::vector<RFWnd*>::reverse_iterator it = _children.rbegin(); it != rend; ++it) {
		switch (m_vAlign) {
		case 0:
			y = 0.0f;
			break;
		case 1:
			y = (panelSize.height - (*it)->GetSize().height) * 0.5;
			break;
		case 2:
			y = panelSize.height - (*it)->GetSize().height;
			break;
		}
		(*it)->SetPos(x, y);
		x += (*it)->GetSize().width;
	}
}
