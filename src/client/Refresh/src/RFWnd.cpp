// Original: client\Refresh\src\RFWnd.cpp (JP 983 exe, unit bounds in
// jp/units.tsv; the file name is assumed from the class)
// lib: 0x401d40, 0x401d50, 0x401db0, 0x4028b0, 0x4028e0, 0x408260
// The implicit destructors of WindowCloseEventArgs 0x401d40, EventArgs
// 0x401d50, VisibleChangedArgs 0x401db0 and AnimationEventArgs 0x408260 (this
// in eax/esi), RFRenderContext's implicit copy constructor 0x4028b0 and
// operator= 0x4028e0 (all paired).
// lib: 0x4027c0 (the class's scalar deleting destructor ??_G; our cl emits
// only the ??_E of the dllexport class)
// lib: 0x40fc00, 0x40fc20, 0x40fc30, 0x40fc40, 0x40fc50
// MemberFunctionFunctor<RFWnd>'s operator(), AddRef, Release, GetRefCount and
// scalar deleting destructor (template members, paired).
// lib: 0x402850, 0x405180, 0x408be0, 0x408d90, 0x408f60, 0x409210
// std:: instantiations: map<wstring, wstring>'s node-tree teardown (erase
// all, delete the head) 0x402850, mem_fun_t<void, RFWnd>::operator() with
// UpdateLayout inlined 0x405180, vector<RFWnd*>::operator= 0x408be0,
// vector<RFWnd*>::push_back 0x408d90 (calls _Reserve 0x40a7d0),
// vector<RFWindowModifier*>::operator= 0x408f60, map<wstring,
// wstring>::operator[] 0x409210.
#include "RFWnd.h"

#include <assert.h>

#include <algorithm>
#include <functional>

#include "RFAnimation.h"
#include "RFDirector.h"
#include "RFEmptyString.h"
#include "RFEventArgs.h"
#include "RFInterface.h"
#include "RFManager.h"
#include "RFVisualEffect.h"
#include "RFWindowModifier.h"

// The class name, the static const at 0xe4691c (dynamic initializer
// 0xcc7250, outside the unit's code range like the ones below).
std::wstring const RFWnd::TypeName = L"WindowBase";

// The event and animation names; their dynamic initializers are at
// 0xcc7270..0xcc7570, one per name (assign from the literal, atexit).
std::wstring const RFWnd::EventClose = L"Close";
std::wstring const RFWnd::EventFinalize = L"Finalize";
std::wstring const RFWnd::EventLButtonUp = L"LButtonUp";
std::wstring const RFWnd::EventLButtonDown = L"LButtonDown";
std::wstring const RFWnd::EventLButtonDblClick = L"LButtonDblClick";
std::wstring const RFWnd::EventRButtonUp = L"RButtonUp";
std::wstring const RFWnd::EventRButtonDown = L"RButtonDown";
std::wstring const RFWnd::EventMouseMove = L"MouseMove";
std::wstring const RFWnd::EventMouseWheel = L"MouseWheel";
std::wstring const RFWnd::EventMouseEnter = L"MouseEnter";
std::wstring const RFWnd::EventMouseLeave = L"MouseLeave";
std::wstring const RFWnd::EventContextMenu = L"ContextMenu";
std::wstring const RFWnd::EventKeyUp = L"KeyUp";
std::wstring const RFWnd::EventKeyDown = L"KeyDown";
std::wstring const RFWnd::EventChar = L"EventChar";
std::wstring const RFWnd::EventActivated = L"Activated";
std::wstring const RFWnd::EventDeactivated = L"Deactivated";
std::wstring const RFWnd::EventSetFocus = L"SetFocus";
std::wstring const RFWnd::EventKillFocus = L"KillFocus";
std::wstring const RFWnd::EventUpdate = L"Update";
std::wstring const RFWnd::EventDraw = L"Draw";
std::wstring const RFWnd::EventPlayUIAnimation = L"PlayUIAnimation";
std::wstring const RFWnd::EventVisibleChanged = L"VisibleChanged";
std::wstring const RFWnd::AnimOpening = L"_Opening_";
std::wstring const RFWnd::AnimClosing = L"_Closing_";

// The one-line accessors 0x401dc0..0x4020c0 are RFWnd.h inlines.

// 0x4020e0
void RFWnd::LoadResource() {
}

// 0x4020f0
void RFWnd::OnInitialUpdate() {
}

// 0x402100
void RFWnd::OnDestroy() {
}

// 0x402110
void RFWnd::OnEnabled(bool) {
}

// 0x402940
RFWnd *RFWnd::FindWnd(float x, float y) {
	return ChildWindowFromPoint(x, y, (RFCWPFlags)1);
}

// 0x402960
RFWnd *RFWnd::GetSuper(bool top) {
	if (top) {
		RFWnd const *wnd = this;
		while (wnd != 0) {
			wnd = wnd->GetSuper(false);
		}
		return const_cast<RFWnd*>(wnd);
	}
	return m_super;
}

// 0x4029e0
void RFWnd::SetHitTestVisible(bool visible) {
	m_hitTestVisible = visible;
}

// 0x4029f0
bool RFWnd::IsHitTestVisible() const {
	return m_hitTestVisible;
}

// 0x402a00
void RFWnd::SetHitTestThrough(bool through) {
	m_hitTestThrough = through;
}

// 0x402a10
bool RFWnd::IsHitTestThrough() const {
	return m_hitTestThrough;
}

// 0x402a20
uint32_t RFWnd::GetChildCount() const {
	return _children.size();
}

