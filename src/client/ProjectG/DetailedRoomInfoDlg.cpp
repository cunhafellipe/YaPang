// Original: d:\build\custom\temp\code\client\projectg\detailedroominfodlg.cpp
#include "SharedDoc.h"
#include "Packet.h"
#include "TaskManager.h"
#include "actor.h"
#include "DetailedRoomInfoDlg.h"
#include "ObjectFactory.h"
#include "frbutton.h"
#include "fredit.h"
#include "frlistbox.h"
#include "fresh.h"
#include "PasswordDlg.h"
#include "commonutil.h"
#include "ProjectG.h"
#include "NetResourceManager.h"
#include "frgraphicinterface.h"
#include "frwndmanager.h"
#include "bitmap.h"
#include "user_info.h"
#include "shared/classdefine.h"
#include <stdio.h>
#include <string.h>

typedef std::map<unsigned long, sBriefUserInfo> UserMap;

// detailedroominfodlg.cpp:65 (sort by nickname, ascending; players missing
// from the user map first)
static bool CompareNickAsc(const void* pA, const void* pB) {
	const sRoomUserInfo* a = (const sRoomUserInfo*)pA;
	const sRoomUserInfo* b = (const sRoomUserInfo*)pB;

	UserMap::iterator itA = WSingleton<CSharedDoc>::Instance()->m_userMap.find(a->m_uid);
	UserMap::iterator itB = WSingleton<CSharedDoc>::Instance()->m_userMap.find(b->m_uid);

	if (itA == WSingleton<CSharedDoc>::Instance()->m_userMap.end())
		return true;

	if (itB == WSingleton<CSharedDoc>::Instance()->m_userMap.end())
		return true;




	return strcmp(itA->second.nick, itB->second.nick) < 0 ? true : false;
}

// detailedroominfodlg.cpp:40 (sort by level, or rank in channels with flag
// 0x80, descending)
static bool CompareLevelDesc(const void* pA, const void* pB) {
	const sRoomUserInfo* a = (const sRoomUserInfo*)pA;
	const sRoomUserInfo* b = (const sRoomUserInfo*)pB;

	if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80) {

		if (a->m_rankValue > b->m_rankValue)
			return true;
		else if (a->m_rankValue < b->m_rankValue)
			return false;
	} else {

		if (a->m_level > b->m_level)
			return true;

		else if (a->m_level < b->m_level)
			return false;
	}


	return CompareNickAsc(pA, pB);
}

// detailedroominfodlg.cpp:85 (sort by status, descending)
static bool CompareStatusDesc(const void* pA, const void* pB) {
	const sRoomUserInfo* a = (const sRoomUserInfo*)pA;
	const sRoomUserInfo* b = (const sRoomUserInfo*)pB;

	if ((a->m_status & 0x1f) > (b->m_status & 0x1f))
		return true;
	else if ((a->m_status & 0x1f) < (b->m_status & 0x1f))
		return false;

	return CompareNickAsc(pA, pB);
}

// detailedroominfodlg.cpp:98 (sort by level, or rank in channels with flag
// 0x80, ascending)
static bool CompareLevelAsc(const void* pA, const void* pB) {
	const sRoomUserInfo* a = (const sRoomUserInfo*)pA;
	const sRoomUserInfo* b = (const sRoomUserInfo*)pB;

	if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80) {

		if (a->m_rankValue > b->m_rankValue)
			return false;
		else if (a->m_rankValue < b->m_rankValue)
			return true;
	} else {

		if (a->m_level > b->m_level)
			return false;

		else if (a->m_level < b->m_level)
			return true;
	}


	return CompareNickAsc(pA, pB);
}

// detailedroominfodlg.cpp:123 (sort by nickname, descending; players missing
// from the user map first)
static bool CompareNickDesc(const void* pA, const void* pB) {
	const sRoomUserInfo* a = (const sRoomUserInfo*)pA;
	const sRoomUserInfo* b = (const sRoomUserInfo*)pB;

	UserMap::iterator itA = WSingleton<CSharedDoc>::Instance()->m_userMap.find(a->m_uid);
	UserMap::iterator itB = WSingleton<CSharedDoc>::Instance()->m_userMap.find(b->m_uid);

	if (itA == WSingleton<CSharedDoc>::Instance()->m_userMap.end())
		return true;

	if (itB == WSingleton<CSharedDoc>::Instance()->m_userMap.end())
		return true;




	return strcmp(itA->second.nick, itB->second.nick) >= 0 ? true : false;
}

