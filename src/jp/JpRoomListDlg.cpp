// JP-only game code: FrRoomListDlg, the lobby's room list form. See
// JpRoomListDlg.h.
#include "JpRoomListDlg.h"

#include <string>

#include "DetailedRoomInfoDlg.h"
#include "ObjectFactory.h"
#include "Packet.h"
#include "RFLobbyDesktop.h"
#include "RFTextManager.h"
#include "SharedDoc.h"
#include "SoundManager.h"
#include "TaskManager.h"
#include "ChatMsg.h"
#include "user_info.h"
#include "IContentsDoc.h"
#include "ItemManager.h"
#include "NetResourceManager.h"
#include "ProjectG.h"
#include "bitmap.h"
#include "commonutil.h"
#include "shared/classdefine.h"
#include "InputManager.h"
#include "JpContentsContainers.h"
#include "JpLobbyGlue.h"
#include "JpRoomListState.h"
#include "frbutton.h"
#include "fresh.h"
#include "frlistbox.h"
#include "frgraphicinterface.h"
#include "frviewer.h"
#include "frwndmanager.h"
#include "frwnd.h"
#include "woverlay.h"
#include "wview.h"
#include "globalgamedefine.h"
#include "windows.h"

// lib: 0x51d470, 0x51d460 (this-adjusting deleting-destructor thunks),
// 0xccd1c0, 0xccd1e0, 0xccd200 (static initializers)

// JP Fresh, declared only: 0xbe7870 (JP FrWnd, unpaired; name ours) sets
// the FrWnd static flag 0xf1aac8 (0xbe7850 is its other writer).
void JpFrWnd_RaiseFlagF1aac8(void);
// JpLobbyGlue.cpp: 0x59fa80 (the JP CLobbyMain::LatestVersion) and the KR
// m_currentChannel.id (JP CSharedDoc +0x10a98).
bool CLobbyMain_LatestVersion(void);
unsigned char JpSharedDoc_GetCurrentChannelId(void);

// JP Fresh (defined below as port glue): the JP FrListBox fields the list
// INIT handlers set inline: dword +0xfc = 1 (UseRightButton) and byte
// +0x114 = 1 (UseDummy).
void JpFrListBox_SetRoomUserListFlags(FrListBox* list);
// Only the dword +0xfc = 1 (OnUserListInit; name ours).
void JpFrListBox_SetFlagFc(FrListBox* list);
// 0xace330 (JP, unplaced; name ours): on the JP room detail form (+0x148),
// its join button (+0x120) Enable through vfn +0x18; the KR
// CLobbyMain::EnableRoomEnterControls calls FrDtRoomDlg::EnableJoinBtn(true)
// in its place.
void JpDtRoomDlg_EnableJoinBtn(FrWnd* dlg, bool enable);

// The JP FrListBox stores the KR UseRightButton flag as a dword at +0xfc
// and UseDummy's at +0x114 (the KR CLobbyMain list INIT handlers call
// UseDummy(true) / UseRightButton(true) in their place): port glue.
void JpFrListBox_SetRoomUserListFlags(FrListBox* list) {
	list->UseRightButton(true);
	list->UseDummy(true);
}

void JpFrListBox_SetFlagFc(FrListBox* list) {
	list->UseRightButton(true);
}

// 0xace330 (JP FrDtRoomDlg::EnableJoinBtn: its +0x120 join button's
// Enable, JP FrWnd vfn +0x18)
void JpDtRoomDlg_EnableJoinBtn(FrWnd* dlg, bool enable) {
	static_cast<FrDtRoomDlg*>(dlg)->EnableJoinBtn(enable);
}

// 0xe74e78 (static init 0xcccf70): cate_vs_list's rows (Korean keys: stroke, match)
static std::string s_vsListItems[2] = {
	Jp_LookupText("\xbd\xba\xc6\xae\xb7\xce\xc5\xa9"),
	Jp_LookupText("\xb8\xc5\xc4\xa1"),
};

// 0xe74eb0 (static init 0xcccf70): cate_mass_list's rows (30-player tournament, 30-player team match, guild battle)
static std::string s_massListItems[3] = {
	Jp_LookupText("\x33\x30\xc0\xce\xb4\xeb\xc8\xb8"),
	Jp_LookupText("\x33\x30\xc0\xce\xc6\xc0\xc0\xfc"),
	Jp_LookupText("\xb1\xe6\xb5\xe5\xb4\xeb\xc0\xfc"),
};

// 0xe74f04 (static init 0xcccf70): cate_battle_list's rows (pang battle, approach)
static std::string s_battleListItems[2] = {
	Jp_LookupText("\xc6\xce\xb9\xe8\xc6\xb2"),
	Jp_LookupText("\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1"),
};

// 0xccd1c0 (run-time type)
const WRTTI FrRoomListDlg::m_RTTI("FrRoomListDlg", &FrChatDlg::m_RTTI);

// 0x51ecc0
IObject* FrRoomListDlgMakeInstance(void) {
	return new FrRoomListDlg;
}

// 0x51ed30 (static init 0xccd1e0)
struct __sFrRoomListDlg {
	__sFrRoomListDlg(void) {
		ObjectFactory().AddObjectFunctor(FrRoomListDlgMakeInstance, "FrRoomListDlg");
	}
};
static __sFrRoomListDlg s_registerFrRoomListDlg;

// 0x51c9e0
const sFRESH_MSGMAP* FrRoomListDlg::GetMessageMap(void) const {
	return &FrRoomListDlg::_MsgMap;
}

sFRESH_MSGMAP FrRoomListDlg::_MsgMap = { &FrChatDlg::_MsgMap, &FrRoomListDlg::_MsgEntries[0] };

#define ROOMLIST_VI(name, cmd, fn) { name, cmd, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRoomListDlg::fn }
#define ROOMLIST_VV(name, cmd, fn) { name, cmd, FrFuncVV, (FRESH_PMSG)&FrRoomListDlg::fn }

