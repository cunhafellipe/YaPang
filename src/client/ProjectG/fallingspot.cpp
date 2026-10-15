// Original: d:\build\custom\temp\code\client\projectg\fallingspot.cpp
// actor.h names IActor in MsgObject before declaring it.
class IActor;

#include "fallingspot.h"

#include <new>
#include <math.h>

#include "ObjectFactory.h"
#include "3dfont.h"
#include "golfball.h"
#include "wview.h"
#include "wresrcmng.h"
#include "woverlay.h"
#include "fontinfo.h"
#include "SimpleOverlay.h"
#include "club.h"
#include "quadtree.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "ChatMsg.h"
#include "CapturedBg.h"
#include "InputManager.h"
#include "ProjectG.h"
#include "mousecursor.h"
#include "commonutil.h"
#include "wpuppet.h"
#include "wutil.h"
#include "wtypes.h"

extern WMatrix g_camera;
extern LightSet g_lightset;

float Random(float min, float max, int seed);

// fallingspot.cpp:28 (run-time type and factory registration of CSpot)
const WRTTI CSpot::m_RTTI("CSpot", &IActor::m_RTTI);

// The indicator colours by height difference, red (below) to green (above).
const unsigned long CSpot::m_adwColors[5] = { 0xffff0000, 0xffff8a00, 0xffffde00, 0xff95fb21, 0xff02ff42 };

// Turn rate (radians per second) of the range quad and the hole indicator.
static const float s_turnRate = 3.1415927f;

IObject* CSpotMakeInstance(void) {
	return new CSpot;
}

struct __impCSpot {
	__impCSpot(void) {
		ObjectFactory().AddObjectFunctor(CSpotMakeInstance, "CSpot");
		ActorFactory().AddActorClass("CSpot");
	}
};
static __impCSpot s_impCSpot;

// fallingspot.cpp:43
CSpot::CSpot(void) : m_pFont1c(NULL), m_pFont20(NULL), m_pRangeInfo(NULL), m_hTexture9c(0), m_hTexturea0(0), m_hTexturea4(0), m_hTexturea8(0), m_pOverlayc4(NULL), m_pOverlayc8(NULL), m_pSimpleUI(NULL), m_hTextured4(0), m_pRivals(NULL), m_pMaskedFont(NULL) {
	AddSkipList(0x1400);

	m_priority = 1;
	m_pVertices[0] = &m_vertices[0];
	m_pVertices[1] = &m_vertices[1];
	m_pVertices[2] = &m_vertices[3];
	m_pVertices[3] = &m_vertices[2];
	m_pVertices[4] = NULL;
	m_pOverlaybc = NULL;
	m_pOverlayc0 = NULL;
}

// fallingspot.cpp:59
void CSpot::OnPreLoadInit(void) {
	m_bRegistered = true;
	CSceneManager::Instance()->RegisterElement((eObjType)5, 0, this, "\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1", 0, 0);
}

// fallingspot.cpp:66
void CSpot::OnDestroy(void) {
	if (g_resrcmng != NULL && m_pOverlayc4 != NULL) { g_resrcmng->Release(m_pOverlayc4); m_pOverlayc4 = NULL; }
	if (g_resrcmng != NULL && m_pOverlaybc != NULL) { g_resrcmng->Release(m_pOverlaybc); m_pOverlaybc = NULL; }
	if (g_resrcmng != NULL && m_pOverlayc0 != NULL) { g_resrcmng->Release(m_pOverlayc0); m_pOverlayc0 = NULL; }
	if (g_resrcmng != NULL && m_hTexture9c != 0) { g_resrcmng->Release(m_hTexture9c); m_hTexture9c = 0; }
	if (g_resrcmng != NULL && m_hTexturea0 != 0) { g_resrcmng->Release(m_hTexturea0); m_hTexturea0 = 0; }
	if (g_resrcmng != NULL && m_hTexturea4 != 0) { g_resrcmng->Release(m_hTexturea4); m_hTexturea4 = 0; }
	if (g_resrcmng != NULL && m_pOverlayc8 != NULL) { g_resrcmng->Release(m_pOverlayc8); m_pOverlayc8 = NULL; }
	if (g_resrcmng != NULL && m_hTextured4 != 0) { g_resrcmng->Release(m_hTextured4); m_hTextured4 = 0; }
	if (g_resrcmng != NULL && m_hTexturea8 != 0) { g_resrcmng->Release(m_hTexturea8); m_hTexturea8 = 0; }

	if (m_pSimpleUI != NULL) { delete m_pSimpleUI; m_pSimpleUI = NULL; }
	if (m_pFont20 != NULL) { delete m_pFont20; m_pFont20 = NULL; }
	if (m_pFont1c != NULL) { delete m_pFont1c; m_pFont1c = NULL; }
	if (m_pRangeInfo != NULL) { delete m_pRangeInfo; m_pRangeInfo = NULL; }

	if (m_pRivals != NULL) { delete[] m_pRivals; m_pRivals = NULL; }
}

