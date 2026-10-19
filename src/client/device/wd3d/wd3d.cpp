// Original: d:\build\custom\temp\code\client\device\wd3d\wd3d.cpp
#include <stdlib.h>
#include <new>

#include "wd3d.h"

#ifdef PG_MATERIAL_HINTS
// Port only (port/compat/pg_material_hints.h): each buffered polygon keeps the
// material hint it was drawn with and sets it again when it is flushed. Side
// tables by the polygon's index in m_bufPolySw / m_bufPolyHw (the layout
// stays the original's).
#include <vector>
#include "pg_material_hints.h"

static std::vector<unsigned char> s_pgHintSw, s_pgHintHw;

static void PgHintKeep(std::vector<unsigned char>& hints, size_t index, size_t capacity) {
	if (hints.size() < capacity) {
		hints.resize(capacity, 0);
	}
	if (index < hints.size()) {
		hints[index] = (unsigned char)PgMaterialHintGet();
	}
}

static int PgHintOf(const std::vector<unsigned char>& hints, size_t index) {
	return index < hints.size() ? hints[index] : 0;
}
#endif

// wd3d.cpp:39
WDirect3D::WDirect3D() {
	m_mainThreadId = 0;
	m_chVertex = 0;
	m_dwOffset = 0;
	m_bufSortNum = 0;
	m_bufPolyHwNum = 0;
	m_bufPolySwNum = 0;
	m_texCount1 = 128;
	m_texCount2 = 1;
	m_useHiQualityTex = false;
	for (int i = 0; i < 2048; i++) {
		m_texList[i] = TS_VOID;
	}

	m_MaxTextureBlendStages = 0;
	ApplyGamma(1.3f);

	m_fog = false;
	m_fogColor = 0;
	m_fogStart = 0;
	m_fogEnd = 0;

	m_xiVbCount = m_xiIbCount = 1;
	memset(m_xahVbs, 0, sizeof(m_xahVbs));
	memset(m_xahIbs, 0, sizeof(m_xahIbs));
	// Handle 0 stays reserved.
	m_xahVbs[0] = 1;
	m_xahIbs[0] = 1;
}

// wd3d.cpp:78
WDirect3D::~WDirect3D() {
	Release();
}

// wd3d.cpp:83
void WDirect3D::Init() {
	m_chVertex = new char[0x100000];
	m_devState = W_VDEVSTATE_NORMAL;

	m_buf4sort.resize(0x1200);
	m_bufPolySw.resize(0x1000);
	m_bufPolyHw.resize(0x200);
}

// wd3d.cpp:120
void WDirect3D::Release() {
	int i;

	for (i = 0; i < sizeof(m_texList) / sizeof(m_texList[0]); i++) {
		if (m_texList[i] != TS_VOID) {
			DestroyTexture(i);
			m_texList[i] = TS_VOID;
		}
	}

	for (i = 1; i < sizeof(m_xahVbs) / sizeof(m_xahVbs[0]); i++) {
		if (m_xahVbs[i]) {
			xReleaseVertexBuffer(i);
			m_xahVbs[i] = 0;
		}
	}

	for (i = 1; i < 256; i++) {
		if (m_xahIbs[i]) {
			xReleaseIndexBuffer(i);
			m_xahIbs[i] = 0;
		}
	}

	if (m_chVertex) {
		delete[] m_chVertex;
		m_chVertex = 0;
	}
}

