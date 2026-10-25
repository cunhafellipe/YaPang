// Original: client\Refresh\src\RFTextArea.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include "RFTextArea.h"

#include <algorithm>

#include "RFCaret.h"
#include "RFEventArgs.h"
#include "RFIme.h"
#include "RFInterface.h"
#include "RFManager.h"
#include "RFRenderer.h"
#include "RFText.h"
#include "RFTextComponent.h"

#include <windows.h>

// lib: 0x428480 (std::wstring::replace(off, n, const wchar_t*, count)), 0x428680
// (std::wstring::replace(off, n, const wstring&, roff, count)), 0x4283c0
// (LTCG's constant clone of replace(off, 2, L"\n") calling 0x428480, off in
// eax; unreferenced), 0x428670 (an empty std helper, `ret`, between the two
// replace instantiations; unreferenced); the implicit members of the
// dllexport class: 0x415720 (copy constructor), 0x4159d0 (operator=),
// 0x415bf0 (vector deleting destructor), 0x415bc0 (scalar deleting
// destructor ??_G, paired); event-argument members instantiated here
// (unreferenced copies, LTCG conventions), paired: 0x426300, 0x4263f0,
// 0x4264f0 (the implicit destructors of HyperLinkClickedEventArgs,
// HyperLinkEnterEventArgs, RichTextDarwIamgeEventArgs; this in esi),
// 0x426330, 0x426420 (the HyperLinkEnterEventArgs and
// RichTextDarwIamgeEventArgs constructors; this on the stack), 0x4262f0 (an
// implicit destructor writing only EventArgs's vptr, this in eax: paired as
// WindowEventArgs_<RFTextArea>'s like RFButton's 0x421680; not emitted by
// this TU), 0x428410, 0x428450 (WindowEventArgs_<RFTextArea>'s constructor,
// this in eax, and scalar deleting destructor: paired, not emitted by this
// TU, which raises no WindowEventArgs_<RFTextArea>). 0x426520-0x426570 are
// RFText's field accessors (this in eax: IsComposing, SetHeight, SetHAlign,
// SetTextLimit, GetTextLimit, GetLineLimit), header inlines of src/RFText.h
// (unreferenced copies, LTCG conventions; this TU emits them, paired)

std::wstring const RFTextArea::EventHyperLinkClicked = L"HyperLinkClicked";
std::wstring const RFTextArea::EventHyperLinkEnter = L"HyperLinkEnter";
std::wstring const RFTextArea::EventHyperLinkLeave = L"HyperLinkLeave";
std::wstring const RFTextArea::EventRichTextDrawImage = L"RichTextDrawImage";

// The paragraph layout pass of Invaildate (std::for_each 0x458950, its loop
// 0x459440: library instantiations): each paragraph is laid out at (x, y)
// with the text's alignment and y moves down by its height. 0xc bytes,
// passed by value. The name is ours.
struct ParagraphLayout {
	ParagraphLayout(int32_t& y_, int32_t const& x_, RFText* text_) : y(y_), x(x_), text(text_) {}

	void operator()(RFParagraph* paragraph) const {
		y += paragraph->Layout(x, y, text->m_hAlign);
	}

	int32_t& y;     // 0x0
	int32_t x;      // 0x4
	RFText* text;   // 0x8
};

// 0x415690
wchar_t const* RFTextArea::ToString() const {
	return L"RFTextArea";
}

// 0x4156a0
void RFTextArea::SetPadding(RFRect const& padding) {
	m_padding = padding;
}

// 0x4156d0
RFRect const& RFTextArea::GetPadding() const {
	return m_padding;
}

// 0x4156e0
void RFTextArea::SetHyperLinkFontInfo(FontInfo const& fontInfo) {
	m_hyperLinkFontInfo = fontInfo;
}

// 0x415c50
void RFTextArea::SetColor(uint32_t color) {
	m_fontInfo.color = color;
}

