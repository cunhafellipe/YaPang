// The JP 983 game's RFRenderer (RTTI RefreshRenderer, vtable 0xd1307c,
// 11 slots, size 0x2c from RFSceneManager::Setup's operator new;
// constructor 0x5f53d0). Declarations and the JP x86 layout only; the port
// implements it on the macOS renderer.
#ifndef PG_JP_REFRESHRENDERER_H
#define PG_JP_REFRESHRENDERER_H

#include <stdint.h>
#include <string>

#include "defines.h"
#include "RFRect.h"
#include "RFRenderer.h"
#include "wtypes.h"

class WOverlay;

class RefreshRenderer : public RFRenderer {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5f53d0: both clip rects from the four floats at 0xf145f8, depth 0.
	RefreshRenderer();

	// [0] 0x5f5c50: narrows the name, asks RFTextureManager (the WSingleton
	// at 0xe78c30, 0x6022f0) for the image and wraps it in a new
	// RefreshTexture (0x40 bytes) with m_owner; null when the name is empty,
	// there is no texture manager or the image is missing.
	virtual RFTexture* CreateTexture(std::wstring const& name);
	// [1] 0x5f5340: deletes the texture (its slot 0).
	virtual void DeleteTexture(RFTexture* texture);
	// [2] 0x5f5370: empty.
	virtual void Draw(RFTexture*, float, RFMatrix const&);
	// [3] 0x5f5360: empty.
	virtual void Draw(RFTexture*, float, float, float, float, RFVector2 const&, RFSize const&, float, float, TileModeTypes::Type const&);
	// [4] 0x5f5900: draws the texture's entry (RefreshTexture +0x8) at pos
	// with size and alpha through 0x5f56f0; the tile mode is not used.
	virtual void Draw(RFTexture* texture, RFVector2 const& pos, RFSize const& size, float alpha, float, TileModeTypes::Type const&);
	// [5] 0x5f5950 (RFRenderer's first pure virtual): sets both clip rects
	// to the rect (x = left, y = top, w = right - left, h = bottom - top),
	// gives it to RFTextManager's RFTextDraw (0xbec470/0xbec480, Fresh
	// FrGraphicInterface clipping) and increments the depth.
	virtual void PushClipRect(RFRect const& rect);
	// [6] 0x5f59d0 (pure in RFRenderer): when the depth is positive, resets
	// the clip to the root window's size (RFManager +0x4c, RFWnd::GetSize),
	// hands it to RFTextDraw and decrements the depth.
	virtual void PopClipRect();
	// [7] 0x5f5a80: fills a rectangle through the game's WView (0xf023e8):
	// sets the scissor to m_clip when it has an area (0xb52890), fills
	// (0x5f5560), then restores the full view (WView +0x19c/+0x1a0).
	virtual void FillRectangle(uint32_t color, float x, float y, float w, float h, float);
	// [8] OnPreResetDevice, [9] OnPostResetDevice: inherited.
	// [10] 0x5f58d0: scalar deleting destructor (body 0x5f5430).
	virtual ~RefreshRenderer();

	// 0x5f5490 (not virtual): both clip rects to (0, 0, root window size);
	// called by RFSceneManager::SetResolution (0x6035b0).
	void ResetClipRect();

protected:
	// 0x5f56f0 (not virtual, the overlay in ecx; its owner is not attested,
	// the static member is ours): draws the overlay at pos with size (its
	// texture size when the size is 0 x 0) and alpha, clipped to the
	// instance renderer's m_clip (m_baseClip when baseClip).
	static void DrawOverlay(WOverlay* overlay, RFVector2 const& pos, RFSize const& size, float alpha, bool baseClip);

	void* m_device;             // 0x4  0 at construction; RefreshTexture's owner (set elsewhere)
	WRect m_clip;               // 0x8  current clip x, y, w, h (handed to WView/FrGraphicInterface as a WRect*)
	WRect m_baseClip;           // 0x18 the same, kept by Push/Reset
	int32_t m_clipDepth;        // 0x28 PushClipRect count
};
CHECK_SIZE(RefreshRenderer, 0x2c);
BEGIN_LAYOUT_CHECK(RefreshRenderer)
	CHECK_FIELD(RefreshRenderer, m_device, 0x4);
	CHECK_FIELD(RefreshRenderer, m_clip, 0x8);
	CHECK_FIELD(RefreshRenderer, m_baseClip, 0x18);
	CHECK_FIELD(RefreshRenderer, m_clipDepth, 0x28);
END_LAYOUT_CHECK

// 0x5f5560 (a free game helper, name ours): a box at pos with size in color,
// through WOverlay::DrawBox on g_view (z 0.001f, 0xd08dcc). FillRectangle
// and RFModalRenderer::DrawModalBackground (0x5e58f0) call it.
void FillBox(const RFVector2& pos, const RFSize& size, uint32_t color);

#endif
