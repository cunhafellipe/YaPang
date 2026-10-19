// Original: d:\build\custom\temp\code\client\projectg\letter_effect.cpp
// actor.h names IActor in MsgObject before declaring it.
class IActor;

#include "letter_effect.h"

#include <new>
#include <string.h>

#include "ObjectFactory.h"
#include "CapturedBg.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "InputManager.h"
#include "wfont.h"
#include "wview.h"
#include "headicon.h"
#include "commonutil.h"
#include "contentsdoc.h"
#include "shared/Localize.h"

extern unsigned long g_CurrentTime;

// Replay sign blink state (DisplayReplay).
static bool s_replayVisible;
static bool s_replayBlink;

// letter_effect.cpp:23 (run-time type and factory registration of CLetter)
const WRTTI CLetter::m_RTTI("CLetter", &IActor::m_RTTI);

IObject* CLetterMakeInstance(void) {
	return new CLetter;
}

struct __impCLetter {
	__impCLetter(void) {
		ObjectFactory().AddObjectFunctor(CLetterMakeInstance, "CLetter");
		ActorFactory().AddActorClass("CLetter");
	}
};
static __impCLetter s_impCLetter;

// letter_effect.cpp:27
CLetter::CLetter(void) : m_pCurrent(NULL), m_textList(0x20, 0x10) {
	m_pFont = NULL; m_pReplayText = NULL; m_pReplay = NULL; m_priority = 4; AddSkipList(0x2000);
}

// letter_effect.cpp:34
void CLetter::OnPreLoadInit(void) {
	m_fadeTime = 0.5f;
	m_show = true;
	m_time = 0.0f;
}

// letter_effect.cpp:114
void CLetter::OnInit(void) {
	m_state = 0;
	m_align = 2;
}

// letter_effect.cpp:217
void CLetter::OnDestroy(void) {
	if (g_resrcmng != NULL && m_pFont != NULL) { g_resrcmng->Release(m_pFont); m_pFont = NULL; }
	for (sText* p = m_textList.Start(); p != NULL; p = m_textList.Next()) { delete p; } m_textList.Reset();
}

