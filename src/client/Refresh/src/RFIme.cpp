// Original: client\Refresh\src\RFIme.cpp (JP 983 exe; the file name is
// assumed from the class: the asserts inside the range name only
// RFSingleton.h and RFManager.cpp, inlined from RFManager under LTCG)
#include <new>

#include "RFIme.h"

#include <malloc.h>
#include <string.h>

#include "RFInterface.h"
#include "RFManager.h"

// lib: 0x450ff0 (RFInputMethodService's implicit constructor: stores the vptr)
// lib: 0x4512b0 (~RFInputMethodService's body, `this` in eax under LTCG)
// lib: 0x4519b0 implicit RFImeResult copy constructor, 0x4519e0 wstring _Tidy
// (register arguments under LTCG)
// lib: 0x4510a0 (out-of-line copy of an m_composing read, `this` in eax under
// LTCG; inlined in QueueResult)
// lib: std:: instantiations used here (register arguments under LTCG):
// 0x452530 wstring insert at end, 0x452580 wstring::clear,
// 0x452590 vector _Vector_val constructor, 0x4525a0 vector<RFImeResult>::_Tidy,
// 0x4525e0 vector iterator copy, 0x4525f0 vector<RFImeResult>::size,
// 0x452600 vector<RFImeResult>::push_back, 0x4526e0 vector<RFImeResult>::erase,
// 0x452820 erase(first, last), 0x452880 _Destroy_range<RFImeResult>,
// 0x452a80 wstring::insert(off, count, ch), 0x452df0 wstring::assign(first, last),
// 0x453f00 vector<wstring>::push_back, 0x454300 _Destroy_range<wstring>,
// 0x454610 _Move<wstring*>

// the "ImeWndClass" window proc (0x451370, below)
LRESULT CALLBACK ImeWndProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam);

// 0x451090
bool RFIme::IsEnabled() {
	return m_enabled;
}

// 0x4510b0 (out-of-line copy, `flags` in eax under LTCG)
static bool HasCompositionString(LPARAM flags) {
	return (flags & GCS_COMPSTR) != 0;
}

// 0x4510c0 (out-of-line copy, `flags` in eax under LTCG)
static bool HasResultString(LPARAM flags) {
	return (flags & GCS_RESULTSTR) != 0;
}

// 0x451130
RFIme::RFIme(HWND parent)
	: m_parentWnd(parent),
	  m_enabled(true),
	  m_contextActive(false),
	  m_composing(false),
	  m_savedContext(NULL),
	  m_unusedHandle(NULL),
	  m_imeWnd(NULL) {
	WNDCLASSEXW wc;
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = ImeWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(RFIme*);
	wc.hInstance = GetModuleHandleW(NULL);
	wc.hIcon = NULL;
	wc.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"ImeWndClass";
	wc.hIconSm = NULL;
	RegisterClassExW(&wc);
	RECT rect;
	GetClientRect(m_parentWnd, &rect);
	m_imeWnd = CreateWindowExW(0, L"ImeWndClass", NULL, WS_CHILD, 0, 0, rect.right, rect.bottom,
		m_parentWnd, NULL, GetModuleHandleW(NULL), NULL);
	SetWindowLongPtrW(m_imeWnd, 0, (LONG_PTR)this);
	UpdateLanguage(GetKeyboardLayout(0));
	UpdateConversionStatus();
}

// 0x4512c0
RFIme::~RFIme() {
	DestroyWindow(m_imeWnd);
}

// 0x451350
void RFIme::Resize(int width, int height) {
	if (m_imeWnd) {
		MoveWindow(m_imeWnd, 0, 0, width, height, FALSE);
	}
}

