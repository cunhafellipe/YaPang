// Original: d:\build\custom\temp\code\client\projectg\autotarget.cpp
#include "autotarget.h"
#include "golfball.h"
#include "quadtree.h"
#include "club.h"
#include "SharedDoc.h"
#include "GolfDoc.h"

#include <math.h>

// Nameless .rdata epsilon SolveEquation compares against (Wangreal math's
// 1e-5f, read through memory).
extern "C" {
static float data_9eb410 = 1e-5f;
}

// autotarget.cpp:20
CAutoTarget::~CAutoTarget() {
	m_pointList.clear();
}

// autotarget.cpp:25
void CAutoTarget::Reset(void) {
	m_pointList.clear();
}

// autotarget.cpp:30
float CAutoTarget::GetAngle(void) {
	return CalcDeltaAngle(GetPoint(0) - GolfBall().m_pos, WVector::UNIT_POS_Z);
}

// autotarget.cpp:35
float CAutoTarget::InArea(const WVector &a, const WVector &b, float radius) const {
	return sqrtf((a.x - b.x) * (a.x - b.x) + (a.z - b.z) * (a.z - b.z)) - radius;
}

// autotarget.cpp:40
// Where to drop the ball: 3 on a ground type below 3 and 4 out of bounds
// (both back to the last shot position), otherwise the first ground of type
// 10 (2, back to the last shot position) or 1 (1, on that ground) stepping
// from pos towards the nearest point of the list; 0 when none is found.
int CAutoTarget::CalcDropPos(const WVector &pos) {
	if (GetPVS().GetGroundType(pos, true, 0) < 3) {
		m_dropPos = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_lastShotPos;
		return 3;
	}

	if (GOLFDOC()->m_pPolySoup->IsOutOfBound(pos)) {
		m_dropPos = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_lastShotPos;
		return 4;
	}

	float minDist = 3200.0f;
	WVector dir;

	for (unsigned char i = 0; i < m_pointList.size() - 1; i++) {
		WVector nearest;
		nearest = GetNearest(i);
		float dist = (nearest - pos).Magnitude();
		if (dist < minDist) {
			minDist = dist;
			dir = nearest - pos;
			dir.y = 0.0f;
		}
	}

	float step = 32.0f;
	dir.Normalize();

	do {
		unsigned char type = GetPVS().GetGroundType(pos + dir * step, true, 0);

		if (type == 10) {
			m_dropPos = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_lastShotPos;
			return 2;
		}

		if (type == 1) {
			m_dropPos = GetPVS().GetGroundPoint(pos + dir * step, false, 0, false);
			m_dropPos.y += GolfBall().GetRadius();
			return 1;
		}
	} while ((step += 32.0f) < 192.0f);

	m_dropPos = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_lastShotPos;
	return 0;
}

// autotarget.cpp:107
// The point of segment [index, index + 1] nearest to the ball on ground type 1,
// sampled every 16 units.
WVector CAutoTarget::GetNearest(unsigned char index) {
	float t = 0.0f;
	float nearest = 1.0f;
	float minDist = 3200.0f;
	float step = 1.0f / ceil((m_pointList[index + 1] - m_pointList[index]).Magnitude() * 0.0625f);
	WVector pos;

	for (; t < 1.0f; t += step) {
		pos = (1.0f - t) * m_pointList[index] + t * m_pointList[index + 1];

		if (GetPVS().GetGroundType(pos, true, 0) == 1) {
			float dist = (GolfBall().m_pos - pos).Magnitude();
			if (dist < minDist) {
				minDist = dist;
				nearest = t;
			}
		}
	}

	return (1.0f - nearest) * m_pointList[index] + nearest * m_pointList[index + 1];
}

// autotarget.cpp:137
float CAutoTarget::GetDistance(const WVector &a, const WVector &b) {
	float dx = a.x - b.x;
	float dz = a.z - b.z;

	dx *= dx;
	return sqrtf(dx + dz * dz);
}

// autotarget.cpp:145
// The point to aim at with the given club: the cup when it is in reach,
// otherwise the last point of the list whose segment crosses the club's
// range circle around the ball, towards the cup.
WVector CAutoTarget::GetPoint(const char *clubName) {
	float range = GolfClub().GetRange(clubName) * 3.2f;

	if (clubName != 0) {
		if (GolfClub().GetApproachMode() < 4 || InArea(GolfBall().m_pos, WVector(GOLFDOC()->GetHoleData().m_cupPos), range) < 96.0f)
			return GOLFDOC()->GetHoleData().m_cupPos;
	} else if ((GetPVS().GetGroundType(GolfBall().m_pos, true, 0) != 0
					|| (GetPVS().GetGroundType(GolfBall().m_pos, true, 0) == 0 && GOLFDOC()->GetHoleData().m_par < 5))
			&& (GolfClub().GetApproachMode() < 4 || InArea(GolfBall().m_pos, GetHoleCupPos(), range) < 32.0f)) {
		return GOLFDOC()->GetHoleData().m_cupPos;
	}

	for (int i = m_pointList.size() - 2; i >= 0; --i) {
		float startDist = InArea(GolfBall().m_pos, m_pointList[i], range);
		float endDist = InArea(GolfBall().m_pos, m_pointList[i + 1], range);

		if (startDist * endDist < 0.0f) {
			WVector point = SolveEquation(i, range) + GolfBall().m_pos;

			WVector dir[2]; // towards the point, towards the cup
			dir[0] = point - GolfBall().m_pos;

			dir[1] = WVector(GOLFDOC()->GetHoleData().m_cupPos) - GolfBall().m_pos;

			if (dir[0].x * dir[1].x + dir[0].z * dir[1].z > 0.0f
					&& (WVector2D(dir[1].x, dir[1].z) - WVector2D(dir[0].x, dir[0].z)).Magnitude() < sqrtf(dir[1].x * dir[1].x + dir[1].z * dir[1].z)) {
				point.y = 0.0f;
				return point;
			}
		}
	}

	return GOLFDOC()->GetHoleData().m_cupPos;
}

// autotarget.cpp:190
// Where the line through points index and index + 1 (relative to the ball)
// crosses the circle of the given radius around the ball, on the XZ plane.
WVector CAutoTarget::SolveEquation(unsigned char index, float radius) {
	WVector end[2], pos; // the segment ends relative to the ball
	end[0] = m_pointList[index] - GolfBall().m_pos;
	end[1] = m_pointList[index + 1] - GolfBall().m_pos;
	float x[2], z[2];

	if (WAbsBySign(end[0].x - end[1].x) < data_9eb410) {
		x[0] = x[1] = end[0].x;

		z[0] = sqrtf(radius * radius - end[0].x * end[0].x);
		z[1] = z[0] * -1.0f;
	} else {
		float slope = (end[1].z - end[0].z) / (end[1].x - end[0].x);
		float intercept = end[0].z - slope * end[0].x;

		float disc = (slope * slope + 1.0f) * radius * radius - intercept * intercept;

		x[0] = (sqrtf(disc) - slope * intercept) / (slope * slope + 1.0f);
		x[1] = (slope * intercept * -1.0f - sqrtf(disc)) / (slope * slope + 1.0f);

		z[0] = slope * x[0] + intercept;
		z[1] = slope * x[1] + intercept;
	}

	if (x[0] >= (end[0].x > end[1].x ? end[1].x : end[0].x) && x[0] <= (end[0].x < end[1].x ? end[1].x : end[0].x)) {
		pos.x = x[0];
		pos.z = z[0];
	} else {
		pos.x = x[1]; pos.z = z[1];
	}

	return pos;
}