// letter_effect.cpp:223
void CLetter::OnLoad(void) {
	if (WSingleton<CSharedDoc>::Instance()->GetPeriodDoc()->IsUsableContents(3)) {
		AddOverlay("\xc6\xce\xbe\xdf""00", "[text_pangya32.jpg", NULL, false);
	} else if (IsLocalContent((localContentType_t)0x65)) {
		AddOverlay("\xc6\xce\xbe\xdf""00", "[text_pangya12.jpg", NULL, false);
	} else if (IsLocalContent((localContentType_t)0x7e)) {
		AddOverlay("\xc6\xce\xbe\xdf""00", "[text_pangya17.jpg", NULL, false);
	} else if (IsLocalContent((localContentType_t)0x8b)) {
		AddOverlay("\xc6\xce\xbe\xdf""00", "[text_pangya18.jpg", NULL, false);
	} else if (IsLocalContent((localContentType_t)0x93)) {
		AddOverlay("\xc6\xce\xbe\xdf""00", "[text_pangya22.jpg", NULL, false);
	} else {
		AddOverlay("\xc6\xce\xbe\xdf""00", "[text_pangya00.jpg", NULL, false);
	}

	AddOverlay("\xc6\xce\xbe\xdf""01", "[text_pangya01.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""02", "[text_pangya02.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""03", "[text_pangya03.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""04", "[text_pangya04.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""06", "[text_pangya06.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""08", "[text_pangya08.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""11", "[text_pangya11.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""13", "[text_pangya13.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""16", "[text_pangya16.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""19", "[text_pangya19.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""20", "[text_pangya20.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""21", "[text_pangya21.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""23", "[text_pangya23.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""26", "[text_pangya26.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""27", "[text_pangya27.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""28", "[text_pangya28.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""29", "[text_pangya29.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""31", "[text_pangya31.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""58", "[text_pangya58.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""67", "[text_pangya67.jpg", NULL, false);
	AddOverlay("\xc6\xce\xbe\xdf""68", "[text_pangya68.jpg", NULL, false);
	AddOverlay("OB", "[ob.jpg", NULL, false);

	unsigned char roomType = WSingleton<CSharedDoc>::Instance()->m_roomType;
	if (roomType == 1 || roomType == 3) {
		AddOverlay("win", "[hole_end.jpg", &WRect(0.0f, 384.0f, 256.0f, 64.0f), false);
		AddOverlay("lose", "[hole_end.jpg", &WRect(256.0f, 384.0f, 256.0f, 64.0f), false);
		AddOverlay("draw", "[hole_end.jpg", &WRect(0.0f, 448.0f, 256.0f, 64.0f), false);
	} else {
		AddOverlay("\xbe\xcb\xb9\xd9\xc6\xae\xb7\xce\xbd\xba", "[hole_end.jpg", &WRect(0.0f, 0.0f, 512.0f, 64.0f), false);
	}
	AddOverlay("\xc0\xcc\xb1\xdb", "[hole_end.jpg", &WRect(0.0f, 64.0f, 256.0f, 64.0f), false);
	AddOverlay("\xb9\xf6\xb5\xf0", "[hole_end.jpg", &WRect(256.0f, 64.0f, 256.0f, 64.0f), false);
	AddOverlay("\xc6\xc4", "[hole_end.jpg", &WRect(0.0f, 128.0f, 256.0f, 64.0f), false);
	AddOverlay("\xba\xb8\xb1\xe2", "[hole_end.jpg", &WRect(256.0f, 128.0f, 256.0f, 64.0f), false);
	AddOverlay("\xb4\xf5\xba\xed\xba\xb8\xb1\xe2", "[hole_end.jpg", &WRect(0.0f, 192.0f, 512.0f, 64.0f), false);
	AddOverlay("\xc6\xae\xb8\xae\xc7\xc3\xba\xb8\xb1\xe2", "[hole_end.jpg", &WRect(0.0f, 256.0f, 512.0f, 64.0f), false);
	AddOverlay("+4", "[hole_end.jpg", &WRect(0.0f, 320.0f, 64.0f, 64.0f), false);
	AddOverlay("+5", "[hole_end.jpg", &WRect(64.0f, 320.0f, 64.0f, 64.0f), false);
	AddOverlay("+6", "[hole_end.jpg", &WRect(128.0f, 320.0f, 64.0f, 64.0f), false);
	AddOverlay("+7", "[hole_end.jpg", &WRect(192.0f, 320.0f, 64.0f, 64.0f), false);
	AddOverlay("+8", "[hole_end.jpg", &WRect(256.0f, 320.0f, 64.0f, 64.0f), false);
	AddOverlay("+9", "[hole_end.jpg", &WRect(320.0f, 320.0f, 64.0f, 64.0f), false);
	AddOverlay("+10", "[hole_end.jpg", &WRect(384.0f, 320.0f, 128.0f, 64.0f), false);

	AddOverlay("\xbf\xf6\xc5\xcd\xc7\xd8\xc0\xfa\xb5\xe5", "[shot_end.jpg", &WRect(0.0f, 0.0f, 512.0f, 64.0f), false);
	AddOverlay("\xc6\xf7\xb1\xe2", "[shot_end.jpg", &WRect(0.0f, 64.0f, 256.0f, 64.0f), false);
	AddOverlay("\xc8\xa6\xc0\xce\xbf\xf8", "[text_holeinone.jpg", NULL, false);
	AddOverlay("\xc5\xb8\xc0\xd3\xbe\xc6\xbf\xf4", "[timeout.jpg", NULL, false);

	AddOverlay("\xb9\xf6\xb5\xf0\xc2\xf9\xbd\xba", "[birdie_shot.jpg", NULL, false);
	AddOverlay("\xc0\xcc\xb1\xdb\xc2\xf9\xbd\xba", "[eagle_shot.jpg", NULL, false);

	AddOverlay("winningshot", "[for_win1.jpg", NULL, false);
	AddOverlay("matchshot", "[for_win2.jpg", NULL, false);

	AddOverlay("winningshot2", "[for_win1_2.jpg", NULL, false);
	AddOverlay("matchshot2", "[for_win2_2.jpg", NULL, false);

	AddOverlay("winningshot3", "[for_win1_3.jpg", NULL, false);
	AddOverlay("matchshot3", "[for_win2_2.jpg", NULL, false);

	AddOverlay("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1""30", "[approach30.jpg", NULL, true);
	AddOverlay("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1""60", "[approach60.jpg", NULL, true);

	AddOverlay("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1""30_\xbf\xac\xc0\xe5", "[approach30_extra.jpg", NULL, true);
	SetPos("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1""30_\xbf\xac\xc0\xe5", g_view->GetWidth() * 0.5f, g_view->GetHeight() * 0.25f + 64.0f);

	AddOverlay("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1""60_\xbf\xac\xc0\xe5", "[approach60_extra.jpg", NULL, true);
	SetPos("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1""60_\xbf\xac\xc0\xe5", g_view->GetWidth() * 0.5f, g_view->GetHeight() * 0.25f + 64.0f);

	AddOverlay("OneShot", "[oneshot.jpg", NULL, true);
	SetPos("OneShot", g_view->GetWidth() * 0.5f, g_view->GetHeight() * 0.25f + 42.0f);

	AddOverlay("\xc2\xf7\xb7\xca\xc7\xa5\xbd\xc3", "[play.jpg", NULL, true);
	AddOverlay("NoPlay", "[noplay.jpg", NULL, false);
	AddOverlay("Retry", "[noplay2.jpg", NULL, false);

	AddOverlay("Wait", "[wait.jpg", NULL, false);
	SetPos("Wait", g_view->GetWidth() * 0.5f, g_view->GetHeight() * 0.18f);

	AddOverlay("AI", "[ai.jpg", NULL, false);
	SetPos("AI", g_view->GetWidth() * 0.5f, g_view->GetHeight() * 0.18f);

	AddOverlay("Replay", "[replay-icon.jpg", NULL, false);
	SetPos("Replay", 45.0f, 60.0f);

	AddOverlay("FastForward", "[forward-icon.jpg", NULL, false);
	SetPos("FastForward", 45.0f, 60.0f);

	AddOverlay("Demo", "[demo.png", NULL, false);
	SetPos("Demo", g_view->GetWidth() * 0.5f, g_view->GetHeight() * 0.18f);

	m_pFont = g_resrcmng->GetTitleFont();

	char* fontFile = "[bigfont.jpg";
	m_fontInfo.filename = &fontFile;
	m_fontInfo.flag = 0;
	m_fontInfo.fontw = m_fontInfo.fonth = 32;
	m_fontInfo.numPages = 1;
	m_fontInfo.texw = 256;
	m_fontInfo.texh = 128;
	m_fontInfo.pCharSet = "1234567890ym%-./x ";
	m_pFont->Create(&m_fontInfo);
}

