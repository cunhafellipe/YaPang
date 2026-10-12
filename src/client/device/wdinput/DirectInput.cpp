// Original: d:\build\custom\temp\code\client\device\wdinput\directinput.cpp
#include "DirectInput.h"

#include "dikeyboard.h"
#include "dimouse.h"

// directinput.cpp:21
WDirectInput::WDirectInput(void) {
	di = NULL;
}

// directinput.cpp:26
WDirectInput::~WDirectInput(void) {
	if (di) { di->Release(); di = NULL; }
}

// directinput.cpp:31
char *WDirectInput::GetDeviceName(void) {
	return "DirectInput";
}

// directinput.cpp:40
char *WDirectInput::EnumModeName(void) {
	return "Keyboard\0Mouse\0";
}

// directinput.cpp:45
WInputDev *WDirectInput::MakeClone(char *modeName, HWND__ *hwnd) {
	if (di == NULL) {
		DirectInput8Create((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&di, NULL);
	}

	if (strcmpi(modeName, "keyboard") == 0) {
		return new DirectInputKeyboard(di, hwnd);
	} else if (strcmpi(modeName, "mouse") == 0) {
		return new DirectInputMouse(di, hwnd);
	}

	return NULL;
}
