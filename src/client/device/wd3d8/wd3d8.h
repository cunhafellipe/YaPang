// Original: d:\build\custom\temp\code\client\device\wd3d8\wd3d8.h
#ifndef PG_CLIENT_DEVICE_WD3D8_WD3D8_H
#define PG_CLIENT_DEVICE_WD3D8_WD3D8_H

#include <windows.h>
#include <string.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <deque>
#include <list>
#include <map>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "defines.h"
#include "padding.h"
#include "wd3d/wd3d.h"
#include "wproc.h"
#include "wscene.h"
#include "wtextureview.h"
#include "screencape.h"

class Bitmap;
class WSplashD3D;

namespace nsWindowUtility {
class CDWMApiDll;
}

// The Direct3D 9 video device of the Wangreal DLL (PDB layout, size 0x3068c).
// Member declarations are added as their functions are decompiled.
class WDirect3D8 : public WDirect3D, public WProc {
	LAYOUT_CHECK_FRIEND;
	friend class WSplashD3D;
public:
	struct sRtBackup {
		struct Rt {
			// wd3d8.h:203
			Rt() { Init(); }
			// wd3d8.h:204
			void Init() {
				memset(m_surf, 0, sizeof(m_surf));
			}

			IDirect3DSurface9* m_surf[2];       // 0x0
		};
		sRtBackup();
		void Init();
		void Restore(IDirect3DDevice9* dev, const sRtBackup& curRt);

		Rt m_rt;                                // 0x0
		IDirect3DSurface9* m_depthSurf;         // 0x8
		_D3DVIEWPORT9 m_viewport;               // 0xc
	};
	struct sRtFormat {
		_D3DFORMAT fmt;                         // 0x0
		int bpp;                                // 0x4
	};
	struct FMTLIST {
		_D3DFORMAT format;                      // 0x0
		char* dispaly;                          // 0x4
		int bitNum;                             // 0x8
	};
	struct pix_info {
		_D3DFORMAT pixFmt;                      // 0x0
		unsigned int cpp;                       // 0x4
		unsigned int r_r_shift;                 // 0x8
		unsigned int r_l_shift;                 // 0xc
		unsigned int g_r_shift;                 // 0x10
		unsigned int g_l_shift;                 // 0x14
		unsigned int b_r_shift;                 // 0x18
		unsigned int b_l_shift;                 // 0x1c
		unsigned int a_r_shift;                 // 0x20
		unsigned int a_l_shift;                 // 0x24

		// wd3d8.h:314
		unsigned long Pack(unsigned char r, unsigned char g, unsigned char b) {
			return ((r >> r_r_shift) << r_l_shift) | ((g >> g_r_shift) << g_l_shift) | ((b >> b_r_shift) << b_l_shift);
		}
		// wd3d8.h:318
		unsigned long Pack(unsigned char a, unsigned char r, unsigned char g, unsigned char b) {
			return ((a >> a_r_shift) << a_l_shift) | ((r >> r_r_shift) << r_l_shift) | ((g >> g_r_shift) << g_l_shift) | ((b >> b_r_shift) << b_l_shift);
		}
	};
	struct d3d8_texture {
		Bitmap* bitmap;                         // 0x0
		int dxtcDataSize;                       // 0x4
		unsigned char* dxtcData;                // 0x8
		int updateType;                         // 0xc
		bool needToBeFilled;                    // 0x10
		IDirect3DTexture9* pTex;                // 0x14
		IDirect3DSurface9* pSurf;               // 0x18
		int width;                              // 0x1c
		int height;                             // 0x20
		int mipmaplevel;                        // 0x24
		pix_info* pixFmtInfo;                   // 0x28
		int renderTargetType;                   // 0x2c
		WRenderToTextureSizeInfo renderTargetSizeInfo;  // 0x30
		bool isFilled;                          // 0x3c
	};
	struct sDepthSurf {
		unsigned short wWidth;                  // 0x0
		unsigned short wHeight;                 // 0x2
		IDirect3DSurface9* pDepth;              // 0x4
	};
	struct sVb8 {
		unsigned long xdwFVF;                   // 0x0
		unsigned long xdwUsage;                 // 0x4
		unsigned char xbStride;                 // 0x8
		IDirect3DVertexBuffer9* xpVb;           // 0xc
		int xnVtxs;                             // 0x10
		bool xbNeedToBeFilled;                  // 0x14
		unsigned char* xpVertexData;            // 0x18
	};
	struct sIb8 {
		unsigned long xdwUsage;                 // 0x0
		IDirect3DIndexBuffer9* xpIb;            // 0x4
		int xnIdxs;                             // 0x8
		bool xbNeedToBeFilled;                  // 0xc
		unsigned short* xpIndexData;            // 0x10
	};
	struct sVertexDecl {
		IDirect3DVertexDeclaration9* pDecl;     // 0x0
		int pitch;                              // 0x4
	};
	struct sEffect {
		struct Param {
			// wd3d8.h:565
			Param() : handle(NULL), isShared(false) {
			}

			const char* handle;                 // 0x0
			bool isShared;                      // 0x4
		};
		// wd3d8.h:574
		sEffect() : pEffect(NULL) {
		}