// 0x451370: dispatches the WM_IME_* messages to slots 12-21
LRESULT CALLBACK ImeWndProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam) {
	RFIme* ime = (RFIme*)GetWindowLongPtrW(wnd, 0);
	if (!ime) {
		return DefWindowProcW(wnd, message, wParam, lParam);
	}
	bool result = true;
	switch (message) {
	case WM_INPUTLANGCHANGE:
		result = ime->OnInputLangChange(wParam, lParam);
		break;
	case WM_IME_STARTCOMPOSITION:
		result = ime->OnImeStartComposition();
		break;
	case WM_CHAR:
		result = ime->OnChar(wParam, lParam);
		ime->QueueResult();
		RFManager::GetInstance().UpdateInputMethod();
		break;
	case WM_KEYDOWN:
		RFManager::GetInstance().InjectKeyDown(wParam, lParam);
		break;
	case WM_KEYUP:
		RFManager::GetInstance().InjectKeyUp(wParam, lParam);
		break;
	case WM_IME_ENDCOMPOSITION:
		result = ime->OnImeEndComposition();
		ime->QueueResult();
		RFManager::GetInstance().UpdateInputMethod();
		break;
	case WM_IME_COMPOSITION:
		result = ime->OnImeComposition(wParam, lParam);
		ime->QueueResult();
		RFManager::GetInstance().UpdateInputMethod();
		break;
	case WM_IME_SETCONTEXT:
		result = ime->OnImeSetContext(wParam, &lParam);
		break;
	case WM_IME_NOTIFY:
		result = ime->OnImeNotify(wParam, lParam);
		break;
	case WM_IME_CHAR:
		result = ime->OnImeChar(wParam, lParam);
		break;
	default:
		return DefWindowProcW(wnd, message, wParam, lParam);
	}
	if (!result) {
		return 0;
	}
	return DefWindowProcW(wnd, message, wParam, lParam);
}

// 0x451600
void RFIme::Enable(bool enable) {
	m_enabled = enable;
	EndComposition(false);
	ActivateContext(enable);
	m_results.clear();
	if (m_enabled) {
		SetFocus(m_imeWnd);
	} else {
		SetFocus(m_parentWnd);
	}
}

// 0x451660
void RFIme::RestoreFocus() {
	if (m_enabled) {
		SetFocus(m_imeWnd);
	}
}

// 0x451680
void RFIme::SetNativeMode(bool native) {
	HWND wnd;
	if (m_enabled) {
		wnd = m_imeWnd;
	} else {
		wnd = m_parentWnd;
	}
	RFImeContext imc(wnd);
	DWORD conversion;
	DWORD sentence;
	ImmGetConversionStatus(imc, &conversion, &sentence);
	ImmSetConversionStatus(imc, (conversion & ~IME_CMODE_LANGUAGE) | (native == true), sentence);
}

// 0x4516e0
bool RFIme::IsNativeMode() {
	HWND wnd;
	if (m_enabled) {
		wnd = m_imeWnd;
	} else {
		wnd = m_parentWnd;
	}
	RFImeContext imc(wnd);
	DWORD conversion;
	DWORD sentence;
	ImmGetConversionStatus(imc, &conversion, &sentence);
	if (conversion & IME_CMODE_NATIVE) {
		return true;
	}
	return false;
}

// 0x451740
void RFIme::GetStatus(int* language, int* conversion) {
	*language = m_language;
	*conversion = m_conversion;
}

// 0x451760 (PostIMStatusEvent 0x418270 inlined under LTCG)
void RFIme::UpdateConversionStatus() {
	RFImeContext imc(m_imeWnd);
	DWORD conversion;
	DWORD sentence;
	ImmGetConversionStatus(imc, &conversion, &sentence);
	m_conversion = 0;
	if (conversion & IME_CMODE_NATIVE) {
		m_conversion = 1;
	}
	if (conversion & IME_CMODE_KATAKANA) {
		m_conversion |= 4;
	}
	if (conversion & IME_CMODE_FULLSHAPE) {
		m_conversion |= 2;
	}
	RFManager::GetInstance().PostIMStatusEvent(m_language, m_conversion);
}

