// Original: d:\build\custom\temp\code\client\projectg\tutorial.cpp
#include "NetworkSystem.h"
#include "tutorial.h"
#include "ObjectFactory.h"
#include "actor.h"
#include "fresh.h"
#include "QuizDlg.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "polysoup.h"
#include "InputManager.h"
#include "mousecursor.h"
#include "TaskManager.h"
#include "ProjectG.h"
#include "commonutil.h"
#include "wind.h"
#include "golfball.h"
#include "CapturedBg.h"
#include "club.h"
#include "client/Wangreal/include/wutil.h"
#include "client/Wangreal/include/wmath.h"
#include <math.h>
#include <stdio.h>
#include "frarea.h"
#include "frbutton.h"
#include "client/Wangreal/include/wview.h"
#include "client/Wangreal/include/woverlay.h"
#include "client/Wangreal/include/wresrcmng.h"
#include "client/Wangreal/include/westpak.h"
#include "quadtree.h"
#include "ChatMsg.h"

#ifdef JP_PAKS
// JP 983: the tutorial container's mission send (0x9079e0,
// src/jp/JpTutorialWidget.cpp), declared here (L38).
void JpTutorialContainer_SendMissionResult(int index, unsigned long missionBit, int played, int complete);
#endif

// tutorial.cpp:29 (run-time type and factory registration of CTutorial)
const WRTTI CTutorial::m_RTTI("CTutorial", &CTaskMain::m_RTTI);

IObject *CTutorialMakeInstance(void) {
	return new CTutorial;
}

struct __impCTutorial {
	__impCTutorial(void) {
		ObjectFactory().AddObjectFunctor(CTutorialMakeInstance, "CTutorial");
		ActorFactory().AddActorClass("CTutorial");
	}
};
static __impCTutorial s_impCTutorial;

// tutorial.cpp:133
AnimatedOverlay::AnimatedOverlay(void) {
	m_dst = WRect(0, 0, 1.0f, 1.0f);
	m_src = WRect(WPoint(0, 0), 1.0f, 1.0f);
	m_frameTime = 0;

	m_elapsed = 0;
	m_frame = 0;
	m_frameCount = 0;
	m_active = false;
	m_visible = false;
	m_loop = false;

	m_fadeDir = 0;
	m_alpha = 255.0f;

	m_overlay = 0;

	m_columns = 0;
}

// tutorial.cpp:156
AnimatedOverlay::~AnimatedOverlay(void) {
	for (std::vector<WOverlay *>::iterator it = m_overlays.begin(); it != m_overlays.end(); ++it) {
		if (g_resrcmng && *it) { g_resrcmng->Release(*it); *it = 0; }
	}
}

// tutorial.cpp:176
void AnimatedOverlay::Process(float dt) {
	if (!m_visible && !m_active) {
		return;
	}

	if (m_frameCount < 1) {
		return;
	}

	if (m_fadeDir == 0 && m_frameTime > 0.0f) {
		m_elapsed += dt;
		if (m_elapsed > m_frameTime) {
			if (m_frameCount == 1) {
				m_visible = !m_visible;
			}
			m_elapsed -= m_frameTime;
			m_frame++;

			if (!m_active || !m_loop) {
				if (m_frame == m_frameCount) {
					m_frame = 0;
					m_elapsed = 0;
					m_frameTime = 0;
					m_visible = m_active;
				}
			} else {
				m_frame %= m_frameCount;
			}
		}
	} else if (m_fadeDir != 0 || m_frameTime == 0.0f) {
		if (m_fadeDir != 0) {
			m_elapsed += dt * 3.0f;
			float alpha = (1 - m_fadeDir) * 128.0f; alpha += m_fadeDir * m_elapsed * 255.0f;
			m_alpha = alpha;
			if (alpha < 0.0f || alpha > 255.0f) {
				m_alpha = Between(0.0f, alpha, 255.0f);
				m_fadeDir = 0; m_elapsed = 0; m_visible = m_active;
			}
		}
	}

	m_dst.x = (m_frame % m_columns) * m_dst.w; m_dst.y = (m_frame / m_columns) * m_dst.h;
}

// tutorial.cpp:244
void AnimatedOverlay::Activate(bool active, bool fade, bool loop) {
	m_active = active; m_loop = loop;

	if (active) {
		m_frame = 0;
		m_elapsed = 0;
		m_alpha = 255.0f;
		m_visible = active;
	}

	if (fade) {
		m_fadeDir = active ? 1 : -1;
	}
}

// tutorial.cpp:270
void AnimatedOverlay::Render(void) {
	if (!m_visible) {
		return;
	}

	if (m_overlay) {
		m_overlay->Render(g_view, m_dst, m_src, 0, ((int)m_alpha << 24) | 0xFFFFFF, 0, 0);
	}
}

// tutorial.cpp:286
void AnimatedOverlay::SetSourceOverlay(const char *name, int columns, int rows, int mode, float w, float h) {
	if (m_overlays.size()) {
		std::vector<WOverlay *>::iterator it = m_overlays.begin();
		if (g_resrcmng && *it) { g_resrcmng->Release(*it); *it = 0; }
		m_overlays.erase(it);
	}

	WOverlay *overlay = g_resrcmng->GetOverlay(name, 0);

	if (overlay) {
		m_overlays.push_back(overlay);

		m_columns = columns;
		m_overlay = overlay;
		m_frameCount = columns * rows;

		if (mode == 1) {
			m_dst.w = w / overlay->GetWidth();
			m_dst.h = h / overlay->GetHeight();
		} else {
			m_dst.w = 1.0f / columns;
			m_dst.h = 1.0f / rows;
		}
	}
}

// tutorial.cpp:332
void AnimatedOverlay::GetNextOverlay(void) {
	int col = m_frame % m_columns, row = m_frame / m_columns;

	m_dst.x = col * m_dst.w;
	m_dst.y = row * m_dst.h;
}

// tutorial.cpp:419
CTutorial::CTutorial(void) {
	m_tipForm = 0;
	m_yesNoForm = 0;
	m_reviewForm = 0;

	m_tipCaddieForm = 0;
	m_tipCaddieForm2 = 0;
	m_tutorialInfo = 0;

	m_missionForm = 0;
	m_missionOverlay = 0;

	m_quizForm = 0;
	m_quizIndex = 0;

	m_keepMouseBlocked = false;
	m_fakeKeyDown = false;

	memset(m_missionItems, 0, sizeof(m_missionItems));

	m_priority = 4;
	AddSkipList(0x10);

	m_inputBlocked = false;
}

// tutorial.cpp:449
CTutorial::~CTutorial(void) {
	if (m_inputBlocked) {
		FreeInput();
	}
}

// tutorial.cpp:457
void CTutorial::OnInit(void) {
	if (GOLFDOC()->m_gameState == 15) {
		return;
	}

	for (int i = 0; i < 0xF; i++) {
		m_missionCleared[i] = false;
	}

	Wind().CheckSpecialWind();

	m_tutorialIndex = -1;
	m_singleMission = false;
	m_reserved2A0 = 0;
	m_yesNoForm = 0;
	m_failCount = 0;
	m_targetHits = 0;
	m_macroHold = false;
	m_macroTimer = -1;
	m_pulseScale = 1.5f;
	m_reserved2D0 = 0;

	m_reserved2D5 = false;
	m_reserved2D8 = 0;

	m_practice = false;

	m_tipRect.x = (g_view->GetWidth() - 370.0f) * 0.5f;
	m_tipRect.y = 50.0f;
	m_tipRect.w = 370.0f;
	m_tipRect.h = 125.0f;

	IsPractice();
}

// tutorial.cpp:491
void CTutorial::OnLoad(void) {
	if (GOLFDOC()->m_gameState == 15) {
		return;
	}

	cFile *file = g_resrcmng->GetCFile("tutorial_text.txt", -1);
	if (file) {
		int size = file->Length();
		char *buffer = new char[size + 1];
		memset(buffer, 0, size + 1);
		file->Read(buffer, size);

		char *cursor = buffer;
		if (cursor) {
			do {
				int length = (int)strlen(cursor);
				if (!length) {
					break;
				}

				int key;
				char line[0x400];
				sscanf(cursor, "%d", &key);
				cursor = GetLine(cursor, line);
				m_texts[key] = line;
			} while (cursor);
		}

		CloseCFile(file);
		delete[] buffer;
	}

	CGolfDoc *doc = GOLFDOC();
	doc->m_target.m_cameraTarget = g_resrcmng->GetPuppet("camera_target.pet", false, true, false);

	OnImageLoad();
}

// tutorial.cpp:532
void CTutorial::OnProcess(float dt) {
	if (GOLFDOC()->m_gameState == 15) {
		return;
	}

	if (m_fakeKey[0]) {
		g_input->SetFakeInput(m_fakeKey, m_fakeKeyDown);
		m_fakeKeyDown = false;
	}

	if (m_specialWait >= 0) {
		m_specialWait -= (int)(dt * 1000.0f);
		if (m_specialWait < 0) {
			if (m_tipCaddieForm && !m_tipCaddieForm2) {
				m_tipCaddieForm->Close(true);
			} else {
				PopMacro();
			}
		}
	}

	if (m_step == 1) {
		if (!m_missionForm && !WSingleton<CCapturedBg>::Instance()->IsCaptured()) {
			OpenMissionForm();
		}

		if (m_step == 1 && m_macroHold) {
			m_macroTimer += (int)(dt * -1000.0f);
			if (m_macroTimer < 0) {
				m_macroTimer = 24000;
				if (GOLFDOC()->m_gameMode & 0x208) {
					Again_Msg(false);
				}
			} else if (m_macroTimer < 15000) {
				Again_Msg(true);
			}
		}
	}

	for (int i = 0; i < 0x1A; i++) {
		m_tutorialOverlays[i].Process(dt);
	}

	m_singleOverlay.Process(dt);
	if (m_singleOverlay.IsActivate()) {
		if (m_singleOverlay.m_src.y > 20.0f) {
			m_singleOverlay.m_src.y -= 2.0f;
		}
		if (m_pulseScale > 1.0f) {
			m_pulseScale -= 0.004f;
		}
		m_singleOverlay.m_src.w = m_pulseScale * 262.0f;
		m_singleOverlay.m_src.h = m_pulseScale * 44.0f;
		m_singleOverlay.m_src.x = (g_view->GetWidth() - m_singleOverlay.m_src.w) * 0.5f;
	}

	for (int i = 0; i < 3; i++) {
		m_chanceOverlays[i].Process(dt);
	}

	switch (m_step) {
	case 1:
		if (GOLFDOC()->m_gameState != 12 && CheckMissionComplete()) {
			MissionComplate();
		}
		break;
	case 4:
		m_step = 5;
		BlockInput();
		OpenMissionForm();
		OpenQuizForm(m_quizIndex);
		break;
	case 6:
		if (IsLocalContent((localContentType_t)0x38)) {
			if (m_missionForm) {
				m_missionForm->Close(true);
			}
			if (m_quizCorrect) {
				MissionComplate();
			} else {
				HandleMsg(MsgObject(0, 0x154, 0, 0, 0, 0, 0));
			}
		}
		break;
	}
}

// tutorial.cpp:683
void CTutorial::OnDisplay(void) {
	if (!IsLocalContent((localContentType_t)0x38)) {
		return;
	}

	for (int i = 0; i < 0x1A; i++) {
		m_tutorialOverlays[i].Render();
	}

	m_singleOverlay.Render();

	ChanceOverlay();

	for (int i = 0; i < 3; i++) {
		m_chanceOverlays[i].Render();
	}

	if (WSingleton<CCapturedBg>::Instance()->IsCaptured()) {
		return;
	}

	if (m_missionOverlay) {
		m_missionOverlay->Render(g_view, WRect(0, 0, 1.0f, 1.0f), WRect(g_view->GetWidth() - m_missionOverlay->GetWidth() - 10.0f, 40.0f, m_missionOverlay->GetWidth(), m_missionOverlay->GetHeight()), 0, 0xFFFFFFFF, 0, 0);
	}
}

// tutorial.cpp:715
void CTutorial::OnDestroy(void) {
	if (CSharedDoc::Instance()->m_roomType == 0xB || CSharedDoc::Instance()->m_roomType == 0xC) {
		SetCurMap(0);
		CSharedDoc::Instance()->m_roomType = 0;
		strcpy(CSharedDoc::Instance()->m_roomName, CSharedDoc::Instance()->m_gameTypeNames[0].name);

		g_input->FreeKey();
		g_input->FreeMouse();

		ClearMacro();
		m_texts.clear();

		if (g_resrcmng) {
			if (GOLFDOC()->m_target.m_cameraTarget) {
				g_resrcmng->Release(GOLFDOC()->m_target.m_cameraTarget, false);
				GOLFDOC()->m_target.m_cameraTarget = 0;
			}
		}

		if (g_resrcmng && m_missionOverlay) {
			g_resrcmng->Release(m_missionOverlay);
			m_missionOverlay = 0;
		}
	}
}

