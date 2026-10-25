// Original: (JP 983 game code, no PDB) _gateway::CArinBirthDay2014Handler
// lib: 0x921110 (deleting destructor), 0x4788e0 (FunctionMapper::Exec
// instance), 0x478870 / 0x478600 (the function map's insert), 0x9211c0
// (factory creator)
#include "JpArinEventHandler.h"

#include "JpArinEvent.h"
#include "JpMasteryCard.h"
#include "Packet.h"
#include "TaskManager.h"
#include "actor.h"

namespace _gateway {

// 0x477780
void CArinBirthDay2014Handler::OnIgnored(intptr_t) {
}

// 0x477bf0
void CArinBirthDay2014Handler::OnTicketsDone(void) {
	if (g_arinBirthday2014Dlg != NULL) {
		g_arinBirthday2014Dlg->SetStage(3);
		g_arinBirthday2014Dlg->UpdateStage();
	}
}

// 0x477c10
void CArinBirthDay2014Handler::OnStage5(void) {
	if (g_arinBirthday2014Dlg != NULL) {
		g_arinBirthday2014Dlg->SetStage(5);
		g_arinBirthday2014Dlg->UpdateStage();
	}
}

// 0x477c30
void CArinBirthDay2014Handler::OnRewardTaken(intptr_t param) {
	const MsgObject* pMsg = (const MsgObject*)param;
	WReceivedPacket* pPacket = (WReceivedPacket*)pMsg->m_param2;
	int reward = (int)pPacket->Decode4();
	if (g_arinBirthday2014Dlg != NULL) {
		g_arinBirthday2014Dlg->ShowRewardNotify();
		g_arinBirthday2014Dlg->ShowText(10);
		g_arinBirthday2014Dlg->ApplyReward(reward - 0x15);
	}
}

// 0x477c80
void CArinBirthDay2014Handler::OnFinalRewardTaken(void) {
	if (g_arinBirthday2014Dlg != NULL) {
		g_arinBirthday2014Dlg->ShowFinalNotify();
		g_arinBirthday2014Dlg->ApplyFinalReward();
	}
}

// 0x478720
void CArinBirthDay2014Handler::OpenDialog(void) {
	int stage;
	if (Jp_GameState_FindInventoryItem(0x1be00013) != NULL) {
		stage = 9;
	} else {
		stage = Jp_GameState_FindInventoryItem(0x1a0002b8) != NULL ? 4 : 0;
	}
	if (Jp_GameState_FindInventoryItem(0x1be00014) != NULL) {
		stage = 0xb;
	}
	if (g_arinBirthday2014Dlg == NULL) {
		RF2014ArinBirthdayDlg* dlg = static_cast<RF2014ArinBirthdayDlg*>(CreateArinBirthday2014Dlg(NULL));
		if (dlg != NULL) {
			dlg->SetEventState(GetDocument<CArinBirthDay2014Doc*>());
			dlg->SetStage(stage);
			dlg->UpdateStage();
			dlg->CenterWindow(NULL);
			dlg->ShowModal();
		}
	}
}

// 0x4787c0
void CArinBirthDay2014Handler::OnEventState(intptr_t param) {
	CArinBirthDay2014Doc* doc = GetDocument<CArinBirthDay2014Doc*>();
	if (doc != NULL) {
		doc->ReadState(*(WReceivedPacket*)((const MsgObject*)param)->m_param2);
		if (doc->m_missions.size() >= 5) {
			OpenDialog();
		}
	}
}

// 0x478a30
bool CArinBirthDay2014Handler::Execute(const MsgObject& msg) {
	SetFunctionType(msg.m_param1);
	return Exec(this, (intptr_t)&msg);
}

// 0x478ab0
bool CArinBirthDay2014Handler::Initialize(void) {
	AddFunctionType(0, FUNCTION_TYPE_VV, (FUNCTION_MAPPER_PFN)&CArinBirthDay2014Handler::OpenDialog);
	AddFunctionType(1, FUNCTION_TYPE_VP, (FUNCTION_MAPPER_PFN)(void (CArinBirthDay2014Handler::*)(void))&CArinBirthDay2014Handler::OnEventState);
	AddFunctionType(2, FUNCTION_TYPE_VV, (FUNCTION_MAPPER_PFN)&CArinBirthDay2014Handler::OnTicketsDone);
	AddFunctionType(3, FUNCTION_TYPE_VV, (FUNCTION_MAPPER_PFN)&CArinBirthDay2014Handler::OnStage5);
	AddFunctionType(4, FUNCTION_TYPE_VP, (FUNCTION_MAPPER_PFN)(void (CArinBirthDay2014Handler::*)(void))&CArinBirthDay2014Handler::OnRewardTaken);
	AddFunctionType(5, FUNCTION_TYPE_VV, (FUNCTION_MAPPER_PFN)&CArinBirthDay2014Handler::OnFinalRewardTaken);
	AddFunctionType(6, FUNCTION_TYPE_VP, (FUNCTION_MAPPER_PFN)(void (CArinBirthDay2014Handler::*)(void))&CArinBirthDay2014Handler::OnIgnored);
	return true;
}

// --- CArinBirthDay2014Doc ------------------------------------------------------

// 0x920050
bool CArinBirthDay2014Doc::Release(void) {
	if (m_pGatewayHandler != NULL) {
		delete m_pGatewayHandler;
		m_pGatewayHandler = NULL;
	}
	return true;
}

// 0x920a90 (the JP keeps the document type 0xB0 at +8; the KR container
// base has no such member). The JP leaves +0xc to Initialize.
CArinBirthDay2014Doc::CArinBirthDay2014Doc(void) : m_pGatewayHandler(NULL) {
}

// 0x920ab0
CGatewayHandler* CArinBirthDay2014Doc::GetGatewayHandler(void) {
	return m_pGatewayHandler;
}

// 0x920b40 (deleting destructor; the missions freed)
CArinBirthDay2014Doc::~CArinBirthDay2014Doc(void) {
}

// 0x920cc0
void CArinBirthDay2014Doc::ReadState(WReceivedPacket& packet) {
	m_missions.clear();
	unsigned long count = packet.Decode4();
	for (unsigned long i = 0; i < count; ++i) {
		ArinBirthday2014Mission mission;
		mission.m_unread0 = packet.Decode4();
		mission.m_progress = packet.Decode4();
		mission.m_goal = (int32_t)packet.Decode4();
		mission.m_completed = packet.Decode1() == 1;
		mission.m_rewarded = packet.Decode1() == 1;
		m_missions.push_back(mission);
	}
}

// 0x920be0 (the factory creator: new 0x20, ctor 0x920a90(0xB0)); the KR
// CContentsDoc::Initialize inserts it as type 0xB0 under JP_PAKS.
IContentsDataContainer* JpCreateArinDocument(void) {
	return new CArinBirthDay2014Doc;
}

// --- CArinBirthDay2014Handler ---------------------------------------------------

// 0x920fd0
CArinBirthDay2014Handler::CArinBirthDay2014Handler(void) : m_unknown24(0) {
}

// 0x921060
CArinBirthDay2014Handler::~CArinBirthDay2014Handler(void) {
}

// 0x921360: the handler from the factory creator 0x9211c0 (the port's KR
// CGatewayHandler is no IObject, so `new`). Its Initialize is called here
// because the KR QueryHandler does not (the JP one calls the handler's vfn
// [9] 0x4bb3d0), as CQuestDocument::Initialize does.
bool CArinBirthDay2014Doc::Initialize(void) {
	m_pGatewayHandler = new CArinBirthDay2014Handler;
	if (m_pGatewayHandler != NULL) {
		m_pGatewayHandler->Initialize();
	}
	return true;
}

} // namespace _gateway