// 0x402a30
uint32_t RFWnd::GetAnimationMask() const {
	return m_animationMask;
}

// 0x402a40
void RFWnd::RemoveAnimationMask(uint32_t mask) {
	if (m_animationMask & mask) {
		m_animationMask ^= mask;
	}
}

// 0x402a60
void RFWnd::AddAnimationMask(uint32_t mask) {
	if (!(m_animationMask & mask)) {
		m_animationMask |= mask;
	}
}

// 0x404830
std::wstring const& RFWnd::GetTypeName() {
	return TypeName;
}

// 0x404840
RFWnd::RFWnd()
	: m_createStruct(0),
	  m_super(0),
	  m_active(false),
	  m_overlapped(true),
	  m_hitTestVisible(true),
	  m_hitTestThrough(false),
	  m_modal(false),
	  m_subControl(false),
	  m_animationMask(0xffffffff) {
	m_dragable = false;
	m_preDraw = false;
}

// lib: 0x4027f0 (the vector deleting destructor ??_E, compiler-generated,
// paired with map_symbol.ts)

// 0x404a40
RFWnd::~RFWnd() {
}

// 0x404c20
void RFWnd::Initialize() {
	PreInitialize();
	LoadResource();
	OnInitialUpdate();
	RegisterTo(L"", EventKeyDown, Subscriber(&RFWnd::KeyDownEventHandler, this));
}

// 0x404d30
void RFWnd::Create(WndBaseStruct* createStruct, RFWnd* parent) {
	m_type = createStruct->GetType();
	m_name = createStruct->m_name;
	m_localPos = createStruct->m_pos;
	m_initialPos = createStruct->m_pos;
	RFSize size(createStruct->m_rect.Width(), createStruct->m_rect.Height());
	m_localSize = size;
	m_initialSize = size;
	m_showSound = createStruct->m_showSound;
	m_hideSound = createStruct->m_hideSound;
	m_dragable = createStruct->m_dragable.GetValue();
	m_hitTestVisible = createStruct->m_hitTestVisible.GetValue();
	m_hitTestThrough = createStruct->m_hitTestThrough.GetValue();
	m_startPosition = createStruct->m_startPosition.GetValue();
	// The struct's EscKeyAction is not copied: the window starts with 0.
	m_escKeyAction = static_cast<EscKeyActionTypes::Type>(0);
	m_anchor = createStruct->m_anchor.GetValue();
	m_clipChildren = createStruct->m_clipChildren.GetValue();
	m_rc.m_clip = createStruct->m_clipChildren.GetValue();
	m_createStruct = createStruct;
	if (parent != 0) {
		parent->RegisterChild(this);
	}
}

// 0x404e50
void RFWnd::Destroy() {
	RFManager::GetInstance().NotifyWindowDestroyed(this);
	WindowEventArgs_<RFWnd> args(this);
	EventMap::iterator it = m_events.find(EventFinalize);
	if (it != m_events.end()) {
		it->second(args);
	}
	OnDestroy();
	DeleteAllChildren();
	if (m_animation != 0) {
		delete m_animation;
		m_animation = 0;
	}
	if (m_createStruct != 0) {
		delete m_createStruct;
		m_createStruct = 0;
	}
	if (m_visualEffect != 0) {
		delete m_visualEffect;
		m_visualEffect = 0;
	}
}

// 0x404f40
void RFWnd::DeleteChild(RFWnd* child) {
	std::vector<RFWnd*>::iterator it = std::find(_children.begin(), _children.end(), child);
	if (it != _children.end()) {
		RFWnd* wnd = *it;
		_children.erase(it);
		wnd->Destroy();
		delete wnd;
	}
}

// 0x404fc0
void RFWnd::DeleteAllChildren() {
	while (!_children.empty()) {
		RFWnd* child = _children.back();
		_children.pop_back();
		child->Destroy();
		delete child;
	}
}

// 0x405020
void RFWnd::EjectChild_(RFWnd* child) {
	std::vector<RFWnd*>::iterator it = std::find(_children.begin(), _children.end(), child);
	if (it != _children.end()) {
		_children.erase(it);
	}
}

// The child order ValidatePriority keeps: higher priority first, and on equal
// priority the non-modal windows before the modal ones. Inlined into the
// stable_sort helpers (insertion sort 0x412fb0, merges 0x412460/0x412510);
// the out-of-line copy (paired) takes its arguments in esi/edx (LTCG).
struct PriorityGreater {
	// 0x402120
	bool operator()(RFWnd* lhs, RFWnd* rhs) const {
		if (lhs->GetPriority() == rhs->GetPriority()) {
			return lhs->IsModal() < rhs->IsModal();
		}
		return lhs->GetPriority() > rhs->GetPriority();
	}
};

// 0x405080
void RFWnd::ValidatePriority() {
	if (m_priorityDirty) {
		std::stable_sort(_children.begin(), _children.end(), PriorityGreater());
		m_priorityDirty = false;
	}
}

// 0x405140
void RFWnd::UpdateLayout() {
	OnLayoutChanged(0x10000000);
	std::for_each(_children.begin(), _children.end(), std::mem_fun(&RFWnd::UpdateLayout));
}

// 0x4051d0
RFRenderer *RFWnd::GetRenderer() {
	return RFManager::s_renderer;
}

// 0x4051e0
RFInterface *RFWnd::GetInterface() {
	return RFManager::s_interface;
}