sFRESH_ENTRY FrRoomListDlg::_MsgEntries[] = {
	ROOMLIST_VI("room_no", FRCMD_INIT, OnRoomNoInit),
	ROOMLIST_VV("room_no", FRCMD_LBUTTONDOWN, OnRoomNoLBtnDown),
	ROOMLIST_VI("room_no", FRCMD_OWNERDRAW, OnRoomNoOwnerDraw),
	ROOMLIST_VI("btn_cate_total", FRCMD_INIT, OnCateTotalInit),
	ROOMLIST_VV("btn_cate_total", FRCMD_LBUTTONUP, OnCateTotalLBtnUp),
	ROOMLIST_VI("btn_cate_vs", FRCMD_INIT, OnCateVsInit),
	ROOMLIST_VV("btn_cate_vs", FRCMD_LBUTTONUP, OnCateVsLBtnUp),
	ROOMLIST_VI("btn_cate_mass", FRCMD_INIT, OnCateMassInit),
	ROOMLIST_VV("btn_cate_mass", FRCMD_LBUTTONUP, OnCateMassLBtnUp),
	ROOMLIST_VI("btn_cate_battle", FRCMD_INIT, OnCateBattleInit),
	ROOMLIST_VV("btn_cate_battle", FRCMD_LBUTTONUP, OnCateBattleLBtnUp),
	ROOMLIST_VI("btn_cate_chat", FRCMD_INIT, OnCateChatInit),
	ROOMLIST_VV("btn_cate_chat", FRCMD_LBUTTONUP, OnCateChatLBtnUp),
	ROOMLIST_VI("room_hole", FRCMD_INIT, OnRoomHoleInit),
	ROOMLIST_VV("room_hole", FRCMD_LBUTTONDOWN, OnRoomHoleLBtnDown),
	ROOMLIST_VI("room_hole", FRCMD_OWNERDRAW, OnRoomHoleOwnerDraw),
	ROOMLIST_VI("room_course", FRCMD_INIT, OnRoomCourseInit),
	ROOMLIST_VV("room_course", FRCMD_LBUTTONDOWN, OnRoomCourseLBtnDown),
	ROOMLIST_VI("room_course", FRCMD_OWNERDRAW, OnRoomCourseOwnerDraw),
	ROOMLIST_VI("room_state", FRCMD_INIT, OnRoomStateInit),
	ROOMLIST_VV("room_state", FRCMD_LBUTTONDOWN, OnRoomStateLBtnDown),
	ROOMLIST_VI("room_state", FRCMD_OWNERDRAW, OnRoomStateOwnerDraw),
	ROOMLIST_VI("cate_vs_list", FRCMD_INIT, OnVsListInit),
	ROOMLIST_VV("cate_vs_list", FRCMD_LBUTTONDOWN, OnVsListLBtnDown),
	ROOMLIST_VI("cate_vs_list", FRCMD_OWNERDRAW, OnVsListOwnerDraw),
	ROOMLIST_VI("cate_mass_list", FRCMD_INIT, OnMassListInit),
	ROOMLIST_VV("cate_mass_list", FRCMD_LBUTTONDOWN, OnMassListLBtnDown),
	ROOMLIST_VI("cate_mass_list", FRCMD_OWNERDRAW, OnMassListOwnerDraw),
	ROOMLIST_VI("cate_battle_list", FRCMD_INIT, OnBattleListInit),
	ROOMLIST_VV("cate_battle_list", FRCMD_LBUTTONDOWN, OnBattleListLBtnDown),
	ROOMLIST_VI("cate_battle_list", FRCMD_OWNERDRAW, OnBattleListOwnerDraw),
	ROOMLIST_VI("makeroom", FRCMD_INIT, OnMakeRoomInit),
	ROOMLIST_VV("makeroom", FRCMD_LBUTTONUP, OnMakeRoomLBtnUp),
	ROOMLIST_VV("close_wnd", FRCMD_LBUTTONUP, OnCloseLBtnUp),
	ROOMLIST_VI("cur_equip_item", FRCMD_INIT, OnCurEquipItemInit),
	ROOMLIST_VV("cur_equip_item", FRCMD_LBUTTONUP, OnCurEquipItemLBtnUp),
	ROOMLIST_VI("quest_btn", FRCMD_INIT, OnQuestInit),
	ROOMLIST_VV("quest_btn", FRCMD_LBUTTONUP, OnQuestLBtnUp),
	ROOMLIST_VI("userlist", FRCMD_INIT, OnUserListInit),
	ROOMLIST_VV("userlist", FRCMD_LBUTTONDOWN, OnUserListLBtnDown),
	ROOMLIST_VV("userlist", FRCMD_RBUTTONUP, OnUserListRBtnUp),
	ROOMLIST_VI("userlist", FRCMD_OWNERDRAW, OnUserListOwnerDraw),
	ROOMLIST_VI("user_gender", FRCMD_INIT, OnUserGenderInit),
	ROOMLIST_VV("user_gender", FRCMD_LBUTTONDOWN, OnUserGenderLBtnDown),
	ROOMLIST_VI("user_gender", FRCMD_OWNERDRAW, OnUserGenderOwnerDraw),
	ROOMLIST_VI("user_level", FRCMD_INIT, OnUserLevelInit),
	ROOMLIST_VV("user_level", FRCMD_LBUTTONDOWN, OnUserLevelLBtnDown),
	ROOMLIST_VI("user_level", FRCMD_OWNERDRAW, OnUserLevelOwnerDraw),
	ROOMLIST_VI("user_guild", FRCMD_INIT, OnUserGuildInit),
	ROOMLIST_VV("user_guild", FRCMD_LBUTTONDOWN, OnUserGuildLBtnDown),
	ROOMLIST_VI("user_guild", FRCMD_OWNERDRAW, OnUserGuildOwnerDraw),
	ROOMLIST_VI("user_nick", FRCMD_INIT, OnUserNickInit),
	ROOMLIST_VV("user_nick", FRCMD_LBUTTONDOWN, OnUserNickLBtnDown),
	ROOMLIST_VI("user_nick", FRCMD_OWNERDRAW, OnUserNickOwnerDraw),
	ROOMLIST_VI("def_bg.tab_gproom", FRCMD_INIT, OnGpRoomTabInit),
	ROOMLIST_VV("def_bg.tab_gproom", FRCMD_LBUTTONUP, OnGpRoomTabLBtnUp),
	ROOMLIST_VI("def_bg.tab_mtroom", FRCMD_INIT, OnMtRoomTabInit),
	ROOMLIST_VV("def_bg.tab_mtroom", FRCMD_LBUTTONUP, OnMtRoomTabLBtnUp),
	ROOMLIST_VI("def_bg.tab_avatarchat", FRCMD_INIT, OnAvatarChatTabInit),
	ROOMLIST_VV("def_bg.tab_avatarchat", FRCMD_LBUTTONUP, OnAvatarChatTabLBtnUp),
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

#undef ROOMLIST_VI
#undef ROOMLIST_VV

// 0x51cc90
void FrRoomListDlg::UpdateCategoryButtons(void) {
	int category = JpRoomListData_GetCategory(m_pRoomListData);
	m_pCateVsButton->SetStatus(category == 0 ? FrButton::PRESSED : FrButton::NORMAL);
	m_pCateMassButton->SetStatus(category == 1 ? FrButton::PRESSED : FrButton::NORMAL);
	m_pCateBattleButton->SetStatus(category == 2 ? FrButton::PRESSED : FrButton::NORMAL);
	m_pCateChatButton->SetStatus(category == 3 ? FrButton::PRESSED : FrButton::NORMAL);
	m_pCateTotalButton->SetStatus(category == 4 ? FrButton::PRESSED : FrButton::NORMAL);
}

// 0x51ce40
void FrRoomListDlg::SetRoomState(JpRoomListState* state) {
	m_pRoomState = state;
}

// 0x51c9f0
FrRoomListDlg::~FrRoomListDlg(void) {
}

// 0x51ca20
void FrRoomListDlg::OnProc(const float dt) {
	FrChatDlg::OnProc(dt);
}

// 0x51ca40
void FrRoomListDlg::LoadUserListImages(void) {
	m_userGenderIcons[0][0] = g_pFresh->RegisterBitmap("i_male");
	m_userGenderIcons[0][1] = g_pFresh->RegisterBitmap("i_female");
	m_userGenderIcons[1][0] = g_pFresh->RegisterBitmap("i_male_02");
	m_userGenderIcons[1][1] = g_pFresh->RegisterBitmap("i_female_02");
	m_userGenderIcons[2][0] = g_pFresh->RegisterBitmap("i_male_03");
	m_userGenderIcons[2][1] = g_pFresh->RegisterBitmap("i_female_03");
	m_userGenderIcons[3][0] = g_pFresh->RegisterBitmap("i_male_manner");
	m_userGenderIcons[3][1] = g_pFresh->RegisterBitmap("i_female_manner");
	m_userGenderIcons[4][0] = g_pFresh->RegisterBitmap("i_male_angel");
	m_userGenderIcons[4][1] = g_pFresh->RegisterBitmap("i_female_angel");
	m_userGenderImages[0] = g_pFresh->RegisterBitmap("user_sort_gender_normal");
	m_userGenderImages[1] = g_pFresh->RegisterBitmap("user_sort_gender_over");
	m_userGenderImages[2] = g_pFresh->RegisterBitmap("user_sort_gender_click");
	m_userLevelImages[0] = g_pFresh->RegisterBitmap("user_sort_level_normal");
	m_userLevelImages[1] = g_pFresh->RegisterBitmap("user_sort_level_over");
	m_userLevelImages[2] = g_pFresh->RegisterBitmap("user_sort_level_click");
	m_userGuildImages[0] = g_pFresh->RegisterBitmap("user_sort_guild_normal");
	m_userGuildImages[1] = g_pFresh->RegisterBitmap("user_sort_guild_over");
	m_userGuildImages[2] = g_pFresh->RegisterBitmap("user_sort_guild_click");
	m_userNickImages[0] = g_pFresh->RegisterBitmap("user_sort_nick_normal");
	m_userNickImages[1] = g_pFresh->RegisterBitmap("user_sort_nick_over");
	m_userNickImages[2] = g_pFresh->RegisterBitmap("user_sort_nick_click");
	m_userSortArrowImages[0] = g_pFresh->RegisterBitmap("user_sort_up");
	m_userSortArrowImages[1] = g_pFresh->RegisterBitmap("user_sort_down");
	m_pInGameImage = g_pFresh->RegisterBitmap("ingame");
	m_pAiPlayerImage = g_pFresh->RegisterBitmap("i_aiplayer");
}

// 0x51cd20
void FrRoomListDlg::SortUserList(int column) {
	if (m_pUserList) {
		m_value2f0 = column;
		switch (column) {
		case 0:
			if (m_flag2ea) {
				m_pUserList->SortItem(JpUserList_CompareGenderDesc);
			} else {
				m_pUserList->SortItem(JpUserList_CompareGenderAsc);
			}
			break;
		case 1:
			if (m_flag2ea) {
				m_pUserList->SortItem(JpUserList_CompareLevelDesc);
			} else {
				m_pUserList->SortItem(JpUserList_CompareLevelAsc);
			}
			break;
		case 2:
			if (m_flag2ea) {
				m_pUserList->SortItem(JpUserList_CompareGuildDesc);
			} else {
				m_pUserList->SortItem(JpUserList_CompareGuildAsc);
			}
			break;
		case 3:
			if (m_flag2ea) {
				m_pUserList->SortItem(JpUserList_CompareNickDesc);
			} else {
				m_pUserList->SortItem(JpUserList_CompareNickAsc);
			}
			break;
		}
	}
}

// 0x51cdf0
void FrRoomListDlg::HideCategoryLists(int column) {
	m_roomSortFlag[0] = false;
	m_roomSortFlag[1] = false;
	m_roomSortFlag[2] = false;
	m_roomSortFlag[3] = false;
	if (m_pVsList) {
		m_pVsList->SetVisible(false);
	}
	if (m_pMassList) {
		m_pMassList->SetVisible(false);
	}
	if (m_pBattleList) {
		m_pBattleList->SetVisible(false);
	}
}

// 0x51ce50
void FrRoomListDlg::SetLobbyValues(WTitleFontEx* titleFont, WTitleFontEx* font158, WTitleFontEx* font15c) {
	m_pTitleFont = titleFont;
	m_pFont158 = font158;
	m_pFont15c = font15c;
}

// 0x51ce80
void FrRoomListDlg::UpdateAvatarChatTab(void) {
	if (m_pAvatarChatTab) {
		bool avatarChat = JpLobby_GetAvatarChatMode() == 0x10;
		m_pAvatarChatTab->SetVisible(avatarChat);
		if (m_pMtRoomTab) {
			m_pMtRoomTab->SetVisible(!avatarChat);
		}
	}
}

// 0x51cee0
void FrRoomListDlg::DisableCategoryButton(int bit) {
	switch (bit) {
	case 2:
		m_pCateVsButton->Enable(false);
		break;
	case 4:
		m_pCateMassButton->Enable(false);
		break;
	case 8:
		m_pCateBattleButton->Enable(false);
		break;
	case 16:
		m_pCateChatButton->Enable(false);
		break;
	}
}

// 0x51d0d0
void FrRoomListDlg::OnCloseLBtnUp(void) {
	Close(FrOK, true);
}

// 0x51d0e0
void FrRoomListDlg::OnUserGenderLBtnDown(void) {
	if (m_value2d4 > 0.0f) {
		return;
	}
	if (m_value2f0 == 0) {
		m_flag2ea = !m_flag2ea;
	} else {
		m_flag2ea = false;
	}
	m_value2d4 = 0.3f;
	m_userSortFlag[0] = true;
	m_userSortFlag[1] = false;
	m_userSortFlag[2] = false;
	m_userSortFlag[3] = false;
	SortUserList(0);
	g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

// 0x51d160
void FrRoomListDlg::OnUserLevelLBtnDown(void) {
	if (m_value2d4 > 0.0f) {
		return;
	}
	if (m_value2f0 == 1) {
		m_flag2ea = !m_flag2ea;
	} else {
		m_flag2ea = false;
	}
	m_value2d4 = 0.3f;
	m_userSortFlag[0] = false;
	m_userSortFlag[1] = true;
	m_userSortFlag[2] = false;
	m_userSortFlag[3] = false;
	SortUserList(1);
	g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

// 0x51d1e0
void FrRoomListDlg::OnUserGuildLBtnDown(void) {
	if (m_value2d4 > 0.0f) {
		return;
	}
	if (m_value2f0 == 2) {
		m_flag2ea = !m_flag2ea;
	} else {
		m_flag2ea = false;
	}
	m_value2d4 = 0.3f;
	m_userSortFlag[0] = false;
	m_userSortFlag[1] = false;
	m_userSortFlag[2] = true;
	m_userSortFlag[3] = false;
	SortUserList(2);
	g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

// 0x51d260
void FrRoomListDlg::OnUserNickLBtnDown(void) {
	if (m_value2d4 > 0.0f) {
		return;
	}
	if (m_value2f0 == 3) {
		m_flag2ea = !m_flag2ea;
	} else {
		m_flag2ea = false;
	}
	m_value2d4 = 0.3f;
	m_userSortFlag[0] = false;
	m_userSortFlag[1] = false;
	m_userSortFlag[2] = false;
	m_userSortFlag[3] = true;
	SortUserList(3);
	g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

// 0x51d2e0
void FrRoomListDlg::OnAvatarChatTabLBtnUp(void) {
}

// 0x51d510
FrRoomListDlg::FrRoomListDlg(void) {
	m_value2cc = 0.0f;
	m_value2d4 = 0.0f;
	m_value2d8 = 0.0f;
	m_value2dc = 0.0f;
	m_value2e0 = 0.0f;
	m_pRoomState = NULL;
	m_pDtRoomDlg = NULL;
	m_value14c = 0;
	m_pMakeRoomButton = NULL;
	m_pGuildInfoButton = NULL;
	m_pUserList = NULL;
	m_pRoomList = NULL;
	m_pSelectedRoom = NULL;
	m_value2f0 = 3;
	m_flag2e9 = false;
	m_flag2ea = false;
	m_pAltRoomList = NULL;
	m_pTitleFont = NULL;
	m_pFont158 = NULL;
	m_pFont15c = NULL;
	m_pCateTotalButton = NULL;
	m_pCateVsButton = NULL;
	m_pCateMassButton = NULL;
	m_pCateBattleButton = NULL;
	m_pCateChatButton = NULL;
	m_pVsList = NULL;
	m_pMassList = NULL;
	m_pBattleList = NULL;
	m_pRoomNoColumn = NULL;
	m_pRoomHoleColumn = NULL;
	m_pRoomCourseColumn = NULL;
	m_pRoomStateColumn = NULL;
	m_pCurEquipItemButton = NULL;
	m_pQuestButton = NULL;
	m_pUserSelectWnd = NULL;
	m_pInGameImage = NULL;
	m_value2f4 = 0;
	m_value2ec = 0;
	m_pGpRoomTab = NULL;
	m_pMtRoomTab = NULL;
	m_pAvatarChatTab = NULL;
	WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0xa5, m_pRoomListData);
}

// 0x51d6b0
void FrRoomListDlg::ResetRoomListImages(void) {
	m_roomRowImages[0][0] = g_pFresh->RegisterBitmap("roomlistbox_n_01");
	m_roomRowImages[1][0] = g_pFresh->RegisterBitmap("roomlistbox_n_02");
	m_roomRowImages[2][0] = g_pFresh->RegisterBitmap("roomlistbox_n_03");
	m_roomRowImages[0][1] = g_pFresh->RegisterBitmap("roomlistbox_o_01");
	m_roomRowImages[1][1] = g_pFresh->RegisterBitmap("roomlistbox_o_02");
	m_roomRowImages[2][1] = g_pFresh->RegisterBitmap("roomlistbox_o_03");
	m_roomRowImages[0][2] = g_pFresh->RegisterBitmap("roomlistbox_n_01");
	m_roomRowImages[1][2] = g_pFresh->RegisterBitmap("roomlistbox_n_02");
	m_roomRowImages[2][2] = g_pFresh->RegisterBitmap("roomlistbox_n_03");
	m_roomRowImages[0][3] = g_pFresh->RegisterBitmap("roomlistbox_e_01");
	m_roomRowImages[1][3] = g_pFresh->RegisterBitmap("roomlistbox_e_02");
	m_roomRowImages[2][3] = g_pFresh->RegisterBitmap("roomlistbox_e_03");
	m_roomNoImages[0] = g_pFresh->RegisterBitmap("room_sort_no_n");
	m_roomNoImages[1] = g_pFresh->RegisterBitmap("room_sort_no_o");
	m_roomNoImages[2] = g_pFresh->RegisterBitmap("room_sort_no_d");
	m_roomCateImages[0] = g_pFresh->RegisterBitmap("room_sort_cate_n_down");
	m_roomCateImages[1] = g_pFresh->RegisterBitmap("room_sort_cate_o");
	m_roomCateImages[2] = g_pFresh->RegisterBitmap("room_sort_cate_d");
	m_roomHoleImages[0] = g_pFresh->RegisterBitmap("room_sort_hole_n");
	m_roomHoleImages[1] = g_pFresh->RegisterBitmap("room_sort_hole_o");
	m_roomHoleImages[2] = g_pFresh->RegisterBitmap("room_sort_hole_d");
	m_roomCourseImages[0] = g_pFresh->RegisterBitmap("room_sort_course_n");
	m_roomCourseImages[1] = g_pFresh->RegisterBitmap("room_sort_course_o");
	m_roomCourseImages[2] = g_pFresh->RegisterBitmap("room_sort_course_d");
	m_roomStateImages[0][0] = g_pFresh->RegisterBitmap("room_sort_state_up_n");
	m_roomStateImages[0][1] = g_pFresh->RegisterBitmap("room_sort_state_up_o");
	m_roomStateImages[0][2] = g_pFresh->RegisterBitmap("room_sort_state_up_d");
	m_roomStateImages[1][0] = g_pFresh->RegisterBitmap("room_sort_state_dn_n");
	m_roomStateImages[1][1] = g_pFresh->RegisterBitmap("room_sort_state_dn_o");
	m_roomStateImages[1][2] = g_pFresh->RegisterBitmap("room_sort_state_dn_d");
	LoadUserListImages();
}

// 0x51d950
void FrRoomListDlg::OnRoomNoInit(intptr_t param) {
	m_pRoomNoColumn = DYNAMIC_CAST(FrViewer, param);
	m_roomSortFlag[0] = false;
}

// 0x51d9d0
void FrRoomListDlg::OnRoomNoOwnerDraw(intptr_t param) {
	FrWndManager* manager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = manager->GetGDI();
	if (pGDI) {
		if (m_pRoomNoColumn) {
			int image = 0;
			if (!m_roomSortFlag[0]) {
				WRect rect = m_pRoomNoColumn->GetRect();
				if (rect.IsInRect(manager->GetMousePos())) {
					image = 1;
				}
			} else {
				image = 2;
			}
			pGDI->DrawTexture(m_roomNoImages[image], m_pRoomNoColumn->GetRect(), 0xffffffff, 0);
		}
	}
}

// 0x51da60
void FrRoomListDlg::OnCateTotalInit(intptr_t param) {
	m_pCateTotalButton = DYNAMIC_CAST(FrButton, param);
	m_pCateTotalButton->SetStatus(JpRoomListData_GetCategory(m_pRoomListData) == 4 ? FrButton::PRESSED : FrButton::NORMAL);
}

// 0x51dae0
void FrRoomListDlg::OnCateVsInit(intptr_t param) {
	m_pCateVsButton = DYNAMIC_CAST(FrButton, param);
	m_pCateVsButton->SetStatus(JpRoomListData_GetCategory(m_pRoomListData) == 0 ? FrButton::PRESSED : FrButton::NORMAL);
}

// 0x51db60
void FrRoomListDlg::OnCateMassInit(intptr_t param) {
	m_pCateMassButton = DYNAMIC_CAST(FrButton, param);
	m_pCateMassButton->SetStatus(JpRoomListData_GetCategory(m_pRoomListData) == 1 ? FrButton::PRESSED : FrButton::NORMAL);
}

// 0x51dbe0
void FrRoomListDlg::OnCateBattleInit(intptr_t param) {
	m_pCateBattleButton = DYNAMIC_CAST(FrButton, param);
	m_pCateBattleButton->SetStatus(JpRoomListData_GetCategory(m_pRoomListData) == 2 ? FrButton::PRESSED : FrButton::NORMAL);
}

// 0x51dc60
void FrRoomListDlg::OnCateChatInit(intptr_t param) {
	m_pCateChatButton = DYNAMIC_CAST(FrButton, param);
	m_pCateChatButton->SetStatus(JpRoomListData_GetCategory(m_pRoomListData) == 3 ? FrButton::PRESSED : FrButton::NORMAL);
}

// 0x51dce0
void FrRoomListDlg::OnRoomHoleInit(intptr_t param) {
	m_pRoomHoleColumn = DYNAMIC_CAST(FrViewer, param);
	m_roomSortFlag[1] = false;
}

// 0x51dd60
void FrRoomListDlg::OnRoomHoleOwnerDraw(intptr_t param) {
	FrWndManager* manager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = manager->GetGDI();
	if (pGDI) {
		if (m_pRoomHoleColumn) {
			int image = 0;
			if (!m_roomSortFlag[1]) {
				WRect rect = m_pRoomHoleColumn->GetRect();
				if (rect.IsInRect(manager->GetMousePos())) {
					image = 1;
				}
			} else {
				image = 2;
			}
			pGDI->DrawTexture(m_roomHoleImages[image], m_pRoomHoleColumn->GetRect(), 0xffffffff, 0);
		}
	}
}

// 0x51ddf0
void FrRoomListDlg::OnRoomCourseInit(intptr_t param) {
	m_pRoomCourseColumn = DYNAMIC_CAST(FrViewer, param);
	m_roomSortFlag[2] = false;
}

// 0x51de70
void FrRoomListDlg::OnRoomCourseOwnerDraw(intptr_t param) {
	FrWndManager* manager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = manager->GetGDI();
	if (pGDI) {
		if (m_pRoomCourseColumn) {
			int image = 0;
			if (!m_roomSortFlag[2]) {
				WRect rect = m_pRoomCourseColumn->GetRect();
				if (rect.IsInRect(manager->GetMousePos())) {
					image = 1;
				}
			} else {
				image = 2;
			}
			pGDI->DrawTexture(m_roomCourseImages[image], m_pRoomCourseColumn->GetRect(), 0xffffffff, 0);
		}
	}
}

// 0x51df00
void FrRoomListDlg::OnRoomStateInit(intptr_t param) {
	m_pRoomStateColumn = DYNAMIC_CAST(FrViewer, param);
	m_roomSortFlag[3] = false;
}

// 0x51df80
void FrRoomListDlg::OnRoomStateOwnerDraw(intptr_t param) {
	FrWndManager* manager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = manager->GetGDI();
	if (pGDI) {
		if (m_pRoomStateColumn) {
			int image = 0;
			if (!m_roomSortFlag[3]) {
				WRect rect = m_pRoomStateColumn->GetRect();
				if (rect.IsInRect(manager->GetMousePos())) {
					image = 1;
				}
			} else {
				image = 2;
			}
			pGDI->DrawTexture(m_roomStateImages[m_flag2e9 ? 1 : 0][image], m_pRoomStateColumn->GetRect(), 0xffffffff, 0);
		}
	}
}

// 0x51e020
void FrRoomListDlg::OnVsListInit(intptr_t param) {
	m_pVsList = DYNAMIC_CAST(FrListBox, param);
	if (m_pVsList) {
		for (std::string* item = s_vsListItems; item < s_vsListItems + 2; item++) {
			m_pVsList->AddItem(item);
		}
		m_pVsList->SetVisible(false);
		m_pVsList->EnableScrollBar(false);
	}
}

// 0x51e0d0
void FrRoomListDlg::OnMassListInit(intptr_t param) {
	m_pMassList = DYNAMIC_CAST(FrListBox, param);
	if (m_pMassList) {
		for (std::string* item = s_massListItems; item < s_massListItems + 3; item++) {
			m_pMassList->AddItem(item);
		}
		m_pMassList->SetVisible(false);
		m_pMassList->EnableScrollBar(false);
	}
}

// 0x51e180
void FrRoomListDlg::OnBattleListInit(intptr_t param) {
	m_pBattleList = DYNAMIC_CAST(FrListBox, param);
	if (m_pBattleList) {
		for (std::string* item = s_battleListItems; item < s_battleListItems + 2; item++) {
			m_pBattleList->AddItem(item);
		}
		m_pBattleList->SetVisible(false);
		m_pBattleList->EnableScrollBar(false);
	}
}

// 0x51e230
void FrRoomListDlg::OnMakeRoomInit(intptr_t param) {
	m_pMakeRoomButton = DYNAMIC_CAST(FrButton, param);
}

// 0x51e2a0
void FrRoomListDlg::OnCurEquipItemInit(intptr_t param) {
	m_pCurEquipItemButton = DYNAMIC_CAST(FrButton, param);
}

// 0x51e310
void FrRoomListDlg::OnQuestInit(intptr_t param) {
	m_pQuestButton = DYNAMIC_CAST(FrButton, param);
}

// 0x51e380
void FrRoomListDlg::OnUserListLBtnDown(void) {
	FrListItem* pItem = m_pUserList->GetItemUnderCursor();  // JP +0x104
	if (pItem) {
		if (pItem->pData) {
			JpRoomListData_SelectUser(m_pRoomListData, pItem->pData);
			if (m_pUserSelectWnd) {
				m_pUserSelectWnd->Enable(true);
			}
		}
		pItem->selected = true;
	}
}

// 0x51e3d0
void FrRoomListDlg::OnUserGenderInit(intptr_t param) {
	m_pUserGenderColumn = DYNAMIC_CAST(FrViewer, param);
	m_userSortFlag[0] = false;
}

// 0x51e450
void FrRoomListDlg::OnUserGenderOwnerDraw(intptr_t param) {
	FrWndManager* manager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = manager->GetGDI();
	if (pGDI) {
		if (m_pUserGenderColumn) {
			int image = 0;
			if (!m_userSortFlag[0]) {
				WRect rect = m_pUserGenderColumn->GetRect();
				if (rect.IsInRect(manager->GetMousePos())) {
					image = 1;
				}
			} else {
				image = 2;
			}
			WRect rect = m_pUserGenderColumn->GetRect();
			pGDI->DrawTexture(m_userGenderImages[image], rect, 0xffffffff, 0);
			if (m_value2f0 == 0) {
				WRect arrowRect(rect.x + rect.w, rect.y + 5.0, 12.0f, 12.0f);
				pGDI->DrawTexture(m_userSortArrowImages[!m_flag2ea], arrowRect, 0xffffffff, 0);
			}
		}
	}
}

// 0x51e570
void FrRoomListDlg::OnUserLevelInit(intptr_t param) {
	m_pUserLevelColumn = DYNAMIC_CAST(FrViewer, param);
	m_userSortFlag[1] = false;
}

// 0x51e5f0
void FrRoomListDlg::OnUserLevelOwnerDraw(intptr_t param) {
	FrWndManager* manager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = manager->GetGDI();
	if (pGDI) {
		if (m_pUserLevelColumn) {
			int image = 0;
			if (!m_userSortFlag[1]) {
				WRect rect = m_pUserLevelColumn->GetRect();
				if (rect.IsInRect(manager->GetMousePos())) {
					image = 1;
				}
			} else {
				image = 2;
			}
			WRect rect = m_pUserLevelColumn->GetRect();
			pGDI->DrawTexture(m_userLevelImages[image], rect, 0xffffffff, 0);
			if (m_value2f0 == 1) {
				WRect arrowRect(rect.x + rect.w, rect.y + 5.0, 12.0f, 12.0f);
				pGDI->DrawTexture(m_userSortArrowImages[!m_flag2ea], arrowRect, 0xffffffff, 0);
			}
		}
	}
}

// 0x51e710
void FrRoomListDlg::OnUserGuildInit(intptr_t param) {
	m_pUserGuildColumn = DYNAMIC_CAST(FrViewer, param);
	m_userSortFlag[2] = false;
}

// 0x51e790
void FrRoomListDlg::OnUserGuildOwnerDraw(intptr_t param) {
	FrWndManager* manager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = manager->GetGDI();
	if (pGDI) {
		if (m_pUserGuildColumn) {
			int image = 0;
			if (!m_userSortFlag[2]) {
				WRect rect = m_pUserGuildColumn->GetRect();
				if (rect.IsInRect(manager->GetMousePos())) {
					image = 1;
				}
			} else {
				image = 2;
			}
			WRect rect = m_pUserGuildColumn->GetRect();
			pGDI->DrawTexture(m_userGuildImages[image], rect, 0xffffffff, 0);
			if (m_value2f0 == 2) {
				WRect arrowRect(rect.x + rect.w, rect.y + 5.0, 12.0f, 12.0f);
				pGDI->DrawTexture(m_userSortArrowImages[!m_flag2ea], arrowRect, 0xffffffff, 0);
			}
		}
	}
}

// 0x51e8b0
void FrRoomListDlg::OnUserNickInit(intptr_t param) {
	m_pUserNickColumn = DYNAMIC_CAST(FrViewer, param);
	m_userSortFlag[3] = false;
}

// 0x51e930
void FrRoomListDlg::OnUserNickOwnerDraw(intptr_t param) {
	FrWndManager* manager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = manager->GetGDI();
	if (pGDI) {
		if (m_pUserNickColumn) {
			int image = 0;
			if (!m_userSortFlag[3]) {
				WRect rect = m_pUserNickColumn->GetRect();
				if (rect.IsInRect(manager->GetMousePos())) {
					image = 1;
				}
			} else {
				image = 2;
			}
			WRect rect = m_pUserNickColumn->GetRect();
			pGDI->DrawTexture(m_userNickImages[image], rect, 0xffffffff, 0);
			if (m_value2f0 == 3) {
				WRect arrowRect(rect.x + rect.w, rect.y + 5.0, 12.0f, 12.0f);
				pGDI->DrawTexture(m_userSortArrowImages[!m_flag2ea], arrowRect, 0xffffffff, 0);
			}
		}
	}
}

// 0x51ea50
void FrRoomListDlg::OnMtRoomTabInit(intptr_t param) {
	m_pMtRoomTab = DYNAMIC_CAST(FrButton, param);
}

// 0x51eac0
void FrRoomListDlg::OnAvatarChatTabInit(intptr_t param) {
	m_pAvatarChatTab = DYNAMIC_CAST(FrButton, param);
	UpdateAvatarChatTab();
}

// 0x51ed50
bool FrRoomListDlg::OnInit(void) {
	JpFrWnd_RaiseFlagF1aac8();
	if (RFLobbyDesktop::s_instance) {
		CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0x59, 9, 0, 0, 0);
	}
	// FrForm's (0xbe6570), not FrChatDlg's.
	return FrForm::OnInit();
}

// 0x51ed90
void FrRoomListDlg::EnableRoomEnterControls(bool enable) {
	if (m_pRoomList) {
		m_pRoomList->Enable(enable);
	}
	if (m_pMakeRoomButton) {
		m_pMakeRoomButton->Enable(enable);
	}
	if (m_pCurEquipItemButton) {
		m_pCurEquipItemButton->Enable(enable);
	}
	if (m_pQuestButton) {
		m_pQuestButton->Enable(enable);
	}
	if (m_pGuildInfoButton) {
		m_pGuildInfoButton->Enable(false);
	}
	if (enable) {
		if (m_pDtRoomDlg) {
			JpDtRoomDlg_EnableJoinBtn(m_pDtRoomDlg, true);
		}
	}
	// JP CSharedDoc +0x812: the KR m_userInfo.identity.
	if ((WSingleton<CSharedDoc>::Instance()->m_userInfo.identity & 0xe) == 0xe && m_pGuildInfoButton) {
		m_pGuildInfoButton->Enable(false);
	}
}

// 0x51ee90
void FrRoomListDlg::UpdateGpRoomTab(void) {
	if (m_pGpRoomTab) {
		m_pGpRoomTab->SetVisible(JpSharedDoc_IsGpRoomTabShown());
	}
}

// 0x51eec0
void FrRoomListDlg::OnVsListOwnerDraw(intptr_t param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	FrListItem* pItem = (FrListItem*)param;
	if (pGDI && pItem) {
		const std::string* text = (const std::string*)pItem->pData;
		WOverlay::DrawBox(g_view, WRect(pItem->pos.x, pItem->pos.y, 56.0f, 22.0f), 0, 0x880e4f75, 0.0f);
		WRect rect(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f, 54.0f, 20.0f);
		unsigned long style;
		if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
			WOverlay::DrawBox(g_view, rect, 0, 0x88082b46, 0.0f);
			style = 1;
		} else {
			style = 0;
		}
		pGDI->SetTextStyle(style);
		pGDI->SetTextColor(0xffffffff, 0xffffffff);
		pGDI->Print(WPoint((float)(pItem->pos.x + 2.0), (float)(pItem->pos.y + 6.0)), 0, text->c_str());
	}
}

// 0x51f020
void FrRoomListDlg::OnMassListOwnerDraw(intptr_t param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	FrListItem* pItem = (FrListItem*)param;
	if (pGDI && pItem) {
		const std::string* text = (const std::string*)pItem->pData;
		WOverlay::DrawBox(g_view, WRect(pItem->pos.x, pItem->pos.y, 56.0f, 22.0f), 0, 0x880e4f75, 0.0f);
		WRect rect(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f, 54.0f, 20.0f);
		unsigned long style;
		if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
			WOverlay::DrawBox(g_view, rect, 0, 0x88082b46, 0.0f);
			style = 1;
		} else {
			style = 0;
		}
		pGDI->SetTextStyle(style);
		pGDI->SetTextColor(0xffffffff, 0xffffffff);
		pGDI->Print(WPoint((float)(pItem->pos.x + 2.0), (float)(pItem->pos.y + 6.0)), 0, text->c_str());
	}
}

// 0x51f180
void FrRoomListDlg::OnBattleListOwnerDraw(intptr_t param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	FrListItem* pItem = (FrListItem*)param;
	if (pGDI && pItem) {
		const std::string* text = (const std::string*)pItem->pData;
		WOverlay::DrawBox(g_view, WRect(pItem->pos.x, pItem->pos.y, 56.0f, 22.0f), 0, 0x880e4f75, 0.0f);
		WRect rect(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f, 54.0f, 20.0f);
		unsigned long style;
		if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
			WOverlay::DrawBox(g_view, rect, 0, 0x88082b46, 0.0f);
			style = 1;
		} else {
			style = 0;
		}
		pGDI->SetTextStyle(style);
		pGDI->SetTextColor(0xffffffff, 0xffffffff);
		pGDI->Print(WPoint((float)(pItem->pos.x + 2.0), (float)(pItem->pos.y + 6.0)), 0, text->c_str());
	}
}

