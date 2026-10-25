// Original: client\Refresh\src\RFPoint.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include "RFPoint.h"

// The constructors, set, Length and operator= (0x4012e0-0x401360) are
// header inlines in RFPoint.h.

// lib: 0x401360 (RFPoint::operator=, the implicit copy assignment)

// 0x41dc10
RFPoint RFPoint::operator+(RFPoint const& other) const {
	return RFPoint(x + other.x, y + other.y);
}

// 0x41dc30
void RFPoint::operator+=(RFPoint const& other) {
	x += other.x;
	y += other.y;
}