// 0x4051f0
void RFWnd::ApplyAnchor() {
	for (uint32_t i = 0; i < _children.size(); ++i) {
		RFWnd* child = _children[i];
		uint32_t anchor = child->m_anchor;
		if (anchor == 0) {
			continue;
		}
		RFVector2 pos = child->m_localPos;
		float width = child->m_localSize.width;
		float height = child->m_localSize.height;
		if (anchor & 1) {
			pos.x = child->m_anchorValue.left;
		}
		if (anchor & 4) {
			if (anchor & 1) {
				RFSize parentSize(GetSize().width, GetSize().height);
				width = parentSize.width - child->m_anchorValue.right - pos.x;
			} else {
				RFSize parentSize(GetSize().width, GetSize().height);
				pos.x = parentSize.width - child->m_anchorValue.right - child->m_localSize.width;
			}
		}
		if (anchor & 2) {
			pos.y = child->m_anchorValue.top;
		}
		if (anchor & 8) {
			if (anchor & 2) {
				RFSize parentSize(GetSize().width, GetSize().height);
				height = parentSize.height - child->m_anchorValue.bottom - pos.y;
			} else {
				RFSize parentSize(GetSize().width, GetSize().height);
				pos.y = parentSize.height - child->m_anchorValue.bottom - child->m_localSize.height;
			}
		}
		child->SetPos(pos.x, pos.y);
		RFRect rect(0.0f, 0.0f, width, height);
		RFSize size(rect.Width(), rect.Height());
		if (size != child->m_localSize) {
			child->m_localSize = size;
			child->OnLayoutChanged(2);
		}
	}
}
// 0x4053f0
void RFWnd::UpdateAnchorValue() {
	RFWnd* super = m_super;
	if (super != 0) {
		RFRect superRect = super->GetScreenRect();
		RFRect rect = GetScreenRect();
		if (super == RFManager::s_root) {
			superRect.right = rect.right;
			superRect.bottom = rect.bottom;
		}
		m_anchorValue.left = rect.left - superRect.left;
		m_anchorValue.right = superRect.right - rect.right;
		m_anchorValue.top = rect.top - superRect.top;
		m_anchorValue.bottom = superRect.bottom - rect.bottom;
	}
}

// 0x405470
RFRect RFWnd::ClientToScreen() const {
	RFRect rect(0.0f, 0.0f, GetSize().width, GetSize().height);
	float top = 0.0f;
	for (RFWnd const* wnd = this; wnd != 0; wnd = wnd->m_super) {
		top += wnd->GetPos().y;
	}
	float left = 0.0f;
	for (RFWnd const* wnd = this; wnd != 0; wnd = wnd->m_super) {
		left += wnd->GetPos().x;
	}
	return RFRect(left + rect.left, top + rect.top, left + rect.right, top + rect.bottom);
}

// 0x405540
RFVector2 RFWnd::ScreenToClient(RFVector2 const& pt) const {
	RFVector2 offset(0.0f, 0.0f);
	for (RFWnd const* wnd = this; wnd != 0; wnd = wnd->m_super) {
		offset += wnd->GetPos();
	}
	return pt - offset;
}

// 0x4055b0
RFVector2 RFWnd::ClientToScreen(RFVector2 const& pt) const {
	RFVector2 offset(0.0f, 0.0f);
	for (RFWnd const* wnd = this; wnd != 0; wnd = wnd->m_super) {
		offset += wnd->GetPos();
	}
	return pt + offset;
}

// 0x405620
void RFWnd::PlaySfx(std::wstring const& sfx) {
	if (!sfx.empty()) {
		std::wstring name(sfx.c_str());
		RFManager::GetInstance().GetInterface()->PlaySfx(name);
	}
}

// 0x405700
void RFWnd::PrintText(FontInfo const& font, int32_t x, int32_t y, std::wstring const& text) {
	float depth = m_rc.m_depth;
	RFManager::GetInstance().GetInterface()->PrintText(font, x, y, text, depth);
}

// 0x405760
void RFWnd::KeyDownEventHandler(EventArgs& args) {
	KeyEventArgs& keyArgs = static_cast<KeyEventArgs&>(args);
	// 0x1b: VK_ESCAPE; action 1 closes the window with close state 2
	if (keyArgs.keyCode == 0x1b && m_escKeyAction == static_cast<EscKeyActionTypes::Type>(1)) {
		Close(static_cast<RFWNDCLOSESTATE>(2));
		args.handled = true;
	}
}

// 0x405790
RFWnd const *RFWnd::GetSuper(bool top) const {
	if (top) {
		RFWnd const *wnd = this;
		while (wnd != 0) {
			wnd = wnd->GetSuper(false);
		}
		return wnd;
	}
	return m_super;
}

// 0x4057c0
RFWnd *RFWnd::GetControlSuper() {
	if (m_subControl == true) {
		if (m_super == 0) {
			return this;
		}
		if (m_super->m_subControl == false) {
			return m_super;
		}
		return m_super->GetControlSuper();
	}
	return 0;
}

// 0x4057f0
RFWnd *RFWnd::GetOverlapped() {
	for (RFWnd *wnd = this; wnd != 0; wnd = wnd->m_super) {
		if (wnd->m_overlapped) {
			return wnd;
		}
	}
	return 0;
}

// 0x405810
RFWnd *RFWnd::GetActiveChild() {
	if (!IsActive()) {
		return 0;
	}
	IteratorBase<std::vector<RFWnd*>::iterator> list = GetWindowList();
	for (; list.m_first != list.m_last; ++list.m_first) {
		if ((*list.m_first)->IsActive()) {
			return (*list.m_first)->GetActiveChild();
		}
	}
	return this;
}

