// Original: d:\build\custom\temp\code\client\projectg\rankingdlg.cpp
#include "NetworkSystem.h"
#include <windows.h>
#include <atlbase.h>	// stand-in for the precompiled header: _AtlWinModule initializer
#include "RankingDlg.h"
#include "ObjectFactory.h"
#include "actor.h"
#include "frstatic.h"
#include "frbutton.h"
#include "frlistbox.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "SharedDoc.h"
#include "SoundManager.h"
#include "frcombobox.h"
#include "fredit.h"
#include "frviewer.h"
#include "frarea.h"
#include "frtextbutton.h"
#include "frgaugebar.h"
#include "winput.h"
#include "InputManager.h"

// The IME input device (public ?g_ime@@3PAVWInputDev@@A).
extern WInputDev* g_ime;
#include "binstr.h"
#include "exhibition.h"
#include "parttidlist.h"
#include "EmoticonDlg.h"
#include "TaskManager.h"
#include "avatar_task.h"
#include "avatar_avatar.h"
#include "commonutil.h"
#include "Packet.h"
#include "shared/Localize.h"
#include "frgraphicinterface.h"
#include "wview.h"
#include "woverlay.h"
#include "mousecursor.h"
#include "bitmap.h"
#include "ProjectG.h"

// Names of the first entries, read through constants: the tables are
// initialized by code (dynamic initializers that store each member).
static const char* const RANK_TYPE_TOTAL_NAME = "\xc1\xbe\xc7\xd5 \xb7\xa9\xc5\xb7";
static const char* const CLASS_TYPE_ALL_NAME = "\xc0\xfc\xc3\xbc";
static const char* const GEN_SUB_TYPE_TOTAL_NAME = "\xc1\xbe\xc7\xd5 \xc1\xa1\xbc\xf6";
static const char* const COURSE_TYPE_BLUE_LAGOON_NAME = "\xba\xed\xb7\xe7\xb6\xf3\xb1\xba";
static const char* const RECORD_TYPE_ALBATROSS_NAME = "\xbe\xcb\xb9\xd9\xc6\xae\xb7\xce\xbd\xba";
static const char* const TERM_TYPE_SEASON5_NAME = "\xbd\xc3\xc1\xf0" "5";
static const char* const SEARCH_TYPE_NICKNAME_NAME = "\xb4\xd0\xb3\xd7\xc0\xd3";

// rankingdlg.cpp:49
const sTypeName FrRankingDlg::s_aRankType[] = {
	{ 0, RANK_TYPE_TOTAL_NAME },
	{ 1, "\xc4\xda\xbd\xba\xba\xb0 \xb7\xa9\xc5\xb7" },
	{ 2, "\xb1\xe2\xb7\xcf \xb7\xa9\xc5\xb7" }
};

// rankingdlg.cpp:56
const sTypeName FrRankingDlg::s_aClassType[] = {
	{ 0, CLASS_TYPE_ALL_NAME },
	{ 1, "\xc3\xca\xb1\xde" },
	{ 2, "\xc1\xdf\xb1\xde" },
	{ 3, "\xbb\xf3\xb1\xde" }
};

// rankingdlg.cpp:65
const sTypeName FrRankingDlg::s_aGenSubType[] = {
	{ 0, GEN_SUB_TYPE_TOTAL_NAME },
	{ 1, "\xbd\xba\xc4\xda\xbe\xee \xc1\xa1\xbc\xf6" },
	{ 2, "\xc6\xae\xb7\xce\xc7\xc7 \xc1\xa1\xbc\xf6" },
	{ 3, "\xc8\xb9\xb5\xe6\xc6\xce \xc1\xa1\xbc\xf6" },
	{ 4, "\xc7\xc3\xb7\xb9\xc0\xcc\xc8\xa6 \xc1\xa1\xbc\xf6" }
};

// rankingdlg.cpp:176
const sTypeName FrRankingDlg::s_aCourseType[] = {
	{ 0, COURSE_TYPE_BLUE_LAGOON_NAME },
	{ 1, "\xba\xed\xb7\xe7\xbf\xf6\xc5\xcd" },
	{ 2, "\xbc\xbc\xc7\xc7\xbe\xc6\xc0\xa9\xb5\xe5" },
	{ 3, "\xc0\xa9\xb5\xe5\xc8\xfa" },
	{ 4, "\xc0\xa7\xc1\xee\xc0\xa7\xc1\xee" },
	{ 5, "\xbf\xfe\xbd\xba\xc6\xae\xc0\xa7\xc1\xee" },
	{ 6, "\xba\xed\xb7\xe7\xb9\xae" },
	{ 7, "\xbd\xc7\xba\xf1\xbe\xc6\xc4\xb3\xb3\xed" },
	{ 8, "\xbe\xc6\xc0\xcc\xbd\xba\xc4\xb3\xb3\xed" },
	{ 9, "\xc8\xad\xc0\xcc\xc6\xae\xc0\xa7\xc1\xee" },
	{ 10, "\xbb\xfe\xc0\xcc\xb4\xd7\xbb\xf7\xb5\xe5" },
	{ 11, "\xc7\xce\xc5\xa9\xc0\xa9\xb5\xe5" },
	{ 12, "\xb5\xf6\xc0\xce\xc6\xe4\xb8\xa3\xb3\xeb" },
	{ 13, "\xbe\xc6\xc0\xcc\xbd\xba \xbd\xba\xc6\xc4" },
	{ 14, "\xb7\xce\xbd\xba\xc6\xae \xbd\xc3\xbf\xfe\xc0\xcc" },
	{ 15, "\xc0\xcc\xbd\xba\xc5\xcf \xba\xa7\xb8\xae" },

	{ 16, "\xc0\xa7\xc1\xee \xbd\xc3\xc6\xbc" }
};

// rankingdlg.cpp:199
const sTypeName FrRankingDlg::s_aRecordType[] = {
	{ 0, RECORD_TYPE_ALBATROSS_NAME },
	{ 1, "\xc8\xa6\xc0\xce\xbf\xf8" },
	{ 2, "\xb0\xad\xc1\xbe\xc0\xb2" },
	{ 3, "\xb7\xb9\xba\xa7" },
	{ 4, "\xc3\xd1\xc5\xb8\xb1\xb8\xba\xf1\xb0\xc5\xb8\xae" }
};

// rankingdlg.cpp:209
const sTypeName FrRankingDlg::s_aTermType[] = {
	{ 0, TERM_TYPE_SEASON5_NAME }
};

// rankingdlg.cpp:214
const sTypeName FrRankingDlg::s_aSearchType[] = {
	{ 0, SEARCH_TYPE_NICKNAME_NAME },





	{ 1, "\xbc\xf8\xc0\xa7" }
};

// rankingdlg.cpp:235
long GetRealCourseType(long course) {
	long realCourse = course;
	if (course == 12) {
		realCourse = 13;
	} else if (course == 13) {
		realCourse = 14;
	} else if (course == 14) {
		realCourse = 15;
	} else if (course == 15) {
		realCourse = 16;
	} else if (course == 16) {
		realCourse = 19;
	}

	return realCourse;
}

// rankingdlg.cpp:265 (run-time type and factory registration of FrRankingDlg)
const WRTTI FrRankingDlg::m_RTTI("FrRankingDlg", &FrForm::m_RTTI);

IObject* FrRankingDlgMakeInstance(void) {
	return new FrRankingDlg;
}

struct __impFrRankingDlg {
	__impFrRankingDlg(void) {
		ObjectFactory().AddObjectFunctor(FrRankingDlgMakeInstance, "FrRankingDlg");
		ActorFactory().AddActorClass("FrRankingDlg");
	}
};
static __impFrRankingDlg s_impFrRankingDlg;

// rankingdlg.cpp:267
const sFRESH_MSGMAP* FrRankingDlg::GetMessageMap(void) const {
	return &FrRankingDlg::_MsgMap;
}

sFRESH_MSGMAP FrRankingDlg::_MsgMap = { &FrForm::_MsgMap, &FrRankingDlg::_MsgEntries[0] };

