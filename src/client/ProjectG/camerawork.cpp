// Original: d:\build\custom\temp\code\client\projectg\camerawork.cpp
#define CAMERAWORK_WMATH_INLINE
#define CAMERAWORK_WPUPPET_INLINE
#include "camerawork.h"
#include "commonutil.h"
#include "SoundManager.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "wview.h"
#include "InputManager.h"
#include "TaskManager.h"
#include "actor.h"
#include "golfball.h"
#include "quadtree.h"
#include "scenemanager.h"
#include "wpuppet.h"
#include "wbone.h"
#include "wutil.h"
#include "club.h"
#include "wresrcmng.h"
#include "parttidlist.h"
#include "classdefine.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// Nameless data: the name ParsePuppetScript cuts out of a script, and the
// name it is compared with (a char * of another unit; size not established).
static char data_af1f60[256];
extern "C" char *data_a7ee68;

extern WMatrix g_camera;

// commonutil.cpp:714 (?Random@@YIMMMH@Z); commonutil.h does not declare it yet.
float Random(float min, float max, int seed);

// Nameless .rdata floats of Wangreal's math (pi, pi/2, degrees to radians,
// radians to degrees, epsilon), read through memory rather than literals.
extern "C" const float data_9eb420[5];

// One camera of the selection tables ReplayCamera cycles through (header
// not identified; 8 bytes).
struct sCameraSelectionData {
	int m_weight;                   // 0x0, the camera is skipped when <= 0
	bool m_checkBallVisibility;     // 0x4, drive cameras: switch away when the ball is hidden
	bool m_ballVisibilitySafe;      // 0x5, the constructor lists it in m_safeCameras
	PADDING(0x6, 0x8);
};
CHECK_SIZE(sCameraSelectionData, 0x8);
CHECK_FIELD(sCameraSelectionData, m_checkBallVisibility, 0x4);
CHECK_FIELD(sCameraSelectionData, m_ballVisibilitySafe, 0x5);

// What the caddie actor hands back for message 0x1e0 (type and header not
// identified; only the byte CheckCameraType reads).
struct sCaddieShotState {
	UNKNOWN(0x0, 0x3d);
	bool m_keepLongShotCamera;      // 0x3d, CheckCameraType keeps the long-shot camera (type 10) while set
};
CHECK_FIELD(sCaddieShotState, m_keepLongShotCamera, 0x3d);

// The putt cameras (camera ids 100..112) and the drive cameras (ids 0..28),
// in the order SelectCamera and ReplayCamera index them.
sCameraSelectionData puttTable[] = {
	{ 1, false, false }, // 0
	{ 1, false, false }, // 1
	{ 1, false, false }, // 2
	{ 1, false, false }, // 3
	{ 1, false, false }, // 4
	{ 0, false, false }, // 5
	{ 0, false, false }, // 6
	{ 1, false, false }, // 7
	{ 1, false, false }, // 8
	{ -1, false, false }, // 9
	{ -1, false, false }, // 10
	{ -1, false, false }, // 11
	{ -1, false, false }, // 12
};
sCameraSelectionData driveTable[] = {
	{ 1, true, false }, // 0
	{ 1, true, false }, // 1
	{ -1, true, false }, // 2
	{ 1, true, false }, // 3
	{ 1, true, false }, // 4
	{ 0, true, true }, // 5
	{ 0, false, false }, // 6
	{ 0, false, false }, // 7
	{ 1, true, false }, // 8
	{ 1, true, false }, // 9
	{ 1, true, false }, // 10
	{ 1, true, false }, // 11
	{ 1, true, false }, // 12
	{ -1, true, false }, // 13
	{ -1, true, false }, // 14
	{ 0, false, false }, // 15
	{ 0, false, false }, // 16
	{ 0, false, false }, // 17
	{ 1, true, false }, // 18
	{ -1, true, false }, // 19
	{ 0, false, false }, // 20
	{ -1, true, false }, // 21
	{ -1, true, false }, // 22
	{ 1, true, false }, // 23
	{ 0, false, false }, // 24
	{ 0, false, false }, // 25
	{ 0, false, false }, // 26
	{ 0, false, false }, // 27
	{ 0, false, true }, // 28
};

// Minimum helper of an unidentified header (also local in PetBody.cpp).
template <class T> T MinOf(T a, T b) { return a > b ? b : a; }
template <class T> T MaxOf(T a, T b) { return a < b ? b : a; }

// Clamp written out where the original's line marks show no inline
// function (a macro of an unidentified header).
#define CAMERA_CLAMP(v, lo, hi) ((v) < (lo) ? (lo) : ((v) > (hi) ? (hi) : (v)))

// Camera ids: drive cameras from 0, putt cameras from 100 (SelectCamera).
enum { DRIVE_CAMERA_BASE = 0, PUTT_CAMERA_BASE = 100 };

// camerawork.cpp:98
CCameraWork::CCameraWork()
	: m_pCameraProc(0), m_driveWeightTotal(0), m_puttWeightTotal(0), m_destFollowsGround(false) {
	m_driveCameraCount = 29;
	m_puttCameraCount = 13;

	for (int i = 0; i < m_driveCameraCount; i++) {
		m_driveWeightTotal += driveTable[i].m_weight > 0 ? driveTable[i].m_weight : 0;

		if (driveTable[i].m_ballVisibilitySafe)
			m_safeCameras.push_back(i + DRIVE_CAMERA_BASE);
	}

	for (int i = 0; i < m_puttCameraCount; i++) {
		m_puttWeightTotal += puttTable[i].m_weight > 0 ? puttTable[i].m_weight : 0;

		if (puttTable[i].m_ballVisibilitySafe)
			m_safeCameras.push_back(i + PUTT_CAMERA_BASE);
	}

	memset(m_holeInCams, 0, sizeof(m_holeInCams));
	m_pCameraPuppet = 0;
	m_pScriptPuppet = 0;
	m_pScriptMotion = 0;
	m_bankAngle = 0.0f;
	m_pTarget = &GolfBall().m_pos;
	m_freePitch = false;
	m_keepInputOffsets = false;
	m_cameraChecking = true;
	m_inputMode = 1;
	m_installed = false;
}

// camerawork.cpp:136
CCameraWork::~CCameraWork() {
	m_cameraQueue.clear();

	for (int i = 0; i < 4; i++) {
		if (g_resrcmng && m_holeInCams[i].m_pPuppet) { g_resrcmng->Release(m_holeInCams[i].m_pPuppet, false); m_holeInCams[i].m_pPuppet = 0; }
	}

	if (g_resrcmng && m_pCameraPuppet) { g_resrcmng->Release(m_pCameraPuppet, false); m_pCameraPuppet = 0; }
}

// camerawork.cpp:150
void CCameraWork::Load(void) {
	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (GOLFDOC()->GetPlayer(i)->m_state == 3)
			continue;

		unsigned long tidChar = GOLFDOC()->GetPartTidList(i).m_tidChar;

		if (FindHoleInCam(tidChar))
			continue;

		for (int j = 0; j < GOLFDOC()->GetPlayerNum(); j++) {
			if (GOLFDOC()->GetPlayer(j)->m_state == 3)
				continue;

			if (m_holeInCams[j].m_id == 0) {
				IFF_STRUCT::sChar *pChar = ItemManager()->FindChar(tidChar);

				if (pChar) {
					char name[64];
					sprintf(name, "%s_cam.apet", pChar->m_model);
					m_holeInCams[j].m_pPuppet = g_resrcmng->GetPuppet(name, false, true, false);
				} else {
					m_holeInCams[j].m_pPuppet = 0;
				}

				m_holeInCams[j].m_id = tidChar;

				if (m_holeInCams[j].m_pPuppet == 0) {
					if ((tidChar & 0x3FFFFFF) == 4)
						m_holeInCams[j].m_pPuppet = g_resrcmng->GetPuppet("m_def_cam.apet", false, true, false);
					else
						m_holeInCams[j].m_pPuppet = g_resrcmng->GetPuppet("f_def_cam.apet", false, true, false);
				}
				break;
			}
		}
	}

	m_skipTurn = false;
}

// camerawork.cpp:205
WPuppet *CCameraWork::FindHoleInCam(unsigned long id) {
	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (m_holeInCams[i].m_id == id)
			return m_holeInCams[i].m_pPuppet;
	}
	return 0;
}

// camerawork.cpp:221
void CCameraWork::ReplayCamera(bool keep) {
	if (!keep)
		m_replayCameras.clear();

	int index;

	if (GolfClub().GetType() == 3) {
		index = m_cameraIndex - 100;

		do {
			index = (index + 1) % m_puttCameraCount;
		} while (puttTable[index].m_weight <= 0);

		index += 100;
	} else {
		index = m_cameraIndex;

		do {
			index = (index + 1) % m_driveCameraCount;
		} while (driveTable[index].m_weight <= 0);
	}

	m_input.Reset();
	m_cameraIndex = index;
	m_nextCameraType = -1;
	m_stateFlag = false;
	m_keepInputOffsets = false;
	m_zoomAngle = 0.95993108f;
	m_replayPos = 0;
	SetCamera(index, true);
	CheckCameraType(true);
	InitCtrlPoints(m_cameraType);
}

// camerawork.cpp:266
void CCameraWork::SelectCamera(int type) {
	m_input.Reset();
	m_nextCameraType = -1;
	m_destFollowsGround = false;
	m_pScriptMotion = 0;

	m_baseMatrix = g_camera;
	m_baseMatrixCopy = g_camera;

	m_zoomAngle = 0.95993108f; m_keepInputOffsets = type == -1;
	m_fixedCameraIndex = rand();
	m_replayPos = -1;
	m_replayCameras.clear();
	m_pTarget = &GolfBall().m_pos;

	if (type == -1) {
		if (GolfClub().GetType() == 3) {
			int pick = m_fixedCameraIndex % m_puttWeightTotal;
			type = 100;

			for (int i = 0; i < m_puttCameraCount; i++) {
				pick -= puttTable[i].m_weight > 0 ? puttTable[i].m_weight : 0;

				if (pick < 0) {
					type = i + 100;
					break;
				}
			}
		} else {
			int pick = m_fixedCameraIndex % m_driveWeightTotal;
			type = 0;

			for (int i = 0; i < m_driveCameraCount; i++) {
				pick -= driveTable[i].m_weight > 0 ? driveTable[i].m_weight : 0;

				if (pick < 0) {
					type = i;
					break;
				}
			}
		}
	}

	m_cameraIndex = type;
	m_pScriptPuppet = 0;
	SetCamera(type, true);

	if (type < 200) {
		CheckCameraType(false);
		InitCtrlPoints(m_cameraType);
	}
}

// camerawork.cpp:340
void CCameraWork::SetCaddieCamera(const WMatrix &matrix, const WVector &target) {
	m_keepInputOffsets = false;
	m_destFollowsGround = false;
	m_zoomAngle = 0.95993108f;
	m_baseMatrix = matrix;
	SetCamera(0xCA, true);

	m_baseMatrix.GetRotMatrix(&m_baseRotation);
	WVector dir;
	dir = WVector::UNIT_POS_Z * m_baseRotation;
	WVector toTarget;
	toTarget = target - GolfBall().m_pos;
	m_swingAngle = CalcDeltaAngle(toTarget, dir);
	m_sourceCamera = toTarget * 0.5f + GolfBall().m_pos + WVector(0.0f, 10.0f, 0.0f);
	m_introFrom = m_sourceCamera;
	m_introTo = matrix.pivot;
	m_baseMatrixCopy = m_baseMatrix;
}

// camerawork.cpp:363
float CCameraWork::LoadCamera(eCamMode mode, char *name, const WVector &target) {
	m_destFollowsGround = true; m_keepInputOffsets = false; m_scriptTarget = target;

	m_pTarget = &m_scriptTarget;
	m_bankAngle = 0.0f;

	if (mode == 0 || mode == 2) {
		unsigned long tidChar = GOLFDOC()->GetPartTidList(GOLFDOC()->m_myPlayerIndex).m_tidChar;

		m_pScriptPuppet = FindHoleInCam(tidChar);
	} else if (mode == 1) {
		if (m_pCameraPuppet == 0) {
			switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
			case 0:
			case 3:
			case 7:
				m_pCameraPuppet = g_resrcmng->GetPuppet("rank_stroke.apet", false, true, false);
				break;
			case 1:
				m_pCameraPuppet = g_resrcmng->GetPuppet("rank_match.apet", false, true, false);
				break;
			}
		}

		m_pScriptPuppet = m_pCameraPuppet;
	} else if (mode == 3) {
		unsigned long tidChar = GOLFDOC()->GetPartTidList(GOLFDOC()->m_myPlayerIndex).m_tidChar;
		m_pScriptPuppet = FindHoleInCam(tidChar);

		name = 0;

		rand();

		AfxGetTask()->GetActor("Player") << MsgObject(0, 0x2715, (intptr_t)&name, 0, 0, 0, 0);
		name = MakeStr("%s_%02d", name, 1);
	} else {
		m_pScriptPuppet = 0;
	}

	if (m_pScriptPuppet)
		m_pScriptMotion = m_pScriptPuppet->FindMotionData(name);
	else
		m_pScriptMotion = 0;

	InitVars();

	if (m_pScriptMotion) {
		m_cameraTime = m_pScriptMotion->s;
		float length = m_pScriptMotion->e - m_pScriptMotion->s;

		for (w_motion_data *next = m_pScriptPuppet->FindMotionData(m_pScriptMotion->nextmotion); next; next = m_pScriptPuppet->FindMotionData(next->nextmotion)) {
			float nextLength = next->e - next->s;
			if (WAbsBySign(nextLength) < data_9eb420[4])
				break;

			length += nextLength;

			if ((mode == 0 || mode == 203) && length > 10.0f) {
				length = 10.0f;
				break;
			}
		}

		m_transitionTime = length + m_pScriptMotion->s;
		return length;
	}

	m_transitionTime = -1.0f;
	return -1.0f;
}

