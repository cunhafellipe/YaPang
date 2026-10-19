// Original: d:\build\custom\temp\code\client\projectg\background.cpp
#include <windows.h>
#include "background.h"
#include "bitmap.h"
#include "wresrcmng.h"
#include "wvideo.h"
#include "wutil.h"

// background.cpp:20
CBackGround::CBackGround(const char* filename, bool bScale)
	: m_pSplash(NULL), m_du(0.0f), m_dv(0.0f), m_u(0.0f), m_v(0.0f) {
	Load(filename, bScale);
}

// background.cpp:25
CBackGround::CBackGround(bool bScale, const Bitmap* bitmap) {
	if (bitmap) {
		WSplash* splash = (WSplash*)g_resrcmng->VideoReference()->Command(W_VDEV_GET_SPLASH, 0, 0);
		if (splash) {
			if (!splash->Init(*bitmap->bi, bitmap->vram, bScale)) {
				delete splash;
				splash = NULL;
			}
			m_pSplash = splash;
		}
	}
}

// background.cpp:46
CBackGround::~CBackGround(void) {
	if (m_pSplash) { delete m_pSplash; m_pSplash = NULL; }
}

// background.cpp:51
void CBackGround::Load(const char* filename, bool bScale) {
	if (filename) {
		Bitmap* bitmap = g_resrcmng->LoadBitmap(filename, 0, false);

		WSplash* splash = (WSplash*)g_resrcmng->VideoReference()->Command(W_VDEV_GET_SPLASH, 0, 0);
		if (splash) {
			if (!splash->Init(*bitmap->bi, bitmap->vram, bScale)) {
				delete splash;
				splash = NULL;
			}
			m_pSplash = splash;
		}

		if (bitmap) delete bitmap;
	}
	else {
		WSplash* splash = (WSplash*)g_resrcmng->VideoReference()->Command(W_VDEV_GET_SPLASH, 0, 0);
		if (splash) {
			if (!splash->InitFromScreen(true)) {
				delete splash;
				splash = NULL;
			}
			m_pSplash = splash;
		}
	}
}

// background.cpp:81
void CBackGround::Process(const float deltaTime) {
	if (m_pSplash) {
		m_u += deltaTime * m_du;
		m_v += deltaTime * m_dv;

		if (Abs(m_u) > 1.0f) {
			if (m_u > 0.0f)
				m_u = m_u - 1.0f;
			else
				m_u = m_u + 1.0f;
		}

		if (Abs(m_v) > 1.0f) {
			if (m_v > 0.0f)
				m_v = m_v - 1.0f;
			else
				m_v = m_v + 1.0f;
		}

		m_pSplash->SetTexCoordOffset(m_u, m_v);
	}
}

// background.cpp:108
void CBackGround::Draw(unsigned long color) {
	if (m_pSplash)
		m_pSplash->Draw(color);
}

// background.cpp:114
void CBackGround::Draw(const WPoint* pos, const WRect* src, unsigned long color) {
	if (m_pSplash)
		m_pSplash->Draw(pos, src, color);
}

// background.cpp:120
void CBackGround::ResetScreenSize(void) {
	if (m_pSplash)
		m_pSplash->ResetScreenSize();
}

// background.cpp:126
int CBackGround::GetWidth(void) {
	if (m_pSplash)
		return m_pSplash->GetWidth();
	return 0;
}

// background.cpp:134
int CBackGround::GetHeight(void) {
	if (m_pSplash)
		return m_pSplash->GetHeight();
	return 0;
}
