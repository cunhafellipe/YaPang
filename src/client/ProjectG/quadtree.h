// Original: d:\build\custom\temp\code\client\projectg\quadtree.h
#ifndef PG_CLIENT_PROJECTG_QUADTREE_H
#define PG_CLIENT_PROJECTG_QUADTREE_H

#include "defines.h"
#include "padding.h"
#include "wmath.h"
#include "polysoup.h"
#include "golfball.h"

#include <vector>


// The blend mode of a decal's polygons (DrawDecal).
enum enumPolyBlend {
	W_POLY_BLEND_NORMAL = 0,
	W_POLY_BLEND_ADD = 0x800000,
	W_POLY_BLEND_MULTIPLY = 0x1000000,
	W_POLY_BLEND_INVMULTIPLY = 0x1800000,
	W_POLY_BLEND_FORCE_DWORD = 0x7fffffff
};

// The result of a box trace (TraceAABB_BackGround, TraceOBB_Object).
struct trace_s {
	trace_s();

	WVector m_endPos;      // 0x0, where the box stops (the end when start == end)
	float m_fraction;      // 0xC, TraceAABB_BackGround ends by setting it to 1.0
	WVector m_normal;      // 0x10, the hit plane's normal (TraceAABB copies the WPlane AabbIntersect returns)
	float m_planeDist;     // 0x1C, the hit plane's distance
	bool m_flag;           // 0x20, TraceAABB clears it (meaning not established)
	// 0x21-0x24: alignment padding left implicit (a char-array PADDING would bring /GS cookies the original lacks, sub_537c50)
};
CHECK_SIZE(trace_s, 0x24);
CHECK_FIELD(trace_s, m_endPos, 0x0);
CHECK_FIELD(trace_s, m_fraction, 0xC);
CHECK_FIELD(trace_s, m_normal, 0x10);
CHECK_FIELD(trace_s, m_planeDist, 0x1C);
CHECK_FIELD(trace_s, m_flag, 0x20);

// avatar_avatar.cpp defines AVATAR_AVATAR_QUADTREE_INLINE empty before its
// includes: the original holds the out-of-line copy of the constructor in
// avatar_avatar's object; every other includer inlines it.
#ifndef AVATAR_AVATAR_QUADTREE_INLINE
#define AVATAR_AVATAR_QUADTREE_INLINE inline
#endif

// quadtree.h
AVATAR_AVATAR_QUADTREE_INLINE trace_s::trace_s() {
}

// An animated object of the course (MakeObjStructure creates them; the
// derived classes below add the members of the object kinds that need them).
// Only the members the decompiled code uses are transcribed.
class CAniObj {
	LAYOUT_CHECK_FRIEND;
public:
	CAniObj();      // quadtree.h:41
	~CAniObj();     // quadtree.h:42

	unsigned char m_type;   // 0x0, the model's sub-order (1: CMoveObj, 2: CRotObj, 3: CVtxObj, 4: CTranObj)
	PADDING(0x1, 0x4);
	unsigned long m_rand;   // 0x4, MakeRand of the model's animation length
	short m_objIndex;       // 0x8, the model's object index
	PADDING(0xA, 0xC);
	float m_time;           // 0xC, (m_rand + frame) * 0.02, PreProcess

private:
	char m_sound[0x20];     // 0x10, the "*snd(" script's sound name (sscanf "%s"); empty when no sound plays
	float m_speed;          // 0x30

	friend class CQuadTree;
};
CHECK_SIZE(CAniObj, 0x34);
BEGIN_LAYOUT_CHECK(CAniObj)
	CHECK_FIELD(CAniObj, m_type, 0x0);
	CHECK_FIELD(CAniObj, m_rand, 0x4);
	CHECK_FIELD(CAniObj, m_objIndex, 0x8);
	CHECK_FIELD(CAniObj, m_time, 0xC);
	CHECK_FIELD(CAniObj, m_sound, 0x10);
	CHECK_FIELD(CAniObj, m_speed, 0x30);
