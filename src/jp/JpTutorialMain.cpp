// JP-only game code: the JP CTutorialMain (JpTutorialMain.h), the
// "tutorialmain" page form, from the JP exe's bodies (node scripts/disasm.ts
// --jp 0x<va>, jp_hlil.ts).
#include "JpTutorialMain.h"

#include <list>

#include "JpTutorialWidget.h"
#include "RFTextManager.h"

#include "TaskManager.h"
#include "actor.h"
#include "exhibition.h"
#include "fresh.h"
#include "frform.h"
#include "frwndmanager.h"
#include "mousecursor.h"
#include "QuestInfoDlg.h"
#include "SharedDoc.h"
#include "icontentsdoc.h"

// lib: 0x7d4490, 0x7d44a0, 0x7d2070 (the +0x4 / +0x8 base thunks)

// The clear gift of each mission (0xd28c60, 14 rows of 0xc bytes; the JP
// rewards differ from the KR data_9f7be8 of TutorialMain.cpp, the masks do
// not). IsCompleteQuest reads the masks; the gift dialog (OpenQuestGift,
// not written) the rest.
struct sJpQuestReward {
	unsigned long m_mask;   // 0x0, the mission's bit in CSharedDoc::m_tutorialData[category]
	unsigned long m_item;   // 0x4, the gift's item type id
	unsigned long m_count;  // 0x8
};
static const sJpQuestReward s_questRewards[14] = {
	{ 0x1, 0x1A000002, 10 }, { 0x2, 0x1800000B, 3 }, { 0x4, 0x18000025, 3 },
	{ 0x8, 0x18000005, 3 }, { 0x10, 0x18000004, 3 }, { 0x20, 0x1800000A, 3 },
	{ 0x40, 0x18000000, 3 }, { 0x80, 0x1A000010, 1000 }, { 0x100, 0x1A000002, 10 },
	{ 0x200, 0x18000028, 1 }, { 0x400, 0x18000006, 1 }, { 0x800, 0x1800000A, 3 },
	{ 0x1000, 0x18000000, 4 }, { 0x2000, 0x18000001, 4 },
};

// lib: 0xcd7ab0 (this table's dynamic initializer: each caption through
// Jp_LookupText 0xb4eb80, the KR TutorialMain.cpp captions as keys;
// m_pending and m_status 0, m_enabled true)
JpTutorialMain::sTutorialMap JpTutorialMain::m_Tutorials[14] = {
	{ Jp_LookupText("\xc5\xac\xb7\xb4\xbc\xb1\xc5\xc3"), 0, 1, 0, 0, 0, 0, 0, true },
	{ Jp_LookupText("\xc5\xb8\xb1\xb8\xb9\xe6\xb9\xfd"), 0, 2, 0, 1, 1, 0, 0, true },
	{ Jp_LookupText("\xc6\xce\xbe\xdf\xb0\xd4\xc0\xcc\xc1\xf6"), 0, 3, 0, 2, 2, 0, 0, true },
	{ Jp_LookupText("\xc6\xdb\xc6\xc3\xc0\xc7 \xb1\xe2\xba\xbb"), 0, 4, 0, 2, 3, 0, 0, true },
	{ Jp_LookupText("\xbe\xc6\xc0\xcc\xc5\xdb\xbb\xe7\xbf\xeb"), 0, 5, 0, 3, 4, 0, 0, true },
	{ Jp_LookupText("\xc1\xf6\xb8\xe9\xbc\xd3\xbc\xba"), 0, 6, 0, 3, 5, 0, 0, true },
	{ Jp_LookupText("\xbd\xba\xc4\xda\xbe\xee\xb0\xe8\xbb\xea"), 0, 7, 0, 3, 6, 0, 0, true },
	{ Jp_LookupText("\xc8\xa6\xc0\xce\xc7\xcf\xb1\xe2"), 0, 8, 0, 4, 7, 0, 0, true },
	{ Jp_LookupText("\xb0\xf8\xbf\xc5\xb1\xe2\xb1\xe2"), 1, 9, 0, 0, 8, 0, 0, true },
	{ Jp_LookupText("\xb0\xf8\xc0\xc7 \xb1\xe2\xbf\xef\xb1\xe2"), 1, 10, 0, 1, 9, 0, 0, true },
	{ Jp_LookupText("\xb9\xd9\xb6\xf7 \xc0\xd0\xb1\xe2"), 1, 11, 0, 1, 10, 0, 0, true },
	{ Jp_LookupText("\xc5\xb8\xc1\xa1\xc0\xcc\xb5\xbf"), 1, 12, 0, 2, 11, 0, 0, true },
	{ Jp_LookupText("\xc6\xc4\xbf\xf6 \xbd\xba\xc7\xc9"), 1, 13, 0, 3, 12, 0, 0, true },
	{ Jp_LookupText("\xc6\xc4\xbf\xf6 \xc4\xbf\xba\xea"), 1, 14, 0, 3, 13, 0, 0, true },
};

