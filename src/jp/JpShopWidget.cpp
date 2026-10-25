// JP-only game code: CShopWidget (JpShopWidget.h), the lobby's shop page,
// from the JP exe's bodies (node scripts/disasm.ts --jp 0x<va>).
#include "NetworkSystem.h"
#include "JpShopWidget.h"

#include <string.h>
#include <string>

#include "TaskManager.h"
#include "actor.h"
#include "frform.h"
#include "wview.h"
#include "Packet.h"
#include "SharedDoc.h"
#include "commonutil.h"
#include "RealMyRoomData.h"
#include "shared/Localize.h"

#include "JpFrForms.h"
#include "RFTextManager.h"

extern int g_iRecvPacketType;  // JP 0xf02424

// JP CSharedDoc +0x106fc (a byte, no KR counterpart): set to 1 after a
// successful purchase (packet 0x68 result 0). Declared only.
void JpSharedDoc_SetShopPurchased(bool purchased);
// 0x5fbe40 on RFLobbyDesktop::s_instance (0xe78c38) when there is one
// (RefreshOnShow is protected; RFLobbyDesktop.h cannot be included next to
// the KR headers: R_OK). Declared only.
void JpLobbyDesktop_RefreshOnShow(void);

// The JP inlines this at every result case: the task's main actor
// (CTask vtable +0x30, SendMsgToMainActor) gets message 0x23 with the JP
// game text of a Korean text key (Jp_LookupText 0xb4eb80).
static void ShopWidget_PostText(const char* key) {
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText(key), 0, 0, 0, 0));
}

// 0xe12e58: the widget's name and its Fresh layout (two std::strings, one
// static object: initializer 0xcd1e90).
// lib: 0xcd1e90 (its dynamic initializer), 0xcf9ec0 (its atexit
// destructor), 0xc78770 (the initializer's unwind funclet)
struct sTaskWidgetInfo {
	std::string m_name;    // 0x0
	std::string m_layout;  // 0x1c (0xe12e74, read by Load)
};
static sTaskWidgetInfo s_shopWidgetInfo = { "CShopWidget", "shopmain" };

// lib: 0xcd1f10 (m_RTTI's dynamic initializer, compiler-generated from
// this definition: WRTTI::WRTTI 0xa4c140, base CTaskWidget)
const WRTTI CShopWidget::m_RTTI("CShopWidget", &CTaskWidget::m_RTTI);

// 0x5e0ca0 (CTaskWidget::CTaskWidget 0x64f620, then the three vptrs)
CShopWidget::CShopWidget(void) {
}

// 0x5e0ce0
void CShopWidget::Process(float dt) {
	CTaskWidget::Process(dt);
}

// 0x5e0d70: the factory function AddObjectFunctor registers.
static IObject* CShopWidgetMakeInstance(void) {
	return new CShopWidget;
}

// 0x5e0de0 (the registrar's constructor; ObjectFactory 0xa4c0d0,
// AddObjectFunctor 0xa4bcf0)
struct __sCShopWidget {
	__sCShopWidget(void) {
		ObjectFactory().AddObjectFunctor(CShopWidgetMakeInstance, "CShopWidget");
	}
};
static __sCShopWidget s_registerCShopWidget;

// 0x5e0e00 (0x5e0e50: the deleting destructor; 0x5e0d60: its FrCmdTarget
// thunk). The vptrs, then ~CTaskWidget 0x64f710.
CShopWidget::~CShopWidget(void) {
}

// 0x5e0e80: 800.0 is the double 0xd00028, 0.3 the double 0xd00020 (the
// float 0.3f widened). The JP reads the form's rect (+0x34..+0x40) and
// moves it through vfn +0x8c with the whole rect; KR's MoveWindow takes its
// origin. The form's Open is the JP [25] (+0x64) Open(cb, flag) with a null
// callback.
void CShopWidget::Open(void) {
	if (m_pForm) {
		if (AfxGetTask()->GetActor("Lobby")) {
			WRect rect = m_pForm->GetRect();
			rect.x = (float)((g_view->GetWidth() - 800.0) * 0.3f + rect.x);
			m_pForm->MoveWindow(WPoint(rect.x, rect.y));
		}
		m_pForm->Open(NULL, 1);
	}
	CTaskWidget::Open();
}

// 0x5e0fd0
void CShopWidget::Load(void) {
	m_layoutFile.assign(s_shopWidgetInfo.m_layout, 0, std::string::npos);
	CTaskWidget::Load();
}