// tutorial.cpp:737
void CTutorial::HandleMsg(const MsgObject &msg) {
	switch (msg.m_msg) {
	case 0x148:
		PopMacro();
		break;
	case 0x14B:
		if (!m_yesNoForm && !m_missionCleared[GOLFDOC()->m_gameState]) {
			m_singleMission = true;
		}
		SkipMission();
		break;
	case 0x149:
		if (m_step == 1) {
			if (GOLFDOC()->m_gameState == 12) {
				CheckMissionComplete();
			} else {
				MissionComplate();
			}
		} else {
			PopMacro();
		}
		break;
	case 0x154:
		if (!IsLocalContent((localContentType_t)0x38)) {
			break;
		}

		if (m_reviewForm) {
			m_reviewForm->Close(FrCANCEL, true);
		}

		BlockInput();

		g_input->BlockKey("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9");

		if (m_step == 1 || m_step == 6) {
			m_dialogType = 4;
			m_dialogState = 2;
			m_step = 7;

			switch (GOLFDOC()->m_gameState) {
			case 2:
				m_dialogType = 1;
				OpenTipCaddieForm(m_texts[0x133].c_str(), (FRESH_RESULT_CALLBACK)&CTutorial::OnTipCaddieNoneResult);
				OpenMissionFailedForm();
				return;
			case 4:
				if (m_failCount >= 2) {
					m_failCount++;
					OpenTipCaddieForm(m_texts[0x199].c_str(), (FRESH_RESULT_CALLBACK)&CTutorial::OnTipCaddieNoneResult);
					OpenMissionFailedForm();
					return;
				}
				m_step = 1;
				m_failCount++;
				FreeInput();
				SendMsg(this, "Screen", 0x1BA, 1, 0, 0, 0);
				break;
			case 3:
				OpenTipCaddieForm(m_texts[0x260].c_str(), (FRESH_RESULT_CALLBACK)&CTutorial::OnTipCaddieNoneResult);
				OpenMissionFailedForm();
				return;
			case 5:
				OpenTipCaddieForm(m_texts[0x32A].c_str(), (FRESH_RESULT_CALLBACK)&CTutorial::OnTipCaddieNoneResult);
				OpenMissionFailedForm();
				return;
			case 8:
				SendMsg(this, "Letter", 0x18C, 0, 0, 0, 0);
				OpenTipCaddieForm(m_texts[0x38B].c_str(), (FRESH_RESULT_CALLBACK)&CTutorial::OnTipCaddieNoneResult);
				OpenMissionFailedForm();
				return;
			case 6:
				OpenTipCaddieForm(m_texts[0x2C6].c_str(), (FRESH_RESULT_CALLBACK)&CTutorial::OnTipCaddieNoneResult);
				OpenMissionFailedForm();
				return;
			case 7:
				OpenTipCaddieForm(m_texts[0x1FE].c_str(), (FRESH_RESULT_CALLBACK)&CTutorial::OnTipCaddieNoneResult);
				OpenMissionFailedForm();
				return;
			case 9:
				OpenTipCaddieForm(m_texts[0x453].c_str(), (FRESH_RESULT_CALLBACK)&CTutorial::OnTipCaddieNoneResult);
				OpenMissionFailedForm();
				return;
			case 10:
				OpenTipCaddieForm(m_texts[0x4B8].c_str(), (FRESH_RESULT_CALLBACK)&CTutorial::OnTipCaddieNoneResult);
				OpenMissionFailedForm();
				return;
			case 12:
				while (!m_macros.empty()) {
					m_macros.pop();
				}
				PushMacroHitPointMission();
				m_targetHits = 0;
				SendMsg(this, "GolfRule", 0x89, (intptr_t)"\xBD\xC3\xC0\xDB\xC1\xA1", (intptr_t)"\xB3\xAB\xC7\xCF\xC1\xF6\xC1\xA1\x33", 0, 0);
				PopMacro();
				OpenTipCaddieForm(m_texts[0x51D].c_str(), (FRESH_RESULT_CALLBACK)&CTutorial::OnTipCaddieNoneResult);
				OpenMissionFailedForm();
				return;
			case 11:
				OpenTipCaddieForm(m_texts[0x3F2].c_str(), (FRESH_RESULT_CALLBACK)&CTutorial::OnTipCaddieNoneResult);
				OpenMissionFailedForm();
				return;
			case 13:
				OpenTipCaddieForm(m_texts[0x585].c_str(), (FRESH_RESULT_CALLBACK)&CTutorial::OnTipCaddieNoneResult);
				OpenMissionFailedForm();
				return;
			case 14:
				OpenTipCaddieForm(m_texts[0x5E9].c_str(), (FRESH_RESULT_CALLBACK)&CTutorial::OnTipCaddieNoneResult);
				OpenMissionFailedForm();
				return;
			default:
				return;
			}
		} else {
			SendMsg(this, "Screen", 0x1BA, 1, 0, 0, 0);
		}
		break;
	case 0x14C:
		ReviewTutorial();
		break;
	case 0x14A: {
		const float *pos = (const float *)msg.m_param1;
		m_tipPos[0] = pos[0];
		m_tipPos[1] = pos[1];
		break;
	}
	case 0x152:
		*(unsigned long **)msg.m_param1 = m_missionItems;
		break;
	case 0x155:
		m_reserved2D8 = (float)msg.m_param1;
		if (m_reserved2D8 < GOLFDOC()->m_value15C) {
			GOLFDOC()->m_bar.m_accuracy = (float)GOLFDOC()->m_value15C;
			SendMsg(0, "GolfRule", 0xA2, 0, 0, 0, 0);
		}
		break;
	case 0x156:
		PopSpecial();
		break;
	case 0x20F:
		switch (msg.m_param1) {
		case 0:
			m_keepMouseBlocked = true;
			g_input->BlockMouse(LEFT_BUTTON);
			BlockInput();
			break;
		case 1:
			m_keepMouseBlocked = false;
			g_input->FreeMouse(LEFT_BUTTON);
			break;
		case 2: {
			float accuracy = *(const float *)msg.m_param2;
			GOLFDOC()->m_bar.m_accuracy = accuracy;
			if (msg.m_param3 == 1) {
				GOLFDOC()->m_bar.m_state = 1;
			} else if (msg.m_param3 == 2) {
				GOLFDOC()->m_bar.m_prevAccuracy = accuracy;
			}
			break;
		}
		}
		break;
	}
}

// tutorial.cpp:996
bool CTutorial::CheckMissionComplete(void) {
	switch (GOLFDOC()->m_gameState) {
	case 0:
		return CheckCameraTarget();
	case 1:
		return CheckClubTarget();
	case 12:
		m_targetHits++;

		if (m_targetHits == 4) {
			MissionComplate();
		}
		BlockInput();
		WSingleton<CMouseCursor>::Instance()->SetActive(true);

		if (m_tipForm) {
			m_tipForm->Close(FrCANCEL, true);
		}
		PopMacro();
		return true;
	}

	return false;
}

// tutorial.cpp:1033
void CTutorial::BlockInput(void) {
	g_input->BlockKey();

	g_input->FreeKey("ESCAPE");
	g_input->FreeKey("ENTER");
	g_input->FreeKey("PADENTER");
	g_input->FreeKey("FUNC_11");

	WSingleton<CMouseCursor>::Instance()->m_inputEnable = false;

	g_input->BlockMouse(MIDDLE_BUTTON);
	g_input->BlockMouse(RIGHT_BUTTON);

	m_inputBlocked = true;
}

// tutorial.cpp:1050
void CTutorial::FreeInput(void) {
	g_input->FreeKey();

	g_input->FreeMouse();
	g_input->FreeMouse(LEFT_BUTTON);
	g_input->FreeMouse(MIDDLE_BUTTON);
	g_input->FreeMouse(RIGHT_BUTTON);

	WSingleton<CMouseCursor>::Instance()->m_inputEnable = true;

	m_inputBlocked = false;
}

// tutorial.cpp:1065
char *CTutorial::GetLine(char *src, char *dst) {
	while (*src != '"' && *src != 0)
		src++;

	if (*src == 0)
		return 0;

	for (src++; *src != 0 && *src != '"';) {
		if (*src == '\\' && src[1] == 'n') {
			*dst++ = '\n';
			src += 2;
		} else {
			*dst++ = *src++;
		}
	}

	if (*src == 0)
		return 0;

	src++;
	*dst = 0;
	return src;
}

// A 12-byte entry per tutorial (data at 0x9F63C0); only its first int is read.
struct sTutorialTableEntry {
	int m_info;        // 0x0
	int m_reserved[2]; // 0x4
};
extern "C" const sTutorialTableEntry data_9f63c0[];

// tutorial.cpp:1097
void CTutorial::IsPractice(void) {
	CTask *pTask = AfxGetPreservedTask();
	if (pTask != 0) {
		IActor *pActor = pTask->GetActor("TutorialMain");
		if (pActor != 0) {
			int value1;
			int value2;
			int mode;
			int value4;
			pActor << MsgObject(0, 0x14F, (intptr_t)&value1, (intptr_t)&value2, (intptr_t)&mode, (intptr_t)&m_tutorialIndex, (intptr_t)&value4);
			GOLFDOC()->m_gameState = mode;

			m_tutorialInfo = data_9f63c0[m_tutorialIndex].m_info;
		}
	}

	m_yesNoForm = 0;
	WSingleton<CMouseCursor>::Instance()->Store();

	for (int i = 0; i < 0x1A; i++) {
		AnimatedOverlay &overlay = m_tutorialOverlays[i];
		overlay.m_active = false;
		overlay.m_loop = true;
		overlay.m_fadeDir = -1;
	}

	BlockInput();

	SendMsg(this, "Screen", 0x1BC, 2, 0, 0, 0);
	InitTutorialMode(GOLFDOC()->m_gameState);
}

// tutorial.cpp:1133
void CTutorial::ReviewTutorial(void) {
	if (m_reviewForm != 0) {
		return;
	}

	BlockInput();

	WSingleton<CMouseCursor>::Instance()->SetActive(true);
	if (m_tipForm) {
		m_tipForm->Close(FrCANCEL, true);
	}
	m_step = 3;

	m_reviewForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);
	m_reviewForm->SetMessage("\xBC\xB3\xB8\xED\xC0\xBB \xB4\xD9\xBD\xC3 \xB5\xE9\xC0\xB8\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);
	m_reviewForm->EnableDrag(false);
	m_reviewForm->Open((FRESH_RESULT_CALLBACK)&CTutorial::OnReviewFormResult,
		WPoint((g_view->GetWidth() - m_reviewForm->GetRect().w) * 0.5f, 80.0f), 2);
	m_reviewForm->MoveCursor("ok");
}

// tutorial.cpp:1153
void CTutorial::MoveNextStep(void) {
	if (!IsLocalContent((localContentType_t)0x38)) {
		if (m_yesNoForm != 0) {
			return;
		}

		if (m_missionForm) {
			m_missionForm->Close(FrNONE, true);
		}

		if (!m_singleMission) {
			m_missionCleared[GOLFDOC()->m_gameState] = true;
		}

		BlockInput();

		WSingleton<CMouseCursor>::Instance()->SetActive(true);
		if (m_tipForm) {
			m_tipForm->Close(FrCANCEL, true);
		}
		m_step = 3;
	}

	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
		CProjectG::Instance()->FromGolfToLobbySettings();

		int complete = m_tutorialInfo == 0x80;

#ifdef JP_PAKS
		// JP 0x7cd2d0: the mission's packet 0xAE goes through the tutorial
		// container 0xa4 (0x9079e0, CTutorial +0xdac), then back to the
		// lobby's tutorial page: Lobby 0 "TOPPAGE", Lobby 0x55 "tutorial"
		// (the desktop's bar state 8, then 0x88 "CTutorialWidget").
		JpTutorialContainer_SendMissionResult(m_tutorialIndex, m_tutorialInfo, 1, complete);
		CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);
		CTaskManager::Instance()->PostMsg(0, "Lobby", 0, (intptr_t)"TOPPAGE", 0, 0, 0);
		CTaskManager::Instance()->PostMsg(0, "Lobby", 0x55, (intptr_t)"tutorial", 0, 0, 0);
#else
		CTask *pTask = AfxGetPreservedTask();
		if (pTask != 0) {
			IActor *pActor = pTask->GetActor("TutorialMain");
			if (pActor != 0) {
				pActor << MsgObject(0, 0x150, m_tutorialIndex, m_tutorialInfo, 1, complete, 0);
			}
		}

		CTaskManager::Instance()->ChangeTask("CTutorialTask", "", false);
		CTaskManager::Instance()->PostMsg(0, "TutorialMain", 0, (intptr_t)"TUTORIAL_MAIN", 0, 0, 0);
#endif
	}

	if (GOLFDOC()->m_gameState < 15) {
		m_yesNoForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "tutorial", 0);
		m_yesNoForm->SetMessage("\xB9\xCC\xBC\xC7\xC0\xBB \xBF\xCF\xB7\xE1\xC7\xCF\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9. \x0A\x0A\xC3\xE0\xC7\xCF\xC7\xD5\xB4\xCF\xB4\xD9! ", false);
	} else {
		m_yesNoForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "tutorial", 0);
		m_yesNoForm->SetMessage("\xC6\xCE\xBE\xDF\xB9\xE8\xBF\xEC\xB1\xE2\xB8\xA6 \xB8\xB6\xC4\xA1\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9. \xC3\xE0\xC7\xCF\xC7\xD5\xB4\xCF\xB4\xD9.", false);
	}

	if (m_yesNoForm) {
		m_yesNoForm->EnableDrag(false);
		m_yesNoForm->Open((FRESH_RESULT_CALLBACK)&CTutorial::OnCompleteFormResult,
			WPoint((g_view->GetWidth() - m_yesNoForm->GetRect().w) * 0.5f, 80.0f), 2);
		m_yesNoForm->MoveCursor("ok");
	}
}

// tutorial.cpp:1238
void CTutorial::MissionComplate(void) {
	if (m_yesNoForm != 0) {
		return;
	}

	if (m_missionForm) {
		m_missionForm->Close(FrNONE, true);
	}

	if (!m_singleMission) {
		m_missionCleared[GOLFDOC()->m_gameState] = true;
	}

	BlockInput();

	WSingleton<CMouseCursor>::Instance()->SetActive(true);
	if (m_tipForm) {
		m_tipForm->Close(FrCANCEL, true);
	}
	m_step = 3;

	m_singleOverlay.m_src.w = 393.0f;
	m_singleOverlay.m_src.h = 66.0f;
	m_singleOverlay.m_src.x = (g_view->GetWidth() - 393.0f) * 0.5f;
	m_singleOverlay.m_src.y = (g_view->GetHeight() - 66.0f) * 0.5f;

	m_singleOverlay.m_frameTime = 0;

	m_singleOverlay.Activate(true, true, true);
	m_macroHold = false;
	PopMacro();
}