// 0x405880
bool RFWnd::IsActive() const {
	if (!m_active) {
		return false;
	}
	if (m_super != 0 && !m_super->IsActive()) {
		return false;
	}
	return true;
}

// 0x4058a0
RFWnd *RFWnd::GetChild(std::wstring const& name) {
	for (uint32_t i = 0; i < _children.size(); ++i) {
		RFWnd* child = _children[i];
		if (child->m_name == name) {
			return child;
		}
	}
	for (uint32_t i = 0; i < _children.size(); ++i) {
		RFWnd* child = _children[i];
		RFWnd* found = child->GetChild(name);
		if (found != 0) {
			return found;
		}
	}
	return 0;
}

// 0x405960
void RFWnd::RegisterChild(RFWnd* child) {
	if (child == 0) {
		return;
	}
	if (std::find(_children.begin(), _children.end(), child) == _children.end()) {
		if (RFManager::s_root != this) {
			child->m_overlapped = false;
		}
		child->m_super = this;
		_children.push_back(child);
		InvalidatePriority();
	}
}

// 0x4059c0
void RFWnd::CenterWindow(RFWnd* parent) {
	if (parent == 0) {
		parent = m_super;
	}
	RFSize parentSize;
	if (parent == 0) {
		parentSize = RFManager::GetInstance().m_root->GetSize();
	} else {
		parentSize.width = parent->GetRect().Width();
		parentSize.height = parent->GetRect().Height();
	}
	SetPos((parentSize.width - GetRect().Width()) * 0.5, (parentSize.height - GetRect().Height()) * 0.5);
}

// 0x405b20
void RFWnd::CenterWindow_Horizontal(RFWnd* parent) {
	if (parent == 0) {
		parent = m_super;
	}
	RFSize parentSize;
	if (parent == 0) {
		parentSize = RFManager::GetInstance().m_root->GetSize();
	} else {
		parentSize.width = parent->GetRect().Width();
	}
	float width = GetRect().Width();
	SetPos((parentSize.width - width) * 0.5, GetPos().y);
}

// 0x405c20
void RFWnd::CenterWindow_Vertical(RFWnd* parent) {
	if (parent == 0) {
		parent = m_super;
	}
	RFSize parentSize;
	if (parent == 0) {
		parentSize = RFManager::GetInstance().m_root->GetSize();
	} else {
		parentSize.height = parent->GetRect().Height();
	}
	float height = GetRect().Height();
	SetPos(GetPos().x, (parentSize.height - height) * 0.5);
}

// 0x405d20 (the split cuts it at 75 bytes: the rest of its body is the
// inventory's sub_405d6b). Walks a backslash-separated path: a leading
// backslash starts at the desktop, "." stays, ".." goes to the parent, any
// other segment is a child name.
RFWnd *RFWnd::GetWindow_(std::wstring const& path) {
	RFWnd *wnd = this;
	std::wstring::size_type start = 0;
	std::wstring name;
	if (path.empty()) {
		return 0;
	}
	if (path.at(0) == L'\\') {
		start = 1;
		wnd = RFManager::GetInstance().GetDesktop();
		if (wnd == 0) {
			return 0;
		}
	}
	for (;;) {
		std::wstring::size_type pos = path.find(L'\\', start);
		if (pos != std::wstring::npos) {
			name = path.substr(start, pos - start);
			start = pos + 1;
			if (name == L".") {
				continue;
			}
			if (name == L"..") {
				wnd = wnd->m_super;
			} else {
				wnd = wnd->GetChild(name);
			}
			if (wnd == 0) {
				return 0;
			}
		}
		if (start == path.size()) {
			break;
		}
		if (pos == std::wstring::npos) {
			name = path.substr(start, path.size() - start);
			wnd = wnd->GetChild(name);
			break;
		}
	}
	return wnd;
}

// 0x405ff0
bool RFWnd::IsWindowEnabled(bool self) const {
	if (self == true) {
		return m_enabled;
	}
	if (!m_enabled) {
		return false;
	}
	if (m_super != 0 && !m_super->IsWindowEnabled(false)) {
		return false;
	}
	return true;
}

// 0x406030
void RFWnd::Activate() {
	if (m_overlapped && !m_active) {
		m_active = true;
		RFManager& mgr = RFManager::GetInstance();
		if (this != mgr.m_activeWnd) {
			if (mgr.m_activeWnd != 0) {
				mgr.m_activeWnd->Deactivate();
			}
			mgr.m_activeWnd = this;
		}
		OnActivated();
	}
}

// 0x406090
void RFWnd::Deactivate() {
	if (m_overlapped && m_active) {
		m_active = false;
		RFManager& mgr = RFManager::GetInstance();
		if (this == mgr.m_activeWnd) {
			mgr.m_activeWnd = 0;
		}
		OnDeactivated();
	}
}

// 0x4060e0
void RFWnd::SetFocus() {
	OnSetFocus();
}

// 0x4060f0
void RFWnd::KillFocus() {
	OnKillFocus();
}

// 0x406100
void RFWnd::SetCapture() {
	RFManager::GetInstance().SetCapturedWindow(this);
}

// 0x406130
void RFWnd::ReleaseCapture() {
	RFManager::GetInstance().SetCapturedWindow(0);
}

// 0x406160
void RFWnd::AddModifier(RFWindowModifier* modifier) {
	if (std::find(m_modifiers.begin(), m_modifiers.end(), modifier) == m_modifiers.end()) {
		m_modifiers.push_back(modifier);
	}
}