// 0x451870
void RFIme::ActivateContext(bool activate) {
	if (activate) {
		ImmAssociateContext(m_imeWnd, m_savedContext);
		return;
	}
	HIMC previous = ImmAssociateContext(m_imeWnd, NULL);
	if (previous) {
		m_savedContext = previous;
	}
}

// 0x4518b0
int RFIme::PopResult(wchar_t* buffer, int length, bool* composing) {
	std::vector<RFImeResult>::iterator it = m_results.begin();
	RFImeResult result = *it;
	int size = result.text.size();
	if (buffer) {
		memcpy(buffer, result.text.c_str(), length * sizeof(wchar_t));
		*composing = result.composing;
		m_results.erase(it);
	}
	return size;
}

// 0x451a10
int RFIme::GetResultCount() {
	return m_results.size();
}

// 0x451a20
void RFIme::SetCompositionPos(int x, int y) {
	RFImeContext imc(m_imeWnd);
	COMPOSITIONFORM form;
	memset(&form, 0, sizeof(form));
	form.dwStyle = CFS_POINT;
	form.ptCurrentPos.x = x;
	form.ptCurrentPos.y = y;
	ImmSetCompositionWindow(imc, &form);
}

// 0x451aa0
void RFIme::SetCompositionArea(int x, int y, const RECT& area) {
	RFImeContext imc(m_imeWnd);
	COMPOSITIONFORM form;
	memset(&form, 0, sizeof(form));
	form.dwStyle = CFS_RECT;
	form.ptCurrentPos.x = x;
	form.ptCurrentPos.y = y;
	form.rcArea = area;
	ImmSetCompositionWindow(imc, &form);
}

// 0x451b30
void RFIme::SetCandidatePos(int pos) {
	RFImeContext imc(m_imeWnd);
	CANDIDATEFORM form;
	memset(&form, 0, sizeof(form));
	form.dwStyle = CFS_CANDIDATEPOS;
	form.ptCurrentPos.x = pos;
	form.ptCurrentPos.y = pos;
	ImmSetCandidateWindow(imc, &form);
}