// fallingspot.cpp:86
void CSpot::OnLoad(void) {
	m_pRangeInfo = new CClubRangeInfo;

	m_pFont1c = new C3dFont;
	m_pFont1c->LoadTexture("[font_wind.jpg", 16, 16, "1234567890ym%-.?/:", 3.0f, -30.0f);

	m_pFont20 = new C3dFont;
	m_pFont20->LoadTexture("[nplayer.jpg", 32, 16, "0123rb", 0.0f, -30.0f);

	m_pOverlaybc = g_resrcmng->GetOverlay("[fall_indicator1.png", 0);
	m_pOverlayc0 = g_resrcmng->GetOverlay("[fall_indicator2.png", 0);
	m_pOverlayc4 = g_resrcmng->GetOverlay("spot_line.bmp", 0);
	m_pOverlayc8 = g_resrcmng->GetOverlay("{player_pos.bmp", 0);

	switch (CSharedDoc::Instance()->m_roomType) {
	case 0:
	case 3:
	case 7:
		m_pSimpleUI = new CSimpleUI("[nplayer.jpg");
		m_pSimpleUI->Add("1", WRect(0.0f, 0.0f, 32.0f, 16.0f), false);
		m_pSimpleUI->Add("2", WRect(32.0f, 0.0f, 32.0f, 16.0f), false);
		m_pSimpleUI->Add("3", WRect(0.0f, 16.0f, 32.0f, 16.0f), false);
		m_pSimpleUI->Add("4", WRect(32.0f, 16.0f, 32.0f, 16.0f), false);
		break;

	case 1:
		m_pSimpleUI = new CSimpleUI("[nplayer.jpg");
		m_pSimpleUI->Add("r", WRect(0.0f, 32.0f, 32.0f, 16.0f), false);
		m_pSimpleUI->Add("b", WRect(32.0f, 32.0f, 32.0f, 16.0f), false);
		break;

	case 6:
		m_pRivals = new sRivalPos[CSharedDoc::Instance()->m_gameResultList.size()];

		for (unsigned char i = 0; i < CSharedDoc::Instance()->m_gameResultList.size(); i++) {
			if (CSharedDoc::Instance()->GetPangyaUserInfo().guildUID == CSharedDoc::Instance()->m_gameResultList[i].m_hasEmblem) {
				m_pRivals[i].m_speed = 0.0f;
				m_pRivals[i].m_fade = -1.0f;
				m_pRivals[i].m_hole = CSharedDoc::Instance()->m_holeOrder[0];
				m_pRivals[i].m_pos = GOLFDOC()->GetHoleData().m_teePos;
			}
		}
		break;

	case 4:
	case 5:
	case 9:
	case 10:
		m_pRivals = new sRivalPos[CSharedDoc::Instance()->m_gameResultList.size()];

		for (unsigned char i = 0; i < CSharedDoc::Instance()->m_gameResultList.size(); i++) {
			m_pRivals[i].m_speed = 0.0f;

			if (CSharedDoc::Instance()->m_roomType == 10) {
				m_pRivals[i].m_fade = 0.0f;
			}

			m_pRivals[i].m_hole = CSharedDoc::Instance()->m_holeOrder[0];
			m_pRivals[i].m_pos = CSharedDoc::Instance()->m_gameResultList[i].m_ballPos;
		}
		break;
	}

	m_hTexture9c = g_resrcmng->LoadTexture("{indicator.bmp", 0, 0, 0);
	m_hTexturea0 = g_resrcmng->LoadTexture("[falling.jpg", 0, 0, 0);
	m_hTexturea4 = g_resrcmng->LoadTexture("[slope.jpg", 0, 0, 0);
	m_hTexturea8 = g_resrcmng->LoadTexture("[target_hole_posi.jpg", 0, 0, 0);

	if (GOLFDOC()->m_gameState < 15) {
		m_hTextured4 = g_resrcmng->LoadTexture("[score_target.jpg", 0, 0, 0);
	}

	m_pMaskedFont = CChatMsg::Instance()->GetMaskedFont();
}

// fallingspot.cpp:163
void CSpot::OnInit(void) {
	m_unknown78 = 0;
	m_angle = 0.0f;
	m_rangeRotation = 0;
	m_spotPos.Reset();
	m_unknown68.Reset();
	m_unknown30.Reset();

	m_unknown80 = true;
	m_unknown18c = true;
	m_unknown4c = 1.0f;
	m_unknown84 = 2.0f;
	m_unknown88 = 12.0f;
	m_unknown94 = 20.0f;
	m_unknown98 = 20.0f;
	m_unknown74 = 0.0f;
	m_rangeSpread = 0;
	m_unknownd0 = 0;

	m_unknownb4 = 0;
	m_unknownb8 = 1.0f;

	m_unknown50.Reset();
	float angle = Random(-3.1415927f, 3.1415927f, -1);
	m_dir = Random(0.0f, 0.02f * (GolfClub().GetRange(0) * 3.2f), -1) * WVector(cosf(angle), 0.0f, sinf(angle));
}

