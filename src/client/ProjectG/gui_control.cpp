// Original: d:\build\custom\temp\code\client\projectg\gui_control.cpp
// The original holds the out-of-line copy of CMouseCursor::IsVisible and
// IsCamMode (mousecursor.h) in this object.
#define GUI_CONTROL_MOUSECURSOR_INLINE
// ... and of CClub::GetBarWidth and GetBarAlpha (club.h).
#define GUI_CONTROL_CLUB_INLINE

#include <atlbase.h>	// stand-in for the precompiled header: _AtlWinModule initializer
			// (keeps BAR_LENGTH dynamically initialized, docs/tough_problems/010)
#include "gui_control.h"
#include "fredit.h"
#include "ObjectFactory.h"
#include "actor.h"
#include "fontinfo.h"
#include "SimpleOverlay.h"
#include "wresrcmng.h"
#include "wview.h"
#include "mousecursor.h"
#include "GolfDoc.h"
#include "SharedDoc.h"
#include "ChatMsg.h"
#include "frbutton.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "bitmap.h"
#include "ReportDlg.h"
#include "shared/classdefine.h"
#include "Packet.h"
#include "TaskManager.h"
#include "CapturedBg.h"
#include "InputManager.h"
#include "QuitWindow.h"
#include "autotarget.h"
#include "golfball.h"
#include "club.h"
#include "wind.h"
#include "s5/Utilities.h"
#include "s5/SkinnedTheme.h"
#include "s5/classicserver/ClassicServer.h"
#include "commonutil.h"
#include "fxinterpreter.h"
#include "wfont.h"
#include "ClientSetting.h"
#include "PowerGauge.h"
#include "quadtree.h"
#include "ground_info.h"
#include "HelpMsg.h"
#include "headicon.h"
#include "ItemWindow.h"
#include "wpuppet.h"
#include "ProjectG.h"
#include "SoundManager.h"
#include "user_info.h"
#include "TrainingOptionDlg.h"

extern WMatrix g_camera;

// pi / 180, an unnamed .rdata constant (HandleMsg, DrawClubInfo)
extern "C" float data_9ef91c;
// 180 / pi, 140 (the hit bar's left edge) and 1e-5: unnamed .rdata constants (OnProcess).
extern "C" float data_9ef920;
extern "C" float data_9ef938;
extern "C" float data_9ef928;
// commonutil.cpp; commonutil.h does not declare it yet.
float Random(float min, float max, int seed);

// The wind font's page file (tagWTITLEFONT::filename of CScreen's title font).
extern "C" {
static char* data_a80ec8 = "[font_wind.jpg";
}

// gui_control.cpp:45 (run-time type and factory registration of CScreen)
const WRTTI CScreen::m_RTTI("CScreen", &IActor::m_RTTI);

IObject* CScreenMakeInstance(void) {
	return new CScreen;
}

struct __impCScreen {
	__impCScreen(void) {
		ObjectFactory().AddObjectFunctor(CScreenMakeInstance, "CScreen");
		ActorFactory().AddActorClass("CScreen");
	}
};
static __impCScreen s_impCScreen;

// gui_control.cpp:54
CScreen::CScreen(void)
	: m_pWindPuppet(NULL), m_overlays(0x20, 0x20), m_pFont(NULL) {
	m_pFontOverlay = NULL;
	m_pBase = NULL;
	m_pWindInfo = NULL;
	m_pRemainTimeInfo = NULL;
	m_pClubTypeInfo = NULL;
	m_pMiniScore = NULL;
	m_pAuxMenuUI = NULL;
	m_pAccuracyInfo = NULL;
	m_pPangInfo = NULL;
	m_pPowerInfo = NULL;

	m_pCammodeUI = NULL;
	m_pSimpleUI21C = NULL;
	m_pSimpleUI220 = NULL;
	m_pTutoTitleUI = NULL;


	m_pTitleFont = NULL;
	m_pMissionUI = NULL;





	for (int i = 0; i < 12; i++) m_pTutorialUIs[i] = NULL;
	m_missionTitleTime = 0;
	m_missionTitleAlpha = 1.0f;
	m_pCommandArrowUI = NULL;
	m_pTutoEscMsgUI = NULL;
	m_pCalipers = NULL;
	m_pHolePoseSkins = NULL;
	m_pExtraHoleInfo = NULL;
	m_clubBoundingBox = -1;
	m_pGuildInfo = NULL;
	m_priority = 4;
	AddSkipList(0x400);
}

// gui_control.cpp:98
void CScreen::OnDestroy(void) {

	if (m_pFontOverlay) { delete m_pFontOverlay; m_pFontOverlay = NULL; }
	if (m_pBase) { delete m_pBase; m_pBase = NULL; }
	if (m_pWindInfo) { delete m_pWindInfo; m_pWindInfo = NULL; }
	if (m_pRemainTimeInfo) { delete m_pRemainTimeInfo; m_pRemainTimeInfo = NULL; }
	if (m_pClubTypeInfo) { delete m_pClubTypeInfo; m_pClubTypeInfo = NULL; }
	if (m_pMiniScore) { delete m_pMiniScore; m_pMiniScore = NULL; }
	if (m_pAuxMenuUI) { delete m_pAuxMenuUI; m_pAuxMenuUI = NULL; }
	if (m_pAccuracyInfo) { delete m_pAccuracyInfo; m_pAccuracyInfo = NULL; }
	if (m_pPangInfo) { delete m_pPangInfo; m_pPangInfo = NULL; }
	if (m_pPowerInfo) { delete m_pPowerInfo; m_pPowerInfo = NULL; }

	if (m_pCammodeUI) { delete m_pCammodeUI; m_pCammodeUI = NULL; }
	if (m_pSimpleUI21C) { delete m_pSimpleUI21C; m_pSimpleUI21C = NULL; }
	if (m_pSimpleUI220) { delete m_pSimpleUI220; m_pSimpleUI220 = NULL; }

	if (m_pTutoTitleUI) { delete m_pTutoTitleUI; m_pTutoTitleUI = NULL; }


	for (int i = 0; i < 12; i++) {

		if (m_pTutorialUIs[i]) { delete m_pTutorialUIs[i]; m_pTutorialUIs[i] = NULL; }
	}


	if (m_pMissionUI) { delete m_pMissionUI; m_pMissionUI = NULL; }
	if (m_pTitleFont) {

		m_pTitleFont->Erase();
		if (g_resrcmng && m_pTitleFont) { g_resrcmng->Release(m_pTitleFont); m_pTitleFont = 0; }
	}


	if (m_pCommandArrowUI) { delete m_pCommandArrowUI; m_pCommandArrowUI = NULL; }
	if (m_pCalipers) { delete m_pCalipers; m_pCalipers = NULL; }
	if (m_pHolePoseSkins) { delete m_pHolePoseSkins; m_pHolePoseSkins = NULL; }
	if (m_pExtraHoleInfo) { delete m_pExtraHoleInfo; m_pExtraHoleInfo = NULL; }
	if (m_pTutoEscMsgUI) { delete m_pTutoEscMsgUI; m_pTutoEscMsgUI = NULL; }

	if (m_pGuildInfo) { delete m_pGuildInfo; m_pGuildInfo = NULL; }

	delete WSingleton<CHelpMsg>::Instance();

	if (g_resrcmng && m_pWindPuppet) { g_resrcmng->Release(m_pWindPuppet, false); m_pWindPuppet = 0; }



	for (sOverlay* pOverlay = m_overlays.Start(); pOverlay; pOverlay = m_overlays.Next())
		delete pOverlay;

	m_overlays.Reset();

	GetFxInterpreter()->DetachFx(m_clubBoundingBox);
}

// gui_control.cpp:154
void CScreen::OnPreLoadInit(void) {

	m_unknown3E1 = true; m_unknown3E3 = false;
	m_unknown3E2 = false;
}