// 0x51f2e0: the JP form of KR CLobbyMain's make-room button (the
// "server" form when no channel is joined, else "makeroom"), through the
// lobby's message 0x8b (0 / 1).
void FrRoomListDlg::OnMakeRoomLBtnUp(void) {
	if (!CLobbyMain_LatestVersion())
		return;

	if (WSingleton<CSharedDoc>::Instance()->IsControlServerService(2)) {
		CTaskManager* manager = CTaskManager::Instance();
		// 0xd06358: "this feature is being checked" (Korean key).
		(!manager ? 0 : manager->GetCurrentTask())->SendMsgToMainActor(MsgObject(0, 0x23,
			(intptr_t)Jp_LookupText("\xB0\xD4\xC0\xD3 \xB0\xFC\xB7\xC3 \xBA\xCE\xBA\xD0 \xC1\xA1\xB0\xCB\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9."),
			0, 0, 0, 0));
		return;
	}

	if (JpSharedDoc_GetCurrentChannelId() == 0xFF) {
		WSendPacket packet(GCT_REQUEST_SERVER_LIST);
		packet.Send(SEND_TO_DEFAULT);
		CTaskManager* manager = CTaskManager::Instance();
		(!manager ? 0 : manager->GetCurrentTask())->SendMsgToMainActor(MsgObject(0, 0x8b, 0, 0, 0, 0, 0));
		return;
	}

	EnableRoomEnterControls(false);
	CTaskManager* manager = CTaskManager::Instance();
	(!manager ? 0 : manager->GetCurrentTask())->SendMsgToMainActor(MsgObject(0, 0x8b, 1, 0, 0, 0, 0));
}