// letter_effect.cpp:412
void CLetter::AddOverlay(const char* name, const char* file, WRect* rect, bool skippable) {
	sText* p = new sText;

	p->m_pOverlay = g_resrcmng->GetOverlay(file, 0);

	if (p->m_pOverlay == NULL) {
		delete p;
		return;
	}

	strcpy(p->m_name, name);
	p->m_time = 0.0f;

	float w = (float)p->m_pOverlay->GetWidth();
	float h = (float)p->m_pOverlay->GetHeight();

	if (rect != NULL) {
		p->m_uv.x = rect->x * (1.0f / w);
		p->m_uv.y = rect->y * (1.0f / h);
		p->m_uv.w = rect->w * (1.0f / w);
		p->m_uv.h = rect->h * (1.0f / h);
	} else {
		p->m_uv = WRect(0.0f, 0.0f, 1.0f, 1.0f);
	}

	p->m_rect.w = w * p->m_uv.w;
	p->m_rect.h = h * p->m_uv.h;
	p->m_rect.x = (float)(int)(g_view->GetWidth() * 0.5f - p->m_rect.w * 0.5f);
	p->m_rect.y = (float)(int)(g_view->GetHeight() * 0.25f - p->m_rect.h * 0.5f);
	p->m_skippable = skippable;
	p->m_score.m_show = false;

	m_textList.AddItem(p, p->m_name, false);
}

