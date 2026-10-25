// Original: client\Refresh\include\RFWindowFactory.h (JP 983 exe; path from
// the library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFWINDOWFACTORY_H
#define PG_CLIENT_REFRESH_INCLUDE_RFWINDOWFACTORY_H

#include <string>

#include "RFExport.h"
#include "defines.h"

class RFWnd;

// Window factory: the type name and the Create/Destroy pair; vtable
// 0xd64188, size 0x20 (from the vector deleting destructor 0x4170c0).
// dllexport as in the original library: cl 16 then emits the vector
// deleting destructor ??_E for the vtable's destructor slot. The destructor
// is an inline: the derived factories' scalar deleting destructors
// (RFPictureFactory 0x433260, RFProgressFactory 0x4333b0, RFComboBoxFactory
// 0x432ba0) inline its string tidy, and dllexport still emits the
// out-of-line copy 0x416f80 in RFWindowFactory.cpp.
class RF_API RFWindowFactory {
	LAYOUT_CHECK_FRIEND;

public:
	virtual ~RFWindowFactory() {}        // [0] vector deleting destructor 0x4170c0, ~RFWindowFactory 0x416f80
	virtual RFWnd* Create() = 0;         // [1] _purecall 0xc3c2f8; inferred from the derived overrides (RFButtonFactory 0x432810)
	virtual void Destroy(RFWnd*) = 0;    // [2] _purecall; inferred from RFButtonFactory 0x432880

	std::wstring const& GetTypeName() const; // 0x416fc0

	RFWindowFactory(RFWindowFactory const&); // 0x417010
	RFWindowFactory& operator=(RFWindowFactory const&); // 0x417050

protected:
	RFWindowFactory(std::wstring const&);    // 0x416fd0 (protected: ??0...@@IAE)

public:
	std::wstring m_typeName; // 0x4
};
CHECK_SIZE(RFWindowFactory, 0x20);
BEGIN_LAYOUT_CHECK(RFWindowFactory)
	CHECK_FIELD(RFWindowFactory, m_typeName, 0x4);
END_LAYOUT_CHECK

#endif
