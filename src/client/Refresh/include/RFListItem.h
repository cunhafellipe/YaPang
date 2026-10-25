// Original: client\Refresh\include\RFListItem.h (JP 983 exe; the file name
// is inferred: the three item classes are not exported, their code
// 0x421cc0-0x422580 sits just before RFList's own (0x422660))
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFLISTITEM_H
#define PG_CLIENT_REFRESH_INCLUDE_RFLISTITEM_H

#include <string>

#include "RFSize.h"
#include "RFVector2.h"
#include "RFWnd.h"
#include "defines.h"

class RFWnd;

// RFList's event arguments, declared in RFEventArgs.h (EventSelChanged's
// vtable 0xd644e4, EventDrawItem's 0xd644ec).
class ListSelChangedEventArgs;
class ListDrawItemEventArgs;

// The items an RFList holds (RFList 0x180 m_items). None of the three
// classes is exported; the code lies in address order per class:
// constructor, destructor, the virtuals in slot order, the scalar deleting
// destructor. The constructors use LTCG's register convention (this in
// eax) and RFList::InsertItem 0x423df0 / InsertWindowItem_Impl 0x424100
// inline them. Every destructor writes back RFListItemBase's vptr only:
// the destructor bodies are empty.

// Abstract item: vtable 0xd643bc, 23 slots, size 0x10.
class RFListItemBase {
	LAYOUT_CHECK_FRIEND;

public:
	RFListItemBase();                                  //     0x421cc0 (index and draw index -1, the flags false)
	virtual ~RFListItemBase();                         // [0] scalar deleting destructor 0x421d90, ~RFListItemBase 0x421ce0
	virtual std::wstring const& GetTypeName() = 0;     // [1] _purecall; RFListItemWnd's is compared in RFList::DeleteItem 0x424a60
	virtual bool HitTest(RFVector2 const&) = 0;        // [2] _purecall; RFList::GetItemOnPosition_Impl 0x4249d0
	virtual void SetIndex(int32_t);                    // [3] 0x421cf0 (RFList::ReindexItems)
	virtual int32_t GetIndex() const;                  // [4] 0x421d00
	virtual void SetDrawIndex(int32_t);                // [5] 0x421d10 (the order among the drawn items, RFList 0x423930)
	virtual int32_t GetDrawIndex() const;              // [6] 0x421d20
	virtual void Show(bool) = 0;                       // [7] _purecall (InsertItem calls it with true)
	virtual bool IsVisible() = 0;                      // [8] _purecall
	virtual void SetSelected(bool);                    // [9] 0x421d30
	virtual bool IsSelected() const;                   // [10] 0x421d40
	virtual void SetMouseOver(bool);                   // [11] 0x421d50 (RFList::RelaseAllOverItem: false)
	virtual bool IsMouseOver() const;                  // [12] 0x421d60
	virtual void SetDisabled(bool);                    // [13] 0x421d70 (RFList::EnableItem passes !enable)
	virtual bool IsDisabled() const;                   // [14] 0x421d80
	virtual void SetPos(RFVector2 const&) = 0;         // [15] _purecall
	virtual RFVector2 const& GetPos() = 0;             // [16] _purecall (RFListItemWnd's recomputes the member)
	virtual void SetSize(RFSize const&) = 0;           // [17] _purecall (InsertItem: the list's item width/height)
	virtual RFSize const& GetSize() = 0;               // [18] _purecall
	virtual void SetAlpha(float) = 0;                  // [19] _purecall
	virtual float GetAlpha() = 0;                      // [20] _purecall
	virtual void SetData(void*) = 0;                   // [21] _purecall (RFList::InsertItem, SetItemData)
	virtual void* GetData() = 0;                       // [22] _purecall (RFList::GetItemData)

protected:
	int32_t m_index;     // 0x4 (constructor -1)
	int32_t m_drawIndex; // 0x8 (constructor -1)
	bool m_selected;     // 0xc
	bool m_mouseOver;    // 0xd
	bool m_disabled;     // 0xe
};
CHECK_SIZE(RFListItemBase, 0x10);
BEGIN_LAYOUT_CHECK(RFListItemBase)
	CHECK_FIELD(RFListItemBase, m_index, 0x4);
	CHECK_FIELD(RFListItemBase, m_drawIndex, 0x8);
	CHECK_FIELD(RFListItemBase, m_selected, 0xc);
	CHECK_FIELD(RFListItemBase, m_mouseOver, 0xd);
	CHECK_FIELD(RFListItemBase, m_disabled, 0xe);
