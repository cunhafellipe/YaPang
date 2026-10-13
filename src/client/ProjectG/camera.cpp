// Original: d:\build\custom\temp\code\client\projectg\camera.cpp
// camera's object holds the out-of-line copies of these headers' inlines.
#define CAMERA_MOUSECURSOR_INLINE
#define CAMERA_CLUB_INLINE

#include "NetworkSystem.h"
#include "camera.h"
#include <math.h>
#include "ObjectFactory.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "ProjectG.h"

#include "wview.h"
#include "quadtree.h"
#include "mousecursor.h"
#include "club.h"
#include "golfball.h"
#include "QuitWindow.h"
#include "ItemWindow.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "ChatMsg.h"
#include "wutil.h"
#include "CapturedBg.h"
#include "Packet.h"
#include "commonutil.h"
#include "polysoup.h"
#include "HelpMsg.h"
#include "Fresh.h"
#include "frwndmanager.h"
#include "user_info.h"
#include "messengerdlg.h"

extern WMatrix g_camera;


// Input of the previous frame, compared by the Get*Acc ramps.
static int data_00af2adc;
static int data_00af2ae0;
static int data_00af2ae4;

// Key-repeat state of ProcessInput's fast up/down moves (HOME/END).
static bool data_00af2ae8;          // the down move ran last
static bool data_00af2ae9;          // the up move ran last
static unsigned int data_00af2aec;  // repeat count, up to 25
static bool data_00af2af0;          // repeating
static unsigned long data_00af2af4; // timeGetTime of this frame
static unsigned long data_00af2af8; // timeGetTime of the last step
static bool data_00a7fb84 = true;   // the key was just pressed

// camera.cpp:33 (run-time type and factory registration of CCamera)
const WRTTI CCamera::m_RTTI("CCamera", &IActor::m_RTTI);

IObject *CCameraMakeInstance(void) {
	return new CCamera;
}

struct __impCCamera {
	__impCCamera(void) {
		ObjectFactory().AddObjectFunctor(CCameraMakeInstance, "CCamera");
		ActorFactory().AddActorClass("CCamera");
	}
};
static __impCCamera s_impCCamera;

// camera.cpp:56
CCamera::CCamera(void) {
	AddSkipList(0x84c82);
}

// camera.cpp:61
void CCamera::OnInit(void) {
	m_rotateAcc = 0;
	m_moveAcc = 0;
	m_horizonAcc = 0;

	m_unk50 = 0;
	m_zoom = 0;
	m_unk60 = false;
	m_inputDelay = 0;
	m_camMode = false;
	m_yawSendTime = 0;
	m_yawChanged = false;
	m_enabled = false;

	m_forcedMoveTotal = m_forcedMoveLeft = 0;

	m_spotTime = 0;
	m_spotMoving = false;

	m_baseYaw = 0;
	m_yaw = 0;
}

// camera.cpp:85
void CCamera::OnProcess(float dt) {
	if (!m_enabled || CCapturedBg::Instance()->IsCaptured() || (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_identity & 1))
		return;

	if (g_input->GetButton(MIDDLE_BUTTON) == 1 && CItemWindow::IsClosed()) {
		m_camMode = !m_camMode;
		CMouseCursor::Instance()->SetCamMode(m_camMode);

		if (m_camMode)
			SendMsg(this, "Screen", 0x1b1, 0, 0, 0, 0);
	}

	if (CMouseCursor::Instance()->GetArea() == 0x16 && CMouseCursor::Instance()->IsInButtonDownArea(LEFT_BUTTON) && g_input->GetButton(LEFT_BUTTON) == 2)
		SendMsg(this, "GolfRule", 0x78, 8, 0, 0, 0);

	if (OnlinePlay() && CSharedDoc::Instance()->m_replayState != 2) {
		if (IsMyTurn(false)) {
			m_yawSendTime += dt;

			if (m_yawSendTime > 2.0f) {
				m_yawSendTime -= 2.0f;
				if (m_yawChanged) {
					m_yawChanged = false;
					WSendPacket packet((enumClientPacket)0x13);
					packet.EncodeBuffer(&GOLFDOC()->GetCameraData().m_yaw, 4);
					packet.Send(SEND_TO_DEFAULT);
				}
			}
			goto process;
		}
	} else if (CSharedDoc::Instance()->m_gameMode != 5 || IsMyTurn(false)) {
		goto process;
	}

	{
		float angle = Between(dt * -0.5235988f, GetBaseAngle(m_yaw - m_baseYaw), dt * 0.5235988f);
		if (!WisZero(angle, 1e-5f)) {
			SendMsg(this, "Player", 0x1a1, (intptr_t)&angle, 0, 0, 0);
			SendMsg(this, "Spot", 0x1a1, (intptr_t)&angle, 0, 0, 0);
			SendMsg(this, "Screen", 0x1a1, (intptr_t)&angle, 0, 0, 0);
			SendMsg(this, "Grid", 0x1a1, (intptr_t)&angle, 0, 0, 0);
			SendMsg(this, "Caddie", 0x1a1, (intptr_t)&angle, 0, 0, 0);
			SendMsg(this, "GuideLine", 0x1a1, 0, 0, 0, 0);
			m_baseYaw = GetBaseAngle(angle + m_baseYaw);
			RotateForGallery(angle);
		}
	}

process:
	ProcessToSpot(dt);
	FixedCamera(dt);

	if (GOLFDOC()->GameMode() & 0x8000)
		return;

	if (m_inputDelay > 0.0f) {
		m_inputDelay -= dt;
		return;
	}

	if (WisZero(m_forcedMoveLeft, 1e-5f)) {
		ProcessInput(dt);
	} else {
		ProcessForcedMove(dt);
	}

	GetRotateAcc(dt);
	GetMoveAcc(dt);
	GetHorizonAcc(dt);
}

