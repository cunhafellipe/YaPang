// Original: client\Refresh\src\RFList.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
// lib: 0x4150b0 (vector deleting destructor, ??_E of the dllexport class),
// 0x425600 (std::_Find_if<RFListItemBase**, ListItemOnPosition>), 0x425690
// (std::_For_each<RFListItemBase**, ListItemDeleter>), 0x424e10, 0x424f40,
// 0x4257b0, 0x425900 (std::vector<RFListItemBase*> push_back/_Reserve/
// emplace_back/rotate), 0x414e00 (implicit copy constructor), 0x414f60
// (implicit operator=); scalar deleting destructors: 0x421d90
// (RFListItemBase), 0x4223a0 (RFListItemWnd), 0x422560 (RFListItem), 0x4225b0
// (ListSelChangedEventArgs), 0x422620 (ListDrawItemEventArgs), 0x424ef0
// (WindowEventArgs_<RFList>), 0x425460 (MemberFunctionFunctor<RFList>, inside
// this unit's address run), 0x415080 (RFList); implicit destructors:
// 0x4225e0 (ListSelChangedEventArgs), 0x4225f0 (EventArgs), 0x422650
// (ListDrawItemEventArgs), 0x425490 (MemberFunctionFunctor<RFList>); 0x4253c0
// (ListItemDrawer's implicit copy constructor); std::vector<RFListItemBase*>
// and iterator members: 0x424de0, 0x424df0, 0x424e00, 0x424e70, 0x424ec0,
// 0x424ed0, 0x424f20, 0x424fa0, 0x424fb0, 0x424fc0, 0x425050, 0x425080,
// 0x425250, 0x425370, 0x425380, 0x425390, 0x4254a0, 0x425550, 0x4255b0,
// 0x4255c0, 0x425640, 0x425650; algorithms: 0x425090, 0x4254b0
// (for_each<ListItemDestroyer>), 0x4250d0, 0x4254e0
// (for_each<ListItemIndexer>), 0x4250f0, 0x425530 (find), 0x425310, 0x425660
// (find_if<ListItemDataEquals>), 0x425350 (for_each<ListItemDeleter>),
// 0x425560 (_For_each<ListItemDrawer>), 0x4252e0 (find_if<ListItemOnPosition>
// over vector iterators), 0x425300 (vector const_iterator operator+=, LTCG
// register copy); 0x424ea0 (unreferenced LTCG copy of the
// WindowEventArgs_<RFList> constructor without the sender store)
#include <algorithm>
#include <new>

#include "RFList.h"
#include "RFEventArgs.h"
#include "RFManager.h"
#include "RFRenderer.h"

// The events an RFList raises, the static consts at 0xe46cf0, 0xe46d0c,
// 0xe46d28 and 0xe46d44.
std::wstring const RFList::EventSelChanged = L"SelChanged";
std::wstring const RFList::EventBeginDrawItem = L"BeginDrawItem";
std::wstring const RFList::EventEndDrawItem = L"EndDrawItem";
std::wstring const RFList::EventDrawItem = L"DrawItem";

// The item type names, the static consts at 0xe46d60 (RFListItem) and
// 0xe46d7c (RFListItemWnd).
std::wstring const RFListItem::TypeName = L"UserItemType";
std::wstring const RFListItemWnd::TypeName = L"WindowItemType";

// The predicate of GetItemOnPosition_Impl: a visible item under the point
// (std::find_if instantiation 0x425600). The name is ours.
struct ListItemOnPosition {
	explicit ListItemOnPosition(RFVector2 const& pos) : m_pos(pos) {}  // 0x424a20
	bool operator()(RFListItemBase* item) const {  // 0x424a30
		if (item->IsVisible() && item->HitTest(m_pos)) {
			return true;
		}
		return false;
	}
	RFVector2 m_pos;
};

