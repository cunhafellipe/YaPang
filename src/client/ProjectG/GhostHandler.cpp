// Original: d:\build\custom\temp\code\client\projectg\ghosthandler.cpp
#include <windows.h>
#include <atlbase.h>	// stand-in for the precompiled header: _AtlWinModule initializer
#include "ghosthandler.h"
#include "ghostdocument.h"
#include "ghostmanager.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "TaskManager.h"
#include "Packet.h"
#include "mousecursor.h"
#include "fresh.h"
#include "frform.h"
#include "Localize.h"
#include "ProjectG.h"
#include "GhostGameDlg.h"
#include "frdesktop.h"
#include "tinyxml.h"

#include <list>
#include <string>

// ghosthandler.cpp:14
const WRTTI CGhostRecorder::m_RTTI("CGhostRecorder", &IActor::m_RTTI);

IObject* CGhostRecorderMakeInstance(void) {
	return new CGhostRecorder;
}

struct __impCGhostRecorder {
	__impCGhostRecorder(void) {
		ObjectFactory().AddObjectFunctor(CGhostRecorderMakeInstance, "CGhostRecorder");
		ActorFactory().AddActorClass("CGhostRecorder");
	}
};
static __impCGhostRecorder s_registerCGhostRecorder;

// ghosthandler.cpp:18
CGhostRecorder::CGhostRecorder(void) : m_pDocument(0) {
}

// ghosthandler.cpp:29
bool CGhostRecorder::IsSaveAvailable(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_roomType != 4 && WSingleton<CSharedDoc>::Instance()->m_roomType != 0) {
		return false;
	}
	if (WSingleton<CSharedDoc>::Instance()->m_gameKind == 4) {
		return false;
	}
	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomMode == 14) {
		return false;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_holes == 18) {
		return true;
	}
	return false;
}

// ghosthandler.cpp:47
void CGhostRecorder::HandleMsg(const MsgObject &msg) {
	if (m_pDocument) {
		switch (msg.m_msg) {
		case 11:
			CGhostManager::Instance()->ReleaseDocument();
			break;
		default:
			m_pDocument->RecordHoleData(msg);
			break;
		}
	}
}

// ghosthandler.cpp:68
void CGhostRecorder::OnLoad(void) {
	CGhostManager::Instance()->ReleaseDocument();
	if (OnlinePlay() && IsSaveAvailable()) {
		m_pDocument = CGhostManager::Instance()->GetDocument();
		m_pDocument->RecordUserData();
		m_pDocument->RecordGameData();
	}
}

// ghosthandler.cpp:79
void CGhostRecorder::OnReload(void) {
}

// ghosthandler.cpp:87
void CGhostRecorder::OnInit(void) {
}

// ghosthandler.cpp:92
void CGhostRecorder::OnDestroy(void) {
}

// ghosthandler.cpp:104
CGhostLoader::CGhostLoader(void) {
}

// ghosthandler.cpp:109
bool CGhostLoader::LoadFromMemory(unsigned char *data, unsigned int size) {
	CGhostManager::Instance()->ReleaseDocument();
	CGhostDocument *document = CGhostManager::Instance()->GetDocument();
	return document->Import(data, size);
}

// ghosthandler.cpp:116
bool CGhostLoader::LoadFromFile(const std::string &fileName) {
	CGhostManager::Instance()->ReleaseDocument();
	CGhostDocument *document = CGhostManager::Instance()->GetDocument();
	return document->Import(fileName);
}

