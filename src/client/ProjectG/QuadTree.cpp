// Original: d:\build\custom\temp\code\client\projectg\quadtree.cpp
#define QUADTREE_WMATH_INLINE
#define QUADTREE_GOLFBALL_INLINE
#include "quadtree.h"
#include "GolfDoc.h"
#include "contentsdoc.h"
#include "wpuppet.h"
#include "wbone.h"
#include "wview.h"
#include "wvideo.h"
#include "ClientSetting.h"
#include "commonutil.h"
#include "slicestr.h"
#include "wutil.h"
#include "wind.h"
#include "SoundManager.h"
#include "SharedDoc.h"
#include "fx.h"
#include "club.h"
#include "InputManager.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <string>

extern LightSet g_lightset;

// The camera; its pivot is the sound listener position.
extern WMatrix g_camera;

// commonutil.cpp (no header declares it yet)
const char *GetStrConst(const char *pSrc, char *pDest);

// wmath.h:86 (?WisZero@@YIHABMM@Z); no header declares it yet.
int WisZero(const float &value, float eps);

// The commands ParsePuppetScript recognizes, by index (.data 0xa9e060).
// *hide and *show have no case.
static char *s_puppetCommands[4] = {
	"*fx(", "*snd(", "*hide", "*show"
};

// projectg.cpp:184 (no header declares it yet)
void __cdecl TextOutA(WView *view, float x, float y, const char *format, ...);

// golfdoc.cpp:69 (no header declares it yet)
WVector GetTeeShotPos(void);

// wmathutil.cpp
float __cdecl AabbIntersect(const Waabb &src, const WVector &direction, const Waabb &dest, WPlane *pPlane);
float __cdecl ObbIntersect(const Waabb &src, const WVector &direction, const Wobb &dest, WPlane *pPlane);

float RayIntersectAABB(const Waabb &aabb, const WVector &pos, const WVector &moveDelta, WPlane *pReturnPlane, float radius);


namespace intersection {
	float IntersectRayTriangle(const WVector &orig, const WVector &dir, const WVector *tri);
	bool SameSide(const WVector &p1, const WVector &p2, const WVector &a, const WVector &b);
	float IntersectPlane(const WVector &orig, const WVector &dir, const WPlane &plane, bool bothSides);
	WVector closestPointOnTriangle(const WVector &p, const WVector &a, const WVector &b, const WVector &c);
	WVector closestPointOnLine(const WVector &a, const WVector &b, const WVector &p);
	float intersectPolygon(const WVector &pos, const WVector &center, const WVector &dir, const float &radius);
	bool PointInTriangle(const WVector &p, const WVector &a, const WVector &b, const WVector &c);
}

// quadtree.h:41
CAniObj::CAniObj() {
	m_speed = 100.0f;
	m_sound[0] = '\0';
}

// quadtree.h:42
CAniObj::~CAniObj() {
}

// contentsdoc.h:472
bool CVectorSlideTestContainer::IsNewTypeVectorSlide(void) {
	return m_newTypeVectorSlide;
}

// contentsdoc.h:473
int CVectorSlideTestContainer::GetLimitSpeed(void) {
	return m_limitSpeed;
}

// contentsdoc.h:474
int CVectorSlideTestContainer::GetBoundHeight(void) {
	return m_boundHeight;
}

// contentsdoc.h:475
int CVectorSlideTestContainer::GetLimitVectorSlideCount(void) {
	return m_limitVectorSlideCount;
}

// quadtree.h
CQuadTree::sColData::sColData() {
}

// quadtree.h
fragmentsCache_s::fragmentsCache_s() {
}

// quadtree.cpp:35
WVector intersection::closestPointOnLine(const WVector &a, const WVector &b, const WVector &p) {
	WVector c = p - a;
	WVector V = (b - a).Normalize();
	float d = (a - b).Magnitude();
	float t = V.x * c.x + c.y * V.y + c.z * V.z;

	if (t < 0.0f)
		return a;
	if (t > d)
		return b;

	V = V * t;
	return a + V;
}

// quadtree.cpp:51
WVector intersection::closestPointOnTriangle(const WVector &p, const WVector &a, const WVector &b, const WVector &c) {
	WVector Rab = closestPointOnLine(a, b, p);
	WVector Rbc = closestPointOnLine(b, c, p);
	WVector Rca = closestPointOnLine(c, a, p);

	float dAB = (Rab - p).Magnitude();
	float dBC = (Rbc - p).Magnitude();
	float dCA = (Rca - p).Magnitude();

	if (dAB < dBC) {
		if (dAB < dCA)
			return Rab;
		else
			return Rca;
	} else {
		if (dBC < dCA)
			return Rbc;
		else
			return Rca;
	}
}

// quadtree.cpp:77
float intersection::intersectPolygon(const WVector &pos, const WVector &center, const WVector &dir, const float &radius) {
	float t[2];
	WVector rayDir = dir * -1.0f;
	WVector diff = pos - center;
	float a = WDotProduct(rayDir, rayDir);
	float b = WDotProduct(diff, rayDir);
	float c = WDotProduct(diff, diff);
	float disc = b * b - a * (c - radius * radius);

	if (disc < 0.0f)
		return 1.0f;

	float root = sqrtf(disc);
	float inv = 1.0f / a;
	t[0] = (-1.0f * b - root) * inv;
	t[1] = (root - b) * inv;

	if (t[0] < 0.0f) {
		if (t[1] > 0.0f && t[1] < 1.0f)
			return t[1];
	} else if (t[0] < 1.0f) {
		return t[0];
	}

	return 1.0f;
}

// quadtree.cpp:111
float intersection::IntersectRayTriangle(const WVector &orig, const WVector &dir, const WVector *tri) {
	WVector edge1, edge2, pvec, tvec, qvec;

	edge1 = tri[1] - tri[0];
	edge2 = tri[2] - tri[0];
	pvec = WCrossProduct(dir, edge2);
	float det = pvec.x * edge1.x + pvec.y * edge1.y + pvec.z * edge1.z;

	if (WAbsBySign(det) < g_EPSILON)
		return 1.0f;

	det = 1.0f / det;

	tvec = orig - tri[0];

	float u = WDotProduct(tvec, pvec * det);
	if (u < 0.0f || u > 1.0f)
		return 1.0f;

	qvec = WCrossProduct(tvec, edge1);

	float v = WDotProduct(qvec * det, dir);
	if (v < 0.0f || v > 1.0f)
		return 1.0f;

	float t = WDotProduct(qvec * det, edge2);

	return Min(t, 1.0f);
}

// quadtree.cpp:146
bool intersection::SameSide(const WVector &p1, const WVector &p2, const WVector &a, const WVector &b) {
	WVector cp1, cp2;
	cp1 = WCrossProduct(b - a, p1 - a);
	cp2 = WCrossProduct(b - a, p2 - a);

	if (cp1.x * cp2.x + cp1.y * cp2.y + cp1.z * cp2.z > 0.0f
		|| WisEqual(cp1, WVector::ZERO, WMATH_EPSILON) || WisEqual(cp2, WVector::ZERO, WMATH_EPSILON))
		return true;

	return false;
}

// quadtree.cpp:159
bool intersection::PointInTriangle(const WVector &p, const WVector &a, const WVector &b, const WVector &c) {
	if (SameSide(p, a, b, c) && SameSide(p, b, a, c) && SameSide(p, c, a, b))
		return true;

	return false;
}

// quadtree.cpp:167
float intersection::IntersectPlane(const WVector &orig, const WVector &dir, const WPlane &plane, bool bothSides) {
	float d1 = orig.x * plane.x + orig.y * plane.y + orig.z * plane.z + plane.dis;
	WVector end = orig + dir;
	float d2 = WDotProduct(end, plane.normal) + plane.dis;

	if (!(dir.x * plane.x + dir.y * plane.y + dir.z * plane.z > 0.0f)) {
		if (bothSides && d1 < 0.0f)
			return 0.0f;

		if (d1 * d2 <= 0.0f)
			return (d1 > 0.0f ? d1 : -d1) / ((d1 > 0.0f ? d1 : -d1) + (d2 > 0.0f ? d2 : -d2));
	}

	return 1.0f;
}

// quadtree.cpp:188
void ReflectVector(const WVector &normal, WVector &v) {
	v = v - normal * (2.0f * WDotProduct(normal, v));
}

// quadtree.cpp:574
// Separating-axis test of two oriented boxes (the 15 axes of Eberly's box-box
// test); the half-axes' lengths are the extents.
static bool TestIntersectionOBB(const Wobb &box0, const Wobb &box1) {
	double afEA[3];
	afEA[0] = box0.extend[0].Magnitude(); afEA[1] = box0.extend[1].Magnitude(); afEA[2] = box0.extend[2].Magnitude();
	double afEB[3];
	afEB[0] = box1.extend[0].Magnitude();
	afEB[1] = box1.extend[1].Magnitude(); afEB[2] = box1.extend[2].Magnitude();

	// Cutoff for the cosine of the angle between two axes: a parallel pair
	// makes the cross-product axes needless.
	const double fCutoff = 0.999999;
	bool bExistsParallelPair = false;
	int i;

	WVector kD = box1.center - box0.center;
	WVector akA[3];
	for (i = 0; i < 3; i++) {
		akA[i] = box0.extend[i];
		akA[i].Normalize();
	}
	WVector akB[3];
	for (i = 0; i < 3; i++) {
		akB[i] = box1.extend[i];
		akB[i].Normalize();
	}

	double aafC[3][3];     // c_ij = Dot(A_i, B_j)
	double aafAbsC[3][3];  // |c_ij|
	double afAD[3];        // Dot(A_i, D)
	double fR0, fR1, fR;
	double fR01;

	// axis C0 + t*A0
	for (i = 0; i < 3; i++) {
		aafC[0][i] = akA[0] * akB[i];
		aafAbsC[0][i] = fabs(aafC[0][i]);
		if (aafAbsC[0][i] > fCutoff)
			bExistsParallelPair = true;
	}
	afAD[0] = akA[0] * kD;
	fR = fabs(afAD[0]);
	fR1 = afEB[0] * aafAbsC[0][0] + afEB[1] * aafAbsC[0][1] + afEB[2] * aafAbsC[0][2];
	fR01 = afEA[0] + fR1;
	if (fR > fR01)
		return false;

	// axis C0 + t*A1
	for (i = 0; i < 3; i++) {
		aafC[1][i] = akA[1] * akB[i];
		aafAbsC[1][i] = fabs(aafC[1][i]);
		if (aafAbsC[1][i] > fCutoff)
			bExistsParallelPair = true;
	}
	afAD[1] = akA[1] * kD;
	fR = fabs(afAD[1]);
	fR1 = afEB[0] * aafAbsC[1][0] + afEB[1] * aafAbsC[1][1] + afEB[2] * aafAbsC[1][2];
	fR01 = afEA[1] + fR1;
	if (fR > fR01)
		return false;

	// axis C0 + t*A2
	for (i = 0; i < 3; i++) {
		aafC[2][i] = akA[2] * akB[i];
		aafAbsC[2][i] = fabs(aafC[2][i]);
		if (aafAbsC[2][i] > fCutoff)
			bExistsParallelPair = true;
	}
	afAD[2] = akA[2] * kD;
	fR = fabs(afAD[2]);
	fR1 = afEB[0] * aafAbsC[2][0] + afEB[1] * aafAbsC[2][1] + afEB[2] * aafAbsC[2][2];
	fR01 = afEA[2] + fR1;
	if (fR > fR01)
		return false;

	// axis C0 + t*B0
	fR = fabs(akB[0] * kD);
	fR0 = afEA[0] * aafAbsC[0][0] + afEA[1] * aafAbsC[1][0] + afEA[2] * aafAbsC[2][0];
	fR01 = fR0 + afEB[0];
	if (fR > fR01)
		return false;

	// axis C0 + t*B1
	fR = fabs(akB[1] * kD);
	fR0 = afEA[0] * aafAbsC[0][1] + afEA[1] * aafAbsC[1][1] + afEA[2] * aafAbsC[2][1];
	fR01 = fR0 + afEB[1];
	if (fR > fR01)
		return false;

	// axis C0 + t*B2
	fR = fabs(akB[2] * kD);
	fR0 = afEA[0] * aafAbsC[0][2] + afEA[1] * aafAbsC[1][2] + afEA[2] * aafAbsC[2][2];
	fR01 = fR0 + afEB[2];
	if (fR > fR01)
		return false;

	// A parallel pair: the separation is effectively in 2D, where the face
	// axes above suffice.
	if (bExistsParallelPair)
		return true;

	// axis C0 + t*A0xB0
	fR = fabs(afAD[2] * aafC[1][0] - afAD[1] * aafC[2][0]);
	fR0 = afEA[1] * aafAbsC[2][0] + afEA[2] * aafAbsC[1][0];
	fR1 = afEB[1] * aafAbsC[0][2] + afEB[2] * aafAbsC[0][1];
	fR01 = fR0 + fR1;
	if (fR > fR01)
		return false;

	// axis C0 + t*A0xB1
	fR = fabs(afAD[2] * aafC[1][1] - afAD[1] * aafC[2][1]);
	fR0 = afEA[1] * aafAbsC[2][1] + afEA[2] * aafAbsC[1][1];
	fR1 = afEB[0] * aafAbsC[0][2] + afEB[2] * aafAbsC[0][0];
	fR01 = fR0 + fR1;
	if (fR > fR01)
		return false;

	// axis C0 + t*A0xB2
	fR = fabs(afAD[2] * aafC[1][2] - afAD[1] * aafC[2][2]);
	fR0 = afEA[1] * aafAbsC[2][2] + afEA[2] * aafAbsC[1][2];
	fR1 = afEB[0] * aafAbsC[0][1] + afEB[1] * aafAbsC[0][0];
	fR01 = fR0 + fR1;
	if (fR > fR01)
		return false;

	// axis C0 + t*A1xB0
	fR = fabs(afAD[0] * aafC[2][0] - afAD[2] * aafC[0][0]);
	fR0 = afEA[0] * aafAbsC[2][0] + afEA[2] * aafAbsC[0][0];
	fR1 = afEB[1] * aafAbsC[1][2] + afEB[2] * aafAbsC[1][1];
	fR01 = fR0 + fR1;
	if (fR > fR01)
		return false;

	// axis C0 + t*A1xB1
	fR = fabs(afAD[0] * aafC[2][1] - afAD[2] * aafC[0][1]);
	fR0 = afEA[0] * aafAbsC[2][1] + afEA[2] * aafAbsC[0][1];
	fR1 = afEB[0] * aafAbsC[1][2] + afEB[2] * aafAbsC[1][0];
	fR01 = fR0 + fR1;
	if (fR > fR01)
		return false;

	// axis C0 + t*A1xB2
	fR = fabs(afAD[0] * aafC[2][2] - afAD[2] * aafC[0][2]);
	fR0 = afEA[0] * aafAbsC[2][2] + afEA[2] * aafAbsC[0][2];
	fR1 = afEB[0] * aafAbsC[1][1] + afEB[1] * aafAbsC[1][0];
	fR01 = fR0 + fR1;
	if (fR > fR01)
		return false;

	// axis C0 + t*A2xB0
	fR = fabs(afAD[1] * aafC[0][0] - afAD[0] * aafC[1][0]);
	fR0 = afEA[0] * aafAbsC[1][0] + afEA[1] * aafAbsC[0][0];
	fR1 = afEB[1] * aafAbsC[2][2] + afEB[2] * aafAbsC[2][1];
	fR01 = fR0 + fR1;
	if (fR > fR01)
		return false;

	// axis C0 + t*A2xB1
	fR = fabs(afAD[1] * aafC[0][1] - afAD[0] * aafC[1][1]);
	fR0 = afEA[0] * aafAbsC[1][1] + afEA[1] * aafAbsC[0][1];
	fR1 = afEB[0] * aafAbsC[2][2] + afEB[2] * aafAbsC[2][0];
	fR01 = fR0 + fR1;
	if (fR > fR01)
		return false;

	// axis C0 + t*A2xB2
	fR = fabs(afAD[1] * aafC[0][2] - afAD[0] * aafC[1][2]);
	fR0 = afEA[0] * aafAbsC[1][2] + afEA[1] * aafAbsC[0][2];
	fR1 = afEB[0] * aafAbsC[2][1] + afEB[1] * aafAbsC[2][0];
	fR01 = fR0 + fR1;
	if (fR > fR01)
		return false;

	return true;
}

int CPVSPool::ms_InstanceIdx;

// quadtree.cpp:778
CQuadTree &CPVSPool::GetInstance(void) {
	return m_pool[ms_InstanceIdx];
}

// quadtree.cpp:784
void CPVSPool::SetInstanceIdx(int idx) {
	ms_InstanceIdx = idx;
}

// quadtree.cpp:790
void SetPVSIndex(int idx) {
	CPVSPool::SetInstanceIdx(idx);
}

// quadtree.cpp:795
CQuadTree &GetPVS(void) {
	static CPVSPool pool;
	return pool.GetInstance();
}

// quadtree.cpp:802
void CQuadTree::AddWater(Waabb aabb, int index) {
	sWater water;
	water.m_aabb = aabb;
	water.m_index = index;
	m_waterList.push_back(water);
}

// quadtree.cpp:810
void CQuadTree::ResetHoleData(void) {
	m_greenBox.Reset();

	for (int i = 0; i < m_pPolySoup->GetAreaNum(); i++) {
		if (m_pPolySoup->GetArea(i)->m_type == 7) {
			m_greenBox = m_pPolySoup->GetArea(i)->m_aabb;
			break;
		}
	}

	// "\xc1\xdf\xbd\xc9": the course's "center" point
	WPolySoup::sPoint *pCenter = m_pPolySoup->FindPoint("\xc1\xdf\xbd\xc9");

	WVector pos = pCenter
		? pCenter->m_pos
		: GOLFDOC()->GetHoleData().m_cupPos;
	int index;
	GetMeshList(pos);
	pos = ColTest(pos, false, &index, true);
	if (pos.y < m_pBaseModel->m_aabb.min.y)
		pos.y = m_pBaseModel->m_aabb.min.y;
	m_greenCenter = pos;

	pos = GetHoleCupPos();
	m_cupBottom = pos.y - GolfBall().GetRadius() * 6.0f;

	m_frameTime = 0.0f;
	m_frame = 0;
	m_randSeed = 0;
	m_markedTriangle = -1;

	m_gravity = 1.0f;

	ResetVariable();
}

// quadtree.cpp:850
void CQuadTree::ResetVariable(void) {
	m_colList.clear();

	g_colNum = 0;

	m_ballState = 0;
	m_bounceCount = 0;
	m_cupRollCount = 0;
	m_objColStop = false;
	m_applyForce = true;
}

// quadtree.cpp:866
CQuadTree::CQuadTree() : m_pPolySoup(0), m_pBaseModel(0) {
	m_pQuads = 0;
	m_slopeFactor = 1.1f;
	m_bounceFactor = 1.0f;
	m_rollFactor = 1.0f;
	m_greenSlope = 0.2f;

	g_colNum = 0;

	m_holeCupRadius = GolfBall().GetRadius() * 3.0f;

	m_baseVtxColor = WVector(255.0f, 255.0f, 255.0f);
	m_objVtxColor = WVector(255.0f, 255.0f, 255.0f);
	m_vtxColorTestMode = 0;
	m_gravity = 1.0f;
}

// quadtree.cpp:886
CQuadTree::~CQuadTree() {
	for (int i = 0; i < m_quadCols * m_quadRows; i++) {
		if (m_pQuads[i].m_pList) {
			delete[] m_pQuads[i].m_pList;
			m_pQuads[i].m_pList = 0;
		}
	}
	if (m_pQuads) {
		delete[] m_pQuads;
		m_pQuads = 0;
	}

	m_waterList.clear();
	m_colList.clear();

	for (unsigned int i = 0; i < m_aniObjs.size(); i++) {
		if (m_aniObjs[i]) {
			delete m_aniObjs[i];
			m_aniObjs[i] = 0;
		}
	}
	m_aniObjs.clear();

	if (m_pBuffer2220) {
		delete[] m_pBuffer2220;
		m_pBuffer2220 = 0;
	}
	if (m_pBuffer2224) {
		delete[] m_pBuffer2224;
		m_pBuffer2224 = 0;
	}
	if (m_pBuffer2224) {
		delete[] m_pBuffer2224;
		m_pBuffer2224 = 0;
	}
}

// quadtree.cpp:907
void CQuadTree::Reserve(short count) {
	for (unsigned int i = 0; i < m_aniObjs.size(); i++) {
		if (m_aniObjs[i]) {
			delete m_aniObjs[i];
			m_aniObjs[i] = 0;
		}
	}
	PG_VECTOR_FREE_STORAGE(m_aniObjs);

	if (count > 0) {
		m_aniObjs.reserve(count);
	}
}

// quadtree.cpp:923
unsigned long CQuadTree::MakeRand(unsigned long range) {
	while (m_randSeed < range)
		m_randSeed = m_randSeed * m_randSeed + 1;

	m_randSeed = m_randSeed % range;
	return m_randSeed;
}

// quadtree.cpp:933
void CQuadTree::MakeObjStructure(short objIndex, int option, bool bRebuild) {
	WPolySoup::sModel *pModel;
	if (IsLocalContent((localContentType_t)0x56)) {
		if (option != -1)
			pModel = m_pPolySoup->m_models[option];
		else
			pModel = m_pPolySoup->m_models[objIndex];
	} else {
		pModel = m_pPolySoup->m_models[objIndex];
	}

	// A model without a puppet: drop its animated object.
	if (pModel->m_pPuppet == 0) {
		for (unsigned int i = 0; i < m_aniObjs.size(); i++) {
			if (m_aniObjs[i] && m_aniObjs[i]->m_objIndex == pModel->m_objIndex) {
				delete m_aniObjs[i];
				m_aniObjs[i] = 0;
				return;
			}
		}
		return;
	}

	if (pModel->m_subOrder != 0) {
		CAniObj *pAni = 0;

		if (bRebuild) {
			for (unsigned int i = 0; i < m_aniObjs.size(); i++) {
				if (m_aniObjs[i] && m_aniObjs[i]->m_objIndex == pModel->m_objIndex) {
					pAni = m_aniObjs[i];
					break;
				}
			}
		}

		int randRange;
		if (pModel->m_pPuppet->GetAnimationLength() > 0.0)
			randRange = (int)(pModel->m_pPuppet->GetAnimationLength() * 50.0f);
		else
			randRange = 50;

		switch (pModel->m_subOrder) {
		case 1: {
			CMoveObj *pMove;
			if (bRebuild)
				pMove = (CMoveObj *)pAni;
			else
				pMove = new CMoveObj;

			pMove->m_standTime = 0.0f;
			pMove->m_moveTime = 1.0f;
			pMove->m_startPos = pModel->m_matrix.pivot;
			pMove->m_endPos = pModel->m_matrix.pivot;

			if (pModel->m_baseName[0]) {
				char node[0x20];
				WSliceStr slice = pModel->m_baseName;
				slice.Get("*node", "%s", node);
				slice.Get("*stand", "%f", &pMove->m_standTime);
				slice.Get("*move", "%f", &pMove->m_moveTime);

				CNodeContainer *pNode = m_pPolySoup->FindNode(node);
				if (pNode) {
					pMove->m_startPos = pNode->m_points[0];
					pMove->m_endPos = pNode->m_points[1];
					pModel->m_matrix.pivot = pNode->m_points[0];
				}
			}
			pAni = pMove;
			break;
		}
		case 2: {
			CRotObj *pScale = (CRotObj *)pAni;
			if (!bRebuild)
				pScale = new CRotObj;

			float dy = pModel->m_aabb.max.y - pModel->m_aabb.min.y;
			float dx = pModel->m_aabb.max.x - pModel->m_aabb.min.x;
			float dz = pModel->m_aabb.max.z - pModel->m_aabb.min.z;
			pScale->m_invSize = 1.0f / Max(Max(dx, dy), dz);
			pAni = pScale;
			break;
		}
		case 3:
			if (!bRebuild)
				pAni = new CVtxObj;
			break;
		case 4:
			if (!bRebuild)
				pAni = new CTranObj;
			break;
		}

		if (pAni) {
			pAni->m_rand = MakeRand(randRange);
			pAni->m_type = pModel->m_subOrder;
			pAni->m_objIndex = objIndex;
			if (!bRebuild)
				m_aniObjs.push_back(pAni);
		}
	}

	if (pModel->m_reservedByte == 3)
		return;

	// The base quads: which cells of the base grid the model's box covers.
	float lo[3] = {0.0f, 0.0f, 0.0f};
	float hi[3] = {0.0f, 0.0f, 0.0f};

	if (pModel->m_subOrder != 1 && pModel->m_subOrder != 3) {
		int minCol = (int)((pModel->m_aabb.min.x - m_quadOrigin.x) * 0.05f);
		if (minCol < 0)
			minCol = 0;
		else if (minCol > m_quadCols - 1)
			minCol = m_quadCols - 1;

		int maxCol = (int)((pModel->m_aabb.max.x - m_quadOrigin.x) * 0.05f);
		if (maxCol < 0)
			maxCol = 0;
		else if (maxCol > m_quadCols - 1)
			maxCol = m_quadCols - 1;

		int minRow = (int)((pModel->m_aabb.min.z - m_quadOrigin.z) * 0.05f);
		if (minRow < 0)
			minRow = 0;
		else if (minRow > m_quadRows - 1)
			minRow = m_quadRows - 1;

		int maxRow = (int)((pModel->m_aabb.max.z - m_quadOrigin.z) * 0.05f);
		if (maxRow < 0)
			maxRow = 0;
		else if (maxRow > m_quadRows - 1)
			maxRow = m_quadRows - 1;

		for (int row = minRow; row <= maxRow; row++) {
			for (int col = minCol; col <= maxCol; col++) {
				WPolySoup::sQuad &quad = m_pQuads[m_quadCols * row + col];

				if (quad.m_count2 == 0) {
					short *pNewList = new short[4];
					quad.m_pList2 = pNewList;
					pNewList[0] = objIndex;
					quad.m_count2 = 1;
					continue;
				}

				short *pList = quad.m_pList2;
				int j;
				for (j = 0; j < quad.m_count2; j++) {
					if (pList[j] == objIndex)
						break;
				}

				if (j < quad.m_count2)
					continue;

				if (quad.m_count2 % 4 == 0) {
					short *pTemp = new short[quad.m_count2];
					memcpy(pTemp, pList, quad.m_count2 * sizeof(short));

					delete[] pList;
					pList = new short[quad.m_count2 + 4];
					memcpy(pList, pTemp, quad.m_count2 * sizeof(short));
					quad.m_pList2 = pList;

					delete[] pTemp;
				}

				pList[quad.m_count2] = objIndex;
				quad.m_count2++;
			}
		}
	}

	if (pModel->m_reservedByte & 2)
		return;

	// The model's collision triangles, in world space for the static kinds.
	if (bRebuild) {
		delete[] pModel->m_pTriangles;
		if (pModel->m_pQuads)
			delete[] pModel->m_pQuads;
	}

	int faceNum = pModel->m_pPuppet->GetFaceNum();
	pModel->m_pTriangles = new WPolySoup::sTriangle[faceNum];

	bool bHasAbv = pModel->m_pPuppet->FindBoneByTail("abv") != 0;

	int triIndex = 0;
	for (w_bound_box *pBox = pModel->m_pPuppet->m_bbList.Start(); pBox; pBox = pModel->m_pPuppet->m_bbList.Next()) {
		WBone *pBone = pBox->bone;

		if (_stricmp(pBox->info->name, pBone->GetBoneName()) != 0)
			continue;

		if (bHasAbv) {
			if (!strstr(_strlwr(pBox->info->name), "abv"))
				continue;
		}

		for (w_mesh *pMesh = pBone->GetMesh(); pMesh; pMesh = pMesh->next) {
			for (int k = 0; k < pMesh->indexNum; k += 3) {
				if (pModel->m_subOrder == 0 || pModel->m_subOrder == 4) {
					for (int j = 0; j < 3; j++)
						pModel->m_pTriangles[triIndex].m_vertex[j] = pMesh->vecList[pMesh->indexList[k + j]] * pBone->GetMatrix();

					WPolySoup::sTriangle *pTri = &pModel->m_pTriangles[triIndex];

					WVector normal = WCrossProduct(pTri->m_vertex[2] - pTri->m_vertex[1], pTri->m_vertex[0] - pTri->m_vertex[1]);
					normal.Normalize();

					pTri->m_plane.normal = normal;
					pModel->m_pTriangles[triIndex].m_plane.dis = WDotProduct(pModel->m_pTriangles[triIndex].m_plane.normal * -1.0f, pTri->m_vertex[0]);
				}

				pModel->m_pTriangles[triIndex].m_pBone = pBone;
				pModel->m_pTriangles[triIndex].m_doubleSided = (pMesh->xiDrawFlag2 >> 10) & 1;
				pModel->m_pTriangles[triIndex].m_propIndex = m_pPolySoup->m_pTextures[m_pPolySoup->m_texIndexMap[pMesh->texHandle & 0x7FF]].m_param;
				pModel->m_pTriangles[triIndex].m_texIndex = m_pPolySoup->m_pTextures[m_pPolySoup->m_texIndexMap[pMesh->texHandle & 0x7FF]].m_handle;

				triIndex++;
				if (triIndex >= faceNum)
					goto trianglesDone;
			}
		}
	}
trianglesDone:

	// The object quads: one quad holding every triangle, or the 8 octants.
	if (pModel->m_pPuppet->GetFaceNum() < 0x200 || pModel->m_subOrder == 2 || pModel->m_subOrder == 1 || pModel->m_subOrder == 3) {
		pModel->m_pQuads = new WPolySoup::sQuad[1];
		pModel->m_singleQuad = 1;
		pModel->m_pQuads[0].m_pList = new int[pModel->m_pPuppet->GetFaceNum()];
		pModel->m_pQuads[0].m_count = (short)pModel->m_pPuppet->GetFaceNum();

		for (int i = 0; i < pModel->m_pPuppet->GetFaceNum(); i++)
			pModel->m_pQuads[0].m_pList[i] = i;
		return;
	}

	pModel->m_pQuads = new WPolySoup::sQuad[8];
	pModel->m_singleQuad = 0;

	// The center of the model: the mean of its vertices in world space.
	pModel->m_basePos.x = 0.0f;
	pModel->m_basePos.y = 0.0f;
	pModel->m_basePos.z = 0.0f;

	int vertexCount = 0;
	for (w_bound_box *pBox = pModel->m_pPuppet->m_bbList.Start(); pBox; pBox = pModel->m_pPuppet->m_bbList.Next()) {
		WBone *pBone = pBox->bone;

		if (_stricmp(pBox->info->name, pBone->GetBoneName()) != 0)
			continue;

		for (w_mesh *pMesh = pBone->GetMesh(); pMesh; pMesh = pMesh->next) {
			for (int v = 0; v < pMesh->vtxNum; v++) {
				WVector pos = pMesh->vecList[v] * pBone->GetMatrix();
				vertexCount++;
				pModel->m_basePos.x += pos.x;
				pModel->m_basePos.y += pos.y;
				pModel->m_basePos.z += pos.z;
			}
		}
	}

	float invCount = 1.0f / vertexCount;
	pModel->m_basePos.x *= invCount;
	pModel->m_basePos.y *= invCount;
	pModel->m_basePos.z *= invCount;

	for (int i = 0; i < pModel->m_pPuppet->GetFaceNum(); i++) {
		WPolySoup::sTriangle *pTri = &pModel->m_pTriangles[i];

		lo[0] = Min(Min(pTri->m_vertex[0].x, pTri->m_vertex[1].x), pTri->m_vertex[2].x);
		lo[1] = Min(Min(pTri->m_vertex[0].y, pTri->m_vertex[1].y), pTri->m_vertex[2].y);
		lo[2] = Min(Min(pTri->m_vertex[0].z, pTri->m_vertex[1].z), pTri->m_vertex[2].z);

		hi[0] = Max(Max(pTri->m_vertex[0].x, pTri->m_vertex[1].x), pTri->m_vertex[2].x);
		hi[1] = Max(Max(pTri->m_vertex[0].y, pTri->m_vertex[1].y), pTri->m_vertex[2].y);
		hi[2] = Max(Max(pTri->m_vertex[0].z, pTri->m_vertex[1].z), pTri->m_vertex[2].z);

		// Which octants around the center the triangle touches.
		int octants[8] = {1, 1, 1, 1, 1, 1, 1, 1};

		const float *pCenter = &pModel->m_basePos.x;
		for (int axis = 0; axis < 3; axis++) {
			int bit = 1 << axis;
			if (hi[axis] < pCenter[axis]) {
				for (int q = 0; q < 8; q++) {
					if ((bit & q) == bit)
						octants[q] = 0;
				}
			} else if (lo[axis] > pCenter[axis]) {
				for (int q = 0; q < 8; q++) {
					if (!(q & bit))
						octants[q] = 0;
				}
			}
		}

		for (int q = 0; q < 8; q++) {
			if (octants[q] != 1)
				continue;

			WPolySoup::sQuad &quad = pModel->m_pQuads[q];

			if (quad.m_count == 0) {
				quad.m_pList = new int[0x40];
				quad.m_pList[0] = i;
				quad.m_count = 1;
				continue;
			}

			if (quad.m_count % 0x40 == 0) {
				int *pTemp = new int[quad.m_count];
				memcpy(pTemp, quad.m_pList, quad.m_count * sizeof(int));

				delete[] quad.m_pList;
				quad.m_pList = new int[quad.m_count + 0x40];
				memcpy(quad.m_pList, pTemp, quad.m_count * sizeof(int));

				delete[] pTemp;
			}

			quad.m_pList[quad.m_count] = i;
			quad.m_count++;
		}
	}
}

