// Original: client\Refresh\include\RFButton.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFBUTTON_H
#define PG_CLIENT_REFRESH_INCLUDE_RFBUTTON_H

#include <string>

#include "RFBrush.h"
#include "RFExport.h"
#include "RFWindowFactory.h"
#include "RFWnd.h"
#include "defines.h"
#include "padding.h"

// The layout data of a button, read by RFButton::Create (its RTTI name
// ".?AUButtonBase@@", vtable 0xd65a40): the window's WndBaseStruct followed
// by the per-state image sets and the sounds (LabelButtonBase and
// AnimButtonBase continue it, +0x204 ...).
struct ButtonBase : public WndBaseStruct {
	// [0] 0x43bd90 (RFXmlLoader.cpp): releases the state images
	virtual ~ButtonBase();

	// [1] 0x43bf80 (the button's WndType)
	virtual WndType GetType() const { return WndType_Button; }

	ButtonBase();

	ImageSet *m_stateImages[4];             // 0x17c by RFBUTTON_STATE
	std::wstring m_pushSound;               // 0x18c
	std::wstring m_overSound;               // 0x1a8
	std::wstring m_releaseSound;            // 0x1c4
	RFBooleanProperty m_defaultHandler;     // 0x1e0 "IsDefaultButton"
};
CHECK_SIZE(ButtonBase, 0x204);
CHECK_FIELD(ButtonBase, m_stateImages, 0x17c);
CHECK_FIELD(ButtonBase, m_pushSound, 0x18c);
CHECK_FIELD(ButtonBase, m_overSound, 0x1a8);
CHECK_FIELD(ButtonBase, m_releaseSound, 0x1c4);
CHECK_FIELD(ButtonBase, m_defaultHandler, 0x1e0);

// A Refresh button: RFWnd plus its own state handler (vtable 0xd62f1c,
// 35 slots), size 0x1f4 (RFButtonFactory::Create 0x432810).
class RF_API RFButton : public RFWnd {
	LAYOUT_CHECK_FRIEND;

public:
	// The button's state; also the index of its brush (m_stateBrushes).
	// Values from use: OnLButtonDown sets 1, MouseEnter_Impl 2, OnEnabled
	// 0 or 3.
	enum RFBUTTON_STATE {
		RFBUTTON_STATE_NORMAL = 0,
		RFBUTTON_STATE_DOWN = 1,
		RFBUTTON_STATE_OVER = 2,
		RFBUTTON_STATE_DISABLE = 3
	};

	// The event raised when the button is clicked (0xe46cb8).
	static std::wstring const EventClicked;
	// The class name, the static const at 0xe46cd4 that GetTypeName returns.
	static std::wstring const TypeName;

	// The header inlines (their copies sit in the COMDAT run of the implicit
	// copy constructor, operator= and ??_E, 0x402a80-0x402e30).
	virtual wchar_t const* ToString() const { return L"RFButton"; }     // [0]  0x402a80
	virtual ~RFButton();                                      // [1]  scalar deleting destructor 0x402d40, ~RFButton 0x420e70
	virtual std::wstring const& GetTypeName() { return TypeName; }    // [2]  0x402a90
	virtual void Create(WndBaseStruct*, RFWnd*);              // [4]  0x420f30

	RFButton();                                               //      0x420e10
	RFBUTTON_STATE GetState() const { return m_state; }       //      0x402aa0
	void SetState(RFBUTTON_STATE state) {                     //      0x402da0
		if (state == RFBUTTON_STATE_DISABLE) {
			EnableWindow(false);
		} else {
			m_state = state;
			m_pushed = state == RFBUTTON_STATE_DOWN;
			OnStateChanged();
		}
	}
	std::wstring const& GetPushSound() const { return m_pushSound; }        //      0x402e00
	std::wstring const& GetOverSound() const { return m_overSound; }        //      0x402e10
	std::wstring const& GetReleaseSound() const { return m_releaseSound; }  //      0x402e20
	void ButtonReset();                                       //      0x421240
	void ChangeNormalButtonImage(std::wstring);               //      0x4218f0
	void ChangeOverButtonImage(std::wstring);                 //      0x421990
	void ChangeDownButtonImage(std::wstring);                 //      0x421a30
	void ChangeDisableButtonImage(std::wstring);              //      0x421ad0

protected:
	virtual void LoadResource();                              // [8]  0x4211c0
	virtual void PreInitialize();                             // [9]  0x420fd0
	virtual void OnDestroy();                                 // [11] 0x421210
	virtual void OnEnabled(bool);                             // [12] 0x421420
	virtual void OnShow(bool);                                // [13] 0x421440
	virtual void OnLButtonDown(MouseEventArgs&);              // [15] 0x4214d0
	virtual void OnLButtonUp(MouseEventArgs&);                // [16] 0x421540
	virtual void OnMouseEnter();                              // [22] 0x421690
	virtual void OnMouseLeave();                              // [23] 0x421710
	virtual void OnDraw();                                    // [32] 0x421260
	virtual void OnStateChanged() {}                          // [34] 0x402ab0
	void MouseEnter_Impl();                                   //      0x4217c0
	void MouseLeave_Impl();                                   //      0x421820
	// Raises EventClicked on the Enter key (PreInitialize subscribes it to
	// the overlapped window's EventKeyUp when m_defaultHandler is set).
	void DefaultButtonEventHandler(EventArgs&);               //      0x421870

protected:
	RFBUTTON_STATE m_state;                 // 0x178
	bool m_pushed;                          // 0x17c (SetState(DOWN) sets it)
	bool m_mouseOver;                       // 0x17d (MouseEnter_Impl/MouseLeave_Impl)
	bool m_defaultHandler;                  // 0x17e PreInitialize subscribes DefaultButtonEventHandler when set (Create copies it from the WndBaseStruct, +0x200)
	std::wstring m_pushSound;               // 0x180
	std::wstring m_overSound;               // 0x19c
	std::wstring m_releaseSound;            // 0x1b8
	RFBrush *m_stateBrushes[4];             // 0x1d4 by state, built by LoadResource, freed by OnDestroy
	ImageSet *m_stateImages[4];             // 0x1e4 by state, what Change*ButtonImage sets
};
CHECK_SIZE(RFButton, 0x1f4);
BEGIN_LAYOUT_CHECK(RFButton)
	CHECK_FIELD(RFButton, m_state, 0x178);
	CHECK_FIELD(RFButton, m_pushed, 0x17c);
	CHECK_FIELD(RFButton, m_mouseOver, 0x17d);
	CHECK_FIELD(RFButton, m_defaultHandler, 0x17e);
	CHECK_FIELD(RFButton, m_pushSound, 0x180);
	CHECK_FIELD(RFButton, m_overSound, 0x19c);
	CHECK_FIELD(RFButton, m_releaseSound, 0x1b8);
	CHECK_FIELD(RFButton, m_stateBrushes, 0x1d4);
	CHECK_FIELD(RFButton, m_stateImages, 0x1e4);
END_LAYOUT_CHECK

// The factory RFWindowFactoryManager creates buttons with (vtable 0xd64f04,
// 3 slots; slot 0, the scalar deleting destructor, is 0x4328a0).
class RFButtonFactory : public RFWindowFactory {
public:
	RFButtonFactory(); // 0x4327c0 (RFControlFactories.cpp)
	virtual RFWnd *Create();             // [1] 0x432810
	virtual void Destroy(RFWnd *);       // [2] 0x432880
};

#endif
