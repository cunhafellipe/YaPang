// Original: d:\build\custom\temp\code\client\projectg\sky.cpp
#include "sky.h"
#include "commonutil.h"
#include "FxBox.h"
#include "fx.h"
#include "fxspray.h"
#include "fxsequence.h"
#include "GolfDoc.h"
#include "LensFlare.h"
#include "polysoup.h"
#include "SharedDoc.h"
#include "wresrcmng.h"
#include "wview.h"
#include "wind.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifdef PG_CLOUDS_FACE_CAMERA
// The port's runtime switch (the dev console's "clouds"; port/compat).
#include "pg_switches.h"
#endif
#ifdef PG_CONTACT_SHADOWS
// The port's volumetric clouds (renderer/metal/pg_mtl_contact.h): while on
// (the dev console's "vclouds"), each billboard cloud is drawn as a volume.
extern "C" bool PgVCloudsOn(void);
extern "C" void PgVCloudsAdd(int index, float x, float y, float z, float size, float alpha, unsigned color);
#endif

float Random(float min, float max, int seed);  // commonutil.cpp

// The original's max: `a < b ? b : a`.
template <class T> T MaxOf(T a, T b) { return a < b ? b : a; }

// The batch flags of a sky texture: the handle in the low 11 bits, then the
// render flags of the sky layers (their Wangreal names are not in the PDB).
#define SKY_TEX_FLAG(tex) (((tex) & 0x7FF) | 0x22300000)

extern WMatrix g_camera;

// sky.cpp:3
sCloud::sCloud(void) {
}

// sky.cpp:94
CSky::CSky(void) : m_lensFlare(0), m_fxBox(0), m_pvsFxBox(0) {
	m_cloudTex = 0;

	m_cloudCount = 0;
	m_cloudGenCount = 0;
	m_cloudInterval = 0;

	m_cloudTimer = 0;

	m_rotateSpeed = 0;

	m_loadParam = 0; m_cloudRadius = 1120.0f; m_cloudFadeRange = 1600.0f; m_cloudHeight = 416.0f; m_skyColor = 0xffffffff;
	m_texColumns = 2;
	m_texRows = 2;

	memset(m_farTex, 0, sizeof(m_farTex));
	memset(m_nearTex, 0, sizeof(m_nearTex));
	memset(m_upTex, 0, sizeof(m_upTex));
	memset(m_dnTex, 0, sizeof(m_dnTex));

	m_loaded = 0;

	m_upAndBottomAngle = 0;
	m_nearCylinderAngle = 0;
}

// sky.cpp:126
CSky::~CSky(void) {
	Clear();
}

// sky.cpp:131
void CSky::Clear(void) {
	if (m_loaded == 0) {
		return;
	}

	for (int i = 0; i < 4; i++) {
		// A batch with a texture owns a vertex and an index buffer.
		if (m_batch[i].xiFlag0 & 0x7FF) {
			g_resrcmng->VideoReference()->xReleaseVertexBuffer((unsigned int)m_batch[i].xiFlag1 >> 22);
			g_resrcmng->VideoReference()->xReleaseIndexBuffer((m_batch[i].xiFlag1 >> 14) & 0xFF);

			m_batch[i].xiFlag1 = 0; m_batch[i].xiFlag0 = 0;
		}
	}

	for (unsigned int i = 0; i < 5; i++) {
		if (g_resrcmng && m_farTex[i]) { g_resrcmng->Release(m_farTex[i]); m_farTex[i] = 0; }
	}
	for (int i = 0; i < 5; i++) {
		if (g_resrcmng && m_nearTex[i]) { g_resrcmng->Release(m_nearTex[i]); m_nearTex[i] = 0; }
	}
	for (int i = 0; i < 2; i++) {
		if (g_resrcmng && m_upTex[i]) { g_resrcmng->Release(m_upTex[i]); m_upTex[i] = 0; }
	}
	for (int i = 0; i < 2; i++) {
		if (g_resrcmng && m_dnTex[i]) { g_resrcmng->Release(m_dnTex[i]); m_dnTex[i] = 0; }
	}

	if (g_resrcmng && m_cloudTex) { g_resrcmng->Release(m_cloudTex); m_cloudTex = 0; }
	m_cloudCount = 0; m_cloudGenCount = 0;

	if (m_lensFlare) { delete m_lensFlare; m_lensFlare = 0; }

	if (m_fxBox) { delete m_fxBox; m_fxBox = 0; }

	if (m_pvsFxBox) { delete m_pvsFxBox; m_pvsFxBox = 0; }


	m_loaded = 0;
}

