// Original: client\Refresh\include\RFRect.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFRECT_H
#define PG_CLIENT_REFRESH_INCLUDE_RFRECT_H

#include "RFExport.h"
#include "RFVector2.h"
#include "defines.h"

// Axis-aligned float rectangle, 0x10. PtInRect (0x41dd10) tests
// left <= x <= right and top <= y <= bottom (edges inclusive); Width/Height (0x4014d0, 0x4014f0)
// subtract the pairs; the 4-float constructor (0x4013e0) takes
// (left, top, right, bottom).
// dllexport as in the original library: cl 16 then emits the header inlines
// (and the implicit copy assignment) into RFRect.obj, where they pair.
class RF_API RFRect {
public:
	// 0x4013d0
	RFRect() : left(0.0f), right(0.0f), top(0.0f), bottom(0.0f) {}
	// 0x4013e0
	RFRect(float left_, float top_, float right_, float bottom_)
		: left(left_), right(right_), top(top_), bottom(bottom_) {}
	// 0x401400
	RFRect(RFRect const& other)
		: left(other.left), right(other.right), top(other.top), bottom(other.bottom) {}

	// 0x401430
	RFVector2 LT() const { return RFVector2(left, top); }
	// 0x401450
	RFVector2 RB() const { return RFVector2(right, bottom); }
	// 0x401470
	RFVector2 XY() const { return RFVector2(left, top); }
	// 0x401490
	RFVector2 WH() const { return RFVector2(right - left, bottom - top); }
	// 0x4014b0
	void set(float left_, float top_, float right_, float bottom_) {
		left = left_;
		right = right_;
		top = top_;
		bottom = bottom_;
	}
	// 0x4014d0
	float Width() const { return right - left; }
	// 0x4014f0
	float Height() const { return bottom - top; }
	// operator= (0x401510, ??4RFRect@@QAEAAV0@ABV0@@Z) is the implicit one:
	// four dword moves, inlined so by its callers (RFTextArea::SetPadding).

	bool PtInRect(float, float) const;                                 // 0x41dd10
	bool PtInRect(RFVector2 const&) const;                             // 0x41dd60
	bool IntersectRect(RFRect const&, RFRect const&);                  // 0x41ddc0
	RFRect MoveRect(float, float) const;                               // 0x41de80
	void ApplyPadding(RFRect const&);                                  // 0x41dec0
	RFRect InflateRect(float, float);                                  // 0x41def0
	RFRect DeflateRect(float, float);                                  // 0x41df30
	void operator+=(RFRect const&);                                    // 0x41df70
	void operator-=(RFRect const&);                                    // 0x41dfa0
	void operator*=(float);                                            // 0x41dfd0
	RFRect operator+(RFRect const&) const;                             // 0x41e000
	RFRect operator-(RFRect const&) const;                             // 0x41e030
	RFRect operator*(RFRect const&) const;                             // 0x41e060
	RFRect operator/(RFRect const&) const;                             // 0x41e090
	static RFRect const ZERO;                                          // 0xf1ddc4

	float left;    // 0x0
	float right;   // 0x4
	float top;     // 0x8
	float bottom;  // 0xc
};
CHECK_SIZE(RFRect, 0x10);
CHECK_FIELD(RFRect, left, 0x0);
CHECK_FIELD(RFRect, right, 0x4);
CHECK_FIELD(RFRect, top, 0x8);
CHECK_FIELD(RFRect, bottom, 0xc);

#endif
