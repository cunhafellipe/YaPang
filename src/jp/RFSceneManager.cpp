// The JP 983 game's RFSceneManager (RFSceneManager.h), from the JP exe's
// bodies (node scripts/disasm.ts --jp). The game objects it reaches: the
// mouse cursor (0xe73e0c) is the KR CMouseCursor, reached through
// CMouseCursor::Instance(); the JP Fresh (0xf022d0) is the KR g_pFresh,
// whose FrWndManager carries the JP-only members under JP_PAKS; the
// layout list (0xe74d18) is declared below with the members it reads.
#include "RFSceneManager.h"

#include "RFFormDesktopBase.h"
#include "RFLobbyDesktop.h"
#include "RFManager.h"
#include "RFSize.h"
#include "RFTooltipHelper.h"
#include "RFVector2.h"
#include "RFWnd.h"
#include "RefreshInputHandler.h"
#include "RefreshInterface.h"
#include "RefreshRenderer.h"

#include "fresh.h"
#include "frwnd.h"
#include "frwndmanager.h"
#include "mousecursor.h"
#include "wtypes.h"

#include <vector>

// 0xf023ec: the game window (ProjectG.cpp).
extern HWND g_hwnd;

// 0xe73e0c is the JP WSingleton<CMouseCursor>::m_pInstance: 0x891000 is
// the KR CMouseCursor::GetCursor 0x63afa0 instruction for instruction (the
// override flag at JP +0x126, KR m_cursorOverride +0x123), and 0x891020 is
// the KR SetCursor(int) 0x63afc0 behind two JP-only gates: the JP-only
// members of the KR CMouseCursor under JP_PAKS (mousecursor.h): the lock
// IsLocked/SetLocked (JP +0x138; SetCursor does nothing while it is set,
// UpdateMouseOver skips) and SetCursor(int cursor, int owner) (owner JP
// +0x13c; a call from another owner changes only cursor 0); JP has 15
// cursors (override from 0xf) where KR has 12 (0xc).

// 0xe74d18: an unidentified JP-only BaseObject singleton (constructor
// 0xb13e70, vtable 0xd513d4) holding objects whose FrWnd (+0x90) it lays
// out again after a resolution change: a column-major grid of 80-pixel
// cells from (5, 185), as many rows as fit above the bottom 340 pixels of
// the root window. The KR game has none: NULL in the port.
struct JpFrWndLayoutEntry {
	PADDING(0x0, 0x90);
	FrWnd* m_wnd;  // 0x90
};
CHECK_FIELD(JpFrWndLayoutEntry, m_wnd, 0x90);
class JpFrWndLayoutList {
public:
	void PlaceEntry(unsigned int index, WRect* rect);  // 0xb14110
	void Relayout();                                   // 0xb141b0

	PADDING(0x0, 0x28);
	std::vector<JpFrWndLayoutEntry*> m_entries;  // 0x28
};
CHECK_FIELD(JpFrWndLayoutList, m_entries, 0x28);
JpFrWndLayoutList* g_jpFrWndLayoutList = NULL;  // 0xe74d18

// 0xb14110: moves `rect` to the grid cell of entry `index` (this unused).
void JpFrWndLayoutList::PlaceEntry(unsigned int index, WRect* rect) {
	if (rect != NULL) {
		float height;
		RFManager* manager = RFSceneManager::Instance()->GetManager();
		if (manager != NULL) {
			height = manager->GetRootWnd()->GetSize().height;
		} else {
			height = 600.0f;  // 0xd05628
		}
		// 340.0 (0xd33068), 80.0 (0xd06b38)
		unsigned int rows = (unsigned int)((height - 340.0) / 80.0);
		rect->x = (float)(index / rows * 80 + 5);
		// 100.0 (0xd66ca8)
		rect->y = (float)((double)(index % rows * 80 + 5) + 100.0 + 80.0);
	}
}

