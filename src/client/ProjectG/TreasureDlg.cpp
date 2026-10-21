// Original: d:\build\custom\temp\code\client\projectg\treasuredlg.cpp
// Emits the out-of-line CTHunter::GetRepositorySize copy (see Treasure.h).
#define TREASUREDLG_TREASURE_INLINE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "TreasureDlg.h"
#include "ObjectFactory.h"
#include "Treasure.h"
#include "frarea.h"
#include "frbutton.h"
#include "frcombobox.h"
#include "frdesktop.h"
#include "frlistbox.h"
#include "frgaugebar.h"
#include "frviewer.h"
#include "frgraphicinterface.h"
#include "bitmap.h"
#include "fresh.h"
#include "wview.h"
#include "client/Wangreal/include/woverlay.h"
#include "client/Wangreal/include/wresrcmng.h"
#include "client/Wangreal/include/wfont.h"
#include "SimpleOverlay.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "commonutil.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "shared/ItemManager.h"
#include "shared/classdefine.h"
#include "shared/globalgamedefine.h"
#include "Packet.h"
#include "TaskManager.h"
#include "frwndmanager.h"
#include "client/Wangreal/include/wutil.h"
#include "UccManager.h"

float Random(float min, float max, int seed);   // commonutil.cpp:1106

extern "C" unsigned long data_a94af8;   // grid line color of FrTreasureGift (0xffa2d7fd)

// treasuredlg.cpp:39
bool nsTHunter::IsWndSizeMinimum(void) {
	WRect rect = g_pFresh->GetDesktop()->GetRect();
	if (rect.w == 640.0f && rect.h == 480.0f) {
		return true;
	}
	return false;
}

// treasuredlg.cpp:49
unsigned long nsTHunter::CalcGauge(unsigned long value) {
	return value < 701 ? 0 : value - 700;
}

// treasuredlg.cpp:56
bool CalculateUVPos(WRect& rc, int width, int height, float cellW, float cellH, int index) {
	if ((float)width < cellW || (float)height < cellH)
		return false;

	int rows = (int)((float)height / cellH);
	int cols = (int)((float)width / cellW);

	if (cols * rows < index)
		return false;

	float invW = 1.0f / (float)width;
	int col = index / rows;
	int row = index % rows;

	rc.x = invW * (col * cellW);
	float invH = 1.0f / (float)height;
	rc.y = invH * (row * cellH);
	rc.w = invW * cellW;
	rc.h = invH * cellH;
	return true;
}

// treasuredlg.cpp:89 (run-time type and factory registration of FrNextHole)
const WRTTI FrNextHole::m_RTTI("FrNextHole", &FrForm::m_RTTI);

IObject* FrNextHoleMakeInstance(void) {
	return new FrNextHole;
}

struct __sFrNextHole {
	__sFrNextHole(void) {
		ObjectFactory().AddObjectFunctor(FrNextHoleMakeInstance, "FrNextHole");
	}
};
static __sFrNextHole s_registerFrNextHole;

// treasuredlg.cpp:91
const sFRESH_MSGMAP* FrNextHole::GetMessageMap(void) const {
	return &FrNextHole::_MsgMap;
}

sFRESH_MSGMAP FrNextHole::_MsgMap = { &FrForm::_MsgMap, &FrNextHole::_MsgEntries[0] };

