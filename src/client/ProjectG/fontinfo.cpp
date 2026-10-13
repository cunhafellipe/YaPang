// Original: d:\build\custom\temp\code\client\projectg\fontinfo.cpp
#define FONTINFO_FONTINFO_INLINE
#include <atlbase.h>	// stand-in for the precompiled header: _AtlWinModule initializer
			// (keeps BAR_LENGTH dynamically initialized, docs/tough_problems/010)
#include "fontinfo.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "cardsystem.h"
#include "club.h"
#include "golfball.h"
#include "wind.h"
#include "mousecursor.h"
#include "wview.h"
#include "quadtree.h"
#include "wresrcmng.h"
#include "SoundManager.h"
#include "SimpleOverlay.h"
#include "ClientSetting.h"
#include "InputManager.h"
#include "wutil.h"
#include "s5/Utilities.h"
#include "s5/classicserver/ClassicServer.h"
#include "../../shared/Localize.h"
#include "shared/s5/SharedUtilities.h"
#include "logininfo.h"
#include "commonutil.h"
#include "binstr.h"
#include "GroundItemMan.h"
#include "TaskManager.h"
#include "UccManager.h"
#include "ShuffleBonus.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

// Calipers tuning floats without a symbol: [20] = 1.0f (CDistToHoleInfo::Display), [21] = 15.0f (speed limit),
// [26] = 500.0f (right end of the bar), [28] = left margin past the club's
// narrow accuracy.
// [0]-[19] are ints: the game clear bonus per course (GetGameClearBonus).
union uFontInfoConst {
	int i;
	float f;
};
extern "C" const uFontInfoConst data_9eb5b8[29];
// A float with no symbol (a screen width, set elsewhere).
extern "C" float data_af23d0;
// Calipers key-repeat timer.
extern "C" float data_af23d8;
// Calipers key-driven cursor position and its move speed.
extern "C" {
static float data_af23d4;
static float data_a7f8c0 = 1.0f;
}
extern WMatrix g_camera;

int WisZero(const float &f, float eps);

// fontinfo.h:33
void CFontInfo::Process(float fDelta) {
}

// fontinfo.h:34
void CFontInfo::Display(void) {
}

// fontinfo.h:42
void CFontInfo::GetScreenPos(float *pfX, float *pfY) {
}

// fontinfo.h:46
void CFontInfo::Reset(void) {
}

// fontinfo.h:49
char *&CFontInfo::Buffer(void) {
	return m_pBuffer;
}

// fontinfo.h:458
char *&CAccuracyInfo::Buffer2(void) {
	return m_pBuffer2;
}

// fontinfo.h:493
char *&CGuildInfo::Buffer2(void) {
	return m_pBuffer2;
}

// fontinfo.h:796
void CCalipers::VisiblePowerBar(int nState) {
	m_nPowerBarState = nState;
}

// quadtree.h:541
const std::vector<CQuadTree::sColData> &CQuadTree::GetColList(void) {
	return m_colList;
}

// cardsystem.h:63
unsigned char CCardManager::GetWindColor(void) {
	return m_windColor;
}

// mousecursor.h:204
void CMouseCursor::SetHoleCup(float x, float y) {
	m_holeCupX = x;
	m_holeCupY = y;
}

// shareddoc.h:642
unsigned long CSharedDoc::GetFlagAbilityItem(void) {
	return m_flagAbilityItem;
}

// shareddoc.h:644
unsigned char CSharedDoc::GetNumBonusPangItem(void) {
	return m_numBonusPangItem;
}

// shareddoc.h:1012
bool CSharedDoc::IsWearHalloweenItem(void) {
	return m_wearHalloweenItem;
}

// shareddoc.inl:174
bool IsGameType(unsigned char type) {
	return WSingleton<CSharedDoc>::Instance()->m_roomType == type ? true : false;
}

bool CFontInfo::m_loaded;

// fontinfo.cpp:40
CFontInfo::CFontInfo(void) : m_pBuffer(NULL) {
	if (!m_loaded) {
		m_loaded = true;

		AddFont("[font_hole.jpg", 16, 16, 1, 128, 32, "1234567890ym%-.,+>");
		AddFont("[font_bar.jpg", 16, 16, 1, 128, 32, "1234567890ym%-. ");
		AddFont("[font_wind.jpg", 16, 16, 1, 128, 64, "1234567890ym%-.?/:");
		AddFont("[font_club.jpg", 32, 32, 1, 128, 128, "0123456789abcdef");
		AddFont("[bigfont.jpg", 32, 32, 1, 256, 128, "1234567890ym%-./x?() ");

		if (IsGameType(7)) {
			AddFont("[font_hole_skins_red.jpg", 16, 16, 1, 128, 32, "1234567890-,");
			AddFont("[font_hole_skins_blue.jpg", 16, 16, 1, 128, 32, "1234567890+,");
			AddFont("[font_hole_skins_grey.jpg", 16, 16, 1, 128, 32, "1234567890>,");
		}

		if (IsGameType(6)) {
			AddFont("[font_hole_skins_red.jpg", 16, 16, 1, 128, 32, "1234567890-,");
			AddFont("[font_hole_skins_blue.jpg", 16, 16, 1, 128, 32, "1234567890+,");
		}
	}
}

// fontinfo.cpp:67
WList<CFontInfo::sFontInfo *> CFontInfo::m_fontList(16, 4);

// fontinfo.cpp:70
CFontInfo::~CFontInfo() {
	Clear();
}

// The font textures AddFont accepts; a registered font points at its entry.
static char *s_fontFiles[8] = {
	"[font_hole.jpg",
	"[font_bar.jpg",
	"[font_wind.jpg",
	"[font_club.jpg",
	"[bigfont.jpg",
	"[font_hole_skins_red.jpg",
	"[font_hole_skins_blue.jpg",
	"[font_hole_skins_grey.jpg",
};

// fontinfo.cpp:76
void CFontInfo::AddFont(const char *name, int fontw, int fonth, int numPages, int texw, int texh, char *charSet) {
	sFontInfo *pInfo = new sFontInfo;

	memset(pInfo, 0, sizeof(tagWTITLEFONT));

	for (int i = 0; i < sizeof(s_fontFiles) / sizeof(s_fontFiles[0]); i++) {
		if (strcmp(s_fontFiles[i], name) == 0) {
			pInfo->filename = &s_fontFiles[i];
			break;
		}
	}

	if (pInfo->filename != NULL) {
		pInfo->fontw = fontw;
		pInfo->fonth = fonth;
		pInfo->numPages = numPages;
		pInfo->texw = texw;
		pInfo->texh = texh;
		pInfo->pCharSet = charSet;
		pInfo->m_index = -1;

		strcpy(pInfo->m_name, name);

		m_fontList.AddItem(pInfo, pInfo->m_name, false);
	} else {
		delete pInfo;
	}
}

// fontinfo.cpp:126
void CFontInfo::Init(char *pszName, sFont *pFont) {
	if (pFont == NULL) {
		pFont = &m_font;
	}
	pFont->m_pFontInfo = m_fontList.Find(pszName);

	pFont->m_pTitleFont = g_resrcmng->GetTitleFont();
	pFont->m_pTitleFont->Create(pFont->m_pFontInfo);

	m_color = 0xFFFFFFFF;
	m_nFlag = 0;
}

// fontinfo.cpp:141
void CFontInfo::Clear(void) {
	if (g_resrcmng && m_font.m_pTitleFont) {
		g_resrcmng->Release(m_font.m_pTitleFont);
		m_font.m_pTitleFont = NULL;
	}
	if (m_pBuffer) {
		delete[] m_pBuffer;
		m_pBuffer = NULL;
	}
	for (sFontInfo *pInfo = m_fontList.Start(); pInfo; pInfo = m_fontList.Next()) { if (pInfo) delete pInfo; } m_fontList.Reset();

	m_loaded = false;
}

// fontinfo.cpp:151
float CFontInfo::GetTextWidth(const char *text, float space, float scale) {
	int len = strlen(text);
	return (float)(len * m_font.m_pFontInfo->fontw) * scale * 0.5f + len * space * 2;
}

// fontinfo.cpp:157
float CFontInfo::GetExactTextWidth(const char *text, float space, float scale) {
	if (text == NULL) {
		return 0.0f;
	}

	int len = strlen(text);
	if (len == 0) {
		return 0.0f;
	}

	float oldScale = m_font.m_pTitleFont->GetScale();

	m_font.m_pTitleFont->SetScale(scale);

	char *charSet = m_font.m_pFontInfo->pCharSet;

	float width = 0;
	while (*text) {
		char c = *text++;
		char *pFound = strchr(charSet, c);

		width += m_font.m_pTitleFont->GetCharWidthA(g_view, pFound - charSet) + space * scale;
	}

	float result = width - space;

	m_font.m_pTitleFont->SetScale(oldScale);

	return result;
}

// fontinfo.cpp:194
float CFontInfo::Print(float x, float y, const char *text, float space, int flag, float scale, sFont *pFont) {
	if (text == NULL) {
		return 0.0f;
	}

	int len = strlen(text);
	if (len == 0) {
		return 0.0f;
	}

	if (pFont == NULL) {
		pFont = &m_font;
	}

	char *charSet = pFont->m_pFontInfo->pCharSet;

	float oldScale = pFont->m_pTitleFont->GetScale();
	pFont->m_pTitleFont->SetScale(scale);

	float width = 0.0f;
	switch (m_nFlag) {
	case 1: {
		const char *p = text;
		while (*p) {
			char *pFound = strchr(charSet, *p++);
			width += pFont->m_pTitleFont->GetCharWidthA(g_view, pFound - charSet) + space * scale;
		}
		width = (width - space) * 0.5f;
		break;
	}
	case 2: {
		const char *p = text;
		while (*p) {
			char *pFound = strchr(charSet, *p++);
			width += pFont->m_pTitleFont->GetCharWidthA(g_view, pFound - charSet) + space * scale;
		}
		width = width - space;
		break;
	}
	}

	x -= width;
	while (*text) {
		char *pFound = strchr(charSet, *text++);
		float charWidth = pFont->m_pTitleFont->PutChar(g_view, x, y, pFound - charSet, flag, m_color);
		x += charWidth + space;
	}

	pFont->m_pTitleFont->SetScale(oldScale);
	return width;
}

// fontinfo.cpp:271
CDistToHoleInfo::CDistToHoleInfo(void) {
	ResetUiPosition();
	m_fScale = 0.77f;
	m_curPos.y = 63.0f;
	m_curPos.z = 0.5f;
	m_nState = 5;
	m_fBlend = 0;

	Init("[bigfont.jpg", NULL);

	m_pBuffer = new char[0x20];
}

// fontinfo.cpp:290
void CDistToHoleInfo::Process(float fDelta) {
	if (GOLFDOC()->GameMode() & 2) {
		return;
	}

	if ((GOLFDOC()->GameMode() & 0x8210) || !IsMyTurn(true)
		|| (GOLFDOC()->m_gameState < 15 && GOLFDOC()->m_gameState != 8)
		|| (GOLFDOC()->m_gameState == 8 && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[0] == (unsigned char)1)) {
		m_nState = 0;
	}

	if (CSharedDoc::Instance()->m_roomType == 10) {
		m_nState = 0;
	}

	switch (m_nState) {
	case 1:
		m_fTime += fDelta;
		if (m_fTime > 0.5f) {
			m_fTime = 0.5f;
			m_nState = 0;
		}

		m_fBlend = m_fTime * 2;

		if (g_input->GetList() && GOLFDOC()->GameMode() != 4) {
			m_nState = 0;
		}
		break;
	case 2:
		m_fAlpha += fDelta * 510.0f;
		if (m_fAlpha > 255.0f) {
			m_fTime = 0;
			m_nState = 4;
			m_fAlpha = 255.0f;
		}
		break;
	case 4:
		m_fTime += fDelta;
		if (m_fTime > 2.0f) {
			m_fTime = 0;
			m_nState = 1;
		}

		if (g_input->GetList() && GOLFDOC()->GameMode() != 4) {
			m_nState = 3;
		} else {
			WVector toCup = GetHoleCupPos() - g_camera.pivot;
			if (g_camera.za.x * toCup.x + g_camera.za.y * toCup.y + g_camera.za.z * toCup.z < 0.0f) {
				m_nState = 3;
			}
		}
		break;
	case 3:
		m_fAlpha -= fDelta * 510.0f;
		if (!(m_fAlpha < 0.0f)) {
			break;
		}
		m_nState = 0;
	case 0:
		m_fBlend = 1.0f;
		m_fAlpha = 255.0f;
		break;
	}
}

// fontinfo.cpp:366
void CDistToHoleInfo::Display(void) {
	if (m_nState == 5 || (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_identity & 1) && m_nState != 0 && GOLFDOC()->GameMode() != 4) {
		return;
	}

	WVector cup = GOLFDOC()->GetHoleData().m_cupPos;
	WVector screen = g_view->Projection(cup);

	if (screen.z <= 0.0f || data_9eb5b8[20].f <= screen.z) {
		return;
	}

	float fInv = data_9eb5b8[20].f - m_fBlend;
	m_fixPos = WVector((screen.x + m_fOffsetX) * m_fBlend + fInv * m_uiX,
		fInv * m_uiY + m_curPos.y * m_fBlend,
		fInv * m_fScale + m_curPos.z * m_fBlend);

	m_color = ((int)m_fAlpha << 24) | 0xFF0000;

	Print(m_fixPos.x, GOLFDOC()->GameMode() == 0x200 ? screen.y : m_fixPos.y, m_pBuffer, m_fixPos.z * 6.0f, 0, m_fixPos.z, NULL);
}

// fontinfo.cpp:406
void CDistToHoleInfo::Fix(void) {
	m_fixPos.x = m_curPos.x;
	m_fAlpha = 255.0f;
	m_nState = 0;
	m_fixPos.y = m_curPos.y;
	m_fixPos.z = m_curPos.z;
}

// fontinfo.cpp:419
void CDistToHoleInfo::Reset(void) {
	sPlayerData *pPlayer = PLAYER(GOLFDOC()->m_myPlayerIndex);
	WVector cup = GetHoleCupPos();
	WVector delta;
	delta.x = cup.x - pPlayer->m_ballPos.x;
	delta.z = cup.z - pPlayer->m_ballPos.z;

	if (IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == 2) {
		delta = GetHoleCupPos() - GolfBall().m_pos;
	}

	m_fDistance = sqrtf(delta.x * delta.x + delta.z * delta.z) * 0.3125f;
	if (m_fDistance <= 100.0f) {
		sprintf(m_pBuffer, "%.1fy", m_fDistance);
		if (m_fDistance <= 10.0f) {
			m_fOffsetX = -14.0f;
		} else {
			m_fOffsetX = -20.0f;
		}
	} else {
		sprintf(m_pBuffer, "%.0fy", m_fDistance);
		m_fOffsetX = -20.0f;
	}

	m_fTime = 0;
	m_fBlend = 0;

	if (IsMyTurn(true) && !(IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == 2)) {
		m_fixPos = WVector(m_uiX, m_uiY, m_fScale);
		m_fAlpha = 0;
		m_nState = 2;
	} else {
		m_nState = 0;
		m_fAlpha = 255.0f;
	}
}

// fontinfo.cpp:467
void CDistToHoleInfo::ResetUiPosition(void) {
	m_uiX = g_view->GetWidth() * 0.5f + 20.0f;
	m_uiY = g_view->GetHeight() * 0.25f + 25.0f;
}

// fontinfo.cpp:479
CGuildInfo::CGuildInfo(void) {
	Init("[font_hole_skins_red.jpg", &m_leftFont);
	Init("[font_hole_skins_blue.jpg", &m_rightFont);

	m_pBuffer = new char[0x20];
	m_pBuffer2 = new char[0x20];
	m_fTime = 0;
	m_fScale = 1.5f;
	m_pszSound = "pang_down";

	memset(m_targetScore, 0, sizeof(m_targetScore));

	memset(m_displayScore, 0, sizeof(m_displayScore));
}

// fontinfo.cpp:496
CGuildInfo::~CGuildInfo() {
	if (m_pBuffer2) {
		delete m_pBuffer2;
		m_pBuffer2 = NULL;
	}
}

// fontinfo.cpp:502
void CGuildInfo::Process(float fDelta) {
	m_fTime += fDelta;

	if (m_targetScore[0] > m_displayScore[0]) {

		m_displayScore[0]++;
	}

	if (m_targetScore[1] > m_displayScore[1]) {

		m_displayScore[1]++;
	}

	float fScale = 2.5f - m_fTime * 1.5f;
	m_fScale = fScale < 1.5f ? 1.5f : (fScale > 2.5f ? 2.5f : fScale);
}

// fontinfo.cpp:520
void CGuildInfo::SetScore(int nScore1, int nScore2) {
	m_targetScore[0] = nScore1;
	m_targetScore[1] = nScore2;
	m_fTime = 0;

	if (m_pszSound && !g_audio->IsPlaying(m_pszSound)) {
		g_audio->PlaySfx(m_pszSound, NULL, 0, m_pszSound, NULL, 0.5f, 200.0f);
	}
}

// fontinfo.cpp:540
void CGuildInfo::Display(void) {
	m_nFlag = 0;
	sprintf(m_pBuffer, "%d", m_displayScore[0]);
	sprintf(m_pBuffer2, "%d", m_displayScore[1]);

	Print(g_view->GetWidth() - 80.0f, 250.0f, m_pBuffer, 4.0f, 0, m_fScale, &m_leftFont);
	Print(g_view->GetWidth() - 30.0f, 250.0f, m_pBuffer2, 4.0f, 0, m_fScale, &m_rightFont);
}

// fontinfo.cpp:559
CHoleCupInfo::CHoleCupInfo(void) : m_pDistToHole(NULL) {
	Init("[font_bar.jpg", NULL);

	m_nDisplayMode = 1;

	m_pTopHoleOverlay[0] = g_resrcmng->GetOverlay("[top_hole_l.jpg", 0);
	m_pTopHoleOverlay[1] = g_resrcmng->GetOverlay("[top_hole_m.jpg", 0);
	m_pTopHoleOverlay[2] = g_resrcmng->GetOverlay("[top_hole_h.jpg", 0);

	m_pDistToHole = new CDistToHoleInfo;
}

// fontinfo.cpp:574
CHoleCupInfo::~CHoleCupInfo() {

	for (int i = 0; i < 3; i++) {
		if (m_pTopHoleOverlay[i]) {

			g_resrcmng->Release(m_pTopHoleOverlay[i]);
			m_pTopHoleOverlay[i] = NULL;
		}
	}

	if (m_pDistToHole) {
		delete m_pDistToHole;
		m_pDistToHole = NULL;
	}
}

// fontinfo.cpp:590
void CHoleCupInfo::Reset(void) {
	WVector cup = GOLFDOC()->GetHoleData().m_cupPos;
	m_fCupHeight = (cup.y - (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos.y - GolfBall().m_radius)) * 0.3125f * 0.9144f;
	if (m_pDistToHole)
		m_pDistToHole->Reset();
}

// fontinfo.cpp:597
void CHoleCupInfo::Fix(void) {
	if (m_pDistToHole) {
		m_pDistToHole->Fix();
	}
}

// fontinfo.cpp:606
void CHoleCupInfo::GetScreenPos(float *pfX, float *pfY) {
	*pfX = m_screenX;
	*pfY = m_screenY;
}

// fontinfo.cpp:614
void CHoleCupInfo::Process(float fDelta) {
	if (m_pDistToHole) {
		m_pDistToHole->Process(fDelta);
	}
}

// fontinfo.cpp:623
void CHoleCupInfo::Display(void) {
	char buf[0x20];

	if (m_pDistToHole) {
		m_pDistToHole->Display();
	}

	if (Abs(m_fCupHeight) > 10.0f) {
		sprintf(buf, "%.1fm", m_fCupHeight);
	} else {
		sprintf(buf, "%.2fm", m_fCupHeight);
	}

	WVector proj = g_view->Projection(GetHoleCupPos());

	if (proj.z <= 0.0f || data_9eb5b8[20].f <= proj.z) {
		return;
	}

	m_screenX = proj.x;
	m_screenY = GOLFDOC()->GameMode() == 0x200 ? proj.y : 62.0f;
	CMouseCursor::Instance()->SetHoleCup(proj.x, m_screenY);

	if (Abs(m_fCupHeight) < 0.1f) {
		m_nDisplayMode = 1;
		Print(m_screenX + 16.0f, m_screenY - 26.0f, buf, 3.0f, 0, 1.0f, NULL);
	} else if (m_fCupHeight > 0.0f) {
		m_nDisplayMode = 2;
		Print(m_screenX + 16.0f, m_screenY - 17.0f, buf, 3.0f, 0, 1.0f, NULL);
	} else {
		m_nDisplayMode = 0;
		Print(m_screenX + 16.0f, m_screenY - 37.0f, buf, 3.0f, 0, 1.0f, NULL);
	}

	if (m_pTopHoleOverlay[m_nDisplayMode]) {
		WRect rc(m_screenX - 32.0f, m_screenY - 47.0f, 64.0f, 64.0f);
		WRect uv(0.0f, 0.0f, 1.0f, 1.0f);
		m_pTopHoleOverlay[m_nDisplayMode]->Render(g_view, uv, rc, 0, 0xFFFFFFFF, 0.0f, 0);
	}
}

// fontinfo.cpp:684
void CHoleCupInfo::ResetUiPosition(void) {
	if (m_pDistToHole) {
		m_pDistToHole->ResetUiPosition();
	}
}

// fontinfo.cpp:697
CClubRangeInfo::CClubRangeInfo(void) {
	Init("[font_wind.jpg", NULL);
	m_vPos.x = m_vPos.y = m_vPos.z = 0;

	m_nFlag = 1;
	m_pBuffer = new char[0x20];
}

// fontinfo.cpp:708
void CClubRangeInfo::Display(void) {
	WVector proj = g_view->Projection(m_vPos);

	if (proj.z <= 0.0f || proj.z >= 1.0f)
		return;

	const WVector &ball = GolfBall().m_pos;
	float dx = m_vPos.x - ball.x;
	float dz = m_vPos.z - ball.z;

	sprintf(m_pBuffer, "%.0fy", sqrtf(dz * dz + dx * dx) * 0.3125f);
	Print(proj.x, proj.y - 25.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);
}

// fontinfo.cpp:737
CFlyInfo::CFlyInfo(void) : m_pGroundUI(NULL), m_pGroundTextUI(NULL), m_pApproachOutOverlay(NULL) {
	Init("[bigfont.jpg", NULL);
	m_pBuffer = new char[0x20];
	m_nFlag = 2;

	m_pGroundUI = new CSimpleUI("[ground.jpg");
	m_pGroundUI->Add("Fairway", WRect(0.0f, 0.0f, 128.0f, 25.0f), false);
	m_pGroundUI->Add("Rough", WRect(0.0f, 25.0f, 128.0f, 25.0f), false);
	m_pGroundUI->Add("Green", WRect(0.0f, 50.0f, 128.0f, 25.0f), false);
	m_pGroundUI->Add("Bunker", WRect(0.0f, 75.0f, 128.0f, 25.0f), false);
	m_pGroundUI->Add("Tee", WRect(0.0f, 100.0f, 128.0f, 25.0f), false);
	m_pGroundUI->Add("Ice", WRect(0.0f, 125.0f, 128.0f, 25.0f), false);
	m_pGroundUI->Add("Sand", WRect(0.0f, 150.0f, 128.0f, 25.0f), false);
	m_pGroundUI->Add("Water", WRect(0.0f, 175.0f, 128.0f, 25.0f), false);

	m_pGroundUI->Add("Vector", WRect(0.0f, 200.0f, 128.0f, 25.0f), false);

	m_pGroundTextUI = new CSimpleUI("[ground_text.jpg");

	m_pGroundTextUI->Add("ºñ°Å¸®", WRect(0.0f, 0.0f, 64.0f, 27.0f), false);
	m_pGroundTextUI->Add("³²Àº°Å¸®", WRect(0.0f, 27.0f, 64.0f, 27.0f), false);

	if (CSharedDoc::Instance()->m_roomType == 10) {
		m_pApproachOutOverlay = g_resrcmng->GetOverlay("approach_out.png", 0);
	}

	m_fTime = 0;
}

// fontinfo.cpp:776
CFlyInfo::~CFlyInfo() {
	if (m_pGroundUI) {
		delete m_pGroundUI;
		m_pGroundUI = NULL;
	}
	if (m_pGroundTextUI) {
		delete m_pGroundTextUI;
		m_pGroundTextUI = NULL;
	}

	if (g_resrcmng && m_pApproachOutOverlay) {
		g_resrcmng->Release(m_pApproachOutOverlay);
		m_pApproachOutOverlay = NULL;
	}
}

// fontinfo.cpp:786
void CFlyInfo::Process(float fDelta) {
	if (GOLFDOC()->GameMode() & 0x80) {
		CGolfBall &ball = GolfBall();

		if (ball.m_state != 0) {
			m_fTime = 0;
			m_fGroundAlpha = 0;
			m_fCarryAlpha = 0;
			m_fToPinAlpha = 0;
			CalcDistance();
			return;
		}

		if (!ball.m_landed && ball.m_lie == 0 && ball.IsHoleIn() != 1) {
			if (WisZero(m_fTime, 1e-5f)) {
				CalcDistance();

				switch (GetPVS().GetGroundType(ball.m_pos, true, 0)) {
				case 1:
					strcpy(m_szGround, "Fairway");
					break;
				case 2:
					strcpy(m_szGround, "Green");
					break;
				case 4:
					strcpy(m_szGround, "Rough");
					break;
				case 3:
					strcpy(m_szGround, "Bunker");
					break;
				case 0:
					strcpy(m_szGround, "Tee");
					break;
				case 7:
					strcpy(m_szGround, "Ice");
					break;
				case 8:
					strcpy(m_szGround, "Vector");
					break;
				case 11:
					strcpy(m_szGround, "Sand");
					break;
				case 14:
					strcpy(m_szGround, "Water");
					break;
				default:
					strcpy(m_szGround, "Rough");
					break;
				}
			}

			fDelta += m_fTime;
			m_fTime = fDelta;
			m_fGroundAlpha = Between(0.0f, fDelta * 510.0f + m_fGroundAlpha, 255.0f);
			m_fCarryAlpha = Between(0.0f, (fDelta - data_9eb5b8[20].f) * 510.0f + m_fCarryAlpha, 255.0f);
			m_fToPinAlpha = Between(0.0f, (fDelta - 1.5f) * 510.0f + m_fToPinAlpha, 255.0f);
		}
	}
}

// fontinfo.cpp:866
void CFlyInfo::CalcDistance(void) {
	WVector diff;
	diff = GolfBall().m_pos - GolfBall().PreData(0)->m_pos;

	m_fCarry = sqrtf(diff.x * diff.x + diff.z * diff.z) * 0.3125f;

	WVector cup = GOLFDOC()->GetHoleData().m_cupPos;
	diff = GolfBall().m_pos - cup;
	m_fToPin = sqrtf(diff.x * diff.x + diff.z * diff.z) * 0.3125f;
}