// wd3d.cpp:158
// Copies n vertices into the vertex buffer in the layout of the format
// dwVertexTypeDesc (bit 31: write the fog factor into the specular alpha) and
// returns where they start; store keeps them (advances the offset).
void* WDirect3D::ModifyVertices(WTVertex** vl, int n, unsigned long dwVertexTypeDesc, bool store) {
	int offset = m_dwOffset;
	int i;

	// Flush before the buffer (0x100000 bytes) is 80% full.
	if (offset >= 0xCCCCC) {
		Flush(0);
		offset = 0;
	}

	void* v = m_chVertex + offset;

	switch (dwVertexTypeDesc) {
	case D3DFVF_XYZRHW | D3DFVF_DIFFUSE:
		for (i = 0; i < n; i++, offset += 0x14) {
			memcpy(m_chVertex + offset, vl[i], 20);
		}
		break;
	case 0x80000000 | D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR:
		for (i = 0; i < n; i++, offset += 0x18) {
			memcpy(m_chVertex + offset, vl[i], 20);
			(m_chVertex + offset)[0x17] = CalcFog(*(float*)(m_chVertex + offset + 8));
		}
		break;
	case D3DFVF_XYZRHW | D3DFVF_TEX1:
		for (i = 0; i < n; i++, offset += 0x18) {
			memcpy(m_chVertex + offset, vl[i], 16);
			memcpy(m_chVertex + offset + 0x10, &vl[i]->tu, 8);
		}
		break;
	case 0x80000000 | D3DFVF_XYZRHW | D3DFVF_SPECULAR | D3DFVF_TEX1:
		for (i = 0; i < n; i++, offset += 0x1c) {
			memcpy(m_chVertex + offset, vl[i], 16);
			memcpy(m_chVertex + offset + 0x14, &vl[i]->tu, 8);
			(m_chVertex + offset)[0x13] = CalcFog(*(float*)(m_chVertex + offset + 8));
		}
		break;
	case 0:
		for (i = 0; i < n; i++, offset += 0x18) {
			memcpy(m_chVertex + offset, vl[i], 16);
			memcpy(m_chVertex + offset + 0x10, &vl[i]->lv, 8);
		}
		break;
	case D3DFVF_DIFFUSE:
		for (i = 0; i < n; i++, offset += 0x1c) {
			memcpy(m_chVertex + offset, vl[i], 20);
			memcpy(m_chVertex + offset + 0x14, &vl[i]->lv, 8);
		}
		break;
	case 0x80000000 | D3DFVF_SPECULAR:
		for (i = 0; i < n; i++, offset += 0x1c) {
			memcpy(m_chVertex + offset, vl[i], 16);
			memcpy(m_chVertex + offset + 0x14, &vl[i]->lv, 8);
			(m_chVertex + offset)[0x13] = CalcFog(*(float*)(m_chVertex + offset + 8));
		}
		break;
	case D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2:
		for (i = 0; i < n; i++, offset += 0x24) {
			memcpy(m_chVertex + offset, vl[i], 20);
			memcpy(m_chVertex + offset + 0x14, &vl[i]->tu, 16);
		}
		break;
	case D3DFVF_XYZRHW | D3DFVF_TEX2:
		for (i = 0; i < n; i++, offset += 0x20) {
			memcpy(m_chVertex + offset, vl[i], 16);
			memcpy(m_chVertex + offset + 0x10, &vl[i]->tu, 16);
		}
		break;
	case 0x80000000 | D3DFVF_XYZRHW | D3DFVF_SPECULAR | D3DFVF_TEX2:
		for (i = 0; i < n; i++, offset += 0x24) {
			memcpy(m_chVertex + offset, vl[i], 16);
			memcpy(m_chVertex + offset + 0x14, &vl[i]->tu, 16);
			(m_chVertex + offset)[0x13] = CalcFog(*(float*)(m_chVertex + offset + 8));
		}
		break;
	case D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1:
		for (i = 0; i < n; i++, offset += 0x1c) {
			memcpy(m_chVertex + offset, vl[i], 20);
			memcpy(m_chVertex + offset + 0x14, &vl[i]->tu, 8);
		}
		break;
	case 0x80000000 | D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1:
		for (i = 0; i < n; i++, offset += 0x20) {
			memcpy(m_chVertex + offset, vl[i], 20);
			memcpy(m_chVertex + offset + 0x18, &vl[i]->tu, 8);
			(m_chVertex + offset)[0x17] = CalcFog(*(float*)(m_chVertex + offset + 8));
		}
		break;
	case 0x80000000 | D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2:
		for (i = 0; i < n; i++, offset += 0x28) {
			memcpy(m_chVertex + offset, vl[i], 20);
			memcpy(m_chVertex + offset + 0x18, &vl[i]->tu, 16);
			(m_chVertex + offset)[0x17] = CalcFog(*(float*)(m_chVertex + offset + 8));
		}
		break;
	case D3DFVF_XYZ | D3DFVF_DIFFUSE:
		for (i = 0; i < n; i++, offset += 0x10) {
			memcpy(m_chVertex + offset, vl[i], 12);
			memcpy(m_chVertex + offset + 0xc, &vl[i]->diffuse, 4);
		}
		break;
	case 0x80000000 | D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR:
		for (i = 0; i < n; i++, offset += 0x14) {
			memcpy(m_chVertex + offset, vl[i], 12);
			memcpy(m_chVertex + offset + 0xc, &vl[i]->diffuse, 4);
		}
		break;
	case D3DFVF_XYZ | D3DFVF_TEX1:
		for (i = 0; i < n; i++, offset += 0x14) {
			memcpy(m_chVertex + offset, vl[i], 12);
			memcpy(m_chVertex + offset + 0xc, &vl[i]->tu, 8);
		}
		break;
	case 0x80000000 | D3DFVF_XYZ | D3DFVF_SPECULAR | D3DFVF_TEX1:
		for (i = 0; i < n; i++, offset += 0x18) {
			memcpy(m_chVertex + offset, vl[i], 12);
			memcpy(m_chVertex + offset + 0x10, &vl[i]->tu, 8);
		}
		break;
	case D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2:
		for (i = 0; i < n; i++, offset += 0x20) {
			memcpy(m_chVertex + offset, vl[i], 12);
			memcpy(m_chVertex + offset + 0xc, &vl[i]->diffuse, 4);
			memcpy(m_chVertex + offset + 0x10, &vl[i]->tu, 16);
		}
		break;
	case D3DFVF_XYZ | D3DFVF_TEX2:
		for (i = 0; i < n; i++, offset += 0x1c) {
			memcpy(m_chVertex + offset, vl[i], 12);
			memcpy(m_chVertex + offset + 0xc, &vl[i]->tu, 16);
		}
		break;
	case 0x80000000 | D3DFVF_XYZ | D3DFVF_SPECULAR | D3DFVF_TEX2:
		for (i = 0; i < n; i++, offset += 0x20) {
			memcpy(m_chVertex + offset, vl[i], 12);
			memcpy(m_chVertex + offset + 0x10, &vl[i]->tu, 16);
		}
		break;
	case D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1:
		for (i = 0; i < n; i++, offset += 0x18) {
			memcpy(m_chVertex + offset, vl[i], 12);
			memcpy(m_chVertex + offset + 0xc, &vl[i]->diffuse, 4);
			memcpy(m_chVertex + offset + 0x10, &vl[i]->tu, 8);
		}
		break;
	case 0x80000000 | D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1:
		for (i = 0; i < n; i++, offset += 0x1c) {
			memcpy(m_chVertex + offset, vl[i], 12);
			memcpy(m_chVertex + offset + 0xc, &vl[i]->diffuse, 4);
			memcpy(m_chVertex + offset + 0x14, &vl[i]->tu, 8);
		}
		break;
	case 0x80000000 | D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2:
		for (i = 0; i < n; i++, offset += 0x24) {
			memcpy(m_chVertex + offset, vl[i], 12);
			memcpy(m_chVertex + offset + 0xc, &vl[i]->diffuse, 4);
			memcpy(m_chVertex + offset + 0x14, &vl[i]->tu, 16);
			(m_chVertex + offset)[0x13] = CalcFog(*(float*)(m_chVertex + offset + 8));
		}
		break;
	}

	if (store) {
		m_dwOffset = offset;
	}

	return v;
}