// fallingspot.cpp:196
void CSpot::OnProcess(float elapsed) {
	if (CCapturedBg::Instance()->IsCaptured()) {
		return;
	}

	if (!GolfClub().IsCalcRandomPower()) {
		if (CSharedDoc::Instance()->m_roomType == 3 && IsMyTurn(false) && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId != 0x1800000c && GolfClub().PowerShotType() != 0 && (GolfClub().GetType() == 0 || GolfClub().GetType() == 1)) {
			if (!GOLFDOC()->m_clockHold) {
				m_unknown74 += elapsed * 8.0f;
			}

			if (m_unknown74 > 1.0f) {
				m_unknown74 = 0.0f;
				m_spotPos = m_unknown68 + m_dir;
				m_unknown50 = m_dir;

				WVector dir;
				dir = m_unknown68 - GolfBall().m_pos;
				dir.y = 0.0f;
				dir.Normalize();

				WVector side;
				side = WCrossProduct(dir, WVector::UNIT_POS_Y);
				side.y = 0.0f;
				side.Normalize();

				if (rand() % 10 == 0 && GolfClub().GetType() == 0) {
					m_dir = dir * Random(1.0f, 1.5f, -1) + side * Random(-1.0f, 1.0f, -1);
				} else {
					m_dir = dir * Random(-1.0f, 1.0f, -1) + side * Random(-1.0f, 1.0f, -1);
				}
				m_dir *= 0.02f * (GolfClub().GetRange(0) * 3.2f);
			} else {
				m_spotPos = GetPVS().GetGroundPoint(m_unknown68 + (1.0f - m_unknown74) * m_unknown50 + m_unknown74 * m_dir, true, &m_unknown78, true);
			}

			WVector toSpot = m_spotPos - GolfBall().m_pos;
			toSpot.y = 0.0f;
			GolfClub().SetRandomPower(toSpot.Magnitude() / 3.2f - GolfClub().GetRange(0));
		} else {
			m_unknown74 = 0.0f; m_spotPos = m_unknown68;
			GolfClub().SetRandomPower(0.0f);
		}
	} else {
		m_unknown74 = 0.0f;
	}

	if (GOLFDOC()->m_target.m_unk10) {
		m_unknownd0 += elapsed * s_turnRate;
		if (m_unknownd0 > 6.2831855f) {
			m_unknownd0 -= 6.2831855f;
		}
	}

	if (GolfClub().Updated()) {
		m_unknown18c = true;
	}

	if (!g_input->Get("LCONTROL", true) && !g_input->Get("RCONTROL", true) && g_input->GetDown("HIDE", true)) {
		m_bRegistered = !m_bRegistered;
	}

	switch (CSharedDoc::Instance()->m_roomType) {
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
		for (unsigned char i = 0; i < CSharedDoc::Instance()->m_gameResultList.size(); i++) {
			if (CSharedDoc::Instance()->m_gameResultList[i].m_state == 0 && m_pRivals[i].m_hole == GOLFDOC()->m_currentHole) {
				if (m_pRivals[i].m_fade < 1.0f) {
					m_pRivals[i].m_fade += elapsed * m_pRivals[i].m_speed;
				}

				if (CSharedDoc::Instance()->m_roomType == 10) {
					if (i == MyIndex()) {
						if (GolfBall().m_preDataIndex >= 0 && GolfBall().m_preDataIndex < GolfBall().m_preDataCount) {
							WVector dir;
							dir = GetHoleCupPos() - GolfBall().m_pos;
							dir.y = 0.0f;
							CSharedDoc::Instance()->m_gameResultList[i].m_approachDistanceCopy = (int)floorf(0.3125f * (WVectorLen(dir) * 10.0f));
						}
					} else if (m_pRivals[i].m_fade > 1.0f) {
						m_pRivals[i].m_fade = 1.0f;
						CSharedDoc::Instance()->m_gameResultList[i].m_approachDistanceCopy = CSharedDoc::Instance()->m_gameResultList[i].m_approachDistance;
						SendMsg(this, "Score", 0x2a, 0, 0, 0, 0);
					} else if (m_pRivals[i].m_fade < 1.0f) {
						float rate = GetMovingRate(m_pRivals[i].m_fade);
						WVector dir;
						dir = GetHoleCupPos() - ((1.0f - rate) * m_pRivals[i].m_pos + rate * CSharedDoc::Instance()->m_gameResultList[i].m_ballPos);
						dir.y = 0.0f;
						CSharedDoc::Instance()->m_gameResultList[i].m_approachDistanceCopy = (int)floorf(0.3125f * (WVectorLen(dir) * 10.0f));
					}
				} else if (m_pRivals[i].m_fade > 1.0f) {
					m_pRivals[i].m_fade = 1.0f;
					WVector dir;
					dir = CSharedDoc::Instance()->m_gameResultList[i].m_ballPos - GOLFDOC()->GetHoleData(m_pRivals[i].m_hole).m_cupPos;
					dir.y = 0.0f;
					if (sqrtf(dir.z * dir.z + dir.x * dir.x) < GolfBall().GetRadius() * 2.0f) {
						m_pRivals[i].m_fade = -1.0f;
						m_pRivals[i].m_hole = CSharedDoc::Instance()->m_holeOrder[GetHoleIndex(m_pRivals[i].m_hole)];
					}
				}
			}
		}
		break;
	}

	if (!m_unknown80) {
		return;
	}

	m_rangeRotation += elapsed * s_turnRate;
	if (m_rangeRotation > 6.2831855f) {
		m_rangeRotation -= 6.2831855f;
	}

	m_rangeSpread += elapsed * 0.5f;
	if (m_rangeSpread > 1.0f) {
		m_rangeSpread = 0.0f;
	}

	if (m_unknown18c && WisEqual(m_unknown4c, 1.0f, 0.00001f)) {
		if (GolfClub().Updated()) {
			m_rangeSpread = 0.0f;
		}
		m_unknown18c = false;
		Rotate();
	}

	if (GolfClub().GetType() != 3 && WisEqual(m_unknown4c, 1.0f, 0.00001f)) {
		m_unknownb4 += elapsed * m_unknownb8 * 2.0f;
		if (m_unknownb4 > 1.0f) {
			m_unknownb4 = 1.0f;
			m_unknownb8 = -1.0f;
		} else if (m_unknownb4 < 0.0f) {
			m_unknownb4 = 0.0f;
			m_unknownb8 = 1.0f;
		}

		float t = m_unknownb4;
		m_groundPower = GetPVS().GetMinPowerOnGround() * (1.0f - t * t) + GetPVS().GetMaxPowerOnGround() * (t * t);
	} else {
		m_groundPower = 1.0f;
	}
}