// fontinfo.cpp:878
void CFlyInfo::Display(void) {
	CGolfBall &ball = GolfBall();

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 10) {
		if (WSingleton<CSharedDoc>::Instance()->m_gameStartTime) {
			m_color = 0xFFFF3434;
			m_nFlag = 0;

			CSharedDoc *pDoc = WSingleton<CSharedDoc>::Instance();
			if (GOLFDOC()->GameMode() & 0x40) {
				sprintf(m_pBuffer, "-");
				Print(100.0f, 202.0f, m_pBuffer, 2.0f, 0, 0.64f, NULL);
			} else if (GOLFDOC()->GameMode() & 0x80) {
				if (GolfBall().m_state == 0) {
					pDoc = WSingleton<CSharedDoc>::Instance();
					unsigned int dist = pDoc->m_gameResultList[MyIndex()].m_approachDistance;
					if (dist == 0xFFFFFFFF) {
						if (m_pApproachOutOverlay) {
							WRect rc(72.0f, 193.0f, 64.0f, 32.0f);
							WRect uv(0.0f, 0.0f, 1.0f, 1.0f);
							m_pApproachOutOverlay->Render(g_view, uv, rc, 0, 0xFFFF3434, 0.0f, 0);
						}
					} else {
						sprintf(m_pBuffer, "%d.%d", dist / 10, dist % 10);
						Print(80.0f, 202.0f, m_pBuffer, 2.0f, 0, 0.64f, NULL);
					}
				} else {
					int whole = float2int(m_fToPin);
					sprintf(m_pBuffer, "%d.%d", whole, float2int((m_fToPin - whole) * 10.0f));
					Print(80.0f, 202.0f, m_pBuffer, 2.0f, 0, 0.64f, NULL);
				}
			} else if (GOLFDOC()->GameMode() & 0x800) {
				unsigned int dist = pDoc->m_gameResultList[MyIndex()].m_approachDistance;
				if (dist == 0xFFFFFFFF) {
					if (m_pApproachOutOverlay) {
						WRect rc(72.0f, 193.0f, 64.0f, 32.0f);
						WRect uv(0.0f, 0.0f, 1.0f, 1.0f);
						m_pApproachOutOverlay->Render(g_view, uv, rc, 0, 0xFFFF3434, 0.0f, 0);
					}
				} else {
					sprintf(m_pBuffer, "%d.%d", dist / 10, dist % 10);
					Print(80.0f, 202.0f, m_pBuffer, 2.0f, 0, 0.64f, NULL);
				}
			}

			pDoc = WSingleton<CSharedDoc>::Instance();
			if (GOLFDOC()->GameMode() & 0x8C0) {
				int point = pDoc->m_gameResultList[MyIndex()].m_approachPoint;
				if (point == -1) {
					sprintf(m_pBuffer, "-");
					Print(100.0f, 226.0f, m_pBuffer, 2.0f, 0, 0.64f, NULL);
				} else {
					sprintf(m_pBuffer, "%d.%03d", point / 1000, point % 1000);
					Print(80.0f, 226.0f, m_pBuffer, 2.0f, 0, 0.64f, NULL);
				}
			}
		}
	} else if (GOLFDOC()->GameMode() & 0x80) {
		m_color = 0xFFFFFFFF;

		int carry = float2int(m_fCarry);
		int carryFrac = float2int((m_fCarry - carry) * 100.0f);
		float barY = COption::Instance()->gGetBar_Y();

		float left = g_view->GetWidth() * 0.5f - 20.0f;
		float top = barY - 33.0f;
		float right = g_view->GetWidth() * 0.5f + 35.0f;
		float bottom = g_view->GetHeight() * 0.5f - 37.0f;

		float dx = (right - left) * Between(0.0f, (m_fTime - 0.5f) * 2.0f, 1.0f);
		float dy = (bottom - top) * Between(0.0f, (m_fTime - 0.5f) * 2.0f, 1.0f);

		m_nFlag = 2;
		sprintf(m_pBuffer, "%d", carry);
		float y = top + dy;
		Print(left - 5.0f + dx, y, m_pBuffer, 4.0f, 0, 0.64f, NULL);

		float x = left + dx;
		m_nFlag = 1;
		Print(x, y, ".", 4.0f, 0, 0.64f, NULL);

		m_nFlag = 0;
		if (GOLFDOC()->m_approachMode) {
			sprintf(m_pBuffer, "%dy", carryFrac / 10);
		} else {
			sprintf(m_pBuffer, "%02dy", carryFrac);
		}
		Print(x + 5.0f, y, m_pBuffer, 4.0f, 0, 0.64f, NULL);

		m_color = 0xFFFF3434;
		carry = float2int(m_fToPin);
		carryFrac = float2int((m_fToPin - carry) * 100.0f);

		left = g_view->GetWidth() * 0.5f - 20.0f;
		top = barY - 12.0f;
		right = g_view->GetWidth() * 0.5f + 35.0f;
		bottom = g_view->GetHeight() * 0.5f - 7.0f;

		dx = (right - left) * Between(0.0f, (m_fTime - 1.0f) * 2.0f, 1.0f);
		dy = (bottom - top) * Between(0.0f, (m_fTime - 1.0f) * 2.0f, 1.0f);

		m_nFlag = 2;
		sprintf(m_pBuffer, "%d", carry);
		y = top + dy;
		Print(left - 5.0f + dx, y, m_pBuffer, 4.0f, 0, 0.64f, NULL);

		x = left + dx;
		m_nFlag = 1;
		Print(x, y, ".", 4.0f, 0, 0.64f, NULL);

		m_nFlag = 0;
		if (GOLFDOC()->m_approachMode) {
			if (ball.m_state == 0 && ball.IsHoleIn() == 1) {
				sprintf(m_pBuffer, "%dy", 0);
			} else {
				sprintf(m_pBuffer, "%dy", carryFrac / 10);
			}
		} else {
			sprintf(m_pBuffer, "%02dy", ball.m_state == 0 && ball.IsHoleIn() == 1 ? 0 : carryFrac);
		}
		Print(x + 5.0f, y, m_pBuffer, 4.0f, 0, 0.64f, NULL);

		if (ball.m_state == 0 && !ball.m_landed && ball.IsHoleIn() != 1 && ball.m_lie == 0) {
			if (m_pGroundUI) {
				m_pGroundUI->SetColor(((int)m_fGroundAlpha << 24) | 0xFFFFFF);
				m_pGroundUI->Render(m_szGround, g_view->GetWidth() * 0.5f - 64.0f, g_view->GetHeight() * 0.5f - 72.0f, 1.0f, 0);
			}

			if (m_pGroundTextUI) {
				if (IsLocalContent((localContentType_t)0x4F) && WSingleton<CSharedDoc>::Instance()->m_playingMode == 2) {
					m_fCarryAlpha = 0;
					m_fToPinAlpha = 0;
				}
				m_pGroundTextUI->SetColor(((int)m_fCarryAlpha << 24) | 0xFFFFFF);
				m_pGroundTextUI->Render("\xBA" "\xF1" "\xB0" "\xC5" "\xB8" "\xAE", g_view->GetWidth() * 0.5f - 80.0f, g_view->GetHeight() * 0.5f - 37.0f, 1.0f, 0);
				m_pGroundTextUI->SetColor(((int)m_fToPinAlpha << 24) | 0xFFFFFF);
				m_pGroundTextUI->Render("\xB3" "\xB2" "\xC0" "\xBA" "\xB0" "\xC5" "\xB8" "\xAE", g_view->GetWidth() * 0.5f - 80.0f, g_view->GetHeight() * 0.5f - 7.0f, 1.0f, 0);
			}
		}
	}
}

// fontinfo.cpp:1062
CWindInfo::CWindInfo(void) {
	Init("[font_wind.jpg", NULL);

	m_nFlag = 1;
	m_pBuffer = new char[0x20];
}

// fontinfo.cpp:1070
void CWindInfo::Display(void) {
	if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000006
		|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000020) {
		m_color = 0xFF00FF00;
		sprintf(m_pBuffer, "1m");
	} else {
		if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000016) {
			m_color = 0xFF0000FF;
		} else if (CSharedDoc::Instance()->m_roomType == 3
			|| (GolfBall().m_preDataIndex >= 0 && Wind().IsInTurbulence(GolfBall().m_pos))
			|| (GolfBall().m_preDataIndex < 0 && Wind().IsInTurbulence(g_camera.pivot))) {
			m_color = 0xFFFFFFFF;
			sprintf(m_pBuffer, "?");
			Print(g_view->GetWidth() - 21.0f, COption::Instance()->gGetBar_Y() + 26.0f, m_pBuffer, 1.0f, 0, 1.0f, NULL);
			return;
		} else {
			m_color = 0xFFFFFFFF;

			switch (CSharedDoc::Instance()->m_roomType) {
			case 0:
			case 1:
			case 7:
				CCardManager::Instance()->SetPlayerIndex(GOLFDOC()->m_myPlayerIndex);
				break;
			case 4:
			case 5:
			case 6:
			case 9:
			case 10:
				CCardManager::Instance()->SetPlayerIndex(0xFF);
				break;
			}

			if (CCardManager::Instance()->GetWindColor() == 1) {
				m_color = 0xFF59D3FF;
			}
		}
		sprintf(m_pBuffer, "%.0fm", Wind().GetGlobalIntensity());
	}

	Print(g_view->GetWidth() - 21.0f, COption::Instance()->gGetBar_Y() + 26.0f, m_pBuffer, 1.0f, 0, 1.0f, NULL);
}

// fontinfo.cpp:1132
CBonusInfo::CBonusInfo(void) : m_pPangOverlay(NULL), m_pPcBangOverlay(NULL) {
	Init("[bigfont.jpg", NULL);

	m_nFlag = 2;
	m_pBuffer = new char[0x20];
	m_fTime = 0;

	m_awardPang = 0;
	m_extraPang = 0;
	m_auxPartPang = 0;
	m_itemPang = 0;
	m_totalPang = 0;

	CSharedDoc *pDoc = CSharedDoc::Instance();

	if (pDoc->m_roomType == 7) {
		m_pPangOverlay = g_resrcmng->GetOverlay("[add_pang_skins.png", 0);
	} else {
		m_pPangOverlay = g_resrcmng->GetOverlay("[add_pang.png", 0);
	}

	if (CLoginInfo::Instance()->IsPcBang()) {
		m_pPcBangOverlay = g_resrcmng->GetOverlay("[pc_eventmark.tga", 0);
	}

	if (IsLocalContent((localContentType_t)0x17)) {
		m_pEventOverlay = g_resrcmng->GetOverlay(MakeStr("%s.tga", "z_Halloween_Male"), 0);
	}
}

// fontinfo.cpp:1166
CBonusInfo::~CBonusInfo() {
	if (IsLocalContent((localContentType_t)0x17)) {
		if (g_resrcmng && m_pEventOverlay) {
			g_resrcmng->Release(m_pEventOverlay);
			m_pEventOverlay = NULL;
		}
	}
	if (g_resrcmng && m_pPangOverlay) {
		g_resrcmng->Release(m_pPangOverlay);
		m_pPangOverlay = NULL;
	}
	if (g_resrcmng && m_pPcBangOverlay) {
		g_resrcmng->Release(m_pPcBangOverlay);
		m_pPcBangOverlay = NULL;
	}

	for (std::map<unsigned long, WOverlay *>::iterator it = m_overlays.begin(); it != m_overlays.end(); ++it) {
		if (g_resrcmng && it->second) {
			g_resrcmng->Release(it->second);
			it->second = NULL;
		}
	}
	m_overlays.clear();
}

// fontinfo.cpp:1180
void CBonusInfo::Process(float fDelta) {
	m_fTime += fDelta;

	m_awardPang = PLAYER(MyIndex())->m_ghostAward;
	m_extraPang = PLAYER(MyIndex())->m_extraPang;
	m_auxPartPang = (int)((float)(m_awardPang + m_extraPang) * CSharedDoc::Instance()->GetAuxPartProperty(MyIndex(), 3) * CSharedDoc::Instance()->m_pangPercent * CSharedDoc::Instance()->m_pangRate * 0.01f) - m_awardPang - m_extraPang;

	m_itemPang = PLAYER(MyIndex())->m_itemPang;
	m_totalPang = m_itemPang + m_extraPang + m_awardPang + m_auxPartPang;
}

// fontinfo.cpp:1194
void CBonusInfo::Display(void) {
	if (m_pPangOverlay) {
		WRect uv(0.0f, 0.0f, 1.0f, 1.0f);
		WRect rc;
		rc.x = g_view->GetWidth() * 0.5f - 256.0f;
		if (CSharedDoc::Instance()->m_roomType == 7) {
			rc.w = 256.0f;
			rc.h = 256.0f;
			rc.y = (g_view->GetHeight() - 256.0f - 128.0f + 60.0f) * 0.5f;
		} else {
			rc.w = 256.0f;
			rc.h = 128.0f;
			rc.y = (g_view->GetHeight() - 128.0f) * 0.5f;
		}
		m_pPangOverlay->Render(g_view, uv, rc, 0, 0xFFFFFFFF, 0.0f, 0);
	}

	// fontinfo.cpp:1203
	if (CLoginInfo::Instance()->IsPcBang() && m_pPcBangOverlay) {
		WRect uv(0.0f, 0.0f, 1.0f, 1.0f);
		WRect rc(g_view->GetWidth() * 0.5f + 150.0f, (g_view->GetHeight() + 25.0f) * 0.5f, 70.0f, 50.0f);
		m_pPcBangOverlay->Render(g_view, uv, rc, 0, 0xFFFFFFFF, 0.0f, 0);
	}

	// fontinfo.cpp:1208
	if (IsLocalContent((localContentType_t)0x17) && CSharedDoc::Instance()->IsWearHalloweenItem()) {
		WRect uv(0.0f, 0.0f, 1.0f, 1.0f);
		WRect rc(g_view->GetWidth() * 0.5f + 170.0f, g_view->GetHeight() * 0.5f - 52.0f, (float)m_pEventOverlay->GetWidth(), (float)m_pEventOverlay->GetHeight());
		m_pEventOverlay->Render(g_view, uv, rc, 0, 0xFFFFFFFF, 0.0f, 0);

		m_nFlag = 2;
		sprintf(m_pBuffer, "%%%d", 100);
		Print(g_view->GetWidth() * 0.5f + 220.0f, g_view->GetHeight() * 0.5f - 18.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);
	}

	// fontinfo.cpp:1220
	int yOff = 0;
	if (CSharedDoc::Instance()->m_roomType == 7) {
		m_nFlag = 2;
		sprintf(m_pBuffer, "%I64d", m_itemPang);
		Print(g_view->GetWidth() * 0.5f + 105.0f, g_view->GetHeight() * 0.5f - 88.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);

		sprintf(m_pBuffer, "%I64d", m_awardPang);
		Print(g_view->GetWidth() * 0.5f + 100.0f, g_view->GetHeight() * 0.5f - 53.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);

		sprintf(m_pBuffer, "%I64d", m_extraPang);
		Print(g_view->GetWidth() * 0.5f + 100.0f, g_view->GetHeight() * 0.5f - 18.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);

		m_nFlag = 0;
		sprintf(m_pBuffer, "%%%I64d", m_auxPartPang);
		Print(g_view->GetWidth() * 0.5f + 105.0f, g_view->GetHeight() * 0.5f - 18.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);

		m_nFlag = 2;
		sprintf(m_pBuffer, "%I64d", m_totalPang);
		Print(g_view->GetWidth() * 0.5f + 100.0f, g_view->GetHeight() * 0.5f + 25.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);

		yOff = 0x1C;
	} else {
		m_nFlag = 2;
		sprintf(m_pBuffer, "%I64d", m_awardPang);
		Print(g_view->GetWidth() * 0.5f + 40.0f, g_view->GetHeight() * 0.5f - 53.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);

		sprintf(m_pBuffer, "%I64d", m_extraPang);
		Print(g_view->GetWidth() * 0.5f + 40.0f, g_view->GetHeight() * 0.5f - 18.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);

		m_nFlag = 0;
		sprintf(m_pBuffer, "%%%I64d", m_auxPartPang);
		Print(g_view->GetWidth() * 0.5f + 45.0f, g_view->GetHeight() * 0.5f - 18.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);
	}

	// fontinfo.cpp:1266
	WOverlay::DrawBox(g_view, WRect((g_view->GetWidth() - 400.0f) * 0.5f, g_view->GetHeight() * 0.5f + data_9eb5b8[21].f, 400.0f, 3.0f), 0, 0xFF000000, 0.001f);
	WOverlay::DrawBox(g_view, WRect((g_view->GetWidth() - 400.0f) * 0.5f + data_9eb5b8[20].f, g_view->GetHeight() * 0.5f + 16.0f, 399.0f, 1.0f), 0, 0xFFFFFFFF, 0.001f);

	// fontinfo.cpp:1274
	CSharedDoc *pDoc = WSingleton<CSharedDoc>::Instance();
	std::vector<unsigned long> &list = pDoc->m_gainedItemList[0];
	int xCur = 0;
	int yCur = yOff;

	if (list.empty()) {
		Print(g_view->GetWidth() * 0.5f + 30.0f, g_view->GetHeight() * 0.5f + yOff + 28.0f, "-", 3.0f, 0, 1.0f, NULL);
		return;
	}

	// fontinfo.cpp:1280
	for (std::vector<unsigned long>::iterator it = list.begin(); it != list.end(); ++it) {
		unsigned long typeId = *it;
		WOverlay *pOverlay = NULL;

		std::map<unsigned long, WOverlay *>::iterator found = m_overlays.find(typeId);
		if (found != m_overlays.end()) {
			pOverlay = found->second;
		} else {
			IFF_ITEM_COMMON *pItem = ItemManager()->FindCommonItem(typeId);
			if (pItem) {
				switch (typeId >> 26) {
				case 2:
				case 5:
				case 6:
				case 0x1D:
					pOverlay = g_resrcmng->GetOverlay(MakeStr("%s.tga", pItem->m_icon), 0);
					if (pOverlay) {
						m_overlays.insert(std::map<unsigned long, WOverlay *>::value_type(typeId, pOverlay));
					}
					break;
				}
			}
		}

		if (pOverlay) {
			WRect rc(g_view->GetWidth() * 0.5f + xCur + data_9eb5b8[21].f, g_view->GetHeight() * 0.5f + yCur + 25.0f, (float)pOverlay->GetWidth(), (float)pOverlay->GetHeight());
			if ((typeId >> 26) == 2 || (typeId >> 26) == 0x1D) {
				pOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(rc.x - 11.0f, rc.y - 11.0f, rc.w, rc.h), 0, 0xFFFFFFFF, 0.0f, 0);
			} else {
				pOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), rc, 0, 0xFFFFFFFF, 0.0f, 0);
			}

			int adv = pOverlay->GetWidth();
			if (adv <= 40) {
				adv = 40;
			}
			xCur += adv;
			if (xCur > 132) {
				yCur += 40;
				xCur = 0;
			}
		}
	}
}

// fontinfo.cpp:1347
CRemainTimeInfo::CRemainTimeInfo(void) {
	Init("[font_bar.jpg", NULL);
	m_nFlag = 1;
	m_color = 0xFF147EFF;

	m_pBuffer = new char[0x20];
}

// fontinfo.cpp:1356
void CRemainTimeInfo::Display(void) {
	if (CSharedDoc::Instance()->m_shotTimeLimit == 0) {
		sprintf(m_pBuffer, "-");
		Print(79.0f, COption::Instance()->gGetBar_Y() + 34.0f, m_pBuffer, 1.0f, 0, 1.0f, NULL);
	} else {
		unsigned long sec = GOLFDOC()->m_remainShotTime / 1000;
		if (IsLocalCountry(30)) {
			sprintf(m_pBuffer, "%d", Min(GOLFDOC()->m_remainShotTime > sec * 1000 ? sec + 1 : sec, CSharedDoc::Instance()->m_shotTimeLimit / 1000));
		} else {
			sprintf(m_pBuffer, "%d", Min(GOLFDOC()->m_remainShotTime > sec * 1000 ? sec + 1 : sec, CSharedDoc::Instance()->m_shotTimeLimit / 1000));
		}

		Print(79.0f, COption::Instance()->gGetBar_Y() + 34.0f, m_pBuffer, 1.0f, 0, 1.0f, NULL);
	}
}

// fontinfo.cpp:1381
CTimeOutInfo::CTimeOutInfo(void) {
	Init("[bigfont.jpg", NULL);
	m_pBuffer = new char[0x20];

	m_nFlag = 2;
	m_nState = 3;
	m_fAlpha = 0;
}

// fontinfo.cpp:1391
void CTimeOutInfo::Reset(void) {
	m_nState = 0;
	m_fAlpha = 0;
}

// fontinfo.cpp:1397
void CTimeOutInfo::Process(float fDelta) {
	switch (m_nState) {
	case 2:
		m_fTime += fDelta;
		if (m_fTime > 1.5f) {
			m_nState = 1;
		}
		break;

	case 0:
		m_fAlpha += fDelta * 510.0f;
		if (m_fAlpha > 255.0f) {
			m_fAlpha = 255.0f;
			m_nState = 2;
			m_fTime = 0;
		}
		break;

	case 1:
		m_fAlpha -= fDelta * 510.0f;
		if (m_fAlpha < 0) {
			m_fAlpha = 0;
			m_nState = 3;
		}
		break;

	case 3:
		m_fAlpha = 0;
		break;
	}
}

// fontinfo.cpp:1432
void CTimeOutInfo::Display(void) {
	if (m_nState == 3) {
		return;
	}

	sprintf(m_pBuffer, "%d", 3 - PLAYER(GOLFDOC()->m_myPlayerIndex)->m_numTimeout);

	SetColor(((int)m_fAlpha << 24) | 0xFF0000);
	Print(g_view->GetWidth() * 0.5f - 88.0f, g_view->GetHeight() * 0.25f + 3.0f, m_pBuffer, 1.0f, 0, 1.0f, NULL);
}

// fontinfo.cpp:1455
CTotalDistInfo::CTotalDistInfo(void) {
	Init("[font_hole.jpg", NULL);
	m_pBuffer = new char[0x20];

	m_nFlag = 1;
}

// fontinfo.cpp:1462
void CTotalDistInfo::Display(void) {
	WVector cup = GOLFDOC()->GetHoleData().m_cupPos;
	const WVector &ball = GolfBall().m_pos;

	float dx = cup.x - ball.x;
	float dz = cup.z - ball.z;
	m_fDistance = sqrtf(dz * dz + dx * dx + 0.0f) * 0.3125f;

	if (m_fDistance < 1.0f)
		sprintf(m_pBuffer, "%.1fy", m_fDistance);
	else
		sprintf(m_pBuffer, "%.0fy", m_fDistance);

	Print(34.0f, COption::Instance()->gGetBar_Y() + 8.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);
}

// fontinfo.cpp:1481
// Through a const variable: the original initializes it dynamically (docs/tough_problems/010).
static const float FLOAT_RATE = 30.0f;
static float s_fFloatInterval = 1.0f / FLOAT_RATE;

// fontinfo.cpp:1485
CShotCountInfo::CShotCountInfo(void) {
	Init("[font_hole.jpg", NULL);

	m_pBuffer = new char[0x20];
	m_nFlag = 1;

	if (IsGameType(7)) {

		Init("[font_hole_skins_red.jpg", &m_redFont);
		Init("[font_hole_skins_grey.jpg", &m_greyFont);
		Init("[font_hole_skins_blue.jpg", &m_blueFont);

		m_pCoinOverlay = g_resrcmng->GetOverlay("coin_pang.tga", 0);
	}
}

// fontinfo.cpp:1502
CShotCountInfo::~CShotCountInfo() {
	if (IsGameType(7)) {
		if (g_resrcmng && m_pCoinOverlay) {
			g_resrcmng->Release(m_pCoinOverlay);
			m_pCoinOverlay = NULL;
		}
	}
}

// fontinfo.cpp:1508
void CShotCountInfo::StartFloating(void) {
	m_fFloatTime = s_fFloatInterval;
	m_nFloatCount = 0;
	m_alpha = 255;
}

// fontinfo.cpp:1515
void CShotCountInfo::Process(float fDelta) {
	if (m_fFloatTime <= 0) {
		return;
	}

	m_fFloatTime -= fDelta;
	if (m_fFloatTime < 0) {
		m_nFloatCount++;
		if (m_nFloatCount == 60) {
			m_fFloatTime = 0;
		} else {
			m_alpha -= 4;
			m_fFloatTime = s_fFloatInterval;
		}
	}
}

// fontinfo.cpp:1537
void CShotCountInfo::Display(float x, float y, unsigned char nPlayer) {
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
		sprintf(m_pBuffer, "%d", (unsigned char)GOLFDOC()->GetTeam(nPlayer)->m_holes[GOLFDOC()->m_currentHole - 1]);
	} else {
		sprintf(m_pBuffer, "%d", (unsigned char)GOLFDOC()->GetPlayer(nPlayer)->m_holes[GOLFDOC()->m_currentHole]);
	}

	if (IsLocalContent((localContentType_t)0x4F) && WSingleton<CSharedDoc>::Instance()->m_playingMode == 2) {
		// The original first stores CSharedDoc 0x5BD0 (protected in SharedDoc.h) into
		// GetPlayer(nPlayer)->m_holes[GOLFDOC()->m_currentHole]: needs that member public.
		GOLFDOC()->GetPlayer(nPlayer)->m_ghostResult = 0;
		sprintf(m_pBuffer, "%d", (unsigned char)GOLFDOC()->GetPlayer(nPlayer)->m_holes[GOLFDOC()->m_currentHole]);
	}
	Print(x, y, m_pBuffer, 1.0f, 0, 1.0f, NULL);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
		sprintf(m_pBuffer, "%d", GOLFDOC()->GetTeam(nPlayer)->m_resetCount);
	} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
		sprintf(m_pBuffer, "%d", GUILDMATCHUP(nPlayer)->m_uniteGuildIndex);
	} else {
		sprintf(m_pBuffer, "%d", GOLFDOC()->GetPlayer(nPlayer)->m_ghostResult);
	}

	Print(x, y + 16.0f, m_pBuffer, 1.0f, 0, 1.0f, NULL);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		float fOffset = (nPlayer & 2) ? -48.0f : 0.0f;

		if (m_pCoinOverlay) {
			m_pCoinOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(x - 52.0f + fOffset, y + 32.0f, 32, 32), 0, m_color, 0.0f, 0);
		}

		char szPang[32] = "";

		__int64 pang = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[nPlayer].m_pang;
		MakePriceStrA(szPang, pang < 0 ? -pang : pang);
		if (pang < 0) {
			sprintf(m_pBuffer, "-%s", szPang);
		} else {
			strcpy(m_pBuffer, szPang);
		}
		Print(x - 22.0f + fOffset, y + 39.0f, m_pBuffer, 0.0f, 0, 1.0f, NULL);

		__int64 reward = GOLFDOC()->GetPlayer(nPlayer)->m_rewardPang;
		MakePriceStrA(szPang, reward < 0 ? -reward : reward);

		float fX = ((nPlayer & 2) ? -108.0f : 0.0f) + x + 20.0f;
		float fY = y + 29.0f;

		if (GOLFDOC()->GameMode() & 0x1000) {
			if (GOLFDOC()->GetPlayer(nPlayer)->m_rewardPang > 0) {
				sprintf(m_pBuffer, "+%s", szPang);
				m_color = ((255 - m_alpha) << 24) | 0xFFFFFF;
				Print(fX, (float)(60 - m_nFloatCount) + fY, m_pBuffer, 0.0f, 0, 1.0f, &m_blueFont);
			} else if (GOLFDOC()->GetPlayer(nPlayer)->m_rewardPang < 0) {
				m_color = (m_alpha << 24) | 0xFFFFFF;
				sprintf(m_pBuffer, "-%s", szPang);
				Print(fX, fY - (float)m_nFloatCount, m_pBuffer, 0.0f, 0, 1.0f, &m_redFont);
			}
		}
	}
}

// fontinfo.cpp:1622
CPowerInfo::CPowerInfo(void) {
	Init("[font_bar.jpg", NULL);
	m_pBuffer = new char[0x20];

	m_nFlag = 1;
}

// fontinfo.cpp:1629
void CPowerInfo::Display(void) {
	m_color = (GOLFDOC()->GetUIAlpha() << 24) | 0xFFFFFF;

	const WVector &ball = GolfBall().m_pos;

	float dx = m_vPos.x - ball.x;
	float dz = m_vPos.z - ball.z;
	float fDistance = sqrtf(dz * dz + dx * dx) * 0.3125f;

	sprintf(m_pBuffer, "%.0fy", fDistance);
	Print(data_af23d0 + data_9eb5b8[28].f - 3.0f, COption::Instance()->gGetBar_Y() + data_9eb5b8[27].f, m_pBuffer, 1.0f, 0, 1.0f, NULL);
	sprintf(m_pBuffer, "%.0fy", fDistance * 0.5f);
	Print(data_af23d0 * 0.5f + 143.0f, COption::Instance()->gGetBar_Y() + data_9eb5b8[27].f, m_pBuffer, 1.0f, 0, 1.0f, NULL);
}

// fontinfo.cpp:1649
CClubTypeInfo::CClubTypeInfo(void) {
	Init("[font_club.jpg", NULL);
	m_pBuffer = new char[0x20];
}

// fontinfo.cpp:1655
void CClubTypeInfo::Display(void) {
	sprintf(m_pBuffer, "%x", GolfClub().GetID() > 15 ? 15 : GolfClub().GetID());
	Print(6.0f, COption::Instance()->gGetBar_Y() - 51.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);
}

// fontinfo.cpp:1667
CAccuracyInfo::CAccuracyInfo(void) : m_pImpactUI(NULL), m_pBuffer2(NULL) {
	Init("[font_bar.jpg", NULL);
	m_pImpactUI = new CSimpleUI("[impact.png");

	m_bFlicker = false;
	m_pImpactUI->Add("@Bad", WRect(0.0f, 0.0f, 128.0f, 18.0f), false);
	m_pImpactUI->Add("@Pangya", WRect(0.0f, 18.0f, 128.0f, 18.0f), false);
	m_pImpactUI->Add("@PowerSpin", WRect(0.0f, 36.0f, 128.0f, 18.0f), false);
	m_pImpactUI->Add("@PowerCurve", WRect(0.0f, 54.0f, 128.0f, 18.0f), false);
	m_pImpactUI->Add("@Tomahawk", WRect(0.0f, 72.0f, 128.0f, 18.0f), false);
	m_pImpactUI->Add("@Full", WRect(0.0f, 90.0f, 128.0f, 18.0f), false);
	m_pImpactUI->Add("@SuperPangya", WRect(0.0f, 108.0f, 128.0f, 18.0f), false);
	m_pImpactUI->Add("@Cobra", WRect(0.0f, 126.0f, 128.0f, 18.0f), false);

	m_pImpactUI->Add("@Spike", WRect(0.0f, 144.0f, 128.0f, 18.0f), false);
	m_pImpactUI->Add("@PowerTomahawk", WRect(0.0f, 162.0f, 128.0f, 18.0f), false);
	m_pImpactUI->Add("@PowerCobra", WRect(0.0f, 180.0f, 128.0f, 18.0f), false);
	m_pImpactUI->Add("@PowerSpike", WRect(0.0f, 198.0f, 128.0f, 18.0f), false);

	m_pBuffer2 = NULL;
	m_pBuffer = NULL;
	m_pBuffer = new char[0x20];
	m_pBuffer[0] = 0;
	m_pBuffer2 = new char[0x20];
	m_pBuffer2[0] = 0;
	m_fTextX = 0;
	m_nFlag = 1;
}

