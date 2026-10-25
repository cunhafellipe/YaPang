// Original: client\Refresh\include\RFObject.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFOBJECT_H
#define PG_CLIENT_REFRESH_INCLUDE_RFOBJECT_H

#include "RFExport.h"
#include "defines.h"

// The base Refresh object: vtable 0xd63048, 2 slots, size 0x4 (the ctors
// write only the vptr and the copy ctor copies nothing: no members).
// Every member is a header inline: their copies sit at 0x401540-0x401580
// among the other header inlines (RFVector2, RFRect), and the derived
// classes inline them (~RFEventHandler stores RFObject's vptr itself; the
// implicit copy ctors / operator= of RFDirector, RFManager, RFWnd ... call
// no ??0RFObject / ??4RFObject).
// dllexport as in the original library: cl 16 then emits the inlines and
// the vector deleting destructor ??_E (the vtable's destructor slot) into
// RFObject.obj, where they pair.
class RF_API RFObject {
	LAYOUT_CHECK_FRIEND;

public:
	// [0] 0x401540
	virtual wchar_t const* ToString() const { return L"RFObject"; }
	// [1] 0x401550 (vector deleting destructor 0x4015c0)
	virtual ~RFObject() {}

	// 0x401560
	RFObject() {}
	// 0x401570
	RFObject(RFObject const&) {}
	// 0x401580
	RFObject& operator=(RFObject const&) { return *this; }
};
CHECK_SIZE(RFObject, 0x4);

#endif