// gui_control.cpp:285
void CScreen::OnLoad(void) {
	AddOverlay("\xb9\xd9\xc5\xc1\xbf\xf8", "[left_back.jpg", 0);
	AddOverlay("\xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3\xbf\xf8", "[left_time.jpg", 0);
	AddOverlay("\xc1\xa4\xba\xb8\xc7\xa5\xbd\xc3\xbf\xf8", "[left_ht.jpg", 0);
	AddOverlay("\xba\xbc\xbb\xf3\xc5\xc2", "[ball_con.jpg", 0);
	AddOverlay("\xbf\xde\xc2\xca\xb0\xd4\xc0\xcc\xc1\xf6", "middle_mblue.jpg", 0);
	AddOverlay("\xc6\xc4\xbf\xf6\xb0\xd4\xc0\xcc\xc1\xf6", "middle_lblue.jpg", 0);
	AddOverlay("\xc3\xbc\xb7\xc2\xb0\xd4\xc0\xcc\xc1\xf6", "middle_dblue.jpg", 0);
	AddOverlay("\xb4\xab\xb1\xdd_\xb5\xe5\xb6\xf3\xc0\xcc\xba\xea", "[middle.jpg", 0);
	AddOverlay("\xb4\xab\xb1\xdd_\xb5\xe5\xb6\xf3\xc0\xcc\xba\xea_\xb0\xc5\xb2\xd9\xb7\xce", "[middle.jpg", 0);
	AddOverlay("\xb4\xab\xb1\xdd_\xc6\xdb\xc6\xc3" "10", "[middle-p.jpg", 0);
	AddOverlay("\xb4\xab\xb1\xdd_\xc6\xdb\xc6\xc3" "15", "[middle-p15.jpg", 0);
	AddOverlay("\xb4\xab\xb1\xdd_\xc6\xdb\xc6\xc3" "20", "[middle-p20.jpg", 0);
	AddOverlay("\xc0\xd4\xc3\xbc\xb9\xd9", "middle_bar.jpg", 0);
	AddOverlay("\xb0\xd4\xc0\xcc\xc1\xf6\xc1\xa4\xc1\xf6", "[left_play_0.jpg", 0x80000000);
	AddOverlay("\xb0\xd4\xc0\xcc\xc1\xf6\xc5\xac\xb8\xaf", "[left_play_1.jpg", 0x80000000);
	AddOverlay("\xb0\xd4\xc0\xcc\xc1\xf6\xc0\xcc\xb5\xbf", "[left_play_4.jpg", 0x80000000);
	AddOverlay("\xc5\xb8\xc0\xcc\xb9\xd6\xbf\xb5\xbf\xaa", "middle_rblue.jpg", 0);
	AddOverlay("\xbd\xba\xc7\xc9\xc6\xf7\xc0\xce\xc6\xae", "[left_ballpoint.jpg", 0);
	AddOverlay("\xbd\xba\xc7\xc9\xc6\xf7\xc0\xce\xc6\xae_Red", "[left_ballpoint_red.png", 0);

	if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty())) {
		AddOverlay("\xb9\xd9\xb6\xf7\xbc\xbc\xb1\xe2\xbe\xe0", "classic_green.tga", 0);
		AddOverlay("\xb9\xd9\xb6\xf7\xbc\xbc\xb1\xe2\xc1\xdf", "classic_yellow.tga", 0);
		AddOverlay("\xb9\xd9\xb6\xf7\xbc\xbc\xb1\xe2\xb0\xad", "classic_red.tga", 0);
	}

	_ISkinnedTheme* pTheme = S5::THEME::GetCurrentSkinnedTheme();
	AddOverlay("\xb9\xd9\xb6\xf7\xc5\xab\xc3\xa2", pTheme->GetWindOutterCircleImgName(), 0);
	AddOverlay("\xb9\xd9\xb6\xf7\xc0\xdb\xc0\xba\xc3\xa2", pTheme->GetWindInnerCircleImgName(), 0);

	AddOverlay("\xbd\xc3\xc0\xdb\xc1\xa1", "middle_blue.jpg", 0);

	AddOverlay("\xc8\xa6\xc4\xc5\xb1\xb8\xb0\xa3", "hole_position.jpg", 0);
	AddOverlay("\xc5\xac\xb7\xb4\xb9\xd9", "{middle_club.png", 0);
	AddOverlay("\xc5\xbe\xba\xe4\xb5\xb5\xbf\xf2\xb8\xbb", "[camera_top.jpg", 0);

	AddOverlay("\xc1\xb6\xc1\xbe\xba\xe4\xb5\xb5\xbf\xf2\xb8\xbb", "[camera_free.jpg", 0);
	AddOverlay("\xb3\xab\xc7\xcf\xba\xe4\xb5\xb5\xbf\xf2\xb8\xbb", "[camera_target.jpg", 0);

	AddOverlay("\xc5\xac\xb8\xaf\xc7\xa5\xbd\xc3", "[click.png", 0);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 10) {
		AddOverlay("\xb0\xd4\xc0\xd3\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3_border", "icon_time.png", 0);
		AddOverlay("\xb0\xd4\xc0\xd3\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3_base", "icon_time_02.png", 0);
		AddOverlay("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1\xc1\xa4\xba\xb8", "[ground_text_02.jpg", 0);

		AddOverlay("\xb9\xcc\xbc\xc7\xb9\xda\xbd\xba\xc7\xc1\xb7\xb9\xc0\xd3" "1", "treasurebox_frm1.tga", 0);
		AddOverlay("\xb9\xcc\xbc\xc7\xb9\xda\xbd\xba\xc7\xc1\xb7\xb9\xc0\xd3" "2", "treasurebox_frm2.png", 0);

		switch (GetCurMap()) {
		case 2:
		case 3:
		case 11:
		case 16:
		case 19:
			AddOverlay("\xc6\xae\xb7\xb9\xc1\xae\xb9\xda\xbd\xba", "treasurebox_a.png", 0);
			break;
		case 0:
		case 1:
		case 6:
		case 7:
			AddOverlay("\xc6\xae\xb7\xb9\xc1\xae\xb9\xda\xbd\xba", "treasurebox_b.png", 0);
			break;
		case 10:
		case 13:
		case 15:
		case 17:
			AddOverlay("\xc6\xae\xb7\xb9\xc1\xae\xb9\xda\xbd\xba", "treasurebox_c.png", 0);
			break;
		case 4:
		case 5:
		case 8:
		case 9:
		case 14:
			AddOverlay("\xc6\xae\xb7\xb9\xc1\xae\xb9\xda\xbd\xba", "treasurebox_d.png", 0);
			break;
		}
	}
	else {
		AddOverlay("\xb0\xd4\xc0\xd3\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3", "timer.png", 0);
	}

	m_pCammodeUI = new CSimpleUI("[camera_mode.jpg");
	m_pCammodeUI->Add("FORWARD_OVER", WRect(0.0f, 0.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("FORWARD_CLICK", WRect(0.0f, 85.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("BACKWARD_OVER", WRect(85.0f, 0.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("BACKWARD_CLICK", WRect(85.0f, 85.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("LEFT_OVER", WRect(0.0f, 170.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("LEFT_CLICK", WRect(0.0f, 255.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("RIGHT_OVER", WRect(85.0f, 170.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("RIGHT_CLICK", WRect(85.0f, 255.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("UP_OVER", WRect(0.0f, 340.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("UP_CLICK", WRect(0.0f, 425.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("DOWN_OVER", WRect(85.0f, 340.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("DOWN_CLICK", WRect(85.0f, 425.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("NORMAL", WRect(170.0f, 0.0f, 85.0f, 85.0f), false);
	m_pCammodeUI->Add("INACTIVE", WRect(170.0f, 85.0f, 85.0f, 85.0f), false);

	m_pSimpleUI21C = new CSimpleUI("[club_line.jpg");
	m_pSimpleUI21C->Add("1W", WRect(0.0f, 0.0f, 512.0f, 44.0f), false);
	m_pSimpleUI21C->Add("2W", WRect(0.0f, 44.0f, 512.0f, 44.0f), false);
	m_pSimpleUI21C->Add("3W", WRect(0.0f, 88.0f, 512.0f, 44.0f), false);
	m_pSimpleUI21C->Add("2I", WRect(0.0f, 132.0f, 512.0f, 44.0f), false);
	m_pSimpleUI21C->Add("3I", WRect(0.0f, 176.0f, 512.0f, 44.0f), false);
	m_pSimpleUI21C->Add("4I", WRect(0.0f, 220.0f, 512.0f, 44.0f), false);
	m_pSimpleUI21C->Add("5I", WRect(0.0f, 264.0f, 256.0f, 62.0f), false);
	m_pSimpleUI21C->Add("6I", WRect(256.0f, 264.0f, 256.0f, 62.0f), false);
	m_pSimpleUI21C->Add("7I", WRect(0.0f, 326.0f, 256.0f, 62.0f), false);
	m_pSimpleUI21C->Add("8I", WRect(256.0f, 326.0f, 256.0f, 62.0f), false);
	m_pSimpleUI21C->Add("9I", WRect(0.0f, 388.0f, 256.0f, 62.0f), false);
	m_pSimpleUI21C->Add("PW", WRect(256.0f, 388.0f, 256.0f, 62.0f), false);
	m_pSimpleUI21C->Add("SW", WRect(0.0f, 450.0f, 256.0f, 62.0f), false);
	m_pSimpleUI21C->Add("1PT", WRect(256.0f, 450.0f, 256.0f, 62.0f), false);
	m_pSimpleUI21C->Add("2PT", WRect(256.0f, 450.0f, 256.0f, 62.0f), false);

	m_pCommandArrowUI = new CSimpleUI("keys.png");
	m_pCommandArrowUI->Add("DOWN_S", WRect(0.0f, 0.0f, 20.0f, 20.0f), false);
	m_pCommandArrowUI->Add("LEFT_S", WRect(20.0f, 0.0f, 20.0f, 20.0f), false);
	m_pCommandArrowUI->Add("RIGHT_S", WRect(40.0f, 0.0f, 20.0f, 20.0f), false);
	m_pCommandArrowUI->Add("UP_S", WRect(0.0f, 20.0f, 20.0f, 20.0f), false);
	m_pCommandArrowUI->Add("DOWN", WRect(20.0f, 20.0f, 20.0f, 20.0f), false);
	m_pCommandArrowUI->Add("LEFT", WRect(40.0f, 20.0f, 20.0f, 20.0f), false);
	m_pCommandArrowUI->Add("RIGHT", WRect(0.0f, 40.0f, 20.0f, 20.0f), false);
	m_pCommandArrowUI->Add("UP", WRect(20.0f, 40.0f, 20.0f, 20.0f), false);
	m_pCommandArrowUI->Add("INVALID", WRect(40.0f, 40.0f, 20.0f, 20.0f), false);

	m_pSimpleUI220 = new CSimpleUI("[icon01.jpg");
	m_pSimpleUI220->Add("\xc7\xf6\xc0\xe7\xc5\xac\xb7\xb4", WRect(288.0f, 0.0f, 48.0f, 48.0f), false);

	if (WSingleton<CSharedDoc>::Instance()->m_pGolfDoc->m_gameState < 15) {
		CSimpleUI** ppTutorialUI = m_pTutorialUIs;

		bool wideTitle[12] = { true, true, false, true, true, true, false, false, false, true, false, false };

		for (int i = 0; i < 12; i++) {
			CSimpleUI* pUI = new CSimpleUI(MakeStr("quest_message_%d.png", i + 1));


			*ppTutorialUI = pUI; if (wideTitle[i] == true) {

				pUI->Add("MissionTitle", WRect(0.0f, 0.0f, 256.0f, 16.0f), false);

			}
			else {
				pUI->Add("MissionTitle", WRect(0.0f, 0.0f, 512.0f, 16.0f), false);
			}
			ppTutorialUI++;
		}
	}

	m_remainTimeY = WSingleton<COption>::Instance()->gGetBar_Y();

	m_pWindPuppet = g_resrcmng->GetPuppet("wind.pet", false, true, false);

	m_pFontOverlay = new CFontOverlay;
	m_pBase = new CBase;
	m_pPangInfo = new CPangInfo;
	m_pMiniScore = new CMiniScore;
	m_pWindInfo = new CWindInfo;
	m_pClubTypeInfo = new CClubTypeInfo;
	m_pAccuracyInfo = new CAccuracyInfo;
	m_pPowerInfo = new CPowerInfo;

	if (GetGameType() == 7) {

		m_pHolePoseSkins = new CHolePoseSkins;
		m_pExtraHoleInfo = new CExtraHoleInfo;
	}







	if (GOLFDOC()->m_gameState == 15) {


		AddOverlay("\xb9\xf6\xc6\xb0\xc3\xa2_normal", "[left_ic_00.jpg", 0x80000000);
		AddOverlay("\xb9\xf6\xc6\xb0\xc3\xa2_item", "[left_ic_i.jpg", 0x80000000);
		AddOverlay("\xb9\xf6\xc6\xb0\xc3\xa2_chat", "[left_ic_c.jpg", 0x80000000);
		AddOverlay("\xbd\xc3\xb0\xe8", "[left_clock.jpg", 0);
		AddOverlay("\xbf\xac\xbd\xc0\xb8\xf0\xb5\xe5\xbf\xc9\xbc\xc7", "train_option_small.png", 0);

		m_pAuxMenuUI = new CAuxMenuUI;
		m_pRemainTimeInfo = new CRemainTimeInfo;
		m_pCalipers = new CCalipers;

		new CHelpMsg;
	}
	else {
		if (GOLFDOC()->m_gameState == 5 || GOLFDOC()->m_gameState == 9) {
			AddOverlay("\xb9\xf6\xc6\xb0\xc3\xa2_normal", "[left_ic_00.jpg", 0x80000000);
			AddOverlay("\xb9\xf6\xc6\xb0\xc3\xa2_item", "[left_ic_i.jpg", 0x80000000);
			AddOverlay("\xb9\xf6\xc6\xb0\xc3\xa2_chat", "[left_ic_c.jpg", 0x80000000);

			m_pAuxMenuUI = new CAuxMenuUI;
			new CHelpMsg;
		}

		AddOverlay("\xc1\xb6\xc1\xd8\xbf\xb5\xbf\xaa", "{sight.png", 0);
		AddOverlay("\xc7\xd9\xbd\xc9\xc6\xf7\xc0\xce\xc6\xae", "check_point.png", 0);
		AddOverlay("\xc7\xd9\xbd\xc9\xc6\xf7\xc0\xce\xc6\xaesmall", "check_point_small.png", 0);

		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 12) {
			m_tutorialNo = 0;
			m_pTutoTitleUI = new CSimpleUI("[tuto_opentext3.png");
			for (int i = 0; i <= 14; i++) {
				m_pTutoTitleUI->Add(MakeStr("\xc5\xb8\xc0\xcc\xc6\xb2_%d", i + 1), WRect(0.0f, (float)i * 28.0f, 128.0f, 28.0f), true);
			}
		}
		else {
			AddOverlay("\xb9\xcc\xbc\xc7\xc1\xa4\xba\xb8_0", "[mission_00.jpg", 0);
		}

		m_tutoMsgBlinkPeriod = 0.25f;
		m_tutoMsgShown = 0;
		m_tutoMsgBlinkCount = 2;
		m_tutoMsgBlinkTime = 0;
		m_missionMarkBlinkPeriod = 0.25f;
		m_missionMarkBlinkCount = 3;
		m_tutoTitleState = 2;
		m_tutoTitleTime = 0;
		m_tutoTitleAlpha = 0;
		m_missionTitleState = 2;
		m_missionTitleTime = 0;

		m_pTutoEscMsgUI = new CSimpleUI("[tuto_esc_msg2.png");
		m_pTutoEscMsgUI->Add("MSG0", WRect(0.0f, 0.0f, 256.0f, 16.0f), false);
		m_pTutoEscMsgUI->Add("MSG1", WRect(0.0f, 16.0f, 256.0f, 16.0f), false);
	}

	m_pFont = WSingleton<CChatMsg>::Instance()->GetMaskedFont();

	for (int i = 0; i < 15; i++) {
		m_missionMarkShown[i] = i < GOLFDOC()->m_gameState;
	}

	if (GOLFDOC()->m_gameState < 15) {
		if (m_pFontOverlay)
			m_pFontOverlay->FixHoleInfo();
	}
}

// gui_control.cpp:610
void CScreen::OnInit(void) {
	m_bannerTime = 0;


	m_hitBarCount = 10;
	m_hitBarFrom = 0;
	m_hitBarTime = 0;
	m_selectedIndex = -1;
	m_turnStarted = false;
	m_bannerGameMode = 0;

	m_accumValue = 0;
	m_fadeValue = 0;
	m_blinkTime = 0;
	m_hudVisible = true;
	m_clubAngle = 6.2831855f;
	m_clubSpinDir = 0;
	m_clubSwing = 0;
	m_playerCount = 0;

	m_msgColorTime = 0;
	m_msgColor = 0;

	m_cammodeAlpha = 0;

	m_drawFlags.set(1);

	unsigned long clubSetTypeId = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId;
	sClubSet* pClubSet = GolfClub().FindClubSet(clubSetTypeId);
	WPuppet* pPuppet = NULL;
	if (pClubSet) {

		pPuppet = pClubSet->m_puppet[1][GolfClub().GetType()];
	}
	m_clubBoundingBox = GetFxInterpreter()->ParseBoundingBoxScript_fx(pPuppet, FxInterpreter::SCRIPT_FORMAT_TEXT);


	if (m_pTitleFont == NULL) {
		tagWTITLEFONT info;
		memset(&info, 0, sizeof(info));
		info.filename = &data_a80ec8;
		info.fontw = 16;
		info.fonth = 16;
		info.numPages = 1;
		info.texw = 128;
		info.texh = 64;
		info.pCharSet = "1234567890ym%-.?/:";
		m_pTitleFont = g_resrcmng->GetTitleFont();
		m_pTitleFont->Create(&info);
	}

	if (m_pMissionUI == NULL) {
		m_pMissionUI = new CSimpleUI("text_mission.png");
		m_pMissionUI->Add("\xb9\xcc\xbc\xc7\xbe\xf8\xc0\xbd", WRect(91.0f, 10.0f, 232.0f, 100.0f), false);
		m_pMissionUI->Add("\xb9\xcc\xbc\xc7\xc0\xd6\xc0\xbd", WRect(50.0f, 130.0f, 256.0f, 130.0f), false);
	}
}

// gui_control.cpp:675
void CScreen::OnProcess(float elapsed) {
	if (GOLFDOC()->m_gameState < 0xF) {
		if (m_missionMarkBlinkCount < 3) {
			m_missionMarkBlinkTime += elapsed;
			if (m_missionMarkBlinkTime > m_missionMarkBlinkPeriod) {
				m_missionMarkBlinkTime -= m_missionMarkBlinkPeriod;
				m_missionMarkShown[GOLFDOC()->m_gameState] = !m_missionMarkShown[GOLFDOC()->m_gameState];
				if (m_missionMarkShown[GOLFDOC()->m_gameState])
					m_missionMarkBlinkCount++;
			}
		}

		if (m_tutoMsgBlinkCount < 2) {
			m_tutoMsgBlinkTime += elapsed;
			if (m_tutoMsgBlinkTime > m_tutoMsgBlinkPeriod) {
				m_tutoMsgBlinkTime -= m_tutoMsgBlinkPeriod;
				m_tutoMsgShown = 1 - m_tutoMsgShown;
				if (m_tutoMsgShown)
					m_tutoMsgBlinkCount++;
			}
		}

		float startX = g_view->GetWidth() * 0.5f - 50.0f;
		float startY = g_view->GetHeight() * 0.3f;
		float endX = g_view->GetWidth() - 250.0f;

		switch (m_tutoTitleState) {
		case 0:
			m_tutoTitleTime += elapsed;
			if (m_tutoTitleTime > 1.0f) {
				m_tutoTitleTime = 0;
				m_tutoTitleAlpha = 0xFF;
				m_tutoTitleState = 1;
			}
			else {
				m_tutoTitleAlpha = (unsigned char)(m_tutoTitleTime * 255.0f);
			}
			m_tutoTitleX = startX;
			m_tutoTitleY = startY;
			break;
		case 1: {
			m_tutoTitleTime += elapsed;
			if (m_tutoTitleTime > 1.0f) {
				m_tutoTitleTime = 1.0f;
				m_tutoTitleState = 2;
				IActor* pTutorial = GetActor("Tutorial");
				if (pTutorial)
					pTutorial << MsgObject(0, 0x148, 0, 0, 0, 0, 0);
			}
			WVector2D pos = WVector2D(startX, startY) * (1.0f - m_tutoTitleTime) + WVector2D(endX, 2.0f) * m_tutoTitleTime;
			m_tutoTitleX = pos.x;
			m_tutoTitleY = pos.y;
			break;
		}
		}
	}

	if (GOLFDOC()->m_gameState >= 0 && GOLFDOC()->m_gameState <= 0xE) {
		if (GOLFDOC()->m_gameState != 6 && GOLFDOC()->m_gameState != 7) {
			float startX = g_view->GetWidth() * 0.5f;
			float startY = g_view->GetHeight() * 0.33333334f;
			float endX = (float)((int)g_view->GetWidth() / 2);

			switch (m_missionTitleState) {
			case 0:
				m_missionTitleAlpha = 1.2f;
				m_missionTitleTime += elapsed;
				if (m_missionTitleTime > 1.0f) {
					m_missionTitleTime = 0;
					m_missionTitleState = 1;
				}
				m_missionTitleX = startX;
				m_missionTitleY = startY;
				break;
			case 1: {
				m_missionTitleTime += elapsed;
				m_missionTitleAlpha -= elapsed * 0.2f;
				if (m_missionTitleTime > 1.0f) {
					m_missionTitleTime = 1.0f;
					m_missionTitleAlpha = 1.0f;
					m_missionTitleState = 2;
				}
				WVector2D pos = WVector2D(startX, startY) * (1.0f - m_missionTitleTime) + WVector2D(endX, 10.0f) * m_missionTitleTime;
				m_missionTitleX = pos.x;
				m_missionTitleY = pos.y;
				break;
			}
			case 2:
				m_missionTitleAlpha = 1.0f;
				break;
			}
		}
	}

	if (WSingleton<CCapturedBg>::Instance()->IsCaptured()) {
		if (GOLFDOC()->m_clockHold == 2) {
			m_hitBarTime += elapsed * 100.0f;
			if (m_hitBarTime > 20.0f)
				m_hitBarTime -= 40.0f;
		}
	}
	else {
		Wind().Process(elapsed);

		if (m_pBase)
			m_pBase->Process(elapsed);

		if (m_pFontOverlay)
			m_pFontOverlay->Process(elapsed);

		if (m_pPangInfo)
			m_pPangInfo->Process(elapsed);

		if (m_pGuildInfo)
			m_pGuildInfo->Process(elapsed);

		float alpha = elapsed * 7.0f + m_cammodeAlpha;
		if (alpha > 1.0f)
			alpha = 1.0f;
		m_cammodeAlpha = alpha;

		if (GolfClub().TypeUpdated()) {
			GetFxInterpreter()->DetachFx(m_clubBoundingBox);
			WPuppet* pPuppet = GolfClub().FindClubSet(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId)->m_puppet[1][GolfClub().GetType()];
			m_clubBoundingBox = GetFxInterpreter()->ParseBoundingBoxScript_fx(pPuppet, FxInterpreter::SCRIPT_FORMAT_TEXT);
		}

		if (GolfClub().Updated()) {
			m_clubAngle = 0;
			m_clubSpinDir = 1;
			m_clubSwing = 0;
		}
		else {
			switch (m_clubSpinDir) {
			case -1:
				m_clubAngle -= elapsed * 21.99115f;
				if (m_clubAngle < 6.2831855f) {
					m_clubAngle = 6.2831855f;
					m_clubSpinDir = 0;
				}
				break;
			case 0:
				m_clubSwing += elapsed * 2.0f;
				break;
			case 1:
				m_clubAngle += elapsed * 21.99115f;
				if (m_clubAngle > 6.8067842f) {
					m_clubAngle = 6.8067842f;
					m_clubSpinDir = -1;
				}
				break;
			}
		}
	}

	if (m_pCalipers) {
		if (IsMyTurn(false) && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000009) {
			m_pCalipers->Process(elapsed);
		}
		else if (S5::IsUsableCalipers() == true) {
			if (WSingleton<CSharedDoc>::Instance()->m_gameMode != 1 && WSingleton<CSharedDoc>::Instance()->m_gameMode != 2)
				m_pCalipers->Process(elapsed);
		}
	}

	if (m_pAccuracyInfo)
		m_pAccuracyInfo->Process(elapsed);

	if (WSingleton<CSharedDoc>::Instance()->m_shotTimeLimit) {
		if (GOLFDOC()->m_clockHold <= 0 && IsMyTurn(false)) {
			if (GOLFDOC()->m_remainShotTime < 10000) {
				m_blinkTime += elapsed;
				if (m_blinkTime > 0.25f) {
					m_blinkTime -= 0.25f;
					m_hudVisible = !m_hudVisible;
				}
			}
			else {
				m_hudVisible = true;
			}
		}
		else {
			m_hudVisible = true;
			m_blinkTime = 0;
		}
	}

	if (m_pMiniScore && GOLFDOC()->m_gameState >= 7)
		m_pMiniScore->Process(elapsed);

	if (GOLFDOC()->GameMode() & 0x40300) {
		if (m_bannerGameMode != GOLFDOC()->GameMode()) {
			m_bannerGameMode = GOLFDOC()->GameMode();
			m_bannerTime = -255.0f;
		}
		else {
			float time = elapsed * 512.0f + m_bannerTime;
			if (time > 255.0f)
				time = 255.0f;
			m_bannerTime = time;
		}
	}

	if (GolfBall().m_groundType == 2) {
		CGolfDoc* pDoc = GOLFDOC();
		WVector cupPos = pDoc->GetHoleData().m_cupPos;
		float range = Max(0.0f, 0.69813168f - Abs(CalcDeltaAngle(cupPos - GolfBall().m_pos, pDoc->GetCameraData().m_matrix.za)));
		unsigned char alpha = (unsigned char)(128.0f * (range * 1.4323945f));

		if (GolfClub().PowerShotType() != 0)
			m_hitBarRangeColor = (alpha << 24) | 0xFFF600;
		else
			m_hitBarRangeColor = (alpha << 24) | 0xFF96;
	}

	if (m_turnStarted) {
		m_turnStartTime += elapsed;
		if (m_turnStartTime > 5.0f) {
			m_turnStarted = false;
			CMouseCursor::Instance()->m_enable[0x21] = true;
		}
	}

	if (!WSingleton<CQuitWindow>::Instance()->IsActive()) {
		switch (CMouseCursor::Instance()->GetArea()) {
		case 7:
			if (g_input->GetButton(LEFT_BUTTON) == 2 && CMouseCursor::Instance()->IsInButtonDownArea(LEFT_BUTTON) && GOLFDOC()->m_gameState == 0xF) {
				if (WSingleton<CChatMsg>::Instance()->IsOpened())
					WSingleton<CChatMsg>::Instance()->Close(false, true);
				else
					WSingleton<CChatMsg>::Instance()->Open(false, true);
				SendMsg(this, "Item", 0x195, 0, 0, 0, 0);
			}
			break;
		case 5: {
			int state;
			SendMsg(this, "Item", 0x199, (intptr_t)&state, 0, 0, 0);
			if (state == 3) {
				SendMsg(this, "Player", 0x199, (intptr_t)&state, 0, 0, 0);
				if (state != 3) {
					if (m_pCalipers)
						m_pCalipers->VisiblePowerBar(1);

					if (g_input->GetButton(LEFT_BUTTON) == 1 && (m_drawFlags >> 1).test(0) && IsMyTurn(false) && GOLFDOC()->m_clockHold == 0 && !GOLFDOC()->m_pause && !WSingleton<CChatMsg>::Instance()->IsActive() && GOLFDOC()->m_gameState >= 2 && GolfClub().BarIs(2)) {
						SendMsg(this, "GolfRule", 0x76, 0, 0, 0, 0);
					}
				}
			}
			break;
		}
		case 10:
		case 11:
		case 12:
		case 13: {
			bool busy;
			SendMsg(this, "Score", 0x139, (intptr_t)&busy, 0, 0, 0);
			if (!busy) {
				if (g_input->GetButton(RIGHT_BUTTON)) {
					IActor* pGolfRule = GetActor("GolfRule");
					if (pGolfRule) {
						if (!WSingleton<CChatMsg>::Instance()->IsOpened())
							WSingleton<CChatMsg>::Instance()->Open(true, true);
						unsigned char index = CMouseCursor::Instance()->GetArea() - 10;
						pGolfRule << MsgObject(0, 0x37, (intptr_t)WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_name, 0, 0, 0, 0);
					}
				}

				if (g_input->GetButton(LEFT_BUTTON) == 2 && CMouseCursor::Instance()->IsInButtonDownArea(LEFT_BUTTON) && OnlinePlay()) {
					unsigned char index = CMouseCursor::Instance()->GetArea() - 10;
					if (PLAYER(index)->m_state != 3) {
						g_audio->PlaySfx("play_menu_open", 0, 0, 0, 0, 0.5f, 200.0f);
						CMouseCursor::Instance()->SetCamMode(false);

						std::map<unsigned long, sBriefUserInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_userMap.find(PLAYER(index)->m_uid);
						if (it != WSingleton<CSharedDoc>::Instance()->m_userMap.end())
							WSingleton<CUserInfo>::Instance()->SetInfo(it->second.id, PLAYER(index)->m_uid, true, true, false, false, it->second.nick);
					}
				}
			}
			break;
		}
		case 31:
			if (g_input->GetButton(LEFT_BUTTON) == 2 && CMouseCursor::Instance()->IsInButtonDownArea(LEFT_BUTTON)) {
				IActor* pTutorial = GetActor("Tutorial");
				if (pTutorial)
					pTutorial << MsgObject(0, 0x14B, 0, 0, 0, 0, 0);
			}
			break;
		case 32:
			if (g_input->GetButton(LEFT_BUTTON) == 2 && CMouseCursor::Instance()->IsInButtonDownArea(LEFT_BUTTON)) {
				IActor* pTutorial = GetActor("Tutorial");
				if (pTutorial)
					pTutorial << MsgObject(0, 0x14C, 0, 0, 0, 0, 0);
			}
			break;
		case 33:
			if (g_input->GetButton(LEFT_BUTTON) == 2 && CMouseCursor::Instance()->IsInButtonDownArea(LEFT_BUTTON)) {
				if (GOLFDOC()->m_gameState < 0xF && !m_turnStarted) {
					m_turnStarted = true;
					m_turnStartTime = 0;
					CMouseCursor::Instance()->m_enable[0x21] = false;
				}
			}
			break;
		case 36:
			if (g_input->GetButton(LEFT_BUTTON) == 2 && CMouseCursor::Instance()->IsInButtonDownArea(LEFT_BUTTON)) {
				CMouseCursor::Instance()->m_trainingOptionEnable = false;
				FrTrainingOptionDlg* pDlg = CreateForm<FrTrainingOptionDlg>(g_pFresh->GetManager(), this, "training", 0);
				pDlg->Open((FRESH_RESULT_CALLBACK)&CScreen::OnTrainingOptionDlgResult, 7);
			}
			break;
		}
	}

	if (WSingleton<CHelpMsg>::Instance())
		WSingleton<CHelpMsg>::Instance()->Process(elapsed);

	if (WSingleton<CHeadIcon>::Instance())
		WSingleton<CHeadIcon>::Instance()->Process(elapsed);

	if (IsMyTurn(true)) {
		if (GolfClub().GetType() == 3) {
			GOLFDOC()->GetBar().m_startPosition = 140.0f;
		}
		else {
			float offset = 0;
			SendMsg(this, "Player", 0x113, (intptr_t)&offset, 0, 0, 0);

			if (IsLocalContent((localContentType_t)0x2F) && GOLFDOC()->GetPlayer(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000028)
				offset = (float)floor(0.0);
			else
				offset = (float)floor((double)(offset * 0.5f * data_9ef920));

			if (Abs(offset) < 2.5f)
				offset = 0;
			else if (offset > 0.0f)
				offset -= 2.0f;
			else
				offset += 2.0f;

			float pangYaArea = WSingleton<CPowerGauge>::Instance()->GetPangYaArea();
			GOLFDOC()->GetBar().m_startPosition = Between(pangYaArea - GolfClub().GetNarrowAccuracy(), offset, GolfClub().GetNarrowAccuracy() - pangYaArea) + data_9ef938;

			if (GolfClub().GetNarrowAccuracy() > Abs(GOLFDOC()->GetBar().m_prevAccuracy - GOLFDOC()->GetBar().m_startPosition) && GOLFDOC()->m_clockHold == 2) {
				for (int i = 0; i < 3; i++) {
					if (WAbsBySign(m_narrowFlicker[i] - 255.0f) < data_9ef928)
						m_narrowFlicker[i] = Random(225.0f, 255.0f, -1);
				}
			}
			else {
				m_narrowFlicker[0] = 255.0f;
				m_narrowFlicker[1] = 255.0f;
				m_narrowFlicker[2] = 255.0f;
			}
		}
	}

	if (m_pHolePoseSkins)
		m_pHolePoseSkins->Process(elapsed);

	float fade = m_fadeValue - elapsed;
	m_fadeValue = 0.0f < fade ? fade : 0.0f;
}

// gui_control.cpp:1139
bool CScreen::OnTrainingOptionDlgResult(int ret, FrForm* pForm) {
	CMouseCursor::Instance()->m_trainingOptionEnable = true;
	return true;
}

// gui_control.cpp:1146
void CScreen::OnDisplay(void) {
	if (m_unknown3E1) {
		m_remainTimeY = WSingleton<COption>::Instance()->gGetBar_Y();
		if (!WSingleton<CCapturedBg>::Instance()->IsCaptured()) {
			WRect rect;
			rect = WRect(12.0f, m_remainTimeY - 64.0f,
				128.0f, 64.0f);
			if (GOLFDOC()->m_gameState == 15) {
				if (WSingleton<CChatMsg>::Instance()->IsActive())
					DrawOverlay("\xB9\xF6\xC6\xB0\xC3\xA2_chat", rect, 0xFF, 0);
				else if (CItemWindow::IsClosed())
					DrawOverlay("\xB9\xF6\xC6\xB0\xC3\xA2_normal", rect, 0xFF, 0);
				else
					DrawOverlay("\xB9\xF6\xC6\xB0\xC3\xA2_item", rect, 0xFF, 0);
			}

			DrawWindInfo();
			DrawCammodeInfo();
			DrawClubInfo();

			if (IsMassGame() && WSingleton<CSharedDoc>::Instance()->m_roomType != 10) {
				DrawOverlay("\xB0\xD4\xC0\xD3\xC1\xA6\xC7\xD1\xBD\xC3\xB0\xA3", WRect(g_view->GetWidth() * 0.5f - 87.0f, g_view->GetHeight() - 32.0f, 256.0f, 32.0f), 0xFF, 0);

				IFF_STRUCT::sMatch* pMatch = ItemManager()->FindMatch(WSingleton<CSharedDoc>::Instance()->m_matchTypeId);

				if (pMatch) {
					if (WSingleton<CSharedDoc>::Instance()->m_roomType == 5) {
						char text[64];
						sprintf(text, "%s %s", pMatch->m_name, "\xC6\xC0\xC0\xFC");
						float width = m_pFont->GetTextWidth(g_view, text);
						m_pFont->Print(g_view, (g_view->GetWidth() - width) * 0.5f - 25.0f, g_view->GetHeight() - 16.0f, text, 0, 0xFFFFFFFF, 0);
					}
					else {
						char text[80] = "";
						if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomMode == 14) {
							const char* name = "\xBC\xC5\xC7\xC3\xC4\xDA\xBD\xBA";
							strcpy(text, name);
						}
						else {
							strcpy(text, pMatch->m_name);
						}
						float width = m_pFont->GetTextWidth(g_view, text);
						m_pFont->Print(g_view, (g_view->GetWidth() - width) * 0.5f - 25.0f, g_view->GetHeight() - 16.0f, text, 0, 0xFFFFFFFF, 0);
					}
				}
				else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
					float width = m_pFont->GetTextWidth(g_view, "\xB1\xE6\xB5\xE5 \xB4\xEB\xC0\xFC");
					m_pFont->Print(g_view, (g_view->GetWidth() - width) * 0.5f - 25.0f, g_view->GetHeight() - 16.0f, "\xB1\xE6\xB5\xE5 \xB4\xEB\xC0\xFC", 0, 0xFFFFFFFF, 0);
				}

				if (WSingleton<CSharedDoc>::Instance()->m_gameTimeLimit + WSingleton<CSharedDoc>::Instance()->m_gameStartTime > g_CurrentTime) {
					unsigned long sec = (WSingleton<CSharedDoc>::Instance()->m_gameTimeLimit + WSingleton<CSharedDoc>::Instance()->m_gameStartTime - g_CurrentTime) / 1000;
					m_pFont->Print(g_view, g_view->GetWidth() * 0.5f + 39.0f, g_view->GetHeight() - 16.0f, MakeStr("%02d:%02d", sec / 60, sec % 60), 0, sec < 180 ? 0xFFFF0000 : 0xFFFFFFFF, 0);
				}
				else {
					m_pFont->Print(g_view, g_view->GetWidth() * 0.5f + 39.0f, g_view->GetHeight() - 16.0f, "00:00", 0, 0xFFFF0000, 0);
				}
				m_pFont->Flush(g_view);
			}
			else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
				DrawOverlay("\xB0\xD4\xC0\xD3\xC1\xA6\xC7\xD1\xBD\xC3\xB0\xA3", WRect(g_view->GetWidth() * 0.5f - 87.0f, g_view->GetHeight() - 32.0f, 256.0f, 32.0f), 0xFF, 0);

				std::string title("\xB1\xE6\xB5\xE5 \xB4\xEB\xC0\xFC");

				float width = m_pFont->GetTextWidth(g_view, title.c_str());
				m_pFont->Print(g_view, (g_view->GetWidth() - width) * 0.5f - 25.0f, g_view->GetHeight() - 16.0f, title.c_str(), 0, 0xFFFFFFFF, 0);

				unsigned long endTime = WSingleton<CSharedDoc>::Instance()->m_gameTimeLimit + WSingleton<CSharedDoc>::Instance()->m_gameStartTime;
				if (endTime > g_CurrentTime) {
					unsigned long sec = (WSingleton<CSharedDoc>::Instance()->m_gameTimeLimit - g_CurrentTime + WSingleton<CSharedDoc>::Instance()->m_gameStartTime) / 1000;
					m_pFont->Print(g_view, g_view->GetWidth() * 0.5f + 39.0f, g_view->GetHeight() - 16.0f, MakeStr("%02d:%02d", sec / 60, sec % 60), 0, sec < 180 ? 0xFFFF0000 : 0xFFFFFFFF, 0);
				}
				else {
					m_pFont->Print(g_view, g_view->GetWidth() * 0.5f + 39.0f, g_view->GetHeight() - 16.0f, "00:00", 0, 0xFFFF0000, 0);
				}

				m_pFont->Flush(g_view);
			}
		}

		DrawApproachInfo();
		if (m_missionMarkShown[1])
			DrawOverlay("\xB9\xD9\xC5\xC1\xBF\xF8", WRect(-6.0f, m_remainTimeY - 60.0f, 128.0f, 128.0f), 0xFF, 0);

		if (m_hudVisible && m_missionMarkShown[1])
			DrawRemainTime();

		if (m_drawFlags.test(0) && GOLFDOC()->m_gameState >= 2 && m_missionMarkShown[2] && (!GOLFDOC()->m_approachMode || IsMyTurn(true)))
			DrawHitBarInfo();

		if (!(GOLFDOC()->GameMode() & 0x10)) {
			if (GOLFDOC()->m_gameState == 15 || GOLFDOC()->m_gameState == 5)
				SendMsg(this, "Item", 0x198, 0, 0, 0, 0);

			WSingleton<CChatMsg>::Instance()->PrintInScreen();
			WSingleton<CChatMsg>::Instance()->DrawChatWindow();
			WSingleton<CChatMsg>::Instance()->PrintInChatWindow();
		}

		if (m_missionMarkShown[1]) {
			DrawClubBallUI();
		}

		if (GOLFDOC()->m_gameState >= 12) {
			sOverlay* pOverlay = m_overlays.Find("\xBD\xC3\xB0\xE8");
			if (pOverlay && pOverlay->m_pOverlay)
				pOverlay->m_pOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(64.0f, m_remainTimeY + 40.0f - 16.0f, 32.0f, 32.0f), 0, 0xFFFFFFFF, 0.0f, 0);

			if (m_pRemainTimeInfo)
				m_pRemainTimeInfo->Display();

			if (GolfClub().GetType() != 3) {
				float x, y;
				int color;
				SendMsg(this, "Spin", 0x167, (intptr_t)&x, (intptr_t)&y, (intptr_t)&color, 0);
				DrawOverlay2("\xBD\xBA\xC7\xC9\xC6\xF7\xC0\xCE\xC6\xAE", WRect(x - 16.0f, y - 16.0f, 32.0f, 32.0f), color, 0);

				if (m_unknown3E3 == true) {
					SendMsg(this, "Spin", 0x168, (intptr_t)&x, (intptr_t)&y, (intptr_t)&color, 0);
					DrawOverlay2("\xBD\xBA\xC7\xC9\xC6\xF7\xC0\xCE\xC6\xAE_Red", WRect(x - 16.0f, y - 16.0f, 32.0f, 32.0f), color, 0);
				}
			}
		}

		if (!WSingleton<CCapturedBg>::Instance()->IsCaptured()) {
			if (m_pFontOverlay)
				m_pFontOverlay->Display();

			if (m_pMiniScore && (m_missionMarkShown[7] || m_missionMarkShown[3]) && !WSingleton<CProjectG>::Instance()->HidePI())
				m_pMiniScore->Display();

			if (GOLFDOC()->m_gameState == 15) {
				if (m_bannerTime > 0.0f) {
					switch (GOLFDOC()->GameMode()) {
					case 0x200:
						DrawOverlay("\xC5\xBE\xBA\xE4\xB5\xB5\xBF\xF2\xB8\xBB", WRect(0.0f, 29.0f, 256.0f, 128.0f), (unsigned char)m_bannerTime, 0);
						break;
					case 0x40000:
						DrawOverlay("\xB3\xAB\xC7\xCF\xBA\xE4\xB5\xB5\xBF\xF2\xB8\xBB", WRect(0.0f, 29.0f, 256.0f, 64.0f), (unsigned char)m_bannerTime, 0);
						break;
					case 0x100:
						DrawOverlay("\xC1\xB6\xC1\xBE\xBA\xE4\xB5\xB5\xBF\xF2\xB8\xBB", WRect(0.0f, 29.0f, 256.0f, 128.0f), (unsigned char)m_bannerTime, 0);
						break;
					}
				}
			}

			if (GOLFDOC()->GameMode() != 0x10) {
				if (WSingleton<CHelpMsg>::Instance())
					WSingleton<CHelpMsg>::Instance()->Display();
			}

			if (m_pPangInfo && !WSingleton<CProjectG>::Instance()->HidePI())
				m_pPangInfo->Display();

			if (WSingleton<CSharedDoc>::Instance()->m_gameMode == 2)
				DrawOverlay("\xBF\xAC\xBD\xC0\xB8\xF0\xB5\xE5\xBF\xC9\xBC\xC7", WRect(0.0f, 245.0f, 32.0f, 32.0f), 0xFF, 0);
		}

		DrawCommandArrow();

		if (m_pAccuracyInfo)
			m_pAccuracyInfo->Display();

		if (m_pHolePoseSkins)
			m_pHolePoseSkins->Display();

		if (m_pExtraHoleInfo)
			m_pExtraHoleInfo->Display();

		if (m_pGuildInfo)
			m_pGuildInfo->Display();

		if (m_playerCount > 0 && (GOLFDOC()->GameMode() & 2) && GOLFDOC()->IsTeeShotReady()) {
			char text[112];
			sprintf(text, "\xBE\xC6\xC1\xF7 %d\xB8\xED\xC0\xC7 \xC0\xAF\xC0\xFA\xB0\xA1 \xC1\xD8\xBA\xF1\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9.", m_playerCount);
			float width = m_pFont->GetTextWidth(g_view, text);
			m_pFont->Print(g_view, (g_view->GetWidth() - width) * 0.5f, 200.0f, text, 0, 0xFF000000, 0);
		}

		DrawTutorialUI();
	}
}

// gui_control.cpp:1405
void CScreen::AddOverlay(const char* name, const char* file, int flags) {
	sOverlay* overlay = new sOverlay;
	overlay->m_pOverlay = g_resrcmng->GetOverlay(file, flags);
	if (overlay->m_pOverlay) {
		strcpy(overlay->m_name, name);
		m_overlays.AddItem(overlay, overlay->m_name);
	} else {
		delete overlay;
	}
}

// gui_control.cpp:1418
void CScreen::DrawOverlay(const char* name, const WRect& rect, unsigned char alpha, int flags) {
	if (alpha) {
		sOverlay* overlay = m_overlays.Find(name);
		if (overlay && overlay->m_pOverlay)
			overlay->m_pOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), rect, flags, (alpha << 24) | 0xFFFFFF, 0.0f, 0);
	}
}

// gui_control.cpp:1429
void CScreen::DrawOverlay2(const char* name, const WRect& rect, int color, int flags) {
	if (color & 0xFF000000) {
		sOverlay* overlay = m_overlays.Find(name);
		if (overlay && overlay->m_pOverlay)
			overlay->m_pOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), rect, flags, color, 0.0f, 0);
	}
}

// gui_control.cpp:1441
void CScreen::HandleMsg(const MsgObject& msg) {
	switch (msg.m_msg) {
	case 0x1D0:
		if (msg.m_param1 == 1) GOLFDOC()->m_setFlag159 = true; else GOLFDOC()->m_setFlag159 = false;
		break;
	case 0x1CF:
		m_playerCount = (unsigned char)msg.m_param1;
		break;
	case 0x1CB:
		if (m_pAuxMenuUI)
			m_pAuxMenuUI->SetUnplayable();
		break;
	case 0x1CC:
		if (m_pAuxMenuUI)
			m_pAuxMenuUI->SetPause();
		break;
	case 0x1CD:
		if (m_pAuxMenuUI)
			m_pAuxMenuUI->OpenChatRecord();
		break;
	case 0x1C9:
		if (m_pExtraHoleInfo)
			m_pExtraHoleInfo->SetShotTime(GOLFDOC()->m_shotElapsed);
		break;
	case 0x1CA:
		if (m_pExtraHoleInfo)
			m_pExtraHoleInfo->UpdateLeader();
		break;
	case 0x1B9:
		if (m_pFontOverlay) {
			m_pFontOverlay->ResetTimeOutInfo();
		}
		break;
	case 0x2718:
		if (m_pBase) {
			m_accumValue = *(float*)msg.m_param1;
			m_pBase->SetAngle(m_accumValue);
		}
		break;
	case 0x1A1:
		if (m_pBase) {
			float* delta = (float*)msg.m_param1;
			m_accumValue += *delta;
			m_pBase->SetAngle(m_accumValue);
		}
		break;
	case 0x1A3:
		if (m_pBase) {
			WVector normal = GetPVS().GetMeshNormal(GolfBall().m_groundArea);
			m_pBase->SetNormal(normal);
		}
		if (m_pFontOverlay) {
			m_pFontOverlay->ResetHoleCupInfo();
		}
		m_hitBarRangeColor = 0x8000FF96;
		m_ballStateCut = GetPVS().GetMaxPowerOnGround();
		if (m_ballStateCut < 1.0f)
			m_ballStateCut -= 0.05f;
		break;
	case 0x20C: {
		WPolySoup::sPoint* pPoint = GOLFDOC()->m_pPolySoup->FindPoint((const char*)msg.m_param1);
		GolfBall().m_pos = GetPVS().GetGroundPoint(pPoint->m_pos, false, NULL, false) + WVector(0.0f, GolfBall().m_radius, 0.0f);
		GolfBall().m_groundArea = GetPVS().GetMeshIndex(GolfBall().m_pos, false);
		WVector normal = GetPVS().GetMeshNormal(GolfBall().m_groundArea);
		m_pBase->SetNormal(normal);
		break;
	}
	case 0x1A4:
		if (m_pFontOverlay) {
			m_pFontOverlay->SetActive(CFontOverlay::INFO_HOLECUP, msg.m_param1 != 0);
		}
		break;
	case 0x1A5:
		if (m_pFontOverlay) {
			m_pFontOverlay->SetActive(CFontOverlay::INFO_FLY, msg.m_param1 != 0);
		}
		break;
	case 0x1A7:
		m_drawFlags.set(0);
		break;
	case 0x1A8:
		m_drawFlags.reset(0);
		break;
	case 0x1A6:
		m_fadeValue = 1.0f;
		break;
	case 0x1A9:
		m_remainTimeColor = msg.m_param1;
		m_remainTimeArc = *(float*)msg.m_param2;
		if (m_pRemainTimeInfo)
			m_pRemainTimeInfo->SetColor(m_remainTimeColor);
		break;
	case 0x1AA:
		m_hitBarColor = ULblend(0xFF016A96, 0xFFC81E42, float2int(*(float*)msg.m_param1 * 255.0f));
		break;
	case 0x1AC:
		if (m_pFontOverlay) {
			m_pFontOverlay->SetAccuracy();
		}
		break;
	case 0x1AB:
		if (m_pAccuracyInfo)
			m_pAccuracyInfo->SetPower();
		break;
	case 0x1AD: {
		float* alpha = (float*)msg.m_param1;
		m_msgColorTime = 0xFFFFFF - ((int)(*alpha * -255.0f) << 24);
		alpha = (float*)msg.m_param2;
		m_msgColor = 0xFFFFFF - ((int)(*alpha * -255.0f) << 24);
		break;
	}
	case 0x1AE:
		m_hitBarFrom = msg.m_param1;
		m_hitBarCount = msg.m_param2;
		if (msg.m_param3) {
			m_remainTimeY = COption::Instance()->gGetBar_Y();
			float maxY, minY;
			maxY = minY = m_hitMarks[0].y = ((float*)msg.m_param3)[0];
			int i;
			for (i = 1; i < m_hitBarCount + 1; i++) {
				float y = ((float*)msg.m_param3)[i];
				m_hitMarks[i].y = y;
				if (y < minY) minY = y;
				if (y > maxY) maxY = y;
			}
			float center = (maxY - minY) * 0.5f + minY;
			float scale = (maxY - minY) > 1.6f ? 1.6f / (maxY - minY) : 1.0f;
			m_hitMarks[0].y = (m_remainTimeY - 1.0f) - (m_hitMarks[0].y - center) * scale * 5.0f;
			for (i = 1; i < m_hitBarCount + 1; i++) {
				m_hitMarks[i].y = (m_remainTimeY - 1.0f) - (m_hitMarks[i].y - center) * scale * 5.0f;
				float offset = Between(-8.0f, m_hitMarks[i].y - m_hitMarks[0].y, 8.0f);
				if (offset > 0.0f) {
					int c = (int)(255.0f - offset * 31.875f);
					m_hitMarks[i].color = (c << 16) | c | 0xFF00E500;
				}
				else {
					int c = (int)(offset * 31.875f + 255.0f);
					m_hitMarks[i].color = ((c | 0xFFFFFF00) << 8) | c;
				}
			}
		}
		break;
	case 0x1A0:
		m_hitBarTime = 0.0f;
		if (CHelpMsg::Instance())
			CHelpMsg::Instance()->OnChangeTurn();
		if (m_pPangInfo)
			m_pPangInfo->ChangeTurn();
		if (m_pBase)
			m_pBase->ChangeTurn();
		if (m_pAuxMenuUI)
			m_pAuxMenuUI->ChangeTurn();
		if (m_pCalipers) {
			m_pCalipers->ResetBar();
		}
		break;
	case 0x1AF:
		m_drawFlags.set(1);
		break;
	case 0x1B0:
		m_drawFlags.reset(1);
		break;
	case 0x1B1:
		m_cammodeAlpha = 0.0f;
		break;
	case 0x1B2:
		if (m_pFontOverlay) {
			m_pFontOverlay->ResetHoleInfo();
		}
		break;
	case 0x1B4:
		if (m_pPangInfo) {
			m_pPangInfo->PreCalc(msg.m_param1 != 0);
		}
		break;
	case 0x1B5:
		if (m_pPangInfo)
			m_pPangInfo->Restore();
		break;
	case 0x1B6:
		if (m_pPangInfo)
			m_pPangInfo->Skip();
		break;
	case 0x1B7:
		if (m_pPangInfo) {
			switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
			case 1:
				m_pPangInfo->PreCalcTeamHoleInBonus();
				break;
			case 3:
				m_pPangInfo->PreCalcMatchHoleInBonus();
				break;
			case 7:
				m_pPangInfo->ShowMyPang();
				break;
			}
			SendMsg(this, "Score", 0x132, 0, 0, 0, 0);
		}
		break;
	case 0x1B8:
		if (m_pPangInfo) {
			*(bool*)msg.m_param1 = m_pPangInfo->IsFinalized();
		}
		break;
	case 0x4C:
		if (m_pBase)
			m_pBase->ChangeBall(msg.m_param1);
		break;
	case 0x1BA:
		m_tutoMsgShown = 0;
		if (msg.m_param1) {
			m_tutoMsgBlinkTime = 0.0f;
			m_selectedIndex = GOLFDOC()->m_gameState;
			if (GOLFDOC()->m_gameState < 8 && WSingleton<CSharedDoc>::Instance()->m_roomType == 12) {
				m_turnStarted = true;
				m_turnStartTime = 0;
				CMouseCursor::Instance()->m_enable[0x21] = false;
			}
		}
		break;
	case 0x208:
		m_missionTitleState = msg.m_param1 != 0 ? 0 : 3;
		break;
	case 0x1BB:
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 11) {
			IActor* pTutorial = GetActor("Tutorial");
			if (pTutorial)
				pTutorial << MsgObject(0, 0x148, 0, 0, 0, 0, 0);
		}
		else {
			m_tutoTitleState = 0;
			m_tutoTitleTime = 0.0f;
			m_tutoTitleAlpha = 0;
		}
		m_tutorialNo = msg.m_param1;
		break;
	case 0x1BC: {
		switch (msg.m_param1) {
		case 0:
			m_missionMarkShown[GOLFDOC()->m_gameState] = false;
			m_missionMarkBlinkTime = 0.0f;
			m_missionMarkBlinkCount = 0;
			break;
		case 1:
			m_missionMarkShown[GOLFDOC()->m_gameState] = false;
			m_missionMarkBlinkCount = 3;
			break;
		case 2:
			m_missionMarkShown[GOLFDOC()->m_gameState] = true;
			break;
		}
		for (int i = 0; i < 15; i++) {
			if (i < GOLFDOC()->m_gameState)
				m_missionMarkShown[i] = true;
		}
		break;
	}
	case 0x271B:
		m_remainTimeY = COption::Instance()->gGetBar_Y();
		if (m_pBase)
			m_pBase->ResetUiPosition();
		if (m_pPangInfo)
			m_pPangInfo->ResetUiPosition();
		if (m_pFontOverlay)
			m_pFontOverlay->ResetUiPosition();
		if (m_pMiniScore)
			m_pMiniScore->ResetUiPosition();
		SendMsg(this, "Grid", 0x1D4, 0, 0, 0, 0);
		CHelpMsg::Instance()->ResetUiPosition();
		break;
	case 0x1BD:
		if (CMiniScore* pMiniScore = m_pMiniScore) {
			pMiniScore->m_state = 0; pMiniScore->m_stateTime = 1.0f; pMiniScore->m_alpha = 0xFF;
		}
		break;
	case 0x1BE:
		if (m_pFontOverlay) {
			m_pFontOverlay->FlickerHoleInfo();
		}
		break;
	case 0x1BF:
		if (m_pAuxMenuUI)
			m_pAuxMenuUI->SetActive(true, msg.m_param1);
		break;
	case 0x1C0:
		if (m_pAuxMenuUI)
			m_pAuxMenuUI->m_index = -1;
		break;
	case 0x1C1:
		if (m_pBase)
			*(WVector*)msg.m_param1 = m_pBase->GetBaseMatrix().ya;
		break;
	case 0x1C2:
		*(float*)msg.m_param1 = m_ballStateCut;
		break;
	case 0x190:
		m_unknown3E1 = msg.m_param1 == 1;
		break;
	case 0x1C3:
		if (m_pMiniScore)
			m_pMiniScore->Show();
		break;
	case 0x1C4:
		if (m_pMiniScore)
			m_pMiniScore->Hide();
		break;
	case 0x1B3:
		if (m_pFontOverlay) {
			m_pFontOverlay->FixHoleInfo();
		}
		break;
	case 0x1C5:
		if (m_pFontOverlay) {
			m_pFontOverlay->FixHoleCupInfo();
		}
		break;
	case 0x1C6:
		if (m_pFontOverlay) {
			m_pFontOverlay->GetHoleInfoPos((float*)msg.m_param1, (float*)msg.m_param2);
		}
		break;
	case 0x49:
		if (m_pMiniScore)
			m_pMiniScore->ChangeCharacter();
		break;
	case 0x64:
		if (m_pAuxMenuUI) {
			FrChatFuncDlg* pChatFuncDlg = m_pAuxMenuUI->m_pChatFuncDlg;
			if (pChatFuncDlg)
				pChatFuncDlg->AddChatMsg((const char*)msg.m_param1, msg.m_param2);
		}
		break;
	case 0x1C7:
		if (m_pHolePoseSkins)
			m_pHolePoseSkins->SetPose(msg.m_param1, msg.m_param2);
		break;
	case 0x1C8:
		if (m_pMiniScore) {
			if (m_pMiniScore->m_pShotCountInfo) m_pMiniScore->m_pShotCountInfo->StartFloating();
		}
		break;
	case 0x1CE:
		if (m_pGuildInfo)
			m_pGuildInfo->SetScore(msg.m_param1, msg.m_param2);
		break;
	case 0x202:
		if (IsLocalContent((localContentType_t)0x2F)) {
			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel = 0;
			WVector up = WVector::UNIT_POS_Y;
			m_pBase->SetNormal(up);
			m_hitBarRangeColor = 0x8000FF96;
			m_ballStateCut = GetPVS().GetMaxPowerOnGround();
			if (m_ballStateCut < 1.0f)
				m_ballStateCut -= 0.05f;
			GolfClub().SetAccuracy();
			CFxSequence* pSeq = CFx::Instance()->OpenSequence("safetee_ball.seq", true);
			SendMsg(this, "Player", 0x203, 1, 0, 0, 0);
			if (pSeq) {
				pSeq->m_pos = GolfBall().m_pos;
			}
		}
		break;
	case 0xA0:
		if (m_pCalipers) {
			m_pCalipers->Reset();
		}
		break;
	case 0xA1:
		if (m_pCalipers) {
			float* limit = (float*)msg.m_param1;
			m_pCalipers->SetCalipersLimit(*limit);
		}
		break;
	case 0x20E:
		if (GOLFDOC()->m_gameState == 11)
			m_unknown3E2 = true;
		break;
	case 0x1F0: {
		float intensity = (float)msg.m_param1;
		if (!(intensity >= 10.0f)) {
			Wind().SetIntensity(intensity);
		}
		break;
	}
	case 0x1F1:
		Wind().SetDirection((float)msg.m_param1 * data_9ef91c);
		GOLFDOC()->m_pPolySoup->UpdateBgFxWind();
		break;
	case 0x1D1:
		m_unknown3E3 = msg.m_param1 == 1;
		break;
	}
}

// gui_control.cpp:2044
CMiniScore::CMiniScore(void)
	: m_state(1)
	, m_alpha(0) {
	m_pOverlay04 = NULL; m_pOverlay1C = NULL; m_pOverlay20 = NULL; m_stateTime = 0; memset(m_pCharBitmap, 0, sizeof(m_pCharBitmap));

	for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (PLAYER(i)->m_state == 3)
			continue;

		CPartTidList& tidList = GOLFDOC()->GetPartTidList(i);

		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(tidList.m_tidChar);

		if (pChar)
			m_pCharBitmap[i] = g_pFresh->GetManager()->GetBitmap("ITEMS", pChar->m_icon);
	}

	m_pVsIcon = g_pFresh->GetManager()->GetBitmap("golf", "pc_icon_vs");

	m_pOverlay20 = g_resrcmng->GetOverlay("[nplayer.jpg", 0);
	m_pOverlay1C = g_resrcmng->GetOverlay("info_hp.jpg", 0);

	m_pBestRecord[0] = g_resrcmng->GetOverlay("[best_record_l.png", 0);
	m_pBestRecord[1] = g_resrcmng->GetOverlay("[best_record_r.png", 0);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1)
		m_pOverlay04 = g_resrcmng->GetOverlay("[info_2p.jpg", 0);
	else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7)
		m_pOverlay04 = g_resrcmng->GetOverlay("[skins_info_1p.jpg", 0);
	else
		m_pOverlay04 = g_resrcmng->GetOverlay("[info_1p.jpg", 0);

	m_pFont = WSingleton<CChatMsg>::Instance()->GetMaskedFont();

	m_pShotCountInfo = new CShotCountInfo;

	m_pSimpleUI = new CSimpleUI("[hole_out.jpg");
	m_pSimpleUI->Add("HoleOut", WRect(0.0f, 0.0f, 128.0f, 32.0f), false);
	m_pSimpleUI->Add("GiveUp", WRect(0.0f, 32.0f, 128.0f, 32.0f), false);
	m_pSimpleUI->SetAngle(-0.52359879f);

	m_iconUV = WRect(0.0f, 0.0f, 0.46875f, 1.0f);
	m_iconRect.w = 120.0f;
	m_iconRect.h = (WSingleton<CSharedDoc>::Instance()->m_roomType == 7 ? 128.0f : 64.0f) * (WSingleton<CSharedDoc>::Instance()->m_roomType == 1 ? 2.0f : 1.0f);

	m_offsetY = 0;
	m_posX = g_view->GetWidth() - 120.0f;
	m_gameMode = 0;
}

// gui_control.cpp:2105
CMiniScore::~CMiniScore(void) {
	if (m_pOverlay04 != NULL)
	{
		g_resrcmng->Release(m_pOverlay04);
		m_pOverlay04 = NULL;
	}

	if (m_pOverlay1C != NULL)
	{
		g_resrcmng->Release(m_pOverlay1C);
		m_pOverlay1C = NULL;
	}

	if (m_pOverlay20 != NULL)
	{
		g_resrcmng->Release(m_pOverlay20);
		m_pOverlay20 = NULL;
	}

	if (m_pBestRecord[0] != NULL)
	{
		g_resrcmng->Release(m_pBestRecord[0]);
		m_pBestRecord[0] = NULL;
	}

	if (m_pBestRecord[1] != NULL)
	{
		g_resrcmng->Release(m_pBestRecord[1]);
		m_pBestRecord[0] = NULL;   // the original clears [0] here
	}

	if (m_pShotCountInfo) { delete m_pShotCountInfo; m_pShotCountInfo = NULL; }
	if (m_pSimpleUI) { delete m_pSimpleUI; m_pSimpleUI = NULL; }
}

// gui_control.cpp:2141
void CMiniScore::ChangeCharacter(void) {
	switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
	case 4: case 5: case 6: case 9: case 10: {
		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_characterInfo.typeId);
		if (pChar)
			m_pCharBitmap[0] = g_pFresh->GetManager()->GetBitmap("ITEMS", pChar->m_icon);
		else
			m_pCharBitmap[0] = NULL;
		break;
	}
	default:
		for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_characterInfo.typeId);

			if (pChar)
				m_pCharBitmap[i] = g_pFresh->GetManager()->GetBitmap("ITEMS", pChar->m_icon);
			else
				m_pCharBitmap[i] = NULL;
		}
		break;
	}
}

// gui_control.cpp:2168
WRect CMiniScore::GetIconRect(int flags) {
	float x = (flags & 2) ? g_view->GetWidth() - 120.0f : 0.0f;
	float y = (flags & 1) ? 124.0f : 30.0f;
	WSize size(m_iconRect.w, m_iconRect.h);

	return WRect(WPoint(x, y), size);
}

// gui_control.cpp:2179
void CMiniScore::Process(float elapsed) {
	if (m_gameMode != GOLFDOC()->GameMode()) {
		if ((m_gameMode & ~0x38) && ((GOLFDOC()->GameMode() & 8) || (WSingleton<CSharedDoc>::Instance()->m_roomType == 7 && (GOLFDOC()->GameMode() & 0x1000)))) {
			m_state = 2; m_stateTime = 0; m_offsetY = 0; m_posX = g_view->GetWidth() - 120.0f; m_alpha = 0;
		}
		else if ((GOLFDOC()->GameMode() & ~0x3A) && (m_gameMode & 0x2A)) {
			m_state = 3;
			m_stateTime = 0;
			m_alpha = 255;
		}

		m_gameMode = GOLFDOC()->GameMode();
	}

	if (m_state == 0) {
		if (m_pShotCountInfo)
			m_pShotCountInfo->Process(elapsed);
	}

	if (m_state == 0 || m_state == 1)
		return;

	if (m_state == 2) {
		m_stateTime += elapsed * 2.0f;
		if (m_stateTime > 1.0f) {
			m_state = 0;
			m_stateTime = 1.0f;
		}

		m_alpha = (unsigned char)(m_stateTime * 255.0f);
	}
	else {
		m_stateTime += elapsed;
		if (m_stateTime > 1.0f) {
			m_state = 1;
			m_stateTime = 1.0f;
		}

		m_offsetY -= m_stateTime * 30.0f;
		m_posX += m_stateTime * 30.0f;
	}
}

// gui_control.cpp:2235
void CMiniScore::Flicker(void) {
	m_state = 0;
	m_stateTime = 1.0f;
	m_alpha = 255;
}

// gui_control.cpp:2242
void CMiniScore::Display(void) {
	if (IsLocalContent((localContentType_t)0x4F) && WSingleton<CSharedDoc>::Instance()->m_playingMode == 2) {
		GOLFDOC()->GetPlayer(0)->m_state = 0;
		m_state = 0; m_stateTime = 1.0f; m_alpha = 255;
	}

	if (m_state == 1)
		return;

	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (!pDevice)
		return;

	bool bScoreOpen = false;
	WRect rc;
	{
		MsgObject msg(0, 0x139, (intptr_t)&bScoreOpen, 0, 0, 0, 0);
		AfxGetTask()->GetActor("Score") << msg;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
		m_iconRect.y = 30.0f;
		for (unsigned char team = 0; team < 2; team++) {
			WRect uv;
			if (team) {
				uv.x = 0.5f; m_iconRect.x = m_posX; m_charRect.x = m_posX;
			} else {
				uv.x = 0.0f; m_iconRect.x = m_offsetY; m_charRect.x = m_offsetY + 120.0f - 46.0f;
			}
			m_charRect.y = m_iconRect.y + 14.0f;

			uv.y = 0.0f; uv.w = m_iconUV.w;
			uv.h = m_iconUV.h;

			unsigned long color = (team == PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute && !bScoreOpen) ? m_alpha << 24 : (m_alpha / 2) << 24;

			if (m_pOverlay04)
				m_pOverlay04->Render(g_view, uv, m_iconRect, 0, color | 0xFFFFFF, 0.0f, 0);

			int row = 0;
			for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
				if (PLAYER(i)->m_state == 3)
					continue;

				if (PLAYER(i)->m_attribute != team)
					continue;

				color = ((i == GOLFDOC()->m_myPlayerIndex && !bScoreOpen) ? m_alpha << 24 : (m_alpha / 2) << 24) | 0xFFFFFF;

				if (m_pCharBitmap[i]) {
					m_charRect.w = (float)m_pCharBitmap[i]->Width();
					m_charRect.h = (float)m_pCharBitmap[i]->Height();

					pDevice->DrawTexture(m_pCharBitmap[i], m_charRect, color, 0);
				}

				rc.x = (team ? m_posX : m_offsetY) + 5.0f;
				rc.y = row++ * 112.0f + m_iconRect.y + 2.0f;
				if (!WSingleton<CProjectG>::Instance()->HidePrivacy())
					g_pFresh->GetManager()->PrintText(rc.TopLeft(), 0, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_name, -1.0f, color);

				if (m_pVsIcon && (bool)((WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_identity >> 7) & 1)) {
					WRect rect(m_iconRect.x + 98.0f, (i % 2) * 116.0f + (m_iconRect.y - 2.0f),
						(float)m_pVsIcon->Width(), (float)m_pVsIcon->Height());
					pDevice->DrawTexture(m_pVsIcon, rect, color, 0);
				}

				if (WSingleton<CMouseCursor>::Instance()->GetArea() == i + 10) {
					char text[32];
					rc.x = team ? m_posX - 256.0f + 6.0f : m_offsetY + 120.0f - 6.0f;
					rc.y = m_charRect.y;

					if (m_pBestRecord[team])
						m_pBestRecord[team]->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(rc.x, rc.y, 256.0f, 128.0f), 0, color, 0.0f, 0);

					rc.x += team ? 110.0f : 15.0f;

					m_pFont->Print(g_view, rc.x, rc.y + 20.0f, "\xC8\xB9\xB5\xE6\xC6\xCE :", 0, color & 0xFF000000, 0);
					m_pFont->Print(g_view, rc.x, rc.y + 37.0f, "\xC3\xD6\xB4\xEB\xBA\xF1\xB0\xC5\xB8\xAE :", 0, color & 0xFF000000, 0);
					m_pFont->Print(g_view, rc.x, rc.y + 54.0f, "\xC3\xD6\xB4\xEB\xC4\xA8\xC0\xCE\xB0\xC5\xB8\xAE :", 0, color & 0xFF000000, 0);
					m_pFont->Print(g_view, rc.x, rc.y + 71.0f, "\xC3\xD6\xB4\xEB\xC6\xDB\xC6\xC3\xB0\xC5\xB8\xAE :", 0, color & 0xFF000000, 0);

					rc.x += 135.0f;
					sprintf(text, "%dp", (int)PLAYER(i)->m_ghostAward);
					rc.w = m_pFont->GetTextWidth(g_view, text);
					m_pFont->Print(g_view, rc.x - rc.w, rc.y + 20.0f, text, 0, color & 0xFF000000, 0);

					sprintf(text, "%.2fy", WSingleton<CSharedDoc>::Instance()->m_gameStatistics[i].bestDrive);
					rc.w = m_pFont->GetTextWidth(g_view, text);
					m_pFont->Print(g_view, rc.x - rc.w, rc.y + 37.0f, text, 0, color & 0xFF000000, 0);

					sprintf(text, "%.2fy", WSingleton<CSharedDoc>::Instance()->m_gameStatistics[i].longestChipIn);
					rc.w = m_pFont->GetTextWidth(g_view, text);
					m_pFont->Print(g_view, rc.x - rc.w, rc.y + 54.0f, text, 0, color & 0xFF000000, 0);

					sprintf(text, "%.2fy", WSingleton<CSharedDoc>::Instance()->m_gameStatistics[i].longestPutt);
					rc.w = m_pFont->GetTextWidth(g_view, text);
					m_pFont->Print(g_view, rc.x - rc.w, rc.y + 71.0f, text, 0, color & 0xFF000000, 0);
				}

				m_charRect.y += 49.0f;
			}

			if (GOLFDOC()->GetTeam(team)->m_teamRate > 0.0f) {
				rc.y = m_iconRect.y + 61.0f; rc.h = 4.0f;

				unsigned long barAlpha = (team == PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute && !bScoreOpen) ? m_alpha << 24 : (m_alpha / 2) << 24;
				unsigned char count = (unsigned char)(WSingleton<CSharedDoc>::Instance()->GetComboGaugeLimit(team) * 0.030303031f);

				for (unsigned char j = 0; j < count; j++) {
					float rest = GOLFDOC()->GetTeam(team)->m_teamRate - j * 33.0f;
					int fill = (int)(33.0f > rest ? rest : 33.0f);
					if (fill <= 0)
						break;

					unsigned char slot;
					unsigned long rgb;
					if (j >= 3) {
						slot = j - 3 > 0 ? 0 : j - 3;
						rgb = fill == 33.0f ? 0xFF : 0xC2FF;
					} else {
						slot = j;
						rgb = fill == 33.0f ? 0xFF0000 : 0xFFC200;
					}

					rc.w = fill * 0.60606062f;
					unsigned long barColor = rgb | barAlpha;

					if (team & 1)
						rc.x = (slot * 21.0f + 1.0f) + (m_posX + 46.0f) + 5.0f;
					else
						rc.x = m_offsetY + 120.0f - 46.0f - 5.0f - rc.w - (slot * 21.0f + 1.0f);

					if (m_pOverlay1C)
						m_pOverlay1C->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), rc, 0, barColor, 0.0f, 0);
				}
			}

			color = ((team == PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute && !bScoreOpen) ? m_alpha << 24 : (m_alpha / 2) << 24) | 0xFFFFFF;
			rc.x = (team ? m_posX + 46.0f : m_offsetY) + 56.0f;
			if (m_pShotCountInfo) {

				m_pShotCountInfo->SetColor(color);
				m_pShotCountInfo->Display(rc.x, m_iconRect.y + 69.0f, team);
			}

			m_pFont->Flush(g_view);

			float stateX = !team ? m_offsetY + 35.0f : m_posX + 82.0f;

			if (m_pSimpleUI) {

				unsigned char alpha = (unsigned char)(m_alpha * 0.5f);

				m_pSimpleUI->SetColor((alpha << 24) | 0xFFFFFF);

				if (GOLFDOC()->GetTeam(team)->m_state == 1) {

					if ((unsigned char)GOLFDOC()->GetTeam(team)->m_holes[GOLFDOC()->m_currentHole - 1] > (unsigned char)(GOLFDOC()->GetHoleData().m_par + 4))
						m_pSimpleUI->Render("GiveUp", stateX, 80.0f, 1.0f, 0x22);
					else if (PLAYER(team)->m_holes[GOLFDOC()->m_currentHole - 1] > (unsigned char)0)
						m_pSimpleUI->Render("HoleOut", stateX, 80.0f, 1.0f, 0x22);
				} else if (team == PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_state == 0) {

					if (GOLFDOC()->GameMode() & 0x1000)
						m_pSimpleUI->Render("HoleOut", stateX, 80.0f, 1.0f, 0x22);
					else if (GOLFDOC()->GameMode() & 0x80000)
						m_pSimpleUI->Render("GiveUp", stateX, 80.0f, 1.0f, 0x22);
				}
			}
		}
	} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			if (PLAYER(i)->m_state == 3)
				continue;

			m_iconRect.y = (i & 1) ? 124.0f : 30.0f;

			WRect uv;
			if (i & 2) {
				uv.x = 0.5f; m_iconRect.x = m_posX; m_charRect.x = m_posX;
			} else {
				uv.x = 0.0f; m_iconRect.x = m_offsetY; m_charRect.x = m_offsetY + 120.0f - 46.0f;
			}
			m_charRect.y = m_iconRect.y + 14.0f;

			unsigned long color = ((i == GOLFDOC()->m_myPlayerIndex && !bScoreOpen) ? m_alpha << 24 : (m_alpha / 2) << 24) | 0xFFFFFF;

			uv.y = 0.0f; uv.w = m_iconUV.w;
			uv.h = m_iconUV.h;

			if (m_pOverlay04)
				m_pOverlay04->Render(g_view, uv, m_iconRect, 0, color, 0.0f, 0);

			if (m_pCharBitmap[i]) {
				m_charRect.w = (float)m_pCharBitmap[i]->Width();
				m_charRect.h = (float)m_pCharBitmap[i]->Height();

				pDevice->DrawTexture(m_pCharBitmap[i], m_charRect, color, 0);
			}

			rc.x = ((i & 2) ? m_posX : m_offsetY) + 25.0f;
			rc.y = m_iconRect.y + 2.0f;

			if (!WSingleton<CProjectG>::Instance()->HidePrivacy())
				g_pFresh->GetManager()->PrintText(rc.TopLeft(), 0, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_name, 84.0f, color);

			if (m_pVsIcon && (bool)((WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_identity >> 7) & 1)) {
				WRect rect(m_iconRect.x + 98.0f, m_iconRect.y - 2.0f,
					(float)m_pVsIcon->Width(), (float)m_pVsIcon->Height());
				pDevice->DrawTexture(m_pVsIcon, rect, color, 0);
			}

			if (PLAYER(i)->m_comboGauge > 0.0f) {
				rc.y = m_iconRect.y + 21.0f; rc.h = 4.0f;

				unsigned long barAlpha = (i == GOLFDOC()->m_myPlayerIndex && !bScoreOpen) ? m_alpha << 24 : (m_alpha / 2) << 24;
				unsigned char count = (unsigned char)(WSingleton<CSharedDoc>::Instance()->GetComboGaugeLimit(i) * 0.030303031f);

				for (unsigned char j = 0; j < count; j++) {
					float rest = PLAYER(i)->m_comboGauge - j * 33.0f;
					int fill = (int)(33.0f > rest ? rest : 33.0f);
					if (fill <= 0)
						break;

					unsigned char slot;
					unsigned long rgb;
					if (j >= 3) {
						slot = j - 3 > 0 ? 0 : j - 3;
						rgb = fill == 33.0f ? 0xFF : 0xC2FF;
					} else {
						slot = j;
						rgb = fill == 33.0f ? 0xFF0000 : 0xFFC200;
					}

					rc.w = fill * 0.60606062f;
					unsigned long barColor = rgb | barAlpha;

					if (i & 2)
						rc.x = (slot * 21.0f + 1.0f) + (m_posX + 46.0f) + 5.0f;
					else
						rc.x = m_offsetY + 120.0f - 46.0f - 5.0f - rc.w - (slot * 21.0f + 1.0f);

					if (m_pOverlay1C)
						m_pOverlay1C->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), rc, 0, barColor, 0.0f, 0);
				}
			}

			color = ((i == GOLFDOC()->m_myPlayerIndex && !bScoreOpen) ? m_alpha << 24 : (m_alpha / 2) << 24) | PLAYER(i)->m_color;

			uv.x = (i & 1) ? 0.5f : 0.0f;
			uv.y = (i & 2) ? 0.25f : 0.0f;
			uv.w = 0.5f; uv.h = 0.25f;
			rc.x = ((i & 2) ? m_posX : m_offsetY) - 4.0f;
			rc.y = m_iconRect.y - 3.0f; rc.w = 32.0f; rc.h = 16.0f;

			if (m_pOverlay20)
				m_pOverlay20->Render(g_view, uv, rc, 0, color, 0.0f, 0);

			color = ((i == GOLFDOC()->m_myPlayerIndex && !bScoreOpen) ? m_alpha << 24 : (m_alpha / 2) << 24) | 0xFFFFFF;
			rc.x = ((i & 2) ? m_posX + 46.0f : m_offsetY) + 56.0f;
			if (m_pShotCountInfo) {

				m_pShotCountInfo->SetColor(color);
				m_pShotCountInfo->Display(rc.x, m_iconRect.y + 29.0f, i);
			}

			m_pFont->Flush(g_view);

			float x = (i & 2) ? m_posX + 62.0f : m_offsetY + 51.0f;
			float y = (i & 1) ? 157.0f : 62.0f;

			if (m_pSimpleUI) {

				unsigned char alpha = (unsigned char)(m_alpha * 0.5f);

				m_pSimpleUI->SetColor((alpha << 24) | 0xFFFFFF); if (PLAYER(i)->m_state == 1) {

					if (PLAYER(i)->m_holes[GOLFDOC()->m_currentHole - 1] > (unsigned char)(GOLFDOC()->GetHoleData().m_par + 4))
						m_pSimpleUI->Render("GiveUp", x, y, 1.0f, 0x22);
					else if (PLAYER(i)->m_holes[GOLFDOC()->m_currentHole - 1] > (unsigned char)0)
						m_pSimpleUI->Render("HoleOut", x, y, 1.0f, 0x22);
				} else if (i == GOLFDOC()->m_myPlayerIndex && PLAYER(i)->m_state == 0) {

					if (GOLFDOC()->GameMode() & 0x1000)
						m_pSimpleUI->Render("HoleOut", x, y, 1.0f, 0x22);
					else if (GOLFDOC()->GameMode() & 0x80000)
						m_pSimpleUI->Render("GiveUp", x, y, 1.0f, 0x22);
				}
			}
		}
	} else {
		for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			if (PLAYER(i)->m_state == 3)
				continue;

			m_iconRect.y = (i & 1) ? 124.0f : 30.0f;

			WRect uv;
			if (i & 2) {
				m_charRect.x = m_iconRect.x = m_posX;
				uv.x = 0.5f;
			} else {
				m_iconRect.x = m_offsetY;
				m_charRect.x = m_offsetY + 120.0f - 46.0f;
				uv.x = 0.0f;
			}
			m_charRect.y = m_iconRect.y + 14.0f;

			unsigned long color = ((i == GOLFDOC()->m_myPlayerIndex && !bScoreOpen) ? m_alpha << 24 : (m_alpha / 2) << 24) | 0xFFFFFF;

			uv.y = 0.0f; uv.w = m_iconUV.w;
			uv.h = m_iconUV.h;

			if (m_pOverlay04)
				m_pOverlay04->Render(g_view, uv, m_iconRect, 0, color, 0.0f, 0);

			if (m_pCharBitmap[i]) {
				m_charRect.w = (float)m_pCharBitmap[i]->Width();
				m_charRect.h = (float)m_pCharBitmap[i]->Height();

				pDevice->DrawTexture(m_pCharBitmap[i], m_charRect, color, 0);
			}

			rc.x = ((i & 2) ? m_posX : m_offsetY) + 25.0f;
			rc.y = m_iconRect.y + 2.0f;

			if (!WSingleton<CProjectG>::Instance()->HidePrivacy())
				g_pFresh->GetManager()->PrintText(rc.TopLeft(), 0, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_name, 84.0f, color);

			if (m_pVsIcon && (bool)((WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_identity >> 7) & 1)) {
				WRect rect(m_iconRect.x + 98.0f, m_iconRect.y - 2.0f,
					(float)m_pVsIcon->Width(), (float)m_pVsIcon->Height());
				pDevice->DrawTexture(m_pVsIcon, rect, color, 0);
			}

			if (PLAYER(i)->m_comboGauge > 0.0f) {
				rc.y = m_iconRect.y + 21.0f; rc.h = 4.0f;

				unsigned long barAlpha = (i == GOLFDOC()->m_myPlayerIndex && !bScoreOpen) ? m_alpha << 24 : (m_alpha / 2) << 24;
				unsigned char count = (unsigned char)(WSingleton<CSharedDoc>::Instance()->GetComboGaugeLimit(i) * 0.030303031f);

				for (unsigned char j = 0; j < count; j++) {
					float rest = PLAYER(i)->m_comboGauge - j * 33.0f;
					int fill = (int)(33.0f > rest ? rest : 33.0f);
					if (fill <= 0)
						break;

					unsigned char slot;
					unsigned long rgb;
					if (j >= 3) {
						slot = j - 3 > 0 ? 0 : j - 3;
						rgb = fill == 33.0f ? 0xFF : 0xC2FF;
					} else {
						slot = j;
						rgb = fill == 33.0f ? 0xFF0000 : 0xFFC200;
					}

					rc.w = fill * 0.60606062f;
					unsigned long barColor = rgb | barAlpha;

					if (i & 2)
						rc.x = (slot * 21.0f + 1.0f) + (m_posX + 46.0f) + 5.0f;
					else
						rc.x = m_offsetY + 120.0f - 46.0f - 5.0f - rc.w - (slot * 21.0f + 1.0f);

					if (m_pOverlay1C)
						m_pOverlay1C->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), rc, 0, barColor, 0.0f, 0);
				}
			}

			if (i == MyIndex()) {
				int hasExtra = 0;
				float extraGauge = 0.0f;
				MsgObject msg(0, 0x129, (intptr_t)&hasExtra, (intptr_t)&extraGauge, 0, 0, 0);
				AfxGetTask()->GetActor("Player") << msg;

				if (hasExtra) {
					rc.y = m_iconRect.y + 21.0f; rc.h = 4.0f;

					for (unsigned char j = 0; j < 3; j++) {
						int fill = (int)(33.0f > extraGauge - j * 33.0f ? extraGauge - j * 33.0f : 33.0f);
						if (fill <= 0)
							break;

						unsigned long barColor;
						if (fill == 33.0f)
							barColor = ((i == GOLFDOC()->m_myPlayerIndex && !bScoreOpen) ? m_alpha << 24 : (m_alpha / 2) << 24) | 0xFF0000;
						else
							barColor = ((i == GOLFDOC()->m_myPlayerIndex && !bScoreOpen) ? m_alpha << 24 : (m_alpha / 2) << 24) | 0xFFC200;

						rc.w = fill * 0.60606062f;
						if (i & 2)
							rc.x = (j * 21.0f + 1.0f) + (m_posX + 46.0f) + 5.0f;
						else
							rc.x = m_offsetY + 120.0f - 46.0f - 5.0f - rc.w - (j * 21.0f + 1.0f);

						if (m_pOverlay1C)
							m_pOverlay1C->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), rc, 0, barColor, 0.0f, 0);
					}
				}
			}

			color = ((i == GOLFDOC()->m_myPlayerIndex && !bScoreOpen) ? m_alpha << 24 : (m_alpha / 2) << 24) | PLAYER(i)->m_color;

			uv.x = (i & 1) ? 0.5f : 0.0f;
			uv.y = (i & 2) ? 0.25f : 0.0f;
			uv.w = 0.5f; uv.h = 0.25f;
			rc.x = ((i & 2) ? m_posX : m_offsetY) - 4.0f;
			rc.y = m_iconRect.y - 3.0f; rc.w = 32.0f; rc.h = 16.0f;

			if (m_pOverlay20)
				m_pOverlay20->Render(g_view, uv, rc, 0, color, 0.0f, 0);

			color = ((i == GOLFDOC()->m_myPlayerIndex && !bScoreOpen) ? m_alpha << 24 : (m_alpha / 2) << 24) | 0xFFFFFF;
			rc.x = ((i & 2) ? m_posX + 46.0f : m_offsetY) + 56.0f;
			if (m_pShotCountInfo) {

				m_pShotCountInfo->SetColor(color);
				m_pShotCountInfo->Display(rc.x, m_iconRect.y + 29.0f, i);
			}

			if (WSingleton<CMouseCursor>::Instance()->GetArea() == i + 10) {

				if (IsLocalContent((localContentType_t)0x4F) && WSingleton<CSharedDoc>::Instance()->m_playingMode == 2)
					break;

				char text[32];
				rc.x = (i & 2) ? m_posX - 256.0f + 6.0f : m_offsetY + 120.0f - 6.0f;
				rc.y = m_iconRect.y;

				m_pBestRecord[(i >> 1) & 1]->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(rc.x, rc.y, 256.0f, 128.0f), 0, color, 0.0f, 0);

				rc.x += (i & 2) ? 110.0f : 15.0f;

				// "game pang :"
				m_pFont->Print(g_view, rc.x, rc.y + 20.0f, "\xC8\xB9\xB5\xE6\xC6\xCE :", 0, color & 0xFF000000, 0);
				switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
				case 4: case 5: case 6: case 9: case 10:
					// "total distance :"
					m_pFont->Print(g_view, rc.x, rc.y + 37.0f, "\xB4\xA9\xC0\xFB\xBA\xF1\xB0\xC5\xB8\xAE :", 0, color & 0xFF000000, 0);
					break;
				default:
					// "longest drive :"
					m_pFont->Print(g_view, rc.x, rc.y + 37.0f, "\xC3\xD6\xB4\xEB\xBA\xF1\xB0\xC5\xB8\xAE :", 0, color & 0xFF000000, 0);
					break;
				}
				// "longest chip-in :"
				m_pFont->Print(g_view, rc.x, rc.y + 54.0f, "\xC3\xD6\xB4\xEB\xC4\xA8\xC0\xCE\xB0\xC5\xB8\xAE :", 0, color & 0xFF000000, 0);
				// "longest putt :"
				m_pFont->Print(g_view, rc.x, rc.y + 71.0f, "\xC3\xD6\xB4\xEB\xC6\xDB\xC6\xC3\xB0\xC5\xB8\xAE :", 0, color & 0xFF000000, 0);

				rc.x += 135.0f;
				sprintf(text, "%dp", (int)PLAYER(i)->m_ghostAward);
				rc.w = m_pFont->GetTextWidth(g_view, text);
				m_pFont->Print(g_view, rc.x - rc.w, rc.y + 20.0f, text, 0, color & 0xFF000000, 0);

				switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
				case 4: case 5: case 6: case 9: case 10:
					sprintf(text, "%dy", WSingleton<CSharedDoc>::Instance()->m_gameStatistics[i].totalDistance);
					break;
				default:
					sprintf(text, "%.2fy", WSingleton<CSharedDoc>::Instance()->m_gameStatistics[i].bestDrive);
					break;
				}
				rc.w = m_pFont->GetTextWidth(g_view, text);
				m_pFont->Print(g_view, rc.x - rc.w, rc.y + 37.0f, text, 0, color & 0xFF000000, 0);

				sprintf(text, "%.2fy", WSingleton<CSharedDoc>::Instance()->m_gameStatistics[i].longestChipIn);
				rc.w = m_pFont->GetTextWidth(g_view, text);
				m_pFont->Print(g_view, rc.x - rc.w, rc.y + 54.0f, text, 0, color & 0xFF000000, 0);

				sprintf(text, "%.2fy", WSingleton<CSharedDoc>::Instance()->m_gameStatistics[i].longestPutt);
				rc.w = m_pFont->GetTextWidth(g_view, text);
				m_pFont->Print(g_view, rc.x - rc.w, rc.y + 71.0f, text, 0, color & 0xFF000000, 0);
			}

			m_pFont->Flush(g_view);

			float x = (i & 2) ? m_posX + 62.0f : m_offsetY + 51.0f;
			float y = (i & 1) ? 157.0f : 62.0f;

			if (m_pSimpleUI) {

				unsigned char alpha = (unsigned char)(m_alpha * 0.5f);

				m_pSimpleUI->SetColor((alpha << 24) | 0xFFFFFF); if (PLAYER(i)->m_state == 1) {

					if (PLAYER(i)->m_holes[GOLFDOC()->m_currentHole - 1] > (unsigned char)(GOLFDOC()->GetHoleData().m_par + 4))
						m_pSimpleUI->Render("GiveUp", x, y, 1.0f, 0x22);
					else if (PLAYER(i)->m_holes[GOLFDOC()->m_currentHole - 1] > (unsigned char)0)
						m_pSimpleUI->Render("HoleOut", x, y, 1.0f, 0x22);
				} else if (i == GOLFDOC()->m_myPlayerIndex && PLAYER(i)->m_state == 0) {

					if (GOLFDOC()->GameMode() & 0x1000)
						m_pSimpleUI->Render("HoleOut", x, y, 1.0f, 0x22);
					else if (GOLFDOC()->GameMode() & 0x80000)
						m_pSimpleUI->Render("GiveUp", x, y, 1.0f, 0x22);
				}
			}
		}
	}

	if (WSingleton<CHeadIcon>::Instance())
		WSingleton<CHeadIcon>::Instance()->DisplayBesideBoard(m_offsetY + 120.0f, m_posX);
}

