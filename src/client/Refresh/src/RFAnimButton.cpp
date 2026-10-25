// Original: client\Refresh\src\RFAnimButton.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include <new>

#include "RFAnimButton.h"

#include "RFAnimation.h"
#include "RFDirector.h"
#include "RFEmptyString.h"
#include "RFEventArgs.h"
#include "RFManager.h"
#include "RFRenderer.h"
#include "RFVector2.h"

// lib: 0x414940, 0x4149a0 (the implicit copy constructor and operator=),
// 0x414a30 (vector deleting destructor ??_E), 0x414a00 (scalar deleting
// destructor)

// A button drawn by the scenes of its RFDirector (m_director) instead of
// images: LoadResource loads the animation file of each state image (only
// the normal one when m_singleAnimation is set), and the state handlers play
// the scene GetAnimName names for the state. Entering and leaving play the
// ENTER/LEAVE scenes first (m_pendingState); OnUpdate plays the state's own
// scene once that one has stopped.
//
// GetAnimName's assert must stay on the original's line 134 (its __LINE__ is
// pushed to _wassert): keep this file head's line count when editing above
// GetAnimName.

// The animation (scene) names of the states 0-5 GetAnimName returns
// (unexported, 0xe46ecc-0xe46f58).
static std::wstring const s_animNormal = L"NORMAL";     // 0xe46ecc
static std::wstring const s_animDown = L"DOWN";         // 0xe46ee8
static std::wstring const s_animOver = L"OVER";         // 0xe46f04
static std::wstring const s_animDisable = L"DISABLE";   // 0xe46f20
static std::wstring const s_animEnter = L"ENTER";       // 0xe46f3c
static std::wstring const s_animLeave = L"LEAVE";       // 0xe46f58
// The shared empty string (0xe46e94, RFEmptyString.h), GetAnimName's default
// for a state above 5. Its defining unit is not identified: defined here as a
// stand-in.
std::wstring const g_emptyWString;

// The class name, the static const at 0xe46f74.
std::wstring const RFAnimButton::TypeName = L"AnimButton";

// 0x4344f0
RFAnimButton::RFAnimButton()
	: m_director(0),
	  m_pendingState(0),
	  m_singleAnimation(false) {
}

// 0x434560
RFAnimButton::~RFAnimButton() {
}

// 0x434570
void RFAnimButton::Create(WndBaseStruct* data, RFWnd* parent) {
	RFButton::Create(data, parent);
	AnimButtonBase* animData = dynamic_cast<AnimButtonBase*>(data);
	m_state4Image = animData->m_state4Image;
	m_state5Image = animData->m_state5Image;
	m_singleAnimation = animData->m_singleAnimation;
}

// 0x4345d0
void RFAnimButton::PreInitialize() {
	RFButton::PreInitialize();
	m_director = new RFDirector;
	m_pendingState = 0;
}

// 0x434650 (RFButton::OnDestroy is inlined across units by LTCG)
void RFAnimButton::OnDestroy() {
	if (m_director != 0) {
		delete m_director;
		m_director = 0;
	}
	RFButton::OnDestroy();
}

// 0x4346a0 (RFDirector::Play is inlined across units by LTCG)
void RFAnimButton::LoadResource() {
	if (m_singleAnimation) {
		LoadAnimation_(static_cast<SingleImageSet*>(m_stateImages[RFBUTTON_STATE_NORMAL])->m_fileName);
	} else {
		LoadAnimation_(static_cast<SingleImageSet*>(m_stateImages[RFBUTTON_STATE_NORMAL])->m_fileName);
		LoadAnimation_(static_cast<SingleImageSet*>(m_stateImages[RFBUTTON_STATE_DOWN])->m_fileName);
		LoadAnimation_(static_cast<SingleImageSet*>(m_stateImages[RFBUTTON_STATE_OVER])->m_fileName);
		LoadAnimation_(static_cast<SingleImageSet*>(m_stateImages[RFBUTTON_STATE_DISABLE])->m_fileName);
		LoadAnimation_(static_cast<SingleImageSet*>(m_state4Image)->m_fileName);
		LoadAnimation_(static_cast<SingleImageSet*>(m_state5Image)->m_fileName);
	}
	m_director->Play(GetAnimName(m_state));
}

// 0x434770 (RFDirector::Play is inlined across units by LTCG)
void RFAnimButton::LoadAnimation(std::wstring const& name) {
	if (m_director != 0) {
		delete m_director;
		m_director = 0;
	}
	m_director = new RFDirector;
	LoadAnimation_(name);
	m_director->Play(GetAnimName(GetState()));
}

// 0x434840
void RFAnimButton::UnloadAnimation() {
	if (m_director != 0) {
		delete m_director;
		m_director = 0;
	}
}

// 0x434870 (RFDirector::Play is inlined across units by LTCG)
R_ERRORTYPE RFAnimButton::SetAnimation(RFDirector* director) {
	if (director != 0) {
		UnloadAnimation();
		m_director = director;
		m_director->Play(GetAnimName(GetState()));
		return R_OK;
	}
	return R_FAIL;
}