// 0x451bb0
void RFIme::EndComposition(bool complete) {
	RFImeContext imc(m_imeWnd);
	if (complete) {
		ImmNotifyIME(imc, NI_COMPOSITIONSTR, CPS_COMPLETE, 0);
	} else {
		ImmNotifyIME(imc, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
		m_results.clear();
		OnImeEndComposition();
	}
}

// 0x451c60
void RFIme::QueueResult() {
	wchar_t composition[256] = {0};
	wchar_t result[256] = {0};
	if (m_composing) {
		int length = m_composition.size();
		if (length > 0) {
			GetCompositionString(composition, length);
		}
		RFImeResult item(composition, true);
		m_results.push_back(item);
	} else {
		int length = m_resultString.size();
		if (length > 0) {
			GetResultString(length, result);
			RFImeResult item(result, false);
			m_results.push_back(item);
		}
	}
}

// 0x451f20
int RFIme::GetCompositionString(wchar_t* buffer, int length) {
	int size = m_composition.size();
	if (buffer && size > 0 && length > 0 && size >= length) {
		memcpy(buffer, m_composition.c_str(), length * sizeof(wchar_t));
		m_composition.clear();
	}
	return size;
}

// 0x451f70
int RFIme::GetResultString(int length, wchar_t* buffer) {
	int size = m_resultString.size();
	if (buffer && size > 0 && size >= length) {
		memcpy(buffer, m_resultString.c_str(), length * sizeof(wchar_t));
		m_resultString.clear();
	}
	return size;
}

// 0x451fc0
void RFIme::ReadComposition(LPARAM flags) {
	RFImeContext imc(m_imeWnd);
	if (imc) {
		LONG size = ImmGetCompositionStringW(imc, flags, NULL, 0);
		if (size > 0) {
			wchar_t* buffer = (wchar_t*)_alloca(size);
			ImmGetCompositionStringW(imc, flags, buffer, size);
			if (HasCompositionString(flags)) {
				m_composition.assign(buffer, buffer + size / sizeof(wchar_t));
			} else {
				m_resultString.assign(buffer, buffer + size / sizeof(wchar_t));
			}
		}
	}
}

// 0x452090
bool RFIme::OnInputLangChange(WPARAM wParam, LPARAM lParam) {
	UpdateLanguage((HKL)lParam);
	UpdateConversionStatus();
	return true;
}

// 0x4520b0
bool RFIme::OnImeSetContext(WPARAM wParam, LPARAM* lParam) {
	return false;
}

// 0x4520c0
bool RFIme::OnImeStartComposition() {
	RFManager::GetInstance().GetInterface()->OnInputMethodStateChanged(InputMethodStateTypes::StartComposition);
	m_composing = true;
	return true;
}

// 0x452100
bool RFIme::OnImeComposition(WPARAM wParam, LPARAM lParam) {
	if (HasResultString(lParam)) {
		m_composing = false;
		ReadComposition(GCS_RESULTSTR);
	}
	return true;
}

// 0x452130
bool RFIme::OnImeEndComposition() {
	m_composing = false;
	m_composition.erase();
	m_resultString.erase();
	RFManager::GetInstance().GetInterface()->OnInputMethodStateChanged(InputMethodStateTypes::EndComposition);
	return true;
}

// 0x452190
bool RFIme::OnImeNotify(WPARAM wParam, LPARAM lParam) {
	switch (wParam) {
	case IMN_CLOSECANDIDATE:
		OnCloseCandidate();
		break;
	case IMN_OPENCANDIDATE:
		OnOpenCandidate();
		break;
	case IMN_SETCONVERSIONMODE:
		UpdateConversionStatus();
		break;
	}
	return true;
}

// 0x4521d0
void RFIme::OnOpenCandidate() {
	RFManager::GetInstance().GetInterface()->OnInputMethodStateChanged(InputMethodStateTypes::OpenCandidate);
}

// 0x452200
void RFIme::OnCloseCandidate() {
	RFManager::GetInstance().GetInterface()->OnInputMethodStateChanged(InputMethodStateTypes::CloseCandidate);
}

// 0x452230
bool RFIme::OnImeChar(WPARAM wParam, LPARAM lParam) {
	return false;
}

// 0x452240
bool RFIme::OnChar(WPARAM wParam, LPARAM lParam) {
	switch (wParam) {
	case VK_BACK:
	case VK_TAB:
	case VK_RETURN:
	case VK_ESCAPE:
		return true;
	}
	if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
		wchar_t character;
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)&wParam, 1, &character, 1);
		m_resultString.erase();
		m_resultString.insert(m_resultString.end(), character);
	}
	return true;
}