// 0x51f490
void FrRoomListDlg::OnCurEquipItemLBtnUp(void) {
	AfxGetTask()->PassMsgToActor("Lobby", MsgObject(NULL, 0x8b, 5, 0, 0, 0, 0));
}

// 0x51f4f0
void FrRoomListDlg::OnQuestLBtnUp(void) {
	{
		WSendPacket packet(0x151);
		packet.Send(SEND_TO_DEFAULT);
	}
	AfxGetTask()->PassMsgToActor("Lobby", MsgObject(NULL, 1, 0, 0, 0, 0, 0));
}

// 0x51f5a0: a lobby user row (the item data is the user record, KR
// sBriefUserInfo). The JP FrListBox's item size (+0xe0/+0xe4) is the KR
// m_itemSize (GetItemWidth/GetItemHeight).
void FrRoomListDlg::OnUserListOwnerDraw(intptr_t param) {
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem) {
		return;
	}
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI) {
		return;
	}
	sBriefUserInfo* pUser = (sBriefUserInfo*)pItem->pData;
	if (!pUser) {
		return;
	}
	// The nick color: administrators and observers are listed half
	// transparent to an administrator, and else only when invisible; with
	// local content 0x10 a nick color type 1 draws the nick in 0xFF33DBFF.
	unsigned long color;
	if ((unsigned char)pUser->identity & 0x14) {
		if (WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().identity & 0x14) {
			color = 0x50FFFFFF;
		} else {
			if (!pUser->invisible) {
				return;
			}
			color = 0xFFFFFFFF;
		}
	} else if (!IsLocalContent((localContentType_t)0x10)) {
		color = 0xFFFFFFFF;
	} else {
		color = (pUser->nickColorType == 1) ? 0xFF33DBFF : 0xFFFFFFFF;
	}
	// The picked user's row (the 0xa5 container's selected UID).
	if (pItem->selected) {
		if (JpRoomListData_GetSelectedUid(m_pRoomListData) == pUser->uid) {
			pGDI->Box(WRect(pItem->pos.x, pItem->pos.y, (float)m_pUserList->GetItemWidth(), (float)m_pUserList->GetItemHeight()), 0xFF265785, 0, 0.0f);
		} else {
			pItem->selected = false;
		}
	}
	// The sex icon: angel, manner, or the plain icon of the character type.
	const Bitmap* pSex;
	if (pUser->angel) {
		pSex = m_userGenderIcons[4][pUser->charType % 2];
	} else if (pUser->manner) {
		pSex = m_userGenderIcons[3][pUser->charType % 2];
	} else {
		pSex = m_userGenderIcons[pUser->charType / 2][pUser->charType % 2];
	}
	WRect rect(pItem->pos.x + 8.0, pItem->pos.y, (float)pSex->Width(), (float)pSex->Height());
	pGDI->DrawTexture(pSex, rect, 0xFFFFFFFF, 0);
	// In a room: the "ingame" mark over the icon.
	if (pUser->roomNumber != 0xFFFF) {
		WRect inGameRect(rect.x, rect.y + 3.0, (float)m_pInGameImage->Width(), (float)m_pInGameImage->Height());
		pGDI->DrawTexture(m_pInGameImage, inGameRect, 0xFFFFFFFF, 0);
	}
	// The equipped title's icon, else the level's; a GP host (identity
	// 0x8000) shows LEVELS gp_host.
	const char* levelName;
	if (pUser->titleTypeId) {
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pUser->titleTypeId);
		if (pSkin) {
			levelName = pSkin->m_icon;
		} else {
			levelName = MakeStr("level_%03d", pUser->level + 1);
		}
	} else {
		levelName = MakeStr("level_%03d", pUser->level + 1);
	}
	const Bitmap* pLevel = g_pFresh->GetBitmap(levelName);
	if (pUser->identity & 0x8000) {
		pLevel = g_pFresh->GetManager()->GetBitmap("LEVELS", "gp_host");
	}
	if (pLevel) {
		pGDI->DrawTexture(pLevel, WRect(pItem->pos.x + 66.0 - float2int(pLevel->Width() * 0.5), pItem->pos.y + 11.0 - float2int(pLevel->Height() * 0.5), (float)pLevel->Width(), (float)pLevel->Height()), 0xFFFFFFFF, 0);
	}
	// The guild emblem. The JP NetResourceManager::GetEmblemByName takes the
	// guildUID and the JP record's +0x32 dword ("%d_%d.png"); the port's KR
	// record carries the emblem name there.
	if (pUser->guildUID && !pUser->IsIdentity(0x14)) {
		const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pUser->guildMark);
		if (pEmblem) {
			pGDI->DrawTexture(pEmblem, WRect(pItem->pos.x + 106.0, pItem->pos.y + 7.0 - 6.0, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
		}
	}
	if (WSingleton<CProjectG>::Instance()->HidePrivacy()) {
		return;
	}
	if (pItem->underCursor) {
		pGDI->SetTextColor(0xFFFFFFFF, 0xFF124371);
		pGDI->SetTextStyle(2);
	} else {
		pGDI->SetTextColor(color, 0xFFFFFFFF);
		pGDI->SetTextStyle(0);
	}
	g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 143.0, pItem->pos.y + 7.0), 0, pUser->nick, m_pUserList->GetItemWidth() - 143.0, color);
}

