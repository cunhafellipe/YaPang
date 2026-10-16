// Original: d:\build\custom\temp\code\client\projectg\userlistdlg.cpp
#include "UserListDlg.h"
#include "AddFriendDlg.h"
#include "ObjectFactory.h"
#include "SharedDoc.h"
#include "Localize.h"
#include "frbutton.h"
#include "frlistbox.h"
#include "frscrollbar.h"
#include "TaskManager.h"
#include "actor.h"
#include "fresh.h"
#include "user_info.h"
#include "commonutil.h"
#include "Packet.h"
#include "ProjectG.h"
#include "NetResourceManager.h"
#include "frgraphicinterface.h"
#include "frwndmanager.h"
#include "bitmap.h"

#include <string.h>

// userlistdlg.cpp:35 (run-time type and factory registration of FrUserListDlg)
const WRTTI FrUserListDlg::m_RTTI("FrUserListDlg", &FrForm::m_RTTI);

IObject* FrUserListDlgMakeInstance(void) {
	return new FrUserListDlg;
}

struct __sFrUserListDlg {
	__sFrUserListDlg(void) {
		ObjectFactory().AddObjectFunctor(FrUserListDlgMakeInstance, "FrUserListDlg");
	}
};
static __sFrUserListDlg s_registerFrUserListDlg;

// userlistdlg.cpp:37
const sFRESH_MSGMAP* FrUserListDlg::GetMessageMap(void) const {
	return &FrUserListDlg::_MsgMap;
}

sFRESH_MSGMAP FrUserListDlg::_MsgMap = { &FrForm::_MsgMap, &FrUserListDlg::_MsgEntries[0] };

sFRESH_ENTRY FrUserListDlg::_MsgEntries[] = {
	// userlistdlg.cpp:39
	{ "invite", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserListDlg::OnInviteBtnInit },
	// userlistdlg.cpp:40
	{ "invite", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserListDlg::OnInviteBtnUp },
	// userlistdlg.cpp:41
	{ "whisper", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserListDlg::OnWhisperBtnInit },
	// userlistdlg.cpp:42
	{ "whisper", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserListDlg::OnWhisperBtnUp },
	// userlistdlg.cpp:43
	{ "addfriend", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserListDlg::OnFriendBtnUp },
	// userlistdlg.cpp:44
	{ "sex", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserListDlg::OnSortBtnInit },
	// userlistdlg.cpp:45
	{ "sex", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserListDlg::OnSexBtnUp },
	// userlistdlg.cpp:46
	{ "level", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserListDlg::OnSortBtnInit },
	// userlistdlg.cpp:47
	{ "level", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserListDlg::OnLevelBtnUp },
	// userlistdlg.cpp:48
	{ "guild", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserListDlg::OnSortBtnInit },
	// userlistdlg.cpp:49
	{ "guild", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserListDlg::OnGuildBtnUp },
	// userlistdlg.cpp:50
	{ "id", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserListDlg::OnSortBtnInit },
	// userlistdlg.cpp:51
	{ "id", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserListDlg::OnIdBtnUp },
	// userlistdlg.cpp:52
	{ "user", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserListDlg::OnUserListInit },
	// userlistdlg.cpp:53
	{ "user", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserListDlg::OnUserListOwnerDraw },
	// userlistdlg.cpp:54
	{ "user", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrUserListDlg::OnUserListLBtnDown },
	// userlistdlg.cpp:55
	{ "user", FRCMD_RBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserListDlg::OnUserListRBtnUp },
	// userlistdlg.cpp:56
	{ "user", FRCMD_DBLCLICK, FrFuncVV, (FRESH_PMSG)&FrUserListDlg::OnUserListDClick },

	// userlistdlg.cpp:58
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// userlistdlg.cpp:65
FrUserListDlg::FrUserListDlg(void) {
	m_pWhisperBtn = 0; m_pInviteBtn = 0; m_pUserList = 0;
	m_selectedUID = 0xffffffff;
	m_pInviteDelayDlg = 0;
	m_selectedID = 0xffffffff;
	m_sortType = 3;
	m_sortReverse = false;

	m_pAddFriendDlg = 0;
}

// userlistdlg.cpp:77
void FrUserListDlg::MakeUserList(std::map<unsigned long, sBriefUserInfo>& userMap) {
	m_pUserList->ClearItem();
	for (std::map<unsigned long, sBriefUserInfo>::iterator it = userMap.begin(); it != userMap.end(); ++it) {
		// leave the local player out
		if (it->second.uid == WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().guid)
			continue;

		// only an administrator sees the users with identity 0x14
		if (!AdminMode() && (it->second.identity & 0x14))
			continue;

		m_pUserList->AddItem(&it->second);
	}
}

// userlistdlg.cpp:92
void FrUserListDlg::OnProc(const float dt) {
	if (m_pInviteDelayDlg) {
		m_inviteDelayTime -= dt;

		if (m_inviteDelayTime <= 0.0f) {
			m_pInviteDelayDlg->Close(true);
			m_pInviteDelayDlg = 0;
		}
	}
}

// userlistdlg.cpp:109
bool UserList_UserSexCompare(const void* a, const void* b) {
	const sBriefUserInfo* pA = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pB = (const sBriefUserInfo*)b;

	return pA->charType % 2 > pB->charType % 2;
}

// userlistdlg.cpp:117
bool UserList_UserSexRevCompare(const void* a, const void* b) {
	const sBriefUserInfo* pA = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pB = (const sBriefUserInfo*)b;

	return pA->charType % 2 < pB->charType % 2;
}

// userlistdlg.cpp:125
bool UserList_UserLevelCompare(const void* a, const void* b) {
	const sBriefUserInfo* pA = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pB = (const sBriefUserInfo*)b;

	if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80)
		return pA->rankValue > pB->rankValue;

	return pA->level > pB->level;
}

