// JP-only game code: CTutorialWidget (JpTutorialWidget.h), the lobby's
// TUTORIAL page, from the JP exe's bodies (node scripts/disasm.ts --jp
// 0x<va>).
#include "JpTutorialWidget.h"

#include <string>

#include "TaskManager.h"
#include "actor.h"
#include "frform.h"
#include "Packet.h"
#include "SharedDoc.h"
#include "shared/Localize.h"

#include "JpFrForms.h"
#include "RFTextManager.h"

extern int g_iRecvPacketType;  // JP 0xf02424

// 0xd19e90: Jp_LookupText's key for the gift mail Open sends with the
// tutorial reward (item 0x1a000006): the KR text CLobbyMain::OnToppage_Finish
// sends with message 0x3e (LobbyMain.cpp), "[Pangya gift mail] ...".
static const char s_jpTextKeyTutorialGiftMail[] =
	"[\xc7\xc7\xc7\xc9 \xbc\xb1\xb9\xb0 \xb8\xde\xc0\xcf]\n"
	"\n"
	"\xbe\xc8\xb3\xe7~ \xc0\xfc \xb4\xe7\xbd\xc5\xc0\xbb \xbf\xa9\xb1\xe2 \xb5\xa5\xb8\xae\xb0\xed \xbf\xc2 \xbd\xc3\xb0\xa3\xc0\xc7 \xbf\xe4\xc1\xa4 \xc7\xc7\xc7\xc9\xc0\xcc\xb6\xf3\xb0\xed \xc7\xd5\xb4\xcf\xb4\xd9. \xb9\xe6\xb1\xdd \xbf\xb9\xbb\xdb \xc4\xb3\xb8\xaf\xc5\xcd\xb8\xa6 \xbc\xb1\xc5\xc3\xc7\xdf\xb3\xd7\xbf\xe4. \xb0\xe8\xbc\xd3 \xc0\xdf \xba\xb8\xbb\xec\xc6\xec\xc1\xd6\xbc\xbc\xbf\xe4.\n"
	"\n"
	"\xbe\xc6~ \xb1\xd7\xb8\xae\xb0\xed \xc0\xcc\xb0\xc5 \xb9\xde\xc0\xb8\xbc\xbc\xbf\xe4. \xb4\xe7\xbd\xc5\xc0\xcc \xc6\xce\xbe\xdf\xbf\xa1 \xba\xb8\xb4\xd9 \xbd\xb1\xb0\xd4 \xc0\xfb\xc0\xc0\xc7\xcf\xb1\xe2 \xc0\xa7\xc7\xd8\xbc\xad \xb5\xe5\xb8\xae\xb4\xc2 \xc7\xe0\xbf\xee\xc0\xc7 \xb8\xf1\xb0\xc9\xc0\xcc\xc0\xd4\xb4\xcf\xb4\xd9.\n"
	"\xc0\xcc \xc7\xe0\xbf\xee\xc0\xc7 \xb8\xf1\xb0\xc9\xc0\xcc\xb4\xc2 \xb4\xe7\xbd\xc5\xc0\xcc \xb8\xb8\xb5\xe7 \xc4\xb3\xb8\xaf\xc5\xcd\xc0\xc7 \\c0xffff0000\\c\xc1\xa4\xc8\xae\xb5\xb5\\c0xff000000\\c\xb8\xa6 \xb3\xf4\xc7\xf4\xc1\xd6\xb0\xed \\c0xffff0000\\c\xc6\xce\xbe\xdf\xc0\xd3\xc6\xd1\xc6\xae\xc1\xb8\\c0xff000000\\c\xc0\xbb \xb3\xd0\xc7\xf4\xc1\xd6\xb4\xc2 \xbf\xaa\xc7\xd2\xc0\xbb \xc7\xd1\xb4\xe4\xb4\xcf\xb4\xd9.\n"
	"\xc7\xcf\xb3\xaa \xc1\xd6\xc0\xc7\xc7\xd2 \xb0\xcd\xc0\xba\xbf\xe4~ \xb4\xe7\xbd\xc5\xc0\xc7 \xbd\xc7\xb7\xc2\xc0\xcc \xbe\xee\xb4\xc0 \xc1\xa4\xb5\xb5 \xbe\xc8\xc1\xa4\xb5\xc7\xb4\xc2 \xb7\xb9\xba\xa7, \xc1\xef \\c0xffff0000\\c\xc7\xc1\xb7\xce" "E\\c0xff000000\\c\xb0\xa1 \xb5\xc7\xb8\xe9 \xc8\xbf\xb0\xfa\xb0\xa1 \xbe\xf8\xbe\xee\xc1\xf6\xb4\xcf\xb1\xee\xbf\xe4~ \xbe\xcb\xbe\xc6\xb5\xce\xbd\xc3\xb0\xed\xbf\xe4.\n"
	"\n"
	"\xb1\xd7\xb8\xae\xb0\xed \xb4\xeb\xc0\xfc\xb8\xf0\xb5\xe5\xbf\xa1 \xb0\xa1\xb1\xe2 \xc0\xfc\xbf\xa1 \xc7\xd1\xb9\xf8 \xc8\xa5\xc0\xda \xbf\xac\xbd\xc0\xc7\xcf\xb0\xed \xbd\xcd\xb4\xd9\xb8\xe9 \xc6\xa9\xc5\xe4\xb8\xae\xbe\xf3 \xb8\xf0\xb5\xe5\xbf\xa1 \xb5\xe9\xbe\xee\xb0\xa1\xba\xb8\xbc\xbc\xbf\xe4.\n"
	"\xc2\xf7\xb1\xd9\xc2\xf7\xb1\xd9 \xb8\xf0\xb5\xce \xb9\xe8\xbf\xec\xbd\xc3\xb8\xe9 \xbc\xb1\xb9\xb0\xb5\xb5 \xc1\xd8\xb4\xd9\xb0\xed \xc7\xcf\xb3\xd7\xbf\xe4. ^^\n"
	"\n"
	"\xb1\xd7\xb7\xb3 \xb4\xe3\xbf\xa1 \xb6\xc7 \xba\xc1\xbf\xe4. \xbe\xc8\xb3\xe7";

