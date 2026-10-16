// Original: d:\build\custom\temp\code\client\projectg\tutorialmain.h
#ifndef PG_CLIENT_PROJECTG_TUTORIALMAIN_H
#define PG_CLIENT_PROJECTG_TUTORIALMAIN_H

#include "defines.h"
#include "padding.h"
#include "TaskMain.h"
#include "RTTI.h"
#include "client/Wangreal/include/wtypes.h"

#include <list>
#include <string>
#include <vector>
#include <windows.h>
#include <mmsystem.h>

class IObject;
class CExhibition;
class Bitmap;
class FrQuestInfoDlg;
class FrQuestGift;
class FrArea;
class FrListBox;

// The tutorial pet's motion timers and its next motion (CTutorialMain
// 0x4FC, size 0x28).
class CPipinTime {
	LAYOUT_CHECK_FRIEND;
public:
	// tutorialmain.h:60
	CPipinTime(void) : m_lastMotionTime(0), m_lastIdleTime(0), m_selected(false) {}
	// tutorialmain.h:61
	~CPipinTime(void) {}

	// tutorialmain.h:64
	void Reset(void) {
		m_lastMotionTime = 0;
		m_lastIdleTime = 0;
		m_selected = false;
	}

	// tutorialmain.h:71
	void SetTime(void) {
		m_lastMotionTime = timeGetTime();
		m_lastIdleTime = m_lastMotionTime;
	}

	unsigned long m_lastMotionTime;  // 0x0, timeGetTime of the pet's last random motion (OnProcess, every 5 s)
	unsigned long m_lastIdleTime;    // 0x4, timeGetTime of the pet's last idle motion (OnProcess, every 16 s)
	bool m_selected;                 // 0x8, cleared by OnDestroy
	PADDING(0x9, 0xC);
	std::string m_nextMotion;        // 0xC, the motion name DecideNextMotion picks
};
CHECK_SIZE(CPipinTime, 0x28);
CHECK_FIELD(CPipinTime, m_lastMotionTime, 0x0);
CHECK_FIELD(CPipinTime, m_lastIdleTime, 0x4);
CHECK_FIELD(CPipinTime, m_selected, 0x8);
CHECK_FIELD(CPipinTime, m_nextMotion, 0xC);

// The tutorial lobby task. Its size (0x534) is the `new` of
// CTutorialMainMakeInstance. Only the members the transcribed functions
// touch are named.
class CTutorialMain : public CTaskMain {
	LAYOUT_CHECK_FRIEND;
public:
	static const WRTTI m_RTTI;
	virtual const WRTTI *GetRTTI(void) const { return &m_RTTI; } // tutorialmain.h:85

	// A row of m_Tutorials: a tutorial button and the quest it opens.
	struct sTutorialMap {
		const char *m_name;       // 0x0, the button's caption
		unsigned char m_category; // 0x4
		unsigned char m_index;    // 0x5
		unsigned char m_state;    // 0x6
		unsigned char m_level;    // 0x7, the button's row in the course tree (0..4)
		unsigned char m_quest;    // 0x8
		int m_pending;            // 0xC, cleared when a dialog over the button closes
		int m_status;             // 0x10, 0 or 1, 2 once cleared (shows the course arrow, replays ask first)
		bool m_enabled;           // 0x14, set by EnableAllBtn
	};

	// A row of the tutorial list box (OnTutorialListInit adds one per category).
	struct sTutorialResult {
		unsigned char m_category; // 0x0
		int m_count;              // 0x4, 3, or 4 once the category is complete (CheckNodeStates)
	};

	// A course arrow between tutorial buttons: shown once the buttons from
	// m_first to m_last are cleared (DrawCourseArrow).
	struct sArrowInfo {
		unsigned char m_first; // 0x0
		unsigned char m_last;  // 0x1
		unsigned char m_show;  // 0x2
	};

	// The tutorials a cleared one opens (CheckEnable, indexed by the cleared
	// quest): with m_unlock 0, the m_count tutorials from m_start; otherwise
	// the tutorial m_unlock once those m_count tutorials are all cleared.
	struct sTutorialCondition {
		int m_index;  // 0x0
		int m_unlock; // 0x4
		int m_start;  // 0x8
		int m_count;  // 0xC
	};

	// The quests a quest opens: m_count quests from m_first (OnInit fills
	// CQuestInfo::m_items from it).
	struct sChildTable {
		int m_quest; // 0x0
		int m_first; // 0x4
		int m_count; // 0x8
	};

