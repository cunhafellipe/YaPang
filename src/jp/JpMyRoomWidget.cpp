// JP-only game code: CMyRoomWidget (JpMyRoomWidget.h), the lobby's MY ROOM
// page, from the JP exe's bodies (node scripts/disasm.ts --jp 0x<va>).
#include "NetworkSystem.h"
#include "JpMyRoomWidget.h"

#include <algorithm>
#include <list>
#include <stdio.h>
#include <string.h>
#include <string>

#include "ItemStorage.h"
#include "TaskManager.h"
#include "actor.h"
#include "frform.h"
#include "fresh.h"
#include "wview.h"
#include "Packet.h"
#include "SharedDoc.h"
#include "commonutil.h"
#include "shared/Localize.h"

#include "JpFrForms.h"
#include "JpRealMyRoomMain.h"
#include "RFTextManager.h"

extern int g_iRecvPacketType;  // JP 0xf02424

// 0x5fbe40 on RFLobbyDesktop::s_instance (0xe78c38) when there is one
// (RFLobbyDesktop.cpp).
void JpLobbyDesktop_RefreshOnShow(void);
// 0x540a10 (JpLobbyGlue.h): the game's string table text for an id.
const char* Jp_GetTableText(int id);

// 0xe0f64c: the widget's name and its Fresh layout (two std::strings, one
// static object: initializer 0xcd1570).
// lib: 0xcd1570 (its dynamic initializer), 0xcf9d40 (its atexit
// destructor), 0xc726d0 (the initializer's unwind funclet)
struct sMyRoomWidgetInfo {
	std::string m_name;    // 0x0
	std::string m_layout;  // 0x1c (0xe0f668, read by Load)
};
static sMyRoomWidgetInfo s_myRoomWidgetInfo = { "CMyRoomWidget", "realmyroom" };

// Port only (G11): the "realmyroom" form is created by name through
// JpFrFormRegistry, and JpRealMyRoomMain.o defines nothing else the game
// references, so the static pg_refresh_game archive would drop it with its
// registrar and Load would get NULL. This reference keeps it linked (this
// object is kept by JpTaskPage.cpp's g_jpTaskPageRTTIs). The JP links it
// directly.
extern FrWnd* (* const g_jpMyRoomFormCreator)(const char* name, FrWndManager* pManager);
FrWnd* (* const g_jpMyRoomFormCreator)(const char* name, FrWndManager* pManager) = &JpRealMyRoomMain_Create;

// lib: 0xcd15f0 (m_RTTI's dynamic initializer, compiler-generated from
// this definition: WRTTI::WRTTI 0xa4c140, base CTaskWidget)
const WRTTI CMyRoomWidget::m_RTTI("CMyRoomWidget", &CTaskWidget::m_RTTI);

// 0x5a7010 (CTaskWidget::CTaskWidget 0x64f620, then the three vptrs)
CMyRoomWidget::CMyRoomWidget(void) {
}

// 0x5a7040
void CMyRoomWidget::Process(float dt) {
	CTaskWidget::Process(dt);
}

// 0x5a74a0: the factory function AddObjectFunctor registers.
static IObject* CMyRoomWidgetMakeInstance(void) {
	return new CMyRoomWidget;
}

// 0x5a7510 (the registrar's constructor; ObjectFactory 0xa4c0d0,
// AddObjectFunctor 0xa4bcf0). lib: 0xcd1610 (the static registrar's dynamic
// initializer, the constructor inlined)
struct __sCMyRoomWidget {
	__sCMyRoomWidget(void) {
		ObjectFactory().AddObjectFunctor(CMyRoomWidgetMakeInstance, "CMyRoomWidget");
	}
};
static __sCMyRoomWidget s_registerCMyRoomWidget;

// 0x5a7790: the same body as CShopWidget::Open 0x5e0e80. 800.0 is the
// double 0xd00028, 0.3 the double 0xd00020 (the float 0.3f widened). The JP
// reads the form's rect (+0x34..+0x40) and moves it through vfn +0x8c with
// the whole rect; KR's MoveWindow takes its origin. The form's Open is the JP
// [25] (+0x64) Open(cb, flag) with a null callback.
void CMyRoomWidget::Open(void) {
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

// 0x5a7870: 0x2af is the lobby actors' "resume" message (JpLobbyPlayer.cpp
// clears its pause flag on it). CItemStorage's deleting destructor is its
// vtable slot 0 (0xe7688c is WSingleton<CItemStorage>::m_pInstance).
void CMyRoomWidget::CloseForm(void) {
	AfxGetTask()->GetActor("LobbyAvatar") << MsgObject(0, 0x2af, 0, 0, 0, 0, 0);
	AfxGetTask()->GetActor("LobbyCaddie") << MsgObject(0, 0x2af, 0, 0, 0, 0, 0);
	if (CItemStorage::IsInstantiated()) {
		delete CItemStorage::Instance();
	}
}

// 0x5a7b00 (0x5a7d10: the deleting destructor; 0x5a73c0: its FrCmdTarget
// thunk). The vptrs, CloseForm 0x5a7870 (called directly), then
// ~CTaskWidget 0x64f710.
CMyRoomWidget::~CMyRoomWidget(void) {
	CloseForm();
}

// 0x5a7f60: the KR CRealMyRoomTask constructor creates CItemStorage (only
// with local content 0x71); the JP widget always does, here (its
// constructor 0x7fd2d0, size 0x180, registers the singleton).
void CMyRoomWidget::Load(void) {
	m_layoutFile.assign(s_myRoomWidgetInfo.m_layout, 0, std::string::npos);
	new CItemStorage;
	CTaskWidget::Load();
}

// wtemplate.h:409 (the same local copy as RealMyRoomTask.cpp's; the JP
// find_if instantiation is 0x587c70): finds a list element by its serial.
struct _FindByGuid {
	unsigned long m_guid;

	_FindByGuid(unsigned long guid) : m_guid(guid) {}
	template <class T>
	bool operator()(const T& item) const { return m_guid == item.m_serial; }
};

// The JP inlines this at every site: the current task (0xe73df0
// GetCurrentTask 0x9b79c0, NULL without a task manager) gets the message
// through its vtable +0x30 (SendMsgToMainActor, MsgObject by value).
static void MyRoomWidget_Post(int msg, intptr_t param1, intptr_t param2, intptr_t param3) {
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, msg, param1, param2, param3, 0, 0));
}

// Message 0x23 with the JP game text of a Korean text key (Jp_LookupText
// 0xb4eb80).
static void MyRoomWidget_PostText(const char* key) {
	MyRoomWidget_Post(0x23, (intptr_t)Jp_LookupText(key), 0, 0);
}

