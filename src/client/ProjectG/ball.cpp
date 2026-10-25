// Original: d:\build\custom\temp\code\client\projectg\ball.cpp
// The original holds the out-of-line copy of wmath.inl's operator*(WMatrix, float) here.
#define BALL_WMATH_INLINE
#include "ball.h"
#include "ObjectFactory.h"
#include "actor.h"
#include "golfball.h"
#include "client/Wangreal/include/wpuppet.h"
#include "client/Wangreal/include/wresrcmng.h"
#include "scenemanager.h"
#include "fx.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "client/Wangreal/include/wutil.h"
#include "client/Wangreal/include/wview.h"
#include "commonutil.h"
#include "quadtree.h"
#include "ProjectG.h"
#include "SoundManager.h"
#include "TaskManager.h"
#include "client/Wangreal/include/wbone.h"

#include <algorithm>

extern WMatrix g_camera;

// The scene's light (golfbg sets it up); its real header is not known yet.
extern LightSet g_lightset;

// The ratio applied to the ambient colour of the ball's light.
float BALL_AMBIENT_RATIO = 0.8f;

// ball.cpp:20 (run-time type and factory registration of CBall)
const WRTTI CBall::m_RTTI("CBall", &IActor::m_RTTI);

IObject* CBallMakeInstance(void) {
	return new CBall;
}

struct __impCBall {
	__impCBall(void) {
		ObjectFactory().AddObjectFunctor(CBallMakeInstance, "CBall");
		ActorFactory().AddActorClass("CBall");
	}
};
static __impCBall s_registerCBall;

// ball.cpp:27
CBall::CBall(void) : m_pPuppet(0), m_hShadowTexture(0) {
	m_bVisible = true;
	m_priority = 1;
	m_lastPreDataIndex = -1;
}

// ball.cpp:32
void CBall::OnPreLoadInit(void) {
	m_axis = WVector::UNIT_POS_X;

	for (int i = 0; i < 4; i++) {
		m_shadowVertices[i].diffuse = 0x80ffffff;
		m_shadowVertices[i].tu = (i & 2) ? 1.0f : 0.0f;
		m_shadowVertices[i].tv = (i & 1) ? 0.0f : 1.0f;
	}

	m_shadowPolygon[0] = &m_shadowVertices[0];
	m_shadowPolygon[1] = &m_shadowVertices[1];
	m_shadowPolygon[2] = &m_shadowVertices[3];
	m_shadowPolygon[3] = &m_shadowVertices[2];
	m_shadowPolygon[4] = NULL;
	m_light.type = LightSet::W_LIGHT_DIRECTIONAL;
	m_hShadowTexture = g_resrcmng->LoadTexture("[ball_shadow.jpg", 0, 0, NULL);

	CSceneManager::Instance()->RegisterElement((eObjType)4, NULL, NULL, "\xB0\xF8", 8, NULL);

	CSceneManager::Instance()->RegisterElement((eObjType)1, NULL, this, "\xB0\xF8\xB1\xD7\xB8\xB2\xC0\xDA", 0, NULL);
}

// ball.cpp:62
void CBall::OnDestroy(void) {
	for (std::list<sBall*>::iterator it = m_balls.begin(); it != m_balls.end(); ++it) {
		if (*it) {
			if (g_resrcmng != NULL && (*it)->m_pPuppet != NULL) { g_resrcmng->Release((*it)->m_pPuppet, false); (*it)->m_pPuppet = NULL; }
			if (*it != NULL) { delete *it; *it = NULL; }
		}
	}

	m_balls.clear();

	if (g_resrcmng != NULL && m_hShadowTexture != 0) { g_resrcmng->Release(m_hShadowTexture); m_hShadowTexture = 0; }
}

// ball.cpp:79
void CBall::OnLoad(void) {
	if (IsLocalCountry(30)) {
		sFxKeeper keeper;
		keeper.m_pKeeper = this;
		keeper.m_pfnCallBack = &CFxKeeper::EmptyFx;
		WSingleton<CFx>::Instance()->AddFxKeeper(keeper);
	}

	if (IsLocalContent((localContentType_t)0x3F)) {
		AddBall(0x14000000);
	}

	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (GOLFDOC()->GetPlayer(i)->m_state == 3) {
			continue;
		}

		AddBall(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_ballId);
	}

	std::list<sBall*>::iterator it = m_balls.begin();

	if (it != m_balls.end()) {
		m_pPuppet = (*it)->m_pPuppet;
	}
}

