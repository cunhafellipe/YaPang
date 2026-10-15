// Original: d:\build\custom\temp\code\client\projectg\realmyroom_avatar.cpp
#include "NetworkSystem.h"
#include "RealMyRoom_Avatar.h"

#include <stdlib.h>
#include <string.h>
#include <algorithm>

#include "Packet.h"
#include "partsinfo.h"
#include "texturepart.h"
#include "wbone.h"
#include "wpuppet.h"
#include <math.h>
#include "shared/Localize.h"
#include "wview.h"
#include "SharedDoc.h"
#include "RealMyRoomMain.h"
#include "quadtree.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "wutil.h"
#include "commonutil.h"
#include "ClientSetting.h"
#include "NetResourceManager.h"
#include "balloontext.h"
#include "wresrcmng.h"
#include "fremoticon.h"
#include "woverlay.h"
#include "ProjectG.h"
#include "GolfDoc.h"
#include "SoundManager.h"
#include "fxspray.h"

const char *GetStrConst(const char *pSrc, char *pDest);
const char *GetCharsTillSpace(const char *pSrc, char *pDest);

// Nameless globals of the default light (0xAA5274 is probably a LightSet:
// nearone 0xAA5278, diffuse 0xAA5284).
extern LightSet g_lightset;
extern WMatrix g_camera;
float Random(float min, float max, int seed); // commonutil.cpp:1106
extern WVector data_00aa5278;
extern unsigned long data_00aa5284;

// The face texture of each character type (4 facials per character).
extern "C" const char *data_a88020[][4];

// wmath.h's header-static constants as this object holds them (PI, 2 PI,
// FLT_MAX, 1e-5 at 0x9FA510).
extern "C" const float data_9fa510[4];

// ParsePuppetScript's buffers (nameless data, one object in the original):
// [0] (0xAF5610) the sound name / texture part name, [1] (0xAF5650) the
// "*command(" being parsed.
static char data_af5610[2][64];

// realmyroom_avatar.h:93
const WVector &CRealMyRoom_Avatar::GetPosition() const {
	return m_position;
}

// realmyroom_avatar.h:125
void CRealMyRoom_Avatar::SetHeadIcon(int headIcon) {
	m_headIcon = headIcon;
}

// realmyroom_avatar.cpp:76
CRealMyRoom_Avatar::CRealMyRoom_Avatar(const sSlotInfo &slotInfo, const sCharacterInfo &charInfo, CRealMyRoomMain *pMain)
	: m_boneSet0(true), m_boneSet1(true) {
	strcpy(m_idName, slotInfo.m_nickname);
	strcpy(m_nickName, slotInfo.m_nickname);
	m_moveFlag = false;
	m_followNickName[0] = '\0';

	if (IsLocalContent((localContentType_t)0x10)) {
		if (slotInfo.m_valueCD == 1) {
			strcpy(m_slotBlockD1, slotInfo.m_blockD1);
			m_hasSlotBlockD1 = true;
		}
		else {
			memset(m_slotBlockD1, 0, sizeof(m_slotBlockD1));
			m_hasSlotBlockD1 = false;
		}
	}

	m_guid = slotInfo.m_uid;
	m_isAdministrator = (slotInfo.m_identity >> 2) & 1;
	m_userFlag = slotInfo.m_attributeBits.userFlag;
	m_slotFlag55 = slotInfo.m_flags55.flag0;
	m_slotFlag57 = slotInfo.m_flags57.flag0;
	m_charTypeId = slotInfo.m_characterTypeId;
	m_state = slotInfo.m_avatarState;
	m_prevState = m_state;
	m_action = slotInfo.m_avatarAction;

	m_pMotionData = NULL;
	m_pPrevMotionData = NULL;
	m_blendTime = 0.0f;
	m_blendDuration = 0.0f;
	m_totalTime = 0.0f;
	m_facialTime = 0.0f;
	m_groundNormal = WVector::UNIT_POS_Y;
	m_isTargetPositionChanged = false;
	m_lastMotionSendTime = 0;

	m_startPosition = WVector::UNIT_POS_Y;

	m_pMain = pMain;
	m_hasPendingTargetAngle = false;

	m_startPosition = pMain->m_cameraDropOrigin;

	m_burdenChanged = false;
	m_pPetFrame = NULL;
	m_charInfo = charInfo;

	m_headIcon = -1;
	m_pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(slotInfo.m_emblemName);

	m_pBalloonText = NULL;
	m_balloonOffset[0] = Random(0.8f, 1.0f, -1);
	m_balloonOffset[1] = Random(0.8f, 1.0f, -1);
	m_balloonOffset[2] = Random(0.8f, 1.0f, -1);
	m_balloonTime = 1.0f;

	m_moveDelta.Reset();
	m_angle = RadianNormalize2PI(slotInfo.m_angle);
	m_targetAngle = m_angle;
	SetState(m_state);
	SetAction(m_action);
	if (slotInfo.m_tradeTitle) {
		strcpy(m_tradeTitle, slotInfo.m_tradeTitle);
	}
	else {
		m_tradeTitle[0] = '\0';
	}

	m_light.type = LightSet::W_LIGHT_DIRECTIONAL;
	m_light.ambient = ModifyColor(g_lightset.ambient, 1.0f);
	m_light.nearone = g_lightset.nearone;

	m_fxBox.Init(g_resrcmng);
	m_fxBox.SetCamera(&g_camera);
	m_fxBox.m_frameTime = 33.333332f;
}

// realmyroom_avatar.cpp:170
CRealMyRoom_Avatar::~CRealMyRoom_Avatar() {
	std::list<sTexturePart *>::iterator it;
	for (it = m_pendingTexParts.begin(); it != m_pendingTexParts.end(); ++it) {
		delete *it;
	}
	m_pendingTexParts.clear();

	m_fxBox.ClearAll(false);
}

// realmyroom_avatar.cpp:194
bool CRealMyRoom_Avatar::IsTargetPositionChanged(bool changed) {
	bool wasChanged = m_isTargetPositionChanged;
	m_isTargetPositionChanged = changed;
	return wasChanged;
}

// realmyroom_avatar.cpp:200
void CRealMyRoom_Avatar::SetHide(bool hide) {
	m_hide = hide;

	if (m_pMain->m_pRoomFx != 0) {
		CFx::Instance()->SetActive(hide);
	}
}

// realmyroom_avatar.cpp:208
bool CRealMyRoom_Avatar::IsClickable(float x, float y) const {
	if (!g_pFresh->IsVisible()) {
		return false;
	}

	if (m_pPetFrame == NULL || m_pPetFrame->GetCurAniPet() == NULL) {
		return false;
	}

	if (m_alpha < 0.1f) {
		return false;
	}

	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();
	if (m_state & 0x80) {
		WVector head;
		head = m_pPetFrame->GetBonePet()->FindBone("Bip01 HeadNub")->GetMatrix().pivot;

		WVector screen = g_view->Projection(head);

		if (!(screen.z <= 0.0f) && !(1.0f <= screen.z)) {
			int screenX = (int)floorf(screen.x);
			int screenY = (int)floorf(screen.y) - 48;

			if (m_state & 0x80) {
				if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar != NULL) {
					if ((WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar->m_position - m_position).Magnitude() > 19.2f) {
						return false;
					}
				}

				int width = (int)pGDI->GetTextExtend(m_tradeTitle) + 10;
				if (width < 100) {
					width = 100;
				}

				int left = screenX - width / 2;

				int top = screenY - 8;
				if (left < x && x < left + width && top < y && y < top + 29) {
					return true;
				}
			}
		}
	}

	return false;
}