// 0xb141b0: only while the lobby desktop is up in lobby mode.
void JpFrWndLayoutList::Relayout() {
	if (RFLobbyDesktop::s_instance != NULL && RFLobbyDesktop::s_mode == 0) {
		for (unsigned int i = 0; i < m_entries.size(); ++i) {
			JpFrWndLayoutEntry* entry = m_entries[i];
			if (entry != NULL && entry->m_wnd != NULL) {
				FrWnd* wnd = entry->m_wnd;
				WRect rect;
				// 0xbe79a0, 0xbe8940: the JP FrWnd::GetClientRect and
				// SetClientRect (0xbe8940 adds the parents' positions and
				// calls SetRect, as the KR SetClientRect 0x88e7e0).
				wnd->GetClientRect(rect);
				PlaceEntry(i, &rect);
				wnd->SetClientRect(rect);
			}
		}
	}
}

// 0xe73ddc
template <> RFSceneManager* WSingleton<RFSceneManager>::m_pInstance = NULL;

// 0x604090
RFSceneManager::RFSceneManager()
	: m_manager(NULL), m_interface(NULL), m_renderer(NULL) {
	m_input = RF_SHARED_PTR<RefreshInputHandler>((RefreshInputHandler*)NULL);
	m_enabled = true;
	m_mouseOverForm = false;
	m_cursorLocked = false;
	m_unknown3c = NULL;
}

// 0x603d90. The manager is deleted while Instance() and m_manager still
// point at it, as in the exe. ~RFManager runs m_root->DeleteAllChildren()
// before freeing m_root: each desktop is popped from the root, then
// RFWnd::Destroy -> RFFormDesktopBase::OnDestroy calls DeleteDesktop on this
// still-valid manager; DeleteWnd(m_desktop) then finds the desktop no longer
// among the root's children and deletes nothing, so there is no double
// delete and no re-entry into the window being destroyed.
RFSceneManager::~RFSceneManager() {
	delete RFTooltipHelper::Instance();
	if (m_manager != NULL) {
		delete m_manager;
		m_manager = NULL;
	}
	if (m_interface != NULL) {
		delete m_interface;
		m_interface = NULL;
	}
	if (m_renderer != NULL) {
		delete m_renderer;
		m_renderer = NULL;
	}
	m_unknown3c = NULL;
}

// 0x603e80
void RFSceneManager::Update(float dt) {
	if (m_manager != NULL) {
		m_manager->InjectUpdate(dt);
	}
	if (m_input.get() != NULL) {
		m_input->Update(dt);
	}
	UpdateMouseOver();
	UpdateInputFocus();
}

// 0x604160
bool RFSceneManager::Setup() {
	m_interface = new RefreshInterface;
	m_renderer = new RefreshRenderer;
	m_manager = new RFManager;
	m_input = RF_SHARED_PTR<RefreshInputHandler>(new RefreshInputHandler);
	m_manager->SetInterface(m_interface);
	m_manager->SetRenderer(m_renderer);
	m_manager->InitializeInputMethod(g_hwnd, true);
	// 800.0f (0xd0562c), 600.0f (0xd05628)
	SetResolution(800.0f, 600.0f);
	// The call target is SetVisualEffect(RFVisualEffect*) 0x418860; a
	// pointer below 0x10 is an effect index (1: Fade).
	m_manager->SetVisualEffect(reinterpret_cast<RFVisualEffect*>(1));
	new RFTooltipHelper;
	return true;
}

// 0x6035b0
void RFSceneManager::SetResolution(float width, float height) {
	if (m_manager != NULL) {
		m_manager->GetRootWnd()->SetSize(RFSize(width, height));
		m_manager->GetRootWnd()->UpdateLayout();
		m_renderer->ResetClipRect();
		if (g_jpFrWndLayoutList != NULL) {
			g_jpFrWndLayoutList->Relayout();
		}
	}
}

// 0x603610
void RFSceneManager::SetFreshInputDisabled(bool disabled) {
	if (g_pFresh != NULL && g_pFresh->GetManager() != NULL) {
		g_pFresh->GetManager()->SetKeyInputDisabled(disabled);
		g_pFresh->GetManager()->SetInputSuspended(disabled);
	}
}

// 0x603660
void RFSceneManager::SetFreshInputSuspended(bool suspended) {
	if (g_pFresh != NULL && g_pFresh->GetManager() != NULL) {
		g_pFresh->GetManager()->SetInputSuspended(suspended);
	}
}