// userlistdlg.cpp:136
bool UserList_UserLevelRevCompare(const void* a, const void* b) {
	const sBriefUserInfo* pA = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pB = (const sBriefUserInfo*)b;

	if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80)
		return pA->rankValue < pB->rankValue;

	return pA->level < pB->level;
}

// userlistdlg.cpp:147
bool UserList_UserNickCompare(const void* a, const void* b) {
	const sBriefUserInfo* pA = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pB = (const sBriefUserInfo*)b;

	return _strcmpi(pA->nick, pB->nick) < 0;
}

// userlistdlg.cpp:155
bool UserList_UserNickRevCompare(const void* a, const void* b) {
	const sBriefUserInfo* pA = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pB = (const sBriefUserInfo*)b;

	return _strcmpi(pA->nick, pB->nick) > 0;
}

// userlistdlg.cpp:163
bool UserList_UserGuildRevCompare(const void* a, const void* b) {
	const sBriefUserInfo* pA = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pB = (const sBriefUserInfo*)b;

	return pA->guildUID < pB->guildUID;
}

// userlistdlg.cpp:171
bool UserList_UserGuildCompare(const void* a, const void* b) {
	const sBriefUserInfo* pA = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pB = (const sBriefUserInfo*)b;

	return pA->guildUID > pB->guildUID;
}

// userlistdlg.cpp:179
void FrUserListDlg::OnSortBtnInit(intptr_t param) {
	FrButton* pButton = DYNAMIC_CAST(FrButton, param);
	pButton->SetPushDelay(0.0f);
}

// userlistdlg.cpp:185
void FrUserListDlg::OnSexBtnUp(void) {
	if (m_sortType != 0) {
		m_sortType = 0;
	} else {
		m_sortReverse = !m_sortReverse;
	}

	SortUserList();

	if (m_pUserList && m_pUserList->GetScrollBar()) {
		m_pUserList->GetScrollBar()->ScrollToFirst();
	}
}

// userlistdlg.cpp:200
void FrUserListDlg::OnLevelBtnUp(void) {
	if (m_sortType != 1) {
		m_sortType = 1;
	} else {
		m_sortReverse = !m_sortReverse;
	}

	SortUserList();

	if (m_pUserList && m_pUserList->GetScrollBar()) {
		m_pUserList->GetScrollBar()->ScrollToFirst();
	}
}

// userlistdlg.cpp:225
void FrUserListDlg::OnGuildBtnUp(void) {
	if (m_sortType != 2) {
		m_sortType = 2;
	} else {
		m_sortReverse = !m_sortReverse;
	}

	SortUserList();

	if (m_pUserList && m_pUserList->GetScrollBar()) {
		m_pUserList->GetScrollBar()->ScrollToFirst();
	}
}

// userlistdlg.cpp:241
void FrUserListDlg::OnIdBtnUp(void) {
	if (m_sortType != 3) {
		m_sortType = 3;
	} else {
		m_sortReverse = !m_sortReverse;
	}

	SortUserList();

	if (m_pUserList && m_pUserList->GetScrollBar()) {
		m_pUserList->GetScrollBar()->ScrollToFirst();
	}
}

