// Original: d:\build\custom\temp\code\client\projectg\shotreview.cpp
#include "ShotReview.h"
#include "GolfDoc.h"
#include "SharedDoc.h"
#include "wview.h"
#include "ObjectFactory.h"
#include "TitleFontEx.h"
#include "InputManager.h"
#include "wresrcmng.h"
#include "woverlay.h"
#include "quadtree.h"
#include "golfball.h"
#include "commonutil.h"
#include <math.h>
#include "shared/globalgamedefine.h"
#include <algorithm>
#include <functional>

int WisZero(const float &value, float eps);

// shotreview.cpp:19
SpotHistory::SpotHistory() {
	m_pBarFont = new CBarFont;
	m_pBarFont->SetColor(0xFFFFFFFF);

	m_pIndicator1 = g_resrcmng->GetOverlay("[fall_indicator1.png", 0);
	m_pIndicator2 = g_resrcmng->GetOverlay("[fall_indicator2.png", 0);
}

// shotreview.cpp:28
SpotHistory::~SpotHistory() {
	if (g_resrcmng && m_pIndicator2) { g_resrcmng->Release(m_pIndicator2); m_pIndicator2 = 0; }
	if (g_resrcmng && m_pIndicator1) { g_resrcmng->Release(m_pIndicator1); m_pIndicator1 = 0; }
	if (m_pBarFont) { delete m_pBarFont; m_pBarFont = 0; }
}

// shotreview.cpp:35
void SpotHistory::Clear() {
	m_spots.clear();
}

// shotreview.cpp:40
// Draws each shot's start-to-landing line, the landing indicator and the
// shot's number; older shots fade out.
void SpotHistory::Display() {
	if (!(GOLFDOC()->GameMode() & 0x200)) {
		return;
	}

	if (m_spots.size() == 0) {
		return;
	}

	std::vector<float> alphas;
	alphas.reserve(m_spots.size());

	float alpha = 180.0f;
	for (unsigned int i = 0; i < m_spots.size(); ++i) {
		alphas.push_back(alpha);
		alpha = 0.0f > alpha - 20.0f ? 0.0f : alpha - 20.0f - 20.0f; // the original steps by 40 past the test
	}
	std::reverse(alphas.begin(), alphas.end());

	std::vector<std::pair<WVector, WVector> >::iterator it = m_spots.begin();
	int index = 0;
	for (; it != m_spots.end(); ++it) {
		unsigned long color = ((unsigned char)alphas[index] << 24) | 0xC8C8C8;

		int groundIndex;
		WVector start = GetPVS().GetGroundPoint(it->first, false, 0, false);
		WVector end = GetPVS().GetGroundPoint(it->second, true, &groundIndex, true);

		g_view->DrawLine2D(start, end, color, 1.0f);
		g_view->Flush(0);

		WTVertex vtx;
		g_view->Projection2(&vtx, it->second);

		WOverlay *pIndicator = groundIndex == -1 ? m_pIndicator2 : m_pIndicator1;
		int width = pIndicator->GetWidth();
		int height = pIndicator->GetHeight();
		{
			WRECT dst = { vtx.x - (width >> 1), vtx.y - (height >> 1), (float)width, (float)height };
			WRECT src = { 0.0f, 0.0f, 1.0f, 1.0f };
			pIndicator->Render(g_view, src, dst, 0, color, 0, 0);
		}

		m_pBarFont->Print(vtx.x, vtx.y - m_pBarFont->GetFontH() * 0.5f, MakeStr("%d", ++index), TITLEFONT_ALIGN_CENTER, 0, 1.0f);
	}
}

// shotreview.cpp:96
void SpotHistory::AddItem(unsigned int param1, unsigned int param2) {
	const WVector& start = *(const WVector*)param1;
	const WVector& end = *(const WVector*)param2;
	m_spots.push_back(std::pair<WVector, WVector>(start, end));
}

// shotreview.cpp:103
void SpotHistory::PopBack() {
	if (!m_spots.empty()) {
		m_spots.pop_back();
	}
}

// shotreview.cpp:114
BallTraceHistory::BallTraceHistory() {
}

// shotreview.cpp:118
BallTraceHistory::~BallTraceHistory() {
}

// shotreview.cpp:122
void BallTraceHistory::Clear() {
	m_traces.clear();
}

// shotreview.cpp:127
void BallTraceHistory::Display() {
	if (!(GOLFDOC()->GameMode() & 0x200)) {
		return;
	}
	if (m_traces.empty()) {
		return;
	}
	std::list<std::vector<WVector> >::iterator it = m_traces.begin();
	while (it != m_traces.end()) {
		std::vector<WVector>& trace = *it;
		if (!trace.empty()) {
			float alpha = 100.0f;
			float step = 140.0f / trace.size();
			for (unsigned int i = 0; i < trace.size() - 1; ++i) {
				g_view->DrawLine2D(trace[i], trace[i + 1], ((unsigned char)alpha << 24) | 0xC81414, 1.0f);
				alpha += step;
			}
		}
		++it;
	}
}

