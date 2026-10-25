// The JP 983 mastery slot animation (JpMasterySlotAni.h), from the JP exe's
// bodies (node scripts/disasm.ts --jp 0x<va>). Its creator uses
// RFManager.h's CreateWnd<T> / CreateDesktopWnd<T> (instances 0x5c5750 /
// 0x5c57e0).
#include "JpMasterySlotAni.h"

#include <string>

#include "JpLobbyDialogs.h"
#include "JpMasterySlot.h"
#include "RFEventArgs.h"
#include "RFManager.h"
#include "RFRect.h"
#include "RFVector2.h"

#include "FxBox.h"
#include "SoundManager.h"
#include "fxsequence.h"
#include "wresrcmng.h"
#include "wview.h"

extern WMatrix g_camera;   // 0xf02430

// 0xe780fc
RFWnd* RFSoltAniWnd::s_loadParent = NULL;

// 0x5c02e0
void RFSoltAniWnd::OnUpdate(UpdateEventArgs& args) {
	RFWnd::OnUpdate(args);
	if (m_fxBox.Finished()) {
		Close(static_cast<RFWNDCLOSESTATE>(1));
		return;
	}
	m_fxBox.ProcessAttacher(args.elapsed * 1000.0);
	m_fxBox.Process(args.elapsed * 1000.0);
}

// 0x5c08b0
void RFSoltAniWnd::PlayStatEffect(int stat) {
	float height = GetRect().Height();
	float width = GetRect().Width();
	RFVector2 left = GetScreenPos();
	RFVector2 top = GetScreenPos();
	m_fxBox.SetScreenCenter(width * 0.5 + left.x, height * 0.5 + top.y);
	const char* name;
	switch (stat) {
	case 0:
		name = "mastery_eff_power.seq";
		break;
	case 1:
		name = "mastery_eff_con.seq";
		break;
	case 3:
		name = "mastery_eff_spin.seq";
		break;
	case 4:
		name = "mastery_eff_curve.seq";
		break;
	default:
		return;
	}
	CFxSequence* sequence = m_fxBox.LoadSequence(name, true, NULL);
	if (sequence == NULL) {
		return;
	}
	float x = GetRect().Width() * 0.5 + GetScreenPos().x;
	float y = GetRect().Height() * 0.5 + GetScreenPos().y;
	// Both coordinates are rounded to an int (__ftol2_sse) before they are
	// stored as floats.
	Jp_FxSequence_SetScreenPos(sequence, (float)(int)x, (float)(int)y);
	Jp_FxSequence_SetPos(sequence, g_jpSlotAniSequencePos);
	m_fxBox.AddSequence(sequence);
	g_audio->PlaySfx("mastery_stat_extend", NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

// 0x5c0a80
void RFSoltAniWnd::OnDraw() {
	RFWndEx::OnDraw();
	if (m_fxBox.Finished()) {
		return;
	}
	bool fog = g_view->SetFogEnable(false);
	g_view->SetCamera(m_camera);
	float clipNear = g_view->GetClipNearValue();
	float clipFar = g_view->GetClipFarValue();
	WView::PROJECTION_MODE mode = g_view->GetProjectionMode();
	// 0xb4f950: the JP GetFOV (out of line).
	float fov = g_view->GetFOV();
	g_view->SetProjectionMode(WView::PERSPECTIVE);
	g_view->SetFOV(1.5707964f);         // 0xcff0ac
	g_view->SetClip(0.5f, 50.0f, false);  // 0xcff0a8
	g_view->UpdateCamera();
	m_fxBox.Display(g_view);
	// JP [4] 0xb4fe00 takes a second argument (1 here) that the KR
	// WView::Flush does not have.
	g_view->Flush(1);
	g_view->SetProjectionMode(mode);
	g_view->SetClip(clipNear, clipFar, false);
	g_view->SetFOV(fov);
	g_view->ResetClippingArea();     // 0xb52890 on (0, 0, width, height)
	g_view->ResetScreenCenter();     // 0xb4fcf0 on (width / 2, height / 2)
	g_view->UpdateCamera();
	g_view->SetFogEnable(fog);
}

// 0x5c0c30 (slot [10] is the jump 0x5c1620)
void RFSoltAniWnd::OnInitialUpdate() {
	m_fxBox.Init(g_resrcmng);
	m_fxBox.SetCamera(&g_camera);
	// JP: m_fxBox's frame time (+0x4, the KR protected CFxBox::m_frameTime)
	// set to 33.333f (0xcff420); the KR CFxBox has no setter for it.
	m_camera = WMatrix::IDENTITY;
	m_camera.pivot = WVector(0.0f, 0.0f, -30.0f);   // 0xd04d04
}

// 0x5c5130
RFSoltAniWnd::RFSoltAniWnd() {
}

// 0x5c51c0
RFSoltAniWnd::~RFSoltAniWnd() {
	m_fxBox.ClearAll(false);
	if (RFCharMasteryTileDlg::s_instance != NULL) {
		RFCharMasteryTileDlg::s_instance->OnSlotAnimationDone();
	}
}

// 0x5c5870
RFWnd* CreateSoltAniWnd(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpModalCreateMode) {
	case 0:
		wnd = CreateWnd<RFSoltAniWnd>(&RFManager::GetInstance(), L"kam_mastery_slotani.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFSoltAniWnd>(&RFManager::GetInstance(), L"kam_mastery_slotani.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFSoltAniWnd>(&RFManager::GetInstance(), L"kam_mastery_slotani.xml");
		break;
	}
	RFSoltAniWnd::s_loadParent = parent;
	return wnd;
}
