// Original: d:\build\custom\temp\code\client\device\wdinput\dikeyboard.cpp
#include "dikeyboard.h"

#include <string.h>

int DirectInputKeyboard::scan2ascii_without_shift[256];
int DirectInputKeyboard::scan2ascii_with_shift[256];
int DirectInputKeyboard::ascii2scan[256];

// Scan code -> ASCII without shift, with shift (values from the DLL's .data).
DirectInputKeyboard::w_conv_scan_code DirectInputKeyboard::convList[99] = {
	{ DIK_ESCAPE, 27, 27 },
	{ DIK_1, '1', '!' },
	{ DIK_2, '2', '@' },
	{ DIK_3, '3', '#' },
	{ DIK_4, '4', '$' },
	{ DIK_5, '5', '%' },
	{ DIK_6, '6', '^' },
	{ DIK_7, '7', '&' },
	{ DIK_8, '8', '*' },
	{ DIK_9, '9', '(' },
	{ DIK_0, '0', ')' },
	{ DIK_MINUS, '-', '_' },
	{ DIK_EQUALS, '=', '+' },
	{ DIK_BACK, 8, 8 },
	{ DIK_TAB, 9, 9 },
	{ DIK_Q, 'Q', 'Q' },
	{ DIK_W, 'W', 'W' },
	{ DIK_E, 'E', 'E' },
	{ DIK_R, 'R', 'R' },
	{ DIK_T, 'T', 'T' },
	{ DIK_Y, 'Y', 'Y' },
	{ DIK_U, 'U', 'U' },
	{ DIK_I, 'I', 'I' },
	{ DIK_O, 'O', 'O' },
	{ DIK_P, 'P', 'P' },
	{ DIK_LBRACKET, '[', '{' },
	{ DIK_RBRACKET, ']', '}' },
	{ DIK_RETURN, 13, 13 },
	{ DIK_LCONTROL, 30, 30 },
	{ DIK_A, 'A', 'A' },
	{ DIK_S, 'S', 'S' },
	{ DIK_D, 'D', 'D' },
	{ DIK_F, 'F', 'F' },
	{ DIK_G, 'G', 'G' },
	{ DIK_H, 'H', 'H' },
	{ DIK_J, 'J', 'J' },
	{ DIK_K, 'K', 'K' },
	{ DIK_L, 'L', 'L' },
	{ DIK_SEMICOLON, ';', ':' },
	{ DIK_APOSTROPHE, '\'', '"' },
	{ DIK_GRAVE, '`', '~' },
	{ DIK_LSHIFT, 129, 129 },
	{ DIK_BACKSLASH, '\\', '|' },
	{ DIK_Z, 'Z', 'Z' },
	{ DIK_X, 'X', 'X' },
	{ DIK_C, 'C', 'C' },
	{ DIK_V, 'V', 'V' },
	{ DIK_B, 'B', 'B' },
	{ DIK_N, 'N', 'N' },
	{ DIK_M, 'M', 'M' },
	{ DIK_COMMA, ',', '<' },
	{ DIK_PERIOD, '.', '>' },
	{ DIK_SLASH, '/', '?' },
	{ DIK_RSHIFT, 130, 130 },
	{ DIK_LMENU, 28, 28 },
	{ DIK_SPACE, ' ', ' ' },
	{ DIK_CAPITAL, 11, 11 },
	{ DIK_F1, 14, 14 },
	{ DIK_F2, 15, 15 },
	{ DIK_F3, 16, 16 },
	{ DIK_F4, 17, 17 },
	{ DIK_F5, 18, 18 },
	{ DIK_F6, 19, 19 },
	{ DIK_F7, 20, 20 },
	{ DIK_F8, 21, 21 },
	{ DIK_F9, 22, 22 },
	{ DIK_F10, 23, 23 },
	{ DIK_NUMLOCK, 131, 131 },
	{ DIK_SCROLL, 10, 10 },
	{ DIK_NUMPAD7, 133, 133 },
	{ DIK_NUMPAD8, 134, 134 },
	{ DIK_NUMPAD9, 135, 135 },
	{ DIK_SUBTRACT, 136, 136 },
	{ DIK_NUMPAD4, 137, 137 },
	{ DIK_NUMPAD5, 138, 138 },
	{ DIK_NUMPAD6, 139, 139 },
	{ DIK_ADD, 140, 140 },
	{ DIK_NUMPAD1, 141, 141 },
	{ DIK_NUMPAD2, 142, 142 },
	{ DIK_NUMPAD3, 143, 143 },
	{ DIK_NUMPAD0, 144, 144 },
	{ DIK_F11, 24, 24 },
	{ DIK_F12, 25, 25 },
	{ DIK_F15, 'f', 'f' },
	{ DIK_NUMPADENTER, 145, 145 },
	{ DIK_RCONTROL, 31, 31 },
	{ DIK_SYSRQ, 132, 132 },
	{ DIK_RMENU, 29, 29 },
	{ DIK_HOME, 12, 12 },
	{ DIK_UP, 6, 6 },
	{ DIK_PRIOR, 2, 2 },
	{ DIK_LEFT, 4, 4 },
	{ DIK_RIGHT, 5, 5 },
	{ DIK_END, 26, 26 },
	{ DIK_DOWN, 7, 7 },
	{ DIK_NEXT, 3, 3 },
	{ DIK_INSERT, 127, 127 },
	{ DIK_DELETE, 1, 1 },
	{ DIK_MULTIPLY, 146, 146 },
};

