// Original: d:\build\custom\temp\code\client\projectg\rival.cpp
#include "rival.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "club.h"
#include "golfball.h"
#include "quadtree.h"
#include "autotarget.h"
#include "wind.h"

#include <math.h>

float Random(float min, float max, int seed);

// wmath (?WisZero@@YIHABMM@Z); wmath.h does not declare it yet.
int WisZero(const float &value, float eps);

// The bar power of the rival's shot at a power rate of 1 (no symbol).
extern float data_00af23f4;

template <> CRival* WSingleton<CRival>::m_pInstance = NULL;

// rival.cpp:21
CRival::CRival(void) {
	m_state = 0;
	m_lieSlope = 0;
	m_lieAngle = 0;
	m_result = 0;
}

// rival.cpp:29
CRival::~CRival(void) {
}

// rival.cpp:33
void CRival::Shot(void) {
	if (WAbsBySign(m_powerRate - 1.0f) < g_EPSILON) {
		GOLFDOC()->GetBar().m_stopped = true;
	}

	GOLFDOC()->GetBar().m_power = data_00af23f4 * m_powerRate + BAR_START;
	GOLFDOC()->GetBar().m_position = Random(-10.0f, 10.0f, -1) + GOLFDOC()->GetBar().m_startPosition;

	float error = Abs(GOLFDOC()->GetBar().m_position - BAR_START);

	if (error < 2.0f) {
		GOLFDOC()->GetBar().m_accuracyGrade = 4;
	} else if (error < GolfClub().GetNarrowAccuracy()) {
		GOLFDOC()->GetBar().m_accuracyGrade = 3;
	} else if (error < GolfClub().GetWideAccuracy()) {
		GOLFDOC()->GetBar().m_accuracyGrade = 2;
	} else {
		GOLFDOC()->GetBar().m_accuracyGrade = 1;
	}
}

// rival.cpp:53
void CRival::SetVariable(CGolfDoc *pGolfDoc) {
	float maxPower = GetPVS().GetMaxPowerOnGround(GolfBall().m_pos);
	// The miss of the last shot, then the vector to the cup, then the obstacle test ray.
	WVector delta;

	if (m_result != 0) {
		GolfClub().SetAccuracy();

		delta = m_expectedPos - m_pos;

		GolfClub().SetClub((unsigned char)m_clubIndex);

		if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] == 3) {
			if (m_result != 1)
				goto aim;

			int index = m_clubIndex + 2;
			if (index > 12)
				index = 12;
			m_clubIndex = index;
			GolfClub().SetClub((unsigned char)m_clubIndex);

			m_pos = pGolfDoc->m_pAutoTarget->GetPoint(NULL);
		} else {
			switch (m_result) {
			case 1: {
				int index;
				if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_rivalLevel > 1)
					index = m_clubIndex + 3;
				else
					index = m_clubIndex + 2;
				if (index > 12)
					index = 12;
				m_clubIndex = index;
				GolfClub().SetClub((unsigned char)m_clubIndex);

				m_pos = pGolfDoc->m_pAutoTarget->GetPoint(NULL);
				m_pos = GolfBall().m_pos + maxPower * (m_pos - GolfBall().m_pos);
				return;
			}
			case 2:
				if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_rivalLevel == 1)
					goto selectClub;
				break;
			case 3:
				if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_rivalLevel == 1)
					goto aim;
				break;
			case 4:
				m_aimAngle = CalcDeltaAngle(m_pos - GolfBall().m_pos - delta, WVector::UNIT_POS_Z);
				m_pos = GolfBall().m_pos + WVector::UNIT_POS_Z * (GolfClub().GetRange(NULL) * 3.2f) * RotMat(m_aimAngle, 1);
				break;
			}

			if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_rivalLevel <= 1)
				return;

			int index = m_clubIndex + 3;
			if (index > 12)
				index = 12;
			m_clubIndex = index;
			GolfClub().SetClub((unsigned char)m_clubIndex);

			m_pos = pGolfDoc->m_pAutoTarget->GetPoint(NULL);
		}

		m_pos = GolfBall().m_pos + (m_pos - GolfBall().m_pos) * maxPower;
		return;
	}