// camerawork.cpp:475
WVector CCameraWork::GetSourceCamera(void) {
	return m_sourceCamera;
}

// camerawork.cpp:479
WVector CCameraWork::GetDestCamera(void) {
	return m_destCamera;
}

// camerawork.cpp:483
void CCameraWork::SetDestCamera(const WVector &dest) {
	m_destCamera = dest;
}

// camerawork.cpp:488
void CCameraWork::SetInstalledCamera(const WVector &source) {
	SetCamera(0x15, false);
	m_sourceCamera = source;

	m_installed = true;
}

// camerawork.cpp:497
void CCameraWork::UninstallCamera(void) {
	m_installed = false;
}

// camerawork.cpp:502
void CCameraWork::SetCamera(int type, bool immediately) {
	if (m_installed)
		return;

	CGolfBall &ball = GolfBall();

	int lastIndex = ball.m_preDataCount;
	int midIndex = ball.m_eventPreDataIndex[1] > 0 ? ball.m_eventPreDataIndex[1] : 0;
	int landIndex = ball.m_eventPreDataIndex[0] > 0 ? ball.m_eventPreDataIndex[0] : 0;

	m_cameraType = type;
	m_ballHiddenFrame = -1;

	int replayCount = m_replayCameras.size();

	if (m_replayPos >= 0 && m_replayPos < replayCount) {
		m_cameraType = m_replayCameras[m_replayPos++];
	} else {
		m_replayPos = -1;
		m_replayCameras.push_back(m_cameraType);
	}

	if (!m_keepInputOffsets) {
		m_input.Reset();
	}
	if (immediately) {
		m_sourceCamera = m_baseMatrix.pivot;
		m_destCamera = m_baseMatrix.za + m_baseMatrix.pivot;
		m_baseMatrixCopy = GetCameraMatrix(m_sourceCamera, m_destCamera, m_bankAngle);

		InitVars();
	}

	if (m_cameraType < 100 && immediately)
		m_phase0Time = 75.0f;
	else
		m_phase0Time = -1.0f;

	m_bankAngle = 0.0f;
	m_processedFrame = -2;
	if (m_cameraType < 100 && immediately && GolfBall().m_preDataCount < 50) {
		m_pCameraProc = NULL;
		return;
	}

	switch (m_cameraType) {
	case 1:
		m_phase0Time = -1.0f;
		m_pCameraProc = &CCameraWork::LookBackCamera;
		break;
	case 2:
		m_pCameraProc = &CCameraWork::Rail_or_SideCamera;
		break;
	case 3: {
		WVector dir;
		dir = WCrossProduct(WVector(0.0f, m_fixedCameraIndex % 2 ? 1.0f : -1.0f, 0.0f), ball.PreData(midIndex)->m_velocity);
		dir = dir.Normalize() * 10.0f;
		float height = (float)(m_fixedCameraIndex % 11) - 5.0f;
		const CGolfBall::sExpected *pMid = ball.PreData(midIndex);
		m_phase0SourceCamera = WVector(dir.x + pMid->m_pos.x, height + pMid->m_pos.y + dir.y, dir.z + pMid->m_pos.z);
		m_flags = m_fixedCameraIndex % 2 + 1;
		m_pCameraProc = &CCameraWork::PanCamera;
		break;
	}
	case 4:
	case 11: {
		WVector dir;
		dir = ball.PreData(lastIndex)->m_pos - ball.PreData(landIndex)->m_pos;

		if (WisZero(dir.x, 1e-5f) && WisZero(dir.z, 1e-5f)) {

			dir = ball.PreData(lastIndex)->m_pos - ball.PreData(0)->m_pos;

			dir.y = 0.0f; dir.Normalize();

		} else {


			dir.y = 0.0f; dir = RotVec(dir.Normalize(), RotMat(Random(-2.0943952f, 2.0943952f, m_fixedCameraIndex), 1));
		}


		if (ball.m_landed)
			m_phase0SourceCamera = dir * 50.0f + ball.PreData(lastIndex)->m_pos + WVector(0.0f, 25.0f, 0.0f);
		else
			m_phase0SourceCamera = dir * 20.0f + ball.PreData(lastIndex)->m_pos + WVector(0.0f, 10.0f, 0.0f);

		if (m_cameraType == 4)
			m_pCameraProc = &CCameraWork::DropCamera;
		else
			m_pCameraProc = &CCameraWork::Drop_and_ZoomCamera;

		break;
	}
	case 5:
		if (GolfClub().GetType() == 3)
			m_phase0Time = 25.0f;

		if (m_preDataFrame > 10) {
			m_destCamera = ball.PreData(m_preDataFrame - 10)->m_pos;

			WVector dir;
			dir = ball.PreData(m_preDataFrame - 10)->m_pos - ball.PreData(m_preDataFrame - 9)->m_pos;

			dir.y = 0.0f; dir.Normalize();

			dir.y = 0.5f; dir.Normalize();

			m_sourceCamera = dir * 40.0f + ball.PreData(m_preDataFrame)->m_pos;
		}

		m_zoomAngle = 0.95993109f;
		m_pCameraProc = &CCameraWork::Chase_and_FollowCamera;
		break;
	case 7:
		m_phase0Time = -1.0f;
		m_pCameraProc = &CCameraWork::CircleCamera;
		break;
	case 8:
		m_pCameraProc = &CCameraWork::Chase_and_LookBackCamera;
		break;
	case 9: {
		m_phase0Time = -1.0f;
		WVector side;
		side = WCrossProduct(WVector::UNIT_POS_Y, ball.PreData(midIndex)->m_velocity);
		side = side.Normalize() * 40.0f;

		side.y += 8.0f;
		WVector vel;
		vel = ball.PreData(midIndex)->m_velocity;
		m_sourceCamera = side + ball.PreData(0)->m_pos + vel.Normalize() * ((float)(m_fixedCameraIndex % 11) - 5.0f);
		m_destCamera = ball.PreData(0)->m_pos;
		m_destCamera.y += 2.0f;
		m_pCameraProc = &CCameraWork::ZoomInCamera;
		break;
	}
	case 10:
		m_pCameraProc = &CCameraWork::CaddyViewCamera;
		break;
	case 18: {
		m_phase0Time = -1.0f;
		WVector side;
		side = WCrossProduct(WVector::UNIT_POS_Y, ball.PreData(midIndex)->m_velocity);
		side = side.Normalize() * 40.0f;

		side.y += 8.0f;
		WVector vel;
		vel = ball.PreData(midIndex)->m_velocity;
		m_sourceCamera = side + ball.PreData(0)->m_pos + vel.Normalize() * ((float)(m_fixedCameraIndex % 11) - 5.0f);
		m_destCamera = ball.PreData(0)->m_pos;
		m_destCamera.y += 2.0f;
		m_swingAngle = 0.2617994f;
		m_pCameraProc = &CCameraWork::FollowWithSwingCamera;
		break;
	}
	case 19:
		m_phase0Time = -1.0f;
		m_cameraTime = 0.0f;
		m_swingAngle = 0.0f;
		m_pCameraProc = &CCameraWork::SidePanWithZoomInCamera;
		break;
	case 12: {
		m_phase0Time = -1.0f;
		WVector side;
		side = WCrossProduct(WVector::UNIT_POS_Y, ball.PreData(midIndex)->m_velocity);
		side = side.Normalize() * 40.0f;

		side.y += 8.0f;
		WVector vel;
		vel = ball.PreData(midIndex)->m_velocity;
		m_sourceCamera = side + ball.PreData(0)->m_pos + vel.Normalize() * ((float)(m_fixedCameraIndex % 11) - 5.0f);
		m_destCamera = ball.PreData(0)->m_pos;
		m_destCamera.y += 2.0f;
		m_pCameraProc = &CCameraWork::BezierCamera;
		break;
	}
	case 13: {
		m_phase0Time = -1.0f;
		WVector dir;
		dir = WCrossProduct(WVector(0.0f, m_fixedCameraIndex % 2 ? 1.0f : -1.0f, 0.0f), ball.PreData(0)->m_velocity);

		dir.y = 0.0f; dir = dir.Normalize() * 30.0f;

		m_sourceCamera = (ball.PreData(lastIndex)->m_pos - ball.PreData(0)->m_pos) * 0.5f + ball.PreData(0)->m_pos + dir;
		m_sourceCamera.y = ball.PreData(midIndex)->m_pos.y + 2.0f;

		m_pCameraProc = &CCameraWork::FromMidPointCamera;
		break;
	}
	case 14: {
		m_phase0Time = -1.0f;
		int frame = m_workLength;
		WVector dir;
		dir = (ball.PreData(frame - 1)->m_pos - ball.PreData(frame)->m_pos).Normalize()
		    + (ball.PreData(frame + 1)->m_pos - ball.PreData(frame)->m_pos).Normalize();

		dir.y = 0.0f; dir = dir.Normalize() * 160.0f;
		m_sourceCamera = dir + ball.PreData(m_workLength)->m_pos;
		m_sourceCamera.y += 96.0f;
		m_pCameraProc = &CCameraWork::FromColPointCamera;
		break;
	}
	case 20: {
		WPuppet *pPuppet = CSceneManager::Instance()->GetPuppet(MakeStr("PLAYER_%d", GOLFDOC()->m_myPlayerIndex));

		WBone *pHeadNub = pPuppet->FindBone("Bip01 HeadNub"); WBone *pHead = pPuppet->FindBone("Bip01 Head");
		m_destCamera = 0.7f * pHead->GetMatrix().pivot + 0.3f * pHeadNub->GetMatrix().pivot;
		m_sourceCamera = m_destCamera - RotMat(GOLFDOC()->GetCameraData().m_yaw + 0.34906584f, 1).za * 3.0f;
		m_sourceCamera.y += 0.5f;
		m_phase0Time = -1.0f;
		m_pCameraProc = &CCameraWork::PangyaACamera;
		break;
	}
	case 21:
		m_swingAngle = 1.5707964f;
		m_destCamera = ball.PreData(0)->m_pos + WVector(0.0f, 3.0f, 0.0f);
		m_sourceCamera = ball.PreData(0)->m_pos - RotMat(GOLFDOC()->GetCameraData().m_yaw + m_swingAngle, 1).za * 15.0f + WVector(0.0f, 3.0f, 0.0f);
		m_pCameraProc = &CCameraWork::FixedAtSideCamera;
		break;
	case 22:
		m_destCamera = GolfBall().PreData(0)->m_pos + WVector(0.0f, 3.0f, 0.0f);
		m_sourceCamera = RotMat(GOLFDOC()->GetCameraData().m_yaw + Random(-1.5707964f, 1.5707964f, m_fixedCameraIndex), 1).za * 30.0f + ball.PreData(0)->m_pos + WVector(0.0f, 10.0f, 0.0f);
		m_pCameraProc = &CCameraWork::FromRandomPointCamera;

		break;
	case 23:
		m_phase0Time = 0.0f;
		m_pCameraProc = &CCameraWork::BankFromHeadCamera;
		break;
	case 24:
		if (m_replayMode != 0 || m_pScriptPuppet != NULL) {

			m_phase0Time = -1.0f;
		} else {



			m_phase0Time = 25.0f;

			WMatrix rot = RotMat(GOLFDOC()->GetCameraData().m_yaw, 1);

			m_sourceCamera = ball.PreData(0)->m_pos + RotVec(WVector(10.0f, 0.0f, -20.0f), rot);
			m_destCamera = ball.PreData(0)->m_pos + RotVec(WVector(0.0f, 2.0f, -2.0f), rot);
		}

		m_pCameraProc = &CCameraWork::CobraACamera;
		break;
	case 25:
		if (m_replayMode != 0 || m_pScriptPuppet != NULL) {

			m_phase0Time = -1.0f;
		} else {



			m_phase0Time = 25.0f;

			m_preDataFrame = GolfBall().m_preDataIndex;

			if (m_preDataFrame < GolfBall().m_eventPreDataIndex[9]) {

				m_sourceCamera = GOLFDOC()->GetCameraData().m_matrix.pivot;
				m_destCamera = m_sourceCamera + GOLFDOC()->GetCameraData().m_matrix.za;
			} else {



				m_bankAngle = 0.2617994f - (float)(m_preDataFrame - GolfBall().m_eventPreDataIndex[9]) * 0.0017453292f;

				m_sourceCamera = GolfBall().PreData((GolfBall().m_eventPreDataIndex[9] + m_preDataFrame * 4) / 5)->m_pos;
				m_sourceCamera.y += 1.0f;

				m_destCamera = GolfBall().m_pos;
			}
		}

		m_pCameraProc = &CCameraWork::CobraBCamera;
		break;
	case 26:
		if (m_replayMode != 0 || m_pScriptPuppet != NULL)
			m_phase0Time = -1.0f;
		else

			m_phase0Time = 25.0f;

		m_pCameraProc = &CCameraWork::TomahawkACamera;
		break;
	case 27: {
		m_phase0Time = 25.0f;
		WVector side;
		side = WCrossProduct(WVector::UNIT_POS_Y, ball.PreData(midIndex)->m_velocity);
		side = side.Normalize() * 40.0f;

		side.y += 8.0f;
		WVector vel;
		vel = ball.PreData(midIndex)->m_velocity;
		m_sourceCamera = side + ball.PreData(0)->m_pos + vel.Normalize() * ((float)(m_fixedCameraIndex % 11) - 5.0f);
		m_destCamera = ball.PreData(0)->m_pos;
		m_destCamera.y += 2.0f;

		m_pCameraProc = &CCameraWork::SpikeACamera;
		break;
	}
	case 28: {
		float dist = Between(100.0f, ball.PreData(m_preDataFrame)->m_velocity.Magnitude() * 2.0f, 150.0f);

		if (m_preDataFrame > 10) {



			m_destCamera = ball.PreData(m_preDataFrame - 10)->m_pos;

			WVector dir;
			dir = ball.PreData(m_preDataFrame - 10)->m_pos - ball.PreData(m_preDataFrame - 9)->m_pos;

			dir.y = 0.0f; dir.Normalize();

			dir.y = 2.0f; dir.Normalize();

			m_sourceCamera = dist * dir + ball.PreData(m_preDataFrame)->m_pos;
		}

		m_zoomAngle = 0.95993109f;
		m_pCameraProc = &CCameraWork::TopViewCamera;
		break;
	}
	case 101: {
		WVector side;
		side = WCrossProduct(WVector::UNIT_POS_Y, ball.PreData(0)->m_velocity);
		side.Normalize();
		m_sourceCamera = side * 16.0f + ball.PreData(0)->m_pos + WVector(0.0f, 32.0f, 0.0f);
		m_destCamera = ball.PreData(0)->m_pos;
		m_destCamera.y += 2.5f;
		m_pCameraProc = &CCameraWork::SideCamera;
		break;
	}
	case 102: {
		WVector dir;
		dir = ball.PreData(0)->m_velocity;
		m_sourceCamera = ball.PreData(0)->m_pos + WVector(0.0f, 25.0f, 0.0f) - dir.Normalize() * 5.0f;
		m_destCamera = ball.PreData(0)->m_pos;
		m_pCameraProc = &CCameraWork::JustLookingCamera;
		break;
	}
	case 103: {
		WVector dir;
		dir = (ball.PreData(0)->m_pos - GetHoleCupPos()).Normalize();

		m_sourceCamera = GetHoleCupPos() + WVector(0.0f, 25.0f, 0.0f)
		               - (dir + WCrossProduct(WVector(0.0f, m_fixedCameraIndex % 2 ? 1.0f : -1.0f, 0.0f), dir).Normalize()) * 3.0f;
		m_destCamera = ball.PreData(0)->m_pos;
		m_pCameraProc = &CCameraWork::JustLookingCamera;
		break;
	}
	case 104: {
		WVector dir;
		dir = (ball.PreData(0)->m_pos - ball.PreData(ball.m_preDataCount)->m_pos).Normalize();
		m_sourceCamera = WCrossProduct(WVector(0.0f, m_fixedCameraIndex % 2 ? 1.0f : -1.0f, 0.0f), dir).Normalize() * 3.0f
		               + ball.PreData(ball.m_preDataCount / 2)->m_pos;
		m_destCamera = ball.PreData(0)->m_pos;
		m_pCameraProc = &CCameraWork::JustLookingCamera;
		break;
	}
	case 105:
		m_pCameraProc = &CCameraWork::BallSideCamera;
		break;
	case 106:
		m_destCamera = ball.PreData(0)->m_pos + WVector(0.0f, 3.0f, 0.0f);
		m_sourceCamera = m_destCamera - RotMat(GOLFDOC()->GetCameraData().m_yaw + 1.7453293f, 1).za * 16.0f;
		m_pCameraProc = &CCameraWork::FollowBallCamera;
		m_firstCamera = false;
		break;
	case 107: {
		WVector dir;
		dir = GetHoleCupPos() - ball.PreData(0)->m_pos;

		dir.y = 0.0f; m_sourceCamera = ball.PreData(0)->m_pos + dir.Normalize() * 20.0f * RotMat(-2.6179938f, 1);
		m_sourceCamera.y += 20.0f;
		m_destCamera = (ball.PreData(0)->m_pos + GetHoleCupPos()) * 0.5f;
		m_pCameraProc = &CCameraWork::FixedCamera;
		break;
	}
	case 108: {
		WVector dir;
		dir = GetHoleCupPos() - ball.PreData(0)->m_pos;

		dir.y = 0.0f; m_sourceCamera = GetHoleCupPos() + dir.Normalize() * 20.0f * RotMat(-1.0471976f, 1);
		m_sourceCamera.y += 20.0f;
		m_destCamera = (ball.PreData(0)->m_pos + GetHoleCupPos()) * 0.5f;
		m_pCameraProc = &CCameraWork::FixedCamera;
		break;
	}
	case 109: {
		WVector dir;
		dir = GetHoleCupPos() - ball.PreData(0)->m_pos;

		dir.y = 0.0f; m_sourceCamera = GetHoleCupPos() + dir.Normalize() * 10.0f * RotMat(-1.5707964f, 1);
		m_sourceCamera.y += 20.0f;
		m_destCamera = ball.m_pos;
		m_pCameraProc = &CCameraWork::JustLookingCamera;
		break;
	}
	case 110: {
		WVector dir;
		dir = ball.m_velocity;
		m_sourceCamera = GetHoleCupPos() - dir.Normalize() * 10.0f;
		m_sourceCamera.y += 10.0f;
		m_destCamera = ball.m_pos;
		m_pCameraProc = &CCameraWork::JustLookingCamera;
		break;
	}
	case 100:
		m_pCameraProc = &CCameraWork::JustLookingCamera;
		break;
	case 111: {
		WVector dir;
		dir = GetHoleCupPos() - ball.m_pos;

		dir.y = 0.0f; m_sourceCamera = GetHoleCupPos() + dir.Normalize() * 15.0f * RotMat(-0.34906584f, 1);
		m_sourceCamera.y += 20.0f;
		m_destCamera = ball.m_pos;
		m_pCameraProc = &CCameraWork::PuttSecondACamera;
		break;
	}
	case 112: {
		WVector dir;
		dir = GetHoleCupPos() - ball.PreData(0)->m_pos;

		dir.y = 0.0f; dir.Normalize();
		m_sourceCamera = GetHoleCupPos() + dir * 60.0f * RotMat(-0.052359879f, 1);
		m_destCamera = ball.m_pos;
		m_zoomAngle = 0.17453292f;
		m_pCameraProc = &CCameraWork::PuttSecondBCamera;
		break;
	}
	case 150:
		m_sourceCamera = GetHoleCupPos() - WVector(0.0f, 2.0f, 0.0f);
		m_destCamera = m_sourceCamera + WVector(0.0f, 1.0f, 0.2f);
		m_pCameraProc = &CCameraWork::FromHoleCupCamera;
		break;
	case 205:
		m_baseMatrixCopy = GOLFDOC()->GetDefaultCamera();
		m_sourceCamera = m_baseMatrixCopy.pivot;
		m_destCamera = m_sourceCamera + m_baseMatrixCopy.za;
		m_zoomAngle = 0.017453292f;
		m_transitionTime = 1.0f;
		m_cameraTime = 0.0f;
		m_phase0Time = -1.0f;
		m_pCameraProc = &CCameraWork::ZoomOutCamera;
		break;
	case 203:
		if ((IsMassGame() && GOLFDOC()->m_currentHole != WSingleton<CSharedDoc>::Instance()->m_holeOrder[0]) || WSingleton<CSharedDoc>::Instance()->m_roomType == 10) {

			m_sourceCamera = GolfBall().m_pos - RotMat(GOLFDOC()->GetCameraData().m_yaw, 1).za * 25.0f;
			m_sourceCamera.y += 5.0f;
			m_destCamera = m_sourceCamera + RotMat(GOLFDOC()->GetCameraData().m_yaw, 1).za;
			m_baseMatrixCopy = GetCameraMatrix(m_sourceCamera, m_destCamera, 0.0f);






			SetCamera(204, false);

			break;
		} else {

			float time = LoadCamera((eCamMode)3, NULL, GetTeeShotPos());








			m_firstCamera = false; m_phase0Time = -1.0f;
			if (time > 0.0f) {
				m_pCameraProc = NULL;
			} else {
				m_destFollowsGround = false;


				m_pCameraProc = &CCameraWork::IntroACamera;

				m_swingAngle = 2.6179938f;
				m_introDistance = 80.0f;
				m_introHeight = 15.0f;
			}
		}
		break;
	case 204:
		m_fromRotation = m_baseMatrixCopy;
		m_toRotation = GOLFDOC()->GetDefaultCamera();
		m_introFrom = m_baseMatrixCopy.pivot;
		m_introTo = GOLFDOC()->GetCameraData().m_matrix.pivot;
		m_cameraTime = 0.0f;
		m_transitionTime = 1.0f;
		m_phase0Time = -1.0f;

		m_pCameraProc = &CCameraWork::ToDefaultCamera;

		break;
	case 200:
		m_pCameraProc = &CCameraWork::FirstPersonCamera;
		break;
	case 201:
		m_pCameraProc = &CCameraWork::StillFrameCamera;
		break;
	case 202:
		m_phase0Time = -1.0f;
		m_pCameraProc = &CCameraWork::CaddieChanceCamera;
		break;
	case 206:
		m_phase0Time = -1.0f;
		switch (m_replayMode) {
		case 1:
			m_sourceCamera = GolfBall().m_pos + (GOLFDOC()->GetCameraData().m_matrix.ya + GOLFDOC()->GetCameraData().m_matrix.za) * 32.0f * RotMat(2.0943952f, 1);
			m_destCamera = GolfBall().m_pos;
			break;


		case 2:
			m_sourceCamera = GolfBall().m_pos + (GOLFDOC()->GetCameraData().m_matrix.ya + GOLFDOC()->GetCameraData().m_matrix.za) * 32.0f * RotMat(-1.9198622f, 1);
			m_destCamera = GolfBall().m_pos + WVector(0.0f, 2.0f, 0.0f);
			break;
		}


		m_pCameraProc = NULL;

		break;
	case 207:
		m_phase0Time = -1.0f;
		switch (m_replayMode) {
		case 1:
			m_introFrom = WVector(-5.92f, -4.61f, 37.5f) * GOLFDOC()->GetCameraData().m_matrix;
			m_fromRotation = MakeCameraMatrix(m_introFrom, m_introFrom + WVector(0.21f, 0.3f, -0.93f));

			m_introTo = WVector(0.99f, -5.4f, 37.78f) * GOLFDOC()->GetCameraData().m_matrix;
			m_toRotation = MakeCameraMatrix(m_introTo, m_introTo + WVector(-0.3f, 0.41f, -0.86f));
			break;


		case 2:
			m_introFrom = WVector(-10.28f, -0.51f, 17.76f) * GOLFDOC()->GetCameraData().m_matrix;
			m_fromRotation = MakeCameraMatrix(m_introFrom, m_introFrom + WVector(0.93f, 0.09f, 0.35f));

			m_introTo = WVector(-6.45f, 0.62f, 13.05f) * GOLFDOC()->GetCameraData().m_matrix;
			m_toRotation = MakeCameraMatrix(m_introTo, m_introTo + WVector(0.1f, -0.07f, 0.99f));
			break;
		}


		m_transitionTime = 0.5f;

		m_pCameraProc = &CCameraWork::SwingReplayBCamera;

		break;
	case 208:
		m_phase0Time = -1.0f;
		m_destCamera = GetHoleCupPos();
		m_destCamera.y += 3.0f;
		m_swingAngle = 0.0f;
		m_introDistance = 64.0f;
		m_pCameraProc = &CCameraWork::ExtraTimeCamera;
		m_inputMode = 0;
		break;
	case 0:
		m_pCameraProc = &CCameraWork::FollowCamera;
		break;
	default:
		m_pCameraProc = &CCameraWork::FollowCamera;
		break;
	}

	m_prevSourceCamera = m_sourceCamera; m_prevSourceCamera2 = m_sourceCamera;
}