// fallingspot.cpp:381
void CSpot::Display(void) {
	if (Skipped() || CCapturedBg::Instance()->IsCaptured()) {
		return;
	}

	WVector pos;
	if (GOLFDOC()->m_target.m_unk10) {
		switch (GOLFDOC()->m_gameState) {
		case 2:
		case 6:
		case 11:
		case 10:
		case 13:
		case 14: {
			float size = GOLFDOC()->m_gameState == 2 ? 2.0f : 1.0f;

			for (int i = 0; i < 4; i++) {
				WVector p = GOLFDOC()->m_target.m_pos;
				p.y += 1.0f;
				p.x += ((i & 1) ? 1.0f : -1.0f) * size * 44.8f;
				p.z += ((i & 2) ? 1.0f : -1.0f) * size * 44.8f;
				m_vertices[i].SetPosition(p);
				m_vertices[i].tu = (i & 1) ? 1.0f : 0.0f;
				m_vertices[i].tv = (i & 2) ? 0.0f : 1.0f;
				m_vertices[i].diffuse = 0xffffffff;
			}

			g_view->DrawPolygonFan(m_pVertices, m_hTextured4 & 0x7ff, 0x400, false);
			break;
		}
		default: {
			WMatrix m;
			m = RotMat(m_unknownd0, 1);
			m *= 0.5f;
			// A plain three-dword copy in the original, not WVector::operator=.
			memcpy(&m.pivot, &GOLFDOC()->m_target.m_pos, sizeof(WVector));

			GOLFDOC()->m_target.m_cameraTarget->ApplyBones(NULL, m);
			GOLFDOC()->m_target.m_cameraTarget->Transform(NULL, 1.0f, 0);
			GOLFDOC()->m_target.m_cameraTarget->UpdateLightSource(&g_lightset, false, NULL);
			GOLFDOC()->m_target.m_cameraTarget->Render(g_view, true, 1.0f, false, true, false);
			break;
		}
		}
	}

	if (m_bRegistered) {
		if (GOLFDOC()->GameMode() & 0x200) {
			switch (CSharedDoc::Instance()->m_roomType) {
			case 0:
			case 3:
			case 7:
				for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
					if (GOLFDOC()->GetPlayer(i)->m_state == 0 && i != GOLFDOC()->m_myPlayerIndex && PLAYER(i)->m_holes[GOLFDOC()->m_currentHole - 1] != 0) {
						pos = g_view->Projection(PLAYER(i)->m_ballPos);
						if (0.0f < pos.z && pos.z < 1.0f) {
							m_pOverlayc8->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(pos.x - 4.0f, pos.y - 4.0f, 8.0f, 8.0f), 0, PLAYER(i)->m_color | 0x80000000, 0.0f, 0);
							unsigned long color = PLAYER(i)->m_color;
							m_pSimpleUI->SetColor(color | 0x80000000);
							m_pSimpleUI->Render(MakeStr("%d", i + 1), pos.x, pos.y - 12.0f, 1.0f, 0x22);
						}
					}
				}
				break;
			case 1:
				for (unsigned char i = 0; i < 2; i++) {
					if (TEAM(i)->m_state == 0 && i != PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute && TEAM(i)->m_holes[GOLFDOC()->m_currentHole - 1] != 0) {
						pos = g_view->Projection(TEAM(i)->m_ballPos);
						if (0.0f < pos.z && pos.z < 1.0f) {
							m_pOverlayc8->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(pos.x - 4.0f, pos.y - 4.0f, 8.0f, 8.0f), 0, TEAM(i)->m_color | 0x80000000, 0.0f, 0);
							m_pSimpleUI->SetColor(TEAM(i)->m_color | 0x80000000);
							m_pSimpleUI->Render(i == 0 ? "r" : "b", pos.x, pos.y - 12.0f, 1.0f, 0x22);
						}
					}
				}
				break;
			case 6:
				if (!CProjectG::Instance()->HidePI()) {
					for (unsigned char i = 0; i < CSharedDoc::Instance()->m_gameResultList.size(); i++) {
						if (CSharedDoc::Instance()->m_gameResultList[i].m_state == 0 && CSharedDoc::Instance()->m_gameResultList[i].m_uid != MyGuid(true) && m_pRivals[i].m_hole == GOLFDOC()->m_currentHole && !(m_pRivals[i].m_fade < 0.0f) && CSharedDoc::Instance()->m_userInfo.guildUID == CSharedDoc::Instance()->m_gameResultList[i].m_hasEmblem) {
							float t = m_pRivals[i].m_fade;
							pos = g_view->Projection(GetPVS().GetGroundPoint((1.0f - GetMovingRate(t)) * m_pRivals[i].m_pos + GetMovingRate(t) * CSharedDoc::Instance()->m_gameResultList[i].m_ballPos, false, NULL, true));
							if (0.0f < pos.z && pos.z < 1.0f) {
								unsigned long color = CSharedDoc::Instance()->m_gameResultList[i].m_team ? 0x800060ff : 0x80ff0000;
								m_pOverlayc8->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(pos.x - 4.0f, pos.y - 4.0f, 8.0f, 8.0f), 0, color, 0.0f, 0);
								m_pMaskedFont->Print(g_view, pos.x - m_pMaskedFont->GetTextWidth(g_view, CSharedDoc::Instance()->m_gameResultList[i].m_nickname) * 0.5f, pos.y - 15.0f, CSharedDoc::Instance()->m_gameResultList[i].m_nickname, 0, 0xffffffff, NULL);
								m_pMaskedFont->Flush(g_view);
							}
						}
					}
				}
				break;
			case 4:
			case 5:
			case 9:
			case 10:
				if (!CProjectG::Instance()->HidePI()) {
					for (unsigned char i = 0; i < CSharedDoc::Instance()->m_gameResultList.size(); i++) {
						if (CSharedDoc::Instance()->m_gameResultList[i].m_state == 0 && CSharedDoc::Instance()->m_gameResultList[i].m_uid != MyGuid(true) && m_pRivals[i].m_hole == GOLFDOC()->m_currentHole && !(m_pRivals[i].m_fade < 0.0f)) {
							float t = m_pRivals[i].m_fade;
							pos = g_view->Projection(GetPVS().GetGroundPoint((1.0f - GetMovingRate(t)) * m_pRivals[i].m_pos + GetMovingRate(t) * CSharedDoc::Instance()->m_gameResultList[i].m_ballPos, false, NULL, true));
							if (0.0f < pos.z && pos.z < 1.0f) {
								unsigned long color;
								if (CSharedDoc::Instance()->m_roomType == 5) {
									color = CSharedDoc::Instance()->m_gameResultList[i].m_team ? 0x800060ff : 0x80ff0000;
								} else if (CSharedDoc::Instance()->m_roomType == 10) {
									color = CSharedDoc::Instance()->m_gameResultList[i].m_approachDistanceCopy != 0xffffffff ? 0x80ffffff : 0x80505050;
								} else {
									color = 0x80ffffff;
								}
								m_pOverlayc8->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(pos.x - 4.0f, pos.y - 4.0f, 8.0f, 8.0f), 0, color, 0.0f, 0);

								if (CSharedDoc::Instance()->m_roomType == 10) {
									if (!CSharedDoc::Instance()->IsCurrentMissionBlind() || (CSharedDoc::Instance()->IsCurrentMissionBlind() && CSharedDoc::Instance()->m_gameResultList[i].m_uid == MyGuid(false))) {
										m_pMaskedFont->Print(g_view, pos.x - m_pMaskedFont->GetTextWidth(g_view, CSharedDoc::Instance()->m_gameResultList[i].m_nickname) * 0.5f, pos.y - 15.0f, CSharedDoc::Instance()->m_gameResultList[i].m_nickname, 0, 0xffffffff, NULL);
									} else {
										continue;
									}
								} else {
									m_pMaskedFont->Print(g_view, pos.x - m_pMaskedFont->GetTextWidth(g_view, CSharedDoc::Instance()->m_gameResultList[i].m_nickname) * 0.5f, pos.y - 15.0f, CSharedDoc::Instance()->m_gameResultList[i].m_nickname, 0, 0xffffffff, NULL);
								}
								m_pMaskedFont->Flush(g_view);
							}
						}
					}
				}
				break;
			}
		} else if (GOLFDOC()->GameMode() & 0x8dca) {
			switch (CSharedDoc::Instance()->m_roomType) {
			case 0:
			case 3:
			case 7:
				for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
					if (GOLFDOC()->GetPlayer(i)->m_state == 0 && i != GOLFDOC()->m_myPlayerIndex && PLAYER(i)->m_holes[GOLFDOC()->m_currentHole - 1] != 0) {
						for (unsigned char j = 0; j < 4; j++) {
							WVector p = PLAYER(i)->m_ballPos;
							p.x += ((j & 1) ? 1.0f : -1.0f) * 0.5f;
							p.z += ((j & 2) ? 1.0f : -1.0f) * 0.5f;
							m_vertices[j].SetPosition(p);
							m_vertices[j].diffuse = PLAYER(i)->m_color | 0x80000000;
							m_vertices[j].tu = (j & 1) ? 1.0f : 0.0f;
							m_vertices[j].tv = (j & 2) ? 0.0f : 1.0f;
						}

						g_view->DrawPolygonFan(m_pVertices, m_hTexturea4 & 0x7ff, 0x400, false);
						DrawIndicator(PLAYER(i)->m_ballPos, PLAYER(i)->m_color | 0x80000000);
						if (!CProjectG::Instance()->HidePI()) {
							m_pFont20->SetPos(PLAYER(i)->m_ballPos);
							m_pFont20->SetColor(PLAYER(i)->m_color | 0x80000000);
							m_pFont20->Printf(g_view, 0, "%d", i);
						}
					}
				}
				break;
			case 1:
				for (unsigned char i = 0; i < 2; i++) {
					if (TEAM(i)->m_state == 0 && i != PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute && TEAM(i)->m_holes[GOLFDOC()->m_currentHole - 1] != 0) {
						for (unsigned char j = 0; j < 4; j++) {
							WVector p;
							p = TEAM(i)->m_ballPos;
							p.x += ((j & 1) ? 1.0f : -1.0f) * 0.5f;
							p.z += ((j & 2) ? 1.0f : -1.0f) * 0.5f;
							m_vertices[j].SetPosition(p);
							m_vertices[j].diffuse = TEAM(i)->m_color | 0x80000000;
							m_vertices[j].tu = (j & 1) ? 1.0f : 0.0f;
							m_vertices[j].tv = (j & 2) ? 0.0f : 1.0f;
						}

						g_view->DrawPolygonFan(m_pVertices, m_hTexturea4 & 0x7ff, 0x400, false);
						DrawIndicator(TEAM(i)->m_ballPos, TEAM(i)->m_color | 0x80000000);
						if (!CProjectG::Instance()->HidePI()) {
							m_pFont20->SetPos(TEAM(i)->m_ballPos);
							m_pFont20->SetColor(TEAM(i)->m_color | 0x80000000);
							m_pFont20->Printf(g_view, 0, i == 0 ? "r" : "b");
						}
					}
				}
				break;
			case 6:
				if (!CProjectG::Instance()->HidePI()) {
					for (unsigned char i = 0; i < CSharedDoc::Instance()->m_gameResultList.size(); i++) {
						if (CSharedDoc::Instance()->m_gameResultList[i].m_state == 0 && CSharedDoc::Instance()->m_gameResultList[i].m_uid != MyGuid(true) && m_pRivals[i].m_hole == GOLFDOC()->m_currentHole && !(m_pRivals[i].m_fade < 0.0f) && CSharedDoc::Instance()->m_userInfo.guildUID == CSharedDoc::Instance()->m_gameResultList[i].m_hasEmblem) {
							float t = m_pRivals[i].m_fade;
							WVector ground = GetPVS().GetGroundPoint((1.0f - GetMovingRate(t)) * m_pRivals[i].m_pos + GetMovingRate(t) * CSharedDoc::Instance()->m_gameResultList[i].m_ballPos, false, NULL, true);
							if (g_view->InFrustum(ground)) {
								unsigned long color = CSharedDoc::Instance()->m_gameResultList[i].m_team ? 0x800060ff : 0x80ff0000;
								WVector p;
								for (unsigned char j = 0; j < 4; j++) {
									p = ground;
									p.x += ((j & 1) ? 1.0f : -1.0f) * 0.5f;
									p.y += GolfBall().m_radius;
									p.z += ((j & 2) ? 1.0f : -1.0f) * 0.5f;
									m_vertices[j].SetPosition(p);
									m_vertices[j].diffuse = color;
									m_vertices[j].tu = (j & 1) ? 1.0f : 0.0f;
									m_vertices[j].tv = (j & 2) ? 0.0f : 1.0f;
								}

								g_view->DrawPolygonFan(m_pVertices, m_hTexturea4 & 0x7ff, 0x400, false);
								p = ground;
								p.y += GolfBall().m_radius;
								DrawIndicator(p, color);
								pos = g_view->Projection(p);
								m_pMaskedFont->Print(g_view, pos.x - m_pMaskedFont->GetTextWidth(g_view, CSharedDoc::Instance()->m_gameResultList[i].m_nickname) * 0.5f, pos.y - 32.0f, CSharedDoc::Instance()->m_gameResultList[i].m_nickname, 0, 0xffffffff, NULL);
								m_pMaskedFont->Flush(g_view);
							}
						}
					}
				}
				break;
			case 4:
			case 5:
			case 9:
			case 10:
				if (!CProjectG::Instance()->HidePI()) {
					for (unsigned char i = 0; i < CSharedDoc::Instance()->m_gameResultList.size(); i++) {
						if (CSharedDoc::Instance()->m_gameResultList[i].m_state == 0 && m_pRivals[i].m_hole == GOLFDOC()->m_currentHole && !(m_pRivals[i].m_fade < 0.0f)) {
							if (CSharedDoc::Instance()->m_gameResultList[i].m_uid != MyGuid(true) || (CSharedDoc::Instance()->m_roomType == 10 && (GOLFDOC()->GameMode() & 0x800))) {
								float t = m_pRivals[i].m_fade;
								WVector ground = GetPVS().GetGroundPoint((1.0f - GetMovingRate(t)) * m_pRivals[i].m_pos + GetMovingRate(t) * CSharedDoc::Instance()->m_gameResultList[i].m_ballPos, false, NULL, true);
								if (g_view->InFrustum(ground)) {
									unsigned long color;
									if (CSharedDoc::Instance()->m_roomType == 5) {
										color = CSharedDoc::Instance()->m_gameResultList[i].m_team ? 0x800060ff : 0x80ff0000;
									} else if (CSharedDoc::Instance()->m_roomType == 10) {
										if (CSharedDoc::Instance()->m_gameResultList[i].m_uid == MyGuid(true)) {
											color = 0x80ff0000;
										} else {
											color = CSharedDoc::Instance()->m_gameResultList[i].m_approachDistanceCopy != 0xffffffff ? 0x80ffffff : 0x80505050;
										}
									} else {
										color = 0x80ffffff;
									}

									WVector p;
									for (unsigned char j = 0; j < 4; j++) {
										p = ground;
										p.x += ((j & 1) ? 1.0f : -1.0f) * 0.5f;
										p.y += GolfBall().m_radius;
										p.z += ((j & 2) ? 1.0f : -1.0f) * 0.5f;
										m_vertices[j].SetPosition(p);
										m_vertices[j].diffuse = color;
										m_vertices[j].tu = (j & 1) ? 1.0f : 0.0f;
										m_vertices[j].tv = (j & 2) ? 0.0f : 1.0f;
									}

									g_view->DrawPolygonFan(m_pVertices, m_hTexturea4 & 0x7ff, 0x400, false);
									p = ground;
									p.y += GolfBall().m_radius;
									DrawIndicator(p, color);
									pos = g_view->Projection(p);

									if (CSharedDoc::Instance()->m_roomType == 10) {
										if (CSharedDoc::Instance()->IsCurrentMissionBlind() && (!CSharedDoc::Instance()->IsCurrentMissionBlind() || CSharedDoc::Instance()->m_gameResultList[i].m_uid != MyGuid(false))) {
											continue;
										}
									}
									m_pMaskedFont->Print(g_view, pos.x - m_pMaskedFont->GetTextWidth(g_view, CSharedDoc::Instance()->m_gameResultList[i].m_nickname) * 0.5f, pos.y - 32.0f, CSharedDoc::Instance()->m_gameResultList[i].m_nickname, 0, 0xffffffff, NULL);
									m_pMaskedFont->Flush(g_view);
								}
							}
						}
					}
				}
				break;
			}
		}
	}

	if (!m_unknown80 || GOLFDOC()->m_gameState < 1) {
		return;
	}

	WVector d = GolfBall().m_pos - m_spotPos;
	if (m_unknown78 == -1) {
		m_indicatorColor = 0xffff0000;
	} else {
		float level = (Between(-4.0f, 0.9144f * (d.y / 3.2f), 4.0f) + 4.0f) * 0.5f;
		int index = (int)level;
		float frac = level - index;
		if (WAbsBySign(frac) < 0.00001f) {
			m_indicatorColor = m_adwColors[index];
		} else {
			m_indicatorColor = ULblend(m_adwColors[index], m_adwColors[index + 1], (unsigned char)(int)(frac * 255.0f));
		}
	}

	if (GOLFDOC()->GameMode() & 0x200) {
		if (IsMyTurn(false) && !WisEqual(m_unknown4c, 1.0f, 0.00001f)) {
			pos = m_unknown30 * (1.0f - m_unknown4c) + m_spotPos * m_unknown4c;
			g_view->Projection2(&m_vertices[0], pos);
			m_vertices[0].sx = g_view->GetWidth() * 0.5f;
			WVector screen = g_view->Projection(m_impactPos);
			CMouseCursor::Instance()->ForceMove(screen.x, screen.y, -1.0f);
		} else {
			g_view->Projection2(&m_vertices[0], m_spotPos);
		}

		g_view->Projection2(&m_vertices[1], GolfBall().m_pos);
		if (IsMyTurn(false) && m_vertices[0].sy > m_vertices[1].sy) {
			m_vertices[0].sy *= -1.0f;
		}

		if (_ValidTopViewLineDraw() == true) {
			float x1 = m_vertices[1].sx;
			float y1 = m_vertices[1].sy;
			pos.y = m_vertices[0].sy;
			WVector2D mid = m_groundPower * WVector2D(m_vertices[0].sx, m_vertices[0].sy) + (1.0f - m_groundPower) * WVector2D(m_vertices[1].sx, m_vertices[1].sy);
			float angle = atan2f(m_vertices[0].sy - m_vertices[1].sy, m_vertices[0].sx - m_vertices[1].sx) + 1.5707964f;
			float dx = m_vertices[0].sx - x1;
			float dy = pos.y - y1;
			float len = sqrtf(dy * dy + dx * dx);
			m_pOverlayc4->RenderWithAxis(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(mid.x - m_unknown84 * 0.5f, mid.y, m_unknown84, len * m_groundPower), mid.x, mid.y, angle, 0, m_indicatorColor, 0);
			m_pOverlayc4->RenderWithAxis(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(m_vertices[0].sx - m_unknown84 * 0.5f, m_vertices[0].sy, m_unknown84, len * (1.0f - m_groundPower)), m_vertices[0].sx, m_vertices[0].sy, angle, 0, 0xa0000000, 0);
		}

		WRect dst(0.0f, 0.0f, 1.0f, 1.0f);
		WRect src(m_vertices[0].sx - m_unknown88 * 0.5f, m_vertices[0].sy - m_unknown88 * 0.5f, m_unknown88, m_unknown88);
		if (m_unknown78 == -1) {
			m_pOverlayc0->Render(g_view, dst, src, 0, m_indicatorColor, 0.0f, 0);
		} else {
			m_pOverlaybc->Render(g_view, dst, src, 0, m_indicatorColor, 0.0f, 0);
			DrawHoleIndicator();
			DrawRange();
		}

		if (WisEqual(m_unknown4c, 1.0f, 0.00001f)) {
			m_pRangeInfo->m_color = m_indicatorColor;
			m_pRangeInfo->SetPos(m_spotPos);
			m_pRangeInfo->Display();
		}
	} else if (m_unknown78 != -1) {
		DrawIndicator(m_spotPos, m_indicatorColor);
		m_pFont1c->SetColor(m_indicatorColor);
		m_pFont1c->SetPos(m_spotPos);
		m_pFont1c->Printf(g_view, -5, "%.0fy", sqrtf(d.z * d.z + d.x * d.x) / 3.2f);
		DrawHoleIndicator();
		DrawRange();
	}
}

