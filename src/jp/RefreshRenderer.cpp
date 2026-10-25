// The JP 983 game's RFRenderer (RefreshRenderer.h), from the JP exe's
// bodies (node scripts/jp_hlil.ts on the vtable 0xd1307c slots), on the
// game's WView (g_view, 0xf023e8), WOverlay and RFTextManager's RFTextDraw.
#include "RefreshRenderer.h"

#include <stdint.h>
#include <string>

#include "RFManager.h"
#include "RFSceneManager.h"
#include "RFTextDraw.h"
#include "RFTextManager.h"
#include "RFTextureManager.h"
#include "RFWnd.h"
#include "RefreshStringUtil.h"
#include "RefreshTexture.h"
#include "woverlay.h"
#include "wview.h"

// 0xf145f8: a Wangreal WRect global the constructors copy and DrawOverlay
// uses as a source rect; it is in .bss and read as (0, 0, 0, 0) by its other
// users (0xb5e0c0 returns it for a missing glyph). Its KR name is not known.
static const WRect s_emptyRect(0.0f, 0.0f, 0.0f, 0.0f);
// 0xe428fc: the source rect (0, 0, 1, 1), the whole texture.
static const WRect s_fullRect(0.0f, 0.0f, 1.0f, 1.0f);

// 0x510250 (a game helper, name ours): the colour with its alpha byte scaled
// by alpha.
static uint32_t ScaleColorAlpha(uint32_t color, float alpha) {
	return ((uint32_t)(int64_t)((float)(color >> 24) * alpha) << 24) | (color & 0xffffff);
}

// 0x5f5560
void FillBox(const RFVector2& pos, const RFSize& size, uint32_t color) {
	WOverlay::DrawBox(g_view, WRect(pos.x, pos.y, size.width, size.height), 0, color, 0.001f);
}

// 0x5f53d0
RefreshRenderer::RefreshRenderer() {
	m_device = NULL;
	m_clip = s_emptyRect;
	m_baseClip = s_emptyRect;
	m_clipDepth = 0;
}

// 0x5f5c50
RFTexture* RefreshRenderer::CreateTexture(std::wstring const& name) {
	if (name.size() == 0 || RFTextureManager::Instance() == NULL) {
		return NULL;
	}
	std::string file = WStringToShiftJis(std::wstring(name.c_str()));
	WOverlay* overlay = RFTextureManager::Instance()->GetTexture(std::string(file.c_str()));
	if (overlay == NULL) {
		return NULL;
	}
	return new RefreshTexture(overlay, m_device);
}

// 0x5f5340
void RefreshRenderer::DeleteTexture(RFTexture* texture) {
	if (texture != NULL) {
		delete texture;
	}
}

// 0x5f5370
void RefreshRenderer::Draw(RFTexture*, float, RFMatrix const&) {
}

// 0x5f5360
void RefreshRenderer::Draw(RFTexture*, float, float, float, float, RFVector2 const&, RFSize const&, float, float, TileModeTypes::Type const&) {
}

// 0x5f56f0
void RefreshRenderer::DrawOverlay(WOverlay* overlay, RFVector2 const& pos, RFSize const& size, float alpha, bool baseClip) {
	if (overlay == NULL) {
		return;
	}
	WView* view = g_view;
	if (view == NULL) {
		return;
	}
	float width = size.width;
	float height = size.height;
	if (width == 0.0f && height == 0.0f) {
		width = (float)overlay->GetWidth();
		height = (float)overlay->GetHeight();
	}
	// The original takes the rect at +0x8 (+0x18 with baseClip) of the
	// instance's renderer without testing it (a bogus pointer without one);
	// the port passes no rect then.
	RFSceneManager* sceneManager = RFSceneManager::Instance();
	RefreshRenderer* renderer = sceneManager != NULL ? sceneManager->GetRenderer() : NULL;
	const WRect* clip = NULL;
	if (renderer != NULL) {
		clip = baseClip ? &renderer->m_baseClip : &renderer->m_clip;
	}
	overlay->SetClippingArea(view, clip);
	WRect dest(pos.x, pos.y, width, height);
	// 0xcfea80: 4.0f.
	if (width < 4.0f) {
		overlay->Render(g_view, s_emptyRect, dest, 0, ScaleColorAlpha(0xffffffff, alpha), 0.0f, 0);
	} else {
		overlay->Render(g_view, s_fullRect, dest, 0, ScaleColorAlpha(0xffffffff, alpha), 0.0f, 0);
	}
}

// 0x5f5900
void RefreshRenderer::Draw(RFTexture* texture, RFVector2 const& pos, RFSize const& size, float alpha, float, TileModeTypes::Type const&) {
	if (texture == NULL) {
		return;
	}
	WOverlay* overlay = static_cast<RefreshTexture*>(texture)->GetOverlay();
	if (overlay != NULL) {
		DrawOverlay(overlay, pos, size, alpha, false);
	}
}

// 0x5f5950
void RefreshRenderer::PushClipRect(RFRect const& rect) {
	WRect clip(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	m_clip = clip;
	m_baseClip = clip;
	RFTextManager* textManager = RFTextManager::Instance();
	if (textManager != NULL && textManager->GetTextDraw() != NULL) {
		textManager->GetTextDraw()->SetClippingArea(&m_clip);
		textManager->GetTextDraw()->SetClippingArea11(&m_clip);
	}
	++m_clipDepth;
}

// 0x5f59d0
void RefreshRenderer::PopClipRect() {
	if (m_clipDepth > 0) {
		// The original reads RFManager +0x4c (the root window) directly.
		RFSize root = RFSceneManager::Instance()->GetManager()->GetRootWnd()->GetSize();
		WRect clip(0.0f, 0.0f, root.width, root.height);
		m_clip = clip;
		RFTextManager* textManager = RFTextManager::Instance();
		if (textManager != NULL && textManager->GetTextDraw() != NULL) {
			textManager->GetTextDraw()->SetClippingArea(&clip);
			textManager->GetTextDraw()->SetClippingArea11(&clip);
		}
		--m_clipDepth;
	}
}

// 0x5f5a80
void RefreshRenderer::FillRectangle(uint32_t color, float x, float y, float w, float h, float) {
	WView* view = g_view;
	if (view == NULL) {
		return;
	}
	bool clipped = false;
	if (m_clip.w != 0.0f && m_clip.h != 0.0f) {
		clipped = true;
		view->SetClippingArea(m_clip);
	}
	FillBox(RFVector2(x, y), RFSize(w, h), color);
	if (clipped) {
		// (0, 0, WView +0x19c, +0x1a0).
		g_view->ResetClippingArea();
	}
}

// 0x5f5430 (scalar deleting destructor 0x5f58d0): the body stores the
// constructor's values again.
RefreshRenderer::~RefreshRenderer() {
	m_device = NULL;
	m_clip = s_emptyRect;
	m_baseClip = s_emptyRect;
	m_clipDepth = 0;
}

// 0x5f5490
void RefreshRenderer::ResetClipRect() {
	// The original reads RFManager +0x4c (the root window) directly.
	RFSize root = RFSceneManager::Instance()->GetManager()->GetRootWnd()->GetSize();
	WRect clip(0.0f, 0.0f, root.width, root.height);
	m_clip = clip;
	m_baseClip = clip;
}