	// A node of the tutorial list FindTutorialList searches.
	struct sTutorialInfo {
		int m_id;       // 0x0
		bool m_enabled; // 0x4, set by CheckEnable
		bool m_cleared; // 0x5, set by MissionClear
		UNKNOWN(0x6, 0x8);
	};

	// A quest's entry of m_questInfo (0x14 bytes, 14 of them).
	class CQuestInfo {
	public:
		std::vector<int> m_items;         // 0x0, the quests this one opens (OnInit, from m_ChildTable)
		unsigned char m_parentCount;      // 0x10, the quests one row up that open this one
		unsigned char m_parentsCleared;   // 0x11, how many of those CheckNodeStates found cleared
		UNKNOWN(0x12, 0x14);
	};

	CTutorialMain(void);                   // tutorialmain.cpp:375
	virtual ~CTutorialMain(void);         // tutorialmain.cpp:438
	virtual void OnPreserveBack(const char *); // tutorialmain.cpp:443
	virtual void OnLoad(void);             // tutorialmain.cpp:457
	virtual void OnInit(void);             // tutorialmain.cpp:464
	virtual void OnRestorePreserved(void); // tutorialmain.cpp:450

	void EnableAllBtn(void);               // tutorialmain.cpp:525
	void ShowPet(void);                    // tutorialmain.cpp:656
	void HidePet(void);                    // tutorialmain.cpp:689
	int GetSelectedBtn(int type, WPoint pt); // tutorialmain.cpp:1332
	void OpenGradeGift(unsigned char grade); // tutorialmain.cpp:1517
	void OpenQuestGift(void);              // tutorialmain.cpp:1509
	void DecideNextMotion(unsigned char grade); // tutorialmain.cpp:1526
	bool IsCompleteCategory(int category); // tutorialmain.cpp:1815
	bool IsCompleteQuest(int category, int index); // tutorialmain.cpp:1800
	void OpenQuest(int category, int index); // tutorialmain.cpp:1467
	void SetCompleteStates(void);          // tutorialmain.cpp:536
	void CheckNodeStates(void);            // tutorialmain.cpp:590
	virtual void OnProcess(float dt);      // tutorialmain.cpp:737
	virtual void OnDestroy(void);          // tutorialmain.cpp:798
	virtual void HandleMsg(const MsgObject &msg); // tutorialmain.cpp:805

private:
	static sFRESH_ENTRY _MsgEntries[];

protected:
	static sTutorialMap m_Tutorials[14];
	static const sTutorialCondition m_TutoCondition[8];
	static const sChildTable m_ChildTable[14];
	static sTutorialResult m_CompleteItems[3];
	static sArrowInfo m_ArrowItems[10];
	static sFRESH_MSGMAP _MsgMap;
	virtual const sFRESH_MSGMAP *GetMessageMap(void) const; // tutorialmain.cpp:176

	void LoadTutorialMainImages(void);     // tutorialmain.cpp:517
	void DisableOverBarButtons(void);      // tutorialmain.cpp:914
	void DisableUnderBarButtons(void);     // tutorialmain.cpp:953
	void OnTutorialMainInit(PG_MSGARG);          // tutorialmain.cpp:1007
	void OnTutorialMainFinish(void);       // tutorialmain.cpp:1013
	void OnTutorialAreaPipin(PG_MSGARG param);   // tutorialmain.cpp:1024
	void OnTutorialAreaPipinOwnerDraw(PG_MSGARG); // tutorialmain.cpp:1029
	void OnTutorialListInit(PG_MSGARG param);    // tutorialmain.cpp:1040
	void OnTutorialListOwnerDraw(PG_MSGARG param); // tutorialmain.cpp:1062
	void DrawCourseArrow(void);            // tutorialmain.cpp:1309
	void OnTutorialListLBtnDown(void);     // tutorialmain.cpp:1368
	void OnTutorialListLBtnUp(void);       // tutorialmain.cpp:1425
	void CheckEnable(int index);           // tutorialmain.cpp:1752
	void ProcessLobbyBgCamera(float dt);   // tutorialmain.cpp:698
	void MissionClear(int &index, unsigned long &flags); // tutorialmain.cpp:1569
	void MissionComplete(unsigned long flags);           // tutorialmain.cpp:1557
	void ProcessAnimateIcon(void);         // tutorialmain.cpp:784
	bool OnGiftDlgResult(int result, FrForm *form); // tutorialmain.cpp:1628
	sTutorialInfo *FindTutorialList(int &id); // tutorialmain.cpp:1734
	void StartSelectedTutorial(void);      // tutorialmain.cpp:1641
	void InitGolfTutorial(void);           // tutorialmain.cpp:1702
	void OpenTutorialInfoDlg(void);
	bool OnTutorialInfoDlgResult(int result, FrForm *form); // tutorialmain.cpp:1601
	bool OnQuestionFormResult(int result, FrForm *form);    // tutorialmain.cpp:1848
	void SetQuestInformation(sTutorialMap *map);         // tutorialmain.cpp:1446
	void SetQuestInformation(int category, int index);   // tutorialmain.cpp:1457

private:
	int IsInArea(int type, WPoint pt);     // tutorialmain.cpp:1339