// quadtree.cpp:1382
unsigned char CQuadTree::GetObjQuad(const WVector &pos, const WVector &center) {
	unsigned char quad = 0;
	if (pos.x > center.x)
		quad = 1;
	if (pos.y > center.y)
		quad |= 2;
	if (pos.z > center.z)
		quad |= 4;

	return quad;
}

// quadtree.cpp:1395
// Collects the object's triangles the ball's move crosses (its octant quads at
// the start and at the end, merged sorted), then sweeps the ball by dir against
// them: the nearest hit closer than t updates t, *pPlane and index.
bool CQuadTree::ObjColTest(const WVector &dir, short objIndex, WPlane *pPlane, float &t, int &index) {
	WPolySoup::sModel *pModel = m_pPolySoup->m_models[objIndex];
	bool result = false;

	if (pModel->m_singleQuad) {
		m_meshNum = pModel->m_pQuads[0].m_count;
		memcpy(m_meshList, pModel->m_pQuads[0].m_pList, m_meshNum * sizeof(int));
	} else {
		int from = GetObjQuad(GolfBall().m_pos, pModel->m_basePos);
		int to = GetObjQuad(GolfBall().m_pos + dir, pModel->m_basePos);

		if (from == to) {
			m_meshNum = pModel->m_pQuads[from].m_count;
			memcpy(m_meshList, pModel->m_pQuads[from].m_pList, m_meshNum * sizeof(int));
		} else {
			// Merges the two sorted lists without duplicates.
			int a = 0;
			int b = 0;
			m_meshNum = 0;

			while (pModel->m_pQuads[from].m_count && pModel->m_pQuads[to].m_count) {
				if (pModel->m_pQuads[from].m_pList[a] < pModel->m_pQuads[to].m_pList[b]) {
					m_meshList[m_meshNum] = pModel->m_pQuads[from].m_pList[a];
					a++;
				} else if (pModel->m_pQuads[to].m_pList[b] < pModel->m_pQuads[from].m_pList[a]) {
					m_meshList[m_meshNum] = pModel->m_pQuads[to].m_pList[b];
					b++;
				} else {
					m_meshList[m_meshNum] = pModel->m_pQuads[from].m_pList[a];
					a++;
					b++;
				}
				m_meshNum++;

				if (a == pModel->m_pQuads[from].m_count) {
					if (b == pModel->m_pQuads[to].m_count)
						break;

					int n = pModel->m_pQuads[to].m_count - b;
					if (n > 0x800 - m_meshNum)
						n = 0x800 - m_meshNum;
					if (n < 0) {
						m_objColStop = true;
						return false;
					}
					memcpy(&m_meshList[m_meshNum], &pModel->m_pQuads[to].m_pList[b], n * sizeof(int));
					m_meshNum += n;
					break;
				}

				if (b == pModel->m_pQuads[to].m_count) {
					int n = pModel->m_pQuads[from].m_count - a;
					if (n > 0x800 - m_meshNum)
						n = 0x800 - m_meshNum;
					if (n < 0) {
						m_objColStop = true;
						return false;
					}
					memcpy(&m_meshList[m_meshNum], &pModel->m_pQuads[from].m_pList[a], n * sizeof(int));
					m_meshNum += n;
					break;
				}
			}

			if (m_meshNum > 0x800)
				m_objColStop = true;
		}
	}

	// The height range the ball sweeps.
	float low;
	float high;
	if (dir.y > 0.0f) {
		low = GolfBall().m_pos.y - GolfBall().m_radius;
		high = GolfBall().m_pos.y + dir.y + GolfBall().m_radius;
	} else {
		low = GolfBall().m_pos.y - GolfBall().m_radius + dir.y;
		high = GolfBall().m_radius + GolfBall().m_pos.y;
	}

	for (int i = 0; i < m_meshNum; i++) {
		if (objIndex == GolfBall().m_lastContactId && i == GolfBall().m_lastContactIndex)
			continue;

		int tri = m_meshList[i];

		if (pModel->m_pTriangles[tri].m_pBone->IsInvisible())
			continue;

		if (m_pPolySoup->GetProperty(pModel->m_pTriangles[tri].m_propIndex).m_penetrable) {
			if ((GolfBall().m_flags & 0x10)
					|| ((GolfBall().m_flags & 0x20) && GolfBall().m_eventPreDataIndex[9] < 0)
					|| WAbsBySign(m_pPolySoup->GetProperty(pModel->m_pTriangles[tri].m_propIndex).m_bounceCoeff - 1.0f) < g_EPSILON)
				continue;

			if (GolfBall().m_flags & 0x40)
				continue;
		}

		int above = 0;
		int below = 0;

		pModel->m_pTriangles[tri].m_edgeHit = false;

		WVector vtx[3];
		for (int k = 0; k < 3; k++) {
			vtx[k] = pModel->m_pTriangles[tri].m_vertex[k];

			if (vtx[k].y > high)
				above++;
			else if (vtx[k].y < low)
				below++;

			if (GolfBall().m_eventPreDataIndex[0] < 0 && GolfBall().m_eventPreDataIndex[2] < 0 && WVectorLen(GolfBall().m_pos - vtx[k]) < 3.0f)
				GolfBall().m_eventPreDataIndex[2] = GolfBall().m_preDataCount;
		}

		if (below == 3 || above == 3)
			continue;

		WPlane plane = pModel->m_pTriangles[tri].m_plane;
		if (pModel->m_pTriangles[tri].m_doubleSided && WDotProduct(plane.normal, GolfBall().m_pos) + plane.dis < 0.0f) {
			plane.normal.Negate();
			plane.dis *= -1.0f;
		}

		WVector start = GolfBall().m_pos - plane.normal * GolfBall().m_radius;
		float d = intersection::IntersectPlane(start, dir, plane, false);

		WVector hit = start + dir * d;

		if (d < t) {
			if (intersection::SameSide(hit, vtx[0], vtx[1], vtx[2]) && intersection::SameSide(hit, vtx[1], vtx[0], vtx[2]) && intersection::SameSide(hit, vtx[2], vtx[0], vtx[1])) {
				t = d;
				*pPlane = pModel->m_pTriangles[tri].m_plane;
				index = tri;

				result = true;
				if (pModel->m_pTriangles[tri].m_doubleSided && WDotProduct(GolfBall().m_pos, pModel->m_pTriangles[tri].m_plane.normal) + pModel->m_pTriangles[tri].m_plane.dis < 0.0f) {
					pPlane->normal.Negate();
					pPlane->dis *= -1.0f;
				}
			} else {
				WVector closest = intersection::closestPointOnTriangle(hit, vtx[0], vtx[1], vtx[2]);
				float d2 = intersection::intersectPolygon(closest, GolfBall().m_pos, dir, GolfBall().GetRadius());

				if (d2 < t) {
					pModel->m_pTriangles[tri].m_edgeHit = true;

					t = d2;
					*pPlane = pModel->m_pTriangles[tri].m_plane;
					index = tri;

					result = true;
					if (pModel->m_pTriangles[tri].m_doubleSided && WDotProduct(pModel->m_pTriangles[tri].m_plane.normal, GolfBall().m_pos) + pModel->m_pTriangles[tri].m_plane.dis < 0.0f) {
						pPlane->normal.Negate();
						pPlane->dis *= -1.0f;
					}
				}
			}
		}
	}

	return result;
}

// quadtree.cpp:1618
void CQuadTree::MakeBaseStructure(WPolySoup::sModel *pModel, bool bRebuild) {
	ResetWater();

	m_pBaseModel = pModel;

	if (m_pQuads) {
		delete[] m_pQuads;
		m_pQuads = 0;
	}

	const Waabb &aabb = m_pBaseModel->m_aabb;

	m_lowest = 100000.0f;
	m_quadOrigin = WVector(aabb.min.x, 0.0f, aabb.min.z);
	m_quadCols = 1 - (int)((m_pBaseModel->m_aabb.max.x - m_pBaseModel->m_aabb.min.x) * -0.05f);
	m_quadRows = 1 - (int)((m_pBaseModel->m_aabb.max.z - m_pBaseModel->m_aabb.min.z) * -0.05f);

	if (bRebuild) {
		if (m_pQuads)
			delete[] m_pQuads;
		m_pQuads = 0;
	}

	m_pQuads = new WPolySoup::sQuad[m_quadCols * m_quadRows];

	for (int i = 0; i < m_pBaseModel->m_pPuppet->GetFaceNum(); i++) {
		WPolySoup::sTriangle &tri = m_pBaseModel->m_pTriangles[i];

		if (tri.m_doubleSided) {
			if (Abs(m_pBaseModel->m_pTriangles[i].m_plane.normal.y) > (float)cos(0.34906585f))
				m_lowest = Min(Min(m_lowest, tri.m_vertex[0].y), Min(tri.m_vertex[1].y, tri.m_vertex[2].y));
		} else if (m_pBaseModel->m_pTriangles[i].m_plane.normal.y > (float)cos(0.34906585f)) {
			m_lowest = Min(Min(m_lowest, tri.m_vertex[0].y), Min(tri.m_vertex[1].y, tri.m_vertex[2].y));
		}

		float minX = Min(tri.m_vertex[0].x, Min(tri.m_vertex[1].x, tri.m_vertex[2].x));
		float maxX = Max(tri.m_vertex[0].x, Max(tri.m_vertex[1].x, tri.m_vertex[2].x));
		float minZ = Min(tri.m_vertex[0].z, Min(tri.m_vertex[1].z, tri.m_vertex[2].z));
		float maxZ = Max(tri.m_vertex[0].z, Max(tri.m_vertex[1].z, tri.m_vertex[2].z));

		minX -= m_quadOrigin.x;
		maxX -= m_quadOrigin.x;
		minZ -= m_quadOrigin.z;
		maxZ -= m_quadOrigin.z;

		int minCol = (int)(minX * 0.05f);
		int maxCol = (int)(maxX * 0.05f);
		int minRow = (int)(minZ * 0.05f);
		int maxRow = (int)(maxZ * 0.05f);

		for (int row = minRow; row <= maxRow; row++) {
			for (int col = minCol; col <= maxCol; col++) {
				int index = row * m_quadCols + col;

				if (m_pQuads[index].m_count == 0) {
					m_pQuads[index].m_pList = new int[0x10];
					m_pQuads[index].m_pList[0] = i;
					m_pQuads[index].m_count = 1;
					continue;
				}

				int j;
				for (j = 0; j < m_pQuads[index].m_count; j++) {
					if (m_pQuads[index].m_pList[j] == i)
						break;
				}

				if (j < m_pQuads[index].m_count)
					continue;

				if (m_pQuads[index].m_count % 0x10 == 0) {
					int *pTemp = new int[m_pQuads[index].m_count];
					memcpy(pTemp, m_pQuads[index].m_pList, m_pQuads[index].m_count * sizeof(int));

					delete[] m_pQuads[index].m_pList;
					m_pQuads[index].m_pList = new int[m_pQuads[index].m_count + 0x10];
					memcpy(m_pQuads[index].m_pList, pTemp, m_pQuads[index].m_count * sizeof(int));

					delete[] pTemp;
				}

				m_pQuads[row * m_quadCols + col].m_pList[m_pQuads[row * m_quadCols + col].m_count] = i;
				m_pQuads[row * m_quadCols + col].m_count++;
			}
		}
	}

	m_lowest = Max(m_lowest, m_pBaseModel->m_aabb.min.y) - 1.0f;
}

// quadtree.cpp:1729
WVector CQuadTree::ColTest(const WVector &pos, bool bBase, int *pIndex, bool bAll) {
	WVector vtx[3];
	WVector result = pos;
	bool found = false;

	result.y = -100000.0f;

	for (int i = 0; i < m_meshNum; i++) {
		int index = m_meshList[i];

		if (!bAll && m_pBaseModel->m_pTriangles[index].m_doubleSided)
			continue;

		if (!m_pBaseModel->m_pTriangles[index].m_doubleSided && m_pBaseModel->m_pTriangles[index].m_plane.normal.y < 0.0f)
			continue;

		if (Abs(m_pBaseModel->m_pTriangles[index].m_plane.normal.y) < 0.1f)
			continue;

		for (int k = 0; k < 3; k++) {
			vtx[k] = m_pBaseModel->m_pTriangles[index].m_vertex[k];
			vtx[k].y = result.y;
		}

		if (intersection::SameSide(result, vtx[0], vtx[1], vtx[2]) && intersection::SameSide(result, vtx[1], vtx[0], vtx[2]) && intersection::SameSide(result, vtx[2], vtx[0], vtx[1])) {
			float y = (result.x * m_pBaseModel->m_pTriangles[index].m_plane.normal.x + result.z * m_pBaseModel->m_pTriangles[index].m_plane.normal.z + m_pBaseModel->m_pTriangles[index].m_plane.dis) / m_pBaseModel->m_pTriangles[index].m_plane.normal.y * -1.0f;

			if (y > result.y) {
				result.y = y;
				found = true;
				if (pIndex)
					*pIndex = index;
			}
		}
	}

	if (!found) {
		if (bBase) {
			if (pIndex)
				*pIndex = -1;
			return pos;
		}
		if (GolfBall().m_pos.y < m_pBaseModel->m_aabb.min.y)
			result.y = m_pBaseModel->m_aabb.min.y;
	}

	return result;
}

// quadtree.cpp:1795
WVector CQuadTree::ColObject(const WVector &pos, bool bBase, int *pIndex, bool bAll) {
	if (!IsLocalContent((localContentType_t)0x56))
		return pos;

	WVector result = pos;
	bool found = false;

	WPolySoup::sModel *pModel = m_pPolySoup->m_models[0];
	int count = pModel->m_pQuads->m_count;

	for (int i = 0; i < count; i++) {
		int index = pModel->m_pQuads->m_pList[i];

		// the first triangle's flag (as the original reads it)
		if (!bAll && pModel->m_pTriangles->m_doubleSided)
			continue;

		WPolySoup::sTriangle &tri = pModel->m_pTriangles[index];
		if (!tri.m_doubleSided && tri.m_plane.normal.y < 0.0f)
			continue;

		float ny = tri.m_plane.normal.y;
		if (!(ny > 0.0f))
			ny = -ny;
		if (ny < 0.1f)
			continue;

		WVector v[3];
		for (int k = 0; k < 3; k++)
			v[k] = pModel->m_pTriangles[index].m_vertex[k];

		if (intersection::SameSide(result, v[0], v[1], v[2]) && intersection::SameSide(result, v[1], v[0], v[2]) && intersection::SameSide(result, v[2], v[0], v[1])) {
			float y = (result.x * tri.m_plane.normal.x + result.z * tri.m_plane.normal.z + tri.m_plane.dis) / tri.m_plane.normal.y * -1.0f;

			if (y > result.y) {
				result.y = y;
				found = true;
				break;
			}
		}
	}

	if (!found) {
		if (bBase) {
			if (pIndex)
				*pIndex = -1;
			return pos;
		}

		if (pos.y < m_pBaseModel->m_aabb.min.y)
			result.y = m_pBaseModel->m_aabb.min.y;
	}

	return result;
}

// quadtree.cpp:1869
WVector CQuadTree::TestCameraCollision(const WVector &target) {
	WVector camera = m_pPolySoup->GetCameraPos(target, GolfBall().m_pos);

	camera = GetGroundPoint(camera, true, 0, true);
	float limit = target.y - GOLFDOC()->GetCameraData().m_groundOffset;
	camera.y = (camera.y < limit ? limit : camera.y) + GOLFDOC()->GetCameraData().m_groundOffset;

	return camera;
}

// quadtree.cpp:1883
int CQuadTree::GetMeshIndex(const WVector &pos, bool bAll) {
	int result = -1;
	std::vector<int> candidates;
	float best = 100000.0f;
	WVector vtx[3];
	int i;

	GetMeshList(pos);

	for (i = 0; i < m_meshNum; i++) {
		int index = m_meshList[i];

		// back faces do not count unless the triangle is double sided
		if (!m_pBaseModel->m_pTriangles[index].m_doubleSided && m_pBaseModel->m_pTriangles[index].m_plane.normal.y < 0.0f)
			continue;

		// the triangle seen from above
		for (int k = 0; k < 3; k++) {
			vtx[k] = m_pBaseModel->m_pTriangles[index].m_vertex[k];
			vtx[k].y = pos.y;
		}

		if (intersection::SameSide(pos, vtx[0], vtx[1], vtx[2]) && intersection::SameSide(pos, vtx[1], vtx[0], vtx[2]) && intersection::SameSide(pos, vtx[2], vtx[0], vtx[1]))
			candidates.push_back(m_meshList[i]);
	}

	if (candidates.size() == 1) {
		result = candidates[0];
	} else {
		// several triangles are over the point: the plane nearest to it wins
		int count = candidates.size();
		for (i = 0; i < count; i++) {
			const WPolySoup::sTriangle &tri = m_pBaseModel->m_pTriangles[candidates[i]];
			float dist = pos * tri.m_plane;
			if (dist < 0.0f && tri.m_doubleSided)
				dist *= -1.0f;

			if (fabs(dist) < fabs(best)) {
				best = dist;
				result = candidates[i];
			}
		}
	}

	if (bAll) {
		for (i = 0; i < (int)m_waterList.size(); i++) {
			Waabb aabb = m_waterList[i].m_aabb;
			if (pos.x > aabb.min.x && pos.x < aabb.max.x && pos.z > aabb.min.z && pos.z < aabb.max.z) {
				if (ColTest(pos, true, 0, true).y < aabb.max.y) {
					result = -2;
					break;
				}
			}
		}
	}

	return result;
}

// quadtree.cpp:1949
int CQuadTree::GetObjIndex(const WVector &pos, bool bAll) {
	int result = -1;
	std::vector<int> candidates;
	float best = 100000.0f;

	if (!IsLocalContent((localContentType_t)0x56))
		return -1;

	GetObjList(pos, pos);

	for (int i = 0; i < m_objNum; i++) {
		const WPolySoup::sTriangle *pTri = m_pPolySoup->m_models[m_objList[i]]->m_pTriangles;

		if (!pTri->m_doubleSided && pTri->m_plane.normal.y < 0.0f)
			continue;

		WVector v[3];
		for (int k = 0; k < 3; k++) {
			v[k] = m_pPolySoup->m_models[m_objList[i]]->m_pTriangles->m_vertex[k];
			v[k].y = pos.y;
		}

		if (PointInTriangle(pos, v[0], v[1], v[2]))
			candidates.push_back(m_objList[i]);
	}

	if (candidates.size() == 1) {
		result = m_objList[0];
	} else {
		int count = candidates.size();
		for (int i = 0; i < count; i++) {
			const WPolySoup::sTriangle *pTri = m_pPolySoup->m_models[candidates[i]]->m_pTriangles;

			float dist = pos * pTri->m_plane;
			if (dist < 0.0f && pTri->m_doubleSided)
				dist *= -1.0f;

			if (fabs(dist) < fabs(best)) {
				best = dist;
				result = candidates[i];
			}
		}
	}

	return result;
}

// quadtree.cpp:2019
WVector CQuadTree::GetGroundPoint(WVector pos, bool bBase, int *pIndex, bool bWater) {
	GetMeshList(pos);

	int index = -1;
	pos = ColTest(pos, bBase, &index, true);

	if (bBase && index == -1) {
		pos.y = m_pBaseModel->m_aabb.min.y;
	}

	if (pIndex != 0) {
		*pIndex = index;
	}

	if (bWater) {
		for (int i = 0; i < (int)m_waterList.size(); i++) {
			Waabb aabb = m_waterList[i].m_aabb;
			if (pos.x > aabb.min.x && pos.x < aabb.max.x
				&& pos.z > aabb.min.z && pos.z < aabb.max.z
				&& pos.y < aabb.max.y) {
				pos.y = aabb.max.y;
				if (pIndex != 0) {
					*pIndex = -2;
				}
				break;
			}
		}
	}

	if (pos.y < m_pBaseModel->m_aabb.min.y) {
		pos.y = m_pBaseModel->m_aabb.min.y;
		if (pIndex != 0) {
			*pIndex = Min(-1, *pIndex);
		}
	}

	return pos;
}

// quadtree.cpp:2065
float CQuadTree::GetHighestGround(WVector pos, int *pIndex) {
	WVector vtx[3];
	GetMeshList(pos);

	float highest = -100000.0f;
	bool found = false;

	for (int i = 0; i < m_meshNum; i++) {
		int index = m_meshList[i];

		if (Abs(m_pBaseModel->m_pTriangles[index].m_plane.normal.y) < 0.1f)
			continue;

		if (!m_pBaseModel->m_pTriangles[index].m_doubleSided && m_pBaseModel->m_pTriangles[index].m_plane.normal.y < 0.0f)
			continue;

		for (int k = 0; k < 3; k++) {
			vtx[k] = m_pBaseModel->m_pTriangles[index].m_vertex[k];
			vtx[k].y = pos.y;
		}

		if (intersection::SameSide(pos, vtx[0], vtx[1], vtx[2]) && intersection::SameSide(pos, vtx[1], vtx[0], vtx[2]) && intersection::SameSide(pos, vtx[2], vtx[0], vtx[1])) {
			float y = (pos.x * m_pBaseModel->m_pTriangles[index].m_plane.normal.x + pos.z * m_pBaseModel->m_pTriangles[index].m_plane.normal.z + m_pBaseModel->m_pTriangles[index].m_plane.dis) / m_pBaseModel->m_pTriangles[index].m_plane.normal.y * -1.0f;

			// The ground closest to the given height wins.
			if (Abs(y - pos.y) < Abs(highest - pos.y)) {
				found = true;
				if (pIndex)
					*pIndex = index;
				highest = y;
			}
		}
	}

	if (!found) {
		if (pIndex)
			*pIndex = -1;
		return pos.y;
	}

	return highest;
}

// quadtree.cpp:2118
float CQuadTree::GetLowestGround(WVector pos) {
	WVector vtx[3];
	GetMeshList(pos);

	float lowest = 100000.0f;
	bool found = false;

	for (int i = 0; i < m_meshNum; i++) {
		int index = m_meshList[i];

		if (Abs(m_pBaseModel->m_pTriangles[index].m_plane.normal.y) < 0.1f)
			continue;

		if (!m_pBaseModel->m_pTriangles[index].m_doubleSided && m_pBaseModel->m_pTriangles[index].m_plane.normal.y < 0.0f)
			continue;

		for (int k = 0; k < 3; k++) {
			vtx[k] = m_pBaseModel->m_pTriangles[index].m_vertex[k];
			vtx[k].y = pos.y;
		}

		if (intersection::SameSide(pos, vtx[0], vtx[1], vtx[2]) && intersection::SameSide(pos, vtx[1], vtx[0], vtx[2]) && intersection::SameSide(pos, vtx[2], vtx[0], vtx[1])) {
			found = true;
			float y = (pos.x * m_pBaseModel->m_pTriangles[index].m_plane.normal.x + pos.z * m_pBaseModel->m_pTriangles[index].m_plane.normal.z + m_pBaseModel->m_pTriangles[index].m_plane.dis) / m_pBaseModel->m_pTriangles[index].m_plane.normal.y * -1.0f;

			if (lowest > y)
				lowest = y;
		}
	}

	if (!found)
		return pos.y;

	return lowest;
}

// quadtree.cpp:2157
void CQuadTree::GetMeshList(WVector pos) {
	m_meshNum = 0;
	WVector local = pos - m_quadOrigin;
	int row = Between(0, (int)(local.z * 0.05f), m_quadRows - 1);
	int col = Between(0, (int)(local.x * 0.05f), m_quadCols - 1);

	for (int i = 0; i < m_pQuads[row * m_quadCols + col].m_count; i++) {
		int j;
		for (j = 0; j < m_meshNum; j++) {
			if (m_pQuads[row * m_quadCols + col].m_pList[i] == m_meshList[j])
				break;
		}

		if (j == m_meshNum) {
			m_meshList[m_meshNum] = m_pQuads[row * m_quadCols + col].m_pList[i];
			m_meshNum++;
		}
	}
}