// tutorial.cpp:1277
void CTutorial::PrepareMission(void) {
	m_failCount = 0;

	if (GOLFDOC()->m_gameState == 3) {
		GOLFDOC()->GetPlayer(0)->m_comboGauge = 33.0f;
	} else if (GOLFDOC()->m_gameState == 5) {
		m_missionItems[0] = 0x18000004;
		m_missionItems[1] = 0x18000004;
		m_missionItems[2] = 0x18000004;
		GOLFDOC()->GetPlayer(0)->m_numItems = 3;
	} else if (GOLFDOC()->m_gameState == 8) {
		GOLFDOC()->GetPlayer(0)->m_ghostAward = 0;
		GOLFDOC()->GetPlayer(0)->m_powerShotLevel = 0;
		sPlayerData *player = GOLFDOC()->GetPlayer(0);
		player->m_holes[GOLFDOC()->m_currentHole - 1] = 1;
		SendMsg(this, "GolfRule", 0x89, (intptr_t)"\xbd\xc3\xc0\xdb\xc1\xa1", 0, 0, 0);
	}

	switch (GOLFDOC()->m_gameState) {
	case 2:
	case 3:
	case 5:
	case 11:
	case 13:
	case 14:
		SendMsg(this, "GolfRule", 0x89, (intptr_t)"\xbd\xc3\xc0\xdb\xc1\xa1", (intptr_t)"\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1" "3", 0, 0);
		break;
	}
}

// tutorial.cpp:1319
void CTutorial::SkipMission(void) {
	if (m_yesNoForm != 0) {
		return;
	}

	BlockInput();

	WSingleton<CMouseCursor>::Instance()->SetActive(true);
	if (m_tipForm) {
		m_tipForm->Close(FrCANCEL, true);
	}
	m_step = 3;

	m_yesNoForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);
	m_yesNoForm->SetMessage("\xC7\xF6\xC0\xE7 \xC1\xF8\xC7\xE0\xC1\xDF\xC0\xCE \xC4\xF9\xBD\xBA\xC6\xAE\xB8\xA6 \xC6\xF7\xB1\xE2\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);
	m_yesNoForm->EnableDrag(false);
	m_yesNoForm->Open((FRESH_RESULT_CALLBACK)&CTutorial::OnSkipMissionFormResult,
		WPoint((g_view->GetWidth() - m_yesNoForm->GetRect().w) * 0.5f, 80.0f), 2);
	m_yesNoForm->MoveCursor("ok");
}

// tutorial.cpp:1345
void CTutorial::OnImageLoad(void) {
	char name[256];

	m_tutorialOverlays[0].SetSourceOverlay("tuto_point.tga", 4, 1, 0, 1.0f, 1.0f);
	m_singleOverlay.SetSourceOverlay("text_questclear.png", 1, 1, 0, 1.0f, 1.0f);
	m_chanceOverlays[0].SetSourceOverlay("tutorial_icon_chance.png", 1, 1, 0, 1.0f, 1.0f);
	m_chanceOverlays[1].SetSourceOverlay("tutorial_icon_chance.png", 1, 1, 0, 1.0f, 1.0f);
	m_chanceOverlays[2].SetSourceOverlay("tutorial_icon_chance.png", 1, 1, 0, 1.0f, 1.0f);

	for (int i = 1; i < 26; i++) {
		if (i < 20) {
			sprintf(name, "tutorial_keyanmaion%02d.png", i);
		} else {
			sprintf(name, "tutorial_ground%02d.png", i - 20);
		}

		switch (i) {
		case 1:
			m_tutorialOverlays[1].SetSourceOverlay(name, 3, 1, 1, 128.0f, 88.0f);
			break;
		case 3:
			m_tutorialOverlays[3].SetSourceOverlay(name, 2, 1, 1, 128.0f, 88.0f);
			break;
		case 4:
			m_tutorialOverlays[4].SetSourceOverlay(name, 3, 1, 1, 136.0f, 88.0f);
			break;
		case 5:
			m_tutorialOverlays[5].SetSourceOverlay(name, 2, 1, 1, 136.0f, 88.0f);
			break;
		case 0:
		case 2:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 16:
		case 17:
		case 18:
		case 19:
			m_tutorialOverlays[i].SetSourceOverlay(name, 2, 1, 1, 128.0f, 88.0f);
			break;
		case 12:
			m_tutorialOverlays[12].SetSourceOverlay(name, 2, 4, 1, 136.0f, 105.0f);
			break;
		case 13:
		case 14:
			m_tutorialOverlays[i].SetSourceOverlay(name, 2, 1, 1, 136.0f, 105.0f);
			break;
		case 15:
			m_tutorialOverlays[15].SetSourceOverlay(name, 2, 2, 1, 136.0f, 105.0f);
			break;
		case 20:
		case 21:
		case 22:
			m_tutorialOverlays[i].SetSourceOverlay(name, 1, 1, 0, 1.0f, 1.0f);
			break;
		case 23:
			m_tutorialOverlays[23].SetSourceOverlay(name, 2, 1, 1, 128.0f, 88.0f);
			break;
		case 24:
			sprintf(name, "tu_icon_click.tga");
			m_tutorialOverlays[24].SetSourceOverlay(name, 1, 1, 0, 1.0f, 1.0f);
			break;
		case 25:
			sprintf(name, "tu_arrow_click.tga");
			m_tutorialOverlays[25].SetSourceOverlay(name, 1, 1, 0, 1.0f, 1.0f);
			break;
		}
	}

	m_tutorialOverlays[23].SetSourceOverlay("tutorial_preview.png", 1, 1, 0, 1.0f, 1.0f);
}

// tutorial.cpp:1475
void CTutorial::Again_Msg(bool retry) {
	if (m_step == 1) {
		if (retry) {
			if (m_tipCaddieForm) {
				m_tipCaddieForm->Close(true);
			}
			return;
		}

		m_dialogType = 1;
		m_dialogState = 0;

		int textKey = 5;
		switch (GOLFDOC()->m_gameState) {
		case 1: textKey = 0xD5; break;
		case 2: textKey = 0x134; break;
		case 3: textKey = 0x19A; break;
		case 4: textKey = 0x25F; break;
		case 5: textKey = 0x329; break;
		case 9: textKey = 0x453; break;
		case 10: textKey = 0x4B9; break;
		case 11: textKey = 0x3F3; break;
		case 12: textKey = 0x51D; break;
		case 13: textKey = 0x586; break;
		case 14: textKey = 0x5EA; break;
		}

		if (m_tipCaddieForm == 0) {
			OpenTipCaddieForm(m_texts[textKey].c_str(), (FRESH_RESULT_CALLBACK)&CTutorial::OnTipCaddieNoneResult);
		}

		if (m_tipCaddieForm) {
			m_tipCaddieForm->EnableFlag(2);
		}

		if (m_tipCaddieForm2) {
			m_tipCaddieForm2->Enable(true);
		}

		WSingleton<CMouseCursor>::Instance()->ForceMove(g_view->GetWidth() * 0.5f, g_view->GetHeight() * 0.5f, -1.0f);
	}
}

// tutorial.cpp:1529
void CTutorial::ChanceOverlay(void) {
	if (m_step == 1 && GOLFDOC()->m_gameState == 4) {
		for (int i = 0; i < 3; i++) {
			if (i < 3 - m_failCount) {
				m_chanceOverlays[i].m_src.x = g_view->GetWidth() - (i + 1) * 62.0f - 20.0f;
				m_chanceOverlays[i].m_src.y = 160.0f;
				m_chanceOverlays[i].m_src.w = 58.0f;
				m_chanceOverlays[i].m_src.h = 53.0f;
				m_chanceOverlays[i].Activate(true, true, true);
			} else {
				m_chanceOverlays[i].Activate(false, true, true);
			}
		}
	} else {
		for (int i = 0; i < 3; i++) {
			AnimatedOverlay &chance = m_chanceOverlays[i];
			chance.m_active = false;
			chance.m_loop = true;
			chance.m_fadeDir = -1;
		}
	}
}

// tutorial.cpp:1564
void CTutorial::PriviewOverlay(void) {
	float y = g_view->GetHeight() * 0.25f;
	float x = (g_view->GetWidth() - 128.0f) * 0.5f;
	PushMacro((eMacroType)0x13, MakeStr("%d %.1f %.1f", 0x17, x, y), 0, 1);
	PushMacro((eMacroType)0x10, MakeStr("%d %.1f %.1f %.1f", 0x17, 128.0, 64.0, 0.0), 0, 1);
	PushMacro(MACRO_WAIT, 0, 2000, 1);
	PushMacro((eMacroType)0x12, MakeStr("%d", 0x17), 500, 1);
}

// tutorial.cpp:1574
bool CTutorial::OnTipFormResult(int result, FrForm *form) {
	m_tipForm = 0;
	if (result == FrCANCEL) {
		return true;
	}

	if (m_keepMouseBlocked) {
		g_input->BlockMouse(LEFT_BUTTON);
	} else {
		FreeInput();
	}

	WSingleton<CMouseCursor>::Instance()->Store();

	if (m_tipState == 6) {
		m_tipState = 0;
		MissionComplate();
	} else {
		PopMacro();
	}

	return true;
}

extern bool g_bQuit;

// tutorial.cpp:1612
bool CTutorial::OnCompleteFormResult(int result, FrForm *form) {
	m_yesNoForm = 0;
	WSingleton<CMouseCursor>::Instance()->Store();

	if (!IsLocalContent((localContentType_t)0x38)) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 11 || GOLFDOC()->m_gameState == 7) {
			if (WSingleton<WNetworkSystem>::Instance()->IsConnected((WNetworkSystem::eNetUnit)0)) {
				CProjectG::Instance()->FromGolfToLobbySettings();

				CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);
				CTaskManager::Instance()->PostMsg(0, "Lobby", 0, (intptr_t)"TOPPAGE", 0, 0, 0);
			} else {
				g_bQuit = true;
			}
		} else {
			if (m_singleMission && result == 1) {
				m_reserved2A0 += result;
			}
			m_singleMission = false;
		}

		SendMsg(this, "Screen", 0x1BC, result == 1 ? 2 : 1, 0, 0, 0);
		InitTutorialMode(GOLFDOC()->m_gameState + (result == 1));
	} else {
		if (result == FrNONE) {
			return true;
		}

		if (!WSingleton<WNetworkSystem>::Instance()->IsConnected((WNetworkSystem::eNetUnit)0)) {
			g_bQuit = true;
		}
	}

	return true;
}

// tutorial.cpp:1670
bool CTutorial::OnReviewFormResult(int result, FrForm *form) {
	m_reviewForm = 0;

	WSingleton<CMouseCursor>::Instance()->Store();

	if (result == 1) {
		SendMsg(this, "Screen", 0x208, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1BC, 1, 0, 0, 0);
		InitTutorialMode(GOLFDOC()->m_gameState);
	} else if (result == 2) {
		if (GOLFDOC()->m_gameState == 6 || GOLFDOC()->m_gameState == 7) {
			m_step = 4;
		} else {
			FreeInput();

			m_step = 1;

			switch (GOLFDOC()->m_gameState) {
			case 2:
			case 3:
			case 4:
			case 5:
			case 8:
			case 10:
			case 11:
			case 13:
			case 14:
				break;
			default:
				OpenMissionForm();
				SendMsg(this, "Screen", 0x1BC, 2, 0, 0, 0);
				SendMsg(this, "Screen", 0x1BA, 1, 0, 0, 0);
				break;
			}
		}
	}

	return true;
}

// tutorial.cpp:1715
bool CTutorial::OnSkipMissionFormResult(int result, FrForm *form) {
	m_yesNoForm = 0;

	WSingleton<CMouseCursor>::Instance()->Store();

	if (m_singleMission == true && result == 1) {
		if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
			CProjectG::Instance()->FromGolfToLobbySettings();
#ifdef JP_PAKS
			// JP 0x7c1f71: back to the lobby's tutorial page.
			CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);
			CTaskManager::Instance()->PostMsg(0, "Lobby", 0x88, (intptr_t)"CTutorialWidget", (intptr_t)"TOPPAGE", 0, 0);
#else
			CTaskManager::Instance()->ChangeTask("CTutorialTask", "", false);
			CTaskManager::Instance()->PostMsg(0, "TutorialMain", 0, (intptr_t)"TUTORIAL_MAIN", 0, 0, 0);
#endif
		}
		return true;
	}

	if (GOLFDOC()->m_gameState == 6 || GOLFDOC()->m_gameState == 7) {
		m_step = 4;
		return true;
	}

	m_step = 1;
	FreeInput();
	OpenMissionForm();
	SendMsg(this, "Screen", 0x1BC, 2, 0, 0, 0);
	SendMsg(this, "Screen", 0x1BA, 1, 0, 0, 0);
	return true;
}

// tutorial.cpp:1751
bool CTutorial::OnPracticeFormResult(int result, FrForm *form) {
	m_yesNoForm = 0;
	WSingleton<CMouseCursor>::Instance()->Store();

	if (result == FrOK) {
		m_practice = true;
	}

	for (int i = 0; i < 0x1A; i++) {
		AnimatedOverlay &overlay = m_tutorialOverlays[i];
		overlay.m_active = false;
		overlay.m_loop = true;
		overlay.m_fadeDir = -1;
	}

	BlockInput();

	InitTutorialMode(GOLFDOC()->m_gameState);

	return true;
}

// Names of the tip form's three buttons ("ok", "yes", "keep"), indexed by m_dialogState (no symbol).
extern "C" const char data_9f64b0[][8];
// Pipin's face images of the tip form, indexed by m_dialogType (no symbol).
extern "C" const char data_9f64c8[][32];

