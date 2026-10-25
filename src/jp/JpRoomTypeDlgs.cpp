// JP-only game code: the four room dialogs derived from FrGameRoomDlg
// (JpRoomTypeDlgs.h), in JP address order. Bodies read from the JP exe
// (node scripts/disasm.ts --jp); the members that read JP game state not
// in the port are declared only (the header says which).
#include "JpRoomTypeDlgs.h"

#include "InputManager.h"
#include "bitmap.h"
#include "commonutil.h"
#include "ObjectFactory.h"
#include "Packet.h"
#include "frarea.h"
#include "frbutton.h"
#include "frcombobox.h"
#include "frframe.h"
#include "frgaugebar.h"
#include "frgraphicinterface.h"
#include "frgroupbox.h"
#include "frviewer.h"
#include "JpHoleSelector.h"
#include "JpChipInContainer.h"
#include "JpLobbyGlue.h"
#include "RFTextManager.h"
#include "RFLobbyDesktop.h"
#include "shareddoc.h"
#include "ItemManager.h"
#include "NetResourceManager.h"
#include "ProjectG.h"
#include "SoundManager.h"
#include "fresh.h"
#include "frlistbox.h"
#include "frwndmanager.h"
#include "Intrusion.h"
#include "ScoreDlg.h"
#include "icontentsdoc.h"
#include "shared/luasystem/LuaScript.h"
#include "shared/luasystem/LuaScriptManager.h"
#include "shared/luasystem/luatablehelper.h"
#include "TaskManager.h"
#include "client/Wangreal/include/wfont.h"
#include "client/Wangreal/include/wresrcmng.h"

#include <list>
#include <map>
#include <string.h>
#include <vector>

// The JP FrListBox fields OnUserInit and On30sUserInit set inline (dword
// +0xfc = 1, byte +0x114 = 1); declared only (JpGameRoomDlg.cpp).
void JpFrListBox_SetRoomUserListFlags(FrListBox* list);
// JP 0x59fc40 (in the JP LobbyMain unit, unpaired; the KR
// CLobbyMain::GetMedalIndex there, a free function taking the uid in ecx;
// name ours, defined below): the medal of a result row, 0-2, else 3.
int JpLobbyMain_GetMedalIndex(uint32_t uid);
int JpSharedDoc_GetMedalForRank(int memberCount, int rank);
// The JP room player record's byte +0x159 (no KR sSlotInfo counterpart):
// set for a Grand Prix AI player, whose GP user row has no level icon
// (OnGPUserOwnerDraw 0x5143b0; role inferred from the GP waiting room's AI
// slots). The port's KR records never come from the GP AI table: false
// (accessor ours).
bool JpSlotInfo_IsGrandPrixAi(const sSlotInfo& slot) {
	return false;
}

// JP CSharedDoc dwords +0x106a0 and +0x1057c, whose sum
// FrMassTypeRoomDlg::OnProc counts down to, and the JP global tick 0xf022d4
// (76 readers) it compares with, in milliseconds (accessors ours, declared
// only; roles not proven; REQUEST for JpLobbyGlue).
uint32_t JpSharedDoc_GetRoom106a0(void);
uint32_t JpSharedDoc_GetRoom1057c(void);
uint32_t Jp_GetTickF022d4(void);
// JP CSharedDoc byte +0x11609: the single room's fixed hole cup position
// option, toggled by "course_ctrl.fix_holecup_pos" (accessors ours,
// declared only; REQUEST for JpLobbyGlue).
bool JpSharedDoc_IsHolecupPosFixed(void);
void JpSharedDoc_SetHolecupPosFixed(bool fixed);
// The JP CUserInfo (instance 0xe74da4) glue of JpGameRoomDlg.cpp (declared
// only there): whether it exists, and the user info form of a room user
// (the JP map +0x109d4 entry's room number, then CUserInfo::SetInfo).
bool JpUserInfo_IsCreated(void);
void JpRoomUser_OpenUserInfo(uint32_t userId);

// 0xf023e0 / 0xf023e4: the game's two WInputDev devices (ProjectG.cpp's
// g_keyboard and g_mouse, as in JpGameRoomDlg.cpp).
extern WInputDev* g_keyboard;
extern WInputDev* g_mouse;
// 0x633580 (thiscall on the JP CSharedDoc): the room's starting hole, which
// the "hole_selector" form opens on (accessor ours, declared only).
unsigned char JpSharedDoc_GetRoomHole(void);

// lib: 0xccc4a0 (m_RTTI's dynamic initializer, compiler-generated from the definition below)
const WRTTI FrMassTypeRoomDlg::m_RTTI("FrMassTypeRoomDlg", &FrGameRoomDlg::m_RTTI);

// lib: 0xccc890 (m_RTTI's dynamic initializer, compiler-generated from the definition below)
const WRTTI FrGPTypeRoomDlg::m_RTTI("FrGPTypeRoomDlg", &FrMassTypeRoomDlg::m_RTTI);

// lib: 0xccdaa0 (m_RTTI's dynamic initializer, compiler-generated from the definition below)
const WRTTI FrSingleTypeDlg::m_RTTI("FrSingleTypeDlg", &FrGameRoomDlg::m_RTTI);

// lib: 0xccdd80 (m_RTTI's dynamic initializer, compiler-generated from the definition below)
const WRTTI FrStrokeTypeRoomDlg::m_RTTI("FrStrokeTypeRoomDlg", &FrGameRoomDlg::m_RTTI);

// 0x510a10
const sFRESH_MSGMAP* FrMassTypeRoomDlg::GetMessageMap(void) const {
	return &FrMassTypeRoomDlg::_MsgMap;
}

sFRESH_MSGMAP FrMassTypeRoomDlg::_MsgMap = { &FrGameRoomDlg::_MsgMap, &FrMassTypeRoomDlg::_MsgEntries[0] };

