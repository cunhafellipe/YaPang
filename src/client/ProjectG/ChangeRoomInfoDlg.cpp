// Original: d:\build\custom\temp\code\client\projectg\changeroominfodlg.cpp
#include "ChangeRoomInfoDlg.h"
#include "LobbyMain.h"
#include "TreasureDlg.h"
#include "ObjectFactory.h"
#include "frarea.h"
#include "frbutton.h"
#include "fredit.h"
#include "frviewer.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "SoundManager.h"
#include "frgraphicinterface.h"
#include "bitmap.h"
#include "SharedDoc.h"
#include "InputManager.h"
#include "commonutil.h"
#include "TaskManager.h"
#include "Packet.h"

#include <algorithm>
#include <string.h>

// Game type names drawn on the game type buttons, indexed by game type
// (0xab3290, no symbol; the same strings as FrMakeRoomDlg's table).
static const char* s_gameTypeName[] = {
	"\xbd\xba\xc6\xae\xb7\xce\xc5\xa9",
	"\xb8\xc5\xc4\xa1",
	"\xc3\xa4\xc6\xc3\xb9\xe6",
	"\xb7\xa1\xb4\xf5",
	"\xb4\xeb\xc8\xb8",
	"\xb4\xeb\xc8\xb8\xc6\xc0\xc0\xfc",
	"\xb1\xe6\xb5\xe5\xb4\xeb\xc0\xfc",
	"\xc6\xce\xb9\xe8\xc6\xb2",
	"",
	"",
	"\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1",
	"",
	"",
	"\xbc\xc5\xc7\xc3\xc4\xda\xbd\xba"
};

namespace {

// Functor of std::for_each over a password: flags a space.
struct _invalidPwd {
	_invalidPwd(void);
	void operator()(char c);

	bool m_bInvalid;   // 0x0
};

// changeroominfodlg.cpp:31
_invalidPwd::_invalidPwd(void) : m_bInvalid(false) {}

// changeroominfodlg.cpp:32
void _invalidPwd::operator()(char c) {
	if (c == ' ')
		m_bInvalid = true;
}

} // namespace

// changeroominfodlg.cpp:41 (run-time type and factory registration of FrChangeRoomInfoDlg)
const WRTTI FrChangeRoomInfoDlg::m_RTTI("FrChangeRoomInfoDlg", &FrForm::m_RTTI);

IObject* FrChangeRoomInfoDlgMakeInstance(void) {
	return new FrChangeRoomInfoDlg;
}

struct __sFrChangeRoomInfoDlg {
	__sFrChangeRoomInfoDlg(void) {
		ObjectFactory().AddObjectFunctor(FrChangeRoomInfoDlgMakeInstance, "FrChangeRoomInfoDlg");
	}
};
static __sFrChangeRoomInfoDlg s_registerFrChangeRoomInfoDlg;

// changeroominfodlg.cpp:43
const sFRESH_MSGMAP* FrChangeRoomInfoDlg::GetMessageMap(void) const {
	return &FrChangeRoomInfoDlg::_MsgMap;
}

sFRESH_MSGMAP FrChangeRoomInfoDlg::_MsgMap = { &FrForm::_MsgMap, &FrChangeRoomInfoDlg::_MsgEntries[0] };