sFRESH_ENTRY FrRankingDlg::_MsgEntries[] = {
	// rankingdlg.cpp:269
	{ "class_type", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnClassType_ComboInit },
	// rankingdlg.cpp:270
	{ "class_type", FRCMD_LBUTTONDOWN, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnClassType_ComboDown },
	// rankingdlg.cpp:271
	{ "btn_current_season", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnTermType_S5_BtnInit },
	// rankingdlg.cpp:272
	{ "btn_current_season", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnTermType_S5_BtnUp },
	// rankingdlg.cpp:273
	{ "course", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnCourseAreaInit },
	// rankingdlg.cpp:274
	{ "course", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnCourseAreaOwnerDraw },
	// rankingdlg.cpp:275
	{ "user_rank", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnUserRankEditInit },
	// rankingdlg.cpp:276
	{ "user", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnRankList_Init },
	// rankingdlg.cpp:277
	{ "user", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnRankList_OwnerDraw },
	// rankingdlg.cpp:278
	{ "user", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnRankList_LBtnDown },
	// rankingdlg.cpp:279
	{ "prev_page", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnRankList_PrevPageInit },
	// rankingdlg.cpp:280
	{ "prev_page", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnRankList_PrevPageBtnUp },
	// rankingdlg.cpp:281
	{ "next_page", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnRankList_NextPageInit },
	// rankingdlg.cpp:282
	{ "next_page", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnRankList_NextPageBtnUp },
	// rankingdlg.cpp:283
	{ "cur_page", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnRankList_CurrentPageInit },
	// rankingdlg.cpp:284
	{ "tot_page", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnRankList_TotalPageInit },
	// rankingdlg.cpp:285
	{ "rlist_myinfo", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnRankList_MyInfo_Init },
	// rankingdlg.cpp:286
	{ "rlist_myinfo", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnRankList_MyInfo_LBtnDown },
	// rankingdlg.cpp:287
	{ "rlist_myinfo", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnRankList_MyInfo_OwnerDraw },
	// rankingdlg.cpp:288
	{ "rlist_s6", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnRankList_NoInRank_Init },
	// rankingdlg.cpp:289
	{ "search_user", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnRankList_SearchUserInit },
	// rankingdlg.cpp:290
	{ "search_user", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnRankList_SearchUserBtnUp },
	// rankingdlg.cpp:291
	{ "edit_id", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnRankList_UserIdEditInit },
	// rankingdlg.cpp:292
	{ "edit_id", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnRankList_UserIdEditDown },
	// rankingdlg.cpp:293
	{ "emoticon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnEmoticonInit },
	// rankingdlg.cpp:294
	{ "emoticon", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnEmoticonBtnUp },
	// rankingdlg.cpp:295
	{ "language", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnLanguageInit },
	// rankingdlg.cpp:296
	{ "pet_char", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnPetCharacterInit },
	// rankingdlg.cpp:297
	{ "pet_char", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnPetCharacterOwnerDraw },
	// rankingdlg.cpp:298
	{ "pet_load", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnPetLoadInit },
	// rankingdlg.cpp:299
	{ "info_up", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnUpperMiniInfoInit },
	// rankingdlg.cpp:300
	{ "info_up", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnUpperMiniInfoOwnerDraw },
	// rankingdlg.cpp:301
	{ "about", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnAboutBtnInit },
	// rankingdlg.cpp:302
	{ "about", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnAboutBtnUp },
	// rankingdlg.cpp:303
	{ "close", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnCloseBtnUp },
	// rankingdlg.cpp:304
	{ "message", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnBottomMessageInit },
	// rankingdlg.cpp:306
	{ "rank_type", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnRankTypeListInit },
	// rankingdlg.cpp:307
	{ "rank_type", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnRankTypeListLDown },
	// rankingdlg.cpp:308
	{ "rank_type", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnRankTypeListOwnerDraw },
	// rankingdlg.cpp:309
	{ "rank_type_v", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnRankTypeViewerInit },
	// rankingdlg.cpp:310
	{ "rank_type_v", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnRankTypeViewerDown },
	// rankingdlg.cpp:311
	{ "rank_type_v", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnRankTypeViewerOwnerDraw },
	// rankingdlg.cpp:313
	{ "gensub_type", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnGeneralSubTypeListInit },
	// rankingdlg.cpp:314
	{ "gensub_type", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnGeneralSubTypeListLDown },
	// rankingdlg.cpp:315
	{ "gensub_type", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnGeneralSubTypeListOwnerDraw },
	// rankingdlg.cpp:316
	{ "gensub_type_v", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnGeneralSubTypeViewerInit },
	// rankingdlg.cpp:317
	{ "gensub_type_v", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnGeneralSubTypeViewerDown },
	// rankingdlg.cpp:318
	{ "gensub_type_v", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnGeneralSubTypeViewerOwnerDraw },
	// rankingdlg.cpp:320
	{ "course_type", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnCourseTypeListInit },
	// rankingdlg.cpp:321
	{ "course_type", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnCourseTypeListLDown },
	// rankingdlg.cpp:322
	{ "course_type", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnCourseTypeListOwnerDraw },
	// rankingdlg.cpp:323
	{ "course_type_v", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnCourseTypeViewerInit },
	// rankingdlg.cpp:324
	{ "course_type_v", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnCourseTypeViewerDown },
	// rankingdlg.cpp:325
	{ "course_type_v", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnCourseTypeViewerOwnerDraw },
	// rankingdlg.cpp:327
	{ "record_type", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnRecordTypeListInit },
	// rankingdlg.cpp:328
	{ "record_type", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnRecordTypeListLDown },
	// rankingdlg.cpp:329
	{ "record_type", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnRecordTypeListOwnerDraw },
	// rankingdlg.cpp:330
	{ "record_type_v", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnRecordTypeViewerInit },
	// rankingdlg.cpp:331
	{ "record_type_v", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnRecordTypeViewerDown },
	// rankingdlg.cpp:332
	{ "record_type_v", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnRecordTypeViewerOwnerDraw },
	// rankingdlg.cpp:334
	{ "search_type", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnSearchTypeListInit },
	// rankingdlg.cpp:335
	{ "search_type", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnSearchTypeListLDown },
	// rankingdlg.cpp:336
	{ "search_type", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnSearchTypeListOwnerDraw },
	// rankingdlg.cpp:337
	{ "search_type_v", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnSearchTypeViewerInit },
	// rankingdlg.cpp:338
	{ "search_type_v", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnSearchTypeViewerDown },
	// rankingdlg.cpp:339
	{ "search_type_v", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnSearchTypeViewerOwnerDraw },
	// rankingdlg.cpp:341
	{ "ghost_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingDlg::OnGhostBtnInit },
	// rankingdlg.cpp:342
	{ "ghost_btn", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrRankingDlg::OnGhostBtnUp },
	// rankingdlg.cpp:344
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// rankingdlg.cpp:352
FrRankingDlg::FrRankingDlg(void) {
	s_iRankType = 0;
	s_iSubType = 0;
	s_iTermType = 0;
	s_iClassType = 0;
	s_iPageNum = 0;
	s_iPageTotal = 0;
	s_iSearchType = 0;

	m_pExhibition = 0;
	m_pPetData = 0;
	m_pUpperMiniInfo = 0;
	m_userInfoDelay = -1.0f;
	m_rankList.clear();
	memset(&m_userInfo, 0, sizeof(m_userInfo));

	m_userInfo.uid = 0xffffffff;
	m_myInfo.uid = 0xffffffff;
	ResetControl();

	if (IsLocalContent((localContentType_t)0x59)) {
		WSendPacket packet(GCT_UCC_REQUEST);
		packet.Encode1(2);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// rankingdlg.cpp:383
FrRankingDlg::~FrRankingDlg(void) {
	if (m_pExhibition) {
		delete m_pExhibition;
		m_pExhibition = 0;
	}
	if (m_pPetData) {
		delete m_pPetData;
		m_pPetData = 0;
	}

	g_resrcmng->UnlockAllByThread(m_anotherHand.GetThreadID());
	m_anotherHand.BreakWorkingThread();

	if (CMouseCursor::Instance()) {
		CMouseCursor::Instance()->SetActive(true);
	}

	if (IsLocalContent((localContentType_t)0x59)) {
		if (WNetworkSystem::Instance()) {
			WSendPacket packet(GCT_UCC_REQUEST);
			packet.Encode1(1);
			packet.Send(SEND_TO_DEFAULT);
		}
	}
}

// rankingdlg.cpp:409
bool FrRankingDlg::OnInit(void) {
	FrForm::OnInit();

	SendRequestRank(true);
	SendRequestUserInfo(MyUID(), CSharedDoc::Instance()->GetPangyaUserInfo().id);

	return true;
}

// rankingdlg.cpp:419
void FrRankingDlg::OnCancel(void) {
	Close(true);
}

// rankingdlg.cpp:424
void FrRankingDlg::OnProc(const float dt) {
	if (m_userInfoDelay > 0.0f) {
		m_userInfoDelay -= dt;

		if (m_userInfoDelay <= 0.0f) {
			SetVisibleUserInfo(true);
			SetAllControlsEnable(true);
		}
	}

	if (g_input->GetButton(LEFT_BUTTON) || g_input->GetButton(RIGHT_BUTTON)) {
		CloseAllComboBtns();
	}

	if (g_input->GetDown("ENTER", true) || g_input->GetDown("PADENTER", true)) {
		if (m_pUserIdEdit->IsViewFocused()) {
			OnRankList_SearchUserBtnUp();
			g_audio->PlaySfx("ui_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
		}
	}

	if (m_pLanguageArea) {
		// IME mode the indicator shows (data 0xABE7A0, no symbol)
		static bool s_bAlphaNumericMode;

		if (g_ime->IsAlphaNumericMode() != s_bAlphaNumericMode) {
			s_bAlphaNumericMode = g_ime->IsAlphaNumericMode();

			if (s_bAlphaNumericMode) {
				m_pLanguageArea->SetBgImg("chat_eng");
			} else {
				m_pLanguageArea->SetBgImg("chat_kor");
			}
		}
	}

	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) && g_input->GetDown("EMOTICON", false)) {
		OnEmoticonBtnUp();
	}
}

// rankingdlg.cpp:541
void FrRankingDlg::ResetControl(void) {
	m_pTermS5Btn = 0;

	m_pRankList = 0;
	m_pMyInfoList = 0;
	m_pClassTypeCombo = 0;
	m_pCourseArea = 0;
	m_pNoMyRankWnd = 0;
	m_pPetCharacterArea = 0;
	m_pSearchUserBtn = 0;
	m_pEmoticonBtn = 0;
	m_pUserIdEdit = 0;
	m_pCurPageEdit = 0;
	m_pTotalPageEdit = 0;
	m_pUserRankEdit = 0;
	m_pRankTypeList = 0;
	m_pRankTypeViewer = 0;
	m_pGenSubTypeList = 0;
	m_pGenSubTypeViewer = 0;
	m_pCourseTypeList = 0;
	m_pCourseTypeViewer = 0;
	m_pRecordTypeList = 0;
	m_pRecordTypeViewer = 0;
	m_pSearchTypeList = 0;
	m_pSearchTypeViewer = 0;
	m_pPetLoadGauge = 0;
	m_pBottomMsg = 0;
	m_pEmoticonDlg = 0;
	m_pAboutDlg = 0;
	m_pGhostBtn = 0;

	memset(m_pRankTypeViewerImg, 0, sizeof(m_pRankTypeViewerImg));
	memset(m_pGenSubTypeViewerImg, 0, sizeof(m_pGenSubTypeViewerImg));
	memset(m_pCourseTypeViewerImg, 0, sizeof(m_pCourseTypeViewerImg));
	memset(m_pRecordTypeViewerImg, 0, sizeof(m_pRecordTypeViewerImg));
	memset(m_pSearchTypeViewerImg, 0, sizeof(m_pSearchTypeViewerImg));
}

// rankingdlg.cpp:580
void FrRankingDlg::SetVisibleByRankType(void) {
	if (m_pGenSubTypeViewer) {
		m_pGenSubTypeViewer->SetVisible(s_iRankType == 0);
	}

	if (m_pCourseTypeViewer) {
		m_pCourseTypeViewer->SetVisible(s_iRankType == 1);
	}

	if (m_pRecordTypeViewer) {
		m_pRecordTypeViewer->SetVisible(s_iRankType == 2);
	}
}

// rankingdlg.cpp:597
void FrRankingDlg::SetVisibleUserInfo(bool bVisible) {
	if (m_pPetCharacterArea) {
		m_pPetCharacterArea->SetVisible(bVisible);
	}
	if (m_pUpperMiniInfo) {
		m_pUpperMiniInfo->SetVisible(bVisible);
	}
	if (m_pPetLoadGauge) {
		// the gauge shows while the pet loads
		m_pPetLoadGauge->SetVisible(!bVisible);
		m_pPetLoadGauge->SetPos(0);
		m_pPetLoadGauge->SetDestPos(150, bVisible);
	}

	if (m_pUserRankEdit) {
		m_pUserRankEdit->SetVisible(bVisible);
	}
}

// rankingdlg.cpp:616
void FrRankingDlg::UpdateRankList(void) {
	if (m_pRankList) {
		m_pRankList->ClearItem();

		int count = 0;
		std::list<ranktype::sListInfo>::iterator it;
		for (it = m_rankList.begin(); it != m_rankList.end(); ++it) {
			if (count >= 12) {
				break;
			}

			m_pRankList->AddItem(&*it);
			count++;
		}
	}

	SynchronizeButtons();

	char text[32];
	itoa(s_iPageNum + 1, text, 10);
	m_pCurPageEdit->SetLine(1, text, 0, false, 0);
	m_pCurPageEdit->SetVisible(true);
}

// rankingdlg.cpp:652
void FrRankingDlg::ClearRankList(void) {
	if (m_pRankList) {
		m_pRankList->ClearItem();
	}

	char text[32];
	s_iPageNum = 0;
	itoa(s_iPageNum, text, 10);
	m_pCurPageEdit->SetLine(1, text, 0, false, 0);

	s_iPageTotal = 0;
	itoa(s_iPageTotal, text, 10);
	m_pTotalPageEdit->SetLine(1, text, 0, false, 0);

	SynchronizeButtons();
}

// rankingdlg.cpp:676
void FrRankingDlg::UpdateMyInfo(void) {
	if (m_pMyInfoList) {
		m_pMyInfoList->ClearItem();

		if (m_myInfo.uid != -1) {
			m_pMyInfoList->AddItem(&m_myInfo);
			m_pMyInfoList->SetVisible(true);
			m_pNoMyRankWnd->SetVisible(false);
		} else {
			m_pMyInfoList->SetVisible(false);
			m_pNoMyRankWnd->SetVisible(true);
		}
	}
}

// rankingdlg.cpp:696
void FrRankingDlg::UpdateUserInfo(void) {
	m_anotherHand.Work(this, 0, 0, true, -1);

	if (m_pPetCharacterArea) {
		m_pPetCharacterArea->SetVisible(true);
	}

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	pGDI->SetTextStyle(1);

	char text[32];
	if (m_userInfo.ranks[0].rank <= 0) {
		sprintf(text, "- \xc0\xa7");
	} else {
		sprintf(text, "%d \xc0\xa7", m_userInfo.ranks[0].rank);
	}
	m_pUserRankEdit->SetLine(1, text, 0xffffffff, false, 0);

	pGDI->SetTextStyle(0);
}

// rankingdlg.cpp:473
void FrRankingDlg::OnAnotherHand(int nWork, void* pData) {
	if (nWork == 0) {
		SetPetCharacter();
	}
}

// rankingdlg.cpp:716
void FrRankingDlg::SetPetCharacter(void) {
	m_anotherHand.Enter();

	if (!m_pPetCharacterArea) {
		m_anotherHand.Leave();
		return;
	}

	m_pPetLoadGauge->SetVisible(true);
	if (m_pExhibition) { delete m_pExhibition; m_pExhibition = 0; } if (m_pPetData) { delete m_pPetData; m_pPetData = 0; }

	WRect rect = m_pPetCharacterArea->GetRect();

	m_pExhibition = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.15f, true);
	m_pExhibition->SetDistance(g_view->GetWidth() * 0.0203125f, true, false);
	m_pExhibition->SetArea(rect.w, rect.h);

	m_pPetData = new CPartTidList;

	sCharacterInfo* pCharInfo = &m_userInfo.charInfo;
	if (!m_pPetData->SetTids(pCharInfo, 0xFF) || !m_pPetData->IsComboValid())
		m_pPetData->SetDefaultTids();

	m_pExhibition->SetModel(*m_pPetData, pCharInfo->auxParts.tids, 0, 3.0f, 0);

	m_anotherHand.Leave();
}

// rankingdlg.cpp:746
void FrRankingDlg::SetListItemInfo(FrListItem* pItem) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	unsigned long color = 0xffffffff;

	ranktype::sListInfo* pInfo = (ranktype::sListInfo*)pItem->pData;
	if (pInfo->uid == m_userInfo.uid) {
		WOverlay::DrawBox(g_view, WRect(pItem->pos.x + 1.0f, pItem->pos.y - 5.0f, m_pRankList->GetItemWidth() + 17.0f, (float)m_pRankList->GetItemHeight()), 0, 0xa0ffff00, 0.0f);
		color = 0xff000000;
	}

	pGDI->SetTextStyle(0);
	if (pInfo->prevRank == 0 || fabs((float)pInfo->rank - (float)pInfo->prevRank) > 10000.0f) {
		const Bitmap* pImg = g_pFresh->GetManager()->GetBitmap("ICONS", "new");
		if (pImg) {
			pGDI->DrawTexture(pImg, WRect(pItem->pos.x + 45.0f, pItem->pos.y + 4.0f, (float)pImg->Width(), (float)pImg->Height()), 0xffffffff, 0);
		}
	} else if (pInfo->rank > pInfo->prevRank) {
		const Bitmap* pImg = g_pFresh->GetManager()->GetBitmap("RANKING", "dn");
		if (pImg) {
			pGDI->DrawTexture(pImg, WRect(pItem->pos.x + 45.0f, pItem->pos.y + 2.0f, (float)pImg->Width(), (float)pImg->Height()), 0xffffffff, 0);
		}
		pGDI->SetTextColor(0xff0000ff, 0xffffffff);
		pGDI->PrintText(WPoint(pImg->Width() + pItem->pos.x + 45.0f, pItem->pos.y), 0, MakeStr("%d", pInfo->rank - pInfo->prevRank), 0);
	} else if (pInfo->rank < pInfo->prevRank) {
		const Bitmap* pImg = g_pFresh->GetManager()->GetBitmap("RANKING", "up");
		if (pImg) {
			pGDI->DrawTexture(pImg, WRect(pItem->pos.x + 45.0f, pItem->pos.y + 2.0f, (float)pImg->Width(), (float)pImg->Height()), 0xffffffff, 0);
		}
		pGDI->SetTextColor(0xffff0000, 0xffffffff);
		pGDI->PrintText(WPoint(pImg->Width() + pItem->pos.x + 45.0f, pItem->pos.y), 0, MakeStr("%d", pInfo->prevRank - pInfo->rank), 0);
	} else {
		pGDI->SetTextColor(color, 0xffffffff);
		pGDI->PrintText(WPoint(pItem->pos.x + 53.0f, pItem->pos.y), 0, "-", 0);
	}

	const char* text;
	if (pInfo->rank == 0 || pInfo->rank > 10000 || pInfo->record == 0) {
		text = "-";
	} else {
		text = MakeStr("%d", pInfo->rank);
	}
	pGDI->SetTextColor(color, 0xffffffff);
	pGDI->SetTextStyle(1);
	pGDI->PrintText(WPoint(pItem->pos.x + 7.0f, pItem->pos.y), 0, text, 0);

	pGDI->SetTextStyle(0);

	if (!WSingleton<CProjectG>::Instance()->HidePrivacy()) {
		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 85.0f, pItem->pos.y), 0, pInfo->text30.c_str(), -1.0f, 0xffffffff);
	}
	pGDI->SetTextColor(color, 0xffffffff);

	if (s_iRankType == 2 && s_iSubType == 3) {
		int level = pInfo->level + 1;
		if (level >= 0x47) {
			level = 0x47;
		}
		const Bitmap* pImg = g_pFresh->GetManager()->GetBitmap("LEVELS", MakeStr("level_%03d", (unsigned short)level));
		if (pImg) {
			pGDI->DrawTexture(pImg, WRect(pItem->pos.x + 240.0f, pItem->pos.y - 4.0f, (float)pImg->Width(), (float)pImg->Height()), 0xffffffff, 0);
		}
	} else {
		if (pInfo->record > 9999999999) {
			pGDI->PrintText(WPoint(pItem->pos.x + 310.0f, pItem->pos.y), 2, "\xb1\xe2\xb7\xcf\xbe\xf8\xc0\xbd", 0);
		} else if (s_iRankType == 2 && s_iSubType == 2) {
			pGDI->PrintText(WPoint(pItem->pos.x + 310.0f, pItem->pos.y), 2, MakeStr("%.1f", pInfo->record * 0.1f), 0);
		} else if (s_iRankType == 2 && s_iSubType == 4) {
			pGDI->PrintText(WPoint(pItem->pos.x + 310.0f, pItem->pos.y), 2, MakeStr("%d \xb8\xb8y", pInfo->record / 10000), 0);
		} else {
			pGDI->PrintText(WPoint(pItem->pos.x + 310.0f, pItem->pos.y), 2, MakeStr("%d", pInfo->record), 0);
		}
	}
	pGDI->SetTextColor(0xffffffff, 0xffffffff);
}

