// Original: d:\build\custom\temp\code\client\device\wdinput\dimouse.cpp
#include "dimouse.h"

#include <mmsystem.h>

// dimouse.cpp:9
DirectInputMouse::DirectInputMouse(IDirectInput8A *di, HWND__ *hwnd) {
	diBackup = di;
	hEvent = NULL;
	active = true;
	diMouse = NULL;
	m_moveMode = false;
	m_myHwnd = hwnd;

	ClearBuffer();

	memset(timebuf, 0, sizeof(timebuf));

	bUpdated = false;
	dwLastInputTime = timeGetTime();

	InitDevice(hwnd, true);
}

// dimouse.cpp:28
DirectInputMouse::~DirectInputMouse(void) {
	if (hEvent) {
		CloseHandle(hEvent);
		hEvent = NULL;
	}

	if (diMouse) {
		diMouse->Unacquire();
		diMouse->Release();
		diMouse = NULL;
	}
}

// dimouse.cpp:44
bool DirectInputMouse::InitDevice(HWND__ *hwnd, bool exclusive) {
	if (diMouse) {
		diMouse->Unacquire();
		diMouse->Release();
		diMouse = NULL;
	}

	if (diBackup->CreateDevice(GUID_SysMouse, &diMouse, NULL) != DI_OK)
		return false;

	if (diMouse->SetDataFormat(&c_dfDIMouse) != DI_OK)
		return false;

	if (diMouse->SetCooperativeLevel(hwnd, (exclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE) | DISCL_FOREGROUND) != DI_OK)
		return false;

	if (hEvent) {
		CloseHandle(hEvent);
		hEvent = NULL;
	}

	hEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
	if (hEvent == NULL)
		return false;

	if (diMouse->SetEventNotification(hEvent) != DI_OK)
		return false;

	DIPROPDWORD dipdw;
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = 16;

	if (diMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph) != DI_OK) {
		if (diMouse) { diMouse->Release(); diMouse = NULL; }
		return false;
	}

	bufCur = passCur = 0;

	return true;
}

// dimouse.cpp:100
void DirectInputMouse::GetDeviceData(void) {
	unsigned long dwItems;
	HRESULT hres;
	DIDEVICEOBJECTDATA dims[16];
	unsigned long i;

	Acquire();

	do {
		dwItems = 16;
		hres = diMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), dims, &dwItems, 0);

		if (hres == DI_OK || hres == DI_BUFFEROVERFLOW) {
			for (i = 0; i < dwItems;) {
				unsigned long n = (dwItems < 256 - (bufCur & 0xFF)) ? dwItems : 256 - (bufCur & 0xFF);

				memcpy(&mousebuf[bufCur & 0xFF], &dims[i], n * sizeof(DIDEVICEOBJECTDATA));

				bufCur += n;
				i += n;
			}
		}
	} while (hres == DI_BUFFEROVERFLOW);
}

// dimouse.cpp:131
unsigned long DirectInputMouse::GetEventTime(int n) {
	return timebuf[n];
}