// fallingspot.cpp:1082
bool CSpot::_ValidTopViewLineDraw(void) {
	if (g_view->InFrustum(GolfBall().m_pos) || g_view->InFrustum(m_spotPos)) {
		return true;
	}

	WVector cam;
	cam.x = g_camera.pivot.x; cam.z = g_camera.pivot.z;
	WVector2D ball(GolfBall().m_pos.x, GolfBall().m_pos.z);

	WVector spot;
	spot.x = m_spotPos.x; spot.z = m_spotPos.z;
	WVector toCam;
	toCam.x = cam.x - ball.x; toCam.z = cam.z - ball.y;
	WVector2D toSpot(spot.x - ball.x, spot.z - ball.y);
	if (sqrtf(toCam.x * toCam.x + toCam.z * toCam.z) > sqrtf(toSpot.x * toSpot.x + toSpot.y * toSpot.y)) {
		return false;
	}

	return true;
}

// fallingspot.cpp:1101
void CSpot::DrawRange(void) {
	if (GolfClub().GetType() == 3 || CSharedDoc::Instance()->m_roomType == 3) {
		return;
	}

	float dist = GolfClub().GetRange(0) * 3.2f;
	float range = dist * 0.05f;
	for (int i = 0; i < 4; i++) {
		WVector pos;
		pos.x = ((i & 2) ? 1.0f : -1.0f) * m_rangeSpread * range;
		pos.y = range * 0.1f;
		pos.z = ((i & 1) ? 1.0f : -1.0f) * m_rangeSpread * range;
		pos = pos * RotMat(m_rangeRotation * 0.7f, 1);
		pos += m_spotPos;

		m_vertices[i].tu = (i & 2) ? 1.0f : 0.0f;
		m_vertices[i].tv = (i & 1) ? 0.0f : 1.0f;
		m_vertices[i].diffuse = ((int)((1.0f - m_rangeSpread) * 255.0f) << 24) | 0xffffff;
		m_vertices[i].SetPosition(pos);
	}

	g_view->DrawPolygonFan(m_pVertices, (m_hTexturea0 & 0x7ff) | 0xc00000, 0x400, false);
}

