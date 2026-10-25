// JP-only game code: CSingleModeStartDlg, the single practice mode start
// form (JpSingleModeStartDlg.h), and the three CLobbyMain glue functions
// LobbyMain.cpp declares under JP_PAKS.
#include "JpSingleModeStartDlg.h"

#include "ObjectFactory.h"
#include "TaskManager.h"
#include "actor.h"
#include "frarea.h"
#include "frbutton.h"
#include "fresh.h"
#include "frwnd.h"

class FrWndManager;

// lib: 0xccd8c0 (m_RTTI's dynamic initializer, compiler-generated from
// this definition: WRTTI::WRTTI 0xa4c140, base FrForm)
const WRTTI CSingleModeStartDlg::m_RTTI("FrSingleModeStarter", &FrForm::m_RTTI);

// 0x521e40
IObject* CSingleModeStartDlgMakeInstance(void) {
	return new CSingleModeStartDlg;
}

// 0x521eb0 (static init 0xccd8e0): registration with the form factory
struct __sCSingleModeStartDlg {
	__sCSingleModeStartDlg(void) {
		ObjectFactory().AddObjectFunctor(CSingleModeStartDlgMakeInstance, "FrSingleModeStarter");
	}
};
static __sCSingleModeStartDlg s_registerCSingleModeStartDlg;

// 0x521710
const sFRESH_MSGMAP* CSingleModeStartDlg::GetMessageMap(void) const {
	return &CSingleModeStartDlg::_MsgMap;
}

sFRESH_MSGMAP CSingleModeStartDlg::_MsgMap = { &FrForm::_MsgMap, &CSingleModeStartDlg::_MsgEntries[0] };

