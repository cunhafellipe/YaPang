// JP-only game code (no KR counterpart): CTaskWidget, the page a JP task
// shows in its CTask +0x1fc slot (the lobby's shop, my-room and tutorial
// pages: CShopWidget, CMyRoomWidget, CTutorialWidget, and CNullWidget for
// "none"). A Fresh layout loaded on the widget's CAnotherHand thread under a
// lock, then opened, with the "ESCAPE" key taken while it is up; the task's
// main actor gets message 0 with the page name (the KR NaviOpenLayout
// convention, CTaskManager::history_t). Its RTTI is 0xe78d18 ("CTaskWidget",
// static init 0xcd3910, base IObject 0xf0073c); vtable 0xd1694c (IObject),
// 0xd16944 (CHandOwner), 0xd16938 (FrCmdTarget). Size 0x1f8 (x86).
#ifndef PG_JP_JPTASKPAGE_H
#define PG_JP_JPTASKPAGE_H

#include <string>

#include <windows.h>

#include "defines.h"
#include "padding.h"
#include "ObjectFactory.h"
#include "AnotherHand.h"
#include "frcmdtarget.h"
#include "wflag.h"

class FrForm;
class FrTaskForm;
class CTask;
class CTaskManager;
class WReceivedPacket;

// The widget's lock at +0x1dc: a vtable (0xd0a5ec) then a CRITICAL_SECTION;
// CTaskWidget 0x64f620 runs its inline constructor (spin count 0x7d0),
// ~CTaskWidget 0x64f710 its inline destructor (DeleteCriticalSection).
class CTaskWidgetLock {
public:
	CTaskWidgetLock(void) { InitializeCriticalSectionAndSpinCount(&m_criticalSection, 0x7d0); }
	virtual ~CTaskWidgetLock(void) { DeleteCriticalSection(&m_criticalSection); }
	void Lock(void) { EnterCriticalSection(&m_criticalSection); }
	void Unlock(void) { LeaveCriticalSection(&m_criticalSection); }

	CRITICAL_SECTION m_criticalSection; // 0x4
};

class CTaskWidget : public IObject, public CHandOwner, public FrCmdTarget {
public:
	static const WRTTI m_RTTI;                                        // 0xe78d18
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x64f700
	CTaskWidget(void);                                                // 0x64f620
	virtual ~CTaskWidget(void);                                       // [1] 0x64f8a0 (deleting), 0x64f710
	// Loads m_layoutFile into m_pForm under m_lock; a failure sends the main
	// actor message 0x23 with game text 0x1389 / 0x138a.
	virtual void Load(void);                                          // [2] 0x64f3b0
	// Shows m_pForm, sets the opened flag, releases "ESCAPE".
	virtual void Open(void);                                          // [3] 0x64f230
	virtual void CloseForm(void);                                     // [4] 0x64f1a0 (m_pForm's slot 39)
	virtual void Process(float dt);                                   // [5] 0x64f290
	// The packet by reference (ret 4), as the KR CTask::OnPacket; nonzero =
	// handled.
	virtual int OnPacket(WReceivedPacket& packet) { return 0; }       // [6] 0x64f1e0
	virtual void OnProcessWhileLoading(float dt) {}                   // [7] 0x64f1c0
	virtual void OnInitFinished(void) {}                              // [8] 0x64f1d0

	// CHandOwner: job 1 (posted by the loading code) = Start's body.
	virtual void OnAnotherHand(int nWork, void* pData);               // 0x64f5d0 (thunk, this - 4)

	FrForm* GetForm(void) const;                                      // 0x64f1f0
	void SetPage(const char* page);                                   // 0x64f8d0
	void Start(void);                                                 // 0x64f600

protected:
	void BeginOpen(void);                                             // 0x64f310

	WFlags m_stateFlags;          // 0xc (bit 0: loaded, bit 1: opened)
	bool m_processing;            // 0x10 (Process runs only while set; Open clears it)
	UNKNOWN(0x11, 0x14);
	CAnotherHand m_anotherHand;   // 0x14
	int m_value19c;               // 0x19c (cleared by BeginOpen)
	FrTaskForm* m_pForm;          // 0x1a0
	std::string m_layoutFile;     // 0x1a4 (the Fresh layout Load reads)
	std::string m_page;           // 0x1c0 (SetPage; message 0's param1)
	CTaskWidgetLock m_lock;       // 0x1dc
};

// Defined in JpTaskPage.cpp; declared in JpLobbyGlue.h (owned elsewhere).
bool Task_IsLayoutOpen(CTask* task, const char* layout);              // 0x9bd4f0
FrForm* Task_GetWidgetForm(CTask* task);                              // 0x9b7810
void Form_Close(FrForm* form, bool close);
void TaskManager_9bf6e0(CTaskManager* manager, const char* name, const char* page);

#endif