// 0xe09478 (the handlers are stored by the static init 0xccc4e0)
sFRESH_ENTRY FrMassTypeRoomDlg::_MsgEntries[] = {
	{ "invite", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::OnInviteInit },
	{ "invite", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrMassTypeRoomDlg::OnInviteLBtnUp },
	{ "group_30s.room_event", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::OnRoomEventOwnerDraw },
	{ "group_30s.user", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::On30sUserInit },
	{ "group_30s.user", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::On30sUserOwnerDraw },
	{ "group_30s.user", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrMassTypeRoomDlg::OnUserLBtnDown },
	{ "group_30s.user", FRCMD_RBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrMassTypeRoomDlg::OnUserRBtnUp },
	{ "group_30s.user", FRCMD_DBLCLICK, FrFuncVV, (FRESH_PMSG)&FrMassTypeRoomDlg::OnUserDblClick },
	{ "group_30s.30_team1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::On30sTeam1Init },
	{ "group_30s.30_team2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::On30sTeam2Init },
	{ "group_30s.30_guild1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::On30sGuild1Init },
	{ "group_30s.30_guild2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::On30sGuild2Init },
	{ "group_30s.30_team_num", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::On30sTeamNumInit },
	{ "group_30s.30_team_num", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::On30sTeamNumOwnerDraw },
	{ "group_30s.30_guild_num", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::On30sGuildNumInit },
	{ "group_30s.30_guild_num", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::On30sGuildNumOwnerDraw },
	{ "group_30s.countdown", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::On30sCountdownInit },
	{ "group_result.user_info", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::OnResultUserInfoInit },
	{ "group_result.user_info", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrMassTypeRoomDlg::OnResultUserInfoLBtnDown },
	{ "group_result.rankframe", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::OnResultRankFrameInit },
	{ "group_result.score_gb", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::OnResultScoreGbInit },
	{ "group_result.rankingtab", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::OnResultRankingTabInit },
	{ "group_result.ranking", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::OnResultRankingInit },
	{ "group_result.ranking", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::OnResultRankingOwnerDraw },
	{ "group_result.ranking", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrMassTypeRoomDlg::OnResultRankingLBtnDown },
	{ "group_result.ranking", FRCMD_RBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrMassTypeRoomDlg::OnResultRankingRBtnUp },
	{ "group_result.guildranking", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::OnResultGuildRankingInit },
	{ "group_result.guildranking", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::OnResultGuildRankingOwnerDraw },
	{ "group_result.guildranking", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrMassTypeRoomDlg::OnResultGuildRankingLBtnDown },
	{ "group_result.guildranking", FRCMD_RBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrMassTypeRoomDlg::OnResultGuildRankingRBtnUp },
	{ "group_result.score", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMassTypeRoomDlg::OnResultScoreInit },
	{ "group_result.score", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrMassTypeRoomDlg::OnResultScoreLBtnDown },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x510a40
void FrMassTypeRoomDlg::SetupStateResult(void) {
	OpenNewRecordForm();
	if (m_pMap) {
		m_pMap->SetVisible(false);
	}
	if (m_pMapPrev) {
		m_pMapPrev->SetVisible(false);
	}
	if (m_pMapNext) {
		m_pMapNext->SetVisible(false);
	}
	if (m_pMapCombo) {
		m_pMapCombo->SetVisible(false);
	}
	if (m_pStart) {
		m_pStart->SetVisible(false);
	}
	if (m_pReady) {
		m_pReady->SetVisible(false);
	}
	if (m_pModify) {
		m_pModify->SetVisible(false);
	}
	if (m_pShortGame) {
		m_pShortGame->SetVisible(false);
	}
	if (m_pCurCaddie) {
		m_pCurCaddie->SetVisible(false);
	}
	if (m_pCurClub) {
		m_pCurClub->SetVisible(false);
	}
	if (m_pCurAztec) {
		m_pCurAztec->SetVisible(false);
	}
	if (m_pCurMascot) {
		m_pCurMascot->SetVisible(false);
	}
	if (m_pCurEquipItem) {
		m_pCurEquipItem->SetVisible(false);
	}
}

// 0x510bf0
const sFRESH_MSGMAP* FrGPTypeRoomDlg::GetMessageMap(void) const {
	return &FrGPTypeRoomDlg::_MsgMap;
}

sFRESH_MSGMAP FrGPTypeRoomDlg::_MsgMap = { &FrMassTypeRoomDlg::_MsgMap, &FrGPTypeRoomDlg::_MsgEntries[0] };

// 0xe09898 (the handlers are stored by the static init 0xccc8d0)
sFRESH_ENTRY FrGPTypeRoomDlg::_MsgEntries[] = {
	{ "group_gp.user", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGPTypeRoomDlg::OnUserInit },
	{ "group_gp.user", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGPTypeRoomDlg::OnGPUserOwnerDraw },
	{ "group_gp.user", FRCMD_RBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrGPTypeRoomDlg::OnUserRBtnUp },
	{ "group_gp.gp_detail", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGPTypeRoomDlg::OnGPDetailInit },
	{ "group_gp.gp_detail", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGPTypeRoomDlg::OnGPDetailHoverOn },
	{ "group_gp.gp_detail", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGPTypeRoomDlg::OnGPDetailHoverOff },
	{ "group_gp.grandprixinfo", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGPTypeRoomDlg::OnGrandPrixInfoInit },
	{ "group_gp.grandprixinfo", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrGPTypeRoomDlg::OnGrandPrixInfoOwnerDraw },
	{ "group_gp.game_start_waiting", FRCMD_INIT, FrFuncVV, (FRESH_PMSG)&FrGPTypeRoomDlg::OnGameStartWaitingInit },
	{ "group_gp.game_start_waiting", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrGPTypeRoomDlg::OnGameStartWaitingOwnerDraw },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x510fd0
FrMassTypeRoomDlg::~FrMassTypeRoomDlg(void) {
}

// 0x511020
void FrGPTypeRoomDlg::ReleaseGrandPrixRoomUi(void) {
	g_audio->StopSfx("gp_lobby_amb");
	if (m_pGrandPrixRoom) {
		delete m_pGrandPrixRoom;
		m_pGrandPrixRoom = NULL;
	}
}

// 0x511060
void FrMassTypeRoomDlg::On30sTeam1Init(PG_MSGARG param) {
	m_p30sTeam1 = DYNAMIC_CAST(FrArea, param);
}

// 0x5110d0
void FrMassTypeRoomDlg::On30sTeam2Init(PG_MSGARG param) {
	m_p30sTeam2 = DYNAMIC_CAST(FrArea, param);
}

// 0x511140
void FrMassTypeRoomDlg::On30sGuild1Init(PG_MSGARG param) {
	m_p30sGuild1 = DYNAMIC_CAST(FrArea, param);
}

// 0x5111b0
void FrMassTypeRoomDlg::On30sGuild2Init(PG_MSGARG param) {
	m_p30sGuild2 = DYNAMIC_CAST(FrArea, param);
}

// 0x511220
void FrMassTypeRoomDlg::On30sTeamNumInit(PG_MSGARG param) {
	m_p30sTeamNum = DYNAMIC_CAST(FrArea, param);
}

// 0x511290
void FrMassTypeRoomDlg::OnInviteInit(PG_MSGARG param) {
	m_pInvite = DYNAMIC_CAST(FrButton, param);
}

// 0x511300
void FrMassTypeRoomDlg::On30sCountdownInit(PG_MSGARG param) {
	m_p30sCountdown = DYNAMIC_CAST(FrArea, param);
	if (m_p30sCountdown) {
		m_p30sCountdown->SetVisible(false);
	}
}

// 0x511380
void FrMassTypeRoomDlg::OnResultUserInfoInit(PG_MSGARG param) {
	m_pResultUserInfo = DYNAMIC_CAST(FrButton, param);
	if (m_pResultUserInfo) {
		m_pResultUserInfo->Enable(false);
	}
}

// 0x511400 (RTTI 0xf1aba4: FrFrame, GetRTTI 0xc06470)
void FrMassTypeRoomDlg::OnResultRankFrameInit(PG_MSGARG param) {
	m_pResultRankFrame = DYNAMIC_CAST(FrFrame, param);
}

// 0x511470 (KR CLobbyMain::OnGameRoomExtRes_RoomUserRankLBtnUp; JP reads
// the list's selected item, not the one under the cursor): the picked
// player goes to the score card when it is open, else enables its buttons.
void FrMassTypeRoomDlg::OnResultRankingLBtnDown(void) {
	FrListItem* item = m_pResultRanking->GetSelected();
	if (item) {
		sRivalData* info = (sRivalData*)item->pData;
		if (info) {
			m_pLobbyData->m_selectedUserId = info->m_uid;
			m_pLobbyData->m_selectedUserNo = info->m_id;
			if (m_pScoreDlg) {
				m_pScoreDlg->SetPlayer(m_pLobbyData->m_selectedUserId);
				return;
			}
			if (m_pResultScore) {
				m_pResultScore->Enable(true);
			}
			if (m_pResultUserInfo) {
				m_pResultUserInfo->Enable(true);
			}
		}
	}
}

// 0x5114f0 (KR CLobbyMain::OnGameRoomExtRes_RoomGuildRankLBtnUp): the
// picked match's first player goes to the guild score card.
void FrMassTypeRoomDlg::OnResultGuildRankingLBtnDown(void) {
	FrListItem* item = m_pResultGuildRanking->GetSelected();
	if (item) {
		sGuildMatchup* matchup = (sGuildMatchup*)item->pData;
		if (matchup) {
			if (m_pScoreDlg) {
				m_pScoreDlg->SetGuildPlayer(matchup->m_guidA);
				return;
			}
			if (m_pResultScore) {
				m_pResultScore->Enable(true);
			}
		}
	}
}

// 0x511540
void FrMassTypeRoomDlg::OnResultScoreInit(PG_MSGARG param) {
	m_pResultScore = DYNAMIC_CAST(FrButton, param);
	if (m_pResultScore) {
		m_pResultScore->Enable(false);
	}
}

// 0x5115c0
FrGPTypeRoomDlg::~FrGPTypeRoomDlg(void) {
	ReleaseGrandPrixRoomUi();
}

// 0x5116b0 (JP FrWnd m_nFlags +0x1c bit 0: the tool tip)
void FrGPTypeRoomDlg::OnGPDetailHoverOn(PG_MSGARG param) {
	m_pGPDetail->EnableToolTip(true);
}

// 0x5116c0
void FrGPTypeRoomDlg::OnGPDetailHoverOff(PG_MSGARG param) {
	m_pGPDetail->EnableToolTip(false);
}

// 0x5116d0
void FrGPTypeRoomDlg::OnGrandPrixInfoInit(PG_MSGARG param) {
	m_pGrandPrixInfo = DYNAMIC_CAST(FrArea, param);
}

// 0x511c90
void FrMassTypeRoomDlg::OnProc(const float dt) {
	FrGameRoomDlg::OnProc(dt);
	if (m_roomState == 8) {
		uint32_t endTick = JpSharedDoc_GetRoom106a0() + JpSharedDoc_GetRoom1057c();
		uint32_t now = Jp_GetTickF022d4();
		if (endTick > now) {
			uint32_t seconds = (endTick - now) / 1000;
			UpdateTitle(MakeStr("%02d:%02d", seconds / 60, seconds % 60));
			return;
		}
		UpdateTitle("00:00");
	}
}

// JP CSharedDoc +0x109c5: the room's grand prix type id
// (JpGrandPrixAwardDlg.cpp).
uint32_t JpSharedDoc_GetGrandPrixTypeId(void);
// 0xa17c10 (JP CItemManager at CSharedDoc +0x20c, in the JP ItemManager
// unit; name ours): whether its grand prix table (a map at +0x504, lookup
// 0xa17b50) holds the type id. The port has no JP grand prix table: port
// glue, false.
static bool JpItemManager_HasGrandPrix(uint32_t typeId) {
	(void)typeId;
	return false;
}

// 0x511d90
void FrGPTypeRoomDlg::SetupGrandPrixCourse(void) {
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	// JP +0x10950: the KR m_roomInfo.m_course.
	SetCourseImages(JpItemManager_HasGrandPrix(JpSharedDoc_GetGrandPrixTypeId()) ? 0x11 : doc->m_roomInfo.m_course);
	UpdateTitle(NULL);
	m_pStart->Enable(false);
	m_pStart->SetVisible(false);
	m_pReady->Enable(true);
	m_pReady->SetVisible(true);
	m_gpEntryItemCount = 0;
	AfxGetTask()->PassMsgToActor("Gateway",
		MsgObject(0, 0x753e, 4, 0x1a000264, (intptr_t)&m_gpEntryItemCount, 0, 0));
	if (m_pGrandPrixRoom->HasFixedEquipment()) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x4f, 0, 0, 0, 0, 0));
		EnableEquipViewers(false);
	}
	SetLevelBar();
	g_audio->PlaySfx("gp_lobby_amb", NULL, 1, "gp_lobby_amb", NULL, 0.5f, 200.0f);
}

// 0x511f10 (KR CLobbyMain::OnGameRoom_EventOwnerDraw): the event badge of
// a lounge-sized room. JP CSharedDoc +0x1094c, +0x1095e and +0x10937: the
// KR sRoomInfo m_gameType, m_option[1] and m_userLimit.
void FrMassTypeRoomDlg::OnRoomEventOwnerDraw(PG_MSGARG param) {
	const sRoomInfo& room = WSingleton<CSharedDoc>::Instance()->m_roomInfo;
	if (room.m_gameType != 0xb && room.m_option[1] != 0 && room.m_userLimit > 99) {
		FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
		if (gdi) {
			if (m_pRoomEventBitmap == 0) {
				m_pRoomEventBitmap = g_pFresh->GetBitmap("room_event02");
			}
			if (m_pRoomEventBitmap) {
				gdi->DrawTexture(m_pRoomEventBitmap, WRect(8.0f, 45.0f, 43.0f, 32.0f), 0xffffffff, 0);
			}
		}
	}
}

// JP CSharedDoc byte +0x109bd bit 1 (room record +0xb9): the room is a
// short game. The port's KR sRoomInfo has no such field (as
// JpRoomInfo_IsShortGame for the room list records): port glue, false
// (name ours).
static bool JpSharedDoc_IsRoomShortGame(void) {
	return false;
}

// 0x511fc0 (KR CLobbyMain::OnGameRoomExt_TeamNumOwnerDraw): the team sizes
// of a team room (JP game type 1 only; KR 1 or 5), else the room's
// category ("tournament", or "short game" in a short-game room) and its
// player count (JP CSharedDoc +0x10a00: the KR m_roomPlayerList.size()).
void FrMassTypeRoomDlg::On30sTeamNumOwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI) {
		return;
	}
	WRect rect = m_p30sTeamNum->GetRect();
	pGDI->SetTextStyle(0);
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if (doc->m_roomInfo.m_gameType == 1) {
		pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
		g_pFresh->GetManager()->PrintText(WPoint(rect.x + 209.0f, rect.y + 12.0f), 0,
			MakeStr("(%d)", (int)m_slotList1.size()), -1.0f, 0xFFFFFFFF);
		pGDI->SetTextColor(0xFF0060FF, 0xFFFFFFFF);
		g_pFresh->GetManager()->PrintText(WPoint(rect.x + 458.0f, rect.y + 12.0f), 0,
			MakeStr("(%d)", (int)m_slotList2.size()), -1.0f, 0xFFFFFFFF);
		return;
	}
	pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
	if (JpSharedDoc_IsRoomShortGame()) {
		g_pFresh->GetManager()->PrintText(WPoint(rect.x + 109.0f, rect.y + 12.0f), 0,
			Jp_LookupText("\xBC\xF4\xB0\xD4\xC0\xD3"), -1.0f, 0xFFFFFFFF);
	} else {
		g_pFresh->GetManager()->PrintText(WPoint(rect.x + 109.0f, rect.y + 12.0f), 0,
			Jp_LookupText("\xB4\xEB\xC8\xB8"), -1.0f, 0xFFFFFFFF);
	}
	float x = 209.0f;
	if (doc->m_roomPlayerList.size() >= 100) {
		x = 200.0f;
	}
	g_pFresh->GetManager()->PrintText(WPoint(x + rect.x, rect.y + 12.0f), 0,
		MakeStr("(%d)", (int)doc->m_roomPlayerList.size()), -1.0f, 0xFFFFFFFF);
}

// 0x5121d0
void FrMassTypeRoomDlg::On30sGuildNumInit(PG_MSGARG param) {
	m_p30sGuildNum = DYNAMIC_CAST(FrArea, param);
	// JP CSharedDoc +0x1094c: 6 is the guild match.
	if (JpSharedDoc_GetRoomGameType() != 6) {
		m_p30sGuildNum->SetVisible(false);
	}
}

// 0x512250 (KR CLobbyMain::OnGameRoomExt_GuildNumOwnerDraw): each guild's
// emblem, name and member count (red, then blue); in the result state (8,
// KR: the GAMEROOM_EXTRES layout) both guilds' points after them. JP
// CSharedDoc: the guild UIDs +0x1095f / +0x10963, the names +0x10983 /
// +0x10997 (the KR m_roomInfo.m_guild), the points +0x106e0 / +0x106e2
// (the KR m_teamPoint). The JP emblem lookup 0x9ab3e0 takes the guild UID
// and the words +0x1097f / +0x10981; the port's KR room info carries the
// emblem names (as OnResultGuildRankingInit).
void FrMassTypeRoomDlg::On30sGuildNumOwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI) {
		return;
	}
	WRect rect = m_p30sTeamNum->GetRect();
	pGDI->SetTextStyle(1);
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	const sGuildRoomInfo& guild = doc->m_roomInfo.m_guild;
	if (guild.m_guildUID[0] != 0) {
		const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(guild.m_guildMark[0]);
		if (pEmblem) {
			pGDI->DrawTexture(pEmblem, WRect(rect.x + 24.0f, rect.y + 8.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
		}
	}
	pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	g_pFresh->GetManager()->PrintText(WPoint(rect.x + 60.0f, rect.y + 12.0f), 0, guild.m_guildName[0], -1.0f, 0xFFFFFFFF);
	g_pFresh->GetManager()->PrintText(WPoint(rect.x + 209.0f, rect.y + 12.0f), 0,
		MakeStr("(%d)", (int)m_slotList1.size()), -1.0f, 0xFFFFFFFF);
	if (guild.m_guildUID[1] != 0) {
		const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(guild.m_guildMark[1]);
		if (pEmblem) {
			pGDI->DrawTexture(pEmblem, WRect(rect.x + 273.0f, rect.y + 8.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
		}
	}
	pGDI->SetTextColor(0xFF0060FF, 0xFFFFFFFF);
	g_pFresh->GetManager()->PrintText(WPoint(rect.x + 309.0f, rect.y + 12.0f), 0, guild.m_guildName[1], -1.0f, 0xFFFFFFFF);
	g_pFresh->GetManager()->PrintText(WPoint(rect.x + 458.0f, rect.y + 12.0f), 0,
		MakeStr("(%d)", (int)m_slotList2.size()), -1.0f, 0xFFFFFFFF);
	if (m_roomState == 8) {
		g_pFresh->GetManager()->PrintText(WPoint(rect.x + 180.0f, rect.y + 12.0f), 0,
			MakeStr("%d", (int)doc->m_teamPoint[0]), -1.0f, 0xFFFFFFFF);
		g_pFresh->GetManager()->PrintText(WPoint(rect.x + 429.0f, rect.y + 12.0f), 0,
			MakeStr("%d", (int)doc->m_teamPoint[1]), -1.0f, 0xFFFFFFFF);
	}
}

// 0x5125a0
void FrMassTypeRoomDlg::OnResultScoreGbInit(PG_MSGARG param) {
	m_pResultScoreGb = DYNAMIC_CAST(FrGaugeBar, param);
	m_pResultScoreGb->SetVisible(false);
}

// 0x512ee0
FrMassTypeRoomDlg::FrMassTypeRoomDlg(void) {
	m_pGroup30s = NULL;
	m_pGroupGP = NULL;
	if (m_roomTab == 0) {
		// 0x62bf90: JP CSharedDoc +0x11638.
		if (JpSharedDoc_GetLobbyMode() == 8) {
			InitRoomInLobbyMode8();
		} else {
			// 0x510a20 jumps to 0x503290.
			InitRoom();
		}
	}
}

// 0x512f80: the GP room's state, its object (+0x150) and the info box's
// images and offsets from "grandprix_room_ui.lua" (JP 0xc1ff80 the script
// manager, 0xc1feb0 CreateScript, 0xc20530 RunFile, 0xc20240 GetTable,
// 0x4d5ba0 a string entry, 0x4d5250 a table entry, 0x46e1f0 the float
// read, 0xc1fd50 ReleaseScript). Like the original, the loops write as many
// entries as the script's arrays hold.
void FrGPTypeRoomDlg::LoadGrandPrixRoomUi(void) {
	// The dword 0x17c (with the raw byte 0x17f) and the word 0x180.
	m_idleWarned = false;
	m_ready = false;
	m_readyLocked = false;
	m_pStartTip = NULL;
	m_pReadyTip = NULL;
	g_keyboard->ResetInputTime();
	g_mouse->ResetInputTime();
	if (m_pGrandPrixRoom == NULL) {
		m_pGrandPrixRoom = new JpGrandPrixRoomObject;
		// Not written: the object's setup 0x55dd40 (no declaration in
		// JpGameRoomDlg.h yet).
	}
	lua_system::lua_script* script = lua_system::lua_script_manger::GetInstance()->CreateScript(false);
	if (!script) {
		return;
	}
	script->RunFile("grandprix_room_ui.lua");
	memset(m_gpUiImages, 0, sizeof(m_gpUiImages));
	{
		lua_system::lua_table_helper table = script->GetTable("grandprixroomuiname");
		int count = table.GetArraySize();
		const Bitmap** image = m_gpUiImages;
		for (int i = 1; i <= count; ++i) {
			const char* name = table.GetValueByIndex<const char*>(i);
			if (name) {
				*image = g_pFresh->GetBitmap(name);
			}
			++image;
		}
	}
	{
		lua_system::lua_table_helper table = script->GetTable("grandprixroomuioffset");
		int count = table.GetArraySize();
		WPoint* offset = m_gpUiOffsets;
		for (int i = 1; i <= count; ++i) {
			float x;
			{
				lua_system::lua_table_helper entry(table.GetValueByIndex<lua_tinker::table>(i));
				x = entry.GetValueByIndex<float>(1);
			}
			float y;
			{
				lua_system::lua_table_helper entry(table.GetValueByIndex<lua_tinker::table>(i));
				y = entry.GetValueByIndex<float>(2);
			}
			offset->x = x;
			offset->y = y;
			++offset;
		}
	}
	lua_system::lua_script_manger::GetInstance()->ReleaseScript(script);
}

// 0x513270 (KR CLobbyMain::OnGameRoomExt_RoomUserOwnerDraw, same layout):
// one row of the 30-player user list. The row data is the port's KR
// sSlotInfo; the JP room player record is the KR one shifted down a byte
// (JP +0x2f identity, +0x33 title, +0x53 attribute, +0x54 manner/flag1,
// +0x55 level, +0x56 angel, +0x58 guild UID), and its +0x154 bit 0 is read
// where the KR twin reads m_flags151. JP differences from KR: local content
// 0x55 (KR 0x54), the team colours of the situation mark only in the guild
// match (game type 6; KR 5 or 6), no ladder icon, a GP host (identity
// 0x8000) shows LEVELS gp_host.
void FrMassTypeRoomDlg::On30sUserOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem) {
		return;
	}
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI) {
		return;
	}
	sSlotInfo* pSlot = (sSlotInfo*)pItem->pData;
	if (IsLocalContent((localContentType_t)0x55) && pSlot && pSlot->m_flags151.flag) {
		pGDI->DrawTexture(m_pInviteBitmap, WRect(pItem->pos.x, pItem->pos.y, (float)m_pInviteBitmap->Width(), (float)m_pInviteBitmap->Height()), 0xFFFFFFFF, 0);
		return;
	}
	pGDI->DrawTexture(m_p30sNoPlayerBase, WRect(pItem->pos.x, pItem->pos.y, (float)m_p30sPlayerBase->Width(), (float)m_p30sPlayerBase->Height()), 0xFFFFFFFF, 0);
	if (pItem->selected) {
		pGDI->Box(WRect(pItem->pos.x, pItem->pos.y, (float)m_pUserList->GetItemWidth() + 15.0, (float)m_pUserList->GetItemHeight()), 0x7F265785, 0, 0.0f);
	}
	if (!pSlot) {
		return;
	}
	const sRoomInfo& room = WSingleton<CSharedDoc>::Instance()->m_roomInfo;
	WRect rect(0.0f, 0.0f, 0.0f, 0.0f);
	// The situation mark: 0 waiting; 1 room master, 2 other states; in the
	// guild match +2 for team 0, +4 for the others.
	int situ;
	if (!pSlot->m_flags55.flag1 && !(pSlot->m_attribute & 0x18)) {
		situ = 0;
	} else {
		situ = (pSlot->m_attribute & 0x18) ? 1 : 2;
		// JP CSharedDoc +0x1094c: the KR m_roomInfo.m_gameType.
		if (room.m_gameType == 6) {
			if (!(pSlot->m_attribute & 3)) {
				situ += 2;
			} else {
				situ += 4;
			}
		}
	}
	const Bitmap* pSitu = m_p30sSituBitmap[situ];
	rect = WRect(pItem->pos.x + 10.0, pItem->pos.y + 5.0, (float)pSitu->Width(), (float)pSitu->Height());
	pGDI->DrawTexture(pSitu, rect, 0xFFFFFFFF, 0);
	// The gender icon: angel, manner, else the plain one of the user flag.
	const Bitmap* pGender;
	if (pSlot->m_flags57.flag0) {
		pGender = m_pGenderIcon[8 + pSlot->m_attributeBits.userFlag % 2];
	} else if (pSlot->m_flags55.flag0) {
		pGender = m_pGenderIcon[6 + pSlot->m_attributeBits.userFlag % 2];
	} else {
		pGender = m_pGenderIcon[pSlot->m_attributeBits.userFlag];
	}
	rect = WRect(pItem->pos.x + 30.0, pItem->pos.y, (float)pGender->Width(), (float)pGender->Height());
	pGDI->DrawTexture(pGender, rect, 0xFFFFFFFF, 0);
	if (m_pDiveBitmap && (pSlot->m_attribute & 4)) {
		pGDI->DrawTexture(m_pDiveBitmap, WRect(pItem->pos.x + 34.0, pItem->pos.y + 2.0, (float)m_pDiveBitmap->Width(), (float)m_pDiveBitmap->Height()), 0xFFFFFFFF, 0);
	}
	// The equipped title's icon (JP item manager at CSharedDoc +0x20c,
	// FindSkin 0xa174d0), else the level's.
	const char* levelName;
	if (pSlot->m_titleTypeId) {
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pSlot->m_titleTypeId);
		if (pSkin) {
			levelName = pSkin->m_icon;
		} else {
			levelName = MakeStr("level_%03d", pSlot->m_level + 1);
		}
	} else {
		levelName = MakeStr("level_%03d", pSlot->m_level + 1);
	}
	const Bitmap* pLevel = g_pFresh->GetBitmap(levelName);
	if (pSlot->m_identity & 0x8000) {
		pLevel = g_pFresh->GetManager()->GetBitmap("LEVELS", "gp_host");
	}
	if (pLevel) {
		pGDI->DrawTexture(pLevel, WRect(pItem->pos.x + 80.0 - float2int(pLevel->Width() * 0.5), pItem->pos.y + 13.0 - float2int(pLevel->Height() * 0.5), (float)pLevel->Width(), (float)pLevel->Height()), 0xFFFFFFFF, 0);
	}
	// The guild emblem. The JP lookup 0x9ab3e0 takes the guild UID and the
	// record's +0x68 dword; the port's KR record carries the emblem name
	// (as in FrRoomListDlg's user rows).
	if (pSlot->m_guildUID && !pSlot->IsIdentity(0x14)) {
		const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pSlot->m_emblemName);
		if (pEmblem) {
			pGDI->DrawTexture(pEmblem, WRect(pItem->pos.x + 115.0, pItem->pos.y + 7.0 - 6.0, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
		}
	}
	unsigned long color = ((unsigned char)pSlot->m_identity & 0x14) ? 0x50FFFFFF : 0xFFFFFFFF;
	if (pItem->underCursor) {
		pGDI->SetTextColor(0xFFFFFFFF, 0xFF124371);
		pGDI->SetTextStyle(2);
	} else {
		pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
		pGDI->SetTextStyle(0);
	}
	if (!WSingleton<CProjectG>::Instance()->HidePrivacy()) {
		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 140.0, pItem->pos.y + 7.0), 0, pSlot->m_nickname, -1.0f, color);
	}
	// The kick button, for the room master (the 0xa5 container's host, JP
	// CSharedDoc +0x81a the KR m_userInfo.guid) on players who are not
	// masters, outside the guild match; after the game only for an admin
	// (JP CSharedDoc +0x10488 the KR m_gameOver, +0x812 bit 2 AdminMode).
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if ((!doc->m_gameOver || AdminMode())
		&& doc->m_userInfo.guid == m_pLobbyData->m_hostUserId
		&& !(pSlot->m_attribute & 0x18)
		&& room.m_gameType != 6) {
		pGDI->DrawTexture(m_pKickBitmap, WRect(pItem->pos.x + 226.0, pItem->pos.y + 7.0 - 2.0, (float)m_pKickBitmap->Width(), (float)m_pKickBitmap->Height()), pItem->underCursor ? 0xFFFFFFFF : 0x80FFFFFF, 0);
	}
}

