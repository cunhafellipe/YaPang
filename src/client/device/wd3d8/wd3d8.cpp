// Original: d:\build\custom\temp\code\client\device\wd3d8\wd3d8.cpp
// The DLL's WList instances allocate with the CRT (__imp__malloc / __imp__free),
// not with Wangreal's g_mem (wmemblock.h).
#define WLIST_ALLOC(size) malloc(size)
#define WLIST_FREE(ptr) free(ptr)

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "defines.h"
#include "screencape.h"
#include "bitmap.h"
#include "wdxtc.h"
#include "device.h"
#include "wd3d8.h"
#include "wlocalize.h"

#ifdef PG_MATERIAL_HINTS
// Port only (port/compat/pg_material_hints.h): the merge buffer holds the
// draws of one material hint, set again around its device draw.
#include "pg_material_hints.h"

static int s_pgMergeHint = 0;
#endif

// The VC7.1 <xmmintrin.h> is missing from externs/vs-2003: these are its
// declarations of the SSE type and the intrinsics FillVertex uses.
#ifndef _INCLUDED_MM2
#define _INCLUDED_MM2
typedef struct __declspec(intrin_type) __declspec(align(16)) __m128 {
	float m128_f32[4];
} __m128;

extern "C" {
extern __m128 _mm_add_ps(__m128 _A, __m128 _B);
extern __m128 _mm_mul_ps(__m128 _A, __m128 _B);
extern __m128 _mm_set_ps1(float _A);
extern __m128 _mm_set_ps(float _A, float _B, float _C, float _D);
extern __m128 _mm_load_ps1(float const* _A);
}
#endif

// The Windows version the DLL detected at start-up (?g_winVer@@3W4eWindowsVersion@@A,
// PDB enum). Its declaring header is not named by any line record yet.
enum eWindowsVersion {
	WV_NONE = 0,
	WV_WINDOWS_OLD = 1,
	WV_WINDOWS_NT = 2,
	WV_WINDOWS_2000 = 3,
	WV_WINDOWS_XP = 4,
	WV_WINDOWS_VISTA = 5,
	WV_WINDOWS_7 = 6
};

extern eWindowsVersion g_winVer;

// wd3d8.cpp:46
eWindowsVersion GetWindowsVersion(void) {
	OSVERSIONINFOEXA osver;
	eWindowsVersion ver = WV_NONE;

	ZeroMemory(&osver, sizeof(OSVERSIONINFOEXA));

	osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
	if (!GetVersionExA((OSVERSIONINFOA*)&osver)) {
		// Windows NT 4.0 SP5 and older only take the short structure.
		osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
		if (!GetVersionExA((OSVERSIONINFOA*)&osver))
			return WV_NONE;
	}

	switch (osver.dwPlatformId) {
	case VER_PLATFORM_WIN32_NT:
		if (osver.dwMajorVersion <= 4)
			ver = WV_WINDOWS_NT;
		if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 0)
			ver = WV_WINDOWS_2000;
		else if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 1)
			ver = WV_WINDOWS_XP;
		else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 0)
			ver = WV_WINDOWS_VISTA;
		else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 1)
			ver = WV_WINDOWS_7;
		break;

	case VER_PLATFORM_WIN32s:
	case VER_PLATFORM_WIN32_WINDOWS:
		ver = WV_WINDOWS_OLD;
		break;
	}

	return ver;
}

// wd3d8.cpp:94
IDirect3D9 *fnWD3D_Create(void) {
	return Direct3DCreate9(D3D_SDK_VERSION);
}

// wd3d8.cpp:123
long fnWD3DDevice_GetRenderTarget(IDirect3DDevice9 *dev, unsigned long rtIdx, IDirect3DSurface9 **surf) {
	return dev->GetRenderTarget(rtIdx, surf);
}

// wd3d8.cpp:132
long fnWD3DDevice_CreateSurface(IDirect3DDevice9 *dev, unsigned int w, unsigned int h, _D3DFORMAT fmt, _D3DPOOL pool, IDirect3DSurface9 **surf, void **sharedHandle) {
	return dev->CreateOffscreenPlainSurface(w, h, fmt, pool, surf, sharedHandle);
}

// wd3d8.cpp:141
long fnWD3DDevice_CreateTexture(IDirect3DDevice9 *dev, unsigned int w, unsigned int h, unsigned int lvl, unsigned long usage, _D3DFORMAT fmt, _D3DPOOL pool, IDirect3DTexture9 **tex, void **sharedHandle) {
	return dev->CreateTexture(w, h, lvl, usage, fmt, pool, tex, sharedHandle);
}

// wd3d8.cpp:155
long fnWD3DDevice_CreateVertexBuffer(IDirect3DDevice9 *dev, unsigned int len, unsigned long usage, unsigned long fvf, _D3DPOOL pool, IDirect3DVertexBuffer9 **vb, void **sharedHandle) {
	return dev->CreateVertexBuffer(len, usage, fvf, pool, vb, sharedHandle);
}

// wd3d8.cpp:169
long fnWD3DDevice_CreateIndexBuffer(IDirect3DDevice9 *dev, unsigned int len, unsigned long usage, _D3DFORMAT fmt, _D3DPOOL pool, IDirect3DIndexBuffer9 **ib, void **sharedHandle) {
	return dev->CreateIndexBuffer(len, usage, fmt, pool, ib, sharedHandle);
}

// wd3d8.cpp:183
long fnWD3DDevice_SetStreamSource(IDirect3DDevice9 *dev, unsigned int strmNo, IDirect3DVertexBuffer9 *vb, unsigned int offset, unsigned int stride) {
	return dev->SetStreamSource(strmNo, vb, offset, stride);
}

// wd3d8.cpp:192
long fnWD3DDevice_SetIndices(IDirect3DDevice9 *dev, IDirect3DIndexBuffer9 *ib, unsigned int baseVtxIdx) {
	return dev->SetIndices(ib);
}

// wd3d8.cpp:201
long fnWD3DDevice_DrawIndexedPrimitive(IDirect3DDevice9 *dev, _D3DPRIMITIVETYPE primType, int baseVtxIdx, unsigned int minIdx, unsigned int numVtxs, unsigned int startIdx, unsigned int primCount) {
	return dev->DrawIndexedPrimitive(primType, baseVtxIdx, minIdx, numVtxs, startIdx, primCount);
}

// wd3d8.cpp:210
long fnWD3DDevice_CopyRect(IDirect3DDevice9 *dev, IDirect3DSurface9 *srcSurf, const tagRECT *srcRc, IDirect3DSurface9 *dstSurf) {
	return dev->StretchRect(srcSurf, srcRc, dstSurf, srcRc, D3DTEXF_NONE);
}

// wd3d8.cpp:219
long fnWD3DVertexBuffer_Lock(IDirect3DVertexBuffer9 *vb, unsigned int offset, unsigned int size, unsigned char **data, unsigned long flags) {
	return vb->Lock(offset, size, (void **)data, flags);
}

// wd3d8.cpp:228
long fnWD3DIndexBuffer_Lock(IDirect3DIndexBuffer9 *ib, unsigned int offset, unsigned int size, unsigned char **data, unsigned long flags) {
	return ib->Lock(offset, size, (void **)data, flags);
}

// wd3d8.cpp:245
long fnWD3DX_D3DXCreateTexture(IDirect3DDevice9 *pDevice, unsigned int Width, unsigned int Height, unsigned int MipLevels, unsigned long Usage, _D3DFORMAT Format, _D3DPOOL Pool, IDirect3DTexture9 **ppTexture) {
	return D3DXCreateTexture(pDevice, Width, Height, MipLevels, Usage, Format, Pool, ppTexture);
}

