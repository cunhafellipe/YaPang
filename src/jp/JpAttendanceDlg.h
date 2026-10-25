// JP-only game code (no KR counterpart): FrAttendanceDlg, the lobby's daily
// attendance dialog, layout "attendance_dlg". One owner-drawn area
// ("image_area") shows the check-in or check-out button picture, the reward
// item of today or of tomorrow (icon, "x<count>", tooltip with its name) and
// the attendance day count ("%04d"). After five seconds the dialog closes
// itself (check-in) or, for a check-out, returns the player to the login
// screen.
//
// Openers (JP CTaskMain, both through the factory 0x6550d0 =
// CreateForm<FrAttendanceDlg>(manager, owner = &task->+0x18, "attendance_dlg");
// the task keeps the dialog at JP +0x190; port/refresh/game/JpLobbyTaskGlue.cpp):
// - 0x657200, the JP CTaskMain::OnQuitDlgResult (the "quit" dialog's result
//   callback, set by 0x658a20 and 0x65c0d0; CTaskMain +0x184 is the KR
//   m_quitDlg): when the JP FrLogoutDlg's check-in (+0x120, its "exit"
//   button) or check-out (+0x121, its "logout" button) byte is set:
//   m_mode = 0 (then quit the game) or 1 (then back to the login screen),
//   m_kind = 1, then packet 0x16e (no body); m_quitDlg is cleared, returns
//   true.
// - 0x65f3c0, CTaskMain message 0x327 (HandleMsgCommon 0x65d8e0, jump
//   table 0x65f8c0 entry 16; param1 -> the flag byte, param2 -> the six
//   dwords; posted by the 0x248 / 0x249 replies, docs/protocol/lobby.md):
//   with no dialog yet, flag 0 needs the current task to be the RTTI
//   0xe7ac98 class (ntAvatarChatTask by its use in 0x6572f0: the
//   preserve flag of ChangeTask("CShopTask")); the dialog gets m_kind 0
//   when the current task is not that class, otherwise m_kind 1, m_mode 0,
//   m_openedByPacket = true. Then [30](0), SetInfo (0x47da80) with the six
//   dwords, and Open(result callback 0x651270, flag 0x11).
// The result callback 0x651270 (CTaskMain): on result 2, m_mode 1 runs
// ReturnToLogin (0x47d770), m_mode 0 quits the game (0xf02419, the KR
// g_bQuit); the task's dialog pointer (+0x190) is cleared.
//
// JP x86 facts: RTTI m_RTTI 0xe73df4 (GetRTTI 0x47c9f0), registered at
// static init 0xcc7b80 / 0x47d2c0 as "FrAttendanceDlg" (make instance
// 0x47d250), size 0x1f0, base the JP FrForm (ctor 0xbe5e20, dtor 0xbe5e80,
// size 0x110). Vtables 0xcfe794 (38 slots; overrides [0] GetRTTI, [1] the
// deleting destructor 0x47cee0, [9] OnProc 0x47d960, [32] OnInit 0x47d590;
// [6] 0x47ccd0, [10] 0x47ccf0, [14]-[19] 0x47cd00-0x47cd50, [21] 0x47cd60
// and [31] 0x47cd70 are this TU's copies of the FrWnd/FrForm header inlines),
// 0xcfe784 (FrCmdTarget: [0] 0x47cb10 destructor thunk, [1] GetMessageMap
// 0x47c940), 0xcfe77c ([0] 0x47cb20 destructor thunk). Message map
// 0xe034b0 (rows 0xe034b8, handlers stored by the static init 0xcc7ba0).
// The port keeps the KR FrForm layout, so the JP offsets are comments only.
#ifndef PG_JP_JPATTENDANCEDLG_H
#define PG_JP_JPATTENDANCEDLG_H

#include <stdint.h>

#include "defines.h"
#include "frcmdtarget.h"
#include "frform.h"
#include "wtypes.h"

class Bitmap;
class CWindFont;
class FrArea;
class FrWnd;
class FrWndManager;
class WTitleFontEx;

// The six dwords the server packet gives (copied whole by SetInfo 0x47da80).
struct sAttendanceInfo {
	bool status;                  // JP +0x0 (the constructor clears the byte; not read by the dialog)
	unsigned int todayItemTypeId; // JP +0x4 (m_kind 0: the reward item shown)
	unsigned int todayCount;      // JP +0x8 (m_kind 0: "x%d")
	unsigned int nextItemTypeId;  // JP +0xc (m_kind 1: tomorrow's reward item)
	unsigned int nextCount;       // JP +0x10 (m_kind 1: "x%d")
	int days;                     // JP +0x14 (the attendance day count, "%04d")
};