// camera.cpp:187
void CCamera::ProcessToSpot(float dt) {
	if (GOLFDOC()->m_gameState < 1)
		return;

	if (CQuitWindow::Instance()->IsActive() || !CItemWindow::IsClosed())
		return;

	if (!IsMyTurn(false) || !WisZero(m_forcedMoveLeft, 1e-5f))
		return;

	if (g_input->GetDown("NUM_1", true) || g_input->GetDown("PAD_1", true)) {
		if (GOLFDOC()->GameMode() != 0x40000 && GetPVS().GetMeshIndex(GolfBall().m_pos + GolfClub().GetRange(0) * 3.2f * GOLFDOC()->GetCameraData().m_matrix.za, false) == -1)
			return;

		switch (GOLFDOC()->GameMode()) {
		case 0x40000:
			SendMsg(this, "GolfRule", 0x78, 8, 0, 0, 0);
			break;
		case 8:
		case 0x100:
			SendMsg(this, "GolfRule", 0x78, 0x40000, 0, 0, 0);
			SetSpotCamera(false);
			g_audio->PlaySfx("camera_move_to_spot", 0, 0, 0, 0, 0.5f, 200.0f);
			break;
		case 0x200:
			if (GOLFDOC()->m_dropPending)
				break;
		case 0x8000:
			SendMsg(this, "GolfRule", 0x78, 0x40000, 0, 0, 0);
			SetSpotCamera(true);
			g_audio->PlaySfx("camera_move_to_spot", 0, 0, 0, 0, 0.5f, 200.0f);
			break;
		}
	}

	if (GOLFDOC()->GameMode() != 0x40000)
		return;

	if (GolfClub().Updated())
		SetSpotCamera(true);

	if (!m_spotMoving)
		return;

	m_spotTime += dt;
	if (m_spotTime > 1.2f) {
		m_spotTime = 1.2f;
		m_spotMoving = false;
	}

	m_spotCam[2].m_pitch = (m_spotCam[1].m_pitch - m_spotCam[0].m_pitch) * m_spotTime + m_spotCam[0].m_pitch;
	m_pos = m_spotCam[0].m_pos + m_spotTime * m_spotCam[2].m_pos;
	m_pos.y = Max(GetPVS().GetGroundPoint(m_pos, false, NULL, true).y + GOLFDOC()->GetCameraData().m_groundOffset, m_pos.y);
	m_at = m_pos + GOLFDOC()->GetCameraData().m_matrix.za;
	m_at.y += tan(m_spotCam[2].m_pitch);

	g_camera = MakeCameraMatrix(m_pos, m_at);
}

// camera.cpp:257
void CCamera::SetSpotCamera(bool show) {
	m_spotMoving = !show;

	int groundType;
	m_spotCam[1].m_pos = GetPVS().GetGroundPoint(GolfClub().GetRange(0) * 3.2f * GOLFDOC()->GetCameraData().m_matrix.za + GolfBall().m_pos, true, &groundType, true);

	if (groundType == -1) {
		if (show)
			SendMsg(this, "GolfRule", 0x78, 8, 0, 0, 0);
		return;
	}

	m_spotCam[1].m_pos += RotVec(WVector(0.0f, 30.0f, -60.0f), GOLFDOC()->GetCameraData().m_matrix);
	m_spotCam[1].m_pitch = -0.2617994f;

	if (show) {
		m_pos = m_spotCam[1].m_pos;
		m_at = m_spotCam[1].m_pos + GOLFDOC()->GetCameraData().m_matrix.za;
		m_at.y += tan(m_spotCam[1].m_pitch);
		LookAt(m_pos, m_at);
	} else {
		m_spotCam[0].m_pos = g_camera.pivot;
		m_spotCam[0].m_pitch = atan(g_camera.za.y / sqrt(1.0f - g_camera.za.y * g_camera.za.y));

		m_spotCam[2].m_pos = (m_spotCam[1].m_pos - m_spotCam[0].m_pos) * 0.8333333f;
		m_spotTime = 0;
	}
}