// detailedroominfodlg.cpp:143 (sort by status, ascending)
static bool CompareStatusAsc(const void* pA, const void* pB) {
	const sRoomUserInfo* a = (const sRoomUserInfo*)pA;
	const sRoomUserInfo* b = (const sRoomUserInfo*)pB;

	if ((a->m_status & 0x1f) > (b->m_status & 0x1f))
		return false;
	else if ((a->m_status & 0x1f) < (b->m_status & 0x1f))
		return true;

	return CompareNickAsc(pA, pB);
}

// detailedroominfodlg.cpp:156 (run-time type and factory registration of FrDtRoomDlg)
const WRTTI FrDtRoomDlg::m_RTTI("FrDtRoomDlg", &FrForm::m_RTTI);

IObject* FrDtRoomDlgMakeInstance(void) {
	return new FrDtRoomDlg;
}

struct __sFrDtRoomDlg {
	__sFrDtRoomDlg(void) {
		ObjectFactory().AddObjectFunctor(FrDtRoomDlgMakeInstance, "FrDtRoomDlg");
	}
};
static __sFrDtRoomDlg s_registerFrDtRoomDlg;

// detailedroominfodlg.cpp:158
const sFRESH_MSGMAP* FrDtRoomDlg::GetMessageMap(void) const {
	return &FrDtRoomDlg::_MsgMap;
}

sFRESH_MSGMAP FrDtRoomDlg::_MsgMap = { &FrForm::_MsgMap, &FrDtRoomDlg::_MsgEntries[0] };

