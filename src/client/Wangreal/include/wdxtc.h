// Original: d:\build\custom\temp\code\client\wangreal\include\wdxtc.h
#ifndef PG_CLIENT_WANGREAL_INCLUDE_WDXTC_H
#define PG_CLIENT_WANGREAL_INCLUDE_WDXTC_H

#include <windows.h>
#include <ddraw.h>

#include "defines.h"

// A 32-bit pixel, B G R A in memory (PDB layout, size 0x4).
struct sClr32 {
	union {
		unsigned int clr;     // 0x0
		struct {
			unsigned char b;  // 0x0
			unsigned char g;  // 0x1
			unsigned char r;  // 0x2
			unsigned char a;  // 0x3
		};
	};
};

CHECK_SIZE(sClr32, 0x4);
CHECK_FIELD(sClr32, clr, 0x0);
CHECK_FIELD(sClr32, b, 0x0);
CHECK_FIELD(sClr32, g, 0x1);
CHECK_FIELD(sClr32, r, 0x2);
CHECK_FIELD(sClr32, a, 0x3);

// A DXT color block: two RGB565 colors and 4x4 2-bit indices (size 0x8).
struct sDXTC_ClrBlk {
	unsigned short Clr0;   // 0x0
	unsigned short Clr1;   // 0x2
	unsigned char Row[4];  // 0x4
};

CHECK_SIZE(sDXTC_ClrBlk, 0x8);
CHECK_FIELD(sDXTC_ClrBlk, Clr0, 0x0);
CHECK_FIELD(sDXTC_ClrBlk, Clr1, 0x2);
CHECK_FIELD(sDXTC_ClrBlk, Row, 0x4);

// A DXT3 explicit alpha block: 4x4 4-bit alphas (size 0x8).
struct sDXTC_ExpAlphaBlk {
	unsigned short Row[4];  // 0x0
};

CHECK_SIZE(sDXTC_ExpAlphaBlk, 0x8);
CHECK_FIELD(sDXTC_ExpAlphaBlk, Row, 0x0);

// A DXT5 interpolated alpha block: two alphas and 4x4 3-bit indices (size 0x8).
struct sDXTC_LinearAlphaBlk3Bit {
	unsigned char a0;        // 0x0
	unsigned char a1;        // 0x1
	unsigned char stuff[6];  // 0x2
};

CHECK_SIZE(sDXTC_LinearAlphaBlk3Bit, 0x8);
CHECK_FIELD(sDXTC_LinearAlphaBlk3Bit, a0, 0x0);
CHECK_FIELD(sDXTC_LinearAlphaBlk3Bit, a1, 0x1);
CHECK_FIELD(sDXTC_LinearAlphaBlk3Bit, stuff, 0x2);

// Loads a DDS file image (DXT1/DXT3/DXT5) and decompresses it to 32-bit
// pixels (PDB layout, size 0x84).
class WDXTC {
	LAYOUT_CHECK_FRIEND;
public:
	WDXTC();
	~WDXTC();

	bool Load(const unsigned char* pData, int iSize);
	_DDSURFACEDESC2* GetDdsd2();
	unsigned char* GetDecompData();

private:
	bool DecompDXTC();
	void Decomp_DXT1();
	void Decomp_DXT3();
	void Decomp_DXT5();
	void DecompBlock_DXT1(unsigned char* pDecomp, const sDXTC_ClrBlk* pBlock);
	void DecompBlock_DXT3(unsigned char* pDecomp, const sDXTC_ClrBlk* pBlock);
	void DecompBlock_DXT5(unsigned char* pDecomp, const sDXTC_ClrBlk* pBlock);
	void GetColorFromBlock(sClr32* aClr32, const sDXTC_ClrBlk* pBlock);
	void DecodeColor(sClr32* pDecomp, const sDXTC_ClrBlk* pBlock, sClr32* aClr32);
	void DecodeExpAlphaBlock(sClr32* pDecomp, const sDXTC_ExpAlphaBlk* pAlpha);
	void DecodeLinearAlphaBlock3Bit(sClr32* pDecomp, const sDXTC_LinearAlphaBlk3Bit* pAlpha);
	void SaveAsBmp(char* FileName) const;

	_DDSURFACEDESC2 m_DDSD2;         // 0x0
	unsigned char* m_pCompData;      // 0x7c
	unsigned char* m_pDecompData;    // 0x80
};

CHECK_SIZE(WDXTC, 0x84);
BEGIN_LAYOUT_CHECK(WDXTC)
	CHECK_FIELD(WDXTC, m_DDSD2, 0x0);
	CHECK_FIELD(WDXTC, m_pCompData, 0x7c);
	CHECK_FIELD(WDXTC, m_pDecompData, 0x80);
END_LAYOUT_CHECK

#endif // PG_CLIENT_WANGREAL_INCLUDE_WDXTC_H