// The texts of OpenQuest and StartSelectedTutorial: Jp_LookupText keys,
// the KR TutorialMain.cpp literals.
// 0xd28f14: "You can do it after finishing the previous grade."
static const char s_textPreviousGrade[] =
	"\xC0\xCC\xC0\xFC \xB5\xEE\xB1\xDE\xC0\xBB \xB8\xB6\xC4\xA3 \xC8\xC4 \xC7\xCF\xBD\xC7 \xBC\xF6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.";
// 0xd28eb8: "This quest is already complete, so the clear gift cannot be
// received. Play it again anyway?"
static const char s_textAlreadyComplete[] =
	"\xC0\xCC\xB9\xCC \xBF\xCF\xB7\xE1\xB5\xC8 \xC4\xF9\xBD\xBA\xC6\xAE\xC0\xCC\xB9\xC7\xB7\xCE, \xC5\xAC\xB8\xAE\xBE\xEE \xB1\xE2\xB3\xE4 \xBC\xB1\xB9\xB0\xC0\xBB \xB9\xDE\xC0\xBB \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9. \n\xB1\xD7\xB7\xA1\xB5\xB5 \xB4\xD9\xBD\xC3 \xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?";
// 0xd28e68: "The practice mode setting is wrong."
static const char s_textInvalidSetting[] =
	"\xBF\xAC\xBD\xC0\xB8\xF0\xB5\xE5 \xBC\xB3\xC1\xA4\xC0\xCC \xC0\xDF\xB8\xF8\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.";

// lib: (m_RTTI's dynamic initializer, compiler-generated from this
// definition: WRTTI::WRTTI 0xa4c140, base FrTaskForm)
const WRTTI JpTutorialMain::m_RTTI("CTutorialMain", &FrTaskForm::m_RTTI);

// 0x7d20c0
void JpTutorialMain::EnableAllBtn(void) {
	for (int i = 0; i < 14; i++) {
		m_Tutorials[i].m_enabled = true;
	}
}

// 0x7d20e0
void JpTutorialMain::HidePet(void) {
	if (m_pet) {
		delete m_pet;
		m_pet = NULL;
	}
}

// 0x7d2300
void JpTutorialMain::SetQuestInformation(int category, int index) {
	m_container->m_questCategory = category;
	m_container->m_questNextIndex = index + 1;
	m_container->m_questIndex = index;
}

// 0x7d2490
bool JpTutorialMain::Start(void) {
	m_container = NULL;
	CTutorialContainer* container = static_cast<CTutorialContainer*>(
		CContentsDoc::Instance()->GetContainer((localContentType_t)0xa4));
	if (container) {
		m_container = container;
	}
	LoadTutorialMainImages();
	return true;
}

// 0x7d3aa0
bool JpTutorialMain::IsCompleteQuest(int category, int index) {
	bool complete = false;
	if (m_Tutorials[index].m_category == category
		&& (WSingleton<CSharedDoc>::Instance()->m_tutorialData[category] & s_questRewards[index].m_mask)) {
		complete = true;
	}
	return complete;
}

// 0x7d4030
bool JpTutorialMain::Shutdown(void) {
	m_pipinTime.Reset();
	m_tutorialList.clear();
	FrTaskForm::Shutdown();
	return true;
}

// 0x7d40a0
bool JpTutorialMain::HandleMsg(const MsgObject& msg) {
	if (msg.m_msg == 0x1b9) {
		*(const char**)msg.m_param1 = "CTutorial";
		*(const char**)msg.m_param2 = "Tutorial";
		return true;
	}
	if (msg.m_msg != 0x1bc) {
		return false;
	}
	unsigned char openGift = (unsigned char)msg.m_param2;
	SetCompleteStates();
	CheckNodeStates();
	EnableAllBtn();
	DrawCourseArrow();
	if (openGift) {
		OpenQuestGift();
	}
	return true;
}