// 0x4061f0
void RFWnd::RemoveModifier(RFWindowModifier* modifier) {
	std::vector<RFWindowModifier*>::iterator it = std::find(m_modifiers.begin(), m_modifiers.end(), modifier);
	if (it != m_modifiers.end()) {
		m_modifiers.erase(it);
	}
}

// 0x406240
RFWindowModifier *RFWnd::GetModifier(std::wstring const& name) const {
	for (std::vector<RFWindowModifier*>::const_iterator it = m_modifiers.begin(); it != m_modifiers.end(); ++it) {
		RFWindowModifier* modifier = *it;
		if (modifier->GetName() == name) {
			return modifier;
		}
	}
	return 0;
}


// 0x4062b0
void RFWnd::LoadUIAnimation(std::wstring const& file) {
	if (file.empty()) {
		return;
	}
	m_animation = new RFDirector;
	m_animation->LoadAnimation(file);
	m_animation->SubscribeEvent(RFDirector::EventAnimFinished, Subscriber(&RFWnd::OnAnimFinished, this));
	if (m_animation->GetAnimation(AnimOpening) != 0 && m_animation->GetAnimation(AnimClosing) != 0) {
		if (m_visualEffect != 0) {
			delete m_visualEffect;
			m_visualEffect = 0;
		}
		m_visualEffect = CreateVisualEffect(2);
	}
}
// 0x406430
void RFWnd::RegisterTo(std::wstring const& window, std::wstring const& name, Subscriber const& subscriber) {
	RFWnd* wnd = this;
	if (!window.empty() && window != m_name) {
		wnd = GetWindow_(window);
	}
	if (wnd == 0) {
		return;
	}
	wnd->SubscribeEvent(name, subscriber);
}

// 0x4064c0
void RFWnd::SetPos(float x, float y) {
	x = (float)(int)x;
	y = (float)(int)y;
	RFVector2 pos(x, y);
	if (m_localPos.x != x || m_localPos.y != y) {
		m_anchorValue.left = x;
		m_anchorValue.top = y;
		m_localPos = pos;
		OnLayoutChanged(1);
	}
}

// 0x406550
RFVector2 RFWnd::GetPos() const {
	RFVector2 pos = m_localPos;
	for (std::vector<RFWindowModifier*>::const_iterator it = m_modifiers.begin(); it != m_modifiers.end(); ++it) {
		RFVector2 offset = (*it)->GetPos();
		pos.x += offset.x;
		pos.y += offset.y;
	}
	return pos;
}

// 0x4065c0
RFVector2 const& RFWnd::GetLocalPos() const {
	return m_localPos;
}

// 0x4065d0
void RFWnd::SetScreenPos(float x, float y) {
	if (m_super != 0) {
		RFVector2 pos = m_super->GetScreenPos();
		SetPos(x - pos.x, y - pos.y);
	} else {
		SetPos(x, y);
	}
}

// 0x406670
RFVector2 RFWnd::GetScreenPos() const {
	RFVector2 pos(0.0f, 0.0f);
	for (RFWnd const* wnd = this; wnd != 0; wnd = wnd->m_super) {
		RFVector2 local = wnd->GetPos();
		pos.x += local.x;
		pos.y += local.y;
	}
	return pos;
}

// 0x4066c0
RFVector2 RFWnd::GetScreenPos_Impl() const {
	RFVector2 pos(0.0f, 0.0f);
	for (RFWnd const* wnd = this; wnd != 0; wnd = wnd->m_super) {
		pos.x += wnd->m_localPos.x;
		pos.y += wnd->m_localPos.y;
	}
	return pos;
}

// 0x406700
void RFWnd::SetSize(RFSize const& size) {
	if (m_localSize.width != size.width || m_localSize.height != size.height) {
		m_localSize = size;
		OnLayoutChanged(2);
	}
}

// 0x406750
RFSize RFWnd::GetSize() const {
	RFSize size = m_localSize;
	for (std::vector<RFWindowModifier*>::const_iterator it = m_modifiers.begin(); it != m_modifiers.end(); ++it) {
		RFSize offset = (*it)->GetSize();
		size.width += offset.width;
		size.height += offset.height;
	}
	return size;
}

// 0x4067b0
void RFWnd::SetRect(RFRect const& rect) {
	SetSize(RFSize(rect.Width(), rect.Height()));
}

// 0x406810
RFRect RFWnd::GetRect() const {
	float width = GetSize().width;
	float height = GetSize().height;
	return RFRect(0.0f, 0.0f, width, height);
}

// 0x406860
void RFWnd::SetScreenRect(RFRect const& rect) {
	m_localSize = RFSize(rect.Width(), rect.Height());
	SetScreenPos(rect.left, rect.top);
	OnLayoutChanged(3);
}

// 0x4068c0
RFRect RFWnd::GetScreenRect() const {
	RFVector2 pos = GetScreenPos();
	float width = GetSize().width;
	float height = GetSize().height;
	return RFRect(pos.x, pos.y, pos.x + width, pos.y + height);
}


// 0x406960
void RFWnd::Draw(RFRenderContext const& rc) {
	if (this != RFManager::s_root && this != RFManager::s_desktop && m_preDraw) {
		return;
	}
	bool clip = rc.m_clip;
	RFRenderContext *myRc = GetRC();
	if (clip) {
		myRc->m_clip = true;
	}
	myRc->m_clipRect = rc.m_clipRect;
	float depth = rc.m_depth;
	myRc->m_depth = depth + 1.0;
	if (!m_rc.m_clip) {
		m_rc.m_clipRect = ClientToScreen();
	}
	if ((m_hidePending != true && m_visible && (m_super == 0 || m_super->IsWindowVisible(false)))
		|| m_visualState != VisualStateTypes::Closed) {
		WindowEventArgs_<RFWnd> args(this);
		EventMap::iterator it = m_events.find(EventDraw);
		if (it != m_events.end()) {
			it->second(args);
		}
		if (!args.handled) {
			OnDraw();
		}
		ValidatePriority();
		for (uint32_t i = 0; i < _children.size(); ++i) {
			_children[i]->Draw(m_rc);
		}
	}
}


