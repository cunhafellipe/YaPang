// Original: d:\build\custom\temp\code\client\device\wdinput\directinput.h
#ifndef PG_CLIENT_DEVICE_WDINPUT_DIRECTINPUT_H
#define PG_CLIENT_DEVICE_WDINPUT_DIRECTINPUT_H

#include <windows.h>

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#include <dinput.h>

#include "defines.h"
#include "winput.h"

// The DirectInput device: owns the IDirectInput8 object and clones the
// keyboard and mouse devices from it (PDB layout, size 0x10).
class WDirectInput : public WInputDev {
	LAYOUT_CHECK_FRIEND;
public:
	WDirectInput(void);
	virtual ~WDirectInput(void);

	virtual char *GetDeviceName(void);
	virtual char *EnumModeName(void);
	virtual WInputDev *MakeClone(char *modeName, HWND__ *hwnd);

private:
	IDirectInput8A *di;  // 0xC
};
BEGIN_LAYOUT_CHECK(WDirectInput)
	CHECK_SIZE(WDirectInput, 0x10);
	CHECK_FIELD(WDirectInput, di, 0xC);
END_LAYOUT_CHECK

#endif // PG_CLIENT_DEVICE_WDINPUT_DIRECTINPUT_H