// 0x51fa10
void FrRoomListDlg::OnGpRoomTabInit(intptr_t param) {
	m_pGpRoomTab = DYNAMIC_CAST(FrButton, param);
	UpdateGpRoomTab();
}

// 0x51fa90
void FrRoomListDlg::OnGpRoomTabLBtnUp(void) {
	if (!JpSharedDoc_IsGpRoomTabShown()) {
		m_pGpRoomTab->Enable(false);
		return;
	}
	Close(FrOK, true);
	JpSharedDoc_SetRoomTab(1);
}

// 0x51fad0
void FrRoomListDlg::OnMtRoomTabLBtnUp(void) {
	Close(FrOK, true);
	JpSharedDoc_SetRoomTab(0);
}

// 0x51fd30
void FrRoomListDlg::FillRoomList(int mode, int value) {
	if (!m_pRoomList) {
		return;
	}
	m_pRoomList->ClearItem();
	// JP CSharedDoc +0x109e4: the KR m_roomList. The JP record's +0x58 is
	// the KR m_gameType, +0xc4 the KR m_roomMode (rooms of the JP modes
	// 0x14 and 0x15 are not listed); +0xd9 bit 0 is JP-only
	// (JpRoomInfo_HasSortFlag).
	const std::list<sRoomInfo>& rooms = JpSharedDoc_GetRoomList();
	if (mode == 1) {
		JpRoomListData_SetCategory(m_pRoomListData, value);
		for (std::list<sRoomInfo>::const_iterator i = rooms.begin(); i != rooms.end(); ++i) {
			const sRoomInfo& room = *i;
			if (!JpRoomInfo_HasSortFlag(&room) && (unsigned int)(room.m_roomMode - 0x14) > 1) {
				if (Jp_GetRoomCategory(room.m_gameType) == value || value == 4) {
					m_pRoomList->AddItem(const_cast<sRoomInfo*>(&room));
				} else if (Jp_GetRoomCategory(room.m_gameType) == 4) {
					m_pRoomList->AddItem(const_cast<sRoomInfo*>(&room));
				}
			}
		}
	} else if (mode == 3) {
		JpRoomListData_SetSelection(m_pRoomListData, value);
		for (std::list<sRoomInfo>::const_iterator i = rooms.begin(); i != rooms.end(); ++i) {
			const sRoomInfo& room = *i;
			if (!JpRoomInfo_HasSortFlag(&room) && (unsigned int)(room.m_roomMode - 0x14) > 1 && room.m_gameType == value) {
				m_pRoomList->AddItem(const_cast<sRoomInfo*>(&room));
			}
		}
	}
	// Six rows at least: empty rows (NULL data) fill up.
	int count = m_pRoomList->GetCurrentItemSize(true);
	if (count >= 6) {
		return;
	}
	int missing = 6 - count;
	do {
		m_pRoomList->AddItem(NULL);
	} while (--missing);
}