sFRESH_ENTRY FrDtRoomDlg::_MsgEntries[] = {
	// detailedroominfodlg.cpp:160
	{ "gamedesc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrDtRoomDlg::OnGameDescInit },
	// detailedroominfodlg.cpp:161
	{ "join", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrDtRoomDlg::OnJoinBtnInit },
	// detailedroominfodlg.cpp:162
	{ "join", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrDtRoomDlg::OnJoinBtnUp },
	// detailedroominfodlg.cpp:163
	{ "userinfo", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrDtRoomDlg::OnUserInfoBtnInit },
	// detailedroominfodlg.cpp:164
	{ "userinfo", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrDtRoomDlg::OnUserInfoBtnUp },
	// detailedroominfodlg.cpp:165
	{ "level", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrDtRoomDlg::OnLevelHdrInit },
	// detailedroominfodlg.cpp:166
	{ "level", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrDtRoomDlg::OnLevelHdrDown },
	// detailedroominfodlg.cpp:167
	{ "id", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrDtRoomDlg::OnIdHdrInit },
	// detailedroominfodlg.cpp:168
	{ "id", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrDtRoomDlg::OnIdHdrDown },
	// detailedroominfodlg.cpp:169
	{ "status", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrDtRoomDlg::OnStatusHdrInit },
	// detailedroominfodlg.cpp:170
	{ "status", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrDtRoomDlg::OnStatusHdrDown },
	// detailedroominfodlg.cpp:171
	{ "userlist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrDtRoomDlg::OnUserListInit },
	// detailedroominfodlg.cpp:172
	{ "userlist", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrDtRoomDlg::OnUserListOwnerDraw },
	// detailedroominfodlg.cpp:173
	{ "userlist", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrDtRoomDlg::OnUserListLBtnDown },
	// detailedroominfodlg.cpp:174
	{ "userlist", FRCMD_RBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrDtRoomDlg::OnUserListRBtnUp },

	// detailedroominfodlg.cpp:176
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// detailedroominfodlg.cpp:182
FrDtRoomDlg::FrDtRoomDlg(void)
	: m_pGameDesc(0), m_pJoinBtn(0), m_pUserInfoBtn(0), m_pGalleryBtn(0), m_pUserList(0) {
	m_pSelectedUser = 0; m_pIdHdr = 0; m_pLevelHdr = 0; m_pStatusHdr = 0; m_sortState = 1;
}

// detailedroominfodlg.cpp:189
bool FrDtRoomDlg::OnInit(void) {
	m_sortState = HEADER_LEVEL + 3;
	OnHeaderLBtnDown(HEADER_LEVEL);

	return true;
}

// detailedroominfodlg.cpp:197
void FrDtRoomDlg::SetRoomInfo(const sRoomInfo& roomInfo) {
	m_roomInfo = roomInfo;
}

// detailedroominfodlg.cpp:202
void FrDtRoomDlg::OnGameDescInit(intptr_t param) {
	m_pGameDesc = (FrEdit*)((__rtti_obj = (void*)param) != 0 ? (((IObject*)__rtti_obj)->IsKindOf(&FrEdit::m_RTTI) ? (void*)param : 0) : 0);

	char text[256];
	const char* pName;



	switch (CSharedDoc::Instance()->m_gameDesc.m_descGameType) {
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
	case 14: {
		IFF_STRUCT::sMatch* pMatch = ItemManager()->FindMatch(CSharedDoc::Instance()->m_gameDesc.m_descMatchId);

		if (pMatch) {
			pName = pMatch->m_name;
			break;
		}
	}
	default:
		pName = CSharedDoc::Instance()->m_gameTypeNames[CSharedDoc::Instance()->m_gameDesc.m_descGameType].name;
	}


	sprintf(text, "\xb0\xd4\xc0\xd3\xc5\xb8\xc0\xd4 : %s%s", pName, CSharedDoc::Instance()->m_gameDesc.m_descGameType == 5 ? "\xc6\xc0\xc0\xfc" : "");

	m_pGameDesc->SetLine(1, text, 0, false, 0);


	unsigned char course = CSharedDoc::Instance()->m_gameDesc.m_descCourse;
	if (course == 0xff) {

		const char* pRandom = "\xc4\xda\xbd\xba : RANDOM";
		strcpy(text, pRandom);

	} else {

		unsigned long typeId = 0x28000000 | ((course > 0x7F && course != 0xFD) ? course - 0x80 : course);
		IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(typeId);

		if (pCourse) {
			sprintf(text, "\xc4\xda\xbd\xba : %s", pCourse->m_name);
		} else {
			const char* pUnknown = "\xc4\xda\xbd\xba : \xb8\xf0\xb8\xa7";
			strcpy(text, pUnknown);
		}
	}


	m_pGameDesc->SetLine(2, text, 0, false, 0);


















	unsigned char mode = CSharedDoc::Instance()->m_gameDesc.m_descMode;
	if (mode == 2 || mode == 3) {


		sprintf(text, "\xbd\xc3\xc0\xdb\xc8\xa6 : \xb7\xa3\xb4\xfd \xc8\xa6 / \xc8\xa6 \xbc\xf6 : %d \xc8\xa6", CSharedDoc::Instance()->m_gameDesc.m_descHoleCount);

	} else {


		sprintf(text, "\xbd\xc3\xc0\xdb\xc8\xa6 : %d \xc8\xa6 / \xc8\xa6 \xbc\xf6 : %d \xc8\xa6", mode ? 10 : 1, CSharedDoc::Instance()->m_gameDesc.m_descHoleCount);
	}


	m_pGameDesc->SetLine(3, text, 0, false, 0);


	unsigned char type = CSharedDoc::Instance()->m_gameDesc.m_descGameType;
	if (type == 2) {

		text[0] = 0;

	} else {
		switch (type) {
		case 4:
		case 5:
		case 6:
		case 9:
		case 10:
		case 14:
			if (type != 10) {

				sprintf(text, "\xb0\xd4\xc0\xd3\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3 : %d \xba\xd0", CSharedDoc::Instance()->m_gameDesc.m_descGameTime / 60000);
				break;
			}
		default:

			if (CSharedDoc::Instance()->m_gameDesc.m_descGameTime == 0) {
				const char* pNoLimit = "\xc5\xb8\xb1\xb8\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3 : \xbe\xf8\xc0\xbd";
				strcpy(text, pNoLimit);
			} else {


				sprintf(text, "\xc5\xb8\xb1\xb8\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3 : %d \xc3\xca", CSharedDoc::Instance()->m_gameDesc.m_descGameTime / 1000);
			}
		}
	}

	m_pGameDesc->SetLine(4, text, 0, false, 0);
}

// detailedroominfodlg.cpp:294
void FrDtRoomDlg::OnJoinBtnInit(intptr_t param) {
	m_pJoinBtn = DYNAMIC_CAST(FrButton, param);


	if (m_pJoinBtn && (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80)) {
		m_pJoinBtn->Enable(false);
	}
}

// detailedroominfodlg.cpp:304
void FrDtRoomDlg::OnJoinBtnUp(void) {
	// identity bits 1-3 all set (gallery, administrator and 8): join as an observer
	int bObserver = (WSingleton<CSharedDoc>::Instance()->m_userInfo.identity & 0xE) == 0xE ? 1 : 0;
	if (bObserver) {

		OnGalleryBtnUp();
		return;
	}














	if (m_roomInfo.m_intrusionAccess) {

		if (!AdminMode()) {

			if (m_roomInfo.m_userCount == m_roomInfo.m_userLimit) {

				AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 0x23, (intptr_t)"\xc1\xa4\xbf\xf8\xc0\xcc \xc3\xca\xb0\xfa \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9", 0, 0, 0, 0));
				return;
			}


			if (m_roomInfo.m_gameType == 5) {

				UserMap::iterator it = WSingleton<CSharedDoc>::Instance()->m_userMap.find(MyGuid(false));
				if (it != WSingleton<CSharedDoc>::Instance()->m_userMap.end() && it->second.charType > 1) {

					AfxGetTask()->GetActor("Lobby") << MsgObject(0, 0x23, (intptr_t)"\xb0\xad\xc1\xa6 \xc1\xbe\xb7\xe1\xc0\xb2\xc0\xcc \xb3\xf4\xc0\xba \xc0\xaf\xc0\xfa\xb4\xc2 30\xc0\xce \xc6\xc0\xc0\xfc \xb0\xd4\xc0\xd3\xc0\xbb \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0);
					return;
				}
			}
		}

















		if (m_roomInfo.m_gameType == 6 && !WSingleton<CSharedDoc>::Instance()->m_userInfo.IsIdentity(0x14)) {

			std::string message;

			unsigned long guildUID = WSingleton<CSharedDoc>::Instance()->m_userInfo.guildUID;
			bool bRefuse = false;

			if (guildUID == 0) {

				message = "\xb1\xe6\xb5\xe5\xbf\xa1 \xb0\xa1\xc0\xd4\xc7\xcf\xbc\xc5\xbe\xdf \xb1\xe6\xb5\xe5\xb4\xeb\xc0\xfc\xc0\xcc \xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.";
				bRefuse = true;
			}





			if (m_roomInfo.m_guild.m_guildUID[0] != guildUID && m_roomInfo.m_guild.m_guildUID[0] != 0 && m_roomInfo.m_guild.m_guildUID[1] != guildUID && m_roomInfo.m_guild.m_guildUID[1] != 0) {

				message = "\xc5\xb8\xb1\xe6\xb5\xe5\xc0\xc7 \xb4\xeb\xc0\xfc\xc0\xd4\xb4\xcf\xb4\xd9.";
				bRefuse = true;
			}


			if (bRefuse) {

				AfxGetTask()->GetActor("Lobby") << MsgObject(0, 0x23, (intptr_t)message.c_str(), 0, 0, 0, 0);
				return;
			}
		}

		if (m_roomInfo.m_isPublic || WSingleton<CSharedDoc>::Instance()->m_userInfo.IsIdentity(0x14)) {














			if (m_pJoinBtn)
				m_pJoinBtn->Enable(false);
			{
				WSendPacket packet(GCT_REQUEST_JOIN_SESSION);
				packet.Encode2(m_roomInfo.m_roomNumber);
				packet.EncodeStr(std::string(""));
				packet.Send(SEND_TO_DEFAULT);

				AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 0x16, 0, 0, 0, 0, 0));
				AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 1, 0, 0, 0, 0, 0));
			}

			Close(FrOK, true);
		} else {



			FrPasswordDlg* pDlg = CreateForm<FrPasswordDlg>(g_pFresh->GetManager(), this, "password", 0);
			pDlg->Open((FRESH_RESULT_CALLBACK)&FrDtRoomDlg::OnPasswordDlgResult, 3);
		}
	} else {





		AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 0x23, (intptr_t)"\xb0\xd4\xc0\xd3 \xc1\xf8\xc7\xe0\xc1\xdf\xc0\xce \xb9\xe6\xc0\xd4\xb4\xcf\xb4\xd9", 0, 0, 0, 0));
	}
}