		ID3DXEffect* pEffect;                   // 0x0
		Param param[21];                        // 0x4
	};
	struct sMergeBuffer {
		_D3DPRIMITIVETYPE Type;                 // 0x0
		unsigned int NumVertices;               // 0x4
		unsigned int PrimitiveCount;            // 0x8
		unsigned int VertexStreamZeroStride;    // 0xc
		int xhIb;                               // 0x10
		int xhVb;                               // 0x14
		unsigned int MinIndex;                  // 0x18
		unsigned int StartIndex;                // 0x1c
		int LockVertex;                         // 0x20
		int LockIndex;                          // 0x24
		D3DXMATRIX m_xLastMatrix;               // 0x28
		unsigned char* VertexStreamBuffer;      // 0x68
		unsigned long VertexStreamBufferSize;   // 0x6c
		unsigned short* IndexBuffer;            // 0x70
		unsigned long IndexBufferSize;          // 0x74

		// wd3d8.h:616
		unsigned long GetVertexStreamBufferSize(void) const { return VertexStreamBufferSize; }
		// wd3d8.h:617
		unsigned long GetIndexBufferSize(void) const { return IndexBufferSize; }
		void CheckAndIncreaseVertexStreamBuffer(unsigned long size);
		void CheckAndIncreaseIndexBuffer(unsigned long size);
	};

	// The effect parameter cache shared by the device's effects (size 0x94).
	class WFxParamPool {
		LAYOUT_CHECK_FRIEND;
	public:
		void Create(IDirect3DDevice9* pDevice);
		void Release();
		void OnLostDevice(IDirect3DDevice9* pDevice);
		void OnResetDevice(IDirect3DDevice9* pDevice);
		void GetInt(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, int& Value) const;
		void GetVector2(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, D3DXVECTOR2& Value) const;
		void GetVector3(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, D3DXVECTOR3& Value) const;
		void GetVector4(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, D3DXVECTOR4& Value) const;
		void GetMatrix(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, D3DXMATRIX& Value) const;
		void GetTexture(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, IDirect3DBaseTexture9*& Value) const;
		void SetInt(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, int Value);
		void SetVector2(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, const D3DXVECTOR2& Value);
		void SetVector3(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, const D3DXVECTOR3& Value);
		void SetVector4(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, const D3DXVECTOR4& Value);
		void SetMatrix(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, const D3DXMATRIX& Value);
		void SetTexture(ID3DXEffect* ef, WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared, IDirect3DBaseTexture9* Value);
		// wd3d8.h:648
		WFxParamPool() : m_pool(NULL) {
		}
		// wd3d8.h:663
		ID3DXEffectPool* GetPool(void) const {
			return m_pool;
		}

	private:
		void ClearParamCache();

		ID3DXEffectPool* m_pool;                                                                              // 0x0
		std::pair<std::map<WVDFXPARAMETERTYPE, int>, std::map<const char*, int> > m_paramCacheListPair_Int;   // 0x4
		std::pair<std::map<WVDFXPARAMETERTYPE, D3DXVECTOR2>, std::map<const char*, D3DXVECTOR2> > m_paramCacheListPair_Vec2;   // 0x1c
		std::pair<std::map<WVDFXPARAMETERTYPE, D3DXVECTOR3>, std::map<const char*, D3DXVECTOR3> > m_paramCacheListPair_Vec3;   // 0x34
		std::pair<std::map<WVDFXPARAMETERTYPE, D3DXVECTOR4>, std::map<const char*, D3DXVECTOR4> > m_paramCacheListPair_Vec4;   // 0x4c
		std::pair<std::map<WVDFXPARAMETERTYPE, D3DXMATRIX>, std::map<const char*, D3DXMATRIX> > m_paramCacheListPair_Mat;      // 0x64
		std::pair<std::map<WVDFXPARAMETERTYPE, IDirect3DBaseTexture9*>, std::map<const char*, IDirect3DBaseTexture9*> > m_paramCacheListPair_Tex;   // 0x7c
	};

	// One recorded state: its key, the value to set and the value it replaced.
	template <class T1, class T2>
	struct sTwoVarRs {
		// wd3d8.h:746
		sTwoVarRs(const T1& Type, const T2& Value) : m_Type(Type), m_Value(Value) {
			memset(&m_OldValue, 0, sizeof(m_OldValue));
		}
		// wd3d8.h:751
		virtual ~sTwoVarRs() {
		}
		bool operator==(const sTwoVarRs& rhs) const;
		bool operator!=(const sTwoVarRs& rhs) const;

		T1 m_Type;                              // 0x4
		T2 m_Value;
		T2 m_OldValue;
	};
	// A recorded state with a two-part key (stage or sampler, then type).
	template <class T1, class T2, class T3>
	struct sThreeVarRs : public sTwoVarRs<T1, T3> {
		// wd3d8.h:777
		sThreeVarRs(const T1& Type, const T2& Type2, const T3& Value) : sTwoVarRs<T1, T3>(Type, Value), m_Type2(Type2) {
		}
		bool operator==(const sThreeVarRs& rhs) const;
		bool operator!=(const sThreeVarRs& rhs) const;

		T2 m_Type2;                             // 0x10
	};
	struct sClipPlane : public sTwoVarRs<unsigned long, D3DXPLANE> {
		// wd3d8.h:801
		sClipPlane(unsigned long Index, const D3DXPLANE& Plane) : sTwoVarRs<unsigned long, D3DXPLANE>(Index, Plane) {
		}

		D3DXPLANE m_SetValue;                   // 0x28
	};