// 0x603680
bool RFSceneManager::IsFreshInputDisabled() {
	if (g_pFresh != NULL && g_pFresh->GetManager() != NULL) {
		FrWndManager* manager = g_pFresh->GetManager();
		if (manager->IsKeyInputDisabled() || manager->IsInputSuspended()) {
			return true;
		}
	}
	return false;
}

// 0x6036d0
bool RFSceneManager::IsFreshInputSuspended() {
	if (g_pFresh != NULL && g_pFresh->GetManager() != NULL) {
		return g_pFresh->GetManager()->IsInputSuspended();
	}
	return false;
}

// 0x603710
void RFSceneManager::SetMouseCursor(int cursor, bool lock) {
	if (CMouseCursor::Instance() != NULL) {
		CMouseCursor::Instance()->SetLocked(false);
		CMouseCursor::Instance()->SetCursor(cursor, 1);
		CMouseCursor::Instance()->SetLocked(lock);
		m_cursorLocked = lock;
	}
}

// 0x603750
int RFSceneManager::GetMouseCursor() {
	if (CMouseCursor::Instance() != NULL) {
		return CMouseCursor::Instance()->GetCursor();
	}
	return 0;
}

// 0x603290
void RFSceneManager::PreDraw() {
	if (m_enabled && m_manager != NULL) {
		m_manager->InjectPreDraw();
	}
}

// 0x6032b0
void RFSceneManager::Draw() {
	if (m_enabled && m_manager != NULL) {
		m_manager->InjectDraw();
	}
}

// 0x6032d0
bool RFSceneManager::InjectLButtonDown(float x, float y) {
	if (m_manager != NULL) {
		return m_manager->InjectLButtonDown(x, y);
	}
	return false;
}

// 0x603300
bool RFSceneManager::InjectLButtonUp(float x, float y) {
	if (m_manager != NULL) {
		return m_manager->InjectLButtonUp(x, y);
	}
	return false;
}

// 0x603330
bool RFSceneManager::InjectLButtonDblClick(float x, float y) {
	if (m_manager != NULL) {
		return m_manager->InjectLButtonDblClick(x, y);
	}
	return false;
}

// 0x603360
bool RFSceneManager::InjectRButtonDown(float x, float y) {
	if (m_manager != NULL) {
		return m_manager->InjectRButtonDown(x, y);
	}
	return false;
}

// 0x603390
bool RFSceneManager::InjectRButtonUp(float x, float y) {
	if (m_manager != NULL) {
		return m_manager->InjectRButtonUp(x, y);
	}
	return false;
}

// 0x6033c0
bool RFSceneManager::InjectMouseMove(float x, float y) {
	if (m_manager != NULL) {
		return m_manager->InjectMouseMove(x, y);
	}
	return false;
}

// 0x6033f0
bool RFSceneManager::InjectMouseWheel(int delta, float x, float y) {
	if (m_manager != NULL) {
		return m_manager->InjectMouseWheel(delta, x, y);
	}
	return false;
}

// 0x603420
bool RFSceneManager::InjectKeyDown(UINT key, UINT flags) {
	if (m_manager != NULL) {
		return m_manager->InjectKeyDown(key, flags);
	}
	return false;
}

// 0x603440
bool RFSceneManager::InjectKeyUp(UINT key, UINT flags) {
	if (m_manager != NULL) {
		return m_manager->InjectKeyUp(key, flags);
	}
	return false;
}

// 0x603850
void RFSceneManager::OnWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_SETFOCUS:
		// Through the instance pointer, not this.
		if (Instance() != NULL && Instance()->m_manager != NULL) {
			Instance()->m_manager->RestoreInputMethod();
		}
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_LBUTTONUP:
		if (m_input.get() != NULL) {
			m_input->QueueMessage(msg, wParam, lParam);
		}
		break;
	}
}

// 0x6038b0
WVector const& RFSceneManager::GetMousePosition() const {
	if (m_input.get() != NULL) {
		return m_input->GetMouse().m_pos;
	}
	return WVector::ZERO;
}