// camera.cpp:294
void CCamera::ProcessInput(float dt) {
	m_rotateKey = 0;
	m_moveKey = 0;
	m_horizonKey = 0;

	if ((GOLFDOC()->GameMode() & 0x40000) && m_spotMoving)
		return;

	if (CQuitWindow::Instance()->IsActive() || !CItemWindow::IsClosed())
		return;

	// "\xC1\xC2" (left)
	if ((CMouseCursor::Instance()->GetArea() == 0x17 && g_input->GetButton(LEFT_BUTTON) > 0) || (g_input->Get("\xC1\xC2", true) && !g_input->Get("LCONTROL", true) && !g_input->Get("RCONTROL", true) && !g_input->Get("LSHIFT", true) && !g_input->Get("RSHIFT", true))) {
		m_rotateKey = 1;
		Rotate(dt * 1.0471976f);
	}

	// "\xBF\xEC" (right)
	if ((CMouseCursor::Instance()->GetArea() == 0x18 && g_input->GetButton(LEFT_BUTTON) > 0) || (g_input->Get("\xBF\xEC", true) && !g_input->Get("LCONTROL", true) && !g_input->Get("RCONTROL", true) && !g_input->Get("LSHIFT", true) && !g_input->Get("RSHIFT", true))) {
		m_rotateKey = -1;
		Rotate(dt * -1.0471976f);
	}

	if (GOLFDOC()->GameMode() & 0x40000)
		return;

	// "\xBB\xF3" (up)
	if ((CMouseCursor::Instance()->GetArea() == 8 && g_pFresh->IsDesktopFocused()) || (CMouseCursor::Instance()->GetArea() == 0x19 && g_input->GetButton(LEFT_BUTTON) > 0) || (g_input->Get("\xBB\xF3", true) && !g_input->Get("LCONTROL", true) && !g_input->Get("RCONTROL", true) && !g_input->Get("LSHIFT", true) && !g_input->Get("RSHIFT", true))) {
		if (GOLFDOC()->GameMode() == 0x200) {
			m_horizonKey = 1;
			Translate(dt);
		} else if (m_unk50 < 1.0f) {
			m_moveKey = 1;
			UpDown(dt);
		}
	}

	// "\xC7\xCF" (down)
	if ((CMouseCursor::Instance()->GetArea() == 9 && g_pFresh->IsDesktopFocused()) || (CMouseCursor::Instance()->GetArea() == 0x1a && g_input->GetButton(LEFT_BUTTON) > 0) || (g_input->Get("\xC7\xCF", true) && !g_input->Get("LCONTROL", true) && !g_input->Get("RCONTROL", true) && !g_input->Get("LSHIFT", true) && !g_input->Get("RSHIFT", true))) {
		if (GOLFDOC()->GameMode() == 0x200) {
			m_horizonKey = -1;
			Translate(dt * -1.0f);
		} else if (m_unk50 > 0.0f) {
			m_moveKey = -1;
			UpDown(dt * -1.0f);
		}
	}

	float boost = 0;
	if ((CMouseCursor::Instance()->GetArea() == 0x1b && g_input->GetButton(LEFT_BUTTON) > 0) || (g_input->Get("\xBB\xF3", true) && (g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true))) || g_input->Get("HOME", true)) {
		if (GOLFDOC()->GameMode() == 0x200) {
			data_00af2af4 = timeGetTime();
			if (data_00af2af4 - data_00af2af8 > 800 && !data_00af2af0) {
				data_00af2af8 = data_00af2af4;
				if (!data_00a7fb84)
					data_00af2af0 = true;
				data_00a7fb84 = false;
			}

			if (data_00af2af4 - data_00af2af8 > 50 && data_00af2af0 == true) {
				data_00af2af8 = data_00af2af4;
				if (data_00af2aec < 25)
					data_00af2aec++;
				boost = data_00af2aec / 2 * dt;
			}

			if (m_unk50 > 0.0f) {
				m_moveKey = -1;
				UpDown((boost + dt) * -1.0f);
			}
			data_00af2ae9 = true;
			data_00af2ae8 = false;
		} else if (IsMyTurn(false)) {
			m_horizonKey = 1;
			Translate(dt);
		}
	} else if (!data_00af2ae8) {
		data_00af2af4 = 0;
		data_00af2af8 = 0;
		data_00af2af0 = false;
		boost = 0;
		data_00af2aec = 0;
		data_00af2ae9 = false;
		data_00a7fb84 = true;
	}

	if ((CMouseCursor::Instance()->GetArea() == 0x1c && g_input->GetButton(LEFT_BUTTON) > 0) || (g_input->Get("\xC7\xCF", true) && (g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true))) || g_input->Get("END", true)) {
		if (GOLFDOC()->GameMode() == 0x200) {
			data_00af2af4 = timeGetTime();
			if (data_00af2af4 - data_00af2af8 > 800 && !data_00af2af0) {
				data_00af2af8 = data_00af2af4;
				if (!data_00a7fb84)
					data_00af2af0 = true;
				data_00a7fb84 = false;
			}

			if (data_00af2af4 - data_00af2af8 > 50 && data_00af2af0 == true) {
				data_00af2af8 = data_00af2af4;
				if (data_00af2aec < 25)
					data_00af2aec++;
				boost = data_00af2aec / 2 * dt;
			}

			if (m_unk50 < 1.0f) {
				m_moveKey = 1;
				UpDown(boost + dt);
			}
			data_00af2ae8 = true;
			data_00af2ae9 = false;
		} else if (IsMyTurn(false)) {
			m_horizonKey = -1;
			Translate(dt * -1.0f);
		}
	} else if (!data_00af2ae9) {
		data_00af2af4 = 0;
		data_00af2af8 = 0;
		data_00af2af0 = false;
		boost = 0;
		data_00af2aec = 0;
		data_00af2ae8 = false;
		data_00a7fb84 = true;
	}

	bool guideBookShown;
	SendMsg(this, "GuideBook", 0x12c, (intptr_t)&guideBookShown, 0, 0, 0);

	bool scoreShown;
	SendMsg(this, "Score", 0x139, (intptr_t)&scoreShown, 0, 0, 0);

	if ((GOLFDOC()->GameMode() & 0x200) && IsMyTurn(false) && m_moveKey == 0 && m_horizonKey == 0 && CMouseCursor::Instance()->GetArea() == 0 && g_input->GetButton(LEFT_BUTTON) == 1 && GOLFDOC()->m_gameState >= 1 && !CUserInfo::Instance()->IsVisible() && !guideBookShown && !scoreShown && !g_pFresh->GetManager()->HasValidWindow() && CMessengerInfo::Instance()->m_gameFocus == true)
		ProcessMousePick(dt);
}