// 0xe15e80: the widget's name and its Fresh layout (two std::strings, one
// static object: initializer 0xcd45a0).
// lib: 0xcd45a0 (its dynamic initializer), 0xcfa300 (its atexit
// destructor), 0xc81e80 (the initializer's unwind funclet)
struct sTutorialWidgetInfo {
	std::string m_name;    // 0x0
	std::string m_layout;  // 0x1c (0xe15e9c, read by Load)
};
static sTutorialWidgetInfo s_tutorialWidgetInfo = { "CTutorialWidget", "tutorialmain" };

// lib: 0xcd4620 (m_RTTI's dynamic initializer, compiler-generated from
// this definition: WRTTI::WRTTI 0xa4c140, base CTaskWidget)
const WRTTI CTutorialWidget::m_RTTI("CTutorialWidget", &CTaskWidget::m_RTTI);

// 0x6b2bf0 (CTaskWidget::CTaskWidget 0x64f620, then the three vptrs)
CTutorialWidget::CTutorialWidget(void) {
}

// 0x6b2c30
void CTutorialWidget::Process(float dt) {
	CTaskWidget::Process(dt);
}

// 0x6b2c60: the factory function AddObjectFunctor registers.
static IObject* CTutorialWidgetMakeInstance(void) {
	return new CTutorialWidget;
}

// 0x6b2cd0 (the registrar's constructor; ObjectFactory 0xa4c0d0,
// AddObjectFunctor 0xa4bcf0). lib: 0xcd4640 (the static registrar's
// dynamic initializer, the constructor inlined)
struct __sCTutorialWidget {
	__sCTutorialWidget(void) {
		ObjectFactory().AddObjectFunctor(CTutorialWidgetMakeInstance, "CTutorialWidget");
	}
};
static __sCTutorialWidget s_registerCTutorialWidget;

// 0x6b2cf0 (0x6b2d10: the deleting destructor; 0x6b2c50: its FrCmdTarget
// thunk). The vptrs, then ~CTaskWidget 0x64f710.
CTutorialWidget::~CTutorialWidget(void) {
}

// 0x6b2d40: the form's Open is the JP [25] (+0x64) Open(cb, flag) with a
// null callback and flag 3. The rest is the KR CLobbyMain::OnToppage_Finish
// tail (LobbyMain.cpp) on the "Lobby" actor: JP CSharedDoc +0x10b24 is the
// KR m_paranNickChanged (JpLobbyGlue.h), +0x110ca the KR m_flag50F6, and the
// KR OpenEventPrizeForm(5 / 2) is Lobby message 0x43 with that kind.
void CTutorialWidget::Open(void) {
	if (m_pForm) {
		m_pForm->Open(NULL, 3);
		if (WSingleton<CSharedDoc>::Instance()->m_paranNickChanged) {
			MsgObject msg(0, 0x43, 0, 0, 0, 0, 0);
			if (IsLocalContent((localContentType_t)0x10)) {
				WSingleton<CSharedDoc>::Instance()->m_paranNickChanged = false;
				msg.m_param1 = 5;
				AfxGetTask()->GetActor("Lobby") << msg;
				// JP +0x110ca: the KR m_flag50F6 (protected; CTutorialWidget
				// is its friend under JP_PAKS, SharedDoc.h).
				if (WSingleton<CSharedDoc>::Instance()->m_flag50F6 == true) {
					AfxGetTask()->GetActor("Lobby") << MsgObject(0, 0x3e, 0x1a000006,
						(intptr_t)Jp_LookupText(s_jpTextKeyTutorialGiftMail), 0, 0, 0);
					WSingleton<CSharedDoc>::Instance()->m_flag50F6 = false;
				}
			} else {
				WSingleton<CSharedDoc>::Instance()->m_paranNickChanged = false;
				msg.m_param1 = 2;
				AfxGetTask()->GetActor("Lobby") << msg;
			}
		}
	}
	CTaskWidget::Open();
}