// wd3d.cpp:394
void* WDirect3D::ModifyVertices(WTVertex* vl, int n, unsigned long dwVertexTypeDesc, bool store) {
	int offset = m_dwOffset;
	int i;

	// Flush before the buffer (0x100000 bytes) is 80% full.
	if (offset >= 0xCCCCC) {
		Flush(0);
		offset = 0;
	}

	void* v = m_chVertex + offset;

	switch (dwVertexTypeDesc) {
	case D3DFVF_XYZRHW | D3DFVF_DIFFUSE:
		for (i = 0; i < n; i++, offset += 0x14) {
			memcpy(m_chVertex + offset, &vl[i], 20);
		}
		break;
	case 0x80000000 | D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR:
		for (i = 0; i < n; i++, offset += 0x18) {
			memcpy(m_chVertex + offset, &vl[i], 20);
			(m_chVertex + offset)[0x17] = CalcFog(*(float*)(m_chVertex + offset + 8));
		}
		break;
	case D3DFVF_XYZRHW | D3DFVF_TEX1:
		for (i = 0; i < n; i++, offset += 0x18) {
			memcpy(m_chVertex + offset, &vl[i], 16);
			memcpy(m_chVertex + offset + 0x10, &vl[i].tu, 8);
		}
		break;
	case 0x80000000 | D3DFVF_XYZRHW | D3DFVF_SPECULAR | D3DFVF_TEX1:
		for (i = 0; i < n; i++, offset += 0x1c) {
			memcpy(m_chVertex + offset, &vl[i], 16);
			memcpy(m_chVertex + offset + 0x14, &vl[i].tu, 8);
			(m_chVertex + offset)[0x13] = CalcFog(*(float*)(m_chVertex + offset + 8));
		}
		break;
	case 0:
		for (i = 0; i < n; i++, offset += 0x18) {
			memcpy(m_chVertex + offset, &vl[i], 16);
			memcpy(m_chVertex + offset + 0x10, &vl[i].lv, 8);
		}
		break;
	case D3DFVF_DIFFUSE:
		for (i = 0; i < n; i++, offset += 0x1c) {
			memcpy(m_chVertex + offset, &vl[i], 20);
			memcpy(m_chVertex + offset + 0x14, &vl[i].lv, 8);
		}
		break;
	case 0x80000000 | D3DFVF_SPECULAR:
		for (i = 0; i < n; i++, offset += 0x1c) {
			memcpy(m_chVertex + offset, &vl[i], 16);
			memcpy(m_chVertex + offset + 0x14, &vl[i].lv, 8);
			(m_chVertex + offset)[0x13] = CalcFog(*(float*)(m_chVertex + offset + 8));
		}
		break;
	case D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2:
		for (i = 0; i < n; i++, offset += 0x24) {
			memcpy(m_chVertex + offset, &vl[i], 20);
			memcpy(m_chVertex + offset + 0x14, &vl[i].tu, 16);
		}
		break;
	case D3DFVF_XYZRHW | D3DFVF_TEX2:
		for (i = 0; i < n; i++, offset += 0x20) {
			memcpy(m_chVertex + offset, &vl[i], 16);
			memcpy(m_chVertex + offset + 0x10, &vl[i].tu, 16);
		}
		break;
	case 0x80000000 | D3DFVF_XYZRHW | D3DFVF_SPECULAR | D3DFVF_TEX2:
		for (i = 0; i < n; i++, offset += 0x24) {
			memcpy(m_chVertex + offset, &vl[i], 16);
			memcpy(m_chVertex + offset + 0x14, &vl[i].tu, 16);
			(m_chVertex + offset)[0x13] = CalcFog(*(float*)(m_chVertex + offset + 8));
		}
		break;
	case D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1:
		for (i = 0; i < n; i++, offset += 0x1c) {
			memcpy(m_chVertex + offset, &vl[i], 20);
			memcpy(m_chVertex + offset + 0x14, &vl[i].tu, 8);
		}
		break;
	case 0x80000000 | D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1:
		for (i = 0; i < n; i++, offset += 0x20) {
			memcpy(m_chVertex + offset, &vl[i], 20);
			memcpy(m_chVertex + offset + 0x18, &vl[i].tu, 8);
			(m_chVertex + offset)[0x17] = CalcFog(*(float*)(m_chVertex + offset + 8));
		}
		break;
	case 0x80000000 | D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2:
		for (i = 0; i < n; i++, offset += 0x28) {
			memcpy(m_chVertex + offset, &vl[i], 20);
			memcpy(m_chVertex + offset + 0x18, &vl[i].tu, 16);
			(m_chVertex + offset)[0x17] = CalcFog(*(float*)(m_chVertex + offset + 8));
		}
		break;
	case D3DFVF_XYZ | D3DFVF_DIFFUSE:
		for (i = 0; i < n; i++, offset += 0x10) {
			memcpy(m_chVertex + offset, &vl[i], 12);
			memcpy(m_chVertex + offset + 0xc, &vl[i].diffuse, 4);
		}
		break;
	case 0x80000000 | D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR:
		for (i = 0; i < n; i++, offset += 0x14) {
			memcpy(m_chVertex + offset, &vl[i], 12);
			memcpy(m_chVertex + offset + 0xc, &vl[i].diffuse, 4);
		}
		break;
	case D3DFVF_XYZ | D3DFVF_TEX1:
		for (i = 0; i < n; i++, offset += 0x14) {
			memcpy(m_chVertex + offset, &vl[i], 12);
			memcpy(m_chVertex + offset + 0xc, &vl[i].tu, 8);
		}
		break;
	case 0x80000000 | D3DFVF_XYZ | D3DFVF_SPECULAR | D3DFVF_TEX1:
		for (i = 0; i < n; i++, offset += 0x18) {
			memcpy(m_chVertex + offset, &vl[i], 12);
			memcpy(m_chVertex + offset + 0x10, &vl[i].tu, 8);
		}
		break;
	case D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2:
		for (i = 0; i < n; i++, offset += 0x20) {
			memcpy(m_chVertex + offset, &vl[i], 12);
			memcpy(m_chVertex + offset + 0xc, &vl[i].diffuse, 4);
			memcpy(m_chVertex + offset + 0x10, &vl[i].tu, 16);
		}
		break;
	case D3DFVF_XYZ | D3DFVF_TEX2:
		for (i = 0; i < n; i++, offset += 0x1c) {
			memcpy(m_chVertex + offset, &vl[i], 12);
			memcpy(m_chVertex + offset + 0xc, &vl[i].tu, 16);
		}
		break;
	case 0x80000000 | D3DFVF_XYZ | D3DFVF_SPECULAR | D3DFVF_TEX2:
		for (i = 0; i < n; i++, offset += 0x20) {
			memcpy(m_chVertex + offset, &vl[i], 12);
			memcpy(m_chVertex + offset + 0x10, &vl[i].tu, 16);
		}
		break;
	case D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1:
		for (i = 0; i < n; i++, offset += 0x18) {
			memcpy(m_chVertex + offset, &vl[i], 12);
			memcpy(m_chVertex + offset + 0xc, &vl[i].diffuse, 4);
			memcpy(m_chVertex + offset + 0x10, &vl[i].tu, 8);
		}
		break;
	case 0x80000000 | D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1:
		for (i = 0; i < n; i++, offset += 0x1c) {
			memcpy(m_chVertex + offset, &vl[i], 12);
			memcpy(m_chVertex + offset + 0xc, &vl[i].diffuse, 4);
			memcpy(m_chVertex + offset + 0x14, &vl[i].tu, 8);
		}
		break;
	case 0x80000000 | D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2:
		for (i = 0; i < n; i++, offset += 0x24) {
			memcpy(m_chVertex + offset, &vl[i], 12);
			memcpy(m_chVertex + offset + 0xc, &vl[i].diffuse, 4);
			memcpy(m_chVertex + offset + 0x14, &vl[i].tu, 16);
			// No fog factor here, unlike the WTVertex** overload.
		}
		break;
	}

	if (store) {
		m_dwOffset = offset;
	}

	return v;
}