END_LAYOUT_CHECK

// An animated object that rotates with the wind (model sub-order 2).
class CRotObj : public CAniObj {
public:
	float m_angle;          // 0x34, the rotation PreProcess applies
	float m_invSize;        // 0x38, 1 / the model's largest box extent
};
CHECK_SIZE(CRotObj, 0x3C);
CHECK_FIELD(CRotObj, m_angle, 0x34);
CHECK_FIELD(CRotObj, m_invSize, 0x38);

// An animated object that moves between two points (model sub-order 1).
class CMoveObj : public CAniObj {
public:
	WVector m_startPos;     // 0x34, the model's position, or the "*node" polygon's first point
	WVector m_endPos;       // 0x40, the model's position, or the "*node" polygon's second point
	float m_standTime;      // 0x4C, "*stand" option (0 by default)
	float m_moveTime;       // 0x50, "*move" option (1 by default)
};
CHECK_SIZE(CMoveObj, 0x54);
CHECK_FIELD(CMoveObj, m_startPos, 0x34);
CHECK_FIELD(CMoveObj, m_endPos, 0x40);
CHECK_FIELD(CMoveObj, m_standTime, 0x4C);
CHECK_FIELD(CMoveObj, m_moveTime, 0x50);

// An animated object with no members of its own (model sub-order 3).
class CVtxObj : public CAniObj {
};
CHECK_SIZE(CVtxObj, 0x34);

// An animated object with no members of its own (model sub-order 4).
class CTranObj : public CAniObj {
};
CHECK_SIZE(CTranObj, 0x34);

// A cache of polygon fragments (members not transcribed yet; the size is
// not established).
struct fragmentsCache_s {
	fragmentsCache_s();
};

// The course's collision quadtree. Only the members the decompiled
// accessors use are transcribed; the class size is not established yet.
class CQuadTree {
	LAYOUT_CHECK_FRIEND;
	friend class CConsole;  // Command's paint/green/condition/bounce/roll set the tuning members
public:
	// One collision record (element of a std::vector, 0x24 bytes).
	struct sColData {
		sColData();

		unsigned char m_objType;        // 0x0, the sModel::m_subOrder of the object hit (IntersectObj)
		PADDING(0x1, 0x2);
		short m_objIndex;       // 0x2, the object hit, negative for the ground (GetObjColNum)
		int m_triIndex;         // 0x4, the triangle hit in the object (IntersectObj)
		int m_frame;            // 0x8, frame of the contact (GetBounceCount)
		WVector m_point;        // 0xC, the contact point (CommitResponse: sColResponse::m_point)
		WVector m_normal;       // 0x18, the contact normal (CommitResponse: sColResponse::m_normal)
	};

	// A water area of the course (AddWater).
	struct sWater {
		Waabb m_aabb;           // 0x0
		int m_index;            // 0x18
	};

	// The state of the ball a collision response works on.
	struct sColResponse {
		// A copy of the ball's state taken before the response.
		struct GolfBallClone {
			GolfBallClone(const CGolfBall &ball);

			// quadtree.h:292
			float GetRadius(void) const { return m_pBall->GetRadius(); }
			// quadtree.h:293
			unsigned long GetSpecial(void) const { return m_pBall->m_flags; }
			// quadtree.h:294
			float GetInitSpin(void) const { return m_pBall->m_lieAngle; }
			// quadtree.h:295
			int GetRenderNum(void) const { return m_pBall->m_preDataCount; }
			// quadtree.h:296
			int GetTopFrame(void) const { return m_pBall->m_eventPreDataIndex[1]; }
			// quadtree.h:297
			int GetAttrIndex(void) const { return m_pBall->m_lastEventPreDataIndex; }

			WVector m_pos;              // 0x0, the ball's m_pos
			WVector m_velocity;         // 0xC, the ball's m_velocity
			short m_lastContactId;      // 0x18
			int m_lastContactIndex;     // 0x1C
			int m_state;                // 0x20
			const CGolfBall *m_pBall;   // 0x24
		};