// The functor of DeleteAllItems: deletes an item, and a window item's window
// from the list's children (std::for_each instantiation 0x425690). The name
// is ours.
struct ListItemDeleter {
	explicit ListItemDeleter(RFList* list) : m_list(list) {}  // 0x424d40
	void operator()(RFListItemBase* item) const {  // 0x424d50
		if (item->GetTypeName() == RFListItemWnd::TypeName) {
			m_list->DeleteChild(static_cast<RFWnd*>(item->GetData()));
		}
		delete item;
	}
	RFList* m_list;
};

// The functor of OnDraw: draws the items inside the list's clip rectangle and
// numbers the drawn ones (std::for_each instantiation 0x425120; operator()
// 0x423930; implicit copy constructor 0x4253c0). The name is ours.
struct ListItemDrawer {
	// 0x423900
	ListItemDrawer(int32_t* drawIndex, RFList* list, RFRect const& clipRect)
		: m_drawIndex(drawIndex), m_list(list), m_clipRect(clipRect) {}
	void operator()(RFListItemBase* item);
	int32_t* m_drawIndex;  // 0x0
	RFList* m_list;        // 0x4
	RFRect m_clipRect;     // 0x8
};

// The functor of OnDestroy: deletes an item (std::for_each instantiation
// 0x4254b0). The name is ours.
struct ListItemDestroyer {
	void operator()(RFListItemBase* item) const {  // 0x422940
		delete item;
	}
};

// The predicate of DeleteItem(void*): the item holding the data
// (std::find_if instantiation 0x425660). The name is ours.
struct ListItemDataEquals {
	explicit ListItemDataEquals(void* const& data) : m_data(data) {}  // 0x424c70
	bool operator()(RFListItemBase* item) const {  // 0x424c80
		return item->GetData() == m_data;
	}
	void* m_data;  // 0x0
};

// The functor of ReindexItems: numbers the items in order (std::for_each
// instantiation 0x4254e0). The name is ours.
struct ListItemIndexer {
	explicit ListItemIndexer(int32_t* index) : m_index(index) {}  // 0x422d70
	void operator()(RFListItemBase* item) const {  // 0x422d80
		item->SetIndex((*m_index)++);
	}
	int32_t* m_index;  // 0x0
};

// 0x422660
RFList::RFList() {
}

// 0x4226e0
RFList::~RFList() {
}

// 0x422750
void RFList::Create(WndBaseStruct* wndStruct, RFWnd* parent) {
	RFWndEx::Create(wndStruct, parent);
	ListBase* listStruct = static_cast<ListBase*>(wndStruct);
	m_padding = listStruct->m_padding;
	m_columnCount = listStruct->m_columnCount;
	m_itemSize = RFSize(listStruct->_itemrect.Width(), listStruct->_itemrect.Height());
	m_selectionMode = listStruct->m_selectionMode;
	m_deselectable = listStruct->m_deselectable;
	m_reselectable = false;
	m_rClickAction = listStruct->m_rClickAction;
	m_orientation = listStruct->m_orientation.GetValue();
	m_firstDrawItemIndex = -1;
	m_itemClipEnabled = true;
}

// 0x422830
void RFList::PreInitialize() {
	// The original's body is RFWnd's inline PreInitialize, the same code as
	// RFWndEx::PreInitialize (0x41d9e0).
	RFWnd::PreInitialize();
}

// 0x422890
void RFList::OnDestroy() {
	m_selItems.clear();
	std::for_each(m_items.begin(), m_items.end(), ListItemDestroyer());
	m_items.clear();
	RFWndEx::OnDestroy();
}

// 0x422950
void RFList::OnLayoutChanged(uint32_t flags) {
	if (flags & 2) {
		ApplyAnchor();
	}
}

// 0x422970
void RFList::EnableMultiSelect(bool enable) {
	m_selectionMode = static_cast<ListSelectionTypes::Type>(enable != false);
}

// 0x422990
void RFList::RelaseAllSelectedItem() {
	for (uint32_t i = 0; i < m_items.size(); ++i) {
		RFListItemBase* item = m_items[i];
		if (item->IsSelected()) {
			item->SetSelected(false);
			ListSelChangedEventArgs args(this, item);
			SendEvent(EventSelChanged, args);
		}
	}
	m_selItems.clear();
}

