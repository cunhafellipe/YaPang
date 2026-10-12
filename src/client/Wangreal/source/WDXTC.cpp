// Original: d:\build\custom\temp\code\client\wangreal\source\wdxtc.cpp
#include <stdio.h>
#include <string.h>
#include <new>

#include "../include/wdxtc.h"

// wdxtc.cpp:18
WDXTC::WDXTC() {
	m_pCompData = NULL;
	m_pDecompData = NULL;
}

// wdxtc.cpp:22
WDXTC::~WDXTC() {
	if (m_pDecompData) { delete[] m_pDecompData; m_pDecompData = NULL; }
	if (m_pCompData) { delete[] m_pCompData; m_pCompData = NULL; }
}

// wdxtc.cpp:28
bool WDXTC::Load(const unsigned char* pData, int iSize) {
	// "DDS " magic
	if (pData[0] != 'D' || pData[1] != 'D' || pData[2] != 'S') {
		return false;
	}
	pData += 4;

	memcpy(&m_DDSD2, pData, sizeof(_DDSURFACEDESC2));

	if (m_DDSD2.dwFlags & DDSD_LINEARSIZE) {
		m_pCompData = new unsigned char[m_DDSD2.dwLinearSize];
		memcpy(m_pCompData, pData + sizeof(_DDSURFACEDESC2), m_DDSD2.dwLinearSize);
	} else {
		return false;
	}

	return DecompDXTC();
}

// wdxtc.cpp:56
bool WDXTC::DecompDXTC() {
	if (m_pDecompData) { delete[] m_pDecompData; m_pDecompData = NULL; }
	m_pDecompData = new unsigned char[m_DDSD2.dwWidth * m_DDSD2.dwHeight * 4];

	switch (m_DDSD2.ddpfPixelFormat.dwFourCC) {
	case FOURCC_DXT1:
		Decomp_DXT1();
		break;
	case FOURCC_DXT3:
		Decomp_DXT3();
		break;
	case FOURCC_DXT5:
		Decomp_DXT5();
		break;
	default:
		return false;
	}

	return true;
}

// wdxtc.cpp:85
_DDSURFACEDESC2* WDXTC::GetDdsd2() {
	return &m_DDSD2;
}

// wdxtc.cpp:90
unsigned char* WDXTC::GetDecompData() {
	return m_pDecompData;
}

// wdxtc.cpp:95
void WDXTC::Decomp_DXT1() {
	int nXBlocks = m_DDSD2.dwWidth / 4;
	int nYBlocks = m_DDSD2.dwHeight / 4;

	for (int i = 0; i < nYBlocks; i++) {
		const sDXTC_ClrBlk* pBlock = (const sDXTC_ClrBlk*)(m_pCompData + i * nXBlocks * 8);
		unsigned char* pDecomp = m_pDecompData + i * m_DDSD2.dwWidth * 16;

		for (int j = 0; j < nXBlocks; j++) {
			DecompBlock_DXT1(pDecomp, pBlock);

			pBlock++;
			pDecomp += 16;
		}
	}
}

// wdxtc.cpp:120
void WDXTC::Decomp_DXT3() {
	int nXBlocks = m_DDSD2.dwWidth / 4;
	int nYBlocks = m_DDSD2.dwHeight / 4;

	for (int i = 0; i < nYBlocks; i++) {
		const sDXTC_ClrBlk* pBlock = (const sDXTC_ClrBlk*)(m_pCompData + i * nXBlocks * 16);
		unsigned char* pDecomp = m_pDecompData + i * m_DDSD2.dwWidth * 16;

		for (int j = 0; j < nXBlocks; j++) {
			DecompBlock_DXT3(pDecomp, pBlock);

			pBlock += 2;
			pDecomp += 16;
		}
	}
}

// wdxtc.cpp:145
void WDXTC::Decomp_DXT5() {
	int nXBlocks = m_DDSD2.dwWidth / 4;
	int nYBlocks = m_DDSD2.dwHeight / 4;

	for (int i = 0; i < nYBlocks; i++) {
		const sDXTC_ClrBlk* pBlock = (const sDXTC_ClrBlk*)(m_pCompData + i * nXBlocks * 16);
		unsigned char* pDecomp = m_pDecompData + i * m_DDSD2.dwWidth * 16;

		for (int j = 0; j < nXBlocks; j++) {
			DecompBlock_DXT5(pDecomp, pBlock);

			pBlock += 2;
			pDecomp += 16;
		}
	}
}