		sColResponse(const CGolfBall &ball, const WVector &point, const WVector &normal);

		GolfBallClone m_ball;   // 0x0
		PADDING(0x28, 0x48);
		char m_sound[0x40];     // 0x48, the sound name (ObjResponse formats "t_Ex4-%d"); the constructor clears it
		PADDING(0x88, 0x8C);
		WVector m_contactVector;        // 0x8C, ObjResponse's third argument; CommitResponse stores it in sExpected::m_contactVector
		PADDING(0x98, 0x9C);
		WVector m_point;        // 0x9C, the constructor's second argument
		WVector m_normal;       // 0xA8, the constructor's third argument
		bool m_flagB4;          // 0xB4, cleared by the constructor (meaning not established)
		bool m_flagB5;          // 0xB5, cleared by the constructor (meaning not established)
	};

	CQuadTree();
	~CQuadTree();

	void SetSlopeFactor(float factor);      // quadtree.h:112
	// quadtree.h:119
	void SetPolySoup(WPolySoup *pPolySoup) { m_pPolySoup = pPolySoup; }
	float GetLowest(void) const;            // quadtree.h:157
	WVector GetPenaltyPos(void);            // quadtree.h:423
	void Display(void);
	void SetFrame(unsigned long frame);     // quadtree.h:497
	unsigned long GetFrame(void);           // quadtree.h:505
	// quadtree.h:510
	void SetRandSeed(unsigned long seed) { m_randSeed = seed; }
	const std::vector<sColData> &GetColList(void);  // quadtree.h:541
	// quadtree.h:578
	const std::vector<WPolySoup::sTriangle*> *GetTriArray(int texHandle) const { return m_pPolySoup->GetTriArray(texHandle); }

	void Reserve(short count);
	void SetBaseVtxColor(unsigned long color);
	void SetObjVtxColor(unsigned long color);
	void UpdateVtxColor(float elapsed);
	float GetGroundSlope(int index) const;
	bool IsFallenIntoWater(WVector pos, Waabb *pBox, bool bTest);
	void GetMeshList(WVector pos);
	void GetMeshList(WVector pos, const WVector &dir);
	void GetMeshList(Waabb box);
	// quadtree.h:204
	void GetAABB(Waabb& aabb) { aabb = m_pBaseModel->m_aabb; }
	float GetLowestGround(WVector pos);
	float GetHighestGround(WVector pos, int *pIndex);
	int CheckCamera(void);
	float RayIntersectAll(const WVector &pos, const WVector &dir, bool bAll);
	bool OverlappedWithObject(WVector pos, float radius);
	int GetObjIndex(const WVector &pos, bool bAll);
	WVector GetMeshObjcet(const WVector &pos);
	float GetMinPowerOnGround(void) const;
	float GetMinPowerOnGround(const WVector &pos);
	void CalcGreenPlane(void);