// 0x415c60
uint32_t RFTextArea::GetColor() const {
	return m_fontInfo.color;
}

// 0x415c70
void RFTextArea::SetShadowColor(uint32_t shadowColor) {
	m_fontInfo.shadowColor = shadowColor;
}

// 0x415c80
uint32_t RFTextArea::GetShadowColor() const {
	return m_fontInfo.shadowColor;
}

// 0x415c90
void RFTextArea::SetOutlineColor(uint32_t outlineColor) {
	m_fontInfo.outlineColor = outlineColor;
}

// 0x415ca0
uint32_t RFTextArea::GetOutlineColor() const {
	return m_fontInfo.outlineColor;
}

// 0x415cb0
void RFTextArea::SetStyle(uint32_t style) {
	m_fontInfo.style = style;
}

// 0x415cc0
uint32_t RFTextArea::GetStyle() const {
	return m_fontInfo.style;
}

// 0x415cd0
FontInfo const& RFTextArea::GetFontInfo() const {
	return m_fontInfo;
}

// 0x4261e0 (this in eax: LTCG). pos is default-constructed (RFVector2()
// 0x4289b0, inlined in the original), then Clear() zeroes it again.
TextAreaPendingClick::TextAreaPendingClick() {
	Clear();
}

// 0x426580
RFTextArea::RFTextArea(): m_scrollX(0), m_scrollY(0) {
}

// 0x426690
RFTextArea::~RFTextArea() {
}

// 0x4267b0
// LTCG inlines RFWndEx::Create (0x41d9b0) in the original. The text's
// literal "\\n" sequences become line breaks before it is set.
void RFTextArea::Create(WndBaseStruct* wndStruct, RFWnd* parent) {
	RFWndEx::Create(wndStruct, parent);
	TextAreaBase* base = static_cast<TextAreaBase*>(wndStruct);
	m_fontInfo.name = base->m_fontName.GetValue();
	m_fontInfo.size = base->m_fontSize.GetValue();
	m_fontInfo.color = base->m_color.GetValue();
	m_fontInfo.shadowColor = base->m_shadowColor.GetValue();
	m_fontInfo.outlineColor = base->m_outlineColor.GetValue();
	m_fontInfo.style = base->m_style;
	m_hyperLinkFontInfo = m_fontInfo;
	m_padding = base->m_padding;
	m_hAlign = base->m_hAlign;
	m_vAlign = static_cast<VertAlignTypes::Type>(0);
	m_showCaret = base->m_showCaret.GetValue();
	m_caretBrushDesc = base->m_caretBrushDesc;
	m_autoFit = base->m_autoFit.GetValue();

	std::wstring text = base->m_text.GetValue();
	RFRect rect = ClientToScreen();
	ApplyGlobalPadding(rect);
	m_text = new RFText(static_cast<int32_t>(rect.Width()), static_cast<int32_t>(rect.Height()), m_hAlign,
		m_vAlign, &m_fontInfo, &m_hyperLinkFontInfo, this);
	if (!text.empty()) {
		std::wstring::size_type pos;
		while ((pos = text.find(L"\\n")) != std::wstring::npos) {
			text.replace(pos, 2, L"\n");
		}
		SetText(text);
	}

	wchar_t sample[] = L"A";
	RFSize size = RFManager::s_interface->MeasureText(m_fontInfo, sample);
	int32_t height = static_cast<int32_t>(size.height + 0.5);
	m_lineHeight = static_cast<int32_t>(height * 130 / 100.0 + 0.5);
}

// 0x426b20
// LTCG inlines RFWndEx::PreInitialize (0x41d9e0) in the original.
void RFTextArea::PreInitialize() {
	RFWndEx::PreInitialize();
	m_focused = false;
	m_caret = 0;
	m_hyperLinkHovered = false;
	m_clipChildren = true;
}

