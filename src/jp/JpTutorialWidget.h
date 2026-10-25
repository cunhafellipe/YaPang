// JP-only game code (no KR counterpart class): CTutorialWidget, the lobby's
// TUTORIAL page. The JP has no CTutorialTask switch from the lobby: the
// TOPPAGE.pangyastudy click 0x5937f0 runs CloseAllDialogs (vtable +0x3c),
// then the lobby's own HandleMsg (+0x2c) with message 0x88
// ("CTutorialWidget", "TOPPAGE"); the Refresh desktop's 0x88 sets bar state
// 8 and sends 0x180, whose completion creates this widget through
// ObjectFactory (TaskManager_9bf6e0) as the current task's +0x1fc page. Its
// layout is the Fresh form "tutorialmain" (JP data/ui/tutorial.xml, the
// TUTORIAL_MAIN form). RTTI 0xe78fe8 ("CTutorialWidget", static init
// 0xcd4620, base CTaskWidget 0xe78d18); vtables 0xd19e6c (IObject),
// 0xd19e64 (CHandOwner: CTaskWidget's 0x64f5d0), 0xd19e58 (FrCmdTarget: the
// deleting-destructor thunk 0x6b2c50). Size 0x1f8 (x86, the factory's new):
// no members of its own.
//
// It also carries what the KR CLobbyMain did after the paran nick change
// (OnToppage_Finish, LobbyMain.cpp: the event prize form and the gift mail,
// now Lobby messages 0x43 and 0x3e in Open) and the KR CTutorialTask's
// packets (OnPacket: KR 0x11b -> JP 0x11f, KR 0x11c -> JP 0x120).
#ifndef PG_JP_JPTUTORIALWIDGET_H
#define PG_JP_JPTUTORIALWIDGET_H

#include "JpTaskPage.h"
#include "icontentsdoc.h"

class WReceivedPacket;

// Contents type 0xa4 (JP CContentsDoc table 0x915da0: `new` 0x18 bytes,
// the constructor inlined to the vptr 0xd3c5ec, RTTI "CTutorialContainer").
// The tutorial state the KR CTutorialMain kept in its own members
// (TutorialMain.h: m_questCategory, m_questNextIndex, m_questIndex,
// m_questReward, m_gradeGift): the JP CTutorialMain is a page form
// (FrTaskForm, its container pointer at +0x458) that dies with the page, so
// what the golf round and the page's return need lives here. Writers: the
// JP CTutorialMain::SetQuestInformation 0x7d2300 (+0x4 category, +0x8
// index + 1, +0xc index), OpenQuest 0x7d52d0 (+0x15), the widget's 0x120.
// The KR CContentsDoc has no 0xa4 container: IContentsDoc.cpp inserts it
// under JP_PAKS (JpCreateTutorialContainer below).
class CTutorialContainer : public IContentsDataContainer {
	LAYOUT_CHECK_FRIEND;
	// The JP CTutorialMain (JpTutorialMain.h) writes +0x4..+0xc and +0x15
	// directly (SetQuestInformation 0x7d2300, OpenQuest 0x7d52d0).
	friend class JpTutorialMain;
public:
	virtual ~CTutorialContainer(void) {}                              // [0] 0x912350 (deleting)

	virtual bool Initialize(void) { Reset(); return true; }           // [1] 0x9079a0
	virtual bool Release(void) { return true; }                       // [2] 0x9079b0
	virtual bool Reset(void) {                                        // [3] 0x9079c0
		m_questCategory = -1;
		m_questNextIndex = -1;
		m_questIndex = -1;
		m_value10 = -1;
		m_gradeGift = 0xff;
		m_questReward = 0;
		return true;
	}

	void SetGradeGift(unsigned char gradeGift) { m_gradeGift = gradeGift; }  // 0x6b2be0

	// The KR CTutorialMain::HandleMsg 0x150 send, called by the JP
	// CTutorial::MoveNextStep 0x7cd220 with (m_tutorialIndex, m_tutorialInfo,
	// 1, m_tutorialInfo == 0x80): packet 0xAE (the KR 0xA6) u8 complete, u8
	// m_questCategory, u32 the mission's bit, only when played is 1 and
	// m_questReward is set (cleared here). index is not read.
	void SendMissionResult(int index, unsigned long missionBit, int played, int complete);
	// Whether SetQuestInformation ran: the JP StartSelectedTutorial 0x7d4750
	// starts the round only then (else the main actor's message 0x23).
	bool IsQuestSet(void) const;

private:
	int m_questCategory;          // 0x4 (the KR CTutorialMain::m_questCategory)
	int m_questNextIndex;         // 0x8 (index + 1)
	int m_questIndex;             // 0xc
	int m_value10;                // 0x10 (-1 after Reset, not read yet)
	unsigned char m_gradeGift;    // 0x14 (packet 0x120; 0xff after Reset)
	unsigned char m_questReward;  // 0x15 (the KR m_questReward: the mission was not done before)
	UNKNOWN(0x16, 0x18);
};
BEGIN_LAYOUT_CHECK(CTutorialContainer)
	CHECK_FIELD(CTutorialContainer, m_questCategory, 0x4);
	CHECK_FIELD(CTutorialContainer, m_questNextIndex, 0x8);
	CHECK_FIELD(CTutorialContainer, m_questIndex, 0xc);
	CHECK_FIELD(CTutorialContainer, m_gradeGift, 0x14);
	CHECK_FIELD(CTutorialContainer, m_questReward, 0x15);
END_LAYOUT_CHECK
CHECK_SIZE(CTutorialContainer, 0x18);

class CTutorialWidget : public CTaskWidget {
public:
	static const WRTTI m_RTTI;                                        // 0xe78fe8
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x6b2c10

	CTutorialWidget(void);                                            // 0x6b2bf0
	virtual ~CTutorialWidget(void);                                   // [1] 0x6b2d10 (deleting), 0x6b2cf0
	// Sets m_layoutFile to "tutorialmain", sends the task's main actor
	// message 0 with "TOPPAGE", then CTaskWidget::Load.
	virtual void Load(void);                                          // [2] 0x6b3020
	// Opens the form (no callback, flag 3); after a paran nick change, the
	// Lobby actor's event prize message 0x43 and the gift mail 0x3e; then
	// CTaskWidget::Open.
	virtual void Open(void);                                          // [3] 0x6b2d40
	// Empty: the tutorial form is not shut down by the widget.
	virtual void CloseForm(void) {}                                   // [4] 0x6b2c20
	virtual void Process(float dt);                                   // [5] 0x6b2c30
	virtual int OnPacket(WReceivedPacket& packet);                    // [6] 0x6b2ee0
	// [7] and [8] are CTaskWidget's (0x64f1c0, 0x64f1d0).

	// Port glue (no JP function): JP CSharedDoc +0x10d5c, the KR
	// m_value4D98 (protected; this class is CSharedDoc's friend under
	// JP_PAKS), for JpTutorialMain::InitGolfTutorial 0x7d4110.
	static unsigned long GetTutorialQuest(void);
	static void SetTutorialQuest(unsigned long quest);
};
CHECK_SIZE(CTutorialWidget, 0x1f8);

// Port glue for the KR tutorial.cpp (JP_PAKS, declared there without this
// header, L38): CTutorialContainer::SendMissionResult on the contents
// container 0xa4 (the JP CTutorial holds it at +0xdac). Nothing without one.
void JpTutorialContainer_SendMissionResult(int index, unsigned long missionBit, int played, int complete);

#endif // PG_JP_JPTUTORIALWIDGET_H