// rankingdlg.cpp:845
void FrRankingDlg::ShowUserInfo(void) {
	if (m_userInfo.uid == 0xffffffff) {
		return;
	}

	WPoint pt = m_pUpperMiniInfo->GetRect().TopLeft();

	WPoint pos(pt.x, pt.y + 40.0f);
	DrawUserInfoLine(1, 0, pos);
	DrawUserInfoLine(2, 1, pos);
	DrawUserInfoLine(3, 2, pos);
	DrawUserInfoLine(4, 3, pos);
	pos.y += 7.0f;
	DrawUserInfoLine(0, 4, pos);
}

// rankingdlg.cpp:861
void FrRankingDlg::DrawUserInfoLine(int type, int line, WPoint& pos) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	pGDI->SetTextColor(0xffffffff, 0xffffffff);
	pGDI->SetTextStyle(0);

	unsigned long rank = m_userInfo.ranks[type].rank;
	const char* text;
	if (rank != 0 && rank < 10000 && m_userInfo.ranks[type].record != 0) {
		text = MakeStr("%d\xc0\xa7", rank);
	} else {
		text = "- ";
	}
	pGDI->SetTextColor(0xffffffff, 0xffffffff);
	pGDI->PrintText(WPoint(pos.x + 100.0f, line * 15 + pos.y), 2, text, 0);

	if (m_userInfo.ranks[type].record > 99999999) {
		pGDI->PrintText(WPoint(pos.x + 195.0f, line * 15 + pos.y), 2, "\xb1\xe2\xb7\xcf\xbe\xf8\xc0\xbd", 0);
	} else {
		pGDI->PrintText(WPoint(pos.x + 195.0f, line * 15 + pos.y), 2, MakeStr("%d", m_userInfo.ranks[type].record), 0);
	}

	unsigned long prevRank = m_userInfo.ranks[type].prevRank;

	if (prevRank == 0 || fabs((float)rank - (float)prevRank) > 10000.0f) {
		const Bitmap* pImg = g_pFresh->GetManager()->GetBitmap("ICONS", "new");
		if (pImg) {
			pGDI->DrawTexture(pImg, WRect(pos.x + 105.0f, line * 15 + pos.y + 4.0f, (float)pImg->Width(), (float)pImg->Height()), 0xffffffff, 0);
		}
	} else if (rank > prevRank) {
		const Bitmap* pImg = g_pFresh->GetManager()->GetBitmap("RANKING", "dn");
		if (pImg) {
			pGDI->DrawTexture(pImg, WRect(pos.x + 105.0f, line * 15 + pos.y + 2.0f, (float)pImg->Width(), (float)pImg->Height()), 0xffffffff, 0);
		}
		pGDI->SetTextColor(0xff0000ff, 0xffffffff);
		pGDI->PrintText(WPoint(pImg->Width() + pos.x + 105.0f, line * 15 + pos.y), 0, MakeStr("%d", rank - prevRank), 0);
		pGDI->SetTextColor(0xffffffff, 0xffffffff);
	} else if (rank < prevRank) {
		const Bitmap* pImg = g_pFresh->GetManager()->GetBitmap("RANKING", "up");
		if (pImg) {
			pGDI->DrawTexture(pImg, WRect(pos.x + 105.0f, line * 15 + pos.y + 2.0f, (float)pImg->Width(), (float)pImg->Height()), 0xffffffff, 0);
		}
		pGDI->SetTextColor(0xffff0000, 0xffffffff);
		pGDI->PrintText(WPoint(pImg->Width() + pos.x + 105.0f, line * 15 + pos.y), 0, MakeStr("%d", prevRank - rank), 0);
		pGDI->SetTextColor(0xffffffff, 0xffffffff);
	} else {
		pGDI->SetTextColor(0xffffffff, 0xffffffff);
		pGDI->PrintText(WPoint(pos.x + 113.0f, line * 15 + pos.y), 0, "-", 0);
	}
}

