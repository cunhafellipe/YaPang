// Original: client\Refresh\include\RFList.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFLIST_H
#define PG_CLIENT_REFRESH_INCLUDE_RFLIST_H

#include <vector>

#include "RFExport.h"
#include "RFWndEx.h"
#include "RFRect.h"
#include "RFSize.h"
#include "RFVector2.h"
#include "defines.h"
#include "padding.h"

#include "RFListItem.h"

// How a list selects its items (GetSelectionMode 0x414d60,
// SetSelectionMode 0x414d70); the values come with the XML loader.
struct ListSelectionTypes {
	enum Type {
	};
};

// What a right click on a list item does (GetRClickAction 0x414dc0,
// SetRClickAction 0x414dd0); the values come with the XML loader.
enum RFRCLICKACTION {
};

// The layout data of an RFList, read by RFList::Create (0x422750); RTTI
// ".?AUListBase@@", vtable 0xd65a94. The frame's WndExBase followed by the
// list's settings. Its destructor (~ListBase 0x43d3e0, scalar deleting
// 0x43d3b0) destroys the RFOrientationProperty at 0x1b0 and then the WndExBase part.
struct ListBase : public WndExBase {
	// [1] 0x43d3a0 (the list's WndType)
	virtual WndType GetType() const { return WndType_List; }

	ListBase();

	RFRect m_padding;                                         // 0x180
	int32_t m_columnCount;                                    // 0x190
	RFRect _itemrect;                                         // 0x194 (the name ReadListBase's asserts spell), Create keeps its width and height
	ListSelectionTypes::Type m_selectionMode;                 // 0x1a4
	bool m_deselectable;                                      // 0x1a8
	RFRCLICKACTION m_rClickAction;                            // 0x1ac
	RFOrientationProperty m_orientation;                      // 0x1b0 "Orientation"
};
CHECK_SIZE(ListBase, 0x1d4);
CHECK_FIELD(ListBase, m_padding, 0x180);
CHECK_FIELD(ListBase, m_columnCount, 0x190);
CHECK_FIELD(ListBase, _itemrect, 0x194);
CHECK_FIELD(ListBase, m_selectionMode, 0x1a4);
CHECK_FIELD(ListBase, m_deselectable, 0x1a8);
CHECK_FIELD(ListBase, m_rClickAction, 0x1ac);
CHECK_FIELD(ListBase, m_orientation, 0x1b0);

// The list control: vtable 0xd63b5c, 37 slots (0–33 as RFWndEx, the list
// virtuals 34–36 are new), size 0x1dc (from its scalar deleting destructor).
// dllexport as in the original library: cl 16 then emits the vector deleting
// destructor and the implicit copy constructor (0x414e00) and copy
// assignment (0x414f60).
class RF_API RFList: public RFWndEx {
	LAYOUT_CHECK_FRIEND;

public:
	// The events an RFList raises (exported static consts; their initialisers
	// are not in the unit). EventSelChanged carries a ListSelChangedEventArgs,
	// EventDrawItem a ListDrawItemEventArgs (RFEventArgs.h).
	static std::wstring const EventSelChanged;                // 0xe46cf0
	static std::wstring const EventBeginDrawItem;             // 0xe46d0c
	static std::wstring const EventEndDrawItem;               // 0xe46d28
	static std::wstring const EventDrawItem;                  // 0xe46d44

