// Original: client\Refresh\src\RFManager.cpp (JP 983 exe, unit bounds in
// jp/units.tsv; the file name is assumed from the class)
#include "RFManager.h"

#include <new>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "RFAnimButton.h"
#include "RFButton.h"
#include "RFCheck.h"
#include "RFComboBox.h"
#include "RFControlFactories.h"
#include "RFEdit.h"
#include "RFLabel.h"
#include "RFLabelButton.h"
#include "RFListSC.h"
#include "RFPicture.h"
#include "RFPoint.h"
#include "RFProgress.h"
#include "RFScrollBar.h"
#include "RFSlider.h"
#include "RFStackPanel.h"
#include "RFTextAreaSC.h"
#include "RFVisualEffect.h"
#include "RFEventArgs.h"
#include "RFIme.h"
#include "RFInterface.h"
#include "RFWnd.h"
#include "RFWndEx.h"
#include "RFWindowFactory.h"
#include "RFWindowFactoryManager.h"
#include "RFXmlLoader.h"

// lib: 0x403360 (the vector deleting destructor ??_E, compiler-generated;
// paired with map_symbol.ts), 0x40c170 (std::list<std::wstring>::clear)
// lib: 0x403330 (??_GRFManager), 0x4179e0 (??_GRFWindowFactoryManager,
// the delete of m_windowFactoryManager), 0x417a00 (??_GRFRendererLibrary):
// scalar deleting destructors, paired with map_symbol.ts
// lib: 0x413eb0, 0x413f10, 0x413f80, 0x413fe0, 0x4140f0 (the implicit
// destructors of MouseEventArgs, KeyEventArgs, CharEventArgs,
// UpdateEventArgs, HyperLinkEventArgs; paired with map_symbol.ts)
// lib: 0x409610 (std::list<std::wstring> copy constructor), 0x416f40
// (std::wstring destructor of GetUIAnimation's result in CreateWnd),
// 0x417e40, 0x417e60 (std::wstring construction pieces, LTCG copies)

// GetInstance's assert sits on the original's line 58 (`push 0x3a` before
// _wassert): the members 0x402f10-0x4033f0 that come before it in address
// order are RFManager.h inlines.


// 0x417450
RFManager& RFManager::GetInstance() {
	assert(_singleton);
	return *_singleton;
}

// 0x417480
RFManager *RFManager::GetInstancePtr() {
	return RFSingleton<RFManager>::GetInstancePtr();
}

// The renderer DLL SetRenderer(wstring const&, void*) 0x418500 loads; its
// exports createRenderer and deleteRenderer build and free the renderer.
struct RFRendererLibrary {
	RFRendererLibrary(std::wstring const& name)
		: m_module(0) {
		m_module = LoadLibraryW(name.c_str());
	}
	~RFRendererLibrary() {
		if (m_module) {
			FreeLibrary(m_module);
			m_module = 0;
		}
	}
	bool IsLoaded() const {
		return m_module != 0;
	}
	FARPROC GetFunction(std::string const& name) const {
		return GetProcAddress(m_module, name.c_str());
	}

	HMODULE m_module;  // 0x0
};
CHECK_SIZE(RFRendererLibrary, 0x4);

typedef RFRenderer *(*CreateRendererFunc)(void *param, RFInterface *iface);
typedef void (*DeleteRendererFunc)(RFRenderer *renderer);

// The mirrors of the manager's renderer and interface, written by
// SetRenderer (0x4184e0) and SetInterface (0x418640).
RFRenderer *RFManager::s_renderer;
RFInterface *RFManager::s_interface;
RFWnd *RFManager::s_desktop;
RFWnd *RFManager::s_root;

std::wstring const RFManager::EventHyperLink = L"HyperLink";
std::wstring const RFManager::EventGlobalHyperLinkEnter = L"GlobalHyperLinkEnter";
std::wstring const RFManager::EventGlobalHyperLinkLeave = L"GlobalHyperLinkLeave";
std::wstring const RFManager::EventInputMethodStatusChanged = L"IMStatusChanged";

// The blank lines below keep the later asserts on the original's lines.








































































// The control factories, each a function-local static of its getter
// (RFControlFactories.h). The getters belong to the factories' own object,
// RFControlFactories.cpp (0x432660-0x4344f0): defined here, cl would inline
// them into the constructor (static guard, atexit), where the original calls
// them.

