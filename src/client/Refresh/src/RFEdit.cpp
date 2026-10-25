// Original: client\Refresh\src\RFEdit.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include "RFEdit.h"

#include "RFCaret.h"
#include "RFIme.h"
#include "RFEventArgs.h"
#include "RFInterface.h"
#include "RFManager.h"
#include "RFRenderer.h"

#include <windows.h>

#include <algorithm>
#include <malloc.h>
#include <string.h>

// lib: 0x455fd0 (RFCaret's scalar deleting destructor, emitted with RFCaret's
// vtable in RFCaret.cpp; LTCG placed it in this unit's range), the implicit
// copy constructor 0x415f90 and operator= 0x4161c0, the scalar deleting
// destructor 0x416370 (paired); std::wstring members LTCG copied into this
// unit's run with register arguments: 0x4371c0 (assign(wchar_t const*)),
// 0x4371f0 (insert(size_type, wstring const&)), 0x437200 (erase(iterator)),
// 0x437250 (begin), 0x437270 (end), 0x4372a0 (begin() + n), 0x4372b0
// (iterator ==), 0x4372d0 (iterator !=), 0x4372e0 (iterator *), 0x4372f0
// (iterator ++), 0x437300 (append(wstring const&)), 0x437500
// (std::operator+(wstring const&, wstring const&))

// The clipboard helpers RFEdit::KeyDownCopyAndPaste calls (CF_UNICODETEXT
// through Open/Get/SetClipboardData); names ours. LTCG placed them after the
// TinyXML block, so they are defined at the end of this file.
bool GetClipboardText(std::wstring& text);
bool SetClipboardText(std::wstring const& text);

// The class name, the static const at 0xe46fac.
std::wstring const RFEdit::TypeName = L"Edit";
// The event KeyDownReturn raises, the static const at 0xe46f90.
std::wstring const RFEdit::EventEnterKeyDown = L"EnterKeyDown";

// 0x415f20
wchar_t const* RFEdit::ToString() const {
	return L"RFEdit";
}

// 0x415f30
std::wstring const& RFEdit::GetTypeName() {
	return TypeName;
}

// 0x415f40
bool RFEdit::IsEnableCopyAndPaste() const {
	return m_enableCopyAndPaste;
}

// 0x415f50
void RFEdit::SetLimitText(int32_t limitText) {
	m_limitText = limitText;
}

// 0x415f60
int32_t RFEdit::GetLimitText() const {
	return m_limitText;
}

// 0x415f70
bool RFEdit::IsFocusable() const {
	return true;
}

// 0x415f80
int32_t RFEdit::GetCaretLocate() const {
	return m_caretLocate;
}

// 0x416400
void RFEdit::SetColor(uint32_t color) {
	m_fontInfo.color = color;
}

// 0x416410
uint32_t RFEdit::GetColor() const {
	return m_fontInfo.color;
}

// 0x416420
void RFEdit::SetShadowColor(uint32_t color) {
	m_fontInfo.shadowColor = color;
}

// 0x416430
uint32_t RFEdit::GetShadowColor() const {
	return m_fontInfo.shadowColor;
}

// 0x416440
void RFEdit::SetOutlineColor(uint32_t color) {
	m_fontInfo.outlineColor = color;
}

// 0x416450
uint32_t RFEdit::GetOutlineColor() const {
	return m_fontInfo.outlineColor;
}

// 0x416460
void RFEdit::SetSelectionColor(uint32_t color0, uint32_t color1) {
	m_selectionColor[0] = color0;
	m_selectionColor[1] = color1;
}

// 0x416480
void RFEdit::SetStyle(uint32_t style) {
	m_fontInfo.style = style;
}

// 0x416490
uint32_t RFEdit::GetStyle() const {
	return m_fontInfo.style;
}

// 0x4164a0
void RFEdit::SetPadding(RFRect const& padding) {
	m_padding = padding;
}

// 0x4164d0
RFRect const& RFEdit::GetPadding() const {
	return m_padding;
}

// 0x4164e0
void RFEdit::SetPasswordChar(wchar_t passwordChar) {
	m_passwordChar = passwordChar;
}

// 0x416500
wchar_t RFEdit::GetPasswordChar() const {
	return m_passwordChar;
}

// 0x416510
bool RFEdit::IsFocused() const {
	return m_focused;
}

// 0x416520
void RFEdit::SetValidator(RFValidator* validator) {
	m_validator = validator;
}