// 0x426b90
void RFTextArea::OnDestroy() {
	if (m_caret) {
		delete m_caret;
		m_caret = 0;
	}
	if (m_text) {
		delete m_text;
		m_text = 0;
	}
	RFWndEx::OnDestroy();
}

// 0x426bf0
void RFTextArea::LoadResource() {
	RFWndEx::LoadResource();
	m_caret = new RFCaret;
	m_caret->SetBrush(m_caretBrushDesc);
}

// 0x426c90
void RFTextArea::OnLayoutChanged(uint32_t flags) {
	if (flags & 2) {
		ApplyAnchor();
	}
	if ((flags & 2 || flags & 0x10000000) && m_text) {
		RFRect rect = ClientToScreen();
		ApplyGlobalPadding(rect);
		m_text->SetWidth((int32_t)(rect.right - rect.left));
		m_text->SetHeight((int32_t)(rect.bottom - rect.top));
	}
}

// 0x426d90
void RFTextArea::OnEnabled(bool enabled) {
	if (m_focused) {
		if (enabled && m_showCaret) {
			ResetCaretBlink();
		}
		RFInputMethodProvider& ime = RFInputMethodProvider::GetInstance();
		if (ime.m_service != 0) {
			ime.m_service->Enable(enabled);
		}
	}
}

// 0x426e00
// A left press on a hyperlink only raises its event; otherwise, with a
// caret, it moves the caret to the point (or defers that during an IME
// composition: OnChar replays the pending click).
void RFTextArea::OnLButtonDown(MouseEventArgs& args) {
	RFWnd::OnLButtonDown(args);
	args.handled = true;
	if (m_enabled) {
		if (!HitTestHyperLink(args, true) && m_showCaret) {
			if (m_text->IsComposing()) {
				m_pendingClick.pending = true;
				RFVector2 pos(args.pos.x, args.pos.y);
				m_pendingClick.pos.x = pos.x;
				m_pendingClick.pos.y = pos.y;
				return;
			}
			RFRect rect = ClientToScreen();
			ApplyGlobalPadding(rect);
			m_text->MoveCaretTo((int32_t)args.pos.x, (int32_t)args.pos.y, (int32_t)(args.pos.x - rect.left));
			ResetCaretBlink();
		}
	}
}

// 0x426ee0
void RFTextArea::OnRButtonDown(MouseEventArgs& args) {
	RFWnd::OnRButtonDown(args);
	if (m_enabled) {
		if (!HitTestHyperLink(args, true)) {
			args.handled = true;
		}
	}
}

// 0x426f30
void RFTextArea::OnRButtonUp(MouseEventArgs& args) {
	RFWnd::OnRButtonUp(args);
	if (m_enabled) {
		if (!HitTestHyperLink(args, false)) {
			args.handled = true;
		}
	}
}

// 0x426f60
// The original's code continues at 0x427000 (the splitter cuts it there).
bool RFTextArea::HitTestHyperLink(MouseEventArgs& args, bool down) {
	std::wstring link;
	int32_t kind;
	if (args.button == 0) {
		if (down) {
			kind = 2;
		}
	} else if (args.button == 1) {
		kind = !down;
	} else {
		return false;
	}
	if (m_text->HitTestHyperLink(&link, (int32_t)args.pos.x, (int32_t)args.pos.y, kind)) {
		HyperLinkClickedEventArgs clicked(this, link);
		SendEvent(EventHyperLinkClicked, clicked);
		RFManager::GetInstancePtr()->PostHyperLinkEvent(this, m_name, link);
		return true;
	}
	return false;
}