// fontinfo.cpp:1703
CAccuracyInfo::~CAccuracyInfo() {
	if (m_pImpactUI) {
		delete m_pImpactUI;
		m_pImpactUI = NULL;
	}
	if (m_pBuffer) {
		delete[] m_pBuffer;
		m_pBuffer = NULL;
	}
	if (m_pBuffer2) {
		delete[] m_pBuffer2;
		m_pBuffer2 = NULL;
	}
}

// fontinfo.cpp:1710
void CAccuracyInfo::Process(float fDelta) {
	if (m_bFlicker) {
		m_fAlpha -= fDelta * 128.0f;
		if (m_fAlpha < 0) {
			m_fAlpha = 0;
			m_bFlicker = false;
		}
	}
}

// fontinfo.cpp:1723
void CAccuracyInfo::SetPower(void) {
	m_bFlicker = true;
	m_fAlpha = 255.0f;
	sprintf(Buffer(), "@Full");

	m_fTextX = 436.0f;
}

// fontinfo.cpp:1736
void CAccuracyInfo::SetAccuracy(void) {
	m_bFlicker = true;
	m_fAlpha = 255.0f;
	m_fTextX = GOLFDOC()->GetBar().m_position - 64.0f;

	*Buffer() = 0;
	*Buffer2() = 0;

	switch (GOLFDOC()->GetBar().m_accuracyGrade) {
	case 2:
	case 3:
	case 4:
		if (!IsLocalContent((localContentType_t)0x50)) {
			if (GOLFDOC()->GetBar().m_accuracyGrade != 4) {
				return;
			}
		}

		if (GolfBall().m_powerFlags & 0x70) {
			if (GolfClub().PowerShotType() == 2 || GolfClub().PowerShotType() == 3) {
				strcpy(Buffer(), "@Power");
			} else {
				strcpy(Buffer(), "@");
			}

			if (GolfBall().m_powerFlags & 0x10) {
				strcat(Buffer(), "Tomahawk");
			} else if (GolfBall().m_powerFlags & 0x20) {
				strcat(Buffer(), "Cobra");
			} else if (GolfBall().m_powerFlags & 0x40) {
				strcat(Buffer(), "Spike");
			}

			if (GolfBall().m_powerFlags & 3) {
				sprintf(Buffer2(), "@PowerSpin");
			} else if (GolfBall().m_powerFlags & 0xC) {
				sprintf(Buffer2(), "@PowerCurve");
			}
		} else if (GolfBall().m_powerFlags & 3) {
			sprintf(Buffer(), "@PowerSpin");
		} else if (GolfBall().m_powerFlags & 0xC) {
			sprintf(Buffer(), "@PowerCurve");
		} else if (IsLocalContent((localContentType_t)0x50)) {
			if (GOLFDOC()->GetBar().m_accuracyGrade != 4) {
				return;
			}

			if (GolfClub().PowerShotType() != 0 && GOLFDOC()->GetBar().m_stopped) {
				sprintf(Buffer(), "@SuperPangya");
			} else {
				sprintf(Buffer(), "@Pangya");
			}
		} else {
			if (GolfClub().PowerShotType() != 0 && GOLFDOC()->GetBar().m_stopped) {
				sprintf(Buffer(), "@SuperPangya");
			} else {
				sprintf(Buffer(), "@Pangya");
			}
		}
		break;

	case 0:
	case 1:
		sprintf(Buffer(), "@Bad");
		break;
	}
}

// fontinfo.cpp:1818
void CAccuracyInfo::Display(void) {
	if (!m_bFlicker || !m_pImpactUI) {
		return;
	}

	if (strchr(m_pBuffer, '@')) {
		if (IsLocalContent((localContentType_t)0x50)) {
			if (stricmp(m_pBuffer, "@Full") != 0 && stricmp(m_pBuffer, "@Bad") != 0 && GOLFDOC()->GetBar().m_accuracyGrade != 4) {
				m_pImpactUI->SetColor(0xFF8A8A8A);
			} else {
				m_pImpactUI->SetColor(((int)m_fAlpha << 24) | 0xFFFFFF);
			}
		} else {
			m_pImpactUI->SetColor(((int)m_fAlpha << 24) | 0xFFFFFF);
		}
		m_pImpactUI->Render(m_pBuffer, m_fTextX, COption::Instance()->gGetBar_Y() - 22.0f, 1.0f, 0);
	}

	if (strchr(m_pBuffer2, '@')) {
		m_pImpactUI->SetColor(((int)m_fAlpha << 24) | 0xFFFFFF);
		m_pImpactUI->Render(m_pBuffer2, m_fTextX, COption::Instance()->gGetBar_Y() - 34.0f, 1.0f, 0);
	}
}

// fontinfo.cpp:1858
CHoleInfo::CHoleInfo(void) : m_pCourseUI(NULL), m_pHoleUI(NULL), m_pStartPosiUI(NULL), m_pPlayerNumUI(NULL), m_pEmotiUI(NULL),
	m_pShuffleBonusOverlay(NULL), m_pCompleteOverlay(NULL), m_pScoreEventOverlay(NULL), m_pExtraStageOverlay(NULL),
	m_pExtraStageSmallOverlay(NULL), m_pCoin1Overlay(NULL), m_pCoin2Overlay(NULL), m_pCoin3Overlay(NULL),
	m_pMapBonusOverlay(NULL), m_pSkinsPang1Overlay(NULL), m_pSkinsPang3Overlay(NULL), m_pApproachOverlay(NULL) {
	Init("[bigfont.jpg", NULL);
	m_pBuffer = new char[0x20];

	m_fTime = 0.0f;
	m_fAlpha = 0.0f;
	m_fTargetAlpha = 0.0f;

	m_items[0].baseX = g_view->GetWidth() * 0.5f - 116.0f;
	m_items[0].baseY = 97.0f;

	m_items[0].curX = -14.0f;
	m_items[0].curY = 8.0f;

	m_items[1].baseX = g_view->GetWidth() * 0.5f - 75.0f;
	m_items[1].baseY = 127.0f;
	m_items[1].curX = 100.0f;
	m_items[1].curY = 7.0f;

	m_items[2].baseX = g_view->GetWidth() * 0.5f - 144.0f;
	m_items[2].baseY = 159.0f;
	m_items[2].curX = 85.0f;
	m_items[2].curY = 30.0f;

	m_items[3].baseX = g_view->GetWidth();
	m_items[3].baseY = 110.0f;

	char szCount[8];
	sprintf(szCount, "%d", (int)CSharedDoc::Instance()->m_gameResultList.size() * 100);
	int len = (int)strlen(szCount);
	m_items[3].curX = g_view->GetWidth() * 0.5f - 50.0f - (m_font.m_pFontInfo->fontw * len * 0.5f + len * 4.0f);
	m_items[3].curY = 5.0f;

	std::map<unsigned int, IFF_STRUCT::sCourse> &courseMap = ItemManager()->m_courseMap;
	m_pCourseUI = new CSimpleUI("[start_name.jpg");

	std::map<unsigned int, IFF_STRUCT::sCourse>::const_iterator it;
	for (it = courseMap.begin(); it != courseMap.end(); ++it) {
		int index = it->second.m_typeId & 0x3FFFFFF;

		WRect rc;
		if (IsLocalContent((localContentType_t)0x26)) {
			if (index >= 14) {
				rc = WRect(256.0f, (float)(index - 14) * 36.0f, 256.0f, 36.0f);
			} else {
				rc = WRect(0.0f, (float)index * 36.0f, 256.0f, 36.0f);
			}
		} else {
			rc = WRect(0.0f, (float)index * 36.0f, 256.0f, 36.0f);
		}

		m_pCourseUI->Add(it->second.m_name, rc, false);
	}

	m_pHoleUI = new CSimpleUI("[start_hole.png");
	m_pHoleUI->Add("Hole", WRect(0.0f, 0.0f, 64.0f, 32.0f), false);
	m_pHoleUI->Add("Par", WRect(0.0f, 32.0f, 64.0f, 32.0f), false);

	if (CSharedDoc::Instance()->m_roomType == 7) {
		m_pHoleUI->Add("Pang", WRect(0.0f, 64.0f, 64.0f, 32.0f), false);
		m_pHoleUI->Add("x2", WRect(0.0f, 96.0f, 64.0f, 32.0f), false);
		m_pHoleUI->Add("x4", WRect(0.0f, 128.0f, 64.0f, 32.0f), false);
		m_pHoleUI->Add("x6", WRect(0.0f, 160.0f, 64.0f, 32.0f), false);
		m_pHoleUI->Add("x8", WRect(0.0f, 192.0f, 64.0f, 32.0f), false);
	}

	if (CSharedDoc::Instance()->m_roomType == 10) {
		m_pApproachOverlay = g_resrcmng->GetOverlay("text_approach.png", 0);
	} else {
		m_pStartPosiUI = new CSimpleUI("[start_posi.jpg");
		m_pPlayerNumUI = new CSimpleUI("[nplayer.jpg");

		if (CSharedDoc::Instance()->m_roomType == 0) {
			m_pStartPosiUI->Add("\xBC\xB1\xB5\xEE", WRect(0.0f, 0.0f, 64.0f, 32.0f), false);

			m_pPlayerNumUI->Add("1", WRect(0.0f, 0.0f, 32.0f, 16.0f), false);
			m_pPlayerNumUI->Add("2", WRect(32.0f, 0.0f, 32.0f, 16.0f), false);
			m_pPlayerNumUI->Add("3", WRect(0.0f, 16.0f, 32.0f, 16.0f), false);
			m_pPlayerNumUI->Add("4", WRect(32.0f, 16.0f, 32.0f, 16.0f), false);
		} else if (CSharedDoc::Instance()->m_roomType == 3) {
			m_pStartPosiUI->Add("vs", WRect(0.0f, 32.0f, 64.0f, 32.0f), false);

			m_pPlayerNumUI->Add("1", WRect(0.0f, 0.0f, 32.0f, 16.0f), false);
			m_pPlayerNumUI->Add("2", WRect(32.0f, 0.0f, 32.0f, 16.0f), false);
		} else {
			m_pStartPosiUI->Add("vs", WRect(0.0f, 32.0f, 64.0f, 32.0f), false);

			m_pPlayerNumUI->Add("red", WRect(0.0f, 32.0f, 32.0f, 16.0f), false);
			m_pPlayerNumUI->Add("blue", WRect(32.0f, 32.0f, 32.0f, 16.0f), false);
		}

		m_pCompleteOverlay = g_resrcmng->GetOverlay("[complete.png", 0);
		m_pScoreEventOverlay = g_resrcmng->GetOverlay("[score_event.png", 0);

		if (CSharedDoc::Instance()->m_roomType == 7) {
			m_pSkinsPang1Overlay = g_resrcmng->GetOverlay("[skins_pang01.png", 0);
			m_pSkinsPang3Overlay = g_resrcmng->GetOverlay("[skins_pang03.png", 0);
			m_pExtraStageOverlay = g_resrcmng->GetOverlay("extrastage.png", 0);
			m_pExtraStageSmallOverlay = g_resrcmng->GetOverlay("extrastagesmall.png", 0);
			m_pCoin1Overlay = g_resrcmng->GetOverlay("P_coin01.png", 0);
			m_pCoin2Overlay = g_resrcmng->GetOverlay("P_coin02.png", 0);
			m_pCoin3Overlay = g_resrcmng->GetOverlay("P_coin03.png", 0);

			m_pEmotiUI = new CSimpleUI("[emoti_object.png");
			m_pEmotiUI->Add("0", WRect(0.0f, 128.0f, 64.0f, 64.0f), false);
			m_pEmotiUI->Add("1", WRect(64.0f, 128.0f, 64.0f, 64.0f), false);
			m_pEmotiUI->Add("2", WRect(128.0f, 128.0f, 64.0f, 64.0f), false);
			m_pEmotiUI->Add("3", WRect(192.0f, 128.0f, 64.0f, 64.0f), false);
			m_pEmotiUI->Add("4", WRect(0.0f, 192.0f, 64.0f, 64.0f), false);
			m_pEmotiUI->Add("5", WRect(64.0f, 192.0f, 64.0f, 64.0f), false);
			m_pEmotiUI->Add("6", WRect(128.0f, 192.0f, 64.0f, 64.0f), false);
			m_pEmotiUI->Add("7", WRect(192.0f, 192.0f, 64.0f, 64.0f), false);

			m_nEmoti = -1;
		}

		m_pShuffleBonusOverlay = g_resrcmng->GetOverlay("text_shufflebonus.png", 0);
		m_pMapBonusOverlay = g_resrcmng->GetOverlay("text_mapbonus.png", 0);
	}
}

// fontinfo.cpp:1997
CHoleInfo::~CHoleInfo() {
	if (m_pCourseUI) { delete m_pCourseUI; m_pCourseUI = NULL; }
	if (m_pHoleUI) { delete m_pHoleUI; m_pHoleUI = NULL; }
	if (m_pStartPosiUI) { delete m_pStartPosiUI; m_pStartPosiUI = NULL; }
	if (m_pPlayerNumUI) { delete m_pPlayerNumUI; m_pPlayerNumUI = NULL; }

	if (g_resrcmng && m_pSkinsPang1Overlay) { g_resrcmng->Release(m_pSkinsPang1Overlay); m_pSkinsPang1Overlay = NULL; }
	if (g_resrcmng && m_pSkinsPang3Overlay) { g_resrcmng->Release(m_pSkinsPang3Overlay); m_pSkinsPang3Overlay = NULL; }
	if (g_resrcmng && m_pApproachOverlay) { g_resrcmng->Release(m_pApproachOverlay); m_pApproachOverlay = NULL; }
	if (g_resrcmng && m_pExtraStageOverlay) { g_resrcmng->Release(m_pExtraStageOverlay); m_pExtraStageOverlay = NULL; }
	if (g_resrcmng && m_pExtraStageSmallOverlay) { g_resrcmng->Release(m_pExtraStageSmallOverlay); m_pExtraStageSmallOverlay = NULL; }
	if (g_resrcmng && m_pCoin1Overlay) { g_resrcmng->Release(m_pCoin1Overlay); m_pCoin1Overlay = NULL; }
	if (g_resrcmng && m_pCoin2Overlay) { g_resrcmng->Release(m_pCoin2Overlay); m_pCoin2Overlay = NULL; }
	if (g_resrcmng && m_pCoin3Overlay) { g_resrcmng->Release(m_pCoin3Overlay); m_pCoin3Overlay = NULL; }
	if (g_resrcmng && m_pShuffleBonusOverlay) { g_resrcmng->Release(m_pShuffleBonusOverlay); m_pShuffleBonusOverlay = NULL; }

	if (m_pEmotiUI) { delete m_pEmotiUI; m_pEmotiUI = NULL; }

	if (g_resrcmng && m_pCompleteOverlay) { g_resrcmng->Release(m_pCompleteOverlay); m_pCompleteOverlay = NULL; }
	if (g_resrcmng && m_pScoreEventOverlay) { g_resrcmng->Release(m_pScoreEventOverlay); m_pScoreEventOverlay = NULL; }

	if (g_resrcmng && m_pMapBonusOverlay) { g_resrcmng->Release(m_pMapBonusOverlay); m_pMapBonusOverlay = NULL; }
}

// fontinfo.cpp:2022
void CHoleInfo::Process(float fDelta) {
	switch (m_nState) {
	case 0: {
		m_fTime += fDelta;

		m_fAlpha += fDelta * 255.0f;
		if (m_fAlpha > 255.0f)
			m_fAlpha = 255.0f;
		m_fTargetAlpha += fDelta * 255.0f;
		if (m_fTargetAlpha > 255.0f)
			m_fTargetAlpha = 255.0f;

		float fWait;
		CSharedDoc *pDoc = CSharedDoc::Instance();
		switch (pDoc->m_roomType) {
		case 4:
		case 5:
		case 6:
		case 9:
		case 10:
			if (GOLFDOC()->m_currentHole != pDoc->m_holeOrder[0]) {
				fWait = 1.0f;
				break;
			}
		default:
			fWait = 4.0f;
			break;
		}
		if (pDoc->m_roomType == 10)
			fWait = 2.0f;

		if (pDoc->m_roomType == 7 && !GOLFDOC()->m_approachMode
			&& GOLFDOC()->m_currentHole == pDoc->m_holeOrder[0]) {
			if (m_nEmoti == -1) {
				if (m_fTime > 1.0f) {
					m_nEmoti = 0;
					g_audio->PlaySfx("play_fail_item", NULL, 0, NULL, NULL, 0.5f, 200.0f);
				}
			} else if (m_nEmoti < 8) {
				m_nEmoti = (int)((m_fTime - 1.0f) * 8.0f);
			}
		}

		if (CSharedDoc::Instance()->m_roomType == 10) {
			float t = m_fTargetAlpha * (1.0f / 255.0f);
			m_items[3].fixY = m_items[3].baseY;
			m_items[3].scale = 1.0f;
			m_items[3].fixX = (1.0f - t) * m_items[3].baseX + t * m_items[3].curX;
		}

		if (m_fTime > fWait) {
			m_fTime = 0.0f;
			m_nState = 1;

			if (CSharedDoc::Instance()->m_roomType == 10) {
				m_items[3].baseX = m_items[3].curX;
				m_items[3].curX = m_items[1].curX + 60.0f;
			}
		}
		break;
	}
	case 1:
		m_fTargetAlpha -= fDelta * 510.0f;
		if (m_fTargetAlpha < 0.0f)
			m_fTargetAlpha = 0.0f;

		m_fTime += fDelta * 1.5f;
		if (m_fTime > 1.0f) {
			m_nState = 2;
			m_fTime = 1.0f;
		}

		m_items[0].fixX = (1.0f - m_fTime) * m_items[0].baseX + m_items[0].curX * m_fTime;
		m_items[0].fixY = (1.0f - m_fTime) * m_items[0].baseY + m_items[0].curY * m_fTime;
		m_items[0].scale = (1.0f - m_fTime) + m_fTime * 0.5f;
		m_items[1].fixX = (1.0f - m_fTime) * m_items[1].baseX + m_items[1].curX * m_fTime;
		m_items[1].fixY = (1.0f - m_fTime) * m_items[1].baseY + m_items[1].curY * m_fTime;
		m_items[1].scale = (1.0f - m_fTime) + m_fTime * 0.6f;
		m_items[2].fixX = (1.0f - m_fTime) * m_items[2].baseX + m_items[2].curX * m_fTime;
		m_items[2].fixY = (1.0f - m_fTime) * m_items[2].baseY + m_items[2].curY * m_fTime;
		m_items[2].scale = (1.0f - m_fTime) + m_fTime * 0.6f;
		m_items[3].fixX = (1.0f - m_fTime) * m_items[3].baseX + m_items[3].curX * m_fTime;
		m_items[3].fixY = (1.0f - m_fTime) * m_items[3].baseY + m_items[3].curY * m_fTime;
		m_items[3].scale = (1.0f - m_fTime) + m_fTime * 0.6f;
		break;

	case 2:
		m_fAlpha -= fDelta * 128.0f;
		if (m_fAlpha < 178.5f) {
			m_fAlpha = 178.5f;
			m_nState = 3;
		}
		break;

	case 4:
		m_fTime += fDelta;
		if (m_fTime > 0.25f) {
			m_fTime -= 0.25f;
			m_fAlpha = 255.0f - m_fAlpha;
			if (m_fAlpha > 0.0f) {
				m_nFlickerTime++;
				if (m_nFlickerTime == 3)
					m_nState = 3;
			}
		}
		break;
	}
}

// fontinfo.cpp:2137
void CHoleInfo::Flicker(void) {
	m_nState = 4;
	m_nFlickerTime = 0;
}

// fontinfo.cpp:2143
void CHoleInfo::Reset(void) {
	m_fTime = 0.0f;
	m_nState = 0;
	m_fAlpha = 0.0f;
	m_fTargetAlpha = 0.0f;

	m_items[0].fixX = m_items[0].baseX;
	m_items[0].fixY = m_items[0].baseY;
	m_items[0].scale = 1.0f;

	m_items[1].fixX = m_items[1].baseX;
	m_items[1].fixY = m_items[1].baseY;
	m_items[1].scale = 1.0f;

	m_items[2].fixX = m_items[2].baseX;
	m_items[2].fixY = m_items[2].baseY;
	m_items[2].scale = 1.0f;

	m_firstPlayer = 0;

	for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (GOLFDOC()->GetPlayer(i)->m_state == 3)
			continue;

		if (GOLFDOC()->GetPlayer(i)->m_turnOrder == 1) {
			m_firstPlayer = i;
			break;
		}
	}
}

// fontinfo.cpp:2178
void CHoleInfo::Fix(void) {
	m_fAlpha = 255.0f;
	m_fTargetAlpha = 255.0f;
	m_items[0].fixY = m_items[0].curY;
	m_items[0].fixX = m_items[0].curX;
	m_items[1].fixY = m_items[1].curY;
	m_items[1].fixX = m_items[1].curX;
	m_items[2].fixX = m_items[2].curX;
	m_fTime = 0.0f;
	m_nState = 3;
	m_items[0].scale = 0.5f;
	m_items[1].scale = 0.6f;
	m_items[2].fixY = m_items[2].curY;
	m_items[2].scale = 0.6f;
}