sFRESH_ENTRY FrChangeRoomInfoDlg::_MsgEntries[] = {
	// changeroominfodlg.cpp:46
	{ "text", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeRoomInfoDlg::OnTextInit },

	// changeroominfodlg.cpp:49
	{ "title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeRoomInfoDlg::OnTitleInit },
	// changeroominfodlg.cpp:50
	{ "title", FRCMD_ENTERKEY, FrFuncVV, (FRESH_PMSG)&FrChangeRoomInfoDlg::OnOkBtnUp },
	// changeroominfodlg.cpp:51
	{ "password", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeRoomInfoDlg::OnPasswordInit },
	// changeroominfodlg.cpp:52
	{ "password", FRCMD_ENTERKEY, FrFuncVV, (FRESH_PMSG)&FrChangeRoomInfoDlg::OnOkBtnUp },

	// changeroominfodlg.cpp:56
	{ "mapprev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeRoomInfoDlg::OnMapPrevInit },
	// changeroominfodlg.cpp:57
	{ "mapprev", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrChangeRoomInfoDlg::OnMapPrevUp },
	// changeroominfodlg.cpp:58
	{ "mapnext", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeRoomInfoDlg::OnMapNextInit },
	// changeroominfodlg.cpp:59
	{ "mapnext", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrChangeRoomInfoDlg::OnMapNextUp },

	// changeroominfodlg.cpp:62
	{ "gametype", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeRoomInfoDlg::OnGameTypeInit },
	// changeroominfodlg.cpp:63
	{ "gametype", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeRoomInfoDlg::OnGameTypeOwnerDraw },
	// changeroominfodlg.cpp:64
	{ "userlimit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeRoomInfoDlg::OnUserLimitInit },
	// changeroominfodlg.cpp:65
	{ "userlimit", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeRoomInfoDlg::OnUserLimitOwnerDraw },
	// changeroominfodlg.cpp:66
	{ "hole", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeRoomInfoDlg::OnHoleInit },
	// changeroominfodlg.cpp:67
	{ "hole", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeRoomInfoDlg::OnHoleOwnerDraw },
	// changeroominfodlg.cpp:68
	{ "timelimit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeRoomInfoDlg::OnTimeLimitInit },
	// changeroominfodlg.cpp:69
	{ "timelimit", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeRoomInfoDlg::OnTimeLimitOwnerDraw },
	// changeroominfodlg.cpp:70
	{ "course", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeRoomInfoDlg::OnCourseInit },
	// changeroominfodlg.cpp:71
	{ "course", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrChangeRoomInfoDlg::OnCourseUp },
	// changeroominfodlg.cpp:72
	{ "course", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeRoomInfoDlg::OnCourseOwnerDraw },
	// changeroominfodlg.cpp:73
	{ "holetype", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeRoomInfoDlg::OnHoleTypeInit },
	// changeroominfodlg.cpp:74
	{ "holetype", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeRoomInfoDlg::OnHoleTypeOwnerDraw },

	// changeroominfodlg.cpp:76
	{ "ok", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrChangeRoomInfoDlg::OnOkBtnUp },

	// changeroominfodlg.cpp:78
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// changeroominfodlg.cpp:81
FrChangeRoomInfoDlg::FrChangeRoomInfoDlg(void) {
	m_pTitle = 0;
	m_pPassword = 0;
	m_pText = 0;
	m_pCourseFrame = 0;
	m_pMapPrev = 0;
	m_pMapNext = 0;
	m_pGameType = 0;
	m_pUserLimit = 0;
	m_pHole = 0;
	m_pTimeLimit = 0;
	m_pCourse = 0;
	m_pHoleType = 0;

	m_pSelectButtonImg[0] = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_button_n");
	m_pSelectButtonImg[1] = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_button_o");
	m_pSelectButtonImg[2] = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_button_d");

	m_pHoleTypeImg[0] = g_pFresh->GetManager()->GetBitmap("ITEMS", "hole_front_n");
	m_pHoleTypeImg[1] = g_pFresh->GetManager()->GetBitmap("ITEMS", "hole_front_d");
	m_pHoleTypeImg[2] = g_pFresh->GetManager()->GetBitmap("ITEMS", "hole_back_n");
	m_pHoleTypeImg[3] = g_pFresh->GetManager()->GetBitmap("ITEMS", "hole_back_d");
	m_pHoleTypeImg[4] = g_pFresh->GetManager()->GetBitmap("ITEMS", "hole_random_n");
	m_pHoleTypeImg[5] = g_pFresh->GetManager()->GetBitmap("ITEMS", "hole_random_d");
	m_pHoleTypeImg[6] = g_pFresh->GetManager()->GetBitmap("ITEMS", "hole_shuffle_n");
	m_pHoleTypeImg[7] = g_pFresh->GetManager()->GetBitmap("ITEMS", "hole_shuffle_d");

	m_pMapSelectImg = g_pFresh->GetManager()->GetBitmap("MAKEROOM", "map_select_o");

	m_pMapSelectDlg = 0;

	m_roomInfo = WSingleton<CSharedDoc>::Instance()->m_roomInfo;

	m_hole = m_roomInfo.m_holeCount;
	m_userLimit = m_roomInfo.m_userLimit;
	m_textType = 0;
	m_gameType = m_roomInfo.m_gameType;
	m_gameTime = m_roomInfo.m_value51;
	m_shotTime = m_roomInfo.m_value4D;
	m_course = m_roomInfo.m_course;
	m_holeType = m_roomInfo.m_value4B;
	switch (m_gameType) {
	case 0:
	case 1:
	case 3:
	case 7:
		m_timeLimit = m_shotTime / 1000;
		break;
	case 4:
	case 5:
	case 6:
		m_timeLimit = m_gameTime / 60000;
		break;
	case 9:
	case 10:
		m_timeLimit = m_gameTime / 1000;
		break;
	}

	m_btnDelay = 0.7f;
}

// changeroominfodlg.cpp:149
FrChangeRoomInfoDlg::~FrChangeRoomInfoDlg(void) {
}

// changeroominfodlg.cpp:155
bool FrChangeRoomInfoDlg::OnInit(void) {
	ChangeGameType(m_roomInfo.m_gameType, false, true);
	ChangeCourse(m_roomInfo.m_course);

	if (m_gameType == 10 || m_gameType == 6) {
		if (m_pMapPrev)
			m_pMapPrev->Enable(false);
		if (m_pMapNext)
			m_pMapNext->Enable(false);
	}

	return true;
}

// changeroominfodlg.cpp:169
void FrChangeRoomInfoDlg::OnTextInit(intptr_t param) {
	m_pText = DYNAMIC_CAST(FrArea, param);
}

// changeroominfodlg.cpp:176
void FrChangeRoomInfoDlg::OnTitleInit(intptr_t param) {
	m_pTitle = DYNAMIC_CAST(FrEdit, param);

	if (m_pTitle) {
		m_pTitle->SetCharLimit(20, false);
		m_pTitle->SetLine(1, m_roomInfo.m_title, 0, false, 0);
	}
}

// changeroominfodlg.cpp:187
void FrChangeRoomInfoDlg::OnPasswordInit(intptr_t param) {
	m_pPassword = DYNAMIC_CAST(FrEdit, param);

	if (m_pPassword) {
		m_pPassword->SetCharLimit(7, false);
		m_pPassword->SetLine(1, m_roomInfo.m_password, 0, false, 0);
	}
}

// changeroominfodlg.cpp:206
void FrChangeRoomInfoDlg::OnMapPrevInit(intptr_t param) {
	m_pMapPrev = DYNAMIC_CAST(FrButton, param);

	if (m_pMapPrev) {
		m_pMapPrev->SetPushDelay(0.0f);
		m_pMapPrev->SetVisible(true);
	}
}

// changeroominfodlg.cpp:217
void FrChangeRoomInfoDlg::OnMapPrevUp(void) {
	if (m_pMapPrev) {
		unsigned char course = PrevMap((unsigned char)m_course);
		ChangeCourse(course);
	}
}

// changeroominfodlg.cpp:239
void FrChangeRoomInfoDlg::OnMapNextInit(intptr_t param) {
	m_pMapNext = DYNAMIC_CAST(FrButton, param);

	if (m_pMapNext) {
		m_pMapNext->SetPushDelay(0.0f);
		m_pMapNext->SetVisible(true);
	}
}

// changeroominfodlg.cpp:250
void FrChangeRoomInfoDlg::OnMapNextUp(void) {
	if (m_pMapNext) {
		unsigned char course = NextMap((unsigned char)m_course, m_gameType == 2);
		ChangeCourse(course);
	}
}

// changeroominfodlg.cpp:272
void FrChangeRoomInfoDlg::OnGameTypeInit(intptr_t param) {
	m_pGameType = DYNAMIC_CAST(FrArea, param);
}

// changeroominfodlg.cpp:280
void FrChangeRoomInfoDlg::DrawGameTypeButton(FrGraphicInterface* pDevice, int gameType, WRect rect, bool b) {
	const Bitmap* pImg = m_pSelectButtonImg[0];
	unsigned long color = 0xFFFFFFFF;

	if (b) {
		if (m_gameType == gameType) {
			pImg = m_pSelectButtonImg[2];
		} else {
			const WPoint& mousePos = g_pFresh->GetManager()->GetMousePos();
			if (IsBtnEnable() && rect.IsInRect(mousePos)) {
				if (g_input->GetButton(LEFT_BUTTON) == 2)
					ChangeGameType(gameType, false, false);

				pImg = m_pSelectButtonImg[1];
			}
		}
	} else {
		color = 0xFF909090;
	}

	pDevice->DrawTexture(pImg, rect, color, 0);

	pDevice->PrintText11(WPoint(rect.x + rect.w * 0.5f, rect.y + 5.0f), 1, s_gameTypeName[gameType], 0);
}

// changeroominfodlg.cpp:318
void FrChangeRoomInfoDlg::OnGameTypeOwnerDraw(intptr_t param) {
	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (pDevice == 0)
		return;

	WRect rect = m_pGameType->GetRect();

	switch (m_textType) {
	case 0: {
		WRect rc1(rect.x, rect.y, rect.w * 0.5f, rect.h);
		WRect rc2(rect.x + rect.w * 0.5f, rect.y, rect.w * 0.5f, rect.h);
		DrawGameTypeButton(pDevice, 0, rc1, true);
		DrawGameTypeButton(pDevice, 1, rc2, true);
		break;
	}
	case 1: {
		if (IsLocalContent((localContentType_t)0x2B) && IsAdministrator(0xFFFFFFFF) && WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_userLimit >= 100) {
			WRect rc1(rect.x, rect.y, rect.w, rect.h);
			DrawGameTypeButton(pDevice, 4, rc1, false);
		} else {
			WRect rc1(rect.x, rect.y, rect.w * 0.33333334f, rect.h);
			WRect rc2(rect.x + rect.w * 0.33333334f, rect.y, rect.w * 0.33333334f, rect.h);
			WRect rc3(rect.x + rect.w * 0.6666667f, rect.y, rect.w * 0.33333334f, rect.h);
			bool b = m_gameType == 6;
			DrawGameTypeButton(pDevice, 4, rc1, !b);
			DrawGameTypeButton(pDevice, 5, rc2, !b);
			DrawGameTypeButton(pDevice, 6, rc3, b);
		}
		break;
	}
	case 3:
		DrawGameTypeButton(pDevice, 2, rect, true);
		break;
	case 2: {
		WRect rc1(rect.x, rect.y, rect.w * 0.5f, rect.h);
		WRect rc2(rect.x + rect.w * 0.5f, rect.y, rect.w * 0.5f, rect.h);

		switch (m_gameType) {
		case 7:
			DrawGameTypeButton(pDevice, 7, rc1, true);
			DrawGameTypeButton(pDevice, 10, rc2, false);
			break;
		case 10:
			DrawGameTypeButton(pDevice, 7, rc1, false);
			DrawGameTypeButton(pDevice, 10, rc2, true);
			break;
		}
		break;
	}
	}
}

// changeroominfodlg.cpp:381
void FrChangeRoomInfoDlg::OnUserLimitInit(intptr_t param) {
	m_pUserLimit = DYNAMIC_CAST(FrArea, param);
}

// changeroominfodlg.cpp:389
void FrChangeRoomInfoDlg::DrawUserLimitButton(FrGraphicInterface* pDevice, int userLimit, WRect rect) {
	const Bitmap* pImg = m_pSelectButtonImg[0];

	if (m_userLimit == userLimit) {
		pImg = m_pSelectButtonImg[2];
	} else {
		const WPoint& mousePos = g_pFresh->GetManager()->GetMousePos();
		if (IsBtnEnable() && rect.IsInRect(mousePos)) {
			if (g_input->GetButton(LEFT_BUTTON) == 2)
				m_userLimit = userLimit;

			pImg = m_pSelectButtonImg[1];
		}
	}

	pDevice->DrawTexture(pImg, rect, 0xFFFFFFFF, 0);

	pDevice->PrintText11(WPoint(rect.x + rect.w * 0.5f, rect.y + 5.0f), 1, MakeStr("%d", userLimit), 0);
}

// changeroominfodlg.cpp:415
void FrChangeRoomInfoDlg::OnUserLimitOwnerDraw(intptr_t param) {
	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (pDevice == 0)
		return;

	WRect rect = m_pUserLimit->GetRect();

	switch (m_gameType) {
	case 0: {
		WRect rc1(rect.x, rect.y, rect.w * 0.33333334f, rect.h);
		WRect rc2(rect.x + rect.w * 0.33333334f, rect.y, rect.w * 0.33333334f, rect.h);
		WRect rc3(rect.x + rect.w * 0.6666667f, rect.y, rect.w * 0.33333334f, rect.h);
		DrawUserLimitButton(pDevice, 2, rc1);
		DrawUserLimitButton(pDevice, 3, rc2);
		DrawUserLimitButton(pDevice, 4, rc3);
		break;
	}
	case 1: {
		WRect rc1(rect.x, rect.y, rect.w * 0.5f, rect.h);
		WRect rc2(rect.x + rect.w * 0.5f, rect.y, rect.w * 0.5f, rect.h);
		DrawUserLimitButton(pDevice, 2, rc1);
		DrawUserLimitButton(pDevice, 4, rc2);
		break;
	}
	case 7: {
		WRect rc1(rect.x, rect.y, rect.w * 0.5f, rect.h);
		WRect rc2(rect.x + rect.w * 0.5f, rect.y, rect.w * 0.5f, rect.h);
		DrawUserLimitButton(pDevice, 2, rc1);
		DrawUserLimitButton(pDevice, 4, rc2);
		break;
	}
	case 2:
	case 5:
	case 6: {
		WRect rc1(rect.x, rect.y, rect.w * 0.33333334f, rect.h);
		WRect rc2(rect.x + rect.w * 0.33333334f, rect.y, rect.w * 0.33333334f, rect.h);
		WRect rc3(rect.x + rect.w * 0.6666667f, rect.y, rect.w * 0.33333334f, rect.h);
		DrawUserLimitButton(pDevice, 10, rc1);
		DrawUserLimitButton(pDevice, 20, rc2);
		DrawUserLimitButton(pDevice, 30, rc3);
		break;
	}
	case 10: {
		WRect rc1(rect.x, rect.y, rect.w * 0.33333334f, rect.h);
		WRect rc2(rect.x + rect.w * 0.33333334f, rect.y, rect.w * 0.33333334f, rect.h);
		WRect rc3(rect.x + rect.w * 0.6666667f, rect.y, rect.w * 0.33333334f, rect.h);
		DrawUserLimitButton(pDevice, 6, rc1);
		DrawUserLimitButton(pDevice, 20, rc2);
		DrawUserLimitButton(pDevice, 30, rc3);
		break;
	}
	case 4:
		if (IsLocalContent((localContentType_t)0x2B) && IsAdministrator(0xFFFFFFFF) && WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_userLimit >= 100) {
			WRect rc1(rect.x, rect.y, rect.w, rect.h);
			DrawUserLimitButton(pDevice, WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_userLimit, rc1);
		} else {
			WRect rc1(rect.x, rect.y, rect.w * 0.33333334f, rect.h);
			WRect rc2(rect.x + rect.w * 0.33333334f, rect.y, rect.w * 0.33333334f, rect.h);
			WRect rc3(rect.x + rect.w * 0.6666667f, rect.y, rect.w * 0.33333334f, rect.h);
			DrawUserLimitButton(pDevice, 10, rc1);
			DrawUserLimitButton(pDevice, 20, rc2);
			DrawUserLimitButton(pDevice, 30, rc3);
		}
		break;
	}
}

// changeroominfodlg.cpp:497
void FrChangeRoomInfoDlg::OnHoleInit(intptr_t param) {
	m_hole = m_roomInfo.m_holeCount;
	m_pHole = DYNAMIC_CAST(FrArea, param);
}

// changeroominfodlg.cpp:506
void FrChangeRoomInfoDlg::DrawHoleButton(FrGraphicInterface* pDevice, int hole, WRect rect) {
	const Bitmap* pImg = m_pSelectButtonImg[0];

	if (m_hole == hole) {
		pImg = m_pSelectButtonImg[2];
	} else {
		const WPoint& mousePos = g_pFresh->GetManager()->GetMousePos();
		if (IsBtnEnable() && rect.IsInRect(mousePos)) {
			if (g_input->GetButton(LEFT_BUTTON) == 2)
				ChangeHole(hole);

			pImg = m_pSelectButtonImg[1];
		}
	}

	pDevice->DrawTexture(pImg, rect, 0xFFFFFFFF, 0);

	pDevice->PrintText11(WPoint(rect.x + rect.w * 0.5f, rect.y + 5.0f), 1, MakeStr("%d", hole), 0);
}

// changeroominfodlg.cpp:532
void FrChangeRoomInfoDlg::OnHoleOwnerDraw(intptr_t param) {
	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (pDevice == 0)
		return;

	WRect rect = m_pHole->GetRect();

	switch (m_gameType) {
	case 0: {
		WRect rc1(rect.x, rect.y, rect.w * 0.25f, rect.h);
		WRect rc2(rect.x + rect.w * 0.25f, rect.y, rect.w * 0.25f, rect.h);
		WRect rc3(rect.x + rect.w * 0.5f, rect.y, rect.w * 0.25f, rect.h);
		WRect rc4(rect.x + rect.w * 0.75f, rect.y, rect.w * 0.25f, rect.h);
		DrawHoleButton(pDevice, 3, rc1);
		DrawHoleButton(pDevice, 6, rc2);
		DrawHoleButton(pDevice, 9, rc3);
		DrawHoleButton(pDevice, 18, rc4);
		break;
	}
	case 1:
	case 7: {
		WRect rc1(rect.x, rect.y, rect.w * 0.33333334f, rect.h);
		WRect rc2(rect.x + rect.w * 0.33333334f, rect.y, rect.w * 0.33333334f, rect.h);
		WRect rc3(rect.x + rect.w * 0.6666667f, rect.y, rect.w * 0.33333334f, rect.h);
		DrawHoleButton(pDevice, 6, rc1);
		DrawHoleButton(pDevice, 9, rc2);
		DrawHoleButton(pDevice, 18, rc3);
		break;
	}
	case 4:
	case 5:
	case 6: {
		WRect rc1(rect.x, rect.y, rect.w * 0.33333334f, rect.h);
		WRect rc2(rect.x + rect.w * 0.33333334f, rect.y, rect.w * 0.33333334f, rect.h);
		WRect rc3(rect.x + rect.w * 0.6666667f, rect.y, rect.w * 0.33333334f, rect.h);
		DrawHoleButton(pDevice, 3, rc1);
		DrawHoleButton(pDevice, 6, rc2);
		DrawHoleButton(pDevice, 9, rc3);
		break;
	}
	case 9:
	case 10: {
		WRect rc1(rect.x, rect.y, rect.w * 0.5f, rect.h);
		WRect rc2(rect.x + rect.w * 0.5f, rect.y, rect.w * 0.5f, rect.h);
		DrawHoleButton(pDevice, 9, rc1);
		DrawHoleButton(pDevice, 18, rc2);
		break;
	}
	}
}

// changeroominfodlg.cpp:588
void FrChangeRoomInfoDlg::OnTimeLimitInit(intptr_t param) {
	m_pTimeLimit = DYNAMIC_CAST(FrArea, param);
}

// changeroominfodlg.cpp:596
void FrChangeRoomInfoDlg::DrawTimeLimitButton(FrGraphicInterface* pDevice, int timeLimit, WRect rect) {
	const Bitmap* pImg = m_pSelectButtonImg[0];

	if (m_timeLimit == timeLimit) {
		pImg = m_pSelectButtonImg[2];
	} else {
		const WPoint& mousePos = g_pFresh->GetManager()->GetMousePos();
		if (IsBtnEnable() && rect.IsInRect(mousePos)) {
			if (g_input->GetButton(LEFT_BUTTON) == 2)
				m_timeLimit = timeLimit;

			pImg = m_pSelectButtonImg[1];
		}
	}

	pDevice->DrawTexture(pImg, rect, 0xFFFFFFFF, 0);

	pDevice->PrintText11(WPoint(rect.x + rect.w * 0.5f, rect.y + 5.0f), 1, MakeStr("%d", timeLimit), 0);
}

// changeroominfodlg.cpp:622
void FrChangeRoomInfoDlg::OnTimeLimitOwnerDraw(intptr_t param) {
	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (pDevice == 0)
		return;

	WRect rect = m_pTimeLimit->GetRect();

	switch (m_gameType) {
	case 9:
	case 10:
		DrawTimeLimitButton(pDevice, 40, rect);
		break;
	case 0:
	case 1:
	case 7: {
		WRect rc1(rect.x, rect.y, rect.w * 0.25f, rect.h);
		WRect rc2(rect.x + rect.w * 0.25f, rect.y, rect.w * 0.25f, rect.h);
		WRect rc3(rect.x + rect.w * 0.5f, rect.y, rect.w * 0.25f, rect.h);
		WRect rc4(rect.x + rect.w * 0.75f, rect.y, rect.w * 0.25f, rect.h);
		DrawTimeLimitButton(pDevice, 40, rc1);
		DrawTimeLimitButton(pDevice, 60, rc2);
		DrawTimeLimitButton(pDevice, 90, rc3);
		DrawTimeLimitButton(pDevice, 120, rc4);
		break;
	}
	case 4:
	case 5:
		if (m_hole == 9) {
			WRect rc1(rect.x, rect.y, rect.w * 0.25f, rect.h);
			WRect rc2(rect.x + rect.w * 0.25f, rect.y, rect.w * 0.25f, rect.h);
			WRect rc3(rect.x + rect.w * 0.5f, rect.y, rect.w * 0.25f, rect.h);
			WRect rc4(rect.x + rect.w * 0.75f, rect.y, rect.w * 0.25f, rect.h);
			DrawTimeLimitButton(pDevice, 15, rc1);
			DrawTimeLimitButton(pDevice, 20, rc2);
			DrawTimeLimitButton(pDevice, 25, rc3);
			DrawTimeLimitButton(pDevice, 30, rc4);
		} else if (m_hole == 18) {
			WRect rc1(rect.x, rect.y, rect.w * 0.2f, rect.h);
			WRect rc2(rect.x + rect.w * 0.2f, rect.y, rect.w * 0.2f, rect.h);
			WRect rc3(rect.x + rect.w * 0.4f, rect.y, rect.w * 0.2f, rect.h);
			WRect rc4(rect.x + rect.w * 0.6f, rect.y, rect.w * 0.2f, rect.h);
			WRect rc5(rect.x + rect.w * 0.8f, rect.y, rect.w * 0.2f, rect.h);
			DrawTimeLimitButton(pDevice, 30, rc1);
			DrawTimeLimitButton(pDevice, 35, rc2);
			DrawTimeLimitButton(pDevice, 40, rc3);
			DrawTimeLimitButton(pDevice, 45, rc4);
			DrawTimeLimitButton(pDevice, 50, rc5);
		}
		break;
	case 6: {
		WRect rc1(rect.x, rect.y, rect.w, rect.h);
		break;
	}
	}
}

// changeroominfodlg.cpp:728
void FrChangeRoomInfoDlg::OnCourseInit(intptr_t param) {
	m_pCourse = DYNAMIC_CAST(FrViewer, param);
}

// changeroominfodlg.cpp:733
bool FrChangeRoomInfoDlg::OnMapSelectDlgResult(int result, FrForm* pForm) {
	unsigned char course = 0;
	if (pForm) {

		course = static_cast<FrTreasureCourse*>(pForm)->GetCurMapIdx();
	}

	ChangeCourse(course);
	m_pMapSelectDlg = 0;
	m_btnDelay = 0.7f;
	return true;
}

// changeroominfodlg.cpp:749
void FrChangeRoomInfoDlg::OnCourseUp(void) {
	if (m_gameType == 10 || m_gameType == 6)
		return;

	g_audio->PlaySfx("ui_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);

	m_pMapSelectDlg = CreateForm<FrTreasureCourse>(g_pFresh->GetManager(), this, "treasuremap", 0);
	if (m_pMapSelectDlg) {
		m_pMapSelectDlg->SetSelectMap((unsigned char)m_course);
		m_pMapSelectDlg->SetMapIndexChanged(false);
		m_pMapSelectDlg->EnableDrag(false);
		m_pMapSelectDlg->Open((FRESH_RESULT_CALLBACK)&FrChangeRoomInfoDlg::OnMapSelectDlgResult, 3);
	}
}

// changeroominfodlg.cpp:770
void FrChangeRoomInfoDlg::OnCourseOwnerDraw(PG_MSGARG param) {
	if (m_gameType == 10 || m_gameType == 6)
		return;

	if (m_pCourse == 0)
		return;
	if (m_pMapSelectImg == 0)
		return;

	WRect rect = m_pCourse->GetRect();
	FrWndManager* pManager = g_pFresh->GetManager();
	if (rect.IsInRect(pManager->GetMousePos())) {
		FrGraphicInterface* pDevice = pManager->GetGDI();
		if (pDevice)
			pDevice->DrawTexture(m_pMapSelectImg, WRect(rect.x - 25.0f, rect.y - 15.0f, (float)m_pMapSelectImg->Width(), (float)m_pMapSelectImg->Height()), 0xFFFFFFFF, 0);
	}
}

// changeroominfodlg.cpp:790
void FrChangeRoomInfoDlg::OnHoleTypeInit(intptr_t param) {
	m_pHoleType = DYNAMIC_CAST(FrArea, param);
}

// changeroominfodlg.cpp:798
void FrChangeRoomInfoDlg::DrawHoleTypeButton(FrGraphicInterface* pDevice, eHoleType holeType, WRect rect, bool b) {
	const Bitmap* pImg = m_pHoleTypeImg[holeType * 2];
	unsigned long color = 0xFFFFFFFF;

	if (b) {
		if (m_holeType == holeType) {
			pImg = m_pHoleTypeImg[holeType * 2 + 1];
		} else {
			const WPoint& mousePos = g_pFresh->GetManager()->GetMousePos();
			if (IsBtnEnable() && rect.IsInRect(mousePos)) {
				if (g_input->GetButton(LEFT_BUTTON) == 2)
					m_holeType = holeType;
			}
		}
	} else {
		color = 0xFF909090;
	}

	pDevice->DrawTexture(pImg, rect, color, 0);
}

// changeroominfodlg.cpp:829
void FrChangeRoomInfoDlg::OnHoleTypeOwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (pDevice == 0)
		return;

	WRect rect = m_pHoleType->GetRect();

	float width = (float)m_pHoleTypeImg[0]->Width();
	float height = (float)m_pHoleTypeImg[0]->Height();

	WRect rc1(rect.x, rect.y, width, height);
	WRect rc2(rc1.x + rc1.w, rect.y, (float)m_pHoleTypeImg[2]->Width(), height);
	WRect rc3(rc2.x + rc2.w, rect.y, (float)m_pHoleTypeImg[4]->Width(), height);
	WRect rc4(rc3.x + rc3.w, rect.y, (float)m_pHoleTypeImg[6]->Width(), height);

	if (m_gameType == 10) {
		DrawHoleTypeButton(pDevice, (eHoleType)0, rc1, false);
		DrawHoleTypeButton(pDevice, (eHoleType)1, rc2, false);
		DrawHoleTypeButton(pDevice, (eHoleType)2, rc3, false);
		DrawHoleTypeButton(pDevice, (eHoleType)3, rc4, false);
	} else if (m_hole == 18) {
		DrawHoleTypeButton(pDevice, (eHoleType)0, rc1, true);
		DrawHoleTypeButton(pDevice, (eHoleType)1, rc2, false);
		DrawHoleTypeButton(pDevice, (eHoleType)2, rc3, false);
		DrawHoleTypeButton(pDevice, (eHoleType)3, rc4, true);
	} else {
		DrawHoleTypeButton(pDevice, (eHoleType)0, rc1, true);
		DrawHoleTypeButton(pDevice, (eHoleType)1, rc2, true);
		DrawHoleTypeButton(pDevice, (eHoleType)2, rc3, true);
		DrawHoleTypeButton(pDevice, (eHoleType)3, rc4, true);
	}
}

// changeroominfodlg.cpp:869
void FrChangeRoomInfoDlg::OnOkBtnUp(void) {
	if (m_roomInfo.m_userCount > 1 && !AdminMode()) {
		std::list<sSlotInfo>::iterator it;
		for (it = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.end(); ++it) {
			if (!(it->m_attribute & 0x18) && !(it->m_mannerFlag & 2))
				break;
		}

		if (it == WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.end()) {
			OnFreshOkay();
			return;
		}
	}

	std::string title = m_pTitle->GetLine(1, false);

	if (title.empty() || title.find_first_not_of(' ', 0) == std::string::npos) {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "Please enter a room title"
		pForm->SetMessage("\xb9\xe6 \xc1\xa6\xb8\xf1\xc0\xbb \xb3\xd6\xbe\xee\xc1\xd6\xbc\xbc\xbf\xe4", false);
		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
		return;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_chatManager.Filtering((char*)title.c_str())) {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "The room title contains characters that cannot be used."
		pForm->SetMessage("\xb9\xe6 \xc1\xa6\xb8\xf1\xbf\xa1\xbc\xad \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb9\xae\xc0\xda\xb8\xa6 \xc6\xf7\xc7\xd4\xc7\xcf\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.", false);
		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
		return;
	}

	if (IsMassGame((unsigned char)m_gameType)) {
		m_shotTime = 0;
		if (m_gameType == 10)
			m_gameTime = m_timeLimit * 1000;
		else
			m_gameTime = m_timeLimit * 60000;
	} else {
		m_shotTime = m_timeLimit * 1000;
		m_gameTime = 0;
	}

	unsigned char changeCount = 0;
	if (m_gameType != m_roomInfo.m_gameType) changeCount++;
	if (m_hole != m_roomInfo.m_holeCount) changeCount++;
	if (m_shotTime != m_roomInfo.m_value4D) changeCount++;
	if (m_gameTime != m_roomInfo.m_value51) changeCount++;
	if (m_userLimit != m_roomInfo.m_userLimit) changeCount++;
	if (strcmp(m_roomInfo.m_title, m_pTitle->GetLine(1, false)) != 0) changeCount++;
	if (strcmp(m_roomInfo.m_password, m_pPassword->GetLine(1, false)) != 0) changeCount++;
	if (m_holeType != m_roomInfo.m_value4B) changeCount++;
	if (m_course != m_roomInfo.m_course) changeCount++;

	if (changeCount != 0) {
		if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 2)
			changeCount++;

		WSendPacket packet(GCT_REQUEST_ROOMINFO_CHANGED);
		packet.Encode2(0xFFFF);
		packet.Encode1(changeCount);

		if (strcmp(m_roomInfo.m_title, m_pTitle->GetLine(1, false)) != 0) {
			packet.Encode1(0);
			packet.EncodeStr(std::string(m_pTitle->GetLine(1, false)));
		}

		if (strcmp(m_roomInfo.m_password, m_pPassword->GetLine(1, false)) != 0) {
			std::string password("");
			packet.Encode1(1);
			packet.EncodeStr(std::string(m_pPassword->GetLine(1, false)));
			password = m_pPassword->GetLine(1, false);

			_invalidPwd result = std::for_each(password.begin(), password.end(), _invalidPwd());
			if (result.m_bInvalid) {
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				// "Spaces cannot be used in the password."
				pForm->SetMessage("\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xbf\xa1 \xb0\xf8\xb9\xe9\xc0\xba \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", false);
				pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
				return;
			}

			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 0x27, (intptr_t)m_pPassword->GetLine(1, false), 0, 0, 0, 0);
		}

		if (m_hole != m_roomInfo.m_holeCount) {
			packet.Encode1(4);
			packet.Encode1(m_hole);
		}

		if (m_shotTime != m_roomInfo.m_value4D) {
			packet.Encode1(6);
			packet.Encode1((int)m_shotTime / 1000);
		}

		if (m_gameTime != m_roomInfo.m_value51) {
			packet.Encode1(8);
			if (m_gameType == 10)
				packet.Encode1((int)m_gameTime / 1000);
			else
				packet.Encode1((int)m_gameTime / 60000);
		}

		if (m_userLimit != m_roomInfo.m_userLimit) {
			packet.Encode1(7);
			packet.Encode1(m_userLimit);
		}

		if (m_gameType != m_roomInfo.m_gameType) {
			packet.Encode1(2);
			packet.Encode1(m_gameType);
		}

		if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 2) {
			packet.Encode1(5);
			packet.Encode1(m_roomInfo.m_value4B);
		}

		if (m_holeType != m_roomInfo.m_value4B) {
			packet.Encode1(5);
			packet.Encode1(m_holeType);
		}

		if (m_course != m_roomInfo.m_course) {
			packet.Encode1(3);
			packet.Encode1(m_course);
		}

		packet.Send(SEND_TO_DEFAULT);

		AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 1, 0, 0, 0, 0, 0));
	}

	OnFreshOkay();
}