// 0x452300
int RFIme::UpdateLanguage(HKL layout) {
	WORD language = LOWORD(layout);
	m_language = LocaleIndicatorTypes::None;
	switch (PRIMARYLANGID(language)) {
	case LANG_JAPANESE:
		m_language = LocaleIndicatorTypes::Japanese;
		return SHIFTJIS_CHARSET;
	case LANG_KOREAN:
		m_language = LocaleIndicatorTypes::Korean;
		return HANGUL_CHARSET;
	case LANG_CHINESE:
		switch (SUBLANGID(language)) {
		case SUBLANG_CHINESE_TRADITIONAL:
			m_language = LocaleIndicatorTypes::TraditionalChinese;
			return CHINESEBIG5_CHARSET;
		case SUBLANG_CHINESE_SIMPLIFIED:
			m_language = LocaleIndicatorTypes::SimplifiedChinese;
			return GB2312_CHARSET;
		}
		break;
	case LANG_GREEK:
		return GREEK_CHARSET;
	case LANG_TURKISH:
		return TURKISH_CHARSET;
	case LANG_HEBREW:
		return HEBREW_CHARSET;
	case LANG_ARABIC:
		return ARABIC_CHARSET;
	case LANG_ESTONIAN:
	case LANG_LATVIAN:
	case LANG_LITHUANIAN:
		return BALTIC_CHARSET;
	case LANG_THAI:
		return THAI_CHARSET;
	case LANG_CZECH:
	case LANG_HUNGARIAN:
	case LANG_POLISH:
	case LANG_CROATIAN:
	case LANG_MACEDONIAN:
	case LANG_ROMANIAN:
	case LANG_SLOVAK:
	case LANG_SLOVENIAN:
		return EASTEUROPE_CHARSET;
	case LANG_RUSSIAN:
	case LANG_BELARUSIAN:
	case LANG_BULGARIAN:
	case LANG_UKRAINIAN:
		return RUSSIAN_CHARSET;
	case LANG_VIETNAMESE:
		return VIETNAMESE_CHARSET;
	}
	return ANSI_CHARSET;
}

// 0x452510 (the switch's cases sit before it, 0x452410)
int RFIme::GetCodePage(HKL layout) {
	switch (UpdateLanguage(layout)) {
	case SHIFTJIS_CHARSET:
		return 932;
	case HANGUL_CHARSET:
		return 949;
	case GB2312_CHARSET:
		return 936;
	case CHINESEBIG5_CHARSET:
		return 950;
	case GREEK_CHARSET:
		return 1253;
	case TURKISH_CHARSET:
		return 1254;
	case HEBREW_CHARSET:
		return 1255;
	case ARABIC_CHARSET:
		return 1256;
	case BALTIC_CHARSET:
		return 1257;
	case THAI_CHARSET:
		return 874;
	case EASTEUROPE_CHARSET:
		return 1250;
	case VIETNAMESE_CHARSET:
		return 1258;
	}
	return 1252;
}

// 0x4537e0 (out-of-line copy, `flags` in eax under LTCG): GCS_COMPATTR test
static bool HasCompositionAttributes(LPARAM flags) {
	return (flags & GCS_COMPATTR) != 0;
}

// 0x4537f0
RFCustomIme::RFCustomIme(HWND parent)
	: RFIme(parent),
	  m_candidateOpen(false),
	  m_closeSystemCandidates(true),
	  m_pageCandidateCount(0),
	  m_keepNativeMode(false),
	  m_nativeMode(false) {
	m_targetStart = 0;
	m_targetEnd = 0;
}

// 0x453890 (inlined into the scalar deleting destructor 0x453830)
RFCustomIme::~RFCustomIme() {
}

// 0x4538e0
void RFCustomIme::SaveNativeMode(bool keep) {
	m_keepNativeMode = keep;
	m_nativeMode = IsNativeMode();
}

// 0x4538f0
int* RFCustomIme::GetTargetClause() {
	return &m_targetStart;
}

// 0x453900
int RFCustomIme::GetPageCandidateCount() {
	return m_pageCandidateCount;
}

// 0x453910
bool RFCustomIme::IsComposing() {
	return m_composing;
}

// 0x453920
bool RFCustomIme::IsCandidateOpen() {
	return m_candidateOpen;
}

// 0x453930
void RFCustomIme::UpdateTargetClause() {
	RFImeContext imc(m_imeWnd);
	if (imc) {
		LONG size = ImmGetCompositionStringW(imc, GCS_COMPATTR, NULL, 0);
		if (size > 0) {
			char* attributes = (char*)_alloca(size + 1);
			memset(attributes, 0, size + 1);
			ImmGetCompositionStringW(imc, GCS_COMPATTR, attributes, size);
			int start;
			for (start = 0; start < size; start++) {
				if (attributes[start] == ATTR_TARGET_CONVERTED
					|| attributes[start] == ATTR_TARGET_NOTCONVERTED) {
					break;
				}
			}
			int end;
			for (end = start; end < size; end++) {
				if (attributes[end] != attributes[start]) {
					break;
				}
			}
			m_targetStart = start;
			m_targetEnd = end;
		}
	}
}

