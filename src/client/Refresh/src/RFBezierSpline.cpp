// Original: client\Refresh\src\RFBezierSpline.cpp (JP 983 exe; VS 2010 SP1)
#include <algorithm>

#include "RFBezierSpline.h"

// lib: 0x4039e0 (vector deleting destructor, slot 0; paired as ??_E for the
// score), 0x409720 / 0x409810 (std::vector<RFVector2> copy constructor and
// assignment, called by the implicit copy members 0x4038c0 / 0x403950),
// 0x42f3d0 (std::vector<RFVector2>::reserve, from push_back), the scalar
// deleting destructor 0x4039b0 (paired as ??_G); std::vector<RFVector2>
// internals 0x42f2c0 (_Vector_val ctor), 0x42f2d0 (operator[]), 0x42f2e0
// (push_back), 0x42f340 (_Inside), 0x42f3b0, 0x42f3c0, 0x42f4e0 (empty),
// 0x42f4a0 (_Grow_to), 0x42f4f0 / 0x42f510 (forwarders to 0x42f540),
// 0x42f530; std::max<int> 0x42f4d0 (GetValue's max(1, mid))

// 0x42ee70
RFBezierSpline::RFBezierSpline(int pointCount, RFVector2 const& control1, RFVector2 const& control2)
    : m_control1(control1), m_control2(control2), m_pointCount(pointCount) {
	m_start.set(0.0f, 0.0f);
	m_end.set(1.0f, 1.0f);
	GenerateBezierPoints(m_start, m_control1, m_control2, m_end);
}

// 0x42ef20
RFBezierSpline::~RFBezierSpline() {
	m_points.clear();
}

// 0x42ef80
float RFBezierSpline::GetValue(float t) const {
	int high = m_points.size() - 1;
	int low = 0;
	while (low <= high) {
		int middle = (low + high) / 2;
		RFVector2 previous = m_points[std::max(1, middle) - 1];
		RFVector2 current = m_points[middle];
		if (previous.x <= t && t <= current.x) {
			return current.y;
		}
		if (t < current.x) {
			high = middle - 1;
		} else {
			low = middle + 1;
		}
	}
	return m_points[m_points.size() - 1].y;
}

// 0x42f050
void RFBezierSpline::GenerateBezierPoints(RFVector2 const& p0, RFVector2 const& p1, RFVector2 const& p2,
                                          RFVector2 const& p3) {
	m_points.clear();
	for (int i = 0; i < m_pointCount; ++i) {
		RFVector2 point = GetBezierPoint((float)i / m_pointCount, p0, p1, p2, p3);
		m_points.push_back(point);
	}
}

// 0x42f1e0
RFVector2 RFBezierSpline::GetBezierPoint(float t, RFVector2 const& p0, RFVector2 const& p1,
                                         RFVector2 const& p2, RFVector2 const& p3) const {
	return p0 * ((1 - t) * (1 - t) * (1 - t)) + p1 * (float)(3.0 * t * (1 - t) * (1 - t)) +
	       p2 * (float)(3.0 * t * t * (1 - t)) + p3 * (t * t * t);
}