// letter_effect.cpp:456
void CLetter::SetPos(const char* name, float x, float y) {
	sText* p = m_textList.Find(name);

	if (p != NULL) {


		p->m_rect.x = (float)(int)(x - p->m_rect.w * 0.5f);
		p->m_rect.y = (float)(int)(y - p->m_rect.h * 0.5f);
	}
}

// letter_effect.cpp:467
void CLetter::OnProcess(float elapsed) {
	m_time += elapsed;

	if (m_pCurrent == NULL || WSingleton<CCapturedBg>::Instance()->IsCaptured()) {
		return;
	}
	if (GOLFDOC()->GameMode() & 0x8210) {
		m_state = 0;
	}
	switch (m_state) {
	case 1:
		m_alpha += elapsed / m_fadeTime * 255.0f;
		if (m_alpha > 255.0f) {
			m_state = 2;
		}
		break;
	case 3:
		m_alpha -= elapsed / m_fadeTime * 255.0f;
		if (m_alpha < 0.0f) {
			m_alpha = 0.0f;
			m_state = 0;
		}
		break;
	case 2:
		if (m_pCurrent->m_time > 0.0f) {
			m_pCurrent->m_time -= elapsed;
			if (m_pCurrent->m_time < 0.0f) {
				m_state = 3;
			}
		}
		if (m_pCurrent->m_skippable && g_input->GetList()) {
			m_state = 3;
		}
		break;
	}
	float alpha = m_alpha; m_alpha = alpha < 0.0f ? 0.0f : alpha > 255.0f ? 255.0f : alpha;
}

// letter_effect.cpp:510
void CLetter::DisplayReplay(void) {
	if (!m_show) {
		return;
	}
	if (m_time > 1.0f) {
		s_replayBlink ^= true;
		m_time = 0.5f;
		s_replayVisible = true;
	}
	if (s_replayBlink) {
		if (s_replayVisible) {
			if (m_pReplay != NULL) {
				m_pReplay->m_pOverlay->Render(g_view, m_pReplay->m_uv, m_pReplay->m_rect, 0, 0xffffffff, 0.0f, 0);
			}
		}
	} else {
		if (s_replayVisible) {
			s_replayVisible = false;
		}
	}
}

// letter_effect.cpp:542
void CLetter::OnDisplay(void) {
	if (WSingleton<CCapturedBg>::Instance()->IsCaptured()) {
		return;
	}
	if (WSingleton<CHeadIcon>::Instance() != NULL) {
		WSingleton<CHeadIcon>::Instance()->DisplayOverPlayer();
	}
	DisplayReplay();

	if (m_pReplayText != NULL) {
		if (m_pReplay != NULL) {
			m_pReplayText->m_rect.x = 88.0f;
		} else {
			m_pReplayText->m_rect.x = 13.0f;
		}
		m_pReplayText->m_pOverlay->Render(g_view, m_pReplayText->m_uv, m_pReplayText->m_rect, 0, 0xffffffff, 0.0f, 0);
	}

	if (m_show && m_state != 0 && m_pCurrent != NULL && !(GOLFDOC()->GameMode() & 0x20400)) {
		m_pCurrent->m_pOverlay->Render(g_view, m_pCurrent->m_uv, m_pCurrent->m_rect, 0, ((int)m_alpha << 24) + 0xffffff, 0.0f, 0);

		if (m_pCurrent->m_score.m_show) {
			float h = g_view->GetHeight();
			float w = g_view->GetWidth();
			Print(w * 0.5f + 5.0f, h * 0.25f + 5.0f, MakeStr("%d", m_pCurrent->m_score.m_value), 3.0f, 0, m_pCurrent->m_score.m_scale, m_pCurrent->m_score.m_color);
		}
	}
}

