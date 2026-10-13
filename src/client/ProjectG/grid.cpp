// Original: d:\build\custom\temp\code\client\projectg\grid.cpp
#include "grid.h"
#include "ObjectFactory.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "club.h"
#include "golfball.h"
#include "quadtree.h"
#include "wresrcmng.h"
#include "wview.h"
#include "Localize.h"
#include "CapturedBg.h"

extern WMatrix g_camera;

// grid.cpp:18 (run-time type and factory registration of CGrid)
const WRTTI CGrid::m_RTTI("CGrid", &IActor::m_RTTI);

IObject* CGridMakeInstance(void) {
	return new CGrid;
}

struct __impCGrid {
	__impCGrid(void) {
		ObjectFactory().AddObjectFunctor(CGridMakeInstance, "CGrid");
		ActorFactory().AddActorClass("CGrid");
	}
};
static __impCGrid s_impCGrid;

// grid.cpp:27
CGrid::CGrid(void) : m_slopeTex(0), m_middleBlueTex(0) {
	m_priority = 1;
}

// grid.cpp:32
void CGrid::OnDestroy(void) {
	if (g_resrcmng && m_slopeTex) { g_resrcmng->Release(m_slopeTex); m_slopeTex = 0; }
	if (g_resrcmng && m_middleBlueTex) { g_resrcmng->Release(m_middleBlueTex); m_middleBlueTex = 0; }
}

// grid.cpp:38
void CGrid::OnPreLoadInit(void) {
	for (int i = 0; i < 4; i++) {
		m_vertices[i].tu = (i & 2) ? 1.0f : 0.0f;
		m_vertices[i].tv = (i & 1) ? 0.0f : 1.0f;
	}

	m_pQuad[0] = &m_vertices[0];
	m_pQuad[1] = &m_vertices[1];
	m_pQuad[2] = &m_vertices[3];
	m_pQuad[3] = &m_vertices[2];
	m_pQuad[4] = 0;
	m_angle = 0;
	m_unknown30b0 = -1.0f;
	m_unknown316c = false;

	Reset();

	CSceneManager::Instance()->RegisterElement((eObjType)1, 0, this, "\xb1\xd7\xb8\xae\xb5\xe5", 0, 0);
}

// grid.cpp:62
void CGrid::OnLoad(void) {
	m_slopeTex = g_resrcmng->LoadTexture("slope.jpg", 0, 0, 0);
	m_middleBlueTex = g_resrcmng->LoadTexture("middle_blue.jpg", 0, 0, 0);
}

// grid.cpp:68
void CGrid::OnReload(void) {
	m_unknown316c = false;

	Reset();
}

// grid.cpp:75
void CGrid::Reset(void) {
	m_lineTime = 0;
	m_lineColor = 0xFFFF0000;

	for (int i = 0; i < 21; i++) {
		for (int j = 0; j < 21; j++) {
			m_blocks[i][j].m_bUse = false;
		}
	}

	m_unknown3088 = false;
	m_unknown30ac = false;

	m_flag &= ~4;

	switch (GetCurMap()) {
	case 6:
		m_gridColor = 0xFF509E75;
		break;
	case 9:
		m_gridColor = 0xFF677943;
		break;
	case 11:
		m_gridColor = 0xFF3E7D30;
		break;
	case 13:
		m_gridColor = 0xFFFBB817;
		break;
	default:
		m_gridColor = 0xFF00562F;
		break;
	}
}

// grid.cpp:116
void CGrid::SetGridType(void) {
	sPlayerData *player = PLAYER(GOLFDOC()->m_myPlayerIndex);
	WVector cup = GOLFDOC()->GetHoleData().m_cupPos;
	WVector d;
	d.x = cup.x - player->m_ballPos.x;
	d.z = cup.z - player->m_ballPos.z;

	if (GolfBall().m_groundType != 2 && !(sqrtf(d.z * d.z + d.x * d.x) < 128.0f)) {
		m_flag |= 2;
		m_gridCountZ = 10;
		m_gridCountX = 11;
		m_blockSize = 12.8f;
		m_puttExtraCountZ = 0;
		m_puttExtraCountX = 0;
		m_puttExtraSize = 0.0f;
		MakeStaticGrid();
	} else {
		m_flag &= ~2;

		if (GolfBall().m_groundType == 2 && GolfClub().GetType() == 3 && !GolfClub().IsPowerPutter()) {
			m_gridCountZ = 10;
			m_gridCountX = 11;
			m_blockSize = 3.2f;
			m_puttExtraCountZ = -4;
			m_puttExtraCountX = 0;
			m_puttExtraSize = 3.2f;
		} else {
			m_gridCountZ = 8;
			m_gridCountX = 16;
			m_blockSize = 6.4f;
			m_puttExtraCountZ = -2;
			m_puttExtraCountX = 5;
			m_puttExtraSize = 0.0f;
		}
	}

	Reset();
}