// camera.cpp:504
void CCamera::ProcessMousePick(float dt) {
	WVector vec = g_input->GetMousePoint();

	WVector from, dir, pick;
	g_view->GetScreen2World(vec.x, vec.y, from, dir);
	pick = GetPVS().GetRayIntersection(from, dir);

	if (g_input->Get("LCONTROL", true) || g_input->Get("RCONTROL", true)) {
		pick.y += GolfBall().m_radius;

		if (GOLFDOC()->m_dropPending) {
			WVector ballFromCup, pickFromCup;
			WVector cup = GOLFDOC()->GetHoleData().m_cupPos;
			ballFromCup = GolfBall().m_pos - cup;

			ballFromCup.y = 0; cup = GOLFDOC()->GetHoleData().m_cupPos; pickFromCup = pick - cup; pickFromCup.y = 0; unsigned char groundType = GetPVS().GetGroundType(pick, true, NULL);
			bool overlapped = GetPVS().OverlappedWithObject(pick, 0.0f);

			if (!overlapped && (groundType == 0 || (groundType == 1 && !GOLFDOC()->m_pPolySoup->IsOutOfBound(pick) && pickFromCup.Magnitude() > ballFromCup.Magnitude() - 25.6f))) {
				GOLFDOC()->m_dropPending = false;

				if (OnlinePlay() && GOLFDOC()->m_gameState == 0xf) {
					WSendPacket packet((enumClientPacket)0x19);
					packet.EncodeBuffer(&pick, 12);
					packet.Send(SEND_TO_DEFAULT);
				} else {
					SendMsg(this, "GolfRule", 0x8d, (intptr_t)&pick, 0, 0, 0);
				}
			} else {
				g_audio->PlaySfx("play_unplayable", 0, 0, 0, 0, 0.5f, 200.0f);
				if (CHelpMsg::Instance())
					CHelpMsg::Instance()->SetHelpMsg(0x14, NULL);
			}
		} else if (!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
			SendMsg(this, "GolfRule", 0x8e, (intptr_t)&pick, 0, 0, 0);
		}
		return;
	}

	WVector ball = g_view->Projection(GolfBall().m_pos);
	float dx = vec.x - ball.x;
	float dy = vec.y - ball.y;

	if (sqrt(dx * dx + dy * dy) > 10.0f) {
		dir = pick - GolfBall().m_pos;

		dir.y = 0; float distSq = dir.z * dir.z + dir.x * dir.x; dir.Normalize();

		GolfClub().SelectClubByDist(sqrt(distSq) * 0.3125f);

		vec = WCrossProduct(g_camera.ya, dir);

		// The original reads the epsilon from a named constant (0x9ED8B4), not a __real.
		if (!(WAbsBySign(vec.y) < g_EPSILON)) {
			float angle = acos(Between(-1.0f, dir.z * g_camera.ya.z + dir.x * g_camera.ya.x + dir.y * g_camera.ya.y, 1.0f));

			if (!WisZero(angle, 1e-5f)) {
				if (vec.y > 0.0f) {
					angle *= -1.0f;
					m_rotateKey = -1;
				} else {
					m_rotateKey = 1;
				}

				angle = Between(-3.1415927f, angle, 3.1415927f);
				m_forcedMoveTotal = m_forcedMoveLeft = angle;

				angle += GOLFDOC()->GetCameraData().m_yaw;
				SendMsg(this, "Spot", 0xf6, (intptr_t)&angle, (intptr_t)&pick, 0, 0);
				return;
			}
		}

		SendMsg(this, "Spot", 0xf5, 0, 0, 0, 0);
	}
}