// fallingspot.cpp:1145
void CSpot::HandleMsg(const MsgObject& msg) {
	switch (msg.m_msg) {
	case 0x1a1:
		m_angle += *(float*)msg.m_param1;
		m_unknown18c = true;
		break;

	case 0x2718:
		m_angle = *(float*)msg.m_param1;
		m_unknown18c = true;
		break;

	case 0xfa:
		*(float*)msg.m_param1 = CalcDeltaAngle(m_spotPos - GolfBall().m_pos, m_unknown68 - GolfBall().m_pos);
		break;

	case 0xf4:
		*(WVector*)msg.m_param1 = m_spotPos;
		break;

	case 0x190:
		m_unknown80 = msg.m_param1 == 1;
		break;

	case 0x163:
		m_unknown80 = true;
		break;

	case 0x164:
		m_unknown80 = false;
		break;

	case 0xf5:
		m_unknown18c = true;
		break;

	case 0xf6: {
		float angle = *(float*)msg.m_param1;

		m_impactPos = *(WVector*)msg.m_param2;
		m_unknown30 = m_spotPos;
		m_unknown68 = GolfBall().m_pos + GolfClub().GetRange(0) * 3.2f * RotMat(angle, 1).za;
		m_unknown4c = 0.0f;
		m_rangeSpread = 0.4f;
		break;
	}

	case 0xf7:
		m_unknown4c = *(float*)msg.m_param1;
		break;

	case 0xf8:
		if (CSharedDoc::Instance()->m_roomType == 10) {
			m_pRivals[msg.m_param1].m_hole = (unsigned char)msg.m_param2;
		}

		if (m_pRivals[msg.m_param1].m_hole == msg.m_param2) {
			m_pRivals[msg.m_param1].m_speed = 50.0f / (float)msg.m_param3;
			m_pRivals[msg.m_param1].m_fade = 0.0f;
			m_pRivals[msg.m_param1].m_pos = CSharedDoc::Instance()->m_gameResultList[msg.m_param1].m_ballPos;

			if (CSharedDoc::Instance()->m_roomType == 10 && CSharedDoc::Instance()->m_gameResultList[msg.m_param1].m_approachPoint == -1) {
				m_pRivals[msg.m_param1].m_fade = 1.0f;
			}
		}
		break;

	case 0xf9: {
		unsigned char hole = (unsigned char)msg.m_param2;
		if (IsValidHole(hole)) {
			m_pRivals[msg.m_param1].m_hole = hole;
			m_pRivals[msg.m_param1].m_fade = -1.0f;
			m_pRivals[msg.m_param1].m_pos = GOLFDOC()->GetHoleData(m_pRivals[msg.m_param1].m_hole).m_teePos;
			CSharedDoc::Instance()->m_gameResultList[msg.m_param1].m_ballPos = m_pRivals[msg.m_param1].m_pos;
		}
		break;
	}

	case 0xfb:
		UpdateRivals();
		break;
	}
}