namespace {

// Message texts. The original reads each through a non-const pointer
// (`?X@?A0x...@@3PBDB`) initialized dynamically from a constant, one
// `mov [X], literal` initializer per line (sub_009a6a30..sub_009a6ae0;
// docs/tough_problems/010).
// ghosthandler.cpp:131
// "The ghost service is under maintenance. Sorry for the inconvenience."
const char *const NOTIFY_MSG_SERVICE_STOP_TEXT = "\xb0\xed\xbd\xba\xc6\xae \xbc\xad\xba\xf1\xbd\xba \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9. \xc0\xcc\xbf\xeb\xbf\xa1 \xba\xd2\xc6\xed\xc0\xbb \xb5\xe5\xb7\xc1\xbc\xad \xc1\xcb\xbc\xdb\xc7\xd5\xb4\xcf\xb4\xd9.";
const char *NOTIFY_MSG_SERVICE_STOP = NOTIFY_MSG_SERVICE_STOP_TEXT;
// ghosthandler.cpp:132
// "Loading failed."
const char *const RECORD_DLG_LOADING_FAIL_TEXT = "\xb7\xce\xb5\xf9\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.";
const char *RECORD_DLG_LOADING_FAIL = RECORD_DLG_LOADING_FAIL_TEXT;
// ghosthandler.cpp:133
// "...'s ghost is confused." (prefixed by a nick)
const char *const CANNOT_UNDERSTAND_MSG_TEXT = "\xb4\xd4\xc0\xc7 \xb0\xed\xbd\xba\xc6\xae\xb0\xa1 \xbe\xee\xb8\xae\xb5\xd5\xc0\xfd\xc7\xd8 \xc7\xd5\xb4\xcf\xb4\xd9.";
const char *CANNOT_UNDERSTAND_MSG = CANNOT_UNDERSTAND_MSG_TEXT;
// ghosthandler.cpp:134
// "No ghost is currently saved for the user."
const char *const NOTIFY_MSG_GHOST_NOSAVE_TEXT = "\xc7\xf6\xc0\xe7 \xc0\xaf\xc0\xfa\xbf\xa1\xb0\xd4 \xc0\xfa\xc0\xe5\xb5\xc8 \xb0\xed\xbd\xba\xc6\xae\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";
const char *NOTIFY_MSG_GHOST_NOSAVE = NOTIFY_MSG_GHOST_NOSAVE_TEXT;
// ghosthandler.cpp:135
// "The ghost data does not exist."
const char *const NOTIFY_MSG_GHOST_NODATA_TEXT = "\xb0\xed\xbd\xba\xc6\xae \xb5\xa5\xc0\xcc\xc5\xcd\xb0\xa1 \xc1\xb8\xc0\xe7 \xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.";
const char *NOTIFY_MSG_GHOST_NODATA = NOTIFY_MSG_GHOST_NODATA_TEXT;
// ghosthandler.cpp:136
// "The ghost data has been updated."
const char *const NOTIFY_MSG_GHOST_SAVED_TEXT = "\xb0\xed\xbd\xba\xc6\xae \xb5\xa5\xc0\xcc\xc5\xcd\xb0\xa1 \xb0\xbb\xbd\xc5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.";
const char *NOTIFY_MSG_GHOST_SAVED = NOTIFY_MSG_GHOST_SAVED_TEXT;
// ghosthandler.cpp:137
// "The system is under maintenance."
const char *const NOTIFY_MSG_UNKNOWN_ERROR_TEXT = "\xbd\xc3\xbd\xba\xc5\xdb \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.";
const char *NOTIFY_MSG_UNKNOWN_ERROR = NOTIFY_MSG_UNKNOWN_ERROR_TEXT;
// ghosthandler.cpp:138
// "Corrupted data was found; the game cannot continue."
const char *const NOTIFY_MSG_DATA_ERROR_TEXT = "\xbc\xd5\xbb\xf3\xb5\xc8 \xb5\xa5\xc0\xcc\xc5\xcd\xb0\xa1 \xb9\xdf\xb0\xdf\xb5\xc7\xbe\xee \xb4\xf5\xc0\xcc\xbb\xf3 \xb0\xd4\xc0\xd3\xc0\xbb \xc1\xf8\xc7\xe0\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";
const char *NOTIFY_MSG_DATA_ERROR = NOTIFY_MSG_DATA_ERROR_TEXT;
// ghosthandler.cpp:139
// "The server has too many requests. Please try again later."
const char *const NOTIFY_MSG_NETWAIT_OVER_TEXT = "\xbc\xad\xb9\xf6\xbf\xa1 \xbf\xe4\xc3\xbb\xc0\xcc \xb8\xb9\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xe1\xbd\xc3 \xc8\xc4 \xb4\xd9\xbd\xc3 \xc0\xcc\xbf\xeb\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.";
const char *NOTIFY_MSG_NETWAIT_OVER = NOTIFY_MSG_NETWAIT_OVER_TEXT;
// ghosthandler.cpp:140
// "(Notice) <nick> has entered."
const char *const GHOST_ROOM_JOIN_WELCOME_TEXT = "(\xbe\xcb\xb8\xb2)\\c0xffffffff\\c%s\\c0xffff7878\\c\xb4\xd4\xc0\xcc \xc0\xd4\xc0\xe5\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.";
const char *GHOST_ROOM_JOIN_WELCOME = GHOST_ROOM_JOIN_WELCOME_TEXT;
// ghosthandler.cpp:141
// "A corrupted ghost cannot be replayed. Please play another ghost."
const char *const NOTIFY_MSG_DISABLE_GAME_TEXT = "\xbc\xd5\xbb\xf3\xb5\xc8 \xb5\xa5\xc0\xcc\xc5\xcd\xbf\xcd\xb4\xc2 \xc0\xe7\xb4\xeb\xb0\xe1 \xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xb8\xa5 \xb0\xed\xbd\xba\xc6\xae\xbf\xcd \xb4\xeb\xc0\xfc\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.";
const char *NOTIFY_MSG_DISABLE_GAME = NOTIFY_MSG_DISABLE_GAME_TEXT;
// ghosthandler.cpp:142
// "No error message is defined."
const char *const NOTIFY_MSG_NOT_DEFINED_TEXT = "\xc1\xa4\xc0\xc7\xb5\xc8 \xbf\xc0\xb7\xf9 \xb8\xde\xbc\xbc\xc1\xf6\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";
const char *NOTIFY_MSG_NOT_DEFINED = NOTIFY_MSG_NOT_DEFINED_TEXT;

} // namespace