// rankingdlg.cpp:931
void FrRankingDlg::DisconnectRankServer(void) {
	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_RANK)) {
		WNetworkSystem::Instance()->ForceShutDown(WNetworkSystem::NET_UNIT_RANK);
	}
}

// rankingdlg.cpp:939
void FrRankingDlg::SetRankTypes(unsigned char rankType, unsigned char subType, unsigned char termType,
	unsigned char classType, int pageNum, int pageTotal) {
	s_iRankType = rankType;
	s_iSubType = subType;
	s_iTermType = termType;
	s_iClassType = classType;
	s_iPageNum = pageNum;
	s_iPageTotal = pageTotal;
}

// rankingdlg.cpp:949
void FrRankingDlg::SelectRankList(int index) {
	if (m_pRankList) {
		FrListItem* pItem = m_pRankList->GetItem(index);
		if (!pItem) {
			return;
		}

		m_pRankList->SelectItem(pItem, true);
	}

	FrListItem* pSelected = m_pRankList->GetSelected();
	ranktype::sListInfo* pInfo = (ranktype::sListInfo*)pSelected->pData;
	if (pInfo) {
		SendRequestUserInfo(pInfo->uid, pInfo->nickname.c_str());
	}
}

// rankingdlg.cpp:967
void FrRankingDlg::SynchronizeButtons(void) {
	if (m_pRankTypeList) {
		const char* name = ((FrLine*)m_pRankTypeList->GetSelected()->pData)->text.c_str();
		if (strcmpi(s_aRankType[s_iRankType].m_name, name) == 0) {
			m_pRankTypeList->SelectItem(m_pRankTypeList->GetItem(s_iRankType), true);
			m_pRankTypeList->SetVisible(false);
			SetVisibleByRankType();
		}
	}

	switch (s_iRankType) {
	case 0:
		if (m_pGenSubTypeList) {
			m_pGenSubTypeList->SelectItem(m_pGenSubTypeList->GetItem(s_iSubType), true);
			m_pGenSubTypeList->SetVisible(false);
		}
		break;

	case 1:
		if (m_pCourseTypeList) {
			m_pCourseTypeList->SelectItem(m_pCourseTypeList->GetItem(s_iSubType), true);
			m_pCourseTypeList->SetVisible(false);
		}
		break;

	case 2:
		if (m_pRecordTypeList) {
			m_pRecordTypeList->SelectItem(m_pRecordTypeList->GetItem(s_iSubType), true);
			m_pRecordTypeList->SetVisible(false);
		}
		break;
	}
}

// rankingdlg.cpp:1024
void FrRankingDlg::CloseAllComboBtns(void) {
	const WPoint& mousePos = g_pFresh->GetManager()->GetMousePos();
	WRect rect;
	FrListBox* pList;

	pList = m_pRankTypeList;
	if (pList && m_pRankTypeViewer) {
		if (pList->IsVisible()) {
			rect = m_pRankTypeViewer->GetRect();
			if (!rect.IsInRect(mousePos)) {
				rect = pList->GetRect();
				if (!rect.IsInRect(mousePos)) {
					m_pRankTypeList->SetVisible(false);
				}
			}
		}
	}

	pList = m_pGenSubTypeList;
	if (pList) {
		if (pList->IsVisible()) {
			rect = m_pGenSubTypeViewer->GetRect();
			if (!rect.IsInRect(mousePos)) {
				rect = pList->GetRect();
				if (!rect.IsInRect(mousePos)) {
					m_pGenSubTypeList->SetVisible(false);
				}
			}
		}
	}

	pList = m_pCourseTypeList;
	if (pList) {
		if (pList->IsVisible()) {
			rect = m_pCourseTypeViewer->GetRect();
			if (!rect.IsInRect(mousePos)) {
				rect = pList->GetRect();
				if (!rect.IsInRect(mousePos)) {
					m_pCourseTypeList->SetVisible(false);
				}
			}
		}
	}

	pList = m_pRecordTypeList;
	if (pList) {
		if (pList->IsVisible()) {
			rect = m_pRecordTypeViewer->GetRect();
			if (!rect.IsInRect(mousePos)) {
				rect = pList->GetRect();
				if (!rect.IsInRect(mousePos)) {
					m_pRecordTypeList->SetVisible(false);
				}
			}
		}
	}

	pList = m_pSearchTypeList;
	if (pList) {
		if (pList->IsVisible()) {
			rect = m_pSearchTypeViewer->GetRect();
			if (!rect.IsInRect(mousePos)) {
				rect = pList->GetRect();
				if (!rect.IsInRect(mousePos)) {
					m_pSearchTypeList->SetVisible(false);
				}
			}
		}
	}
}

// rankingdlg.cpp:1110
void FrRankingDlg::SetAllControlsEnable(bool bEnable) {
	m_pTermS5Btn->Enable(bEnable);

	m_pRankList->Enable(bEnable);
	m_pMyInfoList->Enable(bEnable);
	m_pSearchUserBtn->Enable(bEnable);
	m_pRankTypeList->Enable(bEnable);
	m_pRankTypeViewer->Enable(bEnable);
	m_pGenSubTypeList->Enable(bEnable);
	m_pGenSubTypeViewer->Enable(bEnable);
	m_pCourseTypeList->Enable(bEnable);
	m_pCourseTypeViewer->Enable(bEnable);
	m_pRecordTypeList->Enable(bEnable);
	m_pRecordTypeViewer->Enable(bEnable);
	m_pSearchTypeList->Enable(bEnable);
	m_pSearchTypeViewer->Enable(bEnable);
}

// rankingdlg.cpp:1130
void FrRankingDlg::HidePet(bool bHide) {
	// Both branches are the same; the compiler merges them.
	if (bHide) {
		if (m_pExhibition) { delete m_pExhibition; m_pExhibition = 0; }
		if (m_pPetData) { delete m_pPetData; m_pPetData = 0; }
	} else {
		if (m_pExhibition) { delete m_pExhibition; m_pExhibition = 0; }
		if (m_pPetData) { delete m_pPetData; m_pPetData = 0; }
	}
}

// rankingdlg.cpp:1144
void FrRankingDlg::SendRequestRank(bool bFirst) {
	if (!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_RANK)) {
		return;
	}

	WSendPacket packet(RC_REQUEST_RANK);

	packet.Encode4(MyUID());
	packet.EncodeStr(std::string(CSharedDoc::Instance()->GetPangyaUserInfo().id));
	packet.Encode1(s_iRankType);
	packet.Encode1(s_iSubType);
	packet.Encode1(s_iTermType);
	packet.Encode1(s_iClassType);
	packet.Encode4(s_iPageNum);
	packet.Encode1(bFirst);
	packet.Send(SEND_TO_RANK);

	m_userInfoDelay = 1.0f;
	SetAllControlsEnable(false);

	CRankingInfo::Instance()->GetDlg()->SetBottomMsg("");
}

// rankingdlg.cpp:1170
void FrRankingDlg::SendRequestUserInfo(unsigned long uid, const char* nickname) {
	if (!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_RANK)) {
		return;
	}

	if (uid == 0xffffffff) {
		return;
	}

	WSendPacket packet(RC_REQUEST_USERINFO);
	packet.Encode4(uid);
	packet.EncodeStr(nickname);
	packet.Encode1(s_iTermType);
	packet.Send(SEND_TO_RANK);

	m_userInfoDelay = 1.0f;
	SetVisibleUserInfo(false);
	SetAllControlsEnable(false);

	CRankingInfo::Instance()->GetDlg()->SetBottomMsg("");
}

// rankingdlg.cpp:1195
void FrRankingDlg::SendRequestSearchUser(const char* nickname) {
	if (!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_RANK)) {
		return;
	}

	int cmp = strcmp(nickname, "");
	if (cmp == 0) {
		// "Please enter the target to search."
		OnNotify("\xC3\xA3\xC0\xBB \xB4\xEB\xBB\xF3\xC0\xBB \xC0\xD4\xB7\xC2\xC7\xD8\xC1\xD6\xBD\xCA\xBD\xC3\xBF\xC0.", 2);
		return;
	}

	if (strstr(nickname, "'") || strstr(nickname, ";") || strstr(nickname, "\"")) {
		// "Special characters ' ; \" cannot be entered."
		OnNotify("\xC6\xAF\xBC\xF6\xB1\xE2\xC8\xA3 ' ; \x22 \xB4\xC2 \xC0\xD4\xB7\xC2\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 2);
		return;
	}

	WSendPacket packet(RC_REQUEST_SEARCHUSER);
	packet.Encode1(s_iSearchType);

	if (s_iSearchType == 1) {
		int rank = atoi(nickname);
		if (rank <= 0) {
			// "It is not a number or the value is too small."
			OnNotify("\xBC\xFD\xC0\xDA\xB0\xA1 \xBE\xC6\xB4\xCF\xB0\xC5\xB3\xAA \xB0\xAA\xC0\xCC \xB3\xCA\xB9\xAB \xC0\xDB\xBD\xC0\xB4\xCF\xB4\xD9.", 2);
			return;
		} else if (rank > 100000) {
			// "The rank to search is too large."
			OnNotify("\xC3\xA3\xB4\xC2 \xBC\xF8\xC0\xA7\xC0\xC7 \xB0\xAA\xC0\xCC \xB3\xCA\xB9\xAB \xC5\xAE\xB4\xCF\xB4\xD9.", 2);
			return;
		}

		packet.Encode4(rank);
	} else {
		packet.EncodeStr(std::string(nickname));
	}

	packet.Encode1(s_iRankType);
	packet.Encode1(s_iSubType);
	packet.Encode1(s_iTermType);
	packet.Encode1(s_iClassType);
	packet.Encode4(s_iPageNum);
	packet.Send(SEND_TO_RANK);

	m_userInfoDelay = 1.0f;
	SetAllControlsEnable(false);

	CRankingInfo::Instance()->GetDlg()->SetBottomMsg("");
}

// rankingdlg.cpp:1257
void FrRankingDlg::OnTermType_S5_BtnInit(int param) {
	m_pTermS5Btn = DYNAMIC_CAST(FrButton, param);
	m_pTermS5Btn->SetPushDelay(1.0f);

	m_pTermS5Btn->SetStatus(FrButton::PRESSED);
}

// rankingdlg.cpp:1265
void FrRankingDlg::OnTermType_S5_BtnUp(void) {
	s_iTermType = 0;

	m_pTermS5Btn->SetStatus(FrButton::PRESSED);

	s_iPageNum = 0;
	SendRequestRank(true);
	SendRequestUserInfo(m_userInfo.uid, m_userInfo.nickname);
}

// rankingdlg.cpp:1283
void FrRankingDlg::OnCourseAreaInit(int param) {
	m_pCourseArea = DYNAMIC_CAST(FrArea, param);
}

// rankingdlg.cpp:1288
void FrRankingDlg::OnCourseAreaOwnerDraw(int param) {
}

// rankingdlg.cpp:1315
void FrRankingDlg::OnRankList_Init(int param) {
	if (m_pRankList) {
		m_pRankList->ClearItem();
	} else {
		m_pRankList = DYNAMIC_CAST(FrListBox, param);
	}
}

// rankingdlg.cpp:1321
void FrRankingDlg::OnRankList_OwnerDraw(int param) {
	if (m_rankList.empty()) {
		return;
	}

	if (!param) {
		return;
	}

	SetListItemInfo((FrListItem*)param);
}