// 0x5a8160: the pouch result holder, a function-local static. lib: 0xcf9da0
// (its atexit destructor)
sJpPouchResult* JpPouchResult_Instance(void) {
	static sJpPouchResult s_pouchResult;
	return &s_pouchResult;
}

// 0xaa8eb0
void sJpPouchResult::SetItem(unsigned long itemTypeId) {
	m_itemTypeId = itemTypeId;
}

// 0xaa9860
void sJpPouchResult::Clear(void) {
	m_prizes.clear();
	m_itemTypeId = 0;
}

// 0xaaa1f0
unsigned int sJpPouchResult::AddPrize(const sJpPouchPrize& prize) {
	m_prizes.push_back(prize);
	return (unsigned int)m_prizes.size();
}

// The JP tiki report lines are a std::vector of 0x83-byte records at
// CSharedDoc +0x1043c (BuildTikiReportList 0x644610 walks +0x1043c..+0x10440
// in 0x83 steps; 0x5a8210 / 0x5a82e0 are its clear / push_back); the port
// keeps the KR m_tikiReportList of 0x7f-byte sCaddieReportData that the KR
// BuildTikiReportList reads. The JP record is the KR one with a dword
// inserted at +0x5d: 0x644610 reads +0x59 and +0x5d as two dwords (two
// rival fields), the emblem name at +0x61, the room type at +0x6d, the
// flags at +0x71 and the winning team at +0x72 (KR +0x5d, +0x69, +0x6d,
// +0x6e). The port keeps +0x59 as the KR m_guildUID and drops the JP dword
// +0x5d (which of the two is the KR's emblem flag is not proven).
static void MyRoomWidget_AddTikiReport(const unsigned char* record) {
	sCaddieReportData report;
	memcpy(&report, record, 0x5d);
	memcpy((unsigned char*)&report + 0x5d, record + 0x61, sizeof(report) - 0x5d);
	WSingleton<CSharedDoc>::Instance()->m_tikiReportList.push_back(report);
}

// A little-endian dword of a byte-packed JP record.
static unsigned long MyRoomWidget_RecordDword(const unsigned char* record, int offset) {
	return (unsigned long)record[offset] | ((unsigned long)record[offset + 1] << 8) | ((unsigned long)record[offset + 2] << 16) | ((unsigned long)record[offset + 3] << 24);
}

// The JP room player record (0x15c bytes, the static 0xe76ab0; its
// constructor 0x500360) as the KR sSlotInfo (0x152) the port's
// m_roomPlayerList holds. From the two constructors (KR globalgamedefine.h
// sSlotInfo): the JP fields sit one byte lower from the KR m_connection
// 0x2f (JP 0x2e) through the emblem name (KR 0x5d, JP 0x5c, 0xc bytes),
// then a JP dword at +0x68 (no KR field), then three bytes higher from the
// KR m_ruleUID 0x69 (JP +0x6c) through m_valueCD 0xcd (JP +0xd0), the 0x80
// block 0xd1 (JP +0xd4) and the banish flags 0x151 (JP +0x154); the JP
// adds a float +0x155 (0.0), a byte +0x159 and a word +0x15a. The byte the
// JP drops is the guild name's: the JP-era server's PlayerRoomInfo
// (externs/SuperSS-Dev, TYPE/pangya_game_st.h) has char nickname[22] at
// +0x4 and char guild_name[20] at +0x1a (KR 0x15), the position at +0x2e,
// and names the dword +0x68 guild_mark_index (after the 12-byte mark image
// +0x5c); no JP client reader of either string was checked. The nickname
// is taken whole, the guild name's last byte is 0.
static void MyRoomWidget_SlotFromJp(const unsigned char* record, sSlotInfo* slot) {
	unsigned char* out = (unsigned char*)slot;
	memcpy(out, record, 0x2e);
	out[0x2e] = 0;
	memcpy(out + 0x2f, record + 0x2e, 0x69 - 0x2f);
	memcpy(out + 0x69, record + 0x6c, 0x152 - 0x69);
}

// The JP character record (0x201 bytes, the static 0xe768a8) as the KR
// sCharacterInfo: the fields JpCharacter_EncodeRecord (JpMasteryGlue.cpp)
// places, read back (type id +0x0, id +0x4, colours +0x8 / +0x9, part type
// ids +0xc, part ids +0x6c, the 0xd8-byte block +0xcc, aux parts +0x1a4).
// The KR stat levels (unknown6A) and unknown6F have no proven JP place: 0.
static void MyRoomWidget_CharacterFromJp(const unsigned char* record, sCharacterInfo* character) {
	*character = sCharacterInfo();
	character->typeId = MyRoomWidget_RecordDword(record, 0x0);
	character->id = MyRoomWidget_RecordDword(record, 0x4);
	character->hairColor = record[0x8];
	character->shirtColor = record[0x9];
	for (int i = 0; i < 24; i++) {
		character->partTypeIds[i] = MyRoomWidget_RecordDword(record, 0xc + i * 4);
		character->partIds[i] = MyRoomWidget_RecordDword(record, 0x6c + i * 4);
	}
	memcpy(&character->unknown84, record + 0xcc, 0xd8);
	for (int i = 0; i < 5; i++) {
		character->auxParts.tids[i] = MyRoomWidget_RecordDword(record, 0x1a4 + i * 4);
	}
}