class FrAttendanceDlg : public FrForm {
public:
	static const WRTTI m_RTTI;                                        // 0xe73df4
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x47c9f0

	FrAttendanceDlg(void);                                            // 0x47ce10
	virtual ~FrAttendanceDlg(void);                                   // [1] 0x47cee0 (deleting), 0x47c950

	virtual void OnProc(const float dt);                              // [9] 0x47d960
	virtual bool OnInit(void);                                        // [32] 0x47d590

	// Copies the server's record, then the reward icon, count and tooltip.
	void SetInfo(const sAttendanceInfo& info);                        // 0x47da80

	// The JP CTaskMain writes these directly (JP +0x12c, +0x130, +0x1a6).
	void SetMode(int mode) { m_mode = mode; }
	void SetKind(int kind) { m_kind = kind; }
	void SetOpenedByPacket(bool opened) { m_openedByPacket = opened; }
	int GetMode(void) const { return m_mode; }

	// 0x47d770: the JP FrLogoutDlg's logout procedure (0xadc360, the KR
	// FrLogoutDlg::OnLogoutBtnUp without its REALMYROOM message): the
	// current task's wait message, the network links, the JP login data,
	// packet 0x16 on the messenger link, then back to the login screen.
	static void ReturnToLogin(void);

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe034b8

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe034b0
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // FrCmdTarget [1] 0x47c940

	void OnImageAreaInit(intptr_t param);                             // 0x47cf10
	void OnImageAreaDraw(intptr_t param);                             // 0x47cfa0

private:
	// The button, item frame and countdown pictures of m_kind.
	void LoadBitmaps(void);                                           // 0x47ca00
	// m_count from the record (today's or tomorrow's count).
	void UpdateCount(void);                                           // 0x47cad0
	// 0x47d330: reads the table
	// "attendance_ui_config" of "attendance.lua" into the rectangles and
	// points below (checkin_or_checkout_x/y/w/h -> m_buttonRect,
	// todayitem_or_tomorrowitem_x/y/w/h -> m_itemRect, font_x/y ->
	// m_daysPos, item_x/y -> m_itemPos, itemtooltip_x/y/w/h ->
	// m_tooltipRect; x and y offset by m_origin).
	void LoadLayoutConfig(void);
	void UpdateItemIcon(void);                                        // 0x47d2e0
	void UpdateItemTooltip(void);                                     // 0x47d640

	FrArea* m_pImageArea;           // JP +0x110 ("image_area")
	sAttendanceInfo m_info;         // JP +0x114
	int m_mode;                     // JP +0x12c (0 check-in, 1 check-out; -1)
	int m_kind;                     // JP +0x130 (0 today's pictures, 1 tomorrow's; -1)
	// [0] the item frame (check_item_today / check_item_next), [1]-[3]
	// check_out_countdown1-3, [4] the button (check_in_Normal /
	// check_out_Normal); m_countdown indexes it.
	const Bitmap* m_images[5];      // JP +0x134
	const Bitmap* m_buttonHoverImg; // JP +0x148 (check_in_Hover)
	const Bitmap* m_itemIcon;       // JP +0x14c
	WRect m_itemRect;               // JP +0x150
	// JP +0x160..+0x190: not touched by this class.
	WRect m_buttonRect;             // JP +0x190
	int m_countdown;                // JP +0x1a0 (-1 none)
	bool m_buttonHover;             // JP +0x1a4 (draws m_buttonHoverImg)
	bool m_hasInfo;                 // JP +0x1a5 (SetInfo ran: the day count is drawn)
	bool m_openedByPacket;          // JP +0x1a6
	WTitleFontEx* m_pDaysFont;      // JP +0x1a8 (the JP font 0x6f5000, vtable 0xd1c1f0)
	CWindFont* m_pCountFont;        // JP +0x1ac
	unsigned int m_count;           // JP +0x1b0 (-1)
	WPoint m_daysPos;               // JP +0x1b4
	WPoint m_itemPos;               // JP +0x1bc
	WRect m_tooltipRect;            // JP +0x1c4
	float m_elapsed;                // JP +0x1d4
	WPoint m_origin;                // JP +0x1d8 (image_area's position)
	WPoint m_originOffset;          // JP +0x1e0 (zeroed with m_origin)
	// JP +0x1e8..+0x1f0: not touched by this class.
};
// size 0x1f0 (x86)