// camera.cpp:649
void CCamera::ProcessForcedMove(float dt) {
	Rotate((m_rotateKey == 1 ? 1.0f : -1.0f) * dt * 4.1887903f);

	if (WAbsBySign(m_forcedMoveTotal) < 1e-5f) {
		dt = 1.0f;
	} else {
		dt = 1.0f - m_forcedMoveLeft / m_forcedMoveTotal;
	}
	SendMsg(this, "Spot", 0xf7, (intptr_t)&dt, 0, 0, 0);
}

// camera.cpp:663
void CCamera::HandleMsg(const MsgObject &msg) {
	if ((unsigned long)msg.m_reserved > g_CurrentTime) {
		StoreDelayedMsg(msg);
		return;
	}

	switch (msg.m_msg) {
	case 0xf1:
		m_zoom = 0;
		m_unk50 = 0.5f;
		break;
	case 0x2718:
		m_yawSendTime = 0;
		GOLFDOC()->GetCameraData().m_yaw = *(float *)msg.m_param1;
		if (!IsMyTurn(false)) {
			m_yaw = GOLFDOC()->GetCameraData().m_yaw;
			m_baseYaw = GOLFDOC()->GetCameraData().m_yaw;
		}
		break;
	case 0xee:
		m_pos = GOLFDOC()->GetCameraData().m_matrix.pivot;
		m_at = GOLFDOC()->GetCameraData().m_matrix.za + GOLFDOC()->GetCameraData().m_matrix.pivot;
		m_at.y += tan(GOLFDOC()->GetCameraData().m_pitch);

		m_zoom = 0;
		m_startHeightRate = GetHeightRate(GOLFDOC()->GetCameraData().m_matrix.pivot.y);
		m_unk50 = m_startHeightRate;
		m_heightRate = GetHeightRate(GOLFDOC()->GetCameraData().m_matrix.pivot.y);
		break;
	case 0xef:
		m_baseHeight = GetPVS().m_lowest;
		m_maxHeight = m_baseHeight + 192.0f;
		break;
	case 0xf0:
		m_unk60 = false;
		break;
	case 0x19a:
		m_enabled = true;
		break;
	case 0x19b:
		m_enabled = false;
		break;
	case 0xf2:
		m_yaw = *(float *)msg.m_param1;
		break;
	case 0x19c:
		*(int *)msg.m_param1 = m_enabled;
		break;
	}
}

// camera.cpp:729
void CCamera::LookAt(WVector &pos, WVector &at) {
	g_camera = MakeCameraMatrix(pos, at);
}

// camera.cpp:734
void CCamera::Rotate(float angle) {
	angle *= m_rotateAcc;

	if (m_forcedMoveLeft < 0.0f || 0.0f < m_forcedMoveLeft) {
		if (WAbsBySign(m_forcedMoveLeft) < WAbsBySign(angle) || WAbsBySign(m_forcedMoveLeft - angle) < 1e-5f) {
			angle = m_forcedMoveLeft;
			m_forcedMoveTotal = m_forcedMoveLeft = 0;
		} else {
			m_forcedMoveLeft -= angle;
		}
	}

	WMatrix mat = g_camera * RotMat(angle, 1);
	mat = g_camera;

	mat.pivot.x = mat.pivot.y = mat.pivot.z = 0.0f; mat = mat * RotMat(angle, 1);

	RotateEnvironment(angle);

	if (GOLFDOC()->GameMode() & 0x8) {
		mat.pivot = GOLFDOC()->GetCameraData().m_matrix.pivot;
	} else {
		CGolfDoc *doc = GOLFDOC();
		mat.pivot = (g_camera.pivot - doc->GetCameraData().m_target) * RotMat(angle, 1) + doc->GetCameraData().m_target;
		mat.pivot = GetPVS().TestCameraCollision(mat.pivot);
		if (GOLFDOC()->GameMode() & 0x200)
			mat.pivot.y = g_camera.pivot.y;
	}

	g_camera = mat;

	m_pos = g_camera.pivot;
	m_at = g_camera.za + g_camera.pivot;

	if ((GOLFDOC()->GameMode() & 0x40000) && !m_spotMoving)
		SetSpotCamera(true);
}

