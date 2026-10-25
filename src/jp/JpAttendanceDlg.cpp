// JP-only game code: FrAttendanceDlg, the lobby's daily attendance dialog
// (JpAttendanceDlg.h). JP VAs before every definition.
#include "JpAttendanceDlg.h"

#include <string>

#include "LobbyTask.h"
#include "NetworkSystem.h"
#include "ObjectFactory.h"
#include "Packet.h"
#include "SharedDoc.h"
#include "TaskManager.h"
#include "TitleFontEx.h"
#include "actor.h"
#include "client/Wangreal/include/wfont.h"
#include "client/Wangreal/include/wresrcmng.h"
#include "messengerdlg.h"
#include "commonutil.h"
#include "bitmap.h"
#include "frarea.h"
#include "fresh.h"
#include "frgraphicinterface.h"
#include "frwnd.h"
#include "frwndmanager.h"
#include "shared/luasystem/LuaScript.h"
#include "shared/luasystem/LuaScriptManager.h"
#include "shared/luasystem/luatablehelper.h"

#include <string.h>

// The KR g_bQuit (ProjectG.cpp), JP 0xf02419.
extern bool g_bQuit;

// 0xe73df4 (run-time type, base FrForm)
const WRTTI FrAttendanceDlg::m_RTTI("FrAttendanceDlg", &FrForm::m_RTTI);

// 0x47c940
const sFRESH_MSGMAP* FrAttendanceDlg::GetMessageMap(void) const {
	return &FrAttendanceDlg::_MsgMap;
}

sFRESH_MSGMAP FrAttendanceDlg::_MsgMap = { &FrForm::_MsgMap, &FrAttendanceDlg::_MsgEntries[0] };