// detailedroominfodlg.cpp:431
void FrDtRoomDlg::OnGalleryBtnInit(int param) {
	m_pGalleryBtn = DYNAMIC_CAST(FrButton, param);
}

// detailedroominfodlg.cpp:436
void FrDtRoomDlg::OnGalleryBtnUp(void) {
	if (m_roomInfo.m_intrusionAccess) {

		if (m_roomInfo.m_galleryCount == m_roomInfo.m_galleryLimit && !AdminMode()) {

			AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 0x23, (intptr_t)"\xc1\xa4\xbf\xf8\xc0\xcc \xc3\xca\xb0\xfa \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9", 0, 0, 0, 0));
			return;
		}


		if (m_roomInfo.m_isPublic || AdminMode()) {

			if (m_pGalleryBtn)
				m_pGalleryBtn->Enable(false);

			WSendPacket packet((enumClientPacket)0x3E);
			packet.Encode2(m_roomInfo.m_roomNumber);
			packet.EncodeStr(std::string(""));
			packet.Send(SEND_TO_DEFAULT);

			AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 0x16, 0, 0, 0, 0, 0));
			AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 1, 0, 0, 0, 0, 0));

			Close(FrOK, true);
		} else {



			FrPasswordDlg* pDlg = CreateForm<FrPasswordDlg>(g_pFresh->GetManager(), this, "password", 0);
			pDlg->Open((FRESH_RESULT_CALLBACK)&FrDtRoomDlg::OnPasswordDlgResult, 3);
		}
	} else {



		AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 0x23, (intptr_t)"\xb0\xd4\xc0\xd3 \xc1\xf8\xc7\xe0\xc1\xdf\xc0\xce \xb9\xe6\xc0\xd4\xb4\xcf\xb4\xd9", 0, 0, 0, 0));
	}
}