// realmyroom_avatar.cpp:281
CRealMyRoom_Avatar *CRealMyRoom_Avatar::GetFollowingAvatar() const {
	if (m_followNickName[0] == '\0') {
		return NULL;
	}

	return WSingleton<CRealMyRoom_AvatarManager>::Instance()->FindAvatarByNickName(m_followNickName);
}

// realmyroom_avatar.cpp:292
bool CRealMyRoom_Avatar::GetBoneScreenCoord(const char *boneName, int *pX, int *pY) const {
	if (m_pPetFrame == NULL || m_pPetFrame->GetCurAniPet() == NULL) {
		return false;
	}

	WBone *pBone = m_pPetFrame->GetBonePet()->FindBone(boneName);
	if (pBone == NULL) {
		return false;
	}

	WVector screen = g_view->Projection(pBone->GetMatrix().pivot);
	*pX = (int)floor(screen.x);
	*pY = (int)floor(screen.y);

	return true;
}

// realmyroom_avatar.cpp:318
void CRealMyRoom_Avatar::ChangeCharacterInfo() {
	m_fxBox.ClearAll(false);

	ParseBoundingBoxScript();
}

// realmyroom_avatar.cpp:337
void CRealMyRoom_Avatar::SetCharactorInfo(const sCharacterInfo &charInfo) {
	Stop();

	m_pPetFrame = NULL; m_charInfo = charInfo;
	m_charTypeId = charInfo.typeId;

	m_fxBox.ClearAll(false);
}

// realmyroom_avatar.cpp:351
void CRealMyRoom_Avatar::SetPosition(const WVector &position) {
	m_position = position;
	m_targetPosition = position; m_isTargetPositionChanged = true;
}

// realmyroom_avatar.cpp:359
void CRealMyRoom_Avatar::SetStartPosition(const WVector &position) {
	m_startPosition = position;
}

// realmyroom_avatar.cpp:364
void CRealMyRoom_Avatar::SetTargetPosition(const WVector &position) {
	m_targetPosition = position;
	m_isTargetPositionChanged = true;
}

// realmyroom_avatar.cpp:370
void CRealMyRoom_Avatar::SetAngle(float angle) {
	angle = RadianNormalize2PI(angle);
	m_angle = angle;
	m_targetAngle = angle;
}

// realmyroom_avatar.cpp:377
void CRealMyRoom_Avatar::SetTargetAngle(float angle) {
	m_targetAngle = RadianNormalize2PI(angle);
}

// realmyroom_avatar.cpp:383
void CRealMyRoom_Avatar::SetPendingTargetAngle(float angle) {
	m_pendingTargetAngle = RadianNormalize2PI(angle);
	m_hasPendingTargetAngle = true;
}

// realmyroom_avatar.cpp:389
void CRealMyRoom_Avatar::SetState(unsigned long state) {
	if (m_state == state) {
		return;
	}

	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)
		&& this == WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar) {
		WSendPacket packet(GCT_SYNC_ACTIVITY);
		packet.Encode1(8);
		packet.Encode4(state);
		packet.Send(SEND_TO_DEFAULT);
	}

	m_state = state; if (state & 0x7E) {
		Stop();
	}
}

// realmyroom_avatar.cpp:412
void CRealMyRoom_Avatar::SetAction(int action) {
	if (m_action == action) {
		return;
	}

	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)
		&& this == WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar) {
		WSendPacket packet(GCT_SYNC_ACTIVITY);
		packet.Encode1(5);
		packet.Encode4(action);
		packet.Send(SEND_TO_DEFAULT);
	}

	m_action = action;
}

// realmyroom_avatar.cpp:428
void CRealMyRoom_Avatar::SendMove(const WVector &delta, bool force) {
	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)
		&& this == WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar) {
		m_moveDelta += delta;

		if (m_moveDelta.Magnitude() >= 16.5f || force) {
			m_moveDelta.Reset();
		}
	}
}

// realmyroom_avatar.cpp:451
void CRealMyRoom_Avatar::Stop() {
	if (this == WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar) {
		if (!WisEqual(m_targetPosition, m_position, 0.00001f)) {
			SendMove(WVector(0.0f, 0.0f, 0.0f), true);

			m_targetPosition = m_position;
			m_isTargetPositionChanged = true;
		}

		m_moveFlag = false;
		m_followNickName[0] = '\0';
	}
}

// realmyroom_avatar.cpp:468
void CRealMyRoom_Avatar::Follow(const char *nickName) {
	if (nickName) {
		strcpy(m_followNickName, nickName);
	}
	else {
		m_followNickName[0] = '\0';
	}
}

// realmyroom_avatar.cpp:479
void CRealMyRoom_Avatar::ChangeMotion(int motion, const char *name, float blendTime) {
	const char *motionName;

	m_motion = motion;
	if (m_pPetFrame == NULL) {
		return;
	}

	switch (motion) {
	case 0:
		motionName = "\xb1\xe2\xba\xbb\xc0\xda\xbc\xbc";
		break;
	case 2:
		motionName = "chat_\xbe\xc9\xb1\xe2";
		break;
	case 3:
		motionName = "chat_\xb4\xaf\xb1\xe2";
		break;
	case 1:
		motionName = "chat_\xb4\xde\xb8\xae\xb1\xe2";
		break;
	case 4:
		motionName = name;
		if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)
			&& this == WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar) {
			unsigned long now = timeGetTime();

			if (now - m_lastMotionSendTime <= 1000) {
				return;
			}

			m_lastMotionSendTime = now;
			WSendPacket packet(GCT_SYNC_ACTIVITY);
			packet.Encode1(7);
			packet.EncodeStr(name);
			packet.Send(SEND_TO_DEFAULT);
		}
		break;
	default:
		motionName = "\xb1\xe2\xba\xbb\xc0\xda\xbc\xbc";
		break;
	}

	w_motion_data *pData = m_pPetFrame->FindMotionData(motionName, true, NULL);
	if (pData == NULL) {
		return;
	}

	if (blendTime > 0.0f && m_pMotionData != NULL) {
		m_boneSet1.CopyFrom(&m_boneSet0);
		m_pPrevMotionData = m_pMotionData;
		m_prevMotion = m_motion;
		m_pMotionData = pData;
		m_motion = motion;
		m_blendTime = 0.0f;
		m_blendDuration = blendTime;
	} else {
		m_pPrevMotionData = NULL; m_blendTime = 0.0f; m_pMotionData = pData; m_boneSet0.CopyFrom(m_pPetFrame->GetCurAniPet()->GetBones(NULL, pData->s, 0, NULL, false));
	}

	if (m_blendTime == 0.0f) {
		// the props of the character are shown again by the custom motion
		bool hideProps = motion != 4;
		WBone *pBone;

		switch (m_charTypeId & 0x3FFFFFF) {
		case 4:
			pBone = m_pPetFrame->GetBonePet()->FindBone("D_Ball");
			if (pBone) {
				pBone->SetInvisible(false, hideProps);
			}
			pBone = m_pPetFrame->GetBonePet()->FindBone("D_Racket");
			if (pBone) {
				pBone->SetInvisible(false, hideProps);
			}
			break;
		case 5:
			pBone = m_pPetFrame->GetBonePet()->FindBone("E_Cannon");
			if (pBone) {
				pBone->SetInvisible(false, hideProps);
			}
			pBone = m_pPetFrame->GetBonePet()->FindBone("E_Telescope");
			if (pBone) {
				pBone->SetInvisible(false, hideProps);
			}
			break;
		case 6:
			pBone = m_pPetFrame->GetBonePet()->FindBone("G_Bird");
			if (pBone) {
				pBone->SetInvisible(false, hideProps);
			}
			break;
		case 7:
			pBone = m_pPetFrame->GetBonePet()->FindBone("H_Sword01");
			if (pBone) {
				pBone->SetInvisible(false, hideProps);
			}
			pBone = m_pPetFrame->GetBonePet()->FindBone("H_Sword02");
			if (pBone) {
				pBone->SetInvisible(false, hideProps);
			}
			pBone = m_pPetFrame->GetBonePet()->FindBone("H_karen");
			if (pBone) {
				pBone->SetInvisible(false, hideProps);
			}
			break;
		}

		if ((m_charTypeId & 0x3FFFFFF) < 10) {
			SetPendingTexPart("face", data_a88020[m_charTypeId & 0x3FFFFFF][3]);
		}
	}

	ProcessMotion(0.0f);
}