// wd3d.cpp:630
void WDirect3D::ApplyGamma(float gamma) {
	for (int i = 0; i < 256; i++) {
		m_iRefTable[i] = Between(0, (int)(pow(i / 256.0, 1.0 / gamma) * 256.0), 255);
	}
}

// wd3d.cpp:641
bool WDirect3D::SetFogEnable(bool enable) {
	bool prev = m_fog;
	m_fog = enable;
	return prev;
}

// wd3d.cpp:648
void WDirect3D::SetFogState(float fogStart, float fogEnd, unsigned long color) {
	m_fogStart = fogStart;
	m_fogEnd = fogEnd;
	m_fogColor = color & 0xffffff;
}

// wd3d.cpp:655
unsigned char WDirect3D::CalcFog(float depth) {
	if (m_clip_near_scale == 0.0f) {
		m_clip_near_scale = 0.1f;
	}

	depth = m_clip_scale_z - depth;
	if (depth == 0.0f) {
		depth = 1e-7f;
	}
	float d = m_clip_near_scale / depth;

	if (d > m_fogEnd) {
		return 0;
	}
	if (d <= m_fogStart) {
		return 0xff;
	}
	return (unsigned char)(unsigned long)(int)((m_fogEnd - d) * 255.0f / (m_fogEnd - m_fogStart));
}

// wd3d.cpp:742
float WDirect3D::CalcDepth(WTVertex** p, int num) {
	float depth = 0.0f;

	for (int i = 0; i < num; i++) {
		depth += p[i]->vz;
	}
	return depth / num;
}