// gui_control.cpp:2845
void CMiniScore::ResetUiPosition(void) {
	if (m_state == 0 || m_state == 2)
		m_posX = g_view->GetWidth() - 120.0f;
}

// gui_control.cpp:2851
void CMiniScore::Show(void) {
	m_state = 2; m_stateTime = 0;
	m_offsetY = 0;
	m_posX = g_view->GetWidth() - 120.0f;
	m_alpha = 0;
}

// gui_control.cpp:2860
void CMiniScore::Hide(void) {
	m_state = 1;
	m_stateTime = 1.0f;
	m_offsetY = -120.0f;
	m_posX = g_view->GetWidth() - 120.0f + 120.0f;
	m_alpha = 255;
}

// gui_control.cpp:2869
void CMiniScore::StartFloating(void) {
	if (m_pShotCountInfo)
		m_pShotCountInfo->StartFloating();
}

// gui_control.cpp:2881
CAuxMenuUI::CAuxMenuUI(void)
	: m_pBase(NULL)
	, m_pDropDlg(NULL)
	, m_pDropFailDlg(NULL)
	, m_pChatFuncDlg(NULL) {
	m_pBase = g_resrcmng->GetOverlay("[option_base.png", 0x80000000);
	m_pItemOverlay[0] = g_resrcmng->GetOverlay("[option_black.png", 0x80000000);
	m_pItemOverlay[1] = g_resrcmng->GetOverlay("[option_white.png", 0x80000000);

	m_active = 0; m_fade = 0; m_unknown28 = 2;
}