// 0x427170
// Every piece of the text drops its hover state, then the one under the
// mouse (if any) is a hyperlink: entering one, or moving to another, raises
// EventHyperLinkEnter; leaving them raises EventHyperLinkLeave. The enter
// args are marked handled after both posts (the original stores it, on a
// local about to die). LTCG inlines RFText::HitTestHyperLink, SendEvent and
// RFManager::PostGlobalHyperLinkLeaveEvent (0x4181f0) in the original.
void RFTextArea::OnMouseMove(MouseEventArgs& args) {
	RFWnd::OnMouseMove(args);
	m_text->ResetHover();
	std::wstring link;
	if (m_text->HitTestHyperLink(&link, (int32_t)args.pos.x, (int32_t)args.pos.y, 3)) {
		if (!m_hyperLinkHovered) {
			m_hyperLinkHovered = true;
			m_hoveredHyperLink = link;
			HyperLinkEnterEventArgs enter(this, link);
			SendEvent(EventHyperLinkEnter, enter);
			RFManager::GetInstancePtr()->PostGlobalHyperLinkEnterEvent(this, m_name, link);
			enter.handled = true;
		} else if (m_hoveredHyperLink != link) {
			m_hoveredHyperLink = link;
			HyperLinkEnterEventArgs enter(this, link);
			SendEvent(EventHyperLinkEnter, enter);
			RFManager::GetInstancePtr()->PostGlobalHyperLinkEnterEvent(this, m_name, link);
			enter.handled = true;
		}
	} else if (m_hyperLinkHovered) {
		m_hyperLinkHovered = false;
		WindowEventArgs_<RFWnd> leave(this);
		SendEvent(EventHyperLinkLeave, leave);
		RFManager::GetInstancePtr()->PostGlobalHyperLinkLeaveEvent(this);
	}
}

// 0x4274e0
// LTCG inlines RFWnd::OnMouseLeave (0x4079f0) and
// RFManager::PostGlobalHyperLinkLeaveEvent (0x4181f0) in the original.
void RFTextArea::OnMouseLeave() {
	RFWnd::OnMouseLeave();
	m_text->ResetHover();
	if (m_hyperLinkHovered) {
		m_hyperLinkHovered = false;
		WindowEventArgs_<RFWnd> leave(this);
		SendEvent(EventHyperLinkLeave, leave);
		RFManager::GetInstancePtr()->PostGlobalHyperLinkLeaveEvent(this);
	}
}

// 0x4275f0
// LTCG inlines RFWnd::OnKeyDown (0x407c40) and ResetCaretBlink in the
// original. The editing keys go to the text, Return breaks the line.
void RFTextArea::OnKeyDown(KeyEventArgs& args) {
	RFWnd::OnKeyDown(args);
	if (!m_enabled || !m_showCaret || args.keyCode == VK_ESCAPE) {
		return;
	}
	ResetCaretBlink();
	switch (args.keyCode) {
	case VK_HOME:
		m_text->HandleKey(EditKeyTypes::Home);
		break;
	case VK_END:
		m_text->HandleKey(EditKeyTypes::End);
		break;
	case VK_DELETE:
		m_text->HandleKey(EditKeyTypes::Delete);
		break;
	case VK_BACK:
		m_text->HandleKey(EditKeyTypes::Back);
		break;
	case VK_UP:
		m_text->HandleKey(EditKeyTypes::Up);
		break;
	case VK_DOWN:
		m_text->HandleKey(EditKeyTypes::Down);
		break;
	case VK_LEFT:
		m_text->HandleKey(EditKeyTypes::Left);
		break;
	case VK_RIGHT:
		m_text->HandleKey(EditKeyTypes::Right);
		break;
	case VK_RETURN:
		m_text->AddText(L"\n", false);
		break;
	}
	args.handled = true;
}

// 0x427810
// Typed text goes into the text at the caret; a click deferred during the
// composition is replayed afterwards.
void RFTextArea::OnChar(CharEventArgs& args) {
	if (m_enabled && m_showCaret) {
		m_text->InsertText(args.text, args.composing);
		if (m_pendingClick.pending) {
			RFRect rect = ClientToScreen();
			ApplyGlobalPadding(rect);
			m_text->MoveCaretTo((int32_t)m_pendingClick.pos.x, (int32_t)m_pendingClick.pos.y,
			    (int32_t)(m_pendingClick.pos.x - rect.left));
			m_pendingClick.Clear();
		}
		args.handled = true;
	}
}