// quadtree.cpp:2184
void CQuadTree::GetMeshList(WVector pos, const WVector &dir) {
	m_meshNum = 0;
	pos.x -= m_quadOrigin.x, pos.z -= m_quadOrigin.z;

	// The quads the segment's bounding rectangle covers.
	int minCol = Between(0, (int)((dir.x > 0.0f ? pos.x : pos.x + dir.x) * 0.05f), m_quadCols - 1);
	int maxCol = Between(0, (int)((dir.x < 0.0f ? pos.x : pos.x + dir.x) * 0.05f), m_quadCols - 1);
	int minRow = Between(0, (int)((dir.z > 0.0f ? pos.z : pos.z + dir.z) * 0.05f), m_quadRows - 1);
	int maxRow = Between(0, (int)((dir.z < 0.0f ? pos.z : pos.z + dir.z) * 0.05f), m_quadRows - 1);

	int numCols = maxCol - minCol + 1, numRows = maxRow - minRow + 1;
	int *pGrid = new int[numCols * numRows];

	if (Wabs(dir.x) < g_EPSILON) {
		// A segment along z: the quads of one column.
		int i;
		for (i = 0; i < numRows; i++)
			pGrid[i] = 1;
	} else {
		// The row the segment crosses each column edge at, then the rows between two edges.
		int *pEdge = new int[numCols + 1];
		int i;
		for (i = 0; i < numCols + 1; i++)
			pEdge[i] = (int)((((minCol + i) * 20.0f - pos.x) * (dir.z / dir.x) + pos.z) * 0.05f) - minRow;

		for (i = 0; i < numCols; i++) {
			int j;
			for (j = 0; j < numRows; j++)
				pGrid[j * numCols + i] = j >= (pEdge[i] > pEdge[i + 1] ? pEdge[i + 1] : pEdge[i]) && j <= (pEdge[i] < pEdge[i + 1] ? pEdge[i + 1] : pEdge[i]) ? 1 : 0;
		}

		delete[] pEdge;
	}

	int row;
	for (row = 0; row < numRows; row++) {
		int col;
		for (col = 0; col < numCols; col++) {
			if (pGrid[row * numCols + col]) {
				int i;
				for (i = 0; i < m_pQuads[(minRow + row) * m_quadCols + col + minCol].m_count; i++) {
					int j;
					for (j = 0; j < m_meshNum; j++) {
						if (m_pQuads[(minRow + row) * m_quadCols + col + minCol].m_pList[i] == m_meshList[j])
							break;
					}

					if (j == m_meshNum) {
						m_meshList[m_meshNum] = m_pQuads[(minRow + row) * m_quadCols + col + minCol].m_pList[i];
						m_meshNum++;
					}
				}
			}
		}
	}

	delete[] pGrid;
}

// quadtree.cpp:2259
void CQuadTree::GetMeshList(Waabb box) {
	m_meshNum = 0;
	box.min.x -= m_quadOrigin.x, box.min.z -= m_quadOrigin.z;
	box.max.x -= m_quadOrigin.x, box.max.z -= m_quadOrigin.z;

	m_boxMinCol = Between(0, (int)(box.min.x * 0.05f), m_quadCols - 1);
	m_boxMaxCol = Between(0, (int)(box.max.x * 0.05f), m_quadCols - 1);
	m_boxMinRow = Between(0, (int)(box.min.z * 0.05f), m_quadRows - 1);
	m_boxMaxRow = Between(0, (int)(box.max.z * 0.05f), m_quadRows - 1);

	for (int row = m_boxMinRow; row <= m_boxMaxRow; row++) {
		for (int col = m_boxMinCol; col <= m_boxMaxCol; col++) {
			for (int i = 0; i < m_pQuads[row * m_quadCols + col].m_count; i++) {
				int j;
				for (j = 0; j < m_meshNum; j++) {
					if (m_pQuads[row * m_quadCols + col].m_pList[i] == m_meshList[j])
						break;
				}

				if (j == m_meshNum) {
					m_meshList[m_meshNum] = m_pQuads[row * m_quadCols + col].m_pList[i];
					m_meshNum++;
				}
			}
		}
	}
}

// quadtree.cpp:2299
void CQuadTree::GetObjList(Waabb box) {
	m_objNum = 0;
	box.min.x -= m_quadOrigin.x, box.min.z -= m_quadOrigin.z;
	box.max.x -= m_quadOrigin.x, box.max.z -= m_quadOrigin.z;

	m_boxMinCol = Between(0, (int)(box.min.x * 0.05f), m_quadCols - 1);
	m_boxMaxCol = Between(0, (int)(box.max.x * 0.05f), m_quadCols - 1);
	m_boxMinRow = Between(0, (int)(box.min.z * 0.05f), m_quadRows - 1);
	m_boxMaxRow = Between(0, (int)(box.max.z * 0.05f), m_quadRows - 1);

	for (int row = m_boxMinRow; row <= m_boxMaxRow; row++) {
		for (int col = m_boxMinCol; col <= m_boxMaxCol; col++) {
			for (int i = 0; i < m_pQuads[row * m_quadCols + col].m_count2; i++) {
				// the list holds short object indices
				int j;
				for (j = 0; j < m_objNum; j++) {
					if (m_pQuads[row * m_quadCols + col].m_pList2[i] == m_objList[j])
						break;
				}

				if (j == m_objNum) {
					m_objList[m_objNum] = m_pQuads[row * m_quadCols + col].m_pList2[i];
					m_objNum++;
				}
			}
		}
	}

	// The moving objects, wherever they are.
	unsigned int i;
	for (i = 0; i < m_aniObjs.size(); i++) {
		if (m_aniObjs[i]) {
			if (m_aniObjs[i]->m_type == 1 || m_aniObjs[i]->m_type == 3) {
				if (m_pPolySoup->GetModelNum() == m_pPolySoup->GetRmrModelNum()) {
					m_objList[m_objNum] = m_aniObjs[i]->m_objIndex;
				} else {
					int j;
					for (j = 0; j < m_pPolySoup->m_numObjects; j++) {
						if (m_pPolySoup->m_models[j]->m_objIndex == m_aniObjs[i]->m_objIndex)
							break;
					}

					if (j >= m_pPolySoup->m_numObjects)
						continue;

					m_objList[m_objNum] = (short)j;
				}

				m_objNum++;
			}
		}
	}
}

// quadtree.cpp:2365
void CQuadTree::GetObjList(WVector pos, const WVector &dir) {
	m_objNum = 0;
	pos.x -= m_quadOrigin.x, pos.z -= m_quadOrigin.z;

	int minCol = Between(0, (int)((dir.x > 0.0f ? pos.x : pos.x + dir.x) * 0.05f), m_quadCols - 1);
	int maxCol = Between(0, (int)((dir.x < 0.0f ? pos.x : pos.x + dir.x) * 0.05f), m_quadCols - 1);
	int minRow = Between(0, (int)((dir.z > 0.0f ? pos.z : pos.z + dir.z) * 0.05f), m_quadRows - 1);
	int maxRow = Between(0, (int)((dir.z < 0.0f ? pos.z : pos.z + dir.z) * 0.05f), m_quadRows - 1);

	int numCols = maxCol - minCol + 1, numRows = maxRow - minRow + 1;
	int *pGrid = new int[numCols * numRows];

	if (Wabs(dir.x) < g_EPSILON) {
		int i;
		for (i = 0; i < numRows; i++)
			pGrid[i] = 1;
	} else {
		int *pEdge = new int[numCols + 1];
		int i;
		for (i = 0; i < numCols + 1; i++)
			pEdge[i] = (int)((((minCol + i) * 20.0f - pos.x) * (dir.z / dir.x) + pos.z) * 0.05f) - minRow;

		if (dir.z / dir.x >= 0.0f) {
			for (i = 0; i < numCols; i++) {
				int j;
				for (j = 0; j < numRows; j++)
					pGrid[j * numCols + i] = j >= pEdge[i] && j <= pEdge[i + 1] ? 1 : 0;
			}
		} else {
			for (i = 0; i < numCols; i++) {
				int j;
				for (j = 0; j < numRows; j++)
					pGrid[j * numCols + i] = j >= pEdge[i + 1] && j <= pEdge[i] ? 1 : 0;
			}
		}

		delete[] pEdge;
	}

	int row;
	for (row = 0; row < numRows; row++) {
		int col;
		for (col = 0; col < numCols; col++) {
			if (pGrid[row * numCols + col]) {
				int i;
				for (i = 0; i < m_pQuads[(minRow + row) * m_quadCols + col + minCol].m_count2; i++) {
					// the list holds short object indices
					int j;
					for (j = 0; j < m_objNum; j++) {
						if (m_pQuads[(minRow + row) * m_quadCols + col + minCol].m_pList2[i] == m_objList[j])
							break;
					}

					if (j == m_objNum) {
						m_objList[m_objNum] = m_pQuads[(minRow + row) * m_quadCols + col + minCol].m_pList2[i];
						m_objNum++;
					}
				}
			}
		}
	}

	delete[] pGrid;

	// The moving objects, wherever they are.
	unsigned int i;
	for (i = 0; i < m_aniObjs.size(); i++) {
		if (m_aniObjs[i]) {
			if (m_aniObjs[i]->m_type == 1 || m_aniObjs[i]->m_type == 3) {
				if (m_pPolySoup->GetModelNum() == m_pPolySoup->GetRmrModelNum()) {
					m_objList[m_objNum] = m_aniObjs[i]->m_objIndex;
				} else {
					int j;
					for (j = 0; j < m_pPolySoup->m_numObjects; j++) {
						if (m_pPolySoup->m_models[j]->m_objIndex == m_aniObjs[i]->m_objIndex)
							break;
					}

					if (j >= m_pPolySoup->m_numObjects)
						continue;

					m_objList[m_objNum] = (short)j;
				}

				m_objNum++;
			}
		}
	}
}

// quadtree.cpp:2477
float CQuadTree::GetGroundSlope(int index) const {
	if (index < 0)
		return 0.0f;

	WVector normal = m_pBaseModel->m_pTriangles[index].m_plane.normal;
	if (m_pBaseModel->m_pTriangles[index].m_doubleSided && normal.y < 0.0f)
		normal.y = -normal.y;

	float angle = (float)acos(normal.y < -1.0f ? -1.0f : (normal.y > 1.0f ? 1.0f : normal.y));
	return angle > 0.0f ? angle : -angle;
}

// quadtree.cpp:2495
WVector CQuadTree::GetMeshNormal(int index) {
	if (index < 0)
		return GetMeshNormal(GolfBall().m_pos);

	WVector normal = m_pBaseModel->m_pTriangles[index].m_plane.normal;
	if (m_pBaseModel->m_pTriangles[index].m_doubleSided && normal.y < 0.0f)
		normal.x = -normal.x, normal.y = -normal.y, normal.z = -normal.z;

	return normal;
}

// quadtree.cpp:2508
WVector CQuadTree::GetMeshNormal(const WVector &pos) {
	int index = -1;
	if (IsLocalContent((localContentType_t)0x56))
		index = GetMeshIndex(pos, false);

	if (index < 0)
		return WVector::ZERO;

	WVector normal = m_pBaseModel->m_pTriangles[index].m_plane.normal;
	if (m_pBaseModel->m_pTriangles[index].m_doubleSided && normal.y < 0.0f)
		normal.x = -normal.x, normal.y = -normal.y, normal.z = -normal.z;

	return normal;
}

// quadtree.cpp:2526
WVector CQuadTree::GetMeshObjcet(const WVector &pos) {
	int index = GetObjIndex(pos, false);

	if (index < 0)
		return WVector::ZERO;

	short objIndex = m_objList[index];
	WVector normal = m_pPolySoup->m_models[objIndex]->m_basePos;
	if (m_pPolySoup->m_models[objIndex]->m_pTriangles[0].m_doubleSided && normal.y < 0.0f)
		normal.x = -normal.x, normal.y = -normal.y, normal.z = -normal.z;

	return normal;
}

// quadtree.cpp:2543
bool CQuadTree::PointInTriangle(const WVector &p, const WVector &a, const WVector &b, const WVector &c) const {
	return intersection::SameSide(p, a, b, c) && intersection::SameSide(p, b, a, c) && intersection::SameSide(p, c, a, b) ? true : false;
}

// quadtree.cpp:2548
bool CQuadTree::SameSide(const WVector &p1, const WVector &p2, const WVector &a, const WVector &b) const {
	return intersection::SameSide(p1, p2, a, b);
}

// quadtree.cpp:2553
float CQuadTree::IntersectPlane(const WVector &orig, const WVector &dir, const WPlane &plane, bool bothSides) {
	return intersection::IntersectPlane(orig, dir, plane, bothSides);
}

// quadtree.cpp:2558
float CQuadTree::IntersectObj(const WVector &pos, const WVector &dir, WPlane *pPlane) {
	float t = 1.0f;
	short hitObj = -1;
	int index = -1;
	WVector radius = WVector::ONE * GolfBall().GetRadius(); WVector absDir(Abs(dir.x), Abs(dir.y), Abs(dir.z));

	WVector ext = radius + absDir;

	Waabb box;
	box.min = pos - ext;
	box.max = pos + ext;
	GetObjList(box);

	unsigned int i;
	for (i = 0; i < m_objNum; i++) {
		WPolySoup::sModel *pModel = m_pPolySoup->m_models[m_objList[i]];

		Waabb box;
		box.min = pModel->m_aabb.min - ext;
		box.max = ext + pModel->m_aabb.max;

		if (box.min.x <= pos.x && pos.x <= box.max.x && box.min.y <= pos.y && pos.y <= box.max.y
			&& box.min.z <= pos.z && pos.z <= box.max.z) {
			if (pModel->m_subOrder == 2 || pModel->m_subOrder == 1 || pModel->m_subOrder == 3)
				MakeCollisionData(m_objList[i]);

			if (ObjColTest(dir, m_objList[i], pPlane, t, index))
				hitObj = m_objList[i];
		}
	}

	if (hitObj >= 0) {
		WPolySoup::sModel *pModel = m_pPolySoup->m_models[hitObj];

		if (GolfBall().m_flags & 0x10)
			pModel->m_hitCount++;

		if ((GolfBall().m_flags & 0x40) && GolfBall().m_eventPreDataIndex[1] >= 0
			&& GolfBall().m_preDataCount > GolfBall().m_eventPreDataIndex[1])
			pModel->m_hitCount++;

		m_colData.m_objIndex = hitObj;
		m_colData.m_triIndex = index;
		m_colData.m_objType = pModel->m_subOrder;
		m_colData.m_frame = GolfBall().m_preDataCount;

		int texId = pModel->m_pTriangles[index].m_texIndex & 0x7FF; m_pPolySoup->m_texIndexMap.find(texId); strcpy(g_colTex[g_colNum], m_pPolySoup->m_pTextures[m_pPolySoup->m_texIndexMap[texId]].m_name);
		g_colNum++;
		if (g_colNum >= 16)
			g_colNum = 15;

		GolfBall().m_lastEventPreDataIndex = pModel->m_pTriangles[index].m_propIndex;
	}

	return t;
}

// quadtree.cpp:2631
int CQuadTree::GetTextureHandle(const WVector &pos) {
	int index = GetMeshIndex(pos, false);
	if (index < 0)
		return 0;

	return m_pBaseModel->m_pTriangles[index].m_texIndex;
}

// quadtree.cpp:2640
const char *CQuadTree::GetTextureName(const WVector &pos) {
	int index = GetMeshIndex(pos, false);
	if (index < 0)
		return 0;

	int texId = m_pBaseModel->m_pTriangles[index].m_texIndex & 0x7FF; m_pPolySoup->m_texIndexMap.find(texId); return m_pPolySoup->m_pTextures[m_pPolySoup->m_texIndexMap[texId]].m_name;
}

// quadtree.cpp:2649
void CQuadTree::MakeCollisionData(short objIndex) {
	WPolySoup::sModel *pModel = m_pPolySoup->m_models[objIndex];

	if (!pModel->m_pPuppet)
		return;

	// Rebuild the object's triangles (vertices and planes) from its meshes.
	int triIndex = 0;
	for (w_bound_box *pBox = pModel->m_pPuppet->m_bbList.Start(); pBox; pBox = pModel->m_pPuppet->m_bbList.Next()) {
		WBone *pBone = pBox->bone;

		if (strcmpi(pBox->info->name, pBone->GetBoneName()) != 0)
			continue;

		for (w_mesh *pMesh = pBone->GetMesh(); pMesh; pMesh = pMesh->next) {
			for (int k = 0; k < pMesh->indexNum; k += 3) {
				for (int j = 0; j < 3; j++)
					pModel->m_pTriangles[triIndex].m_vertex[j] = pMesh->vecList[pMesh->indexList[k + j]] * pBone->GetMatrix();

				WPolySoup::sTriangle *pTri = &pModel->m_pTriangles[triIndex];

				WVector normal;
				normal = WCrossProduct(pTri->m_vertex[2] - pTri->m_vertex[1], pTri->m_vertex[0] - pTri->m_vertex[1]);
				normal.Normalize();

				pTri->m_plane.normal = normal;
				pModel->m_pTriangles[triIndex].m_plane.dis = WDotProduct(pModel->m_pTriangles[triIndex].m_plane.normal * -1.0f, pTri->m_vertex[0]);

				triIndex++;
			}
		}
	}
}

// quadtree.cpp:2694
void CQuadTree::CalcObjMeshData(short objIndex, int faceIndex) {
	WPolySoup::sModel *pModel = m_pPolySoup->m_models[objIndex];

	if (!pModel->m_pPuppet)
		return;

	// Recompute the vertices of the object's triangle faceIndex from its mesh.
	for (w_bound_box *pBox = pModel->m_pPuppet->m_bbList.Start(); pBox; pBox = pModel->m_pPuppet->m_bbList.Next()) {
		WBone *pBone = pBox->bone;

		if (strcmpi(pBox->info->name, pBone->GetBoneName()) != 0)
			continue;

		int triIndex = 0;
		for (w_mesh *pMesh = pBone->GetMesh(); pMesh; pMesh = pMesh->next) {
			for (int k = 0; k < pMesh->indexNum; k += 3) {
				if (triIndex == faceIndex) {
					for (int j = 0; j < 3; j++)
						pModel->m_pTriangles[triIndex].m_vertex[j] = pMesh->vecList[pMesh->indexList[k + j]] * pBone->GetMatrix();
					return;
				}
				triIndex++;
			}
		}
	}
}

// quadtree.cpp:2734
void CQuadTree::PreProcess(float elapsed, bool bForce) {
	WPolySoup::sModel *pModel;
	char sound[16];

	if (bForce) {
		m_frame += 1;
	} else {
		float time = elapsed + m_frameTime;
		m_frameTime = time;
		if (time < 0.02f || WisZero(elapsed, g_EPSILON))
			return;

		unsigned long ticks = (long)(time * 50.0f);

		float used = (float)ticks * 0.02f;
		m_frameTime = time - used; m_frame += ticks;
	}

	if (Wind().HasSpecialWind()) {
		unsigned long phase = (m_frame / 50) % 170;
		if (IsLocalContent((localContentType_t)0x4F)) {
			if (WSingleton<CSharedDoc>::Instance()->m_playingMode != 2)
				Wind().SetSpecialWind(phase >= 80);
		} else {
			Wind().SetSpecialWind(phase >= 80);
		}

		if (!bForce) {
			if (Wind().IsActiveSpecialWind()) {
				if (Wind().GetSpWindStartFrame() == 0)
					Wind().SetSpWindStartFrame(m_frame);
			} else if (Wind().GetSpWindStartFrame() != 0) {
				Wind().SetSpWindStartFrame(0);
			}
			Wind().GenerateFx(phase >= 60);
		}
	}

	for (unsigned int i = 0; i < m_aniObjs.size(); i++) {
		CAniObj *pAni = m_aniObjs[i];
		if (!pAni)
			continue;

		int objIndex = pAni->m_objIndex;
		if (m_pPolySoup->GetModelNum() == m_pPolySoup->GetRmrModelNum()) {
			pModel = m_pPolySoup->m_models[objIndex];
		} else {
			for (int j = 0; j < m_pPolySoup->m_numObjects; j++) {
				if (m_pPolySoup->m_models[j]->m_objIndex == objIndex) {
					pModel = m_pPolySoup->m_models[j];
					break;
				}
				pModel = 0;
			}
		}
		if (!pModel || !pModel->m_pPuppet)
			continue;

		if (!bForce && pAni->m_sound[0]) {
			sprintf(sound, "sound%d", i);
			if (g_audio->IsPlaying(sound)) {
				float dx = pModel->m_matrix.pivot.x - g_camera.pivot.x;
				float dy = pModel->m_matrix.pivot.y - g_camera.pivot.y;
				float dz = pModel->m_matrix.pivot.z - g_camera.pivot.z;
				g_audio->SetSFXVolumeWhilePlaying(sound, Max(0.0f, 1.0f - (float)sqrt(dx * dx + dy * dy + dz * dz) / (pAni->m_speed * 3.2f)));
			} else {
				pAni->m_sound[0] = '\0';
			}
		}

		if (pModel->m_subOrder == 4 && bForce)
			continue;

		float aniTime = (float)(unsigned long)(pAni->m_rand + m_frame) * 0.02f; pAni->m_time = aniTime;

		switch (pModel->m_subOrder) {
		case 2: {
			CRotObj *pScale = (CRotObj *)pAni;
			if (pModel->m_windFactor > 0.0f)
				pScale->m_angle = Wind().GetGlobalIntensity() * pScale->m_invSize * pModel->m_windFactor * pAni->m_time * g_PI;
			else
				pScale->m_angle = aniTime * pModel->m_windFactor * -3.1415927f;
			pModel->m_pPuppet->ApplyBones(0, RotMat(pScale->m_angle, 2) * pModel->m_matrix);
			pModel->m_pPuppet->Transform(0, 1.0f, 0);
			continue;
		}
		case 1: {
			CMoveObj *pMove = (CMoveObj *)pAni;
			float local = pAni->m_time - (float)floor(aniTime / (pMove->m_standTime + pMove->m_moveTime)) * (pMove->m_standTime + pMove->m_moveTime);

			int odd = (int)floor(pAni->m_time / (pMove->m_standTime + pMove->m_moveTime)) % 2;

			if (local > pMove->m_moveTime) {
				pModel->m_matrix.pivot = (&pMove->m_startPos)[1 - odd];
			} else {
				pModel->m_matrix.pivot = ((&pMove->m_startPos)[1 - odd] - (&pMove->m_startPos)[odd]) * (local / pMove->m_moveTime) + (&pMove->m_startPos)[odd];
			}
			break;
		}
		case 3: {
			CVtxObj *pVtx = (CVtxObj *)pAni;
			break;
		}
		case 4: {
			CTranObj *pTran = (CTranObj *)pAni;
			break;
		}
		}

		if (pModel->m_pPuppet && pModel->m_pPuppet->m_aniLen > 0.0f) {
			float rem = Max(0.0f, pAni->m_time - (float)floor(pAni->m_time / pModel->m_pPuppet->m_aniLen) * pModel->m_pPuppet->m_aniLen);

			if (!bForce)
				ScriptProcess(i, rem - elapsed, rem);

			pModel->m_pPuppet->ApplyBones(pModel->m_pPuppet->GetBones(0, rem, 0, 0, false), pModel->m_matrix);
		} else {
			pModel->m_pPuppet->ApplyBones(0, pModel->m_matrix);
		}
		pModel->m_pPuppet->Transform(0, 1.0f, 0);
	}
}


// quadtree.cpp:2904
void CQuadTree::ScriptProcess(int index, float from, float to) {
	CAniObj *pAni = m_aniObjs[index];
	WPolySoup::sModel *pModel = m_pPolySoup->m_models[pAni->m_objIndex];
	WPuppet *pPuppet = pModel->m_pPuppet;

	if (from == to)
		return;

	from = ((0.0f < from) ? from : 0.0f) * 30.0f;
	to *= 30.0f;
	float tolerance = pPuppet->GetFrameCount() ? (((float)pPuppet->m_frame[1].num * (1.0f / 30.0f * 1.0f) - (float)pPuppet->m_frame[0].num * (1.0f / 30.0f * 1.0f)) < 0.0f ? 0.0f : ((float)pPuppet->m_frame[1].num * (1.0f / 30.0f * 1.0f) - (float)pPuppet->m_frame[0].num * (1.0f / 30.0f * 1.0f))) : 0.3f;
	int first = Ceil(from, tolerance);

	int last = ((float)floor(to) == to) ? (int)to - 1 : (int)floor(to);

	if (first > last)
		return;

	{
		int lo, hi;
		if (pPuppet->CheckFrameData(first, last, lo, hi)) {
			for (int f = lo; f <= hi; f++)
				ParsePuppetScript(index, pPuppet->m_frame[f].data);
		}
	}
}

// quadtree.cpp:2944
void CQuadTree::ParsePuppetScript(int index, const char *script) {
	static char s_command[64];

	WPuppet *pPuppet = m_pPolySoup->m_models[m_aniObjs[index]->m_objIndex]->m_pPuppet;

	if (script == NULL) {
		return;
	}

	const char *p = script;
	int len;

	while (p) {
		p = strchr(p, '*');

		if (p == NULL) {
			break;
		}

		int cmdLen = 0;

		while (p[cmdLen] != '\0') {
			if (p[++cmdLen] == ' ') {
				break;
			}
			if (p[cmdLen] == '(') {
				cmdLen++;
				break;
			}
		}

		if (cmdLen <= 1) {
			break;
		}

		strncpy(s_command, p, cmdLen);
		s_command[cmdLen] = '\0';
		p += cmdLen;

		for (unsigned int command = 0; command < sizeof(s_puppetCommands) / sizeof(s_puppetCommands[0]); command++) {
			if (strcmpi(s_command, s_puppetCommands[command]) == 0) {
				switch (command) {
				case 0: {   // *fx(
					char token[64];
					char fxName[64];
					char boneName[64];

					token[0] = '\0';
					p = GetStrConst(p, token);

					if ((len = strlen(token)) == 0 || strlen(token) >= sizeof(token)) {
						break;
					}

					if (token[0] == '@') {
						p = GetStrConst(p, fxName);

						if ((len = strlen(fxName)) == 0 || strlen(fxName) >= sizeof(fxName)) {
							break;
						}
					}
					else {
						strcpy(fxName, token);
					}

					if (pPuppet) {
						WVector pivot = pPuppet->GetPivot(NULL);
						WMatrix mat = pPuppet->m_mat;
						unsigned char ground = GetGroundType(pivot * mat, false, NULL);
						if ((ground == 10 || ground == 14) && strstr(fxName, "dust")) {
							// On water the dust effects become their water variants.
							char arg[256] = { 0 };

							WSliceStr(fxName).Get("dust", "%s", arg);
							std::string argStr(arg);
							std::string fxStr(fxName);
							std::string prefix = fxStr.substr(0, fxStr.length() - argStr.length() - 4);
							fxStr = prefix + "water" + argStr;
							strcpy(fxName, fxStr.c_str());
						}
					}

					Attacher attacher;

					unsigned int i;
					for (i = 0; i < 4; i++) {
						p = GetStrConst(p, boneName);

						if ((len = strlen(boneName)) == 0 || strlen(boneName) >= sizeof(boneName)) {
							break;
						}

						WBone *pBone = pPuppet->FindBone(boneName);
						if (pBone) {
							attacher.m_pMatrix[i] = &pBone->GetMatrix();
						}
						else {
							w_bound_box *pBox = pPuppet->m_bbList.Find(boneName);
							if (pBox) {
								if (pBox->bone) {
									attacher.m_pMatrix[i] = &pBox->bone->GetMatrix();
								}
								attacher.m_pOffset[i] = &pBox->info->spherePivot;
							}
						}
					}

					if ((int)i > 0) {
						WSingleton<CFx>::Instance()->Attach(attacher, fxName, true, NULL);
					}
					break;
				}
				case 1: {   // *snd(
					char alias[16];
					char name[128];
					int loop;

					sprintf(alias, "sound%d", index);
					p = GetStrConst(p, name);

					loop = 0;
					sscanf(name, "%s %d", m_aniObjs[index]->m_sound, &loop);

					if (g_audio->IsPlaying(alias))
						g_audio->StopSfx(alias);

					WPolySoup::sModel *pModel = m_pPolySoup->m_models[m_aniObjs[index]->m_objIndex];
					WVector d = pModel->m_matrix.pivot - g_camera.pivot;

					g_audio->PlaySfx(m_aniObjs[index]->m_sound, NULL, loop != 0, alias, NULL, 0.5f, 200.0f);
					float volume = 1.0f - (float)sqrt(d.x * d.x + d.y * d.y + d.z * d.z) / (m_aniObjs[index]->m_speed * 3.2f);
					g_audio->SetSFXVolumeWhilePlaying(alias, (0.0f < volume) ? volume : 0.0f);
					break;
				}
				}
			}
		}
	}
}