// dikeyboard.cpp:9
DirectInputKeyboard::DirectInputKeyboard(IDirectInput8A *di, HWND__ *hwnd) {
	memset(keyState, 0, sizeof(keyState));

	diKey = NULL;
	diBackup = di;
	InitDevice(hwnd, false);

	memset(scan2ascii_with_shift, 0, sizeof(scan2ascii_with_shift));
	memset(scan2ascii_without_shift, 0, sizeof(scan2ascii_without_shift));
	memset(ascii2scan, 0, sizeof(ascii2scan));

	memset(timebuf, 0, sizeof(timebuf));

	ClearBuffer();

	for (int i = 0; i < sizeof(convList) / sizeof(convList[0]); i++) {
		scan2ascii_with_shift[convList[i].di_scan] = convList[i].asciiwithshift;
		scan2ascii_without_shift[convList[i].di_scan] = convList[i].ascii;
		ascii2scan[convList[i].ascii] = convList[i].di_scan;
	}
}

// dikeyboard.cpp:34
DirectInputKeyboard::~DirectInputKeyboard(void) {
	if (diKey) {
		diKey->Unacquire();
		diKey->Release();
		diKey = NULL;
	}
}

// dikeyboard.cpp:44
bool DirectInputKeyboard::InitDevice(HWND__ *hwnd, bool exclusive) {
	if (!diBackup) {
		return false;
	}

	if (diKey) {
		diKey->Unacquire();
		diKey->Release();
		diKey = NULL;
	}

	if (diBackup->CreateDevice(GUID_SysKeyboard, &diKey, NULL) != DI_OK) {
		return false;
	}

	if (diKey->SetDataFormat(&c_dfDIKeyboard) != DI_OK) {
		return false;
	}

	if (diKey->SetCooperativeLevel(hwnd, (exclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE) | DISCL_FOREGROUND) != DI_OK) {
		return false;
	}

	DIPROPDWORD dipdw;
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.diph.dwObj = 0;
	dipdw.dwData = 64;

	if (diKey->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph) != DI_OK) {
		return false;
	}

	return diKey->Acquire() == DI_OK ? true : false;
}

