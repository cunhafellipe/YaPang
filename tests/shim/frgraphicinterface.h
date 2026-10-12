// Host test shim for Fresh's FrGraphicInterface.
//
// The real src/client/Fresh/include/frgraphicinterface.h pulls in the whole
// Wangreal view stack (woverlay/wview/wvideo/wresrcmng -> <windows.h> + D3D),
// which is not host-buildable. FrArea.cpp only needs GDI()->DrawTexture and the
// FrALPHA helper from it, so the host test target resolves that one include to
// this stand-in. The FrArea code under test is still the real
// src/client/Fresh/source/FrArea.cpp; only this heavy dependency is inert.
#ifndef PG_TESTS_SHIM_FRGRAPHICINTERFACE_H
#define PG_TESTS_SHIM_FRGRAPHICINTERFACE_H

#include "defines.h"
#include "client/Wangreal/include/wtypes.h"

class Bitmap;

class FrGraphicInterface {
public:
	virtual ~FrGraphicInterface(void) {}

	void DrawTexture(const Bitmap* pBitmap, const WRect& src, const WRect& dest, unsigned long diffuse, int type) const;
};

// frgraphicinterface.h:159; scales the alpha byte of a colour.
inline unsigned long FrALPHA(unsigned long color, float alpha) {
	return ((unsigned long)((color >> 24) * alpha) << 24) | (color & 0x00ffffff);
}

#endif // PG_TESTS_SHIM_FRGRAPHICINTERFACE_H
