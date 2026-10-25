// Original: client\Refresh\include\RFSize.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFSIZE_H
#define PG_CLIENT_REFRESH_INCLUDE_RFSIZE_H

#include <string>

#include "RFExport.h"
#include "defines.h"

// Width and height as two floats, size 0x8, no vtable. The constructors and
// the copy assignment are header inlines (their kept copies lie with
// RFPoint's and RFVector2's at 0x401280-0x4013c0); the rest is defined in
// RFSize.cpp. dllexport as in the original library: cl 16 then emits the
// header inlines into RFSize.obj, where they pair.
class RF_API RFSize {
public:
	// 0x401380
	RFSize() : width(0.0f), height(0.0f) {}
	// 0x401390
	RFSize(float width_, float height_) : width(width_), height(height_) {}

	// operator= 0x4013b0 is the implicit copy assignment.

	bool operator==(RFSize const&) const;     // 0x41dc50
	bool operator!=(RFSize const&) const;     // 0x41dc80
	RFSize operator-(RFSize const&) const;    // 0x41dcb0
	static RFSize FromString(std::string const&);  // 0x41dcd0

	float width;   // 0x0
	float height;  // 0x4
};
CHECK_SIZE(RFSize, 0x8);
CHECK_FIELD(RFSize, width, 0x0);
CHECK_FIELD(RFSize, height, 0x4);

#endif
