// Original: d:\build\custom\temp\code\client\device\wime\wime.cpp
#define WIME_WIME_INLINE
#include "wime.h"

// wime.cpp:31
WIme::WIme(void) {
}

// wime.cpp:45
WIme::WIme(HWND__ *hwnd_) {
	m_hWnd = 0;
	m_bActive = false;

	m_hKeyLayout = 0;
	m_nState = 0;

	m_nCompLen = 0;

	m_property = 0;
	m_charWidth = 0;
	m_charHeight = 0;
	m_hIMC = 0;
	m_dwConvMode = 0;

	memset(m_TextComp, 0, sizeof(m_TextComp));

	old_current = 0;
	current = 0;
	pos = 0;

	bUpdated = false;
	dwLastInputTime = timeGetTime();

	InitIme(hwnd_);
}

// wime.cpp:95
WIme::~WIme(void) {
}

// wime.cpp:126
void WIme::InitIme(HWND__ *hWnd) {
	if (hWnd) {
		m_hWnd = hWnd;
	}

	m_hKeyLayout = GetKeyboardLayout(0);
	m_property = ImmGetProperty(m_hKeyLayout, IGP_PROPERTY);

	ClearData();
}

// wime.cpp:233
void WIme::ClearData(void) {
	for (int i = 0; i < 32; i++) {
		m_hwndCand[i] = 0;
		m_candList[i] = 0;
	}

	m_nState = 0; m_nCompLen = 0;

	old_current = 0; current = 0; pos = 0;
}

// wime.cpp:255
int WIme::WinProc(unsigned int msg, unsigned long wparam, unsigned long lparam) {
	if (m_bActive) {
		switch (msg) {
		case WM_IME_STARTCOMPOSITION:
			OnStartComposition(wparam, lparam);

			return 1;

		case WM_IME_ENDCOMPOSITION:
			OnEndComposition(wparam, lparam);
			return 1;

		case WM_IME_COMPOSITION:
			OnComposition(wparam, lparam);
			return 1;

		case WM_IME_SETCONTEXT:
			OnSetContext(wparam, lparam);
			return 1;

		case WM_IME_NOTIFY:
			return OnNotify(wparam, lparam);

		case WM_INPUTLANGCHANGE:
			OnInputLangChange(wparam, lparam);
			break;

		case WM_IME_COMPOSITIONFULL:
			OnCompositionFull(wparam, lparam);
			return 1;

		case WM_IME_CONTROL:
			OnControl(wparam, lparam);
			return 1;

		case WM_KEYDOWN:
			if (LOWORD(wparam) >= VK_PRIOR && LOWORD(wparam) <= VK_DELETE) {
				OnChar(wparam | 0x80);
			}
			return 1;

		case WM_CHAR:
			OnChar(wparam);
			break;
		}
	}

	return 0;
}

// wime.cpp:372
void WIme::OnStartComposition(unsigned int dwCommand, long dwData) {
	if (Check()) {
		m_nCompLen = 0;
		m_nState |= 1;
		current = 0;
		pos = 0;
	}
}

// wime.cpp:386
void WIme::OnInputLangChange(unsigned int dwCommand, long dwData) {
	tagCANDIDATEFORM cf;

	if (ImmIsIME(m_hKeyLayout) && (m_property & IME_PROP_AT_CARET)) {
		ClearData();
	}
	InitIme(0);

	if (IME_Enter()) {
		for (int i = 0; i < 32; i++) {
			if (!(m_property & IME_PROP_AT_CARET)) {
				if (ImmGetCandidateWindow(m_hIMC, i, &cf)) {
					if (cf.dwStyle != CFS_DEFAULT) {
						cf.dwStyle = CFS_DEFAULT;
						ImmSetCandidateWindow(m_hIMC, &cf);
					}
				}
			}
		}
		Leave();
	}
}

// wime.cpp:439
void WIme::OnSetContext(unsigned int dwCommand, long dwData) {
	if (m_property & IME_PROP_AT_CARET) {
		dwData &= ~(ISC_SHOWUICOMPOSITIONWINDOW | ISC_SHOWUICANDIDATEWINDOW);
	}
	DefWindowProc(m_hWnd, WM_IME_SETCONTEXT, dwCommand, dwData);
}

// wime.cpp:448
void WIme::OnComposition(unsigned int dwCommand, long dwData) {
	if (Check()) {
		if (dwData & GCS_RESULTSTR) {
			GetResultString();
		} else if (dwData & GCS_COMPSTR) {
			GetCompString(dwData);
		}
	}
}

// wime.cpp:466
void WIme::OnEndComposition(unsigned int dwCommand, long dwData) {
	if (Check()) {
		m_nState &= ~1;
		m_nCompLen = 0;

		memset(m_TextComp, 0, sizeof(m_TextComp));
	}
}

// wime.cpp:488
void WIme::OnCompositionFull(unsigned int dwCommand, long dwData) {
}

// wime.cpp:494
int WIme::OnNotify(unsigned int dwCommand, long dwData) {
	unsigned long dwSenMode;

	switch (dwCommand) {
	case IMN_CHANGECANDIDATE:
	case IMN_CLOSECANDIDATE:
	case IMN_OPENCANDIDATE:
		return 1;

	case IMN_SETCONVERSIONMODE:
	case IMN_SETSENTENCEMODE:
		return 0;

	case IMN_SETOPENSTATUS:
		IME_Enter();
		ImmGetConversionStatus(m_hIMC, &m_dwConvMode, &dwSenMode);
		Leave();

		return 1;
	}

	return 0;
}

// wime.cpp:628
void WIme::OnControl(unsigned int dwCommand, long dwData) {
}

