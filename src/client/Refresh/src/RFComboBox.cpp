// Original: client\Refresh\src\RFComboBox.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include <new>

#include "RFButton.h"
#include "RFComboBox.h"
#include "RFEmptyString.h"
#include "RFEventArgs.h"
#include "RFListItem.h"
#include "RFListSC.h"
#include "RFManager.h"
#include "RFTextArea.h"
#include "RFWindowFactoryManager.h"

// RFComboBox: a text area (m_textArea), a drop-down list (m_listArea) and a
// button (m_button), the sub-controls Create builds through their factories
// from the creation struct's ComboBase::m_textArea/m_listArea/m_button. The
// button and a click on the text area toggle the list (ShowListArea); a
// selection in the list copies the item's text into the text area and raises
// EventSelChanged. The items' data are heap std::wstring (AddText,
// DeleteAllItems).
// Create's three assert(factory) sit on the original's lines 53, 65 and 78
// (the __LINE__ it pushes): keep the line count above Create.

// The event OnListSelChanged raises, the static const at 0xe47070.
std::wstring const RFComboBox::EventSelChanged = L"SelChanged";
// The class name, the static const at 0xe4708c.
std::wstring const RFComboBox::TypeName = L"ComboBox";

// 0x454950
RFComboBox::RFComboBox() {
}

// 0x454980
RFComboBox::~RFComboBox() {
}

// 0x4549d0
void RFComboBox::Create(WndBaseStruct* wndStruct, RFWnd* parent) {
	RFWnd::Create(wndStruct, parent);
	m_button = 0;
	m_textArea = 0;
	m_listArea = 0;

	ComboBase* data = static_cast<ComboBase*>(wndStruct);
	m_itemTextColor = data->m_itemTextColor.GetValue();
	m_itemSelTextColor = data->m_itemSelTextColor.GetValue();
	m_itemFontName = data->m_itemFontName.GetValue();
	m_itemFontSize = data->m_itemFontSize.GetValue();
	if (data->m_textArea) {
		std::wstring const& name = RFManager::GetInstance().GetCompatibleControlName(data->m_textArea->GetType());
		RFWindowFactory* factory = RFWindowFactoryManager::GetInstance().GetFactory(name);
		assert(factory);

		RFWnd* wnd = factory->Create();
		wnd->Create(data->m_textArea, this);
		wnd->Initialize();
		wnd->SetSubControl(true);
		m_textArea = static_cast<RFTextArea*>(wnd);
	}

	if (data->m_listArea) {
		std::wstring const& name = RFManager::GetInstance().GetCompatibleControlName(data->m_listArea->GetType());
		RFWindowFactory* factory = RFWindowFactoryManager::GetInstance().GetFactory(name);
		assert(factory);

		RFWnd* wnd = factory->Create();
		wnd->Create(data->m_listArea, this);
		wnd->Initialize();
		wnd->SetSubControl(true);
		m_listArea = static_cast<RFListSC*>(wnd);
		m_listArea->SetReselectable(true);
	}

	if (data->m_button) {
		std::wstring const& name = RFManager::GetInstance().GetCompatibleControlName(data->m_button->GetType());
		RFWindowFactory* factory = RFWindowFactoryManager::GetInstance().GetFactory(name);
		assert(factory);

		RFWnd* wnd = factory->Create();
		wnd->Create(data->m_button, this);
		wnd->Initialize();
		wnd->SetSubControl(true);
		wnd->SetPriority(-1);
		m_button = static_cast<RFButton*>(wnd);
	}
}

// 0x454ca0
void RFComboBox::Destroy() {
	DeleteAllItems();
	RFWnd::Destroy();
}