// rankingdlg.cpp:1333
void FrRankingDlg::OnRankList_LBtnDown(void) {
	if (!(m_userInfoDelay > 0.0f) && !m_anotherHand.IsWorking(0)) {
		FrListItem* pItem = m_pRankList->GetItemUnderCursor();
		if (!pItem) {
			return;
		}

		ranktype::sListInfo* pInfo = (ranktype::sListInfo*)pItem->pData;
		if (!pInfo) {
			return;
		}

		if (strcmpi(pInfo->nickname.c_str(), m_userInfo.nickname) == 0) {
			return;
		}

		SendRequestUserInfo(pInfo->uid, pInfo->nickname.c_str());
	}
}

// rankingdlg.cpp:1353
void FrRankingDlg::OnRankList_PrevPageInit(int param) {
	FrButton* pBtn = DYNAMIC_CAST(FrButton, param);
	pBtn->SetPushDelay(1.0f);
}

// rankingdlg.cpp:1359
void FrRankingDlg::OnRankList_PrevPageBtnUp(void) {
	if (s_iPageNum > 0) {
		s_iPageNum--;
		SendRequestRank(false);
	} else {
		// "There are no more ranking records."
		m_pBottomMsg->SetCaption("\xB4\xF5 \xC0\xCC\xBB\xF3 \xB7\xA9\xC5\xB7 \xB1\xE2\xB7\xCF\xC0\xCC \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9."); m_pBottomMsg->SetVisible(true);
	}
}

// rankingdlg.cpp:1372
void FrRankingDlg::OnRankList_NextPageInit(int param) {
	FrButton* pBtn = DYNAMIC_CAST(FrButton, param);
	pBtn->SetPushDelay(1.0f);
}

// rankingdlg.cpp:1378
void FrRankingDlg::OnRankList_NextPageBtnUp(void) {
	s_iPageNum++;
	SendRequestRank(false);
}

// rankingdlg.cpp:1384
void FrRankingDlg::OnRankList_CurrentPageInit(int param) {
	m_pCurPageEdit = DYNAMIC_CAST(FrEdit, param);
}

// rankingdlg.cpp:1389
void FrRankingDlg::OnRankList_TotalPageInit(int param) {
	m_pTotalPageEdit = DYNAMIC_CAST(FrEdit, param);
}

// rankingdlg.cpp:1394
void FrRankingDlg::OnRankList_MyInfo_Init(int param) {
	if (m_pMyInfoList) {
		m_pMyInfoList->ClearItem();
	} else {
		m_pMyInfoList = DYNAMIC_CAST(FrListBox, param);
	}

	m_pMyInfoList->UseRightButton(true);
}

// rankingdlg.cpp:1402
void FrRankingDlg::OnRankList_MyInfo_LBtnDown(void) {
	if (!(m_userInfoDelay > 0.0f) && !m_anotherHand.IsWorking(0)) {
		FrListItem* pItem = m_pMyInfoList->GetItemUnderCursor();
		if (!pItem) {
			return;
		}

		ranktype::sListInfo* pInfo = (ranktype::sListInfo*)pItem->pData;
		if (!pInfo) {
			return;
		}

		if (strcmpi(pInfo->nickname.c_str(), m_userInfo.nickname) == 0) {
			return;
		}

		SendRequestUserInfo(pInfo->uid, pInfo->nickname.c_str());
	}
}

// rankingdlg.cpp:1422
void FrRankingDlg::OnRankList_MyInfo_OwnerDraw(int param) {
	if (param) {
		SetListItemInfo((FrListItem*)param);
	}
}

// rankingdlg.cpp:1433
void FrRankingDlg::OnRankList_NoInRank_Init(int param) {
	m_pNoMyRankWnd = DYNAMIC_CAST(FrStatic, param);
	m_pNoMyRankWnd->SetVisible(false);
}

// rankingdlg.cpp:1439
void FrRankingDlg::OnRankList_SearchUserInit(int param) {
	m_pSearchUserBtn = DYNAMIC_CAST(FrButton, param);
	m_pSearchUserBtn->SetPushDelay(0.1f);
}

// rankingdlg.cpp:1445
void FrRankingDlg::OnRankList_SearchUserBtnUp(void) {
	char nickname[260];
	nickname[0] = 0;

	if (m_pUserIdEdit) {
		strcpy(nickname, m_pUserIdEdit->GetLine(1, false));
		TrimA(nickname, " \t\n\r");
	}

	SendRequestSearchUser(nickname);
}

// rankingdlg.cpp:1456
void FrRankingDlg::OnRankList_UserIdEditInit(int param) {
	m_pUserIdEdit = DYNAMIC_CAST(FrEdit, param);
	m_pUserIdEdit->SetCharLimit(0x20, true);
}

// rankingdlg.cpp:1462
void FrRankingDlg::OnRankList_UserIdEditDown(void) {
}

// rankingdlg.cpp:1467
void FrRankingDlg::OnEmoticonInit(intptr_t param) {
	m_pEmoticonBtn = DYNAMIC_CAST(FrButton, param);
}

// rankingdlg.cpp:1472
void FrRankingDlg::OnEmoticonBtnUp(void) {
	if (m_pEmoticonBtn && !m_pEmoticonDlg) {
		m_pEmoticonDlg = CreateForm<FrEmoticonDlg>(g_pFresh->GetManager(), this, "emoticon", 0);
		if (!m_pEmoticonDlg) {
			return;
		}

		FrButton* pBtn = m_pEmoticonBtn;
		WPoint pos(0.0f, 0.0f);
		if (pBtn) {
			pos.x = pBtn->GetRect().x - 200.0f;
			pos.y = pBtn->GetRect().y - 260.0f;
		}

		m_pEmoticonDlg->Open((FRESH_RESULT_CALLBACK)&FrRankingDlg::OnEmoticonResult, pos, 3);
	}
}

// rankingdlg.cpp:1493
void FrRankingDlg::OnLanguageInit(intptr_t param) {
	m_pLanguageArea = DYNAMIC_CAST(FrArea, param);
	if (m_pLanguageArea) {
		if (g_ime->IsAlphaNumericMode())
			m_pLanguageArea->SetBgImg("chat_eng");
		else
			m_pLanguageArea->SetBgImg("chat_kor");
	}
}

// rankingdlg.cpp:1506
void FrRankingDlg::OnAboutBtnInit(int param) {
	if ((__rtti_obj = (void*)param) != 0 ? ((IObject*)__rtti_obj)->IsKindOf(&FrButton::m_RTTI) : false) {
	}
}

// rankingdlg.cpp:1512
void FrRankingDlg::OnAboutBtnUp(void) {
	m_pAboutDlg = CreateForm<FrRankingAboutDlg>(g_pFresh->GetManager(), this, "ranking_about", 0);

	if (m_pAboutDlg) {
		m_pAboutDlg->Open((FRESH_RESULT_CALLBACK)&FrRankingDlg::OnAboutDlgResult, 3);
	}
}

// rankingdlg.cpp:1522
void FrRankingDlg::OnCloseBtnUp(void) {
	Close(true);
}

// rankingdlg.cpp:1528
void FrRankingDlg::OnBottomMessageInit(int param) {
	m_pBottomMsg = DYNAMIC_CAST(FrStatic, param);
	m_pBottomMsg->SetVisible(false);
}

// rankingdlg.cpp:1534
void FrRankingDlg::OnPetCharacterInit(int param) {
	m_pPetCharacterArea = DYNAMIC_CAST(FrArea, param);
}

// rankingdlg.cpp:1539
void FrRankingDlg::OnPetCharacterOwnerDraw(void) {
	if (m_pPetLoadGauge && m_pPetLoadGauge->IsVisible()) {
		return;
	}

	if (!m_anotherHand.IsWorking(0)) {
		m_anotherHand.Enter();
		if (m_pExhibition) {
			m_pExhibition->Display(0.0f, 0.0f);
		}
		m_anotherHand.Leave();
	}
}

// rankingdlg.cpp:1553
void FrRankingDlg::OnPetLoadInit(int param) {
	m_pPetLoadGauge = DYNAMIC_CAST(FrGaugeBar, param);
	if (m_pPetLoadGauge) {
		m_pPetLoadGauge->SetRange(0, 150, 0);
		m_pPetLoadGauge->SetVisible(false);
	}
}

// rankingdlg.cpp:1563
void FrRankingDlg::OnUpperMiniInfoInit(int param) {
	m_pUpperMiniInfo = DYNAMIC_CAST(FrArea, param);
}

// rankingdlg.cpp:1568
void FrRankingDlg::OnUpperMiniInfoOwnerDraw(void) {
	if (m_pUpperMiniInfo == 0) {
		return;
	}

	WRect rc = m_pUpperMiniInfo->GetRect();
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (m_userInfo.uid != 0xffffffff) {
		unsigned char level = m_userInfo.level + 1 < 0x47 ? m_userInfo.level + 1 : 0x47;
		const Bitmap* pImg = g_pFresh->GetManager()->GetBitmap("LEVELS", MakeStr("level_%03d", level));
		if (pImg) {
			pGDI->DrawTexture(pImg, WRect(rc.x + 5.0f, rc.y + 6.0f, (float)pImg->Width(), (float)pImg->Height()), 0xffffffff, 0);
		}

		char text[256];
		sprintf(text, "%s", m_userInfo.shownName);
		pGDI->SetTextStyle(0);

		if (!WSingleton<CProjectG>::Instance()->HidePrivacy()) {
			g_pFresh->GetManager()->PrintText(WPoint(rc.x + 60.0f, rc.y + 9.0f), 0, text, -1.0f, 0xffffffff);
		}
	}

	ShowUserInfo();
}

// rankingdlg.cpp:1595
void FrRankingDlg::OnClassType_ComboInit(int param) {
	m_pClassTypeCombo = DYNAMIC_CAST(FrComboBox, param);

	for (int i = 0; i < 4; i++) {
		m_pClassTypeCombo->AddString(s_aClassType[i].m_name);
	}

	m_pClassTypeCombo->SetLine(1, s_aClassType[s_iClassType].m_name, 0, false, 0);
	m_pClassTypeCombo->SetVisible(false);
}

// rankingdlg.cpp:1608
void FrRankingDlg::OnClassType_ComboDown(int param) {
	const char* text = m_pClassTypeCombo->GetLine(1, false);

	int prevClassType = s_iClassType;
	s_iClassType = m_pClassTypeCombo->FindString(text) - 1;
	if (s_iClassType < 0) {
		s_iClassType = 0;
	} else if (s_iClassType > 3) {
		s_iClassType = 3;
	}

	if (s_iClassType != prevClassType) {
		SendRequestRank(true);
	}
}

// rankingdlg.cpp:1622
void FrRankingDlg::OnRankTypeListInit(int param) {
	m_pRankTypeList = DYNAMIC_CAST(FrListBox, param);

	if (m_pRankTypeList) {
		for (int i = 0; i < 3; i++) {
			m_rankTypeLine[i].text = std::string(s_aRankType[i].m_name);
			m_pRankTypeList->AddItem(&m_rankTypeLine[i]);
		}

		m_pRankTypeList->SelectItem(m_pRankTypeList->FindItem(&m_rankTypeLine[0]), true);
		m_pRankTypeList->SetVisible(false);
		m_pRankTypeList->EnableScrollBar(false);
	}
}