	virtual wchar_t const* ToString() const;                  // [0]  0x414cf0
	virtual ~RFList();                                        // [1]  scalar deleting destructor 0x4150b0, ~RFList 0x4226e0
	virtual void Create(WndBaseStruct*, RFWnd*);              // [4]  0x422750

protected:
	virtual void PreInitialize();                             // [9]  0x422830
	virtual void OnDestroy();                                 // [11] 0x422890
	virtual void OnLayoutChanged(uint32_t);                   // [14] 0x422950
	virtual void OnLButtonDown(MouseEventArgs&);              // [15] 0x422b30
	virtual void OnRButtonDown(MouseEventArgs&);              // [18] 0x422b70
	virtual void OnMouseMove(MouseEventArgs&);                // [20] 0x422cc0
	virtual void OnMouseEnter();                              // [22] 0x422bc0
	virtual void OnMouseLeave();                              // [23] 0x422c30
	virtual void OnUpdate(UpdateEventArgs&);                  // [31] 0x423180
	virtual void OnDraw();                                    // [32] 0x423700

public:
	virtual void ItemClipEnable(bool);                        // [34] 0x414df0

protected:
	virtual void OnItemAdded();                               // [35] 0x422b10
	virtual void OnItemRemoved();                             // [36] 0x422b20

public:
	RFRect const& GetPadding() const;                         // 0x414d00
	void SetPadding(RFRect const&);                           // 0x414d10
	float GetItemWidth() const;                               // 0x414d40
	float GetItemHeight() const;                              // 0x414d50
	ListSelectionTypes::Type GetSelectionMode() const;        // 0x414d60
	void SetSelectionMode(ListSelectionTypes::Type);          // 0x414d70
	bool IsDeselectable() const;                              // 0x414d80
	void SetDeselectable(bool);                               // 0x414d90
	bool IsReselectable() const;                              // 0x414da0
	void SetReselectable(bool);                               // 0x414db0
	RFRCLICKACTION GetRClickAction() const;                   // 0x414dc0
	void SetRClickAction(RFRCLICKACTION);                     // 0x414dd0
	int32_t GetFirstDrawItemIndex() const;                    // 0x414de0
	int32_t GetSelItemCount() const;                          // 0x415110
	RFListItemBase* GetSelItem(int32_t);                      // 0x415120
	int32_t GetColumnCount() const;                           // 0x415150
	int32_t GetItemCount() const;                             // 0x415160
	RFList();                                                 // 0x422660
	void EnableMultiSelect(bool);                             // 0x422970
	void RelaseAllSelectedItem();                             // 0x422990
	void RelaseAllOverItem();                                 // 0x422ac0
	float GetAllItemsWidth() const;                           // 0x423a70
	RFRect GetItemRect(int32_t, int32_t, bool);               // 0x423bc0
	float GetAllItemsHeight() const;                          // 0x423c80
	OrientationTypes::Type GetOrientation() const;            // 0x423dd0
	void SetOrientation(OrientationTypes::Type);              // 0x423de0
	void InsertItem(void*);                                   // 0x423df0
	RFWnd* InsertWindowItem(std::wstring const&);             // 0x423f70
	RFWnd* InsertWindowItem(RFWnd*);                          // 0x423fa0
	RFWnd* InsertWindowItem(std::wstring const&, uint32_t);   // 0x424040
	RFWnd* InsertWindowItem(RFWnd*, uint32_t);                // 0x424090
	void SetItemData(int32_t, void*);                         // 0x424340
	void* GetItemData(int32_t);                               // 0x4243a0
	RFListItemBase* GetListItem(int32_t);                     // 0x4243f0
	void SelectItem(int32_t, bool);                           // 0x424440
	void SelectItem(void*, bool);                             // 0x424540
	void UnselectItem(int32_t, bool);                         // 0x4246e0
	void UnselectItem(void*, bool);                           // 0x4247d0
	void EnableItem(void*, bool);                             // 0x424920
	RFListItemBase* GetItemUnderCursor();                     // 0x424970
	void DeleteItem(int32_t);                                 // 0x424a60
	void DeleteItem(void*);                                   // 0x424b70
	void DeleteAllItems();                                    // 0x424ca0

protected:
	void ReindexItems();                                      // 0x422d40
	void DeleteFromSelList(RFListItemBase*);                  // 0x422da0
	void ButtonDown_Impl(MouseEventArgs&);                    // 0x422e00
	void MultipleSelectionProc(RFListItemBase*, MouseEventArgs&); // 0x422f50
	void SingleSelectionProc(RFListItemBase*, MouseEventArgs&);   // 0x422f90
	void UpdateSelectList();                                  // 0x423000
	void UpdateItemPosition();                                // 0x4231a0
	void VericalAlignment(RFVector2 const&);                  // 0x423240
	void HorizontalAlignment(RFVector2 const&);               // 0x4234a0
	void OnItemMouseLeave(EventArgs&);                        // 0x423a60
	RFListItemWnd* InsertWindowItem_Impl(RFWnd*);             // 0x424100
	RFListItemBase* GetItemOnPosition_Impl(RFVector2 const&); // 0x4249d0
	bool IsValidIndex(int32_t) const;                         // 0x424db0