// userlistdlg.cpp:256
void FrUserListDlg::SortUserList(void) {
	if (m_pUserList) {
		switch (m_sortType) {
		case 0:
			if (m_sortReverse) {
				m_pUserList->SortItem(UserList_UserSexRevCompare);
			} else {
				m_pUserList->SortItem(UserList_UserSexCompare);
			}
			break;
		case 1:
			if (m_sortReverse) {
				m_pUserList->SortItem(UserList_UserLevelRevCompare);
			} else {
				m_pUserList->SortItem(UserList_UserLevelCompare);
			}
			break;
		case 2:
			if (m_sortReverse) {
				m_pUserList->SortItem(UserList_UserNickRevCompare);
			} else {
				m_pUserList->SortItem(UserList_UserNickCompare);
			}
			break;
		case 3:
			if (m_sortReverse) {
				m_pUserList->SortItem(UserList_UserGuildRevCompare);
			} else {
				m_pUserList->SortItem(UserList_UserGuildCompare);
			}
			break;
		}
	}
}

// userlistdlg.cpp:294
void FrUserListDlg::EnableRightButton(bool enable) {
	m_pUserList->UseRightButton(enable);
}

// userlistdlg.cpp:299
void FrUserListDlg::OnUserListInit(intptr_t param) {
	m_pUserList = (FrListBox*)param;
	if (m_pUserList) {
		m_pUserList->UseRightButton(true);

		m_pSexBmp[0] = m_pUserList->GetBitmap("i_male");
		m_pSexBmp[1] = m_pUserList->GetBitmap("i_female");
		m_pSexBmp[2] = m_pUserList->GetBitmap("i_male_02");
		m_pSexBmp[3] = m_pUserList->GetBitmap("i_female_02");
		m_pSexBmp[4] = m_pUserList->GetBitmap("i_male_03");
		m_pSexBmp[5] = m_pUserList->GetBitmap("i_female_03");

		m_pMannerBmp[0] = m_pUserList->GetBitmap("i_male_manner");
		m_pMannerBmp[1] = m_pUserList->GetBitmap("i_female_manner");

		m_pAngelBmp[0] = m_pUserList->GetBitmap("i_male_angel");
		m_pAngelBmp[1] = m_pUserList->GetBitmap("i_female_angel");

		m_pInGameBmp = m_pUserList->GetBitmap("ingame");
	}
}

// userlistdlg.cpp:343
void FrUserListDlg::OnUserListOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == 0)
		return;

	sBriefUserInfo* pUser = (sBriefUserInfo*)pItem->pData;
	if (pUser == 0)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	const Bitmap* pSex;
	if (pUser->angel) {
		pSex = m_pAngelBmp[pUser->charType % 2];
	} else if (pUser->manner) {
		pSex = m_pMannerBmp[pUser->charType % 2];
	} else {
		pSex = m_pSexBmp[pUser->charType];
	}

	WRect rect(pItem->pos.x, pItem->pos.y, (float)pSex->Width(), (float)pSex->Height());

	if (pUser->uid == m_selectedUID)
		pGDI->Box(WRect(rect.x, rect.y, (float)m_pUserList->GetItemWidth(), (float)m_pUserList->GetItemHeight()), 0x999B9DFF, 0, 0);

	pGDI->DrawTexture(pSex, rect, 0xFFFFFFFF, 0);

	if (pUser->roomNumber != 0xFFFF)
		pGDI->DrawTexture(m_pInGameBmp, WRect(rect.x, rect.y + 3.0f, (float)m_pInGameBmp->Width(), (float)m_pInGameBmp->Height()), 0xFFFFFFFF, 0);

	const Bitmap* pLevel;
	if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80) {
		pLevel = g_pFresh->GetManager()->GetBitmap("LEVELS", MakeStr("ladder_%03d", pUser->rankValue / 100));
	} else {
		const char* levelName;
		if (pUser->titleTypeId) {
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pUser->titleTypeId);
			if (pSkin)
				levelName = pSkin->m_icon;
			else
				levelName = MakeStr("level_%03d", pUser->level + 1);
		} else {
			levelName = MakeStr("level_%03d", pUser->level + 1);
		}
		pLevel = g_pFresh->GetBitmap(levelName);
	}

	if (pLevel) {
		pGDI->DrawTexture(pLevel, WRect(pItem->pos.x + 50.0f - float2int(pLevel->Width() * 0.5f), pItem->pos.y + 13.0f - float2int(pLevel->Height() * 0.5f), (float)pLevel->Width(), (float)pLevel->Height()), 0xFFFFFFFF, 0);
	}

	if (pUser->guildUID && !(pUser->identity & 0x14)) {
		const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pUser->guildMark);

		if (pEmblem)
			pGDI->DrawTexture(pEmblem, WRect(pItem->pos.x + 85.0f, pItem->pos.y + 7.0f - 6.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
	}

	if (WSingleton<CProjectG>::Instance()->HidePrivacy())
		return;

	unsigned long color = (pUser->identity & 0x14) ? 0x80FFFFFF : 0xFFFFFFFF;

	if (pItem->underCursor) {
		pGDI->SetTextColor(0xFFFFFFFF, 0xFF808080);
		pGDI->SetTextStyle(2);
	} else {
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGDI->SetTextStyle(0);
	}

	g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 112.0f, pItem->pos.y + 7.0f), 0, pUser->nick, -1.0f, color);
}