// 0x5e0ff0: the KR CShopTask::OnPacket (ShopTask.cpp) on the widget. The
// messages go to the task's main actor instead of "Shop", the texts through
// Jp_LookupText; the JP numbers are the KR ones + 2 for the packets (0x66
// buy -> 0x68, 0x68 gift -> 0x6a, 0xb4 -> 0xb6, 0x105 -> 0x107, 0x125 ->
// 0x12d, 0x199 -> 0x194) and the KR result codes above 0x29 less 1, with a
// new last code 0x35 (message 0xe9 with a dword). Every buy / gift result is
// followed by message 0xea. Returns 1 when handled; 0 for the caddie
// upgrade 0x194 and unknown packets (the KR passes those to
// OnPacketCommon).
int CShopWidget::OnPacket(WReceivedPacket& packet) {
	packet.Reset();
	g_iRecvPacketType = packet.Decode2();
	int handled = 1;

	switch (g_iRecvPacketType) {
	case 0x6A: {
		// Gift result (KR 0x68).
		IActor* pMain = AfxGetTask()->GetMainActor();
		pMain << MsgObject(0, 2, 0, 0, 0, 0, 0);
		pMain << MsgObject(0, 0xE2, 1, 0, 0, 0, 0);

		switch (packet.Decode4()) {
		case 0x0:
			WSingleton<CSharedDoc>::Instance()->m_myPang = packet.Decode8();        // JP +0x91e
			WSingleton<CSharedDoc>::Instance()->m_myBonusCash = packet.Decode8();  // JP +0x3870
			WSingleton<CSharedDoc>::Instance()->RefreshAllCookie();                 // JP 0x6332b0

			// "Gift sent."
			ShopWidget_PostText("\xbc\xb1\xb9\xb0\xc7\xcf\xb1\xe2\xb0\xa1 \xbf\xcf\xb7\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9");
			JpLobbyDesktop_RefreshOnShow();  // 0x5fbe40 when 0xe78c38 is set

			if (IsLocalContent((localContentType_t)0x61)) {  // JP id
				if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_MSN)) {
					WSendPacket send(0x29);
					send.Encode4(MyUID());                                                       // JP +0x8cb
					send.Encode4(WSingleton<CSharedDoc>::Instance()->m_lastGiftReceiverUID);  // JP +0x10d7c
					send.Send(SEND_TO_MSN);
				}
			}
			break;
		case 0x2:
			ShopWidget_PostText("\xb1\xdd\xbe\xd7\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9");
			break;
		case 0x3:
			ShopWidget_PostText("\xc0\xdf\xb8\xf8\xb5\xc8 \xbb\xf3\xc7\xb0 \xc4\xda\xb5\xe5\xc0\xd4\xb4\xcf\xb4\xd9");
			break;
		case 0x1:
			ShopWidget_PostText("\xbc\xb1\xb9\xb0\xc7\xcf\xb1\xe2\xb0\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9");
			break;
		case 0x4:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
			ShopWidget_PostText("\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc \xc0\xcc\xb9\xcc \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9");
			break;
		case 0x28:
			ShopWidget_PostText("\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc \xc0\xcc\xb9\xcc \xbf\xec\xc6\xed\xc7\xd4\xbf\xa1 \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x2A:
			ShopWidget_PostText("\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc \xc0\xcc\xb9\xcc \xb5\xb9\xc7\xc7\xb4\xcf\xc0\xc7 \xb1\xdd\xb0\xed\xbf\xa1 \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x8:
			ShopWidget_PostText("\xbc\xb1\xb9\xb0\xc7\xcf\xb1\xe2\xb8\xa6 \xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb0\xe8\xc1\xa4\xc0\xd4\xb4\xcf\xb4\xd9.");
			break;
		case 0x7:
			ShopWidget_PostText("\xc1\xd6\xb9\xce\xb9\xf8\xc8\xa3\xb0\xa1 \xc0\xdf\xb8\xf8 \xc0\xd4\xb7\xc2\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9");
			break;
		case 0xB: {
			_SYSTEMTIME serverTime;
			packet.DecodeBuffer(&serverTime, 0x10);
			WSingleton<CSharedDoc>::Instance()->SetServerTime(serverTime);  // JP 0x643e60
			ShopWidget_PostText("\xc6\xc7\xb8\xc5\xb1\xe2\xb0\xa3\xc0\xbb \xc8\xae\xc0\xce\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.");
			break;
		}
		case 0x9: {
			_SYSTEMTIME serverTime;
			packet.DecodeBuffer(&serverTime, 0x10);
			WSingleton<CSharedDoc>::Instance()->SetServerTime(serverTime);
			ShopWidget_PostText("\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xb1\xe2\xb0\xa3\xc0\xbb \xc8\xae\xc0\xce\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.");
			break;
		}
		case 0xC:
			ShopWidget_PostText("\xbb\xf3\xb4\xeb\xb9\xe6\xbf\xa1\xb0\xd4 \xc7\xd8\xb4\xe7 \xc4\xb3\xb5\xf0\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9");
			break;
		case 0xE:
			ShopWidget_PostText("\xbb\xf3\xb4\xeb\xb9\xe6\xbf\xa1\xb0\xd4 \xc7\xd8\xb4\xe7 \xc4\xb3\xb8\xaf\xc5\xcd\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9");
			break;
		case 0x10:
			ShopWidget_PostText("\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc \xb1\xe2\xb0\xa3\xc1\xa6 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbb\xe7\xbf\xeb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9");
			break;
		case 0x11:
			ShopWidget_PostText("\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc \xb0\xb9\xbc\xf6\xc1\xa6 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbb\xe7\xbf\xeb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9");
			break;
		case 0x12:
			ShopWidget_PostText("\xbe\xc6\xc0\xcc\xc5\xdb \xba\xb8\xc0\xaf \xc7\xd1\xb5\xb5\xb8\xa6 \xc3\xca\xb0\xfa\xc7\xd5\xb4\xcf\xb4\xd9.");
			break;
		case 0x15:
			ShopWidget_PostText("\xc7\xd1\xb9\xf8\xbf\xa1 \xb3\xca\xb9\xab \xb8\xb9\xc0\xba \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb1\xb8\xc0\xd4\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x17:
			ShopWidget_PostText("\xba\xb8\xc0\xaf\xc7\xcf\xb0\xed \xc0\xd6\xb4\xc2 \xc4\xed\xc5\xb0\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.");
			break;
		case 0x18:
			ShopWidget_PostText("\xc4\xed\xc5\xb0\xc1\xa4\xba\xb8\xb8\xa6 \xbe\xf7\xb5\xa5\xc0\xcc\xc6\xae\xc7\xcf\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x1C:
			ShopWidget_PostText("\xb1\xb8\xc0\xd4\xc7\xcf\xbd\xc5 \xbc\xb1\xb9\xb0\xc0\xbb \xc0\xfa\xc0\xe5\xc7\xcf\xc1\xf6 \xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x1E:
			ShopWidget_PostText("\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xc7 \xc1\xa4\xba\xb8\xb8\xa6 \xc1\xb6\xc8\xb8\xc7\xcf\xb4\xc2\xb5\xa5 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x1F:
			ShopWidget_PostText("\xb1\xb8\xc0\xd4\xc7\xcf\xb7\xc1\xb0\xed \xc7\xcf\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x20:
			ShopWidget_PostText("\xbc\xb1\xc5\xc3\xc7\xcf\xbd\xc5 \xc4\xed\xc6\xf9\xc0\xbb \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9..");
			break;
		case 0x21:
			ShopWidget_PostText("\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xc7 \xbc\xb1\xb9\xb0\xc7\xd4\xc0\xcc \xb0\xa1\xb5\xe6 \xc2\xf7 \xb4\xf5 \xc0\xcc\xbb\xf3 \xbc\xb1\xb9\xb0\xc0\xbb \xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x13:
			ShopWidget_PostText("\xc6\xc7\xb8\xc5\xc7\xcf\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9.");
			break;
		case 0x19:
			ShopWidget_PostText("\xc4\xed\xc5\xb0\xb8\xa6 \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xb4\xd9\xb4\xc2 \xc1\xa4\xba\xb8\xb8\xa6 \xc3\xa3\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x2D:
			ShopWidget_PostText("\xc3\xa4\xb3\xce\xb8\xb5\xbc\xad\xba\xf1\xbd\xba\xb0\xa1 \xc1\xbe\xb7\xe1\xb5\xc7\xbe\xee \xb4\xf5\xc0\xcc\xbb\xf3 \xb1\xb8\xb8\xc5\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9..");
			break;
		case 0x22:
			ShopWidget_PostText("\xbc\xb1\xb9\xb0\xc0\xbb \xc7\xcf\xb7\xc1\xb4\xc2 \xc0\xaf\xc0\xfa\xb4\xc2 \xc0\xcc \xbc\xb1\xb9\xb0\xc0\xbb \xb9\xde\xc0\xb8\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x2B:
			ShopWidget_PostText("\xbc\xb1\xb9\xb0\xc0\xbb \xb9\xde\xb4\xc2 \xc0\xaf\xc0\xfa\xc0\xc7 \xb7\xb9\xba\xa7\xc0\xcc \xb3\xb7\xbe\xc6 \xbc\xb1\xb9\xb0\xc7\xd2\xbc\xf6\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x2C:
			ShopWidget_PostText("\xbc\xb1\xb9\xb0\xc0\xbb \xb9\xde\xb4\xc2 \xc0\xaf\xc0\xfa\xc0\xc7 \xb7\xb9\xba\xa7\xc0\xcc \xb3\xf4\xbe\xc6 \xbc\xb1\xb9\xb0 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x33:
			ShopWidget_PostText("\xc0\xda\xb1\xe2 \xc0\xda\xbd\xc5\xbf\xa1\xb0\xd4\xb4\xc2 \xbc\xb1\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x32:
			ShopWidget_PostText("\xbc\xb1\xb9\xb0\xc7\xd2 \xc0\xaf\xc0\xfa\xc0\xc7 \xc1\xa4\xba\xb8\xb8\xa6 \xc3\xa3\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4");
			break;
		case 0x34:
			ShopWidget_PostText("\xc7\xf6\xc0\xe7 \xb8\xf0\xb5\xe7 \xc4\xb3\xbd\xac\xbf\xa1 \xb4\xeb\xc7\xd1 \xc1\xb6\xc8\xb8 \xb9\xd7 \xbb\xe7\xbf\xeb\xc0\xcc \xc1\xa4\xc1\xf6\xb5\xc7\xbe\xee \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x35: {
			unsigned long value = packet.Decode4();
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xE9, value, 0, 0, 0, 0));
			break;
		}
		}
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xEA, 0, 0, 0, 0, 0));
		break;
	}

	case 0x68: {
		// Buy result (KR 0x66).
		IActor* pMain = AfxGetTask()->GetMainActor();
		pMain << MsgObject(0, 2, 0, 0, 0, 0, 0);
		pMain << MsgObject(0, 0xE2, 1, 0, 0, 0, 0);

		switch (packet.Decode4()) {
		case 0x2E: {
			unsigned long typeId = packet.Decode4();
			unsigned long serial = packet.Decode4();
			unsigned long count = packet.Decode4();
			unsigned long bonus = packet.Decode4();

			// JP +0x375c
			std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin();
			for (; it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {
				if (it->m_serial == serial && it->m_typeId == typeId) {
					it->m_count = (short)count;
					break;
				}
			}

			if (it == WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end()) {
				sItemInfo item;
				memset(&item, 0, sizeof(item));
				item.m_typeId = typeId;
				item.m_serial = serial;
				item.m_flags28.flags01 |= 2;
				item.m_count = (short)count;
				WSingleton<CSharedDoc>::Instance()->m_itemInfoList.push_back(item);
			}

			// "Purchase completed.\nYou received bonus %s x%d." (0xd12640; the
			// JP looks the formatted text up, message 0x2bc = KR 0x241).
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x2BC, (intptr_t)Jp_LookupText(MakeStr("\xc1\xa4\xbb\xf3\xc0\xfb\xc0\xb8\xb7\xce \xb1\xb8\xb8\xc5\xb0\xa1 \xbf\xcf\xb7\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.\n\xba\xb8\xb3\xca\xbd\xba %s %d\xb0\xb3\xb8\xa6 \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.", ItemManager()->GetItemName(typeId), bonus)), 0, 0, 0, 0));
			break;
		}
		case 0x0:
			WSingleton<CSharedDoc>::Instance()->m_myPang = packet.Decode8();        // JP +0x91e
			WSingleton<CSharedDoc>::Instance()->m_myBonusCash = packet.Decode8();  // JP +0x3870
			WSingleton<CSharedDoc>::Instance()->RefreshAllCookie();

			// "Purchase completed."
			ShopWidget_PostText("\xc1\xa4\xbb\xf3\xc0\xfb\xc0\xb8\xb7\xce \xb1\xb8\xb8\xc5\xb0\xa1 \xbf\xcf\xb7\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
			JpLobbyDesktop_RefreshOnShow();  // 0x5fbe40 when 0xe78c38 is set
			JpSharedDoc_SetShopPurchased(true);  // JP +0x106fc = 1
			break;
		case 0x2:
			// 0xcff308 (a shared literal)
			ShopWidget_PostText("\xba\xb8\xc0\xaf\xc7\xcf\xb0\xed \xc0\xd6\xb4\xc2 \xc6\xce\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9");
			break;
		case 0x3:
			ShopWidget_PostText("\xc0\xdf\xb8\xf8\xb5\xc8 \xbb\xf3\xc7\xb0 \xc4\xda\xb5\xe5\xc0\xd4\xb4\xcf\xb4\xd9");
			break;
		case 0x1:
			ShopWidget_PostText("\xb1\xb8\xb8\xc5\xb0\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9");
			break;
		case 0x4:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
			ShopWidget_PostText("\xc0\xcc\xb9\xcc \xb1\xb8\xb8\xc5\xc7\xd1 \xb9\xb0\xb0\xc7\xc0\xd4\xb4\xcf\xb4\xd9.");
			break;
		case 0x28:
			ShopWidget_PostText("\xc0\xcc\xb9\xcc \xbf\xec\xc6\xed\xc7\xd4\xbf\xa1 \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x2A:
			ShopWidget_PostText("\xc0\xcc\xb9\xcc \xb5\xb9\xc7\xc7\xb4\xcf\xc0\xc7 \xba\xf1\xb9\xd0\xb1\xdd\xb0\xed\xbf\xa1 \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0xB: {
			_SYSTEMTIME serverTime;
			packet.DecodeBuffer(&serverTime, 0x10);
			WSingleton<CSharedDoc>::Instance()->SetServerTime(serverTime);
			ShopWidget_PostText("\xc6\xc7\xb8\xc5\xb1\xe2\xb0\xa3\xc0\xbb \xc8\xae\xc0\xce\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.");
			break;
		}
		case 0x9: {
			_SYSTEMTIME serverTime;
			packet.DecodeBuffer(&serverTime, 0x10);
			WSingleton<CSharedDoc>::Instance()->SetServerTime(serverTime);
			ShopWidget_PostText("\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xb1\xe2\xb0\xa3\xc0\xbb \xc8\xae\xc0\xce\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.");
			break;
		}
		case 0x10:
			ShopWidget_PostText("\xb1\xe2\xb0\xa3\xc1\xa6 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbb\xe7\xbf\xeb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9");
			break;
		case 0x11:
			ShopWidget_PostText("\xb0\xb9\xbc\xf6\xc1\xa6 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbb\xe7\xbf\xeb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9");
			break;
		case 0x15:
			ShopWidget_PostText("\xc7\xd1\xb9\xf8\xbf\xa1 \xb3\xca\xb9\xab \xb8\xb9\xc0\xba \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb1\xb8\xc0\xd4\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x17:
			ShopWidget_PostText("\xba\xb8\xc0\xaf\xc7\xcf\xb0\xed \xc0\xd6\xb4\xc2 \xc4\xed\xc5\xb0\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.");
			break;
		case 0x18:
			ShopWidget_PostText("\xc4\xed\xc5\xb0\xb8\xa6 \xbe\xf7\xb5\xa5\xc0\xcc\xc6\xae\xc7\xcf\xb4\xc2\xb5\xa5 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x12:
			ShopWidget_PostText("\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb4\xf5 \xc0\xcc\xbb\xf3 \xb1\xb8\xc0\xd4\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x23:
			ShopWidget_PostText("\xbb\xf3\xc1\xa1\xc0\xcc \xc8\xa5\xc0\xe2\xc7\xcf\xbf\xa9 \xc1\xf6\xb1\xdd\xc0\xba \xb1\xb8\xc0\xd4\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xe1\xbd\xc3 \xc8\xc4 \xc0\xcc\xbf\xeb\xc7\xd8\xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.");
			break;
		case 0x13:
			ShopWidget_PostText("\xc6\xc7\xb8\xc5\xc7\xcf\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9.");
			break;
		case 0x2D:
			ShopWidget_PostText("\xc3\xa4\xb3\xce\xb8\xb5\xbc\xad\xba\xf1\xbd\xba\xb0\xa1 \xc1\xbe\xb7\xe1\xb5\xc7\xbe\xee \xb4\xf5\xc0\xcc\xbb\xf3 \xb1\xb8\xb8\xc5\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x22:
			ShopWidget_PostText("\xc0\xcc \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb4\xf5 \xc0\xcc\xbb\xf3 \xba\xb8\xc0\xaf\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x2B:
		case 0x2C:
			ShopWidget_PostText("\xc7\xf6\xc0\xe7 \xb7\xb9\xba\xa7\xb7\xce\xb4\xc2 \xb1\xb8\xc0\xd4\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9");
			break;
		case 0x34:
			ShopWidget_PostText("\xc7\xf6\xc0\xe7 \xb8\xf0\xb5\xe7 \xc4\xb3\xbd\xac\xbf\xa1 \xb4\xeb\xc7\xd1 \xc1\xb6\xc8\xb8 \xb9\xd7 \xbb\xe7\xbf\xeb\xc0\xcc \xc1\xa4\xc1\xf6\xb5\xc7\xbe\xee \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x35: {
			unsigned long value = packet.Decode4();
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xE9, value, 0, 0, 0, 0));
			break;
		}
		}
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xEA, 0, 0, 0, 0, 0));
		break;
	}

	case 0xB6:
		// KR 0xb4 (message 0xc1 to "Shop").
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xE3, packet.Decode4(), 0, 0, 0, 0));
		break;

	case 0xB0:
		// KR 0xae: nothing to do.
		break;

	case 0x107:
		// KR 0x105 (message 0x1ff to "Shop").
		if (packet.Decode1() == 0)
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x27C, 0, 0, 0, 0, 0));
		break;

	case 0x12D:
		// KR 0x125: the my-room furniture list.
		if (IsLocalContent((localContentType_t)0x57)) {  // JP id
			unsigned long result = packet.Decode4();
			if (result == 1) {
				int count = packet.Decode2();
				for (int i = 0; i < count; i++) {
					sFurniture_List furniture;
					memset(&furniture, 0, sizeof(furniture));
					packet.DecodeBuffer(&furniture, 0x1B);

					// JP +0x37d4
					std::list<sFurniture_List>::iterator it;
					for (it = WSingleton<CSharedDoc>::Instance()->m_furnitureList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_furnitureList.end(); ++it) {
						if (it->m_id == furniture.m_id)
							break;
					}

					if (it == WSingleton<CSharedDoc>::Instance()->m_furnitureList.end())
						WSingleton<CSharedDoc>::Instance()->AddFurniture(furniture);  // JP 0x640620
				}
			} else {
				// KR: the "RealMyRoom" actor's 0x228.
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x2A6, 0x12D, result, 0, 0, 0));
			}
		}
		break;

	case 0x194:
		// KR 0x199: the caddie upgrade result; not handled (0) afterwards,
		// as the KR passes it on to OnPacketCommon.
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));

		switch (packet.Decode4()) {
		case 0x0: {
			unsigned long caddieId = packet.Decode4();
			unsigned long equipped = packet.Decode4();
			sCaddieInfo caddie;
			packet.DecodeBuffer(&caddie, 0x19);

			// JP +0x3714 (find 0x507c70, erase 0x587260)
			std::map<unsigned int, sCaddieInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_caddieMap.find(caddieId);
			if (it != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end())
				WSingleton<CSharedDoc>::Instance()->m_caddieMap.erase(it);

			WSingleton<CSharedDoc>::Instance()->AddCaddie(&caddie);     // JP 0x642870
			WSingleton<CSharedDoc>::Instance()->m_myCaddieId = equipped; // JP +0xa26
			// "Upgrade succeeded." (message 0x2cb = KR 0x24f)
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x2CB, (intptr_t)Jp_LookupText("\xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xb8\xa6 \xbc\xba\xb0\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9."), caddie.typeId, 0, 0, 0));
			break;
		}
		case 0x1:
			ShopWidget_PostText("\xc7\xd8\xb4\xe7 \xc4\xb3\xb5\xf0\xb8\xa6 \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x2:
			ShopWidget_PostText("\xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xb8\xa6 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 0x3:
			ShopWidget_PostText("\xbe\xcb\xbc\xf6\xbe\xf8\xb4\xc2 \xbf\xc0\xb7\xf9\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		}
		handled = 0;
		break;

	default:
		handled = 0;
		break;
	}

	return handled;
}