// gui_control.cpp:2893
CAuxMenuUI::~CAuxMenuUI(void) {
	if (g_resrcmng && m_pBase) { g_resrcmng->Release(m_pBase); m_pBase = 0; }
	if (g_resrcmng && m_pItemOverlay[0]) { g_resrcmng->Release(m_pItemOverlay[0]); m_pItemOverlay[0] = 0; }
	if (g_resrcmng && m_pItemOverlay[1]) { g_resrcmng->Release(m_pItemOverlay[1]); m_pItemOverlay[1] = 0; }
}

// gui_control.cpp:2900
void CAuxMenuUI::Process(float elapsed) {
	m_hoverIndex = WSingleton<CMouseCursor>::Instance()->GetArea() - 0x27;

	switch (m_active) {
	case 0:
		if (WSingleton<CMouseCursor>::Instance()->GetArea() == 0x26 && g_input->GetButton(LEFT_BUTTON) == 2) {

			m_active = 1;
		}
		break;
	case 1:
		m_fade += elapsed * 5.0f;
		if (m_fade > 1.0f) {
			m_fade = 1.0f;
			m_active = 2;

			WSingleton<CMouseCursor>::Instance()->m_uiEnable[0] = true;
			WSingleton<CMouseCursor>::Instance()->m_uiEnable[1] = true;
			WSingleton<CMouseCursor>::Instance()->m_uiEnable[2] = true;
			WSingleton<CMouseCursor>::Instance()->m_uiEnable[3] = true;
			WSingleton<CMouseCursor>::Instance()->m_uiEnable[4] = true;
			WSingleton<CMouseCursor>::Instance()->m_uiEnable[5] = true;
		}
		break;
	case 2:
		if (m_hoverIndex < 0 || m_hoverIndex > 5) {

			WSingleton<CMouseCursor>::Instance()->m_uiEnable[0] = false;
			WSingleton<CMouseCursor>::Instance()->m_uiEnable[1] = false;
			WSingleton<CMouseCursor>::Instance()->m_uiEnable[2] = false;
			WSingleton<CMouseCursor>::Instance()->m_uiEnable[3] = false;
			WSingleton<CMouseCursor>::Instance()->m_uiEnable[4] = false;
			WSingleton<CMouseCursor>::Instance()->m_uiEnable[5] = false;
			m_active = 3;
		}
		else if (g_input->GetButton(LEFT_BUTTON) == 2) {
			switch (m_hoverIndex) {
			case 0:
				if (WSingleton<CQuitWindow>::Instance())
					WSingleton<CQuitWindow>::Instance()->CreateOptionWindow();
				break;
			case 1:
				AfxGetTask()->GetActor("GuideBook") << MsgObject(0, 0x12A, 0, 0, 0, 0, 0);
				break;
			case 2:
				SetUnplayable();
				break;
			case 3:
				SetPause();
				break;
			case 4:
				OpenChatRecord();
				break;
			}
		}
		break;
	case 3:
		m_fade -= elapsed * 5.0f;
		if (m_fade < 0.0f) {
			m_fade = 0;
			m_active = 0;
		}
		break;
	}
}