// 0x51ffa0
void FrRoomListDlg::OnCateTotalLBtnUp(void) {
	FillRoomList(1, 4);
	UpdateCategoryButtons();
	JpRoomListData_ResetSelection(m_pRoomListData);
}

// 0x51ffd0
void FrRoomListDlg::OnCateVsLBtnUp(void) {
	FillRoomList(1, 0);
	UpdateCategoryButtons();
	m_value2d8 = 0.0f;
	JpRoomListData_ResetSelection(m_pRoomListData);
}

// 0x520000
void FrRoomListDlg::OnCateMassLBtnUp(void) {
	FillRoomList(1, 1);
	UpdateCategoryButtons();
	m_value2dc = 0.0f;
	JpRoomListData_ResetSelection(m_pRoomListData);
}

// 0x520030
void FrRoomListDlg::OnCateBattleLBtnUp(void) {
	FillRoomList(1, 2);
	UpdateCategoryButtons();
	m_value2e0 = 0.0f;
	JpRoomListData_ResetSelection(m_pRoomListData);
}

// 0x520060
void FrRoomListDlg::OnCateChatLBtnUp(void) {
	FillRoomList(1, 3);
	UpdateCategoryButtons();
	JpRoomListData_ResetSelection(m_pRoomListData);
}

// 0x520090
void FrRoomListDlg::OnVsListLBtnDown(void) {
	int count = m_pVsList->GetCurrentItemSize(true);
	for (int i = 0; i < count; i++) {
		FrListItem* pItem = m_pVsList->GetItem(i);
		if (pItem) {
			const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
			float x = pItem->pos.x + 1.0f;
			float y = pItem->pos.y + 1.0f;
			if (x <= mouse.x && x + 54.0 >= mouse.x && y <= mouse.y && y + 21.0 >= mouse.y) {
				m_listRowPicked = true;
				FillRoomList(3, pItem->idx);
				break;
			}
		}
	}
	if (m_pVsList) {
		m_pVsList->SetVisible(false);
	}
}

// 0x520170
void FrRoomListDlg::OnMassListLBtnDown(void) {
	int count = m_pMassList->GetCurrentItemSize(true);
	for (int i = 0; i < count; i++) {
		FrListItem* pItem = m_pMassList->GetItem(i);
		if (pItem) {
			const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
			float x = pItem->pos.x + 1.0f;
			float y = pItem->pos.y + 1.0f;
			if (x <= mouse.x && x + 54.0 >= mouse.x && y <= mouse.y && y + 21.0 >= mouse.y) {
				m_listRowPicked = true;
				FillRoomList(3, pItem->idx);
				break;
			}
		}
	}
	if (m_pMassList) {
		m_pMassList->SetVisible(false);
	}
}

// 0x520250
void FrRoomListDlg::OnBattleListLBtnDown(void) {
	int count = m_pBattleList->GetCurrentItemSize(true);
	for (int i = 0; i < count; i++) {
		FrListItem* pItem = m_pBattleList->GetItem(i);
		if (pItem) {
			const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
			float x = pItem->pos.x + 1.0f;
			float y = pItem->pos.y + 1.0f;
			if (x <= mouse.x && x + 54.0 >= mouse.x && y <= mouse.y && y + 21.0 >= mouse.y) {
				m_listRowPicked = true;
				FillRoomList(3, pItem->idx);
				break;
			}
		}
	}
	if (m_pBattleList) {
		m_pBattleList->SetVisible(false);
	}
}

// 0x520340
void FrRoomListDlg::OnUserListInit(intptr_t param) {
	m_pUserList = DYNAMIC_CAST(FrListBox, param);
	if (m_pUserList) {
		JpFrListBox_SetFlagFc(m_pUserList);
		// JP CSharedDoc +0x109d8: the KR m_userMap (0xc8-byte JP user
		// records, the KR sBriefUserInfo; JP +0x21 bit 2 = administrator,
		// JP +0x42 bit 0 = the KR invisible bit at +0x3e). Someone who is
		// not an administrator sees an administrator only while its
		// invisible bit is set.
		const std::map<uint32_t, sBriefUserInfo>& users = JpSharedDoc_GetLobbyUserMap();
		for (std::map<uint32_t, sBriefUserInfo>::const_iterator i = users.begin(); i != users.end(); ++i) {
			const sBriefUserInfo& user = i->second;
			if (!Jp_IsAdminIdentity(-1)) {
				if (!user.identityBits.administrator) {
					m_pUserList->AddItem(const_cast<sBriefUserInfo*>(&user));
				} else if (user.invisible) {
					m_pUserList->AddItem(const_cast<sBriefUserInfo*>(&user));
				}
			} else {
				m_pUserList->AddItem(const_cast<sBriefUserInfo*>(&user));
			}
		}
	}
}

// 0x520520
void FrRoomListDlg::UpdateRoomList(float dt) {
	if (m_value2cc > 0.0f) {
		m_value2cc -= dt;
		if (m_value2cc <= 0.0f) {
			m_roomSortFlag[0] = false;
			m_roomSortFlag[1] = false;
			m_roomSortFlag[2] = false;
			m_roomSortFlag[3] = false;
		}
	}
	if (m_value2d4 > 0.0f) {
		m_value2d4 -= dt;
		if (m_value2d4 <= 0.0f) {
			m_userSortFlag[0] = false;
			m_userSortFlag[1] = false;
			m_userSortFlag[2] = false;
			m_userSortFlag[3] = false;
		}
	}
	if (JpSharedDoc_GetCurrentRoomTab() != 0) {
		return;
	}
	// Holding the left button on a pressed category button opens its list
	// after 0.8 s; the list stays open while the cursor is on the button
	// or the list.
	if (g_input->GetButton(LEFT_BUTTON) == 3) {
		WVector mouse = g_input->GetMousePoint();
		WVector down = g_input->GetButtonDownPos(LEFT_BUTTON);
		WRect vsButtonRect = m_pCateVsButton->GetRect();
		WRect vsListRect = m_pVsList->GetRect();
		WRect massButtonRect = m_pCateMassButton->GetRect();
		WRect massListRect = m_pMassList->GetRect();
		WRect battleButtonRect = m_pCateBattleButton->GetRect();
		WRect battleListRect = m_pBattleList->GetRect();
		WPoint downPt(down.x, down.y);
		WPoint mousePt(mouse.x, mouse.y);
		if (m_pCateVsButton->GetStatus() == FrButton::PRESSED
			&& vsButtonRect.IsInRect(downPt)
			&& (vsButtonRect.IsInRect(mousePt) || vsListRect.IsInRect(mousePt))) {
			m_value2d8 += dt;
			if (m_value2d8 > 0.8f) {
				m_pVsList->SetVisible(true);
			}
		} else {
			m_value2d8 = 0.0f;
			m_pVsList->SetVisible(false);
		}
		if (m_pCateMassButton->GetStatus() == FrButton::PRESSED
			&& massButtonRect.IsInRect(downPt)
			&& (massButtonRect.IsInRect(mousePt) || massListRect.IsInRect(mousePt))) {
			m_value2dc += dt;
			if (m_value2dc > 0.8f) {
				m_pMassList->SetVisible(true);
			}
		} else {
			m_value2dc = 0.0f;
			m_pMassList->SetVisible(false);
		}
		if (m_pCateBattleButton->GetStatus() == FrButton::PRESSED
			&& battleButtonRect.IsInRect(downPt)
			&& (battleButtonRect.IsInRect(mousePt) || battleListRect.IsInRect(mousePt))) {
			m_value2e0 += dt;
			if (m_value2e0 > 0.8f) {
				m_pBattleList->SetVisible(true);
			}
		} else {
			m_value2e0 = 0.0f;
			m_pBattleList->SetVisible(false);
		}
	} else {
		if (m_pVsList->IsVisible()) {
			OnVsListLBtnDown();
		}
		if (m_pMassList->IsVisible()) {
			OnMassListLBtnDown();
		}
		if (m_pBattleList->IsVisible()) {
			OnBattleListLBtnDown();
		}
		m_value2d8 = 0.0f;
		m_value2dc = 0.0f;
		m_value2e0 = 0.0f;
	}
}