// 0x416530
RFValidator* RFEdit::GetValidator() const {
	return m_validator;
}

// 0x435000
RFEdit::RFEdit()
	: m_textOffset()
	, m_selectionStart(0)
	, m_selectionEnd(0) {
}

// 0x4350c0
RFEdit::~RFEdit() {
	CleanupPasswordText();
}

// 0x435150
void RFEdit::Create(WndBaseStruct* wndStruct, RFWnd* parent) {
	RFWndEx::Create(wndStruct, parent);
	EditBase* editStruct = static_cast<EditBase*>(wndStruct);
	m_fontInfo.name = editStruct->m_fontName.GetValue();
	m_fontInfo.size = editStruct->m_fontSize.GetValue();
	m_fontInfo.color = editStruct->m_color.GetValue();
	m_fontInfo.shadowColor = editStruct->m_shadowColor.GetValue();
	m_fontInfo.outlineColor = editStruct->m_outlineColor.GetValue();
	m_fontInfo.style = editStruct->m_style;
	m_hAlign = editStruct->m_hAlign;
	m_vAlign = editStruct->m_vAlign;
	m_padding = editStruct->m_padding;
	m_caretBrushDesc = editStruct->m_caretBrushDesc;
	m_passwordChar = editStruct->m_passwordChar;
}

// 0x435230
void RFEdit::PreInitialize() {
	RFWndEx::PreInitialize();
	m_caretLocate = 0;
	m_unreadFlag = false;
	m_validator = 0;
	m_caret = 0;
	m_focused = false;
	m_enableCopyAndPaste = true;
	m_limitText = 40;
	m_selectionColor[0] = 0xfaadd6ff;
	m_selectionColor[1] = 0xfaa0a0a0;
	m_rc.m_clip = true;
}

// 0x4352d0
void RFEdit::OnDestroy() {
	if (m_caret != 0) {
		delete m_caret;
		m_caret = 0;
	}
	RFWndEx::OnDestroy();
}

// 0x435310
void RFEdit::LoadResource() {
	RFWndEx::LoadResource();
	if (m_caretBrushDesc != 0) {
		m_caret = new RFCaret;
		m_caret->SetBrush(m_caretBrushDesc);
	}
}

// 0x435390
void RFEdit::SetText(wchar_t const* text) {
	m_text.assign(text);
	SetCaretLocate((int32_t)m_text.size());
}

// 0x4353e0
wchar_t const* RFEdit::GetText() const {
	return m_text.c_str();
}

// 0x435400
void RFEdit::ClearText() {
	RFInputMethodProvider& ime = RFInputMethodProvider::GetInstance();
	if (ime.m_service != 0) {
		ime.m_service->EndComposition(false);
	}
	m_composition.end = m_caretLocate;
	m_composition.start = m_caretLocate;
	m_composition.composing = false;
	CleanupPasswordText();
	m_text.clear();
	SetCaretLocate(0);
	m_selectionStart = 0;
	m_selectionEnd = 0;
}

// 0x4354a0
void RFEdit::OnEnabled(bool enabled) {
	if (m_focused) {
		if (enabled) {
			ResetCaretBlink();
		}
		RFInputMethodProvider& ime = RFInputMethodProvider::GetInstance();
		if (ime.m_service != 0) {
			ime.m_service->Enable(enabled);
		}
		RFManager::s_interface->OnKeyboardAcquired(enabled);
	}
}

// 0x435510
void RFEdit::OnSetFocus() {
	RFWnd::OnSetFocus();
	m_focused = true;
	if (m_enabled) {
		ResetCaretBlink();
		RFInputMethodProvider& ime = RFInputMethodProvider::GetInstance();
		if (ime.m_service != 0) {
			ime.m_service->Enable(true);
		}
		RFManager::s_interface->OnKeyboardAcquired(true);
		if (m_passwordChar != 0) {
			RFInputMethodProvider& passwordIme = RFInputMethodProvider::GetInstance();
			if (passwordIme.m_service != 0) {
				passwordIme.m_service->ActivateContext(false);
			}
		}
	}
}

// 0x4355d0
void RFEdit::OnKillFocus() {
	RFWnd::OnKillFocus();
	m_focused = false;
	if (m_enabled) {
		if (m_composition.composing) {
			AddCaretLocate(m_composition.end - m_composition.start);
			int32_t locate = m_caretLocate;
			m_composition.end = locate;
			m_composition.start = locate;
			m_composition.composing = false;
		}
		RFManager::s_interface->OnKeyboardAcquired(false);
		RFInputMethodProvider& ime = RFInputMethodProvider::GetInstance();
		if (ime.m_service != 0) {
			ime.m_service->Enable(false);
		}
	}
}

