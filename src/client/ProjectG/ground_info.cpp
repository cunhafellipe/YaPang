// Original: d:\build\custom\temp\code\client\projectg\ground_info.cpp
#include "ground_info.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "ClientSetting.h"
#include "TaskManager.h"
#include "golfball.h"
#include "club.h"
#include "quadtree.h"
#include "fxspray.h"
#include "fxsequence.h"
#include "client/Wangreal/include/wbone.h"
#include "client/Wangreal/include/wpuppet.h"
#include "client/Wangreal/include/wresrcmng.h"
#include "client/Wangreal/include/wview.h"

#include <algorithm>
#include <stdio.h>

extern WMatrix g_camera;

// ground_info.cpp:19
CBase::CBase(void) : m_pCurBall(0), m_hConditionLine(0), m_hConditionFont(0) {
	float barY = COption::Instance()->gGetBar_Y();

	m_screenX = 58.0f;
	m_screenY = barY + 4.0f;

	Load();

	m_hConditionLine = g_resrcmng->LoadTexture("{condition_line.png", 0, 0, 0);
	m_hConditionFont = g_resrcmng->LoadTexture("[font_conditoin.jpg", 0, 0, 0);

	m_spin.Reset();
	m_unknownC0 = 0;
	m_angle = 0;

	m_uiMatrix.Reset();
	m_uiMatrix.pivot = WVector(0.0f, 0.0f, -1.8f) * (g_view->GetWidth() * 0.0015625f);

	m_light.type = LightSet::W_LIGHT_DIRECTIONAL;

	m_light.ambient = 0x808080;
	m_light.diffuse = 0xFFFFFF;
	m_light.nearone = WVector(-1.0f, -1.0f, 1.0f).Normalize();

	m_normal.Reset();
	m_matrix90.Reset();

	m_vertexList[0] = &m_vertices[0];
	m_vertexList[1] = &m_vertices[1];
	m_vertexList[2] = &m_vertices[3];
	m_vertexList[3] = &m_vertices[2];
	m_vertexList[4] = 0;

	m_powerLoss = 0;
	m_lieIconIndex = 12;

	m_fxBox.Init(g_resrcmng);
	m_fxBox.SetCamera(&g_camera);
	m_fxBox.m_frameTime = 33.333332f;
}

// ground_info.cpp:61
CBase::~CBase(void) {
	for (std::list<sBall*>::iterator it = m_balls.begin(); it != m_balls.end(); ++it) {
		if (*it) {
			if (g_resrcmng != NULL && (*it)->m_pPuppet != NULL) { g_resrcmng->Release((*it)->m_pPuppet, false); (*it)->m_pPuppet = NULL; }
			delete *it;
		}
	}

	m_balls.clear();

	if (m_hConditionLine) {
		g_resrcmng->Release(m_hConditionLine);
		m_hConditionLine = 0;
	}

	if (m_hConditionFont) {
		g_resrcmng->Release(m_hConditionFont);
		m_hConditionFont = 0;
	}

	m_fxBox.ClearAll(false);
}

// ground_info.cpp:90
void CBase::Load(void) {
	for (std::list<sBall*>::iterator it = m_balls.begin(); it != m_balls.end(); ++it) {
		if (*it) {
			if (g_resrcmng != NULL && (*it)->m_pPuppet != NULL) { g_resrcmng->Release((*it)->m_pPuppet, false); (*it)->m_pPuppet = NULL; }
			delete *it;
		}
	}

	m_balls.clear();

	m_fxBox.ClearAll(false);

	for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (PLAYER(i)->m_state != 3) {
			AddBall(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_ballId);
		}
	}
}

// ground_info.cpp:115
void CBase::ResetUiPosition(void) {
	float barY = COption::Instance()->gGetBar_Y();

	m_screenY = barY + 4.0f;
	m_fxBox.SetScreenCenter(m_screenX, m_screenY);

	m_uiMatrix.pivot = WVector(0.0f, 0.0f, -1.8f) * (g_view->GetWidth() * 0.0015625f);
}

// ground_info.cpp:126
void CBase::Process(float elapsed) {
	if (GOLFDOC()->m_gameState >= 12) {
		if (GolfBall().m_preDataIndex < 0) {
			AfxGetTask()->GetActor("Spin") << MsgObject(0, 0x161, (intptr_t)&m_spin.x, (intptr_t)&m_spin.y, 0, 0, 0);
			m_spin *= elapsed * 12.566371f;

			if (GolfClub().GetType() != 3) {
				m_matrix90 = m_matrix90 * (RotMat(m_spin.x, 1) * RotMat(m_spin.y, 0));
			}
		} else {
			m_matrix90 = RotMat(GolfBall().m_rotation.x, 1) * RotMat(GolfBall().m_rotation.y, 0);
		}

		m_unknownC0 += elapsed * 0.31415927f;

		elapsed *= 1000.0f;
		CFxBox* pFxBox = &m_fxBox;
		pFxBox->ProcessAttacher(elapsed);
		m_fxBox.Process(elapsed);
	}
}

