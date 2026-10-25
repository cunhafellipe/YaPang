// The JP 983 game's RFTexture (RefreshTexture.h), from the JP exe's bodies
// (node scripts/jp_hlil.ts on the vtable 0xd13d44 slots).
#include "RefreshTexture.h"

#include "woverlay.h"

// 0x603160
RefreshTexture::RefreshTexture(WOverlay* overlay, void* owner) {
	m_srcLeft = 0.0f;
	m_srcTop = 0.0f;
	m_srcWidth = 0.0f;
	m_owner = NULL;
	m_srcHeight = 0.0f;
	m_overlay = NULL;
	m_scale[0] = 1.0f;
	m_scale[1] = 1.0f;
	m_scale[2] = 1.0f;
	m_flags = 0;
	m_alpha = 0xff;
	if (overlay != NULL) {
		m_srcLeft = 0.0f;
		m_srcTop = 0.0f;
		m_owner = owner;
		m_overlay = overlay;
		// fild: the texture size read as signed ints.
		m_srcWidth = (float)(int)overlay->GetWidth();
		m_srcHeight = (float)(int)overlay->GetHeight();
	}
}

// 0x6031e0 (scalar deleting destructor 0x6031f0)
RefreshTexture::~RefreshTexture() {
}

// 0x6030c0
unsigned int RefreshTexture::GetWidth() const {
	return (unsigned int)m_srcWidth;
}

// 0x6030f0
unsigned int RefreshTexture::GetHeight() const {
	return (unsigned int)m_srcHeight;
}
