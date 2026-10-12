// XUnzip.h: Hans Dietrich's XUnzip (Lucian Wischik's unzip over zlib 1.1.3 inflate)
#ifndef PG_CLIENT_WANGREAL_SOURCE_XUNZIP_H
#define PG_CLIENT_WANGREAL_SOURCE_XUNZIP_H

#include <windows.h>

#include "../../../defines.h"

struct HZIP__ {
	int unused; // 0x0
};
CHECK_SIZE(HZIP__, 0x4);
typedef HZIP__* HZIP;

typedef DWORD ZRESULT;

struct ZIPENTRY {
	int index;          // 0x0
	char name[MAX_PATH]; // 0x4
	DWORD attr;         // 0x108
	FILETIME atime;     // 0x10c
	FILETIME ctime;     // 0x114
	FILETIME mtime;     // 0x11c
	long comp_size;     // 0x124
	long unc_size;      // 0x128
};
CHECK_SIZE(ZIPENTRY, 0x12c);
CHECK_FIELD(ZIPENTRY, attr, 0x108);
CHECK_FIELD(ZIPENTRY, unc_size, 0x128);

struct ZIPENTRYW {
	int index;          // 0x0
	char name[MAX_PATH]; // 0x4
	DWORD attr;         // 0x108
	FILETIME atime;     // 0x10c
	FILETIME ctime;     // 0x114
	FILETIME mtime;     // 0x11c
	long comp_size;     // 0x124
	long unc_size;      // 0x128
};
CHECK_SIZE(ZIPENTRYW, 0x12c);

HZIP OpenZipU(void* z, unsigned int len, DWORD flags);
ZRESULT CloseZipU(HZIP hz);
ZRESULT GetZipItemA(HZIP hz, int index, ZIPENTRY* ze);
ZRESULT GetZipItemW(HZIP hz, int index, ZIPENTRYW* ze);
ZRESULT FindZipItemA(HZIP hz, const char* name, bool ic, int* index, ZIPENTRY* ze);
ZRESULT FindZipItemW(HZIP hz, const char* name, bool ic, int* index, ZIPENTRYW* ze);
ZRESULT UnzipItem(HZIP hz, int index, void* dst, unsigned int len, DWORD flags);
unsigned int FormatZipMessageU(ZRESULT code, char* buf, unsigned int len);
bool IsZipHandleU(HZIP hz);

#endif // PG_CLIENT_WANGREAL_SOURCE_XUNZIP_H