// fontinfo.cpp:2199
void CHoleInfo::Display(void) {
	if (m_fAlpha > 0.0f) {
		if (GOLFDOC()->m_approachMode) {
			if (CSharedDoc::Instance()->m_roomType == 10 || (m_nState != 1 && m_nState != 0)) {
				IFF_STRUCT::sCourse *pCourse = CSharedDoc::Instance()->m_pCurCourse;

				if (CSharedDoc::Instance()->m_roomType == 10) {
					if (m_pCourseUI) {
						m_pCourseUI->SetColor(((int)m_fAlpha << 24) | 0xFFFFFF);
						m_pCourseUI->Render(pCourse->m_name, m_items[0].curX, m_items[0].curY, 0.5f, 0);
					}

					if (m_pHoleUI) {
						m_pHoleUI->SetColor(((int)m_fAlpha << 24) | 0xFFFFFF);
						m_pHoleUI->Render("Hole", m_items[1].curX, m_items[1].curY, 0.6f, 0);
					}

					SetColor(((int)m_fAlpha << 24) | 0xFFFFFF);

					sprintf(m_pBuffer, "%d/%d", GetHoleIndex(GOLFDOC()->m_currentHole), CSharedDoc::Instance()->m_holes);
					Print(m_items[1].curX + 39.0f, m_items[1].curY + 2.4f, m_pBuffer, 2.0f, 0, 0.6f, NULL);
				} else {
					if (m_pCourseUI) {
						m_pCourseUI->SetColor(((int)m_fAlpha << 24) | 0xFFFFFF);
						m_pCourseUI->Render(pCourse->m_name, m_items[0].fixX, m_items[0].fixY, m_items[0].scale, 0);
					}

					if (m_pExtraStageSmallOverlay) {
						m_pExtraStageSmallOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(m_items[1].fixX, m_items[1].fixY, 128.0f, 32.0f), 0, 0xFFFFFFFF, 0.0f, 0);
					}
				}
			}
		} else {
			IFF_STRUCT::sCourse *pCourse = CSharedDoc::Instance()->m_pCurCourse;
			if (m_pCourseUI) {
				m_pCourseUI->SetColor(((int)m_fAlpha << 24) | 0xFFFFFF);
				m_pCourseUI->Render(pCourse->m_name, m_items[0].fixX, m_items[0].fixY, m_items[0].scale, 0);
			}

			if (m_pHoleUI) {
				m_pHoleUI->SetColor(((int)m_fAlpha << 24) | 0xFFFFFF);
				m_pHoleUI->Render("Hole", m_items[1].fixX, m_items[1].fixY, m_items[1].scale, 0);
				m_pHoleUI->Render("Par", m_items[1].scale * 105.0f + m_items[1].fixX, m_items[1].fixY, m_items[1].scale, 0);
			}

			SetColor(((int)m_fAlpha << 24) | 0xFFFFFF);

			sprintf(m_pBuffer, "%d", GOLFDOC()->m_currentHole);
			Print(m_items[1].scale * 65.0f + m_items[1].fixX, m_items[1].scale * 4.0f + m_items[1].fixY, m_pBuffer, 2.0f, 0, m_items[1].scale, NULL);

			sprintf(m_pBuffer, "/");
			Print(m_items[1].scale * 96.0f + m_items[1].fixX, m_items[1].scale * 4.0f + m_items[1].fixY, m_pBuffer, 2.0f, 0, m_items[1].scale, NULL);

			sprintf(m_pBuffer, "%d", GOLFDOC()->GetHoleData().m_par);
			Print(m_items[1].scale * 160.0f + m_items[1].fixX, m_items[1].scale * 4.0f + m_items[1].fixY, m_pBuffer, 2.0f, 0, m_items[1].scale, NULL);
		}

		if (CSharedDoc::Instance()->m_roomType == 7) {
			sprintf(m_pBuffer, "%I64d", GOLFDOC()->m_skinsPangTotal);
			float fWidth = GetTextWidth(m_pBuffer, 1.0f, m_items[2].scale);

			SetColor(((int)m_fAlpha << 24) | 0xFFDE44);
			Print(m_items[2].scale * 118.0f + m_items[2].fixX, m_items[2].scale * 4.0f + m_items[2].fixY, m_pBuffer, 2.0f, 0, m_items[2].scale, NULL);

			if (GOLFDOC()->m_skinsCarryCount > 1 && m_pHoleUI) {
				m_pHoleUI->Render(MakeStr("x%d", GOLFDOC()->m_skinsCarryCount), m_items[2].scale * 218.0f + m_items[2].fixX + fWidth, m_items[2].fixY, m_items[2].scale, 0);
			}

			if (m_nState == 3) {
				if (GOLFDOC()->m_skinsCarryCount > 1) {
					if (m_pHoleUI) {
						m_pHoleUI->Render(MakeStr("x%d", GOLFDOC()->m_skinsCarryCount), m_items[2].scale * 118.0f + m_items[2].fixX + fWidth, m_items[2].fixY, m_items[2].scale, 0);
					}
					fWidth = m_items[2].scale * 32.0f + fWidth + 3.0f;
				}

				if (m_pHoleUI) {
					m_pHoleUI->Render("Pang", m_items[2].scale * 118.0f + m_items[2].fixX + fWidth, m_items[2].fixY, m_items[2].scale, 0);
				}
			}
		}
	}

	if (CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK) {
		return;
	}

	if (m_fTargetAlpha > 0.0f && GOLFDOC()->m_gameState == 15) {
		SetColor(((int)m_fTargetAlpha << 24) + 0xFFFFFF);

		if (m_nState != 3 && CSharedDoc::Instance()->m_gameMode == 0) {
			if (CSharedDoc::Instance()->m_roomType == 7) {
				if (GOLFDOC()->m_approachMode && m_pExtraStageOverlay) {
					m_pExtraStageOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(g_view->GetWidth() * 0.5f - 110.0f, 97.0f, 256.0f, 64.0f), 0, ((int)m_fTargetAlpha << 24) | 0xFFFFFF, 0.0f, 0);
				}

				if (m_pSkinsPang1Overlay) {
					m_pSkinsPang1Overlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(g_view->GetWidth() * 0.5f - 169.0f, 143.0f, 256.0f, 64.0f), 0, ((int)m_fTargetAlpha << 24) | 0xFFFFFF, 0.0f, 0);
				}

				sprintf(m_pBuffer, "%I64d", GOLFDOC()->m_skinsPangTotal);
				float fWidth = GetTextWidth(m_pBuffer, 1.0f, m_items[2].scale);
				float fX = m_items[2].scale * 118.0f + m_items[2].fixX;
				if (GOLFDOC()->m_skinsCarryCount > 1) {
					fWidth += 32.0f;
				}

				if (m_pSkinsPang3Overlay) {
					m_pSkinsPang3Overlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(fX + fWidth, 143.0f, 256.0f, 64.0f), 0, ((int)m_fTargetAlpha << 24) | 0xFFFFFF, 0.0f, 0);
				}

				// TODO fontinfo.cpp:2353-2381: if (!GOLFDOC()->m_approachMode && m_currentHole == m_holeOrder[0])
				// { five coins (m_pCoin1Overlay for i < <CSharedDoc 0x5BC> % 5 + (m_nEmoti >= 4), else m_pCoin2Overlay)
				// at ((w - 250) * 0.5 + i * 50, 250, 64, 64); m_nEmoti 0..7: m_pEmotiUI->Render(MakeStr("%d", <0x5BC> % 5), ...);
				// m_nEmoti == -1 && <0x5BC> == 5: m_pCoin3Overlay at ((w - 250) * 0.5 + 55, 265, 256, 32) }.
				// Needs CSharedDoc 0x5BC (unsigned long, in PADDING(0x5AC, 0x5C6)) in SharedDoc.h.
			} else if (CSharedDoc::Instance()->m_roomType == 10) {
				if (GOLFDOC()->m_currentHole == CSharedDoc::Instance()->m_holeOrder[0]) {
					if (m_pApproachOverlay) {
						m_pApproachOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(g_view->GetWidth() * 0.5f - 128.0f, 143.0f, 256.0f, 128.0f), 0, ((int)m_fTargetAlpha << 24) | 0xFFFFFF, 0.0f, 0);
					}
				}
			}

			if (!GOLFDOC()->m_approachMode && GOLFDOC()->m_currentHole == CSharedDoc::Instance()->m_holeOrder[0]) {
				m_nFlag = 0;
				if (m_pCompleteOverlay) {
					m_pCompleteOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(g_view->GetWidth() * 0.5f - 106.0f, 202.0f, 256.0f, 32.0f), 0, ((int)m_fTargetAlpha << 24) | 0xFFFFFF, 0.0f, 0);
				}

				// TODO fontinfo.cpp:2402: sprintf(m_pBuffer, "%d", CSharedDoc::Instance()->m_roomPlayers[IsMassGame() ? 0 : MyIndex()].<sUserInfo 0x1C0>);
				// sUserInfo 0x1C0 (int) is in PADDING(0x1B0, 0x1FA) in SharedDoc.h.

				Print(g_view->GetWidth() * 0.5f + 100.0f, 202.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);

				if (CSharedDoc::Instance()->m_holes == 18 && CSharedDoc::Instance()->m_roomInfo.m_roomMode != 14
					&& (CSharedDoc::Instance()->m_roomType == 0 || IsMassGame())) {
					if (m_pScoreEventOverlay) {
						m_pScoreEventOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(g_view->GetWidth() * 0.5f - 106.0f, 242.0f, 256.0f, 72.0f), 0, ((int)m_fTargetAlpha << 24) | 0xFFFFFF, 0.0f, 0);
					}

					// TODO fontinfo.cpp:2428-2433: int score = IsClassicServer(GetServerProperty())
					// ? m_myMapStatistics2[GetCurMap()].m_bestScore : m_myMapStatistics[GetCurMap()].m_bestScore;
					// Print(w * 0.5f + 100.0f, 242.0f, score != 0x7F ? MakeStr("%d", score) : "-", 3.0f, 0, 1.0f, NULL);
					// Needs CSharedDoc::m_myMapStatistics/2 reachable (protected; friend class CHoleInfo in SharedDoc.h).
				}
			}
		}

		CShuffleBonus::Instance()->Render(m_pShuffleBonusOverlay, m_fTargetAlpha);

		if ((!GOLFDOC()->m_approachMode && GOLFDOC()->m_currentHole == CSharedDoc::Instance()->m_holeOrder[0])
			|| CSharedDoc::Instance()->m_roomType == 4 || GalleryMode()) {
			return;
		}

		if (m_pStartPosiUI) {
			m_pStartPosiUI->SetColor(((int)m_fTargetAlpha << 24) | 0xFFFFFF);
		}

		switch (CSharedDoc::Instance()->m_roomType) {
		case 0:
			if (m_pStartPosiUI) {
				m_pStartPosiUI->Render("\xbc\xb1\xb5\xce", g_view->GetWidth() * 0.5f - 45.0f, 160.0f, 1.0f, 0);
			}

			if (m_pPlayerNumUI) {
				sPlayerData *pPlayer = PLAYER(m_firstPlayer);
				m_pPlayerNumUI->SetColor(((int)m_fTargetAlpha << 24) | pPlayer->m_color);
				m_pPlayerNumUI->Render(MakeStr("%d", m_firstPlayer + 1), g_view->GetWidth() * 0.5f + 35.0f, 164.0f, 1.0f, 0);
			}
			break;

		case 3: {
			if (m_pStartPosiUI) {
				m_pStartPosiUI->Render("vs", g_view->GetWidth() * 0.5f + 15.0f - 32.0f, 161.0f, 1.0f, 0);

				sPlayerData *pPlayer = PLAYER(0);
				m_pPlayerNumUI->SetColor(((int)m_fTargetAlpha << 24) | pPlayer->m_color);
				m_pPlayerNumUI->Render("1", g_view->GetWidth() * 0.5f + 15.0f - 32.0f - 60.0f, 168.0f, 1.0f, 0);

				pPlayer = PLAYER(1);
				m_pPlayerNumUI->SetColor(((int)m_fTargetAlpha << 24) | pPlayer->m_color);
				m_pPlayerNumUI->Render("2", g_view->GetWidth() * 0.5f + 68.0f, 168.0f, 1.0f, 0);
			}

			sprintf(m_pBuffer, "%d", PLAYER(0)->m_ghostResult);
			Print(g_view->GetWidth() * 0.5f + 15.0f - 32.0f - 15.0f, 164.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);

			sprintf(m_pBuffer, "%d", PLAYER(1)->m_ghostResult);
			Print(g_view->GetWidth() * 0.5f + 47.0f - 5.0f, 164.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);
			break;
		}

		case 1:
			if (m_pStartPosiUI) {
				m_pStartPosiUI->Render("vs", g_view->GetWidth() * 0.5f + 15.0f - 32.0f, 161.0f, 1.0f, 0);

				m_pPlayerNumUI->SetColor(((int)m_fTargetAlpha << 24) | 0xFFFFFF);
				m_pPlayerNumUI->Render("red", g_view->GetWidth() * 0.5f + 15.0f - 32.0f - 50.0f, 168.0f, 1.0f, 0);
				m_pPlayerNumUI->Render("blue", g_view->GetWidth() * 0.5f + 68.0f, 168.0f, 1.0f, 0);
			}

			sprintf(m_pBuffer, "%d", TEAM(0)->m_resetCount);
			Print(g_view->GetWidth() * 0.5f + 15.0f - 32.0f - 15.0f, 164.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);

			sprintf(m_pBuffer, "%d", TEAM(1)->m_resetCount);
			Print(g_view->GetWidth() * 0.5f + 47.0f - 5.0f, 164.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);
			break;

		case 5: {
			if (m_nState == 3) {
				break;
			}

			SetColor(((int)m_fTargetAlpha << 24) | 0xFFFFFF);

			if (m_pStartPosiUI) {
				m_pStartPosiUI->Render("vs", g_view->GetWidth() * 0.5f + 15.0f - 32.0f, 161.0f, 1.0f, 0);
			}

			if (m_pPlayerNumUI) {
				m_pPlayerNumUI->SetColor(((int)m_fTargetAlpha << 24) | 0xFFFFFF);
			}

			float fRedWidth = m_font.m_pTitleFont->GetTextWidth(g_view, MakeStr("%d", CSharedDoc::Instance()->m_teamResult[0].m_score));

			if (m_pPlayerNumUI) {
				m_pPlayerNumUI->Render("red", g_view->GetWidth() * 0.5f + 15.0f - 32.0f - 45.0f - fRedWidth, 168.0f, 1.0f, 0);
			}

			sprintf(m_pBuffer, "%d", CSharedDoc::Instance()->m_teamResult[0].m_score);
			Print(g_view->GetWidth() * 0.5f + 15.0f - 32.0f - 5.0f - fRedWidth, 164.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);

			float fBlueWidth = m_font.m_pTitleFont->GetTextWidth(g_view, MakeStr("%d", CSharedDoc::Instance()->m_teamResult[1].m_score));

			if (m_pPlayerNumUI) {
				m_pPlayerNumUI->Render("blue", g_view->GetWidth() * 0.5f + fBlueWidth + 57.0f, 168.0f, 1.0f, 0);
			}

			sprintf(m_pBuffer, "%d", CSharedDoc::Instance()->m_teamResult[1].m_score);
			Print(g_view->GetWidth() * 0.5f + 47.0f - 5.0f, 164.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);
			break;
		}
		}
	}
}

// fontinfo.cpp:2542
void CHoleInfo::ResetUiPosition(void) {
	m_items[0].baseX = g_view->GetWidth() * 0.5f - 116.0f;
	m_items[0].baseY = 97.0f;
	m_items[0].curX = m_items[0].fixX = -14.0f;
	m_items[0].curY = m_items[0].fixY = 8.0f;

	m_items[1].baseX = g_view->GetWidth() * 0.5f - 75.0f;
	m_items[1].baseY = 127.0f;
	m_items[1].curX = m_items[1].fixX = 100.0f;
	m_items[1].curY = m_items[1].fixY = 6.0f;
}

// The pang overlays, loaded by the constructor into m_pPangOverlay.
const char *g_pangOverlayFilenames[2] = {
	"[pang_bonus.jpg",
	"[pang_bonus2.jpg",
};

// The image of each case (m_cases).
PangOverlayInfo g_pangOverlayInfo[24] = {
	{ 0, 0, 0 },
	{ 0, 1, 0 },
	{ 0, 2, 0 },
	{ 0, 3, 0 },
	{ 0, 4, 0 },
	{ 0, 5, 0 },
	{ 0, 6, 0 },
	{ 0, 7, 0 },
	{ 0, 8, 0 },
	{ 0, 0, 1 },
	{ 0, 1, 1 },
	{ 0, 2, 1 },
	{ 0, 3, 1 },
	{ 0, 4, 1 },
	{ 0, 5, 1 },
	{ 0, 6, 1 },
	{ 0, 7, 1 },
	{ 0, 8, 1 },
	{ 0, 9, 1 },
	{ 0, 10, 1 },
	{ 0, 11, 1 },
	{ 0, 12, 1 },
	{ 1, 3, 1 },
	{ 1, 3, 0 },
};

// The image of each player's total (m_totalCases).
PangOverlayInfo g_totalPangOverlayInfo[4] = {
	{ 0, 15, 1 },
	{ 0, 14, 1 },
	{ 1, 4, 1 },
	{ 1, 4, 0 },
};

// fontinfo.cpp:2684
CPangInfo::CPangInfo(void) {
	Init("[bigfont.jpg", NULL);

	m_color = 0xFFFFFFFF;
	m_nFlag = 2;
	m_pBuffer = new char[0x20];

	int i;
	for (i = 0; i < sizeof(g_pangOverlayFilenames) / sizeof(g_pangOverlayFilenames[0]); i++) {
		m_pPangOverlay[i] = g_resrcmng->GetOverlay(g_pangOverlayFilenames[i], 0);
	}

	for (i = 0; i < 24; i++) {
		m_cases[i].m_nOverlay = g_pangOverlayInfo[i].m_overlay;
		m_cases[i].m_rect = WRect(g_pangOverlayInfo[i].m_width * 0.5f, g_pangOverlayInfo[i].m_height * 0.0625f, 0.5f, 0.0625f);
	}

	m_nTotalOverlay = 0;
	m_totalRect = WRect(0.5f, 0.8125f, 0.5f, 0.0625f);

	for (i = 0; i < 4; i++) {
		m_totalCases[i].m_pang = 0;
		m_totalCases[i].m_nIndex = -1;
		m_totalCases[i].m_nOverlay = g_totalPangOverlayInfo[i].m_overlay;
		m_totalCases[i].m_rect = WRect(g_totalPangOverlayInfo[i].m_width * 0.5f, g_totalPangOverlayInfo[i].m_height * 0.0625f, 0.5f, 0.0625f);
		m_totalCases[i].m_byAlpha = 0xFF;

		m_pangs[i] = 0;
		m_bonusPangs[i] = 0;
		m_shownPangs[i] = 0;
		m_extraPangs[i] = 0;
	}

	m_flags = 0;
	m_pszSfx = NULL;
	m_fHappyBonusTime = 0;
	m_fAztecBonusTime = 0;
	m_fAngelBonusTime = 0;
	m_fDevilBonusTime = 0;
	m_fFinalizeTime = 0;
	ResetUiPosition();

	m_bReserved = 0;
	memset(m_turnExtraPangs, 0, sizeof(m_turnExtraPangs));

	if (IsLocalContent((localContentType_t)0x32)) {
		m_caddieBonus.SetTicket();
	}

	if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty())) {
		sHoleInPang holeInPangs[20] = {
			{ 158, 158, 93, 55, 37, 0 },
			{ 243, 243, 143, 84, 56, 0 },
			{ 256, 256, 150, 89, 59, 0 },
			{ 351, 351, 207, 122, 81, 0 },
			{ 256, 256, 150, 89, 59, 0 },
			{ 145, 145, 85, 50, 34, 0 },
			{ 243, 243, 143, 84, 56, 0 },
			{ 280, 280, 164, 97, 65, 0 },
			{ 156, 156, 92, 54, 36, 0 },
			{ 256, 256, 150, 89, 59, 0 },
			{ 176, 176, 103, 61, 41, 0 },
			{ 182, 182, 107, 63, 42, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 340, 340, 200, 118, 79, 0 },
			{ 104, 104, 61, 36, 24, 0 },
			{ 100, 100, 59, 35, 23, 0 },
			{ 173, 173, 102, 60, 40, 0 },
			{ 800, 800, 520, 260, 90, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 173, 173, 102, 60, 40, 0 },
		};
		memcpy(m_holeInPangs, holeInPangs, sizeof(holeInPangs));
	} else {
		sHoleInPang holeInPangs[20] = {
			{ 50, 50, 30, 12, 3, 0 },
			{ 100, 100, 60, 30, 11, 0 },
			{ 110, 110, 65, 32, 12, 0 },
			{ 180, 180, 98, 59, 20, 0 },
			{ 150, 150, 85, 43, 16, 0 },
			{ 55, 55, 36, 17, 5, 0 },
			{ 100, 100, 60, 30, 11, 0 },
			{ 150, 150, 85, 43, 16, 0 },
			{ 80, 80, 52, 26, 9, 0 },
			{ 110, 110, 65, 32, 12, 0 },
			{ 80, 80, 52, 26, 9, 0 },
			{ 50, 50, 30, 12, 3, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 180, 180, 98, 59, 20, 0 },
			{ 50, 50, 30, 12, 3, 0 },
			{ 50, 50, 30, 12, 3, 0 },
			{ 80, 80, 52, 26, 9, 0 },
			{ 800, 800, 520, 260, 90, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 80, 80, 52, 26, 9, 0 },
		};
		memcpy(m_holeInPangs, holeInPangs, sizeof(holeInPangs));
	}
}

// fontinfo.cpp:2882
CPangInfo::~CPangInfo() {
	for (int i = 0; i < 2; i++) {
		if (g_resrcmng && m_pPangOverlay[i]) {
			g_resrcmng->Release(m_pPangOverlay[i]);
			m_pPangOverlay[i] = NULL;
		}
	}
}

// fontinfo.cpp:2890
void CPangInfo::ResetUiPosition(void) {
	float fBarY = COption::Instance()->gGetBar_Y() - 108.0f;

	m_totalCases[0].m_fY = fBarY + 23.0f;
	m_totalCases[1].m_fY = fBarY + 23.0f;
	m_totalCases[2].m_fY = fBarY + 23.0f;
	m_totalCases[3].m_fY = fBarY + 23.0f;

	float fWidth = g_view->GetWidth();

	unsigned char roomType = CSharedDoc::Instance()->m_roomType;
	if (roomType == 0 || roomType == 3 || roomType == 7) {
		m_uiPos[0][0] = -25.0f;
		m_uiPos[0][1] = 48.0f;
		m_uiPos[1][0] = -25.0f;
		m_uiPos[1][1] = 141.0f;
		m_uiPos[2][0] = fWidth - 186.0f;
		m_uiPos[2][1] = 48.0f;
		m_uiPos[3][0] = fWidth - 186.0f;
		m_uiPos[3][1] = 141.0f;

		m_uiPos[4][0] = -25.0f;
		m_uiPos[4][1] = 68.0f;
		m_uiPos[5][0] = -25.0f;
		m_uiPos[5][1] = 161.0f;
		m_uiPos[6][0] = fWidth - 186.0f;
		m_uiPos[6][1] = 68.0f;
		m_uiPos[7][0] = fWidth - 186.0f;
		m_uiPos[7][1] = 161.0f;
	} else if (roomType == 1) {
		m_uiPos[0][0] = -25.0f;
		m_uiPos[0][1] = 51.0f;
		m_uiPos[1][0] = -25.0f;
		m_uiPos[1][1] = 100.0f;
		m_uiPos[2][0] = fWidth - 186.0f;
		m_uiPos[2][1] = 51.0f;
		m_uiPos[3][0] = fWidth - 186.0f;
		m_uiPos[3][1] = 100.0f;

		m_uiPos[4][0] = -25.0f;
		m_uiPos[4][1] = 68.0f;
		m_uiPos[5][0] = -25.0f;
		m_uiPos[5][1] = 161.0f;
		m_uiPos[6][0] = fWidth - 186.0f;
		m_uiPos[6][1] = 68.0f;
		m_uiPos[7][0] = fWidth - 186.0f;
		m_uiPos[7][1] = 161.0f;
	}
}

// fontinfo.cpp:2930
void CPangInfo::ChangeTurn(void) {
	if (m_pszSfx) {
		g_audio->StopSfx(m_pszSfx);
	}

	if (CSharedDoc::Instance()->m_roomType == 7 && GOLFDOC()->m_approachMode) {
		m_bonusPangs[0] = m_shownPangs[0] = m_totalCases[0].m_pang = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ghostAward;
		m_bonusPangs[1] = m_shownPangs[1] = m_totalCases[1].m_pang = m_turnExtraPangs[GOLFDOC()->m_myPlayerIndex] + PLAYER(GOLFDOC()->m_myPlayerIndex)->m_extraPang;
	} else {
		m_bonusPangs[0] = m_shownPangs[0] = m_totalCases[0].m_pang = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ghostAward;
		m_bonusPangs[1] = m_shownPangs[1] = m_totalCases[1].m_pang = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_extraPang;
	}

	GroundItemMan *pMan = (GroundItemMan *)AfxGetTask()->GetActor("GroundItemMan");
	if (pMan) {
		m_bonusPangs[2] = m_shownPangs[2] = m_totalCases[2].m_pang = pMan->m_playerStats[GOLFDOC()->m_myPlayerIndex].m_totalCount[0];
		m_bonusPangs[3] = m_shownPangs[3] = m_totalCases[3].m_pang = pMan->m_playerStats[GOLFDOC()->m_myPlayerIndex].m_totalCount[1];
	} else {
		m_bonusPangs[2] = m_shownPangs[2] = m_totalCases[2].m_pang = 0;
		m_bonusPangs[3] = m_shownPangs[3] = m_totalCases[3].m_pang = 0;
	}

	ResetVars(0, 1);
}

// fontinfo.cpp:3057
bool HaveAngelWing(int nPlayer) {
	for (int i = 0; i < 24; i++) {
		switch (WSingleton<CSharedDoc>::Instance()->m_roomPlayers[nPlayer].m_characterInfo.partTypeIds[i]) {
		case 0x8016800:
		case 0x8058800:
		case 0x8098800:
		case 0x80DC800:
		case 0x8118800:
		case 0x8160800:
		case 0x8190800:
		case 0x81E2800:
		case 0x8214800:
		case 0x8254800:
			return true;
		}
	}
	return false;
}

// fontinfo.cpp:3069
int GetAngelBonus(sPlayerData *player) {
	int grade = player->m_angelWingGrade;
	int bonus = 0;

	if (grade <= 5) {
		bonus = 7;
	} else if (grade >= 6 && grade <= 10) {
		bonus = 5;
	} else if (grade >= 11 && grade <= 20) {
		bonus = 3;
	} else if (grade >= 21) {
		bonus = 1;
	}

	return bonus;
}

// fontinfo.cpp:3091
int GetGameClearBonus(void) {
	int nBonus = 0;

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 3) {
		if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty())) {
			for (int i = 0; i < WSingleton<CSharedDoc>::Instance()->m_holes; i++) {
				if (GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]).m_kind == 2) {
					nBonus += S5::CLASSICSRV::GetClearBonus(2, GetCurMap());
				}
			}
			return nBonus;
		}

		int nCount = 0;
		for (int i = 0; i < WSingleton<CSharedDoc>::Instance()->m_holes; i++) {
			if (GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]).m_kind == 2) {
				nCount++;
			}
		}
		return data_9eb5b8[GetCurMap()].i * nCount;
	} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
		if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty())) {
			for (int i = 0; i < WSingleton<CSharedDoc>::Instance()->m_holes; i++) {
				if (GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]).m_kind == 4) {
					nBonus += S5::CLASSICSRV::GetClearBonus(4, GetCurMap());
				}
			}
			return nBonus;
		}

		int nCount = 0;
		int nHoles = GetHoleIndex(GOLFDOC()->m_currentHole);
		for (int i = 0; i < nHoles; i++) {
			if (GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]).m_kind <= 4) {
				nCount++;
			}
		}
		return data_9eb5b8[GetCurMap()].i * nCount;
	} else {
		if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty())) {
			for (int i = 0; i < WSingleton<CSharedDoc>::Instance()->m_holes; i++) {
				sHoleData *pHole = &GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]);
				nBonus += S5::CLASSICSRV::GetClearBonus(pHole->m_kind, GetCurMap());
			}
			return nBonus;
		}

		unsigned char nKind2 = 0;
		unsigned char nKind3 = 0;
		unsigned char nKind4 = 0;
		for (int i = 0; i < WSingleton<CSharedDoc>::Instance()->m_holes; i++) {
			switch (GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]).m_kind) {
			case 2:
				nKind2++;
				break;
			case 3:
				nKind3++;
				break;
			case 4:
				nKind4++;
				break;
			}
		}
		return (nKind2 + nKind3 * 2 + nKind4 * 3) * data_9eb5b8[GetCurMap()].i;
	}
}

// fontinfo.cpp:3179
bool IsEnabledOverDrive(unsigned char byCondition) {
	return byCondition != 19 ? true : false;
}

// fontinfo.cpp:2990
void CPangInfo::Process(float fDelta) {
	if (CSharedDoc::Instance()->m_roomType == 10) {
		return;
	}

	if (GOLFDOC()->m_gameState < 15 && GOLFDOC()->m_gameState != 8) {
		return;
	}

	if (!(GOLFDOC()->GameMode() & 0x81880) || !m_flags) {
		return;
	}

	if ((GOLFDOC()->GameMode() & 0x1000) && CSharedDoc::Instance()->m_roomType != 1 && CSharedDoc::Instance()->m_roomType != 3 && CSharedDoc::Instance()->m_roomType != 7) {
		return;
	}

	UpdateBallState();
	ProcessTotal(fDelta);
	ProcessCases(fDelta);
	CheckReadyToMakeDecision();
	CheckReadyToFinalize(fDelta);

	if (!(m_flags & 8)) {
		RecoveryBonus();
		SuperPangya();
		LongDrive();
		Multiply(1, 2);
		Approach();
		LongPutt();
		LongChipIn();
		BeamImpact();
		SpecialShot();
		NiceRecovery();
		ClashCombo();
		HoleIn();
		Multiply(9, 2);
		Multiply(10, 2);
		DormieBonus();
		HappyBonus();
		AztecBonus();
		AztecBonusWater();
		AngelWingBonus();
		DevilWingBonus();
		if (IsLocalContent((localContentType_t)3)) {
			MascotBonus();
		}
		AbilityItemBonus();
		GimmickItem(GIMMICK_ITEM_PANG_COIN);
		GimmickItem(GIMMICK_ITEM_RANDOM_BOX);
		ClearBonus();
	} else {
		ProcessHappyBonus(fDelta);
		ProcessAztecBonus(fDelta);
		ProcessAngelBonus(fDelta);
		ProcessDevilBonus(fDelta);
	}

	for (int i = 0; i < 24; i++) {
		if (m_cases[i].m_flags & 0x46) {
			m_cases[i].m_fTime += fDelta;
		}
	}

	m_nSubState = GolfBall().m_preDataIndex;
}