// wd3d.cpp:877
void WDirect3D::DrawPolygonFan(WTVertex** p, int iType, int iNum, int iType2, unsigned long dwVertexTypeDesc) {
	if (m_devState == W_VDEVSTATE_LOST) {
		return;
	}

	WTVertex* t[64];
	_D3DPRIMITIVETYPE dptPrimitiveType;

	if (iType & 0x4000000) {
		// Wireframe: the outline as a closed line strip.
		if (iNum == 2) {
			dptPrimitiveType = D3DPT_LINESTRIP;
		} else {
			int i;
			for (i = 0; i < iNum; i++) {
				t[i] = p[i];
			}
			t[i] = p[0];

			p = t;
			iNum++;

			dptPrimitiveType = D3DPT_LINESTRIP;
		}
	} else {
		dptPrimitiveType = D3DPT_TRIANGLEFAN;
	}

	dwVertexTypeDesc |= ((iType & 0x40000000) ? 0 : D3DFVF_DIFFUSE) | ((iType & 0x3f800) ? D3DFVF_TEX2 : ((iType & 0x7ff) ? D3DFVF_TEX1 : 0));

	if (iType & 0x8000000) {
		if (m_fog) {
			dwVertexTypeDesc |= 0x80000000 | D3DFVF_SPECULAR;
		} else {
			iType &= ~0x8000000;
		}
	}

	if (iType & 0x400000) {
		// Buffered, sorted by depth.
		Buffering(iType, iType2, iNum, dwVertexTypeDesc, ModifyVertices(p, iNum, dwVertexTypeDesc, true), dptPrimitiveType, ((iType & 0x300000) == 0x300000 || (iType & 0x300000) == 0x100000) ? 0.0f : CalcDepth(p, iNum));
	} else if ((iType & 0x3f800) && m_MaxTextureBlendStages == 1) {
		// One texture stage only: the second texture as an extra pass.
		if (!(dwVertexTypeDesc & 0x80000000)) {
			if (iType & 0x7ff) {
				if (dwVertexTypeDesc & D3DFVF_DIFFUSE) {
					DrawPrimitive(iType & 0xfff807ff, iNum, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, ModifyVertices(p, iNum, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, false), dptPrimitiveType, iType2);
				} else {
					DrawPrimitive(iType & 0xfff807ff, iNum, D3DFVF_XYZRHW | D3DFVF_TEX1, ModifyVertices(p, iNum, D3DFVF_XYZRHW | D3DFVF_TEX1, false), dptPrimitiveType, iType2);
				}

				if (iType & 0x40000) {
					if (dwVertexTypeDesc & D3DFVF_DIFFUSE) {
						Buffering(((iType >> 11) & 0x7f) | 0x20900000, 0, iNum, D3DFVF_XYZRHW | D3DFVF_TEX1, ModifyVertices(p, iNum, 0, true), dptPrimitiveType, 0.0f);
					} else {
						Buffering(((iType >> 11) & 0x7f) | 0x60900000, 0, iNum, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, ModifyVertices(p, iNum, D3DFVF_DIFFUSE, true), dptPrimitiveType, 0.0f);
					}
				} else {
					Buffering(((iType >> 11) & 0x7f) | 0x61100000, 0, iNum, D3DFVF_XYZRHW | D3DFVF_TEX1, ModifyVertices(p, iNum, 0, true), dptPrimitiveType, 0.0f);
				}
			} else {
				if (dwVertexTypeDesc & D3DFVF_DIFFUSE) {
					DrawPrimitive((iType >> 11) & 0x7f, iNum, D3DFVF_XYZRHW | D3DFVF_TEX1, ModifyVertices(p, iNum, 0, true), dptPrimitiveType, iType2);
				} else {
					DrawPrimitive(((iType >> 11) & 0x7f) | 0x40000000, iNum, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, ModifyVertices(p, iNum, D3DFVF_DIFFUSE, true), dptPrimitiveType, iType2);
				}
			}
		} else {
			if (iType & 0x3ffff) {
				DrawPrimitive(iType & 0xfffc07ff, iNum, D3DFVF_XYZRHW | D3DFVF_SPECULAR | D3DFVF_TEX1, ModifyVertices(p, iNum, 0x80000000 | D3DFVF_XYZRHW | D3DFVF_SPECULAR | D3DFVF_TEX1, false), dptPrimitiveType, iType2);
			}

			unsigned char* v = (unsigned char*)ModifyVertices(p, iNum, 0x80000000 | D3DFVF_SPECULAR, true);

			// The fog pass only when a vertex is fogged (specular alpha below 0xff).
			for (int i = 0; i < iNum; i++) {
				if (v[i * 0x1c + 0x13] != 0xff) {
					Buffering(((iType >> 11) & 0x7f) | 0x69100000, 0, iNum, D3DFVF_XYZRHW | D3DFVF_SPECULAR | D3DFVF_TEX1, v, dptPrimitiveType, 0.0f);
					break;
				}
			}
		}
	} else {
		DrawPrimitive(iType, iNum, dwVertexTypeDesc, ModifyVertices(p, iNum, dwVertexTypeDesc, false), dptPrimitiveType, iType2);
	}
}

// wd3d.cpp:1045
void WDirect3D::DrawIndexedTriangles(WTVertex* p, int pnum, unsigned short* flist, int fnum, int iType, int iType2) {
	if (m_devState == W_VDEVSTATE_LOST) {
		return;
	}

	unsigned long dwVertexTypeDesc = D3DFVF_XYZ | ((iType & 0x40000000) ? 0 : D3DFVF_DIFFUSE) | ((iType & 0x3f800) ? D3DFVF_TEX2 : ((iType & 0x7ff) ? D3DFVF_TEX1 : 0));

	if (iType & 0x8000000) {
		if (m_fog) {
			dwVertexTypeDesc |= 0x80000000 | D3DFVF_SPECULAR;
		} else {
			iType &= ~0x8000000;
		}
	}

	if (iType & 0x400000) {
		// Buffered: one triangle at a time, sorted by depth.
		for (int i = 0; i < fnum; ) {
			WTVertex* t[3];
			t[0] = &p[flist[i++]];
			t[1] = &p[flist[i++]];
			t[2] = &p[flist[i++]];

			Buffering(iType, iType2, 3, dwVertexTypeDesc, ModifyVertices(t, 3, dwVertexTypeDesc, true), D3DPT_TRIANGLEFAN, ((iType & 0x300000) == 0x300000 || (iType & 0x300000) == 0x100000) ? 0.0f : (t[0]->vz + t[1]->vz + t[2]->vz) * (1.0f / 3.0f));
		}
	} else if (!(iType & 0x4000000)) {
		DrawPrimitiveIndexed(iType, dwVertexTypeDesc, ModifyVertices(p, pnum, dwVertexTypeDesc, false), pnum, flist, fnum, iType2);
	} else {
		// Wireframe: each triangle as a closed line strip.
		for (int i = 0; i < fnum; ) {
			WTVertex* t[4];
			t[0] = &p[flist[i++]];
			t[1] = &p[flist[i++]];
			t[2] = &p[flist[i++]];
			t[3] = t[0];

			DrawPrimitive(iType, 4, dwVertexTypeDesc, ModifyVertices(t, 4, dwVertexTypeDesc, false), D3DPT_LINESTRIP, iType2);
		}
	}
}