// 0x406af0
void RFWnd::Draw2() {
	if ((m_hidePending != true && m_visible && (m_super == 0 || m_super->IsWindowVisible(false)))
		|| m_visualState != VisualStateTypes::Closed) {
		OnDraw();
		ValidatePriority();
		RFRenderContext rc;
		rc.m_clipRect = ClientToScreen();
		for (uint32_t i = 0; i < _children.size(); ++i) {
			_children[i]->Draw(rc);
		}
	}
}
// 0x406c00
void RFWnd::PreDraw(RFRenderContext const& rc) {
	if (this != RFManager::s_root && this != RFManager::s_desktop && !m_preDraw) {
		return;
	}
	bool clip = rc.m_clip;
	RFRenderContext *myRc = GetRC();
	if (clip) {
		myRc->m_clip = true;
	}
	myRc->m_clipRect = rc.m_clipRect;
	float depth = rc.m_depth;
	myRc->m_depth = depth + 1.0;
	if (!m_rc.m_clip) {
		m_rc.m_clipRect = ClientToScreen();
	}
	if ((m_hidePending != true && m_visible && (m_super == 0 || m_super->IsWindowVisible(false)))
		|| m_visualState != VisualStateTypes::Closed) {
		WindowEventArgs_<RFWnd> args(this);
		EventMap::iterator it = m_events.find(EventDraw);
		if (it != m_events.end()) {
			it->second(args);
		}
		if (!args.handled) {
			OnDraw();
		}
		ValidatePriority();
		for (uint32_t i = 0; i < _children.size(); ++i) {
			_children[i]->PreDraw(m_rc);
		}
	}
}
// 0x406d90
bool RFWnd::IsWindowVisible(bool self) const {
	if (self) {
		return !m_hidePending && m_visible;
	}
	if (m_hidePending == true || !m_visible) {
		return false;
	}
	if (m_super != 0 && !m_super->IsWindowVisible(false)) {
		return false;
	}
	return true;
}

// 0x406de0
void RFWnd::ShowWindow(bool show) {
	if (!show && m_visualState == VisualStateTypes::Closing) {
		return;
	}
	if (m_visible == show && m_visualState != VisualStateTypes::Closing) {
		return;
	}
	if (show) {
		PlaySfx(m_showSound);
		if (m_visualEffect != 0) {
			m_visualState = VisualStateTypes::Opening;
		}
	} else {
		PlaySfx(m_hideSound);
		if (m_visualEffect != 0) {
			m_visualState = VisualStateTypes::Closing;
		}
	}
	if (m_visualEffect != 0) {
		m_visualEffect->Reset(this);
	} else if (!show) {
		m_visualState = VisualStateTypes::Closed;
	}
	if (show) {
		m_visible = true;
		m_hidePending = false;
		Activate();
	} else {
		if (m_overlapped) {
			RFWnd* focus = RFManager::GetInstance().m_focusWnd;
			if (focus != 0 && focus->GetOverlapped() == this) {
				RFWnd* next = RFManager::GetInstance().FindActiveFocusableWnd(this);
				if (next != 0) {
					RFManager::GetInstance().ActivateWindow(next);
					RFManager::GetInstance().FocusWindow(next);
				}
			}
		}
		if (m_visualState != VisualStateTypes::Closed) {
			m_hidePending = true;
		} else {
			m_visible = false;
			m_hidePending = false;
		}
		Deactivate();
	}
	OnShow(show);
	if (!show) {
		m_lButtonDown = false;
		m_modal = false;
		return;
	}
	switch (m_startPosition) {
	case StartPositionTypes::CenterParent:
		CenterWindow(0);
		break;
	case StartPositionTypes::CenterScreen: {
		RFSize screen = RFManager::GetInstance().m_root->GetSize();
		RFSize size(GetRect().Width(), GetRect().Height());
		SetPos((screen.width - size.width) * 0.5, (screen.height - size.height) * 0.5);
		break;
	}
	}
	for (uint32_t i = 0; i < _children.size(); ++i) {
		if (GetChild(i)->IsFocusable()) {
			break;
		}
	}
}

// 0x407070
void RFWnd::ShowModal() {
	m_modal = true;
	ShowWindow(true);
	RFManager::GetInstance().ActivateWindow(this);
	RFManager::GetInstance().FocusWindow(this);
}

// 0x407110
void RFWnd::BringChildWndToTop(RFWnd* child) {
	std::vector<RFWnd*>::iterator it = std::find(_children.begin(), _children.end(), child);
	if (it == _children.end()) {
		return;
	}
	_children.erase(it);
	_children.push_back(child);
	InvalidatePriority();
}

// 0x4071f0
void RFWnd::SetPreDraw(bool preDraw) {
	m_preDraw = preDraw;
	for (uint32_t i = 0; i < _children.size(); ++i) {
		_children[i]->SetPreDraw(preDraw);
	}
}