// rankingdlg.cpp:1642
void FrRankingDlg::OnRankTypeListLDown(void) {
	FrListItem* pItem = m_pRankTypeList->GetItemUnderCursor();

	if (pItem) {
		m_pRankTypeList->SelectItem(pItem, true);
		m_pRankTypeList->SetVisible(false);

		s_iRankType = pItem->idx;
		if (s_iRankType < 0) {
			s_iRankType = 0;
		} else if (s_iRankType > 2) {
			s_iRankType = 2;
		}

		s_iSubType = 0;
		s_iPageNum = 0;
		SendRequestRank(true);
		SetVisibleByRankType();
	}
}

// rankingdlg.cpp:1662
void FrRankingDlg::OnRankTypeListOwnerDraw(int param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI) {
		FrListItem* pItem = (FrListItem*)param;
		if (pItem) {
			FrLine* pLine = (FrLine*)pItem->pData;

			WPoint pt(pItem->pos.x, pItem->pos.y);
			WOverlay::DrawBox(g_view, WRect(pt, 82.0f, 22.0f), 0, 0xAA0E4F75, 0.0f);

			if (pItem->underCursor) {
				pGDI->Box(WRect(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f, 80.0f, 20.0f), 0x80082B46, 0, 0.0f);
				pGDI->SetTextStyle(1);
			} else {
				pGDI->SetTextStyle(0);
			}

			pGDI->SetTextColor(0xffffffff, 0xffffffff);
			pGDI->Print(WPoint(pItem->pos.x + 10.0f, pItem->pos.y + 6.0f), 0, pLine->text.c_str());
		}
	}
}

// rankingdlg.cpp:1691
void FrRankingDlg::OnRankTypeViewerInit(int param) {
	m_pRankTypeViewer = DYNAMIC_CAST(FrViewer, param);

	m_pRankTypeViewerImg[0] = g_pFresh->RegisterBitmap("btn_drop01_n");
	m_pRankTypeViewerImg[1] = g_pFresh->RegisterBitmap("btn_drop01_o");
	m_pRankTypeViewerImg[2] = g_pFresh->RegisterBitmap("btn_drop01_d");
}

// rankingdlg.cpp:1700
void FrRankingDlg::OnRankTypeViewerDown(void) {
	if (m_pRankTypeList) {
		m_pRankTypeList->SetVisible(!m_pRankTypeList->IsVisible());
		g_audio->PlaySfx("ui_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
	}
}

// rankingdlg.cpp:1709
void FrRankingDlg::OnRankTypeViewerOwnerDraw(void) {
	FrWndManager* pMgr = g_pFresh->GetManager(); FrGraphicInterface* pGDI = pMgr->GetGDI();

	if (pGDI) {
		FrViewer* pViewer = m_pRankTypeViewer;
		if (pViewer) {
			int state = 0;
			if (m_pRankTypeList->IsVisible()) {
				state = 2;
			} else {
				WRect rect = pViewer->GetRect();

				if (rect.IsInRect(pMgr->GetMousePos())) {
					state = 1;
				}
			}

			pGDI->DrawTexture(m_pRankTypeViewerImg[state], pViewer->GetRect(), 0xffffffff, 0);

			WRect rc = m_pRankTypeViewer->GetRect();
			FrLine* pLine = (FrLine*)m_pRankTypeList->GetSelected()->pData;

			WOverlay::DrawBox(g_view, WRect(rc.x, rc.y, rc.w, rc.h), 0, 0x800E4F75, 0.0f);
			pGDI->SetTextStyle(1);

			pGDI->SetTextColor(0xffffffff, 0xffffffff);
			pGDI->Print(WPoint(rc.x + 4.0f, rc.y + 5.0f), 0, pLine->text.c_str());
		}
	}
}

// rankingdlg.cpp:1750
void FrRankingDlg::OnGeneralSubTypeListInit(int param) {
	m_pGenSubTypeList = DYNAMIC_CAST(FrListBox, param);

	if (m_pGenSubTypeList) {
		for (int i = 0; i < 5; i++) {
			m_genSubTypeLine[i].text = std::string(s_aGenSubType[i].m_name);
			m_pGenSubTypeList->AddItem(&m_genSubTypeLine[i]);
		}

		m_pGenSubTypeList->SelectItem(m_pGenSubTypeList->FindItem(&m_genSubTypeLine[0]), true);
		m_pGenSubTypeList->SetVisible(false);
		m_pGenSubTypeList->EnableScrollBar(false);
	}
}

// rankingdlg.cpp:1770
void FrRankingDlg::OnGeneralSubTypeListLDown(void) {
	FrListItem* pItem = m_pGenSubTypeList->GetItemUnderCursor();

	if (pItem) {
		m_pGenSubTypeList->SelectItem(pItem, true);
		m_pGenSubTypeList->SetVisible(false);

		s_iSubType = pItem->idx;
		if (s_iSubType < 0) {
			s_iSubType = 0;
		} else if (s_iSubType > 4) {
			s_iSubType = 4;
		}

		s_iPageNum = 0;
		SendRequestRank(true);
	}
}

// rankingdlg.cpp:1786
void FrRankingDlg::OnGeneralSubTypeListOwnerDraw(int param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI) {
		FrListItem* pItem = (FrListItem*)param;
		if (pItem) {
			FrLine* pLine = (FrLine*)pItem->pData;

			WPoint pt(pItem->pos.x, pItem->pos.y);
			WOverlay::DrawBox(g_view, WRect(pt, 91.0f, 22.0f), 0, 0xAA0E4F75, 0.0f);

			if (pItem->underCursor) {
				pGDI->Box(WRect(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f, 90.0f, 20.0f), 0x80082B46, 0, 0.0f);
				pGDI->SetTextStyle(1);
			} else {
				pGDI->SetTextStyle(0);
			}

			pGDI->SetTextColor(0xffffffff, 0xffffffff);
			pGDI->Print(WPoint(pItem->pos.x + 10.0f, pItem->pos.y + 6.0f), 0, pLine->text.c_str());
		}
	}
}

// rankingdlg.cpp:1815
void FrRankingDlg::OnGeneralSubTypeViewerInit(int param) {
	m_pGenSubTypeViewer = DYNAMIC_CAST(FrViewer, param);

	m_pGenSubTypeViewerImg[0] = g_pFresh->RegisterBitmap("btn_drop02_n");
	m_pGenSubTypeViewerImg[1] = g_pFresh->RegisterBitmap("btn_drop02_o");
	m_pGenSubTypeViewerImg[2] = g_pFresh->RegisterBitmap("btn_drop02_d");
}

// rankingdlg.cpp:1824
void FrRankingDlg::OnGeneralSubTypeViewerDown(void) {
	if (m_pGenSubTypeList) {
		m_pGenSubTypeList->SetVisible(!m_pGenSubTypeList->IsVisible());
		g_audio->PlaySfx("ui_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
	}
}

// rankingdlg.cpp:1833
void FrRankingDlg::OnGeneralSubTypeViewerOwnerDraw(void) {
	FrWndManager* pMgr = g_pFresh->GetManager(); FrGraphicInterface* pGDI = pMgr->GetGDI();

	if (pGDI) {
		FrViewer* pViewer = m_pGenSubTypeViewer;
		if (pViewer) {
			int state = 0;
			if (m_pGenSubTypeList->IsVisible()) {
				state = 2;
			} else {
				WRect rect = pViewer->GetRect();

				if (rect.IsInRect(pMgr->GetMousePos())) {
					state = 1;
				}
			}

			pGDI->DrawTexture(m_pGenSubTypeViewerImg[state], pViewer->GetRect(), 0xffffffff, 0);

			WRect rc = m_pGenSubTypeViewer->GetRect();
			FrLine* pLine = (FrLine*)m_pGenSubTypeList->GetSelected()->pData;

			WOverlay::DrawBox(g_view, WRect(rc.x, rc.y, rc.w, rc.h), 0, 0x800E4F75, 0.0f);
			pGDI->SetTextStyle(1);

			pGDI->SetTextColor(0xffffffff, 0xffffffff);
			pGDI->Print(WPoint(rc.x + 4.0f, rc.y + 5.0f), 0, pLine->text.c_str());
		}
	}
}

// rankingdlg.cpp:1874
void FrRankingDlg::OnCourseTypeListInit(int param) {
	m_pCourseTypeList = DYNAMIC_CAST(FrListBox, param);

	if (m_pCourseTypeList) {
		for (int i = 0; i < 17; i++) {
			IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(0x28000000 | GetRealCourseType(i));
			if (pCourse) {
				m_courseTypeLine[i].text = std::string(s_aCourseType[i].m_name);
				m_pCourseTypeList->AddItem(&m_courseTypeLine[i]);
			}
		}

		const WRect& rect = m_pCourseTypeList->GetRect();

		m_pCourseTypeList->SetRect(WRect(rect.TopLeft(), 107.0f, 374.0f));

		m_pCourseTypeList->SelectItem(m_pCourseTypeList->FindItem(&m_courseTypeLine[0]), true);
		m_pCourseTypeList->SetVisible(false);
		m_pCourseTypeList->EnableScrollBar(false);
	}
}

// rankingdlg.cpp:1905
void FrRankingDlg::OnCourseTypeListLDown(void) {
	FrListItem* pItem = m_pCourseTypeList->GetItemUnderCursor();

	if (pItem) {
		m_pCourseTypeList->SelectItem(pItem, true);
		m_pCourseTypeList->SetVisible(false);

		s_iSubType = pItem->idx;
		if (s_iSubType < 0) {
			s_iSubType = 0;
		} else if (s_iSubType > 16) {
			s_iSubType = 16;
		}

		s_iPageNum = 0;
		SendRequestRank(true);
	}
}

// rankingdlg.cpp:1921
void FrRankingDlg::OnCourseTypeListOwnerDraw(int param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI) {
		FrListItem* pItem = (FrListItem*)param;
		if (pItem) {
			FrLine* pLine = (FrLine*)pItem->pData;

			WPoint pt(pItem->pos.x, pItem->pos.y);
			WOverlay::DrawBox(g_view, WRect(pt, 91.0f, 22.0f), 0, 0xAA0E4F75, 0.0f);

			if (pItem->underCursor) {
				pGDI->Box(WRect(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f, 90.0f, 20.0f), 0x80082B46, 0, 0.0f);
				pGDI->SetTextStyle(1);
			} else {
				pGDI->SetTextStyle(0);
			}

			pGDI->SetTextColor(0xffffffff, 0xffffffff);
			pGDI->Print(WPoint(pItem->pos.x + 10.0f, pItem->pos.y + 6.0f), 0, pLine->text.c_str());
		}
	}
}

// rankingdlg.cpp:1950
void FrRankingDlg::OnCourseTypeViewerInit(int param) {
	m_pCourseTypeViewer = DYNAMIC_CAST(FrViewer, param);

	m_pCourseTypeViewerImg[0] = g_pFresh->RegisterBitmap("btn_drop02_n");
	m_pCourseTypeViewerImg[1] = g_pFresh->RegisterBitmap("btn_drop02_o");
	m_pCourseTypeViewerImg[2] = g_pFresh->RegisterBitmap("btn_drop02_d");
}

