// Original: d:\build\custom\temp\code\client\projectg\jukeboxdlg.cpp
#include <stdlib.h>
#include <vector>
#include "JukeBoxDlg.h"
#include "ObjectFactory.h"
#include "fredit.h"
#include "frbutton.h"
#include "SoundManager.h"
#include "commonutil.h"
#include "TaskManager.h"
#include "InputManager.h"
#include "ClientSetting.h"
#include "wutil.h"
#ifdef JP_PAKS
#include <list>
#include <string>
#include "fresh.h"
#include "frlistbox.h"
#include "frscrollbar.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "shared/luasystem/LuaScript.h"
#include "shared/luasystem/LuaScriptManager.h"
#include "shared/luasystem/luatablehelper.h"
#endif

// jukeboxdlg.cpp:20 (run-time type and factory registration of FrJukeBoxDlg)
const WRTTI FrJukeBoxDlg::m_RTTI("FrJukeBoxDlg", &FrForm::m_RTTI);

IObject* FrJukeBoxDlgMakeInstance(void) {
	return new FrJukeBoxDlg;
}

struct __sFrJukeBoxDlg {
	__sFrJukeBoxDlg(void) {
		ObjectFactory().AddObjectFunctor(FrJukeBoxDlgMakeInstance, "FrJukeBoxDlg");
	}
};
static __sFrJukeBoxDlg s_registerFrJukeBoxDlg;

// jukeboxdlg.cpp:22
const sFRESH_MSGMAP* FrJukeBoxDlg::GetMessageMap(void) const {
	return &FrJukeBoxDlg::_MsgMap;
}

sFRESH_MSGMAP FrJukeBoxDlg::_MsgMap = { &FrForm::_MsgMap, &FrJukeBoxDlg::_MsgEntries[0] };

bool FrJukeBoxDlg::m_preserve_isPlaying = false;