// changeroominfodlg.cpp:1079
void FrChangeRoomInfoDlg::ChangeGameType(int gameType, bool b1, bool b2) {
	if (m_gameType == 2 && gameType != 2) {
		m_pCourseFrame->SetVisible(true);
		m_pMapPrev->SetVisible(true);
		m_pMapNext->SetVisible(true);
		m_pHoleType->SetVisible(true);
		m_pCourse->SetVisible(true);
	} else if (m_gameType != 2 && gameType == 2) {
		m_pCourseFrame->SetVisible(false);
		m_pMapPrev->SetVisible(false);
		m_pMapNext->SetVisible(false);
		m_pHoleType->SetVisible(false);
		m_pCourse->SetVisible(false);
	} else if (m_gameType == 6 && gameType != 6) {
		m_pCourse->SetMouseEvent(true);
		ChangeCourse(FirstMap());
	} else if (m_gameType != 6 && gameType == 6) {
		m_pCourse->SetMouseEvent(false);
		ChangeCourse(0x7F);

		m_pMapPrev->Enable(false);
		m_pMapNext->Enable(false);
	} else if (m_gameType == 10 && gameType != 10) {
		m_pCourse->SetMouseEvent(true);
		ChangeCourse(FirstMap());
	} else if (m_gameType != 10 && gameType == 10) {
		m_pCourse->SetMouseEvent(false);
		ChangeCourse(0x7F);

		m_pMapPrev->Enable(false);
		m_pMapNext->Enable(false);
	}

	m_gameType = gameType;
	switch (m_gameType) {
	case 0:
		m_pText->SetBgImg("text_vs");
		m_textType = 0;
		if (!b2) {
			m_userLimit = 2;
			m_hole = 3;
			m_shotTime = 40000;
			m_timeLimit = 40;
		}
		break;
	case 1:
		m_pText->SetBgImg("text_vs");
		m_textType = 0;
		if (!b2) {
			m_userLimit = 4;
			m_hole = 6;
			m_shotTime = 40000;
			m_timeLimit = 40;
		}
		break;
	case 4:
	case 5:
	case 6:
		m_pText->SetBgImg("text_mass");
		m_textType = 1;
		break;
	case 7:
		m_pText->SetBgImg("text_vs");
		m_textType = 2;
		break;
	case 9:
	case 10:
		m_pText->SetBgImg("text_vs");
		m_textType = 2;
		break;
	case 2:
		m_pText->SetBgImg("text_chat");
		m_textType = 3;
		break;
	}
}