// dimouse.cpp:136
void DirectInputMouse::FlushBuffer(unsigned long timeStamp) {
	bUpdated = true;
	dwLastInputTime = timeGetTime();

	while (passCur < bufCur) {
		if (timeStamp < mousebuf[passCur & 0xFF].dwTimeStamp)
			break;

		switch (mousebuf[passCur & 0xFF].dwOfs) {
		case DIMOFS_X:
			x += mousebuf[passCur & 0xFF].dwData;
			break;

		case DIMOFS_Y:
			y += mousebuf[passCur & 0xFF].dwData;
			break;

		case DIMOFS_Z:
			z += mousebuf[passCur & 0xFF].dwData;
			break;

		case DIMOFS_BUTTON0:
			b[0] = (mousebuf[passCur & 0xFF].dwData & 0x80) ? 1 : 0;
			if (b[0])
				timebuf[0] = mousebuf[passCur & 0xFF].dwTimeStamp;
			break;

		case DIMOFS_BUTTON1:
			b[1] = (mousebuf[passCur & 0xFF].dwData & 0x80) ? 1 : 0;
			if (b[1])
				timebuf[1] = mousebuf[passCur & 0xFF].dwTimeStamp;
			break;

		case DIMOFS_BUTTON2:
			b[2] = (mousebuf[passCur & 0xFF].dwData & 0x80) ? 1 : 0;
			if (b[2])
				timebuf[2] = mousebuf[passCur & 0xFF].dwTimeStamp;
			break;

		case DIMOFS_BUTTON3:
			b[3] = (mousebuf[passCur & 0xFF].dwData & 0x80) ? 1 : 0;
			if (b[3])
				timebuf[3] = mousebuf[passCur & 0xFF].dwTimeStamp;
			break;
		}

		passCur++;
	}

	if (bufCur == passCur)
		bufCur = passCur = 0;
}

// dimouse.cpp:194
void DirectInputMouse::Update(unsigned long timeStamp) {
	DIMOUSESTATE dims;
	POINT curPt;

	bUpdated = false;
	x = y = z = 0;

	if (timeStamp == 0 || !active) {
		b[0] = 0; b[1] = 0; b[2] = 0; b[3] = 0;
		return;
	}

	if (diMouse) {
		// The counter advances twice per pass: three attempts at most.
		for (int i = 0; i < 5; i++) {
			i++;
			if (diMouse->GetDeviceState(sizeof(DIMOUSESTATE), &dims) == DI_OK)
				break;
			memset(&dims, 0, sizeof(dims));
			Acquire();
		}

		GetDeviceData();

		if (bufCur - passCur > 0)
			FlushBuffer(timeStamp);
	}

	if (m_moveMode) {
		GetCursorPos(&curPt);
		MoveWindow(m_myHwnd, curPt.x - m_clientX, curPt.y - m_clientY, m_clientWidth, m_clientHeight, TRUE);
	}
}

// dimouse.cpp:254
void DirectInputMouse::ClearBuffer(void) {
	bufCur = 0;
	passCur = 0;

	x = y = z = 0;

	b[0] = 0; b[1] = 0; b[2] = 0; b[3] = 0;
}

// dimouse.cpp:264
int DirectInputMouse::WinProc(unsigned int message, unsigned long wParam, unsigned long lParam) {
	RECT clientRect;
	POINT pt;

	switch (message) {
	case WM_NCLBUTTONDOWN:
		if (wParam == HTCAPTION) {
			m_moveMode = true;
			GetCursorPos(&pt);
			GetWindowRect(m_myHwnd, &clientRect);
			m_clientX = pt.x - clientRect.left;
			m_clientY = pt.y - clientRect.top;
			m_clientWidth = clientRect.right - clientRect.left;
			m_clientHeight = clientRect.bottom - clientRect.top;
		}
		break;

	case WM_LBUTTONUP:
		if (m_moveMode)
			m_moveMode = false;
		break;

	case WM_NCLBUTTONUP:
		m_moveMode = false;
		break;

	case WM_ACTIVATE:
		if (wParam) {
			Acquire();
			GetDeviceData();
		} else {
			diMouse->Unacquire();
			m_moveMode = false;
		}

		ClearBuffer();

		return 1;
	}

	return 0;
}

// dimouse.cpp:386
int DirectInputMouse::GetState(int sort, int n) {
	switch (sort) {
	case 0:
		if (n == 1)
			return x;
		else if (n == 2)
			return y;
		else if (n == 3)
			return z;
		break;

	case 1:
		return b[n];
	}

	return 0;
}

// dimouse.cpp:414
HRESULT DirectInputMouse::Acquire(void) {
	if (!m_moveMode)
		return diMouse->Acquire();

	return 1;
}