// The KR result ranking orders (LobbyMain.cpp). The JP 0x59f440 / 0x59f3e0
// compare the same keys of the JP records (+0x154 sort group, +0x14c
// score, +0x128 64-bit pang / +0x190 approach distance, +0x194 points,
// then +0x153); the port's rows are the KR records.
bool RankUserCompare(const void* lhs, const void* rhs);
bool ApproachRankUserCompare(const void* lhs, const void* rhs);

// 0x513910 (KR CLobbyMain::OnGameRoomExtRes_RoomUserRankInit): the result
// board's user ranking, hidden in guild rooms (JP CSharedDoc +0x10594, the
// KR m_roomType): one row per result record (JP +0x1042c, the KR
// m_gameResultList), sorted (approach order in room type 10), the rank
// images.
void FrMassTypeRoomDlg::OnResultRankingInit(PG_MSGARG param) {
	m_pResultRanking = DYNAMIC_CAST(FrListBox, param);
	if (!m_pResultRanking) {
		return;
	}
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if (doc->m_roomType == 6) {
		m_pResultRanking->SetVisible(false);
		return;
	}
	m_pResultRanking->UseRightButton(true);
	for (std::vector<sRivalData>::iterator it = doc->m_gameResultList.begin(); it != doc->m_gameResultList.end(); ++it) {
		m_pResultRanking->AddItem(&*it);
	}
	if (m_pResultRanking->GetScrollBar()) {
		m_pResultRanking->GetScrollBar()->SetGuideVisible(true);
	}
	if (doc->m_roomType != 10) {
		m_pResultRanking->SortItem(RankUserCompare);
	} else {
		m_pResultRanking->SortItem(ApproachRankUserCompare);
	}
	m_p30sRedTeamRankBitmap = g_pFresh->RegisterBitmap("30in_team_redteam_rank");
	m_p30sBlueTeamRankBitmap = g_pFresh->RegisterBitmap("30in_team_blueteam_rank");
	m_p30sRankBitmap = g_pFresh->RegisterBitmap("30in_rank");
	m_pRankGoldBitmap = g_pFresh->RegisterBitmap("rank_gold");
	m_pRankSilverBitmap = g_pFresh->RegisterBitmap("rank_silver");
	m_pRankBronzeBitmap = g_pFresh->RegisterBitmap("rank_bronze");
	m_pRankNoAwardBitmap = g_pFresh->RegisterBitmap("rank_noaward");
	m_resultUserInfoBlocked = 0;
}