// realmyroom_avatar.cpp:626
void CRealMyRoom_Avatar::ProcessMotion(float elapsed) {
	if (m_pPetFrame == NULL || m_pPetFrame->GetCurAniPet() == NULL) {
		return;
	}

	if (m_motion != 0 && m_motion != 2 && m_motion != 3 && m_motion != 1) {
		if (m_pMotionData->e - m_pMotionData->s < m_blendTime) {
			ChangeMotion(0, NULL, 0.2f);
		}
	}

	if (m_pPrevMotionData != NULL) {
		float t = m_blendTime / m_blendDuration;

		if (t > 1.0f) {
			m_pPrevMotionData = NULL;
		}
		else {
			float time = m_blendTime;
			float length = m_pMotionData->e - m_pMotionData->s;
			if (length < m_blendTime) {
				time = length;
			}

			m_boneSet0.Blend(&m_boneSet1, m_pPetFrame->GetCurAniPet()->GetBones(m_pMotionData->name, time, 0, NULL, false), t);
		}
	}
	else {
		float length = m_pMotionData->e - m_pMotionData->s;

		if (m_blendTime > length) {
			if (length > 0.01f) {
				float loops = floorf(m_blendTime / length);
				m_blendTime -= loops * length;
			}
			else {
				m_blendTime = 0.0f;
			}
		}

		m_boneSet0.CopyFrom(m_pPetFrame->GetCurAniPet()->GetBones(NULL, m_pMotionData->s + m_blendTime, 0, NULL, false));
	}

	if (m_blendTime == 0.0f) {
		ScriptProcess(m_pMotionData, m_pMotionData->s, m_pMotionData->s, true);
	}
	else {
		ScriptProcess(m_pMotionData, m_pMotionData->s + m_blendTime, m_pMotionData->s + m_blendTime + elapsed, false);
	}
}

// realmyroom_avatar.cpp:697
// Loads the effect file name (a ".spr" spray, anything else a sequence) into
// the avatar's effect box, attached where attacher says; NULL when the name
// is too short or the file does not load.
void *CRealMyRoom_Avatar::AttachFx(const Attacher &attacher, const char *name, bool bAlive, defines_t *pDefines) {
	int len = strlen(name);

	if (len < 5)
		return NULL;

	if (strcmpi(name + len - 3, "spr") == 0) {
		CFxSpray *pSpray = m_fxBox.LoadSpray(name, pDefines);

		if (pSpray == NULL)
			return NULL;

		pSpray->m_bAlive = bAlive; pSpray->m_pAttacher = &attacher;
		pSpray->m_fAttachLimitA = attacher.m_fLimitA;
		pSpray->m_fAttachLimitB = attacher.m_fLimitB;
		m_fxBox.AddSpray(pSpray);
		return pSpray;
	}
	else {
		CFxSequence *pSequence = m_fxBox.LoadSequence(name, true, pDefines);

		if (pSequence == NULL)
			return NULL;

		pSequence->m_bAlive = bAlive; pSequence->m_pAttacher = &attacher;
		pSequence->m_fDistLimitA = attacher.m_fLimitA;
		pSequence->m_fDistLimitB = attacher.m_fLimitB;
		m_fxBox.AddSequence(pSequence);
		return pSequence;
	}
}

// realmyroom_avatar.cpp:755
// A one-step SlideMove (avatar_avatar.cpp:861 has the full one): moves pos by
// velocity against the PVS with box, the end position to out. True when the
// box is stuck or the move was clipped.
static bool SlideMove(const WVector &velocity, const WVector &pos, const Waabb &box, WVector &out) {
	WVector start;
	WVector dir;
	WPlane planes[4];
	trace_s trace;
	int numPlanes;
	int bump;
	int i;

	if (WAbsBySign(velocity.SquareMagnitude()) < data_9fa510[3]) {
		out = pos;
		return false;
	}

	start = pos;
	dir = velocity;
	numPlanes = 0;

	for (bump = 0; bump < 1; bump++) {
		GetPVS().TraceOBB_New(start, start + dir, box, &trace);

		if (trace.m_flag) {
			out = start;
			return true;
		}

		if (trace.m_fraction > 0.0f) {
			out = trace.m_endPos;
			numPlanes = 0;
		}

		if (trace.m_fraction == 1.0f) {
			break;
		}

		if (numPlanes >= 3) {
			return true;
		}

		for (i = 0; i < numPlanes; i++) {
			if (WDotProduct(trace.m_plane.normal, planes[i].normal) > 0.99f) {
				dir += trace.m_plane.normal;
				break;
			}
		}
		if (i < numPlanes) {
			continue;
		}

		planes[numPlanes].normal = trace.m_plane.normal;
		planes[numPlanes].dis = trace.m_plane.dis;
		numPlanes++;
	}

	return bump > 0;
}

// realmyroom_avatar.cpp:875
// The rotation that turns the world's up axis onto a ground normal (original
// sub_0057fe60), called by Process.
static void MakeUpRotation(WMatrix &mat, const WVector &up) {
	if (WisEqual(up, WVector::UNIT_POS_Y, 1e-5f)) {
		mat.Reset();
	}
	else {
		WVector axis;
		axis = WCrossProduct(up, WVector::UNIT_POS_Y);
		axis.Normalize();

		float angle = acosf(WDotProduct(up, WVector::UNIT_POS_Y));
		mat = RotMat(axis, angle);
	}
}

// realmyroom_avatar.cpp:895
void CRealMyRoom_Avatar::ProcessChangeState() {
	m_prevState = m_state;
}

