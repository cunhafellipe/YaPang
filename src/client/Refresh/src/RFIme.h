// Original: client\Refresh\src\RFIme.h (JP 983 exe; RFIme, RFCustomIme and
// RFInputMethodService are not exported, the file name is assumed from the
// class)
#ifndef PG_CLIENT_REFRESH_SRC_RFIME_H
#define PG_CLIENT_REFRESH_SRC_RFIME_H

#include <windows.h>
#include <imm.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "RFEventArgs.h"
#include "RFSingleton.h"
#include "defines.h"
#include "padding.h"

// The input-method interface the manager drives (vtable 0xd65c24, size 4).
// Only RestoreFocus and the destructor have a body; RFIme implements the
// rest over the Win32 IMM.
class RFInputMethodService {
public:
	virtual void Enable(bool enable) = 0;                  // [0]
	virtual bool IsEnabled() = 0;                          // [1]
	virtual void RestoreFocus() {                          // [2] 0x450fe0
	}
	virtual void SetNativeMode(bool native) = 0;           // [3]
	virtual bool IsNativeMode() = 0;                       // [4]
	virtual void GetStatus(int* language, int* conversion) = 0;  // [5]
	virtual void ActivateContext(bool activate) = 0;       // [6]
	virtual void SetCompositionPos(int x, int y) = 0;      // [7]
	virtual void EndComposition(bool complete) = 0;        // [8]
	virtual int PopResult(wchar_t* buffer, int length, bool* composing) = 0;  // [9]
	virtual int GetResultCount() = 0;                      // [10]
	// [11] scalar deleting destructor 0x451000; the body (0x4512b0, called
	// with `this` in eax under LTCG) is inlined into ~RFIme
	virtual ~RFInputMethodService() {
	}
};
CHECK_SIZE(RFInputMethodService, 0x4);

// The input context of a window for one scope (the name is ours): every
// IMM user of RFIme gets it on entry and releases it on exit, inline (an EH
// state of its own where the scope can throw, EndComposition 0x451bb0).
class RFImeContext {
public:
	RFImeContext(HWND wnd)                  // 0x451030
		: m_imc(NULL), m_wnd(wnd) {
		m_imc = ImmGetContext(wnd);
	}
	~RFImeContext() {                       // 0x451050
		if (m_imc) {
			ImmReleaseContext(m_wnd, m_imc);
		}
	}
	operator HIMC() const {                 // 0x451070
		return m_imc;
	}

private:
	HIMC m_imc;                             // 0x0
	HWND m_wnd;                             // 0x4
};

// A finished composition string queued for the focused edit control
// (RFIme::m_results, 0x20 bytes).
struct RFImeResult {
	RFImeResult(const std::wstring& text_, bool composing_) {  // 0x4510d0
		text = text_;
		composing = composing_;
	}

	std::wstring text;                      // 0x00
	bool composing;                         // 0x1c (an unfinished composition, handed back by PopResult)
};
CHECK_SIZE(RFImeResult, 0x20);

// The system IME (vtable 0xd65c5c, size 0x68): a hidden child window of the
// game window ("ImeWndClass", window proc 0x451370) owns the input context
// and receives the WM_IME_* messages, which it dispatches to slots 12-21.
// Built by RFManager::InitializeInputMethod 0x418670 when the custom IME is
// not asked for.
class RFIme: public RFInputMethodService {
	LAYOUT_CHECK_FRIEND;

public:
	RFIme(HWND parent);                     // 0x451130