// sky.cpp:173
void CSky::LoadTexByWeather(const char* name, const char* layer) {
	char path[128] = "";
	int tex;
	int i;

	sprintf(path, "%s_%s.jpg", name, layer);
	tex = g_resrcmng->LoadTexture(path, 0x80000000, 0, 0);
	if (!tex) {
		char altPath[256] = "";
		sprintf(altPath, "[%s", path);
		tex = g_resrcmng->LoadTexture(altPath, 0x80000000, 0, 0);
	}
	if (tex) {
		if (strcmp("far", layer) == 0) {
			m_farTex[0] = tex;
		} else if (strcmp("near", layer) == 0) {
			m_nearTex[0] = tex;
		} else if (strcmp("up", layer) == 0) {
			m_upTex[0] = tex;
		} else if (strcmp("dn", layer) == 0) {
			m_dnTex[0] = tex;
		}
	}

	for (i = tex != 0; i < 5; i++) {
		sprintf(path, "%s_%s_%d.jpg", name, layer, i);
		tex = g_resrcmng->LoadTexture(path, 0x80000000, 0, 0);
		if (strcmp("far", layer) == 0) {
			m_farTex[i] = tex;
		} else if (strcmp("near", layer) == 0) {
			m_nearTex[i] = tex;
		} else if (strcmp("up", layer) == 0) {
			m_upTex[i] = tex;
			return;
		} else if (strcmp("dn", layer) == 0) {
			m_dnTex[i] = tex;
			return;
		}
	}
}

// sky.cpp:257
int CSky::IsThereTexHandle(const char* name, int index) {
	int* handle;
	if (strcmp(name, "far") == 0) {
		handle = &m_farTex[index];
	} else if (strcmp(name, "near") == 0) {
		handle = &m_nearTex[index];
	} else if (strcmp(name, "up") == 0) {
		if (index == 0) {
			handle = &m_upTex[0];
		} else {
			handle = &m_upTex[1];
		}
	} else if (strcmp(name, "dn") == 0) {
		if (index == 0) {
			handle = &m_dnTex[0];
		} else {
			handle = &m_dnTex[1];
		}
	} else {
		return -1;
	}
	return *handle;
}

