// Original: d:\build\custom\temp\code\client\projectg\gatewayactor.cpp
#include "GatewayActor.h"
#include "ObjectFactory.h"
#include "gatewayinterface.h"
#include "icontentsdoc.h"
#include "Exception.h"

// Task class names, "CLobbyTask", "CShopTask", "CRealMyRoomTask",
// "ntAvatarChatTask", "CGolfTask" (unnamed data, not cut yet).
extern "C" const char* data_a9df60[5];

// gatewayactor.cpp:20 (run-time type and factory registration of CGatewayActor)
const WRTTI _gateway::CGatewayActor::m_RTTI("CGatewayActor", &IActor::m_RTTI);

namespace _gateway {

struct __impCGatewayActor {
	__impCGatewayActor(void) {
		ObjectFactory().AddObjectFunctor(CGatewayActorMakeInstance, "CGatewayActor");
		ActorFactory().AddActorClass("CGatewayActor");
	}
};
static __impCGatewayActor s_impCGatewayActor;

// gatewayactor.cpp:20
IObject* CGatewayActorMakeInstance(void) {
	return new CGatewayActor;
}

}

// gatewayactor.cpp:23
_gateway::CGatewayActor::CGatewayActor(void) {
	m_creators.insert(std::make_pair(data_a9df60[0], &CGatewayActor::CreateLobbyHandler));
	m_creators.insert(std::make_pair(data_a9df60[1], &CGatewayActor::CreateShopHandler));
	m_creators.insert(std::make_pair(data_a9df60[2], &CGatewayActor::CreateMyRoomHandler));
	m_creators.insert(std::make_pair(data_a9df60[3], &CGatewayActor::CreateAvatarChatHandler));
	m_creators.insert(std::make_pair(data_a9df60[4], &CGatewayActor::CreateGolfHandler));
}

// gatewayactor.cpp:32
_gateway::CGatewayActor::~CGatewayActor(void) {
}

// gatewayactor.cpp:37
void _gateway::CGatewayActor::OnDestroy(void) {
	m_creators.clear();
}

// gatewayactor.cpp:42
void _gateway::CGatewayActor::HandleMsg(const MsgObject& msg) {
	taskMsg_t msgId = (taskMsg_t)msg.m_msg;
	switch (msgId) {
	case 0x2712:
		CreateHandler(msg.m_param1);
		return;
	}

	HandlerMap::iterator it;
	it = m_handlers.find(msgId);
	if (it == m_handlers.end()) {
		return;
	}

	it->second->Execute(msg);
}

// gatewayactor.cpp:67
bool _gateway::CGatewayActor::CreateHandler(intptr_t name) {
	CreatorMap::iterator it;
	it = m_creators.find(std::string(reinterpret_cast<const char*>(name)));
	if (it == m_creators.end()) {
		return false;
	}

	(this->*it->second)();
	return true;
}

// gatewayactor.cpp:84
bool _gateway::CGatewayActor::InsertHandler(taskMsg_t msg, CGatewayHandler* pHandler) {
	m_handlers.insert(HandlerMap::value_type(msg, pHandler));
	return true;
}

// gatewayactor.cpp:92
void _gateway::CGatewayActor::QueryHandler(localContentType_t type, taskMsg_t msg) {
#ifdef JP_PAKS
	// JP 0x91e850 also accepts the quest document (0xAC, CQuestDocument)
	// and the 2014 Arin event document (0xB0, CArinBirthDay2014Doc).
	if ((type < 0x9D || type > 0xA0) && type != 0xAC && type != 0xB0) {
#else
	if (type < 0x9D || type > 0xA0) {
#endif
		throw WAppException("Passed Unknown doctype!!\nCode[0]");
	}

	IContentsDataContainer* pDoc;
	if (!WSingleton<CContentsDoc>::Instance()->GetContainer(type, pDoc)) {
		throw WAppException("Need Doc Object!!\nCode[1]");
	}

	CGatewayHandler* pHandler = pDoc->GetGatewayHandler();
	if (!pHandler) {
		throw WAppException("Need Gateway Handler!!\nCode[2]");
	}

	pHandler->SetDocument(pDoc);
	InsertHandler(msg, pHandler);
}

// gatewayactor.cpp:129
void _gateway::CGatewayActor::CreateLobbyHandler(void) {
	QueryHandler((localContentType_t)0x9D, (taskMsg_t)0x26D);
	QueryHandler((localContentType_t)0x9E, (taskMsg_t)0x26F);
	QueryHandler((localContentType_t)0xA0, (taskMsg_t)0x276);
#ifdef JP_PAKS
	// JP registry 0x91ecf0: the quest document 0xAC, message 0x753A, task
	// flags 0x1F (every task; the port wires the lobby and the avatar chat).
	QueryHandler((localContentType_t)0xAC, (taskMsg_t)0x753A);
	// The Arin event document 0xB0, message 0x753F, task flags 1: the
	// lobby task only (ctor 0x91fc40: "CLobbyTask" 1, "CShopTask" 2,
	// "CRealMyRoomTask" 4, "ntAvatarChatTask" 8, "CGolfTask" 0x10).
	QueryHandler((localContentType_t)0xB0, (taskMsg_t)0x753F);
#endif
}

// gatewayactor.cpp:141
void _gateway::CGatewayActor::CreateAvatarChatHandler(void) {
	QueryHandler((localContentType_t)0x9D, (taskMsg_t)0x26D);
	QueryHandler((localContentType_t)0x9F, (taskMsg_t)0x273);
#ifdef JP_PAKS
	QueryHandler((localContentType_t)0xAC, (taskMsg_t)0x753A);
#endif
}

// gatewayactor.cpp:149
void _gateway::CGatewayActor::CreateMyRoomHandler(void) {
	QueryHandler((localContentType_t)0x9D, (taskMsg_t)0x26D);
	QueryHandler((localContentType_t)0xA0, (taskMsg_t)0x276);
}

// gatewayactor.cpp:157
void _gateway::CGatewayActor::CreateShopHandler(void) {
	QueryHandler((localContentType_t)0x9D, (taskMsg_t)0x26D);
}

// gatewayactor.cpp:162
void _gateway::CGatewayActor::CreateGolfHandler(void) {
	QueryHandler((localContentType_t)0x9E, (taskMsg_t)0x26F);
}