	// 0x451600: m_enabled = enable, EndComposition(false),
	// ActivateContext(enable), drops the queued results, focuses the IME or
	// the parent window
	virtual void Enable(bool enable);                      // [0] 0x451600
	virtual bool IsEnabled();                              // [1] 0x451090
	virtual void RestoreFocus();                           // [2] 0x451660
	virtual void SetNativeMode(bool native);               // [3] 0x451680
	virtual bool IsNativeMode();                           // [4] 0x4516e0
	virtual void GetStatus(int* language, int* conversion);  // [5] 0x451740
	virtual void ActivateContext(bool activate);           // [6] 0x451870
	virtual void SetCompositionPos(int x, int y);          // [7] 0x451a20
	virtual void EndComposition(bool complete);            // [8] 0x451bb0
	virtual int PopResult(wchar_t* buffer, int length, bool* composing);  // [9] 0x4518b0
	virtual int GetResultCount();                          // [10] 0x451a10
	virtual ~RFIme();                                      // [11] scalar deleting destructor 0x451280; plain 0x4512c0
	virtual bool OnInputLangChange(WPARAM wParam, LPARAM lParam);       // [12] 0x452090 (WM_INPUTLANGCHANGE)
	virtual bool OnImeSetContext(WPARAM wParam, LPARAM* lParam);        // [13] 0x4520b0 (WM_IME_SETCONTEXT)
	virtual bool OnImeStartComposition();                  // [14] 0x4520c0 (WM_IME_STARTCOMPOSITION)
	virtual bool OnImeComposition(WPARAM wParam, LPARAM lParam);        // [15] 0x452100 (WM_IME_COMPOSITION)
	virtual bool OnImeEndComposition();                    // [16] 0x452130 (WM_IME_ENDCOMPOSITION)
	virtual bool OnImeNotify(WPARAM wParam, LPARAM lParam);             // [17] 0x452190 (WM_IME_NOTIFY)
	// [18], [19]: OnImeNotify's IMN_OPENCANDIDATE and IMN_CLOSECANDIDATE;
	// they report input-method state 2 and 3 to the interface
	virtual void OnOpenCandidate();                            // [18] 0x4521d0
	virtual void OnCloseCandidate();                            // [19] 0x452200
	virtual bool OnImeChar(WPARAM wParam, LPARAM lParam);  // [20] 0x452230 (WM_IME_CHAR)
	virtual bool OnChar(WPARAM wParam, LPARAM lParam);     // [21] 0x452240 (WM_CHAR)

	// 0x451c60: queues the composition (while composing) or the finished
	// string; the window proc calls it after each composition message
	void QueueResult();
	// 0x451350: resizes the "ImeWndClass" window
	void Resize(int width, int height);
	// 0x451aa0 (no caller; `this` pushed, `area` in esi under LTCG): the
	// composition window at (x, y) inside `area` (CFS_RECT)
	void SetCompositionArea(int x, int y, const RECT& area);
	// 0x451b30 (no caller; `pos` in ebx under LTCG): the candidate window
	// at (pos, pos), CFS_CANDIDATEPOS
	void SetCandidatePos(int pos);

protected:
	// 0x452300 (register arguments under LTCG): m_language from the
	// keyboard layout's primary language
	// and returns the layout's character set
	int UpdateLanguage(HKL layout);
	// 0x452510 (no caller; register arguments under LTCG, its switch body
	// placed before it at 0x452410): the ANSI code page of the layout's
	// character set
	int GetCodePage(HKL layout);
	// 0x451760: m_conversion from the context's conversion status, raises
	// RFManager::EventInputMethodStatusChanged
	void UpdateConversionStatus();
	// 0x451fc0: reads the composition strings GCS_* of `flags`
	void ReadComposition(LPARAM flags);
	// 0x451f20: copies `length` characters of the composition into
	// `buffer` and clears it; returns the composition's length
	int GetCompositionString(wchar_t* buffer, int length);
	// 0x451f70: the same for the finished string
	int GetResultString(int length, wchar_t* buffer);