// 0x407240
void RFWnd::SetVisualEffect(RFVisualEffect* effect) {
	if (m_visualEffect != 0) {
		delete m_visualEffect;
		m_visualEffect = 0;
	}
	// A value below 0x10 is an index for CreateVisualEffect, not an effect.
	if (reinterpret_cast<uintptr_t>(effect) < 0x10) {
		m_visualEffect = CreateVisualEffect(static_cast<int32_t>(reinterpret_cast<uintptr_t>(effect)));
	} else {
		m_visualEffect = effect->Clone();
	}
	if (effect == 0) {
		m_visualState = VisualStateTypes::Opened;
	}
}

// 0x407290
bool RFWnd::HitTest(RFVector2 const& pt, bool ignoreEnabled) {
	if (!m_visible || !m_hitTestVisible || (!ignoreEnabled && !m_enabled)) {
		return false;
	}
	RFRect screen = ClientToScreen();
	RFRect rect;
	if (m_rc.m_clip) {
		rect.IntersectRect(screen, m_rc.m_clipRect);
	} else {
		rect = screen;
	}
	return rect.PtInRect(pt) ? true : false;
}

// 0x407340
RFWnd *RFWnd::HitTestChild(RFVector2 const& pt, bool recursive, bool ignoreEnabled) {
	if (!m_visible) {
		return 0;
	}
	if (m_clipChildren && !HitTest(pt, ignoreEnabled)) {
		return 0;
	}
	IteratorBase<std::vector<RFWnd*>::reverse_iterator> list = GetReverseWindowList();
	for (; list.m_first != list.m_last; ++list.m_first) {
		RFWnd* child = *list.m_first;
		if (!child->m_clipChildren || child->HitTest(pt, ignoreEnabled)) {
			if (!recursive) {
				return child;
			}
			RFWnd* hit = child->HitTestChild(pt, recursive, ignoreEnabled);
			if (hit != 0) {
				return hit;
			}
		}
	}
	if (m_clipChildren || HitTest(pt, ignoreEnabled)) {
		return m_hitTestThrough ? 0 : this;
	}
	return 0;
}

// 0x407420
RFWnd *RFWnd::ChildWindowFromPoint(float x, float y, RFCWPFlags flags) {
	if (!ClientToScreen().PtInRect(x, y)) {
		return 0;
	}
	if (!m_visible && (flags & 1)) {
		return 0;
	}
	if (!m_enabled && (flags & 2)) {
		return 0;
	}
	for (uint32_t i = 0; i < _children.size(); ++i) {
		RFWnd* found = GetChild(i)->ChildWindowFromPoint(x, y, flags);
		if (found != 0) {
			return found;
		}
	}
	return this;
}

// 0x407520
bool RFWnd::IsMouseOver() {
	return HitTest(RFManager::GetInstance().GetCursorPos(), false);
}

// 0x407560
RFManager *RFWnd::GetMgr() {
	return RFSingleton<RFManager>::GetInstancePtr();
}

// 0x407570
void RFWnd::SetAlpha(float alpha) {
	m_alpha = std::min(std::max(0.0f, alpha), 1.0f);
}

// 0x4075c0
float RFWnd::GetAlpha(bool local) const {
	float alpha = m_alpha;
	for (std::vector<RFWindowModifier*>::const_iterator it = m_modifiers.begin(); it != m_modifiers.end(); ++it) {
		alpha *= (*it)->GetAlpha();
	}
	if (local != true && m_inheritsAlpha && m_super != 0) {
		return m_super->GetAlpha(false) * alpha;
	}
	return alpha;
}

// 0x407630
void RFWnd::OnLayoutChanged(uint32_t flags) {
	if (flags & 2) {
		ApplyAnchor();
	}
}