// 0x422ac0
void RFList::RelaseAllOverItem() {
	for (uint32_t i = 0; i < m_items.size(); ++i) {
		m_items[i]->SetMouseOver(false);
	}
}

// 0x422b10
void RFList::OnItemAdded() {
	UpdateItemPosition();
}

// 0x422b20
void RFList::OnItemRemoved() {
	UpdateItemPosition();
}

// 0x422b30
void RFList::OnLButtonDown(MouseEventArgs& e) {
	RFWnd::OnLButtonDown(e);
	e.handled = true;
	if (!m_items.empty()) {
		ButtonDown_Impl(e);
	}
}

// 0x422b70
void RFList::OnRButtonDown(MouseEventArgs& e) {
	RFWnd::OnRButtonDown(e);
	e.handled = true;
	if (!m_items.empty()) {
		ButtonDown_Impl(e);
	}
}

// 0x422bc0
void RFList::OnMouseEnter() {
	RFWnd::OnMouseEnter();
}

// 0x422c30
void RFList::OnMouseLeave() {
	RFWnd::OnMouseLeave();
	RelaseAllOverItem();
}

// 0x422cc0
void RFList::OnMouseMove(MouseEventArgs& e) {
	RFWnd::OnMouseMove(e);
	e.handled = true;
	if (m_items.empty()) {
		return;
	}
	RFListItemBase* item = GetItemOnPosition_Impl(e.pos);
	if (item != 0) {
		if (!item->IsMouseOver()) {
			RelaseAllOverItem();
			item->SetMouseOver(true);
		}
	} else {
		RelaseAllOverItem();
	}
}

// 0x422d40
void RFList::ReindexItems() {
	int32_t index = 0;
	std::for_each(m_items.begin(), m_items.end(), ListItemIndexer(&index));
}

// 0x422da0
void RFList::DeleteFromSelList(RFListItemBase* item) {
	std::vector<RFListItemBase*>::iterator it = std::find(m_selItems.begin(), m_selItems.end(), item);
	if (it != m_selItems.end()) {
		m_selItems.erase(it);
	}
}

// 0x422e00
void RFList::ButtonDown_Impl(MouseEventArgs& e) {
	// button 1 with right-click action 1: the right button does not select
	if (e.button == 1 && m_rClickAction == 1) {
		return;
	}
	RFListItemBase* item = GetItemOnPosition_Impl(e.pos);
	if (item == 0) {
		return;
	}
	bool wasSelected = item->IsSelected();
	if (m_selectionMode == 1) {
		MultipleSelectionProc(item, e);
	} else {
		SingleSelectionProc(item, e);
	}
	UpdateSelectList();
	if (wasSelected != item->IsSelected() || (item->IsSelected() && m_reselectable)) {
		ListSelChangedEventArgs args(this, item);
		SendEvent(EventSelChanged, args);
	}
}

// 0x422f50
void RFList::MultipleSelectionProc(RFListItemBase* item, MouseEventArgs& e) {
	if (item->IsSelected()) {
		if (e.button == 0) {
			item->SetSelected(false);
		}
	} else {
		item->SetSelected(true);
	}
}

// 0x422f90
void RFList::SingleSelectionProc(RFListItemBase* item, MouseEventArgs& e) {
	if (m_deselectable && e.button == 0) {
		if (item->IsSelected()) {
			item->SetSelected(false);
			return;
		}
	} else if (item->IsSelected()) {
		return;
	}
	RelaseAllSelectedItem();
	item->SetSelected(true);
}

// 0x423000
void RFList::UpdateSelectList() {
	m_selItems.clear();
	for (uint32_t i = 0; i < m_items.size(); ++i) {
		if (m_items[i]->IsSelected()) {
			m_selItems.push_back(m_items[i]);
		}
	}
}

// 0x423180
void RFList::OnUpdate(UpdateEventArgs& e) {
	RFWndEx::OnUpdate(e);
	UpdateItemPosition();
}