selectClub:
	GolfClub().ClearPointFactor();
	GolfClub().SelectBestClub();

	m_clubIndex = GolfClub().GetIndex();

	for (;;) {
		m_pos = pGolfDoc->m_pAutoTarget->GetPoint(NULL);
		m_pos = GolfBall().m_pos + (m_pos - GolfBall().m_pos) * (GolfClub().GetType() == 3 ? 1.0f : maxPower);

		int groundResult;
		GetPVS().GetGroundPoint(m_pos, true, &groundResult, true);

		if (groundResult < 0)
			GolfClub().SetPointFactor(0);
		else if (GetPVS().GetGroundType(m_pos, true, NULL) == 4)
			GolfClub().SetPointFactor(0.8f);
		else if (GetPVS().GetGroundType(m_pos, true, NULL) == 3)
			GolfClub().SetPointFactor(0.6f);
		else
			break;

		GolfClub().SelectBestClub();

		if (GolfClub().GetIndex() == m_clubIndex)
			break;

		m_clubIndex = GolfClub().GetIndex();

		if (GolfClub().GetIndex() == 12) {
			m_pos = pGolfDoc->m_pAutoTarget->GetPoint(NULL);
			m_pos = GolfBall().m_pos + (m_pos - GolfBall().m_pos) * (GolfClub().GetType() == 3 ? 1.0f : maxPower);

			GetPVS().GetGroundPoint(m_pos, true, &groundResult, true);

			if (groundResult < 0) {
				GolfClub().SetPointFactor(0);
				GolfClub().SelectBestClub();
				m_pos = pGolfDoc->m_pAutoTarget->GetPoint(NULL);
				m_pos = GolfBall().m_pos + (GolfClub().GetType() == 3 ? 1.0f : maxPower) * (m_pos - GolfBall().m_pos);
				m_clubIndex = GolfClub().GetIndex();
			}
			break;
		}
	}

	GolfClub().ClearPointFactor();