// treasuredlg.cpp:92
sFRESH_ENTRY FrNextHole::_MsgEntries[] = {
	{ "next", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrNextHole::OnAreaNextHole_Init },
	{ "next", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrNextHole::OnAreaNextHole_OwnerDraw },

	{ "outback1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrNextHole::OnArea_OutFrame_Init },
	{ "outback2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrNextHole::OnArea_OutFrame_Init },
	{ "outback3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrNextHole::OnArea_OutFrame_Init },

	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// treasuredlg.cpp:103
FrNextHole::FrNextHole(void) {
	m_pOverlay[0] = 0;
	m_pOverlay[1] = 0;
	m_pNextHoleArea = 0;
	m_overlayIdx = 0;
	m_state = STATE_NONE;
	m_pOverlay[2] = 0;
}

// treasuredlg.cpp:114
FrNextHole::~FrNextHole(void) {
	if (g_resrcmng && m_pOverlay[1]) { g_resrcmng->Release(m_pOverlay[1]); m_pOverlay[1] = 0; }
	if (g_resrcmng && m_pOverlay[0]) { g_resrcmng->Release(m_pOverlay[0]); m_pOverlay[0] = 0; }

	if (g_resrcmng && m_pOverlay[2]) { g_resrcmng->Release(m_pOverlay[2]); m_pOverlay[2] = 0; }
}

// treasuredlg.cpp:122
void FrNextHole::OnAreaNextHole_Init(intptr_t param) {
	m_pNextHoleArea = DYNAMIC_CAST(FrArea, param);
}

// treasuredlg.cpp:127
bool FrNextHole::OnInit(void) {
	FrForm::OnInit();

	IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(((GetCurMap() > 0x7F && GetCurMap() != 0xFD) ? GetCurMap() - 0x80 : GetCurMap()) | 0x28000000);
	if (pCourse == 0)
		return false;

	m_pOverlay[0] = g_resrcmng->GetOverlay(MakeStr("%s_1.png", pCourse->m_name), 0);
	m_pOverlay[1] = g_resrcmng->GetOverlay(MakeStr("%s_2.png", pCourse->m_name), 0);

	m_pOverlay[2] = g_resrcmng->GetOverlay("next_chaos.tga", 0);

	if (m_pOverlay[0] != 0 || m_pOverlay[1] != 0) {

		m_overlayWidth = m_pOverlay[0]->GetWidth();
		m_overlayHeight = m_pOverlay[0]->GetHeight();
	}






	m_outFrames.reserve(3);


	MoveWindow(GetCalcPoint());

	return true;
}

// treasuredlg.cpp:157
void FrNextHole::SetNextHole(int hole) {
	if (m_pOverlay[0] == 0 || m_pOverlay[1] == 0)
		return;

	if (hole > 10 || hole == 19 || CSharedDoc::Instance()->m_holeOrder[CSharedDoc::Instance()->m_holes - 1] == GOLFDOC()->m_currentHole) {
		m_overlayIdx = 1;
		hole -= 10;
	} else {
		m_overlayIdx = 0;
	}

	if (hole == 19 || CSharedDoc::Instance()->m_holeOrder[CSharedDoc::Instance()->m_holes - 1] == GOLFDOC()->m_currentHole) {
		CalculateUVPos(m_drawRect, m_overlayWidth, m_overlayHeight, 140.0f, 93.0f, 8);
	} else {
		CalculateUVPos(m_drawRect, m_overlayWidth, m_overlayHeight, 140.0f, 93.0f, hole - 1);
	}
}

// treasuredlg.cpp:191
void FrNextHole::OnAreaNextHole_OwnerDraw(PG_MSGARG param) {
	if (m_state == STATE_HIDE)
		return;

	WRect rect = m_pNextHoleArea->GetRect();

	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomMode == 14) {
		m_overlayIdx = 2;
		m_drawRect.x = 0.0f;
		m_drawRect.y = 0.0f;
		m_drawRect.w = 1.0f;
		m_drawRect.h = 1.0f;
	}

	if (m_pOverlay[m_overlayIdx]) {
		m_pOverlay[m_overlayIdx]->Render(g_view, m_drawRect, WRect(rect.x, rect.y, 140.0f, 93.0f), 0, 0xffffffff, 0, 0);
	}
}

// treasuredlg.cpp:210
void FrNextHole::OnArea_OutFrame_Init(intptr_t param) {
	FrArea* pArea = DYNAMIC_CAST(FrArea, param);
	if (pArea) {
		m_outFrames.push_back(pArea);
	}
}

// treasuredlg.cpp:223
void FrNextHole::SetState(eState state) {
	if (m_state == state) {
		if (IsEnabled()) {
			if (m_state == STATE_HIDE) {
				Enable(false);
				SetVisible(false);
			}
		}
		return;
	}

	m_state = state;
	if (state == STATE_HIDE) {
		Enable(false);
		SetVisible(false);
		return;
	}

	Enable(true);
	MoveWindow(GetCalcPoint());
	SetVisible(true);

	if (nsTHunter::IsWndSizeMinimum() == true)
		SetOutFrameVisible(false);
	else
		SetOutFrameVisible(true);

	SetNextHole(GetNextHole());
}

// treasuredlg.cpp:266
WPoint FrNextHole::GetCalcPoint(void) {
	WRect rect = m_rect;

	float offset = nsTHunter::IsWndSizeMinimum() == true ? 6.0f : -38.0f;

	WPoint pt;
	pt.x = (float)(int)((g_view->GetWidth() - rect.w) * 0.5f) + 210.0f;
	pt.y = (float)(int)(g_view->GetHeight() - rect.h + offset);

	return pt;
}

// treasuredlg.cpp:282
void FrNextHole::SetOutFrameVisible(bool bVisible) {
	int count = m_outFrames.size();
	for (int i = 0; i < count; ++i) {
		if (m_outFrames[i]) {
			m_outFrames[i]->SetVisible(bVisible);
		}
	}
}

// treasuredlg.cpp:296 (run-time type and factory registration of FrTreasureGauge)
const WRTTI FrTreasureGauge::m_RTTI("FrTreasureGauge", &FrForm::m_RTTI);

IObject* FrTreasureGaugeMakeInstance(void) {
	return new FrTreasureGauge;
}

struct __sFrTreasureGauge {
	__sFrTreasureGauge(void) {
		ObjectFactory().AddObjectFunctor(FrTreasureGaugeMakeInstance, "FrTreasureGauge");
	}
};
static __sFrTreasureGauge s_registerFrTreasureGauge;

// treasuredlg.cpp:298
const sFRESH_MSGMAP* FrTreasureGauge::GetMessageMap(void) const {
	return &FrTreasureGauge::_MsgMap;
}

sFRESH_MSGMAP FrTreasureGauge::_MsgMap = { &FrForm::_MsgMap, &FrTreasureGauge::_MsgEntries[0] };

// treasuredlg.cpp:300
sFRESH_ENTRY FrTreasureGauge::_MsgEntries[] = {
	{ "pointgauge", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGauge::OnImageBarInit },
	{ "gaugearrow", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGauge::OnArea_Arrow_Init },
	{ "gaugearrow", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGauge::OnArea_Arrow_OwnerDraw },

	{ "outback1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGauge::OnArea_OutFrame_Init },
	{ "outback2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGauge::OnArea_OutFrame_Init },
	{ "outback3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGauge::OnArea_OutFrame_Init },

	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// treasuredlg.cpp:311
FrTreasureGauge::FrTreasureGauge(void) {
	m_pGauge = 0;
	m_pArrowArea = 0;
	m_state = STATE_NONE;
}

// treasuredlg.cpp:318
FrTreasureGauge::~FrTreasureGauge(void) {
}

// treasuredlg.cpp:323
void FrTreasureGauge::OnImageBarInit(PG_MSGARG param) {
	m_pGauge = DYNAMIC_CAST(FrGaugeBarImage, param);
	if (m_pGauge) {
		m_pGauge->SetRange(0, 1000, 0);
		m_pGauge->SetBarSpeed(250.0f);
	}
}

// treasuredlg.cpp:333
bool FrTreasureGauge::OnInit(void) {
	m_pArrowBitmap = g_pFresh->RegisterBitmap("th_gage_arrow");

	m_outFrames.reserve(3);

	MoveWindow(GetCalcPoint());

	return true;
}

// treasuredlg.cpp:349
void FrTreasureGauge::SetTPoint(void) {
	unsigned long point = CTHunter::Instance()->GetTreasurePoint();
	if (m_pGauge) {
		m_pGauge->SetDestPos(point, false);
	}
}

// treasuredlg.cpp:362
void FrTreasureGauge::SetState(eState state) {
	if (m_state != state) {
		m_state = state;

		if (state == (eState)1) {
			Enable(false);
			SetVisible(false);
		} else {
			Enable(true);
			MoveWindow(GetCalcPoint());
			SetVisible(true);

			if (nsTHunter::IsWndSizeMinimum() == true)
				SetOutFrameVisible(false);
			else
				SetOutFrameVisible(true);

			if (state == (eState)3) {
				unsigned long point = CTHunter::Instance()->GetTreasurePoint();
	if (m_pGauge) {
		m_pGauge->SetDestPos(point, false);
	}
			}
		}
	}
}

// treasuredlg.cpp:393
WPoint FrTreasureGauge::GetCalcPoint(void) {
	WRect rect = m_rect;

	float offset = nsTHunter::IsWndSizeMinimum() == true ? 14.0f : -30.0f;

	WPoint pt;
	pt.x = (float)(int)((g_view->GetWidth() - rect.w) * 0.5f) - 90.0f;
	pt.y = (float)(int)(g_view->GetHeight() - rect.h + offset);

	return pt;
}

// treasuredlg.cpp:408
void FrTreasureGauge::SetOutFrameVisible(bool bVisible) {
	int count = m_outFrames.size();
	for (int i = 0; i < count; ++i) {
		if (m_outFrames[i]) {
			m_outFrames[i]->SetVisible(bVisible);
		}
	}
}

// treasuredlg.cpp:418
void FrTreasureGauge::OnArea_Arrow_Init(PG_MSGARG param) {
	m_pArrowArea = DYNAMIC_CAST(FrArea, param);
}

// treasuredlg.cpp:423
void FrTreasureGauge::OnArea_Arrow_OwnerDraw(PG_MSGARG) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI) {



		static float pixelPerHeight = (float)m_pGauge->GetPixelPerHeight();
		if (m_pArrowBitmap) {

			WRect rc = m_pGauge->GetRect();
			pixelPerHeight = (float)m_pGauge->GetPixelPerHeight();

			pGDI->DrawTexture(m_pArrowBitmap, WRect(pixelPerHeight + rc.x - 7.0f, rc.y - 15.0f, m_pArrowBitmap->Width(), m_pArrowBitmap->Height()), 0xFFFFFFFF, 0);
		}
	}
}

// treasuredlg.cpp:440
void FrTreasureGauge::OnArea_OutFrame_Init(intptr_t param) {
	FrArea* pArea = DYNAMIC_CAST(FrArea, param);
	if (pArea) {
		m_outFrames.push_back(pArea);
	}
}

// treasuredlg.cpp:453 (run-time type and factory registration of FrTreasureCourse)
const WRTTI FrTreasureCourse::m_RTTI("FrTreasureCourse", &FrForm::m_RTTI);

IObject* FrTreasureCourseMakeInstance(void) {
	return new FrTreasureCourse;
}

struct __sFrTreasureCourse {
	__sFrTreasureCourse(void) {
		ObjectFactory().AddObjectFunctor(FrTreasureCourseMakeInstance, "FrTreasureCourse");
	}
};
static __sFrTreasureCourse s_registerFrTreasureCourse;

// treasuredlg.cpp:455
const sFRESH_MSGMAP* FrTreasureCourse::GetMessageMap(void) const {
	return &FrTreasureCourse::_MsgMap;
}

sFRESH_MSGMAP FrTreasureCourse::_MsgMap = { &FrForm::_MsgMap, &FrTreasureCourse::_MsgEntries[0] };

sFRESH_ENTRY FrTreasureCourse::_MsgEntries[] = {
	// treasuredlg.cpp:464 (no line mark of its own)
	{ "selectmap", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnAreaSelectMapInit },
	// treasuredlg.cpp:465
	{ "bluelagoon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	// treasuredlg.cpp:466
	{ "bluelagoon", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_Lagoon_Down },
	// treasuredlg.cpp:467
	{ "bluelagoon", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_Lagoon_OwnerDraw },
	// treasuredlg.cpp:469
	{ "bluewater", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	// treasuredlg.cpp:470
	{ "bluewater", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_Water_Down },
	// treasuredlg.cpp:471
	{ "bluewater", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_Water_OwnerDraw },
	// treasuredlg.cpp:473
	{ "sepiawind", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	// treasuredlg.cpp:474
	{ "sepiawind", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_Sepia_Down },
	// treasuredlg.cpp:475
	{ "sepiawind", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_Sepia_OwnerDraw },
	// treasuredlg.cpp:477
	{ "windhill", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	// treasuredlg.cpp:478
	{ "windhill", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_Hill_Down },
	// treasuredlg.cpp:479
	{ "windhill", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_Hill_OwnerDraw },
	// treasuredlg.cpp:481
	{ "wizwiz", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	// treasuredlg.cpp:482
	{ "wizwiz", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_WizWiz_Down },
	// treasuredlg.cpp:483
	{ "wizwiz", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_WizWiz_OwnerDraw },
	// treasuredlg.cpp:485
	{ "westwiz", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	// treasuredlg.cpp:486
	{ "westwiz", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_West_Down },
	// treasuredlg.cpp:487
	{ "westwiz", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_West_OwnerDraw },
	// treasuredlg.cpp:489
	{ "bluemoon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	// treasuredlg.cpp:490
	{ "bluemoon", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_Moon_Down },
	// treasuredlg.cpp:491
	{ "bluemoon", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_Moon_OwnerDraw },
	// treasuredlg.cpp:493
	{ "silvia", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	// treasuredlg.cpp:494
	{ "silvia", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_Silvia_Down },
	// treasuredlg.cpp:495
	{ "silvia", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_Silvia_OwnerDraw },
	// treasuredlg.cpp:497
	{ "icecannon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	// treasuredlg.cpp:498
	{ "icecannon", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_Cannon_Down },
	// treasuredlg.cpp:499
	{ "icecannon", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_Cannon_OwnerDraw },
	// treasuredlg.cpp:501
	{ "whitewiz", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	// treasuredlg.cpp:502
	{ "whitewiz", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_White_Down },
	// treasuredlg.cpp:503
	{ "whitewiz", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_White_OwnerDraw },
	// treasuredlg.cpp:505
	{ "shining", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	// treasuredlg.cpp:506
	{ "shining", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_Shining_Down },
	// treasuredlg.cpp:507
	{ "shining", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_Shining_OwnerDraw },
	// treasuredlg.cpp:509
	{ "pinkwind", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	// treasuredlg.cpp:510
	{ "pinkwind", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_Pink_Down },
	// treasuredlg.cpp:511
	{ "pinkwind", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_Pink_OwnerDraw },
	// treasuredlg.cpp:513
	{ "inferno", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	// treasuredlg.cpp:514
	{ "inferno", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_Inferno_Down },
	// treasuredlg.cpp:515
	{ "inferno", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_Inferno_OwnerDraw },
	// treasuredlg.cpp:517
	{ "icespa", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	// treasuredlg.cpp:518
	{ "icespa", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_IceSpa_Down },
	// treasuredlg.cpp:519
	{ "icespa", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_IceSpa_OwnerDraw },
	// treasuredlg.cpp:521
	{ "seaway", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	// treasuredlg.cpp:522
	{ "seaway", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_SeaWay_Down },
	// treasuredlg.cpp:523
	{ "seaway", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_SeaWay_OwnerDraw },
	// treasuredlg.cpp:525
	{ "panda", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	// treasuredlg.cpp:526
	{ "panda", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_Panda_Down },
	// treasuredlg.cpp:527
	{ "panda", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_Panda_OwnerDraw },
	// treasuredlg.cpp:530
	{ "wizcity", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	// treasuredlg.cpp:531
	{ "wizcity", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_Wizcity_Down },
	// treasuredlg.cpp:532
	{ "wizcity", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_Wizcity_OwnerDraw },
#ifdef JP_PAKS
	// JP data: the JP form's buttons of Ice Inferno, Abbot Mine and Mystic
	// Ruins (docs/JP_CLIENT.md)
	{ "demon_land_map", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	{ "demon_land_map", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_IceInferno_Down },
	{ "demon_land_map", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_IceInferno_OwnerDraw },
	{ "abbot_mine", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	{ "abbot_mine", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_AbbotMine_Down },
	{ "abbot_mine", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_AbbotMine_OwnerDraw },
	{ "luna", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_ALL_Init },
	{ "luna", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_Luna_Down },
	{ "luna", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_Luna_OwnerDraw },
#endif
	// treasuredlg.cpp:535
	{ "random", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnBtnMap_Random_Init },
	// treasuredlg.cpp:536
	{ "random", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrTreasureCourse::OnBtnMap_Random_Down },
	// treasuredlg.cpp:538
	{ "f1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnViewerF1 },
	// treasuredlg.cpp:539
	{ "f2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnViewerF2 },
	// treasuredlg.cpp:541
	{ "gauge1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnImageBar },
	// treasuredlg.cpp:542
	{ "gauge2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnGauge_SmallBar },
	// treasuredlg.cpp:543
	{ "gauge2", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnGauge_SmallBar_OwnerDraw },
	// treasuredlg.cpp:545
	{ "currentmap", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnViewer_CurrMap_Init },
	// treasuredlg.cpp:546
	{ "explane", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnArea_Explane_Init },
	// treasuredlg.cpp:547
	{ "explane", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnArea_Explane_OwnerDraw },
	// treasuredlg.cpp:549
	{ "cloud", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnInit_Cloud_Area },
	// treasuredlg.cpp:550
	{ "cloud", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureCourse::OnOwnerDraw_Cloud_Area },
	// treasuredlg.cpp:558
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// treasuredlg.cpp:561
FrTreasureCourse::FrTreasureCourse(void) {
	FrButton* pNullBtn = 0;
#ifdef JP_PAKS
	// JP data: a slot per course index up to Mystic Ruins (TreasureDlg.h)
	m_mapBtns.reserve(CMapTypeAdapter::MAP_INDEX_COUNT_JP);

	m_mapBtns.insert(m_mapBtns.begin(), CMapTypeAdapter::MAP_INDEX_COUNT_JP, pNullBtn);

	for (int i = 0; i < CMapTypeAdapter::MAP_INDEX_COUNT_JP; i++) {
#else
	m_mapBtns.reserve(20);

	m_mapBtns.insert(m_mapBtns.begin(), 20, pNullBtn);

	for (int i = 0; i < 20; i++) {
#endif
		m_mapBitmaps[i] = 0;
		m_mapGauges[i] = 0;
	}

	m_randomMapBitmap = 0;
	m_unknown254 = 0;
	m_randomBtn = 0;
	m_pCloudOverlay = 0;
	m_pSelectMapArea = 0;
	m_pViewerF1 = 0;
	m_pViewerF2 = 0;
	m_curMapIdx = 0;
	m_unknown1ec = 0;
	m_unknown26c = 0;
	m_unknown270 = 0;
	m_mapBtnInitIdx = 0;
	m_unknown1e9 = 0;
	m_pImageBar = 0;
	m_pSmallBar = 0;
	m_openedMapIdx = -1;
	m_mapIndexChanged = true;
}

// treasuredlg.cpp:607
FrTreasureCourse::~FrTreasureCourse(void) {
	if (g_resrcmng && m_pCloudOverlay) { g_resrcmng->Release(m_pCloudOverlay); m_pCloudOverlay = 0; }
}

// treasuredlg.cpp:615
bool FrTreasureCourse::OnInit(void) {
	m_gaugePos[0] = WPoint(291.0f, 390.0f);
	m_gaugePos[1] = WPoint(129.0f, 391.0f);
	m_gaugePos[2] = WPoint(98.0f, 261.0f);
	m_gaugePos[3] = WPoint(177.0f, 226.0f);
	m_gaugePos[4] = WPoint(234.0f, 146.0f);
	m_gaugePos[5] = WPoint(170.0f, 160.0f);
	m_gaugePos[6] = WPoint(53.0f, 349.0f);
	m_gaugePos[7] = WPoint(230.0f, 457.0f);
	m_gaugePos[8] = WPoint(25.0f, 231.0f);
	m_gaugePos[9] = WPoint(298.0f, 138.0f);
	m_gaugePos[10] = WPoint(493.0f, 278.0f);
	m_gaugePos[11] = WPoint(187.0f, 305.0f);
	m_gaugePos[12] = WPoint(0.0f, 0.0f);
	m_gaugePos[13] = WPoint(329.0f, 239.0f);
	m_gaugePos[14] = WPoint(52.0f, 151.0f);
	m_gaugePos[15] = WPoint(406.0f, 351.0f);
	m_gaugePos[16] = WPoint(339.0f, 329.0f);
	m_gaugePos[19] = WPoint(257.0f, 224.0f);
#ifdef JP_PAKS
	// JP data: the JP exe's positions of the later courses' gauges
	m_gaugePos[18] = WPoint(378.0f, 163.0f);
	m_gaugePos[20] = WPoint(461.0f, 189.0f);
	m_gaugePos[21] = WPoint(490.0f, 362.0f);
#endif

	WRect rect = GetRect();
#ifdef JP_PAKS
	for (int i = 0; i < CMapTypeAdapter::MAP_INDEX_COUNT_JP; i++) {
#else
	for (int i = 0; i < 20; i++) {
#endif
		if (i == 12 ||
			i == 17) {
			continue;
		}
		m_gaugePos[i].x += rect.x;
		m_gaugePos[i].y += rect.y;
	}

	m_randomMapBitmap = g_pFresh->RegisterBitmap("map_random_small");
	m_pCloudBitmap = g_pFresh->RegisterBitmap("th_cloud");
	m_pCloudOverlay = g_resrcmng->GetOverlay("th_cloud.tga", 0);
	m_pExplaneBitmap = g_pFresh->RegisterBitmap("th_gage_tip");
	m_pGaugeHighBitmap = g_pFresh->RegisterBitmap("gauge_high");
	m_pGaugeLowBitmap = g_pFresh->RegisterBitmap("gauge_low");
	m_pMapLockBitmap = g_pFresh->RegisterBitmap("map_lock_small2");

	m_initRect = m_rect;

	WSingleton<CTHunter>::Instance()->DecideTMark();

#ifdef JP_PAKS
	for (int j = 0; j < CMapTypeAdapter::MAP_INDEX_COUNT_JP; j++) { unsigned long gauge = WSingleton<CTHunter>::Instance()->GetTHunterGauge((eMapType)j); if (gauge != 9999) m_mapGauges[j] = gauge; }
#else
	for (int j = 0; j < 20; j++) { unsigned long gauge = WSingleton<CTHunter>::Instance()->GetTHunterGauge((eMapType)j); if (gauge != 9999) m_mapGauges[j] = gauge; }
#endif
	InitCloud();

	if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x800) {
		unsigned char count = (unsigned char)m_mapBtns.size();
		for (unsigned char k = 0; k < count; k++) {
			if (k == 0x11) {
				continue;
			}
			if (k == 0x12) {
				continue;
			}
#ifdef JP_PAKS
			// JP data: course slots the JP form has no button for
			if (m_mapBtns[k] == NULL) {
				continue;
			}
#endif
			IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(0x28000000 | ((k > 0x7F && k != 0xFD) ? k - 0x80 : k));
			if (pCourse == NULL) {
				continue;
			}
			if ((pCourse->courseFlags & 0xF) > 3) {
				m_mapBtns[k]->Enable(false);
			} else {
				switch (k) {
				case MAP_BLUE_WATER:
				case MAP_BLUE_MOON:
				case MAP_ICE_CANNON:
				case MAP_WHITE_WIZ:
				case MAP_ICE_SPA:
					m_mapBtns[k]->Enable(false);
					break;
				}
			}
		}

		if (m_randomBtn) {
			m_randomBtn->Enable(false);
		}
	}

	return true;
}

// treasuredlg.cpp:799
void FrTreasureCourse::OnProc(const float dt) {
	ProcCloud(dt);
}

// treasuredlg.cpp:804
void FrTreasureCourse::InitCloud(void) {
	int cloudCounts[2] = { 3, 4 };
	srand((unsigned int)time(NULL));

	int count = cloudCounts[rand() % 2];

	sCloud cloud;
	m_clouds.reserve(count);

	for (int i = 0; i < count; ++i) {
		m_clouds.push_back(cloud);
	}

	m_cloudCount = m_clouds.size();
	for (int j = 0; j < m_cloudCount; ++j) {
		ReInitCloud(j);
	}
}

// treasuredlg.cpp:830
void FrTreasureCourse::ReInitCloud(int cloudIdx) {
	WRect wndRect = GetRect();
	WRect areaRect = m_pCloudArea->GetRect();

	float step = wndRect.h / m_cloudCount;
	float minY = cloudIdx * step + areaRect.y;
	float maxY = (areaRect.y + areaRect.h) - (areaRect.y + 20.0f);
	float scales[3] = { 0.25f, 0.5f, 1.0f };
	float steps[10] = { 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
	float randomY = cloudIdx * step + step / steps[rand() % 10];
	float y = Between(minY, randomY, maxY);

	WRect rect;
	rect = WRect(0.0f, 0.0f, (float)m_pCloudBitmap->Width(), (float)m_pCloudBitmap->Height());
	sCloud cloud;

	cloud.m_speed = Between(5.0f, Random(1.0f, 10.0f, -1), 10.0f);
	cloud.m_scale = scales[rand() % 4];

	float offset = steps[rand() % 10];
	rect.x = offset * (m_pCloudArea->GetRect().w * 0.1f);

	if (1.0f - cloud.m_scale >= 0.0001f) {
		rect.w *= cloud.m_scale;
		rect.h = cloud.m_scale * rect.h;
		if (cloud.m_scale != 1.0f) {
			rect.w += 0.5f; rect.h += 0.5f;
		}
	}

	if (y + rect.w > maxY) {
		y -= rect.w;
	}

	rect.y = y; cloud.srcRect = rect;

	cloud.alpha = 0;
	if (m_pCloudArea->GetRect().x < rect.x) {
		cloud.m_startVisible = true;
		cloud.alpha = 255;
	}

	sCloud& dst = m_clouds[cloudIdx];
	dst = cloud;
}

// treasuredlg.cpp:888
void FrTreasureCourse::ProcCloud(float dt) {
	for (unsigned char i = 0; i < m_cloudCount; ++i) {
		CheckArea(i, dt);
		m_clouds[i].srcRect.x += dt * m_clouds[i].m_speed;
	}
}

// treasuredlg.cpp:902
void FrTreasureCourse::CheckArea(int cloudIdx, float dt) {
	WRect areaRect = m_pCloudArea->GetRect();
	WRect cloudRect = m_clouds[cloudIdx].srcRect;
	float areaRight = areaRect.x + areaRect.w;
	float cloudRight = cloudRect.x + cloudRect.w;

	if (areaRect.x <= cloudRect.x && areaRect.x + cloudRect.w * 2.0f > cloudRight) {
		if (!m_clouds[cloudIdx].m_startVisible) { if (m_clouds[cloudIdx].alpha < 255) m_clouds[cloudIdx].alpha += 1; else m_clouds[cloudIdx].alpha = 255; }
	} else if (cloudRight > areaRight && cloudRight + areaRight > cloudRect.x) {
		if (m_clouds[cloudIdx].alpha > 0) m_clouds[cloudIdx].alpha -= 5; else m_clouds[cloudIdx].alpha = 0;
	}

	if (cloudRect.x > areaRight) {
		ReInitCloud(cloudIdx);
	}
}

// treasuredlg.cpp:928
void FrTreasureCourse::ShiftAlpha(int cloudIdx, float dt, eALPHA type) {
	if (type == ALPHA_IN && !m_clouds[cloudIdx].m_startVisible) {
		if (m_clouds[cloudIdx].alpha < 255) {
			m_clouds[cloudIdx].alpha += 1;
		} else {
			m_clouds[cloudIdx].alpha = 255;
		}
	} else if (type == ALPHA_OUT) {
		if (m_clouds[cloudIdx].alpha > 0) {
			m_clouds[cloudIdx].alpha -= 5;
		} else {
			m_clouds[cloudIdx].alpha = 0;
		}
	}
}

// treasuredlg.cpp:957
void FrTreasureCourse::OnAreaSelectMapInit(PG_MSGARG param) {
	m_pSelectMapArea = DYNAMIC_CAST(FrArea, param);
}

// treasuredlg.cpp:975
void FrTreasureCourse::OnBtnMap_ALL_Init(PG_MSGARG param) {
	FrButton* pBtn = DYNAMIC_CAST(FrButton, param);
#ifdef JP_PAKS
	// JP data: the JP form puts Ice Inferno's button between Eastern Valley's
	// and Wiz City's and adds Abbot Mine's and Mystic Ruins': each button
	// takes the slot of its course by name, not by the order of the calls.
	if (pBtn) {
		static const struct { const char* name; int slot; } kButtons[] = {
			{ "bluelagoon", MAP_BLUE_LAGOON }, { "bluewater", MAP_BLUE_WATER },
			{ "sepiawind", MAP_SEPIA_WIND }, { "windhill", MAP_WIND_HILL },
			{ "wizwiz", MAP_WIZ_WIZ }, { "westwiz", MAP_WEST_WIZ },
			{ "bluemoon", MAP_BLUE_MOON }, { "silvia", MAP_SILVIA_CANNON },
			{ "icecannon", MAP_ICE_CANNON }, { "whitewiz", MAP_WHITE_WIZ },
			{ "shining", MAP_SHINING_SAND }, { "pinkwind", MAP_PINK_WIND },
			{ "inferno", MAP_DEEP_INFERNO }, { "icespa", MAP_ICE_SPA },
			{ "seaway", MAP_LOST_SEAWAY }, { "panda", MAP_EASTERN_VALLEY },
			{ "demon_land_map", MAP_ICE_INFERNO }, { "wizcity", MAP_WIZ_CITY },
			{ "abbot_mine", MAP_ABBOT_MINE }, { "luna", MAP_MYSTIC_RUINS },
		};
		std::string name;
		pBtn->GetWindowName(name);
		std::string::size_type dot = name.rfind('.');
		if (dot != std::string::npos)
			name = name.substr(dot + 1);
		for (unsigned int i = 0; i < sizeof(kButtons) / sizeof(kButtons[0]); i++) {
			if (stricmp(name.c_str(), kButtons[i].name) == 0 && kButtons[i].slot < (int)m_mapBtns.size()) {
				m_mapBtns[kButtons[i].slot] = pBtn;
				break;
			}
		}
	}
#else
	if (pBtn) {
		if (m_mapBtnInitIdx == 12 || m_mapBtnInitIdx == 17) {
			m_mapBtnInitIdx++;
		}

		if (m_mapBtnInitIdx == 18)
			m_mapBtnInitIdx = 19;

		m_mapBtns[m_mapBtnInitIdx++] = pBtn;

		int count = m_mapBtns.size();
		if (m_mapBtnInitIdx == count) {
			m_mapBtnInitIdx = 0;
		}
	}
#endif
}

// treasuredlg.cpp:1000
void FrTreasureCourse::OnBtnMap_Lagoon_Down(void) {
	SelectMapBtn(MAP_BLUE_LAGOON);
	Close(true);
}

// treasuredlg.cpp:1005
void FrTreasureCourse::OnBtnMap_Lagoon_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_BLUE_LAGOON);
}

// treasuredlg.cpp:1010
void FrTreasureCourse::OnBtnMap_Water_Down(void) {
	SelectMapBtn(MAP_BLUE_WATER);
	Close(true);
}

// treasuredlg.cpp:1015
void FrTreasureCourse::OnBtnMap_Water_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_BLUE_WATER);
}

// treasuredlg.cpp:1020
void FrTreasureCourse::OnBtnMap_Sepia_Down(void) {
	SelectMapBtn(MAP_SEPIA_WIND);
	Close(true);
}

// treasuredlg.cpp:1025
void FrTreasureCourse::OnBtnMap_Sepia_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_SEPIA_WIND);
}

// treasuredlg.cpp:1030
void FrTreasureCourse::OnBtnMap_Hill_Down(void) {
	SelectMapBtn(MAP_WIND_HILL);
	Close(true);
}

// treasuredlg.cpp:1035
void FrTreasureCourse::OnBtnMap_Hill_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_WIND_HILL);
}

// treasuredlg.cpp:1040
void FrTreasureCourse::OnBtnMap_WizWiz_Down(void) {
	SelectMapBtn(MAP_WIZ_WIZ);
	Close(true);
}

// treasuredlg.cpp:1045
void FrTreasureCourse::OnBtnMap_WizWiz_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_WIZ_WIZ);
}

// treasuredlg.cpp:1050
void FrTreasureCourse::OnBtnMap_West_Down(void) {
	SelectMapBtn(MAP_WEST_WIZ);
	Close(true);
}

// treasuredlg.cpp:1055
void FrTreasureCourse::OnBtnMap_West_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_WEST_WIZ);
}

// treasuredlg.cpp:1060
void FrTreasureCourse::OnBtnMap_Moon_Down(void) {
	SelectMapBtn(MAP_BLUE_MOON);
	Close(true);
}

// treasuredlg.cpp:1065
void FrTreasureCourse::OnBtnMap_Moon_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_BLUE_MOON);
}

// treasuredlg.cpp:1070
void FrTreasureCourse::OnBtnMap_Silvia_Down(void) {
	SelectMapBtn(MAP_SILVIA_CANNON);
	Close(true);
}

// treasuredlg.cpp:1075
void FrTreasureCourse::OnBtnMap_Silvia_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_SILVIA_CANNON);
}

// treasuredlg.cpp:1080
void FrTreasureCourse::OnBtnMap_Cannon_Down(void) {
	SelectMapBtn(MAP_ICE_CANNON);
	Close(true);
}

// treasuredlg.cpp:1085
void FrTreasureCourse::OnBtnMap_Cannon_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_ICE_CANNON);
}

// treasuredlg.cpp:1090
void FrTreasureCourse::OnBtnMap_White_Down(void) {
	SelectMapBtn(MAP_WHITE_WIZ);
	Close(true);
}

// treasuredlg.cpp:1095
void FrTreasureCourse::OnBtnMap_White_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_WHITE_WIZ);
}

// treasuredlg.cpp:1100
void FrTreasureCourse::OnBtnMap_Shining_Down(void) {
	SelectMapBtn(MAP_SHINING_SAND);
	Close(true);
}

// treasuredlg.cpp:1105
void FrTreasureCourse::OnBtnMap_Shining_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_SHINING_SAND);
}

// treasuredlg.cpp:1110
void FrTreasureCourse::OnBtnMap_Pink_Down(void) {
	SelectMapBtn(MAP_PINK_WIND);
	Close(true);
}

// treasuredlg.cpp:1115
void FrTreasureCourse::OnBtnMap_Pink_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_PINK_WIND);
}

// treasuredlg.cpp:1120
void FrTreasureCourse::OnBtnMap_Inferno_Down(void) {
	SelectMapBtn(MAP_DEEP_INFERNO);
	Close(true);
}

// treasuredlg.cpp:1125
void FrTreasureCourse::OnBtnMap_Inferno_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_DEEP_INFERNO);
}

// treasuredlg.cpp:1130
void FrTreasureCourse::OnBtnMap_IceSpa_Down(void) {
	SelectMapBtn(MAP_ICE_SPA);
	Close(true);
}

// treasuredlg.cpp:1135
void FrTreasureCourse::OnBtnMap_IceSpa_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_ICE_SPA);
}

// treasuredlg.cpp:1140
void FrTreasureCourse::OnBtnMap_SeaWay_Down(void) {
	SelectMapBtn(MAP_LOST_SEAWAY);
	Close(true);
}

// treasuredlg.cpp:1146
void FrTreasureCourse::OnBtnMap_SeaWay_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_LOST_SEAWAY);
}

// treasuredlg.cpp:1151
void FrTreasureCourse::OnBtnMap_Panda_Down(void) {
	SelectMapBtn(MAP_EASTERN_VALLEY);
	Close(true);
}

// treasuredlg.cpp:1157
void FrTreasureCourse::OnBtnMap_Panda_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_EASTERN_VALLEY);
}