// changeroominfodlg.cpp:1200
void FrChangeRoomInfoDlg::ChangeUserLimit(int userLimit) {
	m_userLimit = userLimit;
}

// changeroominfodlg.cpp:1205
void FrChangeRoomInfoDlg::ChangeHole(int hole) {
	m_hole = hole;
	switch (m_gameType) {
	case 0:
	case 1:
	case 7:
		if (hole == 18) {
			if (m_holeType == 1 || m_holeType == 2) {
				m_holeType = 0;
			}
		}
		break;
	case 4:
	case 5:
		if (hole == 9) {
			m_timeLimit = 15;
		} else if (hole == 18) {
			m_timeLimit = 30;
			if (m_holeType == 1 || m_holeType == 2) {
				m_holeType = 0;
			}
		}
		break;
	case 6:
		if (hole == 9) {
			m_timeLimit = 20;
		} else if (hole == 18) {
			m_timeLimit = 30;
			m_holeType = 0;
		}
		break;
	}
}

// changeroominfodlg.cpp:1252
void FrChangeRoomInfoDlg::ChangeTimeLimit(int timeLimit) {
	m_timeLimit = timeLimit;
}

// changeroominfodlg.cpp:1257
void FrChangeRoomInfoDlg::ChangeCourse(int course) {
	IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(0x28000000 | ((course > 0x7F && course != 0xFD) ? course - 0x80 : course));
	if (pCourse == 0)
		return;

	if (course != 0x7F) {
		m_pCourse->Open(MakeStr("small_%s.tga", pCourse->m_icon));
	} else {
		m_pCourse->Open("map_random_middle.tga");
	}

	m_course = course;

	if (m_pMapPrev)
		m_pMapPrev->Enable(m_course != FirstMap());

	if (m_pMapNext)
		m_pMapNext->Enable(m_course != LastMap(false));
}

// changeroominfodlg.cpp:1282
void FrChangeRoomInfoDlg::ChangeHoleType(eHoleType holeType) {
	m_holeType = holeType;
}

// changeroominfodlg.cpp:1289
void FrChangeRoomInfoDlg::OnProc(const float dt) {
	FrForm::OnProc(dt);

	if (m_pMapSelectDlg == 0 && m_btnDelay >= 0.0f) {
		m_btnDelay -= dt;
	}
}

// changeroominfodlg.cpp:1299
bool FrChangeRoomInfoDlg::IsBtnEnable(void) {
	if (m_pMapSelectDlg == 0 && m_btnDelay < 0.0f)
		return true;

	return false;
}