// wd3d8.cpp:272
long fnWD3DX_D3DXCreateTextureFromFileInMemoryEx(IDirect3DDevice9 *pDevice, const void *pSrcData, unsigned int SrcDataSize, unsigned int Width, unsigned int Height, unsigned int MipLevels, unsigned long Usage, _D3DFORMAT Format, _D3DPOOL Pool, unsigned long Filter, unsigned long MipFilter, unsigned long ColorKey, _D3DXIMAGE_INFO *pSrcInfo, tagPALETTEENTRY *pPalette, IDirect3DTexture9 **ppTexture) {
	return D3DXCreateTextureFromFileInMemoryEx(pDevice, pSrcData, SrcDataSize, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
}

namespace nsWindowUtility {

// Loads dwmapi.dll at run time to query and switch desktop composition
// (PDB layout, size 0x10).
class CDWMApiDll {
	LAYOUT_CHECK_FRIEND;
public:
	typedef HRESULT (__stdcall *pfDwmIsCompositionEnabled)(int *);
	typedef HRESULT (__stdcall *pfDwmEnableComposition)(int);

	CDWMApiDll(void);
	virtual ~CDWMApiDll(void) {
		if (m_hDWMApi) {
			FreeLibrary(m_hDWMApi);
			m_hDWMApi = NULL;
		}
	}

	int Load_DWMAPIDll(void);
	int IsCompositionEnabled(void);
	HRESULT EnableComposition(int bEnable);

private:
	HINSTANCE__ *m_hDWMApi;                     // 0x4
	pfDwmIsCompositionEnabled m_fIsEnabled;     // 0x8
	pfDwmEnableComposition m_fEnable;           // 0xc
};

CHECK_SIZE(CDWMApiDll, 0x10);
BEGIN_LAYOUT_CHECK(CDWMApiDll)
	CHECK_FIELD(CDWMApiDll, m_hDWMApi, 0x4);
	CHECK_FIELD(CDWMApiDll, m_fIsEnabled, 0x8);
	CHECK_FIELD(CDWMApiDll, m_fEnable, 0xc);
END_LAYOUT_CHECK

// wd3d8.cpp:330
CDWMApiDll::CDWMApiDll(void) {
	m_hDWMApi = NULL;
	m_fIsEnabled = NULL;
	m_fEnable = NULL;
}

// wd3d8.cpp:347
int CDWMApiDll::Load_DWMAPIDll(void) {
	m_hDWMApi = LoadLibraryA("dwmapi.dll");
	if (m_hDWMApi == NULL) return FALSE;

	m_fIsEnabled = (pfDwmIsCompositionEnabled)GetProcAddress(m_hDWMApi, "DwmIsCompositionEnabled");
	m_fEnable = (pfDwmEnableComposition)GetProcAddress(m_hDWMApi, "DwmEnableComposition");

	return TRUE;
}

// wd3d8.cpp:359
int CDWMApiDll::IsCompositionEnabled(void) {
	int bEnabled = FALSE;

	if (m_fIsEnabled) {
		if (SUCCEEDED(m_fIsEnabled(&bEnabled))) {
			if (bEnabled == TRUE) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

// wd3d8.cpp:377
HRESULT CDWMApiDll::EnableComposition(int bEnable) {
	if (m_fEnable == NULL || m_fIsEnabled == NULL) return E_FAIL;
	return m_fEnable(bEnable);
}

}   // namespace nsWindowUtility

// How the screen is captured and presented (PDB layout, size 0x10).
struct sCpatureOption {
	enum eCaptureMode {
		CM_A = 0,
		CM_B = 1,
		CM_Q = 2
	};

	sCpatureOption(void);
	void SetMode(eCaptureMode mode, bool windowed);

	eCaptureMode currentMode;                         // 0x0
	bool useMemCopy;                                  // 0x4
	bool updateWholeScreen;                           // 0x5
	bool dxCopyRectsOK;                               // 0x6
	bool useTexture;                                  // 0x7
	_D3DSWAPEFFECT SwapEffect;                        // 0x8
	unsigned int FullScreen_PresentationInterval;     // 0xc
};

CHECK_SIZE(sCpatureOption, 0x10);
CHECK_FIELD(sCpatureOption, currentMode, 0x0);
CHECK_FIELD(sCpatureOption, useMemCopy, 0x4);
CHECK_FIELD(sCpatureOption, updateWholeScreen, 0x5);
CHECK_FIELD(sCpatureOption, dxCopyRectsOK, 0x6);
CHECK_FIELD(sCpatureOption, useTexture, 0x7);
CHECK_FIELD(sCpatureOption, SwapEffect, 0x8);
CHECK_FIELD(sCpatureOption, FullScreen_PresentationInterval, 0xc);

// wd3d8.cpp:451
sCpatureOption::sCpatureOption(void) {
	dxCopyRectsOK = true;
	useTexture = false;
}

// wd3d8.cpp:460
void sCpatureOption::SetMode(eCaptureMode mode, bool windowed) {
	currentMode = mode;
	switch (mode) {
	case CM_A:
		FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
		SwapEffect = D3DSWAPEFFECT_COPY;
		useMemCopy = true;
		updateWholeScreen = true;
		useTexture = false;
		break;
	case CM_B:
		FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		SwapEffect = D3DSWAPEFFECT_COPY;
		useMemCopy = false;
		updateWholeScreen = false;
		useTexture = g_winVer >= WV_WINDOWS_VISTA;
		break;
	}
}

static sCpatureOption g_captureOption;

// Set by Reset when the back buffer size, depth or window mode changes
// (a file-local of the DLL, 0x1004409c).
static bool g_formatChanged;

static D3DXMATRIX g_mId(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

// wd3d8.cpp:565
static void SetD3DMATRIXFromWMatrix(_D3DMATRIX& d3dm, const WMatrix& wm) {
	*(WVector*)&d3dm._11 = wm.xa;
	*(WVector*)&d3dm._21 = wm.ya;
	*(WVector*)&d3dm._31 = wm.za;
	*(WVector*)&d3dm._41 = wm.pivot;
	d3dm._14 = d3dm._24 = d3dm._34 = 0;
	d3dm._44 = 1.0f;
}

// The device's full-screen splash surface, split into up to 64 textures
// (PDB layout, size 0x724).
class WSplashD3D : public WSplash {
public:
	struct _MYVERTEX {
		// wd3d8.cpp:647
		_MYVERTEX() {}
		// wd3d8.cpp:648
		_MYVERTEX(const _D3DVECTOR& v, float _rhw, unsigned long _color, float _tu, float _tv) {
			sx = v.x;
			sy = v.y;
			sz = v.z;
			rhw = _rhw;
			color = _color;
			tu = _tu;
			tv = _tv;
		}

		float sx;               // 0x0
		float sy;               // 0x4
		float sz;               // 0x8
		float rhw;              // 0xc
		unsigned long color;    // 0x10
		float tu;               // 0x14
		float tv;               // 0x18
	};
	typedef _MYVERTEX MYVERTEX;
	typedef _MYVERTEX* LPMYVERTEX;

	WSplashD3D(WDirect3D8* pDriver);
	virtual ~WSplashD3D();
	virtual int Init(tagBITMAPINFO& info, void* bits, bool bScale);
	virtual int InitFromScreen(bool bScale);
	virtual void Reset();
	virtual void Draw(unsigned long color);
	virtual void Draw(const WPoint* pos, const WRect* src, unsigned long color);
	virtual void ResetScreenSize();
	virtual int GetWidth();
	virtual int GetHeight();
	virtual void SetTexCoordOffset(float u, float v);

protected:
	WDirect3D8* m_pDriver;      // 0x4
	int m_hTexs[64];            // 0x8
	tagRECT m_rects[64];        // 0x108
	WVector2D m_uvs[64];        // 0x508
	WVector2D m_offset;         // 0x708
	int m_iBufWidth;            // 0x710
	int m_iBufHeight;           // 0x714
	int m_iSrcWidth;            // 0x718
	int m_iSrcHeight;           // 0x71c
	int m_nSurfCount;           // 0x720
};
CHECK_SIZE(WSplashD3D::_MYVERTEX, 0x1c);
CHECK_SIZE(WSplashD3D, 0x724);

// wd3d8.cpp:663
WSplashD3D::WSplashD3D(WDirect3D8* pDriver) : m_pDriver(pDriver) {
	m_nSurfCount = 0; memset(m_hTexs, 0, sizeof(m_hTexs));
}

// wd3d8.cpp:668
WSplashD3D::~WSplashD3D() {
	for (int i = 0; i < m_nSurfCount; i++) {
		if (m_hTexs[i]) {
			m_pDriver->DestroyTexture(m_hTexs[i]);
			m_hTexs[i] = 0;
		}
	}
}

// wd3d8.cpp:682
int WSplashD3D::Init(tagBITMAPINFO& bi, void* data, bool fitToScreen) {
	m_offset.x = 0;
	m_offset.y = 0;
	m_nSurfCount = 0;
	m_iSrcWidth = bi.bmiHeader.biWidth;
	m_iSrcHeight = bi.bmiHeader.biHeight;

	if (fitToScreen) {
		m_iBufWidth = m_pDriver->m_d3dpp.BackBufferWidth;
		m_iBufHeight = m_pDriver->m_d3dpp.BackBufferHeight;
	} else {
		m_iBufWidth = m_iSrcWidth;
		m_iBufHeight = m_iSrcHeight;
	}

	int maxWidth = m_pDriver->m_d3dcaps.MaxTextureWidth;
	int maxHeight = m_pDriver->m_d3dcaps.MaxTextureHeight;

	if (m_iSrcWidth <= maxWidth && m_iSrcHeight <= maxHeight) {
		if (m_hTexs[0] == 0) {
			m_hTexs[0] = m_pDriver->CreateTexture(&bi, 0x80000);
			if (m_hTexs[0] == 0)
				return 0;
			m_pDriver->UpdateTexture(m_hTexs[0], &bi, data, 0x80000);
		}

		m_rects[0].left = 0;
		m_rects[0].top = 0;
		m_rects[0].right = m_iBufWidth;
		m_rects[0].bottom = m_iBufHeight;
		m_nSurfCount = 1;
		m_uvs[0].x = (float)bi.bmiHeader.biWidth / m_pDriver->GetTextureWidth(m_hTexs[0]);
		m_uvs[0].y = (float)bi.bmiHeader.biHeight / m_pDriver->GetTextureHeight(m_hTexs[0]);
	} else {
		Bitmap bitmap(maxWidth, maxHeight, bi.bmiHeader.biBitCount);
		int numX = (m_iSrcWidth + maxWidth - 1) / maxWidth;
		int numY = (m_iSrcHeight + maxHeight - 1) / maxHeight;
		int cps = bi.bmiHeader.biBitCount / 8;
		int srcBpl = (bi.bmiHeader.biWidth * cps + 3) & ~3;

		for (int i = 0; i < numY; i++) {
			for (int j = 0; j < numX; j++) {
				m_hTexs[m_nSurfCount] = m_pDriver->CreateTexture(bitmap.bi, 0x80000);

				if (m_hTexs[m_nSurfCount] == 0)
					return 0;

				int widthToFill;
				if ((j + 1) * maxWidth > bi.bmiHeader.biWidth)
					widthToFill = bi.bmiHeader.biWidth - j * maxWidth;
				else
					widthToFill = maxWidth;

				int heightToFill;
				if ((i + 1) * maxHeight > bi.bmiHeader.biHeight)
					heightToFill = bi.bmiHeader.biHeight - i * maxHeight;
				else
					heightToFill = maxHeight;

				int k;
				for (k = 0; k < heightToFill; k++)
					memcpy(bitmap.GetVram(k), (unsigned char*)data + (i * maxHeight + k) * srcBpl + j * maxWidth * cps, widthToFill * cps);

				if (widthToFill < maxWidth) {
					for (k = 0; k < heightToFill; k++)
						memset(bitmap.GetVram(k) + widthToFill * cps, 0, (maxWidth - widthToFill) * cps);
				}

				if (heightToFill < maxHeight) {
					for (k = heightToFill; k < maxHeight; k++)
						memset(bitmap.GetVram(k), 0, maxWidth * cps);
				}

				m_pDriver->UpdateTexture(m_hTexs[m_nSurfCount], bitmap.bi, bitmap.vram, 0x80000);

				m_rects[m_nSurfCount].left = m_iBufWidth * j * maxWidth / m_iSrcWidth;
				m_rects[m_nSurfCount].right = m_iBufWidth * (j + 1) * maxWidth / m_iSrcWidth;
				m_rects[m_nSurfCount].top = m_iBufHeight * i * maxHeight / m_iSrcHeight;
				m_rects[m_nSurfCount].bottom = m_iBufHeight * (i + 1) * maxHeight / m_iSrcHeight;

				if (m_rects[m_nSurfCount].right > m_iBufWidth)
					m_rects[m_nSurfCount].right = m_iBufWidth;

				if (m_rects[m_nSurfCount].bottom > m_iBufHeight)
					m_rects[m_nSurfCount].bottom = m_iBufHeight;

				tagRECT dest;
				dest.left = j * maxWidth;
				dest.right = (j + 1) * maxWidth;
				dest.top = i * maxHeight;
				dest.bottom = (i + 1) * maxHeight;

				if (dest.right > m_iSrcWidth) dest.right = m_iSrcWidth;
				if (dest.bottom > m_iSrcHeight) dest.bottom = m_iSrcHeight;

				dest.right -= dest.left;
				dest.bottom -= dest.top;

				m_uvs[m_nSurfCount].x = (float)dest.right / maxWidth;
				m_uvs[m_nSurfCount].y = (float)dest.bottom / maxHeight;

				m_nSurfCount++;
			}
		}
	}

	for (int i = m_nSurfCount; i < 64; i++) {
		if (m_hTexs[i] > 0) {
			m_pDriver->DestroyTexture(m_hTexs[i]);
			m_hTexs[i] = 0;
		}
	}

	return 1;
}

// wd3d8.cpp:825
int WSplashD3D::InitFromScreen(bool bCopy) {
	m_nSurfCount = 0;
	m_iBufWidth = m_pDriver->m_d3dpp.BackBufferWidth;
	m_iBufHeight = m_pDriver->m_d3dpp.BackBufferHeight;

	IDirect3DSurface9* pRenderTarget;
	_D3DSURFACE_DESC sd;

	if (FAILED(m_pDriver->m_pd3dDevice->GetRenderTarget(0, &pRenderTarget)))
		return 0;

	if (FAILED(pRenderTarget->GetDesc(&sd))) {
		pRenderTarget->Release();
		return 0;
	}

	WDirect3D8::pix_info* pixInfo = WDirect3D8::FindPixInfoByFormat(sd.Format);

	if (pixInfo == NULL) {
		pRenderTarget->Release();
		return 0;
	}

	unsigned int maxWidth = m_pDriver->m_d3dcaps.MaxTextureWidth;
	unsigned int maxHeight = m_pDriver->m_d3dcaps.MaxTextureHeight;

	maxWidth = Min<unsigned int>(maxWidth, m_iBufWidth);
	maxHeight = Min<unsigned int>(maxHeight, m_iBufHeight);

	int numX = (m_iBufWidth + maxWidth - 1) / maxWidth;
	int numY = (m_iBufHeight + maxHeight - 1) / maxHeight;

	tagBITMAPINFO bi;
	memset(&bi, 0, sizeof(bi));

	bi.bmiHeader.biWidth = maxWidth;
	bi.bmiHeader.biHeight = maxHeight;
	bi.bmiHeader.biBitCount = (WORD)(pixInfo->cpp * 8);

	for (int i = 0; i < numY; i++) {
		for (int j = 0; j < numX; j++) {
			if (m_hTexs[m_nSurfCount] == 0) {
				m_hTexs[m_nSurfCount] = m_pDriver->CreateTexture(&bi, 0x80000);

				if (m_hTexs[m_nSurfCount] == 0) {
					pRenderTarget->Release();
					return 0;
				}

				m_pDriver->xInstantiateTexture(m_hTexs[m_nSurfCount]);

				if (m_pDriver->m_texList[m_hTexs[m_nSurfCount]].pTex == NULL) {
					m_pDriver->DestroyTexture(m_hTexs[m_nSurfCount]);
					pRenderTarget->Release();
					return 0;
				}
			}

			float u = (float)bi.bmiHeader.biWidth / m_pDriver->GetTextureWidth(m_hTexs[m_nSurfCount]);
			float v = (float)bi.bmiHeader.biHeight / m_pDriver->GetTextureHeight(m_hTexs[m_nSurfCount]);

			m_rects[m_nSurfCount].left = j * maxWidth;
			m_rects[m_nSurfCount].right = (j + 1) * maxWidth;
			m_rects[m_nSurfCount].top = i * maxHeight;
			m_rects[m_nSurfCount].bottom = (i + 1) * maxHeight;

			if (m_rects[m_nSurfCount].right > m_iBufWidth) {
				m_uvs[m_nSurfCount].x = u - (float)(m_rects[m_nSurfCount].right - m_iBufWidth) / maxWidth;
				m_rects[m_nSurfCount].right = m_iBufWidth;
			} else {
				m_uvs[m_nSurfCount].x = u;
			}

			if (m_rects[m_nSurfCount].bottom > m_iBufHeight) {
				m_uvs[m_nSurfCount].y = v - (float)(m_rects[m_nSurfCount].bottom - m_iBufHeight) / maxHeight;
				m_rects[m_nSurfCount].bottom = m_iBufHeight;
			} else {
				m_uvs[m_nSurfCount].y = v;
			}

			if (bCopy) {
				tagRECT src = { j * maxWidth, i * maxHeight,
					(j + 1) * maxWidth, (i + 1) * maxHeight };

				if (src.right > m_iBufWidth) src.right = m_iBufWidth;
				if (src.bottom > m_iBufHeight) src.bottom = m_iBufHeight;

				tagRECT dest = { 0, 0, src.right - src.left, src.bottom - src.top };

				IDirect3DSurface9* targSurf;
				if (SUCCEEDED(m_pDriver->m_texList[m_hTexs[m_nSurfCount]].pTex->GetSurfaceLevel(0, &targSurf))) {
					D3DXLoadSurfaceFromSurface(targSurf, NULL, &dest, pRenderTarget, NULL, &src, D3DX_FILTER_NONE, 0);
					targSurf->Release();
				}
			}

			m_nSurfCount++;
		}
	}

	for (int k = m_nSurfCount; k < 64; k++) {
		if (m_hTexs[k] > 0)
			m_pDriver->DestroyTexture(m_hTexs[k]);
	}

	pRenderTarget->Release();

	return 1;
}

// wd3d8.cpp:974
void WSplashD3D::Reset() {
	if (m_nSurfCount == 0) return;

	IDirect3DSurface9* pRenderTarget;
	_D3DSURFACE_DESC sdRenderTarget;
	_D3DSURFACE_DESC sdSplashTex;

	if (FAILED(m_pDriver->m_pd3dDevice->GetRenderTarget(0, &pRenderTarget))) return;

	if (FAILED(pRenderTarget->GetDesc(&sdRenderTarget))) {
		pRenderTarget->Release();
		return;
	}

	pRenderTarget->Release();

	m_pDriver->m_texList[m_hTexs[0]].pTex->GetLevelDesc(0, &sdSplashTex);

	if (sdRenderTarget.Format != sdSplashTex.Format) {
		for (int i = 0; i < m_nSurfCount; i++) {
			if (m_hTexs[i]) {
				m_pDriver->DestroyTexture(m_hTexs[i]);
				m_hTexs[i] = 0;
			}
		}
	}

	InitFromScreen(false);
}

// wd3d8.cpp:1015
void WSplashD3D::SetTexCoordOffset(float u, float v) {
	m_offset.x = u;
	m_offset.y = v;
}

// wd3d8.cpp:1021
void WSplashD3D::Draw(const WPoint* pSrc, const WRect* pDest, unsigned long color) {
	if (m_pDriver->GetDeviceState() == W_VDEVSTATE_LOST)
		return;

	for (int i = 0; i < m_nSurfCount; i++) {
		float l = (float)m_rects[i].left, ol = l;
		float r = (float)m_rects[i].right, or = r;
		float t = (float)m_rects[i].top, ot = t;
		float b = (float)m_rects[i].bottom, ob = b;

		float su = 0, u = m_uvs[i].x;
		float sv = 0, v = m_uvs[i].y;

		if (pSrc) {
			if (pSrc->x > r || pSrc->y > b)
				continue;

			if (l < pSrc->x) {
				su = (pSrc->x - l) / (r - l) * m_uvs[i].x;
				l = pSrc->x;
			}

			if (pSrc->y > t) {
				sv = (pSrc->y - t) / (b - t) * m_uvs[i].y;
				t = pSrc->y;
			}

			l -= pSrc->x;
			r -= pSrc->x;
			t -= pSrc->y;
			b -= pSrc->y;

			if (pDest) {
				if (l > pDest->w || pDest->h < t)
					continue;

				if (pDest->w < r) {
					u = (pSrc->x + pDest->w - l) / (or - ol) * m_uvs[i].x;
					r = pDest->w;
				}

				if (pDest->h < b) {
					v = (pSrc->y + pDest->h - t) / (ob - ot) * m_uvs[i].y;
					b = pDest->h;
				}

				l += pDest->x;
				r += pDest->x;
				b += pDest->y;
				t += pDest->y;
			}
		}

		l -= 0.5f;
		r -= 0.5f;
		t -= 0.5f;
		b -= 0.5f;

		MYVERTEX rect[4];
		rect[0] = MYVERTEX(D3DXVECTOR3(l, b, 0.001f), 1.0f, color, su, v);
		rect[1] = MYVERTEX(D3DXVECTOR3(l, t, 0.001f), 1.0f, color, su, sv);
		rect[2] = MYVERTEX(D3DXVECTOR3(r, b, 0.001f), 1.0f, color, u, v);
		rect[3] = MYVERTEX(D3DXVECTOR3(r, t, 0.001f), 1.0f, color, u, sv);

		m_pDriver->DrawPrimitive((m_hTexs[i] & 0x7ff) | 0x22300000, 4, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, rect, D3DPT_TRIANGLESTRIP, 0x404);
	}
}

// wd3d8.cpp:1117
void WSplashD3D::Draw(unsigned long color) {
	Draw(NULL, NULL, color);
}

// wd3d8.cpp:1122
void WSplashD3D::ResetScreenSize() {
	float fSclX = (float)m_pDriver->m_d3dpp.BackBufferWidth / m_iBufWidth;
	float fSclY = (float)m_pDriver->m_d3dpp.BackBufferHeight / m_iBufHeight;

	m_iBufWidth = m_pDriver->m_d3dpp.BackBufferWidth;
	m_iBufHeight = m_pDriver->m_d3dpp.BackBufferHeight;

	for (int i = 0; i < m_nSurfCount; i++) {
		m_rects[i].left *= fSclX;
		m_rects[i].top *= fSclY;
		m_rects[i].right *= fSclX;
		m_rects[i].bottom *= fSclY;
	}
}

// wd3d8.cpp:1139
int WSplashD3D::GetWidth() {
	return m_iSrcWidth;
}

// wd3d8.cpp:1144
int WSplashD3D::GetHeight() {
	return m_iSrcHeight;
}

// wd3d8.cpp:1152
void WDirect3D8::sMergeBuffer::CheckAndIncreaseVertexStreamBuffer(unsigned long size) {
	if (VertexStreamBufferSize < size) {
		VertexStreamBufferSize = size;
		VertexStreamBuffer = (unsigned char*)realloc(VertexStreamBuffer, size);
	}
}

// wd3d8.cpp:1164
void WDirect3D8::sMergeBuffer::CheckAndIncreaseIndexBuffer(unsigned long size) {
	if (IndexBufferSize < size) {
		IndexBufferSize = size;
		IndexBuffer = (unsigned short*)realloc(IndexBuffer, size);
	}
}

// wd3d8.cpp:1179
WDirect3D8::sRtBackup::sRtBackup() {
	Init();
}

// wd3d8.cpp:1184
void WDirect3D8::sRtBackup::Init() {
	m_rt.Init();
	m_depthSurf = NULL;
	memset(&m_viewport, 0, sizeof(m_viewport));
}

// wd3d8.cpp:1191
void WDirect3D8::sRtBackup::Restore(IDirect3DDevice9* dev, const sRtBackup& curRt) {
	for (int i = 0; i < 2; i++) {
		if (m_rt.m_surf[i] != curRt.m_rt.m_surf[i])
			dev->SetRenderTarget(i, m_rt.m_surf[i]);
	}

	if (m_depthSurf != curRt.m_depthSurf)
		dev->SetDepthStencilSurface(m_depthSurf);

	if (memcmp(&m_viewport, &curRt.m_viewport, sizeof(m_viewport)) != 0)
		dev->SetViewport(&m_viewport);
}

// wd3d8.cpp:1220
void WDirect3D8::WFxParamPool::Create(IDirect3DDevice9* pDevice) {
}

// wd3d8.cpp:1229
void WDirect3D8::WFxParamPool::Release() {
	ClearParamCache();

	if (m_pool) { m_pool->Release(); m_pool = NULL; }
}

// wd3d8.cpp:1236
void WDirect3D8::WFxParamPool::OnLostDevice(IDirect3DDevice9* pDevice) {
}

// wd3d8.cpp:1240
void WDirect3D8::WFxParamPool::OnResetDevice(IDirect3DDevice9* pDevice) {
	ClearParamCache();
}

// Looks a parameter up in one of the pool's caches (by type when shared, by
// handle otherwise) and returns from the getter with the cached value.
#define FX_PARAM_CACHE_GET(T, cache, value) \
	if (isShared) { \
		std::map<WVDFXPARAMETERTYPE, T>::const_iterator itrShared = cache.first.find(wvdFxParamType); \
		if (itrShared != cache.first.end()) { value = itrShared->second; return; } \
	} else { \
		std::map<const char*, T>::const_iterator itr = cache.second.find(hParam); \
		if (itr != cache.second.end()) { value = itr->second; return; } \
	}

// Records a parameter's value in one of the pool's caches (by type when
// shared, by handle otherwise) and returns from the setter when the cached
// value is already the same.
#define FX_PARAM_CACHE_SET(T, cache, NOT_EQUAL) \
	if (isShared) { \
		std::map<WVDFXPARAMETERTYPE, T>::iterator itrShared = cache.first.find(wvdFxParamType); \
		if (itrShared == cache.first.end()) cache.first.insert(std::map<WVDFXPARAMETERTYPE, T>::value_type(wvdFxParamType, Value)); \
		else if (NOT_EQUAL(itrShared->second, Value)) itrShared->second = Value; \
		else return; \
	} else { \
		std::map<const char*, T>::iterator itr = cache.second.find(hParam); \
		if (itr == cache.second.end()) cache.second.insert(std::map<const char*, T>::value_type(hParam, Value)); \
		else if (NOT_EQUAL(itr->second, Value)) itr->second = Value; \
		else return; \
	}

#define FX_VALUE_NOT_EQUAL(a, b) ((a) != (b))

// wd3d8.cpp:1321
void WDirect3D8::WFxParamPool::GetInt(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, int& Value) const {
	if (ef && hParam) {
		FX_PARAM_CACHE_GET(int, m_paramCacheListPair_Int, Value)

		ef->GetInt(hParam, &Value);
	}
}

// wd3d8.cpp:1329
void WDirect3D8::WFxParamPool::SetInt(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, int Value) {
	if (ef && hParam) {
		FX_PARAM_CACHE_SET(int, m_paramCacheListPair_Int, FX_VALUE_NOT_EQUAL)

		ef->SetInt(hParam, Value);
	}
}

// wd3d8.cpp:1337
void WDirect3D8::WFxParamPool::GetVector2(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, D3DXVECTOR2& Value) const {
	if (ef && hParam) {
		FX_PARAM_CACHE_GET(D3DXVECTOR2, m_paramCacheListPair_Vec2, Value)

		ef->GetFloatArray(hParam, (float*)&Value, 2);
	}
}

// wd3d8.cpp:1345
void WDirect3D8::WFxParamPool::SetVector2(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, const D3DXVECTOR2& Value) {
	if (ef && hParam) {
		FX_PARAM_CACHE_SET(D3DXVECTOR2, m_paramCacheListPair_Vec2, FX_VALUE_NOT_EQUAL)

		ef->SetFloatArray(hParam, (const float*)&Value, 2);
	}
}

// wd3d8.cpp:1353
void WDirect3D8::WFxParamPool::GetVector3(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, D3DXVECTOR3& Value) const {
	if (ef && hParam) {
		FX_PARAM_CACHE_GET(D3DXVECTOR3, m_paramCacheListPair_Vec3, Value)

		ef->GetFloatArray(hParam, (float*)&Value, 3);
	}
}

// wd3d8.cpp:1361
void WDirect3D8::WFxParamPool::SetVector3(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, const D3DXVECTOR3& Value) {
	if (ef && hParam) {
		FX_PARAM_CACHE_SET(D3DXVECTOR3, m_paramCacheListPair_Vec3, FX_VALUE_NOT_EQUAL)

		ef->SetFloatArray(hParam, (const float*)&Value, 3);
	}
}

// wd3d8.cpp:1369
void WDirect3D8::WFxParamPool::GetVector4(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, D3DXVECTOR4& Value) const {
	if (ef && hParam) {
		FX_PARAM_CACHE_GET(D3DXVECTOR4, m_paramCacheListPair_Vec4, Value)

		ef->GetVector(hParam, &Value);
	}
}

// wd3d8.cpp:1377
void WDirect3D8::WFxParamPool::SetVector4(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, const D3DXVECTOR4& Value) {
	if (ef && hParam) {
		FX_PARAM_CACHE_SET(D3DXVECTOR4, m_paramCacheListPair_Vec4, FX_VALUE_NOT_EQUAL)

		ef->SetVector(hParam, &Value);
	}
}

// wd3d8.cpp:1385
void WDirect3D8::WFxParamPool::GetMatrix(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, D3DXMATRIX& Value) const {
	if (ef && hParam) {
		FX_PARAM_CACHE_GET(D3DXMATRIX, m_paramCacheListPair_Mat, Value)

		ef->GetMatrix(hParam, &Value);
	}
}

// wd3d8.cpp:1393
void WDirect3D8::WFxParamPool::SetMatrix(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, const D3DXMATRIX& Value) {
	if (ef && hParam) {
		FX_PARAM_CACHE_SET(D3DXMATRIX, m_paramCacheListPair_Mat, FX_VALUE_NOT_EQUAL)

		ef->SetMatrix(hParam, &Value);
	}
}

// wd3d8.cpp:1401
void WDirect3D8::WFxParamPool::GetTexture(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, IDirect3DBaseTexture9*& Value) const {
	if (ef && hParam) {
		FX_PARAM_CACHE_GET(IDirect3DBaseTexture9*, m_paramCacheListPair_Tex, Value)

		ef->GetTexture(hParam, &Value);
	}
}

// wd3d8.cpp:1409
void WDirect3D8::WFxParamPool::SetTexture(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, IDirect3DBaseTexture9* Value) {
	if (ef && hParam) {
		FX_PARAM_CACHE_SET(IDirect3DBaseTexture9*, m_paramCacheListPair_Tex, FX_VALUE_NOT_EQUAL)

		ef->SetTexture(hParam, Value);
	}
}

// wd3d8.cpp:1417
void WDirect3D8::WFxParamPool::ClearParamCache() {
	m_paramCacheListPair_Int.first.clear();
	m_paramCacheListPair_Int.second.clear();
	m_paramCacheListPair_Vec2.first.clear();
	m_paramCacheListPair_Vec2.second.clear();
	m_paramCacheListPair_Vec3.first.clear();
	m_paramCacheListPair_Vec3.second.clear();
	m_paramCacheListPair_Vec4.first.clear();
	m_paramCacheListPair_Vec4.second.clear();
	m_paramCacheListPair_Mat.first.clear();
	m_paramCacheListPair_Mat.second.clear();
	m_paramCacheListPair_Tex.first.clear();
	m_paramCacheListPair_Tex.second.clear();
}

// Every record of this list has a record of the same key in rhs's list, with
// the same value.
#define RS_COMPARE_LIST(ListType, list) \
	for (ListType::const_iterator itr = list.begin(); itr != list.end(); ++itr) { \
		ListType::const_iterator found; \
		for (found = rhs.list.begin(); found != rhs.list.end(); ++found) { \
			if (itr->m_Type == found->m_Type) \
				break; \
		} \
		if (found == rhs.list.end()) \
			return false; \
		if (itr->m_Value != found->m_Value) \
			return false; \
	}

// The same for a list keyed by a stage or sampler and a type.
#define RS_COMPARE_LIST2(ListType, list) \
	for (ListType::const_iterator itr = list.begin(); itr != list.end(); ++itr) { \
		ListType::const_iterator found; \
		for (found = rhs.list.begin(); found != rhs.list.end(); ++found) { \
			if (itr->m_Type == found->m_Type && itr->m_Type2 == found->m_Type2) \
				break; \
		} \
		if (found == rhs.list.end()) \
			return false; \
		if (itr->m_Value != found->m_Value) \
			return false; \
	}

// wd3d8.cpp:1438
bool WDirect3D8::WRenderState::operator==(const WRenderState& rhs) const {
	if (m_fxMacro != rhs.m_fxMacro)
		return false;

	if (m_rsList.size() != rhs.m_rsList.size())
		return false;
	if (m_tssList.size() != rhs.m_tssList.size())
		return false;
	if (m_transfList.size() != rhs.m_transfList.size())
		return false;
	if (m_texList.size() != rhs.m_texList.size())
		return false;
	if (m_clipPlaneList.size() != rhs.m_clipPlaneList.size())
		return false;
	if (m_ssList.size() != rhs.m_ssList.size())
		return false;
	if (m_fxParamIntList.size() != rhs.m_fxParamIntList.size())
		return false;
	if (m_fxParamVec2List.size() != rhs.m_fxParamVec2List.size())
		return false;
	if (m_fxParamVec3List.size() != rhs.m_fxParamVec3List.size())
		return false;
	if (m_fxParamVec4List.size() != rhs.m_fxParamVec4List.size())
		return false;
	if (m_fxParamMatList.size() != rhs.m_fxParamMatList.size())
		return false;
	if (m_fxParamTexList.size() != rhs.m_fxParamTexList.size())
		return false;

	RS_COMPARE_LIST(RsList, m_rsList);
	RS_COMPARE_LIST2(TssList, m_tssList);
	RS_COMPARE_LIST(TransfList, m_transfList);
	RS_COMPARE_LIST(TexList, m_texList);
	RS_COMPARE_LIST(ClipPlaneList, m_clipPlaneList);

	RS_COMPARE_LIST2(SsList, m_ssList);

	RS_COMPARE_LIST(FxParamIntList, m_fxParamIntList);
	RS_COMPARE_LIST(FxParamVec2List, m_fxParamVec2List);
	RS_COMPARE_LIST(FxParamVec3List, m_fxParamVec3List);
	RS_COMPARE_LIST(FxParamVec4List, m_fxParamVec4List);
	RS_COMPARE_LIST(FxParamMatList, m_fxParamMatList);
	RS_COMPARE_LIST(FxParamTexList, m_fxParamTexList);

	return true;
}

// Records a state keyed by its type: updates the entry already in the list,
// or appends a new one.
#define RS_SET_STATE(ListType, list, EntryType, type, value) \
	{ \
		ListType::iterator it; \
		for (it = list.begin(); it != list.end(); ++it) { \
			if (it->m_Type == type) { \
				it->m_Value = value; \
				break; \
			} \
		} \
		if (it == list.end()) list.push_back(EntryType(type, value)); \
	}

// The same for a state keyed by a stage or sampler and a type.
#define RS_SET_STATE2(ListType, list, EntryType, type, type2, value) \
	{ \
		ListType::iterator it; \
		for (it = list.begin(); it != list.end(); ++it) { \
			if (it->m_Type == type && it->m_Type2 == type2) { \
				it->m_Value = value; \
				break; \
			} \
		} \
		if (it == list.end()) list.push_back(EntryType(type, type2, value)); \
	}

// wd3d8.cpp:1560
void WDirect3D8::WRenderState::SetRenderState(_D3DRENDERSTATETYPE State, unsigned long Value) {
	RS_SET_STATE(RsList, m_rsList, sRs, State, Value);
}

// wd3d8.cpp:1565
void WDirect3D8::WRenderState::SetTextureStageState(unsigned long Stage, _D3DTEXTURESTAGESTATETYPE Type, unsigned long Value) {
	RS_SET_STATE2(TssList, m_tssList, sTss, Stage, Type, Value);
}

// wd3d8.cpp:1570
void WDirect3D8::WRenderState::SetTransform(_D3DTRANSFORMSTATETYPE State, const D3DXMATRIX& Matrix) {
	RS_SET_STATE(TransfList, m_transfList, sTransf, State, Matrix);
}

// wd3d8.cpp:1575
void WDirect3D8::WRenderState::SetTexture(unsigned long Stage, IDirect3DBaseTexture9* pTexture) {
	RS_SET_STATE(TexList, m_texList, sTex, Stage, pTexture);
}

// wd3d8.cpp:1580
void WDirect3D8::WRenderState::SetClipPlane(unsigned long Index, const D3DXPLANE& Plane) {
	RS_SET_STATE(ClipPlaneList, m_clipPlaneList, sClipPlane, Index, Plane);
}

// wd3d8.cpp:1586
void WDirect3D8::WRenderState::SetSamplerState(unsigned long Sampler, _D3DSAMPLERSTATETYPE Type, unsigned long Value) {
	RS_SET_STATE2(SsList, m_ssList, sSs, Sampler, Type, Value);
}

// wd3d8.cpp:1591
void WDirect3D8::WRenderState::SetFxMacro(unsigned long FxMacro) {
	m_fxMacro = FxMacro;
}

// wd3d8.cpp:1597
void WDirect3D8::WRenderState::SetFxParamInt(WVDFXPARAMETERTYPE ParamType, int Value) {
	RS_SET_STATE(FxParamIntList, m_fxParamIntList, sFxParamInt, ParamType, Value);
}

// wd3d8.cpp:1602
void WDirect3D8::WRenderState::SetFxParamVector2(WVDFXPARAMETERTYPE ParamType, const D3DXVECTOR2& Value) {
	RS_SET_STATE(FxParamVec2List, m_fxParamVec2List, sFxParamVec2, ParamType, Value);
}

// wd3d8.cpp:1607
void WDirect3D8::WRenderState::SetFxParamVector3(WVDFXPARAMETERTYPE ParamType, const D3DXVECTOR3& Value) {
	RS_SET_STATE(FxParamVec3List, m_fxParamVec3List, sFxParamVec3, ParamType, Value);
}

// wd3d8.cpp:1612
void WDirect3D8::WRenderState::SetFxParamVector4(WVDFXPARAMETERTYPE ParamType, const D3DXVECTOR4& Value) {
	RS_SET_STATE(FxParamVec4List, m_fxParamVec4List, sFxParamVec4, ParamType, Value);
}

// wd3d8.cpp:1617
void WDirect3D8::WRenderState::SetFxParamMatrix(WVDFXPARAMETERTYPE ParamType, const D3DXMATRIX& Value) {
	RS_SET_STATE(FxParamMatList, m_fxParamMatList, sFxParamMat, ParamType, Value);
}

// wd3d8.cpp:1622
void WDirect3D8::WRenderState::SetFxParamTexture(WVDFXPARAMETERTYPE ParamType, IDirect3DBaseTexture9* pTexture) {
	RS_SET_STATE(FxParamTexList, m_fxParamTexList, sFxParamTex, ParamType, pTexture);
}

// wd3d8.cpp:1628
void WDirect3D8::WRenderState::Begin(WDirect3D8& wd3d) {
	if (!HasAnyState())
		return;

	for (RsList::iterator itr = m_rsList.begin(); itr != m_rsList.end(); ++itr) {
		wd3d.m_pd3dDevice->GetRenderState(itr->m_Type, &itr->m_OldValue);
		if (itr->m_Value != itr->m_OldValue)
			wd3d.m_pd3dDevice->SetRenderState(itr->m_Type, itr->m_Value);
	}

	for (TssList::iterator itr = m_tssList.begin(); itr != m_tssList.end(); ++itr) {
		wd3d.m_pd3dDevice->GetTextureStageState(itr->m_Type, itr->m_Type2, &itr->m_OldValue);
		if (itr->m_Value != itr->m_OldValue)
			wd3d.m_pd3dDevice->SetTextureStageState(itr->m_Type, itr->m_Type2, itr->m_Value);
	}

	for (TransfList::iterator itr = m_transfList.begin(); itr != m_transfList.end(); ++itr) {
		wd3d.m_pd3dDevice->GetTransform(itr->m_Type, &itr->m_OldValue);
		if (itr->m_Value != itr->m_OldValue)
			wd3d.m_pd3dDevice->SetTransform(itr->m_Type, &itr->m_Value);
	}

	for (TexList::iterator itr = m_texList.begin(); itr != m_texList.end(); ++itr) {
		wd3d.m_pd3dDevice->GetTexture(itr->m_Type, &itr->m_OldValue);
		if (itr->m_Value != itr->m_OldValue)
			wd3d.m_pd3dDevice->SetTexture(itr->m_Type, itr->m_Value);
		if (itr->m_OldValue)
			itr->m_OldValue->Release();
	}

	for (ClipPlaneList::iterator itr = m_clipPlaneList.begin(); itr != m_clipPlaneList.end(); ++itr) {
		wd3d.m_pd3dDevice->GetClipPlane(itr->m_Type, itr->m_OldValue);
		if (wd3d.m_lastEffect) {
			// The effect's planes are in clip space: carry the plane through
			// the inverse-transposed view and projection matrices.
			D3DXMATRIX view;
			D3DXMatrixInverse(&view, NULL, &wd3d.m_xLastViewMatrix);
			D3DXMatrixTranspose(&view, &view);
			D3DXPlaneTransform(&itr->m_SetValue, &itr->m_Value, &view);
			D3DXMATRIX proj;
			D3DXMatrixInverse(&proj, NULL, &wd3d.m_xLastProjMatrix);
			D3DXMatrixTranspose(&proj, &proj);
			D3DXPlaneTransform(&itr->m_SetValue, &itr->m_SetValue, &proj);
		} else {
			itr->m_SetValue = itr->m_Value;
		}
		if (itr->m_SetValue != itr->m_OldValue)
			wd3d.m_pd3dDevice->SetClipPlane(itr->m_Type, itr->m_SetValue);
	}

	for (SsList::iterator itr = m_ssList.begin(); itr != m_ssList.end(); ++itr) {
		wd3d.m_pd3dDevice->GetSamplerState(itr->m_Type, itr->m_Type2, &itr->m_OldValue);
		if (itr->m_Value != itr->m_OldValue)
			wd3d.m_pd3dDevice->SetSamplerState(itr->m_Type, itr->m_Type2, itr->m_Value);
	}

	if (!wd3d.m_lastEffect)
		return;

	sEffect& effect = wd3d.m_EffectTable[wd3d.m_lastEffect];
	if (!effect.pEffect)
		return;

	for (FxParamIntList::iterator itr = m_fxParamIntList.begin(); itr != m_fxParamIntList.end(); ++itr) {
		if (effect.param[itr->m_Type].handle) {
			wd3d.GetFxParamPool().GetInt(effect.pEffect, itr->m_Type, effect.param[itr->m_Type].handle, effect.param[itr->m_Type].isShared, itr->m_OldValue);
			if (itr->m_Value != itr->m_OldValue)
				wd3d.GetFxParamPool().SetInt(effect.pEffect, itr->m_Type, effect.param[itr->m_Type].handle, effect.param[itr->m_Type].isShared, itr->m_Value);
		}
	}

	for (FxParamVec2List::iterator itr = m_fxParamVec2List.begin(); itr != m_fxParamVec2List.end(); ++itr) {
		if (effect.param[itr->m_Type].handle) {
			wd3d.GetFxParamPool().GetVector2(effect.pEffect, itr->m_Type, effect.param[itr->m_Type].handle, effect.param[itr->m_Type].isShared, itr->m_OldValue);
			if (itr->m_Value != itr->m_OldValue)
				wd3d.GetFxParamPool().SetVector2(effect.pEffect, itr->m_Type, effect.param[itr->m_Type].handle, effect.param[itr->m_Type].isShared, itr->m_Value);
		}
	}

	for (FxParamVec3List::iterator itr = m_fxParamVec3List.begin(); itr != m_fxParamVec3List.end(); ++itr) {
		if (effect.param[itr->m_Type].handle) {
			wd3d.GetFxParamPool().GetVector3(effect.pEffect, itr->m_Type, effect.param[itr->m_Type].handle, effect.param[itr->m_Type].isShared, itr->m_OldValue);
			if (itr->m_Value != itr->m_OldValue)
				wd3d.GetFxParamPool().SetVector3(effect.pEffect, itr->m_Type, effect.param[itr->m_Type].handle, effect.param[itr->m_Type].isShared, itr->m_Value);
		}
	}

	for (FxParamVec4List::iterator itr = m_fxParamVec4List.begin(); itr != m_fxParamVec4List.end(); ++itr) {
		if (effect.param[itr->m_Type].handle) {
			wd3d.GetFxParamPool().GetVector4(effect.pEffect, itr->m_Type, effect.param[itr->m_Type].handle, effect.param[itr->m_Type].isShared, itr->m_OldValue);
			if (itr->m_Value != itr->m_OldValue)
				wd3d.GetFxParamPool().SetVector4(effect.pEffect, itr->m_Type, effect.param[itr->m_Type].handle, effect.param[itr->m_Type].isShared, itr->m_Value);
		}
	}

	for (FxParamMatList::iterator itr = m_fxParamMatList.begin(); itr != m_fxParamMatList.end(); ++itr) {
		if (effect.param[itr->m_Type].handle) {
			wd3d.GetFxParamPool().GetMatrix(effect.pEffect, itr->m_Type, effect.param[itr->m_Type].handle, effect.param[itr->m_Type].isShared, itr->m_OldValue);
			if (itr->m_Value != itr->m_OldValue)
				wd3d.GetFxParamPool().SetMatrix(effect.pEffect, itr->m_Type, effect.param[itr->m_Type].handle, effect.param[itr->m_Type].isShared, itr->m_Value);
		}
	}

	for (FxParamTexList::iterator itr = m_fxParamTexList.begin(); itr != m_fxParamTexList.end(); ++itr) {
		if (effect.param[itr->m_Type].handle) {
			wd3d.GetFxParamPool().GetTexture(effect.pEffect, itr->m_Type, effect.param[itr->m_Type].handle, effect.param[itr->m_Type].isShared, itr->m_OldValue);
			if (itr->m_Value != itr->m_OldValue)
				wd3d.GetFxParamPool().SetTexture(effect.pEffect, itr->m_Type, effect.param[itr->m_Type].handle, effect.param[itr->m_Type].isShared, itr->m_Value);
			if (itr->m_OldValue)
				itr->m_OldValue->Release();
		}
	}

	effect.pEffect->CommitChanges();
}

// wd3d8.cpp:1869
void WDirect3D8::WRenderState::End(WDirect3D8& wd3d) {
	if (!HasAnyState())
		return;

	for (RsList::iterator itr = m_rsList.begin(); itr != m_rsList.end(); ++itr) {
		if (itr->m_OldValue != itr->m_Value)
			wd3d.m_pd3dDevice->SetRenderState(itr->m_Type, itr->m_OldValue);
	}

	for (TssList::iterator itr = m_tssList.begin(); itr != m_tssList.end(); ++itr) {
		if (itr->m_OldValue != itr->m_Value)
			wd3d.m_pd3dDevice->SetTextureStageState(itr->m_Type, itr->m_Type2, itr->m_OldValue);
	}

	for (TransfList::iterator itr = m_transfList.begin(); itr != m_transfList.end(); ++itr) {
		if (itr->m_OldValue != itr->m_Value)
			wd3d.m_pd3dDevice->SetTransform(itr->m_Type, &itr->m_OldValue);
	}

	for (TexList::iterator itr = m_texList.begin(); itr != m_texList.end(); ++itr) {
		if (itr->m_OldValue != itr->m_Value)
			wd3d.m_pd3dDevice->SetTexture(itr->m_Type, itr->m_OldValue);
	}

	for (ClipPlaneList::iterator itr = m_clipPlaneList.begin(); itr != m_clipPlaneList.end(); ++itr) {
		if (itr->m_OldValue != itr->m_SetValue)
			wd3d.m_pd3dDevice->SetClipPlane(itr->m_Type, itr->m_OldValue);
	}

	for (SsList::iterator itr = m_ssList.begin(); itr != m_ssList.end(); ++itr) {
		if (itr->m_OldValue != itr->m_Value)
			wd3d.m_pd3dDevice->SetSamplerState(itr->m_Type, itr->m_Type2, itr->m_OldValue);
	}

	if (!wd3d.m_lastEffect)
		return;

	sEffect& effect = wd3d.m_EffectTable[wd3d.m_lastEffect];
	if (!effect.pEffect)
		return;

	for (FxParamIntList::iterator itr = m_fxParamIntList.begin(); itr != m_fxParamIntList.end(); ++itr) {
		if (effect.param[itr->m_Type].handle) {
			if (itr->m_OldValue != itr->m_Value)
				wd3d.GetFxParamPool().SetInt(effect.pEffect, itr->m_Type, effect.param[itr->m_Type].handle, effect.param[itr->m_Type].isShared, itr->m_OldValue);
		}
	}

	for (FxParamVec2List::iterator itr = m_fxParamVec2List.begin(); itr != m_fxParamVec2List.end(); ++itr) {
		if (effect.param[itr->m_Type].handle) {
			if (itr->m_OldValue != itr->m_Value)
				wd3d.GetFxParamPool().SetVector2(effect.pEffect, itr->m_Type, effect.param[itr->m_Type].handle, effect.param[itr->m_Type].isShared, itr->m_OldValue);
		}
	}

	for (FxParamVec3List::iterator itr = m_fxParamVec3List.begin(); itr != m_fxParamVec3List.end(); ++itr) {
		if (effect.param[itr->m_Type].handle) {
			if (itr->m_OldValue != itr->m_Value)
				wd3d.GetFxParamPool().SetVector3(effect.pEffect, itr->m_Type, effect.param[itr->m_Type].handle, effect.param[itr->m_Type].isShared, itr->m_OldValue);
		}
	}

	for (FxParamVec4List::iterator itr = m_fxParamVec4List.begin(); itr != m_fxParamVec4List.end(); ++itr) {
		if (effect.param[itr->m_Type].handle) {
			if (itr->m_OldValue != itr->m_Value)
				wd3d.GetFxParamPool().SetVector4(effect.pEffect, itr->m_Type, effect.param[itr->m_Type].handle, effect.param[itr->m_Type].isShared, itr->m_OldValue);
		}
	}

	for (FxParamMatList::iterator itr = m_fxParamMatList.begin(); itr != m_fxParamMatList.end(); ++itr) {
		if (effect.param[itr->m_Type].handle) {
			if (itr->m_OldValue != itr->m_Value)
				wd3d.GetFxParamPool().SetMatrix(effect.pEffect, itr->m_Type, effect.param[itr->m_Type].handle, effect.param[itr->m_Type].isShared, itr->m_OldValue);
		}
	}

	for (FxParamTexList::iterator itr = m_fxParamTexList.begin(); itr != m_fxParamTexList.end(); ++itr) {
		if (effect.param[itr->m_Type].handle) {
			if (itr->m_OldValue != itr->m_Value)
				wd3d.GetFxParamPool().SetTexture(effect.pEffect, itr->m_Type, effect.param[itr->m_Type].handle, effect.param[itr->m_Type].isShared, itr->m_OldValue);
		}
	}

	wd3d.m_EffectTable[wd3d.m_lastEffect].pEffect->CommitChanges();
}

// The Direct3D texture-stage state of each of the engine's state types 0-5
// (0x10043090). Type 6, the sampler state right after it in the data
// (g_TexSamplerStateList, 0x100430a8), is read through the same index.
static _D3DTEXTURESTAGESTATETYPE g_TexStageStateList[6] = {
	D3DTSS_COLORARG1, D3DTSS_COLORARG2, D3DTSS_COLOROP,
	D3DTSS_ALPHAARG1, D3DTSS_ALPHAARG2, D3DTSS_ALPHAOP
};
static _D3DSAMPLERSTATETYPE g_TexSamplerStateList[1] = {
	D3DSAMP_MIPFILTER
};

// The back-buffer formats a display mode can use, with their names and depths.
WDirect3D8::FMTLIST WDirect3D8::ms_fmtList[5] = {
	{ D3DFMT_R5G6B5, "R5G6B5", 16 },
	{ D3DFMT_X1R5G5B5, "X1R5G5B5", 16 },
	{ D3DFMT_A1R5G5B5, "A1R5G5B5", 16 },
	{ D3DFMT_X8R8G8B8, "X8R8G8B8", 32 },
	{ D3DFMT_A8R8G8B8, "A8R8G8B8", 32 }
};

// The system directories searched for the display driver's file
// (WX_VDEV_DRIVER_INFO).
static char* DriverPath[4] = {
	"C:/WINNT/system/",
	"C:/WINNT/system32/",
	"C:/WINDOWS/system/",
	"C:/WINDOWS/system32/"
};

// The texture formats tried, in order, for each of the engine's six texture
// format types.
_D3DFORMAT WDirect3D8::ms_fmtRecomList[6][4] = {
	{ D3DFMT_R5G6B5, D3DFMT_X1R5G5B5, D3DFMT_A1R5G5B5, D3DFMT_R8G8B8 },
	{ D3DFMT_A1R5G5B5, D3DFMT_A4R4G4B4, D3DFMT_A8R8G8B8, D3DFMT_A8R3G3B2 },
	{ D3DFMT_A4R4G4B4, D3DFMT_R8G8B8, D3DFMT_A8R3G3B2, D3DFMT_A1R5G5B5 },
	{ D3DFMT_R8G8B8, D3DFMT_X8R8G8B8, D3DFMT_R5G6B5, D3DFMT_A1R5G5B5 },
	{ D3DFMT_A8R8G8B8, D3DFMT_A1R5G5B5, D3DFMT_A4R4G4B4, D3DFMT_UNKNOWN },
	{ D3DFMT_A8R8G8B8, D3DFMT_A4R4G4B4, D3DFMT_A1R5G5B5, D3DFMT_UNKNOWN }
};

// The channel shifts of each texture format the engine converts bitmaps to:
// format, bytes per pixel, then the right/left shift of red, green, blue and
// alpha (an 8-bit channel is shifted right, then left into place).
WDirect3D8::pix_info WDirect3D8::ms_fmtTypeList[9] = {
	{ D3DFMT_R5G6B5, 2, 3, 11, 2, 5, 3, 0, 8, 0 },
	{ D3DFMT_R8G8B8, 3, 0, 16, 0, 8, 0, 0, 8, 0 },
	{ D3DFMT_A1R5G5B5, 2, 3, 10, 3, 5, 3, 0, 7, 15 },
	{ D3DFMT_X1R5G5B5, 2, 3, 10, 3, 5, 3, 0, 8, 16 },
	{ D3DFMT_R3G3B2, 1, 5, 5, 5, 2, 6, 0, 8, 0 },
	{ D3DFMT_A4R4G4B4, 2, 4, 8, 4, 4, 4, 0, 4, 12 },
	{ D3DFMT_A8R8G8B8, 4, 0, 16, 0, 8, 0, 0, 0, 24 },
	{ D3DFMT_A8R3G3B2, 2, 5, 5, 5, 2, 6, 0, 0, 8 },
	{ D3DFMT_X8R8G8B8, 4, 0, 16, 0, 8, 0, 0, 8, 24 }
};

// The render-target formats tried, in order, with their bytes per pixel.
WDirect3D8::sRtFormat WDirect3D8::ms_RtFmt[4] = {
	{ D3DFMT_A8R8G8B8, 4 },
	{ D3DFMT_A4R4G4B4, 2 },
	{ D3DFMT_A1R5G5B5, 2 },
	{ D3DFMT_A8R3G3B2, 2 }
};
WDirect3D8::sRtFormat WDirect3D8::ms_RtNoAlphaFmt[3] = {
	{ D3DFMT_X8R8G8B8, 4 },
	{ D3DFMT_R5G6B5, 2 },
	{ D3DFMT_X1R5G5B5, 2 }
};
WDirect3D8::sRtFormat WDirect3D8::ms_RtDepthFmt[5] = {
	{ D3DFMT_D24X8, 4 },
	{ D3DFMT_D16, 2 },
	{ D3DFMT_D24S8, 4 },
	{ D3DFMT_D24X4S4, 4 },
	{ D3DFMT_D15S1, 2 }
};

// The depth histogram W_VDEV_GET_ZBUFF_HISTOGRAM fills (0x10043c98).
unsigned long WDirect3D8::ms_ZbuffHistogram[256];

// The spherical-harmonics coefficients a new device starts with (7 rows of
// 4, read in order), as in the DLL's data (0x10043700).
static float _shcoeff[28] = {
	-0.33615f, 0.940161f, 0.127409f, 1.14203f,
	-0.33537f, 1.16755f, 0.100581f, 1.3035f,
	-0.28732f, 1.35096f, 0.036699f, 1.39474f,
	-0.168298f, 0.0227974f, -0.220767f, -0.159484f,
	-0.149726f, -0.0385005f, -0.296651f, -0.140673f,
	-0.106568f, -0.126265f, -0.361535f, -0.102911f,
	0.0286815f, -0.00496096f, -0.060617f, 1.0f
};

// The message of the last device error, shown when the device is destroyed
// (a file-local of the DLL, 0x100440a0).
static const char* g_error = NULL;

// The "Direct3D 9 cannot be initialised" message (?g_msgD3DInitFailed@@3PBDB:
// a const pointer to const char, 0x10043320, "Direct3D 9" followed by the
// Korean text). Only declared: cl 13.10 folds a use of this const pointer to
// 0 once its definition with the string is anywhere in the file, whereas the
// original reads it from memory.
extern const char* const g_msgD3DInitFailed;

// wd3d8.cpp:2076
WDirect3D8::WDirect3D8(char* devName, int id) {
	if (g_winVer == WV_NONE)
		g_winVer = GetWindowsVersion();

	if (devName) {
		m_devName = new char[strlen(devName) + 6];
		strcpy(m_devName, "[DX8]");
		strcat(m_devName, devName);
	}

	m_devId = id;
	m_modList = NULL;
	m_pd3dDevice = NULL;
	m_hwnd = NULL;
	m_BackBufBpp = 0;
	m_fps = 60.0f;
	m_d3d8 = NULL;
	m_fmtWindowed = D3DFMT_X8R8G8B8;
	m_clientRcCheckedAfterReset = true;
	m_pEventQuery = NULL;
	m_pCopiedScreenSurface = NULL;
	m_hCopiedScreenTexture = 0;
	m_pCopiedScreenSplash = NULL;
	m_useCopiedScreen = false;
	memset(m_texList, 0, sizeof(m_texList));
	memset(m_depthSurfList, 0, sizeof(m_depthSurfList));
	m_commonDepthSurf = NULL;

	memset(m_xaVbList, 0, sizeof(m_xaVbList));
	memset(m_xaIbList, 0, sizeof(m_xaIbList));

	m_bUseMipmap = false;
	m_iMaxMipLvl = 1;
	m_dwMipTexStateFilter = D3DTEXF_POINT;

	m_xnTotalTris = m_xnDPUPs = m_xnDIPUPs = m_xnDPs = m_xnDIPs = 0;
	m_iDDSRes = 0;
	m_dwMipCreateFilter = D3DX_FILTER_POINT;
	m_RtFmtIdx = -1;
	m_RtNoAlphaFmtIdx = -1;
	m_RtLowMemFmtIdx = -1;
	m_RtNoAlphaLowMemFmtIdx = -1;
	m_RtDepthFmtIdx = -1;
	m_xiVbSize = 0;
	m_pLockableVB = NULL;
	m_pLockableIB = NULL;
	memset(&m_MergeBuffer, 0, sizeof(m_MergeBuffer));

	m_Effect = 0xffffffff;

	for (int i = 0; i < 28; i++)
		m_SHCoeff[i / 4][i % 4] = _shcoeff[i];

	m_GlobalRS[0] = 0;
	m_GlobalRS[1] = 0;
	m_pScreenCape = NULL;
	m_pScreenCapeFactory = NULL;
	m_pkDWMApiDll = NULL;
	m_pkDWMApiDll = new nsWindowUtility::CDWMApiDll;
	if (m_pkDWMApiDll) {
		if (g_winVer < WV_WINDOWS_VISTA || !m_pkDWMApiDll->Load_DWMAPIDll()) {
			delete m_pkDWMApiDll;
			m_pkDWMApiDll = NULL;
		}
	}

	m_fillScrMode = false;
}

// wd3d8.cpp:2216
WDirect3D8::~WDirect3D8() {
	WDirect3D::Release();
	Release();

	if (g_error) {
		MessageBox(NULL, g_error, "\xba\xf1\xb5\xf0\xbf\xc0 \xc4\xab\xb5\xe5 \xbf\xa1\xb7\xaf", MB_OK);     // video card error
		g_error = NULL;
	}

	if (m_pkDWMApiDll) { delete m_pkDWMApiDll; m_pkDWMApiDll = NULL; }
}

// wd3d8.cpp:2232
void WDirect3D8::Release(void) {
	if (m_pLockableVB) { m_pLockableVB->Release(); m_pLockableVB = NULL; }
	if (m_pLockableIB) { m_pLockableIB->Release(); m_pLockableIB = NULL; }

	if (m_MergeBuffer.VertexStreamBuffer)
		free(m_MergeBuffer.VertexStreamBuffer);

	if (m_MergeBuffer.IndexBuffer)
		free(m_MergeBuffer.IndexBuffer);

	if (m_pScreenCape) { m_pScreenCape->Release(); m_pScreenCape = NULL; }
	if (m_pScreenCapeFactory) { delete m_pScreenCapeFactory; m_pScreenCapeFactory = NULL; }

	if (m_commonDepthSurf) { m_commonDepthSurf->Release(); m_commonDepthSurf = NULL; }

	for (int i = 0; i < 32; i++)
		if (m_depthSurfList[i].pDepth) { m_depthSurfList[i].pDepth->Release(); m_depthSurfList[i].pDepth = NULL; }

	ReleaseCopiedScreenResource();

	if (m_pd3dDevice)
		ReleaseAllRendertargetBackupResource();

	ReleaseShaderResource();

	if (m_devName) { delete[] m_devName; m_devName = NULL; }
	if (m_modList) { delete[] m_modList; m_modList = NULL; }

	ReleaseEventQuery();

	if (m_pd3dDevice) { m_pd3dDevice->Release(); m_pd3dDevice = NULL; }
	if (m_d3d8) { m_d3d8->Release(); m_d3d8 = NULL; }

	m_devId = -1;
}

// wd3d8.cpp:2292
int WDirect3D8::WinProc(unsigned int message, unsigned long wParam, unsigned long lParam) {
	switch (message) {
	case WM_PAINT:
		if (m_pd3dDevice && m_bWindow) {
			if (SUCCEEDED(m_pd3dDevice->TestCooperativeLevel()))
				m_pd3dDevice->Present(NULL, NULL, m_hwnd, NULL);
		}
		return -1;

	case WM_SYSCOMMAND:
		switch (wParam) {
		case SC_MOVE:
		case SC_SIZE:
		case SC_MAXIMIZE:
		case SC_KEYMENU:
		case SC_MONITORPOWER:
			if (!m_bWindow)
				return 1;
			break;
		}
		break;

	case WM_ACTIVATEAPP:
		return 1;
	}

	return 0;
}

// wd3d8.cpp:2340
void WDirect3D8::SetRenderState(unsigned long dwType, unsigned long dwTypeEx) {
	dwType &= 0xfffc0000;

	dwType |= dwTypeEx & 0x3fff;

	if (m_lastRenderState == dwType) return;

	FlushRenderPrimitive();

	SetBlendMode(dwType);

	unsigned long dwXor = m_lastRenderState ^ dwType;

	if (dwXor & 0x300000) {
		if ((m_lastRenderState & 0x300000) == 0x200000) {
			_SetRenderState(D3DRS_DEPTHBIAS, 0);
		}

		switch (dwType & 0x300000) {
		case 0:
			if ((m_lastRenderState & 0x300000) != 0x200000)
				_SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
			break;
		case 0x100000:
			_SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);
			break;
		case 0x300000:
			_SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
			break;
		case 0x200000:
			if (m_lastRenderState & 0x300000) {
				_SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
			}
			{
				static float fDepthBias = -0.0005f;
				_SetRenderState(D3DRS_DEPTHBIAS, *(unsigned long*)&fDepthBias);
			}
			break;
		}
	}

	if (dwXor & 0x2000000) {
		if (dwType & 0x2000000) {
			_SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			_SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		} else {
			_SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			_SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		}
	}

	if (dwXor & 0x8000000)
		_SetRenderState(D3DRS_FOGENABLE, (dwType >> 27) & 1);

	if (dwXor & 0x80000) {
		if (dwType & 0x80000) {
			_SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			_SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		} else {
			_SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			_SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		}
	}

	if (dwXor & 0x20000000)
		_SetRenderState(D3DRS_ZWRITEENABLE, ~(dwType >> 29) & 1);

	if (dwXor & 0x80000000)
		_SetRenderState(D3DRS_ALPHAREF, (dwType >> 24) & 0x80);

	if (dwXor & 0xc00) {
		switch (dwType & 0xc00) {
		case 0:
			_SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			break;
		case 0x400:
			_SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			break;
		case 0x800:
			_SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
			break;
		}
	}

	if (dwXor & 4) {
		if (dwType & 4)
			_SetRenderState(D3DRS_LIGHTING, FALSE);
		else
			_SetRenderState(D3DRS_LIGHTING, TRUE);
	} else if (dwXor & 0x10) {
		if (dwType & 0x10)
			_SetRenderState(D3DRS_LIGHTING, FALSE);
		else
			_SetRenderState(D3DRS_LIGHTING, TRUE);
	}

	if (dwXor & 0x2000)
		_SetRenderState(D3DRS_COLORVERTEX, ~(dwType >> 13) & 1);

	if (dwXor & 0x2014) {
		m_xbUseTFactor = (dwType & 0x14) && (dwType & 0x2000);

		SetTextureStageState(0, 1, m_xbUseTFactor ? 3 : 0);
		SetTextureStageState(0, 4, 0);
	}

	m_lastRenderState = dwType;
}

// wd3d8.cpp:2527
void WDirect3D8::SetBlendState(unsigned long srcblend, unsigned long dstblend) {
	if (m_blendEnable && srcblend == D3DBLEND_ONE && dstblend == D3DBLEND_ZERO) {
		_SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_blendEnable = 0;
		return;
	}

	if (!m_blendEnable) {
		_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_blendEnable = 1;
	}

	if (m_lastBlendState[0] != srcblend) {
		_SetRenderState(D3DRS_SRCBLEND, srcblend);
		m_lastBlendState[0] = srcblend;
	}

	if (m_lastBlendState[1] != dstblend) {
		_SetRenderState(D3DRS_DESTBLEND, dstblend);
		m_lastBlendState[1] = dstblend;
	}
}

// wd3d8.cpp:2556
void WDirect3D8::SetBlendMode(unsigned long dwState) {
	dwState &= 0x41800000;

	if (m_lastBlendMode == dwState) return;

	m_lastBlendMode = dwState;
	switch (dwState) {
	case 0:
		SetBlendState(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
		break;
	case 0x40000000:
		SetBlendState(D3DBLEND_ONE, D3DBLEND_ZERO);
		break;
	case 0x800000:
		SetBlendState(D3DBLEND_SRCALPHA, D3DBLEND_ONE);
		break;
	case 0x40800000:
		SetBlendState(D3DBLEND_ONE, D3DBLEND_ONE);
		break;
	case 0x1000000:
	case 0x41000000:
		SetBlendState(D3DBLEND_ZERO, D3DBLEND_SRCCOLOR);
		break;
	case 0x1800000:
	case 0x41800000:
		SetBlendState(D3DBLEND_ZERO, D3DBLEND_INVSRCCOLOR);
		break;
	}
}

// wd3d8.cpp:2606
void WDirect3D8::SetTextureStageState(int stage, int type, unsigned long value) {
	if (type >= 6 && stage < m_MaxTextureBlendStages && m_texStageState[stage][type - 6] != value) {
		_SetSamplerState(stage, (_D3DSAMPLERSTATETYPE)g_TexStageStateList[type], value);
		m_texStageState[stage][type] = value;
	} else if (type < sizeof(m_texStageState[0]) / sizeof(m_texStageState[0][0]) && stage < m_MaxTextureBlendStages && m_texStageState[stage][type] != value) {
		_SetTextureStageState(stage, (_D3DTEXTURESTAGESTATETYPE)g_TexStageStateList[type], value);
		m_texStageState[stage][type] = value;
	}
}

// wd3d8.cpp:2625
long WDirect3D8::SetVertexShader(unsigned long fvf) {
	if (m_xLastVertexDecl == fvf)
		return 0;

	FlushRenderPrimitive();

	m_xLastVertexDecl = fvf;
	if ((fvf & D3DFVF_POSITION_MASK) == D3DFVF_XYZ || (fvf & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW)
		return m_pd3dDevice->SetFVF(fvf);
	return m_pd3dDevice->SetFVF((fvf & ~(D3DFVF_POSITION_MASK | D3DFVF_LASTBETA_UBYTE4)) | D3DFVF_XYZ);
}

// wd3d8.cpp:2733
void WDirect3D8::SetVtxMode(unsigned long dwVertexTypeDesc) {
	m_lastVtxType = dwVertexTypeDesc & 0x7fffffff;
	SetVertexShader(dwVertexTypeDesc & 0x3fffffff);
	m_vtxSize = VertexSize(m_lastVtxType & 0xbfffffff);

	switch (m_lastVtxType & 0x7fffff7f) {
	case 0x102:
		if (m_xbUseTFactor)
			SetTextureStageState(0, 2, 4);
		else
			SetTextureStageState(0, 2, 2);
		SetTextureStageState(1, 2, 1);
		SetTextureStageState(0, 5, 4);
		break;
	case 0x104:
		SetTextureStageState(0, 2, 2);
		SetTextureStageState(1, 2, 1);
		SetTextureStageState(0, 5, 2);
		break;
	case 0x12:
	case 0x42:
	case 0x44:
	case 0x52:
		SetTextureStageState(0, 2, 3);
		SetTextureStageState(1, 2, 1);
		SetTextureStageState(0, 5, 3);
		break;
	case 0x112:
	case 0x142:
	case 0x144:
	case 0x152:
	case 0x1106:
	case 0x1108:
	case 0x110a:
	case 0x110c:
	case 0x1116:
	case 0x1118:
	case 0x111a:
	case 0x111c:
	case 0x1156:
	case 0x1158:
	case 0x115a:
	case 0x115c:
		SetTextureStageState(0, 2, 4);
		SetTextureStageState(1, 2, 1);
		SetTextureStageState(0, 5, 4);
		break;
	case 0x204:
		SetTextureStageState(0, 2, 2);
		SetTextureStageState(1, 2, 4);
		SetTextureStageState(0, 5, 2);
		break;
	case 0x40000204:
		SetTextureStageState(0, 2, 2);
		SetTextureStageState(1, 2, 7);
		SetTextureStageState(0, 5, 2);
		break;
	case 0x202:
	case 0x212:
	case 0x242:
	case 0x244:
	case 0x252:
		SetTextureStageState(0, 2, 4);
		SetTextureStageState(1, 2, 4);
		SetTextureStageState(0, 5, 4);
		break;
	case 0x40000202:
	case 0x40000212:
	case 0x40000242:
	case 0x40000244:
	case 0x40000252:
		SetTextureStageState(0, 2, 4);
		SetTextureStageState(1, 2, 7);
		SetTextureStageState(0, 5, 4);
		break;
	}
}

// wd3d8.cpp:2840
void WDirect3D8::SetTexture(unsigned long texState) {
	if (m_lastTexState != texState) {
		int hTex = texState & 0x7ff;
		if (hTex != (int)(m_lastTexState & 0x7ff)) {
			xInstantiateAndFillTexture(hTex);
			_SetTexture(0, m_texList[hTex].pTex);
		}

		hTex = (texState >> 11) & 0x7f;
		if (hTex != (int)((m_lastTexState >> 11) & 0x7f)) {
			xInstantiateAndFillTexture(hTex);
			_SetTexture(1, m_texList[hTex].pTex);
		}

		m_lastTexState = texState;
	}
}

// wd3d8.cpp:2870
void WDirect3D8::SetVtxType(unsigned long type, unsigned long type2, unsigned long vtxtype, unsigned long diffuse) {
	type |= m_GlobalRS[0];
	type2 |= m_GlobalRS[1];

	SetRenderState(type, type2);
	SetTexture(type & 0x3ffff);

	m_xbCurHwTnL = (type2 >> 22) != 0;
	xSetRenderState(type2, diffuse);
	xSetTnLBuffer(type2 >> 22, (type2 >> 14) & 0xff);

	if (m_xbCurHwTnL != m_xbLastHwTnL) {
		if (m_fog && (type & 0x08000000)) {
			unsigned long source = m_xbCurHwTnL ? D3DMCS_MATERIAL : D3DMCS_COLOR2;
			FlushRenderPrimitive();
			m_pd3dDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, source);
		}

		m_xbLastHwTnL = m_xbCurHwTnL;
	}

	SetVtxMode(((type & 0x40000) << 12) | vtxtype);

	unsigned long flags = 0;
	if (IsSupportVS() || IsSupportPS()) {
		flags = m_CustomRenderState.GetFxMacro();
		if (type & 0x7ff)
			flags |= 0x1;
		if (!(type & 0x40000000) && (vtxtype & D3DFVF_DIFFUSE))
			flags |= 0x4;

		if ((vtxtype & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW) {
			flags |= 0x2000;

			switch (type2 & 0x60) {
			case 0x20:
				flags |= 0x801;
				break;
			case 0x40:
				flags |= 0x1001;
				break;
			}
		} else {
			if (type & 0x3f800)
				flags |= 0x10;
			if (type & 0x40000)
				flags |= 0x20;
			if ((type2 & 0x100) && !(type & 0x01800000))
				flags |= 0x4000;
			if ((type2 & 0x80) && !(type & 0x01800000))
				flags |= 0x8000;
			if (m_LightEnable == true && m_xdwDiffuse != 0xffffffff)
				flags |= 0x8;
			if (type & 0x08000000)
				flags |= 0x80;
			if (vtxtype & D3DFVF_NORMAL)
				flags |= 0x2;
			if (m_xbUseTFactor == true)
				flags |= 0x40;
			if (type2 & 0x4)
				flags |= 0x100;
			if ((type2 & 0x200) && !(type2 & 0x4))
				flags |= 0x400;
		}

		if (!IsSupportVS())
			flags |= 0x20000000;

		if (!IsSupportPS())
			flags |= 0x40000000;
	}

	_SetShader(flags);
}

// wd3d8.cpp:3014
bool WDirect3D8::SetFogEnable(bool enable) {
	return WDirect3D::SetFogEnable(enable);
}

// wd3d8.cpp:3030
void WDirect3D8::SetFogState(float fogStart, float fogEnd, unsigned long fogColor) {
	if (fogStart != m_fogStart) {
		unsigned long value = *(unsigned long*)&fogStart; FlushRenderPrimitive(); m_LastFogStart = *(float*)&value; m_pd3dDevice->SetRenderState(D3DRS_FOGSTART, value);
	}

	if (fogEnd != m_fogEnd) {
		unsigned long value = *(unsigned long*)&fogEnd; FlushRenderPrimitive(); m_LastFogEnd = *(float*)&value; m_pd3dDevice->SetRenderState(D3DRS_FOGEND, value);
	}

	if (fogColor != m_fogColor) {
		FlushRenderPrimitive(); m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, fogColor);
	}

	WDirect3D::SetFogState(fogStart, fogEnd, fogColor);
}

// wd3d8.cpp:3082
void WDirect3D8::ResetFogState(void) {
	// Each line sets one fog state through a DWORD copy of the stored value
	// (the copy lives in memory because its address is reinterpreted).
	{ unsigned long fogStart = *(unsigned long*)&m_fogStart; FlushRenderPrimitive(); m_LastFogStart = *(float*)&fogStart; m_pd3dDevice->SetRenderState(D3DRS_FOGSTART, fogStart); }
	{ unsigned long fogEnd = *(unsigned long*)&m_fogEnd; FlushRenderPrimitive(); m_LastFogEnd = *(float*)&fogEnd; m_pd3dDevice->SetRenderState(D3DRS_FOGEND, fogEnd); }
	{ unsigned long fogColor = m_fogColor; FlushRenderPrimitive(); m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, fogColor); }
}

// wd3d8.cpp:3095
_D3DFORMAT WDirect3D8::FindDepthBufferFormat(_D3DFORMAT fmt) {
	for (int i = 0; i < sizeof(ms_RtDepthFmt) / sizeof(ms_RtDepthFmt[0]); i++) {
		if (m_d3d8->CheckDeviceFormat(m_devId, D3DDEVTYPE_HAL, fmt, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, ms_RtDepthFmt[i].fmt) == D3D_OK) {
			if (m_d3d8->CheckDepthStencilMatch(m_devId, D3DDEVTYPE_HAL, fmt, fmt, ms_RtDepthFmt[i].fmt) == D3D_OK) {
				return ms_RtDepthFmt[i].fmt;
			}
		}
	}

	return D3DFMT_D16;
}

// wd3d8.cpp:3127
int WDirect3D8::Init(char* modName, HWND__* hwnd, int iTnL) {
	D3DPERF_SetOptions(1);
	m_renderCount = 0;

	m_d3d8 = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_d3d8 == NULL) {
		g_error = g_msgD3DInitFailed;
		return 1;
	}

	m_d3d8->GetDeviceCaps(m_devId, D3DDEVTYPE_HAL, &m_d3dcaps);

	m_MaxTextureBlendStages = m_d3dcaps.MaxSimultaneousTextures;

	_D3DDISPLAYMODE d3ddm;
	ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));

	if (m_d3d8->GetAdapterDisplayMode(m_devId, &d3ddm) == D3D_OK)
		m_fmtWindowed = d3ddm.Format;

	if (strstr(modName, "Window")) {
		// Windowed: the back buffer takes the desktop's format and the
		// window's client size.
		if (m_d3d8->GetAdapterDisplayMode(m_devId, &d3ddm) != D3D_OK) {
			g_error = "\xc0\xfb\xc0\xfd\xc7\xd1 \xb5\xf0\xbd\xba\xc7\xc3\xb7\xb9\xc0\xcc \xb8\xf0\xb5\xe5\xb8\xa6 \xbe\xf2\xbe\xee \xbf\xc3 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";     // no suitable display mode
			return 4;
		}

		tagRECT rect;
		GetClientRect(hwnd, &rect);

		m_fps = d3ddm.RefreshRate == 0 ? 120.0f : (float)d3ddm.RefreshRate;
		m_bWindow = true;
		m_BackBufBpp = GetBackBufferBpp(d3ddm.Format);

		if (m_BackBufBpp == 0) {
			g_error = "\xc0\xfb\xc0\xfd\xc7\xd1 \xb5\xf0\xbd\xba\xc7\xc3\xb7\xb9\xc0\xcc \xb8\xf0\xb5\xe5\xb8\xa6 \xbe\xf2\xbe\xee \xbf\xc3 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";     // no suitable display mode
			return 4;
		}

		m_d3dpp.BackBufferWidth = rect.right - rect.left;
		m_d3dpp.BackBufferHeight = rect.bottom - rect.top;
		m_d3dpp.BackBufferFormat = d3ddm.Format;
		m_d3dpp.Windowed = TRUE;
		m_d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
		m_d3dpp.FullScreen_RefreshRateInHz = 0;
		m_d3dpp.PresentationInterval = 0;
	} else {
		// Full screen: the first format the adapter has modes for, then the
		// mode whose name ("w%d h%d b%d") is modName.
		const _D3DFORMAT _list[3] = { D3DFMT_R5G6B5, D3DFMT_X1R5G5B5, D3DFMT_X8R8G8B8 };
		_D3DFORMAT fmt = D3DFMT_X8R8G8B8;
		unsigned long i;
		for (i = 0; i < 3; i++) {
			if (m_d3d8->GetAdapterModeCount(0, _list[i])) {
				fmt = _list[i];
				break;
			}
		}

		_D3DDISPLAYMODE t;
		unsigned long m;
		for (i = 0, m = 0xffffffff; i < m_d3d8->GetAdapterModeCount(m_devId, fmt); i++) {
			char temp[64];

			m_d3d8->EnumAdapterModes(m_devId, fmt, i, &d3ddm);

			if (GetDevName(d3ddm, temp, sizeof(temp))) continue;

			if (strcmp(modName, temp) != 0) continue;

			if (m == 0xffffffff || (d3ddm.RefreshRate != 60 && d3ddm.RefreshRate == 0)) {
				t = d3ddm;
				m = i;
			}
		}

		if (m == 0xffffffff) {
			g_error = "\xc0\xfb\xc0\xfd\xc7\xd1 \xb8\xae\xc7\xc1\xb7\xb9\xbd\xc3 \xb7\xb9\xc0\xcc\xc6\xae\xb8\xa6 \xbe\xf2\xbe\xee \xbf\xc3 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";     // no suitable refresh rate
			return 2;
		}

		m_fps = t.RefreshRate == 0 ? 120.0f : (float)t.RefreshRate;
		m_bWindow = false;

		m_BackBufBpp = GetBackBufferBpp(t.Format);

		if (m_BackBufBpp == 0) {
			g_error = "\xc0\xfb\xc0\xfd\xc7\xd1 \xb5\xf0\xbd\xba\xc7\xc3\xb7\xb9\xc0\xcc \xb8\xf0\xb5\xe5\xb8\xa6 \xbe\xf2\xbe\xee \xbf\xc3 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";     // no suitable display mode
			return 4;
		}

		m_d3dpp.BackBufferWidth = t.Width;
		m_d3dpp.BackBufferHeight = t.Height;
		m_d3dpp.FullScreen_RefreshRateInHz = t.RefreshRate;
		m_d3dpp.Windowed = FALSE;
		m_d3dpp.BackBufferFormat = t.Format;
		m_d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
		m_d3dpp.PresentationInterval = 0;
	}

	m_d3dpp.hDeviceWindow = hwnd;
	m_d3dpp.BackBufferCount = 1;
	m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	m_d3dpp.EnableAutoDepthStencil = TRUE;
	m_d3dpp.AutoDepthStencilFormat = FindDepthBufferFormat(m_d3dpp.BackBufferFormat);
	m_d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	m_hwnd = hwnd;
	m_lWndStyle = -1;

	bool isSupportHwVs = IsSupportVS();
	bool isSupportHwPs = IsSupportPS();

	if ((m_d3dcaps.DevCaps & D3DDEVCAPS_HWRASTERIZATION) && (m_d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)) {
		m_xdwDevBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	} else {
		m_xdwDevBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	}

	if (m_d3d8->CreateDevice(m_devId, D3DDEVTYPE_HAL, hwnd, m_xdwDevBehavior, &m_d3dpp, &m_pd3dDevice) != D3D_OK) {
		g_error = "\xb5\xf0\xb9\xd9\xc0\xcc\xbd\xba\xb8\xa6 \xbb\xfd\xbc\xba\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";     // cannot create the device
		return 5;
	}

	CreateEventQuery();

	m_pd3dDevice->GetDeviceCaps(&m_d3dcaps);

	if (!isSupportHwVs || !isSupportHwPs) {
		m_d3dcaps.VertexShaderVersion = D3DVS_VERSION(0, 0);
		m_d3dcaps.PixelShaderVersion = D3DPS_VERSION(0, 0);
	}

	// Both results are unused (the body is empty in this build).
	if (IsSupportVS() || IsSupportPS()) {
	}

	m_pScreenCapeFactory = new Nv::Factory::ScreenCapeFactory;
	m_pScreenCape = m_pScreenCapeFactory->CreateScreenCapeLayer();
	if (m_pScreenCape) {
		m_pScreenCape->Initialize(hwnd, (void**)&m_pd3dDevice, Nv::GraphicDeviceType::DIRECT3D9);
	}

	m_pd3dDevice->ShowCursor(TRUE);

	SetTextureFormat(m_d3dpp.BackBufferFormat);

	SetRenderTargetFormat();

	SetDefaultState();

	m_pd3dDevice->BeginScene();
	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 0.0f, 0);
	m_pd3dDevice->EndScene();
	Present();

	WDirect3D::Init();

	if (m_pCopiedScreenSplash) {
		delete m_pCopiedScreenSplash;
		m_pCopiedScreenSplash = NULL;
	}

	m_pd3dDevice->SetVertexShader(NULL);
	m_pd3dDevice->SetPixelShader(NULL);

	BackupMainRenderTarget();
	m_curRt = m_mainRt;

	m_MergeBuffer.CheckAndIncreaseVertexStreamBuffer(0x23280);
	m_MergeBuffer.CheckAndIncreaseIndexBuffer(0x4650);

	return 0;
}