// 0x513ac0 (KR CLobbyMain::OnGameRoomExtRes_RoomUserRankOwnerDraw): one
// row of the result board's user ranking. The row data is the port's KR
// sRivalData (CSharedDoc::m_gameResultList; JP +0x1042c, 0x1b8-byte
// records: +0x4 uid, +0x8 nick, +0x36 state, +0x128 pang, +0x14c score,
// +0x151 hole, +0x152 rank, +0x170 character type, +0x174 guild UID,
// +0x190 approach distance, +0x194 approach points). JP differences from
// KR: no team emblem branch (always the rank image), the medal only in a
// lounge-sized room (JP +0x1095e / +0x10937: the KR m_option[1] and
// m_userLimit > 99) outside the approach modes 10 and 11, the own row in
// red whatever the team, the hole text looked up.
void FrMassTypeRoomDlg::OnResultRankingOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem) {
		return;
	}
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	WRect rect(pItem->pos.x, pItem->pos.y, (float)m_pResultRanking->GetItemWidth(), (float)m_pResultRanking->GetItemHeight());
	if (!pGDI) {
		return;
	}
	sRivalData* pInfo = (sRivalData*)pItem->pData;
	if (m_pLobbyData->m_selectedUserId == pInfo->m_uid) {
		pGDI->Box(WRect(rect.x, rect.y + 2.0f, rect.w + 20.0f, rect.h - 2.0f), 0x999B9DFF, 0, 0.0f);
	}
	int oldSpace = pGDI->SetSpace(0);
	pGDI->SetTextStyle(0);
	pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
	const sRoomInfo& room = WSingleton<CSharedDoc>::Instance()->m_roomInfo;
	if (m_p30sRankBitmap) {
		pGDI->DrawTexture(m_p30sRankBitmap, WRect(rect.x + 5.0f, rect.y + 7.0f - 1.0f, (float)m_p30sRankBitmap->Width(), (float)m_p30sRankBitmap->Height()), 0xFFFFFFFF, 0);
	}
	if (pInfo->m_state == 3) {
		pGDI->Print(WPoint(pItem->pos.x + 12.0f, pItem->pos.y + 7.0f), 1, "-");
	} else if (room.m_gameType == 10 && pInfo->m_approachDistance == 0xFFFFFFFF) {
		pGDI->Print(WPoint(pItem->pos.x + 12.0f, pItem->pos.y + 7.0f), 1, "-");
	} else {
		pGDI->Print(WPoint(pItem->pos.x + 12.0f, pItem->pos.y + 7.0f), 1, "%d", pInfo->m_rank);
	}
	// JP CSharedDoc +0x1051e: the KR m_holeOrder.
	if (room.m_gameType != 10 && room.m_gameType != 11 && room.m_option[1] != 0 && room.m_userLimit > 99
		&& pInfo->m_state != 3 && pInfo->m_hole != WSingleton<CSharedDoc>::Instance()->m_holeOrder[0]) {
		int medal = JpLobbyMain_GetMedalIndex(pInfo->m_uid);
		if ((unsigned int)medal <= 2) {
			if (pInfo->m_characterType > 3) {
				medal = 3;
			}
			const Bitmap* medals[4] = { m_pRankGoldBitmap, m_pRankSilverBitmap, m_pRankBronzeBitmap, m_pRankNoAwardBitmap };
			const Bitmap* pMedal = medals[medal];
			pGDI->DrawTexture(pMedal, WRect(rect.x - 5.0f, rect.y + 7.0f - 2.0f, (float)pMedal->Width(), (float)pMedal->Height()), 0xFFFFFFFF, 0);
		}
	}
	// The JP lookup 0x9ab3e0 takes the guild UID and the record's +0x178
	// dword; the port's KR record carries the emblem name.
	if (pInfo->m_hasEmblem) {
		const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pInfo->m_emblemName);
		if (pEmblem) {
			pGDI->DrawTexture(pEmblem, WRect(pItem->pos.x + 24.0f, pItem->pos.y + 1.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
		}
	}
	// JP CSharedDoc +0x816 for a gallery player (+0x812 bit 1), else +0x81a.
	if (pInfo->m_uid == MyGuid(true)) {
		pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	} else if (pInfo->m_state == 3) {
		pGDI->SetTextColor(0xFF808080, 0xFFFFFFFF);
	} else {
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	}
	if (!WSingleton<CProjectG>::Instance()->HidePrivacy()) {
		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 47.0f, pItem->pos.y + 7.0f), 0, pInfo->m_nickname, 80.0f, 0xFFFFFFFF);
	}
	if (room.m_gameType == 10) {
		if (pInfo->m_approachPoint == -1) {
			pGDI->Print(WPoint(pItem->pos.x + 142.0f, pItem->pos.y + 7.0f), 2, "Out");
			pGDI->Print(WPoint(pItem->pos.x + 188.0f, pItem->pos.y + 7.0f), 2, "-");
		} else {
			if (pInfo->m_approachDistance == 0xFFFFFFFF) {
				pGDI->Print(WPoint(pItem->pos.x + 142.0f, pItem->pos.y + 7.0f), 2, "Out");
			} else {
				pGDI->Print(WPoint(pItem->pos.x + 142.0f, pItem->pos.y + 7.0f), 2, "%d.%dy", pInfo->m_approachDistance / 10, pInfo->m_approachDistance % 10);
			}
			pGDI->Print(WPoint(pItem->pos.x + 207.0f, pItem->pos.y + 7.0f), 2, "%d.%03d", pInfo->m_approachPoint / 1000, pInfo->m_approachPoint % 1000);
		}
	} else {
		pGDI->Print(WPoint(pItem->pos.x + 147.0f, pItem->pos.y + 7.0f), 2, "%d", pInfo->m_score);
		pGDI->Print(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 7.0f), 2, "%I64d", pInfo->m_pang);
		if (pInfo->m_state == 2 || GetHoleIndex(pInfo->m_hole) == 19) {
			pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 7.0f), 2, "End");
		} else if (pInfo->m_state == 3) {
			pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 7.0f), 2, "Out");
		} else {
			// 0xd06fd4: "%d hole".
			pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 7.0f), 2, Jp_LookupText("%d\xC8\xA6"), GetHoleIndex(pInfo->m_hole));
		}
	}
	pGDI->SetSpace(oldSpace);
}

// 0x5140c0 (KR CLobbyMain::OnGameRoomExtRes_RoomGuildRankInit): the result
// board's guild ranking, guild rooms only (JP CSharedDoc +0x10594, the KR
// m_roomType): one row per guild match pairing (JP +0x10450, 9-byte
// records: the KR m_vector4524), the two guilds' emblems (JP 0x9ab3e0 with
// the room's guild UIDs +0x1095f / +0x10963 and words +0x1097f / +0x10981;
// the port's KR room info carries the emblem names) and the rank image.
void FrMassTypeRoomDlg::OnResultGuildRankingInit(PG_MSGARG param) {
	m_pResultGuildRanking = DYNAMIC_CAST(FrListBox, param);
	if (!m_pResultGuildRanking) {
		return;
	}
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if (doc->m_roomType != 6) {
		m_pResultGuildRanking->SetVisible(false);
		return;
	}
	m_pResultGuildRanking->UseRightButton(true);
	for (std::vector<sGuildMatchup>::iterator it = doc->m_vector4524.begin(); it != doc->m_vector4524.end(); ++it) {
		m_pResultGuildRanking->AddItem(&*it);
	}
	if (m_pResultGuildRanking->GetScrollBar()) {
		m_pResultGuildRanking->GetScrollBar()->SetGuideVisible(true);
	}
	m_p30sRedTeamRankBitmap = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(doc->m_roomInfo.m_guild.m_guildMark[0]);
	m_p30sBlueTeamRankBitmap = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(doc->m_roomInfo.m_guild.m_guildMark[1]);
	m_p30sRankBitmap = g_pFresh->RegisterBitmap("30in_rank_02");
}

// 0x514340
FrGPTypeRoomDlg::FrGPTypeRoomDlg(void) {
	LoadGrandPrixRoomUi();
}

// 0x5143b0: one row of the GP room's user list; On30sUserOwnerDraw's
// layout (same JP record mapping) with these differences: the situation
// mark is 0 (waiting) or 2, never in team colours; a GP AI player (the JP
// record's byte +0x159) gets no level icon, and its gender icon is the
// FrGameRoomDlg bitmap +0x3c0 when that is loaded; the gender icon is drawn
// only when present.
void FrGPTypeRoomDlg::OnGPUserOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem) {
		return;
	}
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI) {
		return;
	}
	sSlotInfo* pSlot = (sSlotInfo*)pItem->pData;
	if (IsLocalContent((localContentType_t)0x55) && pSlot && pSlot->m_flags151.flag) {
		pGDI->DrawTexture(m_pInviteBitmap, WRect(pItem->pos.x, pItem->pos.y, (float)m_pInviteBitmap->Width(), (float)m_pInviteBitmap->Height()), 0xFFFFFFFF, 0);
		return;
	}
	pGDI->DrawTexture(m_p30sNoPlayerBase, WRect(pItem->pos.x, pItem->pos.y, (float)m_p30sPlayerBase->Width(), (float)m_p30sPlayerBase->Height()), 0xFFFFFFFF, 0);
	if (pItem->selected) {
		pGDI->Box(WRect(pItem->pos.x, pItem->pos.y, (float)m_pUserList->GetItemWidth() + 15.0, (float)m_pUserList->GetItemHeight()), 0x7F265785, 0, 0.0f);
	}
	if (!pSlot) {
		return;
	}
	WRect rect(0.0f, 0.0f, 0.0f, 0.0f);
	int situ;
	if (!pSlot->m_flags55.flag1 && !(pSlot->m_attribute & 0x18)) {
		situ = 0;
	} else {
		situ = 2;
	}
	const Bitmap* pSitu = m_p30sSituBitmap[situ];
	rect = WRect(pItem->pos.x + 10.0, pItem->pos.y + 5.0, (float)pSitu->Width(), (float)pSitu->Height());
	pGDI->DrawTexture(pSitu, rect, 0xFFFFFFFF, 0);
	const Bitmap* pGender;
	if (pSlot->m_flags57.flag0) {
		pGender = m_pGenderIcon[8 + pSlot->m_attributeBits.userFlag % 2];
	} else if (pSlot->m_flags55.flag0) {
		pGender = m_pGenderIcon[6 + pSlot->m_attributeBits.userFlag % 2];
	} else {
		pGender = m_pGenderIcon[pSlot->m_attributeBits.userFlag];
	}
	// Not written: when JpSlotInfo_IsGrandPrixAi(*pSlot) and the
	// FrGameRoomDlg bitmap +0x3c0 (kept UNKNOWN in JpGameRoomDlg.h: REQUEST)
	// is set, the original draws that bitmap instead.
	if (pGender) {
		rect = WRect(pItem->pos.x + 30.0, pItem->pos.y, (float)pGender->Width(), (float)pGender->Height());
		pGDI->DrawTexture(pGender, rect, 0xFFFFFFFF, 0);
	}
	if (m_pDiveBitmap && (pSlot->m_attribute & 4)) {
		pGDI->DrawTexture(m_pDiveBitmap, WRect(pItem->pos.x + 34.0, pItem->pos.y + 2.0, (float)m_pDiveBitmap->Width(), (float)m_pDiveBitmap->Height()), 0xFFFFFFFF, 0);
	}
	const char* levelName;
	if (pSlot->m_titleTypeId) {
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pSlot->m_titleTypeId);
		if (pSkin) {
			levelName = pSkin->m_icon;
		} else {
			levelName = MakeStr("level_%03d", pSlot->m_level + 1);
		}
	} else {
		levelName = MakeStr("level_%03d", pSlot->m_level + 1);
	}
	const Bitmap* pLevel = g_pFresh->GetBitmap(levelName);
	if (pSlot->m_identity & 0x8000) {
		pLevel = g_pFresh->GetManager()->GetBitmap("LEVELS", "gp_host");
	}
	if (!JpSlotInfo_IsGrandPrixAi(*pSlot) && pLevel) {
		pGDI->DrawTexture(pLevel, WRect(pItem->pos.x + 80.0 - float2int(pLevel->Width() * 0.5), pItem->pos.y + 13.0 - float2int(pLevel->Height() * 0.5), (float)pLevel->Width(), (float)pLevel->Height()), 0xFFFFFFFF, 0);
	}
	if (pSlot->m_guildUID && !pSlot->IsIdentity(0x14)) {
		const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pSlot->m_emblemName);
		if (pEmblem) {
			pGDI->DrawTexture(pEmblem, WRect(pItem->pos.x + 115.0, pItem->pos.y + 7.0 - 6.0, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
		}
	}
	unsigned long color = ((unsigned char)pSlot->m_identity & 0x14) ? 0x50FFFFFF : 0xFFFFFFFF;
	if (pItem->underCursor) {
		pGDI->SetTextColor(0xFFFFFFFF, 0xFF124371);
		pGDI->SetTextStyle(2);
	} else {
		pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
		pGDI->SetTextStyle(0);
	}
	if (!WSingleton<CProjectG>::Instance()->HidePrivacy()) {
		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 140.0, pItem->pos.y + 7.0), 0, pSlot->m_nickname, -1.0f, color);
	}
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if ((!doc->m_gameOver || AdminMode())
		&& doc->m_userInfo.guid == m_pLobbyData->m_hostUserId
		&& !(pSlot->m_attribute & 0x18)
		&& doc->m_roomInfo.m_gameType != 6) {
		pGDI->DrawTexture(m_pKickBitmap, WRect(pItem->pos.x + 226.0, pItem->pos.y + 7.0 - 2.0, (float)m_pKickBitmap->Width(), (float)m_pKickBitmap->Height()), pItem->underCursor ? 0xFFFFFFFF : 0x80FFFFFF, 0);
	}
}