// 0x6038d0
float RFSceneManager::GetMouseWheel() const {
	if (m_input.get() != NULL) {
		return m_input->GetMouse().m_wheel;
	}
	return 0.0f;
}

// 0x6038f0
void RFSceneManager::SetInputDisabled(bool disabled) {
	RFSceneManager* scene = Instance();
	if (scene != NULL) {
		RefreshInputHandler* input = scene->m_input.get();
		if (input != NULL) {
			input->SetDisabled(disabled);
			input->GetMouse().Reset();
		}
	}
}

// 0x603920
bool RFSceneManager::IsInputDisabled() {
	RFSceneManager* scene = Instance();
	if (scene != NULL) {
		RefreshInputHandler* input = scene->m_input.get();
		if (input != NULL) {
			return input->IsDisabled();
		}
	}
	return false;
}

// 0x6039a0
void RFSceneManager::UpdateInputFocus() {
	RFManager* manager = Instance() != NULL ? Instance()->m_manager : NULL;
	// The desktop is registered only by RFFormDesktopBase<RFWndEx>::
	// ShowDesktop (0x5fb820), the exe's one desktop base. The exe reads the
	// int at its +0x17c (0x6039d1): RFWndEx::m_brushDesc, not
	// m_pendingTaskChange (+0x184; RFFormDesktopBase.h,
	// IsFocusLayoutValuePositive). That int is > 0 and never a modal count
	// when the brush is set, 0 otherwise.
	RFFormDesktopBase<RFWndEx>* desktop =
		static_cast<RFFormDesktopBase<RFWndEx>*>(manager->GetDesktop());
	if (desktop != NULL) {
		if (g_pFresh == NULL) {
			return;
		}
		FrWndManager* manager = g_pFresh->GetManager();
		if (manager != NULL) {
			bool otherLayout;
			if (desktop->IsFocusLayoutValuePositive()) {
				otherLayout = true;
			} else {
				otherLayout = manager->GetModalFormCount() != 0;
			}
			if (otherLayout) {
				return;
			}
		}
	}
	if (g_pFresh == NULL || g_pFresh->GetManager() == NULL) {
		return;
	}
	if (g_pFresh->GetManager()->GetModalFormCount() != 0) {
		RFManager* modalManager = Instance() != NULL ? Instance()->m_manager : NULL;
		if (!modalManager->IsModalExist()) {
			if (!IsInputDisabled()) {
				SetInputDisabled(true);
			}
			if (IsFreshInputDisabled()) {
				SetFreshInputDisabled(false);
			}
			return;
		}
	}
	if (IsInputDisabled()) {
		SetInputDisabled(false);
	}
}

// 0x603b90
void RFSceneManager::UpdateMouseOver() {
	if (IsInputDisabled()) {
		return;
	}
	if (CMouseCursor::Instance() == NULL || m_input.get() == NULL
		|| CMouseCursor::Instance()->IsLocked()) {
		return;
	}
	WVector const& mouse = m_input->GetMouse().m_pos;
	RFVector2 pos(mouse.x, mouse.y);
	int cursor = 0;
	RFWnd* wnd = m_manager->HitTest2(pos, true, true);
	if (wnd != NULL) {
		switch (wnd->GetType()) {
		case WndType_Button:
		case WndType_LabelButton:
		case WndType_AnimButton:
			cursor = wnd->IsWindowVisible(false) ? 1 : 0;
			break;
		case WndType_Picture:
			cursor = 0;
			break;
		case WndType_Edit:
			if (wnd->IsFocusable() || wnd == m_manager->GetFocusWnd()) {
				cursor = 0xc;
			} else {
				cursor = 0;
			}
			break;
		}
		if (wnd->IsDragable()) {
			cursor = wnd->GetType() != WndType_Button ? 8 : 0;
		}
	}
	if (GetMouseCursor() != cursor) {
		SetMouseCursor(cursor, false);
	}
}

// 0x603ca0
void RFSceneManager::SetAllInputDisabled(bool disabled) {
	SetFreshInputDisabled(disabled);
	SetInputDisabled(disabled);
}
