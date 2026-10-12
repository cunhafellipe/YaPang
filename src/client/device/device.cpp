// Original: d:\build\custom\temp\code\client\device\device.cpp
// The DLL's WList instances allocate with the CRT (__imp__malloc / __imp__free),
// not with Wangreal's g_mem (wmemblock.h).
#define WLIST_ALLOC(size) malloc(size)
#define WLIST_FREE(ptr) free(ptr)

#include <stdlib.h>
#include <windows.h>

#include "device.h"
#include "wlist.h"
#include "wvideo.h"
#include "winput.h"
#include "wdinput/DirectInput.h"
#include "wime/wime.h"

// wd3d8.cpp: enumerates the Direct3D adapters and adds a video device for each.
void EnumDirect3D8(void);

// device.cpp:28
static WList<WVideoDev *> vidList(16, 16);
// device.cpp:29
static WList<WInputDev *> inpList(16, 16);

// device.cpp:34
void AddVideoDevice(WVideoDev *video) {
	vidList += video;
}

// device.cpp:44
void AddInputDevice(WInputDev *input) {
	inpList += input;
}

// device.cpp:56
extern "C" __declspec(dllexport) WVideoDev *__cdecl WEnumVideoDevices(int iIndex) {
	if (vidList.Start() == NULL) {
		EnumDirect3D8();
	}

	int i = 0;
	for (WVideoDev *video = vidList.Start(); video != NULL; video = vidList.Next()) {
		if (i == iIndex) return video;
		i++;
	}
	return NULL;
}

// device.cpp:127
extern "C" __declspec(dllexport) WInputDev *__cdecl WEnumInputDevices(int iIndex) {
	if (inpList.Start() == NULL) {
		AddInputDevice(new WDirectInput);
		AddInputDevice(new WIme);
	}

	int i = 0;
	for (WInputDev *input = inpList.Start(); input != NULL; input = inpList.Next()) {
		if (i == iIndex) return input;
		i++;
	}
	return NULL;
}

// device.cpp:148
extern "C" __declspec(dllexport) char *__cdecl WVersion(void) {
	return __DATE__;
}

// device.cpp:153
extern "C" __declspec(dllexport) char *__cdecl WDeviceName(void) {
	return "DIrectX8";
}

// device.cpp:162
extern "C" BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason, LPVOID lpReserved) {
	switch (ul_reason) {
	case DLL_PROCESS_DETACH:
		{
			for (WVideoDev *video = vidList.Start(); video != NULL; video = vidList.Next()) {
				delete video;
			}
			for (WInputDev *input = inpList.Start(); input != NULL; input = inpList.Next()) {
				delete input;
			}
		}
		break;
	}
	return TRUE;
}