// wd3d8.cpp:3454
int WDirect3D8::GetBackBufferBpp(_D3DFORMAT fmtBackBuf) {
	int i;
	for (i = 4; i >= 0; i--) {
		if (ms_fmtList[i].format == fmtBackBuf) break;
	}
	if (i >= 0) {
		return ms_fmtList[i].bitNum;
	}
	return 0;
}

// wd3d8.cpp:3472
void WDirect3D8::SetDefaultState(void) {
	unsigned long value;
	int n;
	IDirect3DBaseTexture9* surf;

	{
		static const struct {
			_D3DRENDERSTATETYPE State;
			unsigned long Value;
		} def_render_state[] = {
			{ D3DRS_ZENABLE, D3DZB_TRUE },
			{ D3DRS_FILLMODE, D3DFILL_SOLID },
			{ D3DRS_SHADEMODE, D3DSHADE_GOURAUD },
			{ D3DRS_ZWRITEENABLE, TRUE },
			{ D3DRS_ALPHATESTENABLE, TRUE },
			{ D3DRS_LASTPIXEL, TRUE },
			{ D3DRS_ZFUNC, D3DCMP_LESSEQUAL },
			{ D3DRS_ALPHAREF, 0 },
			{ D3DRS_ALPHAFUNC, D3DCMP_GREATER },
			{ D3DRS_DITHERENABLE, TRUE },
			{ D3DRS_ALPHABLENDENABLE, FALSE },
			{ D3DRS_FOGENABLE, FALSE },
			{ D3DRS_SPECULARENABLE, FALSE },
			{ D3DRS_FOGCOLOR, 0 },
			{ D3DRS_SEPARATEALPHABLENDENABLE, FALSE },
			{ D3DRS_RANGEFOGENABLE, FALSE },
			{ D3DRS_STENCILENABLE, FALSE },
			{ D3DRS_TEXTUREFACTOR, 0 },
			{ D3DRS_CULLMODE, D3DCULL_NONE },
			{ D3DRS_CLIPPING, TRUE },
			{ D3DRS_LIGHTING, FALSE },
			{ D3DRS_AMBIENT, 0 },
			{ D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR },
			{ D3DRS_SCISSORTESTENABLE, TRUE },
			{ D3DRS_MULTISAMPLEANTIALIAS, FALSE },
			{ D3DRS_VERTEXBLEND, D3DVBF_DISABLE },
			{ D3DRS_NORMALIZENORMALS, FALSE },
			{ D3DRS_COLORVERTEX, TRUE },
			{ D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2 },
			{ D3DRS_COLORWRITEENABLE, 0xf }
		};

		for (n = 0; n < sizeof(def_render_state) / sizeof(def_render_state[0]); n++) {
			m_pd3dDevice->GetRenderState(def_render_state[n].State, &value);
			if (value != def_render_state[n].Value)
				m_pd3dDevice->SetRenderState(def_render_state[n].State, def_render_state[n].Value);
		}
	}

	{
		static const struct {
			unsigned long Stage;
			_D3DTEXTURESTAGESTATETYPE Type;
			unsigned long Value;
		} def_texture_state[] = {
			{ 0, D3DTSS_TEXCOORDINDEX, 0 },
			{ 1, D3DTSS_TEXCOORDINDEX, 1 },
			{ 0, D3DTSS_COLORARG1, D3DTA_TEXTURE },
			{ 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE },
			{ 1, D3DTSS_COLORARG1, D3DTA_TEXTURE },
			{ 1, D3DTSS_COLORARG2, D3DTA_CURRENT },
			{ 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE },
			{ 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE },
			{ 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE },
			{ 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE },
			{ 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE },
			{ 1, D3DTSS_COLOROP, D3DTOP_SELECTARG1 }
		};

		for (n = 0; n < sizeof(def_texture_state) / sizeof(def_texture_state[0]); n++) {
			m_pd3dDevice->GetTextureStageState(def_texture_state[n].Stage, def_texture_state[n].Type, &value);
			if (value != def_texture_state[n].Value)
				_SetTextureStageState(def_texture_state[n].Stage, def_texture_state[n].Type, def_texture_state[n].Value);
		}
	}

	{
		static const struct {
			unsigned long Stage;
			_D3DSAMPLERSTATETYPE Type;
			unsigned long Value;
		} def_sampler_state[] = {
			{ 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP },
			{ 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP },
			{ 1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP },
			{ 1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP },
			{ 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR },
			{ 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR },
			{ 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT },
			{ 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR },
			{ 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR },
			{ 1, D3DSAMP_MIPFILTER, D3DTEXF_POINT }
		};

		for (n = 0; n < sizeof(def_sampler_state) / sizeof(def_sampler_state[0]); n++) {
			m_pd3dDevice->GetSamplerState(def_sampler_state[n].Stage, def_sampler_state[n].Type, &value);
			if (value != def_sampler_state[n].Value)
				_SetSamplerState(def_sampler_state[n].Stage, def_sampler_state[n].Type, def_sampler_state[n].Value);
		}
	}

	for (n = 0; n < 2; n++) {
		m_pTexture[n] = NULL;

		m_pd3dDevice->GetTexture(n, &surf);
		if (surf) {
			_SetTexture(n, NULL);
			surf->Release();
		}
	}

	m_pd3dDevice->GetRenderState(D3DRS_SRCBLEND, &m_lastBlendState[0]);
	m_pd3dDevice->GetRenderState(D3DRS_DESTBLEND, &m_lastBlendState[1]);
	m_pd3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &m_blendEnable);

	for (n = 0; n < 2; n++) {
		for (int i = 0; i < sizeof(g_TexStageStateList) / sizeof(g_TexStageStateList[0]); i++)
			m_pd3dDevice->GetTextureStageState(n, g_TexStageStateList[i], &m_texStageState[n][i]);
	}

	m_lastBlendMode = 0xffffffff;
	m_lastVtxType = 0xffffffff;

	m_xLastDrawType = 0x404;
	m_xLastRenderState = 0x404;
	m_xLastVertexDecl = 0xffffffff;

	m_lastTexState = 0;
	m_lastRenderState = 0;
	m_fog = false;
	m_xhLastVb = 0;
	m_xhLastIb = 0;
	m_xdwLastUsage = 0;
	m_xdwLastFVF = 0;
	m_xbLastHwTnL = false;
	m_xbUseTFactor = false;
	m_xbLastVertexBlend = false;
	m_lastEffect = 0;
	memset(m_xaLights, 0, sizeof(m_xaLights));
	memset(m_xaWLights, 0, sizeof(m_xaWLights));
	memset(&m_xMaterial, 0, sizeof(m_xMaterial));
	m_xMaterial.Diffuse.r = m_xMaterial.Diffuse.g = m_xMaterial.Diffuse.b = m_xMaterial.Diffuse.a = 1.0f;
	m_xMaterial.Ambient.r = m_xMaterial.Ambient.g = m_xMaterial.Ambient.b = m_xMaterial.Ambient.a = 1.0f;
	m_xdwDiffuse = 0xffffffff;
	m_xdwTFactor = 0xffffffff;
	m_LightEnable = false;

	for (int i = 0; i < sizeof(m_xLastWorldMatrix) / sizeof(m_xLastWorldMatrix[0]); i++) D3DXMatrixIdentity(&m_xLastWorldMatrix[i]);
	D3DXMatrixIdentity(&m_xLastViewMatrix);
	D3DXMatrixIdentity(&m_xLastProjMatrix);
	D3DXMatrixIdentity(&m_xPrevViewMatrix);

	m_LastTFactor = 0xffffffff;
	ResetFogState();

	m_pd3dDevice->SetMaterial(&m_xMaterial);
	m_pd3dDevice->LightEnable(0, TRUE);
	_SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	_SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);
}