// 0x435690
void RFEdit::OnLButtonDown(MouseEventArgs& args) {
	RFWnd::OnLButtonDown(args);
	args.handled = true;
	if (m_enabled && !m_composition.composing) {
		ResetCaretBlink();
		RFRect rect = ClientToScreen();
		rect.left += m_padding.left;
		int32_t x = (int32_t)args.pos.x;
		int32_t width = 0;
		int32_t prevWidth = 0;
		wchar_t str[2] = {0};
		int32_t left = (int32_t)rect.left;
		for (int32_t i = 0; i < (int32_t)m_text.size(); ++i) {
			str[0] = m_text[i];
			RFSize size = RFManager::s_interface->MeasureText(m_fontInfo, std::wstring(str));
			width += (int32_t)size.width;
			int32_t start = left + m_textOffset[0];
			int32_t charLeft = prevWidth + start;
			int32_t charRight = start + width;
			if (charLeft <= x && x <= charRight) {
				if (x - charLeft < charRight - x) {
					SetCaretLocate(i);
				} else {
					SetCaretLocate(i + 1);
				}
				break;
			}
			prevWidth = width;
		}
	}
}

// 0x4358a0
void RFEdit::OnKeyDown(KeyEventArgs& args) {
	RFWnd::OnKeyDown(args);
	if (!m_enabled || args.keyCode == VK_ESCAPE) {
		return;
	}
	KeyDownSelectStart();
	switch (args.keyCode) {
	case 'C':
	case 'V':
	case 'c':
	case 'v':
		KeyDownCopyAndPaste(args.keyCode);
		break;
	case VK_HOME:
		ResetSelection();
		KeyDownHome();
		break;
	case VK_END:
		ResetSelection();
		KeyDownEnd();
		break;
	case VK_BACK:
		KeyDownDeleteChar(true);
		break;
	case VK_DELETE:
		KeyDownDeleteChar(false);
		break;
	case VK_UP:
	case VK_DOWN:
		ResetSelection();
		break;
	case VK_LEFT:
		ResetSelection();
		KeyDownArrow(-1);
		break;
	case VK_RIGHT:
		ResetSelection();
		KeyDownArrow(1);
		break;
	case VK_RETURN:
		ResetSelection();
		KeyDownReturn(args);
		break;
	}
	KeyDownSelectEnd();
	ResetCaretBlink();
	args.handled = true;
}

// 0x435ab0
void RFEdit::OnChar(CharEventArgs& args) {
	if (!m_enabled) {
		return;
	}
	std::wstring str(args.text);
	if (m_validator != 0 && m_validator->Validate(str) == 0) {
		return;
	}
	if ((int32_t)m_text.size() > 0 && m_composition.composing) {
		DeleteCharAtCaretPos();
	}
	int32_t selection = m_selectionEnd - m_selectionStart;
	if (selection != 0) {
		if (selection > 0) {
			AddCaretLocate(m_selectionStart - m_selectionEnd);
		}
		DeleteSelectionChar();
	}
	std::wstring inserted = InsertStringAtCaretPos(str);
	if (inserted.size() != 0) {
		m_text = inserted;
		if (args.composing) {
			m_composition.start = m_caretLocate;
			m_composition.end = m_caretLocate + str.size();
			m_composition.composing = m_composition.start != m_composition.end;
		} else {
			AddCaretLocate(str.size());
			m_composition.end = m_caretLocate;
			m_composition.start = m_caretLocate;
			m_composition.composing = false;
		}
	} else {
		if (RFManager::GetInstance().IsUsingCustomInputMethod()) {
			RFInputMethodProvider& ime = RFInputMethodProvider::GetInstance();
			if (ime.m_service != 0) {
				ime.m_service->EndComposition(false);
			}
		}
		m_composition.end = m_caretLocate;
		m_composition.start = m_caretLocate;
		m_composition.composing = false;
	}
	args.handled = true;
}