// sky.cpp:298
void CSky::LoadSky(const char* name, float param) {
	if (m_loaded) {
		Clear();
	}

	// texture divisions of each layer: far, near, up, down
	m_loaded = 1;
	int divs[4];
	divs[0] = 2; divs[1] = 2; divs[2] = 1; divs[3] = 1;
	LoadTexByWeather(name, "far");

	int tex = IsThereTexHandle("far", 0);
	if (tex) {
		divs[0] = 1;
		m_batch[0].xiFlag0 = SKY_TEX_FLAG(tex);
	} else {
		m_batch[0].xiFlag0 = 0;
	}

	LoadTexByWeather(name, "near");
	tex = IsThereTexHandle("near", 0);
	if (tex) {
		divs[1] = 1;
		m_batch[1].xiFlag0 = SKY_TEX_FLAG(tex);
	} else {
		m_batch[1].xiFlag0 = 0;
	}

	LoadTexByWeather(name, "up");
	tex = IsThereTexHandle("up", 0);
	if (tex) {
		m_batch[2].xiFlag0 = SKY_TEX_FLAG(tex);
	} else {
		m_batch[2].xiFlag0 = 0;
	}

	LoadTexByWeather(name, "dn");
	tex = IsThereTexHandle("dn", 0);
	if (tex) {
		m_batch[3].xiFlag0 = SKY_TEX_FLAG(tex);
	} else {
		m_batch[3].xiFlag0 = 0;
	}

	m_loadParam = param; m_texColumns = divs[0] < 1 ? 1 : divs[0];
	m_texRows = divs[1] < 1 ? 1 : divs[1];

	float turn[2];
	turn[0] = param / divs[0];
	WVideoDev* video = g_resrcmng->VideoReference();
	turn[1] = param / divs[1];

	for (int layer = 0; layer < 4; layer++) {
		if (!(m_batch[layer].xiFlag0 & 0x7FF)) {
			continue;
		}

		m_batch[layer].xiFlag1 = 5;
		unsigned long fvf = video->xDetermineFVF(m_batch[layer].xiFlag0, 5, 0);
		unsigned long usage = video->xDetermineBufferUsage(fvf) | 8;
		int stride = video->VertexSize(fvf);

		int numVerts;
		int numIdxs;
		int bandBytes;
		int rows;
		int cols;
		float radius;
		float ys[3];
		float vs[3];
		if (layer <= 1) {
			// cylinder: three bands of (divs + 32) vertices
			numVerts = (divs[layer] + 32) * 3;
			bandBytes = (divs[layer] + 32) * stride;
			numIdxs = divs[layer] * 4 + 130;
			radius = 500.0f;
			if (layer) {
				radius = 400.0f;
			}

			rows = divs[layer];
			cols = 32 / divs[layer] + 1;
			ys[0] = radius * 0.785398185f;
			ys[1] = radius * 0.628318548f;
			ys[2] = radius * -0.628318548f;
			vs[0] = 0.0f;
			vs[1] = 0.1f;
			vs[2] = 1.0f;
		} else {
			// up or down plane: one quad
			m_batch[layer].xiFlag1 |= 0x400;
			numVerts = numIdxs = 4;
			bandBytes = stride * 2;
			ys[0] = (layer == 2 ? 1.0f : -1.0f) * 196.349548f;

			rows = divs[layer];
			cols = 2 / divs[layer];
			vs[0] = 0.0f;
			vs[1] = 1.0f;
		}

		int vb = video->xCreateVertexBuffer(numVerts, fvf, usage);
		int ib = video->xCreateIndexBuffer(numIdxs, usage);

		unsigned long color = ModifyColor(m_skyColor, GOLFDOC()->m_fogBrightness);
		if (layer) {
			color |= 0xFF000000;
		}
		unsigned long colors[3];
		colors[0] = color;
		colors[1] = colors[2] = colors[0] | 0xFF000000;

		unsigned char* vtx = video->xLockVertexBuffer(vb, 0, 0);
		float x, z, u;
		for (int row = 0; row < rows; row++) {
			for (int col = 0; col < cols; col++) {
				if (layer <= 1) {
					int step = (cols - 1) * row + col;

					float angle = (step * 0.03125f - turn[layer]) * 6.28318548f;
					x = (float)(cos(angle) * radius);
					z = (float)(sin(angle) * radius);
					u = 1.0f - (rows * col) * 0.03125f;
					int off = (row * cols + col) * stride;
					memcpy(vtx + off, &x, 4); off += 4;
					memcpy(vtx + off, &ys[0], 4); off += 4;
					memcpy(vtx + off, &z, 4); off += 4;
					memcpy(vtx + off, &colors[0], 4); off += 4;
					memcpy(vtx + off, &u, 4); off += 4;
					memcpy(vtx + off, &vs[0], 4);

					off = (row * cols + col) * stride + bandBytes;
					memcpy(vtx + off, &x, 4); off += 4;
					memcpy(vtx + off, &ys[1], 4); off += 4;
					memcpy(vtx + off, &z, 4); off += 4;
					memcpy(vtx + off, &colors[1], 4); off += 4;
					memcpy(vtx + off, &u, 4); off += 4;
					memcpy(vtx + off, &vs[1], 4);

					off = (row * cols + col) * stride + bandBytes * 2;
					memcpy(vtx + off, &x, 4); off += 4;
					memcpy(vtx + off, &ys[2], 4); off += 4;
					memcpy(vtx + off, &z, 4); off += 4;
					memcpy(vtx + off, &colors[2], 4); off += 4;
					memcpy(vtx + off, &u, 4); off += 4;
					memcpy(vtx + off, &vs[2], 4);
				} else {
					if (col % 2 != 0) {
						u = 0.0f;
					} else {
						u = 1.0f;
					}

					for (int j = 0; j < 2; j++) {
						int off = (row * cols + col) * stride + bandBytes * j;
						x = (col ? 1.0f : -1.0f) * 500.0f;
						z = j ? -500.0f : 500.0f;
						memcpy(vtx + off, &x, 4); off += 4;
						memcpy(vtx + off, &ys[0], 4); off += 4;
						memcpy(vtx + off, &z, 4); off += 4;
						memcpy(vtx + off, &colors[0], 4); off += 4;
						memcpy(vtx + off, &u, 4); off += 4;
						memcpy(vtx + off, &vs[j], 4);
					}
				}
			}
		}
		video->xUnlockVertexBuffer(vb);

		unsigned short* idx = (unsigned short*)video->xLockIndexBuffer(ib, 0, 0);
		int count;
		if (layer <= 1) {
			// a strip per band pair, joined by a degenerate triangle
			count = 0;
			for (int band = 0; band < 2; band++) {
				for (int row = 0; row < rows; row++) {
					for (int col = 0; col < cols; col++) {
						unsigned short top = (unsigned short)(band * (divs[layer] + 32) + row * cols + col);
						idx[count] = top;
						idx[count + 1] = (unsigned short)(top + divs[layer] + 32);
						count += 2;
					}
				}
				if (band < 1) {
					idx[count] = idx[count - 1];
					idx[count + 1] = (unsigned short)(divs[layer] + 32);
					count += 2;
				}
			}
		} else {
			idx[0] = 0;
			idx[1] = 2; idx[2] = 1; idx[3] = 3;
			count = 4;
		}
		video->xUnlockIndexBuffer(ib);

		m_batch[layer].xnmTransfs = 0;
		m_batch[layer].xiBaseVtxIdx = 0; m_batch[layer].xnVtxs = numVerts;
		m_batch[layer].xiBaseIdxIdx = 0; m_batch[layer].xnIdxs = count;
		m_batch[layer].xiFlag1 |= ((vb << 8) | (ib & 0xFF)) << 14;
	}
}