	WVector GetGroundPoint(WVector, bool, int *, bool);
	WVector GetRayIntersection(WVector from, WVector dir);
	WVector GetRayIntersectionWall(WVector from, WVector dir);
	WVector ColObject(const WVector &pos, bool bBase, int *pIndex, bool bAll);
	bool IsOverWater(WVector pos);
	unsigned char GetGroundType(WVector pos, bool bWater, CGolfBall::eWater *pWater);
	float GetShadowIntensity(const WVector &pos);
	void PreProcess(float elapsed, bool bForce);
	int GetObjColNum(void);
	WVector GetMeshNormal(const WVector &pos);
	WVector GetMeshNormal(int index);
	float GetGravity(void);
	void GravityModifier(float gravity);
	void MakeBaseStructure(WPolySoup::sModel *pModel, bool bRebuild);
	void MakeObjStructure(short objIndex, int option, bool bRebuild);
	const char *GetBounceSound(int property) const;
	const char *GetRollSound(int property) const;
	const char *GetStepSound(int property) const;
	const char *GetStepSound(const WVector &pos);
	int GetTextureHandle(const WVector &pos);
	const char *GetTextureName(const WVector &pos);
	int GetMeshIndex(const WVector &pos, bool bAll);
	void AddWater(Waabb aabb, int index);
	void ResetWater(void);
	void HoleIn(void);
	bool IsOBArea(const WVector &pos);
	void ResetVariable(void);
	void ResetHoleData(void);
	void BallProcess(float elapsed);
	int GetBounceCount(void);
	void SetCondition(void);
	float GetBasePosY(WVector pos, float height);
	float GetMaxPowerOnGround(void) const;
	float GetMaxPowerOnGround(const WVector &pos);
	float GetGreenPos(const WVector &pos);
	WVector TestCameraCollision(const WVector &target);
	// quadtree.h:461
	float GetHoleCupRadius(void) const { return m_holeCupRadius; }
	// quadtree.h:460
	unsigned char CollideBetweenBallandCaddie(void) { return m_ballState; }
	// quadtree.h:459
	void SetCaddieBox(const Waabb &box) { m_ballTestBox = box; }
	void GetObjList(Waabb box);
	void GetObjList(WVector pos, const WVector &dir);
	bool SimpleColTest(const WVector &pos, const WVector &dir);
	void SetAvatarPosition(const WVector &pos, float size);
	bool CheckPickRoom(WVector pos);
	void DrawDecal(int texture, const WVector &pos, const WVector &normal, const WVector &tangent, float size, float alpha, enumPolyBlend blend, bool bAll);
	void DecalFragments(const WVector *box, const WVector &normal, WVector *pVertices, int *pNumVertices, int maxVertices, unsigned short *pIndices, int *pNumIndices, int maxIndices, bool bAll);
	void TraceAABB(const WVector &start, const WVector &end, const Waabb &box, trace_s *pTrace);
	bool TraceAABB_BackGround(const WVector &start, const WVector &end, const Waabb &box, trace_s *pTrace, WPolySoup::sModel *pModel);
	bool TraceOBB_Object(const WVector &start, const WVector &end, const Waabb &box, trace_s *pTrace, WPolySoup::sModel *pModel, bool bAll);
	void TraceOBB_New(const WVector &start, const WVector &end, const Waabb &box, trace_s *pTrace);
	void TraceOBB(const WVector &start, const WVector &end, const Waabb &box, trace_s *pTrace);
	void TraceOBB_Wall(const WVector &start, const WVector &end, trace_s *pTrace, WPolySoup::sModel *pModel, int option, bool bAll);
	void TraceOBB_WallSliding(const WVector &start, const WVector &end, trace_s *pTrace, WPolySoup::sModel *pModel, int option, bool bAll);
	void TraceOBB_Avatar_genPosition(const WVector &start, const WVector &end, trace_s *pTrace, WPolySoup::sModel *pModel);
	int Get_WallSlidingAngle(const WVector &pos, WPolySoup::sModel *pModel, WVector &normal);
	float GetObbPos(Wobb &obb, bool bAll, int option);
	void OBBToAABB(Wobb obb, Waabb &aabb, WVector &center);
	WVector GetRayIntersectionObj(WVector from, WVector dir);
	void DrawDebugTraceAABB(const WVector &start, const WVector &end, const Waabb &box);
	void DrawDebugTraceOBB(const WVector &start, const WVector &end, const Waabb &box);
	float IntersectRollingBallGround(const WVector &pos, const WVector &dir, const float &radius, int &index, WVector &normal, bool bRolling, bool bWallsOnly, bool bothSides);
	void UpdateProperty(void);

protected:
	WVector ColTest(const WVector &pos, bool bBase, int *pIndex, bool bAll);
	float CheckCollision(const WVector &pos, const WVector &dir, int *pIndex, bool bAll);
	bool IsValid(const WVector &pos);
	void ScriptProcess(int index, float from, float to);
	void ParsePuppetScript(int index, const char *script);
	void BounceProcess(float elapsed);
	void RollProcess(float elapsed);
	void SpinProcess(float elapsed);
	float IntersectPlane(const WVector &orig, const WVector &dir, const WPlane &plane, bool bothSides);
	void CommitResponse(CGolfBall &ball, const sColResponse &response, int index);
	void RollingBallGroundResponse(CGolfBall &ball, float t, const WVector &dir, int index, const WVector &normal);
	bool ObjColTest(const WVector &pos, short objIndex, WPlane *pPlane, float &t, int &index);
	float IntersectObj(const WVector &pos, const WVector &dir, WPlane *pPlane);
	void ObjResponse(const WVector &dir, float t, const WVector &contactVector, sColResponse &response);
	void BounceResponse(const WVector &normal);

private:
	bool SameSide(const WVector &p1, const WVector &p2, const WVector &a, const WVector &b) const;
	WVector closestPointOnTriangle(const WVector &a, const WVector &b, const WVector &c, const WVector &p) const;
	WVector closestPointOnLine(const WVector &a, const WVector &b, const WVector &p) const;
	WVector DistanceBetweenRayNSegment(const WVector &rayOrig, const WVector &rayDir, const WVector &segOrig, const WVector &segDir);
	float GetRollCoeff(int property) const;
	float GetBounceCoeff(int property) const;
	float GetWeatherBounceCoeff(int property) const;
	float GetWeatherRollCoeff(int property) const;
	bool IsPenetrable(int property) const;
	unsigned long MakeRand(unsigned long range);
	unsigned char GetObjQuad(const WVector &pos, const WVector &center);
	float IntersectRayTriangle(const WVector &orig, const WVector &dir, const WVector *tri);
	void MakeAfterBurner(void);
	float intersectPolygon(const WVector &pos, const WVector &center, const WVector &dir) const;
	bool PointInTriangle(const WVector &p, const WVector &a, const WVector &b, const WVector &c) const;
	const char *FindTexture(int index);
	WVector GetCollisionPoint(const WVector &dir);
	void ClipDecalTriangle(const WPolySoup::sTriangle *pTri, WVector normal);
	void ClipDecalPolygon(WVector *pVerts, int numVerts, int plane);
	void MakeVectorSlide(float scale, float maxSpeed, float lift, int maxCount);
	float IntersectPoint(WVector pos);
	bool StuckInBunker(const WVector &pos);
	WVector GetFriction(void);
	void CalcObjMeshData(short objIndex, int faceIndex);
	WVector ApplyForce(void);
	void MakeCollisionData(short objIndex);
	void SetPenaltyPos(const Waabb &aabb);
	void HoleProcess(float elapsed);

