// JP-only game code: CTaskWidget (JpTaskPage.h), the JP CTask +0x1fc widget
// members (TaskManager.h, JP_PAKS) and the lobby glue that reads them
// (declared in JpLobbyGlue.h). JP VAs in comments; the JP-only Fresh (the
// form registry, FrTaskForm) is JpFrForms.h.
#include "JpTaskPage.h"

#include <string.h>

#include "TaskManager.h"
#include "frwnd.h"
#include "frform.h"
#include "frwndmanager.h"
#include "fresh.h"
#include "InputManager.h"
#include "mousecursor.h"

#include "JpFrForms.h"
#include "JpLobbyGlue.h"
#include "JpShopWidget.h"
#include "JpMyRoomWidget.h"
#include "JpTutorialWidget.h"

// --- CTask (JP_PAKS members, TaskManager.h) ------------------------------

// 0xf00688
CTaskWidget* CTask::ms_pNullWidget = NULL;

// 0x9b7780
void CTask::SetWidget(CTaskWidget* pWidget) {
	m_pWidget = pWidget;
}

// 0x9b7790
void CTask::ResetWidget(void) {
	if (m_pWidget != ms_pNullWidget) {
		if (m_pWidget) {
			delete m_pWidget;
			m_pWidget = NULL;
		}
		m_pWidget = ms_pNullWidget;
	}
}

// 0x9b77d0
void CTask::DeleteWidget(void) {
	if (m_pWidget != ms_pNullWidget && m_pWidget) {
		delete m_pWidget;
		m_pWidget = NULL;
	}
	m_pWidget = ms_pNullWidget;
}

// 0x9b7810 (0x9b7830: the same with an unused argument)
FrForm* CTask::GetWidgetForm(void) const {
	if (m_pWidget == ms_pNullWidget) {
		return NULL;
	}
	return m_pWidget->GetForm();
}

// 0x9bd4f0
bool CTask::IsWidgetLayout(const char* layout) const {
	if (m_pWidget == ms_pNullWidget) {
		return false;
	}
	FrForm* pForm = m_pWidget->GetForm();
	if (!pForm) {
		return false;
	}
	// 0xbe75d0: JP FrWnd +0x64 = the KR m_wndName (+0x50 plus the JP
	// FrModalDim base).
	std::string name;
	pForm->GetWindowName(name);
	return name == layout;
}

// --- CTaskWidget ----------------------------------------------------------

// lib: 0xcd3910 (m_RTTI's dynamic initializer, compiler-generated from
// this definition: WRTTI::WRTTI 0xa4c140, base IObject)
const WRTTI CTaskWidget::m_RTTI("CTaskWidget", &IObject::m_RTTI);

// 0x64f1a0
void CTaskWidget::CloseForm(void) {
	if (m_pForm) {
		m_pForm->Shutdown();  // JP [39] 0x523610
	}
}

// 0x64f1f0
FrForm* CTaskWidget::GetForm(void) const {
	return m_pForm;
}

// 0x64f230
void CTaskWidget::Open(void) {
	if (m_pForm) {
		m_pForm->Enable(true);      // JP [6] 0x47ccd0
		m_pForm->SetVisible(true);  // JP [7] 0xbe7a00
	}
	m_stateFlags.Enable(2);
	m_processing = false;
	g_input->FreeKey("ESCAPE");               // 0x9a6e70
	g_input->FreeMouse();                     // 0x9a64b0
	CMouseCursor::Instance()->SetCursor(1, 0); // 0x891020
}

// 0x64f290
void CTaskWidget::Process(float dt) {
	if (m_processing && !(m_stateFlags & 3)) {
		OnProcessWhileLoading(dt);
		Sleep(30);
	}
}

// 0x64f310
void CTaskWidget::BeginOpen(void) {
	g_input->BlockMouse();        // 0x9a6490
	g_input->BlockKey("ESCAPE");  // 0x9a6e50
	m_value19c = 0;
	CMouseCursor::Instance()->SetCursor(7, 0); // 0x891020
	AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0, (intptr_t)m_page.c_str(), 0, 0, 0, 0));
}