// ball.cpp:105
void CBall::OnReload(void) {
	for (std::list<sBall*>::iterator it = m_balls.begin(); it != m_balls.end(); ++it) {
		if (*it) {
			if (g_resrcmng != NULL && (*it)->m_pPuppet != NULL) { g_resrcmng->Release((*it)->m_pPuppet, false); (*it)->m_pPuppet = NULL; }
			if (*it != NULL) { delete *it; *it = NULL; }
		}
	}

	m_balls.clear();

	CSceneManager::Instance()->SetPuppet("\xB0\xF8", NULL);

	OnLoad();
}

// ball.cpp:124
void CBall::OnInit(void) {
	m_matrix.Reset();
	m_sinkFactor = 0;
	m_sinkStartIndex = -1;
	m_light.nearone = g_lightset.nearone;
}

// ball.cpp:132
CBall::sBall* CBall::AddBall(unsigned long typeId) {
	sBall* pBall = FindBall(typeId);

	if (pBall)
		return pBall;

	IFF_STRUCT::sBall* pData = ItemManager()->FindBall(typeId);

	if (pData) {
		pBall = new sBall;
		pBall->m_pData = pData;
		char name[40];
		sprintf(name, "%s.pet", pData->m_model);
		pBall->m_pPuppet = g_resrcmng->GetPuppet(name, false, true, false);

		m_balls.push_back(pBall);
	}

	return pBall;
}

// ball.cpp:161
CBall::sBall* CBall::FindBall(unsigned long typeId) {
	std::list<sBall*>::iterator it = std::find_if(m_balls.begin(), m_balls.end(), sFindBall(typeId));

	if (it != m_balls.end()) {
		return *it;
	}
	return 0;
}

// ball.cpp:173
float CBall::GetDownDelta(void) {
	if (m_sinkStartIndex == GolfBall().m_preDataCount)
		return 1.0f;

	float delta = (float)(GolfBall().m_preDataIndex - m_sinkStartIndex);
	return (0.0f < delta ? delta : 0.0f) / (float)(GolfBall().m_preDataCount - m_sinkStartIndex);
}

// ball.cpp:182
void CBall::OnProcess(float elapsed) {
	if (IsLocalContent((localContentType_t)0x4F) && WSingleton<CSharedDoc>::Instance()->m_playingMode == 2) {
		CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
		if (pDoc->m_replayState == 0xB || pDoc->m_replayState == 0xC) {
			if (pDoc->m_replayPlayType == 2) {
				if (pDoc->m_replayState == 0xB) {
					if (pDoc->m_replayStartB > -1) {
						WSingleton<CSharedDoc>::Instance()->m_replayStartB -= 2;

						if (WSingleton<CSharedDoc>::Instance()->m_replayStartB == 0) {
							WSingleton<CSharedDoc>::Instance()->m_replayState = 1;
						} else if (WSingleton<CSharedDoc>::Instance()->m_replayStartB < 0) {
							WSingleton<CSharedDoc>::Instance()->m_replayState = 0xE;
						}
					}
				} else if (pDoc->m_replayState == 0xC) {
					pDoc->m_replayState = 3;
				}
				AfxGetTask()->GetActor("GolfRule") << MsgObject(0, 0xA4, 0, 0, 0, 0, 0);
			} else if (pDoc->m_replayPlayType == 1) {
				AfxGetTask()->GetActor("GolfRule") << MsgObject(0, 0xA4, 0, 0, 0, 0, 0);
			}
		}
	}

	if (!m_bVisible) {
		return;
	}

	CGolfBall& ball = GolfBall();

	if (ball.m_state == 0) {
		m_matrix.pivot = ball.m_pos;
		if (m_sinkFactor > 0.0f) {
			m_matrix.ym -= ball.m_radius * m_sinkFactor * 2.0f;
		}
		if (GOLFDOC()->GameMode() & 0x80) {
			DetachFx(3);
			DetachFx(5);
		}
		return;
	}

	m_light.diffuse = g_lightset.diffuse;

	if (GolfBall().m_velocity.Magnitude() < 10.0f) {
		DetachFx(3);
		DetachFx(5);
	}

	m_axis = WCrossProduct(WVector::UNIT_POS_Y, ball.m_velocity);
	if (WisEqual(m_axis, WVector::ZERO, 1e-5f)) {
		m_axis = WVector::UNIT_POS_Y;
	}
	m_matrix = RotMat(m_axis, ball.m_rotation.y);
	m_matrix.pivot = ball.m_pos;

	if (ball.m_preDataIndex > 0 && ball.m_eventPreDataIndex[6] > 0 && ball.m_preDataIndex >= ball.m_eventPreDataIndex[6] && ball.m_preDataIndex < ball.m_eventPreDataIndex[6] + 50) {
		m_matrix.pivot += WVector((rand() % 11 - 5) * 0.005f, 0.0f, (rand() % 11 - 5) * 0.005f);
	}

	if (m_sinkFactor > 0.0f) {
		float radius = ball.m_radius;
		m_matrix.ym -= m_sinkFactor * (radius * GetDownDelta()) * 2.0f;
	}

	if (m_lastPreDataIndex < GolfBall().m_eventPreDataIndex[0] && GolfBall().m_eventPreDataIndex[0] <= GolfBall().m_preDataIndex) {
		AttachFx(4);
	}
	m_lastPreDataIndex = GolfBall().m_preDataIndex;

	sBall* pBall = FindBall(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId);

	if (pBall == 0) {
		return;
	}

	bool bHasFx = false;
	unsigned int i;
	for (i = 0; i < 4; i++) {
		if (pBall->m_pSpray[i]) {
			bHasFx = true;
			break;
		}
	}
	if (!bHasFx) {
		for (i = 0; i < 4; i++) {
			if (pBall->m_pSequence[i]) {
				bHasFx = true;
				break;
			}
		}
	}

	if (bHasFx) {
		TransformBall();
	}
}