// 0x435cd0
void RFEdit::OnDraw() {
	RFWndEx::OnDraw();
	RFRect rect = ClientToScreen();
	rect.ApplyPadding(m_padding);
	std::wstring text(m_text);
	if (m_passwordChar != 0) {
		// The password character repeated over the text's length.
		int32_t length = (int32_t)text.size();
		wchar_t* masked = (wchar_t*)_alloca(length * sizeof(wchar_t) + sizeof(wchar_t));
		for (int32_t i = 0; i < length; ++i) {
			masked[i] = m_passwordChar;
		}
		masked[length] = 0;
		text = masked;
	}
	int32_t x = GetTextHAlignPos(rect, text);
	int32_t y = GetTextVAlignPos(rect, text);
	int32_t caretPos = GetCaretPos(text, true);
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
	DrawSelection(RFVector2((float)x, (float)y), m_rc.m_depth);
	PrintText(font, x, y, text.c_str());
	if (m_focused) {
		RFInputMethodProvider& ime = RFInputMethodProvider::GetInstance();
		if (ime.m_service != 0) {
			ime.m_service->SetCompositionPos(caretPos + x, y);
		}
	}
	DrawCaret(RFVector2((float)(caretPos + x), (float)y), m_rc.m_depth);
	RFRect screenClip;
	screenClip.IntersectRect(m_rc.m_clipRect, rect);
	m_rc.m_clipRect = screenClip;
	if (m_rc.m_clip) {
		RFManager::s_renderer->PopClipRect();
	}
}

// 0x4361c0
void RFEdit::OnUpdate(UpdateEventArgs& args) {
	if (m_caret != 0) {
		m_caret->Update(args.elapsed);
	}
}

// 0x436210
std::wstring RFEdit::InsertStringAtCaretPos(std::wstring const& str) {
	int32_t size = (int32_t)m_text.size();
	if (size > 0) {
		if ((int32_t)(str.size() + size) <= m_limitText) {
			if (m_caretLocate <= 0) {
				m_caretLocate = 0;
				std::wstring text = str + m_text;
				return text;
			} else if (m_caretLocate >= size) {
				m_caretLocate = size;
				std::wstring text = m_text + str;
				return text;
			} else {
				std::wstring text(m_text);
				text.insert(m_caretLocate, str);
				return text;
			}
		}
		return L"";
	}
	return str;
}

// 0x4363b0
void RFEdit::DeleteCharAtCaretPos() {
	int32_t count = 1;
	if (m_composition.composing) {
		count = m_composition.end - m_composition.start;
	}
	DeleteChar(m_caretLocate, count);
	m_composition.end = m_caretLocate;
	m_composition.start = m_caretLocate;
	m_composition.composing = false;
}

// 0x436400
void RFEdit::DeleteSelectionChar() {
	if (m_selectionEnd - m_selectionStart != 0) {
		int32_t first = (std::min)(m_selectionStart, m_selectionEnd);
		int32_t last = (std::max)(m_selectionStart, m_selectionEnd);
		DeleteChar(first, last - first);
		m_selectionStart = m_caretLocate;
		m_selectionEnd = m_caretLocate;
	}
}

// 0x436460
void RFEdit::DeleteChar(int32_t pos, int32_t count) {
	if (count > 0 && pos < (int32_t)m_text.size()) {
		for (int32_t i = 0; i < count; ++i) {
			std::wstring::iterator it = m_text.begin() + pos;
			if (it == m_text.end()) {
				break;
			}
			m_text.erase(it);
		}
	}
}

// 0x4364e0
void RFEdit::CleanupPasswordText() {
	if (m_passwordChar != 0) {
		for (std::wstring::iterator it = m_text.begin(); it != m_text.end(); ++it) {
			*it = 0;
		}
	}
}