// 0x417490
RFManager::RFManager()
	: m_rendererLibrary(0)
	, m_inputsFroze(false)
	, m_root(0)
	, m_desktop(0)
	, m_activeWnd(0)
	, m_focusWnd(0)
	, m_capturedWindow(0)
	, m_wndContainingMouse(0)
	, m_globalPaddingEnabled(false)
	, m_usingCustomInputMethod(false) {
	m_renderer = 0;
	m_interface = 0;
	m_visualEffect = 0;
	m_inputMethod = 0;
	m_windowFactoryManager = new RFWindowFactoryManager;
	m_windowFactoryManager->AddFactory(GetAnimButtonFactory());
	m_windowFactoryManager->AddFactory(GetButtonFactory());
	m_windowFactoryManager->AddFactory(GetCheckFactory());
	m_windowFactoryManager->AddFactory(GetComboBoxFactory());
	m_windowFactoryManager->AddFactory(GetEditFactory());
	m_windowFactoryManager->AddFactory(GetWndExFactory());
	m_windowFactoryManager->AddFactory(GetLabelFactory());
	m_windowFactoryManager->AddFactory(GetLabelButtonFactory());
	m_windowFactoryManager->AddFactory(GetListSCFactory());
	m_windowFactoryManager->AddFactory(GetPictureFactory());
	m_windowFactoryManager->AddFactory(GetProgressFactory());
	m_windowFactoryManager->AddFactory(GetScrollBarFactory());
	m_windowFactoryManager->AddFactory(GetSliderFactory());
	m_windowFactoryManager->AddFactory(GetStackPanelFactory());
	m_windowFactoryManager->AddFactory(GetTextAreaSCFactory());
	SetVisualEffect(CreateVisualEffect(0));
	m_root = new RFWnd;
	m_root->Initialize();
	m_root->m_active = true;
	m_root->SetName(L"__refresh_system_layer__");
	m_root->SetSize(RFSize(1024.0f, 768.0f));
	s_root = m_root;
}

// 0x4177f0 (~RFWindowFactoryManager 0x417280 is inlined under LTCG)
RFManager::~RFManager() {
	if (m_windowFactoryManager) {
		delete m_windowFactoryManager;
		m_windowFactoryManager = 0;
	}
	if (m_visualEffect) {
		delete m_visualEffect;
		m_visualEffect = 0;
	}
	m_root->DeleteAllChildren();
	if (m_root) {
		delete m_root;
		m_root = 0;
	}
	if (m_rendererLibrary) {
		DeleteRendererFunc destroy = (DeleteRendererFunc)m_rendererLibrary->GetFunction(std::string("deleteRenderer"));
		destroy(m_renderer);
		m_renderer = 0;
		s_renderer = 0;
		if (m_rendererLibrary) {
			delete m_rendererLibrary;
			m_rendererLibrary = 0;
		}
	}
	if (m_inputMethod) {
		RFInputMethodService *service = m_inputMethod->m_service;
		if (service) {
			m_inputMethod->m_service = 0;
			delete service;
		}
		if (m_inputMethod) {
			delete m_inputMethod;
			m_inputMethod = 0;
		}
	}
}

// 0x417a90: builds the window a layout file describes (into `wnd` when
// given, else a new control of the file's type) under `parent` or the root.
// A file that does not load is read whole through the interface and nothing
// is built: the catch funclet 0x417d1c opens std::wstring(name.c_str()),
// reads it into a std::vector<char> (ctor 0x41ac00, dtor 0x41ac50), closes
// it and drops the buffer; each of its three exits resumes at 0x417d60,
// which destroys uiAnimation and the loader and returns 0. The original
// discards the buffer.
RFWnd *RFManager::CreateWnd(std::wstring const& name, RFWnd *wnd, RFWnd *parent) {
	RFXmlLoader loader(m_interface);
	std::wstring uiAnimation;
	try {
		loader.Load(m_interface->GetBasePath(RF_BASEPATH_ANIMATION) + name);
		uiAnimation = loader.GetUIAnimation();
		WndBaseStruct *wndStruct = loader.GetRootStruct();
		if (wnd == 0) {
			std::wstring const& typeName = GetCompatibleControlName(wndStruct->GetType());
			RFWindowFactory *factory = RFWindowFactoryManager::GetInstance().GetFactory(typeName);
			if (factory) {
				wnd = factory->Create();
				wnd->Create(wndStruct, 0);
			}
		} else {
			wnd->Create(wndStruct, 0);
		}
		(parent ? parent : m_root)->RegisterChild(wnd);
		CreateChild(loader, wnd, wndStruct);
	} catch (...) {
		void *file = s_interface->FOpen(name.c_str());
		if (file == 0) {
			return 0;
		}
		s_interface->FSeek(file, 0, 2);
		int32_t size = s_interface->FTell(file);
		s_interface->FSeek(file, 0, 0);
		if (size <= 0) {
			s_interface->FClose(file);
			return 0;
		}
		std::vector<char> buffer(size + 1);
		s_interface->FRead(file, &buffer[0], size);
		s_interface->FClose(file);
		return 0;
	}
	wnd->Initialize();
	wnd->SetVisualEffect(m_visualEffect);
	wnd->LoadUIAnimation(uiAnimation);
	wnd->ShowWindow_(false);
	(parent ? parent : s_root)->ApplyAnchor();
	loader.Clear();
	return wnd;
}

// 0x417e70
void RFManager::FreezeInputs(bool freeze) {
	m_inputsFroze = freeze;
}

// 0x417e80
bool RFManager::IsInputsFroze() const {
	return m_inputsFroze;
}