// camerawork.cpp:1136
void CCameraWork::CheckCameraType(bool replay) {
	if (!m_cameraChecking)
		return;

	if (GolfClub().GetType() == 3) {
		if (GolfBall().m_groundType == 2) {
			float angle = CalcDeltaAngle(GetHoleCupPos() - GolfBall().m_pos, GolfBall().m_velocity);
			if ((angle > 0.0f ? angle : -angle) > 0.78539819f)
				SetCamera(100, true);
		} else {
			int index = rand() % m_safeCameras.size();

			SetCamera(m_safeCameras[index], true);
		}
		return;
	}

	if (GOLFDOC()->GetBar().m_accuracyGrade == 1) {
		if (GolfBall().m_preDataCount == 0) {
			SetCamera(0x16, true);
		} else {
			switch (m_fixedCameraIndex % 3) {
			case 0:
				SetCamera(100, true);
				break;
			case 1:
				SetCamera(0x15, true);
				break;
			case 2:
				SetCamera(0x16, true);
				break;
			}
		}
		return;
	}

	int powerFlags = GolfBall().m_powerFlags & 0x70;

	if (powerFlags) {
		const CGolfBall::sExpected *pLast = GolfBall().PreData(GolfBall().m_preDataCount);
		WVector toCup = GetHoleCupPos() - pLast->m_pos;

		int chance = 1;
		if (replay)
			chance = 5;

		if (sqrt(toCup.z * toCup.z + toCup.x * toCup.x) < 16.0f) {
			int r = rand();

			switch (powerFlags) {
			case 0x10:
				if (r % 100 < chance * 5 && GolfBall().m_eventPreDataIndex[10] == GolfBall().m_eventPreDataIndex[0])
					SetCamera(0x1a, true);
				break;
			case 0x20:
				if (r % 100 < chance * 10) {
					if (r % 10 < 5)
						SetCamera(0x18, true);
					else
						SetCamera(0x19, true);
				}
				break;
			case 0x40:
				if (r % 100 < chance * 5 && GolfBall().m_eventPreDataIndex[14] == GolfBall().m_eventPreDataIndex[0])
					SetCamera(0x1b, true);
				break;
			}
		}
	}

	if (m_phase1Time < 0.0f)
		return;

	if (m_cameraType == 10) {
		if (!m_longShotCamera) {
			SetCamera(0, true);
			return;
		}

		IActor *pCaddie = AfxGetTask()->GetActor("Caddie");
		if (pCaddie) {
			sCaddieShotState *pState = 0;
			pCaddie << MsgObject(0, 0x1e0, (intptr_t)&pState, 0, 0, 0, 0);

			if (pState == 0 || !pState->m_keepLongShotCamera)
				SetCamera(0, true);
		}
	}

	for (m_workLength = 0; m_workLength < float2int(m_phase2Time); m_workLength++) {
		const CGolfBall::sExpected *pData = GolfBall().PreData(m_workLength);

		if (pData && pData->m_groundType == 2)
			break;
	}

	if (m_workLength < float2int(m_phase1Time)) {
		m_flags |= 0x20000000;

		if (m_workLength - m_phase0Time < 100.0f) {
			SetCamera(0xe, true);
		} else if (m_cameraType == 4 || m_cameraType == 0xb) {
			SetCamera(0xd, true);
		}
	} else if (m_phase1Time - m_phase0Time < 100.0f || m_duration - m_phase0Time < 50.0f) {
		SetCamera(0xd, true);
	} else if (m_phase1Time - m_phase0Time < 150.0f) {
		switch (m_fixedCameraIndex % 2) {
		case 0:
			SetCamera(4, true);
			break;
		case 1:
			SetCamera(0xd, true);
			break;
		}
	}
}