// treasuredlg.cpp:1163
void FrTreasureCourse::OnBtnMap_Wizcity_Down(void) {
	SelectMapBtn(MAP_WIZ_CITY);
	Close(true);
}

// treasuredlg.cpp:1169
void FrTreasureCourse::OnBtnMap_Wizcity_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_WIZ_CITY);
}

#ifdef JP_PAKS
// JP data: the buttons of the JP form's later courses, as the KR ones
void FrTreasureCourse::OnBtnMap_IceInferno_Down(void) {
	SelectMapBtn(MAP_ICE_INFERNO);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_IceInferno_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_ICE_INFERNO);
}

void FrTreasureCourse::OnBtnMap_AbbotMine_Down(void) {
	SelectMapBtn(MAP_ABBOT_MINE);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_AbbotMine_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_ABBOT_MINE);
}

void FrTreasureCourse::OnBtnMap_Luna_Down(void) {
	SelectMapBtn(MAP_MYSTIC_RUINS);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_Luna_OwnerDraw(PG_MSGARG) {
	DrawMapBtn(MAP_MYSTIC_RUINS);
}
#endif

// treasuredlg.cpp:1175
void FrTreasureCourse::OnBtnMap_Random_Init(PG_MSGARG param) {
	m_randomBtn = DYNAMIC_CAST(FrButton, param);
}

// treasuredlg.cpp:1179
void FrTreasureCourse::OnBtnMap_Random_Down(void) {
	SelectMapBtn(0x7f);
	Close(true);
}

// treasuredlg.cpp:1185
void FrTreasureCourse::OnViewerF1(PG_MSGARG param) {
	m_pViewerF1 = DYNAMIC_CAST(FrViewer, param);
	m_pViewerF1->SetMouseEvent(false);
	m_pViewerF1->Open("th_quick_frm1.tga");
}

// treasuredlg.cpp:1192
void FrTreasureCourse::OnViewerF2(PG_MSGARG param) {
	m_pViewerF2 = DYNAMIC_CAST(FrViewer, param);
	m_pViewerF2->SetMouseEvent(false);
	m_pViewerF2->Open("th_quick_frm2.tga");
}

// treasuredlg.cpp:1199
void FrTreasureCourse::OnImageBar(PG_MSGARG param) {
	m_pImageBar = DYNAMIC_CAST(FrGaugeBarImage, param);
	if (m_pImageBar) {
		m_pImageBar->SetRange(0, 300, 0);
		m_pImageBar->SetBarSpeed(200.0f);
		m_pImageBar->SetVisible(false);
	}
}

// treasuredlg.cpp:1210
void FrTreasureCourse::OnGauge_SmallBar(PG_MSGARG param) {
	m_pSmallBar = DYNAMIC_CAST(FrGaugeBarImage, param);
	if (m_pSmallBar) {
		m_pSmallBar->SetRange(0, 300, 0);
		m_pSmallBar->SetBarSpeed(200.0f);
	}
}

// treasuredlg.cpp:1220
void FrTreasureCourse::OnGauge_SmallBar_OwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == 0)
		return;

	const Bitmap* pBackSkin = m_pSmallBar->GetGaugeSkin(0);
	const Bitmap* pBarSkin = m_pSmallBar->GetGaugeSkin(1);
	if (pBackSkin == 0 || pBarSkin == 0)
		return;