// gui_control.cpp:2986
void CAuxMenuUI::SetUnplayable(void) {
	if (GOLFDOC()->m_dropPending == false && !GalleryMode() && IsMyTurn(false)) {

		WSingleton<CMouseCursor>::Instance()->m_uiEnable[2] = false;

		bool overlapped = GetPVS().OverlappedWithObject(GolfBall().m_pos, 0);

		if (!overlapped && (GolfBall().m_groundType == 0 || GolfBall().m_groundType == 1 || GolfBall().m_groundType == 2)) {

			if (m_pDropFailDlg != NULL) return;

			m_pDropFailDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);

			switch (GolfBall().m_groundType) {
			case 0:

				// "An unplayable cannot be declared at the tee."
				m_pDropFailDlg->SetMessage("\xC6\xBC\xBC\xA6\xC0\xA7\xC4\xA1\xBF\xA1\xBC\xAD\xB4\xC2 \xBE\xF0\xC7\xC3\xB7\xB9\xC0\xCC\xBE\xEE\xBA\xED\xC0\xBB \xBC\xB1\xBE\xF0 \xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
				break;
			case 1:

				// "An unplayable cannot be declared on the fairway."
				m_pDropFailDlg->SetMessage("\xC6\xE4\xBE\xEE\xBF\xFE\xC0\xCC\xBF\xA1\xBC\xAD\xB4\xC2 \xBE\xF0\xC7\xC3\xB7\xB9\xC0\xCC\xBE\xEE\xBA\xED\xC0\xBB \xBC\xB1\xBE\xF0 \xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
				break;
			case 2:

				// "An unplayable cannot be declared on the green."
				m_pDropFailDlg->SetMessage("\xB1\xD7\xB8\xB0\xBF\xA1\xBC\xAD\xB4\xC2 \xBE\xF0\xC7\xC3\xB7\xB9\xC0\xCC\xBE\xEE\xBA\xED\xC0\xBB \xBC\xB1\xBE\xF0 \xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
				break;
			}

			m_pDropFailDlg->Open((FRESH_RESULT_CALLBACK)&CAuxMenuUI::OnDropFailDlgResult, 4);
			m_pDropFailDlg->SetTimeLimit(3.0f);
		}
		else {

			GOLFDOC()->m_dropPending = true;

			if (GOLFDOC()->GameMode() & ~0x200)
				AfxGetTask()->GetActor("GolfRule") << MsgObject(0, 0x78, 0x200, 0, 0, 0, 0);

			g_camera.pivot = GetPVS().GetGroundPoint(GolfBall().m_pos, false, NULL, false);
			g_camera.pivot.y += 192.0f;
		}
	}
}

// gui_control.cpp:3032
void CAuxMenuUI::SetPause(void) {
	switch (GetGameType()) {
	case 4: case 5: case 6: case 9: case 10:
		return;
	}
	if (GetShotTimeLimit() == 0)
		return;

	WSingleton<CMouseCursor>::Instance()->m_uiEnable[3] = false;

	if (m_unknown28 == 0) {
		// "Pausing the game can be used only twice per game."
		AfxGetTask()->PassMsgToActor("GolfRule", MsgObject(0, 0x23, (intptr_t)"\xB0\xD4\xC0\xD3\xC1\xA4\xC1\xF6\xB4\xC2 \xC7\xD1 \xB0\xD4\xC0\xD3\xBF\xA1 2\xB9\xF8\xB8\xB8 \xBB\xE7\xBF\xEB\xC7\xD2 \xBC\xF6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
		return;
	}

	m_unknown28--;

	if (OnlinePlay()) {
		WSendPacket packet(GCT_PAUSE);
		packet.Encode1(1);
		packet.Send(SEND_TO_DEFAULT);
	}
	else {
		// "The game is paused.\nPress OK to continue the game."
		AfxGetTask()->PassMsgToActor("GolfRule", MsgObject(0, 0x84, 1, (intptr_t)"\xC7\xF6\xC0\xE7 \xB0\xD4\xC0\xD3\xC0\xCC \xC0\xE1\xBD\xC3 \xC1\xDF\xB4\xDC\xB5\xC8 \xBB\xF3\xC5\xC2\xC0\xD4\xB4\xCF\xB4\xD9.\n\xB0\xD4\xC0\xD3\xC0\xBB \xB0\xE8\xBC\xD3\xC7\xCF\xB1\xE2 \xC0\xA7\xC7\xD8\xBC\xAD\xB4\xC2 \xC8\xAE\xC0\xCE\xC0\xBB \xB4\xAD\xB7\xAF\xC1\xD6\xBC\xBC\xBF\xE4.", 0, 0, 0));
	}
}

// gui_control.cpp:3059
void CAuxMenuUI::OpenChatRecord(void) {
	if (OnlinePlay()) {
		if (m_pChatFuncDlg == NULL) {
			m_pChatFuncDlg = CreateForm<FrChatFuncDlg>(g_pFresh->GetManager(), this, "chat_func", 0);
			m_pChatFuncDlg->Open((FRESH_RESULT_CALLBACK)&CAuxMenuUI::OnChatFuncDlgResult, 5);
		}
	}
}

// gui_control.cpp:3071
bool CAuxMenuUI::OnDropDlgResult(int ret, FrForm* pForm) {
	if (ret == 1) {

		if (OnlinePlay()) {
			WSendPacket packet(GCT_DROP);
			WVector pos = GOLFDOC()->m_pAutoTarget->GetDropPos();
			packet.EncodeBuffer(&pos, sizeof(WVector));
			packet.Send(SEND_TO_DEFAULT);
		}
		else {
			GolfBall().m_pos = GOLFDOC()->m_pAutoTarget->GetDropPos();
			AfxGetTask()->PassMsgToActor("GolfRule", MsgObject(0, 0x8D, 0, 0, 0, 0, 0));
		}
	}

	WSingleton<CMouseCursor>::Instance()->m_uiEnable[2] = true;

	m_pDropDlg = NULL;
	return true;
}

// gui_control.cpp:3094
bool CAuxMenuUI::OnDropFailDlgResult(int ret, FrForm* pForm) {
	WSingleton<CMouseCursor>::Instance()->m_uiEnable[2] = true;

	m_pDropFailDlg = NULL;
	return true;
}

// gui_control.cpp:3102
void CAuxMenuUI::Display(void) {
	if (!CMouseCursor::Instance()->IsVisible())
		return;

	unsigned long color;
	if (m_active == 0) {
		switch (CMouseCursor::Instance()->GetArea()) {
		case 0x25:
			color = 0x80FFFFFF;
			break;
		case 0x26:
			color = 0xFFFFFFFF;
			break;
		default:
			color = 0x80FFFFFF;
			break;
		}
	}
	else
		color = 0xFFFFFFFF;

	if (m_pBase)
		m_pBase->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(g_view->GetWidth() - 128.0f + (1.0f - m_fade) * 79.0f, g_view->GetHeight() * 0.5f - 66.0f, 128.0f, 256.0f), 0x2000000, color, 0.0f, 0);

	for (int i = 0; i < 6; i++) {
		if (m_pItemOverlay[i == m_hoverIndex || i == m_index])
			m_pItemOverlay[i == m_hoverIndex || i == m_index]->Render(g_view, WRect(0.0f, i * 0.0859375f, 1.0f, 0.0859375f), WRect(g_view->GetWidth() - m_fade * 79.0f, g_view->GetHeight() * 0.5f + (i - 3) * 22.0f, 128.0f, 22.0f), 0, 0xFFFFFFFF, 0.0f, 0);
	}
}

// gui_control.cpp:3139
bool CAuxMenuUI::OnChatFuncDlgResult(int ret, FrForm* pForm) {
	m_pChatFuncDlg = NULL;
	return true;
}

// gui_control.cpp:3145
void CAuxMenuUI::SetActive(bool active, int index) {
	if (active) {
		m_index = index; m_active = 1;

		WSingleton<CMouseCursor>::Instance()->m_uiEnable[index] = true;
		WSingleton<CMouseCursor>::Instance()->ForceMove(g_view->GetWidth() - 50.0f, ((index - 3) + 0.5f) * 22.0f + g_view->GetHeight() * 0.5f, -1.0f);
	} else {
		m_index = -1;
	}
}

// gui_control.cpp:3161
void CAuxMenuUI::ChangeTurn(void) {
	m_index = -1; m_active = 0; m_fade = 0;

	WSingleton<CMouseCursor>::Instance()->m_uiEnable[0] = false;
	WSingleton<CMouseCursor>::Instance()->m_uiEnable[1] = false;
	WSingleton<CMouseCursor>::Instance()->m_uiEnable[2] = false;
	WSingleton<CMouseCursor>::Instance()->m_uiEnable[3] = false;
	WSingleton<CMouseCursor>::Instance()->m_uiEnable[4] = false;
	WSingleton<CMouseCursor>::Instance()->m_uiEnable[5] = false;
}

// gui_control.cpp:3183
CDrivingDistInfo::CDrivingDistInfo(void)
	: m_pPlayerOverlay(NULL)
	, m_pTopOverlay(NULL)
	, m_unknown08(true)
	, m_unknown09(false)
	, m_bestPlayer(0xFF)
	, m_unknown0B(0xFF)
	, m_bestColor(0xFFFFFFFF)
	, m_myColor(0xFFFFFFFF)
	, m_shownDistance(0)
	, m_bestDistance(0)
	, m_myDistance(0) {
	m_pPlayerOverlay = g_resrcmng->GetOverlay("[nplayer.jpg", 0);
	m_pTopOverlay = g_resrcmng->GetOverlay("[top_drv.jpg", 0);
}

// gui_control.cpp:3189
CDrivingDistInfo::~CDrivingDistInfo(void) {
	if (g_resrcmng && m_pPlayerOverlay) { g_resrcmng->Release(m_pPlayerOverlay); m_pPlayerOverlay = 0; }
	if (g_resrcmng && m_pTopOverlay) { g_resrcmng->Release(m_pTopOverlay); m_pTopOverlay = 0; }
}

// The longest drive of player `index`: the float at +0x10 of
// CSharedDoc::m_gameStatistics[index] (0x4118, 0xEB bytes each). Both are out
// of reach here: the member is protected (CSharedDoc does not befriend
// CDrivingDistInfo yet) and globalgamedefine.h covers +0x10 with UNKNOWN.
#define GAME_LONGEST_DRIVE(index) \
	(*(const float*)((const char*)WSingleton<CSharedDoc>::Instance() + 0x4118 + (index) * 0xEB + 0x10))

// gui_control.cpp:3195
void CDrivingDistInfo::OnChangeTurn(void) {
	m_unknown08 = true;

	m_myDistance = GAME_LONGEST_DRIVE(MyIndex());

	if (!(Wabs(m_bestDistance) < g_EPSILON)) {
		m_bestPlayer = m_unknown0B = 0;
		m_bestDistance = GAME_LONGEST_DRIVE(0);

		for (unsigned char i = 1; i < GOLFDOC()->GetPlayerNum(); i++) {
			if (m_bestDistance < GAME_LONGEST_DRIVE(i)) {
				m_bestPlayer = m_unknown0B = i;
				m_bestDistance = GAME_LONGEST_DRIVE(i);
			}
		}

		m_shownDistance = m_bestDistance;
	}
}

// gui_control.cpp:3222
void CDrivingDistInfo::Process(float elapsed) {
	if (!m_unknown08)
		return;

	static float s_blinkTime = 0;
	s_blinkTime += elapsed;

	if (s_blinkTime > (m_unknown09 ? 1.5f : 2.0f)) {
		s_blinkTime = 0; m_unknown09 = !m_unknown09;
	}

	m_bestColor = m_myColor = 0xFFFFFFFF;

	if (GolfBall().m_state == 0 && GolfBall().m_landed) {
		if (m_bestDistance > m_shownDistance) {
			if (m_bestDistance - m_shownDistance < 0.05f) {
				m_bestPlayer = m_unknown0B;
				m_bestDistance = m_shownDistance;
				m_bestColor = 0xFFFFFFFF;
			}
			else {
				m_bestDistance -= (m_bestDistance - m_shownDistance) * elapsed * 5.0f;
				m_bestColor = 0xFFFF0000;
			}
		}

		if (GOLFDOC()->m_myPlayerIndex == MyIndex()) {
			const float& target = GAME_LONGEST_DRIVE(GOLFDOC()->m_myPlayerIndex);
			if (m_myDistance > target) {
				if (m_myDistance - target < 0.05f) {
					m_myDistance = target;
					m_bestColor = 0xFFFFFFFF;
				}
				else {
					m_myDistance -= (m_myDistance - target) * elapsed * 5.0f;
					m_myColor = 0xFFFF0000;
				}
			}
		}
	}
	else if (GOLFDOC()->GameMode() == 0x80) {
		if (WisEqual(GolfBall().PreData(0)->m_velocity, WVector::ZERO, 1e-5f)) {
			m_unknown08 = false;
			return;
		}

		WVector d;
		d = GolfBall().m_pos - GolfBall().PreData(0)->m_pos;
		float dist = sqrtf(d.x * d.x + d.z * d.z) * 0.3125f;

		if (m_bestDistance <= dist) {
			m_bestPlayer = GOLFDOC()->m_myPlayerIndex;
			m_bestDistance = dist;
			m_bestColor = 0xFF00FF97;
		}
		if (GOLFDOC()->m_myPlayerIndex == MyIndex() && m_myDistance <= dist) {
			m_myDistance = dist;
			m_myColor = 0xFF00FF97;
		}
	}
}

// gui_control.cpp:3309
void CDrivingDistInfo::Display(WFont* pFont) {
	char text[32];
	WRect rect;
	WRect uv;

	float y = WSingleton<COption>::Instance()->gGetBar_Y();

	WOverlay::DrawBox(g_view, WRect(27.0f, y + 4.0f, 62.0f, 1.0f), 0, 0xFFEEEEEE, 0.001f);

	if (GOLFDOC()->GameMode() != 0x80 && m_unknown09) {
		uv.y = 0.0f; uv.x = 0.0f; uv.h = 1.0f; uv.w = 1.0f;
		rect.x = 27.0f; rect.y = y - 28.0f; rect.h = 64.0f; rect.w = 64.0f;

		m_pTopOverlay->Render(g_view, uv, rect, 0, 0xFFFFFFFF, 0.0f, 0);
	}
	else {
		sprintf(text, "%.2fy", m_bestDistance);
		float width = pFont->GetTextWidth(g_view, text);
		float x;
		x = 57.0f - (int)(width * 0.5f);
		pFont->Print(g_view, x, y - 9.0f, text, 0, m_bestColor, 0);

		sprintf(text, "%.2fy", m_myDistance);
		width = pFont->GetTextWidth(g_view, text);
		x = 57.0f - (int)(width * 0.5f);
		pFont->Print(g_view, x, y + 7.0f, text, 0, m_myColor, 0);

		uv.w = 0.5f;
		uv.h = 0.25f;
		rect.x = 41.0f;
		rect.w = 32.0f;
		rect.h = 16.0f;
		if (m_bestPlayer != 0xFF) {
			uv.x = (m_bestPlayer & 1) ? 0.5f : 0.0f;
			uv.y = (m_bestPlayer & 2) ? 0.25f : 0.0f;
			rect.y = y - 25.0f;
			if (m_pPlayerOverlay) {
				m_pPlayerOverlay->SetClippingArea(&rect);
				m_pPlayerOverlay->Render(g_view, uv, rect, 0, PLAYER(m_bestPlayer)->m_color | 0xFF000000, 0.0f, 0);
			}
		}

		uv.x = (MyIndex() & 1) ? 0.5f : 0.0f;
		uv.y = (MyIndex() & 2) ? 0.25f : 0.0f;
		rect.y = y + 17.0f;
		if (m_pPlayerOverlay) {
			m_pPlayerOverlay->SetClippingArea(&rect);
			m_pPlayerOverlay->Render(g_view, uv, rect, 0, PLAYER(MyIndex())->m_color | 0xFF000000, 0.0f, 0);
		}

		pFont->Flush(g_view);
	}
}

// gui_control.cpp:3370 (run-time type and factory registration of FrChatFuncDlg)
const WRTTI FrChatFuncDlg::m_RTTI("FrChatFuncDlg", &FrForm::m_RTTI);

IObject* FrChatFuncDlgMakeInstance(void) {
	return new FrChatFuncDlg;
}

struct __sFrChatFuncDlg {
	__sFrChatFuncDlg(void) {
		ObjectFactory().AddObjectFunctor(FrChatFuncDlgMakeInstance, "FrChatFuncDlg");
	}
};
static __sFrChatFuncDlg s_registerFrChatFuncDlg;

// gui_control.cpp:3372
const sFRESH_MSGMAP* FrChatFuncDlg::GetMessageMap(void) const {
	return &FrChatFuncDlg::_MsgMap;
}

sFRESH_MSGMAP FrChatFuncDlg::_MsgMap = { &FrForm::_MsgMap, &FrChatFuncDlg::_MsgEntries[0] };

sFRESH_ENTRY FrChatFuncDlg::_MsgEntries[] = {
	// gui_control.cpp:3374
	{ "chatview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChatFuncDlg::OnChatViewInit },
	{ "hidechat", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChatFuncDlg::OnHideChatInit },
	{ "showchat", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChatFuncDlg::OnShowChatInit },

	// gui_control.cpp:3378
	{ "hidechat", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrChatFuncDlg::OnBtnHideChatUp },
	{ "showchat", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrChatFuncDlg::OnBtnShowChatUp },
	{ "savechat", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrChatFuncDlg::OnBtnSaveChatUp },
	{ "accuse", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrChatFuncDlg::OnBtnAccuseUp },
	{ "closedlg", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrChatFuncDlg::OnBtnCloseDlgUp },

	// gui_control.cpp:3384
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// gui_control.cpp:3393
FrChatFuncDlg::FrChatFuncDlg(void)
	: m_pUnknown110(NULL)
	, m_pBtnShowChat(NULL)
	, m_pBtnHideChat(NULL)
	, m_pUnknown11C(NULL)
	, m_pUnknown120(NULL)
	, m_pChatView(NULL) {
}

// gui_control.cpp:3397
FrChatFuncDlg::~FrChatFuncDlg(void) {
}

// gui_control.cpp:3401
void FrChatFuncDlg::OnBtnShowChatUp(void) {
	if (m_pBtnHideChat) {
		m_pBtnHideChat->SetVisible(true);
	}
	if (m_pBtnShowChat) {
		m_pBtnShowChat->SetVisible(false);
	}

	GOLFDOC()->ShowChat(true);

	// "Turns the in-game chat window on."
	WSingleton<CChatMsg>::Instance()->AddChatMsg("\xB0\xD4\xC0\xD3\xB3\xBB \xB4\xEB\xC8\xAD\xC3\xA2\xC0\xBB \xC8\xB0\xBC\xBA\xC8\xAD\xC7\xD5\xB4\xCF\xB4\xD9.", 0xFFFF7878, true, false);
}

// gui_control.cpp:3413
void FrChatFuncDlg::OnBtnHideChatUp(void) {
	if (m_pBtnHideChat) {
		m_pBtnHideChat->SetVisible(false);
	}
	if (m_pBtnShowChat) {
		m_pBtnShowChat->SetVisible(true);
	}

	GOLFDOC()->ShowChat(false);

	// "Hides the in-game chat window."
	WSingleton<CChatMsg>::Instance()->AddChatMsg("\xB0\xD4\xC0\xD3\xB3\xBB \xB4\xEB\xC8\xAD\xC3\xA2\xC0\xBB \xBC\xFB\xB1\xE9\xB4\xCF\xB4\xD9.", 0xFFFF7878, true, false);
}

// gui_control.cpp:3425
void FrChatFuncDlg::OnBtnSaveChatUp(void) {
	int index = WSingleton<CSharedDoc>::Instance()->m_chatManager.SaveChatHistory(GOLFDOC()->GetSaveChatNum(), 1);
	GOLFDOC()->SetSaveChatNum(index);
}

// gui_control.cpp:3431
bool FrChatFuncDlg::OnReportDlgResult(int ret, FrForm* pForm) {
	Close(FrOK, true);

	m_pReportDlg = NULL;
	return true;
}

// gui_control.cpp:3439
void FrChatFuncDlg::OnBtnAccuseUp(void) {
	m_pReportDlg = CreateForm<FrReportDlg>(g_pFresh->GetManager(), this, "report", 0);
	if (m_pReportDlg) {
		std::string msg;
		// "1. Earlier contents of the chat window are recorded on the server too."
		msg = "1. \xB4\xEB\xC8\xAD\xC3\xA2\xC0\xC7 \xC1\xF6\xB3\xAD \xB3\xBB\xBF\xEB\xB5\xB5 \xB8\xF0\xB5\xCE \xBC\xAD\xB9\xF6\xBF\xA1 \xB1\xE2\xB7\xCF\xB5\xCB\xB4\xCF\xB4\xD9.\n";
		// "2. Manipulated, false or repeated reports are punished."
		msg += "2. \xC1\xB6\xC0\xDB, \xC7\xE3\xC0\xA7, \xB9\xDD\xBA\xB9\xBD\xC5\xB0\xED\xB4\xC2 \xC3\xB3\xB9\xFA\xC0\xBB \xB9\xDE\xBD\xC0\xB4\xCF\xB4\xD9.\n";
		// "3. Reports are handled within 48 hours."
		msg += "3. \xBD\xC5\xB0\xED\xC7\xCF\xBD\xC5 \xB3\xBB\xBF\xEB\xC0\xBA 48\xBD\xC3\xB0\xA3 \xC0\xCC\xB3\xBB\xBF\xA1 \xC3\xB3\xB8\xAE\xB5\xCB\xB4\xCF\xB4\xD9.";

		m_pReportDlg->SetMessage(msg.c_str(), false);
		m_pReportDlg->Open((FRESH_RESULT_CALLBACK)&FrChatFuncDlg::OnReportDlgResult, 3);
	}
}

// gui_control.cpp:3455
void FrChatFuncDlg::OnBtnCloseDlgUp(void) {
	Close(FrOK, true);
}

// gui_control.cpp:3460
void FrChatFuncDlg::OnHideChatInit(intptr_t param) {
	m_pBtnHideChat = DYNAMIC_CAST(FrButton, param);
	if (m_pBtnHideChat) {


		if (GOLFDOC()->IsShowChat()) {

			m_pBtnHideChat->SetVisible(true);
		}
		else {

			m_pBtnHideChat->SetVisible(false);
		}
	}
}

// gui_control.cpp:3476
void FrChatFuncDlg::OnShowChatInit(intptr_t param) {
	m_pBtnShowChat = DYNAMIC_CAST(FrButton, param);
	if (m_pBtnShowChat) {


		if (GOLFDOC()->IsShowChat()) {

			m_pBtnShowChat->SetVisible(false);
		}
		else {

			m_pBtnShowChat->SetVisible(true);
		}
	}
}

// gui_control.cpp:3492
void FrChatFuncDlg::OnChatViewInit(intptr_t param) {
	m_pChatView = DYNAMIC_CAST(FrEdit, param);
	if (m_pChatView) {
		std::list<sChatLine>::iterator it;
		for (it = CSharedDoc::Instance()->m_chatLineList.begin(); it != CSharedDoc::Instance()->m_chatLineList.end(); ++it) {
			sChatLine& line = *it;
			m_pChatView->AddLine(line.m_text.c_str(), WSingleton<CChatMsg>::Instance()->ConvertToUIColor(line.m_color), false);
		}
	}
}

// gui_control.cpp:3505
void FrChatFuncDlg::AddChatMsg(const char* msg, int color) {
	if (m_pChatView) {
		m_pChatView->AddLine(msg, color, false);
	}
}


// gui_control.cpp:3511
void CScreen::DrawClubInfo(void) {
	if (!m_missionMarkShown[1] || WSingleton<CCapturedBg>::Instance()->IsCaptured())
		return;

	sClubSet* clubSet = GolfClub().FindClubSet(CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId);

	float clipNear = g_view->GetClipNearValue();
	if (!(0.0f < clipNear))
		clipNear = 0.0f;

	float clipFar = g_view->GetClipFarValue(); WRect rect(0.0f, m_remainTimeY - 170.0f, 140.0f, 150.0f);
	WVector2D center;
	center.y = m_remainTimeY - 36.0f;
	WMatrix cam = clubSet->m_cameras[GolfClub().GetType()];
	float fov = g_view->GetFOV();

	g_view->SetFOV(data_9ef91c * (g_view->GetHeight() / g_view->GetWidth() * 85.0f));
	g_view->SetClip(0.1f, 100.0f, false);

	float scale;
	if (WSingleton<COption>::Instance()->vIsWideMode())
		scale = 800.0f / g_view->GetWidth();
	else
		scale = (600.0f / g_view->GetHeight() + 800.0f / g_view->GetWidth()) * 0.5f;

	cam.za *= scale;
	float offset = (m_clubAngle * 0.15915494f - 1.0f) * 50.0f;
	center.x = 24.0f + offset;
	center.y -= offset;

	g_view->SetCamera(cam);
	g_view->SetClippingArea(rect);
	g_view->SetScreenCenter(center.x, center.y);
	g_view->UpdateCamera();

	WPuppet* puppet = clubSet->m_puppet[1][GolfClub().GetType()];
	if (puppet) {
		puppet->ApplyBones(NULL, RotMat((float)sin(m_clubSwing) * 0.08726646f - m_clubAngle, 0));
		puppet->SetAlpha((unsigned char)Between(0.0f, m_clubAngle * 40.584511f, 255.0f));
		puppet->Transform(NULL, 1.0f, 0);
		puppet->UpdateLightSource(&clubSet->m_lights[GolfClub().GetType()], false, NULL);
		puppet->Render(g_view, true, 1.0f, false, true, false);
	}

	g_view->Flush(1);

	g_view->SetCamera(g_camera);
	g_view->SetClip(clipNear, clipFar, false);
	g_view->SetFOV(fov);
	g_view->ResetClippingArea();
	g_view->SetScreenCenter(g_view->GetWidth() * 0.5f, g_view->GetHeight() * 0.5f);
	g_view->UpdateCamera();
}

// gui_control.cpp:3598
void CScreen::DrawCammodeInfo(void) {
	if (!WSingleton<CMouseCursor>::Instance()->IsCamMode())
		return;

	char text[16];
	char state[8];

	if (g_input->GetButton(LEFT_BUTTON))
		strcpy(state, "CLICK");
	else
		strcpy(state, "OVER");

	switch (WSingleton<CMouseCursor>::Instance()->GetArea()) {
	case 0x17:
		strcpy(text, MakeStr("LEFT_%s", state));
		break;
	case 0x18:
		strcpy(text, MakeStr("RIGHT_%s", state));
		break;
	case 0x19:
		strcpy(text, MakeStr("UP_%s", state));
		break;
	case 0x1A:
		strcpy(text, MakeStr("DOWN_%s", state));
		break;
	case 0x1B:
		if (GOLFDOC()->GameMode() & 0x8000)
			strcpy(text, "INACTIVE");
		else
			strcpy(text, MakeStr("FORWARD_%s", state));
		break;
	case 0x1C:
		if (GOLFDOC()->GameMode() & 0x8000)
			strcpy(text, "INACTIVE");
		else
			strcpy(text, MakeStr("BACKWARD_%s", state));
		break;
	default:
		if (GOLFDOC()->GameMode() == 0x8000)
			strcpy(text, "INACTIVE");
		else
			strcpy(text, "NORMAL");
		break;
	}

	if (m_pCammodeUI) {
		m_pCammodeUI->SetColor((WSingleton<CMouseCursor>::Instance()->IsVisible() || m_cammodeAlpha < 1.0f) ? 0xFFFFFFFF : 0x80FFFFFF);
		m_pCammodeUI->Render(text, g_view->GetWidth() * 0.65f, g_view->GetHeight() - 150.0f, m_cammodeAlpha, 0x22);
	}
}

// gui_control.cpp:3653
void CScreen::DrawWindInfo(void) {
	bool visible;

	SendMsg(NULL, "ShotReview", -1, (intptr_t)"GET_VISIBLE_STATE", (intptr_t)&visible, 0, 0);

	if (GolfClub().GetType() == 3) {
		if (!visible)
			return;
	}

	if (GOLFDOC()->m_gameState == 15 || (GOLFDOC()->m_gameState == 11 && m_unknown3E2)) {
		DrawOverlay2("\xb9\xd9\xb6\xf7\xc5\xab\xc3\xa2", WRect(g_view->GetWidth() - 114.0f, m_remainTimeY - 60.0f, 128.0f, 128.0f), 0xFFFFFFFF, 0);
		Wind().Display(m_pWindPuppet, g_view->GetWidth() - 50.0f, m_remainTimeY + 5.0f);
		DrawOverlay2("\xb9\xd9\xb6\xf7\xc0\xdb\xc0\xba\xc3\xa2", WRect(g_view->GetWidth() - 52.0f, m_remainTimeY + 1.0f, 64.0f, 64.0f), 0xFFFFFFFF, 0);

		if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty())) {
			S5::CLASSICSRV::eWINDPOWERSTEP step = S5::CLASSICSRV::GetWindPowerStep(Wind().GetGlobalIntensity());

			WRect rect(g_view->GetWidth() - 34.0f, m_remainTimeY + 19.0f, 32.0f, 32.0f);

			switch (step) {
			case S5::CLASSICSRV::WINDPOWERSTEP_WEAK:
				DrawOverlay("\xb9\xd9\xb6\xf7\xbc\xbc\xb1\xe2\xbe\xe0", rect, 0xFF, 0);
				break;
			case S5::CLASSICSRV::WINDPOWERSTEP_MEDIUM:
				DrawOverlay("\xb9\xd9\xb6\xf7\xbc\xbc\xb1\xe2\xc1\xdf", rect, 0xFF, 0);
				break;
			case S5::CLASSICSRV::WINDPOWERSTEP_STRONG:
				DrawOverlay("\xb9\xd9\xb6\xf7\xbc\xbc\xb1\xe2\xb0\xad", rect, 0xFF, 0);
				break;
			}
		}
		else {
			if (m_pWindInfo)
				m_pWindInfo->Display();
		}
	}
}

// gui_control.cpp:3728
void CScreen::DrawApproachInfo(void) {
	if (CSharedDoc::Instance()->m_roomType == 10 && CSharedDoc::Instance()->m_gameStartTime != 0) {
		// "mission box frame 1" / "mission box frame 2"
		DrawOverlay("\xB9\xCC\xBC\xC7\xB9\xDA\xBD\xBA\xC7\xC1\xB7\xB9\xC0\xD3" "1", WRect(g_view->GetWidth() - 270.0f, 46.0f, 256.0f, 32.0f), 255, 0);
		DrawOverlay("\xB9\xCC\xBC\xC7\xB9\xDA\xBD\xBA\xC7\xC1\xB7\xB9\xC0\xD3" "2", WRect(g_view->GetWidth() - 270.0f, 70.0f, 256.0f, 256.0f), 255, 0);
		if (GOLFDOC()->GetApproachMissionType() == 0)
			// "no mission"
			m_pMissionUI->Render("\xB9\xCC\xBC\xC7\xBE\xF8\xC0\xBD", g_view->GetWidth() - 182.0f, 95.0f, 1.0f, 0);
		else
			// "mission"
			m_pMissionUI->Render("\xB9\xCC\xBC\xC7\xC0\xD6\xC0\xBD", g_view->GetWidth() - 220.0f, 75.0f, 1.0f, 0);

		if (GOLFDOC()->GetApproachMissionType() != 0) {
			char prizeText[64] = "\0";
			switch (GOLFDOC()->GetApproachWinnerType()) {
			case 1:
				// "* on mission success, %d more for everyone."
				sprintf(prizeText, "\xA1\xD8 \xB9\xCC\xBC\xC7\xBC\xBA\xB0\xF8\xBD\xC3 \xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4 %d\xB0\xB3 \xB4\xF5.", GOLFDOC()->GetApproachPrizeType());
				break;
			case 2:
				// "* %d more for the successful users."
				sprintf(prizeText, "\xA1\xD8 \xBC\xBA\xB0\xF8\xC7\xD1 \xC0\xAF\xC0\xFA\xBF\xA1\xB0\xD4 %d\xB0\xB3 \xB4\xF5.", GOLFDOC()->GetApproachPrizeType());
				break;
			case 3:
				sprintf(prizeText, "\xA1\xD8 \xB9\xCC\xBC\xC7\xBC\xBA\xB0\xF8\xBD\xC3 \xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4 %d\xB0\xB3 \xB4\xF5.", GOLFDOC()->GetApproachPrizeType());
				break;
			}

			float x = g_view->GetWidth() - 205.0f;

			std::string desc(GOLFDOC()->GetApproachMissionDesc());
			std::string lines[4] = { "\0" };
			lines[3] = prizeText;

			if (desc.length() >= 33) {
				int pos = 0;
				int len = 33;
				for (int i = 0; i < 3; i++) {
					if (desc.length() < (unsigned int)(pos + 33)) {
						lines[i] = desc.substr(pos, desc.length());
						break;
					}

					for (; len > 0; len--) {
						if (desc[pos + len] == ' ')
							break;
					}

					lines[i] = desc.substr(pos, len);
					pos += len + 1;
					len = 33;
				}
			}
			else {
				lines[0] = desc;
			}

			for (int i = 0; i < 4; i++)
				m_pFont->Print(g_view, x, i * 17.0f + 138.0f, lines[i].c_str(), 0, 0xFFFFFFFF, NULL);
		}

		m_pFont->Flush(g_view);

		// "treasure box"
		DrawOverlay("\xC6\xAE\xB7\xB9\xC1\xAE\xB9\xDA\xBD\xBA", WRect(g_view->GetWidth() - 60.0f, 47.0f, 32.0f, 32.0f), 255, 0);

		unsigned long prize = GOLFDOC()->GetApproachTotalPrize();

		char prizeNum[4] = "";
		itoa(prize, prizeNum, 10);

		float offset = 65.0f;
		if (prize > 9)
			offset = 75.0f;

		m_pTitleFont->Print(g_view, g_view->GetWidth() - offset, 49.0f, prizeNum, 0, 0xFFFFFFFF, NULL);

		unsigned long remain;
		if (CSharedDoc::Instance()->m_gameStartTime == 0)
			remain = CSharedDoc::Instance()->m_gameTimeLimit;
		else if (CSharedDoc::Instance()->m_gameTimeLimit + CSharedDoc::Instance()->m_gameStartTime < g_CurrentTime)
			remain = 0;
		else
			remain = CSharedDoc::Instance()->m_gameTimeLimit + CSharedDoc::Instance()->m_gameStartTime - g_CurrentTime;

		char timeText[16];
		sprintf(timeText, "%d.%03d", remain / 1000, remain % 1000);

		unsigned long color;
		if (remain > 20000)
			color = 0xFF000000;
		else
			color = remain > 10000 ? 0xFFFF8000 : 0xFFFF0000;

		// "game time limit"
		DrawOverlay2("\xB0\xD4\xC0\xD3\xC1\xA6\xC7\xD1\xBD\xC3\xB0\xA3_base", WRect(17.0f, 150.0f, 128.0f, 64.0f), color, 0);
		DrawOverlay("\xB0\xD4\xC0\xD3\xC1\xA6\xC7\xD1\xBD\xC3\xB0\xA3_border", WRect(17.0f, 150.0f, 128.0f, 64.0f), 255, 0);

		float width = m_pFont->GetTextWidth(g_view, timeText);
		m_pFont->Print(g_view, 75.0f - width, 161.0f, timeText, 0, 0xFFFFFFFF, NULL);

		if (GOLFDOC()->GameMode() & 0x8C0)
			// "approach info"
			DrawOverlay("\xBE\xEE\xC7\xC1\xB7\xCE\xC4\xA1\xC1\xA4\xBA\xB8", WRect(10.0f, 200.0f, 64.0f, 64.0f), 255, 0);

		m_pFont->Flush(g_view);
	}
}

// The hit bar's left edge (140), an unnamed float of .rdata, and the bar's
// width, an unnamed float of .bss (DrawHitBarInfo).
extern "C" float data_9ef938;
extern "C" float data_af34dc;

// gui_control.cpp:3906
void CScreen::DrawHitBarInfo(void) {
	bool bigBar = false;
	float y = WSingleton<COption>::Instance()->gGetBar_Y();
	float h = 9.0f;

	if (GolfClub().GetType() == 3) {
		y -= 10.0f; bigBar = true;
		h = 19.0f;
	}

	unsigned long color = (GOLFDOC()->GetUIAlpha() << 24) | 0xFFFFFF;
	DrawOverlay2("\xc3\xbc\xb7\xc2\xb0\xd4\xc0\xcc\xc1\xf6", WRect(140.0f, y, data_af34dc, h), m_hitBarColor & color, 0);

	if (!bigBar)
		DrawOverlay("\xbf\xde\xc2\xca\xb0\xd4\xc0\xcc\xc1\xf6", WRect(105.0f, y, 35.0f, h), GOLFDOC()->GetUIAlpha(), 0);

	if (GolfBall().m_groundType == 2 && m_hitBarFrom < m_hitBarCount) {

		float unit = data_af34dc / (float)m_hitBarCount;
		DrawOverlay2("\xc8\xa6\xc4\xc5\xb1\xb8\xb0\xa3", WRect((float)m_hitBarFrom * unit + data_9ef938, y, unit, h), m_hitBarRangeColor & color, 0);
	}

	if (bigBar && GolfBall().m_groundType == 2) {

		float step = 360.0f / (float)m_hitBarCount;

		for (int i = 1; i < m_hitBarCount + 1; i++)
			WOverlay::DrawBox(g_view, WRect(((float)i - 0.5f) * step + data_9ef938 - 1.0f, m_hitMarks[i].y, 3.0f, 3.0f), 0, m_hitMarks[i].color & color, 0.001f);
	}

	sOverlay* overlay;
	if (GOLFDOC()->m_clockHold < 2) {
		WRect rect(140.0f, y, GOLFDOC()->GetBar().m_prevAccuracy - data_9ef938, h); overlay = m_overlays.Find("\xc6\xc4\xbf\xf6\xb0\xd4\xc0\xcc\xc1\xf6"); if (overlay && overlay->m_pOverlay) overlay->m_pOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), rect, 0, 0xFFFFFFFF, 0.0f, 0);
	} else {
		WRect rect(140.0f, y, GOLFDOC()->GetBar().m_power - data_9ef938, h); overlay = m_overlays.Find("\xc6\xc4\xbf\xf6\xb0\xd4\xc0\xcc\xc1\xf6"); if (overlay && overlay->m_pOverlay) overlay->m_pOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), rect, 0, 0xFFFFFFFF, 0.0f, 0);
	}

	WRect src; src.x = 90.0f; src.w = 479.0f; if (bigBar) {
		if (GolfBall().m_groundType == 2)
			overlay = m_overlays.Find(MakeStr("\xb4\xab\xb1\xdd_\xc6\xdb\xc6\xc3%d", m_hitBarCount));
		else
			overlay = m_overlays.Find("\xb4\xab\xb1\xdd_\xc6\xdb\xc6\xc3" "10");

		src.y = m_remainTimeY - 17.0f;
		src.h = 64.0f;
	} else {
		DrawOverlay("\xc5\xb8\xc0\xcc\xb9\xd6\xbf\xb5\xbf\xaa", WRect(data_9ef938 - GolfClub().GetNarrowAccuracy(), y, GolfClub().GetNarrowAccuracy() * 2.0f, h), GOLFDOC()->GetUIAlpha(), 0);

		overlay = m_overlays.Find("\xb4\xab\xb1\xdd_\xb5\xe5\xb6\xf3\xc0\xcc\xba\xea");

		src.y = m_remainTimeY - 6.0f;
		src.h = 32.0f;
	}

	if (overlay && overlay->m_pOverlay)
		overlay->m_pOverlay->Render(g_view, WRect(0.064453125f, 0.0f, 0.935546875f, 1.0f), src, 0, color, 0.0f, 0);

	float pangYaArea = WSingleton<CPowerGauge>::Instance()->GetPangYaArea();

	if (IsLocalContent((localContentType_t)0x6E)) {
		float offset = 0.0f;
		float width = pangYaArea * 2.0f;
		if ((int)width % 2 == 1)
			offset = 0.5f;

		DrawOverlay("\xbd\xc3\xc0\xdb\xc1\xa1", WRect(GOLFDOC()->GetBar().m_startPosition - pangYaArea - offset, y, width, h), GOLFDOC()->GetUIAlpha(), 0);
	} else
		DrawOverlay("\xbd\xc3\xc0\xdb\xc1\xa1", WRect(GOLFDOC()->GetBar().m_startPosition - pangYaArea, y, pangYaArea * 2.0f, h), GOLFDOC()->GetUIAlpha(), 0);

	if (bigBar) {

		if (IsLocalContent((localContentType_t)0x4F) && WSingleton<CSharedDoc>::Instance()->m_playingMode == 2) {

			if (WSingleton<CSharedDoc>::Instance()->m_replayState != 3)
				GOLFDOC()->GetBar().m_prevAccuracy = 140.0f;
			else
				GOLFDOC()->GetBar().m_prevAccuracy = WSingleton<CSharedDoc>::Instance()->m_replayPrevAccuracy;
		}

		DrawOverlay("\xc0\xd4\xc3\xbc\xb9\xd9", WRect(GOLFDOC()->GetBar().m_prevAccuracy - 4.0f, m_remainTimeY - 12.0f, 8.0f, 24.0f), GOLFDOC()->GetUIAlpha(), 0);
	} else
		DrawOverlay("\xc0\xd4\xc3\xbc\xb9\xd9", WRect(GOLFDOC()->GetBar().m_prevAccuracy - 4.0f, m_remainTimeY - 4.0f, 8.0f, 16.0f), GOLFDOC()->GetUIAlpha(), 0);

	if (GOLFDOC()->m_clockHold == 0)
		overlay = m_overlays.Find("\xb0\xd4\xc0\xcc\xc1\xf6\xc1\xa4\xc1\xf6");
	else if (GOLFDOC()->m_clockHold == 1)
		overlay = m_overlays.Find("\xb0\xd4\xc0\xcc\xc1\xf6\xc5\xac\xb8\xaf");
	else
		overlay = m_overlays.Find("\xb0\xd4\xc0\xcc\xc1\xf6\xc0\xcc\xb5\xbf");

	if (overlay && overlay->m_pOverlay)
		overlay->m_pOverlay->Render(g_view, WRect(0.125f, 0.0f, 0.875f, 1.0f), WRect(92.0f, m_remainTimeY + 13.0f, 56.0f, 32.0f), 0, color, 0.0f, 0);

	if (m_pPowerInfo) {

		WVector spot;
		SendMsg(this, "Spot", 0xF4, (intptr_t)&spot, 0, 0, 0);

		m_pPowerInfo->SetColor(color);
		m_pPowerInfo->SetPos(spot);
		m_pPowerInfo->Display();
	}

	if (m_pCalipers) {

		bool usable = S5::IsUsableCalipers();

		if (IsMyTurn(false) && !usable) {
			bool impact = true;
			// One DrawShotPowerRange/DrawLimitLine site for both branches:
			// the original's first branch jumps into the second one's.
			if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId != 0x18000009) {
				if (GOLFDOC()->m_clockHold == 2 || GOLFDOC()->m_clockHold == 3) {
					impact = false;
					goto drawRange;
				}
			} else if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000009) {
				m_pCalipers->Display();
				if (GOLFDOC()->m_clockHold == 2 || GOLFDOC()->m_clockHold == 3) {
drawRange:
					float pos = 0.0f;
					m_pCalipers->DrawShotPowerRange(pos, false);
					m_pCalipers->DrawLimitLine(pos, impact);
				}
			}
		} else if (usable == true) {
			if (WSingleton<CSharedDoc>::Instance()->m_gameMode != 1 && WSingleton<CSharedDoc>::Instance()->m_gameMode != 2)
				m_pCalipers->Display();

			if (!m_pCalipers->IsVisiblePowerBar() && (GOLFDOC()->m_clockHold == 2 || GOLFDOC()->m_clockHold == 3)) {
				if (IsMyTurn(false)) {
					float pos = 0.0f;
					m_pCalipers->DrawShotPowerRange(pos, false);
					m_pCalipers->DrawLimitLine(pos, false);
				}
			}
		}
	}

	if (GolfClub().GetType() != 3 && !GalleryMode()) {
		if (GOLFDOC()->m_clockHold == 1 && data_af34dc * 0.5f + data_9ef938 < GOLFDOC()->GetBar().m_prevAccuracy)
			DrawOverlay("\xc5\xac\xb8\xaf\xc7\xa5\xbd\xc3", WRect(GOLFDOC()->GetBar().m_prevAccuracy - 23.0f, m_remainTimeY - 51.0f, 64.0f, 64.0f), 0xFF, 0);
		else if (GOLFDOC()->m_clockHold == 2)
			DrawOverlay("\xc5\xac\xb8\xaf\xc7\xa5\xbd\xc3", WRect(117.0f, m_remainTimeY - 71.0f + Abs(m_hitBarTime), 64.0f, 64.0f), 0xFF, 0);
	}
}

