// Original: client\Refresh\include\RFVector2.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFVECTOR2_H
#define PG_CLIENT_REFRESH_INCLUDE_RFVECTOR2_H

#include <string>

#include "RFExport.h"
#include "defines.h"

// The float point of the library: two floats, size 0x8, 4-aligned (the
// exported ZERO sits at 0xf1ddd4). The constructor from two floats, set and
// the copy assignment are header inlines (their kept copies lie with
// RFPoint's at 0x401280); the rest is defined in RFVector2.cpp.
class RF_API RFVector2 {
public:
	RFVector2();                                       // 0x4289b0
	// 0x401280
	RFVector2(float x_, float y_) : x(x_), y(y_) {}

	// 0x4012a0
	void set(float x_, float y_) {
		x = x_;
		y = y_;
	}
	// operator= 0x4012c0 is the implicit copy assignment.

	RFVector2& operator+=(RFVector2 const&);           // 0x4289c0
	RFVector2& operator-=(RFVector2 const&);           // 0x4289e0
	RFVector2& operator*=(RFVector2 const&);           // 0x428a00
	RFVector2& operator/=(RFVector2 const&);           // 0x428a20
	RFVector2& operator*=(float);                      // 0x428a40
	RFVector2& operator/=(float);                      // 0x428a60
	RFVector2 const operator+(RFVector2 const&) const; // 0x428a90
	RFVector2 const operator-(RFVector2 const&) const; // 0x428ab0
	RFVector2 const operator*(RFVector2 const&) const; // 0x428ad0
	RFVector2 const operator/(RFVector2 const&) const; // 0x428af0
	RFVector2 const operator*(float) const;            // 0x428b10
	RFVector2 const operator/(float) const;            // 0x428b30
	bool operator==(RFVector2 const&) const;           // 0x428b60
	bool operator!=(RFVector2 const&) const;           // 0x428b90
	static RFVector2 FromString(std::string const&);   // 0x428bc0

	static RFVector2 const ZERO;                       // 0xf1ddd4

	float x;    // 0x0
	float y;    // 0x4
};
CHECK_SIZE(RFVector2, 0x8);
CHECK_FIELD(RFVector2, x, 0x0);
CHECK_FIELD(RFVector2, y, 0x4);

#endif