	FrListBox *m_tutorialListBox;    // 0x1E0, OnTutorialListInit
	PADDING(0x1E4, 0x204);
	FrForm *m_questionForm;          // 0x204, cleared by OnQuestionFormResult
	FrQuestInfoDlg *m_tutorialInfoDlg; // 0x208, cleared by OnTutorialInfoDlgResult
	FrQuestGift *m_questGift;        // 0x20C, OpenQuestGift / OpenGradeGift
	PADDING(0x210, 0x214);
	FrArea *m_pipinArea;             // 0x214, OnTutorialAreaPipin
	PADDING(0x218, 0x21C);
	CExhibition *m_pet;              // 0x21C, deleted by HidePet
	float m_camDirection;            // 0x220, +-1: the lobby camera's pan direction (ProcessLobbyBgCamera)
	bool m_camPaused;                // 0x224, the camera waits 2 s before reversing
	PADDING(0x225, 0x228);
	unsigned long m_camPauseTime;    // 0x228, g_CurrentTime the pause started
	unsigned long m_camMoveTime;     // 0x22C, g_CurrentTime the pan started (80 s)
	const Bitmap *m_images[64];      // 0x230, LoadTutorialMainImages
	WPoint m_overBarBtnPos[8];       // 0x330, IsInArea type 0 (85x48 buttons)
	WPoint m_underBarBtnPos[6];      // 0x370, IsInArea type 1
	float m_columnX[5];              // 0x3A0, 121..486, set by the constructor (readers not transcribed yet)
	int m_listState;                 // 0x3B4, cleared by the constructor (readers not transcribed yet)
	int m_questState;                // 0x3B8
	int m_questCategory;             // 0x3BC
	int m_questNextIndex;            // 0x3C0
	int m_questIndex;                // 0x3C4
	int m_questValue;                // 0x3C8, reported by message 0x14F with the four above (writer not transcribed yet)
	bool m_categoryClear[3];         // 0x3CC, [0] set when mission 7 clears with flag 0x80
	bool m_questReward;              // 0x3CF, OpenQuest: set for a quest not cleared yet
	unsigned char m_gradeGift;       // 0x3D0, the grade OnGiftDlgResult opens
	PADDING(0x3D1, 0x3D4);
	CQuestInfo m_questInfo[14];      // 0x3D4
	std::list<sTutorialInfo> m_tutorialList; // 0x4EC
	unsigned char m_pendingTutorial; // 0x4F8, index into m_Tutorials, 0xFF for none
	PADDING(0x4F9, 0x4FC);
	CPipinTime m_pipinTime;          // 0x4FC
	float m_iconOffsetX;             // 0x524, cleared by the constructor
	float m_iconOffset;              // 0x528, ProcessAnimateIcon's bounce
	float m_iconOffsetZ;             // 0x52C, cleared by the constructor
	unsigned char m_categoryCount;   // 0x530, the categories SetCompleteStates walks
	PADDING(0x531, 0x534);
};

IObject *CTutorialMainMakeInstance(void);