// 0x4278d0
void RFTextArea::OnSetFocus() {
	RFWnd::OnSetFocus();
	m_focused = true;
	if (m_enabled && m_showCaret) {
		ResetCaretBlink();
		RFInputMethodProvider& ime = RFInputMethodProvider::GetInstance();
		if (ime.m_service != 0) {
			ime.m_service->Enable(true);
		}
		RFManager::s_interface->OnKeyboardAcquired(true);
	}
}

// 0x427950
void RFTextArea::OnKillFocus() {
	RFWnd::OnKillFocus();
	m_focused = false;
	if (m_enabled && m_showCaret) {
		RFManager::s_interface->OnKeyboardAcquired(false);
		RFInputMethodProvider& ime = RFInputMethodProvider::GetInstance();
		if (ime.m_service != 0) {
			ime.m_service->Enable(false);
		}
	}
}

// 0x4279c0
// Draws the text inside the padded client rectangle, clipped to the
// render context while it clips, then the caret.
void RFTextArea::OnDraw() {
	RFWndEx::OnDraw();
	RFRect rect = ClientToScreen();
	rect.left += m_padding.left;
	rect.top += m_padding.top;
	rect.right -= m_padding.right;
	rect.bottom -= m_padding.bottom;
	if (rect.Width() < 0.0f) {
		return;
	}
	if (rect.Height() < 0.0f) {
		return;
	}
	Invaildate();
	GetAlpha(false);
	RFRect clip;
	clip.IntersectRect(m_rc.m_clipRect, rect);
	if (m_rc.m_clip) {
		RFManager::s_renderer->PushClipRect(clip);
	}
	m_text->Draw(GetAlpha(false));
	DrawCaret(m_rc.m_depth);
	RFRect screenClip;
	screenClip.IntersectRect(m_rc.m_clipRect, rect);
	m_rc.m_clipRect = screenClip;
	if (m_rc.m_clip) {
		RFManager::s_renderer->PopClipRect();
	}
}

// 0x427b30
void RFTextArea::OnUpdate(UpdateEventArgs& args) {
	RFWnd::OnUpdate(args);
	if (m_caret) {
		m_caret->Update(args.elapsed);
	}
}

// 0x427b80
void RFTextArea::OnMouseWheel(MouseEventArgs& args) {
	RFWnd::OnMouseWheel(args);
}

// 0x427bc0
void RFTextArea::SetText(std::wstring const& text) {
	ClearText();
	AddText(text);
}

// 0x427bf0
void RFTextArea::AddText(std::wstring const& text) {
	m_text->AddText(text.c_str(), true);
	if (m_autoFit) {
		FitToContent(false);
	}
}

// 0x427c30
void RFTextArea::SetHAlign(HorzAlignTypes::Type hAlign) {
	m_hAlign = hAlign;
	m_text->SetHAlign(hAlign);
}

// 0x427c50
void RFTextArea::AddPlainText(std::wstring const& text) {
	m_text->AddText(text.c_str(), false);
	if (m_autoFit) {
		FitToContent(false);
	}
}

// 0x427c90
std::wstring const& RFTextArea::GetText() {
	m_textCache = m_text->GetText();
	return m_textCache;
}

// 0x427d10
void RFTextArea::ClearText() {
	m_scrollX = 0;
	m_scrollY = 0;
	m_text->Clear();
}

// 0x427d30
void RFTextArea::SetTextLimit(uint32_t textLimit) {
	m_text->SetTextLimit(textLimit);
}

// 0x427d50
uint32_t RFTextArea::GetTextLimit() const {
	return m_text->GetTextLimit();
}