// sky.cpp:588
void CSky::SetSkyTex(int texhandle, unsigned char layer, unsigned char batch) {
	if (texhandle) {
		m_batch[batch].xiFlag0 = SKY_TEX_FLAG(texhandle);
	} else {
		m_batch[batch].xiFlag0 = 0;
	}
}

// sky.cpp:619
void CSky::ChangeWeatherTex(unsigned char index) {
	int farTex = IsThereTexHandle("far", index);
	int nearTex = IsThereTexHandle("near", index);
	if (farTex == 0 && nearTex == 0) {
		return;
	}
	if (farTex) { m_batch[0].xiFlag0 = SKY_TEX_FLAG(farTex); } else { m_batch[0].xiFlag0 = 0; }
	if (nearTex) { m_batch[1].xiFlag0 = SKY_TEX_FLAG(nearTex); } else { m_batch[1].xiFlag0 = 0; }

	int upTex = IsThereTexHandle("up", index);
	if (upTex) { m_batch[2].xiFlag0 = SKY_TEX_FLAG(upTex); } else { m_batch[2].xiFlag0 = 0; }

	int dnTex = IsThereTexHandle("dn", index);
	if (dnTex) { m_batch[3].xiFlag0 = SKY_TEX_FLAG(dnTex); } else { m_batch[3].xiFlag0 = 0; }
}

// sky.cpp:641
void CSky::ClearClouds(void) {
	if (g_resrcmng && m_cloudTex) { g_resrcmng->Release(m_cloudTex); m_cloudTex = 0; }

	m_cloudCount = 0;
	m_cloudGenCount = 0;
}

// sky.cpp:648
void CSky::RegisterCloud(char* name) {
	ClearClouds();
	m_cloudTex = g_resrcmng->LoadTexture(name, 0, 0, 0);
}

// sky.cpp:654
void CSky::ShowClouds(bool show) {
	if (show == true) {
		for (int i = 0; i < m_cloudCount; i++) {
			m_clouds[i].m_active = rand() % 2 == 0;
		}
	} else {
		for (int i = 0; i < m_cloudCount; i++) {
			m_clouds[i].m_active = true;
		}
	}
}

// sky.cpp:668
void CSky::GenerateClouds(int count, float density) {
	if (count > 50) {
		count = 50;
	}

	m_cloudCount = count;
	if (density < 1.0f) {
		m_cloudInterval = 10.0f;
	} else {
		m_cloudInterval = 9.0f / density + 1.0f;
	}

	for (int i = 0; i < count; i++) {
		m_clouds[i].m_size = Random(1.0f, 2.0f, -1) * 256.0f;
		m_clouds[i].m_texIndex = rand() % 4;
		m_clouds[i].m_rotSpeed = Random(-0.1f, 0.1f, -1);
		m_clouds[i].m_flip = rand() % 2 != 0;

		if (rand() % 2 == 0) {
			m_clouds[i].m_pos = m_cloudRadius * WVector(Random(-1.0f, 1.0f, -1), 0.0f, Random(-1.0f, 1.0f, -1));
			m_clouds[i].m_pos.y = m_cloudHeight;
			m_clouds[i].m_active = true;
			m_cloudGenCount++;
		} else {
			m_clouds[i].m_pos.y = m_cloudHeight;
			m_clouds[i].m_active = false;
		}
	}
}