// Element of the event ghost file's game list; layout not recovered yet.
struct GHOST_GAME_BRIEF;

namespace {

// Reader of the event ghost XML file: the player's nick and the list of
// ghost games it describes.
class CEventGhostXmlFile {
	LAYOUT_CHECK_FRIEND;
public:
	bool Load(void);
	const std::string &GetNick(void) const;
	const std::list<GHOST_GAME_BRIEF> &GetGameList(void) const;

private:
	std::string m_nick;                      // 0x00
	std::list<GHOST_GAME_BRIEF> m_gameList;  // 0x1c
};

CHECK_SIZE(CEventGhostXmlFile, 0x28);
BEGIN_LAYOUT_CHECK(CEventGhostXmlFile)
	CHECK_FIELD(CEventGhostXmlFile, m_nick, 0x00);
	CHECK_FIELD(CEventGhostXmlFile, m_gameList, 0x1c);
END_LAYOUT_CHECK

// ghosthandler.cpp:148
bool CEventGhostXmlFile::Load(void) {
	m_gameList.clear();
	TiXmlDocument doc;
	if (!doc.LoadFileEx("event_ghost.xml")) {
		return false;
	}

	std::string uid;
	std::string fileName;
	TiXmlNode *active = doc.FirstChild("active");
	if (!active) {
		return false;
	}

	TiXmlElement *activeElement = active->ToElement();
	uid = activeElement->Attribute("uid");
	if (uid == "-1") {
		return false;
	}

	TiXmlNode *node = active->FirstChild("element");
	while (node) {
		TiXmlElement *element = node->ToElement();
		if (uid == element->Attribute("uid")) {

			m_nick = element->Attribute("nick");
			TiXmlNode *param = node->FirstChild("param");

			while (param) {
				TiXmlElement *paramElement = param->ToElement();
				fileName = paramElement->Attribute("file");

				if (!fileName.empty()) {

					GHOST_GAME_BRIEF brief; memset(&brief, 0, sizeof(brief));
					GetGhostGameBriefFromFileName(&brief, fileName.c_str());

					brief.m_hasData = 1; m_gameList.push_back(brief);
				}
				param = param->NextSibling();
			}

			return !m_gameList.empty();
		}

		node = node->NextSibling();
	}

	return false;
}

// ghosthandler.cpp:197
const std::string &CEventGhostXmlFile::GetNick(void) const {
	return m_nick;
}

// ghosthandler.cpp:198
const std::list<GHOST_GAME_BRIEF> &CEventGhostXmlFile::GetGameList(void) const {
	return m_gameList;
}

} // namespace

// ghosthandler.cpp:207
CGhostHandler::CGhostHandler(void) {
}

// ghosthandler.cpp:212
CGhostHandler::~CGhostHandler(void) {
}

// ghosthandler.cpp:217
bool CGhostHandler::IsAvailableGhostSystem(void) {
	if (!IsLocalContent((localContentType_t)0x5D)) {
		return false;
	}
	// ghosthandler.cpp:221
	return (bool)!((WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags >> 11) & 1);
}