// 0x520900
void FrRoomListDlg::ReloadRoomList(void) {
	int channelType = JpLobby_GetAvatarChatMode();
	// The JP CSharedDoc +0x10d3c bit 7 (the KR m_serverProperty, like the GP
	// room tab's bit 11): the channel type is a mask of allowed categories.
	if (WSingleton<CSharedDoc>::Instance()->m_serverProperty.GetFlag(0x80)) {
		m_pCateTotalButton->Enable(true);
		m_pCateVsButton->Enable(true);
		m_pCateMassButton->Enable(true);
		m_pCateBattleButton->Enable(true);
		m_pCateChatButton->Enable(true);
		int disabled = 0;
		const char bits[5] = { 1, 2, 4, 8, 16 };
		for (int i = 0; i < 5; ++i) {
			if (channelType & bits[i]) {
				++disabled;
				DisableCategoryButton(bits[i]);
			}
		}
		if (disabled == 5) {
			m_pCateTotalButton->Enable(false);
			return;
		}
		OnCateTotalLBtnUp();
		return;
	}
	switch (channelType) {
	case 1:
		m_pCateTotalButton->Enable(true);
		m_pCateVsButton->Enable(true);
		m_pCateMassButton->Enable(true);
		m_pCateBattleButton->Enable(true);
		m_pCateChatButton->Enable(true);
		OnCateTotalLBtnUp();
		break;
	case 2:
		m_pCateTotalButton->Enable(false);
		m_pCateVsButton->Enable(true);
		m_pCateMassButton->Enable(false);
		m_pCateBattleButton->Enable(false);
		m_pCateChatButton->Enable(false);
		OnCateVsLBtnUp();
		break;
	case 4:
		m_pCateTotalButton->Enable(false);
		m_pCateVsButton->Enable(false);
		m_pCateMassButton->Enable(true);
		m_pCateBattleButton->Enable(false);
		m_pCateChatButton->Enable(false);
		OnCateMassLBtnUp();
		break;
	case 8:
		m_pCateTotalButton->Enable(false);
		m_pCateVsButton->Enable(false);
		m_pCateMassButton->Enable(false);
		m_pCateBattleButton->Enable(true);
		m_pCateChatButton->Enable(false);
		OnCateBattleLBtnUp();
		break;
	case 16:
		m_pCateTotalButton->Enable(false);
		m_pCateVsButton->Enable(false);
		m_pCateMassButton->Enable(false);
		m_pCateBattleButton->Enable(false);
		m_pCateChatButton->Enable(true);
		OnCateChatLBtnUp();
		break;
	case 14:
		m_pCateTotalButton->Enable(true);
		m_pCateVsButton->Enable(true);
		m_pCateMassButton->Enable(true);
		m_pCateBattleButton->Enable(true);
		m_pCateChatButton->Enable(false);
		OnCateTotalLBtnUp();
		break;
	}
}

// 0x520c40: the JP form of KR CLobbyMain::OnRoomList_UserListRBtnUp (JP
// FrListBox +0x104 = the KR m_pItemUnderCursor). The JP tests the CUserInfo
// instance pointer (0xe74da4) and its SetInfo (0x8a9ae0) takes two more
// arguments after the nick (0, 0) than the port's KR one; the empty-row
// text goes through Jp_LookupText (0xb4eb80) to CChatMsg::AddChatMsg
// (0x9a1600 on 0xe73e8c, colour 7).
void FrRoomListDlg::OnUserListRBtnUp(void) {
	FrListItem* pItem = m_pUserList->GetItemUnderCursor();
	if (!pItem) {
		return;
	}
	const sBriefUserInfo* pUser = (const sBriefUserInfo*)pItem->pData;
	if (pUser) {
		JpRoomListData_SelectUser(m_pRoomListData, pUser);
		if (m_pUserSelectWnd) {
			m_pUserSelectWnd->Enable(true);
		}
		if (WSingleton<CUserInfo>::Instance()) {
			WSingleton<CUserInfo>::Instance()->SetInfo(pUser->id, pUser->uid, true, true, false, false,
				pUser->nick);
		}
	} else {
		// 0xd07f70: the KR "invalid target." text, looked up.
		WSingleton<CChatMsg>::Instance()->AddChatMsg(
			Jp_LookupText("\xC0\xDF\xB8\xF8\xB5\xC8 \xB4\xEB\xBB\xF3\xC0\xD4\xB4\xCF\xB4\xD9."), 7, true, false);
	}
	pItem->selected = true;
}

// 0x51eba0 (the constructor; the JP list's merge 0x520df0 and sort
// 0x520f70 are its std::list instantiations): the room list's sort
// predicate. Unlike the KR FrListBoxGreater it never hands an empty row
// (NULL data, the padding rows) to the room predicates, which read their
// records unchecked: an empty first row goes after, an empty second row
// before.
JpRoomListGreater::JpRoomListGreater(bool (*compare)(const void*, const void*)) : m_compare(compare) {
}

bool JpRoomListGreater::operator()(FrListItem* const& a, FrListItem* const& b) const {
	if (!a->pData) {
		return false;
	}
	if (!b->pData) {
		return true;
	}
	return m_compare(a->pData, b->pData);
}

// 0x521260 (FrListBox::SortItem with the predicate above; the JP list's
// item list is at +0xd0, the KR m_itemList)
void JpRoomList_SortItem(FrListBox* list, JpRoomListGreater greater) {
	list->m_itemList.sort(greater);
	list->RecalcTopBottom();
}

// 0x521290
void FrRoomListDlg::SortRoomList(int column, int arg) {
	if (!m_pRoomList) {
		return;
	}
	m_value2ec = column;
	switch (column) {
	case 0:
		if (m_flag2e9) {
			JpRoomList_SortItem(m_pRoomList, JpRoomList_CompareNo);
		} else {
			JpRoomList_SortItem(m_pRoomList, JpRoomList_CompareNoRev);
		}
		break;
	case 1:
		break;
	case 2:
		if (m_flag2e9) {
			JpRoomList_SortItem(m_pRoomList, JpRoomList_CompareColumn2);
		} else {
			JpRoomList_SortItem(m_pRoomList, JpRoomList_CompareColumn2Rev);
		}
		break;
	case 4:
		if (m_flag2e9) {
			JpRoomList_SortItem(m_pRoomList, JpRoomList_CompareHole);
		} else {
			JpRoomList_SortItem(m_pRoomList, JpRoomList_CompareHoleRev);
		}
		break;
	case 3:
		if (m_flag2e9) {
			JpRoomList_SortItem(m_pRoomList, JpRoomList_CompareColumn3);
		} else {
			JpRoomList_SortItem(m_pRoomList, JpRoomList_CompareColumn3Rev);
		}
		break;
	case 5:
		if (m_flag2e9) {
			JpRoomList_SortItem(m_pRoomList, JpRoomList_CompareCourse);
		} else {
			JpRoomList_SortItem(m_pRoomList, JpRoomList_CompareCourseRev);
		}
		break;
	case 6:
		if (m_flag2e9) {
			JpRoomList_SortItem(m_pRoomList, JpRoomList_CompareState);
		} else {
			JpRoomList_SortItem(m_pRoomList, JpRoomList_CompareStateRev);
		}
		break;
	}
}

// 0x521470
void FrRoomListDlg::PadRoomList(int which) {
	FrListBox* list = NULL;
	if (which == 0) {
		list = m_pRoomList;
	} else if (which == 1) {
		list = m_pAltRoomList;
	}
	if (!list) {
		return;
	}
	// Six rows: empty rows (NULL data) are added, or removed down to six.
	int count = list->GetCurrentItemSize(true);
	if (count > 6) {
		int excess = count - 6;
		FrListBox::ITEM_LIST::iterator i = list->m_itemList.begin();
		while (i != list->m_itemList.end() && excess > 0) {
			if (!(*i)->pData) {
				--excess;
				i = list->_DelItem(i);
			} else {
				++i;
			}
		}
	} else if (count < 6) {
		int missing = 6 - count;
		do {
			list->AddItem(NULL);
		} while (--missing);
	}
	SortRoomList(m_value2ec, 0);
}

