// Original: client\Refresh\include\RFStackPanel.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFSTACKPANEL_H
#define PG_CLIENT_REFRESH_INCLUDE_RFSTACKPANEL_H

#include <stdint.h>
#include <string>

#include "RFExport.h"
#include "RFLabel.h"
#include "RFList.h"
#include "RFWindowFactory.h"
#include "RFWnd.h"
#include "defines.h"
#include "padding.h"

// The layout data of a stack panel, read by RFStackPanel::Create (its RTTI
// name ".?AUStackPanelBase@@", vtable 0xd65a70): the window's WndBaseStruct
// followed by the panel's fields.
struct StackPanelBase : public WndBaseStruct {
	// [1] 0x43c7d0 (the stack panel's WndType)
	virtual WndType GetType() const { return WndType_StackPanel; }

	StackPanelBase();                           // 0x43c570 (RFXmlLoader.cpp)

	RFHAlignProperty m_hAlign;                  // 0x17c "HAlign"
	RFVAlignProperty m_vAlign;                  // 0x1a0 "VAlign"
	RFOrientationProperty m_orientation;        // 0x1c4 "Orientation"
};
CHECK_SIZE(StackPanelBase, 0x1e8);
CHECK_FIELD(StackPanelBase, m_hAlign, 0x17c);
CHECK_FIELD(StackPanelBase, m_vAlign, 0x1a0);
CHECK_FIELD(StackPanelBase, m_orientation, 0x1c4);

// A Refresh stack panel: RFWnd that lays its children out in a stack
// (vtable 0xd640fc, 34 slots, all within RFWnd's range), size 0x184
// (RFStackPanelFactory::Create 0x433770). Exported: its implicit copy
// constructor (0x416a70) and copy assignment (0x416ac0) are exports, so they
// stay implicit.
class RF_API RFStackPanel : public RFWnd {
	LAYOUT_CHECK_FRIEND;

public:
	// The class name, the static const at 0xe47000 that GetTypeName returns.
	static std::wstring const TypeName;

	virtual wchar_t const* ToString() const;                  // [0]  0x416a50
	virtual ~RFStackPanel();                                  // [1]  vector deleting destructor 0x416b30, ~RFStackPanel 0x44e730
	virtual std::wstring const& GetTypeName();                // [2]  0x416a60
	virtual void Initialize();                                // [3]  0x44e740
	virtual void Create(WndBaseStruct*, RFWnd*);              // [4]  0x44e780

	RFStackPanel();                                           //      0x44e710
	OrientationTypes::Type GetOrientation() const;            //      0x44e7c0
	void SetOrientation(OrientationTypes::Type);              //      0x44e7d0
	void StackUp();                                           //      0x44e840

protected:
	virtual void OnLayoutChanged(uint32_t);                   // [14] 0x44e7e0

	void StackUp_Impl();                                      //      0x44e890
	void StackUpVertically();                                 //      0x44e8c0
	void StackUpHorizontally();                               //      0x44eac0

private:
	HorzAlignTypes::Type m_hAlign;          // 0x178 where each child sits across the stack
	VertAlignTypes::Type m_vAlign;          // 0x17c where the stack sits in the panel
	OrientationTypes::Type m_orientation;   // 0x180 0 vertical, 1 horizontal
};
CHECK_SIZE(RFStackPanel, 0x184);
BEGIN_LAYOUT_CHECK(RFStackPanel)
	CHECK_FIELD(RFStackPanel, m_hAlign, 0x178);
	CHECK_FIELD(RFStackPanel, m_vAlign, 0x17c);
	CHECK_FIELD(RFStackPanel, m_orientation, 0x180);
END_LAYOUT_CHECK

// The factory RFManager creates stack panels with (vtable 0xd64fb4: slot 0
// is the implicit scalar deleting destructor at 0x4337f0, emitted where the
// factory is constructed).
class RFStackPanelFactory : public RFWindowFactory {
public:
	RFStackPanelFactory(); // 0x433720 (RFStackPanel.cpp, beside Create)
	virtual RFWnd* Create();                // [1] 0x433770
	virtual void Destroy(RFWnd*);           // [2] 0x4337d0
};

#endif