CHECK_SIZE(CTutorialMain, 0x534);
CHECK_FIELD(CTutorialMain::sTutorialMap, m_name, 0x0);
CHECK_FIELD(CTutorialMain::sTutorialMap, m_category, 0x4);
CHECK_FIELD(CTutorialMain::sTutorialMap, m_index, 0x5);
CHECK_FIELD(CTutorialMain::sTutorialMap, m_state, 0x6);
CHECK_FIELD(CTutorialMain::sTutorialMap, m_level, 0x7);
CHECK_FIELD(CTutorialMain::sTutorialMap, m_quest, 0x8);
CHECK_FIELD(CTutorialMain::sTutorialMap, m_pending, 0xC);
CHECK_FIELD(CTutorialMain::sTutorialMap, m_status, 0x10);
CHECK_FIELD(CTutorialMain::sTutorialResult, m_count, 0x4);
CHECK_SIZE(CTutorialMain::sTutorialResult, 0x8);
CHECK_SIZE(CTutorialMain::sArrowInfo, 0x3);
CHECK_SIZE(CTutorialMain::sTutorialCondition, 0x10);
CHECK_FIELD(CTutorialMain::sTutorialCondition, m_unlock, 0x4);
CHECK_FIELD(CTutorialMain::sTutorialCondition, m_start, 0x8);
CHECK_FIELD(CTutorialMain::sTutorialCondition, m_count, 0xC);
CHECK_FIELD(CTutorialMain::sTutorialMap, m_enabled, 0x14);
CHECK_SIZE(CTutorialMain::sTutorialMap, 0x18);
CHECK_FIELD(CTutorialMain::sChildTable, m_first, 0x4);
CHECK_FIELD(CTutorialMain::sChildTable, m_count, 0x8);
CHECK_SIZE(CTutorialMain::sChildTable, 0xC);
CHECK_SIZE(CTutorialMain::sTutorialInfo, 0x8);
CHECK_SIZE(CTutorialMain::CQuestInfo, 0x14);
CHECK_FIELD(CTutorialMain::sTutorialInfo, m_id, 0x0);
CHECK_FIELD(CTutorialMain::sTutorialInfo, m_enabled, 0x4);
CHECK_FIELD(CTutorialMain::sTutorialInfo, m_cleared, 0x5);
BEGIN_LAYOUT_CHECK(CTutorialMain)
	CHECK_FIELD(CTutorialMain, m_tutorialListBox, 0x1E0);
	CHECK_FIELD(CTutorialMain, m_questionForm, 0x204);
	CHECK_FIELD(CTutorialMain, m_tutorialInfoDlg, 0x208);
	CHECK_FIELD(CTutorialMain, m_questGift, 0x20C);
	CHECK_FIELD(CTutorialMain, m_pipinArea, 0x214);
	CHECK_FIELD(CTutorialMain, m_pet, 0x21C);
	CHECK_FIELD(CTutorialMain, m_camDirection, 0x220);
	CHECK_FIELD(CTutorialMain, m_camPaused, 0x224);
	CHECK_FIELD(CTutorialMain, m_camPauseTime, 0x228);
	CHECK_FIELD(CTutorialMain, m_camMoveTime, 0x22C);
	CHECK_FIELD(CTutorialMain, m_images, 0x230);
	CHECK_FIELD(CTutorialMain, m_overBarBtnPos, 0x330);
	CHECK_FIELD(CTutorialMain, m_underBarBtnPos, 0x370);
	CHECK_FIELD(CTutorialMain, m_columnX, 0x3A0);
	CHECK_FIELD(CTutorialMain, m_listState, 0x3B4);
	CHECK_FIELD(CTutorialMain, m_questState, 0x3B8);
	CHECK_FIELD(CTutorialMain, m_questCategory, 0x3BC);
	CHECK_FIELD(CTutorialMain, m_questNextIndex, 0x3C0);
	CHECK_FIELD(CTutorialMain, m_questIndex, 0x3C4);
	CHECK_FIELD(CTutorialMain, m_categoryClear, 0x3CC);
	CHECK_FIELD(CTutorialMain, m_questReward, 0x3CF);
	CHECK_FIELD(CTutorialMain, m_gradeGift, 0x3D0);
	CHECK_FIELD(CTutorialMain, m_questInfo, 0x3D4);
	CHECK_FIELD(CTutorialMain, m_tutorialList, 0x4EC);
	CHECK_FIELD(CTutorialMain, m_pendingTutorial, 0x4F8);
	CHECK_FIELD(CTutorialMain, m_pipinTime, 0x4FC);
	CHECK_FIELD(CTutorialMain, m_iconOffsetX, 0x524);
	CHECK_FIELD(CTutorialMain, m_iconOffset, 0x528);
	CHECK_FIELD(CTutorialMain, m_iconOffsetZ, 0x52C);
	CHECK_FIELD(CTutorialMain, m_categoryCount, 0x530);
END_LAYOUT_CHECK

#endif // PG_CLIENT_PROJECTG_TUTORIALMAIN_H