// ground_info.cpp:162
void CBase::Display(void) {
	float oldNear = g_view->GetClipNearValue();
	if (!(0.0f < oldNear)) oldNear = 0.0f;
	float oldFar = g_view->GetClipFarValue();
	float oldFov = g_view->GetFOV();
	g_view->SetFOV(g_view->GetRatio() * 2.0943952f);
	g_view->SetClip(0.1f, 10.0f, false);
	g_view->SetCamera(m_uiMatrix);
	g_view->SetScreenCenter(m_screenX, m_screenY);
	g_view->UpdateCamera();

	if (m_pCurBall && m_pCurBall->m_pPuppet) {
		m_pCurBall->m_pPuppet->ApplyBones(0, m_matrix90);
		m_pCurBall->m_pPuppet->Transform(0, 1.0f, 0);
		m_pCurBall->m_pPuppet->UpdateLightSource(&m_light, false, 0);
		m_pCurBall->m_pPuppet->Render(g_view, true, 1.0f, true, true, false);
	}

	if (GolfBall().m_preDataIndex < 0) {
		DrawRing();
	}

	m_fxBox.Display(g_view);

	g_view->Flush(0);
	g_view->SetCamera(g_camera);
	g_view->SetClip(oldNear, oldFar, false);
	g_view->SetFOV(oldFov);
	g_view->ResetScreenCenter();
	g_view->UpdateCamera();
}

// ground_info.cpp:204
// Draws the condition ring around the ball: one quad per step of the ring,
// textured with the lie icon of the power loss on the steps near the front
// and back (the first two of each half), with the plain line elsewhere.
void CBase::DrawRing(void) {
	int count;
	int powerLoss;

	g_view->SetScreenCenter(m_screenX, m_screenY);
	g_view->UpdateCamera();

	if (GolfClub().GetType() == 3) {
		count = 12;
		powerLoss = 0;
	} else {
		count = m_lieIconIndex;
		powerLoss = m_powerLoss;
	}

	for (int i = 0; i < count; i++) {
		for (int j = 0; j < 4; j++) {
			int step = (j & 1) + i;
			WVector pos;
			pos = RotMat((float)step * 6.2831855f / (float)count, 1).za * 0.17f;
			pos.y = (j & 2 ? -1.0f : 1.0f) * 0.05f;

			m_vertices[j].SetPosition(pos * GetBaseMatrix());
			m_vertices[j].diffuse = 0xFF00FF96;
		}

		if (i % (count / 2) < 2) {
			for (int j = 0; j < 4; j++) {
				m_vertices[j].tu = (powerLoss / 16) * 0.25f + (powerLoss == 0 ? 1.0f : 0.85f) * ((i % 2 ? 0.125f : 0.0f) + (j & 1 ? 1.0f : 0.0f) * 0.125f);
				m_vertices[j].tv = (float)((j & 2 ? 1 : 0) + powerLoss) * 0.0625f;
			}

			g_view->DrawPolygonFan(m_vertexList, m_hConditionFont & 0x7FF, 0, false);
		} else {
			for (int j = 0; j < 4; j++) {
				m_vertices[j].tu = j & 1 ? 1.0f : 0.0f;
				m_vertices[j].tv = j & 2 ? 0.0f : 1.0f;
			}

			g_view->DrawPolygonFan(m_vertexList, m_hConditionLine & 0x7FF, 0, false);
		}
	}

	g_view->ResetScreenCenter();
	g_view->UpdateCamera();
}

// ground_info.cpp:277
void CBase::SetNormal(const WVector& normal) {
	m_normal = normal;

	GolfBall().m_groundType = GetPVS().GetGroundType(GolfBall().m_pos, true, 0);
	GetPVS().SetCondition();

	if (GolfBall().m_groundType == 2) {
		GOLFDOC()->GetCameraData().m_pitch = -0.2268928f;
	} else {
		GOLFDOC()->GetCameraData().m_pitch = 0.0f;
		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holeIn[GOLFDOC()->m_currentHole - 1] = 0;
	}

	if (GolfBall().m_groundType == 2) {
		m_powerLoss = 0;
	} else {
		m_powerLoss = 100 - (int)(GetPVS().GetMaxPowerOnGround(GolfBall().m_pos) * 100.0f + 0.1f);
	}

	m_lieIconIndex = m_powerLoss ? 16 : 12;
}