// 0x521510
void FrRoomListDlg::OnRoomNoLBtnDown(void) {
	if (m_value2cc > 0.0f) {
		return;
	}
	if (m_value2ec == 0) {
		m_flag2e9 = !m_flag2e9;
	}
	HideCategoryLists(0);
	m_value2cc = 0.3f;
	m_roomSortFlag[0] = true;
	SortRoomList(0, 0);
	g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

// 0x521590
void FrRoomListDlg::OnRoomHoleLBtnDown(void) {
	if (m_value2cc > 0.0f) {
		return;
	}
	if (m_value2ec == 4) {
		m_flag2e9 = !m_flag2e9;
	}
	HideCategoryLists(4);
	m_value2cc = 0.3f;
	m_roomSortFlag[1] = true;
	SortRoomList(4, 0);
	g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

// 0x521610
void FrRoomListDlg::OnRoomCourseLBtnDown(void) {
	if (m_value2cc > 0.0f) {
		return;
	}
	if (m_value2ec == 5) {
		m_flag2e9 = !m_flag2e9;
	}
	HideCategoryLists(5);
	m_value2cc = 0.3f;
	m_roomSortFlag[2] = true;
	SortRoomList(5, 0);
	g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

// 0x521690
void FrRoomListDlg::OnRoomStateLBtnDown(void) {
	if (m_value2cc > 0.0f) {
		return;
	}
	if (m_value2ec == 6) {
		m_flag2e9 = !m_flag2e9;
	}
	HideCategoryLists(6);
	m_value2cc = 0.3f;
	m_roomSortFlag[3] = true;
	SortRoomList(6, 0);
	g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

// The room list's sort predicates (JP LobbyMain, file-local there), on the
// rooms' item data (KR sRoomInfo; the JP record has the same fields 0x10
// bytes further from m_isPublic on, after its longer title). Unlike the
// user list's, they do not test for NULL.

// 0x59f220
bool JpRoomList_CompareColumn2(const void* a, const void* b) {
	return lstrcmpiA(((const sRoomInfo*)a)->m_title, ((const sRoomInfo*)b)->m_title) > 0;
}

// 0x59f230
bool JpRoomList_CompareColumn2Rev(const void* a, const void* b) {
	return lstrcmpiA(((const sRoomInfo*)a)->m_title, ((const sRoomInfo*)b)->m_title) < 0;
}

// 0x59f240
bool JpRoomList_CompareColumn3(const void* a, const void* b) {
	const sRoomInfo* pA = (const sRoomInfo*)a;
	const sRoomInfo* pB = (const sRoomInfo*)b;
	if (pA->m_gameType == pB->m_gameType) {
		return pA->m_trophy > pB->m_trophy;
	}
	return pA->m_gameType > pB->m_gameType;
}

// 0x59f260 (only the game type order is reversed)
bool JpRoomList_CompareColumn3Rev(const void* a, const void* b) {
	const sRoomInfo* pA = (const sRoomInfo*)a;
	const sRoomInfo* pB = (const sRoomInfo*)b;
	if (pA->m_gameType == pB->m_gameType) {
		return pA->m_trophy > pB->m_trophy;
	}
	return pA->m_gameType < pB->m_gameType;
}

// The user list's sort predicates (JP LobbyMain, file-local there). The JP
// user record (0xc8 bytes) starts like the KR sBriefUserInfo: nick +0xa,
// level +0x20, the character type bits of +0x2d, guildUID +0x2e. A NULL
// first record sorts after, a NULL second one before.

// 0x59f280
bool JpUserList_CompareGenderDesc(const void* a, const void* b) {
	const sBriefUserInfo* pA = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pB = (const sBriefUserInfo*)b;
	if (!pA) {
		return false;
	}
	if (!pB) {
		return true;
	}
	return pA->charType > pB->charType;
}

// 0x59f2b0
bool JpUserList_CompareGenderAsc(const void* a, const void* b) {
	const sBriefUserInfo* pA = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pB = (const sBriefUserInfo*)b;
	if (!pA) {
		return false;
	}
	if (!pB) {
		return true;
	}
	return pA->charType < pB->charType;
}

// 0x59f2e0
bool JpUserList_CompareLevelDesc(const void* a, const void* b) {
	const sBriefUserInfo* pA = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pB = (const sBriefUserInfo*)b;
	if (!pA) {
		return false;
	}
	if (!pB) {
		return true;
	}
	return pA->level > pB->level;
}

// 0x59f300
bool JpUserList_CompareLevelAsc(const void* a, const void* b) {
	const sBriefUserInfo* pA = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pB = (const sBriefUserInfo*)b;
	if (!pA) {
		return false;
	}
	if (!pB) {
		return true;
	}
	return pA->level < pB->level;
}

// 0x59f320
bool JpUserList_CompareGuildDesc(const void* a, const void* b) {
	const sBriefUserInfo* pA = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pB = (const sBriefUserInfo*)b;
	if (!pA) {
		return false;
	}
	if (!pB) {
		return true;
	}
	return pA->guildUID > pB->guildUID;
}

// 0x59f340
bool JpUserList_CompareGuildAsc(const void* a, const void* b) {
	const sBriefUserInfo* pA = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pB = (const sBriefUserInfo*)b;
	if (!pA) {
		return false;
	}
	if (!pB) {
		return true;
	}
	return pA->guildUID < pB->guildUID;
}

// 0x59f360
bool JpUserList_CompareNickDesc(const void* a, const void* b) {
	const sBriefUserInfo* pA = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pB = (const sBriefUserInfo*)b;
	if (!pA) {
		return false;
	}
	if (!pB) {
		return true;
	}
	return lstrcmpiA(pA->nick, pB->nick) > 0;
}

// 0x59f390
bool JpUserList_CompareNickAsc(const void* a, const void* b) {
	const sBriefUserInfo* pA = (const sBriefUserInfo*)a;
	const sBriefUserInfo* pB = (const sBriefUserInfo*)b;
	if (!pA) {
		return false;
	}
	if (!pB) {
		return true;
	}
	return lstrcmpiA(pA->nick, pB->nick) < 0;
}

// The room predicates below first put the rooms of game type 11 first, then
// the lounge-sized rooms (another game type, m_option[1] set and more than
// 99 places), and only then compare their column (inlined in each of them;
// name ours).
static bool JpRoomList_IsLoungeRoom(const sRoomInfo* room) {
	return room->m_gameType != 11 && room->m_option[1] != 0 && room->m_userLimit > 99;
}

// The common head of the room predicates: 1 = a first, 0 = b first, -1 =
// undecided (name ours).
static int JpRoomList_CompareGroups(const sRoomInfo* a, const sRoomInfo* b) {
	if (a->m_gameType != 11 && b->m_gameType == 11) {
		return 0;
	}
	if (a->m_gameType == 11 && b->m_gameType != 11) {
		return 1;
	}
	if (!JpRoomList_IsLoungeRoom(a) && JpRoomList_IsLoungeRoom(b)) {
		return 0;
	}
	if (JpRoomList_IsLoungeRoom(a) && !JpRoomList_IsLoungeRoom(b)) {
		return 1;
	}
	return -1;
}

// 0x59f510
bool JpRoomList_CompareNo(const void* a, const void* b) {
	const sRoomInfo* pA = (const sRoomInfo*)a;
	const sRoomInfo* pB = (const sRoomInfo*)b;
	int group = JpRoomList_CompareGroups(pA, pB);
	if (group >= 0) {
		return group != 0;
	}
	return pA->m_roomNumber > pB->m_roomNumber;
}

// 0x59f580
bool JpRoomList_CompareNoRev(const void* a, const void* b) {
	const sRoomInfo* pA = (const sRoomInfo*)a;
	const sRoomInfo* pB = (const sRoomInfo*)b;
	int group = JpRoomList_CompareGroups(pA, pB);
	if (group >= 0) {
		return group != 0;
	}
	return pA->m_roomNumber < pB->m_roomNumber;
}

// 0x59f5f0
bool JpRoomList_CompareHole(const void* a, const void* b) {
	const sRoomInfo* pA = (const sRoomInfo*)a;
	const sRoomInfo* pB = (const sRoomInfo*)b;
	int group = JpRoomList_CompareGroups(pA, pB);
	if (group >= 0) {
		return group != 0;
	}
	return pA->m_holeCount > pB->m_holeCount;
}

// 0x59f660
bool JpRoomList_CompareHoleRev(const void* a, const void* b) {
	const sRoomInfo* pA = (const sRoomInfo*)a;
	const sRoomInfo* pB = (const sRoomInfo*)b;
	int group = JpRoomList_CompareGroups(pA, pB);
	if (group >= 0) {
		return group != 0;
	}
	return pA->m_holeCount < pB->m_holeCount;
}

// 0x59f6d0
bool JpRoomList_CompareCourse(const void* a, const void* b) {
	const sRoomInfo* pA = (const sRoomInfo*)a;
	const sRoomInfo* pB = (const sRoomInfo*)b;
	int group = JpRoomList_CompareGroups(pA, pB);
	if (group >= 0) {
		return group != 0;
	}
	return pA->m_course > pB->m_course;
}

// 0x59f740
bool JpRoomList_CompareCourseRev(const void* a, const void* b) {
	const sRoomInfo* pA = (const sRoomInfo*)a;
	const sRoomInfo* pB = (const sRoomInfo*)b;
	int group = JpRoomList_CompareGroups(pA, pB);
	if (group >= 0) {
		return group != 0;
	}
	return pA->m_course < pB->m_course;
}

// The state order of 0x59f7b0 / 0x59f8b0 once the groups are equal: true
// when "first" goes before "second". Public rooms first; then only rooms
// that both have m_intrusionAccess 1 go on (else first goes after): full
// rooms last, then m_option[0] set first, then games in progress first
// (inlined in both; name ours).
static bool JpRoomList_StateBefore(const sRoomInfo* first, const sRoomInfo* second) {
	if (second->m_isPublic == 0 && first->m_isPublic == 1) {
		return false;
	}
	if (second->m_isPublic == 1 && first->m_isPublic == 0) {
		return true;
	}
	if (second->m_intrusionAccess == 0 && first->m_intrusionAccess == 1) {
		return false;
	}
	if (second->m_intrusionAccess == 1 && first->m_intrusionAccess == 0) {
		return true;
	}
	if (second->m_intrusionAccess == 1 && first->m_intrusionAccess == 1) {
		if (second->m_userCount == second->m_userLimit && first->m_userCount < first->m_userLimit) {
			return false;
		}
		if (second->m_userCount < second->m_userLimit && first->m_userCount == first->m_userLimit) {
			return true;
		}
		if (second->m_option[0] == 1 && first->m_option[0] == 0) {
			return false;
		}
		if (second->m_option[0] == 0 && first->m_option[0] == 1) {
			return true;
		}
		if (!second->m_state && first->m_state) {
			return false;
		}
		if (second->m_state && !first->m_state) {
			return true;
		}
	}
	return false;
}

// 0x59f7b0
bool JpRoomList_CompareState(const void* a, const void* b) {
	const sRoomInfo* pA = (const sRoomInfo*)a;
	const sRoomInfo* pB = (const sRoomInfo*)b;
	int group = JpRoomList_CompareGroups(pA, pB);
	if (group >= 0) {
		return group != 0;
	}
	return JpRoomList_StateBefore(pA, pB);
}

// 0x59f8b0 (the group order is kept, the state order reversed)
bool JpRoomList_CompareStateRev(const void* a, const void* b) {
	const sRoomInfo* pA = (const sRoomInfo*)a;
	const sRoomInfo* pB = (const sRoomInfo*)b;
	int group = JpRoomList_CompareGroups(pA, pB);
	if (group >= 0) {
		return group != 0;
	}
	return JpRoomList_StateBefore(pB, pA);
}

// Port glue for the 0xa5 container (CLobbyContainer,
// JpContentsContainers.h): the JP reads and writes its fields inline.
// Its +0x8, the shown room category.
int JpRoomListData_GetCategory(const IContentsDataContainer* data) {
	return static_cast<const CLobbyContainer*>(data)->m_roomCategory;
}

// Its +0x8 (FillRoomList mode 1).
void JpRoomListData_SetCategory(IContentsDataContainer* data, int category) {
	static_cast<CLobbyContainer*>(data)->m_roomCategory = category;
}

// Its +0x4 (FillRoomList mode 3: the game type the list shows; -1 none).
void JpRoomListData_SetSelection(IContentsDataContainer* data, int value) {
	static_cast<CLobbyContainer*>(data)->m_selectedRoom = value;
}

// Its +0x4 (the multiplayer handler's Attach).
int JpRoomListData_GetSelection(const IContentsDataContainer* data) {
	return static_cast<const CLobbyContainer*>(data)->m_selectedRoom;
}

// Its +0x4 = -1 (the category buttons).
void JpRoomListData_ResetSelection(IContentsDataContainer* data) {
	static_cast<CLobbyContainer*>(data)->m_selectedRoom = -1;
}

// Its +0xc.
uint32_t JpRoomListData_GetSelectedUid(const IContentsDataContainer* data) {
	return static_cast<const CLobbyContainer*>(data)->m_selectedUserId;
}

// The user record's +0x4 (uid) to +0xc, its +0x0 (id) to +0x10.
void JpRoomListData_SelectUser(IContentsDataContainer* data, const void* userRecord) {
	const sBriefUserInfo* user = (const sBriefUserInfo*)userRecord;
	CLobbyContainer* lobby = static_cast<CLobbyContainer*>(data);
	lobby->m_selectedUserId = user->uid;
	lobby->m_selectedUserNo = user->id;
}
