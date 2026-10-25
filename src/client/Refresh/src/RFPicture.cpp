// Original: client\Refresh\src\RFPicture.cpp (JP 983 exe, unit bounds in
// jp/units.tsv; the file name is assumed from the class)
#include <new>

#include "RFPicture.h"

#include "RFAnimation.h"
#include "RFEventArgs.h"
#include "RFManager.h"
#include "RFRect.h"
#include "RFRenderer.h"
#include "RFVector2.h"

// lib: 0x416ca0 (the vector deleting destructor); 0x416c70 (the scalar
// deleting destructor ??_GRFPicture, paired with map_symbol.ts); 0x41e0c0, 0x41e0e0,
// 0x41e100, 0x41e3d0, 0x41e2c0, 0x41e3b0 (std::wstring construction and
// _Tidy, LTCG register copies); the implicit copy constructor 0x416bd0 and operator= 0x416c20; the
// scalar deleting destructors of RFReferenceCount 0x41e170, ImageSet
// 0x41e1d0, SingleImageSet 0x41e270 and NinePatchImageSet 0x41e360.

// Not library: the original also kept here LTCG register copies (this in eax)
// of RFBrush.h's header inlines, which our build never emits out of line: the
// constructors of RFReferenceCount 0x41e130, ImageSet 0x41e1a0,
// SingleImageSet 0x41e200 and NinePatchImageSet 0x41e2f0, and
// RFReferenceCount::Release 0x41e150 (game code, left todo).

// The class name, the static const at 0xe46c80.
std::wstring const RFPicture::TypeName = L"RFPicture";

// The image sets' destructors and kinds (RFBrush.h): the original kept
// their copies in this object (their scalar deleting destructors are in the
// lib block above).

// 0x41e140 (each image set's members follow its constructor in the
// original: constructor, destructor, GetType or Release, scalar deleting
// destructor)
RFReferenceCount::~RFReferenceCount() {
}

// 0x41e1b0
ImageSet::~ImageSet() {
}

// 0x41e1c0
ImageSetTypes::Type ImageSet::GetType() const {
	return ImageSetTypes::None;
}

// 0x41e220
SingleImageSet::~SingleImageSet() {
}

// 0x41e260
ImageSetTypes::Type SingleImageSet::GetType() const {
	return ImageSetTypes::Single;
}

// 0x41e310
NinePatchImageSet::~NinePatchImageSet() {
}

// 0x41e350
ImageSetTypes::Type NinePatchImageSet::GetType() const {
	return ImageSetTypes::NinePatch;
}

// 0x41e400
RFPicture::RFPicture() {
}

// 0x41e420
RFPicture::~RFPicture() {
}

// 0x41e430
void RFPicture::Create(WndBaseStruct* wndStruct, RFWnd* parent) {
	RFWnd::Create(wndStruct, parent);
	PictureBase* pictureStruct = static_cast<PictureBase*>(wndStruct);
	m_brushSource = pictureStruct->m_brushSource;
	m_usePicture = pictureStruct->m_usePicture;
}

// 0x41e470
void RFPicture::PreInitialize() {
	RFWnd::PreInitialize();
	m_brush = 0;
	m_director = 0;
}

// 0x41e4d0
void RFPicture::Destroy() {
	if (m_director != 0) {
		delete m_director;
		m_director = 0;
	}
	// The image set is the picture's own (SetPicture) unless the layout
	// data supplied it.
	if (static_cast<PictureBase*>(m_createStruct)->m_brushSource == 0 && m_brushSource != 0) {
		delete m_brushSource;
		m_brushSource = 0;
	}
	RFWnd::Destroy();
}

// 0x41e520 (the original inlines RFDirector::Play(int) 0x4312c0 and its
// live assert, LTCG)
void RFPicture::LoadResource() {
	if (m_usePicture) {
		m_brush = CreateBrush(m_brushSource);
	} else {
		LoadAnimation_(L"ANIMPICTURE", static_cast<SingleImageSet*>(m_brushSource)->m_fileName);
		m_director->Play(0);
	}
}

// 0x41e630
void RFPicture::OnDestroy() {
	if (m_brush != 0) {
		delete m_brush;
		m_brush = 0;
	}
}

// 0x41e660
void RFPicture::OnLayoutChanged(uint32_t flags) {
	if (flags & 2) {
		ApplyAnchor();
	}
	if ((flags & 1) && m_director != 0) {
		RFVector2 pos = GetScreenPos();
		SetAnimPos(pos.x, pos.y);
	}
}

// 0x41e6c0 (the original inlines RFDirector::Play(int) 0x4312c0 and its
// live assert, LTCG)
void RFPicture::LoadAnimation(std::wstring const& fileName) {
	UnloadAnimation();
	m_usePicture = false;
	LoadAnimation_(L"ANIMPICTURE", fileName);
	m_director->Play(0);
}

// 0x41e7d0
void RFPicture::UnloadAnimation() {
	if (m_director != 0) {
		delete m_director;
		m_director = 0;
	}
}

// 0x41e800 (the original inlines RFWnd::GetScreenPos 0x406670 and
// RFDirector::Play(int) 0x4312c0 with its live assert, LTCG)
R_ERRORTYPE RFPicture::SetAnimation(RFDirector* director) {
	if (director != 0) {
		m_usePicture = false;
		UnloadAnimation();
		m_director = director;
		RFVector2 pos = GetScreenPos();
		SetAnimPos(pos.x, pos.y);
		return m_director->Play(0);
	}
	return R_FAIL;
}