	HWND m_parentWnd;                       // 0x04 (the game window)
	bool m_enabled;                         // 0x08
	bool m_contextActive;                   // 0x09 (RFCustomIme: WM_IME_SETCONTEXT's wParam == 1)
	bool m_composing;                       // 0x0a (between WM_IME_STARTCOMPOSITION and the result)
	std::vector<RFImeResult> m_results;     // 0x0c
	std::wstring m_composition;             // 0x1c
	std::wstring m_resultString;            // 0x38 (the finished string: GCS_RESULTSTR, OnChar's character)
	HIMC m_savedContext;                    // 0x54 (ActivateContext(false) detaches it, true restores it)
	// 0x58: the constructor sets it to NULL between the two handles beside
	// it; no RFIme or RFCustomIme function (nor the window procedure, nor
	// RFManager::InitializeInputMethod) reads or writes it again
	void* m_unusedHandle;                   // 0x58
	HWND m_imeWnd;                          // 0x5c ("ImeWndClass" child)
	LocaleIndicatorTypes::Type m_language;  // 0x60 (UpdateLanguage)
	uint32_t m_conversion;                  // 0x64 (UpdateConversionStatus: 1 native, 2 full shape, 4 katakana)
};
CHECK_SIZE(RFIme, 0x68);
BEGIN_LAYOUT_CHECK(RFIme)
	CHECK_FIELD(RFIme, m_parentWnd, 0x04);
	CHECK_FIELD(RFIme, m_enabled, 0x08);
	CHECK_FIELD(RFIme, m_composing, 0x0a);
	CHECK_FIELD(RFIme, m_results, 0x0c);
	CHECK_FIELD(RFIme, m_composition, 0x1c);
	CHECK_FIELD(RFIme, m_resultString, 0x38);
	CHECK_FIELD(RFIme, m_savedContext, 0x54);
	CHECK_FIELD(RFIme, m_unusedHandle, 0x58);
	CHECK_FIELD(RFIme, m_imeWnd, 0x5c);
	CHECK_FIELD(RFIme, m_language, 0x60);
	CHECK_FIELD(RFIme, m_conversion, 0x64);
END_LAYOUT_CHECK

// The library's own IME (vtable 0xd65cbc, size 0x8c): RFIme with its own
// composition and candidate handling (slots 11, 13-17, 19, 20). Built by
// RFManager::InitializeInputMethod 0x418670 (constructor 0x4537f0 inlined
// there by LTCG: 0x68 = 0x100, 0x6c..0x88 zeroed).
class RFCustomIme: public RFIme {
	LAYOUT_CHECK_FRIEND;

public:
	// 0x4537f0 (out-of-line copy, `this` in esi under LTCG; inlined in
	// RFManager::InitializeInputMethod 0x418670)
	RFCustomIme(HWND parent);
	virtual ~RFCustomIme();                                // [11] scalar deleting destructor 0x453830; plain 0x453890
	virtual bool OnImeSetContext(WPARAM wParam, LPARAM* lParam);        // [13] 0x453bc0
	virtual bool OnImeStartComposition();                  // [14] 0x453be0
	virtual bool OnImeComposition(WPARAM wParam, LPARAM lParam);        // [15] 0x453c20
	virtual bool OnImeEndComposition();                    // [16] 0x453c80
	virtual bool OnImeNotify(WPARAM wParam, LPARAM lParam);             // [17] 0x453d20
	virtual void OnCloseCandidate();                            // [19] 0x453dd0
	virtual bool OnImeChar(WPARAM wParam, LPARAM lParam);  // [20] 0x453e50

	// 0x4538e0 (register arguments under LTCG): `keep` asks
	// IMN_SETCONVERSIONMODE to restore the current native mode
	void SaveNativeMode(bool keep);
	// 0x4538f0 (`this` in eax under LTCG; no caller): the target clause,
	// m_targetStart followed by m_targetEnd (name ours)
	int* GetTargetClause();
	int GetPageCandidateCount();            // 0x453900
	bool IsComposing();                     // 0x453910
	bool IsCandidateOpen();                 // 0x453920

protected:
	// 0x453930: m_targetStart/End from the composition attributes (the
	// first run of target characters)
	void UpdateTargetClause();
	// 0x453a00 (`this` pushed under LTCG): m_candidates from the current
	// page of the context's first candidate list
	void UpdateCandidates();
	// 0x453e60 (out-of-line copy, `this` in esi under LTCG; inlined into
	// OnImeEndComposition and OnCloseCandidate): forgets the candidate list
	void ClearCandidates();