// 0x417e90
void RFManager::CreateChild(RFXmlLoader& loader, RFWnd *parent, WndBaseStruct *parentStruct) {
	int count = loader.GetChildCount(parentStruct);
	for (int i = 0; i < count; ++i) {
		WndBaseStruct *child = loader.GetChild(parentStruct, i);
		std::wstring const& typeName = GetCompatibleControlName(child->GetType());
		RFWindowFactory *factory = RFWindowFactoryManager::GetInstance().GetFactory(typeName);
		if (factory != 0) {
			RFWnd *wnd = factory->Create();
			wnd->Create(child, parent);
			CreateChild(loader, wnd, child);
			wnd->Initialize();
		}
	}
}

// 0x417fd0
void RFManager::PostHyperLinkEvent(RFWnd *wnd, std::wstring const& windowName,
                   std::wstring const& link) {
	HyperLinkEventArgs args(wnd, windowName, link);
	EventMap::iterator it = m_events.find(EventHyperLink);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x4180e0
void RFManager::PostGlobalHyperLinkEnterEvent(RFWnd *wnd, std::wstring const& windowName,
                   std::wstring const& link) {
	HyperLinkEventArgs args(wnd, windowName, link);
	EventMap::iterator it = m_events.find(EventGlobalHyperLinkEnter);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x4181f0
void RFManager::PostGlobalHyperLinkLeaveEvent(RFWnd *wnd) {
	WindowEventArgs_<RFWnd> args(wnd);
	EventMap::iterator it = m_events.find(EventGlobalHyperLinkLeave);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x418270
void RFManager::PostIMStatusEvent(LocaleIndicatorTypes::Type const& indicatorType,
                                  uint32_t const& status) {
	IMStatusEventArgs args(indicatorType, status);
	EventMap::iterator it = m_events.find(EventInputMethodStatusChanged);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x4182f0
void RFManager::ActivateWindow(RFWnd *wnd) {
	if (wnd != 0 && wnd != m_root && wnd != m_activeWnd && wnd->m_overlapped) {
		if (m_activeWnd != 0) {
			m_activeWnd->Deactivate();
		}
		m_activeWnd = wnd;
		wnd->Activate();
		m_activeWnd->m_super->BringChildWndToTop(m_activeWnd);
	}
}

// 0x418340
void RFManager::FocusWindow(RFWnd *wnd) {
	if (wnd == 0) {
		if (m_focusWnd != 0) {
			m_focusWnd->OnKillFocus();
			m_focusWnd = 0;
		}
		return;
	}
	RFWnd *previous = m_focusWnd;
	if (wnd->m_subControl) {
		m_focusWnd = wnd->GetControlSuper();
	} else {
		m_focusWnd = wnd;
	}
	if (previous != m_focusWnd) {
		if (previous != 0) {
			previous->OnKillFocus();
		}
		if (m_focusWnd != 0) {
			m_focusWnd->OnSetFocus();
		}
	}
}

// 0x4183b0
R_ERRORTYPE RFManager::DeleteDesktop() {
	R_ERRORTYPE result = DeleteWnd(m_desktop);
	s_desktop = 0;
	return result;
}

// 0x418410
R_ERRORTYPE RFManager::DeleteWnd(RFWnd *wnd) {
	if (wnd == 0) {
		return R_FAIL;
	}
	NotifyWindowDestroyed(wnd);
	RFWnd *parent = wnd->m_super;
	assert(parent);
	parent->DeleteChild(wnd);
	return R_OK;
}

// 0x418460
void RFManager::DeleteAllWnd() {
	m_root->DeleteAllChildren();
}

// 0x418470
void RFManager::RegisterDesktop(RFWnd *desktop) {
	m_desktop = desktop;
	s_desktop = desktop;
	desktop->SetPriority(1);
	m_root->RegisterChild(desktop);
}

// 0x4184b0
RFWnd *RFManager::GetDesktop() {
	return m_desktop;
}

// 0x4184c0
RFWnd *RFManager::GetWnd(std::wstring const& name) {
	return m_root->GetChild(name);
}

// 0x4184d0
RFWnd *RFManager::GetInputTargetWindow() const {
	if (m_activeWnd != 0) {
		return m_activeWnd->GetActiveChild();
	}
	return 0;
}

// 0x4184e0
void RFManager::SetRenderer(RFRenderer *renderer) {
	m_renderer = renderer;
	s_renderer = renderer;
}

// 0x418500 (the library holder stays dangling when the DLL does not load)
void RFManager::SetRenderer(std::wstring const& library, void *param) {
	delete m_rendererLibrary;
	m_rendererLibrary = new RFRendererLibrary(library);
	if (!m_rendererLibrary->IsLoaded()) {
		delete m_rendererLibrary;
		return;
	}
	CreateRendererFunc create = (CreateRendererFunc)m_rendererLibrary->GetFunction(std::string("createRenderer"));
	m_renderer = create(param, m_interface);
	s_renderer = m_renderer;
}

// 0x418640
void RFManager::SetInterface(RFInterface *iface) {
	m_interface = iface;
	s_interface = iface;
}

// 0x418670
void RFManager::InitializeInputMethod(HWND wnd, bool custom) {
	m_usingCustomInputMethod = custom;
	m_inputMethod = new RFInputMethodProvider;
	if (custom) {
		m_inputMethod->m_service = new RFCustomIme(wnd);
	} else {
		m_inputMethod->m_service = new RFIme(wnd);
	}
	if (m_inputMethod->m_service) {
		m_inputMethod->m_service->SetNativeMode(false);
	}
	if (m_inputMethod->m_service) {
		m_inputMethod->m_service->Enable(false);
	}
}

// 0x4187a0
void RFManager::RestoreInputMethod() {
	if (m_inputMethod && m_inputMethod->m_service) {
		m_inputMethod->m_service->RestoreFocus();
	}
}

// 0x4187c0
RFInputMethodProvider *RFManager::GetInputMethod() {
	return m_inputMethod;
}

// 0x4187d0
void RFManager::BringWndToTop(RFWnd *wnd) {
	m_root->BringChildWndToTop(wnd);
}

// 0x4187e0: windows of the types 0-2, 7, 8 and 14 keep their place
void RFManager::BringControlToTop(RFWnd *wnd) {
	RFWnd *control;
	if (wnd->m_subControl) {
		control = wnd->GetControlSuper();
	} else {
		control = wnd;
	}
	if (control->m_super != 0) {
		switch (control->m_type) {
		case 0: case 1: case 2: case 7: case 8: case 14:
			break;
		default:
			control->m_super->BringChildWndToTop(control);
			break;
		}
	}
}

// 0x418860
void RFManager::SetVisualEffect(RFVisualEffect *effect) {
	if (m_visualEffect) {
		delete m_visualEffect;
		m_visualEffect = 0;
	}
	if (reinterpret_cast<uintptr_t>(effect) < 0x10) {
		effect = CreateVisualEffect(static_cast<int32_t>(reinterpret_cast<uintptr_t>(effect)));
	}
	m_visualEffect = effect;
}

// 0x4188a0
wchar_t const* RFManager::GetVersion() const {
	return L"librefresh-1.5.5 Release (Thu Jun 18 21:25:25 2015) - Japan";
}

// 0x4188b0
RFWnd *RFManager::HitTest(RFVector2 const& pos, bool hitTestThrough) {
	return m_root->HitTestChild(pos, false, hitTestThrough);
}

// 0x4188d0
RFWnd *RFManager::HitTest2(RFVector2 const& pos, bool hitTestThrough, bool controls) {
	RFWnd *modal = GetLastActiveModal();
	if (modal != 0) {
		return modal->HitTestChild(pos, hitTestThrough, controls);
	}
	RFWnd *hit = m_root->HitTestChild(pos, hitTestThrough, controls);
	if (hit == m_root) {
		return 0;
	}
	return hit;
}

// 0x418950
RFWnd *RFManager::GetLastActiveModal() const {
	std::vector<RFWnd*> const& children = m_root->_children;
	std::vector<RFWnd*>::const_reverse_iterator end = children.rend();
	for (std::vector<RFWnd*>::const_reverse_iterator it = children.rbegin(); it != end; ++it) {
		if ((*it)->m_modal) {
			return *it;
		}
	}
	return 0;
}

// 0x418990
bool RFManager::IsModalExist() const {
	std::vector<RFWnd*> const& children = m_root->_children;
	std::vector<RFWnd*>::const_reverse_iterator end = children.rend();
	for (std::vector<RFWnd*>::const_reverse_iterator it = children.rbegin(); it != end; ++it) {
		if ((*it)->m_modal) {
			return true;
		}
	}
	return false;
}

// 0x4189d0
RFWnd *RFManager::FindActiveFocusableWnd(RFWnd *except) {
	std::vector<RFWnd*> const& children = m_root->_children;
	std::vector<RFWnd*>::const_reverse_iterator end = children.rend();
	for (std::vector<RFWnd*>::const_reverse_iterator it = children.rbegin(); it != end; ++it) {
		RFWnd *wnd = *it;
		if (wnd == except) {
			continue;
		}
		if (wnd->m_priority > -1 || wnd->m_modal) {
			if (m_desktop != 0 && wnd == m_desktop) {
				return wnd;
			}
			if (wnd->m_hitTestVisible && wnd->m_hidePending != true && wnd->m_visible) {
				if (wnd->m_super == 0 || wnd->m_super->IsWindowVisible(false)) {
					return wnd;
				}
			}
		}
	}
	return 0;
}

// 0x418a60
void RFManager::UpdateWindowContainingMouse(RFVector2 const& pos) {
	RFWnd *wnd = HitTest2(pos, true, false);
	if (wnd != 0) {
		if (m_wndContainingMouse == 0) {
			m_wndContainingMouse = m_root;
		}
		if (wnd != m_wndContainingMouse) {
			if (m_capturedWindow == 0 || m_wndContainingMouse == m_capturedWindow) {
				m_wndContainingMouse->OnMouseLeave();
			}
			if (m_capturedWindow == 0 || wnd == m_capturedWindow) {
				BringControlToTop(wnd);
				wnd->OnMouseEnter();
			}
			m_wndContainingMouse = wnd;
		}
	}
}

// 0x418ad0
bool RFManager::InjectLButtonDown(float x, float y) {
	if (IsInputsFroze()) {
		return false;
	}
	RFWnd *target = m_capturedWindow;
	if (target == 0) {
		target = HitTest2(RFVector2(x, y), true, false);
		if (target == 0) {
			return false;
		}
	}
	RFWnd *overlapped = target->GetOverlapped();
	if (overlapped != 0) {
		if (overlapped->GetVisualState() == VisualStateTypes::Closing
			|| overlapped->IsInputLocked() == true) {
			return false;
		}
	}
	ActivateWindow(overlapped);
	FocusWindow(target);
	MouseEventArgs args;
	args.window = target;
	args.pos.x = x;
	args.pos.y = y;
	args.button = 0;
	args.wheelDelta = 0.0f;
	RFWnd *wnd = target;
	do {
		wnd->OnLButtonDown(args);
		if (args.handled) {
			break;
		}
		wnd = wnd->m_super;
	} while (wnd != 0);
	return true;
}

// 0x418be0
bool RFManager::InjectLButtonUp(float x, float y) {
	if (IsInputsFroze()) {
		return false;
	}
	RFWnd *target = m_capturedWindow;
	if (target == 0) {
		target = HitTest2(RFVector2(x, y), true, false);
		if (target == 0) {
			return false;
		}
	}
	RFWnd *overlapped = target->GetOverlapped();
	if (overlapped != 0) {
		if (overlapped->GetVisualState() == VisualStateTypes::Closing
			|| overlapped->IsInputLocked() == true) {
			return false;
		}
	}
	MouseEventArgs args;
	args.window = target;
	args.pos.x = x;
	args.pos.y = y;
	args.button = 0;
	args.wheelDelta = 0.0f;
	RFWnd *wnd = target;
	do {
		wnd->OnLButtonUp(args);
		if (args.handled) {
			break;
		}
		wnd = wnd->m_super;
	} while (wnd != 0);
	return true;
}

// 0x418ce0
bool RFManager::InjectLButtonDblClick(float x, float y) {
	if (IsInputsFroze()) {
		return false;
	}
	RFWnd *target = m_capturedWindow;
	if (target == 0) {
		target = HitTest2(RFVector2(x, y), true, false);
		if (target == 0) {
			return false;
		}
	}
	RFWnd *overlapped = target->GetOverlapped();
	if (overlapped != 0) {
		if (overlapped->GetVisualState() == VisualStateTypes::Closing
			|| overlapped->IsInputLocked() == true) {
			return false;
		}
	}
	ActivateWindow(overlapped);
	FocusWindow(target);
	MouseEventArgs args;
	args.window = target;
	args.pos.x = x;
	args.pos.y = y;
	args.button = 0;
	args.wheelDelta = 0.0f;
	RFWnd *wnd = target;
	do {
		wnd->OnLButtonDblClick(args);
		if (args.handled) {
			break;
		}
		wnd = wnd->m_super;
	} while (wnd != 0);
	return true;
}

// 0x418df0
bool RFManager::InjectRButtonDown(float x, float y) {
	if (IsInputsFroze()) {
		return false;
	}
	RFWnd *target = m_capturedWindow;
	if (target == 0) {
		target = HitTest2(RFVector2(x, y), true, false);
		if (target == 0) {
			return false;
		}
	}
	RFWnd *overlapped = target->GetOverlapped();
	if (overlapped != 0) {
		if (overlapped->GetVisualState() == VisualStateTypes::Closing
			|| overlapped->IsInputLocked() == true) {
			return false;
		}
	}
	ActivateWindow(overlapped);
	FocusWindow(target);
	MouseEventArgs args;
	args.window = target;
	args.pos.x = x;
	args.pos.y = y;
	args.button = 0;
	args.wheelDelta = 0.0f;
	RFWnd *wnd = target;
	do {
		wnd->OnRButtonDown(args);
		if (args.handled) {
			break;
		}
		wnd = wnd->m_super;
	} while (wnd != 0);
	return true;
}

// 0x418f00
bool RFManager::InjectRButtonUp(float x, float y) {
	if (IsInputsFroze()) {
		return false;
	}
	RFWnd *target = m_capturedWindow;
	if (target == 0) {
		target = HitTest2(RFVector2(x, y), true, false);
		if (target == 0) {
			return false;
		}
	}
	RFWnd *overlapped = target->GetOverlapped();
	if (overlapped != 0) {
		if (overlapped->GetVisualState() == VisualStateTypes::Closing
			|| overlapped->IsInputLocked() == true) {
			return false;
		}
	}
	ActivateWindow(overlapped);
	FocusWindow(target);
	MouseEventArgs args;
	args.window = target;
	args.pos.x = x;
	args.pos.y = y;
	args.button = 0;
	args.wheelDelta = 0.0f;
	RFWnd *wnd = target;
	do {
		wnd->OnRButtonUp(args);
		if (args.handled) {
			break;
		}
		wnd = wnd->m_super;
	} while (wnd != 0);
	return true;
}

// 0x419010: a move outside the resolution is dropped
bool RFManager::InjectMouseMove(float x, float y) {
	RFSize size = m_root->GetSize();
	if (x < 0.0f || size.width < x || y < 0.0f || size.height < y) {
		return false;
	}
	RFVector2 pos(x, y);
	UpdateWindowContainingMouse(pos);
	m_cursorPos = pos;
	RFWnd *target = m_capturedWindow;
	if (target == 0) {
		target = HitTest2(RFVector2(x, y), true, false);
		if (target == 0) {
			return false;
		}
	}
	RFWnd *overlapped = target->GetOverlapped();
	if (overlapped != 0) {
		if (overlapped->GetVisualState() == VisualStateTypes::Closing
			|| overlapped->IsInputLocked() == true) {
			return false;
		}
	}
	MouseEventArgs args;
	args.window = target;
	args.pos = pos;
	args.button = 3;
	args.wheelDelta = 0.0f;
	RFWnd *wnd = target;
	do {
		wnd->OnMouseMove(args);
		if (args.handled) {
			break;
		}
		wnd = wnd->m_super;
	} while (wnd != 0);
	return true;
}

// 0x419180
bool RFManager::InjectMouseWheel(int32_t delta, float x, float y) {
	if (IsInputsFroze()) {
		return false;
	}
	RFWnd *target = m_capturedWindow;
	if (target == 0) {
		target = HitTest2(RFVector2(x, y), true, false);
		if (target == 0) {
			return false;
		}
	}
	RFWnd *overlapped = target->GetOverlapped();
	if (overlapped != 0) {
		if (overlapped->GetVisualState() == VisualStateTypes::Closing
			|| overlapped->IsInputLocked() == true) {
			return false;
		}
	}
	MouseEventArgs args;
	args.window = target;
	args.pos.x = x;
	args.pos.y = y;
	args.button = 3;
	args.wheelDelta = static_cast<float>(delta);
	RFWnd *wnd = target;
	do {
		wnd->OnMouseWheel(args);
		if (args.handled) {
			break;
		}
		wnd = wnd->m_super;
	} while (wnd != 0);
	return true;
}

// 0x419280
bool RFManager::InjectKeyDown(uint32_t keyCode, uint32_t keyData) {
	if (IsInputsFroze()) {
		return false;
	}
	RFWnd *focus = m_focusWnd;
	if (focus == 0) {
		return false;
	}
	RFWnd *overlapped = focus->GetOverlapped();
	if (overlapped != 0) {
		if (overlapped->GetVisualState() == VisualStateTypes::Closing
			|| overlapped->IsInputLocked() == true) {
			return false;
		}
	}
	KeyEventArgs args(focus, keyCode, keyData);
	RFWnd *wnd = focus;
	do {
		wnd->OnKeyDown(args);
		if (args.handled) {
			break;
		}
		wnd = wnd->m_super;
	} while (wnd != 0);
	return true;
}

// 0x419340
bool RFManager::InjectKeyUp(uint32_t keyCode, uint32_t keyData) {
	if (IsInputsFroze()) {
		return false;
	}
	RFWnd *focus = m_focusWnd;
	if (focus == 0) {
		return false;
	}
	RFWnd *overlapped = focus->GetOverlapped();
	if (overlapped != 0) {
		if (overlapped->GetVisualState() == VisualStateTypes::Closing
			|| overlapped->IsInputLocked() == true) {
			return false;
		}
	}
	KeyEventArgs args(focus, keyCode, keyData);
	RFWnd *wnd = focus;
	do {
		wnd->OnKeyUp(args);
		if (args.handled) {
			break;
		}
		wnd = wnd->m_super;
	} while (wnd != 0);
	return true;
}

// 0x419400: the control characters 0x00-0x1f and 0x7f are filtered out
// (the first occurrence of each), then the focus window gets the rest
bool RFManager::InjectChar(wchar_t *text, bool composing, RFPoint const& point) {
	if (IsInputsFroze()) {
		return false;
	}
	std::wstring str(text);
	for (wchar_t c = 0; c < 0x20; ++c) {
		std::wstring::size_type pos = str.find(c);
		if (pos != std::wstring::npos) {
			str.erase(pos, 1);
		}
	}
	std::wstring::size_type pos = str.find(L'\x7f');
	if (pos != std::wstring::npos) {
		str.erase(pos, 1);
	}
	RFWnd *focus = m_focusWnd;
	if (focus == 0) {
		return false;
	}
	RFWnd *overlapped = focus->GetOverlapped();
	if (overlapped != 0) {
		if (overlapped->GetVisualState() == VisualStateTypes::Closing
			|| overlapped->IsInputLocked() == true) {
			return false;
		}
	}
	CharEventArgs args(focus, str.c_str(), composing, point);
	focus->OnChar(args);
	return true;
}

// 0x4196d0
bool RFManager::InjectPreDraw() {
	RFRenderContext rc;
	rc.m_clipRect = m_root->ClientToScreen();
	m_root->PreDraw(rc);
	return true;
}

// 0x419780
bool RFManager::InjectDraw() {
	RFRenderContext rc;
	rc.m_clipRect = m_root->ClientToScreen();
	m_root->Draw(rc);
	return true;
}

// 0x419830
bool RFManager::InjectUpdate(float elapsed) {
	RFWnd *focus = m_focusWnd;
	if (focus != 0) {
		if (focus->m_hidePending == true || !focus->m_visible
			|| (focus->m_super != 0 && !focus->m_super->IsWindowVisible(false))) {
			FocusWindow(m_desktop);
		}
	}
	UpdateEventArgs args(elapsed);
	m_root->OnUpdate(args);
	return true;
}

// 0x4198d0
void RFManager::NotifyWindowGotFocus(RFWnd *wnd) {
	if (m_focusWnd != 0 && wnd != m_focusWnd) {
		m_focusWnd->OnKillFocus();
	}
	m_focusWnd = wnd;
}

// 0x419900
void RFManager::NotifyWindowLostFocus(RFWnd *wnd) {
	if (wnd == m_focusWnd) {
		m_focusWnd = 0;
	}
}

// 0x419920
void RFManager::NotifyWindowActivated(RFWnd *wnd) {
	if (wnd != m_activeWnd) {
		if (m_activeWnd != 0) {
			m_activeWnd->Deactivate();
		}
		m_activeWnd = wnd;
	}
}

// 0x419950
void RFManager::NotifyWindowDeactivated(RFWnd *wnd) {
	if (wnd == m_activeWnd) {
		m_activeWnd = 0;
	}
}

// 0x419970
void RFManager::NotifyWindowDestroyed(RFWnd *wnd) {
	if (wnd == m_activeWnd) {
		m_activeWnd = 0;
	}
	if (wnd == m_focusWnd) {
		if (wnd->GetOverlapped()->m_hitTestVisible) {
			RFWnd *focusable = FindActiveFocusableWnd(0);
			if (focusable != 0) {
				FocusWindow(focusable);
			} else {
				FocusWindow(m_desktop);
			}
		} else {
			FocusWindow(m_desktop);
		}
	}
	if (wnd == m_capturedWindow) {
		m_capturedWindow = 0;
	}
	if (wnd == m_wndContainingMouse) {
		m_wndContainingMouse = 0;
	}
	if (wnd == m_desktop) {
		m_desktop = 0;
	}
}

// 0x4199f0
std::wstring const& RFManager::GetCompatibleControlName(WndType type) {
	switch (type) {
	case 0: case 1:
		return RFWndEx::TypeName;
	case 2:
		return RFLabel::TypeName;
	case 3:
		return RFButton::TypeName;
	case 4:
		return RFLabelButton::TypeName;
	case 5:
		return RFAnimButton::TypeName;
	case 6:
		return RFCheck::TypeName;
	case 12:
	case 13:
		return RFListSC::TypeName;
	case 15:
	case 16:
		return RFTextAreaSC::TypeName;
	case 11:
		return RFEdit::TypeName;
	case 17:
		return RFComboBox::TypeName;
	case 9:
		return RFScrollBar::TypeName;
	case 10:
		return RFSlider::TypeName;
	case 14:
		return RFStackPanel::TypeName;
	case 7:
		return RFPicture::TypeName;
	case 8:
		return RFProgress::TypeName;
	}
	assert(!"Unknown window type.");
	static std::wstring const unknown(L"<unknown>");
	return unknown;
}

// 0x419c30
bool RFManager::IsUsingCustomInputMethod() {
	return m_usingCustomInputMethod;
}

// 0x419c40: feeds the input method's pending results to InjectChar, asking
// each result's length first
void RFManager::UpdateInputMethod() {
	if (m_inputMethod && m_inputMethod->m_service) {
		int count = m_inputMethod->m_service->GetResultCount();
		if (count != 0) {
			wchar_t buffer[256] = {0};
			bool composing = false;
			for (int i = 0; i < count; ++i) {
				RFPoint pos;
				int length;
				if (m_inputMethod->m_service) {
					length = m_inputMethod->m_service->PopResult(0, 0, &composing);
				} else {
					length = -1;
				}
				if (m_inputMethod->m_service) {
					m_inputMethod->m_service->PopResult(buffer, length, &composing);
				}
				InjectChar(buffer, composing, pos);
			}
		}
	}
}

// RFManager.cpp's converters of an XML attribute, called by the properties'
// Parse (RFProperty.cpp). LTCG passes the string in eax.

// The blank lines between the converters keep each assert on the original's
// line (its __LINE__ is pushed to _wassert): 1180, 1195, 1267, 1295, 1318, 1335.







// 0x419d50
bool ParseBoolean(std::string const& string) {
	if (_stricmp(string.c_str(), "True") == 0) {
		return true;
	}
	if (_stricmp(string.c_str(), "False") != 0) {
		assert(!"Invalid Boolean string.");
	}
	return false;
}




// 0x419db0: "#RRGGBB" or "#RRGGBBAA" (hex, alpha 0xff when absent) to
// 0xAARRGGBB, or a decimal number
uint32_t ParseColor(std::string const& string) {
	if (string.size() == 0) {
		return 0;
	}
	if (string[0] == '#') {
		assert(string.size() == 7 || string.size() == 9);
		int32_t components[4] = { 0xff, 0xff, 0xff, 0xff };
		char const *digits = string.c_str() + 1;
		uint32_t count = (string.size() - 1) / 2;
		for (uint32_t i = 0; i < count; ++i, digits += 2) {
			sscanf_s(digits, "%02x", &components[i]);
		}
		return ((components[3] & 0xff) << 24) | ((components[0] & 0xff) << 16)
		       | ((components[1] & 0xff) << 8) | (components[2] & 0xff);
	}
	return atoi(string.c_str());
}







// 0x419ec0
StartPositionTypes::Type ParseStartPosition(std::string const& string) {
	if (_stricmp(string.c_str(), "CenterParent") == 0) {
		return static_cast<StartPositionTypes::Type>(1);
	}
	if (_stricmp(string.c_str(), "CenterScreen") == 0) {
		return static_cast<StartPositionTypes::Type>(2);
	}
	return static_cast<StartPositionTypes::Type>(0);
}







// 0x419f10
EscKeyActionTypes::Type ParseEscKeyAction(std::string const& string) {
	if (_stricmp(string.c_str(), "Close") == 0) {
		return static_cast<EscKeyActionTypes::Type>(1);
	}
	return static_cast<EscKeyActionTypes::Type>(0);
}






// 0x419f30: ", "-separated "Left" 1, "Top" 2, "Right" 4, "Bottom" 8 or
// "None" (clears them)
uint32_t ParseAnchor(std::string const& string) {
	std::string::size_type start = 0;
	uint32_t anchor = 0;
	std::string token;
	char const delimiter[] = ", ";
	do {
		std::string::size_type pos = string.find(delimiter, start);
		std::string::size_type length = (pos != std::string::npos ? pos : string.size()) - start;
		token = string.substr(start, length);
		start += length + (pos != std::string::npos ? 2 : 0);
		if (_stricmp(token.c_str(), "Left") == 0) {
			anchor |= 1;
		} else if (_stricmp(token.c_str(), "Top") == 0) {
			anchor |= 2;
		} else if (_stricmp(token.c_str(), "Right") == 0) {
			anchor |= 4;
		} else if (_stricmp(token.c_str(), "Bottom") == 0) {
			anchor |= 8;
		} else if (_stricmp(token.c_str(), "None") == 0) {
			anchor = 0;
		} else {
			assert(!"Invalid Anchor string.");
		}
	} while (start != string.size());
	return anchor;
}









// 0x41a170
HorzAlignTypes::Type ParseHorzAlign(std::string const& string) {
	if (isdigit(string.at(0))) {
		return static_cast<HorzAlignTypes::Type>(atoi(string.c_str()));
	}
	if (_stricmp(string.c_str(), "Left") == 0) {
		return HorzAlignTypes::Left;
	}
	if (_stricmp(string.c_str(), "Center") == 0) {
		return HorzAlignTypes::Center;
	}
	if (_stricmp(string.c_str(), "Right") == 0) {
		return HorzAlignTypes::Right;
	}
	assert(!"Invalid HorzAlignTypes string.");
	return HorzAlignTypes::Left;
}






// 0x41a240
VertAlignTypes::Type ParseVertAlign(std::string const& string) {
	if (isdigit(string.at(0))) {
		return static_cast<VertAlignTypes::Type>(atoi(string.c_str()));
	}
	if (_stricmp(string.c_str(), "Top") == 0) {
		return VertAlignTypes::Top;
	}
	if (_stricmp(string.c_str(), "Center") == 0) {
		return VertAlignTypes::Center;
	}
	if (_stricmp(string.c_str(), "Bottom") == 0) {
		return VertAlignTypes::Bottom;
	}
	assert(!"Invalid VertAlignTypes string.");
	return VertAlignTypes::Top;
}






// 0x41a310
TextTrimmingTypes::Type ParseTextTrimming(std::string const& string) {
	if (_stricmp(string.c_str(), "None") == 0) {
		return TextTrimmingTypes::None;
	}
	if (_stricmp(string.c_str(), "CharacterEllipsis") == 0) {
		return TextTrimmingTypes::CharacterEllipsis;
	}
	assert(!"Invalid TextTrimmingTypes string.");
	return TextTrimmingTypes::None;
}

// 0x41a370 (no out-of-line caller: RFOrientationProperty::Parse 0x4651e0
// inlines it under LTCG)
OrientationTypes::Type ParseOrientation(std::string const& string) {
	if (_stricmp(string.c_str(), "Vertical") == 0) {
		return OrientationTypes::Vertical;
	}
	if (_stricmp(string.c_str(), "Horizontal") == 0) {
		return OrientationTypes::Horizontal;
	}
	return OrientationTypes::Vertical;
}