// shotreview.cpp:160
void BallTraceHistory::AddItem(unsigned int param1, unsigned int param2) {
	m_traces.push_back(*(const std::vector<WVector>*)param1);
}

// shotreview.cpp:166
void BallTraceHistory::PopBack() {
	if (!m_traces.empty()) {
		m_traces.pop_back();
	}
}

// shotreview.cpp:177
FirstBoundHistory::FirstBoundHistory() {
	m_pIndicator = g_resrcmng->GetOverlay("{indicator.bmp", 0);
	m_pBarFont = new CBarFont;
	m_pBarFont->SetColor(0xFFFFFFFF);
}

// shotreview.cpp:184
FirstBoundHistory::~FirstBoundHistory() {
	if (m_pBarFont) { delete m_pBarFont; m_pBarFont = 0; }
	if (g_resrcmng && m_pIndicator) { g_resrcmng->Release(m_pIndicator); m_pIndicator = 0; }
}

// shotreview.cpp:190
void FirstBoundHistory::Clear() {
	m_type0Points.clear();
	m_type1Points.clear();
}

// shotreview.cpp:196
void FirstBoundHistory::Display() {
	if (!(GOLFDOC()->GameMode() & 0x200)) {
		return;
	}
	std::vector<WVector>::iterator it;
	for (it = m_type1Points.begin(); it != m_type1Points.end(); ++it) {
		DrawIndicator(*it, 0xC8C8C8C8);
	}
	for (it = m_type0Points.begin(); it != m_type0Points.end(); ++it) {
		DrawIndicator(*it, 0xC8FF0000);
	}
}

// shotreview.cpp:210
void FirstBoundHistory::AddItem(unsigned int param1, unsigned int param2) {
	if (param1 == 0) {
		m_type0Points.push_back(*(const WVector*)param2);
	} else if (param1 == 1) {
		m_type1Points.push_back(*(const WVector*)param2);
	}
}

// shotreview.cpp:225
void FirstBoundHistory::PopBack() {
	if (!m_type0Points.empty()) {
		m_type0Points.pop_back();
	}
	if (!m_type1Points.empty()) {
		m_type1Points.pop_back();
	}
}

// shotreview.cpp:234
// Draws the indicator on the ground under pos and, above it, the distance
// from pos to the hole's cup.
void FirstBoundHistory::DrawIndicator(const WVector& pos, unsigned long color) {
	int groundIndex;
	WVector ground = GetPVS().GetGroundPoint(pos, true, &groundIndex, true);

	WTVertex vtx;
	g_view->Projection2(&vtx, ground);

	ground.x = vtx.x;
	ground.y = vtx.y;
	WRECT dst = { ground.x - 10.0f, ground.y - 20.0f, 20.0f, 20.0f };
	m_pIndicator->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), dst, 0, color, 0, 0);

	WVector ballPos = pos;

	WVector diff = ballPos - WVector(GOLFDOC()->GetHoleData().m_cupPos);
	float distance = (float)sqrt(diff.x * diff.x + diff.z * diff.z);
	m_pBarFont->Print(ground.x, ground.y - 40.0f - 3.0f, MakeStr("%.1fy", distance * 0.3125f), TITLEFONT_ALIGN_CENTER, 0, 1.0f);
}

// shotreview.cpp:271 (run-time type and factory registration of ShotReview)
const WRTTI ShotReview::m_RTTI("ShotReview", &IActor::m_RTTI);

IObject *ShotReviewMakeInstance(void) {
	return new ShotReview;
}

struct __impShotReview {
	__impShotReview(void) {
		ObjectFactory().AddObjectFunctor(ShotReviewMakeInstance, "ShotReview");
		ActorFactory().AddActorClass("ShotReview");
	}
};
static __impShotReview s_impShotReview;

// shotreview.cpp:274
ShotReview::ShotReview(void) {
	m_show = true; m_enabled = true; AddSkipList(0x1000);

	m_priority = 4;
	CSceneManager::Instance()->RegisterElement((eObjType)5, 0, this, "ShotReviewScene", 0, 0);

	m_histories.push_back(new SpotHistory);
	m_histories.push_back(new BallTraceHistory);
	m_histories.push_back(new FirstBoundHistory);
}

// shotreview.cpp:285
ShotReview::~ShotReview(void) {
	std::for_each(m_histories.begin(), m_histories.end(), Delete_Object());
}

// shotreview.cpp:291
void ShotReview::OnInit(void) {
	std::for_each(m_histories.begin(), m_histories.end(), std::mem_fun(&IReviewHistory::Clear));
}