// camerawork.cpp:1293
// Lays out the four Bezier control points of the camera works that follow
// the ball's predicted flight (types 0, 1, 2, 4, 8, 11, 12, 18), then sets
// m_riseHeight from the distance between the last point and the ball's end.
void CCameraWork::InitCtrlPoints(int type) {
	CGolfBall &ball = GolfBall();

	int midIndex = ball.m_eventPreDataIndex[1] > 0 ? ball.m_eventPreDataIndex[1] : 0;
	int landIndex = ball.m_eventPreDataIndex[0] > 0 ? ball.m_eventPreDataIndex[0] : 0;
	int spinIndex = ball.m_eventPreDataIndex[7] > 0 ? ball.m_eventPreDataIndex[7] : 0;
	int startIndex = Between(0, float2int(m_phase0Time), ball.m_preDataCount);

	if (midIndex == 0)
		midIndex = ball.m_preDataCount / 2;

	if (landIndex < midIndex) {
		midIndex = ball.m_preDataCount / 3;
		spinIndex = ball.m_preDataCount * 2 / 3;
	}

	m_riseHeight = 5.0f;

	WVector dir;
	if (GolfBall().IsHoleIn() == 1)
		dir = ball.PreData(ball.m_preDataCount)->m_pos - ball.PreData(0)->m_pos;
	else
		dir = GetHoleCupPos() - ball.PreData(ball.m_preDataCount)->m_pos;

	dir.y = 0.0f;
	dir.Normalize();

	switch (type) {
	case 0: {
		m_bezierPoints[0] = m_baseMatrix.pivot;

		WVector half;
		half = ball.PreData(0)->m_velocity * 0.5f;
		const CGolfBall::sExpected *pMid = ball.PreData(midIndex);
		if (pMid)
			m_bezierPoints[1] = half + pMid->m_pos;
		else
			m_bezierPoints[1] = WVector(half.x, half.y + 1.0f, half.z);

		WVector side;
		side = WCrossProduct(WVector(0.0f, m_fixedCameraIndex % 2 ? 1.0f : -1.0f, 0.0f), half).Normalize() * 30.0f;
		const CGolfBall::sExpected *pSpin = ball.PreData(spinIndex);
		if (pSpin)
			m_bezierPoints[2] = pSpin->m_pos + WVector(0.0f, 25.0f, 0.0f) + side;
		else
			m_bezierPoints[2] = m_bezierPoints[1] + WVector(0.0f, 25.0f, 0.0f) + side;

		WVector back;
		back = ball.PreData(0)->m_pos - ball.PreData(ball.m_preDataCount)->m_pos;
		WVector offset;
		if (dir * WVector(back.x, 0.0f, back.z) > 0.0f)
			offset = back.Normalize() * 15.0f;
		else
			offset = back.Normalize() * -15.0f;
		offset.y += 5.0f;

		const CGolfBall::sExpected *pLand = ball.PreData(landIndex);
		if (pLand)
			m_bezierPoints[3] = pLand->m_pos + offset * 5.0f;
		else
			m_bezierPoints[3] = m_bezierPoints[2] + offset * 5.0f;
		break;
	}
	case 1: {
		WVector vel = ball.PreData(startIndex)->m_velocity;
		m_bezierPoints[0] = vel * 0.8f + ball.PreData(startIndex)->m_pos;
		vel = ball.PreData(midIndex)->m_velocity;
		m_bezierPoints[1] = ball.PreData(midIndex)->m_pos + WVector(0.0f, 50.0f, 0.0f) + vel * 2.0f;
		vel = ball.PreData(spinIndex)->m_velocity;
		m_bezierPoints[2] = ball.PreData(spinIndex)->m_pos + WVector(0.0f, 10.0f, 0.0f) + vel;
		m_bezierPoints[3] = ball.PreData(ball.m_preDataCount)->m_pos + WVector(0.0f, 10.0f, 0.0f);
		break;
	}
	case 2: {
		WVector vel = ball.PreData(0)->m_velocity;
		WVector side = WCrossProduct(WVector(0.0f, m_fixedCameraIndex % 2 ? 1.0f : -1.0f, 0.0f), vel).Normalize() * 25.0f;
		m_bezierPoints[0] = side + ball.PreData(startIndex)->m_pos;
		vel = ball.PreData(midIndex)->m_velocity;
		m_bezierPoints[1] = ball.PreData(midIndex)->m_pos + WVector(0.0f, 50.0f, 0.0f) + vel * 1.2f + side;
		vel = ball.PreData(spinIndex)->m_velocity;
		m_bezierPoints[2] = vel * 1.5f + ball.PreData(spinIndex)->m_pos + side;
		if (ball.m_landed)
			m_bezierPoints[3] = ball.PreData(ball.m_preDataCount)->m_pos + WVector(0.0f, 30.0f, 0.0f) + vel * 5.0f;
		else
			m_bezierPoints[3] = ball.PreData(ball.m_preDataCount)->m_pos + WVector(0.0f, 10.0f, 0.0f) + side;
		break;
	}
	case 4:
	case 11:
		m_bezierPoints[0] = ball.PreData(midIndex)->m_pos;
		m_bezierPoints[1] = ball.PreData(midIndex - float2int((ball.m_preDataCount - midIndex) * -0.5f))->m_pos;
		m_bezierPoints[2] = ball.PreData(midIndex - float2int((ball.m_preDataCount - midIndex) * -0.75f))->m_pos;
		m_bezierPoints[3] = ball.PreData(float2int(m_phase2Time))->m_pos + WVector(0.0f, 3.0f, 0.0f);
		break;
	case 8: {
		m_bezierPoints[0] = m_baseMatrix.pivot;
		WVector vel = ball.PreData(0)->m_velocity;
		WVector side = WCrossProduct(WVector(0.0f, m_fixedCameraIndex % 2 ? 1.0f : -1.0f, 0.0f), vel).Normalize() * 30.0f;
		m_bezierPoints[1] = ball.PreData(midIndex)->m_pos + WVector(0.0f, 25.0f, 0.0f) + vel * 2.5f + side;
		vel = ball.PreData(spinIndex)->m_velocity;
		m_bezierPoints[2] = vel * 1.5f + ball.PreData(spinIndex)->m_pos + side;
		if (ball.m_landed) {
			m_bezierPoints[3] = ball.PreData(ball.m_preDataCount)->m_pos + WVector(0.0f, 10.0f, 0.0f) + dir * 40.0f;
		} else {
			WVector back;
			back = ball.PreData(0)->m_pos - ball.PreData(ball.m_preDataCount)->m_pos;
			back.y = 0.0f;
			if (back * dir > 0.0f)
				m_bezierPoints[3] = ball.PreData(ball.m_preDataCount)->m_pos + WVector(0.0f, 10.0f, 0.0f) - dir * 40.0f;
			else
				m_bezierPoints[3] = ball.PreData(ball.m_preDataCount)->m_pos + WVector(0.0f, 10.0f, 0.0f) + dir * 40.0f;
		}
		break;
	}
	case 12: {
		m_bezierPoints[0] = m_sourceCamera;
		m_bezierPoints[1] = WVector((m_fixedCameraIndex % 2 ? 1.0f : -1.0f) * 10.0f, 100.0f, (m_fixedCameraIndex % 2 ? 1.0f : -1.0f) * 10.0f) + ball.PreData(midIndex)->m_pos;
		WVector side = WCrossProduct(WVector(0.0f, m_fixedCameraIndex % 2 ? 1.0f : -1.0f, 0.0f), dir).Normalize() * 15.0f;
		m_bezierPoints[2] = WVector((m_fixedCameraIndex % 2 ? 1.0f : -1.0f) * 10.0f, 50.0f, (m_fixedCameraIndex % 2 ? 1.0f : -1.0f) * 10.0f) + ball.PreData(spinIndex)->m_pos + side;
		dir = dir * -15.0f;
		dir = dir + side * 0.3f;
		dir.y += 3.0f;
		if (ball.m_landed)
			m_bezierPoints[3] = dir * 5.0f + ball.PreData(ball.m_preDataCount)->m_pos;
		else
			m_bezierPoints[3] = dir * 3.0f + ball.PreData(ball.m_preDataCount)->m_pos;
		break;
	}
	case 18: {
		m_bezierPoints[0] = m_sourceCamera;
		WVector side = WCrossProduct(WVector::UNIT_POS_Y, ball.PreData(0)->m_velocity).Normalize() * 100.0f;
		m_bezierPoints[1] = ball.PreData(midIndex)->m_pos + WVector(0.0f, 100.0f, 0.0f) - side;
		m_bezierPoints[2] = ball.PreData(spinIndex)->m_pos + WVector(0.0f, 50.0f, 0.0f) + side;
		dir = side;
		dir = dir.Normalize() * 30.0f;
		dir.y += 15.0f;
		m_bezierPoints[3] = dir + ball.PreData(ball.m_preDataCount)->m_pos;
		break;
	}
	}

	WVector rest = ball.PreData(ball.m_preDataCount)->m_pos - m_bezierPoints[3];
	m_riseHeight = rest.Magnitude() * 0.1f; m_riseHeight = Between(1.0f, m_riseHeight, 5.0f);
}

// camerawork.cpp:1475
void CCameraWork::InitVars(void) {
	m_duration = (float)GolfBall().m_eventPreDataIndex[1];
	m_phase1Time = (float)GolfBall().m_eventPreDataIndex[0];
	m_phase2Time = (float)GolfBall().m_preDataCount;
	m_processedFrame = -2;
	m_preDataFrame = 0;
	m_cameraTime = 0.0f;
	m_panDone = false;
	m_ballSoundState = 0;
	m_lastQuakeTime = 0.0f;
	m_ballSoundTime = 0.0f;
	m_transitionTime = -1.0f;
	m_firstCamera = true;
	m_riseHeight = 5.0f;
	m_inputMode = 2;

	if (m_fixedCameraIndex % 6 == 0 && GetPVS().GetObjColNum() == 0 && GolfClub().GetType() != 3 &&
		GetPVS().GetGroundType(GolfBall().PreData(GolfBall().m_preDataCount)->m_pos, true, 0) == 2)
		m_landsInWater = true;
	else
		m_landsInWater = false;

	m_stateFlag = false;

	m_holeInReached = false;
	m_flags = 1;
	m_ballStartPos = GolfBall().m_pos;

	g_audio->StopSfx("\xb0\xf8_\xb9\xd9\xb6\xf7");
}

