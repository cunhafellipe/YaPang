// Original: d:\build\custom\temp\code\client\wangreal\include\wdevice.h
#ifndef PG_CLIENT_WANGREAL_INCLUDE_WDEVICE_H
#define PG_CLIENT_WANGREAL_INCLUDE_WDEVICE_H

#include "defines.h"

class WProc;






// Base of the engine's devices (PDB: size 0x4, a vfptr only). Its bodies
// are in-class inlines at the lines the DLL's line records give (wdevice.h
// 20-23); the blank lines above keep them there. The default constructor
// is implicit.
class WDevice {
public:
	virtual ~WDevice() {}                                // vtable 0x0
	virtual char *GetDeviceName(void) { return NULL; }   // vtable 0x4
	virtual char *EnumModeName(void) { return ""; }      // vtable 0x8
	virtual WProc *ExternProc(void) { return NULL; }     // vtable 0xC
};
CHECK_SIZE(WDevice, 0x4);

#endif // PG_CLIENT_WANGREAL_INCLUDE_WDEVICE_H
