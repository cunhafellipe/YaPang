// JP-only game code: the JP CLobbyMain's four number fonts
// (JpLobbyTitleFonts.h).
#include "JpLobbyTitleFonts.h"

#include <string.h>

#include "JpMakeRoomDlg.h"
#include "RefreshSharedPtr.h"
#include "fresh.h"
#include "wfont.h"
#include "wresrcmng.h"

// lib: 0x6f5cb0, 0x6f5ce0, 0x6f5d10, 0x6f5d40 (the scalar deleting
// destructors, the only slot of vtables 0xd1c244, 0xd1c264, 0xd1c2a0,
// 0xd1c2a8)
// lib: 0xab8a80, 0xab8b20, 0x55b7e0, 0xab8bc0 (std::tr1::shared_ptr's
// constructor from each class's pointer)

// 0xd1c234: the GP time count's character set.
static char s_timeCountChars[] = "0123456789";

// 0xe169cc: the texture entry (0xd1c248).
static char s_timeCountFontName[] = "gp_timecount_num.tga";
static char* s_timeCountFontFile = s_timeCountFontName;

// 0x6f5180
CGPTimeCountFont::CGPTimeCountFont(void) {
	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = &s_timeCountFontFile;
	info.numPages = 1;
	info.fontw = 22;
	info.fonth = 28;
	info.texw = 220;
	info.texh = 28;
	info.pCharSet = s_timeCountChars;

	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

// 0xe169fc: the texture entry (0xd1c268).
static char s_timeCountBigFontName[] = "gp_timecount_num_big.tga";
static char* s_timeCountBigFontFile = s_timeCountBigFontName;

// 0x6f5240
CGPTimeCountBigFont::CGPTimeCountBigFont(void) {
	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = &s_timeCountBigFontFile;
	info.numPages = 1;
	info.fontw = 28;
	info.fonth = 36;
	info.texw = 280;
	info.texh = 36;
	info.pCharSet = s_timeCountChars;

	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

// 0xe16a30: the texture entry (0xd064c4, the KR CWindFont's file name).
static char s_windFontName[] = "[font_wind.jpg";
static char* s_windFontFile = s_windFontName;
// 0xd1c284: its character set.
static char s_windFontChars[] = "1234567890ym%-.?/:, ";

// 0x6f5300
CLobbyWindFont::CLobbyWindFont(void) {
	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = &s_windFontFile;
	info.numPages = 1;
	info.fontw = 16;
	info.fonth = 16;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = s_windFontChars;

	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

// 0xe16a60: the texture entry (0xd1c2ac).
static char s_degreeFontName[] = "gp_font_degree.tga";
static char* s_degreeFontFile = s_degreeFontName;
// 0xd1c040: its character set (shared with the GP award's rank font).
static char s_degreeFontChars[] = "1234567890";

// 0x6f53c0
CGPDegreeFont::CGPDegreeFont(void) {
	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = &s_degreeFontFile;
	info.numPages = 1;
	info.fontw = 10;
	info.fonth = 12;
	info.texw = 100;
	info.texh = 12;
	info.pCharSet = s_degreeFontChars;

	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

// JP CLobbyMain +0x784, +0x78c, +0x794, +0x79c.
static RF_SHARED_PTR<WTitleFontEx> s_lobbyFonts[JP_LOBBY_FONT_COUNT];

// 0xab9807-0xab9a42 (in CLobbyMain::OnPreLoadInit 0xab9720)
void JpLobbyTitleFonts_Create(void) {
	s_lobbyFonts[JP_LOBBY_FONT_TIMECOUNT] = RF_SHARED_PTR<WTitleFontEx>(new CGPTimeCountFont);
	s_lobbyFonts[JP_LOBBY_FONT_TIMECOUNT_BIG] = RF_SHARED_PTR<WTitleFontEx>(new CGPTimeCountBigFont);
	s_lobbyFonts[JP_LOBBY_FONT_WIND] = RF_SHARED_PTR<WTitleFontEx>(new CLobbyWindFont);
	s_lobbyFonts[JP_LOBBY_FONT_DEGREE] = RF_SHARED_PTR<WTitleFontEx>(new CGPDegreeFont);
}

// 0xaba6f3-0xaba7cf (in ~CLobbyMain 0xaba620): the members' release, last
// slot first.
void JpLobbyTitleFonts_Release(void) {
	for (int slot = JP_LOBBY_FONT_COUNT - 1; slot >= 0; --slot)
		s_lobbyFonts[slot].reset();
}

WTitleFontEx* JpLobbyTitleFonts_Get(int slot) {
	if (slot < 0 || slot >= JP_LOBBY_FONT_COUNT)
		return NULL;
	return s_lobbyFonts[slot].get();
}

// 0x55cef0 (thiscall on the GP clock, name ours): the clock's three images,
// each fetched once.
bool JpGrandPrixClock_LoadImages(CGrandPrixClock* clock) {
	if (clock->m_pUnsetImage == NULL)
		clock->m_pUnsetImage = g_pFresh->GetBitmap("gp_time_am");
	if (clock->m_pSetImage == NULL)
		clock->m_pSetImage = g_pFresh->GetBitmap("gp_time_pm");
	if (clock->m_pBlinkImage == NULL)
		clock->m_pBlinkImage = g_pFresh->GetBitmap("gp_timecount_w");
	return true;
}

// 0xab9a44-0xab9a60 (in CLobbyMain::OnPreLoadInit 0xab9720). The JP clock
// (0xe74e1c) exists by then and is used without a test; the port does not
// create it yet (no caller of its constructor 0x55d1b0 is traced), hence
// the NULL test.
void JpLobbyTitleFonts_SetupGrandPrixClock(void) {
	CGrandPrixClock* clock = WSingleton<CGrandPrixClock>::Instance();
	if (clock == NULL)
		return;
	clock->m_pFont = s_lobbyFonts[JP_LOBBY_FONT_TIMECOUNT_BIG].get();
	JpGrandPrixClock_LoadImages(clock);
}

// G7 placeholder: JP 0x657690 clears the JP CSharedDoc bytes +0x1069a and
// +0x10b26 and, when the JP scene manager (0xe741c0) exists, runs its
// 0x9af820(0) (0x9af7c0(1, 1), then releases its +0xb0 and +0xb4
// objects); none of them has a port counterpart.
void JpTaskMain_ResetLobbySceneState(CTaskMain*) {
}