// ghosthandler.cpp:228
void CGhostHandler::OpenNotifyDlg(int type) {
	FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);

	pForm->SetTopmost(true);
	pForm->FindNextTopFocus(false);
	pForm->SetTimeLimit(7.0f);
	switch (type) {
	case 7: pForm->SetMessage(NOTIFY_MSG_SERVICE_STOP, false); break;
	case 0: pForm->SetMessage(NOTIFY_MSG_GHOST_NOSAVE, false); break;
	case 1: pForm->SetMessage(NOTIFY_MSG_GHOST_NODATA, false); break;
	case 2: pForm->SetMessage(RECORD_DLG_LOADING_FAIL, false); break;
	case 4: pForm->SetMessage(NOTIFY_MSG_UNKNOWN_ERROR, false); break;
	case 5: pForm->SetMessage(NOTIFY_MSG_DATA_ERROR, false); break;
	case 6: pForm->SetMessage(NOTIFY_MSG_NETWAIT_OVER, false); break;
	case 8: pForm->SetMessage(NOTIFY_MSG_DISABLE_GAME, false); break;
	default:
		pForm->SetMessage(NOTIFY_MSG_NOT_DEFINED, false);
		break;
	}

	pForm->Open((FRESH_RESULT_CALLBACK)0, 0);
}

// ghosthandler.cpp:253
bool CGhostHandler::IsExistEventGhost(void) {
	CEventGhostXmlFile file;
	return file.Load();
}

// ghosthandler.cpp:259
bool CGhostHandler::OpenEventGhostGameDlg(void) {
	CEventGhostXmlFile file;
	if (!file.Load()) {
		return false;
	}

	OpenGhostGameDlg(0xFFFFFFFF, file.GetNick(), file.GetGameList(), (FRESH_RESULT_CALLBACK)&CGhostHandler::OnResultEventGhostGameDlg);
	return true;
}

// ghosthandler.cpp:269
void CGhostHandler::OpenGhostGameDlg(unsigned long uid, const std::string &nick, const std::list<GHOST_GAME_BRIEF> &games, FRESH_RESULT_CALLBACK pfnResult) {
	if (g_pFresh->GetDesktop()->FindChildForm("GHOST_GAMEMAP") == 0) {

		FrGhostGameMapDlg *pDlg = CreateForm<FrGhostGameMapDlg>(g_pFresh->GetManager(), this, "GHOST_GAMEMAP", 0);

		pDlg->SetTopmost(true);
		pDlg->FindNextTopFocus(false);
		pDlg->Init(uid, nick, games);
		pDlg->EnableDrag(false);

		pDlg->SetFlag(8);
		pDlg->Open(pfnResult, 1);
	}
}

// ghosthandler.cpp:288
void CGhostHandler::RecordComplete(void) {
	if (GOLFDOC()) {
		int result = PLAYER(MyIndex())->m_ghostResult;
		int award = PLAYER(MyIndex())->m_ghostAward;

		// ghosthandler.cpp:297
		result += (unsigned char)PLAYER(MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1] - GOLFDOC()->GetHoleData().m_par;

		// ghosthandler.cpp:300
		unsigned char character = (unsigned char)WSingleton<CSharedDoc>::Instance()->m_roomPlayers[MyIndex()].m_characterInfo.typeId;

		if (result < 0) {
			WSendPacket packet(GCT_GAME_GHOST);
			packet.Encode1(4);
			packet.Encode1(GetCurMap());
			packet.Encode1(character);
			packet.Encode4(abs(result));
			packet.Encode4(award);
			packet.Send(SEND_TO_DEFAULT);
			return;
		}
	}

	CGhostManager::Instance()->ReleaseDocument();
}

// ghosthandler.cpp:320
void CGhostHandler::LoadingComplete(bool success) {
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
	if (success) {
		// ghosthandler.cpp:324
		CMouseCursor::Instance()->SetActive(false);
		CTaskManager::Instance()->ChangeTask("CGhostTask", "", CTaskManager::Instance()->GetPreservedTask() != 0);
		CTaskManager::Instance()->PostMsg(0, "Ghost", 0, (intptr_t)"GHOST", 0, 0, 0);
	} else {
		// ghosthandler.cpp:330
		FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		pForm->SetTopmost(true);
		pForm->FindNextTopFocus(false);
		pForm->SetTimeLimit(7.0f);
		pForm->SetMessage(RECORD_DLG_LOADING_FAIL, false);
		pForm->Open((FRESH_RESULT_CALLBACK)0, 0);
		SendErrorReport("GHOST FILE LOAD FAILED", CGhostManager::Instance()->GetLatestFileName());
	}
}