// wd3d.cpp:1124
void WDirect3D::DrawBuffered(const w_poly_buffer_sw& bufSw) {
	ApplyCustomRenderState(bufSw.customRenderState);

#ifdef PG_MATERIAL_HINTS
	int pgHint = PgMaterialHintGet();
	PgMaterialHint(PgHintOf(s_pgHintSw, (size_t)(&bufSw - &m_bufPolySw[0])));
#endif
	DrawPrimitive(bufSw.type, bufSw.num, bufSw.dwVertexTypeDesc, bufSw.lpvVertices, bufSw.dptPrimitiveType, bufSw.type2);
#ifdef PG_MATERIAL_HINTS
	PgMaterialHint(pgHint);
#endif
}

// wd3d.cpp:1131
void WDirect3D::DrawBuffered(const w_poly_buffer_hw& bufHw) {
	ApplyCustomRenderState(bufHw.customRenderState);

#ifdef PG_MATERIAL_HINTS
	int pgHint = PgMaterialHintGet();
	PgMaterialHint(PgHintOf(s_pgHintHw, (size_t)(&bufHw - &m_bufPolyHw[0])));
#endif
	xDrawIndexedPrimitive(bufHw.VState, bufHw.BState);
#ifdef PG_MATERIAL_HINTS
	PgMaterialHint(pgHint);
#endif
}

// wd3d.cpp:1138
int WDirect3D::ComparePolyDepth(const void* elem1, const void* elem2) {
	const w_poly_buffer* buf1 = *(const w_poly_buffer* const*)elem1;
	const w_poly_buffer* buf2 = *(const w_poly_buffer* const*)elem2;

	// Far polygons first; equal depths keep their buffer order.
	if (buf1->depth > buf2->depth) {
		return -1;
	} else if (buf1->depth < buf2->depth) {
		return 1;
	}

	return buf1 < buf2 ? -1 : 1;
}

// wd3d.cpp:1156
void WDirect3D::Flush(unsigned long flag) {
	FlushRenderPrimitive();

	ClearVertices();

	if (m_devState != W_VDEVSTATE_LOST && m_bufSortNum > 0) {
		BeginUsingCustomRenderState();

		FlushEqual();

		FlushRenderPrimitive();

		// Sort the buffered polygons back to front.
		qsort(&m_buf4sort[0], m_bufSortNum, sizeof(w_poly_buffer*), ComparePolyDepth);

		if (flag & 1) {
			FlushMultiPass(flag);
		} else {
			FlushOnePass(flag);
		}

		FlushRenderPrimitive();

		FlushAlways();

		FlushRenderPrimitive();

		EndUsingCustomRenderState();
	}

	ClearCustomRenderStateSnapShotList();

	// A full buffer grows by half for the next frame.
	if (m_bufPolySwNum == GetSortBufferSwSize()) {
		m_bufPolySw.clear();
		m_bufPolySw.resize((int)(m_bufPolySwNum * 1.5f));
	}

	if (m_bufPolyHwNum == GetSortBufferHwSize()) {
		m_bufPolyHw.clear();
		m_bufPolyHw.resize(m_bufPolyHwNum * 1.5);
	}

	if (GetSortBufferSwSize() + GetSortBufferHwSize() != GetSortBufferSize()) {
		m_buf4sort.clear();
		m_buf4sort.resize(GetSortBufferSwSize() + GetSortBufferHwSize());
	}

	m_bufPolySwNum = m_bufPolyHwNum = m_bufSortNum = 0;
}

