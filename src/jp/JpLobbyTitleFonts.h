// JP-only game code (no KR counterpart): the four number fonts the JP
// CLobbyMain holds (JP +0x784, +0x78c, +0x794, +0x79c, each a VS 2010
// std::tr1::shared_ptr<WTitleFontEx>: pointer, then its reference block at
// +4), created by CLobbyMain::OnPreLoadInit (JP 0xab9807-0xab9a42) and
// handed to the room list forms (FrRoomListDlg::SetLobbyValues 0x51ce50,
// FrGameRoomDlg::SetLobbyValues 0x4ff300). The classes are WTitleFontEx
// subclasses of 0xc bytes (TitleFontEx.h) that only describe their texture;
// their constructors sit in the JP TitleFontEx unit (0x6f5180..0x6f5440).
// Names ours, from their textures.
//
// LobbyMain.cpp does not include this header (refresh_brief L38): it
// declares JpLobbyTitleFonts_Create / JpLobbyTitleFonts_Get under JP_PAKS.
#ifndef PG_JP_JPLOBBYTITLEFONTS_H
#define PG_JP_JPLOBBYTITLEFONTS_H

#include "TitleFontEx.h"

// JP +0x784: "gp_timecount_num.tga", 22x28 digits (vtable 0xd1c244).
class CGPTimeCountFont : public WTitleFontEx {
public:
	CGPTimeCountFont(void);
};
CHECK_SIZE(CGPTimeCountFont, 0xc);

// JP +0x78c: "gp_timecount_num_big.tga", 28x36 digits (vtable 0xd1c264).
class CGPTimeCountBigFont : public WTitleFontEx {
public:
	CGPTimeCountBigFont(void);
};
CHECK_SIZE(CGPTimeCountBigFont, 0xc);

// JP +0x794: "[font_wind.jpg", 16x16 (vtable 0xd1c2a0); the KR CWindFont's
// texture with a space where KR has 'x'. The GP room object 0x55ca90 also
// makes one.
class CLobbyWindFont : public WTitleFontEx {
public:
	CLobbyWindFont(void);
};
CHECK_SIZE(CLobbyWindFont, 0xc);

// JP +0x79c: "gp_font_degree.tga", 10x12 digits (vtable 0xd1c2a8).
class CGPDegreeFont : public WTitleFontEx {
public:
	CGPDegreeFont(void);
};
CHECK_SIZE(CGPDegreeFont, 0xc);

// The CLobbyMain slot of each font.
enum eJpLobbyTitleFont {
	JP_LOBBY_FONT_TIMECOUNT = 0,      // JP +0x784
	JP_LOBBY_FONT_TIMECOUNT_BIG = 1,  // JP +0x78c
	JP_LOBBY_FONT_WIND = 2,           // JP +0x794
	JP_LOBBY_FONT_DEGREE = 3,         // JP +0x79c
	JP_LOBBY_FONT_COUNT = 4
};

// JP 0xab9807-0xab9a42 (inside CLobbyMain::OnPreLoadInit 0xab9720): a new
// font of each slot replaces the previous one (released when its last
// reference goes).
void JpLobbyTitleFonts_Create(void);
// JP 0xaba6f3-0xaba7cf (in ~CLobbyMain 0xaba620): the four members
// released, +0x79c first.
void JpLobbyTitleFonts_Release(void);
// The font of a slot (eJpLobbyTitleFont), NULL before the first Create.
WTitleFontEx* JpLobbyTitleFonts_Get(int slot);

class CGrandPrixClock;
class CTaskMain;

// 0x55cef0 (a CGrandPrixClock member in the original, name ours): its
// "gp_time_am" / "gp_time_pm" / "gp_timecount_w" images (+0x74, +0x78,
// +0x7c), each fetched once.
bool JpGrandPrixClock_LoadImages(CGrandPrixClock* clock);
// JP 0xab9a44-0xab9a60 (in CLobbyMain::OnPreLoadInit): the GP clock
// (0xe74e1c) prints with font 1 (+0x78c) and loads its images.
void JpLobbyTitleFonts_SetupGrandPrixClock(void);
// 0x657690 (a JP CTaskMain member, name ours; called by OnPreLoadInit):
// resets JP lobby scene state with no port counterpart.
void JpTaskMain_ResetLobbySceneState(CTaskMain* task);

#endif // PG_JP_JPLOBBYTITLEFONTS_H