// userlistdlg.cpp:467
void FrUserListDlg::OnUserListLBtnDown(void) {
	FrListItem* pItem = m_pUserList->GetItemUnderCursor();
	if (pItem == 0)
		return;

	sBriefUserInfo* pInfo = (sBriefUserInfo*)pItem->pData;
	m_selectedUID = pInfo->uid;
	m_selectedID = pInfo->id;

	if (m_pInviteBtn) {
		m_pInviteBtn->Enable(m_selectedUID != WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().guid && pInfo->roomNumber == 0xFFFF && !(WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80));
	}

	if (m_pWhisperBtn)
		m_pWhisperBtn->Enable(m_selectedUID != WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().guid);

	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 6)
		m_pInviteBtn->Enable(false);
}

// userlistdlg.cpp:491
void FrUserListDlg::OnUserListRBtnUp(void) {
	FrListItem* pItem = m_pUserList->GetItemUnderCursor();
	if (pItem == 0)
		return;

	sBriefUserInfo* pInfo = (sBriefUserInfo*)pItem->pData;

	if (pInfo == 0)
		return;

	m_selectedUID = pInfo->uid;
	m_selectedID = pInfo->id;

	if (m_pInviteBtn) {
		m_pInviteBtn->Enable(!GalleryMode() && pInfo->roomNumber == 0xFFFF && !(WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80));
	}

	if (m_pWhisperBtn)
		m_pWhisperBtn->Enable(m_selectedUID != WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().guid);

	if (CUserInfo::Instance()) {
		std::map<unsigned long, sBriefUserInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_userMap.find(m_selectedUID);
		if (it != WSingleton<CSharedDoc>::Instance()->m_userMap.end()) {
			CUserInfo::Instance()->SetInfo(it->second.id, m_selectedUID, true, true, false, false, it->second.nick);
		}
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 6)
		m_pInviteBtn->Enable(false);
}

// userlistdlg.cpp:527
void FrUserListDlg::OnUserListDClick(void) {
	if ((WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().identity & 0xe) == 0xe) {
		OnInviteBtnUp();
	}
}

// userlistdlg.cpp:533
void FrUserListDlg::OnInviteBtnInit(intptr_t param) {
	m_pInviteBtn = (FrButton*)param;

	if (m_pInviteBtn) {
		m_pInviteBtn->Enable(false);
	}
}