#ifdef JP_PAKS
	for (int i = 0; i < CMapTypeAdapter::MAP_INDEX_COUNT_JP; ++i) {
#else
	for (int i = 0; i < 20; ++i) {
#endif
		if (i == 12)
			continue;
		if (i == 17)
			continue;
		if (m_openedMapIdx == i)
			continue;
		m_pSmallBar->SetPos(nsTHunter::CalcGauge(m_mapGauges[i]));

		pGI->DrawTexture(pBackSkin, WRect(m_gaugePos[i].x, m_gaugePos[i].y, (float)pBackSkin->Width(), (float)pBackSkin->Height()), 0xffffffff, 0);
		pGI->DrawTexture(pBarSkin, WRect(m_gaugePos[i].x, m_gaugePos[i].y, (float)pBarSkin->Width(), (float)pBarSkin->Height()), 0xffffffff, 0);

		m_pSmallBar->ResetGauge();
	}
}

// treasuredlg.cpp:1243
void FrTreasureCourse::OnViewer_CurrMap_Init(PG_MSGARG param) {
	m_pCurMapViewer = DYNAMIC_CAST(FrViewer, param);
}

// treasuredlg.cpp:1248
void FrTreasureCourse::OnArea_Explane_Init(PG_MSGARG param) {
	m_pExplaneArea = DYNAMIC_CAST(FrArea, param);
}