// 0x41e900 (the original inlines RFWnd::GetScreenPos 0x406670, LTCG; the
// first argument is not read)
void RFPicture::LoadAnimation_(std::wstring const&, std::wstring const& fileName) {
	m_director = new RFDirector;
	m_director->LoadAnimation(fileName);
	m_director->SetPosition(GetScreenPos());
}

// 0x41e9d0
void RFPicture::SetAnimPos(float x, float y) {
	m_director->SetPosition(RFVector2(x, y));
}

// 0x41ea00 (the original inlines RFDirector::Play(std::wstring const&)
// 0x431250 and its live assert, LTCG)
void RFPicture::Play(std::wstring const& name) {
	if (m_director != 0) {
		m_director->Play(name);
	}
}

// 0x41ea80
void RFPicture::Stop() {
	if (m_director != 0 && m_director->GetPlayingScene() != 0) {
		m_director->GetPlayingScene()->Stop();
	}
}

// 0x41eaa0 (the original inlines RFDirector::IsPlaying 0x431340, LTCG)
bool RFPicture::IsPlaying() const {
	if (m_director != 0) {
		return m_director->IsPlaying();
	}
	return false;
}

// 0x41eac0 (the original inlines RFWnd::IsWindowVisible 0x406d90 and calls
// RFAnimation::FrameMove with this in eax, LTCG)
void RFPicture::OnUpdate(UpdateEventArgs& args) {
	if (IsWindowVisible(false) && !m_usePicture && m_director->GetPlayingScene() != 0) {
		m_director->GetPlayingScene()->FrameMove(args.elapsed);
	}
	RFWnd::OnUpdate(args);
}

// 0x41eb20 (the original inlines RFWnd::GetScreenPos 0x406670 and
// GetAlpha 0x4075c0 in the animation branch and realigns its frame, LTCG)
void RFPicture::OnDraw() {
	RFRect screenRect = ClientToScreen();
	RFRect clip;
	clip.IntersectRect(m_rc.m_clipRect, screenRect);
	if (m_rc.m_clip) {
		RFManager::s_renderer->PushClipRect(clip);
	}
	if (m_usePicture) {
		if (m_brush != 0) {
			float depth = m_rc.m_depth;
			m_brush->Draw(GetScreenPos(), GetSize(), GetAlpha(false), depth);
		}
	} else {
		RFVector2 pos = GetScreenPos();
		SetAnimPos(pos.x, pos.y);
		float alpha = GetAlpha(false);
		if (m_director->GetPlayingScene() != 0) {
			m_director->GetPlayingScene()->Render(alpha);
		}
	}
	RFRect finalScreenRect = ClientToScreen();
	RFRect screenClip;
	screenClip.IntersectRect(m_rc.m_clipRect, finalScreenRect);
	m_rc.m_clipRect = screenClip;
	if (m_rc.m_clip) {
		RFManager::s_renderer->PopClipRect();
	}
}

// 0x41ed00
void RFPicture::SetPicture(std::wstring const& fileName) {
	m_usePicture = true;
	PictureBase* pictureStruct = static_cast<PictureBase*>(m_createStruct);
	if (pictureStruct->m_brushSource != 0) {
		// The layout data's image set is shared: drop its reference.
		if (pictureStruct->m_brushSource->Release() == 0) {
			delete pictureStruct->m_brushSource;
		}
		pictureStruct->m_brushSource = 0;
		m_brushSource = 0;
	} else if (m_brushSource != 0) {
		delete m_brushSource;
		m_brushSource = 0;
	}
	if (m_brush != 0) {
		delete m_brush;
		m_brush = 0;
	}
	if (!fileName.empty()) {
		m_brushSource = new SingleImageSet;
		static_cast<SingleImageSet*>(m_brushSource)->m_fileName = fileName;
		m_brush = CreateBrush(m_brushSource);
	}
}

// 0x41ede0
void RFPicture::SetPatchPicture(std::wstring const& fileName, float top, float bottom, float left, float right) {
	m_usePicture = true;
	PictureBase* pictureStruct = static_cast<PictureBase*>(m_createStruct);
	if (pictureStruct->m_brushSource != 0) {
		if (pictureStruct->m_brushSource->Release() == 0) {
			delete pictureStruct->m_brushSource;
		}
		pictureStruct->m_brushSource = 0;
		m_brushSource = 0;
	} else if (m_brushSource != 0) {
		delete m_brushSource;
		m_brushSource = 0;
	}
	if (m_brush != 0) {
		delete m_brush;
		m_brush = 0;
	}
	if (!fileName.empty()) {
		m_brushSource = new NinePatchImageSet;
		static_cast<NinePatchImageSet*>(m_brushSource)->m_fileName = fileName;
		static_cast<NinePatchImageSet*>(m_brushSource)->m_top = top;
		static_cast<NinePatchImageSet*>(m_brushSource)->m_bottom = bottom;
		static_cast<NinePatchImageSet*>(m_brushSource)->m_left = left;
		static_cast<NinePatchImageSet*>(m_brushSource)->m_right = right;
		m_brush = CreateBrush(m_brushSource);
	}
}

// 0x41eef0
RFSize RFPicture::GetTextureSize() const {
	if (m_usePicture && m_brush != 0) {
		return m_brush->GetSize();
	}
	return RFSize();
}

// 0x41ef30 (the original inlines RFWnd::SetSize 0x406700, LTCG)
void RFPicture::FitToContent() {
	if (m_usePicture && m_brush != 0) {
		SetSize(m_brush->GetSize());
	}
}