// userlistdlg.cpp:541
void FrUserListDlg::OnInviteBtnUp(void) {
	if (m_selectedUID == -1 || m_selectedID == -1)
		return;

	std::map<unsigned long, sBriefUserInfo>::iterator it = CSharedDoc::Instance()->m_userMap.find(m_selectedUID);
	if (it == CSharedDoc::Instance()->m_userMap.end())
		return;

	if (CSharedDoc::Instance()->IsInviteMode()) {
		// "an invitation is already pending; please try again later."
		AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 0x23, (intptr_t)"\xc3\xca\xb4\xeb \xbf\xe4\xc3\xbb\xc1\xdf\xbf\xa1 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xe1\xbd\xc3\xc8\xc4 \xb4\xd9\xbd\xc3 \xc0\xcc\xbf\xeb\xc7\xcf\xbc\xbc\xbf\xe4.", 0xFF000000, 0, 0, 0));
		return;
	}

	m_inviteDelayTime = 3.0f;
	if (m_pInviteDelayDlg == 0) {
		m_pInviteDelayDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "wait", 0);
		// "sent an invitation message to %s"
		m_pInviteDelayDlg->SetMessage(MakeStr("%s\xb4\xd4\xbf\xa1\xb0\xd4 \xc3\xca\xb4\xeb \xb8\xde\xbd\xc3\xc1\xf6\xb8\xa6 \xba\xb8\xb3\xc2\xbd\xc0\xb4\xcf\xb4\xd9", it->second.nick), false);
		m_pInviteDelayDlg->Open(0, 0);

		FrButton* pCancel = DYNAMIC_CAST(FrButton, m_pInviteDelayDlg->FindChildByName("cancel"));
		if (pCancel)
			pCancel->SetVisible(false);
	}

	if (IsLocalContent((localContentType_t)0x54)) {
		CSharedDoc::Instance()->SetFriendGameSvrUID(CSharedDoc::Instance()->m_gameServerUID);
		CSharedDoc::Instance()->SetInviteMode(1);
		WSendPacket packet((enumClientPacket)0xB2);

		packet.EncodeStr(std::string(it->second.nick));

		packet.Encode4(it->second.id);

		packet.Send(SEND_TO_DEFAULT);
	} else {
		WSendPacket packet((enumClientPacket)0x29);
		packet.Encode4(it->second.id);

		packet.Send(SEND_TO_DEFAULT);
	}
}

// userlistdlg.cpp:591
void FrUserListDlg::OnFriendBtnUp(void) {
	m_pAddFriendDlg = CreateForm<FrAddFriendDlg>(g_pFresh->GetManager(), this, "addfriend", 0);

	if (m_selectedUID != -1 && m_selectedUID != CSharedDoc::Instance()->GetPangyaUserInfo().guid) {
		std::map<unsigned long, sBriefUserInfo>::iterator it = CSharedDoc::Instance()->m_userMap.find(m_selectedUID);
		if (it != CSharedDoc::Instance()->m_userMap.end()) {
			m_pAddFriendDlg->SetNick(it->second.nick);
		}
	}

	m_pAddFriendDlg->Open(0, 1);
}

// userlistdlg.cpp:612
void FrUserListDlg::OnWhisperBtnInit(intptr_t param) {
	m_pWhisperBtn = (FrButton*)param;
	m_pWhisperBtn->Enable(false);
}

// userlistdlg.cpp:618
void FrUserListDlg::OnWhisperBtnUp(void) {
	if (m_selectedUID == -1) {
		if (m_pWhisperBtn) {
			m_pWhisperBtn->Enable(false);
		}
	} else {
		std::map<unsigned long, sBriefUserInfo>::iterator it = CSharedDoc::Instance()->m_userMap.find(m_selectedUID);
		if (it != CSharedDoc::Instance()->m_userMap.end()) {
			AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 0x37, (intptr_t)it->second.nick, 0, 0, 0, 0));
		}
	}
}

// userlistdlg.cpp:634
void FrUserListDlg::AddUser(void* data) {
	if (m_pUserList) {
		m_pUserList->AddItem(data);
	}
}

// userlistdlg.cpp:640
void FrUserListDlg::DelUser(void* data, int cmpLen) {
	if (m_pUserList) {
		m_pUserList->DelItem(data, cmpLen);
	}
}

// userlistdlg.cpp:647
void FrUserListDlg::SetAddFriendDlgNick(const char* nick, const char* unused) {
	if (m_pAddFriendDlg) {
		m_pAddFriendDlg->SetNick(nick);
	}
}

// userlistdlg.cpp:653
bool FrUserListDlg::OnAddFriendDlgResult(int result, FrForm* pForm) {
	m_pAddFriendDlg = 0;
	return true;
}

// userlistdlg.cpp:660
void FrUserListDlg::CloseInviteDelayDlg(void) {
	if (IsLocalContent((localContentType_t)0x54)) {
		if (m_pInviteDelayDlg) {
			m_pInviteDelayDlg->Close(true);
			m_pInviteDelayDlg = 0;
		}
	}
}

// userlistdlg.cpp:672
void FrUserListDlg::SetRequestFriendData(const char* nick, unsigned long uid) {
	if (nick) {
		if (m_pAddFriendDlg) {
			m_pAddFriendDlg->SetNick(nick);
			m_pAddFriendDlg->SetFindUID(uid);
		}
	}
}
