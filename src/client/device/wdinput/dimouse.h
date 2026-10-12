// Original: d:\build\custom\temp\code\client\device\wdinput\dimouse.h
#ifndef PG_CLIENT_DEVICE_WDINPUT_DIMOUSE_H
#define PG_CLIENT_DEVICE_WDINPUT_DIMOUSE_H

#include "DirectInput.h"
#include "defines.h"
#include "winput.h"
#include "wproc.h"

// The DirectInput mouse device: buffered axis and button events, and the
// dragging of the window by its caption (PDB layout, size 0x146C).
class DirectInputMouse : public WProc, public WInputDev {
	LAYOUT_CHECK_FRIEND;
public:
	DirectInputMouse(IDirectInput8A *di, HWND__ *hwnd);
	virtual ~DirectInputMouse(void);

	virtual bool InitDevice(HWND__ *hwnd, bool exclusive);
	// dimouse.h:24
	virtual WProc *ExternProc(void) { return this; }
	virtual int WinProc(unsigned int message, unsigned long wParam, unsigned long lParam);
	virtual int GetState(int sort, int n);
	virtual void Update(unsigned long timeStamp);
	virtual unsigned long GetEventTime(int n);

private:
	HRESULT Acquire(void);
	void ClearBuffer(void);
	void FlushBuffer(unsigned long timeStamp);
	void GetDeviceData(void);

	IDirectInput8A *diBackup;            // 0x14
	IDirectInputDevice8A *diMouse;       // 0x18
	DIDEVICEOBJECTDATA mousebuf[256];    // 0x1C
	unsigned long timebuf[4];            // 0x141C
	int x;                               // 0x142C
	int y;                               // 0x1430
	int z;                               // 0x1434
	int b[4];                            // 0x1438
	int bufCur;                          // 0x1448
	int passCur;                         // 0x144C
	void *hEvent;                        // 0x1450
	HWND__ *m_myHwnd;                    // 0x1454
	bool m_moveMode;                     // 0x1458
	int m_clientX;                       // 0x145C
	int m_clientY;                       // 0x1460
	int m_clientWidth;                   // 0x1464
	int m_clientHeight;                  // 0x1468
};
BEGIN_LAYOUT_CHECK(DirectInputMouse)
	CHECK_SIZE(DirectInputMouse, 0x146C);
	CHECK_FIELD(DirectInputMouse, diBackup, 0x14);
	CHECK_FIELD(DirectInputMouse, diMouse, 0x18);
	CHECK_FIELD(DirectInputMouse, mousebuf, 0x1C);
	CHECK_FIELD(DirectInputMouse, timebuf, 0x141C);
	CHECK_FIELD(DirectInputMouse, x, 0x142C);
	CHECK_FIELD(DirectInputMouse, y, 0x1430);
	CHECK_FIELD(DirectInputMouse, z, 0x1434);
	CHECK_FIELD(DirectInputMouse, b, 0x1438);
	CHECK_FIELD(DirectInputMouse, bufCur, 0x1448);
	CHECK_FIELD(DirectInputMouse, passCur, 0x144C);
	CHECK_FIELD(DirectInputMouse, hEvent, 0x1450);
	CHECK_FIELD(DirectInputMouse, m_myHwnd, 0x1454);
	CHECK_FIELD(DirectInputMouse, m_moveMode, 0x1458);
	CHECK_FIELD(DirectInputMouse, m_clientX, 0x145C);
	CHECK_FIELD(DirectInputMouse, m_clientY, 0x1460);
	CHECK_FIELD(DirectInputMouse, m_clientWidth, 0x1464);
	CHECK_FIELD(DirectInputMouse, m_clientHeight, 0x1468);
END_LAYOUT_CHECK

#endif // PG_CLIENT_DEVICE_WDINPUT_DIMOUSE_H