// fontinfo.cpp:3197
// Precomputes the pang of the shot just played into m_pangs and the cases'
// states (closed = 0x100), then records them for the local player.
// Residual (92.6%): register allocation only, except the beam-impact
// distance (fontinfo.cpp:3396), whose sqrt the original keeps on the x87
// stack across the test of m_nPangRate and IsHoleIn while ours sinks it
// past them; WVectorLen, a named vector, a double, nested ifs and the
// operand order were ruled out.
void CPangInfo::PreCalc(bool bFlag) {
	CGolfBall &ball = GolfBall();

	ChangeTurn();

	m_flags = 1;

	for (int i = 0; i < 4; i++) {
		m_pangs[i] = 0;
	}

	if (GOLFDOC()->m_approachMode) {
		for (int i = 0; i < 24; i++) {
			m_cases[i].m_flags = 0x100;
		}
		return;
	}

	unsigned char nStroke = CSharedDoc::Instance()->m_roomType != 1
		? PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1]
		: TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1];
	if (IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK) {
		nStroke = (unsigned char)CSharedDoc::Instance()->m_replayStartB;
	}
	int nPar = GOLFDOC()->GetHoleData().m_par;
	int nDiff = nStroke - nPar;
	m_nPangRate = nDiff;

	m_nClubType = GolfClub().GetType();
	m_fLongestDrive = GolfClub().GetRange(NULL);
	m_nStartGround = ball.m_groundType;

	m_aimDir = GetHoleCupPos() - ball.PreData(0)->m_pos;
	m_aimDir.y = 0.0f;
	m_fStartToPin = m_aimDir.Magnitude() * 0.3125f;
	m_aimDir.Normalize();

	WVector diff;
	diff = ball.PreData(0)->m_pos - ball.PreData(ball.m_preDataCount)->m_pos;
	float fDrive = sqrtf(diff.x * diff.x + diff.z * diff.z) * 0.3125f;
	float fDist = (0.0f < fDrive ? fDrive : 0.0f) - m_fLongestDrive;

	if (nStroke == 1 && (m_nClubType == 0 || m_nClubType == 1)) {
		if (IsLocalContent((localContentType_t)0x32)) {
			__int64 nCaddiePang = m_caddieBonus.GetBonusPang(CADDIE_W_BIGBONGDARI, 0);
			if (nCaddiePang) {
				m_cases[21].m_pang += nCaddiePang;
				m_pangs[1] += nCaddiePang;
				PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x1000000;
			}
		}

		if (fDist > 0.0f && !GolfBall().m_landed) {
			if (GetCurMap() != 19) {
				m_pangs[0] += (int)fDist;
				PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 1;
			} else {
				m_cases[0].m_flags = 0x100;
			}
		} else {
			m_cases[0].m_flags = 0x100;
		}

		if (Bar().m_stopped && Bar().m_accuracyGrade == 4 && GolfClub().PowerShotType()) {
			m_pangs[0] *= 2;
			m_cases[1].m_pang = 2;
			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 4;
		} else {
			m_cases[1].m_flags = 0x100;
		}
	} else {
		m_cases[0].m_flags = 0x100;
		m_cases[1].m_flags = 0x100;
	}

	diff = GetHoleCupPos() - ball.PreData(ball.m_preDataCount)->m_pos;
	fDist = sqrtf(diff.x * diff.x + diff.z * diff.z) * 0.3125f;

	if (m_nPangRate < 0 && m_nClubType != 3 && GetPVS().GetGroundType(ball.PreData(0)->m_pos, true, NULL) != 2
		&& fDist < 7.0f && !ball.IsHoleIn() && m_fStartToPin > 30.0f) {
		int nMultiplier = 1;
		if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty())) {
			nMultiplier = 2;
		}

		if (m_fStartToPin < 60.0f) {
			m_pangs[0] += (1 - (10 - float2int(fDist)) * m_nPangRate) * nMultiplier / 2;
		} else {
			m_pangs[0] += -((10 - float2int(fDist)) * m_nPangRate * nMultiplier);
		}
		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x200;
	} else {
		m_cases[2].m_flags = 0x100;
	}

	if (m_fStartToPin > 17.0f && m_nClubType == 3 && m_nPangRate <= 0 && !PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holeIn[GOLFDOC()->m_currentHole - 1]) {
		if (ball.IsHoleIn()) {
			if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty())) {
				m_pangs[0] += float2int(fDrive) * 4;
			} else {
				m_pangs[0] += float2int(fDrive) * 2;
			}
			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x80;
		}
	} else {
		m_cases[3].m_flags = 0x100;
	}

	if (m_nClubType != 3 && ball.m_groundType != 2 && m_fStartToPin > 17.0f && m_nPangRate <= 0) {
		if (ball.IsHoleIn()) {
			if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty())) {
				m_pangs[0] += float2int(fDrive) * 5;
			} else if (!(ball.m_powerFlags & 0x70) && GolfClub().GetApproachMode() == 4) {
				m_pangs[0] += float2int(fDrive) * 4;
				PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x10000;
			} else {
				m_pangs[0] += float2int(fDrive) * 2;
			}

			if (fDrive < 200.0f) {
				PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x20;
			} else {
				PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x40;
			}
		}
	} else {
		m_cases[4].m_flags = 0x100;
	}

	if (GetPVS().GetGroundType(ball.PreData(0)->m_pos, true, NULL) != 2 && ball.m_eventPreDataIndex[8] > 0
		&& ball.m_eventPreDataIndex[8] <= ball.m_preDataCount && ball.m_eventPreDataIndex[7] < 0
		&& (ball.m_eventPreDataIndex[8] < ball.m_eventPreDataIndex[0] || ball.m_eventPreDataIndex[0] < 0)) {
		fDist = (ball.PreData(ball.m_eventPreDataIndex[8])->m_pos - GetHoleCupPos()).Magnitude();

		if (m_nPangRate <= 0 && ball.IsHoleIn() && fDist > ball.m_radius * 2) {
			m_cases[5].m_pang = 100;
			m_pangs[0] += m_cases[5].m_pang;
			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x10;
		} else {
			m_cases[5].m_flags = 0x100;
		}
	} else {
		m_cases[5].m_flags = 0x100;
	}

	if (ball.m_flags & 0x70) {
		if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x1800000A
			|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000007
			|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000002
			|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x1800000B
			|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000008
			|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000003
			|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x1800000F
			|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000004
			|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000010
			|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000012
			|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x1800001F
			|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000022
			|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000021
			|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000027) {
			m_cases[7].m_pang = 5;
		} else {
			m_cases[7].m_pang = 10;
		}

		m_pangs[0] += m_cases[7].m_pang;
		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 8;

		GetSpecialShotOverlayInfo(ball.m_flags, (unsigned char)GolfClub().PowerShotType(), m_cases[7].m_nOverlay, m_cases[7].m_rect.x, m_cases[7].m_rect.y);
	} else {
		m_cases[7].m_flags = 0x100;
	}

	m_cases[6].m_flags = 0x100;
	unsigned char nClash = 0;
	if (!bFlag) {
		const std::vector<CQuadTree::sColData> &colList = GetPVS().GetColList();
		short nLastObj = -1;

		for (std::vector<CQuadTree::sColData>::const_iterator it = colList.begin(); it != colList.end(); ++it) {
			if (it->m_objIndex >= 0 && it->m_objIndex != nLastObj) {
				nClash++;
				nLastObj = it->m_objIndex;
			}
		}
	}

	if (ball.IsHoleIn() && nDiff <= 0) {
		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x100;

		if (CSharedDoc::Instance()->m_roomType == 1 || CSharedDoc::Instance()->m_roomType == 3) {
			m_cases[8].m_flags = 0x100;
		} else {
			m_cases[8].m_pang = GetHoleInPang(nStroke, nPar);
			GetHoleInOverlayInfo(nStroke, nPar, m_cases[8].m_nOverlay, m_cases[8].m_rect.x, m_cases[8].m_rect.y);
		}

		if (Bar().m_accuracyGrade == 4 && m_nPangRate <= 0 && !nClash && (ball.m_powerFlags & 0xF)) {
			if (ball.m_powerFlags & 3) {
				if (CSharedDoc::Instance()->m_roomType == 1) {
					TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holeInKind = 9;
					m_cases[9].m_flags = 0x100;
				} else if (CSharedDoc::Instance()->m_roomType == 3) {
					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holeInKind = 9;
					m_cases[9].m_flags = 0x100;
				} else if (!S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty())) {
					m_pangs[0] += m_cases[8].m_pang * 2;
					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x400;
				} else {
					m_pangs[0] += m_cases[8].m_pang;
					m_cases[9].m_flags = 0x100;
				}

				m_cases[10].m_flags = 0x100;
			} else {
				if (CSharedDoc::Instance()->m_roomType == 1) {
					TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holeInKind = 10;
					m_cases[10].m_flags = 0x100;
				} else if (CSharedDoc::Instance()->m_roomType == 3) {
					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holeInKind = 10;
					m_cases[10].m_flags = 0x100;
				} else {
					m_pangs[0] += m_cases[8].m_pang * 2;
					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x800;
				}

				m_cases[9].m_flags = 0x100;
			}
		} else {
			if (CSharedDoc::Instance()->m_roomType == 1) {
				if (m_nPangRate <= 0) {
					TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holeInKind = 8;
				}
			} else if (CSharedDoc::Instance()->m_roomType == 3) {
				if (m_nPangRate <= 0) {
					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holeInKind = 8;
				}
			} else {
				m_pangs[0] += m_cases[8].m_pang;
			}

			m_cases[9].m_flags = 0x100;
			m_cases[10].m_flags = 0x100;
		}

		if (nClash > 0) {
			if (CSharedDoc::Instance()->m_roomType == 1) {
				if (m_nPangRate <= 0) {
					TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holeInKind = 11;
					TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holeInCount = nClash;
				}
				m_cases[11].m_flags = 0x100;
			} else if (CSharedDoc::Instance()->m_roomType == 3) {
				if (m_nPangRate <= 0) {
					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holeInKind = 11;
					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holeInCount = nClash;
				}
				m_cases[11].m_flags = 0x100;
			} else {
				if (m_nPangRate <= 0) {
					m_pangs[0] += ((int)pow(2.0f, (int)nClash) - 1) * m_cases[8].m_pang;
				}
				m_cases[11].m_pang = 1;
				PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 2;
			}
		} else {
			m_cases[11].m_flags = 0x100;
		}
	} else {
		int nOver = m_nPangRate + (ball.m_landed ? 1 : 0);

		if (nOver >= 5 && bFlag) {
			m_cases[8].m_pang = m_holeInPangs[GetCurMap()].m_perStroke * 5;
			m_pangs[0] += m_cases[8].m_pang;
			m_cases[8].m_rect.y = 0.875f;
		} else {
			m_cases[8].m_flags = 0x100;
		}

		m_cases[9].m_flags = 0x100;
		m_cases[10].m_flags = 0x100;
		if (nClash > 0) {
			m_cases[11].m_pang = 1;
		} else {
			m_cases[11].m_flags = 0x100;
		}
	}

	if (ball.IsHoleIn()
		&& (((CSharedDoc::Instance()->m_roomType == 0 || CSharedDoc::Instance()->m_roomType == 3 || CSharedDoc::Instance()->m_roomType == 7)
				&& PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] == (unsigned char)1)
			|| (CSharedDoc::Instance()->m_roomType == 1
				&& TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] == (unsigned char)1))) {
		m_cases[12].m_pang = 20;
		m_pangs[1] += m_cases[12].m_pang;
		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x1000;
	} else {
		m_cases[12].m_flags = 0x100;
	}

	if (ball.m_eventPreDataIndex[6] > 0 && ball.m_eventPreDataIndex[6] < ball.m_preDataCount) {
		const CGolfBall::sExpected *pSpin = ball.PreData(ball.m_eventPreDataIndex[6]);
		if (pSpin) {
			unsigned char nGround = GetPVS().GetGroundType(pSpin->m_pos, true, NULL);
			if (nGround >= 3) {
				if (GOLFDOC()->m_pPolySoup->IsOutOfBound(pSpin->m_pos)) {
					if (!GOLFDOC()->m_pPolySoup->IsOutOfBound(ball.PreData(ball.m_preDataCount)->m_pos)) {
						m_pangs[0] += 15;
						PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x8000;
					}
				} else {
					unsigned char nEndGround = GetPVS().GetGroundType(ball.PreData(ball.m_preDataCount)->m_pos, true, NULL);
					if (nEndGround == 1 || nEndGround == 2) {
						m_pangs[0] += 10;
						PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x8000;
					}
				}
			} else {
				m_cases[15].m_flags = 0x100;
			}
		} else {
			m_cases[15].m_flags = 0x100;
		}
	} else {
		m_cases[15].m_flags = 0x100;
	}

	if ((CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId & 0x3FFFFFF) && Bar().m_accuracyGrade == 4 && GolfClub().GetType() != 3) {
		bool bUnderPar = false;
		switch (CSharedDoc::Instance()->m_roomType) {
		case 0:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 9:
		case 10:
			if (!(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] > GOLFDOC()->GetHoleData().m_par)) {
				bUnderPar = true;
			}
			break;
		case 1:
			if (!(TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] > GOLFDOC()->GetHoleData().m_par)) {
				bUnderPar = true;
			}
			break;
		}

		if (bUnderPar) {
			__int64 nAztecPang = 30;
			m_cases[16].m_pang = nAztecPang;
			m_pangs[1] += nAztecPang;
			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x4000;

			unsigned long ballId = CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId;
			__int64 nWaterBonus = 0;
			if (ballId == 0x14000005 || ballId == 0x14000080 || ballId == 0x1400003C || ballId == 0x1400003D || ballId == 0x14000016) {
				nWaterBonus = 1;
			}
			nWaterBonus += CCardManager::Instance()->GetCardBounceBonusUp();

			if (nWaterBonus > 0) {
				if (!GolfBall().m_landed) {
					int nBounce = GetPVS().GetBounceCount();
					m_cases[17].m_pang = (nBounce > 20 ? 20 : nBounce) * nWaterBonus;
					if (m_cases[17].m_pang > 1) {
						m_pangs[1] += m_cases[17].m_pang;
						PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x20000;
					} else {
						m_cases[17].m_flags = 0x100;
					}
				} else {
					m_cases[17].m_flags = 0x100;
				}
			} else {
				m_cases[17].m_flags = 0x100;
			}
		} else {
			m_cases[16].m_flags = 0x100;
			m_cases[17].m_flags = 0x100;
		}
	} else {
		m_cases[16].m_flags = 0x100;

		if (Bar().m_accuracyGrade == 4 && GolfClub().GetType() != 3) {
			CCardManager::Instance()->SetPlayerIndex(GOLFDOC()->m_myPlayerIndex);
			__int64 nWaterBonus = CCardManager::Instance()->GetCardBounceBonusUp();
			if (nWaterBonus > 0) {
				if (!GolfBall().m_landed) {
					int nBounce = GetPVS().GetBounceCount();
					m_cases[17].m_pang = (nBounce > 20 ? 20 : nBounce) * nWaterBonus;
					m_pangs[1] += m_cases[17].m_pang;
					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x20000;
				}
			} else {
				m_cases[17].m_flags = 0x100;
			}
		} else {
			m_cases[17].m_flags = 0x100;
		}
	}

	if (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_mascotId && GolfClub().GetType() != 3) {
		if (Bar().m_accuracyGrade >= 2) {
			bool bUnderPar = false;
			switch (CSharedDoc::Instance()->m_roomType) {
			case 0:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 9:
			case 10:
				if (!(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] > GOLFDOC()->GetHoleData().m_par)) {
					bUnderPar = true;
				}
				break;
			case 1:
				if (!(TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] > GOLFDOC()->GetHoleData().m_par)) {
					bUnderPar = true;
				}
				break;
			}

			if (bUnderPar) {
				sMascotInfo *pMascot = &CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_mascot;
				if (pMascot) {
					unsigned long typeId = pMascot->typeId;
					__int64 nMascotPang;
					unsigned long nFlag;
					if (Bar().m_accuracyGrade == 4) {
						nMascotPang = ItemManager()->GetMascotBonusPang(typeId, true);
						nFlag = 0x100000;
					} else {
						nMascotPang = ItemManager()->GetMascotBonusPang(typeId, false);
						nFlag = 0x200000;
					}

					if (nMascotPang > 0) {
						m_cases[20].m_pang = nMascotPang;
						m_pangs[1] += nMascotPang;
						PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= nFlag;
					} else {
						m_cases[20].m_flags = 0x100;
					}
				} else {
					m_cases[20].m_flags = 0x100;
				}
			} else {
				m_cases[20].m_flags = 0x100;
			}
		} else {
			m_cases[20].m_flags = 0x100;
		}
	} else {
		m_cases[20].m_flags = 0x100;
	}

	CSharedDoc::Instance()->SetInitAbilityItem();

	if ((CSharedDoc::Instance()->GetFlagAbilityItem() & 1) && Bar().m_accuracyGrade >= 2 && GolfClub().GetType() != 3) {
		bool bUnderPar = false;
		switch (CSharedDoc::Instance()->m_roomType) {
		case 0:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 9:
		case 10:
			if (!(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] > GOLFDOC()->GetHoleData().m_par)) {
				bUnderPar = true;
			}
			break;
		case 1:
			if (!(TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] > GOLFDOC()->GetHoleData().m_par)) {
				bUnderPar = true;
			}
			break;
		}

		if (bUnderPar) {
			if (Bar().m_accuracyGrade == 4) {
				m_cases[21].m_pang += CSharedDoc::Instance()->GetNumBonusPangItem() * 3;
				PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x400000;
			} else {
				m_cases[21].m_pang += CSharedDoc::Instance()->GetNumBonusPangItem() * 2;
				PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x800000;
			}

			m_pangs[1] += m_cases[21].m_pang;
		}
	}

	if (GetWeather() == 2 && !bFlag) {
		for (int i = 0; i < 2; i++) {
			if (m_pangs[i] > 0) {
				m_extraPangs[i] = m_pangs[i];
				m_pangs[i] *= 2;
			}
		}

		m_cases[14].m_pang = 2;
	} else {
		m_cases[14].m_flags = 0x100;
	}

	if (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_eventFlags.eventFlag3 && Bar().m_accuracyGrade == 4 && GolfClub().GetType() != 3) {
		switch (CSharedDoc::Instance()->m_roomType) {
		case 0:
		case 1:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 9:
		case 10:
			m_cases[18].m_pang = -1;
			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x40000;
			break;
		default:
			m_cases[18].m_flags = 0x100;
			break;
		}
	} else {
		m_cases[18].m_flags = 0x100;
	}

	if (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_eventFlags.eventFlag5 && Bar().m_accuracyGrade == 4 && GolfClub().GetType() != 3) {
		m_cases[19].m_pang = 1;
		m_pangs[1] += m_cases[19].m_pang;
		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x80000;
	} else {
		m_cases[19].m_flags = 0x100;
	}

	if (GOLFDOC()->m_gameState == 15 && GOLFDOC()->m_currentHole == CSharedDoc::Instance()->m_holeOrder[CSharedDoc::Instance()->m_holes - 1]
		&& (CSharedDoc::Instance()->m_roomType == 0 || CSharedDoc::Instance()->m_roomType == 1 || CSharedDoc::Instance()->m_roomType == 3)
		&& (ball.IsHoleIn() || !ball.IsHoleIn() && bFlag)) {
		m_cases[13].m_pang = GetGameClearBonus();

		float fProb = CCardManager::Instance()->GetSpecialClearBonusProb(GetCurMap());
		m_cases[13].m_pang += (unsigned long)(int)(GetGameClearBonus() * fProb);

		if (IsLocalContent((localContentType_t)0x32)) {
			__int64 nCaddiePang = m_caddieBonus.GetBonusPang(CADDIE_R_BIGBONGDARI, m_cases[13].m_pang);
			if (nCaddiePang) {
				m_cases[21].m_pang += nCaddiePang;
				m_pangs[1] += nCaddiePang;
				PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x1000000;
			}
		}

		if (CSharedDoc::Instance()->m_value4D74 & 1) {
			m_cases[13].m_pang *= 2;
		}

		if (bFlag) {
			m_pangs[1] += m_cases[13].m_pang;
		}

		if (CSharedDoc::Instance()->m_roomType == 7) {
			m_turnExtraPangs[GOLFDOC()->m_myPlayerIndex] = m_cases[13].m_pang;
		}

		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x2000;
	} else {
		m_cases[13].m_flags = 0x100;
	}

	if (m_cases[21].m_pang == 0) {
		m_cases[21].m_flags = 0x100;
	}

	Record(GOLFDOC()->m_myPlayerIndex, m_pangs);
}

// fontinfo.cpp:4328
void CPangInfo::PreCalcMatchHoleInBonus(void) {
	if (CSharedDoc::Instance()->m_roomType != 3) {
		return;
	}

	if (m_pszSfx) {
		g_audio->StopSfx(m_pszSfx);
	}

	m_bonusPangs[0] = m_shownPangs[0] = m_totalCases[0].m_pang = (int)PLAYER(MyIndex())->m_ghostAward;
	m_bonusPangs[1] = m_shownPangs[1] = m_totalCases[1].m_pang = (int)PLAYER(MyIndex())->m_extraPang;

	ResetVars(1, 0x100);

	PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags &= ~0x1000;

	sPlayerData *pPlayer = PLAYER(MyIndex());

	if (pPlayer->m_holeInKind != 0) {
		m_cases[8].m_flags = 1;

		int nScore = (unsigned char)pPlayer->m_holes[GOLFDOC()->m_currentHole - 1];
		int nPar = GOLFDOC()->GetHoleData().m_par;
		int nDiff = nScore - nPar;

		if (nDiff <= 0) {
			m_cases[8].m_pang = GetHoleInPang(nScore, nPar);
			GetHoleInOverlayInfo(nScore, nPar, m_cases[8].m_nOverlay, m_cases[8].m_rect.x, m_cases[8].m_rect.y);
		} else {
			m_cases[8].m_flags = 0x100;
		}

		if (pPlayer->m_holeInKind == 9 || pPlayer->m_holeInKind == 10) {
			if (!S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty()) && pPlayer->m_holeInKind == 9) {
				m_pangs[0] += m_cases[8].m_pang * 2;
				m_cases[9].m_flags = 1;
			} else {
				m_pangs[0] += m_cases[8].m_pang;
				m_cases[9].m_flags = 0x100;
			}
		} else {
			m_pangs[0] += m_cases[8].m_pang;
		}

		if (pPlayer->m_holeInCount > 0) {
			m_cases[11].m_pang = (int)pow(2.0f, (int)pPlayer->m_holeInCount);

			if (nDiff <= 0) {
				m_pangs[0] += (m_cases[11].m_pang - 1) * m_cases[8].m_pang;
			}

			m_cases[11].m_flags = 1;
		}

		if (GetWeather() == 2) {
			for (int i = 0; i < 2; i++) {
				if (m_pangs[i] > 0) {
					m_extraPangs[i] = m_pangs[i];
					m_pangs[i] *= 2;
				}
			}

			m_cases[14].m_flags = 1;
		}
	}

	int nHoleDiff = PLAYER(MyIndex())->m_ghostResult - PLAYER(1 - MyIndex())->m_ghostResult;
	int nRemain = CSharedDoc::Instance()->m_holes - GetHoleIndex(GOLFDOC()->m_currentHole);
	bool bDormie = false;
	bool bLost = false;

	if (nRemain > 0) {
		if (PLAYER(MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1] < PLAYER(1 - MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1]) {
			nHoleDiff++;
		} else if (PLAYER(MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1] > PLAYER(1 - MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1]) {
			nHoleDiff--;
		}

		if (nHoleDiff > nRemain) {
			m_cases[6].m_pang = nRemain * 20;
			m_pangs[0] += m_cases[6].m_pang;
			m_cases[6].m_flags = 1;

			bDormie = true;
		} else {
			int nOtherDiff = PLAYER(1 - MyIndex())->m_ghostResult - PLAYER(MyIndex())->m_ghostResult;

			if (PLAYER(MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1] > PLAYER(1 - MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1]) {
				nOtherDiff++;
			} else if (PLAYER(MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1] < PLAYER(1 - MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1]) {
				nOtherDiff--;
			}

			if (nOtherDiff > nRemain) {
				bLost = true;
			}
		}
	}

	if (GOLFDOC()->m_currentHole == CSharedDoc::Instance()->m_holeOrder[CSharedDoc::Instance()->m_holes - 1] || bDormie || bLost) {
		m_cases[13].m_pang = GetGameClearBonus();

		float fProb = CCardManager::Instance()->GetSpecialClearBonusProb(GetCurMap());
		m_cases[13].m_pang += (unsigned long)(int)(GetGameClearBonus() * fProb);

		if (CSharedDoc::Instance()->m_value4D74 & 1) {
			m_cases[13].m_pang *= 2;
		}

		m_cases[13].m_flags = 1;

		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x2000;

		m_pangs[1] += m_cases[13].m_pang;
		if (PLAYER(MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1] > PLAYER(1 - MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1]) {
			m_pangs[0] = 0;
		}

		Record(MyIndex(), m_pangs);
	} else if (PLAYER(MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1] > PLAYER(1 - MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1]) {
		m_pangs[0] = 0;
	} else {
		Record(MyIndex(), m_pangs);
	}

	__int64 otherPangs[2] = { 0, 0 };

	if (PLAYER(1 - MyIndex())->m_holeInKind != 0 && PLAYER(MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1] >= PLAYER(1 - MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1]) {
		int nScore = (unsigned char)PLAYER(1 - MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1];
		int nPar = GOLFDOC()->GetHoleData().m_par;
		int nDiff = nScore - nPar;

		if (nDiff <= 0) {
			otherPangs[0] = GetHoleInPang(nScore, nPar);

			if ((!S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty()) && PLAYER(1 - MyIndex())->m_holeInKind == 9)
				|| PLAYER(1 - MyIndex())->m_holeInKind == 10) {
				otherPangs[0] *= 2;
			}

			if (PLAYER(1 - MyIndex())->m_holeInCount > 0) {
				otherPangs[0] *= (int)pow(2.0f, (int)PLAYER(1 - MyIndex())->m_holeInCount);
			}

			if (GetWeather() == 2) {
				otherPangs[0] *= 2;
			}
		}
	}

	if (bLost) {
		otherPangs[0] += nRemain * 20;
	}

	if (otherPangs[0] > 0) {
		Record(1 - MyIndex(), otherPangs);
	}
}

// fontinfo.cpp:4571
void CPangInfo::PreCalcTeamHoleInBonus(void) {
	if (CSharedDoc::Instance()->m_roomType != 1) {
		return;
	}

	if (m_pszSfx) {
		g_audio->StopSfx(m_pszSfx);
	}

	m_bonusPangs[0] = m_shownPangs[0] = m_totalCases[0].m_pang = (int)TEAM(PLAYER(MyIndex())->m_attribute)->m_teamValue;
	m_bonusPangs[1] = m_shownPangs[1] = m_totalCases[1].m_pang = (int)TEAM(PLAYER(MyIndex())->m_attribute)->m_teamValue2;

	ResetVars(1, 0x100);

	PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags &= ~0x1000;

	sTeamData *pTeam = TEAM(PLAYER(MyIndex())->m_attribute);

	if (pTeam->m_holeInKind != 0) {
		m_cases[8].m_flags = 1;

		int nScore = (unsigned char)pTeam->m_holes[GOLFDOC()->m_currentHole - 1];
		int nPar = GOLFDOC()->GetHoleData().m_par;
		int nDiff = nScore - nPar;

		if (nDiff <= 0) {
			m_cases[8].m_pang = GetHoleInPang(nScore, nPar);
			GetHoleInOverlayInfo(nScore, nPar, m_cases[8].m_nOverlay, m_cases[8].m_rect.x, m_cases[8].m_rect.y);
		} else {
			m_cases[8].m_flags = 0x100;
		}

		if (pTeam->m_holeInKind == 9 || pTeam->m_holeInKind == 10) {
			if (pTeam->m_holeInKind == 9) {
				int bClassic = S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty());

				if (!bClassic) {
					m_pangs[0] += m_cases[8].m_pang * 2;
					m_cases[9].m_flags = 1;
				} else {
					m_pangs[0] += m_cases[8].m_pang;
					m_cases[9].m_flags = 0x100;
				}
			} else {
				m_pangs[0] += m_cases[8].m_pang * 2;
				m_cases[10].m_flags = 1;
			}
		} else {
			m_pangs[0] += m_cases[8].m_pang;
		}

		if (pTeam->m_holeInCount > 0) {
			m_cases[11].m_pang = (int)pow(2.0f, (int)pTeam->m_holeInCount);

			if (nDiff <= 0) {
				m_pangs[0] += (m_cases[11].m_pang - 1) * m_cases[8].m_pang;
			}

			m_cases[11].m_flags = 1;
		}

		if (GetWeather() == 2) {
			for (int i = 0; i < 2; i++) {
				if (m_pangs[i] > 0) {
					m_extraPangs[i] = m_pangs[i];
					m_pangs[i] *= 2;
				}
			}

			m_cases[14].m_flags = 1;
		}
	}

	int myTeam = PLAYER(MyIndex())->m_attribute;
	int nHoleDiff = TEAM(myTeam)->m_resetCount - TEAM(1 - myTeam)->m_resetCount;
	int nRemain = CSharedDoc::Instance()->m_holes - GetHoleIndex(GOLFDOC()->m_currentHole);
	bool bDormie = false;
	bool bLost = false;

	if (nRemain > 0) {
		if (TEAM(myTeam)->m_holes[GOLFDOC()->m_currentHole - 1] < TEAM(1 - myTeam)->m_holes[GOLFDOC()->m_currentHole - 1]) {
			nHoleDiff++;
		} else if (TEAM(myTeam)->m_holes[GOLFDOC()->m_currentHole - 1] > TEAM(1 - myTeam)->m_holes[GOLFDOC()->m_currentHole - 1]) {
			nHoleDiff--;
		}

		if (nHoleDiff > nRemain) {
			m_cases[6].m_pang = nRemain * 20;
			m_pangs[0] += m_cases[6].m_pang;
			m_cases[6].m_flags = 1;

			bDormie = true;
		} else {
			int nOtherDiff = TEAM(1 - myTeam)->m_resetCount - TEAM(myTeam)->m_resetCount;

			if (TEAM(myTeam)->m_holes[GOLFDOC()->m_currentHole - 1] > TEAM(1 - myTeam)->m_holes[GOLFDOC()->m_currentHole - 1]) {
				nOtherDiff++;
			} else if (TEAM(myTeam)->m_holes[GOLFDOC()->m_currentHole - 1] < TEAM(1 - myTeam)->m_holes[GOLFDOC()->m_currentHole - 1]) {
				nOtherDiff--;
			}

			if (nOtherDiff > nRemain) {
				bLost = true;
			}
		}
	}

	if (GOLFDOC()->m_currentHole == CSharedDoc::Instance()->m_holeOrder[CSharedDoc::Instance()->m_holes - 1] || bDormie || bLost) {
		m_cases[13].m_pang = GetGameClearBonus();

		float fProb = CCardManager::Instance()->GetSpecialClearBonusProb(GetCurMap());
		m_cases[13].m_pang += (unsigned long)(int)(GetGameClearBonus() * fProb);

		if (CSharedDoc::Instance()->m_value4D74 & 1) {
			m_cases[13].m_pang *= 2;
		}

		m_cases[13].m_flags = 1;

		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags |= 0x2000;

		m_pangs[1] += m_cases[13].m_pang;
		if (TEAM(myTeam)->m_holes[GOLFDOC()->m_currentHole - 1] > TEAM(1 - myTeam)->m_holes[GOLFDOC()->m_currentHole - 1]) {
			m_pangs[0] = 0;
		}

		Record(MyIndex(), m_pangs);
	} else if (TEAM(myTeam)->m_holes[GOLFDOC()->m_currentHole - 1] > TEAM(1 - myTeam)->m_holes[GOLFDOC()->m_currentHole - 1]) {
		m_pangs[0] = 0;
	} else {
		Record(MyIndex(), m_pangs);
	}

	__int64 otherPangs[2];
	otherPangs[1] = 0;

	otherPangs[0] = 0;
	if (TEAM(1 - myTeam)->m_holeInKind != 0 && !(TEAM(myTeam)->m_holes[GOLFDOC()->m_currentHole - 1] < TEAM(1 - myTeam)->m_holes[GOLFDOC()->m_currentHole - 1])) {
		int nScore = (unsigned char)TEAM(1 - myTeam)->m_holes[GOLFDOC()->m_currentHole - 1];
		int nPar = GOLFDOC()->GetHoleData().m_par;
		int nDiff = nScore - nPar;

		if (nDiff <= 0) {
			otherPangs[0] = GetHoleInPang(nScore, nPar);

			if (!S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty()) && TEAM(1 - myTeam)->m_holeInKind == 9) {
				otherPangs[0] *= 2;
			} else if (TEAM(1 - myTeam)->m_holeInKind == 10) {
				otherPangs[0] *= 2;
			}

			if (TEAM(1 - myTeam)->m_holeInCount > 0) {
				otherPangs[0] *= (int)pow(2.0f, (int)TEAM(1 - myTeam)->m_holeInCount);
			}

			if (GetWeather() == 2) {
				otherPangs[0] *= 2;
			}
		}
	}

	if (bLost) {
		otherPangs[0] += nRemain * 20;
	}

	if (otherPangs[0] > 0) {
		for (int i = 0; i < 4; i++) {
			if (GOLFDOC()->GetPlayer(i)->m_attribute != myTeam) {
				Record(i, otherPangs);
				break;
			}
		}
	}
}