// camera.cpp:780
void CCamera::Translate(float dt) {
	if (GOLFDOC()->GameMode() == 0x200) {
		float rate = Between(0.1f, (g_camera.pivot.y - GetPVS().GetGroundPoint(g_camera.pivot, true, NULL, true).y) * 0.01f, 1.0f);

		dt = rate * m_horizonAcc * dt;
		g_camera.pivot += GOLFDOC()->GetCameraData().m_matrix.za * (dt * 400.0f); g_camera.pivot = GetPVS().TestCameraCollision(g_camera.pivot);
		return;
	}

	dt = dt * m_horizonAcc;

	WVector pos = GetPVS().TestCameraCollision(g_camera.pivot + GOLFDOC()->GetCameraData().m_matrix.za * (dt * 400.0f));
	float ground = Max(GetPVS().GetGroundPoint(pos, true, NULL, true).y, m_baseHeight);

	pos.y = Between(ground + GOLFDOC()->GetCameraData().m_groundOffset, pos.y, m_maxHeight);
	WVector move = pos - g_camera.pivot;

	m_pos += move;
	m_at = GOLFDOC()->GetCameraData().m_matrix.za + m_pos;

	m_unk50 = (m_pos.y - m_baseHeight) * 0.0052083335f;
	m_zoom = m_unk50 - m_startHeightRate;
	m_at.y += tan(GetCameraAngle());

	LookAt(m_pos, m_at);

	m_unk60 = true; m_startHeightRate = GetHeightRate(ground + GOLFDOC()->GetCameraData().m_groundOffset);

	WVector before = g_camera.pivot - GOLFDOC()->GetCameraData().m_matrix.pivot - move;
	WVector after = g_camera.pivot - GOLFDOC()->GetCameraData().m_matrix.pivot;

	if (GOLFDOC()->GameMode() == 0x100 && WDotProduct(after, GOLFDOC()->GetCameraData().m_matrix.za) * WDotProduct(before, GOLFDOC()->GetCameraData().m_matrix.za) < 0.0f && Abs(g_camera.pivot.y - GOLFDOC()->GetCameraData().m_matrix.pivot.y) < 3.2f) {
		m_pos = GOLFDOC()->GetCameraData().m_matrix.pivot;
		m_at = GOLFDOC()->GetCameraData().m_matrix.za + GOLFDOC()->GetCameraData().m_matrix.pivot;
		m_at.y += tan(GOLFDOC()->GetCameraData().m_pitch);

		m_zoom = 0; m_unk50 = m_startHeightRate;

		g_camera.pivot = GOLFDOC()->GetCameraData().m_matrix.pivot;

		if (IsMyTurn(true)) {
			GOLFDOC()->GetUIAlpha() = 0xFF;
			CChatMsg::Instance()->ReplaceBackupUiAlpha();
		}

		GOLFDOC()->GameMode() = 8;
		m_inputDelay = 0.25f;
	} else if (GOLFDOC()->GameMode() == 8) {
		GOLFDOC()->GetUIAlpha() = 0x80;
		CChatMsg::Instance()->ReplaceBackupUiAlpha();
		GOLFDOC()->GameMode() = 0x100;
	}
}

// camera.cpp:844
void CCamera::UpDown(float dt) {
	if (GOLFDOC()->GameMode() == 0x200) {
		float ground = GetPVS().GetGroundPoint(g_camera.pivot, true, NULL, true).y + 16.0f;

		float height = g_camera.pivot.y - ground;
		if (height < 300.0f) {
			dt *= Max(0.2f, height / 300.0f);
		}
		g_camera.pivot.y += dt * 400.0f;

		float limit = m_baseHeight + 2560.0f; if (g_camera.pivot.y < ground) g_camera.pivot.y = ground; else if (g_camera.pivot.y > limit) g_camera.pivot.y = limit;
		return;
	}

	if (GOLFDOC()->GameMode() == 8 && dt > 0.0f) {
		GOLFDOC()->GameMode() = 0x100;
		GOLFDOC()->GetUIAlpha() = 0x80;
		CChatMsg::Instance()->ReplaceBackupUiAlpha();
	}

	m_zoom = Between(0.0f, dt * m_moveAcc + m_zoom, 1.0f - m_startHeightRate);

	if (m_unk60) {
		m_unk50 = m_zoom + m_startHeightRate;
	} else {
		if (GolfBall().m_groundType == 2) {
			m_unk50 = m_startHeightRate + m_zoom;
		} else {
			m_unk50 = GetChangeRate() + m_startHeightRate;
		}

		float dist = (GOLFDOC()->GetCameraData().m_matrix.pivot.y - m_baseHeight + 192.0f) * (1.0f - m_startHeightRate);
		g_camera.pivot = GOLFDOC()->GetCameraData().m_matrix.pivot - GOLFDOC()->GetCameraData().m_matrix.za * (dist * m_zoom);
	}

	g_camera.pivot.y = (GOLFDOC()->GetCameraData().m_matrix.pivot.y - m_baseHeight + 192.0f) * m_unk50 + m_baseHeight;
	g_camera.pivot = GetPVS().TestCameraCollision(g_camera.pivot);
	m_pos = g_camera.pivot;
	m_at = g_camera.pivot + GOLFDOC()->GetCameraData().m_matrix.za;
	m_at.y += tan(GetCameraAngle());
	LookAt(m_pos, m_at);

	if (!m_unk60 && m_unk50 == m_heightRate) {
		GOLFDOC()->GameMode() = 8;
		if (IsMyTurn(true)) {
			GOLFDOC()->GetUIAlpha() = 0xFF;
			CChatMsg::Instance()->ReplaceBackupUiAlpha();
		}
	}
}

