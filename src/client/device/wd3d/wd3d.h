// Original: d:\build\custom\temp\code\client\device\wd3d\wd3d.h
#ifndef PG_CLIENT_DEVICE_WD3D_WD3D_H
#define PG_CLIENT_DEVICE_WD3D_WD3D_H

#include <windows.h>
#include <math.h>
#include <string.h>
#include <d3d9.h>
#include <vector>

#include "defines.h"
#include "wvideo.h"
#include "wview.h"
#include "wutil.h"

// The Direct3D video device base of the Wangreal DLL: polygon buffering and
// sorting, texture and vertex/index buffer handle tables (PDB layout, size
// 0x3888). WDirect3D8 (wd3d8.h) implements the device.
class WDirect3D : public WVideoDev {
	LAYOUT_CHECK_FRIEND;
public:
	// One buffered polygon batch, sorted by depth.
	struct w_poly_buffer {
		bool bHw;                               // 0x0
		float depth;                            // 0x4
	};
	// A software-transformed batch drawn with DrawPrimitive.
	struct w_poly_buffer_sw : public w_poly_buffer {
		int type;                               // 0x8
		int type2;                              // 0xc
		int num;                                // 0x10
		unsigned long dwVertexTypeDesc;         // 0x14
		void* lpvVertices;                      // 0x18
		_D3DPRIMITIVETYPE dptPrimitiveType;     // 0x1c
		void* customRenderState;                // 0x20
	};
	// A vertex/index buffer batch drawn with xDrawIndexedPrimitive.
	struct w_poly_buffer_hw : public w_poly_buffer {
		WxViewState VState;                     // 0x8
		WxBatchState BState;                    // 0xa4
		void* customRenderState;                // 0x100
	};
	enum enumTexState {
		TS_VOID,
		TS_CREATED,
		TS_UPDATED
	};

	WDirect3D();
	virtual ~WDirect3D();

	// wd3d.h:62
	virtual wVDevState GetDeviceState() const { return m_devState; }
	// wd3d.h:63
	virtual int GetBackBufferBpp() const { return m_BackBufBpp; }
	// wd3d.h:88
	virtual int GetBufferingMeshNum() const { return m_bufPolySwNum; }

	virtual void DrawPolygonFan(WTVertex** p, int iType, int iNum, int iType2, unsigned long dwVertexTypeDesc);
	virtual void DrawIndexedTriangles(WTVertex* pVtx, int nVtx, unsigned short* pIdx, int nIdx, int a, int b);
	virtual int CreateTexture(tagBITMAPINFO* src, int type);
	virtual void UpdateTexture(int texHandle, tagBITMAPINFO* src, void* data, unsigned long type);
	virtual int UploadCompressedTexture(void* pSrc, unsigned int srcSize, int type);
	virtual bool SetFogEnable(bool enable);
	virtual void SetFogState(float fogStart, float fogEnd, unsigned long color);
	virtual bool Reset(bool windowed, int width, int height, int bpp, long refresh, int flags) = 0;

	// Virtuals WDirect3D introduces (vtable 0x140-0x184, in slot order).
	virtual void DrawPrimitive(int type, int num, unsigned long dwVertexTypeDesc, void* lpvVertices, _D3DPRIMITIVETYPE dptPrimitiveType, int type2) = 0;  // vtable 0x140
	virtual void DrawPrimitiveIndexed(int type, unsigned long dwVertexTypeDesc, void* lpvVertices, int nVtx, unsigned short* pIdx, int nIdx, int flags) = 0;  // vtable 0x144
	virtual bool SetRenderState4Flushing(int type) = 0;    // vtable 0x148
protected:
	virtual void Flush(unsigned long flags);                // vtable 0x14c
	virtual void FlushEqual();                              // vtable 0x150
	virtual void FlushMultiPass(unsigned long flags);       // vtable 0x154
	virtual void FlushOnePass(unsigned long flags);         // vtable 0x158
	virtual void FlushAlways();                             // vtable 0x15c
public:
	virtual int UploadCompressedTextureSurface(void* pSrc, unsigned int srcSize, int type) = 0;  // vtable 0x160
	virtual void CreateTextureSurface(unsigned long dwWidth, unsigned long dwHeight, int iMipmapcount, int renderTargetType, int iIndex, int bChromakey, int bitnum) = 0;  // vtable 0x164
	virtual void UpdateTextureSurface(int texHandle, tagBITMAPINFO* src, void* data, int type) = 0;  // vtable 0x168
private:
	// wd3d.h:140
	virtual void FlushRenderPrimitive() {}                  // vtable 0x16c
protected:
	virtual void* SnapShotCustomRenderState() = 0;          // vtable 0x170
	virtual void ClearCustomRenderStateSnapShotList() = 0;  // vtable 0x174
	virtual void ApplyCustomRenderState(const void* state) = 0;  // vtable 0x178
public:
	virtual void xCreateVertexBuffer(int hVb, int nVtxs, unsigned long dwFVF, unsigned long dwUsage) = 0;  // vtable 0x17c
	virtual void xCreateIndexBuffer(int hIb, int nIdxs, unsigned long dwUsage) = 0;  // vtable 0x180
	virtual void xDrawIndexedPrimitive(const WxViewState& VState, const WxBatchState& BState) = 0;  // vtable 0x184