// gui_control.cpp:4143
void CScreen::DrawRemainTime(void) {
	// "remaining-time circle"
	sOverlay* overlay = m_overlays.Find("\xB3\xB2\xC0\xBA\xBD\xC3\xB0\xA3\xBF\xF8");

	if (WSingleton<CCapturedBg>::Instance()->IsCaptured()) {
		if (overlay && overlay->m_pOverlay)
			overlay->m_pOverlay->ArcClipRender(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(11.5f, m_remainTimeY + 4.0f - 46.5f, 93.0f, 93.0f), m_remainTimeArc, 5.0614548f, 0, m_remainTimeColor);
	}
	else {
		m_remainTimeArcShown = m_remainTimeArc;
		if (overlay && overlay->m_pOverlay)
			overlay->m_pOverlay->ArcClipRender(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(11.5f, m_remainTimeY + 4.0f - 46.5f, 93.0f, 93.0f), m_remainTimeArc, 5.0614548f, 0, m_remainTimeColor);
	}
}

// gui_control.cpp:4178
void CScreen::DrawClubBallUI(void) {
	if (IsMyTurn(false)) {
		// "club bar"
		sOverlay* overlay = m_overlays.Find("\xC5\xAC\xB7\xB4\xB9\xD9");

		WRect clip;
		clip.x = 58.0f; clip.y = m_remainTimeY - 6.0f;
		clip.w = GolfClub().GetBarWidth();
		clip.h = 32.0f;

		unsigned long color = (GolfClub().GetBarAlpha() << 24) | 0xFFFFFF;

		if (overlay && overlay->m_pOverlay) {
			overlay->m_pOverlay->SetClippingArea(&clip);
			overlay->m_pOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(clip.x - 456.0f + clip.w, clip.y, 512.0f, 32.0f), 0, color, 0.0f, 0);
		}

		if (GolfClub().BarIs(0) || GolfClub().BarIs(3)) {
			if (m_pSimpleUI21C) {
				m_pSimpleUI21C->SetColor(color);
				m_pSimpleUI21C->Render(GolfClub().GetName(), 85.0f, m_remainTimeY + 5.0f, 1.0f, 0x40);
			}

			if (m_pSimpleUI220) {
				m_pSimpleUI220->SetColor(color);
				// "current club"
				m_pSimpleUI220->Render("\xC7\xF6\xC0\xE7\xC5\xAC\xB7\xB4", 494.0f - Min<unsigned char>(GolfClub().GetIndex(), 13) * 26.0f, m_remainTimeY + 5.0f, 1.0f, 0x22);
			}
		}
	}

	// "info display circle"
	DrawOverlay("\xC1\xA4\xBA\xB8\xC7\xA5\xBD\xC3\xBF\xF8", WRect(5.0f, m_remainTimeY - 52.0f, 128.0f, 128.0f), 255, 0);

	if (m_pBase)
		m_pBase->Display();

	if (GolfBall().m_preDataIndex < 0) {
		unsigned long color;
		switch (GolfBall().m_groundType) {
		case 0:
		case 1:
		case 2:
		case 6:
		case 7:
			color = 0;
			break;
		case 3:
			color = 0x80E5CA77;
			break;
		case 5:
			color = 0x80FFFFFF;
			break;
		default:
			color = 0x8078E577;
			break;
		}

		if (color) {
			// "ball state"
			sOverlay* overlay = m_overlays.Find("\xBA\xBC\xBB\xF3\xC5\xC2");
			if (overlay && overlay->m_pOverlay)
				overlay->m_pOverlay->Render(g_view, WRect(0.0f, m_ballStateCut, 1.0f, 1.0f - m_ballStateCut), WRect(26.0f, m_remainTimeY - 28.0f + m_ballStateCut * 64.0f, 64.0f, 64.0f * (1.0f - m_ballStateCut)), 0, color, 0.0f, 0);
		}
	}

	if (m_pClubTypeInfo)
		m_pClubTypeInfo->Display();
}

