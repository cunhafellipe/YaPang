// Original: client\Refresh\include\RFBezierSpline.h (JP 983 exe; VS 2010 SP1)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFBEZIERSPLINE_H
#define PG_CLIENT_REFRESH_INCLUDE_RFBEZIERSPLINE_H

#include <vector>

#include "defines.h"
#include "RFExport.h"
#include "RFInterpolation.h"
#include "RFVector2.h"

// A cubic Bezier easing curve from (0, 0) to (1, 1) with two control points,
// sampled into m_pointCount points; GetValue looks the argument up among the
// samples' x and answers the sample's y. Vtable 0xd630e4, 2 slots, size 0x38
// (the vector deleting destructor 0x4039e0 passes the element size 0x38).
// The copy constructor (0x4038c0) and the copy assignment (0x403950) are the
// implicit ones, exported with the class.
class RF_API RFBezierSpline: public RFInterpolation {
	LAYOUT_CHECK_FRIEND;

public:
	RFBezierSpline(int pointCount, RFVector2 const& control1, RFVector2 const& control2);  // 0x42ee70

	virtual ~RFBezierSpline();             // [0] vector deleting destructor 0x4039e0, ~RFBezierSpline 0x42ef20
	virtual float GetValue(float t) const; // [1] 0x42ef80

private:
	void GenerateBezierPoints(RFVector2 const& p0, RFVector2 const& p1, RFVector2 const& p2,
	                          RFVector2 const& p3);  // 0x42f050
	RFVector2 GetBezierPoint(float t, RFVector2 const& p0, RFVector2 const& p1, RFVector2 const& p2,
	                         RFVector2 const& p3) const;  // 0x42f1e0

	RFVector2 m_start;                 // 0x4
	RFVector2 m_control1;              // 0xc
	RFVector2 m_control2;              // 0x14
	RFVector2 m_end;                   // 0x1c
	int m_pointCount;                  // 0x24
	std::vector<RFVector2> m_points;   // 0x28
};
CHECK_SIZE(RFBezierSpline, 0x38);
BEGIN_LAYOUT_CHECK(RFBezierSpline)
	CHECK_FIELD(RFBezierSpline, m_start, 0x4);
	CHECK_FIELD(RFBezierSpline, m_control1, 0xc);
	CHECK_FIELD(RFBezierSpline, m_control2, 0x14);
	CHECK_FIELD(RFBezierSpline, m_end, 0x1c);
	CHECK_FIELD(RFBezierSpline, m_pointCount, 0x24);
	CHECK_FIELD(RFBezierSpline, m_points, 0x28);
END_LAYOUT_CHECK

#endif