// 0x514a80: the GP waiting room's timers ("grandprix_ai_player.lua",
// table "ai_player_waiting_room_slot_add_table"; JP 0xc1ff80 the script
// manager, 0xc1feb0 CreateScript, 0xc20530 RunFile, 0xc20240 GetTable,
// 0x470f20 the float field read, 0xc1f430 ReleaseScript).
void FrGPTypeRoomDlg::OnGameStartWaitingInit(void) {
	m_pLobbyData->m_userMenuBlocked = false;
	m_gpWaiting = false;
	m_gpWaitTick0 = 0;
	m_gpWaitTick1 = 0;
	lua_system::lua_script* script = lua_system::lua_script_manger::GetInstance()->CreateScript(false);
	if (script) {
		script->RunFile("grandprix_ai_player.lua");
		lua_system::lua_table_helper table = script->GetTable("ai_player_waiting_room_slot_add_table");
		m_gpGameStartWaitingTimeSec = table.GetValueByName<float>("game_start_waiting_time_sec");
		m_gpAddSlotTimeSec = table.GetValueByName<float>("add_slot_time_sec");
		lua_system::lua_script_manger::GetInstance()->ReleaseScript(script);
	}
}

// 0x515000
IObject* FrMassTypeRoomDlgMakeInstance(void) {
	return new FrMassTypeRoomDlg;
}

// 0x515090 (KR CLobbyMain::OnGameRoomExt_RoomUserInit by the pairing):
// the 30-player user list and its images, the picked user cleared, then a
// notice when the own ticket report count (type 0x1a000042 in the JP
// CSharedDoc +0x375c, the KR m_itemInfoList) has reached 10.
void FrMassTypeRoomDlg::On30sUserInit(PG_MSGARG param) {
	m_pUserList = DYNAMIC_CAST(FrListBox, param);
	if (m_pUserList == 0) {
		return;
	}
	JpFrListBox_SetRoomUserListFlags(m_pUserList);
	// The +0x318 slot: "30_player_wait" in the mass room.
	m_pInviteBitmap = m_pUserList->GetBitmap("30_player_wait");
	m_pGenderIcon[0] = m_pUserList->GetBitmap("i_male");
	m_pGenderIcon[1] = m_pUserList->GetBitmap("i_female");
	m_pGenderIcon[2] = m_pUserList->GetBitmap("i_male_02");
	m_pGenderIcon[3] = m_pUserList->GetBitmap("i_female_02");
	m_pGenderIcon[4] = m_pUserList->GetBitmap("i_male_03");
	m_pGenderIcon[5] = m_pUserList->GetBitmap("i_female_03");
	m_pGenderIcon[6] = m_pUserList->GetBitmap("i_male_manner");
	m_pGenderIcon[7] = m_pUserList->GetBitmap("i_female_manner");
	m_pGenderIcon[8] = m_pUserList->GetBitmap("i_male_angel");
	m_pGenderIcon[9] = m_pUserList->GetBitmap("i_female_angel");
	m_p30sSituBitmap[0] = m_pUserList->GetBitmap("30in_situ03");
	m_p30sSituBitmap[1] = m_pUserList->GetBitmap("30in_situ01");
	m_p30sSituBitmap[2] = m_pUserList->GetBitmap("30in_situ02");
	m_p30sSituBitmap[3] = m_pUserList->GetBitmap("30in_situ_red_cp");
	m_p30sSituBitmap[4] = m_pUserList->GetBitmap("30in_situ_red");
	m_p30sSituBitmap[5] = m_pUserList->GetBitmap("30in_situ_blue_cp");
	m_p30sSituBitmap[6] = m_pUserList->GetBitmap("30in_situ_blue");
	m_pDiveBitmap = m_pUserList->GetBitmap("dive");
	m_p30sNoPlayerBase = g_pFresh->RegisterBitmap("30_noplayer_base");
	m_p30sPlayerBase = g_pFresh->RegisterBitmap("30_player_base");
	m_pKickBitmap = g_pFresh->RegisterBitmap("kick");
	m_pLobbyData->m_selectedUserId = 0xffffffff;
	m_pLobbyData->m_selectedUserNo = 0xffffffff;
	if (m_addOwnPlayer) {
		MakeRoomUserList();
	}
	std::list<sItemInfo>& items = WSingleton<CSharedDoc>::Instance()->m_itemInfoList;
	unsigned int ticketReports = 0;
	for (std::list<sItemInfo>::iterator it = items.begin(); it != items.end(); ++it) {
		sItemInfo item = *it;
		if (item.m_typeId == 0x1a000042) {
			ticketReports++;
		}
	}
	if (ticketReports >= 10) {
		CTaskManager* manager = CTaskManager::Instance();
		// 0xd070c8: "the unchecked ticket reports exceed 10 ... please check
		// the reports you hold" (Korean key).
		(!manager ? 0 : manager->GetCurrentTask())->SendMsgToMainActor(MsgObject(0, 0x23,
			(intptr_t)Jp_LookupText("\xB9\xCC\xC8\xAE\xC0\xCE \xC6\xBC\xC5\xB0\xB8\xAE\xC6\xF7\xC6\xAE\xC0\xC7 "
				"\xBA\xB8\xC0\xAF\xB7\xAE\xC0\xCC 10\xB0\xB3\xB8\xA6 \xC3\xCA\xB0\xFA \xC7\xCF\xBF\xB4\xC0\xB8\xB9\xC7\xB7\xCE "
				"\xB4\xF5\xC0\xCC\xBB\xF3 \xBB\xE7\xBF\xEB\xC7\xCF\xBD\xC7 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9 "
				"\xC6\xBC\xC5\xB0\xB8\xAE\xC6\xF7\xC6\xAE\xB8\xA6 \xBB\xE7\xBF\xEB\xC7\xCF\xBD\xC3\xB1\xE2 \xC0\xA7\xC7\xD8\xBC\xAD\xB4\xC2 "
				"\xBA\xB8\xC0\xAF\xC7\xCF\xB0\xED \xC0\xD6\xB4\xC2 \xB8\xAE\xC6\xF7\xC6\xAE\xB8\xA6 \xC8\xAE\xC0\xCE\xC7\xCF\xBF\xA9 "
				"\xC1\xD6\xBD\xC3\xB1\xE2 \xB9\xD9\xB6\xF8\xB4\xCF\xB4\xD9."),
			0, 0, 0, 0));
	}
}

// 0x5153d0
void FrMassTypeRoomDlg::OnResultGuildRankingRBtnUp(void) {
	OnResultGuildRankingLBtnDown();
	OnResultScoreLBtnDown();
}

// 0x5153e0
IObject* FrGPTypeRoomDlgMakeInstance(void) {
	return new FrGPTypeRoomDlg;
}

// 0x515470: every add_slot_time_sec the next room player (by its waiting
// slot) takes the first empty row of the user list; past
// game_start_waiting_time_sec the main actor gets message 0x86 and the
// timers restart (OnGameStartWaitingInit).
void FrGPTypeRoomDlg::OnGameStartWaitingOwnerDraw(void) {
	if (!m_pLobbyData->m_userMenuBlocked) {
		return;
	}
	uint32_t now = Jp_GetTickF022d4();
	uint32_t waited = now - m_gpWaitTick1;
	uint32_t waitLimit = (uint32_t)(int)(m_gpGameStartWaitingTimeSec * 1000.0);
	uint32_t sinceSlot = now - m_gpWaitTick0;
	uint32_t slotPeriod = (uint32_t)(int)(1000.0 * m_gpAddSlotTimeSec);
	if (sinceSlot >= slotPeriod && m_gpWaiting) {
		int nextSlot = m_pUserList->GetCurrentItemSize(false) + 1;
		std::list<sSlotInfo>& players = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList;
		for (std::list<sSlotInfo>::iterator it = players.begin(); it != players.end(); ++it) {
			if (JpSlotInfo_GetWaitingSlot(*it) == nextSlot) {
				FrListBox::ITEM_LIST& rows = m_pUserList->m_itemList;
				for (FrListBox::ITEM_LIST::iterator row = rows.begin(); row != rows.end(); ++row) {
					if ((*row)->pData == 0) {
						m_pUserList->_DelItem(row);
						m_pUserList->AddItem(&*it);
						m_pUserList->SortItem(JpRoomUserCompare);
						break;
					}
				}
				m_gpWaitTick0 = Jp_GetTickF022d4();
			}
		}
	}
	if (waited >= waitLimit) {
		CTaskManager* manager = CTaskManager::Instance();
		(!manager ? 0 : manager->GetCurrentTask())->SendMsgToMainActor(MsgObject(0, 0x86, 0, 0, 0, 0, 0));
		OnGameStartWaitingInit();
	}
}

// 0x515810 (KR CLobbyMain::OnGameRoomExt_UserInfoLBtnUp): the picked
// player's user info, or the button disabled when none is picked.
void FrMassTypeRoomDlg::OnResultUserInfoLBtnDown(void) {
	if (m_resultUserInfoBlocked) {
		return;
	}
	if (m_pLobbyData->m_selectedUserId != 0xffffffff) {
		if (JpUserInfo_IsCreated()) {
			JpRoomUser_OpenUserInfo(m_pLobbyData->m_selectedUserId);
		}
	} else if (m_pResultUserInfo) {
		m_pResultUserInfo->Enable(false);
	}
}

// 0x5158c0 (KR CLobbyMain::OnGameRoomExtRes_RoomUserRankRBtnUp): picks
// the player like OnResultRankingLBtnDown, then opens its user info unless
// it left (state 3).
void FrMassTypeRoomDlg::OnResultRankingRBtnUp(void) {
	FrListItem* item = m_pResultRanking->GetSelected();
	if (item) {
		sRivalData* info = (sRivalData*)item->pData;
		if (info) {
			m_pLobbyData->m_selectedUserId = info->m_uid;
			m_pLobbyData->m_selectedUserNo = info->m_id;
			if (m_pScoreDlg) {
				m_pScoreDlg->SetPlayer(m_pLobbyData->m_selectedUserId);
			} else if (m_pResultScore) {
				m_pResultScore->Enable(true);
			}
			if (JpUserInfo_IsCreated() && info->m_state != 3) {
				JpRoomUser_OpenUserInfo(info->m_uid);
			}
		}
	}
}

// 0x5160a0
void FrMassTypeRoomDlg::SetupStateWaiting(void) {
	// Not written: when the FrGameRoomDlg byte +0x18d is set (the
	// constructor sets it; kept UNKNOWN in JpGameRoomDlg.h: REQUEST), the
	// original builds the own room player record (0x2dc bytes: 0x8b2940 /
	// 0x500230 constructors, m_pPet0's +0x194 dword, CSharedDoc::
	// GetMyPartTidList, the JP CSharedDoc map +0x3704 entry of +0xa2a, the
	// +0x832 bits 5 and 6, the +0x8cb dword), stores it into the JP
	// CSharedDoc map +0x3890 under the own key +0x81a and calls
	// AddPlayer(+0x81a). JP game state with no port counterpart yet.
	// JP CSharedDoc +0x10950: the KR m_roomInfo.m_course.
	SetCourseImages(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course);
	UpdateTitle(NULL);
}

