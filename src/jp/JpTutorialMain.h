// JP-only game code: the JP CTutorialMain (RTTI 0xe7b3c8 "CTutorialMain",
// base FrTaskForm 0xe74f88), the "tutorialmain" page form that
// CTutorialWidget (JpTutorialWidget.h) loads through CTaskWidget::Load. The
// class is named JpTutorialMain in the port: the KR CTutorialMain
// (yapang/src/client/ProjectG/TutorialMain.h) is a CTaskMain task, linked
// into the same port, and the JP class keeps only its logic (the same
// course table, buttons and quests), its state moved to the contents
// container 0xa4 (CTutorialContainer, JpTutorialWidget.h).
//
// What the JP class is (node scripts/disasm.ts --jp, jp_hlil.ts):
// - creator 0x7d5160 = CreateForm<CTutorialMain> 0x7d5090 (`new` 0x460,
//   constructor 0x7d4820, FrForm::_Init, the thread-local +0x124 slot,
//   DynamicCast to 0xe7b3c8), registered as "tutorialmain" by the static
//   init 0xcd7dc0 (the bool 0xe7b3d1);
// - vtables 0xd28dbc (FrForm, 41 slots: [0] GetRTTI 0x7d4480, [1] the
//   deleting destructor 0x7d4b70, [6] 0x7d2460, [9] OnProc 0x7d4e80,
//   [32] OnInit 0x7d4ba0, [38] Start 0x7d2490, [39] Shutdown 0x7d4030,
//   [40] HandleMsg 0x7d40a0, the rest FrTaskForm's / FrForm's), 0xd28dac
//   (+0x4: 0x7d4490, 0x7d2070), 0xd28da4 (+0x8: 0x7d44a0);
// - its range 0x7d2000-0x7d5530 (~45 functions); m_Tutorials is the
//   static table 0xe7b3d8 (initialized by code, 0xcd7ab0), the clear gift
//   table 0xd28c60 (s_questRewards), the images table 0xd28a54.
//
// Not written yet (declared below with their VAs, or not declared): the
// constructor 0x7d4820 (845 bytes: the m_questInfo vector array, the list
// head, the button positions and column floats), the destructor 0x7d4b70,
// OnInit 0x7d4ba0, OnProc 0x7d4e80, the message map and its control
// handlers, the draw code, LoadTutorialMainImages 0x7d2080,
// SetCompleteStates 0x7d2980, CheckNodeStates 0x7d3c30, OpenQuestGift
// 0x7d3eb0, DrawCourseArrow 0x7d2180. Until the
// constructor, destructor and OnInit have bodies the form is NOT
// registered for "tutorialmain" (a gen_missing stub constructor would
// leave the vptrs unset: G7); CTaskWidget gets NULL for the name, as for
// any unknown form.
#ifndef PG_JP_JPTUTORIALMAIN_H
#define PG_JP_JPTUTORIALMAIN_H

#include <list>

#include "defines.h"
#include "padding.h"
#include "JpFrForms.h"
#include "TutorialMain.h"

class CExhibition;
class CTutorialContainer;
class FrForm;
class FrQuestInfoDlg;
class FrWnd;
class FrWndManager;
class MsgObject;

class JpTutorialMain : public FrTaskForm {
	LAYOUT_CHECK_FRIEND;
public:
	// A row of m_Tutorials (0x18 bytes): the KR CTutorialMain::sTutorialMap.
	struct sTutorialMap {
		const char* m_name;       // 0x0, the button's caption (Jp_LookupText of the KR text)
		unsigned char m_category; // 0x4
		unsigned char m_index;    // 0x5
		unsigned char m_state;    // 0x6
		unsigned char m_level;    // 0x7, the button's row in the course tree
		unsigned char m_quest;    // 0x8
		int m_pending;            // 0xc, cleared when a dialog over the button closes
		int m_status;             // 0x10, 2: the category before is not complete
		bool m_enabled;           // 0x14, set by EnableAllBtn
	};
	// A node of m_tutorialList (the KR CTutorialMain::sTutorialInfo; the
	// list's node is 0x10 bytes: two links and these 8).
	struct sTutorialInfo {
		int m_id;       // 0x0
		bool m_enabled; // 0x4
		bool m_cleared; // 0x5
		UNKNOWN(0x6, 0x8);
	};

	static const WRTTI m_RTTI;                                        // 0xe7b3c8
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x7d4480

	JpTutorialMain(void);                                             // 0x7d4820 (not written)
	virtual ~JpTutorialMain(void);                                    // [1] 0x7d4b70 (deleting; not written)