// wd3d.cpp:1223
void WDirect3D::FlushMultiPass(unsigned long flag) {
	int i;
	int startIdx;
	int endIdx;
	int increment;
	int typeToRemove;
	int typeToAdd;

	// First pass, back to front: the polygons without an alpha state only.
	bool bLastAlphaTest = SetRenderState4Flushing(0);

	for (i = m_bufSortNum - 1; i >= 0; i--) {
		if (!m_buf4sort[i]->bHw) {
			w_poly_buffer_sw* bufSw = static_cast<w_poly_buffer_sw*>(m_buf4sort[i]);
			if (bLastAlphaTest) {
				bufSw->type |= 0x80000000;
			} else {
				bufSw->type &= ~0x80000000;
			}
			if (!(bufSw->type & 0x1800000)) {
				DrawBuffered(*bufSw);
			}
		} else {
			w_poly_buffer_hw* bufHw = static_cast<w_poly_buffer_hw*>(m_buf4sort[i]);
			if (bLastAlphaTest) {
				bufHw->BState.xiFlag0 |= 0x80000000;
			} else {
				bufHw->BState.xiFlag0 &= ~0x80000000;
			}
			if (!(bufHw->BState.xiFlag0 & 0x1800000)) {
				DrawBuffered(*bufHw);
			}
		}
	}

	ApplyCustomRenderState(0);

	FlushRenderPrimitive();

	SetRenderState4Flushing(1);

	int alphaState = 0;

	if (flag & 2) {
		// Front to back.
		startIdx = m_bufSortNum - 1;
		typeToRemove = 0x20000000;
		typeToAdd = 0;
		increment = -1;
		endIdx = -1;
	} else {
		typeToRemove = 0;
		typeToAdd = 0x20000000;
		startIdx = 0;
		increment = 1;
		endIdx = m_bufSortNum;
	}

	for (i = startIdx; i != endIdx; i += increment) {
		if (!m_buf4sort[i]->bHw) {
			w_poly_buffer_sw* bufSw = static_cast<w_poly_buffer_sw*>(m_buf4sort[i]);

			if ((bufSw->type & 0x1800000) != alphaState) {
				if ((bufSw->type & 0x1800000) == 0) {
					SetRenderState4Flushing(1);
				} else {
					SetRenderState4Flushing(2);
				}
				alphaState = bufSw->type & 0x1800000;
			}

			bufSw->type = (bufSw->type & ~typeToRemove) | typeToAdd;

			DrawBuffered(*bufSw);
		} else {
			w_poly_buffer_hw* bufHw = static_cast<w_poly_buffer_hw*>(m_buf4sort[i]);

			if ((bufHw->BState.xiFlag0 & 0x1800000) != alphaState) {
				if ((bufHw->BState.xiFlag0 & 0x1800000) == 0) {
					SetRenderState4Flushing(1);
				} else {
					SetRenderState4Flushing(2);
				}
				alphaState = bufHw->BState.xiFlag0 & 0x1800000;
			}

			bufHw->BState.xiFlag0 = (bufHw->BState.xiFlag0 & ~typeToRemove) | typeToAdd;

			DrawBuffered(*bufHw);
		}
	}

	ApplyCustomRenderState(0);

	FlushRenderPrimitive();

	SetRenderState4Flushing(3);
}

// wd3d.cpp:1350
void WDirect3D::FlushOnePass(unsigned long flag) {
	int startIdx;
	int endIdx;
	int increment;
	int typeToRemove;
	int typeToAdd;

	if (flag & 2) {
		// Front to back.
		startIdx = m_bufSortNum - 1;
		typeToRemove = 0xa0000000;
		typeToAdd = 0;
		increment = -1;
		endIdx = -1;
	} else {
		typeToRemove = 0x80000000;
		typeToAdd = 0x20000000;
		startIdx = 0;
		increment = 1;
		endIdx = m_bufSortNum;
	}

	for (int i = startIdx; i != endIdx; i += increment) {
		if (!m_buf4sort[i]->bHw) {
			w_poly_buffer_sw* bufSw = static_cast<w_poly_buffer_sw*>(m_buf4sort[i]);
			bufSw->type &= ~typeToRemove;
			bufSw->type |= typeToAdd;

			DrawBuffered(*bufSw);
		} else {
			w_poly_buffer_hw* bufHw = static_cast<w_poly_buffer_hw*>(m_buf4sort[i]);
			bufHw->BState.xiFlag0 &= ~typeToRemove;
			bufHw->BState.xiFlag0 |= typeToAdd;

			DrawBuffered(*bufHw);
		}
	}

	ApplyCustomRenderState(0);

	FlushRenderPrimitive();
}

// wd3d.cpp:1404
void WDirect3D::FlushEqual() {
	int i;

	for (i = 0; i < m_bufPolySwNum; i++) {
		const w_poly_buffer_sw& bufSw = m_bufPolySw[i];

		if ((bufSw.type & 0x300000) == 0x100000) {
			DrawBuffered(bufSw);
		}
	}

	for (i = 0; i < m_bufPolyHwNum; i++) {
		const w_poly_buffer_hw& bufHw = m_bufPolyHw[i];

		if ((bufHw.BState.xiFlag0 & 0x300000) == 0x100000) {
			DrawBuffered(bufHw);
		}
	}

	ApplyCustomRenderState(0);
}

// wd3d.cpp:1429
void WDirect3D::FlushAlways() {
	int i;

	for (i = 0; i < m_bufPolySwNum; i++) {
		const w_poly_buffer_sw& bufSw = m_bufPolySw[i];

		if ((bufSw.type & 0x300000) == 0x300000) {
			DrawBuffered(bufSw);
		}
	}

	for (i = 0; i < m_bufPolyHwNum; i++) {
		const w_poly_buffer_hw& bufHw = m_bufPolyHw[i];

		if ((bufHw.BState.xiFlag0 & 0x300000) == 0x300000) {
			DrawBuffered(bufHw);
		}
	}

	ApplyCustomRenderState(0);
}

// wd3d.cpp:1454
void WDirect3D::Buffering(int type, int type2, int num, unsigned long dwVertexTypeDesc, void* p, _D3DPRIMITIVETYPE dptPrimitiveType, float depth) {
	w_poly_buffer_sw* buf = &m_bufPolySw[m_bufPolySwNum++];
	buf->bHw = false;
	buf->type = type;
	buf->type2 = type2 & ~0x1000;
	buf->num = num;
	buf->dwVertexTypeDesc = dwVertexTypeDesc;
	buf->lpvVertices = p;
	buf->dptPrimitiveType = dptPrimitiveType;
	buf->customRenderState = SnapShotCustomRenderState();
#ifdef PG_MATERIAL_HINTS
	PgHintKeep(s_pgHintSw, (size_t)(buf - &m_bufPolySw[0]), m_bufPolySw.size());
#endif

	if ((type & 0x300000) != 0x300000 && (type & 0x300000) != 0x100000) {
		buf->depth = depth;
		m_buf4sort[m_bufSortNum++] = buf;
	}

	if (m_bufPolySwNum == GetSortBufferSwSize()) {
		Flush(1);
	}
}