// 0x5162c0
bool FrMassTypeRoomDlg::OnInit(void) {
	if (m_roomTab == 0) {
		m_pGroup30s->SetVisible(true);
	}
	if (m_roomState >= 4 && m_roomState <= 5) {
		SetupStateWaiting();
		if (m_pFrameBg) {
			m_pFrameBg->Open("gameroom_background.tga");
		}
	} else if (m_roomState == 8) {
		m_pGroupResult->SetVisible(true);
		SetupStateResult();
		if (m_pFrameBg) {
			m_pFrameBg->Open("gameroom_background.tga");
		}
	}
	m_pLobbyData->m_userMenuBlocked = false;
	// Not written: the original also sets m_procTimer to 1.0f when the dword
	// FrGameRoomDlg +0x168 (zeroed by the constructor, role not traced, kept
	// UNKNOWN) is 0.
	if (m_pBackground) {
		m_pBackground->SetVisible(false);
	}
	JpFrWnd_RaiseFlagF1aac8();
	if (RFLobbyDesktop::s_instance) {
		CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0x59, 10, 0, 0, 0);
	}
	// FrForm's (0xbe6570), not FrGameRoomDlg's.
	return FrForm::OnInit();
}

// 0x516390 (KR CLobbyMain::OnGameRoomExt_GuildInviteLBtnUp): in the guild
// match (JP CSharedDoc +0x1094c, the KR m_roomInfo.m_gameType, 6) counts
// the lobby's players per guild (JP +0x109d8, the KR m_userMap; not the own
// guild, JP +0x82a; only players in no room), invites every player of the
// guilds with at least the room's user count (JP +0x10938) of them, and
// says so in a "notify" form. Otherwise the base's user list. JP
// differences from KR: the invite packet is 0xba (local content 0x55), no
// invite delay, the second notify opens with flag 0x800.
void FrMassTypeRoomDlg::OnInviteLBtnUp(void) {
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if (doc->m_roomInfo.m_gameType != 6) {
		FrGameRoomDlg::OnInviteLBtnUp();
		return;
	}
	const std::map<uint32_t, sBriefUserInfo>& users = JpSharedDoc_GetLobbyUserMap();
	std::map<uint32_t, unsigned int> guildUserCount;
	bool found = false;
	std::map<uint32_t, sBriefUserInfo>::const_iterator it;
	for (it = users.begin(); it != users.end(); ++it) {
		if (it->second.guildUID != 0 && doc->m_userInfo.guildUID != it->second.guildUID
			&& !it->second.unknownBit && it->second.roomNumber == 0xFFFF) {
			guildUserCount[it->second.guildUID] += 1;
		}
	}
	std::vector<uint32_t> guilds;
	for (std::map<uint32_t, unsigned int>::iterator git = guildUserCount.begin(); git != guildUserCount.end(); ++git) {
		if (doc->m_roomInfo.m_userCount <= git->second) {
			guilds.push_back(git->first);
			found = true;
		}
	}
	if (!found) {
		// 0xd072ec: "There is no guild to invite." (the KR source passes the
		// end iterator's nick to the format, which does not use it).
		m_pNotifyForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		m_pNotifyForm->SetMessage(MakeStr(Jp_LookupText("\xC3\xCA\xB4\xEB\xC7\xD2 \xBB\xF3\xB4\xEB \xB1\xE6\xB5\xE5\xB0\xA1 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9."), it->second.nick), false);
		m_pNotifyForm->Open((FRESH_RESULT_CALLBACK)0, 0);
		FrButton* pCancel = DYNAMIC_CAST(FrButton, m_pNotifyForm->FindChildByName("cancel"));
		(void)pCancel;
		return;
	}
	unsigned int count = guilds.size();
	for (it = users.begin(); it != users.end(); ++it) {
		for (unsigned int i = 0; i < count; i++) {
			if (it->second.guildUID == guilds[i]) {
				if (IsLocalContent((localContentType_t)0x55)) {
					WSendPacket packet(0xBA);
					packet.EncodeStr(it->second.nick);
					packet.Encode4(it->second.id);
					packet.Send(SEND_TO_DEFAULT);
				} else {
					WSendPacket packet(0x29);
					packet.Encode4(it->second.id);
					packet.Send(SEND_TO_DEFAULT);
				}
			}
		}
	}
	// 0xd072d0: "The invitation message was sent".
	m_pNotifyForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
	m_pNotifyForm->SetMessage(MakeStr(Jp_LookupText("\xC3\xCA\xB4\xEB \xB8\xDE\xBD\xC3\xC1\xF6\xB8\xA6 \xBA\xB8\xB3\xC2\xBD\xC0\xB4\xCF\xB4\xD9"), it->second.nick), false);
	m_pNotifyForm->Open((FRESH_RESULT_CALLBACK)0, 0x800);
	FrButton* pCancel = DYNAMIC_CAST(FrButton, m_pNotifyForm->FindChildByName("cancel"));
	if (pCancel) {
		pCancel->SetVisible(false);
	}
}

// 0x5169b0 (KR CLobbyMain::OnGameRoomExtRes_RoomGuildRankOwnerDraw): one
// guild match pairing (KR sGuildMatchup; JP 9-byte record: +0 kind, +1 /
// +5 the two players' uids) of the result board: the pairing's rank, then
// both players (their result records by CSharedDoc::GetIndex 0x50ea00 in
// JP +0x1042c, the KR m_gameResultList; JP +0x188 the KR
// m_uniteGuildIndex). JP differences from KR: the rank and hole texts are
// looked up (0xd07318, 0xd06fd4).
void FrMassTypeRoomDlg::OnResultGuildRankingOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem) {
		return;
	}
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	WRect rect(pItem->pos.x + 2.0f, pItem->pos.y, m_pResultGuildRanking->GetItemWidth() - 10.0f, (float)m_pResultGuildRanking->GetItemHeight());
	if (!pGDI) {
		return;
	}
	int oldSpace = pGDI->SetSpace(0);
	sGuildMatchup* pMatchup = (sGuildMatchup*)pItem->pData;
	if (m_pLobbyData->m_selectedUserId == pMatchup->m_guidA || m_pLobbyData->m_selectedUserId == pMatchup->m_guidB) {
		pGDI->Box(WRect(rect.x, rect.y + 2.0f, rect.w + 20.0f, rect.h - 2.0f), 0x999B9DFF, 0, 0.0f);
	}
	pGDI->SetTextStyle(0);
	pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
	if (m_p30sRankBitmap) {
		pGDI->DrawTexture(m_p30sRankBitmap, WRect(rect.x - 3.0f, rect.y + 6.0f, (float)m_p30sRankBitmap->Width(), (float)m_p30sRankBitmap->Height()), 0xFFFFFFFF, 0);
		// "%d group".
		pGDI->Print(WPoint(pItem->pos.x + 13.0f, pItem->pos.y + 10.0f), 1, Jp_LookupText("%d \xC1\xB6"), (int)pMatchup->m_kind);
	}
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	sRivalData* pInfo = &doc->m_gameResultList[doc->GetIndex(pMatchup->m_guidA)];
	if (m_p30sRedTeamRankBitmap) {
		pGDI->DrawTexture(m_p30sRedTeamRankBitmap, WRect(pItem->pos.x + 34.0f, pItem->pos.y + 3.0f, (float)(m_p30sRedTeamRankBitmap->Width() / 2), (float)(m_p30sRedTeamRankBitmap->Height() / 2)), 0xFFFFFFFF, 0);
	}
	// JP CSharedDoc +0x816 when +0x812 bit 0 is set, else +0x81a (the KR
	// MyGuid(true) tests identity bit 1).
	uint32_t ownUid = doc->m_userInfo.IsIdentity(1) ? doc->m_userInfo.galleryGuid : doc->m_userInfo.guid;
	if (pInfo->m_uid == ownUid) {
		pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	} else if (pInfo->m_state == 3) {
		pGDI->SetTextColor(0xFF808080, 0xFFFFFFFF);
	} else {
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	}
	if (!WSingleton<CProjectG>::Instance()->HidePrivacy()) {
		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 47.0f, pItem->pos.y + 3.0f), 0, pInfo->m_nickname, 80.0f, 0xFFFFFFFF);
	}
	pGDI->Print(WPoint(pItem->pos.x + 147.0f, pItem->pos.y + 3.0f), 2, "%d", pInfo->m_uniteGuildIndex);
	pGDI->Print(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 3.0f), 2, "%I64d", pInfo->m_pang);
	if (pInfo->m_state == 2 || GetHoleIndex(pInfo->m_hole) == 19) {
		pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 3.0f), 2, "End");
	} else if (pInfo->m_state == 3) {
		pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 3.0f), 2, "Out");
	} else {
		pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 3.0f), 2, Jp_LookupText("%d\xC8\xA6"), GetHoleIndex(pInfo->m_hole));
	}
	pInfo = &doc->m_gameResultList[doc->GetIndex(pMatchup->m_guidB)];
	if (m_p30sBlueTeamRankBitmap) {
		pGDI->DrawTexture(m_p30sBlueTeamRankBitmap, WRect(pItem->pos.x + 34.0f, pItem->pos.y + 21.0f, (float)(m_p30sBlueTeamRankBitmap->Width() / 2), (float)(m_p30sBlueTeamRankBitmap->Height() / 2)), 0xFFFFFFFF, 0);
	}
	if (pInfo->m_uid == ownUid) {
		pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	} else if (pInfo->m_state == 3) {
		pGDI->SetTextColor(0xFF808080, 0xFFFFFFFF);
	} else {
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	}
	if (!WSingleton<CProjectG>::Instance()->HidePrivacy()) {
		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 47.0f, pItem->pos.y + 20.0f), 0, pInfo->m_nickname, 80.0f, 0xFFFFFFFF);
	}
	pGDI->Print(WPoint(pItem->pos.x + 147.0f, pItem->pos.y + 20.0f), 2, "%d", pInfo->m_uniteGuildIndex);
	pGDI->Print(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 20.0f), 2, "%I64d", pInfo->m_pang);
	if (pInfo->m_state == 2 || GetHoleIndex(pInfo->m_hole) == 19) {
		pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 20.0f), 2, "End");
	} else if (pInfo->m_state == 3) {
		pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 20.0f), 2, "Out");
	} else {
		pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 20.0f), 2, Jp_LookupText("%d\xC8\xA6"), GetHoleIndex(pInfo->m_hole));
	}
	pGDI->SetSpace(oldSpace);
}

// 0x516fc0
bool FrGPTypeRoomDlg::OnInit(void) {
	m_pGroupGP->SetVisible(true);
	SetupGrandPrixCourse();
	// The constructor's LoadGrandPrixRoomUi created the object.
	m_pGrandPrixRoom->SetLobbyValue(m_lobbyValue);
	if (m_pFrameBg) {
		m_pFrameBg->Open("gameroom_background_gp.tga");
	}
	if (m_pInvite) {
		m_pInvite->SetVisible(false);
	}
	if (m_pChangeTeam) {
		m_pChangeTeam->SetVisible(false);
	}
	if (m_pModify) {
		m_pModify->SetVisible(false);
	}
	// The map and its combo, the GP detail and info rows move up (15, 38,
	// 15, 40 pixels).
	WRect rect = m_pMap->GetRect();
	rect.y -= 15.0f;
	m_pMap->SetRect(rect);
	rect = m_pMapCombo->GetRect();
	rect.y -= 38.0f;
	m_pMapCombo->SetRect(rect);
	rect = m_pGPDetail->GetRect();
	rect.y -= 15.0f;
	m_pGPDetail->SetRect(rect);
	rect = m_pGrandPrixInfo->GetRect();
	rect.y -= 40.0f;
	m_pGrandPrixInfo->SetRect(rect);
	return FrMassTypeRoomDlg::OnInit();
}

// 0x521f70
const sFRESH_MSGMAP* FrSingleTypeDlg::GetMessageMap(void) const {
	return &FrSingleTypeDlg::_MsgMap;
}

sFRESH_MSGMAP FrSingleTypeDlg::_MsgMap = { &FrGameRoomDlg::_MsgMap, &FrSingleTypeDlg::_MsgEntries[0] };