// fontinfo.cpp:4831
void CPangInfo::ResetVars(int nFlags, int nCaseState) {
	m_flags = nFlags;
	m_pszSfx = NULL;
	m_nState = 0;
	m_nSubState = 0;
	m_nWaterHits = 0;
	m_nLastWaterHit = -1;

	for (int i = 0; i < 4; i++) {
		m_totalCases[i].m_flags = 8;
		m_totalCases[i].m_fTime = 0;

		m_extraPangs[i] = 0;
		m_pangs[i] = 0;
	}

	m_fFinalizeTime = 0;

	for (int i = 0; i < 24; i++) {
		m_cases[i].m_flags = nCaseState;
		m_cases[i].m_pang = 0;
		m_cases[i].m_nIndex = -1;
		m_cases[i].m_fTime = 0;
		m_cases[i].m_byAlpha = 0;
	}
}

// fontinfo.cpp:4864
void CPangInfo::ShowMyPang(void) {
	if (CSharedDoc::Instance()->m_roomType == 7) {
		if (m_pszSfx) {
			g_audio->StopSfx(m_pszSfx);
		}

		ResetVars(0, 0x100);

		CSharedDoc *pDoc = CSharedDoc::Instance();
		if (pDoc->m_roomType == 7 && GOLFDOC()->m_currentHole == pDoc->m_holeOrder[pDoc->m_holes - 1]) {
			m_bonusPangs[0] = m_shownPangs[0] = m_totalCases[0].m_pang = PLAYER(MyIndex())->m_ghostAward;
			m_bonusPangs[1] = m_shownPangs[1] = m_totalCases[1].m_pang = (int)PLAYER(MyIndex())->m_extraPang + m_turnExtraPangs[MyIndex()];
		} else {
			m_bonusPangs[0] = m_shownPangs[0] = m_totalCases[0].m_pang = PLAYER(MyIndex())->m_ghostAward;
			m_bonusPangs[1] = m_shownPangs[1] = m_totalCases[1].m_pang = PLAYER(MyIndex())->m_extraPang;
		}

		GroundItemMan *pMan = (GroundItemMan *)AfxGetTask()->GetActor("GroundItemMan");
		if (pMan) {
			m_bonusPangs[2] = m_shownPangs[2] = m_totalCases[2].m_pang = pMan->m_playerStats[MyIndex()].m_totalCount[0];
			m_bonusPangs[3] = m_shownPangs[3] = m_totalCases[3].m_pang = pMan->m_playerStats[MyIndex()].m_totalCount[1];
		} else {
			m_bonusPangs[2] = m_shownPangs[2] = m_totalCases[2].m_pang = 0;
			m_bonusPangs[3] = m_shownPangs[3] = m_totalCases[3].m_pang = 0;
		}
	}
}

// fontinfo.cpp:4904
__int64 CPangInfo::GetHoleInPang(int nScore, int nPar) {
	unsigned char course = GetCurMap();

	if (GOLFDOC()->m_gameState < 15)
		course = 0;

	if (nScore == 1)
		return m_holeInPangs[course].m_holeInOne;

	switch (nScore - nPar) {
	case -3:
		return m_holeInPangs[course].m_albatross;
	case -2:
		return m_holeInPangs[course].m_eagle;
	case -1:
		return m_holeInPangs[course].m_birdie;
	case 0:
		return m_holeInPangs[course].m_par;
	default:
		return m_holeInPangs[course].m_perStroke * (nScore - nPar);
	}
}

// The pang overlays of a special shot: per shot kind, four {overlay, height, width}.
PangOverlayInfo g_specialShotPangOverlayInfo[][4] = {
	{
		{ 0, 0, 0 }, { 1, 1, 0 }, { 1, 1, 1 }, { 1, 1, 1 }
	},
	{
		{ 0, 0, 0 }, { 1, 2, 0 }, { 1, 2, 1 }, { 1, 2, 1 }
	},
	{
		{ 0, 0, 0 }, { 1, 0, 0 }, { 1, 0, 1 }, { 1, 0, 1 }
	}
};

// The pang overlays of a hole-in (GetHoleInOverlayInfo's index).
PangOverlayInfo g_holeInPangOverlayInfo[] = {
	{ 0, 9, 0 }, { 0, 10, 0 }, { 0, 11, 0 },
	{ 0, 12, 0 }, { 0, 13, 0 }, { 0, 14, 0 }
};

// fontinfo.cpp:4933
void CPangInfo::GetHoleInOverlayInfo(int nScore, int nPar, unsigned int &overlay, float &fWidth, float &fHeight) {
	int index;
	if (nScore == 1) {
		index = 0;
	} else {
		switch (nScore - nPar) {
		case -3:
			index = 1;
			break;
		case -2:
			index = 2;
			break;
		case -1:
			index = 3;
			break;
		case 0:
			index = 4;
			break;
		default:
			index = 5;
			break;
		}
	}

	overlay = g_holeInPangOverlayInfo[index].m_overlay;

	fWidth = g_holeInPangOverlayInfo[index].m_width * 0.5f;
	fHeight = g_holeInPangOverlayInfo[index].m_height * 0.0625f;
}

// fontinfo.cpp:4967
void CPangInfo::GetSpecialShotOverlayInfo(unsigned long flags, unsigned char index, unsigned int &overlay, float &fWidth, float &fHeight) {
	int type;
	if (flags & 0x10) {
		type = 0;
	} else if (flags & 0x20) {
		type = 1;
	} else {
		type = 2;
	}

	overlay = g_specialShotPangOverlayInfo[type][index].m_overlay;

	fWidth = g_specialShotPangOverlayInfo[type][index].m_width * 0.5f;
	fHeight = g_specialShotPangOverlayInfo[type][index].m_height * 0.0625f;
}

// fontinfo.cpp:4987
void CPangInfo::UpdateBallState(void) {
	if (GolfBall().m_preDataIndex >= 0 && !(GOLFDOC()->GameMode() & 0x1000) && !(m_flags & 0x10)) {
		CGolfBall &ball = GolfBall();

		WVector diff, toCup;
		diff = ball.PreData(ball.m_preDataIndex)->m_pos - ball.PreData(0)->m_pos;
		toCup = GetHoleCupPos() - ball.PreData(ball.m_preDataIndex)->m_pos;

		m_fDriveDistance = sqrtf(diff.x * diff.x + diff.z * diff.z) * 0.3125f;
		m_fToPin = sqrtf(toCup.x * toCup.x + toCup.y * toCup.y + toCup.z * toCup.z) * 0.3125f;
	}
}

// fontinfo.cpp:5003
void CPangInfo::ProcessTotal(float fDelta) {
	if (m_flags & 0x10) {
		return;
	}

	for (int i = 0; i < 4; i++) {
		m_totalCases[i].m_fTime += fDelta;

		if (m_totalCases[i].m_flags == 2 && m_totalCases[i].m_fTime > 0.15f) {
			m_totalCases[i].m_flags = 4;
			m_totalCases[i].m_fTime = 0;
		} else if (m_totalCases[i].m_flags == 4 && m_totalCases[i].m_fTime > 0.1f) {
			m_totalCases[i].m_flags = 8;
			m_totalCases[i].m_fTime = 0;
		}
	}
}

// fontinfo.cpp:5025
void CPangInfo::ProcessCases(float fDelta) {
	if (m_flags & 0x10) {
		return;
	}

	float fBarY = COption::Instance()->gGetBar_Y();

	if (m_flags & 8) {
		for (int i = 1; i < 10; i++) {
			if (m_cases[i].m_flags & 0x1000) {
				m_cases[i].m_fTargetY -= fDelta * 96.0f;
			}
		}
		if (m_cases[0x11].m_flags & 0x1000) {
			m_cases[0x11].m_fTargetY -= fDelta * 96.0f;
		}
		if (m_cases[0x10].m_flags & 0x1000) {
			m_cases[0x10].m_fTargetY -= fDelta * 96.0f;
		}
		if (m_cases[0xE].m_flags & 0x1000) {
			m_cases[0xE].m_fTargetY -= fDelta * 96.0f;
		}
		if (m_cases[0x12].m_flags & 0x1000) {
			m_cases[0x12].m_fTargetY -= fDelta * 96.0f;
		}
		if (m_cases[0x13].m_flags & 0x1000) {
			m_cases[0x13].m_fTargetY -= fDelta * 96.0f;
		}
		if (IsLocalContent((localContentType_t)3)) {
			if (m_cases[0x14].m_flags & 0x1000) {
				m_cases[0x14].m_fTargetY -= fDelta * 96.0f;
			}
		}
		if (m_cases[0x15].m_flags & 0x1000) {
			m_cases[0x15].m_fTargetY -= fDelta * 96.0f;
		}
	}

	for (int i = 0; i < 24; i++) {
		if (m_cases[i].m_flags & 0x1000) {
			m_cases[i].m_fY -= fDelta * 40.0f;

			if (m_cases[i].m_fY <= m_cases[i].m_fTargetY) {
				m_cases[i].m_flags &= ~0x1000;
			}
		} else {
			switch (m_cases[i].m_flags & 0xFFF) {
			case 2:
				if (m_cases[i].m_fTime > 0.5f) {
					m_cases[i].m_flags = 4;
					m_cases[i].m_fTime = 0;
				}
				break;
			case 4:
				if (m_cases[i].m_fTime > 0.5f) {
					m_cases[i].m_flags = 8;
					m_cases[i].m_fTime = 0;
				}
				break;
			case 0x10:
				if (m_pszSfx == NULL) {
					if (m_pangs[0] > 0) {
						m_pszSfx = "pang_up";
					} else {
						m_pszSfx = "pang_down";
					}

					if (m_pszSfx && !g_audio->IsPlaying(m_pszSfx)) {
						g_audio->PlaySfx(m_pszSfx, NULL, 1, m_pszSfx, NULL, 0.5f, 200.0f);
					}
				}

				m_cases[i].m_fY += fDelta * 96.0f;

				if (m_cases[i].m_fY > fBarY - 108.0f + 20.0f) {
					switch (i) {
					case 1:
						m_shownPangs[0] += (m_cases[i].m_pang - 1) * m_cases[0].m_pang;
						m_totalCases[0].m_pang = m_shownPangs[0];
						break;
					case 8:
						m_shownPangs[0] += m_cases[8].m_pang;
						m_shownPangs[0] = (int)m_shownPangs[0] < 0 ? 0 : (int)m_shownPangs[0];
						m_totalCases[0].m_pang = m_shownPangs[0];
						break;
					case 9:
					case 10:
					case 11:
						m_shownPangs[0] += (m_cases[i].m_pang - 1) * m_cases[8].m_pang;
						m_totalCases[0].m_pang = m_shownPangs[0];
						break;
					case 14:
						for (int j = 0; j < 2; j++) {
							if (m_extraPangs[j] > 0) {
								m_shownPangs[j] += m_extraPangs[j];
								m_totalCases[j].m_pang = m_shownPangs[j];
							}
						}
						break;
					case 19:
						m_shownPangs[1] += m_cases[i].m_pang;
						m_totalCases[1].m_pang = m_shownPangs[1];
						break;
					case 20:
						m_shownPangs[1] += m_cases[i].m_pang;
						m_totalCases[1].m_pang = m_shownPangs[1];
						break;
					case 12:
					case 13:
					case 16:
					case 17:
					case 21:
						m_shownPangs[1] += m_cases[i].m_pang;
						m_totalCases[1].m_pang = m_shownPangs[1];
						break;
					case 22:
						m_shownPangs[2] += m_cases[i].m_pang;
						m_totalCases[2].m_pang = m_shownPangs[2];
						break;
					case 23:
						m_shownPangs[3] += m_cases[i].m_pang;
						m_totalCases[3].m_pang = m_shownPangs[3];
						break;
					default:
						m_shownPangs[0] += m_cases[i].m_pang;
						m_totalCases[0].m_pang = m_shownPangs[0];
						break;
					}

					m_cases[i].m_flags = 0x20;
					m_cases[i].m_byAlpha = 0;

					switch (i) {
					case 0x16:
						m_totalCases[2].m_flags = 2;
						m_totalCases[2].m_fTime = 0;
						break;
					case 0x17:
						m_totalCases[3].m_flags = 2;
						m_totalCases[3].m_fTime = 0;
						break;
					default:
						for (int j = 0; j < 2; j++) {
							m_totalCases[j].m_flags = 2;
							m_totalCases[j].m_fTime = 0;
						}
						break;
					}
				} else if (m_cases[i].m_fY > fBarY - 108.0f) {
					float fRatio = (m_cases[i].m_fY - (fBarY - 108.0f)) * 0.05f;
					fRatio = fRatio > data_9eb5b8[20].f ? data_9eb5b8[20].f : fRatio;
					float fAlpha = (fBarY - 108.0f + 20.0f - m_cases[i].m_fY) * 12.75f;
					m_cases[i].m_byAlpha = (unsigned char)(fAlpha < 0.0f ? 0.0f : (fAlpha > 255.0f ? 255.0f : fAlpha));

					switch (i) {
					case 1: {
						__int64 nMul = m_cases[i].m_pang - 1;
						m_totalCases[0].m_pang = m_shownPangs[0] + float2int(nMul * fRatio * m_cases[0].m_pang);
						break;
					}
					case 8:
						m_totalCases[0].m_pang = m_shownPangs[0] + float2int(m_cases[8].m_pang * fRatio);
						m_totalCases[0].m_pang = (int)m_totalCases[0].m_pang < 0 ? 0 : (int)m_totalCases[0].m_pang;
						break;
					case 9:
					case 10:
					case 11: {
						__int64 nMul = m_cases[i].m_pang - 1;
						m_totalCases[0].m_pang = m_shownPangs[0] + float2int(nMul * fRatio * m_cases[8].m_pang);
						break;
					}
					case 14: {
						__int64 nMul = m_cases[i].m_pang - 1;
						float fExtra = nMul * fRatio;
						m_totalCases[0].m_pang = m_shownPangs[0] + float2int(m_extraPangs[0] * fExtra);
						m_totalCases[1].m_pang = m_shownPangs[1] + float2int(m_extraPangs[1] * fExtra);
						break;
					}
					case 12:
					case 13:
					case 16:
					case 17:
					case 19:
					case 20:
					case 21:
						m_totalCases[1].m_pang = m_shownPangs[1] + float2int(m_cases[i].m_pang * fRatio);
						break;
					case 22:
						m_totalCases[2].m_pang = m_shownPangs[2] + float2int(m_cases[i].m_pang * fRatio);
						break;
					case 23:
						m_totalCases[3].m_pang = m_shownPangs[3] + float2int(m_cases[i].m_pang * fRatio);
						break;
					default:
						m_totalCases[0].m_pang = m_shownPangs[0] + float2int(m_cases[i].m_pang * fRatio);
						break;
					}
				}
				break;
			case 0x20:
				m_cases[i].m_flags = 0x2000;

				if (m_pszSfx) {
					g_audio->StopSfx(m_pszSfx);
				}
				break;
			case 0x40: {
				m_cases[i].m_fY = fBarY - 108.0f - m_cases[i].m_nIndex * 20.0f - m_cases[i].m_fTime * 160.0f;
				float fAlpha = (0.5f - m_cases[i].m_fTime) * 255.0f;
				m_cases[i].m_byAlpha = (unsigned char)(fAlpha < 0.0f ? 0.0f : fAlpha);

				if (m_cases[i].m_fTime > 0.5f) {
					m_cases[i].m_flags = 0x80;
				}
				break;
			}
			}
		}
	}
}

// fontinfo.cpp:5292
void CPangInfo::ProcessHappyBonus(float fDelta) {
	if (m_flags & 8) {
		m_fHappyBonusTime += fDelta;
	}
}

// fontinfo.cpp:5300
void CPangInfo::ProcessAztecBonus(float fDelta) {
	if (m_flags & 8) {
		m_fAztecBonusTime += fDelta;
	}
}

// fontinfo.cpp:5308
void CPangInfo::ProcessAngelBonus(float fDelta) {
	if (m_flags & 8) {
		m_fAngelBonusTime += fDelta;
	}
}

// fontinfo.cpp:5316
void CPangInfo::ProcessDevilBonus(float fDelta) {
	if (m_flags & 8) {
		m_fDevilBonusTime += fDelta;
	}
}

// fontinfo.cpp:5324
void CPangInfo::CheckReadyToMakeDecision(void) {
	if (GolfBall().m_state != 0 || (m_flags & 8)) {
		return;
	}

	if (m_flags & 4) {
		m_flags |= 8;
		m_nBonusEffects = 0;

		if (m_cases[0x10].m_flags != 0x100) {
			m_nBonusEffects++;
			m_fAztecBonusTime = 0;
			AfxGetTask()->GetActor("Screen") << MsgObject(0, 0x1C3, 0, 0, 0, 0, 0);
		}

		if (m_cases[0x12].m_flags != 0x100) {
			m_nBonusEffects++;
			m_fAngelBonusTime = 0;
			AfxGetTask()->GetActor("Screen") << MsgObject(0, 0x1C3, 0, 0, 0, 0, 0);
		}

		if (m_cases[0x13].m_flags != 0x100) {
			m_nBonusEffects++;
			m_fDevilBonusTime = 0;
			AfxGetTask()->GetActor("Screen") << MsgObject(0, 0x1C3, 0, 0, 0, 0, 0);
		}

		if (m_cases[0xC].m_flags != 0x100) {
			m_nBonusEffects++;
			m_fHappyBonusTime = 0;
			AfxGetTask()->GetActor("Screen") << MsgObject(0, 0x1C3, 0, 0, 0, 0, 0);
		}
	} else {
		m_flags |= 4;

		for (int i = 0; i < 24; i++) {
			if (m_cases[i].m_flags & 0x1007) {
				m_flags &= ~4;
				return;
			}
		}
	}
}

// fontinfo.cpp:5382
void CPangInfo::CheckReadyToFinalize(float fDelta) {
	if (GolfBall().m_state != 0 || (m_flags & 0x10)) {
		return;
	}

	if (m_cases[0x10].m_flags != 0x100 && m_fAztecBonusTime < data_9eb5b8[20].f) {
		return;
	}
	if (m_cases[0x12].m_flags != 0x100 && m_fAngelBonusTime < data_9eb5b8[20].f) {
		return;
	}
	if (m_cases[0x13].m_flags != 0x100 && m_fDevilBonusTime < data_9eb5b8[20].f) {
		return;
	}
	if (m_cases[0xC].m_flags != 0x100 && m_fHappyBonusTime < data_9eb5b8[20].f) {
		return;
	}

	if (m_fFinalizeTime == 0.0f) {
		for (int i = 0; i < 24; i++) {
			if (m_cases[i].m_flags != 0x80 && m_cases[i].m_flags != 0x100 && m_cases[i].m_flags != 0x2000) {
				return;
			}
		}
	}

	m_fFinalizeTime += fDelta;

	if (m_fFinalizeTime > 1.5f) {
		m_flags |= 0x10;
	}
}

// fontinfo.cpp:5414
void CPangInfo::Insert(int nCase) {
	float fBarY = COption::Instance()->gGetBar_Y();

	int nIndex = -1;
	if (nCase == 0) {
		nIndex = 0;
		m_cases[0].m_nIndex = nIndex;
	} else {
		if (nCase == 8 && m_cases[11].m_flags != 0x100) {
			nIndex = m_cases[11].m_nIndex;
			m_cases[8].m_nIndex = nIndex;
		} else if (nCase == 9 || nCase == 10) {
			nIndex = m_cases[8].m_nIndex + 1;
			m_cases[nCase].m_nIndex = nIndex;
		} else if (nCase == 22 || nCase == 23) {
			m_cases[nCase].m_nIndex = m_nState;
		} else if (m_cases[14].m_flags == 0x100 || nCase == 13 || nCase == 14) {
			if (!(m_cases[22].m_flags & 0x101)) {
				nIndex = m_cases[22].m_nIndex;
			}
			if (!(m_cases[23].m_flags & 0x101)) {
				if (nIndex < 0) {
					nIndex = m_cases[23].m_nIndex;
				} else if (m_cases[23].m_nIndex <= nIndex) {
					nIndex = m_cases[23].m_nIndex;
				}
			}
			if (nIndex < 0) {
				m_cases[nCase].m_nIndex = m_nState;
			} else {
				m_cases[nCase].m_nIndex = nIndex;
			}
		} else {
			nIndex = m_cases[14].m_nIndex;
			m_cases[nCase].m_nIndex = nIndex;
		}
	}

	if (nIndex >= 0) {
		for (int i = 0; i < 24; i++) {
			if (i != nCase && !(m_cases[i].m_flags & 0x101) && m_cases[i].m_nIndex >= nIndex) {
				m_cases[i].m_flags |= 0x1000;
				m_cases[i].m_nIndex++;
				m_cases[i].m_fTargetY = fBarY - 108.0f - m_cases[i].m_nIndex * 20.0f;
			}
		}
	}

	m_nState++;
	m_cases[nCase].m_fY = fBarY - 108.0f - m_cases[nCase].m_nIndex * 20.0f;
	m_cases[nCase].m_flags = 2;

	if (nCase == 12 || nCase == 16 || nCase == 18) {
		m_fPopupX = (float)floor(g_view->GetWidth() - 10.0f - 192.6f);
		m_fPopupY = m_cases[nCase].m_fY - 13.44f;
	}
}

// fontinfo.cpp:5506
void CPangInfo::MakeDecision(int nCase, bool bSuccess) {
	m_cases[nCase].m_flags = (m_cases[nCase].m_flags & 0xFFFFF000) | (bSuccess ? 0x10 : 0x40);
	m_cases[nCase].m_fTime = 0;
}

// fontinfo.cpp:5512
void CPangInfo::SuperPangya(void) {
	if (m_cases[1].m_flags == 0x100) {
		return;
	}

	if (m_cases[1].m_flags == 1) {
		Insert(1);
	}
}

// fontinfo.cpp:5521
void CPangInfo::LongDrive(void) {
	if (m_cases[0].m_flags == 0x100) {
		return;
	}

	if (m_fDriveDistance > m_fLongestDrive) {

		if (m_cases[0].m_flags == 1)
			Insert(0);
		m_cases[0].m_pang = (int)(m_fDriveDistance - m_fLongestDrive);
	}

	if (m_flags & 4) {

		if (m_fDriveDistance > m_fLongestDrive && !GolfBall().m_landed)
			m_cases[0].SetState(0x10);
		else
			m_cases[0].SetState(0x40);
	}
}

// fontinfo.cpp:5542
void CPangInfo::Approach(void) {
	if (m_cases[2].m_flags == 0x100 || GolfBall().m_state != 0) {
		return;
	}

	if (m_cases[2].m_flags == 1) {
		Insert(2);
		int nMultiplier = 1;
		if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty()))
			nMultiplier = 2;

		m_cases[2].m_pang = -((10 - (int)m_fToPin) * m_nPangRate * nMultiplier);

		if (m_fStartToPin < 60.0f)
			m_cases[2].m_pang = (m_cases[2].m_pang + 1) / 2;
	} else if (m_flags & 4) {
		m_cases[2].SetState(0x10);
	}
}

// fontinfo.cpp:5564
void CPangInfo::LongPutt(void) {
	if (m_cases[3].m_flags == 0x100) {
		return;
	}

	if (GolfBall().m_state != 0) {
		if (m_cases[3].m_flags == 1 && GolfBall().m_preDataIndex > 0) {
			WVector dir = GolfBall().m_pos - GolfBall().PreData(0)->m_pos;
			dir.y = 0.0f;
			dir.Normalize();

			if (WDotProduct(dir, m_aimDir) > (float)cos(0.017453292f * 20.0))
				Insert(3);
		} else {
			m_cases[3].m_pang = (int)m_fDriveDistance * 2;
		}
	} else if (m_flags & 4) {
		MakeDecision(3, GolfBall().IsHoleIn());
	}
}

// fontinfo.cpp:5600
void CPangInfo::LongChipIn(void) {
	if (m_cases[4].m_flags == 0x100) {
		return;
	}

	if (GolfBall().m_state != 0) {
		if (m_cases[4].m_flags == 1 && GolfBall().m_preDataIndex > 0) {
			WVector dir = GolfBall().m_pos - GolfBall().PreData(0)->m_pos;
			dir.y = 0.0f;
			dir.Normalize();

			if (WDotProduct(dir, m_aimDir) > cos(0.017453292f * 20.0) && m_fToPin < 30.0f)
				Insert(4);
		} else {
			int bClassic = S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty());

			if (bClassic) {
				m_cases[4].m_pang = (int)m_fDriveDistance * 5;
			} else {
				m_cases[4].m_pang = (int)m_fDriveDistance * 2;
				if (!(GolfBall().m_powerFlags & 0x70) && GolfClub().GetApproachMode() == 4)
					m_cases[4].m_pang *= 2;
			}
		}
	} else {
		if (m_cases[4].m_flags == 1)
			m_cases[4].m_flags = 0x100;

		if (m_flags & 4)
			MakeDecision(4, GolfBall().IsHoleIn() == 1);
	}
}

// fontinfo.cpp:5650
void CPangInfo::BeamImpact(void) {
	if (m_cases[5].m_flags == 0x100 || GolfBall().m_state != 0) {
		return;
	}

	if (m_cases[5].m_flags == 1) {
		Insert(5);
	} else if (m_flags & 4) {
		m_cases[5].SetState(0x10);
	}
}

// fontinfo.cpp:5661
void CPangInfo::SpecialShot(void) {
	if (m_cases[7].m_flags == 0x100 || GolfBall().m_state != 0) {
		return;
	}

	if (m_cases[7].m_flags == 1) {
		Insert(7);
	} else if (m_flags & 4) {
		m_cases[7].SetState(0x10);
	}
}

// fontinfo.cpp:5672
void CPangInfo::NiceRecovery(void) {
	if (m_cases[0xF].m_flags == 0x100) {
		return;
	}

	if (m_cases[0xF].m_flags == 1) {
		if (GolfBall().m_preDataIndex > GolfBall().m_eventPreDataIndex[6]) {
			Insert(0xF);

			if (GOLFDOC()->m_pPolySoup->IsOutOfBound(GolfBall().PreData(GolfBall().m_eventPreDataIndex[6])->m_pos)) {
				m_cases[0xF].m_pang = 15;
			} else {
				m_cases[0xF].m_pang = 10;
			}
		}
	} else if (m_flags & 4) {
		if (GOLFDOC()->m_pPolySoup->IsOutOfBound(GolfBall().PreData(GolfBall().m_eventPreDataIndex[6])->m_pos)) {
			MakeDecision(0xF, !GOLFDOC()->m_pPolySoup->IsOutOfBound(GolfBall().PreData(GolfBall().m_preDataCount)->m_pos));
		} else {
			unsigned char ground = GetPVS().GetGroundType(GolfBall().PreData(GolfBall().m_preDataCount)->m_pos, true, NULL);
			MakeDecision(0xF, ground == 1 || ground == 2);
		}
	}
}

// fontinfo.cpp:5704
void CPangInfo::HoleIn(void) {
	if (m_cases[8].m_flags == 0x100 || GolfBall().m_state != 0) {
		return;
	}

	if (m_cases[8].m_flags == 1) {
		Insert(8);
	} else if (m_flags & 4) {
		if (CSharedDoc::Instance()->m_roomType == 1) {
			int team = PLAYER(MyIndex())->m_attribute;

			if ((unsigned char)TEAM(1 - team)->m_holes[GOLFDOC()->m_currentHole - 1] >= (unsigned char)TEAM(team)->m_holes[GOLFDOC()->m_currentHole - 1]) {
				m_cases[8].SetState(0x10);
			} else {
				m_cases[8].SetState(0x40);
			}
		} else if (CSharedDoc::Instance()->m_roomType == 3) {
			if ((unsigned char)PLAYER(1 - MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1] >= (unsigned char)PLAYER(MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1]) {
				m_cases[8].SetState(0x10);
			} else {
				m_cases[8].SetState(0x40);
			}
		} else {
			m_cases[8].SetState(0x10);
		}
	}
}