// quadtree.cpp:3112
// One frame of the ball: holes it when it rests in the cup, stops it below
// the course, tests it against the caddie's box and the cup, then runs the
// process of its state.
void CQuadTree::BallProcess(float elapsed) {
	if (IsLocalContent((localContentType_t)0x3A)
			&& WSingleton<CSharedDoc>::Instance()->m_roomType != 0xB
			&& WSingleton<CSharedDoc>::Instance()->m_roomType != 0xC) {
		if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x800)
			m_holeCupRadius = GolfBall().GetRadius() * 6.0f;
		else
			m_holeCupRadius = GolfBall().GetRadius() * 3.0f;
	}

	CGolfBall &ball = GolfBall();

	m_lowestGroundMode = false;

	if (ball.m_state != 2) {
		WVector moved = ball.PreData(Max(0, ball.m_preDataCount - 2))->m_pos - ball.PreData(Max(0, ball.m_preDataCount - 1))->m_pos;
		if (WisEqual(moved, WVector::ZERO, 1e-5f)) {
			ball.m_shotStage++;
		} else {
			WVector d;
			d = ball.m_pos - GetHoleCupPos();
			if ((float)sqrt(d.x * d.x + d.z * d.z) < m_holeCupRadius) {
				ball.m_shotStage++;
				if (ball.m_shotStage == 100) {
					ball.m_pos = GetHoleCupPos();
					ball.m_pos.y = m_cupBottom + ball.m_radius;
					HoleIn();
					return;
				}
			} else {
				ball.m_shotStage = 0;
			}
		}
	}

	ball.m_groundType = GetGroundType(ball.m_pos, true, 0);

	WVector d = ball.m_pos - GetHoleCupPos();
	d.y = 0.0f;

	if ((float)sqrt(d.x * d.x + d.z * d.z) < m_holeCupRadius && m_cupBottom + ball.m_radius > ball.m_pos.y) {
		ball.m_pos.y = m_cupBottom + ball.m_radius;
		HoleIn();
		return;
	}

	if (ball.m_pos.y < m_pBaseModel->m_aabb.min.y) {
		ball.m_velocity.Reset();
		ball.m_landed = true;
		ball.m_state = 0;
		return;
	}

	if (ball.m_preDataCount < 100 && m_ballState == 0) {
		WVector center = ball.m_pos;
		center.y = ((m_ballTestBox.max + m_ballTestBox.min) * 0.5f).y;
		if (RayIntersectAABB(m_ballTestBox, center, WVector(elapsed * ball.m_velocity.x, 0.0f, elapsed * ball.m_velocity.z), 0, 0.0f) < 1.0f) {
			if (ball.m_pos.y < m_ballTestBox.min.y)
				m_ballState = 2;
			else if (ball.m_pos.y > m_ballTestBox.max.y)
				m_ballState = 1;
			else
				m_ballState = 3;
		}
	}

	m_cupApproach = DistanceBetweenRayNSegment(GetHoleCupPos() - WVector(0.0f, GolfBall().m_radius * 6.0f, 0.0f), WVector::UNIT_POS_Y, ball.m_pos, ball.m_velocity * elapsed);

	if (ball.m_eventPreDataIndex[8] < 0 && ball.m_eventPreDataIndex[7] < 0
			&& m_holeCupRadius - ball.m_radius > m_cupApproach.z
			&& ((ball.m_flags & 0x20) ? ball.m_velocity.y < 0.0f : true)) {
		ball.m_eventPreDataIndex[8] = ball.m_preDataCount;
		ball.m_pos += m_cupApproach.y * ball.m_velocity * elapsed;

		d = GetHoleCupPos() - ball.m_pos;
		d.y = 0.0f;
		elapsed = d.Magnitude() * 0.4f / (m_holeCupRadius - ball.m_radius) + 0.1f;
		ball.m_velocity = WVector(elapsed * ball.m_velocity.x, Abs(ball.m_velocity.y) * -2.0f, elapsed * ball.m_velocity.z);
		return;
	}

	WVector next = ball.m_pos + ball.m_velocity * elapsed;
	d = next - GetHoleCupPos();

	int index;
	if (m_holeCupRadius + ball.GetRadius() > m_cupApproach.z) {
		if (ball.m_state == 3 && ball.m_velocity.Magnitude() > 12.0f
				&& Abs(CalcDeltaAngle(WVector(d.x, 0.0f, d.z) * -1.0f, ball.m_velocity)) < 1.0471976f) {
			ball.m_curve = ball.m_spin = 0.0f;
			ball.m_velocity.y = Min(ball.m_velocity.Magnitude() * 0.5f, 5.0f);
			ball.m_pos += ball.m_velocity * elapsed;
			ball.m_state = 1;
			ball.m_groundArea = -1;
			CGolfBall::sExpected *pData = ball.PreDataToWrite(ball.m_preDataCount);
			pData->m_groundSubType = 2;
			pData->m_groundType = 3;
			pData->m_contactVector = WVector::UNIT_POS_Y;
			strcpy(pData->m_sound, "\xc3\xe6\xb5\xb9_wood");
			ball.m_eventPreDataIndex[4] = ball.m_preDataCount;
			return;
		}

		WVector probe = next; GetMeshList(next); probe = ColTest(probe, false, &index, true); if (probe.y < m_pBaseModel->m_aabb.min.y) probe.y = m_pBaseModel->m_aabb.min.y; WVector ground = probe;
		if (ground.y + ball.m_radius + 0.01f < next.y) {
			ball.m_groundArea = -1;
			ball.m_state = 1;
		} else if (m_cupApproach.z < m_holeCupRadius) {
			ball.m_state = 4;
			if (ball.m_eventPreDataIndex[3] < 0)
				ball.m_eventPreDataIndex[3] = ball.m_preDataCount;
		}
	} else {
		m_cupRollCount = 0;

		if (ball.m_state == 4) {
			WVector probe = next; GetMeshList(next); probe = ColTest(probe, false, &index, true); if (probe.y < m_pBaseModel->m_aabb.min.y) probe.y = m_pBaseModel->m_aabb.min.y; WVector ground = probe;
			if (ground.y + ball.m_radius + 0.01f < next.y) {
				ball.m_state = 1;
			} else {
				probe = ball.m_pos; GetMeshList(ball.m_pos); probe = ColTest(probe, false, &index, true); if (probe.y < m_pBaseModel->m_aabb.min.y) probe.y = m_pBaseModel->m_aabb.min.y; ground = probe;
				ball.m_pos.y = ground.y + ball.m_radius;
				ball.m_state = 3;
			}
			ball.m_eventPreDataIndex[4] = ball.m_preDataCount;
		}
	}

	switch (ball.m_state) {
	case 1:
		BounceProcess(elapsed);
		break;
	case 2:
		SpinProcess(elapsed);
		break;
	case 3:
		RollProcess(elapsed);
		break;
	case 4:
		HoleProcess(elapsed);
		break;
	}
}

// quadtree.cpp:3351
bool CQuadTree::IsValid(const WVector &pos) {
	return true;
}

// quadtree.cpp:3409
WVector CQuadTree::ApplyForce(void) {
	WVector force(0.0f, 0.0f, 0.0f);

	if (!(WAbsBySign(GolfBall().m_curve) < g_EPSILON) && GolfBall().m_eventPreDataIndex[9] < 0) {
		force = (GolfClub().GetCurve() * GolfBall().m_curve * 0.75f) * WVector(GolfBall().m_velocity.z * -1.0f, 0.0f, GolfBall().m_velocity.x).Normalize();
	} else if (!(WAbsBySign(GolfBall().m_curve) < g_EPSILON) && GolfBall().m_eventPreDataIndex[12] < 0) {
		force = (GolfClub().GetCurve() * GolfBall().m_curve * 0.75f) * WVector(GolfBall().m_velocity.z * -1.0f, 0.0f, GolfBall().m_velocity.x).Normalize();
	}

	if (!((GolfBall().m_flags & 0x20) && GolfBall().m_eventPreDataIndex[9] < 0)
			&& !((GolfBall().m_flags & 0x40) && GolfBall().m_eventPreDataIndex[12] < 0)) {
		if (m_applyForce) {
			if (GolfBall().m_flags & 0x40)
				force += Wind().GetWind(GolfBall().m_pos) * 0.01f;
			else
				force += Wind().GetWind(GolfBall().m_pos) * 0.02f;
		}

		CGolfBall &ball = GolfBall();

		force.y -= m_gravity * ball.m_mass * 34.295296f;
		if (!(WAbsBySign(GolfBall().m_spin) < g_EPSILON))
			force.y += GolfClub().GetSpin() * GolfBall().m_spin * 3.0f;
		force -= (GolfBall().m_velocity.Magnitude() * 0.00008f) * GolfBall().m_velocity;
	}
	return force;
}

// quadtree.cpp:3452
void CQuadTree::SetPenaltyPos(const Waabb &aabb) {
	if (GolfBall().m_lie == 1) {
		m_penaltyPos = GolfBall().PreData(0)->m_pos;
		return;
	}

	WVector dir = GolfBall().PreData(0)->m_pos - GolfBall().m_pos;

	dir.y = 0.0f; dir.Normalize();

	// one 16-unit step back along the shot line, then forward
	dir *= 16.0f;

	// candidate drop points: one searched backward, one forward
	WVector found[2];
	WVector pos;
	int count = 0;
	for (int i = 0; i < 2; i++) {
		found[count] = GolfBall().m_pos;
		while (true) {
			found[count] += dir;

			WVector probe = found[count]; GetMeshList(found[count]); int hit; probe = ColTest(probe, false, &hit, true); if (probe.y < m_pBaseModel->m_aabb.min.y) probe.y = m_pBaseModel->m_aabb.min.y; pos = probe;

			if (GetMeshIndex(pos, false) == -1)
				break;

			WVector ground = ColTest(pos, true, 0, true);

			Waabb water;
			if (!IsFallenIntoWater(pos, &water, true) && ground.y > aabb.max.y && !m_pPolySoup->IsOutOfBound(pos)) {
				float slopeLimit = (float)cos(20.0 * 0.017453292f);
				if (GetMeshNormal(pos).y > slopeLimit) {
					// the ground must stay flat all around: 18 directions, 20 degrees apart
					int step = 0;
					for (float angle = 0.0f; step < 18; angle += 20.0f * 0.017453292f) {
						WVector around = dir * RotMat(angle, 1) + found[count];
						WVector p = around; GetMeshList(around); int aroundHit; p = ColTest(p, false, &aroundHit, true); if (p.y < m_pBaseModel->m_aabb.min.y) p.y = m_pBaseModel->m_aabb.min.y; pos = p;
						if (GetMeshNormal(pos).y < slopeLimit)
							break;
						step++;
					}

					if (!OverlappedWithObject(pos, 1.6f) && step == 18) {
						count++;
						break;
					}
				}
			}
		}

		dir *= -1.0f;
	}

	switch (count) {
	case 0:
		m_penaltyPos = GolfBall().PreData(0)->m_pos;
		break;
	case 1:
		m_penaltyPos = found[0];
		{ WVector probe = m_penaltyPos; GetMeshList(m_penaltyPos); int hit; probe = ColTest(probe, false, &hit, true); if (probe.y < m_pBaseModel->m_aabb.min.y) probe.y = m_pBaseModel->m_aabb.min.y; m_penaltyPos = probe; }
		m_penaltyPos.y += GolfBall().m_radius;
		break;
	case 2:
		{
			WVector toFirst = GetHoleCupPos() - found[0];
			WVector toSecond = GetHoleCupPos() - found[1];
			m_penaltyPos = WVectorLen(toFirst) > WVectorLen(toSecond) ? found[0] : found[1];
		}
		{ WVector probe = m_penaltyPos; GetMeshList(m_penaltyPos); int hit; probe = ColTest(probe, false, &hit, true); if (probe.y < m_pBaseModel->m_aabb.min.y) probe.y = m_pBaseModel->m_aabb.min.y; m_penaltyPos = probe; }
		m_penaltyPos.y += GolfBall().m_radius;
		break;
	default:
		m_penaltyPos = GolfBall().PreData(0)->m_pos;
		break;
	}
}

// quadtree.cpp:3541
// One frame of the flying ball: forces, spin, the ground, water, the objects
// it hits and the contact it slides along.
void CQuadTree::BounceProcess(float elapsed) {
	bool bSpecialBall = false;

	CGolfBall &ball = GolfBall();
	CGolfBall::sExpected *pData = ball.PreDataToWrite(ball.m_preDataCount);

	if ((GolfBall().m_flags & 0x40) && GolfBall().m_eventPreDataIndex[1] >= 0 && GolfBall().m_eventPreDataIndex[1] + 60 > GolfBall().m_preDataCount)
		return;

	ball.m_velocity += ApplyForce() / ball.m_mass * elapsed;

	if (GolfBall().m_flags & 0x40)
		GolfBall();     // the original reaches the ball here and does nothing with it

	if (ball.m_lastContactIndex != -1) {
		// Sliding along the last contact: keep only the ball.m_velocity along its face.
		WVector normal = m_pPolySoup->m_models[ball.m_lastContactId]->m_pTriangles[ball.m_lastContactIndex].m_plane.normal;

		ball.m_velocity -= normal * WDotProduct(normal, ball.m_velocity);

		if (ball.m_velocity.y > 0.0f) {
			ball.m_velocity.Reset();
			ball.m_state = 0;
		}
	}

	ball.m_rotation.x += elapsed * ball.m_curve * 12.566371f;
	if (GolfBall().m_flags & 3)
		ball.m_rotation.y += elapsed * ball.m_lieAngle * 62.831856f;
	else
		ball.m_rotation.y += elapsed * ball.m_spin * 25.132742f;

	if (ball.m_groundArea >= 0 && WDotProduct(ball.PreDataToWrite(ball.m_preDataCount - 1)->m_contactVector, ball.m_velocity) < 0.0f) {
		// On the ground: drop the ball.m_velocity into the ground and put the ball on it.
		ball.m_velocity -= ball.PreDataToWrite(ball.m_preDataCount - 1)->m_contactVector * WDotProduct(ball.m_velocity, ball.PreDataToWrite(ball.m_preDataCount - 1)->m_contactVector);
		{ WVector pos = ball.m_pos; CQuadTree &pvs = GetPVS(); pvs.GetMeshList(pos); int hit; WVector ground = pvs.ColTest(pos, false, &hit, true); if (ground.y < pvs.m_pBaseModel->m_aabb.min.y) ground.y = pvs.m_pBaseModel->m_aabb.min.y; ball.m_pos = ground; }

		ball.m_pos.y += ball.m_radius;
		if (ball.m_flags & 2) {
			MakeAfterBurner();
		} else {
			ball.m_eventPreDataIndex[7] = ball.m_preDataCount;
			ball.m_state = 3;
		}

		const char *desc = m_pPolySoup->GetProperty(m_pBaseModel->m_pTriangles[ball.m_groundArea].m_propIndex).m_desc;
		float speed = (ball.m_spinImpulse + ball.m_velocity).Magnitude();

		if (desc[0] && speed > 20.0f)
			strcpy(pData->m_sound, desc);

		if (GetGroundType(ball.m_pos, true, 0) == 2 && speed > 20.0f)
			ball.m_eventPreDataIndex[5] = ball.m_preDataCount;

		switch (ball.m_state) {
		case 2:
			SpinProcess(elapsed);
			return;
		case 3:
			RollProcess(elapsed);
			return;
		}
		return;
	}

	if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000013)
		bSpecialBall = true;

	WVector move = ball.m_velocity * elapsed;
	float t = CheckCollision(ball.m_pos, move, &ball.m_groundArea, false);

	Waabb water;
	WVector dest = move + ball.m_pos;

	if (move.y < 0.0f && IsFallenIntoWater(dest, &water, false)) {
		float waterT = (ball.m_pos.y - water.max.y) / move.y * -1.0f;
		if (waterT < t) {
			ball.m_pos += move * waterT;
			if (ball.m_eventPreDataIndex[0] < 0)
				ball.m_eventPreDataIndex[0] = ball.m_preDataCount;
			ball.m_flags &= ~0x20;

			float angle = (float)acos(Between(0.0f, ball.m_velocity.y / ball.m_velocity.Magnitude() * -1.0f, 1.0f));
			if (!(ball.m_flags & 0x50) && angle < 1.3962634f && ball.m_velocity.y < -10.0f
					&& WVector(ball.m_velocity.x, 0.0f, ball.m_velocity.z).Magnitude() > 10.0f
					&& (angle > 1.0471976f || (ball.m_flags & 1))) {
				// Skipping on the water.
				m_bounceCount++;
				ball.m_lie = 0;
				ball.m_velocity *= 1.0f - ball.m_lieAngle * 0.1f;
				ball.m_velocity.y = -((ball.m_lieAngle * 0.1f + 0.75f) * ball.m_velocity.y);
				pData->m_groundType = 4;
				pData->m_groundSubType = 4;
				pData->m_contactVector = WVector::UNIT_POS_Y;
				strcpy(pData->m_sound, "\xc3\xe6\xb5\xb9_water");
				return;
			}

			ball.m_velocity.Reset();
			ball.m_state = 0;
			ball.m_landed = true;
			pData->m_groundSubType = 4;
			strcpy(pData->m_sound, "\xc3\xe6\xb5\xb9_water");
			SetPenaltyPos(water);
			return;
		}
		GolfBall().m_lie = 0;
	}

	if (t < 1.0f) {
		// The ball hits the ground.
		WPolySoup::sTriangle *pTri = &m_pBaseModel->m_pTriangles[ball.m_groundArea];
		WVector normal;
		if (pTri->m_edgeHit) {
			// An edge hit: the normal points from the edge to the ball.
			normal = intersection::closestPointOnTriangle(ball.m_pos + move * t, pTri->m_vertex[0], pTri->m_vertex[1], pTri->m_vertex[2]);
			normal = ball.m_pos + move * t - normal;
			normal.Normalize();
		} else {
			normal = pTri->m_plane.normal;
			if (pTri->m_doubleSided && WDotProduct(normal, move) > 0.0f)
				normal.Negate();
		}

		ball.m_pos += move * t;
		ball.m_lastEventPreDataIndex = pTri->m_propIndex;
		BounceResponse(normal);

		CVectorSlideTestContainer *pContainer = (CVectorSlideTestContainer *)WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0xA2);
		if (pContainer && pContainer->m_newTypeVectorSlide) {
			if (WDotProduct(normal, WVector(0.0f, 1.0f, 0.0f)) < cos(45.0f * g_DEGTORAD) && ball.m_groundType == 8)
				ball.m_velocity.y = 0.0f;
		}

		if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel == 0)
			pData->m_groundSubType = cos(45.0f * g_DEGTORAD) < normal.y ? 1 : 2;
		else
			pData->m_groundSubType = 3;

		pData->m_contactVector = normal;
		pData->m_groundType = 1;

		if (m_pPolySoup->GetProperty(ball.m_lastEventPreDataIndex).m_weatherBounceType == 8) {
			float scale = (500.0f < m_pPolySoup->GetProperty(GolfBall().m_lastEventPreDataIndex).m_maxPowerOnGround[1] ? 500.0f
				: m_pPolySoup->GetProperty(GolfBall().m_lastEventPreDataIndex).m_maxPowerOnGround[1]) * 0.01f;

			CVectorSlideTestContainer *pSlide = (CVectorSlideTestContainer *)WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0xA2);
			if (!pSlide)
				return;

			MakeVectorSlide(scale, (float)pSlide->m_limitSpeed, (float)pSlide->m_boundHeight, pSlide->m_limitVectorSlideCount);
			return;
		}

		int texId = m_pBaseModel->m_pTriangles[ball.m_groundArea].m_texIndex & 0x7FF; m_pPolySoup->m_texIndexMap.find(texId); strcpy(g_colTex[g_colNum], m_pPolySoup->m_pTextures[m_pPolySoup->m_texIndexMap[texId]].m_name);
		g_colNum++;
		if (g_colNum >= 16)
			g_colNum = 15;

		m_colData.m_frame = ball.m_preDataCount;
		m_colData.m_objIndex = -1;
		m_colData.m_triIndex = ball.m_groundArea;
		m_colData.m_objType = 0;
		m_colData.m_point = ball.m_pos;
		m_colData.m_normal = ball.m_velocity;
		m_colList.push_back(m_colData);
		return;
	}

	WPlane plane;
	float objT = IntersectObj(ball.m_pos, move, &plane);

	if (objT < 1.0f && !bSpecialBall) {
		if (m_colData.m_objIndex >= 0 && m_pPolySoup->GetProperty(ball.m_lastEventPreDataIndex).m_weatherBounceType == 0xC) {
			// A launcher object: its name holds the launch speeds.
			WPolySoup::sModel *pModel = m_pPolySoup->m_models[m_colData.m_objIndex];

			ball.m_pos += move;

			pData->m_groundType = 5;
			float forward, up;
			if (sscanf(pModel->m_baseName, "%f %f", &forward, &up) < 2)
				return;

			float vz = ball.m_velocity.z;
			ball.m_velocity = pModel->m_matrix.ya * (fabs(ball.m_velocity.y) + up);
			ball.m_velocity += pModel->m_matrix.za * (fabs(vz) + forward);
			return;
		} else if (m_colData.m_objIndex >= 0 && m_pPolySoup->GetProperty(ball.m_lastEventPreDataIndex).m_weatherBounceType == 0xD) {
			WPolySoup::sModel *pModel = m_pPolySoup->m_models[m_colData.m_objIndex];

			ball.m_pos += move;
			pData->m_groundType = 5;

			float up, forward;
			if (sscanf(pModel->m_baseName, "%f %f", &up, &forward) < 2)
				return;

			float vy = ball.m_velocity.y;
			float vz = ball.m_velocity.z;

			up *= 0.33333334f;
			forward += up;
			if (up < 0.1f)
				up = 1.0f;

			ball.m_velocity = pModel->m_matrix.ya * (fabs(vy) + up);
			ball.m_velocity += pModel->m_matrix.za * (fabs(vz) + forward);
			return;
		}

		sColResponse response(GolfBall(), m_colData.m_point, m_colData.m_normal);
		ObjResponse(move, objT, plane.normal, response);
		CommitResponse(GolfBall(), response, GolfBall().m_preDataCount);

		pData->m_groundSubType = 2;
		pData->m_groundType = 2;

		ball.m_spin = ball.m_curve = 0.0f;
		ball.m_flags = 0;

		CVectorSlideTestContainer *pContainer = 0;
		WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0xA2, pContainer);
		if (pContainer && pContainer->m_newTypeVectorSlide) {
			if (WDotProduct(plane.normal, WVector(0.0f, 1.0f, 0.0f)) < cos(45.0f * g_DEGTORAD) && ball.m_groundType == 8)
				ball.m_velocity.y = 0.0f;
		}
		return;
	}

	ball.m_pos += move;
	if (ball.m_lastContactIndex != -1) {
		// Still above the face slid along?
		WVector tri[3];
		for (int i = 0; i < 3; i++) {
			tri[i] = m_pPolySoup->m_models[ball.m_lastContactId]->m_pTriangles[ball.m_lastContactIndex].m_vertex[i];
			tri[i].y = ball.m_pos.y;
		}

		if (!PointInTriangle(ball.m_pos, tri[0], tri[1], tri[2])) {
			ball.m_lastContactId = -1;
			ball.m_lastContactIndex = -1;
		}
	}
}

// quadtree.cpp:3888
float CQuadTree::CheckCollision(const WVector &pos, const WVector &dir, int *pIndex, bool bAll) {
	float t;
	float t2;
	WPlane plane;
	WVector vtx[3];
	WVector start;
	WVector d;
	WVector hit;

	if (IsLocalContent((localContentType_t)0x3A)
			&& WSingleton<CSharedDoc>::Instance()->m_roomType != 0xB
			&& WSingleton<CSharedDoc>::Instance()->m_roomType != 0xC) {
		if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x800)
			m_holeCupRadius = GolfBall().m_radius * 6.0f;
		else
			m_holeCupRadius = GolfBall().m_radius * 3.0f;
	}

	CGolfBall &ball = GolfBall();

	WVector boxMin = pos - WVector(Abs(dir.x), Abs(dir.y), Abs(dir.z)) + WVector::ONE * ball.m_radius;
	WVector boxMax = WVector(Abs(dir.x), Abs(dir.y), Abs(dir.z)) + pos + WVector::ONE * ball.m_radius;
	GetMeshList(Waabb(boxMin, boxMax));

	t = 1.0f;
	if (pIndex)
		*pIndex = -1;

	for (int i = 0; i < m_meshNum; i++) {
		int index = m_meshList[i];

		for (int k = 0; k < 3; k++)
			vtx[k] = m_pBaseModel->m_pTriangles[index].m_vertex[k];

		m_pBaseModel->m_pTriangles[index].m_edgeHit = false;

		plane = m_pBaseModel->m_pTriangles[index].m_plane;
		if (m_pBaseModel->m_pTriangles[index].m_doubleSided && WDotProduct(plane.normal, pos) + plane.dis < 0.0f) {
			plane.normal.Negate();
			plane.dis *= -1.0f;
		}

		start = pos - plane.normal * ball.m_radius;
		bool bothSides = m_pPolySoup->GetProperty(m_pBaseModel->m_pTriangles[index].m_propIndex).m_weatherBounceType == 2;
		t2 = intersection::IntersectPlane(start, dir, plane, bothSides);

		if (t2 < t) {
			hit = start + dir * t2;
			if (!bAll) {
				if ((ball.m_flags & 0x20) && ball.m_eventPreDataIndex[9] < 0 && cos(20.0f * g_DEGTORAD) < plane.normal.y) {
					m_lowestGroundMode = true;
					continue;
				}

				if ((ball.m_flags & 0x40) && ball.m_eventPreDataIndex[12] < 0 && cos(20.0f * g_DEGTORAD) < plane.normal.y) {
					m_lowestGroundMode = true;
					continue;
				}

				d = GetHoleCupPos() - (pos + dir * t2);

				if ((float)sqrt(d.z * d.z + d.x * d.x) < m_holeCupRadius)
					continue;
			}

			if (intersection::SameSide(hit, vtx[0], vtx[1], vtx[2]) && intersection::SameSide(hit, vtx[1], vtx[0], vtx[2]) && intersection::SameSide(hit, vtx[2], vtx[0], vtx[1])) {
				t = t2;
				if (pIndex)
					*pIndex = index;
			} else {
				WVector closest = intersection::closestPointOnTriangle(hit, vtx[0], vtx[1], vtx[2]);
				float t3 = intersection::intersectPolygon(closest, pos, dir, GolfBall().GetRadius());

				if (t3 < t) {
					t = t3;
					m_pBaseModel->m_pTriangles[index].m_edgeHit = true;
					if (pIndex)
						*pIndex = index;
				}
			}
		}
	}

	return t;
}

// quadtree.cpp:3995
void CQuadTree::CalcGreenPlane(void) {
	if (!WisEqual(m_greenBox.max, m_greenBox.min, 1e-5f)) {
		WVector delta = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos - m_greenCenter;

		m_greenPlane.normal = WVector::UNIT_POS_Y + WVector(delta.x / (m_greenBox.max.x - m_greenBox.min.x), 0.0f, delta.z / (m_greenBox.max.z - m_greenBox.min.z)) * m_greenSlope;
		m_greenPlane.normal.Normalize();

		m_greenPlane.d = WDotProduct(m_greenPlane.normal * -1.0f, m_greenCenter);
	}
}

// quadtree.cpp:4008
float CQuadTree::GetGreenPos(const WVector &pos) {
	if (!WisEqual(m_greenBox.max, m_greenBox.min, 1e-5f) && GetGroundType(pos, true, 0) == 2) {
		// the height of the green's plane under pos
		return (m_greenPlane.c * pos.z + m_greenPlane.a * pos.x + m_greenPlane.d) / m_greenPlane.b * -1.0f;
	}

	return pos.y;
}

