// Original: d:\build\custom\temp\code\client\projectg\lobbymain.cpp
#define LOBBYMAIN_SHAREDDOC_INLINE
#define LOBBYMAIN_EXHIBITION_INLINE
#define LOBBYMAIN_EVENTPRIZEDLG_INLINE
#include <atlbase.h>	// stand-in for the precompiled header: _AtlWinModule initializer
#include "NetworkSystem.h"
#include "SoundManager.h"
#include <windows.h>
#include <shellapi.h>
#include "ClientSetting.h"
#include "LobbyMain.h"
#include "LevelUpItemDlg.h"
#include "StandingInLine.h"
#include "guilddefine.h"
#include "EventPrizeDlg.h"
#include "Localize.h"
#include "TopIconManager.h"
#include "logininfo.h"
#include "frbutton.h"
#include "ObjectFactory.h"
#include "actor.h"
#include "CreateNickDlg.h"
#include "buddymanager.h"
#include "mousecursor.h"
#include "frarea.h"
#include "TaskManager.h"
#include "globalgamedefine.h"
#include "shared/globalgamedefine.h"
#include "SharedDoc.h"
#include "Packet.h"
#include "fresh.h"
#include "frgraphicinterface.h"
#include "frwndmanager.h"
#ifdef JP_PAKS
#include "frgroupbox.h"
#endif
#include "frlistbox.h"
#include "LoginDlg.h"
#include "EquipDlg.h"
#include "MakeRoomDlg.h"
#include "exhibition.h"
#include "noticedlg.h"
#include "s5/SkinnedTheme.h"
#include "playerdlg.h"
#include "messengerdlg.h"
#include "frcombobox.h"
#include "Intrusion.h"
#include "user_info.h"
#include "frviewer.h"
#include "fredit.h"
#include "frstatic.h"
#include "frgaugebar.h"
#include "frframe.h"
#include "ServerDlg.h"
#include "UserListDlg.h"
#include "MailBoxDlg.h"
#include "ParanNickDlg.h"
#include "GuildNoticeDlg.h"
#include "NewsReader.h"
#include "winput.h"
#include "icontentsdoc.h"
#include "contentsdoc.h"
#include "ScoreDlg.h"
#include "DetailedRoomInfoDlg.h"
#include "PasswordDlg.h"
#include "CaddieWarningDlg.h"
#include "CaddieInfoDlg.h"
#include "ChangeRoomInfoDlg.h"
#include "partsinfo.h"
#include "ReportDlg.h"
#include "frscrollbar.h"
#include "frdesktop.h"
#include "ToolTip.h"
#include "OptionDlg.h"
#include "ProjectG.h"
#include "GolfDoc.h"
#include "polysoup.h"
#include "wview.h"
#include "cardsystem.h"
#include "itembuffactor.h"
#include "ValentineEventDlg.h"
#include "bitmap.h"
#include "WorldTourEventDlg.h"
#include "FrFindEbortEventDlg.h"
#include "FrHalloweenDlg.h"
#include "ChristamsEventDlg.h"
#include "FrTicketExchangeDlg.h"
#include "gamedatadb.h"
#include "gdbitmapbox.h"
#include "InputManager.h"
#include "ResultDlg.h"
#include "TreasureDlg.h"
#include "Treasure.h"
#include "RankingDlg.h"
#include "OnelineReqDlg.h"
#include "ChatMsg.h"
#include "EmoticonDlg.h"
#include "UccManager.h"
#include "NetResourceManager.h"
#include "shared/s5/SharedUtilities.h"
#include <algorithm>
#include "FamilyMain.h"
#include "InviteDlg.h"
#include "DlgQuickStartSetting.h"
#include "NewRecordDlg.h"
#include "MissionEventDlg.h"
#include "BingoEventDlg.h"
#include "PointEventDlg.h"
#include "Browser.h"
#include "birthdayeventdlg.h"
#include "pg_boot_todo.h"

// The IME input device (public ?g_ime@@3PAVWInputDev@@A).
extern WInputDev *g_ime;
// The mouse input device (public ?g_mouse@@3PAVWInputDev@@A).
extern WInputDev *g_mouse;
// The camera matrix of the 3D view (public ?g_camera@@3VWMatrix@@A).
extern WMatrix g_camera;

// The room list's game type names: VS [0..1], Mass [2..4], Battle [5..6],
// chat [7] (defined at lobbymain.cpp:226).
extern std::string g_strGameType[8];

extern bool g_bQuit;

// Unnamed data: the name of the actor the guild info button sends to, and
// the room list filter (category tab, game type the category tabs reset to -1).
struct sLobbyRoomListState {
	const char *m_actorName; // 0x0
	UNKNOWN(0x4, 0x8);
	int m_roomCate;          // 0x8
	int m_gameType;          // 0xC
};
CHECK_FIELD(sLobbyRoomListState, m_roomCate, 0x8);
CHECK_FIELD(sLobbyRoomListState, m_gameType, 0xC);
// File-local in the original: the compiler knows no pointer reaches it, so it
// moves its loads across stores through pointers (OnRoomList_RoomListInit,
// the category button inits). Initial values from the exe's .data
// (port/compat/pg_data_generated.cpp); 0x4 holds a pointer, zeroed there.
static sLobbyRoomListState data_aa9eac = { "GuildActor", { 0 }, 4, -1 };

// Unnamed data: the IME mode (alphanumeric) the chat language mark shows.
static bool data_ab2830;

// clientsetting.h:404
int COption::gGetChatWndUp(void) {
	return m_option.m_chatWndUp;
}

// clientsetting.h:405
void COption::gSetChatWndUp(int up) {
	m_option.m_chatWndUp = up;
}

// invitedlg.h:30
unsigned long FrInviteDlg::GetSvrGUID(void) {
	return m_svrGUID;
}

// invitedlg.h:31
unsigned char FrInviteDlg::GetChannelIdx(void) {
	return m_channelIdx;
}

// invitedlg.h:32
unsigned short FrInviteDlg::GetRoomIdx(void) {
	return m_roomIdx;
}

// eventprizedlg.h:13
void FrEventPrizeDlg::SetEvent(unsigned char event) {
	m_event = event;
}

// eventprizedlg.h:14
unsigned char FrEventPrizeDlg::GetEvent(void) const {
	return m_event;
}

// topiconmanager.h:77
void CIconManager::AddTopIconFromOutSide(void) {
	m_outSideIconCount++;
}

// topiconmanager.h:78
void CIconManager::ClearInformation(void) {
	m_outSideIconCount = 0;
}

// Top-page event icon slot (values only known by number so far).
enum eTopIconID {
	TOPICON_0 = 0
};

// One icon button of the top page, placed by its slot.
class cTopEventIcon {
public:
	cTopEventIcon(eTopIconID id);
	void Prepare(FrButton* button);

private:
	eTopIconID m_id;    // 0x0
};

// lobbymain.cpp:185
cTopEventIcon::cTopEventIcon(eTopIconID id) {
	m_id = id;
}

// lobbymain.cpp:187
void cTopEventIcon::Prepare(FrButton* button) {
	if (button != 0) {
		button->SetPushSound("ui_desktop_icon_click");
		button->EnableHover(true, 0.5f);
		int slot = CLoginInfo::Instance()->IsPcBang() ? m_id : m_id - 1;
		button->SetRect(WRect((float)(slot * 80) + 1.0f, 50.0f, 80.0f, 80.0f));
	}
}

// lobbymain.cpp:217
// Room list category names, indexed like the category tabs (VS, Mass,
// Battle, Chat, All).
std::string g_strCate[5] = {
	"\xB4\xEB\xC0\xFC",
	"\xB4\xEB\xC8\xB8",
	"\xB9\xE8\xC6\xB2",
	"\xB4\xEB\xC8\xAD",
	"\xB8\xF0\xB5\xCE"
};

// lobbymain.cpp:226
// Room list game type names (stroke, match, 30-player tournament, 30-player
// team match, guild match, Pang battle, approach, chat).
std::string g_strGameType[8] = {
	"\xBD\xBA\xC6\xAE\xB7\xCE\xC5\xA9",
	"\xB8\xC5\xC4\xA1",
	"30\xC0\xCE\xB4\xEB\xC8\xB8",
	"30\xC0\xCE\xC6\xC0\xC0\xFC",
	"\xB1\xE6\xB5\xE5\xB4\xEB\xC0\xFC",
	"\xC6\xCE\xB9\xE8\xC6\xB2",
	"\xBE\xEE\xC7\xC1\xB7\xCE\xC4\xA1",
	"\xB4\xEB\xC8\xAD"
};

// lobbymain.cpp:241
int GetCateByGameType(int type) {
	switch (type) {
	case 4:
		return 1;
	case 5:
		return 1;
	case 6:
		return 1;
	case 7:
		return 2;
	case 9:
		return 2;
	case 10:
		return 2;
	case 2:
		return 3;
	case 0:
		return 0;
	case 1:
		return 0;
	case 3:
		return 0;
	case 8:
		return 0;
	}
	return 0;
}

// lobbymain.cpp:259
long GetShotTimeLimitByGameType(int type) {
	switch (type) {
	case 0:
		return 40000;
	case 1:
		return 40000;
	case 2:
		return 0;
	case 3:
		return 40000;
	case 4:
		return 0;
	case 5:
		return 0;
	case 6:
		return 0;
	case 7:
		return 40000;
	case 8:
		return 40000;
	case 9:
		return 40000;
	case 10:
		return 40000;
	}
	return 40000;
}

// lobbymain.cpp:277
long GetGameTimeLimitByGameType(int type) {
	switch (type) {
	case 0:
		return 0;
	case 1:
		return 0;
	case 2:
		return 0;
	case 3:
		return 0;
	case 4:
		return 2400000;
	case 5:
		return 2400000;
	case 6:
		return 2400000;
	case 7:
		return 0;
	case 8:
		return 0;
	case 9:
		return 40000;
	case 10:
		return 40000;
	}
	return 0;
}

// lobbymain.cpp:296
unsigned char GetRandGameTypeByCate(int cate) {
	switch (cate) {
	case 0:
		return 0;
	case 1:
		return 4;
	case 2:
		return 10;
	case 3:
		return 2;
	case 4:
		return 0;
	}
	return 0;
}

// lobbymain.cpp:316
// Korean display name of a game type, for the lobby's room messages.
static const char* GetLobbyGameTypeName(unsigned char gameType) {
	switch (gameType) {
	case 0:
		return "\xBD\xBA\xC6\xAE\xB7\xCE\xC5\xA9";
	case 2:
		return "\xC3\xA4\xC6\xC3\xB9\xE6";
	case 3:
		return "\xB8\xC5\xC4\xA1";
	case 4:
		return "\xB4\xEB\xC8\xB8";
	case 5:
		return "\xB4\xEB\xC8\xB8\xC6\xC0\xC0\xFC";
	case 6:
		return "\xB1\xE6\xB5\xE5\xB4\xEB\xC0\xFC";
	case 7:
		return "\xC6\xCE\xB9\xE8\xC6\xB2";
	case 8:
		return "\xB8\xAE\xBE\xF3\xB8\xB6\xC0\xCC\xB7\xEB";
	case 10:
		return "\xBE\xEE\xC7\xC1\xB7\xCE\xC4\xA1";
	}
	return "";
}

// lobbymain.cpp:336
// Approach ranking order: sort group (higher first), approach distance (lower
// first), approach point (higher first), then 0x13F (lower first).
bool ApproachRankUserCompare(const void* lhs, const void* rhs) {
	const sRivalData* pLhs = (const sRivalData*)lhs;
	const sRivalData* pRhs = (const sRivalData*)rhs;

	if (pLhs->m_sortGroup > pRhs->m_sortGroup) {
		return true;
	}
	if (pLhs->m_sortGroup < pRhs->m_sortGroup) {
		return false;
	}
	if (pLhs->m_approachDistance < pRhs->m_approachDistance) {
		return true;
	}
	if (pLhs->m_approachDistance > pRhs->m_approachDistance) {
		return false;
	}
	if (pLhs->m_approachPoint > pRhs->m_approachPoint) {
		return true;
	}
	if (pLhs->m_approachPoint < pRhs->m_approachPoint) {
		return false;
	}
	return pLhs->m_value13F < pRhs->m_value13F;
}

// lobbymain.cpp:362
// Result ranking order: sort group (higher first), score (lower first), pang
// (higher first), then 0x13F (lower first).
bool RankUserCompare(const void* lhs, const void* rhs) {
	const sRivalData* pLhs = (const sRivalData*)lhs;
	const sRivalData* pRhs = (const sRivalData*)rhs;

	if (pLhs->m_sortGroup > pRhs->m_sortGroup) {
		return true;
	}
	if (pLhs->m_sortGroup < pRhs->m_sortGroup) {
		return false;
	}
	if (pLhs->m_score < pRhs->m_score) {
		return true;
	}
	if (pLhs->m_score > pRhs->m_score) {
		return false;
	}
	if (pLhs->m_pang > pRhs->m_pang) {
		return true;
	}
	if (pLhs->m_pang < pRhs->m_pang) {
		return false;
	}
	return pLhs->m_value13F < pRhs->m_value13F;
}

// lobbymain.cpp:393 (run-time type and factory registration of CLobbyMain)
const WRTTI CLobbyMain::m_RTTI("CLobbyMain", &CTaskMain::m_RTTI);

IObject* CLobbyMainMakeInstance(void) {
	return new CLobbyMain;
}

struct __impCLobbyMain {
	__impCLobbyMain(void) {
		ObjectFactory().AddObjectFunctor(CLobbyMainMakeInstance, "CLobbyMain");
		ActorFactory().AddActorClass("CLobbyMain");
	}
};
static __impCLobbyMain s_impCLobbyMain;

// lobbymain.cpp:394
const sFRESH_MSGMAP* CLobbyMain::GetMessageMap(void) const {
	return &CLobbyMain::_MsgMap;
}

sFRESH_MSGMAP CLobbyMain::_MsgMap = { &CTaskMain::_MsgMap, &CLobbyMain::_MsgEntries[0] };

sFRESH_ENTRY CLobbyMain::_MsgEntries[] = {
	// lobbymain.cpp:408
	{ "LOGIN", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnLogin_Init },
	// lobbymain.cpp:409
	{ "LOGIN", FRCMD_FINISH, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnLogin_Finish },
	// lobbymain.cpp:399
	{ "BLANK", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnBlank_Init },
	// lobbymain.cpp:400
	{ "BLANK", FRCMD_FINISH, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnBlank_Finish },
	// lobbymain.cpp:403
	{ "SECOND_PWD", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnSecond_Pwd_Init },
	// lobbymain.cpp:404
	{ "SECOND_PWD", FRCMD_FINISH, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnSecond_Pwd_Finish },
	// lobbymain.cpp:408
	{ "TOPPAGE", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_Init },
	// lobbymain.cpp:409
	{ "TOPPAGE", FRCMD_FINISH, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_Finish },
	// lobbymain.cpp:419
	{ "TOPPAGE", FRCMD_DESTROY, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_Destroy },
	// lobbymain.cpp:420
	{ "TOPPAGE.gameplay", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_GameInit },
	// lobbymain.cpp:412
	{ "TOPPAGE.gameplay", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_GameUp },
	// lobbymain.cpp:413
	{ "TOPPAGE.family", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_FamilyInit },
	// lobbymain.cpp:414
	{ "TOPPAGE.family", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_FamilyUp },
	// lobbymain.cpp:415
	{ "TOPPAGE.pangyastudy", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_PangysStudyInit },
	// lobbymain.cpp:416
	{ "TOPPAGE.pangyastudy", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_PangysStudyUp },
	// lobbymain.cpp:419
	{ "TOPPAGE.notice", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_NoticeInit },
	// lobbymain.cpp:420
	{ "TOPPAGE.notice", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_NoticeUp },
	// lobbymain.cpp:421
	{ "TOPPAGE.server", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_ServerInit },
	// lobbymain.cpp:422
	{ "TOPPAGE.server", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_ServerUp },
	// lobbymain.cpp:423
	{ "TOPPAGE.pcbang", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_PcbangInit },
	// lobbymain.cpp:424
	{ "TOPPAGE.pcbang", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_PcbangUp },
	// lobbymain.cpp:425
	{ "TOPPAGE.rental_expired", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_RentalExpiredAlramInit },
	// lobbymain.cpp:426
	{ "TOPPAGE.rental_expired", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_RentalExpiredAlramUp },
	// lobbymain.cpp:427
	{ "TOPPAGE.event1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_Event1Init },
	// lobbymain.cpp:428
	{ "TOPPAGE.event1", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_Event1Up },
	// lobbymain.cpp:430
	{ "TOPPAGE.new_2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_NewInit },
	// lobbymain.cpp:431
	{ "TOPPAGE.new_2", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_NewOwnerDraw },
	// lobbymain.cpp:451
	{ "TOPPAGE.TopIcon1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_TopIcon1Init },
	// lobbymain.cpp:452
	{ "TOPPAGE.TopIcon1", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_TopIcon1Up },
	// lobbymain.cpp:454
	{ "TOPPAGE.TopIcon2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_TopIcon2Init },
	// lobbymain.cpp:455
	{ "TOPPAGE.TopIcon2", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_TopIcon2Up },
	// lobbymain.cpp:457
	{ "TOPPAGE.TopIcon3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_TopIcon3Init },
	// lobbymain.cpp:458
	{ "TOPPAGE.TopIcon3", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_TopIcon3Up },
	// lobbymain.cpp:460
	{ "TOPPAGE.TopIcon4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_TopIcon4Init },
	// lobbymain.cpp:461
	{ "TOPPAGE.TopIcon4", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_TopIcon4Up },
	// lobbymain.cpp:463
	{ "TOPPAGE.TopIcon5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_TopIcon5Init },
	// lobbymain.cpp:464
	{ "TOPPAGE.TopIcon5", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_TopIcon5Up },
	// lobbymain.cpp:466
	{ "TOPPAGE.TopIcon6", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_TopIcon6Init },
	// lobbymain.cpp:467
	{ "TOPPAGE.TopIcon6", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_TopIcon6Up },
	// lobbymain.cpp:476
	{ "TOPPAGE.pointevent", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_PointEventInit },
	// lobbymain.cpp:477
	{ "TOPPAGE.pointevent", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_PointEventDown },
	// lobbymain.cpp:480
	{ "TOPPAGE.valevent_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_ValEventInit },
	// lobbymain.cpp:481
	{ "TOPPAGE.valevent_btn", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_ValEventDown },
	// lobbymain.cpp:484
	{ "TOPPAGE.worldtourevent", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_WorldTourEventInit },
	// lobbymain.cpp:485
	{ "TOPPAGE.worldtourevent", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_WorldTourEventDown },
	// lobbymain.cpp:488
	{ "TOPPAGE.findebortevent", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_FindEbortEventInit },
	// lobbymain.cpp:489
	{ "TOPPAGE.findebortevent", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_FindEbortEventDown },
	// lobbymain.cpp:492
	{ "TOPPAGE.halloween2007event", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_Halloween2007Init },
	// lobbymain.cpp:493
	{ "TOPPAGE.halloween2007event", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_Halloween2007EvenDown },
	// lobbymain.cpp:496
	{ "TOPPAGE.Christmas2007Event", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_Christmas2007EventInit },
	// lobbymain.cpp:497
	{ "TOPPAGE.Christmas2007Event", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_Christmas2007EventDown },
	// lobbymain.cpp:506
	{ "TOPPAGE.ticketexchage", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_TicketExchangeInit },
	// lobbymain.cpp:507
	{ "TOPPAGE.ticketexchage", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_TicketExchangeDown },
	// lobbymain.cpp:510
	{ "TOPPAGE.MissionEvent", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_MissionEventInit },
	// lobbymain.cpp:511
	{ "TOPPAGE.MissionEvent", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_MissionEventDown },
	// lobbymain.cpp:514
	{ "TOPPAGE.BingoEvent", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_BingoEventInit },
	// lobbymain.cpp:515
	{ "TOPPAGE.BingoEvent", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_BingoEventDown },
	// lobbymain.cpp:518
	{ "TOPPAGE.KoohBirthdayEvent", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_KoohBirthdayEventInit },
	// lobbymain.cpp:519
	{ "TOPPAGE.KoohBirthdayEvent", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_KoohBirthdayEventDown },
	// lobbymain.cpp:522
	{ "TOPPAGE.treasureisland", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnToppage_TreasureIslandInit },
	// lobbymain.cpp:523
	{ "TOPPAGE.treasureisland", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_TreasureIslandLBDown },
	// lobbymain.cpp:559
	{ "TOPPAGE.ubar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBarInit },
	// lobbymain.cpp:560
	{ "TOPPAGE.ubar.back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_BackInit },
	// lobbymain.cpp:561
	{ "TOPPAGE.ubar.back", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_BackUp },
	// lobbymain.cpp:562
	{ "TOPPAGE.ubar.back", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:563
	{ "TOPPAGE.ubar.back", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:564
	{ "TOPPAGE.ubar.front", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_FrontInit },
	// lobbymain.cpp:565
	{ "TOPPAGE.ubar.front", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_FrontUp },
	// lobbymain.cpp:566
	{ "TOPPAGE.ubar.front", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:567
	{ "TOPPAGE.ubar.front", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:568
	{ "TOPPAGE.ubar.shop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ShopInit },
	// lobbymain.cpp:569
	{ "TOPPAGE.ubar.shop", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ShopUp },
	// lobbymain.cpp:570
	{ "TOPPAGE.ubar.shop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:571
	{ "TOPPAGE.ubar.shop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:572
	{ "TOPPAGE.ubar.myroom", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MyRoomInit },
	// lobbymain.cpp:573
	{ "TOPPAGE.ubar.myroom", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MyRoomUp },
	// lobbymain.cpp:574
	{ "TOPPAGE.ubar.myroom", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:575
	{ "TOPPAGE.ubar.myroom", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:576
	{ "TOPPAGE.ubar.itemstorage", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ItemStorageInit },
	// lobbymain.cpp:577
	{ "TOPPAGE.ubar.itemstorage", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ItemStorageUp },
	// lobbymain.cpp:578
	{ "TOPPAGE.ubar.itemstorage", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:579
	{ "TOPPAGE.ubar.itemstorage", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:580
	{ "TOPPAGE.ubar.gift", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GiftInit },
	// lobbymain.cpp:581
	{ "TOPPAGE.ubar.gift", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GiftUp },
	// lobbymain.cpp:582
	{ "TOPPAGE.ubar.gift", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:583
	{ "TOPPAGE.ubar.gift", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:584
	{ "TOPPAGE.ubar.cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_CookieInit },
	// lobbymain.cpp:585
	{ "TOPPAGE.ubar.cookie", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_CookieUp },
	// lobbymain.cpp:586
	{ "TOPPAGE.ubar.cookie", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:587
	{ "TOPPAGE.ubar.cookie", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:588
	{ "TOPPAGE.ubar.game", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GameInit },
	// lobbymain.cpp:589
	{ "TOPPAGE.ubar.game", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GameUp },
	// lobbymain.cpp:590
	{ "TOPPAGE.ubar.game", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:591
	{ "TOPPAGE.ubar.game", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:592
	{ "TOPPAGE.ubar.server", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ServerInit },
	// lobbymain.cpp:593
	{ "TOPPAGE.ubar.server", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ServerUp },
	// lobbymain.cpp:594
	{ "TOPPAGE.ubar.server", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:595
	{ "TOPPAGE.ubar.server", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:596
	{ "TOPPAGE.ubar.exit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ExitInit },
	// lobbymain.cpp:597
	{ "TOPPAGE.ubar.exit", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ExitUp },
	// lobbymain.cpp:598
	{ "TOPPAGE.ubar.exit", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:599
	{ "TOPPAGE.ubar.exit", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:600
	{ "TOPPAGE.ubar.tabbtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_TabBtnInit },
	// lobbymain.cpp:601
	{ "TOPPAGE.ubar.tabbtn", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_TabBtnUp },
	// lobbymain.cpp:604
	{ "TOPPAGE.utab", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderTab_Init },
	// lobbymain.cpp:606
	{ "TOPPAGE.utab.magicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MagicBoxInit },
	// lobbymain.cpp:607
	{ "TOPPAGE.utab.magicbox", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MagicBoxUp },
	// lobbymain.cpp:608
	{ "TOPPAGE.utab.magicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:609
	{ "TOPPAGE.utab.magicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:611
	{ "TOPPAGE.utab.tikimagicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_TikiMagicBoxInit },
	// lobbymain.cpp:612
	{ "TOPPAGE.utab.tikimagicbox", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_TikiMagicBoxUp },
	// lobbymain.cpp:613
	{ "TOPPAGE.utab.tikimagicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:614
	{ "TOPPAGE.utab.tikimagicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:636
	{ "TOPPAGE.utab.scratch", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ScratchInit },
	// lobbymain.cpp:637
	{ "TOPPAGE.utab.scratch", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ScratchUp },
	// lobbymain.cpp:638
	{ "TOPPAGE.utab.scratch", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:639
	{ "TOPPAGE.utab.scratch", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:641
	{ "TOPPAGE.utab.bongdarishop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_BongdariShopInit },
	// lobbymain.cpp:642
	{ "TOPPAGE.utab.bongdarishop", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_BongdariShopUp },
	// lobbymain.cpp:643
	{ "TOPPAGE.utab.bongdarishop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:644
	{ "TOPPAGE.utab.bongdarishop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:645
	{ "TOPPAGE.utab.ranking", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_RankingInit },
	// lobbymain.cpp:646
	{ "TOPPAGE.utab.ranking", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_RankingUp },
	// lobbymain.cpp:647
	{ "TOPPAGE.utab.ranking", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:648
	{ "TOPPAGE.utab.ranking", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:649
	{ "TOPPAGE.utab.guild", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GuildInit },
	// lobbymain.cpp:650
	{ "TOPPAGE.utab.guild", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GuildUp },
	// lobbymain.cpp:651
	{ "TOPPAGE.utab.guild", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:652
	{ "TOPPAGE.utab.guild", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:653
	{ "TOPPAGE.utab.notice", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_NoticeInit },
	// lobbymain.cpp:654
	{ "TOPPAGE.utab.notice", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_NoticeUp },
	// lobbymain.cpp:655
	{ "TOPPAGE.utab.notice", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:656
	{ "TOPPAGE.utab.notice", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:659
	{ "TOPPAGE.utab.maket", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MaketInit },
	// lobbymain.cpp:660
	{ "TOPPAGE.utab.maket", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MaketUp },
	// lobbymain.cpp:661
	{ "TOPPAGE.utab.maket", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:662
	{ "TOPPAGE.utab.maket", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:673
	{ "TOPPAGE.obar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_Init },
	// lobbymain.cpp:674
	{ "TOPPAGE.obar.title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_TitleInit },
	// lobbymain.cpp:675
	{ "TOPPAGE.obar.site", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_SiteInit },
	// lobbymain.cpp:676
	{ "TOPPAGE.obar.char", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharInit },
	// lobbymain.cpp:677
	{ "TOPPAGE.obar.char", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_CharDown },
	// lobbymain.cpp:678
	{ "TOPPAGE.obar.char", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharOwnerDraw },
	// lobbymain.cpp:679
	{ "TOPPAGE.obar.caddie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieInit },
	// lobbymain.cpp:680
	{ "TOPPAGE.obar.caddie", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_CaddieDown },
	// lobbymain.cpp:681
	{ "TOPPAGE.obar.caddie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieOwnerDraw },
	// lobbymain.cpp:682
	{ "TOPPAGE.obar.club", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubInit },
	// lobbymain.cpp:683
	{ "TOPPAGE.obar.club", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_ClubDown },
	// lobbymain.cpp:684
	{ "TOPPAGE.obar.club", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubOwnerDraw },
	// lobbymain.cpp:685
	{ "TOPPAGE.obar.aztec", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecInit },
	// lobbymain.cpp:686
	{ "TOPPAGE.obar.aztec", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_AztecDown },
	// lobbymain.cpp:687
	{ "TOPPAGE.obar.aztec", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecOwnerDraw },
	// lobbymain.cpp:688
	{ "TOPPAGE.obar.mascot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotInit },
	// lobbymain.cpp:689
	{ "TOPPAGE.obar.mascot", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MascotDown },
	// lobbymain.cpp:690
	{ "TOPPAGE.obar.mascot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotOwnerDraw },
	// lobbymain.cpp:691
	{ "TOPPAGE.obar.char_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharSelInit },
	// lobbymain.cpp:692
	{ "TOPPAGE.obar.char_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharSelOwnerDraw },
	// lobbymain.cpp:693
	{ "TOPPAGE.obar.caddie_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieSelInit },
	// lobbymain.cpp:694
	{ "TOPPAGE.obar.caddie_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieSelOwnerDraw },
	// lobbymain.cpp:695
	{ "TOPPAGE.obar.club_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubSelInit },
	// lobbymain.cpp:696
	{ "TOPPAGE.obar.club_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubSelOwnerDraw },
	// lobbymain.cpp:697
	{ "TOPPAGE.obar.aztec_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecSelInit },
	// lobbymain.cpp:698
	{ "TOPPAGE.obar.aztec_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecSelOwnerDraw },
	// lobbymain.cpp:699
	{ "TOPPAGE.obar.mascot_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotSelInit },
	// lobbymain.cpp:700
	{ "TOPPAGE.obar.mascot_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotSelOwnerDraw },
	// lobbymain.cpp:701
	{ "TOPPAGE.obar.pang_cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_PangCookieInit },
	// lobbymain.cpp:702
	{ "TOPPAGE.obar.pang_cookie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_PangCookieOwnerDraw },
	// lobbymain.cpp:703
	{ "TOPPAGE.obar.onelinemsg_req", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineReqBtnInit },
	// lobbymain.cpp:704
	{ "TOPPAGE.obar.onelinemsg_req", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_OneLineReqBtnUp },
	// lobbymain.cpp:705
	{ "TOPPAGE.obar.onelineview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineViewInit },
	// lobbymain.cpp:706
	{ "TOPPAGE.obar.onelineview", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineViewOwnerDraw },
	// lobbymain.cpp:707
	{ "TOPPAGE.obar.spcard", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_SpecialCardInit },
	// lobbymain.cpp:708
	{ "TOPPAGE.obar.spcard", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_SpecialCardUp },
	// lobbymain.cpp:709
	{ "TOPPAGE.obar.spcard", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:710
	{ "TOPPAGE.obar.spcard", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:711
	{ "TOPPAGE.obar.messenger", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MessengerInit },
	// lobbymain.cpp:712
	{ "TOPPAGE.obar.messenger", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MessengerUp },
	// lobbymain.cpp:713
	{ "TOPPAGE.obar.messenger", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:714
	{ "TOPPAGE.obar.messenger", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:715
	{ "TOPPAGE.obar.myinfo", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MyInfoInit },
	// lobbymain.cpp:716
	{ "TOPPAGE.obar.myinfo", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MyInfoUp },
	// lobbymain.cpp:717
	{ "TOPPAGE.obar.myinfo", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:718
	{ "TOPPAGE.obar.myinfo", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:719
	{ "TOPPAGE.obar.option", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OptionInit },
	// lobbymain.cpp:720
	{ "TOPPAGE.obar.option", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_OptionUp },
	// lobbymain.cpp:721
	{ "TOPPAGE.obar.option", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:722
	{ "TOPPAGE.obar.option", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:725
	{ "TOPPAGE.ubar.undertab_tooltip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_TabToolTipInit },
	// lobbymain.cpp:726
	{ "TOPPAGE.obar.messenger_bubble", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleInit },
	// lobbymain.cpp:727
	{ "TOPPAGE.obar.messenger_bubble", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleOwnerDraw },
	// lobbymain.cpp:728
	{ "TOPPAGE.obar.messenger_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleEditInit },
	// lobbymain.cpp:729
	{ "TOPPAGE.obar.messenger_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerAlarmBtnInit },
	// lobbymain.cpp:730
	{ "TOPPAGE.obar.messenger_alarm", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnMessengerAlarmBtnUp },
	// lobbymain.cpp:731
	{ "TOPPAGE.obar.gift_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerGiftAlarmBtnInit },
	// lobbymain.cpp:732
	{ "TOPPAGE.obar.gift_alarm", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnMessengerGiftAlarmBtnUp },
	// lobbymain.cpp:733
	{ "TOPPAGE.obar.tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarTooltipOwnerDraw },
	// lobbymain.cpp:734
	{ "TOPPAGE.treasure_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnTreasureAlarmInit },
	// lobbymain.cpp:735
	{ "TOPPAGE.treasure_alarm", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnTreasureAlarmUp },
	// lobbymain.cpp:738
	{ "TOPPAGE.FirstConnect", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnFirstConnectOwnerDraw },
	// lobbymain.cpp:741
	{ "TOPPAGE.Go_UCCShop", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_UCCShopBtnUp },
	// lobbymain.cpp:742
	{ "TOPPAGE.Go_CardShop", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnToppage_CardShopBtnUp },
	// lobbymain.cpp:745
	{ "SERVERLIST", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnServer_Init },
	// lobbymain.cpp:746
	{ "SERVERLIST", FRCMD_FINISH, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnServer_Finish },
	// lobbymain.cpp:749
	{ "ROOMLIST", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_Init },
	// lobbymain.cpp:750
	{ "ROOMLIST", FRCMD_FINISH, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_Finish },
	// lobbymain.cpp:751
	{ "ROOMLIST", FRCMD_DESTROY, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_Destroy },
	// lobbymain.cpp:753
	{ "ROOMLIST.roomlist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomListInit },
	// lobbymain.cpp:754
	{ "ROOMLIST.roomlist", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomListOwnerDraw },
	// lobbymain.cpp:755
	{ "ROOMLIST.roomlist", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_RoomListLDown },
	// lobbymain.cpp:756
	{ "ROOMLIST.roomlist", FRCMD_RBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_RoomListRUp },
	// lobbymain.cpp:757
	{ "ROOMLIST.roomlist", FRCMD_DBLCLICK, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_RoomListDClick },
	// lobbymain.cpp:759
	{ "ROOMLIST.room_no", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomSortNoInit },
	// lobbymain.cpp:760
	{ "ROOMLIST.room_no", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_RoomSortNoDown },
	// lobbymain.cpp:761
	{ "ROOMLIST.room_no", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomSortNoOwnerDraw },
	// lobbymain.cpp:763
	{ "ROOMLIST.btn_cate_total", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomSortCateAll_BtnInit },
	// lobbymain.cpp:764
	{ "ROOMLIST.btn_cate_total", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_RoomSortCateAll_BtnDown },
	// lobbymain.cpp:765
	{ "ROOMLIST.btn_cate_vs", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomSortCateVS_BtnInit },
	// lobbymain.cpp:766
	{ "ROOMLIST.btn_cate_vs", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_RoomSortCateVS_BtnDown },
	// lobbymain.cpp:767
	{ "ROOMLIST.btn_cate_mass", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomSortCateMass_BtnInit },
	// lobbymain.cpp:768
	{ "ROOMLIST.btn_cate_mass", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_RoomSortCateMass_BtnDown },
	// lobbymain.cpp:769
	{ "ROOMLIST.btn_cate_battle", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomSortCateBattle_BtnInit },
	// lobbymain.cpp:770
	{ "ROOMLIST.btn_cate_battle", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_RoomSortCateBattle_BtnDown },
	// lobbymain.cpp:771
	{ "ROOMLIST.btn_cate_chat", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomSortCateChat_BtnInit },
	// lobbymain.cpp:772
	{ "ROOMLIST.btn_cate_chat", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_RoomSortCateChat_BtnDown },
	// lobbymain.cpp:780
	{ "ROOMLIST.room_hole", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomSortHoleInit },
	// lobbymain.cpp:781
	{ "ROOMLIST.room_hole", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_RoomSortHoleDown },
	// lobbymain.cpp:782
	{ "ROOMLIST.room_hole", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomSortHoleOwnerDraw },
	// lobbymain.cpp:783
	{ "ROOMLIST.room_course", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomSortCourseInit },
	// lobbymain.cpp:784
	{ "ROOMLIST.room_course", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_RoomSortCourseDown },
	// lobbymain.cpp:785
	{ "ROOMLIST.room_course", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomSortCourseOwnerDraw },
	// lobbymain.cpp:786
	{ "ROOMLIST.room_state", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomSortStateInit },
	// lobbymain.cpp:787
	{ "ROOMLIST.room_state", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_RoomSortStateDown },
	// lobbymain.cpp:788
	{ "ROOMLIST.room_state", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomSortStateOwnerDraw },
	// lobbymain.cpp:790
	{ "ROOMLIST.cate_vs_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomSortCateVS_ListInit },
	// lobbymain.cpp:791
	{ "ROOMLIST.cate_vs_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_RoomSortCateVS_ListLUp },
	// lobbymain.cpp:792
	{ "ROOMLIST.cate_vs_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomSortCateVS_ListOwnerDraw },
	// lobbymain.cpp:793
	{ "ROOMLIST.cate_mass_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomSortCateMass_ListInit },
	// lobbymain.cpp:794
	{ "ROOMLIST.cate_mass_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_RoomSortCateMass_ListLUp },
	// lobbymain.cpp:795
	{ "ROOMLIST.cate_mass_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomSortCateMass_ListOwnerDraw },
	// lobbymain.cpp:796
	{ "ROOMLIST.cate_battle_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomSortCateBattle_ListInit },
	// lobbymain.cpp:797
	{ "ROOMLIST.cate_battle_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_RoomSortCateBattle_ListLUp },
	// lobbymain.cpp:798
	{ "ROOMLIST.cate_battle_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_RoomSortCateBattle_ListOwnerDraw },
	// lobbymain.cpp:801
	{ "ROOMLIST.chatbg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatBgInit },
	// lobbymain.cpp:802
	{ "ROOMLIST.chatbg", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatBgOwnerDraw },
	// lobbymain.cpp:803
	{ "ROOMLIST.chatwndbtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatWndBtnInit },
	// lobbymain.cpp:804
	{ "ROOMLIST.chatwndbtn", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnChatWndBtnUp },
	// lobbymain.cpp:805
	{ "ROOMLIST.emoticon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnEmoticonInit },
	// lobbymain.cpp:806
	{ "ROOMLIST.emoticon", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnEmoticonBtnUp },
	// lobbymain.cpp:807
	{ "ROOMLIST.language", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnLanguageInit },
	// lobbymain.cpp:808
	{ "ROOMLIST.chatinput", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatInputInit },
	// lobbymain.cpp:809
	{ "ROOMLIST.chatinput", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&CLobbyMain::OnChatInputEnterKey },
	// lobbymain.cpp:810
	{ "ROOMLIST.chattarget", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatTargetInit },
	// lobbymain.cpp:811
	{ "ROOMLIST.chattarget", FRCMD_LBUTTONDOWN, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatTargetBtnDown },
	// lobbymain.cpp:812
	{ "ROOMLIST.chattarget", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&CLobbyMain::OnChatTargetEnterKey },
	// lobbymain.cpp:813
	{ "ROOMLIST.chatview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_ChatViewInit },
	// lobbymain.cpp:814
	{ "ROOMLIST.report", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnReportBtnUp },
	// lobbymain.cpp:820
	{ "ROOMLIST.makeroom", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_MakeRoomInit },
	// lobbymain.cpp:821
	{ "ROOMLIST.makeroom", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_MakeRoomUp },
	// lobbymain.cpp:824
	{ "ROOMLIST.guildinfo", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_GuildInfoInit },
	// lobbymain.cpp:825
	{ "ROOMLIST.guildinfo", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_GuildInfoLButtonUp },
	// lobbymain.cpp:836
	{ "ROOMLIST.userlist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_UserListInit },
	// lobbymain.cpp:837
	{ "ROOMLIST.userlist", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_UserListLBtnUp },
	// lobbymain.cpp:838
	{ "ROOMLIST.userlist", FRCMD_RBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_UserListRBtnUp },
	// lobbymain.cpp:839
	{ "ROOMLIST.userlist", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_UserListOwnerDraw },
	// lobbymain.cpp:841
	{ "ROOMLIST.user_gender", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_UserSortGenderInit },
	// lobbymain.cpp:842
	{ "ROOMLIST.user_gender", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_UserSortGenderUp },
	// lobbymain.cpp:843
	{ "ROOMLIST.user_gender", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_UserSortGenderOwnerDraw },
	// lobbymain.cpp:844
	{ "ROOMLIST.user_level", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_UserSortLevelInit },
	// lobbymain.cpp:845
	{ "ROOMLIST.user_level", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_UserSortLevelUp },
	// lobbymain.cpp:846
	{ "ROOMLIST.user_level", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_UserSortLevelOwnerDraw },
	// lobbymain.cpp:852
	{ "ROOMLIST.user_guild", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_UserSortGuildInit },
	// lobbymain.cpp:853
	{ "ROOMLIST.user_guild", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_UserSortGuildUp },
	// lobbymain.cpp:854
	{ "ROOMLIST.user_guild", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_UserSortGuildOwnerDraw },
	// lobbymain.cpp:855
	{ "ROOMLIST.user_nick", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_UserSortNickInit },
	// lobbymain.cpp:856
	{ "ROOMLIST.user_nick", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnRoomList_UserSortNickUp },
	// lobbymain.cpp:857
	{ "ROOMLIST.user_nick", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_UserSortNickOwnerDraw },
	// lobbymain.cpp:859
	{ "ROOMLIST.ubar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBarInit },
	// lobbymain.cpp:860
	{ "ROOMLIST.ubar.back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_BackInit },
	// lobbymain.cpp:861
	{ "ROOMLIST.ubar.back", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_BackUp },
	// lobbymain.cpp:862
	{ "ROOMLIST.ubar.back", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:863
	{ "ROOMLIST.ubar.back", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:864
	{ "ROOMLIST.ubar.front", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_FrontInit },
	// lobbymain.cpp:865
	{ "ROOMLIST.ubar.front", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_FrontUp },
	// lobbymain.cpp:866
	{ "ROOMLIST.ubar.front", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:867
	{ "ROOMLIST.ubar.front", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:868
	{ "ROOMLIST.ubar.shop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ShopInit },
	// lobbymain.cpp:869
	{ "ROOMLIST.ubar.shop", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ShopUp },
	// lobbymain.cpp:870
	{ "ROOMLIST.ubar.shop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:871
	{ "ROOMLIST.ubar.shop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:872
	{ "ROOMLIST.ubar.myroom", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MyRoomInit },
	// lobbymain.cpp:873
	{ "ROOMLIST.ubar.myroom", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MyRoomUp },
	// lobbymain.cpp:874
	{ "ROOMLIST.ubar.myroom", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:875
	{ "ROOMLIST.ubar.myroom", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:876
	{ "ROOMLIST.ubar.itemstorage", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ItemStorageInit },
	// lobbymain.cpp:877
	{ "ROOMLIST.ubar.itemstorage", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ItemStorageUp },
	// lobbymain.cpp:878
	{ "ROOMLIST.ubar.itemstorage", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:879
	{ "ROOMLIST.ubar.itemstorage", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:880
	{ "ROOMLIST.ubar.gift", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GiftInit },
	// lobbymain.cpp:881
	{ "ROOMLIST.ubar.gift", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GiftUp },
	// lobbymain.cpp:882
	{ "ROOMLIST.ubar.gift", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:883
	{ "ROOMLIST.ubar.gift", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:884
	{ "ROOMLIST.ubar.cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_CookieInit },
	// lobbymain.cpp:885
	{ "ROOMLIST.ubar.cookie", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_CookieUp },
	// lobbymain.cpp:886
	{ "ROOMLIST.ubar.cookie", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:887
	{ "ROOMLIST.ubar.cookie", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:888
	{ "ROOMLIST.ubar.game", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GameInit },
	// lobbymain.cpp:889
	{ "ROOMLIST.ubar.game", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GameUp },
	// lobbymain.cpp:890
	{ "ROOMLIST.ubar.game", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:891
	{ "ROOMLIST.ubar.game", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:892
	{ "ROOMLIST.ubar.server", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ServerInit },
	// lobbymain.cpp:893
	{ "ROOMLIST.ubar.server", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ServerUp },
	// lobbymain.cpp:894
	{ "ROOMLIST.ubar.server", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:895
	{ "ROOMLIST.ubar.server", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:896
	{ "ROOMLIST.ubar.exit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ExitInit },
	// lobbymain.cpp:897
	{ "ROOMLIST.ubar.exit", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ExitUp },
	// lobbymain.cpp:898
	{ "ROOMLIST.ubar.exit", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:899
	{ "ROOMLIST.ubar.exit", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:900
	{ "ROOMLIST.ubar.tabbtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_TabBtnInit },
	// lobbymain.cpp:901
	{ "ROOMLIST.ubar.tabbtn", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_TabBtnUp },
	// lobbymain.cpp:904
	{ "ROOMLIST.utab", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderTab_Init },
	// lobbymain.cpp:906
	{ "ROOMLIST.utab.magicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MagicBoxInit },
	// lobbymain.cpp:907
	{ "ROOMLIST.utab.magicbox", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MagicBoxUp },
	// lobbymain.cpp:908
	{ "ROOMLIST.utab.magicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:909
	{ "ROOMLIST.utab.magicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:911
	{ "ROOMLIST.utab.tikimagicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_TikiMagicBoxInit },
	// lobbymain.cpp:912
	{ "ROOMLIST.utab.tikimagicbox", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_TikiMagicBoxUp },
	// lobbymain.cpp:913
	{ "ROOMLIST.utab.tikimagicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:914
	{ "ROOMLIST.utab.tikimagicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:936
	{ "ROOMLIST.utab.scratch", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ScratchInit },
	// lobbymain.cpp:937
	{ "ROOMLIST.utab.scratch", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ScratchUp },
	// lobbymain.cpp:938
	{ "ROOMLIST.utab.scratch", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:939
	{ "ROOMLIST.utab.scratch", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:941
	{ "ROOMLIST.utab.bongdarishop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_BongdariShopInit },
	// lobbymain.cpp:942
	{ "ROOMLIST.utab.bongdarishop", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_BongdariShopUp },
	// lobbymain.cpp:943
	{ "ROOMLIST.utab.bongdarishop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:944
	{ "ROOMLIST.utab.bongdarishop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:945
	{ "ROOMLIST.utab.ranking", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_RankingInit },
	// lobbymain.cpp:946
	{ "ROOMLIST.utab.ranking", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_RankingUp },
	// lobbymain.cpp:947
	{ "ROOMLIST.utab.ranking", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:948
	{ "ROOMLIST.utab.ranking", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:949
	{ "ROOMLIST.utab.guild", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GuildInit },
	// lobbymain.cpp:950
	{ "ROOMLIST.utab.guild", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GuildUp },
	// lobbymain.cpp:951
	{ "ROOMLIST.utab.guild", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:952
	{ "ROOMLIST.utab.guild", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:953
	{ "ROOMLIST.utab.notice", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_NoticeInit },
	// lobbymain.cpp:954
	{ "ROOMLIST.utab.notice", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_NoticeUp },
	// lobbymain.cpp:955
	{ "ROOMLIST.utab.notice", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:956
	{ "ROOMLIST.utab.notice", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:959
	{ "ROOMLIST.utab.maket", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MaketInit },
	// lobbymain.cpp:960
	{ "ROOMLIST.utab.maket", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MaketUp },
	// lobbymain.cpp:961
	{ "ROOMLIST.utab.maket", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:962
	{ "ROOMLIST.utab.maket", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:973
	{ "ROOMLIST.obar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_Init },
	// lobbymain.cpp:974
	{ "ROOMLIST.obar.title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_TitleInit },
	// lobbymain.cpp:975
	{ "ROOMLIST.obar.site", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_SiteInit },
	// lobbymain.cpp:976
	{ "ROOMLIST.obar.char", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharInit },
	// lobbymain.cpp:977
	{ "ROOMLIST.obar.char", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_CharDown },
	// lobbymain.cpp:978
	{ "ROOMLIST.obar.char", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharOwnerDraw },
	// lobbymain.cpp:979
	{ "ROOMLIST.obar.caddie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieInit },
	// lobbymain.cpp:980
	{ "ROOMLIST.obar.caddie", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_CaddieDown },
	// lobbymain.cpp:981
	{ "ROOMLIST.obar.caddie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieOwnerDraw },
	// lobbymain.cpp:982
	{ "ROOMLIST.obar.club", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubInit },
	// lobbymain.cpp:983
	{ "ROOMLIST.obar.club", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_ClubDown },
	// lobbymain.cpp:984
	{ "ROOMLIST.obar.club", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubOwnerDraw },
	// lobbymain.cpp:985
	{ "ROOMLIST.obar.aztec", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecInit },
	// lobbymain.cpp:986
	{ "ROOMLIST.obar.aztec", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_AztecDown },
	// lobbymain.cpp:987
	{ "ROOMLIST.obar.aztec", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecOwnerDraw },
	// lobbymain.cpp:988
	{ "ROOMLIST.obar.mascot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotInit },
	// lobbymain.cpp:989
	{ "ROOMLIST.obar.mascot", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MascotDown },
	// lobbymain.cpp:990
	{ "ROOMLIST.obar.mascot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotOwnerDraw },
	// lobbymain.cpp:991
	{ "ROOMLIST.obar.char_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharSelInit },
	// lobbymain.cpp:992
	{ "ROOMLIST.obar.char_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharSelOwnerDraw },
	// lobbymain.cpp:993
	{ "ROOMLIST.obar.caddie_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieSelInit },
	// lobbymain.cpp:994
	{ "ROOMLIST.obar.caddie_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieSelOwnerDraw },
	// lobbymain.cpp:995
	{ "ROOMLIST.obar.club_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubSelInit },
	// lobbymain.cpp:996
	{ "ROOMLIST.obar.club_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubSelOwnerDraw },
	// lobbymain.cpp:997
	{ "ROOMLIST.obar.aztec_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecSelInit },
	// lobbymain.cpp:998
	{ "ROOMLIST.obar.aztec_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecSelOwnerDraw },
	// lobbymain.cpp:999
	{ "ROOMLIST.obar.mascot_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotSelInit },
	// lobbymain.cpp:1000
	{ "ROOMLIST.obar.mascot_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotSelOwnerDraw },
	// lobbymain.cpp:1001
	{ "ROOMLIST.obar.pang_cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_PangCookieInit },
	// lobbymain.cpp:1002
	{ "ROOMLIST.obar.pang_cookie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_PangCookieOwnerDraw },
	// lobbymain.cpp:1003
	{ "ROOMLIST.obar.onelinemsg_req", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineReqBtnInit },
	// lobbymain.cpp:1004
	{ "ROOMLIST.obar.onelinemsg_req", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_OneLineReqBtnUp },
	// lobbymain.cpp:1005
	{ "ROOMLIST.obar.onelineview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineViewInit },
	// lobbymain.cpp:1006
	{ "ROOMLIST.obar.onelineview", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineViewOwnerDraw },
	// lobbymain.cpp:1007
	{ "ROOMLIST.obar.spcard", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_SpecialCardInit },
	// lobbymain.cpp:1008
	{ "ROOMLIST.obar.spcard", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_SpecialCardUp },
	// lobbymain.cpp:1009
	{ "ROOMLIST.obar.spcard", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1010
	{ "ROOMLIST.obar.spcard", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1011
	{ "ROOMLIST.obar.messenger", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MessengerInit },
	// lobbymain.cpp:1012
	{ "ROOMLIST.obar.messenger", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MessengerUp },
	// lobbymain.cpp:1013
	{ "ROOMLIST.obar.messenger", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1014
	{ "ROOMLIST.obar.messenger", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1015
	{ "ROOMLIST.obar.myinfo", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MyInfoInit },
	// lobbymain.cpp:1016
	{ "ROOMLIST.obar.myinfo", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MyInfoUp },
	// lobbymain.cpp:1017
	{ "ROOMLIST.obar.myinfo", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1018
	{ "ROOMLIST.obar.myinfo", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1019
	{ "ROOMLIST.obar.option", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OptionInit },
	// lobbymain.cpp:1020
	{ "ROOMLIST.obar.option", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_OptionUp },
	// lobbymain.cpp:1021
	{ "ROOMLIST.obar.option", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1022
	{ "ROOMLIST.obar.option", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1024
	{ "ROOMLIST.obar.messenger_bubble", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleInit },
	// lobbymain.cpp:1025
	{ "ROOMLIST.obar.messenger_bubble", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleOwnerDraw },
	// lobbymain.cpp:1026
	{ "ROOMLIST.obar.messenger_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleEditInit },
	// lobbymain.cpp:1027
	{ "ROOMLIST.obar.messenger_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerAlarmBtnInit },
	// lobbymain.cpp:1028
	{ "ROOMLIST.obar.messenger_alarm", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnMessengerAlarmBtnUp },
	// lobbymain.cpp:1029
	{ "ROOMLIST.obar.gift_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerGiftAlarmBtnInit },
	// lobbymain.cpp:1030
	{ "ROOMLIST.obar.gift_alarm", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnMessengerGiftAlarmBtnUp },
	// lobbymain.cpp:1031
	{ "ROOMLIST.obar.tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarTooltipOwnerDraw },
	// lobbymain.cpp:1032
	{ "ROOMLIST.treasure_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnTreasureAlarmInit },
	// lobbymain.cpp:1033
	{ "ROOMLIST.treasure_alarm", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnTreasureAlarmUp },
	// lobbymain.cpp:1036
	{ "GAMEROOM", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_Init },
	// lobbymain.cpp:1037
	{ "GAMEROOM", FRCMD_FINISH, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_Finish },
	// lobbymain.cpp:1038
	{ "GAMEROOM", FRCMD_DESTROY, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_Destroy },
	// lobbymain.cpp:1041
	{ "GAMEROOM.title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_TitleInit },
	// lobbymain.cpp:1043
	{ "GAMEROOM.background", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_BackGroundInit },
	// lobbymain.cpp:1044
	{ "GAMEROOM.pet0", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_Pet0Init },
	// lobbymain.cpp:1045
	{ "GAMEROOM.pet1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_Pet1Init },
	// lobbymain.cpp:1046
	{ "GAMEROOM.pet2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_Pet2Init },
	// lobbymain.cpp:1047
	{ "GAMEROOM.pet3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_Pet3Init },
	// lobbymain.cpp:1048
	{ "GAMEROOM.user", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_RoomUserInit },
	// lobbymain.cpp:1049
	{ "GAMEROOM.user", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_RoomUserOwnerDraw },
	// lobbymain.cpp:1050
	{ "GAMEROOM.user", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_RoomUserLBtnUp },
	// lobbymain.cpp:1051
	{ "GAMEROOM.user", FRCMD_RBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_RoomUserRBtnUp },
	// lobbymain.cpp:1052
	{ "GAMEROOM.user", FRCMD_DBLCLICK, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_RoomUserDClick },
	// lobbymain.cpp:1054
	{ "GAMEROOM.chatbg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatBgInit },
	// lobbymain.cpp:1055
	{ "GAMEROOM.chatbg", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatBgOwnerDraw },
	// lobbymain.cpp:1056
	{ "GAMEROOM.chatwndbtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatWndBtnInit },
	// lobbymain.cpp:1057
	{ "GAMEROOM.chatwndbtn", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnChatWndBtnUp },
	// lobbymain.cpp:1058
	{ "GAMEROOM.emoticon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnEmoticonInit },
	// lobbymain.cpp:1059
	{ "GAMEROOM.emoticon", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnEmoticonBtnUp },
	// lobbymain.cpp:1060
	{ "GAMEROOM.language", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnLanguageInit },
	// lobbymain.cpp:1061
	{ "GAMEROOM.chatinput", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatInputInit },
	// lobbymain.cpp:1062
	{ "GAMEROOM.chatinput", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&CLobbyMain::OnChatInputEnterKey },
	// lobbymain.cpp:1063
	{ "GAMEROOM.chattarget", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatTargetInit },
	// lobbymain.cpp:1064
	{ "GAMEROOM.chattarget", FRCMD_LBUTTONDOWN, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatTargetBtnDown },
	// lobbymain.cpp:1065
	{ "GAMEROOM.chattarget", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&CLobbyMain::OnChatTargetEnterKey },
	// lobbymain.cpp:1066
	{ "GAMEROOM.chatview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_ChatViewInit },
	// lobbymain.cpp:1067
	{ "GAMEROOM.report", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnReportBtnUp },
	// lobbymain.cpp:1069
	{ "GAMEROOM.map_background", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MapBgInit },
	// lobbymain.cpp:1070
	{ "GAMEROOM.map", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MapInit },
	// lobbymain.cpp:1071
	{ "GAMEROOM.map", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_MapBtnUp },
	// lobbymain.cpp:1072
	{ "GAMEROOM.mapprev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MapPrevInit },
	// lobbymain.cpp:1073
	{ "GAMEROOM.mapnext", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MapNextInit },
	// lobbymain.cpp:1074
	{ "GAMEROOM.mapprev", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_MapPrevBtnUp },
	// lobbymain.cpp:1075
	{ "GAMEROOM.mapnext", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_MapNextBtnUp },
	// lobbymain.cpp:1076
	{ "GAMEROOM.mapcomboex", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MapHoleComboInit },
	// lobbymain.cpp:1077
	{ "GAMEROOM.mapcomboex", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_MapHoleComboDown },
	// lobbymain.cpp:1078
	{ "GAMEROOM.overgauge", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MapGaugeInit },
	// lobbymain.cpp:1079
	{ "GAMEROOM.maplock", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MapLockInit },
	// lobbymain.cpp:1081
	{ "GAMEROOM.start_tip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_StartTipInit },
	// lobbymain.cpp:1082
	{ "GAMEROOM.ready_tip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_ReadyTipInit },
	// lobbymain.cpp:1083
	{ "GAMEROOM.start", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_StartInit },
	// lobbymain.cpp:1084
	{ "GAMEROOM.start", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_StartBtnUp },
	// lobbymain.cpp:1086
	{ "GAMEROOM.ready", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_ReadyInit },
	// lobbymain.cpp:1087
	{ "GAMEROOM.ready", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_ReadyBtnUp },
	// lobbymain.cpp:1089
	{ "GAMEROOM.change_team", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_ChangeTeamBtnInit },
	// lobbymain.cpp:1090
	{ "GAMEROOM.change_team", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_ChangeTeamBtnUp },
	// lobbymain.cpp:1091
	{ "GAMEROOM.invite", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_UserListInit },
	// lobbymain.cpp:1092
	{ "GAMEROOM.invite", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_UserListBtnUp },
	// lobbymain.cpp:1093
	{ "GAMEROOM.modify", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_RoomOptionInit },
	// lobbymain.cpp:1094
	{ "GAMEROOM.modify", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_RoomOptionBtnUp },
	// lobbymain.cpp:1096
	{ "GAMEROOM.powerbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_PowerBarInit },
	// lobbymain.cpp:1097
	{ "GAMEROOM.controlbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_ControlBarInit },
	// lobbymain.cpp:1098
	{ "GAMEROOM.accuracybar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_AccuracyBarInit },
	// lobbymain.cpp:1099
	{ "GAMEROOM.spinbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_SpinBarInit },
	// lobbymain.cpp:1100
	{ "GAMEROOM.curvebar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CurveBarInit },
	// lobbymain.cpp:1101
	{ "GAMEROOM.epower", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_PowerEditInit },
	// lobbymain.cpp:1102
	{ "GAMEROOM.econtrol", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_ControlEditInit },
	// lobbymain.cpp:1103
	{ "GAMEROOM.eaccuracy", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_AccuracyEditInit },
	// lobbymain.cpp:1104
	{ "GAMEROOM.espin", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_SpinEditInit },
	// lobbymain.cpp:1105
	{ "GAMEROOM.ecurve", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CurveEditInit },
	// lobbymain.cpp:1108
	{ "GAMEROOM.cur_char", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CharInit },
	// lobbymain.cpp:1109
	{ "GAMEROOM.cur_char", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_CharDown },
	// lobbymain.cpp:1110
	{ "GAMEROOM.cur_char", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CharOwnerDraw },
	// lobbymain.cpp:1111
	{ "GAMEROOM.cur_caddie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CaddieInit },
	// lobbymain.cpp:1112
	{ "GAMEROOM.cur_caddie", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_CaddieDown },
	// lobbymain.cpp:1113
	{ "GAMEROOM.cur_caddie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CaddieOwnerDraw },
	// lobbymain.cpp:1114
	{ "GAMEROOM.cur_club", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_ClubInit },
	// lobbymain.cpp:1115
	{ "GAMEROOM.cur_club", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_ClubDown },
	// lobbymain.cpp:1116
	{ "GAMEROOM.cur_club", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_ClubOwnerDraw },
	// lobbymain.cpp:1117
	{ "GAMEROOM.cur_aztec", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_AztecInit },
	// lobbymain.cpp:1118
	{ "GAMEROOM.cur_aztec", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_AztecDown },
	// lobbymain.cpp:1119
	{ "GAMEROOM.cur_aztec", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_AztecOwnerDraw },
	// lobbymain.cpp:1120
	{ "GAMEROOM.cur_mascot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MascotInit },
	// lobbymain.cpp:1121
	{ "GAMEROOM.cur_mascot", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_MascotDown },
	// lobbymain.cpp:1122
	{ "GAMEROOM.cur_mascot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MascotOwnerDraw },
	// lobbymain.cpp:1123
	{ "GAMEROOM.cur_equip_item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_EquipItemInit },
	// lobbymain.cpp:1124
	{ "GAMEROOM.cur_equip_item", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_EquipItemDown },
	// lobbymain.cpp:1125
	{ "GAMEROOM.cur_equip_item", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_EquipItemOwnerDraw },
	// lobbymain.cpp:1127
	{ "GAMEROOM.cur_char_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CharSelInit },
	// lobbymain.cpp:1128
	{ "GAMEROOM.cur_char_sel", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_CharSelDown },
	// lobbymain.cpp:1129
	{ "GAMEROOM.cur_char_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CharSelOwnerDraw },
	// lobbymain.cpp:1130
	{ "GAMEROOM.cur_caddie_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CaddieSelInit },
	// lobbymain.cpp:1131
	{ "GAMEROOM.cur_caddie_sel", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_CaddieSelDown },
	// lobbymain.cpp:1132
	{ "GAMEROOM.cur_caddie_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CaddieSelOwnerDraw },
	// lobbymain.cpp:1133
	{ "GAMEROOM.cur_club_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_ClubSelInit },
	// lobbymain.cpp:1134
	{ "GAMEROOM.cur_club_sel", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_ClubSelDown },
	// lobbymain.cpp:1135
	{ "GAMEROOM.cur_club_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_ClubSelOwnerDraw },
	// lobbymain.cpp:1136
	{ "GAMEROOM.cur_aztec_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_AztecSelInit },
	// lobbymain.cpp:1137
	{ "GAMEROOM.cur_aztec_sel", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_AztecSelDown },
	// lobbymain.cpp:1138
	{ "GAMEROOM.cur_aztec_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_AztecSelOwnerDraw },
	// lobbymain.cpp:1139
	{ "GAMEROOM.cur_mascot_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MascotSelInit },
	// lobbymain.cpp:1140
	{ "GAMEROOM.cur_mascot_sel", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_MascotSelDown },
	// lobbymain.cpp:1141
	{ "GAMEROOM.cur_mascot_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MascotSelOwnerDraw },
	// lobbymain.cpp:1143
	{ "GAMEROOM.ubar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBarInit },
	// lobbymain.cpp:1144
	{ "GAMEROOM.ubar.back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_BackInit },
	// lobbymain.cpp:1145
	{ "GAMEROOM.ubar.back", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_BackUp },
	// lobbymain.cpp:1146
	{ "GAMEROOM.ubar.back", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1147
	{ "GAMEROOM.ubar.back", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1148
	{ "GAMEROOM.ubar.front", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_FrontInit },
	// lobbymain.cpp:1149
	{ "GAMEROOM.ubar.front", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_FrontUp },
	// lobbymain.cpp:1150
	{ "GAMEROOM.ubar.front", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1151
	{ "GAMEROOM.ubar.front", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1152
	{ "GAMEROOM.ubar.shop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ShopInit },
	// lobbymain.cpp:1153
	{ "GAMEROOM.ubar.shop", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ShopUp },
	// lobbymain.cpp:1154
	{ "GAMEROOM.ubar.shop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1155
	{ "GAMEROOM.ubar.shop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1156
	{ "GAMEROOM.ubar.myroom", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MyRoomInit },
	// lobbymain.cpp:1157
	{ "GAMEROOM.ubar.myroom", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MyRoomUp },
	// lobbymain.cpp:1158
	{ "GAMEROOM.ubar.myroom", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1159
	{ "GAMEROOM.ubar.myroom", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1160
	{ "GAMEROOM.ubar.itemstorage", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ItemStorageInit },
	// lobbymain.cpp:1161
	{ "GAMEROOM.ubar.itemstorage", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ItemStorageUp },
	// lobbymain.cpp:1162
	{ "GAMEROOM.ubar.itemstorage", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1163
	{ "GAMEROOM.ubar.itemstorage", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1164
	{ "GAMEROOM.ubar.gift", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GiftInit },
	// lobbymain.cpp:1165
	{ "GAMEROOM.ubar.gift", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GiftUp },
	// lobbymain.cpp:1166
	{ "GAMEROOM.ubar.gift", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1167
	{ "GAMEROOM.ubar.gift", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1168
	{ "GAMEROOM.ubar.cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_CookieInit },
	// lobbymain.cpp:1169
	{ "GAMEROOM.ubar.cookie", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_CookieUp },
	// lobbymain.cpp:1170
	{ "GAMEROOM.ubar.cookie", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1171
	{ "GAMEROOM.ubar.cookie", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1172
	{ "GAMEROOM.ubar.game", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GameInit },
	// lobbymain.cpp:1173
	{ "GAMEROOM.ubar.game", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GameUp },
	// lobbymain.cpp:1174
	{ "GAMEROOM.ubar.game", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1175
	{ "GAMEROOM.ubar.game", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1176
	{ "GAMEROOM.ubar.server", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ServerInit },
	// lobbymain.cpp:1177
	{ "GAMEROOM.ubar.server", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ServerUp },
	// lobbymain.cpp:1178
	{ "GAMEROOM.ubar.server", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1179
	{ "GAMEROOM.ubar.server", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1180
	{ "GAMEROOM.ubar.exit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ExitInit },
	// lobbymain.cpp:1181
	{ "GAMEROOM.ubar.exit", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ExitUp },
	// lobbymain.cpp:1182
	{ "GAMEROOM.ubar.exit", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1183
	{ "GAMEROOM.ubar.exit", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1184
	{ "GAMEROOM.ubar.tabbtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_TabBtnInit },
	// lobbymain.cpp:1185
	{ "GAMEROOM.ubar.tabbtn", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_TabBtnUp },
	// lobbymain.cpp:1188
	{ "GAMEROOM.utab", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderTab_Init },
	// lobbymain.cpp:1190
	{ "GAMEROOM.utab.magicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MagicBoxInit },
	// lobbymain.cpp:1191
	{ "GAMEROOM.utab.magicbox", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MagicBoxUp },
	// lobbymain.cpp:1192
	{ "GAMEROOM.utab.magicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1193
	{ "GAMEROOM.utab.magicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1195
	{ "GAMEROOM.utab.tikimagicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_TikiMagicBoxInit },
	// lobbymain.cpp:1196
	{ "GAMEROOM.utab.tikimagicbox", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_TikiMagicBoxUp },
	// lobbymain.cpp:1197
	{ "GAMEROOM.utab.tikimagicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1198
	{ "GAMEROOM.utab.tikimagicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1220
	{ "GAMEROOM.utab.scratch", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ScratchInit },
	// lobbymain.cpp:1221
	{ "GAMEROOM.utab.scratch", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ScratchUp },
	// lobbymain.cpp:1222
	{ "GAMEROOM.utab.scratch", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1223
	{ "GAMEROOM.utab.scratch", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1225
	{ "GAMEROOM.utab.bongdarishop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_BongdariShopInit },
	// lobbymain.cpp:1226
	{ "GAMEROOM.utab.bongdarishop", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_BongdariShopUp },
	// lobbymain.cpp:1227
	{ "GAMEROOM.utab.bongdarishop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1228
	{ "GAMEROOM.utab.bongdarishop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1229
	{ "GAMEROOM.utab.ranking", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_RankingInit },
	// lobbymain.cpp:1230
	{ "GAMEROOM.utab.ranking", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_RankingUp },
	// lobbymain.cpp:1231
	{ "GAMEROOM.utab.ranking", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1232
	{ "GAMEROOM.utab.ranking", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1233
	{ "GAMEROOM.utab.guild", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GuildInit },
	// lobbymain.cpp:1234
	{ "GAMEROOM.utab.guild", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GuildUp },
	// lobbymain.cpp:1235
	{ "GAMEROOM.utab.guild", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1236
	{ "GAMEROOM.utab.guild", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1237
	{ "GAMEROOM.utab.notice", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_NoticeInit },
	// lobbymain.cpp:1238
	{ "GAMEROOM.utab.notice", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_NoticeUp },
	// lobbymain.cpp:1239
	{ "GAMEROOM.utab.notice", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1240
	{ "GAMEROOM.utab.notice", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1243
	{ "GAMEROOM.utab.maket", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MaketInit },
	// lobbymain.cpp:1244
	{ "GAMEROOM.utab.maket", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MaketUp },
	// lobbymain.cpp:1245
	{ "GAMEROOM.utab.maket", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1246
	{ "GAMEROOM.utab.maket", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1257
	{ "GAMEROOM.obar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_Init },
	// lobbymain.cpp:1258
	{ "GAMEROOM.obar.title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_TitleInit },
	// lobbymain.cpp:1259
	{ "GAMEROOM.obar.site", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_SiteInit },
	// lobbymain.cpp:1260
	{ "GAMEROOM.obar.char", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharInit },
	// lobbymain.cpp:1261
	{ "GAMEROOM.obar.char", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_CharDown },
	// lobbymain.cpp:1262
	{ "GAMEROOM.obar.char", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharOwnerDraw },
	// lobbymain.cpp:1263
	{ "GAMEROOM.obar.caddie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieInit },
	// lobbymain.cpp:1264
	{ "GAMEROOM.obar.caddie", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_CaddieDown },
	// lobbymain.cpp:1265
	{ "GAMEROOM.obar.caddie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieOwnerDraw },
	// lobbymain.cpp:1266
	{ "GAMEROOM.obar.club", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubInit },
	// lobbymain.cpp:1267
	{ "GAMEROOM.obar.club", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_ClubDown },
	// lobbymain.cpp:1268
	{ "GAMEROOM.obar.club", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubOwnerDraw },
	// lobbymain.cpp:1269
	{ "GAMEROOM.obar.aztec", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecInit },
	// lobbymain.cpp:1270
	{ "GAMEROOM.obar.aztec", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_AztecDown },
	// lobbymain.cpp:1271
	{ "GAMEROOM.obar.aztec", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecOwnerDraw },
	// lobbymain.cpp:1272
	{ "GAMEROOM.obar.mascot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotInit },
	// lobbymain.cpp:1273
	{ "GAMEROOM.obar.mascot", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MascotDown },
	// lobbymain.cpp:1274
	{ "GAMEROOM.obar.mascot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotOwnerDraw },
	// lobbymain.cpp:1275
	{ "GAMEROOM.obar.char_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharSelInit },
	// lobbymain.cpp:1276
	{ "GAMEROOM.obar.char_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharSelOwnerDraw },
	// lobbymain.cpp:1277
	{ "GAMEROOM.obar.caddie_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieSelInit },
	// lobbymain.cpp:1278
	{ "GAMEROOM.obar.caddie_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieSelOwnerDraw },
	// lobbymain.cpp:1279
	{ "GAMEROOM.obar.club_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubSelInit },
	// lobbymain.cpp:1280
	{ "GAMEROOM.obar.club_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubSelOwnerDraw },
	// lobbymain.cpp:1281
	{ "GAMEROOM.obar.aztec_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecSelInit },
	// lobbymain.cpp:1282
	{ "GAMEROOM.obar.aztec_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecSelOwnerDraw },
	// lobbymain.cpp:1283
	{ "GAMEROOM.obar.mascot_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotSelInit },
	// lobbymain.cpp:1284
	{ "GAMEROOM.obar.mascot_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotSelOwnerDraw },
	// lobbymain.cpp:1285
	{ "GAMEROOM.obar.pang_cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_PangCookieInit },
	// lobbymain.cpp:1286
	{ "GAMEROOM.obar.pang_cookie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_PangCookieOwnerDraw },
	// lobbymain.cpp:1287
	{ "GAMEROOM.obar.onelinemsg_req", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineReqBtnInit },
	// lobbymain.cpp:1288
	{ "GAMEROOM.obar.onelinemsg_req", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_OneLineReqBtnUp },
	// lobbymain.cpp:1289
	{ "GAMEROOM.obar.onelineview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineViewInit },
	// lobbymain.cpp:1290
	{ "GAMEROOM.obar.onelineview", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineViewOwnerDraw },
	// lobbymain.cpp:1291
	{ "GAMEROOM.obar.spcard", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_SpecialCardInit },
	// lobbymain.cpp:1292
	{ "GAMEROOM.obar.spcard", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_SpecialCardUp },
	// lobbymain.cpp:1293
	{ "GAMEROOM.obar.spcard", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1294
	{ "GAMEROOM.obar.spcard", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1295
	{ "GAMEROOM.obar.messenger", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MessengerInit },
	// lobbymain.cpp:1296
	{ "GAMEROOM.obar.messenger", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MessengerUp },
	// lobbymain.cpp:1297
	{ "GAMEROOM.obar.messenger", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1298
	{ "GAMEROOM.obar.messenger", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1299
	{ "GAMEROOM.obar.myinfo", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MyInfoInit },
	// lobbymain.cpp:1300
	{ "GAMEROOM.obar.myinfo", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MyInfoUp },
	// lobbymain.cpp:1301
	{ "GAMEROOM.obar.myinfo", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1302
	{ "GAMEROOM.obar.myinfo", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1303
	{ "GAMEROOM.obar.option", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OptionInit },
	// lobbymain.cpp:1304
	{ "GAMEROOM.obar.option", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_OptionUp },
	// lobbymain.cpp:1305
	{ "GAMEROOM.obar.option", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1306
	{ "GAMEROOM.obar.option", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1308
	{ "GAMEROOM.obar.messenger_bubble", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleInit },
	// lobbymain.cpp:1309
	{ "GAMEROOM.obar.messenger_bubble", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleOwnerDraw },
	// lobbymain.cpp:1310
	{ "GAMEROOM.obar.messenger_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleEditInit },
	// lobbymain.cpp:1311
	{ "GAMEROOM.obar.messenger_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerAlarmBtnInit },
	// lobbymain.cpp:1312
	{ "GAMEROOM.obar.messenger_alarm", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnMessengerAlarmBtnUp },
	// lobbymain.cpp:1313
	{ "GAMEROOM.obar.gift_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerGiftAlarmBtnInit },
	// lobbymain.cpp:1314
	{ "GAMEROOM.obar.gift_alarm", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnMessengerGiftAlarmBtnUp },
	// lobbymain.cpp:1315
	{ "GAMEROOM.obar.tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarTooltipOwnerDraw },
	// lobbymain.cpp:1316
	{ "GAMEROOM.treasure_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnTreasureAlarmInit },
	// lobbymain.cpp:1317
	{ "GAMEROOM.treasure_alarm", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnTreasureAlarmUp },
	// lobbymain.cpp:1320
	{ "GAMEROOM_EXT", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_Init },
	// lobbymain.cpp:1321
	{ "GAMEROOM_EXT", FRCMD_FINISH, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoomExt_Finish },
	// lobbymain.cpp:1322
	{ "GAMEROOM_EXT", FRCMD_DESTROY, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoomExt_Destroy },
	// lobbymain.cpp:1324
	{ "GAMEROOM_EXT.title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_TitleInit },
	// lobbymain.cpp:1325
	{ "GAMEROOM_EXT.room_event", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_EventOwnerDraw },
	// lobbymain.cpp:1327
	{ "GAMEROOM_EXT.user", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_RoomUserInit },
	// lobbymain.cpp:1328
	{ "GAMEROOM_EXT.user", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_RoomUserOwnerDraw },
	// lobbymain.cpp:1329
	{ "GAMEROOM_EXT.user", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_RoomUserLBtnUp },
	// lobbymain.cpp:1330
	{ "GAMEROOM_EXT.user", FRCMD_RBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_RoomUserRBtnUp },
	// lobbymain.cpp:1331
	{ "GAMEROOM_EXT.user", FRCMD_DBLCLICK, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_RoomUserDClick },
	// lobbymain.cpp:1332
	{ "GAMEROOM_EXT.30_team1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_TeamTab1Init },
	// lobbymain.cpp:1333
	{ "GAMEROOM_EXT.30_team2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_TeamTab2Init },
	// lobbymain.cpp:1334
	{ "GAMEROOM_EXT.30_guild1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_GuildTab1Init },
	// lobbymain.cpp:1335
	{ "GAMEROOM_EXT.30_guild2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_GuildTab2Init },
	// lobbymain.cpp:1336
	{ "GAMEROOM_EXT.30_team_num", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_TeamNumInit },
	// lobbymain.cpp:1337
	{ "GAMEROOM_EXT.30_team_num", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_TeamNumOwnerDraw },
	// lobbymain.cpp:1338
	{ "GAMEROOM_EXT.30_guild_num", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_GuildNumInit },
	// lobbymain.cpp:1339
	{ "GAMEROOM_EXT.30_guild_num", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_GuildNumOwnerDraw },
	// lobbymain.cpp:1341
	{ "GAMEROOM_EXT.chatbg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatBgInit },
	// lobbymain.cpp:1342
	{ "GAMEROOM_EXT.chatbg", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatBgOwnerDraw },
	// lobbymain.cpp:1343
	{ "GAMEROOM_EXT.chatwndbtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatWndBtnInit },
	// lobbymain.cpp:1344
	{ "GAMEROOM_EXT.chatwndbtn", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnChatWndBtnUp },
	// lobbymain.cpp:1345
	{ "GAMEROOM_EXT.emoticon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnEmoticonInit },
	// lobbymain.cpp:1346
	{ "GAMEROOM_EXT.emoticon", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnEmoticonBtnUp },
	// lobbymain.cpp:1347
	{ "GAMEROOM_EXT.language", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnLanguageInit },
	// lobbymain.cpp:1348
	{ "GAMEROOM_EXT.chatinput", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatInputInit },
	// lobbymain.cpp:1349
	{ "GAMEROOM_EXT.chatinput", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&CLobbyMain::OnChatInputEnterKey },
	// lobbymain.cpp:1350
	{ "GAMEROOM_EXT.chattarget", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatTargetInit },
	// lobbymain.cpp:1351
	{ "GAMEROOM_EXT.chattarget", FRCMD_LBUTTONDOWN, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatTargetBtnDown },
	// lobbymain.cpp:1352
	{ "GAMEROOM_EXT.chattarget", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&CLobbyMain::OnChatTargetEnterKey },
	// lobbymain.cpp:1353
	{ "GAMEROOM_EXT.chatview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_ChatViewInit },
	// lobbymain.cpp:1354
	{ "GAMEROOM_EXT.report", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnReportBtnUp },
	// lobbymain.cpp:1356
	{ "GAMEROOM_EXT.map_background", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MapBgInit },
	// lobbymain.cpp:1357
	{ "GAMEROOM_EXT.map", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MapInit },
	// lobbymain.cpp:1358
	{ "GAMEROOM_EXT.map", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_MapBtnUp },
	// lobbymain.cpp:1359
	{ "GAMEROOM_EXT.mapprev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MapPrevInit },
	// lobbymain.cpp:1360
	{ "GAMEROOM_EXT.mapnext", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MapNextInit },
	// lobbymain.cpp:1361
	{ "GAMEROOM_EXT.mapprev", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_MapPrevBtnUp },
	// lobbymain.cpp:1362
	{ "GAMEROOM_EXT.mapnext", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_MapNextBtnUp },
	// lobbymain.cpp:1363
	{ "GAMEROOM_EXT.mapcomboex", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MapHoleComboInit },
	// lobbymain.cpp:1364
	{ "GAMEROOM_EXT.mapcomboex", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_MapHoleComboDown },
	// lobbymain.cpp:1366
	{ "GAMEROOM_EXT.overgauge", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MapGaugeInit },
	// lobbymain.cpp:1367
	{ "GAMEROOM_EXT.maplock", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MapLockInit },
	// lobbymain.cpp:1369
	{ "GAMEROOM_EXT.start_tip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_StartTipInit },
	// lobbymain.cpp:1370
	{ "GAMEROOM_EXT.ready_tip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_ReadyTipInit },
	// lobbymain.cpp:1371
	{ "GAMEROOM_EXT.start", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_StartInit },
	// lobbymain.cpp:1372
	{ "GAMEROOM_EXT.start", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_StartBtnUp },
	// lobbymain.cpp:1374
	{ "GAMEROOM_EXT.ready", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_ReadyInit },
	// lobbymain.cpp:1375
	{ "GAMEROOM_EXT.ready", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_ReadyBtnUp },
	// lobbymain.cpp:1377
	{ "GAMEROOM_EXT.change_team", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_ChangeTeamBtnInit },
	// lobbymain.cpp:1378
	{ "GAMEROOM_EXT.change_team", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_ChangeTeamBtnUp },
	// lobbymain.cpp:1379
	{ "GAMEROOM_EXT.invite", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_GuildInviteInit },
	// lobbymain.cpp:1380
	{ "GAMEROOM_EXT.invite", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoomExt_GuildInviteLBtnUp },
	// lobbymain.cpp:1381
	{ "GAMEROOM_EXT.modify", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_RoomOptionInit },
	// lobbymain.cpp:1382
	{ "GAMEROOM_EXT.modify", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_RoomOptionBtnUp },
	// lobbymain.cpp:1384
	{ "GAMEROOM_EXT.powerbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_PowerBarInit },
	// lobbymain.cpp:1385
	{ "GAMEROOM_EXT.controlbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_ControlBarInit },
	// lobbymain.cpp:1386
	{ "GAMEROOM_EXT.accuracybar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_AccuracyBarInit },
	// lobbymain.cpp:1387
	{ "GAMEROOM_EXT.spinbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_SpinBarInit },
	// lobbymain.cpp:1388
	{ "GAMEROOM_EXT.curvebar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CurveBarInit },
	// lobbymain.cpp:1389
	{ "GAMEROOM_EXT.epower", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_PowerEditInit },
	// lobbymain.cpp:1390
	{ "GAMEROOM_EXT.econtrol", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_ControlEditInit },
	// lobbymain.cpp:1391
	{ "GAMEROOM_EXT.eaccuracy", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_AccuracyEditInit },
	// lobbymain.cpp:1392
	{ "GAMEROOM_EXT.espin", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_SpinEditInit },
	// lobbymain.cpp:1393
	{ "GAMEROOM_EXT.ecurve", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CurveEditInit },
	// lobbymain.cpp:1395
	{ "GAMEROOM_EXT.cur_char", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CharInit },
	// lobbymain.cpp:1396
	{ "GAMEROOM_EXT.cur_char", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_CharDown },
	// lobbymain.cpp:1397
	{ "GAMEROOM_EXT.cur_char", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CharOwnerDraw },
	// lobbymain.cpp:1398
	{ "GAMEROOM_EXT.cur_caddie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CaddieInit },
	// lobbymain.cpp:1399
	{ "GAMEROOM_EXT.cur_caddie", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_CaddieDown },
	// lobbymain.cpp:1400
	{ "GAMEROOM_EXT.cur_caddie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CaddieOwnerDraw },
	// lobbymain.cpp:1401
	{ "GAMEROOM_EXT.cur_club", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_ClubInit },
	// lobbymain.cpp:1402
	{ "GAMEROOM_EXT.cur_club", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_ClubDown },
	// lobbymain.cpp:1403
	{ "GAMEROOM_EXT.cur_club", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_ClubOwnerDraw },
	// lobbymain.cpp:1404
	{ "GAMEROOM_EXT.cur_aztec", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_AztecInit },
	// lobbymain.cpp:1405
	{ "GAMEROOM_EXT.cur_aztec", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_AztecDown },
	// lobbymain.cpp:1406
	{ "GAMEROOM_EXT.cur_aztec", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_AztecOwnerDraw },
	// lobbymain.cpp:1407
	{ "GAMEROOM_EXT.cur_mascot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MascotInit },
	// lobbymain.cpp:1408
	{ "GAMEROOM_EXT.cur_mascot", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_MascotDown },
	// lobbymain.cpp:1409
	{ "GAMEROOM_EXT.cur_mascot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MascotOwnerDraw },
	// lobbymain.cpp:1410
	{ "GAMEROOM_EXT.cur_equip_item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_EquipItemInit },
	// lobbymain.cpp:1411
	{ "GAMEROOM_EXT.cur_equip_item", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_EquipItemDown },
	// lobbymain.cpp:1412
	{ "GAMEROOM_EXT.cur_equip_item", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_EquipItemOwnerDraw },
	// lobbymain.cpp:1414
	{ "GAMEROOM_EXT.cur_char_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CharSelInit },
	// lobbymain.cpp:1415
	{ "GAMEROOM_EXT.cur_char_sel", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_CharSelDown },
	// lobbymain.cpp:1416
	{ "GAMEROOM_EXT.cur_char_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CharSelOwnerDraw },
	// lobbymain.cpp:1417
	{ "GAMEROOM_EXT.cur_caddie_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CaddieSelInit },
	// lobbymain.cpp:1418
	{ "GAMEROOM_EXT.cur_caddie_sel", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_CaddieSelDown },
	// lobbymain.cpp:1419
	{ "GAMEROOM_EXT.cur_caddie_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_CaddieSelOwnerDraw },
	// lobbymain.cpp:1420
	{ "GAMEROOM_EXT.cur_club_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_ClubSelInit },
	// lobbymain.cpp:1421
	{ "GAMEROOM_EXT.cur_club_sel", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_ClubSelDown },
	// lobbymain.cpp:1422
	{ "GAMEROOM_EXT.cur_club_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_ClubSelOwnerDraw },
	// lobbymain.cpp:1423
	{ "GAMEROOM_EXT.cur_aztec_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_AztecSelInit },
	// lobbymain.cpp:1424
	{ "GAMEROOM_EXT.cur_aztec_sel", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_AztecSelDown },
	// lobbymain.cpp:1425
	{ "GAMEROOM_EXT.cur_aztec_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_AztecSelOwnerDraw },
	// lobbymain.cpp:1426
	{ "GAMEROOM_EXT.cur_mascot_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MascotSelInit },
	// lobbymain.cpp:1427
	{ "GAMEROOM_EXT.cur_mascot_sel", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_MascotSelDown },
	// lobbymain.cpp:1428
	{ "GAMEROOM_EXT.cur_mascot_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_MascotSelOwnerDraw },
	// lobbymain.cpp:1430
	{ "GAMEROOM_EXT.ubar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBarInit },
	// lobbymain.cpp:1431
	{ "GAMEROOM_EXT.ubar.back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_BackInit },
	// lobbymain.cpp:1432
	{ "GAMEROOM_EXT.ubar.back", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_BackUp },
	// lobbymain.cpp:1433
	{ "GAMEROOM_EXT.ubar.back", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1434
	{ "GAMEROOM_EXT.ubar.back", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1435
	{ "GAMEROOM_EXT.ubar.front", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_FrontInit },
	// lobbymain.cpp:1436
	{ "GAMEROOM_EXT.ubar.front", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_FrontUp },
	// lobbymain.cpp:1437
	{ "GAMEROOM_EXT.ubar.front", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1438
	{ "GAMEROOM_EXT.ubar.front", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1439
	{ "GAMEROOM_EXT.ubar.shop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ShopInit },
	// lobbymain.cpp:1440
	{ "GAMEROOM_EXT.ubar.shop", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ShopUp },
	// lobbymain.cpp:1441
	{ "GAMEROOM_EXT.ubar.shop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1442
	{ "GAMEROOM_EXT.ubar.shop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1443
	{ "GAMEROOM_EXT.ubar.myroom", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MyRoomInit },
	// lobbymain.cpp:1444
	{ "GAMEROOM_EXT.ubar.myroom", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MyRoomUp },
	// lobbymain.cpp:1445
	{ "GAMEROOM_EXT.ubar.myroom", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1446
	{ "GAMEROOM_EXT.ubar.myroom", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1447
	{ "GAMEROOM_EXT.ubar.itemstorage", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ItemStorageInit },
	// lobbymain.cpp:1448
	{ "GAMEROOM_EXT.ubar.itemstorage", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ItemStorageUp },
	// lobbymain.cpp:1449
	{ "GAMEROOM_EXT.ubar.itemstorage", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1450
	{ "GAMEROOM_EXT.ubar.itemstorage", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1451
	{ "GAMEROOM_EXT.ubar.gift", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GiftInit },
	// lobbymain.cpp:1452
	{ "GAMEROOM_EXT.ubar.gift", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GiftUp },
	// lobbymain.cpp:1453
	{ "GAMEROOM_EXT.ubar.gift", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1454
	{ "GAMEROOM_EXT.ubar.gift", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1455
	{ "GAMEROOM_EXT.ubar.cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_CookieInit },
	// lobbymain.cpp:1456
	{ "GAMEROOM_EXT.ubar.cookie", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_CookieUp },
	// lobbymain.cpp:1457
	{ "GAMEROOM_EXT.ubar.cookie", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1458
	{ "GAMEROOM_EXT.ubar.cookie", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1459
	{ "GAMEROOM_EXT.ubar.game", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GameInit },
	// lobbymain.cpp:1460
	{ "GAMEROOM_EXT.ubar.game", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GameUp },
	// lobbymain.cpp:1461
	{ "GAMEROOM_EXT.ubar.game", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1462
	{ "GAMEROOM_EXT.ubar.game", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1463
	{ "GAMEROOM_EXT.ubar.server", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ServerInit },
	// lobbymain.cpp:1464
	{ "GAMEROOM_EXT.ubar.server", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ServerUp },
	// lobbymain.cpp:1465
	{ "GAMEROOM_EXT.ubar.server", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1466
	{ "GAMEROOM_EXT.ubar.server", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1467
	{ "GAMEROOM_EXT.ubar.exit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ExitInit },
	// lobbymain.cpp:1468
	{ "GAMEROOM_EXT.ubar.exit", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ExitUp },
	// lobbymain.cpp:1469
	{ "GAMEROOM_EXT.ubar.exit", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1470
	{ "GAMEROOM_EXT.ubar.exit", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1471
	{ "GAMEROOM_EXT.ubar.tabbtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_TabBtnInit },
	// lobbymain.cpp:1472
	{ "GAMEROOM_EXT.ubar.tabbtn", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_TabBtnUp },
	// lobbymain.cpp:1475
	{ "GAMEROOM_EXT.utab", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderTab_Init },
	// lobbymain.cpp:1477
	{ "GAMEROOM_EXT.utab.magicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MagicBoxInit },
	// lobbymain.cpp:1478
	{ "GAMEROOM_EXT.utab.magicbox", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MagicBoxUp },
	// lobbymain.cpp:1479
	{ "GAMEROOM_EXT.utab.magicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1480
	{ "GAMEROOM_EXT.utab.magicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1482
	{ "GAMEROOM_EXT.utab.tikimagicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_TikiMagicBoxInit },
	// lobbymain.cpp:1483
	{ "GAMEROOM_EXT.utab.tikimagicbox", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_TikiMagicBoxUp },
	// lobbymain.cpp:1484
	{ "GAMEROOM_EXT.utab.tikimagicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1485
	{ "GAMEROOM_EXT.utab.tikimagicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1507
	{ "GAMEROOM_EXT.utab.scratch", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ScratchInit },
	// lobbymain.cpp:1508
	{ "GAMEROOM_EXT.utab.scratch", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ScratchUp },
	// lobbymain.cpp:1509
	{ "GAMEROOM_EXT.utab.scratch", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1510
	{ "GAMEROOM_EXT.utab.scratch", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1512
	{ "GAMEROOM_EXT.utab.bongdarishop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_BongdariShopInit },
	// lobbymain.cpp:1513
	{ "GAMEROOM_EXT.utab.bongdarishop", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_BongdariShopUp },
	// lobbymain.cpp:1514
	{ "GAMEROOM_EXT.utab.bongdarishop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1515
	{ "GAMEROOM_EXT.utab.bongdarishop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1516
	{ "GAMEROOM_EXT.utab.ranking", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_RankingInit },
	// lobbymain.cpp:1517
	{ "GAMEROOM_EXT.utab.ranking", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_RankingUp },
	// lobbymain.cpp:1518
	{ "GAMEROOM_EXT.utab.ranking", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1519
	{ "GAMEROOM_EXT.utab.ranking", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1520
	{ "GAMEROOM_EXT.utab.guild", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GuildInit },
	// lobbymain.cpp:1521
	{ "GAMEROOM_EXT.utab.guild", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GuildUp },
	// lobbymain.cpp:1522
	{ "GAMEROOM_EXT.utab.guild", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1523
	{ "GAMEROOM_EXT.utab.guild", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1524
	{ "GAMEROOM_EXT.utab.notice", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_NoticeInit },
	// lobbymain.cpp:1525
	{ "GAMEROOM_EXT.utab.notice", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_NoticeUp },
	// lobbymain.cpp:1526
	{ "GAMEROOM_EXT.utab.notice", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1527
	{ "GAMEROOM_EXT.utab.notice", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1530
	{ "GAMEROOM_EXT.utab.maket", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MaketInit },
	// lobbymain.cpp:1531
	{ "GAMEROOM_EXT.utab.maket", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MaketUp },
	// lobbymain.cpp:1532
	{ "GAMEROOM_EXT.utab.maket", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1533
	{ "GAMEROOM_EXT.utab.maket", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1544
	{ "GAMEROOM_EXT.obar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_Init },
	// lobbymain.cpp:1545
	{ "GAMEROOM_EXT.obar.title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_TitleInit },
	// lobbymain.cpp:1546
	{ "GAMEROOM_EXT.obar.site", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_SiteInit },
	// lobbymain.cpp:1547
	{ "GAMEROOM_EXT.obar.char", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharInit },
	// lobbymain.cpp:1548
	{ "GAMEROOM_EXT.obar.char", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_CharDown },
	// lobbymain.cpp:1549
	{ "GAMEROOM_EXT.obar.char", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharOwnerDraw },
	// lobbymain.cpp:1550
	{ "GAMEROOM_EXT.obar.caddie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieInit },
	// lobbymain.cpp:1551
	{ "GAMEROOM_EXT.obar.caddie", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_CaddieDown },
	// lobbymain.cpp:1552
	{ "GAMEROOM_EXT.obar.caddie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieOwnerDraw },
	// lobbymain.cpp:1553
	{ "GAMEROOM_EXT.obar.club", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubInit },
	// lobbymain.cpp:1554
	{ "GAMEROOM_EXT.obar.club", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_ClubDown },
	// lobbymain.cpp:1555
	{ "GAMEROOM_EXT.obar.club", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubOwnerDraw },
	// lobbymain.cpp:1556
	{ "GAMEROOM_EXT.obar.aztec", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecInit },
	// lobbymain.cpp:1557
	{ "GAMEROOM_EXT.obar.aztec", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_AztecDown },
	// lobbymain.cpp:1558
	{ "GAMEROOM_EXT.obar.aztec", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecOwnerDraw },
	// lobbymain.cpp:1559
	{ "GAMEROOM_EXT.obar.mascot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotInit },
	// lobbymain.cpp:1560
	{ "GAMEROOM_EXT.obar.mascot", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MascotDown },
	// lobbymain.cpp:1561
	{ "GAMEROOM_EXT.obar.mascot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotOwnerDraw },
	// lobbymain.cpp:1562
	{ "GAMEROOM_EXT.obar.char_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharSelInit },
	// lobbymain.cpp:1563
	{ "GAMEROOM_EXT.obar.char_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharSelOwnerDraw },
	// lobbymain.cpp:1564
	{ "GAMEROOM_EXT.obar.caddie_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieSelInit },
	// lobbymain.cpp:1565
	{ "GAMEROOM_EXT.obar.caddie_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieSelOwnerDraw },
	// lobbymain.cpp:1566
	{ "GAMEROOM_EXT.obar.club_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubSelInit },
	// lobbymain.cpp:1567
	{ "GAMEROOM_EXT.obar.club_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubSelOwnerDraw },
	// lobbymain.cpp:1568
	{ "GAMEROOM_EXT.obar.aztec_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecSelInit },
	// lobbymain.cpp:1569
	{ "GAMEROOM_EXT.obar.aztec_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecSelOwnerDraw },
	// lobbymain.cpp:1570
	{ "GAMEROOM_EXT.obar.mascot_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotSelInit },
	// lobbymain.cpp:1571
	{ "GAMEROOM_EXT.obar.mascot_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotSelOwnerDraw },
	// lobbymain.cpp:1572
	{ "GAMEROOM_EXT.obar.pang_cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_PangCookieInit },
	// lobbymain.cpp:1573
	{ "GAMEROOM_EXT.obar.pang_cookie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_PangCookieOwnerDraw },
	// lobbymain.cpp:1574
	{ "GAMEROOM_EXT.obar.onelinemsg_req", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineReqBtnInit },
	// lobbymain.cpp:1575
	{ "GAMEROOM_EXT.obar.onelinemsg_req", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_OneLineReqBtnUp },
	// lobbymain.cpp:1576
	{ "GAMEROOM_EXT.obar.onelineview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineViewInit },
	// lobbymain.cpp:1577
	{ "GAMEROOM_EXT.obar.onelineview", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineViewOwnerDraw },
	// lobbymain.cpp:1578
	{ "GAMEROOM_EXT.obar.spcard", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_SpecialCardInit },
	// lobbymain.cpp:1579
	{ "GAMEROOM_EXT.obar.spcard", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_SpecialCardUp },
	// lobbymain.cpp:1580
	{ "GAMEROOM_EXT.obar.spcard", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1581
	{ "GAMEROOM_EXT.obar.spcard", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1582
	{ "GAMEROOM_EXT.obar.messenger", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MessengerInit },
	// lobbymain.cpp:1583
	{ "GAMEROOM_EXT.obar.messenger", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MessengerUp },
	// lobbymain.cpp:1584
	{ "GAMEROOM_EXT.obar.messenger", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1585
	{ "GAMEROOM_EXT.obar.messenger", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1586
	{ "GAMEROOM_EXT.obar.myinfo", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MyInfoInit },
	// lobbymain.cpp:1587
	{ "GAMEROOM_EXT.obar.myinfo", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MyInfoUp },
	// lobbymain.cpp:1588
	{ "GAMEROOM_EXT.obar.myinfo", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1589
	{ "GAMEROOM_EXT.obar.myinfo", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1590
	{ "GAMEROOM_EXT.obar.option", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OptionInit },
	// lobbymain.cpp:1591
	{ "GAMEROOM_EXT.obar.option", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_OptionUp },
	// lobbymain.cpp:1592
	{ "GAMEROOM_EXT.obar.option", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1593
	{ "GAMEROOM_EXT.obar.option", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1594
	{ "GAMEROOM_EXT.countdown", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_CountdownInit },
	// lobbymain.cpp:1596
	{ "GAMEROOM_EXT.obar.messenger_bubble", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleInit },
	// lobbymain.cpp:1597
	{ "GAMEROOM_EXT.obar.messenger_bubble", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleOwnerDraw },
	// lobbymain.cpp:1598
	{ "GAMEROOM_EXT.obar.messenger_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleEditInit },
	// lobbymain.cpp:1599
	{ "GAMEROOM_EXT.obar.messenger_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerAlarmBtnInit },
	// lobbymain.cpp:1600
	{ "GAMEROOM_EXT.obar.messenger_alarm", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnMessengerAlarmBtnUp },
	// lobbymain.cpp:1601
	{ "GAMEROOM_EXT.obar.gift_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerGiftAlarmBtnInit },
	// lobbymain.cpp:1602
	{ "GAMEROOM_EXT.obar.gift_alarm", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnMessengerGiftAlarmBtnUp },
	// lobbymain.cpp:1603
	{ "GAMEROOM_EXT.obar.tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarTooltipOwnerDraw },
	// lobbymain.cpp:1604
	{ "GAMEROOM_EXT.treasure_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnTreasureAlarmInit },
	// lobbymain.cpp:1605
	{ "GAMEROOM_EXT.treasure_alarm", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnTreasureAlarmUp },
	// lobbymain.cpp:1608
	{ "GAMEROOM_EXTRES", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExtRes_Init },
	// lobbymain.cpp:1609
	{ "GAMEROOM_EXTRES", FRCMD_FINISH, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoomExtRes_Finish },
	// lobbymain.cpp:1610
	{ "GAMEROOM_EXTRES", FRCMD_DESTROY, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoomExtRes_Destroy },
	// lobbymain.cpp:1613
	{ "GAMEROOM_EXTRES.title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_TitleInit },
	// lobbymain.cpp:1614
	{ "GAMEROOM_EXTRES.room_event", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_EventOwnerDraw },
	// lobbymain.cpp:1616
	{ "GAMEROOM_EXTRES.user", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_RoomUserInit },
	// lobbymain.cpp:1617
	{ "GAMEROOM_EXTRES.user", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_RoomUserOwnerDraw },
	// lobbymain.cpp:1618
	{ "GAMEROOM_EXTRES.user", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_RoomUserLBtnUp },
	// lobbymain.cpp:1619
	{ "GAMEROOM_EXTRES.user", FRCMD_RBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_RoomUserRBtnUp },
	// lobbymain.cpp:1620
	{ "GAMEROOM_EXTRES.user", FRCMD_DBLCLICK, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_RoomUserDClick },
	// lobbymain.cpp:1621
	{ "GAMEROOM_EXTRES.30_team1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_TeamTab1Init },
	// lobbymain.cpp:1622
	{ "GAMEROOM_EXTRES.30_team2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_TeamTab2Init },
	// lobbymain.cpp:1623
	{ "GAMEROOM_EXTRES.30_guild1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_GuildTab1Init },
	// lobbymain.cpp:1624
	{ "GAMEROOM_EXTRES.30_guild2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_GuildTab2Init },
	// lobbymain.cpp:1625
	{ "GAMEROOM_EXTRES.30_team_num", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_TeamNumInit },
	// lobbymain.cpp:1626
	{ "GAMEROOM_EXTRES.30_team_num", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_TeamNumOwnerDraw },
	// lobbymain.cpp:1627
	{ "GAMEROOM_EXTRES.30_guild_num", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_GuildNumInit },
	// lobbymain.cpp:1628
	{ "GAMEROOM_EXTRES.30_guild_num", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_GuildNumOwnerDraw },
	// lobbymain.cpp:1629
	{ "GAMEROOM_EXTRES.score_gb", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExtRes_GuildScoreGaugeInit },
	// lobbymain.cpp:1631
	{ "GAMEROOM_EXTRES.chatbg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatBgInit },
	// lobbymain.cpp:1632
	{ "GAMEROOM_EXTRES.chatbg", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatBgOwnerDraw },
	// lobbymain.cpp:1633
	{ "GAMEROOM_EXTRES.chatwndbtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatWndBtnInit },
	// lobbymain.cpp:1634
	{ "GAMEROOM_EXTRES.chatwndbtn", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnChatWndBtnUp },
	// lobbymain.cpp:1635
	{ "GAMEROOM_EXTRES.emoticon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnEmoticonInit },
	// lobbymain.cpp:1636
	{ "GAMEROOM_EXTRES.emoticon", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnEmoticonBtnUp },
	// lobbymain.cpp:1637
	{ "GAMEROOM_EXTRES.language", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnLanguageInit },
	// lobbymain.cpp:1638
	{ "GAMEROOM_EXTRES.chatinput", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatInputInit },
	// lobbymain.cpp:1639
	{ "GAMEROOM_EXTRES.chatinput", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&CLobbyMain::OnChatInputEnterKey },
	// lobbymain.cpp:1640
	{ "GAMEROOM_EXTRES.chattarget", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatTargetInit },
	// lobbymain.cpp:1641
	{ "GAMEROOM_EXTRES.chattarget", FRCMD_LBUTTONDOWN, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChatTargetBtnDown },
	// lobbymain.cpp:1642
	{ "GAMEROOM_EXTRES.chattarget", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&CLobbyMain::OnChatTargetEnterKey },
	// lobbymain.cpp:1643
	{ "GAMEROOM_EXTRES.chatview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnRoomList_ChatViewInit },
	// lobbymain.cpp:1644
	{ "GAMEROOM_EXTRES.report", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnReportBtnUp },
	// lobbymain.cpp:1646
	{ "GAMEROOM_EXTRES.rankframe", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_FrameInit },
	// lobbymain.cpp:1647
	{ "GAMEROOM_EXTRES.rankingtab", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExtRes_RoomUserRankTabInit },
	// lobbymain.cpp:1648
	{ "GAMEROOM_EXTRES.ranking", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExtRes_RoomUserRankInit },
	// lobbymain.cpp:1649
	{ "GAMEROOM_EXTRES.ranking", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExtRes_RoomUserRankOwnerDraw },
	// lobbymain.cpp:1650
	{ "GAMEROOM_EXTRES.ranking", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoomExtRes_RoomUserRankLBtnUp },
	// lobbymain.cpp:1651
	{ "GAMEROOM_EXTRES.ranking", FRCMD_RBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoomExtRes_RoomUserRankRBtnUp },
	// lobbymain.cpp:1652
	{ "GAMEROOM_EXTRES.guildranking", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExtRes_RoomGuildRankInit },
	// lobbymain.cpp:1653
	{ "GAMEROOM_EXTRES.guildranking", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExtRes_RoomGuildRankOwnerDraw },
	// lobbymain.cpp:1654
	{ "GAMEROOM_EXTRES.guildranking", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoomExtRes_RoomGuildRankLBtnUp },
	// lobbymain.cpp:1655
	{ "GAMEROOM_EXTRES.guildranking", FRCMD_RBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoomExtRes_RoomGuildRankRBtnUp },
	// lobbymain.cpp:1656
	{ "GAMEROOM_EXTRES.score", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoom_ScoreInit },
	// lobbymain.cpp:1657
	{ "GAMEROOM_EXTRES.score", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoom_ScoreBtnUp },
	// lobbymain.cpp:1658
	{ "GAMEROOM_EXTRES.user_info", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnGameRoomExt_UserInfoInit },
	// lobbymain.cpp:1659
	{ "GAMEROOM_EXTRES.user_info", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoomExt_UserInfoLBtnUp },
	// lobbymain.cpp:1662
	{ "GAMEROOM_EXTRES.ubar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBarInit },
	// lobbymain.cpp:1663
	{ "GAMEROOM_EXTRES.ubar.back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_BackInit },
	// lobbymain.cpp:1664
	{ "GAMEROOM_EXTRES.ubar.back", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_BackUp },
	// lobbymain.cpp:1665
	{ "GAMEROOM_EXTRES.ubar.back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnChannelRoomCloseBtnInit },
	// lobbymain.cpp:1666
	{ "GAMEROOM_EXTRES.ubar.back", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnGameRoomExt_ResCloseBtnUp },
	// lobbymain.cpp:1667
	{ "GAMEROOM_EXTRES.ubar.back", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1668
	{ "GAMEROOM_EXTRES.ubar.back", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1669
	{ "GAMEROOM_EXTRES.ubar.front", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_FrontInit },
	// lobbymain.cpp:1670
	{ "GAMEROOM_EXTRES.ubar.front", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_FrontUp },
	// lobbymain.cpp:1671
	{ "GAMEROOM_EXTRES.ubar.front", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1672
	{ "GAMEROOM_EXTRES.ubar.front", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1673
	{ "GAMEROOM_EXTRES.ubar.shop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ShopInit },
	// lobbymain.cpp:1674
	{ "GAMEROOM_EXTRES.ubar.shop", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ShopUp },
	// lobbymain.cpp:1675
	{ "GAMEROOM_EXTRES.ubar.shop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1676
	{ "GAMEROOM_EXTRES.ubar.shop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1677
	{ "GAMEROOM_EXTRES.ubar.myroom", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MyRoomInit },
	// lobbymain.cpp:1678
	{ "GAMEROOM_EXTRES.ubar.myroom", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MyRoomUp },
	// lobbymain.cpp:1679
	{ "GAMEROOM_EXTRES.ubar.myroom", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1680
	{ "GAMEROOM_EXTRES.ubar.myroom", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1681
	{ "GAMEROOM_EXTRES.ubar.itemstorage", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ItemStorageInit },
	// lobbymain.cpp:1682
	{ "GAMEROOM_EXTRES.ubar.itemstorage", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ItemStorageUp },
	// lobbymain.cpp:1683
	{ "GAMEROOM_EXTRES.ubar.itemstorage", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1684
	{ "GAMEROOM_EXTRES.ubar.itemstorage", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1685
	{ "GAMEROOM_EXTRES.ubar.gift", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GiftInit },
	// lobbymain.cpp:1686
	{ "GAMEROOM_EXTRES.ubar.gift", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GiftUp },
	// lobbymain.cpp:1687
	{ "GAMEROOM_EXTRES.ubar.gift", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1688
	{ "GAMEROOM_EXTRES.ubar.gift", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1689
	{ "GAMEROOM_EXTRES.ubar.cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_CookieInit },
	// lobbymain.cpp:1690
	{ "GAMEROOM_EXTRES.ubar.cookie", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_CookieUp },
	// lobbymain.cpp:1691
	{ "GAMEROOM_EXTRES.ubar.cookie", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1692
	{ "GAMEROOM_EXTRES.ubar.cookie", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1693
	{ "GAMEROOM_EXTRES.ubar.game", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GameInit },
	// lobbymain.cpp:1694
	{ "GAMEROOM_EXTRES.ubar.game", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GameUp },
	// lobbymain.cpp:1695
	{ "GAMEROOM_EXTRES.ubar.game", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1696
	{ "GAMEROOM_EXTRES.ubar.game", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1697
	{ "GAMEROOM_EXTRES.ubar.server", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ServerInit },
	// lobbymain.cpp:1698
	{ "GAMEROOM_EXTRES.ubar.server", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ServerUp },
	// lobbymain.cpp:1699
	{ "GAMEROOM_EXTRES.ubar.server", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1700
	{ "GAMEROOM_EXTRES.ubar.server", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1701
	{ "GAMEROOM_EXTRES.ubar.exit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ExitInit },
	// lobbymain.cpp:1702
	{ "GAMEROOM_EXTRES.ubar.exit", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ExitUp },
	// lobbymain.cpp:1703
	{ "GAMEROOM_EXTRES.ubar.exit", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1704
	{ "GAMEROOM_EXTRES.ubar.exit", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1705
	{ "GAMEROOM_EXTRES.ubar.tabbtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_TabBtnInit },
	// lobbymain.cpp:1706
	{ "GAMEROOM_EXTRES.ubar.tabbtn", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_TabBtnUp },
	// lobbymain.cpp:1709
	{ "GAMEROOM_EXTRES.utab", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderTab_Init },
	// lobbymain.cpp:1711
	{ "GAMEROOM_EXTRES.utab.magicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MagicBoxInit },
	// lobbymain.cpp:1712
	{ "GAMEROOM_EXTRES.utab.magicbox", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MagicBoxUp },
	// lobbymain.cpp:1713
	{ "GAMEROOM_EXTRES.utab.magicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1714
	{ "GAMEROOM_EXTRES.utab.magicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1716
	{ "GAMEROOM_EXTRES.utab.tikimagicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_TikiMagicBoxInit },
	// lobbymain.cpp:1717
	{ "GAMEROOM_EXTRES.utab.tikimagicbox", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_TikiMagicBoxUp },
	// lobbymain.cpp:1718
	{ "GAMEROOM_EXTRES.utab.tikimagicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1719
	{ "GAMEROOM_EXTRES.utab.tikimagicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1741
	{ "GAMEROOM_EXTRES.utab.scratch", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ScratchInit },
	// lobbymain.cpp:1742
	{ "GAMEROOM_EXTRES.utab.scratch", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ScratchUp },
	// lobbymain.cpp:1743
	{ "GAMEROOM_EXTRES.utab.scratch", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1744
	{ "GAMEROOM_EXTRES.utab.scratch", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1746
	{ "GAMEROOM_EXTRES.utab.bongdarishop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_BongdariShopInit },
	// lobbymain.cpp:1747
	{ "GAMEROOM_EXTRES.utab.bongdarishop", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_BongdariShopUp },
	// lobbymain.cpp:1748
	{ "GAMEROOM_EXTRES.utab.bongdarishop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1749
	{ "GAMEROOM_EXTRES.utab.bongdarishop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1750
	{ "GAMEROOM_EXTRES.utab.ranking", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_RankingInit },
	// lobbymain.cpp:1751
	{ "GAMEROOM_EXTRES.utab.ranking", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_RankingUp },
	// lobbymain.cpp:1752
	{ "GAMEROOM_EXTRES.utab.ranking", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1753
	{ "GAMEROOM_EXTRES.utab.ranking", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1754
	{ "GAMEROOM_EXTRES.utab.guild", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GuildInit },
	// lobbymain.cpp:1755
	{ "GAMEROOM_EXTRES.utab.guild", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GuildUp },
	// lobbymain.cpp:1756
	{ "GAMEROOM_EXTRES.utab.guild", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1757
	{ "GAMEROOM_EXTRES.utab.guild", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1758
	{ "GAMEROOM_EXTRES.utab.notice", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_NoticeInit },
	// lobbymain.cpp:1759
	{ "GAMEROOM_EXTRES.utab.notice", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_NoticeUp },
	// lobbymain.cpp:1760
	{ "GAMEROOM_EXTRES.utab.notice", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1761
	{ "GAMEROOM_EXTRES.utab.notice", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1764
	{ "GAMEROOM_EXTRES.utab.maket", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MaketInit },
	// lobbymain.cpp:1765
	{ "GAMEROOM_EXTRES.utab.maket", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MaketUp },
	// lobbymain.cpp:1766
	{ "GAMEROOM_EXTRES.utab.maket", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1767
	{ "GAMEROOM_EXTRES.utab.maket", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1778
	{ "GAMEROOM_EXTRES.obar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_Init },
	// lobbymain.cpp:1779
	{ "GAMEROOM_EXTRES.obar.title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_TitleInit },
	// lobbymain.cpp:1780
	{ "GAMEROOM_EXTRES.obar.site", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_SiteInit },
	// lobbymain.cpp:1781
	{ "GAMEROOM_EXTRES.obar.char", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharInit },
	// lobbymain.cpp:1782
	{ "GAMEROOM_EXTRES.obar.char", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_CharDown },
	// lobbymain.cpp:1783
	{ "GAMEROOM_EXTRES.obar.char", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharOwnerDraw },
	// lobbymain.cpp:1784
	{ "GAMEROOM_EXTRES.obar.caddie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieInit },
	// lobbymain.cpp:1785
	{ "GAMEROOM_EXTRES.obar.caddie", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_CaddieDown },
	// lobbymain.cpp:1786
	{ "GAMEROOM_EXTRES.obar.caddie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieOwnerDraw },
	// lobbymain.cpp:1787
	{ "GAMEROOM_EXTRES.obar.club", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubInit },
	// lobbymain.cpp:1788
	{ "GAMEROOM_EXTRES.obar.club", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_ClubDown },
	// lobbymain.cpp:1789
	{ "GAMEROOM_EXTRES.obar.club", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubOwnerDraw },
	// lobbymain.cpp:1790
	{ "GAMEROOM_EXTRES.obar.aztec", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecInit },
	// lobbymain.cpp:1791
	{ "GAMEROOM_EXTRES.obar.aztec", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_AztecDown },
	// lobbymain.cpp:1792
	{ "GAMEROOM_EXTRES.obar.aztec", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecOwnerDraw },
	// lobbymain.cpp:1793
	{ "GAMEROOM_EXTRES.obar.mascot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotInit },
	// lobbymain.cpp:1794
	{ "GAMEROOM_EXTRES.obar.mascot", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MascotDown },
	// lobbymain.cpp:1795
	{ "GAMEROOM_EXTRES.obar.mascot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotOwnerDraw },
	// lobbymain.cpp:1796
	{ "GAMEROOM_EXTRES.obar.char_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharSelInit },
	// lobbymain.cpp:1797
	{ "GAMEROOM_EXTRES.obar.char_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharSelOwnerDraw },
	// lobbymain.cpp:1798
	{ "GAMEROOM_EXTRES.obar.caddie_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieSelInit },
	// lobbymain.cpp:1799
	{ "GAMEROOM_EXTRES.obar.caddie_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieSelOwnerDraw },
	// lobbymain.cpp:1800
	{ "GAMEROOM_EXTRES.obar.club_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubSelInit },
	// lobbymain.cpp:1801
	{ "GAMEROOM_EXTRES.obar.club_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubSelOwnerDraw },
	// lobbymain.cpp:1802
	{ "GAMEROOM_EXTRES.obar.aztec_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecSelInit },
	// lobbymain.cpp:1803
	{ "GAMEROOM_EXTRES.obar.aztec_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecSelOwnerDraw },
	// lobbymain.cpp:1804
	{ "GAMEROOM_EXTRES.obar.mascot_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotSelInit },
	// lobbymain.cpp:1805
	{ "GAMEROOM_EXTRES.obar.mascot_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotSelOwnerDraw },
	// lobbymain.cpp:1806
	{ "GAMEROOM_EXTRES.obar.pang_cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_PangCookieInit },
	// lobbymain.cpp:1807
	{ "GAMEROOM_EXTRES.obar.pang_cookie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_PangCookieOwnerDraw },
	// lobbymain.cpp:1808
	{ "GAMEROOM_EXTRES.obar.onelinemsg_req", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineReqBtnInit },
	// lobbymain.cpp:1809
	{ "GAMEROOM_EXTRES.obar.onelinemsg_req", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_OneLineReqBtnUp },
	// lobbymain.cpp:1810
	{ "GAMEROOM_EXTRES.obar.onelineview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineViewInit },
	// lobbymain.cpp:1811
	{ "GAMEROOM_EXTRES.obar.onelineview", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineViewOwnerDraw },
	// lobbymain.cpp:1812
	{ "GAMEROOM_EXTRES.obar.spcard", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_SpecialCardInit },
	// lobbymain.cpp:1813
	{ "GAMEROOM_EXTRES.obar.spcard", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_SpecialCardUp },
	// lobbymain.cpp:1814
	{ "GAMEROOM_EXTRES.obar.spcard", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1815
	{ "GAMEROOM_EXTRES.obar.spcard", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1816
	{ "GAMEROOM_EXTRES.obar.messenger", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MessengerInit },
	// lobbymain.cpp:1817
	{ "GAMEROOM_EXTRES.obar.messenger", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MessengerUp },
	// lobbymain.cpp:1818
	{ "GAMEROOM_EXTRES.obar.messenger", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1819
	{ "GAMEROOM_EXTRES.obar.messenger", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1820
	{ "GAMEROOM_EXTRES.obar.myinfo", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MyInfoInit },
	// lobbymain.cpp:1821
	{ "GAMEROOM_EXTRES.obar.myinfo", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MyInfoUp },
	// lobbymain.cpp:1822
	{ "GAMEROOM_EXTRES.obar.myinfo", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1823
	{ "GAMEROOM_EXTRES.obar.myinfo", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1824
	{ "GAMEROOM_EXTRES.obar.option", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OptionInit },
	// lobbymain.cpp:1825
	{ "GAMEROOM_EXTRES.obar.option", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_OptionUp },
	// lobbymain.cpp:1826
	{ "GAMEROOM_EXTRES.obar.option", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// lobbymain.cpp:1827
	{ "GAMEROOM_EXTRES.obar.option", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// lobbymain.cpp:1829
	{ "GAMEROOM_EXTRES.obar.messenger_bubble", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleInit },
	// lobbymain.cpp:1830
	{ "GAMEROOM_EXTRES.obar.messenger_bubble", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleOwnerDraw },
	// lobbymain.cpp:1831
	{ "GAMEROOM_EXTRES.obar.messenger_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleEditInit },
	// lobbymain.cpp:1832
	{ "GAMEROOM_EXTRES.obar.messenger_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerAlarmBtnInit },
	// lobbymain.cpp:1833
	{ "GAMEROOM_EXTRES.obar.messenger_alarm", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnMessengerAlarmBtnUp },
	// lobbymain.cpp:1834
	{ "GAMEROOM_EXTRES.obar.gift_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerGiftAlarmBtnInit },
	// lobbymain.cpp:1835
	{ "GAMEROOM_EXTRES.obar.gift_alarm", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnMessengerGiftAlarmBtnUp },
	// lobbymain.cpp:1836
	{ "GAMEROOM_EXTRES.obar.tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarTooltipOwnerDraw },
	// lobbymain.cpp:1838
	{ "GAMEROOM_EXTRES.treasure_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnTreasureAlarmInit },
	{ "GAMEROOM_EXTRES.treasure_alarm", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnTreasureAlarmUp },
	// lobbymain.cpp:1841
	{ "CREATE", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnCreate_Init },
	// lobbymain.cpp:1842
	{ "CREATE", FRCMD_FINISH, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnCreate_Finish },
	// lobbymain.cpp:1843
	{ "CREATE", FRCMD_DESTROY, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnCreate_Destroy },
	// lobbymain.cpp:1845
	{ "CREATE.pet", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnCreatePetInit },
	// lobbymain.cpp:1846
	{ "CREATE.pet", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnCreatePetOwnerDraw },
	// lobbymain.cpp:1847
	{ "CREATE.char", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnCreate_CharInit },
	// lobbymain.cpp:1848
	{ "CREATE.charprev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnCreate_CharPrevInit },
	// lobbymain.cpp:1849
	{ "CREATE.charnext", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnCreate_CharNextInit },
	// lobbymain.cpp:1850
	{ "CREATE.hair", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnCreate_HairInit },
	// lobbymain.cpp:1851
	{ "CREATE.hairprev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnCreate_HairPrevInit },
	// lobbymain.cpp:1852
	{ "CREATE.hairprev", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnCreate_HairPrevBtnUp },
	// lobbymain.cpp:1853
	{ "CREATE.hairnext", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnCreate_HairNextInit },
	// lobbymain.cpp:1854
	{ "CREATE.hairnext", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnCreate_HairNextBtnUp },
	// lobbymain.cpp:1855
	{ "CREATE.shirt", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnCreate_ShirtsInit },
	// lobbymain.cpp:1856
	{ "CREATE.shirtprev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnCreate_ShirtsPrevInit },
	// lobbymain.cpp:1857
	{ "CREATE.shirtprev", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnCreate_ShirtsPrevBtnUp },
	// lobbymain.cpp:1858
	{ "CREATE.shirtnext", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnCreate_ShirtsNextInit },
	// lobbymain.cpp:1859
	{ "CREATE.shirtnext", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnCreate_ShirtsNextBtnUp },
	// lobbymain.cpp:1860
	{ "CREATE.create", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CLobbyMain::OnCreate_CreateBtnUp },
	// lobbymain.cpp:1861
	{ "CREATE.desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnCreate_DescInit },
	// lobbymain.cpp:1862
	{ "CREATE.warning", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::OnCreate_WarningInit },
	// lobbymain.cpp:1863
	{ "CREATE.powerbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::On_PowerBarInit },
	// lobbymain.cpp:1864
	{ "CREATE.controlbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::On_ControlBarInit },
	// lobbymain.cpp:1865
	{ "CREATE.accuratebar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::On_AccuracyBarInit },
	// lobbymain.cpp:1867
	{ "CREATE.spinbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::On_SpinBarInit },
	{ "CREATE.curvebar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CLobbyMain::On_CurveBarInit },
	// lobbymain.cpp:1871
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// lobbymain.cpp:1879
bool FinishUserCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sSlotInfo* pLhs = (const sSlotInfo*)lhs;
	const sSlotInfo* pRhs = (const sSlotInfo*)rhs;

	return WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(pLhs->m_uid)].m_finishOrder < WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(pRhs->m_uid)].m_finishOrder;
}

// lobbymain.cpp:1896
bool ConnectionUserCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sSlotInfo* pLhs = (const sSlotInfo*)lhs;
	const sSlotInfo* pRhs = (const sSlotInfo*)rhs;

	return pLhs->m_connection < pRhs->m_connection;
}

// lobbymain.cpp:1918
CLobbyMain::CLobbyMain(void) {
	WSingleton<CSharedDoc>::Instance()->m_lobbyMessage = "";

	m_loginDlg = NULL;
	m_resultReportDlg = NULL;
	m_approachGiftDlg = NULL;
	m_resultDlg = NULL;
	m_waitDlg = NULL;
	m_createCharEdit = NULL;
	m_createDescEdit = NULL;
	m_createHairEdit = NULL;
	m_createShirtsEdit = NULL;
	m_pointEventDlg = NULL;
	m_worldTourEventDlg = NULL;
	m_findEbortEventDlg = NULL;
	m_extraEventDlg = NULL;
	m_overBarButtonDown = false;
	m_subSelectionPending = false;
	m_roomListDirty = false;
	m_charViewer = NULL;
	m_caddieViewer = NULL;
	m_aztecViewer = NULL;
	m_clubViewer = NULL;
	m_mascotViewer = NULL;
	m_equipItemViewer = NULL;
	m_charSelViewer = NULL;
	m_caddieSelViewer = NULL;
	m_aztecSelViewer = NULL;
	m_clubSelViewer = NULL;
	m_mascotSelViewer = NULL;
	m_roomMasterUid = -1;
	m_waitDlgTime = -10.0f;
	m_storedText[0] = 0;
	m_chatWndOpen = true;
	m_roomSortPending = false;
	m_userSort = USERSORT_DEFAULT;
	m_userSortPending = false;
	m_titleFont = NULL;
	m_userRankTabArea = NULL;
	m_caddieWarningPending = false;
	memset(m_warningCaddies, 0, sizeof(m_warningCaddies));

	ResetControl();

	m_scrollActive = false;

	m_autoStartCountdown = 15.0f;

	m_toppagePointEventBtn = NULL;

	m_loginBitmaps[0] = NULL;
	m_loginBitmaps[1] = NULL;
	m_loginBitmaps[2] = NULL;

	m_newTabBitmap = NULL;

	m_pendingDlg = NULL;
	m_useReportEnabled = false;

	m_toppageWorldTourBtn = NULL;

	m_secondPwdDlg = NULL;

	m_toppageHalloweenBtn = NULL;
	m_halloweenEventDlg = NULL;
	m_halloweenEvent2Dlg = NULL;

	m_toppageChristmasBtn = NULL;
	m_christmasEventDlg = NULL;

	m_toppageChristmasSockBtn = NULL;
	m_christmasSockEventDlg = NULL;

	m_toppageTicketExchangeBtn = NULL;
	m_ticketExchangeDlg = NULL;

	m_toppageMissionBtn = NULL;
	m_missionEventDlg = NULL;

	m_toppageBingoBtn = NULL;
	m_bingoEventDlg = NULL;

	m_toppageEvent1Btn = NULL;

	m_toppageExtraBtn = NULL;

	m_quickResultDlg = NULL;

	m_quickResultDlg2 = NULL;
	m_valEventDlg = NULL;
	m_createNickDlg = NULL;
	m_topPageBtnIndex = 0;
	m_toppageBtnScale = 1.0f;

	WSingleton<CIconManager>::Instance()->ClearInformation();
}

// lobbymain.cpp:2087
CLobbyMain::~CLobbyMain(void) {
	std::map<unsigned long, sRoomSlot>::iterator it;
	for (it = CSharedDoc::Instance()->m_roomSlotMap.begin(); it != CSharedDoc::Instance()->m_roomSlotMap.end(); ++it) {
		if (it->second.m_pPet) { delete it->second.m_pPet; it->second.m_pPet = NULL; }
	}

	this << MsgObject(0, 2, 0, 0, 0, 0, 0);
}

// lobbymain.cpp:2097
void CLobbyMain::OnPreLoadInit(void) {
	CTaskMain::OnPreLoadInit();

	WSingleton<CMouseCursor>::Instance()->SetAutoHide(false);
}

// lobbymain.cpp:2109
void CLobbyMain::OnLoad(void) {
	CTaskMain::OnLoad();
}

// lobbymain.cpp:2118
void CLobbyMain::OnInit(void) {
	g_pFresh->GetManager()->SetExclusiveKey(true);

	CTaskMain::OnInit();

	if (m_approachGiftDlg == NULL)
		m_approachGiftTimer = 1.0f;

	WPolySoup::sCamera* camera = GOLFDOC()->m_pPolySoup->FindCamera("DefaultCamera");
	if (camera != NULL) {
		g_camera = camera->m_matrix;
		g_view->SetFOV(camera->m_fov * 0.017453292f);
	}

	m_bgCameraPaused = false;
	m_bgCameraDirection = 1.0f;
	m_bgCameraMoveTime = g_CurrentTime;

	m_cateNewRoom[0] = 0;
	m_cateNewRoom[1] = 0;
	m_cateNewRoom[2] = 0;

	for (int i = 0; i < 3; i++) {
		m_loginBitmaps[i] = g_pFresh->GetBitmap(MakeStr("login_%02d", i));
	}

	m_newTabBitmap = g_pFresh->GetBitmap("tap_new2");
}

// lobbymain.cpp:2165
void CLobbyMain::OnDestroy(void) {
	CTaskMain::OnDestroy();
}

// lobbymain.cpp:2174
// Pans the lobby's background camera sideways for 80 seconds, waits 2 seconds
// and pans back, easing in and out over 2 seconds (0.3 units per second).
void CLobbyMain::ProcessLobbyBgCamera(float dt) {
	if (CSharedDoc::Instance()->m_3DBackgroundOff)
		return;

	if (m_bgCameraPaused) {
		if (g_CurrentTime - m_bgCameraPauseTime > 2000) {
			m_bgCameraPaused = false;
			m_bgCameraDirection *= -1.0f;
			m_bgCameraMoveTime = g_CurrentTime;
		}
		return;
	}

	unsigned long elapsed = g_CurrentTime - m_bgCameraMoveTime;
	if (elapsed > 80000) {
		m_bgCameraPaused = true;
		m_bgCameraPauseTime = g_CurrentTime;
		return;
	}

	float speed = 1.0f;
	if (elapsed < 2000)
		speed = elapsed * 0.0005f;
	else if (elapsed > 78000)
		speed = 1.0f - (elapsed - 78000) * 0.0005f;

	g_camera.pivot += g_camera.xa * m_bgCameraDirection * dt * 0.3f * speed;
}

// lobbymain.cpp:2215
// Administrator hotkeys: F10 in a game room acts on the selected user,
// Ctrl+Insert in the room list on the selected room.
void CLobbyMain::ProcessAdministrator(float) {
	if (AdminMode()) {
		if (strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM") == 0
			|| strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXT") == 0) {
			if (g_input->GetDown("FUNC_10", true) && m_selectedUserId != 0xFFFFFFFF) {
				WSendPacket packet(GCT_BANISH_ALL);
				packet.Encode4(m_selectedUserId);
				packet.Send(SEND_TO_DEFAULT);
			}
		}

		if (strcmpi(g_pFresh->GetManager()->GetLayoutID(), "ROOMLIST") == 0) {
			if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false))
				&& g_input->GetDown("INSERT", true)) {
				if (m_selectedRoomData) {
					WSendPacket packet(GCT_SPY_ENTER_ROOM);
					packet.Encode2(static_cast<sRoomInfo*>(m_selectedRoomData)->m_roomNumber);
					packet.Send(SEND_TO_DEFAULT);
				}
			}
		}
	}
}

// lobbymain.cpp:2251
// Dialog keys: left/right page the notice dialogs, up/down scroll the room
// list, Page Up/Down walk the chat history (chat input) or the chat targets
// (target edit); the chat input is coloured by the chat target.
void CLobbyMain::ProcessDialogKeyboard(float) {
	if (m_noticeDlg) {
		if (g_input->GetDown("\xC1\xC2", false))   // left
			m_noticeDlg->Prev();
		else if (g_input->GetDown("\xBF\xEC", false))   // right
			m_noticeDlg->Next();
	}

	if (m_pcbangNoticeDlg) {
		if (g_input->GetDown("\xC1\xC2", false))
			m_pcbangNoticeDlg->Prev();
		else if (g_input->GetDown("\xBF\xEC", false))
			m_pcbangNoticeDlg->Next();
	}

	if (m_guildNoticeDlg) {
		if (g_input->GetDown("\xC1\xC2", false))
			m_guildNoticeDlg->Prev();
		else if (g_input->GetDown("\xBF\xEC", false))
			m_guildNoticeDlg->Next();
	}

	if (strcmpi(g_pFresh->GetManager()->GetLayoutID(), "ROOMLIST") == 0 && m_roomListBox && !m_emoticonDlg) {
		if (g_input->GetDown("\xBB\xF3", false))   // up
			m_roomListBox->GetScrollBar()->ScrollUp(1);

		if (g_input->GetDown("\xC7\xCF", false))   // down
			m_roomListBox->GetScrollBar()->ScrollDown(1);
	}

	if (m_chatTargetCombo && m_chatEdit) {
		if (g_input->GetDown("PAGE_UP", true)) {
			if (m_chatEdit->HasKeyFocus() && CSharedDoc::Instance()->m_chatHistory.size() > 0) {
				const char* line = m_chatEdit->GetLine(1, false);
				std::list<std::string>::iterator it = std::find(CSharedDoc::Instance()->m_chatHistory.begin(), CSharedDoc::Instance()->m_chatHistory.end(), line);
				if (it != CSharedDoc::Instance()->m_chatHistory.end()) {
					if (it != CSharedDoc::Instance()->m_chatHistory.begin()) {
						--it;
						CChatMsg::Instance()->SetChatText(it->c_str(), false);
					}
				} else {
					CChatMsg::Instance()->SetChatText(CSharedDoc::Instance()->m_chatHistory.back().c_str(), false);
				}
			} else if (m_chatTargetCombo->HasKeyFocus() && CSharedDoc::Instance()->m_list4CD8.size() > 0) {
				CChatMsg::Instance()->SetChatText(CSharedDoc::Instance()->GetPrevChatTarget(m_chatTargetCombo->GetLine(1, false)), false);
			}
		} else if (g_input->GetDown("PAGE_DOWN", true)) {
			if (m_chatEdit->HasKeyFocus() && CSharedDoc::Instance()->m_chatHistory.size() > 0) {
				const char* line = m_chatEdit->GetLine(1, false);
				std::list<std::string>::iterator it = std::find(CSharedDoc::Instance()->m_chatHistory.begin(), CSharedDoc::Instance()->m_chatHistory.end(), line);
				if (it != CSharedDoc::Instance()->m_chatHistory.end()) {
					++it;
					if (it != CSharedDoc::Instance()->m_chatHistory.end())
						CChatMsg::Instance()->SetChatText(it->c_str(), false);
					else
						CChatMsg::Instance()->SetChatText("", false);
				}
			} else if (m_chatTargetCombo->HasKeyFocus() && CSharedDoc::Instance()->m_list4CD8.size() > 0) {
				CChatMsg::Instance()->SetChatText(CSharedDoc::Instance()->GetNextChatTarget(m_chatTargetCombo->GetLine(1, false)), false);
			}
		}

		if (strlen(m_chatTargetCombo->GetLine(1, false)) == 0
			|| strcmp(m_chatTargetCombo->GetLine(1, false), "\xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4") == 0) {   // "to all"
			m_chatEdit->SetFontColor(0xFFFFFFFF);
		} else if (strlen(m_chatTargetCombo->GetLine(1, false)) == 0
			|| strcmp(m_chatTargetCombo->GetLine(1, false), "\xB1\xE6\xB5\xE5\xBF\xA1\xB0\xD4") == 0) {   // "to the guild"
			m_chatEdit->SetFontColor(0xFF5BFFB0);
		} else {
			m_chatEdit->SetFontColor(0xFFCF67FF);
		}
	}
}

// lobbymain.cpp:2346
// Runs the room's pets and, in the game room, sends the local player's pet
// direction and motion changes to the others.
void CLobbyMain::ProcessLobbyPetSync(float dt) {
	std::map<unsigned long, sRoomSlot>::const_iterator it;
	for (it = CSharedDoc::Instance()->m_roomSlotMap.begin(); it != CSharedDoc::Instance()->m_roomSlotMap.end(); ++it) {
		if (it->second.m_pPet == NULL)
			continue;

		it->second.m_pPet->Process(dt, false);

		if (it->first == MyGuid(false)
			&& strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM") == 0) {
			if (it->second.m_pPet->IsDirectionChanged()) {
				float direction = it->second.m_pPet->GetDirection();

				WSendPacket packet(GCT_SYNC_ACTIVITY);
				packet.Encode1(0);
				packet.EncodeBuffer(&direction, 4);
				packet.Send(SEND_TO_DEFAULT);
			}

			if (it->second.m_pPet->IsNextMotion()) {
				WSendPacket packet(GCT_SYNC_ACTIVITY);
				packet.Encode1(1);
				packet.EncodeStr(std::string(it->second.m_pPet->GetMotionName()));
				packet.Send(SEND_TO_DEFAULT);
			}
		}
	}
}

// lobbymain.cpp:2416
// The lobby's per-frame UI: the login dialog's idle fade, the approach gift
// and result dialogs' timeout, chat hotkeys, the IME language mark, closing
// the category lists and the selection bars, the start tip blink, the
// result screen's clock, the away state, the Event1 button bobbing and the
// treasure hunt's approach gift.
void CLobbyMain::ProcessLobbyUI(float dt) {
	if (strcmpi(g_pFresh->GetManager()->GetLayoutID(), "LOGIN") == 0 && !m_scrollActive) {
		if (m_loginDlg) {
			if ((FrLoginDlg::GetState() == 0 || FrLoginDlg::GetState() == 5)
				&& timeGetTime() - g_mouse->GetLastInputTime() > 60000
				&& timeGetTime() - g_ime->GetLastInputTime() > 60000) {
				m_loginDlg->SetFadeout(true);

				WSingleton<CMouseCursor>::Instance()->SetActive(false);
			}
		} else if ((g_mouse->IsUpdated() || g_ime->IsUpdated()) && !g_bQuit) {
			m_loginDlg = CreateForm<FrLoginDlg>(g_pFresh->GetManager(), this, "login", NULL);
			m_loginDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnLoginDlgResult, 10);
			WSingleton<CMouseCursor>::Instance()->SetActive(true);
		}
	}

	if (m_approachGiftTimer <= 0.0f) {
		m_approachGiftTimer += dt;

		bool expired = false;
		if (m_approachGiftTimer > 0.0f)
			expired = true;

		if (g_input->GetDown("ESCAPE", true) == 1) {
			FrUniteResultDlg* resultDlg = (FrUniteResultDlg*)m_resultReportDlg;
			if (resultDlg == NULL)
				expired = true;
			else if (resultDlg->GetDone() && !resultDlg->CheckChildDlgVisible())
				expired = true;
		}

		if (expired) {
			m_approachGiftTimer = 1.0f;

			if (!m_levelupDlgs.empty()) {
				for (unsigned int i = 0; i < m_levelupDlgs.size(); i++) {
					if (m_levelupDlgs[i]) {
						m_levelupDlgs[i]->Close((eFormRet)0, true);
						m_levelupDlgs[i] = NULL;
					}
				}
				m_levelupDlgs.clear();
			}

			if (m_resultReportDlg)
				m_resultReportDlg->Close(false);

			if (CUserInfo::Instance() && CUserInfo::Instance()->GetDlg())
				CUserInfo::Instance()->Close();

			if (strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXTRES") == 0) {
				if (CSharedDoc::Instance()->m_roomInfo.m_roomMode != 14) {
					OpenLayout("GAMEROOM_EXT");
					HandleMsg(MsgObject(0, 0x0F, 0, 0, 0, 0, 0));
					HandleMsg(MsgObject(0, 0x2F, 0, 0, 0, 0, 0));
				} else {
					SendRoomExitPacket();
				}
			}
		}
	} else if (m_approachGiftDlg) {
		m_approachGiftDlg->Close(true);
		m_approachGiftDlg = NULL;
	}

	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) && g_input->GetDown("EMOTICON", false)) {
		if (!CRankingInfo::Instance()->GetDlg() && !CMessengerInfo::Instance()->IsVisible())
			OnEmoticonBtnUp();
	}

	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) && g_input->GetDown("REPLAY", false))
		OnGameRoom_StartBtnUp();

	if (m_languageArea) {
		if (g_ime->IsAlphaNumericMode() != data_ab2830) {
			data_ab2830 = g_ime->IsAlphaNumericMode();

			if (data_ab2830)
				m_languageArea->SetBgImg("chat_eng");
			else
				m_languageArea->SetBgImg("chat_kor");

			if (m_onelineReqDlg)
				((FrOnelineReqDlg*)m_onelineReqDlg)->SetLanguage(data_ab2830);
		}
	}

	if (strcmpi(g_pFresh->GetManager()->GetLayoutID(), "ROOMLIST") == 0) {
		if (g_input->GetButton(LEFT_BUTTON) == 2)
			m_overBarButtonDown = true;

		if ((m_cateVSList && m_cateVSList->IsVisible())
			|| (m_cateMassList && m_cateMassList->IsVisible())
			|| (m_cateBattleList && m_cateBattleList->IsVisible())) {
			if (m_overBarButtonDown && g_input->GetButton(LEFT_BUTTON) == 1) {
				m_overBarButtonDown = false;
				m_cateVSList->SetVisible(false);
			}
		}
	}

	if (strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM") == 0
		|| strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXT") == 0) {
		if (g_input->GetButton(LEFT_BUTTON) == 2)
			m_overBarButtonDown = true;

		if (m_subSelectionPending && m_overBarButtonDown && g_input->GetButton(LEFT_BUTTON) == 1) {
			m_overBarButtonDown = false;
			m_subSelectionPending = false;
			SendChangedUserInfo();
			InitControlsSelectedUserInfo();
		}
	}

	if (m_tipBlinking) {
		m_tipBlinkTime += dt;
		if (m_tipBlinkTime > 0.2f) {
			m_tipBlinkTime -= 0.2f;
			m_tipBlinkCount++;

			if (m_blinkTipArea)
				m_blinkTipArea->SetVisible(!m_blinkTipArea->IsVisible());

			if (m_tipBlinkCount == 3) {
				m_tipBlinking = false;
				m_tipBlinkCount = 0;
			}
		}
	}

	if (strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXTRES") == 0) {
		unsigned long endTime = GetGameTimeLimit() + CSharedDoc::Instance()->m_gameStartTime;
		if (endTime > g_CurrentTime) {
			unsigned long seconds = (endTime - g_CurrentTime) / 1000;
			HandleMsg(MsgObject(this, 0x10, (intptr_t)MakeStr("%02d:%02d", seconds / 60, seconds % 60), 0, 0, 0, 0));
		} else {
			HandleMsg(MsgObject(this, 0x10, (intptr_t)"00:00", 0, 0, 0, 0));
		}
	}

	if (m_waitDlgTime < 0.0f
		&& (strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM") == 0
			|| strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXT") == 0)) {
		bool awayChanged = false;

		if (m_away) {
			if (g_mouse->IsUpdated() || g_ime->IsUpdated()) {
				awayChanged = true;
				m_away = false;
			}

			// Five minutes away: the room master is told to kick the player.
			if (timeGetTime() - g_mouse->GetLastInputTime() > 300000
				&& timeGetTime() - g_ime->GetLastInputTime() > 300000) {
				std::list<sSlotInfo>::iterator it = std::find(CSharedDoc::Instance()->m_roomPlayerList.begin(), CSharedDoc::Instance()->m_roomPlayerList.end(), MyGuid(false));

				if (it != CSharedDoc::Instance()->m_roomPlayerList.end() && (it->m_attribute & 0x18)) {
					WSendPacket packet(GCT_EXIT_ROOM);
					packet.Encode1(0);
					packet.Encode2(0xFFFF);
					packet.Encode8(0);
					packet.Encode8(0);
					packet.Send(SEND_TO_DEFAULT);

					this << MsgObject(0, 1, 0, 0, 0, 0, 0);
				}
			}
		} else if (timeGetTime() - g_mouse->GetLastInputTime() > 60000
			&& timeGetTime() - g_ime->GetLastInputTime() > 60000) {
			m_away = true;
			awayChanged = true;
		}

		if (awayChanged) {
			std::list<sSlotInfo>::iterator it = std::find(CSharedDoc::Instance()->m_roomPlayerList.begin(), CSharedDoc::Instance()->m_roomPlayerList.end(), MyGuid(false));

			if (it == CSharedDoc::Instance()->m_roomPlayerList.end())
				return;

			if (it->m_attribute & 0x18) {
				WSendPacket packet(GCT_REQUEST_ROOMINFO_CHANGED);
				packet.Encode2(0xFFFF);
				packet.Encode1(1);
				packet.Encode1(9);
				packet.Encode1(m_away != false);
				packet.Send(SEND_TO_DEFAULT);
			}

			WSendPacket packet(GCT_SLEEP);
			packet.Encode1(m_away != false);
			packet.Send(SEND_TO_DEFAULT);
		}
	}

	if (IsLocalContent((localContentType_t)0x18) && !IsLocalContent((localContentType_t)0x48)) {
		if (m_toppageEvent1Btn && m_toppageEvent1Btn->IsVisible()) {
			WRect rect;
			rect = m_toppageEvent1Btn->GetRect();
			float top = rect.y + dt * m_toppageBtnScale * 8.0f;

			if ((top > m_event1IconRect.y + 15.0f && m_toppageBtnScale > 0.0f)
				|| (top < m_event1IconRect.y - 15.0f && m_toppageBtnScale < 0.0f))
				m_toppageBtnScale *= -1.0f;

			WPoint pt;
			pt.x = rect.x;
			pt.y = top;
			m_toppageEvent1Btn->MoveWindow(pt);
		}
	}

	if (CSharedDoc::Instance()->GetAppGiftDownLoadState() == 2 && CSharedDoc::Instance()->m_roomType == 10 && m_approachGiftDlg == NULL) {
		unsigned int giftCount = CTHunter::Instance()->GetGiftSize();
		if (giftCount <= 8)
			m_approachGiftTimer = -9.0f;
		else if (giftCount <= 16)
			m_approachGiftTimer = -12.0f;
		else
			m_approachGiftTimer = -20.0f;

		FrTreasureGift* dlg = CreateForm<FrTreasureGift>(g_pFresh->GetManager(), this, "trgift", NULL);
		m_approachGiftDlg = dlg;
		dlg->SetApproachMode();
		m_approachGiftDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnApproachGiftDlgResult, 0x0C);
		m_approachGiftDlg->SetTimeLimit(12.0f);
		m_approachGiftDlg->EnableDrag(false);
		m_approachGiftDlg->SetFixed(true);
		CSharedDoc::Instance()->m_value5D08 = 0;
	}
}

// lobbymain.cpp:2817
// The game room's auto start: once enough players are in a non-guild room
// that allows it, counts down m_autoStartCountdown, announcing the last five
// seconds, and starts the game for the room master at zero.
void CLobbyMain::ProcessAutoStart(float dt) {
	sRoomInfo& room = CSharedDoc::Instance()->m_roomInfo;

	if (m_autoStartCountdown > 0.0f
		&& strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXT") == 0
		&& (m_roomOptionD || m_autoStartArmed)) {
		int needed = room.m_userLimit * 3 / 4;
		int minPlayers = (CSharedDoc::Instance()->m_curServer.flags & 2) ? 5 : 10;
		if (minPlayers >= needed)
			needed = minPlayers;

		if (room.m_userCount >= needed && CSharedDoc::Instance()->m_roomInfo.m_gameType != 6 && room.m_option[1] == 0) {
			float left = m_autoStartCountdown - dt;
			m_autoStartCountdown = left;

			if (left < 6.0f && left + dt > 6.0f) {
				this << MsgObject(0, 3, (intptr_t)"(\xBE\xCB\xB8\xB2)\xC2\xFC\xBF\xA9\xC0\xCE\xBF\xF8\xC0\xCC \xC0\xFC\xBF\xF8 \xC1\xD8\xBA\xF1\xBF\xCF\xB7\xE1\xB0\xA1 \xB5\xC7\xBE\xFA\xC0\xB8\xB9\xC7\xB7\xCE 5\xC3\xCA \xC8\xC4\xBF\xA1 \xB0\xD4\xC0\xD3\xC0\xBB \xC0\xDA\xB5\xBF \xBD\xC3\xC0\xDB\xC7\xD5\xB4\xCF\xB4\xD9.", 0xFFFF7878, 0, 0, 0);
			} else if (left < 5.0f && left + dt > 5.0f) {
				this << MsgObject(0, 3, (intptr_t)"(\xBE\xCB\xB8\xB2)\xB0\xD4\xC0\xD3\xBD\xC3\xC0\xDB 5\xC3\xCA\xC0\xFC\xC0\xD4\xB4\xCF\xB4\xD9.", 0xFFFF7878, 0, 0, 0);
			} else if (left < 4.0f && left + dt > 4.0f) {
				this << MsgObject(0, 3, (intptr_t)"(\xBE\xCB\xB8\xB2)\xB0\xD4\xC0\xD3\xBD\xC3\xC0\xDB 4\xC3\xCA\xC0\xFC\xC0\xD4\xB4\xCF\xB4\xD9.", 0xFFFF7878, 0, 0, 0);
			} else if (left < 3.0f && left + dt > 3.0f) {
				this << MsgObject(0, 3, (intptr_t)"(\xBE\xCB\xB8\xB2)\xB0\xD4\xC0\xD3\xBD\xC3\xC0\xDB 3\xC3\xCA\xC0\xFC\xC0\xD4\xB4\xCF\xB4\xD9.", 0xFFFF7878, 0, 0, 0);
			} else if (left < 2.0f && left + dt > 2.0f) {
				this << MsgObject(0, 3, (intptr_t)"(\xBE\xCB\xB8\xB2)\xB0\xD4\xC0\xD3\xBD\xC3\xC0\xDB 2\xC3\xCA\xC0\xFC\xC0\xD4\xB4\xCF\xB4\xD9.", 0xFFFF7878, 0, 0, 0);
			} else if (left < 1.0f && left + dt > 1.0f) {
				m_autoStartArmed = false;
				this << MsgObject(0, 3, (intptr_t)"(\xBE\xCB\xB8\xB2)\xB0\xD4\xC0\xD3\xBD\xC3\xC0\xDB 1\xC3\xCA\xC0\xFC\xC0\xD4\xB4\xCF\xB4\xD9.", 0xFFFF7878, 0, 0, 0);
			} else if (left < 0.0f && m_roomMasterUid == CSharedDoc::Instance()->GetPangyaUserInfo().guid
				&& CSharedDoc::Instance()->m_roomInfo.m_gameType != 6) {
				OnGameRoom_StartBtnUp();
			}

			return;
		}
	}

	if (CSharedDoc::Instance()->m_roomInfo.m_gameType == 6)
		m_autoStartCountdown = 6.0f;
	else
		m_autoStartCountdown = 15.0f;
}

// lobbymain.cpp:2856
void CLobbyMain::OnProcess(float dt) {
	CTaskMain::OnProcess(dt);

	ProcessLobbyBgCamera(dt);
	ProcessLobbyPetSync(dt);

	if (IsLocalContent((localContentType_t)0x0A))
		ProcessAdministrator(dt);

	ProcessDialogKeyboard(dt);
	ProcessLobbyUI(dt);
	ProcessAutoStart(dt);
	ResetControlsRoomListSort(dt);

	if (IsLocalContent((localContentType_t)0x4C) && g_pFresh->IsCurrentLayout("TOPPAGE"))
		AnimateWebEventButton(dt);

	if (IsLocalContent((localContentType_t)0x57))
		RefreshAllUccClothes();
}

// lobbymain.cpp:2889
// Cycles the Christmas event button through its "xmas_main_i<n>" frames
// (1, 2, 3, 2), one every 0.3 seconds.
void CLobbyMain::AnimateWebEventButton(float dt) {
	static int s_frame;
	static int s_frameIndex;
	static float s_frameTime;

	s_frameTime += dt;
	if (s_frameTime > 0.3f) {
		s_frameTime = 0.0f;
		int frames[4] = { 1, 2, 3, 2 };

		if (++s_frameIndex > 3)
			s_frameIndex = 0;
		s_frame = frames[s_frameIndex];

		char number[4] = { 0, };
		std::string image("xmas_main_i");
		image += itoa(s_frame, number, 10);

		if (m_toppageChristmasBtn) {
			m_toppageChristmasBtn->SetButtonImg(image.c_str(), FrButton::NORMAL);
			m_toppageChristmasBtn->SetButtonImg(image.c_str(), FrButton::OVER);
		}
	}
}

// lobbymain.cpp:2917
void CLobbyMain::OnDisplay(void) {
}

// Defined in shareddoc.inl:213 (SharedDoc unit); not declared by SharedDoc.h yet.
const char* GetGameTypeName(unsigned char gameType);

// lobbymain.cpp:2921
void CLobbyMain::HandleMsg(const MsgObject& msg) {
	switch (msg.m_msg) {
	case 0x5B: {
		// lobbymain.cpp:2945, a room slot's pet direction (param2 0) or
		// motion (param2 1)
		std::map<unsigned long, sRoomSlot>::iterator it = CSharedDoc::Instance()->m_roomSlotMap.find((unsigned long)msg.m_param1);
		if (it == CSharedDoc::Instance()->m_roomSlotMap.end())
			break;

		if (CSharedDoc::Instance()->m_roomSlotMap[(unsigned long)msg.m_param1].m_pPet == NULL)
			break;

		switch (msg.m_param2) {
		case 0:
			// lobbymain.cpp:2954
			CSharedDoc::Instance()->m_roomSlotMap[(unsigned long)msg.m_param1].m_pPet->SetDirection(*(float*)msg.m_param3);
			break;
		case 1:
			// lobbymain.cpp:2958
			CSharedDoc::Instance()->m_roomSlotMap[(unsigned long)msg.m_param1].m_pPet->SetMotion((const char*)msg.m_param3, false, 0.0f);
			break;
		}
		break;
	}

	case 0x59: {
		// lobbymain.cpp:2968, delete the room slot pets, clear the slots and
		// re-enable the pet areas
		std::map<unsigned long, sRoomSlot>::iterator it;
		for (it = CSharedDoc::Instance()->m_roomSlotMap.begin(); it != CSharedDoc::Instance()->m_roomSlotMap.end(); ++it) {
			if (it->second.m_pPet) { delete it->second.m_pPet; it->second.m_pPet = NULL; }
		}
		CSharedDoc::Instance()->m_roomSlotMap.clear();

		for (int i = 0; i < 4; i++) {
			if (m_petArea[i])
				m_petArea[i]->SetMouseEvent(false);
		}
		break;
	}

	case 0x5A: {
		// lobbymain.cpp:2981, a player's room slot (pet area, character, angel wings)
		sSlotInfo* player = (sSlotInfo*)msg.m_param1;
		sCharacterInfo* characterInfo = (sCharacterInfo*)msg.m_param2;

		sRoomSlot slot;
		slot.m_pPetArea = m_petArea[player->m_connection - 1];
		slot.m_pPet = NULL;
		slot.m_characterInfo = *characterInfo;

		slot.m_angelFlags.flag0 = player->m_flags57.flag0;
		slot.m_angelFlags.flag1 = player->m_flags57.flag1;

		CSharedDoc::Instance()->m_roomSlotMap[player->m_uid] = slot;
		ShowPet(player->m_uid);
		break;
	}

	case 0x4F: {
		// lobbymain.cpp:2999, ask to remove the selected friend
		sFriend* pFriend = BuddyMGR->GetBuddy(m_removeFriendUid);
		if (pFriend) {
			FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", NULL);
			form->SetMessage(MakeStr("%s\xB4\xD4\xC0\xBB \xC4\xA3\xB1\xB8 \xB8\xAE\xBD\xBA\xC6\xAE\xBF\xA1\xBC\xAD \xBB\xE8\xC1\xA6\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", pFriend->nick), false);
			form->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnRemoveFriendDlgResult, 0);
		}
		break;
	}

	case 0x44:
		// lobbymain.cpp:3017
		OpenEventPrizeForm((unsigned char)msg.m_param1);
		break;

	case 0: {
		// lobbymain.cpp:3021, open a layout
		if (g_pFresh) {
			const char* name = (const char*)msg.m_param1;

			if (!g_pFresh->IsCurrentLayout(name)) {
				CloseAllDialogs();
				ResetControl();

				if (stricmp(name, "TOPPAGE") == 0) {
					CIconManager::Instance()->ClearInformation();
					AfxGetTask()->SetWhisper(false);
				} else {
					AfxGetTask()->SetWhisper(true);
				}

				g_pFresh->OpenLayout(name, this, false);
				WSingleton<CMouseCursor>::Instance()->Toggle(true);
				CheckTikiReport(name);
			}
		}
		break;
	}

	case 0x03:
		// lobbymain.cpp:3088, a chat line: kept for the chat view outside
		// the game room layouts
		if (strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM") == 0
			|| strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXT") == 0
			|| strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXTRES") == 0) {
			sChatLine line;
			line.m_text = (const char*)msg.m_param1;
			line.m_color = msg.m_param2;
			CSharedDoc::Instance()->m_chatLineList.push_back(line);
		}
		if (m_chatView)
			m_chatView->AddLine((const char*)msg.m_param1, msg.m_param2, false);
		break;

	case 0x60:
		// lobbymain.cpp:3104, a chat line's facial expression on the
		// speaker's pet in the game room
		if (strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM") == 0) {
			std::string name = (const char*)msg.m_param1;
			std::string text = (const char*)msg.m_param2;

			int face = CSharedDoc::Instance()->m_chatManager.GetFaceIndex(text.c_str());
			if (face != 5) {
				if (m_roomUserListBox) {
					FrListBox::ITEM_LIST::iterator it;
					for (it = m_roomUserListBox->m_itemList.begin(); it != m_roomUserListBox->m_itemList.end(); ++it) {
						sSlotInfo* player = (sSlotInfo*)(*it)->pData;

						if (player) {
							if (strcmp(player->m_nickname, name.c_str()) == 0) {
								if (CSharedDoc::Instance()->m_roomSlotMap[player->m_uid].m_pPet)
									CSharedDoc::Instance()->m_roomSlotMap[player->m_uid].m_pPet->SetChatFacial(face, player->m_characterTypeId);
								break;
							}
						}
					}
				}
			}
		}
		break;

	case 0x0A: {
		// lobbymain.cpp:3138, add a lobby user (hidden administrators
		// only show to themselves or to administrators)
		sBriefUserInfo* info = (sBriefUserInfo*)msg.m_param1;
		CSharedDoc* doc = CSharedDoc::Instance();
		if (doc->m_userInfo.guid == info->uid && doc->m_userInfo.identityBits.administrator) {
			if (info->stayInLobby)
				break;
		}
		if (doc->m_userInfo.guid != info->uid && !doc->m_userInfo.IsIdentity(4)
			&& (info->identity & 0x14)) {
			if (!info->invisible)
				break;
		}
		if (m_userListBox) {
			m_userListBox->AddItem(info);
			RoomList_SortUserList(m_userSort);
		}
		if (m_userListDlg) {
			m_userListDlg->AddUser((void*)msg.m_param1);
			m_userListDlg->SortUserList();
		}
		break;
	}

	case 0x0C: {
		// lobbymain.cpp:3174, remove a lobby user
		sBriefUserInfo* info = (sBriefUserInfo*)msg.m_param1;
		if (info->uid == CSharedDoc::Instance()->m_userInfo.guid && CSharedDoc::Instance()->m_userInfo.identityBits.administrator) {
			if (info->stayInLobby)
				break;
		}
		if (m_userListBox)
			m_userListBox->DelItem(info, msg.m_param2);
		if (m_userListDlg)
			m_userListDlg->DelUser((void*)msg.m_param1, msg.m_param2);
		if (info->uid == m_selectedUserId) {
			m_selectedUserId = -1;
			m_removeFriendUid = -1;
			if (m_userActionBtn)
				m_userActionBtn->Enable(false);
		}
		break;
	}

	case 0x0B:
		// lobbymain.cpp:3204, clear the lobby user list
		if (m_userListBox)
			m_userListBox->ClearItem();
		m_selectedUserId = -1;
		m_removeFriendUid = -1;
		break;

	case 0x04: {
		// lobbymain.cpp:3213, add a room to the room list (filtered by the
		// current game type, else by the current category)
		if (m_roomListBox) {
			sRoomInfo* room = (sRoomInfo*)msg.m_param1;
			if (data_aa9eac.m_gameType != -1) {
				if (room->m_gameType == data_aa9eac.m_gameType)
					m_roomListBox->AddItem(room);
			} else if (GetCateByGameType(room->m_gameType) == data_aa9eac.m_roomCate || data_aa9eac.m_roomCate == ROOMCATE_ALL) {
				m_roomListBox->AddItem(room);
			}
		}
		break;
	}

	case 0x05: {
		// lobbymain.cpp:3228, remove a room from the room list, keeping
		// at least 6 lines
		if (m_roomListBox) {
			sRoomInfo* room = (sRoomInfo*)msg.m_param1;
			if (data_aa9eac.m_gameType != -1) {
				if (room->m_gameType == data_aa9eac.m_gameType) {
					m_roomListBox->DelItem(room, msg.m_param2);
					for (int i = m_roomListBox->GetCurrentItemSize(true); i < 6; i++)
						m_roomListBox->AddItem(NULL);
				}
			} else if (GetCateByGameType(room->m_gameType) == data_aa9eac.m_roomCate || data_aa9eac.m_roomCate == ROOMCATE_ALL) {
				m_roomListBox->DelItem(room, msg.m_param2);
				for (int i = m_roomListBox->GetCurrentItemSize(true); i < 6; i++)
					m_roomListBox->AddItem(NULL);
			}
		}
		break;
	}

	case 0x06:
		// lobbymain.cpp:3251, clear the room list
		if (m_roomListBox)
			m_roomListBox->ClearItem();
		break;

	case 0x0D: {
		// lobbymain.cpp:3257, pad the room list to 6 lines (or drop empty
		// lines past 6), then sort it
		if (m_roomListBox) {
			int size = m_roomListBox->GetCurrentItemSize(true);
			if (size <= 6) {
				for (; size < 6; size++)
					m_roomListBox->AddItem(NULL);
			} else {
				int extra = size - 6;
				FrListBox::ITEM_LIST::iterator it = m_roomListBox->m_itemList.begin();
				while (it != m_roomListBox->m_itemList.end() && extra > 0) {
					FrListBox::ITEM_LIST::iterator cur = it++;
					if ((*cur)->pData == NULL) {
						m_roomListBox->_DelItem(cur);
						extra--;
					}
				}
			}
			RoomList_SortRoomList(m_roomSort, 0);
		}
		break;
	}

	case 0x07: {
		// lobbymain.cpp:3293, a player entered the game room
		if (CSharedDoc::Instance()->m_roomInfo.m_gameType == 5 || CSharedDoc::Instance()->m_roomInfo.m_gameType == 6) {
			if (CSharedDoc::Instance()->m_gameOver) {
				sSlotInfo* slot = (sSlotInfo*)msg.m_param1;
				if ((slot->m_attribute & 3) == 0)
					m_slotList1.push_back(slot);
				else
					m_slotList2.push_back(slot);
				RemakeTeam(false);
			} else {
				RemakeTeam(true);
			}
		} else if (m_roomUserListBox) {
			m_roomUserListBox->AddItem((void*)msg.m_param1);
			m_roomUserListBox->SortItem(ConnectionUserCompare);
		}

		if (CSharedDoc::Instance()->m_roomInfo.m_gameType == 4 || CSharedDoc::Instance()->m_roomInfo.m_gameType == 10) {
			// lobbymain.cpp:3322, drop one empty line past 24
			if (m_roomUserListBox && m_roomUserListBox->GetCurrentItemSize(true) > 24) {
				FrListBox::ITEM_LIST::iterator it;
				for (it = m_roomUserListBox->m_itemList.begin(); it != m_roomUserListBox->m_itemList.end(); ++it) {
					if ((*it)->pData == NULL) {
						m_roomUserListBox->_DelItem(it);
						break;
					}
				}
				m_roomUserListBox->SortItem(ConnectionUserCompare);
			}
			break;
		}
		if (IsMassGame(CSharedDoc::Instance()->m_roomInfo.m_gameType))
			break;

		// lobbymain.cpp:3341, the player's slot (pet area, character)
		sSlotInfo* player = (sSlotInfo*)msg.m_param1;
		sRoomSlot slot;
		slot.m_pPetArea = m_petArea[player->m_connection - 1];
		slot.m_pPet = NULL;
		slot.m_characterInfo = *(sCharacterInfo*)msg.m_param2;
		slot.m_angelFlags.flag0 = player->m_flags57.flag0;
		slot.m_angelFlags.flag1 = player->m_flags57.flag1;
		CSharedDoc::Instance()->m_roomSlotMap[player->m_uid] = slot;
		if (IsLocalContent((localContentType_t)0x54)) {
			if (!(player->m_banishFlags & 1))
				ShowPet(player->m_uid);
		} else {
			ShowPet(player->m_uid);
		}

		// lobbymain.cpp:3365, drop one empty line past 4
		if (m_roomUserListBox && m_roomUserListBox->GetCurrentItemSize(true) > 4) {
			FrListBox::ITEM_LIST::iterator it;
			for (it = m_roomUserListBox->m_itemList.begin(); it != m_roomUserListBox->m_itemList.end(); ++it) {
				if ((*it)->pData == NULL) {
					m_roomUserListBox->_DelItem(it);
					break;
				}
			}
			m_roomUserListBox->SortItem(ConnectionUserCompare);
		}
		break;
	}

	case 0x08: {
		// lobbymain.cpp:3396, a player left the game room
		unsigned long uid = msg.m_param2;
		if (CSharedDoc::Instance()->m_roomInfo.m_gameType == 5 || CSharedDoc::Instance()->m_roomInfo.m_gameType == 6) {
			if (CSharedDoc::Instance()->m_gameOver) {
				std::list<sSlotInfo*>::iterator it;
				if (msg.m_param3 == 0) {
					for (it = m_slotList1.begin(); it != m_slotList1.end(); ++it) {
						if ((*it)->m_uid == uid) {
							m_slotList1.erase(it);
							break;
						}
					}
				} else {
					for (it = m_slotList2.begin(); it != m_slotList2.end(); ++it) {
						if ((*it)->m_uid == uid) {
							m_slotList2.erase(it);
							break;
						}
					}
				}
				RemakeTeam(false);
			} else {
				RemakeTeam(true);
			}
		} else if (m_roomUserListBox && msg.m_param1) {
			m_roomUserListBox->DelItem((void*)msg.m_param1);
		}

		if (CSharedDoc::Instance()->m_roomInfo.m_gameType == 4 || CSharedDoc::Instance()->m_roomInfo.m_gameType == 10) {
			// lobbymain.cpp:3440, keep 24 lines
			if (m_roomUserListBox && m_roomUserListBox->GetCurrentItemSize(true) < 24)
				m_roomUserListBox->AddItem(NULL);
		} else if (!IsMassGame(CSharedDoc::Instance()->m_roomInfo.m_gameType)) {
			HidePet(uid);
			std::map<unsigned long, sRoomSlot>::iterator it = CSharedDoc::Instance()->m_roomSlotMap.find(uid);
			if (it != CSharedDoc::Instance()->m_roomSlotMap.end()) {
				if (it->second.m_pPetArea)
					it->second.m_pPetArea->SetMouseEvent(false);
				CSharedDoc::Instance()->m_roomSlotMap.erase(it);
			}
			// lobbymain.cpp:3460, the players after the one who left moved up
			// one place: each pet goes to its new place's area
			for (it = CSharedDoc::Instance()->m_roomSlotMap.begin(); it != CSharedDoc::Instance()->m_roomSlotMap.end(); ++it) {
				std::list<sSlotInfo>& players = CSharedDoc::Instance()->m_roomPlayerList;
				std::list<sSlotInfo>::iterator player = std::find(players.begin(), players.end(), it->first);
				if (player != players.end()) {
					FrArea* area = m_petArea[player->m_connection - 1];
					it->second.m_pPetArea = area;
					if (it->second.m_pPet) {
						it->second.m_pPet->SetCenter(area->GetRect().x + area->GetRect().w * 0.5f,
							area->GetRect().y + area->GetRect().h * 0.5f);
						if (it->second.m_angelFlags.flag1)
							it->second.m_pPet->AttachAngelWingFx();
					}
				}
			}
			if (m_roomUserListBox && m_roomUserListBox->GetCurrentItemSize(true) < 4)
				m_roomUserListBox->AddItem(NULL);
		}

		if (uid == m_selectedUserId) {
			m_selectedUserId = -1;
			m_removeFriendUid = -1;
		}
		break;
	}

	case 0x09: {
		// lobbymain.cpp:3495, a room player's angel wing bits changed
		sSlotInfo* player = (sSlotInfo*)msg.m_param1;
		CSharedDoc* pDoc = CSharedDoc::Instance();
		std::map<unsigned long, sRoomSlot>::iterator slot = pDoc->m_roomSlotMap.find(player->m_uid);
		if (slot != pDoc->m_roomSlotMap.end()) {
			slot->second.m_angelFlags.flag1 = player->m_flags57.flag1;
			slot->second.m_angelFlags.flag0 = player->m_flags57.flag0;
		}
		if (m_roomUserListBox) {
			FrListBox::ITEM_LIST::iterator it;
			for (it = m_roomUserListBox->m_itemList.begin(); it != m_roomUserListBox->m_itemList.end(); ++it) {
				sSlotInfo* item = (sSlotInfo*)(*it)->pData;
				if (item && player->m_uid == item->m_uid) {
					item->m_flags57.flag0 = player->m_flags57.flag0;
					item->m_flags57.flag1 = player->m_flags57.flag1;
					break;
				}
			}
		}
		break;
	}

	case 0x0E:
		// lobbymain.cpp:3532, rebuild the game room's user list
		if (CSharedDoc::Instance()->m_roomInfo.m_userCount > 1) {
			m_tipBlinking = true;
			m_tipBlinkTime = -10.0f;
			m_blinkTipArea = m_readyTipArea;
		} else {
			m_blinkTipArea = m_startTipArea;
		}
		MakeRoomUserList();
		this << MsgObject(0, 0x0F, 0, 0, 0, 0, 0);
		break;

	case 0x0F: {
		// lobbymain.cpp:3554, find the room master
		CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
		std::list<sSlotInfo>& players = pDoc->m_roomPlayerList;
		bool isMaster = false;
		if (AdminMode()) {
			m_roomMasterUid = pDoc->m_userInfo.guid;
			isMaster = true;
		} else {
			std::list<sSlotInfo>::iterator it = pDoc->m_roomPlayerList2.begin();
			if (it != pDoc->m_roomPlayerList2.end()) {
				sSlotInfo player;
				do {
					player = *it;
					if (player.m_attribute & 0x18)
						break;
					++it;
				} while (it != pDoc->m_roomPlayerList2.end());

				m_roomMasterUid = player.m_uid;
				if (m_roomMasterUid == WSingleton<CSharedDoc>::Instance()->m_userInfo.guid)
					isMaster = true;
			}

			for (it = players.begin(); it != players.end(); ++it) {
				if (it->m_attribute & 0x18) {
					m_roomMasterUid = it->m_uid;
					if (it->m_uid == WSingleton<CSharedDoc>::Instance()->m_userInfo.guid)
						isMaster = true;
					break;
				}
			}
		}

		if (isMaster) {
			m_roomOptionA = false;
			m_roomOptionB = false;
			m_blinkTipArea = m_startTipArea;
		}

		if (m_mapBtn) {
			if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomMode == 0x0E)
				m_mapBtn->Enable(false);
			else
				m_mapBtn->Enable(isMaster);
		}

		SetStartBtn(isMaster);
		SetRoomControls(isMaster);

		if (m_roomControlWnd)
			m_roomControlWnd->Enable(isMaster && WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType != 6);
		break;
	}


	case 0x71:
		// lobbymain.cpp:3628
		RemakeTeam(true);
		break;

	case 0x10: {
		// lobbymain.cpp:3635, the room title line: param1 the time text, or 0
		// to build it from the room's time limits
		if (m_roomTitleEdit == NULL)
			break;

		char timeText[64];
		if (msg.m_param1) {
			strcpy(timeText, (const char*)msg.m_param1);
		} else if (IsMassGame(CSharedDoc::Instance()->m_roomInfo.m_gameType)) {
			// lobbymain.cpp:3647
			if (CSharedDoc::Instance()->m_roomInfo.m_gameType == 10)
				sprintf(timeText, "%d\xC3\xCA", CSharedDoc::Instance()->m_roomInfo.m_value51 / 1000); // "%d s"

			if (CSharedDoc::Instance()->m_roomInfo.m_gameType == 6)
				sprintf(timeText, "");
			else
				sprintf(timeText, "%d\xBA\xD0", CSharedDoc::Instance()->m_roomInfo.m_value51 / 60000); // "%d min"
		} else {
			// lobbymain.cpp:3662
			unsigned long timeLimit = CSharedDoc::Instance()->m_roomInfo.m_value4D;
			if (timeLimit == 0)
				sprintf(timeText, "\xBD\xC3\xB0\xA3\xC1\xA6\xC7\xD1\xBE\xF8\xC0\xBD"); // "no time limit"
			else
				sprintf(timeText, "%d\xC3\xCA", timeLimit / 1000); // "%d s"
		}

		// lobbymain.cpp:3671, the mode name
		const char* modeName;
		if (!IsMassGame(CSharedDoc::Instance()->m_roomInfo.m_gameType) || CSharedDoc::Instance()->m_roomInfo.m_gameType == 10) {
			modeName = GetGameTypeName(CSharedDoc::Instance()->m_roomInfo.m_gameType);
		} else {
			IFF_STRUCT::sMatch* pMatch = ItemManager()->FindMatch(CSharedDoc::Instance()->m_roomInfo.m_trophy);
			if (pMatch != NULL) {
				if (CSharedDoc::Instance()->m_roomInfo.m_roomMode != 14)
					modeName = pMatch->m_name;
				else
					modeName = "\xBC\xC5\xC7\xC3\xC4\xDA\xBD\xBA"; // "Shuffle course"
			} else {
				modeName = GetGameTypeName(CSharedDoc::Instance()->m_roomInfo.m_gameType);
			}
		}

		// lobbymain.cpp:3704, "[%d]   %s %s    %s %s  %2d holes  (%d/%d)  %s":
		// "[private]" and "team" markers in CP949
		m_roomTitleEdit->SetLine(1, MakeStr("[%d]   %s %s    %s %s  %2d\xC8\xA6  (%d/%d)  %s",
			CSharedDoc::Instance()->m_roomInfo.m_roomNumber,
			CSharedDoc::Instance()->m_roomInfo.m_isPublic != 0 ? "" : "[\xBA\xF1]",
			CSharedDoc::Instance()->m_roomInfo.m_title,
			modeName,
			CSharedDoc::Instance()->m_roomInfo.m_gameType == 5 ? "\xC6\xC0\xC0\xFC" : "",
			CSharedDoc::Instance()->m_roomInfo.m_holeCount,
			CSharedDoc::Instance()->m_roomInfo.m_userCount,
			CSharedDoc::Instance()->m_roomInfo.m_userLimit,
			timeText), 0, false, 0);
		break;
	}

	case 0x11: {
		// lobbymain.cpp:3711, the room's course changed
		unsigned char course = (unsigned char)msg.m_param1;

		IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(0x28000000 | (course >= 0x7F ? 0x7F : course));
		if (pCourse) {
			if (m_mapBtn)
				m_mapBtn->SetButtonImg(MakeStr("big_%s", pCourse->m_icon), FrButton::NORMAL);

			if (m_backGroundViewer) {
				// lobbymain.cpp:3722
				m_backGroundViewer->Open(MakeStr("background%s.tga", strstr(pCourse->m_icon, "_")));
			}

			if (m_mapBgViewer)
				m_mapBgViewer->Open("map_background.tga");
		}

		// lobbymain.cpp:3732
		if (m_mapGauge) {
			m_mapGauge->ResetGauge();
			if (course < 0x7F) {
				unsigned long gauge = CTHunter::Instance()->GetTHunterGauge((eMapType)GetCurRoomMapType());
				m_mapGauge->SetPos(gauge < 701 ? 0 : gauge - 700);
			} else {
				m_mapGauge->SetPos(0);
			}
		}

		// lobbymain.cpp:3747
		std::list<sSlotInfo>::iterator it = std::find(CSharedDoc::Instance()->m_roomPlayerList.begin(), CSharedDoc::Instance()->m_roomPlayerList.end(), MyGuid(false));
		if (it != CSharedDoc::Instance()->m_roomPlayerList.end())
			SetRoomControls((it->m_attribute & 0x18) || AdminMode());
		break;
	}

	case 0x12: {
		// lobbymain.cpp:3756, reload the hole selection
		ReLoadHoleItem();
		if (m_holeCombo) {
			int index = msg.m_param1;
			if (CSharedDoc::Instance()->m_roomInfo.m_holeCount == 18) {
				if (index > 0)
					index = 1;
			}
			m_holeCombo->SelectItem(index);
		}
		break;
	}

	case 0x16:
		// lobbymain.cpp:3771
		m_storedFlag = msg.m_param1 != 0;
		break;

	case 0x17:
		// lobbymain.cpp:3775
		*(int*)msg.m_param1 = m_storedFlag;
		break;

	case 0x18: {
		// lobbymain.cpp:3780, start the game
		std::list<sSlotInfo>::iterator it;
		for (it = CSharedDoc::Instance()->m_roomPlayerList.begin(); it != CSharedDoc::Instance()->m_roomPlayerList.end(); ++it)
			it->m_attribute &= ~4;
		CSharedDoc::Instance()->m_gameOver = false;
		WSingleton<CMouseCursor>::Instance()->m_mode = CMouseCursor::SCREEN_GAME;
		WSingleton<CMouseCursor>::Instance()->SetActive(false);
		CTaskManager::Instance()->ChangeTask("CGolfTask", "CGolfDoc", false);
		break;
	}

	case 0x1E:
		// lobbymain.cpp:3792, login result state (CLobbyTask)
		if (m_loginDlg) {
			FrLoginDlg::SetState((eLoginState)msg.m_param1);
			break;
		}
		if (msg.m_param1 == 0xD)
			CTaskManager::Instance()->PostMsg(0, "Lobby", 0, (intptr_t)"CREATE", 0, 0, 0);
		break;

	case 0x1F:
		// lobbymain.cpp:3806, game server login state
		if (m_serverDlg)
			m_serverDlg->SetState((gsLoginState_t)msg.m_param1);
		break;

	case 0x42: {
		// lobbymain.cpp:3815, the skinned theme changed
		_ISkinnedTheme* theme = S5::THEME::GetCurrentSkinnedTheme();
		g_pFresh->GetDesktop()->SetWallPaper(theme->GetRoomListBackGroundImgName(), true);

		if (m_makeRoomBtn) {
			THEME_BUTTONS buttons;
			theme->GetMakeRoomButtonImgName(buttons);
			m_makeRoomBtn->SetButtonImg(buttons.szPush, FrButton::PRESSED);
			m_makeRoomBtn->SetButtonImg(buttons.szNormal, FrButton::NORMAL);
			m_makeRoomBtn->SetButtonImg(buttons.szOver, FrButton::OVER);
		}

		if (m_guildInfoBtn) {
			THEME_BUTTONS buttons;
			theme->GetQuickStartButtonImgName(buttons);
			m_guildInfoBtn->SetButtonImg(buttons.szPush, FrButton::PRESSED);
			m_guildInfoBtn->SetButtonImg(buttons.szNormal, FrButton::NORMAL);
			m_guildInfoBtn->SetButtonImg(buttons.szOver, FrButton::OVER);
		}

		// lobbymain.cpp:3843
		if (m_barFrame[0])
			S5::THEME::ChangeOverbarTheme(S5::THEME::GetCurrentSkinnedTheme(), m_barFrame[0]);
		break;
	}

	case 0x30:
		// lobbymain.cpp:3850, a friend's invitation to a room
		if (IsLocalContent((localContentType_t)0x54)) {
			if (m_matchingDlg == NULL && CSharedDoc::Instance()->m_mySlot == 0xFFFF && COption::Instance()->gIsInvitationAllowed()) {
				m_matchingDlg = CreateForm<FrInviteDlg>(g_pFresh->GetManager(), this, "invite", NULL);

				m_matchingDlg->SetInvaiteInfo(msg.m_param1, (unsigned char)msg.m_param2, (unsigned short)msg.m_param3, msg.m_param4, CSharedDoc::Instance()->m_myUID);

				// lobbymain.cpp:3859
				m_matchingDlg->SetMessage(MakeStr("(%s)\xB4\xD4\xB2\xB2\xBC\xAD \xB4\xE7\xBD\xC5\xC0\xBB \xC3\xCA\xB4\xEB\xC7\xCF\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.\n\xC3\xCA\xB4\xEB\xBF\xA1 \xC0\xC0\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", ((std::string*)msg.m_param5)->c_str(), 0), false);

				m_matchingDlg->SetTimeLimit(5.0f);
				m_matchingDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnInviteDlgResult, 1);
			}
		}
		break;


	case 0x31: {
		// lobbymain.cpp:3880, the banish vote (not for the room master nor
		// the player it is about)
		unsigned long guid = CSharedDoc::Instance()->GetPangyaUserInfo().guid;
		if (guid != m_roomMasterUid && guid != (unsigned long)msg.m_param2) {
			FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(), this, "banish_vote", NULL);
			form->SetMessage((const char*)msg.m_param1, false);
			form->SetTimeLimit(5.0f);
			form->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnVoteDlgResult, 1);
		}
		break;
	}

	case 0x23: {
		// lobbymain.cpp:3892, a topmost notice
		FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		form->SetMessage((const char*)msg.m_param1, false);
		form->SetTimeLimit(5.0f);
		form->Open((FRESH_RESULT_CALLBACK)0, 3);
		form->SetTopmost(true);
		form->FindNextTopFocus(false);
		break;
	}

	case 0x25: {
		// lobbymain.cpp:3909, a notice that quits
		FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		form->SetMessage((const char*)msg.m_param1, false);
		form->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnNotifyQuitDlgResult, 3);
		break;
	}

	case 0x27:
		// lobbymain.cpp:3916
		strcpy(m_storedText, (const char*)msg.m_param1);
		break;

	case 0x28:
		// lobbymain.cpp:3920
		*(char**)msg.m_param1 = m_storedText;
		break;

	case 0x29: {
		// lobbymain.cpp:3926, a player's ready state (param2: 0 ready, 1 not ready, 2 refused)
		unsigned long uid = msg.m_param1;
		int state = msg.m_param2;
		if (state == 2) {
			m_roomOptionB = false;
			this << MsgObject(this, 0x23, (intptr_t)"\xC1\xF6\xB1\xDD\xC0\xBA \xB7\xB9\xB5\xF0\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9. \xC0\xE1\xBD\xC3\xC8\xC4 \xB4\xD9\xBD\xC3 \xB7\xB9\xB5\xF0\xC7\xD8\xC1\xD6\xBC\xBC\xBF\xE4.", 0, 0, 0, 0);
			break;
		}

		if (uid == WSingleton<CSharedDoc>::Instance()->m_userInfo.guid || uid == 0xFFFFFFFF) {
			m_roomOptionB = false;
			m_roomOptionA = state == 0;
			m_tipBlinking = state == 1;
			m_tipBlinkTime = -10.0f;

			if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 1 || WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 5) {
				if (m_changeTeamBtn)
					m_changeTeamBtn->Enable(!m_roomOptionA);
			}

			if (m_roomUserListBox)
				m_roomUserListBox->UseRightButton(!m_roomOptionA);

			if (m_userListDlg)
				m_userListDlg->EnableRightButton(!m_roomOptionA);
		}

		std::list<sSlotInfo>& players = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList;
		std::list<sSlotInfo>::iterator it;
		if (uid == 0xFFFFFFFF) {
			for (it = players.begin(); it != players.end(); ++it)
				it->m_flags55.flag1 = state == 0;
		} else {
			it = std::find(players.begin(), players.end(), uid);
			if (it != players.end())
				it->m_mannerFlag ^= (((state == 0) << 1) ^ it->m_mannerFlag) & 2;
		}

		bool isMaster = false;
		for (it = players.begin(); it != players.end(); ++it) {
			if (it->m_attribute & 0x18) {
				if (it->m_uid == WSingleton<CSharedDoc>::Instance()->m_userInfo.guid)
					isMaster = true;
				break;
			}
		}

		if (AdminMode())
			isMaster = true;

		SetStartBtn(isMaster);
		SetRoomControls(isMaster);
		break;
	}

	case 0x46: {
		// lobbymain.cpp:4017, a room player changed team: param1 the UID,
		// param2 the new team
		std::list<sSlotInfo>::iterator it = std::find(CSharedDoc::Instance()->m_roomPlayerList.begin(), CSharedDoc::Instance()->m_roomPlayerList.end(), msg.m_param1);
		if (it == CSharedDoc::Instance()->m_roomPlayerList.end())
			break;

		if (m_changeTeamBtn)
			m_changeTeamBtn->SetButtonImg((it->m_attribute & 3) == 0 ? "btn_teamchange_blue_n" : "btn_teamchange_red_n", FrButton::NORMAL);

		if (CSharedDoc::Instance()->m_roomInfo.m_gameType != 5)
			break;

		sSlotInfo* pSlot = &*it;
		if (msg.m_param2 == 0) {
			// lobbymain.cpp:4030
			m_slotList1.push_back(pSlot);
			for (std::list<sSlotInfo*>::iterator team = m_slotList2.begin(); team != m_slotList2.end(); ++team) {
				if ((*team)->m_uid == pSlot->m_uid) {
					m_slotList2.erase(team);
					break;
				}
			}
		} else {
			// lobbymain.cpp:4043
			m_slotList2.push_back(pSlot);
			for (std::list<sSlotInfo*>::iterator team = m_slotList1.begin(); team != m_slotList1.end(); ++team) {
				if ((*team)->m_uid == pSlot->m_uid) {
					m_slotList1.erase(team);
					break;
				}
			}
		}

		// lobbymain.cpp:4055
		RemakeTeam(false);
		break;
	}

	case 0x49: {
		// lobbymain.cpp:4066, a room player's character changed: param1 the
		// UID, param2 the sCharacterInfo
		unsigned long uid = msg.m_param1;
		sCharacterInfo* pInfo = (sCharacterInfo*)msg.m_param2;
		if (uid == MyGuid(true)) {
			CSharedDoc::Instance()->m_myCharacterId = pInfo->id;
			CSharedDoc::Instance()->BuildMyPartTidList(*pInfo);
			SetLevelBar();
		}

		// lobbymain.cpp:4073
		for (std::list<sSlotInfo>::iterator it = CSharedDoc::Instance()->m_roomPlayerList.begin(); it != CSharedDoc::Instance()->m_roomPlayerList.end(); ++it) {
			if (it->m_uid == uid) {
				// lobbymain.cpp:4077
				sRoomSlot* pSlot = &CSharedDoc::Instance()->m_roomSlotMap[uid];
				it->m_characterTypeId = pInfo->typeId;
				pSlot->m_characterInfo = *pInfo;

				if (uid == MyGuid(true)) {
					// lobbymain.cpp:4085
					if (!pSlot->m_partTidList.SetTids(&pSlot->m_characterInfo, 0xFF) || !pSlot->m_partTidList.IsComboValid()) {
						pSlot->m_partTidList.m_tidChar = pSlot->m_characterInfo.typeId;
						pSlot->m_partTidList.SetDefaultTids();
					}

					if (pSlot->m_pPet)
						pSlot->m_pPet->SetModel(pSlot->m_partTidList, pSlot->m_characterInfo.auxParts.tids, 0, 3.0f, 0);

					if (pSlot->m_pPet && (pSlot->m_angelFlag & 2))
						pSlot->m_pPet->AttachAngelWingFx();

					SetLevelBar();
					SetRoomCharControl(BTN_DIR_NONE);
				} else {
					// lobbymain.cpp:4102
					ShowPet(uid);
				}
				break;
			}
		}
		break;
	}

	case 0x4A: {
		// lobbymain.cpp:4115, the local player's caddie: param2 the sCaddieInfo
		unsigned long uid = msg.m_param1;
		if (uid == MyGuid(true)) {
			CSharedDoc::Instance()->m_roomPlayers[0].m_caddieInfo = *(const sCaddieInfo*)msg.m_param2;
			CSharedDoc::Instance()->m_myCaddieId = CSharedDoc::Instance()->m_roomPlayers[0].m_caddieInfo.id;
			SetLevelBar();
		}
		break;
	}

	case 0x4B: {
		// lobbymain.cpp:4127, the local player's club set: param2 the sClubSetRecord
		unsigned long uid = msg.m_param1;
		if (uid == MyGuid(true)) {
			CSharedDoc::Instance()->m_roomPlayers[0].m_clubSet = *(const sClubSetRecord*)msg.m_param2;
			CSharedDoc::Instance()->m_myClubId = CSharedDoc::Instance()->m_roomPlayers[0].m_clubSet.id;
			SetLevelBar();
		}
		break;
	}

	case 0x4C: {
		// lobbymain.cpp:4139, the local player's ball: param2 its type id
		unsigned long uid = msg.m_param1;
		if (uid == MyGuid(true)) {
			CSharedDoc::Instance()->m_myBallId = msg.m_param2;
			CSharedDoc::Instance()->m_roomPlayers[0].m_ballId = msg.m_param2;
			SetLevelBar();
		}
		break;
	}

	case 0x4E: {
		// lobbymain.cpp:4151, the local player's mascot: param2 the sMascotInfo
		unsigned long uid = msg.m_param1;
		if (uid == MyGuid(true)) {
			CSharedDoc::Instance()->m_roomPlayers[0].m_mascot = *(const sMascotInfo*)msg.m_param2;
			CSharedDoc::Instance()->m_myMascotId = CSharedDoc::Instance()->m_roomPlayers[0].m_mascot.id;
			SetLevelBar();
		}
		break;
	}

	case 0x2B:
		// lobbymain.cpp:4160, lock the room controls (param1 == 1)
		m_roomOptionC = msg.m_param1 == 1;
		if (m_startBtn)
			m_startBtn->Enable(!m_roomOptionC);
		if (m_channelRoomCloseBtn)
			m_channelRoomCloseBtn->Enable(!m_roomOptionC);
		m_tipBlinking = !m_roomOptionC;
		if (m_blinkTipArea)
			m_blinkTipArea->SetVisible(!m_roomOptionC);
		break;

	case 0x2C:
		// lobbymain.cpp:4179, one part of the owned item lists arrived; all
		// five (bits 0-4) in: sort them
		CSharedDoc::Instance()->m_lobbyStateValue |= msg.m_param1;
		if (CSharedDoc::Instance()->m_lobbyStateValue == 0x1F)
			CSharedDoc::Instance()->SortAllMyItemList();
		break;

	case 0x2A:
		// lobbymain.cpp:4187
		SortRank();
		break;

	case 0x2D:
		// lobbymain.cpp:4192, the game is over: show the results (the
		// tournament (4) hides the room's rank controls first; game type 10
		// only renumbers the room's players)
		if (!CSharedDoc::Instance()->m_gameOver)
			break;

		switch (CSharedDoc::Instance()->m_roomInfo.m_gameType) {
		case 4:
			SortRank();

			if (m_extFrame) m_extFrame->SetVisible(false);
			if (m_userRankTabArea) m_userRankTabArea->SetVisible(false);
			if (m_userRankListBox) m_userRankListBox->SetVisible(false);
			if (m_guildRankListBox) m_guildRankListBox->SetVisible(false);
			if (m_scoreBtn) m_scoreBtn->SetVisible(false);
			if (m_userActionBtn) m_userActionBtn->SetVisible(false);

			if (!m_resultReportDlg) {
				m_resultReportDlg = CreateForm<FrUniteResultDlg>(g_pFresh->GetManager(), this, "uniteresult", NULL);
				m_resultReportDlg->EnableDrag(false);
				m_resultReportDlg->SetFixed(true);
				m_resultReportDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnResultDlgResult, WPoint(68.0f, 48.0f), 0);
			}

			m_approachGiftTimer = -20.0f;
			break;

		case 10:
			if (m_roomUserListBox) {
				unsigned char index = 1;
				for (FrListBox::ITEM_LIST::iterator it = m_roomUserListBox->m_itemList.begin(); it != m_roomUserListBox->m_itemList.end(); ++it, ++index) {
					sSlotInfo* pSlot = (sSlotInfo*)(*it)->pData;
					if (pSlot)
						pSlot->m_connection = index;
				}
			}

			CSharedDoc::Instance()->m_gameOver = false;
			return;

		default:
			SortRank();

			if (!m_resultReportDlg && CSharedDoc::Instance()->m_roomType != 10) {
				m_resultReportDlg = CreateForm<FrUniteResultDlg>(g_pFresh->GetManager(), this, "uniteresult", NULL);
				m_resultReportDlg->EnableDrag(false);
				m_resultReportDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnResultDlgResult, WPoint(68.0f, 48.0f), 0);
			}

			m_approachGiftTimer = -20.0f;
			break;
		}
		break;

	case 0x12D: {
		// lobbymain.cpp:4264, the level-up item form: param1 the sLevelUpDone
		FrLevelupItemForm* form = CreateForm<FrLevelupItemForm>(g_pFresh->GetManager(), this, "levelupitem", NULL);
		if (form) {
			form->SetData(*(sLevelUpDone*)msg.m_param1);
			form->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnLevelupFormResult, 3);

			m_levelupDlgs.push_back(form);
		}
		break;
	}

	case 0x2E:
		// lobbymain.cpp:4278
		m_chatWndOpen = false;
		break;

	case 0x32:
		// lobbymain.cpp:4282, rebuild the lobby user dialog
		if (m_userListDlg)
			m_userListDlg->MakeUserList(CSharedDoc::Instance()->m_userMap);
		break;

	case 0x33:
		// lobbymain.cpp:4288, room enter answer (1: the room does not exist)
		if (msg.m_param1 == 1) {
			this << MsgObject(this, 0x23, (intptr_t)"\xC7\xF6\xC0\xE7 \xC1\xB8\xC0\xE7\xC7\xCF\xC1\xF6 \xBE\xCA\xB4\xC2 \xB9\xE6\xC0\xD4\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
			break;
		}
		EnableRoomEnterControls(false);
		if (m_dtRoomDlg == NULL && m_roomListBox && m_roomListBox->GetSelected()) {
			FrDtRoomDlg* dlg = CreateForm<FrDtRoomDlg>(g_pFresh->GetManager(), this, "detailroominfo", NULL);
			m_dtRoomDlg = dlg;
			dlg->SetRoomInfo(*(sRoomInfo*)m_roomListBox->GetSelected()->pData);
			m_dtRoomDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnDtRoomDlgResult, 1);
		}
		break;

	case 0x34:
		// lobbymain.cpp:4313, the equipped consumables
		memcpy(CSharedDoc::Instance()->m_equipItem, (const void*)msg.m_param1, sizeof(CSharedDoc::Instance()->m_equipItem));
		break;

	case 0x35:
		// lobbymain.cpp:4329, posted with "TOPPAGE" once the channel is
		// entered from the server list
		if (IsLocalContent((localContentType_t)0x10))
			OpenParanNickChange();
		else
			OpenCaddieWarningForm();
		break;

	case 0x37:
		// lobbymain.cpp:4365, whisper to a player: select them as the
		// chat target and focus the chat input
		if (CSharedDoc::Instance()->AddWhisperPartner(m_chatTargetCombo, (const char*)msg.m_param1))
			m_chatTargetCombo->SetLine(1, (const char*)msg.m_param1, 0, false, 0);
		if (CUserInfo::Instance()->IsVisible())
			CUserInfo::Instance()->Close();
		if (m_userListDlg)
			m_userListDlg->Close(true);
		if (m_chatEdit)
			m_chatEdit->SetKeyFocus(true);
		break;

	case 0x38:
		// lobbymain.cpp:4377, add a whisper partner
		CSharedDoc::Instance()->AddWhisperPartner(m_chatTargetCombo, (const char*)msg.m_param1);
		break;

	case 0x47:
		// lobbymain.cpp:4381, refill the chat target list
		CSharedDoc::Instance()->InitRecentWhisperList(m_chatTargetCombo);
		CSharedDoc::Instance()->InitReservedChatList(m_chatTargetCombo, (eReservedChatPartner)0);
		break;

	case 0x3A:
		// lobbymain.cpp:4387, walks the channel list of a channel with flag
		// 2 (the loop body compiles to nothing)
		if (CSharedDoc::Instance()->m_currentChannel.flags & 2) {
			std::list<sChannelInfo>::iterator it = CSharedDoc::Instance()->m_channelList.begin();

			for (; it != CSharedDoc::Instance()->m_channelList.end(); ++it) {
			}
		}
		break;

	case 0x2F: {
		// lobbymain.cpp:4404, the room's game type: the team and guild
		// controls
		bool isTeam = CSharedDoc::Instance()->m_roomInfo.m_gameType == 1 || CSharedDoc::Instance()->m_roomInfo.m_gameType == 5;
		bool isGuild = CSharedDoc::Instance()->m_roomInfo.m_gameType == 6;

		std::list<sSlotInfo>::iterator it;

		if (!GalleryMode()) {
			it = std::find(CSharedDoc::Instance()->m_roomPlayerList.begin(), CSharedDoc::Instance()->m_roomPlayerList.end(), MyGuid(false));
			if (it == CSharedDoc::Instance()->m_roomPlayerList.end())
				break;
		} else {
			isTeam = false;
		}

		// lobbymain.cpp:4420
		if (m_changeTeamBtn)
			m_changeTeamBtn->Enable(isTeam);
		if (m_guildTab1Area)
			m_guildTab1Area->SetVisible(isGuild);
		if (m_guildTab2Area)
			m_guildTab2Area->SetVisible(isGuild);

		if (m_teamTab1Area) {
			if (isTeam)
				m_teamTab1Area->SetBgImg("bar_redteam");
			else
				m_teamTab1Area->SetBgImg("bar_base");
		}

		if (m_teamTab2Area)
			m_teamTab2Area->SetVisible(isTeam);

		// lobbymain.cpp:4442
		if (isTeam) {
			if (m_changeTeamBtn)
				m_changeTeamBtn->SetButtonImg((it->m_attribute & 3) == 0 ? "btn_teamchange_blue_n" : "btn_teamchange_red_n", FrButton::NORMAL);
		}
		break;
	}

	case 0x3B:
		// lobbymain.cpp:4454, the answer to a room create or enter request:
		// param1 the result code. Failures show a notice; code 12 (no room
		// found for a quick join) creates one itself.
		switch ((unsigned char)msg.m_param1) {
		case 2:
			// "The room is full"
			this << MsgObject(0, 0x23, (intptr_t)"\xC1\xA4\xBF\xF8\xC0\xCC \xC3\xCA\xB0\xFA \xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
			break;

		case 3:
			// "The room does not exist"
			this << MsgObject(0, 0x23, (intptr_t)"\xC1\xB8\xC0\xE7\xC7\xCF\xC1\xF6 \xBE\xCA\xB4\xC2 \xB9\xE6\xC0\xD4\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
			break;

		case 4:
			// "Wrong password"
			this << MsgObject(0, 0x23, (intptr_t)"\xBA\xF1\xB9\xD0\xB9\xF8\xC8\xA3\xB0\xA1 \xC6\xB2\xB7\xC8\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
			break;

		case 7:
		case 9:
			// "The room creation request failed"
			this << MsgObject(0, 0x23, (intptr_t)"\xB9\xE6\xB8\xB8\xB5\xE9\xB1\xE2 \xBF\xE4\xC3\xBB\xC0\xCC \xBD\xC7\xC6\xD0\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
			break;

		case 8:
			// "The game in this room has started"
			this << MsgObject(0, 0x23, (intptr_t)"\xB0\xD4\xC0\xD3 \xC1\xF8\xC7\xE0\xC1\xDF\xC0\xCE \xB9\xE6\xC0\xD4\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
			break;

		case 5:
			// "At your current level you cannot create or enter a room."
			this << MsgObject(0, 0x23, (intptr_t)"\xC7\xF6\xC0\xE7 \xB7\xB9\xBA\xA7\xBF\xA1\xBC\xAD\xB4\xC2 \xB9\xE6\xC0\xBB \xB8\xB8\xB5\xE9\xB0\xC5\xB3\xAA \xB9\xE6\xBF\xA1 \xC0\xD4\xC0\xE5\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
			break;

		case 13:
			// "You must join a guild."
			this << MsgObject(0, 0x23, (intptr_t)"\xB1\xE6\xB5\xE5\xBF\xA1 \xB0\xA1\xC0\xD4\xC7\xD8\xBE\xDF \xC7\xD5\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
			break;

		case 18:
			// "Rooms cannot be opened on the Fantasy PangYa island."
			this << MsgObject(0, 0x23, (intptr_t)"\xC8\xAF\xBB\xF3\xC0\xC7 \xC6\xCE\xBE\xDF\xBC\xB6\xBF\xA1\xBC\xAD\xB4\xC2 \xB9\xE6 \xB0\xB3\xBC\xB3\xC0\xCC \xBA\xD2\xB0\xA1\xB4\xC9\xC7\xD5\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
			break;

		case 12: {
			// lobbymain.cpp:4491, no room to join: a match channel offers to
			// create a match room
			if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80) {
				FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", NULL);
				// "There is no suitable match player at the moment..."; the
				// literal's name keeps only its first 32 of 67 bytes, the
				// rest of the text is not known yet.
				form->SetMessage("\xC7\xF6\xC0\xE7 \xC0\xFB\xB4\xE7\xC7\xD1 \xB8\xC5\xC4\xA1 \xC7\xC3\xB7\xB9\xC0\xCC\xBE\xEE\xB0\xA1 \xBE\xF8\xBD\xC0", false);
				form->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnMatchRoomCreateDlgResult, 0);
				return;
			}

			// lobbymain.cpp:4501
			if ((WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x400)
				&& WSingleton<CSharedDoc>::Instance()->m_myLevel < 11
				&& !AdminMode()) {
				// "Could not find a room you can play in."
				this << MsgObject(0, 0x23, (intptr_t)"\xB0\xD4\xC0\xD3\xC0\xBB \xC7\xD2 \xBC\xF6 \xC0\xD6\xB4\xC2 \xB9\xE6\xC0\xBB \xC3\xA3\xC1\xF6 \xB8\xF8\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
				return;
			}

			// lobbymain.cpp:4507, create a room of the channel's kind
			WSendPacket packet((enumClientPacket)8);
			packet.Encode1(0);

			unsigned char gameType = 0x10;
			unsigned char course = FirstMap();

			if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 2) {
				packet.Encode4(0);
				packet.Encode4(2400000);

				packet.Encode1(WSingleton<CSharedDoc>::Instance()->m_gameTypeNames[4].maxPlayers);

				packet.Encode1(4);
				packet.Encode1(18);
			} else if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80) {
				packet.Encode4(60000);
				packet.Encode4(0);

				packet.Encode1(WSingleton<CSharedDoc>::Instance()->m_gameTypeNames[3].maxPlayers);

				packet.Encode1(3);
				packet.Encode1(9);
			} else if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x4000) {
				packet.Encode4(40000);
				packet.Encode4(0);

				packet.Encode1(WSingleton<CSharedDoc>::Instance()->m_gameTypeNames[7].maxPlayers);

				packet.Encode1(7);
				packet.Encode1(6);
			} else {
				gameType = GetRandGameTypeByCate(m_roomCategory);

				packet.Encode4(GetShotTimeLimitByGameType(gameType));
				packet.Encode4(GetGameTimeLimitByGameType(gameType));

				packet.Encode1(GetGameTypeInfo(gameType)->maxPlayers);

				packet.Encode1(gameType);
				packet.Encode1(GetGameTypeInfo(gameType)->defaultHoles);

				if (gameType == 10) {
					course = 0x7F;
				}
			}

			packet.Encode1(course);
			packet.Encode1(gameType == 10 ? 2 : 0);
			packet.EncodeStr(std::string(WSingleton<CSharedDoc>::Instance()->GetDefaultTitle(gameType)));
			packet.EncodeStr(std::string(""));
			packet.Send(SEND_TO_DEFAULT);

			this << MsgObject(0, 0x16, 1, 0, 0, 0, 0);
			return;
		}

		case 15:
			// "Not enough pins for the pin battle"
			this << MsgObject(0, 0x23, (intptr_t)"\xC6\xCE\xB9\xE8\xC6\xB2\xBF\xA1 \xC7\xCA\xBF\xE4\xC7\xD1 \xC6\xCE\xC0\xCC \xBA\xCE\xC1\xB7\xC7\xD5\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
			break;

		case 17:
			// "Not enough pins for the approach game."
			this << MsgObject(0, 0x23, (intptr_t)"\xBE\xEE\xC7\xC1\xB7\xCE\xC4\xA1\xB0\xD4\xC0\xD3\xBF\xA1 \xC7\xCA\xBF\xE4\xC7\xD1 \xC6\xCE\xC0\xCC \xBA\xCE\xC1\xB7\xC7\xD5\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
			break;
		}

		// lobbymain.cpp:4593
		EnableRoomEnterControls(true);
		break;

	case 0x3D: {
		// lobbymain.cpp:4599, a room player's dive bit: param1 the UID,
		// param2 1 to set it
		std::list<sSlotInfo>::iterator it = std::find(CSharedDoc::Instance()->m_roomPlayerList.begin(), CSharedDoc::Instance()->m_roomPlayerList.end(), msg.m_param1);
		if (it != CSharedDoc::Instance()->m_roomPlayerList.end())
			it->m_attributeBits.dive = msg.m_param2 == 1;
		break;
	}

	case 0x3E:
		// lobbymain.cpp:4609
		OpenCreateInfo(msg.m_param1, (const char*)msg.m_param2);
		break;

	case 0x3F:
		// lobbymain.cpp:4615, hand out the chat input line
		*(const char**)msg.m_param1 = m_chatEdit ? m_chatEdit->GetLine(1, false) : NULL;
		break;

	case 0x40:
		// lobbymain.cpp:4620, nickname change result (0: done)
		if (IsLocalContent((localContentType_t)0x10) && m_paranNickChangeDlg) {
			if (msg.m_param1 == 0)
				m_paranNickChangeDlg->CloseDlg();
			else
				m_paranNickChangeDlg->EnableControls(true);
		}
		if (m_optionDlg) {
			if (msg.m_param1 == 0)
				m_optionDlg->CloseDlg();
			else
				m_optionDlg->EnableChangeNickControls(true);
		}
		break;

	case 0x51: {
		// lobbymain.cpp:4646, a friend request: to the user info form's and
		// the user list's add-friend dialogs
		FrUserInfoForm* pUserInfo = USERINFODLG();
		if (pUserInfo) {
			if (CUserInfo::Instance()->IsVisible() == true) {
				if (pUserInfo->IsOpenAddFriendDlg())
					pUserInfo->SetRequestFriendData((const char*)msg.m_param1, msg.m_param2);
			}

			// lobbymain.cpp:4657
			if (m_userListDlg) {
				if (m_userListDlg->IsOpenAddFriendDlg())
					m_userListDlg->SetRequestFriendData((const char*)msg.m_param1, msg.m_param2);
			}
		}
		break;
	}

	case 0x52: {
		// lobbymain.cpp:4670 (the original tests the form it just found
		// null)
		FrUserInfoForm* pUserInfo = USERINFODLG();
		if (pUserInfo == NULL) {
			if (CUserInfo::Instance()->IsVisible() == true) {
				if (pUserInfo->IsOpenAddFriendDlg())
					pUserInfo->SetRequestFriendData((const char*)msg.m_param1, msg.m_param2);
			}
		}
		break;
	}

	case 0x53:
		// lobbymain.cpp:4686
		OpenNewRecordForm();
		break;

	case 0x56:
		// lobbymain.cpp:4691, start the tutorial
		CSharedDoc::Instance()->m_logoutPending = false;
		CloseAllDialogs();

		CTaskManager::Instance()->ChangeTask("CTutorialTask", "", false);
		AfxPostMsg(0, "TutorialMain", 0, (intptr_t)"TUTORIAL_MAIN", 0, 0, 0);
		break;

	case 0x58:
		// lobbymain.cpp:4700
		m_autoStartArmed = true;
		break;

	case 0x5C: {
		// lobbymain.cpp:4705, end the caddie contract in the open caddie
		// info dialog
		FrCaddieInfoDlg* pDlg = FrStaticCast<FrCaddieInfoDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("caddie_info"));
		if (pDlg == NULL)
			break;

		pDlg->EndContract(msg.m_param1);
		break;
	}

	case 0x5E: {
		// lobbymain.cpp:4720, a guild match hole result: param1 the UID of
		// a team's trophy player, param2 the hole number
		unsigned long uid = msg.m_param1;
		if (CSharedDoc::Instance()->m_trophyUID[0] == uid || CSharedDoc::Instance()->m_trophyUID[1] == uid) {
			sRivalData* pRed = GUILDMATCHUP(0);
			sRivalData* pBlue = GUILDMATCHUP(1);

			if (pRed && pBlue) {
				// lobbymain.cpp:4735
				int hole = msg.m_param2;
				char text[1024];

				if (pRed->m_holeShots[hole - 1] && pBlue->m_holeShots[hole - 1]) {
					if (pRed->m_holeShots[hole - 1] < pBlue->m_holeShots[hole - 1])
						// CP949: "%s won hole %d"
						sprintf(text, "%s\xB4\xD4\xC0\xCC %d\xC8\xA6\xBF\xA1\xBC\xAD \xBD\xC2\xB8\xAE\xC7\xCF\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9", pRed->m_nickname, hole);
					else if (pRed->m_holeShots[hole - 1] > pBlue->m_holeShots[hole - 1])
						// CP949: "%s lost hole %d"
						sprintf(text, "%s\xB4\xD4\xC0\xBA %d\xC8\xA6\xBF\xA1\xBC\xAD \xC6\xD0\xB9\xE8\xC7\xCF\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9", pRed->m_nickname, hole);
					else
						// CP949: "hole %d was a draw"
						sprintf(text, "%d\xC8\xA6\xC0\xBA \xB9\xAB\xBD\xC2\xBA\xCE\xB0\xA1 \xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9", hole);

					// lobbymain.cpp:4758
					this << MsgObject(0, 3, (intptr_t)text, 0xFFFF7878, 0, 0, 0);
				}
			}
		}
		break;
	}

	case 0x5F:
		// lobbymain.cpp:4765
		m_roomMasterUid = msg.m_param1;
		break;

	case 0x61:
		// lobbymain.cpp:4798, caddies to warn about: param1 the count,
		// param2 the records
		m_caddieWarningPending = true;
		memset(m_warningCaddies, 0, sizeof(sCaddieInfo));
		memcpy(m_warningCaddies, (const void*)msg.m_param2, msg.m_param1 * sizeof(sCaddieInfo));
		break;

	case 0xB3:
		// lobbymain.cpp:4781, open the bingo event page once
		if (CSharedDoc::Instance()->m_flag4D84) {
			char url[1024];
			sprintf(url, "www.pangya.com/pang_event_bingo.asp?userid=%s", CSharedDoc::Instance()->m_userInfo.id);

			WSingleton<CBrowser>::Instance()->Open(url, false, false);
			CSharedDoc::Instance()->m_flag4D84 = false;
		}
		break;

	case 0x1F7:
		// lobbymain.cpp:4808
		GameExtResQuit();
		break;

	case 0x201:
		// lobbymain.cpp:4829, the point event dialog
		if (IsLocalContent((localContentType_t)0x15)) {
			if (m_pointEventDlg == NULL)
				m_pointEventDlg = CreateForm<FrPointEventDlg>(g_pFresh->GetManager(), this, "point_event_dlg", NULL);

			if (m_pointEventDlg) {
				m_pointEventDlg->SetMyPoint(CSharedDoc::Instance()->m_userInfo.pointEventPoints);
				m_pointEventDlg->SetIconRect(m_toppagePointEventBtn->GetRect());
				m_pointEventDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnPointEventDlgResult, 0x11);
				// CP949: "check your points and join the event"
				m_pointEventDlg->SetDesc("\xC7\xD8\xB4\xE7 \xC6\xF7\xC0\xCE\xC6\xAE\xB8\xA6 \xC8\xAE\xC0\xCE\xC7\xCF\xBD\xC3\xB0\xED \xC0\xCC\xBA\xA5\xC6\xAE\xBF\xA1 \xC2\xFC\xBF\xA9\xC7\xCF\xBC\xBC\xBF\xE4.");
			}
		}
		break;

	case 0x21C:
		// lobbymain.cpp:4866, a match request from another player: param1 the
		// requester's name, param2 the game type, param3 the requester's UID
		if (IsLocalContent((localContentType_t)0x59)) {
			std::string name(*(const std::string*)msg.m_param1);
			const char* gameTypeName = GetLobbyGameTypeName(*(const unsigned char*)msg.m_param2);
			unsigned long fromUid = *(const unsigned long*)msg.m_param3;
			if (m_matchingDlg == 0 && CSharedDoc::Instance()->m_userInfoMod == 0) {
				FrInviteDlg* dlg = CreateForm<FrInviteDlg>(g_pFresh->GetManager(), this, "invite", NULL);
				m_matchingDlg = dlg;
				dlg->SetInvaiteInfo(0, 0, 0, fromUid, CSharedDoc::Instance()->m_myUID);
				// CP949: "(%s) requests a %s mode match. Accept the match?"
				m_matchingDlg->SetMessage(MakeStr("(%s)\xB4\xD4\xB2\xB2\xBC\xAD %s\xB8\xF0\xB5\xE5 \xB8\xC5\xC4\xAA \xBF\xE4\xC3\xBB\xC0\xBB\xC7\xCF\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.\n\xB8\xC5\xC4\xAA\xBF\xA1 \xC0\xC0\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", name.c_str(), gameTypeName), false);
				m_matchingDlg->SetTimeLimit(5.0f);
				m_matchingDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnMatchingDlgResult, 1);
			} else {
				// lobbymain.cpp:4885, refuse it: packet 0xB8 (not named in packet.h yet)
				WSendPacket packet((enumClientPacket)0xB8);
				packet.Encode4(fromUid);
				packet.Encode1(0);
				packet.Send(SEND_TO_DEFAULT);
			}
		}
		break;

	case 0x21D:
		// lobbymain.cpp:4895, the quick start search result: param1 0 enables
		// the OK button, param2 the text
		if (IsLocalContent((localContentType_t)0x59)) {
			if (m_quickResultDlg) {
				m_quickResultDlg->SetContents((const char*)msg.m_param2);

				if (msg.m_param1 == 0)
					m_quickResultDlg->SetOkbtnEnable(true);
			}

			CSharedDoc::Instance()->m_userInfoMod = 0;
		}
		break;

	case 0x230:
		// lobbymain.cpp:4917
		if (m_userListDlg)
			m_userListDlg->CloseInviteDelayDlg();
		break;

	case 0x240:
		// lobbymain.cpp:4926, the gift event notice
		if (IsLocalContent((localContentType_t)0x72)) {
			FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
			WRect rect = form->GetRect();
			form->SetRect(WRect(rect.x, rect.y, rect.w, rect.h));
			form->SetTimeLimit(12.0f);
			// CP949: "-\\c0xffff0000\\c<prize event>\\c0xff000000\\c: you were entered automatically"
			form->SetMessage("-\\c0xffff0000\\c\xB0\xE6\xC7\xB0 \xC0\xCC\xBA\xA5\xC6\xAE\\c0xff000000\\c\xBF\xA1 \xC0\xDA\xB5\xBF\xC0\xB8\xB7\xCE \xC0\xC0\xB8\xF0\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9", false);
			form->Open(NULL, 0);
		}
		break;

	case 0x248:
		// lobbymain.cpp:4940, the valentine event dialog
		if (IsLocalContent((localContentType_t)0x78) == true) {
			if (m_valEventDlg == NULL)
				m_valEventDlg = CreateForm<FrValEventDlg>(g_pFresh->GetManager(), this, "valentine_event_s4", NULL);

			if (m_valEventDlg) {
				((FrValEventDlg*)m_valEventDlg)->SetGiftFlag(msg.m_param1);

				m_valEventDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnValEventDlgResult, 0x19);
			}
		}
		break;

	case 0x249:
		// lobbymain.cpp:4959, the nickname creation: param1 1 opens the
		// dialog (or sets the nick), 0 the name was accepted, 3 forced change
		if (msg.m_param1 == 1) {
			if (m_createNickDlg == NULL) {
				m_createNickDlg = CreateForm<FrCreateNickDlg>(g_pFresh->GetManager(), this, "createnick", NULL);
				if (m_createNickDlg)
					m_createNickDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnCreateNickResult, 0);
			} else {
				m_createNickDlg->SetNick((const char*)msg.m_param2);
			}
		} else if (msg.m_param1 == 0) {
			if (m_createNickDlg) {
				strcpy(CSharedDoc::Instance()->m_userInfo.nickname, (const char*)msg.m_param2);
				m_createNickDlg->CloseDlg(1);
				m_createNickDlg = NULL;
			}
		} else if (msg.m_param1 == 3) {
			if (IsLocalContent((localContentType_t)0x81)) {
				if (m_createNickDlg == NULL) {
					m_createNickDlg = CreateForm<FrCreateNickDlg>(g_pFresh->GetManager(), this, "createnick", NULL);
					if (m_createNickDlg) {
						m_createNickDlg->SetNick(CSharedDoc::Instance()->m_userInfo.nickname);
						m_createNickDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnCreateNickResult, 0);
					}
				}
			}
		}
		break;

	case 0x6D:
		// lobbymain.cpp:5004, the ticket exchange answer
		if (m_ticketExchangeDlg) {
			m_ticketExchangeDlg->Reponse(msg.m_param1, (unsigned char)msg.m_param2);
		}
		break;

	case 0x6E:
		// lobbymain.cpp:5013, the nickname check result
		if (IsLocalContent((localContentType_t)0x81)) {
			if (msg.m_param1 == 0) {
				if (m_createNickDlg)
					m_createNickDlg->ReceiveCheckCode(msg.m_param2);
			}
		}
		break;

	case 0x6F:
		// lobbymain.cpp:5032, the mission event: param1 0 opens the dialog,
		// 1 reports a reward (param2 its result)
		if (IsLocalContent((localContentType_t)0x83)) {
			if (msg.m_param1 == 0) {
				m_missionEventDlg = CreateForm<FrMissionEventDlg>(g_pFresh->GetManager(), this, "mission_event", NULL);
				if (m_missionEventDlg) {
					m_missionEventDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnMissionEventResult, 1);
					m_missionEventDlg->EnableDrag(false);
				}
			} else if (msg.m_param1 == 1) {
				const char* text = NULL;
				switch (msg.m_param2) {
				case 0:
					// CP949: "the item was sent by mail, check your mailbox"
					text = "\xBB\xF3\xC7\xB0\xC0\xCC \xBF\xEC\xC6\xED\xC0\xB8\xB7\xCE \xC0\xFC\xB4\xDE\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9. \xBF\xEC\xC6\xED\xC7\xD4\xC0\xBB \xC8\xAE\xC0\xCE\xC7\xCF\xBC\xBC\xBF\xE4.";
					break;
				case 1:
					// CP949: "already received"
					text = "\xC0\xCC\xB9\xCC \xB9\xDE\xC0\xB8\xBC\xCC\xB3\xD7\xBF\xE4.";
					break;
				case 2:
					// CP949: "the item could not be sent, please apply again"
					text = "\xBB\xF3\xC7\xB0\xC0\xCC \xC0\xFC\xB4\xDE\xC0\xCC \xBD\xC7\xC6\xD0\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9. \xB4\xD9\xBD\xC3 \xC7\xD1\xB9\xF8 \xBD\xC5\xC3\xBB\xC7\xD8\xC1\xD6\xBC\xBC\xBF\xE4.";
					break;
				}

				this << MsgObject(0, 0x23, (intptr_t)text, 0, 0, 0, 0);
			}
		}
		break;

	case 0x70:
		// lobbymain.cpp:5069, the bingo event: param1 0 opens the dialog with
		// the sBingoInfo (param2), 1 a drawn number, 2 the gifts
		if (IsLocalContent((localContentType_t)0x85)) {
			if (msg.m_param1 == 0) {
				m_bingoEventDlg = CreateForm<FrBingoEventDlg>(g_pFresh->GetManager(), this, "bingo_event", NULL);
				if (m_bingoEventDlg) {
					m_bingoEventDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnBingoEventResult, 1);
					m_bingoEventDlg->EnableDrag(false);

					m_bingoEventDlg->SetBingoData((GlobalEnum::sBingoInfo*)msg.m_param2);
				}
			} else if (msg.m_param1 == 1) {
				if (m_bingoEventDlg) {
					m_bingoEventDlg->SetBingoData((GlobalEnum::sBingoInfo*)msg.m_param2);
					m_bingoEventDlg->SetSelectedData(msg.m_param3, msg.m_param4, *(unsigned int*)msg.m_param5);
				}
			} else if (msg.m_param1 == 2) {
				if (m_bingoEventDlg) {
					m_bingoEventDlg->SetGiftData((std::vector<unsigned long>*)msg.m_param2,
						*(unsigned int*)msg.m_param3);
				}
			}
		}
		break;

	case 0x250:
		// lobbymain.cpp:5122, the birthday event dialog: param1 the completed
		// missions, param2 the gifts left, param3/param4 the mission lists
		if (IsLocalContent((localContentType_t)0x8E)) {
			std::string skin = "BirthdayEventSkin1";
			if (IsLocalContent((localContentType_t)0x9B))
				skin = "BirthdayEventSkin2";

			m_birthdayEventDlg = CreateForm<FrBirthdayEventDlg>(g_pFresh->GetManager(), this, skin.c_str(), NULL);
			if (m_birthdayEventDlg) {
				unsigned long* pCompleted = (unsigned long*)msg.m_param1, * pRemain = (unsigned long*)msg.m_param2;
				std::string name = "Kooh";
				if (IsLocalContent((localContentType_t)0x94))
					name = "Hana";
				else if (IsLocalContent((localContentType_t)0x9B))
					name = "Arin";

				m_birthdayEventDlg->SetEventName(name);
				m_birthdayEventDlg->SetMissionData(*pCompleted, *pRemain);

				std::vector<unsigned long>* pMissions = (std::vector<unsigned long>*)msg.m_param3;
				std::vector<unsigned long>* pValues = (std::vector<unsigned long>*)msg.m_param4;
				m_birthdayEventDlg->SetMissionInfo(*pMissions, *pValues);
				m_birthdayEventDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnKoohBirthdayEventDlg, 0x11);
			}
		}
		break;

	case 0x251: {
		// lobbymain.cpp:5159, a birthday gift was sent: param1 the gifts left
		unsigned long* pRemain = (unsigned long*)msg.m_param1;

		// CP949: "the item was sent by mail, check your mailbox"
		this << MsgObject(0, 0x23, (intptr_t)"\xBB\xF3\xC7\xB0\xC0\xCC \xBF\xEC\xC6\xED\xC0\xB8\xB7\xCE \xC0\xFC\xB4\xDE\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9. \xBF\xEC\xC6\xED\xC7\xD4\xC0\xBB \xC8\xAE\xC0\xCE\xC7\xCF\xBC\xBC\xBF\xE4.", 0, 0, 0, 0);

		if (m_birthdayEventDlg)
			m_birthdayEventDlg->SetRemainGiftCount(*pRemain);
		break;
	}


	case 0x41:
	case 0x6B:
	case 0xB2:
	case 0xB4:
	case 0x211:
	case 0x21B:
		// ignored by the original (no HandleMsgCommon)
		PG_BOOT_TODOF(("lobbymain.cpp:4620 empty case %#x (of 0x41, 0x6B, 0xB2, 0xB4, 0x211, 0x21B)", (unsigned)(msg.m_msg)));
		break;

	default:
		// lobbymain.cpp:5265
		HandleMsgCommon(msg);
		break;
	}
}

// lobbymain.cpp:5272
void CLobbyMain::_RefreshUccClothes(sSlotInfo* slot, unsigned long typeId, const char* uccKey) {
	if (slot) {
		// lobbymain.cpp:5276, the player's room slot
		CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
		std::map<unsigned long, sRoomSlot>::iterator it = pDoc->m_roomSlotMap.find(slot->m_uid);
		if (it != pDoc->m_roomSlotMap.end() && IsLocalContent((localContentType_t)0x5F)) {
			sRoomSlot* pSlot = &it->second;
			if (pSlot && pSlot->m_pPet) {
				unsigned char index = (unsigned char)((typeId >> 13) & 0x1F);
				if (pSlot->m_partTidList.m_partTids[index] == typeId) {
					unsigned long itemId = pSlot->m_partTidList.m_partIds[index];

					sUccClothes* clothes = UccManager()->FindClothes(itemId);
					if (clothes && strcmpi(clothes->m_uccKey, uccKey) == 0) {
						// lobbymain.cpp:5295, set twice as the original does
						pSlot->m_pPet->GetPetFrame()->SetPart(&pSlot->m_partTidList, typeId, itemId);
						pSlot->m_pPet->GetPetFrame()->SetPart(&pSlot->m_partTidList, typeId, itemId);
					}
				}
			}
		}
	}
}

// lobbymain.cpp:5360
void CLobbyMain::OpenPCBangNoticeForm(void) {
	if ((m_pcbangNoticeDlg == 0 && CLoginInfo::Instance()->IsPcBang())
		|| CSharedDoc::Instance()->m_noticeShown) {
		OnUnderBar_NoticeUp();
		if (m_toppageNoticeBtn && m_noticeDlg) {
			m_noticeDlg->SetIconRect(m_toppageNoticeBtn->GetRect());
		}
	}
}

// lobbymain.cpp:5382
bool CLobbyMain::NeedsChangeNick(void) {
	if (CLoginInfo::Instance()->IsWebLogin()
		&& CSharedDoc::Instance()->GetPangyaUserInfo().nameByEdit == 1
		&& strcmp(CSharedDoc::Instance()->GetPangyaUserInfo().nickname, CSharedDoc::Instance()->m_loginId) == 0) {
		return true;
	}

	return false;
}

// lobbymain.cpp:5394
void CLobbyMain::OpenParanNickChange(void) {
	if (NeedsChangeNick() == true) {
		m_paranNickChangeDlg = CreateForm<FrParanNickDlg>(g_pFresh->GetManager(), this, "changenick_paran", NULL);
		m_paranNickChangeDlg->SetMessage("\xc6\xce\xbe\xdf\xbf\xa1\xbc\xad \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7 \xb4\xeb\xc8\xad\xb8\xed\xc0\xbb \xc0\xd4\xb7\xc2\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.", false);
		m_paranNickChangeDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnParanNickChangeResult, 0);
	} else {
		OpenCaddieWarningForm();
	}
}

// lobbymain.cpp:5409
void CLobbyMain::OpenCaddieWarningForm(void) {
	if (m_caddieWarningPending && CSharedDoc::Instance()->IsOpenFirstLoginAlarm()) {
		m_caddieWarningPending = false;

		for (int i = 0; i < 12; i++) {
			if (m_warningCaddies[i].id != 0) {
				sCaddieInfo info = m_warningCaddies[i];
				memset(&m_warningCaddies[i], 0, sizeof(sCaddieInfo));

				if (info.id != 0) {
					m_caddieWarningDlg = CreateForm<FrCaddieWarningDlg>(g_pFresh->GetManager(), this, "caddie_warning", NULL);
					static_cast<FrCaddieWarningDlg*>(m_caddieWarningDlg)->SetCaddieInfo(info);
					m_caddieWarningDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnCaddieWarningDlgResult, 0x40);
				}
				break;
			}
		}
	} else {
		OpenNoteForm();
	}
}

// lobbymain.cpp:5442
void CLobbyMain::OpenNoteForm(void) {
	if (CSharedDoc::Instance()->IsOpenFirstLoginAlarm() && CSharedDoc::Instance()->m_noteList.size() > 0) {
		if (m_mailBoxDlg == 0) {
			m_mailBoxDlg = CreateForm<FrMailBoxDlg>(g_pFresh->GetManager(), this, "mailbox", NULL);
			m_mailBoxDlg->EnableDrag(false);
			m_mailBoxDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnMailBoxDlgResult, 0);
		}
	}
}

// lobbymain.cpp:5461
void CLobbyMain::OpenEventPrizeForm(unsigned char event) {
	if (m_eventDlg[event] == 0) {
		m_eventDlg[event] = CreateForm<FrEventPrizeDlg>(g_pFresh->GetManager(), this, "notice_popup", NULL);
		static_cast<FrEventPrizeDlg*>(m_eventDlg[event])->SetEvent(event);
		m_eventDlg[event]->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnEventDlgResult, 3);
	}
}

// lobbymain.cpp:5476
void CLobbyMain::OnToppage_Init(PG_MSGARG) {
	CTaskMain::OnInit();

	CSharedDoc::Instance()->ClearUserInfoTimeMap((unsigned long)-1);
	memset(CSharedDoc::Instance()->m_roomPlayers, 0, sizeof(CSharedDoc::Instance()->m_roomPlayers));

	SetRandomBGM();
	TurnOn3DBackground();

	if (IsLocalContent((localContentType_t)0x6A)) {
		m_topPageBtnIndex = CLoginInfo::Instance()->IsPcBang() ? 2 : 1;
	}
}

// lobbymain.cpp:5503
void CLobbyMain::OnToppage_Finish(void) {
	if (CSharedDoc::Instance()->m_paranNickChanged) {
		if (IsLocalContent((localContentType_t)0x10)) {
			CSharedDoc::Instance()->m_paranNickChanged = false;
			OpenEventPrizeForm(5);

			if (CSharedDoc::Instance()->m_flag50F6 == true) {
				this << MsgObject(0, 0x3E, 0x1A000006, (intptr_t)
			"[\xC7\xC7\xC7\xC9 \xBC\xB1\xB9\xB0 \xB8\xDE\xC0\xCF]\n"
			"\n"
			"\xBE\xC8\xB3\xE7~ \xC0\xFC \xB4\xE7\xBD\xC5\xC0\xBB \xBF\xA9\xB1\xE2 \xB5\xA5\xB8\xAE\xB0\xED \xBF\xC2 \xBD\xC3\xB0\xA3\xC0\xC7 \xBF\xE4\xC1\xA4 \xC7\xC7\xC7\xC9\xC0\xCC\xB6\xF3\xB0\xED \xC7\xD5\xB4\xCF\xB4\xD9. \xB9\xE6\xB1\xDD \xBF\xB9\xBB\xDB \xC4\xB3\xB8\xAF\xC5\xCD\xB8\xA6 \xBC\xB1\xC5\xC3\xC7\xDF\xB3\xD7\xBF\xE4. \xB0\xE8\xBC\xD3 \xC0\xDF \xBA\xB8\xBB\xEC\xC6\xEC\xC1\xD6\xBC\xBC\xBF\xE4.\n"
			"\n"
			"\xBE\xC6~ \xB1\xD7\xB8\xAE\xB0\xED \xC0\xCC\xB0\xC5 \xB9\xDE\xC0\xB8\xBC\xBC\xBF\xE4. \xB4\xE7\xBD\xC5\xC0\xCC \xC6\xCE\xBE\xDF\xBF\xA1 \xBA\xB8\xB4\xD9 \xBD\xB1\xB0\xD4 \xC0\xFB\xC0\xC0\xC7\xCF\xB1\xE2 \xC0\xA7\xC7\xD8\xBC\xAD \xB5\xE5\xB8\xAE\xB4\xC2 \xC7\xE0\xBF\xEE\xC0\xC7 \xB8\xF1\xB0\xC9\xC0\xCC\xC0\xD4\xB4\xCF\xB4\xD9.\n"
			"\xC0\xCC \xC7\xE0\xBF\xEE\xC0\xC7 \xB8\xF1\xB0\xC9\xC0\xCC\xB4\xC2 \xB4\xE7\xBD\xC5\xC0\xCC \xB8\xB8\xB5\xE7 \xC4\xB3\xB8\xAF\xC5\xCD\xC0\xC7 \\c0xffff0000\\c\xC1\xA4\xC8\xAE\xB5\xB5\\c0xff000000\\c\xB8\xA6 \xB3\xF4\xC7\xF4\xC1\xD6\xB0\xED \\c0xffff0000\\c\xC6\xCE\xBE\xDF\xC0\xD3\xC6\xD1\xC6\xAE\xC1\xB8\\c0xff000000\\c\xC0\xBB \xB3\xD0\xC7\xF4\xC1\xD6\xB4\xC2 \xBF\xAA\xC7\xD2\xC0\xBB \xC7\xD1\xB4\xE4\xB4\xCF\xB4\xD9.\n"
			"\xC7\xCF\xB3\xAA \xC1\xD6\xC0\xC7\xC7\xD2 \xB0\xCD\xC0\xBA\xBF\xE4~ \xB4\xE7\xBD\xC5\xC0\xC7 \xBD\xC7\xB7\xC2\xC0\xCC \xBE\xEE\xB4\xC0 \xC1\xA4\xB5\xB5 \xBE\xC8\xC1\xA4\xB5\xC7\xB4\xC2 \xB7\xB9\xBA\xA7, \xC1\xEF \\c0xffff0000\\c\xC1\xD6\xB4\xCF\xBE\xEE" "E\\c0xff000000\\c\xB0\xA1 \xB5\xC7\xB8\xE9 \xC8\xBF\xB0\xFA\xB0\xA1 \xBE\xF8\xBE\xEE\xC1\xF6\xB4\xCF\xB1\xEE\xBF\xE4~ \xBE\xCB\xBE\xC6\xB5\xCE\xBD\xC3\xB0\xED\xBF\xE4.\n"
			"\n"
			"\xB1\xD7\xB8\xAE\xB0\xED \xB4\xEB\xC0\xFC\xB8\xF0\xB5\xE5\xBF\xA1 \xB0\xA1\xB1\xE2 \xC0\xFC\xBF\xA1 \xC7\xD1\xB9\xF8 \xC8\xA5\xC0\xDA \xBF\xAC\xBD\xC0\xC7\xCF\xB0\xED \xBD\xCD\xB4\xD9\xB8\xE9 \xC6\xA9\xC5\xE4\xB8\xAE\xBE\xF3 \xB8\xF0\xB5\xE5\xBF\xA1 \xB5\xE9\xBE\xEE\xB0\xA1\xBA\xB8\xBC\xBC\xBF\xE4.\n"
			"\xC2\xF7\xB1\xD9\xC2\xF7\xB1\xD9 \xB8\xF0\xB5\xCE \xB9\xE8\xBF\xEC\xBD\xC3\xB8\xE9 \xBC\xB1\xB9\xB0\xB5\xB5 \xC1\xD8\xB4\xD9\xB0\xED \xC7\xCF\xB3\xD7\xBF\xE4. ^^\n"
			"\n"
			"\xB1\xD7\xB7\xB3 \xB4\xE3\xBF\xA1 \xB6\xC7 \xBA\xC1\xBF\xE4. \xBE\xC8\xB3\xE7", 0, 0, 0);
				CSharedDoc::Instance()->m_flag50F6 = false;
			}
		} else {
			CSharedDoc::Instance()->m_paranNickChanged = false;
			OpenEventPrizeForm(2);
		}
	}

	if (CSharedDoc::Instance()->m_tutorialFinish) {
		this << MsgObject(0, 0xAA, 1, 0, 0, 0, 0);
	}
}

// lobbymain.cpp:5544
void CLobbyMain::OnToppage_Destroy(void) {
}

// lobbymain.cpp:5548
void CLobbyMain::OnToppage_GameInit(intptr_t param) {
	m_toppageGameBtn = DYNAMIC_CAST(FrButton, param);
	if (m_toppageGameBtn) {
		m_toppageGameBtn->SetPushSound("ui_desktop_icon_click");
		m_toppageGameBtn->EnableHover(true, 0.5f);
	}
}

// lobbymain.cpp:5558
void CLobbyMain::OnToppage_GameUp(void) {
	OnUnderBar_GameUp();
}

// lobbymain.cpp:5563
void CLobbyMain::OnToppage_FamilyInit(intptr_t param) {
	m_toppageFamilyBtn = DYNAMIC_CAST(FrButton, param);
	if (m_toppageFamilyBtn) {
		m_toppageFamilyBtn->SetPushSound("ui_desktop_icon_click");
		m_toppageFamilyBtn->EnableHover(true, 0.5f);
	}
}

// lobbymain.cpp:5573
void CLobbyMain::OnToppage_FamilyUp(void) {
	for (int i = 0; i < 4; i++) {
		memset(&WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i], 0, sizeof(sUserInfo));

		if (i < 2) {
			sprintf(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_name, "PLAYER%d", i + 1);
		}
		else {
			WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_nickname[0] = 0;
			WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_name[0] = 0;
		}

		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_level = WSingleton<CSharedDoc>::Instance()->m_myLevel;
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_mascotId = WSingleton<CSharedDoc>::Instance()->m_myMascotId;
		memset(CFamilyMain::ms_itemslotBackup[i], 0, sizeof(CFamilyMain::ms_itemslotBackup[i]));
	}

	WSingleton<CSharedDoc>::Instance()->m_shotTimeLimit = 0;
	SetCurMap(0);
	WSingleton<CSharedDoc>::Instance()->m_roomType = 0; strcpy(WSingleton<CSharedDoc>::Instance()->m_roomName, WSingleton<CSharedDoc>::Instance()->m_gameTypeNames[0].name);
	WSingleton<CSharedDoc>::Instance()->m_holes = 3;
	WSingleton<CSharedDoc>::Instance()->m_gameKind = 0;
	WSingleton<CSharedDoc>::Instance()->m_gameOver = false;
	WSingleton<CMouseCursor>::Instance()->SetActive(false);

	CTaskManager::Instance()->ChangeTask("CFamilyTask", "", false);
	PostMsg(0, "Family", 0, (intptr_t)"FAMILY", 1, 0, 0);
}

// lobbymain.cpp:5606
void CLobbyMain::OnToppage_PangysStudyInit(intptr_t param) {
	m_toppagePangysStudyBtn = DYNAMIC_CAST(FrButton, param);
	if (m_toppagePangysStudyBtn) {
		m_toppagePangysStudyBtn->SetPushSound("ui_desktop_icon_click");
		m_toppagePangysStudyBtn->EnableHover(true, 0.5f);
	}
}

// lobbymain.cpp:5615
void CLobbyMain::OnToppage_PangysStudyUp(void) {
	CloseAllDialogs();

	CTaskManager::Instance()->ChangeTask("CTutorialTask", "", false);
	CTaskManager::Instance()->PostMsg(0, "TutorialMain", 0, (intptr_t)"TUTORIAL_MAIN", 0, 0, 0);
}

// lobbymain.cpp:5623
void CLobbyMain::OnToppage_NoticeInit(intptr_t param) {
	m_toppageNoticeBtn = DYNAMIC_CAST(FrButton, param);
	if (m_toppageNoticeBtn) {
		m_toppageNoticeBtn->SetPushSound("ui_desktop_icon_click");
		m_toppageNoticeBtn->EnableHover(true, 0.5f);
	}
}

// lobbymain.cpp:5633
void CLobbyMain::OnToppage_NoticeUp(void) {
	OnUnderBar_NoticeUp();

	if (m_toppageNoticeBtn && m_noticeDlg) {
		m_noticeDlg->SetIconRect(m_toppageNoticeBtn->GetRect());
	}
}

// lobbymain.cpp:5641
void CLobbyMain::OnToppage_PcbangInit(intptr_t param) {
	m_toppagePcbangBtn = DYNAMIC_CAST(FrButton, param);
	if (m_toppagePcbangBtn) {
		m_toppagePcbangBtn->SetVisible(IsPcBang() ? true : false);
		m_toppagePcbangBtn->Enable(IsPcBang() ? true : false);

		if (IsPcBang()) {
			CIconManager::Instance()->AddTopIconFromOutSide();
		}

		cTopEventIcon((eTopIconID)0).Prepare(m_toppagePcbangBtn);
	}
}

// lobbymain.cpp:5662
void CLobbyMain::OnToppage_PcbangUp(void) {
	CloseAllDialogs();
	m_pcbangNoticeDlg = CreateForm<FrNoticeDlg>(g_pFresh->GetManager(), this, "notice_popup", NULL);
	m_pcbangNoticeDlg->LoadInitFile("pcbang.ini");
	m_pcbangNoticeDlg->SetIconRect(m_toppagePcbangBtn->GetRect());
	m_pcbangNoticeDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnPCBangDlgResult, 0x11);
}

// lobbymain.cpp:5681
void CLobbyMain::OnToppage_RentalExpiredAlramInit(intptr_t param) {
	FrButton* btn = DYNAMIC_CAST(FrButton, param);
	if (btn) {
		if (!S5::RENTALPARTS::IsExistExpiredRentalItem(CSharedDoc::Instance()->m_myPartsList)) {
			btn->Enable(false);
			btn->SetVisible(false);
		}
	}
}

// Not yet declared in shared/s5/SharedUtilities.h (read-only for this unit);
// move this declaration into its RENTALPARTS namespace.
namespace S5 {
namespace RENTALPARTS {
void CollectRentalItem(std::list<sItemInfo>& rentalList, const std::list<sItemInfo>& itemList);
} // namespace RENTALPARTS
} // namespace S5

// lobbymain.cpp:5694
// The expired rental alarm: counts the expired rental parts and tells the
// player in a "notify" form (ok only).
void CLobbyMain::OnToppage_RentalExpiredAlramUp(void) {
	std::list<sItemInfo> rentalList;
	S5::RENTALPARTS::CollectRentalItem(rentalList, CSharedDoc::Instance()->m_myPartsList);

	int expiredCount = 0;
	for (std::list<sItemInfo>::iterator it = rentalList.begin(); it != rentalList.end(); ++it) {
		if (it->m_stateBits.expired)
			expiredCount++;
	}

	FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
	// CP949: "There are <%d> expired outfits.\nYou can check them in My Room and
	// extend their period or delete them."
	form->SetMessage(MakeStr("\xB1\xE2\xB0\xA3\xC0\xCC \xB8\xB8\xB7\xE1\xB5\xC8 \xC0\xC7\xBB\xF3\xC0\xCC <%d>\xB0\xB3 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.\n\xB8\xB6\xC0\xCC\xB7\xEB\xBF\xA1\xBC\xAD \xC8\xAE\xC0\xCE\xC7\xD2 \xBC\xF6 \xC0\xD6\xC0\xB8\xB8\xE7 \xB1\xE2\xB0\xA3\xC0\xBB \xBF\xAC\xC0\xE5\xC7\xCF\xB0\xC5\xB3\xAA \xBB\xE8\xC1\xA6\xC7\xD2 \xBC\xF6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.", expiredCount), false);

	FrButton* cancelBtn = DYNAMIC_CAST(FrButton, form->FindChildByName("cancel"));
	if (cancelBtn)
		cancelBtn->SetVisible(false);

	// the original looks the "ok" button up without using it
	FrButton* okBtn = DYNAMIC_CAST(FrButton, form->FindChildByName("ok"));

	form->Open((FRESH_RESULT_CALLBACK)0, 0);
}

// lobbymain.cpp:5727
void CLobbyMain::OnToppage_Event1Init(intptr_t param) {
	m_toppageEvent1Btn = DYNAMIC_CAST(FrButton, param);
	if (m_toppageEvent1Btn) {
		m_toppageEvent1Btn->SetPushSound("ui_desktop_icon_click");
		m_toppageEvent1Btn->EnableHover(true, 0.5f);
		m_toppageEvent1Btn->SetVisible(false);

		if (IsLocalContent((localContentType_t)0x18)) {
			m_toppageEvent1Btn->SetVisible(true);
		}

		m_event1IconRect = m_toppageEvent1Btn->GetRect();
	}
}

// lobbymain.cpp:5745
void CLobbyMain::OnToppage_Event1Up(void) {
	if (IsLocalContent((localContentType_t)0x18)) {
		CloseAllDialogs();

		if (IsLocalContent((localContentType_t)0x48)) {
			m_holesEventDlg = CreateForm<ntHolesEventDlg>(g_pFresh->GetManager(), this, "holes_EntranceExam", NULL);
		} else {
			m_holesEventDlg = CreateForm<ntHolesEventDlg>(g_pFresh->GetManager(), this, "holes_event", NULL);
		}

		m_holesEventDlg = CreateForm<ntHolesEventDlg>(g_pFresh->GetManager(), this, "holes_event", NULL);
		m_holesEventDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnHolesEventDlgResult, 0x11);
		m_holesEventDlg->SetIconRect(m_toppageEvent1Btn->GetRect());
	}
}

// lobbymain.cpp:5762
void CLobbyMain::OnToppage_NewInit(intptr_t param) {
	m_newArea = DYNAMIC_CAST(FrArea, param);
}

// lobbymain.cpp:5767
// Draws the "new" tab image next to the Pangya's study button.
void CLobbyMain::OnToppage_NewOwnerDraw(PG_MSGARG) {
	if (m_toppagePangysStudyBtn) {
		if (m_newTabBitmap) {
			WRECT rect = m_toppagePangysStudyBtn->GetRect();
			g_pFresh->GetManager()->GetGDI()->DrawTexture(m_newTabBitmap, WRect(rect.x + 58.0f, rect.y + 12.0f, m_newTabBitmap->Width(), m_newTabBitmap->Height()), 0xFFFFFFFF, 0);
		}
	}
}

// lobbymain.cpp:5798
// Draws the attendance-point balloon under the point event button: the
// three "login" pieces and its three lines of text.
void CLobbyMain::OnFirstConnectOwnerDraw(PG_MSGARG) {
	if (!m_toppagePointEventBtn)
		return;

	if (!WSingleton<CSharedDoc>::Instance()->m_pointEventFlag)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (!pGDI)
		return;

	for (int i = 0; i < 3; i++) {
		if (!m_loginBitmaps[i])
			return;
	}

	WRect rect = m_toppagePointEventBtn->GetRect();
	rect.y = rect.y + (rect.h - 10.0f);

	if (m_loginBitmaps[0]) {
		pGDI->DrawTexture(m_loginBitmaps[0], WRect(rect.x, rect.y, rect.h - 15.0f, m_loginBitmaps[0]->Height()), 0xFFFFFFFF, 0);
		rect.y += m_loginBitmaps[0]->Height();
	}

	if (m_loginBitmaps[1]) {
		pGDI->DrawTexture(m_loginBitmaps[1], WRect(rect.x, rect.y, rect.h - 15.0f, m_loginBitmaps[1]->Height() + 18.0f), 0xFFFFFFFF, 0);
		rect.y += m_loginBitmaps[1]->Height() + 18.0f;
	}

	if (m_loginBitmaps[2]) {
		pGDI->DrawTexture(m_loginBitmaps[2], WRect(rect.x, rect.y, rect.h - 15.0f, m_loginBitmaps[2]->Height()), 0xFFFFFFFF, 0);
		rect.y -= (m_loginBitmaps[0]->Height() + m_loginBitmaps[1]->Height()) + 18.0f;
	}

	float x = rect.x + 3.0f;
	pGDI->Print(WPoint(x, rect.y + 8.0f), 0, "\xC3\xE2\xBC\xAE\xC6\xF7\xC0\xCE\xC6\xAE");

	pGDI->Print(WPoint(x, rect.y + 19.0f), 0, "25 \xC6\xF7\xC0\xCE\xC6\xAE");

	pGDI->Print(WPoint(x, rect.y + 30.0f), 0, "\xC8\xB9\xB5\xE6!!");
}

// lobbymain.cpp:5849
bool CLobbyMain::OnValEventDlgResult(int, FrForm*) {
	m_valEventDlg = 0;
	return true;
}

// lobbymain.cpp:5861
// Shows the point event button where the content allows it: in the next top
// page slot, next to the PC bang button, or at x 160 in a PC bang.
void CLobbyMain::OnToppage_PointEventInit(intptr_t param) {
	m_toppagePointEventBtn = DYNAMIC_CAST(FrButton, param);
	if (m_toppagePointEventBtn) {
		if (IsLocalContent((localContentType_t)0x15)) {
			m_toppagePointEventBtn->SetPushSound("ui_desktop_icon_click");
			m_toppagePointEventBtn->EnableHover(true, 0.5f);
			m_toppagePointEventBtn->SetVisible(true);
			m_toppagePointEventBtn->SetPushDelay(0.0f);

			if (IsLocalContent((localContentType_t)0x6A)) {
				WRect rect = m_toppagePointEventBtn->GetRect();
				rect.x = GetTopPageBtnRect();
				m_toppagePointEventBtn->SetRect(rect);
			} else if (!CLoginInfo::Instance()->IsPcBang()) {
				const WRect& pcbangRect = m_toppagePcbangBtn->GetRect();
				float x = pcbangRect.x;
				float y = pcbangRect.y;
				WRect rect = m_toppagePointEventBtn->GetRect();
				rect.x = x;
				rect.y = y - 2.0f;
				m_toppagePointEventBtn->SetRect(rect);
			} else {
				WRect rect = m_toppagePointEventBtn->GetRect();
				rect.y -= 2.0f;
				rect.x = 160.0f;
				m_toppagePointEventBtn->SetRect(rect);
			}
		} else {
			m_toppagePointEventBtn->SetVisible(false);
		}
	}
}

// lobbymain.cpp:5929
void CLobbyMain::OnToppage_PointEventDown(void) {
	if (IsLocalContent((localContentType_t)0x15)) {
		CloseAllDialogs();
		WSendPacket packet(GCT_REQ_PLAYTIME_UPDATE);
		packet.Send(SEND_TO_DEFAULT);

		CSharedDoc::Instance()->m_pointEventState = 2;
		CSharedDoc::Instance()->m_pointEventFlag = false;
	}
}

// lobbymain.cpp:5956
void CLobbyMain::OnToppage_ValEventInit(intptr_t param) {
	m_toppageValEventBtn = DYNAMIC_CAST(FrButton, param);
	if (m_toppageValEventBtn) {
		m_toppageValEventBtn->SetVisible(IsLocalContent((localContentType_t)0x78));
		if (m_toppageValEventBtn->IsVisible()) {
			m_toppageValEventBtn->SetPushDelay(0.1f);
			cTopEventIcon((eTopIconID)8).Prepare(m_toppageValEventBtn);
		}
	}
}

// lobbymain.cpp:5985
// Shows the world tour event button where the content allows it, after the
// point event or PC bang button.
void CLobbyMain::OnToppage_WorldTourEventInit(intptr_t param) {
	m_toppageWorldTourBtn = DYNAMIC_CAST(FrButton, param);
	if (m_toppageWorldTourBtn) {
		if (IsLocalContent((localContentType_t)0x1E)) {
			m_toppageWorldTourBtn->SetPushSound("ui_desktop_icon_click");
			m_toppageWorldTourBtn->EnableHover(true, 0.5f);
			m_toppageWorldTourBtn->SetVisible(true);
			m_toppageWorldTourBtn->SetPushDelay(0.0f);

			bool isPcBang = CLoginInfo::Instance()->IsPcBang() != 0;
			bool hasPointEvent = IsLocalContent((localContentType_t)0x15);

			WRect pcbangRect = m_toppagePcbangBtn->GetRect();
			WRect pointRect = m_toppagePointEventBtn->GetRect();
			WRect rect = m_toppageWorldTourBtn->GetRect();

			rect.y += 2.0f;

			if (IsLocalContent((localContentType_t)0x6A)) {
				WRect slotRect = m_toppageWorldTourBtn->GetRect();
				slotRect.x = GetTopPageBtnRect();
				m_toppageWorldTourBtn->SetRect(slotRect);
			} else if (isPcBang && hasPointEvent) {
				rect.x = pointRect.x + pointRect.w;
				m_toppageWorldTourBtn->SetRect(rect);
			} else if (isPcBang || hasPointEvent) {
				if (isPcBang) {
					rect.x = pcbangRect.x + pcbangRect.w;
					m_toppageWorldTourBtn->SetRect(rect);
				} else {
					rect.x = pointRect.x + pointRect.w;
					m_toppageWorldTourBtn->SetRect(rect);
				}
			}
		} else {
			m_toppageWorldTourBtn->SetVisible(false);
			m_toppageWorldTourBtn->Enable(false);
		}
	}
}

// lobbymain.cpp:6046
// Opens the world tour event dialog on its map flags (or refreshes them).
void CLobbyMain::OnToppage_WorldTourEventDown(void) {
	if (IsLocalContent((localContentType_t)0x1E)) {
		if (!m_toppageWorldTourBtn)
			return;

		CloseAllDialogs();

		CWorldTourEvent* pEvent = (CWorldTourEvent*)WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0x1E);
		if (!pEvent)
			return;

		if (m_worldTourEventDlg) {
			m_worldTourEventDlg->SetMapFlag(pEvent->GetMapFlag());
		} else {
			m_worldTourEventDlg = CreateForm<FrWorldTourEventDlg>(g_pFresh->GetManager(), this, "world_tour_event_dlg", NULL);
			if (m_worldTourEventDlg) {
				m_worldTourEventDlg->SetMapFlag(pEvent->GetMapFlag());
				m_worldTourEventDlg->SetIconRect(m_toppageWorldTourBtn->GetRect());
				m_worldTourEventDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnWorldTourEventDlgResult, 0x11);
			}
		}
	}
}

// lobbymain.cpp:6081
// Shows the find-Ebort event button after the PC bang, point event and world
// tour buttons that are shown.
void CLobbyMain::OnToppage_FindEbortEventInit(intptr_t param) {
	m_toppageFindEbortBtn = DYNAMIC_CAST(FrButton, param);
	if (m_toppageFindEbortBtn) {
		if (IsLocalContent((localContentType_t)0x20)) {
			m_toppageFindEbortBtn->SetVisible(true);
			m_toppageFindEbortBtn->Enable(true);
			m_toppageFindEbortBtn->SetPushDelay(0.0f);

			bool isPcBang = CLoginInfo::Instance()->IsPcBang() != 0;
			bool hasPointEvent = IsLocalContent((localContentType_t)0x15);
			bool hasWorldTour = IsLocalContent((localContentType_t)0x1E);

			WRect rect = m_toppageFindEbortBtn->GetRect();

			if (isPcBang)
				rect.x += m_toppagePcbangBtn->GetRect().w + m_toppagePcbangBtn->GetRect().x;

			if (hasPointEvent)
				rect.x += m_toppagePointEventBtn->GetRect().w + m_toppagePointEventBtn->GetRect().x;

			if (hasWorldTour)
				rect.x += m_toppageWorldTourBtn->GetRect().w + m_toppageWorldTourBtn->GetRect().x;

			rect.y -= 5.0f;
			m_toppageFindEbortBtn->SetRect(rect);
		} else {
			m_toppageFindEbortBtn->SetVisible(false);
			m_toppageFindEbortBtn->Enable(false);
		}
	}
}

// lobbymain.cpp:6121
void CLobbyMain::OnToppage_FindEbortEventDown(void) {
	if (IsLocalContent((localContentType_t)0x20)) {
		if (m_toppageFindEbortBtn) {
			CloseAllDialogs();

			if (!m_findEbortEventDlg) {
				m_findEbortEventDlg = CreateForm<FrFindEbortEventDlg>(g_pFresh->GetManager(), this, "find_ebort_event_dlg", NULL);
				if (m_findEbortEventDlg) {
					m_findEbortEventDlg->SetIconRect(m_toppageFindEbortBtn->GetRect());
					m_findEbortEventDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnFindEbortEventDlgResult, 0x11);
				}
			}
		}
	}
}

// lobbymain.cpp:6144
void CLobbyMain::OnToppage_Halloween2007Init(intptr_t param) {
	m_toppageHalloweenBtn = DYNAMIC_CAST(FrButton, param);
	if (m_toppageHalloweenBtn) {
		if (IsLocalContent((localContentType_t)0x21)) {
			CHalloweenEvent* pEvent;
			if (CContentsDoc::Instance()->GetContainer((localContentType_t)0x21, pEvent)) {
				m_toppageHalloweenBtn->SetVisible(true);
				m_toppageHalloweenBtn->Enable(true);
			} else {
				m_toppageHalloweenBtn->SetVisible(false);
				m_toppageHalloweenBtn->Enable(false);
			}

			cTopEventIcon((eTopIconID)9).Prepare(m_toppageHalloweenBtn);
		} else {
			m_toppageHalloweenBtn->SetVisible(false);
			m_toppageHalloweenBtn->Enable(false);
		}
	}
}

// lobbymain.cpp:6178
void CLobbyMain::OnToppage_Halloween2007EvenDown(void) {
	if (IsLocalContent((localContentType_t)0x21)) {
		if (m_toppageHalloweenBtn) {
			CloseAllDialogs();

			if (!m_halloweenEventDlg) {
				m_halloweenEventDlg = CreateForm<FrHalloweenEventDlg>(g_pFresh->GetManager(), this, "halloween2007_event_dlg", NULL);
				if (m_halloweenEventDlg) {
					m_halloweenEventDlg->SetIconRect(m_toppageHalloweenBtn->GetRect());
					m_halloweenEventDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnHalloweenEventDlgResult, 0x11);
				}
			}
		}
	}
}

// lobbymain.cpp:6201
void CLobbyMain::OnToppage_Christmas2007EventInit(intptr_t param) {
	m_toppageChristmasBtn = DYNAMIC_CAST(FrButton, param);
	if (IsLocalContent((localContentType_t)0x4C)) {
		CChristmasEvent* pEvent;
		if (CContentsDoc::Instance()->GetContainer((localContentType_t)0x4C, pEvent) && pEvent->GetFlag()) {
			m_toppageChristmasBtn->SetVisible(true);
			m_toppageChristmasBtn->Enable(true);

			cTopEventIcon((eTopIconID)7).Prepare(m_toppageChristmasBtn);
			return;
		}
	}

	m_toppageChristmasBtn->SetVisible(false);
	m_toppageChristmasBtn->Enable(false);
}

// lobbymain.cpp:6230
void CLobbyMain::OnToppage_Christmas2007EventDown(void) {
	if (IsLocalContent((localContentType_t)0x4C)) {
		if (m_toppageChristmasBtn) {
			CloseAllDialogs();

			if (!m_christmasEventDlg) {
				m_christmasEventDlg = CreateForm<FrChristmasEventDlg>(g_pFresh->GetManager(), this, "christmas2007_event_dlg", NULL);
				if (m_christmasEventDlg) {
					m_christmasEventDlg->SetIconRect(m_toppageChristmasBtn->GetRect());
					m_christmasEventDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnChristmasEventDlgResult, 0x11);
				}
			}
		}
	}
}

// lobbymain.cpp:6306
// Shows the ticket exchange button while the ticket exchange is open.
void CLobbyMain::OnToppage_TicketExchangeInit(intptr_t param) {
	m_toppageTicketExchangeBtn = DYNAMIC_CAST(FrButton, param);
	if (IsLocalContent((localContentType_t)0x7A)) {
		CTicketExchange* pExchange;
		if (WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0x7A, pExchange)) {
			if (pExchange->GetFlag()) {
				m_toppageTicketExchangeBtn->SetVisible(true);
				m_toppageTicketExchangeBtn->Enable(true);

				cTopEventIcon((eTopIconID)10).Prepare(m_toppageTicketExchangeBtn);

				m_toppageTicketExchangeBtn->SetPushSound("ui_desktop_icon_click");
				m_toppageTicketExchangeBtn->EnableHover(true, 0.5f);
				m_toppageTicketExchangeBtn->SetVisible(true);
				m_toppageTicketExchangeBtn->SetPushDelay(0.0f);

				if (IsLocalContent((localContentType_t)0x6A)) {
					WRect rect = m_toppageTicketExchangeBtn->GetRect();
					rect.x = GetTopPageBtnRect();
					m_toppageTicketExchangeBtn->SetRect(rect);
				}
				return;
			}
		}
	}

	m_toppageTicketExchangeBtn->SetVisible(false);
	m_toppageTicketExchangeBtn->Enable(false);
}

// lobbymain.cpp:6349
void CLobbyMain::OnToppage_TicketExchangeDown(void) {
	if (IsLocalContent((localContentType_t)0x7A)) {
		if (m_toppageTicketExchangeBtn) {
			CloseAllDialogs();

			if (!m_ticketExchangeDlg) {
				m_ticketExchangeDlg = CreateForm<FrTicketExchangeDlg>(g_pFresh->GetManager(), this, "ticket_exchage_dlg", NULL);
				if (m_ticketExchangeDlg) {
					m_ticketExchangeDlg->SetIconRect(m_ticketExchangeDlg->GetRect());
					m_ticketExchangeDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnTicketExchangeDlgResult, 0x11);
				}
			}
		}
	}
}

// lobbymain.cpp:6374
void CLobbyMain::OnToppage_MissionEventInit(intptr_t param) {
	m_toppageMissionBtn = DYNAMIC_CAST(FrButton, param);
	cTopEventIcon((eTopIconID)13).Prepare(m_toppageMissionBtn);

	if (!IsLocalContent((localContentType_t)0x83)) {
		m_toppageMissionBtn->Enable(false);
		m_toppageMissionBtn->SetVisible(false);
	}
}

// lobbymain.cpp:6389
void CLobbyMain::OnToppage_MissionEventDown(void) {
	if (!IsLocalContent((localContentType_t)0x83)) {
		return;
	}

	WSendPacket packet(GCT_REQ_MISSION_EVENT_OPEN);
	packet.Send(SEND_TO_DEFAULT);

	this << MsgObject(this, 1, 0, 0, 0, 0, 0);
}

// lobbymain.cpp:6402
void CLobbyMain::OnToppage_BingoEventInit(intptr_t param) {
	m_toppageBingoBtn = DYNAMIC_CAST(FrButton, param);
	cTopEventIcon((eTopIconID)12).Prepare(m_toppageBingoBtn);

	if (!IsLocalContent((localContentType_t)0x85)) {
		m_toppageBingoBtn->Enable(false);
		m_toppageBingoBtn->SetVisible(false);
	}
}

// lobbymain.cpp:6417
void CLobbyMain::OnToppage_BingoEventDown(void) {
	if (!IsLocalContent((localContentType_t)0x85)) {
		return;
	}

	WSendPacket packet(GCT_REQ_BINGO_EVENT_INFO);
	packet.Encode4(0);
	packet.Send(SEND_TO_DEFAULT);

	this << MsgObject(this, 1, 0, 0, 0, 0, 0);
}

// lobbymain.cpp:5973
void CLobbyMain::OnToppage_ValEventDown(void) {
	if (IsLocalContent((localContentType_t)0x78) == true) {
		WSendPacket packet(GCT_VALENTINE_OPEN_REQ);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// lobbymain.cpp:6431
// Skins the birthday event button (Kooh's, Hana's or Arin's) from its bitmap
// box, or hides it when no birthday event runs.
void CLobbyMain::OnToppage_KoohBirthdayEventInit(intptr_t param) {
	m_toppageKoohBirthdayBtn = DYNAMIC_CAST(FrButton, param);

	std::string boxName("");
	if (IsLocalContent((localContentType_t)0x8E)) {
		boxName = "KoohBirthdayEvent";
	}

	if (IsLocalContent((localContentType_t)0x94)) {
		boxName = "HanaBirthdayEvent";
	} else if (IsLocalContent((localContentType_t)0x9B)) {
		boxName = "ArinBirthdayEvent";
	}

	if (boxName != "") {
		CGDBitmapBox* pBox = (CGDBitmapBox*)WSingleton<CGameDataDB>::Instance()->GetGameData(eGAMEDATADB_BITMAPBOX, boxName.c_str());
		m_toppageKoohBirthdayBtn->SetButtonImg(pBox->GetFileName("LobbyIconNormal"), FrButton::NORMAL);
		m_toppageKoohBirthdayBtn->SetButtonImg(pBox->GetFileName("LobbyIconOver"), FrButton::OVER);
		m_toppageKoohBirthdayBtn->SetButtonImg(pBox->GetFileName("LobbyIconBlink"), FrButton::BLINK);

		cTopEventIcon((eTopIconID)6).Prepare(m_toppageKoohBirthdayBtn);
	} else {
		m_toppageKoohBirthdayBtn->Enable(false);
		m_toppageKoohBirthdayBtn->SetVisible(false);
	}
}

// lobbymain.cpp:6469
void CLobbyMain::OnToppage_KoohBirthdayEventDown(void) {
	WSendPacket packet(GCT_KOOH_BIRTHDAY_BINGO);
	packet.Send(SEND_TO_DEFAULT);
	this << MsgObject(this, 1, 0, 0, 0, 0, 0);
}

// lobbymain.cpp:6477
void CLobbyMain::OnToppage_TreasureIslandInit(intptr_t param) {
	m_toppageTreasureBtn = DYNAMIC_CAST(FrButton, param);
	cTopEventIcon((eTopIconID)11).Prepare(m_toppageTreasureBtn);

	if (!IsLocalContent((localContentType_t)0x8C)) {
		m_toppageTreasureBtn->Enable(false);
		m_toppageTreasureBtn->SetVisible(false);
	}
}

// lobbymain.cpp:6492
void CLobbyMain::OnToppage_TreasureIslandLBDown(void) {
	if (IsLocalContent((localContentType_t)0x8C)) {
		ShellExecuteA(NULL, NULL, "http://qa.pangya.gametree.co.kr/events/bomul/PangyaBomul.aspx", NULL, NULL, SW_SHOWMAXIMIZED);
	}
}

// lobbymain.cpp:6647
void CLobbyMain::OnToppage_ServerInit(intptr_t param) {
	m_toppageServerBtn = DYNAMIC_CAST(FrButton, param);
	if (m_toppageServerBtn) {
		m_toppageServerBtn->SetPushSound("ui_desktop_icon_click");
		m_toppageServerBtn->EnableHover(true, 0.5f);
	}
}

// lobbymain.cpp:6657
void CLobbyMain::OnToppage_ServerUp(void) {
	CloseAllDialogs();
	OnUnderBar_ServerUp();
}

// lobbymain.cpp:6664
void CLobbyMain::CloseAllDialogs(void) {
	if (m_optionDlg)
		m_optionDlg->Close(true);

	if (m_pcbangNoticeDlg)
		m_pcbangNoticeDlg->Close(true);

	if (CMessengerInfo::Instance()->IsVisible())
		CMessengerInfo::Instance()->Toggle();

	if (CUserInfo::Instance())
		CUserInfo::Instance()->Close();

	if (m_serverDlg)
		m_serverDlg->Close(false);

	if (m_pointEventDlg)
		m_pointEventDlg->Close(true);

	if (m_createNickDlg)
		m_createNickDlg->CloseDlg(0);

	CIconToolTip::GetInstance()->ClearToolTip();

	CTaskMain::CloseAllDialogs();
}

// lobbymain.cpp:6705
void CLobbyMain::OnServer_Init(PG_MSGARG) {
	CTaskMain::OnInit();
	SetRandomBGM();
	TurnOn3DBackground();
}

// lobbymain.cpp:6713
void CLobbyMain::OnServer_Finish(void) {
	if (m_serverDlg == 0) {
		m_serverDlg = CreateForm<FrServerDlg>(g_pFresh->GetManager(), this, "server", NULL);

		m_serverDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnServerDlgResult, 0x10);
	}
}

// lobbymain.cpp:6729
// Answer to the "create a match room" question: 1 creates a 60-second match
// room (packet 8, game type 3, 9 holes on the first course) and tells the
// lobby (message 0x16); anything else re-enables the room controls.
bool CLobbyMain::OnMatchRoomCreateDlgResult(int result, FrForm*) {
	if (result == 1) {
		WSendPacket packet((enumClientPacket)8);

		packet.Encode1(0);
		packet.Encode4(60000);
		packet.Encode4(0);

		packet.Encode1(WSingleton<CSharedDoc>::Instance()->m_gameTypeNames[3].maxPlayers);

		packet.Encode1(3);
		packet.Encode1(9);
		packet.Encode1(FirstMap());
		packet.Encode1(0);

		// CP949: "A fine match"
		packet.EncodeStr(std::string("\xB8\xDA\xC1\xF8 \xB4\xEB\xC0\xFC \xC7\xD1\xC6\xC7"));
		packet.EncodeStr(std::string(""));
		packet.Send(SEND_TO_DEFAULT);

		this << MsgObject(0, 0x16, 1, 0, 0, 0, 0);
	} else {
		EnableRoomEnterControls(true);
	}

	m_reportDlg = 0;
	return true;
}

// lobbymain.cpp:6763
bool CLobbyMain::OnNewRecordDlgResult(int, FrForm*) {
	m_newRecordDlg = 0;
	return true;
}

// lobbymain.cpp:6775
void CLobbyMain::OnLogin_Init(PG_MSGARG) {
	CTaskMain::OnInit();

	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_MSN)) {
		CMessengerInfo::Instance()->Close(true);
		WNetworkSystem::Instance()->ForceShutDown(WNetworkSystem::NET_UNIT_MSN);
	}

	CSharedDoc::Instance()->ClearAll();
	BuddyMGR->ClearBuddy();

	SetRandomBGM();
	TurnOn3DBackground();
}

// lobbymain.cpp:6792
void CLobbyMain::OnLogin_Finish(void) {
	g_mouse->ResetInputTime();
	g_ime->ResetInputTime();
	WSingleton<CMouseCursor>::Instance()->SetActive(true);

	if (!WSingleton<CProjectG>::Instance()->CheckUIHack()) {
		m_scrollActive = true;

		FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		// "A modified file was found. Remove the file or reinstall PangYa, then run it again."
		form->SetMessage("\xBA\xAF\xC1\xB6\xB5\xC8 \xC6\xC4\xC0\xCF\xC0\xCC \xB9\xDF\xB0\xDF\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9. \xC7\xD8\xB4\xE7 \xC6\xC4\xC0\xCF\xC0\xBB \xC1\xA6\xB0\xC5 \xC7\xCF\xB0\xC5\xB3\xAA \xC6\xCE\xBE\xDF\xB8\xA6 \xC0\xE7\xBC\xB3\xC4\xA1 \xC8\xC4 \xB4\xD9\xBD\xC3 \xBD\xC7\xC7\xE0\xC7\xD8\xC1\xD6\xBD\xC3\xB1\xE2 \xB9\xD9\xB6\xF8\xB4\xCF\xB4\xD9.", false);
		form->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnUIHackDlgResult, 0);
		return;
	}

	m_loginDlg = CreateForm<FrLoginDlg>(g_pFresh->GetManager(), this, "login", NULL);
	m_loginDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnLoginDlgResult, 10);

	WSingleton<CCardManager>::Instance()->InitCardBuffDlgPosition();
	CItemBuff::ResetBuffList();
}

// lobbymain.cpp:6815
void CLobbyMain::OnBlank_Init(PG_MSGARG) {
	CTaskMain::OnInit();
}

// lobbymain.cpp:6820
void CLobbyMain::OnBlank_Finish(void) {
	unsigned char forceNickname = CSharedDoc::Instance()->GetForceNicknameChange();
	if (forceNickname == 1 || forceNickname == 2) {
		GetActor("Lobby") << MsgObject(0, 0x249, 1, 0, 0, 0, 0);
	}
}

// lobbymain.cpp:6833
void CLobbyMain::OnSecond_Pwd_Init(PG_MSGARG) {
	CTaskMain::OnInit();
}

// lobbymain.cpp:6838
void CLobbyMain::OnSecond_Pwd_Finish(void) {
}

// lobbymain.cpp:6852
bool CLobbyMain::OnMakeRoomDlgResult(int ret, FrForm*) {
	if (ret == FrOK) {
		if (m_makeRoomDlg) {
			if (LatestVersion() && !m_makeRoomDlg->MakeRoom(false)) {
				return false;
			}
		}
	} else {
		EnableRoomEnterControls(true);
	}

	m_makeRoomDlg = 0;
	return true;
}

// lobbymain.cpp:6873
bool CLobbyMain::OnRemoveFriendDlgResult(int ret, FrForm*) {
	if (ret == 1) {
		BuddyMGR->GetBuddy(m_removeFriendUid);
		m_removeFriendUid = (unsigned long)-1;
	}
	return true;
}

// lobbymain.cpp:6888
bool CLobbyMain::OnEventDlgResult(int, FrForm* form) {
	m_eventDlg[static_cast<FrEventPrizeDlg*>(form)->GetEvent()] = 0;
	return true;
}

// lobbymain.cpp:6899
bool CLobbyMain::OnMailBoxDlgResult(int, FrForm*) {
	m_mailBoxDlg = 0;
	return true;
}

// lobbymain.cpp:6911
bool CLobbyMain::OnScoreDlgResult(int, FrForm*) {
	m_scoreDlg = 0;
	return true;
}

// lobbymain.cpp:6918
bool CLobbyMain::OnResultDlg_Report(int, FrForm*) {
	if (!IsLocalContent((localContentType_t)0x14)) {
		return false;
	}
	m_resultReportDlg = 0;
	return true;
}

// lobbymain.cpp:6927
bool CLobbyMain::OnApproachGiftDlgResult(int, FrForm*) {
	m_approachGiftDlg = 0;

	WSendPacket packet(GCT_CAN_APPROACH_READY);
	packet.Encode4(MyGuid(false));
	packet.Send(SEND_TO_DEFAULT);

	return true;
}

// lobbymain.cpp:6939
bool CLobbyMain::OnResultDlgResult(int, FrForm*) {
	m_resultReportDlg = 0;
	m_approachGiftTimer = -0.1f;

	if (m_roomUserListBox) {
		// Renumber the room's players in list order.
		FrListBox::ITEM_LIST::iterator it;
		unsigned char connection;
		for (it = m_roomUserListBox->m_itemList.begin(), connection = 1; it != m_roomUserListBox->m_itemList.end(); ++it, ++connection) {
			sSlotInfo* pPlayer = (sSlotInfo*)(*it)->pData;
			if (pPlayer) {
				pPlayer->m_connection = connection;
			}
		}
	}

	CSharedDoc::Instance()->m_gameOver = false;

	return true;
}

// lobbymain.cpp:6962
bool CLobbyMain::OnGuildNoticeDlgResult(int, FrForm*) {
	m_guildNoticeDlg = 0; if (m_guildNewsReader) { delete m_guildNewsReader; m_guildNewsReader = 0; }

	OpenPCBangNoticeForm();
	CSharedDoc::Instance()->SetNeedFirstLoginAlarm(false);
	return true;
}

// lobbymain.cpp:6981
// Result of the item-use report on the game result screen: Yes uses one
// 0x1A000041 item (dropping it at zero) and leaves the room with the game
// statistics (packet 0xA2); otherwise the quit question comes back.
bool CLobbyMain::OnRoomUseReportDlgResult(int ret, FrForm* pForm) {
	if (ret == 1) {
		std::list<sItemInfo>::iterator it;
		for (it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {
			if (sItemInfo(*it).m_typeId == 0x1A000041) {
				it->m_count--;
				if (it->m_count < 1) {
					WSingleton<CSharedDoc>::Instance()->m_itemInfoList.erase(it);
				}
				break;
			}
		}

		WSendPacket packet(0xA2);
		packet.EncodeBuffer(WSingleton<CSharedDoc>::Instance()->m_gameStatistics, 0xEB);
		packet.Send(SEND_TO_DEFAULT);

		FrWnd* pOk = pForm->FindChildByName("ok");
		if (pOk)
			pOk->Enable(false);

		WSingleton<CSharedDoc>::Instance()->m_gameOver = false;
		this << MsgObject(0, 1, 0, 0, 0, 0, 0);
	} else {
		if (m_channelRoomCloseBtn)
			m_channelRoomCloseBtn->Enable(true);

		OpenGameExtResQuitForm();
	}

	m_roomUseReportDlg = 0;
	return true;
}

// lobbymain.cpp:7027
// Result of the quit question on the game result screen: Yes leaves the room
// (with the local player's pang of a type 4/5 game when tracked); otherwise
// the room's close button comes back.
bool CLobbyMain::OnRoomExtResQuitDlgResult(int ret, FrForm* pForm) {
	if (ret == 1) {
		FrWnd* pOk = pForm->FindChildByName("ok");

		if (pOk)
			pOk->Enable(false);

		WSendPacket packet(GCT_EXIT_ROOM);
		packet.Encode1(0);
		packet.Encode2(0xFFFF);

		sGameResultInfo* pMyResult = 0;
		if (IsLocalContent((localContentType_t)0x14)) {
			CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
			if (pDoc->m_roomInfo.m_gameType == 4 || pDoc->m_roomInfo.m_gameType == 5) {
				for (std::vector<sGameResultInfo>::iterator it = pDoc->m_gameResultList.begin(); it != pDoc->m_gameResultList.end(); it++) {
					if (it->m_uid == pDoc->m_userInfo.guid) {
						pMyResult = &*it;
						break;
					}
				}
			}
		}

		if (pMyResult) {
			packet.Encode8(pMyResult->m_pang);
			packet.Encode8(pMyResult->m_extraPang);
		} else {
			packet.Encode8(0);
			packet.Encode8(0);
		}

		packet.Send(SEND_TO_DEFAULT);

		WSingleton<CSharedDoc>::Instance()->m_gameOver = false;
		this << MsgObject(0, 1, 0, 0, 0, 0, 0);
	} else {
		if (m_channelRoomCloseBtn)
			m_channelRoomCloseBtn->Enable(true);
	}

	m_roomExtResQuitDlg = 0;
	return true;
}

// lobbymain.cpp:7092
// Result of the 30-second quit question on the game result screen: Yes leaves
// the room; otherwise the room's close button comes back.
bool CLobbyMain::OnRoomExt30sResQuitDlgResult(int ret, FrForm* pForm) {
	if (ret == 1) {
		FrWnd* pOk = pForm->FindChildByName("ok");

		if (pOk)
			pOk->Enable(false);

		WSendPacket packet(GCT_EXIT_ROOM);
		packet.Encode1(0);
		packet.Encode2(0xFFFF);
		packet.Encode8(0);
		packet.Encode8(0);
		packet.Send(SEND_TO_DEFAULT);

		WSingleton<CSharedDoc>::Instance()->m_gameOver = false;
		this << MsgObject(0, 1, 0, 0, 0, 0, 0);
	} else {
		if (m_channelRoomCloseBtn)
			m_channelRoomCloseBtn->Enable(true);
	}

	return true;
}

// lobbymain.cpp:7124
bool CLobbyMain::OnUserListDlgResult(int, FrForm*) {
	m_userListDlg = 0;
	return true;
}

// lobbymain.cpp:7130
bool CLobbyMain::OnNotifyQuitDlgResult(int, FrForm*) {
	g_bQuit = true;
	return true;
}

// lobbymain.cpp:7136
bool CLobbyMain::OnLoginDlgResult(int ret, FrForm*) {
	if (ret == FrOK) {
		if (m_loginDlg) {
			m_loginDlg->Login();
			m_loginDlg->SetCloseResult(FrNONE);
			return false;
		}
	} else {
		if (ret == FrCANCEL) {
			g_bQuit = true;
		}

		m_loginDlg = 0;
	}

	return true;
}

// lobbymain.cpp:7156
bool CLobbyMain::OnDtRoomDlgResult(int ret, FrForm*) {
	if (ret != 1) {
		EnableRoomEnterControls(true);
	}
	m_dtRoomDlg = 0;
	return true;
}

// lobbymain.cpp:7165
bool CLobbyMain::OnVoteDlgResult(int ret, FrForm*) {
	WSendPacket packet(GCT_VOTE_FOR_BANISH);

	if (ret == 1)
		packet.Encode1(0);
	else
		packet.Encode1(1);

	packet.Send(SEND_TO_DEFAULT);
	return true;
}

// lobbymain.cpp:7178
// The invitation dialog closed: on accept, join the inviter's room when it is
// on this server and channel, otherwise follow the friend to its server.
bool CLobbyMain::OnInviteDlgResult(int result, FrForm* pForm) {
	if (!m_matchingDlg)
		return false;

	bool inviteContent = IsLocalContent((localContentType_t)0x54);
	if (result == 1) {
		if (inviteContent) {
			if (m_matchingDlg->m_svrGUID == WSingleton<CSharedDoc>::Instance()->m_gameServerUID
				&& m_matchingDlg->m_channelIdx == WSingleton<CSharedDoc>::Instance()->m_currentChannel.id) {
				std::list<sRoomInfo>::iterator it = std::find(WSingleton<CSharedDoc>::Instance()->m_roomList.begin(), WSingleton<CSharedDoc>::Instance()->m_roomList.end(), m_matchingDlg->GetRoomIdx());
				if (it == WSingleton<CSharedDoc>::Instance()->m_roomList.end()) {
					// CP949: "The room you were invited to no longer exists"
					this << MsgObject(this, 0x23, (intptr_t)"\xC3\xCA\xB4\xEB\xB9\xDE\xC0\xBA \xB9\xE6\xC0\xCC \xC1\xF6\xB1\xDD \xC1\xB8\xC0\xE7\xC7\xCF\xC1\xF6 \xBE\xCA\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
				} else {
					FrWnd* pConsent = pForm->FindChildByName("b_consent");
					if (pConsent)
						pConsent->Enable(false);

					EnableRoomEnterControls(false);

					WSendPacket packet((enumClientPacket)9);
					packet.Encode2(m_matchingDlg->GetRoomIdx());
					packet.EncodeStr("");
					packet.Send(SEND_TO_DEFAULT);

					this << MsgObject(0, 0x16, 0, 0, 0, 0, 0);
					this << MsgObject(0, 1, 0, 0, 0, 0, 0);
				}
			} else {
				sFriend* pFriend = BuddyMGR->GetBuddy(m_matchingDlg->GetFromUID());
				if (pFriend) {
					if (g_pFresh->IsCurrentLayout("RealMyRoom"))
						AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0xDF, 0, 0, 0, 0, 0);

					WSingleton<CSharedDoc>::Instance()->m_gameServerList.clear();
					WSingleton<CSharedDoc>::Instance()->m_channelList.clear();

					WSendPacket packet((enumClientPacket)0x43);
					packet.Send(SEND_TO_DEFAULT);

					WSingleton<CSharedDoc>::Instance()->SetGoWithModeStart(pFriend->uid);
					WSingleton<CSharedDoc>::Instance()->SetGoWithMode(2);
				}
			}
		}
	} else if (inviteContent) {
		if (m_matchingDlg->m_svrGUID == WSingleton<CSharedDoc>::Instance()->m_gameServerUID
			&& m_matchingDlg->m_channelIdx == WSingleton<CSharedDoc>::Instance()->m_currentChannel.id) {
			WSingleton<CSharedDoc>::Instance()->SetInviteMode(0);
		}
	}

	m_matchingDlg = 0;
	return true;
}

// lobbymain.cpp:7290
bool CLobbyMain::OnItemEquipResult(int ret, FrForm*) {
	if (ret == 1 || ret == 2) {
		if (m_itemEquipDlg) {
			memcpy(m_equipItem, m_itemEquipDlg->GetMyItemSlot(), sizeof(m_equipItem));
		}
		m_itemEquipDlg = NULL;
		HandleMsg(MsgObject(0, 1, 0, 0, 0, 0, 0));

		WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
		packet.Encode1(2);
		packet.EncodeBuffer(m_equipItem, sizeof(m_equipItem));
		packet.Send(SEND_TO_DEFAULT);
	}
	return true;
}

// lobbymain.cpp:7310
// Result of the room password question: OK joins the room (packet 9 with the
// password, which message 0x27 also stores); otherwise the room controls
// come back.
bool CLobbyMain::OnPasswordDlgResult(int result, FrForm*) {
	char unused[32];   // never used: the original's /GS cookie (likely a macro's buffer)

	if (result == 1) {
		if (m_passwordDlg) {
			FrWnd* pWnd = m_passwordDlg->FindChildByName("password");
			FrEdit* pEdit = (FrEdit*)((__rtti_obj = (void*)pWnd) != 0 ? ((IObject*)pWnd)->DynamicCast(&FrEdit::m_RTTI) : 0);

			std::string password;
			if (pEdit) {
				const char* text = pEdit->GetLine(1, false);
				password = text;
				HandleMsg(MsgObject(0, 0x27, (intptr_t)text, 0, 0, 0, 0));
			}

			FrWnd* pOk = m_passwordDlg->FindChildByName("ok");
			if (pOk)
				pOk->Enable(false);

			EnableRoomEnterControls(false);

			WSendPacket packet((enumClientPacket)9);
			packet.Encode2(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomNumber);
			packet.EncodeStr(password);
			packet.Send(SEND_TO_DEFAULT);

			HandleMsg(MsgObject(0, 0x16, 0, 0, 0, 0, 0));
			HandleMsg(MsgObject(0, 1, 0, 0, 0, 0, 0));
		} else {
			EnableRoomEnterControls(true);
		}
	} else {
		EnableRoomEnterControls(true);
	}

	m_passwordDlg = 0;
	return true;
}

// lobbymain.cpp:7353
bool CLobbyMain::OnPCBangDlgResult(int, FrForm*) {
	m_pcbangNoticeDlg = 0;

	if (CSharedDoc::Instance()->m_noticeShown) {
		OnToppage_NoticeUp();
	}
	return true;
}

// lobbymain.cpp:7364
bool CLobbyMain::OnCreateInfoResult(int, FrForm*) {
	return true;
}

// lobbymain.cpp:7369
// Inserts the chosen emoticon at the cursor of the chat input, or of the
// report dialog's "id" edit while that dialog is open, if it still fits.
bool CLobbyMain::OnEmoticonResult(int result, FrForm* pForm) {
	if (m_emoticonDlg && result == 1) {
		FrEmoticonDlg* pDlg = DYNAMIC_CAST(FrEmoticonDlg, pForm);
		if (pDlg) {
			const char* icon = pDlg->GetSelectedIcon();
			if (icon) {
				FrEdit* pEdit = m_chatEdit;
				if (m_reportDlg)
					pEdit = DYNAMIC_CAST(FrEdit, m_reportDlg->FindChildByName("id"));

				if (pEdit) {
					const char* front = pEdit->GetEditText_Front();
					const char* comp = pEdit->GetEditText_Comp();
					const char* end = pEdit->GetEditText_End();

					if (CChatMsg::Instance()->GetMaskedFont()->GetTextWidth(g_view, MakeStr("%s%s%s%s", front, comp, icon, end)) < pEdit->GetWidthLimit())
						pEdit->SetLine(1, MakeStr("%s%s%s%s", front, comp, icon, end), 0, false, 0);

					pEdit->SetKeyFocus(true);
				}
			}
		}
	}

	m_emoticonDlg = 0;
	return true;
}

// lobbymain.cpp:7402
bool CLobbyMain::OnReportDlgResult(int, FrForm*) {
	m_reportDlg = 0;
	return true;
}

// lobbymain.cpp:7408
bool CLobbyMain::OnParanNickChangeResult(int, FrForm*) {
	m_paranNickChangeDlg = 0;

	if (WSingleton<CSharedDoc>::Instance()->m_paranNickChanged) {
		WSingleton<CSharedDoc>::Instance()->m_paranNickChanged = false;
		OpenEventPrizeForm(2);
	}

	OpenCaddieWarningForm();

	return true;
}

// lobbymain.cpp:7424
bool CLobbyMain::OnCaddieWarningDlgResult(int, FrForm*) {
	m_caddieWarningDlg = 0;
	OpenNoteForm();
	return true;
}

// lobbymain.cpp:7431
bool CLobbyMain::OnUIHackDlgResult(int, FrForm*) {
	g_bQuit = true;
	return true;
}

// lobbymain.cpp:7438
bool CLobbyMain::OnMapSelectDlgResult(int, FrForm*) {
	m_mapSelectDlg = 0;
	return true;
}

// lobbymain.cpp:7445
bool CLobbyMain::OnHolesEventDlgResult(int, FrForm*) {
	m_holesEventDlg = 0;
	return true;
}

// lobbymain.cpp:7451
bool CLobbyMain::OnPointEventDlgResult(int, FrForm*) {
	m_pointEventDlg = 0;
	return true;
}

// lobbymain.cpp:7458
bool CLobbyMain::OnWorldTourEventDlgResult(int, FrForm*) {
	m_worldTourEventDlg = 0;
	return true;
}

// lobbymain.cpp:7465
bool CLobbyMain::OnFindEbortEventDlgResult(int, FrForm*) {
	m_findEbortEventDlg = 0;
	return true;
}

// lobbymain.cpp:7472
bool CLobbyMain::OnHalloweenEventDlgResult(int, FrForm*) {
	m_halloweenEventDlg = 0;
	return true;
}

// lobbymain.cpp:7479
bool CLobbyMain::OnChristmasEventDlgResult(int, FrForm*) {
	m_christmasEventDlg = 0;
	return true;
}

// lobbymain.cpp:7487
bool CLobbyMain::OnChristmasSockEventDlgResult(int, FrForm*) {
	m_christmasSockEventDlg = 0;
	return true;
}

// lobbymain.cpp:7495
bool CLobbyMain::OnTicketExchangeDlgResult(int, FrForm*) {
	m_ticketExchangeDlg = 0;
	return true;
}

// lobbymain.cpp:7528
bool CLobbyMain::OnLevelupFormResult(int, FrForm*) {
	if (!m_levelupDlgs.empty()) {

		m_levelupDlgs[m_levelupDlgs.size() - 1] = 0;
		m_levelupDlgs.erase(m_levelupDlgs.end() - 1);
	}

	return true;
}

// lobbymain.cpp:7539
bool CLobbyMain::OnFirstTutorialDlgResult(int, FrForm*) {
	return true;
}

// lobbymain.cpp:7545
bool CLobbyMain::OnQuickStartDlgResult(int result, FrForm*) {
	switch (result) {
	case 1:
		if (m_quickResultDlg == NULL) {
			m_quickResultDlg = CreateForm<FrQuickResultDlg>(g_pFresh->GetManager(), this, "quick_result", NULL);
			if (m_quickResultDlg) {
				m_quickResultDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnQuickResultDlgResult, 0x40);
				((FrQuickResultDlg*)m_quickResultDlg)->SetContents(MakeStr("\xB8\xC5\xC4\xAA \xB4\xEB\xBB\xF3\xC0\xBB \xB0\xCB\xBB\xF6 \xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9."));
			}
		}
		break;
	}

	m_extraEventDlg = 0;

	return true;
}

// lobbymain.cpp:7577
bool CLobbyMain::OnQuickResultDlgResult(int, FrForm*) {
	m_quickResultDlg = 0;
	return true;
}

// lobbymain.cpp:7584
void CLobbyMain::OpenLayout(const char* layoutName) {
	this << MsgObject(0, 0, (intptr_t)layoutName, 0, 0, 0, 0);
}

// lobbymain.cpp:7589
void CLobbyMain::ResetControl(void) throw() {
	m_toppageGameBtn = NULL;
	m_toppageFamilyBtn = NULL;
	m_toppagePangysStudyBtn = NULL;
	m_toppageMsgBtn = NULL;
	m_barButton[13] = NULL;
	m_toppageNoticeBtn = NULL;
	m_toppageServerBtn = NULL;

	m_matchingDlg = NULL;
	m_userListDlg = NULL;
	m_dtRoomDlg = NULL;
	m_scoreDlg = NULL;
	m_passwordDlg = NULL;

	m_optionDlg = NULL;
	m_emoticonDlg = NULL;
	m_reportDlg = NULL;
	m_serverDlg = NULL;
	m_guildNoticeDlg = NULL;
	m_itemEquipDlg = NULL;

	m_unusedDlg = NULL;
	m_mailBoxDlg = NULL;
	m_pcbangNoticeDlg = NULL;
	m_resultReportDlg = NULL;
	m_newRecordDlg = NULL;
	m_paranNickChangeDlg = NULL;
	m_caddieWarningDlg = NULL;
	m_roomExtResQuitDlg = NULL;
	m_roomUseReportDlg = NULL;
	m_channelRoomCloseBtn = NULL;
	m_makeRoomBtn = NULL;

	m_guildInfoBtn = NULL;



	m_userListBox = NULL;
	m_roomListBox = NULL;
	m_selectedRoomData = NULL;
	m_languageArea = NULL;
	m_chatEdit = NULL;
	m_chatView = NULL;
	m_chatTargetCombo = NULL;
	m_emoticonBtn = NULL;
	m_roomTitleEdit = NULL;
	m_chatBg = NULL;


	m_mapBgViewer = NULL;
	m_roomOptionBtn = NULL;
	m_roomUserListBox = NULL;
	m_userRankListBox = NULL;
	m_guildRankListBox = NULL;
	m_mapBtn = NULL;
	m_mapPrevBtn = NULL;
	m_mapNextBtn = NULL;
	m_holeCombo = NULL;
	m_mapGauge = NULL;
	m_mapLockArea = NULL;
	m_backGroundViewer = NULL;
	m_startBtn = NULL;
	m_readyBtn = NULL;
	m_roomSettingWnd = NULL;
	m_scoreBtn = NULL;
	m_guildInviteBtn = NULL;
	m_changeTeamBtn = NULL;
	m_teamNumArea = NULL;
	m_teamTab1Area = NULL;
	m_teamTab2Area = NULL;
	m_guildTab1Area = NULL;
	m_guildTab2Area = NULL;

	m_cateAllBtn = NULL;
	m_cateVSBtn = NULL;
	m_cateMassBtn = NULL;
	m_cateBattleBtn = NULL;
	m_cateChatBtn = NULL;
	m_cateVSList = NULL;
	m_cateMassList = NULL;
	m_cateBattleList = NULL;

	m_roomControlWnd = NULL;

	m_userActionBtn = NULL;
	m_extReservedWnd = NULL;

	m_popupDlg = NULL;

	m_mapSelectDlg = NULL;

	m_onelineReqDlg = NULL;
	m_mapExtraWnd = NULL;
	m_extReservedWnd2 = NULL;
	m_extFrame = NULL;
	m_charControlWnd = NULL;
	m_roomImage = NULL;
	m_roomUserImage[0] = NULL;
	m_roomUserImage[1] = NULL;
	m_roomUserMarkImage[0] = NULL;
	m_roomUserMarkImage[1] = NULL;
	m_guildScoreGauge = NULL;
	m_roomUserSelImage = NULL;
	m_roomUserReserved2 = NULL;
	m_roomUserReserved3 = NULL;
	m_roomListImage = NULL;
	m_eventImage = NULL;

	m_tipBlinking = false;
	m_tipBlinkCount = 0;
	m_tipBlinkTime = 0.0f;

	memset(m_extRoomUserImage, 0, sizeof(m_extRoomUserImage));
	memset(m_roomUserReserved, 0, sizeof(m_roomUserReserved));
	memset(m_petArea, 0, sizeof(m_petArea));
	memset(m_eventDlg, 0, sizeof(m_eventDlg));
	memset(&m_powerBar, 0, sizeof(FrGaugeBar*) * 5);
	memset(&m_powerStatic, 0, sizeof(FrStatic*) * 5);

	m_guildNewsReader = NULL;
	m_roomSortResetTime = 0;
	m_userSortResetTime = 0;
	m_blinkTipArea = NULL;
	m_startTipArea = NULL;
	m_readyTipArea = NULL;
	m_countdownArea = NULL;

	m_toppageEvent1Btn = NULL;
}

// lobbymain.cpp:7722
void CLobbyMain::ShowPet(unsigned long uid) {
	CSharedDoc *doc = CSharedDoc::Instance();
	std::map<unsigned long, sRoomSlot>::iterator it = doc->m_roomSlotMap.find(uid);

	if (it == doc->m_roomSlotMap.end())
		return;

	HidePet(uid);

	if (uid == MyGuid(true)) {
		SetLevelBar();
	}

	sRoomSlot *pSlot = &it->second;

	if (!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM")) {
		std::list<sSlotInfo>::iterator player = std::find(CSharedDoc::Instance()->m_roomPlayerList.begin(), CSharedDoc::Instance()->m_roomPlayerList.end(), uid);

		if (player == CSharedDoc::Instance()->m_roomPlayerList.end())
			return;

		pSlot->m_pPetArea = m_petArea[player->m_connection - 1];
	}
	else if (!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXT")) {
		pSlot->m_pPetArea = m_petArea[0];
	}

	if (!pSlot || !pSlot->m_pPetArea)
		return;

	if (!g_pFresh->GetManager()->IsValidWindow(pSlot->m_pPetArea))
		return;

	pSlot->m_pPetArea->SetMouseEvent(uid == CSharedDoc::Instance()->m_userInfo.guid);

	if (GalleryMode()) {
		if (CSharedDoc::Instance()->m_userInfo.galleryGuid == 0xFFFFFFFF)
			return;

		if (!pSlot->m_partTidList.SetTids(&pSlot->m_characterInfo, 0xFF) || !pSlot->m_partTidList.IsComboValid()) {
			pSlot->m_partTidList.m_tidChar = pSlot->m_characterInfo.typeId;
			pSlot->m_partTidList.SetDefaultTids();
		}
	}
	else {
		if (!pSlot->m_partTidList.SetTids(&pSlot->m_characterInfo, 0xFF) || !pSlot->m_partTidList.IsComboValid()) {
			pSlot->m_partTidList.m_tidChar = pSlot->m_characterInfo.typeId;
			pSlot->m_partTidList.SetDefaultTids();
		}

		if (uid == CSharedDoc::Instance()->m_userInfo.guid)
			CSharedDoc::Instance()->BuildMyPartTidList();
	}

	WRect rect(pSlot->m_pPetArea->GetRect());

	pSlot->m_pPet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.35f, uid != CSharedDoc::Instance()->m_userInfo.guid);

	pSlot->m_pPet->SetDistanceRange(10.0f, 14.0f); pSlot->m_pPet->SetDistance(12.0f, true, false);
	pSlot->m_pPet->SetArea(rect.w, rect.h);
	pSlot->m_pPet->SetModel(pSlot->m_partTidList, pSlot->m_characterInfo.auxParts.tids, 0, 3.0f, uid);

	if (pSlot->m_angelFlag & 2)
		pSlot->m_pPet->AttachAngelWingFx();

	if (uid == CSharedDoc::Instance()->m_userInfo.guid) {
		if (!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "CREATE"))
			pSlot->m_pPet->SetControlFlag(0xF);
		else
			pSlot->m_pPet->SetControlFlag(0x1A);
	}
	else {
		pSlot->m_pPet->SetControlFlag(0x10);
	}
}

// lobbymain.cpp:7817
void CLobbyMain::HidePet(unsigned long uid) {
	CSharedDoc *doc = CSharedDoc::Instance();
	std::map<unsigned long, sRoomSlot>::iterator it = doc->m_roomSlotMap.find(uid);

	if (it != doc->m_roomSlotMap.end()) {
		if (it->second.m_pPet) {
			delete it->second.m_pPet;
			it->second.m_pPet = NULL;
		}
	}
}

// lobbymain.cpp:7825
void CLobbyMain::EnableRoomEnterControls(bool enable) {
	if (m_roomListBox)
		m_roomListBox->Enable(enable);

	if (m_makeRoomBtn && !(CSharedDoc::Instance()->m_currentChannel.flags & 0x80))
		m_makeRoomBtn->Enable(enable);

	if (m_guildInfoBtn) {
		m_guildInfoBtn->Enable(enable);
	}

	if (enable) {
		if (m_dtRoomDlg)
			static_cast<FrDtRoomDlg*>(m_dtRoomDlg)->EnableJoinBtn(true);
	}

	if ((CSharedDoc::Instance()->GetPangyaUserInfo().identity & 0xE) == 0xE) {
		if (m_guildInfoBtn)
			m_guildInfoBtn->Enable(false);
	}
}

// lobbymain.cpp:7865
void CLobbyMain::SetStartBtn(bool check) {
	if (!check)
		return;

	std::list<sSlotInfo>& players = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList;
	std::list<sSlotInfo>::iterator it;
	switch (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType) {
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
	case 14:
		if (WSingleton<WNetworkSystem>::Instance()->GetConnectionType() == 0
			&& players.size() < (unsigned int)((WSingleton<CSharedDoc>::Instance()->m_curServer.flags & 2) ? 5 : WSingleton<CSharedDoc>::Instance()->m_gameTypeNames[4].defaultPlayers)
			&& !AdminMode()) {
			m_roomOptionD = true;
			for (it = players.begin(); it != players.end(); ++it) {
				if (!(it->m_mannerFlag & 2) && !(it->m_attribute & 0x18)) {
					m_roomOptionD = false;
					break;
				}
			}
		} else {
			m_roomOptionD = true;
			for (it = players.begin(); it != players.end(); ++it) {
				if (!(it->m_mannerFlag & 2) && !(it->m_attribute & 0x18)) {
					m_roomOptionD = false;
					break;
				}
			}
		}
		break;

	default:
		if (players.size() == 1 && !AdminMode()) {
			m_roomOptionD = false;
			break;
		}

		m_roomOptionD = true;

		if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 1) {
			if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_userLimit > players.size())
				m_roomOptionD = false;

			if (m_roomOptionD) {
				int red = 0;
				int blue = 0;
				for (it = players.begin(); it != players.end(); ++it) {
					unsigned char team = it->m_attribute & 3;
					if (team == 0) {
						red++;
					} else if (team == 1) {
						blue++;
					}
				}

				if (red != blue)
					m_roomOptionD = false;
			}
		}

		if (m_roomOptionD) {
			for (it = players.begin(); it != players.end(); ++it) {
				if (!(it->m_mannerFlag & 2) && !(it->m_attribute & 0x18)) {
					m_roomOptionD = false;
					break;
				}
			}
		}
		break;
	}

	if (m_roomOptionD && players.size() > 1) {
		if (m_blinkTipArea) {
			m_tipBlinking = true;
			m_tipBlinkTime = 0.0f;
		}

		if (m_mapSelectDlg)
			m_mapSelectDlg->Close(true);
	} else {
		m_tipBlinking = false;
		if (m_blinkTipArea)
			m_blinkTipArea->SetVisible(false);
	}
}

// lobbymain.cpp:7987
void CLobbyMain::SetRoomControls(bool isMaster) {
	CSharedDoc *doc = WSingleton<CSharedDoc>::Instance();
	unsigned char playerCount = (unsigned char)doc->m_roomPlayerList.size();
	bool lockedCourse = false;

	if (doc->m_currentChannel.flags & 0x800) {
		IFF_STRUCT::sCourse *pCourse = ItemManager()->FindCourse(0x28000000 | ((doc->m_roomInfo.m_course > 0x7F && doc->m_roomInfo.m_course != 0xFD) ? doc->m_roomInfo.m_course - 0x80 : doc->m_roomInfo.m_course));

		if (pCourse && (pCourse->courseFlags & 0xF) >= 3)
			lockedCourse = true;
	}

	unsigned char identityBit2 = (unsigned char)((WSingleton<CSharedDoc>::Instance()->m_userInfo.identity >> 2) & 1);
	if (identityBit2)
		lockedCourse = false;

	if (m_roomOptionBtn) {
		if (isMaster) {
			m_roomOptionBtn->Enable((playerCount == 1 || !m_roomOptionD || identityBit2) && !(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomMode == 0xE));
		}
		else {
			m_roomOptionBtn->Enable(false);
		}
	}

	if (m_roomControlWnd) {
		if ((WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80)
			|| WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 0xA
			|| WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomMode == 0xE) {
			m_roomControlWnd->Enable(false);
		}
		else if (isMaster) {
			m_roomControlWnd->Enable(playerCount == 1 || !m_roomOptionD || AdminMode());
		}
		else {
			m_roomControlWnd->Enable(false);
		}
	}

	if (m_mapPrevBtn) {
		if (isMaster) {
			bool visible = (playerCount == 1 || !m_roomOptionD || AdminMode()) && WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomMode != 0xE;
			m_mapPrevBtn->SetVisible(visible);

			if ((WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80) || WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 0xA) {
				m_mapPrevBtn->Enable(false);
			}
			else {
				m_mapPrevBtn->Enable(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course != FirstMap());
			}
		}
		else {
			m_mapPrevBtn->SetVisible(false);
		}
	}

	if (m_mapNextBtn) {
		if (isMaster) {
			bool visible = (playerCount == 1 || !m_roomOptionD || AdminMode()) && WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomMode != 0xE;
			m_mapNextBtn->SetVisible(visible);

			if ((WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80) || WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 0xA) {
				m_mapNextBtn->Enable(false);
			}
			else {
				m_mapNextBtn->Enable(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course < LastMap(false));
			}
		}
		else {
			m_mapNextBtn->SetVisible(false);
		}
	}

	if (m_holeCombo) {
		if (lockedCourse
			|| (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x90)
			|| WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 0xA
			|| WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomMode == 0xE) {
			m_holeCombo->Enable(false);
		}
		else if (isMaster) {
			m_holeCombo->Enable(playerCount == 1 || !m_roomOptionD || AdminMode());
		}
		else {
			m_holeCombo->Enable(false);
		}
	}

	if (m_mapLockArea) {
		m_mapLockArea->SetVisible(lockedCourse);
	}

	if (m_startBtn) {
		if (isMaster) {
			m_startBtn->SetVisible(true);
			m_startBtn->Enable(true);

			if (m_readyBtn)
				m_readyBtn->SetVisible(false);

			if (m_readyBtn)
				m_readyBtn->Enable(false);

			EnableSettingButton(true);

			if (IsLocalContent((localContentType_t)0x2B) && WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_userLimit >= 100) {
				if (WSingleton<CSharedDoc>::Instance()->m_userInfo.identity & 4)
					m_startBtn->Enable(true);
				else
					m_startBtn->Enable(false);
			}
			else {
				m_startBtn->Enable(WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.size() >= 1);
			}
		}
		else if (!m_roomOptionB) {
			m_readyBtn->SetButtonImg(m_roomOptionA ? "btn_ready_cancel_n" : "btn_ready_n", FrButton::NORMAL);
			m_readyBtn->SetButtonImg(m_roomOptionA ? "btn_ready_cancel_o" : "btn_ready_o", FrButton::OVER);
			m_readyBtn->SetVisible(true);
			m_readyBtn->Enable(true);
			m_startBtn->SetVisible(false);
			m_startBtn->Enable(false);
			m_startBtn->Enable(true);
		}
	}

	if (m_channelRoomCloseBtn) {
		if (isMaster) {
			m_channelRoomCloseBtn->Enable(true);
		}
		else if (!m_roomOptionB) {
			if (GalleryMode())
				m_channelRoomCloseBtn->Enable(true);
			else
				m_channelRoomCloseBtn->Enable(!m_roomOptionA);
		}
	}

	if (m_roomSettingWnd) {
		if (GalleryMode()) {
			m_roomSettingWnd->Enable((WSingleton<CSharedDoc>::Instance()->m_userInfo.identity & 0xE) == 0xE && WSingleton<CSharedDoc>::Instance()->m_userInfo.galleryGuid != 0xFFFFFFFF);
		}
		else if (isMaster) {
			m_roomSettingWnd->Enable(true);
		}
		else {
			m_roomSettingWnd->Enable(!m_roomOptionA);
		}

		if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80)
			m_roomSettingWnd->Enable(false);

		if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 8)
			m_roomSettingWnd->Enable(false);
	}

	SetRoomCharControl(BTN_DIR_NONE);
	SetRoomCaddieControl(BTN_DIR_NONE);

	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 6) {
		if (m_roomControlWnd)
			m_roomControlWnd->Enable(false);
		if (m_mapPrevBtn)
			m_mapPrevBtn->Enable(false);
		if (m_mapNextBtn)
			m_mapNextBtn->Enable(false);
		if (m_holeCombo)
			m_holeCombo->Enable(false);

		if (strcmp(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXTRES"))
			RemakeTeam(true);
	}

	if (IsLocalContent((localContentType_t)0x54))
		CMessengerInfo::Instance()->EnableInvite(isMaster);
}

// lobbymain.cpp:8326
void CLobbyMain::SetRoomCharControl(eBtnDir dir) {
	if (GalleryMode()) {
		if (WSingleton<CSharedDoc>::Instance()->m_userInfo.galleryGuid == 0xFFFFFFFF) {
			if (m_charControlWnd)
				m_charControlWnd->SetVisible(false);
		}
		else if (m_charControlWnd) {
			m_charControlWnd->SetVisible(true);

			IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_characterInfo.typeId);

			if (pChar)
				static_cast<FrButton*>(m_charControlWnd)->SetButtonImg(pChar->m_icon, FrButton::NORMAL);
		}
	}

	if (m_roomOptionA || GalleryMode())
		return;

	std::map<unsigned int, sCharacterInfo>& characterMap = WSingleton<CSharedDoc>::Instance()->m_characterMap;
	std::map<unsigned int, sCharacterInfo>::const_iterator it = characterMap.find(WSingleton<CSharedDoc>::Instance()->m_myCharacterId);

	if (it == characterMap.end())
		return;

	switch (dir) {
	case BTN_DIR_PREV:
		if (it == characterMap.begin())
			return;

		--it;
		if (it == characterMap.end())
			return;

		{
			WSendPacket packet((enumClientPacket)0x0C);

			packet.Encode1(4);
			packet.Encode4(it->second.id);
			packet.Send(SEND_TO_DEFAULT);
		}
		break;

	case BTN_DIR_NEXT:
		++it;
		if (it == characterMap.end())
			return;

		{
			WSendPacket packet((enumClientPacket)0x0C);

			packet.Encode1(4);
			packet.Encode4(it->second.id);
			packet.Send(SEND_TO_DEFAULT);
		}
		break;
	}
}

// lobbymain.cpp:8400
// Steps the local player's equipped caddie through the owned caddies (or
// only refreshes the caddie preview), tells the room and sends the change.
void CLobbyMain::SetRoomCaddieControl(eBtnDir dir) {
	if (GalleryMode()) {
		if (WSingleton<CSharedDoc>::Instance()->m_userInfo.galleryGuid == 0xFFFFFFFF)
			this << MsgObject(0, 0x13, 0, 0, 0, 0, 0);
		else
			this << MsgObject(0, 0x13, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_caddieInfo.typeId, 0, 0, 0, 0);
	}

	CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
	std::map<unsigned int, sCaddieInfo>& caddieMap = pDoc->m_caddieMap;

	if (caddieMap.size() == 0) {
		pDoc->m_myCaddieId = 0;
		this << MsgObject(0, 0x13, 0, 0, 0, 0, 0);
		return;
	}

	unsigned int id = pDoc->m_myCaddieId;
	std::map<unsigned int, sCaddieInfo>::const_iterator it = caddieMap.find(id);

	switch (dir) {
	case BTN_DIR_PREV: {
		if (it != caddieMap.begin()) {
			--it;
			if (it != caddieMap.end()) {
				pDoc->m_myCaddieId = it->second.id;

				this << MsgObject(0, 0x13, it->second.typeId, 0, 0, 0, 0);

				SetLevelBar();

				WSendPacket packet((enumClientPacket)0x0C);
				packet.Encode1(1);
				packet.Encode4(it->second.id);
				packet.Send(SEND_TO_DEFAULT);
				break;
			}
		}

		pDoc->m_myCaddieId = 0;
		this << MsgObject(0, 0x13, 0, 0, 0, 0, 0);

		SetLevelBar();

		WSendPacket packet((enumClientPacket)0x0C);
		packet.Encode1(1);
		packet.Encode4(0);
		packet.Send(SEND_TO_DEFAULT);
		break;
	}

	case BTN_DIR_NEXT:
		if (pDoc->m_myCaddieId == 0)
			it = caddieMap.begin();
		else
			++it;

		if (it == caddieMap.end())
			return;

		pDoc->m_myCaddieId = it->second.id;
		this << MsgObject(0, 0x13, it->second.typeId, 0, 0, 0, 0);

		SetLevelBar();

		{
			WSendPacket packet((enumClientPacket)0x0C);
			packet.Encode1(1);
			packet.Encode4(it->second.id);
			packet.Send(SEND_TO_DEFAULT);
		}
		break;

	case BTN_DIR_NONE:
		if (it == caddieMap.end()) {
			pDoc->m_myCaddieId = 0;
			this << MsgObject(0, 0x13, 0, 0, 0, 0, 0);
			return;
		}

		pDoc->m_myCaddieId = it->second.id;
		this << MsgObject(0, 0x13, it->second.typeId, 0, 0, 0, 0);
		break;
	}
}

// lobbymain.cpp:8513
// Fills the room's user list box (or the two team lists) from the room's
// players; after a finished game, the finishers sorted by finish order, then
// sets up the local player's room slot.
void CLobbyMain::MakeRoomUserList(void) {
	if (!m_roomUserListBox)
		return;

	switch (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType) {
	case 4:
	case 9:
	case 10:
		m_roomUserListBox->ClearItem();

		if (WSingleton<CSharedDoc>::Instance()->m_gameOver) {
			int unfinished = 0;
			if (WSingleton<CSharedDoc>::Instance()->m_gameResultList.size() > 0) {
				std::list<sSlotInfo>::iterator it;
				for (it = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.end(); ++it) {
					sSlotInfo* player = &*it;
					unsigned char index = WSingleton<CSharedDoc>::Instance()->GetIndex(player->m_uid);

					if (WSingleton<CSharedDoc>::Instance()->m_gameResultList[index].m_state == 2) {
						m_roomUserListBox->AddItem(player);
					} else if (WSingleton<CSharedDoc>::Instance()->m_gameResultList[index].m_state == 0) {
						unfinished++;
					}
				}
			}

			for (int size = m_roomUserListBox->GetCurrentItemSize(true); size < 24; size++)
				m_roomUserListBox->AddItem(NULL);

			m_roomUserListBox->SortItem(FinishUserCompare);

			if (unfinished == 0)
				PostMsg(this, "Lobby", 0x2D, 0, 0, 0, 0);
		} else {
			std::list<sSlotInfo>::iterator it;
			for (it = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.end(); ++it)
				m_roomUserListBox->AddItem(&*it);

			for (int size = m_roomUserListBox->GetCurrentItemSize(true); size < 24; size++)
				m_roomUserListBox->AddItem(NULL);

			if (WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.size() > 1)
				m_roomUserListBox->SortItem(ConnectionUserCompare);
		}
		break;

	case 6:
		if (WSingleton<CSharedDoc>::Instance()->m_gameOver) {
			m_slotList1.clear();
			m_slotList2.clear();

			std::list<sSlotInfo>::iterator it;
			for (it = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.end(); ++it) {
				sSlotInfo* player = &*it;
				unsigned char index = WSingleton<CSharedDoc>::Instance()->GetIndex(player->m_uid);

				if (WSingleton<CSharedDoc>::Instance()->m_gameResultList[index].m_state == 2) {
					if (player->m_guildUID == WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_guild.m_guildUID[0])
						m_slotList1.push_back(&*it);
					else if (player->m_guildUID == WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_guild.m_guildUID[1])
						m_slotList2.push_back(&*it);
				}
			}

			m_slotList1.sort(FinishUserCompare);
			m_slotList2.sort(FinishUserCompare);

			RemakeTeam(false);
		} else {
			RemakeTeam(true);
		}
		break;

	case 5:
		if (WSingleton<CSharedDoc>::Instance()->m_gameOver) {
			m_slotList1.clear();
			m_slotList2.clear();

			int unfinished = 0;
			std::list<sSlotInfo>::iterator it;
			for (it = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.end(); ++it) {
				sSlotInfo* player = &*it;
				unsigned char index = WSingleton<CSharedDoc>::Instance()->GetIndex(player->m_uid);

				if (WSingleton<CSharedDoc>::Instance()->m_gameResultList[index].m_state == 2) {
					if (!(player->m_attribute & 3))
						m_slotList1.push_back(&*it);
					else
						m_slotList2.push_back(&*it);
				} else if (WSingleton<CSharedDoc>::Instance()->m_gameResultList[index].m_state == 0) {
					unfinished++;
				}
			}

			m_slotList1.sort(FinishUserCompare);
			m_slotList2.sort(FinishUserCompare);

			RemakeTeam(false);

			if (unfinished == 0)
				PostMsg(this, "Lobby", 0x2D, 0, 0, 0, 0);
		} else {
			RemakeTeam(true);
		}
		break;

	default: {
		m_roomUserListBox->ClearItem();

		std::list<sSlotInfo>::iterator it;
		for (it = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.end(); ++it)
			m_roomUserListBox->AddItem(&*it);

		for (unsigned char size = (unsigned char)m_roomUserListBox->GetCurrentItemSize(true); size < 4; size++)
			m_roomUserListBox->AddItem(NULL);

		if (WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.size() > 1)
			m_roomUserListBox->SortItem(ConnectionUserCompare);
		break;
	}
	}

	switch (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType) {
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
	case 14:
		if (!m_chatWndOpen) {
			sRoomSlot slot;
			slot.m_pPetArea = m_petArea[0];
			slot.m_pPet = NULL;
			slot.m_partTidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();
			slot.m_characterInfo = WSingleton<CSharedDoc>::Instance()->m_characterMap[WSingleton<CSharedDoc>::Instance()->m_myCharacterId];
			slot.m_angelFlag = slot.m_angelFlag ^ ((slot.m_angelFlag ^ (WSingleton<CSharedDoc>::Instance()->m_userInfo.eventFlags >> 3)) & 1);
			slot.m_angelFlag = slot.m_angelFlag ^ ((slot.m_angelFlag ^ (WSingleton<CSharedDoc>::Instance()->m_userInfo.eventFlags >> 3)) & 2);
			WSingleton<CSharedDoc>::Instance()->m_roomSlotMap[MyGuid(false)] = slot;

			ShowPet(MyGuid(false));
		}
		break;
	}
}

// lobbymain.cpp:8724
// Lays the room's players out in two team columns (at least 12 rows); with
// rebuild, first splits the room's players into the two teams (by guild in a
// guild match, by the team bits otherwise).
void CLobbyMain::RemakeTeam(bool rebuild) {
	if (!m_roomUserListBox)
		return;

	if (rebuild) {
		m_slotList1.clear();
		m_slotList2.clear();

		std::list<sSlotInfo>::iterator it;
		for (it = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.end(); ++it) {
			if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 6) {
				if (it->m_guildUID == WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_guild.m_guildUID[0])
					m_slotList1.push_back(&*it);
				else if (it->m_guildUID == WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_guild.m_guildUID[1])
					m_slotList2.push_back(&*it);
			} else {
				if (!(it->m_attribute & 3))
					m_slotList1.push_back(&*it);
				else
					m_slotList2.push_back(&*it);
			}
		}

		if (m_slotList1.size() == 0)
			WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_guild.Reset(0);

		if (m_slotList2.size() == 0)
			WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_guild.Reset(1);
	}

	m_roomUserListBox->ClearItem();

	int rows = m_slotList1.size() < m_slotList2.size() ? m_slotList2.size() : m_slotList1.size();
	if (rows < 12)
		rows = 12;

	int count1 = 0;
	int count2 = 0;
	std::list<sSlotInfo*>::iterator it1 = m_slotList1.begin();
	std::list<sSlotInfo*>::iterator it2 = m_slotList2.begin();
	for (int i = 0; i < rows; i++) {
		if (it1 == m_slotList1.end()) {
			m_roomUserListBox->AddItem(NULL);
		} else {
			m_roomUserListBox->AddItem(*it1);
			++it1;
			count1++;
		}

		if (it2 == m_slotList2.end()) {
			m_roomUserListBox->AddItem(NULL);
		} else {
			m_roomUserListBox->AddItem(*it2);
			++it2;
			count2++;
		}
	}

	std::list<sSlotInfo>::iterator me = std::find(WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.begin(), WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.end(), MyGuid(false));
	if (me == WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.end())
		return;

	if (!m_guildInviteBtn)
		return;

	if (rebuild == true && strcmp(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXTRES") != 0) {
		if ((me->m_attribute & 0x18) && ((count2 < 1 && count1 > 2) || (count2 > 2 && count1 < 1))) {
			m_guildInviteBtn->Enable(true);
			return;
		}

		m_guildInviteBtn->Enable(false);
	}
}

// lobbymain.cpp:8822
// Ranks the finished game's results (m_rank = 1 + the players ahead), sorts
// the rank list box and, in a team game, shows the team totals.
void CLobbyMain::SortRank(void) {
	std::vector<sRivalData>::iterator it;
	std::vector<sRivalData>::iterator other;

	if (CSharedDoc::Instance()->m_roomInfo.m_gameType == 10) {
		for (it = CSharedDoc::Instance()->m_gameResultList.begin(); it != CSharedDoc::Instance()->m_gameResultList.end(); ++it) {
			if (it->m_state == 3) {
				it->m_rank = (unsigned char)(CSharedDoc::Instance()->m_gameResultList.size() - it->m_sortGroup);
				continue;
			}
			it->m_rank = 1;
			for (other = CSharedDoc::Instance()->m_gameResultList.begin(); other != CSharedDoc::Instance()->m_gameResultList.end(); ++other) {
				if (it == other || other->m_state == 3)
					continue;

				if (it->m_approachDistance > other->m_approachDistance
					|| (it->m_approachDistance == other->m_approachDistance && it->m_approachPoint < other->m_approachPoint))
					it->m_rank++;
			}
		}

		if (m_userRankListBox)
			m_userRankListBox->SortItem(ApproachRankUserCompare);
	} else {
		for (it = CSharedDoc::Instance()->m_gameResultList.begin(); it != CSharedDoc::Instance()->m_gameResultList.end(); ++it) {
			if (it->m_state == 3)
				continue;

			it->m_rank = 1;
			for (other = CSharedDoc::Instance()->m_gameResultList.begin(); other != CSharedDoc::Instance()->m_gameResultList.end(); ++other) {
				if (it == other || other->m_state == 3)
					continue;

				if (it->m_score > other->m_score
					|| (it->m_score == other->m_score && it->m_pang < other->m_pang))
					it->m_rank++;
			}
		}

		if (m_userRankListBox)
			m_userRankListBox->SortItem(RankUserCompare);
	}

	if (m_extFrame) {
		if (CSharedDoc::Instance()->m_roomInfo.m_gameType == 5) {
			// CP949: "RED %d<points>/%I64d<pang>  vs  BLUE %d<points>/%I64d<pang>"
			m_extFrame->SetDesc(MakeStr("RED %d\xC1\xA1/%I64d\xC6\xCE  vs  BLUE %d\xC1\xA1/%I64d\xC6\xCE",
				CSharedDoc::Instance()->m_teamResult[0].m_score, CSharedDoc::Instance()->m_teamResult[0].m_pang,
				CSharedDoc::Instance()->m_teamResult[1].m_score, CSharedDoc::Instance()->m_teamResult[1].m_pang));
		}
	}
}

// lobbymain.cpp:8896
// Fills the five stat gauges (and their captions) from the character being
// created, the gallery player, or the local player's equipped character,
// club set and caddie.
void CLobbyMain::SetLevelBar(void) {
	unsigned long charTypeId;
	const char* levels;
	const unsigned long* parts;
	const unsigned long* cards;
	unsigned long clubTypeId;
	const short* clubLevels = NULL;
	unsigned long caddieTypeId;
	unsigned char level = 0;

	if (g_pFresh->IsCurrentLayout("CREATE")) {
		charTypeId = m_createCharType;
		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(charTypeId |= 0x4000000);
		levels = (const char*)pChar->m_baseLevels;
		parts = NULL;
		cards = NULL;
		clubTypeId = 0;
		caddieTypeId = 0;
	} else if (g_pFresh->IsCurrentLayout("GAMEROOM") || g_pFresh->IsCurrentLayout("GAMEROOM_EXT")) {
		CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();

		if (GalleryMode()) {
			// sCharacterInfo::unknown6A holds the character's five stat levels.
			charTypeId = pDoc->m_roomPlayers[0].m_characterInfo.typeId;
			levels = (const char*)&pDoc->m_roomPlayers[0].m_characterInfo.unknown6A;
			parts = pDoc->m_roomPlayers[0].m_characterInfo.partTypeIds;
			cards = pDoc->m_roomPlayers[0].m_characterInfo.auxParts.tids;
			clubTypeId = pDoc->m_roomPlayers[0].m_clubSet.typeId;
			clubLevels = pDoc->m_roomPlayers[0].m_clubSet.statLevel;
			caddieTypeId = pDoc->m_roomPlayers[0].m_caddieInfo.typeId;
			level = pDoc->m_roomPlayers[0].m_level;
		} else {
			unsigned long caddieId = pDoc->m_myCaddieId;

			std::map<unsigned int, sCharacterInfo>::const_iterator charIt = pDoc->m_characterMap.find(pDoc->m_myCharacterId);

			charTypeId = charIt->second.typeId;
			levels = (const char*)&charIt->second.unknown6A;
			parts = charIt->second.partTypeIds;
			cards = charIt->second.auxParts.tids;

			std::map<unsigned int, sItemInfo>::iterator clubIt = pDoc->m_myItemMap.find(pDoc->m_myClubId);

			clubTypeId = clubIt->second.m_typeId;
			clubLevels = clubIt->second.m_clubStatLevel;

			if (caddieId == 0) {
				caddieTypeId = caddieId;
			} else {
				std::map<unsigned int, sCaddieInfo>::const_iterator caddieIt = pDoc->m_caddieMap.find(caddieId);

				caddieTypeId = caddieIt->second.typeId;
			}

			level = pDoc->m_myLevel;
		}
	} else {
		return;
	}

	if (IsLocalContent((localContentType_t)0x51)) {
		WSingleton<CCardManager>::Instance()->SetPlayerIndex(0xFF);
		WSingleton<CCardManager>::Instance()->CalcCardPeriodAndStatus();
	}

	for (unsigned char i = 0; i < 5; i++) {
		status_t status = ItemManager()->GetLevelWithPenalty((eLvlType)i, charTypeId, levels, parts, cards, clubTypeId, clubLevels, caddieTypeId, level);
		int statLevel = status.m_level;
		int penalty = status.m_penalty;

		unsigned char capacity = ItemManager()->GetCapacity((eLvlType)i, charTypeId, levels, parts, clubTypeId, clubLevels, caddieTypeId, level);

		m_statBar[i]->SetDestPos(statLevel, false);
		m_statBar[i]->SetExpand(capacity - statLevel);
		if (m_statStatic[i])
			m_statStatic[i]->SetCaption11(MakeStatusString3(statLevel, capacity, penalty));
	}
}

// lobbymain.cpp:9001
// The "information" form of an item: its portrait, name, level and
// description (an unknown-item text when the item is not found).
void CLobbyMain::OpenInformation(unsigned long typeId) {
	if (g_pFresh->GetManager()->GetDesktop()->FindChildByName("information")) {
		return;
	}

	__rtti_obj = 0;
	FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "information", 0);

	IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(typeId);

	FrArea* pPortrait = DYNAMIC_CAST(FrArea, pForm->FindChildByName("portrait"));
	if (pPortrait) {
		if (pItem) {
			pPortrait->SetBgImg(pItem->m_icon);
		} else {
			pPortrait->SetBgImg("hide");
		}
	}

	if (pItem) {
		FrEdit* pName = DYNAMIC_CAST(FrEdit, pForm->FindChildByName("name"));
		FrArea* pLevel = DYNAMIC_CAST(FrArea, pForm->FindChildByName("level"));
		if (pName && pLevel) {
			if (pItem->m_level & 0x80) {
				pLevel->SetBgImg(MakeStr("level_%03d", (pItem->m_level & 0x7F) + 1));
				pName->AddText(MakeStr("\xC0\xCC\xB8\xA7: %s\n\n\xB7\xB9\xBA\xA7:              \xC0\xCC\xC7\xCF", pItem->m_name), false, true); // CP949: "name: %s, level: ... and below"
			} else {
				pLevel->SetBgImg(MakeStr("level_%03d", (pItem->m_level & 0x7F) + 1));
				pName->AddText(MakeStr("\xC0\xCC\xB8\xA7: %s\n\n\xB7\xB9\xBA\xA7:              \xC0\xCC\xBB\xF3", pItem->m_name), false, true); // CP949: "name: %s, level: ... and above"
			}
		}

		IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(pItem->m_typeId);
		if (pDesc) {
			pForm->SetMessage(pDesc->m_text, false);
		}
	} else {
		FrEdit* pName = DYNAMIC_CAST(FrEdit, pForm->FindChildByName("name"));
		if (pName) {
			pName->AddText("\xC0\xCC\xB8\xA7: \xBA\xD2\xB8\xED\n\n\xB7\xB9\xBA\xA7: \xBA\xD2\xB8\xED", false, true); // CP949: "name: unknown, level: unknown"
		}

		pForm->SetMessage("\xBA\xA3\xC0\xCF\xBF\xA1 \xBD\xCE\xC0\xCE \xC1\xA4\xC3\xBC\xBA\xD2\xB8\xED\xC0\xC7 \xC1\xB8\xC0\xE7", false); // CP949: "an unidentified being wrapped in a veil"
	}

	pForm->Open(0, 3);
}

// lobbymain.cpp:9059
void CLobbyMain::OpenCreateInfo(unsigned long id, const char* text) {
	std::string message;
	if (text) {
		message = text;
	}

	FrForm* pForm = FrStaticCast<FrForm>(g_pFresh->GetDesktop()->FindChildByName("information_large"));
	if (pForm == NULL) {
		pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "information_large", 0);
	} else {
		return;
	}

	IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(id);

	FrArea* pPortrait = DYNAMIC_CAST(FrArea, pForm->FindChildByName("portrait"));
	if (pItem && pPortrait) {
		pPortrait->SetBgImg(pItem->m_icon);
	}

	FrEdit* pName = DYNAMIC_CAST(FrEdit, pForm->FindChildByName("name"));
	FrArea* pLevel = DYNAMIC_CAST(FrArea, pForm->FindChildByName("level"));
	if (pItem) {
		if (pName && pLevel) {
			if (pItem->m_level & 0x80) {
				pLevel->SetBgImg(MakeStr("level_%03d", (pItem->m_level & 0x7F) + 1));
				pName->AddText(MakeStr("\xC0\xCC\xB8\xA7: %s\n\n\xB7\xB9\xBA\xA7:              \xC0\xCC\xC7\xCF", pItem->m_name), false, true); // CP949: "name: %s, level: ... and below"
			} else {
				pLevel->SetBgImg(MakeStr("level_%03d", (pItem->m_level & 0x7F) + 1));
				pName->AddText(MakeStr("\xC0\xCC\xB8\xA7: %s\n\n\xB7\xB9\xBA\xA7:              \xC0\xCC\xBB\xF3", pItem->m_name), false, true); // CP949: "name: %s, level: ... and above"
			}
		}

		if (text == NULL) {
			IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(pItem->m_typeId);
			if (pDesc) {
				if (message[0] != 0) {
					message += "\n\n";
				}

				message += "\\c0xffff0000\\c";
				message += pDesc->m_text;
			}
		}
	}

	pForm->SetMessage(message.c_str(), false);
	pForm->Open(text ? (FRESH_RESULT_CALLBACK)&CLobbyMain::OnCreateInfoResult : 0, 3);
}

// lobbymain.cpp:9114
void CLobbyMain::OpenCaddieInfo(const sCaddieInfo& info) {
	FrCaddieInfoDlg* pDlg = FrStaticCast<FrCaddieInfoDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("caddie_info"));
	if (pDlg) {
		return;
	}

	pDlg = CreateForm<FrCaddieInfoDlg>(g_pFresh->GetManager(), this, "caddie_info", NULL);
	if (!pDlg) {
		return;
	}

	pDlg->SetCaddieInfo(info);
	pDlg->Open(NULL, 3);
}

// lobbymain.cpp:9137
// The medal a player earns from the finished game: 0 gold .. 2 bronze, 3 none.
// Depends on the hole count, the number of players counted and the player's
// rank among them.
int CLobbyMain::GetMedalIndex(unsigned long uid) {
	if (WSingleton<CSharedDoc>::Instance()->m_gameResultList.size() < 10)
		return 3;

	unsigned char index = WSingleton<CSharedDoc>::Instance()->GetIndex(uid);
	if (index == 0xFF || WSingleton<CSharedDoc>::Instance()->m_gameResultList[index].m_characterType > 3)
		return 3;

	unsigned char myRank = WSingleton<CSharedDoc>::Instance()->m_gameResultList[index].m_rank;
	unsigned char memberCount = 0;
	unsigned char skipCount = 0;

	unsigned char expMemberCount = 0;
	if (IsLocalContent((localContentType_t)0x2E))
		expMemberCount = WSingleton<CIntrusion>::Instance()->GetCalcExpMemberSize();

	if (expMemberCount == 0) {
		for (std::vector<sGameResultInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
			unsigned char state = it->m_state;
			if (state == 3 && GetHoleIndex(it->m_hole) < 4 && it->m_state == 3)
				continue;
			memberCount++;

			if (state != 3 && it->m_rank < myRank && it->m_characterType > 3)
				skipCount++;
		}
	} else {
		memberCount = expMemberCount;
		for (std::vector<sGameResultInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
			if (it->m_state != 3 && it->m_rank < myRank && it->m_characterType > 3)
				skipCount++;
		}
	}

	myRank -= skipCount;
	if (WSingleton<CSharedDoc>::Instance()->m_holes == 9) {
		if (memberCount <= 14) {
			return 3;
		} else if (memberCount <= 18) {
			if (myRank == 1)
				return 2;
		} else if (memberCount <= 22) {
			if (myRank == 1)
				return 1;
		} else if (memberCount <= 26) {
			if (myRank == 1)
				return 1;
			else if (myRank == 2)
				return 2;
		} else {
			if (myRank == 1)
				return 0;
			else if (myRank == 2)
				return 1;
		}
	} else if (WSingleton<CSharedDoc>::Instance()->m_holes == 18) {
		if (memberCount <= 14) {
			if (myRank == 1)
				return 2;
		} else if (memberCount <= 18) {
			if (myRank == 1)
				return 1;
			else if (myRank == 2)
				return 2;
		} else if (memberCount <= 22) {
			if (myRank == 1)
				return 0;
			else if (myRank == 2)
				return 1;
			else if (myRank == 3)
				return 2;
		} else if (memberCount <= 26) {
			if (myRank == 1)
				return 0;
			else if (myRank == 2)
				return 1;
			else if (myRank <= 4)
				return 2;
		} else {
			if (myRank == 1)
				return 0;
			else if (myRank <= 3)
				return 1;
			else if (myRank <= 6)
				return 2;
		}
	}

	return 3;
}

// lobbymain.cpp:9273
bool CLobbyMain::LatestVersion(void) {
	if (CSharedDoc::Instance()->m_latestVersion == "645.00") {
		this << MsgObject(this, 0x23, (intptr_t)"\xC3\xD6\xBD\xC5 \xB9\xF6\xC1\xAF\xC0\xB8\xB7\xCE \xBE\xF7\xB5\xA5\xC0\xCC\xC6\xAE\xB0\xA1 \xC7\xCA\xBF\xE4\xC7\xD5\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
		return false;
	}

	return true;
}

// lobbymain.cpp:9308
bool RoomStateCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sRoomInfo* pLhs = (const sRoomInfo*)lhs;
	const sRoomInfo* pRhs = (const sRoomInfo*)rhs;

	// Lounge-sized rooms first.
	if (pLhs->m_userLimit != 100 && pRhs->m_userLimit == 100) {
		return false;
	}
	if (pLhs->m_userLimit == 100 && pRhs->m_userLimit != 100) {
		return true;
	}

	if (pLhs->m_isPublic == 0 && pRhs->m_isPublic == 1) {
		return false;
	}
	if (pLhs->m_isPublic == 1 && pRhs->m_isPublic == 0) {
		return true;
	}
	if (pLhs->m_intrusionAccess == 0 && pRhs->m_intrusionAccess == 1) {
		return false;
	}
	if (pLhs->m_intrusionAccess == 1 && pRhs->m_intrusionAccess == 0) {
		return true;
	}
	if (pLhs->m_intrusionAccess == 1 && pRhs->m_intrusionAccess == 1) {
		if (pLhs->m_userCount == pLhs->m_userLimit && pRhs->m_userCount < pRhs->m_userLimit) {
			return false;
		}
		if (pLhs->m_userCount < pLhs->m_userLimit && pRhs->m_userCount == pRhs->m_userLimit) {
			return true;
		}
		if (pLhs->m_option[0] == 1 && pRhs->m_option[0] == 0) {
			return false;
		}
		if (pLhs->m_option[0] == 0 && pRhs->m_option[0] == 1) {
			return true;
		}
		if (pLhs->m_state == 0 && pRhs->m_state == 1) {
			return false;
		}
		if (pLhs->m_state == 1 && pRhs->m_state == 0) {
			return true;
		}
	}

	return false;
}

// lobbymain.cpp:9374
bool RoomStateRevCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sRoomInfo* pLhs = (const sRoomInfo*)lhs;
	const sRoomInfo* pRhs = (const sRoomInfo*)rhs;

	// Lounge-sized rooms first.
	if (pLhs->m_userLimit != 100 && pRhs->m_userLimit == 100) {
		return false;
	}
	if (pLhs->m_userLimit == 100 && pRhs->m_userLimit != 100) {
		return true;
	}

	if (pRhs->m_isPublic == 0 && pLhs->m_isPublic == 1) {
		return false;
	}
	if (pRhs->m_isPublic == 1 && pLhs->m_isPublic == 0) {
		return true;
	}
	if (pRhs->m_intrusionAccess == 0 && pLhs->m_intrusionAccess == 1) {
		return false;
	}
	if (pRhs->m_intrusionAccess == 1 && pLhs->m_intrusionAccess == 0) {
		return true;
	}
	if (pRhs->m_intrusionAccess == 1 && pLhs->m_intrusionAccess == 1) {
		if (pRhs->m_userCount == pRhs->m_userLimit && pLhs->m_userCount < pLhs->m_userLimit) {
			return false;
		}
		if (pRhs->m_userCount < pRhs->m_userLimit && pLhs->m_userCount == pLhs->m_userLimit) {
			return true;
		}
		if (pRhs->m_option[0] == 1 && pLhs->m_option[0] == 0) {
			return false;
		}
		if (pRhs->m_option[0] == 0 && pLhs->m_option[0] == 1) {
			return true;
		}
		if (pRhs->m_state == 0 && pLhs->m_state == 1) {
			return false;
		}
		if (pRhs->m_state == 1 && pLhs->m_state == 0) {
			return true;
		}
	}

	return false;
}

// lobbymain.cpp:9440
bool RoomNoCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sRoomInfo* pLhs = (const sRoomInfo*)lhs;
	const sRoomInfo* pRhs = (const sRoomInfo*)rhs;

	// Lounge-sized rooms first.
	if (pLhs->m_userLimit != 100 && pRhs->m_userLimit == 100) {
		return false;
	}
	if (pLhs->m_userLimit == 100 && pRhs->m_userLimit != 100) {
		return true;
	}

	return pLhs->m_roomNumber < pRhs->m_roomNumber;
}

// lobbymain.cpp:9472
bool RoomNoRevCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sRoomInfo* pLhs = (const sRoomInfo*)lhs;
	const sRoomInfo* pRhs = (const sRoomInfo*)rhs;

	// Lounge-sized rooms first.
	if (pLhs->m_userLimit != 100 && pRhs->m_userLimit == 100) {
		return false;
	}
	if (pLhs->m_userLimit == 100 && pRhs->m_userLimit != 100) {
		return true;
	}

	return pLhs->m_roomNumber > pRhs->m_roomNumber;
}

// lobbymain.cpp:9504
bool RoomHoleCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sRoomInfo* pLhs = (const sRoomInfo*)lhs;
	const sRoomInfo* pRhs = (const sRoomInfo*)rhs;

	// Lounge-sized rooms first.
	if (pLhs->m_userLimit != 100 && pRhs->m_userLimit == 100) {
		return false;
	}
	if (pLhs->m_userLimit == 100 && pRhs->m_userLimit != 100) {
		return true;
	}

	return pLhs->m_holeCount < pRhs->m_holeCount;
}

// lobbymain.cpp:9537
bool RoomHoleRevCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sRoomInfo* pLhs = (const sRoomInfo*)lhs;
	const sRoomInfo* pRhs = (const sRoomInfo*)rhs;

	// Lounge-sized rooms first.
	if (pLhs->m_userLimit != 100 && pRhs->m_userLimit == 100) {
		return false;
	}
	if (pLhs->m_userLimit == 100 && pRhs->m_userLimit != 100) {
		return true;
	}

	return pLhs->m_holeCount > pRhs->m_holeCount;
}

// lobbymain.cpp:9570
bool RoomCourseCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sRoomInfo* pLhs = (const sRoomInfo*)lhs;
	const sRoomInfo* pRhs = (const sRoomInfo*)rhs;

	// Lounge-sized rooms first.
	if (pLhs->m_userLimit != 100 && pRhs->m_userLimit == 100) {
		return false;
	}
	if (pLhs->m_userLimit == 100 && pRhs->m_userLimit != 100) {
		return true;
	}

	return pLhs->m_course < pRhs->m_course;
}

// lobbymain.cpp:9602
bool RoomCourseRevCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sRoomInfo* pLhs = (const sRoomInfo*)lhs;
	const sRoomInfo* pRhs = (const sRoomInfo*)rhs;

	// Lounge-sized rooms first.
	if (pLhs->m_userLimit != 100 && pRhs->m_userLimit == 100) {
		return false;
	}
	if (pLhs->m_userLimit == 100 && pRhs->m_userLimit != 100) {
		return true;
	}

	return pLhs->m_course > pRhs->m_course;
}

// lobbymain.cpp:9634
bool RoomTitleCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sRoomInfo* pLhs = (const sRoomInfo*)lhs;
	const sRoomInfo* pRhs = (const sRoomInfo*)rhs;

	return lstrcmpiA(pLhs->m_title, pRhs->m_title) < 0;
}

// lobbymain.cpp:9651
bool RoomTitleRevCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sRoomInfo* pLhs = (const sRoomInfo*)lhs;
	const sRoomInfo* pRhs = (const sRoomInfo*)rhs;

	return lstrcmpiA(pLhs->m_title, pRhs->m_title) > 0;
}

// lobbymain.cpp:9668
bool RoomGameTypeCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sRoomInfo* pLhs = (const sRoomInfo*)lhs;
	const sRoomInfo* pRhs = (const sRoomInfo*)rhs;

	if (pLhs->m_gameType == pRhs->m_gameType) {
		return pLhs->m_trophy > pRhs->m_trophy;
	}

	return pLhs->m_gameType < pRhs->m_gameType;
}

// lobbymain.cpp:9688
bool RoomGameTypeRevCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sRoomInfo* pLhs = (const sRoomInfo*)lhs;
	const sRoomInfo* pRhs = (const sRoomInfo*)rhs;

	if (pLhs->m_gameType == pRhs->m_gameType) {
		return pLhs->m_trophy > pRhs->m_trophy;
	}

	return pLhs->m_gameType > pRhs->m_gameType;
}

// lobbymain.cpp:9708
bool RoomUserCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sRoomInfo* pLhs = (const sRoomInfo*)lhs;
	const sRoomInfo* pRhs = (const sRoomInfo*)rhs;

	if (pLhs->m_userLimit > pRhs->m_userLimit) {
		return false;
	}
	if (pLhs->m_userLimit < pRhs->m_userLimit) {
		return true;
	}

	return pLhs->m_userCount <= pRhs->m_userCount ? true : false;
}

// lobbymain.cpp:9734
bool RoomUserRevCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sRoomInfo* pLhs = (const sRoomInfo*)lhs;
	const sRoomInfo* pRhs = (const sRoomInfo*)rhs;

	if (pLhs->m_userLimit > pRhs->m_userLimit) {
		return true;
	}
	if (pLhs->m_userLimit < pRhs->m_userLimit) {
		return false;
	}

	return pLhs->m_userCount <= pRhs->m_userCount ? true : false;
}

// lobbymain.cpp:9760
bool RoomGuildCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	// The original compares the left room's distance with itself: both sides read lhs.
	const sRoomInfo* pLhs = (const sRoomInfo*)lhs;
	int lhsGuild0 = pLhs->m_guild.m_guildUID[0];
	int myGuild = WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().guildUID;
	int lhsGuild1 = pLhs->m_guild.m_guildUID[1];

	return min(Abs(lhsGuild0 - myGuild), Abs(lhsGuild1 - myGuild)) > min(Abs(lhsGuild0 - myGuild), Abs(lhsGuild1 - myGuild));
}

// lobbymain.cpp:9782
bool UserGenderCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sBriefUserInfo* pLhs = (const sBriefUserInfo*)lhs;
	const sBriefUserInfo* pRhs = (const sBriefUserInfo*)rhs;

	return pLhs->charType < pRhs->charType;
}

// lobbymain.cpp:9799
bool UserGenderRevCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sBriefUserInfo* pLhs = (const sBriefUserInfo*)lhs;
	const sBriefUserInfo* pRhs = (const sBriefUserInfo*)rhs;

	return pLhs->charType > pRhs->charType;
}

// lobbymain.cpp:9816
bool UserLevelCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sBriefUserInfo* pLhs = (const sBriefUserInfo*)lhs;
	const sBriefUserInfo* pRhs = (const sBriefUserInfo*)rhs;

	return pLhs->level < pRhs->level;
}

// lobbymain.cpp:9833
bool UserLevelRevCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sBriefUserInfo* pLhs = (const sBriefUserInfo*)lhs;
	const sBriefUserInfo* pRhs = (const sBriefUserInfo*)rhs;

	return pLhs->level > pRhs->level;
}

// lobbymain.cpp:9886
bool UserGuildCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sBriefUserInfo* pLhs = (const sBriefUserInfo*)lhs;
	const sBriefUserInfo* pRhs = (const sBriefUserInfo*)rhs;

	return pLhs->guildUID < pRhs->guildUID;
}

// lobbymain.cpp:9903
bool UserGuildRevCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sBriefUserInfo* pLhs = (const sBriefUserInfo*)lhs;
	const sBriefUserInfo* pRhs = (const sBriefUserInfo*)rhs;

	return pLhs->guildUID > pRhs->guildUID;
}

// lobbymain.cpp:9920
bool UserNicknameCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sBriefUserInfo* pLhs = (const sBriefUserInfo*)lhs;
	const sBriefUserInfo* pRhs = (const sBriefUserInfo*)rhs;

	return lstrcmpiA(pLhs->nick, pRhs->nick) < 0;
}

// lobbymain.cpp:9937
bool UserNicknameRevCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}

	const sBriefUserInfo* pLhs = (const sBriefUserInfo*)lhs;
	const sBriefUserInfo* pRhs = (const sBriefUserInfo*)rhs;

	return lstrcmpiA(pLhs->nick, pRhs->nick) > 0;
}

// lobbymain.cpp:9954
void CLobbyMain::RoomList_LoadRoomImages(void) {
	m_roomListBoxBmp[0][0] = g_pFresh->RegisterBitmap("roomlistbox_n_01");
	m_roomListBoxBmp[1][0] = g_pFresh->RegisterBitmap("roomlistbox_n_02");
	m_roomListBoxBmp[2][0] = g_pFresh->RegisterBitmap("roomlistbox_n_03");
	m_roomListBoxBmp[0][1] = g_pFresh->RegisterBitmap("roomlistbox_o_01");
	m_roomListBoxBmp[1][1] = g_pFresh->RegisterBitmap("roomlistbox_o_02");
	m_roomListBoxBmp[2][1] = g_pFresh->RegisterBitmap("roomlistbox_o_03");
	m_roomListBoxBmp[0][2] = g_pFresh->RegisterBitmap("roomlistbox_d_01");
	m_roomListBoxBmp[1][2] = g_pFresh->RegisterBitmap("roomlistbox_d_02");
	m_roomListBoxBmp[2][2] = g_pFresh->RegisterBitmap("roomlistbox_d_03");
	m_roomListBoxBmp[0][3] = g_pFresh->RegisterBitmap("roomlistbox_e_01");
	m_roomListBoxBmp[1][3] = g_pFresh->RegisterBitmap("roomlistbox_e_02");
	m_roomListBoxBmp[2][3] = g_pFresh->RegisterBitmap("roomlistbox_e_03");

	m_roomSortNoBmp[0] = g_pFresh->RegisterBitmap("room_sort_no_n");
	m_roomSortNoBmp[1] = g_pFresh->RegisterBitmap("room_sort_no_o");
	m_roomSortNoBmp[2] = g_pFresh->RegisterBitmap("room_sort_no_d");
	m_roomSortCateBmp[0] = g_pFresh->RegisterBitmap("room_sort_cate_n_down");
	m_roomSortCateBmp[1] = g_pFresh->RegisterBitmap("room_sort_cate_o");
	m_roomSortCateBmp[2] = g_pFresh->RegisterBitmap("room_sort_cate_d");
	m_roomSortHoleBmp[0] = g_pFresh->RegisterBitmap("room_sort_hole_n");
	m_roomSortHoleBmp[1] = g_pFresh->RegisterBitmap("room_sort_hole_o");
	m_roomSortHoleBmp[2] = g_pFresh->RegisterBitmap("room_sort_hole_d");
	m_roomSortCourseBmp[0] = g_pFresh->RegisterBitmap("room_sort_course_n");
	m_roomSortCourseBmp[1] = g_pFresh->RegisterBitmap("room_sort_course_o");
	m_roomSortCourseBmp[2] = g_pFresh->RegisterBitmap("room_sort_course_d");
	m_roomSortStateBmp[0] = g_pFresh->RegisterBitmap("room_sort_state_up_n");
	m_roomSortStateBmp[1] = g_pFresh->RegisterBitmap("room_sort_state_up_o");
	m_roomSortStateBmp[2] = g_pFresh->RegisterBitmap("room_sort_state_up_d");
	m_roomSortStateBmp[3] = g_pFresh->RegisterBitmap("room_sort_state_dn_n");
	m_roomSortStateBmp[4] = g_pFresh->RegisterBitmap("room_sort_state_dn_o");
	m_roomSortStateBmp[5] = g_pFresh->RegisterBitmap("room_sort_state_dn_d");

	m_genderBmp[0][0] = g_pFresh->RegisterBitmap("i_male");
	m_genderBmp[0][1] = g_pFresh->RegisterBitmap("i_female");
	m_genderBmp[1][0] = g_pFresh->RegisterBitmap("i_male_02");
	m_genderBmp[1][1] = g_pFresh->RegisterBitmap("i_female_02");
	m_genderBmp[2][0] = g_pFresh->RegisterBitmap("i_male_03");
	m_genderBmp[2][1] = g_pFresh->RegisterBitmap("i_female_03");

	m_genderMannerBmp[0] = g_pFresh->RegisterBitmap("i_male_manner");
	m_genderMannerBmp[1] = g_pFresh->RegisterBitmap("i_female_manner");

	m_genderAngelBmp[0] = g_pFresh->RegisterBitmap("i_male_angel");
	m_genderAngelBmp[1] = g_pFresh->RegisterBitmap("i_female_angel");

	m_userSortGenderBmp[0] = g_pFresh->RegisterBitmap("user_sort_gender_normal");
	m_userSortGenderBmp[1] = g_pFresh->RegisterBitmap("user_sort_gender_over");
	m_userSortGenderBmp[2] = g_pFresh->RegisterBitmap("user_sort_gender_click");
	m_userSortLevelBmp[0] = g_pFresh->RegisterBitmap("user_sort_level_normal");
	m_userSortLevelBmp[1] = g_pFresh->RegisterBitmap("user_sort_level_over");
	m_userSortLevelBmp[2] = g_pFresh->RegisterBitmap("user_sort_level_click");

	m_userSortGuildBmp[0] = g_pFresh->RegisterBitmap("user_sort_guild_normal");
	m_userSortGuildBmp[1] = g_pFresh->RegisterBitmap("user_sort_guild_over");
	m_userSortGuildBmp[2] = g_pFresh->RegisterBitmap("user_sort_guild_click");
	m_userSortNickBmp[0] = g_pFresh->RegisterBitmap("user_sort_nick_normal");
	m_userSortNickBmp[1] = g_pFresh->RegisterBitmap("user_sort_nick_over");
	m_userSortNickBmp[2] = g_pFresh->RegisterBitmap("user_sort_nick_click");

	m_userSortArrowBmp[0] = g_pFresh->RegisterBitmap("user_sort_up");
	m_userSortArrowBmp[1] = g_pFresh->RegisterBitmap("user_sort_down");

	m_roomImage = g_pFresh->RegisterBitmap("ingame");
}

// lobbymain.cpp:10044
void CLobbyMain::OnRoomList_Init(PG_MSGARG param) {
	CTaskMain::OnInit();

	WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType = 0x10;

	WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomMode = 0x10;

	RoomList_LoadRoomImages();

	_ISkinnedTheme* theme = S5::THEME::GetCurrentSkinnedTheme();
	g_pFresh->GetDesktop()->SetWallPaper(theme->GetRoomListBackGroundImgName(), true);

	SetTimeVariableBGM();
	TurnOff3DBackground();

	std::map<unsigned long, sRoomSlot>::iterator it;
	for (it = CSharedDoc::Instance()->m_roomSlotMap.begin(); it != CSharedDoc::Instance()->m_roomSlotMap.end(); ++it) {
		if (it->second.m_pPet) { delete it->second.m_pPet; it->second.m_pPet = NULL; }
	}
	CSharedDoc::Instance()->m_roomSlotMap.clear();

	if (IsLocalContent((localContentType_t)0x57)) {
		WSingleton<NetResourceManager>::Instance()->SetClearGarbage();
	}

	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	unsigned long games = doc->m_myStatGamesPlayed;
	if (games < 1) games = 1;
	if (!((float)doc->m_myStatNoMannerGames / (float)games * 100.0f < 3.0f)) {
		CPartTidList& partTidList = doc->GetMyPartTidList();
		sCharacterInfo& info = WSingleton<CSharedDoc>::Instance()->m_characterMap[WSingleton<CSharedDoc>::Instance()->m_myCharacterId];

		for (int i = 0; i < partTidList.m_numTotalParts; i++) {
			switch (info.partTypeIds[i]) {
			case 0x8016800:
			case 0x8058800:
			case 0x8098800:
			case 0x80DC800:
			case 0x8118800:
			case 0x8160800:
			case 0x8190800:
			case 0x81E2800:
			case 0x8214800:
			case 0x8254800:
				info.partTypeIds[i] = 0;
				info.partIds[i] = 0;
			}

			switch (partTidList.m_partTids[i]) {
			case 0x8016800:
			case 0x8058800:
			case 0x8098800:
			case 0x80DC800:
			case 0x8118800:
			case 0x8160800:
			case 0x8190800:
			case 0x81E2800:
			case 0x8214800:
			case 0x8254800:
				partTidList.m_partTids[i] = 0;
			}
		}
	}

	m_roomSort = (roomSort_t)6;
	m_roomSortPending = false;
}

// lobbymain.cpp:10101
void CLobbyMain::OnRoomList_Finish(void) {
	m_titleArea->SetBgImg("title_roomlist");
#ifdef JP_PAKS
	// The JP form draws its own title row where the over bar sits; the bar
	// stays for the controls this code keeps (docs/JP_CLIENT.md).
	if (m_barFrame[0] != 0) {
		m_barFrame[0]->SetVisible(false);
	}
#endif

	if (WSingleton<CSharedDoc>::Instance()->m_tutorialFinish) {
		this << MsgObject(0, 0xAA, 1, 0, 0, 0, 0);
	}
}

// lobbymain.cpp:10110
void CLobbyMain::OnRoomList_Destroy(void) {
}

// lobbymain.cpp:10114
void CLobbyMain::OnRoomList_RoomListInit(intptr_t param) {
	m_roomListBox = DYNAMIC_CAST(FrListBox, param);
	if (!m_roomListBox) {
		return;
	}

	m_roomListBox->UseDummy(true);
	m_roomListBox->UseRightButton(true);

	if (data_aa9eac.m_gameType != -1) {
		for (std::list<sRoomInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_roomList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_roomList.end(); ++it) {
			if (it->m_gameType == data_aa9eac.m_gameType) {
				m_roomListBox->AddItem(&*it);
			}
		}
	} else {
		for (std::list<sRoomInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_roomList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_roomList.end(); ++it) {
			if (GetCateByGameType(it->m_gameType) == data_aa9eac.m_roomCate || data_aa9eac.m_roomCate == ROOMCATE_ALL) {
				m_roomListBox->AddItem(&*it);
			}
		}
	}

	this << MsgObject(0, 0x0D, 0, 0, 0, 0, 0);
}

// lobbymain.cpp:10210
const char* RoomListGetGameCategory(unsigned char gameType) {
	switch (gameType) {
	case 0:
	case 1:
		return "\xB4\xEB\xC0\xFC";
	case 4:
	case 5:
	case 6:
		return "\xB4\xEB\xC8\xB8";
	case 7:
	case 9:
	case 10:
		return "\xB9\xE8\xC6\xB2";
	case 2:
		return "\xB4\xEB\xC8\xAD";
	}
	return 0;
}

// lobbymain.cpp:10235
unsigned long DecideRoomColor(sRoomInfo* info) {
	if (info->m_intrusionAccess && info->m_isPublic) {
		if (info->m_roomMode != 14) {
			if (!info->m_state) {
				return 0xFFFFFFFF;
			}
			return WSingleton<CIntrusion>::Instance()->GetActiveColor();
		}
		return 0;
	}
	return info->m_roomMode != 14 ? 0xB2FFFFFF : 0;
}

// lobbymain.cpp:10271
// Draws one row of the room list: the three background pieces (tinted by the
// room's state), the room number or event mark, the category, game type,
// title, course image, hole count and players.
void CLobbyMain::OnRoomList_RoomListOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;

	if (!pItem)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	sRoomInfo* pRoom = (sRoomInfo*)pItem->pData;
	int state = 0;
	unsigned long color = 0x37FFFFFF;

	if (pRoom) {
		if (pItem->selected) {
			state = 2;
		} else if (pItem->underCursor) {
			state = 1;
		}

		if (pRoom->m_intrusionAccess && pRoom->m_isPublic) {
			if (pRoom->m_state)
				color = WSingleton<CIntrusion>::Instance()->GetActiveColor();
			else
				color = 0xFFFFFFFF;
		} else {
			color = 0xB2FFFFFF;
		}

		if (pRoom->m_roomMode == 14) {
			if (pRoom->m_intrusionAccess && pRoom->m_isPublic) {
				color &= 0xFFDD77FF;
			} else {
				color &= 0xFFCC66DD;
			}
		}
	}

	pGDI->DrawTexture(m_roomListBoxBmp[0][state], WRect(pItem->pos.x, pItem->pos.y, 40.0f, 48.0f), color, 0);
	pGDI->DrawTexture(m_roomListBoxBmp[1][state], WRect(pItem->pos.x + 40.0f, pItem->pos.y, 438.0f, 48.0f), color, 0);
	pGDI->DrawTexture(m_roomListBoxBmp[2][state], WRect(pItem->pos.x + 478.0f, pItem->pos.y, 40.0f, 48.0f), color, 0);

	if (!pRoom)
		return;

	if (pItem->selected) {
		pGDI->DrawTexture(m_roomListBoxBmp[0][3], WRect(pItem->pos.x, pItem->pos.y, 40.0f, 48.0f), 0xFFFFFFFF, 0);
		pGDI->DrawTexture(m_roomListBoxBmp[1][3], WRect(pItem->pos.x + 40.0f, pItem->pos.y, 438.0f, 48.0f), 0xFFFFFFFF, 0);
		pGDI->DrawTexture(m_roomListBoxBmp[2][3], WRect(pItem->pos.x + 478.0f, pItem->pos.y, 40.0f, 48.0f), 0xFFFFFFFF, 0);
	}

	if (pRoom->m_userLimit >= 100) {
		if (!m_roomListImage)
			m_roomListImage = g_pFresh->GetBitmap("room_event");
		if (m_roomListImage)
			pGDI->DrawTexture(m_roomListImage, WRect(pItem->pos.x + 7.0f, pItem->pos.y + 3.0f, 43.0f, 38.0f), 0xFFFFFFFF, 0);

		pGDI->SetTextStyle(1);
		pGDI->SetTextColor(0xFF47A5DD, 0xFFFFFFFF);
	} else {
		Print(pItem->pos.x + 17.0f, pItem->pos.y + 18.0f, MakeStr("%03d", pRoom->m_roomNumber), 0.0f, 0, 1.0f, 0xFFFFB258);
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	}

	pGDI->PrintText(WPoint(pItem->pos.x + 59.0f, pItem->pos.y + 18.0f), 0, RoomListGetGameCategory(pRoom->m_gameType), 0);

	switch (pRoom->m_gameType) {
	case 6:
		if (pRoom->m_guild.m_guildUID[0]) {
			const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pRoom->m_guild.m_guildMark[0]);
			if (pEmblem)
				pGDI->DrawTexture(pEmblem, WRect(pItem->pos.x + 237.0f, pItem->pos.y + 12.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
		}

		if (pRoom->m_guild.m_guildUID[1]) {
			const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pRoom->m_guild.m_guildMark[1]);
			if (pEmblem)
				pGDI->DrawTexture(pEmblem, WRect(pItem->pos.x + 294.0f, pItem->pos.y + 12.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
		}

		pGDI->Print(WPoint(pItem->pos.x + 120.0f, pItem->pos.y + 18.0f), 0, "%s", GetGameTypeName(pRoom->m_gameType));
		break;

	case 9:
		pGDI->Print(WPoint(pItem->pos.x + 120.0f, pItem->pos.y + 18.0f), 0, "%s", GetGameTypeName(pRoom->m_gameType));
		break;

	default: {
		IFF_STRUCT::sMatch* pMatch = ItemManager()->FindMatch(pRoom->m_trophy);
		if (pMatch) {
			char name[80] = { 0 };

			if (pRoom->m_roomMode != 14) {
				strcpy(name, pMatch->m_name);
			} else {
				const char* pszName = "\xBC\xC5\xC7\xC3\xC4\xDA\xBD\xBA";
				strcpy(name, pszName);
			}

			pGDI->Print(WPoint(pItem->pos.x + 120.0f, pItem->pos.y + 18.0f), 0, "%s %s", name, pRoom->m_gameType == 5 ? "\xC6\xC0\xC0\xFC" : "");
		} else {
			pGDI->Print(WPoint(pItem->pos.x + 120.0f, pItem->pos.y + 18.0f), 0, "%s %s", GetGameTypeName(pRoom->m_gameType), pRoom->m_gameType == 5 ? "\xC6\xC0\xC0\xFC" : "");
		}
		break;
	}
	}

	pGDI->SetTextStyle(0);

	g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 221.0f, pItem->pos.y + 18.0f), 0, pRoom->m_gameType == 6 ? "            VS             " : pRoom->m_title, -1.0f, 0xFFFFFFFF);

	if (pRoom->m_roomMode != 14) {
		IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(0x28000000 | (pRoom->m_course >= 0x7F ? 0x7F : pRoom->m_course));
		if (pCourse) {
			const Bitmap* pCourseBmp = g_pFresh->GetBitmap(MakeStr("room_%s", pCourse->m_icon));
			if (pCourseBmp) {
				float x = pItem->pos.x + 442.0f;

				pGDI->DrawTexture(pCourseBmp, WRect(x, pItem->pos.y, (float)pCourseBmp->Width(), (float)pCourseBmp->Height()), 0xFFFFFFFF, 0);

				if (WSingleton<CSharedDoc>::Instance()->GetMapEventPangRate(pCourse->m_typeId & 0x3FFFFFF) > 100) {
					const Bitmap* pMark = g_pFresh->GetBitmap("lobby_mapevent_mark");
					if (pMark)
						pGDI->DrawTexture(pMark, WRect(x + (pCourseBmp->Width() - pMark->Width()), pItem->pos.y, (float)pMark->Width(), (float)pMark->Height()), 0xFFFFFFFF, 0);
				}
			}
		}
	} else {
		float x = pItem->pos.x + 442.0f;

		IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(0x2800007F);
		if (pCourse) {
			const Bitmap* pCourseBmp = g_pFresh->GetBitmap("room_map_17_01");
			if (pCourseBmp)
				pGDI->DrawTexture(pCourseBmp, WRect(x, pItem->pos.y, (float)pCourseBmp->Width(), (float)pCourseBmp->Height()), 0xFFFFFFFF, 0);
		}
	}

	pGDI->Print(WPoint(pItem->pos.x + 387.0f, pItem->pos.y + 18.0f), 2, "%d\xC8\xA6", pRoom->m_holeCount);

	if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80) {
		pGDI->Print(WPoint(pItem->pos.x + 434.0f, pItem->pos.y + 18.0f), 2, "  -  ");
	} else {
		pGDI->Print(WPoint(pItem->pos.x + 434.0f, pItem->pos.y + 18.0f), 2, "%d/%d", pRoom->m_userCount, pRoom->m_userLimit);
	}

	if (pRoom->m_gameType == 13)
		return;

	if ((WSingleton<CSharedDoc>::Instance()->m_curServer.m_flags2 & 0x20)
		&& (WSingleton<CSharedDoc>::Instance()->m_curServer.eventCourseMask & (1 << ((pRoom->m_course > 0x7F && pRoom->m_course != 0xFD) ? pRoom->m_course - 0x80 : pRoom->m_course)))) {
		const Bitmap* pIcon = g_pFresh->GetBitmap("icon_pangevent");
		pGDI->DrawTexture(pIcon, WRect(pItem->pos.x + 492.0f, pItem->pos.y + 4.0f, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);
	}

	if (IsLocalContent((localContentType_t)0x2B) && pRoom->m_userLimit >= 100) {
		pGDI->SetTextStyle(0);
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	}
}

// lobbymain.cpp:10627
void CLobbyMain::OnRoomList_RoomListLDown(void) {
	FrListItem* pItem = m_roomListBox->GetItemUnderCursor();

	if (pItem) {
		if (m_roomListBox) {
			m_roomListBox->SelectItem(pItem, true);
		}
		m_selectedRoomData = pItem->pData;
	} else {
		if (m_roomListBox) {
			m_roomListBox->SelectItem(0, true);
		}
		m_selectedRoomData = 0;
	}
}

// lobbymain.cpp:10650
// Right click on the room list: selects the room and asks the server for its
// details (an administrator also sees private rooms; a tournament channel
// only for rooms not open to intrusion).
void CLobbyMain::OnRoomList_RoomListRUp(void) {
	if (!m_roomListBox)
		return;

	FrListItem* item = m_roomListBox->GetItemUnderCursor();

	if (!item)
		return;

	sRoomInfo* room = (sRoomInfo*)item->pData;
	m_roomListBox->SelectItem(item, true);

	if (!room)
		return;

	if (room->m_isPublic == 0 && !AdminMode()) {
		this << MsgObject(this, 0x23, (intptr_t)"\xBA\xF1\xB0\xF8\xB0\xB3 \xB9\xE6\xC0\xBA \xB7\xEB\xC1\xA4\xBA\xB8\xB8\xA6 \xBA\xBC\xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
	} else if (!(WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80) || !room->m_intrusionAccess) {
		WSendPacket packet((enumClientPacket)0x2D);
		packet.Encode2(room->m_roomNumber);
		packet.Send(SEND_TO_DEFAULT);
	}

	m_selectedRoomData = room;
}

// lobbymain.cpp:10720
// Double click on the room list: joins the selected room. A room open to
// intrusion is joined through CIntrusion; otherwise the level, guild and
// full-room rules apply (an administrator skips them), then the join packet
// goes out, or the password dialog opens for a private room.
void CLobbyMain::OnRoomList_RoomListDClick(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80)
		return;

	if (!m_selectedRoomData)
		return;

	if (m_roomListBox && !m_roomListBox->IsEnabled())
		return;

	WSingleton<CIntrusion>::Instance()->FinishGame();

	if (static_cast<sRoomInfo*>(m_selectedRoomData)->m_intrusionAccess) {
		if (!AdminMode()) {
			if (static_cast<sRoomInfo*>(m_selectedRoomData)->m_userCount == static_cast<sRoomInfo*>(m_selectedRoomData)->m_userLimit) {
				this << MsgObject(this, 0x23, (intptr_t)"\xC1\xA4\xBF\xF8\xC0\xCC \xC3\xCA\xB0\xFA \xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
				return;
			}

			switch (static_cast<sRoomInfo*>(m_selectedRoomData)->m_gameType) {
			case 10:
				if (WSingleton<CSharedDoc>::Instance()->m_myLevel == 0) {
					this << MsgObject(this, 0x23, (intptr_t)"\xB7\xE7\xC5\xB0" "F \xB7\xB9\xBA\xA7\xC0\xBA \xBE\xEE\xC7\xC1\xB7\xCE\xC4\xA1 \xB0\xD4\xC0\xD3\xC0\xBB \xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
					return;
				}
				break;

			case 5: {
				std::map<unsigned long, sBriefUserInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_userMap.find(MyGuid(false));
				if (it != WSingleton<CSharedDoc>::Instance()->m_userMap.end() && it->second.charType > 1) {
					this << MsgObject(this, 0x23, (intptr_t)"\xB0\xAD\xC1\xA6 \xC1\xBE\xB7\xE1\xC0\xB2\xC0\xCC \xB3\xF4\xC0\xBA \xC0\xAF\xC0\xFA\xB4\xC2 30\xC0\xCE \xC6\xC0\xC0\xFC \xB0\xD4\xC0\xD3\xC0\xBB \xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
					return;
				}
				break;
			}

			case 6: {
				std::string message;

				unsigned long guildUID = WSingleton<CSharedDoc>::Instance()->m_userInfo.guildUID;
				bool bRefuse = false;

				if (guildUID == 0) {
					message = "\xB1\xE6\xB5\xE5\xBF\xA1 \xB0\xA1\xC0\xD4\xC7\xCF\xBC\xC5\xBE\xDF \xB1\xE6\xB5\xE5\xB4\xEB\xC0\xFC\xC0\xCC \xB0\xA1\xB4\xC9\xC7\xD5\xB4\xCF\xB4\xD9.";
					bRefuse = true;
				}

				if (static_cast<sRoomInfo*>(m_selectedRoomData)->m_guild.m_guildUID[0] != guildUID && static_cast<sRoomInfo*>(m_selectedRoomData)->m_guild.m_guildUID[0] != 0 && static_cast<sRoomInfo*>(m_selectedRoomData)->m_guild.m_guildUID[1] != guildUID && static_cast<sRoomInfo*>(m_selectedRoomData)->m_guild.m_guildUID[1] != 0) {
					message = "\xC5\xB8\xB1\xE6\xB5\xE5\xC0\xC7 \xB4\xEB\xC0\xFC\xC0\xD4\xB4\xCF\xB4\xD9.";
					bRefuse = true;
				}

				if (bRefuse) {
					this << MsgObject(this, 0x23, (intptr_t)message.c_str(), 0, 0, 0, 0);
					return;
				}
				break;
			}
			}
		}

		EnableRoomEnterControls(false);

		if ((WSingleton<CSharedDoc>::Instance()->m_userInfo.identity & 0xE) == 0xE) {
			EnableRoomEnterControls(false);
			WSendPacket packet((enumClientPacket)0x3E);
			packet.Encode2(static_cast<sRoomInfo*>(m_selectedRoomData)->m_roomNumber);
			packet.EncodeStr(std::string(""));
			packet.Send(SEND_TO_DEFAULT);

			this << MsgObject(0, 0x16, 0, 0, 0, 0, 0);
			this << MsgObject(0, 1, 0, 0, 0, 0, 0);
			return;
		}

		bool bManager = (WSingleton<CSharedDoc>::Instance()->m_userInfo.identity & 0x14) != 0;

		bool bInvited = false;
		if (IsLocalContent((localContentType_t)0x54) && WSingleton<CSharedDoc>::Instance()->IsInviteMode())
			bInvited = true;

		if (static_cast<sRoomInfo*>(m_selectedRoomData)->m_isPublic || bManager || bInvited) {
			if (IsLocalContent((localContentType_t)0x2E) && static_cast<sRoomInfo*>(m_selectedRoomData)->m_state == 1 && !bManager) {
				WSingleton<CIntrusion>::Instance()->DoJoinRoom(static_cast<sRoomInfo*>(m_selectedRoomData)->m_roomNumber);
				return;
			}

			WSendPacket packet((enumClientPacket)9);
			packet.Encode2(static_cast<sRoomInfo*>(m_selectedRoomData)->m_roomNumber);
			packet.EncodeStr(std::string(""));
			packet.Send(SEND_TO_DEFAULT);

			this << MsgObject(0, 0x16, 0, 0, 0, 0, 0);
			this << MsgObject(0, 1, 0, 0, 0, 0, 0);
		} else {
			WSingleton<CSharedDoc>::Instance()->m_roomInfo = *static_cast<sRoomInfo*>(m_selectedRoomData);

			if (!m_passwordDlg) {
				m_passwordDlg = CreateForm<FrPasswordDlg>(g_pFresh->GetManager(), this, "password", 0);
				m_passwordDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnPasswordDlgResult, 3);
			}
		}
	} else {
		if (static_cast<sRoomInfo*>(m_selectedRoomData)->m_state) {
			WSingleton<CSharedDoc>::Instance()->m_roomInfo = *static_cast<sRoomInfo*>(m_selectedRoomData);
			WSingleton<CIntrusion>::Instance()->DoJoinRoom(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomNumber);
			return;
		}

		HandleMsg(MsgObject(this, 0x23, (intptr_t)"\xB0\xD4\xC0\xD3 \xC1\xF8\xC7\xE0\xC1\xDF\xC0\xCE \xB9\xE6\xC0\xD4\xB4\xCF\xB4\xD9", 0, 0, 0, 0));
	}
}

// lobbymain.cpp:10933
void CLobbyMain::OnRoomList_RoomSortNoInit(intptr_t param) {
	m_roomSortNoViewer = DYNAMIC_CAST(FrViewer, param);
	m_roomSortDescending[0] = false;
}

// lobbymain.cpp:10939
void CLobbyMain::OnRoomList_RoomSortNoDown(void) {
	if (m_roomSortResetTime > 0.0f) {
		return;
	}

	{
		if (m_roomSort == 0) {
			m_roomSortPending ^= 1;
		}

		m_roomSortDescending[0] = false;
		m_roomSortDescending[1] = false;
		m_roomSortDescending[2] = false;
		m_roomSortDescending[3] = false;
		if (m_cateVSList) {
			m_cateVSList->SetVisible(false);
		}
		if (m_cateMassList) {
			m_cateMassList->SetVisible(false);
		}
		if (m_cateBattleList) {
			m_cateBattleList->SetVisible(false);
		}

		m_roomSortDescending[0] = true;
		m_roomSortResetTime = 0.3f;
		if (m_roomListBox) {
			m_roomSort = (roomSort_t)0;
			if (m_roomSortPending) {
				m_roomListBox->SortItem(RoomNoRevCompare);
			} else {
				m_roomListBox->SortItem(RoomNoCompare);
			}
		}
		g_audio->PlaySfx("ui_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
	}
}

// lobbymain.cpp:10955
void CLobbyMain::OnRoomList_RoomSortNoOwnerDraw(PG_MSGARG) {
	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();

	if (pGDI) {
		FrViewer* pViewer = m_roomSortNoViewer;

		if (pViewer) {
			int state = 0;

			if (m_roomSortDescending[0]) {
				state = 2;
			} else {
				WRect rect = pViewer->GetRect();

				if (rect.IsInRect(pManager->GetMousePos())) {
					state = 1;
				}
			}

			pGDI->DrawTexture(m_roomSortNoBmp[state], pViewer->GetRect(), 0xFFFFFFFF, 0);
		}
	}
}

// lobbymain.cpp:10982
void CLobbyMain::OnRoomList_RoomSortCateAll_BtnInit(intptr_t param) {
	m_cateAllBtn = DYNAMIC_CAST(FrButton, param);
	m_cateAllBtn->SetStatus(data_aa9eac.m_roomCate == ROOMCATE_ALL ? FrButton::PRESSED : FrButton::NORMAL);
}

// lobbymain.cpp:10988
void CLobbyMain::OnRoomList_RoomSortCateAll_BtnDown(void) {
	RoomList_FilterRoomList(ROOMSORT_CATEGORY, ROOMCATE_ALL);
	RoomList_SetCateTabBtn();
	data_aa9eac.m_gameType = -1;
}

// lobbymain.cpp:10996
void CLobbyMain::OnRoomList_RoomSortCateVS_BtnInit(intptr_t param) {
	m_cateVSBtn = DYNAMIC_CAST(FrButton, param);
	m_cateVSBtn->SetStatus(data_aa9eac.m_roomCate == ROOMCATE_VS ? FrButton::PRESSED : FrButton::NORMAL);
}

// lobbymain.cpp:11002
void CLobbyMain::OnRoomList_RoomSortCateVS_BtnDown(void) {
	RoomList_FilterRoomList(ROOMSORT_CATEGORY, ROOMCATE_VS);
	RoomList_SetCateTabBtn();
	m_cateNewRoom[0] = 0;
	data_aa9eac.m_gameType = -1;
}

// lobbymain.cpp:11011
void CLobbyMain::OnRoomList_RoomSortCateMass_BtnInit(intptr_t param) {
	m_cateMassBtn = DYNAMIC_CAST(FrButton, param);
	m_cateMassBtn->SetStatus(data_aa9eac.m_roomCate == ROOMCATE_MASS ? FrButton::PRESSED : FrButton::NORMAL);
}

// lobbymain.cpp:11017
void CLobbyMain::OnRoomList_RoomSortCateMass_BtnDown(void) {
	RoomList_FilterRoomList(ROOMSORT_CATEGORY, ROOMCATE_MASS);
	RoomList_SetCateTabBtn();
	m_cateNewRoom[1] = 0;
	data_aa9eac.m_gameType = -1;
}

// lobbymain.cpp:11026
void CLobbyMain::OnRoomList_RoomSortCateBattle_BtnInit(intptr_t param) {
	m_cateBattleBtn = DYNAMIC_CAST(FrButton, param);
	m_cateBattleBtn->SetStatus(data_aa9eac.m_roomCate == ROOMCATE_BATTLE ? FrButton::PRESSED : FrButton::NORMAL);
}

// lobbymain.cpp:11032
void CLobbyMain::OnRoomList_RoomSortCateBattle_BtnDown(void) {
	RoomList_FilterRoomList(ROOMSORT_CATEGORY, ROOMCATE_BATTLE);
	RoomList_SetCateTabBtn();
	m_cateNewRoom[2] = 0;
	data_aa9eac.m_gameType = -1;
}

// lobbymain.cpp:11041
void CLobbyMain::OnRoomList_RoomSortCateChat_BtnInit(intptr_t param) {
	m_cateChatBtn = DYNAMIC_CAST(FrButton, param);
	m_cateChatBtn->SetStatus(data_aa9eac.m_roomCate == ROOMCATE_CHAT ? FrButton::PRESSED : FrButton::NORMAL);
}

// lobbymain.cpp:11047
void CLobbyMain::OnRoomList_RoomSortCateChat_BtnDown(void) {
	RoomList_FilterRoomList(ROOMSORT_CATEGORY, ROOMCATE_CHAT);
	RoomList_SetCateTabBtn();
	data_aa9eac.m_gameType = -1;
}

// lobbymain.cpp:11155
void CLobbyMain::OnRoomList_RoomSortHoleInit(intptr_t param) {
	m_roomSortHoleViewer = DYNAMIC_CAST(FrViewer, param);
	m_roomSortDescending[1] = false;
}

// lobbymain.cpp:11161
void CLobbyMain::OnRoomList_RoomSortHoleDown(void) {
	if (m_roomSortResetTime > 0.0f) {
		return;
	}

	{
		if (m_roomSort == 4) {
			m_roomSortPending ^= 1;
		}

		m_roomSortDescending[0] = false;
		m_roomSortDescending[1] = false;
		m_roomSortDescending[2] = false;
		m_roomSortDescending[3] = false;
		if (m_cateVSList) {
			m_cateVSList->SetVisible(false);
		}
		if (m_cateMassList) {
			m_cateMassList->SetVisible(false);
		}
		if (m_cateBattleList) {
			m_cateBattleList->SetVisible(false);
		}

		m_roomSortDescending[1] = true;
		m_roomSortResetTime = 0.3f;
		if (m_roomListBox) {
			m_roomSort = (roomSort_t)4;
			if (m_roomSortPending) {
				m_roomListBox->SortItem(RoomHoleRevCompare);
			} else {
				m_roomListBox->SortItem(RoomHoleCompare);
			}
		}
		g_audio->PlaySfx("ui_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
	}
}

// lobbymain.cpp:11177
void CLobbyMain::OnRoomList_RoomSortHoleOwnerDraw(PG_MSGARG) {
	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();

	if (pGDI) {
		FrViewer* pViewer = m_roomSortHoleViewer;

		if (pViewer) {
			int state = 0;

			if (m_roomSortDescending[1]) {
				state = 2;
			} else {
				WRect rect = pViewer->GetRect();

				if (rect.IsInRect(pManager->GetMousePos())) {
					state = 1;
				}
			}

			pGDI->DrawTexture(m_roomSortHoleBmp[state], pViewer->GetRect(), 0xFFFFFFFF, 0);
		}
	}
}

// lobbymain.cpp:11204
void CLobbyMain::OnRoomList_RoomSortCourseInit(intptr_t param) {
	m_roomSortCourseViewer = DYNAMIC_CAST(FrViewer, param);
	m_roomSortDescending[2] = false;
}

// lobbymain.cpp:11210
void CLobbyMain::OnRoomList_RoomSortCourseDown(void) {
	if (m_roomSortResetTime > 0.0f) {
		return;
	}

	{
		if (m_roomSort == 5) {
			m_roomSortPending ^= 1;
		}

		m_roomSortDescending[0] = false;
		m_roomSortDescending[1] = false;
		m_roomSortDescending[2] = false;
		m_roomSortDescending[3] = false;
		if (m_cateVSList) {
			m_cateVSList->SetVisible(false);
		}
		if (m_cateMassList) {
			m_cateMassList->SetVisible(false);
		}
		if (m_cateBattleList) {
			m_cateBattleList->SetVisible(false);
		}

		m_roomSortDescending[2] = true;
		m_roomSortResetTime = 0.3f;
		if (m_roomListBox) {
			m_roomSort = (roomSort_t)5;
			if (m_roomSortPending) {
				m_roomListBox->SortItem(RoomCourseRevCompare);
			} else {
				m_roomListBox->SortItem(RoomCourseCompare);
			}
		}
		g_audio->PlaySfx("ui_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
	}
}

// lobbymain.cpp:11226
void CLobbyMain::OnRoomList_RoomSortCourseOwnerDraw(PG_MSGARG) {
	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();

	if (pGDI) {
		FrViewer* pViewer = m_roomSortCourseViewer;

		if (pViewer) {
			int state = 0;

			if (m_roomSortDescending[2]) {
				state = 2;
			} else {
				WRect rect = pViewer->GetRect();

				if (rect.IsInRect(pManager->GetMousePos())) {
					state = 1;
				}
			}

			pGDI->DrawTexture(m_roomSortCourseBmp[state], pViewer->GetRect(), 0xFFFFFFFF, 0);
		}
	}
}

// lobbymain.cpp:11253
void CLobbyMain::OnRoomList_RoomSortStateInit(intptr_t param) {
	m_roomSortStateViewer = DYNAMIC_CAST(FrViewer, param);
	m_roomSortDescending[3] = false;
}

// lobbymain.cpp:11259
void CLobbyMain::OnRoomList_RoomSortStateDown(void) {
	if (m_roomSortResetTime > 0.0f) {
		return;
	}

	{
		if (m_roomSort == 6) {
			m_roomSortPending ^= 1;
		}

		m_roomSortDescending[0] = false;
		m_roomSortDescending[1] = false;
		m_roomSortDescending[2] = false;
		m_roomSortDescending[3] = false;
		if (m_cateVSList) {
			m_cateVSList->SetVisible(false);
		}
		if (m_cateMassList) {
			m_cateMassList->SetVisible(false);
		}
		if (m_cateBattleList) {
			m_cateBattleList->SetVisible(false);
		}

		m_roomSortDescending[3] = true;
		m_roomSortResetTime = 0.3f;
		if (m_roomListBox) {
			m_roomSort = (roomSort_t)6;
			if (m_roomSortPending) {
				m_roomListBox->SortItem(RoomStateRevCompare);
			} else {
				m_roomListBox->SortItem(RoomStateCompare);
			}
		}
		g_audio->PlaySfx("ui_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
	}
}

// lobbymain.cpp:11275
void CLobbyMain::OnRoomList_RoomSortStateOwnerDraw(PG_MSGARG) {
	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();

	if (pGDI) {
		FrViewer* pViewer = m_roomSortStateViewer;

		if (pViewer) {
			int state = 0;

			if (m_roomSortDescending[3]) {
				state = 2;
			} else {
				WRect rect = pViewer->GetRect();

				if (rect.IsInRect(pManager->GetMousePos())) {
					state = 1;
				}
			}

			pGDI->DrawTexture(m_roomSortStateBmp[state + m_roomSortPending * 3], pViewer->GetRect(), 0xFFFFFFFF, 0);
		}
	}
}

// lobbymain.cpp:11300
void CLobbyMain::OnRoomList_RoomSortCateVS_ListInit(intptr_t param) {
	m_cateVSList = DYNAMIC_CAST(FrListBox, param);
	if (m_cateVSList) {
		for (int i = 0; i < 2; i++) {
			m_cateVSList->AddItem(&g_strGameType[i]);
		}

		m_cateVSList->SetVisible(false);
		m_cateVSList->EnableScrollBar(false);
	}
}

// lobbymain.cpp:11316
// Click on the VS game type drop list: filters the room list by the
// clicked game type and hides the list.
void CLobbyMain::OnRoomList_RoomSortCateVS_ListLUp(void) {
	int count = m_cateVSList->GetCurrentItemSize(true);

	for (int i = 0; i < count; i++) {
		FrListItem* item = m_cateVSList->GetItem(i);
		if (item) {
			WRect rect(item->pos.x + 1.0f, item->pos.y + 1.0f, 54.0f, 21.0f);
			if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
				m_roomListDirty = true;

				RoomList_FilterRoomList(ROOMSORT_GAMETYPE, item->idx);
				break;
			}
		}
	}

	if (m_cateVSList)
		m_cateVSList->SetVisible(false);
}

// lobbymain.cpp:11341
void CLobbyMain::OnRoomList_RoomSortCateVS_ListOwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI) {
		FrListItem* pItem = (FrListItem*)param;
		if (pItem) {
			std::string* pText = (std::string*)pItem->pData;

			WPoint pt(pItem->pos.x, pItem->pos.y);

			WOverlay::DrawBox(g_view, WRect(pt, 56.0f, 22.0f), 0, 0x880E4F75, 0);

			{
				WRect rect(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f, 54.0f, 20.0f);
				if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
					pGDI->Box(rect, 0x88082B46, 0, 0);
					pGDI->SetTextStyle(1);
				} else {
					pGDI->SetTextStyle(0);
				}
			}

			pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
			pGDI->Print(WPoint(pItem->pos.x + 2.0f, pItem->pos.y + 6.0f), 0, pText->c_str());
		}
	}
}

// lobbymain.cpp:11370
void CLobbyMain::OnRoomList_RoomSortCateMass_ListInit(intptr_t param) {
	m_cateMassList = DYNAMIC_CAST(FrListBox, param);
	if (m_cateMassList) {
		for (int i = 2; i < 5; i++) {
			m_cateMassList->AddItem(&g_strGameType[i]);
		}

		m_cateMassList->SetVisible(false);
		m_cateMassList->EnableScrollBar(false);
	}
}

// lobbymain.cpp:11386
// Click on the Mass game type drop list: filters the room list by the
// clicked game type and hides the list.
void CLobbyMain::OnRoomList_RoomSortCateMass_ListLUp(void) {
	int count = m_cateMassList->GetCurrentItemSize(true);

	for (int i = 0; i < count; i++) {
		FrListItem* item = m_cateMassList->GetItem(i);
		if (item) {
			WRect rect(item->pos.x + 1.0f, item->pos.y + 1.0f, 54.0f, 21.0f);
			if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
				m_roomListDirty = true;

				RoomList_FilterRoomList(ROOMSORT_GAMETYPE, item->idx + 4);
				break;
			}
		}
	}

	if (m_cateMassList)
		m_cateMassList->SetVisible(false);
}

// lobbymain.cpp:11411
void CLobbyMain::OnRoomList_RoomSortCateMass_ListOwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI) {
		FrListItem* pItem = (FrListItem*)param;
		if (pItem) {
			std::string* pText = (std::string*)pItem->pData;

			WPoint pt(pItem->pos.x, pItem->pos.y);

			WOverlay::DrawBox(g_view, WRect(pt, 56.0f, 22.0f), 0, 0x880E4F75, 0);

			{
				WRect rect(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f, 54.0f, 20.0f);
				if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
					pGDI->Box(rect, 0x88082B46, 0, 0);
					pGDI->SetTextStyle(1);
				} else {
					pGDI->SetTextStyle(0);
				}
			}

			pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
			pGDI->Print(WPoint(pItem->pos.x + 2.0f, pItem->pos.y + 6.0f), 0, pText->c_str());
		}
	}
}

// lobbymain.cpp:11441
void CLobbyMain::OnRoomList_RoomSortCateBattle_ListInit(intptr_t param) {
	m_cateBattleList = DYNAMIC_CAST(FrListBox, param);
	if (m_cateBattleList) {
		for (int i = 5; i < 7; i++) {
			m_cateBattleList->AddItem(&g_strGameType[i]);
		}

		m_cateBattleList->SetVisible(false);
		m_cateBattleList->EnableScrollBar(false);
	}
}

// lobbymain.cpp:11457
// Click on the Battle game type drop list: filters the room list by the
// clicked game type and hides the list.
void CLobbyMain::OnRoomList_RoomSortCateBattle_ListLUp(void) {
	int count = m_cateBattleList->GetCurrentItemSize(true);

	for (int i = 0; i < count; i++) {
		FrListItem* item = m_cateBattleList->GetItem(i);
		if (item) {
			WRect rect(item->pos.x + 1.0f, item->pos.y + 1.0f, 54.0f, 21.0f);
			if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
				m_roomListDirty = true;

				RoomList_FilterRoomList(ROOMSORT_GAMETYPE, item->idx != 0 ? 10 : 7);
				break;
			}
		}
	}

	if (m_cateBattleList)
		m_cateBattleList->SetVisible(false);
}

// lobbymain.cpp:11481
void CLobbyMain::OnRoomList_RoomSortCateBattle_ListOwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI) {
		FrListItem* pItem = (FrListItem*)param;
		if (pItem) {
			std::string* pText = (std::string*)pItem->pData;

			WPoint pt(pItem->pos.x, pItem->pos.y);

			WOverlay::DrawBox(g_view, WRect(pt, 56.0f, 22.0f), 0, 0x880E4F75, 0);

			{
				WRect rect(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f, 54.0f, 20.0f);
				if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
					pGDI->Box(rect, 0x88082B46, 0, 0);
					pGDI->SetTextStyle(1);
				} else {
					pGDI->SetTextStyle(0);
				}
			}

			pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
			pGDI->Print(WPoint(pItem->pos.x + 2.0f, pItem->pos.y + 6.0f), 0, pText->c_str());
		}
	}
}

// lobbymain.cpp:11523
void CLobbyMain::OnRoomList_MakeRoomInit(intptr_t param) {
	m_makeRoomBtn = DYNAMIC_CAST(FrButton, param);
	if (m_makeRoomBtn) {
		_ISkinnedTheme* theme = S5::THEME::GetCurrentSkinnedTheme();
		THEME_BUTTONS buttons;
		theme->GetMakeRoomButtonImgName(buttons);

		m_makeRoomBtn->SetButtonImg(buttons.szPush, FrButton::PRESSED);
		m_makeRoomBtn->SetButtonImg(buttons.szNormal, FrButton::NORMAL);
		m_makeRoomBtn->SetButtonImg(buttons.szOver, FrButton::OVER);
	}
}

// lobbymain.cpp:11601
void CLobbyMain::OnRoomList_GuildInfoInit(intptr_t param) {
	m_guildInfoBtn = DYNAMIC_CAST(FrButton, param);
	if (m_guildInfoBtn) {
		_ISkinnedTheme* theme = S5::THEME::GetCurrentSkinnedTheme();

		if (theme) {
			THEME_BUTTONS buttons;
			theme->GetQuickStartButtonImgName(buttons);

			m_guildInfoBtn->SetButtonImg(buttons.szPush, FrButton::PRESSED);
			m_guildInfoBtn->SetButtonImg(buttons.szNormal, FrButton::NORMAL);
			m_guildInfoBtn->SetButtonImg(buttons.szOver, FrButton::OVER);
			m_guildInfoBtn->SetPushDelay(0.1f);
		}
	}
}

// lobbymain.cpp:11626
void CLobbyMain::OnRoomList_GuildInfoLButtonUp(void) {
	BOOL controlled = CSharedDoc::Instance()->IsControlServerService(0x1000000);

	if (controlled) {
		this << MsgObject(this, 0x23, (intptr_t)"\xB1\xE6\xB5\xE5 \xB0\xFC\xB7\xC3 \xBA\xCE\xBA\xD0 \xC1\xA1\xB0\xCA\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
	} else {
		IActor *pActor = AfxGetTask()->GetActor(data_aa9eac.m_actorName);
		if (!pActor) {
			return;
		}

		pActor << MsgObject(this, 0x263, 1, 0, 0, 0, 0);
	}
}

// lobbymain.cpp:11820
void CLobbyMain::OnRoomList_UserListInit(intptr_t param) {
	m_userListBox = DYNAMIC_CAST(FrListBox, param);
	if (!m_userListBox) {
		return;
	}

	m_userListBox->UseRightButton(true);

	for (std::map<unsigned long, sBriefUserInfo>::const_iterator it = WSingleton<CSharedDoc>::Instance()->m_userMap.begin();
		it != WSingleton<CSharedDoc>::Instance()->m_userMap.end(); ++it) {
		const sBriefUserInfo* user = &it->second;

		// An administrator sees everyone; others see an administrator only
		// while its invisible bit is set.
		if (!WSingleton<CSharedDoc>::Instance()->m_userInfo.identityBits.administrator) {
			if (sBriefUserInfo(*user).identityBits.administrator) {
				if (!sBriefUserInfo(*user).invisible) {
					continue;
				}
			}
		}

		m_userListBox->AddItem((void*)user);
	}
}

// lobbymain.cpp:11854
void CLobbyMain::OnRoomList_UserListLBtnUp(void) {
	FrListItem* item = m_userListBox->GetItemUnderCursor();
	if (item) {
		sBriefUserInfo* info = (sBriefUserInfo*)item->pData;

		if (info) {
			m_selectedUserId = info->uid;
			m_removeFriendUid = info->id;

			if (m_userActionBtn) {
				m_userActionBtn->Enable(true);
			}
		}

		item->selected = true;
	}
}

// lobbymain.cpp:11874
// Right click on the room list screen's user list: selects the user and opens
// its user info, or says the user is not valid.
void CLobbyMain::OnRoomList_UserListRBtnUp(void) {
	FrListItem* item = m_userListBox->GetItemUnderCursor();
	if (item) {
		sBriefUserInfo* info = (sBriefUserInfo*)item->pData;

		if (info) {
			m_selectedUserId = info->uid;
			m_removeFriendUid = info->id;

			if (m_userActionBtn) {
				m_userActionBtn->Enable(true);
			}

			if (WSingleton<CUserInfo>::Instance()) {
				WSingleton<CUserInfo>::Instance()->SetInfo(info->id, info->uid, true, true, false, false, info->nick);
			}
		} else {
			CChatMsg::Instance()->AddChatMsg("\xC0\xDF\xB8\xF8\xB5\xC8 \xB4\xEB\xBB\xF3\xC0\xD4\xB4\xCF\xB4\xD9.", 7, true, false);
		}

		item->selected = true;
	}
}

// lobbymain.cpp:11905
// Draws one row of the room list screen's user list: the selection box, the
// gender icon (angel / manner variants), the in-room mark, the level (or
// ladder, or title) icon, the guild emblem and the nick.
void CLobbyMain::OnRoomList_UserListOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;

	if (!pItem)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	sBriefUserInfo* pUser = (sBriefUserInfo*)pItem->pData;
	if (!pUser)
		return;

	unsigned long color;
	if (pUser->identity & 0x14) {
		if (WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().identity & 0x14) {
			color = 0x50FFFFFF;
		} else {
			if (!pUser->invisible)
				return;
			color = 0xFFFFFFFF;
		}
	} else if (IsLocalContent((localContentType_t)0x10)) {
		// sBriefUserInfo lacks the dword at 0x40 (1: pink nick)
		color = pUser->nickColorType == 1 ? 0xFF33DBFF : 0xFFFFFFFF;
	} else {
		color = 0xFFFFFFFF;
	}

	if (pItem->selected) {
		if (m_selectedUserId == ((sBriefUserInfo*)pItem->pData)->uid) {
			pGDI->Box(WRect(pItem->pos.x, pItem->pos.y, (float)m_userListBox->GetItemWidth(), (float)m_userListBox->GetItemHeight()), 0xFF265785, 0, 0);
		} else {
			pItem->selected = false;
		}
	}

	// sBriefUserInfo lacks bits 5 (angel) and 4 (manner) of the byte at 0x2D
	const Bitmap* pGender;
	if (pUser->angel) {
		pGender = m_genderAngelBmp[pUser->charType % 2];
	} else if (pUser->manner) {
		pGender = m_genderMannerBmp[pUser->charType % 2];
	} else {
		pGender = m_genderBmp[0][pUser->charType];
	}

	WRect rc(pItem->pos.x + 8.0f, pItem->pos.y, pGender->Width(), pGender->Height());
	pGDI->DrawTexture(pGender, rc, 0xFFFFFFFF, 0);

	if (pUser->roomNumber != 0xFFFF)
		pGDI->DrawTexture(m_roomImage, WRect(rc.x, rc.y + 3.0f, (float)m_roomImage->Width(), (float)m_roomImage->Height()), 0xFFFFFFFF, 0);

	const Bitmap* pLevel;
	if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80) {
		pLevel = g_pFresh->GetBitmap(MakeStr("ladder_%03d", pUser->rankValue / 100));
	} else if (pUser->titleTypeId) {
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pUser->titleTypeId);
		if (pSkin)
			pLevel = g_pFresh->GetBitmap(pSkin->m_icon);
		else
			pLevel = g_pFresh->GetBitmap(MakeStr("level_%03d", pUser->level + 1));
	} else {
		pLevel = g_pFresh->GetBitmap(MakeStr("level_%03d", pUser->level + 1));
	}

	if (pLevel)
		pGDI->DrawTexture(pLevel, WRect(pItem->pos.x + 66.0f - float2int(pLevel->Width() * 0.5f), pItem->pos.y + 11.0f - float2int(pLevel->Height() * 0.5f), (float)pLevel->Width(), (float)pLevel->Height()), 0xFFFFFFFF, 0);

	if (pUser->guildUID && !(pUser->identity & 0x14)) {
		const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pUser->guildMark);

		if (pEmblem)
			pGDI->DrawTexture(pEmblem, WRect(pItem->pos.x + 106.0f, pItem->pos.y + 7.0f - 6.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
	}

	if (WSingleton<CProjectG>::Instance()->HidePrivacy())
		return;

	if (pItem->underCursor) {
		pGDI->SetTextColor(0xFFFFFFFF, 0xFF124371);
		pGDI->SetTextStyle(2);
	} else {
		pGDI->SetTextColor(color, 0xFFFFFFFF);
		pGDI->SetTextStyle(0);
	}

	g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 143.0f, pItem->pos.y + 7.0f), 0, pUser->nick, (float)m_userListBox->GetItemWidth() - 143.0f, color);
}

// lobbymain.cpp:12203
void CLobbyMain::OnRoomList_UserSortGenderInit(intptr_t param) {
	m_userSortGenderViewer = DYNAMIC_CAST(FrViewer, param);
	m_userSortDescending[0] = false;
}

// lobbymain.cpp:12209
void CLobbyMain::OnRoomList_UserSortGenderUp(void) {
	if (m_userSortResetTime > 0.0f) {
		return;
	}

	{
		if (m_userSort == 0) {
			m_userSortPending ^= 1;
		} else {
			m_userSortPending = false;
		}

		m_userSortDescending[0] = true;
		m_userSortDescending[1] = false;
		m_userSortDescending[2] = false;
		m_userSortDescending[3] = false;
		m_userSortResetTime = 0.3f;
		if (m_userListBox) {
			m_userSort = (userSort_t)0;
			if (m_userSortPending) {
				m_userListBox->SortItem(UserGenderRevCompare);
			} else {
				m_userListBox->SortItem(UserGenderCompare);
			}
		}
		g_audio->PlaySfx("ui_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
	}
}

// lobbymain.cpp:12231
void CLobbyMain::OnRoomList_UserSortGenderOwnerDraw(PG_MSGARG) {
	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();

	if (pGDI) {
		FrViewer* pViewer = m_userSortGenderViewer;

		if (pViewer) {
			int state = 0;

			if (m_userSortDescending[0]) {
				state = 2;
			} else {
				WRect rect = pViewer->GetRect();

				if (rect.IsInRect(pManager->GetMousePos())) {
					state = 1;
				}
			}

			WRect rect = pViewer->GetRect();
			pGDI->DrawTexture(m_userSortGenderBmp[state], rect, 0xFFFFFFFF, 0);

			if (m_userSort == 0) {
				WRect arrow(rect.x + rect.w, rect.y + 5.0f, 12.0f, 12.0f);
				pGDI->DrawTexture(m_userSortArrowBmp[m_userSortPending == 0], arrow, 0xFFFFFFFF, 0);
			}
		}
	}
}

// lobbymain.cpp:12263
void CLobbyMain::OnRoomList_UserSortLevelInit(intptr_t param) {
	m_userSortLevelViewer = DYNAMIC_CAST(FrViewer, param);
	m_userSortDescending[1] = false;
}

// lobbymain.cpp:12269
void CLobbyMain::OnRoomList_UserSortLevelUp(void) {
	if (m_userSortResetTime > 0.0f) {
		return;
	}

	{
		if (m_userSort == 1) {
			m_userSortPending ^= 1;
		} else {
			m_userSortPending = false;
		}

		m_userSortDescending[0] = false;
		m_userSortDescending[1] = true;
		m_userSortDescending[2] = false;
		m_userSortDescending[3] = false;
		m_userSortResetTime = 0.3f;
		if (m_userListBox) {
			m_userSort = (userSort_t)1;
			if (m_userSortPending) {
				m_userListBox->SortItem(UserLevelRevCompare);
			} else {
				m_userListBox->SortItem(UserLevelCompare);
			}
		}
		g_audio->PlaySfx("ui_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
	}
}

// lobbymain.cpp:12290
void CLobbyMain::OnRoomList_UserSortLevelOwnerDraw(PG_MSGARG) {
	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();

	if (pGDI) {
		FrViewer* pViewer = m_userSortLevelViewer;

		if (pViewer) {
			int state = 0;

			if (m_userSortDescending[1]) {
				state = 2;
			} else {
				WRect rect = pViewer->GetRect();

				if (rect.IsInRect(pManager->GetMousePos())) {
					state = 1;
				}
			}

			WRect rect = pViewer->GetRect();
			pGDI->DrawTexture(m_userSortLevelBmp[state], rect, 0xFFFFFFFF, 0);

			if (m_userSort == 1) {
				WRect arrow(rect.x + rect.w, rect.y + 5.0f, 12.0f, 12.0f);
				pGDI->DrawTexture(m_userSortArrowBmp[m_userSortPending == 0], arrow, 0xFFFFFFFF, 0);
			}
		}
	}
}

// lobbymain.cpp:12369
void CLobbyMain::OnRoomList_UserSortGuildInit(intptr_t param) {
	m_userSortGuildViewer = DYNAMIC_CAST(FrViewer, param);
	m_userSortDescending[2] = false;
}

// lobbymain.cpp:12375
void CLobbyMain::OnRoomList_UserSortGuildUp(void) {
	if (m_userSortResetTime > 0.0f) {
		return;
	}

	{
		if (m_userSort == 2) {
			m_userSortPending ^= 1;
		} else {
			m_userSortPending = false;
		}

		m_userSortDescending[0] = false;
		m_userSortDescending[1] = false;
		m_userSortDescending[2] = true;
		m_userSortDescending[3] = false;
		m_userSortResetTime = 0.3f;
		if (m_userListBox) {
			m_userSort = (userSort_t)2;
			if (m_userSortPending) {
				m_userListBox->SortItem(UserGuildRevCompare);
			} else {
				m_userListBox->SortItem(UserGuildCompare);
			}
		}
		g_audio->PlaySfx("ui_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
	}
}

// lobbymain.cpp:12396
void CLobbyMain::OnRoomList_UserSortGuildOwnerDraw(PG_MSGARG) {
	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();

	if (pGDI) {
		FrViewer* pViewer = m_userSortGuildViewer;

		if (pViewer) {
			int state = 0;

			if (m_userSortDescending[2]) {
				state = 2;
			} else {
				WRect rect = pViewer->GetRect();

				if (rect.IsInRect(pManager->GetMousePos())) {
					state = 1;
				}
			}

			WRect rect = pViewer->GetRect();
			pGDI->DrawTexture(m_userSortGuildBmp[state], rect, 0xFFFFFFFF, 0);

			if (m_userSort == 2) {
				WRect arrow(rect.x + rect.w, rect.y + 5.0f, 12.0f, 12.0f);
				pGDI->DrawTexture(m_userSortArrowBmp[m_userSortPending == 0], arrow, 0xFFFFFFFF, 0);
			}
		}
	}
}

// lobbymain.cpp:12428
void CLobbyMain::OnRoomList_UserSortNickInit(intptr_t param) {
	m_userSortNickViewer = DYNAMIC_CAST(FrViewer, param);
	m_userSortDescending[3] = false;
}

// lobbymain.cpp:12434
void CLobbyMain::OnRoomList_UserSortNickUp(void) {
	if (m_userSortResetTime > 0.0f) {
		return;
	}

	{
		if (m_userSort == 3) {
			m_userSortPending ^= 1;
		} else {
			m_userSortPending = false;
		}

		m_userSortDescending[0] = false;
		m_userSortDescending[1] = false;
		m_userSortDescending[2] = false;
		m_userSortDescending[3] = true;
		m_userSortResetTime = 0.3f;
		if (m_userListBox) {
			m_userSort = (userSort_t)3;
			if (m_userSortPending) {
				m_userListBox->SortItem(UserNicknameRevCompare);
			} else {
				m_userListBox->SortItem(UserNicknameCompare);
			}
		}
		g_audio->PlaySfx("ui_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
	}
}

// lobbymain.cpp:12455
void CLobbyMain::OnRoomList_UserSortNickOwnerDraw(PG_MSGARG) {
	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();

	if (pGDI) {
		FrViewer* pViewer = m_userSortNickViewer;

		if (pViewer) {
			int state = 0;

			if (m_userSortDescending[3]) {
				state = 2;
			} else {
				WRect rect = pViewer->GetRect();

				if (rect.IsInRect(pManager->GetMousePos())) {
					state = 1;
				}
			}

			WRect rect = pViewer->GetRect();
			pGDI->DrawTexture(m_userSortNickBmp[state], rect, 0xFFFFFFFF, 0);

			if (m_userSort == 3) {
				WRect arrow(rect.x + rect.w, rect.y + 5.0f, 12.0f, 12.0f);
				pGDI->DrawTexture(m_userSortArrowBmp[m_userSortPending == 0], arrow, 0xFFFFFFFF, 0);
			}
		}
	}
}

// lobbymain.cpp:12487
// Room list category tab of a game type (ROOMCATE_*): Mass for 3-5, Battle for
// 6 and 8-10, chat for 1, otherwise 0.
static int RoomList_GetRoomCate(unsigned char gameType) {
	switch (gameType) {
	case 3:
	case 4:
	case 5:
		return 1;
	case 6:
	case 8:
	case 9:
	case 10:
		return 2;
	case 1:
		return 3;
	case 0:
		return 0;
	case 2:
		return 0;
	case 7:
		return 0;
	}
	return 0;
}

// Refills the room list with the rooms of a category tab (ROOMSORT_CATEGORY)
// or of one game type (ROOMSORT_GAMETYPE), padded to six rows.
void CLobbyMain::RoomList_FilterRoomList(roomSort_t sort, int cate) {
	if (!m_roomListBox)
		return;

	m_roomListBox->ClearItem();

	if (sort == ROOMSORT_CATEGORY) {
		data_aa9eac.m_roomCate = cate;
		for (std::list<sRoomInfo>::iterator it = CSharedDoc::Instance()->m_roomList.begin(); it != CSharedDoc::Instance()->m_roomList.end(); ++it) {
			if (RoomList_GetRoomCate(it->m_gameType) == data_aa9eac.m_roomCate || data_aa9eac.m_roomCate == ROOMCATE_ALL)
				m_roomListBox->AddItem(&*it);
		}
	} else if (sort == ROOMSORT_GAMETYPE) {
		data_aa9eac.m_gameType = cate;
		for (std::list<sRoomInfo>::iterator it = CSharedDoc::Instance()->m_roomList.begin(); it != CSharedDoc::Instance()->m_roomList.end(); ++it) {
			if (it->m_gameType == cate)
				m_roomListBox->AddItem(&*it);
		}
	}

	for (int i = m_roomListBox->GetCurrentItemSize(true); i < 6; i++)
		m_roomListBox->AddItem(NULL);
}

// lobbymain.cpp:12522
void CLobbyMain::RoomList_SortRoomList(roomSort_t sort, int) {
	if (m_roomListBox == NULL) {
		return;
	}

	m_roomSort = sort;

	switch (sort) {
	case ROOMSORT_NO:
		if (m_roomSortPending) {
			m_roomListBox->SortItem(RoomNoRevCompare);
		} else {
			m_roomListBox->SortItem(RoomNoCompare);
		}
		break;
	case ROOMSORT_TITLE:
		if (m_roomSortPending) {
			m_roomListBox->SortItem(RoomTitleRevCompare);
		} else {
			m_roomListBox->SortItem(RoomTitleCompare);
		}
		break;
	case ROOMSORT_HOLE:
		if (m_roomSortPending) {
			m_roomListBox->SortItem(RoomHoleRevCompare);
		} else {
			m_roomListBox->SortItem(RoomHoleCompare);
		}
		break;
	case ROOMSORT_GAMETYPE:
		if (m_roomSortPending) {
			m_roomListBox->SortItem(RoomGameTypeRevCompare);
		} else {
			m_roomListBox->SortItem(RoomGameTypeCompare);
		}
		// no break in the original: falls into the course sort
	case ROOMSORT_COURSE:
		if (m_roomSortPending) {
			m_roomListBox->SortItem(RoomCourseRevCompare);
		} else {
			m_roomListBox->SortItem(RoomCourseCompare);
		}
		break;
	case ROOMSORT_STATE:
		if (m_roomSortPending) {
			m_roomListBox->SortItem(RoomStateRevCompare);
		} else {
			m_roomListBox->SortItem(RoomStateCompare);
		}
		break;
	}
}

// lobbymain.cpp:12572
void CLobbyMain::RoomList_SetCateTabBtn(void) {
	// data_aa9eac.m_roomCate: the room list's current category (ROOMCATE_*)
	m_cateVSBtn->SetStatus(data_aa9eac.m_roomCate == ROOMCATE_VS ? FrButton::PRESSED : FrButton::NORMAL);
	m_cateMassBtn->SetStatus(data_aa9eac.m_roomCate == ROOMCATE_MASS ? FrButton::PRESSED : FrButton::NORMAL);
	m_cateBattleBtn->SetStatus(data_aa9eac.m_roomCate == ROOMCATE_BATTLE ? FrButton::PRESSED : FrButton::NORMAL);
	m_cateChatBtn->SetStatus(data_aa9eac.m_roomCate == ROOMCATE_CHAT ? FrButton::PRESSED : FrButton::NORMAL);
	m_cateAllBtn->SetStatus(data_aa9eac.m_roomCate == ROOMCATE_ALL ? FrButton::PRESSED : FrButton::NORMAL);
}

// lobbymain.cpp:12583
// Sorts the room list screen's user list by gender, level, guild or nick
// (m_userSortPending: descending).
void CLobbyMain::RoomList_SortUserList(userSort_t sort) {
	if (!m_userListBox)
		return;

	m_userSort = sort;

	switch (sort) {
	case 0:
		if (m_userSortPending)
			m_userListBox->SortItem(UserGenderRevCompare);
		else
			m_userListBox->SortItem(UserGenderCompare);
		break;
	case 1:
		if (m_userSortPending)
			m_userListBox->SortItem(UserLevelRevCompare);
		else
			m_userListBox->SortItem(UserLevelCompare);
		break;
	case 2:
		if (m_userSortPending)
			m_userListBox->SortItem(UserGuildRevCompare);
		else
			m_userListBox->SortItem(UserGuildCompare);
		break;
	case 3:
		if (m_userSortPending)
			m_userListBox->SortItem(UserNicknameRevCompare);
		else
			m_userListBox->SortItem(UserNicknameCompare);
		break;
	}
}

// lobbymain.cpp:12631
void CLobbyMain::SetControlsRoomListSort(int sort) {
	m_roomSortDescending[0] = false;
	m_roomSortDescending[1] = false;
	m_roomSortDescending[2] = false;
	m_roomSortDescending[3] = false;

	switch (sort) {
	default:
		if (m_cateVSList) {
			m_cateVSList->SetVisible(false);
		}
		if (m_cateMassList) {
			m_cateMassList->SetVisible(false);
		}
		if (m_cateBattleList) {
			m_cateBattleList->SetVisible(false);
		}
		break;

	case 1:
		if (m_cateVSList) {
			m_cateVSList->SetVisible(false);
		}
		if (m_cateMassList) {
			m_cateMassList->SetVisible(false);
		}
		if (m_cateBattleList) {
			m_cateBattleList->SetVisible(false);
		}
		break;
	}
}

// lobbymain.cpp:12682
// Clears the sort orders once their reset timers run out and, in the room
// list, opens a category tab's list after the tab is held for 0.8 seconds;
// releasing the button picks from the open list.
void CLobbyMain::ResetControlsRoomListSort(float dt) {
	if (m_roomSortResetTime > 0.0f) {
		m_roomSortResetTime -= dt;
		if (m_roomSortResetTime <= 0.0f) {
			m_roomSortDescending[0] = false;
			m_roomSortDescending[1] = false;
			m_roomSortDescending[2] = false;
			m_roomSortDescending[3] = false;
		}
	}

	if (m_userSortResetTime > 0.0f) {
		m_userSortResetTime -= dt;
		if (m_userSortResetTime <= 0.0f) {
			m_userSortDescending[0] = false;
			m_userSortDescending[1] = false;
			m_userSortDescending[2] = false;
			m_userSortDescending[3] = false;
		}
	}

	if (strcmpi(g_pFresh->GetManager()->GetLayoutID(), "ROOMLIST") != 0)
		return;

	if (g_input->GetButton(LEFT_BUTTON) == 3) {
		WVector mouse = g_input->GetMousePoint(), down = g_input->GetButtonDownPos(LEFT_BUTTON);

		WRect vsRect = m_cateVSBtn->GetRect(), vsListRect = m_cateVSList->GetRect();
		WRect massRect = m_cateMassBtn->GetRect(), massListRect = m_cateMassList->GetRect();
		WRect battleRect = m_cateBattleBtn->GetRect(), battleListRect = m_cateBattleList->GetRect();

		if (m_cateVSBtn->GetStatus() == 2 && vsRect.IsInRect(WPoint(down.x, down.y))
			&& (vsRect.IsInRect(WPoint(mouse.x, mouse.y)) || vsListRect.IsInRect(WPoint(mouse.x, mouse.y)))) {
			m_cateNewRoom[0] += dt;

			if (m_cateNewRoom[0] > 0.8f)
				m_cateVSList->SetVisible(true);

		} else {
			m_cateNewRoom[0] = 0.0f;
			m_cateVSList->SetVisible(false);
		}

		if (m_cateMassBtn->GetStatus() == 2 && massRect.IsInRect(WPoint(down.x, down.y))
			&& (massRect.IsInRect(WPoint(mouse.x, mouse.y)) || massListRect.IsInRect(WPoint(mouse.x, mouse.y)))) {
			m_cateNewRoom[1] += dt;

			if (m_cateNewRoom[1] > 0.8f)
				m_cateMassList->SetVisible(true);

		} else {
			m_cateNewRoom[1] = 0.0f;
			m_cateMassList->SetVisible(false);
		}

		if (m_cateBattleBtn->GetStatus() == 2 && battleRect.IsInRect(WPoint(down.x, down.y))
			&& (battleRect.IsInRect(WPoint(mouse.x, mouse.y)) || battleListRect.IsInRect(WPoint(mouse.x, mouse.y)))) {
			m_cateNewRoom[2] += dt;

			if (m_cateNewRoom[2] > 0.8f)
				m_cateBattleList->SetVisible(true);

		} else {
			m_cateNewRoom[2] = 0.0f;
			m_cateBattleList->SetVisible(false);
		}
	} else {
		if (m_cateVSList->IsVisible())
			OnRoomList_RoomSortCateVS_ListLUp();
		if (m_cateMassList->IsVisible())
			OnRoomList_RoomSortCateMass_ListLUp();
		if (m_cateBattleList->IsVisible())
			OnRoomList_RoomSortCateBattle_ListLUp();

		m_cateNewRoom[0] = 0.0f;
		m_cateNewRoom[1] = 0.0f;
		m_cateNewRoom[2] = 0.0f;
	}
}

// lobbymain.cpp:12791
void CLobbyMain::OnChatBgInit(intptr_t param) {
	m_chatBg = DYNAMIC_CAST(FrArea, param);
}

// lobbymain.cpp:12796
void CLobbyMain::OnChatBgOwnerDraw(PG_MSGARG) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI) {
		pGDI->Box(m_chatBg->GetRect(), 0x58000000, 0, 0.0f);
	}
}

// lobbymain.cpp:12805
void CLobbyMain::OnChatWndBtnInit(intptr_t param) {
	m_chatWndBtn = DYNAMIC_CAST(FrButton, param);
	m_chatWndBtn->SetPushDelay(0.0f);

	bool up = COption::Instance()->gGetChatWndUp() != 0;

	ToggleChatBtn(up);
}

// lobbymain.cpp:12815
void CLobbyMain::OnChatWndBtnUp(void) {
	COption::Instance()->gSetChatWndUp(COption::Instance()->gGetChatWndUp() == 0);
	ToggleChatBtn(true);
}

// lobbymain.cpp:12823
void CLobbyMain::ToggleChatBtn(bool resize) {
	if (COption::Instance()->gGetChatWndUp()) {
		m_chatWndBtn->SetButtonImg("chatwnd_down", FrButton::NORMAL);
		if (resize) {
			const WRect& bgRect = m_chatBg->GetRect();
			m_chatBg->SetRect(WRect(bgRect.x, bgRect.y - 200.0f, bgRect.w, bgRect.h + 200.0f));

			const WRect& viewRect = m_chatView->GetRect();
			m_chatView->SetRect(WRect(viewRect.x, viewRect.y - 200.0f, viewRect.w, viewRect.h + 200.0f));

			if (m_chatView->GetScrollBar()) {
				m_chatView->GetScrollBar()->ScrollToBottom();
			}
		}
	} else {
		m_chatWndBtn->SetButtonImg("chatwnd_up", FrButton::NORMAL);
		if (resize) {
			const WRect& bgRect = m_chatBg->GetRect();
			m_chatBg->SetRect(WRect(bgRect.x, bgRect.y + 200.0f, bgRect.w, bgRect.h - 200.0f));

			const WRect& viewRect = m_chatView->GetRect();
			m_chatView->SetRect(WRect(viewRect.x, viewRect.y + 200.0f, viewRect.w, viewRect.h - 200.0f));

			if (m_chatView->GetScrollBar()) {
				m_chatView->GetScrollBar()->ScrollToBottom();
			}
		}
	}
}

// lobbymain.cpp:12864
void CLobbyMain::ReLoadHoleItem(void) {
	if (m_holeCombo) {
		m_holeCombo->ClearAllListItem();
		m_holeCombo->AddTexItem("hole_front");
		if (CSharedDoc::Instance()->m_roomInfo.m_holeCount < 18) {
			m_holeCombo->AddTexItem("hole_back");
			m_holeCombo->AddTexItem("hole_random");
		}
		m_holeCombo->AddTexItem("hole_shuffle");
	}
}

// lobbymain.cpp:12881
void CLobbyMain::OnRoomList_ChatViewInit(intptr_t param) {
	m_chatView = DYNAMIC_CAST(FrEdit, param);
	m_chatView->HidePrivacy(true);
	CSharedDoc::Instance()->m_chatLineList.clear();
}

// lobbymain.cpp:12889
void CLobbyMain::OnLanguageInit(intptr_t param) {
	m_languageArea = DYNAMIC_CAST(FrArea, param);
	if (m_languageArea) {
		if (g_ime->IsAlphaNumericMode()) {
			m_languageArea->SetBgImg("chat_eng");
		} else {
			m_languageArea->SetBgImg("chat_kor");
		}
	}
}

// lobbymain.cpp:12908
void CLobbyMain::OnChatInputInit(intptr_t param) {
	m_chatEdit = DYNAMIC_CAST(FrEdit, param);
	if (m_chatEdit) {
		m_chatEdit->SetKeyFocus(true);
	}
}

// lobbymain.cpp:12925
bool CLobbyMain::OnChatInputEnterKey(intptr_t param) {
	const char* msg = (const char*)param;
	if (strlen(msg)) {
		if (!CSharedDoc::Instance()->UTIL_SendChatMessage(m_chatTargetCombo, msg, true)) {
			m_chatTargetCombo->SetLine(1, "\xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4", 0, false, 0); // "to everyone"
			this << MsgObject(0, 3, (intptr_t)"\xB1\xD3\xB8\xBB \xBB\xF3\xB4\xEB\xB0\xA1 \xC0\xDF\xB8\xF8\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9. \"\xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4\"\xB7\xCE \xBA\xAF\xB0\xE6\xC7\xD5\xB4\xCF\xB4\xD9.", 0xFFAF00A1, 0, 0, 0); // "Wrong whisper target. Changed to "to everyone"."
		}
	}

	return true;
}

// lobbymain.cpp:12940
void CLobbyMain::OnReportBtnUp(void) {
	m_reportDlg = CreateForm<FrReportDlg>(g_pFresh->GetManager(), this, "report", NULL);

	// CP949: "1. past chat is also logged on the server. 2. fake or repeated
	// reports are punished. 3. reports are handled within 48 hours."
	std::string text;
	text = "1. \xb4\xeb\xc8\xad\xc3\xa2\xc0\xc7 \xc1\xf6\xb3\xad \xb3\xbb\xbf\xeb\xb5\xb5 \xb8\xf0\xb5\xce \xbc\xad\xb9\xf6\xbf\xa1 \xb1\xe2\xb7\xcf\xb5\xcb\xb4\xcf\xb4\xd9.\n";
	text += "2. \xc1\xb6\xc0\xdb, \xc7\xe3\xc0\xa7, \xb9\xdd\xba\xb9\xbd\xc5\xb0\xed\xb4\xc2 \xc3\xb3\xb9\xfa\xc0\xbb \xb9\xde\xbd\xc0\xb4\xcf\xb4\xd9.\n";
	text += "3. \xbd\xc5\xb0\xed\xc7\xcf\xbd\xc5 \xb3\xbb\xbf\xeb\xc0\xba 48\xbd\xc3\xb0\xa3 \xc0\xcc\xb3\xbb\xbf\xa1 \xc3\xb3\xb8\xae\xb5\xcb\xb4\xcf\xb4\xd9.";

	m_reportDlg->SetMessage(text.c_str(), false);
	m_reportDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnReportDlgResult, 3);
}

// lobbymain.cpp:12953
void CLobbyMain::OnChatTargetInit(intptr_t param) {
	m_chatTargetCombo = DYNAMIC_CAST(FrComboBox, param);
	m_chatTargetCombo->HidePrivacy(true);
	CSharedDoc::Instance()->InitReservedChatList(m_chatTargetCombo,
		(eReservedChatPartner)(CSharedDoc::Instance()->m_roomInfo.m_gameType == 5 || CSharedDoc::Instance()->m_roomInfo.m_gameType == 6 ? 3 : 2));
}

// lobbymain.cpp:12960
void CLobbyMain::OnChatTargetBtnDown(PG_MSGARG) {
	m_chatEdit->SetKeyFocus(false);
}

// lobbymain.cpp:12965
bool CLobbyMain::OnChatTargetEnterKey(PG_MSGARG) {
	m_chatEdit->SetKeyFocus(true);
	return false;
}

// lobbymain.cpp:12971
void CLobbyMain::OnEmoticonInit(intptr_t param) {
	m_emoticonBtn = DYNAMIC_CAST(FrButton, param);
}

// lobbymain.cpp:12976
void CLobbyMain::OnEmoticonBtnUp(void) {
	if (m_emoticonBtn && m_emoticonDlg == NULL) {
		m_emoticonDlg = CreateForm<FrEmoticonDlg>(g_pFresh->GetManager(), this, "emoticon", 0);
		if (m_emoticonDlg == NULL) {
			return;
		}

		WPoint pt(0.0f, 0.0f);
		if (m_emoticonBtn) {
			const WRect& rect = m_emoticonBtn->GetRect();
			pt.x = rect.x - 200.0f;
			pt.y = rect.y - 260.0f;
		}

		m_emoticonDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnEmoticonResult, pt, 3);
	}
}

// lobbymain.cpp:12997
void CLobbyMain::OnChannelRoomCloseBtnInit(intptr_t param) {
	m_channelRoomCloseBtn = DYNAMIC_CAST(FrButton, param);
}

// lobbymain.cpp:13007
void CLobbyMain::OnGameRoom_Init(PG_MSGARG) {
	CTaskMain::OnInit();

	m_away = false;
	m_roomOptionA = false;
	m_roomOptionC = false;
	m_roomOptionD = false;
	m_autoStartArmed = false;

	if (CSharedDoc::Instance()->GetPangyaUserInfo().identity == 2) {
		m_roomOptionA = true;
		m_roomOptionC = true;
	}

	m_roomOptionB = false;
	m_selectedUserId = 0xFFFFFFFF;
	m_removeFriendUid = 0xFFFFFFFF;
	m_blinkTipArea = NULL;
	m_startTipArea = NULL;
	m_readyTipArea = NULL;

	g_mouse->ResetInputTime();
	g_ime->ResetInputTime();

	if (CSharedDoc::Instance()->m_roomInfo.m_roomMode == 14) {
		g_pFresh->GetDesktop()->SetWallPaper("chaos_background.jpg", true);
		(!CTaskManager::Instance() ? 0 : CTaskManager::Instance()->GetCurrentTask())->SendMsgToMainActor(MsgObject(0, 0x23,
			(intptr_t)"\xBD\xBA\xC6\xF3\xBC\xC8 \xBC\xC5\xC7\xC3 \xB8\xF0\xB5\xE5\xBF\xA1\xBC\xAD\xB4\xC2, \n\\c0xffff0000\\c\xB0\xD4\xC0\xD3 \xC1\xDF \xC0\xD4\xC0\xE5\\c0xff000000\\c \xB9\xD7 \\c0xffff0000\\c\xC6\xBC\xC5\xB0\xB8\xAE\xC6\xF7\xC6\xAE\\c0xff000000\\c \xBB\xE7\xBF\xEB\xC0\xCC \xBA\xD2\xB0\xA1\xB4\xC9 \xC7\xD5\xB4\xCF\xB4\xD9.",
			0, 0, 0, 0));
	} else {
		g_pFresh->GetDesktop()->SetWallPaper("gameroom_background.jpg", true);
	}

	SetTimeVariableBGM();
}

// lobbymain.cpp:13059
void CLobbyMain::OnGameRoom_Finish(void) {
	m_titleArea->SetBgImg("title_vs");
#ifdef JP_PAKS
	// The JP form draws its own title row where the over bar sits; the bar
	// stays for the controls this code keeps (docs/JP_CLIENT.md).
	if (m_barFrame[0] != 0) {
		m_barFrame[0]->SetVisible(false);
	}
#endif

	std::list<sSlotInfo>::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.end(); ++it) {
		if (m_roomUserListBox) {
			m_roomUserListBox->AddItem(&*it);
		}

		ShowPet(it->m_uid);
	}

	this << MsgObject(this, 0x11, WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course, 0, 0, 0, 0);

	if (WSingleton<CSharedDoc>::Instance()->m_tutorialFinish) {
		this << MsgObject(0, 0xAA, 1, 0, 0, 0, 0);
	}

	EnableUnderBar(false);

	OpenNewRecordForm();
}

// lobbymain.cpp:13082
void CLobbyMain::OnGameRoom_Destroy(void) {
	EnableUnderBar(true);
}

// lobbymain.cpp:13087
void CLobbyMain::OnGameRoom_BackGroundInit(intptr_t param) {
	m_backGroundViewer = DYNAMIC_CAST(FrViewer, param);
	if (m_backGroundViewer) {
		m_backGroundViewer->SetMouseEvent(false);
	}
}

// lobbymain.cpp:13094
void CLobbyMain::OnGameRoom_Pet0Init(intptr_t param) {
	m_petArea[0] = DYNAMIC_CAST(FrArea, param);
}

// lobbymain.cpp:13101
void CLobbyMain::OnGameRoom_Pet1Init(intptr_t param) {
	m_petArea[1] = DYNAMIC_CAST(FrArea, param);
}

// lobbymain.cpp:13108
void CLobbyMain::OnGameRoom_Pet2Init(intptr_t param) {
	m_petArea[2] = DYNAMIC_CAST(FrArea, param);
}

// lobbymain.cpp:13115
void CLobbyMain::OnGameRoom_Pet3Init(intptr_t param) {
	m_petArea[3] = DYNAMIC_CAST(FrArea, param);
}

// lobbymain.cpp:13122
void CLobbyMain::Banish(void) {
	if (m_selectedUserId != (unsigned long)-1) {
		WSendPacket packet(GCT_REQUEST_BANISH);
		packet.Encode4(m_removeFriendUid);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// lobbymain.cpp:13132
// The room's player list: right button and dummy rows on, and its bitmaps.
void CLobbyMain::OnGameRoom_RoomUserInit(PG_MSGARG param) {
	m_roomUserListBox = (FrListBox*)((__rtti_obj = (void*)param) != 0 ? (((IObject*)__rtti_obj)->IsKindOf(&FrListBox::m_RTTI) ? (void*)param : 0) : 0);
	if (m_roomUserListBox == 0)
		return;

	m_roomUserListBox->UseRightButton(true);
	m_roomUserListBox->UseDummy(true);

	m_teamBmp[0] = m_roomUserListBox->GetBitmap("team_red");
	m_teamBmp[1] = m_roomUserListBox->GetBitmap("team_blue");
	m_teamBaseBmp[0] = m_roomUserListBox->GetBitmap("team_base");
	m_teamBaseBmp[1] = m_roomUserListBox->GetBitmap("team_base02");
	m_inviteBmp = m_roomUserListBox->GetBitmap("invite");

	m_roomUserImage[0] = m_roomUserListBox->GetBitmap("master_sign");
	m_roomUserImage[1] = m_roomUserListBox->GetBitmap("ready_sign");
	m_roomUserMarkImage[0] = m_roomUserListBox->GetBitmap("team_select");

	m_genderBmp[0][0] = m_roomUserListBox->GetBitmap("i_male");
	m_genderBmp[0][1] = m_roomUserListBox->GetBitmap("i_female");
	m_genderBmp[1][0] = m_roomUserListBox->GetBitmap("i_male_02");
	m_genderBmp[1][1] = m_roomUserListBox->GetBitmap("i_female_02");
	m_genderBmp[2][0] = m_roomUserListBox->GetBitmap("i_male_03");
	m_genderBmp[2][1] = m_roomUserListBox->GetBitmap("i_female_03");

	m_genderMannerBmp[0] = m_roomUserListBox->GetBitmap("i_male_manner");
	m_genderMannerBmp[1] = m_roomUserListBox->GetBitmap("i_female_manner");

	m_genderAngelBmp[0] = m_roomUserListBox->GetBitmap("i_male_angel");
	m_genderAngelBmp[1] = m_roomUserListBox->GetBitmap("i_female_angel");

	m_roomUserMarkImage[1] = m_roomUserListBox->GetBitmap("dive");

	m_roomUserSelImage = g_pFresh->RegisterBitmap("kick");
}

// lobbymain.cpp:13190
// Draws one player card of the game room: an empty slot (team base 2), the
// invitation mark, the slot's pet (created on first draw), the card base
// (team colour in team games), the kick button for the master, the selection
// mark, the master / ready sign, the dive mark, the gender icon, the level
// (ladder or title) icon, the guild emblem and the nick. Two layouts, picked
// by local content 0x54, differ in the level icon and the invitation rules.
void CLobbyMain::OnGameRoom_RoomUserOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	if (!pItem->pData) {
		pGDI->DrawTexture(m_teamBaseBmp[1], WRect(pItem->pos.x, pItem->pos.y, (float)m_teamBaseBmp[1]->Width(), (float)m_teamBaseBmp[1]->Height()), 0xFFFFFFFF, 0);
		return;
	}

	if (IsLocalContent((localContentType_t)0x54)) {
		if (((sSlotInfo*)pItem->pData)->m_flags151.flag)
			pGDI->DrawTexture(m_inviteBmp, WRect(pItem->pos.x, pItem->pos.y, (float)m_inviteBmp->Width(), (float)m_inviteBmp->Height()), 0xFFFFFFFF, 0);
	}

	sSlotInfo* pSlot = (sSlotInfo*)pItem->pData;
	if (!pSlot)
		return;

	if (CSharedDoc::Instance()->m_roomSlotMap[pSlot->m_uid].m_pPet == NULL) {
		CSharedDoc::Instance()->m_roomSlotMap[pSlot->m_uid].m_pPetArea = m_petArea[pSlot->m_connection - 1];
		CSharedDoc::Instance()->m_roomSlotMap[pSlot->m_uid].m_pPet = NULL;
		CSharedDoc::Instance()->m_roomSlotMap[pSlot->m_uid].m_angelFlags.flag0 = pSlot->m_flags57.flag0;
		CSharedDoc::Instance()->m_roomSlotMap[pSlot->m_uid].m_angelFlags.flag1 = pSlot->m_flags57.flag1;

		if (IsLocalContent((localContentType_t)0x54)) {
			if (!pSlot->m_flags151.flag)
				ShowPet(pSlot->m_uid);
		} else {
			ShowPet(pSlot->m_uid);
		}
	}

	if (IsLocalContent((localContentType_t)0x54)) {
		WRect rect(pItem->pos.x, pItem->pos.y, (float)m_teamBaseBmp[0]->Width(), (float)m_teamBaseBmp[0]->Height());

		if (!pSlot->m_flags151.flag) {
			if (!m_bongdariShop[2]) {
				std::map<unsigned long, sRoomSlot>::iterator it = CSharedDoc::Instance()->m_roomSlotMap.find(pSlot->m_uid);
				if (it != CSharedDoc::Instance()->m_roomSlotMap.end()) {
					if (it->second.m_pPet)
						it->second.m_pPet->Display(0.0f, 0.0f);
				}
			}

			if (CSharedDoc::Instance()->m_roomInfo.m_gameType == 1)
				pGDI->DrawTexture(m_teamBmp[pSlot->m_attribute & 3], rect, 0xFFFFFFFF, 0);
			else
				pGDI->DrawTexture(m_teamBaseBmp[0], rect, 0xFFFFFFFF, 0);
		} else {
			pGDI->DrawTexture(m_inviteBmp, rect, 0xFFFFFFFF, 0);
		}

		if (pSlot->m_roomCardSkinTypeId) {
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pSlot->m_roomCardSkinTypeId);

			if (pSkin) {
				const Bitmap* pCard = g_pFresh->GetBitmap(pSkin->m_cutinImage);
				pGDI->DrawTexture(pCard, rect, 0xFFFFFFFF, 0);
			}
		}

		if (CSharedDoc::Instance()->m_userInfo.guid == m_roomMasterUid && !(pSlot->m_attribute & 0x18)) {
			if (!(CSharedDoc::Instance()->m_curServer.flags & 4))
				pGDI->DrawTexture(m_roomUserSelImage, WRect(rect.x + rect.w - 8.0f - (float)m_roomUserSelImage->Width(), rect.y + 8.0f, (float)m_roomUserSelImage->Width(), (float)m_roomUserSelImage->Height()), 0xFFFFFFFF, 0);
		}

		if (pSlot->m_uid == m_selectedUserId)
			pGDI->DrawTexture(m_roomUserMarkImage[0], rect, 0xFFFFFFFF, 0);

		int x = (int)pItem->pos.x + (m_teamBmp[pSlot->m_attribute & 3]->Width() - m_roomUserImage[1]->Width()) / 2;

		if (pSlot->m_attribute & 0x18) {
			rect = WRect((float)x, pItem->pos.y + 6.0f, (float)m_roomUserImage[0]->Width(), (float)m_roomUserImage[0]->Height());
			pGDI->DrawTexture(m_roomUserImage[0], rect, 0xFFFFFFFF, 0);
		} else if (pSlot->m_flags55.flag1) {
			rect = WRect((float)x, pItem->pos.y + 6.0f, (float)m_roomUserImage[1]->Width(), (float)m_roomUserImage[1]->Height());
			pGDI->DrawTexture(m_roomUserImage[1], rect, 0xFFFFFFFF, 0);
		}

		const Bitmap* pIcon = NULL;
		if (pSlot->m_iconSkinTypeId == 0) {
			IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(pSlot->m_characterTypeId);

			if (pChar)
				pIcon = g_pFresh->GetBitmap(pChar->m_icon);
		} else {
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pSlot->m_iconSkinTypeId);

			if (pSkin)
				pIcon = g_pFresh->GetBitmap(pSkin->m_cutinImage);
		}

		if (pIcon) {
			rect.x = pItem->pos.x + 8.0f;
			rect.y = pItem->pos.y + 36.0f;
			rect.w = (float)pIcon->Width();
			rect.h = (float)pIcon->Height();

			pGDI->DrawTexture(pIcon, rect, 0xFFFFFFFF, 0);
		}

		if (pSlot->m_attribute & 4)
			pGDI->DrawTexture(m_roomUserMarkImage[1], WRect(pItem->pos.x + 6.0f, pItem->pos.y + 100.0f, (float)m_roomUserMarkImage[1]->Width(), (float)m_roomUserMarkImage[1]->Height()), 0xFFFFFFFF, 0);

		const Bitmap* pGender;
		if (pSlot->m_flags57.flag0)
			pGender = m_genderAngelBmp[pSlot->m_attributeBits.userFlag % 2];
		else if (pSlot->m_flags55.flag0)
			pGender = m_genderMannerBmp[pSlot->m_attributeBits.userFlag % 2];
		else
			pGender = m_genderBmp[0][pSlot->m_attributeBits.userFlag];

		if (pGender)
			pGDI->DrawTexture(pGender, WRect(pItem->pos.x + 2.0f, pItem->pos.y + 98.0f, (float)pGender->Width(), (float)pGender->Height()), 0xFFFFFFFF, 0);

		const Bitmap* pLevel = NULL;
		if (CSharedDoc::Instance()->m_currentChannel.flags & 0x80) {
			pLevel = g_pFresh->GetBitmap(MakeStr("ladder_%03d", pSlot->m_ladderGrade));
		} else if (pSlot->m_titleTypeId) {
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pSlot->m_titleTypeId);
			if (pSkin)
				pLevel = g_pFresh->GetBitmap(pSkin->m_icon);
		} else if (!((sSlotInfo*)pItem->pData)->m_flags151.flag) {
			pLevel = g_pFresh->GetBitmap(MakeStr("level_%03d", pSlot->m_level + 1));
		}

		if (pLevel)
			pGDI->DrawTexture(pLevel, WRect(pItem->pos.x + 90.0f - float2int(pLevel->Width() * 0.5f), pItem->pos.y + 68.0f, (float)pLevel->Width(), (float)pLevel->Height()), 0xFFFFFFFF, 0);

		if (pSlot->m_guildUID && pSlot->m_flags151.flag) {
			const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pSlot->m_emblemName);

			if (pEmblem)
				pGDI->DrawTexture(pEmblem, WRect(pItem->pos.x + 60.0f, pItem->pos.y + 44.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
		}

		if (WSingleton<CProjectG>::Instance()->HidePrivacy())
			return;

		if (pItem->underCursor) {
			pGDI->SetTextColor(0xFFFFFFFF, 0xFF808080);
			pGDI->SetTextStyle(2);
		} else {
			if (pSlot->m_uid == MyGuid(true))
				pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
			else
				pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
			pGDI->SetTextStyle(0);
		}

		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 20.0f, pItem->pos.y + 107.0f), 0, pSlot->m_nickname, -1.0f, 0xFFFFFFFF);
	} else {
		if (!m_bongdariShop[2]) {
			std::map<unsigned long, sRoomSlot>::iterator it = CSharedDoc::Instance()->m_roomSlotMap.find(pSlot->m_uid);
			if (it != CSharedDoc::Instance()->m_roomSlotMap.end()) {
				if (it->second.m_pPet)
					it->second.m_pPet->Display(0.0f, 0.0f);
			}
		}

		WRect rect(pItem->pos.x, pItem->pos.y, (float)m_teamBaseBmp[0]->Width(), (float)m_teamBaseBmp[0]->Height());

		if (CSharedDoc::Instance()->m_roomInfo.m_gameType == 1)
			pGDI->DrawTexture(m_teamBmp[pSlot->m_attribute & 3], rect, 0xFFFFFFFF, 0);
		else
			pGDI->DrawTexture(m_teamBaseBmp[0], rect, 0xFFFFFFFF, 0);

		if (pSlot->m_roomCardSkinTypeId) {
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pSlot->m_roomCardSkinTypeId);

			if (pSkin) {
				const Bitmap* pCard = g_pFresh->GetBitmap(pSkin->m_cutinImage);
				pGDI->DrawTexture(pCard, rect, 0xFFFFFFFF, 0);
			}
		}

		if (CSharedDoc::Instance()->m_userInfo.guid == m_roomMasterUid && !(pSlot->m_attribute & 0x18)) {
			if (!(CSharedDoc::Instance()->m_curServer.flags & 4))
				pGDI->DrawTexture(m_roomUserSelImage, WRect(rect.x + rect.w - 8.0f - (float)m_roomUserSelImage->Width(), rect.y + 8.0f, (float)m_roomUserSelImage->Width(), (float)m_roomUserSelImage->Height()), 0xFFFFFFFF, 0);
		}

		if (pSlot->m_uid == m_selectedUserId)
			pGDI->DrawTexture(m_roomUserMarkImage[0], rect, 0xFFFFFFFF, 0);

		int x = (int)pItem->pos.x + (m_teamBmp[pSlot->m_attribute & 3]->Width() - m_roomUserImage[1]->Width()) / 2;

		if (pSlot->m_attribute & 0x18) {
			rect = WRect((float)x, pItem->pos.y + 6.0f, (float)m_roomUserImage[0]->Width(), (float)m_roomUserImage[0]->Height());
			pGDI->DrawTexture(m_roomUserImage[0], rect, 0xFFFFFFFF, 0);
		} else if (pSlot->m_flags55.flag1) {
			rect = WRect((float)x, pItem->pos.y + 6.0f, (float)m_roomUserImage[1]->Width(), (float)m_roomUserImage[1]->Height());
			pGDI->DrawTexture(m_roomUserImage[1], rect, 0xFFFFFFFF, 0);
		}

		const Bitmap* pIcon = NULL;
		if (pSlot->m_iconSkinTypeId == 0) {
			IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(pSlot->m_characterTypeId);

			if (pChar)
				pIcon = g_pFresh->GetBitmap(pChar->m_icon);
		} else {
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pSlot->m_iconSkinTypeId);

			if (pSkin)
				pIcon = g_pFresh->GetBitmap(pSkin->m_cutinImage);
		}

		if (pIcon) {
			rect.x = pItem->pos.x + 8.0f;
			rect.y = pItem->pos.y + 36.0f;
			rect.w = (float)pIcon->Width();
			rect.h = (float)pIcon->Height();

			pGDI->DrawTexture(pIcon, rect, 0xFFFFFFFF, 0);
		}

		if (pSlot->m_attribute & 4)
			pGDI->DrawTexture(m_roomUserMarkImage[1], WRect(pItem->pos.x + 6.0f, pItem->pos.y + 100.0f, (float)m_roomUserMarkImage[1]->Width(), (float)m_roomUserMarkImage[1]->Height()), 0xFFFFFFFF, 0);

		const Bitmap* pGender;
		if (pSlot->m_flags57.flag0)
			pGender = m_genderAngelBmp[pSlot->m_attributeBits.userFlag % 2];
		else if (pSlot->m_flags55.flag0)
			pGender = m_genderMannerBmp[pSlot->m_attributeBits.userFlag % 2];
		else
			pGender = m_genderBmp[0][pSlot->m_attributeBits.userFlag];

		if (pGender)
			pGDI->DrawTexture(pGender, WRect(pItem->pos.x + 2.0f, pItem->pos.y + 98.0f, (float)pGender->Width(), (float)pGender->Height()), 0xFFFFFFFF, 0);

		const Bitmap* pLevel;
		if (CSharedDoc::Instance()->m_currentChannel.flags & 0x80) {
			pLevel = g_pFresh->GetBitmap(MakeStr("ladder_%03d", pSlot->m_ladderGrade));
		} else if (pSlot->m_titleTypeId) {
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pSlot->m_titleTypeId);
			if (pSkin)
				pLevel = g_pFresh->GetBitmap(pSkin->m_icon);
			else
				pLevel = g_pFresh->GetBitmap(MakeStr("level_%03d", pSlot->m_level + 1));
		} else {
			pLevel = g_pFresh->GetBitmap(MakeStr("level_%03d", pSlot->m_level + 1));
		}

		if (pLevel)
			pGDI->DrawTexture(pLevel, WRect(pItem->pos.x + 60.0f, pItem->pos.y + 68.0f, (float)pLevel->Width(), (float)pLevel->Height()), 0xFFFFFFFF, 0);

		if (pSlot->m_guildUID) {
			const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pSlot->m_emblemName);

			if (pEmblem)
				pGDI->DrawTexture(pEmblem, WRect(pItem->pos.x + 60.0f, pItem->pos.y + 44.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
		}

		if (WSingleton<CProjectG>::Instance()->HidePrivacy())
			return;

		if (pItem->underCursor) {
			pGDI->SetTextColor(0xFFFFFFFF, 0xFF808080);
			pGDI->SetTextStyle(2);
		} else {
			if (pSlot->m_uid == MyGuid(true))
				pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
			else
				pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
			pGDI->SetTextStyle(0);
		}

		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 20.0f, pItem->pos.y + 107.0f), 0, pSlot->m_nickname, -1.0f, 0xFFFFFFFF);
	}
}

// lobbymain.cpp:13666
void CLobbyMain::OnGameRoom_RoomUserLBtnUp(void) {
	if (m_roomUserListBox) {
		FrListItem* pItem = m_roomUserListBox->GetItemUnderCursor();
		if (pItem) {
			sSlotInfo* pSlot = (sSlotInfo*)pItem->pData;
			if (pSlot) {
				if (pSlot->m_uid != m_selectedUserId) {
					m_selectedUserId = pSlot->m_uid;
					m_removeFriendUid = pSlot->m_ruleUID;
				}

				// lobbymain.cpp:13689, the kick button's rect in the row
				WRect rect;
				if (!(WSingleton<CSharedDoc>::Instance()->m_curServer.flags & 4)) {
					if (strcmp(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM") == 0) {
						rect = WRect((float)m_teamBmp[pSlot->m_attribute & 3]->Width() - 8.0f - (float)m_roomUserSelImage->Width(), 8.0f,
							(float)m_roomUserSelImage->Width(), (float)m_roomUserSelImage->Height());
					} else {
						rect = WRect(227.0f, 5.0f, (float)m_roomUserSelImage->Width(), (float)m_roomUserSelImage->Height());
					}
				}

				if (strcmp(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXTRES") != 0) {
					CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
					if (m_roomMasterUid == pDoc->m_userInfo.guid
						&& m_selectedUserId != pDoc->m_userInfo.guid
						&& (rect + pItem->pos).IsInRect(g_pFresh->GetManager()->GetMousePos())
						&& pDoc->m_roomInfo.m_gameType != 6) {
						if (IsLocalContent((localContentType_t)0x54)) {
							if (!(pSlot->m_banishFlags & 1))
								Banish();
						} else {
							Banish();
						}
					}
				}
			}
		}
	}
}

// lobbymain.cpp:13737
void CLobbyMain::OnGameRoom_RoomUserRBtnUp(void) {
	OnGameRoom_RoomUserLBtnUp();

	if (WSingleton<CUserInfo>::Instance()) {
		std::map<unsigned long, sBriefUserInfo>::iterator it = CSharedDoc::Instance()->m_userMap.find(m_selectedUserId);
		if (it != CSharedDoc::Instance()->m_userMap.end()) {
			it->second.roomNumber = CSharedDoc::Instance()->m_roomInfo.m_roomNumber;
		}
		WSingleton<CUserInfo>::Instance()->SetInfo(it->second.id, m_selectedUserId, true, true, false, false, it->second.nick);
	}
}

// lobbymain.cpp:13762
void CLobbyMain::OnGameRoom_RoomUserDClick(void) {
	if (GalleryMode()) {
		if (CSharedDoc::Instance()->GetPangyaUserInfo().galleryGuid != m_selectedUserId) {
			WSendPacket packet(GCT_CHANGE_TARGET);
			packet.Encode4(m_removeFriendUid);
			packet.Send(SEND_TO_DEFAULT);
		}
	}
}

// lobbymain.cpp:13775
void CLobbyMain::OnGameRoom_TitleInit(intptr_t param) {
	m_roomTitleEdit = DYNAMIC_CAST(FrEdit, param);
	HandleMsg(MsgObject(this, 0x10, 0, 0, 0, 0, 0));

	WSingleton<CSharedDoc>::Instance()->SortAllMyItemList();

	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_userLimit >= 100) {
		m_roomTitleEdit->SetFontColor(0xFF47A5DD);
	} else {
		m_roomTitleEdit->SetFontColor(0xFFFFFFFF);
	}
}

// lobbymain.cpp:13788
void CLobbyMain::OnGameRoom_EventOwnerDraw(PG_MSGARG) {
	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_userLimit >= 100) {
		FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

		if (pGDI) {
			if (m_eventImage == 0) {
				m_eventImage = g_pFresh->GetBitmap("room_event02");
			}

			if (m_eventImage) {
				pGDI->DrawTexture(m_eventImage, WRect(8.0f, 45.0f, 43.0f, 32.0f), 0xFFFFFFFF, 0);
			}
		}
	}
}

// lobbymain.cpp:13804
void CLobbyMain::OnGameRoom_MapBgInit(intptr_t param) {
	m_mapBgViewer = DYNAMIC_CAST(FrViewer, param);
	if (m_mapBgViewer) {
		m_mapBgViewer->SetMouseEvent(false);
	}
}

// lobbymain.cpp:13811
// The game room's treasure hunter gauge of the room's course (0-300 past
// 700 points); empty for a random course.
void CLobbyMain::OnGameRoom_MapGaugeInit(PG_MSGARG param) {
	m_mapGauge = (FrGaugeBarImage*)((__rtti_obj = (void*)param) != 0 ? (((IObject*)__rtti_obj)->IsKindOf(&FrGaugeBarImage::m_RTTI) ? (void*)param : 0) : 0);
	if (m_mapGauge) {
		m_mapGauge->SetRange(0, 300, 0);
		unsigned char course = WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course;
		if (course < 0x7F) {
			unsigned long gauge = CTHunter::Instance()->GetTHunterGauge((eMapType)course);
			m_mapGauge->SetDestPos(gauge < 701 ? 0 : gauge - 700, false);
		} else {
			m_mapGauge->SetPos(0);
		}
	}
}

// lobbymain.cpp:13830
// Sends the item picked in the selection bar (packet 0x0C): m_subSelection is
// its type id, whose group (the top six bits) picks the change kind.
void CLobbyMain::SendChangedUserInfo(void) {
	if (m_subSelection == 0)
		return;

	switch ((unsigned long)m_subSelection >> 26) {
	case 1: {
		WSendPacket packet((enumClientPacket)0x0C);
		packet.Encode1(4);
		packet.Encode4(m_subSelectionTab);
		packet.Send(SEND_TO_DEFAULT);
		break;
	}
	case 7: {
		WSendPacket packet((enumClientPacket)0x0C);
		packet.Encode1(1);
		packet.Encode4(m_subSelectionTab);
		packet.Send(SEND_TO_DEFAULT);
		break;
	}
	case 4: {
		WSendPacket packet((enumClientPacket)0x0C);
		packet.Encode1(3);
		packet.Encode4(m_subSelectionTab);
		packet.Send(SEND_TO_DEFAULT);
		break;
	}
	case 5: {
		WSendPacket packet((enumClientPacket)0x0C);
		packet.Encode1(2);
		packet.Encode4(m_subSelection);
		packet.Send(SEND_TO_DEFAULT);
		break;
	}
	case 16: {
		WSendPacket packet((enumClientPacket)0x0C);
		packet.Encode1(5);
		packet.Encode4(m_subSelectionTab);
		packet.Send(SEND_TO_DEFAULT);
		break;
	}
	}
}

// lobbymain.cpp:13880
void CLobbyMain::InitControlsSelectedUserInfo(void) {
	m_subSelection = 0;
	m_subSelectionTab = 0;

	if (m_charSelViewer) m_charSelViewer->SetVisible(false);
	if (m_caddieSelViewer) m_caddieSelViewer->SetVisible(false);
	if (m_aztecSelViewer) m_aztecSelViewer->SetVisible(false);
	if (m_clubSelViewer) m_clubSelViewer->SetVisible(false);
	if (m_mascotSelViewer) m_mascotSelViewer->SetVisible(false);
}

// lobbymain.cpp:13893
// Frames the selected user-info slot while the mouse is over it.
void CLobbyMain::DrawFrameSelectedUserInfo(WRect rect) {
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == 0)
		return;

	if (rect.IsInRect(g_input->GetMousePoint().x, g_input->GetMousePoint().y)) {
		const Bitmap* pBmp = g_pFresh->GetBitmap("gameroom_select_frame");
		if (pBmp) {
			pGI->DrawTexture(pBmp, WRect(rect.x - 2.0f, rect.y - 2.0f, (float)pBmp->Width(), (float)pBmp->Height()), 0xFFFFFFFF, 0);
		}
	}
}

// lobbymain.cpp:13908
// Opens the new record form, then the holes event form when the record count
// changed, then the Halloween gift dialog when the event asks for it.
void CLobbyMain::OpenNewRecordForm(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_value4D7E != 0xFF && m_newRecordDlg == 0) {
		m_newRecordDlg = CreateForm<FrNewRecordDlg>(g_pFresh->GetManager(), this, "new_record", NULL);
		m_newRecordDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnNewRecordDlgResult, 0);
		WSingleton<CSharedDoc>::Instance()->m_value4D7E = 0xFF;
	}

	if (IsLocalContent((localContentType_t)0x18)) {
		if (WSingleton<CSharedDoc>::Instance()->m_value4D88 == -1)
			WSingleton<CSharedDoc>::Instance()->m_value4D88 = WSingleton<CSharedDoc>::Instance()->m_myStatRecordCount;

		if (WSingleton<CSharedDoc>::Instance()->m_value4D88 != WSingleton<CSharedDoc>::Instance()->m_myStatRecordCount) {
			if (IsLocalContent((localContentType_t)0x48))
				m_holesEventDlg = CreateForm<ntHolesEventDlg>(g_pFresh->GetManager(), this, "holes_EntranceExam", NULL);
			else
				m_holesEventDlg = CreateForm<ntHolesEventDlg>(g_pFresh->GetManager(), this, "holes_event", NULL);

			m_holesEventDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnHolesEventDlgResult, 0x11);
			WSingleton<CSharedDoc>::Instance()->m_value4D88 = WSingleton<CSharedDoc>::Instance()->m_myStatRecordCount;
		}
	}

	if (IsLocalContent((localContentType_t)0x21)) {
		CHalloweenEvent* pEvent = (CHalloweenEvent*)WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0x21);
		if (pEvent) {
			if (pEvent->GetShowGiftDlg()) {
				if (m_halloweenEvent2Dlg == 0) {
					m_halloweenEvent2Dlg = CreateForm<FrHalloweenEventGiftDlg>(g_pFresh->GetManager(), this, "halloween2007_event_gift_dlg", NULL);
					if (m_halloweenEvent2Dlg) {
						m_halloweenEvent2Dlg->Open((FRESH_RESULT_CALLBACK)0, 0x11);
						m_halloweenEvent2Dlg = 0;
					}
				}

				pEvent->SetShowGiftDlg(false);
			}
		}
	}
}

// lobbymain.cpp:13957
void CLobbyMain::GameExtResQuit(void) {
	if (!IsLocalContent((localContentType_t)0x14)) {
		m_useReportEnabled = false;
	}

	if (m_resultReportDlg == 0) {
		if (m_useReportEnabled && CSharedDoc::Instance()->m_roomInfo.m_roomMode != 14) {
			OpenUseReportForm();
		} else {
			OpenGameExtResQuitForm();
		}
	}
}

// lobbymain.cpp:14014
void CLobbyMain::OpenUseReportForm(void) {
	if (m_roomUseReportDlg == 0) {
		m_roomUseReportDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", NULL);
		m_roomUseReportDlg->SetMessage("\xC6\xBC\xC5\xB0 \xB8\xAE\xC6\xF7\xC6\xAE \xBF\xEB\xC1\xF6\xB8\xA6 \xBB\xE7\xBF\xEB\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);
		m_roomUseReportDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnRoomUseReportDlgResult, 1);
	}
}

// lobbymain.cpp:14027
void CLobbyMain::OpenGameExtResQuitForm(void) {
	if (m_roomExtResQuitDlg == 0) {
		m_roomExtResQuitDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", NULL);

		m_roomExtResQuitDlg->SetMessage("\xB0\xD4\xC0\xD3\xC0\xCC \\c0xffff0000\\c\xBF\xCF\xC0\xFC\xC8\xF7 \xC1\xBE\xB7\xE1\\c0xff000000\\c\xB5\xC7\xB1\xE2 \xC0\xFC\xBF\xA1 \xB0\xD4\xC0\xD3\xC0\xBB \xC6\xF7\xB1\xE2\xC7\xCF\xB8\xE9, \\c0xffff0000\\c\xB0\xE6\xC7\xE8\xC4\xA1 \xC8\xB9\xB5\xE6\xB0\xFA \xB0\xA2\xC1\xBE \xB1\xE2\xB7\xCF\xC0\xCC \xB9\xAB\xC8\xBF\xC3\xB3\xB8\xAE\\c0xff000000\\c\xB5\xC7\xC1\xF6\xB8\xB8 \\c0xffff0000\\c\xB0\xAD\xC1\xA6\xC1\xBE\xB7\xE1\xC0\xB2\xC0\xBA \xBF\xC3\xB6\xF3\xB0\xA1\xC1\xF6 \xBE\xCA\xBD\xC0\xB4\xCF\xB4\xD9.\\c0xff000000\\c\n\n\xB0\xD4\xC0\xD3\xC0\xBB \xC6\xF7\xB1\xE2\xC7\xCF\xB0\xED \xB7\xCE\xBA\xF1\xB7\xCE \xB3\xAA\xB0\xA1\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);

		m_roomExtResQuitDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnRoomExtResQuitDlgResult, 1);
	}
}

// lobbymain.cpp:14041
void CLobbyMain::OnGameRoom_CharInit(intptr_t param) {
	m_charViewer = DYNAMIC_CAST(FrViewer, param);
}

// lobbymain.cpp:14046
void CLobbyMain::OnGameRoom_CharDown(void) {
	WRect rect = m_charSelViewer->GetRect();
	WPoint pt(rect.x, rect.y - 5.0f);
	OpenSubEquipBar(pt, (PANGYA_ITEM_GROUP)1, 1);

	if (m_subEquipBar) {
		WRect barRect = m_subEquipBar->GetRect();

		barRect.x = rect.x - barRect.w * 0.5f + 0.5f;
		if (barRect.x + barRect.w > g_view->GetWidth()) {
			barRect.x = g_view->GetWidth() - barRect.w;
		}
		m_subEquipBar->SetRect(barRect);
	}
}

// lobbymain.cpp:14067
// Draws the game room's character button (greyed while disabled): the
// equipped character's icon, or "w_caddie_no" without one.
void CLobbyMain::OnGameRoom_CharOwnerDraw(PG_MSGARG) {
	if (!m_charViewer)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (!pGDI)
		return;

	unsigned long color = m_charViewer->IsEnabled() ? 0xFFFFFFFF : 0x50B0B0B0;

	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if (doc->m_myCharacterId == 0) {
		const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "w_caddie_no");

		if (pBmp) {
			pGDI->DrawTexture(pBmp, m_charViewer->GetRect(), color, 0);
		}
	} else {
		std::map<unsigned int, sCharacterInfo>::iterator it = doc->m_characterMap.find(doc->m_myCharacterId);
		IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.typeId);

		if (pItem) {
			const Bitmap* pBmp = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->m_icon));

			if (pBmp) {
				pGDI->DrawTexture(pBmp, WRect(m_charViewer->GetRect().x, m_charViewer->GetRect().y, (float)pBmp->Width(), (float)pBmp->Height()), color, 0);
			}
		}
	}

	if (m_charViewer->IsEnabled())
		DrawFrameSelectedUserInfo(m_charViewer->GetRect());
}

// lobbymain.cpp:14108
void CLobbyMain::OnGameRoom_CaddieInit(intptr_t param) {
	m_caddieViewer = DYNAMIC_CAST(FrViewer, param);
}

// lobbymain.cpp:14113
void CLobbyMain::OnGameRoom_CaddieDown(void) {
	WRect rect = m_caddieSelViewer->GetRect();
	WPoint pt(rect.x, rect.y - 5.0f);
	OpenSubEquipBar(pt, (PANGYA_ITEM_GROUP)7, 1);

	if (m_subEquipBar) {
		WRect barRect = m_subEquipBar->GetRect();

		barRect.x = rect.x - barRect.w * 0.5f + 0.5f;
		if (barRect.x + barRect.w > g_view->GetWidth()) {
			barRect.x = g_view->GetWidth() - barRect.w;
		}
		m_subEquipBar->SetRect(barRect);
	}
}

// lobbymain.cpp:14134
// Draws the game room's caddie button (greyed while disabled): the equipped
// caddie's icon, struck through in red when its contract ran out, or
// "w_caddie_no" without one.
void CLobbyMain::OnGameRoom_CaddieOwnerDraw(PG_MSGARG) {
	if (!m_caddieViewer)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	unsigned long color = m_caddieViewer->IsEnabled() ? 0xFFFFFFFF : 0x50B0B0B0;

	WRect rect = m_caddieViewer->GetRect();
	if (WSingleton<CSharedDoc>::Instance()->m_myCaddieId == 0) {
		const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "w_caddie_no");
		if (pBmp)
			pGDI->DrawTexture(pBmp, WRect(m_caddieViewer->GetRect().x, m_caddieViewer->GetRect().y, (float)pBmp->Width(), (float)pBmp->Height()), color, 0);
	} else {
		std::map<unsigned int, sCaddieInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_caddieMap.find(WSingleton<CSharedDoc>::Instance()->m_myCaddieId);

		IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.typeId);
		if (pItem) {
			const Bitmap* pBmp = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->m_icon));
			if (pBmp)
				pGDI->DrawTexture(pBmp, WRect(m_caddieViewer->GetRect().x, m_caddieViewer->GetRect().y, (float)pBmp->Width(), (float)pBmp->Height()), color, 0);
		}

		IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(it->second.typeId);
		if (pCaddie) {
			if ((it->second.flags & 2) && it->second.days <= 0 && it->second.cadItemTypeId == 0 && pCaddie->fee != 0) {
				g_view->DrawLine2D(WPoint(rect.x, rect.y + rect.h * 0.5f - 1.0f), WPoint(rect.x + rect.w - 1.0f, rect.y + rect.h * 0.5f - 1.0f), 0xFFFF0000, 0xFFFF0000, 0);
				g_view->DrawLine2D(WPoint(rect.x, rect.y + rect.h * 0.5f), WPoint(rect.x + rect.w - 1.0f, rect.y + rect.h * 0.5f), 0xFFFF0000, 0xFFFF0000, 0);
				g_view->DrawLine2D(WPoint(rect.x, rect.y + rect.h * 0.5f + 1.0f), WPoint(rect.x + rect.w - 1.0f, rect.y + rect.h * 0.5f + 1.0f), 0xFFFF0000, 0xFFFF0000, 0);
			}
		}
	}

	if (m_caddieViewer->IsEnabled())
		DrawFrameSelectedUserInfo(m_caddieViewer->GetRect());
}

// lobbymain.cpp:14178
void CLobbyMain::OnGameRoom_ClubInit(intptr_t param) {
	m_clubViewer = DYNAMIC_CAST(FrViewer, param);
}

// lobbymain.cpp:14183
void CLobbyMain::OnGameRoom_ClubDown(void) {
	WRect rect = m_clubSelViewer->GetRect();
	WPoint pt(rect.x, rect.y - 5.0f);
	OpenSubEquipBar(pt, (PANGYA_ITEM_GROUP)4, 1);

	if (m_subEquipBar) {
		WRect barRect = m_subEquipBar->GetRect();

		barRect.x = rect.x - barRect.w * 0.5f + 0.5f;
		if (barRect.x + barRect.w > g_view->GetWidth()) {
			barRect.x = g_view->GetWidth() - barRect.w;
		}
		m_subEquipBar->SetRect(barRect);
	}
}

// lobbymain.cpp:14204
// Draws the game room's club button (greyed while disabled): the equipped
// club set's icon, or "w_caddie_no" without one.
void CLobbyMain::OnGameRoom_ClubOwnerDraw(PG_MSGARG) {
	if (!m_clubViewer)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	unsigned long color = m_clubViewer->IsEnabled() ? 0xFFFFFFFF : 0x50B0B0B0;

	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if (doc->m_myClubId == 0) {
		const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "w_caddie_no");
		if (pBmp)
			pGDI->DrawTexture(pBmp, m_clubViewer->GetRect(), color, 0);
	} else {
		std::map<unsigned int, sItemInfo>::iterator it = doc->m_myItemMap.find(doc->m_myClubId);

		if (it != doc->m_myItemMap.end()) {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.m_typeId);

			if (pItem) {
				const Bitmap* pBmp = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->m_icon));
				if (pBmp)
					pGDI->DrawTexture(pBmp, WRect(m_clubViewer->GetRect().x, m_clubViewer->GetRect().y, (float)pBmp->Width(), (float)pBmp->Height()), color, 0);
			}
		}
	}

	if (m_clubViewer->IsEnabled())
		DrawFrameSelectedUserInfo(m_clubViewer->GetRect());
}

// lobbymain.cpp:14242
void CLobbyMain::OnGameRoom_AztecInit(intptr_t param) {
	m_aztecViewer = DYNAMIC_CAST(FrViewer, param);
}

// lobbymain.cpp:14247
void CLobbyMain::OnGameRoom_AztecDown(void) {
	WRect rect = m_aztecSelViewer->GetRect();
	WPoint pt(rect.x, rect.y - 5.0f);
	OpenSubEquipBar(pt, (PANGYA_ITEM_GROUP)5, 1);

	if (m_subEquipBar) {
		WRect barRect = m_subEquipBar->GetRect();

		barRect.x = rect.x - barRect.w * 0.5f + 0.5f;
		if (barRect.x + barRect.w > g_view->GetWidth()) {
			barRect.x = g_view->GetWidth() - barRect.w;
		}
		m_subEquipBar->SetRect(barRect);
	}
}

// lobbymain.cpp:14268
// Draws the game room's ball button (greyed while disabled): the equipped
// ball's icon with the count held, or "w_caddie_no" without one.
void CLobbyMain::OnGameRoom_AztecOwnerDraw(PG_MSGARG) {
	if (!m_aztecViewer)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	unsigned long color = m_aztecViewer->IsEnabled() ? 0xFFFFFFFF : 0x50B0B0B0;

	if (WSingleton<CSharedDoc>::Instance()->m_myBallId == 0) {
		const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "w_caddie_no");
		if (pBmp)
			pGDI->DrawTexture(pBmp, m_aztecViewer->GetRect(), color, 0);
	} else {
		IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(WSingleton<CSharedDoc>::Instance()->m_myBallId);
		if (pItem) {
			int count = 0;

			if (IsLocalContent((localContentType_t)0x87)) {
				sItemInfo* pInfo = NULL;
				std::list<sItemInfo>::iterator it;
				for (it = WSingleton<CSharedDoc>::Instance()->m_myGearList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myGearList.end(); ++it) {
					if (it->m_typeId == WSingleton<CSharedDoc>::Instance()->m_myBallId) {
						pInfo = &*it;
						count = it->m_count;
						break;
					}
				}

				if (!pInfo) {
					WSingleton<CSharedDoc>::Instance()->m_myBallId = 0x14000000;
					pItem = ItemManager()->FindCommonItem(WSingleton<CSharedDoc>::Instance()->m_myBallId);
				}

				const Bitmap* pBmp = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->m_icon));
				if (pBmp)
					pGDI->DrawTexture(pBmp, WRect(m_aztecViewer->GetRect().x, m_aztecViewer->GetRect().y, (float)pBmp->Width(), (float)pBmp->Height()), color, 0);
			} else {
				const Bitmap* pBmp = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->m_icon));
				if (pBmp)
					pGDI->DrawTexture(pBmp, WRect(m_aztecViewer->GetRect().x, m_aztecViewer->GetRect().y, (float)pBmp->Width(), (float)pBmp->Height()), color, 0);

				std::list<sItemInfo>::iterator it;
				for (it = WSingleton<CSharedDoc>::Instance()->m_myGearList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myGearList.end(); ++it) {
					if (it->m_typeId == WSingleton<CSharedDoc>::Instance()->m_myBallId) {
						count = it->m_count;
						break;
					}
				}
			}

			if (count > 0) {
				unsigned long textColor = m_aztecViewer->IsEnabled() ? 0xFFFFFFFF : 0x7FFFFFFF;
				Print(m_aztecViewer->GetRect().x + 35.0f, m_aztecViewer->GetRect().y + 22.0f, MakeStr("%d", count), 2.0f, 2, 1.0f, textColor);
			}
		}
	}

	if (m_aztecViewer->IsEnabled())
		DrawFrameSelectedUserInfo(m_aztecViewer->GetRect());
}

// lobbymain.cpp:14347
void CLobbyMain::OnGameRoom_MascotInit(intptr_t param) {
	m_mascotViewer = DYNAMIC_CAST(FrViewer, param);
}

// lobbymain.cpp:14352
void CLobbyMain::OnGameRoom_MascotDown(void) {
	WRect rect = m_mascotSelViewer->GetRect();
	WPoint pt(rect.x, rect.y - 5.0f);
	OpenSubEquipBar(pt, (PANGYA_ITEM_GROUP)16, 1);

	if (m_subEquipBar) {
		WRect barRect = m_subEquipBar->GetRect();

		barRect.x = rect.x - barRect.w * 0.5f + 0.5f;
		if (barRect.x + barRect.w > g_view->GetWidth()) {
			barRect.x = g_view->GetWidth() - barRect.w;
		}
		m_subEquipBar->SetRect(barRect);
	}
}

// lobbymain.cpp:14373
// Draws the game room's mascot button (greyed while disabled): the equipped
// mascot's icon, or "w_caddie_no" without one.
void CLobbyMain::OnGameRoom_MascotOwnerDraw(PG_MSGARG) {
	if (!m_mascotViewer)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	unsigned long color = m_mascotViewer->IsEnabled() ? 0xFFFFFFFF : 0x50B0B0B0;

	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if (doc->m_myMascotId == 0) {
		const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "w_caddie_no");
		if (pBmp)
			pGDI->DrawTexture(pBmp, WRect(m_mascotViewer->GetRect().x, m_mascotViewer->GetRect().y, (float)pBmp->Width(), (float)pBmp->Height()), color, 0);
	} else {
		std::map<unsigned int, sMascotInfo>::iterator it = doc->m_myMascotMap.find(doc->m_myMascotId);
		IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.typeId);
		if (pItem) {
			const Bitmap* pBmp = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->m_icon));
			if (pBmp)
				pGDI->DrawTexture(pBmp, WRect(m_mascotViewer->GetRect().x, m_mascotViewer->GetRect().y, (float)pBmp->Width(), (float)pBmp->Height()), color, 0);
		}
	}

	if (m_mascotViewer->IsEnabled())
		DrawFrameSelectedUserInfo(m_mascotViewer->GetRect());
}

// lobbymain.cpp:14405
void CLobbyMain::OnGameRoom_EquipItemInit(intptr_t param) {
	m_equipItemViewer = DYNAMIC_CAST(FrViewer, param);
}

// lobbymain.cpp:11540
void CLobbyMain::OnRoomList_MakeRoomUp(void) {
	CloseAllDialogs();

	if (LatestVersion()) {
		if (CSharedDoc::Instance()->IsControlServerService(2)) {
			HandleMsg(MsgObject(this, 0x272, 2, 0, 0, 0, 0));
			return;
		}

		if (m_serverDlg == 0 && CSharedDoc::Instance()->m_currentChannel.id == 0xFF) {
			WSendPacket packet(GCT_REQUEST_SERVER_LIST);
			packet.Send(SEND_TO_DEFAULT);

			m_serverDlg = CreateForm<FrServerDlg>(g_pFresh->GetManager(), this, "server", 0);
			m_serverDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnServerDlgResult, WPoint(m_serverDlg->GetRect().x, 50.0f), 0x10);
			m_serverDlg->SetIconRect(m_barButton[8]->GetRect());
		} else {
			EnableRoomEnterControls(false);

			m_makeRoomDlg = CreateForm<FrMakeRoomDlg>(g_pFresh->GetManager(), this, "makeroom", 0);
			m_makeRoomDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnMakeRoomDlgResult, 3);
		}
	}
}

// lobbymain.cpp:14410
void CLobbyMain::OnGameRoom_EquipItemDown(void) {
	InitControlsSelectedUserInfo();

	if (!m_itemEquipDlg) {
		m_itemEquipDlg = CreateForm<FrEquipDlg>(g_pFresh->GetManager(), this, "equip", NULL);

		CSharedDoc* doc = CSharedDoc::Instance();
		if (GalleryMode())
			m_itemEquipDlg->Init(doc->m_roomPlayers[0].m_equipItem, true);
		else
			m_itemEquipDlg->Init(doc->m_equipItem, true);

		m_itemEquipDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnItemEquipResult, 3);
	}
}

// lobbymain.cpp:14427
// Draws the game room's equip item button (greyed while disabled) and, while
// enabled, the selection frame around it.
void CLobbyMain::OnGameRoom_EquipItemOwnerDraw(PG_MSGARG) {
	if (!m_equipItemViewer)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (!pGDI)
		return;

	unsigned long color = m_equipItemViewer->IsEnabled() ? 0xFFFFFFFF : 0x50B0B0B0;

	const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "gameroom_equip_item_bn");

	if (pBmp) {
		pGDI->DrawTexture(pBmp, WRect(m_equipItemViewer->GetRect().x, m_equipItemViewer->GetRect().y, (float)pBmp->Width(), (float)pBmp->Height()), color, 0);
	}

	if (m_equipItemViewer->IsEnabled())
		DrawFrameSelectedUserInfo(m_equipItemViewer->GetRect());
}

// lobbymain.cpp:14450
void CLobbyMain::OnGameRoom_CharSelInit(intptr_t param) {
	m_charSelViewer = DYNAMIC_CAST(FrViewer, param);
	if (m_charSelViewer) {
		m_charSelViewer->SetVisible(false);
	}
}

// lobbymain.cpp:14460
void CLobbyMain::OnGameRoom_CharSelDown(void) {
	InitControlsSelectedUserInfo();
}

// lobbymain.cpp:14465
// Draws the character selection bar above the character button: one icon
// per owned character, framed under the mouse, which becomes the hovered
// sub selection.
void CLobbyMain::OnGameRoom_CharSelOwnerDraw(PG_MSGARG) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	int count = WSingleton<CSharedDoc>::Instance()->m_characterMap.size();
	if (count <= 1)
		return;

	float barWidth = count * 41.0f + 8.0f;
	float x = m_charSelViewer->GetRect().x;
	float y = m_charSelViewer->GetRect().y;
	float w = 0.0f;

	if (x + barWidth > 800.0f)
		x = 800.0f - barWidth;

	const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_01");
	if (pBmp) {
		w = 4.0f;
		pGDI->DrawTexture(pBmp, WRect(x, y, w, 54.0f), 0xFFFFFFFF, 0);
	}
	x += w;

	float itemX = x;
	pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_02");
	if (pBmp) {
		w = (count - 1) * 3.0f + count * 38.0f;
		pGDI->DrawTexture(pBmp, WRect(x, y, w, 54.0f), 0xFFFFFFFF, 0);
	}
	x += w;

	pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_03");
	if (pBmp) {
		w = 4.0f;
		pGDI->DrawTexture(pBmp, WRect(x, y, w, 54.0f), 0xFFFFFFFF, 0);
	}

	y += 4.0f;
	m_subSelection = 0;
	m_subSelectionTab = 0;

	std::map<unsigned int, sCharacterInfo>::const_iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++it) {
		IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.typeId);
		if (pItem) {
			WRect rect(itemX, y, 38.0f, 38.0f);

			const Bitmap* pIcon = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->m_icon));
			if (pIcon)
				pGDI->DrawTexture(pIcon, rect, 0xFFFFFFFF, 0);

			if (rect.IsInRect(g_input->GetMousePoint().x, g_input->GetMousePoint().y)) {
				const Bitmap* pFrame = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_frame");
				if (pFrame)
					pGDI->DrawTexture(pFrame, WRect(rect.x - 3.0f, rect.y - 3.0f, rect.w + 6.0f, rect.h + 6.0f), 0xFFFFFFFF, 0);

				m_subSelection = it->second.typeId;
				m_subSelectionTab = it->second.id;
			}

			itemX += 41.0f;
		}
	}
}

// lobbymain.cpp:14544
void CLobbyMain::OnGameRoom_CaddieSelInit(intptr_t param) {
	m_caddieSelViewer = DYNAMIC_CAST(FrViewer, param);
	if (m_caddieSelViewer) {
		m_caddieSelViewer->SetVisible(false);
	}
}

// lobbymain.cpp:14554
void CLobbyMain::OnGameRoom_CaddieSelDown(void) {
	InitControlsSelectedUserInfo();
}

// lobbymain.cpp:14559
// Draws the caddie selection bar: one icon per owned caddie (struck through
// in red when its contract ran out) plus, while a caddie is equipped, the
// "w_caddie_no" slot that unequips it; the icon under the mouse is framed and
// becomes the hovered sub selection.
void CLobbyMain::OnGameRoom_CaddieSelOwnerDraw(PG_MSGARG) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	int count = WSingleton<CSharedDoc>::Instance()->m_caddieMap.size();
	if (count <= 1)
		return;

	if (WSingleton<CSharedDoc>::Instance()->m_myCaddieId != 0)
		count++;

	float barWidth = count * 41.0f + 8.0f;
	float x = m_caddieSelViewer->GetRect().x;
	float y = m_caddieSelViewer->GetRect().y;
	float w = 0.0f;

	if (x + barWidth > 800.0f)
		x = 800.0f - barWidth;

	const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_01");
	if (pBmp) {
		w = 4.0f;
		pGDI->DrawTexture(pBmp, WRect(x, y, w, 54.0f), 0xFFFFFFFF, 0);
	}
	x += w;

	float itemX = x;
	pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_02");
	if (pBmp) {
		w = (count - 1) * 3.0f + count * 38.0f;
		pGDI->DrawTexture(pBmp, WRect(x, y, w, 54.0f), 0xFFFFFFFF, 0);
	}
	x += w;

	pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_03");
	if (pBmp) {
		w = 4.0f;
		pGDI->DrawTexture(pBmp, WRect(x, y, w, 54.0f), 0xFFFFFFFF, 0);
	}

	y += 4.0f;
	m_subSelection = 0;
	m_subSelectionTab = 0;

	std::map<unsigned int, sCaddieInfo>::const_iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_caddieMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end(); ++it) {
		IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.typeId);
		if (pItem) {
			WRect rect(itemX, y, 38.0f, 38.0f);

			const Bitmap* pIcon = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->m_icon));
			if (pIcon)
				pGDI->DrawTexture(pIcon, rect, 0xFFFFFFFF, 0);

			IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(it->second.typeId);
			if (pCaddie) {
				if ((it->second.flags & 2) && it->second.days <= 0 && it->second.cadItemTypeId == 0 && pCaddie->fee != 0) {
					g_view->DrawLine2D(WPoint(rect.x, rect.y + rect.h * 0.5f - 1.0f), WPoint(rect.x + rect.w - 1.0f, rect.y + rect.h * 0.5f - 1.0f), 0xFFFF0000, 0xFFFF0000, 0);
					g_view->DrawLine2D(WPoint(rect.x, rect.y + rect.h * 0.5f), WPoint(rect.x + rect.w - 1.0f, rect.y + rect.h * 0.5f), 0xFFFF0000, 0xFFFF0000, 0);
					g_view->DrawLine2D(WPoint(rect.x, rect.y + rect.h * 0.5f + 1.0f), WPoint(rect.x + rect.w - 1.0f, rect.y + rect.h * 0.5f + 1.0f), 0xFFFF0000, 0xFFFF0000, 0);
				}
			}

			if (rect.IsInRect(g_input->GetMousePoint().x, g_input->GetMousePoint().y)) {
				const Bitmap* pFrame = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_frame");
				if (pFrame)
					pGDI->DrawTexture(pFrame, WRect(rect.x - 3.0f, rect.y - 3.0f, rect.w + 6.0f, rect.h + 6.0f), 0xFFFFFFFF, 0);

				m_subSelection = it->second.typeId;
				m_subSelectionTab = it->second.id;
			}

			itemX += 41.0f;
		}
	}

	if (WSingleton<CSharedDoc>::Instance()->m_myCaddieId != 0) {
		WRect rect(itemX, y, 38.0f, 38.0f);

		pBmp = g_pFresh->GetBitmap("w_caddie_no");
		if (pBmp)
			pGDI->DrawTexture(pBmp, rect, 0xFFFFFFFF, 0);

		if (rect.IsInRect(g_input->GetMousePoint().x, g_input->GetMousePoint().y)) {
			pBmp = g_pFresh->GetBitmap("select_frame");
			if (pBmp)
				pGDI->DrawTexture(pBmp, WRect(rect.x - 3.0f, rect.y - 3.0f, rect.w + 6.0f, rect.h + 6.0f), 0xFFFFFFFF, 0);

			m_subSelection = 0x1C000000;
			m_subSelectionTab = 0;
		}
	}
}

// lobbymain.cpp:14674
void CLobbyMain::OnGameRoom_ClubSelInit(intptr_t param) {
	m_clubSelViewer = DYNAMIC_CAST(FrViewer, param);
	if (m_clubSelViewer) {
		m_clubSelViewer->SetVisible(false);
	}
}

// lobbymain.cpp:14684
void CLobbyMain::OnGameRoom_ClubSelDown(void) {
	InitControlsSelectedUserInfo();
}

// lobbymain.cpp:14689
// Draws the club set selection bar: one icon per owned item, struck through
// in red when the player's level is under the club set's; the icon under the
// mouse is framed and, when usable, becomes the hovered sub selection.
void CLobbyMain::OnGameRoom_ClubSelOwnerDraw(PG_MSGARG) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	int count = WSingleton<CSharedDoc>::Instance()->m_myItemMap.size();

	if (count <= 1)
		return;

	float barWidth = count * 41.0f + 8.0f;
	float x = m_clubSelViewer->GetRect().x;
	float y = m_clubSelViewer->GetRect().y;
	float w = 0.0f;

	if (x + barWidth > 800.0f)
		x = 800.0f - barWidth;

	const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_01");
	if (pBmp) {
		w = 4.0f;
		pGDI->DrawTexture(pBmp, WRect(x, y, w, 54.0f), 0xFFFFFFFF, 0);
	}
	x += w;

	float itemX = x;
	pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_02");
	if (pBmp) {
		w = (count - 1) * 3.0f + count * 38.0f;
		pGDI->DrawTexture(pBmp, WRect(x, y, w, 54.0f), 0xFFFFFFFF, 0);
	}
	x += w;

	pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_03");
	if (pBmp) {
		w = 4.0f;
		pGDI->DrawTexture(pBmp, WRect(x, y, w, 54.0f), 0xFFFFFFFF, 0);
	}

	y += 4.0f;
	m_subSelection = 0;
	m_subSelectionTab = 0;

	std::map<unsigned int, sItemInfo>::const_iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_myItemMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myItemMap.end(); ++it) {
		IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.m_typeId);

		if (pItem) {
			const Bitmap* pIcon = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->m_icon));
			if (pIcon)
				pGDI->DrawTexture(pIcon, WRect(itemX, y, 38.0f, 38.0f), 0xFFFFFFFF, 0);

			bool usable = WSingleton<CSharedDoc>::Instance()->CheckUnderLevelClubSet(it->second.m_typeId);
			if (!usable) {
				g_view->DrawLine2D(WPoint(itemX, y + 19.0f - 1.0f), WPoint(itemX + 38.0f - 1.0f, y + 19.0f - 1.0f), 0xFFFF0000, 0xFFFF0000, 0);
				g_view->DrawLine2D(WPoint(itemX, y + 19.0f), WPoint(itemX + 38.0f - 1.0f, y + 19.0f), 0xFFFF0000, 0xFFFF0000, 0);
				g_view->DrawLine2D(WPoint(itemX, y + 19.0f + 1.0f), WPoint(itemX + 38.0f - 1.0f, y + 19.0f + 1.0f), 0xFFFF0000, 0xFFFF0000, 0);
			}

			if (WRect(itemX, y, 38.0f, 38.0f).IsInRect(g_input->GetMousePoint().x, g_input->GetMousePoint().y)) {
				const Bitmap* pFrame = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_frame");
				if (pFrame)
					pGDI->DrawTexture(pFrame, WRect(itemX - 3.0f, y - 3.0f, 44.0f, 44.0f), 0xFFFFFFFF, 0);

				if (usable == true) {
					m_subSelection = it->second.m_typeId;
					m_subSelectionTab = it->second.m_serial;
				}
			}

			itemX += 41.0f;
		}
	}
}

// lobbymain.cpp:14784
void CLobbyMain::OnGameRoom_AztecSelInit(intptr_t param) {
	m_aztecSelViewer = DYNAMIC_CAST(FrViewer, param);
	if (m_aztecSelViewer) {
		m_aztecSelViewer->SetVisible(false);
	}
}

// lobbymain.cpp:14794
void CLobbyMain::OnGameRoom_AztecSelDown(void) {
	InitControlsSelectedUserInfo();
}

// lobbymain.cpp:14799
// Draws the ball selection bar: one icon per gear item with the count held;
// the icon under the mouse is framed and becomes the hovered sub selection.
void CLobbyMain::OnGameRoom_AztecSelOwnerDraw(PG_MSGARG) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	IFF_ITEM_COMMON* pBall = ItemManager()->FindCommonItem(WSingleton<CSharedDoc>::Instance()->m_myBallId);
	if (!pBall)
		return;

	int count = WSingleton<CSharedDoc>::Instance()->m_myGearList.size();
	if (count <= 1)
		return;

	float barWidth = count * 41.0f + 8.0f;
	float x = m_aztecSelViewer->GetRect().x;
	float y = m_aztecSelViewer->GetRect().y;
	float w = 0.0f;

	if (x + barWidth > 800.0f)
		x = 800.0f - barWidth;

	const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_01");
	if (pBmp) {
		w = 4.0f;
		pGDI->DrawTexture(pBmp, WRect(x, y, w, 54.0f), 0xFFFFFFFF, 0);
	}
	x += w;

	float itemX = x;
	pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_02");
	if (pBmp) {
		w = (count - 1) * 3.0f + count * 38.0f;
		pGDI->DrawTexture(pBmp, WRect(x, y, w, 54.0f), 0xFFFFFFFF, 0);
	}
	x += w;

	pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_03");
	if (pBmp) {
		w = 4.0f;
		pGDI->DrawTexture(pBmp, WRect(x, y, w, 54.0f), 0xFFFFFFFF, 0);
	}

	y += 4.0f;
	m_subSelection = 0;
	m_subSelectionTab = 0;

	std::list<sItemInfo>::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_myGearList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myGearList.end(); ++it) {
		IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->m_typeId);
		if (pItem) {
			WRect rect(itemX, y, 38.0f, 38.0f);

			const Bitmap* pIcon = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->m_icon));
			if (pIcon)
				pGDI->DrawTexture(pIcon, WRect(itemX, y, 38.0f, 38.0f), 0xFFFFFFFF, 0);

			int num = it->m_count;
			if (num > 0) {
				unsigned long textColor = m_aztecSelViewer->IsEnabled() ? 0xFFFFFFFF : 0x7FFFFFFF;

				Print(itemX + 35.0f, y + 22.0f, MakeStr("%d", num), 2.0f, 2, 1.0f, textColor);
			}

			if (rect.IsInRect(WPoint(g_input->GetMousePoint().x, g_input->GetMousePoint().y))) {
				const Bitmap* pFrame = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_frame");
				if (pFrame)
					pGDI->DrawTexture(pFrame, WRect(itemX - 3.0f, y - 3.0f, 44.0f, 44.0f), 0xFFFFFFFF, 0);

				m_subSelection = it->m_typeId;
				m_subSelectionTab = it->m_serial;
			}

			itemX += 41.0f;
		}
	}
}

// lobbymain.cpp:14894
void CLobbyMain::OnGameRoom_MascotSelInit(intptr_t param) {
	m_mascotSelViewer = DYNAMIC_CAST(FrViewer, param);
	if (m_mascotSelViewer) {
		m_mascotSelViewer->SetVisible(false);
	}
}

// lobbymain.cpp:14904
void CLobbyMain::OnGameRoom_MascotSelDown(void) {
	InitControlsSelectedUserInfo();
}

// lobbymain.cpp:14909
// Draws the mascot selection bar: one icon per owned mascot plus, while a
// mascot is equipped, the "w_caddie_no" slot that unequips it; the icon under
// the mouse is framed and becomes the hovered sub selection.
void CLobbyMain::OnGameRoom_MascotSelOwnerDraw(PG_MSGARG) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	int count = WSingleton<CSharedDoc>::Instance()->m_myMascotMap.size();
	if (count == 0)
		return;

	if (WSingleton<CSharedDoc>::Instance()->m_myMascotId != 0)
		count++;

	float barWidth = count * 41.0f + 8.0f;
	float x = m_mascotSelViewer->GetRect().x;
	float y = m_mascotSelViewer->GetRect().y;
	float w = 0.0f;

	if (x + barWidth > 800.0f)
		x = 800.0f - barWidth;

	const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_01");
	if (pBmp) {
		w = 4.0f;
		pGDI->DrawTexture(pBmp, WRect(x, y, w, 54.0f), 0xFFFFFFFF, 0);
	}
	x += w;

	float itemX = x;
	pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_02");
	if (pBmp) {
		w = (count - 1) * 3.0f + count * 38.0f;
		pGDI->DrawTexture(pBmp, WRect(x, y, w, 54.0f), 0xFFFFFFFF, 0);
	}
	x += w;

	pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_bar_03");
	if (pBmp) {
		w = 4.0f;
		pGDI->DrawTexture(pBmp, WRect(x, y, w, 54.0f), 0xFFFFFFFF, 0);
	}

	y += 4.0f;
	m_subSelection = 0;
	m_subSelectionTab = 0;

	std::map<unsigned int, sMascotInfo>::const_iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_myMascotMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myMascotMap.end(); ++it) {
		IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.typeId);
		if (pItem) {
			const Bitmap* pIcon = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->m_icon));
			if (pIcon)
				pGDI->DrawTexture(pIcon, WRect(itemX, y, 38.0f, 38.0f), 0xFFFFFFFF, 0);

			if (WRect(itemX, y, 38.0f, 38.0f).IsInRect(g_input->GetMousePoint().x, g_input->GetMousePoint().y)) {
				const Bitmap* pFrame = g_pFresh->GetManager()->GetBitmap("ITEMS", "select_frame");
				if (pFrame)
					pGDI->DrawTexture(pFrame, WRect(itemX - 3.0f, y - 3.0f, 44.0f, 44.0f), 0xFFFFFFFF, 0);

				m_subSelection = it->second.typeId;
				m_subSelectionTab = it->second.id;
			}

			itemX += 41.0f;
		}
	}

	if (WSingleton<CSharedDoc>::Instance()->m_myMascotId != 0) {
		WRect rect(itemX, y, 38.0f, 38.0f);

		pBmp = g_pFresh->GetBitmap("w_caddie_no");
		if (pBmp)
			pGDI->DrawTexture(pBmp, rect, 0xFFFFFFFF, 0);

		if (rect.IsInRect(g_input->GetMousePoint().x, g_input->GetMousePoint().y)) {
			pBmp = g_pFresh->GetBitmap("select_frame");
			if (pBmp)
				pGDI->DrawTexture(pBmp, WRect(rect.x - 3.0f, rect.y - 3.0f, rect.w + 6.0f, rect.h + 6.0f), 0xFFFFFFFF, 0);

			m_subSelection = 0x40000000;
			m_subSelectionTab = 0;
		}
	}
}

// lobbymain.cpp:15013
void CLobbyMain::OnGameRoom_StartTipInit(intptr_t param) {
	m_startTipArea = DYNAMIC_CAST(FrArea, param);
	if (m_startTipArea) {
		m_startTipArea->SetVisible(false);
	}
}

// lobbymain.cpp:15022
void CLobbyMain::OnGameRoom_ReadyTipInit(intptr_t param) {
	m_readyTipArea = DYNAMIC_CAST(FrArea, param);
	if (m_readyTipArea) {
		m_readyTipArea->SetVisible(false);
	}
}

// lobbymain.cpp:15031
void CLobbyMain::OnGameRoom_StartInit(intptr_t param) {
	m_startBtn = DYNAMIC_CAST(FrButton, param);
}

// lobbymain.cpp:15038
void CLobbyMain::OnGameRoom_StartBtnUp(void) {
	if (!m_startBtn || !m_startBtn->IsEnabled())
		return;

	if (WSingleton<CSharedDoc>::Instance()->IsControlServerService(2)) {
		this << MsgObject(this, 0x272, 2, 0, 0, 0, 0);
		return;
	}

	if (MyGuid(false) == m_roomMasterUid) {
		if (!LatestVersion())
			return;

		if ((WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x800) && !AdminMode()) {
			unsigned char course = WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course;
			IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(0x28000000 | ((course > 0x7F && course != 0xFD) ? course - 0x80 : course));
			if (!pCourse)
				return;

			// TODO: the original re-reads m_course through an inline course-id helper
			// (shareddoc.inl, not identified) at each of the three compares.
			if ((pCourse->courseFlags & 0x0F) >= 3
				|| ((WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x800)
					&& ((0x28000000 | ((WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course > 0x7F && WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course != 0xFD) ? WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course - 0x80 : WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course)) == 1
						|| (0x28000000 | ((WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course > 0x7F && WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course != 0xFD) ? WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course - 0x80 : WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course)) == 0x0B
						|| (0x28000000 | ((WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course > 0x7F && WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course != 0xFD) ? WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course - 0x80 : WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course)) == 0x0E))) {
				this << MsgObject(this, 0x23, (intptr_t)"\xB7\xE7\xC5\xB0\xC3\xA4\xB3\xCE\xBF\xA1\xBC\xAD\xB4\xC2 \xC0\xCC \xB8\xCA\xC0\xBB \xC7\xC3\xB7\xB9\xC0\xCC \xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
				return;
			}
		}

		if (m_startBtn)
			m_startBtn->Enable(false);

		WSendPacket packet(GCT_REQUEST_START_GAME);
		packet.Encode4(MyGuid(false));
		packet.Send(SEND_TO_DEFAULT);

		if (IsLocalContent((localContentType_t)0x54))
			CMessengerInfo::Instance()->EnableInvite(false);
		return;
	}

	if (m_roomOptionC || m_roomOptionB)
		return;

	m_roomOptionB = true;
	if (m_roomOptionA) {
		WSendPacket packet(GCT_SET_PLAYER_READY);
		packet.Encode1(1);
		packet.Send(SEND_TO_DEFAULT);
	} else {
		m_tipBlinking = false;
		if (m_blinkTipArea)
			m_blinkTipArea->SetVisible(false);

		WSendPacket packet(GCT_SET_PLAYER_READY);
		packet.Encode1(0);
		packet.Send(SEND_TO_DEFAULT);
	}

	EnableSettingButton(m_roomOptionA);
}

// lobbymain.cpp:15167
void CLobbyMain::OnGameRoom_ReadyInit(intptr_t param) {
	m_readyBtn = DYNAMIC_CAST(FrButton, param);
	if (m_readyBtn) {
		m_readyBtn->SetVisible(false);
		m_readyBtn->Enable(false);
	}
}

// lobbymain.cpp:15177
void CLobbyMain::OnGameRoom_ReadyBtnUp(void) {
	if (WSingleton<CSharedDoc>::Instance()->IsControlServerService(2)) {
		this << MsgObject(this, 0x272, 2, 0, 0, 0, 0);
		return;
	}

	if (MyGuid(false) == m_roomMasterUid) {
		return;
	}
	if (m_roomOptionC || m_roomOptionB) {
		return;
	}

	m_roomOptionB = true;
	if (m_roomOptionA) {
		WSendPacket packet(GCT_SET_PLAYER_READY);
		packet.Encode1(1);
		packet.Send(SEND_TO_DEFAULT);
	} else {
		m_tipBlinking = false;
		if (m_blinkTipArea) {
			m_blinkTipArea->SetVisible(false);
		}

		WSendPacket packet(GCT_SET_PLAYER_READY);
		packet.Encode1(0);
		packet.Send(SEND_TO_DEFAULT);
	}

	EnableSettingButton(m_roomOptionA);
}

// lobbymain.cpp:15218
void CLobbyMain::EnableSettingButton(bool enable) {
	if (CTaskMain::m_charViewer) {
		CTaskMain::m_charViewer->Enable(enable);
	}
	if (CTaskMain::m_caddieViewer) {
		CTaskMain::m_caddieViewer->Enable(enable);
	}
	if (CTaskMain::m_clubViewer) {
		CTaskMain::m_clubViewer->Enable(enable);
	}
	if (CTaskMain::m_aztecViewer) {
		CTaskMain::m_aztecViewer->Enable(enable);
	}
	if (CTaskMain::m_mascotViewer) {
		CTaskMain::m_mascotViewer->Enable(enable);
	}

	CloseAllSubSelection();

	if (m_charViewer) {
		m_charViewer->Enable(enable);
	}
	if (m_caddieViewer) {
		m_caddieViewer->Enable(enable);
	}
	if (m_clubViewer) {
		m_clubViewer->Enable(enable);
	}
	if (m_aztecViewer) {
		m_aztecViewer->Enable(enable);
	}
	if (m_mascotViewer) {
		m_mascotViewer->Enable(enable);
	}
	if (m_equipItemViewer) {
		m_equipItemViewer->Enable(enable);
	}

	InitControlsSelectedUserInfo();

	if (!enable && m_itemEquipDlg) {
		m_itemEquipDlg->Close(FrOK, true);
	}
}

// lobbymain.cpp:15262
void CLobbyMain::OnGameRoom_RoomOptionInit(intptr_t param) {
	m_roomOptionBtn = DYNAMIC_CAST(FrButton, param);
}

// lobbymain.cpp:15267
void CLobbyMain::OnGameRoom_RoomOptionBtnUp(void) {
	// "everyone is ready: the room settings can no longer be changed" (CP949)
	if (m_blinkTipArea && m_blinkTipArea->IsVisible() && !AdminMode()) {
		this << MsgObject(this, 0x23, (intptr_t)"\xb8\xf0\xb5\xce \xc1\xd8\xba\xf1\xb0\xa1 \xb5\xc8 \xbb\xf3\xc8\xb2\xbf\xa1\xbc\xad\xb4\xc2 \xb9\xe6\xc1\xa4\xba\xb8\xb8\xa6 \xba\xaf\xb0\xe6\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9", 0, 0, 0, 0);
		return;
	}

	FrChangeRoomInfoDlg* pDlg = CreateForm<FrChangeRoomInfoDlg>(g_pFresh->GetManager(), this, "changeroominfo", NULL);
	pDlg->SetDesc("\xb9\xe6\xc0\xc7 \xbf\xc9\xbc\xc7\xc0\xbb \xba\xaf\xb0\xe6\xc7\xcf\xbc\xbc\xbf\xe4."); // "change the room's options." (CP949)
	pDlg->Open(NULL, 1);
	pDlg->MoveCursor("ok");
}

// lobbymain.cpp:15289
void CLobbyMain::OnGameRoom_MapInit(PG_MSGARG param) {
	m_mapBtn = DYNAMIC_CAST(FrButton, param);
	if (!m_mapBtn) {
		return;
	}

	m_mapBtn->SetStyle(FrButton::BT_NORMAL);

	// tournament channels and the game types 6 and 10 keep the map button
	// pressed
	if ((CSharedDoc::Instance()->m_currentChannel.flags & 0x80)
		|| CSharedDoc::Instance()->m_roomInfo.m_gameType == 6 || CSharedDoc::Instance()->m_roomInfo.m_gameType == 10) {
		m_mapBtn->SetStyle(FrButton::BT_ALWAYSON);
		m_mapBtn->SetPushDelay(0.0f);
	}
}

// lobbymain.cpp:15310
void CLobbyMain::OnGameRoom_MapBtnUp(void) {
	if (m_roomMasterUid != MyGuid(false)) {
		return;
	}
	if (m_roomOptionD && WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.size() > 1 && !AdminMode()) {
		return;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 10
		|| WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 6) {
		return;
	}

	if (m_mapSelectDlg) {
		return;
	}

	m_mapSelectDlg = CreateForm<FrTreasureCourse>(g_pFresh->GetManager(), this, "treasuremap", 0);
	if (m_mapSelectDlg) {
		m_mapSelectDlg->SetSelectMap(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course);
		m_mapSelectDlg->SetMapIndexChanged(false);
		m_mapSelectDlg->LockRandom(WSingleton<CSharedDoc>::Instance()->m_roomType == 2);
		m_mapSelectDlg->EnableDrag(false);
		m_mapSelectDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnMapSelectDlgResult, 3);
	}
}

// lobbymain.cpp:15340
void CLobbyMain::OnGameRoom_MapPrevInit(intptr_t param) {
	m_mapPrevBtn = DYNAMIC_CAST(FrButton, param);
	if (m_mapPrevBtn) {
		m_mapPrevBtn->Enable(false);
		m_mapPrevBtn->SetVisible(false);
		m_mapPrevBtn->SetPushDelay(0.0f);
	}
}

// lobbymain.cpp:15351
void CLobbyMain::OnGameRoom_MapNextInit(intptr_t param) {
	m_mapNextBtn = DYNAMIC_CAST(FrButton, param);
	if (m_mapNextBtn) {
		m_mapNextBtn->Enable(false);
		m_mapNextBtn->SetVisible(false);
		m_mapNextBtn->SetPushDelay(0.0f);
	}
}

// lobbymain.cpp:15362
void CLobbyMain::OnGameRoom_MapPrevBtnUp(void) {
	if (m_roomOptionC) {
		return;
	}

	unsigned char map = PrevMap(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course);

	WSendPacket packet(GCT_REQUEST_ROOMINFO_CHANGED);
	packet.Encode2(0xFFFF);
	packet.Encode1(1);
	packet.Encode1(3);
	packet.Encode1(map);
	packet.Send(SEND_TO_DEFAULT);

	this << MsgObject(0, 1, 0, 0, 0, 0, 0);
}

// lobbymain.cpp:15392
void CLobbyMain::OnGameRoom_MapNextBtnUp(void) {
	if (m_roomOptionC) {
		return;
	}

	unsigned char map = NextMap(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course, false);

	WSendPacket packet(GCT_REQUEST_ROOMINFO_CHANGED);
	packet.Encode2(0xFFFF);
	packet.Encode1(1);
	packet.Encode1(3);
	packet.Encode1(map);
	packet.Send(SEND_TO_DEFAULT);

	this << MsgObject(0, 1, 0, 0, 0, 0, 0);
}

// lobbymain.cpp:15422
void CLobbyMain::OnGameRoom_MapHoleComboInit(PG_MSGARG param) {
	m_holeCombo = DYNAMIC_CAST(FrComboCtlEx, param);
	ReLoadHoleItem();

	unsigned char select = CSharedDoc::Instance()->m_roomInfo.m_value4B;
	if (m_holeCombo) {
		if (CSharedDoc::Instance()->m_roomInfo.m_holeCount == 18) {
			if (select > 0) {
				select = 1;
			}
		}

		m_holeCombo->SelectItem(select);
	}
}

// lobbymain.cpp:15437
void CLobbyMain::OnGameRoom_MapHoleComboDown(void) {
	if (!m_holeCombo) {
		return;
	}
	if (m_roomOptionC) {
		return;
	}

	unsigned char select = m_holeCombo->GetCurrentIdx();
	if (CSharedDoc::Instance()->m_roomInfo.m_holeCount != 18) {
		if (CSharedDoc::Instance()->m_roomInfo.m_value4B == select) {
			return;
		}
	} else if (select == 0) {
		if (CSharedDoc::Instance()->m_roomInfo.m_value4B == 0) {
			return;
		}
	} else {
		if (CSharedDoc::Instance()->m_roomInfo.m_value4B == 3) {
			return;
		}
		select = 3;
	}

	WSendPacket packet(GCT_REQUEST_ROOMINFO_CHANGED);
	packet.Encode2(0xFFFF);
	packet.Encode1(1);
	packet.Encode1(5);
	packet.Encode1(select);
	packet.Send(SEND_TO_DEFAULT);

	this << MsgObject(0, 1, 0, 0, 0, 0, 0);
}

// lobbymain.cpp:15484
void CLobbyMain::OnGameRoom_MapLockInit(intptr_t param) {
	m_mapLockArea = DYNAMIC_CAST(FrArea, param);
	if (m_mapLockArea) {
		m_mapLockArea->SetVisible(false);
	}
}

// lobbymain.cpp:15494
void CLobbyMain::OnGameRoom_UserListInit(PG_MSGARG) {
}

// lobbymain.cpp:15498
void CLobbyMain::OnGameRoom_UserListBtnUp(void) {
	if (m_userListDlg) {
		return;
	}

	m_userListDlg = CreateForm<FrUserListDlg>(g_pFresh->GetManager(), this, "userlist", NULL);
	if (!m_userListDlg) {
		return;
	}

	m_userListDlg->MakeUserList(WSingleton<CSharedDoc>::Instance()->m_userMap);
	m_userListDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnUserListDlgResult, 3);

	// right edge of the screen, 5 pixels in
	WPoint pt;
	pt.x = g_view->GetWidth() - m_userListDlg->GetRect().w - 5.0f;
	pt.y = m_userListDlg->GetRect().y;
	m_userListDlg->MoveWindow(pt);

	if (m_userListDlg) {
		m_userListDlg->EnableRightButton(m_roomOptionA == false);
	}
}

// lobbymain.cpp:15519
void CLobbyMain::OnGameRoom_ChangeTeamBtnInit(intptr_t param) {
	m_changeTeamBtn = DYNAMIC_CAST(FrButton, param);
	if (m_changeTeamBtn) {
		m_changeTeamBtn->SetPushDelay(0.5f);
	}
}

// lobbymain.cpp:15527
void CLobbyMain::OnGameRoom_ChangeTeamBtnUp(void) {
	if (m_roomOptionA || m_roomOptionC) {
		return;
	}

	CSharedDoc *doc = CSharedDoc::Instance();

	std::list<sSlotInfo>::iterator it = doc->m_roomPlayerList.begin();
	unsigned long guid = doc->m_userInfo.guid;
	while (it != doc->m_roomPlayerList.end() && it->m_uid != guid) {
		++it;
	}

	if (it != CSharedDoc::Instance()->m_roomPlayerList.end()) {
		it->m_attributeBits.team = 1 - it->m_attributeBits.team;

		WSendPacket packet(GCT_CHANGE_TEAM);
		packet.Encode1(it->m_attributeBits.team);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// lobbymain.cpp:15547
void CLobbyMain::OnGameRoom_PowerBarInit(intptr_t param) {
	m_powerBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_powerBar->SetRange(0, 50, 0);
}

// lobbymain.cpp:15553
void CLobbyMain::OnGameRoom_ControlBarInit(intptr_t param) {
	m_controlBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_controlBar->SetRange(0, 30, 0);
}

// lobbymain.cpp:15559
void CLobbyMain::OnGameRoom_AccuracyBarInit(intptr_t param) {
	m_accuracyBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_accuracyBar->SetRange(0, 30, 0);
}

// lobbymain.cpp:15565
void CLobbyMain::OnGameRoom_SpinBarInit(intptr_t param) {
	m_spinBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_spinBar->SetRange(0, 30, 0);
}

// lobbymain.cpp:15571
void CLobbyMain::OnGameRoom_CurveBarInit(intptr_t param) {
	m_curveBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_curveBar->SetRange(0, 30, 0);
}

// lobbymain.cpp:15577
void CLobbyMain::OnGameRoom_PowerEditInit(intptr_t param) {
	m_powerStatic = DYNAMIC_CAST(FrStatic, param);
}

// lobbymain.cpp:15582
void CLobbyMain::OnGameRoom_ControlEditInit(intptr_t param) {
	m_controlStatic = DYNAMIC_CAST(FrStatic, param);
}

// lobbymain.cpp:15587
void CLobbyMain::OnGameRoom_AccuracyEditInit(intptr_t param) {
	m_accuracyStatic = DYNAMIC_CAST(FrStatic, param);
}

// lobbymain.cpp:15592
void CLobbyMain::OnGameRoom_SpinEditInit(intptr_t param) {
	m_spinStatic = DYNAMIC_CAST(FrStatic, param);
}

// lobbymain.cpp:15597
void CLobbyMain::OnGameRoom_CurveEditInit(intptr_t param) {
	m_curveStatic = DYNAMIC_CAST(FrStatic, param);
}

// lobbymain.cpp:15602
void CLobbyMain::OnGameRoom_ScoreInit(intptr_t param) {
	m_scoreBtn = DYNAMIC_CAST(FrButton, param);
	if (m_scoreBtn) {
		m_scoreBtn->Enable(false);
	}
}

// lobbymain.cpp:15611
// Opens the score dialog of the selected player ("score_guild" in a guild
// room; the local player when nobody is selected).
void CLobbyMain::OnGameRoom_ScoreBtnUp(void) {
	if (!m_scoreDlg) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
			m_scoreDlg = CreateForm<FrScoreDlg>(g_pFresh->GetManager(), this, "score_guild", NULL);
			m_scoreDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnScoreDlgResult, 1);

			m_scoreDlg->SetGuildPlayer(m_selectedUserId);
			m_scoreDlg->SetVisibleCloseBtn(false);
		} else {
			m_scoreDlg = CreateForm<FrScoreDlg>(g_pFresh->GetManager(), this, "score", NULL);
			m_scoreDlg->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnScoreDlgResult, 1);

			if (m_selectedUserId == 0xFFFFFFFF) {
				m_selectedUserId = WSingleton<CSharedDoc>::Instance()->m_userInfo.guid;
				m_removeFriendUid = WSingleton<CSharedDoc>::Instance()->m_myUID;
			}

			m_scoreDlg->SetPlayer(m_selectedUserId);
			m_scoreDlg->SetVisibleCloseBtn(true);
		}
	}
}

// lobbymain.cpp:15645
void CLobbyMain::OnGameRoomExt_Init(PG_MSGARG param) {
	OnGameRoom_Init(param);
}

// lobbymain.cpp:15650
void CLobbyMain::OnGameRoomExt_Finish(void) {
	m_titleArea->SetBgImg("title_mass");

	if (m_chatWndOpen) {
		sRoomSlot slot;
		slot.m_pPetArea = m_petArea[0];
		slot.m_pPet = NULL;
		slot.m_partTidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();
		slot.m_characterInfo = WSingleton<CSharedDoc>::Instance()->m_characterMap[WSingleton<CSharedDoc>::Instance()->m_myCharacterId];
		slot.m_angelFlag = slot.m_angelFlag ^ ((slot.m_angelFlag ^ (WSingleton<CSharedDoc>::Instance()->m_userInfo.eventFlags >> 3)) & 1);
		slot.m_angelFlag = slot.m_angelFlag ^ ((slot.m_angelFlag ^ (WSingleton<CSharedDoc>::Instance()->m_userInfo.eventFlags >> 3)) & 2);
		WSingleton<CSharedDoc>::Instance()->m_roomSlotMap[MyGuid(false)] = slot;

		ShowPet(MyGuid(false));
	}

	this << MsgObject(this, 0x11, WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_course, 0, 0, 0, 0);

	if (WSingleton<CSharedDoc>::Instance()->m_tutorialFinish) {
		this << MsgObject(0, 0xAA, 1, 0, 0, 0, 0);
	}

	EnableUnderBar(false);
}

// lobbymain.cpp:15704
void CLobbyMain::OnGameRoomExt_Destroy(void) {
	EnableUnderBar(true);
}

// lobbymain.cpp:15721
// The extended room's player list: right button and dummy rows on, its
// bitmaps, no selection, and the list itself once the chat window is open.
void CLobbyMain::OnGameRoomExt_RoomUserInit(PG_MSGARG param) {
	m_roomUserListBox = (FrListBox*)((__rtti_obj = (void*)param) != 0 ? (((IObject*)__rtti_obj)->IsKindOf(&FrListBox::m_RTTI) ? (void*)param : 0) : 0);
	if (m_roomUserListBox == 0)
		return;

	m_roomUserListBox->UseRightButton(true);
	m_roomUserListBox->UseDummy(true);

	m_inviteBmp = m_roomUserListBox->GetBitmap("30_player_wait");

	m_genderBmp[0][0] = m_roomUserListBox->GetBitmap("i_male");
	m_genderBmp[0][1] = m_roomUserListBox->GetBitmap("i_female");
	m_genderBmp[1][0] = m_roomUserListBox->GetBitmap("i_male_02");
	m_genderBmp[1][1] = m_roomUserListBox->GetBitmap("i_female_02");
	m_genderBmp[2][0] = m_roomUserListBox->GetBitmap("i_male_03");
	m_genderBmp[2][1] = m_roomUserListBox->GetBitmap("i_female_03");

	m_genderMannerBmp[0] = m_roomUserListBox->GetBitmap("i_male_manner");
	m_genderMannerBmp[1] = m_roomUserListBox->GetBitmap("i_female_manner");

	m_genderAngelBmp[0] = m_roomUserListBox->GetBitmap("i_male_angel");
	m_genderAngelBmp[1] = m_roomUserListBox->GetBitmap("i_female_angel");

	m_situBmp[0] = m_roomUserListBox->GetBitmap("30in_situ03");
	m_situBmp[1] = m_roomUserListBox->GetBitmap("30in_situ01");
	m_situBmp[2] = m_roomUserListBox->GetBitmap("30in_situ02");
	m_situBmp[3] = m_roomUserListBox->GetBitmap("30in_situ_red_cp");
	m_situBmp[4] = m_roomUserListBox->GetBitmap("30in_situ_red");
	m_situBmp[5] = m_roomUserListBox->GetBitmap("30in_situ_blue_cp");
	m_situBmp[6] = m_roomUserListBox->GetBitmap("30in_situ_blue");
	m_roomUserMarkImage[1] = m_roomUserListBox->GetBitmap("dive");

	m_extRoomUserImage[0] = g_pFresh->RegisterBitmap("30_noplayer_base");
	m_extRoomUserImage[1] = g_pFresh->RegisterBitmap("30_player_base");

	m_roomUserSelImage = g_pFresh->RegisterBitmap("kick");

	m_removeFriendUid = m_selectedUserId = (unsigned long)-1;

	if (m_chatWndOpen) {
		MakeRoomUserList();
	}
}

// lobbymain.cpp:15820
// Draws one row of the extended room's player list: a waiting slot, or the
// row base, the selection box, the situation mark (master / ready, team
// colours in team and guild games), the gender icon (angel / manner
// variants), the dive mark, the level (or ladder, or title) icon, the guild
// emblem, the nick and, for the room master, the kick button.
void CLobbyMain::OnGameRoomExt_RoomUserOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	sSlotInfo* pSlot = (sSlotInfo*)pItem->pData;

	if (IsLocalContent((localContentType_t)0x54)) {
		if (pSlot) {
			if (pSlot->m_flags151.flag) {
				pGDI->DrawTexture(m_inviteBmp, WRect(pItem->pos.x, pItem->pos.y, (float)m_inviteBmp->Width(), (float)m_inviteBmp->Height()), 0xFFFFFFFF, 0);
				return;
			}
		}
	}

	pGDI->DrawTexture(m_extRoomUserImage[0], WRect(pItem->pos.x, pItem->pos.y, (float)m_extRoomUserImage[1]->Width(), (float)m_extRoomUserImage[1]->Height()), 0xFFFFFFFF, 0);

	if (pItem->selected)
		pGDI->Box(WRect(pItem->pos.x, pItem->pos.y, (float)m_roomUserListBox->GetItemWidth() + 15.0f, (float)m_roomUserListBox->GetItemHeight()), 0x7F265785, 0, 0);

	if (!pSlot)
		return;

	WRect rect(0.0f, 0.0f, 0.0f, 0.0f);

	int situ;
	if (!pSlot->m_flags55.flag1 && !(pSlot->m_attribute & 0x18)) {
		situ = 0;
	} else {
		situ = (pSlot->m_attribute & 0x18) ? 1 : 2;

		if (CSharedDoc::Instance()->m_roomInfo.m_gameType == 5 || CSharedDoc::Instance()->m_roomInfo.m_gameType == 6) {
			if (!(pSlot->m_attribute & 3))
				situ += 2;
			else
				situ += 4;
		}
	}

	const Bitmap* pSitu = m_situBmp[situ];
	rect = WRect(pItem->pos.x + 10.0f, pItem->pos.y + 5.0f, (float)pSitu->Width(), (float)pSitu->Height());
	pGDI->DrawTexture(pSitu, rect, 0xFFFFFFFF, 0);

	const Bitmap* pGender;
	if (pSlot->m_flags57.flag0)
		pGender = m_genderAngelBmp[pSlot->m_attributeBits.userFlag % 2];
	else if (pSlot->m_flags55.flag0)
		pGender = m_genderMannerBmp[pSlot->m_attributeBits.userFlag % 2];
	else
		pGender = m_genderBmp[0][pSlot->m_attributeBits.userFlag];

	rect = WRect(pItem->pos.x + 30.0f, pItem->pos.y, (float)pGender->Width(), (float)pGender->Height());
	pGDI->DrawTexture(pGender, rect, 0xFFFFFFFF, 0);

	if (m_roomUserMarkImage[1] && (pSlot->m_attribute & 4))
		pGDI->DrawTexture(m_roomUserMarkImage[1], WRect(pItem->pos.x + 34.0f, pItem->pos.y + 2.0f, (float)m_roomUserMarkImage[1]->Width(), (float)m_roomUserMarkImage[1]->Height()), 0xFFFFFFFF, 0);

	const Bitmap* pLevel;
	if (CSharedDoc::Instance()->m_currentChannel.flags & 0x80) {
		pLevel = g_pFresh->GetBitmap(MakeStr("ladder_%03d", pSlot->m_ladderGrade));
	} else if (pSlot->m_titleTypeId) {
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pSlot->m_titleTypeId);
		if (pSkin)
			pLevel = g_pFresh->GetBitmap(pSkin->m_icon);
		else
			pLevel = g_pFresh->GetBitmap(MakeStr("level_%03d", pSlot->m_level + 1));
	} else {
		pLevel = g_pFresh->GetBitmap(MakeStr("level_%03d", pSlot->m_level + 1));
	}

	if (pLevel) {
		pGDI->DrawTexture(pLevel, WRect(pItem->pos.x + 80.0f - float2int(pLevel->Width() * 0.5f), pItem->pos.y + 13.0f - float2int(pLevel->Height() * 0.5f), (float)pLevel->Width(), (float)pLevel->Height()), 0xFFFFFFFF, 0);
	}

	if (pSlot->m_guildUID && !(pSlot->m_identity & 0x14)) {
		const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pSlot->m_emblemName);

		if (pEmblem)
			pGDI->DrawTexture(pEmblem, WRect(pItem->pos.x + 115.0f, pItem->pos.y + 7.0f - 6.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
	}

	unsigned long color = ((unsigned char)pSlot->m_identity & 0x14) ? 0x50FFFFFF : 0xFFFFFFFF;

	if (pItem->underCursor) {
		pGDI->SetTextColor(0xFFFFFFFF, 0xFF124371);
		pGDI->SetTextStyle(2);
	} else {
		pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
		pGDI->SetTextStyle(0);
	}

	if (!WSingleton<CProjectG>::Instance()->HidePrivacy())
		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 140.0f, pItem->pos.y + 7.0f), 0, pSlot->m_nickname, -1.0f, color);

	if ((!CSharedDoc::Instance()->m_gameOver || AdminMode())
		&& CSharedDoc::Instance()->m_userInfo.guid == m_roomMasterUid
		&& !(pSlot->m_attribute & 0x18)
		&& CSharedDoc::Instance()->m_roomInfo.m_gameType != 6) {
		pGDI->DrawTexture(m_roomUserSelImage, WRect(pItem->pos.x + 226.0f, pItem->pos.y + 7.0f - 2.0f, (float)m_roomUserSelImage->Width(), (float)m_roomUserSelImage->Height()), pItem->underCursor ? 0xFFFFFFFF : 0x80FFFFFF, 0);
	}
}

// lobbymain.cpp:15995
void CLobbyMain::OnGameRoomExt_CountdownInit(intptr_t param) {
	m_countdownArea = DYNAMIC_CAST(FrArea, param);
	if (m_countdownArea) {
		m_countdownArea->SetVisible(false);
	}
}

// lobbymain.cpp:16013
// Back button of the game result screen: a gallery player leaves the room at
// once; a player asks first with the "tolobby" form.
void CLobbyMain::OnGameRoomExt_ResCloseBtnUp(void) {
	if (m_channelRoomCloseBtn)
		m_channelRoomCloseBtn->Enable(false);

	if (GalleryMode()) {
		WSendPacket packet(GCT_EXIT_ROOM);
		packet.Encode1(0);
		packet.Encode2(0xFFFF);
		packet.Encode8(0);
		packet.Encode8(0);
		packet.Send(SEND_TO_DEFAULT);

		MsgObject msg(NULL, 1, 0, 0, 0, 0, 0);
		HandleMsg(msg);
	} else {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "tolobby", NULL);

		pForm->SetMessage("\xB0\xD4\xC0\xD3\xC0\xCC \\c0xffff0000\\c\xBF\xCF\xC0\xFC\xC8\xF7 \xC1\xBE\xB7\xE1\\c0xff000000\\c\xB5\xC7\xB1\xE2 \xC0\xFC\xBF\xA1 \xB0\xD4\xC0\xD3\xC0\xBB \xC6\xF7\xB1\xE2\xC7\xCF\xB8\xE9, \\c0xffff0000\\c\xB0\xE6\xC7\xE8\xC4\xA1 \xC8\xB9\xB5\xE6\xB0\xFA \xB0\xA2\xC1\xBE \xB1\xE2\xB7\xCF\xC0\xCC \xB9\xAB\xC8\xBF\xC3\xB3\xB8\xAE\\c0xff000000\\c\xB5\xCB\xB4\xCF\xB4\xD9.\n\n \t\t\t\t\t\t\t\t\t\xB6\xC7\xC7\xD1 \xB8\xB9\xC0\xBA \xB0\xAD\xC1\xA6\xC1\xBE\xB7\xE1\xB7\xCE \xC0\xCE\xC7\xD8 \xB0\xAD\xC1\xA6\xC1\xBE\xB7\xE1\xC0\xB2\xC0\xCC \xB4\xD9\xC0\xBD\xB0\xFA \xB0\xB0\xC0\xBA \xBC\xF6\xC4\xA1\xB0\xA1 \xB5\xC7\xB8\xE9 \\c0xffff0000\\c\xBC\xBA\xBA\xB0 \xC7\xA5\xBD\xC3 \xBE\xC6\xC0\xCC\xC4\xDC\\c0xff000000\\c\xC0\xCC \xC5\xB9\xC7\xD8\xC1\xFC\xB0\xFA \xB5\xBF\xBD\xC3\xBF\xA1 \t\t\t\t\t\t\t\t\t\\c0xffff0000\\c\xBB\xE7\xBF\xEB \xB1\xE2\xB4\xC9\xBF\xA1 \xB4\xEB\xC7\xD1 \xC1\xA6\xBE\xE0\\c0xff000000\\c\xC0\xCC \xBB\xFD\xB1\xE9\xB4\xCF\xB4\xD9.\n\n\n\n\n\n\n\n\xB0\xD4\xC0\xD3\xC0\xBB \xC6\xF7\xB1\xE2\xC7\xCF\xB0\xED \xB7\xCE\xBA\xF1\xB7\xCE \xB3\xAA\xB0\xA1\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);

		pForm->Open((FRESH_RESULT_CALLBACK)&CLobbyMain::OnRoomExtResQuitDlgResult, 1);
	}
}

// lobbymain.cpp:16064
void CLobbyMain::OnGameRoomExt_FrameInit(intptr_t param) {
	m_extFrame = DYNAMIC_CAST(FrFrame, param);
}

// lobbymain.cpp:16069
void CLobbyMain::OnGameRoomExt_GuildTab1Init(intptr_t param) {
	m_guildTab1Area = DYNAMIC_CAST(FrArea, param);
}

// lobbymain.cpp:16074
void CLobbyMain::OnGameRoomExt_GuildTab2Init(intptr_t param) {
	m_guildTab2Area = DYNAMIC_CAST(FrArea, param);
}

// lobbymain.cpp:16079
void CLobbyMain::OnGameRoomExt_TeamTab1Init(intptr_t param) {
	m_teamTab1Area = DYNAMIC_CAST(FrArea, param);
}

// lobbymain.cpp:16084
void CLobbyMain::OnGameRoomExt_TeamTab2Init(intptr_t param) {
	m_teamTab2Area = DYNAMIC_CAST(FrArea, param);
}

// lobbymain.cpp:16089
void CLobbyMain::OnGameRoomExt_TeamNumInit(intptr_t param) {
	m_teamNumArea = DYNAMIC_CAST(FrArea, param);
}

// lobbymain.cpp:16097
// Team counts of the extended game room: the red and blue team sizes in a
// team game (game types 1 and 5), otherwise the room's player count.
void CLobbyMain::OnGameRoomExt_TeamNumOwnerDraw(PG_MSGARG) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	WRect rect = m_teamNumArea->GetRect();

	pGDI->SetTextStyle(0);

	if (CSharedDoc::Instance()->m_roomInfo.m_gameType == 1 || CSharedDoc::Instance()->m_roomInfo.m_gameType == 5) {
		pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
		g_pFresh->GetManager()->PrintText(WPoint(rect.x + 209.0f, rect.y + 12.0f), 0, MakeStr("(%d)", m_slotList1.size()), -1.0f, 0xFFFFFFFF);

		pGDI->SetTextColor(0xFF0060FF, 0xFFFFFFFF);
		g_pFresh->GetManager()->PrintText(WPoint(rect.x + 458.0f, rect.y + 12.0f), 0, MakeStr("(%d)", m_slotList2.size()), -1.0f, 0xFFFFFFFF);
	} else {
		pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
		g_pFresh->GetManager()->PrintText(WPoint(rect.x + 109.0f, rect.y + 12.0f), 0, "\xB4\xEB\xC8\xB8", -1.0f, 0xFFFFFFFF);

		float x = 209.0f;
		if (CSharedDoc::Instance()->m_roomPlayerList.size() >= 100)
			x = 200.0f;
		g_pFresh->GetManager()->PrintText(WPoint(x + rect.x, rect.y + 12.0f), 0, MakeStr("(%d)", CSharedDoc::Instance()->m_roomPlayerList.size()), -1.0f, 0xFFFFFFFF);
	}
}

// lobbymain.cpp:16127
void CLobbyMain::OnGameRoomExtRes_GuildScoreGaugeInit(intptr_t param) {
	m_guildScoreGauge = DYNAMIC_CAST(FrGaugeBar, param);
	m_guildScoreGauge->SetVisible(false);
}

// lobbymain.cpp:16155
void CLobbyMain::OnGameRoomExt_GuildNumInit(intptr_t param) {
	m_guildNumArea = DYNAMIC_CAST(FrArea, param);
	if (CSharedDoc::Instance()->m_roomInfo.m_gameType != 6) {
		m_guildNumArea->SetVisible(false);
	}
}

// lobbymain.cpp:16172
// Guild counts of the extended game room in a guild match: each guild's
// emblem, name and member count (red, then blue) and, on the result screen,
// the guild points.
void CLobbyMain::OnGameRoomExt_GuildNumOwnerDraw(PG_MSGARG) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	WRect rect = m_teamNumArea->GetRect();
	pGDI->SetTextStyle(1);

	if (CSharedDoc::Instance()->m_roomInfo.m_guild.m_guildUID[0] != 0) {
		const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(CSharedDoc::Instance()->m_roomInfo.m_guild.m_guildMark[0]);

		if (pEmblem)
			pGDI->DrawTexture(pEmblem, WRect(rect.x + 24.0f, rect.y + 8.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
	}

	pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	g_pFresh->GetManager()->PrintText(WPoint(rect.x + 60.0f, rect.y + 12.0f), 0, CSharedDoc::Instance()->m_roomInfo.m_guild.m_guildName[0], -1.0f, 0xFFFFFFFF);
	g_pFresh->GetManager()->PrintText(WPoint(rect.x + 209.0f, rect.y + 12.0f), 0, MakeStr("(%d)", m_slotList1.size()), -1.0f, 0xFFFFFFFF);

	if (strcmp(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXTRES") == 0) {
		g_pFresh->GetManager()->PrintText(WPoint(rect.x + 180.0f, rect.y + 12.0f), 0, MakeStr("%d", CSharedDoc::Instance()->m_teamPoint[0]), -1.0f, 0xFFFFFFFF);
	}

	if (CSharedDoc::Instance()->m_roomInfo.m_guild.m_guildUID[1] != 0) {
		const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(CSharedDoc::Instance()->m_roomInfo.m_guild.m_guildMark[1]);

		if (pEmblem)
			pGDI->DrawTexture(pEmblem, WRect(rect.x + 273.0f, rect.y + 8.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
	}

	pGDI->SetTextColor(0xFF0060FF, 0xFFFFFFFF);
	g_pFresh->GetManager()->PrintText(WPoint(rect.x + 309.0f, rect.y + 12.0f), 0, CSharedDoc::Instance()->m_roomInfo.m_guild.m_guildName[1], -1.0f, 0xFFFFFFFF);
	g_pFresh->GetManager()->PrintText(WPoint(rect.x + 458.0f, rect.y + 12.0f), 0, MakeStr("(%d)", m_slotList2.size()), -1.0f, 0xFFFFFFFF);

	if (strcmp(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM_EXTRES") == 0) {
		g_pFresh->GetManager()->PrintText(WPoint(rect.x + 429.0f, rect.y + 12.0f), 0, MakeStr("%d", CSharedDoc::Instance()->m_teamPoint[1]), -1.0f, 0xFFFFFFFF);
	}
}

// lobbymain.cpp:16231
void CLobbyMain::OnGameRoomExt_GuildInviteInit(intptr_t param) {
	m_guildInviteBtn = DYNAMIC_CAST(FrButton, param);
}

// lobbymain.cpp:16240
// Guild room "invite": counts the lobby's players per guild (not the local
// player's guild, only players in no room), then invites every player of the
// guilds with at least the room's user count of such players; a "notify" form
// says no guild can be invited, or that the invitations were sent. Outside a
// guild room it is the plain user list.
void CLobbyMain::OnGameRoomExt_GuildInviteLBtnUp(void) {
	char text[32];   // unused: the original's /GS cookie without a buffer
	if (CSharedDoc::Instance()->m_roomInfo.m_gameType == 6) {
		std::map<unsigned long, unsigned int> guildUserCount;
		bool found = false;
		std::map<unsigned long, sBriefUserInfo>::const_iterator it;
		for (it = CSharedDoc::Instance()->m_userMap.begin(); it != CSharedDoc::Instance()->m_userMap.end(); ++it) {
			if (it->second.guildUID != 0 && CSharedDoc::Instance()->m_userInfo.guildUID != it->second.guildUID
				&& !it->second.unknownBit && it->second.roomNumber == 0xFFFF) {
				guildUserCount[it->second.guildUID] += 1;
			}
		}

		std::vector<unsigned long> guilds;
		for (std::map<unsigned long, unsigned int>::iterator git = guildUserCount.begin(); git != guildUserCount.end(); ++git) {
			if (CSharedDoc::Instance()->m_roomInfo.m_userCount <= git->second) {
				guilds.push_back(git->first);
				found = true;
			}
		}

		m_guildInviteDelay = 2.0f;
		if (!found) {
			// "There is no guild to invite."
			m_guildInviteNotifyDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
			m_guildInviteNotifyDlg->SetMessage(MakeStr("\xC3\xCA\xB4\xEB\xC7\xD2 \xBB\xF3\xB4\xEB \xB1\xE6\xB5\xE5\xB0\xA1 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", it->second.nick), false);
			m_guildInviteNotifyDlg->Open((FRESH_RESULT_CALLBACK)0, 0);

			FrButton* pCancel = DYNAMIC_CAST(FrButton, m_guildInviteNotifyDlg->FindChildByName("cancel"));
			return;
		}

		unsigned int count = guilds.size();

		for (it = CSharedDoc::Instance()->m_userMap.begin(); it != CSharedDoc::Instance()->m_userMap.end(); ++it) {
			for (unsigned int i = 0; i < count; i++) {
				if (it->second.guildUID == guilds[i]) {
					if (IsLocalContent((localContentType_t)0x54)) {
						WSendPacket packet(0xB2);
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

		// "The invitation message was sent"
		m_guildInviteNotifyDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		m_guildInviteNotifyDlg->SetMessage(MakeStr("\xC3\xCA\xB4\xEB \xB8\xDE\xBD\xC3\xC1\xF6\xB8\xA6 \xBA\xB8\xB3\xC2\xBD\xC0\xB4\xCF\xB4\xD9", it->second.nick), false);
		m_guildInviteNotifyDlg->Open((FRESH_RESULT_CALLBACK)0, 0);

		FrButton* pCancel = DYNAMIC_CAST(FrButton, m_guildInviteNotifyDlg->FindChildByName("cancel"));
		if (pCancel) {
			pCancel->SetVisible(false);
		}
	} else {
		OnGameRoom_UserListBtnUp();
	}
}

// lobbymain.cpp:16333
void CLobbyMain::OnGameRoomExtRes_RoomUserRankTabInit(intptr_t param) {
	m_userRankTabArea = DYNAMIC_CAST(FrArea, param);
	if (m_userRankTabArea) {
		if (CSharedDoc::Instance()->m_roomInfo.m_gameType == 10) {
			m_userRankTabArea->SetBgImg("approach_tab");
		}
	}
}

// lobbymain.cpp:16354
// The result screen's user ranking (not in guild rooms): one item per result
// line, sorted by rank, and the rank bitmaps.
void CLobbyMain::OnGameRoomExtRes_RoomUserRankInit(PG_MSGARG param) {
	m_userRankListBox = (FrListBox*)((__rtti_obj = (void*)param) != 0 ? (((IObject*)__rtti_obj)->IsKindOf(&FrListBox::m_RTTI) ? (void*)param : 0) : 0);
	if (m_userRankListBox == 0)
		return;

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
		m_userRankListBox->SetVisible(false);
		return;
	}

	m_userRankListBox->UseRightButton(true);

	std::vector<sGameResultInfo>::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); it++) {
		m_userRankListBox->AddItem(&*it);
	}

	if (m_userRankListBox->GetScrollBar()) {
		m_userRankListBox->GetScrollBar()->SetGuideVisible(true);
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 10) {
		m_userRankListBox->SortItem(ApproachRankUserCompare);
	} else {
		m_userRankListBox->SortItem(RankUserCompare);
	}

	m_guildMatchEmblem[0] = g_pFresh->RegisterBitmap("30in_team_redteam_rank");
	m_guildMatchEmblem[1] = g_pFresh->RegisterBitmap("30in_team_blueteam_rank");
	m_guildRankBitmap = g_pFresh->RegisterBitmap("30in_rank");

	m_rankBitmap[0] = g_pFresh->RegisterBitmap("rank_gold");
	m_rankBitmap[1] = g_pFresh->RegisterBitmap("rank_silver");
	m_rankBitmap[2] = g_pFresh->RegisterBitmap("rank_bronze");
	m_rankBitmap[3] = g_pFresh->RegisterBitmap("rank_noaward");

	m_userInfoLocked = 0;
}

// lobbymain.cpp:16404
// The result screen's user ranking line: rank (team emblem in a team game),
// medal, guild emblem, nickname (the local player in red, or his team's
// colour), then the approach distance and points, or the score, pang and
// hole.
void CLobbyMain::OnGameRoomExtRes_RoomUserRankOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;

	if (!pItem)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	WRect rect(pItem->pos.x, pItem->pos.y, (float)m_userRankListBox->GetItemWidth(), (float)m_userRankListBox->GetItemHeight());

	if (!pGDI)
		return;

	sGameResultInfo* pInfo = (sGameResultInfo*)pItem->pData;

	if (m_selectedUserId == pInfo->m_uid)
		pGDI->Box(WRect(rect.x, rect.y + 2.0f, rect.w + 20.0f, rect.h - 2.0f), 0x999B9DFF, 0, 0);

	int oldSpace = pGDI->SetSpace(0);

	pGDI->SetTextStyle(0);
	pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);

	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 5) {
		if (m_guildMatchEmblem[pInfo->m_team])
			pGDI->DrawTexture(m_guildMatchEmblem[pInfo->m_team], WRect(rect.x + 5.0f, rect.y + 7.0f - 1.0f, (float)m_guildMatchEmblem[pInfo->m_team]->Width(), (float)m_guildMatchEmblem[pInfo->m_team]->Height()), 0xFFFFFFFF, 0);

		pGDI->Print(WPoint(pItem->pos.x + 12.0f, pItem->pos.y + 7.0f), 1, "%d", pInfo->m_rank);
	} else {
		if (m_guildRankBitmap)
			pGDI->DrawTexture(m_guildRankBitmap, WRect(rect.x + 5.0f, rect.y + 7.0f - 1.0f, (float)m_guildRankBitmap->Width(), (float)m_guildRankBitmap->Height()), 0xFFFFFFFF, 0);

		if (pInfo->m_state == 3)
			pGDI->Print(WPoint(pItem->pos.x + 12.0f, pItem->pos.y + 7.0f), 1, "-");
		else if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 10 && pInfo->m_approachDistance == 0xFFFFFFFF)
			pGDI->Print(WPoint(pItem->pos.x + 12.0f, pItem->pos.y + 7.0f), 1, "-");
		else
			pGDI->Print(WPoint(pItem->pos.x + 12.0f, pItem->pos.y + 7.0f), 1, "%d", pInfo->m_rank);
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType != 10
		&& (!IsLocalContent((localContentType_t)0x2B) || WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_userLimit < 100)
		&& pInfo->m_state != 3) {
		if (pInfo->m_hole != WSingleton<CSharedDoc>::Instance()->m_holeOrder[0]) {
			int medal = GetMedalIndex(pInfo->m_uid);

			if (medal >= 0 && medal <= 2) {
				if (pInfo->m_characterType > 3)
					medal = 3;

				const Bitmap* pMedal = m_rankBitmap[medal];
				pGDI->DrawTexture(pMedal, WRect(rect.x - 5.0f, rect.y + 7.0f - 2.0f, (float)pMedal->Width(), (float)pMedal->Height()), 0xFFFFFFFF, 0);
			}
		}
	}

	if (pInfo->m_hasEmblem) {
		const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pInfo->m_emblemName);

		if (pEmblem)
			pGDI->DrawTexture(pEmblem, WRect(pItem->pos.x + 24.0f, pItem->pos.y + 1.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
	}

	if (pInfo->m_uid == MyGuid(true)) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 5)
			pGDI->SetTextColor(pInfo->m_team ? 0xFF0060FF : 0xFFFF0000, 0xFFFFFFFF);
		else
			pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	} else if (pInfo->m_state == 3) {
		pGDI->SetTextColor(0xFF808080, 0xFFFFFFFF);
	} else {
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	}

	if (!WSingleton<CProjectG>::Instance()->HidePrivacy())
		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 47.0f, pItem->pos.y + 7.0f), 0, pInfo->m_nickname, 80.0f, 0xFFFFFFFF);

	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 10) {
		if (pInfo->m_approachPoint == -1) {
			pGDI->Print(WPoint(pItem->pos.x + 142.0f, pItem->pos.y + 7.0f), 2, "Out");
			pGDI->Print(WPoint(pItem->pos.x + 188.0f, pItem->pos.y + 7.0f), 2, "-");
		} else {
			if (pInfo->m_approachDistance == 0xFFFFFFFF)
				pGDI->Print(WPoint(pItem->pos.x + 142.0f, pItem->pos.y + 7.0f), 2, "Out");
			else
				pGDI->Print(WPoint(pItem->pos.x + 142.0f, pItem->pos.y + 7.0f), 2, "%d.%dy", pInfo->m_approachDistance / 10, pInfo->m_approachDistance % 10);

			pGDI->Print(WPoint(pItem->pos.x + 207.0f, pItem->pos.y + 7.0f), 2, "%d.%03d", pInfo->m_approachPoint / 1000, pInfo->m_approachPoint % 1000);
		}
	} else {
		pGDI->Print(WPoint(pItem->pos.x + 147.0f, pItem->pos.y + 7.0f), 2, "%d", pInfo->m_score);
		pGDI->Print(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 7.0f), 2, "%I64d", pInfo->m_pang);

		if (pInfo->m_state == 2 || GetHoleIndex(pInfo->m_hole) == 19)
			pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 7.0f), 2, "End");
		else if (pInfo->m_state == 3)
			pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 7.0f), 2, "Out");
		else
			pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 7.0f), 2, "%d\xC8\xA6", GetHoleIndex(pInfo->m_hole));
	}

	pGDI->SetSpace(oldSpace);
}

// lobbymain.cpp:16552
void CLobbyMain::OnGameRoomExtRes_RoomUserRankLBtnUp(void) {
	FrListItem* item = m_userRankListBox->GetItemUnderCursor();

	if (!item) {
		return;
	}

	sGameResultInfo* info = (sGameResultInfo*)item->pData;
	if (!info) {
		return;
	}

	m_selectedUserId = info->m_uid;
	m_removeFriendUid = info->m_id;

	if (m_scoreDlg) {
		m_scoreDlg->SetPlayer(m_selectedUserId);
	} else {
		if (m_scoreBtn) {
			m_scoreBtn->Enable(true);
		}
		if (m_userActionBtn) {
			m_userActionBtn->Enable(true);
		}
	}
}

// lobbymain.cpp:16588
// Right click on the result screen's user ranking: selects the player (shown
// by an open score dialog) and opens its user info unless it skipped.
void CLobbyMain::OnGameRoomExtRes_RoomUserRankRBtnUp(void) {
	FrListItem* item = m_userRankListBox->GetItemUnderCursor();

	if (item) {
		sGameResultInfo* info = (sGameResultInfo*)item->pData;

		if (info) {
			m_selectedUserId = info->m_uid;
			m_removeFriendUid = info->m_id;

			if (m_scoreDlg) {
				m_scoreDlg->SetPlayer(m_selectedUserId);
			} else if (m_scoreBtn) {
				m_scoreBtn->Enable(true);
			}

			if (WSingleton<CUserInfo>::Instance() && info->m_state != 3) {
				CSharedDoc* doc = CSharedDoc::Instance();
				std::map<unsigned long, sBriefUserInfo>::iterator it = doc->m_userMap.find(info->m_uid);
				if (it != doc->m_userMap.end()) {
					it->second.roomNumber = doc->m_roomInfo.m_roomNumber;
				}
				WSingleton<CUserInfo>::Instance()->SetInfo(it->second.id, info->m_uid, true, true, false, false, it->second.nick);
			}
		}
	}
}

// lobbymain.cpp:16630
// The result screen's guild ranking (guild rooms only): one item per guild
// match pairing, and the two guilds' emblems.
void CLobbyMain::OnGameRoomExtRes_RoomGuildRankInit(PG_MSGARG param) {
	m_guildRankListBox = (FrListBox*)((__rtti_obj = (void*)param) != 0 ? (((IObject*)__rtti_obj)->IsKindOf(&FrListBox::m_RTTI) ? (void*)param : 0) : 0);
	if (m_guildRankListBox == 0)
		return;

	if (WSingleton<CSharedDoc>::Instance()->m_roomType != 6) {
		m_guildRankListBox->SetVisible(false);
		return;
	}

	m_guildRankListBox->UseRightButton(true);

	std::vector<sGuildMatchup>::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_vector4524.begin(); it != WSingleton<CSharedDoc>::Instance()->m_vector4524.end(); it++) {
		m_guildRankListBox->AddItem(&*it);
	}

	if (m_guildRankListBox->GetScrollBar()) {
		m_guildRankListBox->GetScrollBar()->SetGuideVisible(true);
	}

	m_guildMatchEmblem[0] = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_guild.m_guildMark[0]);
	m_guildMatchEmblem[1] = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_guild.m_guildMark[1]);

	m_guildRankBitmap = g_pFresh->RegisterBitmap("30in_rank_02");
}

// lobbymain.cpp:16671
void CLobbyMain::OnGameRoomExtRes_RoomGuildRankLBtnUp(void) {
	FrListItem* item = m_guildRankListBox->GetItemUnderCursor();

	if (!item) {
		return;
	}

	sGuildRankEntry* entry = (sGuildRankEntry*)item->pData;
	if (!entry) {
		return;
	}

	if (m_scoreDlg) {
		m_scoreDlg->SetGuildPlayer(entry->m_uid);
	} else {
		if (m_scoreBtn) {
			m_scoreBtn->Enable(true);
		}
	}
}

// lobbymain.cpp:16696
void CLobbyMain::OnGameRoomExtRes_RoomGuildRankRBtnUp(void) {
	OnGameRoomExtRes_RoomGuildRankLBtnUp();

	OnGameRoom_ScoreBtnUp();
}

// lobbymain.cpp:16712
// The result screen's guild ranking line: the pairing's rank, then each of
// the two players with the team emblem, nickname, unite guild index, pang
// and hole (the local player in red, a skipped one in grey).
void CLobbyMain::OnGameRoomExtRes_RoomGuildRankOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;

	if (!pItem)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	WRect rect(pItem->pos.x + 2.0f, pItem->pos.y, m_guildRankListBox->GetItemWidth() - 10.0f, (float)m_guildRankListBox->GetItemHeight());

	if (!pGDI)
		return;

	int oldSpace = pGDI->SetSpace(0);

	sGuildMatchup* pMatchup = (sGuildMatchup*)pItem->pData;

	if (m_selectedUserId == pMatchup->m_guidA || m_selectedUserId == pMatchup->m_guidB)
		pGDI->Box(WRect(rect.x, rect.y + 2.0f, rect.w + 20.0f, rect.h - 2.0f), 0x999B9DFF, 0, 0);

	pGDI->SetTextStyle(0);
	pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);

	if (m_guildRankBitmap) {
		pGDI->DrawTexture(m_guildRankBitmap, WRect(rect.x - 3.0f, rect.y + 6.0f, m_guildRankBitmap->Width(), m_guildRankBitmap->Height()), 0xFFFFFFFF, 0);
		pGDI->Print(WPoint(pItem->pos.x + 13.0f, pItem->pos.y + 10.0f), 1, "%d \xC1\xB6", pMatchup->m_kind);
	}

	sRivalData* pInfo = &WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(pMatchup->m_guidA)];

	if (m_guildMatchEmblem[0])
		pGDI->DrawTexture(m_guildMatchEmblem[0], WRect(pItem->pos.x + 34.0f, pItem->pos.y + 3.0f, m_guildMatchEmblem[0]->Width() / 2, m_guildMatchEmblem[0]->Height() / 2), 0xFFFFFFFF, 0);

	if (pInfo->m_uid == MyGuid(true))
		pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	else if (pInfo->m_state == 3)
		pGDI->SetTextColor(0xFF808080, 0xFFFFFFFF);
	else
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	if (!WSingleton<CProjectG>::Instance()->HidePrivacy())
		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 47.0f, pItem->pos.y + 3.0f), 0, pInfo->m_nickname, 80.0f, 0xFFFFFFFF);

	pGDI->Print(WPoint(pItem->pos.x + 147.0f, pItem->pos.y + 3.0f), 2, "%d", pInfo->m_uniteGuildIndex);
	pGDI->Print(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 3.0f), 2, "%I64d", pInfo->m_pang);

	if (pInfo->m_state == 2 || GetHoleIndex(pInfo->m_hole) == 19)
		pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 3.0f), 2, "End");
	else if (pInfo->m_state == 3)
		pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 3.0f), 2, "Out");
	else
		pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 3.0f), 2, "%d\xC8\xA6", GetHoleIndex(pInfo->m_hole));

	pInfo = &WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(pMatchup->m_guidB)];

	if (m_guildMatchEmblem[1])
		pGDI->DrawTexture(m_guildMatchEmblem[1], WRect(pItem->pos.x + 34.0f, pItem->pos.y + 21.0f, m_guildMatchEmblem[1]->Width() / 2, m_guildMatchEmblem[1]->Height() / 2), 0xFFFFFFFF, 0);

	if (pInfo->m_uid == MyGuid(true))
		pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	else if (pInfo->m_state == 3)
		pGDI->SetTextColor(0xFF808080, 0xFFFFFFFF);
	else
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	if (!WSingleton<CProjectG>::Instance()->HidePrivacy())
		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 47.0f, pItem->pos.y + 20.0f), 0, pInfo->m_nickname, 80.0f, 0xFFFFFFFF);

	pGDI->Print(WPoint(pItem->pos.x + 147.0f, pItem->pos.y + 20.0f), 2, "%d", pInfo->m_uniteGuildIndex);
	pGDI->Print(WPoint(pItem->pos.x + 182.0f, pItem->pos.y + 20.0f), 2, "%I64d", pInfo->m_pang);

	if (pInfo->m_state == 2 || GetHoleIndex(pInfo->m_hole) == 19)
		pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 20.0f), 2, "End");
	else if (pInfo->m_state == 3)
		pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 20.0f), 2, "Out");
	else
		pGDI->Print(WPoint(pItem->pos.x + 220.0f, pItem->pos.y + 20.0f), 2, "%d\xC8\xA6", GetHoleIndex(pInfo->m_hole));

	pGDI->SetSpace(oldSpace);
}

// lobbymain.cpp:16820
void CLobbyMain::OnGameRoomExt_UserInfoInit(intptr_t param) {
	m_userActionBtn = DYNAMIC_CAST(FrButton, param);
	if (m_userActionBtn) {
		m_userActionBtn->Enable(false);
	}
}

// lobbymain.cpp:16830
void CLobbyMain::OnGameRoomExt_UserInfoLBtnUp(void) {
	if (m_userInfoLocked == 0) {
		if (m_selectedUserId == 0xFFFFFFFF) {
			m_userActionBtn->Enable(false);
			return;
		}

		if (WSingleton<CUserInfo>::Instance()) {
			CSharedDoc* doc = CSharedDoc::Instance();
			std::map<unsigned long, sBriefUserInfo>::iterator it = doc->m_userMap.find(m_selectedUserId);
			if (it != doc->m_userMap.end()) {
				WSingleton<CUserInfo>::Instance()->SetInfo(it->second.id, m_selectedUserId, true, true, false, false, it->second.nick);
			}
		}
	}
}

// lobbymain.cpp:16878
void CLobbyMain::OnGameRoomExtRes_Init(PG_MSGARG) {
	CTaskMain::OnInit();

	m_away = false;
	g_mouse->ResetInputTime();
	g_ime->ResetInputTime();

	if (CSharedDoc::Instance()->m_roomInfo.m_roomMode == 14) {
		g_pFresh->GetDesktop()->SetWallPaper("chaos_background.jpg", true);
	} else {
		g_pFresh->GetDesktop()->SetWallPaper("gameroom_background.jpg", true);
	}

	m_roomOptionA = false;
	m_roomOptionC = false;
	m_roomOptionD = false;

	if (CSharedDoc::Instance()->GetPangyaUserInfo().identity == 2) {
		m_roomOptionA = true;
		m_roomOptionC = true;
	}

	m_roomOptionB = false;
	m_selectedUserId = 0xFFFFFFFF;
	m_removeFriendUid = 0xFFFFFFFF;

	SetTimeVariableBGM();
}

// lobbymain.cpp:16912
void CLobbyMain::OnGameRoomExtRes_Finish(void) {
	m_titleArea->SetBgImg("title_mass");

	EnableUnderBar(false);
	m_barButton[15]->Enable(true);

	OpenNewRecordForm();
}

// lobbymain.cpp:16922
void CLobbyMain::OnGameRoomExtRes_Destroy(void) {
	EnableUnderBar(true);
}

// lobbymain.cpp:16932
void CLobbyMain::OnCreate_Init(PG_MSGARG) {
	CTaskMain::OnInit();
}

// lobbymain.cpp:16937
void CLobbyMain::OnCreate_Finish(void) {
}

// lobbymain.cpp:16942
void CLobbyMain::OnCreate_Destroy(void) {
	if (m_createNickDlg != 0) {
		m_createNickDlg->CloseDlg(1);
		m_createNickDlg = 0;
	}
}

// lobbymain.cpp:16951
void CLobbyMain::OnCreatePetInit(PG_MSGARG param) {
	m_petArea[0] = (FrArea*)((__rtti_obj = (void*)param) != 0 ? (((IObject*)__rtti_obj)->IsKindOf(&FrArea::m_RTTI) ? (void*)param : 0) : 0);
	m_createCharType = ((WSingleton<CSharedDoc>::Instance()->m_userInfo.eventFlags >> 1) & 1) % 2;
	SetFirstChar(&WSingleton<CSharedDoc>::Instance()->GetMyPartTidList());

	sRoomSlot slot;
	slot.m_pPetArea = m_petArea[0];
	slot.m_pPet = NULL;
	slot.m_partTidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

	// lobbymain.cpp:16980, the creation screen's character from the part list
	slot.m_characterInfo.typeId = slot.m_partTidList.m_tidChar;
	slot.m_characterInfo.hairColor = slot.m_partTidList.m_hairClr;
	slot.m_characterInfo.shirtColor = (slot.m_partTidList.m_shirtsClr & 0xF) | 0x10;
	slot.m_angelFlags.flag0 = 0;
	slot.m_angelFlags.flag1 = 0;
	memcpy(slot.m_characterInfo.partTypeIds, slot.m_partTidList.m_partTids, sizeof(slot.m_characterInfo.partTypeIds));
	std::map<unsigned long, sRoomSlot>::iterator it = WSingleton<CSharedDoc>::Instance()->m_roomSlotMap.find(MyGuid(false));
	if (it != WSingleton<CSharedDoc>::Instance()->m_roomSlotMap.end() && it->second.m_pPet) {
		delete it->second.m_pPet;
		it->second.m_pPet = NULL;
	}
	WSingleton<CSharedDoc>::Instance()->m_roomSlotMap[MyGuid(false)] = slot;
	ShowPet(MyGuid(false));
}

// lobbymain.cpp:16984
void CLobbyMain::OnCreatePetOwnerDraw(void) {
	if (m_bongdariShop[2]) { // 0x122, the same byte CTaskMain names as the third Bongdari shop flag
		return;
	}

	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	int guid = MyGuid(false);
	std::map<unsigned long, sRoomSlot>::iterator it = doc->m_roomSlotMap.find(guid);
	if (it != doc->m_roomSlotMap.end()) {
		if (doc->m_roomSlotMap[guid].m_pPet) {
			WSingleton<CSharedDoc>::Instance()->m_roomSlotMap[MyGuid(false)].m_pPet->Display(0.0f, 0.0f);
		}
	}
}

// lobbymain.cpp:16999
void CLobbyMain::OnCreate_CharInit(intptr_t param) {
	m_createCharEdit = DYNAMIC_CAST(FrEdit, param);
}

// lobbymain.cpp:17004
bool CLobbyMain::SetFirstChar(CPartTidList* pTidList) {
	IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(0x4000000 | m_createCharType);
	if (!pChar) {
		return false;
	}

	pTidList->m_tidChar = 0x4000000 | m_createCharType;
	pTidList->m_unk04 = pChar->m_numParts;
	pTidList->m_numTotalParts = pChar->m_numParts + pChar->m_numExtraParts;
	pTidList->m_hairClr = 0;
	pTidList->m_shirtsClr = 0;

	pTidList->SetDefaultTids();

	if (m_createCharEdit) {
		m_createCharEdit->SetLine(1, pChar->m_name, 0, false, 0);
	}

	if (m_createDescEdit) {
		m_createDescEdit->ClearLine();

		IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(0x4000000 | m_createCharType);
		if (pDesc) {
			m_createDescEdit->AddText(pDesc->m_text, false, true);
		}
	}

	return true;
}

// lobbymain.cpp:17038
void CLobbyMain::OnCreate_CharPrevInit(intptr_t param) {
	FrButton* pButton = DYNAMIC_CAST(FrButton, param);
	if (pButton) {
		pButton->SetPushDelay(0.0f);
	}
}

// lobbymain.cpp:17050
void CLobbyMain::OnCreate_CharNextInit(intptr_t param) {
	FrButton* pButton = DYNAMIC_CAST(FrButton, param);
	if (pButton) {
		pButton->SetPushDelay(0.0f);
	}
}

// lobbymain.cpp:17062
void CLobbyMain::OnCreate_CharBtnUp(void) {
	CPartTidList& tidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

	m_createCharType = (m_createCharType + 1) % 2;

	if (!SetFirstChar(&tidList)) {
		m_createCharType = (m_createCharType + 1) % 2;
		SetFirstChar(&tidList);
		return;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomSlotMap[MyGuid(false)].m_pPet) {
		WSingleton<CSharedDoc>::Instance()->m_roomSlotMap[MyGuid(false)].m_pPet->SetModel(tidList, 0, 0, 3.0f, 0);
	}

	m_createHairColor = m_createShirtsColor = 0;

	if (m_createHairEdit) {
		m_createHairEdit->SetLine(1, "\xb8\xd3\xb8\xae\xbb\xf6" "1", 0, false, 0); // "hair color 1" (CP949)
	}
	if (m_createShirtsEdit) {
		m_createShirtsEdit->SetLine(1, "\xbc\xc5\xc3\xf7\xbb\xf6" "1", 0, false, 0); // "shirts color 1" (CP949)
	}

	SetLevelBar();
}

// lobbymain.cpp:17088
void CLobbyMain::OnCreate_HairInit(intptr_t param) {
	m_createHairColor = 0;
	m_createHairEdit = DYNAMIC_CAST(FrEdit, param);
	if (m_createHairEdit) {
		m_createHairEdit->SetLine(1, "\xb8\xd3\xb8\xae\xbb\xf6" "1", 0, false, 0); // "hair color 1" (CP949)
	}
}

// lobbymain.cpp:17096
void CLobbyMain::OnCreate_HairPrevInit(intptr_t param) {
	FrButton* pButton = DYNAMIC_CAST(FrButton, param);
	if (pButton) {
		pButton->SetPushDelay(0.0f);
	}
}

// lobbymain.cpp:17103
void CLobbyMain::OnCreate_HairPrevBtnUp(void) {
	CPartTidList& tidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

	if (m_createHairColor == 0) {
		m_createHairColor = 2;
	} else {
		m_createHairColor -= 1;
	}

	if (m_createHairEdit) {
		m_createHairEdit->SetLine(1, MakeStr("\xb8\xd3\xb8\xae\xbb\xf6" "%d", m_createHairColor + 1), 0, false, 0);
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomSlotMap[MyGuid(false)].m_pPet) {
		WSingleton<CSharedDoc>::Instance()->m_roomSlotMap[MyGuid(false)].m_pPet->GetPetFrame()->SetHairClr(&tidList, m_createHairColor);
	} else {
		tidList.m_hairClr = m_createHairColor;
	}
}

// lobbymain.cpp:17125
void CLobbyMain::OnCreate_HairNextInit(intptr_t param) {
	FrButton* pButton = DYNAMIC_CAST(FrButton, param);
	if (pButton) {
		pButton->SetPushDelay(0.0f);
	}
}

// lobbymain.cpp:17132
void CLobbyMain::OnCreate_HairNextBtnUp(void) {
	CPartTidList& tidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

	if (m_createHairColor == 2) {
		m_createHairColor = 0;
	} else {
		m_createHairColor += 1;
	}

	if (m_createHairEdit) {
		m_createHairEdit->SetLine(1, MakeStr("\xb8\xd3\xb8\xae\xbb\xf6" "%d", m_createHairColor + 1), 0, false, 0);
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomSlotMap[MyGuid(false)].m_pPet) {
		WSingleton<CSharedDoc>::Instance()->m_roomSlotMap[MyGuid(false)].m_pPet->GetPetFrame()->SetHairClr(&tidList, m_createHairColor);
	} else {
		tidList.m_hairClr = m_createHairColor;
	}
}

// lobbymain.cpp:17154
void CLobbyMain::OnCreate_ShirtsInit(intptr_t param) {
	m_createShirtsEdit = DYNAMIC_CAST(FrEdit, param);
	m_createShirtsColor = 0;
	if (m_createShirtsEdit) {
		m_createShirtsEdit->SetLine(1, "\xbc\xc5\xc3\xf7\xbb\xf6" "1", 0, false, 0); // "shirts color 1" (CP949)
	}
}

// lobbymain.cpp:17163
void CLobbyMain::OnCreate_ShirtsPrevInit(intptr_t param) {
	FrButton* pButton = DYNAMIC_CAST(FrButton, param);
	if (pButton) {
		pButton->SetPushDelay(0.0f);
	}
}

// lobbymain.cpp:17170
void CLobbyMain::OnCreate_ShirtsPrevBtnUp(void) {
	CPartTidList& tidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

	m_createShirtsColor = m_createShirtsColor == 0 ? 2 : --m_createShirtsColor;

	if (m_createShirtsEdit) {
		m_createShirtsEdit->SetLine(1, MakeStr("\xbc\xc5\xc3\xf7\xbb\xf6" "%d", m_createShirtsColor + 1), 0, false, 0);
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomSlotMap[MyGuid(false)].m_pPet) {
		WSingleton<CSharedDoc>::Instance()->m_roomSlotMap[MyGuid(false)].m_pPet->GetPetFrame()->SetShirtsClr(&tidList, m_createShirtsColor);
	} else {
		tidList.m_shirtsClr = m_createShirtsColor;
	}
}

// lobbymain.cpp:17184
void CLobbyMain::OnCreate_ShirtsNextInit(intptr_t param) {
	FrButton* pButton = DYNAMIC_CAST(FrButton, param);
	if (pButton) {
		pButton->SetPushDelay(0.0f);
	}
}

// lobbymain.cpp:17191
void CLobbyMain::OnCreate_ShirtsNextBtnUp(void) {
	CPartTidList& tidList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();

	m_createShirtsColor = m_createShirtsColor == 2 ? 0 : ++m_createShirtsColor;

	if (m_createShirtsEdit) {
		m_createShirtsEdit->SetLine(1, MakeStr("\xbc\xc5\xc3\xf7\xbb\xf6" "%d", m_createShirtsColor + 1), 0, false, 0);
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomSlotMap[MyGuid(false)].m_pPet) {
		WSingleton<CSharedDoc>::Instance()->m_roomSlotMap[MyGuid(false)].m_pPet->GetPetFrame()->SetShirtsClr(&tidList, m_createShirtsColor);
	} else {
		tidList.m_shirtsClr = m_createShirtsColor;
	}
}

// lobbymain.cpp:17205
void CLobbyMain::OnCreate_CreateBtnUp(void) {
	sCreateCharacter data;
	data.m_typeId = m_createCharType | 0x4000000;
	data.m_colors = (m_createHairColor & 0xF) | (m_createShirtsColor << 4);
	WSendPacket packet(LC_CREATE_CHARACTER);
	packet.EncodeBuffer(&data, sizeof(data));
	packet.Send(SEND_TO_LOGIN);
	WSingleton<CSharedDoc>::Instance()->m_paranNickChanged = true;
	MsgObject msg(NULL, 1, 0, 0, 0, 0, 0);
	HandleMsg(msg);
}

// lobbymain.cpp:17225
void CLobbyMain::OnCreate_DescInit(intptr_t param) {
	m_createDescEdit = DYNAMIC_CAST(FrEdit, param);
}

// lobbymain.cpp:17230
void CLobbyMain::OnCreate_WarningInit(intptr_t param) {
	FrEdit* pEdit = DYNAMIC_CAST(FrEdit, param);

	pEdit->AddText("\\c0xffff0000\\c* \xc7\xd1 \xb9\xf8 \xbc\xb1\xc5\xc3\xc7\xd1 \xc4\xb3\xb8\xaf\xc5\xcd\xb4\xc2 \xb3\xaa\xc1\xdf\xbf\xa1 \xb9\xd9\xb2\xdc \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.\\c0xff000000\\c", false, true);
	pEdit->AddText("  \xbd\xc5\xc1\xdf\xc8\xf7 \xbc\xb1\xc5\xc3\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.", false, true);
}

// lobbymain.cpp:17238
void CLobbyMain::On_PowerBarInit(intptr_t param) {
	m_powerBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_powerBar->SetRange(0, 50, 0);
}

// lobbymain.cpp:17244
void CLobbyMain::On_ControlBarInit(intptr_t param) {
	m_controlBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_controlBar->SetRange(0, 30, 0);
}

// lobbymain.cpp:17250
void CLobbyMain::On_AccuracyBarInit(intptr_t param) {
	m_accuracyBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_accuracyBar->SetRange(0, 30, 0);
}

// lobbymain.cpp:17256
void CLobbyMain::On_SpinBarInit(intptr_t param) {
	m_spinBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_spinBar->SetRange(0, 30, 0);
}

// lobbymain.cpp:17262
void CLobbyMain::On_CurveBarInit(intptr_t param) {
	m_curveBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_curveBar->SetRange(0, 30, 0);
}

// lobbymain.cpp:17269
bool CLobbyMain::OnCreateNickResult(int ret, FrForm*) {
	m_createNickDlg = 0;

	if (ret == 0) {
	} else if (ret == 1) {
		if (IsLocalContent((localContentType_t)0x81) && m_serverDlg) {
			m_serverDlg->ClickDisableList(true);
		}
	}

	return true;
}

// lobbymain.cpp:17290
bool CLobbyMain::OnMissionEventResult(int, FrForm*) {
	if (!IsLocalContent((localContentType_t)0x83)) {
		return false;
	}
	m_missionEventDlg = 0;
	return true;
}

// lobbymain.cpp:17303
bool CLobbyMain::OnBingoEventResult(int, FrForm*) {
	if (!IsLocalContent((localContentType_t)0x85)) {
		return false;
	}
	m_bingoEventDlg = 0;
	return true;
}

// lobbymain.cpp:17315
bool CLobbyMain::OnKoohBirthdayEventDlg(int, FrForm*) {
	if (!IsLocalContent((localContentType_t)0x8E)) {
		return false;
	}
	m_toppageKoohBirthdayBtn = 0;
	return true;
}

// lobbymain.cpp:17329
void CLobbyMain::JoinOtherServerRoom(void) {
}

// lobbymain.cpp:17368
// Answer to a match request (packet 0xB8 to the requester): 1 accepts, 2
// refuses, anything else (the time limit ran out) refuses and says so.
bool CLobbyMain::OnMatchingDlgResult(int result, FrForm*) {
	if (!IsLocalContent((localContentType_t)0x59))
		return false;

	if (m_matchingDlg == 0)
		return false;

	unsigned char accept = 0;
	switch (result) {
	case 1:
		accept = 1;
		break;
	case 2:
		break;
	default:
		// CP949: "The matching was cancelled."
		AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 0x23,
			(intptr_t)"\xB8\xC5\xC4\xAA\xC0\xCC \xC3\xEB\xBC\xD2\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
		break;
	}

	WSendPacket packet((enumClientPacket)0xB8);
	packet.Encode4(m_matchingDlg->GetFromUID());
	packet.Encode1(accept);
	packet.Send(SEND_TO_DEFAULT);

	m_matchingDlg = 0;
	return true;
}

// lobbymain.cpp:17405
// Applies up to ten downloaded UCC clothes to the avatars of both slot lists
// and of the game room's user list.
void CLobbyMain::RefreshAllUccClothes(void) {
	if (CUserInfo::Instance()->GetDlg())
		return;

	sUccRefreshItem item;

	for (int i = 0; i < 10; i++) {
		if (!UccManager()->GetRefreshItem(item))
			return;

		std::list<sSlotInfo*>::iterator it;
		for (it = m_slotList2.begin(); it != m_slotList2.end(); ++it)
			_RefreshUccClothes(*it, item.m_typeId, item.m_uccKey);

		for (it = m_slotList1.begin(); it != m_slotList1.end(); ++it)
			_RefreshUccClothes(*it, item.m_typeId, item.m_uccKey);

		if (m_roomUserListBox) {
			for (int j = 0; j < m_roomUserListBox->GetCurrentItemSize(false); j++) {
				FrListItem* listItem = m_roomUserListBox->GetItem(j);
				if (listItem)
					_RefreshUccClothes((sSlotInfo*)listItem->pData, item.m_typeId, item.m_uccKey);
			}
		}
	}
}

// lobbymain.cpp:17445
void CLobbyMain::OnToppage_UCCShopBtnUp(void) {
	CTaskManager::Instance()->ChangeTask("CShopTask", "", false);
	CTaskManager::Instance()->PostMsg(0, "Shop", 0, (intptr_t)"SHOPMAIN", 0, 0, 0);
	CTaskManager::Instance()->PostMsg(0, "Shop", 0xC6, 3, 0, 0, 0);
}

// lobbymain.cpp:17452
void CLobbyMain::OnToppage_CardShopBtnUp(void) {
	CTaskManager::Instance()->ChangeTask("CShopTask", "", false);
	CTaskManager::Instance()->PostMsg(0, "Shop", 0, (intptr_t)"SHOPMAIN", 0, 0, 0);
	CTaskManager::Instance()->PostMsg(0, "Shop", 0xC6, 4, 0, 0, 0);
}

// lobbymain.cpp:17459
float CLobbyMain::GetTopPageBtnRect(void) {
	if (IsLocalContent((localContentType_t)0x6A)) {
		if (CLoginInfo::Instance()->IsPcBang()) {
			return m_topPageBtnIndex++ * 80.0f;
		}
		return m_topPageBtnIndex++ * 80.0f;
	}
	return 0.0f;
}

// lobbymain.cpp:17484
// Decides whether the game result screen offers the use report form.
void CLobbyMain::CheckTikiReport(const char* layout) {
	if (IsLocalContent((localContentType_t)0x14)) {
		if (strcmp(layout, "GAMEROOM_EXTRES") == 0) {
			CSharedDoc* doc = CSharedDoc::Instance();

			if ((doc->GetPangyaUserInfo().identity & 2) || (doc->GetPangyaUserInfo().identity & 4)
				|| (doc->GetPangyaUserInfo().identity & 8)) {
				m_useReportEnabled = false;
			} else {
				for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_itemInfoList.begin();
					it != CSharedDoc::Instance()->m_itemInfoList.end(); ++it) {
					sItemInfo item = *it;

					if (item.m_typeId == 0x1A000041) {
						m_useReportEnabled = true;
						break;
					}

					m_useReportEnabled = false;
				}
			}

			if (CSharedDoc::Instance()->m_roomInfo.m_gameType == 6)
				m_useReportEnabled = false;

			if (CSharedDoc::Instance()->m_roomInfo.m_userLimit > 30)
				m_useReportEnabled = false;

			if (m_resultReportDlg != NULL)
				m_useReportEnabled = false;

			if (CSharedDoc::Instance()->m_currentChannel.flags & 0x800)
				m_useReportEnabled = false;

			if (CSharedDoc::Instance()->m_myLevel <= 5)
				m_useReportEnabled = false;
		}
	}
}

// lobbymain.cpp:18052
void CLobbyMain::OnToppage_TopIcon1Init(PG_MSGARG param) {
	CIconManager::Instance()->InitalizeTopIcon(0, param);
}

// lobbymain.cpp:18056
void CLobbyMain::OnToppage_TopIcon1Up(void) {
	CloseAllDialogs();
	CIconManager::Instance()->OnButtonDown(0);
}

// lobbymain.cpp:18062
void CLobbyMain::OnToppage_TopIcon2Init(PG_MSGARG param) {
	CIconManager::Instance()->InitalizeTopIcon(1, param);
}

// lobbymain.cpp:18066
void CLobbyMain::OnToppage_TopIcon2Up(void) {
	CloseAllDialogs();
	CIconManager::Instance()->OnButtonDown(1);
}

// lobbymain.cpp:18072
void CLobbyMain::OnToppage_TopIcon3Init(PG_MSGARG param) {
	CIconManager::Instance()->InitalizeTopIcon(2, param);
}

// lobbymain.cpp:18076
void CLobbyMain::OnToppage_TopIcon3Up(void) {
	CloseAllDialogs();
	CIconManager::Instance()->OnButtonDown(2);
}

// lobbymain.cpp:18082
void CLobbyMain::OnToppage_TopIcon4Init(PG_MSGARG param) {
	CIconManager::Instance()->InitalizeTopIcon(3, param);
}

// lobbymain.cpp:18087
void CLobbyMain::OnToppage_TopIcon4Up(void) {
	CloseAllDialogs();
	CIconManager::Instance()->OnButtonDown(3);
}

// lobbymain.cpp:18093
void CLobbyMain::OnToppage_TopIcon5Init(PG_MSGARG param) {
	CIconManager::Instance()->InitalizeTopIcon(4, param);
}

// lobbymain.cpp:18098
void CLobbyMain::OnToppage_TopIcon5Up(void) {
	CloseAllDialogs();
	CIconManager::Instance()->OnButtonDown(4);
}

// lobbymain.cpp:18104
void CLobbyMain::OnToppage_TopIcon6Init(PG_MSGARG param) {
	CIconManager::Instance()->InitalizeTopIcon(5, param);
}

// lobbymain.cpp:18109
void CLobbyMain::OnToppage_TopIcon6Up(void) {
	CloseAllDialogs();
	CIconManager::Instance()->OnButtonDown(5);
}