	std::vector<RFListItemBase*> m_items;                     // 0x180, GetItemCount
	std::vector<RFListItemBase*> m_selItems;                  // 0x190, GetSelItemCount / GetSelItem
	RFVector2 m_scrollOffset;                                 // 0x1a0, RFListSC's OnH/VScrollPosChanged store -pos
	int32_t m_firstDrawItemIndex;                             // 0x1a8
	OrientationTypes::Type m_orientation;                     // 0x1ac
	bool m_itemClipEnabled;                                   // 0x1b0, ItemClipEnable
	RFRect m_padding;                                         // 0x1b4
	int32_t m_columnCount;                                    // 0x1c4
	RFSize m_itemSize;                                        // 0x1c8, Create: the ListBase item rect's size
	ListSelectionTypes::Type m_selectionMode;                 // 0x1d0
	bool m_deselectable;                                      // 0x1d4
	bool m_reselectable;                                      // 0x1d5
	RFRCLICKACTION m_rClickAction;                            // 0x1d8
};
CHECK_SIZE(RFList, 0x1dc);
BEGIN_LAYOUT_CHECK(RFList)
	CHECK_FIELD(RFList, m_items, 0x180);
	CHECK_FIELD(RFList, m_scrollOffset, 0x1a0);
	CHECK_FIELD(RFList, m_selItems, 0x190);
	CHECK_FIELD(RFList, m_firstDrawItemIndex, 0x1a8);
	CHECK_FIELD(RFList, m_orientation, 0x1ac);
	CHECK_FIELD(RFList, m_itemClipEnabled, 0x1b0);
	CHECK_FIELD(RFList, m_padding, 0x1b4);
	CHECK_FIELD(RFList, m_columnCount, 0x1c4);
	CHECK_FIELD(RFList, m_itemSize, 0x1c8);
	CHECK_FIELD(RFList, m_selectionMode, 0x1d0);
	CHECK_FIELD(RFList, m_deselectable, 0x1d4);
	CHECK_FIELD(RFList, m_reselectable, 0x1d5);
	CHECK_FIELD(RFList, m_rClickAction, 0x1d8);
END_LAYOUT_CHECK

// The header inlines defined past the class: their copies sit in the shared
// header-inline run 0x414cf0-0x415160 (with RFCheck's and RFScrollBar's), not
// in RFList.cpp's run.

// 0x414cf0
inline wchar_t const* RFList::ToString() const {
	return L"RFList";
}

// 0x414d00
inline RFRect const& RFList::GetPadding() const {
	return m_padding;
}

// 0x414d10
inline void RFList::SetPadding(RFRect const& padding) {
	m_padding = padding;
}

// 0x414d40
inline float RFList::GetItemWidth() const {
	return m_itemSize.width;
}

// 0x414d50
inline float RFList::GetItemHeight() const {
	return m_itemSize.height;
}

// 0x414d60
inline ListSelectionTypes::Type RFList::GetSelectionMode() const {
	return m_selectionMode;
}

// 0x414d70
inline void RFList::SetSelectionMode(ListSelectionTypes::Type mode) {
	m_selectionMode = mode;
}

// 0x414d80
inline bool RFList::IsDeselectable() const {
	return m_deselectable;
}

// 0x414d90
inline void RFList::SetDeselectable(bool deselectable) {
	m_deselectable = deselectable;
}

// 0x414da0
inline bool RFList::IsReselectable() const {
	return m_reselectable;
}

// 0x414db0
inline void RFList::SetReselectable(bool reselectable) {
	m_reselectable = reselectable;
}

// 0x414dc0
inline RFRCLICKACTION RFList::GetRClickAction() const {
	return m_rClickAction;
}

// 0x414dd0
inline void RFList::SetRClickAction(RFRCLICKACTION action) {
	m_rClickAction = action;
}

// 0x414de0
inline int32_t RFList::GetFirstDrawItemIndex() const {
	return m_firstDrawItemIndex;
}

// 0x414df0
inline void RFList::ItemClipEnable(bool enable) {
	m_itemClipEnabled = enable;
}

// 0x415110
inline int32_t RFList::GetSelItemCount() const {
	return m_selItems.size();
}

// 0x415120
inline RFListItemBase* RFList::GetSelItem(int32_t index) {
	if (index < 0 || index >= GetSelItemCount()) {
		return 0;
	}
	return m_selItems[index];
}

// 0x415150
inline int32_t RFList::GetColumnCount() const {
	return m_columnCount;
}

// 0x415160
inline int32_t RFList::GetItemCount() const {
	return (int32_t)m_items.size();
}

#endif