// 0xe034b8 (the handlers are stored by the static init 0xcc7ba0)
sFRESH_ENTRY FrAttendanceDlg::_MsgEntries[] = {
	{ "image_area", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAttendanceDlg::OnImageAreaInit },
	{ "image_area", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAttendanceDlg::OnImageAreaDraw },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x47c950 (0x47cee0 is the deleting destructor; 0x47cb10, 0x47cb20 the
// other bases' thunks)
FrAttendanceDlg::~FrAttendanceDlg(void) {
	if (m_pDaysFont) {
		delete m_pDaysFont;
		m_pDaysFont = 0;
	}
	if (m_pCountFont) {
		delete m_pCountFont;
		m_pCountFont = 0;
	}
}

// 0x47ca00
void FrAttendanceDlg::LoadBitmaps(void) {
	if (m_kind == 0) {
		m_images[4] = g_pFresh->GetBitmap("check_in_Normal");
		m_images[0] = g_pFresh->GetBitmap("check_item_today");
		m_buttonHoverImg = g_pFresh->GetBitmap("check_in_Hover");
	} else if (m_kind == 1) {
		m_images[4] = g_pFresh->GetBitmap("check_out_Normal");
		m_images[0] = g_pFresh->GetBitmap("check_item_next");
	}
	m_images[1] = g_pFresh->GetBitmap("check_out_countdown1");
	m_images[2] = g_pFresh->GetBitmap("check_out_countdown2");
	m_images[3] = g_pFresh->GetBitmap("check_out_countdown3");
}

// 0x47cad0
void FrAttendanceDlg::UpdateCount(void) {
	if (m_kind == 0) {
		m_count = m_info.todayCount;
	} else if (m_kind == 1) {
		m_count = m_info.nextCount;
	}
}

// 0x47ce10
FrAttendanceDlg::FrAttendanceDlg(void) {
	m_pImageArea = 0;
	m_info.status = false;
	m_info.todayItemTypeId = 0;
	m_info.todayCount = 0;
	m_info.nextItemTypeId = 0;
	m_info.nextCount = 0;
	m_info.days = -1;
	m_daysPos.x = 0.0f;
	m_daysPos.y = 0.0f;
	m_buttonHoverImg = 0;
	m_itemPos.x = 0.0f;
	m_itemIcon = 0;
	m_itemPos.y = 0.0f;
	m_buttonHover = false;
	m_hasInfo = false;
	m_openedByPacket = false;
	m_elapsed = 0.0f;
	m_pDaysFont = 0;
	m_pCountFont = 0;
	m_mode = -1;
	m_kind = -1;
	m_countdown = -1;
	m_count = (unsigned int)-1;
	m_images[0] = 0;
	m_images[1] = 0;
	m_images[2] = 0;
	m_images[3] = 0;
	m_images[4] = 0;
}

// 0x47cf10
void FrAttendanceDlg::OnImageAreaInit(intptr_t param) {
	m_pImageArea = DYNAMIC_CAST(FrArea, param);
	m_origin.x = m_pImageArea->GetRect().x;
	m_origin.y = m_pImageArea->GetRect().y;
	m_originOffset.x = 0.0f;
	m_originOffset.y = 0.0f;
}

// 0x47cfa0
void FrAttendanceDlg::OnImageAreaDraw(intptr_t param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI) {
		pGDI->DrawTexture(m_buttonHover ? m_buttonHoverImg : m_images[4], m_buttonRect, 0xffffffff, 0);
		pGDI->DrawTexture(m_images[0], m_itemRect, 0xffffffff, 0);
		if (m_countdown != -1 && m_countdown >= 0 && m_countdown < 5) {
			pGDI->DrawTexture(m_images[m_countdown], m_buttonRect, 0xffffffff, 0);
		}
		if (m_pImageArea) {
			if (m_itemIcon) {
				WRect rect(m_itemPos.x, m_itemPos.y, (float)m_itemIcon->Width(), (float)m_itemIcon->Height());
				pGDI->DrawTexture(m_itemIcon, rect, 0xffffffff, 0);
			}
			if (m_pCountFont) {
				m_pCountFont->Print(m_itemPos.x + 45.0f, m_itemPos.y + 30.0f, MakeStr("x%d", m_count), TITLEFONT_ALIGN_RIGHT, 0.0f, 1.0f);
			}
			if (m_hasInfo && m_pDaysFont) {
				m_pDaysFont->Print(m_daysPos.x, m_daysPos.y, MakeStr("%04d", m_info.days), TITLEFONT_ALIGN_RIGHT, 3.0f, 1.0f);
			}
		}
	}

	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
	if (m_tooltipRect.x <= mouse.x && mouse.x <= m_tooltipRect.x + m_tooltipRect.w
		&& m_tooltipRect.y <= mouse.y && mouse.y <= m_tooltipRect.y + m_tooltipRect.h) {
		m_pImageArea->EnableToolTip(true);
		return;
	}
	m_pImageArea->EnableToolTip(false);
}

// 0x47d250
IObject* FrAttendanceDlgMakeInstance(void) {
	return new FrAttendanceDlg;
}

// 0x47d2c0, 0xcc7b80 (static init)
struct __sFrAttendanceDlg {
	__sFrAttendanceDlg(void) {
		ObjectFactory().AddObjectFunctor(FrAttendanceDlgMakeInstance, "FrAttendanceDlg");
	}
};
static __sFrAttendanceDlg s_registerFrAttendanceDlg;

// 0x47d2e0
void FrAttendanceDlg::UpdateItemIcon(void) {
	const char* icon = 0;
	if (m_kind == 0) {
		icon = JpAttendance_FindItemIcon(m_info.todayItemTypeId);
	} else if (m_kind == 1) {
		icon = JpAttendance_FindItemIcon(m_info.nextItemTypeId);
	} else {
		return;
	}
	if (icon) {
		m_itemIcon = g_pFresh->GetBitmap(icon);
	}
}

// 0x47d330: a fresh script runs "attendance.lua" (JP 0xc1ff80 the
// manager's instance, 0xc1feb0 CreateScript(false), 0xc20530 RunFile,
// 0xc20240 GetTable, 0x470e60 the int field read, 0xc1f430 ReleaseScript).
// The JP reads the table whatever the script.
void FrAttendanceDlg::LoadLayoutConfig(void) {
	lua_system::lua_script* script = lua_system::lua_script_manger::GetInstance()->CreateScript(false);
	if (script) {
		script->RunFile("attendance.lua");
	}
	lua_system::lua_table_helper table = script->GetTable("attendance_ui_config");
	m_buttonRect.x = (float)table.GetValueByName<int>("checkin_or_checkout_x") + m_origin.x;
	m_buttonRect.y = (float)table.GetValueByName<int>("checkin_or_checkout_y") + m_origin.y;
	m_buttonRect.w = (float)table.GetValueByName<int>("checkin_or_checkout_w");
	m_buttonRect.h = (float)table.GetValueByName<int>("checkin_or_checkout_h");
	m_itemRect.x = (float)table.GetValueByName<int>("todayitem_or_tomorrowitem_x") + m_origin.x;
	m_itemRect.y = (float)table.GetValueByName<int>("todayitem_or_tomorrowitem_y") + m_origin.y;
	m_itemRect.w = (float)table.GetValueByName<int>("todayitem_or_tomorrowitem_w");
	m_itemRect.h = (float)table.GetValueByName<int>("todayitem_or_tomorrowitem_h");
	m_daysPos.x = (float)table.GetValueByName<int>("font_x") + m_origin.x;
	m_daysPos.y = (float)table.GetValueByName<int>("font_y") + m_origin.y;
	m_itemPos.x = (float)table.GetValueByName<int>("item_x") + m_origin.x;
	m_itemPos.y = (float)table.GetValueByName<int>("item_y") + m_origin.y;
	m_tooltipRect.x = (float)table.GetValueByName<int>("itemtooltip_x") + m_origin.x;
	m_tooltipRect.y = (float)table.GetValueByName<int>("itemtooltip_y") + m_origin.y;
	m_tooltipRect.w = (float)table.GetValueByName<int>("itemtooltip_w");
	m_tooltipRect.h = (float)table.GetValueByName<int>("itemtooltip_h");
	lua_system::lua_script_manger::GetInstance()->ReleaseScript(script);
}

// 0x47d590
bool FrAttendanceDlg::OnInit(void) {
	LoadLayoutConfig();
	LoadBitmaps();
	m_pDaysFont = JpAttendance_NewDaysFont();
	m_pCountFont = new CWindFont;
	return FrForm::OnInit();
}

// 0x47d640
void FrAttendanceDlg::UpdateItemTooltip(void) {
	if (m_kind == 0) {
		const char* name = JpAttendance_FindItemName(m_info.todayItemTypeId);
		if (name && m_pImageArea) {
			m_pImageArea->SetToolTipText(std::string(name));
		}
	} else if (m_kind == 1) {
		const char* name = JpAttendance_FindItemName(m_info.nextItemTypeId);
		if (name && m_pImageArea) {
			m_pImageArea->SetToolTipText(std::string(name));
		}
	}
}

// 0x47d770 (the JP 0xadc360 is the same body as the JP FrLogoutDlg's
// logout procedure). The JP current task's +0x30 is the KR CTask slot 12,
// SendMsgToMainActor.
void FrAttendanceDlg::ReturnToLogin(void) {
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x104, 0, 0, 0, 0, 0));

	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
		WNetworkSystem::Instance()->ForceShutDown(WNetworkSystem::NET_UNIT_DEFAULT);
	}
	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_LOGIN)) {
		WNetworkSystem::Instance()->ForceShutDown(WNetworkSystem::NET_UNIT_LOGIN);
	}

	JpAttendance_ResetLoginData();

	WSendPacket packet(GCR_LOGOUT);
	packet.Send(SEND_TO_MSN);

	CMessengerInfo::Instance()->Close(true);
	WNetworkSystem::Instance()->ForceShutDown(WNetworkSystem::NET_UNIT_MSN);

	// JP +0x10d41: the KR m_logoutPending (its OnLogoutBtnUp clears it the
	// same way).
	if (WSingleton<CSharedDoc>::Instance()->m_logoutPending == true) {
		WSingleton<CSharedDoc>::Instance()->m_logoutPending = false;
	}

	CTask* task = AfxGetTask();
	if (!(task != 0 ? ((IObject*)task)->IsExactKindOf(&CLobbyTask::m_RTTI) : false)) {
		CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);
	}

	CTaskManager::Instance()->PostMsg(0, "Lobby", 0, (intptr_t)"LOGIN", 0, 0, 0);
}