// realmyroom_avatar.cpp:914
void CRealMyRoom_Avatar::Process(float elapsed) {
	if (m_burdenChanged) {
		m_partTidList = m_burden.m_partTidList;

		m_burdenChanged = false; m_pPetFrame = m_burden.m_pPetFrame;
		m_pPetFrame->CalcBVolume();
		m_pPetFrame->UpdateLightSource(&g_lightset, false, NULL);

		ParseBoundingBoxScript();

		switch (m_action) {
		case 0:
			ChangeMotion(0, NULL, 0.0f);
			break;
		case 1:
			ChangeMotion(2, NULL, 0.0f);
			break;
		case 2:
			ChangeMotion(3, NULL, 0.0f);
			break;
		default:
			ChangeMotion(0, NULL, 0.0f);
			break;
		}
	}

	if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar == this && (WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().flagBlock & 8)) {
		m_headIcon = 3;
	}
	else {
		m_headIcon = -1;
	}

	ProcessChangeState(); ProcessMotion(elapsed);

	if (m_followNickName[0] != '\0') {
		CRealMyRoom_Avatar *pTarget = WSingleton<CRealMyRoom_AvatarManager>::Instance()->FindAvatarByNickName(m_followNickName);
		if (pTarget) {
			WVector delta;
			delta = pTarget->m_position - m_position;
			float dist = delta.Magnitude();
			if (dist > 64.0f) {
				m_followNickName[0] = '\0';
				Stop();
			}
			else if (dist > 12.8f && dist > 0.0f) {
				delta.Normalize();
				delta *= dist - 12.8f;
				SetTargetPosition(delta + m_position);
				SetAction(0);
			}
		}
		else {
			Stop();
			m_moveFlag = false;
			m_followNickName[0] = '\0';
		}
	}

	WVector move(0.0f, 0.0f, 0.0f); WVector dir;
	dir = m_targetPosition - m_position;
	if (!(Wabs(dir.x) < data_9fa510[3]) || !(Wabs(dir.z) < data_9fa510[3])) {
		dir.y = 0.0f; dir.Normalize();

		WVector velocity;
		velocity = dir * 16.5f * elapsed;

		WVector toTarget;
		toTarget = WVector(m_targetPosition.x - m_position.x, 0.0f, m_targetPosition.z - m_position.z);
		if (velocity.Magnitude() > toTarget.Magnitude()) { // original: fxch fxch fcompp, test ah,5 jp
			velocity = toTarget;
		}

		WVector newPos;
		if (this == WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar && m_hide) { // :1010/:1011
			Waabb box(WVector(-1.0f, -40.0f, -1.0f), WVector(1.0f, 8.0f, 1.0f));
			SlideMove(velocity, m_position, box, newPos);
		}
		else {
			newPos = m_position + velocity;
		}

		move = newPos - m_position;
		move.y = 0.0f;

		if (move.Magnitude() > 0.01f) {
			int hit;
			GetPVS().GetGroundPoint(newPos, true, &hit, true);
			if (hit >= 0) {
				m_position = newPos;
			}
			else if (this == WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar) {
				move = WVector(0.0f, 0.0f, 0.0f);
			}
		}
		else {
			move = WVector(0.0f, 0.0f, 0.0f);
		}
	}

	SendMove(move, false);

	float moveSq = move.x * move.x + move.z * move.z;
	if (sqrtf(moveSq) > 0.01f) {
		if (m_motion != 1) {
			ChangeMotion(1, NULL, 0.2f);
		}
	}
	else {
		if (this == WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar) {
			if (m_moveDelta.x != 0.0f || m_moveDelta.z != 0.0f) {
				SendMove(WVector::ZERO, true);
			}
			m_targetPosition = m_position;
		}

		if (m_motion != 4) {
			switch (m_action) {
			case 0:
				if (m_motion != 0) {
					ChangeMotion(0, NULL, 0.2f);
				}
				break;
			case 1:
				if (m_motion != 2) {
					ChangeMotion(2, NULL, 0.2f);
				}
				break;
			case 2:
				if (m_motion != 3) {
					ChangeMotion(3, NULL, 0.3f);
				}
				break;
			}
		}
	}

	if (moveSq > 0.001f) {
		m_targetAngle = (float)atan2(dir.z, dir.x) + 1.5707964f;
	}
	else if (this != WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar) {
		if (m_hasPendingTargetAngle) {
			float angle = m_pendingTargetAngle;
			m_targetAngle = RadianNormalize2PI(angle);
		}
	}

	float turn = m_targetAngle - m_angle;
	turn = RadianNormalize2PI(turn); if (turn > data_9fa510[0]) turn -= data_9fa510[1];
	if (turn > 0.0f && m_targetAngle < m_angle) {
		m_angle -= data_9fa510[1];
	}
	else if (turn < 0.0f && m_targetAngle > m_angle) {
		m_angle += data_9fa510[1];
	}

	if (fabs(turn) > 0.001f) {
		m_angle += elapsed * turn * 10.0f;
		if ((turn > 0.0f && m_angle > m_targetAngle) || (turn < 0.0f && m_angle < m_targetAngle)) {
			m_angle = m_targetAngle;
		}
	}

	m_position = GetPVS().GetGroundPoint(m_position, false, NULL, false);

	WVector toCamera;
	toCamera = g_view->camera.pivot - m_position;

	float cameraDist = sqrtf(toCamera.x * toCamera.x + toCamera.z * toCamera.z);
	if (cameraDist > 192.0f) {
		m_alpha = 0.0f;
	}
	else if (cameraDist > 160.0f) {
		m_alpha = 1.0f - (cameraDist - 160.0f) * 0.03125f;
	}
	else {
		bool faded = false;
		if (IsLocalContent((localContentType_t)10)) {
			CSharedDoc *pDoc = WSingleton<CSharedDoc>::Instance();
			int guid = m_guid;
			const sBriefUserInfo &info = (*pDoc->m_userMap.find(guid)).second;

			if (info.identityBits.administrator) {
				if (!info.invisible) {
					faded = true;
				}
			}
		}
		if (faded) {
			m_alpha = 0.1f;
		}
		else {
			m_alpha = 1.0f;
		}
	}

	WVector up;
	if (m_motion == 3) {
		up = GetPVS().GetMeshNormal(m_position);
	}
	else {
		up = WVector::UNIT_POS_Y;
	}

	WVector axis = WCrossProduct(m_groundNormal, up);
	axis.Normalize();

	if (!(WAbsBySign(axis.SquareMagnitude()) < data_9fa510[3])) { // here the store + test-byte form, not Wabs
		if (acos(WDotProduct(up, m_groundNormal)) < data_9fa510[1] * elapsed) {
			m_groundNormal = up;
		}
		else {
			WMatrix rot = RotMat(axis, elapsed * -6.2831855f);
			m_groundNormal = RotVec(m_groundNormal, rot);
		}
	}

	WMatrix mat; MakeUpRotation(mat, m_groundNormal);

	mat = RotMat(m_angle, 1) * mat;

	mat.pivot = m_position;

	if (m_pPetFrame) {
		m_pPetFrame->ApplyBones(&m_boneSet0, mat);
		m_pPetFrame->Transform(g_view, 1.0f, 0);
		m_pPetFrame->UpdateBound(true);
		m_pPetFrame->SetAlpha((unsigned char)(m_alpha * 255.0f));
	}

	if (m_facialTime > 0.0f) {
		m_facialTime -= elapsed;
		if (m_facialTime < 0.0f) {
			m_facialTime = 0.0f;
			SetPendingTexPart("face", data_a88020[m_charTypeId & 0x3FFFFFF][3]);
		}
	}

	m_fxBox.ProcessAttacher(elapsed * 1000.0f);
	m_fxBox.Process(elapsed * 1000.0f);

	m_blendTime += elapsed;
	m_totalTime += elapsed;
}

