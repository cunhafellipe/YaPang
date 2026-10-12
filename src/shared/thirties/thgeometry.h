// Original: d:\build\custom\temp\code\shared\thirties\thgeometry.h
#ifndef PG_SHARED_THIRTIES_THGEOMETRY_H
#define PG_SHARED_THIRTIES_THGEOMETRY_H

#include "defines.h"
#include "thirties/gamath.h"

// A bounding sphere (ThSphere::Set stores the centre then the radius). Only
// the members the decompiled units use are transcribed.
struct ThSphere {
	// thgeometry.h:132
	void Set(const gaV3 &c, float r) {
		Set(c.x, c.y, c.z, r);
	}

	// thgeometry.h:137
	void Set(float x, float y, float z, float r) {
		center.x = x; center.y = y; center.z = z;
		radius = r;
	}

	// thgeometry.h:143
	void SetPos(float x, float y, float z) {
		center.x = x;
		center.y = y;
		center.z = z;
	}

	// thgeometry.h:150
	bool TestIntersection(const ThSphere &other) const {
		gaV3 d = center - other.center;
		float sumRadius = other.radius + radius;

		return d.SquaredLength() <= sumRadius * sumRadius;
	}

	gaV3 center;    // 0x0
	float radius;   // 0xc
};

CHECK_SIZE(ThSphere, 0x10);

#endif // PG_SHARED_THIRTIES_THGEOMETRY_H