	// Takes the contents container 0xa4 (NULL when missing), then
	// LoadTutorialMainImages.
	virtual bool Start(void);                                         // [38] 0x7d2490
	// Resets m_pipinTime, clears m_tutorialList, then FrTaskForm::Shutdown.
	virtual bool Shutdown(void);                                      // [39] 0x7d4030
	// 0x1b9: the tutorial's task and doc names for the caller; 0x1bc (the
	// KR TutorialMain 0x151, from CTutorialWidget::OnPacket 0x11f): the
	// course states again, the buttons back on, the arrows, and the clear
	// gift when param2 is set.
	virtual bool HandleMsg(const MsgObject& msg);                     // [40] 0x7d40a0

	// Its creator; __fastcall in the JP exe: CreateForm<CTutorialMain>.
	static FrWnd* Create(const char* name, FrWndManager* pManager);   // 0x7d5160

	// A course button's click (the KR OpenQuest): a mission whose category
	// is not open yet gets "notify"; else the container's quest is set, and
	// a mission done before asks "notify_yesno" (no gift), a new one sets
	// the container's reward flag and opens the mission's info dialog.
	void OpenQuest(int category, int index);                          // 0x7d52d0

protected:
	static sTutorialMap m_Tutorials[14];                              // 0xe7b3d8

	void LoadTutorialMainImages(void);                                // 0x7d2080 (not written)
	void HidePet(void);                                               // 0x7d20e0
	void EnableAllBtn(void);                                          // 0x7d20c0
	void DrawCourseArrow(void);                                       // 0x7d2180 (not written)
	// The container's +0x4 category, +0x8 index + 1, +0xc index.
	void SetQuestInformation(int category, int index);                // 0x7d2300
	void SetCompleteStates(void);                                     // 0x7d2980 (not written)
	bool IsCompleteQuest(int category, int index);                    // 0x7d3aa0
	void CheckNodeStates(void);                                       // 0x7d3c30 (not written)
	void OpenQuestGift(void);                                         // 0x7d3eb0 (not written)
	void InitGolfTutorial(void);                                      // 0x7d4110
	void StartSelectedTutorial(void);                                 // 0x7d4750
	// The mission's "quest_info" dialog (once), on the container's index.
	void OpenTutorialInfoDlg(void);                                   // 0x7d5200
	bool OnTutorialInfoDlgResult(int result, FrForm* form);           // 0x7d5040
	bool OnQuestionFormResult(int result, FrForm* form);              // 0x7d5280

private:
	PADDING(0x120, 0x144);
	FrForm* m_questionForm;                     // 0x144, "notify" / "notify_yesno" (OpenQuest)
	FrQuestInfoDlg* m_tutorialInfoDlg;          // 0x148, "quest_info" (OpenTutorialInfoDlg)
	PADDING(0x14c, 0x15c);
	CExhibition* m_pet;                         // 0x15c, deleted by HidePet
	PADDING(0x160, 0x410);
	std::list<sTutorialInfo> m_tutorialList;    // 0x410 (VS 2010: head 0x410, size 0x414)
	PADDING(0x418, 0x41c);
	unsigned char m_pendingTutorial;            // 0x41c, index into m_Tutorials, 0xff for none
	PADDING(0x41d, 0x420);
	// The KR CTutorialMain::m_pipinTime (TutorialMain.h): Shutdown runs its
	// Reset inline (+0x420, +0x424, the byte +0x428); the constructor
	// builds its string (+0x42c, size +0x43c, capacity +0x440 = 0xf).
	CPipinTime m_pipinTime;                     // 0x420
	PADDING(0x448, 0x458);
	CTutorialContainer* m_container;            // 0x458, contents container 0xa4 (Start)
	PADDING(0x45c, 0x460);
};
// size 0x460 (x86, the creator's new)
BEGIN_LAYOUT_CHECK(JpTutorialMain)
	CHECK_FIELD(JpTutorialMain, m_questionForm, 0x144);
	CHECK_FIELD(JpTutorialMain, m_tutorialInfoDlg, 0x148);
	CHECK_FIELD(JpTutorialMain, m_pet, 0x15c);
	CHECK_FIELD(JpTutorialMain, m_tutorialList, 0x410);
	CHECK_FIELD(JpTutorialMain, m_pendingTutorial, 0x41c);
	CHECK_FIELD(JpTutorialMain, m_pipinTime, 0x420);
	CHECK_FIELD(JpTutorialMain, m_container, 0x458);
END_LAYOUT_CHECK
CHECK_SIZE(JpTutorialMain, 0x460);

#endif // PG_JP_JPTUTORIALMAIN_H