// 0x4231a0
void RFList::UpdateItemPosition() {
	RFVector2 origin = GetScreenPos() + m_scrollOffset;
	switch (m_orientation) {
	case OrientationTypes::Vertical:
		VericalAlignment(origin);
		break;
	case OrientationTypes::Horizontal:
		HorizontalAlignment(origin);
		break;
	}
}

// 0x423240
void RFList::VericalAlignment(RFVector2 const& origin) {
	RFRect clientRect = ClientToScreen();
	int32_t firstDrawIndex = -1;
	clientRect.left += m_padding.left;
	clientRect.top += m_padding.top;
	clientRect.right -= m_padding.right;
	clientRect.bottom -= m_padding.bottom;
	float y = 0.0f;
	for (int32_t row = 0;; ++row) {
		float x = 0.0f;
		float rowHeight = 0.0f;
		for (int32_t column = 0; column < m_columnCount; ++column) {
			int32_t index = m_columnCount * row + column;
			if (index >= static_cast<int32_t>(m_items.size())) {
				return;
			}
			RFListItemBase* item = m_items[index];
			item->SetAlpha(GetAlpha(false));
			item->SetPos(RFVector2(origin.x + m_padding.left + x, origin.y + m_padding.top + y));
			x += item->GetSize().width;
			if (rowHeight < item->GetSize().height) {
				rowHeight = item->GetSize().height;
			}
			if (firstDrawIndex == -1) {
				float left = item->GetPos().x;
				float top = item->GetPos().y;
				RFRect itemRect(left, top, left + item->GetSize().width, top + item->GetSize().height);
				RFRect visible;
				if (visible.IntersectRect(clientRect, itemRect)) {
					firstDrawIndex = index;
					m_firstDrawItemIndex = index;
				}
			}
		}
		y += rowHeight;
	}
}

// 0x4234a0
void RFList::HorizontalAlignment(RFVector2 const& origin) {
	RFRect clientRect = ClientToScreen();
	int32_t firstDrawIndex = -1;
	clientRect.left += m_padding.left;
	clientRect.top += m_padding.top;
	clientRect.right -= m_padding.right;
	clientRect.bottom -= m_padding.bottom;
	float x = 0.0f;
	for (int32_t column = 0;; ++column) {
		float columnWidth = 0.0f;
		float y = 0.0f;
		for (int32_t row = 0; row < m_columnCount; ++row) {
			int32_t index = m_columnCount * column + row;
			if (index >= static_cast<int32_t>(m_items.size())) {
				return;
			}
			RFListItemBase* item = m_items[index];
			item->SetAlpha(GetAlpha(false));
			item->SetPos(RFVector2(origin.x + m_padding.left + x, origin.y + m_padding.top + y));
			y += item->GetSize().height;
			if (columnWidth < item->GetSize().width) {
				columnWidth = item->GetSize().width;
			}
			if (firstDrawIndex == -1) {
				float left = item->GetPos().x;
				float top = item->GetPos().y;
				RFRect itemRect(left, top, left + item->GetSize().width, top + item->GetSize().height);
				RFRect visible;
				if (visible.IntersectRect(clientRect, itemRect)) {
					firstDrawIndex = index;
					m_firstDrawItemIndex = index;
				}
			}
		}
		x += columnWidth;
	}
}

// 0x423700
void RFList::OnDraw() {
	RFWndEx::OnDraw();
	if (m_items.empty()) {
		return;
	}
	RFRect clientRect = ClientToScreen();
	clientRect.left += m_padding.left;
	clientRect.top += m_padding.top;
	clientRect.right -= m_padding.right;
	clientRect.bottom -= m_padding.bottom;
	m_rc.m_clip = m_itemClipEnabled;
	RFRect clipRect;
	clipRect.IntersectRect(m_rc.m_clipRect, clientRect);
	if (m_rc.m_clip) {
		RFManager::s_renderer->PushClipRect(clipRect);
	}
	WindowEventArgs_<RFList> args(this);
	SendEvent(EventBeginDrawItem, args);
	int32_t drawIndex = 0;
	std::for_each(m_items.begin(), m_items.end(),
		ListItemDrawer(&drawIndex, this, clientRect));
	SendEvent(EventEndDrawItem, args);
	RFRect restoredRect;
	restoredRect.IntersectRect(m_rc.m_clipRect, clientRect);
	m_rc.m_clipRect = restoredRect;
	if (m_rc.m_clip) {
		RFManager::s_renderer->PopClipRect();
	}
	m_rc.m_clip = !m_itemClipEnabled;
}