// ground_info.cpp:303
WMatrix CBase::GetBaseMatrix(void) {
	WMatrix m;

	m.ya = m_normal; m.xa = WCrossProduct(m.ya, WVector::UNIT_POS_Z);
	m.za = WCrossProduct(m.xa, m.ya);

	m.xa.Normalize();
	m.za.Normalize();
	m.pivot.Reset();

	m = RotMat(m_unknownC0 * -2.0f, 1) * m * RotMat(m_angle * -1.0f, 1);

	return m;
}

// ground_info.cpp:321
CBase::sBall* CBase::ChangeBall(unsigned long typeId) {
	sBall* pBall = AddBall(typeId);

	m_fxBox.ClearAll(false);

	if (pBall) {
		m_pCurBall = pBall;
		AttachFx();
	}

	return pBall;
}

// ground_info.cpp:336
CBase::sBall* CBase::AddBall(unsigned long typeId) {
	sBall* pBall = FindBall(typeId);

	if (pBall)
		return pBall;

	IFF_STRUCT::sBall* pData = ItemManager()->FindBall(typeId);

	if (pData) {
		char path[40];
		pBall = new sBall;

		pBall->m_pData = pData;
		sprintf(path, "%s.pet", pData->m_model);
		pBall->m_pPuppet = g_resrcmng->GetPuppet(path, false, true, false);

		m_balls.push_back(pBall);
	}

	return pBall;
}

// ground_info.cpp:365
CBase::sBall* CBase::FindBall(unsigned long typeId) {
	std::list<sBall*>::iterator it = std::find_if(m_balls.begin(), m_balls.end(), sFindBall(typeId));

	if (it != m_balls.end()) {
		return *it;
	}
	return 0;
}

// ground_info.cpp:377
void CBase::ChangeTurn(void) {
	ChangeBall(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId);
}

// ground_info.cpp:382
// Attaches the current ball's effect (an "xxx.spr" spray or an "xxx.seq"
// sequence) to the bone the ball names, else to its bounding box ("center"
// by default). Balls of the listed types show no effect here.
void CBase::AttachFx(void) {
	if (!m_pCurBall || !m_pCurBall->m_pData || !m_pCurBall->m_pPuppet)
		return;

	switch (m_pCurBall->m_pData->m_typeId) {
	case 0x14000001:
	case 0x14000008:
	case 0x14000009:
	case 0x1400000E:
	case 0x1400003C:
	case 0x1400003D:
		return;
	}

	if (m_pCurBall->m_pData->m_fxName[0][0] == 0)
		return;

	int len = strlen(m_pCurBall->m_pData->m_fxName[0]);

	if (len < 5)
		return;

	Attacher attacher;

	attacher.m_fLimitA = m_screenX;
	attacher.m_fLimitB = m_screenY;

	WBone* pBone = m_pCurBall->m_pPuppet->FindBone(m_pCurBall->m_pData->m_fxBone[0]);

	if (pBone) {
		attacher.m_pMatrix[0] = &pBone->GetMatrix();
	} else {
		w_bound_box* pBox;
		if (m_pCurBall->m_pData->m_fxBone[0][0])
			pBox = m_pCurBall->m_pPuppet->m_bbList.Find(m_pCurBall->m_pData->m_fxBone[0]);
		else
			pBox = m_pCurBall->m_pPuppet->m_bbList.Find("center");

		if (pBox) {
			if (pBox->bone)
				attacher.m_pMatrix[0] = &pBox->bone->GetMatrix();

			attacher.m_pOffset[0] = &pBox->info->spherePivot;
		}
	}

	if (strcmpi(&m_pCurBall->m_pData->m_fxName[0][len - 3], "spr") == 0) {
		CFxSpray* pSpray = m_fxBox.LoadSpray(m_pCurBall->m_pData->m_fxName[0], NULL);
		if (pSpray == NULL)
			return;

		pSpray->m_pAttacher = &attacher;
		pSpray->m_fAttachLimitA = attacher.m_fLimitA;
		pSpray->m_fAttachLimitB = attacher.m_fLimitB;
		memcpy(pSpray->m_pMatrix, attacher.m_pMatrix, sizeof(pSpray->m_pMatrix));
		memcpy(pSpray->m_pOffset, attacher.m_pOffset, sizeof(pSpray->m_pOffset));

		m_fxBox.AddSpray(pSpray);
	} else {
		CFxSequence* pSequence = m_fxBox.LoadSequence(m_pCurBall->m_pData->m_fxName[0], true, NULL);
		if (pSequence == NULL)
			return;

		pSequence->m_pAttacher = &attacher;
		pSequence->m_fDistLimitA = attacher.m_fLimitA;
		pSequence->m_fDistLimitB = attacher.m_fLimitB;
		m_fxBox.AddSequence(pSequence);
	}
}
