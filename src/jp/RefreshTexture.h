// The JP 983 game's RFTexture (RTTI RefreshTexture, vtable 0xd13d44,
// 3 slots, size 0x40 from RefreshRenderer::CreateTexture's operator new).
// Declarations and the JP x86 layout only; the port implements it on the
// macOS renderer.
#ifndef PG_JP_REFRESHTEXTURE_H
#define PG_JP_REFRESHTEXTURE_H

#include <stdint.h>

#include "defines.h"
#include "padding.h"
#include "RFTexture.h"

class WOverlay;  // the image RFTextureManager::GetTexture returns (woverlay.h)

class RefreshTexture : public RFTexture {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x603160: zeroes the source rect, sets the scale to 1 and the alpha
	// to 0xff; with an overlay, the rect is (0, 0, its texture width
	// (WOverlay +0x30), its texture height (+0x34)).
	RefreshTexture(WOverlay* overlay, void* owner);

	virtual ~RefreshTexture();                    // [0] 0x6031f0 scalar deleting destructor
	virtual unsigned int GetWidth() const;        // [1] 0x6030c0
	virtual unsigned int GetHeight() const;       // [2] 0x6030f0

	WOverlay* GetOverlay() const { return m_overlay; }  // ours: RefreshRenderer reads +0x8 directly

protected:
	void* m_owner;               // 0x4  RefreshRenderer's +0x4 member at creation
	WOverlay* m_overlay;         // 0x8  from RFTextureManager (0x6022f0); drawn by RefreshRenderer
	float m_srcLeft;             // 0xc  0
	float m_srcTop;              // 0x10 0
	float m_srcWidth;            // 0x14 the entry's width
	float m_srcHeight;           // 0x18 the entry's height
	float m_scale[3];            // 0x1c 1.0f each
	PADDING(0x28, 0x34);         // not written by the constructor
	int32_t m_flags;             // 0x34 0
	int32_t m_alpha;             // 0x38 0xff
	PADDING(0x3c, 0x40);
};
CHECK_SIZE(RefreshTexture, 0x40);
BEGIN_LAYOUT_CHECK(RefreshTexture)
	CHECK_FIELD(RefreshTexture, m_owner, 0x4);
	CHECK_FIELD(RefreshTexture, m_overlay, 0x8);
	CHECK_FIELD(RefreshTexture, m_srcLeft, 0xc);
	CHECK_FIELD(RefreshTexture, m_scale, 0x1c);
	CHECK_FIELD(RefreshTexture, m_flags, 0x34);
	CHECK_FIELD(RefreshTexture, m_alpha, 0x38);
END_LAYOUT_CHECK

#endif
