// Original: d:\build\custom\temp\code\client\projectg\camera.h
#ifndef PG_CLIENT_PROJECTG_CAMERA_H
#define PG_CLIENT_PROJECTG_CAMERA_H

#include "defines.h"
#include "padding.h"
#include "actor.h"
#include "RTTI.h"
#include "wmath.h"

// Layout not fully reconstructed yet: only the members the unit's functions
// use. Roles are read from OnInit, the Get*Acc ramps and the fixed-camera
// moves.
class CCamera : public IActor {
	LAYOUT_CHECK_FRIEND;
	// The factory function of the class registration (camera.cpp:33).
	friend IObject *CCameraMakeInstance(void);
public:
	// camera.h:11
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }
	// camera.h:9
	virtual ~CCamera(void) {}

	virtual void OnInit(void);
	virtual void OnProcess(float dt);

	// Spot-camera preset. Its implicit constructor is emitted out of line
	// (??0sSpotCam@CCamera@@QAE@XZ, `mov eax, ecx; ret`), which takes a
	// member with a user-declared constructor (WVector) and an array of
	// sSpotCam in CCamera (SetSpotCamera: start, target, per-second step).
	struct sSpotCam {
		WVector m_pos;  // 0x0
		float m_pitch;  // 0xc, the tangent raises the look-at point
	};

	static const WRTTI m_RTTI;

protected:
	CCamera(void);

	virtual void HandleMsg(const MsgObject &msg);

	void LookAt(WVector &pos, WVector &at);
	float GetCameraAngle(void);
	float GetChangeRate(void);
	void GetRotateAcc(float dt);
	void GetMoveAcc(float dt);
	void GetHorizonAcc(float dt);
	float GetHeightRate(float height);
	void YawFixedCamera(float dt);
	void PitchFixedCamera(float dt);
	void SetSpotCamera(bool);
	void ProcessMousePick(float dt);
	void ProcessToSpot(float dt);
	void Translate(float dt);
	void UpDown(float dt);
	void Rotate(float angle);
	void RotateEnvironment(float angle);
	void RotateForGallery(float angle);
	void FixedCamera(float dt);
	void ProcessInput(float dt);
	void ProcessForcedMove(float dt);

	int m_rotateKey;            // 0x18, rotate input of this frame (0 = none)
	float m_forcedMoveTotal;    // 0x1c, length of the forced move (ProcessForcedMove)
	float m_forcedMoveLeft;     // 0x20, what remains of it (ProcessForcedMove)
	int m_moveKey;              // 0x24, move input of this frame (0 = none)
	int m_horizonKey;           // 0x28, horizon input of this frame (0 = none)
	float m_rotateAcc;          // 0x2c, rotate ramp 0..1 (GetRotateAcc)
	float m_moveAcc;            // 0x30, move ramp 0..1 (GetMoveAcc)
	float m_horizonAcc;         // 0x34, horizon ramp 0..1 (GetHorizonAcc)
	WVector m_pos;              // 0x38, eye of the camera matrix
	WVector m_at;               // 0x44, look-at point of the camera matrix
	float m_unk50;              // 0x50, cleared by OnInit
	float m_zoom;               // 0x54, squared by GetChangeRate
	float m_maxHeight;          // 0x58, m_baseHeight + 192 (HandleMsg 0xEF)
	float m_baseHeight;         // 0x5c, origin of GetHeightRate
	bool m_unk60;               // 0x60, cleared by OnInit
	PADDING(0x61, 0x64);
	float m_heightRate;         // 0x64, GetHeightRate of the camera's eye (RotateEnvironment)
	float m_startHeightRate;    // 0x68, GetHeightRate of the doc camera's eye when it is taken over (HandleMsg 0xEE)
	bool m_enabled;             // 0x6c, OnProcess runs only while set (messages 0x19a/0x19b set/clear it, 0x19c reads it)
	PADDING(0x6d, 0x70);
	float m_inputDelay;         // 0x70, seconds left before OnProcess takes input again
	bool m_camMode;             // 0x74, the middle button toggles it (CMouseCursor::SetCamMode)
	PADDING(0x75, 0x78);
	float m_fixedYaw;           // 0x78, fixed-camera yaw, -10..10
	float m_fixedPitch;         // 0x7c, fixed-camera pitch, -5..5
	float m_yawSendTime;        // 0x80, seconds since the yaw was last sent (OnProcess, every 2 s)
	bool m_yawChanged;          // 0x84, the yaw changed since it was last sent (RotateEnvironment)
	PADDING(0x85, 0x88);
	float m_baseYaw;            // 0x88, the doc camera's yaw when another player's turn starts (HandleMsg 0x2718)
	float m_yaw;                // 0x8c, the doc camera's yaw (HandleMsg 0x2718, 0xF2)
	// [0] where the spot move starts, [1] where it ends, [2] its step per second.
	sSpotCam m_spotCam[3];      // 0x90
	float m_spotTime;           // 0xc0, seconds since the spot move started (ProcessToSpot, up to 1.2)
	bool m_spotMoving;          // 0xc4, the camera is moving to the spot (SetSpotCamera(false) .. ProcessToSpot)
	PADDING(0xc5, 0xc8);
};

CHECK_SIZE(CCamera, 0xc8);
BEGIN_LAYOUT_CHECK(CCamera)
	CHECK_FIELD(CCamera, m_rotateKey, 0x18);
	CHECK_FIELD(CCamera, m_rotateAcc, 0x2c);
	CHECK_FIELD(CCamera, m_pos, 0x38);
	CHECK_FIELD(CCamera, m_at, 0x44);
	CHECK_FIELD(CCamera, m_zoom, 0x54);
	CHECK_FIELD(CCamera, m_baseHeight, 0x5c);
	CHECK_FIELD(CCamera, m_heightRate, 0x64);
	CHECK_FIELD(CCamera, m_fixedYaw, 0x78);
	CHECK_FIELD(CCamera, m_maxHeight, 0x58);
	CHECK_FIELD(CCamera, m_startHeightRate, 0x68);
	CHECK_FIELD(CCamera, m_baseYaw, 0x88);
	CHECK_FIELD(CCamera, m_yaw, 0x8c);
	CHECK_FIELD(CCamera, m_spotCam, 0x90);
	CHECK_FIELD(CCamera, m_spotTime, 0xc0);
	CHECK_FIELD(CCamera, m_spotMoving, 0xc4);
END_LAYOUT_CHECK

#endif // PG_CLIENT_PROJECTG_CAMERA_H