// detailedroominfodlg.cpp:471
void FrDtRoomDlg::EnableJoinBtn(bool bEnable) {
	if (m_pJoinBtn) {
		m_pJoinBtn->Enable(bEnable && !(WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80) ? true : false);
	}
}

// detailedroominfodlg.cpp:477
void FrDtRoomDlg::OnProc(const float dt) {
	std::list<void*>::iterator it;

	for (it = m_delList.begin(); it != m_delList.end(); ) {
		m_pUserList->DelItem(*it);
		it = m_delList.erase(it);
	}
}

// detailedroominfodlg.cpp:487
void FrDtRoomDlg::OnUserInfoBtnInit(intptr_t param) {
	m_pUserInfoBtn = DYNAMIC_CAST(FrButton, param);
	m_pUserInfoBtn->Enable(false);
}

// detailedroominfodlg.cpp:493
void FrDtRoomDlg::OnUserInfoBtnUp(void) {
	if (WSingleton<CUserInfo>::Instance()) {

		FrListItem* pItem = m_pUserList->GetSelected();

		if (pItem) {


			sRoomUserInfo* pUser = (sRoomUserInfo*)pItem->pData;

			UserMap::iterator it = WSingleton<CSharedDoc>::Instance()->m_userMap.find(pUser->m_uid);
			if (it != WSingleton<CSharedDoc>::Instance()->m_userMap.end()) {

				WSingleton<CUserInfo>::Instance()->SetInfo(it->second.id, pUser->m_uid, true, true, false, false, it->second.nick);
			}
		}
	}
}

// detailedroominfodlg.cpp:512
void FrDtRoomDlg::OnLevelHdrInit(intptr_t param) {
	m_pLevelHdr = DYNAMIC_CAST(FrButton, param);
}

// detailedroominfodlg.cpp:517
void FrDtRoomDlg::OnLevelHdrDown(void) {
	OnHeaderLBtnDown(HEADER_LEVEL);
}

// detailedroominfodlg.cpp:522
void FrDtRoomDlg::OnIdHdrInit(intptr_t param) {
	m_pIdHdr = DYNAMIC_CAST(FrButton, param);
}

// detailedroominfodlg.cpp:527
void FrDtRoomDlg::OnIdHdrDown(void) {
	OnHeaderLBtnDown(HEADER_ID);
}

// detailedroominfodlg.cpp:532
void FrDtRoomDlg::OnStatusHdrInit(intptr_t param) {
	m_pStatusHdr = DYNAMIC_CAST(FrButton, param);
}

// detailedroominfodlg.cpp:537
void FrDtRoomDlg::OnStatusHdrDown(void) {
	OnHeaderLBtnDown(HEADER_STATUS);
}