	// A recorded set of custom render states (size 0xc4).
	class WRenderState {
		LAYOUT_CHECK_FRIEND;
	public:
		typedef sTwoVarRs<_D3DRENDERSTATETYPE, unsigned long> sRs;
		typedef std::vector<sRs> RsList;
		typedef sThreeVarRs<unsigned long, _D3DTEXTURESTAGESTATETYPE, unsigned long> sTss;
		typedef std::vector<sTss> TssList;
		typedef sTwoVarRs<_D3DTRANSFORMSTATETYPE, D3DXMATRIX> sTransf;
		typedef std::vector<sTransf> TransfList;
		typedef sTwoVarRs<unsigned long, IDirect3DBaseTexture9*> sTex;
		typedef std::vector<sTex> TexList;
		typedef std::vector<sClipPlane> ClipPlaneList;
		typedef sThreeVarRs<unsigned long, _D3DSAMPLERSTATETYPE, unsigned long> sSs;
		typedef std::vector<sSs> SsList;
		typedef sTwoVarRs<WVDFXPARAMETERTYPE, int> sFxParamInt;
		typedef std::vector<sFxParamInt> FxParamIntList;
		typedef sTwoVarRs<WVDFXPARAMETERTYPE, D3DXVECTOR2> sFxParamVec2;
		typedef std::vector<sFxParamVec2> FxParamVec2List;
		typedef sTwoVarRs<WVDFXPARAMETERTYPE, D3DXVECTOR3> sFxParamVec3;
		typedef std::vector<sFxParamVec3> FxParamVec3List;
		typedef sTwoVarRs<WVDFXPARAMETERTYPE, D3DXVECTOR4> sFxParamVec4;
		typedef std::vector<sFxParamVec4> FxParamVec4List;
		typedef sTwoVarRs<WVDFXPARAMETERTYPE, D3DXMATRIX> sFxParamMat;
		typedef std::vector<sFxParamMat> FxParamMatList;
		typedef sTwoVarRs<WVDFXPARAMETERTYPE, IDirect3DBaseTexture9*> sFxParamTex;
		typedef std::vector<sFxParamTex> FxParamTexList;

		// wd3d8.h:873
		WRenderState(void)
			: m_fxMacro(0) {
		}

		// wd3d8.h:920
		void Clear(void) {
			m_rsList.clear();
			m_tssList.clear();
			m_transfList.clear();
			m_texList.clear();
			m_clipPlaneList.clear();

			m_ssList.clear();

			m_fxMacro = 0;
			m_fxParamIntList.clear();
			m_fxParamVec2List.clear();
			m_fxParamVec3List.clear();
			m_fxParamVec4List.clear();
			m_fxParamMatList.clear();
			m_fxParamTexList.clear();
		}
		// wd3d8.h:940
		bool HasAnyState(void) const {
			if (m_rsList.size() > 0 || m_tssList.size() > 0 || m_transfList.size() > 0 || m_texList.size() > 0 || m_clipPlaneList.size() > 0
				|| m_ssList.size() > 0
				|| m_fxMacro != 0 || m_fxParamIntList.size() > 0 || m_fxParamVec2List.size() > 0 || m_fxParamVec3List.size() > 0
				|| m_fxParamVec4List.size() > 0 || m_fxParamMatList.size() > 0 || m_fxParamTexList.size() > 0)
				return true;

			return false;
		}
		bool operator==(const WRenderState& rhs) const;
		// wd3d8.h:967
		bool operator!=(const WRenderState& rhs) const {
			return !(*this == rhs);
		}
		// wd3d8.h:972
		unsigned long GetFxMacro(void) const { return m_fxMacro; }
		void SetRenderState(_D3DRENDERSTATETYPE State, unsigned long Value);
		void SetTextureStageState(unsigned long Stage, _D3DTEXTURESTAGESTATETYPE Type, unsigned long Value);
		void SetTransform(_D3DTRANSFORMSTATETYPE State, const D3DXMATRIX& Matrix);
		void SetClipPlane(unsigned long Index, const D3DXPLANE& Plane);
		void SetSamplerState(unsigned long Sampler, _D3DSAMPLERSTATETYPE Type, unsigned long Value);
		void SetFxMacro(unsigned long FxMacro);
		void SetFxParamInt(WVDFXPARAMETERTYPE ParamType, int Value);
		void SetFxParamVector2(WVDFXPARAMETERTYPE ParamType, const D3DXVECTOR2& Value);
		void SetFxParamVector3(WVDFXPARAMETERTYPE ParamType, const D3DXVECTOR3& Value);
		void SetFxParamVector4(WVDFXPARAMETERTYPE ParamType, const D3DXVECTOR4& Value);
		void SetFxParamMatrix(WVDFXPARAMETERTYPE ParamType, const D3DXMATRIX& Value);
		void SetTexture(unsigned long Stage, IDirect3DBaseTexture9* pTexture);
		void SetFxParamTexture(WVDFXPARAMETERTYPE ParamType, IDirect3DBaseTexture9* pTexture);
		void Begin(WDirect3D8& wd3d);
		void End(WDirect3D8& wd3d);

	protected:
		RsList m_rsList;                        // 0x0
		TssList m_tssList;                      // 0x10
		TransfList m_transfList;                // 0x20
		TexList m_texList;                      // 0x30
		ClipPlaneList m_clipPlaneList;          // 0x40
		SsList m_ssList;                        // 0x50
		unsigned long m_fxMacro;                // 0x60
		FxParamIntList m_fxParamIntList;        // 0x64
		FxParamVec2List m_fxParamVec2List;      // 0x74
		FxParamVec3List m_fxParamVec3List;      // 0x84
		FxParamVec4List m_fxParamVec4List;      // 0x94
		FxParamMatList m_fxParamMatList;        // 0xa4
		FxParamTexList m_fxParamTexList;        // 0xb4
	};

