// Original: client\Refresh\include\RFComboBox.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFCOMBOBOX_H
#define PG_CLIENT_REFRESH_INCLUDE_RFCOMBOBOX_H

#include "RFExport.h"
#include "RFWnd.h"
#include "RFWindowFactory.h"
#include "defines.h"
#include "padding.h"

class RFButton;
class RFListSC;
class RFTextArea;

// The layout data of a combo box, read by RFComboBox::Create (its RTTI name
// ".?AUComboBase@@", vtable 0xd65aac): the window's WndBaseStruct, the layout
// data of its three parts and the item text style.
struct ComboBase : public WndBaseStruct {
	// [1] 0x43d840 (the combo box's WndType)
	virtual WndType GetType() const { return WndType_Combo; }

	ComboBase();                            // 0x43d510 (RFXmlLoader.cpp)

	WndBaseStruct* m_button;                // 0x17c (RFComboBox::m_button)
	WndBaseStruct* m_textArea;              // 0x180 (RFComboBox::m_textArea)
	WndBaseStruct* m_listArea;              // 0x184 (RFComboBox::m_listArea)
	RFColorProperty m_itemTextColor;        // 0x188 "NormalColor" (0xff0000ff)
	RFColorProperty m_itemSelTextColor;     // 0x1ac "SelectColor" (0xffff0000)
	RFStringProperty m_itemFontName;        // 0x1d0 "FontName"
	RFIntegerProperty m_itemFontSize;       // 0x20c "FontSize"
};
CHECK_SIZE(ComboBase, 0x230);
CHECK_FIELD(ComboBase, m_button, 0x17c);
CHECK_FIELD(ComboBase, m_textArea, 0x180);
CHECK_FIELD(ComboBase, m_listArea, 0x184);
CHECK_FIELD(ComboBase, m_itemTextColor, 0x188);
CHECK_FIELD(ComboBase, m_itemSelTextColor, 0x1ac);
CHECK_FIELD(ComboBase, m_itemFontName, 0x1d0);
CHECK_FIELD(ComboBase, m_itemFontSize, 0x20c);

// Drop-down combo box: vtable 0xd6390c, 39 slots (0–33 as RFWnd, the combo
// handlers 34–38 are new), size 0x1b8 (from its vector deleting destructor).
// dllexport as in the original library: cl 16 then emits the implicit copy
// constructor (0x416580) and copy assignment (0x416670).
class RF_API RFComboBox : public RFWnd {
	LAYOUT_CHECK_FRIEND;

public:
	// The class name, the static const at 0xe4708c that GetTypeName returns.
	static std::wstring const TypeName;
	// Raised by OnListSelChanged when the list selects an item (0xe47070).
	static std::wstring const EventSelChanged;

	// [0], [2] and the two accessors are header inlines: their copies sit in
	// the COMDAT run of the implicit copy constructor, operator= and ??_E.
	virtual wchar_t const* ToString() const { return L"RFComboBox"; } // [0] 0x416540
	virtual ~RFComboBox();                                    // [1]  vector deleting destructor 0x416770, ~RFComboBox 0x454980
	virtual std::wstring const& GetTypeName() { return TypeName; } // [2] 0x416550
	void EnableMWheelSelect(bool enable) { m_mwheelSelect = enable; } // 0x416560
	RFListSC* GetListArea() const { return m_listArea; }      // 0x416570
	RFComboBox();                                             //      0x454950
	virtual void Create(WndBaseStruct*, RFWnd*);              // [4]  0x4549d0
	virtual void Destroy();                                   // [5]  0x454ca0
	virtual bool HitTest(RFVector2 const&, bool);             // [6]  0x4551a0
	void AddText(std::wstring const&);                        //      0x455220
	void SetCurSel(int32_t);                                  //      0x4552b0
	int32_t GetCurSel() const;                                //      0x455360
	std::wstring const& GetText(int32_t) const;               //      0x455370
	std::wstring const& GetCurText() const;                   //      0x4553f0
	int32_t GetItemCount() const;                             //      0x455460
	void DeleteAllItems();                                    //      0x455480
	bool IsValidItemidx(int32_t) const;                       //      0x455570

protected:
	virtual void PreInitialize();                             // [9]  0x454cb0
	virtual void OnShow(bool);                                // [13] 0x455080
	void ShowListArea(bool);                                  //      0x455140
	virtual void OnMouseWheel(MouseEventArgs&);               // [21] 0x4555a0
	virtual void OnKillFocus();                               // [30] 0x455680
	virtual void OnCBButClicked(EventArgs&);                  // [34] 0x4556c0
	virtual void OnListSelChanged(EventArgs&);                // [35] 0x455710
	virtual void OnListBeginDrawItem(EventArgs&);             // [36] 0x455880
	virtual void OnListDrawItem(EventArgs&);                  // [37] 0x455890
	virtual void OnListEndDrawItem(EventArgs&);               // [38] 0x455b60

private:
	// Create builds the three parts from the layout's child structs and
	// copies the item text style (0x190–0x1b4) that OnListDrawItem uses.
	RFButton* m_button;                     // 0x178 (EventClicked -> OnCBButClicked)
	RFTextArea* m_textArea;                 // 0x17c (EventLButtonUp -> OnCBButClicked)
	RFListSC* m_listArea;                   // 0x180 (GetListArea)
	bool m_listShown;                       // 0x184 (ShowListArea)
	int32_t m_curSel;                       // 0x188 (GetCurSel; -1 after PreInitialize)
	bool m_mwheelSelect;                    // 0x18c (EnableMWheelSelect)
	uint32_t m_itemTextColor;               // 0x190 (an item drawn normally)
	uint32_t m_itemSelTextColor;            // 0x194 (an item selected or under the cursor)
	std::wstring m_itemFontName;            // 0x198
	int32_t m_itemFontSize;                 // 0x1b4
};
CHECK_SIZE(RFComboBox, 0x1b8);
BEGIN_LAYOUT_CHECK(RFComboBox)
	CHECK_FIELD(RFComboBox, m_button, 0x178);
	CHECK_FIELD(RFComboBox, m_textArea, 0x17c);
	CHECK_FIELD(RFComboBox, m_listArea, 0x180);
	CHECK_FIELD(RFComboBox, m_listShown, 0x184);
	CHECK_FIELD(RFComboBox, m_curSel, 0x188);
	CHECK_FIELD(RFComboBox, m_mwheelSelect, 0x18c);
	CHECK_FIELD(RFComboBox, m_itemTextColor, 0x190);
	CHECK_FIELD(RFComboBox, m_itemSelTextColor, 0x194);
	CHECK_FIELD(RFComboBox, m_itemFontName, 0x198);
	CHECK_FIELD(RFComboBox, m_itemFontSize, 0x1b4);
END_LAYOUT_CHECK

// The factory RFWindowFactoryManager maps the combo box's layout type to:
// vtable 0xd64f24 (slot 0, the implicit scalar deleting destructor 0x432ba0,
// is emitted where the factory is constructed).
class RFComboBoxFactory : public RFWindowFactory {
public:
	RFComboBoxFactory(); // 0x432aa0 (RFControlFactories.cpp)
	virtual RFWnd *Create();             // [1] 0x432af0
	virtual void Destroy(RFWnd *);       // [2] 0x432b80
};

#endif