// tutorial.cpp:1777
void CTutorial::OpenTipCaddieForm(const char *text, FRESH_RESULT_CALLBACK callback) {
	if (m_reviewForm) {
		m_reviewForm->Close(FrCANCEL, true);
	}

	if (m_tipCaddieForm == 0 && g_view != 0) {
		std::string message(text);
		if (message.size() <= 75) {
			m_tipCaddieForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "tip_imageBox", 0);
		} else if (message.size() <= 100) {
			m_tipCaddieForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "tip_MimageBox", 0);
		} else {
			m_tipCaddieForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "tip_BimageBox", 0);
		}

		m_tipCaddieForm->SetMessage(text, false);
		m_tipCaddieForm->EnableDrag(false);

		FrArea *face = DYNAMIC_CAST(FrArea, m_tipCaddieForm->FindChildByName("pipin_face"));
		if (face) {
			face->SetBgImg(data_9f64c8[m_dialogType]);
		}

		for (unsigned int i = 0; i < 3; i++) {
			FrButton *button = 0;
			if ((__rtti_obj = m_tipCaddieForm->FindChildByName(data_9f64b0[i])) != 0) {
				void *child = __rtti_obj;
				button = (FrButton *)(((IObject *)child)->IsKindOf(&FrButton::m_RTTI) ? child : 0);
			}
			if (button) {
				bool selected = (i == (unsigned int)m_dialogState);
				if (selected) {
					m_tipCaddieForm2 = button;
				}
				button->SetVisible(selected);
				button->Enable(false);
			}
		}

		m_tipRect.x = (g_view->GetWidth() - m_tipCaddieForm->GetRect().w) * 0.5f;
		m_tipRect.y = 50.0f;
		m_tipRect.w = m_tipCaddieForm->GetRect().w;
		m_tipRect.h = m_tipCaddieForm->GetRect().h;
		m_tipCaddieForm->SetKeyEvent(true);
		m_tipCaddieForm->Open(callback,
			WPoint((g_view->GetWidth() - m_tipCaddieForm->GetRect().w) * 0.5f, 50.0f), 4);
		m_tipCaddieForm->MoveCursor(data_9f64b0[m_dialogState]);
	}
}

// tutorial.cpp:1839
bool CTutorial::OnTipCaddieFormResult(int result, FrForm *form) {
	m_tipCaddieForm = 0;
	m_tipCaddieForm2 = 0;
	if (result == FrCANCEL) {
		return true;
	}
	if (result == FrNONE) {
		return true;
	}

	WSingleton<CMouseCursor>::Instance()->Store();

	if (m_tipState == 6) {
		m_tipState = 0;
		MissionComplate();
	} else {
		PopMacro();
	}

	return true;
}

// tutorial.cpp:1869
bool CTutorial::OnTipCaddieReopenQuizResult(int result, FrForm *form) {
	m_tipCaddieForm = 0;
	m_tipCaddieForm2 = 0;
	m_step = 4;
	return true;
}

// tutorial.cpp:1877
bool CTutorial::OnTipCaddieNoneResult(int result, FrForm *form) {
	m_tipCaddieForm = 0;
	m_tipCaddieForm2 = 0;

	return true;
}

// The mission overlay image of each tutorial (14 entries of 32 chars, no symbol).
extern "C" const char data_9f65e8[14][32];
// The vertical offset of each tutorial's mission overlay (no symbol).
extern "C" const int data_9f67a8[];

// tutorial.cpp:1890
void CTutorial::OpenMissionForm(void) {
	m_dialogType = 1;
	m_dialogState = 2;

	if (m_missionOverlay == 0) {
		if (m_tutorialIndex >= 0 && m_tutorialIndex < sizeof(data_9f65e8) / sizeof(data_9f65e8[0])) {
			m_missionOverlay = g_resrcmng->GetOverlay(data_9f65e8[m_tutorialIndex], 0);
		}
	}

	if (m_missionForm == 0) {
		m_missionForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "tuto_mission2", 0);

		float x = g_view->GetWidth() - m_missionForm->GetRect().w - 10.0f;
		float y = 25.0f;
		if (m_missionOverlay != 0) {
			x = g_view->GetWidth() - m_missionOverlay->GetWidth() + 20.0f;

			y = m_missionOverlay->GetHeight() + 25.0f - data_9f67a8[m_tutorialIndex];
		}

		m_missionForm->SetFixed(true);

		m_missionForm->SetKeyEvent(true);
		m_missionForm->Open((FRESH_RESULT_CALLBACK)&CTutorial::OnMissionFormResult, WPoint(x, y), 6);
	}

	WSingleton<CMouseCursor>::Instance()->ForceMove(g_view->GetWidth() * 0.5f, g_view->GetHeight() * 0.5f, -1.0f);
}

// tutorial.cpp:1934
bool CTutorial::OnMissionFormResult(int result, FrForm *form) {
	if (m_tipCaddieForm) {
		m_tipCaddieForm->Close(true);
	}

	if (m_quizForm) {
		m_quizForm->Close(true);
	}

	if (g_resrcmng && m_missionOverlay) { g_resrcmng->Release(m_missionOverlay); m_missionOverlay = 0; }

	m_missionForm = 0;
	if (result == 1) {
		this << MsgObject(0, 0x14C, 0, 0, 0, 0, 0);
	} else if (result == 2) {
		this << MsgObject(0, 0x14B, 0, 0, 0, 0, 0);
	}

	return true;
}

// tutorial.cpp:1971
void CTutorial::OpenQuizForm(int quiz) {
	if (m_quizForm == 0) {
		if (quiz == 1) {
			m_quizForm = CreateForm<FrQuizDlg>(g_pFresh->GetManager(), this, "tuto_quiz_ground", 0);
		} else {
			m_quizForm = CreateForm<FrQuizDlg>(g_pFresh->GetManager(), this, "tuto_quiz_score", 0);
		}

		m_quizForm->SetupQuiz(quiz);
		m_dialogType = 1;
		m_dialogState = 2;
		m_quizForm->SetKeyEvent(true);
		m_quizForm->SetFixed(true);
		m_quizForm->EnableDrag(false);
		m_quizForm->Open((FRESH_RESULT_CALLBACK)&CTutorial::OnQuizFormResult,
			WPoint((g_view->GetWidth() - m_quizForm->GetRect().w) * 0.5f,
				(g_view->GetHeight() - m_quizForm->GetRect().h) * 0.5f + 10.0f), 3);
		m_quizForm->MoveCursor("cases");
		m_step = 5;
		m_quizCorrect = false;
	}
}

// tutorial.cpp:2009
bool CTutorial::OnQuizFormResult(int result, FrForm *form) {
	if (m_tipCaddieForm) {
		m_tipCaddieForm->Close(true);
	}

	if (m_quizForm) {
		m_quizCorrect = m_quizForm->IsCorrectAnswer();
	}

	m_quizForm = 0;
	m_step = 6;
	if (result == 2) {
		m_missionForm->Close(FrCANCEL, true);
	}

	return true;
}

// tutorial.cpp:2038
void CTutorial::OpenMissionFailedForm(void) {
	if (m_yesNoForm == 0) {
		m_yesNoForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);
		m_yesNoForm->SetMessage("\xB4\xD9\xBD\xC3 \xB5\xB5\xC0\xFC\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?\n\n(\xBE\xC6\xB4\xCF\xBF\xC0\xB8\xA6 \xB4\xA9\xB8\xA3\xB8\xE9 \xB9\xCC\xBC\xC7\xC0\xBB \xC6\xF7\xB1\xE2\xC7\xCF\xB0\xED\n\xB9\xDB\xC0\xB8\xB7\xCE \xB3\xAA\xB0\xA9\xB4\xCF\xB4\xD9)", false);
		m_yesNoForm->EnableDrag(false);
		m_yesNoForm->Open((FRESH_RESULT_CALLBACK)&CTutorial::OnMissionFailedFormResult,
			WPoint((g_view->GetWidth() - m_yesNoForm->GetRect().w) * 0.5f, 180.0f), 2);
		m_yesNoForm->MoveCursor("ok");
	}
}

// tutorial.cpp:2050
bool CTutorial::OnMissionFailedFormResult(int result, FrForm *form) {
	m_yesNoForm = 0;
	WSingleton<CMouseCursor>::Instance()->Store();

	if (m_tipCaddieForm) {
		m_tipCaddieForm->Close(true);
	}

	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
		if (result == 1) {
			m_step = result;

			if (GOLFDOC()->m_gameState == 7 || GOLFDOC()->m_gameState == 6) {
				m_step = 4;
				return true;
			}

			PrepareMission();
			OpenMissionForm();
			SendMsg(this, "Screen", 0x1BC, 2, 0, 0, 0);
			SendMsg(this, "Screen", 0x1BA, 1, 0, 0, 0);
			FreeInput();
			return true;
		} else if (result == 2) {
			m_macroHold = false;

			CProjectG::Instance()->FromGolfToLobbySettings();

#ifdef JP_PAKS
			// JP 0x7c2109: no 0x150 (played 0 sends nothing), back to the
			// lobby's tutorial page.
			CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);
			CTaskManager::Instance()->PostMsg(0, "Lobby", 0x88, (intptr_t)"CTutorialWidget", (intptr_t)"TOPPAGE", 0, 0);
#else
			CTask *pTask = AfxGetPreservedTask();
			if (pTask != 0) {
				IActor *pActor = pTask->GetActor("TutorialMain");
				if (pActor != 0) {
					pActor << MsgObject(0, 0x150, m_tutorialIndex, m_tutorialInfo, 0, 0, 0);
				}
			}

			CTaskManager::Instance()->ChangeTask("CTutorialTask", "", false);
			CTaskManager::Instance()->PostMsg(0, "TutorialMain", 0, (intptr_t)"TUTORIAL_MAIN", 0, 0, 0);
#endif

			FreeInput();
			return true;
		} else {
			return true;
		}
	}

	SendMsg(this, "Screen", 0x1BA, 1, 0, 0, 0);
	FreeInput();

	return true;
}

// tutorial.cpp:2115
void CTutorial::PushMacroCamera(void) {
	WVector *pos;

	PushMacro((eMacroType)14, "1 ok", 0, 1);
	PushMacro((eMacroType)9, "100", -1, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d", 0x1BC), 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1);
	PushMacro((eMacroType)9, "101", -1, 1);
	PushMacro((eMacroType)15, 0, -1, 1);

	pos = &GOLFDOC()->m_pPolySoup->FindPoint("\xB3\xAB\xC7\xCF\xC1\xF6\xC1\xA1" "1_1")->m_pos; PushMacro((eMacroType)22, MakeStr("Caddie %d " PG_MACRO_PTR " 1", 0x1D8, PG_MACRO_PTRARG(pos)), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Caddie %d " PG_MACRO_PTR, 0x1D7, PG_MACRO_PTRARG("\xC4\xB3\xB5\xF0" "1_1")), 0, 1);
	PushMacro((eMacroType)21, "\xB3\xAB\xC7\xCF\xC1\xF6\xC1\xA1" "1_1", 0, 1);
	PushMacro((eMacroType)8, 0, -1, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1);
	PushMacro((eMacroType)9, "102", -1, 1);
	PushMacro((eMacroType)15, 0, -1, 1);

	pos = &GOLFDOC()->m_pPolySoup->FindPoint("\xB3\xAB\xC7\xCF\xC1\xF6\xC1\xA1" "1_2")->m_pos; PushMacro((eMacroType)22, MakeStr("Caddie %d " PG_MACRO_PTR " 1", 0x1D8, PG_MACRO_PTRARG(pos)), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Caddie %d " PG_MACRO_PTR, 0x1D7, PG_MACRO_PTRARG("\xC4\xB3\xB5\xF0" "1_2")), 0, 1);
	PushMacro((eMacroType)21, "\xB3\xAB\xC7\xCF\xC1\xF6\xC1\xA1" "1_2", -1, 1);

	PushMacro((eMacroType)14, "4 ok", 0, 1);
	PushMacro((eMacroType)9, "103", -1, 1);
	PushMacro((eMacroType)15, 0, -1, 1);

	pos = &GOLFDOC()->m_pPolySoup->FindPoint("\xB3\xAB\xC7\xCF\xC1\xF6\xC1\xA1" "1_3")->m_pos; PushMacro((eMacroType)22, MakeStr("Caddie %d " PG_MACRO_PTR " 1", 0x1D8, PG_MACRO_PTRARG(pos)), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Caddie %d " PG_MACRO_PTR, 0x1D7, PG_MACRO_PTRARG("\xC4\xB3\xB5\xF0" "1_3")), 0, 1);
	PushMacro((eMacroType)21, "\xB3\xAB\xC7\xCF\xC1\xF6\xC1\xA1" "1_3", -1, 1);

	PushMacro((eMacroType)14, "5 ok", 0, 1);
	PushMacro((eMacroType)9, "104", -1, 1);
	PushMacro((eMacroType)15, 0, -1, 1);

	pos = &GOLFDOC()->m_pPolySoup->FindPoint("\xB3\xAB\xC7\xCF\xC1\xF6\xC1\xA1" "1_4")->m_pos; PushMacro((eMacroType)22, MakeStr("Caddie %d " PG_MACRO_PTR " 1", 0x1D8, PG_MACRO_PTRARG(pos)), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Caddie %d " PG_MACRO_PTR, 0x1D7, PG_MACRO_PTRARG("\xC4\xB3\xB5\xF0" "1_4")), 0, 1);
	PushMacro((eMacroType)21, "\xB3\xAB\xC7\xCF\xC1\xF6\xC1\xA1" "1_4", -1, 1);

	PushMacro((eMacroType)14, "6 ok", 0, 1);
	PushMacro((eMacroType)9, "105", -1, 1);
	PushMacro((eMacroType)15, 0, -1, 1);

	pos = &GOLFDOC()->m_pPolySoup->FindPoint("\xB3\xAB\xC7\xCF\xC1\xF6\xC1\xA1" "1_5")->m_pos; PushMacro((eMacroType)22, MakeStr("Caddie %d " PG_MACRO_PTR " 1", 0x1D8, PG_MACRO_PTRARG(pos)), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Caddie %d " PG_MACRO_PTR, 0x1D7, PG_MACRO_PTRARG("\xC4\xB3\xB5\xF0" "1_5")), 0, 1);
	PushMacro((eMacroType)21, "\xB3\xAB\xC7\xCF\xC1\xF6\xC1\xA1" "1_5", -1, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1);
	PushMacro((eMacroType)9, "106", -1, 1);
	PushMacro((eMacroType)15, 0, -1, 1);

	pos = &GOLFDOC()->m_pPolySoup->FindPoint("\xB3\xAB\xC7\xCF\xC1\xF6\xC1\xA1" "1_6")->m_pos; PushMacro((eMacroType)22, MakeStr("Caddie %d " PG_MACRO_PTR " 1", 0x1D8, PG_MACRO_PTRARG(pos)), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Caddie %d " PG_MACRO_PTR, 0x1D7, PG_MACRO_PTRARG("\xC4\xB3\xB5\xF0" "1_6")), 0, 1);
	PushMacro((eMacroType)21, "\xB3\xAB\xC7\xCF\xC1\xF6\xC1\xA1" "1_6", -1, 1);

	PushMacro((eMacroType)14, "1 ok", 0, 1);
	PushMacro((eMacroType)9, "107", -1, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
}

// tutorial.cpp:2176
void CTutorial::PushMacroClub(void) {
	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "200", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "201", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "202", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "203", -1, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d " PG_MACRO_PTR " " PG_MACRO_PTR, 0x1C6, PG_MACRO_PTRARG(&m_tipPos[0]), PG_MACRO_PTRARG(&m_tipPos[1])), 0, 1);
	PushMacro((eMacroType)17, MakeStr("%d %.1f %.1f %.1f", 0, 64.0, 64.0, 1.0), 0, 1);

	PushMacro(MACRO_WAIT, 0, 1500, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 30, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "204", -1, 1);

	float width = g_view->GetWidth();

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, width * 0.5f - 32.0f, g_view->GetHeight() * 0.48333332f - 32.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 64.0, 64.0, 1.0), 0, 1);
	PushMacro(MACRO_WAIT, 0, 1500, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "205", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "206", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0xA, m_tipRect.x + m_tipRect.w - 128.0f, m_tipRect.y + m_tipRect.h + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0xA, 128.0, 88.0, 0.5), 0, 1);

	PushMacro((eMacroType)1, "PAGE_DOWN", 200, 5);
	PushMacro(MACRO_WAIT, 0, 1000, 1);
	PushMacro((eMacroType)1, "PAGE_UP", 200, 5);

	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0xA), 300, 1);

	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "207", -1, 1);

	PushMacro(MACRO_WAIT, 0, 30, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d", 0x1BC), 0, 1);
	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "208", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 9, m_tipRect.x + m_tipRect.w + 20.0f, m_tipRect.y + m_tipRect.h - 88.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 9, 128.0, 88.0, 0.5), 0, 1);
	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)1, "NUM_0", 0, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "209", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 1, m_tipRect.x + m_tipRect.w - 128.0f, m_tipRect.y + m_tipRect.h + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 1, 128.0, 88.0, 0.5), 200, 1);

	PushMacro(MACRO_WAIT, 0, 1500, 1);

	PushMacro((eMacroType)2, "LSHIFT", 0, 1);
	PushMacro((eMacroType)1, "\xBB\xF3", 1, 0x32);
	PushMacro(MACRO_WAIT, 0, 800, 1);

	PushMacro((eMacroType)1, "\xC7\xCF", 1, 0x32);
	PushMacro((eMacroType)3, 0, 500, 1);

	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 9), 300, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 1), 300, 1);

	PushMacro((eMacroType)1, "NUM_0", 0, 1);
	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "210", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)2, "\xC1\xC2", 250, 1);
	PushMacro((eMacroType)3, 0, 0, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "211", -1, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d %d", 0x206, 1), 0, 1);

	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)22, MakeStr("Caddie %d " PG_MACRO_PTR, 0x1D7, PG_MACRO_PTRARG("\xC4\xB3\xB5\xF0" "1_6")), 0, 1);
	PushMacro((eMacroType)21, "\xB3\xA1\xC1\xA1", 0, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x208, 1), 0, 1);
	PushMacro((eMacroType)8, 0, 20000, 1);

	PushMacro(MACRO_WAIT, 0, 2000, 1);
	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "212", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)26, 0, -1, 1);
}