// 0x407650
void RFWnd::OnShow(bool) {
	VisibleChangedArgs args(m_visible, this);
	EventMap::iterator it = m_events.find(EventVisibleChanged);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x4076d0
void RFWnd::OnLButtonDown(MouseEventArgs& args) {
	if (m_dragable) {
		SetCapture();
		m_dragOffset = ScreenToClient(args.pos);
	}
	m_lButtonDown = true;
	EventMap::iterator it = m_events.find(EventLButtonDown);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x407760
void RFWnd::OnLButtonUp(MouseEventArgs& args) {
	if (m_dragable) {
		ReleaseCapture();
	}
	if (m_lButtonDown) {
		EventMap::iterator it = m_events.find(EventLButtonUp);
		if (it != m_events.end()) {
			it->second(args);
		}
	}
	m_lButtonDown = false;
}

// 0x4077d0
void RFWnd::OnLButtonDblClick(MouseEventArgs& args) {
	EventMap::iterator it = m_events.find(EventLButtonDblClick);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x407810
void RFWnd::OnRButtonDown(MouseEventArgs& args) {
	EventMap::iterator it = m_events.find(EventRButtonDown);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x407850
void RFWnd::OnRButtonUp(MouseEventArgs& args) {
	EventMap::iterator it = m_events.find(EventRButtonUp);
	if (it != m_events.end()) {
		it->second(args);
	}
	if (HitTest(args.pos, false)) {
		EventMap::iterator menu = m_events.find(EventContextMenu);
		if (menu != m_events.end()) {
			menu->second(args);
		}
	}
}

// 0x4078c0
void RFWnd::OnMouseMove(MouseEventArgs& args) {
	if (m_dragable && m_lButtonDown) {
		RFVector2 pos(args.pos.x - m_dragOffset.x, args.pos.y - m_dragOffset.y);
		SetScreenPos(pos.x, pos.y);
	}
	EventMap::iterator it = m_events.find(EventMouseMove);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x407940
void RFWnd::OnMouseWheel(MouseEventArgs& args) {
	EventMap::iterator it = m_events.find(EventMouseWheel);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x407980
void RFWnd::OnMouseEnter() {
	WindowEventArgs_<RFWnd> args(this);
	EventMap::iterator it = m_events.find(EventMouseEnter);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x4079f0
void RFWnd::OnMouseLeave() {
	WindowEventArgs_<RFWnd> args(this);
	EventMap::iterator it = m_events.find(EventMouseLeave);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x407a60
void RFWnd::OnKeyDown(KeyEventArgs& args) {
	EventMap::iterator it = m_events.find(EventKeyDown);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x407aa0
void RFWnd::OnKeyUp(KeyEventArgs& args) {
	EventMap::iterator it = m_events.find(EventKeyUp);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x407ae0
void RFWnd::OnChar(CharEventArgs& args) {
	EventMap::iterator it = m_events.find(EventChar);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x407b20
void RFWnd::OnActivated() {
	EventArgs args;
	EventMap::iterator it = m_events.find(EventActivated);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x407b90
void RFWnd::OnDeactivated() {
	EventArgs args;
	EventMap::iterator it = m_events.find(EventDeactivated);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x407c00
void RFWnd::OnSetFocus() {
	GetOverlapped()->Activate();
	RFManager& mgr = RFManager::GetInstance();
	if (mgr.m_focusWnd != 0 && this != mgr.m_focusWnd) {
		mgr.m_focusWnd->OnKillFocus();
	}
	mgr.m_focusWnd = this;
	WindowEventArgs_<RFWnd> args(this);
	EventMap::iterator it = m_events.find(EventSetFocus);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x407d30
void RFWnd::OnKillFocus() {
	RFManager& mgr = RFManager::GetInstance();
	if (this == mgr.m_focusWnd) {
		mgr.m_focusWnd = 0;
	}
	WindowEventArgs_<RFWnd> args(this);
	EventMap::iterator it = m_events.find(EventKillFocus);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// The blank lines below keep OnUpdate's assert on the original's line 1520
// (its __LINE__ is pushed to _wassert).




























































































































































































// 0x407de0
void RFWnd::OnUpdate(UpdateEventArgs& args) {
	for (uint32_t i = 0; i < _children.size(); ++i) {
		RFWnd* child = _children[i];
		if (child->m_closeState != static_cast<RFWNDCLOSESTATE>(0) && child->m_visualState == VisualStateTypes::Closed) {
			m_closedChildren.push_back(child);
		} else {
			child->OnUpdate(args);
		}
	}
	if (m_visualEffect != 0) {
		m_visualEffect->Update(this, args.elapsed);
	}
	if (m_animation != 0) {
		m_animation->FrameMove(this, args.elapsed);
	}
	if (m_hidePending && m_visualState == VisualStateTypes::Closed) {
		m_visible = false;
	}
	EventMap::iterator it = m_events.find(EventUpdate);
	if (it != m_events.end()) {
		it->second(args);
	}
	for (uint32_t i = 0; i < m_closedChildren.size(); ++i) {
		RFWnd* child = m_closedChildren[i];
		std::vector<RFWnd*>::iterator itor = std::find(_children.begin(), _children.end(), child);
		assert(itor != _children.end());
		_children.erase(itor);
		child->Destroy();
		delete child;
	}
	m_closedChildren.clear();
}

// 0x408060
void RFWnd::OnDraw() {
}

// 0x408070
void RFWnd::OnAnimFinished(EventArgs&) {
}

// 0x408080
void RFWnd::Close(RFWNDCLOSESTATE closeState) {
	if (m_closeState != static_cast<RFWNDCLOSESTATE>(0)) {
		return;
	}
	m_closeState = closeState;
	if (m_visible) {
		if (m_visualState != VisualStateTypes::Closing && m_visualEffect != 0) {
			m_visualState = VisualStateTypes::Closing;
			m_visualEffect->Reset(this);
		}
	} else if (m_visualEffect != 0) {
		m_visualState = VisualStateTypes::Closed;
	}
	PlaySfx(m_hideSound);
	WindowCloseEventArgs args(closeState, this);
	EventMap::iterator it = m_events.find(EventClose);
	if (it != m_events.end()) {
		it->second(args);
	}
}

// 0x408150
void RFWnd::PlayAnimation(std::wstring const& name) {
	if (m_animation != 0 && !name.empty()) {
		m_animation->Play(this, name);
		AnimationEventArgs args;
		args.animationName = name;
		EventMap::iterator it = m_events.find(EventPlayUIAnimation);
		if (it != m_events.end()) {
			it->second(args);
		}
	}
}

// 0x4082e0
void RFWnd::StopAnimation() {
	if (m_animation != 0) {
		RFAnimation* scene = m_animation->GetPlayingScene();
		if (scene != 0) {
			scene->Stop();
		}
	}
}

// 0x408300
bool RFWnd::IsAnimationPlaying() const {
	if (m_animation != 0) {
		RFAnimation* scene = m_animation->GetPlayingScene();
		if (scene != 0) {
			return scene->IsPlaying();
		}
	}
	return false;
}

// 0x408320
void RFWnd::SetUserString(std::wstring const& key, std::wstring const& value) {
	m_userStrings[key] = value;
}

// 0x408350
std::wstring const& RFWnd::GetUserString(std::wstring const& key) const {
	std::map<std::wstring, std::wstring>::const_iterator it = m_userStrings.find(key);
	if (it == m_userStrings.end()) {
		return g_emptyWString;
	}
	return it->second;
}

// 0x4083d0
bool RFWnd::IsUserStringDefined(std::wstring const& key) const {
	return m_userStrings.find(key) != m_userStrings.end();
}