// wd3d8.cpp:3793
WDirect3D8::pix_info* WDirect3D8::FindPixInfoByFormat(_D3DFORMAT fmt) {
	for (int i = 0; i < sizeof(ms_fmtTypeList) / sizeof(ms_fmtTypeList[0]); i++) {
		if (ms_fmtTypeList[i].pixFmt == fmt) return &ms_fmtTypeList[i];
	}
	return NULL;
}

// wd3d8.cpp:3806
int WDirect3D8::SetTextureFormat(_D3DFORMAT suffmt) {
	int result = 0;
	int i;

	for (int fmt = 0; fmt < 6; fmt++) {
		for (i = 0; i < 4; i++) {
			_D3DFORMAT texFmt = ms_fmtRecomList[fmt][i];
			if (m_d3d8->CheckDeviceFormat(m_devId, D3DDEVTYPE_HAL, suffmt, 0, D3DRTYPE_TEXTURE, texFmt) == D3D_OK) {
				pix_info* info = FindPixInfoByFormat(texFmt);
				if (info)
					m_fmt[fmt] = info;
				break;
			}
		}

		if (i == 4)
			result |= 1;
	}

	return result;
}

// wd3d8.cpp:3836
HRESULT WDirect3D8::Present(void) {
	if (m_pScreenCape)
		m_pScreenCape->Render();

	m_renderCount++;
	HRESULT hr = m_pd3dDevice->Present(NULL, NULL, m_hwnd, NULL);

	if (SUCCEEDED(hr)) {
		if (m_pEventQuery) {
			m_pEventQuery->Issue(D3DISSUE_END);

			while (m_pEventQuery->GetData(NULL, 0, D3DGETDATA_FLUSH) == S_FALSE)
				;
		} else {
			IDirect3DSurface9* pRenderTarget;
			if (SUCCEEDED(m_pd3dDevice->GetRenderTarget(0, &pRenderTarget))) {
				tagRECT rc;
				SetRect(&rc, 0, 0, 1, 1);
				_D3DLOCKED_RECT lr;
				pRenderTarget->LockRect(&lr, &rc, D3DLOCK_READONLY);
				pRenderTarget->UnlockRect();
				pRenderTarget->Release();
			}
		}
	}

	return hr;
}

// wd3d8.cpp:3887
WDirect3D8* WDirect3D8::CreateClone(char* devName, int id) {
	return new WDirect3D8(devName, id);
}

// wd3d8.cpp:3892
WVideoDev* WDirect3D8::MakeClone(char* modeName, HWND__* hwnd, int iTnL) {
	if (m_devId >= 0) {
		WDirect3D8* clone = CreateClone(m_devName, m_devId);
		int ret = clone->Init(modeName, hwnd, iTnL);
		if (ret == 0) {
			return clone;
		}
		clone->Release();
		delete clone;
	}

	return NULL;
}

static int count = 0;

// wd3d8.cpp:3928
void WDirect3D8::Clear(unsigned long color, int flags, float z) {
	if (m_devState == 1) return;

	if (m_useCopiedScreen) {
		if (g_captureOption.useMemCopy || !g_captureOption.dxCopyRectsOK) {
			IDirect3DSurface9* pSurf = NULL;
			if (g_captureOption.useTexture) {
				m_texList[m_hCopiedScreenTexture].pTex->GetSurfaceLevel(0, &pSurf);
			} else {
				pSurf = m_pCopiedScreenSurface;
			}

			IDirect3DSurface9* pRenderTarget;
			if (m_pd3dDevice->GetRenderTarget(0, &pRenderTarget) == D3D_OK) {
				tagRECT rc;
				if (!g_captureOption.updateWholeScreen && count) {
					rc.left = 118;
					rc.top = m_capturedDdsd.Height - 128;
					rc.right = 523;
					rc.bottom = m_capturedDdsd.Height - 68;
				} else {
					rc.left = 0;
					rc.top = 0;
					rc.right = m_capturedDdsd.Width;
					rc.bottom = m_capturedDdsd.Height;
				}
				D3DXLoadSurfaceFromSurface(pRenderTarget, NULL, &rc, pSurf, NULL, &rc, D3DX_FILTER_NONE, 0);
				pRenderTarget->Release();
			}
			if (g_captureOption.useTexture) pSurf->Release();
		} else if (g_captureOption.useTexture) {
			struct VERTEX {
				// wd3d8.cpp:4031
				VERTEX() {
				}
				// wd3d8.cpp:4037
				VERTEX(float _x, float _y, float _z, float _w, float _u, float _v) : x(_x), y(_y), z(_z), w(_w), u(_u), v(_v) {
				}

				float x, y, z, w;
				float u, v;
			};
			VERTEX rect[4];
			float l, t, r, b;
			float uvl, uvt, uvr, uvb;

			if (!g_captureOption.updateWholeScreen && count) {
				l = 118.0f - 0.5f;
				r = 523.0f - 0.5f;
				t = (m_capturedDdsd.Height - 128) - 0.5f;
				b = (m_capturedDdsd.Height - 68) - 0.5f;
				uvl = 118.0f / m_capturedDdsd.Width;
				uvr = 523.0f / m_capturedDdsd.Width;
				uvt = (m_capturedDdsd.Height - 128) / (float)m_capturedDdsd.Height;
				uvb = (m_capturedDdsd.Height - 68) / (float)m_capturedDdsd.Height;
			} else {
				l = -0.5f;
				r = m_capturedDdsd.Width - 0.5f;
				t = -0.5f;
				b = m_capturedDdsd.Height - 0.5f;
				uvr = 1.0f;
				uvl = 0.0f;
				uvt = 0.0f;
				uvb = 1.0f;
			}

			rect[0] = VERTEX(l, b, 0.001f, 1.0f, uvl, uvb);
			rect[1] = VERTEX(l, t, 0.001f, 1.0f, uvl, uvt);
			rect[2] = VERTEX(r, b, 0.001f, 1.0f, uvr, uvb);
			rect[3] = VERTEX(r, t, 0.001f, 1.0f, uvr, uvt);

			DrawPrimitive((m_hCopiedScreenTexture & 0x7ff) | 0x60300000, 4, D3DFVF_XYZRHW | D3DFVF_TEX1, rect, D3DPT_TRIANGLESTRIP, 0x404);
		} else {
			IDirect3DSurface9* pRenderTarget;
			if (m_pd3dDevice->GetRenderTarget(0, &pRenderTarget) == D3D_OK) {
				if (!g_captureOption.updateWholeScreen && count) {
					tagRECT rect;
					rect.left = 118;
					rect.top = m_capturedDdsd.Height - 128;
					rect.right = 523;
					rect.bottom = m_capturedDdsd.Height - 68;
					fnWD3DDevice_CopyRect(m_pd3dDevice, m_pCopiedScreenSurface, &rect, pRenderTarget);
				} else {
					fnWD3DDevice_CopyRect(m_pd3dDevice, m_pCopiedScreenSurface, NULL, pRenderTarget);
				}
				pRenderTarget->Release();
			}
		}

		if (!g_captureOption.updateWholeScreen) {
			count++;
		}
	} else {
		count = 0;
		unsigned long clearFlags = 0;
		if (flags & 1) {
			clearFlags = D3DCLEAR_TARGET;
		}
		if (flags & 2) {
			clearFlags |= D3DCLEAR_ZBUFFER;
		}
		m_pd3dDevice->Clear(0, NULL, clearFlags, color, z, 0);
	}
}

// wd3d8.cpp:4132
bool WDirect3D8::BeginScene(void) {
	HRESULT hr = m_pd3dDevice->TestCooperativeLevel();

	if (FAILED(hr)) {
		m_devState = W_VDEVSTATE_LOST;

		if (hr == D3DERR_DEVICELOST)
			return false;
		else if (hr != D3DERR_DEVICENOTRESET || !xReset(false))
			return false;
	}

	if (m_d3dpp.Windowed && !m_fillScrMode) {
		if (!m_clientRcCheckedAfterReset) {
			tagRECT rcClient;

			m_clientRcCheckedAfterReset = true;
			GetClientRect(m_hwnd, &rcClient);

			if (rcClient.right - rcClient.left != m_d3dpp.BackBufferWidth || rcClient.bottom - rcClient.top != m_d3dpp.BackBufferHeight) {
				SetRect(&rcClient, 0, 0, m_d3dpp.BackBufferWidth, m_d3dpp.BackBufferHeight);
				AdjustWindowRectEx(&rcClient, GetWindowLong(m_hwnd, GWL_STYLE), FALSE, 0);

				if (rcClient.right - rcClient.left <= GetSystemMetrics(SM_CXFULLSCREEN) && rcClient.bottom - rcClient.top <= GetSystemMetrics(SM_CYFULLSCREEN)) {
					m_devState = W_VDEVSTATE_LOST;
					if (!xReset(false))
						return false;
				}
			}
		}
	}

	m_devState = W_VDEVSTATE_NORMAL;
	m_xhLastIb = 0;
	m_xhLastVb = 0;
	m_xnDIPs = 0;
	m_xnDPs = 0;
	m_xnDIPUPs = 0;
	m_xnDPUPs = 0;
	m_xnTotalTris = 0;
	m_pd3dDevice->BeginScene();

	m_MergeBuffer.PrimitiveCount = 0;

	return true;
}

// wd3d8.cpp:4205
void WDirect3D8::EndScene(void) {
	Flush(0);

	FlushRenderPrimitive();

	if (m_devState != W_VDEVSTATE_LOST) {
		m_Effect = 0; ApplyShader();

		m_pd3dDevice->EndScene();
	}
}

// wd3d8.cpp:4229
bool WDirect3D8::xReset(bool switchWinMode) {
	m_renderCount = 0;

	xReset_ReleaseResource();

	std::map<unsigned long, sEffect>::iterator effectItr;
	for (effectItr = m_EffectTable.begin(); effectItr != m_EffectTable.end(); effectItr++) {
		if (effectItr->second.pEffect)
			effectItr->second.pEffect->OnLostDevice();
	}

	ReleaseEventQuery();

	if (FAILED(m_pd3dDevice->Reset(&m_d3dpp))) {
		// Windowed: the desktop format may have changed under us.
		if (m_d3dpp.Windowed) {
			_D3DDISPLAYMODE d3ddm;
			if (m_d3d8->GetAdapterDisplayMode(m_devId, &d3ddm) != D3D_OK) {
				g_error = "\xc0\xfb\xc0\xfd\xc7\xd1 \xb5\xf0\xbd\xba\xc7\xc3\xb7\xb9\xc0\xcc \xb8\xf0\xb5\xe5\xb8\xa6 \xbe\xf2\xbe\xee \xbf\xc3 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";     // no suitable display mode
				return false;
			}

			if (d3ddm.Format == m_d3dpp.BackBufferFormat)
				return false;

			m_d3dpp.BackBufferFormat = d3ddm.Format; m_d3dpp.AutoDepthStencilFormat = FindDepthBufferFormat(d3ddm.Format);

			if (FAILED(m_pd3dDevice->Reset(&m_d3dpp)))
				return false;
		} else {
			return false;
		}
	}

	m_devState = W_VDEVSTATE_NORMAL;
	m_clientRcCheckedAfterReset = false;
	CreateEventQuery();

	SetTextureFormat(m_d3dpp.BackBufferFormat);
	SetRenderTargetFormat();
	SetDefaultState();

	if (m_d3dpp.Windowed) {
		if (m_lWndStyle != -1)
			SetWindowLongA(m_hwnd, GWL_STYLE, m_lWndStyle | WS_VISIBLE);

		int window_width = m_d3dpp.BackBufferWidth;
		int window_height = m_d3dpp.BackBufferHeight;
		if (m_fillScrMode) {
			window_width = GetSystemMetrics(SM_CXSCREEN);
			window_height = GetSystemMetrics(SM_CYSCREEN);
		}

		tagRECT rc;
		SetRect(&rc, 0, 0, window_width, window_height);

		tagRECT old_rc;
		GetClientRect(m_hwnd, &old_rc);

		if (switchWinMode || old_rc.right - old_rc.left != window_width || old_rc.bottom - old_rc.top != window_height) {
			AdjustWindowRectEx(&rc, GetWindowLongA(m_hwnd, GWL_STYLE), FALSE, 0);
			int x = (GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2;
			int y = (GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2;

			OffsetRect(&rc, x, y);

			SetWindowPos(m_hwnd, HWND_NOTOPMOST, x, y, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW);

			UpdateWindow(m_hwnd);
			g_formatChanged = false;
		}

		// Desktop composition is turned off for a stretched 4:3 back buffer
		// on a wide screen, and on again otherwise.
		if (m_pkDWMApiDll && m_fillScrMode) {
			float aspect = (float)m_d3dpp.BackBufferWidth / (float)m_d3dpp.BackBufferHeight;

			if (aspect < 1.4f && (float)window_width / window_height > 1.4f) {
				if (!m_pkDWMApiDll->IsCompositionEnabled())
					m_pkDWMApiDll->EnableComposition(TRUE);
			} else {
				if (m_pkDWMApiDll->IsCompositionEnabled())
					m_pkDWMApiDll->EnableComposition(FALSE);
			}
		}
	}

	xReset_CreateResource();

	if (m_pCopiedScreenSplash)
		m_pCopiedScreenSplash->Reset();

	BackupMainRenderTarget();

	m_curRt = m_mainRt;
	for (effectItr = m_EffectTable.begin(); effectItr != m_EffectTable.end(); effectItr++) {
		if (effectItr->second.pEffect)
			effectItr->second.pEffect->OnResetDevice();
	}

	return true;
}

// wd3d8.cpp:4431
void WDirect3D8::xReset_ReleaseResource(void) {
	// Only the D3DPOOL_DEFAULT (dynamic) buffers are lost with the device.
	for (int i = 0; i < 1024; i++) {
		if (m_xaVbList[i].xdwUsage & D3DUSAGE_DYNAMIC) {
			m_xaVbList[i].xbNeedToBeFilled = true;
			if (m_xaVbList[i].xpVb) { m_xaVbList[i].xpVb->Release(); m_xaVbList[i].xpVb = NULL; }
		}
	}

	for (int i = 0; i < 256; i++) {
		if (m_xaIbList[i].xdwUsage & D3DUSAGE_DYNAMIC) {
			// The original flags the vertex buffer of the same index here.
			m_xaVbList[i].xbNeedToBeFilled = true;
			if (m_xaIbList[i].xpIb) { m_xaIbList[i].xpIb->Release(); m_xaIbList[i].xpIb = NULL; }
		}
	}

	for (int i = 0; i < 2048; i++) {
		if (m_texList[i].pSurf) {
			m_texList[i].isFilled = false;
			m_texList[i].pSurf->Release();
			m_texList[i].pSurf = NULL; if (m_texList[i].pTex) { m_texList[i].pTex->Release(); m_texList[i].pTex = NULL; }
		}
	}

	if (m_commonDepthSurf) { m_commonDepthSurf->Release(); m_commonDepthSurf = NULL; }

	for (int i = 0; i < 32; i++)
		if (m_depthSurfList[i].pDepth) { m_depthSurfList[i].pDepth->Release(); m_depthSurfList[i].pDepth = NULL; }

	memset(m_depthSurfList, 0, sizeof(m_depthSurfList));

	ReleaseCopiedScreenResource();

	if (m_pLockableVB) { m_pLockableVB->Release(); m_pLockableVB = NULL; }
	if (m_pLockableIB) { m_pLockableIB->Release(); m_pLockableIB = NULL; }

	ReleaseAllRendertargetBackupResource();
}

// wd3d8.cpp:4507
void WDirect3D8::xReset_CreateResource(void) {
	for (int i = 0; i < 1024; i++) {
		if (m_xaVbList[i].xdwUsage & D3DUSAGE_DYNAMIC) {
			m_pd3dDevice->CreateVertexBuffer(m_xaVbList[i].xbStride * m_xaVbList[i].xnVtxs, m_xaVbList[i].xdwUsage, m_xaVbList[i].xdwFVF, D3DPOOL_DEFAULT, &m_xaVbList[i].xpVb, NULL);
		}
	}

	for (int i = 0; i < 256; i++) {
		if (m_xaIbList[i].xdwUsage & D3DUSAGE_DYNAMIC) {
			m_pd3dDevice->CreateIndexBuffer(m_xaIbList[i].xnIdxs * 2, m_xaIbList[i].xdwUsage, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_xaIbList[i].xpIb, NULL);
		}
	}

	// Render-target textures live in D3DPOOL_DEFAULT: create them again.
	for (unsigned int i = 0; i < 2048; i++) {
		if (m_texList[i].renderTargetType) {
			unsigned long width;
			int height;

			WRenderToTextureSizeInfo sizeInfo = m_texList[i].renderTargetSizeInfo;

			if (sizeInfo.m_isAbsolute) {
				width = m_texList[i].width;
				height = m_texList[i].height;
			} else {
				width = (unsigned long)floor(GetWidth() * sizeInfo.m_width + 0.5f);
				height = (int)floor(GetHeight() * sizeInfo.m_height + 0.5f);
			}

			CreateTextureSurface(width, height, m_texList[i].mipmaplevel, m_texList[i].renderTargetType, i, 0, 32);
			SetRenderTargetSizeInfo(i, sizeInfo);
		}
	}
}

// wd3d8.cpp:4591
void WDirect3D8::CreateEventQuery(void) {
	if (SUCCEEDED(m_pd3dDevice->CreateQuery(D3DQUERYTYPE_EVENT, NULL))) {
		m_pd3dDevice->CreateQuery(D3DQUERYTYPE_EVENT, &m_pEventQuery);
	}
}

// wd3d8.cpp:4599
void WDirect3D8::ReleaseEventQuery(void) {
	if (m_pEventQuery) {
		m_pEventQuery->Release();
		m_pEventQuery = NULL;
	}
}

// wd3d8.cpp:4606
void WDirect3D8::xInstantiateAndFillTexture(int hTex) {
	if (hTex) {
		if (m_texList[hTex].pTex == NULL)
			xInstantiateTexture(hTex);

		if (m_texList[hTex].needToBeFilled)
			xFillTexture(hTex);
	}
}

// wd3d8.cpp:4618
void WDirect3D8::xInstantiateTexture(int hTex) {
	d3d8_texture* tex = &m_texList[hTex];

	// A plain texture: created empty, filled later from its bitmap.
	if (tex->pixFmtInfo) {
		m_pd3dDevice->CreateTexture(tex->width, tex->height, tex->mipmaplevel, 0, tex->pixFmtInfo->pixFmt, D3DPOOL_MANAGED, &tex->pTex, NULL);
		return;
	}

	// A compressed texture: created from its .dds data, skipping the top
	// m_iDDSRes levels.
	_D3DSURFACE_DESC topLvlD3dsd;
	unsigned int levelCount;
	if (m_iDDSRes <= 0) {
		D3DXCreateTextureFromFileInMemoryEx(m_pd3dDevice, tex->dxtcData, tex->dxtcDataSize, D3DX_DEFAULT, D3DX_DEFAULT, m_bUseMipmap ? m_iMaxMipLvl : 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, m_dwMipCreateFilter, 0, NULL, NULL, &tex->pTex);

		if (tex->pTex == NULL) return;

		tex->pTex->GetLevelDesc(0, &topLvlD3dsd);
		levelCount = tex->pTex->GetLevelCount();
	} else {
		IDirect3DTexture9* pSrcTex = NULL;
		unsigned int srcLevels;
		if (m_bUseMipmap)
			srcLevels = m_iMaxMipLvl + m_iDDSRes;
		else
			srcLevels = m_iDDSRes + 1;

		D3DXCreateTextureFromFileInMemoryEx(m_pd3dDevice, tex->dxtcData, tex->dxtcDataSize, D3DX_DEFAULT, D3DX_DEFAULT, srcLevels, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, m_dwMipCreateFilter, 0, NULL, NULL, &pSrcTex);

		if (pSrcTex == NULL) return;

		srcLevels = pSrcTex->GetLevelCount();

		unsigned int iTopSrcLvlToLoad = m_iDDSRes;
		if (iTopSrcLvlToLoad + 1 > srcLevels)
			iTopSrcLvlToLoad = srcLevels - 1;

		if (m_bUseMipmap)
			levelCount = srcLevels - iTopSrcLvlToLoad;
		else
			levelCount = 1;

		pSrcTex->GetLevelDesc(iTopSrcLvlToLoad, &topLvlD3dsd);

		D3DXCreateTexture(m_pd3dDevice, topLvlD3dsd.Width, topLvlD3dsd.Height, levelCount, 0, topLvlD3dsd.Format, D3DPOOL_MANAGED, &tex->pTex);

		if (tex->pTex == NULL) {
			pSrcTex->Release();
			return;
		}

		for (unsigned int i = 0; i < levelCount; i++) {
			IDirect3DSurface9* srcSurf;
			if (SUCCEEDED(pSrcTex->GetSurfaceLevel(iTopSrcLvlToLoad + i, &srcSurf))) {
				IDirect3DSurface9* dstSurf;
				if (SUCCEEDED(tex->pTex->GetSurfaceLevel(i, &dstSurf))) {
					D3DXLoadSurfaceFromSurface(dstSurf, NULL, NULL, srcSurf, NULL, NULL, D3DX_FILTER_NONE, 0);
					dstSurf->Release();
				}
				srcSurf->Release();
			}
		}

		pSrcTex->Release();
	}

	tex->width = topLvlD3dsd.Width;
	tex->height = topLvlD3dsd.Height;
	tex->mipmaplevel = levelCount;

	tex->dxtcDataSize = 0;
	if (tex->dxtcData) { delete[] tex->dxtcData; tex->dxtcData = NULL; }
}

// wd3d8.cpp:4773
void WDirect3D8::xFillTexture(int hTex) {
	d3d8_texture* tex = &m_texList[hTex];

	IDirect3DTexture9* pTex = tex->pTex;
	if (pTex) {
		if (tex->bitmap) {
			for (int i = 0; i < tex->mipmaplevel; i++) {
				_D3DLOCKED_RECT rect;
				if (pTex->LockRect(i, &rect, NULL, 0) == D3D_OK) {
					UpdateTextureSurface(rect.pBits, tex->width >> i, tex->height >> i, rect.Pitch, tex->pixFmtInfo, tex->bitmap->bi, tex->bitmap->vram, tex->updateType);
					pTex->UnlockRect(i);
				}
			}
		}

		tex->needToBeFilled = false;
		tex->isFilled = true;
	}
}

// wd3d8.cpp:4831
void WDirect3D8::xInstantiateVertexBuffer(int hVb) {
	D3DPOOL pool;
	if (m_xaVbList[hVb].xdwUsage & D3DUSAGE_DYNAMIC)
		pool = D3DPOOL_DEFAULT;
	else
		pool = D3DPOOL_MANAGED;
	m_pd3dDevice->CreateVertexBuffer(m_xaVbList[hVb].xbStride * m_xaVbList[hVb].xnVtxs, m_xaVbList[hVb].xdwUsage, m_xaVbList[hVb].xdwFVF,
		pool, &m_xaVbList[hVb].xpVb, NULL);
	xFillVertexBuffer(hVb);
	m_xiVbSize += m_xaVbList[hVb].xbStride * m_xaVbList[hVb].xnVtxs;
}

// wd3d8.cpp:4865
void WDirect3D8::xInstantiateIndexBuffer(int hIb) {
	D3DPOOL pool;
	if (m_xaIbList[hIb].xdwUsage & D3DUSAGE_DYNAMIC)
		pool = D3DPOOL_DEFAULT;
	else
		pool = D3DPOOL_MANAGED;
	m_pd3dDevice->CreateIndexBuffer(m_xaIbList[hIb].xnIdxs * sizeof(unsigned short), m_xaIbList[hIb].xdwUsage, D3DFMT_INDEX16,
		pool, &m_xaIbList[hIb].xpIb, NULL);
	xFillIndexBuffer(hIb);
}

// wd3d8.cpp:4893
void WDirect3D8::xFillVertexBuffer(int hVb) {
	if (m_xaVbList[hVb].xpVb) {
		unsigned char* pData;
		unsigned long dwFlags = 0;

		if (m_xaVbList[hVb].xdwUsage & D3DUSAGE_DYNAMIC)
			dwFlags = D3DLOCK_DISCARD;

		m_xaVbList[hVb].xpVb->Lock(0, 0, (void**)&pData, dwFlags);
		memcpy(pData, m_xaVbList[hVb].xpVertexData, m_xaVbList[hVb].xbStride * m_xaVbList[hVb].xnVtxs);
		m_xaVbList[hVb].xpVb->Unlock();

		m_xaVbList[hVb].xbNeedToBeFilled = false;
	}
}

// wd3d8.cpp:4914
void WDirect3D8::xFillIndexBuffer(int hIb) {
	if (m_xaIbList[hIb].xpIb) {
		unsigned char* pData;
		unsigned long dwFlags = 0;

		if (m_xaIbList[hIb].xdwUsage & D3DUSAGE_DYNAMIC)
			dwFlags = D3DLOCK_DISCARD;

		m_xaIbList[hIb].xpIb->Lock(0, 0, (void**)&pData, dwFlags);
		memcpy(pData, m_xaIbList[hIb].xpIndexData, m_xaIbList[hIb].xnIdxs * sizeof(unsigned short));
		m_xaIbList[hIb].xpIb->Unlock();

		m_xaIbList[hIb].xbNeedToBeFilled = false;
	}
}

// wd3d8.cpp:4936
void WDirect3D8::Paint(void) {
	if (m_devState != W_VDEVSTATE_LOST) {
		Present();
	}
}

// wd3d8.cpp:4944
bool WDirect3D8::IsTextureFilled(int texHandle) const {
	return m_texList[texHandle].isFilled;
}

// wd3d8.cpp:4951
int WDirect3D8::GetTextureWidth(int hTex) const {
	return m_texList[hTex].width;
}

// wd3d8.cpp:4962
int WDirect3D8::GetTextureHeight(int hTex) const {
	return m_texList[hTex].height;
}

// wd3d8.cpp:4973
void WDirect3D8::SetRenderTargetSizeInfo(int hTex, const WRenderToTextureSizeInfo& sizeInfo) {
	m_texList[hTex].renderTargetSizeInfo = sizeInfo;
}

// wd3d8.cpp:4983
int WDirect3D8::LockVB(const void* vptr, int vnum, int vpitch, int offset) {
	unsigned char* pVptr;

	if (m_pLockableVB == NULL) {
		m_pd3dDevice->CreateVertexBuffer(0x100000, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_pLockableVB, NULL);
		m_pd3dDevice->CreateIndexBuffer(0x80000, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pLockableIB, NULL);

		m_iVBOffset = 0x100000;
		m_iIBOffset = 0x80000;
	}

	if (m_pLockableVB == NULL || m_pLockableIB == NULL)
		return -1;

	int paddbytes = (vpitch - m_iVBOffset % vpitch) % vpitch;

	unsigned long dwLockFlags = D3DLOCK_NOOVERWRITE;
	int len = (offset + vnum) * vpitch;

	if (m_iVBOffset + len + paddbytes > 0x100000 || (m_iVBOffset + len + paddbytes) / vpitch >= (int)m_d3dcaps.MaxVertexIndex || (m_iVBOffset + len + paddbytes) / vpitch >= 0x10000) {
		m_iVBOffset = 0;
		dwLockFlags = D3DLOCK_DISCARD;
		paddbytes = 0;
	}

	if (m_pLockableVB->Lock(m_iVBOffset + paddbytes + offset * vpitch, vnum * vpitch, (void**)&pVptr, dwLockFlags) != D3D_OK) {
		return -1;
	}

	memcpy(pVptr, vptr, vnum * vpitch);
	m_pLockableVB->Unlock();

	int startVtx = (m_iVBOffset + paddbytes) / vpitch;
	m_iVBOffset += len + paddbytes;

	return startVtx;
}

// wd3d8.cpp:5033
int WDirect3D8::LockIB(const unsigned short* iptr, int inum) {
	unsigned char* pIptr;
	int len = inum * sizeof(unsigned short);
	unsigned long flags = D3DLOCK_NOOVERWRITE;

	if (m_iIBOffset + len > 0x80000) {
		flags = D3DLOCK_DISCARD;
		m_iIBOffset = 0;
	}

	if (fnWD3DIndexBuffer_Lock(m_pLockableIB, m_iIBOffset, len, &pIptr, flags) != D3D_OK) {
		return -1;
	}
	memcpy(pIptr, iptr, len);
	m_pLockableIB->Unlock();

	int startIdx = m_iIBOffset / sizeof(unsigned short);
	m_iIBOffset += len;

	return startIdx;
}

// wd3d8.cpp:5061
bool WDirect3D8::DrawIndexedPrimitiveLockable(_D3DPRIMITIVETYPE PrimitiveType, const void* vptr, int vnum, int vpitch, unsigned short* iptr, int primnum) {
	int off[2];

	off[0] = LockVB(vptr, vnum, vpitch, 0);
	if (off[0] == -1)
		return false;

	if (PrimitiveType == D3DPT_LINELIST)
		off[1] = LockIB(iptr, primnum * 2);
	else
		off[1] = LockIB(iptr, primnum * 3);
	if (off[1] == -1)
		return false;

	int hVB = m_xhLastVb;
	int hIb = m_xhLastIb;
	m_xhLastVb = 0;
	m_xhLastIb = 0;

	m_pd3dDevice->SetStreamSource(0, m_pLockableVB, 0, vpitch);
	m_pd3dDevice->SetIndices(m_pLockableIB);

	if (ApplyShader()) {
		m_CustomRenderState.Begin(*this);

		m_pd3dDevice->DrawIndexedPrimitive(PrimitiveType, off[0], 0, vnum, off[1], primnum);

		m_xnDIPs++;
		m_xnTotalTris += primnum;

		m_CustomRenderState.End(*this);
	}

	xSetTnLBuffer(hVB, hIb);

	return true;
}

// wd3d8.cpp:5109
bool WDirect3D8::DrawPrimitiveLockable(_D3DPRIMITIVETYPE PrimitiveType, unsigned int PrimitiveCount, const void* pVertexStreamZeroData, unsigned int VertexStreamZeroStride) {
	unsigned int vnum = PrimitiveCount;
	switch (PrimitiveType) {
	case D3DPT_TRIANGLESTRIP:
	case D3DPT_TRIANGLEFAN:
		vnum = PrimitiveCount + 2;
		break;
	case D3DPT_TRIANGLELIST: vnum = PrimitiveCount * 3; break;
	case D3DPT_LINESTRIP: vnum = PrimitiveCount + 1; break;
	case D3DPT_LINELIST: vnum = PrimitiveCount * 2; break;
	}

	int off[1];
	off[0] = LockVB(pVertexStreamZeroData, vnum, VertexStreamZeroStride, 0);
	if (off[0] == -1)
		return false;

	int hVB = m_xhLastVb;
	int hIb = m_xhLastIb;
	m_xhLastVb = 0;

	m_pd3dDevice->SetStreamSource(0, m_pLockableVB, 0, VertexStreamZeroStride);

	if (ApplyShader()) {
		m_CustomRenderState.Begin(*this);

		m_pd3dDevice->DrawPrimitive(PrimitiveType, off[0], PrimitiveCount);

		m_xnDPs++;
		m_xnTotalTris += PrimitiveCount;

		m_CustomRenderState.End(*this);
	}

	xSetTnLBuffer(hVB, hIb);

	return true;
}

// wd3d8.cpp:5156
void WDirect3D8::FlushRenderPrimitive(void) {
	int restoretm;
	D3DXMATRIX bak;

	if (m_devState == W_VDEVSTATE_LOST) {
		m_MergeBuffer.PrimitiveCount = 0;
		return;
	}

	if (m_MergeBuffer.PrimitiveCount == 0) return;

	if (GetCurrentThreadId() != m_mainThreadId) return;

#ifdef PG_MATERIAL_HINTS
	int pgHint = PgMaterialHintGet();
	PgMaterialHint(s_pgMergeHint);
#endif

	xInstantiateAndFillTexture(m_lastTexState & 0x7ff);
	xInstantiateAndFillTexture((m_lastTexState >> 11) & 0x7f);

	if (m_lastRenderState & 0x1000) {
		m_MergeBuffer.CheckAndIncreaseVertexStreamBuffer(m_MergeBuffer.VertexStreamZeroStride * m_MergeBuffer.NumVertices);

		FillVertex(m_MergeBuffer.VertexStreamBuffer, m_xaVbList[m_xhLastVb].xpVertexData + m_MergeBuffer.MinIndex * m_MergeBuffer.VertexStreamZeroStride, m_xLastVertexDecl, m_MergeBuffer.NumVertices, m_MergeBuffer.VertexStreamZeroStride, m_xLastWorldMatrix);

		int stride = m_MergeBuffer.VertexStreamZeroStride;
		if ((m_xLastVertexDecl & D3DFVF_POSITION_MASK) == D3DFVF_XYZB4)
			stride -= 16;
		else if ((m_xLastVertexDecl & D3DFVF_POSITION_MASK) == D3DFVF_XYZB3)
			stride -= 12;
		else if ((m_xLastVertexDecl & D3DFVF_POSITION_MASK) == D3DFVF_XYZB2)
			stride -= 8;
		else if ((m_xLastVertexDecl & D3DFVF_POSITION_MASK) == D3DFVF_XYZB1)
			stride -= 4;

		int off[1];
		off[0] = LockVB(m_MergeBuffer.VertexStreamBuffer, m_MergeBuffer.NumVertices, stride, m_MergeBuffer.MinIndex);

		if (off[0] == -1) goto end;

		int hVB = m_xhLastVb;
		int hIb = m_xhLastIb;
		fnWD3DDevice_SetStreamSource(m_pd3dDevice, 0, m_pLockableVB, 0, stride);

		restoretm = m_xLastWorldMatrix[0] != g_mId;

		if (restoretm) {
			bak = m_xLastWorldMatrix[0];
			m_xLastWorldMatrix[0] = g_mId;
			m_pd3dDevice->SetTransform(D3DTS_WORLD, &m_xLastWorldMatrix[0]);
		}

		if (ApplyShader()) {
			m_CustomRenderState.Begin(*this);

			fnWD3DDevice_DrawIndexedPrimitive(m_pd3dDevice, m_MergeBuffer.Type, off[0], 0, m_MergeBuffer.MinIndex + m_MergeBuffer.NumVertices, m_MergeBuffer.StartIndex, m_MergeBuffer.PrimitiveCount);

			m_xnTotalTris += m_MergeBuffer.PrimitiveCount;
			m_xnDIPs++;

			m_CustomRenderState.End(*this);
		}

		m_xhLastVb = 0;
		m_xhLastIb = hIb;
		xSetTnLBuffer(hVB, hIb);
	} else if (!m_MergeBuffer.LockVertex) {
		restoretm = m_xLastWorldMatrix[0] != m_MergeBuffer.m_xLastMatrix;

		if (restoretm) {
			bak = m_xLastWorldMatrix[0];
			m_xLastWorldMatrix[0] = m_MergeBuffer.m_xLastMatrix;
			m_pd3dDevice->SetTransform(D3DTS_WORLD, &m_xLastWorldMatrix[0]);
		}

		int hVb = m_xhLastVb;
		int hIb = m_xhLastIb;
		xSetTnLBuffer(m_MergeBuffer.xhVb, m_MergeBuffer.xhIb);

		if (ApplyShader()) {
			m_CustomRenderState.Begin(*this);

			fnWD3DDevice_DrawIndexedPrimitive(m_pd3dDevice, m_MergeBuffer.Type, 0, m_MergeBuffer.MinIndex, m_MergeBuffer.NumVertices, m_MergeBuffer.StartIndex, m_MergeBuffer.PrimitiveCount);

			m_xnTotalTris += m_MergeBuffer.PrimitiveCount;
			m_xnDIPs++;

			m_CustomRenderState.End(*this);
		}

		xSetTnLBuffer(hVb, hIb);
	} else {
		restoretm = m_xLastWorldMatrix[0] != g_mId;

		if (restoretm) {
			bak = m_xLastWorldMatrix[0];
			m_xLastWorldMatrix[0] = g_mId;
			m_pd3dDevice->SetTransform(D3DTS_WORLD, &m_xLastWorldMatrix[0]);
		}

		if (m_MergeBuffer.LockIndex == 1) {
			DrawIndexedPrimitiveLockable(m_MergeBuffer.Type, m_MergeBuffer.VertexStreamBuffer, m_MergeBuffer.NumVertices, m_MergeBuffer.VertexStreamZeroStride, m_MergeBuffer.IndexBuffer, m_MergeBuffer.PrimitiveCount);
		} else {
			DrawPrimitiveLockable(m_MergeBuffer.Type, m_MergeBuffer.PrimitiveCount, m_MergeBuffer.VertexStreamBuffer, m_MergeBuffer.VertexStreamZeroStride);
		}
	}

	if (restoretm) {
		m_xLastWorldMatrix[0] = bak;
		m_pd3dDevice->SetTransform(D3DTS_WORLD, &m_xLastWorldMatrix[0]);
	}

end:
	m_MergeBuffer.PrimitiveCount = 0;
#ifdef PG_MATERIAL_HINTS
	PgMaterialHint(pgHint);
#endif
}

// wd3d8.cpp:5324
void WDirect3D8::DrawPrimitive(int iType, int iNum, unsigned long dwVertexTypeDesc, void* lpvVertices, _D3DPRIMITIVETYPE dptPrimitiveType, int iType2) {
	if (m_devState == W_VDEVSTATE_LOST) return;

	if (m_xLastWorldMatrix[0] != g_mId) {
		m_xLastWorldMatrix[0] = g_mId;
		if (!IsSupportVS())
			m_pd3dDevice->SetTransform(D3DTS_WORLD, &g_mId);
	}

	SetVtxType(iType, iType2 | 4, dwVertexTypeDesc, 0xffffffff);

	switch (dptPrimitiveType) {
	case D3DPT_TRIANGLESTRIP:
	case D3DPT_TRIANGLEFAN:
		iNum -= 2;
		break;
	case D3DPT_TRIANGLELIST:
		iNum /= 3;
		break;
	case D3DPT_LINESTRIP:
		iNum -= 1;
		break;
	case D3DPT_LINELIST:
		iNum /= 2;
		break;
	}

	_DrawPrimitiveUP(dptPrimitiveType, iNum, lpvVertices, m_vtxSize);
}

// wd3d8.cpp:5352
static int FillIndices(_D3DPRIMITIVETYPE PrimitiveType, unsigned short* pIndexData, int StartIndex, int NumVertices) {
	int i;
	switch (PrimitiveType) {
	case D3DPT_LINELIST:
		for (i = 0; i < NumVertices - 1; i++) {
			pIndexData[i * 2] = StartIndex + i * 2;
			pIndexData[i * 2 + 1] = StartIndex + i * 2 + 1;
		}
		return i;
	case D3DPT_LINESTRIP:
		for (i = 0; i < NumVertices - 1; i++) {
			pIndexData[i * 2] = StartIndex + i;
			pIndexData[i * 2 + 1] = StartIndex + i + 1;
		}
		return i;
	case D3DPT_TRIANGLEFAN:
		for (i = 0; i <= NumVertices - 3; i++) {
			pIndexData[i * 3] = StartIndex;
			pIndexData[i * 3 + 1] = StartIndex + i + 1;
			pIndexData[i * 3 + 2] = StartIndex + i + 2;
		}
		return i;
	case D3DPT_TRIANGLESTRIP:
		for (i = 0; i <= NumVertices - 3; i++) {
			if (!(i & 1)) {
				pIndexData[i * 3 + 1] = StartIndex + i + 1;
				pIndexData[i * 3 + 2] = StartIndex + i + 2;
			} else {
				pIndexData[i * 3] = StartIndex + i;
				pIndexData[i * 3 + 1] = StartIndex + i + 2;
				pIndexData[i * 3 + 2] = StartIndex + i + 1;
			}
		}
		return i;
	case D3DPT_TRIANGLELIST:
		for (i = 0; i < NumVertices / 3; i++) {
			pIndexData[i * 3] = StartIndex + i * 3;
			pIndexData[i * 3 + 1] = StartIndex + i * 3 + 1;
			pIndexData[i * 3 + 2] = StartIndex + i * 3 + 2;
		}
		return i;
	}
	return 0;
}

// wd3d8.cpp:5434
void WDirect3D8::FillVertex(unsigned char* dest, const unsigned char* src, int xLastVertexDecl, int NumVertices, int VertexStreamZeroStride, const D3DXMATRIX* tm) {
	int off = 0;
	int dstOff = 0; // not in the PDB (never given a stack home); name invented
	if ((xLastVertexDecl & D3DFVF_POSITION_MASK) == D3DFVF_XYZ) {
		__m128 m0 = _mm_set_ps(tm->_14, tm->_13, tm->_12, tm->_11);
		__m128 m1 = _mm_set_ps(tm->_24, tm->_23, tm->_22, tm->_21);
		__m128 m2 = _mm_set_ps(tm->_34, tm->_33, tm->_32, tm->_31);
		__m128 m3 = _mm_set_ps(tm->_44, tm->_43, tm->_42, tm->_41);
		if (xLastVertexDecl & D3DFVF_NORMAL) {
			for (int i = 0; i < NumVertices; i++) {
				const float* v = (const float*)(src + off);
				__m128 _d1 = _mm_add_ps(_mm_add_ps(_mm_add_ps(m3, _mm_mul_ps(_mm_set_ps1(v[0]), m0)), _mm_mul_ps(_mm_set_ps1(v[1]), m1)), _mm_mul_ps(_mm_set_ps1(v[2]), m2));
				__m128 _d2 = _mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_set_ps1(v[3]), m0), _mm_mul_ps(_mm_set_ps1(v[4]), m1)), _mm_mul_ps(_mm_set_ps1(v[5]), m2));
				D3DXVECTOR3* p = (D3DXVECTOR3*)(dest + off);
				p[0].x = _d1.m128_f32[0];
				p[0].y = _d1.m128_f32[1];
				p[0].z = _d1.m128_f32[2];
				p[1].x = _d2.m128_f32[0];
				p[1].y = _d2.m128_f32[1];
				p[1].z = _d2.m128_f32[2];
				memcpy(dest + off + 0x18, src + off + 0x18, VertexStreamZeroStride - 0x18);
				off += VertexStreamZeroStride;
			}
		} else {
			for (int i = 0; i < NumVertices; i++) {
				const float* v = (const float*)(src + off);
				__m128 _d = _mm_add_ps(_mm_add_ps(_mm_add_ps(m3, _mm_mul_ps(_mm_set_ps1(v[0]), m0)), _mm_mul_ps(_mm_set_ps1(v[1]), m1)), _mm_mul_ps(_mm_set_ps1(v[2]), m2));
				D3DXVECTOR3* p = (D3DXVECTOR3*)(dest + off);
				p->x = _d.m128_f32[0];
				p->y = _d.m128_f32[1];
				p->z = _d.m128_f32[2];
				memcpy(dest + off + 0xc, src + off + 0xc, VertexStreamZeroStride - 0xc);
				off += VertexStreamZeroStride;
			}
		}
	} else if ((xLastVertexDecl & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW) {
		memcpy(dest, src, NumVertices * VertexStreamZeroStride);
	} else if (xLastVertexDecl & D3DFVF_NORMAL) {
		if ((xLastVertexDecl & D3DFVF_POSITION_MASK) == D3DFVF_XYZB4) {
			for (int i = 0; i < NumVertices; i++) {
				const float* w = (const float*)(src + off + 0xc);
				D3DXVECTOR3 v[4], n[4];
				D3DXVec3TransformCoord(&v[0], (const D3DXVECTOR3*)(src + off), &tm[src[off + 0x18]]);
				D3DXVec3TransformNormal(&n[0], (const D3DXVECTOR3*)(src + off + 0x1c), &tm[src[off + 0x18]]);
				D3DXVec3TransformCoord(&v[1], (const D3DXVECTOR3*)(src + off), &tm[src[off + 0x19]]);
				D3DXVec3TransformNormal(&n[1], (const D3DXVECTOR3*)(src + off + 0x1c), &tm[src[off + 0x19]]);
				D3DXVec3TransformCoord(&v[2], (const D3DXVECTOR3*)(src + off), &tm[src[off + 0x1a]]);
				D3DXVec3TransformNormal(&n[2], (const D3DXVECTOR3*)(src + off + 0x1c), &tm[src[off + 0x1a]]);
				D3DXVec3TransformCoord(&v[3], (const D3DXVECTOR3*)(src + off), &tm[src[off + 0x1b]]);
				D3DXVec3TransformNormal(&n[3], (const D3DXVECTOR3*)(src + off + 0x1c), &tm[src[off + 0x1b]]);

				*(D3DXVECTOR3*)(dest + dstOff) = v[0] * w[0] + v[1] * w[1] + v[2] * w[2] + v[3] * (1.0f - w[0] - w[1] - w[2]);
				*(D3DXVECTOR3*)(dest + dstOff + 0xc) = n[0] * w[0] + n[1] * w[1] + n[2] * w[2] + n[3] * (1.0f - w[0] - w[1] - w[2]);
				memcpy(dest + dstOff + 0x18, src + off + 0x28, VertexStreamZeroStride - 0x28);
				dstOff += VertexStreamZeroStride - 0x10;
				off += VertexStreamZeroStride;
			}
		} else if ((xLastVertexDecl & D3DFVF_POSITION_MASK) == D3DFVF_XYZB3) {
			for (int i = 0; i < NumVertices; i++) {
				const float* w = (const float*)(src + off + 0xc);
				D3DXVECTOR3 v[3], n[3];
				D3DXVec3TransformCoord(&v[0], (const D3DXVECTOR3*)(src + off), &tm[src[off + 0x14]]);
				D3DXVec3TransformNormal(&n[0], (const D3DXVECTOR3*)(src + off + 0x18), &tm[src[off + 0x14]]);
				D3DXVec3TransformCoord(&v[1], (const D3DXVECTOR3*)(src + off), &tm[src[off + 0x15]]);
				D3DXVec3TransformNormal(&n[1], (const D3DXVECTOR3*)(src + off + 0x18), &tm[src[off + 0x15]]);
				D3DXVec3TransformCoord(&v[2], (const D3DXVECTOR3*)(src + off), &tm[src[off + 0x16]]);
				D3DXVec3TransformNormal(&n[2], (const D3DXVECTOR3*)(src + off + 0x18), &tm[src[off + 0x16]]);

				*(D3DXVECTOR3*)(dest + dstOff) = v[0] * w[0] + v[1] * w[1] + v[2] * (1.0f - w[0] - w[1]);
				*(D3DXVECTOR3*)(dest + dstOff + 0xc) = n[0] * w[0] + n[1] * w[1] + n[2] * (1.0f - w[0] - w[1]);
				memcpy(dest + dstOff + 0x18, src + off + 0x24, VertexStreamZeroStride - 0x24);
				dstOff += VertexStreamZeroStride - 0xc;
				off += VertexStreamZeroStride;
			}
		} else if ((xLastVertexDecl & D3DFVF_POSITION_MASK) == D3DFVF_XYZB2) {
			for (int i = 0; i < NumVertices; i++) {
				const float* w = (const float*)(src + off + 0xc);
				D3DXVECTOR3 v[2], n[2];
				D3DXVec3TransformCoord(&v[0], (const D3DXVECTOR3*)(src + off), &tm[src[off + 0x10]]);
				D3DXVec3TransformNormal(&n[0], (const D3DXVECTOR3*)(src + off + 0x14), &tm[src[off + 0x10]]);
				D3DXVec3TransformCoord(&v[1], (const D3DXVECTOR3*)(src + off), &tm[src[off + 0x11]]);
				D3DXVec3TransformNormal(&n[1], (const D3DXVECTOR3*)(src + off + 0x14), &tm[src[off + 0x11]]);

				*(D3DXVECTOR3*)(dest + dstOff) = v[0] * w[0] + v[1] * (1.0f - w[0]);
				*(D3DXVECTOR3*)(dest + dstOff + 0xc) = n[0] * w[0] + n[1] * (1.0f - w[0]);
				memcpy(dest + dstOff + 0x18, src + off + 0x20, VertexStreamZeroStride - 0x20);
				dstOff += VertexStreamZeroStride - 8;
				off += VertexStreamZeroStride;
			}
		} else if ((xLastVertexDecl & D3DFVF_POSITION_MASK) == D3DFVF_XYZB1) {
			for (int i = 0; i < NumVertices; i++) {
				D3DXVec3TransformCoord((D3DXVECTOR3*)(dest + dstOff), (const D3DXVECTOR3*)(src + off), &tm[src[off + 0xc]]);
				D3DXVec3TransformNormal((D3DXVECTOR3*)(dest + dstOff + 0xc), (const D3DXVECTOR3*)(src + off + 0x10), &tm[src[off + 0xc]]);
				memcpy(dest + dstOff + 0x18, src + off + 0x1c, VertexStreamZeroStride - 0x1c);
				dstOff += VertexStreamZeroStride - 4;
				off += VertexStreamZeroStride;
			}
		}
	} else {
		if ((xLastVertexDecl & D3DFVF_POSITION_MASK) == D3DFVF_XYZB4) {
			for (int i = 0; i < NumVertices; i++) {
				const float* w = (const float*)(src + 0xc);
				D3DXVECTOR3 v[4];
				D3DXVec3TransformCoord(&v[0], (const D3DXVECTOR3*)src, &tm[src[0x18]]);
				D3DXVec3TransformCoord(&v[1], (const D3DXVECTOR3*)src, &tm[src[0x19]]);
				D3DXVec3TransformCoord(&v[2], (const D3DXVECTOR3*)src, &tm[src[0x1a]]);
				D3DXVec3TransformCoord(&v[3], (const D3DXVECTOR3*)src, &tm[src[0x1b]]);

				*(D3DXVECTOR3*)(dest + dstOff) = v[0] * w[0] + v[1] * w[1] + v[2] * w[2] + v[3] * (1.0f - w[0] - w[1] - w[2]);
				memcpy(dest + dstOff + 0xc, src + 0x1c, VertexStreamZeroStride - 0x1c);
				dstOff += VertexStreamZeroStride - 0x10;
				src += VertexStreamZeroStride;
			}
		} else if ((xLastVertexDecl & D3DFVF_POSITION_MASK) == D3DFVF_XYZB3) {
			for (int i = 0; i < NumVertices; i++) {
				const float* w = (const float*)(src + off + 0xc);
				D3DXVECTOR3 v[3];
				D3DXVec3TransformCoord(&v[0], (const D3DXVECTOR3*)(src + off), &tm[src[off + 0x14]]);
				D3DXVec3TransformCoord(&v[1], (const D3DXVECTOR3*)(src + off), &tm[src[off + 0x15]]);
				D3DXVec3TransformCoord(&v[2], (const D3DXVECTOR3*)(src + off), &tm[src[off + 0x16]]);

				*(D3DXVECTOR3*)(dest + dstOff) = v[0] * w[0] + v[1] * w[1] + v[2] * (1.0f - w[0] - w[1]);
				memcpy(dest + dstOff + 0xc, src + off + 0x18, VertexStreamZeroStride - 0x18);
				dstOff += VertexStreamZeroStride - 0xc;
				off += VertexStreamZeroStride;
			}
		} else if ((xLastVertexDecl & D3DFVF_POSITION_MASK) == D3DFVF_XYZB2) {
			for (int i = 0; i < NumVertices; i++) {
				const float* w = (const float*)(src + off + 0xc);
				D3DXVECTOR3 v[2];
				D3DXVec3TransformCoord(&v[0], (const D3DXVECTOR3*)(src + off), &tm[src[off + 0x10]]);
				D3DXVec3TransformCoord(&v[1], (const D3DXVECTOR3*)(src + off), &tm[src[off + 0x11]]);

				*(D3DXVECTOR3*)(dest + dstOff) = v[0] * w[0] + v[1] * (1.0f - w[0]);
				memcpy(dest + dstOff + 0xc, src + off + 0x14, VertexStreamZeroStride - 0x14);
				dstOff += VertexStreamZeroStride - 8;
				off += VertexStreamZeroStride;
			}
		} else if ((xLastVertexDecl & D3DFVF_POSITION_MASK) == D3DFVF_XYZB1) {
			for (int i = 0; i < NumVertices; i++) {
				D3DXVec3TransformCoord((D3DXVECTOR3*)(dest + dstOff), (const D3DXVECTOR3*)(src + off), &tm[src[off + 0xc]]);
				memcpy(dest + dstOff + 0xc, src + off + 0x10, VertexStreamZeroStride - 0x10);
				dstOff += VertexStreamZeroStride - 4;
				off += VertexStreamZeroStride;
			}
		}
	}
}