// letter_effect.cpp:586
void CLetter::HandleMsg(const MsgObject& msg) {
	if ((unsigned long)msg.m_reserved > g_CurrentTime) {
		StoreDelayedMsg(msg);
		return;
	}

	switch (msg.m_msg) {
	case 0x192:
		m_pReplay = m_textList.Find("Replay");
		m_time = 0.5f;
		break;

	case 0x193:
		m_pReplayText = m_textList.Find("FastForward");
		break;

	case 0x18c:
		if (m_pCurrent != NULL && (const char*)msg.m_param1 != NULL) {
			if (strstr(m_pCurrent->m_name, (const char*)msg.m_param1) != NULL) {
				m_pCurrent = NULL;
			}
		} else {
			m_pCurrent = NULL;
			m_pReplay = NULL;
			m_pReplayText = NULL;
		}
		break;

	case 0x18f:
		m_state = 3;
		break;

	case 0x18d:
		m_pCurrent = m_textList.Find((const char*)msg.m_param1);
		if (m_pCurrent == NULL) {
			break;
		}
		if ((float*)msg.m_param2 != NULL) {
			m_pCurrent->m_time = *(float*)msg.m_param2;
		} else {
			m_pCurrent->m_time = 0.0f;
		}
		if (msg.m_param3 != 0) {
			m_alpha = 255.0f;
			m_state = 2;
		} else {
			m_alpha = 0.0f;
			m_state = 1;
		}
		break;

	case 0x18e:
		m_state = 1;
		break;

	case 0x191:
		m_pCurrent = m_textList.Find((const char*)msg.m_param1);
		if (m_pCurrent == NULL) {
			break;
		}
		m_pCurrent->m_score = *(sScore*)msg.m_param2;
		m_pCurrent->m_score.m_show = true;
		break;

	case 0x271b: {
		for (sText* p = m_textList.Start(); p != NULL; p = m_textList.Next()) {
			p->m_rect.x = (float)(int)(g_view->GetWidth() * 0.5f - p->m_rect.w * 0.5f);
			p->m_rect.y = (float)(int)(g_view->GetHeight() * 0.25f - p->m_rect.h * 0.5f);
		}
		SetPos("Wait", g_view->GetWidth() * 0.5f, g_view->GetHeight() * 0.18f);
		SetPos("AI", g_view->GetWidth() * 0.5f, g_view->GetHeight() * 0.18f);
		SetPos("Replay", 45.0f, 60.0f);
		SetPos("FastForward", 45.0f, 60.0f);
		break;
	}

	case 0x190:
		m_show = msg.m_param1 == 1;
		break;
	}
}

// letter_effect.cpp:682
void CLetter::Print(float x, float y, const char* str, float spacing, int type, float scale, unsigned long color) {
	if (str == NULL) {
		return;
	}
	int len = strlen(str);
	if (len == 0) {
		return;
	}

	float width = 0.0f;
	const char* charSet = m_fontInfo.pCharSet;
	m_pFont->SetScale(scale);

	switch (m_align) {
	case 1: {
		const char* p = str;
		while (*p) {
			int c = *p++;
			int index = strchr(charSet, c) - charSet;
			width += m_pFont->GetCharWidth(g_view, index) + spacing * scale;
		}
		width -= spacing;
		width *= 0.5f;
		break;
	}
	case 2: {
		const char* p = str;
		while (*p) {
			int c = *p++;
			int index = strchr(charSet, c) - charSet;
			width += m_pFont->GetCharWidth(g_view, index) + spacing * scale;
		}
		width -= spacing;
		break;
	}
	}

	x -= width;

	const char* q = str;
	while (*q) {
		int c = *q++;
		int index = strchr(charSet, c) - charSet;
		float w = m_pFont->PutChar(g_view, x, y, index, type, color);
		w += spacing;
		x += w;
	}
}