// realmyroom_avatar.cpp:1260
void CRealMyRoom_Avatar::Render() {
	if (!m_hide) {
		return;
	}

	if (IsLocalContent((localContentType_t)10)) {
		if (this != WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar && m_isAdministrator) {
			CSharedDoc *pDoc = WSingleton<CSharedDoc>::Instance();
			int guid = m_guid;
			std::map<unsigned long, sBriefUserInfo>::iterator it = pDoc->m_userMap.find(guid);

			if (!sBriefUserInfo((*it).second).invisible) {
				if (guid != WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().guid) {
					return;
				}
			}
		}
	}

	WVector pos;
	if (m_pPetFrame == NULL || m_pPetFrame->GetCurAniPet() == NULL) {
		pos = GetPVS().GetGroundPoint(m_position, false, NULL, false);
	}
	else {
		pos = m_pPetFrame->GetBonePet()->FindBone("Bip01 Pelvis")->GetMatrix().pivot;
		pos = GetPVS().GetGroundPoint(pos, false, NULL, false);
	}

	if (COption::Instance()->vIsShadowEnabled()) {
		GetPVS().DrawDecal(m_pMain->m_shadowTexture, pos, WVector(2.9f, 0.0f, 0.0f), WVector(0.0f, 0.0f, 2.9f), 1.0f, m_alpha, (enumPolyBlend)0, false);
	}

	if (m_pPetFrame != NULL && m_pPetFrame->GetCurAniPet() != NULL) {
		if (g_view->InFrustum(m_pPetFrame->GetBonePet()->GetBoundBox())) {
			if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pLitAvatar == this) {
				m_light.nearone = data_00aa5278;
				m_light.diffuse = m_light.ambient = 0xFFFFFFFF;
			}
			else {
				m_light.nearone = data_00aa5278;
				m_light.ambient = 0xFFD2D2D2;
				m_light.diffuse = ModifyColor(data_00aa5284, GetPVS().GetShadowIntensity(pos));
			}

			m_pPetFrame->UpdateLightSource(&m_light, false, NULL);

			m_pPetFrame->Render(g_view, true, 1.0f, false, false, true);
		}
	}

	g_view->Flush(1);

	m_fxBox.Display(g_view);
}

// realmyroom_avatar.cpp:1357
// Draws the 2D overlay over the avatar's head: the sex icon, the guild
// emblem and the nickname.
void CRealMyRoom_Avatar::RenderOverlay(bool detail) {
	if (m_pBalloonText != NULL) {
		m_pBalloonText->SetTransparency(m_alpha);
	}

	if (GOLFDOC()->GameMode() != 0x200 && WAbsBySign(m_alpha) < data_9fa510[3]) {
		return;
	}

	float alpha;
	if (GOLFDOC()->GameMode() == 0x200) {
		alpha = 1.0f;
	}
	else {
		alpha = m_alpha;
	}

	if (m_pPetFrame == NULL || m_pPetFrame->GetCurAniPet() == NULL) {
		return;
	}

	if (!g_view->InFrustum(m_pPetFrame->GetBonePet()->GetBoundBox())) {
		return;
	}

	WVector head = m_pPetFrame->GetBonePet()->FindBone("Bip01 HeadNub")->GetMatrix().pivot;

	WVector screen = g_view->Projection(head);

	if (screen.z <= 0.044f || screen.z >= 1.0f) {
		return;
	}

	screen.x = floorf(screen.x);
	screen.y = floorf(screen.y);

	if (IsLocalContent((localContentType_t)10)) {
		CSharedDoc *pDoc = WSingleton<CSharedDoc>::Instance();

		if (m_guid != pDoc->GetPangyaUserInfo().guid) {
			unsigned long guid = m_guid;
			std::map<unsigned long, sBriefUserInfo>::iterator it = pDoc->m_userMap.find(guid);
			const sBriefUserInfo &info = (*it).second;

			if (info.identityBits.administrator) {
				if (!info.invisible) {
					return;
				}
			}
		}
	}

	if (WSingleton<CProjectG>::Instance()->HidePrivacy()) {
		return;
	}

	if (!m_hide) {
		return;
	}

	float y = screen.y - 4.0f - 22.0f;
	FrEmoticon *pEmoticon = g_pFresh->GetManager()->GetEmoticon();
	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();

	WOverlay *pIcon;
	if (m_slotFlag57) {
		pIcon = m_pMain->m_pAngelIcons[m_userFlag % 2];
	}
	else if (m_slotFlag55) {
		pIcon = m_pMain->m_pMannerIcons[m_userFlag % 2];
	}
	else {
		pIcon = m_pMain->m_pGenderIcons[m_userFlag];
	}

	float x = screen.x - (pIcon->GetWidth() + pEmoticon->GetTextWidth(m_nickName) + (float)(m_pEmblem != NULL ? m_pEmblem->Width() : 0)) * 0.5f;

	WRECT iconSrc = { 0.0f, 0.0f, 1.0f, 1.0f };
	WRect iconRect(x, y - 7.0f, pIcon->GetWidth(), pIcon->GetHeight());
	x += pIcon->GetWidth();
	unsigned long alphaBits = (unsigned char)(int)(alpha * 255.0f) << 24;
	unsigned long color = alphaBits | 0xFFFFFF;
	pIcon->Render(g_view, iconSrc, iconRect, 0, color, 0.0f, 0);

	if (m_pEmblem != NULL) {
		iconRect = WRect(x + 0.5f / (float)m_pEmblem->Width(),
			y + 0.5f / (float)m_pEmblem->Height() - 5.0f,
			(float)m_pEmblem->Width(), (float)m_pEmblem->Height());
		pGDI->DrawTexture(m_pEmblem, iconRect, color, 0);
		x += m_pEmblem->Width();
	}

	pGDI->SetScale(1.0f);
	pGDI->SetTextStyle(2);
	if (WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pLitAvatar == this) {
		pGDI->SetTextColor(alphaBits, color);
	}
	else {
		pGDI->SetTextColor(color, alphaBits);
	}

	pEmoticon->SetAnim(false);
	pEmoticon->PrintText(WPoint(x, y), 0, m_nickName, color);
}

// realmyroom_avatar.cpp:1598
void CRealMyRoom_Avatar::ScriptProcess(const w_motion_data *data, float from, float to, bool all) {
	if ((!(from <= data->e) || from == to) && !all) {
		return;
	}

	int start, end;
	from *= 30.0f;
	if (all) {
		start = Ceil(from, 0.03f);
		end = start;
	} else {
		to = Min(data->e, to) * 30.0f;
		start = Ceil(from, 0.03f);
		end = WAbsBySign((float)floor(to) - to) < data_9fa510[3] ? (int)to - 1 : (int)floor(to);

		if (start == (int)floor(data->s * 30.0f + 0.5f)) {
			start++;
		}

		if (start > end) {
			return;
		}
	}

	if (m_pPetFrame) {
		WPuppet *pPuppet = m_pPetFrame->GetCurAniPet();

		if (pPuppet) {
			int first, last;
			if (pPuppet->CheckFrameData(start, end, first, last)) {
				for (int i = first; i <= last; i++) {
					ParsePuppetScript(pPuppet->GetFrameData(i));
				}
			}
		}
	}
}