	WPolySoup *m_pPolySoup;         // 0x0

public:
	WPolySoup::sModel *m_pBaseModel;    // 0x4, MakeBaseStructure's model (read by RealMyRoomData, ntAvatarChatMain)

private:
	int m_boxMinCol;                // 0x8, GetMeshList(Waabb)'s quad range
	int m_boxMinRow;                // 0xC
	int m_boxMaxCol;                // 0x10
	int m_boxMaxRow;                // 0x14
	int m_meshList[0x800];          // 0x18, the base model's triangles GetMeshList found (deduplicated)
	int m_meshNum;                  // 0x2018, their count
	short m_objList[0x20];          // 0x201C, the objects GetObjList found (indices into the soup's models)
	unsigned char m_objNum;         // 0x205C, their count
	PADDING(0x205D, 0x2060);
	int m_quadCols;                 // 0x2060, the base model's quad grid along x (20 units per quad)
	int m_quadRows;                 // 0x2064, the quad grid along z
	WVector m_quadOrigin;           // 0x2068, the grid's corner (x and z used)
	std::vector<sWater> m_waterList;    // 0x2074, AddWater/ResetWater
	WVector m_penaltyPos;           // 0x2084
	float m_rollFactor;             // 0x2090, scales GetWeatherRollCoeff (1.0 by the constructor)
	float m_bounceFactor;           // 0x2094, scales GetWeatherBounceCoeff (1.0 by the constructor)
	float m_condition;              // 0x2098, SetCondition: 0 dry, -2/-3 by the ball's ground in rain or snow
	Waabb m_greenBox;               // 0x209C, the green's bounding box (equal corners: no green plane)
	WVector m_greenCenter;          // 0x20B4, CalcGreenPlane: the point the green's plane passes through
	float m_greenSlope;             // 0x20C0, CalcGreenPlane: how far the plane tilts toward the ball
	WPlane m_greenPlane;            // 0x20C4, CalcGreenPlane; GetGreenPos's height on the green
	int m_markedTriangle;           // 0x20D4, the base model's triangle Display outlines, -1 = none (ResetHoleData)
	int m_bounceCount;              // 0x20D8, bounces before m_colList (GetBounceCount; ResetVariable zeroes it)
	WVector *m_pDecalVertices;      // 0x20DC, DecalFragments' output vertices (ClipDecalPolygon appends)
	int m_decalNumVertices;         // 0x20E0, the vertices written so far
	int m_decalMaxVertices;         // 0x20E4, the room in m_pDecalVertices
	WPlane m_decalPlanes[6];        // 0x20E8, the decal box's sides, normals outward (ClipDecalPolygon)
	Waabb m_avatarBox;              // 0x2148, SetAvatarPosition: the avatar's box in the room
	WPolySoup::sQuad *m_pQuads;     // 0x2160, the base model's collision quads (MakeBaseStructure; delete[] by the destructor)
	Waabb m_ballTestBox;            // 0x2164, BallProcess ray-tests the ball against it (RayIntersectAABB)
	unsigned char m_ballState;      // 0x217C, BallProcess sets 1, 2 or 3; ResetVariable clears it
	PADDING(0x217D, 0x2180);
	float m_holeCupRadius;          // 0x2180, RollProcess adds the ball's radius to it
	float m_cupBottom;              // 0x2184, ResetHoleData: the cup's height minus six ball radii
	int m_cupRollCount;             // 0x2188, RollProcess counts frames near the cup up to 75
	WVector m_cupApproach;          // 0x218C, BallProcess: DistanceBetweenRayNSegment of the cup's axis and the ball's step (y: the step fraction, z: the distance)
public:
	float m_lowest;                 // 0x2198
private:
	bool m_lowestGroundMode;        // 0x219C, GetBasePosY: with a ball in play, the base height is GetLowestGround's
	bool m_applyForce;              // 0x219D, tested by ApplyForce; ResetVariable sets it
	bool m_objColStop;              // 0x219E, set by ObjColTest when it gives up
	PADDING(0x219F, 0x21A0);
	WVector m_baseVtxColor;         // 0x21A0, SetBaseVtxColor: the base model's vertex colour (r, g, b, 0-255)
	WVector m_objVtxColor;          // 0x21AC, SetObjVtxColor: the objects' vertex colour (r, g, b, 0-255)
	int m_vtxColorTestMode;         // 0x21B8, a debug key toggles it 0/1 (COption::dIsVtxClrTestEnabled)
	float m_frameTime;              // 0x21BC, PreProcess: the time not yet turned into frames (50 per second)
	unsigned long m_frame;          // 0x21C0
	std::vector<CAniObj *> m_aniObjs;   // 0x21C4, the course's animated objects (Reserve)
	unsigned long m_randSeed;       // 0x21D4, MakeRand's state
	float m_slopeFactor;            // 0x21D8
	sColData m_colData;             // 0x21DC, the contact being built; CommitResponse appends it to m_colList