// fontinfo.cpp:5734
void CPangInfo::Multiply(int nCase, int nMultiply) {
	if (m_cases[nCase].m_flags == 0x100 || GolfBall().m_state != 0) {
		return;
	}

	if (m_cases[nCase].m_flags == 1) {
		Insert(nCase);
		m_cases[nCase].m_pang = nMultiply;
	} else if (m_flags & 4) {
		if (nCase == 1) {
			MakeDecision(nCase, (m_cases[0].m_flags >> 4) & 1);
		} else if (IsGameType(1) || IsGameType(3)) {
			MakeDecision(nCase, m_cases[8].m_flags != 0x100);
		} else {
			MakeDecision(nCase, GolfBall().IsHoleIn() && m_nPangRate <= 0);
		}
	}
}

// fontinfo.cpp:5758
void CPangInfo::ClashCombo(void) {
	if (m_cases[0xB].m_flags == 0x100) {
		return;
	}

	if (GolfBall().m_state != 0) {
		const std::vector<CQuadTree::sColData> &colList = GetPVS().GetColList();

		for (int i = m_nSubState; i < GolfBall().m_preDataIndex; i++) {
			std::vector<CQuadTree::sColData>::const_iterator it;
			for (it = colList.begin(); it != colList.end(); ++it) {
				if (it->m_objIndex != -1 && it->m_frame == i) {
					break;
				}
			}
			if (it == colList.end()) {
				continue;
			}

			m_cases[0xB].m_pang = (int)m_cases[0xB].m_pang * 2;

			if (m_cases[0xB].m_pang > 1 && m_cases[0xB].m_flags == 1) {
				Insert(0xB);
			}
		}
	} else if (m_flags & 4) {
		MakeDecision(0xB, GolfBall().IsHoleIn() && m_nPangRate <= 0);
	}
}

// fontinfo.cpp:5788
void CPangInfo::DormieBonus(void) {
	if (!IsGameType(1) && !IsGameType(3) || m_cases[6].m_flags == 0x100) {
		return;
	}

	if (m_cases[6].m_flags == 1) {
		Insert(6);
	} else if (m_flags & 4) {
		m_cases[6].SetState(0x10);
	}
}

// fontinfo.cpp:5799
void CPangInfo::HappyBonus(void) {
	switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
		return;
	}

	if (GolfBall().m_state != 0 || m_cases[0xC].m_flags == 0x100) {
		return;
	}

	if (m_cases[0xC].m_flags == 1) {
		Insert(0xC);
	} else if (m_flags & 4) {
		m_cases[0xC].m_pang = 20;
		m_cases[0xC].SetState(0x10);
	}
}

// fontinfo.cpp:5816
void CPangInfo::AztecBonusWater(void) {
	if (m_cases[0x11].m_flags == 0x100) {
		return;
	}

	CSharedDoc *pDoc = CSharedDoc::Instance();
	unsigned long ballId = pDoc->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId;
	__int64 bonus = 0;
	if (ballId == 0x14000005 || ballId == 0x14000080 || ballId == 0x14000016 || ballId == 0x1400003C || ballId == 0x1400003D) {
		bonus = 1;
	}

	CCardManager::Instance()->SetPlayerIndex(GOLFDOC()->m_myPlayerIndex);
	bonus += CCardManager::Instance()->GetCardBounceBonusUp();
	if (bonus <= 0) {
		return;
	}

	if (GolfBall().m_state != 0) {
		for (int i = m_nSubState; i < GolfBall().m_preDataIndex && m_nWaterHits < 20; i++) {
			if (i - m_nLastWaterHit > 5) {
				if (GolfBall().PreData(i)->m_groundType == 4 || GolfBall().PreData(i)->m_groundType == 1 || GolfBall().PreData(i)->m_groundType == 2) {
					m_nWaterHits++;
					m_nLastWaterHit = i;
				}
			}
		}

		m_cases[0x11].m_pang = (m_nWaterHits > 20 ? 20 : m_nWaterHits) * bonus;

		if (m_cases[0x11].m_pang > 0 && m_cases[0x11].m_flags == 1) {
			Insert(0x11);
		}
	} else if (m_flags & 4) {
		MakeDecision(0x11, !GolfBall().m_landed);
	}
}

// fontinfo.cpp:5945
void CPangInfo::AngelWingBonus(void) {
	if (m_cases[0x12].m_flags == 0x100) {
		return;
	}

	if (m_cases[0x12].m_flags == 1) {
		Insert(0x12);
	} else if (m_flags & 4) {
		m_cases[0x12].m_pang = 55;
		m_cases[0x12].SetState(0x40);
	}
}

// fontinfo.cpp:5961
void CPangInfo::DevilWingBonus(void) {
	if (m_cases[0x13].m_flags == 0x100) {
		return;
	}

	if (m_cases[0x13].m_flags == 1) {
		Insert(0x13);
	} else if (m_flags & 4) {
		m_cases[0x13].m_pang = 1;
		m_cases[0x13].SetState(0x10);
	}
}

// fontinfo.cpp:5977
void CPangInfo::MascotBonus(void) {
	if (m_cases[0x14].m_flags == 0x100) {
		return;
	}

	if (m_cases[0x14].m_flags == 1) {
		Insert(0x14);
	} else if (m_flags & 4) {
		m_cases[0x14].SetState(0x10);
	}
}

// fontinfo.cpp:5988
void CPangInfo::AbilityItemBonus(void) {
	if (m_cases[0x15].m_flags == 0x100) {
		return;
	}

	if (m_cases[0x15].m_flags == 1) {
		Insert(0x15);
	} else if (m_flags & 4) {
		m_cases[0x15].SetState(0x10);
	}
}

// fontinfo.cpp:5999
void CPangInfo::GimmickItem(GIMMICK_ITEM_TYPE itemType) {
	int nCase;

	switch (itemType) {
	case 0:
		nCase = 0x16;
		break;
	case 1:
		nCase = 0x17;
		break;
	default:
		return;
	}

	if (GolfBall().m_state == 0) {
		if (m_cases[nCase].m_flags == 1) {
			m_cases[nCase].m_flags = 0x100;
		}
	}

	sPangCase &pangCase = m_cases[nCase];
	if (pangCase.m_flags != 0x100) {
		GroundItemMan *pMan = (GroundItemMan *)AfxGetTask()->GetActor("GroundItemMan");
		if (pMan) {
			int nCount = pMan->m_playerStats[GOLFDOC()->m_myPlayerIndex].m_shotCount[itemType];
			if (nCount > 0) {
				if (pangCase.m_flags == 1) {
					Insert(nCase);
				}
				m_cases[nCase].m_pang = nCount;
			}
		}

		if (m_flags & 4) {
			pangCase.SetState(0x10);
			m_cases[nCase].m_fTime = 0;
		}
	}
}

// fontinfo.cpp:6047
void CPangInfo::AztecBonus(void) {
	CSharedDoc *pDoc = CSharedDoc::Instance();

	if ((pDoc->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId & 0x3FFFFFF) == 0 || m_cases[0x10].m_flags == 0x100) {
		return;
	}

	if (m_cases[0x10].m_flags == 1) {
		Insert(0x10);
	} else if (m_flags & 4) {
		m_cases[0x10].m_pang = 30;
		m_cases[0x10].SetState(0x10);
	}
}

// fontinfo.cpp:6102
void CPangInfo::ClearBonus(void) {
	switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
		return;
	}

	if (m_cases[0xD].m_flags == 0x100 || (GOLFDOC()->GameMode() & 0x80) && GolfBall().m_state != 0) {
		return;
	}

	if (m_cases[0xD].m_flags == 1) {
		Insert(0xD);
	} else if (m_flags & 4) {
		m_cases[0xD].SetState(0x10);
	}
}

// fontinfo.cpp:6116
void CPangInfo::RecoveryBonus(void) {
	if (m_cases[0xE].m_flags == 0x100) {
		return;
	}

	if (m_cases[0xE].m_flags == 1) {
		Insert(0xE);
	} else if (m_flags & 4) {
		MakeDecision(0xE, m_pangs[0] > 0 || m_pangs[1] > 0);
	}
}

// fontinfo.cpp:6128
void CPangInfo::Display(void) {
	if (CSharedDoc::Instance()->m_roomType == 10) {
		return;
	}

	if (GOLFDOC()->m_gameState < 15 && GOLFDOC()->m_gameState != 8) {
		return;
	}

	Display_TotalPang();
	Display_TotalGimmickNum();

	__int64 maxPang = Max(m_totalCases[0].m_pang, m_totalCases[1].m_pang);
	float fOffset;
	if (maxPang > 10000) {
		fOffset = 105.0f;
	} else if (maxPang > 1000) {
		fOffset = 85.0f;
	} else {
		fOffset = 65.0f;
	}

	if (!(GOLFDOC()->GameMode() & 0x81880) || m_flags == 0) {
		return;
	}

	if ((GOLFDOC()->GameMode() & 0x1000) && CSharedDoc::Instance()->m_roomType != 1
		&& CSharedDoc::Instance()->m_roomType != 3 && CSharedDoc::Instance()->m_roomType != 7) {
		return;
	}

	WRECT rcSrc;
	WRECT rcDst;
	float fScale;
	int i;
	for (i = 23; i >= 0; i--) {
		if (!(m_cases[i].m_flags & 0xFFFFFE5E)) {
			continue;
		}

		switch (m_cases[i].m_state) {
		case 2: {
			float v = m_cases[i].m_fTime * 0.8f + 0.6f;
			fScale = v > data_9eb5b8[20].f ? 1.0f : v;
			float a = m_cases[i].m_fTime * 510.0f;
			m_cases[i].m_byAlpha = (unsigned char)(a > 255.0f ? 255.0f : a);
			break;
		}
		case 4: {
			float v = (0.5f - m_cases[i].m_fTime) * 0.8f + 0.6f;
			fScale = v < 0.6f ? 0.6f : v;
			break;
		}
		default:
			fScale = 0.6f;
			break;
		}

		if (m_cases[i].m_byAlpha == 0) {
			continue;
		}

		rcDst.w = fScale * 256.0f;
		rcDst.h = fScale * 32.0f;
		rcDst.x = (float)floor(g_view->GetWidth() - 10.0f - rcDst.w - fScale * fOffset);
		rcDst.y = m_cases[i].m_fY - rcDst.h * 0.7f;

		unsigned long color = (m_cases[i].m_byAlpha << 24) | 0xFFFFFF;

		unsigned int overlay;
		if (i == 17 && CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId == 0x14000016) {
			overlay = m_nTotalOverlay;
			rcSrc = m_totalRect;
		} else {
			overlay = m_cases[i].m_nOverlay;
			rcSrc = m_cases[i].m_rect;
		}

		if (m_pPangOverlay[overlay]) {
			m_pPangOverlay[overlay]->Render(g_view, rcSrc, rcDst, 0, color, 0, 0);
		}

		m_color = color;
		if (i == 1 || i == 9 || i == 10 || i == 11 || i == 14) {
			sprintf(m_pBuffer, "x%I64d", m_cases[i].m_pang);
		} else if (i == 18) {
			if (m_cases[18].m_pang >= 0) {
				sprintf(m_pBuffer, "%I64d", m_cases[18].m_pang);
			} else {
				sprintf(m_pBuffer, "%c", '?');
			}
		} else {
			sprintf(m_pBuffer, "%I64d", m_cases[i].m_pang);
		}

		Print(g_view->GetWidth() - 10.0f, rcDst.y + 2.0f, m_pBuffer, 3.0f, 0, fScale, NULL);
	}

	switch (CSharedDoc::Instance()->m_roomType) {
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
		break;
	default:
		if (!(m_cases[16].m_flags & 0x101) && (m_flags & 8)) {
			float t = data_9eb5b8[20].f < m_fAztecBonusTime ? 1.0f : m_fAztecBonusTime;

			rcDst.w = 153.6f;
			rcDst.h = 19.2f;

			for (i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
				if (GOLFDOC()->GetPlayer(i)->m_state == 3 || i == GOLFDOC()->m_myPlayerIndex) {
					continue;
				}

				if (CSharedDoc::Instance()->m_roomType == 1 && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute == PLAYER(i)->m_attribute) {
					sprintf(m_pBuffer, "%d", 30);
				} else {
					sprintf(m_pBuffer, "%d", 5);
				}

				int idx;
				if (CSharedDoc::Instance()->m_roomType == 1) {
					if (PLAYER(i)->m_attribute == 0) {
						idx = i < (PLAYER(i)->m_partnerIndex == 0xFF);
					} else {
						idx = i < (PLAYER(i)->m_partnerIndex == 0xFF) + 2;
					}
				} else {
					idx = i;
				}

				float x = (data_9eb5b8[20].f - t) * m_fPopupX + t * m_uiPos[idx][0];
				float y = (data_9eb5b8[20].f - t) * m_fPopupY + t * m_uiPos[idx][1];
				if (m_nBonusEffects > 1) {
					y -= 10.0f;
				}
				rcDst.x = x;
				rcDst.y = y;

				if (m_pPangOverlay[m_cases[16].m_nOverlay]) {
					m_pPangOverlay[m_cases[16].m_nOverlay]->Render(g_view, m_cases[16].m_rect, rcDst, 0, 0xFFFFFFFF, 0, 0);
				}

				Print(rcDst.x + 180.0f, rcDst.y + 2.0f, m_pBuffer, 3.0f, 0, 0.6f, NULL);
			}
		}
		break;
	}

	switch (CSharedDoc::Instance()->m_roomType) {
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
		break;
	default:
		if (!(m_cases[18].m_flags & 0x101) && (m_flags & 8)) {
			float t = data_9eb5b8[20].f < m_fAngelBonusTime ? 1.0f : m_fAngelBonusTime;

			rcDst.w = 153.6f;
			rcDst.h = 19.2f;

			for (i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
				if (GOLFDOC()->GetPlayer(i)->m_state == 3 || i == GOLFDOC()->m_myPlayerIndex) {
					continue;
				}

				if (CSharedDoc::Instance()->m_roomType == 1 && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute == GOLFDOC()->GetPlayer(i)->m_attribute) {
					sprintf(m_pBuffer, "%d", GetAngelBonus(PLAYER(i)));
				} else {
					sprintf(m_pBuffer, "%d", GetAngelBonus(GOLFDOC()->GetPlayer(i)));
				}

				int idx;
				if (CSharedDoc::Instance()->m_roomType == 1) {
					if (GOLFDOC()->GetPlayer(i)->m_attribute == 0) {
						idx = i < (PLAYER(i)->m_partnerIndex == 0xFF);
					} else {
						idx = i < (PLAYER(i)->m_partnerIndex == 0xFF) + 2;
					}
				} else {
					idx = i;
				}

				int nEffects = m_nBonusEffects;
				float x;
				float y;
				if (nEffects > 1) {
					x = t * m_uiPos[idx + 4][0] + (data_9eb5b8[20].f - t) * m_fPopupX;
					y = (data_9eb5b8[20].f - t) * m_fPopupY + t * m_uiPos[idx + 4][1];
				} else {
					x = t * m_uiPos[idx][0] + (data_9eb5b8[20].f - t) * m_fPopupX;
					y = (data_9eb5b8[20].f - t) * m_fPopupY + t * m_uiPos[idx][1];
				}
				if (nEffects > 1) {
					y -= 10.0f;
				}
				rcDst.x = x;
				rcDst.y = y;

				if (m_pPangOverlay[m_cases[18].m_nOverlay]) {
					m_pPangOverlay[m_cases[18].m_nOverlay]->Render(g_view, m_cases[18].m_rect, rcDst, 0, 0xFFFFFFFF, 0, 0);
				}

				Print(rcDst.x + 180.0f, rcDst.y + 2.0f, m_pBuffer, 3.0f, 0, 0.6f, NULL);
			}
		}
		break;
	}

	if (!(m_cases[12].m_flags & 0x101) && ((m_flags & 8) || (GOLFDOC()->GameMode() & 0x1000))) {
		float t = data_9eb5b8[20].f < m_fHappyBonusTime ? 1.0f : m_fHappyBonusTime;

		sprintf(m_pBuffer, "%d", 20);

		rcDst.w = 153.6f;
		rcDst.h = 19.2f;

		for (i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			if (GOLFDOC()->GetPlayer(i)->m_state == 3 || i == GOLFDOC()->m_myPlayerIndex) {
				continue;
			}

			int idx;
			if (CSharedDoc::Instance()->m_roomType == 1) {
				if (GOLFDOC()->GetPlayer(i)->m_attribute == 0) {
					idx = i < (GOLFDOC()->GetPlayer(i)->m_partnerIndex == 0xFF);
				} else {
					idx = i < (GOLFDOC()->GetPlayer(i)->m_partnerIndex == 0xFF) + 2;
				}
			} else {
				idx = i;
			}

			float x = (data_9eb5b8[20].f - t) * m_fPopupX + t * m_uiPos[idx][0];
			float y = (data_9eb5b8[20].f - t) * m_fPopupY + t * m_uiPos[idx][1];
			if (m_nBonusEffects > 1) {
				y += 10.0f;
			}
			rcDst.x = x;
			rcDst.y = y;

			if (m_pPangOverlay[m_cases[12].m_nOverlay]) {
				m_pPangOverlay[m_cases[12].m_nOverlay]->Render(g_view, m_cases[12].m_rect, rcDst, 0, 0xFFFFFFFF, 0, 0);
			}

			Print(rcDst.x + 180.0f, rcDst.y + 2.0f, m_pBuffer, 3.0f, 0, 0.6f, NULL);
		}
	}
}

// fontinfo.cpp:6407
void CPangInfo::Display_TotalPang(void) {
	float fOffset;
	float fScale;

	__int64 nMax = Max(m_totalCases[0].m_pang, m_totalCases[1].m_pang);

	if (nMax > 10000) {
		fOffset = 105.0f;
	} else if (nMax > 1000) {
		fOffset = 85.0f;
	} else {
		fOffset = 65.0f;
	}

	int nRow = 0;
	for (int i = 0; i <= 1; i++) {
		if (i > 0 && m_totalCases[i].m_pang == 0) {
			continue;
		}

		if (m_totalCases[i].m_flags == 2) {
			float v = m_totalCases[i].m_fTime * 0.6666666f + 0.6f;
			fScale = v > 0.7f ? 0.7f : v;
		} else if (m_totalCases[i].m_flags == 4) {
			float v = (0.15f - m_totalCases[i].m_fTime) * 0.6666666f + 0.6f;
			fScale = v < 0.6f ? 0.6f : v;
		} else {
			fScale = 0.6f;
		}

		WRECT rect;
		rect.w = fScale * 256.0f;
		rect.h = fScale * 32.0f;
		rect.x = (float)floor(g_view->GetWidth() - 10.0f - rect.w - fScale * fOffset);
		rect.y = (float)floor(m_totalCases[i].m_fY - rect.h * 0.7f) + nRow * 20.0f;

		unsigned long color = (m_totalCases[i].m_byAlpha << 24) | 0xFFFFFF;
		m_color = color;

		if (m_pPangOverlay[m_totalCases[i].m_nOverlay]) {
			m_pPangOverlay[m_totalCases[i].m_nOverlay]->Render(g_view, m_totalCases[i].m_rect, rect, 0, color, 0, 0);
		}

		sprintf(m_pBuffer, "%I64d", m_totalCases[i].m_pang);
		Print(g_view->GetWidth() - 10.0f, rect.y + 2.0f, m_pBuffer, 3.0f, 0, fScale, NULL);

		nRow++;
	}
}

// fontinfo.cpp:6454
void CPangInfo::Display_TotalGimmickNum(void) {
	if (m_totalCases[2].m_pang <= 0 && m_totalCases[3].m_pang <= 0) {
		return;
	}

	int nRows = 1;
	if (m_totalCases[1].m_pang > 0) {
		nRows = 2;
	}

	float fX = g_view->GetWidth() - 10.0f;
	float fNumX = fX;

	float fRowY = nRows * 20.0f;

	for (int i = 2; i < 4; i++) {
		float fScale;
		if (m_totalCases[i].m_flags == 2) {
			float v = m_totalCases[i].m_fTime * 0.6666666f + 0.6f;
			fScale = v > 0.7f ? 0.7f : v;
		} else if (m_totalCases[i].m_flags == 4) {
			float v = (0.15f - m_totalCases[i].m_fTime) * 0.6666666f + 0.6f;
			fScale = v < 0.6f ? 0.6f : v;
		} else {
			fScale = 0.6f;
		}

		WRECT rect;
		unsigned long color = (m_totalCases[i].m_byAlpha << 24) | 0xFFFFFF;
		m_color = color;
		rect.w = fScale * 256.0f;

		rect.h = fScale * 32.0f;
		rect.y = (float)floor(m_totalCases[i].m_fY - rect.h * 0.7f) + fRowY;

		sprintf(m_pBuffer, "%I64d", m_totalCases[i].m_pang);
		fX -= Print(fX, rect.y + 2.0f, m_pBuffer, 3.0f, 0, fScale, NULL);
		fNumX -= GetExactTextWidth(m_pBuffer, 3.0f, 0.6f);

		rect.x = (float)floor(fX - rect.w - fScale * 5.0f);

		if (m_pPangOverlay[m_totalCases[i].m_nOverlay]) {
			m_pPangOverlay[m_totalCases[i].m_nOverlay]->Render(g_view, m_totalCases[i].m_rect, rect, 0, color, 0, 0);
		}

		fX = fNumX - 37.2f;
	}
}

// fontinfo.cpp:6505
void CPangInfo::Record(unsigned char index, __int64 *pangs) {
	if (pangs[0] < 0 && -pangs[0] > (__int64)(int)PLAYER(index)->m_ghostAward) {
		PLAYER(index)->m_holeRecord[GOLFDOC()->m_currentHole - 1] = -(int)PLAYER(index)->m_ghostAward;
		PLAYER(index)->m_ghostAward = 0;
	} else {
		PLAYER(index)->m_holeRecord[GOLFDOC()->m_currentHole - 1] += (int)pangs[0];
		PLAYER(index)->m_ghostAward += (int)pangs[0];
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
		TEAM(PLAYER(index)->m_attribute)->m_holeRecord[GOLFDOC()->m_currentHole - 1] = (int)PLAYER(index)->m_holeRecord[GOLFDOC()->m_currentHole - 1];
		TEAM(PLAYER(index)->m_attribute)->m_teamValue = (int)PLAYER(index)->m_ghostAward;

		if (PLAYER(index)->m_partnerIndex != 0xFF) {
			// The original calls an out-of-line WCrypticValue<int>::operator=(const
			// WCrypticValue<int> &) for these two copies, which encrypttypes.h lacks.
			PLAYER(PLAYER(index)->m_partnerIndex)->m_holeRecord[GOLFDOC()->m_currentHole - 1] = PLAYER(index)->m_holeRecord[GOLFDOC()->m_currentHole - 1];
			PLAYER(PLAYER(index)->m_partnerIndex)->m_ghostAward = PLAYER(index)->m_ghostAward;
		}
	}

	PLAYER(index)->m_extraPang += (int)pangs[1];

	switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
		break;
	default:
		if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags & 0x1000) {
			for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
				if (PLAYER(i)->m_state != 3 && i != GOLFDOC()->m_myPlayerIndex) {
					PLAYER(i)->m_extraPang += 20;
				}
			}
			if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
				TEAM(1 - PLAYER(index)->m_attribute)->m_teamValue2 += 20;
			}
		}

		if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags & 0x4000) {
			for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
				if (PLAYER(i)->m_state != 3 && i != GOLFDOC()->m_myPlayerIndex) {
					PLAYER(i)->m_extraPang += 5;
				}
			}
			if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
				TEAM(1 - PLAYER(index)->m_attribute)->m_teamValue2 += 5;
			}
		}

		if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags & 0x40000) {
			for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
				if (PLAYER(i)->m_state != 3 && i != GOLFDOC()->m_myPlayerIndex) {
					PLAYER(i)->m_extraPang += GetAngelBonus(PLAYER(i));
				}
			}
		}

		if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags & 0x80000) {
			for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
				PLAYER(i);
			}
		}

		if (IsLocalContent((localContentType_t)3)) {
			if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags & 0x100000) {
				for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
					PLAYER(i);
				}
			} else if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags & 0x200000) {
				for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
					PLAYER(i);
				}
			}
		}

		if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags & 0x400000) {
			for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
				PLAYER(i);
			}
		} else if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags & 0x800000) {
			for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
				PLAYER(i);
			}
		}
		break;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
		TEAM(PLAYER(index)->m_attribute)->m_teamValue2 = (int)PLAYER(index)->m_extraPang;
		if (PLAYER(index)->m_partnerIndex != 0xFF) {
			PLAYER(PLAYER(index)->m_partnerIndex)->m_extraPang = (int)PLAYER(index)->m_extraPang;
		}
	}
}

// fontinfo.cpp:6664
void CPangInfo::Restore(void) {
	m_flags = 2;
	m_nState = 0;
	m_pszSfx = NULL;
	m_nSubState = 0;
	m_nWaterHits = 0;
	m_nLastWaterHit = -1;

	for (int i = 0; i < 4; i++) {
		m_totalCases[i].m_flags = 8;
		m_totalCases[i].m_pang = m_bonusPangs[i];
		m_shownPangs[i] = m_totalCases[i].m_pang;
		m_totalCases[i].m_fTime = 0;
	}

	m_fFinalizeTime = 0;

	for (int j = 0; j < 24; j++) {
		if (m_cases[j].m_flags != 0x100)
			m_cases[j].m_flags = 1;
		m_cases[j].m_nIndex = -1;
		m_cases[j].m_fTime = 0;
		m_cases[j].m_byAlpha = 0;
	}

	m_cases[11].m_pang = 1;
}

// fontinfo.cpp:6698
void CPangInfo::Skip(void) {
	if (m_pszSfx) {
		g_audio->StopSfx(m_pszSfx);
	}

	for (int i = 0; i < 4; i++) {
		int nPang = (int)(m_bonusPangs[i] + m_pangs[i]);
		m_totalCases[i].m_pang = nPang > 0 ? nPang : 0;
	}
}

// fontinfo.cpp:6723
CCalipers::CCalipers(void) {
	Init("[font_bar.jpg", NULL);
	m_pBuffer = new char[0x20];
	m_nFlag = 1;

	m_pIndicatorOverlay = g_resrcmng->GetOverlay("{indicator.bmp", 0);

	m_pCalipersOverlay = g_resrcmng->GetOverlay("callipers_o.png", 0);
	m_fSoundTime = 0;
	m_bKeyMoving = false;
	m_fDisplayPos = 0;
	m_fMoveDelta = 0;
	m_bSoundPlayed = false;
	m_bResetFlag = true;

	m_fsm.addStateTransition(0, 1, 1);
	m_fsm.addStateTransition(0, 2, 2);
	m_fsm.addStateTransition(1, 2, 0);
	m_fsm.addStateTransition(1, 0, 0);
	m_fsm.addStateTransition(2, 1, 0);
	m_fsm.addStateTransition(2, 0, 0);

	m_fsm.setCurrentState(0);

	ResetBar();
}

// fontinfo.cpp:6755
CCalipers::~CCalipers() {
	if (g_resrcmng && m_pIndicatorOverlay) {
		g_resrcmng->Release(m_pIndicatorOverlay);
		m_pIndicatorOverlay = NULL;
	}
	if (g_resrcmng && m_pCalipersOverlay) {
		g_resrcmng->Release(m_pCalipersOverlay);
		m_pCalipersOverlay = NULL;
	}
}

// fontinfo.cpp:6774
void CCalipers::SetCalipersLimit(float fLimit) {
	m_fCursorPos = m_fDisplayPos = fLimit;

	switch (GOLFDOC()->GameMode()) {
	case 8:
	case 0x100:
	case 0x200:
	case 0x8000:
	case 0x40000:
		if (m_nPowerBarState == 1) {
			float fMin = GolfClub().GetNarrowAccuracy() + data_af23d0 * 0.0f + data_9eb5b8[28].f;
			GOLFDOC()->GetBar().m_impact = fLimit < fMin ? fMin : (fLimit > data_9eb5b8[26].f ? data_9eb5b8[26].f : fLimit);
		}
		break;
	}
}