// realmyroom_avatar.cpp:1642
void CRealMyRoom_Avatar::ParsePuppetScript(const char *script) {
	if (script == NULL) {
		return;
	}

	const char *p = script;

	while (p) {
		p = strchr(p, '*');

		if (p == NULL) {
			return;
		}

		int len = 0;

		while (p[len] != '\0') {
			if (p[++len] == ' ') {
				break;
			}
			if (p[len] == '(') {
				len++;
				break;
			}
		}

		if (len <= 1) {
			return;
		}

		strncpy(data_af5610[1], p, len);
		p += len;
		data_af5610[1][len] = '\0';

		if (strcmp(data_af5610[1], "*fx(") == 0) {
			char token[64];
			char fxName[64];
			char argument[64];
			char boneName[64];

			token[0] = '\0';
			p = GetStrConst(p, token);

			if (strlen(token) == 0 || strlen(token) >= sizeof(token)) {
				return;
			}

			if (token[0] == '@') {
				if (m_pMotionData == NULL || strcmp(token + 1, m_pMotionData->name) != 0) {
					return;
				}

				p = GetStrConst(p, fxName);

				if (strlen(fxName) == 0 || strlen(fxName) >= sizeof(fxName)) {
					return;
				}
			}
			else {
				strcpy(fxName, token);
			}

			p = GetStrConst(p, argument);

			int argumentLen = strlen(argument);
			if (argumentLen == 0 || argumentLen >= 64) {
				return;
			}

			strcpy(boneName, argument);

			if (strlen(boneName) == 0 || strlen(boneName) >= sizeof(boneName)) {
				return;
			}

			Attacher attacher;

			unsigned int i;
			for (i = 0; i < 4; i++) {
				if (i != 0) {
					p = GetStrConst(p, boneName);

					if (strlen(boneName) == 0 || strlen(boneName) >= sizeof(boneName)) {
						break;
					}
				}

				WBone *pBone = m_pPetFrame->GetBonePet()->FindBone(boneName);

				if (pBone) {
					attacher.m_pMatrix[i] = &pBone->GetMatrix();
				}
				else {
					w_bound_box *pBox = m_pPetFrame->GetBonePet()->m_bbList.Find(boneName);

					if (pBox) {
						if (pBox->bone) {
							attacher.m_pMatrix[i] = &pBox->bone->GetMatrix();
						}
						attacher.m_pOffset[i] = &pBox->info->spherePivot;
					}
				}
			}

			if ((int)i > 0) {
				AttachFx(attacher, fxName, true, NULL);
			}
		}
		else if (strcmp(data_af5610[1], "*hidebone(") == 0 || strcmp(data_af5610[1], "*showbone(") == 0) {
			char boneName[64] = "";

			for (int i = 0; i < 64; i++) {
				p = GetStrConst(p, boneName);

				if (strlen(boneName) == 0 || strlen(boneName) >= sizeof(boneName)) {
					break;
				}

				WBone *pBone = m_pPetFrame->GetBonePet()->FindBone(boneName);

				if (pBone) {
					pBone->SetInvisible(false, data_af5610[1][1] == 'h');
				}

				if (p == NULL || *p == ')') {
					break;
				}
			}
		}
		else if (strcmp(data_af5610[1], "*snd(") == 0) {
			p = GetStrConst(p, data_af5610[0]);

			if (strlen(data_af5610[0]) == 0 || strlen(data_af5610[0]) >= sizeof(data_af5610[0])) {
				return;
			}

			WVector toCamera = m_position - g_camera.pivot;
			if (toCamera.Magnitude() < 192.0f) {
				g_audio->PlaySfx(data_af5610[0], &toCamera, 5, NULL, NULL, 0.5f, 200.0f);
			}
		}
		else if (strcmp(data_af5610[1], "*stepsnd(") == 0) {
			if (IsLocalContent((localContentType_t)10)) {
				if (this != WSingleton<CRealMyRoom_AvatarManager>::Instance()->m_pMyAvatar) {
					CSharedDoc *pDoc = WSingleton<CSharedDoc>::Instance();
					int guid = m_guid;
					const sBriefUserInfo &info = (*pDoc->m_userMap.find(guid)).second;

					if (m_isAdministrator) {
						if (!sBriefUserInfo(info).invisible) {
							continue;
						}
					}
				}

				const char *sound = GetPVS().GetStepSound(GetPVS().GetGroundPoint(m_position, false, NULL, false));

				if (sound) {
					WVector toCamera = m_position - g_camera.pivot;
					if (toCamera.Magnitude() < 192.0f) {
						g_audio->PlaySfx(sound, &toCamera, 5, sound, NULL, 0.5f, 200.0f);
					}
				}
			}
			else {
				const char *sound = GetPVS().GetStepSound(GetPVS().GetGroundPoint(m_position, false, NULL, false));

				if (sound) {
					WVector toCamera = m_position - g_camera.pivot;
					if (toCamera.Magnitude() < 192.0f) {
						g_audio->PlaySfx(sound, &toCamera, 5, sound, NULL, 0.5f, 200.0f);
					}
				}
			}
		}
		else if (strcmp(data_af5610[1], "*ptex(") == 0) {
			char token[64];
			char texture[64];

			token[0] = '\0';
			p = GetStrConst(p, token);

			if (strlen(token) == 0 || strlen(token) >= sizeof(token)) {
				return;
			}

			if (token[0] == '@') {
				if (m_pMotionData == NULL || strcmpi(token + 1, m_pMotionData->name) != 0) {
					return;
				}

				p = GetStrConst(p, data_af5610[0]);

				if (strlen(data_af5610[0]) == 0 || strlen(data_af5610[0]) >= sizeof(data_af5610[0])) {
					return;
				}
			}
			else {
				strcpy(data_af5610[0], token);
			}

			p = GetStrConst(p, texture);

			if (strlen(texture) == 0 || strlen(texture) >= sizeof(texture)) {
				return;
			}

			if (m_facialTime > 0.0f) {
				return;
			}

			SetPendingTexPart(data_af5610[0], texture);
		}
		else if (stricmp(data_af5610[1], "*showvis(") == 0) {
			ParseScript_ShowVis(true, p);
		}
		else if (stricmp(data_af5610[1], "*hidevis(") == 0) {
			ParseScript_ShowVis(false, p);
		}
	}
}

// realmyroom_avatar.cpp:1930
void CRealMyRoom_Avatar::ParseScript_ShowVis(bool show, const char *script) {
	char token[0x100] = "";
	int count = 0;

	do {
		script = GetStrConst(script, token);

		if (strlen(token) == 0 || strlen(token) >= sizeof(token)) {
			break;
		}

		std::list<std::string> *pGroup = WSingleton<CSharedDoc>::Instance()->GetVisGroup(token);

		if (pGroup) {
			for (std::list<std::string>::iterator it = pGroup->begin(); it != pGroup->end(); ++it) {
				WBone *pBone = m_pPetFrame->GetBonePet()->FindBone(it->c_str());

				if (pBone) {
					pBone->SetInvisible(false, !show);
				}
			}
		}

		if (script == NULL || *script == ')') {
			break;
		}
	} while (++count < 64);
}