// tutorial.cpp:2291
void CTutorial::PushMacroShot(void) {
	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "300", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)21, "\xB3\xAB\xC7\xCF\xC1\xF6\xC1\xA1" "3", 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "301", -1, 1);

	PushMacro(MACRO_WAIT, 0, 1500, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d", 0x1BC), 0, 1);

	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "302", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 3, m_tipRect.w + m_tipRect.x - 128.0f, m_tipRect.h + m_tipRect.y + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 3, 128.0, 88.0, 0.5), 0, 1);
	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 1 %d %d", 0x87, 0x1EA, 0x8C), -1, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d " PG_MACRO_PTR, 0x8A, PG_MACRO_PTRARG(m_tipPos)), 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d 1", 0x88), 0, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "303", -1, 1);

	PushMacro((eMacroType)17, MakeStr("%d %.1f %.1f %.1f", 0, 64.0, 64.0, 1.0), 0, 1);

	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 0", 0x88), 0, 1);

	PushMacro(MACRO_WAIT, 0, -1, 1);

	PushMacro((eMacroType)23, "0", 0, 1);
	PushMacro((eMacroType)24, "0", 0, 1);

	PushMacro((eMacroType)29, 0, 0, 1);

	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);

	PushMacro((eMacroType)18, MakeStr("%d", 0), 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 1", 0x88), 0, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "304", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 110.0, g_view->GetHeight() - 80.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 64.0, 64.0, 0.5), 0, 1);

	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)18, MakeStr("%d", 3), 0, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 0", 0x88), 0, 1);

	PushMacro((eMacroType)23, "0", 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d 0", 0x87), 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "305", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x208, 1), 0, 1);
	PushMacro((eMacroType)8, 0, 20000, 1);

	PushMacro(MACRO_WAIT, 0, 2000, 1);
	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "306", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)26, 0, -1, 1);
}

// tutorial.cpp:2411
void CTutorial::PushMacroGreen(void) {
	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "400", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "401", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "402", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 4, m_tipRect.w + m_tipRect.x - 128.0f, m_tipRect.h + m_tipRect.y + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 4, 128.0, 88.0, 0.5), 0, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "403", -1, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 4), 300, 1);

	PushMacro((eMacroType)22, MakeStr("Grid %d 1", 0x87), 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d 1 %d", 0x87, 0x15E), -1, 1);
	PushMacro((eMacroType)24, "0", 0, 1);

	PushMacro((eMacroType)29, 0, 0, 1);

	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);
	PushMacro((eMacroType)22, MakeStr("Grid %d 0", 0x87), 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d 0", 0x87), 0, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "404", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "405", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)22, MakeStr("Grid %d 1", 0x207), 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "406", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)2, "\xBF\xEC", 295, 1);
	PushMacro((eMacroType)3, 0, 0, 1);

	PushMacro((eMacroType)22, MakeStr("Grid %d 0", 0x207), 0, 1);

	PushMacro((eMacroType)22, MakeStr("Grid %d 1", 0x87), 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d 1 %d", 0x87, 0x15E), -1, 1);

	PushMacro((eMacroType)29, 0, 0, 1);

	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);
	PushMacro((eMacroType)22, MakeStr("Grid %d 0", 0x87), 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d 0", 0x87), 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "407", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)2, "\xBF\xEC", 100, 1);
	PushMacro((eMacroType)3, 0, 0, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x208, 1), 0, 1);
	PushMacro((eMacroType)8, 0, 20000, 1);

	PushMacro(MACRO_WAIT, 0, 2000, 1);
	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "408", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)26, 0, -1, 1);
}

// tutorial.cpp:2486
void CTutorial::PushMacroGauge(void) {
	PLAYER(GOLFDOC()->m_myPlayerIndex)->m_comboGauge = 30.0f;

	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "600", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "601", -1, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d", 0x1BD), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d", 0x1BC), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 3.0, 45.0), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 76.0, 22.0, 0.5), 0, 1);
	PushMacro(MACRO_WAIT, 0, 500, 1);
	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "602", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)24, "0", 0, 1);
	PushMacro((eMacroType)23, "0", 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d 1 %d %d", 0x87, 0x1D6, 0x8C), -1, 1);

	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);

	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", 1, 1);

	PushMacro((eMacroType)23, "0", 0, 1);
	PushMacro((eMacroType)2, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);

	PushMacro((eMacroType)3, 0, 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "603", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "604", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 5, m_tipRect.w + m_tipRect.x - 128.0f, m_tipRect.h + m_tipRect.y + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 5, 128.0, 88.0, 0.5), 0, 1);

	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 5), 300, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 0, 1);

	PushMacro((eMacroType)24, "0", 0, 1);
	PushMacro((eMacroType)23, "0", 0, 1);

	PushMacro((eMacroType)22, MakeStr("Player %d", 0x11D), 2000, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 1 %d %d", 0x87, 0x1EF, 0x8C), -1, 1);

	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);

	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", 1, 1);

	PushMacro((eMacroType)23, "0", 0, 1);
	PushMacro((eMacroType)2, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);

	PushMacro((eMacroType)3, 0, 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d 0", 0x87), 0, 1);

	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "605", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x208, 1), 0, 1);
	PushMacro((eMacroType)8, 0, 20000, 1);

	PushMacro(MACRO_WAIT, 0, 2000, 1);
	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "606", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)26, 0, -1, 1);


}

// tutorial.cpp:2614
void CTutorial::PushMacroGround(void) {
	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "700", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "701", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "702", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 9, m_tipRect.x + m_tipRect.w - 128.0f, m_tipRect.y + m_tipRect.h + 30.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 9, 128.0, 88.0, 0.5), 0, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 9), 300, 1);

	PushMacro((eMacroType)1, "NUM_0", 500, 1);

	PushMacro((eMacroType)5, MakeStr("%f %f", g_view->GetWidth() * 0.5f - 50.0f, g_view->GetHeight() * 0.5f - 30.0f), 500, 1);
	PushMacro((eMacroType)4, "0", 500, 1);
	PushMacro((eMacroType)5, MakeStr("%f %f", g_view->GetWidth() * 0.5f + 50.0f, g_view->GetHeight() * 0.5f + 50.0f), 500, 1);
	PushMacro((eMacroType)4, "0", 500, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "703", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0x16, m_tipRect.x + m_tipRect.w - 128.0f, m_tipRect.y + m_tipRect.h + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0x16, 256.0, 128.0, 0.0), 0, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0x16), 300, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "704", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0x14, m_tipRect.x + m_tipRect.w - 128.0f, m_tipRect.y + m_tipRect.h + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0x14, 256.0, 128.0, 0.0), 0, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0x14), 300, 1);

	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "705", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0x15, m_tipRect.x + m_tipRect.w - 128.0f, m_tipRect.y + m_tipRect.h + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0x15, 256.0, 128.0, 0.0), 0, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0x15), 300, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "706", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0x15, m_tipRect.x + m_tipRect.w - 128.0f, m_tipRect.y + m_tipRect.h + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0x15, 256.0, 128.0, 0.0), 0, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0x15), 300, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "707", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)1, "NUM_0", 500, 1);
	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "708", -1, 1);
	PushMacro((eMacroType)20, "1", -1, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "709", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)26, 0, -1, 1);
}

// tutorial.cpp:2679
void CTutorial::PushMacroItemUse(void) {
	m_missionItems[0] = 0x18000004;
	m_missionItems[1] = 0x18000004;
	m_missionItems[2] = 0x18000004;

	GOLFDOC()->GetPlayer(0)->m_numItems = 3;

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "800", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "801", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)24, "0", 0, 1);
	PushMacro((eMacroType)23, "0", 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "802", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 6, m_tipRect.w + m_tipRect.x - 128.0f, m_tipRect.h + m_tipRect.y + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 6, 128.0, 88.0, 0.5), 0, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 6), 300, 1);

	PushMacro((eMacroType)1, "ITEMSLOT", 0, 1);
	PushMacro((eMacroType)24, "0", 0, 1);
	PushMacro((eMacroType)23, "0", 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "803", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 7, 80.0, g_view->GetHeight() - 208.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 7, 128.0, 88.0, 0.5), 0, 1);

	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 7), 300, 1);

	PushMacro((eMacroType)24, "0", 0, 1);
	PushMacro((eMacroType)23, "0", 0, 1);

	PushMacro((eMacroType)1, "\xBF\xEC", 500, 1);
	PushMacro((eMacroType)1, "\xBF\xEC", 500, 1);

	PushMacro((eMacroType)24, "0", 0, 1);
	PushMacro((eMacroType)23, "0", 0, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "804", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 8, 80.0, g_view->GetHeight() - 208.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 8, 128.0, 88.0, 0.5), 0, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 8), 300, 1);

	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", 2000, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "805", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)24, "0", 0, 1);
	PushMacro((eMacroType)23, "0", 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 1 %d %d", 0x87, 0x1D6, 0x8C), -1, 1);

	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);

	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", 1, 1);

	PushMacro((eMacroType)23, "0", 0, 1);
	PushMacro((eMacroType)2, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);

	PushMacro((eMacroType)3, 0, 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d 0", 0x87), 0, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "806", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "807", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x208, 1), 0, 1);
	PushMacro((eMacroType)8, 0, 20000, 1);

	PushMacro(MACRO_WAIT, 0, 2000, 1);
	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "808", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)26, 0, -1, 1);
}

// tutorial.cpp:2781
void CTutorial::PushMacroScore(void) {
	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "500", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "501", -1, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d", 0x1BE), 0, 1);

	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "502", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 165.0, 8.0), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 50.0, 20.0, 1.0), 0, 1);
	PushMacro(MACRO_WAIT, 0, 300, 1);

	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "503", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "504", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1);
	PushMacro((eMacroType)9, "505", -1, 1);

	PushMacro(MACRO_WAIT, 0, 500, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 0, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "506", -1, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d", 0x1BD), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d", 0x1BC), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 2.0, 55.0), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 70.0, 38.0, 0.5), 0, 1);

	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "507", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "508", -1, 1);

	PushMacro((eMacroType)20, "0", -1, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "509", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)26, 0, -1, 1);
}