// 0x423930
void ListItemDrawer::operator()(RFListItemBase* item) {
	float left = item->GetPos().x;
	float top = item->GetPos().y;
	float right = left + item->GetSize().width;
	float bottom = top + item->GetSize().height;
	RFRect visibleRect;
	if (visibleRect.IntersectRect(m_clipRect, RFRect(left, top, right, bottom))) {
		item->SetDrawIndex((*m_drawIndex)++);
		ListDrawItemEventArgs args;
		args.sender = m_list;
		args.item = item;
		args.pos = item->GetPos();
		m_list->SendEvent(RFList::EventDrawItem, args);
	}
}

// 0x423a60
void RFList::OnItemMouseLeave(EventArgs&) {
	RelaseAllOverItem();
}

// 0x423a70
float RFList::GetAllItemsWidth() const {
	float width = 0.0f;
	int32_t columnCount = m_columnCount;
	if (m_orientation == 0) {
		// a row per columnCount items: the widest row
		for (int32_t row = 0; ; row += columnCount) {
			float rowWidth = 0.0f;
			int32_t index = row;
			for (int32_t column = 0; column < columnCount; ++column) {
				if (index >= GetItemCount()) {
					if (width < rowWidth) {
						width = rowWidth;
					}
					return width;
				}
				rowWidth += m_items[index]->GetSize().width;
				++index;
			}
			if (width < rowWidth) {
				width = rowWidth;
			}
		}
	} else {
		// a column per columnCount items: the columns' widest items side by side
		for (int32_t row = 0; ; row += columnCount) {
			float columnWidth = 0.0f;
			int32_t index = row;
			for (int32_t column = 0; column < columnCount; ++column) {
				if (index >= GetItemCount()) {
					return columnWidth + width;
				}
				RFListItemBase* item = m_items[index];
				if (columnWidth < item->GetSize().width) {
					columnWidth = item->GetSize().width;
				}
				++index;
			}
			width += columnWidth;
		}
	}
}

// 0x423bc0
RFRect RFList::GetItemRect(int32_t column, int32_t row, bool update) {
	RFRect rect;
	int32_t index = m_columnCount * row + column;
	if (index >= 0 && index < GetItemCount()) {
		if (update) {
			UpdateItemPosition();
		}
		rect.left = m_items[index]->GetPos().x - m_scrollOffset.x;
		rect.top = m_items[index]->GetPos().y - m_scrollOffset.y;
		rect.right = m_items[index]->GetSize().width + rect.left;
		rect.bottom = rect.top + m_items[index]->GetSize().height;
	}
	return rect;
}

// 0x423c80
float RFList::GetAllItemsHeight() const {
	float height = 0.0f;
	int32_t columnCount = m_columnCount;
	if (m_orientation == 0) {
		// a row per columnCount items: the rows' tallest items stacked
		int32_t index = 0;
		for (;;) {
			int32_t row = index;
			float rowHeight = 0.0f;
			for (int32_t column = 0; column < columnCount; ++column) {
				if (index >= GetItemCount()) {
					return rowHeight + height;
				}
				RFListItemBase* item = m_items[index++];
				if (rowHeight < item->GetSize().height) {
					rowHeight = item->GetSize().height;
				}
			}
			height += rowHeight;
			index = row + columnCount;
		}
	} else {
		// a column per columnCount items: the tallest column
		int32_t index = 0;
		for (;;) {
			int32_t row = index;
			float columnHeight = 0.0f;
			for (int32_t column = 0; column < columnCount; ++column) {
				if (index >= GetItemCount()) {
					if (height < columnHeight) {
						height = columnHeight;
					}
					return height;
				}
				columnHeight += m_items[index]->GetSize().height;
				++index;
			}
			if (height < columnHeight) {
				height = columnHeight;
			}
			index = row + columnCount;
		}
	}
}