// 0x436530
int32_t RFEdit::GetTextHAlignPos(RFRect const& rect, std::wstring const& text) {
	int32_t width = (int32_t)rect.Width();
	int32_t caretPos = GetCaretPos(text, true);
	RFSize size = RFManager::s_interface->MeasureText(m_fontInfo, text.c_str());
	m_textOffset[0] += (int32_t)rect.left;
	if ((float)(m_textOffset[0] + caretPos) < rect.left || (float)(m_textOffset[0] + caretPos) > rect.right) {
		int32_t margin = 0;
		if ((float)(m_textOffset[0] + caretPos) > rect.right) {
			int32_t length = m_composition.composing == true
				? m_composition.start - m_composition.end + (int32_t)m_text.size()
				: (int32_t)m_text.size();
			if (m_caretLocate < length) {
				margin = (uint32_t)width / 4;
			}
			m_textOffset[0] = (int32_t)rect.right - margin - caretPos;
		} else if ((float)(m_textOffset[0] + caretPos) < rect.left) {
			if (m_caretLocate > 0) {
				margin = (uint32_t)width / 4;
			}
			m_textOffset[0] = (int32_t)rect.left - caretPos + margin;
			if (m_textOffset[0] > (int32_t)rect.left) {
				m_textOffset[0] = (int32_t)rect.left;
			}
		}
	} else {
		if (m_hAlign != 0) {
			if (size.width < width && m_hAlign == 1) {
				m_textOffset[0] = width / 2 - (int32_t)size.width / 2 + (int32_t)rect.left;
			} else {
				if (m_textOffset[0] > (int32_t)rect.left) {
					m_textOffset[0] = (int32_t)rect.right - (int32_t)size.width - m_textOffset[0];
				}
				int32_t right = (int32_t)rect.right;
				if (m_textOffset[0] + size.width < right) {
					m_textOffset[0] = right - (int32_t)size.width;
				}
			}
		}
	}
	int32_t pos = m_textOffset[0];
	m_textOffset[0] = pos - (int32_t)rect.left;
	return pos;
}

// 0x4367c0
int32_t RFEdit::GetTextVAlignPos(RFRect const& rect, std::wstring const& text) {
	int32_t height = (int32_t)rect.Height();
	RFSize size = RFManager::s_interface->MeasureText(m_fontInfo, text.c_str());
	m_textOffset[1] += (int32_t)rect.top;
	switch (m_vAlign) {
	case 0:
		m_textOffset[1] = (int32_t)rect.top;
		break;
	case 1:
		m_textOffset[1] = (height - (int32_t)size.height) / 2 + (int32_t)rect.top;
		break;
	case 2:
		m_textOffset[1] = (int32_t)rect.bottom - (int32_t)size.height;
		break;
	}
	int32_t pos = m_textOffset[1];
	m_textOffset[1] = pos - (int32_t)rect.top;
	return pos;
}

// 0x436930
int32_t RFEdit::GetCaretPos(std::wstring const& text, bool withComposition) {
	int32_t locate = m_caretLocate;
	if (withComposition && m_composition.composing) {
		locate += m_composition.end - m_composition.start;
	}
	std::wstring before = text.substr(0, locate);
	RFSize size = RFManager::s_interface->MeasureText(m_fontInfo, before.c_str());
	return (int32_t)size.width;
}

// 0x436a50
void RFEdit::EnableCopyAndPaste(bool enable) {
	m_enableCopyAndPaste = enable;
	m_selectionStart = 0;
	m_selectionEnd = 0;
}

// 0x436a70
void RFEdit::DrawCaret(RFVector2 const& pos, float depth) {
	if (m_focused && m_enabled && m_caret != 0) {
		m_caret->Draw(pos, GetAlpha(false), depth);
	}
}

// 0x436ad0
void RFEdit::ResetCaretBlink() {
	if (m_caret != 0) {
		m_caret->ResetBlink();
	}
}

// 0x436af0
void RFEdit::ResetSelection() {
	if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) == 0) {
		m_selectionStart = 0;
		m_selectionEnd = 0;
	}
}

// 0x436b20
void RFEdit::DrawSelection(RFVector2 const& pos, float depth) {
	if (m_text.size() > 0 && m_enableCopyAndPaste) {
		if (m_selectionEnd - m_selectionStart != 0) {
			int32_t first = (std::min)(m_selectionStart, m_selectionEnd);
			int32_t last = (std::max)(m_selectionStart, m_selectionEnd);
			if (first >= 0 && first < (int32_t)m_text.size() && last <= (int32_t)m_text.size()) {
				std::wstring before = m_text.substr(0, first);
				std::wstring selected;
				if (last < (int32_t)m_text.size()) {
					selected = m_text.substr(first, last - first);
				} else {
					selected = m_text.substr(first);
				}
				RFSize beforeSize = RFManager::s_interface->MeasureText(m_fontInfo, before.c_str());
				RFSize selectedSize = RFManager::s_interface->MeasureText(m_fontInfo, selected.c_str());
				uint32_t color = m_focused ? m_selectionColor[0] : m_selectionColor[1];
				RFManager::s_renderer->FillRectangle(color, pos.x + beforeSize.width, pos.y,
					selectedSize.width, selectedSize.height, depth);
			}
		}
	}
}