// 0x47d960
void FrAttendanceDlg::OnProc(const float dt) {
	FrForm::OnProc(dt);
	m_elapsed = dt + m_elapsed;
	float elapsed = m_elapsed;
	if (elapsed < 1.0f) {
		return;
	}
	if (elapsed < 2.0f) {
		if (m_kind == 0) {
			m_buttonHover = true;
		}
		return;
	}
	if (elapsed < 3.0f) {
		m_countdown = 3;
		return;
	}
	if (elapsed < 4.0f) {
		m_countdown = 2;
		return;
	}
	if (elapsed < 5.0f) {
		m_countdown = 1;
		return;
	}
	if (m_kind == 1) {
		if (m_mode == 0) {
			if (m_openedByPacket && OnlinePlay()) {
				JpAttendance_OnCheckInDone();
			}
			JpAttendance_QuitGame();
		} else if (m_mode == 1) {
			ReturnToLogin();
		}
	} else if (m_kind == 0) {
		Close(true);
	}
}

// 0x47da80
void FrAttendanceDlg::SetInfo(const sAttendanceInfo& info) {
	m_info = info;
	m_hasInfo = true;
	UpdateItemIcon();
	UpdateCount();
	UpdateItemTooltip();
}

// 0x6550d0
FrAttendanceDlg* CreateAttendanceDlg(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent) {
	return CreateForm<FrAttendanceDlg>(manager, owner, name, parent);
}