// tutorial.cpp:2840
void CTutorial::PushMacroHoleIn(void) {
	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "900", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "901", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "902", -1, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d", 0x1BE), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 94.0, 8.0), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 130.0, 20.0, 1.0), 0, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 0, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "903", -1, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d", 0x1BD), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d", 0x1BC), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 4.0, 56.0), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 65.0, 40.0, 1.0), 0, 1);
	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)18, MakeStr("%d", 0), 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "905", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x208, 1), 0, 1);
	PushMacro((eMacroType)8, 0, -1, 1);

	PushMacro(MACRO_WAIT, 0, 2000, 1);
	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "906", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)26, 0, -1, 1);
}

// tutorial.cpp:2895
void CTutorial::PushMacroWindReading(void) {
	PushMacro((eMacroType)21, "\xB3\xAB\xC7\xCF\xC1\xF6\xC1\xA1" "3", 0, 1);
	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "1000", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "1001", -1, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d", 0x20E), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, g_view->GetWidth() - 105.0f, g_view->GetHeight() - 105.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 103.0, 103.0, 0.5), 0, 1);
	PushMacro(MACRO_WAIT, 0, 500, 1);

	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "1002", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "1003", -1, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x1F0, 1), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x1F1, 0), 0, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, g_view->GetWidth() - 90.0f, g_view->GetHeight() - 90.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 80.0, 80.0, 0.5), 0, 1);
	PushMacro(MACRO_WAIT, 0, 500, 1);

	for (int angle = 0; angle <= 360; angle += 5) {
		PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x1F1, angle), 100, 1);
	}

	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1004", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, g_view->GetWidth() - 45.0f, g_view->GetHeight() - 45.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 40.0, 40.0, 0.5), 0, 1);

	for (int i = 0; i < 9; i++) {
		PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x1F0, i + 1), 600, 1);
	}

	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "1005", -1, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x1F1, 0x5A), 0, 1);
	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1006", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 1 %d %d", 0x87, 0x1D6, 0x8C), -1, 1);

	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);

	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", 1, 1);

	PushMacro((eMacroType)23, "0", 0, 1);
	PushMacro((eMacroType)2, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);

	PushMacro((eMacroType)3, 0, 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d 0", 0x87), 0, 1);

	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "1007", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "1008", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x208, 1), 0, 1);
	PushMacro((eMacroType)8, 0, 20000, 1);

	PushMacro(MACRO_WAIT, 0, 2000, 1);

	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "1009", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)26, 0, -1, 1);
}

// tutorial.cpp:3020
void CTutorial::PushMacroBallMove(void) {
	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "1100", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "1101", -1, 1);

	PushMacro((eMacroType)24, "0", 0, 1);
	PushMacro((eMacroType)2, "\xBB\xF3", 800, 1);
	PushMacro((eMacroType)2, "\xC1\xC2", 800, 1);
	PushMacro((eMacroType)3, 0, 0, 1);

	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "1102", -1, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0x20D), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, g_view->GetWidth() - 400.0f, 1.0), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 40.0, 30.0, 0.5), 0, 1);
	PushMacro(MACRO_WAIT, 0, 500, 1);

	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro((eMacroType)7, MakeStr("unplayable %d", 0x24A), 0, 1);
	PushMacro((eMacroType)30, "0", 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0x18, g_view->GetWidth() - 395.0f, 35.0), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0x18, 73.0, 33.0, 0.0), 300, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0x18, 80.3f, 36.3f, 0.0), 100, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0x18, 73.0, 33.0, 0.0), 1000, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0x18), 0, 1);

	PushMacro((eMacroType)31, 0, 0, 1);

	PushMacro((eMacroType)4, "0", 0, 1);
	PushMacro((eMacroType)30, "0", 800, 1);

	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "1103", -1, 1);

	PushMacro(MACRO_WAIT, 0, 500, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x1D0, 0), 300, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x1D0, 1), 300, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x1D0, 0), 300, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x1D0, 1), 300, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x1D0, 0), 300, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x1D0, 1), 300, 1);

	PushMacro((eMacroType)31, 0, 0, 1);
	PushMacro((eMacroType)30, "2", 0, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)30, "0", 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "1104", -1, 1);
	PushMacro((eMacroType)31, 0, 0, 1);
	PushMacro((eMacroType)30, "2", 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0xB, m_tipRect.w + m_tipRect.x - 128.0f, m_tipRect.h + m_tipRect.y + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0xB, 128.0, 88.0, 0.5), 500, 1);

	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0xB), 100, 1);

	PushMacro((eMacroType)6, MakeStr("%f %f", g_view->GetWidth() - g_view->GetWidth() * 0.125f, g_view->GetHeight() * 0.5f + 30.0f), 0, 1);

	PushMacro((eMacroType)30, "0", 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0x18, g_view->GetWidth() - g_view->GetWidth() * 0.125f, g_view->GetHeight() * 0.5f + 45.0f), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0x19, g_view->GetWidth() - g_view->GetWidth() * 0.125f - 245.0f, g_view->GetHeight() * 0.5f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0x19, 195.0, 106.0, 0.0), 0, 1);

	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0x18, 73.0, 33.0, 0.0), 300, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0x18, 80.3f, 36.3f, 0.0), 100, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0x18, 73.0, 33.0, 0.0), 1000, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0x18), 0, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0x19), 0, 1);

	PushMacro((eMacroType)31, 0, 100, 1);

	PushMacro((eMacroType)2, "LCONTROL", 100, 1);
	PushMacro((eMacroType)4, "0", 100, 1);
	PushMacro((eMacroType)4, "0", 300, 1);
	PushMacro((eMacroType)3, 0, 500, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "1105", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro(MACRO_WAIT, 0, 500, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d " PG_MACRO_PTR, 0x89, PG_MACRO_PTRARG("\xBD\xC3\xC0\xDB\xC1\xA1_Unplayable"), 0), 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0x20D), 0, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x208, 1), 0, 1);
	PushMacro((eMacroType)8, 0, 20000, 1);

	PushMacro(MACRO_WAIT, 0, 2000, 1);

	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "1106", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)26, 0, -1, 1);

	PushMacro((eMacroType)26, 0, -1, 1);
}

// tutorial.cpp:3150
void CTutorial::PushMacroBallSlope(void) {
	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "1200", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)21, "\xB3\xAB\xC7\xCF\xC1\xF6\xC1\xA1" "3", 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "1201", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "1202", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "1203", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 5.0, g_view->GetHeight() - 105.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 103.0, 103.0, 0.5), 0, 1);
	PushMacro(MACRO_WAIT, 0, 1000, 1);

	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro(MACRO_WAIT, 0, 500, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d " PG_MACRO_PTR, 0x89, PG_MACRO_PTRARG("Slope_Right"), 0), 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "1204", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 5.0, g_view->GetHeight() - 105.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 103.0, 103.0, 0.5), 0, 1);
	PushMacro(MACRO_WAIT, 0, 1000, 1);

	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 1 %d %d", 0x87, 0x1D6, 0x8F), -1, 1);

	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);

	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", 1, 1);

	PushMacro((eMacroType)23, "0", 0, 1);
	PushMacro((eMacroType)2, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);

	PushMacro((eMacroType)3, 0, 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d 0", 0x87), 0, 1);

	PushMacro(MACRO_WAIT, 0, 500, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d " PG_MACRO_PTR, 0x89, PG_MACRO_PTRARG("Slope_Left"), 0), 0, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1205", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 5.0, g_view->GetHeight() - 105.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 103.0, 103.0, 0.5), 0, 1);
	PushMacro(MACRO_WAIT, 0, 1000, 1);

	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 1 %d %d", 0x87, 0x1D6, 0x87), -1, 1);

	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);

	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", 1, 1);

	PushMacro((eMacroType)23, "0", 0, 1);
	PushMacro((eMacroType)2, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);

	PushMacro((eMacroType)3, 0, 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d 0", 0x87), 0, 1);

	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "1206", -1, 1);

	PushMacro(MACRO_WAIT, 0, 500, 1);
	PushMacro((eMacroType)22, MakeStr("Player %d %d %d %d %d", 0x157, 5, -1, -1), 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d " PG_MACRO_PTR, 0x89, PG_MACRO_PTRARG("Slope_Right"), 0), 300, 1);
	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x208, 1), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Player %d %d %d", 0x11C, 0x10000000, 1), 0, 1);
	PushMacro((eMacroType)8, 0, 20000, 1);

	PushMacro(MACRO_WAIT, 0, 2000, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1207", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)26, 0, -1, 1);
}

// tutorial.cpp:3306
void CTutorial::PushMacroHitPoint(void) {
	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "1300", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "1301", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "1302", -1, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x1D1, 1), 500, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x1D1, 0), 500, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x1D1, 1), 500, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x1D1, 0), 500, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x1D1, 1), 500, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 5.0, g_view->GetHeight() - 105.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 103.0, 103.0, 0.5), 0, 1);
	PushMacro(MACRO_WAIT, 0, 1000, 1);

	PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "1303", -1, 1);

	PushMacro((eMacroType)22, MakeStr("Spin %d", 0x16A), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0xC, m_tipRect.x + m_tipRect.w - 128.0f, m_tipRect.y + m_tipRect.h + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0xC, 137.0, 105.0, 1.0), 0, 1);

	PushMacro((eMacroType)22, MakeStr("Spin %d %d %d %d", 0x16B, 2, 3, 1), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d", 0x169, 1), 0, 1);

	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0xC), 300, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "1304", -1, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d %d %d", 0x16B, 1, 0, 0), 300, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d", 0x169, 0), 0, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x1D1, 0), 500, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d %d %d", 0x165, 1, 0, 0), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d %d %d", 0x165, 1, 0, 0xFFFFFFEC), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0xD, m_tipRect.x + m_tipRect.w - 128.0f, m_tipRect.y + m_tipRect.h + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0xD, 137.0, 105.0, 1.0), 0, 1);

	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0xD), 300, 1);

	PushMacro((eMacroType)14, "7 ok", 0, 1); PushMacro((eMacroType)9, "1305", -1, 1);

	PushMacro((eMacroType)22, MakeStr("Spin %d %d %d %d", 0x165, 1, 0, 0), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d %d %d", 0x165, 1, 0, 0x14), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0xE, m_tipRect.x + m_tipRect.w - 128.0f, m_tipRect.y + m_tipRect.h + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0xE, 137.0, 105.0, 1.0), 0, 1);

	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0xE), 300, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "1306", -1, 1);

	PushMacro((eMacroType)22, MakeStr("Spin %d %d", 0x169, 0), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0xF, m_tipRect.x + m_tipRect.w - 128.0f, m_tipRect.y + m_tipRect.h + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0xF, 137.0, 105.0, 1.0), 0, 1);

	PushMacro((eMacroType)22, MakeStr("Spin %d %d %d %d", 0x165, 1, 0xFFFFFFEC, 0), 1000, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d %d %d", 0x165, 1, 0x14, 0), 1000, 1);

	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0xF), 300, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "1307", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)22, MakeStr("Spin %d %d %d %d", 0x165, 1, 0, 0), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 5.0, g_view->GetHeight() - 105.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 103.0, 103.0, 0.5), 0, 1);

	PushMacroHitPointMission();
}

// tutorial.cpp:3409
void CTutorial::PushMacroHitPointMission(void) {
	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 465, 1), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 520, 1), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d %d %d", 357, 2, 0, -20), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d", 361, 2), 0, 1);
	PushMacro((eMacroType)8, 0, 20000, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d", 361, 1), 300, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d %d %d", 357, 2, -20, 0), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d", 361, 0), 1000, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d", 361, 2), 0, 1);
	PushMacro((eMacroType)8, 0, 20000, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d", 361, 1), 300, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d %d %d", 357, 2, 0, 20), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d", 361, 0), 1000, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d", 361, 2), 0, 1);
	PushMacro((eMacroType)8, 0, 20000, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d", 361, 1), 300, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d %d %d", 357, 2, 20, 0), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d", 361, 0), 1000, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d", 361, 2), 0, 1);
	PushMacro((eMacroType)8, 0, 20000, 1);
	PushMacro(MACRO_WAIT, 0, 2000, 1);
	PushMacro((eMacroType)14, "2 ok", 0, 1);
	PushMacro((eMacroType)9, "1308", -1, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)26, 0, -1, 1);
}

// The view matrix of the running camera (no symbol; za and pivot are read).
extern "C" WMatrix data_b7bf88;
// The smallest view-axis distance CheckCameraTarget measures (1e-5, no symbol).
extern "C" const float data_9f67e4;