END_LAYOUT_CHECK

// An item that is a child window of the list (RFList::InsertWindowItem):
// vtable 0xd6441c, size 0x24 (operator new in InsertWindowItem_Impl). The
// selection, mouse-over and disabled setters play the window's "_Selected_",
// "_Over_", "_Disabled_" and "_Normal_" animations.
class RFListItemWnd : public RFListItemBase {
	LAYOUT_CHECK_FRIEND;

public:
	RFListItemWnd();                                   //     0x421dc0
	virtual ~RFListItemWnd();                          // [0] scalar deleting destructor 0x4223a0, ~RFListItemWnd 0x421df0
	virtual std::wstring const& GetTypeName();         // [1] 0x421e00 (returns TypeName)
	virtual bool HitTest(RFVector2 const&);            // [2] 0x421e10 (IsVisible, then the window's HitTest(pt, false))
	virtual void Show(bool);                           // [7] 0x421e40 (the window's ShowWindow)
	virtual bool IsVisible();                          // [8] 0x421e50
	virtual void SetSelected(bool);                    // [9] 0x421e80 (helper 0x421ecf)
	virtual void SetMouseOver(bool);                   // [11] 0x421f60 (helper 0x421fcf)
	virtual void SetDisabled(bool);                    // [13] 0x422070 (helpers 0x42210c, 0x422152; the window's EnableWindow(!disabled))
	virtual void SetPos(RFVector2 const&);             // [15] 0x4221a0 (the window's SetScreenPos)
	virtual RFVector2 const& GetPos();                 // [16] 0x4221d0 (sums the GetPos of the window and its parents)
	virtual void SetSize(RFSize const&);               // [17] 0x422230
	virtual RFSize const& GetSize();                   // [18] 0x422290 (the window's GetSize)
	virtual void SetAlpha(float);                      // [19] 0x4222c0 (clamped to [0, 1] into the window's alpha)
	virtual float GetAlpha();                          // [20] 0x422320
	virtual void SetData(void*);                       // [21] 0x422380 (the data is the window)
	virtual void* GetData();                           // [22] 0x422390

	// The type name, the static const at 0xe46d7c that GetTypeName returns
	// (RFList::DeleteItem compares an item's type name with it).
	static std::wstring const TypeName;

protected:
	RFWnd* m_wnd;     // 0x10 (SetData / GetData)
	RFVector2 m_pos;  // 0x14 (constructor 0)
	RFSize m_size;    // 0x1c (constructor 0)
};
CHECK_SIZE(RFListItemWnd, 0x24);
BEGIN_LAYOUT_CHECK(RFListItemWnd)
	CHECK_FIELD(RFListItemWnd, m_wnd, 0x10);
	CHECK_FIELD(RFListItemWnd, m_pos, 0x14);
	CHECK_FIELD(RFListItemWnd, m_size, 0x1c);
END_LAYOUT_CHECK

