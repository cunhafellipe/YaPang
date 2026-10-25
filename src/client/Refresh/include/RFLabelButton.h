// Original: client\Refresh\include\RFLabelButton.h (JP 983 exe; path from
// the library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFLABELBUTTON_H
#define PG_CLIENT_REFRESH_INCLUDE_RFLABELBUTTON_H

#include <string>

#include "RFButton.h"
#include "RFExport.h"
#include "RFLabel.h"
#include "RFWindowFactory.h"
#include "defines.h"
#include "padding.h"

// The layout data of a label button, read by RFLabelButton::Create
// (0x425fc0; its RTTI name ".?AULabelButtonBase@@", vtable 0xd65a34): the
// button's ButtonBase followed by the child label's own layout data and the
// label colours by state (RFColorProperty members the constructor
// registers).
struct LabelButtonBase : public ButtonBase {
	// [1] 0x43c2e0 (the label button's WndType)
	virtual WndType GetType() const { return WndType_LabelButton; }

	LabelButtonBase();

	WndBaseStruct *m_labelStruct;           // 0x204 the child label's layout data
	bool m_useStateColors;                  // 0x208
	RFColorProperty m_normalColor;          // 0x20c "NormalColor" (0xffffffff)
	RFColorProperty m_overColor;            // 0x230 "HoverColor" (0xffffffff)
	RFColorProperty m_downColor;            // 0x254 "PushColor" (0xffffffff)
	RFColorProperty m_disableColor;         // 0x278 "DisableColor" (0xffffffff)
};
CHECK_SIZE(LabelButtonBase, 0x29c);
CHECK_FIELD(LabelButtonBase, m_labelStruct, 0x204);
CHECK_FIELD(LabelButtonBase, m_useStateColors, 0x208);
CHECK_FIELD(LabelButtonBase, m_normalColor, 0x20c);
CHECK_FIELD(LabelButtonBase, m_overColor, 0x230);
CHECK_FIELD(LabelButtonBase, m_downColor, 0x254);
CHECK_FIELD(LabelButtonBase, m_disableColor, 0x278);

// A Refresh button that draws its label: RFButton plus a child RFLabel whose
// colour follows the button's state (vtable 0xd63acc, 35 slots, no new
// virtuals), size 0x210 (RFLabelButtonFactory::Create 0x432f30).
class RF_API RFLabelButton: public RFButton {
	LAYOUT_CHECK_FRIEND;

public:
	// The class name, the static const at 0xe46dd0 that GetTypeName returns.
	static std::wstring const TypeName;

	virtual wchar_t const* ToString() const;                  // [0]  0x414760
	virtual ~RFLabelButton();                                 // [1]  vector deleting destructor 0x4148a0, ~RFLabelButton 0x425fb0
	virtual std::wstring const& GetTypeName();                // [2]  0x414770
	virtual void Create(WndBaseStruct*, RFWnd*);              // [4]  0x425fc0

	RFLabelButton();                                          //      0x425f50
	void SetLabel(std::wstring const&);                       //      0x426120
	std::wstring const& GetLabel() const;                     //      0x426160

protected:
	virtual void PreInitialize();                             // [9]  0x4260e0
	virtual void OnUpdate(UpdateEventArgs&);                  // [31] 0x426170

private:
	RFLabel *m_label;                       // 0x1f4 the child label Create builds
	RFBUTTON_STATE m_lastState;             // 0x1f8 the state the label's colour was last set for
	bool m_useStateColors;                  // 0x1fc the label's colour follows the state (Create copies it from the WndBaseStruct, +0x208)
	uint32_t m_normalColor;                 // 0x200 label colour by state (WndBaseStruct +0x22c)
	uint32_t m_overColor;                   // 0x204 (+0x250)
	uint32_t m_downColor;                   // 0x208 (+0x274)
	uint32_t m_disableColor;                // 0x20c (+0x298)
};
CHECK_SIZE(RFLabelButton, 0x210);
BEGIN_LAYOUT_CHECK(RFLabelButton)
	CHECK_FIELD(RFLabelButton, m_label, 0x1f4);
	CHECK_FIELD(RFLabelButton, m_lastState, 0x1f8);
	CHECK_FIELD(RFLabelButton, m_useStateColors, 0x1fc);
	CHECK_FIELD(RFLabelButton, m_normalColor, 0x200);
	CHECK_FIELD(RFLabelButton, m_overColor, 0x204);
	CHECK_FIELD(RFLabelButton, m_downColor, 0x208);
	CHECK_FIELD(RFLabelButton, m_disableColor, 0x20c);
END_LAYOUT_CHECK

// The factory RFWindowFactoryManager creates label buttons with (unexported;
// slot 0, the scalar deleting destructor, is 0x432fc0).
class RFLabelButtonFactory : public RFWindowFactory {
public:
	RFLabelButtonFactory(); // 0x432ee0 (RFLabelButton.cpp, beside Create)
	virtual RFWnd *Create();             // [1] 0x432f30
	virtual void Destroy(RFWnd *);       // [2] 0x432fa0
};

#endif