// 0x423dd0
OrientationTypes::Type RFList::GetOrientation() const {
	return m_orientation;
}

// 0x423de0
void RFList::SetOrientation(OrientationTypes::Type orientation) {
	m_orientation = orientation;
}

// 0x423df0
void RFList::InsertItem(void* data) {
	RFListItemBase* item = new RFListItem;
	item->SetData(data);
	item->SetIndex(m_items.size());
	item->Show(true);
	item->SetSelected(false);
	item->SetSize(RFSize(m_itemSize.width, m_itemSize.height));
	item->SetAlpha(GetAlpha(false));
	m_items.push_back(item);
	OnItemAdded();
}

// 0x423f70
RFWnd* RFList::InsertWindowItem(std::wstring const& name) {
	RFWnd* wnd = CreateWnd<RFWndEx>(RFManager::GetInstancePtr(), name, this);
	InsertWindowItem(wnd);
	return wnd;
}

// 0x423fa0
RFWnd* RFList::InsertWindowItem(RFWnd* wnd) {
	RFListItemWnd* item = InsertWindowItem_Impl(wnd);
	m_items.push_back(item);
	OnItemAdded();
	return wnd;
}

// 0x424040
RFWnd* RFList::InsertWindowItem(std::wstring const& name, uint32_t index) {
	if (index > m_items.size()) {
		return 0;
	}
	RFWnd* wnd = CreateWnd<RFWndEx>(RFManager::GetInstancePtr(), name, this);
	InsertWindowItem(wnd, index);
	return wnd;
}

// 0x424090
RFWnd* RFList::InsertWindowItem(RFWnd* wnd, uint32_t index) {
	RFListItemWnd* item = InsertWindowItem_Impl(wnd);
	m_items.insert(m_items.begin() + index, item);
	OnItemAdded();
	return wnd;
}

// 0x424100
RFListItemWnd* RFList::InsertWindowItem_Impl(RFWnd* wnd) {
	if (wnd->GetSuper(false) != this) {
		RegisterChild(wnd);
	}
	wnd->SetSubControl(true);
	wnd->GetRC()->m_clip = m_itemClipEnabled;
	wnd->RegisterTo(L"", RFWnd::EventMouseLeave,
		Subscriber(&RFList::OnItemMouseLeave, this));
	RFListItemWnd* item = new RFListItemWnd;
	item->SetData(wnd);
	item->SetIndex(m_items.size());
	item->SetSelected(false);
	item->SetSize(wnd->GetSize());
	item->SetAlpha(GetAlpha(false));
	item->Show(true);
	return item;
}

// The blank lines keep each assert on the original's line (its __LINE__
// is pushed to _wassert): 659, 670, 681, 692, 736, 814.








// 0x424340
void RFList::SetItemData(int32_t index, void* data) {
	if (!IsValidIndex(index)) {
		assert(!"Parameter 'index' out of range.");
		return;
	}
	m_items[index]->SetData(data);
}



// 0x4243a0
void* RFList::GetItemData(int32_t index) {
	if (!IsValidIndex(index)) {
		assert(!"Parameter 'index' out of range.");
		return 0;
	}
	return m_items[index]->GetData();
}



// 0x4243f0
RFListItemBase* RFList::GetListItem(int32_t index) {
	if (!IsValidIndex(index)) {
		assert(!"Parameter 'index' out of range.");
		return 0;
	}
	return m_items[index];
}



// 0x424440
void RFList::SelectItem(int32_t index, bool sendEvent) {
	if (!IsValidIndex(index)) {
		assert(!"Parameter 'index' out of range.");
		return;
	}
	RFListItemBase* item = m_items[index];
	if (!item->IsSelected()) {
		item->SetSelected(true);
		m_selItems.push_back(item);
		if (sendEvent) {
			ListSelChangedEventArgs args(this, item);
			SendEvent(EventSelChanged, args);
		}
	}
}

