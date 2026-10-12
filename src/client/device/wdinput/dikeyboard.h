// Original: d:\build\custom\temp\code\client\device\wdinput\dikeyboard.h
#ifndef PG_CLIENT_DEVICE_WDINPUT_DIKEYBOARD_H
#define PG_CLIENT_DEVICE_WDINPUT_DIKEYBOARD_H

#include "DirectInput.h"
#include "defines.h"
#include "winput.h"
#include "wproc.h"

// The DirectInput keyboard device: buffered key events turned into an ASCII
// queue, plus the immediate key state (PDB layout, size 0x1D2C).
class DirectInputKeyboard : public WInputDev, public WProc {
	LAYOUT_CHECK_FRIEND;
public:
	DirectInputKeyboard(IDirectInput8A *di, HWND__ *hwnd);
	virtual ~DirectInputKeyboard(void);

	virtual bool InitDevice(HWND__ *hwnd, bool exclusive);
	// dikeyboard.h:27
	virtual WProc *ExternProc(void) { return this; }
	virtual int WinProc(unsigned int message, unsigned long wParam, unsigned long lParam);
	virtual unsigned long GetEventTime(int n);
	virtual int GetState(int sort, int n);
	virtual void Reset(void);
	virtual void Update(unsigned long timeStamp);

private:
	// One DirectInput scan code and the ASCII codes it produces without and
	// with shift (PDB layout, size 0xC).
	struct w_conv_scan_code {
		int di_scan;         // 0x0
		int ascii;           // 0x4
		int asciiwithshift;  // 0x8
	};

	HRESULT Acquire(void);
	void ClearBuffer(void);
	void FlushBuffer(unsigned long timeStamp);
	void GetDeviceData(void);

	IDirectInput8A *diBackup;            // 0x14
	IDirectInputDevice8A *diKey;         // 0x18
	DIDEVICEOBJECTDATA keybuf[256];      // 0x1C
	unsigned long timebuf[256];          // 0x141C
	int asciiBuf[256];                   // 0x181C
	int asciiPos;                        // 0x1C1C
	int asciiCur;                        // 0x1C20
	int bufCur;                          // 0x1C24
	int passCur;                         // 0x1C28
	char keyState[256];                  // 0x1C2C

	static int scan2ascii_without_shift[256];
	static int scan2ascii_with_shift[256];
	static int ascii2scan[256];
	static w_conv_scan_code convList[99];
};
BEGIN_LAYOUT_CHECK(DirectInputKeyboard)
	CHECK_SIZE(DirectInputKeyboard, 0x1D2C);
	CHECK_FIELD(DirectInputKeyboard, diBackup, 0x14);
	CHECK_FIELD(DirectInputKeyboard, diKey, 0x18);
	CHECK_FIELD(DirectInputKeyboard, keybuf, 0x1C);
	CHECK_FIELD(DirectInputKeyboard, timebuf, 0x141C);
	CHECK_FIELD(DirectInputKeyboard, asciiBuf, 0x181C);
	CHECK_FIELD(DirectInputKeyboard, asciiPos, 0x1C1C);
	CHECK_FIELD(DirectInputKeyboard, asciiCur, 0x1C20);
	CHECK_FIELD(DirectInputKeyboard, bufCur, 0x1C24);
	CHECK_FIELD(DirectInputKeyboard, passCur, 0x1C28);
	CHECK_FIELD(DirectInputKeyboard, keyState, 0x1C2C);
	CHECK_SIZE(DirectInputKeyboard::w_conv_scan_code, 0xC);
END_LAYOUT_CHECK

#endif // PG_CLIENT_DEVICE_WDINPUT_DIKEYBOARD_H