// treasuredlg.cpp:1253
void FrTreasureCourse::OnArea_Explane_OwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI) {
		if (m_pExplaneArea && m_pExplaneBitmap) {
			WRect rect = m_pExplaneArea->GetRect();
			if (rect.IsInRect(g_input->GetMousePoint().x, g_input->GetMousePoint().y)) {
				pGI->DrawTexture(m_pExplaneBitmap, WRect(610.0f, 100.0f, (float)m_pExplaneBitmap->Width(), (float)m_pExplaneBitmap->Height()), 0xffffffff, 0);
			}
		}
	}
}

// treasuredlg.cpp:1266
void FrTreasureCourse::OnInit_Cloud_Area(PG_MSGARG param) {
	m_pCloudArea = DYNAMIC_CAST(FrArea, param);
}

// treasuredlg.cpp:1271
void FrTreasureCourse::OnOwnerDraw_Cloud_Area(PG_MSGARG param) {
	if (m_pCloudOverlay) {
		for (int i = 0; i < m_cloudCount; ++i) {
			m_pCloudOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), m_clouds[i].srcRect, 0, (m_clouds[i].alpha << 24) | 0xffffff, 0, 0);
		}
	}
}

// treasuredlg.cpp:1283
void FrTreasureCourse::OpenSelectedMap(unsigned char mapIdx) {
}

// treasuredlg.cpp:1288
void FrTreasureCourse::DrawMapBtn(eMapType type) {
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == NULL) {
		return;
	}

	WRect rect = m_mapBtns[type]->GetRect();

	float offsetX = rect.w * 0.5f - (float)(m_pMapLockBitmap->Width() / 2);
	float offsetY = rect.h * 0.5f - (float)(m_pMapLockBitmap->Height() / 2);

	if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x800) {
		offsetX = (float)floor(offsetX);
		IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(0x28000000 | ((type > 0x7F && type != 0xFD) ? type - 0x80 : type));
		if (pCourse == NULL) {
			return;
		}

		if ((pCourse->courseFlags & 0xF) > 3) {
			pGI->DrawTexture(m_pMapLockBitmap, WRect(rect.x + offsetX, rect.y + offsetY, (float)m_pMapLockBitmap->Width(), (float)m_pMapLockBitmap->Height()), 0xffffffff, 0);
		} else {
			switch (type) {
			case MAP_BLUE_WATER:
			case MAP_BLUE_MOON:
			case MAP_ICE_CANNON:
			case MAP_WHITE_WIZ:
			case MAP_ICE_SPA:
				pGI->DrawTexture(m_pMapLockBitmap, WRect(rect.x + offsetX, rect.y + offsetY, (float)m_pMapLockBitmap->Width(), (float)m_pMapLockBitmap->Height()), 0xffffffff, 0);
				break;
			}
		}
	}

	if (m_mapBtns[type]->GetStatus() == FrButton::OVER) {
		WPoint pt(m_gaugePos[type].x - 15.0f, m_gaugePos[type].y);
		OpenGauge(pt, CMapTypeAdapter::GetMapIndex(type));
	}
}

// treasuredlg.cpp:1359
void FrTreasureCourse::DrawCurrentMap(FrGraphicInterface* pGI, int mapIdx) {
	float y = m_rect.y;
	float x = m_rect.w - 5.0f;

	if (mapIdx != 0x7f) {
		pGI->DrawTexture(m_mapBitmaps[mapIdx], WRect(x, y + 47.0f, m_mapBitmaps[mapIdx]->Width(), m_mapBitmaps[mapIdx]->Height()), 0xffffffff, 0);
	} else {
		pGI->DrawTexture(m_randomMapBitmap, WRect(x, y + 47.0f, m_randomMapBitmap->Width(), m_randomMapBitmap->Height()), 0xffffffff, 0);
	}
}

// treasuredlg.cpp:1372
void FrTreasureCourse::SetMapGauge(void) {
#ifdef JP_PAKS
	for (int i = 0; i < CMapTypeAdapter::MAP_INDEX_COUNT_JP; ++i) {
#else
	for (int i = 0; i < 20; ++i) {
#endif
		unsigned long gauge = WSingleton<CTHunter>::Instance()->GetTHunterGauge((eMapType)i);
		if (gauge != 9999) {
			m_mapGauges[i] = gauge;
		}
	}
}

// treasuredlg.cpp:1386
bool FrTreasureCourse::OpenGauge(WPoint& pt, int mapIdx) {
	if (mapIdx == -1) {
		return false;
	}
	if (m_openedMapIdx == mapIdx) {
		return false;
	}

	m_openedMapIdx = mapIdx;
	m_pImageBar->MoveWindow(pt);
	m_pImageBar->ResetGauge();
	m_pImageBar->SetRange(0, 300, 0);
	m_pImageBar->SetDestPos(m_mapGauges[mapIdx] < 701 ? 0 : m_mapGauges[mapIdx] - 700, false);
	m_pImageBar->SetVisible(true);

	return true;
}

// treasuredlg.cpp:1411
void FrTreasureCourse::OpenCurrentMap(int mapType) {
	if (m_openedMapIdx == mapType) {
		return;
	}

	IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(0x28000000 | (mapType > 127 && mapType != 253 ? mapType - 128 : mapType));
	if (pCourse == 0) {
		return;
	}

	if (mapType < 127) {
		m_pCurMapViewer->Open(MakeStr("mid_%s.tga", pCourse->m_icon));
	} else {
		m_pCurMapViewer->Open("map_random_small.tga");
	}
}

// treasuredlg.cpp:1434
void FrTreasureCourse::SelectMapBtn(unsigned char mapIdx) {
	IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(((mapIdx > 0x7F && mapIdx != 0xFD) ? mapIdx - 0x80 : mapIdx) | 0x28000000);
	if (pCourse == 0)
		return;

	if (m_curMapIdx == mapIdx)
		m_mapIndexChanged = false;
	else
		m_mapIndexChanged = true;

	if (m_mapIndexChanged) {
		m_pCurMapViewer->Open(MakeStr("mid_%s", pCourse->m_icon));
	}

	if (m_mapIndexChanged && (!stricmp(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM") || !stricmp(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXT") || !stricmp(g_pFresh->GetManager()->GetLayoutID(), "AVATARCHAT_MAIN"))) {
		WSendPacket packet(GCT_REQUEST_ROOMINFO_CHANGED);
		packet.Encode2(0xFFFF);
		packet.Encode1(1);
		packet.Encode1(3);
		packet.Encode1((unsigned char)pCourse->m_typeId);
		packet.Send(SEND_TO_DEFAULT);

		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));

		m_curMapIdx = CMapTypeAdapter::GetMapIndex((eMapType)(pCourse->m_typeId & 0x3FFFFFF));
	} else {
		m_curMapIdx = CMapTypeAdapter::GetMapIndex((eMapType)(pCourse->m_typeId & 0x3FFFFFF));
	}
}

// treasuredlg.cpp:1503
void FrTreasureCourse::SetSelectMap(unsigned char mapType) {
	m_curMapIdx = CMapTypeAdapter::GetMapIndex((eMapType)mapType);
#ifdef JP_PAKS
	// JP data: a course without a slot (Grand Zodiac) stays the selection
	// when the form closes with no button pressed: the make-room and room
	// info dialogs take m_curMapIdx for a course id (the KR code gave 0xFF).
	if (m_curMapIdx == (unsigned char)CMapTypeAdapter::MAP_INDEX_NONE)
		m_curMapIdx = mapType;
#endif
	OpenCurrentMap(mapType);
}

// treasuredlg.cpp:1509
void FrTreasureCourse::SetCurMapIdx(unsigned char mapIdx) {
	m_curMapIdx = mapIdx;
}

// treasuredlg.cpp:1514
unsigned char FrTreasureCourse::GetCurMapIdx(void) {
	return m_curMapIdx;
}

// treasuredlg.cpp:1519
bool FrTreasureCourse::GetMapIndexChanged(void) const {
	return m_mapIndexChanged;
}

// treasuredlg.cpp:1524
void FrTreasureCourse::SetMapIndexChanged(bool changed) {
	m_mapIndexChanged = changed;
}

// treasuredlg.cpp:1534 (run-time type and factory registration of FrTreasureScore)
const WRTTI FrTreasureScore::m_RTTI("FrTreasureScore", &FrForm::m_RTTI);

IObject* FrTreasureScoreMakeInstance(void) {
	return new FrTreasureScore;
}

struct __sFrTreasureScore {
	__sFrTreasureScore(void) {
		ObjectFactory().AddObjectFunctor(FrTreasureScoreMakeInstance, "FrTreasureScore");
	}
};
static __sFrTreasureScore s_registerFrTreasureScore;

// treasuredlg.cpp:1536
const sFRESH_MSGMAP* FrTreasureScore::GetMessageMap(void) const {
	return &FrTreasureScore::_MsgMap;
}

sFRESH_MSGMAP FrTreasureScore::_MsgMap = { &FrForm::_MsgMap, &FrTreasureScore::_MsgEntries[0] };