// rankingdlg.cpp:1959
void FrRankingDlg::OnCourseTypeViewerDown(void) {
	if (m_pCourseTypeList) {
		m_pCourseTypeList->SetVisible(!m_pCourseTypeList->IsVisible());
		g_audio->PlaySfx("ui_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
	}
}

// rankingdlg.cpp:1968
void FrRankingDlg::OnCourseTypeViewerOwnerDraw(void) {
	FrWndManager* pMgr = g_pFresh->GetManager(); FrGraphicInterface* pGDI = pMgr->GetGDI();

	if (pGDI) {
		FrViewer* pViewer = m_pCourseTypeViewer;
		if (pViewer) {
			int state = 0;
			if (m_pCourseTypeList->IsVisible()) {
				state = 2;
			} else {
				WRect rect = pViewer->GetRect();

				if (rect.IsInRect(pMgr->GetMousePos())) {
					state = 1;
				}
			}

			pGDI->DrawTexture(m_pCourseTypeViewerImg[state], pViewer->GetRect(), 0xffffffff, 0);

			WRect rc = m_pCourseTypeViewer->GetRect();
			FrLine* pLine = (FrLine*)m_pCourseTypeList->GetSelected()->pData;

			WOverlay::DrawBox(g_view, WRect(rc.x, rc.y, rc.w, rc.h), 0, 0x800E4F75, 0.0f);
			pGDI->SetTextStyle(1);

			pGDI->SetTextColor(0xffffffff, 0xffffffff);
			pGDI->Print(WPoint(rc.x + 4.0f, rc.y + 5.0f), 0, pLine->text.c_str());
		}
	}
}

// rankingdlg.cpp:2010
void FrRankingDlg::OnRecordTypeListInit(int param) {
	m_pRecordTypeList = DYNAMIC_CAST(FrListBox, param);

	if (m_pRecordTypeList) {
		for (int i = 0; i < 5; i++) {
			m_recordTypeLine[i].text = std::string(s_aRecordType[i].m_name);
			m_pRecordTypeList->AddItem(&m_recordTypeLine[i]);
		}

		m_pRecordTypeList->SelectItem(m_pRecordTypeList->FindItem(&m_recordTypeLine[0]), true);
		m_pRecordTypeList->SetVisible(false);
		m_pRecordTypeList->EnableScrollBar(false);
	}
}

// rankingdlg.cpp:2034
void FrRankingDlg::OnRecordTypeListLDown(void) {
	FrListItem* pItem = m_pRecordTypeList->GetItemUnderCursor();

	if (pItem) {
		m_pRecordTypeList->SelectItem(pItem, true);
		m_pRecordTypeList->SetVisible(false);

		s_iSubType = pItem->idx;
		if (s_iSubType < 0) {
			s_iSubType = 0;
		} else if (s_iSubType > 4) {
			s_iSubType = 4;
		}

		s_iPageNum = 0;
		SendRequestRank(true);
	}
}

// rankingdlg.cpp:2058
void FrRankingDlg::OnRecordTypeListOwnerDraw(int param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI) {
		FrListItem* pItem = (FrListItem*)param;
		if (pItem) {
			FrLine* pLine = (FrLine*)pItem->pData;

			WPoint pt(pItem->pos.x, pItem->pos.y);
			WOverlay::DrawBox(g_view, WRect(pt, 91.0f, 22.0f), 0, 0xAA0E4F75, 0.0f);

			if (pItem->underCursor) {
				pGDI->Box(WRect(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f, 90.0f, 20.0f), 0x80082B46, 0, 0.0f);
				pGDI->SetTextStyle(1);
			} else {
				pGDI->SetTextStyle(0);
			}

			pGDI->SetTextColor(0xffffffff, 0xffffffff);
			pGDI->Print(WPoint(pItem->pos.x + 10.0f, pItem->pos.y + 6.0f), 0, pLine->text.c_str());
		}
	}
}

// rankingdlg.cpp:2087
void FrRankingDlg::OnRecordTypeViewerInit(int param) {
	m_pRecordTypeViewer = DYNAMIC_CAST(FrViewer, param);

	m_pRecordTypeViewerImg[0] = g_pFresh->RegisterBitmap("btn_drop02_n");
	m_pRecordTypeViewerImg[1] = g_pFresh->RegisterBitmap("btn_drop02_o");
	m_pRecordTypeViewerImg[2] = g_pFresh->RegisterBitmap("btn_drop02_d");
}

// rankingdlg.cpp:2096
void FrRankingDlg::OnRecordTypeViewerDown(void) {
	if (m_pRecordTypeList) {
		m_pRecordTypeList->SetVisible(!m_pRecordTypeList->IsVisible());
		g_audio->PlaySfx("ui_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
	}
}

// rankingdlg.cpp:2105
void FrRankingDlg::OnRecordTypeViewerOwnerDraw(void) {
	FrWndManager* pMgr = g_pFresh->GetManager(); FrGraphicInterface* pGDI = pMgr->GetGDI();

	if (pGDI) {
		FrViewer* pViewer = m_pRecordTypeViewer;
		if (pViewer) {
			int state = 0;
			if (m_pRecordTypeList->IsVisible()) {
				state = 2;
			} else {
				WRect rect = pViewer->GetRect();

				if (rect.IsInRect(pMgr->GetMousePos())) {
					state = 1;
				}
			}

			pGDI->DrawTexture(m_pRecordTypeViewerImg[state], pViewer->GetRect(), 0xffffffff, 0);

			WRect rc = m_pRecordTypeViewer->GetRect();
			FrLine* pLine = (FrLine*)m_pRecordTypeList->GetSelected()->pData;

			WOverlay::DrawBox(g_view, WRect(rc.x, rc.y, rc.w, rc.h), 0, 0x800E4F75, 0.0f);
			pGDI->SetTextStyle(1);

			pGDI->SetTextColor(0xffffffff, 0xffffffff);
			pGDI->Print(WPoint(rc.x + 6.0f, rc.y + 5.0f), 0, pLine->text.c_str());
		}
	}
}

// rankingdlg.cpp:2146
void FrRankingDlg::OnSearchTypeListInit(int param) {
	m_pSearchTypeList = DYNAMIC_CAST(FrListBox, param);

	if (m_pSearchTypeList) {
		for (int i = 0; i < 2; i++) {
			m_searchTypeLine[i].text = std::string(s_aSearchType[i].m_name);
			m_pSearchTypeList->AddItem(&m_searchTypeLine[i]);
		}

		m_pSearchTypeList->SelectItem(m_pSearchTypeList->FindItem(&m_searchTypeLine[0]), true);
		m_pSearchTypeList->SetVisible(false);
		m_pSearchTypeList->EnableScrollBar(false);
	}
}

// rankingdlg.cpp:2166
void FrRankingDlg::OnSearchTypeListLDown(void) {
	FrListItem* pItem = m_pSearchTypeList->GetItemUnderCursor();

	if (pItem) {
		m_pSearchTypeList->SelectItem(pItem, true);
		m_pSearchTypeList->SetVisible(false);

		s_iSearchType = pItem->idx;
	}
}

// rankingdlg.cpp:2177
void FrRankingDlg::OnSearchTypeListOwnerDraw(int param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI) {
		FrListItem* pItem = (FrListItem*)param;
		if (pItem) {
			FrLine* pLine = (FrLine*)pItem->pData;

			WPoint pt(pItem->pos.x, pItem->pos.y);
			WOverlay::DrawBox(g_view, WRect(pt, 62.0f, 22.0f), 0, 0xAA0E4F75, 0.0f);

			if (pItem->underCursor) {
				pGDI->Box(WRect(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f, 62.0f, 20.0f), 0x80082B46, 0, 0.0f);
				pGDI->SetTextStyle(1);
			} else {
				pGDI->SetTextStyle(0);
			}

			pGDI->SetTextColor(0xffffffff, 0xffffffff);
			pGDI->Print(WPoint(pItem->pos.x + 10.0f, pItem->pos.y + 6.0f), 0, pLine->text.c_str());
		}
	}
}

// rankingdlg.cpp:2206
void FrRankingDlg::OnSearchTypeViewerInit(int param) {
	m_pSearchTypeViewer = DYNAMIC_CAST(FrViewer, param);

	m_pSearchTypeViewerImg[0] = g_pFresh->RegisterBitmap("btn_drop03_n");
	m_pSearchTypeViewerImg[1] = g_pFresh->RegisterBitmap("btn_drop03_o");
	m_pSearchTypeViewerImg[2] = g_pFresh->RegisterBitmap("btn_drop03_d");
}

// rankingdlg.cpp:2215
void FrRankingDlg::OnSearchTypeViewerDown(void) {
	if (m_pSearchTypeList) {
		m_pSearchTypeList->SetVisible(!m_pSearchTypeList->IsVisible());
		g_audio->PlaySfx("ui_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
	}
}

// rankingdlg.cpp:2224
void FrRankingDlg::OnSearchTypeViewerOwnerDraw(void) {
	FrWndManager* pMgr = g_pFresh->GetManager(); FrGraphicInterface* pGDI = pMgr->GetGDI();

	if (pGDI) {
		FrViewer* pViewer = m_pSearchTypeViewer;
		if (pViewer) {
			int state = 0;
			if (m_pSearchTypeList->IsVisible()) {
				state = 2;
			} else {
				WRect rect = pViewer->GetRect();

				if (rect.IsInRect(pMgr->GetMousePos())) {
					state = 1;
				}
			}

			pGDI->DrawTexture(m_pSearchTypeViewerImg[state], pViewer->GetRect(), 0xffffffff, 0);

			WRect rc = m_pSearchTypeViewer->GetRect();
			FrLine* pLine = (FrLine*)m_pSearchTypeList->GetSelected()->pData;

			WOverlay::DrawBox(g_view, WRect(rc.x, rc.y, rc.w, rc.h), 0, 0x800E4F75, 0.0f);
			pGDI->SetTextStyle(1);

			pGDI->SetTextColor(0xffffffff, 0xffffffff);
			pGDI->Print(WPoint(rc.x + 4.0f, rc.y + 5.0f), 0, pLine->text.c_str());
		}
	}
}

// rankingdlg.cpp:2264
void FrRankingDlg::OnUserRankEditInit(int param) {
	m_pUserRankEdit = DYNAMIC_CAST(FrEdit, param);
	m_pUserRankEdit->SetVisible(false);
}

// rankingdlg.cpp:2270
void FrRankingDlg::OnNotify(const char* msg, unsigned long param) {
	FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
	pForm->SetTimeLimit(1.0f);
	pForm->SetMessage(msg, false);
	pForm->Open((FRESH_RESULT_CALLBACK)0, param);

	m_pBottomMsg->SetVisible(false);
}

// rankingdlg.cpp:2280
void FrRankingDlg::SetBottomMsg(const char* msg) {
	m_pBottomMsg->SetCaption(msg);
	m_pBottomMsg->SetVisible(true);
}

// rankingdlg.cpp:2286
void FrRankingDlg::OnCustomProc(float dt) {
	if (m_anotherHand.IsWorking(0)) {
		if (m_pGhostBtn && m_pGhostBtn->IsVisible()) {
			m_pGhostBtn->SetVisible(false);
		}

		Sleep(10);
		return;
	}

	m_anotherHand.Enter();

	if (m_pExhibition && m_pPetLoadGauge && !m_pPetLoadGauge->IsVisible()) {
		WRect rect = m_pPetCharacterArea->GetRect();
		m_pExhibition->SetCenter(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f);
		m_pExhibition->Process(dt, true);
	}

	m_anotherHand.Leave();
}

// rankingdlg.cpp:2331
bool FrRankingDlg::OnEmoticonResult(int ret, FrForm* pForm) {
	if (m_pEmoticonDlg && ret == 1) {
		FrEmoticonDlg* pDlg = DYNAMIC_CAST(FrEmoticonDlg, pForm);
		if (pDlg) {
			const char* icon = pDlg->GetSelectedIcon();
			if (icon) {
				FrEdit* pEdit = m_pUserIdEdit;

				if (pEdit) {
					const char* text = pEdit->GetEditText_Front();
					pEdit->SetLine(1, MakeStr("%s%s", text, icon), 0, false, 0);
					pEdit->SetKeyFocus(true);
				}
			}
		}
	}

	m_pEmoticonDlg = 0;
	return true;
}

// rankingdlg.cpp:2357
bool FrRankingDlg::OnAboutDlgResult(int ret, FrForm* pForm) {
	m_pAboutDlg = 0;
	return true;
}

// rankingdlg.cpp:2363
void FrRankingDlg::OnGhostBtnInit(int param) {
	m_pGhostBtn = DYNAMIC_CAST(FrButton, param);
	m_pGhostBtn->SetVisible(false);
	m_pGhostBtn->Enable(false);
}

// rankingdlg.cpp:2370
void FrRankingDlg::OnGhostBtnUp(void) {
	FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), 0, "notify", 0);
	pForm->SetTimeLimit(5.0f);
	// "The service is being prepared."
	pForm->SetMessage("\xBC\xAD\xBA\xF1\xBD\xBA \xC1\xD8\xBA\xF1 \xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9.", false);
	pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
}

