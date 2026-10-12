// Host test shim for <windows.h>.
//
// The decompiled headers under src/ target PE32/Windows (bitmap.h and the
// Wangreal view stack include <windows.h> for tagRECT / tagBITMAPINFO / ...).
// A host unit test has no Windows SDK, so this shim declares just the handful
// of Win32 types the host-buildable Fresh headers reference. It is NOT part of
// src/ and never ships; it is picked up first via the test target's include
// path (tests/shim before src/). Keep it minimal -- add a type only when a
// host test actually needs it.
#ifndef PG_TESTS_SHIM_WINDOWS_H
#define PG_TESTS_SHIM_WINDOWS_H

struct HWND__;

typedef struct tagPOINT {
	int x;
	int y;
} tagPOINT, POINT;

typedef struct tagRECT {
	int left;
	int top;
	int right;
	int bottom;
} tagRECT, RECT;

typedef struct tagSIZE {
	int cx;
	int cy;
} tagSIZE, SIZE;

typedef struct tagRGBQUAD {
	unsigned char rgbBlue;
	unsigned char rgbGreen;
	unsigned char rgbRed;
	unsigned char rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFOHEADER {
	unsigned int biSize;
	int biWidth;
	int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPINFO {
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[1];
} tagBITMAPINFO, BITMAPINFO;

#endif // PG_TESTS_SHIM_WINDOWS_H
