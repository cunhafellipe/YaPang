// Original: d:\build\custom\temp\code\client\device\screencape.h
#ifndef PG_CLIENT_DEVICE_SCREENCAPE_H
#define PG_CLIENT_DEVICE_SCREENCAPE_H

#include <windows.h>

#include "defines.h"

// The interface of ScreenCape.dll (PDB layout), loaded at run time by
// ScreenCapeFactory.
namespace Nv {

namespace GraphicDeviceType {
	enum Enum {
		DIRECT3D8 = 0,
		DIRECT3D9 = 1
	};
}

struct IScreenCape {
	virtual bool Initialize(HWND__ *hWnd, void **ppDevice, GraphicDeviceType::Enum deviceType) = 0;   // vtable 0x0
	virtual void Release(void) = 0;                                                                      // vtable 0x4
	virtual void Render(void) = 0;                                                                       // vtable 0x8
	virtual void Pause(void) = 0;                                                                        // vtable 0xc
	virtual void Resume(void) = 0;                                                                       // vtable 0x10
};

CHECK_SIZE(IScreenCape, 0x4);

namespace Factory {

// The interface version CreateScreenCape is asked for (the build date 2010-03-29).
const unsigned long SCREENCAPE_INTERFACE_VERSION = 20100329;

class ScreenCapeFactory {
	LAYOUT_CHECK_FRIEND;
public:
	// screencape.h:88
	ScreenCapeFactory(void)
		: m_pProc(NULL) {
		m_hModule = LoadLibraryA("ScreenCape.dll");
		if (m_hModule == NULL) {
			MessageBoxW(NULL, L"Load failed 'ScreenCape.dll'", L"Warning!", MB_OK);
		} else {
			m_pProc = (HRESULT (__stdcall *)(IScreenCape **, unsigned long))GetProcAddress(m_hModule, "CreateScreenCape");
		}
	}

	// screencape.h:97
	~ScreenCapeFactory(void) {
		if (m_hModule) FreeLibrary(m_hModule);
	}

	// screencape.h:103
	IScreenCape *CreateScreenCapeLayer(void) {
		IScreenCape *pInterface = NULL;
		if (m_pProc) {
			m_pProc(&pInterface, SCREENCAPE_INTERFACE_VERSION);
		}
		return pInterface;
	}

private:
	HINSTANCE m_hModule;                                           // 0x0
	HRESULT (__stdcall *m_pProc)(IScreenCape **, unsigned long);      // 0x4
};

CHECK_SIZE(ScreenCapeFactory, 0x8);
BEGIN_LAYOUT_CHECK(ScreenCapeFactory)
	CHECK_FIELD(ScreenCapeFactory, m_hModule, 0x0);
	CHECK_FIELD(ScreenCapeFactory, m_pProc, 0x4);
END_LAYOUT_CHECK

}   // namespace Factory

}   // namespace Nv

#endif   // PG_CLIENT_DEVICE_SCREENCAPE_H