// 0xe0a790 (the handlers are stored by the static init 0xccd900)
sFRESH_ENTRY CSingleModeStartDlg::_MsgEntries[] = {
	{ "expl", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CSingleModeStartDlg::OnExplInit },
	{ "expl", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CSingleModeStartDlg::OnExplClick },
	{ "close", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CSingleModeStartDlg::OnClose },
	{ "img_course", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CSingleModeStartDlg::OnCourseImgInit },
	{ "img_chipin", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CSingleModeStartDlg::OnChipinImgInit },
	{ "btn_course", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CSingleModeStartDlg::OnCourseBtnInit },
	{ "btn_repeat", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CSingleModeStartDlg::OnRepeatBtnInit },
	{ "btn_chipin", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CSingleModeStartDlg::OnChipinBtnInit },
	{ "btn_tutorial", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CSingleModeStartDlg::OnTutorialBtnInit },
	{ "btn_course", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CSingleModeStartDlg::OnCourseClick },
	{ "btn_repeat", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CSingleModeStartDlg::OnRepeatClick },
	{ "btn_chipin", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CSingleModeStartDlg::OnChipinClick },
	{ "btn_tutorial", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CSingleModeStartDlg::OnTutorialClick },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0xe0a974: the "expl" button's images per page (normal, hover).
static const char* const s_explButtonImg[2][2] = {
	{ "single_start_btn_expl_off_Normal", "single_start_btn_expl_off_Hover" },
	{ "single_start_btn_expl_on_Normal", "single_start_btn_expl_on_Hover" }
};

// 0x521720
void CSingleModeStartDlg::OnClose(void) {
	Close(FrCANCEL, true);
}

// 0x521730
void CSingleModeStartDlg::OnCourseClick(void) {
	m_kind = 0;
	Close(FrYES, true);
}

// 0x521750
void CSingleModeStartDlg::OnRepeatClick(void) {
	m_kind = 4;
	Close(FrYES, true);
}

// 0x521770
void CSingleModeStartDlg::OnChipinClick(void) {
	m_kind = 6;
	Close(FrYES, true);
}

// 0x521790
CSingleModeStartDlg::CSingleModeStartDlg(void) {
	m_pExplButton = 0;
	m_pCourseImg = 0;
	m_pChipinImg = 0;
	m_page = 0;
	m_kind = 0xff;

	m_pages[0].courseImg = g_pFresh->GetBitmap("single_start_img_course");
	m_pages[0].chipinImg = g_pFresh->GetBitmap("single_start_img_chipin");
	m_pages[1].courseImg = g_pFresh->GetBitmap("single_start_expl_course");
	m_pages[1].chipinImg = g_pFresh->GetBitmap("single_start_expl_chipin");

	m_offsets[0].course = WPoint(45.0f, 5.0f);
	m_offsets[0].chipin = WPoint(0.0f, 5.0f);
	m_offsets[1].course = WPoint(-45.0f, -5.0f);
	m_offsets[1].chipin = WPoint(0.0f, -5.0f);
}

// 0x5218f0
CSingleModeStartDlg::~CSingleModeStartDlg(void) {
}

// 0x521920 (0x521b90 jumps here)
void CSingleModeStartDlg::OnExplClick(void) {
	m_page = (m_page + 1) % 2;

	if (m_pCourseImg) {
		m_pCourseImg->SetBgImg(m_pages[m_page].courseImg);
		const WRect& rect = m_pCourseImg->GetRect();
		m_pCourseImg->SetRect(WRect(rect.x + m_offsets[m_page].course.x, rect.y + m_offsets[m_page].course.y, rect.w, rect.h));
	}

	if (m_pChipinImg) {
		m_pChipinImg->SetBgImg(m_pages[m_page].chipinImg);
		const WRect& rect = m_pChipinImg->GetRect();
		m_pChipinImg->SetRect(WRect(rect.x + m_offsets[m_page].chipin.x, rect.y + m_offsets[m_page].chipin.y, rect.w, rect.h));
	}

	if (m_pExplButton) {
		m_pExplButton->SetButtonImg(s_explButtonImg[m_page][0], FrButton::NORMAL);
		m_pExplButton->SetButtonImg(s_explButtonImg[m_page][1], FrButton::OVER);
		m_pExplButton->SetButtonImg(s_explButtonImg[m_page][0], FrButton::PRESSED);
	}

	if (m_pCourseButton) {
		m_pCourseButton->SetVisible(m_page == 0);
		m_pRepeatButton->SetVisible(m_page == 0);
		m_pChipinButton->SetVisible(m_page == 0);
		m_pTutorialButton->SetVisible(m_page == 0);
	}
}

// 0x521b00
void CSingleModeStartDlg::OnExplInit(PG_MSGARG param) {
	m_pExplButton = DYNAMIC_CAST(FrButton, param);
	if (m_pExplButton) {
		m_pExplButton->SetPushDelay(0.0f);
		m_pExplButton->UseDblClick(false);
	}
}

// 0x521ba0
void CSingleModeStartDlg::OnCourseImgInit(PG_MSGARG param) {
	m_pCourseImg = DYNAMIC_CAST(FrArea, param);
}

// 0x521c10
void CSingleModeStartDlg::OnChipinImgInit(PG_MSGARG param) {
	m_pChipinImg = DYNAMIC_CAST(FrArea, param);
}

// 0x521c80
void CSingleModeStartDlg::OnCourseBtnInit(PG_MSGARG param) {
	m_pCourseButton = DYNAMIC_CAST(FrButton, param);
}

// 0x521cf0
void CSingleModeStartDlg::OnRepeatBtnInit(PG_MSGARG param) {
	m_pRepeatButton = DYNAMIC_CAST(FrButton, param);
}

// 0x521d60
void CSingleModeStartDlg::OnChipinBtnInit(PG_MSGARG param) {
	m_pChipinButton = DYNAMIC_CAST(FrButton, param);
}

// 0x521dd0
void CSingleModeStartDlg::OnTutorialBtnInit(PG_MSGARG param) {
	m_pTutorialButton = DYNAMIC_CAST(FrButton, param);
}

// 0x521ed0
void CSingleModeStartDlg::OnTutorialClick(void) {
	AfxGetTask()->GetActor("Lobby") << MsgObject(0, 0x63, 0, 0, 0, 0, 0);
	Close(FrYES, true);
}

// 0x595490
FrForm* CreateSingleModeStartDlg(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent) {
	return CreateForm<CSingleModeStartDlg>(manager, owner, name, parent);
}

// DYNAMIC_CAST(CSingleModeStartDlg, form) as CLobbyMain::OnSingleModeStartDlgResult spells it
CSingleModeStartDlg* SingleModeStartDlg_Cast(FrForm* form) {
	return DYNAMIC_CAST(CSingleModeStartDlg, form);
}

// the kind byte (JP +0x160) CLobbyMain::OnSingleModeStartDlgResult reads
unsigned char SingleModeStartDlg_GetKind(const CSingleModeStartDlg* dialog) {
	return dialog->GetKind();
}
