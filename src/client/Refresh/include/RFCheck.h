// Original: client\Refresh\include\RFCheck.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFCHECK_H
#define PG_CLIENT_REFRESH_INCLUDE_RFCHECK_H

#include <string>

#include "RFBrush.h"
#include "RFExport.h"
#include "RFWindowFactory.h"
#include "RFWnd.h"
#include "defines.h"
#include "padding.h"

// The layout data of a check box, read by RFCheck::Create (its RTTI name
// ".?AUCheckBase@@", vtable 0xd65a10): the window's WndBaseStruct followed
// by the check box's fields.
struct CheckBase : public WndBaseStruct {
	// [0] 0x43a720 (RFXmlLoader.cpp): releases the two image sets
	virtual ~CheckBase();

	// [1] 0x43a850 (the check box's WndType)
	virtual WndType GetType() const { return WndType_Check; }

	CheckBase();                        // 0x43a600 (RFXmlLoader.cpp)

	ImageSet* m_brushDescs[2];          // 0x17c unchecked, checked
	RFStringProperty m_clickSound;      // 0x184 "PushSound"
};
CHECK_SIZE(CheckBase, 0x1c0);
CHECK_FIELD(CheckBase, m_brushDescs, 0x17c);
CHECK_FIELD(CheckBase, m_clickSound, 0x184);

// A Refresh check box: RFWnd with its check state (vtable 0xd6387c, 34 slots,
// no new virtuals), size 0x1a8 (RFCheckFactory::Create 0x432970).
// dllexport as in the original library: cl 16 then emits the vector deleting
// destructor 0x414c50 and the implicit copy constructor 0x414ac0 and copy
// assignment 0x414b80, which the exe exports.
class RF_API RFCheck : public RFWnd {
	LAYOUT_CHECK_FRIEND;

public:
	// The class name, the static const at 0xe46db4 that GetTypeName returns.
	static std::wstring const TypeName;
	// The event OnLButtonUp raises when a click toggles the state, with a
	// CheckChangedEventArgs (the static const at 0xe46d98; its initializer
	// is not identified yet).
	static std::wstring const EventChanged;

	// [0] 0x414aa0, [2] 0x414ab0: header inlines (their copies sit in the
	// COMDAT run of the implicit copy constructor, operator= and ??_E)
	virtual wchar_t const* ToString() const {
		return L"RFCheck";
	}
	virtual ~RFCheck();                                       // [1]  vector deleting destructor 0x414c50, ~RFCheck 0x425ab0
	virtual std::wstring const& GetTypeName() {
		return TypeName;
	}
	virtual void Create(WndBaseStruct*, RFWnd*);              // [4]  0x425b00

	RFCheck();                                                //      0x425a80
	bool GetCheck() const;                                    //      0x425c20
	void SetCheck(bool);                                      //      0x425c30

protected:
	virtual void LoadResource();                              // [8]  0x425bc0
	virtual void PreInitialize();                             // [9]  0x425b50
	virtual void OnDestroy();                                 // [11] 0x425bf0
	virtual void OnLButtonDown(MouseEventArgs&);              // [15] 0x425c40
	virtual void OnLButtonUp(MouseEventArgs&);                // [16] 0x425c90
	virtual void OnDraw();                                    // [32] 0x425d80

private:
	bool m_check;                       // 0x178
	std::wstring m_clickSound;          // 0x17c (OnLButtonUp plays it)
	RFBrush* m_brushes[2];              // 0x198 by m_check, built by LoadResource, freed by OnDestroy
	ImageSet* m_brushDescs[2];          // 0x1a0 by m_check, from CheckBase
};
CHECK_SIZE(RFCheck, 0x1a8);
BEGIN_LAYOUT_CHECK(RFCheck)
	CHECK_FIELD(RFCheck, m_check, 0x178);
	CHECK_FIELD(RFCheck, m_clickSound, 0x17c);
	CHECK_FIELD(RFCheck, m_brushes, 0x198);
	CHECK_FIELD(RFCheck, m_brushDescs, 0x1a0);
END_LAYOUT_CHECK

// The factory RFWindowFactoryManager creates check boxes with (vtable
// 0xd64f14, 3 slots; slot 0, the scalar deleting destructor, is 0x432a20).
class RFCheckFactory : public RFWindowFactory {
public:
	RFCheckFactory(); // 0x432920 (RFControlFactories.cpp)
	virtual RFWnd *Create();             // [1] 0x432970
	virtual void Destroy(RFWnd *);       // [2] 0x432a00
};

#endif