// A data item the list draws itself (RFList::InsertItem): vtable 0xd6447c,
// size 0x2c (operator new in InsertItem).
class RFListItem : public RFListItemBase {
	LAYOUT_CHECK_FRIEND;

public:
	RFListItem();                                      //     0x4223d0 (m_alpha is left uninitialised)
	virtual ~RFListItem();                             // [0] scalar deleting destructor 0x422560, ~RFListItem 0x422400
	virtual std::wstring const& GetTypeName();         // [1] 0x422410 (returns TypeName)
	virtual bool HitTest(RFVector2 const&);            // [2] 0x422420 (pt inside [pos, pos + size))
	virtual void Show(bool);                           // [7] 0x4224a0
	virtual bool IsVisible();                          // [8] 0x4224b0
	virtual void SetPos(RFVector2 const&);             // [15] 0x4224c0
	virtual RFVector2 const& GetPos();                 // [16] 0x4224e0
	virtual void SetSize(RFSize const&);               // [17] 0x4224f0
	virtual RFSize const& GetSize();                   // [18] 0x422510
	virtual void SetAlpha(float);                      // [19] 0x422520
	virtual float GetAlpha();                          // [20] 0x422530
	virtual void SetData(void*);                       // [21] 0x422540
	virtual void* GetData();                           // [22] 0x422550

	// The type name, the static const at 0xe46d60 that GetTypeName returns.
	static std::wstring const TypeName;

protected:
	bool m_visible;   // 0x10 (Show / IsVisible)
	RFVector2 m_pos;  // 0x14 (constructor 0)
	RFSize m_size;    // 0x1c (constructor 0)
	float m_alpha;    // 0x24
	void* m_data;     // 0x28 (constructor 0)
};
CHECK_SIZE(RFListItem, 0x2c);
BEGIN_LAYOUT_CHECK(RFListItem)
	CHECK_FIELD(RFListItem, m_visible, 0x10);
	CHECK_FIELD(RFListItem, m_pos, 0x14);
	CHECK_FIELD(RFListItem, m_size, 0x1c);
	CHECK_FIELD(RFListItem, m_alpha, 0x24);
	CHECK_FIELD(RFListItem, m_data, 0x28);
END_LAYOUT_CHECK

// The item members are header inlines: their copies (0x421cc0-0x422580)
// open RFList.cpp's address run, and RFList.cpp's assert lines (659 for
// SetItemData) leave no room for their bodies there. The constructors take
// this in eax (LTCG).

// 0x421cc0
inline RFListItemBase::RFListItemBase()
    : m_index(-1), m_drawIndex(-1), m_selected(false), m_mouseOver(false), m_disabled(false) {
}

// 0x421ce0
inline RFListItemBase::~RFListItemBase() {
}

// 0x421cf0
inline void RFListItemBase::SetIndex(int32_t index) {
	m_index = index;
}

// 0x421d00
inline int32_t RFListItemBase::GetIndex() const {
	return m_index;
}

// 0x421d10
inline void RFListItemBase::SetDrawIndex(int32_t drawIndex) {
	m_drawIndex = drawIndex;
}

// 0x421d20
inline int32_t RFListItemBase::GetDrawIndex() const {
	return m_drawIndex;
}

// 0x421d30
inline void RFListItemBase::SetSelected(bool selected) {
	m_selected = selected;
}

// 0x421d40
inline bool RFListItemBase::IsSelected() const {
	return m_selected;
}

// 0x421d50
inline void RFListItemBase::SetMouseOver(bool mouseOver) {
	m_mouseOver = mouseOver;
}

// 0x421d60
inline bool RFListItemBase::IsMouseOver() const {
	return m_mouseOver;
}

// 0x421d70
inline void RFListItemBase::SetDisabled(bool disabled) {
	m_disabled = disabled;
}

// 0x421d80
inline bool RFListItemBase::IsDisabled() const {
	return m_disabled;
}

// 0x421dc0
inline RFListItemWnd::RFListItemWnd() : m_wnd(0) {
}

// 0x421df0
inline RFListItemWnd::~RFListItemWnd() {
}

// 0x421e00
inline std::wstring const& RFListItemWnd::GetTypeName() {
	return TypeName;
}

// 0x421e10
inline bool RFListItemWnd::HitTest(RFVector2 const& pos) {
	if (IsVisible()) {
		return m_wnd->HitTest(pos, false);
	}
	return false;
}