// detailedroominfodlg.cpp:542
void FrDtRoomDlg::OnHeaderLBtnDown(eHeader header) {
	if (m_pUserList) {


		if (header == m_sortState) {

			m_sortState = header + 3;

			switch (header) {
			case HEADER_LEVEL:

				m_pUserList->SortItem(CompareLevelAsc);
				break;
			case HEADER_ID:

				m_pUserList->SortItem(CompareNickDesc);
				break;
			case HEADER_STATUS:

				m_pUserList->SortItem(CompareStatusAsc);
				break;
			}
		} else {

			m_sortState = header;

			switch (header) {
			case HEADER_LEVEL:

				m_pUserList->SortItem(CompareLevelDesc);
				break;
			case HEADER_ID:

				m_pUserList->SortItem(CompareNickAsc);
				break;
			case HEADER_STATUS:

				m_pUserList->SortItem(CompareStatusDesc);
				break;
			}
		}
	}
}

// detailedroominfodlg.cpp:587
void FrDtRoomDlg::OnUserListInit(intptr_t param) {
	m_pUserList = (FrListBox*)((__rtti_obj = (void*)param) != 0 ? (((IObject*)__rtti_obj)->IsKindOf(&FrListBox::m_RTTI) ? (void*)param : 0) : 0);
	m_pUserList->UseRightButton(true);

	m_pCharIcon[0] = m_pUserList->GetBitmap("i_male");
	m_pCharIcon[1] = m_pUserList->GetBitmap("i_female");
	m_pCharIcon[2] = m_pUserList->GetBitmap("i_male_02");
	m_pCharIcon[3] = m_pUserList->GetBitmap("i_female_02");
	m_pCharIcon[4] = m_pUserList->GetBitmap("i_male_03");
	m_pCharIcon[5] = m_pUserList->GetBitmap("i_female_03");
















	m_pMannerIcon[0] = m_pUserList->GetBitmap("i_male_manner");
	m_pMannerIcon[1] = m_pUserList->GetBitmap("i_female_manner");


	m_pAngelIcon[0] = m_pUserList->GetBitmap("i_male_angel");
	m_pAngelIcon[1] = m_pUserList->GetBitmap("i_female_angel");






	m_pUserList->ClearItem();


	std::list<sRoomUserInfo>& userList = WSingleton<CSharedDoc>::Instance()->m_roomUserList; for (std::list<sRoomUserInfo>::iterator it = userList.begin(); it != userList.end(); ++it) {

		// a hidden player is listed only when found in the user map with the bit 0x3E set
		if (!AdminMode() && (it->m_flags & 0x14)) {

			UserMap::iterator found = WSingleton<CSharedDoc>::Instance()->m_userMap.find(it->m_uid);
			if (found != WSingleton<CSharedDoc>::Instance()->m_userMap.end()) {

				if (!sBriefUserInfo(found->second).invisible)
					continue;
			}
		}

		m_pUserList->AddItem(&*it);
	}

}