// --- JP game glue (JpAttendanceDlg.cpp, except the CTaskMain openers and
// JpAttendance_QuitGame: JpLobbyTaskGlue.cpp) ---
// 0x6f5000 is the constructor of the JP-only WTitleFontEx subclass (size
// 0xc, vtable 0xd1c1f0, texture "[font_check_date.jpg") that draws the day
// count; OnInit 0x47d590 news it.
WTitleFontEx* JpAttendance_NewDaysFont(void);
// The JP CItemManager lookup (0x20c of the game object 0xe47dbc,
// FindItem): its item's name (+0x8) and icon name (JP +0x49); NULL /
// false when the type id is unknown.
const char* JpAttendance_FindItemIcon(unsigned int typeId);
const char* JpAttendance_FindItemName(unsigned int typeId);
// 0xb2ef90 (fastcall, its byte argument in cl: the only caller, OnProc
// 0x47d960, passes 0), run when the check-in finishes while OnlinePlay()
// and m_openedByPacket: the room exit of KR CTaskMain::SendRoomExitPacket
// without its message to the task.
void JpAttendance_OnCheckInDone(void);
// Sets 0xf02419, the KR g_bQuit (the JP "exit" button sets it directly
// when no attendance dialog follows; OnProc and the result callback after
// a check-in).
void JpAttendance_QuitGame(void);
// The JP game data reset of ReturnToLogin 0x47d770 (declared only: JP
// CSharedDoc fields not mapped to the KR one): +0x1cc = -1, +0x1ac = 0,
// the strings +0x1b0 and +0x190 emptied, and +0x28 (the KR
// m_loginServerAddress) unless the byte +0x11458.
void JpAttendance_ResetLoginData(void);

// The JP quit dialog is a JP FrLogoutDlg ("quit" factory 0x6542f0, new
// 0x130, ctor 0xadbcd0, RTTI 0xf031e0, vtables 0xd4c724 / 0xd4c714 /
// 0xd4c70c, message map 0xe39578: "server" 0xadc000 / 0xadc170, "logout"
// 0xadc1e0 / 0xadc550, "exit" 0xadbef0 / 0xadc2e0, stored by 0xcef000). It
// is the KR one plus the bytes +0x120 (check-in) / +0x121 (check-out),
// cleared by the ctor. The KR FrLogoutDlg is 0x120 bytes, so the port
// keeps the two bytes here, for the one dialog that set them.
bool JpQuitDlg_IsCheckIn(const FrForm* quitDlg);
bool JpQuitDlg_IsCheckOut(const FrForm* quitDlg);
// Forgets the bytes (the JP dialog dies with them; called once the result
// callback has read them).
void JpQuitDlg_ClearCheck(void);
// 0xadc2e0 / 0xadc550: the JP "exit" / "logout" button-up. Outside the
// SERVERLIST / CREATE / AGREEMENT / BLANK layouts and with the JP game
// data dword +0x10d30 clear, they set the check-in / check-out byte and
// Close(true) (the result callback opens the attendance dialog); else
// "exit" quits the game (g_bQuit) and "logout" runs the logout procedure
// 0xadc360 (the KR OnLogoutBtnUp: false is returned so that the KR
// FrLogoutDlg runs its own body).
void JpQuitDlg_OnExitUp(FrForm* quitDlg);
bool JpQuitDlg_OnLogoutUp(FrForm* quitDlg);
// The JP game data dword +0x10d30 (declared only, not mapped to the KR
// CSharedDoc): non-zero, the quit dialog's buttons open no attendance
// dialog.
bool JpQuitDlg_IsAttendanceSuppressed(void);

class CTaskMain;
class MsgObject;
// 0x657200 (the JP CTaskMain::OnQuitDlgResult's attendance part).
void TaskMain_OpenAttendanceOnQuit(CTaskMain* task, const FrForm* quitDlg);
// 0x65f3c0 (CTaskMain message 0x327).
void TaskMain_OnAttendanceMsg(CTaskMain* task, const MsgObject& msg);

// The factory 0x6550d0 (KR CreateForm<FrAttendanceDlg>).
FrAttendanceDlg* CreateAttendanceDlg(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent);

#endif // PG_JP_JPATTENDANCEDLG_H