// wdxtc.cpp:170
void WDXTC::DecompBlock_DXT1(unsigned char* pDecomp, const sDXTC_ClrBlk* pBlock) {
	sClr32 aClr[4];

	GetColorFromBlock(aClr, pBlock);
	DecodeColor((sClr32*)pDecomp, pBlock, aClr);
}

// wdxtc.cpp:178
void WDXTC::DecompBlock_DXT3(unsigned char* pDecomp, const sDXTC_ClrBlk* pBlock) {
	// The explicit alpha block comes first, the color block after it.
	sClr32 aClr[4];

	GetColorFromBlock(aClr, pBlock + 1);
	DecodeColor((sClr32*)pDecomp, pBlock + 1, aClr);
	DecodeExpAlphaBlock((sClr32*)pDecomp, (const sDXTC_ExpAlphaBlk*)pBlock);
}

// wdxtc.cpp:190
void WDXTC::DecompBlock_DXT5(unsigned char* pDecomp, const sDXTC_ClrBlk* pBlock) {
	// The interpolated alpha block comes first, the color block after it.
	sClr32 aClr[4];

	GetColorFromBlock(aClr, pBlock + 1);
	DecodeColor((sClr32*)pDecomp, pBlock + 1, aClr);
	DecodeLinearAlphaBlock3Bit((sClr32*)pDecomp, (const sDXTC_LinearAlphaBlk3Bit*)pBlock);
}

// wdxtc.cpp:202
void WDXTC::GetColorFromBlock(sClr32* aClr32, const sDXTC_ClrBlk* pBlock) {
	// RGB565 -> RGB888
	aClr32[0].a = 0xff;
	aClr32[0].r = (unsigned char)(pBlock->Clr0 >> 11) << 3;
	aClr32[0].g = (unsigned char)(pBlock->Clr0 >> 5) << 2;
	aClr32[0].b = pBlock->Clr0 << 3;

	aClr32[1].a = 0xff;
	aClr32[1].r = (unsigned char)(pBlock->Clr1 >> 11) << 3;
	aClr32[1].g = (unsigned char)(pBlock->Clr1 >> 5) << 2;
	aClr32[1].b = pBlock->Clr1 << 3;

	if (pBlock->Clr0 > pBlock->Clr1) {
		// Four-color block.
		aClr32[2].r = (2 * aClr32[0].r + aClr32[1].r) / 3;
		aClr32[2].g = (2 * aClr32[0].g + aClr32[1].g) / 3;
		aClr32[2].b = (2 * aClr32[0].b + aClr32[1].b) / 3;

		aClr32[3].r = (aClr32[0].r + 2 * aClr32[1].r) / 3;
		aClr32[3].g = (aClr32[0].g + 2 * aClr32[1].g) / 3;
		aClr32[3].b = (aClr32[0].b + 2 * aClr32[1].b) / 3;
		aClr32[3].a = 0xff;
	} else {
		// Three-color block: color 3 is transparent.
		aClr32[2].r = (aClr32[0].r + aClr32[1].r) / 2;
		aClr32[2].g = (aClr32[0].g + aClr32[1].g) / 2;
		aClr32[2].b = (aClr32[0].b + aClr32[1].b) / 2;

		aClr32[3].r = 0x00;
		aClr32[3].g = 0xff;
		aClr32[3].b = 0xff;
		aClr32[3].a = 0x00;
	}
	aClr32[2].a = 0xff;
}

// wdxtc.cpp:260
void WDXTC::DecodeColor(sClr32* pDecomp, const sDXTC_ClrBlk* pBlock, sClr32* aClr32) {
	const unsigned char uiMasks[4] = { 0x03, 0x0c, 0x30, 0xc0 };

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			unsigned int uiIndex = (pBlock->Row[y] & uiMasks[x]) >> (x * 2);

			switch (uiIndex) {
			case 0:
				pDecomp->clr = aClr32[0].clr;
				break;
			case 1:
				pDecomp->clr = aClr32[1].clr;
				break;
			case 2:
				pDecomp->clr = aClr32[2].clr;
				break;
			case 3:
				pDecomp->clr = aClr32[3].clr;
				break;
			}
			pDecomp++;
		}
		pDecomp += m_DDSD2.dwWidth - 4;
	}
}

// wdxtc.cpp:297
void WDXTC::DecodeExpAlphaBlock(sClr32* pDecomp, const sDXTC_ExpAlphaBlk* pAlpha) {
	for (int y = 0; y < 4; y++) {
		unsigned short usAlpha = pAlpha->Row[y];

		for (int x = 0; x < 4; x++) {
			unsigned char a = usAlpha & 0x0f;
			pDecomp->a = a | (a << 4);
			usAlpha >>= 4;
			pDecomp++;
		}
		pDecomp += m_DDSD2.dwWidth - 4;
	}
}