// wd3d8.cpp:5665
void WDirect3D8::_DrawPrimitiveUP(_D3DPRIMITIVETYPE Type, unsigned int PrimitiveCount, const void* pVertexStreamZeroData, unsigned int VertexStreamZeroStride) {
	if (m_devState == W_VDEVSTATE_LOST) return;

	int NumVertices = PrimitiveCount;
	switch (Type) {
	case D3DPT_TRIANGLEFAN: NumVertices = PrimitiveCount + 2; break;
	case D3DPT_TRIANGLESTRIP: NumVertices = PrimitiveCount + 2; break;
	case D3DPT_TRIANGLELIST: NumVertices = PrimitiveCount * 3; break;
	case D3DPT_LINESTRIP: NumVertices = PrimitiveCount + 1; break;
	case D3DPT_LINELIST: NumVertices = PrimitiveCount * 2; break;
	}

#ifdef PG_MATERIAL_HINTS
	// Draws of another material hint are not merged.
	if (m_MergeBuffer.PrimitiveCount > 0 && s_pgMergeHint != PgMaterialHintGet())
		FlushRenderPrimitive();
	s_pgMergeHint = PgMaterialHintGet();
#endif

	if (m_MergeBuffer.PrimitiveCount > 0) {
		bool merge = false;
		if (m_MergeBuffer.Type == D3DPT_LINELIST || m_MergeBuffer.Type == D3DPT_LINESTRIP) {
			if (Type == D3DPT_LINELIST || Type == D3DPT_LINESTRIP)
				merge = true;
		} else if (m_MergeBuffer.Type == D3DPT_TRIANGLELIST || m_MergeBuffer.Type == D3DPT_TRIANGLESTRIP || m_MergeBuffer.Type == D3DPT_TRIANGLEFAN) {
			if (Type == D3DPT_TRIANGLELIST || Type == D3DPT_TRIANGLESTRIP || Type == D3DPT_TRIANGLEFAN)
				merge = true;
		}

		if (m_MergeBuffer.xhVb == 0 && m_MergeBuffer.xhIb == 0 && m_MergeBuffer.VertexStreamZeroStride == VertexStreamZeroStride && (m_MergeBuffer.NumVertices + NumVertices) * VertexStreamZeroStride < m_MergeBuffer.GetVertexStreamBufferSize() && merge) {
			int indicesPerPrimitive = 3;
			if (Type == D3DPT_LINELIST || Type == D3DPT_LINESTRIP) {
				indicesPerPrimitive = 2;
			} else if (Type == D3DPT_TRIANGLELIST || Type == D3DPT_TRIANGLESTRIP || Type == D3DPT_TRIANGLEFAN) {
				indicesPerPrimitive = 3;
			}

			m_MergeBuffer.CheckAndIncreaseIndexBuffer((m_MergeBuffer.PrimitiveCount + PrimitiveCount) * indicesPerPrimitive * sizeof(unsigned short));

			if (!m_MergeBuffer.LockIndex) {
				m_MergeBuffer.PrimitiveCount = FillIndices(m_MergeBuffer.Type, m_MergeBuffer.IndexBuffer, 0, m_MergeBuffer.NumVertices);
				m_MergeBuffer.LockIndex = 1;
				m_MergeBuffer.Type = m_MergeBuffer.Type == D3DPT_LINESTRIP ? D3DPT_LINELIST : D3DPT_TRIANGLELIST;
			}

			int offset;
			if (m_MergeBuffer.Type == D3DPT_LINELIST) {
				offset = m_MergeBuffer.PrimitiveCount * 2;
			} else {
				offset = m_MergeBuffer.PrimitiveCount * 3;
			}

			FillVertex(m_MergeBuffer.VertexStreamBuffer + m_MergeBuffer.NumVertices * VertexStreamZeroStride, (const unsigned char*)pVertexStreamZeroData, m_xLastVertexDecl, NumVertices, VertexStreamZeroStride, m_xLastWorldMatrix);
			m_MergeBuffer.PrimitiveCount += FillIndices(Type, m_MergeBuffer.IndexBuffer + offset, m_MergeBuffer.NumVertices, NumVertices);
			m_MergeBuffer.NumVertices += NumVertices;
			return;
		}

		FlushRenderPrimitive();
	}

	m_MergeBuffer.Type = Type;
	m_MergeBuffer.PrimitiveCount = PrimitiveCount;
	m_MergeBuffer.VertexStreamZeroStride = VertexStreamZeroStride;
	m_MergeBuffer.NumVertices = NumVertices;
	m_MergeBuffer.xhVb = 0;
	m_MergeBuffer.xhIb = 0;
	m_MergeBuffer.LockVertex = 1;
	m_MergeBuffer.LockIndex = 0;
	m_MergeBuffer.m_xLastMatrix = m_xLastWorldMatrix[0];
	m_MergeBuffer.CheckAndIncreaseVertexStreamBuffer(NumVertices * VertexStreamZeroStride);

	FillVertex(m_MergeBuffer.VertexStreamBuffer, (const unsigned char*)pVertexStreamZeroData, m_xLastVertexDecl, NumVertices, VertexStreamZeroStride, m_xLastWorldMatrix);
}

// wd3d8.cpp:5797
void WDirect3D8::_DrawIndexedPrimitive(_D3DPRIMITIVETYPE Type, unsigned int MinIndex, unsigned int NumVertices, unsigned int StartIndex, unsigned int PrimitiveCount) {
	if (m_devState == W_VDEVSTATE_LOST) return;

#ifdef PG_MATERIAL_HINTS
	// Draws of another material hint are not merged.
	if (m_MergeBuffer.PrimitiveCount > 0 && s_pgMergeHint != PgMaterialHintGet())
		FlushRenderPrimitive();
	s_pgMergeHint = PgMaterialHintGet();
#endif

	if (m_MergeBuffer.PrimitiveCount > 0) {
		if (m_MergeBuffer.xhVb != 0 && m_MergeBuffer.Type == D3DPT_TRIANGLELIST && m_xaVbList[m_xhLastVb].xbStride == m_MergeBuffer.VertexStreamZeroStride && m_xaVbList[m_xhLastVb].xpVertexData && m_xaIbList[m_xhLastIb].xpIndexData && (m_MergeBuffer.NumVertices + NumVertices) * m_xaVbList[m_xhLastVb].xbStride <= m_MergeBuffer.GetVertexStreamBufferSize()) {
			m_MergeBuffer.CheckAndIncreaseIndexBuffer((m_MergeBuffer.PrimitiveCount + PrimitiveCount) * 3 * sizeof(unsigned short));

			if (!m_MergeBuffer.LockVertex) {
				m_MergeBuffer.LockVertex = 1;
				m_MergeBuffer.LockIndex = 1;
				FillVertex(m_MergeBuffer.VertexStreamBuffer, m_xaVbList[m_MergeBuffer.xhVb].xpVertexData + m_MergeBuffer.MinIndex * m_MergeBuffer.VertexStreamZeroStride, m_xLastVertexDecl, m_MergeBuffer.NumVertices, m_MergeBuffer.VertexStreamZeroStride, &m_MergeBuffer.m_xLastMatrix);

				if (m_MergeBuffer.MinIndex) {
					for (unsigned int i = 0; i < m_MergeBuffer.PrimitiveCount * 3; i++)
						m_MergeBuffer.IndexBuffer[i] = m_xaIbList[m_MergeBuffer.xhIb].xpIndexData[m_MergeBuffer.StartIndex + i] - m_MergeBuffer.MinIndex;
				} else {
					memcpy(m_MergeBuffer.IndexBuffer, &m_xaIbList[m_MergeBuffer.xhIb].xpIndexData[m_MergeBuffer.StartIndex], m_MergeBuffer.PrimitiveCount * 3 * sizeof(unsigned short));
				}
			}

			FillVertex(m_MergeBuffer.VertexStreamBuffer + m_MergeBuffer.NumVertices * m_MergeBuffer.VertexStreamZeroStride, m_xaVbList[m_xhLastVb].xpVertexData + MinIndex * m_MergeBuffer.VertexStreamZeroStride, m_xLastVertexDecl, NumVertices, m_MergeBuffer.VertexStreamZeroStride, m_xLastWorldMatrix);

			for (unsigned int i = 0; i < PrimitiveCount * 3; i++)
				m_MergeBuffer.IndexBuffer[m_MergeBuffer.PrimitiveCount * 3 + i] = m_xaIbList[m_xhLastIb].xpIndexData[StartIndex + i] - MinIndex + m_MergeBuffer.NumVertices;

			m_MergeBuffer.NumVertices += NumVertices;
			m_MergeBuffer.PrimitiveCount += PrimitiveCount;
			return;
		}

		FlushRenderPrimitive();
	}

	m_MergeBuffer.Type = Type;
	m_MergeBuffer.NumVertices = NumVertices;
	m_MergeBuffer.StartIndex = StartIndex;
	m_MergeBuffer.LockVertex = 0;
	m_MergeBuffer.LockIndex = 0;
	m_MergeBuffer.MinIndex = MinIndex;
	m_MergeBuffer.PrimitiveCount = PrimitiveCount;
	m_MergeBuffer.xhVb = m_xhLastVb;
	m_MergeBuffer.xhIb = m_xhLastIb;
	m_MergeBuffer.VertexStreamZeroStride = m_xaVbList[m_xhLastVb].xbStride;
	m_MergeBuffer.m_xLastMatrix = m_xLastWorldMatrix[0];

	if (!m_xaVbList[m_xhLastVb].xpVertexData || !m_xaIbList[m_xhLastIb].xpIndexData || (m_lastRenderState & 0x1000))
		FlushRenderPrimitive();
}

// wd3d8.cpp:5906
void WDirect3D8::_DrawIndexedPrimitiveUP(_D3DPRIMITIVETYPE Type, unsigned int NumVertices, unsigned int PrimitiveCount, const void* pIndexData, const void* pVertexStreamZeroData, unsigned int VertexStreamZeroStride) {
	if (m_devState == 1) return;

	if (Type == D3DPT_TRIANGLELIST) {
		DrawIndexedPrimitiveLockable(D3DPT_TRIANGLELIST, pVertexStreamZeroData, NumVertices, VertexStreamZeroStride, (unsigned short*)pIndexData, PrimitiveCount);
		return;
	}

	m_CustomRenderState.Begin(*this);

	m_pd3dDevice->DrawIndexedPrimitiveUP(Type, 0, NumVertices, PrimitiveCount, pIndexData, D3DFMT_INDEX16, pVertexStreamZeroData, VertexStreamZeroStride);

	m_xnDIPUPs++;
	m_xnTotalTris += PrimitiveCount;

	m_CustomRenderState.End(*this);
}

// wd3d8.cpp:5933
void WDirect3D8::_SetTexture(unsigned long Stage, IDirect3DTexture9* pTexture) {
	if (Stage >= 2) {
		FlushRenderPrimitive();
	} else if (m_pTexture[Stage] != pTexture) {
		FlushRenderPrimitive();
		m_pTexture[Stage] = pTexture;
	}

	m_pd3dDevice->SetTexture(Stage, pTexture);
}

// wd3d8.cpp:5950
void WDirect3D8::_SetLight(unsigned long Index, const _D3DLIGHT9* pLight) {
	FlushRenderPrimitive();

	m_pd3dDevice->SetLight(Index, pLight);
}

// wd3d8.cpp:5959
void WDirect3D8::_SetShader(unsigned long flags) {
	m_Effect = flags;

	if (flags) {
		std::map<unsigned long, sEffect>::const_iterator it = m_EffectTable.find(flags);

		if (it == m_EffectTable.end()) {
			if (!CreateEffect(flags))
				m_Effect = 0;
		} else {
			if (it->second.pEffect == NULL)
				m_Effect = 0;
		}
	}
}

// wd3d8.cpp:5981
bool WDirect3D8::ApplyShader(void) {
	bool ret = true;

	if (m_lastEffect && m_lastEffect != m_Effect) {
		sEffect& last = m_EffectTable[m_lastEffect];

		if (last.pEffect) {
			last.pEffect->EndPass();
			last.pEffect->End();
		}
	}

	if (m_Effect == 0) {
		if (m_lastEffect != 0) {
			m_pd3dDevice->SetVertexShader(NULL);
			m_pd3dDevice->SetPixelShader(NULL);
		}
	} else {
		UpdateShaderValue(m_Effect);

		sEffect& effect = m_EffectTable[m_Effect];

		if (effect.pEffect) {
			if (m_lastEffect == m_Effect) {
				effect.pEffect->CommitChanges();
			} else {
				unsigned int passcnt;
				effect.pEffect->Begin(&passcnt, D3DXFX_DONOTSAVESTATE | D3DXFX_DONOTSAVESHADERSTATE | D3DXFX_DONOTSAVESAMPLERSTATE);
				effect.pEffect->BeginPass(0);
			}
		} else {
			ret = false;
		}
	}

	m_lastEffect = m_Effect;

	return ret;
}

// wd3d8.cpp:6032
// Projects a cube environment map onto order-3 spherical harmonics and stores
// the irradiance constants of Sloan's "Efficient Evaluation of Irradiance
// Environment Maps" (ShaderX2) in m_SHCoeff, as the DirectX SDK samples do.
bool WDirect3D8::LoadSHCoeff(const char* fname) {
	IDirect3DCubeTexture9* pScratchEnvironmentMap = NULL;
	HRESULT hr = D3DXCreateCubeTextureFromFile(m_pd3dDevice, fname, &pScratchEnvironmentMap);

	if (SUCCEEDED(hr)) {
		float fLight[3][9];

		ZeroMemory(fLight, sizeof(fLight));

		D3DXSHProjectCubeMap(3, pScratchEnvironmentMap, fLight[0], fLight[1], fLight[2]);

		static const float s_fSqrtPI = ((float)sqrt(D3DX_PI));
		const float fC0 = 1.0f / (2.0f * s_fSqrtPI);

		const float fC1 = 1.0f / ((float)sqrt(3.0f) * s_fSqrtPI);
		const float fC2 = (float)sqrt(15.0f) / (8.0f * s_fSqrtPI);
		const float fC3 = (float)sqrt(5.0f) / (16.0f * s_fSqrtPI);
		const float fC4 = 0.5f * fC2;

		int iChannel;

		for (iChannel = 0; iChannel < 3; iChannel++) {
			m_SHCoeff[iChannel][0] = -fC1 * fLight[iChannel][3];
			m_SHCoeff[iChannel][1] = -fC1 * fLight[iChannel][1];
			m_SHCoeff[iChannel][2] = fC1 * fLight[iChannel][2];
			m_SHCoeff[iChannel][3] = fC0 * fLight[iChannel][0] - fC3 * fLight[iChannel][6];
		}

		for (iChannel = 0; iChannel < 3; iChannel++) {
			m_SHCoeff[3 + iChannel][0] = fC2 * fLight[iChannel][4];
			m_SHCoeff[3 + iChannel][1] = -fC2 * fLight[iChannel][5];
			m_SHCoeff[3 + iChannel][2] = 3.0f * fC3 * fLight[iChannel][6];
			m_SHCoeff[3 + iChannel][3] = -fC2 * fLight[iChannel][7];
		}

		m_SHCoeff[6][0] = fC4 * fLight[0][8];
		m_SHCoeff[6][1] = fC4 * fLight[1][8];
		m_SHCoeff[6][2] = fC4 * fLight[2][8];
		m_SHCoeff[6][3] = 1.0f;

		pScratchEnvironmentMap->Release();
		return true;
	}

	return false;
}