// grid.cpp:164
void CGrid::OnProcess(float time) {
	if (GOLFDOC()->m_gameState < 4 || CCapturedBg::Instance()->IsCaptured())
		return;

	if (GolfClub().Updated())
		m_flag |= 0x10;

	if ((m_flag >> 2) & 1)
		return;

	WVector d;
	d.x = g_camera.pivot.x - m_center.x;
	d.z = g_camera.pivot.z - m_center.z;

	if (sqrtf(d.z * d.z + d.x * d.x) > 320.0f) {
		m_flag |= 1;
		return;
	}

	m_flag &= ~1;

	if (!((m_flag >> 1) & 1)) {
		if ((m_flag >> 4) & 1) {
			m_axisX = RotMat(m_angle, 1).za;
			m_axisZ = WCrossProduct(WVector::UNIT_POS_Y, m_axisX);

			MakeDynamicGrid();
			SetHoleGrid();
			MakeSlopeIndex();
			m_flag &= ~8;
		} else {
			m_flag |= 8;
		}
	}

	m_flag &= ~0x10;

	if (GolfBall().m_state)
		CheckGridPass();

	if (m_unknown316c)
		m_lineTime += time;
}

// grid.cpp:214
void CGrid::CheckGridPass(void) {
	if (m_unknown30ac) {
		const WVector &ball = GolfBall().m_pos;
		WVector d(ball.x - m_blocks[0][0].m_pos.x, 0.0f, ball.z - m_blocks[0][0].m_pos.z);

		float size = (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraSize : 0.0f) + m_blockSize;
		d /= size;
		int ix = (int)(d * m_axisX);
		int iz = (int)(d * m_axisZ);

		if (ix < m_gridCountX + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountX : 0) - 1
			&& iz < m_gridCountZ + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountZ : 0) - 1)
			m_blocks[ix][iz].m_bUse = true;
	}
}

// grid.cpp:231
void CGrid::Display(void) {
	int i, j, k;

	if (Skipped() || GOLFDOC()->m_gameState < 4 || CCapturedBg::Instance()->IsCaptured())
		return;

	if ((m_flag & 1) || ((m_flag >> 2) & 1))
		return;

	// The cells the ball passed over, as white quads.
	for (i = 0; i < m_gridCountX + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountX : 0) - 1; i++) {
		for (j = 0; j < m_gridCountZ + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountZ : 0) - 1; j++) {
			if (m_blocks[i][j].m_bUse) {
				for (k = 0; k < 4; k++) {
					WVector pos = m_blocks[i + (k & 1)][j + ((k & 2) ? 1 : 0)].m_pos;

					m_vertices[k].SetPosition(pos);

					m_vertices[k].diffuse = 0x80FFFFFF;
				}

				g_view->DrawPolygonFan(m_pQuad, 0x20000000, 0, false);
			}
		}
	}

	// The grid lines along z, then along x.
	for (i = 0; i < m_gridCountX + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountX : 0); i++) {
		for (j = 0; j < m_gridCountZ + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountZ : 0) - 1; j++) {
			g_view->DrawLine(m_blocks[i][j].m_pos, m_gridColor, m_blocks[i][j + 1].m_pos, m_gridColor, 0);
		}
	}

	for (i = 0; i < m_gridCountZ + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountZ : 0); i++) {
		for (j = 0; j < m_gridCountX + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountX : 0) - 1; j++) {
			g_view->DrawLine(m_blocks[j][i].m_pos, m_gridColor, m_blocks[j + 1][i].m_pos, m_gridColor, 0);
		}
	}

	// The slope markers, one small quad per cell.
	for (i = 0; i < m_gridCountX + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountX : 0) - 1; i++) {
		for (j = 0; j < m_gridCountZ + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountZ : 0) - 1; j++) {
			for (k = 0; k < 4; k++) {
				WVector pos = m_blocks[i][j].m_slopePos; pos.x += ((k & 2) ? 1.0f : -1.0f) * 0.2f;
				pos.z += ((k & 1) ? 1.0f : -1.0f) * 0.2f;

				m_vertices[k].SetPosition(pos);

				if (m_unknown316c) {
					if (m_lineTime > 0.5f) {
						m_lineTime = 0;
						m_lineColor = m_lineColor == 0xFFFF0000 ? m_gridColor : 0xFFFF0000;
					}
					m_vertices[k].diffuse = m_lineColor;
				} else {
					m_vertices[k].diffuse = m_blocks[i][j].m_bUse ? 0xFFFF0000 : m_gridColor;
				}
			}

			g_view->DrawPolygonFan(m_pQuad, (m_slopeTex & 0x7FF) | 0x20000000, 0, false);
		}
	}

	// The cell holding the cup.
	if (!((m_flag >> 1) & 1) && ((m_flag >> 3) & 1) && m_cupBlockX >= 0 && m_cupBlockZ >= 0) {
		m_vertices[0].SetPosition(m_blocks[m_cupBlockX][m_cupBlockZ].m_pos);

		m_vertices[0].diffuse = 0x5000FF00;
		m_vertices[1].SetPosition(m_blocks[m_cupBlockX + 1][m_cupBlockZ].m_pos);
		m_vertices[1].diffuse = 0x5000FF00;
		m_vertices[2].SetPosition(m_blocks[m_cupBlockX][m_cupBlockZ + 1].m_pos);
		m_vertices[2].diffuse = 0x5000FF00;
		m_vertices[3].SetPosition(m_blocks[m_cupBlockX + 1][m_cupBlockZ + 1].m_pos);
		m_vertices[3].diffuse = 0x5000FF00;
		g_view->DrawPolygonFan(m_pQuad, 0, 0x400, false);
	}
}