// sky.cpp:703
void CSky::GenerateStars(int count) {
	if (m_fxBox) {
		delete m_fxBox;
		m_fxBox = 0;
	}

	int map = count;
	if (map == -1) {
		map = GetCurMap();
	}
	IFF_STRUCT::sCourse* course = ItemManager()->FindCourse(0x28000000 | ((map > 0x7F && map != 0xFD) ? map - 0x80 : map));
	if (!course || !course->m_skyEffectFile[0]) {
		return;
	}

	const char* name = course->m_skyEffectFile;
	if ((int)strlen(name) < 5) {
		return;
	}

	// sky.cpp:720
	const char* ext = strrchr(name, '.');
	if (!ext || strlen(ext) < 4) {
		return;
	}

	if (!m_fxBox) {
		m_fxBox = new CPvsFxBox;
		m_fxBox->Init(g_resrcmng);
		m_fxBox->SetCamera(&g_camera);
		m_fxBox->m_frameTime = 33.333333f;
	}

	// sky.cpp:735
	if (stricmp(ext + 1, "spr") == 0) {
		CFxSpray* pSpray = m_fxBox->LoadSpray(name, NULL);
		if (!pSpray) {
			return;
		}

		Attacher attacher;
		WView* view = g_view;
		pSpray->m_pAttacher = &attacher;
		pSpray->m_fAttachLimitA = attacher.m_fLimitA;
		pSpray->m_fAttachLimitB = attacher.m_fLimitB; attacher.m_pMatrix[0] = NULL;
		memcpy(pSpray->m_pMatrix, attacher.m_pMatrix, sizeof(pSpray->m_pMatrix));
		attacher.m_pOffset[0] = &view->camera.pivot; memcpy(pSpray->m_pOffset, attacher.m_pOffset, sizeof(pSpray->m_pOffset));
		m_fxBox->AddSpray(pSpray);
	} else if (stricmp(ext + 1, "seq") == 0) {
		// sky.cpp:759
		CFxSequence* pSequence = m_fxBox->LoadSequence(name, true, NULL);
		if (!pSequence) {
			return;
		}

		Attacher attacher;
		WView* view = g_view;
		attacher.m_pOffset[0] = &view->camera.pivot;

		pSequence->m_fDistLimitA = attacher.m_fLimitA;
		pSequence->m_pAttacher = &attacher;
		pSequence->m_fDistLimitB = attacher.m_fLimitB;
		attacher.m_pMatrix[0] = NULL;
		m_fxBox->AddSequence(pSequence);

		m_fxBox->Process(m_fxBox->m_frameTime);

		std::map<CFxSpray*, CFxSpray*>::const_iterator it = pSequence->GetSprayList().begin();
		for (; it != pSequence->GetSprayList().end(); ++it) {
			CFxSpray* pSpray = it->second;
			pSpray->m_pAttacher = &attacher;
			pSpray->m_fAttachLimitA = attacher.m_fLimitA;
			pSpray->m_fAttachLimitB = attacher.m_fLimitB;
			memcpy(pSpray->m_pMatrix, attacher.m_pMatrix, sizeof(pSpray->m_pMatrix));
			memcpy(pSpray->m_pOffset, attacher.m_pOffset, sizeof(pSpray->m_pOffset));
			m_fxBox->AddSpray(pSpray);
		}

		m_fxBox->DeleteSequence(pSequence, false);
	}
}

// sky.cpp:799
void CSky::GenerateMoons(void) {
	if (m_pvsFxBox) {
		delete m_pvsFxBox;
		m_pvsFxBox = 0;
	}

	int count = GOLFDOC()->m_pPolySoup->m_numPoints;
	WPolySoup::sPoint* points = GOLFDOC()->m_pPolySoup->m_pPoints;

	for (int i = 0; i < count; i++) {
		WPolySoup::sPoint* point = &points[i];
		// a moon
		if (point->m_type != 3) {
			continue;
		}

		const char* ext = strrchr(point->m_desc.c_str(), '.');
		if (!ext || strlen(ext) < 4) {
			continue;
		}

		if (!m_pvsFxBox) {
			m_pvsFxBox = new CPvsFxBox;
			m_pvsFxBox->Init(g_resrcmng);
			m_pvsFxBox->SetCamera(&g_camera);
			m_pvsFxBox->m_frameTime = 33.333333f;
		}

		// sky.cpp:829
		if (stricmp(ext + 1, "spr") == 0) {
			CFxSpray* pSpray = m_pvsFxBox->LoadSpray(point->m_desc.c_str(), NULL);
			if (!pSpray) {
				continue;
			}

			Attacher attacher;
			WView* view = g_view;
			attacher.m_pOffset[0] = &view->camera.pivot;

			pSpray->m_fAttachLimitA = attacher.m_fLimitA;
			pSpray->m_pAttacher = &attacher;
			pSpray->m_fAttachLimitB = attacher.m_fLimitB;
			pSpray->m_spawnDir = point->m_pos.Normalize() * 50.0f;
			attacher.m_pMatrix[0] = NULL;
			memcpy(pSpray->m_pMatrix, attacher.m_pMatrix, sizeof(attacher.m_pMatrix));
			memcpy(pSpray->m_pOffset, attacher.m_pOffset, sizeof(attacher.m_pOffset));
			m_pvsFxBox->AddSpray(pSpray);
		} else if (stricmp(ext + 1, "seq") == 0) {
			// sky.cpp:854
			CFxSequence* pSequence = m_pvsFxBox->LoadSequence(point->m_desc.c_str(), true, NULL);
			if (!pSequence) {
				continue;
			}

			Attacher attacher;
			WView* view = g_view;
			attacher.m_pOffset[0] = &view->camera.pivot;

			pSequence->m_fDistLimitA = attacher.m_fLimitA;
			pSequence->m_pAttacher = &attacher;
			pSequence->m_fDistLimitB = attacher.m_fLimitB;
			attacher.m_pMatrix[0] = NULL;
			m_pvsFxBox->AddSequence(pSequence);

			m_pvsFxBox->Process(m_pvsFxBox->m_frameTime);

			// sky.cpp:875
			std::map<CFxSpray*, CFxSpray*>::const_iterator it = pSequence->GetSprayList().begin();
			for (; it != pSequence->GetSprayList().end(); ++it) {
				CFxSpray* pSpray = it->second;
				pSpray->m_pAttacher = &attacher;
				pSpray->m_fAttachLimitA = attacher.m_fLimitA;
				pSpray->m_fAttachLimitB = attacher.m_fLimitB;
				pSpray->m_spawnDir = point->m_pos.Normalize() * 50.0f + pSpray->m_pos;
				memcpy(pSpray->m_pMatrix, attacher.m_pMatrix, sizeof(pSpray->m_pMatrix));
				memcpy(pSpray->m_pOffset, attacher.m_pOffset, sizeof(pSpray->m_pOffset));
				m_pvsFxBox->AddSpray(pSpray);
			}

			m_pvsFxBox->DeleteSequence(pSequence, false);
		}
	}
}