// quadtree.cpp:4019
WVector CQuadTree::GetFriction(void) {
	GolfBall().m_lastEventPreDataIndex = m_pBaseModel->m_pTriangles[GolfBall().m_groundArea].m_propIndex;

	WVector normal;
	if (!WisEqual(m_greenBox.max, m_greenBox.min, 1e-5f) && GetGroundType(GolfBall().m_pos, true, 0) == 2) {
		normal = m_greenPlane.normal;
	} else {
		normal = m_pBaseModel->m_pTriangles[GolfBall().m_groundArea].m_plane.normal;
		if (normal.y < 0.0f && m_pBaseModel->m_pTriangles[GolfBall().m_groundArea].m_doubleSided)
			normal.Negate();
	}

	float cosAngle = WDotProduct(normal, WVector::UNIT_POS_Y);

	float clamped = cosAngle < -1.0f ? -1.0f : (cosAngle > 1.0f ? 1.0f : cosAngle);

	// The slope pulls the ball down along the ground.
	WVector slope = WVector::UNIT_NEG_Y + normal * cosAngle;
	slope.Normalize();
	WVector friction = slope * ((float)sin(acos(clamped)) * m_slopeFactor * GolfBall().GetMass() * m_gravity * 34.295296f);

	// The rolling friction works against the ball's velocity.
	WVector roll = GolfBall().m_velocity * -1.0f;
	roll.Normalize();
	float rollCoeff = GetRollCoeff(GolfBall().m_lastEventPreDataIndex);
	roll *= GetWeatherRollCoeff(GolfBall().m_lastEventPreDataIndex) * m_rollFactor * GolfBall().GetMass() * GetGravity() * rollCoeff * cosAngle;
	friction += roll;

	if (GolfBall().m_velocity.Magnitude() < roll.Magnitude() / GolfBall().GetMass() * 0.02f)
		GolfBall().m_state = 0;

	return friction;
}

// quadtree.cpp:4066
void CQuadTree::SpinProcess(float elapsed) {
	CGolfBall &ball = GolfBall();

	ball.m_rotation.y += elapsed * ball.m_lieAngle * 62.831856f;
	if (ball.m_preDataCount == ball.m_eventPreDataIndex[6] + 50) {
		ball.m_velocity += ball.m_spinImpulse;
		ball.m_eventPreDataIndex[7] = ball.m_preDataCount;
		ball.m_state = 3;

		RollProcess(elapsed);
	}
}

// quadtree.cpp:4081
float CQuadTree::IntersectRollingBallGround(const WVector &pos, const WVector &dir, const float &radius, int &index, WVector &normal, bool bRolling, bool bWallsOnly, bool bothSides) {
	GetMeshList(pos, dir);

	float t = 1.0f;
	for (int i = 0; i < m_meshNum; i++) {
		WPolySoup::sTriangle *pTri = &m_pBaseModel->m_pTriangles[m_meshList[i]];

		WPlane plane;
		plane = pTri->m_plane;
		if (pTri->m_doubleSided && WDotProduct(plane.normal, pos) + plane.dis < 0.0f) {
			plane.normal.Negate();
			plane.dis *= -1.0f;
		}

		// Walls only: skip the floors, the faces turned away and the faces below the ball.
		if (bRolling && bWallsOnly && (Abs(plane.normal.y) > 0.5f || WDotProduct(plane.normal, dir) > 0.0f
			|| (pTri->m_vertex[0].y < pos.y && pTri->m_vertex[1].y < pos.y && pTri->m_vertex[2].y < pos.y)))
			continue;

		normal = plane.normal;

		WVector start = pos - plane.normal * radius;
		t = intersection::IntersectPlane(start, dir, plane, bothSides);

		if (t < 1.0f) {
			WVector hit = start + dir * t;
			if (intersection::SameSide(hit, pTri->m_vertex[0], pTri->m_vertex[1], pTri->m_vertex[2])
				&& intersection::SameSide(hit, pTri->m_vertex[1], pTri->m_vertex[0], pTri->m_vertex[2])
				&& intersection::SameSide(hit, pTri->m_vertex[2], pTri->m_vertex[0], pTri->m_vertex[1])) {
				index = i;
				break;
			}

			WVector closest = intersection::closestPointOnTriangle(start + dir * t,pTri->m_vertex[0], pTri->m_vertex[1], pTri->m_vertex[2]);
			t = intersection::intersectPolygon(closest, pos, dir, GolfBall().GetRadius());

			if (t < 1.0f) {
				index = i;
				break;
			}
		}
	}

	return t;
}

// quadtree.cpp:4133
void CQuadTree::RollingBallGroundResponse(CGolfBall &ball, float t, const WVector &dir, int index, const WVector &normal) {
	if (IsLocalContent((localContentType_t)0x6B)) {
		ball.m_curve = ball.m_spin = 0.0f;
		ball.m_pos += dir * t;
		ball.m_pos.y = GetHighestGround(ball.m_pos, &ball.m_groundArea)
			+ ball.m_radius;
		ball.m_lastEventPreDataIndex = m_pBaseModel->m_pTriangles[index].m_propIndex;

		ReflectVector(normal, ball.m_velocity);
		float coeff = GetBounceCoeff(ball.m_lastEventPreDataIndex) * GetWeatherBounceCoeff(ball.m_lastEventPreDataIndex);
		ball.m_velocity *= coeff;
		ball.PreDataToWrite(ball.m_preDataCount)->m_contactVector = normal;
		return;
	} else {
		ball.m_spin = 0.0f;
		ball.m_curve = 0.0f;

		WVector move = dir * t;

		ball.m_pos.x += move.x;
		ball.m_pos.z += move.z;
		ball.m_pos.y = ball.GetRadius() + ball.m_pos.y + move.y;
		ball.m_lastEventPreDataIndex = m_pBaseModel->m_pTriangles[index].m_propIndex;
		ball.m_velocity *= GetBounceCoeff(ball.m_lastEventPreDataIndex) * GetWeatherBounceCoeff(ball.m_lastEventPreDataIndex) * -1.0f;

		strcpy(ball.PreDataToWrite(ball.m_preDataCount)->m_sound, "\xc3\xe6\xb5\xb9_wood");
	}
	ball.PreDataToWrite(ball.m_preDataCount)->m_contactVector = normal;
}

// quadtree.cpp:4170
// One frame of the rolling ball: friction, the cup, the objects and the
// walls it rolls into, the ground under it and the pull of the cup's rim.
void CQuadTree::RollProcess(float elapsed) {
	if (IsLocalContent((localContentType_t)0x3A)
			&& WSingleton<CSharedDoc>::Instance()->m_roomType != 0xB
			&& WSingleton<CSharedDoc>::Instance()->m_roomType != 0xC) {
		if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x800)
			m_holeCupRadius = GolfBall().m_radius * 6.0f;
		else
			m_holeCupRadius = GolfBall().m_radius * 3.0f;
	}

	CGolfBall &ball = GolfBall();

	ball.m_groundArea = GetMeshIndex(ball.m_pos, false);
	WVector &velocity = ball.m_velocity;

	if (ball.m_groundArea == -1) {
		// Off the course: back off a little and stop when slow enough.
		velocity = ball.m_velocity * -1.0f * (1.0f / ball.m_velocity.Magnitude()) * 0.02f;
		ball.m_pos += ball.m_velocity * 0.02f;
		if (ball.m_velocity.Magnitude() < 0.02f / ball.m_mass) {
			velocity.Reset();
			ball.m_landed = true;
			ball.m_state = 0;
		}
		return;
	}

	ball.m_velocity += GetFriction() / ball.m_mass * elapsed;

	if (ball.m_state == 0) {
		WVector d;
		d = ball.m_pos - GetHoleCupPos();

		if ((float)sqrt(d.z * d.z + d.x * d.x) < m_holeCupRadius) {
			ball.m_pos = GetHoleCupPos();
			ball.m_pos.y = m_cupBottom + ball.m_radius;
			HoleIn();
			return;
		}

		velocity.Reset();

		Waabb water;
		if (IsFallenIntoWater(ball.m_pos, &water, false)) {
			ball.m_landed = true;
			SetPenaltyPos(water);
			return;
		}

		if (ball.m_flags & 1)
			MakeAfterBurner();
		return;
	}

	WVector move;
	move = velocity * elapsed;
	WPlane plane;
	float t = IntersectObj(ball.m_pos, move, &plane);

	float objT = 0.0f;
	sColResponse response(GolfBall(), m_colData.m_point, m_colData.m_normal);
	WVector dest;
	if (t < 1.0f) {
		ObjResponse(move, t, plane.normal, response);
		if (response.m_flagB4)
			objT = response.m_ball.GetRadius() / move.Magnitude() + t;
		dest = response.m_ball.m_pos;
		move = dest - ball.m_pos;
	} else {
		dest = move + ball.m_pos;
	}

	float ground = GetHighestGround(dest, &ball.m_groundArea);
	if (ball.m_groundArea == -1) {
		dest.y = -100000.0f;
	} else {
		dest.y = ground + ball.m_radius;
		ball.m_rotation.y -= (dest - ball.m_pos).Magnitude() / ball.m_radius * 0.3f;
	}

	int index = -1;
	float radius = GolfBall().m_radius;
	WVector normal;
	float groundT = IntersectRollingBallGround(ball.m_pos, move, radius, index, normal, true, true, false);
	if (groundT < 1.0f) {
		if (response.m_flagB4 && t / objT < groundT) {
			response.m_ball.m_pos = GolfBall().m_pos;
			CommitResponse(GolfBall(), response, GolfBall().m_preDataCount);
		}
		RollingBallGroundResponse(ball, groundT, move, index, normal);
		return;
	}

	if (t < 1.0f)
		CommitResponse(GolfBall(), response, GolfBall().m_preDataCount);

	if (move.y + ball.m_pos.y > ball.m_radius * 0.5f + dest.y) {
		// The ground falls away: the ball flies again.
		ball.m_pos += move;
		ball.m_curve = ball.m_spin = 0.0f;
		ball.m_state = 1;
		ball.m_groundArea = -1;
		if (ball.m_eventPreDataIndex[0] < 0)
			ball.m_eventPreDataIndex[0] = ball.m_preDataCount;
	} else {
		if (ball.PreData(ball.m_preDataCount - 1)->m_contactType == 3) {
			move = dest - ball.PreData(ball.m_preDataCount - 1)->m_pos;
			float len = (float)sqrt(move.z * move.z + move.x * move.x);
			if (WisZero((float)sqrt(move.z * move.z + move.x * move.x), 1e-5f)) {
				velocity.Reset();
				ball.m_state = 0;
				if (ball.m_flags & 1)
					MakeAfterBurner();
			} else if (Abs(move.y) / move.Magnitude() < (float)sin(60.0 * 0.017453292f)) {
				ball.m_velocity.y = (float)sqrt(ball.m_velocity.x * ball.m_velocity.x + ball.m_velocity.z * ball.m_velocity.z) / len * move.y;
			}
		}
		ball.m_pos = dest;
	}

	// Near the cup's rim: pull the ball in for at most 75 frames.
	if (!(m_holeCupRadius + ball.m_radius < m_cupApproach.z) && (float)m_cupRollCount < 75.0f) {
		m_cupRollCount++;
		move = ball.m_pos - GetHoleCupPos();
		if (WDotProduct(WVector(move.x, 0.0f, move.z), velocity) > 0.0f) {
			WVector dir = GetHoleCupPos() - ball.m_pos;
			dir.y = 0.0f;
			dir.Normalize();
			velocity += dir * 80.0f * elapsed;
		}
	}
}

// quadtree.cpp:4379
// The ball hits the ground (normal): picks the bounce sound, ends the flight
// when the ball sticks or the bounce is too flat, otherwise reflects the
// velocity with the ground's bounce coefficients.
void CQuadTree::BounceResponse(const WVector &normal) {
	const float kDegToRad = 0.017453292f;
	CGolfBall &ball = GolfBall();
	CGolfBall::sExpected *pData = ball.PreDataToWrite(ball.m_preDataCount);

	const char *pSound = m_pPolySoup->GetProperty(ball.m_lastEventPreDataIndex).m_name;

	if (*pSound) {
		unsigned long ballId = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId;
		if (ballId == 0x14000005 || ballId == 0x14000080) {
			strcpy(pData->m_sound, "wb_bounce");
		} else if (ballId == 0x1400001D) {
			strcpy(pData->m_sound, "HVA_bnound");
		} else if (ballId == 0x1400001C) {
			strcpy(pData->m_sound, "nagashima_Bounce");
		} else if (ballId == 0x1400000B) {
			strcpy(pData->m_sound, "HG_Bounce_1");
		} else {
			const char *pTexture = GetTextureName(ball.m_pos);

			if (pTexture && (!stricmp(pTexture, "icegrass07_03.dds") || !stricmp(pTexture, "igrass03.dds")
				|| !stricmp(pTexture, "igrass06.dds"))) {
				const char *iceSounds[4] = { "ball_ice\xc0\xfd\xba\xae" "3", "ball_ice\xc0\xfd\xba\xae" "3-1", "ball_ice\xc0\xfd\xba\xae" "3-2",
					"ball_ice\xc0\xfd\xba\xae" "3-3" };

				strcpy(pData->m_sound, iceSounds[rand() % 4]);
			} else {
				strcpy(pData->m_sound, pSound);
			}
		}
	}

	bool bBounce = true;
	if (ball.m_eventPreDataIndex[0] < 0) {
		ball.m_eventPreDataIndex[0] = ball.m_preDataCount;
		ball.m_curve = 0.0f;
		if (cos(20 * kDegToRad) < normal.y) {
			if ((ball.m_flags & 0x10) || StuckInBunker(normal)
				|| (ball.m_flags & 0x40))
				bBounce = false;
		} else {
			if ((ball.m_flags & 0x40) && ball.m_eventPreDataIndex[1] >= 0 && ball.m_preDataCount > ball.m_eventPreDataIndex[1])
				ball.m_velocity *= 0.5f;
		}

		ball.m_flags &= ~0x70;

		if (!bBounce) {
			if (ball.m_flags & 3) {
				MakeAfterBurner();
				return;
			}

			if (GetGroundType(ball.m_pos, true, 0) == 3) {
				if (ball.m_velocity.Magnitude() > 150.0f || (ball.m_powerFlags & 0x50))
					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel = 2;
				else
					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel = 1;
			} else {
				PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel = 0;
			}

			ball.m_state = 0;
			ball.m_velocity.x = ball.m_velocity.y = ball.m_velocity.z = 0.0f;
			return;
		}
	}

	ball.m_spin = Min(0.0f, ball.m_spin);

	WVector &velocity = ball.m_velocity;
	float dot = WDotProduct(velocity, normal);
	WVector perpendicular = normal * (dot * -1.0f);
	WVector tangent = perpendicular + velocity;
	float friction = ball.m_lieAngle * 0.1f + 0.3f;
	WVector bounce = perpendicular * (GetWeatherBounceCoeff(ball.m_lastEventPreDataIndex) * m_bounceFactor * GetBounceCoeff(ball.m_lastEventPreDataIndex));
	velocity = tangent * (1.0f - (1.0f - GetWeatherBounceCoeff(ball.m_lastEventPreDataIndex) * m_bounceFactor * GetBounceCoeff(ball.m_lastEventPreDataIndex)) * friction) + bounce;
}

// quadtree.cpp:4503
// Whether the ball rolls into the bank of a bunker (ground types 3, 0xB and
// 0xE) fast enough and head-on enough (within 45 degrees of the normal) to
// stick there.
bool CQuadTree::StuckInBunker(const WVector &normal) {
	const float kDegToRad = 0.017453292f;
	WVector back = GolfBall().m_velocity * -1.0f;
	float cosAngle = back.x * normal.x + back.y * normal.y + back.z * normal.z;
	cosAngle /= GolfBall().m_velocity.Magnitude();
	if ((GetGroundType(GolfBall().m_pos, true, 0) == 3 && GolfBall().m_velocity.Magnitude() > 50.0f && cos(45 * kDegToRad) < cosAngle)
			|| (GetGroundType(GolfBall().m_pos, true, 0) == 0xB && GolfBall().m_velocity.Magnitude() > 50.0f && cos(45 * kDegToRad) < cosAngle)
			|| (GetGroundType(GolfBall().m_pos, true, 0) == 0xE && GolfBall().m_velocity.Magnitude() > 50.0f && cos(45 * kDegToRad) < cosAngle))
		return true;

	return false;
}

// quadtree.cpp:4518
// The ball (response) hits an object at t along dir: picks the sound, then
// passes through a penetrable object, reflects a rolling ball, bounces a
// flying one off the face, or slides it along a too-flat face.
void CQuadTree::ObjResponse(const WVector &dir, float t, const WVector &contactVector, sColResponse &response) {
	const float kDegToRad = 0.017453292f;
	response.m_contactVector = contactVector;

	const char *pSound = m_pPolySoup->GetProperty(response.m_ball.GetAttrIndex()).m_name;

	if (*pSound) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId == 0x14000004)
			sprintf(response.m_sound, "t_Ex4-%d", rand() % 4 + 1);
		else
			strcpy(response.m_sound, pSound);
	}

	if (m_pPolySoup->GetProperty(response.m_ball.GetAttrIndex()).m_penetrable) {
		if (WisEqual(dir, WVector::ZERO, 1e-5f))
			return;

		response.m_flagB4 = true;

		response.m_ball.m_pos += dir * (response.m_ball.GetRadius() / dir.Magnitude() + t);
		response.m_ball.m_velocity *= GetBounceCoeff(response.m_ball.GetAttrIndex());
		response.m_point = response.m_ball.m_pos;
		response.m_normal = response.m_ball.m_velocity;
		response.m_flagB5 = true;
		return;
	}

	if (response.m_ball.m_state == 3) {
		WVector move = dir * t;
		response.m_ball.m_pos += move;
		ReflectVector(contactVector, response.m_ball.m_velocity);
		response.m_ball.m_velocity *= GetBounceCoeff(response.m_ball.GetAttrIndex());

		response.m_point = response.m_ball.m_pos;
		response.m_normal = response.m_ball.m_velocity;
		response.m_flagB5 = true;
		return;
	}

	if (response.m_ball.m_lastContactIndex == -1) {
		WVector &velocity = response.m_ball.m_velocity;
		WVector perpendicular = contactVector * (WDotProduct(velocity, contactVector) * -1.0f);
		WVector tangent = perpendicular + velocity;
		float spin = response.m_ball.GetInitSpin();

		velocity = tangent * (1.0f - (1.0f - GetBounceCoeff(response.m_ball.GetAttrIndex())) * (spin * 0.1f + 0.3f))
			+ perpendicular * GetBounceCoeff(response.m_ball.GetAttrIndex());

		if (response.m_ball.GetSpecial() & 0x10)
			velocity = velocity * 0.25f;

		if ((response.m_ball.GetSpecial() & 0x40) && response.m_ball.GetTopFrame() >= 0 && response.m_ball.GetRenderNum() > response.m_ball.GetTopFrame())
			velocity *= 0.25f;

		WVector move = dir * t;
		response.m_ball.m_pos += move;
		response.m_point = response.m_ball.m_pos;
		response.m_normal = velocity;
		response.m_flagB5 = true;
	} else {
		response.m_ball.m_lastContactId = m_colList.back().m_objIndex;
		response.m_ball.m_lastContactIndex = m_colList.back().m_triIndex;

		WPolySoup::sTriangle *pTri = &m_pPolySoup->m_models[response.m_ball.m_lastContactId]->m_pTriangles[response.m_ball.m_lastContactIndex];

		if (WAbsBySign(pTri->m_plane.normal.y) < g_EPSILON) {
			response.m_ball.m_velocity.x = response.m_ball.m_velocity.y = response.m_ball.m_velocity.z = 0.0f;
			response.m_ball.m_state = 0;
		} else {
			response.m_ball.m_pos.y = (pTri->m_plane.normal.z * response.m_ball.m_pos.z + pTri->m_plane.normal.x * response.m_ball.m_pos.x + pTri->m_plane.dis) / pTri->m_plane.normal.y * -1.0f;
			response.m_ball.m_pos.y += response.m_ball.GetRadius();
		}
	}

	WVector next(response.m_ball.m_velocity.x, response.m_ball.m_velocity.y - GetGravity() * 0.02f, response.m_ball.m_velocity.z);
	if (WDotProduct(next, contactVector) < 0.0f) {
		if (cos(20 * kDegToRad) < contactVector.y) {
			response.m_ball.m_velocity.x = response.m_ball.m_velocity.y = response.m_ball.m_velocity.z = 0.0f;
			response.m_ball.m_state = 0;
			return;
		}

		if (response.m_ball.m_velocity.y > 0.0f)
			response.m_ball.m_velocity.Negate();

		response.m_ball.m_lastContactId = m_colList.back().m_objIndex;
		response.m_ball.m_lastContactIndex = m_colList.back().m_triIndex;
	}
}

// quadtree.cpp:4641
CQuadTree::sColResponse::sColResponse(const CGolfBall &ball, const WVector &point, const WVector &normal)
	: m_ball(ball), m_point(point), m_normal(normal), m_flagB4(false), m_flagB5(false) {
	m_sound[0] = '\0';
}

// quadtree.cpp:4653
CQuadTree::sColResponse::GolfBallClone::GolfBallClone(const CGolfBall &ball)
	: m_pos(ball.m_pos), m_velocity(ball.m_velocity), m_lastContactId(ball.m_lastContactId),
	  m_lastContactIndex(ball.m_lastContactIndex), m_state(ball.m_state), m_pBall(&ball) {
}

// quadtree.cpp:4658
void CQuadTree::CommitResponse(CGolfBall &ball, const sColResponse &response, int index) {
	ball.m_pos = response.m_ball.m_pos;
	ball.m_velocity = response.m_ball.m_velocity;
	ball.m_lastContactId = response.m_ball.m_lastContactId;
	ball.m_lastContactIndex = response.m_ball.m_lastContactIndex;
	ball.m_state = response.m_ball.m_state;

	CGolfBall::sExpected *pData = ball.PreDataToWrite(index);
	pData->m_contactVector = response.m_contactVector;
	strcpy(pData->m_sound, response.m_sound);

	m_colData.m_point = response.m_point;
	m_colData.m_normal = response.m_normal;
	if (response.m_flagB5) {
		m_colList.push_back(m_colData);
	}
}

// quadtree.cpp:4678
void CQuadTree::Display(void) {
	WTVertex vtx[3];
	WTVertex *vl[4] = { &vtx[0], &vtx[1], &vtx[2], 0 };

	if (m_markedTriangle >= 0) {
		for (int i = 0; i < 3; i++) {
			vtx[i].SetPosition(m_pBaseModel->m_pTriangles[m_markedTriangle].m_vertex[i]);
			vtx[i].diffuse = 0xFFFFFFFF;
		}

		g_view->DrawPolygonFan(vl, 0x4300000, 0x400, false);
	}

	if (COption::Instance()->dIsVtxClrTestEnabled()) {
		TextOutA(g_view, 470.0f, 60.0f, m_vtxColorTestMode == 0 ? "<<BASE COLOR MODE>>" : "<<OBJ COLOR MODE>>");
		TextOutA(g_view, 470.0f, 72.0f, MakeStr("Base : %.0f %.0f %.0f", m_baseVtxColor.x, m_baseVtxColor.y, m_baseVtxColor.z));
		TextOutA(g_view, 470.0f, 84.0f, MakeStr("Obj  : %.0f %.0f %.0f", m_objVtxColor.x, m_objVtxColor.y, m_objVtxColor.z));
	}
}

// quadtree.cpp:4782
WVector CQuadTree::closestPointOnTriangle(const WVector &a, const WVector &b, const WVector &c, const WVector &p) const {
	return intersection::closestPointOnTriangle(a, b, c, p);
}

// quadtree.cpp:4787
WVector CQuadTree::closestPointOnLine(const WVector &a, const WVector &b, const WVector &p) const {
	return intersection::closestPointOnLine(a, b, p);
}

// quadtree.cpp:4792
float CQuadTree::intersectPolygon(const WVector &pos, const WVector &center, const WVector &dir) const {
	return intersection::intersectPolygon(pos, center, dir, GolfBall().GetRadius());
}

// quadtree.cpp:4797
// One frame of the ball in the cup: falls under gravity, bounces off the
// cup's bottom and rim (the pole sound), holes it when it reaches the
// bottom, and pushes it back in when it leaves the rim.
void CQuadTree::HoleProcess(float elapsed) {
	if (IsLocalContent((localContentType_t)0x3A)
			&& WSingleton<CSharedDoc>::Instance()->m_roomType != 0xB
			&& WSingleton<CSharedDoc>::Instance()->m_roomType != 0xC) {
		if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x800)
			m_holeCupRadius = GolfBall().m_radius * 6.0f;
		else
			m_holeCupRadius = GolfBall().m_radius * 3.0f;
	}

	CGolfBall &ball = GolfBall();
	CGolfBall::sExpected *pData = ball.PreDataToWrite(ball.m_preDataCount);

	float gravity = elapsed * m_gravity;
	ball.m_velocity.y -= gravity * 34.295296f;
	WVector move;
	move = ball.m_velocity * elapsed;

	// The heights above the cup's bottom now and after the step.
	float height = ball.m_pos.y - ball.m_radius - m_cupBottom;
	float nextHeight = move.y + height;

	float side = nextHeight * height;
	float t;
	if (side < 0.0f && ball.m_velocity.y < 0.0f && !(WAbsBySign(height) < g_EPSILON))
		t = height / (height - nextHeight);
	else
		t = 1.0f;

	move *= t;
	float colT = CheckCollision(ball.m_pos, move, &ball.m_groundArea, false);
	if (ball.m_groundArea >= 0)
		ball.m_lastEventPreDataIndex = m_pBaseModel->m_pTriangles[ball.m_groundArea].m_propIndex;

	// How far the ball is outside the cup's rim, now and after the step.
	WVector d;
	d = ball.m_pos - GetHoleCupPos();
	d.y = 0.0f;
	float over = (float)sqrt(d.x * d.x + d.z * d.z) - (m_holeCupRadius - ball.m_radius);
	d = move + ball.m_pos - GetHoleCupPos();
	d.y = 0.0f;
	float nextOver = (float)sqrt(d.x * d.x + d.z * d.z) - (m_holeCupRadius - ball.m_radius);

	if (ball.PreData(ball.m_preDataCount - 1)->m_contactType == 1 && over > 0.0f) {
		if (nextOver > ball.m_radius) {
			ball.m_pos += move * colT;
			if (colT < 1.0f)
				BounceResponse(GetMeshNormal(ball.m_groundArea));
		} else {
			ball.m_pos += move;

			if (nextOver > 0.0f) {
				// On the rim: the ball rides the rim's edge.
				WVector probe = ball.m_pos; GetMeshList(ball.m_pos); int index; probe = ColTest(probe, false, &index, true); if (probe.y < m_pBaseModel->m_aabb.min.y) probe.y = m_pBaseModel->m_aabb.min.y; WVector ground = probe;
				float rimY = (float)sqrt(ball.m_radius * ball.m_radius - (ball.m_radius - nextOver) * (ball.m_radius - nextOver)) + ground.y;

				WVector dir = ball.m_pos - GetHoleCupPos();
				dir.y = 0.0f;
				dir.Normalize();
				dir *= m_holeCupRadius;
				WVector rim = dir + GetHoleCupPos();

				WVector normal = ball.m_pos - rim;
				dir = normal.Normalize();
				if (ball.m_pos.y < rimY) {
					ball.m_flags &= ~0x73;
					ball.m_curve = ball.m_spin = 0.0f;
					ball.m_pos.y = rimY;
					ball.m_velocity += dir * (Abs(WDotProduct(dir, ball.m_velocity)) * 1.3f);
					pData->m_groundSubType = 0;
					pData->m_groundType = 3;
					pData->m_contactVector = dir;
					strcpy(pData->m_sound, "\xc3\xe6\xb5\xb9_pole");
					return;
				}
			}
		}
	} else if (nextOver < 0.0f) {
		ball.m_pos += move;

		if (side < 0.0f && ball.m_velocity.y < 0.0f)
			HoleIn();
	} else {
		float pointT = IntersectPoint(move);

		if (pointT < colT) {
			ball.m_pos += move * colT;

			if (colT < 1.0f)
				BounceResponse(GetMeshNormal(ball.m_groundArea));
		} else if (pointT < 1.0f) {
			// Hits the pole.
			ball.m_flags &= ~0x73;
			ball.m_pos += move * pointT;
			WVector toCup = GetHoleCupPos() - ball.m_pos;
			toCup.y = 0.0f;
			toCup.Normalize();
			ball.m_velocity = toCup * (ball.m_velocity.Magnitude() * 0.3f);
			pData->m_groundSubType = 0;
			pData->m_groundType = 3;
			pData->m_contactVector = toCup;
			strcpy(pData->m_sound, "\xc3\xe6\xb5\xb9_pole");
		} else {
			ball.m_pos += move;
		}
	}

	d = GetHoleCupPos() - ball.m_pos;
	d.y = 0.0f;

	float dist = (float)sqrt(d.z * d.z + d.x * d.x);
	if (dist > m_holeCupRadius - ball.m_radius) {
		if (ball.m_eventPreDataIndex[7] < 0) {
			if (ball.m_pos.y < GetHoleCupPos().y) {
				if (t < 1.0f) {
					ball.m_pos = GetHoleCupPos();
					ball.m_pos.y = ball.m_radius + m_cupBottom;
					HoleIn();
					return;
				}

				// Below the rim: back against the cup's wall.
				float y = ball.m_pos.y;
				ball.m_pos = GetHoleCupPos() - (m_holeCupRadius - ball.m_radius) * d.Normalize();
				ball.m_pos.y = y;
				ball.m_velocity *= -0.3f;
				ball.m_flags &= ~0x73;
				ball.m_velocity.y *= -1.0f;
				return;
			}

			if (GetHoleCupPos().y + ball.m_radius > ball.m_pos.y) {
				if (dist > m_holeCupRadius) {
					// Out of the cup: back on the ground.
					WVector probe = ball.m_pos; GetMeshList(ball.m_pos); int index; probe = ColTest(probe, false, &index, true); if (probe.y < m_pBaseModel->m_aabb.min.y) probe.y = m_pBaseModel->m_aabb.min.y; WVector ground = probe;
					if (ground.y + ball.m_radius > ball.m_pos.y) {
						ball.m_flags &= ~0x70;
						ball.m_pos.y = ground.y + ball.m_radius;
						ball.m_velocity.y = GetBounceCoeff(m_pBaseModel->m_pTriangles[GetMeshIndex(ball.m_pos, false)].m_propIndex) * ball.m_velocity.y * -1.0f;
						pData->m_groundSubType = 1;
						pData->m_groundType = 1;
						pData->m_contactVector = WVector::UNIT_POS_Y;
						const char *pSound = GetBounceSound(m_pBaseModel->m_pTriangles[GetMeshIndex(ball.m_pos, false)].m_propIndex);
						if (pSound[0] != '\0')
							strcpy(pData->m_sound, pSound);
					}
				} else {
					// On the rim's edge: roll back toward the cup.
					WVector probe = ball.m_pos; GetMeshList(ball.m_pos); int index; probe = ColTest(probe, false, &index, true); if (probe.y < m_pBaseModel->m_aabb.min.y) probe.y = m_pBaseModel->m_aabb.min.y; WVector ground = probe;
					float rimY = (float)sqrt(ball.m_radius * ball.m_radius - (m_holeCupRadius - dist) * (m_holeCupRadius - dist)) + ground.y;
					if (ball.m_pos.y < rimY) {
						WVector dir = ball.m_velocity;
						dir.Normalize();
						float up = -dir.y;
						dir.y = 0.0f;

						WVector toCup = GetHoleCupPos() - ball.m_pos;
						toCup.y = 0.0f;
						toCup.Normalize();

						dir = dir + toCup;
						dir.Normalize();

						dir = dir + toCup;
						dir.Normalize();

						dir.y = up;
						dir.Normalize();

						ball.m_pos.y = rimY;
						ball.m_velocity = (ball.m_velocity.Magnitude() * 0.3f) * dir;
						ball.m_flags &= ~0x73;
					}
				}
			}
		} else {
			// The spin took hold: keep the ball against the cup's wall.
			float y = ball.m_pos.y;
			ball.m_pos = GetHoleCupPos() - (m_holeCupRadius - ball.m_radius) * d.Normalize();
			ball.m_pos.y = y;
			ball.m_velocity = (ball.m_velocity.Magnitude() * 0.3f) * d.Normalize();
		}
	}
}