// camerawork.cpp:1506
void CCameraWork::InputProcess(float elapsed) {
	if (m_inputMode == 0) {
		g_camera = GetCameraMatrix(m_sourceCamera, m_destCamera, m_bankAngle);
		return;
	}

	// The arrow keys, by their Korean names (up, down, left, right).
	if (g_input->GetDown("\xbb\xf3", true) || g_input->GetDown("\xc7\xcf", true) || g_input->GetDown("\xc1\xc2", true) || g_input->GetDown("\xbf\xec", true))
		m_input.m_used = true;

	if (g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true)) {
		if (g_input->Get("\xbb\xf3", true))
			m_input.m_yaw -= elapsed * 0.52359879f;

		if (g_input->Get("\xc7\xcf", true))
			m_input.m_yaw += elapsed * 0.52359879f;
	}

	if (!g_input->Get("\xbb\xf3", true) && !g_input->Get("\xc7\xcf", true) && !(WAbsBySign(m_input.m_yaw) < data_9eb420[4]) && m_inputMode == 2) {
		if (m_input.m_yaw > 0.0f) {
			m_input.m_yaw -= elapsed * 1.0471976f;
			if (m_input.m_yaw < 0.0f)
				m_input.m_yaw = 0.0f;
		} else {
			m_input.m_yaw += elapsed * 1.0471976f;
			if (m_input.m_yaw > 0.0f)
				m_input.m_yaw = 0.0f;
		}
	}

	float speed;

	if (m_inputMode == 2)
		speed = 0.52359879f;
	else
		speed = 0.2617994f;

	if (!g_input->Get("LSHIFT", true) && !g_input->Get("RSHIFT", true)) {
		if (g_input->Get("\xbb\xf3", true)) {
			if (m_inputMode == 1 || !m_freePitch)
				m_input.m_pitch += speed * elapsed;
		}

		if (g_input->Get("\xc7\xcf", true))
			m_input.m_pitch -= speed * elapsed;
	}

	if (!g_input->Get("\xbb\xf3", true) && !g_input->Get("\xc7\xcf", true) && !(WAbsBySign(m_input.m_pitch) < data_9eb420[4]) && m_inputMode == 2) {
		if (m_input.m_pitch > 0.0f) {
			m_input.m_pitch -= speed * elapsed * 2.0f;
			if (m_input.m_pitch < 0.0f)
				m_input.m_pitch = 0.0f;
		} else {
			m_input.m_pitch += speed * elapsed * 2.0f;
			if (m_input.m_pitch > 0.0f)
				m_input.m_pitch = 0.0f;
		}
	}

	if (m_inputMode == 2)
		speed = 1.5707964f;
	else
		speed = 0.52359879f;

	if (g_input->Get("\xc1\xc2", true))
		m_input.m_zoom -= speed * elapsed;

	if (g_input->Get("\xbf\xec", true))
		m_input.m_zoom += speed * elapsed;

	if (!g_input->Get("\xc1\xc2", true) && !g_input->Get("\xbf\xec", true) && !(WAbsBySign(m_input.m_zoom) < data_9eb420[4]) && m_inputMode == 2) {
		if (m_input.m_zoom > 0.0f) {
			m_input.m_zoom -= speed * elapsed * 2.0f;
			if (m_input.m_zoom < 0.0f)
				m_input.m_zoom = 0.0f;
		} else {
			m_input.m_zoom += speed * elapsed * 2.0f;
			if (m_input.m_zoom > 0.0f)
				m_input.m_zoom = 0.0f;
		}
	}

	if (m_inputMode != 2)
		m_input.m_zoom = Between(-1.0471976f, m_input.m_zoom, 1.0471976f);
	m_input.m_pitch = Between(-0.52359879f, m_input.m_pitch, 0.52359879f);

	if (!m_input.m_used) {
		m_input.m_yaw = 0.0f;
		m_input.m_pitch = 0.0f; m_input.m_zoom = 0.0f;
	}

	if (m_keepInputOffsets) {
		float fov = m_input.m_yaw + 0.95993108f;
		g_view->SetFOV(fov < 0.17453292f ? 0.17453292f : (fov > data_9eb420[1] ? 1.5707964f : fov));

		if (WAbsBySign(m_input.m_zoom) < data_9eb420[4] && WAbsBySign(m_input.m_pitch) < data_9eb420[4]) {
			g_camera = m_baseMatrixCopy;
			return;
		}

		g_camera = GetCameraMatrix(m_baseMatrixCopy.pivot, WVector(sinf(m_input.m_zoom) * cosf(m_input.m_pitch), sinf(m_input.m_pitch), cosf(m_input.m_zoom) * cosf(m_input.m_pitch)) * m_baseMatrix, m_bankAngle);
		return;
	}

	float fov = m_zoomAngle + m_input.m_yaw;
	g_view->SetFOV(fov < 0.17453292f ? 0.17453292f : (fov > data_9eb420[1] ? 1.5707964f : fov));

	m_baseMatrixCopy = GetCameraMatrix(m_sourceCamera, m_destCamera, m_bankAngle);

	if (WAbsBySign(m_input.m_zoom) < data_9eb420[4] && WAbsBySign(m_input.m_pitch) < data_9eb420[4]) {
		g_camera = m_baseMatrixCopy;
	} else if (m_inputMode == 2) {
		WVector dir = m_baseMatrixCopy.pivot - *m_pTarget;
		float distance = MaxOf(10.0f, (1.0f - Abs(m_input.m_zoom) * 0.47746483f) * dir.Magnitude());

		dir.Normalize();
		dir *= distance;

		WVector source = dir * RotMat(m_baseMatrixCopy.xa, m_input.m_pitch) * RotMat(m_input.m_zoom, 1) + *m_pTarget;
		WVector dest = m_baseMatrixCopy.za * RotMat(m_baseMatrixCopy.xa, m_input.m_pitch) * RotMat(m_input.m_zoom, 1) + source;

		g_camera = GetCameraMatrix(source, dest, m_bankAngle);
	} else {
		g_camera = GetCameraMatrix(m_baseMatrixCopy.pivot, WVector(sinf(m_input.m_zoom) * cosf(m_input.m_pitch), sinf(m_input.m_pitch), cosf(m_input.m_zoom) * cosf(m_input.m_pitch)) * m_baseMatrixCopy, m_bankAngle);
	}

	if (GolfBall().m_preDataIndex > 0)
		ProcessBallSound();
}