// 0x5a8360: the KR CRealMyRoomTask::OnPacket (RealMyRoomTask.cpp) on the
// widget. Every message goes to the task's main actor (KR "RealMyRoom"),
// the texts through Jp_LookupText, the notify forms are owned by the widget
// (its FrCmdTarget, +8). JP packet ids against the KR: 0x69 = 0x67,
// 0xa5 = 0xa3, 0xcd = 0xcb, 0xe2 = 0xe0 (+2); 0x12c = 0x124 (+8); 0x16c..0x177
// = 0x171..0x17c, 0x17e / 0x17f = 0x183 / 0x184, 0x18f / 0x190 = 0x194 /
// 0x195 (-5). Messages: KR 0xd2..0xde
// + 0x25 (0xf7, 0xfb..0x101, 0x103, 0x106), 0x4e -> 0x5b, the storage family KR
// 0x234..0x23f + 0x7b (0x2af..0x2ba), 0x228 -> 0x2a6, 0x245 -> 0x2c1.
// IsLocalContent (0xa7a8d0) ids: KR 0x56 -> 0x57, 0x71 -> 0x70, 0x76 -> 0x75.
// JP CSharedDoc offsets used, mapped to the KR members:
//   +0x91e   m_myPang                +0x3870  m_myBonusCash (friend)
//   +0x3704  m_characterMap          +0x3740  m_myItemMap
//   +0x110c8 m_flag50F4 (friend)     +0x110c9 m_storageLock
//   +0x3734  m_myPartsList           +0x375c  m_itemInfoList
//   +0x10777 m_value4843             +0x1043c m_tikiReportList (0x83-byte records)
// CItemStorage (0xe7688c): UnLock 0x7fc480, ClearStoredItemInfo 0x7fd5a0,
// AddStoredItemInfo 0x7fe210 (0xb0-byte records), +0x58 stored pang, +0x60
// total page, +0x64 current page.
// Returns 1 when handled (0x175 and 0x177 only break), 0 for other ids.
// The Korean text keys are read with node scripts/disasm.ts --jp --data
// 0x<va> --as str.
int CMyRoomWidget::OnPacket(WReceivedPacket& packet) {
	packet.Reset();
	g_iRecvPacketType = packet.Decode2();
	int handled = 1;

	switch (g_iRecvPacketType) {
	case 0x69: {
		// Exchange result (KR 0x67): u8 result, u8 count, count x 0x26-byte
		// sBuyItemResult, i64 pang, i64 bonus cash.
		MyRoomWidget_Post(2, 0, 0, 0);
		unsigned char result = packet.Decode1();
		if (result == 5) {
			MyRoomWidget_PostText("\xB9\xB0\xB0\xC7\xC0\xBB \xC8\xAF\xBA\xD2\xB9\xDE\xC1\xF6 \xB8\xF8\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9");
		} else {
			MyRoomWidget_PostText("\xC8\xAF\xBA\xD2\xC0\xCC \xBF\xCF\xB7\xE1\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9");
		}

		int count = packet.Decode1();
		for (int i = 0; i < count; i++) {
			sBuyItemResult item;
			packet.DecodeBuffer(&item, 0x26);

			int msg;
			switch (item.typeId >> 26) {
			case 1: msg = 0xFB; break;
			case 2: msg = 0xFC; break;
			case 7: msg = 0xFD; break;
			case 6: msg = 0xFE; break;
			case 4: msg = 0xFF; break;
			case 5: msg = 0x100; break;
			case 14: msg = 0x101; break;
			default: continue;
			}
			MyRoomWidget_Post(msg, item.guid, item.typeId, 0);
		}

		WSingleton<CSharedDoc>::Instance()->m_myPang = packet.Decode8();   // JP +0x91e
		WSingleton<CSharedDoc>::Instance()->m_myBonusCash = packet.Decode8();  // JP +0x3870
		WSingleton<CSharedDoc>::Instance()->RefreshAllCookie();            // JP 0x6332b0
		JpLobbyDesktop_RefreshOnShow();
		break;
	}

	case 0xA5: {
		// Stat upgrade (1) / downgrade (2) (KR 0xa3): u8 result; for 1 / 2:
		// u8 type (bit 0: a club set), u8 stat, u32 id, i64 pang.
		int result = packet.Decode1();
		if (result == 1 || result == 2) {
			unsigned char type = packet.Decode1();
			unsigned char stat = packet.Decode1();
			unsigned long id = packet.Decode4();

			CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
			if (type & 1) {
				// JP +0x3740 (map find 0x4c6740), a short per stat at +0x1c of the node.
				std::map<unsigned int, sItemInfo>::iterator it = pDoc->m_myItemMap.find(id);
				if (it != pDoc->m_myItemMap.end()) {
					if (result == 1) {
						it->second.m_clubStatLevel[stat]++;
					} else {
						it->second.m_clubStatLevel[stat]--;
					}
				}
			} else {
				// JP +0x3704 (map find 0x507c00), a byte per stat at +0x1d8 of the node.
				std::map<unsigned int, sCharacterInfo>::iterator it = pDoc->m_characterMap.find(id);
				if (it != pDoc->m_characterMap.end()) {
					if (result == 1) {
						it->second.unknown6A.level[stat]++;
					} else {
						it->second.unknown6A.level[stat]--;
					}
				}
			}

			__int64 pang = packet.Decode8();
			WSingleton<CSharedDoc>::Instance()->m_myPang -= pang;
			JpLobbyDesktop_RefreshOnShow();
		}
		MyRoomWidget_Post(0xF7, result, 0, 0);
		break;
	}

	case 0xCD: {
		// KR 0xcb: u8 result; 0: u32 first, u32 second, u16 third.
		MyRoomWidget_Post(2, 0, 0, 0);
		unsigned char result = packet.Decode1();
		switch (result) {
		case 0: {
			unsigned long first = packet.Decode4();
			unsigned long second = packet.Decode4();
			unsigned short third = packet.Decode2();
			MyRoomWidget_Post(0x103, second, first, third);
			break;
		}
		case 1:
		case 2:
		case 3:
		case 4:
			MyRoomWidget_PostText("\xC1\xB6\xC7\xD5\xBF\xA1 \xBD\xC7\xC6\xD0\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.");
			break;
		case 5:
			MyRoomWidget_PostText("\xC3\xE0\xC7\xCF\xC7\xD5\xB4\xCF\xB4\xD9. \xC0\xCC\xBA\xA5\xC6\xAE\xBF\xA1 \xB4\xE7\xC3\xB7\xB5\xC7\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9. \xC0\xDA\xBC\xBC\xC7\xD1 \xBB\xE7\xC7\xD7\xC0\xBA \xC8\xA8\xC6\xE4\xC0\xCC\xC1\xF6\xB8\xA6 \xC2\xFC\xC1\xB6\xC7\xCF\xBC\xBC\xBF\xE4.");
			break;
		case 6:
			MyRoomWidget_PostText("\xC0\xCC\xBA\xA5\xC6\xAE\xBF\xA1 \xB4\xE7\xC3\xB7\xC0\xCC \xBE\xC8\xB5\xC7\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.");
			break;
		}
		break;
	}

	case 0xE2: {
		// Mascot message change (KR 0xe0): u8 result, u32 mascot id; 4: string
		// message, i64 pang.
		MyRoomWidget_Post(2, 0, 0, 0);
		unsigned char result = packet.Decode1();
		unsigned long mascotId = packet.Decode4();
		switch (result) {
		case 1:
		case 2:
			MyRoomWidget_PostText("\xB8\xDE\xBC\xBC\xC1\xF6 \xBA\xAF\xB0\xE6\xBF\xA1 \xBD\xC7\xC6\xD0\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.");
			break;
		case 3:
			MyRoomWidget_PostText("\xB1\xDD\xBE\xD7\xC0\xCC \xBA\xCE\xC1\xB7\xC7\xD5\xB4\xCF\xB4\xD9");
			break;
		case 4: {
			MyRoomWidget_PostText("\xB8\xDE\xBC\xBC\xC1\xF6\xB0\xA1 \xBA\xAF\xB0\xE6 \xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.");

			std::string message;
			packet.DecodeStr(message);
			WSingleton<CSharedDoc>::Instance()->m_myPang = packet.Decode8();

			sMascotInfo* pMascot = WSingleton<CSharedDoc>::Instance()->GetMyMascotInfo(mascotId);  // JP 0x63dc00
			if (pMascot) {
				memset(pMascot->message, 0, sizeof(pMascot->message));
				strncpy(pMascot->message, message.c_str(), 0x1D);
			}
			MyRoomWidget_Post(0x5B, pMascot->id, (intptr_t)pMascot, 0);
			JpLobbyDesktop_RefreshOnShow();
			break;
		}
		}
		break;
	}

	case 0x11A:
		// Tiki report (KR 0x116): i32 count (0 / -2 / -1: errors), 0x10-byte
		// SYSTEMTIME, count x 0x83-byte report records.
		if (IsLocalContent((localContentType_t)0x14)) {
			MyRoomWidget_Post(2, 0, 0, 0);
			WSingleton<CSharedDoc>::Instance()->m_tikiReportList.clear();  // JP 0x5a8210
			int count = packet.Decode4();
			SYSTEMTIME reportTime;
			packet.DecodeBuffer(&reportTime, sizeof(reportTime));
			if (count == 0) {
				MyRoomWidget_PostText("\xC6\xBC\xC5\xB0 \xB8\xAE\xC6\xF7\xC6\xAE \xBF\xAD\xB6\xF7\xBF\xA1 \xBD\xC7\xC6\xD0 \xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9. ");
				break;
			} else if (count == -2) {
				MyRoomWidget_PostText("\xB5\xA5\xC0\xCC\xC5\xB8\xB0\xA1 \xC0\xAF\xC8\xBF\xC7\xCF\xC1\xF6 \xBE\xCA\xC0\xBA \xC6\xBC\xC5\xB0 \xB8\xAE\xC6\xF7\xC6\xAE \xC0\xD4\xB4\xCF\xB4\xD9.");
				break;
			} else if (count == -1) {
				MyRoomWidget_PostText("\xB3\xAF\xC2\xA5\xB0\xA1 \xC0\xAF\xC8\xBF\xC7\xCF\xC1\xF6 \xBE\xCA\xC0\xBA \xC6\xBC\xC5\xB0 \xB8\xAE\xC6\xF7\xC6\xAE \xC0\xD4\xB4\xCF\xB4\xD9.");
				break;
			}
			if (IsLocalContent((localContentType_t)0x59)) {  // JP id (KR 0x58)
				WSingleton<CSharedDoc>::Instance()->m_value4843 = 1;
			}
			unsigned long serial = 0;
			MyRoomWidget_Post(0x281, (intptr_t)&serial, 0, 0);
			std::list<sItemInfo>::iterator it;
			for (it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {
				if (it->m_serial == serial)
					break;
			}
			if (it == WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end()) {
				// The KR returns 0; the JP jumps past its result load and
				// returns the list's end node (non-zero): handled.
				break;
			}
			for (int i = 0; i < count; ++i) {
				--it->m_count;
				if (it->m_count <= 0) {
					WSingleton<CSharedDoc>::Instance()->m_itemInfoList.erase(it);
					MyRoomWidget_Post(0x102, 0, 0, 0);
					break;
				}
			}
			for (int j = 0; j < count; ++j) {
				unsigned char record[0x83];
				packet.DecodeBuffer(record, sizeof(record));
				MyRoomWidget_AddTikiReport(record);  // JP push_back 0x5a82e0
			}
			WSingleton<CSharedDoc>::Instance()->BuildTikiReportList();  // JP 0x644610
			MyRoomWidget_Post(0x282, (intptr_t)&reportTime, 0, 0);
		}
		break;

	case 0x129: {
		// Pouch open (KR 0x121, reworked): u8 result; 0: u32 item type id,
		// u32 its new count, u32 prize count, count x 0x14-byte prizes; 1: u8
		// error. The JP keeps the prizes in a holder (message 0x279) instead
		// of the KR's text and message 0x26.
		unsigned char result = packet.Decode1();
		switch (result) {
		case 0: {
			MyRoomWidget_Post(2, 0, 0, 0);
			unsigned long itemTypeId = packet.Decode4();
			int itemCount = packet.Decode4();
			std::list<sItemInfo>::iterator it;
			for (it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {
				if (it->m_typeId == itemTypeId)
					break;
			}
			if (it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end()) {
				it->m_count = (short)itemCount;
				if (it->m_count < 1) {
					WSingleton<CSharedDoc>::Instance()->m_itemInfoList.erase(it);
				}
			}
			int count = packet.Decode4();
			JpPouchResult_Instance()->Clear();
			JpPouchResult_Instance()->SetItem(itemTypeId);
			for (int i = 0; i < count; i++) {
				sJpPouchPrize prize;
				memset(&prize, 0, sizeof(prize));
				packet.DecodeBuffer(&prize, sizeof(prize));
				JpPouchResult_Instance()->AddPrize(prize);
			}
			MyRoomWidget_Post(0x279, 0, 0, 0);
			break;
		}
		case 1: {
			MyRoomWidget_Post(2, 0, 0, 0);
			switch (packet.Decode1()) {
			case 0: {
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				pForm->SetMessage(Jp_LookupText("\xc0\xdf\xb8\xf8\xb5\xc8 \xba\xb9\xc1\xd6\xb8\xd3\xb4\xcf \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9. \xb0\xfc\xb8\xae\xc0\xda\xbf\xa1\xb0\xd4 \xb9\xae\xc0\xc7 \xc7\xcf\xbc\xbc\xbf\xe4."), false);
				pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
				break;
			}
			case 1: {
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				pForm->SetMessage(Jp_LookupText("\xbc\xad\xb9\xf6\xc0\xc7 \xbf\xe4\xc3\xbb\xc0\xcc \xb8\xb9\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xe1\xbd\xc3 \xc8\xc4 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5 \xc7\xd8 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0."), false);
				pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
				break;
			}
			case 3: {
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				pForm->SetMessage(Jp_GetTableText(0x30F34), false);
				pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
				break;
			}
			case 5: {
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				pForm->SetMessage(Jp_LookupText("\xbd\xc3\xbd\xba\xc5\xdb \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9."), false);
				pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
				break;
			}
			}
			break;
		}
		}
		break;
	}

	case 0x12C:
		// KR 0x124: u32 result.
		if (IsLocalContent((localContentType_t)0x57)) {  // JP id
			unsigned long result = packet.Decode4();
			MyRoomWidget_Post(0x2A6, 0x12C, result, 0);
		}
		break;

	case 0x168: {
		// An avatar of the room (KR 0x16e): a 0x15c-byte player record and a
		// 0x201-byte character record into function-local statics (0xe76ab0 /
		// 0xe768a8). The record joins the room player list (JP +0x109fc,
		// push_back 0x50a5e0) when its uid is new (the JP has no avatar chat
		// test), then main actor message 0x10c (KR RealMyRoom 0xe4) with the
		// two records; the port keeps the KR records (the JP reader of 0x10c
		// is not identified).
		static sSlotInfo slot;
		static sCharacterInfo charInfo;
		unsigned char slotRecord[0x15c];
		unsigned char charRecord[0x201];
		packet.DecodeBuffer(slotRecord, sizeof(slotRecord));
		packet.DecodeBuffer(charRecord, sizeof(charRecord));
		MyRoomWidget_SlotFromJp(slotRecord, &slot);
		MyRoomWidget_CharacterFromJp(charRecord, &charInfo);

		std::list<sSlotInfo>& players = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList;
		std::list<sSlotInfo>::iterator it;
		for (it = players.begin(); it != players.end(); ++it) {
			if (it->m_uid == slot.m_uid)
				break;
		}
		if (it == players.end()) {
			players.push_back(slot);
		}
		MyRoomWidget_Post(0x10C, (intptr_t)&slot, (intptr_t)&charInfo, 0);
		break;
	}

	case 0x16C:
		// Storage open (KR 0x171): u32 result.
		if (IsLocalContent((localContentType_t)0x70) == true) {  // JP id
			unsigned long result = packet.Decode4();
			if (result != 0) {
				if (result == 0x66) {
					MyRoomWidget_Post(0x2AF, 0, 0, 0);
				} else {
					MyRoomWidget_Post(0x2BA, 0x16C, result, 0);
				}
			} else {
				WSendPacket request((enumClientPacket)0xD5);
				request.Send(SEND_TO_DEFAULT);
				WSendPacket pageRequest((enumClientPacket)0xCD);
				pageRequest.Encode4(0x63);
				pageRequest.Encode2(1);
				pageRequest.Send(SEND_TO_DEFAULT);
			}
			CItemStorage* pStorage = WSingleton<CItemStorage>::Instance();
			if (pStorage) {
				pStorage->UnLock();
			}
		}
		break;

	case 0x16D:
		// Storage page (KR 0x172): u16 total page, u16 page, u8 count, count x
		// 0xb0-byte sStoredItemInfo.
		if (IsLocalContent((localContentType_t)0x70) == true) {
			CItemStorage* pStorage = WSingleton<CItemStorage>::Instance();
			if (pStorage) {
				pStorage->ClearStoredItemInfo();
				unsigned long totalPage = packet.Decode2();
				unsigned long curPage = packet.Decode2();
				unsigned int count = packet.Decode1();
				for (unsigned int i = 0; i < count; i++) {
					sStoredItemInfo info;
					memset(&info, 0, sizeof(info));
					packet.DecodeBuffer(&info, sizeof(info));
					pStorage->AddStoredItemInfo(info);
				}
				pStorage->SetTotalPage(totalPage);
				pStorage->SetCurPage(curPage);
				MyRoomWidget_Post(0x2B5, 0, 0, 0);
				pStorage->UnLock();
			}
		}
		break;

	case 0x16F:
		// KR 0x174: u32 result.
		if (IsLocalContent((localContentType_t)0x70) == true) {
			unsigned long result = packet.Decode4();
			if (result == 0) {
				MyRoomWidget_Post(0x2B8, result, result, 0);
			} else {
				MyRoomWidget_Post(0x2BA, 0x16F, result, 0);
			}
			CItemStorage* pStorage = WSingleton<CItemStorage>::Instance();
			if (pStorage) {
				pStorage->UnLock();
			}
		}
		break;

	case 0x16E:
		// KR 0x173: u32 result.
		if (IsLocalContent((localContentType_t)0x70) == true) {
			unsigned long result = packet.Decode4();
			if (result == 0) {
				MyRoomWidget_Post(0x2B9, result, result, 0);
			} else {
				MyRoomWidget_Post(0x2BA, 0x16E, result, 0);
			}
			CItemStorage* pStorage = WSingleton<CItemStorage>::Instance();
			if (pStorage) {
				pStorage->UnLock();
			}
		}
		break;

	case 0x170:
		// Storage state (KR 0x175): u32 result, u32 flags. The JP merges the
		// KR 4|8 and 4|0x10 branches into one (flags & 0x18), which locks.
		if (IsLocalContent((localContentType_t)0x70) == true) {
			unsigned long result = packet.Decode4();
			unsigned long flags = packet.Decode4();
			if (result == 0) {
				if (flags & 2) {
					MyRoomWidget_Post(0x2B1, 0, 0, 0);
					WSingleton<CSharedDoc>::Instance()->m_flag50F4 = true;            // JP +0x110c8
				} else if ((flags & 4) && (flags & 0x18)) {
					if (!WSingleton<CSharedDoc>::Instance()->m_flag50F4) {
						WSingleton<CSharedDoc>::Instance()->m_flag50F4 = true;
					} else {
						MyRoomWidget_Post(0x2B0, 0, 0, 0);
					}
					WSingleton<CSharedDoc>::Instance()->m_storageLock = true;       // JP +0x110c9
				}
			} else {
				MyRoomWidget_Post(0x2BA, 0x170, result, 0);
			}
			CItemStorage* pStorage = WSingleton<CItemStorage>::Instance();
			if (pStorage) {
				pStorage->UnLock();
			}
		}
		break;

	case 0x176:
		// KR 0x17b: u32 result.
		if (IsLocalContent((localContentType_t)0x70) == true) {
			unsigned long result = packet.Decode4();
			MyRoomWidget_Post(0x2BA, 0x176, result, 0);
			CItemStorage* pStorage = WSingleton<CItemStorage>::Instance();
			if (pStorage) {
				pStorage->UnLock();
			}
		}
		break;

	case 0x171: {
		// KR 0x176 (no local-content test): u32 result.
		unsigned long result = packet.Decode4();
		if (result != 0) {
			MyRoomWidget_Post(0x2BA, 0x171, result, 0);
		}
		CItemStorage* pStorage = WSingleton<CItemStorage>::Instance();
		if (pStorage) {
			pStorage->UnLock();
		}
		break;
	}

	case 0x172: {
		// Stored pang (KR 0x177): i64.
		CItemStorage* pStorage = WSingleton<CItemStorage>::Instance();
		if (pStorage) {
			pStorage->SetStoredPang(packet.Decode8());
			pStorage->UnLock();
		}
		break;
	}

	case 0x173: {
		// Storage lock setting (KR 0x178): u32 result; 0: u8 lock.
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		std::string message;
		unsigned long result = packet.Decode4();
		switch (result) {
		case 0: {
			message = "\xb5\xb9\xc7\xc7\xb4\xcf\xc0\xc7 \xba\xf1\xb9\xd0\xb1\xdd\xb0\xed \xbc\xb3\xc1\xa4\xc0\xcc \xba\xaf\xb0\xe6 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9";
			unsigned char lock = packet.Decode1();
			WSingleton<CSharedDoc>::Instance()->m_storageLock = (lock == 1);
			break;
		}
		case 0x65:
			message = "\xbc\xb3\xc1\xa4\xc0\xcc \xba\xaf\xb0\xe6\xb5\xc7\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9";
			break;
		case 0x64:
			message = "DataBase\xc1\xf6\xbf\xac \xc0\xe1\xbd\xc3 \xc8\xc4 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5 \xc7\xd8 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0";
			break;
		case 0x75:
			message = "\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xc0\xdf\xb8\xf8\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5 \xc7\xd8 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0";
			break;
		}
		pForm->SetMessage(Jp_LookupText(message.c_str()), false);
		pForm->Open((FRESH_RESULT_CALLBACK)0, 0x803);
		break;
	}

	case 0x174: {
		// Storage password change (KR 0x179): u32 result.
		unsigned long result = packet.Decode4();
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		switch (result) {
		case 0:
			pForm->SetMessage(Jp_LookupText("\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xba\xaf\xb0\xe6 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9."), false);
			break;
		case 0x65:
			pForm->SetMessage(Jp_LookupText("\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xba\xaf\xb0\xe6\xb5\xc7\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9."), false);
			break;
		case 0x64:
			pForm->SetMessage(Jp_LookupText("\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xba\xaf\xb0\xe6\xb5\xc7\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9."), false);
			break;
		case 0x68:
			pForm->SetMessage(Jp_LookupText("\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xc0\xc7 \xb1\xe6\xc0\xcc\xb0\xa1 \xc0\xdf\xb8\xf8 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.."), false);
			break;
		case 0x72:
			pForm->SetMessage(Jp_LookupText("'\xb5\xb9\xc7\xc7\xb4\xcf\xc0\xc7 \xba\xf1\xb9\xd0\xb1\xdd\xb0\xed'\xc0\xc7 \xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xbc\xb3\xc1\xa4 \xb5\xc7\xbe\xee \xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9."), false);
			break;
		case 0x75:
			pForm->SetMessage(Jp_LookupText("\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xc0\xdf\xb8\xf8\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9."), false);
			break;
		}
		pForm->Open((FRESH_RESULT_CALLBACK)0, 0x803);
		break;
	}

	case 0x175:
	case 0x177:
		break;

	case 0x17F:
		// Furniture use result (KR 0x184): u8 result.
		if (IsLocalContent((localContentType_t)0x75)) {  // JP id
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			switch (packet.Decode1()) {
			case 1:
				pForm->SetMessage(Jp_LookupText("\xbc\xd2\xc0\xaf\xc7\xcf\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xc0\xba \xb0\xa1\xb1\xb8\xc0\xd4\xb4\xcf\xb4\xd9."), false);
				break;
			case 2:
				pForm->SetMessage(Jp_LookupText("\xbf\xc0\xb4\xc3\xc0\xba \xc0\xcc \xb0\xa1\xb1\xb8\xb8\xa6 \xb4\xf5 \xc0\xcc\xbb\xf3 \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."), false);
				break;
			case 5:
				pForm->SetMessage(Jp_LookupText("\xbf\xc0\xb4\xc3\xc0\xba \xc0\xcc \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb4\xf5 \xc0\xcc\xbb\xf3 \xbe\xf2\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."), false);
				break;
			case 0:
				pForm->SetMessage(Jp_LookupText("\xc0\xcc \xb0\xa1\xb1\xb8\xc0\xc7 \xb1\xe2\xb4\xc9\xc0\xbb \xbb\xe7\xbf\xeb\xc7\xcf\xb4\xc2\xb5\xa5 \xbd\xc3\xbd\xba\xc5\xdb \xbf\xc0\xb7\xf9\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9. GM\xbf\xa1\xb0\xd4 \xb9\xae\xc0\xc7 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9."), false);
				break;
			case 3:
				pForm->SetMessage(Jp_LookupText("\xb9\xe8\xc4\xa1\xb5\xc7\xc1\xf6 \xbe\xca\xc0\xba \xb0\xa1\xb1\xb8\xc0\xd4\xb4\xcf\xb4\xd9. \xb9\xe8\xc4\xa1\xc7\xd1 \xb4\xd9\xc0\xbd \xc0\xfa\xc0\xe5\xb9\xf6\xc6\xb0\xc0\xbb \xb4\xad\xb7\xaf \xc0\xfa\xc0\xe5\xc7\xcf\xbd\xc5 \xc8\xc4\xbf\xa1 \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9."), false);
				break;
			case 4:
				pForm->SetMessage(Jp_LookupText("\xb0\xa1\xb1\xb8\xc0\xc7 \xb1\xe2\xb4\xc9\xc0\xbb \xbb\xe7\xbf\xeb\xc7\xcf\xbf\xa9 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbe\xf2\xb4\xc2 \xb5\xa5\xbf\xa1 \xb9\xae\xc1\xa6\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xcf\xbf\xa9 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbe\xf2\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9. GM\xbf\xa1\xb0\xd4 \xb9\xae\xc0\xc7 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9."), false);
				break;
			case 6:
				pForm->SetMessage(Jp_LookupText("\xc7\xf6\xc0\xe7 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb0\xa1\xb1\xb8\xc0\xd4\xb4\xcf\xb4\xd9."), false);
				break;
			}
			pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
		} else {
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			pForm->SetMessage(Jp_LookupText("\xb3\xad\xb7\xce \xb1\xe2\xb4\xc9\xc0\xba \xc7\xf6\xc0\xe7 \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. "), false);
			pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
		}
		break;

	case 0x17E:
		// Furniture effect (KR 0x183): u32 first, u32 second.
		if (IsLocalContent((localContentType_t)0x75)) {
			unsigned long first = packet.Decode4();
			unsigned long second = packet.Decode4();
			MyRoomWidget_Post(0x2C1, first, second, 0);
		} else {
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			pForm->SetMessage(Jp_LookupText("\xb3\xad\xb7\xce \xb1\xe2\xb4\xc9\xc0\xba \xc7\xf6\xc0\xe7 \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. "), false);
			pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
		}
		break;

	case 0x18F: {
		// Rental extension (KR 0x194): u8 result; 0: u32 type id, u32 serial.
		AfxGetTask()->GetMainActor() << MsgObject(0, 2, 0, 0, 0, 0, 0);
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		int result = packet.Decode1();
		if (result == 0) {
			unsigned long typeId = packet.Decode4();
			unsigned long serial = packet.Decode4();
			std::list<sItemInfo>* pList = (typeId & 0xFC000000) == 0x08000000 ? &WSingleton<CSharedDoc>::Instance()->m_myPartsList : &WSingleton<CSharedDoc>::Instance()->m_itemInfoList;
			std::list<sItemInfo>::iterator it = std::find_if(pList->begin(), pList->end(), _FindByGuid(serial));
			if (it != pList->end()) {
				it->m_stateBits.expired = 0;   // JP +0x1f bit 1
				it->m_remainHours = 0xA8;      // JP +0x10
				pForm->SetMessage(Jp_LookupText("\xb4\xeb\xbf\xa9 \xb1\xe2\xb0\xa3\xc0\xcc \xbf\xac\xc0\xe5 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9."), false);
			} else {
				pForm->SetMessage(Jp_LookupText("\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xbf\xa1 \xb4\xeb\xc7\xd1 \xc1\xa4\xba\xb8\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."), false);
			}
		} else {
			switch (result) {
			case 1:
				pForm->SetMessage(Jp_LookupText("\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xb4\xeb\xbf\xa9 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9."), false);
				break;
			case 2:
				pForm->SetMessage(Jp_LookupText("\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xbf\xa1 \xb4\xeb\xc7\xd1 \xc1\xa4\xba\xb8\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."), false);
				break;
			case 3:
				pForm->SetMessage(Jp_LookupText("\xb1\xe2\xb0\xa3\xc0\xcc \xb8\xb8\xb7\xe1\xb5\xc8 \xc0\xc7\xbb\xf3\xb8\xb8 \xb1\xe2\xb0\xa3\xc0\xbb \xbf\xac\xc0\xe5 \xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9."), false);
				break;
			case 5:
				pForm->SetMessage(Jp_LookupText("\xb1\xe2\xb0\xa3 \xbf\xac\xc0\xe5 \xbf\xe4\xc3\xbb\xc0\xcc \xb0\xc5\xba\xce \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9."), false);
				break;
			default:
				pForm->SetMessage(MakeStr(Jp_LookupText("\xc3\xb3\xb8\xae\xbf\xa1 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.(%d)"), result), false);
				break;
			}
		}
		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
		break;
	}

	case 0x190: {
		// Rental deletion (KR 0x195): u8 result; 0: u32 type id, u32 serial.
		AfxGetTask()->GetMainActor() << MsgObject(0, 2, 0, 0, 0, 0, 0);
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		int result = packet.Decode1();
		if (result == 0) {
			unsigned long typeId = packet.Decode4();
			unsigned long serial = packet.Decode4();
			std::list<sItemInfo>* pList = (typeId & 0xFC000000) == 0x08000000 ? &WSingleton<CSharedDoc>::Instance()->m_myPartsList : &WSingleton<CSharedDoc>::Instance()->m_itemInfoList;
			std::list<sItemInfo>::iterator it = std::find_if(pList->begin(), pList->end(), _FindByGuid(serial));
			if (it != pList->end()) {
				pList->erase(it);
				pForm->SetMessage(Jp_LookupText("\xb4\xeb\xbf\xa9 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbb\xe8\xc1\xa6 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9."), false);
				AfxGetTask()->GetMainActor() << MsgObject(0, 0x106, 0, 0, 0, 0, 0);
			} else {
				pForm->SetMessage(Jp_LookupText("\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xbf\xa1 \xb4\xeb\xc7\xd1 \xc1\xa4\xba\xb8\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."), false);
			}
		} else {
			switch (result) {
			case 1:
				pForm->SetMessage(Jp_LookupText("\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xb4\xeb\xbf\xa9 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9."), false);
				break;
			case 2:
				pForm->SetMessage(Jp_LookupText("\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xbf\xa1 \xb4\xeb\xc7\xd1 \xc1\xa4\xba\xb8\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."), false);
				break;
			case 4:
				pForm->SetMessage(Jp_LookupText("\xc4\xab\xb5\xe5\xb0\xa1 \xc0\xd6\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xbb\xe8\xc1\xa6 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."), false);
				break;
			case 5:
				pForm->SetMessage(Jp_LookupText("\xbe\xc6\xc0\xcc\xc5\xdb \xbb\xe8\xc1\xa6 \xbf\xe4\xc3\xbb\xc0\xcc \xb0\xc5\xba\xce \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9."), false);
				break;
			default:
				pForm->SetMessage(MakeStr(Jp_LookupText("\xc3\xb3\xb8\xae\xbf\xa1 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.(%d)"), result), false);
				break;
			}
		}
		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
		break;
	}

	case 0x19D: {
		// Box open (KR 0x1a2, 0x5a95e6): u32 result, u32 item id, u32 type
		// id, u32 quantity. Every message goes to the main actor (KR 0xdd ->
		// 0x102). Result 0 tests the type id with CItemManager::FindCommonItem
		// (0xa17cf0 on CSharedDoc +0x20c); its texts switch on (type id >> 26)
		// - 2 (byte table 0x5aaddc, jump table 0x5aadcc): 2 and 5 the "%d"
		// text, 6 the pang (0x1a000010) and experience (0x1a00015d) pouches
		// apart, 0x10 the plain text, the others "%d" only for quantity > 1.
		// Then, unlike the KR, no mail / gift list request: with local content
		// 0x61 (KR 0x60) the mail list (JP +0x378c, clear 0x5a7b70) is
		// emptied, the gift alarm set (JP +0x10d80 = KR m_tutorialFinish:
		// JpLobbyGlue.cpp) and message 0xce(1) posted. 0x200b3b..0x200b3d go
		// to the "Gateway" actor (0xe0f63c) as 0x7531 (KR 0x276).
		MyRoomWidget_Post(2, 0, 0, 0);
		unsigned long result = packet.Decode4();
		unsigned long itemId = packet.Decode4();
		unsigned long typeId = packet.Decode4();
		unsigned long quantity = packet.Decode4();

		const char* failTitle = "\xBB\xF3\xC0\xDA \xBF\xAD\xB1\xE2\xB0\xA1 \xBD\xC7\xC6\xD0\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.";       // 0xd0d51c
		const char* receivedCount = "\xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB %d \xB0\xB3 \xB9\xDE\xC0\xB8\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.";  // 0xd0d4a8
		const char* received = "\xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xB9\xDE\xC0\xB8\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.";            // 0xd0d4c8

		char text[256];
		memset(text, 0, sizeof(text));

		switch (result) {
		case 0: {
			if (!WSingleton<CSharedDoc>::Instance()->m_itemManager.FindCommonItem(typeId)) {
				char wrongType[64];
				memset(wrongType, 0, sizeof(wrongType));
				strcat(wrongType, Jp_LookupText("\xC0\xDF\xB8\xF8\xB5\xC8 \xBE\xC6\xC0\xCC\xC5\xDB \xC5\xB8\xC0\xD4ID \xC0\xD4\xB4\xCF\xB4\xD9."));  // 0xd0d4e0
				MyRoomWidget_Post(0x23, (intptr_t)wrongType, 0, 0);
				MyRoomWidget_Post(0x102, itemId, 1, 0);
				break;
			}

			switch (typeId >> 26) {
			case 2:
			case 5:
				sprintf(text, Jp_LookupText(receivedCount), quantity);
				break;
			case 6:
				if (typeId == 0x1A000010) {
					sprintf(text, Jp_LookupText("\xC6\xCE \xC1\xD6\xB8\xD3\xB4\xCF(%d)\xB8\xA6 \xB9\xDE\xC0\xB8\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9."), quantity);  // 0xd0d464
				} else if (typeId == 0x1A00015D) {
					sprintf(text, Jp_LookupText("\xB0\xE6\xC7\xE8\xC4\xA1 \xC1\xD6\xB8\xD3\xB4\xCF(%d)\xB8\xA6 \xB9\xDE\xC0\xB8\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9."), quantity);  // 0xd0d484
				} else if (quantity > 1) {
					sprintf(text, Jp_LookupText(receivedCount), quantity);
				} else {
					strcpy(text, Jp_LookupText(received));  // the JP passes it to sprintf as the format
				}
				break;
			case 0x10:
				strcpy(text, Jp_LookupText(received));  // sprintf with the text as the format
				break;
			default:
				if (quantity > 1) {
					sprintf(text, Jp_LookupText(receivedCount), quantity);
				} else {
					strcpy(text, Jp_LookupText(received));
				}
				break;
			}

			MyRoomWidget_Post(0x26, (intptr_t)text, typeId, 0);
			MyRoomWidget_Post(0x102, itemId, 1, 0);
			if (IsLocalContent((localContentType_t)0x61)) {  // JP id
				WSingleton<CSharedDoc>::Instance()->m_mailList.clear();
			}
			MyRoomWidget_PostText("\xBB\xF3\xC7\xB0\xC0\xCC \xBF\xEC\xC6\xED\xC0\xB8\xB7\xCE \xC0\xFC\xB4\xDE\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9. \xBF\xEC\xC6\xED\xC7\xD4\xC0\xBB \xC8\xAE\xC0\xCE\xC7\xCF\xBC\xBC\xBF\xE4.");  // 0xd0d42c
			WSingleton<CSharedDoc>::Instance()->m_tutorialFinish = 1;  // JP +0x10d80
			MyRoomWidget_Post(0xCE, 1, 0, 0);
			break;
		}

		case 4:
			strcpy(text, Jp_LookupText(failTitle));
			strcat(text, Jp_LookupText("\x0A\xBC\xF6\xB7\xAE\xC0\xCC \xC3\xE6\xBA\xD0\xC7\xCF\xC1\xF6 \xBE\xCA\xBD\xC0\xB4\xCF\xB4\xD9."));  // 0xd0d500
			MyRoomWidget_Post(0x23, (intptr_t)text, 0, 0);
			break;

		case 8:
			strcpy(text, Jp_LookupText(failTitle));
			strcat(text, Jp_LookupText("\x0A\xBE\xC6\xC0\xCC\xC5\xDB \xC1\xBE\xB7\xF9 \xBF\xA1\xB7\xAF\xC0\xD4\xB4\xCF\xB4\xD9."));  // 0xd0d410
			MyRoomWidget_Post(0x23, (intptr_t)text, 0, 0);
			break;

		case 0x200B3B:
		case 0x200B3C:
		case 0x200B3D:
			AfxGetTask()->GetActor("Gateway") << MsgObject(0, 0x7531, 1, result, 0, 0, 0);
			break;

		default:
			strcpy(text, Jp_LookupText(failTitle));
			strcat(text, Jp_LookupText("\n"));  // 0xd66074
			strcat(text, MakeStr("Error : %d", result));  // 0xd0d404, MakeStr 0x98c3f0
			MyRoomWidget_Post(0x23, (intptr_t)text, 0, 0);
			break;
		}
		break;
	}

	case 0x1B0: {
		// JP only, the self-design shop ("[SelfDeSign_Shop]" 0xd0cf44): u8
		// result. The unknown-code text is not looked up.
		MyRoomWidget_Post(2, 0, 0, 0);
		int result = packet.Decode1();
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		switch (result) {
		case 0:
			pForm->SetMessage(Jp_LookupText("\xbd\xc5\xc3\xbb\xc0\xcc\xbf\xcf\xb7\xe1\xb5\xc8\xc0\xc7\xbb\xf3\xc0\xd4\xb4\xcf\xb4\xd9."), false);
			break;
		case 1:
			pForm->SetMessage(Jp_LookupText("\xbd\xc5\xc3\xbb\xc0\xcc\xbf\xcf\xb7\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9."), false);
			break;
		default: {
			char text[256] = "";
			sprintf(text, "[SelfDeSign_Shop] Unknown Message CODE:%d", result);
			pForm->SetMessage(text, false);
			break;
		}
		}
		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
		MyRoomWidget_Post(0x292, 0, 0, 0);
		break;
	}

	case 0x1B2: {
		// JP only: u32 count, shown in a notify form ("%d" in the text).
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		char text[128] = "";
		unsigned long count = packet.Decode4();
		sprintf(text, Jp_LookupText("\xc7\xc3\xb7\xb9\xc0\xcc\xc7\xd1 \xc8\xa6\xbc\xf6\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9 \xc7\xf6\xc0\xe7:%d\xc8\xa6"), count);
		if (pForm) {
			pForm->SetMessage(text, false);
			pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
		}
		MyRoomWidget_Post(2, 0, 0, 0);
		break;
	}

	default:
		handled = 0;
		break;
	}

	return handled;
}