	WDirect3D8(char* devName, int id);
	virtual ~WDirect3D8();

protected:
	// The first virtual WDirect3D8 introduces (vtable 0x188, slot 98).
	virtual WDirect3D8* CreateClone(char* devName, int id);

public:

	int Init(char* modeName, HWND__* hwnd, int iTnL);
	bool LoadSHCoeff(const char* fname);
	virtual WVideoDev* MakeClone(char* modeName, HWND__* hwnd, int iTnL);

	// wd3d8.h:98
	virtual WProc* ExternProc(void) { return this; }
	// wd3d8.h:135
	virtual void DrawLine(WTVertex** p, int type) {}
	// wd3d8.h:158
	virtual void SetGlobalRenderState(int rs, int rsex) { m_GlobalRS[0] = rs; m_GlobalRS[1] = rsex; }
	// wd3d8.h:160
	virtual bool IsSupportVS(void) const { return m_d3dcaps.VertexShaderVersion >= D3DVS_VERSION(1, 1); }
	// wd3d8.h:161
	virtual bool IsSupportPS(void) const { return m_d3dcaps.PixelShaderVersion >= D3DPS_VERSION(2, 0); }
	// wd3d8.h:162
	virtual bool IsSupportMRT(void) const { return m_d3dcaps.NumSimultaneousRTs > 1 ? true : false; }
	// wd3d8.h:163
	virtual bool IsSupportClipPlane(void) const { return m_d3dcaps.MaxUserClipPlanes >= 1 ? true : false; }
	// wd3d8.h:169
	virtual int GetWidth(void) const { return m_d3dpp.BackBufferWidth; }
	// wd3d8.h:170
	virtual int GetHeight(void) const { return m_d3dpp.BackBufferHeight; }
	// wd3d8.h:171
	virtual bool IsWindowed(void) const { return m_d3dpp.Windowed ? true : false; }
	// wd3d8.h:173
	virtual bool IsFillScreenMode(void) const { return m_fillScrMode; }
	// wd3d8.h:175
	virtual float GetMonitorSupportFPS(void) const { return m_fps; }
	// wd3d8.h:179
	virtual bool SupportRenderTargetFormat(void) const { return m_RtFmtIdx >= 0; }
	virtual bool IsTextureFilled(int texHandle) const;
	virtual void Clear(unsigned long color, int flags, float z);
	virtual int WinProc(unsigned int msg, unsigned long wParam, unsigned long lParam);
	virtual int Command(wVDevMessage msg, int wParam, int lParam);
	virtual bool Reset(bool windowed, int width, int height, int bpp, long refresh, int flags);
	virtual bool BeginScene(void);
	virtual void FixTexturePart(int tex, const tagRECT& rect, tagBITMAPINFO* pInfo, void* pBits, int flags);
	virtual int xGetVertexElemOffset(unsigned long dwFVF, wWxVertexElem elem) const;
	virtual int xGetBlendWeightSize(unsigned long dwFVF) const;
	virtual unsigned long xDetermineFVF(int a, int b, int c);
	virtual void EndUsingCustomRenderState(void);
	virtual bool BeginRenderToTexture(const WRenderToTextureParam& param);
	virtual void EndRenderToTexture(const WRenderToTextureParam& param);
	virtual bool IsSupportedDisplayMode(bool windowed, int width, int height, int bpp);
	virtual void SetViewPort(unsigned long x, unsigned long y, unsigned long width, unsigned long height);
	virtual void DrawPrimitive(int type, int num, unsigned long dwVertexTypeDesc, void* lpvVertices, _D3DPRIMITIVETYPE dptPrimitiveType, int type2);
	virtual void DrawPrimitiveIndexed(int type, unsigned long dwVertexTypeDesc, void* lpvVertices, int nVtx, unsigned short* pIdx, int nIdx, int flags);
	virtual bool SetRenderState4Flushing(int type);
	virtual int UploadCompressedTextureSurface(void* pSrc, unsigned int srcSize, int type);
	virtual void CreateTextureSurface(unsigned long dwWidth, unsigned long dwHeight, int iMipmapcount, int renderTargetType, int iIndex, int bChromakey, int bitnum);
	virtual void UpdateTextureSurface(int texHandle, tagBITMAPINFO* src, void* data, int type);
	virtual void xDrawIndexedPrimitive(const WxViewState& VState, const WxBatchState& BState);
	virtual bool xHasVertexElem(unsigned long dwFVF, wWxVertexElem elem) const;
	virtual void Paint(void);
	virtual void xCreateIndexBuffer(int hIb, int nIdxs, unsigned long dwUsage);
	virtual void DestroyTexture(int texHandle);
	virtual unsigned long xDetermineBufferUsage(unsigned long dwFVF);
	virtual void SetCustomFxMacro(unsigned long FxMacro);
	virtual int GetTextureWidth(int hTex) const;
	virtual int GetTextureHeight(int hTex) const;
	virtual char* GetDeviceName(void);
	virtual char* EnumModeName(void);
	unsigned long xGetFVF(int hVb);
	void xGetCaps(unsigned long cap, unsigned long* value);
	virtual unsigned char xGetStride(int hVb);
	virtual unsigned char* xLockVertexBuffer(int hVb, unsigned int uiOffset, unsigned int uiSize);
	virtual unsigned char* xLockIndexBuffer(int hIb, unsigned int uiOffset, unsigned int uiSize);
	virtual void UpdateCompressedTexture(int texHandle, void* pSrc, unsigned int srcSize, int type);
	virtual bool SetFogEnable(bool enable);
	virtual void SetFogState(float fogStart, float fogEnd, unsigned long fogColor);
	void ResetFogState(void);
	virtual void xUnlockVertexBuffer(int hVb);
	virtual void xUnlockIndexBuffer(int hIb);
	virtual void EndScene(void);
	virtual void xCreateVertexBuffer(int hVb, int nVtxs, unsigned long dwFVF, unsigned long dwUsage);
	virtual void SetShaderSource(const char* src);
	virtual void xReleaseVertexBuffer(int hVb);
	virtual void xReleaseIndexBuffer(int hIb);
	virtual bool GetWindowDisplayMode(int& iWidth, int& iHeight, int& iColor);
	virtual void BeginUsingCustomRenderState(void);
	virtual void SetRenderTargetSizeInfo(int hTex, const WRenderToTextureSizeInfo& sizeInfo);
	virtual void SetCustomRenderState(WVDRENDERSTATETYPE State, unsigned long Value);
	virtual void SetCustomTextureStageState(unsigned long Stage, WVDTEXTURESTAGESTATETYPE Type, unsigned long Value);
	virtual void SetCustomTransform(WVDTRANSFORMSTATETYPE State, const WMatrix4& Matrix);
	virtual void SetCustomTexture(unsigned long Stage, int hTex);
	virtual void SetCustomFxParamTexture(WVDFXPARAMETERTYPE ParamType, int hTex);
	virtual void SetCustomClipPlane(unsigned long Index, const WPlane& Plane);
	virtual void SetCustomSamplerState(unsigned long Sampler, WVDSAMPLERSTATETYPE Type, unsigned long Value);
	virtual void SetCustomFxParamInt(WVDFXPARAMETERTYPE ParamType, int Value);
	virtual void SetCustomFxParamVector2(WVDFXPARAMETERTYPE ParamType, const WVector2D& Value);
	virtual void SetCustomFxParamVector3(WVDFXPARAMETERTYPE ParamType, const WVector& Value);
	virtual void SetCustomFxParamVector4(WVDFXPARAMETERTYPE ParamType, const WVector4& Value);
	virtual void SetCustomFxParamMatrix(WVDFXPARAMETERTYPE ParamType, const WMatrix4& Value);

protected:
	HRESULT Present(void);
	void SetRenderTargetFormat(void);
	void SetDefaultState(void);
	int SetTextureFormat(_D3DFORMAT suffmt);
	void Release(void);
	long SetVertexShader(unsigned long fvf);
	void BackupMainRenderTarget(void);
	void ReleaseMainRenderTarget(void);
	void BackupRenderTarget(const WRenderToTextureParam& param, IDirect3DSurface9* depthSurf, const _D3DVIEWPORT9& viewport);
	void ReleaseAllRendertargetBackupResource(void);
	_D3DFORMAT FindDepthBufferFormat(_D3DFORMAT fmt);
	IDirect3DSurface9* FindDepthSurf(unsigned short w, unsigned short h);
	void ReleaseCopiedScreenResource(void);
	unsigned long* GetZBufferHistogram(void);
	bool CaptureScreen(Bitmap* pBitmap);
	bool BeginCapturedBackground(void);
	bool GetDevName(const _D3DDISPLAYMODE& mode, char* bufptr, int buflen);
	char* DuplicateString(const char* str);
	unsigned char* GetPixelPtr(tagBITMAPINFO* bi, unsigned char* data, int x, int y);
	int GetBackBufferBpp(_D3DFORMAT fmtBackBuf);
	static pix_info* FindPixInfoByFormat(_D3DFORMAT fmt);
	void _SetTransform(_D3DTRANSFORMSTATETYPE state, const D3DXMATRIX& matrix);
	void _DrawIndexedPrimitive(_D3DPRIMITIVETYPE PrimitiveType, unsigned int BaseVertexIndex, unsigned int NumVertices, unsigned int StartIndex, unsigned int PrimitiveCount);
	void SetVtxType(unsigned long flag0, unsigned long flag1, unsigned long fvf, unsigned long diffuse);
	void SetRenderState(unsigned long type, unsigned long type2);
	void _SetRenderState(_D3DRENDERSTATETYPE State, unsigned long Value);
	void SetVtxMode(unsigned long vtxtype);
	void SetBlendState(unsigned long state, unsigned long state2);
	void SetBlendMode(unsigned long mode);
	void _DrawPrimitiveUP(_D3DPRIMITIVETYPE PrimitiveType, unsigned int PrimitiveCount, const void* pVertexStreamZeroData, unsigned int VertexStreamZeroStride);
	void _DrawIndexedPrimitiveUP(_D3DPRIMITIVETYPE Type, unsigned int NumVertices, unsigned int PrimitiveCount, const void* pIndexData, const void* pVertexStreamZeroData, unsigned int VertexStreamZeroStride);
	void RestoreBackedupRenderTarget(void);
	HRESULT ScreenShot(const char* pDestFilename, int format);
	void _SetTextureStageState(unsigned long Stage, _D3DTEXTURESTAGESTATETYPE Type, unsigned long Value);
	void _SetTexture(unsigned long Stage, IDirect3DTexture9* pTexture);
	void SetTexture(unsigned long texState);
	void UpdateTextureSurfaceNormal(void* buff, int pitch, pix_info* pix, tagBITMAPINFO* bi, void* data);
	void UpdateTextureSurfaceDirect(void* buff, int pitch, pix_info* pix, tagBITMAPINFO* bi, void* data);
	void UpdateTextureSurfaceSampling(void* buff, int width, int height, int pitch, pix_info* pix, tagBITMAPINFO* bi, void* data, int type);
	void UpdateTextureSurfaceFiltering(void* buff, int w, int h, int pitch, pix_info* pix, tagBITMAPINFO* bi, unsigned char* data);
	void UpdateTextureSurfaceAlpha(void* buff, int pitch, pix_info* pix, const tagRECT& rc, tagBITMAPINFO* bi, void* data);
	void UpdateTextureSurface(void* buff, int width, int height, int pitch, pix_info* pix, tagBITMAPINFO* bi, void* data, int type);
	void SetTextureStageState(int stage, int type, unsigned long value);
	void _SetLight(unsigned long Index, const _D3DLIGHT9* pLight);
	void _SetSamplerState(unsigned long Sampler, _D3DSAMPLERSTATETYPE Type, unsigned long Value);
	virtual void* SnapShotCustomRenderState(void);
	virtual void ClearCustomRenderStateSnapShotList(void);
	virtual void ApplyCustomRenderState(const void* state);
	void _SetShader(unsigned long effect);