// sky.cpp:897
void CSky::GenerateLensFlare(void) {
	if (m_lensFlare) { delete m_lensFlare; m_lensFlare = 0; }

	int count = GOLFDOC()->m_pPolySoup->m_numPoints;
	WPolySoup::sPoint* point = GOLFDOC()->m_pPolySoup->m_pPoints;

	for (int i = 0; i < count; i++, point++) {
		// the sun
		if (point && point->m_type == 2) {
			if (!m_lensFlare) {
				m_lensFlare = new CLensFlare;
			}
			m_lensFlare->Init(point->m_pos);
			return;
		}
	}
}

// sky.cpp:926
void CSky::ClearSky(void) {
	switch (GetCurMap()) {
	case 5:
	case 6:
		m_fxBox->Clear();
		m_pvsFxBox->Clear();
		if (m_fxBox) {
			delete m_fxBox;
			m_fxBox = 0;
		}
		if (m_pvsFxBox) {
			delete m_pvsFxBox;
			m_pvsFxBox = 0;
		}
		break;
	case 7:
		m_pvsFxBox->Clear();
		if (m_pvsFxBox) {
			delete m_pvsFxBox;
			m_pvsFxBox = 0;
		}
		break;
	}
}

// sky.cpp:949
void CSky::Process(float elapsed) {
	m_cloudTimer += elapsed;
	if (m_cloudTimer > m_cloudInterval) {
		m_cloudTimer -= m_cloudInterval;

		if (m_cloudGenCount < m_cloudCount) {
			for (int i = 0; i < m_cloudCount; i++) {
				if (!m_clouds[i].m_active) {
					float r = Random(-1.0f, 1.0f, -1);
					WVector spawn = WVector(r, 0.0f, -1.0f) * m_cloudRadius;
					m_clouds[i].m_pos = spawn * RotMat(Wind().GetGlobalDirection(), 1);
					m_clouds[i].m_pos.y = m_cloudHeight;
					m_clouds[i].m_active = true;
					m_clouds[i].m_flip = rand() % 2 != 0;
					m_cloudGenCount++;
					break;
				}
			}
		}
	}

	// sky.cpp:977
	for (int i = 0; i < m_cloudCount; i++) {
		sCloud& cloud = m_clouds[i];
		if (!cloud.m_active) {
			continue;
		}

		// The drift: faster for the clouds that turn faster.
		cloud.m_pos += (cloud.m_rotSpeed + 1.0f) * 3.0f * Wind().GetGlobalWind() * elapsed;

		// The alpha fades out toward the edge of the camera's view range.
		float dx = cloud.m_pos.x - g_camera.pivot.x;
		float dz = cloud.m_pos.z - g_camera.pivot.z;
		float t = MaxOf(0.0f, 1.0f - sqrtf(dx * dx + dz * dz) / m_cloudFadeRange);
		if (t > 0.5f) {
			cloud.m_alpha = 255.0f;
		} else {
			cloud.m_alpha = 255.0f * (t * 2.0f);
		}

		// And toward the course's origin, on the ground plane.
		WVector diff;
		diff = cloud.m_pos - GOLFDOC()->m_pPolySoup->GetBaseModel()->m_basePos;
		float dist = sqrtf(diff.x * diff.x + diff.z * diff.z);
		t = MaxOf(0.0f, 1.0f - dist / m_cloudRadius);
		if (t < 0.1f) {
			cloud.m_alpha *= t * 10.0f;
		}

		if (dist > (float)sqrt(2.0) * m_cloudRadius) {
			cloud.m_active = false;
			m_cloudGenCount--;
		}
	}

	// sky.cpp:1010
	if (m_fxBox) {
		m_fxBox->Process(elapsed * 1000.0f);
	}
	if (m_pvsFxBox) {
		m_pvsFxBox->Process(elapsed * 1000.0f);
	}
	if (m_lensFlare) {
		m_lensFlare->Process(elapsed);
	}

	// sky.cpp:1023
	// The near cylinder turns with the wind seen along the camera's view.
	WVector wind = Wind().GetGlobalWind();
	WMatrix rot;
	g_camera.GetRotMatrix(&rot);
	WVector view;
	view = WVector(-1.0f, 0.0f, 0.0f) * rot;
	view.Normalize();
	wind.Normalize();
	float along = view.x * wind.x + view.y * wind.y + view.z * wind.z;

	m_nearCylinderAngle += WVectorLen(Wind().GetGlobalWind()) * along * elapsed * 0.005f;
	if (fabsf(m_nearCylinderAngle) >= 360.0f) {
		m_nearCylinderAngle = 0.0f;
	}

	// sky.cpp:1036
	m_upAndBottomAngle += elapsed * 0.01f;
	if (m_upAndBottomAngle >= 360.0f) {
		m_upAndBottomAngle = 0.0f;
	}
}