// 0x6b2ee0: the KR CTutorialTask::OnPacket (TutorialTask.cpp) on the widget,
// on JP packet ids (KR 0x11b -> 0x11f, 0x11c -> 0x120). JP CSharedDoc
// +0x10d50 is the KR m_tutorialData. 0x11f's TutorialMain message 0x151 is
// the task's main actor's 0x1bc (CTask vtable +0x30, SendMsgToMainActor);
// 0x120's TutorialMain message 0x153 (KR m_gradeGift) is a store into the
// contents container 0xa4 instead (the JP reads that container in 0x11f
// too and drops the result). Returns 1 when handled; 0 for other packets
// (the KR passes those to OnPacketCommon).
int CTutorialWidget::OnPacket(WReceivedPacket& packet) {
	packet.Reset();
	g_iRecvPacketType = packet.Decode2();

	switch (g_iRecvPacketType) {
	case 0x11f: {
		unsigned char index = packet.Decode1();
		unsigned char value = packet.Decode1();
		if (index < 3) {
			WSingleton<CSharedDoc>::Instance()->m_tutorialData[index] = packet.Decode4();
		} else {
			packet.DecodeBuffer(WSingleton<CSharedDoc>::Instance()->m_tutorialData, 12);
		}
		CContentsDoc::Instance()->GetContainer((localContentType_t)0xa4);
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1bc, index, value, 0, 0, 0));
		return 1;
	}
	case 0x120: {
		unsigned char gradeGift = packet.Decode1();
		CTutorialContainer* container = static_cast<CTutorialContainer*>(
			CContentsDoc::Instance()->GetContainer((localContentType_t)0xa4));
		container->SetGradeGift(gradeGift);
		return 1;
	}
	}
	return 0;
}

// 0x6b3020
void CTutorialWidget::Load(void) {
	m_layoutFile.assign(s_tutorialWidgetInfo.m_layout, 0, std::string::npos);
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0, (intptr_t)"TOPPAGE", 0, 0, 0, 0));
	CTaskWidget::Load();
}

// Port glue (no JP function: JP CContentsDoc::Initialize 0x915da0 inlines
// `new CTutorialContainer` (0x18 bytes, vtable 0xd3c5ec) at 0x915f9f and
// inserts it as type 0xa4 through 0x913c70(0xa4, container, false)). For
// the KR IContentsDoc.cpp to call under JP_PAKS without including the port
// header, as JpCreateLobbyContainer.
IContentsDataContainer* JpCreateTutorialContainer(void) {
	return new CTutorialContainer;
}

// 0x9079e0
void CTutorialContainer::SendMissionResult(int index, unsigned long missionBit, int played, int complete) {
	if (played == 1 && m_questReward) {
		m_questReward = 0;
		WSendPacket packet((enumClientPacket)0xae);
		packet.Encode1((unsigned char)complete);
		packet.Encode1((unsigned char)m_questCategory);
		packet.Encode4(missionBit);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// 0x907a80
bool CTutorialContainer::IsQuestSet(void) const {
	if (m_questCategory >= 0 && m_questNextIndex >= 0 && m_questIndex >= 0) {
		return true;
	}
	return false;
}

// Port glue (JpTutorialWidget.h): the JP CTutorial::MoveNextStep 0x7cd220
// call on its +0xdac container.
void JpTutorialContainer_SendMissionResult(int index, unsigned long missionBit, int played, int complete) {
	CTutorialContainer* container = static_cast<CTutorialContainer*>(
		CContentsDoc::Instance()->GetContainer((localContentType_t)0xa4));
	if (container) {
		container->SendMissionResult(index, missionBit, played, complete);
	}
}

// Port glue (JpTutorialWidget.h): CSharedDoc::m_value4D98 for
// JpTutorialMain (JP +0x10d5c).
unsigned long CTutorialWidget::GetTutorialQuest(void) {
	return WSingleton<CSharedDoc>::Instance()->m_value4D98;
}

void CTutorialWidget::SetTutorialQuest(unsigned long quest) {
	WSingleton<CSharedDoc>::Instance()->m_value4D98 = quest;
}