// 0x453a00
void RFCustomIme::UpdateCandidates() {
	RFImeContext imc(m_imeWnd);
	if (imc) {
		int size = ImmGetCandidateListW(imc, 0, NULL, 0);
		if (size > 0) {
			m_candidates.clear();
			CANDIDATELIST* list = (CANDIDATELIST*)_alloca(size);
			ImmGetCandidateListW(imc, 0, list, size);
			if (list) {
				DWORD pageStart = list->dwSelection / list->dwPageSize * list->dwPageSize;
				DWORD i;
				for (i = 0; pageStart + i < list->dwCount && i < list->dwPageSize; i++) {
					std::wstring candidate((const wchar_t*)((char*)list + list->dwOffset[pageStart + i]));
					m_candidates.push_back(candidate);
				}
				m_pageCandidateCount = i;
			}
		}
	}
}

// 0x453bc0
bool RFCustomIme::OnImeSetContext(WPARAM wParam, LPARAM* lParam) {
	m_contextActive = wParam == 1;
	*lParam = 0;
	return true;
}

// 0x453be0
bool RFCustomIme::OnImeStartComposition() {
	RFManager::GetInstance().GetInterface()->OnInputMethodStateChanged(InputMethodStateTypes::StartComposition);
	m_composing = true;
	return false;
}

// 0x453c20
bool RFCustomIme::OnImeComposition(WPARAM wParam, LPARAM lParam) {
	if (HasCompositionString(lParam)) {
		m_composing = true;
		ReadComposition(GCS_COMPSTR);
	}
	if (HasResultString(lParam)) {
		m_composing = false;
		ReadComposition(GCS_RESULTSTR);
	}
	if (HasCompositionAttributes(lParam)) {
		UpdateTargetClause();
	}
	return false;
}

// 0x453c80
bool RFCustomIme::OnImeEndComposition() {
	ClearCandidates();
	m_composing = false;
	m_composition.erase();
	m_resultString.erase();
	RFManager::GetInstance().GetInterface()->OnInputMethodStateChanged(InputMethodStateTypes::EndComposition);
	return false;
}

// 0x453d20
bool RFCustomIme::OnImeNotify(WPARAM wParam, LPARAM lParam) {
	switch (wParam) {
	case IMN_SETCONVERSIONMODE:
		UpdateConversionStatus();
		if (m_keepNativeMode) {
			SetNativeMode(m_nativeMode);
		}
		break;
	case IMN_OPENCANDIDATE:
		if (m_closeSystemCandidates) {
			RFImeContext imc(m_imeWnd);
			ImmNotifyIME(imc, NI_CLOSECANDIDATE, 0, 0);
			return false;
		}
		OnOpenCandidate();
		// fall through: the opened list is read like a changed one
	case IMN_CHANGECANDIDATE:
		m_candidateOpen = true;
		UpdateCandidates();
		return false;
	case IMN_CLOSECANDIDATE:
		OnCloseCandidate();
		return false;
	}
	return true;
}

// 0x453dd0
void RFCustomIme::OnCloseCandidate() {
	ClearCandidates();
	RFManager::GetInstance().GetInterface()->OnInputMethodStateChanged(InputMethodStateTypes::CloseCandidate);
}

// 0x453e50
bool RFCustomIme::OnImeChar(WPARAM wParam, LPARAM lParam) {
	return true;
}

// 0x453e60
void RFCustomIme::ClearCandidates() {
	m_candidateOpen = false;
	m_targetStart = 0;
	m_targetEnd = 0;
	m_pageCandidateCount = 0;
	m_candidates.clear();
}