// sky.cpp:1045
void CSky::Render(WView* view) {
	float oldNear = view->GetClipNearValue();
	float oldFar = view->GetClipFarValue();

	view->SetClip(1.0f, CalcClipFarByView(view), false);
	view->UpdateCamera();

	m_viewPos = view->camera.pivot;
	m_viewPos.y += m_rotateSpeed;

	RenderUpAndBottom(view);

	unsigned long gameMode = GOLFDOC()->GameMode();
	if (gameMode != 0x200 || view->GetReflective()) {
		RenderFarCylinder(view);

		if (m_fxBox) {
			std::map<CFxSpray*, CFxSpray*>::iterator it = m_fxBox->GetSprayList().begin();
			for (; it != m_fxBox->GetSprayList().end(); ++it) {
				it->second->m_pos = g_view->camera.pivot;
			}
			m_fxBox->Display(g_view);
			g_view->Flush(0);
		}

		if (m_pvsFxBox) {
			std::map<CFxSpray*, CFxSpray*>::iterator it = m_pvsFxBox->GetSprayList().begin();
			for (; it != m_pvsFxBox->GetSprayList().end(); ++it) {
				CFxSpray* spray = it->second;
				spray->m_pos = g_view->camera.pivot + spray->m_spawnDir;
			}
			m_pvsFxBox->Display(g_view);
			g_view->Flush(0);
		}

		if (m_lensFlare && !GetWeather()) {
			m_lensFlare->Render(view);
		}

		RenderNearCylinder(view);

		gameMode = GOLFDOC()->GameMode();
		if (gameMode != 0x200000) {
			if (gameMode & 0x200) {
				RenderFlatCloud(view);
			} else {
				RenderBillboardCloud(view);
			}
		}
	}

	view->SetClip(oldNear, oldFar, false);
	view->UpdateCamera();
}

// sky.cpp:1115
void CSky::RenderUpAndBottom(WView* view) {
	if (m_batch[2].xiFlag0 & 0x7FF) {
		WMatrix mat;
		mat.Reset(); mat.Rotate(m_upAndBottomAngle, 1);

		mat *= CalcScaleByView(view);
		mat.pivot = m_viewPos;

		m_batch[2].xmW = mat;

		view->xDrawIndexedTriangles(m_batch[2]);
	}

	if (m_batch[3].xiFlag0 & 0x7FF) {
		WMatrix mat;
		mat.Reset(); mat *= CalcScaleByView(view);
		mat.pivot = m_viewPos;

		m_batch[3].xmW = mat;

		view->xDrawIndexedTriangles(m_batch[3]);
	}
}