	virtual int xCreateVertexBuffer(int nVtxs, unsigned long dwFVF, unsigned long dwUsage);
	virtual int xCreateIndexBuffer(int nIdxs, unsigned long dwUsage);
	virtual void xReleaseVertexBuffer(int hVb);
	virtual void xReleaseIndexBuffer(int hIb);
	virtual void xDrawIndexedTriangles(const WxViewState& VState, const WxBatchState& BState);

protected:
	void Buffering(const WxViewState& VState, const WxBatchState& BState);
	void Buffering(int type, int type2, int num, unsigned long dwVertexTypeDesc, void* p, _D3DPRIMITIVETYPE dptPrimitiveType, float depth);
	void Release();
	void Init();
	int GetTextureNum(int stage);
	virtual void DestroyTexture(int texHandle);
	virtual bool SupportRenderTargetFormat() const = 0;
	virtual bool IsSupportedDisplayMode(bool windowed, int width, int height, int bpp) = 0;
	virtual void SetViewPort(unsigned long x, unsigned long y, unsigned long width, unsigned long height) = 0;
	void* ModifyVertices(WTVertex* pVtx, int nVtx, unsigned long dwVertexTypeDesc, bool b);
	void* ModifyVertices(WTVertex** ppVtx, int nVtx, unsigned long dwVertexTypeDesc, bool b);
	virtual unsigned long VertexSize(unsigned long fvf);
	void ApplyGamma(float gamma);
	// wd3d.h:125
	unsigned int GetSortBufferSize() const { return m_buf4sort.size(); }
	// wd3d.h:126
	unsigned int GetSortBufferSwSize() const { return m_bufPolySw.size(); }
	// wd3d.h:127
	unsigned int GetSortBufferHwSize() const { return m_bufPolyHw.size(); }

private:
	// wd3d.h:137
	void ClearVertices() { m_dwOffset = 0; }
	unsigned char CalcFog(float depth);
	void DrawBuffered(const w_poly_buffer_hw& bufHw);
	void DrawBuffered(const w_poly_buffer_sw& bufSw);
	static int __cdecl ComparePolyDepth(const void* elem1, const void* elem2);
	float CalcDepth(WTVertex** p, int num);
	int xGetVbHandle();
	int xGetIbHandle();

protected:
	int m_iRefTable[256];                           // 0x14
	int m_MaxTextureBlendStages;                    // 0x414
	int m_BackBufBpp;                               // 0x418
private:
	char* m_chVertex;                               // 0x41c
	unsigned long m_dwOffset;                       // 0x420
	std::vector<w_poly_buffer*> m_buf4sort;         // 0x424
	std::vector<w_poly_buffer_sw> m_bufPolySw;      // 0x434
	std::vector<w_poly_buffer_hw> m_bufPolyHw;      // 0x444
	int m_texCount1;                                // 0x454
	int m_texCount2;                                // 0x458
	enumTexState m_texList[2048];                   // 0x45c
protected:
	int m_bufPolySwNum;                             // 0x245c
	int m_bufPolyHwNum;                             // 0x2460
	int m_bufSortNum;                               // 0x2464
	wVDevState m_devState;                          // 0x2468
	bool m_fog;                                     // 0x246c
	float m_fogStart;                               // 0x2470
	float m_fogEnd;                                 // 0x2474
	unsigned long m_fogColor;                       // 0x2478
	bool m_useHiQualityTex;                         // 0x247c
private:
	int m_xiVbCount;                                // 0x2480
	int m_xiIbCount;                                // 0x2484
	int m_xahVbs[1024];                             // 0x2488
	int m_xahIbs[256];                              // 0x3488
};
CHECK_SIZE(WDirect3D::w_poly_buffer, 0x8);
CHECK_SIZE(WDirect3D::w_poly_buffer_sw, 0x24);
CHECK_SIZE(WDirect3D::w_poly_buffer_hw, 0x104);
CHECK_FIELD(WDirect3D::w_poly_buffer_sw, customRenderState, 0x20);
CHECK_FIELD(WDirect3D::w_poly_buffer_hw, BState, 0xa4);
CHECK_FIELD(WDirect3D::w_poly_buffer_hw, customRenderState, 0x100);
CHECK_SIZE(WDirect3D, 0x3888);
BEGIN_LAYOUT_CHECK(WDirect3D)
	CHECK_FIELD(WDirect3D, m_iRefTable, 0x14);
	CHECK_FIELD(WDirect3D, m_MaxTextureBlendStages, 0x414);
	CHECK_FIELD(WDirect3D, m_BackBufBpp, 0x418);
	CHECK_FIELD(WDirect3D, m_chVertex, 0x41c);
	CHECK_FIELD(WDirect3D, m_dwOffset, 0x420);
	CHECK_FIELD(WDirect3D, m_buf4sort, 0x424);
	CHECK_FIELD(WDirect3D, m_bufPolySw, 0x434);
	CHECK_FIELD(WDirect3D, m_bufPolyHw, 0x444);
	CHECK_FIELD(WDirect3D, m_texCount1, 0x454);
	CHECK_FIELD(WDirect3D, m_texCount2, 0x458);
	CHECK_FIELD(WDirect3D, m_texList, 0x45c);
	CHECK_FIELD(WDirect3D, m_bufPolySwNum, 0x245c);
	CHECK_FIELD(WDirect3D, m_bufPolyHwNum, 0x2460);
	CHECK_FIELD(WDirect3D, m_bufSortNum, 0x2464);
	CHECK_FIELD(WDirect3D, m_devState, 0x2468);
	CHECK_FIELD(WDirect3D, m_fog, 0x246c);
	CHECK_FIELD(WDirect3D, m_fogStart, 0x2470);
	CHECK_FIELD(WDirect3D, m_fogEnd, 0x2474);
	CHECK_FIELD(WDirect3D, m_fogColor, 0x2478);
	CHECK_FIELD(WDirect3D, m_useHiQualityTex, 0x247c);
	CHECK_FIELD(WDirect3D, m_xiVbCount, 0x2480);
	CHECK_FIELD(WDirect3D, m_xiIbCount, 0x2484);
	CHECK_FIELD(WDirect3D, m_xahVbs, 0x2488);
	CHECK_FIELD(WDirect3D, m_xahIbs, 0x3488);