// grid.cpp:427
void CGrid::HandleMsg(const MsgObject &msg) {
	switch (msg.m_msg) {
	case 0x2718:
		m_angle = *(float*)msg.m_param1;
		SetGridType();
		m_flag |= 0x10;
		break;
	case 0x1A1:
		m_angle += *(float*)msg.m_param1;
		m_flag |= 0x10;
		break;
	case 0x74:
		if ((WVector*)msg.m_param1)
			m_center = *(WVector*)msg.m_param1;
		SetGridType();
		break;
	case 0x1D2:
		*(float*)msg.m_param1 = (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraSize : 0.0f) + m_blockSize;
		*(int*)msg.m_param2 = m_gridCountX + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountX : 0);
		break;
	case 0x163:
		m_flag &= ~4;
		break;
	case 0x164:
		if (!m_unknown30ac)
			m_flag |= 4;
		break;
	case 0x87:
		m_unknown30ac = msg.m_param1 != 0;
		break;
	case 0x1D3:
		m_unknown30b0 = msg.m_param1 * 3.2f;
		break;
	case 0x1D4:
		SetHoleGrid();
		break;
	case 0x207:
		if (IsLocalContent((localContentType_t)0x38)) {
			if (msg.m_param1) {
				m_unknown316c = true;
				m_lineTime = 0;
				m_lineColor = 0xFFFF0000;
			} else {
				m_unknown316c = false;
			}
		}
		break;
	}
}

// grid.cpp:495
void CGrid::MakeStaticGrid(void) {
	for (int i = 0; i < m_gridCountX; i++) {
		for (int j = 0; j < m_gridCountZ; j++) {
			WVector pos = WVector((j - m_gridCountZ / 2) * m_blockSize, 0.0f, (i - m_gridCountX / 2) * m_blockSize) + m_center;
			m_blocks[i][j].m_pos = GetPVS().GetGroundPoint(pos, true, 0, true) + WVector(0.0f, GolfBall().m_radius, 0.0f);
		}
	}

	m_axisX = WVector::UNIT_POS_Z;
	m_axisZ = WVector::UNIT_POS_X;
	MakeSlopeIndex();
}

// grid.cpp:514
void CGrid::MakeDynamicGrid(void) {
	for (int i = 0; i < m_gridCountX + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountX : 0); i++) {
		for (int j = 0; j < m_gridCountZ + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountZ : 0); j++) {
			WVector pos = ((strstr(GolfClub().GetName(), "1PT") ? m_puttExtraSize : 0.0f) + m_blockSize) * (m_axisX * (float)i + m_axisZ * ((float)j + 0.5f - (m_gridCountZ + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountZ : 0)) / 2)) + GolfBall().m_pos;
			m_blocks[i][j].m_pos = GetPVS().GetGroundPoint(pos, true, 0, true) + WVector(0.0f, GolfBall().m_radius, 0.0f);
		}
	}
}

// grid.cpp:533
int CGrid::GetColor(float slope) {
	float ratio = (slope > 0.0f ? slope : -slope) * 3.12499976f;
	if (ratio > 1.0f) ratio = 1.0f;

	if (slope > 0.0f)
		return ULblend(0xFFA1C400, 0xFFC6F2DE, (unsigned char)(ratio * 255.0f));

	return ULblend(0xFFA1C400, 0xFF00562F, (unsigned char)(ratio * 255.0f));
}