// wd3d8.cpp:6088
void WDirect3D8::UpdateShaderValue(int h) {
	sEffect& effect = m_EffectTable[h];

	if (effect.pEffect == NULL) return;

	// WVDFXPARAM_WORLDVIEWPROJECTION
	if (effect.param[WVDFXPARAM_WORLDVIEWPROJECTION].handle) {
		D3DXMATRIX mat;
		D3DXMATRIX temp;
		D3DXMatrixMultiply(&temp, &m_xLastWorldMatrix[0], &m_xLastViewMatrix);
		D3DXMatrixMultiply(&mat, &temp, &m_xLastProjMatrix);
		m_fxParamPool.SetMatrix(effect.pEffect, WVDFXPARAM_WORLDVIEWPROJECTION, effect.param[WVDFXPARAM_WORLDVIEWPROJECTION].handle, effect.param[WVDFXPARAM_WORLDVIEWPROJECTION].isShared, mat);
	}

	if (effect.param[WVDFXPARAM_WORLD].handle) {
		m_fxParamPool.SetMatrix(effect.pEffect, WVDFXPARAM_WORLD, effect.param[WVDFXPARAM_WORLD].handle, effect.param[WVDFXPARAM_WORLD].isShared, m_xLastWorldMatrix[0]);
	}

	if (effect.param[WVDFXPARAM_WORLDVIEW].handle) {
		D3DXMATRIX mat;
		D3DXMatrixMultiply(&mat, &m_xLastWorldMatrix[0], &m_xLastViewMatrix);
		m_fxParamPool.SetMatrix(effect.pEffect, WVDFXPARAM_WORLDVIEW, effect.param[WVDFXPARAM_WORLDVIEW].handle, effect.param[WVDFXPARAM_WORLDVIEW].isShared, mat);
	}

	if (effect.param[WVDFXPARAM_PREV_VIEW].handle) {
		m_fxParamPool.SetMatrix(effect.pEffect, WVDFXPARAM_PREV_VIEW, effect.param[WVDFXPARAM_PREV_VIEW].handle, effect.param[WVDFXPARAM_PREV_VIEW].isShared, m_xPrevViewMatrix);
	}

	// The original feeds the previous view matrix to WVDFXPARAM_VIEW too.
	if (effect.param[WVDFXPARAM_VIEW].handle) {
		m_fxParamPool.SetMatrix(effect.pEffect, WVDFXPARAM_VIEW, effect.param[WVDFXPARAM_VIEW].handle, effect.param[WVDFXPARAM_VIEW].isShared, m_xPrevViewMatrix);
	}

	if (effect.param[WVDFXPARAM_PROJECTION].handle) {
		m_fxParamPool.SetMatrix(effect.pEffect, WVDFXPARAM_PROJECTION, effect.param[WVDFXPARAM_PROJECTION].handle, effect.param[WVDFXPARAM_PROJECTION].isShared, m_xLastProjMatrix);
	}

	if (effect.param[WVDFXPARAM_VIEWPROJECTION].handle) {
		D3DXMATRIX mat;
		D3DXMatrixMultiply(&mat, &m_xLastViewMatrix, &m_xLastProjMatrix);
		m_fxParamPool.SetMatrix(effect.pEffect, WVDFXPARAM_VIEWPROJECTION, effect.param[WVDFXPARAM_VIEWPROJECTION].handle, effect.param[WVDFXPARAM_VIEWPROJECTION].isShared, mat);
	}

	if (effect.param[WVDFXPARAM_CONSTCOLOR].handle) {
		D3DXVECTOR4 c;
		c.w = ((m_LastTFactor >> 24) & 0xff) * (1.0f / 255.0f);
		c.x = ((m_LastTFactor >> 16) & 0xff) * (1.0f / 255.0f);
		c.y = ((m_LastTFactor >> 8) & 0xff) * (1.0f / 255.0f);
		c.z = (m_LastTFactor & 0xff) * (1.0f / 255.0f);
		m_fxParamPool.SetVector4(effect.pEffect, WVDFXPARAM_CONSTCOLOR, effect.param[WVDFXPARAM_CONSTCOLOR].handle, effect.param[WVDFXPARAM_CONSTCOLOR].isShared, c);
	}

	if (effect.param[WVDFXPARAM_LIGHT_DIFFUSE].handle) {
		m_fxParamPool.SetVector4(effect.pEffect, WVDFXPARAM_LIGHT_DIFFUSE, effect.param[WVDFXPARAM_LIGHT_DIFFUSE].handle, effect.param[WVDFXPARAM_LIGHT_DIFFUSE].isShared, (const D3DXVECTOR4&)m_xaLights[0].Diffuse);
	}

	if (effect.param[WVDFXPARAM_LIGHT_AMBIENT].handle) {
		m_fxParamPool.SetVector4(effect.pEffect, WVDFXPARAM_LIGHT_AMBIENT, effect.param[WVDFXPARAM_LIGHT_AMBIENT].handle, effect.param[WVDFXPARAM_LIGHT_AMBIENT].isShared, (const D3DXVECTOR4&)m_xaLights[0].Ambient);
	}

	if (effect.param[WVDFXPARAM_LIGHT_DIRECTION].handle) {
		D3DXVECTOR4 c(m_xaLights[0].Direction.x, m_xaLights[0].Direction.y, m_xaLights[0].Direction.z, 1.0f);
		m_fxParamPool.SetVector4(effect.pEffect, WVDFXPARAM_LIGHT_DIRECTION, effect.param[WVDFXPARAM_LIGHT_DIRECTION].handle, effect.param[WVDFXPARAM_LIGHT_DIRECTION].isShared, c);
	}

	if (effect.param[WVDFXPARAM_FOG_RANGE].handle) {
		D3DXVECTOR4 c(m_LastFogStart, m_LastFogEnd, 0.0f, 1.0f);
		m_fxParamPool.SetVector4(effect.pEffect, WVDFXPARAM_FOG_RANGE, effect.param[WVDFXPARAM_FOG_RANGE].handle, effect.param[WVDFXPARAM_FOG_RANGE].isShared, c);
	}

	if (effect.param[WVDFXPARAM_FOG_COLOR].handle) {
		D3DXVECTOR3 c;
		c.x = ((m_fogColor >> 16) & 0xff) * (1.0f / 255.0f);
		c.y = ((m_fogColor >> 8) & 0xff) * (1.0f / 255.0f);
		c.z = (m_fogColor & 0xff) * (1.0f / 255.0f);
		m_fxParamPool.SetVector3(effect.pEffect, WVDFXPARAM_FOG_COLOR, effect.param[WVDFXPARAM_FOG_COLOR].handle, effect.param[WVDFXPARAM_FOG_COLOR].isShared, c);
	}

	if (effect.param[WVDFXPARAM_CAMERAPOSITION].handle) {
		D3DXVECTOR4 c(0.0f, 0.0f, 0.0f, 1.0f);
		D3DXMATRIX mat;
		D3DXMatrixInverse(&mat, NULL, &m_xLastViewMatrix);

		c.x = mat._41;
		c.y = mat._42;
		c.z = mat._43;
		m_fxParamPool.SetVector4(effect.pEffect, WVDFXPARAM_CAMERAPOSITION, effect.param[WVDFXPARAM_CAMERAPOSITION].handle, effect.param[WVDFXPARAM_CAMERAPOSITION].isShared, c);
	}

	if (effect.param[WVDFXPARAM_SHCOEFF].handle)
		effect.pEffect->SetVectorArray(effect.param[WVDFXPARAM_SHCOEFF].handle, (const D3DXVECTOR4*)m_SHCoeff, 7);

	if (effect.param[WVDFXPARAM_MATERIALCOLOR].handle) {
		D3DXVECTOR4 c;
		c.w = ((m_xdwDiffuse >> 24) & 0xff) * (1.0f / 255.0f);
		c.x = ((m_xdwDiffuse >> 16) & 0xff) * (1.0f / 255.0f);
		c.y = ((m_xdwDiffuse >> 8) & 0xff) * (1.0f / 255.0f);
		c.z = (m_xdwDiffuse & 0xff) * (1.0f / 255.0f);
		m_fxParamPool.SetVector4(effect.pEffect, WVDFXPARAM_MATERIALCOLOR, effect.param[WVDFXPARAM_MATERIALCOLOR].handle, effect.param[WVDFXPARAM_MATERIALCOLOR].isShared, c);
	}
}

// wd3d8.cpp:6204
void WDirect3D8::ReloadShader(void) {
	m_Effect = 0;
	ApplyShader();

	ReleaseShaderResource();

	if (IsSupportVS() || IsSupportPS()) {
	}
}

// wd3d8.cpp:6215
void WDirect3D8::ReleaseShaderResource(void) {
	m_shadersrc = "";

	for (std::map<unsigned long, sEffect>::iterator it = m_EffectTable.begin(); it != m_EffectTable.end(); ++it) {
		if (it->second.pEffect) {
			it->second.pEffect->Release();
		}
	}
	m_EffectTable.clear();

	m_fxParamPool.Release();
}

// The shader source's #ifdef switches, one per effect flag bit.
static const struct {
	unsigned long flag;
	const char* name;
} macroList[] = {
	{ 0x00000001, "WVDFXMACRO_TEXTURE" },
	{ 0x00000002, "WVDFXMACRO_NORMAL" },
	{ 0x00000004, "WVDFXMACRO_VERTEX_COLOR" },
	{ 0x00000008, "WVDFXMACRO_MATERIAL_COLOR" },
	{ 0x00000010, "WVDFXMACRO_TEXTURE_STAGE2" },
	{ 0x00000020, "WVDFXMACRO_TEXTURE_STAGE2_ADD" },
	{ 0x00000040, "WVDFXMACRO_USE_CONSTCOLOR" },
	{ 0x00000080, "WVDFXMACRO_FOG" },
	{ 0x00000100, "WVDFXMACRO_NO_LIGHTING" },
	{ 0x00000200, "WVDFXMACRO_RIMLIGHTING" },
	{ 0x00000400, "WVDFXMACRO_APPLY_SPHERICAL_HARMONICS" },
	{ 0x00000800, "WVDFXMACRO_POSTPROCESSING_DOF" },
	{ 0x00001000, "WVDFXMACRO_POSTPROCESSING_MOTIONBLUR" },
	{ 0x00002000, "WVDFXMACRO_OVERLAY" },
	{ 0x00004000, "WVDFXMACRO_MRT_VELOCITY" },
	{ 0x00008000, "WVDFXMACRO_MRT_DOF_FACTOR" },
	{ 0x00010000, "WVDFXMACRO_PROJ_TEXCOORD" },
	{ 0x00020000, "WVDFXMACRO_SHADOW_CASTER" },
	{ 0x00040000, "WVDFXMACRO_SHADOW_RECVER" },
	{ 0x00080000, "WVDFXMACRO_BLURRING" },
	{ 0x10000000, "WVDFXMACRO_PERPIXEL_LIGHTING" },
	{ 0x20000000, "WVDFXMACRO_NO_VERTEXSHADER" },
	{ 0x40000000, "WVDFXMACRO_NO_PIXELSHADER" },
	{ 0x80000000, "WVDFXMACRO_PARAM_SHARING" },
};

// The engine parameters, found in the effect by their semantic.
static const struct {
	WVDFXPARAMETERTYPE type;
	const char* name;
} paramList[] = {
	{ WVDFXPARAM_WORLDVIEWPROJECTION, "WVDFXPARAM_WORLDVIEWPROJECTION" },
	{ WVDFXPARAM_WORLD, "WVDFXPARAM_WORLD" },
	{ WVDFXPARAM_WORLDVIEW, "WVDFXPARAM_WORLDVIEW" },
	{ WVDFXPARAM_PREV_VIEW, "WVDFXPARAM_PREV_VIEW" },
	{ WVDFXPARAM_VIEW, "WVDFXPARAM_VIEW" },
	{ WVDFXPARAM_PROJECTION, "WVDFXPARAM_PROJECTION" },
	{ WVDFXPARAM_VIEWPROJECTION, "WVDFXPARAM_VIEWPROJECTION" },
	{ WVDFXPARAM_CONSTCOLOR, "WVDFXPARAM_CONSTCOLOR" },
	{ WVDFXPARAM_LIGHT_DIFFUSE, "WVDFXPARAM_LIGHT_DIFFUSE" },
	{ WVDFXPARAM_LIGHT_AMBIENT, "WVDFXPARAM_LIGHT_AMBIENT" },
	{ WVDFXPARAM_LIGHT_DIRECTION, "WVDFXPARAM_LIGHT_DIRECTION" },
	{ WVDFXPARAM_FOG_RANGE, "WVDFXPARAM_FOG_RANGE" },
	{ WVDFXPARAM_FOG_COLOR, "WVDFXPARAM_FOG_COLOR" },
	{ WVDFXPARAM_CAMERAPOSITION, "WVDFXPARAM_CAMERAPOSITION" },
	{ WVDFXPARAM_SHCOEFF, "WVDFXPARAM_SHCOEFF" },
	{ WVDFXPARAM_EXTRATEXTURE, "WVDFXPARAM_EXTRATEXTURE" },
	{ WVDFXPARAM_REFTEXTURE, "WVDFXPARAM_REFTEXTURE" },
	{ WVDFXPARAM_MATERIALCOLOR, "WVDFXPARAM_MATERIALCOLOR" },
	{ WVDFXPARAM_TEXTURE_TRANSFORM, "WVDFXPARAM_TEXTURE_TRANSFORM" },
	{ WVDFXPARAM_BLUR_UVOFFSET, "WVDFXPARAM_BLUR_UVOFFSET" },
	{ WVDFXPARAM_SHADOW_COLOR, "WVDFXPARAM_SHADOW_COLOR" },
};

// wd3d8.cpp:6232
bool WDirect3D8::CreateEffect(unsigned long flags) {
	D3DXMACRO macro[24];
	memset(macro, 0, sizeof(macro));
	int n = 0;
	for (int i = 0; i < sizeof(macroList) / sizeof(macroList[0]); ++i) {
		if (macroList[i].flag & flags) {
			macro[n].Name = macroList[i].name;
			macro[n].Definition = "";
			++n;
		}
	}

	ID3DXBuffer* err = NULL;
	sEffect& effect = m_EffectTable[flags];

	memset(&effect, 0, sizeof(effect));

	if (D3DXCreateEffect(m_pd3dDevice, m_shadersrc.c_str(), m_shadersrc.size(), macro, NULL, 0, m_fxParamPool.GetPool(), &effect.pEffect, &err) != D3D_OK)
		return false;

	_D3DXPARAMETER_DESC paramDesc;
	for (int i = 0; i < sizeof(paramList) / sizeof(paramList[0]); ++i) {
		D3DXHANDLE hParam = effect.pEffect->GetParameterBySemantic(NULL, paramList[i].name);
		if (hParam && effect.pEffect->IsParameterUsed(hParam, NULL) == TRUE) {
			effect.param[paramList[i].type].handle = hParam;
			effect.pEffect->GetParameterDesc(hParam, &paramDesc);
			if (paramDesc.Flags & D3DX_PARAMETER_SHARED)
				effect.param[paramList[i].type].isShared = true;
		}
	}

	return true;
}

// wd3d8.cpp:6410
void WDirect3D8::_SetTransform(_D3DTRANSFORMSTATETYPE state, const D3DXMATRIX& matrix) {
	if (m_devState == 1) return;

	if (state == D3DTS_VIEW) {
		if (m_xLastViewMatrix == matrix) return;

		FlushRenderPrimitive();

		m_xLastViewMatrix = matrix;
	} else if (state == D3DTS_PROJECTION) {
		if (m_xLastProjMatrix == matrix) return;

		FlushRenderPrimitive();

		m_xLastProjMatrix = matrix;
	} else if (state >= D3DTS_WORLD) {
		if (state - D3DTS_WORLD < sizeof(m_xLastWorldMatrix) / sizeof(m_xLastWorldMatrix[0])) {
			if (m_xLastWorldMatrix[state - D3DTS_WORLD] == matrix) return;

			m_xLastWorldMatrix[state - D3DTS_WORLD] = matrix;

			if (IsSupportVS() || state != D3DTS_WORLD) return;
		}
	}

	m_pd3dDevice->SetTransform(state, &matrix);
}

// wd3d8.cpp:6462
void WDirect3D8::_SetRenderState(_D3DRENDERSTATETYPE State, unsigned long Value) {
	FlushRenderPrimitive();

	switch (State) {
	case D3DRS_TEXTUREFACTOR:
		m_LastTFactor = Value;
		break;
	case D3DRS_FOGSTART:
		m_LastFogStart = *(float*)&Value;
		break;
	case D3DRS_FOGEND:
		m_LastFogEnd = *(float*)&Value;
		break;
	case D3DRS_LIGHTING:
		m_LightEnable = Value != 0;
		break;
	}

	m_pd3dDevice->SetRenderState(State, Value);
}

// wd3d8.cpp:6487
void WDirect3D8::_SetTextureStageState(unsigned long Stage, _D3DTEXTURESTAGESTATETYPE Type, unsigned long Value) {
	FlushRenderPrimitive();

	m_pd3dDevice->SetTextureStageState(Stage, Type, Value);
}

// wd3d8.cpp:6496
void WDirect3D8::_SetSamplerState(unsigned long Sampler, _D3DSAMPLERSTATETYPE Type, unsigned long Value) {
	FlushRenderPrimitive();

	m_pd3dDevice->SetSamplerState(Sampler, Type, Value);
}

// wd3d8.cpp:6506
void WDirect3D8::DrawPrimitiveIndexed(int iType, unsigned long dwVertexTypeDesc, void* lpvVertices, int pnum, unsigned short* flist, int fnum, int iType2) {
	if (m_devState == W_VDEVSTATE_LOST) return;

	if (m_xLastWorldMatrix[0] != g_mId) {
		m_xLastWorldMatrix[0] = g_mId;
		if (!IsSupportVS())
			m_pd3dDevice->SetTransform(D3DTS_WORLD, &g_mId);
	}

	SetVtxType(iType, iType2 | 4, dwVertexTypeDesc, 0xffffffff);

	if (m_devState != W_VDEVSTATE_LOST)
		DrawIndexedPrimitiveLockable(D3DPT_TRIANGLELIST, lpvVertices, pnum, m_vtxSize, flist, fnum / 3);
}

// The alpha reference of the last flushing pass (a file-local of the DLL,
// 0x100440ac).
static unsigned long dwAlphaRef = 0;

// wd3d8.cpp:6524
bool WDirect3D8::SetRenderState4Flushing(int pass) {
	if (pass == 0) {
		_SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_EQUAL);
		_SetRenderState(D3DRS_ALPHAREF, 0xff);

		if (m_lastRenderState & 0x80000000) {
			dwAlphaRef = 0x80;
		} else {
			dwAlphaRef = 0;
			return false;
		}
	} else if (pass == 1) {
		_SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESS);
	} else if (pass == 2) {
		_SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESSEQUAL);
	} else {
		_SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		_SetRenderState(D3DRS_ALPHAREF, dwAlphaRef);
	}

	return true;
}

// wd3d8.cpp:6561
int WDirect3D8::UploadCompressedTextureSurface(void* pSrcData, unsigned int srcDataSize, int type) {
	// The data is a .dds file: the "DDS " magic, then its surface description.
	_DDSURFACEDESC2* ddsd = (_DDSURFACEDESC2*)((unsigned char*)pSrcData + 4);

	if (ddsd->dwWidth > m_d3dcaps.MaxTextureWidth || ddsd->dwHeight > m_d3dcaps.MaxTextureHeight) {
		// Too large for the device: decompress it and upload a clamped bitmap.
		WDXTC dxtc;

		dxtc.Load((const unsigned char*)pSrcData, srcDataSize);
		unsigned char* pDecompData = dxtc.GetDecompData();

		tagBITMAPINFO bmpinfo;
		bmpinfo.bmiHeader.biWidth = ddsd->dwWidth > m_d3dcaps.MaxTextureWidth ? m_d3dcaps.MaxTextureWidth : ddsd->dwWidth;
		bmpinfo.bmiHeader.biHeight = ddsd->dwHeight > m_d3dcaps.MaxTextureHeight ? m_d3dcaps.MaxTextureHeight : ddsd->dwHeight;
		bmpinfo.bmiHeader.biBitCount = 32;
		int hTex = CreateTexture(&bmpinfo, type);

		bmpinfo.bmiHeader.biWidth = ddsd->dwWidth;
		bmpinfo.bmiHeader.biHeight = ddsd->dwHeight;
		UpdateTextureSurface(hTex, &bmpinfo, pDecompData, type);

		return hTex;
	}

	int hTex = GetTextureNum(((unsigned int)type >> 29) & 1);

	m_texList[hTex].width = ddsd->dwWidth >> m_iDDSRes;
	m_texList[hTex].height = ddsd->dwHeight >> m_iDDSRes;

	m_texList[hTex].pixFmtInfo = NULL;
	m_texList[hTex].dxtcDataSize = srcDataSize;
	m_texList[hTex].dxtcData = new unsigned char[srcDataSize];
	memcpy(m_texList[hTex].dxtcData, pSrcData, srcDataSize);

	m_texList[hTex].updateType = 0;
	m_texList[hTex].needToBeFilled = false;

	return hTex;
}

// wd3d8.cpp:6737
void WDirect3D8::UpdateCompressedTexture(int texHandle, void* pSrc, unsigned int srcSize, int type) {
}

// wd3d8.cpp:6847
void WDirect3D8::SetRenderTargetFormat(void) {
	m_RtFmtIdx = -1;
	m_RtNoAlphaFmtIdx = -1;
	m_RtLowMemFmtIdx = -1;
	m_RtNoAlphaLowMemFmtIdx = -1;
	m_RtDepthFmtIdx = -1;

	int i;
	for (i = 0; i < sizeof(ms_RtFmt) / sizeof(ms_RtFmt[0]); i++) {
		if (m_d3d8->CheckDeviceFormat(m_devId, D3DDEVTYPE_HAL, m_d3dpp.BackBufferFormat, D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, ms_RtFmt[i].fmt) == D3D_OK) {
			m_RtFmtIdx = i;
			break;
		}
	}

	for (i = 0; i < sizeof(ms_RtNoAlphaFmt) / sizeof(ms_RtNoAlphaFmt[0]); i++) {
		if (m_d3d8->CheckDeviceFormat(m_devId, D3DDEVTYPE_HAL, m_d3dpp.BackBufferFormat, D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, ms_RtNoAlphaFmt[i].fmt) == D3D_OK) {
			m_RtNoAlphaFmtIdx = i;
			break;
		}
	}

	for (i = 0; i < sizeof(ms_RtDepthFmt) / sizeof(ms_RtDepthFmt[0]); i++) {
		if (m_d3d8->CheckDeviceFormat(m_devId, D3DDEVTYPE_HAL, m_d3dpp.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, ms_RtDepthFmt[i].fmt) == D3D_OK) {
			if (m_d3d8->CheckDepthStencilMatch(m_devId, D3DDEVTYPE_HAL, m_d3dpp.BackBufferFormat, ms_RtFmt[m_RtFmtIdx].fmt, ms_RtDepthFmt[i].fmt) == D3D_OK
				&& m_d3d8->CheckDepthStencilMatch(m_devId, D3DDEVTYPE_HAL, m_d3dpp.BackBufferFormat, ms_RtNoAlphaFmt[m_RtNoAlphaFmtIdx].fmt, ms_RtDepthFmt[i].fmt) == D3D_OK) {
				m_RtDepthFmtIdx = i;
				break;
			}
		}
	}

	// Low-memory formats: 16 bits per pixel at most.
	for (i = 0; i < sizeof(ms_RtFmt) / sizeof(ms_RtFmt[0]); i++) {
		if (ms_RtFmt[i].bpp <= 2) {
			if (m_d3d8->CheckDeviceFormat(m_devId, D3DDEVTYPE_HAL, m_d3dpp.BackBufferFormat, D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, ms_RtFmt[i].fmt) == D3D_OK) {
				if (m_d3d8->CheckDepthStencilMatch(m_devId, D3DDEVTYPE_HAL, m_d3dpp.BackBufferFormat, ms_RtFmt[i].fmt, ms_RtDepthFmt[m_RtDepthFmtIdx].fmt) == D3D_OK) {
					m_RtLowMemFmtIdx = i;
					break;
				}
			}
		}
	}

	for (i = 0; i < sizeof(ms_RtNoAlphaFmt) / sizeof(ms_RtNoAlphaFmt[0]); i++) {
		if (ms_RtNoAlphaFmt[i].bpp <= 2) {
			if (m_d3d8->CheckDeviceFormat(m_devId, D3DDEVTYPE_HAL, m_d3dpp.BackBufferFormat, D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, ms_RtNoAlphaFmt[i].fmt) == D3D_OK) {
				if (m_d3d8->CheckDepthStencilMatch(m_devId, D3DDEVTYPE_HAL, m_d3dpp.BackBufferFormat, ms_RtNoAlphaFmt[i].fmt, ms_RtDepthFmt[m_RtDepthFmtIdx].fmt) == D3D_OK) {
					m_RtNoAlphaLowMemFmtIdx = i;
					break;
				}
			}
		}
	}

	if (m_RtLowMemFmtIdx == -1)
		m_RtLowMemFmtIdx = m_RtFmtIdx;

	if (m_RtNoAlphaLowMemFmtIdx == -1)
		m_RtNoAlphaLowMemFmtIdx = m_RtNoAlphaFmtIdx;
}

// wd3d8.cpp:6938
void WDirect3D8::CreateTextureSurface(unsigned long dwWidth, unsigned long dwHeight, int iMipmapcount, int renderTargetType, int iIndex, int bChromakey, int bitnum) {
	unsigned long width;
	unsigned long height;

	if (renderTargetType == 0 && (m_d3dcaps.TextureCaps & D3DPTEXTURECAPS_POW2)) {
		for (width = 1; width < dwWidth; width += width);
		for (height = 1; height < dwHeight; height += height);
	} else {
		width = dwWidth;
		height = dwHeight;
	}

	if (width >= m_d3dcaps.MaxTextureWidth)
		width = m_d3dcaps.MaxTextureWidth;
	if (height >= m_d3dcaps.MaxTextureHeight)
		height = m_d3dcaps.MaxTextureHeight;

	while (((dwWidth >> iMipmapcount) <= 1 || (dwHeight >> iMipmapcount) <= 1) && iMipmapcount >= 0)
		iMipmapcount--;

	m_texList[iIndex].width = width;
	m_texList[iIndex].height = height;
	m_texList[iIndex].pixFmtInfo = m_fmt[(bitnum > 16 ? 3 : 0) + bChromakey];
	m_texList[iIndex].mipmaplevel = iMipmapcount;
	m_texList[iIndex].renderTargetType = renderTargetType;
	m_texList[iIndex].renderTargetSizeInfo.Reset();
	m_texList[iIndex].isFilled = false;

	if (renderTargetType) {
		_D3DFORMAT fmt;
		unsigned long usage;
		switch (renderTargetType & 0xe000) {
		case 0x2000:
			if (renderTargetType & 0x10000)
				fmt = ms_RtFmt[m_RtLowMemFmtIdx].fmt;
			else
				fmt = ms_RtFmt[m_RtFmtIdx].fmt;
			usage = D3DUSAGE_RENDERTARGET;
			break;
		case 0x4000:
			if (renderTargetType & 0x10000)
				fmt = ms_RtNoAlphaFmt[m_RtNoAlphaLowMemFmtIdx].fmt;
			else
				fmt = ms_RtNoAlphaFmt[m_RtNoAlphaFmtIdx].fmt;
			usage = D3DUSAGE_RENDERTARGET;
			break;
		case 0x8000:
			fmt = ms_RtDepthFmt[m_RtDepthFmtIdx].fmt;
			usage = D3DUSAGE_DEPTHSTENCIL;
			break;
		default:
			fmt = ms_RtFmt[m_RtFmtIdx].fmt;
			usage = D3DUSAGE_RENDERTARGET;
			break;
		}

		m_pd3dDevice->CreateTexture(m_texList[iIndex].width, m_texList[iIndex].height, 1, usage, fmt, D3DPOOL_DEFAULT, &m_texList[iIndex].pTex, NULL);
		if (m_texList[iIndex].pTex)
			m_texList[iIndex].pTex->GetSurfaceLevel(0, &m_texList[iIndex].pSurf);
	}
}

// wd3d8.cpp:7050
unsigned char* WDirect3D8::GetPixelPtr(tagBITMAPINFO* bi, unsigned char* data, int x, int y) {
	if (bi->bmiHeader.biBitCount == 8)
		return (unsigned char*)&bi->bmiColors[data[((bi->bmiHeader.biWidth + 3) & ~3) * y + x]];

	return data + (((bi->bmiHeader.biWidth + 1) * 3) & ~3) * y + x * 3;
}

// wd3d8.cpp:7058
void WDirect3D8::UpdateTextureSurfaceNormal(void* buff, int pitch, pix_info* pix, tagBITMAPINFO* bi, void* data) {
	// Fills the surface with a placeholder pattern: white columns, one in
	// three half transparent, the others fully transparent.
	int width = bi->bmiHeader.biWidth;
	int height = bi->bmiHeader.biHeight;

	for (int y = 0; y < height; y++) {
		unsigned char* ptr = (unsigned char*)buff + pitch * y;

		for (int i = 0; i < width; i++) {
			unsigned long code;
			int column = (unsigned int)i % 3;
			if (column < 1)
				code = pix->Pack(0x80, 0xff, 0xff, 0xff);
			else
				code = pix->Pack(0, 0xff, 0xff, 0xff);
			memcpy(ptr, &code, pix->cpp); ptr += pix->cpp;
		}
	}
}

// wd3d8.cpp:7088
void WDirect3D8::UpdateTextureSurfaceDirect(void* buff, int pitch, pix_info* pix, tagBITMAPINFO* bi, void* data) {
	// Converts a bitmap the size of the surface pixel by pixel (8-bit through
	// its palette); an alpha shift of 8 means the format has no alpha.
	int cps = bi->bmiHeader.biBitCount / 8;
	int w = bi->bmiHeader.biWidth, h = bi->bmiHeader.biHeight;

	int nCps = ((w * cps + 3) & ~3) - w * cps; int nPitch = pitch - pix->cpp * w; unsigned char* src = (unsigned char*)data;
	unsigned char* ptr = (unsigned char*)buff;
	unsigned long code;

	if (bi->bmiHeader.biBitCount == 8) {
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				unsigned char* p = (unsigned char*)&bi->bmiColors[*src]; code = pix->a_r_shift == 8 ? pix->Pack(p[2], p[1], p[0]) : pix->Pack(p[3], p[2], p[1], p[0]); src += cps;
				memcpy(ptr, &code, pix->cpp); ptr += pix->cpp;
			}
			ptr += nPitch; src += nCps;
		}
	} else {
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				code = pix->a_r_shift == 8 ? pix->Pack(src[2], src[1], src[0]) : pix->Pack(src[3], src[2], src[1], src[0]); src += cps;
				memcpy(ptr, &code, pix->cpp); ptr += pix->cpp;
			}
			ptr += nPitch; src += nCps;
		}
	}
}

// wd3d8.cpp:7152
void WDirect3D8::UpdateTextureSurfaceSampling(void* buff, int width, int height, int pitch, pix_info* pix, tagBITMAPINFO* bi, void* data, int type) {
	// Scales the bitmap to the surface by nearest-neighbour sampling; with
	// 0x800 (chroma key) pure blue pixels become transparent.
	unsigned long chromakey;
	if (type & 0x800)
		chromakey = pix->Pack(0xff, 0, 0, 0xff);

	int cps = bi->bmiHeader.biBitCount / 8;
	int bpl = (bi->bmiHeader.biWidth * cps + 3) & ~3;

	for (int j = 0; j < height; j++) {
		unsigned char* ptr = (unsigned char*)buff + pitch * j;
		int srcY = bi->bmiHeader.biHeight != height ? bi->bmiHeader.biHeight * j / height * bpl : j * bpl;

		for (int i = 0; i < width; i++) {
			int srcX = bi->bmiHeader.biWidth != width ? bi->bmiHeader.biWidth * i / width * cps : i * cps;
			unsigned char* src = (unsigned char*)data + srcY + srcX;
			unsigned long code;
			if (bi->bmiHeader.biBitCount == 8)
				src = (unsigned char*)&bi->bmiColors[*src];

			if (pix->a_r_shift == 8) {
				code = pix->Pack(src[2], src[1], src[0]);
			} else if (type & 0x800) {
				code = pix->Pack(0xff, src[2], src[1], src[0]);
				if (code == chromakey)
					code = 0;
			} else {
				code = pix->Pack(src[3], src[2], src[1], src[0]);
			}

			memcpy(ptr, &code, pix->cpp); ptr += pix->cpp;
		}
	}
}