END_LAYOUT_CHECK

// wd3d.h:266
// The size of one vertex of a flexible vertex format (bit 31, the
// fog flag, is not part of the format); 0 for a format not listed.
inline unsigned long WDirect3D::VertexSize(unsigned long dwVertexTypeDesc) {
	switch (dwVertexTypeDesc & ~0x80000000) {
	case D3DFVF_SPECULAR:
	case D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE:
	case D3DFVF_XYZ | D3DFVF_TEX2:
	case D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1:
	case D3DFVF_XYZRHW | D3DFVF_SPECULAR | D3DFVF_TEX1:
	case D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1:
		return 0x1c;
	case D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1:
	case D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1:
	case D3DFVF_XYZRHW | D3DFVF_TEX2:
	case D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2:
	case D3DFVF_XYZ | D3DFVF_SPECULAR | D3DFVF_TEX2:
		return 0x20;
	case D3DFVF_XYZ | D3DFVF_DIFFUSE:
		return 0x10;
	case D3DFVF_XYZRHW | D3DFVF_DIFFUSE:
	case D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR:
	case D3DFVF_XYZ | D3DFVF_TEX1:
		return 0x14;
	case 0:
	case D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR:
	case D3DFVF_XYZRHW | D3DFVF_TEX1:
	case D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1:
	case D3DFVF_XYZ | D3DFVF_SPECULAR | D3DFVF_TEX1:
	// Skinned formats: blend weights, the last one as UBYTE4 indices.
	case D3DFVF_XYZB1 | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4:
		return 0x18;
	case D3DFVF_XYZB2 | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4:
		return 0x1c;
	case D3DFVF_XYZB3 | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4:
		return 0x20;
	case D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2:
	case D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2:
	case D3DFVF_XYZB2 | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4:
		return 0x28;
	case D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2:
	case D3DFVF_XYZB3 | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4:
		return 0x2c;
	case D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1:
	case D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2:
	case D3DFVF_XYZRHW | D3DFVF_SPECULAR | D3DFVF_TEX2:
	case D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2:
	case D3DFVF_XYZB4 | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4:
	case D3DFVF_XYZB1 | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4:
		return 0x24;
	case D3DFVF_XYZB4 | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4:
		return 0x30;
	}

	return 0;
}

#endif // PG_CLIENT_DEVICE_WD3D_WD3D_H
