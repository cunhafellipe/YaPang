// Original: client\Refresh\src\RFLabel.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include "RFLabel.h"

#include <sstream>

#include "RFInterface.h"
#include "RFManager.h"
#include "RFRenderer.h"

// lib: 0x414300 (implicit copy constructor), 0x414450 (implicit operator=),
// 0x414560 (vector deleting destructor), 0x414530 (scalar deleting
// destructor), 0x4145c0, 0x414600, 0x414630 (FontInfo's implicit copy
// constructor, destructor and operator=); std::basic_ostringstream<wchar_t>:
// 0x41f7b0 (virtual-base destructor), 0x4200f0 (constructor), 0x420210 (str)

// The class name, the static const at 0xe46c9c.
std::wstring const RFLabel::TypeName = L"RFLabel";

// 0x41f0c0
RFLabel::RFLabel() {
}

// 0x41f140
RFLabel::~RFLabel() {
}

// 0x41f1c0
void RFLabel::Create(WndBaseStruct* wndStruct, RFWnd* parent) {
	RFWnd::Create(wndStruct, parent);
	LabelBase* labelStruct = static_cast<LabelBase*>(wndStruct);
	m_padding = RFRect(labelStruct->m_padding.GetValue());
	m_fontInfo.name = labelStruct->m_fontName.GetValue();
	m_fontInfo.size = labelStruct->m_fontSize.GetValue();
	RFInterface* iface = RFManager::s_interface;
	m_fontInfo.color = labelStruct->m_color.GetValue();
	m_fontInfo.shadowColor = labelStruct->m_shadowColor.GetValue();
	m_fontInfo.outlineColor = labelStruct->m_outlineColor.GetValue();
	m_fontInfo.style = labelStruct->m_style;
	m_hAlign = labelStruct->m_hAlign.GetValue();
	m_vAlign = labelStruct->m_vAlign.GetValue();
	m_textTrimmingType = labelStruct->m_textTrimmingType.GetValue();
	m_autoFit = labelStruct->m_autoFit.GetValue();
	SetLabel(iface->TranslateText(labelStruct->m_label));
}

// 0x41f340
// SetSize is inlined in the original (LTCG).
void RFLabel::FitToContent() {
	if (!m_label.empty()) {
		RFSize size = RFManager::s_interface->MeasureText(m_fontInfo, std::wstring(m_label.c_str()));
		size.width += m_padding.right + m_padding.left;
		size.height += m_padding.bottom + m_padding.top;
		SetSize(size);
	}
}

// 0x41f490
void RFLabel::AutoFit(bool autoFit) {
	m_autoFit = autoFit;
	if (autoFit) {
		FitToContent();
	}
}

// 0x41f4b0
void RFLabel::PreInitialize() {
	RFWnd::PreInitialize();
	m_hitTestVisible = false;
}

// 0x41f510
void RFLabel::putComma_(wchar_t const* src, wchar_t* dst) {
	// Copies the digits of src into dst with a comma before every group of
	// three counted from the end.
	int32_t remaining = static_cast<int32_t>(wcslen(src));
	while ((*dst = *src) != 0) {
		++dst;
		++src;
		if (--remaining != 0 && remaining % 3 == 0) {
			*dst++ = L',';
		}
	}
}

// 0x41f580
// The second argument is not read. The stream's constructor and str() stay
// out of line in the original (LTCG).
void RFLabel::SetLabel(int32_t value, bool) {
	std::wostringstream stream;
	stream << static_cast<long>(value);  // operator<<(long) in the original
	std::wstring text = stream.str();
	wchar_t buffer[0x400] = { 0 };
	putComma_(text.c_str(), buffer);
	SetLabel(std::wstring(buffer));
}

// 0x41f830
void RFLabel::SetLabel(std::wstring const& label) {
	m_label = label;
	if (m_autoFit) {
		FitToContent();
	}
}

// 0x41f860
void RFLabel::SetPadding(RFRect const& padding) {
	m_padding = padding;
}

// 0x41f890
RFRect const& RFLabel::GetPadding() const {
	return m_padding;
}

// 0x41f8a0
// The original inlines RFRect::ApplyPadding, GetAlpha(false) and PrintText
// and realigns its frame (LTCG).
void RFLabel::OnDraw() {
	if (m_label.empty()) {
		return;
	}
	RFRect rect = ClientToScreen();
	std::wstring trimmed;
	rect.ApplyPadding(m_padding);
	RFSize size = RFManager::s_interface->MeasureText(m_fontInfo, std::wstring(m_label.c_str()));
	wchar_t const* text;
	float width;
	if (m_textTrimmingType == 1 && size.width > (width = rect.Width())) {
		// Keeps the characters that fit with "..." after them.
		RFSize dotsSize = RFManager::s_interface->MeasureText(m_fontInfo, std::wstring(L"..."));
		int32_t total = static_cast<int32_t>(dotsSize.width + 0.5);
		int32_t i = 0;
		do {
			wchar_t ch[2] = { 0 };
			ch[0] = m_label.at(i);
			size = RFManager::s_interface->MeasureText(m_fontInfo, std::wstring(ch));
			if (static_cast<int32_t>(size.width + 0.5) + total > width) {
				break;
			}
			trimmed.append(ch);
			total += static_cast<int32_t>(size.width + 0.5);
		} while (++i < static_cast<int32_t>(m_label.size()));
		trimmed.append(L"...");
		text = trimmed.c_str();
	} else {
		text = m_label.c_str();
	}
	int32_t x = 0;
	int32_t y = 0;
	int32_t w = static_cast<int32_t>(rect.Width());
	int32_t h = static_cast<int32_t>(rect.Height());
	size = RFManager::s_interface->MeasureText(m_fontInfo, std::wstring(text));
	switch (m_hAlign) {
	case 0:
		x = static_cast<int32_t>(rect.left);
		break;
	case 1:
		x = (w - static_cast<int32_t>(size.width)) / 2 + static_cast<int32_t>(rect.left);
		break;
	case 2:
		x = static_cast<int32_t>(rect.right - static_cast<int32_t>(size.width));
		break;
	}
	switch (m_vAlign) {
	case 0:
		y = static_cast<int32_t>(rect.top);
		break;
	case 1:
		y = (h - static_cast<int32_t>(size.height)) / 2 + static_cast<int32_t>(rect.top);
		break;
	case 2:
		y = static_cast<int32_t>(rect.bottom - static_cast<int32_t>(size.height));
		break;
	}
	// The colour's alpha scaled by the window's. The same expression is in
	// RFEdit::OnDraw, RFLabel::OnDraw and TextBlock: perhaps a shared macro or
	// inline in the original, but no out-of-line copy identifies one, so each
	// function spells it out.
	FontInfo font = m_fontInfo;
	float alpha = GetAlpha(false);
	uint32_t color = m_fontInfo.color;
	uint32_t a = static_cast<uint32_t>(static_cast<int32_t>(color >> 24) / 255.0 * alpha * 255.0);
	font.color = ((((a << 8) | static_cast<uint8_t>(color >> 16)) << 8 | static_cast<uint8_t>(color >> 8)) << 8)
		| static_cast<uint8_t>(color);
	RFRect clip;
	clip.IntersectRect(m_rc.m_clipRect, rect);
	if (m_rc.m_clip) {
		RFManager::s_renderer->PushClipRect(clip);
	}
	PrintText(font, x, y, std::wstring(text));
	RFRect screenClip;
	screenClip.IntersectRect(m_rc.m_clipRect, rect);
	m_rc.m_clipRect = screenClip;
	if (m_rc.m_clip) {
		RFManager::s_renderer->PopClipRect();
	}
}