// --- JP game glue ---
// The JP-only day-count font (vtable 0xd1c1f0; name ours, from its
// texture).
class CCheckDateFont : public WTitleFontEx {
public:
	CCheckDateFont(void);
};

// 0xe1697c: the font's texture entry (the KR fonts' s_fontFiles shape).
static char s_checkDateFontName[] = "[font_check_date.jpg";
static char* s_checkDateFontFile = s_checkDateFontName;
// 0xd1c040: its character set.
static char s_checkDateFontChars[] = "1234567890";

// 0x6f5000
CCheckDateFont::CCheckDateFont(void) {
	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = &s_checkDateFontFile;
	info.numPages = 1;
	info.fontw = 40;
	info.fonth = 56;
	info.texw = 200;
	info.texh = 112;
	info.pCharSet = s_checkDateFontChars;

	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

// OnInit 0x47d590's new (0xc bytes) of the font 0x6f5000.
WTitleFontEx* JpAttendance_NewDaysFont(void) {
	return new CCheckDateFont;
}

// The JP FrLogoutDlg's +0x120 / +0x121 (JpAttendanceDlg.h), for the one
// dialog that set them.
static const FrForm* s_quitDlgChecked = NULL;
static bool s_quitDlgCheckIn = false;
static bool s_quitDlgCheckOut = false;

bool JpQuitDlg_IsCheckIn(const FrForm* quitDlg) {
	return quitDlg == s_quitDlgChecked && s_quitDlgCheckIn;
}

bool JpQuitDlg_IsCheckOut(const FrForm* quitDlg) {
	return quitDlg == s_quitDlgChecked && s_quitDlgCheckOut;
}

void JpQuitDlg_ClearCheck(void) {
	s_quitDlgChecked = NULL;
	s_quitDlgCheckIn = false;
	s_quitDlgCheckOut = false;
}

// The common test of 0xadc2e0 / 0xadc550: the login-side layouts (the KR
// FrLogoutDlg::OnServerBtnInit's list) and the JP game data +0x10d30.
static bool QuitDlgOpensAttendance(void) {
	if (g_pFresh->IsCurrentLayout("SERVERLIST") || g_pFresh->IsCurrentLayout("CREATE")
		|| g_pFresh->IsCurrentLayout("AGREEMENT") || g_pFresh->IsCurrentLayout("BLANK")) {
		return false;
	}
	if (JpQuitDlg_IsAttendanceSuppressed()) {
		return false;
	}
	return true;
}

// 0xadc2e0
void JpQuitDlg_OnExitUp(FrForm* quitDlg) {
	if (QuitDlgOpensAttendance()) {
		JpQuitDlg_ClearCheck();
		s_quitDlgChecked = quitDlg;
		s_quitDlgCheckIn = true;
		quitDlg->Close(true);
		return;
	}
	g_bQuit = true;
}

// 0xadc550 (false: the caller runs the logout procedure 0xadc360, the KR
// FrLogoutDlg::OnLogoutBtnUp)
bool JpQuitDlg_OnLogoutUp(FrForm* quitDlg) {
	if (QuitDlgOpensAttendance()) {
		JpQuitDlg_ClearCheck();
		s_quitDlgChecked = quitDlg;
		s_quitDlgCheckOut = true;
		quitDlg->Close(true);
		return true;
	}
	return false;
}

// 0xb2ef90
void JpAttendance_OnCheckInDone(void) {
	WSendPacket packet(GCT_EXIT_ROOM);
	packet.Encode1(0);
	packet.Encode2(0xFFFF);
	packet.Encode8(0);
	packet.Encode8(0);
	packet.Send(SEND_TO_DEFAULT);

	// JP +0x7a0 / +0x1094d.
	WSingleton<CSharedDoc>::Instance()->m_mySlot = 0xFFFF;
	WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomNumber = 0xFFFF;
}

// The JP CItemManager lookup (0x20c of the game object 0xe47dbc): the KR
// ItemManager()->FindItem; the JP icon name (+0x49) is the KR sItem::m_icon
// (the JP name field is 0x18 bytes longer).
const char* JpAttendance_FindItemIcon(unsigned int typeId) {
	IFF_STRUCT::sItem* item = ItemManager()->FindItem(typeId);
	if (item == NULL) {
		return NULL;
	}
	return item->m_icon;
}

// The same lookup's item name (JP +0x8, the KR sItem::m_name).
const char* JpAttendance_FindItemName(unsigned int typeId) {
	IFF_STRUCT::sItem* item = ItemManager()->FindItem(typeId);
	if (item == NULL) {
		return NULL;
	}
	return item->m_name;
}