// ghosthandler.cpp:336
void CGhostHandler::UploadComplete(bool success, const std::string &fileName) {
	ClearLocalGhostFiles();
	if (success) {
		// ghosthandler.cpp:340
		WSendPacket packet(GCT_GAME_GHOST);
		packet.Encode1(5);
		packet.EncodeStr(fileName);
		packet.Send(SEND_TO_DEFAULT);
	} else {
		// ghosthandler.cpp:347
		SendErrorReport("GHOST FILE UPLOAD FAILED.", fileName);
	}
}

// ghosthandler.cpp:352
void CGhostHandler::OnGhostPacket(WReceivedPacket &packet) {
	if (IsLocalContent((localContentType_t)0x5D) && !(WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x800)) {
		switch (packet.Decode1()) {
		case 12: {
			// ghosthandler.cpp:361
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));

			int count = packet.Decode4();
			unsigned long uid = packet.Decode4();

			if (count == 0) {
				OpenNotifyDlg(0);
			} else {
				std::list<GHOST_GAME_BRIEF> games;
				std::string nick;
				for (int i = 0; i < count; i++) {
					// ghosthandler.cpp:380
					int numbers[5] = { 0 }; for (int j = 0; j < 5; j++) { numbers[j] = packet.Decode4(); } packet.DecodeStr(nick); int hasData = packet.Decode4(); GHOST_GAME_BRIEF brief = { uid, numbers[0], { numbers[1], numbers[2], numbers[3], numbers[4] }, hasData };
					games.push_back(brief);
				}

				// ghosthandler.cpp:384
				OpenGhostGameDlg(uid, nick, games, (FRESH_RESULT_CALLBACK)&CGhostHandler::OnResultGhostGameDlg);
			}
			break;
		}

		case 13: {
			// ghosthandler.cpp:390
			std::string fileName;
			packet.DecodeStr(fileName);

			CGhostManager::Instance()->SaveDocument(fileName);
			break;
		}

		case 14:
			// ghosthandler.cpp:399
			CGhostManager::Instance()->ReleaseDocument();
			ClearLocalGhostFiles();
			break;

		case 16:
			// ghosthandler.cpp:410
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));

			switch (packet.Decode1()) {
			case 28: {
				// ghosthandler.cpp:417
				FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0); pForm->SetTopmost(true); pForm->FindNextTopFocus(false); pForm->SetTimeLimit(7.0f); pForm->SetMessage(NOTIFY_MSG_SERVICE_STOP, false); pForm->Open((FRESH_RESULT_CALLBACK)0, 0);
				break;
			}
			case 25: {
				// ghosthandler.cpp:422
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
				FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0); pForm->SetTopmost(true); pForm->FindNextTopFocus(false); pForm->SetTimeLimit(7.0f); pForm->SetMessage(NOTIFY_MSG_GHOST_NODATA, false); pForm->Open((FRESH_RESULT_CALLBACK)0, 0);
				break;
			}
			case 26: {
				// ghosthandler.cpp:428
				FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0); pForm->SetTopmost(true); pForm->FindNextTopFocus(false); pForm->SetTimeLimit(7.0f); pForm->SetMessage(NOTIFY_MSG_NETWAIT_OVER, false); pForm->Open((FRESH_RESULT_CALLBACK)0, 0);
				break;
			}
			}
			break;

		case 19: {
			// ghosthandler.cpp:448
			packet.Decode1();
			unsigned long typeId = packet.Decode4();
			unsigned long count = packet.Decode4();
			packet.Decode1();

			if (!(typeId & 0x2000000)) {
				AfxGetTask()->GetActor("Item") << MsgObject(0, 0x19F, -1, typeId, count, 0, 0);
			}
			break;
		}

		case 22: {
			// ghosthandler.cpp:463
			FrWnd *pWnd = g_pFresh->GetDesktop()->FindChildByName("GHOST_HISTORY");
			FrRecentMatch *pDlg = (FrRecentMatch *)(pWnd != 0 ? (void *)pWnd : (__rtti_obj = (void *)pWnd));
			if (!pDlg) {
				break;
			}

			// ghosthandler.cpp:468
			if (packet.Decode1() != 0xFF) {
				unsigned long count = packet.Decode4();
				for (unsigned long i = 0; i < count; i++) {
					FrRecentMatch::sRecentMatchForm form;
					form.m_byte00 = packet.Decode1();
					form.m_name = packet.DecodeStr();
					form.m_int20 = packet.Decode4();
					packet.DecodeBuffer(&form.m_time, sizeof(form.m_time));
					form.m_int38 = packet.Decode4();
					form.m_byte24 = packet.Decode1();

					pDlg->AddHistory(form);
				}
				pDlg->AddFinish();
			} else {
				// ghosthandler.cpp:489
				pDlg->Exception();
			}
			break;
		}
		}
	}
}