	std::vector<sColData> m_colList;    // 0x2200
	PADDING(0x2210, 0x2220);
	unsigned char *m_pBuffer2220;   // 0x2220, delete[] by the destructor (element type and users not found yet)
	unsigned char *m_pBuffer2224;   // 0x2224, delete[] (twice) by the destructor (element type and users not found yet)
	float m_gravity;                // 0x2228
};
CHECK_SIZE(CQuadTree, 0x222C);
CHECK_SIZE(CQuadTree::sColData, 0x24);
CHECK_FIELD(CQuadTree::sColData, m_objIndex, 0x2);
CHECK_FIELD(CQuadTree::sColData, m_frame, 0x8);
CHECK_FIELD(CQuadTree::sColData, m_point, 0xC);
CHECK_FIELD(CQuadTree::sColData, m_normal, 0x18);
CHECK_SIZE(CQuadTree::sWater, 0x1C);
CHECK_FIELD(CQuadTree::sWater, m_index, 0x18);
CHECK_SIZE(CQuadTree::sColResponse::GolfBallClone, 0x28);
CHECK_FIELD(CQuadTree::sColResponse::GolfBallClone, m_velocity, 0xC);
CHECK_FIELD(CQuadTree::sColResponse::GolfBallClone, m_lastContactId, 0x18);
CHECK_FIELD(CQuadTree::sColResponse::GolfBallClone, m_lastContactIndex, 0x1C);
CHECK_FIELD(CQuadTree::sColResponse::GolfBallClone, m_state, 0x20);
CHECK_FIELD(CQuadTree::sColResponse::GolfBallClone, m_pBall, 0x24);
// sColResponse: the size is not established (at least 0xB6).
CHECK_FIELD(CQuadTree::sColResponse, m_sound, 0x48);
CHECK_FIELD(CQuadTree::sColResponse, m_contactVector, 0x8C);
CHECK_FIELD(CQuadTree::sColResponse, m_point, 0x9C);
CHECK_FIELD(CQuadTree::sColResponse, m_normal, 0xA8);
CHECK_FIELD(CQuadTree::sColResponse, m_flagB4, 0xB4);
CHECK_FIELD(CQuadTree::sColResponse, m_flagB5, 0xB5);
BEGIN_LAYOUT_CHECK(CQuadTree)
	CHECK_FIELD(CQuadTree, m_pPolySoup, 0x0);
	CHECK_FIELD(CQuadTree, m_pBaseModel, 0x4);
	CHECK_FIELD(CQuadTree, m_waterList, 0x2074);
	CHECK_FIELD(CQuadTree, m_meshList, 0x18);
	CHECK_FIELD(CQuadTree, m_meshNum, 0x2018);
	CHECK_FIELD(CQuadTree, m_boxMinCol, 0x8);
	CHECK_FIELD(CQuadTree, m_boxMaxRow, 0x14);
	CHECK_FIELD(CQuadTree, m_quadCols, 0x2060);
	CHECK_FIELD(CQuadTree, m_quadRows, 0x2064);
	CHECK_FIELD(CQuadTree, m_quadOrigin, 0x2068);
	CHECK_FIELD(CQuadTree, m_objList, 0x201C);
	CHECK_FIELD(CQuadTree, m_objNum, 0x205C);
	CHECK_FIELD(CQuadTree, m_avatarBox, 0x2148);
	CHECK_FIELD(CQuadTree, m_bounceCount, 0x20D8);
	CHECK_FIELD(CQuadTree, m_pDecalVertices, 0x20DC);
	CHECK_FIELD(CQuadTree, m_decalNumVertices, 0x20E0);
	CHECK_FIELD(CQuadTree, m_decalMaxVertices, 0x20E4);
	CHECK_FIELD(CQuadTree, m_decalPlanes, 0x20E8);
	CHECK_FIELD(CQuadTree, m_condition, 0x2098);
	CHECK_FIELD(CQuadTree, m_penaltyPos, 0x2084);
	CHECK_FIELD(CQuadTree, m_lowest, 0x2198);
	CHECK_FIELD(CQuadTree, m_greenBox, 0x209C);
	CHECK_FIELD(CQuadTree, m_rollFactor, 0x2090);
	CHECK_FIELD(CQuadTree, m_markedTriangle, 0x20D4);
	CHECK_FIELD(CQuadTree, m_bounceFactor, 0x2094);
	CHECK_FIELD(CQuadTree, m_pQuads, 0x2160);
	CHECK_FIELD(CQuadTree, m_ballTestBox, 0x2164);
	CHECK_FIELD(CQuadTree, m_vtxColorTestMode, 0x21B8);
	CHECK_FIELD(CQuadTree, m_greenPlane, 0x20C4);
	CHECK_FIELD(CQuadTree, m_greenCenter, 0x20B4);
	CHECK_FIELD(CQuadTree, m_greenSlope, 0x20C0);
	CHECK_FIELD(CQuadTree, m_ballState, 0x217C);
	CHECK_FIELD(CQuadTree, m_holeCupRadius, 0x2180);
	CHECK_FIELD(CQuadTree, m_cupRollCount, 0x2188);
	CHECK_FIELD(CQuadTree, m_cupApproach, 0x218C);
	CHECK_FIELD(CQuadTree, m_cupBottom, 0x2184);
	CHECK_FIELD(CQuadTree, m_lowestGroundMode, 0x219C);
	CHECK_FIELD(CQuadTree, m_applyForce, 0x219D);
	CHECK_FIELD(CQuadTree, m_objColStop, 0x219E);
	CHECK_FIELD(CQuadTree, m_frameTime, 0x21BC);
	CHECK_FIELD(CQuadTree, m_frame, 0x21C0);
	CHECK_FIELD(CQuadTree, m_baseVtxColor, 0x21A0);
	CHECK_FIELD(CQuadTree, m_objVtxColor, 0x21AC);
	CHECK_FIELD(CQuadTree, m_aniObjs, 0x21C4);
	CHECK_FIELD(CQuadTree, m_slopeFactor, 0x21D8);
	CHECK_FIELD(CQuadTree, m_randSeed, 0x21D4);
	CHECK_FIELD(CQuadTree, m_colData, 0x21DC);
	CHECK_FIELD(CQuadTree, m_colList, 0x2200);
	CHECK_FIELD(CQuadTree, m_pBuffer2220, 0x2220);
	CHECK_FIELD(CQuadTree, m_pBuffer2224, 0x2224);
	CHECK_FIELD(CQuadTree, m_gravity, 0x2228);