// wdxtc.cpp:321
void WDXTC::DecodeLinearAlphaBlock3Bit(sClr32* pDecomp, const sDXTC_LinearAlphaBlk3Bit* pAlpha) {
	unsigned short gAlphas[8];
	unsigned char gBits[4][4];
	sClr32 gACol[4][4];

	gAlphas[0] = pAlpha->a0;
	gAlphas[1] = pAlpha->a1;

	if (gAlphas[0] > gAlphas[1]) {
		// Eight-alpha block: six interpolated alphas.
		gAlphas[2] = (6 * gAlphas[0] + 1 * gAlphas[1]) / 7;
		gAlphas[3] = (5 * gAlphas[0] + 2 * gAlphas[1]) / 7;
		gAlphas[4] = (4 * gAlphas[0] + 3 * gAlphas[1]) / 7;
		gAlphas[5] = (3 * gAlphas[0] + 4 * gAlphas[1]) / 7;
		gAlphas[6] = (2 * gAlphas[0] + 5 * gAlphas[1]) / 7;
		gAlphas[7] = (1 * gAlphas[0] + 6 * gAlphas[1]) / 7;
	} else {
		// Six-alpha block: four interpolated alphas, then 0 and 255.
		gAlphas[2] = (4 * gAlphas[0] + 1 * gAlphas[1]) / 5;
		gAlphas[3] = (3 * gAlphas[0] + 2 * gAlphas[1]) / 5;
		gAlphas[4] = (2 * gAlphas[0] + 3 * gAlphas[1]) / 5;
		gAlphas[5] = (1 * gAlphas[0] + 4 * gAlphas[1]) / 5;
		gAlphas[6] = 0;
		gAlphas[7] = 255;
	}

	// The 3-bit indices of the first two rows.
	const unsigned int mask = 0x00000007;
	unsigned int bits = *((const unsigned int*)&pAlpha->stuff[0]);

	gBits[0][0] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[0][1] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[0][2] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[0][3] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[1][0] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[1][1] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[1][2] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[1][3] = (unsigned char)(bits & mask);

	// The last two rows.
	bits = *((const unsigned int*)&pAlpha->stuff[3]);

	gBits[2][0] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[2][1] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[2][2] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[2][3] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[3][0] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[3][1] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[3][2] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[3][3] = (unsigned char)(bits & mask);

	int row, pix;

	for (row = 0; row < 4; row++) {
		for (pix = 0; pix < 4; pix++) {
			gACol[row][pix].a = (unsigned char)gAlphas[gBits[row][pix]];
		}
	}

	for (row = 0; row < 4; row++) {
		for (pix = 0; pix < 4; pix++) {
			pDecomp->a = gACol[row][pix].a;
			pDecomp++;
		}
		pDecomp += m_DDSD2.dwWidth - 4;
	}
}

// wdxtc.cpp:434
void WDXTC::SaveAsBmp(char* FileName) const {
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;

	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = 0;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	bfh.bfType = 0x4d42; // "BM"
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biPlanes = 1;
	bih.biBitCount = 24;

	bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + m_DDSD2.dwWidth * m_DDSD2.dwHeight * 3;
	bih.biWidth = m_DDSD2.dwWidth;
	bih.biHeight = m_DDSD2.dwHeight;

	char strBuf[64];
	sprintf(strBuf, "%s_RGB.bmp", FileName);
	FILE* pfRGB = fopen(strBuf, "wb");
	sprintf(strBuf, "%s_Alpha.bmp", FileName);
	FILE* pfAlpha = fopen(strBuf, "wb");

	fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, pfRGB);
	fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, pfRGB);
	fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, pfAlpha);
	fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, pfAlpha);

	unsigned char BGR[3];
	unsigned char Alpha[3];

	// Bottom-up rows.
	for (int i = m_DDSD2.dwHeight - 1; i >= 0; i--) {
		const sClr32* pClr = (const sClr32*)m_pDecompData + m_DDSD2.dwWidth * i;

		for (unsigned long dw = 0; dw < m_DDSD2.dwWidth; dw++) {
			BGR[2] = pClr->b;
			BGR[1] = pClr->g;
			BGR[0] = pClr->r;
			Alpha[0] = Alpha[1] = Alpha[2] = pClr->a;

			fwrite(BGR, 3, 1, pfRGB);
			fwrite(Alpha, 3, 1, pfAlpha);
			pClr++;
		}
	}

	fclose(pfAlpha);
	fclose(pfRGB);
}