// 0x454cb0 (RFWnd::PreInitialize, RFWnd::GetName and RFWnd::ShowWindow_
// are inlined across units by LTCG)
void RFComboBox::PreInitialize() {
	RFWnd::PreInitialize();
	m_listShown = false;
	m_curSel = -1;
	m_mwheelSelect = true;
	m_listArea->ShowWindow_(false);
	RegisterTo(m_button->GetName(), RFButton::EventClicked,
		Subscriber(&RFComboBox::OnCBButClicked, this));
	RegisterTo(m_textArea->GetName(), RFWnd::EventLButtonUp,
		Subscriber(&RFComboBox::OnCBButClicked, this));
	RegisterTo(m_listArea->GetName(), RFList::EventSelChanged,
		Subscriber(&RFComboBox::OnListSelChanged, this));
	RegisterTo(m_listArea->GetName(), RFList::EventBeginDrawItem,
		Subscriber(&RFComboBox::OnListBeginDrawItem, this));
	RegisterTo(m_listArea->GetName(), RFList::EventEndDrawItem,
		Subscriber(&RFComboBox::OnListEndDrawItem, this));
	RegisterTo(m_listArea->GetName(), RFList::EventDrawItem,
		Subscriber(&RFComboBox::OnListDrawItem, this));
}

// 0x455080
void RFComboBox::OnShow(bool show) {
	RFWnd::OnShow(show);
	ShowListArea(false);
}

// 0x455140
void RFComboBox::ShowListArea(bool show) {
	if (show) {
		m_listShown = !m_listShown;
	} else {
		m_listShown = false;
	}
	SetPriority(m_listShown == true ? -1 : 0);
	m_listArea->ShowWindow(m_listShown);
}

// 0x4551a0
bool RFComboBox::HitTest(RFVector2 const& point, bool force) {
	if (m_visible && m_hitTestVisible && (force || m_enabled)) {
		if (m_listArea->HitTest(point, force)) {
			return true;
		}
		RFRect rect = ClientToScreen();
		return rect.PtInRect(point);
	}
	return false;
}

// 0x455220
void RFComboBox::AddText(std::wstring const& text) {
	m_listArea->InsertItem(new std::wstring(text));
}

// 0x4552b0
void RFComboBox::SetCurSel(int32_t index) {
	if (IsValidItemidx(index) && m_curSel != index) {
		m_curSel = index;
		m_textArea->ClearText();
		m_textArea->AddPlainText(GetText(m_curSel));
		m_listArea->RelaseAllSelectedItem();
		m_listArea->ScrollToItem(m_curSel);
		m_listArea->SelectItem(m_curSel, true);
	}
}

// 0x455360
int32_t RFComboBox::GetCurSel() const {
	return m_curSel;
}

// 0x455370
std::wstring const& RFComboBox::GetText(int32_t index) const {
	if (IsValidItemidx(index)) {
		return *static_cast<std::wstring*>(m_listArea->GetItemData(index));
	}
	return g_emptyWString;
}

// 0x4553f0
std::wstring const& RFComboBox::GetCurText() const {
	return GetText(m_curSel);
}

// 0x455460
int32_t RFComboBox::GetItemCount() const {
	return m_listArea->GetItemCount();
}

// 0x455480
void RFComboBox::DeleteAllItems() {
	m_curSel = -1;
	m_textArea->ClearText();
	for (int32_t i = 0; i < GetItemCount(); ++i) {
		delete static_cast<std::wstring*>(m_listArea->GetItemData(i));
	}
	m_listArea->DeleteAllItems();
}

// 0x455570
bool RFComboBox::IsValidItemidx(int32_t index) const {
	if (index < GetItemCount() && index >= 0) {
		return true;
	}
	return false;
}

// 0x4555a0
void RFComboBox::OnMouseWheel(MouseEventArgs& args) {
	if (!m_mwheelSelect) {
		return;
	}
	int32_t curSel = m_curSel;
	if (curSel == -1) {
		return;
	}
	if (m_listArea->IsWindowVisible(false)) {
		args.handled = true;
		return;
	}
	if (args.wheelDelta < 0.0f) {
		if (IsValidItemidx(curSel + 1)) {
			m_listArea->UnselectItem(curSel, false);
			m_listArea->ScrollToItem(m_curSel + 1);
			m_listArea->SelectItem(m_curSel + 1, true);
			args.handled = true;
		}
	} else if (curSel - 1 >= 0) {
		m_listArea->UnselectItem(curSel, false);
		m_listArea->ScrollToItem(m_curSel - 1);
		m_listArea->SelectItem(m_curSel - 1, true);
		args.handled = true;
	}
}

