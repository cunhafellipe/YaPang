// Original: client\Refresh\src\RFVisualEffect.cpp (JP 983 exe, unit bounds
// in jp/refresh_units.tsv; the file name is assumed from the class)
#include <new>

#include "RFVisualEffect.h"

#include "RFAnimation.h"
#include "RFDirector.h"
#include "RFWnd.h"

// The base class's members (0x404490-0x4044c0) are header inlines
// (RFVisualEffect.h): their dllexport copies pair here.
// lib: 0x4044d0, 0x404500 (the compiler-generated scalar and vector
// deleting destructors, paired as ??_G / ??_E); 0x42cc30, 0x42cdd0,
// 0x42cf60 (the effects' scalar deleting destructors). 0x42cc20, 0x42cc60,
// 0x42cc70, 0x42ce00, 0x42cf50, 0x42cf90 are unreferenced out-of-line copies
// of the effects' implicit constructors and destructors (this in eax:
// /LTCG), which no source emits.

// 0x42cbc0
RFVisualEffect* RFVisualEffectDefault::Clone() {
	return new RFVisualEffectDefault;
}

// 0x42cbe0
void RFVisualEffectDefault::Reset(RFWnd* wnd) {
	switch (wnd->GetVisualState()) {
	case VisualStateTypes::Opening:
		wnd->SetVisualState(VisualStateTypes::Opened);
		break;
	case VisualStateTypes::Closing:
		wnd->SetVisualState(VisualStateTypes::Closed);
		break;
	}
}

// 0x42cc10
void RFVisualEffectDefault::Update(RFWnd*, float) {
}

// 0x42cc80
RFVisualEffect* RFVisualEffectFade::Clone() {
	return new RFVisualEffectFade;
}

// 0x42cca0
void RFVisualEffectFade::Reset(RFWnd* wnd) {
	if (wnd->GetVisualState() == VisualStateTypes::Opening) {
		wnd->SetAlpha(0.0f);
	}
	m_elapsed = 0.0f;
}

// 0x42ccc0
void RFVisualEffectFade::Update(RFWnd* wnd, float elapsed) {
	m_elapsed += elapsed;
	if (m_elapsed > 0.4f) {
		m_elapsed = 0.4f;
	}
	switch (wnd->GetVisualState()) {
	case VisualStateTypes::Opening: {
		float alpha = m_elapsed / 0.4f;
		wnd->SetAlpha(alpha);
		if (alpha >= 1.0f) {
			wnd->SetVisualState(VisualStateTypes::Opened);
		}
		break;
	}
	case VisualStateTypes::Closing: {
		float alpha = 1.0f - m_elapsed / 0.4f;
		wnd->SetAlpha(alpha);
		if (alpha <= 0.0f) {
			wnd->SetVisualState(VisualStateTypes::Closed);
		}
		break;
	}
	}
}

// 0x42ce10
RFVisualEffect* RFVisualEffectAnim::Clone() {
	return new RFVisualEffectAnim;
}

// 0x42ce30
void RFVisualEffectAnim::Reset(RFWnd* wnd) {
	RFDirector* director = wnd->GetAnimation();
	switch (wnd->GetVisualState()) {
	case VisualStateTypes::Opening:
		if (director->IsSceneExist(RFWnd::AnimOpening)) {
			director->Play(wnd, RFWnd::AnimOpening);
			director->FrameMove(wnd, 0.0f);
		} else {
			wnd->SetVisualState(VisualStateTypes::Opened);
		}
		break;
	case VisualStateTypes::Closing:
		if (director->IsSceneExist(RFWnd::AnimClosing)) {
			director->Play(wnd, RFWnd::AnimClosing);
		} else {
			wnd->SetVisualState(VisualStateTypes::Closed);
		}
		break;
	}
}

// 0x42cef0
void RFVisualEffectAnim::Update(RFWnd* wnd, float) {
	switch (wnd->GetVisualState()) {
	case VisualStateTypes::Opening:
		if (!wnd->GetAnimation()->IsPlaying()) {
			wnd->SetVisualState(VisualStateTypes::Opened);
		}
		break;
	case VisualStateTypes::Closing:
		if (!wnd->GetAnimation()->IsPlaying()) {
			wnd->SetVisualState(VisualStateTypes::Closed);
		}
		break;
	}
}

// 0x42cfa0
RFVisualEffect* __stdcall CreateVisualEffect(int32_t index) {
	static RFVisualEffectDefault s_default;
	static RFVisualEffectFade s_fade;
	static RFVisualEffectAnim s_anim;
	switch (index) {
	case 0:
		return s_default.Clone();
	case 1:
		return s_fade.Clone();
	case 2:
		return s_anim.Clone();
	}
	return 0;
}