// detailedroominfodlg.cpp:647
void FrDtRoomDlg::OnUserListOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == 0)
		return;

	sRoomUserInfo* pUser = (sRoomUserInfo*)pItem->pData;
	if (pUser == 0)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0)
		return;

	UserMap::iterator it = WSingleton<CSharedDoc>::Instance()->m_userMap.find(pUser->m_uid);

	if (it == WSingleton<CSharedDoc>::Instance()->m_userMap.end()) {
		m_delList.push_back(pItem->pData);
		return;
	}






	unsigned long color = (it->second.identity & 0x14) ? 0x50FFFFFF : 0xFFFFFFFF;




	if (pItem->underCursor) {

		pGDI->SetTextColor(0xFFFFFFFF, 0xFF808080);
		pGDI->SetTextStyle(2);

	} else {

		if (it->second.unknownBit)
			pGDI->SetTextColor(0xFF808080, 0xFFFFFFFF);
		else
			pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

		pGDI->SetTextStyle(0);
	}

















	const Bitmap* pIcon;
	if (it->second.angel)
		pIcon = m_pAngelIcon[it->second.charType % 2];
	else if (it->second.manner)
		pIcon = m_pMannerIcon[it->second.charType % 2];
	else
		pIcon = m_pCharIcon[it->second.charType];


	if (pUser == m_pSelectedUser) {





		pGDI->Box(WRect(pItem->pos.x, pItem->pos.y, (float)m_pUserList->GetItemWidth(), (float)m_pUserList->GetItemHeight() + 2.0f), 0x999B9DFF, 0, 0);
	}





	pGDI->DrawTexture(pIcon, WRect(pItem->pos.x + 58.0f, pItem->pos.y, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);




	const Bitmap* pLevel;
	if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80) {

		pLevel = g_pFresh->GetManager()->GetBitmap("LEVELS", MakeStr("ladder_%03d", pUser->m_rankValue / 100));

	} else {

		if (it->second.titleTypeId) {

			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(it->second.titleTypeId);
			if (pSkin)
				pLevel = g_pFresh->GetBitmap(pSkin->m_icon);
			else
				pLevel = g_pFresh->GetManager()->GetBitmap("LEVELS", MakeStr("level_%03d", pUser->m_level + 1));
		} else {
			pLevel = g_pFresh->GetManager()->GetBitmap("LEVELS", MakeStr("level_%03d", pUser->m_level + 1));
		}
	}











	if (pLevel) {




		pGDI->DrawTexture(pLevel, WRect(pItem->pos.x + 28.0f - float2int(pLevel->Width() * 0.5f), pItem->pos.y + 13.0f - float2int(pLevel->Height() * 0.5f), (float)pLevel->Width(), (float)pLevel->Height()), 0xFFFFFFFF, 0);
	}



	if (it->second.guildUID && !(it->second.identity & 0x14)) {


		const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(it->second.guildMark);




		if (pEmblem) {

			pGDI->DrawTexture(pEmblem, WRect(pItem->pos.x + 75.0f, pItem->pos.y + 1.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
		}
	}



	if (!WSingleton<CProjectG>::Instance()->HidePrivacy())
		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 98.0f, pItem->pos.y + 7.0f), 0, it->second.nick, -1.0f, color);

	if ((pUser->m_status & 0x1f) == 0)
		pGDI->Print(WPoint(pItem->pos.x + 207.0f, pItem->pos.y + 7.0f), 1, it->second.unknownBit ? "\xc0\xe1\xbc\xf6\xc1\xdf" : "\xb4\xeb\xb1\xe2\xc1\xdf");
	else if ((pUser->m_status & 0x1f) == 0x13)
		pGDI->Print(WPoint(pItem->pos.x + 207.0f, pItem->pos.y + 7.0f), 1, "End");

	else
		pGDI->Print(WPoint(pItem->pos.x + 207.0f, pItem->pos.y + 7.0f), 1, "%d \xc8\xa6", pUser->m_status & 0x1f);
}

// detailedroominfodlg.cpp:791
void FrDtRoomDlg::OnUserListLBtnDown(void) {
	FrListItem* pItem = m_pUserList->GetItemUnderCursor();
	if (pItem) {

		sRoomUserInfo* pUser = (sRoomUserInfo*)pItem->pData;
		m_pUserList->SelectItem(pItem, true);



		m_pSelectedUser = pUser;
		if (m_pUserInfoBtn) m_pUserInfoBtn->Enable(true);
	}
}

// detailedroominfodlg.cpp:806
void FrDtRoomDlg::OnUserListRBtnUp(void) {
	OnUserListLBtnDown();


	if (m_pSelectedUser && WSingleton<CUserInfo>::Instance()) {

		UserMap::iterator it = WSingleton<CSharedDoc>::Instance()->m_userMap.find(m_pSelectedUser->m_uid);
		if (it != WSingleton<CSharedDoc>::Instance()->m_userMap.end()) {

			WSingleton<CUserInfo>::Instance()->SetInfo(it->second.id, m_pSelectedUser->m_uid, true, true, false, false, it->second.nick);
		}
	}
}

// detailedroominfodlg.cpp:820
bool FrDtRoomDlg::OnPasswordDlgResult(int result, FrForm* pForm) {
	if (result == FrOK) {

		FrWnd* pWnd = pForm->FindChildByName("password");

		FrEdit* pEdit = DYNAMIC_CAST(FrEdit, pWnd);

		std::string password;
		if (pEdit) {

			const char* pPassword = pEdit->GetLine(1, false);
			password = pPassword;
			AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 0x27, (intptr_t)pPassword, 0, 0, 0, 0));
		}


		FrWnd* pOk = pForm->FindChildByName("ok");
		if (pOk) {
			pOk->Enable(false);
		}

		WSendPacket packet(GCT_REQUEST_JOIN_SESSION);
		packet.Encode2(m_roomInfo.m_roomNumber);
		packet.EncodeStr(password);
		packet.Send(SEND_TO_DEFAULT);

		AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 0x16, 0, 0, 0, 0, 0));
		AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 1, 0, 0, 0, 0, 0));

		Close(FrOK, true);
	}


	return true;
}