// wd3d.cpp:1478
void WDirect3D::Buffering(const WxViewState& VState, const WxBatchState& BState) {
	w_poly_buffer_hw* buf = &m_bufPolyHw[m_bufPolyHwNum++];
	buf->bHw = true;
	memcpy(&buf->VState, &VState, sizeof(WxViewState));
	memcpy(&buf->BState, &BState, sizeof(WxBatchState));
	buf->customRenderState = SnapShotCustomRenderState();
#ifdef PG_MATERIAL_HINTS
	PgHintKeep(s_pgHintHw, (size_t)(buf - &m_bufPolyHw[0]), m_bufPolyHw.size());
#endif

	if ((BState.xiFlag0 & 0x300000) != 0x300000 && (BState.xiFlag0 & 0x300000) != 0x100000) {
		buf->depth = BState.xfDepth;
		m_buf4sort[m_bufSortNum++] = buf;
	}

	if (m_bufPolyHwNum == GetSortBufferHwSize()) {
		Flush(1);
	}
}

// wd3d.cpp:1498
int WDirect3D::GetTextureNum(int stage) {
	int i;

	if (stage) {
		// Handles 1-127.
		for (i = 0; m_texList[m_texCount2] != TS_VOID && i < 127; i++) {
			m_texCount2 = m_texCount2 >= 127 ? 1 : m_texCount2 + 1;
		}

		m_texList[m_texCount2] = TS_CREATED;
		return m_texCount2;
	} else {
		// Handles 128-2047.
		for (i = 0; m_texList[m_texCount1] != TS_VOID && i < 1920; i++) {
			m_texCount1 = m_texCount1 >= 2047 ? 128 : m_texCount1 + 1;
		}

		m_texList[m_texCount1] = TS_CREATED;
		return m_texCount1;
	}
}

// wd3d.cpp:1522
int WDirect3D::UploadCompressedTexture(void* pSrc, unsigned int srcSize, int type) {
	int texHandle = UploadCompressedTextureSurface(pSrc, srcSize, type);

	m_texList[texHandle] = TS_UPDATED;

	return texHandle;
}

// wd3d.cpp:1532
int WDirect3D::CreateTexture(tagBITMAPINFO* src, int type) {
	int texHandle = GetTextureNum((type & 0x20000000) ? 1 : 0);

	// Bit 12: the surface is created later.
	if (!(type & 0x1000)) {
		CreateTextureSurface(src->bmiHeader.biWidth, src->bmiHeader.biHeight, (type & 0x40000000) ? 3 : 1, type & 0x1e000, texHandle, src->bmiHeader.biBitCount >= 32 ? 2 : ((type & 0x800) ? 1 : 0), (m_useHiQualityTex || (type & 0x80000000)) ? 24 : 16);

		m_texList[texHandle] = TS_UPDATED;
	}

	return texHandle;
}

// wd3d.cpp:1558
void WDirect3D::UpdateTexture(int texHandle, tagBITMAPINFO* src, void* data, unsigned long type) {
	if (m_texList[texHandle] == TS_CREATED) {
		CreateTextureSurface(src->bmiHeader.biWidth, src->bmiHeader.biHeight, 1, 0, texHandle, 0, 16);
		m_texList[texHandle] = TS_UPDATED;
	}

	UpdateTextureSurface(texHandle, src, data, type);
}

// wd3d.cpp:1569
void WDirect3D::DestroyTexture(int texHandle) {
	m_texList[texHandle] = TS_VOID;
}

// wd3d.cpp:1580
int WDirect3D::xCreateVertexBuffer(int nVtxs, unsigned long dwFVF, unsigned long dwUsage) {
	int hVb = xGetVbHandle();

	xCreateVertexBuffer(hVb, nVtxs, dwFVF, dwUsage);

	return hVb;
}

// wd3d.cpp:1589
int WDirect3D::xCreateIndexBuffer(int nIdxs, unsigned long dwUsage) {
	int hIb = xGetIbHandle();

	xCreateIndexBuffer(hIb, nIdxs, dwUsage);

	return hIb;
}

// wd3d.cpp:1598
int WDirect3D::xGetVbHandle() {
	// Handle 0 is never handed out: the search wraps to 1.
	for (unsigned int i = 1; m_xahVbs[m_xiVbCount] != 0 && i < 1023; i++) {
		m_xiVbCount = (unsigned int)m_xiVbCount >= 1024 ? 1 : m_xiVbCount + 1;
	}

	m_xahVbs[m_xiVbCount] = 1;
	return m_xiVbCount;
}

// wd3d.cpp:1611
int WDirect3D::xGetIbHandle() {
	for (int i = 1; m_xahIbs[m_xiIbCount] != 0 && i < 255; i++) {
		m_xiIbCount = m_xiIbCount >= 256 ? 1 : m_xiIbCount + 1;
	}

	m_xahIbs[m_xiIbCount] = 1;
	return m_xiIbCount;
}

// wd3d.cpp:1624
void WDirect3D::xReleaseVertexBuffer(int hVb) {
	m_xahVbs[hVb] = 0;
}

// wd3d.cpp:1629
void WDirect3D::xReleaseIndexBuffer(int hIb) {
	m_xahIbs[hIb] = 0;
}

// wd3d.cpp:1634
void WDirect3D::xDrawIndexedTriangles(const WxViewState& VState, const WxBatchState& BState) {
	if (m_devState == W_VDEVSTATE_LOST) {
		return;
	}

	if (BState.xiFlag0 & 0x400000) {
		Buffering(VState, BState);
	} else {
		xDrawIndexedPrimitive(VState, BState);
	}
}