// tutorial.cpp:3471
void CTutorial::PushMacroPowerSpin(void) {
	PushMacro((eMacroType)21, "\xB3\xAB\xC7\xCF\xC1\xF6\xC1\xA1" "3", 0, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d", 0x1A8), 0, 1);

	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "1400", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "1401", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1402", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)24, "0", 0, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d", 0x1A7), 0, 1);

	PushMacro((eMacroType)22, MakeStr("Spin %d %d", 0x169, 3), 0, 1);

	PushMacro((eMacroType)22, MakeStr("Spin %d %d %d %d", 0x165, 1, 0, 0xFFFFFFEC), 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "1403", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0xD, m_tipRect.x + m_tipRect.w - 128.0f, m_tipRect.y + m_tipRect.h + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0xD, 137.0, 105.0, 1.0), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 5.0, g_view->GetHeight() - 105.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 103.0, 103.0, 0.5), 0, 1);
	PushMacro(MACRO_WAIT, 0, 1000, 1);

	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0xD), 300, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro((eMacroType)28, "310", 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d 1 %d %d ", 0x87, 0x1EA, 0x8C), -1, 1);

	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);

	PushMacro((eMacroType)27, 0, 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA3), 0, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1404", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)2, "\xBB\xF3", 0, 1);

	PushMacro((eMacroType)28, "142", 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA2), 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA3), 0, 1);

	PushMacro((eMacroType)27, 0, 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 1", 0x88), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 110.0, g_view->GetHeight() - 115.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 123.0, 45.0, 1.0), 0, 1);
	PushMacro(MACRO_WAIT, 0, 300, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0x10, m_tipRect.x + m_tipRect.w - 128.0f, m_tipRect.y + m_tipRect.h + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0x10, 137.0, 105.0, 1.0), 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA3), 50, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1405", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0x10), 300, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1406", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 110.0, g_view->GetHeight() - 120.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 120.0, 103.0, 1.0), 0, 1);
	PushMacro(MACRO_WAIT, 0, 100, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro((eMacroType)28, "0", 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA2), 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA3), 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 0", 0x88), -1, 1);
	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", 1, 1);

	PushMacro((eMacroType)23, "0", 0, 1);
	PushMacro((eMacroType)2, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", 300, 1);

	PushMacro((eMacroType)3, 0, -1, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 0", 0x87), 100, 1);

	PushMacro((eMacroType)22, MakeStr("Spin %d %d", 0x169, 3), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d %d %d", 0x165, 1, 0, 0x14), 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "1407", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0xE, m_tipRect.x + m_tipRect.w - 128.0f, m_tipRect.y + m_tipRect.h + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0xE, 137.0, 105.0, 1.0), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 5.0, g_view->GetHeight() - 105.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 103.0, 103.0, 0.5), 0, 1);
	PushMacro(MACRO_WAIT, 0, 1000, 1);

	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0xE), 300, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro((eMacroType)28, "310", 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d 1 %d %d ", 0x87, 0x1EA, 0x8C), -1, 1);

	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);

	PushMacro((eMacroType)27, 0, 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA3), 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "1408", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)2, "\xC7\xCF", 0, 1);

	PushMacro((eMacroType)28, "142", 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA2), 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA3), 0, 1);

	PushMacro((eMacroType)27, 0, 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 1", 0x88), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 110.0, g_view->GetHeight() - 115.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 123.0, 45.0, 1.0), 0, 1);
	PushMacro(MACRO_WAIT, 0, 300, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0x11, m_tipRect.x + m_tipRect.w - 128.0f, m_tipRect.y + m_tipRect.h + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0x11, 137.0, 105.0, 1.0), 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA3), 50, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1409", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0x11), 300, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1410", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 110.0, g_view->GetHeight() - 120.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 120.0, 103.0, 1.0), 0, 1);
	PushMacro(MACRO_WAIT, 0, 100, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro((eMacroType)28, "0", 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA2), 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA3), 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 0", 0x88), -1, 1);
	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", 1, 1);

	PushMacro((eMacroType)23, "0", 0, 1);
	PushMacro((eMacroType)2, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", 300, 1);

	PushMacro((eMacroType)3, 0, -1, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 0", 0x87), 1000, 1);

	PushMacro((eMacroType)22, MakeStr("Spin %d %d", 0x169, 4), 0, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1411", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)3, 0, 0, 1);
	PushMacro(MACRO_WAIT, 0, 500, 1);

	PushMacro((eMacroType)22, MakeStr("Player %d %d %d %d %d", 0x157, -1, 0x1C, -1), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x208, 1), 0, 1);
	PushMacro((eMacroType)8, 0, 20000, 1);

	PushMacro(MACRO_WAIT, 0, 2000, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1412", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)26, 0, -1, 1);
}

// tutorial.cpp:3759
void CTutorial::PushMacroPowerCurve(void) {
	PushMacro((eMacroType)21, "\xB3\xAB\xC7\xCF\xC1\xF6\xC1\xA1_curve", 0, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d", 0x1A8), 0, 1);

	PushMacro((eMacroType)14, "1 ok", 0, 1); PushMacro((eMacroType)9, "1500", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "1501", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1502", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)24, "0", 0, 1);

	PushMacro((eMacroType)22, MakeStr("Player %d %d %d %d %d", 0x157, -1, -1, 7), 0, 1);

	PushMacro((eMacroType)22, MakeStr("Screen %d", 0x1A7), 0, 1);

	PushMacro((eMacroType)22, MakeStr("Spin %d %d", 0x169, 3), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d %d %d", 0x165, 1, 7, 0), 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "1503", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0xF, m_tipRect.x + m_tipRect.w - 128.0f, m_tipRect.y + m_tipRect.h + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0xF, 137.0, 105.0, 1.0), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 5.0, g_view->GetHeight() - 105.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 103.0, 103.0, 0.5), 0, 1);
	PushMacro(MACRO_WAIT, 0, 1000, 1);

	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0xF), 300, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro((eMacroType)28, "310", 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d 1 %d %d ", 0x87, 0x1EA, 0x8C), -1, 1);

	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);

	PushMacro((eMacroType)27, 0, 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA3), 0, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1504", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)2, "\xBF\xEC", 0, 1);

	PushMacro((eMacroType)28, "142", 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA2), 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA3), 0, 1);

	PushMacro((eMacroType)27, 0, 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 1", 0x88), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 110.0, g_view->GetHeight() - 115.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 123.0, 45.0, 1.0), 0, 1);
	PushMacro(MACRO_WAIT, 0, 300, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0x12, m_tipRect.x + m_tipRect.w - 128.0f, m_tipRect.y + m_tipRect.h + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0x12, 137.0, 105.0, 1.0), 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA3), 50, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1505", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0x12), 300, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1506", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 110.0, g_view->GetHeight() - 120.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 120.0, 103.0, 1.0), 0, 1);
	PushMacro(MACRO_WAIT, 0, 100, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro((eMacroType)28, "0", 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA2), 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA3), 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 0", 0x88), -1, 1);
	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", 1, 1);

	PushMacro((eMacroType)23, "0", 0, 1);
	PushMacro((eMacroType)2, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", 300, 1);

	PushMacro((eMacroType)3, 0, -1, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 0", 0x87), 100, 1);

	PushMacro((eMacroType)22, MakeStr("Spin %d %d", 0x169, 3), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Spin %d %d %d %d", 0x165, 1, 0xFFFFFFF9, 0), 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "1507", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0xF, m_tipRect.x + m_tipRect.w - 128.0f, m_tipRect.y + m_tipRect.h + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0xF, 137.0, 105.0, 1.0), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 5.0, g_view->GetHeight() - 105.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 103.0, 103.0, 0.5), 0, 1);
	PushMacro(MACRO_WAIT, 0, 1000, 1);

	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0xF), 300, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro((eMacroType)28, "310", 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d 1 %d %d ", 0x87, 0x1EA, 0x8C), -1, 1);

	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", -1, 1);

	PushMacro((eMacroType)27, 0, 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA3), 0, 1);

	PushMacro((eMacroType)14, "2 ok", 0, 1); PushMacro((eMacroType)9, "1508", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)2, "\xC1\xC2", 0, 1);

	PushMacro((eMacroType)28, "142", 0, 1);
	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA2), 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA3), 0, 1);

	PushMacro((eMacroType)27, 0, 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 1", 0x88), 0, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 110.0, g_view->GetHeight() - 115.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 123.0, 45.0, 1.0), 0, 1);
	PushMacro(MACRO_WAIT, 0, 300, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0x13, m_tipRect.x + m_tipRect.w - 128.0f, m_tipRect.y + m_tipRect.h + 20.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0x13, 137.0, 105.0, 1.0), 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA3), 50, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1509", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0x13), 300, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1510", -1, 1);

	PushMacro((eMacroType)19, MakeStr("%d %.1f %.1f", 0, 110.0, g_view->GetHeight() - 120.0f), 0, 1);
	PushMacro((eMacroType)16, MakeStr("%d %.1f %.1f %.1f", 0, 120.0, 103.0, 1.0), 0, 1);
	PushMacro(MACRO_WAIT, 0, 100, 1);
	PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)18, MakeStr("%d", 0), 300, 1);

	PushMacro((eMacroType)28, "0", 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA2), 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d", 0xA3), 0, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 0", 0x88), -1, 1);
	PushMacro((eMacroType)29, 0, 0, 1);
	PushMacro((eMacroType)1, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", 1, 1);

	PushMacro((eMacroType)23, "0", 0, 1);
	PushMacro((eMacroType)2, "\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", 300, 1);

	PushMacro((eMacroType)3, 0, -1, 1);

	PushMacro((eMacroType)22, MakeStr("GolfRule %d 0", 0x87), 1000, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1511", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);

	PushMacro((eMacroType)22, MakeStr("Spin %d %d", 0x169, 4), 0, 1);

	PushMacro((eMacroType)3, 0, 0, 1);
	PushMacro(MACRO_WAIT, 0, 500, 1);

	PushMacro((eMacroType)22, MakeStr("Player %d %d %d %d %d", 0x157, -1, 0x1C, 7), 0, 1);
	PushMacro((eMacroType)22, MakeStr("Screen %d %d", 0x208, 1), 0, 1);
	PushMacro((eMacroType)8, 0, 20000, 1);

	PushMacro(MACRO_WAIT, 0, 2000, 1);

	PushMacro((eMacroType)14, "3 ok", 0, 1); PushMacro((eMacroType)9, "1512", -1, 1); PushMacro((eMacroType)15, 0, -1, 1);
	PushMacro((eMacroType)26, 0, -1, 1);
}

// tutorial.cpp:4057
void CTutorial::InitTutorialMode(int mode) {
	char startText[0x20];
	char missionText[0x20];

	ClearMacro();

	GetPVS().ResetHoleData();

	m_dialogType = 1; m_dialogState = 0;

	m_keepMouseBlocked = false;
	m_fakeKeyDown = false;

	m_step = 7;
	m_specialWait = -1;
	m_fakeKey[0] = 0;

	m_macroHold = false;
	m_failCount = 0;
	m_targetHits = 0;
	startText[0] = 0;
	missionText[0] = 0;
	GOLFDOC()->GetPlayer(0)->m_ghostAward = 0;
	m_pulseScale = 1.5f;
	GOLFDOC()->GetPlayer(0)->m_powerShotLevel = 0;
	sPlayerData *player = GOLFDOC()->GetPlayer(0); player->m_holes[GOLFDOC()->m_currentHole - 1] = 1;
	GOLFDOC()->m_target.m_unk10 = false;

	memset(m_missionItems, 0, sizeof(m_missionItems));
	GOLFDOC()->GetPlayer(0)->m_numItems = 0;
	GOLFDOC()->GetPlayer(0)->m_shotItemTypeId = 0;
	GOLFDOC()->GetPlayer(0)->m_comboGauge = 0.0f;

	SendMsg(this, "Screen", 0x208, 0, 0, 0, 0);
	PriviewOverlay();

	GOLFDOC()->m_gameState = mode;

	switch (GOLFDOC()->m_gameState) {
	case 0:
		m_tipState = 0;
		strcpy(startText, "\xbd\xc3\xc0\xdb\xc1\xa1");
		PushMacroCamera();
		break;

	case 1:
		strcpy(startText, "\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1" "2");
		strcpy(missionText, "\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1" "1_6");
		PushMacroClub();
		break;

	case 2:
		strcpy(startText, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(missionText, "\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1" "3");
		PushMacroShot();
		break;

	case 6:
		strcpy(startText, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(missionText, "\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1" "3");
		PushMacroGround();
		break;

	case 3:
		strcpy(startText, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(missionText, "\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1" "3");
		PushMacroGauge();
		break;

	case 4:
		strcpy(startText, "\xbd\xc3\xc0\xdb\xc1\xa1" "_\xb1\xd7\xb8\xb0");
		PushMacroGreen();
		break;

	case 5:
		strcpy(startText, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(missionText, "\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1" "3");
		PushMacroItemUse();
		break;

	case 7:
		strcpy(startText, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(missionText, "\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1" "3");
		PushMacroScore();
		break;

	case 8:
		strcpy(startText, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(missionText, "\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1" "3");
		PushMacroHoleIn();
		break;

	case 11:
		strcpy(startText, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(missionText, "\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1" "3");
		Wind().Init(7, 0xCC, 0, 1);
		Wind().SetIntensity(0.0f);
		Wind().m_windEnabled = true;
		PushMacroWindReading();
		break;

	case 9:
		strcpy(startText, "\xbd\xc3\xc0\xdb\xc1\xa1" "_Unplayable");
		strcpy(missionText, "\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1" "3");
		PushMacroBallMove();
		break;

	case 10:
		strcpy(startText, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(missionText, "\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1" "3");
		PushMacroBallSlope();
		break;

	case 12:
		strcpy(startText, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(missionText, "\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1" "3");
		PushMacroHitPoint();
		break;

	case 13:
		strcpy(startText, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(missionText, "\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1" "3");
		PushMacroPowerSpin();
		break;

	case 14:
		strcpy(startText, "\xbd\xc3\xc0\xdb\xc1\xa1");
		strcpy(missionText, "\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1" "_curve");
		PushMacroPowerCurve();
		break;
	}

	BlockInput();
	GOLFDOC()->m_target.m_unk10 = false;
	WSingleton<CMouseCursor>::Instance()->SetActive(false);

	SendMsg(this, "Screen", 0x1BA, 0, 0, 0, 0);
	SendMsg(this, "Screen", 0x1BB, m_tutorialIndex + 1, 0, 0, 0);

	SendMsg(this, "GolfRule", 0x89, (intptr_t)startText, missionText[0] != 0 ? (intptr_t)missionText : 0, 0, 0);

	WSingleton<CChatMsg>::Instance()->ClearHistory();
}

// tutorial.cpp:4249
bool CTutorial::CheckCameraTarget(void) {
	WVector diff = GOLFDOC()->m_target.m_pos - data_b7bf88.pivot;
	diff.y += 6.0f;
	float dist = data_b7bf88.za.x * diff.x + data_b7bf88.za.y * diff.y + data_b7bf88.za.z * diff.z;

	if (WAbsBySign(dist) < data_9f67e4) {
		return false;
	}

	if (dist > 96.0f && dist < 288.0f && Abs(acos(Between(-1.0f, dist / diff.Magnitude(), 1.0f))) < 0.05236f) {
		m_tipState++;
		PopMacro();
	}

	return false;
}

// tutorial.cpp:4272
bool CTutorial::CheckClubTarget(void) {
	const WVector &dir = GOLFDOC()->m_cameraData.m_matrix.za;
	WVector reach = dir * (GolfClub().GetRange(0) * 3.2f);
	WVector diff = GOLFDOC()->m_target.m_pos - (GolfBall().m_pos + reach);
	diff.y = 0.0f;

	if (diff.Magnitude() < 16.0f) {
		return true;
	}
	return false;
}

// tutorial.cpp:4285
WVector *CTutorial::GetPoint(const char *name) {
	return &GOLFDOC()->m_pPolySoup->FindPoint(name)->m_pos;
}

// tutorial.cpp:4294
void CTutorial::PushMacro(eMacroType type, const char *text, int param, int count) {
	sMacro macro;
	macro.m_type = type;
	macro.m_param = param;
	if (text == 0) {
		macro.m_text[0] = 0;
	} else {
		strcpy(macro.m_text, text);
	}

	for (int i = 0; i < count; i++) {
		m_macros.push(macro);
	}
}

// tutorial.cpp:4314
void CTutorial::PopSpecial(void) {
	if (!m_macros.empty()) {
		if (m_macros.front().m_type == MACRO_SPECIAL) {
			m_step = 7;
			m_specialWait = m_macros.front().m_param;

			m_macros.pop();

			if (!m_macros.empty()) {
				if (m_specialWait == 0) {
					PopMacro();
				}
			}
		}
	}
}

// tutorial.cpp:4353
void CTutorial::PopMacro(void) {
	if (m_macros.empty() || m_yesNoForm != 0) {
		return;
	}

	m_step = 7;
	m_specialWait = m_macros.front().m_param;

	if (m_macros.front().m_type == MACRO_SPECIAL) {
		return;
	}

	char target[32];
	switch (m_macros.front().m_type) {
	case 1:
		g_input->SetFakeInput(m_macros.front().m_text, m_fakeKeyDown);
		m_fakeKeyDown = false;
		break;

	case 2:
		strcpy(m_fakeKey, m_macros.front().m_text);
		g_input->SetFakeInput(m_fakeKey, m_fakeKeyDown);
		m_fakeKeyDown = false;
		break;

	case 3:
		m_fakeKey[0] = 0;
		break;

	case 4: {
		int button;
		sscanf(m_macros.front().m_text, "%d", &button);
		g_input->SetFakeInput((eButton)button);
		break;
	}

	case 5: {
		float x;
		float y;
		sscanf(m_macros.front().m_text, "%f %f", &x, &y);
		WSingleton<CMouseCursor>::Instance()->ForceMove(x, y, m_specialWait * 0.001f);
		break;
	}

	case 6: {
		float x;
		float y;
		sscanf(m_macros.front().m_text, "%f %f", &x, &y);
		WSingleton<CMouseCursor>::Instance()->ForceMove(x, y, -1.0f);
		break;
	}

	case 7: {
		char name[32] = "";
		sscanf(m_macros.front().m_text, "%s", name);

		FrWnd *wnd = 0;
		SendMsg(this, "GolfRule", 0x24A, (intptr_t)name, (intptr_t)&wnd, 0, 0);

		if (wnd) {
			WSingleton<CMouseCursor>::Instance()->ForceMove(wnd->GetRect().x + 5.0f, wnd->GetRect().y + 5.0f, -1.0f);
		}
		break;
	}

	case 8:
		if (m_specialWait > 0) {
			m_macroHold = true;
			m_macroTimer = m_specialWait + 4000;
			m_specialWait = -1;
		}

		m_step = 1;
		SendMsg(this, "Screen", 0x1BA, 1, 0, 0, 0);

		PrepareMission();
		OpenMissionForm();
		break;

	case 9: {
		m_step = 0;

		WSingleton<CMouseCursor>::Instance()->SetActive(true);

		int textKey;
		sscanf(m_macros.front().m_text, "%d", &textKey);

		if (IsLocalContent((localContentType_t)0x38)) {
			m_specialWait = 0;
			OpenTipCaddieForm(m_texts[textKey].c_str(), (FRESH_RESULT_CALLBACK)&CTutorial::OnTipCaddieFormResult);
			break;
		}

		m_tipForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "tip", 0);
		m_tipForm->SetMessage(m_texts[textKey].c_str(), false);
		m_tipForm->EnableDrag(false);
		m_tipForm->Open((FRESH_RESULT_CALLBACK)&CTutorial::OnTipFormResult,
			WPoint((g_view->GetWidth() - m_tipForm->GetRect().w) * 0.5f, 80.0f), 6);
		m_tipForm->MoveCursor("ok");

		FrWnd *harrow = m_tipForm->FindChildByName("harrow");
		if (harrow) {
			harrow->SetVisible(false);
		}

		FrWnd *varrow = m_tipForm->FindChildByName("varrow");
		if (varrow) {
			varrow->SetVisible(false);
		}
		break;
	}

	case 10: {
		m_step = 0;

		WSingleton<CMouseCursor>::Instance()->SetActive(true);

		std::string unused;
		int textKey;
		float x;
		float y;
		float arrowX;
		float arrowY;
		sscanf(m_macros.front().m_text, "%d %f %f %f %f", &textKey, &x, &y, &arrowX, &arrowY);

		m_specialWait = 0;
		OpenTipCaddieForm(m_texts[textKey].c_str(), (FRESH_RESULT_CALLBACK)&CTutorial::OnTipCaddieFormResult);
		break;
	}

	case 11: {
		m_step = 0;

		WSingleton<CMouseCursor>::Instance()->SetActive(true);

		int textKey;
		float x;
		float y;
		float arrowX;
		float arrowY;
		sscanf(m_macros.front().m_text, "%d %f %f %f %f", &textKey, &x, &y, &arrowX, &arrowY);

		m_tipForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "tip", 0);
		m_tipForm->SetMessage(m_texts[textKey].c_str(), false);
		m_tipForm->EnableDrag(false);
		m_tipForm->Open((FRESH_RESULT_CALLBACK)&CTutorial::OnTipFormResult, WPoint(x, y), 6);
		m_tipForm->MoveCursor("ok");

		FrWnd *harrow = m_tipForm->FindChildByName("harrow");
		if (harrow) {
			harrow->SetClientRect(WRect(arrowX, arrowY, 19.0f, 14.0f));
		}

		FrWnd *varrow = m_tipForm->FindChildByName("varrow");
		if (varrow) {
			varrow->SetVisible(false);
		}
		break;
	}

	case 12:
		m_step = 0;

		WSingleton<CMouseCursor>::Instance()->SetActive(true);
		break;

	case 13: {
		m_step = 0;

		WSingleton<CMouseCursor>::Instance()->SetActive(true);

		int textKey;
		float w;
		float h;
		int layout;
		sscanf(m_macros.front().m_text, "%d %f %f %d", &textKey, &w, &h, &layout);

		m_tipForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "tip_img", 0);
		m_tipForm->EnableDrag(false);
		m_tipForm->SetRect(WRect((g_view->GetWidth() - w) * 0.5f, 200.0f, w, h));

		FrWnd *ok = m_tipForm->FindChildByName("ok");
		if (ok) {
			WRect rc;
			m_tipForm->GetClientRect(rc);
			ok->MoveWindow(WPoint(rc.x + rc.w - 51.0f, rc.y + rc.h - 25.0f));
		}

		switch (layout) {
		case 0: {
			FrWnd *bong = m_tipForm->FindChildByName("bong");
			if (bong) {
				bong->SetVisible(false);
			}

			FrWnd *message = m_tipForm->FindChildByName("message");
			if (message) {
				message->SetClientRect(WRect(10.0f, 30.0f, 273.0f, 102.0f));
			}
			break;
		}

		case 1: {
			FrWnd *score = m_tipForm->FindChildByName("score");
			if (score) {
				score->SetVisible(false);
			}

			FrWnd *message = m_tipForm->FindChildByName("message");
			if (message) {
				message->SetClientRect(WRect(65.0f, 55.0f, 218.0f, 80.0f));
			}
			break;
		}
		}

		m_tipForm->SetMessage(m_texts[textKey].c_str(), false);
		m_tipForm->Open((FRESH_RESULT_CALLBACK)&CTutorial::OnTipFormResult, 6);
		m_tipForm->MoveCursor("ok");
		break;
	}

	case 14: {
		char buttonName[16];
		sscanf(m_macros.front().m_text, "%d %s", &m_dialogType, buttonName);

		if ((unsigned int)m_dialogType >= 9) {
			m_dialogType = 1;
		}

		m_dialogState = 0;
		for (unsigned int i = 0; i < 3; i++) {
			if (stricmp(data_9f64b0[i], buttonName) == 0) {
				m_dialogState = i;
				break;
			}
		}
		break;
	}

	case 15:
		if (m_tipCaddieForm2 && m_tipCaddieForm) {
			m_tipCaddieForm2->Enable(true);
			m_tipCaddieForm->EnableFlag(2);

			if (m_keepMouseBlocked) {
				g_input->FreeMouse(LEFT_BUTTON);
			}
		}
		break;

	case 16: {
		int index = 0;
		float w;
		float h;
		float frameTime;
		sscanf(m_macros.front().m_text, "%d %f %f %f", &index, &w, &h, &frameTime);

		m_tutorialOverlays[index].m_src.w = w;
		m_tutorialOverlays[index].m_src.h = h;
		m_tutorialOverlays[index].m_frameTime = frameTime;

		AnimatedOverlay &overlay = m_tutorialOverlays[index];
		overlay.m_active = true;
		overlay.m_loop = true;
		overlay.m_frame = 0;
		overlay.m_elapsed = 0;
		overlay.m_alpha = 255.0f;
		overlay.m_visible = true;
		overlay.m_fadeDir = 1;
		break;
	}

	case 17: {
		int index = 0;
		float w;
		float h;
		float frameTime;
		sscanf(m_macros.front().m_text, "%d %f %f %f", &index, &w, &h, &frameTime);

		m_tutorialOverlays[index].m_src.w = w;
		m_tutorialOverlays[index].m_src.h = h;
		m_tutorialOverlays[index].m_frameTime = frameTime;

		m_tutorialOverlays[index].m_src.x = m_tipPos[0] - w * 0.5f;
		m_tutorialOverlays[index].m_src.y = m_tipPos[1] - h * 0.5f;

		AnimatedOverlay &overlay = m_tutorialOverlays[index];
		overlay.m_active = true;
		overlay.m_loop = true;
		overlay.m_frame = 0;
		overlay.m_elapsed = 0;
		overlay.m_alpha = 255.0f;
		overlay.m_visible = true;
		overlay.m_fadeDir = 1;
		break;
	}

	case 18: {
		int index = 0;
		sscanf(m_macros.front().m_text, "%d", &index);

		AnimatedOverlay &overlay = m_tutorialOverlays[index];
		overlay.m_active = false;
		overlay.m_loop = true;
		overlay.m_fadeDir = -1;
		break;
	}

	case 19: {
		int index = 0;
		float x;
		float y;
		sscanf(m_macros.front().m_text, "%d %f %f", &index, &x, &y);

		m_tutorialOverlays[index].m_src.x = x;
		m_tutorialOverlays[index].m_src.y = y;
		break;
	}

	case 20:
		m_step = 4;
		sscanf(m_macros.front().m_text, "%d", &m_quizIndex);
		break;

	case 26:
		MoveNextStep();
		break;

	case 21: {
		m_step = 1;
		GOLFDOC()->m_target.m_unk10 = true;
		WPolySoup::sPoint *point = GOLFDOC()->m_pPolySoup->FindPoint(m_macros.front().m_text);
		GOLFDOC()->m_target.m_pos = GetPVS().GetGroundPoint(point->m_pos, false, 0, true);
		break;
	}

	case 22: {
		int msg;
		sMacroParam param[3] = { 0, 0, 0 };
		sscanf(m_macros.front().m_text, "%s %d " PG_MACRO_PTR " " PG_MACRO_PTR " " PG_MACRO_PTR, target, &msg, &param[0], &param[1], &param[2]);
		SendMsg(this, target, msg, (intptr_t)param[0], (intptr_t)param[1], (intptr_t)param[2], 0);
		break;
	}

	case 23: {
		int active;
		sscanf(m_macros.front().m_text, "%d", &active);
		WSingleton<CMouseCursor>::Instance()->SetActive(active != 0);
		break;
	}

	case 24: {
		int blockMouse;
		sscanf(m_macros.front().m_text, "%d", &blockMouse);

		if (blockMouse) {
			g_input->BlockMouse(LEFT_BUTTON);
			g_input->BlockMouse();
		}

		BlockInput();
		break;
	}

	case 25:
		FreeInput();
		break;

	case 30: {
		int button;
		sscanf(m_macros.front().m_text, "%d", &button);

		switch (button) {
		case 0:
			g_input->BlockMouse();
			break;

		case 1:
			g_input->BlockMouse(LEFT_BUTTON);
			break;

		case 2:
			g_input->BlockMouse(RIGHT_BUTTON);
			break;
		}
		break;
	}

	case 31:
		g_input->FreeMouse();
		break;

	case 28:
		sscanf(m_macros.front().m_text, "%d", &GOLFDOC()->m_value15C);
		break;

	case 29:
		m_fakeKeyDown = true;
		break;
	}

	m_macros.pop();

	if (GOLFDOC()->m_gameState != 7 || !m_macros.empty()) {
		if (m_step == 0) {
			BlockInput();
		} else if (m_step == 1) {
			FreeInput();
		} else if (m_step == 5) {
			BlockInput();
		}

		if (m_specialWait == 0) {
			PopMacro();
		}
	}
}

// tutorial.cpp:4865
void CTutorial::ClearMacro(void) {
	while (!m_macros.empty()) {
		m_macros.pop();
	}
}