// fontinfo.cpp:6798
void CCalipers::ProcKeyState(float fDelta) {
	if (CMouseCursor::Instance()->m_area == 5) {
		m_bAccuracyFixed = true;
		return;
	}

	int nLeft = g_input->Get("CALIPERS_LEFT", true);
	int nRight = g_input->Get("CALIPERS_RIGHT", true);
	if (nLeft == 1) {
		m_keyFlags |= nLeft;
	}
	if (nRight == 1) {
		m_keyFlags |= 2;
	}

	unsigned long flags = m_keyFlags;
	int nAction = 0;
	if (flags == 0) {
		if (m_prevKeyFlags == 1 || m_prevKeyFlags == 2) {
			data_af23d8 = 0;
			m_fBarScale = 0.35f;
		}
	} else if (data_af23d8 > m_fBarScale && (flags == 1 || flags == 2)) {
		nAction = 2;
		data_af23d8 = data_af23d8 - m_fBarScale;
		m_fBarScale = 0.05f;
	} else if (!((flags & 1) && (flags & 2)) && flags != 0) {
		if (data_af23d8 > data_9eb5b8[7].f) {
			nAction = 1;
			data_af23d8 = data_af23d8 + fDelta;
		} else if (flags != 0 && !(data_af23d8 < data_9eb5b8[7].f)) {
			data_af23d8 = data_af23d8 + fDelta;
		}
	}

	ChangeKeyState();

	if (nAction != 0) {
		unsigned long state = 0;
		switch (nAction) {
		case 1:
			if (!m_bPowerFixed) {
				m_bPowerFixed = true;
				GOLFDOC()->GetBar().m_impact = m_fDisplayPos;
				m_nPowerBarState = 1;
			} else {
				ExecuteKeyAction(m_curState);
			}
			return;
		case 2:
			state = m_curState;
			break;
		}

		switch (state) {
		case 0:
			break;
		case 1:
			MoveLeft();
			break;
		case 2:
			MoveRight();
			break;
		}
	}

	m_prevKeyFlags = m_keyFlags;
	m_keyFlags = 0;
}

// fontinfo.cpp:6882
void CCalipers::ExecuteKeyAction(int nKey) {
	switch (nKey) {
	case 0:
		MoveNone();
		break;

	case 1:
		MoveLeft();
		break;

	case 2:
		MoveRight();
		break;
	}
}

// fontinfo.cpp:6898
void CCalipers::ChangeKeyState(void) {
	switch (m_curState) {
	case 0:
		if (m_keyFlags & 1) {
			SetEvent(1);
		} else if (m_keyFlags & 2) {
			SetEvent(2);
		}

		m_fMoveSpeed = 1.0f;
		break;

	case 1:
		if (m_prevState != 1) {
			if (!(m_keyFlags & 1)) {
				KeyInputEnd();
			}
		}

		if (m_keyFlags & 2) {
			SetEvent(2);
		}
		break;

	case 2:
		if (m_prevState != 2) {
			if (!(m_keyFlags & 2)) {
				KeyInputEnd();
			}
		}

		if (m_keyFlags & 1) {
			SetEvent(1);
		}
		break;
	}
}

// fontinfo.cpp:6958
void CCalipers::SetEvent(int nEvent) {
	m_fsm.stateTransition(nEvent);
	m_prevState = m_curState;
	m_curState = m_fsm.getCurrentStateID();
}

// fontinfo.cpp:6965
void CCalipers::MoveNone(void) {
}

// fontinfo.cpp:6970
void CCalipers::MoveLeft(void) {
	m_bKeyMoving = true;

	if (GolfClub().GetType() != 3) {
		m_fCursorPos = GolfClub().GetNarrowAccuracy() + data_9eb5b8[28].f > m_fCursorPos - m_fMoveSpeed ? GolfClub().GetNarrowAccuracy() + data_9eb5b8[28].f : m_fCursorPos - m_fMoveSpeed;
	} else {
		m_fCursorPos = 144.0f > m_fCursorPos - m_fMoveSpeed ? 144.0f : m_fCursorPos - m_fMoveSpeed;
	}

	if (m_fMoveSpeed <= data_9eb5b8[21].f) {
		m_fMoveSpeed += 0.5f;
	}

	m_fDisplayPos = m_fCursorPos;
}

// fontinfo.cpp:6988
void CCalipers::MoveRight(void) {
	m_bKeyMoving = true;

	float pos = m_fCursorPos - m_fMoveSpeed * -1.0f;
	pos = data_9eb5b8[26].f < pos ? data_9eb5b8[26].f : pos;
	m_fCursorPos = pos;

	if (m_fMoveSpeed <= data_9eb5b8[21].f) {
		m_fMoveSpeed += 0.5f;
	}

	m_fDisplayPos = pos;
}

// fontinfo.cpp:7003
void CCalipers::KeyInputEnd(void) {
	m_fMoveSpeed = 1.0f;
	SetEvent(0);
	m_bKeyMoving = false;
}

// fontinfo.cpp:7010
void CCalipers::KeyCheck(float fDelta) {
	unsigned char bLeft = (unsigned char)g_input->Get("CALIPERS_LEFT", true);
	unsigned char bRight = (unsigned char)g_input->Get("CALIPERS_RIGHT", true);

	if (bLeft && bRight) {
		data_a7f8c0 = 1.0f;
	} else {
		if (bLeft) {
			if (!m_bKeyMoving) {
				if (GolfClub().GetType() != 3) {
					data_af23d4 = (GolfClub().GetNarrowAccuracy() + 360.0f) / 4.0f + GolfClub().GetNarrowAccuracy() + data_9eb5b8[28].f;
				} else {
					data_af23d4 = (GolfClub().GetNarrowAccuracy() + 360.0f) / 4.0f + 144.0f;
				}
				m_bKeyMoving = true;
			}
		} else if (bRight) {
			if (!m_bKeyMoving) {
				data_af23d4 = 3.0f * ((GolfClub().GetNarrowAccuracy() + 360.0f) / 4.0f) + GolfClub().GetNarrowAccuracy() + data_9eb5b8[28].f;
				m_bKeyMoving = true;
			}
		} else {
			data_a7f8c0 = 1.0f;
			return;
		}

		if (bLeft) {
			CClub &club = GolfClub();
			if (club.GetType() != 3) {
				float fNext = data_af23d4 - data_a7f8c0;
				data_af23d4 = GolfClub().GetNarrowAccuracy() + data_9eb5b8[28].f > fNext ? GolfClub().GetNarrowAccuracy() + data_9eb5b8[28].f : fNext;
			} else {
				data_af23d4 = 144.0f > data_af23d4 - data_a7f8c0 ? 144.0f : data_af23d4 - data_a7f8c0;
			}
		} else {
			float fNext = data_af23d4 - data_a7f8c0 * -1.0f;
			data_af23d4 = data_9eb5b8[26].f < fNext ? 500.0f : fNext;
		}

		if (g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true)) {
			data_a7f8c0 = fDelta * 0.5f;
		} else if (data_a7f8c0 <= data_9eb5b8[27].f) {
			data_a7f8c0 = fDelta * 25.0f + data_a7f8c0;
		}
	}

	if (m_bKeyMoving) {
		m_fDisplayPos = data_af23d4;
	}
}

// fontinfo.cpp:7101
void CCalipers::Reset(void) {
	m_fMoveDelta = 0;
	m_bResetFlag = true;
	m_bKeyMoving = false;

	if (IsLocalContent((localContentType_t)0x4F)) {
		if (WSingleton<CSharedDoc>::Instance()->m_replayPlayType != 1)
			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId = 0;
	} else {
		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId = 0;
	}

	float y = COption::Instance()->gGetBar_Y() - 23.0f;
	if (g_input->GetMousePoint().y > y)
		g_input->SetMousePoint(g_input->GetMousePoint().x, y);
}

// fontinfo.cpp:7120
void CCalipers::ResetBar(void) {
	m_fBarScale = 0.5f;
	m_prevKeyFlags = 0;
	m_keyFlags = 0;
	m_curState = 0;
	m_prevState = 0;
	m_fMoveSpeed = 1.0f;
	m_nPowerBarState = 0;
	m_bPowerFixed = false;
	m_bAccuracyFixed = false;
}

// fontinfo.cpp:7135
void CCalipers::DrawShotPowerRange(float &fPos, bool bImpact) {
	float fOffset = GolfClub().GetType() == 3 ? -8.0f : 0.0f;
	if (bImpact == true) {
		fPos = GOLFDOC()->GetBar().m_impact;
	} else {
		fPos = GOLFDOC()->GetBar().m_power;
	}

	if (fPos > data_9eb5b8[26].f) {
		fPos = 500.0f;
		sprintf(m_pBuffer, "%.1fy", GolfClub().GetRange(NULL));
	} else {
		sprintf(m_pBuffer, "%.1fy", GolfClub().GetRange(NULL) * ((fPos - data_9eb5b8[28].f) / data_af23d0));
	}

	SetColor(0xFF1AFD00);
	Print(fPos + 3.0f, COption::Instance()->gGetBar_Y() - 40.0f + fOffset, m_pBuffer, 1.0f, 0, 1.0f, NULL);
}

// fontinfo.cpp:7161
void CCalipers::DrawLimitLine(float &fPos, bool bImpact) {
	float fUvH = 1.0f, fRcH = 32.0f;

	if (!bImpact) {
		fUvH = 0.375f;
		fRcH = 12.0f;
	}

	if (!GOLFDOC()->GetBar().m_state && GolfClub().GetType() != 3) {
		m_pCalipersOverlay->Render(g_view, WRect(0.75f, 0.0f, 0.125f, fUvH), WRect(fPos - 7.0f, COption::Instance()->gGetBar_Y() - 21.0f, 16.0f, fRcH), 0, 0xFFFFFFFF, 0.0f, 0);
		return;
	}

	if (m_bSoundPlayed) {
		m_pCalipersOverlay->Render(g_view, WRect(0.75f, 0.0f, 0.125f, fUvH), WRect(fPos - 7.0f, COption::Instance()->gGetBar_Y() - 21.0f, 16.0f, fRcH), 0, 0xFFFFFFFF, 0.0f, 0);

		if (m_fSoundTime >= 0.1f && m_fSoundTime <= 0.2f) {
			m_pCalipersOverlay->Render(g_view, WRect(0.875f, 0.0f, 0.125f, fUvH), WRect(fPos - 7.0f, COption::Instance()->gGetBar_Y() - 21.0f, 16.0f, fRcH), 0, 0xFFFFFFFF, 0.0f, 0);
		}
	} else {
		m_pCalipersOverlay->Render(g_view, WRect(0.75f, 0.0f, 0.125f, fUvH), WRect(fPos - 7.0f, COption::Instance()->gGetBar_Y() - 21.0f, 16.0f, fRcH), 0, 0xFFFFFFFF, 0.0f, 0);
	}
}

// fontinfo.cpp:7196
void CCalipers::Process(float fDelta) {
	ProcKeyState(fDelta);

	if (!GOLFDOC()->m_clockHold) {
		WVector mouse = g_input->GetMousePoint();
		float fMouseX = mouse.x;

		if (CMouseCursor::Instance()->m_area == 5) {
			float fLimit = GolfClub().GetType() == 3 ? data_9eb5b8[28].f : GolfClub().GetNarrowAccuracy() + data_9eb5b8[28].f;

			if (mouse.x > fLimit) {
				if (mouse.x > data_9eb5b8[26].f) {
					fMouseX = 500.0f;
				}

				float fPos = fMouseX;
				m_fDisplayPos = fPos;
				m_fCursorPos = fPos;
				m_fMoveDelta = fPos;
				GOLFDOC()->GetBar().m_impact = (float)floor(Between(GolfClub().GetNarrowAccuracy() + data_9eb5b8[28].f, fPos, 500.0f));

				CMouseCursor::Instance()->SetCursor(7);
				m_bKeyMoving = false;
			}
		} else {
			if (m_bAccuracyFixed == true) {
				GOLFDOC()->GetBar().m_impact = 0;
				m_bAccuracyFixed = false;
				m_bPowerFixed = false;
				m_nPowerBarState = 0;
			}

			if (m_bKeyMoving) {
				GOLFDOC()->GetBar().m_impact = (float)floor(Between(GolfClub().GetNarrowAccuracy() + data_af23d0 * 0.0f + data_9eb5b8[28].f, m_fDisplayPos, 500.0f));
			}
		}
	}

	if (m_pCalipersOverlay) {
		if (GOLFDOC()->GetBar().m_impact > data_9eb5b8[28].f) {
			if (m_fDisplayPos > data_9eb5b8[28].f) {
				if (m_fDisplayPos - 5.0f <= GOLFDOC()->GetBar().m_accuracy && m_fDisplayPos + 5.0f >= GOLFDOC()->GetBar().m_accuracy) {
					if (!m_bSoundPlayed && m_bResetFlag) {
						if ((GOLFDOC()->m_clockHold == 2 && GOLFDOC()->GetBar().m_state == -1) || GolfClub().GetType() == 3) {
							m_bSoundPlayed = true;
							m_fSoundTime = 0;
							m_bResetFlag = false;
							g_audio->PlaySfx("½ÃÄÀ°Ë¸»", NULL, 0, NULL, NULL, 0.5f, 200.0f);
						}
					}
				}
			}
		}

		if (m_bSoundPlayed) {
			m_fSoundTime = m_fSoundTime + fDelta;
			if (m_fSoundTime > 0.4f) {
				m_bSoundPlayed = false;
				m_fSoundTime = 0;
			}
		}
	}
}

// fontinfo.cpp:7271
void CCalipers::Display(void) {
	WRect uv(0.0f, 0.0f, 0.5f, 1.0f);
	WRect rc(150.0f, COption::Instance()->gGetBar_Y() + data_9eb5b8[21].f, 64.0f, 32.0f);
	m_pCalipersOverlay->Render(g_view, uv, rc, 0, 0xFFFFFFFF, 0.0f, 0);

	if (m_nPowerBarState == 1) {
		float fPos = 0;
		DrawShotPowerRange(fPos, true);

		if (m_pCalipersOverlay) {
			WVector mouse = g_input->GetMousePoint();
			float fLimit = GolfClub().GetType() == 3 ? data_9eb5b8[28].f : GolfClub().GetNarrowAccuracy() + data_9eb5b8[28].f;

			if (mouse.x > fLimit) {
				if (g_input->Get("CALIPERS_LEFT", true) || m_fMoveDelta - m_fDisplayPos > 0) {
					WRect rcLeft(152.0f, COption::Instance()->gGetBar_Y() + 22.0f, 32.0f, 32.0f);
					WRect uvArrow(0.5f, 0.0f, 0.25f, 1.0f);
					m_pCalipersOverlay->Render(g_view, uvArrow, rcLeft, 0, 0xFFFFFFFF, 0.0f, 0);
				}

				if (g_input->Get("CALIPERS_RIGHT", true) || (m_fMoveDelta > 0 && m_fMoveDelta - m_fDisplayPos < 0)) {
					WRect rcRight(174.0f, COption::Instance()->gGetBar_Y() + 22.0f, 32.0f, 32.0f);
					WRect uvArrow(0.5f, 0.0f, 0.25f, 1.0f);
					m_pCalipersOverlay->Render(g_view, uvArrow, rcRight, 0, 0xFFFFFFFF, 0.0f, 0);
				}
			}

			DrawLimitLine(fPos, true);
		}
	}
}

// fontinfo.cpp:7307
CHolePoseSkins::CHolePoseSkins(void) {
	Init("[bigfont.jpg", NULL);

	m_pBuffer = new char[0x20];

	m_nValue = 0;
	m_nResult = 0;
	m_fAlpha = 0.0f;
	m_nState = 0;
	m_fScale = 2.0f;

	m_pWinOverlay[0] = g_resrcmng->GetOverlay("skins_win01.jpg", 0);
	m_pWinOverlay[1] = g_resrcmng->GetOverlay("skins_win02.jpg", 0);
	m_pWinOverlay[2] = g_resrcmng->GetOverlay("skins_win03.jpg", 0);

	m_pLoseOverlay[0] = g_resrcmng->GetOverlay("skins_lose01.jpg", 0);
	m_pLoseOverlay[1] = g_resrcmng->GetOverlay("skins_lose02.jpg", 0);

	m_pDrawOverlay[0] = g_resrcmng->GetOverlay("skins_draw01.jpg", 0);
	m_pDrawOverlay[1] = g_resrcmng->GetOverlay("skins_draw02.jpg", 0);
}

// fontinfo.cpp:7331
CHolePoseSkins::~CHolePoseSkins() {
	if (g_resrcmng && m_pWinOverlay[0]) {
		g_resrcmng->Release(m_pWinOverlay[0]);
		m_pWinOverlay[0] = NULL;
	}
	if (g_resrcmng && m_pWinOverlay[1]) {
		g_resrcmng->Release(m_pWinOverlay[1]);
		m_pWinOverlay[1] = NULL;
	}
	if (g_resrcmng && m_pWinOverlay[2]) {
		g_resrcmng->Release(m_pWinOverlay[2]);
		m_pWinOverlay[2] = NULL;
	}

	if (g_resrcmng && m_pLoseOverlay[0]) {
		g_resrcmng->Release(m_pLoseOverlay[0]);
		m_pLoseOverlay[0] = NULL;
	}
	if (g_resrcmng && m_pLoseOverlay[1]) {
		g_resrcmng->Release(m_pLoseOverlay[1]);
		m_pLoseOverlay[1] = NULL;
	}

	if (g_resrcmng && m_pDrawOverlay[0]) {
		g_resrcmng->Release(m_pDrawOverlay[0]);
		m_pDrawOverlay[0] = NULL;
	}
	if (g_resrcmng && m_pDrawOverlay[1]) {
		g_resrcmng->Release(m_pDrawOverlay[1]);
		m_pDrawOverlay[1] = NULL;
	}
}

// fontinfo.cpp:7344
void CHolePoseSkins::Process(float fDelta) {
	if (!(GOLFDOC()->GameMode() & 0x1000)) {
		m_nState = 0;
		return;
	}

	switch (m_nState) {
	case 1:
		if (m_fScale > 0.0f) {
			m_fScale -= fDelta;
			if (m_fScale < 0.0f) {
				m_nState = 2;
			}
		}
		break;
	case 2:
		m_fAlpha -= fDelta * 510.0f;
		if (m_fAlpha < 0.0f) {
			m_fAlpha = 0;
			m_nState = 0;
		}
		break;
	}
}

// fontinfo.cpp:7375
void CHolePoseSkins::SetPose(int nResult, int nValue) {
	if (m_nState != 0) {
		return;
	}

	m_nResult = nResult;
	m_nValue = nValue;
	m_fAlpha = 255.0f;
	m_nState = 1;
	m_fScale = 2.0f;

	if (nResult > 0) {
		m_pPoseOverlay[0] = m_pWinOverlay[0];
	} else if (nResult == 0) {
		m_pPoseOverlay[0] = m_pDrawOverlay[0];
		m_pPoseOverlay[1] = m_pDrawOverlay[1];
	} else if (nResult < 0) {
		m_pPoseOverlay[0] = m_pLoseOverlay[0];
		m_pPoseOverlay[1] = m_pLoseOverlay[1];
	}
}

// fontinfo.cpp:7402
void CHolePoseSkins::Display(void) {
	if (m_nState == 0) {
		return;
	}

	WRect uv(0.0f, 0.0f, 1.0f, 1.0f);
	WRect rc;
	unsigned long color = ((int)m_fAlpha << 24) + 0xFFFFFF;

	if (m_pPoseOverlay[0]) {
		rc.w = (float)m_pPoseOverlay[0]->GetWidth();
		rc.h = (float)m_pPoseOverlay[0]->GetHeight();
		rc.x = g_view->GetWidth() * 0.5f - rc.w * 0.5f;
		rc.y = g_view->GetHeight() * 0.25f - rc.h * 0.5f;
		m_pPoseOverlay[0]->Render(g_view, uv, rc, 0, color, 0.0f, 0);
	}

	if (m_nResult > 0) {
		sprintf(m_pBuffer, "%d", m_nValue);
		int len = (int)strlen(m_pBuffer);
		float fTextW = (float)(m_font.m_pFontInfo->fontw * len) * 0.5f + len * 6.0f;

		if (m_pWinOverlay[1]) {
			rc.w = (float)m_pWinOverlay[1]->GetWidth();
			rc.h = (float)m_pWinOverlay[1]->GetHeight();
			rc.x = g_view->GetWidth() * 0.5f - fTextW * 0.5f - rc.w;
			rc.y = rc.y + rc.h + 50.0f;
			m_pWinOverlay[1]->Render(g_view, uv, rc, 0, color, 0.0f, 0);
		}

		m_nFlag = 1;
		m_color = ((unsigned char)(int)m_fAlpha << 24) | 0xFFDE44;
		Print(g_view->GetWidth() * 0.5f + 5.0f, rc.y + 3.0f, m_pBuffer, 3.0f, 0, 1.0f, NULL);

		if (m_pWinOverlay[2]) {
			rc.w = (float)m_pWinOverlay[2]->GetWidth();
			rc.h = (float)m_pWinOverlay[2]->GetHeight();
			rc.x = (fTextW + g_view->GetWidth()) * 0.5f;
			m_pWinOverlay[2]->Render(g_view, uv, rc, 0, color, 0.0f, 0);
		}
	} else {
		if (m_pPoseOverlay[1]) {
			rc.w = (float)m_pPoseOverlay[1]->GetWidth();
			rc.h = (float)m_pPoseOverlay[1]->GetHeight();
			rc.x = g_view->GetWidth() * 0.5f - rc.w * 0.5f;
			rc.y = rc.y + rc.h + 50.0f;
			m_pPoseOverlay[1]->Render(g_view, uv, rc, 0, color, 0.0f, 0);
		}
	}
}

// fontinfo.cpp:7475
CExtraHoleInfo::CExtraHoleInfo(void) {
	Init("[font_hole.jpg", NULL);
	m_pBuffer = new char[0x20];
	m_nFlag = 2;

	for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		m_shotTime[i] = 0;
		m_cupDistance[i] = 0;
	}

	m_pPlayerNumUI = new CSimpleUI("[nplayer.jpg");
	m_pPlayerNumUI->Add("1", WRect(0.0f, 0.0f, 32.0f, 16.0f), false);
	m_pPlayerNumUI->Add("2", WRect(32.0f, 0.0f, 32.0f, 16.0f), false);
	m_pPlayerNumUI->Add("3", WRect(0.0f, 16.0f, 32.0f, 16.0f), false);
	m_pPlayerNumUI->Add("4", WRect(32.0f, 16.0f, 32.0f, 16.0f), false);

	m_leader = 0xFF;
}

// fontinfo.cpp:7497
CExtraHoleInfo::~CExtraHoleInfo() {
	if (m_pPlayerNumUI) {
		delete m_pPlayerNumUI;
		m_pPlayerNumUI = NULL;
	}
}

// fontinfo.cpp:7502
void CExtraHoleInfo::SetShotTime(unsigned long time) {
	m_shotTime[GOLFDOC()->m_myPlayerIndex] = time;
}

// fontinfo.cpp:7507
void CExtraHoleInfo::UpdateLeader(void) {
	if (GOLFDOC()->m_approachMode) {
		WVector cup = GOLFDOC()->GetHoleData().m_cupPos;
		WVector *pBall = &PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos;

		float dx = pBall->x - cup.x;
		float dz = pBall->z - cup.z;
		float dist = sqrtf(dx * dx + dz * dz) * 0.3125f;
		m_cupDistance[GOLFDOC()->m_myPlayerIndex] = (unsigned long)floor(dist * 100.0f);

		if (m_leader == 0xFF) {
			m_leader = GOLFDOC()->m_myPlayerIndex;
		} else {
			unsigned char me = GOLFDOC()->m_myPlayerIndex;
			if (m_cupDistance[me] < m_cupDistance[m_leader] || m_cupDistance[me] == m_cupDistance[m_leader] && m_shotTime[me] < m_shotTime[m_leader]) {
				m_leader = me;
			}
		}
	}
}

// fontinfo.cpp:7527
void CExtraHoleInfo::Display(void) {
	if (GOLFDOC()->m_approachMode) {
		for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			if (GOLFDOC()->GetPlayer(i)->m_state != 3) {
				if (m_pPlayerNumUI) {
					m_pPlayerNumUI->SetColor(GOLFDOC()->GetPlayer(i)->m_color | 0xFF000000);
					m_pPlayerNumUI->Render(MakeStr("%d", i + 1), g_view->GetWidth() - 161.0f, i * 23.0f + 213.0f, 1.0f, 0);
				}

				if (GOLFDOC()->GetPlayer(i)->m_state == 1) {
					m_color = 0xFFFF0A0A;
				} else {
					m_color = (i != m_leader ? 0x2DFF : 0) + 0xFFFFD200;
				}

				if (GOLFDOC()->GetPlayer(i)->m_holes[GOLFDOC()->m_currentHole - 1] == (unsigned char)0) {
					Print(g_view->GetWidth() - 85.0f, i * 23.0f + 213.0f, "-", 3.0f, 0, 1.0f, NULL);
					Print(g_view->GetWidth() - 25.0f, i * 23.0f + 213.0f, "-", 3.0f, 0, 1.0f, NULL);
				} else {
					if (m_cupDistance[i] > 0) {
						Print(g_view->GetWidth() - 65.0f, i * 23.0f + 213.0f, MakeStr("%d.%dy", m_cupDistance[i] / 100, m_cupDistance[i] % 100), 3.0f, 0, 1.0f, NULL);
					} else {
						Print(g_view->GetWidth() - 85.0f, i * 23.0f + 213.0f, "-", 3.0f, 0, 1.0f, NULL);
					}

					if (m_shotTime[i] > 0) {
						Print(g_view->GetWidth() - 5.0f, i * 23.0f + 213.0f, MakeStr("%d.%03d", m_shotTime[i] / 1000, m_shotTime[i] % 1000), 3.0f, 0, 1.0f, NULL);
					} else {
						Print(g_view->GetWidth() - 25.0f, i * 23.0f + 213.0f, "-", 3.0f, 0, 1.0f, NULL);
					}
				}
			}
		}
	}
}

// fontinfo.cpp:7577
CFontOverlay::CFontOverlay(void) {
	m_pHoleCupInfo = NULL;
	m_pFlyInfo = NULL;
	m_pHoleInfo = NULL;
	m_pAccuracyInfo = NULL;
	m_pTimeOutInfo = NULL;

	Init();
}

// fontinfo.cpp:7590
CFontOverlay::~CFontOverlay() {
	if (m_pHoleCupInfo) {
		delete m_pHoleCupInfo;
		m_pHoleCupInfo = NULL;
	}
	if (m_pFlyInfo) {
		delete m_pFlyInfo;
		m_pFlyInfo = NULL;
	}
	if (m_pHoleInfo) {
		delete m_pHoleInfo;
		m_pHoleInfo = NULL;
	}
	if (m_pAccuracyInfo) {
		delete m_pAccuracyInfo;
		m_pAccuracyInfo = NULL;
	}
	if (m_pTimeOutInfo) {
		delete m_pTimeOutInfo;
		m_pTimeOutInfo = NULL;
	}
}

// fontinfo.cpp:7602
void CFontOverlay::Init(void) {
	m_pHoleCupInfo = new CHoleCupInfo;
	m_pFlyInfo = new CFlyInfo;
	m_pHoleInfo = new CHoleInfo;
	m_pAccuracyInfo = new CAccuracyInfo;
	m_pTimeOutInfo = new CTimeOutInfo;

	m_bShowHoleCup = false;
	m_bShowFly = false;
	m_bShowHole = true;
	m_bShowAccuracy = true;
	m_bShowTimeOut = true;
}

// fontinfo.cpp:7620
void CFontOverlay::Process(float fDelta) {
	if (m_bShowHoleCup && GOLFDOC()->m_gameState >= 4) {
		if (m_pHoleCupInfo) {
			m_pHoleCupInfo->Process(fDelta);
		}
	}

	if (m_bShowFly) {
		if (m_pFlyInfo) {
			m_pFlyInfo->Process(fDelta);
		}
	}

	if (GOLFDOC()->m_gameState == 7 || GOLFDOC()->m_gameState >= 8) {
		if (m_pHoleInfo) {
			m_pHoleInfo->Process(fDelta);
		}
	}

	if (m_bShowAccuracy) {
		if (m_pAccuracyInfo) {
			m_pAccuracyInfo->Process(fDelta);
		}
	}

	if (m_bShowTimeOut) {
		if (m_pTimeOutInfo) {
			m_pTimeOutInfo->Process(fDelta);
		}
	}
}

// fontinfo.cpp:7653
void CFontOverlay::Display(void) {
	if (m_bShowHoleCup && GOLFDOC()->m_gameState >= 4 || GOLFDOC()->m_gameState == 1) {
		if (m_pHoleCupInfo) {
			m_pHoleCupInfo->Display();
		}
	}

	if (m_bShowFly) {
		if (m_pFlyInfo) {
			m_pFlyInfo->Display();
		}
	}

	if (GOLFDOC()->m_gameState == 7 || GOLFDOC()->m_gameState >= 8) {
		if (m_pHoleInfo) {
			m_pHoleInfo->Display();
		}
	}

	if (m_bShowAccuracy) {
		if (m_pAccuracyInfo) {
			m_pAccuracyInfo->Display();
		}
	}

	if (m_bShowTimeOut) {
		if (m_pTimeOutInfo) {
			m_pTimeOutInfo->Display();
		}
	}
}

// fontinfo.cpp:7694
void CFontOverlay::ResetUiPosition(void) {
	if (m_pHoleCupInfo) {
		m_pHoleCupInfo->ResetUiPosition();
	}

	if (m_pHoleInfo) {
		m_pHoleInfo->ResetUiPosition();
	}
}

CPangInfo::sPangCase::sPangCase(void) {
}