// 0x421e40
inline void RFListItemWnd::Show(bool show) {
	m_wnd->ShowWindow(show);
}

// 0x421e50
inline bool RFListItemWnd::IsVisible() {
	return m_wnd->IsWindowVisible(false);
}

// 0x421e80
inline void RFListItemWnd::SetSelected(bool selected) {
	m_selected = selected;
	if (selected) {
		m_wnd->PlayAnimation(L"_Selected_");
	} else {
		m_wnd->PlayAnimation(L"_Normal_");
	}
}

// 0x421f60
inline void RFListItemWnd::SetMouseOver(bool mouseOver) {
	if (!IsDisabled()) {
		m_mouseOver = mouseOver;
		if (mouseOver) {
			if (!IsSelected()) {
				m_wnd->PlayAnimation(L"_Over_");
			}
		} else {
			if (!IsSelected()) {
				m_wnd->PlayAnimation(L"_Normal_");
			}
		}
	}
}

// 0x422070
inline void RFListItemWnd::SetDisabled(bool disabled) {
	m_disabled = disabled;
	if (disabled) {
		m_wnd->PlayAnimation(L"_Disabled_");
	} else {
		m_wnd->PlayAnimation(L"_Normal_");
	}
	m_wnd->EnableWindow(!disabled);
}

// 0x4221a0
inline void RFListItemWnd::SetPos(RFVector2 const& pos) {
	m_pos = pos;
	m_wnd->SetScreenPos(pos.x, pos.y);
}

// 0x4221d0
inline RFVector2 const& RFListItemWnd::GetPos() {
	m_pos = m_wnd->GetScreenPos();
	return m_pos;
}

// 0x422230
inline void RFListItemWnd::SetSize(RFSize const& size) {
	m_size = size;
	m_wnd->SetSize(size);
}

// 0x422290
inline RFSize const& RFListItemWnd::GetSize() {
	m_size = m_wnd->GetSize();
	return m_size;
}

// 0x4222c0
inline void RFListItemWnd::SetAlpha(float alpha) {
	m_wnd->SetAlpha(alpha);
}

// 0x422320
inline float RFListItemWnd::GetAlpha() {
	return m_wnd->GetAlpha(false);
}

// 0x422380
inline void RFListItemWnd::SetData(void* data) {
	m_wnd = static_cast<RFWnd*>(data);
}

// 0x422390
inline void* RFListItemWnd::GetData() {
	return m_wnd;
}

// 0x4223d0
inline RFListItem::RFListItem() : m_visible(false), m_data(0) {
}

// 0x422400
inline RFListItem::~RFListItem() {
}

// 0x422410
inline std::wstring const& RFListItem::GetTypeName() {
	return TypeName;
}

// 0x422420
inline bool RFListItem::HitTest(RFVector2 const& pos) {
	RFRect rect(m_pos.x, m_pos.y, m_size.width + m_pos.x, m_size.height + m_pos.y);
	return rect.PtInRect(pos);
}

// 0x4224a0
inline void RFListItem::Show(bool show) {
	m_visible = show;
}

// 0x4224b0
inline bool RFListItem::IsVisible() {
	return m_visible;
}

// 0x4224c0
inline void RFListItem::SetPos(RFVector2 const& pos) {
	m_pos = pos;
}

// 0x4224e0
inline RFVector2 const& RFListItem::GetPos() {
	return m_pos;
}

// 0x4224f0
inline void RFListItem::SetSize(RFSize const& size) {
	m_size = size;
}

// 0x422510
inline RFSize const& RFListItem::GetSize() {
	return m_size;
}

// 0x422520
inline void RFListItem::SetAlpha(float alpha) {
	m_alpha = alpha;
}

// 0x422530
inline float RFListItem::GetAlpha() {
	return m_alpha;
}

// 0x422540
inline void RFListItem::SetData(void* data) {
	m_data = data;
}

// 0x422550
inline void* RFListItem::GetData() {
	return m_data;
}

#endif