// quadtree.cpp:5070
// The fraction of the ball's step (dir, in the horizontal plane) at which it
// meets the cup's rim; 1.0 when it misses.
float CQuadTree::IntersectPoint(WVector dir) {
	if (IsLocalContent((localContentType_t)0x3A)
			&& WSingleton<CSharedDoc>::Instance()->m_roomType != 0xB
			&& WSingleton<CSharedDoc>::Instance()->m_roomType != 0xC) {
		if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x800)
			m_holeCupRadius = GolfBall().m_radius * 6.0f;
		else
			m_holeCupRadius = GolfBall().m_radius * 3.0f;
	}

	WVector d = GolfBall().m_pos - GetHoleCupPos();

	float a = dir.x * dir.x + dir.z * dir.z;
	float b = d.x * dir.x + d.z * dir.z;
	float radius = GolfBall().m_radius;
	CGolfBall &ball = GolfBall();
	float disc = b * b - a * ((d.z * d.z + d.x * d.x) - (m_holeCupRadius - ball.m_radius) * (m_holeCupRadius - radius));
	if (disc < 0.0f)
		return 1.0f;

	float root = (float)sqrt(disc);
	float inv = 1.0f / a;
	float t1 = (b * -1.0f - root) * inv;
	float t2 = (root - b) * inv;

	float rim = m_holeCupRadius - GolfBall().m_radius;
	float result = 1.0f;
	float dist = (float)sqrt(d.x * d.x + d.z * d.z);
	if (rim == dist)
		return 0.0f;
	if (rim > dist) {
		if (t2 < 1.0f)
			result = t2;
	} else {
		result = t1;
	}
	return result;
}

// quadtree.cpp:5127
float CQuadTree::GetWeatherRollCoeff(int property) const {
	if (GetWeather() >= 2 && GetWeather() <= 3) {
		switch (m_pPolySoup->GetProperty(property).m_weatherBounceType) {
		case 0:
			return 1.3f;
		case 3:
			return 1.5f;
		case 2:
			return 1.13f;
		case 7:
		case 8:
			break;
		default:
			return 1.8f;
		}
	}
	return 1.0f;
}

// quadtree.cpp:5164
float CQuadTree::GetWeatherBounceCoeff(int property) const {
	if (GetWeather() >= 2 && GetWeather() <= 3) {
		switch (m_pPolySoup->GetProperty(property).m_weatherBounceType) {
		case 0:
		case 1:
		case 4:
		case 5:
		case 6:
			return 0.8f;
		case 3:
			return 0.7f;
		case 2:
			return 0.9f;
		case 7:
		case 8:
			break;
		default:
			return 0.8f;
		}
	}
	return 1.0f;
}

// quadtree.cpp:5201
void CQuadTree::SetCondition(void) {
	switch (GetWeather()) {
	case 0:
	case 1:
		m_condition = 0.0f;
		break;
	case 2:
	case 3:
		switch (GolfBall().m_groundType) {
		case 0:
		case 1:
		case 3:
			m_condition = -2.0f;
			break;
		case 2:
			m_condition = 0.0f;
			break;
		case 4:
			m_condition = -3.0f;
			break;
		default:
			m_condition = -3.0f;
			break;
		}
		break;
	}
}

// quadtree.cpp:5237
float CQuadTree::GetRollCoeff(int property) const {
	return m_pPolySoup->GetProperty(property).m_rollCoeff;
}

// quadtree.cpp:5242
float CQuadTree::GetBounceCoeff(int property) const {
	return m_pPolySoup->GetProperty(property).m_bounceCoeff;
}

// quadtree.cpp:5247
unsigned char CQuadTree::GetGroundType(WVector pos, bool bWater, CGolfBall::eWater *pWater) {
	if (bWater && !GOLFDOC()->m_approachMode) {
		WVector tee = GOLFDOC()->GetHoleData().m_teePos;
		if (Wabs(pos.x - tee.x) < g_CM_TO_WU && Wabs(pos.z - GetTeeShotPos().z) < g_CM_TO_WU)
			return 0;
	}

	GetMeshList(pos);

	int index;
	pos = ColTest(pos, true, &index, true);
	if (index < 0)
		return 10;

	bool inWater = false;
	for (unsigned int i = 0; i < m_waterList.size(); i++) {
		sWater &water = m_waterList[i];

		if (pos.x > water.m_aabb.min.x && pos.x < water.m_aabb.max.x
			&& pos.z > water.m_aabb.min.z && pos.z < water.m_aabb.max.z
			&& pos.y < water.m_aabb.max.y) {
			if (pWater != 0)
				*pWater = (CGolfBall::eWater)water.m_index;

			inWater = true;
		}
	}

	if (inWater)
		return 10;

	return m_pPolySoup->GetProperty(m_pBaseModel->m_pTriangles[index].m_propIndex).m_weatherBounceType;
}

// quadtree.cpp:5285
bool CQuadTree::IsOBArea(const WVector &pos) {
	int index = GetMeshIndex(pos, false);
	if (index < 0)
		return true;
	return m_pPolySoup->GetProperty(m_pBaseModel->m_pTriangles[index].m_propIndex).m_outOfBounds;
}

// quadtree.cpp:5294
bool CQuadTree::IsPenetrable(int property) const {
	return m_pPolySoup->GetProperty(property).m_penetrable;
}

// quadtree.cpp:5299
float CQuadTree::GetMaxPowerOnGround(void) const {
	if (IsLocalContent((localContentType_t)0x2F)) {
		if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000028)
			return (m_pPolySoup->GetProperty(1)
				.m_maxPowerOnGround[PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel] + m_condition) * 0.01f;
	}

	return (m_pPolySoup->GetProperty(m_pBaseModel->m_pTriangles[GolfBall().m_groundArea].m_propIndex)
		.m_maxPowerOnGround[PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel] + m_condition) * 0.01f;
}

// quadtree.cpp:5310
float CQuadTree::GetMinPowerOnGround(void) const {
	if (IsLocalContent((localContentType_t)0x2F)) {
		if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000028)
			return (m_pPolySoup->GetProperty(1)
				.m_minPowerOnGround[PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel] + m_condition) * 0.01f;
	}

	return (m_pPolySoup->GetProperty(m_pBaseModel->m_pTriangles[GolfBall().m_groundArea].m_propIndex)
		.m_minPowerOnGround[PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel] + m_condition) * 0.01f;
}

// quadtree.cpp:5321
float CQuadTree::GetMaxPowerOnGround(const WVector &pos) {
	if (IsLocalContent((localContentType_t)0x2F)) {
		if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000028)
			return (m_pPolySoup->GetProperty(1)
				.m_maxPowerOnGround[PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel] + m_condition) * 0.01f;
	}

	return (m_pPolySoup->GetProperty(m_pBaseModel->m_pTriangles[GetMeshIndex(pos, false)].m_propIndex)
		.m_maxPowerOnGround[PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel] + m_condition) * 0.01f;
}

// quadtree.cpp:5333
float CQuadTree::GetMinPowerOnGround(const WVector &pos) {
	if (IsLocalContent((localContentType_t)0x2F)) {
		if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000028)
			return (m_pPolySoup->GetProperty(1)
				.m_minPowerOnGround[PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel] + m_condition) * 0.01f;
	}

	return (m_pPolySoup->GetProperty(m_pBaseModel->m_pTriangles[GetMeshIndex(pos, false)].m_propIndex)
		.m_minPowerOnGround[PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel] + m_condition) * 0.01f;
}

// quadtree.cpp:5344
const char *CQuadTree::GetBounceSound(int property) const {
	return m_pPolySoup->GetProperty(property).m_name;
}

// quadtree.cpp:5349
const char *CQuadTree::GetRollSound(int property) const {
	return m_pPolySoup->GetProperty(property).m_desc;
}

// quadtree.cpp:5354
const char *CQuadTree::GetStepSound(int property) const {
	return m_pPolySoup->GetProperty(property).m_stepSound;
}

// quadtree.cpp:5359
const char *CQuadTree::GetStepSound(const WVector &pos) {
	return GetStepSound(m_pBaseModel->m_pTriangles[GetMeshIndex(pos, false)].m_propIndex);
}

// quadtree.cpp:5364
const char *CQuadTree::FindTexture(int index) {
	int texIndex = m_pPolySoup->m_texIndexMap[index];
	return m_pPolySoup->m_pTextures[texIndex].m_name;
}

// quadtree.cpp:5373
void CQuadTree::UpdateProperty(void) {
	IFF_STRUCT::sCourse *pCourse = ItemManager()->FindCourse(0x28000000
			| ((GetCurMap() > 0x7F && GetCurMap() != 0xFD) ? GetCurMap() - 0x80 : GetCurMap()));

	if (pCourse && pCourse->m_propertyFile[0] != 0)
		m_pPolySoup->m_pGolfDoc->LoadXml(pCourse->m_propertyFile,
			GOLFDOC()->m_currentHole - 1, true);
	else
		m_pPolySoup->m_pGolfDoc->LoadXml(MakeStr("%s_property.xml", GetCurMapName()),
			GOLFDOC()->m_currentHole - 1, true);

	for (int i = 0; i < m_pPolySoup->m_numTextures; i++)
		m_pPolySoup->m_pTextures[i].m_param = m_pPolySoup->GetPropertyIndex(m_pPolySoup->m_pTextures[i].m_name);
}

// quadtree.cpp:5389
void CQuadTree::ResetWater(void) {
	m_waterList.clear();
	m_waterList.reserve(32);
}

// quadtree.cpp:5401
void CQuadTree::HoleIn(void) {
	CGolfBall &ball = GolfBall();
	CGolfBall::sExpected *pData = ball.PreDataToWrite(ball.m_preDataCount);

	ball.m_velocity.x = ball.m_velocity.y = ball.m_velocity.z = 0.0f;
	ball.m_state = 0;
	ball.SetHoleIn(1);
	pData->m_groundType = 3;
	pData->m_groundSubType = 0;

	unsigned long ballId = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId;
	if (ballId == 0x14000005 || ballId == 0x14000080) {
		strcpy(pData->m_sound, "wb_holein");
	} else {
		strcpy(pData->m_sound, "\xb0\xf8_\xc8\xa6\xc0\xce");
	}
}

// quadtree.cpp:5421
WVector CQuadTree::GetCollisionPoint(const WVector &dir) {
	WVector point = GolfBall().m_pos - dir * GolfBall().GetRadius();
	return point;
}

// quadtree.cpp:5429
bool CQuadTree::OverlappedWithObject(WVector pos, float radius) {
	for (unsigned char i = 0; i < m_objNum; i++) {
		WPolySoup::sModel *pModel = m_pPolySoup->m_models[m_objList[i]];

		// "*nocol" objects do not collide.
		if (pModel->m_reservedByte & 1)
			continue;

		int count = 0;
		w_bound_box *pBox = pModel->m_pPuppet->m_bbList.Find("char0");

		if (pBox) {
			// The oriented boxes "char0", "char1", ... of the object, grown by the radius.
			while (pBox) {
				Wobb obb;
				obb = pBox->bone->GetMatrix();
				obb *= (pBox->info->aabb.max - pBox->info->aabb.min) * 0.5f + WVector::ONE * radius;
				obb.center = pBox->info->spherePivot * pBox->bone->GetMatrix();

				if (obb.IsInclude(pos))
					return true;

				pBox = m_pPolySoup->m_models[m_objList[i]]->m_pPuppet->m_bbList.Find(MakeStr("char%d", ++count));
			}
		} else {
			if (pos.y > pModel->m_sceneAabb.max.y)
				continue;
			if (pModel->m_sceneAabb.min.y - 10.0f > pos.y)
				continue;

			// The scene box grown by the radius horizontally.
			Waabb box(pModel->m_sceneAabb);
			WVector grow = WVector(1.0f, 0.0f, 1.0f) * radius;
			box.min -= grow;
			box.max += grow;

			if (pos.x > box.min.x && pos.x < box.max.x && pos.z > box.min.z && pos.z < box.max.z)
				return true;
		}
	}

	return false;
}

// quadtree.cpp:5478
void CQuadTree::UpdateVtxColor(float elapsed) {
	// "\xc5\xd7\xbd\xba\xc6\xae": the "test" key binding
	if (g_input->GetDown("\xc5\xd7\xbd\xba\xc6\xae", true))
		m_vtxColorTestMode = (m_vtxColorTestMode + 1) % 2;

	WVector color;
	switch (m_vtxColorTestMode) {
	case 0:
		color = m_baseVtxColor;
		break;
	case 1:
		color = m_objVtxColor;
		break;
	}

	// the channels to change: Q red, W green, E blue
	int channels[3] = { 0, 0, 0 };
	if (g_input->Get('Q'))
		channels[0] = 1;
	if (g_input->Get('W'))
		channels[1] = 1;
	if (g_input->Get('E'))
		channels[2] = 1;

	for (int i = 0; i < 3; i++) {
		if (channels[i]) {
			// '=' raises, '-' lowers the channel, 64 per second, within 0-255
			if (g_input->Get('=')) {
				float v = color.p[i] + elapsed * 64.0f;
				color.p[i] = v < 0.0f ? 0.0f : (v > 255.0f ? 255.0f : v);
			}
			if (g_input->Get('-')) {
				float v = color.p[i] - elapsed * 64.0f;
				color.p[i] = v < 0.0f ? 0.0f : (v > 255.0f ? 255.0f : v);
			}
		}
	}

	switch (m_vtxColorTestMode) {
	case 0:
		if (!WisEqual(m_baseVtxColor, color, 1e-5f))
			SetBaseVtxColor(0xFF000000 | (unsigned char)color.x << 16 | (unsigned char)color.y << 8 | (unsigned char)color.z);
		break;
	case 1:
		if (!WisEqual(m_objVtxColor, color, 1e-5f))
			SetObjVtxColor(0xFF000000 | (unsigned char)color.x << 16 | (unsigned char)color.y << 8 | (unsigned char)color.z);
		break;
	}
}

// quadtree.cpp:5539
void CQuadTree::SetBaseVtxColor(unsigned long color) {
	m_baseVtxColor.x = (float)((color >> 16) & 0xFF);
	m_baseVtxColor.y = (float)((color >> 8) & 0xFF);
	m_baseVtxColor.z = (float)(color & 0xFF);

	if (m_pBaseModel != 0) {
		// the base model's puppet takes the colour
		if (m_pBaseModel->m_pPuppet != 0) {
			m_pBaseModel->m_pPuppet->SetVtxColor(color);
			m_pBaseModel->m_pPuppet->UpdateLightSource(&g_lightset, false, 0);
		}
	}
}

// quadtree.cpp:5555
void CQuadTree::SetObjVtxColor(unsigned long color) {
	m_objVtxColor.x = (float)((color >> 16) & 0xFF);
	m_objVtxColor.y = (float)((color >> 8) & 0xFF);
	m_objVtxColor.z = (float)(color & 0xFF);

	if (m_pPolySoup != 0) {
		for (int i = 0; i < m_pPolySoup->m_numObjects; i++) {
			// every object's puppet takes the colour
			if (m_pPolySoup->m_models[i]->m_pPuppet != 0) {
				m_pPolySoup->m_models[i]->m_pPuppet->SetVtxColor(color);
				m_pPolySoup->m_models[i]->m_pPuppet->UpdateLightSource(&g_lightset, false, 0);
			}
		}
	}
}

// quadtree.cpp:5582
// The closest points of a ray and a segment: returns their parameters on the
// ray (x) and the segment (y), and their distance (z).
WVector CQuadTree::DistanceBetweenRayNSegment(const WVector &rayOrig, const WVector &rayDir, const WVector &segOrig, const WVector &segDir) {
	WVector w = rayOrig - segOrig;
	float a = WDotProduct(rayDir, rayDir), b = WDotProduct(rayDir, segDir);
	float c = WDotProduct(segDir, segDir);
	float d = WDotProduct(rayDir, w);
	float e = WDotProduct(segDir, w);

	float D = a * c - b * b;
	float sN, sD;
	float tN, tD;

	if (WAbsBySign(D) < g_EPSILON) {
		sN = 0.0f;
		tN = e;
	} else {
		sN = b * e - c * d;
		tN = a * e - b * d;
	}

	sD = D;
	if (sN < 0.0f) {
		tN = e;
		sN = 0.0f;
		tD = c;
	} else {
		tD = D;
	}

	if (tN < 0.0f) {
		tN = 0.0f;
		if (-d < 0.0f) {
			sN = 0.0f;
		} else {
			sN = -d;
			sD = a;
		}
	} else if (tN > tD) {
		tN = tD;
		if (b - d < 0.0f) {
			sN = 0.0f;
		} else {
			sN = b - d;
			sD = a;
		}
	}

	float sc;
	if (WAbsBySign(sN) < g_EPSILON)
		sc = 0.0f;
	else
		sc = sN / sD;

	float tc;
	if (WAbsBySign(tN) < g_EPSILON)
		tc = 0.0f;
	else
		tc = tN / tD;

	WVector dP = rayOrig + rayDir * sc - (segOrig + segDir * tc);

	return WVector(sc, tc, sqrtf(dP.x * dP.x + dP.y * dP.y + dP.z * dP.z));
}

// quadtree.cpp:5666
bool CQuadTree::SimpleColTest(const WVector &pos, const WVector &dir) {
	GetObjList(pos, dir);

	for (unsigned char i = 0; i < m_objNum; i++) {
		if (RayIntersectAABB(m_pPolySoup->m_models[m_objList[i]]->m_aabb, pos, dir, 0, 0) < 1.0f)
			return true;
	}

	return false;
}

// quadtree.cpp:5684
float CQuadTree::RayIntersectAll(const WVector &pos, const WVector &dir, bool bAll) {
	float t;
	float result = 1.0f;
	int i, j, k;
	WVector vtx[3];
	WPlane plane;
	WPolySoup::sTriangle *pTri;
	GetObjList(pos, dir);

	for (i = 0; i < m_objNum; i++) {
		WPolySoup::sModel *pModel = m_pPolySoup->m_models[m_objList[i]];

		if (pModel->m_pPuppet) {
			int faceNum = pModel->m_pPuppet->GetFaceNum();
			for (j = 0; j < faceNum; j++) {
				if (!bAll && m_pPolySoup->GetProperty(pModel->m_pTriangles[j].m_propIndex).m_penetrable)
					continue;

				for (k = 0; k < 3; k++) vtx[k] = pModel->m_pTriangles[j].m_vertex[k]; plane = pModel->m_pTriangles[j].m_plane;

				if (pModel->m_pTriangles[j].m_doubleSided && WDotProduct(plane.normal, pos) + plane.dis < 0.0f) {
					plane.x = -plane.x;
					plane.y = -plane.y;
					plane.z = -plane.z;
					plane.dis *= -1.0f;
				}

				t = intersection::IntersectPlane(pos, dir, plane, false);
				if (t < result) {
					WVector hit = pos + dir * t;
					if (intersection::SameSide(hit, vtx[0], vtx[1], vtx[2]) && intersection::SameSide(hit, vtx[1], vtx[0], vtx[2]) && intersection::SameSide(hit, vtx[2], vtx[0], vtx[1]))
						result = t;
				}
			}
		}
	}

	GetMeshList(pos, dir);

	for (i = 0; i < m_meshNum; i++) {
		int index = m_meshList[i];

		if (!bAll && m_pPolySoup->GetProperty(m_pBaseModel->m_pTriangles[index].m_propIndex).m_penetrable)
			continue;

		pTri = m_pBaseModel->m_pTriangles;
		plane = pTri[index].m_plane;

		if (pTri[index].m_doubleSided && WDotProduct(plane.normal, pos) + plane.dis < 0.0f) {
			plane.x = -plane.x;
			plane.y = -plane.y;
			plane.z = -plane.z;
			plane.dis *= -1.0f;
		}

		t = intersection::IntersectPlane(pos, dir, plane, false);
		if (t < result) {
			for (k = 0; k < 3; k++)
				vtx[k] = pTri[index].m_vertex[k];

			WVector hit = pos + dir * t;
			if (intersection::SameSide(hit, vtx[0], vtx[1], vtx[2]) && intersection::SameSide(hit, vtx[1], vtx[0], vtx[2]) && intersection::SameSide(hit, vtx[2], vtx[0], vtx[1]))
				result = t;
		}
	}

	return result;
}

// quadtree.cpp:5779
// Clips the polygon against m_decalPlanes[plane] and recurses on the next
// plane; past the sixth, appends what is left to m_pDecalVertices.
void CQuadTree::ClipDecalPolygon(WVector *pVerts, int numVerts, int plane) {
	WVector newVerts[1024];
	float dist[1024];
	char location[1024];   // 0 inside, 1 outside, 2 on the plane
	int count;
	int i;

	if (numVerts > 1022)
		return;

	if (plane == 6) {
		if (numVerts > 2) {
			if (m_decalNumVertices + numVerts >= m_decalMaxVertices)
				numVerts = m_decalMaxVertices - m_decalNumVertices;

			WVector *pDst = m_pDecalVertices + m_decalNumVertices;
			for (i = 0; i < numVerts; i++)
				*pDst++ = *pVerts++;

			m_decalNumVertices += numVerts;
		}
		return;
	}

	const WPlane &clip = m_decalPlanes[plane];
	bool inside = false;

	for (i = 0; i < numVerts; i++) {
		float d = pVerts[i] * clip.normal - clip.dis;
		if (d > 0.03125f) {
			inside = true;
			location[i] = 0;
		} else if (d < -0.03125f) {
			location[i] = 1;
		} else {
			location[i] = 2;
		}
		dist[i] = d;
	}

	if (!inside)
		return;

	location[numVerts] = location[0];
	dist[numVerts] = dist[0];
	pVerts[numVerts] = pVerts[0];

	count = 0;
	for (i = 0; i < numVerts; i++) {
		if (location[i] == 0 || location[i] == 2)
			newVerts[count++] = pVerts[i];

		if (location[i] != 2 && location[i + 1] != 2) {
			if (location[i] != location[i + 1]) {
				float t = dist[i] / (dist[i] - dist[i + 1]);
				WVector dir;
				dir = pVerts[i + 1] - pVerts[i];
				newVerts[count++] = pVerts[i] + dir * t;
			}
		}
	}

	ClipDecalPolygon(newVerts, count, plane + 1);
}

// quadtree.cpp:5876
void CQuadTree::ClipDecalTriangle(const WPolySoup::sTriangle *pTri, WVector normal) {
	if (!pTri->m_doubleSided && WDotProduct(normal, pTri->m_plane.normal) < 0.01f)
		return;

	WVector verts[4];   // room for the vertex a clip adds
	memcpy(verts, pTri->m_vertex, sizeof(pTri->m_vertex));
	ClipDecalPolygon(verts, 3, 0);
}

// A decal's clipped fragments, kept in a ring of 32 by DecalFragments.
struct sDecalFragments {
	bool m_used;                    // 0x0
	WVector m_box[4];               // 0x4, DecalFragments' first argument
	WVector m_normal;               // 0x34
	int m_numVertices;              // 0x40
	WVector m_vertices[1024];       // 0x44
	int m_numIndices;               // 0x3044
	unsigned short m_indices[1536]; // 0x3048
};
CHECK_SIZE(sDecalFragments, 0x3C48);
CHECK_FIELD(sDecalFragments, m_normal, 0x34);
CHECK_FIELD(sDecalFragments, m_numVertices, 0x40);
CHECK_FIELD(sDecalFragments, m_numIndices, 0x3044);
CHECK_FIELD(sDecalFragments, m_indices, 0x3048);

static int data_00af80b4;
static sDecalFragments data_00af80b8[32];

// quadtree.cpp:5917
static sDecalFragments *FindDecalFragments(const WVector *box, const WVector &normal) {
	int i = data_00af80b4;
	int last = i - 1;
	if (last < 0)
		last = 31;

	while (i != last) {
		sDecalFragments *pFrag = &data_00af80b8[i];

		if (pFrag->m_used) {
			if (WisEqual(pFrag->m_box[0].x, box[0].x, 0.15f) && WisEqual(pFrag->m_box[0].y, box[0].y, 0.15f) && WisEqual(pFrag->m_box[0].z, box[0].z, 0.15f)
				&& WisEqual(pFrag->m_box[1], box[1], 0.15f) && WisEqual(pFrag->m_box[2], box[2], 0.15f) && WisEqual(pFrag->m_box[3], box[3], 0.15f)
				&& WisEqual(pFrag->m_normal, normal, 0.15f))
				return pFrag;
		}

		i++;
		if (i == 32)
			i = 0;
	}

	return 0;
}

// quadtree.cpp:5947
static void SaveDecalFragments(int numVertices, const WVector *box, const WVector &normal, const WVector *vertices, int numIndices, const unsigned short *indices) {
	sDecalFragments *pFrag = &data_00af80b8[data_00af80b4++];
	if (data_00af80b4 == 32)
		data_00af80b4 = 0;

	pFrag->m_used = true;

	pFrag->m_box[0] = box[0];
	pFrag->m_box[1] = box[1];
	pFrag->m_box[2] = box[2];
	pFrag->m_box[3] = box[3];
	pFrag->m_normal = normal;

	pFrag->m_numVertices = numVertices; memcpy(pFrag->m_vertices, vertices, numVertices * sizeof(WVector));
	pFrag->m_numIndices = numIndices;
	memcpy(pFrag->m_indices, indices, numIndices * sizeof(unsigned short));
}