aim:
	WVector dir = m_pos - GolfBall().m_pos;
	dir.y = 0;
	dir.Normalize();

	if (GolfClub().GetType() == 3) {
		dir = GetGreenSlope() * 5.0f;

		float angle = CalcDeltaAngle(m_pos - GolfBall().m_pos - dir, WVector::UNIT_POS_Z);
		m_aimAngle = angle; m_pos = GolfBall().m_pos + WVector::UNIT_POS_Z * RotMat(angle, 1);

		WVector cupPos = GOLFDOC()->GetHoleData().m_cupPos;
		delta = cupPos - GolfBall().m_pos - dir;
		delta.y = 0;

		float rate = sqrtf(delta.x * delta.x + delta.z * delta.z) / (GolfClub().GetRange(NULL) * 3.2f);
		if (rate > 1.0f)
			rate = 1.0f;
		m_powerRate = rate;
		return;
	}

	float windScale;
	if (GolfClub().GetType() == 2) {
		if (GolfClub().GetApproachMode() <= 2)
			windScale = 1.0f;
		else
			windScale = m_powerRate * 3.0f;
	} else {
		windScale = 3.0f;
	}

	m_aimAngle = CalcDeltaAngle(m_pos - GolfBall().m_pos - windScale * Wind().GetGlobalWind(), WVector::UNIT_POS_Z);
	m_pos = GolfBall().m_pos + WVector::UNIT_POS_Z * (GolfClub().GetRange(NULL) * 3.2f) * RotMat(m_aimAngle, 1);

	if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] == 1) {
		m_powerRate = 1.0f;
	} else if (m_result == 3) {
		if ((m_pos - GolfBall().m_pos).Magnitude() < (m_expectedPos - GolfBall().m_pos).Magnitude()) {
			m_powerRate = 0.9f;
			GolfBall().m_lieAngle = 0;
		} else {
			m_powerRate = 1.0f;
			GolfBall().m_lieAngle = 1.0f;
		}
	} else {
		delta = GetHoleCupPos() - GolfBall().m_pos - windScale * Wind().GetGlobalWind();
		delta.y = 0;

		float rate = sqrtf(delta.x * delta.x + delta.z * delta.z) / (GolfClub().GetRange(NULL) * 3.2f);
		if (rate > 1.0f)
			rate = 1.0f;
		m_powerRate = rate;
	}

	float lie = tanf(GolfClub().GetLie(NULL));
	float range = GolfClub().GetRange(NULL) * 3.2f;

	delta = WVector(dir.x, lie, dir.z) * (range * 0.5f);
	delta.y -= 32.0f;
	float hit = GetPVS().RayIntersectAll(GolfBall().m_pos, delta, true);

	if (hit < 1.0f) {
		if (hit > 0.5f && WisZero(GolfBall().m_lieAngle, 1e-5f) && WisZero(m_lieSlope, 1e-5f) && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_rivalLevel == 0) {
			if (GetPVS().RayIntersectAll(GolfBall().m_pos, delta * RotMat(0.17453292f, 1), true) < 1.0f) {
				GolfBall().m_lieSlope = 1.0f;
				return;
			}

			if (GetPVS().RayIntersectAll(GolfBall().m_pos, delta * RotMat(-0.17453292f, 1), true) < 1.0f) {
				GolfBall().m_lieSlope = -1.0f;
				return;
			}

			GolfBall().m_lieAngle = 1.0f;
			return;
		}

		if (hit < 0.3f) {
			int index = m_clubIndex + 3;
			if (index > 13)
				index = 13;
			m_clubIndex = index;
			GolfClub().SetClub(index);
			m_pos = pGolfDoc->m_pAutoTarget->GetPoint(NULL);
			m_pos = GolfBall().m_pos + (m_pos - GolfBall().m_pos) * maxPower;

			dir = m_pos - GolfBall().m_pos;
			dir.y = 0;
			dir.Normalize();

			lie = tanf(GolfClub().GetLie(NULL));
			range = GolfClub().GetRange(NULL) * 3.2f;
			delta = WVector(dir.x, lie, dir.z) * (range * 0.5f);
			delta.y -= 32.0f;

			int level = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_rivalLevel + 1;

			float angle = level * 0.08726646f;
			if (GetPVS().RayIntersectAll(GolfBall().m_pos, delta, true) < 1.0f)
				return;

			if (GetPVS().RayIntersectAll(GolfBall().m_pos, delta * RotMat(angle, 1), true) < 1.0f) {
				m_pos = GolfBall().m_pos + maxPower * 0.8f * (m_pos - GolfBall().m_pos) * RotMat(-angle, 1);
			} else if (GetPVS().RayIntersectAll(GolfBall().m_pos, delta * RotMat(-angle, 1), true) < 1.0f) {
				m_pos = GolfBall().m_pos + maxPower * 0.8f * (m_pos - GolfBall().m_pos) * RotMat(angle, 1);
			}
		}
	} else {
		if (GetPVS().RayIntersectAll(GolfBall().m_pos + delta, delta, true) < 1.0f) {
			int index = m_clubIndex + 1;
			if (index > 12)
				index = 12;
			m_clubIndex = index;
			GolfClub().SetClub((unsigned char)m_clubIndex);

			m_pos = pGolfDoc->m_pAutoTarget->GetPoint(NULL);
			m_pos = GolfBall().m_pos + (m_pos - GolfBall().m_pos) * maxPower;
		}
	}
}

// rival.cpp:350
void CRival::SetResult(void) {
	m_expectedPos = GolfBall().PreData(GolfBall().m_preDataCount)->m_pos;

	if (GolfBall().m_landed || GolfBall().m_bounceCount <= 4) {
		if (GetPVS().GetObjColNum() > 0) {
			m_result = 1;
		} else if (GolfBall().m_lie == 2 || GolfBall().m_lie == 3) {
			m_result = 2;
		} else {
			m_result = GolfBall().m_lie == 1 ? 3 : 4;
		}

		m_lieSlope = GolfBall().m_lieSlope;
		m_lieAngle = GolfBall().m_lieAngle;
	} else {
		m_result = 0;
		m_lieSlope = 0;
		m_lieAngle = 0;
	}
}

// rival.cpp:378
WVector CRival::GetGreenSlope(void) {
	WVector cupPos = GOLFDOC()->GetHoleData().m_cupPos;
	WVector dir;
	dir = cupPos - GolfBall().m_pos;

	dir.y = 0;
	int stepCount = (int)(sqrtf(dir.x * dir.x + dir.z * dir.z) * 0.3125f);
	dir.Normalize();

	WVector slope;
	slope.x = slope.y = slope.z = 0;
	WVector pos = GolfBall().m_pos;
	for (int i = 0; i < stepCount; i++) {
		pos += dir * 3.2f;
		// The original stores the normal into the step direction itself.
		dir = GetPVS().GetMeshNormal(pos);
		slope += WVector(dir.x, 0, dir.z);
	}
	return slope;
}