// camera.cpp:910
float CCamera::GetCameraAngle(void) {
	float angle = GOLFDOC()->GetCameraData().m_pitch - atan(m_zoom * 0.65f);
	return 0.0f > angle ? angle : 0.0f;
}

// camera.cpp:915
float CCamera::GetChangeRate(void) {
	return m_zoom * m_zoom;
}

// camera.cpp:920
void CCamera::GetRotateAcc(float dt) {
	if (data_00af2adc == m_rotateKey && m_rotateKey) {
		float acc = dt + m_rotateAcc; m_rotateAcc = acc < 0.0f ? 0.0f : (acc > 1.0f ? 1.0f : acc);
	} else {
		m_rotateAcc = 0;
	}
	data_00af2adc = m_rotateKey;
}

// camera.cpp:932
void CCamera::GetMoveAcc(float dt) {
	if (data_00af2ae0 == m_moveKey && m_moveKey) {
		float acc = dt + m_moveAcc; m_moveAcc = acc < 0.0f ? 0.0f : (acc > 1.0f ? 1.0f : acc);
	} else {
		m_moveAcc = 0;
	}
	data_00af2ae0 = m_moveKey;
}

// camera.cpp:944
void CCamera::GetHorizonAcc(float dt) {
	if (data_00af2ae4 == m_horizonKey && m_horizonKey) {
		float acc = dt + m_horizonAcc; m_horizonAcc = acc < 0.0f ? 0.0f : (acc > 1.0f ? 1.0f : acc);
	} else {
		m_horizonAcc = 0;
	}
	data_00af2ae4 = m_horizonKey;
}

// camera.cpp:956
float CCamera::GetHeightRate(float height) {
	float diff = height - m_baseHeight;
	return diff / (192.0f + diff);
}

// camera.cpp:966
void CCamera::FixedCamera(float dt) {
	if (CQuitWindow::Instance()->IsActive() || !CItemWindow::IsClosed() || GalleryMode() || GOLFDOC()->m_dropPending)
		return;

	for (int i = 3; i < 10; i++) {
		if (g_input->GetDown(MakeStr("PAD_%d", i), true) || g_input->GetDown(MakeStr("NUM_%d", i), true)) {
			// "카메라%d" (camera N)
			WPolySoup::sCamera *cam = GOLFDOC()->m_pPolySoup->FindCamera(MakeStr("\xC4\xAB\xB8\xDE\xB6\xF3%d", i));
			if (cam == NULL)
				return;

			m_fixedYaw = 0;
			m_fixedPitch = 0;

			m_pos = cam->m_matrix.pivot;
			m_at = m_pos + cam->m_matrix.za * 10.0f;

			g_camera = cam->m_matrix;
			// The original reads pi / 180 from a named constant (0x9ED8AC), not a __real.
			g_view->SetFOV(cam->m_fov * 0.017453292f);

			GOLFDOC()->GameMode() = 0x8000;
			return;
		}
	}

	if (GOLFDOC()->GameMode() != 0x8000)
		return;

	m_rotateKey = 0;
	if (CMouseCursor::Instance()->GetArea() == 0x17 && g_input->GetButton(LEFT_BUTTON) > 0) {
		m_rotateKey = 1;
		YawFixedCamera(dt);
	}

	if (CMouseCursor::Instance()->GetArea() == 0x18 && g_input->GetButton(LEFT_BUTTON) > 0) {
		m_rotateKey = -1;
		YawFixedCamera(dt * -1.0f);
	}

	// "좌" (left) and "우" (right)
	if (g_input->Get("\xC1\xC2", true) && !g_input->Get("LCONTROL", true) && !g_input->Get("RCONTROL", true) && !g_input->Get("LSHIFT", true) && !g_input->Get("RSHIFT", true))
		RotateEnvironment(dt * 1.0471976f);

	if (g_input->Get("\xBF\xEC", true) && !g_input->Get("LCONTROL", true) && !g_input->Get("RCONTROL", true) && !g_input->Get("LSHIFT", true) && !g_input->Get("RSHIFT", true))
		RotateEnvironment(dt * -1.0471976f);

	GetRotateAcc(dt);

	m_horizonKey = 0;

	if (CMouseCursor::Instance()->GetArea() == 8 || (CMouseCursor::Instance()->GetArea() == 0x19 && g_input->GetButton(LEFT_BUTTON) > 0)) {
		m_horizonKey = 1;
		PitchFixedCamera(dt);
	}

	if (CMouseCursor::Instance()->GetArea() == 9 || (CMouseCursor::Instance()->GetArea() == 0x1a && g_input->GetButton(LEFT_BUTTON) > 0)) {
		m_horizonKey = -1;
		PitchFixedCamera(dt * -1.0f);
	}

	GetHorizonAcc(dt);
}