// quadtree.cpp:5973
void CQuadTree::DecalFragments(const WVector *box, const WVector &normal, WVector *pVertices, int *pNumVertices, int maxVertices, unsigned short *pIndices, int *pNumIndices, int maxIndices, bool bAll) {
	Waabb aabb;
	int index;
	int i;
	WVector pos;
	WVector n;
	WVector t;
	WVector b;
	float halfT;
	float halfB;
	float depth;
	float d;

	// the same decal as one of the last 32: reuse its fragments
	sDecalFragments *pFrag = FindDecalFragments(box, normal);
	if (pFrag) {
		memcpy(pVertices, pFrag->m_vertices, pFrag->m_numVertices * sizeof(WVector));
		memcpy(pIndices, pFrag->m_indices, pFrag->m_numIndices * sizeof(unsigned short));
		*pNumVertices = pFrag->m_numVertices;
		*pNumIndices = pFrag->m_numIndices;
		return;
	}

	WVector center(0.0f, 0.0f, 0.0f);
	for (i = 0; i < 4; i++)
		center += box[i];
	center *= 0.25f;

	// the ground under the box's center, no lower than the base model's box
	pos = center;
	GetMeshList(pos);
	pos = ColTest(pos, false, &index, true);
	if (pos.y < m_pBaseModel->m_aabb.min.y)
		pos.y = m_pBaseModel->m_aabb.min.y;
	center = pos;

	n = -normal;
	n.Normalize();

	t = box[0] - box[1];
	t.Normalize();

	b = WVector(t.z * n.y - n.z * t.y, n.z * t.x - t.z * n.x, t.y * n.x - n.y * t.x);
	b.Normalize();

	WVector side0 = box[1] - box[0];
	halfT = (float)sqrt(side0.x * side0.x + side0.y * side0.y + side0.z * side0.z) * 0.5f;
	WVector side1 = box[2] - box[1];
	halfB = (float)sqrt(side1.x * side1.x + side1.y * side1.y + side1.z * side1.z) * 0.5f;
	depth = halfT > halfB ? halfT : halfB;

	// the six sides of the box around the center, normals pointing out
	d = WDotProduct(center, t);
	m_decalPlanes[0] = WPlane(t, d - halfT);
	m_decalPlanes[1] = WPlane(-t, -d - halfT);

	d = WDotProduct(center, b);
	m_decalPlanes[2] = WPlane(b, d - halfB);
	m_decalPlanes[3] = WPlane(-b, -d - halfB);

	d = WDotProduct(center, n);
	m_decalPlanes[4] = WPlane(n, d - depth);
	m_decalPlanes[5] = WPlane(-n, -d - depth);

	aabb.Clear();
	for (i = 0; i < 4; i++)
		aabb.AddPoint(box[i]);
	aabb.min.y -= depth;
	aabb.max.y += depth;

	GetMeshList(aabb);

	m_pDecalVertices = pVertices;
	m_decalNumVertices = 0;
	m_decalMaxVertices = maxVertices;
	int numIndices = 0;

	for (i = 0; i < m_meshNum; i++) {
		int first = m_decalNumVertices;
		ClipDecalTriangle(&m_pBaseModel->m_pTriangles[m_meshList[i]], n);

		int count = m_decalNumVertices - first - 2;
		unsigned short *p = pIndices + numIndices;
		int j;
		for (j = 0; j < count; j++) {
			if (numIndices + 3 > maxIndices)
				break;

			*p++ = (unsigned short)first;
			*p++ = (unsigned short)(first + j + 1);
			*p++ = (unsigned short)(first + j + 2);
		}
		numIndices += j * 3;
	}

	if (bAll) {
		GetObjList(aabb);

		for (i = 0; i < m_objNum; i++) {
			WPolySoup::sModel *pModel = m_pPolySoup->m_models[m_objList[i]];
			int numTri = pModel->m_pPuppet->GetFaceNum();
			for (int k = 0; k < numTri; k++) {
				int first = m_decalNumVertices;
				ClipDecalTriangle(&pModel->m_pTriangles[k], n);

				int count = m_decalNumVertices - first - 2;
				unsigned short *p = pIndices + numIndices;
				int j;
				for (j = 0; j < count; j++) {
					if (numIndices + 3 > maxIndices)
						break;

					*p++ = (unsigned short)first;
					*p++ = (unsigned short)(first + j + 1);
					*p++ = (unsigned short)(first + j + 2);
				}
				numIndices += j * 3;
			}
		}
	}

	SaveDecalFragments(m_decalNumVertices, box, normal, pVertices, numIndices, pIndices);

	*pNumVertices = m_decalNumVertices;
	*pNumIndices = numIndices;
}

// quadtree.cpp:6121
void CQuadTree::DrawDecal(int texture, const WVector &pos, const WVector &normal, const WVector &tangent, float size, float alpha, enumPolyBlend blend, bool bAll) {
	static WVector vertices[1024];
	static unsigned short indices[1536];
	WTVertex tvs[1024];
	int numVerts;
	int numIndices;

	// the decal's frame: the unit normal, tangent and their cross product
	WVector n = normal;
	n.Normalize();
	WVector t = tangent;
	t.Normalize();
	WVector b;
	b = WCrossProduct(n, t);
	b.Normalize();

	// the box the decal covers
	WVector box[4];
	box[0] = pos - normal - tangent;
	box[1] = normal + pos - tangent;
	box[2] = normal + pos + tangent;
	box[3] = pos - normal + tangent;

	DecalFragments(box, b, vertices, &numVerts, 0x400, indices, &numIndices, 0x600, bAll);

	float lenN = (float)sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
	float lenT = (float)sqrt(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z);
	float half = (lenT + lenN) * 0.5f;
	float scaleU = size / (lenN + lenN);
	float scaleV = size / (lenT + lenT);

	for (int i = 0; i < numVerts; i++) {
		tvs[i].x = vertices[i].x;
		tvs[i].y = vertices[i].y;
		tvs[i].z = vertices[i].z;

		float dx = vertices[i].x - pos.x;
		float dy = vertices[i].y - pos.y;
		float dz = vertices[i].z - pos.z;
		tvs[i].tu = (dz * n.z + dy * n.y + dx * n.x) * scaleU + 0.5f;
		tvs[i].tv = 0.5f - (dz * t.z + dy * t.y + dx * t.x) * scaleV;

		// fades out with the height above or below the decal's plane
		float fade = (float)fabs(pos.y - vertices[i].y) < half ? (float)fabs(pos.y - vertices[i].y) : half;
		tvs[i].diffuse = ((unsigned long)(unsigned char)(long)((half - fade) * (1.0f / half) * alpha * 255.0f) << 24) | 0xFFFFFF;
	}

	g_view->DrawIndexedTriangles(tvs, numVerts, indices, numIndices, (texture & 0x7FF) | blend | 0x22200000, 0x400);
}

// quadtree.cpp:6203
void CQuadTree::TraceAABB(const WVector &start, const WVector &end, const Waabb &box, trace_s *pTrace) {
	WPlane plane;
	WPlane hitPlane;
	Waabb startBox;
	Waabb endBox;
	Waabb sweep;
	memset(pTrace, 0, sizeof(trace_s));

	if (WisEqual(start, end, 0.00001f)) {
		pTrace->m_endPos = end;
		return;
	}

	startBox = box + start;
	endBox = box + end;

	sweep = ExpandAABB(startBox, endBox);
	sweep.min -= WVector::ONE;
	sweep.max += WVector::ONE;

	WVector delta;
	delta = end - start;

	float fraction = 1.0f;
	GetObjList(sweep);

	for (int i = 0; i < m_objNum; i++) {
		WPolySoup::sModel *pModel = m_pPolySoup->m_models[m_objList[i]];

		// "*nocol" objects do not collide.
		if (pModel->m_reservedByte & 1)
			continue;

		int count = 0;
		if (pModel->m_pPuppet->m_bbList.Find("char1")) {
			// The boxes "char0", "char1", ... of the object, at its position.
			w_bound_box *pBox = pModel->m_pPuppet->m_bbList.Find("char0");
			while (pBox) {
				count++;
				Waabb charBox = pBox->info->aabb + pModel->m_pPuppet->m_mat.pivot;
				float t = AabbIntersect(startBox, delta, charBox, &hitPlane);
				if (t < fraction) {
					fraction = t;
					plane = hitPlane;
				}

				char *name = MakeStr("char%d", count);
				pBox = pModel->m_pPuppet->m_bbList.Find(name);
			}
		} else {
			float t = AabbIntersect(startBox, delta, pModel->m_sceneAabb, &hitPlane);
			if (t < fraction) {
				fraction = t;
				plane = hitPlane;
			}
		}
	}

	if (fraction == 1.0f) {
		pTrace->m_endPos = end;
	} else {
		pTrace->m_endPos = start + WVector::UNIT_POS_Y + delta * fraction;
	}

	pTrace->m_fraction = fraction;
	pTrace->m_plane = plane;
	pTrace->m_flag = false;
}

// quadtree.cpp:6293
// TraceAABB against the objects' oriented boxes "char0", "char1", ...
void CQuadTree::TraceOBB(const WVector &start, const WVector &end, const Waabb &box, trace_s *pTrace) {
	if (IsLocalContent((localContentType_t)0x56)) {
		WPlane plane;
		WPlane hitPlane;
		Waabb startBox;
		Waabb endBox;
		Waabb sweep;
		memset(pTrace, 0, sizeof(trace_s));

		if (WisEqual(start, end, 0.00001f)) {
			pTrace->m_endPos = end;
			return;
		}

		startBox = box + start;
		endBox = box + end;

		sweep = ExpandAABB(startBox, endBox);
		sweep.min -= WVector::ONE;
		sweep.max += WVector::ONE;

		WVector delta;
		delta = end - start;

		float fraction = 1.0f;
		GetObjList(sweep);

		for (int i = 0; i < m_objNum; i++) {
			WPolySoup::sModel *pModel = m_pPolySoup->m_models[m_objList[i]];

			// "*nocol" objects do not collide.
			if (pModel->m_reservedByte & 1)
				continue;

			int count = 0;
			if (pModel->m_pPuppet->m_bbList.Find("char0")) {
				w_bound_box *pBox = pModel->m_pPuppet->m_bbList.Find("char0");
				while (pBox) {
					Wobb obb;
					obb = pBox->bone->GetMatrix();
					count++;
					obb *= (pBox->info->aabb.max - pBox->info->aabb.min) * 0.5f;
					obb.center = pBox->info->spherePivot * pBox->bone->GetMatrix();
					float t = ObbIntersect(startBox, delta, obb, &hitPlane);
					if (t < fraction) {
						fraction = t;
						plane = hitPlane;
					}

					pBox = pModel->m_pPuppet->m_bbList.Find(MakeStr("char%d", count));
				}
			}
		}

		if (fraction == 1.0f) {
			pTrace->m_endPos = end;
		} else {
			pTrace->m_endPos = start + WVector::UNIT_POS_Y + delta * fraction;
		}

		pTrace->m_fraction = fraction;
		pTrace->m_plane.normal = plane.normal;
		pTrace->m_plane.dis = plane.dis;
		pTrace->m_flag = false;
	}
}

// quadtree.cpp:6420
void CQuadTree::DrawDebugTraceOBB(const WVector &start, const WVector &end, const Waabb &box) {
	Waabb startBox;
	Waabb endBox;
	Waabb sweep;

	startBox = box + start;
	endBox = box + end;

	sweep = ExpandAABB(startBox, endBox);
	sweep.min -= WVector::ONE;
	sweep.max += WVector::ONE;

	GetObjList(sweep);

	for (int i = 0; i < m_objNum; i++) {
		WPolySoup::sModel *pModel = m_pPolySoup->m_models[m_objList[i]];

		// "*nocol" objects do not collide.
		if (pModel->m_reservedByte & 1)
			continue;

		WPuppet *pPuppet = pModel->m_pPuppet;
		if (!pPuppet)
			continue;

		// The boxes "char0", "char1", ... of the object, oriented with their bones.
		int count = 0;
		w_bound_box *pBox = pPuppet->m_bbList.Find("char0");
		while (pBox) {
			count++;

			Wobb obb(pBox->bone->GetMatrix());
			obb *= (pBox->info->aabb.max - pBox->info->aabb.min) * 0.5f;
			obb.center = pBox->info->spherePivot * pBox->bone->GetMatrix();
			g_view->DrawOBB(obb, 0xFF00FF00);

			pBox = pModel->m_pPuppet->m_bbList.Find(MakeStr("char%d", count));
		}
	}
}

// quadtree.cpp:6493
void CQuadTree::DrawDebugTraceAABB(const WVector &start, const WVector &end, const Waabb &box) {
	Waabb startBox;
	Waabb endBox;
	Waabb sweep;

	startBox = box + start;
	endBox = box + end;

	sweep = ExpandAABB(startBox, endBox);
	sweep.min -= WVector::ONE;
	sweep.max += WVector::ONE;

	GetObjList(sweep);

	for (int i = 0; i < m_objNum; i++) {
		WPolySoup::sModel *pModel = m_pPolySoup->m_models[m_objList[i]];

		// "*nocol" objects do not collide.
		if (pModel->m_reservedByte & 1)
			continue;

		if (!pModel->m_pPuppet)
			continue;

		int count = 0;
		if (pModel->m_pPuppet->m_bbList.Find("char1")) {
			// The boxes "char0", "char1", ... of the object, at its position.
			w_bound_box *pBox = pModel->m_pPuppet->m_bbList.Find("char0");
			while (pBox) {
				count++;
				Waabb charBox = pBox->info->aabb + pModel->m_pPuppet->m_mat.pivot;
				g_view->DrawAABB(charBox, 0xFFFF0000, false, 0);

				const char *name = MakeStr("char%d", count);
				pBox = pModel->m_pPuppet->m_bbList.Find(name);
			}
		} else {
			g_view->DrawAABB(pModel->m_sceneAabb, 0xFFFF0000, false, 0);
		}
	}
}

// quadtree.cpp:6552
int CQuadTree::GetObjColNum(void) {
	int num = 0;
	for (std::vector<sColData>::iterator it = m_colList.begin(); it != m_colList.end(); ++it) {
		if ((*it).m_objIndex >= 0)
			num++;
	}
	return num;
}

// quadtree.cpp:6566
bool CQuadTree::IsFallenIntoWater(WVector pos, Waabb *pBox, bool bTest) {
	bool fallen = false;
	for (unsigned int i = 0; i < m_waterList.size(); i++) {
		sWater &water = m_waterList[i];

		// inside the water's box horizontally and below its surface
		if (pos.x > water.m_aabb.min.x && pos.x < water.m_aabb.max.x
			&& pos.z > water.m_aabb.min.z && pos.z < water.m_aabb.max.z
			&& pos.y < water.m_aabb.max.y) {
			if (pBox != 0)
				*pBox = water.m_aabb;

			if (!bTest)
				GolfBall().m_lie = water.m_index;

			fallen = true;
		}
	}

	return fallen;
}

// quadtree.cpp:6591
bool CQuadTree::IsOverWater(WVector pos) {
	GetMeshList(pos);

	for (unsigned int i = 0; i < m_waterList.size(); i++) {
		sWater &water = m_waterList[i];

		pos.y = (water.m_aabb.min.y + water.m_aabb.max.y) * 0.5f;

		// the ground under the water's middle is below its surface
		if (water.m_aabb.IsInclude(pos) && water.m_aabb.max.y > ColTest(pos, true, 0, true).y)
			return true;
	}

	return false;
}

// quadtree.cpp:6608
float CQuadTree::GetShadowIntensity(const WVector &pos) {
	if (!WSingleton<COption>::Instance()->vIsShadowmapEnabled() || !GOLFDOC()->m_day)
		return 1.0f;

	WVector lightDir = g_lightset.nearone;

	lightDir.y = 0.0f; lightDir.Normalize();
	lightDir *= -320.0f;

	GetObjList(pos, lightDir);

	float result = 1.0f;
	Waabb box;
	WVector ext;
	unsigned char ambient = (unsigned char)(g_lightset.ambient >> 17);

	for (int i = 0; i < m_objNum; i++) {
		WPolySoup::sModel *pModel;
		if (m_pPolySoup->GetModelNum() != m_pPolySoup->GetRmrModelNum()) {
			pModel = 0;
			for (int j = 0; j < m_pPolySoup->GetModelNum(); j++) {
				if (m_pPolySoup->m_models[j]->m_objIndex == m_objList[j])
					pModel = m_pPolySoup->m_models[j];
			}
		} else {
			pModel = m_pPolySoup->m_models[m_objList[i]];
		}

		if (!pModel)
			continue;

		// only tall objects cast a shadow
		if (pModel->m_aabb.max.y - pModel->m_aabb.min.y < 16.0f)
			continue;

		// inside the object's box, in front of the light
		if (pModel->m_aabb.IsInclude(pos) && (pos - pModel->m_basePos) * lightDir < 0.0f)
			return ambient;

		ext = pModel->m_aabb.max - pModel->m_basePos;

		box.min = pModel->m_basePos - WVector(ext.x * 0.75f, ext.y, ext.z * 0.75f); box.max = WVector(ext.x * 0.75f, ext.y * 0.5f, ext.z * 0.75f) + pModel->m_basePos; float t = RayIntersectAABBSolid(box, pos, g_lightset.nearone * -320.0f, 0, 0.0f);
		if (t < result)
			result = t;
	}

	return result;
}

// quadtree.cpp:6678
// Marches the ray in steps of 1.6 until it falls below the ground, then
// returns the point the nearest triangle of the last step is hit at.
WVector CQuadTree::GetRayIntersection(WVector from, WVector dir) {
	dir.Normalize();
	dir *= 1.6f;

	float t = 1.0f;

	for (int step = 0; step < 1500; step++) {
		WVector probe; probe = from + dir; GetMeshList(probe); int index = -1; probe = ColTest(probe, true, &index, true); if (index == -1) probe.y = m_pBaseModel->m_aabb.min.y; if (probe.y < m_pBaseModel->m_aabb.min.y) probe.y = m_pBaseModel->m_aabb.min.y; WVector ground = probe; if (ground.y > (from + dir).y) {
			GetMeshList(from, dir);

			if (m_meshNum > 0) {
				for (int i = 0; i < m_meshNum; i++) {
					float hit = intersection::IntersectRayTriangle(from, dir, m_pBaseModel->m_pTriangles[m_meshList[i]].m_vertex);
					if (hit > t)
						continue;
					t = hit;
				}

				if (t < 1.0f) {
					break;
				}
			}
		}

		from += dir;
	}

	WVector point = from + dir * t;
	WVector probe = point; GetMeshList(point); int hit; probe = ColTest(probe, false, &hit, true); if (probe.y < m_pBaseModel->m_aabb.min.y) probe.y = m_pBaseModel->m_aabb.min.y; return probe;
}

// quadtree.cpp:6713
// Marches the ray in steps of 1.6 against the objects' oriented boxes "char0",
// "char1", ... (two triangles of one face each); on a hit returns the point the
// nearest triangle of the object is hit at, else the ground's GetRayIntersection.
WVector CQuadTree::GetRayIntersectionObj(WVector from, WVector dir) {
	WVector step = dir;
	int count = 0;
	bool hit = false;
	if (!IsLocalContent((localContentType_t)0x56))
		return WVector::ONE;

	step.Normalize();
	step *= 1.6f;

	float t = 1.0f;

	int i;
	for (int s = 0; s < 1500; s++) {
		GetObjList(from, step);
		for (i = 0; i < m_objNum; i++) {
			WPuppet *pPuppet = m_pPolySoup->m_models[m_objList[i]]->m_pPuppet;
			if (!pPuppet)
				continue;

			w_bound_box *pBox = pPuppet->m_bbList.Find("center");
			if (!pBox)
				continue;

			pBox = pPuppet->m_bbList.Find("char0");
			while (pBox) {
				Wobb obb(pBox->bone->GetMatrix());
				count++;
				obb *= (pBox->info->aabb.max - pBox->info->aabb.min) * 0.5f;
				obb.center = pBox->info->spherePivot * pBox->bone->GetMatrix();

				WVector center = obb.center;
				WVector axisX = obb.extend[0];
				WVector axisY = obb.extend[1];
				WVector axisZ = obb.extend[2];

				// One face of the box, as the triangles (v0, v1, v2) and (v1, v2, v3).
				WVector tri[3];
				WVector zx = axisZ * -1.0f - axisX * 1.0f;
				tri[0] = axisY + zx + center;
				tri[1] = axisY * -1.0f + zx + center;
				WVector xz = axisZ * -1.0f + axisX;
				tri[2] = xz + axisY * -1.0f + center;
				WVector v3 = xz + axisY + center;

				float d = intersection::IntersectRayTriangle(from, step, tri);
				if (d <= t)
					t = d;
				if (t < 1.0f)
					hit = true;

				tri[0] = tri[1];
				tri[1] = tri[2];
				tri[2] = v3;
				d = intersection::IntersectRayTriangle(from, step, tri);
				if (d <= t)
					t = d;
				if (t < 1.0f)
					goto found;

				if (hit)
					goto found;

				pBox = m_pPolySoup->m_models[m_objList[i]]->m_pPuppet->m_bbList.Find(MakeStr("char%d", count));
			}
		}

		from += step;
	}

	return GetRayIntersection(from, dir);

found:
	{
		WPolySoup::sModel *pModel = m_pPolySoup->m_models[m_objList[i]];
		int faceNum = pModel->m_pQuads->m_count;

		t = 1.0f;
		for (int j = 0; j < faceNum; j++) {
			float d = intersection::IntersectRayTriangle(from, step, pModel->m_pTriangles[j].m_vertex);
			if (d <= t)
				t = d;
			if (t < 1.0f)
				break;
		}

		return from + step * t;
	}
}

// quadtree.cpp:6823
WVector CQuadTree::GetRayIntersectionWall(WVector from, WVector dir) {
	WVector step = dir;
	step.Normalize();
	step *= 1.6f;

	dir = GetRayIntersection(from, dir);

	Waabb box;
	box = GetPVS().m_pBaseModel->m_aabb;

	if (box.IsInclude(dir))
		return dir;

	if (!m_pPolySoup->m_models[0]->m_pPuppet)
		return from;

	float t = 1.0f;

	for (int i = 0; i < 1500; i++) {
		int faceNum = m_pPolySoup->m_models[0]->m_pPuppet->GetFaceNum();

		if (faceNum > 0) {
			for (int j = 0; j < faceNum; j++) {
				float d = Min(intersection::IntersectRayTriangle(from, step, m_pPolySoup->m_models[0]->m_pTriangles[j].m_vertex), t);
				if (d > 0.0f)
					t = d;
			}
			if (t < 1.0f)
				break;
		}

		from += step;
	}

	dir = step * t + from;
	return dir;
}

// quadtree.cpp:6939
float CQuadTree::IntersectRayTriangle(const WVector &orig, const WVector &dir, const WVector *tri) {
	return intersection::IntersectRayTriangle(orig, dir, tri);
}

// quadtree.cpp:6944
void CQuadTree::MakeAfterBurner(void) {
	WVector velocity = GolfBall().PreData(0)->m_velocity;
	velocity.y = 0.0f;
	GolfBall().m_spinImpulse = velocity * (GolfBall().m_lieAngle * -40.0f) * (1.0f / GolfClub().GetPower());

	GolfBall().m_spin = GolfBall().m_lieAngle * 1.5f;
	GolfBall().m_eventPreDataIndex[6] = GolfBall().m_preDataCount;
	GolfBall().m_state = 2;
	GolfBall().m_velocity.Reset();

	GolfBall().m_flags &= ~0x73;
}

// quadtree.cpp:6959
void CQuadTree::MakeVectorSlide(float scale, float maxSpeed, float lift, int maxCount) {
	if (GolfBall().m_powerFlags & 0x50)
		return;

	GolfBall().m_state = 1;

	bool bLimit = false;
	if (GolfBall().GetVectorSlideCount() >= maxCount)
		bLimit = true;

	if (GolfBall().m_velocity.Magnitude() < maxSpeed)
		GolfBall().m_velocity *= scale;

	WVector dir = GolfBall().m_velocity * 0.02f;
	WVector ground = GetGroundPoint(GolfBall().m_pos + dir, true, 0, true);
	WVector slide = ground - GolfBall().m_pos;
	slide.Normalize();
	slide.y *= GolfBall().m_velocity.Magnitude();
	if (!bLimit)
		GolfBall().m_velocity.y = slide.y + lift;
	else
		GolfBall().m_velocity.y = slide.y;

	GolfBall().AddVectorSlideCount();

	GolfBall().PreDataToWrite(GolfBall().m_preDataCount)->m_groundType = 6;
}

// quadtree.cpp:6996
int CQuadTree::GetBounceCount(void) {
	int count = m_bounceCount;
	int lastFrame = -1;
	for (std::vector<sColData>::iterator it = m_colList.begin(); it != m_colList.end(); ++it) {
		if ((*it).m_frame - lastFrame > 5) {
			count++;
			lastFrame = (*it).m_frame;
		}
	}
	return count;
}

// quadtree.cpp:7018
float CQuadTree::GetBasePosY(WVector pos, float height) {
	for (unsigned char i = 0; i < m_waterList.size(); i++) {
		Waabb box = m_waterList[i].m_aabb;
		if (pos.x > box.min.x && pos.x < box.max.x
			&& pos.z > box.min.z && pos.z < box.max.z
			&& pos.y < box.max.y) {
			pos.y = box.max.y;
			break;
		}
	}

	if (m_lowestGroundMode && GolfBall().m_preDataCount > 0) {
		return GetLowestGround(pos) + height;
	}

	float t[2];
	t[0] = CheckCollision(pos, WVector(0.0f, 100.0f, 0.0f), 0, true);
	WVector top(pos.x, pos.y + 100.0f, pos.z);
	t[1] = CheckCollision(top, WVector(0.0f, -100.0f, 0.0f), 0, true);
	float fromTop = 1.0f - t[1];

	int hit = (t[0] < fromTop) ? 0 : 1;
	float y;
	if (t[hit] < 1.0f) {
		if (hit == 0) {
			float down = CheckCollision(pos, WVector(0.0f, -100.0f, 0.0f), 0, true);
			if (down < 1.0f) {
				y = pos.y - down * 100.0f + height;
			} else {
				y = pos.y;
			}
		} else {
			y = fromTop * 100.0f + pos.y + height;
		}
	} else {
		float down = CheckCollision(pos, WVector(0.0f, -100.0f, 0.0f), 0, true);
		if (down < 1.0f) {
			y = pos.y - down * 100.0f + height;
		} else {
			y = pos.y;
		}
	}

	height += m_lowest;
	return Max(y, height);
}

// quadtree.cpp:7080
float CQuadTree::GetGravity(void) {
	return m_gravity * 34.295296f;
}

// quadtree.cpp:7085
void CQuadTree::GravityModifier(float gravity) {
	m_gravity = gravity;
}

// quadtree.cpp:7092
// Whether end lies inside an object: one of its oriented boxes "char0",
// "char1", ... grown by one unit, or, for an object without boxes, its scene
// box grown by one unit on x and z and reaching 10 units below it. A hit
// keeps the trace at start.
void CQuadTree::TraceOBB_New(const WVector &start, const WVector &end, const Waabb &box, trace_s *pTrace) {
	if (IsLocalContent((localContentType_t)0x56)) {
		Waabb startBox;
		Waabb endBox;
		Waabb sweep;
		Waabb objBox;
		WVector pos = end;
		bool bHit = false;
		memset(pTrace, 0, sizeof(trace_s));

		if (WisEqual(start, end, 0.00001f)) {
			pTrace->m_endPos = end;
			return;
		}

		startBox = box + start;
		endBox = box + end;

		sweep = ExpandAABB(startBox, endBox);
		sweep.min -= WVector::ONE;
		sweep.max += WVector::ONE;

		GetObjList(sweep);

		for (int i = 0; i < m_objNum; i++) {
			WPolySoup::sModel *pModel = m_pPolySoup->m_models[m_objList[i]];

			// "*nocol" objects do not collide.
			if (pModel->m_reservedByte & 1)
				continue;

			int count = 0;
			if (!pModel->m_pPuppet)
				continue;

			w_bound_box *pBox = pModel->m_pPuppet->m_bbList.Find("char0");
			if (pBox) {
				while (pBox) {
					Wobb obb;
					obb = pBox->bone->GetMatrix();
					WVector half = (pBox->info->aabb.max - pBox->info->aabb.min) * 0.5f;
					half += WVector::ONE;
					obb *= half;
					obb.center = pBox->info->spherePivot * pBox->bone->GetMatrix();

					if (obb.IsInclude(pos)) {
						bHit = true;
						break;
					}

					pBox = m_pPolySoup->m_models[m_objList[i]]->m_pPuppet->m_bbList.Find(MakeStr("char%d", ++count));
				}
			} else {
				if (!(pos.y > pModel->m_sceneAabb.max.y) && !(pModel->m_sceneAabb.min.y - 10.0f > pos.y)) {
					objBox = pModel->m_sceneAabb;
					objBox.min = WVector(objBox.min.x - 1.0f, objBox.min.y, objBox.min.z - 1.0f);
					objBox.max = WVector(objBox.max.x + 1.0f, objBox.max.y, objBox.max.z + 1.0f);

					if (pos.x > objBox.min.x && pos.x < objBox.max.x && pos.z > objBox.min.z && pos.z < objBox.max.z) {
						bHit = true;
						break;
					}
				}
			}
		}

		if (!bHit) {
			pTrace->m_endPos = end;
		} else {
			pTrace->m_endPos = start;
		}

		pTrace->m_fraction = 1.0f;
		pTrace->m_flag = false;
	}
}