	// wd3d8.h:698
	WFxParamPool& GetFxParamPool(void) {
		return m_fxParamPool;
	}

private:
	bool xReset(bool switchWinMode);
	void xReset_ReleaseResource(void);
	void xReset_CreateResource(void);
	void xSetLight(unsigned long index, const LightSet& light);
	int LockIB(const unsigned short* iptr, int inum);
	bool DrawIndexedPrimitiveLockable(_D3DPRIMITIVETYPE PrimitiveType, const void* pVertexData, int nVtxs, int stride, unsigned short* pIdx, int nPrims);
	bool DrawPrimitiveLockable(_D3DPRIMITIVETYPE PrimitiveType, unsigned int PrimitiveCount, const void* pVertexStreamZeroData, unsigned int VertexStreamZeroStride);
	int LockVB(const void* vptr, int vnum, int vpitch, int flags);
	void xSetTnLBuffer(int hVB, int hIb);
	static void FillVertex(unsigned char* dest, const unsigned char* src, int xLastVertexDecl, int NumVertices, int VertexStreamZeroStride, const D3DXMATRIX* tm);
	void xSetRenderState(unsigned long type2, unsigned long diffuse);
	void CreateEventQuery(void);
	void ReleaseEventQuery(void);
	void xInstantiateTexture(int hTex);
	void xInstantiateAndFillTexture(int hTex);
	void xFillTexture(int hTex);
	void xInstantiateVertexBuffer(int hVb);
	void xInstantiateIndexBuffer(int hIb);
	void xFillVertexBuffer(int hVb);
	void xFillIndexBuffer(int hIb);
	bool ApplyShader(void);
	void UpdateShaderValue(int h);
	bool CreateEffect(unsigned long flags);
	void ReloadShader(void);
	void ReleaseShaderResource(void);
	virtual void FlushRenderPrimitive(void);
	virtual void xSetTransform(WVDTRANSFORMSTATETYPE state, const WMatrix4& matrix);
	virtual void xSetPrevViewTransform(const WMatrix4& Matrix);

public:
	IDirect3DTexture9* m_pTexture[2];                       // 0x3890
protected:
	std::stack<sRtBackup> m_rtBackupList;                   // 0x3898
	sRtBackup m_mainRt;                                     // 0x38ac
	sRtBackup m_curRt;                                      // 0x38d0
	int m_RtFmtIdx;                                         // 0x38f4
	int m_RtNoAlphaFmtIdx;                                  // 0x38f8
	int m_RtLowMemFmtIdx;                                   // 0x38fc
	int m_RtNoAlphaLowMemFmtIdx;                            // 0x3900
	int m_RtDepthFmtIdx;                                    // 0x3904
	static sRtFormat ms_RtFmt[4];
	static sRtFormat ms_RtNoAlphaFmt[3];
	static sRtFormat ms_RtDepthFmt[5];
	IDirect3DSurface9* m_pCopiedScreenSurface;              // 0x3908
	int m_hCopiedScreenTexture;                             // 0x390c
	WSplashD3D* m_pCopiedScreenSplash;                      // 0x3910
	bool m_useCopiedScreen;                                 // 0x3914
	_D3DSURFACE_DESC m_capturedDdsd;                        // 0x3918
	unsigned long m_lastTexState;                           // 0x3938
	unsigned long m_lastRenderState;                        // 0x393c
	unsigned long m_lastBlendMode;                          // 0x3940
	unsigned long m_texStageState[2][6];                    // 0x3944
	unsigned long m_lastBlendState[2];                      // 0x3974
	unsigned long m_blendEnable;                            // 0x397c
	unsigned long m_lastVtxType;                            // 0x3980
	unsigned long m_vtxSize;                                // 0x3984
	static FMTLIST ms_fmtList[5];
	static pix_info ms_fmtTypeList[9];
	d3d8_texture m_texList[2048];                           // 0x3988
	sDepthSurf m_depthSurfList[32];                         // 0x23988
	IDirect3DSurface9* m_commonDepthSurf;                   // 0x23a88
	static _D3DFORMAT ms_fmtRecomList[6][4];
	static unsigned long ms_ZbuffHistogram[256];
	float m_fps;                                            // 0x23a8c
	bool m_bWindow;                                         // 0x23a90
	pix_info* m_fmt[6];                                     // 0x23a94
	IDirect3D9* m_d3d8;                                     // 0x23aac
	IDirect3DDevice9* m_pd3dDevice;                         // 0x23ab0
	_D3DCAPS9 m_d3dcaps;                                    // 0x23ab4
	_D3DPRESENT_PARAMETERS_ m_d3dpp;                        // 0x23be4
	_D3DFORMAT m_fmtWindowed;                               // 0x23c1c
	char* m_devName;                                        // 0x23c20
	char* m_modList;                                        // 0x23c24
	int m_devId;                                            // 0x23c28
	HWND__* m_hwnd;                                         // 0x23c2c
	long m_lWndStyle;                                       // 0x23c30
	bool m_clientRcCheckedAfterReset;                       // 0x23c34
	IDirect3DQuery9* m_pEventQuery;                         // 0x23c38
	Nv::IScreenCape* m_pScreenCape;                         // 0x23c3c
	Nv::Factory::ScreenCapeFactory* m_pScreenCapeFactory;   // 0x23c40
private:
	unsigned long m_xdwDevBehavior;                         // 0x23c44
	int m_xiVbSize;                                         // 0x23c48
	sVb8 m_xaVbList[1024];                                  // 0x23c4c
	sIb8 m_xaIbList[256];                                   // 0x2ac4c
	bool m_xbCurHwTnL;                                      // 0x2c04c
	bool m_xbLastHwTnL;                                     // 0x2c04d
	unsigned long m_xLastDrawType;                          // 0x2c050
	unsigned long m_xLastRenderState;                       // 0x2c054
	unsigned long m_xLastVertexDecl;                        // 0x2c058
	D3DXMATRIX m_xLastWorldMatrix[256];                     // 0x2c05c
	D3DXMATRIX m_xLastViewMatrix;                           // 0x3005c
	D3DXMATRIX m_xLastProjMatrix;                           // 0x3009c
	D3DXMATRIX m_xPrevViewMatrix;                           // 0x300dc
	unsigned long m_LastTFactor;                            // 0x3011c
	float m_LastFogStart;                                   // 0x30120
	float m_LastFogEnd;                                     // 0x30124
	_D3DVECTOR m_LightDirect;                               // 0x30128
	int m_xhLastVb;                                         // 0x30134
	int m_xhLastIb;                                         // 0x30138
	unsigned long m_xdwLastUsage;                           // 0x3013c
	unsigned long m_xdwLastFVF;                             // 0x30140
	unsigned long m_xdwTFactor;                             // 0x30144
	bool m_xbUseTFactor;                                    // 0x30148
	bool m_xbLastVertexBlend;                               // 0x30149
	int m_xnTotalTris;                                      // 0x3014c
	int m_xnDPUPs;                                          // 0x30150
	int m_xnDIPUPs;                                         // 0x30154
	int m_xnDPs;                                            // 0x30158
	int m_xnDIPs;                                           // 0x3015c
	LightSet m_xaWLights[4];                                // 0x30160
	_D3DLIGHT9 m_xaLights[4];                               // 0x301d0
	_D3DMATERIAL9 m_xMaterial;                              // 0x30370
	bool m_LightEnable;                                     // 0x303b4
	unsigned long m_xdwDiffuse;                             // 0x303b8
	unsigned int m_iDDSRes;                                 // 0x303bc
	bool m_bUseMipmap;                                      // 0x303c0
	unsigned int m_iMaxMipLvl;                              // 0x303c4
	unsigned long m_dwMipCreateFilter;                      // 0x303c8
	unsigned long m_dwMipTexStateFilter;                    // 0x303cc
	std::map<int, sVertexDecl> m_VertexDecl;                // 0x303d0
	std::string m_shadersrc;                                // 0x303dc
	unsigned long m_Effect;                                 // 0x303f8
	unsigned long m_lastEffect;                             // 0x303fc
	std::map<unsigned long, sEffect> m_EffectTable;         // 0x30400
	float m_SHCoeff[7][4];                                  // 0x3040c
	unsigned long m_GlobalRS[2];                            // 0x3047c
	sMergeBuffer m_MergeBuffer;                             // 0x30484
	IDirect3DVertexBuffer9* m_pLockableVB;                  // 0x304fc
	IDirect3DIndexBuffer9* m_pLockableIB;                   // 0x30500
	int m_iVBOffset;                                        // 0x30504
	int m_iIBOffset;                                        // 0x30508
protected:
	WFxParamPool m_fxParamPool;                             // 0x3050c
	WRenderState m_CustomRenderState;                       // 0x305a0
	std::stack<WRenderState> m_CustomRenderStateBackupList; // 0x30664
	std::list<WRenderState> m_CustomRenderStateSnapShotList; // 0x30678
	nsWindowUtility::CDWMApiDll* m_pkDWMApiDll;             // 0x30684
	bool m_fillScrMode;                                     // 0x30688
};