// fallingspot.cpp:1240
void CSpot::Rotate(void) {
	m_unknown78 = 0;
	WVector pos = GolfClub().GetRange(0) * 3.2f * RotMat(m_angle, 1).za + GolfBall().m_pos;

	m_unknown68 = GetPVS().GetGroundPoint(pos, true, &m_unknown78, true);

	float power = m_unknown74;
	if (WAbsBySign(power) < 0.00001f) {
		m_spotPos = m_unknown68;
	}
}

// fallingspot.cpp:1253, the four-vertex arrow at the landing point pointing
// at the hole cup; it fades in with the distance to the cup and out with the
// camera's distance.
void CSpot::DrawHoleIndicator(void) {
	WVector cup = GOLFDOC()->GetHoleData().m_cupPos;
	WVector dir = cup - m_spotPos;
	dir.y = 0.0f;
	WVector side(dir.z, 0.0f, dir.x * -1.0f);

	float cupAlpha = (sqrtf(dir.z * dir.z + dir.x * dir.x) - 32.0f) * 0.015625f;
	if (cupAlpha < 0.0f) {
		cupAlpha = 0.0f;
	} else if (cupAlpha > 1.0f) {
		cupAlpha = 1.0f;
	}
	cupAlpha *= 229.5f;

	float camAlpha = 1.0f - (g_camera.pivot - m_spotPos).Magnitude() * 0.0015625f;
	if (camAlpha < 0.0f) {
		camAlpha = 0.0f;
	} else if (camAlpha > 1.0f) {
		camAlpha = 1.0f;
	}
	camAlpha *= 229.5f;

	dir.Normalize();
	side.Normalize();

	for (int i = 0; i < 4; i++) {
		WVector pos = m_spotPos;
		pos.y += 2.0f;
		float sideSign = 1.0f;
		if (!(i & 2)) {
			sideSign = -1.0f;
		}
		float dirSign = (i & 1) ? 1.0f : -1.0f;
		m_vertices[i].SetPosition(pos + (dir * dirSign + side * sideSign) * 10.0f);
		m_vertices[i].tu = (i & 2) ? 1.0f : 0.0f;
		m_vertices[i].tv = (i & 1) ? 0.0f : 1.0f;
		m_vertices[i].diffuse = ((unsigned char)(int)(cupAlpha > camAlpha ? camAlpha : cupAlpha) << 24) | (m_indicatorColor & 0xffffff);
	}

	g_view->DrawPolygonFan(m_pVertices, m_hTexturea8 & 0x7ff, 0x400, false);
}