// quadtree.cpp:7237
bool CQuadTree::TraceAABB_BackGround(const WVector &start, const WVector &end, const Waabb &box, trace_s *pTrace, WPolySoup::sModel *pModel) {
	if (!IsLocalContent((localContentType_t)0x56))
		return false;

	if (WisEqual(start, end, 0.00001f)) {
		pTrace->m_endPos = end;
		return false;
	}

	memset(pTrace, 0, sizeof(trace_s));

	WVector delta;
	delta = end - start;

	// The base model's box, grown by one unit on every side but the top,
	// which is the course model's.
	const Waabb &base = m_pBaseModel->m_aabb;
	Waabb world(base.min, base.max);
	world.min.x -= 1.0f; world.min.y -= 1.0f; world.min.z -= 1.0f;
	world.max += WVector::ONE;
	world.max.y = m_pPolySoup->m_models[0]->m_aabb.max.y;

	// The box moved along the horizontal part of the path.
	Waabb moved = box;
	moved.min.x += delta.x;
	moved.min.z += delta.z;
	moved.max.x += delta.x;
	moved.max.z += delta.z;

	int inside = world.IsInclude(moved);
	pTrace->m_fraction = 1.0f;
	pTrace->m_flag = false;

	return inside ? true : false;
}

// quadtree.cpp:7314
int CQuadTree::CheckCamera(void) {
	int result = -1;
	int index = 0;
	bool visible = false;
	int i;

	if (!IsLocalContent((localContentType_t)0x56))
		return -1;

	WPuppet *pPuppet = m_pPolySoup->m_models.front()->m_pPuppet;
	if (pPuppet != 0) {
		w_bound_box *pCenter = pPuppet->m_bbList.Find("center");

		if (pCenter != 0) {
			w_bound_box *pBox = pPuppet->m_bbList.Find("camera0");
			while (pBox != 0) {
				WVector corners[4] = {
					pBox->info->aabb.min,
					pBox->info->aabb.min, pBox->info->aabb.min, pBox->info->aabb.min
				};
				corners[0].y = pBox->info->aabb.max.y; corners[1].x = pBox->info->aabb.max.x; corners[1].y = pBox->info->aabb.max.y; corners[2].x = pBox->info->aabb.max.x;

				for (i = 0; i < 4; i++) {
					visible = g_view->InFrustum(corners[i]);
					if (visible) {
						result = index;
						break;
					}
				}
				if (visible)
					break;

				index++;
				pBox = m_pPolySoup->m_models[0]->m_pPuppet->m_bbList.Find(MakeStr("camera%d", index));
			}
		}
	}

	return result;
}

// quadtree.cpp:7374
void CQuadTree::SetAvatarPosition(const WVector &pos, float size) {
	if (IsLocalContent((localContentType_t)0x56)) {
		m_avatarBox.Reset();

		m_avatarBox.min = pos;
		m_avatarBox.max = pos;

		for (int i = 0; i < 3; i++) {
			m_avatarBox.min.p[i] -= size;
			m_avatarBox.max.p[i] += size;
		}

		m_avatarBox.min.y = -2.0f;
		m_avatarBox.max.y = 2.0f;
	}
}

// quadtree.cpp:7394
// Moves the avatar's box (its "center" bone, sized by m_avatarBox) along the
// floor from start to end against the object's boxes "char0", "char1", ...;
// a hit pushes it one unit along the move, or keeps it at start.
void CQuadTree::TraceOBB_Avatar_genPosition(const WVector &start, const WVector &end, trace_s *pTrace, WPolySoup::sModel *pModel) {
	if (IsLocalContent((localContentType_t)0x56)) {
		memset(pTrace, 0, sizeof(trace_s));

		int count = 0;
		if (WisEqual(start, end, 0.00001f)) {
			pTrace->m_endPos = start;
			return;
		}

		WVector delta;
		delta = end - start;

		if (!pModel->m_pPuppet) {
			pTrace->m_endPos = start;
			return;
		}

		w_bound_box *pCenter = pModel->m_pPuppet->m_bbList.Find("center");
		if (pCenter) {
			Wobb avatarObb;
			avatarObb = pCenter->bone->GetMatrix();
			avatarObb *= (m_avatarBox.max - m_avatarBox.min) * 0.5f;
			avatarObb.center.x = (m_avatarBox.max.x + m_avatarBox.min.x) * 0.5f; avatarObb.center.y = (m_avatarBox.max.y + m_avatarBox.min.y) * 0.5f; avatarObb.center.z = (m_avatarBox.max.z + m_avatarBox.min.z) * 0.5f;

			for (w_bound_box *pBox = pModel->m_pPuppet->m_bbList.Find("char0"); pBox; pBox = pModel->m_pPuppet->m_bbList.Find(MakeStr("char%d", ++count))) {
				Wobb obb;
				WMatrix &mat = pBox->bone->GetMatrix(); obb.extend[0] = mat.xa; obb.extend[1] = mat.ya; obb.extend[2] = mat.za;
				WVector half = (pBox->info->aabb.max - pBox->info->aabb.min) * 0.5f;
				obb.extend[0] *= half.x; obb.extend[1] *= half.y; obb.extend[2] *= half.z; obb.center = pBox->info->spherePivot * mat;

				delta.y = 0.0f;
				Wobb moved = obb;
				moved.center = WVector(delta.x + moved.center.x, moved.center.y, delta.z + moved.center.z);

				if (TestIntersectionOBB(avatarObb, moved)) {
					WVector dir = delta;
					dir.Normalize();

					moved = obb;
					moved.center = WVector(dir.x + moved.center.x, moved.center.y, dir.z + moved.center.z);

					WVector pos;
					if (TestIntersectionOBB(avatarObb, moved)) {
						pos = start;
					} else {
						pos = WVector(dir.x + start.x, start.y, dir.z + start.z);
					}
					pTrace->m_endPos = pos;
					return;
				}

			}
		}

		pTrace->m_endPos = end;
	}
}

// quadtree.cpp:7495
// Slides the avatar's boxes ("char0", "char1", ... of model 0) by end - start
// against the object's boxes "char0", "char1", ...; option > 0 sweeps the
// object's box (AabbIntersect) and keeps the object inside its
// "center_window" box in height, option 0/1 moves the object's boxes
// (flattened on y / z) and slides the end position along the blocking box.
void CQuadTree::TraceOBB_WallSliding(const WVector &start, const WVector &end, trace_s *pTrace, WPolySoup::sModel *pModel, int option, bool bAll) {
	if (IsLocalContent((localContentType_t)0x56)) {
		memset(pTrace, 0, sizeof(trace_s));

		bool bFound = false;
		bool bHit = false;
		Waabb objBox;
		Waabb charBox;
		WVector result = start;

		int count = 0;
		int objCount = 0;
		WVector pos = end;
		if (WisEqual(start, end, 0.00001f) && !bAll) {
			pTrace->m_endPos = start;
			return;
		}

		WVector delta;
		delta = end - start;
		float fraction = 1.0f;

		if (strcmp(m_pPolySoup->m_models[0]->m_name, pModel->m_name) == 0) {
			pTrace->m_endPos = start;
			return;
		}

		WPuppet *pPuppet = m_pPolySoup->m_models[0]->m_pPuppet;
		if (!pPuppet) {
			pTrace->m_endPos = start;
			return;
		}

		if (!pModel->m_pPuppet) {
			pTrace->m_endPos = start;
			return;
		}

		w_bound_box *pObjCenter = pModel->m_pPuppet->m_bbList.Find("center");
		if (!pObjCenter) {
			pTrace->m_endPos = start;
			return;
		}

		w_bound_box *pCenter = pPuppet->m_bbList.Find("center");
		w_bound_box *pWindow;
		if (option) {
			pWindow = pPuppet->m_bbList.Find("center_window");
			if (!pWindow)
				return;
		}

		objBox = pModel->m_aabb;

		if (!pCenter) {
			pTrace->m_endPos = start;
			return;
		}

		w_bound_box *pBox = m_pPolySoup->m_models[0]->m_pPuppet->m_bbList.Find("char0");
		while (pBox) {
			Wobb charObb;
			charObb = pBox->bone->GetMatrix();
			charObb *= (pBox->info->aabb.max - pBox->info->aabb.min) * 0.5f;
			charObb.center = pBox->info->spherePivot * pBox->bone->GetMatrix();

			charBox = pBox->info->aabb + m_pPolySoup->m_models[0]->m_pPuppet->m_mat.pivot;

			w_bound_box *pObj = pModel->m_pPuppet->m_bbList.Find("char0");
			while (pObj) {
				bFound = true;
				Wobb obb;
				obb = pObj->bone->GetMatrix();
				obb *= (pObj->info->aabb.max - pObj->info->aabb.min) * 0.5f;
				obb.center = pObj->info->spherePivot * pObj->bone->GetMatrix();

				objBox = pModel->m_aabb;

				if (option > 0) {
					WPlane plane;
					float t = AabbIntersect(objBox, delta, charBox, &plane);

					if (t < fraction) {
						fraction = t;
					}
				} else {
					Wobb moved;
					moved = obb;
					if (option == 0) {
						delta.y = 0.0f;
					} else if (option == 1) {
						delta.z = 0.0f;
						objBox = pObj->info->aabb;
						objBox.max += obb.center + delta;
						objBox.min += obb.center + delta;

						if (objBox.max.y > pWindow->info->aabb.max.y && objBox.max.y - pWindow->info->aabb.max.y > 0.001f) {
							delta.y = end.y - (objBox.max.y - pWindow->info->aabb.max.y) - start.y;
							pos.y = delta.y + start.y;
						}

						if (objBox.min.y < pWindow->info->aabb.min.y) {
							delta.y = pWindow->info->aabb.min.y - objBox.min.y + end.y - start.y;
							pos.y = delta.y + start.y;
						}
					}

					moved += delta;
					if (TestIntersectionOBB(charObb, moved)) {
						bHit = true;
						float z = GetObbPos(moved, true, 2) - pBox->info->aabb.min.z;
						float x;
						if (pos.x > 0.0f)
							x = GetObbPos(moved, true, 0) - pBox->info->aabb.min.x;
						else
							x = GetObbPos(moved, false, 0) - pBox->info->aabb.max.x;

						if (fabs(pBox->info->aabb.max.x - pBox->info->aabb.min.x) > fabs(pBox->info->aabb.max.z - pBox->info->aabb.min.z)) {
							pos.z -= z;
						} else {
							if (pos.x > 0.0f) {
								pos.x -= x;
							} else if (pos.x < 0.0f) {
								pos.x -= x;
							} else {
								pos.z = charObb.center.z - z;
								pos.x = charObb.center.x - x;
							}
						}

						if (option == 1) {
							delta.x = pos.x - start.x;
							delta.z = pos.z - start.z;
						} else {
							delta = pos - start;
						}

						result = pos;
					}
				}

				pObj = pModel->m_pPuppet->m_bbList.Find(MakeStr("char%d", ++objCount));
			}

			objCount = 0;
			pBox = m_pPolySoup->m_models[0]->m_pPuppet->m_bbList.Find(MakeStr("char%d", ++count));
		}

		if (option > 0) {
			if (fraction == 1.0f) {
				pTrace->m_endPos = end;
				return;
			}

			if (fraction != 0.0f) {
				pTrace->m_endPos = start + WVector::UNIT_POS_Y + delta * fraction;
			} else {
				pTrace->m_endPos = start + WVector::UNIT_POS_Y + delta;
			}

			charBox = pWindow->info->aabb + m_pPolySoup->m_models[0]->m_pPuppet->m_mat.pivot;

			pTrace->m_endPos.y = end.y;

			if ((pModel->m_aabb.max.y - pModel->m_aabb.min.y) * 0.5f + end.y > charBox.max.y)
				pTrace->m_endPos.y = charBox.max.y - (pModel->m_aabb.max.y - pModel->m_aabb.min.y) * 0.5f;

			if (end.y - (pModel->m_aabb.max.y - pModel->m_aabb.min.y) * 0.5f < charBox.min.y)
				pTrace->m_endPos.y = (pModel->m_aabb.max.y - pModel->m_aabb.min.y) * 0.5f + charBox.min.y;
			return;
		}

		if (bHit) {
			pTrace->m_endPos = result;
			return;
		}

		if (bFound) {
			pTrace->m_endPos = end;
			return;
		}

		pTrace->m_endPos = start;
	}
}

// quadtree.cpp:7808
bool CQuadTree::CheckPickRoom(WVector pos) {
	if (!IsLocalContent((localContentType_t)0x56))
		return false;

	Waabb box;

	if (m_pPolySoup->m_models[0]->m_pPuppet == 0)
		return false;

	w_bound_box *pBound = m_pPolySoup->m_models[0]->m_pPuppet->m_bbList.Find("center_window");
	if (pBound == 0)
		return false;

	box = pBound->info->aabb + m_pPolySoup->m_models[0]->m_pPuppet->GetMatrix().pivot;

	return box.IsInclude(pos);
}

// quadtree.cpp:7834
int CQuadTree::Get_WallSlidingAngle(const WVector &pos, WPolySoup::sModel *pModel, WVector &normal) {
	if (!IsLocalContent((localContentType_t)0x56))
		return -1;

	WVector dir = pos;
	WVector cand[3];
	memset(cand, 0, sizeof(cand));
	WVector low, high;

	WPuppet *pPuppet = m_pPolySoup->m_models[0]->m_pPuppet;
	int count = 0;
	Waabb charBox, objBox;
	if (pPuppet == 0)
		return -1;

	w_bound_box *pCenter = pPuppet->m_bbList.Find("center");
	w_bound_box *pObjCenter = pModel->m_pPuppet->m_bbList.Find("center");
	if (pObjCenter == 0)
		return -1;

	objBox = pObjCenter->info->aabb + pModel->m_pPuppet->m_mat.pivot;

	if (pCenter == 0)
		return -1;

	w_bound_box *pBox = m_pPolySoup->m_models[0]->m_pPuppet->m_bbList.Find("char_col0");
	while (pBox) {
		charBox = pBox->info->aabb + m_pPolySoup->m_models[0]->m_pPuppet->m_mat.pivot;

		if (charBox.IsInclude(pos)) {
			switch (count) {
			case 0: {
				float hx = (objBox.max.x - objBox.min.x) * 0.5f;
				low.z = pos.z;
				high.z = pos.z;
				low.x = pos.x - hx;
				float hy = (objBox.max.y - objBox.min.y) * 0.5f;
				low.y = pos.y - hy;
				high.x = hx + pos.x;
				high.y = hy + pos.y;
				break;
			}
			case 1:
			case 2: {
				float hy = (objBox.max.y - objBox.min.y) * 0.5f;
				low.x = pos.x;
				high.x = pos.x;
				low.y = pos.y - hy;
				float hx = (objBox.max.x - objBox.min.x) * 0.5f;
				low.z = pos.z - hx;
				high.y = hy + pos.y;
				high.z = hx + pos.z;
				break;
			}
			}

			if (charBox.IsInclude(low) && charBox.IsInclude(high)) {
				dir.Normalize();
				normal = (high + low) * 0.5f + dir;
				cand[count] = normal;
			}
		}

		pBox = m_pPolySoup->m_models[0]->m_pPuppet->m_bbList.Find(MakeStr("char_col%d", ++count));
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (cand[i].p[j] != 0.0f) {
				low = normal + normal;

				for (int k = 0; k < 3; k++) {
					dir = cand[k];

					if (sqrtf(dir.x * dir.x + dir.z * dir.z) < sqrtf(low.z * low.z + low.x * low.x) && sqrtf(dir.x * dir.x + dir.z * dir.z) != 0.0) {
						low = cand[k];
						count = k;
					}
				}

				normal = cand[count];
				return count;
			}
		}
	}

	return -1;
}

// quadtree.cpp:8001
void CQuadTree::OBBToAABB(Wobb obb, Waabb &aabb, WVector &center) {
	WPolySoup::sModel *pModel = GOLFDOC()->m_pPolySoup->FindModel(0);

	// Starts inverted (min at the course's max, max at its min) so the corners
	// pull it to their bounds; the bottom is kept between 0 and 30.
	Waabb course = pModel->m_aabb; Waabb box(course.max, course.min); box.min.y = 30.0f;

	WVector c = obb.center;
	WVector ext0 = obb.extend[0];
	WVector ext1 = obb.extend[1];
	WVector ext2 = obb.extend[2];

	WVector ny = ext1 * -1.0f;
	WVector nz = ext2 * -1.0f;
	WVector nx = ext0 * -1.0f;
	WVector corner[8];
	corner[0] = WVector(c.x + (nx.x + (ext1.x + nz.x)), c.y + (nx.y + (ext1.y + nz.y)), c.z + (nx.z + (ext1.z + nz.z)));
	corner[1] = WVector(c.x + (nx.x + (ny.x + nz.x)), c.y + (nx.y + (ny.y + nz.y)), c.z + (nx.z + (ny.z + nz.z)));
	corner[2] = WVector(c.x + (ext0.x + (ny.x + nz.x)), c.y + (ext0.y + (ny.y + nz.y)), c.z + (ext0.z + (ny.z + nz.z)));
	corner[3] = WVector(c.x + (ext0.x + (ext1.x + nz.x)), c.y + (ext0.y + (ext1.y + nz.y)), c.z + (ext0.z + (ext1.z + nz.z)));
	corner[4] = WVector(c.x + (nx.x + (ext1.x + ext2.x)), c.y + (nx.y + (ext1.y + ext2.y)), c.z + (nx.z + (ext1.z + ext2.z)));
	corner[5] = WVector(c.x + (nx.x + (ny.x + ext2.x)), c.y + (nx.y + (ny.y + ext2.y)), c.z + (nx.z + (ny.z + ext2.z)));
	corner[6] = WVector(c.x + (ext0.x + (ny.x + ext2.x)), c.y + (ext0.y + (ny.y + ext2.y)), c.z + (ext0.z + (ny.z + ext2.z)));
	corner[7] = WVector(c.x + (ext0.x + (ext1.x + ext2.x)), c.y + (ext0.y + (ext1.y + ext2.y)), c.z + (ext0.z + (ext1.z + ext2.z)));

	box.max.y = 0.0f;
	WVector *pCorner = corner;
	for (int count = 8; count != 0; count--, pCorner++) {
		if (!(box.max.x > pCorner->x)) box.max.x = pCorner->x;
		if (!(box.max.y > pCorner->y)) box.max.y = pCorner->y;
		if (!(box.max.z > pCorner->z)) box.max.z = pCorner->z;
		if (!(box.min.x < pCorner->x)) box.min.x = pCorner->x;
		if (!(box.min.y < pCorner->y)) box.min.y = pCorner->y;
		if (!(box.min.z < pCorner->z)) box.min.z = pCorner->z;
	}

	aabb.max = box.max;
	aabb.min = box.min;
}

// quadtree.cpp:8058
// The highest (bAll) or lowest coordinate along axis option of the box's
// eight corners.
float CQuadTree::GetObbPos(Wobb &obb, bool bAll, int option) {
	float fResult;
	if (bAll)
		fResult = -1000.0f;
	else
		fResult = 1000.0f;

	WVector center = obb.center;
	WVector ax = obb.extend[0];
	WVector ay = obb.extend[1];
	WVector az = obb.extend[2];

	WVector ny = ay * -1.0f;
	WVector nz = az * -1.0f;
	WVector nx = ax * -1.0f;

	WVector corners[8];
	corners[0] = WVector(center.x + (nx.x + (ay.x + nz.x)), center.y + (nx.y + (ay.y + nz.y)), center.z + (nx.z + (ay.z + nz.z)));
	corners[1] = WVector(center.x + (nx.x + (ny.x + nz.x)), center.y + (nx.y + (ny.y + nz.y)), center.z + (nx.z + (ny.z + nz.z)));
	corners[2] = WVector(center.x + (ax.x + (ny.x + nz.x)), center.y + (ax.y + (ny.y + nz.y)), center.z + (ax.z + (ny.z + nz.z)));
	corners[3] = WVector(center.x + (ax.x + (ay.x + nz.x)), center.y + (ax.y + (ay.y + nz.y)), center.z + (ax.z + (ay.z + nz.z)));
	corners[4] = WVector(center.x + (nx.x + (ay.x + az.x)), center.y + (nx.y + (ay.y + az.y)), center.z + (nx.z + (ay.z + az.z)));
	corners[5] = WVector(center.x + (nx.x + (ny.x + az.x)), center.y + (nx.y + (ny.y + az.y)), center.z + (nx.z + (ny.z + az.z)));
	corners[6] = WVector(center.x + (ax.x + (ny.x + az.x)), center.y + (ax.y + (ny.y + az.y)), center.z + (ax.z + (ny.z + az.z)));
	corners[7] = WVector(center.x + (ax.x + (ay.x + az.x)), center.y + (ax.y + (ay.y + az.y)), center.z + (ax.z + (ay.z + az.z)));

	int i;
	if (bAll) {
		for (i = 0; i < 8; i++)
			fResult = fResult > corners[i].p[option] ? fResult : corners[i].p[option];
	} else {
		for (i = 0; i < 8; i++)
			fResult = fResult < corners[i].p[option] ? fResult : corners[i].p[option];
	}

	return fResult;
}


// quadtree.cpp:8119
// Moves the avatar's boxes ("char0", "char1", ... of model 0) by end - start
// (flattened on y for option 0, on z for option 1) against the object's boxes
// "char0", "char1", ...; a hit retries one unit along the move, else stays at
// start. Without a hit the trace ends at end, or at start when the object has
// no boxes.
void CQuadTree::TraceOBB_Wall(const WVector &start, const WVector &end, trace_s *pTrace, WPolySoup::sModel *pModel, int option, bool bAll) {
	if (IsLocalContent((localContentType_t)0x56)) {
		memset(pTrace, 0, sizeof(trace_s));

		bool bHit = false;
		bool bFound = false;
		WVector result = start;

		int count = 0;
		int objCount = 0;
		if (WisEqual(start, end, 0.00001f) && !bAll) {
			pTrace->m_endPos = start;
			return;
		}

		WVector delta;
		delta = end - start;

		if (strcmp(m_pPolySoup->m_models.front()->m_name, pModel->m_name) == 0) {
			pTrace->m_endPos = start;
			return;
		}

		WPuppet *pPuppet = m_pPolySoup->m_models.front()->m_pPuppet;
		if (!pPuppet) {
			pTrace->m_endPos = start;
			return;
		}

		if (!pModel->m_pPuppet) {
			pTrace->m_endPos = start;
			return;
		}

		w_bound_box *pObjCenter = pModel->m_pPuppet->m_bbList.Find("center");
		if (!pObjCenter) {
			pTrace->m_endPos = start;
			return;
		}

		w_bound_box *pCenter = pPuppet->m_bbList.Find("center");
		if (pCenter) {
			w_bound_box *pBox = pPuppet->m_bbList.Find("char0");
			while (pBox) {
				Wobb charObb;
				charObb = pBox->bone->GetMatrix();
				charObb *= (pBox->info->aabb.max - pBox->info->aabb.min) * 0.5f;
				charObb.center = pBox->info->spherePivot * pBox->bone->GetMatrix();

				w_bound_box *pObj = pModel->m_pPuppet->m_bbList.Find("char0");
				while (pObj) {
					bFound = true;
					Wobb obb;
					obb = pObj->bone->GetMatrix();
					obb *= (pObj->info->aabb.max - pObj->info->aabb.min) * 0.5f;
					obb.center = pObj->info->spherePivot * pObj->bone->GetMatrix();

					Wobb moved;
					moved = obb;
					if (option == 0)
						delta.y = 0.0f;
					else if (option == 1)
						delta.z = 0.0f;

					moved += delta;
					if (TestIntersectionOBB(charObb, moved)) {
						bHit = true;
						WVector dir = delta;
						dir.Normalize();
						if (option == 0)
							dir.y = 0.0f;
						else if (option == 1)
							dir.z = 0.0f;

						moved = obb;
						moved += dir;
						if (TestIntersectionOBB(charObb, moved)) {
							result = start;
							break;
						}

						delta = dir;
						result = start + dir;
						break;
					}

					pObj = pModel->m_pPuppet->m_bbList.Find(MakeStr("char%d", ++objCount));
				}

				objCount = 0;
				pBox = m_pPolySoup->m_models[0]->m_pPuppet->m_bbList.Find(MakeStr("char%d", ++count));
			}
		}

		if (bHit) {
			pTrace->m_endPos = result;
			return;
		}

		if (bFound) {
			pTrace->m_endPos = end;
			return;
		}

		pTrace->m_endPos = start;
	}
}

// quadtree.cpp:8488
// Moves the box of pModel's "char" boxes from start toward end against the
// other objects' "char" boxes; true when it is blocked.
bool CQuadTree::TraceOBB_Object(const WVector &start, const WVector &end, const Waabb &box, trace_s *pTrace, WPolySoup::sModel *pModel, bool bAll) {
	if (IsLocalContent((localContentType_t)0x56)) {
		memset(pTrace, 0, sizeof(trace_s));
		bool bHit = false;
		int count = 0;
		int charCount = 0;
		WVector delta;
		delta = end - start;
		Waabb sweep;
		Waabb startBox;
		Waabb endBox;

		if (m_pPolySoup->m_models.front()->m_pPuppet == 0 || strcmp(m_pPolySoup->m_models.front()->m_name, pModel->m_name) == 0) {
			pTrace->m_endPos = start;
			return true;
		}

		startBox = box + start;
		endBox = box + end;

		sweep = ExpandAABB(startBox, endBox);
		sweep.min -= WVector::ONE;
		sweep.max += WVector::ONE;

		GetObjList(sweep);

		if (pModel->m_pPuppet == 0) {
			pTrace->m_endPos = start;
			return true;
		}

		w_bound_box *pModelCenter = pModel->m_pPuppet->m_bbList.Find("center");
		if (!pModelCenter) {
			pTrace->m_endPos = start;
			return true;
		}

		for (int i = 0; i < m_objNum; i++) {
			if (!bAll && m_objList[i] <= 0)
				continue;

			WPolySoup::sModel *pObj = m_pPolySoup->m_models[m_objList[i]];
			if (strcmp(pObj->m_name, pModel->m_name) == 0)
				continue;
			if (pObj->m_pPuppet == 0)
				continue;

			w_bound_box *pCenter = pObj->m_pPuppet->m_bbList.Find("center");

			char szPoster[256] = "poster_a.pet";
			if (stricmp(m_pPolySoup->m_models[m_objList[i]]->m_pPuppet->GetPuppetName(), szPoster) == 0)
				continue;
			if (!pCenter)
				continue;

			w_bound_box *pBox = m_pPolySoup->m_models[m_objList[i]]->m_pPuppet->m_bbList.Find("char0");
			while (pBox) {
				Wobb obb;
				obb = pBox->bone->GetMatrix();
				obb *= (pBox->info->aabb.max - pBox->info->aabb.min) * 0.5f;
				obb.center = pBox->info->spherePivot * pBox->bone->GetMatrix();

				w_bound_box *pChar = pModel->m_pPuppet->m_bbList.Find("char0");
				while (pChar) {
					Wobb charObb;
					charObb = pChar->bone->GetMatrix();
					charObb *= (pChar->info->aabb.max - pChar->info->aabb.min) * 0.5f;
					charObb.center = pChar->info->spherePivot * pChar->bone->GetMatrix();

					pChar = pModel->m_pPuppet->m_bbList.Find(MakeStr("char%d", ++charCount));

					Wobb moved;
					moved = charObb;
					moved += delta;
					if (TestIntersectionOBB(obb, moved)) {
						// Blocked: try sliding horizontally one unit along the path.
						WVector dir = delta;
						dir.Normalize();

						moved = charObb;
						moved += WVector(dir.x, 0.0f, dir.z);
						if (TestIntersectionOBB(obb, moved)) {
							bHit = true;
							break;
						} else {
							pTrace->m_endPos = WVector(dir.x + start.x, start.y, dir.z + start.z);
							return false;
						}
					}
				}

				if (bHit)
					break;

				charCount = 0;
				pBox = m_pPolySoup->m_models[m_objList[i]]->m_pPuppet->m_bbList.Find(MakeStr("char%d", ++count));
			}
		}

		if (bHit) {
			pTrace->m_endPos = start;
		} else if (bAll) {
			pTrace->m_endPos = start + end;
		} else {
			pTrace->m_endPos = end;
		}

		return bHit;
	}

	return false;
}