// camera.cpp:1048
void CCamera::YawFixedCamera(float dt) {
	m_fixedYaw -= dt * m_rotateAcc * 5.0f;
	if (m_fixedYaw > 10.0f) {
		m_fixedYaw = 10.0f;
	} else if (m_fixedYaw < -10.0f) {
		m_fixedYaw = -10.0f;
	} else {
		WVector move = g_camera.xa * (dt * m_rotateAcc * 5.0f);
		m_at -= move; LookAt(m_pos, m_at);
	}
}

// camera.cpp:1066
void CCamera::PitchFixedCamera(float dt) {
	m_fixedPitch += dt * m_horizonAcc * 5.0f;
	if (m_fixedPitch > 5.0f) {
		m_fixedPitch = 5.0f;
	} else if (m_fixedPitch < -5.0f) {
		m_fixedPitch = -5.0f;
	} else {
		WVector move = g_camera.ya * (dt * m_horizonAcc * 5.0f);
		m_at += move; LookAt(m_pos, m_at);
	}
}

// camera.cpp:1084
void CCamera::RotateEnvironment(float angle) {
	GOLFDOC()->GetCameraData().m_yaw = GetBaseAngle(GOLFDOC()->GetCameraData().m_yaw + angle);
	GOLFDOC()->GetCameraData().m_matrix = RotMat(GOLFDOC()->GetCameraData().m_yaw + (IsMyTurn(true) ? 0.0f : GOLFDOC()->GetCameraData().m_yaw2), 1);

	CGolfDoc *doc = GOLFDOC();
	float dist = doc->GetCameraData().m_distance * -1.0f;
	doc->GetCameraData().m_matrix.pivot = GetPVS().TestCameraCollision(WVector(doc->GetCameraData().m_matrix.za.x * dist, doc->GetCameraData().m_height, doc->GetCameraData().m_matrix.za.z * dist) + doc->GetCameraData().m_target);

	m_heightRate = GetHeightRate(GOLFDOC()->GetCameraData().m_matrix.pivot.y);

	if (IsMyTurn(false)) {
		m_yawChanged = true;
		SendMsg(this, "Player", 0x1a1, (intptr_t)&angle, 0, 0, 0);
		SendMsg(this, "Spot", 0x1a1, (intptr_t)&angle, 0, 0, 0);
		SendMsg(this, "Screen", 0x1a1, (intptr_t)&angle, 0, 0, 0);
		SendMsg(this, "Grid", 0x1a1, (intptr_t)&angle, 0, 0, 0);
		SendMsg(this, "Caddie", 0x1a1, (intptr_t)&angle, 0, 0, 0);
		SendMsg(this, "GuideLine", 0x1a1, 0, 0, 0, 0);
	}
}

// camera.cpp:1109
void CCamera::RotateForGallery(float angle) {
	if (GalleryMode()) {
		WMatrix mat = g_camera * RotMat(angle, 1);
		mat = g_camera;
		mat.pivot.x = mat.pivot.y = mat.pivot.z = 0.0f;
		mat = mat * RotMat(angle, 1);

		RotateEnvironment(angle);

		if (GOLFDOC()->GameMode() & 0x8) {
			mat.pivot = GOLFDOC()->GetCameraData().m_matrix.pivot;
		} else {
			CGolfDoc *doc = GOLFDOC();
			mat.pivot = (g_camera.pivot - doc->GetCameraData().m_target) * RotMat(angle, 1) + doc->GetCameraData().m_target;
			mat.pivot = GetPVS().TestCameraCollision(mat.pivot);
			if (GOLFDOC()->GameMode() & 0x200)
				mat.pivot.y = g_camera.pivot.y;
		}

		g_camera = mat;

		m_pos = g_camera.pivot;
		m_at = g_camera.za + g_camera.pivot;
	}
}