// wd3d8.cpp:7243
void WDirect3D8::UpdateTextureSurfaceFiltering(void* buff, int w, int h, int pitch, pix_info* pix, tagBITMAPINFO* bi, unsigned char* data) {
	// Scales an 8- or 24-bit bitmap to the surface with a box filter: each
	// surface pixel averages the source area it covers, weighted by overlap.
	float scale_w = (float)bi->bmiHeader.biWidth / w;
	int spitch = (bi->bmiHeader.biBitCount / 8 * bi->bmiHeader.biWidth + 3) & ~3;

	float scale_h = (float)bi->bmiHeader.biHeight / h; float extent = scale_h * scale_w; for (int y = 0; y < h; y++) {
		unsigned char* ptr = (unsigned char*)buff + pitch * y;

		for (int x = 0; x < w; x++) {
			float v = y * scale_h;
			float v1 = v + scale_h;
			float u = x * scale_w;
			float u1 = u + scale_w;
			float fr = 0, fg = 0, fb = 0;
			float dv, du;
			for (float va = v; va < v1; va += dv) {
				dv = floor(va) + 1 - va < v1 - va ? floor(va) + 1 - va : v1 - va;
				for (float ua = u; ua < u1; ua += du) {
					du = floor(ua) + 1 - ua < u1 - ua ? floor(ua) + 1 - ua : u1 - ua;
					unsigned char* src;
					if (bi->bmiHeader.biBitCount > 8)
						src = data + (int)va * spitch + (int)ua * 3;
					else
						src = (unsigned char*)&bi->bmiColors[data[(int)va * spitch + (int)ua]];
					fr += src[2] * du * dv;
					fg += src[1] * du * dv;
					fb += src[0] * du * dv;
				}
			}
			int code = pix->Pack((unsigned char)(fr / extent), (unsigned char)(fg / extent), (unsigned char)(fb / extent));
			memcpy(ptr, &code, pix->cpp); ptr += pix->cpp;
		}
	}
}

// wd3d8.cpp:7313
void WDirect3D8::UpdateTextureSurfaceAlpha(void* buff, int pitch, pix_info* pix, const tagRECT& rc, tagBITMAPINFO* bi, void* data) {
	// Blends a 32-bit source over the surface's rectangle by the source's
	// alpha, keeping that alpha in the result.
	int nSrcBpp;
	unsigned long code;
	int nDstBpp;
	int nSrcPitch;
	unsigned char color[4];
	unsigned char p[4];

	nSrcBpp = bi->bmiHeader.biBitCount / 8;
	nSrcPitch = (bi->bmiHeader.biWidth * bi->bmiHeader.biBitCount / 8 + 3) & ~3;
	nDstBpp = pix->cpp;

	for (int y = 0; y < rc.bottom - rc.top; y++) {
		for (int x = 0; x < rc.right - rc.left; x++) {
			unsigned char* src = (unsigned char*)data + nSrcPitch * y + nSrcBpp * x;
			unsigned char* dst = (unsigned char*)buff + pitch * y + nDstBpp * x;
			unsigned char alpha = src[3];
			if (pix->cpp == 2) {
				code = MAKEWORD(dst[0], dst[1]);

				color[2] = (code >> pix->r_l_shift) << pix->r_r_shift;
				color[1] = (code >> pix->g_l_shift) << pix->g_r_shift;
				color[0] = (code >> pix->b_l_shift) << pix->b_r_shift;
			} else {
				memcpy(color, dst, 3);
			}

			for (code = 0; code < 3; code++) p[code] = (src[code] * alpha + color[code] * (255 - alpha)) / 255; code = pix->a_r_shift == 8 ? pix->Pack(p[2], p[1], p[0]) : pix->Pack(src[3], p[2], p[1], p[0]);
			memcpy(dst, &code, pix->cpp);
		}
	}
}

// wd3d8.cpp:7363
void WDirect3D8::UpdateTextureSurface(void* buff, int width, int height, int pitch, pix_info* pix, tagBITMAPINFO* bi, void* data, int type) {
	if (type & 0x20000) {
		UpdateTextureSurfaceNormal(buff, pitch, pix, bi, data);
	} else if (!(type & 0x800)
		&& ((type & 0x80000) ? (width >= bi->bmiHeader.biWidth && height >= bi->bmiHeader.biHeight)
			: (width == bi->bmiHeader.biWidth && height == bi->bmiHeader.biHeight))) {
		UpdateTextureSurfaceDirect(buff, pitch, pix, bi, data);
	} else if (!(type & 0x800) && (bi->bmiHeader.biBitCount == 8 || bi->bmiHeader.biBitCount == 24)) {
		UpdateTextureSurfaceFiltering(buff, width, height, pitch, pix, bi, (unsigned char*)data);
	} else {
		UpdateTextureSurfaceSampling(buff, width, height, pitch, pix, bi, data, type);
	}
}

// wd3d8.cpp:7400
void WDirect3D8::UpdateTextureSurface(int texHandle, tagBITMAPINFO* bi, void* data, int type) {
	d3d8_texture* tex = &m_texList[texHandle];

	if (tex) {
		if (tex->bitmap == NULL) {
			Bitmap bitmap;

			bitmap.SetBITMAPINFO(bi, (unsigned char*)data);

			tex->bitmap = new Bitmap;

			*tex->bitmap = bitmap;
		} else {
			if (bi->bmiHeader.biWidth == tex->bitmap->Width() && bi->bmiHeader.biHeight == tex->bitmap->Height()
				&& bi->bmiHeader.biBitCount == tex->bitmap->BitsPerPixel())
				memcpy(tex->bitmap->vram, data, tex->bitmap->pitch * tex->bitmap->Height());
		}

		tex->updateType = type;
		tex->needToBeFilled = true;
	}
}

// wd3d8.cpp:7473
void WDirect3D8::DestroyTexture(int texHandle) {
	if (texHandle == (int)(m_lastTexState & 0x7ff) || texHandle == (int)((m_lastTexState >> 11) & 0x7f))
		FlushRenderPrimitive();

	if (m_texList[texHandle].pSurf) { m_texList[texHandle].pSurf->Release(); m_texList[texHandle].pSurf = NULL; }
	if (m_texList[texHandle].pTex) { m_texList[texHandle].pTex->Release(); m_texList[texHandle].pTex = NULL; }

	if (m_texList[texHandle].bitmap) { delete m_texList[texHandle].bitmap; m_texList[texHandle].bitmap = NULL; }
	if (m_texList[texHandle].dxtcData) { delete[] m_texList[texHandle].dxtcData; m_texList[texHandle].dxtcData = NULL; }

	memset(&m_texList[texHandle], 0, sizeof(d3d8_texture));

	WDirect3D::DestroyTexture(texHandle);
}

// wd3d8.cpp:7492
void WDirect3D8::FixTexturePart(int texHandle, const tagRECT& rc, tagBITMAPINFO* src, void* data, int type) {
	d3d8_texture* tex = &m_texList[texHandle];

	if (tex) {
		if (tex->bitmap) {
			int cps = tex->bitmap->bi->bmiHeader.biBitCount / 8;
			unsigned char* dst = tex->bitmap->vram + rc.top * tex->bitmap->pitch + rc.left * cps;

			if (type & 0x40000) {
				// Only the alpha channel of a 32-bit source is merged in.
				if (src->bmiHeader.biBitCount == 32 && (tex->bitmap->bi->bmiHeader.biBitCount == 24 || tex->bitmap->bi->bmiHeader.biBitCount == 32)) {
					UpdateTextureSurfaceAlpha(dst, tex->bitmap->pitch, FindPixInfoByFormat(tex->bitmap->bi->bmiHeader.biBitCount == 32 ? D3DFMT_A8R8G8B8 : D3DFMT_R8G8B8), rc, src, data);
				}
			} else if (tex->bitmap->bi->bmiHeader.biBitCount == src->bmiHeader.biBitCount) {
				int srcPitch = (src->bmiHeader.biWidth * cps + 3) & ~3;

				for (int i = 0; i < rc.bottom - rc.top; i++) {
					memcpy(dst + tex->bitmap->pitch * i, data, src->bmiHeader.biWidth * cps); data = (unsigned char*)data + srcPitch;
				}
			}

			tex->updateType = (tex->updateType & 0x80000) | type;
			tex->needToBeFilled = true;
		}
	}
}

// wd3d8.cpp:7588
int WDirect3D8::Command(wVDevMessage message, int param1, int param2) {
	switch (message) {
	case W_VDEV_SETHWND:
		m_hwnd = (HWND__*)param1;
		break;
	case W_VDEV_CAPTURE_SCREEN:
		return CaptureScreen((Bitmap*)param2) ? TRUE : FALSE;
	case W_VDEV_FLUSH:
		Flush(param1);
		break;
	case W_VDEV_GET_CAPTURE_MODE:
		*(sCpatureOption::eCaptureMode*)param1 = g_captureOption.currentMode;
		break;
	case W_VDEV_SET_CAPTURE_MODE:
		g_captureOption.SetMode((sCpatureOption::eCaptureMode)param1, param2 == 1);
	case W_VDEV_RELEASE_CAPTURERESOURCE:
		ReleaseCopiedScreenResource();
		break;
	case W_VDEV_CAPTURED_BG:
		if (param1 == 1) {
			// Draw the captured screen as the background from now on.
			if (!m_useCopiedScreen)
				return BeginCapturedBackground();
		} else {
			m_useCopiedScreen = false;
		}
		break;
	case W_VDEV_SCREEN_SHOT:
		return ScreenShot((const char*)param1, param2);
	case W_VDEV_GET_SPLASH:
		return (int)new WSplashD3D(this);
	case W_VDEV_GET_COPIEDSCREENSPLASH:
		return (int)m_pCopiedScreenSplash;
	case W_VDEV_SET_DDS_RES:
		m_iDDSRes = param1 <= 0 ? 0 : (param1 >= 5 ? 5 : param1);
		m_useHiQualityTex = m_iDDSRes == 0;
		break;
	case W_VDEV_USE_MIPMAP:
		m_bUseMipmap = param1 != 0;
		break;
	case W_VDEV_MAX_MIPLVL:
		m_iMaxMipLvl = param1 > 0 ? param1 : 0;
		break;
	case W_VDEV_MIP_CREATE_FILTER:
		if (param1 >= D3DX_FILTER_NONE && param1 <= D3DX_FILTER_BOX)
			m_dwMipCreateFilter = param1;
		break;
	case W_VDEV_MIP_TEXSTAGE_FILTER:
		if (param1 >= D3DTEXF_NONE && param1 <= D3DTEXF_ANISOTROPIC) {
			m_dwMipTexStateFilter = param1;
			_SetSamplerState(0, D3DSAMP_MIPFILTER, param1);
		}
		break;
	case W_VDEV_NUM_TEXTURES: {
		int count = 0;
		for (int i = 0; i < sizeof(m_texList) / sizeof(m_texList[0]); i++) {
			if (m_texList[i].pTex)
				count++;
		}
		*(int*)param1 = count;
		break;
	}
	case W_VDEV_NUM_TNL_BUFFERS: {
		int nVb = 0;
		for (int i = 0; i < sizeof(m_xaVbList) / sizeof(m_xaVbList[0]); i++) {
			if (m_xaVbList[i].xpVb)
				nVb++;
		}
		int nIb = 0;
		for (int i = 0; i < sizeof(m_xaIbList) / sizeof(m_xaIbList[0]); i++) {
			if (m_xaIbList[i].xpIb)
				nIb++;
		}
		*(int*)param1 = nVb;
		*(int*)param2 = nIb;
		break;
	}
	case WX_VDEV_GET_STATISTICS:
		((int*)param1)[0] = m_xnTotalTris;
		((int*)param1)[1] = m_xnDPUPs;
		((int*)param1)[2] = m_xnDIPUPs;
		((int*)param1)[3] = m_xnDPs;
		((int*)param1)[4] = m_xnDIPs;
		break;
	case WX_VDEV_GET_CAPS:
		xGetCaps(param1, (unsigned long*)param2);
		break;
	case WX_VDEV_FILLMODE:
		if (m_pd3dDevice) {
			switch (param1) {
			case 0: FlushRenderPrimitive(); m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID); break;
			case 1: FlushRenderPrimitive(); m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME); break;
			case 2: FlushRenderPrimitive(); m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT); break;
			}
		}
		break;
	case WX_VDEV_DRIVER_INFO: {
		// param1: the adapter's description; param2: the driver file's WHQL
		// level and creation date (year, month, day).
		D3DADAPTER_IDENTIFIER9 identifier;
		memset(&identifier, 0, sizeof(identifier)); if (m_d3d8->GetAdapterIdentifier(m_devId, 0, &identifier) != D3D_OK) break;

		if (identifier.Description[0] == 0) break;

		strcpy((char*)param1, identifier.Description);

		char filename[260];
		for (unsigned char i = 0; i < sizeof(DriverPath) / sizeof(DriverPath[0]); i++) {
			sprintf(filename, "%s%s", DriverPath[i], identifier.Driver);
			HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) continue;

			FILETIME ftCreate, ftAccess, ftWrite, ftLocal;
			SYSTEMTIME stCreate;
			GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite);
			CloseHandle(hFile);

			FileTimeToLocalFileTime(&ftCreate, &ftLocal);
			FileTimeToSystemTime(&ftLocal, &stCreate);

			int* info = (int*)param2;
			info[0] = identifier.WHQLLevel;
			info[1] = stCreate.wYear;
			info[2] = stCreate.wMonth; info[3] = stCreate.wDay;
			break;
		}
		break;
	}
	case WX_VDEV_GET_SORTBUFFER_SIZE:
		((unsigned int*)param1)[0] = GetSortBufferSize();
		((unsigned int*)param1)[1] = GetSortBufferSwSize();
		((unsigned int*)param1)[2] = GetSortBufferHwSize();
		break;
	case WX_VDEV_GET_MERGEBUFFER_SIZE:
		((unsigned long*)param1)[0] = m_MergeBuffer.GetVertexStreamBufferSize();
		((unsigned long*)param1)[1] = m_MergeBuffer.GetIndexBufferSize();
		break;
	}

	return 0;
}

// wd3d8.cpp:7965
char* WDirect3D8::GetDeviceName(void) {
	return m_devName ? m_devName : "";
}

// wd3d8.cpp:7970
char* WDirect3D8::DuplicateString(const char* str) {
	char* dup = new char[strlen(str) + 1];
	strcpy(dup, str);
	return dup;
}

// wd3d8.cpp:7978
bool WDirect3D8::GetDevName(const _D3DDISPLAYMODE& mode, char* bufptr, int buflen) {
	int i;
	for (i = 4; i >= 0; i--) {
		if (ms_fmtList[i].format == mode.Format) break;
	}
	if (i >= 0) {
		sprintf(bufptr, "w%d h%d b%d", mode.Width, mode.Height, ms_fmtList[i].bitNum);
		return false;
	}

	return true;
}

// wd3d8.cpp:7995
char* WDirect3D8::EnumModeName(void) {
	_D3DCAPS9 caps;
	WList<char*> list(16, 16);
	int len = 0;
	char temp[128];
	_D3DDISPLAYMODE mode;
	unsigned long i;
	char* modename;
	IDirect3D9* d3d8;

	if (m_devId >= 0) {
		if (m_modList == NULL) {
			d3d8 = Direct3DCreate9(D3D_SDK_VERSION);

			if (d3d8 == NULL) {
				g_error = g_msgD3DInitFailed;
				return NULL;
			}

			if (d3d8->GetDeviceCaps(m_devId, D3DDEVTYPE_HAL, &caps) != D3D_OK) {
				d3d8->Release();
				return NULL;
			}

			// The first of these formats the adapter has modes for.
			const _D3DFORMAT _list[3] = { D3DFMT_R5G6B5, D3DFMT_X1R5G5B5, D3DFMT_X8R8G8B8 };
			_D3DFORMAT fmt = D3DFMT_X8R8G8B8;

			for (i = 0; i < sizeof(_list) / sizeof(_list[0]); i++) {
				if (m_d3d8->GetAdapterModeCount(0, _list[i])) {
					fmt = _list[i];
					break;
				}
			}

			for (i = 0; i < d3d8->GetAdapterModeCount(m_devId, fmt); i++) {
				d3d8->EnumAdapterModes(m_devId, fmt, i, &mode);

				if (GetDevName(mode, temp, sizeof(temp)))
					continue;

				if (list.Find(temp))
					continue;

				modename = DuplicateString(temp);
				list.AddItem(modename, modename);
				len += strlen(modename) + 1;
			}

			// One buffer of the names, each terminated, the last followed by
			// an empty one.
			m_modList = new char[len + 1];

			for (modename = list.Start(), len = 0; modename; modename = list.Next()) {
				strcpy(m_modList + len, modename);
				len += strlen(m_modList + len) + 1;
				delete[] modename;
			}

			m_modList[len] = 0;

			d3d8->Release();
		}

		return m_modList;
	}

	return NULL;
}

// wd3d8.cpp:8088
bool WDirect3D8::GetWindowDisplayMode(int& iWidth, int& iHeight, int& iColor) {
	_D3DDISPLAYMODE d3ddm;

	if (m_d3d8->GetAdapterDisplayMode(m_devId, &d3ddm) != D3D_OK)
		return false;

	iWidth = d3ddm.Width;
	iHeight = d3ddm.Height;

	switch (d3ddm.Format) {
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
		iColor = 32;
		break;

	case D3DFMT_R5G6B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
		iColor = 16;
		break;
	}

	return true;
}

// wd3d8.cpp:8115
bool WDirect3D8::IsSupportedDisplayMode(bool bWindowed, int iWidth, int iHeight, int iColor) {
	char modeName[64];
	char temp[64];
	_D3DDISPLAYMODE d3ddm;

	sprintf(modeName, "w%d h%d b%d", iWidth, iHeight, iColor);

	if (bWindowed) {
		if (m_d3d8->GetAdapterDisplayMode(m_devId, &d3ddm) != D3D_OK)
			return false;

		switch (d3ddm.Format) {
		case D3DFMT_A8R8G8B8:
		case D3DFMT_X8R8G8B8:
			if (iColor == 16)
				return false;
			break;

		case D3DFMT_R5G6B5:
		case D3DFMT_X1R5G5B5:
		case D3DFMT_A1R5G5B5:
			if (iColor == 32)
				return false;
			break;
		}

		return true;
	}

	_D3DFORMAT fmt = D3DFMT_X8R8G8B8;
	if (iColor == 16)
		fmt = m_d3d8->GetAdapterModeCount(m_devId, D3DFMT_R5G6B5) ? D3DFMT_R5G6B5 : D3DFMT_X1R5G5B5;

	for (unsigned long i = 0; i < m_d3d8->GetAdapterModeCount(m_devId, fmt); i++) {
		m_d3d8->EnumAdapterModes(m_devId, fmt, i, &d3ddm);

		if (GetDevName(d3ddm, temp, sizeof(temp))) continue;

		if (strcmp(modeName, temp) == 0 && (d3ddm.RefreshRate >= 60 || d3ddm.RefreshRate == 0))
			return true;
	}

	return false;
}

// wd3d8.cpp:8181
bool WDirect3D8::Reset(bool bWindowed, int iWidth, int iHeight, int iColor, long lWndStyle, int fillMode) {
	char temp[64];
	char modeName[64];
	_D3DDISPLAYMODE d3ddm;
	_D3DDISPLAYMODE t;

	if (!bWindowed)
		fillMode = 0;

	bool switchWinMode = false;
	if (m_d3dpp.Windowed != (int)bWindowed)
		switchWinMode = true;

	m_lWndStyle = lWndStyle; m_fillScrMode = fillMode > 0; SetWindowLongA(m_hwnd, GWL_STYLE, lWndStyle);
	long style = GetWindowLongA(m_hwnd, GWL_STYLE);
	SetWindowLongA(m_hwnd, GWL_STYLE, style | WS_VISIBLE);

	if (bWindowed) {
		int bpp = m_BackBufBpp;
		if (m_d3dpp.Windowed) {
			if (m_d3d8->GetAdapterDisplayMode(m_devId, &d3ddm) != D3D_OK) {
				g_error = "\xc0\xfb\xc0\xfd\xc7\xd1 \xb5\xf0\xbd\xba\xc7\xc3\xb7\xb9\xc0\xcc \xb8\xf0\xb5\xe5\xb8\xa6 \xbe\xf2\xbe\xee \xbf\xc3 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";     // no suitable display mode
				return false;
			}
			m_fmtWindowed = d3ddm.Format;
			bpp = GetBackBufferBpp(m_fmtWindowed);
		}

		if (iWidth == (int)m_d3dpp.BackBufferWidth && iHeight == (int)m_d3dpp.BackBufferHeight && m_BackBufBpp == bpp && m_d3dpp.Windowed)
			g_formatChanged = false;
		else
			g_formatChanged = true;

		m_BackBufBpp = bpp;

		m_d3dpp.Windowed = TRUE;
		m_bWindow = true;
		m_d3dpp.BackBufferWidth = iWidth;
		m_d3dpp.BackBufferHeight = iHeight;
		m_d3dpp.BackBufferFormat = m_fmtWindowed;
		m_d3dpp.SwapEffect = g_captureOption.SwapEffect;
		m_d3dpp.FullScreen_RefreshRateInHz = 0;
	} else {
		sprintf(modeName, "w%d h%d b%d", iWidth, iHeight, iColor);

		const _D3DFORMAT _list[3] = { D3DFMT_X8R8G8B8, D3DFMT_R5G6B5, D3DFMT_X1R5G5B5 };
		unsigned long m;
		int j;
		_D3DFORMAT fmt;
		for (j = 0, m = 0xffffffff; m == 0xffffffff && j < sizeof(_list) / sizeof(_list[0]); j++) {
			fmt = _list[j];
			if (m_d3d8->GetAdapterModeCount(0, fmt)) {
				for (unsigned long i = 0; i < m_d3d8->GetAdapterModeCount(m_devId, fmt); i++) {
					m_d3d8->EnumAdapterModes(m_devId, fmt, i, &d3ddm);
					if (GetDevName(d3ddm, temp, sizeof(temp))) continue;
					if (strcmp(modeName, temp) == 0 && (d3ddm.RefreshRate >= 60 || d3ddm.RefreshRate == 0)) {
						t = d3ddm;
						m = i;
						break;
					}
				}
			}
		}

		if (m == 0xffffffff) {
			g_error = "\xc0\xfb\xc0\xfd\xc7\xd1 \xb8\xae\xc7\xc1\xb7\xb9\xbd\xc3 \xb7\xb9\xc0\xcc\xc6\xae\xb8\xa6 \xbe\xf2\xbe\xee \xbf\xc3 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";     // no suitable refresh rate
			return false;
		}

		m_d3dpp.Windowed = FALSE; m_bWindow = false; m_BackBufBpp = iColor; m_fps = t.RefreshRate == 0 ? 120.0f : (float)t.RefreshRate;
		m_d3dpp.BackBufferWidth = t.Width;
		m_d3dpp.BackBufferHeight = t.Height;
		m_d3dpp.BackBufferFormat = t.Format;
		m_d3dpp.SwapEffect = g_captureOption.SwapEffect;
		m_d3dpp.FullScreen_RefreshRateInHz = t.RefreshRate;
	}

	m_d3dpp.PresentationInterval = g_captureOption.FullScreen_PresentationInterval;
	m_d3dpp.AutoDepthStencilFormat = FindDepthBufferFormat(m_d3dpp.BackBufferFormat);

	if (!xReset(switchWinMode)) {
		// The device may still be busy: wait and try once more.
		Sleep(100);

		if (!xReset(switchWinMode))
			return false;
	}

	if (bWindowed) {
		if (m_d3d8->GetAdapterDisplayMode(m_devId, &d3ddm) != D3D_OK) {
			g_error = "\xc0\xfb\xc0\xfd\xc7\xd1 \xb5\xf0\xbd\xba\xc7\xc3\xb7\xb9\xc0\xcc \xb8\xf0\xb5\xe5\xb8\xa6 \xbe\xf2\xbe\xee \xbf\xc3 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";     // no suitable display mode
			return false;
		}
		m_fps = d3ddm.RefreshRate == 0 ? 120.0f : (float)d3ddm.RefreshRate;
		m_fmtWindowed = d3ddm.Format;
	}

	if (g_captureOption.updateWholeScreen) {
		m_pd3dDevice->BeginScene();
		m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 0.0f, 0);
		m_pd3dDevice->EndScene();
		Present();
	} else {
		// Clear both buffers of the flip chain.
		for (int i = 0; i < 2; i++) {
			m_pd3dDevice->BeginScene();
			m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 0.0f, 0);
			m_pd3dDevice->EndScene();
			Present();
		}
	}

	return true;
}

// wd3d8.cpp:8398
void WDirect3D8::ReleaseCopiedScreenResource(void) {
	if (m_pCopiedScreenSurface) { m_pCopiedScreenSurface->Release(); m_pCopiedScreenSurface = NULL; }

	if (m_hCopiedScreenTexture > 0) {
		DestroyTexture(m_hCopiedScreenTexture);
		m_hCopiedScreenTexture = 0;
	}

	m_useCopiedScreen = false;
}

// wd3d8.cpp:8411
bool WDirect3D8::BeginRenderToTexture(const WRenderToTextureParam& param) {
	if (m_devState == 1)
		return false;

	// The first render target decides the size of the viewport and of the
	// shared depth surface.
	if (param.m_rtTexInfo[0].m_hTex <= 0)
		return false;

	const d3d8_texture& tex0 = m_texList[param.m_rtTexInfo[0].m_hTex];

	if (tex0.pSurf == NULL)
		return false;

	// Targets with different clear values are cleared one by one.
	if (!param.CanClearAtOnce()) {
		for (int i = 0; i < WRenderToTextureParam::NUM_SIMULTANEOUSRTS; i++) {
			if (param.m_rtTexInfo[i].m_hTex <= 0)
				continue;

			if (!param.m_rtTexInfo[i].m_needToClear)
				continue;

			m_pd3dDevice->SetRenderTarget(0, m_texList[param.m_rtTexInfo[i].m_hTex].pSurf);
			m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, param.m_rtTexInfo[i].m_clearClr, 1.0f, 0);
		}
	}

	IDirect3DSurface9* pDepth = NULL;

	switch (param.m_depthSurfInfo.m_surfUsage) {
	case WRenderToTextureParam::DepthSurfInfo::USE_MAIN_SURFACE:
		pDepth = m_mainRt.m_depthSurf;
		break;

	case WRenderToTextureParam::DepthSurfInfo::USE_SHARED_SURFACE:
		pDepth = FindDepthSurf((unsigned short)tex0.width, (unsigned short)tex0.height);
		break;

	case WRenderToTextureParam::DepthSurfInfo::USE_EXCLUSIVE_SURFACE:
		pDepth = m_texList[param.m_depthSurfInfo.m_hTex].pSurf;
		break;
	}

	for (int i = 0; i < WRenderToTextureParam::NUM_SIMULTANEOUSRTS; i++) {
		IDirect3DSurface9* pSurf;

		if (param.m_rtTexInfo[i].m_hTex > 0)
			pSurf = m_texList[param.m_rtTexInfo[i].m_hTex].pSurf;
		else
			pSurf = NULL;

		if (pSurf != m_curRt.m_rt.m_surf[i])
			m_pd3dDevice->SetRenderTarget(i, pSurf);
	}

	if (pDepth != m_curRt.m_depthSurf)
		m_pd3dDevice->SetDepthStencilSurface(pDepth);

	_D3DVIEWPORT9 viewport = { 0, 0, tex0.width, tex0.height, 0.0f, 1.0f };

	if (memcmp(&viewport, &m_curRt.m_viewport, sizeof(viewport)) != 0)
		m_pd3dDevice->SetViewport(&viewport);

	unsigned long clearFlags = 0;

	if (param.CanClearAtOnce()) {
		if (param.m_rtTexInfo[0].m_needToClear)
			clearFlags = D3DCLEAR_TARGET;
	}

	if (pDepth && param.m_depthSurfInfo.m_needToClear)
		clearFlags |= D3DCLEAR_ZBUFFER;

	if (clearFlags)
		m_pd3dDevice->Clear(0, NULL, clearFlags, param.m_rtTexInfo[0].m_clearClr, param.m_depthSurfInfo.m_clearZ, 0);

	BackupRenderTarget(param, pDepth, viewport);

	return true;
}

// wd3d8.cpp:8520
void WDirect3D8::EndRenderToTexture(const WRenderToTextureParam& param) {
	if (m_devState == 1) return;

	int hTex = param.m_rtTexInfo[0].m_hTex;
	if (hTex <= 0) return;

	d3d8_texture& tex = m_texList[hTex];

	if (tex.pSurf == NULL) return;

	RestoreBackedupRenderTarget();

	for (int i = 0; i < WRenderToTextureParam::NUM_SIMULTANEOUSRTS; i++) {
		if (param.m_rtTexInfo[i].m_hTex > 0) {
			m_texList[param.m_rtTexInfo[i].m_hTex].isFilled = true;
		}
	}
}

// wd3d8.cpp:8560
IDirect3DSurface9* WDirect3D8::FindDepthSurf(unsigned short w, unsigned short h) {
	int i;

	for (i = 0; i < 32; i++) {
		if (m_depthSurfList[i].pDepth == NULL)
			break;
		if (w == m_depthSurfList[i].wWidth && h == m_depthSurfList[i].wHeight)
			return m_depthSurfList[i].pDepth;
	}

	for (; i < 32; i++) {
		if (m_depthSurfList[i].pDepth == NULL) {
			m_depthSurfList[i].wWidth = w;
			m_depthSurfList[i].wHeight = h;
			m_pd3dDevice->CreateDepthStencilSurface(w, h, ms_RtDepthFmt[m_RtDepthFmtIdx].fmt, D3DMULTISAMPLE_NONE, 0, TRUE, &m_depthSurfList[i].pDepth, NULL);
			return m_depthSurfList[i].pDepth;
		}
	}

	if (m_commonDepthSurf)
		m_commonDepthSurf->Release();

	m_pd3dDevice->CreateDepthStencilSurface(w, h, ms_RtDepthFmt[m_RtDepthFmtIdx].fmt, D3DMULTISAMPLE_NONE, 0, TRUE, &m_commonDepthSurf, NULL);
	return m_commonDepthSurf;
}

// The full-screen rectangle BeginCapturedBackground copies (a file-local of
// the DLL, 0x100440b4).
static tagRECT rc;

// wd3d8.cpp:8625
bool WDirect3D8::BeginCapturedBackground(void) {
	FlushRenderPrimitive();

	IDirect3DSurface9* pRenderTarget;
	if (m_pd3dDevice->GetRenderTarget(0, &pRenderTarget) == D3D_OK) {
		if (pRenderTarget->GetDesc(&m_capturedDdsd) == D3D_OK) {
			bool ready = true;

			if (g_captureOption.useTexture) {
				if (m_hCopiedScreenTexture == 0) {
					pix_info* pix = FindPixInfoByFormat(m_capturedDdsd.Format);

					ready = false;
					if (pix != NULL) {
						tagBITMAPINFO bi;
						memset(&bi, 0, sizeof(bi));
						bi.bmiHeader.biWidth = m_capturedDdsd.Width;
						bi.bmiHeader.biHeight = m_capturedDdsd.Height;
						bi.bmiHeader.biBitCount = (WORD)(pix->cpp * 8);

						m_hCopiedScreenTexture = CreateTexture(&bi, 0x4000);

						if (m_hCopiedScreenTexture > 0) {
							if (m_texList[m_hCopiedScreenTexture].pTex != NULL)
								ready = true;
							else
								DestroyTexture(m_hCopiedScreenTexture);
						}
					}
				}
			} else {
				if (m_pCopiedScreenSurface == NULL) {
					HRESULT hr;
					if (!g_captureOption.useMemCopy && g_captureOption.dxCopyRectsOK)
						hr = m_pd3dDevice->CreateRenderTarget(m_capturedDdsd.Width, m_capturedDdsd.Height, m_capturedDdsd.Format, D3DMULTISAMPLE_NONE, 0, FALSE, &m_pCopiedScreenSurface, NULL);
					else
						hr = fnWD3DDevice_CreateSurface(m_pd3dDevice, m_capturedDdsd.Width, m_capturedDdsd.Height, m_capturedDdsd.Format, D3DPOOL_DEFAULT, &m_pCopiedScreenSurface, NULL);

					ready = SUCCEEDED(hr);
				}
			}

			if (ready) {
				m_useCopiedScreen = true;

				IDirect3DSurface9* pSurf = NULL;
				if (g_captureOption.useTexture)
					m_texList[m_hCopiedScreenTexture].pTex->GetSurfaceLevel(0, &pSurf);
				else
					pSurf = m_pCopiedScreenSurface;

				bool copied = false;
				if (!g_captureOption.useMemCopy && g_captureOption.dxCopyRectsOK) {
					rc.left = 0;
					rc.top = 0;
					rc.right = m_capturedDdsd.Width;
					rc.bottom = m_capturedDdsd.Height;

					if (fnWD3DDevice_CopyRect(m_pd3dDevice, pRenderTarget, &rc, pSurf) == D3D_OK)
						copied = true;
					else
						g_captureOption.dxCopyRectsOK = false;
				}

				if (!copied) {
					_D3DLOCKED_RECT lrSrc;
					_D3DLOCKED_RECT lrDst;

					pRenderTarget->LockRect(&lrSrc, NULL, D3DLOCK_READONLY | D3DLOCK_NOSYSLOCK | D3DLOCK_NO_DIRTY_UPDATE);
					pSurf->LockRect(&lrDst, NULL, 0);
					int smallerPitch = Min(lrSrc.Pitch, lrDst.Pitch);

					for (int i = 0; i < (int)m_capturedDdsd.Height; i++)
						memcpy((unsigned char*)lrDst.pBits + i * lrDst.Pitch, (unsigned char*)lrSrc.pBits + i * lrSrc.Pitch, smallerPitch);

					pRenderTarget->UnlockRect();
					pSurf->UnlockRect();
				}

				pRenderTarget->Release();

				if (g_captureOption.useTexture)
					pSurf->Release();

				return true;
			}
		}

		pRenderTarget->Release();
	}

	return false;
}