// 0x64f3b0
void CTaskWidget::Load(void) {
	const char* file = m_layoutFile.c_str();
	m_lock.Lock();
	FrWnd* pWnd = JpFrFormRegistry_Create(JpFrFormRegistry_Instance(), file, g_pFresh->GetManager());
	if (!pWnd) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23, (intptr_t)Jp_GetTableText(0x1389), 0, 0, 0, 0));
		m_lock.Unlock();
		return;
	}
	pWnd->Enable(false);  // JP [6] 0x47ccd0
	m_lock.Unlock();
	// 0x47cc60 with 0xe74f88; after the unlock the JP also stores pWnd in a
	// thread-local (+0xbc of the TLS block).
	FrTaskForm* pForm = DYNAMIC_CAST(FrTaskForm, pWnd);
	if (!pForm) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23, (intptr_t)Jp_GetTableText(0x1389), 0, 0, 0, 0));
		return;
	}
	if (!pForm->Start()) {  // JP [38] 0x5235f0
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x23, (intptr_t)Jp_GetTableText(0x138a), 0, 0, 0, 0));
		return;
	}
	m_pForm = pForm;
	m_stateFlags.Enable(1);
}

// 0x64f5d0 (CHandOwner thunk)
void CTaskWidget::OnAnotherHand(int nWork, void* pData) {
	if (nWork == 1) {
		BeginOpen();
		Load();
		Open();
	}
}

// 0x64f600
void CTaskWidget::Start(void) {
	BeginOpen();
	Load();
	Open();
}

// 0x64f620: IObject 0xa49170, CHandOwner, FrCmdTarget 0xc08500, then
// m_anotherHand (CAnotherHand 0x98ad90); m_processing is cleared last.
CTaskWidget::CTaskWidget(void) : m_value19c(0), m_pForm(NULL) {
	m_processing = false;
}

// 0x64f710 (0x64f8a0: the deleting destructor)
CTaskWidget::~CTaskWidget(void) {
	CloseForm();
}

// 0x64f8d0
void CTaskWidget::SetPage(const char* page) {
	if (page) {
		m_page.assign(page, strlen(page));
	}
}

// --- lobby glue (JpLobbyGlue.h) ------------------------------------------

// 0x9bd4f0
bool Task_IsLayoutOpen(CTask* task, const char* layout) {
	return task->IsWidgetLayout(layout);
}

// 0x9b7810: the widget's FrForm.
FrForm* Task_GetWidgetForm(CTask* task) {
	return task->GetWidgetForm();
}

// The form's vtable slot 2 (+8): FrWnd::Close.
void Form_Close(FrForm* form, bool close) {
	form->Close(close);
}

// Port only (G11): the pages are created by name through ObjectFactory, and
// their objects define nothing else the game references, so the static
// pg_refresh_game archive would drop them with their registrars and
// CreateObject("CMyRoomWidget") would return NULL. These references keep
// the three page objects linked. The JP links them directly.
extern const WRTTI* const g_jpTaskPageRTTIs[3];
const WRTTI* const g_jpTaskPageRTTIs[3] = {
	&CShopWidget::m_RTTI,
	&CMyRoomWidget::m_RTTI,
	&CTutorialWidget::m_RTTI,
};

// 0x9bf6e0
void TaskManager_9bf6e0(CTaskManager* manager, const char* name, const char* page) {
	IObject* pObject = ObjectFactory().CreateObject(name);
	if (!pObject) {
		manager->GetCurrentTask()->ResetWidget();
		return;
	}
	CTaskWidget* pWidget = DYNAMIC_CAST(CTaskWidget, pObject);
	if (pWidget) {
		CTask* pTask = manager->GetCurrentTask();
		pTask->DeleteWidget();
		pTask->SetWidget(pWidget);
		pWidget->SetPage(page);
		pWidget->Start();
	} else {
		delete pObject;
	}
}