// fallingspot.cpp:1296
void CSpot::DrawIndicator(const WVector& pos, int color) {
	WTVertex vtx;
	g_view->Projection2(&vtx, pos);

	if (vtx.sz <= 0.0f || vtx.sz >= 1.0f) {
		return;
	}

	for (int i = 0; i < 4; i++) {
		m_vertices[i].sz = vtx.sz;
		m_vertices[i].rhw = vtx.rhw;
		m_vertices[i].diffuse = color;
	}

	m_vertices[0].sx = vtx.sx - m_unknown94 * 0.5f;
	m_vertices[0].tu = 0.0f;
	m_vertices[0].tv = 1.0f;
	m_vertices[0].sy = vtx.sy;

	m_vertices[1].sx = vtx.sx - m_unknown94 * 0.5f;
	m_vertices[1].sy = vtx.sy - m_unknown98;
	m_vertices[1].tu = 0.0f;
	m_vertices[1].tv = 0.0f;

	m_vertices[2].sx = vtx.sx + m_unknown94 * 0.5f;
	m_vertices[2].sy = vtx.sy;
	m_vertices[2].tu = 1.0f;
	m_vertices[2].tv = 1.0f;

	m_vertices[3].sx = vtx.sx + m_unknown94 * 0.5f;
	m_vertices[3].sy = vtx.sy - m_unknown98;
	m_vertices[3].tu = 1.0f;
	m_vertices[3].tv = 0.0f;

	if (m_vertices[0].sx > g_view->GetWidth() - 0.5f || m_vertices[2].sx < 0.5f || vtx.sy < 0.5f || g_view->GetHeight() - 0.5f < m_vertices[1].sy) {
		return;
	}

	for (int j = 0; j < 4; j++) {
		ClipIndicator(j);
	}

	g_view->DrawProjPolygonFan(m_pVertices, (m_hTexture9c & 0x7ff) | 0x2400000, 0);
}

// fallingspot.cpp:1356
void CSpot::ClipIndicator(int index) {
	if (m_vertices[index].sx < 0.5f) {
		float d = (0.5f - m_vertices[index].sx) / m_unknown94;
		m_vertices[index].sx = 0.5f; m_vertices[index].tu += d;
	} else if (g_view->GetWidth() - 0.5f < m_vertices[index].sx) {
		m_vertices[index].tu -= (m_vertices[index].sx - g_view->GetWidth() + 0.5f) / m_unknown94;
		m_vertices[index].sx = g_view->GetWidth() - 0.5f;
	}

	if (m_vertices[index].sy < 0.5f) {
		float d = (0.5f - m_vertices[index].sy) / m_unknown98;
		m_vertices[index].sy = 0.5f; m_vertices[index].tv += d;
	} else if (g_view->GetHeight() - 0.5f < m_vertices[index].sy) {
		m_vertices[index].tv -= (m_vertices[index].sy - g_view->GetHeight() + 0.5f) / m_unknown98;
		m_vertices[index].sy = g_view->GetHeight() - 0.5f;
	}
}

// fallingspot.cpp:1381
float CSpot::GetMovingRate(float t) {
	return (3.0f - 2.0f * t) * t * t;
}

// fallingspot.cpp:1387
bool CSpot::UpdateRivals(void) {
	if (m_pRivals) {
		delete[] m_pRivals;
		m_pRivals = NULL;
	}

	int count = CSharedDoc::Instance()->m_gameResultList.size();
	if (count > 0) {
		m_pRivals = new sRivalPos[count];

		for (unsigned char i = 0; i < count; i++) {
			m_pRivals[i].m_speed = 0;

			if (CSharedDoc::Instance()->m_roomType == 10) {
				m_pRivals[i].m_fade = 0;
			}

			m_pRivals[i].m_hole = CSharedDoc::Instance()->m_gameResultList[i].m_hole;
			m_pRivals[i].m_pos = CSharedDoc::Instance()->m_gameResultList[i].m_ballPos;
		}
	} else {
		return false;
	}

	return true;
}