// ball.cpp:301
void CBall::TransformBall(void) {
	int preDataIndex = GolfBall().m_preDataIndex;
	float scale;

	if (preDataIndex < 0) {
		scale = 1.0f;
	} else {
		// The ball's apparent height on screen: keep it between 0.1 and 1.5.
		WVector screen[2];
		screen[0] = g_view->Projection(GolfBall().m_pos - g_camera.ya * GolfBall().m_radius);
		screen[1] = g_view->Projection(GolfBall().m_pos + GolfBall().m_radius * g_camera.ya);
		scale = 1.5f / Between(0.1f, Abs(screen[0].y - screen[1].y), 1.5f);
	}

	if (m_pPuppet) {
		m_pPuppet->ApplyBones(NULL, m_matrix * scale);
		m_pPuppet->Transform(NULL, 1.0f, 0);
		m_pPuppet->UpdateBSphere();
	}
}

// ball.cpp:337
void CBall::OnDisplay(void) {
	if (!m_bVisible) {
		return;
	}

	TransformBall();

	if (m_pPuppet && !m_pPuppet->GetAlpha(0)) {
		return;
	}

	if (GolfBall().m_state == 0 && !m_bRefreshLight) {
		return;
	}

	m_bRefreshLight = false;
	if (m_pPuppet) {
		m_pPuppet->UpdateLightSource(&m_light, false, 0);
	}
}

// ball.cpp:355
void CBall::Display(void) {
	if (Skipped() || !m_bVisible) {
		return;
	}

	WVector cupPos = GOLFDOC()->GetHoleData().m_cupPos;
	WVector dir = cupPos - GolfBall().m_pos;

	CGolfBall& ball = GolfBall();
	if (sqrtf(dir.x * dir.x + dir.z * dir.z) < ball.m_radius * 4.0f) {
		return;
	}

	float size = GolfBall().m_radius * 1.2f;
	int meshIndex;
	WVector ground = GetPVS().GetGroundPoint(GolfBall().m_pos, true, &meshIndex, 0);

	if (meshIndex == -1) {
		return;
	}

	WVector normal = GetPVS().GetMeshNormal(meshIndex);
	WVector axisX = WCrossProduct(normal, WVector::UNIT_POS_Z);
	WVector axisZ = WCrossProduct(normal, axisX);
	axisX.Normalize();
	axisZ.Normalize();
	WVector center(ground.x, ground.y + 0.001f, ground.z);

	for (int i = 0; i < 4; i++) {
		WVector pos = center;
		pos += axisX * (((i & 2) ? 1.0f : -1.0f) * size);
		pos += axisZ * (((i & 1) ? 1.0f : -1.0f) * size);

		m_shadowVertices[i].SetPosition(pos);
	}

	g_view->DrawPolygonFan(m_shadowPolygon, (m_hShadowTexture & 0x7FF) | 0x22300000, 0x400, false);
}

