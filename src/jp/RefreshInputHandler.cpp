// The JP 983 game's RefreshInputHandler and RefreshMouseState
// (RefreshInputHandler.h), from the JP exe's bodies (node scripts/jp_hlil.ts).
#include "RefreshInputHandler.h"

#include "InputManager.h"
#include "RFSceneManager.h"
// <unistd.h>'s access() mode would replace RFManager.h's R_OK enumerator.
#undef R_OK
#include "RFManager.h"
#include "RFWnd.h"

#include <string>

// 0x5f3f20
RefreshMouseState::RefreshMouseState() {
	Reset();
}

// 0x5f38e0
void RefreshMouseState::Reset() {
	m_pos = WVector::ZERO;
	m_wheel = 0.0f;
}

// 0x5f3900
int RefreshMouseState::Poll(CInputManager* input) {
	int flags = 0;
	if (input != NULL) {
		bool doubleClick = input->GetDoubleClick(LEFT_BUTTON);
		int button = input->GetButton(LEFT_BUTTON);
		if (button == 1) {
			flags = doubleClick ? 4 : 1;
		} else if (button == 2) {
			flags = 2;
		}
		button = input->GetButton(RIGHT_BUTTON);
		if (button == 1) {
			flags |= 8;
		} else if (button == 2) {
			flags |= 0x10;
		}
		button = input->GetButton(MIDDLE_BUTTON);
		if (button == 1) {
			flags |= 0x20;
		} else if (button == 2) {
			flags |= 0x40;
		}
		float wheel = input->GetMouseDelta().z;
		if (wheel != m_wheel) {
			m_wheel = wheel;
			flags |= 0x80;
		}
		if (flags == 0) {
			WVector point = input->GetMousePoint();
			// 1e-5f (0xd12fbc)
			if (!WisEqual(m_pos, point, 1e-5f)) {
				m_pos = point;
			}
			return 0x100;
		}
	}
	return flags;
}

// 0x5f3a00
void RefreshInputHandler::DispatchMouse(float dt) {
	if (m_disabled) {
		m_mouse.Poll(g_input);
		return;
	}
	int flags = m_mouse.Poll(g_input);
	RFSceneManager* scene = RFSceneManager::Instance();
	if (flags & 1) {
		scene->InjectLButtonDown(m_mouse.m_pos.x, m_mouse.m_pos.y);
	} else if (flags & 2) {
		scene->InjectLButtonUp(m_mouse.m_pos.x, m_mouse.m_pos.y);
	} else if (flags & 4) {
		scene->InjectLButtonDblClick(m_mouse.m_pos.x, m_mouse.m_pos.y);
	} else if (flags & 8) {
		scene->InjectRButtonDown(m_mouse.m_pos.x, m_mouse.m_pos.y);
	} else if (flags & 0x10) {
		scene->InjectRButtonUp(m_mouse.m_pos.x, m_mouse.m_pos.y);
	} else if (flags & 0x80) {
		int delta = (int)m_mouse.m_wheel;
		if (delta != 0 && m_wheelTimer == 0.0f) {
			scene->InjectMouseWheel(delta, m_mouse.m_pos.x, m_mouse.m_pos.y);
			m_wheelTimer = m_wheelDelay;
		}
	}
	if (flags & 0x100) {
		scene->InjectMouseMove(m_mouse.m_pos.x, m_mouse.m_pos.y);
	}
	if (m_wheelTimer > 0.0f) {
		m_wheelTimer -= dt;
		if (m_wheelTimer < 0.0f) {
			m_wheelTimer = 0.0f;
		}
	}
}

// 0x5f3b90
int RefreshInputHandler::PollMouse() {
	if (g_input == NULL) {
		return 0;
	}
	return m_mouse.Poll(g_input);
}

// 0x5f4870
void RefreshInputHandler::DispatchKeys() {
	if (m_disabled) {
		return;
	}
	std::map<UINT, std::pair<WPARAM, LPARAM> >::iterator it;
	for (it = m_keyMessages.begin(); it != m_keyMessages.end(); ++it) {
		if (it->first == WM_KEYDOWN) {
			RFSceneManager::Instance()->InjectKeyDown(it->second.first, it->second.second);
		} else if (it->first == WM_KEYUP) {
			RFSceneManager::Instance()->InjectKeyUp(it->second.first, it->second.second);
		}
	}
}

// 0x5f4b00
void RefreshInputHandler::Update(float dt) {
	if (RFSceneManager::Instance() == NULL) {
		return;
	}
	DispatchKeys();
	DispatchMouse(dt);
	m_keyMessages.clear();
	m_locked = false;
}

// 0x5f4da0
RefreshInputHandler::RefreshInputHandler()
	: m_locked(false) {
	// 0.2f (0xd097cc)
	m_wheelDelay = 0.2f;
	m_disabled = false;
	m_wheelTimer = m_wheelDelay;
}

// 0x5f4e60
void RefreshInputHandler::QueueMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
	m_lock.Enter();
	if (!m_locked) {
		m_keyMessages[msg] = std::pair<WPARAM, LPARAM>(wParam, lParam);
	}
	m_lock.Leave();
}

// 0x604320 (thiscall on RFSceneManager, defined here until the scene
// manager's source declares it): whether a Refresh window other than the
// desktop is under the screen point (RFManager::HitTest2 with both flags);
// false without a scene manager or its RFManager. The lounge (JP
// ntAvatarChatMain::OnProcess 0x7aa60b) and the lobby avatar (0x566cd0) skip
// their own mouse handling on it.
bool Jp_RefreshScene_IsOverWindow(float x, float y) {
	RFSceneManager* scene = RFSceneManager::Instance();
	if (scene == NULL || scene->GetManager() == NULL) {
		return false;
	}
	RFWnd* wnd = scene->GetManager()->FindWnd(x, y);
	if (wnd == NULL) {
		return false;
	}
	std::wstring wndName = wnd->GetName();
	std::wstring desktopName;
	RFManager* manager = RFSceneManager::Instance() != NULL ? RFSceneManager::Instance()->GetManager() : NULL;
	RFWnd* desktop = manager->GetDesktop();
	if (desktop != NULL) {
		desktopName = desktop->GetName();
	}
	if (desktopName.compare(0, desktopName.size(), wndName.c_str(), wndName.size()) == 0) {
		return false;
	}
	return true;
}