CHECK_SIZE(WDirect3D8::sRtBackup, 0x24);
CHECK_SIZE(WDirect3D8::sRtFormat, 0x8);
CHECK_SIZE(WDirect3D8::FMTLIST, 0xc);
CHECK_SIZE(WDirect3D8::pix_info, 0x28);
CHECK_SIZE(WDirect3D8::d3d8_texture, 0x40);
CHECK_FIELD(WDirect3D8::d3d8_texture, renderTargetSizeInfo, 0x30);
CHECK_FIELD(WDirect3D8::d3d8_texture, isFilled, 0x3c);
CHECK_SIZE(WDirect3D8::sDepthSurf, 0x8);
CHECK_SIZE(WDirect3D8::sVb8, 0x1c);
CHECK_FIELD(WDirect3D8::sVb8, xpVertexData, 0x18);
CHECK_SIZE(WDirect3D8::sIb8, 0x14);
CHECK_SIZE(WDirect3D8::sVertexDecl, 0x8);
CHECK_SIZE(WDirect3D8::sEffect, 0xac);
CHECK_SIZE(WDirect3D8::sEffect::Param, 0x8);
CHECK_FIELD(WDirect3D8::sEffect::Param, isShared, 0x4);
CHECK_FIELD(WDirect3D8::sEffect, param, 0x4);
CHECK_SIZE(WDirect3D8::sMergeBuffer, 0x78);
CHECK_FIELD(WDirect3D8::sMergeBuffer, m_xLastMatrix, 0x28);
CHECK_FIELD(WDirect3D8::sMergeBuffer, IndexBufferSize, 0x74);
CHECK_SIZE(WDirect3D8::WFxParamPool, 0x94);
CHECK_SIZE(WDirect3D8::WRenderState, 0xc4);
CHECK_SIZE(WDirect3D8, 0x3068c);
BEGIN_LAYOUT_CHECK(WDirect3D8::WFxParamPool)
	CHECK_FIELD(WDirect3D8::WFxParamPool, m_paramCacheListPair_Int, 0x4);
	CHECK_FIELD(WDirect3D8::WFxParamPool, m_paramCacheListPair_Tex, 0x7c);