// dikeyboard.cpp:82
void DirectInputKeyboard::ClearBuffer(void) {
	bufCur = 0;
	passCur = 0;
	asciiCur = 0;
	asciiPos = 0;
	memset(asciiBuf, 0, sizeof(asciiBuf));
	memset(keyState, 0, sizeof(keyState));
}

// dikeyboard.cpp:92
int DirectInputKeyboard::WinProc(unsigned int message, unsigned long wParam, unsigned long lParam) {
	switch (message) {
	case WM_ACTIVATE:
		if (wParam != WA_INACTIVE) {
			Acquire();
			GetDeviceData();
		} else {
			diKey->Unacquire();
		}

		ClearBuffer();

		return 1;
	}

	return 0;
}

// dikeyboard.cpp:112
void DirectInputKeyboard::FlushBuffer(unsigned long timeStamp) {
	Reset();

	for (; passCur < bufCur; passCur++) {
		int index = passCur & 0xFF;
		if (timeStamp < keybuf[index].dwTimeStamp) {
			break;
		}

		int ascii = (keyState[DIK_LSHIFT] || keyState[DIK_RSHIFT]) ? scan2ascii_with_shift[keybuf[index].dwOfs] : scan2ascii_without_shift[keybuf[index].dwOfs];

		if (ascii && keyState[ascii2scan[ascii]]) {
			asciiBuf[asciiPos++ & 0xFF] = ascii;
			timebuf[ascii] = keybuf[index].dwTimeStamp;
		}
	}

	if (bufCur == passCur) {
		passCur = 0;
		bufCur = 0;
	}
}

// dikeyboard.cpp:139
void DirectInputKeyboard::GetDeviceData(void) {
	unsigned long dwItems;
	HRESULT hres;
	DIDEVICEOBJECTDATA dims[64];

	diKey->Acquire();

	do {
		dwItems = 64;
		hres = diKey->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), dims, &dwItems, 0);

		if (hres == DI_OK || hres == DI_BUFFEROVERFLOW) {
			for (unsigned long i = 0; i < dwItems; ) {
				unsigned long count = dwItems < 256 - (bufCur & 0xFF) ? dwItems : 256 - (bufCur & 0xFF);

				memcpy(&keybuf[bufCur & 0xFF], &dims[i], count * sizeof(DIDEVICEOBJECTDATA));

				bufCur += count;
				i += count;
			}
		}
	} while (hres == DI_BUFFEROVERFLOW);
}

// dikeyboard.cpp:168
void DirectInputKeyboard::Update(unsigned long timeStamp) {
	if (!timeStamp || !active) {
		memset(keyState, 0, sizeof(keyState));
		return;
	}

	if (!diKey) {
		return;
	}

	for (int t = 0; t < 5 && diKey->GetDeviceState(sizeof(keyState), keyState) == DIERR_NOTACQUIRED; t++) {
		memset(keyState, 0, sizeof(keyState));
		diKey->Acquire();
	}

	GetDeviceData();

	if (bufCur - passCur > 0) {
		FlushBuffer(timeStamp);
	}
}

// dikeyboard.cpp:203
unsigned long DirectInputKeyboard::GetEventTime(int n) {
	return timebuf[n];
}

// dikeyboard.cpp:208
int DirectInputKeyboard::GetState(int sort, int n) {
	switch (sort) {
	case 3:
		if (n == -1) {
			return 1;
		}

		for (int i = asciiCur; i < asciiPos; i++) {
			if (asciiBuf[i & 0xFF] == n) {
				return 1;
			}
		}

		return 0;

	case 2:
		if (n == -1) {
			return 1;
		}

		return (keyState && keyState[ascii2scan[n]]) ? 1 : 0;
	}

	return 0;
}

// dikeyboard.cpp:235
void DirectInputKeyboard::Reset(void) {
	asciiCur = asciiPos;
}

// dikeyboard.cpp:252
HRESULT DirectInputKeyboard::Acquire(void) {
	return diKey->Acquire();
}