// rankingdlg.cpp:2396
CRankingInfo::CRankingInfo(void) {
	m_pRankingDlg = 0;
}

// rankingdlg.cpp:2401
bool CRankingInfo::Open(void) {
	if (!m_pRankingDlg) {
		m_pRankingDlg = CreateForm<FrRankingDlg>(g_pFresh->GetManager(), this, "rankingdlg", 0);
		m_pRankingDlg->Open((FRESH_RESULT_CALLBACK)&CRankingInfo::OnRankingDlgResult, 1);
		m_pRankingDlg->EnableDrag(false);

		if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject*)__rtti_obj)->IsExactKindOf(&ntAvatarChatTask::m_RTTI) : false) {
			ntAvatarManager::Instance()->SetActive(false);
		}
	}

	return true;
}

// rankingdlg.cpp:2415
void CRankingInfo::Close(void) {
	if (m_pRankingDlg) {
		m_pRankingDlg->Close(true);
	}
}

// rankingdlg.cpp:2422
bool CRankingInfo::OnRankingDlgResult(int ret, FrForm* pForm) {
	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_RANK)) WNetworkSystem::Instance()->ForceShutDown(WNetworkSystem::NET_UNIT_RANK);
	m_pRankingDlg = 0;

	if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject*)__rtti_obj)->IsExactKindOf(&ntAvatarChatTask::m_RTTI) : false) {
		ntAvatarManager::Instance()->SetActive(true);
	}
	return true;
}

// rankingdlg.cpp:2432
void CRankingInfo::Process(float dt) {
	if (m_pRankingDlg) {
		m_pRankingDlg->OnCustomProc(dt);
	}
}

// rankingdlg.cpp:2438
void CRankingInfo::OnNotify(const char* msg, unsigned long param) {
	if (m_pRankingDlg) {
		m_pRankingDlg->OnNotify(msg, param);
	} else {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		pForm->SetTimeLimit(3.0f);
		pForm->SetMessage(msg, false);
		pForm->Open((FRESH_RESULT_CALLBACK)0, param);
	}
}

// rankingdlg.cpp:2452 (run-time type and factory registration of FrRankingAboutDlg)
const WRTTI FrRankingAboutDlg::m_RTTI("FrRankingAboutDlg", &FrForm::m_RTTI);

IObject* FrRankingAboutDlgMakeInstance(void) {
	return new FrRankingAboutDlg;
}

struct __sFrRankingAboutDlg {
	__sFrRankingAboutDlg(void) {
		ObjectFactory().AddObjectFunctor(FrRankingAboutDlgMakeInstance, "FrRankingAboutDlg");
	}
};
static __sFrRankingAboutDlg s_registerFrRankingAboutDlg;

// rankingdlg.cpp:2454
const sFRESH_MSGMAP* FrRankingAboutDlg::GetMessageMap(void) const {
	return &FrRankingAboutDlg::_MsgMap;
}

sFRESH_MSGMAP FrRankingAboutDlg::_MsgMap = { &FrForm::_MsgMap, &FrRankingAboutDlg::_MsgEntries[0] };

sFRESH_ENTRY FrRankingAboutDlg::_MsgEntries[] = {
	// rankingdlg.cpp:2456
	{ "aboutviewer", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingAboutDlg::OnAboutViewerInit },
	// rankingdlg.cpp:2457
	{ "index01", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingAboutDlg::OnIndex01Init },
	// rankingdlg.cpp:2458
	{ "index01", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRankingAboutDlg::OnIndex01BnUp },
	// rankingdlg.cpp:2459
	{ "index02", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingAboutDlg::OnIndex02Init },
	// rankingdlg.cpp:2460
	{ "index02", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRankingAboutDlg::OnIndex02BnUp },
	// rankingdlg.cpp:2461
	{ "index03", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingAboutDlg::OnIndex03Init },
	// rankingdlg.cpp:2462
	{ "index03", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRankingAboutDlg::OnIndex03BnUp },
	// rankingdlg.cpp:2463
	{ "index04", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingAboutDlg::OnIndex04Init },
	// rankingdlg.cpp:2464
	{ "index04", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRankingAboutDlg::OnIndex04BnUp },
	// rankingdlg.cpp:2465
	{ "backward", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingAboutDlg::OnBackwardBnInit },
	// rankingdlg.cpp:2466
	{ "backward", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRankingAboutDlg::OnBackwardBnUp },
	// rankingdlg.cpp:2467
	{ "forward", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingAboutDlg::OnForwardBnInit },
	// rankingdlg.cpp:2468
	{ "forward", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRankingAboutDlg::OnForwardBnUp },
	// rankingdlg.cpp:2469
	{ "ok", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankingAboutDlg::OnOkBnInit },
	// rankingdlg.cpp:2470
	{ "ok", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRankingAboutDlg::OnOkBnUp },
	// rankingdlg.cpp:2472
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// rankingdlg.cpp:2475
FrRankingAboutDlg::FrRankingAboutDlg(void) {
	for (int i = 0; i < 4; i++) {
		m_pIndexBtn[i] = 0;
	}
	m_aboutIndex = 0;
	m_pBackwardBtn = 0;
	m_pForwardBtn = 0;
}

// rankingdlg.cpp:2489
FrRankingAboutDlg::~FrRankingAboutDlg(void) {
}

// rankingdlg.cpp:2493
void FrRankingAboutDlg::OnAboutViewerInit(intptr_t param) {
	m_pAboutViewer = DYNAMIC_CAST(FrViewer, param);
}

// rankingdlg.cpp:2499
void FrRankingAboutDlg::OnIndex01Init(intptr_t param) {
	m_pIndexBtn[0] = DYNAMIC_CAST(FrTextButton, param);
}

// rankingdlg.cpp:2504
void FrRankingAboutDlg::OnIndex01BnUp(void) {
	SetImgAboutIndex(0);
}

// rankingdlg.cpp:2509
void FrRankingAboutDlg::OnIndex02Init(intptr_t param) {
	m_pIndexBtn[1] = DYNAMIC_CAST(FrTextButton, param);
}

// rankingdlg.cpp:2514
void FrRankingAboutDlg::OnIndex02BnUp(void) {
	SetImgAboutIndex(1);
}

// rankingdlg.cpp:2519
void FrRankingAboutDlg::OnIndex03Init(intptr_t param) {
	m_pIndexBtn[2] = DYNAMIC_CAST(FrTextButton, param);
}

// rankingdlg.cpp:2524
void FrRankingAboutDlg::OnIndex03BnUp(void) {
	SetImgAboutIndex(2);
}

// rankingdlg.cpp:2529
void FrRankingAboutDlg::OnIndex04Init(intptr_t param) {
	m_pIndexBtn[3] = DYNAMIC_CAST(FrTextButton, param);
}

// rankingdlg.cpp:2534
void FrRankingAboutDlg::OnIndex04BnUp(void) {
	SetImgAboutIndex(3);
}

// rankingdlg.cpp:2539
void FrRankingAboutDlg::OnBackwardBnInit(intptr_t param) {
	m_pBackwardBtn = DYNAMIC_CAST(FrButton, param);
}

// rankingdlg.cpp:2544
void FrRankingAboutDlg::OnBackwardBnUp(void) {
	SetImgAboutIndex(--m_aboutIndex);
	m_pBackwardBtn->SetPushDelay(0.0f);
}

// rankingdlg.cpp:2550
void FrRankingAboutDlg::OnForwardBnInit(intptr_t param) {
	m_pForwardBtn = DYNAMIC_CAST(FrButton, param);
}

// rankingdlg.cpp:2555
void FrRankingAboutDlg::OnForwardBnUp(void) {
	SetImgAboutIndex(++m_aboutIndex);
	m_pForwardBtn->SetPushDelay(0.0f);
}

// rankingdlg.cpp:2561
void FrRankingAboutDlg::OnOkBnInit(intptr_t param) {
	if ((__rtti_obj = (void*)param) != 0 ? ((IObject*)__rtti_obj)->IsKindOf(&FrButton::m_RTTI) : false) {
	}
}

// rankingdlg.cpp:2566
void FrRankingAboutDlg::OnOkBnUp(void) {
	OnFreshOkay();
}

// rankingdlg.cpp:2571
bool FrRankingAboutDlg::OnInit(void) {
	SetImgAboutIndex(0);
	return true;
}

// rankingdlg.cpp:2579
void FrRankingAboutDlg::SetImgAboutIndex(int index) {
	for (int i = 0; i < 4; i++) {
		m_pIndexBtn[i]->Unselect(0);
	}

	if (m_pBackwardBtn) {
		m_pBackwardBtn->Enable(true);
	}

	if (m_pForwardBtn) {
		m_pForwardBtn->Enable(true);
	}

	if (index <= 0) {
		index = 0;
		m_pIndexBtn[0]->Select(0);

		if (m_pBackwardBtn) {
			m_pBackwardBtn->Enable(false);
		}
	} else if (index >= 3) {
		index = 3;
		m_pIndexBtn[3]->Select(0);

		if (m_pForwardBtn) {
			m_pForwardBtn->Enable(false);
		}
	}

	m_pIndexBtn[index]->Select(0);

	m_aboutIndex = index;
	if (m_pAboutViewer) {
		m_pAboutViewer->Open(MakeStr("help_ranking_%02d.jpg", index + 1));
	}
}
