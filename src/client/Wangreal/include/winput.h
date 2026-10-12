// Original: d:\build\custom\temp\code\client\wangreal\include\winput.h
#ifndef PG_CLIENT_WANGREAL_INCLUDE_WINPUT_H
#define PG_CLIENT_WANGREAL_INCLUDE_WINPUT_H

#include "defines.h"
#include "wdevice.h"

struct HWND__;

// WINMM's timer, as mmsystem.h declares it (ResetInputTime calls it).
extern "C" __declspec(dllimport) unsigned long __stdcall timeGetTime(void);

// Input device (keyboard, mouse, IME). PDB layout, size 0xC. The bodies are
// in-class inlines at the lines the DLL's line records give (winput.h
// 55-74).
class WInputDev : public WDevice {
public:
	// What GetState/SetState address (PDB enum WInputDev::Sort).
	enum Sort {
		Axis = 0,
		Button = 1,
		KeyState = 2,
		BufState = 3,
		ImeCompState = 4,
		ImeCandWinState = 5,
		ImeCompDoneState = 6,
		ImeOpenState = 7,
		ImeAlphaNumericState = 8,
		ImeCandListState = 9,
		ImeConvState = 10,
		ImePropAtCaret = 11,
		ImeInitAfterWndShown = 12,
		ImeTermKeyMapSharing = 13
	};
	// The axes of an Axis state (PDB enum WInputDev::Axis_).
	enum Axis_ {
		X = 1,
		Y = 2,
		Z = 3
	};














	WInputDev() { active = true; bUpdated = false; }
	virtual ~WInputDev() {}
	// vtable 0x10
	virtual bool InitDevice(HWND__ *hWnd, bool exclusive) { return false; }
	virtual void SetActive(bool b) { active = b; }                   // vtable 0x14
	virtual void Reset(void) {}                                      // vtable 0x18
	virtual void Update(unsigned long time) {}                       // vtable 0x1C
	virtual int GetState(int state, int arg) { return 0; }           // vtable 0x20
	virtual unsigned long GetEventTime(int state) { return 0; }      // vtable 0x24
	virtual bool IsAlphaNumericMode(void) { return true; }           // vtable 0x28
	virtual void SetAlphaNumericMode(bool on) {}                     // vtable 0x30
	virtual bool IsUpdated(void) { return bUpdated; }                // vtable 0x34
	virtual unsigned long GetLastInputTime(void) { return dwLastInputTime; } // vtable 0x38
	virtual void ResetInputTime(void) { dwLastInputTime = timeGetTime(); }   // vtable 0x3C
	// cl 13.10 slots overloads together in reverse order: (void) 0x2C.
	virtual void SetState(int state, int value) {}                   // vtable 0x40
	virtual void SetAlphaNumericMode(void) {}                        // vtable 0x2C
	virtual void SetOpenStatus(bool open) {}                         // vtable 0x44
	// vtable 0x48
	virtual WInputDev *MakeClone(char *name, HWND__ *hWnd) { return NULL; }

protected:
	bool active;                    // 0x4
	bool bUpdated;                  // 0x5
	unsigned long dwLastInputTime;  // 0x8

	LAYOUT_CHECK_FRIEND;
};
BEGIN_LAYOUT_CHECK(WInputDev)
	CHECK_SIZE(WInputDev, 0xC);
	CHECK_FIELD(WInputDev, active, 0x4);
	CHECK_FIELD(WInputDev, bUpdated, 0x5);
	CHECK_FIELD(WInputDev, dwLastInputTime, 0x8);
END_LAYOUT_CHECK

#endif // PG_CLIENT_WANGREAL_INCLUDE_WINPUT_H