// realmyroom_avatar.cpp:1964
// Opens the effects the avatar's bounding boxes name in their option script:
// "*fx <name>.spr|.seq" or "*fx(\"<name>\") \"&<part id>\"...", where the
// effect needs one of the listed parts worn. The effects opened by the last
// call are deleted first.
void CRealMyRoom_Avatar::ParseBoundingBoxScript() {
	if (m_pPetFrame == NULL || m_pPetFrame->GetBonePet() == NULL) {
		return;
	}

	static std::vector<CFxSpray *> sprays;
	static std::vector<CFxSequence *> sequences;

	char token[] = "*fx";
	WPuppet *pPuppet = m_pPetFrame->GetBonePet();

	unsigned int i;
	for (i = 0; i < sprays.size(); i++)
		m_fxBox.DeleteSpray(sprays[i]);
	for (i = 0; i < sequences.size(); i++)
		m_fxBox.DeleteSequence(sequences[i], true);

	sprays.clear();
	sequences.clear();

	for (w_bound_box *pBox = pPuppet->m_bbList.Start(); pBox; pBox = pPuppet->m_bbList.Next()) {
		if (pBox->info->option == NULL || pBox->bone == NULL || pBox->bone->IsInvisible()) {
			continue;
		}

		const char *script = pBox->info->option;
		for (const char *found = strstr(script, token); found != NULL; found = strstr(script, token)) {
			unsigned int pos = strlen(token);
			if (found[pos] == '(')
				pos++;

			char name[64];
			if (pos > strlen(token))
				script = GetStrConst(found + pos, name);
			else
				script = GetCharsTillSpace(found + pos, name);

			int len = strlen(name);

			if (len == 0 || len >= 64)
				continue;

			char *ext = strrchr(name, '.');
			if (ext == NULL || strlen(ext) < 4)
				continue;

			char part[64];
			script = GetStrConst(script, part);

			len = strlen(part);
			if (len == 0 || len >= 64)
				continue;

			bool partRequired = false;

			while (part[0] == '&') {
				partRequired = true;
				unsigned long tid = atoi(part + 1);

				if (m_pPetFrame->m_partList.FindPart(tid)) {
					partRequired = false;
					break;
				}

				if (m_pPetFrame->FindAuxPart(tid)) {
					partRequired = false;
					break;
				}

				script = GetStrConst(script, part);

				len = strlen(part);
				if (len == 0 || len >= 64)
					break;
			}

			if (partRequired)
				continue;

			Attacher attacher;

			attacher.m_pMatrix[0] = &pBox->bone->GetMatrix();
			attacher.m_pOffset[0] = &pBox->info->spherePivot;

			if (stricmp(ext + 1, "spr") == 0) {
				CFxSpray *pSpray = m_fxBox.LoadSpray(name, NULL);

				if (pSpray == NULL)
					continue;

				pSpray->m_pAttacher = &attacher;
				pSpray->m_fAttachLimitA = attacher.m_fLimitA;
				pSpray->m_fAttachLimitB = attacher.m_fLimitB;
				memcpy(pSpray->m_pMatrix, attacher.m_pMatrix, sizeof(pSpray->m_pMatrix));
				memcpy(pSpray->m_pOffset, attacher.m_pOffset, sizeof(pSpray->m_pOffset));
				m_fxBox.AddSpray(pSpray);
				sprays.push_back(pSpray);
			}
			else if (stricmp(ext + 1, "seq") == 0) {
				CFxSequence *pSequence = m_fxBox.LoadSequence(name, true, NULL);

				if (pSequence == NULL)
					continue;

				pSequence->m_pAttacher = &attacher;
				pSequence->m_fDistLimitA = attacher.m_fLimitA;
				pSequence->m_fDistLimitB = attacher.m_fLimitB;
				m_fxBox.AddSequence(pSequence);
				sequences.push_back(pSequence);
			}
		}
	}
}

// realmyroom_avatar.cpp:2145
sTexturePart *CRealMyRoom_Avatar::FindTexturePart(const char *part, const char *texture) {
	sTexturePart *pTexPart;
	std::list<sTexturePart *>::iterator it;

	it = std::find_if(m_pendingTexParts.begin(), m_pendingTexParts.end(), sFindTexPart(texture));

	if (it != m_pendingTexParts.end()) {
		pTexPart = *it;
	}
	else {
		pTexPart = new sTexturePart;

		strcpy(pTexPart->m_partName, part);
		strcpy(pTexPart->m_texName, texture);
		m_pendingTexParts.push_back(pTexPart);
	}

	return pTexPart;
}

// realmyroom_avatar.cpp:2168
void CRealMyRoom_Avatar::SetPendingTexPart(const char *part, const char *texture) {
	if (m_pPetFrame == NULL || m_pPetFrame->GetCurAniPet() == NULL) {
		return;
	}

	sTexturePart *pTexPart = FindTexturePart(part, texture);

	if (pTexPart->m_pBitmap == NULL) {
		m_pPetFrame->GetBonePet()->ChangeTexturePart(m_pPetFrame->GetFaceTex(), pTexPart->m_texName, &pTexPart->m_pBitmap, &pTexPart->m_rect);
	}
	else {
		m_pPetFrame->GetBonePet()->ChangeTexturePart(m_pPetFrame->GetFaceTex(), pTexPart->m_pBitmap, pTexPart->m_rect);
	}

	m_pPetFrame->SetMakeup();
}

// realmyroom_avatar.cpp:2193
void CRealMyRoom_Avatar::SetChatFacial(int facial) {
	SetPendingTexPart("face", data_a88020[m_charTypeId & 0x3FFFFFF][facial]);

	m_facialTime = 5.0f;
}

// realmyroom_avatar.cpp:2202
void CRealMyRoom_Avatar::SetBalloonText(const char *text) {
	m_balloonTime = m_alpha;
	m_balloonElapsed = 0;
}

// realmyroom_avatar.cpp:2213
void CRealMyRoom_Avatar::SetTradeTitle(const char *title) {
	if (title) {
		strcpy(m_tradeTitle, title);
	}
	else {
		m_tradeTitle[0] = '\0';
	}
}

// realmyroom_avatar.cpp:2221
void CRealMyRoom_Avatar::RefreshUccClothes(unsigned long tid, unsigned long id) {
	if (m_pPetFrame == NULL) {
		return;
	}

	if (IsLocalContent((localContentType_t)0x5F)) {
		m_pPetFrame->SetPart(&m_partTidList, tid, id);
		m_pPetFrame->SetPart(&m_partTidList, tid, id);
	}
}

// realmyroom_avatar.cpp:2241
CRealMyRoom_AvatarManager::CRealMyRoom_AvatarManager() {
	m_avatarCount = 0; for (int i = 0; i < 30; i++) m_pAvatars[i] = NULL;

	m_pMyAvatar = NULL;
	m_pLitAvatar = NULL;
	m_pHighlightAvatar = NULL;
	m_isRenderEnabled = true;
	m_overlayDetail = true;
}

// realmyroom_avatar.cpp:2257
CRealMyRoom_AvatarManager::~CRealMyRoom_AvatarManager() {
	FreeAllAvatars();
}

// realmyroom_avatar.cpp:2262
void CRealMyRoom_AvatarManager::FreeAllAvatars() {
	for (int i = 0; i < 30; i++) {
		if (m_pAvatars[i]) {
			if (m_pAvatars[i]->m_pPetFrame) {
				delete m_pAvatars[i]->m_pPetFrame;
				m_pAvatars[i]->m_pPetFrame = NULL;
			}

			if (m_pAvatars[i]) {
				delete m_pAvatars[i];
				m_pAvatars[i] = NULL;
			}
		}
	}
}

// realmyroom_avatar.cpp:2284
CRealMyRoom_Avatar *CRealMyRoom_AvatarManager::FindAvatarByIDName(const char *idName) const {
	for (int i = 0; i < 30; i++) {
		if (m_pAvatars[i] != NULL) {
			if (strcmp(m_pAvatars[i]->m_idName, idName) == 0) {
				return m_pAvatars[i];
			}
		}
	}

	return NULL;
}

// realmyroom_avatar.cpp:2300
CRealMyRoom_Avatar *CRealMyRoom_AvatarManager::FindAvatarByNickName(const char *nickName) const {
	for (int i = 0; i < 30; i++) {
		if (m_pAvatars[i] != NULL) {
			if (strcmp(m_pAvatars[i]->m_nickName, nickName) == 0) {
				return m_pAvatars[i];
			}
		}
	}

	return NULL;
}

// realmyroom_avatar.cpp:2316
CRealMyRoom_Avatar *CRealMyRoom_AvatarManager::FindAvatarByGuid(int guid) const {
	for (int i = 0; i < 30; i++) {
		if (m_pAvatars[i] != NULL) {
			if (m_pAvatars[i]->m_guid == guid) {
				return m_pAvatars[i];
			}
		}
	}

	return NULL;
}