// 0x4348e0
void RFAnimButton::SetAnimPos(float x, float y) {
	m_director->SetPosition(RFVector2(x, y));
}

// 0x434910
std::wstring const& RFAnimButton::GetAnimName(int state) const {
	assert(state < 6);
	switch (state) {
	case 0:
		return s_animNormal;
	case 1:
		return s_animDown;
	case 2:
		return s_animOver;
	case 3:
		return s_animDisable;
	case 4:
		return s_animEnter;
	case 5:
		return s_animLeave;
	}
	return g_emptyWString;
}

// 0x4349a0
void RFAnimButton::OnStateChanged() {
	m_director->Play(GetAnimName(GetState()));
	m_pendingState = 0;
}

// 0x4349f0 (RFButton::OnLButtonDown and RFDirector::Play are inlined
// across units by LTCG)
void RFAnimButton::OnLButtonDown(MouseEventArgs& args) {
	RFButton::OnLButtonDown(args);
	if (!m_pushed) {
		m_director->Play(GetAnimName(m_state));
	}
	m_pendingState = 0;
}

// 0x434aa0 (RFDirector::Play, IsLoop and End are inlined across units by
// LTCG)
void RFAnimButton::OnLButtonUp(MouseEventArgs& args) {
	RFButton::OnLButtonUp(args);
	if (!m_pushed) {
		std::wstring const& name = GetAnimName(m_state);
		m_director->Play(name);
		if (!m_director->IsLoop(name)) {
			m_director->End(name);
		}
	}
	m_pendingState = 0;
}

// 0x434b60 (RFButton::OnMouseEnter and RFDirector::Play are inlined across
// units by LTCG)
void RFAnimButton::OnMouseEnter() {
	RFButton::OnMouseEnter();
	if (!m_pushed) {
		// The ENTER scene leads into the state's own when it exists.
		m_pendingState = 4;
		if (m_director->GetAnimation(GetAnimName(m_pendingState)) != 0 && m_state != RFBUTTON_STATE_DOWN) {
			m_director->Play(GetAnimName(m_pendingState));
		} else {
			m_pendingState = 0;
			m_director->Play(GetAnimName(m_state));
		}
	}
}

// 0x434c60 (RFButton::OnMouseLeave, RFWnd::IsWindowEnabled and
// RFDirector::Play are inlined across units by LTCG)
void RFAnimButton::OnMouseLeave() {
	RFButton::OnMouseLeave();
	if (!m_pushed && IsWindowEnabled(false)) {
		// The LEAVE scene leads into the state's own when it exists.
		m_pendingState = 5;
		if (m_director->GetAnimation(GetAnimName(m_pendingState)) != 0) {
			m_director->Play(GetAnimName(m_pendingState));
		} else {
			m_pendingState = 0;
			m_director->Play(GetAnimName(m_state));
		}
	} else {
		m_pendingState = 0;
	}
}

// 0x434d10
void RFAnimButton::OnUpdate(UpdateEventArgs& args) {
	if (m_pendingState != 0) {
		if (!m_director->IsPlaying(GetAnimName(m_pendingState))) {
			m_pendingState = 0;
			m_director->Play(GetAnimName(GetState()));
		}
	}
	RFAnimation* scene = m_director->GetPlayingScene();
	if (scene != 0) {
		scene->FrameMove(args.elapsed);
	}
	RFButton::OnUpdate(args);
}

// 0x434db0 (RFWnd::GetScreenPos, GetAlpha and RFAnimation::Render are
// inlined across units by LTCG)
void RFAnimButton::OnDraw() {
	m_director->SetPosition(GetScreenPos());
	RFRect screenRect = ClientToScreen();
	RFRect clip;
	clip.IntersectRect(m_rc.m_clipRect, screenRect);
	if (m_rc.m_clip) {
		RFManager::s_renderer->PushClipRect(clip);
	}
	float alpha = GetAlpha(false);
	if (m_director->GetPlayingScene() != 0) {
		m_director->GetPlayingScene()->Render(alpha);
	}
	RFRect finalScreenRect = ClientToScreen();
	RFRect screenClip;
	screenClip.IntersectRect(m_rc.m_clipRect, finalScreenRect);
	m_rc.m_clipRect = screenClip;
	if (m_rc.m_clip) {
		RFManager::s_renderer->PopClipRect();
	}
}

// 0x434f30
void RFAnimButton::LoadAnimation_(std::wstring const& name) {
	if (name.size() != 0) {
		m_director->LoadAnimation(name);
	}
}

// 0x434f50 (RFButton::OnEnabled is inlined across units by LTCG)
void RFAnimButton::OnEnabled(bool enabled) {
	RFButton::OnEnabled(enabled);
	m_director->Play(GetAnimName(GetState()));
}