// 0x427d60
uint32_t RFTextArea::GetLineLimit() const {
	return m_text->GetLineLimit();
}

// 0x427d70
// Resizes the window to its text plus the padding (and the manager's global
// padding while enabled), the width only when fitWidth is set. Both
// dimensions add the padding's top and bottom, as the original does.
void RFTextArea::FitToContent(bool fitWidth) {
	RFSize size = GetSize();
	RFManager& manager = RFManager::GetInstance();
	RFRect padding;
	if (manager.IsGlobalPaddingEnabled()) {
		padding = m_padding + manager.GetGlobalPadding();
	} else {
		padding = m_padding;
	}
	if (fitWidth) {
		size.width = static_cast<float>(m_text->GetMaxWidth()) + padding.top + padding.bottom;
	}
	size.height = static_cast<float>(m_text->GetTotalHeight()) + padding.top + padding.bottom;
	if (size != m_localSize) {
		m_localSize = size;
		OnLayoutChanged(2);
	}
}

// 0x427f90
void RFTextArea::AutoFit(bool autoFit) {
	m_autoFit = autoFit;
	if (autoFit) {
		FitToContent(false);
	}
}

// 0x427fb0
// Lays the paragraphs out again from the padded screen rectangle, scrolled.
void RFTextArea::Invaildate() {
	RFRect rect = ClientToScreen();
	ApplyGlobalPadding(rect);
	int32_t x = (int32_t)rect.left;
	int32_t y = m_scrollY + (int32_t)rect.top;
	std::deque<RFParagraph*>& paragraphs = m_text->m_paragraphs;
	std::for_each(paragraphs.begin(), paragraphs.end(), ParagraphLayout(y, x, m_text));
}

// 0x428040
void RFTextArea::SendDrawImageEvent(RFVector2 const& pos, std::wstring const& imageName) {
	RichTextDarwIamgeEventArgs args(this, pos, imageName);
	SendEvent(EventRichTextDrawImage, args);
}

// 0x428130
// Shrinks rect by the text area's padding, plus the manager's global
// padding while that is enabled.
void RFTextArea::ApplyGlobalPadding(RFRect& rect) const {
	RFManager& manager = RFManager::GetInstance();
	if (manager.IsGlobalPaddingEnabled()) {
		RFRect padding = m_padding + manager.GetGlobalPadding();
		rect.left += padding.left;
		rect.top += padding.top;
		rect.right -= padding.right;
		rect.bottom -= padding.bottom;
	} else {
		rect.left += m_padding.left;
		rect.top += m_padding.top;
		rect.right -= m_padding.right;
		rect.bottom -= m_padding.bottom;
	}
}

// 0x428220
// The height of a line: the height of "A" in the text font, scaled by the
// 130% line spacing RFText also starts with (its +0x34), rounded.
int32_t RFTextArea::GetTextHeight() const {
	wchar_t sample[] = L"A";
	RFSize size = RFManager::s_interface->MeasureText(m_fontInfo, sample);
	return static_cast<int32_t>(size.height * 130.0 / 100.0 + 0.5);
}

// 0x428300
int32_t RFTextArea::GetNumOfLine() const {
	return m_text->GetNumOfLine();
}

// 0x428310
void RFTextArea::DrawCaret(float arg) {
	if (m_focused && m_enabled && m_showCaret && m_caret) {
		RFParagraph* paragraph = m_text->m_currentParagraph;
		int32_t x = paragraph->m_caretX;
		int32_t y = paragraph->m_caretY;
		m_caret->Draw(RFVector2((float)x, (float)y), GetAlpha(false), arg);
		RFInputMethodProvider& ime = RFInputMethodProvider::GetInstance();
		if (ime.m_service != 0) {
			ime.m_service->SetCompositionPos(x, y);
		}
	}
}

// 0x4283a0
void RFTextArea::ResetCaretBlink() {
	if (m_caret) {
		m_caret->ResetBlink();
	}
}