END_LAYOUT_CHECK
CHECK_SIZE(WDirect3D8::WRenderState::sRs, 0x10);
CHECK_SIZE(WDirect3D8::WRenderState::sTss, 0x14);
CHECK_SIZE(WDirect3D8::WRenderState::sTransf, 0x88);
CHECK_SIZE(WDirect3D8::sClipPlane, 0x38);
CHECK_FIELD(WDirect3D8::sClipPlane, m_SetValue, 0x28);
BEGIN_LAYOUT_CHECK(WDirect3D8::WRenderState)
	CHECK_FIELD(WDirect3D8::WRenderState, m_tssList, 0x10);
	CHECK_FIELD(WDirect3D8::WRenderState, m_ssList, 0x50);
	CHECK_FIELD(WDirect3D8::WRenderState, m_fxMacro, 0x60);
	CHECK_FIELD(WDirect3D8::WRenderState, m_fxParamIntList, 0x64);
	CHECK_FIELD(WDirect3D8::WRenderState, m_fxParamTexList, 0xb4);
END_LAYOUT_CHECK
BEGIN_LAYOUT_CHECK(WDirect3D8)
	CHECK_FIELD(WDirect3D8, m_pTexture, 0x3890);
	CHECK_FIELD(WDirect3D8, m_rtBackupList, 0x3898);
	CHECK_FIELD(WDirect3D8, m_mainRt, 0x38ac);
	CHECK_FIELD(WDirect3D8, m_RtDepthFmtIdx, 0x3904);
	CHECK_FIELD(WDirect3D8, m_capturedDdsd, 0x3918);
	CHECK_FIELD(WDirect3D8, m_vtxSize, 0x3984);
	CHECK_FIELD(WDirect3D8, m_texList, 0x3988);
	CHECK_FIELD(WDirect3D8, m_commonDepthSurf, 0x23a88);
	CHECK_FIELD(WDirect3D8, m_d3dcaps, 0x23ab4);
	CHECK_FIELD(WDirect3D8, m_d3dpp, 0x23be4);
	CHECK_FIELD(WDirect3D8, m_pScreenCapeFactory, 0x23c40);
	CHECK_FIELD(WDirect3D8, m_xaVbList, 0x23c4c);
	CHECK_FIELD(WDirect3D8, m_xaIbList, 0x2ac4c);
	CHECK_FIELD(WDirect3D8, m_xLastWorldMatrix, 0x2c05c);
	CHECK_FIELD(WDirect3D8, m_LightDirect, 0x30128);
	CHECK_FIELD(WDirect3D8, m_xaWLights, 0x30160);
	CHECK_FIELD(WDirect3D8, m_xaLights, 0x301d0);
	CHECK_FIELD(WDirect3D8, m_xMaterial, 0x30370);
	CHECK_FIELD(WDirect3D8, m_dwMipTexStateFilter, 0x303cc);
	CHECK_FIELD(WDirect3D8, m_VertexDecl, 0x303d0);
	CHECK_FIELD(WDirect3D8, m_shadersrc, 0x303dc);
	CHECK_FIELD(WDirect3D8, m_EffectTable, 0x30400);
	CHECK_FIELD(WDirect3D8, m_MergeBuffer, 0x30484);
	CHECK_FIELD(WDirect3D8, m_iIBOffset, 0x30508);
	CHECK_FIELD(WDirect3D8, m_fxParamPool, 0x3050c);
	CHECK_FIELD(WDirect3D8, m_CustomRenderState, 0x305a0);
	CHECK_FIELD(WDirect3D8, m_CustomRenderStateBackupList, 0x30664);
	CHECK_FIELD(WDirect3D8, m_CustomRenderStateSnapShotList, 0x30678);
	CHECK_FIELD(WDirect3D8, m_pkDWMApiDll, 0x30684);
	CHECK_FIELD(WDirect3D8, m_fillScrMode, 0x30688);
END_LAYOUT_CHECK

#endif   // PG_CLIENT_DEVICE_WD3D8_WD3D8_H