// ghosthandler.cpp:498
bool CGhostHandler::OnResultGhostGameDlg(int ret, FrForm *pForm) {
	if (ret == 1) {

		FrGhostGameMapDlg *pDlg = DYNAMIC_CAST(FrGhostGameMapDlg, pForm);
		GHOST_GAME_BRIEF *pBrief = (GHOST_GAME_BRIEF *)pDlg->GetSelectItem();
		if (pBrief) {

			std::string fileName(MakeGhostGameFileName(pBrief));

			CGhostManager::Instance()->LoadDocument(fileName);
			CGhostManager::Instance()->SetRecentSelectedGhostNick(pDlg->GetGhostNick());

			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
		} else {

			FrForm *pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			pNotify->SetTopmost(true);
			pNotify->FindNextTopFocus(false);
			pNotify->SetTimeLimit(7.0f);
			pNotify->SetMessage(NOTIFY_MSG_UNKNOWN_ERROR, false);
			pNotify->Open((FRESH_RESULT_CALLBACK)0, 0);
		}
	}

	return true;
}

// ghosthandler.cpp:521
bool CGhostHandler::OnResultEventGhostGameDlg(int ret, FrForm *pForm) {
	if (ret == 1) {

		FrGhostGameMapDlg *pDlg = DYNAMIC_CAST(FrGhostGameMapDlg, pForm);
		GHOST_GAME_BRIEF *pBrief = (GHOST_GAME_BRIEF *)pDlg->GetSelectItem();
		if (pBrief) {

			std::string fileName(MakeGhostGameFileName(pBrief));
			CGhostManager::Instance()->SetRecentSelectedGhostNick(pDlg->GetGhostNick());
			CGhostManager::Instance()->LoadDocumentFromLocalFile(fileName);
		} else {

			FrForm *pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			pNotify->SetTopmost(true);
			pNotify->FindNextTopFocus(false);
			pNotify->SetTimeLimit(7.0f);
			pNotify->SetMessage(NOTIFY_MSG_UNKNOWN_ERROR, false);
			pNotify->Open((FRESH_RESULT_CALLBACK)0, 0);
		}
	}

	return true;
}

// ghosthandler.cpp:541
void CGhostHandler::ClearLocalGhostFiles(void) {
	WIN32_FIND_DATA findData;

	HANDLE hFind = FindFirstFile(".\\temporary_save\\*.GST", &findData);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			DeleteFile(findData.cFileName);
		} while (FindNextFile(hFind, &findData));

		FindClose(hFind);
	}
}

// ghosthandler.cpp:557
void CGhostHandler::SendErrorReport(const std::string &error, const std::string &fileName) {
	WSendPacket packet(GCT_GAME_GHOST);
	packet.Encode1(8);
	packet.EncodeStr(fileName);
	packet.EncodeStr(error);
	packet.Send(SEND_TO_DEFAULT);
}

// ghosthandler.cpp:566
void CGhostHandler::ExceptionFinishGame(void) {
	CGhostManager::Instance()->ReleaseDocument();
	CProjectG::Instance()->FromGolfToLobbySettings();
	FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0); pForm->SetTopmost(true); pForm->FindNextTopFocus(false); pForm->SetTimeLimit(7.0f); pForm->SetMessage(NOTIFY_MSG_DATA_ERROR, false); pForm->Open((FRESH_RESULT_CALLBACK)0, 0);
	CTaskManager::Instance()->ChangeTask("CGhostTask", "", false);
	CTaskManager::Instance()->PostMsg(0, "Ghost", 0, (intptr_t)"GHOST", 1, 0, 0);
}

// ghosthandler.cpp:575
void CGhostHandler::RequestGhostGameList(unsigned long uid) {
	if (IsLocalContent((localContentType_t)0x5D) && !(WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x800)) {
		// ghosthandler.cpp:578
		if (IsLocalContent((localContentType_t)0x59)) {
			WSendPacket packet(GCT_UCC_REQUEST);
			packet.Encode1(2);
			packet.Send(SEND_TO_DEFAULT);
		}

		// ghosthandler.cpp:585
		WSendPacket packet(GCT_GAME_GHOST);
		packet.Encode1(0);
		packet.Encode4(uid);
		packet.Send(SEND_TO_DEFAULT);

		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
	}
}
