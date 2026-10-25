// The JP 983 game's texture cache for Refresh (RTTI RFTextureManager :
// WSingleton<RFTextureManager> : BaseObject, vtable 0xd13d00, size 0x38;
// constructor 0x601dc0, destructor 0x601ae0; instance pointer 0xe78c30).
// Created first by the JP InitSingletonClasses (0xa65710). Declarations and
// the JP x86 layout only.
#ifndef PG_JP_RFTEXTUREMANAGER_H
#define PG_JP_RFTEXTUREMANAGER_H

#include <stdint.h>
#include <map>
#include <string>

#include "defines.h"
#include "padding.h"
#include "singleton.h"

class WOverlay;

// The mapped value of RFTextureManager's map (the tree node is 0x54 bytes:
// links 0x0-0xc, the std::string key 0xc, this value 0x28, colour/isnil
// bytes 0x50/0x51). Offsets below are inside the value.
struct RFTextureEntry {
	int32_t m_imageType;    // 0x0  (node +0x28) the file name's extension: 2 ".png", 1 ".tga", else 0 (0x600a70)
	std::string m_name;     // 0x4  (node +0x2c) the file name
	BaseObject* m_texture;  // 0x20 (node +0x48) deleted through its slot 0
	WOverlay* m_resource;   // 0x24 (node +0x4c) released with 0xb55a50 while the resource manager lives; what GetTexture returns
};

class RFTextureManager : public WSingleton<RFTextureManager> {
	LAYOUT_CHECK_FRIEND;
public:
	RFTextureManager();            // 0x601dc0
	virtual ~RFTextureManager();   // [0] 0x601e80 scalar deleting destructor (body 0x601ae0)

	// 0x6022f0: the image of a narrow file name (the entry's m_resource,
	// node +0x4c), loading it on first use (0x6020d0); null when missing
	// (RefreshRenderer::CreateTexture). Read as a WOverlay by RefreshTexture
	// (+0x30/+0x34 texture size) and RefreshRenderer (slot 2 Render, 0xb65a00
	// SetClippingArea).
	WOverlay* GetTexture(const std::string& name);

protected:
	// 0x6020d0: loads the image through the resource manager (0xf023f0)
	// and records it under its name; null without a resource manager.
	WOverlay* LoadTexture(const std::string& name);

	std::map<std::string, RFTextureEntry> m_textures;  // 0x28 (head 0x2c, size 0x30)
	void* m_unknown34;                                 // 0x34 0 at construction
};
CHECK_SIZE(RFTextureManager, 0x38);
BEGIN_LAYOUT_CHECK(RFTextureManager)
	CHECK_FIELD(RFTextureManager, m_textures, 0x28);
	CHECK_FIELD(RFTextureManager, m_unknown34, 0x34);
END_LAYOUT_CHECK

#endif