// grid.cpp:546
void CGrid::SetHoleGrid(void) {
	WVector cup = GOLFDOC()->GetHoleData().m_cupPos;
	const WVector &ball = GolfBall().m_pos;
	WVector d(cup.x - ball.x, 0.0f, cup.z - ball.z);

	float size = (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraSize : 0.0f) + m_blockSize;

	m_cupBlockX = (int)((d.x * m_axisX.x + d.z * m_axisX.z + d.y * m_axisX.y) * (1.0f / size));
	m_cupBlockZ = (m_gridCountZ + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountZ : 0)) / 2 + (int)floor((1.0f / size) * (d.x * m_axisZ.x + d.z * m_axisZ.z + d.y * m_axisZ.y) - 0.5f);

	if (m_cupBlockX > (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountX : 0) + m_gridCountX - 2)
		m_cupBlockX = -1;

	if (m_cupBlockZ > m_gridCountZ + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountZ : 0) - 2)
		m_cupBlockZ = -1;

	m_unknown3088 = true;

	float step;
	if (GolfClub().GetApproachMode() > 1) {
		step = 6.4f;
	} else {
		if (strstr(GolfClub().GetName(), "1PT"))
			step = 6.4f;
		else
			step = 3.2f;
	}

	int count = (int)(sqrtf(d.x * d.x + d.z * d.z) / step);
	int n = m_gridCountX + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountX : 0);
	int half = (int)((m_gridCountZ + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountZ : 0)) * 0.5f);

	if (GolfBall().m_groundType != 2 || GolfClub().GetType() != 3 || half < 1) {
		SendMsg(this, "Screen", 0x1AE, count, n - 1, 0, 0);
		return;
	}

	float *heights = new float[n];

	heights[0] = (m_blocks[0][half].m_pos.y + m_blocks[0][half - 1].m_pos.y) * 0.5f;

	for (int i = 1; i < n; i++) {
		heights[i] = (m_blocks[i][half].m_pos.y + m_blocks[i][half - 1].m_pos.y + m_blocks[i - 1][half - 1].m_pos.y + m_blocks[i - 1][half].m_pos.y) * 0.25f;
	}

	SendMsg(this, "Screen", 0x1AE, count, n - 1, (intptr_t)heights, 0);

	delete[] heights;
}

// grid.cpp:604
void CGrid::MoveLine(float delta) {
	m_linePos += delta;
}

// grid.cpp:610
void CGrid::MakeSlopeIndex(void) {
	float size = (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraSize : 0.0f) + m_blockSize;

	for (int i = 0; i < m_gridCountX + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountX : 0) - 1; i++) {
		for (int j = 0; j < m_gridCountZ + (strstr(GolfClub().GetName(), "1PT") ? m_puttExtraCountZ : 0) - 1; j++) {
			float inv = 1.0f / size;
			float slopeX = atanf((GetPVS().GetGreenPos(m_blocks[i][j].m_pos) - GetPVS().GetGreenPos(m_blocks[i + 1][j].m_pos)) * inv) * 8.0f;
			slopeX = (slopeX < -0.45f ? -0.45f : (slopeX > 0.45f ? 0.45f : slopeX)) + 0.5f;
			float slopeZ = atanf((GetPVS().GetGreenPos(m_blocks[i][j].m_pos) - GetPVS().GetGreenPos(m_blocks[i][j + 1].m_pos)) * inv) * 8.0f;
			slopeZ = (slopeZ < -0.45f ? -0.45f : (slopeZ > 0.45f ? 0.45f : slopeZ)) + 0.5f;

			m_blocks[i][j].m_slopePos = GetPVS().GetGroundPoint(m_blocks[i][j].m_pos + (m_axisX * slopeX + m_axisZ * slopeZ) * size, true, 0, true) + WVector(0.0f, GolfBall().m_radius, 0.0f);
		}
	}
}

// grid.cpp:629
float CGrid::GetSlope(int x, int y, float dist, bool alongX) {
	sGridBlock *block = &m_blocks[x][y];

	if (alongX) {
		return atanf((GetPVS().GetGreenPos(block->m_pos) - GetPVS().GetGreenPos(m_blocks[x + 1][y].m_pos)) / dist) * 8.0f;
	}
	return atanf((GetPVS().GetGreenPos(block->m_pos) - GetPVS().GetGreenPos(m_blocks[x][y + 1].m_pos)) / dist) * 8.0f;
}