// 0x455680
void RFComboBox::OnKillFocus() {
	RFWnd::OnKillFocus();
	ShowListArea(false);
}

// 0x4556c0
void RFComboBox::OnCBButClicked(EventArgs&) {
	ShowListArea(true);
}

// 0x455710
void RFComboBox::OnListSelChanged(EventArgs& args) {
	RFListItemBase* item = static_cast<ListSelChangedEventArgs&>(args).item;
	std::wstring* text = static_cast<std::wstring*>(item->GetData());
	if (item->IsSelected()) {
		m_curSel = item->GetIndex();
		m_textArea->ClearText();
		m_textArea->AddPlainText(text->c_str());
		ShowListArea(false);
		WindowEventArgs_<RFComboBox> selArgs(this);
		SendEvent(EventSelChanged, selArgs);
	}
}

// 0x455880
void RFComboBox::OnListBeginDrawItem(EventArgs&) {
}

// 0x455890
void RFComboBox::OnListDrawItem(EventArgs& args) {
	ListDrawItemEventArgs& drawArgs = static_cast<ListDrawItemEventArgs&>(args);
	RFListItemBase* item = drawArgs.item;
	FontInfo font;
	font.name = m_itemFontName;
	font.size = m_itemFontSize;
	font.style = 0;
	font.shadowColor = 0;
	font.outlineColor = 0;
	if (item->IsMouseOver() || item->IsSelected()) {
		font.color = m_itemSelTextColor;
	} else {
		font.color = m_itemTextColor;
	}
	std::wstring* text = static_cast<std::wstring*>(item->GetData());
	RFRect rect = m_listArea->GetScreenRect();
	RFRenderContext* rc = m_listArea->GetRC();
	RFRect const& padding = m_listArea->GetPadding();
	rect.left += padding.left;
	rect.top += padding.top;
	rect.right -= padding.right;
	rect.bottom -= padding.bottom;
	RFRect clip;
	clip.IntersectRect(rc->m_clipRect, rect);
	if (rc->m_clip) {
		RFManager::s_renderer->PushClipRect(clip);
	}
	PrintText(font, static_cast<int32_t>(drawArgs.pos.x), static_cast<int32_t>(drawArgs.pos.y), text->c_str());
	RFRect screenClip;
	screenClip.IntersectRect(rc->m_clipRect, rect);
	rc->m_clipRect = screenClip;
	if (rc->m_clip) {
		RFManager::s_renderer->PopClipRect();
	}
}

// 0x455b60
void RFComboBox::OnListEndDrawItem(EventArgs&) {
}

// lib: 0x416770 (vector deleting destructor), 0x416710 (scalar deleting
// destructor ??_G, paired), 0x416580 (implicit copy constructor), 0x416670
// (implicit operator=), 0x455b70, 0x455b80 (WindowEventArgs_<RFComboBox>'s
// constructor and scalar deleting destructor, paired), 0x455bb0 (the
// Subscriber template constructor for RFComboBox, paired; this in esi),
// 0x455c00, 0x455c20, 0x455c40, 0x455c50, 0x455c60, 0x455c70, 0x455ca0
// (MemberFunctionFunctor<RFComboBox>'s constructor, operator(), AddRef,
// Release, GetRefCount, scalar deleting destructor and destructor, paired;
// the constructors and the destructor take this in eax: LTCG); 0x455870
// (EventArgs's implicit destructor body, vptr 0xd113c8 store with this in
// eax: LTCG copy, reached only from a game-code EH funclet's tail jump)