// 0x436d40
void RFEdit::SetCaretLocate(int32_t caretLocate) {
	if (caretLocate < 0) {
		m_caretLocate = 0;
	} else if (caretLocate > (int32_t)m_text.size()) {
		m_caretLocate = (int32_t)m_text.size();
	} else {
		m_caretLocate = caretLocate;
	}
}

// 0x436d80
void RFEdit::AddCaretLocate(int32_t delta) {
	m_caretLocate += delta;
	if (m_caretLocate < 0) {
		m_caretLocate = 0;
	} else if (m_caretLocate > (int32_t)m_text.size()) {
		m_caretLocate = (int32_t)m_text.size();
	}
}

// 0x436dc0
bool RFEdit::KeyDownCopyAndPaste(uint32_t key) {
	if (m_enableCopyAndPaste) {
		if (key == 'v' || key == 'V') {
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
				if (m_passwordChar == 0) {
					std::wstring text;
					GetClipboardText(text);
					text = text.substr(0, m_limitText);
					RFPoint point;
					CharEventArgs args(this, text.c_str(), false, point);
					OnChar(args);
				}
				return true;
			}
		} else if (key == 'c' || key == 'C') {
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
				if (m_passwordChar == 0 && m_selectionEnd - m_selectionStart != 0) {
					int32_t first = (std::min)(m_selectionStart, m_selectionEnd);
					int32_t last = (std::max)(m_selectionStart, m_selectionEnd);
					std::wstring selected;
					if (last < (int32_t)m_text.size()) {
						selected = m_text.substr(first, last - first);
					} else {
						selected = m_text.substr(first);
					}
					SetClipboardText(selected);
				}
				return true;
			}
		}
	}
	return false;
}

// 0x436ff0
void RFEdit::KeyDownSelectStart() {
	if (m_enableCopyAndPaste && (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0) {
		if (m_selectionEnd - m_selectionStart == 0) {
			m_selectionStart = m_caretLocate;
		}
	}
}

// 0x437030
void RFEdit::KeyDownSelectEnd() {
	if (m_enableCopyAndPaste && (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0) {
		m_selectionEnd = m_caretLocate;
	}
}

// 0x437060
void RFEdit::KeyDownHome() {
	SetCaretLocate(0);
}

// 0x437080
void RFEdit::KeyDownEnd() {
	SetCaretLocate((int32_t)m_text.size());
}

// 0x4370a0
void RFEdit::KeyDownDeleteChar(bool backspace) {
	if (m_composition.composing) {
		return;
	}
	if (m_selectionEnd - m_selectionStart == 0) {
		if (backspace) {
			if (m_caretLocate > 0) {
				AddCaretLocate(-1);
				DeleteCharAtCaretPos();
			}
		} else if ((int32_t)m_text.size() > m_caretLocate) {
			DeleteCharAtCaretPos();
		}
	} else {
		if (m_selectionEnd - m_selectionStart > 0) {
			// the caret moves back to the selection start, clamped like
			// AddCaretLocate
			m_caretLocate += m_selectionStart - m_selectionEnd;
			if (m_caretLocate < 0) {
				m_caretLocate = 0;
			} else if (m_caretLocate > (int32_t)m_text.size()) {
				m_caretLocate = (int32_t)m_text.size();
			}
		}
		DeleteSelectionChar();
	}
}

// 0x437140
void RFEdit::KeyDownArrow(int32_t delta) {
	AddCaretLocate(delta);
}

// 0x437180
void RFEdit::KeyDownReturn(KeyEventArgs& args) {
	EventMap::iterator it = m_events.find(EventEnterKeyDown);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x464e50
bool GetClipboardText(std::wstring& text) {
	if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) {
		return false;
	}
	if (!OpenClipboard(NULL)) {
		return false;
	}
	HANDLE data = GetClipboardData(CF_UNICODETEXT);
	text = static_cast<wchar_t const*>(GlobalLock(data));
	GlobalUnlock(data);
	CloseClipboard();
	return true;
}

// 0x464ec0
bool SetClipboardText(std::wstring const& text) {
	if (!OpenClipboard(NULL)) {
		return false;
	}
	HGLOBAL data = GlobalAlloc(GMEM_MOVEABLE, (text.size() + 1) * sizeof(wchar_t));
	if (data == NULL) {
		CloseClipboard();
		return false;
	}
	void* buffer = GlobalLock(data);
	memcpy(buffer, text.c_str(), (text.size() + 1) * sizeof(wchar_t));
	GlobalUnlock(data);
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, data);
	CloseClipboard();
	return true;
}