// camerawork.cpp:1679
void CCameraWork::Process(float elapsed, float time) {
	m_prevSourceCamera2 = m_prevSourceCamera;
	m_prevSourceCamera = m_sourceCamera;

	CGolfBall &ball = GolfBall();

	m_preDataFrame = ball.m_preDataIndex;
	m_workTime = (float)m_preDataFrame;
	float prevTime = m_cameraTime;
	bool scripted = false;
	WVector up(0.0f, 1.0f, 0.0f);
	m_cameraTime += elapsed;

	if (m_destFollowsGround) {
		static WMatrix s_toCamera(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f);
		static bool s_targetSent = false;

		if (m_pScriptMotion == 0)
			return;

		if (m_cameraTime > m_pScriptMotion->e) {
			ScriptProcess(m_pScriptMotion, prevTime, m_pScriptMotion->e);

			float over = m_cameraTime - m_pScriptMotion->e;
			m_pScriptMotion = m_pScriptPuppet->FindMotionData(m_pScriptMotion->nextmotion);

			if (m_pScriptMotion) {
				if (WAbsBySign(m_pScriptMotion->e - m_pScriptMotion->s) < data_9eb420[4]) {
					m_cameraTime = m_pScriptMotion->s;
					m_transitionTime = m_cameraTime - 1.0f;
				} else {
					m_cameraTime = over + m_pScriptMotion->s;
				}
				prevTime = m_pScriptMotion->s;
				ScriptProcess(m_pScriptMotion, prevTime, m_cameraTime);
			}
		} else {
			ScriptProcess(m_pScriptMotion, prevTime, m_cameraTime);
		}

		if (m_pScriptMotion) {
			m_pScriptPuppet->ApplyBones(m_pScriptPuppet->GetBones(0, m_cameraTime, 0, 0, false));
			m_pScriptPuppet->Transform(0, 1.0f, 0);

			WBone *pBone = m_pScriptPuppet->FindBone(m_pScriptMotion->applybone);
			float yaw;
			WVector offset;
			WVector dir(0.0f, 0.0f, 1.0f);
			if (pBone == 0)
				pBone = m_pScriptPuppet->GetRootBone();

			if (pBone) {
				AfxGetTask()->GetActor("Player") << MsgObject(0, 0x110, (intptr_t)&yaw, 0, 0, 0, 0);
				AfxGetTask()->GetActor("Player") << MsgObject(0, 0x128, (intptr_t)&offset, 0, 0, 0, 0);

				WMatrix mat;
				mat = pBone->GetMatrix();

				if (!(WAbsBySign(offset.x) < data_9eb420[4]) || !(WAbsBySign(offset.y) < data_9eb420[4]) || !(WAbsBySign(offset.z) < data_9eb420[4])) {
					if (!m_skipTurn)
						mat = mat * RotMat(yaw, 1);
				}

				WMatrix rot;
				mat.GetRotMatrix(&rot);
				rot = s_toCamera * rot;

				WVector pos = mat.pivot;
				if (m_cameraType == 0xcb) {
					WMatrix turn(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
					turn.Rotate(data_9eb420[0] + yaw, 1);

					rot = rot * turn;
					pos = pos * turn;

					if (s_targetSent) {
						AfxGetTask()->GetActor("Player") << MsgObject(0, 0x127, (intptr_t)&m_scriptTarget, 1, 0, 0, 0);
					}
					if (!s_targetSent) {
						s_targetSent = true;
					}
				} else if (strcmpi(m_pScriptPuppet->GetPuppetName(), "i_def_cam.apet") == 0) {
					if (strstr(m_pScriptMotion->name, "\xbf\xec\xb5\xe5\xc6\xc4\xbf\xf6\xbc\xa6" "2")) {
						static WMatrix s_turnAround(-1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f);
						rot = rot * s_turnAround;
						pos = pos * s_turnAround;
					}
				}

				dir = dir * rot;

				pos = pos + m_scriptTarget;

				scripted = true;
				m_sourceCamera = offset = pos - offset;
				m_destCamera = dir = dir + m_sourceCamera;
				up = rot.ya;
			}
		}

		if (m_cameraType == 0xcb && m_transitionTime > 0.0f && m_cameraTime > m_transitionTime) {
			s_targetSent = false;
			m_destFollowsGround = false;
			SetCamera(0xcc, false);
		}
	} else {
		m_pScriptMotion = 0;
		if (m_cameraType < 0xca && CheckState()) {
			m_ballStartPos = GolfBall().m_pos;
			InputProcess(elapsed);
			return;
		}

		if (m_nextCameraType >= 0) {
			SetCamera(m_nextCameraType, false);
			m_nextCameraType = -1;
			InitCtrlPoints(m_cameraType);
		}

		if (m_workTime < m_phase0Time) {
			m_processedFrame = m_preDataFrame;
		} else {
			if (m_cameraType == 3 || m_cameraType == 4 || m_cameraType == 11) {
				if (m_processedFrame < (int)m_phase0Time)
					m_sourceCamera = m_phase0SourceCamera;
			}

			if (m_pCameraProc) {
				(this->*m_pCameraProc)(elapsed, time);
				CheckDriveCameraBallVisibilityAndSetBallVisibilitySafeCamera();
			}
		}

		if (m_flags & 0x80000000) {
			if (m_cameraTime - m_quakeStartTime < 0.8f)
				EarthQuake();
			else
				m_flags &= 0x7fffffff;
		}
	}

	if (m_firstCamera)
		CheckGround(m_sourceCamera, m_destCamera);

	if ((m_landsInWater || WVectorLen(m_sourceCamera - GolfBall().m_pos) > 192.0f) && m_preDataFrame > 0 && GolfBall().PreData(m_preDataFrame)->m_contactType == 3 && m_cameraType != 0x19) {
		m_zoomAngle = MaxOf(0.34906584f, m_zoomAngle - elapsed * 0.17453292f);
		m_destCamera.y -= (1.0f - m_zoomAngle * (1.0f / 0.95993108f)) * 5.0f;
	}

	m_ballStartPos = GolfBall().m_pos;

	InputProcess(elapsed);

	if (scripted) {
		g_camera.ya = up;
		g_camera.xa = WCrossProduct(g_camera.ya, g_camera.za);
	}
}

// camerawork.cpp:1917
bool CCameraWork::CheckGround(WVector &pos, WVector &dest) {
	float y = Max(pos.y, GetPVS().GetBasePosY(pos, GOLFDOC()->GetCameraData().m_groundOffset));

	if (m_destFollowsGround)
		dest.y += y - pos.y;

	if (y > pos.y) {
		pos.y = y;
		return true;
	}

	return false;
}

// camerawork.cpp:1934
void CCameraWork::ScriptProcess(w_motion_data *motion, float from, float to) {
	if (m_pScriptPuppet == 0)
		return;

	if (!(from <= motion->e) || from == to)
		return;

	from *= 30.0f;
	to = MinOf(motion->e, to) * 30.0f;
	int first = Ceil(from, 0.03f);
	int last = to == (float)floor(to) ? float2int(to) - 1 : float2int((float)floor(to));

	if (first > last)
		return;

	int start, end;

	if (m_pScriptPuppet->CheckFrameData(first, last, start, end)) {
		for (int i = start; i <= end; i++)
			ParsePuppetScript(m_pScriptPuppet->GetFrameData(i));
	}
}

// camerawork.cpp:1960
void CCameraWork::ParsePuppetScript(char *script) {
	if (script) {
		char *p = strchr(script, '*');

		while (p) {
			int len = 0;

			while (p[len]) {
				if (p[++len] == ' ')
					break;
				if (p[len] == '(') {
					len++;
					break;
				}
			}

			if (len <= 1)
				break;

			strncpy(data_af1f60, p, len);
			data_af1f60[len] = 0;
			p += len;
			strcmpi(data_af1f60, data_a7ee68);
			p = strchr(p, '*');
		}
	}
}

// camerawork.cpp:2019
bool CCameraWork::CheckState(void) {
	if (m_workTime >= m_phase2Time)
		return true;

	if (GolfBall().m_holeIn == 1) {
		WVector dist = GOLFDOC()->GetHoleData().m_cupPos; dist = GolfBall().m_pos - dist;

		if (!m_holeInReached) {
			if (sqrt(dist.x * dist.x + dist.z * dist.z) < GolfBall().GetRadius() * 3.0f && dist.y < 0.0f)
				m_holeInReached = true;
		}

		if (m_holeInReached)
			return true;
	}

	return false;
}

// camerawork.cpp:2042
void CCameraWork::ProcessBallSound(void) {
	if (m_cameraTime - m_ballSoundTime < 0.1f && m_ballSoundState != 1)
		return;

	m_ballSoundTime = m_cameraTime;

	WVector offset;
	offset = GolfBall().m_pos - m_sourceCamera;
	float distance = offset.Magnitude();

	if (m_ballSoundState == 0) {
		if (GolfBall().m_velocity.Magnitude() > 80.0f && distance < 100.0f) {
			m_ballSoundState = 1;
			g_audio->PlaySfx("\xb0\xf8_\xb9\xd9\xb6\xf7", &offset, 5, "\xb0\xf8_\xb9\xd9\xb6\xf7", 0, 0, 100.0f);
		}
	} else if (m_ballSoundState & 1) {
		if (GolfBall().m_velocity.Magnitude() > 80.0f) {
			if (distance < 100.0f) {
				if (!g_audio->IsPlaying("\xb0\xf8_\xb9\xd9\xb6\xf7"))
					g_audio->PlaySfx("\xb0\xf8_\xb9\xd9\xb6\xf7", &offset, 5, "\xb0\xf8_\xb9\xd9\xb6\xf7", 0, 0, 100.0f);

				g_audio->SetSFXVolumeWhilePlaying("\xb0\xf8_\xb9\xd9\xb6\xf7", (100.0f - distance) * 0.01f);
			} else if (m_ballSoundState == 1) {
				g_audio->StopSfx("\xb0\xf8_\xb9\xd9\xb6\xf7");
				m_ballSoundState = 3;
			}
		} else {
			g_audio->StopSfx("\xb0\xf8_\xb9\xd9\xb6\xf7");
			m_ballSoundState = 2;
		}
	}
}

// camerawork.cpp:2098
void CCameraWork::StopBallSound(void) {
	m_ballSoundState = 2;
	g_audio->StopSfx("\xb0\xf8_\xb9\xd9\xb6\xf7");
}

// camerawork.cpp:2104
void CCameraWork::InterpolateBezier(WVector *out, float t, WVector *points, int count) {
	float t2 = t * t;
	float s = 1.0f - t;
	float s2 = s * s;

	if (count == 3) {
		*out = s2 * points[0] + s * t * 2.0f * points[1] + t2 * points[2];
	} else if (count == 4) {
		float t3 = t2 * t;
		float s3 = s2 * s;
		*out = s3 * points[0] + s2 * t * 3.0f * points[1] + s * t2 * 3.0f * points[2] + t3 * points[3];
	}
}

// camerawork.cpp:2129
void CCameraWork::CheckDriveCameraBallVisibilityAndSetBallVisibilitySafeCamera(void) {
	if (m_cameraType >= 100)
		return;

	if (!driveTable[m_cameraType].m_checkBallVisibility)
		return;

	Waabb& aabb = GOLFDOC()->m_pPolySoup->FindModel(0)->m_aabb;
	WVector size = aabb.max - aabb.min;

	WVector toBall = GolfBall().m_pos - m_sourceCamera;

	if (!(toBall.Magnitude() > size.Magnitude() * 2.0f) && GetPVS().RayIntersectAll(m_sourceCamera, toBall, true) == 1.0f) {
		m_ballHiddenFrame = -1;
	} else if (m_ballHiddenFrame < 0) {
		m_ballHiddenFrame = m_preDataFrame;
	} else if (m_preDataFrame - m_ballHiddenFrame > 50) {
		m_installed = false;
		unsigned int i = rand() % m_safeCameras.size();

		if (m_safeCameras[i] == m_cameraType) {
			i = (i + 1) % m_safeCameras.size();

			if (m_safeCameras[i] == m_cameraType)
				return;
		}

		SetCamera(m_safeCameras[i], false);
	}
}

// camerawork.cpp:2178
void CCameraWork::EarthQuake(void) {
	if (m_cameraTime - m_lastQuakeTime < 0.1f)
		return;

	m_lastQuakeTime = m_cameraTime;

	int strength = 6 - (int)((m_cameraTime - m_quakeStartTime) * 6.0f);
	if (strength == 0)
		strength = 1;
	float half = strength * 0.05f;

	WVector shake;
	shake.x = (rand() % strength) * 0.1f - half;
	shake.y = (rand() % strength) * 0.1f - half;
	shake.z = (rand() % strength) * 0.1f - half;
	m_sourceCamera += shake * 0.1f;
}

// camerawork.cpp:2202
void CCameraWork::FollowCamera(float elapsed, float time) {
	float t = (time + m_workTime - m_phase0Time) / (m_phase2Time - m_phase0Time);
	InterpolateBezier(&m_sourceCamera, t, m_bezierPoints, 4);

	if (m_workTime < m_phase1Time) {
		m_destCamera = GolfBall().m_pos;
	} else if (m_workTime < m_phase2Time) {
		float s = (time + m_workTime - m_phase1Time) / (m_phase2Time - m_phase1Time);
		m_destCamera = GolfBall().m_pos + WVector(0.0f, s * m_riseHeight, 0.0f);
	}
}

// camerawork.cpp:2222
void CCameraWork::LookBackCamera(float elapsed, float time) {
	float t = (time + m_workTime - m_phase0Time) / (m_phase2Time - m_phase0Time);
	InterpolateBezier(&m_sourceCamera, t, m_bezierPoints, 4);

	if (m_flags & 0x20000000) {
		if (m_workTime < (float)m_workLength - 40.0f)
			m_destCamera = GolfBall().m_pos;
		else
			m_nextCameraType = 14;
	} else {
		if (m_workTime < m_phase1Time * 0.65f &&
			(m_sourceCamera - m_destCamera).Magnitude() > 10.0f)
			m_destCamera = GolfBall().m_pos;
		else
			m_nextCameraType = 2;
	}
}

// camerawork.cpp:2246
void CCameraWork::Rail_or_SideCamera(float elapsed, float time) {
	float t = (time + m_workTime - m_phase0Time) / (m_phase2Time - m_phase0Time);
	InterpolateBezier(&m_sourceCamera, t, m_bezierPoints, 4);
	m_zoomAngle = 0.95993108f - data_9eb420[2] * (sinf(t * data_9eb420[0]) * 25.0f);

	if (m_workTime < m_phase1Time)
		m_destCamera = GolfBall().m_pos;
	else if (m_workTime < m_phase2Time) {
		float s = (time + m_workTime - m_phase1Time) / (m_phase2Time - m_phase1Time);
		m_destCamera = GolfBall().m_pos + WVector(0.0f, s * m_riseHeight, 0.0f);
	}
}

// camerawork.cpp:2265
void CCameraWork::PanCamera(float elapsed, float time) {
	bool panning;

	if (m_flags & 0x20000000)
		panning = !(m_workTime > (float)m_workLength - 40.0f);
	else
		panning = !(m_workTime > m_phase1Time * 0.7f);

	if (panning) {
		int offset;

		if (m_flags & 1) {
			m_destCamera = GolfBall().m_pos;
			offset = 0;
		} else {
			offset = -10;
			m_destCamera = GolfBall().PreData(m_preDataFrame + offset)->m_pos * (1.0f - time) + GolfBall().PreData(m_preDataFrame + offset + 1)->m_pos * time;
		}

		float t = (time + m_workTime - m_phase0Time) / (m_phase1Time * 0.7f - m_phase0Time);
		m_zoomAngle = 0.95993108f - data_9eb420[2] * (sinf(t * data_9eb420[1]) * 25.0f);

		if (GolfClub().GetType() != 0)
			return;

		if (!m_panDone && m_duration - m_workTime < 15.0f - offset) {
			WVector pos, dir;
			pos = GolfBall().PreData(float2int(m_workTime))->m_pos;
			dir = GolfBall().PreData(float2int(m_workTime) + 1)->m_pos - pos;
			dir.y = 0.0f;
			pos = m_sourceCamera - pos;
			pos.y = 0.0f;

			if (WCrossProduct(dir, pos).y > 0.0f)
				g_audio->PlaySfx("pass_LR", 0, 0, 0, 0, 0.5f, 200.0f);
			else
				g_audio->PlaySfx("pass_RL", 0, 0, 0, 0, 0.5f, 200.0f);

			m_panDone = true;
		}
	} else {
		m_zoomAngle = 0.95993108f; m_nextCameraType = (m_flags & 0x20000000) ? 14 : 0;
	}
}

// camerawork.cpp:2335
void CCameraWork::DropCamera(float elapsed, float time) {
	if (m_workTime < m_phase1Time)
		m_destCamera = GolfBall().m_pos;
	else if (m_workTime < m_phase2Time) {
		float t = time + m_workTime - m_phase1Time;
		InterpolateBezier(&m_destCamera, t / (m_phase2Time - m_phase1Time), m_bezierPoints, 4);
	}
}

// camerawork.cpp:2349
// Chases the ball: looks back along its recorded path for the last turn of
// more than 45 degrees and places the camera behind that point.
void CCameraWork::Chase_and_FollowCamera(float elapsed, float time) {
	m_destCamera = m_destCamera * 0.9f + GolfBall().m_pos * 0.1f;

	float speed = GolfBall().m_velocity.Magnitude();
	WVector pos, dir;
	bool found = false;
	int i;
	for (i = m_preDataFrame; i >= Max(2, m_preDataFrame - 37); i--) {
		WVector prevDir = GolfBall().PreData(i - 1)->m_pos - GolfBall().PreData(i - 2)->m_pos;
		WVector curDir = GolfBall().PreData(i)->m_pos - GolfBall().PreData(i - 1)->m_pos;
		prevDir.y = 0.0f;
		curDir.y = 0.0f;
		prevDir.Normalize();
		curDir.Normalize();

		float dot = curDir * prevDir;
		if (dot < cos(45.0f * g_DEGTORAD)) {
			float cos80 = cos(80.0f * g_DEGTORAD);
			pos = GolfBall().PreData(i - 1)->m_pos;
			pos.y = GolfBall().m_pos.y;
			dir = curDir - prevDir;

			if (dot < cos80) {
				WMatrix right = RotMat(-5.0f * g_DEGTORAD, 1);
				WMatrix left = RotMat(5.0f * g_DEGTORAD, 1);
				float side = prevDir.z * curDir.x - curDir.z * prevDir.x;
				dir = RotVec(dir, side > 0.0f ? right : left);
			}
			found = true;
			break;
		}
	}
	if (!found) {
		pos = GolfBall().m_pos;
		dir = GolfBall().PreData(m_preDataFrame)->m_pos - GolfBall().PreData(m_preDataFrame - 1)->m_pos;
	}

	dir.y = 0.0f;
	dir.Normalize();
	dir.y = 0.5f;
	dir.Normalize();

	float distance = speed * 0.4f;
	if (distance < 40.0f)
		distance = 40.0f;
	else if (distance > 80.0f)
		distance = 80.0f;

	WVector target = pos + dir * distance;
	WVector toSource = m_sourceCamera - m_destCamera;
	WVector toTarget = target - m_destCamera;
	if (toTarget.Magnitude() > toSource.Magnitude()) {
		float length = toSource.Magnitude();
		WVector flat = toSource;
		flat.y = 0.0f;
		flat.Normalize();
		flat.y = 0.5f;
		flat.Normalize();
		target = flat * length + m_destCamera;
	}

	float t = speed * 0.001f;
	if (t < 0.00001f)
		t = 0.00001f;
	else if (t > 0.1f)
		t = 0.1f;

	m_sourceCamera = m_sourceCamera * (1.0f - t) + target * t;
	InterpolateBezier(&m_sourceCamera, 0.5f, &m_sourceCamera, 3);
}

// camerawork.cpp:2433
void CCameraWork::CircleCamera(float elapsed, float time) {
	if (m_preDataFrame < GolfBall().m_eventPreDataIndex[13]) {
		if (m_bankAngle > 0.0f) {
			m_bankAngle -= elapsed * 1.0471976f;
			if (m_bankAngle < 0.0f)
				m_bankAngle = 0.0f;
		}

		WVector dir = GolfBall().m_pos - m_sourceCamera;

		dir.y = 0.0f;
		m_introDistance = sqrt(dir.z * dir.z + dir.x * dir.x); dir.Normalize(); dir = WCrossProduct(dir, WVector::UNIT_POS_Y); m_sourceCamera += dir * (m_introDistance * data_9eb420[0]) * elapsed;
	} else {
		m_sourceCamera += GolfBall().m_velocity * 0.75f * elapsed;
		m_destCamera += GolfBall().m_velocity * 0.75f * elapsed;
		m_destCamera.y -= elapsed;
	}
}

// camerawork.cpp:2463
void CCameraWork::Chase_and_LookBackCamera(float elapsed, float time) {
	float t = (time + m_workTime - m_phase0Time) / (m_phase2Time - m_phase0Time);
	InterpolateBezier(&m_sourceCamera, t, m_bezierPoints, 4);
	m_zoomAngle = 0.95993108f - data_9eb420[2] * (sinf(t * data_9eb420[0]) * 25.0f);

	if (m_flags & 0x20000000) {
		if (m_workTime < (float)m_workLength - 50.0f)
			m_destCamera = GolfBall().m_pos;
		else
			m_nextCameraType = 14;
	} else {
		if (m_workTime < m_phase1Time)
			m_destCamera = GolfBall().m_pos;
		else if (m_workTime < m_phase2Time) {
			float s = (time + m_workTime - m_phase1Time) / (m_phase2Time - m_phase1Time);
			m_destCamera = GolfBall().m_pos + WVector(0.0f, s * m_riseHeight, 0.0f);
		}
	}
}

// camerawork.cpp:2492
void CCameraWork::ZoomInCamera(float elapsed, float time) {
	if (m_workTime < m_duration) {
		float t = (time + m_workTime) / m_duration;
		m_zoomAngle = 0.95993108f - data_9eb420[2] * (t * 35.0f);

		m_destCamera = GolfBall().m_pos;
		m_destCamera.y += (1.0f - t) * 2.0f;
	} else {
		m_zoomAngle = 0.95993108f; m_nextCameraType = (m_flags & 0x20000000) ? 14 : 11;
	}
}

// camerawork.cpp:2530
void CCameraWork::CaddyViewCamera(float elapsed, float time) {
	WPuppet *pCaddie = CSceneManager::Instance()->GetPuppet("\xc4\xb3\xb5\xf0");

	if (pCaddie == 0)
		return;

	WVector dir = pCaddie->GetMatrix().pivot - GolfBall().m_pos;
	dir.y = MinOf(0.0f, dir.y + 10.0f);
	dir.Normalize();

	WBone *pHead = pCaddie->FindBoneByTail("Head");
	if (pHead)
		m_sourceCamera = WVector(pHead->GetMatrix().pivot.x, pHead->GetMatrix().pivot.y + 5.0f, pHead->GetMatrix().pivot.z);
	else
		m_sourceCamera = WVector(pCaddie->GetMatrix().pivot.x, pCaddie->GetMatrix().pivot.y + 10.0f, pCaddie->GetMatrix().pivot.z);

	m_sourceCamera += WCrossProduct(WVector::UNIT_POS_Y, dir) * 10.0f + dir * 35.0f;
	m_destCamera = GolfBall().m_pos;
}

// camerawork.cpp:2551
void CCameraWork::Drop_and_ZoomCamera(float elapsed, float time) {
	if (m_workTime < m_phase1Time) {
		float t = (time + m_workTime - m_phase0Time) / (m_phase1Time - m_phase0Time);
		m_zoomAngle = 0.26179940f - t * -0.69813168f;

		m_destCamera = GolfBall().m_pos;
	} else if (m_workTime < m_phase2Time) {
		float t = time + m_workTime - m_phase1Time;
		InterpolateBezier(&m_destCamera, t / (m_phase2Time - m_phase1Time), m_bezierPoints, 4);
	}
}

// camerawork.cpp:2586
void CCameraWork::BezierCamera(float elapsed, float time) {
	float t = (time + m_workTime - m_phase0Time) / (m_phase2Time - m_phase0Time);
	InterpolateBezier(&m_sourceCamera, t, m_bezierPoints, 4);

	if (m_workTime < m_phase1Time) {
		m_destCamera = GolfBall().m_pos;
		m_destCamera.y += (1.0f - t) * 2.0f;
	} else if (m_workTime < m_phase2Time) {
		float s = (time + m_workTime - m_phase1Time) / (m_phase2Time - m_phase1Time);
		m_destCamera = GolfBall().m_pos + WVector(0.0f, s * m_riseHeight, 0.0f);
	}
}

// camerawork.cpp:2607
void CCameraWork::FollowWithSwingCamera(float elapsed, float time) {
	float t = (time + m_workTime - m_phase0Time) / (m_phase2Time - m_phase0Time);
	InterpolateBezier(&m_sourceCamera, t, m_bezierPoints, 4);

	if (m_workTime < m_phase1Time)
		m_swingAngle -= elapsed * 0.034906585f;

	WVector offset = GolfBall().m_pos - m_sourceCamera; m_destCamera = offset * RotMat(m_swingAngle, 0) + m_sourceCamera;
}

// camerawork.cpp:2621
void CCameraWork::SidePanWithZoomInCamera(float elapsed, float time) {
	WVector dir = GolfBall().m_velocity;

	dir.y = 0.0f; dir.Normalize();

	WVector side = WCrossProduct(WVector::UNIT_POS_Y, dir).Normalize();

	if (m_cameraTime < 2.0f) {
		m_sourceCamera = GolfBall().m_pos + dir * ((1.0f - m_cameraTime * 3.0f) * 2.0f) + side * 3.0f;

		if (m_cameraTime < 0.5f) {
			m_destCamera = m_sourceCamera - side;
		} else {
			if (m_swingAngle > -1.3089969f)
				m_swingAngle -= elapsed * 1.0471976f;

			m_destCamera = m_sourceCamera - side * RotMat(m_swingAngle, 1);
		}
	} else {
		m_nextCameraType = 4;
	}
}

// camerawork.cpp:2649
void CCameraWork::FromMidPointCamera(float elapsed, float time) {
	if (m_flags & 0x20000000) {
		if (m_workTime < (float)m_workLength - 40.0f)
			m_destCamera = GolfBall().m_pos;
		else
			m_nextCameraType = 14;
	} else if (m_workTime < m_phase1Time) {
		m_destCamera = GolfBall().m_pos;
	} else if (m_phase1Time > 250.0f) {
		m_nextCameraType = 4;
	} else {
		float t = (time + m_workTime - m_phase1Time) / (m_phase2Time - m_phase1Time + 1.0f);
		m_destCamera = GolfBall().m_pos + WVector(0.0f, t * 5.0f, 0.0f);
	}
}

// camerawork.cpp:2675
void CCameraWork::FromColPointCamera(float elapsed, float time) {
	if (m_workTime < m_phase1Time)
		m_destCamera = GolfBall().m_pos;
	else {
		float t = (time + m_workTime - m_phase1Time) / (m_phase2Time - m_phase1Time);
		m_destCamera = GolfBall().m_pos + WVector(0.0f, t * 5.0f, 0.0f);
	}
}

// camerawork.cpp:2689
void CCameraWork::PangyaACamera(float elapsed, float time) {
	if (m_cameraTime > 2.0f)
		SetCamera(0x15, true);
}

// camerawork.cpp:2695
void CCameraWork::FixedAtSideCamera(float elapsed, float time) {
	m_destCamera = GolfBall().m_pos;
}

// camerawork.cpp:2700
void CCameraWork::FromRandomPointCamera(float elapsed, float time) {
	time = (time + m_workTime - m_phase0Time) / (m_phase2Time - m_phase0Time);

	m_destCamera = (1.0f - time) * GolfBall().PreData(0)->m_pos + time * GolfBall().m_pos + WVector(0.0f, 3.0f, 0.0f);
}

// camerawork.cpp:2708
void CCameraWork::BankFromHeadCamera(float elapsed, float time) {
	time = (time + m_workTime - m_phase0Time) / (m_duration - m_phase0Time);

	m_bankAngle = (time * 60.0f - 30.0f) * data_9eb420[2];

	WVector dir = GolfBall().m_velocity;
	dir.Normalize();

	WVector side = WCrossProduct(WVector::UNIT_POS_Y, dir);
	side.Normalize();

	m_destCamera = side * (time * 0.5f - 1.0f) + GolfBall().m_pos;
	m_sourceCamera = dir * ((1.0f - time) * 50.0f) + m_destCamera;

	if (m_workTime > m_duration)
		m_nextCameraType = 0;
}

// camerawork.cpp:2730
void CCameraWork::CobraACamera(float elapsed, float time) {
	if (m_preDataFrame < GolfBall().m_eventPreDataIndex[9]) {
		float t = (time + m_workTime) / GolfBall().m_eventPreDataIndex[9];

		WVector dir = GolfBall().PreData(GolfBall().m_eventPreDataIndex[9])->m_pos - GolfBall().PreData(0)->m_pos;
		WVector side = WCrossProduct(WVector::UNIT_POS_Y, dir); side.Normalize();
		side *= 5.0f;

		m_sourceCamera = GolfBall().PreData(0)->m_pos + (GolfBall().PreData(GolfBall().m_eventPreDataIndex[9])->m_pos - GolfBall().PreData(0)->m_pos) * (t * 0.3f + 0.4f) + side;
		m_destCamera = GolfBall().m_pos + WVector(0.0f, 1.0f - t, 0.0f);
	} else {
		m_nextCameraType = 4;
	}
}

// camerawork.cpp:2751
void CCameraWork::CobraBCamera(float elapsed, float time) {
	if (m_preDataFrame < GolfBall().m_eventPreDataIndex[9]) {
		float t = (time + m_workTime - m_phase0Time) / (GolfBall().m_eventPreDataIndex[9] - m_phase0Time);

		m_bankAngle = 0.26179940f;

		m_sourceCamera = (1.0f - time) * GolfBall().PreData((GolfBall().m_eventPreDataIndex[9] + m_preDataFrame * 4) / 5)->m_pos + time * GolfBall().PreData((GolfBall().m_eventPreDataIndex[9] + (m_preDataFrame + 1) * 4) / 5)->m_pos;
		m_sourceCamera.y += t * 2.0f - 1.0f;

		m_destCamera = GolfBall().m_pos;
	} else if (m_preDataFrame < GolfBall().m_eventPreDataIndex[0]) {
		m_destCamera.x = GolfBall().m_pos.x;
		m_destCamera.z = GolfBall().m_pos.z;

		if (m_preDataFrame < GolfBall().m_eventPreDataIndex[1]) {
			m_bankAngle -= elapsed * 0.087266460f;

			if (m_preDataFrame < GolfBall().m_eventPreDataIndex[9] + 50)
				m_sourceCamera.y += elapsed * 6.0f;
			else
				m_destCamera.y += elapsed * 10.0f;
		} else {
			if (m_destCamera.y > GolfBall().m_pos.y)
				m_destCamera.y -= elapsed * 20.0f;
			else
				m_destCamera.y -= elapsed * 5.0f;
		}
	}
}

// camerawork.cpp:2789
void CCameraWork::TomahawkACamera(float elapsed, float time) {
	if (m_cameraTime < 2.0f) {
		CGolfBall &ball = GolfBall();  // unused; the original still makes the call

		WMatrix view = MakeCameraMatrix(GolfBall().m_pos, GolfBall().m_pos - GolfBall().m_velocity);

		m_sourceCamera = WVector(-0.7f, -0.3f, m_cameraTime - 3.0f) * view;
		m_destCamera = GolfBall().PreData(m_preDataFrame - 25 > 0 ? m_preDataFrame - 25 : 0)->m_pos;
	} else {
		if (m_workTime > m_duration)
			m_nextCameraType = 4;
		else
			m_nextCameraType = 0x13;
	}
}

// camerawork.cpp:2814
void CCameraWork::SpikeACamera(float elapsed, float time) {
	if (m_duration - 75.0f > m_workTime) {
		float t = (time + m_workTime) / m_duration;
		m_zoomAngle = 0.95993108f - t * 0.61086524f;

		m_destCamera = GolfBall().m_pos;
		m_destCamera.y += (1.0f - t) * 2.0f;
	} else if (m_workTime < m_duration) {
		m_zoomAngle = 0.95993108f;
		float t = (time + m_workTime) / m_duration;
		m_bankAngle = (t * 60.0f - 30.0f) * data_9eb420[2];

		WVector dir = GolfBall().m_velocity;
		dir.Normalize();

		WVector side = WCrossProduct(WVector::UNIT_POS_Y, dir);
		side.Normalize();

		m_destCamera = side * (t * 0.5f - 1.0f) + GolfBall().m_pos;
		m_sourceCamera = dir * ((1.0f - t * 0.9f) * 50.0f) + m_destCamera;

		m_introFrom = GolfBall().m_pos - m_destCamera;
	} else if (m_preDataFrame < GolfBall().m_eventPreDataIndex[13]) {
		if (m_bankAngle > 0.0f) {
			m_bankAngle -= elapsed * 1.0471976f;
			if (m_bankAngle < 0.0f)
				m_bankAngle = 0.0f;
		}

		WVector dir = GolfBall().m_pos - m_sourceCamera;

		dir.y = 0.0f; m_introDistance = sqrt(dir.z * dir.z + dir.x * dir.x); dir.Normalize(); dir = WCrossProduct(dir, WVector::UNIT_POS_Y);
		float s = (m_workTime - m_duration) * 0.02f;
		if (s < 1.0f) {
			m_sourceCamera += dir * ((s * 1.2f + 1.0f) * m_introDistance * 2.0943952f) * elapsed;
			m_sourceCamera.y += elapsed;
			m_destCamera.y += elapsed * 0.75f;
			m_destCamera += m_introFrom * elapsed;
		}
	} else {
		m_sourceCamera += GolfBall().m_velocity * 0.93f * elapsed;
		m_destCamera += GolfBall().m_velocity * 0.93f * elapsed;

		if (GolfBall().m_eventPreDataIndex[6] == -1 || m_preDataFrame < GolfBall().m_eventPreDataIndex[6])
			m_destCamera.y -= elapsed * 0.5f;
	}
}

// camerawork.cpp:2888
void CCameraWork::TopViewCamera(float elapsed, float time) {
	m_destCamera = m_destCamera * 0.9f + GolfBall().m_pos * 0.1f;

	WVector dir = m_baseMatrix.pivot - GolfBall().PreData(GolfBall().m_preDataCount)->m_pos;
	float distance = GolfBall().m_velocity.Magnitude() * 2.0f;
	distance = distance < 100.0f ? 100.0f : (distance > 150.0f ? 150.0f : distance);

	dir.y = 0.0f; dir.Normalize();

	dir.y = 2.0f; dir.Normalize();

	WVector target = GolfBall().m_pos + dir * distance;

	m_sourceCamera = m_sourceCamera * 0.9f + target * 0.1f; InterpolateBezier(&m_sourceCamera, 0.5f, &m_sourceCamera, 3);
}

// camerawork.cpp:2910
void CCameraWork::JustLookingCamera(float elapsed, float time) {
	if (m_cameraType == 100) {
		WVector axis = m_baseMatrixCopy.za;
		WVector toBall = (GolfBall().m_pos - m_sourceCamera).Normalize();
		WVector toStart = (m_ballStartPos - m_baseMatrixCopy.pivot).Normalize();

		if (toBall.y > axis.y || toBall.y < toStart.y) {
			toBall.y = (toBall.y - axis.y) * elapsed * 0.1f + axis.y;
			m_destCamera = toBall + m_sourceCamera;
		} else {
			float scale = sqrtf((1.0f - axis.y * axis.y) / (1.0f - toBall.y * toBall.y));

			toBall.x *= scale; toBall.y = axis.y; toBall.z *= scale; m_destCamera = toBall + m_sourceCamera;
		}
	} else {
		m_destCamera = GolfBall().m_pos;
	}

	if (m_phase2Time > 180.0f && m_workTime > m_phase2Time * 0.5f
		&& (GolfBall().PreData(GolfBall().m_preDataCount)->m_pos - GetHoleCupPos()) * (GolfBall().PreData(0)->m_pos - GetHoleCupPos()) > 0.0f
		&& (GolfBall().PreData(GolfBall().m_preDataCount)->m_pos - GetHoleCupPos()).Magnitude() < 16.0f)
		SetCamera(m_fixedCameraIndex % 2 + 0x6f, true);
}

// camerawork.cpp:2945
void CCameraWork::SideCamera(float elapsed, float time) {
	WVector velocity = GolfBall().m_velocity;

	velocity.y = 0.0f;
	m_sourceCamera += velocity * elapsed * 0.8f;
	m_destCamera = GolfBall().m_pos;
	m_destCamera.y += 2.5f;
}

// camerawork.cpp:2955
void CCameraWork::BallSideCamera(float elapsed, float time) {
	WVector back = GolfBall().m_velocity;
	WVector side;
	side = WCrossProduct(WVector::UNIT_POS_Y, back);
	back = back.Normalize() * GolfBall().GetRadius() * 2.0f;
	side = side.Normalize() * GolfBall().GetRadius() * 1.2f;

	float t = MinOf(1.0f, (time + m_workTime) / m_phase2Time);

	m_destCamera = GolfBall().m_pos + side - back;
	m_sourceCamera = (m_destCamera - m_baseMatrixCopy.pivot) * t + m_baseMatrixCopy.pivot - back;
}

// camerawork.cpp:2971
void CCameraWork::FollowBallCamera(float elapsed, float time) {
	// One vector: the ball's offset from the cup, then its velocity.
	WVector cup = GOLFDOC()->GetHoleData().m_cupPos; WVector v = GolfBall().m_pos - cup;
	v.y = 0.0f;
	float cupDistance2 = v.x * v.x + v.z * v.z;

	v = GolfBall().m_velocity;

	v.y = 0.0f;

	if (sqrt(v.z * v.z + v.x * v.x) > 1.0f && sqrt(cupDistance2) > GolfBall().GetRadius() * 3.0f) { m_destCamera = GetPVS().GetGroundPoint(GolfBall().m_pos, false, 0, false); m_destCamera.y += GolfBall().GetRadius(); m_sourceCamera = m_destCamera - v.Normalize() * 3.2f; m_sourceCamera.y += 1.0f; }
}

// camerawork.cpp:2990
void CCameraWork::FixedCamera(float elapsed, float time) {
	if (!g_view->InFrustumSafe(WSphere(GolfBall().m_pos, GolfBall().GetRadius())))
		SetCamera(m_fixedCameraIndex % 2 + 0x6d, true);
}

// camerawork.cpp:2997
void CCameraWork::PuttSecondACamera(float elapsed, float time) {
	m_sourceCamera.y -= elapsed * 9.0f;
	m_destCamera = GolfBall().m_pos;
}

// camerawork.cpp:3003
void CCameraWork::PuttSecondBCamera(float elapsed, float time) {
	if ((g_camera.pivot - WVector(GOLFDOC()->GetHoleData().m_cupPos)).Magnitude() < 16.0f) {
		m_pCameraProc = 0;
		return;
	}

	WVector dir = GolfBall().PreData(0)->m_pos - WVector(GOLFDOC()->GetHoleData().m_cupPos);
	dir.y = 0.0f;
	dir.Normalize();
	dir *= elapsed * 20.0f;

	m_sourceCamera += dir;
	m_destCamera = GolfBall().m_pos;

	m_zoomAngle += elapsed * 0.17453292f;
}

// camerawork.cpp:3024
void CCameraWork::FromHoleCupCamera(float elapsed, float time) {
	WVector cup = GOLFDOC()->GetHoleData().m_cupPos; WVector& source = m_sourceCamera = WVector(cup.x, cup.y - 2.0f, cup.z);
	m_destCamera = WVector(source.x, source.y + 1.0f, source.z + 0.2f);
}

// camerawork.cpp:3032
void CCameraWork::IntroACamera(float elapsed, float time) {
	m_swingAngle -= elapsed * 0.52359879f;
	m_introDistance -= elapsed * 11.0f;
	m_introHeight -= elapsed * 2.0f;

	WMatrix rot = RotMat(m_swingAngle + GOLFDOC()->GetCameraData().m_yaw, 1);
	m_sourceCamera = GolfBall().m_pos - rot.za * m_introDistance;
	m_sourceCamera.y += m_introHeight;

	m_destCamera = rot.za + m_sourceCamera;

	if (m_swingAngle < 0.0f)
		m_nextCameraType = 0xcc;
}

// camerawork.cpp:3048
void CCameraWork::IntroBCamera(float elapsed, float time) {
	float t = m_cameraTime * 0.2f;
	m_sourceCamera = (m_introTo - m_introFrom) * t + m_introFrom;
	m_sourceCamera.y += m_introHeight;

	m_destCamera = GolfBall().m_pos;
	float x = t * 2.0f - 1.0f; m_destCamera.y += m_introHeight - (m_introHeight - 3.0f) * (x > 0.0f ? x : -x);

	if (m_cameraTime > 5.0f)
		m_nextCameraType = 0xcd;
}

// camerawork.cpp:3061
void CCameraWork::ZoomOutCamera(float elapsed, float time) {
	m_zoomAngle += elapsed * 0.95993108f;

	if (m_zoomAngle > 0.95993108f)
		m_zoomAngle = 0.95993108f;
}

// camerawork.cpp:3069
void CCameraWork::IntroCCamera(float elapsed, float time) {
	m_swingAngle -= elapsed * 0.52359879f;
	m_introDistance += elapsed * 2.0f;

	WPuppet *pPuppet = CSceneManager::Instance()->GetPuppet(MakeStr("PLAYER_%d", GOLFDOC()->m_myPlayerIndex));
	WBone *pHead = pPuppet->FindBone("Bip01 Head");
	float t = m_cameraTime * 0.2f;

	m_destCamera = (1.0f - t) * pHead->GetMatrix().pivot + (GolfBall().m_pos + WVector(0.0f, 5.0f, 0.0f)) * t;

	WMatrix rot = RotMat(GOLFDOC()->GetCameraData().m_yaw + m_swingAngle, 1);
	m_sourceCamera = m_destCamera - rot.za * m_introDistance;

	if (t > 1.0f)
		m_nextCameraType = 0xcc;
}

// camerawork.cpp:3087
void CCameraWork::IntroDCamera(float elapsed, float time) {
	if (m_cameraTime < 3.0f) {
		WVector move = elapsed * GOLFDOC()->GetCameraData().m_matrix.xa;
		m_sourceCamera += move;
		m_destCamera += move;
	} else if (m_cameraTime > 4.0f) {
		m_sourceCamera = GolfBall().m_pos - RotMat(GOLFDOC()->GetCameraData().m_yaw, 1).za * 25.0f;
		m_sourceCamera.y += 5.0f;
		m_destCamera = m_sourceCamera + RotMat(GOLFDOC()->GetCameraData().m_yaw, 1).za;
		m_baseMatrixCopy = GetCameraMatrix(m_sourceCamera, m_destCamera, 0.0f);

		m_nextCameraType = 0xCC;
	}
}

// camerawork.cpp:3106
void CCameraWork::ToDefaultCamera(float elapsed, float time) {
	if (!(m_cameraTime > m_transitionTime)) {
		m_sourceCamera = (m_introTo - m_introFrom) * m_cameraTime + m_introFrom;

		WMatrix rot;
		rot = WQuaternionSlerp(m_fromRotation, m_toRotation, m_cameraTime);

		m_destCamera = rot.za + m_sourceCamera;
	}
}

// camerawork.cpp:3119
void CCameraWork::FirstPersonCamera(float elapsed, float time) {
}

// camerawork.cpp:3123
void CCameraWork::StillFrameCamera(float elapsed, float time) {
}

// camerawork.cpp:3128
void CCameraWork::CaddieChanceCamera(float elapsed, float time) {
	float t = Between(0.0f, 1.0f - m_cameraTime * 0.33333334f, 1.0f);
	WVector offset = WVector::UNIT_POS_Z * m_baseRotation; offset = offset * RotMat(t * m_swingAngle, 1);

	m_sourceCamera = (m_introFrom - m_introTo) * CAMERA_CLAMP(t, 0.0f, 1.0f) + m_introTo;
	m_destCamera = offset + m_sourceCamera;
}

// camerawork.cpp:3140
void CCameraWork::SwingReplayACamera(float elapsed, float time) {
}

// camerawork.cpp:3145
void CCameraWork::SwingReplayBCamera(float elapsed, float time) {
	if (!(m_cameraTime > m_transitionTime)) {
		float t = m_cameraTime / m_transitionTime;

		m_sourceCamera = (m_introTo - m_introFrom) * t + m_introFrom;

		WMatrix rot;
		rot = WQuaternionSlerp(m_fromRotation, m_toRotation, t);

		m_destCamera = rot.za + m_sourceCamera;
	}
}

// camerawork.cpp:3160
void CCameraWork::ExtraTimeCamera(float elapsed, float time) {
	m_swingAngle += elapsed * 0.08726646f;

	WMatrix rot;
	rot.Reset();
	rot.Rotate(m_swingAngle, 1);

	m_sourceCamera = rot.za * m_introDistance + m_destCamera;
	m_sourceCamera.y += m_introDistance * 0.2f;
}

// camerawork.cpp:3172
void CCameraWork::Quake(void) {
	m_flags |= 0x80000000; m_quakeStartTime = m_cameraTime;
	m_lastQuakeTime = -1.0f;
}

// camerawork.cpp:3179
WMatrix CCameraWork::GetCameraMatrix(const WVector &eye, const WVector &at, float roll) {
	if (WAbsBySign(roll) < WMATH_EPSILON)
		return MakeCameraMatrix(eye, at);
	else
		return RotMat(roll, 2) * MakeCameraMatrix(eye, at);
}