END_LAYOUT_CHECK

// The pool of collision quadtrees; ms_InstanceIdx selects the current one.
class CPVSPool {
	LAYOUT_CHECK_FRIEND;
public:
	CQuadTree &GetInstance(void);
	static void SetInstanceIdx(int idx);

	static int ms_InstanceIdx;

private:
	CQuadTree m_pool[2];            // 0x0
};
CHECK_SIZE(CPVSPool, 0x4458);

extern int g_colNum;
extern char g_colTex[16][32]; // the texture names of the last contacts (IntersectObj)

// GolfRule.cpp defines GOLFRULE_QUADTREE_INLINE empty before its includes:
// the original holds the out-of-line copies of SetFrame and GetFrame in
// GolfRule's object (0x43df90, 0x43dfb0); every other includer inlines them.
#ifndef GOLFRULE_QUADTREE_INLINE
#define GOLFRULE_QUADTREE_INLINE inline
#endif

// quadtree.h:497
GOLFRULE_QUADTREE_INLINE void CQuadTree::SetFrame(unsigned long frame) {
	m_frame = frame;
	m_frameTime = 0.0f;
}

// quadtree.h:505
GOLFRULE_QUADTREE_INLINE unsigned long CQuadTree::GetFrame(void) {
	return m_frame;
}

void SetPVSIndex(int idx);
CQuadTree &GetPVS(void);

#endif // PG_CLIENT_PROJECTG_QUADTREE_H