// 0xe0a9a8 (the handlers are stored by the static init 0xccdae0)
sFRESH_ENTRY FrSingleTypeDlg::_MsgEntries[] = {
	{ "mode_title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSingleTypeDlg::OnModeTitleInit },
	{ "course_ctrl", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSingleTypeDlg::OnCourseCtrlInit },
	{ "course_ctrl.bg_course", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSingleTypeDlg::OnBgCourseInit },
	{ "course_ctrl.bg_course", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrSingleTypeDlg::OnBgCourseOwnerDraw },
	{ "course_ctrl.fix_hole", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSingleTypeDlg::OnFixHoleInit },
	{ "course_ctrl.fix_hole", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrSingleTypeDlg::OnFixHoleLBtnUp },
	{ "course_ctrl.fix_hole", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrSingleTypeDlg::OnFixHoleOwnerDraw },
	{ "course_ctrl.fix_holecup_pos", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSingleTypeDlg::OnFixHolecupPosInit },
	{ "course_ctrl.fix_holecup_pos", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrSingleTypeDlg::OnFixHolecupPosLBtnUp },
	{ "chipin_ctrl", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSingleTypeDlg::OnChipinCtrlInit },
	{ "chipin_ctrl.slope", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSingleTypeDlg::OnChipinSlopeInit },
	{ "chipin_ctrl.slope", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrSingleTypeDlg::OnChipinSlopeLBtnDown },
	{ "chipin_ctrl.size", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSingleTypeDlg::OnChipinSizeInit },
	{ "chipin_ctrl.size", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrSingleTypeDlg::OnChipinSizeLBtnDown },
	{ "chipin_ctrl.ticket", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSingleTypeDlg::OnChipinTicketInit },
	{ "chipin_ctrl.ticket", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrSingleTypeDlg::OnChipinTicketOwnerDraw },
	{ "assist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSingleTypeDlg::OnAssistInit },
	{ "assist", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrSingleTypeDlg::OnAssistLBtnUp },
	{ "assist_tip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSingleTypeDlg::OnAssistTipInit },
	{ "course_ctrl.modify", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrSingleTypeDlg::OnModifyInit },
	{ "course_ctrl.modify", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrSingleTypeDlg::OnModifyLBtnUp },
	{ "close_single", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrSingleTypeDlg::OnCloseSingleLBtnUp },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x521f80
void FrSingleTypeDlg::OnProc(const float dt) {
	FrGameRoomDlg::OnProc(dt);
	if (g_input->GetDown("ESCAPE", true) == 1) {
		Close(FrOK, false);
		g_input->ExclusiveGetDownUseDone("ESCAPE");
	}
}

// 0x521fd0
void FrSingleTypeDlg::InitSingleRoom(void) {
	InitRoom();
	m_pNumFont = 0;
	memset(m_chipinTicketText, 0, sizeof(m_chipinTicketText));
	if (m_pNumFont == 0) {
		m_pNumFont = new JpSingleNumFont;
	}
	if (m_pRoomOwned == 0) {
		m_pRoomOwned = new JpGameRoomOwned;
	}
	memset(m_chipinTicketText, 0, sizeof(m_chipinTicketText));
}

// 0x5220a0
void FrSingleTypeDlg::ReleaseSingleRoom(void) {
	if (m_pNumFont) {
		delete m_pNumFont;
		m_pNumFont = 0;
	}
	ReleaseRoom();
}

// 0x5220d0
void FrSingleTypeDlg::OnCloseSingleLBtnUp(void) {
	Close(FrOK, true);
}

// 0x522120
FrSingleTypeDlg::FrSingleTypeDlg(void) {
	InitSingleRoom();
}

// 0x5221c0
FrSingleTypeDlg::~FrSingleTypeDlg(void) {
	ReleaseSingleRoom();
}

// 0x5222d0
void FrSingleTypeDlg::OnModeTitleInit(PG_MSGARG param) {
	m_pModeTitle = DYNAMIC_CAST(FrArea, param);
}

// 0x5223b0
void FrSingleTypeDlg::OnCourseCtrlInit(PG_MSGARG param) {
	m_pCourseCtrl = DYNAMIC_CAST(FrGroupBox, param);
}

// 0x522420
void FrSingleTypeDlg::OnChipinCtrlInit(PG_MSGARG param) {
	m_pChipinCtrl = DYNAMIC_CAST(FrGroupBox, param);
}

// 0x522490
void FrSingleTypeDlg::OnBgCourseInit(PG_MSGARG param) {
	m_pBgCourse = DYNAMIC_CAST(FrArea, param);
}

// 0x522500
void FrSingleTypeDlg::OnFixHoleInit(PG_MSGARG param) {
	m_pFixHole = DYNAMIC_CAST(FrButton, param);
	if (m_pFixHole) {
		m_pFixHole->SetVisible(false);
	}
}

// 0x522580
void FrSingleTypeDlg::OnFixHolecupPosInit(PG_MSGARG param) {
	m_pFixHolecupPos = DYNAMIC_CAST(FrButton, param);
	if (m_pFixHolecupPos) {
		m_pFixHolecupPos->SetVisible(false);
	}
}

// The chip-in options live in the contents container 0xaa
// (CContentsDoc::GetContainer 0x9137e0 on 0xe47db4, inlined). When it is
// missing the original stores through an uninitialised local (the slot of
// the pushed this): the port skips the store.
static _gateway::_golfgame::CHoleInOneGameDoc* JpGetChipInDoc(void) {
	return static_cast<_gateway::_golfgame::CHoleInOneGameDoc*>(
		CContentsDoc::Instance()->GetContainer((localContentType_t)0xaa));
}

// 0x522600: "apply" (index 0, JP FrComboCtlEx +0x2f1 = KR m_byCurrentIdx)
// turns the slope on.
void FrSingleTypeDlg::OnChipinSlopeLBtnDown(void) {
	unsigned char index = m_pChipinSlope->GetCurrentIdx();
	_gateway::_golfgame::CHoleInOneGameDoc* doc = JpGetChipInDoc();
	if (doc) {
		doc->SetApplySlope(index == 0);
	}
}

// 0x522640
void FrSingleTypeDlg::OnChipinSizeInit(PG_MSGARG param) {
	m_pChipinSize = DYNAMIC_CAST(FrComboCtlEx, param);
}

// 0x5226b0: the size is the selection + 1 (1 normal .. 9).
void FrSingleTypeDlg::OnChipinSizeLBtnDown(void) {
	unsigned char index = m_pChipinSize->GetCurrentIdx();
	_gateway::_golfgame::CHoleInOneGameDoc* doc = JpGetChipInDoc();
	if (doc) {
		doc->SetChipInSize(index + 1);
	}
}

// 0x5226f0
void FrSingleTypeDlg::OnChipinTicketInit(PG_MSGARG param) {
	m_pChipinTicket = DYNAMIC_CAST(FrArea, param);
}

// 0x522760
void FrSingleTypeDlg::OnChipinTicketOwnerDraw(void) {
	if (m_pChipinTicket && m_pNumFont) {
		// JP FrWnd +0x34 / +0x38: the KR m_rect.x / .y.
		m_pNumFont->Print(m_pChipinTicket->GetRect().x + 31.0, m_pChipinTicket->GetRect().y + 35.0,
			m_chipinTicketText, TITLEFONT_ALIGN_CENTER, 0.0f, 1.0f);
	}
}

// 0x5228a0
IObject* FrSingleTypeDlgMakeInstance(void) {
	return new FrSingleTypeDlg;
}

// 0x522960
bool FrSingleTypeDlg::OnHoleSelectorResult(int ret, FrForm* form) {
	if (ret == FrOK) {
		FrHoleSelector* selector = DYNAMIC_CAST(FrHoleSelector, form);
		if (selector) {
			unsigned char hole = selector->GetHole();
			if (hole != JpSharedDoc_GetRoomHole()) {
				WSendPacket packet(GCT_REQUEST_ROOMINFO_CHANGED);
				packet.Encode2(0xFFFF);
				packet.Encode1(1);
				packet.Encode1(0xb);
				packet.Encode1(hole);
				packet.Send(SEND_TO_DEFAULT);
			}
		}
	}
	return true;
}

// The "course_ctrl.bg_course" owner draw: the room's hole count and time
// limit in minutes (right-aligned with the ticket font), then the hole
// mode's caption image "single_txt_front" / "back" / "random" / "shuffle"
// (JP table 0xe0aca8); mode 4 draws no caption.
static const char* s_holeModeNames[] = {
	"front",
	"back",
	"random",
	"shuffle"
};

// 0x522a40
void FrSingleTypeDlg::OnBgCourseOwnerDraw(void) {
	if (m_pBgCourse && m_pNumFont) {
		// JP CSharedDoc +0x1094b, +0x10955 and +0x1094f: room +0x47, +0x51
		// and +0x4b (the KR sRoomInfo offsets).
		const sRoomInfo& room = WSingleton<CSharedDoc>::Instance()->m_roomInfo;
		const WRect& rect = m_pBgCourse->GetRect();
		m_pNumFont->Print(rect.x + 74.0, rect.y + 8.0, MakeStr("%d", room.m_holeCount),
			TITLEFONT_ALIGN_RIGHT, 0.0f, 1.0f);
		m_pNumFont->Print(rect.x + 195.0, rect.y + 8.0, MakeStr("%d", room.m_value51 / 60000),
			TITLEFONT_ALIGN_RIGHT, 0.0f, 1.0f);
		unsigned char mode = room.m_value4B;
		if (mode == 4) {
			return;
		}
		FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
		if (gdi && mode < 4) {
			const Bitmap* bmp = g_pFresh->GetBitmap(MakeStr("single_txt_%s", s_holeModeNames[mode]));
			if (bmp) {
				WRect dest(rect.x + 96.0, rect.y + 37.0, (float)bmp->Width(), (float)bmp->Height());
				gdi->DrawTexture(bmp, dest, 0xffffffff, 0);
			}
		}
	}
}

// 0x522bf0
void FrSingleTypeDlg::OnFixHoleLBtnUp(void) {
	if (m_pFixHole) {
		FrHoleSelector* selector = static_cast<FrHoleSelector*>(
			CreateHoleSelector(g_pFresh->GetManager(), this, "hole_selector", 0));
		if (selector) {
			const WRect& rect = m_pFixHole->GetRect();
			WPoint pt(rect.x - 68.0, rect.y + (rect.h + 3.0));
			unsigned char hole = JpSharedDoc_GetRoomHole();
			selector->EnableDrag(false);
			// JP CSharedDoc +0x10950: the KR m_roomInfo.m_course.
			selector->SetCourse(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course, hole);
			selector->Open((FRESH_RESULT_CALLBACK)&FrSingleTypeDlg::OnHoleSelectorResult, pt, 0x89);
		}
	}
}

// 0x522cd0: the room's starting hole (up to 18) on the fix_hole button,
// centred, with the room font: "%d" below 10, else "!%d" (the font's '!'
// glyph; JP table 0xe0acb8) with the units digit.
void FrSingleTypeDlg::OnFixHoleOwnerDraw(void) {
	if (m_pFixHole == 0 || JpSharedDoc_GetRoomHole() > 0x12 || m_pRoomOwned == 0) {
		return;
	}
	const char* format = JpSharedDoc_GetRoomHole() < 10 ? "%d" : "!%d";
	const char* text = MakeStr(format, JpSharedDoc_GetRoomHole() % 10);
	const WRect& rect = m_pFixHole->GetRect();
	m_pRoomOwned->Print(rect.x + rect.w * 0.5, rect.y + 4.0, text, TITLEFONT_ALIGN_CENTER, 0.0f, 1.0f);
}

// Toggles the option and sends it as room-info field 0xc (6 off, 7 on).
// 0x522d90
void FrSingleTypeDlg::OnFixHolecupPosLBtnUp(void) {
	if (m_pFixHolecupPos) {
		JpSharedDoc_SetHolecupPosFixed(!JpSharedDoc_IsHolecupPosFixed());
		unsigned long option = 6;
		if (JpSharedDoc_IsHolecupPosFixed()) {
			option = 7;
		}
		WSendPacket packet(GCT_REQUEST_ROOMINFO_CHANGED);
		packet.Encode2(0xFFFF);
		packet.Encode1(1);
		packet.Encode1(0xc);
		packet.Encode4(option);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// 0x522e60: outside the hole mode (JP CSharedDoc +0x109b8 = the KR
// m_roomInfo.m_roomMode, 0x13) the slope starts off.
void FrSingleTypeDlg::OnChipinSlopeInit(PG_MSGARG param) {
	m_pChipinSlope = DYNAMIC_CAST(FrComboCtlEx, param);
	if (m_pChipinSlope && WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomMode != 0x13) {
		_gateway::_golfgame::CHoleInOneGameDoc* doc = JpGetChipInDoc();
		if (doc) {
			doc->SetApplySlope(false);
		}
	}
}

// The chip-in ticket item (its count is "chipin_ctrl.ticket"'s text).
static const unsigned long kChipinTicketTypeId = 0x1a00017e;

// 0x522f00
void FrSingleTypeDlg::SetupSingleModeControls(void) {
	// JP FrWnd +0x80 (SetAlpha2ToChild 0xbe8740 keeps m_wndAlpha2 at +0x84):
	// the KR m_wndAlpha.
	if (m_pBackground) {
		m_pBackground->SetAlpha(0.0f);
	}
	CSharedDoc* shared = WSingleton<CSharedDoc>::Instance();
	if (shared->m_roomInfo.m_roomMode != 0x13) {
		// The chip-in room: the course box is hidden.
		if (m_pModeTitle) {
			m_pModeTitle->SetBgImg("single_title_chipin_r");
		}
		_gateway::_golfgame::CHoleInOneGameDoc* doc = JpGetChipInDoc();
		if (m_pChipinSlope) {
			m_pChipinSlope->AddTexItem("single_txt_apply");
			m_pChipinSlope->AddTexItem("single_txt_cancel");
			m_pChipinSlope->SelectItem(1);
			if (doc) {
				doc->SetApplySlope(false);
			}
		}
		if (m_pChipinSize) {
			m_pChipinSize->AddTexItem("single_txt_normalsize");
			m_pChipinSize->AddTexItem("single_num_chipin_x2");
			m_pChipinSize->AddTexItem("single_num_chipin_x3");
			m_pChipinSize->AddTexItem("single_num_chipin_x4");
			m_pChipinSize->AddTexItem("single_num_chipin_x5");
			m_pChipinSize->AddTexItem("single_num_chipin_x6");
			m_pChipinSize->AddTexItem("single_num_chipin_x7");
			m_pChipinSize->AddTexItem("single_num_chipin_x8");
			m_pChipinSize->AddTexItem("single_num_chipin_x9");
			m_pChipinSize->SelectItem(0);
			if (doc) {
				doc->SetChipInSize(1);
			}
		}
		// JP CSharedDoc +0x375c: the KR m_itemInfoList (0x63b570 inlined).
		int count = 0;
		std::list<sItemInfo>::iterator it;
		for (it = shared->m_itemInfoList.begin(); it != shared->m_itemInfoList.end(); ++it) {
			if (it->m_typeId == kChipinTicketTypeId) {
				break;
			}
		}
		if (it != shared->m_itemInfoList.end()) {
			count = it->m_count;
		}
		strncpy(m_chipinTicketText, MakeStr("x%d", count), 7);
		if (m_pCourseCtrl) {
			m_pCourseCtrl->SetVisible(false);
		}
	} else {
		// The hole room: the chip-in box is hidden.
		if (m_pChipinCtrl) {
			m_pChipinCtrl->SetVisible(false);
		}
		// JP CSharedDoc +0x1094f (room +0x4b): the KR m_roomInfo.m_value4B,
		// the hole mode.
		if (shared->m_roomInfo.m_value4B == 4) {
			if (m_pModeTitle) {
				m_pModeTitle->SetBgImg("single_title_hole_r");
			}
			if (m_pBgCourse) {
				m_pBgCourse->SetBgImg("single_map_bg_hole");
			}
			if (m_pFixHole) {
				m_pFixHole->SetVisible(true);
			}
			if (m_pFixHolecupPos) {
				m_pFixHolecupPos->SetVisible(true);
				SetHolecupPosFixedMode(7);
			}
		}
	}
	// JP CSharedDoc +0x10950: the KR m_roomInfo.m_course.
	SetCourseImages(shared->m_roomInfo.m_course);
}

// 0x523160
bool FrSingleTypeDlg::OnInit(void) {
	SetupSingleModeControls();
	JpFrWnd_RaiseFlagF1aac8();
	if (RFLobbyDesktop::s_instance) {
		CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0x59, 0xb, 0, 0, 0);
	}
	// JP CSharedDoc +0x81a: the KR m_userInfo.guid.
	AddPlayer(MyGuid(false));
	// FrForm's (0xbe6570), not FrGameRoomDlg's.
	return FrForm::OnInit();
}

// 0x523250
const sFRESH_MSGMAP* FrStrokeTypeRoomDlg::GetMessageMap(void) const {
	return &FrStrokeTypeRoomDlg::_MsgMap;
}

sFRESH_MSGMAP FrStrokeTypeRoomDlg::_MsgMap = { &FrGameRoomDlg::_MsgMap, &FrStrokeTypeRoomDlg::_MsgEntries[0] };

// 0xe0acc8 (the handlers are stored by the static init 0xccddc0)
sFRESH_ENTRY FrStrokeTypeRoomDlg::_MsgEntries[] = {
	{ "invite", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrStrokeTypeRoomDlg::OnInviteLBtnUp },
	{ "group_stroke.pet0", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrStrokeTypeRoomDlg::OnPet0Init },
	{ "group_stroke.pet1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrStrokeTypeRoomDlg::OnPet1Init },
	{ "group_stroke.pet2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrStrokeTypeRoomDlg::OnPet2Init },
	{ "group_stroke.pet3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrStrokeTypeRoomDlg::OnPet3Init },
	{ "group_stroke.overgauge", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrStrokeTypeRoomDlg::OnOverGaugeInit },
	{ "group_stroke.user", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrStrokeTypeRoomDlg::OnUserInit },
	{ "group_stroke.user", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrStrokeTypeRoomDlg::OnUserOwnerDraw },
	{ "group_stroke.user", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrStrokeTypeRoomDlg::OnUserLBtnDown },
	{ "group_stroke.user", FRCMD_RBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrStrokeTypeRoomDlg::OnUserRBtnUp },
	{ "group_stroke.user", FRCMD_DBLCLICK, FrFuncVV, (FRESH_PMSG)&FrStrokeTypeRoomDlg::OnUserDblClick },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x523260
FrStrokeTypeRoomDlg::FrStrokeTypeRoomDlg(void) {
	InitRoom();
}

// 0x5232e0
FrStrokeTypeRoomDlg::~FrStrokeTypeRoomDlg(void) {
}

// 0x523300
bool FrStrokeTypeRoomDlg::OnInit(void) {
	m_pGroupStroke->SetVisible(true);
	if (m_pFrameBg) {
		m_pFrameBg->Open("gameroom_background.tga");
	}
	return FrGameRoomDlg::OnInit();
}

// 0x523330
void FrStrokeTypeRoomDlg::OnProc(const float dt) {
	FrGameRoomDlg::OnProc(dt);
}

// 0x523370
IObject* FrStrokeTypeRoomDlgMakeInstance(void) {
	return new FrStrokeTypeRoomDlg;
}

// 0x523430
void FrStrokeTypeRoomDlg::OnPet0Init(PG_MSGARG param) {
	m_pPet0 = DYNAMIC_CAST(FrArea, param);
}

// 0x5234a0
void FrStrokeTypeRoomDlg::OnPet1Init(PG_MSGARG param) {
	m_pPet1 = DYNAMIC_CAST(FrArea, param);
}

// 0x523510
void FrStrokeTypeRoomDlg::OnPet2Init(PG_MSGARG param) {
	m_pPet2 = DYNAMIC_CAST(FrArea, param);
}

// 0x523580
void FrStrokeTypeRoomDlg::OnPet3Init(PG_MSGARG param) {
	m_pPet3 = DYNAMIC_CAST(FrArea, param);
}

// 0x59fc40 (KR CLobbyMain::GetMedalIndex): only with ten results or more,
// for a player whose character type is at most 3; its rank among the
// counted players (left players count only from the fourth hole, or the
// intrusion's expected member count when that content is on: JP local
// content 0x2f, the KR 0x2e) less the better-ranked players of character
// type above 3, through the JP table 0x6335c0.
int JpLobbyMain_GetMedalIndex(uint32_t uid) {
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if (doc->m_gameResultList.size() < 10) {
		return 3;
	}
	unsigned char index = doc->GetIndex(uid);
	if (index == 0xFF || doc->m_gameResultList[index].m_characterType > 3) {
		return 3;
	}
	unsigned char myRank = doc->m_gameResultList[index].m_rank;
	unsigned char memberCount = 0;
	unsigned char skipCount = 0;
	unsigned char expMemberCount = 0;
	bool intrusion = IsLocalContent((localContentType_t)0x2f) != 0;
	if (intrusion) {
		expMemberCount = WSingleton<CIntrusion>::Instance()->GetCalcExpMemberSize();
	}
	if (!intrusion || expMemberCount == 0) {
		for (std::vector<sRivalData>::iterator it = doc->m_gameResultList.begin(); it != doc->m_gameResultList.end(); ++it) {
			unsigned char state = it->m_state;
			if (state == 3 && GetHoleIndex(it->m_hole) < 4) {
				continue;
			}
			memberCount++;
			if (state != 3 && it->m_rank < myRank && it->m_characterType > 3) {
				skipCount++;
			}
		}
	} else {
		memberCount = expMemberCount;
		for (std::vector<sRivalData>::iterator it = doc->m_gameResultList.begin(); it != doc->m_gameResultList.end(); ++it) {
			if (it->m_state != 3 && it->m_rank < myRank && it->m_characterType > 3) {
				skipCount++;
			}
		}
	}
	return JpSharedDoc_GetMedalForRank(memberCount, (unsigned char)(myRank - skipCount));
}

// 0x6335c0 (thiscall on the JP CSharedDoc; name ours): the medal (0 gold,
// 1 silver, 2 bronze, 3 none) of a rank among memberCount players; the JP
// room modes 0x14 and 0x15 (+0x109b8, the KR m_roomInfo.m_roomMode) keep
// the 19-22 player table above 26 players. No KR counterpart (the KR
// GetMedalIndex splits by 9 / 18 holes).
int JpSharedDoc_GetMedalForRank(int memberCount, int rank) {
	if (memberCount <= 14) {
		if (rank == 1) {
			return 2;
		}
		return 3;
	}
	if (memberCount <= 18) {
		if (rank == 1) {
			return 1;
		}
		if (rank == 2) {
			return 2;
		}
		return 3;
	}
	bool smallTable = memberCount <= 22;
	if (!smallTable && memberCount > 26) {
		unsigned char mode = WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomMode;
		if (mode == 0x14 || mode == 0x15) {
			smallTable = true;
		} else {
			if (rank == 1) {
				return 0;
			}
			if (rank <= 3) {
				return 1;
			}
			if (rank <= 6) {
				return 2;
			}
			return 3;
		}
	}
	if (smallTable) {
		if (rank == 1) {
			return 0;
		}
		if (rank == 2) {
			return 1;
		}
		if (rank == 3) {
			return 2;
		}
		return 3;
	}
	// 23..26 players.
	if (rank == 1) {
		return 0;
	}
	if (rank == 2) {
		return 1;
	}
	if (rank <= 4) {
		return 2;
	}
	return 3;
}

// 0x6f4ac0 (in the JP TitleFontEx unit): the JP font file entry 0xe16868.
static char* s_singleNumFontFile[] = {
	"[single_num_course.jpg"
};

JpSingleNumFont::JpSingleNumFont(void) {
	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = &s_singleNumFontFile[0];
	info.numPages = 1;
	info.fontw = 10;
	info.fonth = 13;
	info.texw = 128;
	info.texh = 16;
	info.pCharSet = "0123456789x";

	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

// 0xccc4c0
struct __sFrMassTypeRoomDlg {
	__sFrMassTypeRoomDlg(void) {
		ObjectFactory().AddObjectFunctor(FrMassTypeRoomDlgMakeInstance, "FrMassTypeRoomDlg");
	}
};
static __sFrMassTypeRoomDlg s_registerFrMassTypeRoomDlg;

// 0xccc8b0
struct __sFrGPTypeRoomDlg {
	__sFrGPTypeRoomDlg(void) {
		ObjectFactory().AddObjectFunctor(FrGPTypeRoomDlgMakeInstance, "FrGPTypeRoomDlg");
	}
};
static __sFrGPTypeRoomDlg s_registerFrGPTypeRoomDlg;

// 0xccdac0
struct __sFrSingleTypeDlg {
	__sFrSingleTypeDlg(void) {
		ObjectFactory().AddObjectFunctor(FrSingleTypeDlgMakeInstance, "FrSingleTypeDlg");
	}
};
static __sFrSingleTypeDlg s_registerFrSingleTypeDlg;

// 0xccdda0
struct __sFrStrokeTypeRoomDlg {
	__sFrStrokeTypeRoomDlg(void) {
		ObjectFactory().AddObjectFunctor(FrStrokeTypeRoomDlgMakeInstance, "FrStrokeTypeRoomDlg");
	}
};
static __sFrStrokeTypeRoomDlg s_registerFrStrokeTypeRoomDlg;