// realmyroom_avatar.cpp:2332
CRealMyRoom_Avatar *CRealMyRoom_AvatarManager::CreateAvatar(const sSlotInfo &slotInfo, const sCharacterInfo &charInfo, CRealMyRoomMain *pMain) {
	int slot;

	for (slot = 0; slot < 30; slot++) {
		if (m_pAvatars[slot] == 0) {
			break;
		}
	}

	if (slot == 30) {
		// every slot is taken
		return 0;
	}

	m_avatarCount++;

	CRealMyRoom_Avatar *pAvatar = new CRealMyRoom_Avatar(slotInfo, charInfo, pMain);
	WVector position(slotInfo.m_x, 0.0f, slotInfo.m_z);
	position = GetPVS().GetGroundPoint(position, false, 0, false);
	pAvatar->SetPosition(position);
	float angle = slotInfo.m_angle;
	if (angle >= data_9fa510[1] || angle < 0.0f) {
		angle -= (float)floor((double)(angle * 0.15915494f)) * data_9fa510[1];
	}
	pAvatar->m_angle = angle;
	pAvatar->m_targetAngle = angle;

	m_pAvatars[slot] = pAvatar;
	return pAvatar;
}

// realmyroom_avatar.cpp:2366
bool CRealMyRoom_AvatarManager::DeleteAvatar(const char *idName) {
	int i;
	for (i = 0; i < 30; i++) {
		if (m_pAvatars[i]) {
			if (strcmp(m_pAvatars[i]->m_idName, idName) == 0) {
				break;
			}
		}
	}

	if (i == 30) {
		return false;
	}

	if (m_pAvatars[i]) {
		delete m_pAvatars[i];
		m_pAvatars[i] = NULL;
	}
	m_avatarCount--;

	return true;
}

// realmyroom_avatar.cpp:2391
CRealMyRoom_Avatar *CRealMyRoom_AvatarManager::AvatarByRayIntersect(const WVector &pos, const WVector &dir) const {
	Waabb aabb;
	for (int i = 0; i < 30; i++) {
		if (m_pAvatars[i]) {
			if (IsLocalContent((localContentType_t)10)) {
				CSharedDoc *pDoc = WSingleton<CSharedDoc>::Instance();
				if (m_pAvatars[i]->m_guid != pDoc->GetPangyaUserInfo().guid) {
					std::map<unsigned long, sBriefUserInfo>::iterator it;
					it = pDoc->m_userMap.find(m_pAvatars[i]->m_guid);

					if (!(pDoc->GetPangyaUserInfo().identity & 4)) {
						if (sBriefUserInfo((*it).second).identityBits.administrator) {
							if (!sBriefUserInfo((*it).second).invisible)
								break;
						}
					}
				}
			}

			aabb.min = WVector(-1.0f, 0.0f, -1.0f);
			aabb.min += m_pAvatars[i]->m_position;
			aabb.max = WVector(1.0f, 8.0f, 1.0f);
			aabb.max += m_pAvatars[i]->m_position;

			if (RayIntersectAABB(aabb, pos, dir, NULL, 0.0f) < 1.0f) {
				return m_pAvatars[i];
			}
		}
	}

	return NULL;
}

// realmyroom_avatar.cpp:2430
CRealMyRoom_Avatar *CRealMyRoom_AvatarManager::CheckClick(float x, float y) const {
	for (int i = 0; i < 30; i++) {
		if (m_pAvatars[i] && m_pAvatars[i]->IsClickable(x, y)) {
			return m_pAvatars[i];
		}
	}

	return NULL;
}

// realmyroom_avatar.cpp:2443
void CRealMyRoom_AvatarManager::Process(float elapsed) {
	for (int i = 0; i < 30; i++) {
		if (m_pAvatars[i] != NULL) {
			m_pAvatars[i]->Process(elapsed);
		}
	}
}

// realmyroom_avatar.cpp:2454
void CRealMyRoom_AvatarManager::Render() {
	if (!m_isRenderEnabled) {
		return;
	}

	for (int i = 0; i < 30; i++) {
		if (m_pAvatars[i] != NULL) {
			m_pAvatars[i]->Render();
		}
	}
}

// realmyroom_avatar.cpp:2468
void CRealMyRoom_AvatarManager::ResetAvatarPositionAngle() {
	for (int i = 0; i < 30; i++) {
		if (m_pAvatars[i] != NULL) {
			m_pAvatars[i]->m_position = m_pAvatars[i]->m_targetPosition;
			m_pAvatars[i]->m_angle = m_pAvatars[i]->m_targetAngle;
		}
	}
}

// realmyroom_avatar.cpp:2482
// qsort order of RenderOverlay: the avatar farther from the camera first.
static int __cdecl CompareCameraDistance(const void *a, const void *b) {
	WVector delta = g_view->GetCamera().pivot;
	WVector deltaA;
	deltaA = delta - (*(CRealMyRoom_Avatar *const *)a)->m_position;
	delta = delta - (*(CRealMyRoom_Avatar *const *)b)->m_position;

	float distA = deltaA.x * deltaA.x + deltaA.y * deltaA.y + deltaA.z * deltaA.z;
	float distB = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;

	if (distA > distB) {
		return -1;
	}
	if (distA < distB) {
		return 1;
	}
	return 0;
}

// realmyroom_avatar.cpp:2503
void CRealMyRoom_AvatarManager::RenderOverlay() {
	if (!m_isRenderEnabled) {
		return;
	}

	CRealMyRoom_Avatar *avatars[30];
	int count = 0;
	for (int i = 0; i < 30; i++) {
		if (m_pAvatars[i]) {
			avatars[count++] = m_pAvatars[i];
		}
	}

	qsort(avatars, count, sizeof(CRealMyRoom_Avatar *), CompareCameraDistance);

	for (int j = 0; j < count; j++) {
		if (avatars[j]) {
			avatars[j]->RenderOverlay(m_overlayDetail);
		}
	}
}

// realmyroom_avatar.cpp:2528
CRealMyRoom_AvatarBurden::CRealMyRoom_AvatarBurden() : m_charInfo(), m_pPetFrame(NULL) {
}

// realmyroom_avatar.cpp:2533
CRealMyRoom_AvatarBurden::CRealMyRoom_AvatarBurden(unsigned long guid, const sCharacterInfo &charInfo, CPetFrame *pPetFrame) : m_charInfo() {
	m_guid = guid;

	m_charInfo = charInfo; m_pPetFrame = pPetFrame;
}

// realmyroom_avatar.cpp:2540
CRealMyRoom_AvatarBurden::~CRealMyRoom_AvatarBurden() {
}

// realmyroom_avatar.cpp:2545
bool CRealMyRoom_AvatarBurden::LoadAvatarBurden() {
	if (!m_partTidList.SetTids(&m_charInfo, 0xFF)) {
		m_partTidList.SetDefaultTids();
	}
	if (!m_partTidList.IsComboValid()) {
		m_partTidList.SetDefaultTids();
	}

	if (m_pPetFrame) { delete m_pPetFrame; m_pPetFrame = NULL; }

	m_pPetFrame = new CPetFrame;

	if (!m_pPetFrame->Build(&m_partTidList, m_charInfo.auxParts.tids, true, false, m_guid)) {
		delete m_pPetFrame;
		m_pPetFrame = NULL;
		return false;
	}

	m_pPetFrame->SetHairClr(&m_partTidList, m_charInfo.hairColor);
	m_pPetFrame->SetShirtsClr(&m_partTidList, m_charInfo.shirtColor & 0xF);

	return true;
}
