// Original: client\Refresh\include\RFWndEx.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFWNDEX_H
#define PG_CLIENT_REFRESH_INCLUDE_RFWNDEX_H

#include <string>

#include "RFBrush.h"
#include "RFExport.h"
#include "RFWindowFactory.h"
#include "RFWnd.h"
#include "defines.h"
#include "padding.h"

// The layout data of an RFWndEx (RTTI ".?AUWndExBase@@", vtable 0xd65a04):
// the window's WndBaseStruct followed by the frame's image set. Its
// destructor (scalar deleting 0x43aa50) releases m_brushDesc's reference.
// The constructor is inlined into its only users (RFXmlLoader's new
// WndExBase, 0x43f8c0 / 0x43f925); the destructor has no copy of its own,
// only the scalar deleting destructor in the loader's run.
struct WndExBase : public WndBaseStruct {
	WndExBase() : m_brushDesc(0) {}
	virtual ~WndExBase() {              // [0] scalar deleting destructor 0x43aa50
		if (m_brushDesc) {
			if (m_brushDesc->Release() == 0)
				delete m_brushDesc;
			m_brushDesc = 0;
		}
	}
	virtual WndType GetType() const { return WndType_WndEx; } // [1] 0x43aa40

	ImageSet* m_brushDesc;              // 0x17c
};
CHECK_FIELD(WndExBase, m_brushDesc, 0x17c);
CHECK_SIZE(WndExBase, 0x180);

// RFWnd with a frame brush: vtable 0xd6403c, 34 slots (no new virtuals),
// size 0x180 (RFWndExFactory::Create 0x433a60); the base of RFEdit, RFList
// and RFTextArea. dllexport as in the original library: cl 16 then emits
// the vector deleting destructor ??_E for the vtable's destructor slot.
class RF_API RFWndEx: public RFWnd {
	LAYOUT_CHECK_FRIEND;

public:
	// The class name, the static const at 0xe46c64 that GetTypeName returns.
	static std::wstring const TypeName;

	// [0] 0x4141b0, [2] 0x4141c0: header inlines (their copies sit in the
	// COMDAT run of the implicit copy constructor, operator= and ??_E)
	virtual wchar_t const* ToString() const {
		return L"RFWndEx";
	}
	// [1] vector deleting destructor 0x414270
	virtual ~RFWndEx();                                       // 0x41d9a0
	virtual std::wstring const& GetTypeName() {
		return TypeName;
	}
	virtual void Create(WndBaseStruct*, RFWnd*);              // [4]  0x41d9b0

	RFWndEx();                                                // 0x41d980

protected:
	virtual void LoadResource();                              // [8]  0x41da40
	virtual void PreInitialize();                             // [9]  0x41d9e0

public:
	virtual void OnDestroy();                                 // [11] 0x41da60
	virtual void OnDraw();                                    // [32] 0x41da90

protected:
	RFBrush* m_brush;                       // 0x178 the frame (LoadResource builds it, OnDestroy deletes it)
	ImageSet* m_brushDesc;                  // 0x17c (Create copies it from the WndExBase)
};
CHECK_SIZE(RFWndEx, 0x180);
BEGIN_LAYOUT_CHECK(RFWndEx)
	CHECK_FIELD(RFWndEx, m_brush, 0x178);
	CHECK_FIELD(RFWndEx, m_brushDesc, 0x17c);
END_LAYOUT_CHECK

// The window factory of the "RFWndEx" type (vtable 0xd64fd4; slot 0, the
// scalar deleting destructor, is 0x433b10).
class RFWndExFactory : public RFWindowFactory {
public:
	RFWndExFactory(); // 0x433a10 (RFWndEx.cpp, beside Create)
	virtual RFWnd* Create();             // [1] 0x433a60
	virtual void Destroy(RFWnd*);        // [2] 0x433af0
};

#endif