// sky.cpp:1173
void CSky::RenderBillboardCloud(WView* view) {
	WTVertex verts[4];
	WTVertex* vl[5];
	vl[0] = &verts[0];
	vl[1] = &verts[1];
	vl[2] = &verts[3];
	vl[3] = &verts[2];
	vl[4] = 0;   // DrawPolygonFan walks the list to its NULL

#ifdef PG_CONTACT_SHADOWS
	// The port's volumetric clouds: each cloud is sent, in its place, with its
	// size, fade and colour, instead of drawn as a billboard.
	if (PgVCloudsOn()) {
		unsigned long color = ModifyColor(m_skyColor, GOLFDOC()->m_fogBrightness);
		for (int i = 0; i < m_cloudCount; i++) {
			if (m_clouds[i].m_active) {
				const WVector& p = m_clouds[i].m_pos;
				PgVCloudsAdd(i, p.x, p.y, p.z, m_clouds[i].m_size, m_clouds[i].m_alpha / 255.0f, (unsigned)color);
			}
		}
		return;
	}
#endif

	for (int i = 0; i < m_cloudCount; i++) {
		if (m_clouds[i].m_active) {
#ifdef PG_CLOUDS_FACE_CAMERA
			// The port (port/CMakeLists.txt): each cloud stands upright and
			// turns about the vertical toward the camera's position, so it
			// does not swing with the camera's orientation. The same side as
			// the original's camera axes (no mirrored texture).
			WVector toCamera(view->camera.pivot.x - m_clouds[i].m_pos.x, 0.0f, view->camera.pivot.z - m_clouds[i].m_pos.z);
			WVector right = WCrossProduct(WVector::UNIT_POS_Y, toCamera);
			if (right.x * right.x + right.z * right.z < 1e-6f) {
				right = WVector(view->camera.xa.x, 0.0f, view->camera.xa.z);
			}
			right.Normalize();
			if (right.x * view->camera.xa.x + right.z * view->camera.xa.z < 0.0f) {
				right = right * -1.0f;
			}
			WVector up(0.0f, view->camera.ya.y < 0.0f ? -1.0f : 1.0f, 0.0f);
#endif
			for (int k = 0; k < 4; k++) {
				float sx = (k & 1) ? 0.25f : -0.25f;
				float sy = (k & 2) ? 0.5f : -0.5f;

#ifdef PG_CLOUDS_FACE_CAMERA
				if (!PgCloudsUpright()) {
					right = view->camera.xa;
					up = view->camera.ya;
				}
				WVector d(sy * right.x + sx * up.x, sy * right.y + sx * up.y, sy * right.z + sx * up.z);
#else
				float dz = sx * view->camera.ya.z;
				dz += sy * view->camera.xa.z;
				WVector d(sy * view->camera.xa.x + sx * view->camera.ya.x + view->camera.za.x * 0.0f,
				          sy * view->camera.xa.y + sx * view->camera.ya.y + view->camera.za.y * 0.0f,
				          dz + view->camera.za.z * 0.0f);
#endif
				WVector pos;
				pos = d * m_clouds[i].m_size + m_clouds[i].m_pos;
				verts[k].SetPosition(pos);
				verts[k].tu = m_clouds[i].m_flip ? ((k & 2) ? 0.0f : 1.0f) : ((k & 2) ? 1.0f : 0.0f);
				verts[k].tv = (float)(((~k) & 1) + m_clouds[i].m_texIndex) * 0.25f;
				verts[k].diffuse = ModifyColor(m_skyColor, GOLFDOC()->m_fogBrightness)
					| ((unsigned long)(unsigned char)(int)m_clouds[i].m_alpha << 24);
			}
			view->DrawPolygonFan(vl, (m_cloudTex & 0x7FF) | 0x20300000, 0x400, false);
		}
	}
}

// sky.cpp:1156
void CSky::RenderFarCylinder(WView* view) {
	if (!(m_batch[0].xiFlag0 & 0x7FF)) {
		return;
	}

	WMatrix mat;
	mat.Reset(); mat *= CalcScaleByView(view);
	mat.pivot = m_viewPos;

	m_batch[0].xmW = mat;

	view->xDrawIndexedTriangles(m_batch[0]);
}

// sky.cpp:1217
void CSky::RenderFlatCloud(WView* view) {
	WTVertex verts[4];
	WTVertex* vl[5];
	vl[0] = &verts[0];
	vl[1] = &verts[1];
	vl[2] = &verts[3];
	vl[3] = &verts[2];
	vl[4] = 0;   // DrawPolygonFan walks the list to its NULL

	for (int i = 0; i < m_cloudCount; i++) {
		if (m_clouds[i].m_active) {
			for (int k = 0; k < 4; k++) {
				WVector pos;
				pos = WVector((k & 2) ? 0.5f : -0.5f, 0.0f, (k & 1) ? 0.25f : -0.25f) * m_clouds[i].m_size + m_clouds[i].m_pos;
				verts[k].SetPosition(pos);
				verts[k].tu = m_clouds[i].m_flip ? ((k & 2) ? 0.0f : 1.0f) : ((k & 2) ? 1.0f : 0.0f);
				verts[k].tv = (float)(((~k) & 1) + m_clouds[i].m_texIndex) * 0.25f;
				verts[k].diffuse = ModifyColor(m_skyColor, GOLFDOC()->m_fogBrightness)
					| 0xFF000000;
			}
			view->DrawPolygonFan(vl, (m_cloudTex & 0x7FF) | 0x20300000, 0x400, false);
		}
	}
}

// sky.cpp:1261
void CSky::RenderNearCylinder(WView* view) {
	if (!(m_batch[1].xiFlag0 & 0x7FF)) {
		return;
	}

	WMatrix mat;
	mat.Reset(); mat.Rotate(m_nearCylinderAngle, 1);

	mat *= CalcScaleByView(view);
	mat.pivot = m_viewPos;

	m_batch[1].xmW = mat;

	view->xDrawIndexedTriangles(m_batch[1]);
}

// sky.cpp:1281
void CSky::UpdateBright(void) {
}

// sky.cpp:1285
float CSky::CalcScaleByView(WView* view) const {
	if (view->GetProjectionMode() == WView::PARALLEL) {
		float scale = view->GetScale() * 2.0f; return scale * 0.002f;
	}
	return 1.0f;
}

// sky.cpp:1293
float CSky::CalcClipFarByView(WView* view) const {
	if (view->GetProjectionMode() == WView::PARALLEL) {
		return CalcScaleByView(view) * 1000.0f;
	}
	return view->GetClipFarValue();
}