// ball.cpp:402
void CBall::HandleMsg(const MsgObject& msg) {
	if ((unsigned long)msg.m_reserved > g_CurrentTime) {
		StoreDelayedMsg(msg);
		return;
	}

	sBall* pBall;

	switch (msg.m_msg) {
	case 0x163:
		m_bVisible = true;
		WSingleton<CSceneManager>::Instance()->SetVisible("\xB0\xF8", true);
		break;
	case 0x164:
		m_bVisible = false;
		WSingleton<CSceneManager>::Instance()->SetVisible("\xB0\xF8", false);
		break;
	case 0x1EB:
		m_bRefreshLight = true;
		m_light.diffuse = ModifyColor(g_lightset.diffuse, GetPVS().GetShadowIntensity(GolfBall().m_pos));
		m_light.ambient = ModifyColor(g_lightset.ambient, BALL_AMBIENT_RATIO);
		break;
	case 0x1ED:
		switch (GetPVS().GetGroundType(GolfBall().m_pos, true, 0)) {
		case 0:
		case 1:
		case 2:
		case 6:
		case 9:
			m_sinkFactor = 0.0f;
			break;
		case 3:
		case 4:
		case 5:
			m_sinkFactor = 1.0f - GetPVS().GetMinPowerOnGround(GolfBall().m_pos);
			break;
		}
		break;
	case 0x1EC:
		switch (GetPVS().GetGroundType(GolfBall().PreData(GolfBall().m_preDataCount)->m_pos, true, 0)) {
		case 0:
		case 1:
		case 2:
		case 6:
		case 9:
			m_sinkFactor = 0.0f;
			m_sinkStartIndex = -1;
			break;
		case 3:
		case 4:
		case 5:
			m_sinkFactor = 1.0f - GetPVS().GetMinPowerOnGround(GolfBall().PreData(GolfBall().m_preDataCount)->m_pos);
			m_sinkStartIndex = Max(GolfBall().m_preDataCount - 100, GolfBall().m_eventPreDataIndex[7]);
			break;
		}
		break;
	case 0x4C: {
		unsigned long typeId = msg.m_param1;

		DetachFx(0);
		DetachFx(1);

		pBall = AddBall(typeId);
		if (pBall == 0) {
			break;
		}
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId = typeId;

		m_pPuppet = pBall->m_pPuppet;
		if (m_pPuppet) {
			m_pPuppet->UpdateLightSource(&m_light, false, 0);
		}
		WSingleton<CSceneManager>::Instance()->SetPuppet("\xB0\xF8", m_pPuppet);

		AttachFx(0);
		AttachFx(1);
		break;
	}
	case 0x4D: {
		unsigned long typeId = msg.m_param1;
		int playerIndex = msg.m_param2;

		DetachFx(0);
		DetachFx(1);

		pBall = AddBall(typeId);
		if (pBall == 0) {
			break;
		}
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[playerIndex].m_ballId = typeId;

		m_pPuppet = pBall->m_pPuppet;
		if (m_pPuppet) {
			m_pPuppet->UpdateLightSource(&m_light, false, 0);
		}
		WSingleton<CSceneManager>::Instance()->SetPuppet("\xB0\xF8", m_pPuppet);

		AttachFx(0);
		AttachFx(1);
		break;
	}
	case 0x1A0:
		DetachFx(0);
		DetachFx(1);

		m_lastPreDataIndex = -1;
		pBall = FindBall(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId);
		if (pBall == 0) {
			break;
		}
		m_pPuppet = pBall->m_pPuppet;
		if (m_pPuppet) {
			m_pPuppet->UpdateLightSource(&m_light, false, 0);
		}
		WSingleton<CSceneManager>::Instance()->SetPuppet("\xB0\xF8", m_pPuppet);

		AttachFx(0);
		AttachFx(1);
		break;
	case 0x1EE:
		pBall = FindBall(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId);
		if (pBall) {
			AttachFx(0);
			AttachFx(1);

			pBall->m_pSpray[2] = pBall->m_pSpray[3] = 0;

			pBall->m_pSequence[2] = pBall->m_pSequence[3] = 0;
		}
		break;
	case 0x1EF:
		AttachFx(2);
		AttachFx(3);
		break;
	}
}