// shotreview.cpp:296
void ShotReview::OnProcess(float dt) {
	if (g_input->Get("LCONTROL", true) && g_input->GetDown("REPLAY", true)) {
		m_show = !m_show;
	}
}

// shotreview.cpp:302
void ShotReview::Display(void) {
	if (m_show) {
		std::for_each(m_histories.begin(), m_histories.end(), std::mem_fun(&IReviewHistory::Display));
	}
}

// shotreview.cpp:308
void ShotReview::HandleMsg(const MsgObject &msg) {
	std::string name((const char *)msg.m_param1);

	if (name == "GET_VISIBLE_STATE") {
		*(bool *)msg.m_param2 = m_show;
		return;
	}

	if (name == "POP_BACK_ITEM") {
		std::for_each(m_histories.begin(), m_histories.end(), std::mem_fun(&IReviewHistory::PopBack));
		return;
	}

	if (name == "VISIBLE") {
		m_show = msg.m_param2 != 0;
		return;
	}

	if (name == "SAVE_SPOT_INDICATOR") {
		std::vector<IReviewHistory*>::iterator it = std::find_if(m_histories.begin(), m_histories.end(), ReviewHistoryFinder("SpotHistory"));
		if (it != m_histories.end()) {
			WVector start = GolfBall().m_pos;
			WVector spot;
			SendMsg(this, "Spot", 0xF4, (intptr_t)&spot, 0, 0, 0);
			(*it)->AddItem((unsigned int)&start, (unsigned int)&spot);
		}
		return;
	}

	if (name == "RECORD_REAL_BOUNCE_BALL") {
		if (!m_enabled) {
			return;
		}
		m_enabled = false;
		WVector realPos = *(const WVector *)msg.m_param2;
		int predicted;
		WVector predictedPos;
		SendMsg(this, "ShotPreview", -1, (intptr_t)"GET_PREDICTED_FIRSTBOUND_POS", (intptr_t)&predicted, (intptr_t)&predictedPos, 0);

		std::vector<IReviewHistory*>::iterator it = std::find_if(m_histories.begin(), m_histories.end(), ReviewHistoryFinder("FirstBoundHistory"));
		if (it != m_histories.end()) {
			(*it)->AddItem(0, (unsigned int)&realPos);
			if (predicted != 0) {
				(*it)->AddItem(1, (unsigned int)&predictedPos);
			}
		}
		return;
	}

	if (name == "SAVE_BALL_TRACE") {
		m_enabled = true;
		const CGolfBall *pBall = (const CGolfBall *)msg.m_param2;

		std::vector<const CGolfBall::sExpected*> points;
		points.reserve(pBall->m_preDataCount);
		for (int i = 0; i <= pBall->m_preDataCount; ++i) {
			points.push_back(pBall->PreData(i));
		}

		// Thin the points out to the limit: drop every second point that is
		// not on the ground, raising the limit for those kept.
		unsigned int limit = 20;
		while (points.size() > limit) {
			int count = 0;
			std::vector<const CGolfBall::sExpected*>::iterator it = points.begin();
			while (it != points.end()) {
				if ((++count & 1) == 0) {
					if ((*it)->m_groundType > 0) {
						++limit;
					} else {
						it = points.erase(it);
						continue;
					}
				}
				++it;
			}
		}

		if (!points.empty()) {
			const CGolfBall::sExpected *pFirst = points.front();
			const CGolfBall::sExpected *pLast = points.back();
			WVector start = pFirst->m_pos;
			WVector end = pLast->m_pos;
			float height = end.y - start.y;

			float prevHeight = start.y;
			// Horizontal offset from the first point to the last.
			WVector diff = end - start;
			diff.y = 0.0f;
			float distance = diff.Magnitude();
			if (WisZero(distance, 1e-5f)) {
				distance = 1.0f;
			}

			std::vector<WVector> trace;
			trace.reserve(points.size());

			for (std::vector<const CGolfBall::sExpected*>::iterator it = points.begin(); it != points.end(); ++it) {
				int groundIndex;
				WVector pos = GetPVS().GetGroundPoint((*it)->m_pos, true, &groundIndex, true);
				GetPVS().GetGroundType(pos, true, 0);

				if (groundIndex == -1 || fabs(pos.y - prevHeight) > 20.0f) {
					pos.y = WVector2D(pos.x - start.x, pos.z - start.z).Magnitude() / distance * height;
				}

				trace.push_back(pos);
				prevHeight = pos.y;
			}

			std::vector<IReviewHistory*>::iterator found = std::find_if(m_histories.begin(), m_histories.end(), ReviewHistoryFinder("BallTraceHistory"));
			if (found != m_histories.end()) {
				(*found)->AddItem((unsigned int)&trace, 0);
			}
		}
	}
}