// wd3d8.cpp:8813
unsigned long* WDirect3D8::GetZBufferHistogram(void) {
	// Counts the pixels of each depth (top 8 bits of a lockable 16-bit
	// depth buffer); NULL when the depth buffer cannot be read.
	bool ok = false;
	IDirect3DSurface9* pDepthTarget;
	if (m_pd3dDevice->GetDepthStencilSurface(&pDepthTarget) == D3D_OK) {
		_D3DSURFACE_DESC ddsd;
		if (pDepthTarget->GetDesc(&ddsd) == D3D_OK) {
			if (ddsd.Format == D3DFMT_D16_LOCKABLE) {
				_D3DLOCKED_RECT LockedRect;
				if (pDepthTarget->LockRect(&LockedRect, NULL, D3DLOCK_READONLY) == D3D_OK) {
					memset(ms_ZbuffHistogram, 0, sizeof(ms_ZbuffHistogram));

					for (unsigned int y = 0; y < m_d3dpp.BackBufferHeight; y++) {
						unsigned short* p = (unsigned short*)((unsigned char*)LockedRect.pBits + LockedRect.Pitch * y);
						for (unsigned int x = 0; x < m_d3dpp.BackBufferWidth; x++) {
							unsigned short z = *p++;
							z >>= 8;
							ms_ZbuffHistogram[z]++;
						}
					}
					pDepthTarget->UnlockRect();
					ok = true;
				}
			}
		}
		pDepthTarget->Release();
	}

	return ok ? ms_ZbuffHistogram : NULL;
}

// wd3d8.cpp:8860
HRESULT WDirect3D8::ScreenShot(const char* pDestFilename, int format) {
	HRESULT hr;
	IDirect3DSurface9* surf;
	_D3DDISPLAYMODE mode;

	hr = m_pd3dDevice->GetDisplayMode(0, &mode);
	if (FAILED(hr)) return hr;

	hr = m_pd3dDevice->CreateOffscreenPlainSurface(mode.Width, mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &surf, NULL);
	if (FAILED(hr)) return hr;

	hr = m_pd3dDevice->GetFrontBufferData(0, surf);
	if (FAILED(hr)) {
		surf->Release();
		return hr;
	}

	hr = D3DXSaveSurfaceToFileA(pDestFilename, (D3DXIMAGE_FILEFORMAT)format, surf, NULL, NULL);

	surf->Release();

	return hr;
}

// wd3d8.cpp:8904
bool WDirect3D8::CaptureScreen(Bitmap* bitmap) {
	// Copies the render target into a 24-bit bitmap (16- or 32-bit targets).
	_D3DSURFACE_DESC ddsd;
	unsigned char r, g, b;
	IDirect3DSurface9* pRenderTarget;
	_D3DLOCKED_RECT LockedRect;
	bool result = false;
	if (m_pd3dDevice->GetRenderTarget(0, &pRenderTarget) == D3D_OK) {
		if (pRenderTarget->GetDesc(&ddsd) == D3D_OK) {
			pix_info* pix = FindPixInfoByFormat(ddsd.Format);

			if (pix) {
				if (pRenderTarget->LockRect(&LockedRect, NULL, D3DLOCK_READONLY) == D3D_OK) {
					if (pix->cpp == 2) {
						for (int y = 0; y < bitmap->bi->bmiHeader.biHeight; y++) {
							for (int x = 0; x < bitmap->bi->bmiHeader.biWidth; x++) {
								unsigned long code = *(unsigned short*)((unsigned char*)LockedRect.pBits + LockedRect.Pitch * y + pix->cpp * x);

								g = (unsigned char)((code >> pix->g_l_shift) << pix->g_r_shift);
								r = (unsigned char)((code >> pix->r_l_shift) << pix->r_r_shift);
								unsigned char* dst = &bitmap->vram[bitmap->pitch * y + x * 3];
								b = (unsigned char)((code >> pix->b_l_shift) << pix->b_r_shift);
								dst[0] = b;
								dst[1] = g; dst[2] = r;
							}
						}
					} else if (pix->cpp == 4) {
						for (int y = 0; y < bitmap->bi->bmiHeader.biHeight; y++) {
							for (int x = 0; x < bitmap->bi->bmiHeader.biWidth; x++) {
								unsigned long code = *(unsigned long*)((unsigned char*)LockedRect.pBits + LockedRect.Pitch * y + pix->cpp * x);

								g = (unsigned char)(code >> pix->g_l_shift);
								r = (unsigned char)(code >> pix->r_l_shift);
								unsigned char* dst = &bitmap->vram[bitmap->pitch * y + x * 3];
								b = (unsigned char)(code >> pix->b_l_shift);
								dst[0] = b;
								dst[1] = g; dst[2] = r;
							}
						}
					}

					pRenderTarget->UnlockRect();
					result = true;
				}
			}
		}
		pRenderTarget->Release();
	}

	return result;
}

// wd3d8.cpp:9006
void WDirect3D8::xGetCaps(unsigned long iItem, unsigned long* pdwCaps) {
	switch (iItem) {
	case 0:     // hardware transform and lighting
		if ((m_d3dcaps.DevCaps & D3DDEVCAPS_HWRASTERIZATION) && (m_d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT))
			*pdwCaps = 1;
		else
			*pdwCaps = 0;
		break;
	case 1:
		*pdwCaps = 1;
		break;
	case 2:
		*pdwCaps = (m_d3dcaps.Caps2 & D3DCAPS2_DYNAMICTEXTURES) ? 1 : 0;
		break;
	case 3:
		pdwCaps[0] = m_d3dcaps.MaxTextureWidth;
		pdwCaps[1] = m_d3dcaps.MaxTextureHeight;
		break;
	case 4:
		*pdwCaps = m_d3dcaps.MaxTextureBlendStages;
		break;
	case 5:
		*pdwCaps = m_d3dcaps.MaxSimultaneousTextures;
		break;
	case 6:
		*pdwCaps = m_d3dcaps.MaxVertexBlendMatrices;
		break;
	case 7:
		*pdwCaps = m_d3dcaps.MaxVertexBlendMatrixIndex;
		break;
	case 8:
		*(float*)pdwCaps = m_d3dcaps.MaxPointSize;
		break;
	case 9:
		*pdwCaps = m_d3dcaps.VertexShaderVersion;
		break;
	case 10:
		*pdwCaps = m_d3dcaps.PixelShaderVersion;
		break;
	}
}

// wd3d8.cpp:9068
unsigned long WDirect3D8::xDetermineFVF(int iDrawFlag, int iDrawFlag2, int iMaxBoneNum) {
	unsigned long fvf;

	if (iDrawFlag2 & 0x1000) {
		switch (iMaxBoneNum) {
		case 1:
			fvf = D3DFVF_XYZB1 | D3DFVF_LASTBETA_UBYTE4;
			break;
		case 2:
			fvf = D3DFVF_XYZB2 | D3DFVF_LASTBETA_UBYTE4;
			break;
		case 3:
			fvf = D3DFVF_XYZB3 | D3DFVF_LASTBETA_UBYTE4;
			break;
		case 4:
			fvf = D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4;
			break;
		default:
			fvf = D3DFVF_XYZ;
			break;
		}
	} else {
		fvf = D3DFVF_XYZ;
	}

	if (!(iDrawFlag2 & 0x14))
		fvf |= D3DFVF_NORMAL;

	if (!(iDrawFlag & 0x40000000) && !(iDrawFlag2 & 0x2000))
		fvf |= D3DFVF_DIFFUSE;

	if (iDrawFlag & 0x3f800)
		fvf |= D3DFVF_TEX2;
	else
		fvf |= D3DFVF_TEX1;

	return fvf;
}

// wd3d8.cpp:9119
unsigned long WDirect3D8::xGetFVF(int hVb) {
	return m_xaVbList[hVb].xdwFVF;
}

// wd3d8.cpp:9124
unsigned char WDirect3D8::xGetStride(int hVb) {
	return m_xaVbList[hVb].xbStride;
}

// wd3d8.cpp:9163
bool WDirect3D8::xHasVertexElem(unsigned long dwFVF, wWxVertexElem elem) const {
	switch (elem) {
	case WX_VELEM_POSITION:
		return (dwFVF & D3DFVF_POSITION_MASK) ? true : false;

	case WX_VELEM_NORMAL:
		return (dwFVF & D3DFVF_NORMAL) ? true : false;

	case WX_VELEM_DIFFUSE:
		return (dwFVF & D3DFVF_DIFFUSE) ? true : false;

	case WX_VELEM_TEX1:
		return (dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX1 ? true : false;

	case WX_VELEM_TEX2:
		return (dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX2 ? true : false;
	}

	return false;
}

// wd3d8.cpp:9188
int WDirect3D8::xGetVertexElemOffset(unsigned long dwFVF, wWxVertexElem elem) const {
	if (elem == WX_VELEM_POSITION)
		return 0;

	unsigned char offset = 0;
	switch (dwFVF & D3DFVF_POSITION_MASK) {
	case D3DFVF_XYZ:
		offset = 12;
		break;
	case D3DFVF_XYZRHW:
		offset = 16;
		break;
	case D3DFVF_XYZB1:
		offset = 16;
		break;
	case D3DFVF_XYZB2:
		offset = 20;
		break;
	case D3DFVF_XYZB3:
		offset = 24;
		break;
	case D3DFVF_XYZB4:
		offset = 28;
		break;
	}

	if (elem == WX_VELEM_NORMAL) {
		if (dwFVF & D3DFVF_NORMAL)
			return offset;
	} else {
		if (dwFVF & D3DFVF_NORMAL)
			offset += 12;

		if (elem == WX_VELEM_DIFFUSE) {
			if (dwFVF & D3DFVF_DIFFUSE)
				return offset;
		} else {
			if (dwFVF & D3DFVF_DIFFUSE)
				offset += 4;

			if (elem == WX_VELEM_TEX1) {
				if ((dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX1)
					return offset;
			} else {
				unsigned long texCount = (dwFVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
				if (texCount >= 1)
					offset += 8;

				if (elem == WX_VELEM_TEX2) {
					if (texCount >= 2)
						return offset;
				}
			}
		}
	}

	return -1;
}

// wd3d8.cpp:9275
int WDirect3D8::xGetBlendWeightSize(unsigned long dwFVF) const {
	int size = 0;

	switch (dwFVF & D3DFVF_POSITION_MASK) {
	case D3DFVF_XYZ:
	case D3DFVF_XYZRHW:
		size = 0;
		break;
	case D3DFVF_XYZB1:
		size = 4;
		break;
	case D3DFVF_XYZB2:
		size = 8;
		break;
	case D3DFVF_XYZB3:
		size = 12;
		break;
	case D3DFVF_XYZB4:
		size = 16;
		break;
	}

	if (dwFVF & D3DFVF_LASTBETA_UBYTE4)
		size -= 4;

	return size;
}

// wd3d8.cpp:9314
void WDirect3D8::xCreateVertexBuffer(int hVb, int nVtxs, unsigned long dwFVF, unsigned long dwUsage) {
	unsigned long stride = VertexSize(dwFVF);
	m_xaVbList[hVb].xbStride = (unsigned char)stride;

	m_xaVbList[hVb].xdwFVF = dwFVF;
	m_xaVbList[hVb].xdwUsage = dwUsage;

	m_xaVbList[hVb].xnVtxs = nVtxs;
	m_xaVbList[hVb].xbNeedToBeFilled = true;

	m_xaVbList[hVb].xpVertexData = new unsigned char[stride * nVtxs];
	m_xaVbList[hVb].xpVb = NULL;
}

// wd3d8.cpp:9354
void WDirect3D8::xCreateIndexBuffer(int hIb, int nIdxs, unsigned long dwUsage) {
	m_xaIbList[hIb].xdwUsage = dwUsage;
	m_xaIbList[hIb].xnIdxs = nIdxs;
	m_xaIbList[hIb].xbNeedToBeFilled = true;

	m_xaIbList[hIb].xpIndexData = new unsigned short[nIdxs];
	m_xaIbList[hIb].xpIb = NULL;
}

// wd3d8.cpp:9386
unsigned long WDirect3D8::xDetermineBufferUsage(unsigned long dwFVF) {
	if (m_xdwDevBehavior & D3DCREATE_MIXED_VERTEXPROCESSING) {
		if (dwFVF & D3DFVF_LASTBETA_UBYTE4) {
			if (m_d3dcaps.MaxVertexBlendMatrices < 4 || m_d3dcaps.MaxVertexBlendMatrixIndex < 128)
				return D3DUSAGE_SOFTWAREPROCESSING;
		}
	}
	return 0;
}

// wd3d8.cpp:9407
void WDirect3D8::xReleaseVertexBuffer(int hVb) {
	if (hVb > 0) {
		if (hVb == m_xhLastVb)
			FlushRenderPrimitive();

		m_xiVbSize -= m_xaVbList[hVb].xbStride * m_xaVbList[hVb].xnVtxs;

		if (m_xaVbList[hVb].xpVertexData) { delete[] m_xaVbList[hVb].xpVertexData; m_xaVbList[hVb].xpVertexData = NULL; }

		if (m_xaVbList[hVb].xpVb) { m_xaVbList[hVb].xpVb->Release(); m_xaVbList[hVb].xpVb = NULL; }

		memset(&m_xaVbList[hVb], 0, sizeof(sVb8));

		WDirect3D::xReleaseVertexBuffer(hVb);
	}
}

// wd3d8.cpp:9432
void WDirect3D8::xReleaseIndexBuffer(int hIb) {
	if (hIb > 0) {
		if (hIb == m_xhLastIb)
			FlushRenderPrimitive();

		if (m_xaIbList[hIb].xpIndexData) { delete[] m_xaIbList[hIb].xpIndexData; m_xaIbList[hIb].xpIndexData = NULL; }

		if (m_xaIbList[hIb].xpIb) { m_xaIbList[hIb].xpIb->Release(); m_xaIbList[hIb].xpIb = NULL; }

		memset(&m_xaIbList[hIb].xpIb, 0, sizeof(m_xaIbList[hIb].xpIb));

		WDirect3D::xReleaseIndexBuffer(hIb);
	}
}

// wd3d8.cpp:9458
unsigned char* WDirect3D8::xLockVertexBuffer(int hVb, unsigned int uiOffset, unsigned int uiSize) {
	m_xaVbList[hVb].xbNeedToBeFilled = true;

	return m_xaVbList[hVb].xpVertexData + uiOffset;
}

// wd3d8.cpp:9480
void WDirect3D8::xUnlockVertexBuffer(int hVb) {
}

// wd3d8.cpp:9492
unsigned char* WDirect3D8::xLockIndexBuffer(int hIb, unsigned int uiOffset, unsigned int uiSize) {
	m_xaIbList[hIb].xbNeedToBeFilled = true;

	return (unsigned char*)m_xaIbList[hIb].xpIndexData + uiOffset;
}

// wd3d8.cpp:9514
void WDirect3D8::xUnlockIndexBuffer(int hIb) {
}

// wd3d8.cpp:9522
void WDirect3D8::xSetTransform(WVDTRANSFORMSTATETYPE state, const WMatrix4& matrix) {
	_SetTransform((_D3DTRANSFORMSTATETYPE)state, (const D3DXMATRIX&)matrix);
}

// wd3d8.cpp:9528
void WDirect3D8::xSetPrevViewTransform(const WMatrix4& Matrix) {
	m_xPrevViewMatrix = (const D3DXMATRIX&)Matrix;
}

// wd3d8.cpp:9534
void WDirect3D8::xSetLight(unsigned long Index, const LightSet& wLight) {
	if (wLight.type == LightSet::W_LIGHT_NOT_INITIALIZED) return;

	if (wLight.type == m_xaWLights[Index].type && wLight.nearone.x == m_xaWLights[Index].nearone.x && wLight.nearone.y == m_xaWLights[Index].nearone.y && wLight.nearone.z == m_xaWLights[Index].nearone.z && wLight.diffuse == m_xaWLights[Index].diffuse && wLight.ambient == m_xaWLights[Index].ambient) return;

	FlushRenderPrimitive();

	m_xaWLights[Index].type = wLight.type;
	m_xaWLights[Index].nearone.x = wLight.nearone.x;
	m_xaWLights[Index].nearone.y = wLight.nearone.y;
	m_xaWLights[Index].nearone.z = wLight.nearone.z;
	m_xaWLights[Index].diffuse = wLight.diffuse;
	m_xaWLights[Index].ambient = wLight.ambient;

	m_xaLights[Index].Type = wLight.type == LightSet::W_LIGHT_DIRECTIONAL ? D3DLIGHT_DIRECTIONAL : D3DLIGHT_POINT;

	m_xaLights[Index].Ambient.r = ((wLight.ambient >> 16) & 0xff) * (1.0f / 255.0f);
	m_xaLights[Index].Ambient.g = ((wLight.ambient >> 8) & 0xff) * (1.0f / 255.0f);
	m_xaLights[Index].Ambient.b = (wLight.ambient & 0xff) * (1.0f / 255.0f);
	m_xaLights[Index].Diffuse.r = ((wLight.diffuse >> 16) & 0xff) * (1.0f / 255.0f);
	m_xaLights[Index].Diffuse.g = ((wLight.diffuse >> 8) & 0xff) * (1.0f / 255.0f);
	m_xaLights[Index].Diffuse.b = (wLight.diffuse & 0xff) * (1.0f / 255.0f);

	if (wLight.type == LightSet::W_LIGHT_DIRECTIONAL) {
		m_xaLights[Index].Direction.x = wLight.nearone.x;
		m_xaLights[Index].Direction.y = wLight.nearone.y;
		m_xaLights[Index].Direction.z = wLight.nearone.z;
	} else {
		m_xaLights[Index].Position.x = wLight.nearone.x;
		m_xaLights[Index].Position.y = wLight.nearone.y;
		m_xaLights[Index].Position.z = wLight.nearone.z;
	}

	if (Index == 0)
		m_LightDirect = m_xaLights[0].Direction;

	FlushRenderPrimitive();
	m_pd3dDevice->SetLight(Index, &m_xaLights[Index]);
}

// wd3d8.cpp:9593
void WDirect3D8::xDrawIndexedPrimitive(const WxViewState& VState, const WxBatchState& BState) {
	static _D3DPRIMITIVETYPE primType;
	static unsigned int primCount;

	if (m_devState == 1) return;

	if (!(BState.xiFlag1 & 0x14)) {
		xSetLight(0, VState.xLight);
	}
	_SetTransform(D3DTS_VIEW, (const D3DXMATRIX&)VState.xmView);
	_SetTransform(D3DTS_PROJECTION, (const D3DXMATRIX&)VState.xmProj);

	D3DXMATRIX m;
	if (BState.xnmTransfs) {
		for (int i = 0; i < BState.xnmTransfs; i++) {
			SetD3DMATRIXFromWMatrix(m, *BState.xpapmW[i]);

			if (BState.xpapmO) {
				D3DXMATRIX mInv;
				SetD3DMATRIXFromWMatrix(mInv, *BState.xpapmO[i]);
				D3DXMatrixInverse(&mInv, NULL, &mInv);

				m = mInv * m;
			}

			_SetTransform(D3DTS_WORLDMATRIX(i), m);
		}
	} else {
		SetD3DMATRIXFromWMatrix(m, BState.xmW);

		_SetTransform(D3DTS_WORLD, m);
	}

	SetVtxType(m_fog ? BState.xiFlag0 : BState.xiFlag0 & ~0x08000000, BState.xiFlag1, xGetFVF((unsigned int)BState.xiFlag1 >> 22), BState.xdwDiffuse);

	switch (BState.xiFlag1 & 3) {
	case 0:
		primType = D3DPT_TRIANGLELIST;
		primCount = BState.xnIdxs / 3;
		break;
	case 1:
		primType = D3DPT_TRIANGLESTRIP;
		primCount = BState.xnIdxs - 2;
		break;
	case 2:
		primType = D3DPT_POINTLIST;
		primCount = BState.xnIdxs;
		break;
	}

	_DrawIndexedPrimitive(primType, BState.xiBaseVtxIdx, BState.xnVtxs, BState.xiBaseIdxIdx, primCount);
}

// wd3d8.cpp:9666
void WDirect3D8::SetShaderSource(const char* src) {
	ReloadShader();

	if (src)
		m_shadersrc = src;
}

// wd3d8.cpp:9675
void WDirect3D8::xSetTnLBuffer(int hVb, int hIb) {
	if (hVb == 0 || hIb == 0) {
		m_xhLastVb = hVb;
		m_xhLastIb = hIb;
		return;
	}

	bool bChanged = false;

	if (m_xhLastVb != hVb) {
		if (m_xdwDevBehavior & D3DCREATE_MIXED_VERTEXPROCESSING) {
			unsigned long dwChanged = m_xdwLastUsage ^ m_xaVbList[hVb].xdwUsage;

			if (dwChanged) {
				m_xdwLastUsage = m_xaVbList[hVb].xdwUsage;

				if (dwChanged & D3DUSAGE_SOFTWAREPROCESSING) {
					m_pd3dDevice->SetSoftwareVertexProcessing((m_xaVbList[hVb].xdwUsage & D3DUSAGE_SOFTWAREPROCESSING) != 0);

					bChanged = true;
				}
			}
		}

		if (m_xaVbList[hVb].xpVb == NULL)
			xInstantiateVertexBuffer(hVb);

		if (m_xaVbList[hVb].xbNeedToBeFilled)
			xFillVertexBuffer(hVb);

		m_pd3dDevice->SetStreamSource(0, m_xaVbList[hVb].xpVb, 0, m_xaVbList[hVb].xbStride);

		m_xhLastVb = hVb;
	}

	if (m_xhLastIb != hIb || bChanged) {
		if (m_xaIbList[hIb].xpIb == NULL)
			xInstantiateIndexBuffer(hIb);

		if (m_xaIbList[hIb].xbNeedToBeFilled)
			xFillIndexBuffer(hIb);

		m_pd3dDevice->SetIndices(m_xaIbList[hIb].xpIb);

		m_xhLastIb = hIb;
	}
}

// wd3d8.cpp:9789
void WDirect3D8::xSetRenderState(unsigned long dwFlag, unsigned long dwDiffuse) {
	if (!m_xbCurHwTnL) return;

	if (m_xbUseTFactor && dwDiffuse != m_xdwTFactor) {
		m_xdwTFactor = dwDiffuse;
		_SetRenderState(D3DRS_TEXTUREFACTOR, dwDiffuse);
	} else if (dwDiffuse != m_xdwDiffuse) {
		FlushRenderPrimitive();

		m_xdwDiffuse = dwDiffuse;
		m_xMaterial.Diffuse.a = (dwDiffuse >> 24) * (1.0f / 255.0f);
		m_xMaterial.Diffuse.r = ((dwDiffuse >> 16) & 0xff) * (1.0f / 255.0f);
		m_xMaterial.Diffuse.g = ((dwDiffuse >> 8) & 0xff) * (1.0f / 255.0f);
		m_xMaterial.Diffuse.b = (dwDiffuse & 0xff) * (1.0f / 255.0f);

		m_pd3dDevice->SetMaterial(&m_xMaterial);
	}
}

// wd3d8.cpp:9923
void WDirect3D8::BeginUsingCustomRenderState(void) {
	FlushRenderPrimitive();

	if (m_CustomRenderState.HasAnyState()) {
		m_CustomRenderStateBackupList.push(m_CustomRenderState);

		m_CustomRenderState.Clear();
	}
}

// wd3d8.cpp:9941
void WDirect3D8::SetCustomRenderState(WVDRENDERSTATETYPE State, unsigned long Value) {
	m_CustomRenderState.SetRenderState((_D3DRENDERSTATETYPE)State, Value);
}

// wd3d8.cpp:9946
void WDirect3D8::SetCustomTextureStageState(unsigned long Stage, WVDTEXTURESTAGESTATETYPE Type, unsigned long Value) {
	m_CustomRenderState.SetTextureStageState(Stage, (_D3DTEXTURESTAGESTATETYPE)Type, Value);
}

// wd3d8.cpp:9951
void WDirect3D8::SetCustomTransform(WVDTRANSFORMSTATETYPE State, const WMatrix4& Matrix) {
	m_CustomRenderState.SetTransform((_D3DTRANSFORMSTATETYPE)State, (const D3DXMATRIX&)Matrix);
}

// wd3d8.cpp:9956
void WDirect3D8::SetCustomTexture(unsigned long Stage, int hTex) {
	if (hTex < 0)
		return;

	xInstantiateAndFillTexture(hTex);

	m_CustomRenderState.SetTexture(Stage, hTex > 0 ? m_texList[hTex].pTex : NULL);
}

// wd3d8.cpp:9971
void WDirect3D8::SetCustomClipPlane(unsigned long Index, const WPlane& Plane) {
	m_CustomRenderState.SetClipPlane(Index, (const D3DXPLANE&)Plane);
}

// wd3d8.cpp:9977
void WDirect3D8::SetCustomSamplerState(unsigned long Sampler, WVDSAMPLERSTATETYPE Type, unsigned long Value) {
	m_CustomRenderState.SetSamplerState(Sampler, (_D3DSAMPLERSTATETYPE)Type, Value);
}

// wd3d8.cpp:9982
void WDirect3D8::SetCustomFxMacro(unsigned long FxMacro) {
	m_CustomRenderState.SetFxMacro(FxMacro);
}

// wd3d8.cpp:9988
void WDirect3D8::SetCustomFxParamInt(WVDFXPARAMETERTYPE ParamType, int Value) {
	m_CustomRenderState.SetFxParamInt(ParamType, Value);
}

// wd3d8.cpp:9993
void WDirect3D8::SetCustomFxParamVector2(WVDFXPARAMETERTYPE ParamType, const WVector2D& Value) {
	m_CustomRenderState.SetFxParamVector2(ParamType, (const D3DXVECTOR2&)Value);
}

// wd3d8.cpp:9998
void WDirect3D8::SetCustomFxParamVector3(WVDFXPARAMETERTYPE ParamType, const WVector& Value) {
	m_CustomRenderState.SetFxParamVector3(ParamType, (const D3DXVECTOR3&)Value);
}

// wd3d8.cpp:10003
void WDirect3D8::SetCustomFxParamVector4(WVDFXPARAMETERTYPE ParamType, const WVector4& Value) {
	m_CustomRenderState.SetFxParamVector4(ParamType, (const D3DXVECTOR4&)Value);
}

// wd3d8.cpp:10008
void WDirect3D8::SetCustomFxParamMatrix(WVDFXPARAMETERTYPE ParamType, const WMatrix4& Value) {
	m_CustomRenderState.SetFxParamMatrix(ParamType, (const D3DXMATRIX&)Value);
}

// wd3d8.cpp:10013
void WDirect3D8::SetCustomFxParamTexture(WVDFXPARAMETERTYPE ParamType, int hTex) {
	if (hTex < 0)
		return;

	xInstantiateAndFillTexture(hTex);

	m_CustomRenderState.SetFxParamTexture(ParamType, hTex > 0 ? m_texList[hTex].pTex : NULL);
}

// wd3d8.cpp:10029
void WDirect3D8::EndUsingCustomRenderState(void) {
	if (m_CustomRenderState.HasAnyState()) {
		FlushRenderPrimitive();
		m_CustomRenderState.Clear();
	}

	if (m_CustomRenderStateBackupList.size() > 0) {
		m_CustomRenderState = m_CustomRenderStateBackupList.top();
		m_CustomRenderStateBackupList.pop();
	}
}

// wd3d8.cpp:10052
void* WDirect3D8::SnapShotCustomRenderState(void) {
	if (!m_CustomRenderState.HasAnyState())
		return NULL;

	if (m_CustomRenderStateSnapShotList.empty()) {
		m_CustomRenderStateSnapShotList.push_back(m_CustomRenderState);
	} else {
		const WRenderState& last = m_CustomRenderStateSnapShotList.back();

		if (m_CustomRenderState != last)
			m_CustomRenderStateSnapShotList.push_back(m_CustomRenderState);
	}

	return &m_CustomRenderStateSnapShotList.back();
}

// wd3d8.cpp:10070
void WDirect3D8::ClearCustomRenderStateSnapShotList(void) {
	m_CustomRenderStateSnapShotList.clear();
}

// wd3d8.cpp:10075
void WDirect3D8::ApplyCustomRenderState(const void* state) {
	if (state) {
		const WRenderState* pState = (const WRenderState*)state;

		if (!(*pState == m_CustomRenderState))
			FlushRenderPrimitive();

		m_CustomRenderState = *pState;
	} else {
		if (m_CustomRenderState.HasAnyState()) {
			FlushRenderPrimitive();

			m_CustomRenderState.Clear();
		}
	}
}

// wd3d8.cpp:10101
void WDirect3D8::SetViewPort(unsigned long x, unsigned long y, unsigned long w, unsigned long h) {
	if (m_devState == 1) return;

	_D3DVIEWPORT9 viewport = { x, y, w, h, 0.0f, 1.0f };

	if (memcmp(&viewport, &m_curRt.m_viewport, sizeof(viewport)) != 0) {
		FlushRenderPrimitive();

		m_curRt.m_viewport = viewport;

		m_pd3dDevice->SetViewport(&viewport);
	}
}

// wd3d8.cpp:10127
void WDirect3D8::BackupMainRenderTarget(void) {
	m_pd3dDevice->GetRenderTarget(0, &m_mainRt.m_rt.m_surf[0]);
	m_mainRt.m_rt.m_surf[1] = NULL;

	m_pd3dDevice->GetDepthStencilSurface(&m_mainRt.m_depthSurf);

	m_pd3dDevice->GetViewport(&m_mainRt.m_viewport);
}

// wd3d8.cpp:10141
void WDirect3D8::ReleaseMainRenderTarget(void) {
	for (int i = 0; i < sizeof(m_mainRt.m_rt.m_surf) / sizeof(m_mainRt.m_rt.m_surf[0]); i++) {
		if (m_mainRt.m_rt.m_surf[i])
			m_mainRt.m_rt.m_surf[i]->Release();
	}

	if (m_mainRt.m_depthSurf)
		m_mainRt.m_depthSurf->Release();

	m_mainRt.Init();
}

// wd3d8.cpp:10157
void WDirect3D8::BackupRenderTarget(const WRenderToTextureParam& param, IDirect3DSurface9* depthSurf, const _D3DVIEWPORT9& viewport) {
	m_rtBackupList.push(m_curRt);

	sRtBackup newRt;

	for (int i = 0; i < WRenderToTextureParam::NUM_SIMULTANEOUSRTS; i++) {
		if (param.m_rtTexInfo[i].m_hTex > 0)
			newRt.m_rt.m_surf[i] = m_texList[param.m_rtTexInfo[i].m_hTex].pSurf;
		else
			newRt.m_rt.m_surf[i] = NULL;
	}

	newRt.m_depthSurf = depthSurf;
	newRt.m_viewport = viewport;

	m_curRt = newRt;
}

// wd3d8.cpp:10179
void WDirect3D8::RestoreBackedupRenderTarget(void) {
	if (m_rtBackupList.empty()) return;

	sRtBackup backedupRt = m_rtBackupList.top();
	m_rtBackupList.pop();

	backedupRt.Restore(m_pd3dDevice, m_curRt);

	m_curRt = backedupRt;
}

// wd3d8.cpp:10194
void WDirect3D8::ReleaseAllRendertargetBackupResource(void) {
	while (m_rtBackupList.size() > 0)
		m_rtBackupList.pop();

	m_curRt.Init();

	ReleaseMainRenderTarget();
}

// wd3d8.cpp:8978
void EnumDirect3D8(void) {
	_D3DCAPS9 d3dcaps;
	_D3DADAPTER_IDENTIFIER9 identifier;

	IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (pD3D) {
		for (unsigned int i = 0; i < pD3D->GetAdapterCount(); i++) {
			ZeroMemory(&identifier, sizeof(identifier)); if (pD3D->GetAdapterIdentifier(i, 0, &identifier) != D3D_OK) continue;
			if (pD3D->GetDeviceCaps(i, D3DDEVTYPE_HAL, &d3dcaps) != D3D_OK) continue;
			AddVideoDevice(new WDirect3D8(identifier.Description, i));
		}

		pD3D->Release();
	} else {
		g_error = g_msgD3DInitFailed;
	}
}