// ball.cpp:568
void CBall::AttachFx(int index) {
	if (index < 0 || index >= 7) {
		return;
	}

	sBall* pBall = FindBall(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId);

	if (pBall == 0 || pBall->m_pData == 0 || pBall->m_pData->m_fxName[index][0] == 0) {
		return;
	}

	int len = strlen(pBall->m_pData->m_fxName[index]);

	if (len < 5) {
		return;
	}

	Attacher attacher;
	WBone* pBone = 0;

	if (pBall->m_pData->m_fxBone[index][0] && m_pPuppet) {
		pBone = m_pPuppet->FindBone(pBall->m_pData->m_fxBone[index]);
	}
	if (pBone) {
		attacher.m_pMatrix[0] = &pBone->GetMatrix();
	} else {
		if (m_pPuppet) {
			const char* boneName = pBall->m_pData->m_fxBone[index];
			w_bound_box* pBox;
			if (boneName[0]) {
				pBox = m_pPuppet->m_bbList.Find(boneName);
			} else {
				pBox = m_pPuppet->m_bbList.Find("center");
			}

			if (pBox) {
				if (pBox->bone) {
					attacher.m_pMatrix[0] = &pBox->bone->GetMatrix();
				}
				attacher.m_pOffset[0] = &pBox->info->spherePivot;
			}
		}
	}

	int slot;
	switch (index) {
	case 0:
		slot = 0;
		break;
	case 1:
		slot = 1;
		break;
	case 3:
		slot = 2;
		break;
	case 5:
		slot = 3;
		break;
	default:
		slot = -1;
		break;
	}

	void* pFx = WSingleton<CFx>::Instance()->Attach(attacher, pBall->m_pData->m_fxName[index], true, NULL);

	if (slot == -1) {
		return;
	}

	if (strcmpi(pBall->m_pData->m_fxName[index], "BFX_B3.seq") == 0) {
		g_audio->PlaySfx("Ball_B", NULL, 0, NULL, NULL, 0.5f, 200.0f);
	}

	if (strcmpi(&pBall->m_pData->m_fxName[index][len - 3], "spr") == 0) {
		pBall->m_pSpray[slot] = (CFxSpray*)pFx;
	} else {
		pBall->m_pSequence[slot] = (CFxSequence*)pFx;
	}
}

// ball.cpp:656
void CBall::DetachFx(int index) {
	if (index != 0 && index != 1 && index != 3 && index != 5) {
		return;
	}

	sBall* pBall = FindBall(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId);

	if (pBall == 0) {
		return;
	}

	int slot;
	switch (index) {
	case 0:
		slot = 0;
		break;
	case 1:
		slot = 1;
		break;
	case 3:
		slot = 2;
		break;
	case 5:
		slot = 3;
		break;
	default:
		return;
	}

	if (pBall->m_pSpray[slot]) {
		WSingleton<CFx>::Instance()->CloseSpray(pBall->m_pSpray[slot]);
		pBall->m_pSpray[slot] = 0;
	}

	if (pBall->m_pSequence[slot]) {
		if (slot == 0) {
			WSingleton<CFx>::Instance()->CloseSequence(pBall->m_pSequence[slot], true);
		} else {
			WSingleton<CFx>::Instance()->CloseSequence(pBall->m_pSequence[slot], false);
		}
		pBall->m_pSequence[slot] = 0;
	}
}

// ball.cpp:715
void CBall::EmptyFx(void* pFx) {
	if (pFx == 0) {
		return;
	}

	for (std::list<sBall*>::iterator it = m_balls.begin(); it != m_balls.end(); ++it) {
		sBall* pBall = *it;

		if (pBall == 0) {
			continue;
		}

		for (int i = 0; i < 4; i++) {
			if (pBall->m_pSpray[i] == pFx) {
				pBall->m_pSpray[i] = 0;
			}
			if (pBall->m_pSequence[i] == pFx) {
				pBall->m_pSequence[i] = 0;
			}
		}
	}
}