// gui_control.cpp:4279
void CScreen::DrawCommandArrow(void) {
	if (GOLFDOC()->GameMode() & 0x60) {
		float y = 200.0f;
		std::string name;

		std::list<unsigned long>::iterator it = GOLFDOC()->m_commandArrows.begin();
		while (it != GOLFDOC()->m_commandArrows.end()) {
			if (*it == 0x10) {
				++it;
				continue;
			}

			switch (*it) {
			case 1: name = "UP"; break;
			case 2: name = "DOWN"; break;
			case 4: name = "LEFT"; break;
			case 8: name = "RIGHT"; break;
			case 0x40: name = "INVALID"; break;
			case 0x20: name = "INVALID"; y -= 22.0f; break;
			}

			++it;
			if (it != GOLFDOC()->m_commandArrows.end() && *it == 0x10)
				name += "_S";

			if (m_pCommandArrowUI)
				m_pCommandArrowUI->Render(name.c_str(), y, m_remainTimeY - 30.0f, 1.0f, 0);
			y += 22.0f;
		}
	}
}

// gui_control.cpp:4315
void CScreen::DrawTutorialUI(void) {
	WRect rect;

	if (GOLFDOC()->m_gameState == 15)
		return;

	if (!WSingleton<CCapturedBg>::Instance()->IsCaptured()) {
		if (GOLFDOC()->m_gameState == 0 && m_tutoMsgShown)
			// "aim area"
			DrawOverlay("\xC1\xB6\xC1\xD8\xBF\xB5\xBF\xAA", WRect(g_view->GetWidth() * 0.5f - 64.0f, g_view->GetHeight() * 0.5f - 64.0f, 128.0f, 128.0f), 0xFF, 0);

		if ((WSingleton<CSharedDoc>::Instance()->m_roomType == 0xB || WSingleton<CSharedDoc>::Instance()->m_roomType == 0xC) && m_pTutoEscMsgUI) {
			m_pTutoEscMsgUI->Render("MSG0", 150.0f, m_remainTimeY + 33.0f, 1.0f, 0);
			m_pTutoEscMsgUI->Render("MSG1", 406.0f, m_remainTimeY + 33.0f, 1.0f, 0);
		}

		if (GOLFDOC()->m_gameState == 5) {
			// "button window"
			if (CItemWindow::IsClosed())
				DrawOverlay("\xB9\xF6\xC6\xB0\xC3\xA2_normal", rect, 0xFF, 0);
			else
				DrawOverlay("\xB9\xF6\xC6\xB0\xC3\xA2_item", rect, 0xFF, 0);
		}

		if (m_tutoMsgShown) {
			// "quest mission info"
			if (WSingleton<CSharedDoc>::Instance()->m_roomType == 0xC)
				DrawOverlay(MakeStr("\xC4\xF9\xBD\xBA\xC6\xAE\xB9\xCC\xBC\xC7\xC1\xA4\xBA\xB8%d", GOLFDOC()->m_gameState), WRect(g_view->GetWidth() - 256.0f, 26.0f, 256.0f, 256.0f), 0xFF, 0);
			else
				DrawOverlay("\xC4\xF9\xBD\xBA\xC6\xAE\xB9\xCC\xBC\xC7\xC1\xA4\xBA\xB8" "0", WRect(g_view->GetWidth() - 256.0f, 26.0f, 256.0f, 128.0f), 0xFF, 0);
		}

		if (m_pTutoTitleUI) {
			m_pTutoTitleUI->SetColor((m_tutoTitleAlpha << 24) | 0xFFFFFF);
			// "title_%d"
			m_pTutoTitleUI->Render(MakeStr("\xC5\xB8\xC0\xCC\xC6\xB2_%d", m_tutorialNo + 1), m_tutoTitleX + 20.0f, m_tutoTitleY, 1.0f, 0);
		}
	}

	int index;
	switch (GOLFDOC()->m_gameState) {
	case 1: index = 0; break;
	case 2: index = 1; break;
	case 3: index = 2; break;
	case 4: index = 3; break;
	case 5: index = 4; break;
	case 8: index = 5; break;
	case 9: index = 6; break;
	case 10: index = 7; break;
	case 11: index = 8; break;
	case 12: index = 9; break;
	case 13: index = 10; break;
	case 14: index = 11; break;
	default: return;
	}

	if (m_missionTitleState == 3)
		return;

	if (index == 6)
		m_pTutorialUIs[6]->Render("MissionTitle", m_missionTitleX, m_missionTitleY + 20.0f, m_missionTitleAlpha, 2);
	else
		m_pTutorialUIs[index]->Render("MissionTitle", m_missionTitleX, m_missionTitleY, m_missionTitleAlpha, 2);
}
