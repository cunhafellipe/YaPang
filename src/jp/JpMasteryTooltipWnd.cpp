// The JP 983 game's mastery tooltip (JpMasteryTooltipWnd.h), from the JP
// exe's bodies (node scripts/disasm.ts --jp 0x<va>). Its creator uses
// RFManager.h's CreateWnd<T> / CreateDesktopWnd<T> (instances 0x5d7320 /
// 0x5d73b0).
// lib: 0x5d7470 (std::wstring::find(wchar_t, size_t) instance)
#include "JpMasteryTooltipWnd.h"

#include <string>

#include "JpTooltipWnd.h"
#include "RFManager.h"
#include "RFRect.h"
#include "RFSize.h"
#include "RFTextArea.h"
#include "wview.h"

// 0xe781c4
RFWnd* RFMasteryToolTipWnd::s_loadParent = NULL;

// 0x5d71e0
RFMasteryToolTipWnd::RFMasteryToolTipWnd()
	: m_textArea(NULL) {
}

// 0x5d7200 (the body of the scalar deleting destructor 0x5d7440)
RFMasteryToolTipWnd::~RFMasteryToolTipWnd() {
}

// 0x5d7210
void RFMasteryToolTipWnd::SetPosition(float x, float y) {
	if (x < 0.0f) {
		x = 0.0f;
	}
	if (y < 0.0f) {
		y = 0.0f;
	}
	if (g_view->GetWidth() < GetSize().width + x) {
		x = g_view->GetWidth() - GetSize().width;
	}
	if (g_view->GetHeight() < GetSize().height + y) {
		y = g_view->GetHeight() - GetSize().height;
	}
	SetScreenPos(x, y);
}

// 0x5d7300
void RFMasteryToolTipWnd::SetTextColor(uint32_t color) {
	if (m_textArea != NULL) {
		m_textArea->SetColor(color);
	}
}

// 0x5d7490
void RFMasteryToolTipWnd::FitToText() {
	if (m_textArea == NULL) {
		return;
	}
	std::wstring text = m_textArea->GetText();
	size_t pos = text.find(L'\n');
	int32_t longest = static_cast<int32_t>(pos != std::wstring::npos ? pos : text.size());
	int32_t lines = 1;
	while (pos != std::wstring::npos) {
		++lines;
		size_t next = text.find(L'\n', pos + 1);
		int32_t length = static_cast<int32_t>(next != std::wstring::npos ? next - pos : text.size() - pos);
		if (length > longest) {
			longest = length;
		}
		pos = next;
	}
	float width = m_textArea->GetPadding().Width() + longest * 12.0;
	float height = m_textArea->GetPadding().Height() + lines * 19.0;
	if (width > 300.0f) {
		width = 300.0f;
	}
	m_textArea->SetSize(RFSize(width, height));
}

// 0x5d76c0
RFMasteryToolTipWnd* RFMasteryToolTipWnd::Load(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFMasteryToolTipWnd>(&RFManager::GetInstance(), L"kam_mastery_tooltip.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFMasteryToolTipWnd>(&RFManager::GetInstance(), L"kam_mastery_tooltip.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFMasteryToolTipWnd>(&RFManager::GetInstance(), L"kam_mastery_tooltip.xml");
		break;
	}
	s_loadParent = parent;
	return static_cast<RFMasteryToolTipWnd*>(wnd);
}

// 0x5d7800
void RFMasteryToolTipWnd::Show(bool show) {
	if (m_textArea != NULL) {
		FitToText();
		SetSize(m_textArea->GetSize());
		ShowWindow(show);
	}
}

// 0x5d7840
void RFMasteryToolTipWnd::OnInitialUpdate() {
	m_textArea = dynamic_cast<RFTextArea*>(GetChild(L"message"));
}