// wime.cpp:634
void WIme::OnChar(unsigned int nChar) {
	char str[2];

	switch (nChar) {
	case VK_BACK:
		ringBuf[current & 0x3FF] = 0x08; current++;
		break;

	case VK_TAB:
		ringBuf[current & 0x3FF] = 0x09; current++;
		break;

	case VK_RETURN:
		ringBuf[current & 0x3FF] = 0x0D; current++;
		break;

	case VK_ESCAPE:
		ringBuf[current & 0x3FF] = 0x1B; current++;
		break;

	case VK_PRIOR | 0x80:
		ringBuf[current & 0x3FF] = 0x02; current++;
		break;

	case VK_NEXT | 0x80:
		ringBuf[current & 0x3FF] = 0x03; current++;
		break;

	case VK_END | 0x80:
		ringBuf[current & 0x3FF] = 0x1A; current++;
		break;

	case VK_HOME | 0x80:
		ringBuf[current & 0x3FF] = 0x0C; current++;
		break;

	case VK_LEFT | 0x80:
		ringBuf[current & 0x3FF] = 0x04; current++;
		break;

	case VK_RIGHT | 0x80:
		ringBuf[current & 0x3FF] = 0x05; current++;
		break;

	case VK_UP | 0x80:
		ringBuf[current & 0x3FF] = 0x06; current++;
		return;

	case VK_DOWN | 0x80:
		ringBuf[current & 0x3FF] = 0x07; current++;
		return;

	case VK_DELETE | 0x80:
		ringBuf[current & 0x3FF] = 0x01; current++;
		return;
	}

	if (nChar >= 0x20 && nChar <= 0x7E) {
		str[0] = (char)nChar;
		str[1] = 0;
		PutString(str);
	}
}

// wime.cpp:772
void WIme::PutString(char *str) {
	int len = strlen(str);

	for (int i = 0; i < len; i++) {
		ringBuf[current & 0x3FF] = str[i]; current++;
	}
}

// wime.cpp:795
int WIme::GetResultString(void) {
	static char str[128];

	if (IME_Enter()) {
		unsigned long dwLen = ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, 0, 0);
		if (dwLen > 0) {
			if (dwLen > 127) dwLen = 127;

			ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, str, dwLen);
			str[dwLen] = 0;

			ProcessResultString(str);
		}
		Leave();
	}

	return 1;
}

// wime.cpp:818
int WIme::GetCompString(long flag) {
	static char comp[256];
	static char attr[256];

	if (!IME_Enter()) {
		return 0;
	}

	unsigned long dwLen = ImmGetCompositionString(m_hIMC, GCS_COMPSTR, 0, 0);
	if (dwLen > 0) {
		ImmGetCompositionString(m_hIMC, GCS_COMPSTR, comp, dwLen);
		comp[dwLen] = 0;

		if (flag & GCS_COMPATTR) {
			unsigned long dwAttrLen = ImmGetCompositionString(m_hIMC, GCS_COMPATTR, 0, 0);
			if (dwAttrLen > 0) {
				ImmGetCompositionString(m_hIMC, GCS_COMPATTR, attr, dwAttrLen);
				attr[dwAttrLen] = 0;
			}
		}

		ProcessCompString(comp, attr);

		m_nCompLen = dwLen;
	}

	Leave();

	return 1;
}

// wime.cpp:894
void WIme::ProcessCompString(char *str, char *strAttr) {
	if (lstrlen(str) <= 2) {
		strcpy(m_TextComp, str);
	}
}

// wime.cpp:905
void WIme::ProcessResultString(char *str) {
	PutString(str);
}

// wime.cpp:910
WInputDev *WIme::MakeClone(char *name, HWND__ *hWnd) {
	return new WIme(hWnd);
}

// wime.cpp:915
int WIme::GetState(int sort, int n) {
	if (sort == 3) {
		if (n == -1) {
			return 1;
		}
		if (n == -2) {
			int code = 0;
			if (Check()) strcpy((char *)&code, m_TextComp);

			return code;
		}

		if (pos < current) {
			unsigned char ch = ringBuf[pos++ & 0x3FF];

			if (ch >= 0x80) {
				return ch | (ringBuf[pos++ & 0x3FF] << 8);
			}

			return ch;
		}
	}

	return 0;
}

// wime.cpp:1074
void WIme::SetAlphaNumericMode(bool alnum) {
	unsigned long dwSenMode;

	if (m_hIMC == 0) {
		IME_Enter();
		ImmGetConversionStatus(m_hIMC, &m_dwConvMode, &dwSenMode);
		ImmSetConversionStatus(m_hIMC, alnum ? IME_CMODE_ALPHANUMERIC : IME_CMODE_NATIVE, dwSenMode);
		Leave();
	} else {
		ImmGetConversionStatus(m_hIMC, &m_dwConvMode, &dwSenMode);
		ImmSetConversionStatus(m_hIMC, alnum ? IME_CMODE_ALPHANUMERIC : IME_CMODE_NATIVE, dwSenMode);
	}
}

// wime.cpp:1093
bool WIme::IsAlphaNumericMode(void) {
	unsigned long dwSenMode;

	if (m_hIMC == 0) {
		IME_Enter();
		ImmGetConversionStatus(m_hIMC, &m_dwConvMode, &dwSenMode);
		Leave();
	}

	return m_dwConvMode == IME_CMODE_ALPHANUMERIC;
}

// wime.cpp:1107
void WIme::Reset(void) {
	SetActive(true);

	ImmNotifyIME(m_hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0);

	ClearData();

	memset(m_TextComp, 0, sizeof(m_TextComp));
}

// wime.cpp:1128
void WIme::Update(unsigned long timeStamp) {
	if (current != old_current) {
		old_current = current;
		bUpdated = true;
		dwLastInputTime = timeGetTime();
	} else {
		bUpdated = false;
	}
}