// 0x7d4110 (the KR InitGolfTutorial on the JP CSharedDoc: +0x10d5c the KR
// m_value4D98, +0x1048c m_gameMode, +0x10594 m_roomType (SetGameType
// inlined; the JP table has the game type 0x10 entry, the KR one stops at
// 15 and SetGameType then copies no room name), +0x1051e m_holeOrder,
// +0x1069c m_shotTimeLimit, +0x10699 m_holes, +0x1069a m_weather,
// +0x109fc m_roomPlayerList, +0x10488 m_gameOver).
void JpTutorialMain::InitGolfTutorial(void) {
	CTutorialWidget::SetTutorialQuest(m_container->m_questNextIndex);
	WSingleton<CSharedDoc>::Instance()->m_gameMode = 0;

	SetCurMap(0xFD);
	SetGameType(0x10);

	for (unsigned char i = 0; i < 18; i++) {
		WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] = 0;
	}

	SetShotTimeLimit(0);

	WSingleton<CSharedDoc>::Instance()->m_holes = 1;
	WSingleton<CSharedDoc>::Instance()->m_holeOrder[0] =
		(int)CTutorialWidget::GetTutorialQuest() < 14 ? 1 : 2;

	SetWeather(0);

	for (std::list<sSlotInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.begin();
		it != WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.end(); ++it) {
		it->m_attribute &= ~4;
	}

	WSingleton<CSharedDoc>::Instance()->m_gameOver = false;
}

// 0x7d4750: a local round, no room and no packet (the round's end is
// CTutorial::MoveNextStep, tutorial.cpp under JP_PAKS).
void JpTutorialMain::StartSelectedTutorial(void) {
	if (!m_container->IsQuestSet()) {
		CTask* pTask = AfxGetTask();
		pTask->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText(s_textInvalidSetting), 0, 0, 0, 0));
		return;
	}

	if (m_pet) {
		HidePet();
	}

	InitGolfTutorial();
	WSingleton<CMouseCursor>::Instance()->SetMode(1);
	WSingleton<CMouseCursor>::Instance()->SetActive(false);

	CTaskManager::Instance()->ChangeTask("CGolfTask", "CGolfDoc", false);
	// FrWnd slot 2: FrTaskForm's Close 0x523730.
	Close(false);
}

// 0x7d5040
bool JpTutorialMain::OnTutorialInfoDlgResult(int result, FrForm* form) {
	if (result == 0) {
		return true;
	}
	m_tutorialInfoDlg = NULL;
	if (result == 1) {
		StartSelectedTutorial();
	}
	if (m_pendingTutorial != 0xFF) {
		m_Tutorials[m_pendingTutorial].m_pending = 0;
		m_pendingTutorial = 0xFF;
	}
	return true;
}

// 0x7d5160 (__fastcall in the JP: CreateForm<CTutorialMain> 0x7d5090,
// which also keeps the form in the thread-local +0x124 slot)
FrWnd* JpTutorialMain::Create(const char* name, FrWndManager* pManager) {
	return CreateForm<JpTutorialMain>(pManager, NULL, name, NULL);
}

// 0x7d5200
void JpTutorialMain::OpenTutorialInfoDlg(void) {
	if (m_tutorialInfoDlg == NULL) {
		m_tutorialInfoDlg = CreateForm<FrQuestInfoDlg>(g_pFresh->GetManager(), this, "quest_info", 0);
		m_tutorialInfoDlg->SetQuestExplane(m_container->m_questIndex);
		m_tutorialInfoDlg->Open((FRESH_RESULT_CALLBACK)&JpTutorialMain::OnTutorialInfoDlgResult, 3);
	}
}

// 0x7d5280
bool JpTutorialMain::OnQuestionFormResult(int result, FrForm* form) {
	if (result == 0) {
		return true;
	}
	m_questionForm = NULL;
	if (result == 1) {
		OpenTutorialInfoDlg();
	}
	if (m_pendingTutorial != 0xFF) {
		m_Tutorials[m_pendingTutorial].m_pending = 0;
		m_pendingTutorial = 0xFF;
	}
	return true;
}

// 0x7d52d0 (the KR OpenQuest: Open(cb, flag) without the KR's position,
// the texts through Jp_LookupText, the state in the container)
void JpTutorialMain::OpenQuest(int category, int index) {
	if (!m_Tutorials[index].m_enabled) {
		return;
	}

	if (m_Tutorials[index].m_status == 2) {
		m_questionForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		m_questionForm->SetMessage(Jp_LookupText(s_textPreviousGrade), false);
		m_questionForm->Open((FRESH_RESULT_CALLBACK)0, 3);
		m_questionForm->EnableDrag(false);
		m_Tutorials[index].m_pending = 0;
		m_pendingTutorial = 0xFF;
		return;
	}

	SetQuestInformation(category, index);
	if (IsCompleteQuest(category, index)) {
		m_container->m_questReward = 0;
		m_questionForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);
		m_questionForm->SetMessage(Jp_LookupText(s_textAlreadyComplete), false);
		m_questionForm->EnableDrag(false);
		m_questionForm->Open((FRESH_RESULT_CALLBACK)&JpTutorialMain::OnQuestionFormResult, 3);
		m_questionForm->MoveCursor("ok");
	} else {
		m_container->m_questReward = 1;
		OpenTutorialInfoDlg();
	}
}