// treasuredlg.cpp:1538
sFRESH_ENTRY FrTreasureScore::_MsgEntries[] = {
	{ "imagebar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureScore::OnGBarImage_ImageBar_Init },
	{ "back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureScore::OnView_Backgound_Init },
	{ "team", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureScore::OnArea_Team_Init },

	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// treasuredlg.cpp:1545
FrTreasureScore::FrTreasureScore(void) {
	m_pGauge = 0;
	m_pBackground = 0;
	m_pTeamArea = 0;
	m_pTeam0Bitmap = 0;
	m_pTeam1Bitmap = 0;
}

// treasuredlg.cpp:1554
FrTreasureScore::~FrTreasureScore(void) {
}

// treasuredlg.cpp:1559
bool FrTreasureScore::OnInit(void) {
	MoveWindow(GetCalcPoint());

	const char* mapName = WSingleton<CSharedDoc>::Instance()->m_pCurCourse ? WSingleton<CSharedDoc>::Instance()->m_pCurCourse->m_mapName : 0;

	char fileName[128];
	sprintf(fileName, "th_result_%s.tga", mapName);
	if (m_pBackground) {
		m_pBackground->Open(fileName);
	}

	m_pTeam0Bitmap = g_pFresh->RegisterBitmap("th_team_blue");
	m_pTeam1Bitmap = g_pFresh->RegisterBitmap("th_team_red");

	IsTeamMode();
	unsigned long point = CTHunter::Instance()->GetTreasurePoint(); if (m_pGauge) m_pGauge->SetDestPos(point, false);

	return true;
}

// treasuredlg.cpp:1581
void FrTreasureScore::SetTPoint(void) {
	unsigned long point = CTHunter::Instance()->GetTreasurePoint();

	if (m_pGauge) {
		m_pGauge->SetDestPos(point, false);
	}
}

// treasuredlg.cpp:1591
void FrTreasureScore::SetState(bool bState) {
	if (m_bState == bState) {
		return;
	}
	m_bState = bState;
	if (!bState) {
		SetVisible(false);
	} else {
		MoveWindow(GetCalcPoint());
		SetVisible(true);
	}
}

// treasuredlg.cpp:1608
WPoint FrTreasureScore::GetCalcPoint(void) {
	WRect rect = m_rect;

	WPoint pt;
	pt.x = (float)(int)((g_view->GetWidth() - rect.w) * 0.5f);
	pt.y = (float)(int)((g_view->GetHeight() - rect.h) * 0.5f);

	pt.y -= 20.0f;
	return pt;
}

// treasuredlg.cpp:1621
void FrTreasureScore::IsTeamMode(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_roomType != 1) {
		m_pTeamArea->SetVisible(false);
		return;
	}

	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (MyGuid(false) == GOLFDOC()->GetPlayer(i)->m_uid) {
			const Bitmap* pBitmap = GOLFDOC()->GetPlayer(i)->m_attribute == 0 ? m_pTeam0Bitmap : m_pTeam1Bitmap;

			if (pBitmap) {
				m_pTeamArea->SetBgImg(pBitmap);
			}
			return;
		}
	}
}

// treasuredlg.cpp:1647
void FrTreasureScore::OnGBarImage_ImageBar_Init(intptr_t param) {
	m_pGauge = DYNAMIC_CAST(FrGaugeBarImage, param);
	if (m_pGauge) {
		m_pGauge->SetRange(0, 1000, 0);
		m_pGauge->SetBarSpeed(600.0f);
	}
}

// treasuredlg.cpp:1657
void FrTreasureScore::OnView_Backgound_Init(intptr_t param) {
	m_pBackground = DYNAMIC_CAST(FrViewer, param);

	if (m_pBackground) {
		m_pBackground->SetMouseEvent(false);
	}
}

// treasuredlg.cpp:1667
void FrTreasureScore::OnArea_Team_Init(intptr_t param) {
	m_pTeamArea = DYNAMIC_CAST(FrArea, param);
}

// treasuredlg.cpp:1678
CTFrame::CTFrame(void)
	: m_count(3) {
	m_frames.reserve(m_count);
}

// treasuredlg.cpp:1690
CTFrame::CTFrame(unsigned char count)
	: m_count(count) {
	m_frames.reserve(count);
}

// treasuredlg.cpp:1701
CTFrame::~CTFrame(void) {
	Reset();
}

// treasuredlg.cpp:1706
void CTFrame::AddFrame(FrArea* pArea) {
	if (pArea) {
		m_frames.push_back(pArea);
	}
}

// treasuredlg.cpp:1712
bool CTFrame::SetFrameWidth(float width) {
	unsigned char start;
	unsigned char end;

	if (0.0f == width) {
		return false;
	}

	unsigned char size = (unsigned char)m_frames.size();

	if (size == 3) {
		start = size;
		end = 6;
	} else if (size == 9) {
		start = 0;
		end = 3;
	}

	for (unsigned char i = start; i < end; i++) {
		WRect rect;
		rect = m_frames[i]->GetRect();
		rect.w += width;
		m_frames[i]->SetRect(rect);
	}

	return true;
}

// treasuredlg.cpp:1745
bool CTFrame::SetFrameHeight(float height) {
	unsigned char start;
	unsigned char mid;
	unsigned char end;

	if (0.0f == height) {
		return false;
	}

	unsigned char size = (unsigned char)m_frames.size();

	if (size == 6) {
		start = 0; mid = 3; end = 6;
	}

	unsigned char i;
	for (i = start; i < mid; i++) {
		WRect rect;
		rect = m_frames[i]->GetRect();
		rect.h += height;
		m_frames[i]->SetRect(rect);
	}

	for (i = mid; i < end; i++) {
		WRect rect;
		rect = m_frames[i]->GetRect();
		rect.y += height;
		m_frames[i]->SetRect(rect);
	}

	return true;
}

// treasuredlg.cpp:1786 (run-time type and factory registration of FrTreasureGift)
const WRTTI FrTreasureGift::m_RTTI("FrTreasureGift", &FrForm::m_RTTI);

IObject* FrTreasureGiftMakeInstance(void) {
	return new FrTreasureGift;
}

struct __sFrTreasureGift {
	__sFrTreasureGift(void) {
		ObjectFactory().AddObjectFunctor(FrTreasureGiftMakeInstance, "FrTreasureGift");
	}
};
static __sFrTreasureGift s_registerFrTreasureGift;

// treasuredlg.cpp:1788
const sFRESH_MSGMAP* FrTreasureGift::GetMessageMap(void) const {
	return &FrTreasureGift::_MsgMap;
}

sFRESH_MSGMAP FrTreasureGift::_MsgMap = { &FrForm::_MsgMap, &FrTreasureGift::_MsgEntries[0] };

sFRESH_ENTRY FrTreasureGift::_MsgEntries[] = {
	// treasuredlg.cpp:1790
	{ "outback3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGift::OnInit_RangeArea_Out },
	// treasuredlg.cpp:1791
	{ "outback4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGift::OnInit_RangeArea_Out },
	// treasuredlg.cpp:1792
	{ "outback5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGift::OnInit_RangeArea_Out },
	// treasuredlg.cpp:1793
	{ "outback6", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGift::OnInit_RangeArea_Out },
	// treasuredlg.cpp:1794
	{ "outback7", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGift::OnInit_RangeArea_Out },
	// treasuredlg.cpp:1795
	{ "outback8", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGift::OnInit_RangeArea_Out },
	// treasuredlg.cpp:1797
	{ "inback3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGift::OnInit_RangeArea_In },
	// treasuredlg.cpp:1798
	{ "inback4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGift::OnInit_RangeArea_In },
	// treasuredlg.cpp:1799
	{ "inback5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGift::OnInit_RangeArea_In },
	// treasuredlg.cpp:1800
	{ "inback6", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGift::OnInit_RangeArea_In },
	// treasuredlg.cpp:1801
	{ "inback7", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGift::OnInit_RangeArea_In },
	// treasuredlg.cpp:1802
	{ "inback8", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGift::OnInit_RangeArea_In },
	// treasuredlg.cpp:1804
	{ "grid", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGift::OnInit_GridArea },
	// treasuredlg.cpp:1805
	{ "grid", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGift::OnOwnerDraw_GridArea },
	// treasuredlg.cpp:1807
	{ "giftlist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGift::OnInit_Gift_List },
	// treasuredlg.cpp:1809 (no line mark of its own)
	{ "giftlist", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGift::OnOwnerDraw_Gift_List },
	// treasuredlg.cpp:1810
	{ "box", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGift::OnInit_Box_Area },
	// treasuredlg.cpp:1811
	{ "box", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTreasureGift::OnOwnerDraw_Box_Area },
	// treasuredlg.cpp:1814
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// treasuredlg.cpp:1818
FrTreasureGift::FrTreasureGift(void)
	: m_gridColumns(8) {
	m_gridRows = 1;
	m_bState = true;
}

// treasuredlg.cpp:1824
FrTreasureGift::~FrTreasureGift(void) {
}

// treasuredlg.cpp:1829
bool FrTreasureGift::OnInit(void) {
	m_boxOpener.OnInit();

	MoveWindow(GetCalcPoint());
	WSingleton<CTHunter>::Instance()->GetTreasurePoint();
	SetPointByFrameHeight();
	m_gridRows = (unsigned char)(m_boxOpener.m_slotCount / 8);

	return true;
}

// treasuredlg.cpp:1841
void FrTreasureGift::SetPointByFrameHeight(void) {
	float outHeight;
	float inHeight;

	int count = m_boxOpener.m_giftCount;

	if (count > 8 && count < 17) {
		outHeight = 65.0f;
		inHeight = 64.0f;
	} else if (count > 16 && count < 25) {
		outHeight = 129.0f;
		inHeight = 128.0f;
	} else {
		return;
	}

	m_frame0.SetFrameHeight(outHeight);
	m_frame1.SetFrameHeight(inHeight);
}

// treasuredlg.cpp:1865
WPoint FrTreasureGift::GetCalcPoint(void) {
	WRect rect = m_rect;

	WPoint pt;
	pt.x = (float)(int)((g_view->GetWidth() - rect.w) * 0.5f);
	pt.y = (float)(int)((g_view->GetHeight() - rect.h) * 0.5f);

	pt.y += 50.0f;
	return pt;
}

// treasuredlg.cpp:1878
void FrTreasureGift::SetState(bool bState) {
	if (m_bState == bState) {
		return;
	}
	m_bState = bState;
	if (!bState) {
		SetVisible(false);
	} else {
		MoveWindow(GetCalcPoint());
		SetVisible(true);
	}
}

// treasuredlg.cpp:1896
void FrTreasureGift::OnOwnerDraw_Gift_List(intptr_t param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem) {
		m_boxOpener.DrawGiftList(pItem);
	}
}

// treasuredlg.cpp:1906
void FrTreasureGift::OnProc(const float dt) {
	m_boxOpener.OnProc(dt);
}

// treasuredlg.cpp:1913
void FrTreasureGift::OnInit_RangeArea_Out(PG_MSGARG param) {
	m_frame0.AddFrame(DYNAMIC_CAST(FrArea, param));
}

// treasuredlg.cpp:1918
void FrTreasureGift::OnInit_RangeArea_In(PG_MSGARG param) {
	m_frame1.AddFrame(DYNAMIC_CAST(FrArea, param));
}

// treasuredlg.cpp:1923
void FrTreasureGift::OnInit_Gift_List(intptr_t param) {
	m_boxOpener.m_pGiftList = DYNAMIC_CAST(FrListBox, param);
}

// treasuredlg.cpp:1928
void FrTreasureGift::OnInit_GridArea(PG_MSGARG param) {
	m_pGridArea = DYNAMIC_CAST(FrArea, param);
}

// treasuredlg.cpp:1933
void FrTreasureGift::OnOwnerDraw_GridArea(PG_MSGARG param) {
	WRect rect = m_boxOpener.m_pGiftList->GetRect();

	// Horizontal lines of the gift grid.
	_WRECT line = { rect.x + 25.0f, rect.y + 30.0f, rect.w - 65.0f, 1.0f };
	WOverlay::DrawBox(g_view, line, 0, data_a94af8, 0.0f);

	if (m_gridRows == 2) {
		_WRECT line2 = { rect.x + 25.0f, rect.y + 98.0f, rect.w - 65.0f, 1.0f };
		WOverlay::DrawBox(g_view, line2, 0, data_a94af8, 0.0f);
	} else if (m_gridRows == 3) {
		_WRECT line3 = { rect.x + 25.0f, rect.y + 166.0f, rect.w - 65.0f, 1.0f };
		WOverlay::DrawBox(g_view, line3, 0, data_a94af8, 0.0f);
	}

	// Vertical lines.
	for (unsigned char i = 0; i < 8; ++i) {
		_WRECT line4 = { rect.x + i * 65.0f + 33.0f, rect.y + 35.0f, 1.0f, rect.h - 60.0f };
		WOverlay::DrawBox(g_view, line4, 0, data_a94af8, 0.0f);
	}
}

// treasuredlg.cpp:1962
void FrTreasureGift::OnInit_Box_Area(PG_MSGARG param) {
	m_boxOpener.m_pBoxArea = DYNAMIC_CAST(FrArea, param);
	if (m_boxOpener.m_pBoxArea) {
		WRect rect = m_boxOpener.m_pBoxArea->GetRect();

		rect.y = m_rect.y - 10.0f;
		m_boxOpener.m_pBoxArea->SetRect(rect);
	}
}

// treasuredlg.cpp:1975
void FrTreasureGift::OnOwnerDraw_Box_Area(PG_MSGARG param) {
	m_boxOpener.DrawBoxArea();
}

// treasuredlg.cpp:1981 (run-time type and factory registration of FrTestDlg_combo)
const WRTTI FrTestDlg_combo::m_RTTI("FrTestDlg_combo", &FrForm::m_RTTI);

IObject* FrTestDlg_comboMakeInstance(void) {
	return new FrTestDlg_combo;
}

struct __sFrTestDlg_combo {
	__sFrTestDlg_combo(void) {
		ObjectFactory().AddObjectFunctor(FrTestDlg_comboMakeInstance, "FrTestDlg_combo");
	}
};
static __sFrTestDlg_combo s_registerFrTestDlg_combo;

// treasuredlg.cpp:1983
const sFRESH_MSGMAP* FrTestDlg_combo::GetMessageMap(void) const {
	return &FrTestDlg_combo::_MsgMap;
}

sFRESH_MSGMAP FrTestDlg_combo::_MsgMap = { &FrForm::_MsgMap, &FrTestDlg_combo::_MsgEntries[0] };

// treasuredlg.cpp:1985
sFRESH_ENTRY FrTestDlg_combo::_MsgEntries[] = {
	{ "mapcomboex", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTestDlg_combo::OnInit_combo },
	{ "mapcomboex", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrTestDlg_combo::ONLBTN_DOWN },

	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// treasuredlg.cpp:1991
FrTestDlg_combo::FrTestDlg_combo(void) {
	m_pCombo = 0;
}

// treasuredlg.cpp:1996
FrTestDlg_combo::~FrTestDlg_combo(void) {
}

// treasuredlg.cpp:2001
void FrTestDlg_combo::OnInit_combo(intptr_t param) {
	m_pCombo = DYNAMIC_CAST(FrComboCtlEx, param);
	if (m_pCombo) {
		m_pCombo->AddTexItem("hole_front");
		m_pCombo->AddTexItem("hole_back");
		m_pCombo->AddTexItem("hole_random");
		m_pCombo->AddTexItem("hole_shuffle");
		m_pCombo->SelectItem(0);
	}
}

// treasuredlg.cpp:2016
bool FrTestDlg_combo::OnInit(void) {
	return true;
}

// treasuredlg.cpp:2021
void FrTestDlg_combo::ONLBTN_DOWN(void) {
	unsigned char idx = m_pCombo->GetCurrentIdx();
}

// treasuredlg.cpp:2030
CBoxOpener::CBoxOpener(void) {
	m_boxBitmapIdx = 0;
	m_openGiftIdx = 0;
	m_giftCount = 0;
	m_slotCount = 0;

	m_pGiftList = 0;
	m_unknown48 = 0;
	m_nextSeq = SEQ_NONE;
	m_curSeq = SEQ_NONE;
	m_approachMode = 0;
	m_seqDuration = 2000;

	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));
	char* filename = "[font_wind.jpg";
	info.filename = &filename;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:";

	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&info);
}

// treasuredlg.cpp:2060
CBoxOpener::~CBoxOpener(void) {
	if (m_pSimpleUI) { delete m_pSimpleUI; m_pSimpleUI = 0; }
	if (g_resrcmng && m_pTitleFont) { g_resrcmng->Release(m_pTitleFont); m_pTitleFont = 0; }

	m_approachMode = 0;
}

// treasuredlg.cpp:2067
bool CBoxOpener::OnInit(void) {
	if (m_approachMode == 1) {
		WSingleton<CTHunter>::Instance()->ReAdjustRepository();
	}

	m_giftCount = WSingleton<CTHunter>::Instance()->GetRepositorySize();

	if (m_giftCount < 9) {
		m_slotCount = 8;
	} else if (m_giftCount > 8 && m_giftCount < 17) {
		m_slotCount = 16;
	} else if (m_giftCount > 16 && m_giftCount < 25) {
		m_slotCount = 24;
	} else {
		m_slotCount = 8;
	}

	m_gifts.reserve(m_slotCount);

	sTGift gift;
	for (unsigned char i = 0; i < m_slotCount; ++i) {
		if (i < m_giftCount) {
			sTreasureHunt hunt;
			hunt = WSingleton<CTHunter>::Instance()->GetGiftInfo(i);
			gift.itemIdx = i;
			gift.m_typeId = hunt.m_typeId;
			gift.m_count = hunt.m_count;
			gift.m_isMine = hunt.m_index == WSingleton<CSharedDoc>::Instance()->m_myUID;
		} else {
			gift.Reset();
		}

		m_gifts.push_back(gift);
		sTGift* pGift = &m_gifts[i];
		m_pGiftList->AddItem(pGift);
	}

	WRect rect = m_pGiftList->GetRect();
	rect.h = (m_slotCount / 8) * 68.0f;
	m_pGiftList->SetRect(rect);

	m_pItemBoxNBitmap = g_pFresh->RegisterBitmap("th_item_box_n");
	m_pItemBoxOBitmap = g_pFresh->RegisterBitmap("th_item_box_o");

	m_pItemMark1Bitmap = g_pFresh->RegisterBitmap("th_item_mark1");
	m_pItemMark2Bitmap = g_pFresh->RegisterBitmap("th_item_mark2");
	m_pMsg1Bitmap = g_pFresh->RegisterBitmap("th_msg1");

	m_boxBitmaps[0] = g_pFresh->RegisterBitmap("th_box_frm");
	for (int i = 1; i < 5; ++i) {
		m_boxBitmaps[i] = g_pFresh->RegisterBitmap(MakeStr("th_box_%d", i));
	}

	m_pSimpleUI = new CSimpleUI("[emoti_object.png");
	m_pSimpleUI->Add("0", WRect(0.0f, 128.0f, 64.0f, 64.0f), false);
	m_pSimpleUI->Add("1", WRect(64.0f, 128.0f, 64.0f, 64.0f), false);
	m_pSimpleUI->Add("2", WRect(128.0f, 128.0f, 64.0f, 64.0f), false);
	m_pSimpleUI->Add("3", WRect(192.0f, 128.0f, 64.0f, 64.0f), false);
	m_pSimpleUI->Add("4", WRect(0.0f, 192.0f, 64.0f, 64.0f), false);
	m_pSimpleUI->Add("5", WRect(64.0f, 192.0f, 64.0f, 64.0f), false);
	m_pSimpleUI->Add("6", WRect(128.0f, 192.0f, 64.0f, 64.0f), false);
	m_pSimpleUI->Add("7", WRect(192.0f, 192.0f, 64.0f, 64.0f), false);

	m_boxBitmapIdx = DecideTRBox();
	if (m_boxBitmapIdx > 5) {
		m_boxBitmapIdx = 1;
	}

	m_seqStartTime = timeGetTime();
	m_pProcFunc = &CBoxOpener::ProcInit;
	WSingleton<CTHunter>::Instance()->SetReturnLobby(false);
	m_nextSeq = SEQ_NONE;
	m_curSeq = SEQ_INIT;
	return true;
}

// treasuredlg.cpp:2163
void CBoxOpener::OnProc(float dt) {
	m_dt = dt;
	if (m_nextSeq != SEQ_NONE) {
		ChangeSequence(m_nextSeq);
	}

	if (m_pProcFunc != NULL) {
		(this->*m_pProcFunc)();
	}
}

// treasuredlg.cpp:2178
void CBoxOpener::ChangeSequence(eSeqProc seq) {
	switch (seq) {
	case SEQ_INIT:
		m_seqStartTime = timeGetTime();
		m_pProcFunc = &CBoxOpener::ProcInit;
		WSingleton<CTHunter>::Instance()->SetReturnLobby(false);
		break;
	case SEQ_CREATE:
		m_seqStartTime = timeGetTime();
		m_seqDuration = 250;
		m_pProcFunc = &CBoxOpener::ProcCreate;
		m_unknown48 = 3;
		break;
	case SEQ_MYITEM_OPEN:
		m_seqStartTime = timeGetTime();
		m_pProcFunc = &CBoxOpener::ProcMyItemOpen;
		m_unknown48 = 4;
		for (unsigned char i = 0; i < m_giftCount; ++i) {
			GetGiftItem(i);
		}
		m_openGiftIdx = 0;
		m_seqDuration = 200;
		break;
	case SEQ_COMPLETE:
		m_seqStartTime = timeGetTime();
		m_pProcFunc = &CBoxOpener::ProcComplete;
		m_seqDuration = 1500;
		break;
	case SEQ_UNKNOWN5:
		break;
	case SEQ_END:
		m_seqStartTime = timeGetTime();
		m_pProcFunc = &CBoxOpener::ProcEnd;
		m_seqDuration = 3000;
		break;
	case SEQ_CLOSE:
		WSingleton<CTHunter>::Instance()->SetReturnLobby(true);
		break;
	}

	m_curSeq = seq;
	m_nextSeq = SEQ_NONE;
}

// treasuredlg.cpp:2266
void CBoxOpener::ProcInit(void) {
	if (timeGetTime() - m_seqStartTime > 1000) {
		for (unsigned char i = 0; i < m_pGiftList->GetCurrentItemSize(true); ++i) {
			GetGiftItem(i)->m_state = 1;
		}
		m_nextSeq = SEQ_CREATE;
	}
}

// treasuredlg.cpp:2283
void CBoxOpener::ProcCreate(void) {
	if (timeGetTime() - m_seqStartTime > m_seqDuration) {
		if (m_openGiftIdx < m_giftCount) {
			sTGift* pGift = GetGiftItem(m_openGiftIdx++);
			if (pGift && pGift->m_typeId) {
				pGift->m_revealed = 1;
				pGift->m_state = 3;
				g_audio->PlaySfx("play_fail_item", 0, 0, 0, 0, 0.5f, 200.0f);
			}
		} else if (m_openGiftIdx < m_slotCount) {
			sTGift* pGift = GetGiftItem(m_openGiftIdx++);
			pGift->m_state = 2;
		} else {
			m_openGiftIdx++;
		}

		m_seqStartTime = timeGetTime();

		if (m_openGiftIdx == m_giftCount) {
			m_seqDuration = 50;
		}
		if (m_openGiftIdx == m_slotCount) {
			m_nextSeq = SEQ_MYITEM_OPEN;
		}
	}
}

// treasuredlg.cpp:2324
void CBoxOpener::ProcMyItemOpen(void) {
	if (m_openGiftIdx < m_giftCount && timeGetTime() - m_seqStartTime > 250) {
		if (m_openGiftIdx < m_giftCount) {
			sTGift* pGift = GetGiftItem(m_openGiftIdx++);
			if (pGift && pGift->m_typeId) {
				pGift->m_state = 4;
				pGift->m_emoticonFrame = 0;
				g_audio->PlaySfx("play_fail_item", 0, 0, 0, 0, 0.5f, 200.0f);
			} else {
				m_openGiftIdx++;
			}
			m_seqStartTime = timeGetTime();
		}
	}

	if (m_openGiftIdx == m_giftCount && timeGetTime() - m_seqStartTime > m_seqDuration) {
		m_nextSeq = SEQ_COMPLETE;
	}
}

// treasuredlg.cpp:2355
void CBoxOpener::ProcComplete(void) {
	if (timeGetTime() - m_seqStartTime > m_seqDuration && m_curSeq == SEQ_COMPLETE) {
		m_nextSeq = SEQ_CLOSE;
	}
}

// treasuredlg.cpp:2364
void CBoxOpener::ProcBoxNone(void) {
}

// treasuredlg.cpp:2369
void CBoxOpener::ProcEnd(void) {
	if (timeGetTime() - m_seqStartTime > m_seqDuration && m_curSeq == SEQ_END) {
		m_nextSeq = SEQ_CLOSE;
	}
}

// treasuredlg.cpp:2381
CBoxOpener::sTGift* CBoxOpener::GetGiftItem(int index) {
	FrListItem* pItem = m_pGiftList->GetItem(index);
	if (pItem == NULL) {
		return NULL;
	}

	return (sTGift*)pItem->pData;
}

// treasuredlg.cpp:2394
void CBoxOpener::DrawGiftList(FrListItem* pItem) {
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	sTGift* pGift = (sTGift*)pItem->pData;
	if (pGI) {
		WPoint pt = m_pGiftList->GetRect().TopLeft();
		const Bitmap* pBmp;

		if (m_curSeq >= SEQ_COMPLETE && pGift->m_isMine) {
			pBmp = m_pItemBoxOBitmap;
		} else {
			pBmp = m_pItemBoxNBitmap;
		}

		pGI->DrawTexture(pBmp, WRect(pItem->pos.x, pItem->pos.y - 5.0f, (float)pBmp->Width(), (float)pBmp->Height()), 0xffffffff, 0);

		if (m_curSeq >= SEQ_MYITEM_OPEN && m_giftCount == 0) {
			static float s_msgX = g_view->GetWidth() * 0.5f - (float)(m_pMsg1Bitmap->Width() / 2);
			static float s_msgY = g_view->GetHeight() * 0.5f + 30.0f;
			pGI->DrawTexture(m_pMsg1Bitmap, WRect(s_msgX, s_msgY, (float)m_pMsg1Bitmap->Width(), (float)m_pMsg1Bitmap->Height()), 0xffffffff, 0);
		}

		unsigned long color = 0xffffffff;
		pBmp = 0;
		GetItemOffset(pt, pGift->m_typeId, true);

		switch (pGift->m_state) {
		case 0:
			return;
		case 1:
			pBmp = m_pItemMark1Bitmap;
			pt.x = 27.0f; pt.y = 18.0f;
			break;
		case 2:
			pBmp = m_pItemMark2Bitmap;
			pt.x = 27.0f; pt.y = 28.0f;
			break;
		case 3:
			if (pGift->m_revealed == 1) {
				IFF_ITEM_COMMON* pItemData = ItemManager()->FindCommonItem(pGift->m_typeId);
				if (pItemData) {
					if (pGift->m_typeId != 0x1A000010) {
						pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", pItemData->m_icon);
					} else {
						pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "Item_Pang");
					}

					color = 0xA0202020;
					pt.x += 13.0f;
					pt.y += 7.0f;
				}
			}
			break;
		case 4:
			color = pGift->m_isMine == 1 ? 0xffffffff : 0x20202020;
			if (pGift->m_revealed == 1) {
				IFF_ITEM_COMMON* pItemData = ItemManager()->FindCommonItem(pGift->m_typeId);
				if (pItemData) {
					if (pGift->m_typeId != 0x1A000010) {
						pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", pItemData->m_icon);
					} else {
						pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "Item_Pang");
					}

					pt.x += 13.0f;
					pt.y += 7.0f;
				}
			}
			break;
		}

		if (pBmp) {
			pGI->DrawTexture(pBmp, WRect(pt.x + pItem->pos.x, pt.y + pItem->pos.y, (float)pBmp->Width(), (float)pBmp->Height()), color, 0);

			if (m_pTitleFont && pGift->m_state == 4) {
				if (pGift->m_count != 0) {
					WPoint pos(pItem->pos.x, pItem->pos.y);
					GetItemOffset(pos, pGift->m_typeId, false);

					char text[16] = { 0 };
					itoa(pGift->m_count, text, 10);

					if (pGift->m_typeId == 0x1A000010) {
						pos.x -= 15.0f;
					}

					m_pTitleFont->Print(g_view, pos.x + pItem->pos.x + pt.x, pos.y + pt.y + pItem->pos.y, text, 0, 0xffffffff, 0);
				}
			}
		}

		if (pGift->m_state >= 3) {
			if (pGift->m_emoticonFrame >= 0.0f && (int)pGift->m_emoticonFrame < 8) {
				if (m_pSimpleUI) {
					m_pSimpleUI->Render(MakeStr("%d", (int)pGift->m_emoticonFrame), pItem->pos.x - 10.0f, pItem->pos.y - 25.0f, 1.5f, 0);
					pGift->m_emoticonFrame += m_dt * 15.0f;
				}
			}
		}
	}
}

// treasuredlg.cpp:2532
void CBoxOpener::DrawBoxArea(void) {
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == NULL) {
		return;
	}

	WRect rect = m_pBoxArea->GetRect();

	if (m_boxBitmaps[0] != NULL) {
		pGI->DrawTexture(m_boxBitmaps[0], WRect(rect.x, rect.y, (float)m_boxBitmaps[0]->Width(), (float)m_boxBitmaps[0]->Height()), 0xffffffff, 0);
	}

	if (m_boxBitmaps[m_boxBitmapIdx] != NULL) {
		pGI->DrawTexture(m_boxBitmaps[m_boxBitmapIdx], WRect(rect.x + 20.0f, rect.y + 22.0f, (float)m_boxBitmaps[m_boxBitmapIdx]->Width(), (float)m_boxBitmaps[m_boxBitmapIdx]->Height()), 0xffffffff, 0);
	}

	if (m_approachMode == 1) {
		unsigned char giftCount = WSingleton<CTHunter>::Instance()->GetGiftSize();
		if (giftCount > 0) {
			pGI->SetTextColor(0xffff0000, 0xffffffff);
			// "Items of the same kind are added up in one slot."
			pGI->Print(WPoint(rect.x + 230.0f, rect.y + 134.0f), 0, "\xB0\xB0\xC0\xBA\xC1\xBE\xB7\xF9\xC0\xC7 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBA \xC7\xD1 \xBD\xBD\xB7\xD4\xBF\xA1 \xC7\xD5\xBB\xEA\xC7\xA5\xB1\xE2 \xB5\xCB\xB4\xCF\xB4\xD9.");
			pGI->SetTextColor(0xffffffff, 0xffffffff);
		}
	}
}

// treasuredlg.cpp:2565
void CBoxOpener::GetItemOffset(WPoint& pt, unsigned long typeId, bool bOpened) {
	if (bOpened == true) {
		switch (typeId >> 26) {
		case 6:
			// Both coupon kinds use the same offset in the original.
			if (ItemManager()->GetCouponKind(typeId) == 0)
				pt = WPoint(0.0f, 0.0f);
			else
				pt = WPoint(0.0f, 0.0f);
			break;
		case 0x1c:
			pt = WPoint(-10.0f, -20.0f);
			break;
		case 0x1f:
			pt = WPoint(0.0f, -10.0f);
			break;
		}
	} else {
		switch (typeId >> 26) {
		case 6:
			if (ItemManager()->GetCouponKind(typeId) == 0)
				pt = WPoint(40.0f, 33.0f);
			else
				pt = WPoint(40.0f, 33.0f);
			break;
		case 0x1c:
			pt = WPoint(48.0f, 50.0f);
			break;
		case 0x1f:
			pt = WPoint(45.0f, 43.0f);
			break;
		}
	}
}

// treasuredlg.cpp:2638
unsigned char CBoxOpener::DecideTRBox(void) {
	unsigned char boxCourseCount[4] = { 3, 4, 3, 5 };
	// The courses of each box, box after box (boxCourseCount[i] of box i + 1).
	unsigned char boxCourses[15] = { 2, 3, 11, 0, 1, 6, 7, 10, 13, 15, 4, 5, 8, 9, 14 };
	unsigned char roomCourse = WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course;
	unsigned char course = (roomCourse > 0x7F && roomCourse != 0xFD) ? roomCourse - 0x80 : roomCourse;
	unsigned char courseIdx = 0;
	unsigned char boxEnd = 0;

	for (unsigned char box = 0; box < 4; box++) {
		boxEnd += boxCourseCount[box];
		for (; courseIdx < boxEnd; courseIdx++) {
			if (boxCourses[courseIdx] == course) {
				return box + 1;
			}
		}
	}

	return 1;
}