sFRESH_ENTRY FrJukeBoxDlg::_MsgEntries[] = {
	// jukeboxdlg.cpp:24
	{ "jukeboxtitle", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrJukeBoxDlg::OnTitleInit },
	// jukeboxdlg.cpp:25
	{ "jukeboxplay", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrJukeBoxDlg::OnPlayInit },
	// jukeboxdlg.cpp:26
	{ "jukeboxclose", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrJukeBoxDlg::OnCloseBtnUp },
	// jukeboxdlg.cpp:27
	{ "jukeboxplay", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrJukeBoxDlg::OnPlayBtnUp },
	// jukeboxdlg.cpp:28
	{ "jukeboxprev", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrJukeBoxDlg::OnPrevBtnUp },
	// jukeboxdlg.cpp:29
	{ "jukeboxnext", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrJukeBoxDlg::OnNextBtnUp },
	// jukeboxdlg.cpp:30
	{ "jukeboxstop", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrJukeBoxDlg::OnStopBtnUp },
#ifdef JP_PAKS
	// JP rows 0xe1a260 / 0xe1a280 / 0xe1a2a0: the track list.
	{ "jukeboxlist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrJukeBoxDlg::OnListInit },
	{ "jukeboxlist", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrJukeBoxDlg::OnListOwnerDraw },
	{ "jukeboxlist", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrJukeBoxDlg::OnListBtnUp },
#endif

	// jukeboxdlg.cpp:32
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// One jukebox track: the BGM file and the title the dialog shows.
struct sJukeBoxBGM {
	const char* file;    // 0x0
	const char* title;   // 0x4
};

#ifdef JP_PAKS
// JP 0xe7b0e0 (a std::vector of sJukeBoxBGM, 8-byte rows) and 0xe7b0cc: the
// track list and its max_count, read from BGMlist.lua by LoadBGMList. The
// port keeps the script's strings (the script is released after the read).
// Port guard: the KR code indexes the list with m_curBGM from its INIT
// handlers on, before OnInit read the script (the lobby's jukebox opens
// without the lounge's Init); a row outside the list reads as an empty
// track instead of past the vector.
struct sJukeBoxBGMList {
	std::vector<sJukeBoxBGM> rows;
	bool empty(void) const { return rows.empty(); }
	size_t size(void) const { return rows.size(); }
	void push_back(const sJukeBoxBGM& bgm) { rows.push_back(bgm); }
	const sJukeBoxBGM& operator[](int i) const {
		static const sJukeBoxBGM none = { "", "" };
		return i >= 0 && i < (int)rows.size() ? rows[i] : none;
	}
};
static sJukeBoxBGMList s_jukeBoxBGM;
static int s_jukeBoxBGMCount = 0;
static std::list<std::string> s_jukeBoxBGMStrings;

// JP 0xe73e3c: the BGM sequencer's random-play flag (the "(Random)" row).
static bool s_jpRandomPlay = false;
// JP FrJukeBoxDlg +0x110: the "jukeboxlist" list box.
static FrListBox* s_jpJukeBoxList = NULL;
// The "(Random)" row's data (the JP adds the row index as the data).
static int s_jpRandomRow = 0;

static const char* KeepBGMString(const char* text) {
	s_jukeBoxBGMStrings.push_back(std::string(text ? text : ""));
	return s_jukeBoxBGMStrings.back().c_str();
}

// JP 0x7bbab0: once, a script runs "BGMlist.lua": max_count_bgm_list.max_count
// tracks, bgm_list holding file / title pairs (JP 0xc1ff80 the manager,
// 0xc1feb0 CreateScript, 0xc20530 RunFile, 0xc20240 GetTable, 0x470e60 the
// int read, 0x4d5ba0 a string entry, 0xc1fd50 ReleaseScript).
static void LoadBGMList(void) {
	if (!s_jukeBoxBGM.empty())
		return;
	lua_system::lua_script* script = lua_system::lua_script_manger::GetInstance()->CreateScript(false);
	if (!script)
		return;
	script->RunFile("BGMlist.lua");
	{
		lua_system::lua_table_helper table = script->GetTable("max_count_bgm_list");
		s_jukeBoxBGMCount = table.GetValueByName<int>("max_count");
	}
	{
		lua_system::lua_table_helper table = script->GetTable("bgm_list");
		for (int i = 2; i - 2 < s_jukeBoxBGMCount * 2; i += 2) {
			sJukeBoxBGM bgm;
			bgm.file = KeepBGMString(table.GetValueByIndex<const char*>(i - 1));
			bgm.title = KeepBGMString(table.GetValueByIndex<const char*>(i));
			s_jukeBoxBGM.push_back(bgm);
		}
	}
	lua_system::lua_script_manger::GetInstance()->ReleaseScript(script);
	// Port guard: the KR statics index the list; never past what was read.
	s_jukeBoxBGMCount = (int)s_jukeBoxBGM.size();
}

// The JP's rand() % max_count, 0 for an empty list (no division by zero).
static int RandomBGM(void) {
	return s_jukeBoxBGMCount > 0 ? rand() % s_jukeBoxBGMCount : 0;
}
#else
// The jukebox's track list (0x9f5cc0).
static const sJukeBoxBGM s_jukeBoxBGM[] = {
	{ "shiny.mp3", "shiny day" },
	{ "bunny.mp3", "bunny picnic" },
	{ "navy_blue.mp3", "navy blue memory" },
	{ "rising_sun.mp3", "rising sun" },
	{ "happy flight.mp3", "happy flight" },
	{ "crystal waver.mp3", "crystal waver" },
	{ "snowscape.mp3", "snowscape" },
	{ "winter_ride.mp3", "winter ride" },
	{ "somewhere.mp3", "somewhere" },
	{ "nowhere.mp3", "nowhere" },
	{ "volcano.mp3", "dive into volcano" },
	{ "vermilion.mp3", "vermilion sunset" },
	{ "samba3.mp3", "breeze" },
	{ "daydream.mp3", "daydream" },
	{ "frog.mp3", "frog" },
	{ "spring.mp3", "spring" },
	{ "data/Sound/season2/tea_time_piano.mp3", "tea time" },
	{ "crystal lake.mp3", "crystal lake" },
	{ "fade into white.mp3", "fade into white" },
	{ "the mystery of the lost seaway.mp3", "the mystery of the lost seaway" },
	{ "voyage the sky.mp3", "voyage the sky" },
	{ "zero fill love.mp3", "zero fill love" },
	{ "eastern_valley.mp3", "eastern valley" },
	{ "river.mp3", "river" }
};

enum {
	JUKEBOX_BGM_COUNT = 24
};
#endif

int FrJukeBoxDlg::m_curBGM;

// jukeboxdlg.cpp:70
void FrJukeBoxDlg::Init(void) {
#ifdef JP_PAKS
	// The JP reads BGMlist.lua on first use (OnInit 0x7bc1e0, PlayWithMap
	// 0x7bbce0); the KR statics index it from here on.
	LoadBGMList();
	m_curBGM = RandomBGM();
#else
	m_curBGM = rand() % JUKEBOX_BGM_COUNT;
#endif
}

// jukeboxdlg.cpp:76
void FrJukeBoxDlg::Preserve(void) {
#ifdef JP_PAKS
	LoadBGMList();
#endif
	m_preserve_isPlaying = g_audio->IsPlaying(s_jukeBoxBGM[m_curBGM].file);
	g_audio->StopBGM(true);
}

// jukeboxdlg.cpp:82
void FrJukeBoxDlg::Restore(void) {
#ifdef JP_PAKS
	LoadBGMList();
#endif
	g_audio->StopBGM(true);
	if (m_preserve_isPlaying) {
		g_audio->PlayBGM(s_jukeBoxBGM[m_curBGM].file, false, true);
	}
}

// jukeboxdlg.cpp:89
void FrJukeBoxDlg::PlayWithMap(unsigned char map) {
	std::vector<int> bgms;

#ifdef JP_PAKS
	// JP 0x7bbce0 starts with the list read 0x7bbab0.
	LoadBGMList();
#endif
	switch (map) {
	case 4:
	case 5:
		bgms.push_back(0);
		bgms.push_back(1);
		break;
	case 7:
		bgms.push_back(2);
		bgms.push_back(3);
		break;
	case 8:
		bgms.push_back(4);
		bgms.push_back(5);
		break;
	case 9:
		bgms.push_back(6);
		bgms.push_back(7);
		break;
	case 10:
		bgms.push_back(8);
		bgms.push_back(9);
		break;
	case 13:
		bgms.push_back(10);
		bgms.push_back(11);
		break;
	case 14:
		bgms.push_back(17);
		bgms.push_back(18);
		break;
	case 15:
		bgms.push_back(19);
		bgms.push_back(20);
		break;
	case 16:
		bgms.push_back(22);
		bgms.push_back(23);
		break;
	default:
		bgms.push_back(12);
		bgms.push_back(13);
		bgms.push_back(14);
		bgms.push_back(15);
		break;
	}

	if (bgms.size() != 0) {
		g_audio->StopBGM(true);
		m_curBGM = bgms[rand() % bgms.size()];
		g_audio->PlayBGM(s_jukeBoxBGM[m_curBGM].file, false, true);
	}
}

// jukeboxdlg.cpp:151
FrJukeBoxDlg::FrJukeBoxDlg(void)
	: m_pTitle(NULL)
	, m_pPlay(NULL) {
}

// jukeboxdlg.cpp:157
FrJukeBoxDlg::~FrJukeBoxDlg(void) {
#ifdef JP_PAKS
	// The port keeps the list box in a static (JP +0x110): not past the dialog.
	s_jpJukeBoxList = NULL;
#endif
}

// jukeboxdlg.cpp:162
bool FrJukeBoxDlg::OnInit(void) {
#ifdef JP_PAKS
	// JP 0x7bc1e0: the track list read, a random current track, one row per
	// track plus the "(Random)" row; the playing track becomes the current
	// one and is scrolled into the 10 visible rows. Not in the port: the
	// "jukebox_light.tga" overlay (+0x154), the row bitmaps 0x7ba4d0 / the
	// light's rect 0x7bad60 (drawn by the row highlights 0x7badd0 /
	// 0x7baed0) and the +0x150 flag from the sound manager's +0x75.
	LoadBGMList();
	m_curBGM = RandomBGM();
	if (s_jpJukeBoxList) {
		for (int i = 0; i <= s_jukeBoxBGMCount; ++i) {
			s_jpJukeBoxList->AddItem(i == s_jukeBoxBGMCount ? (void*)&s_jpRandomRow : (void*)&s_jukeBoxBGM.rows[i]);
			if (i != s_jukeBoxBGMCount && g_audio->IsPlaying(s_jukeBoxBGM[i].file)) {
				m_curBGM = i;
			}
		}
		if (m_curBGM > 9 && s_jpJukeBoxList->GetScrollBar()) {
			s_jpJukeBoxList->GetScrollBar()->SetCurTopRow(m_curBGM - 9);
		}
	}
	OnChangePlayPauseButton();
	EnableDrag(false);
	return FrForm::OnInit();
#else
	FrForm::OnInit();

	EnableDrag(false);
	return true;
#endif
}

// jukeboxdlg.cpp:170
void FrJukeBoxDlg::OnCloseBtnUp(void) {
	Close(true);
}

// jukeboxdlg.cpp:175
void FrJukeBoxDlg::OnTitleInit(intptr_t param) {
#ifdef JP_PAKS
	// The INIT handlers run before OnInit: the list first (port).
	LoadBGMList();
#endif
	m_pTitle = DYNAMIC_CAST(FrEdit, param);
	if (m_pTitle) m_pTitle->SetLine(1, s_jukeBoxBGM[m_curBGM].title, 0xFFFFFFFF, false, 0);
}

// jukeboxdlg.cpp:181
void FrJukeBoxDlg::OnPlayInit(intptr_t param) {
#ifdef JP_PAKS
	LoadBGMList();
#endif
	m_pPlay = DYNAMIC_CAST(FrButton, param);
	OnChangePlayPauseButton();
}

// jukeboxdlg.cpp:187
void FrJukeBoxDlg::OnPlayBtnUp(void) {
	bool isPlaying = g_audio->IsPlaying(s_jukeBoxBGM[m_curBGM].file);

	if (isPlaying) {
		g_audio->PauseBGM(false);
	} else {
		g_audio->PlayBGM(s_jukeBoxBGM[m_curBGM].file, false, true);
	}

	OnChangePlayPauseButton();

	AfxGetTask()->PassMsgToActor("AvatarChat", MsgObject(0, 0x69, !isPlaying, 0, 0, 0, 0));
}

// jukeboxdlg.cpp:202
void FrJukeBoxDlg::OnStopBtnUp(void) {
	g_audio->StopBGM(true);

	OnChangePlayPauseButton();

	AfxGetTask()->PassMsgToActor("AvatarChat", MsgObject(0, 0x69, 0, 0, 0, 0, 0));
}

// jukeboxdlg.cpp:211
void FrJukeBoxDlg::OnPrevBtnUp(void) {
	bool isPlaying = g_audio->IsPlaying(s_jukeBoxBGM[m_curBGM].file);

#ifdef JP_PAKS
	// JP 0x7baae0: the prev / next buttons leave random play.
	s_jpRandomPlay = false;
#endif
	m_curBGM--;
	if (m_curBGM < 0) {
#ifdef JP_PAKS
		m_curBGM = s_jukeBoxBGMCount - 1;
#else
		m_curBGM = JUKEBOX_BGM_COUNT - 1;
#endif
	}
	if (m_pTitle) {
		m_pTitle->SetLine(1, s_jukeBoxBGM[m_curBGM].title, 0xFFFFFFFF, false, 0);
	}
	if (isPlaying) {
		g_audio->StopBGM(true);
		g_audio->PlayBGM(s_jukeBoxBGM[m_curBGM].file, false, true);
	}
}

// jukeboxdlg.cpp:230
void FrJukeBoxDlg::OnNextBtnUp(void) {
	bool isPlaying = g_audio->IsPlaying(s_jukeBoxBGM[m_curBGM].file);

#ifdef JP_PAKS
	// JP 0x7bab90
	s_jpRandomPlay = false;
#endif
	m_curBGM++;
#ifdef JP_PAKS
	if (m_curBGM > s_jukeBoxBGMCount - 1) {
#else
	if (m_curBGM > sizeof(s_jukeBoxBGM) / sizeof(s_jukeBoxBGM[0]) - 1) {
#endif
		m_curBGM = 0;
	}
	if (m_pTitle) {
		m_pTitle->SetLine(1, s_jukeBoxBGM[m_curBGM].title, 0xFFFFFFFF, false, 0);
	}
	if (isPlaying) {
		g_audio->StopBGM(true);
		g_audio->PlayBGM(s_jukeBoxBGM[m_curBGM].file, false, true);
	}
}

// jukeboxdlg.cpp:249
void FrJukeBoxDlg::OnChangeTitle(void) {
	if (m_pTitle) {
		m_pTitle->SetLine(1, s_jukeBoxBGM[m_curBGM].title, 0xFFFFFFFF, false, 0);
	}
}

// jukeboxdlg.cpp:257
void FrJukeBoxDlg::OnChangePlayPauseButton(void) {
	bool isPlaying = g_audio->IsPlaying(s_jukeBoxBGM[m_curBGM].file);

	if (m_pPlay) {
		const char* name;

		if (isPlaying) {
			name = "btn_pause";
		} else {
			name = "btn_play";
		}
		m_pPlay->SetButtonImg(MakeStr("%s_n", name), FrButton::NORMAL);
		m_pPlay->SetButtonImg(MakeStr("%s_o", name), FrButton::OVER);
		m_pPlay->SetButtonImg(MakeStr("%s_o", name), FrButton::PRESSED);
	}
}

// jukeboxdlg.cpp:278
// Keyboard control: space toggles play/pause, left/right change the track,
// up/down raise or lower the BGM volume (key names in CP949: "spacebar",
// "left", "right", "up", "down").
void FrJukeBoxDlg::OnProc(const float dt) {
	if (g_input->GetDown("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9", true)) {
		OnPlayBtnUp();
		g_audio->PlaySfx("ui_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
	}

	if (g_input->GetDown("\xc1\xc2", true)) {
		OnPrevBtnUp();
	}

	if (g_input->GetDown("\xbf\xec", true)) {
		OnNextBtnUp();
	}

	if (g_input->Get("\xbb\xf3", true)) {
		COption::Instance()->aSetBgmVolume(Between(0.0f, COption::Instance()->aGetBgmVolume() + dt * 0.5f, 1.0f));
		g_audio->SetBGMVolume(COption::Instance()->aGetBgmVolume());
	}

	if (g_input->Get("\xc7\xcf", true)) {
		COption::Instance()->aSetBgmVolume(Between(0.0f, COption::Instance()->aGetBgmVolume() - dt * 0.5f, 1.0f));
		g_audio->SetBGMVolume(COption::Instance()->aGetBgmVolume());
	}
}

#ifdef JP_PAKS
// JP 0x7bacf0: "jukeboxlist"'s INIT keeps the list box (JP +0x110).
void FrJukeBoxDlg::OnListInit(intptr_t param) {
	s_jpJukeBoxList = DYNAMIC_CAST(FrListBox, param);
}

// JP 0x7bb290: one row: the track's title, or "(Random)" for the last row
// (red while random play is on), at +25 / +8 in a 300-wide box. Not in the
// port: the playing / selected row highlights 0x7badd0 / 0x7baed0.
void FrJukeBoxDlg::OnListOwnerDraw(intptr_t param) {
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem)
		return;
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	unsigned long color = pGDI->GetTextColor();
	pGDI->SetTextStyle(1);
	WPoint pt(pItem->pos.x + 25.0f, pItem->pos.y + 8.0f);
	if (pItem->idx == s_jukeBoxBGMCount) {
		if (s_jpRandomPlay) {
			pGDI->SetTextColor(0xff0000, 0xffffffff);
		}
		g_pFresh->GetManager()->PrintText(pt, 0, "(Random)", 300.0f, 0xff00ffff);
	} else if (pItem->idx >= 0 && pItem->idx < s_jukeBoxBGMCount) {
		g_pFresh->GetManager()->PrintText(pt, 0, s_jukeBoxBGM[pItem->idx].title, 300.0f, 0xff00ffff);
	}
	pGDI->SetTextColor(color, 0xffffffff);
	pGDI->SetTextStyle(0);
}

// JP 0x7bb6a0: a click on a row. A track row becomes the current track and
// toggles: playing -> paused, else played (looped) through the BGM sequencer
// 0xe73e24 (0x483fc0: one-track playlist), then the play / pause images and
// the "AvatarChat" message (JP 0x76 = KR 0x69, as OnPlayBtnUp). The
// "(Random)" row toggles random play: on, the JP shuffles every track into
// the sequencer (0x7bb400) and plays it; off, the current track alone.
// The sequencer (which moves on to the next shuffled track) is not in the
// port: random play starts one random track, looped.
void FrJukeBoxDlg::OnListBtnUp(void) {
	if (!s_jpJukeBoxList)
		return;
	FrListItem* pItem = s_jpJukeBoxList->GetSelected();
	if (!pItem)
		return;
	if (pItem->idx == s_jukeBoxBGMCount) {
		if (s_jpRandomPlay) {
			s_jpRandomPlay = false;
			g_audio->StopBGM(true);
			g_audio->PlayBGM(s_jukeBoxBGM[m_curBGM].file, false, true);
			return;
		}
		if (s_jukeBoxBGMCount <= 0)
			return;
		m_curBGM = RandomBGM();
		g_audio->StopBGM(true);
		g_audio->PlayBGM(s_jukeBoxBGM[m_curBGM].file, false, true);
		s_jpRandomPlay = true;
		OnChangePlayPauseButton();
		return;
	}
	if (pItem->idx < 0 || pItem->idx > s_jukeBoxBGMCount)
		return;
	m_curBGM = pItem->idx;
	s_jpRandomPlay = false;
	bool isPlaying = g_audio->IsPlaying(s_jukeBoxBGM[m_curBGM].file);
	if (!isPlaying) {
		g_audio->StopBGM(true);
		g_audio->PlayBGM(s_jukeBoxBGM[m_curBGM].file, false, true);
	} else {
		g_audio->PauseBGM(false);
	}
	OnChangePlayPauseButton();
	AfxGetTask()->PassMsgToActor("AvatarChat", MsgObject(0, 0x69, !isPlaying, 0, 0, 0, 0));
}
#endif