	bool m_candidateOpen;                   // 0x68
	bool m_closeSystemCandidates;           // 0x69 (constructor true; OnImeNotify closes the system candidate list on IMN_OPENCANDIDATE)
	UNKNOWN(0x6a, 0x6c);
	int m_pageCandidateCount;               // 0x6c (UpdateCandidates: the candidates copied from the current page)
	bool m_keepNativeMode;                  // 0x70 (SaveNativeMode)
	bool m_nativeMode;                      // 0x71 (SaveNativeMode: IsNativeMode())
	UNKNOWN(0x72, 0x74);
	int m_targetStart;                      // 0x74 (first target character of the composition)
	int m_targetEnd;                        // 0x78 (one past the last)
	std::vector<std::wstring> m_candidates; // 0x7c
};
CHECK_SIZE(RFCustomIme, 0x8c);
BEGIN_LAYOUT_CHECK(RFCustomIme)
	CHECK_FIELD(RFCustomIme, m_candidateOpen, 0x68);
	CHECK_FIELD(RFCustomIme, m_closeSystemCandidates, 0x69);
	CHECK_FIELD(RFCustomIme, m_pageCandidateCount, 0x6c);
	CHECK_FIELD(RFCustomIme, m_keepNativeMode, 0x70);
	CHECK_FIELD(RFCustomIme, m_nativeMode, 0x71);
	CHECK_FIELD(RFCustomIme, m_targetStart, 0x74);
	CHECK_FIELD(RFCustomIme, m_targetEnd, 0x78);
	CHECK_FIELD(RFCustomIme, m_candidates, 0x7c);
END_LAYOUT_CHECK

// The input-method singleton (RFSingleton<RFInputMethodProvider>::_singleton
// at 0xF1DDE0, GetInstance 0x428420; vtable 0xd65d74, size 8): the holder of
// the active service, RFManager::m_inputMethod (InitializeInputMethod
// 0x418670 builds both). RFEdit and RFTextArea call m_service->Enable(bool)
// on focus and enable changes.
class RFInputMethodProvider: public RFSingleton<RFInputMethodProvider> {
	LAYOUT_CHECK_FRIEND;

public:
	RFInputMethodProvider();                // 0x455d00
	virtual ~RFInputMethodProvider();       // [0] 0x455d60 (??_G), plain 0x455d90

	// The forwarders to the active service (none is called: their callers
	// test and call m_service themselves). Under /LTCG `this` comes in eax and
	// the first argument in edx; SetNativeMode, ActivateContext and
	// EndComposition push a constant false (every caller passes false).
	void SetService(RFInputMethodService* service);         // 0x455db0
	void Enable(bool enable);                               // 0x455dc0
	bool IsEnabled();                                       // 0x455de0
	void RestoreFocus();                                    // 0x455e00
	void SetNativeMode(bool native);                        // 0x455e20
	bool IsNativeMode();                                    // 0x455e40
	void GetStatus(int* language, int* conversion);         // 0x455e60
	void ActivateContext(bool activate);                    // 0x455e80
	void SetCompositionPos(int x, int y);                   // 0x455ea0
	void EndComposition(bool complete);                     // 0x455ec0
	int PopResult(wchar_t* buffer, int length, bool* composing);  // 0x455ee0
	int GetResultCount();                                   // 0x455f10

	RFInputMethodService* m_service;        // 0x04 (shares its offset with the empty singleton base)
};
CHECK_SIZE(RFInputMethodProvider, 0x8);
CHECK_FIELD(RFInputMethodProvider, m_service, 0x04);

#endif
