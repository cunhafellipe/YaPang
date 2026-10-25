// Original: client\Refresh\include\RFPoint.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFPOINT_H
#define PG_CLIENT_REFRESH_INCLUDE_RFPOINT_H

#include <stdint.h>

#include "RFExport.h"
#include "defines.h"

// The integer point of the library (the float one is RFVector2): size 0x8,
// no vtable. Length returns the second coordinate minus the first; the
// operators add the two coordinates componentwise. The constructors, set,
// Length and the copy assignment are header inlines (their kept copies lie
// with RFVector2's and RFSize's at 0x401280-0x4013c0); the operators are
// defined in RFPoint.cpp. dllexport as in the original library: cl 16 then
// emits the header inlines into RFPoint.obj, where they pair.
class RF_API RFPoint {
public:
	// 0x4012e0
	RFPoint() : x(0), y(0) {}
	// 0x4012f0
	RFPoint(int32_t x_, int32_t y_) : x(x_), y(y_) {}
	// 0x401310 (exported, so declared in the original, unlike RFSize's)
	RFPoint(RFPoint const& other) : x(other.x), y(other.y) {}

	// 0x401330
	void set(int32_t x_, int32_t y_) {
		x = x_;
		y = y_;
	}
	// 0x401350
	int32_t Length() const { return y - x; }

	// operator= 0x401360 is the implicit copy assignment.

	RFPoint operator+(RFPoint const&) const;  // 0x41dc10
	void operator+=(RFPoint const&);          // 0x41dc30

	int32_t x;  // 0x0
	int32_t y;  // 0x4
};
CHECK_SIZE(RFPoint, 0x8);
CHECK_FIELD(RFPoint, x, 0x0);
CHECK_FIELD(RFPoint, y, 0x4);

#endif