// 0x424540
void RFList::SelectItem(void* data, bool sendEvent) {
	for (int32_t i = 0; i < GetItemCount(); ++i) {
		RFListItemBase* item = m_items[i];
		if (item->GetData() == data && !item->IsSelected()) {
			item->SetSelected(true);
			m_selItems.push_back(item);
			if (sendEvent) {
				ListSelChangedEventArgs args(this, item);
				SendEvent(EventSelChanged, args);
			}
		}
	}
}













// 0x4246e0
void RFList::UnselectItem(int32_t index, bool sendEvent) {
	if (!IsValidIndex(index)) {
		assert(!"Parameter 'index' out of range.");
		return;
	}
	RFListItemBase* item = m_items[index];
	if (item->IsSelected()) {
		item->SetSelected(false);
		DeleteFromSelList(item);
		if (sendEvent) {
			ListSelChangedEventArgs args(this, item);
			SendEvent(EventSelChanged, args);
		}
	}
}

// 0x4247d0
void RFList::UnselectItem(void* data, bool sendEvent) {
	for (uint32_t i = 0; i < m_items.size(); ++i) {
		RFListItemBase* item = m_items[i];
		if (item->GetData() == data && item->IsSelected()) {
			item->SetSelected(false);
			DeleteFromSelList(item);
			if (sendEvent) {
				ListSelChangedEventArgs args(this, item);
				SendEvent(EventSelChanged, args);
			}
		}
	}
}

// 0x424920
void RFList::EnableItem(void* item, bool enable) {
	for (uint32_t i = 0; i < m_items.size(); ++i) {
		RFListItemBase* listItem = m_items[i];
		if (listItem == item) {
			listItem->SetDisabled(!enable);
			break;
		}
	}
}

// 0x424970
RFListItemBase* RFList::GetItemUnderCursor() {
	return GetItemOnPosition_Impl(RFManager::GetInstance().GetCursorPos());
}

// 0x4249d0
RFListItemBase* RFList::GetItemOnPosition_Impl(RFVector2 const& pos) {
	std::vector<RFListItemBase*>::iterator it =
		std::find_if(m_items.begin(), m_items.end(), ListItemOnPosition(pos));
	if (it == m_items.end()) {
		return 0;
	}
	return *it;
}





















// 0x424a60
void RFList::DeleteItem(int32_t index) {
	if (!IsValidIndex(index)) {
		assert(!"Parameter 'index' out of range.");
		return;
	}
	std::vector<RFListItemBase*>::iterator it = m_items.begin() + index;
	DeleteFromSelList(*it);
	if ((*it)->GetTypeName() == RFListItemWnd::TypeName) {
		DeleteChild(static_cast<RFWnd*>((*it)->GetData()));
	}
	if (*it != 0) {
		delete *it;
		*it = 0;
	}
	m_items.erase(it);
	ReindexItems();
	OnItemRemoved();
}

// 0x424b70
void RFList::DeleteItem(void* data) {
	std::vector<RFListItemBase*>::iterator it =
		std::find_if(m_items.begin(), m_items.end(), ListItemDataEquals(data));
	if (it == m_items.end()) {
		return;
	}
	DeleteFromSelList(*it);
	if ((*it)->GetTypeName() == RFListItemWnd::TypeName) {
		DeleteChild(static_cast<RFWnd*>((*it)->GetData()));
	}
	if (*it != 0) {
		delete *it;
		*it = 0;
	}
	m_items.erase(it);
	ReindexItems();
	OnItemRemoved();
}

// 0x424ca0
void RFList::DeleteAllItems() {
	m_selItems.clear();
	std::for_each(m_items.begin(), m_items.end(), ListItemDeleter(this));
	m_items.clear();
	OnItemRemoved();
}

// 0x424db0
bool RFList::IsValidIndex(int32_t index) const {
	if (index < 0 || index >= GetItemCount()) {
		return false;
	}
	return true;
}
