// The JP 983 game code RFLobbyDesktop calls that the port's KR game classes
// do not have yet (names ours). Each entry says what the JP body does
// (node scripts/disasm.ts --jp 0x<va>) and which KR code it corresponds to;
// the ones below g_jpGameState are defined on the KR CSharedDoc / CProjectG
// members their JP fields are proven to be (JpLobbyGlue.cpp).
#ifndef PG_JP_JPLOBBYGLUE_H
#define PG_JP_JPLOBBYGLUE_H

#include <stdint.h>

#include <list>
#include <map>
#include <string>
#include <vector>

#include "padding.h"

class CLobbyMain;
class CProjectG;
struct _SYSTEMTIME;
class CTask;
class RFWnd;
class RFLobbyCharacterSelectWnd;
class RFLobbyCharacterSelectAnimWnd;
class RFLobbyMessengerBubble;
class RFLoadingProgressDlg;
class RFServerNoticeDlg;
class FrForm;
class CTaskManager;
class CTaskMain;
class WRect;
struct sBriefUserInfo;
struct sRoomInfo;
class FrGraphicInterface;
class FrGameRoomDlg;
class RFLobbyDesktop;
struct sCharacterInfo;
namespace IFF_STRUCT {
struct sItem;
}

// CLobbyMain's "single_mode_start" dialog (btn_practice): JP 0x595f60 is
// CLobbyMain::OnToppage_SingleModeUp, which LobbyMain.cpp (JP_PAKS) defines
// with this wrapper.
void LobbyMain_OpenSingleModeStart(CLobbyMain* lobby);
// 0x596370: CLobbyMain's "notice_popup" dialog with premiumservice.ini
// (btn_premium).
void LobbyMain_OpenPremiumNotice(CLobbyMain* lobby);
// 0x9bd4f0: whether the task's form at +0x1fc is the named Fresh layout.
bool Task_IsLayoutOpen(CTask* task, const char* layout);
// 0x9b7810: the FrForm the task's +0x1fc widget holds at its +0x1a0
// (CTask::m_pWidget, JpTaskPage.cpp).
FrForm* Task_GetWidgetForm(CTask* task);
// The form's vtable slot 2 (+8), which the bar buttons call with 1:
// FrWnd::Close(true), closing the open page's form.
void Form_Close(FrForm* form, bool close);
// 0x5d2980: creates the RFLobbyCharacterSelectWnd
// (JpLobbyCharacterSelectWnd.cpp); RFLobbyDesktop.cpp declares it too.
RFWnd* CreateLobbyCharacterSelectWnd(RFWnd* parent);
// 0x5d6170 / 0x5d7bd0: create the RFLobbyPopupMenuWnd / RFServerNoticeDlg
// (JpLobbyDialogs.cpp); ServerNoticeDialog_Show calls the dialog's [34].
// 0xe781c0 / 0xe781d0 hold the instances (defined in RFLobbyDesktop.cpp).
// RFLobbyDesktop.cpp declares all of these too.
RFWnd* CreateLobbyPopupMenuWnd(RFWnd* parent);
RFWnd* CreateServerNoticeDialog(RFWnd* parent);
void ServerNoticeDialog_Show(RFWnd* dialog, bool show);
extern RFWnd* g_lobbyPopupMenuWnd;
extern RFServerNoticeDlg* g_serverNoticeDlg;
// 0x5d2ef0 / 0x5f9cb0 / 0x5d1870: create the
// RFLobbyCharacterSelectAnimWnd (JpLobbyCharacterSelectWnd.cpp), the
// RFLobbyMessengerBubble and the RFLoadingProgressDlg (JpLobbyDialogs.cpp).
// 0xe781b8 / 0xe78c44 / 0xe781a4 hold the instances (defined in
// RFLobbyDesktop.cpp; the constructors store them).
RFWnd* CreateLobbyCharacterCarousel(RFWnd* parent);
RFLobbyMessengerBubble* CreateMessengerBubble(RFWnd* parent);
RFWnd* CreateLoadingWaitingDialog(RFWnd* parent);
extern RFLobbyCharacterSelectAnimWnd* g_lobbyCharacterCarousel;
extern RFLobbyMessengerBubble* g_messengerBubble;
extern RFLoadingProgressDlg* g_loadingWaitingDialog;
// 0x63f3f0 (in the JP SharedDoc unit, on 0xe47dbc; name ours): the
// character data of the given character (-1: the equipped one, as
// RFLobbyCharacterSelectAnimWnd::SetOpen uses it), whose first dword is
// the character's type id; NULL when missing or not a character type id.
// On the KR CSharedDoc::m_characterMap (JpLobbyGlue.cpp).
const uint32_t* Jp_GameState_GetCharacter(int character);
// Ours: the KR sCharacterInfo Jp_GameState_GetCharacter's data belongs to
// (same lookup and test), for the KR code that takes the whole record.
sCharacterInfo* Jp_GameState_GetCharacterInfo(int character);
// JP CSharedDoc +0x10d80, +0x10d81, +0x10d82 (NotifyMessenger's tests): the
// KR m_tutorialFinish (the gift alarm), m_value4DBD and m_value4DBE (the
// messenger alarm states of CMessengerInfo::Notify3 / Notify4).
bool Jp_GameState_10d80();
bool Jp_GameState_10d81();
bool Jp_GameState_10d82();
// JP CSharedDoc +0x11410 (a byte; name ours): the JP CShopMain's
// OnPetOwnerDraw 0xa90aa0 tests it where the KR CShopMain::OnPetOwnerDraw
// tests CTaskMain::m_bongdariShop[2], so it is that flag moved from the task
// into the shared doc. The port's CSharedDoc has no such member: false.
bool Jp_GameState_11410();
// 0x5f6430 (RFLobbyDesktop's code, no this; REQUESTed there): the lobby
// carousel's character (RFLobbyDesktop::GetSelectedCharacterTypeId 0x5f6410)
// as a character type id, 0 for none (15). Call it only with a desktop.
uint32_t JpLobbyDesktop_GetSelectedCharacterTid(void);
// RFLobbyDesktop::SetBarSelection 0x5fbe70 and UpdateLobbyCamera 0x5f8020
// on s_instance, for JpShopMain::ChangeMainTab and JpRealMyRoomMain's
// SetSubTab (call them only with a desktop: the JP callers test s_instance
// themselves). Both members are protected; RFLobbyDesktop.h befriends these
// two and RFLobbyDesktop.cpp defines them.
void JpLobbyDesktop_SetBarSelection(int id);
void JpLobbyDesktop_UpdateLobbyCamera(void);
// 0xb2ea00 (not placed): the current server's channel type from the KR
// CSharedDoc::IsControlServerService bits 0x40..0x2000 (JP 0x62ba40, a
// 64-bit field there) and m_serverProperty bit 7 (JP +0x10d3c). 0x10 shows btn_talk in place
// of btn_game.
int Jp_GetChannelType();
// 0x566f30 (__fastcall, value in ecx): sends the lobby-view packet.
void Jp_LobbyViewReset(int value);

// 0xe75114 (names ours; class not identified, NULL in the exe's data): the
// game's string table by id, 0x64 bytes on BaseObject (vtable 0xd0963c, ctor
// 0x541800; 0x53f520, never called, is a base with vtable 0xd09624 that
// stores itself at 0xe75114 too). 0x540900 Find looks the id up in the
// std::map<int, std::string> at +0x28 (0x5407b0 = its find, signed key
// compare; the map's head +0x2c is end()) and returns the string's c_str(),
// NULL when missing. +0x38 is a std::list, +0x44 the last load's result,
// +0x48 the std::string URL. The JP fills it from an HTTP translation server
// only (0x5420a0, Translation/Read.aspx; JpLobbyGlue.cpp): no file in the
// paks or bin_jp carries its data, so the port keeps an empty table.
class JpTextTable {
public:
	const char* Find(int id);

protected:
	UNKNOWN(0x00, 0x28);
	std::map<int, std::string> m_texts;  // 0x28
};
extern JpTextTable* g_jpTextTable;
// 0x540a10 (id in ecx): the table's text for id; NULL for id 0 or without
// the table, "" when missing.
const char* Jp_GetTableText(int id);

// 0xe781b0: the RFLobbyCharacterSelectWnd instance.
extern RFLobbyCharacterSelectWnd* g_lobbyCharacterSelectWnd;
// 0xe03268: which creator CreateLobbyDesktop uses (0 CreateWnd, 2
// CreateDesktop).
extern int g_lobbyDesktopCreateMode;
// 0xe78c34: the parent the lobby desktop was last created under.
extern RFWnd* g_lobbyDesktopParent;
// 0xcfec5c: the RFLobbyDesktop constructor's initial m_bannerAlpha.
extern const float g_lobbyDesktopFloatCfec5c;
// 0xe47dbc: the JP CSharedDoc instance (0x63f3f0, which reads it, lies in
// the JP SharedDoc unit; its +0x10b25 is what the KR
// CLobbyMain::OnPCBangDlgResult reads as CSharedDoc::m_noticeShown). Kept
// as raw bytes: the JP CSharedDoc layout differs from the KR one (KR
// m_noticeShown is +0x4b7d), so no KR member can stand for a JP offset.
extern char* g_jpGameState;
// JP CSharedDoc +0x10b27, which RFLobbyDesktop::OnDestroy (0x5f7080) sets
// to 1 after the actors' 0x184 messages (accessor ours, writing it). The JP
// ClearLobbyStateVars (0x6425e0) stores 0x01000100 to +0x10b24 and 0 to the
// word +0x10b28: +0x10b24..+0x10b26 are the KR m_paranNickChanged,
// m_noticeShown and m_3DBackgroundOff, +0x10b28/+0x10b29 the KR
// m_lobbyStateFlag/m_lobbyStateFlag2, and +0x10b27 (1 after the clear) is
// JP-only: the JP_PAKS CSharedDoc::m_lobbyCameraEnabled.
void Jp_GameState_SetLobbyCameraEnabled(bool value);
// JP CSharedDoc +0x10b29: the KR m_lobbyStateFlag2 (CLobbyMain's JP_PAKS
// single mode start, LobbyMain.cpp, declares it too; the other single-mode
// accessors there stay declared: their +0x1094b..+0x1094f fields of the JP
// room info (0xdd bytes, KR sRoomInfo 0xb2) are not tied to KR fields yet).
void Jp_GameState_Set10b29(unsigned char value);

// The JP CSharedDoc character data RFLobbyCharacterSelectWnd uses
// (JpLobbyCharacterSelectWnd.cpp; names ours). The map at +0x3704 (head
// +0x3708) is the KR m_characterMap (Jp_GameState_GetCharacter), defined
// on it in JpLobbyGlue.cpp; SetShownCharacter stays declared.
// 0xe47dbc +0x3708 (the JP CSharedDoc's character map, a std::map whose
// node holds the key at +0xc and the character data from +0x10): the first
// dword (the type id) of each owned character's data, in the map's order.
std::vector<uint32_t> Jp_GameState_CharacterTypeIds();
// 0x643ed0 (JP SharedDoc unit, on 0xe47dbc): the uid (the character data's
// second dword, node +0x14) of the owned character whose type id (node
// +0x10) is typeId and whose kind bits (& 0xfc000000) are 0x4000000; -1
// takes the equipped one (+0xa2a); -1 when none.
int Jp_GameState_FindCharacterUid(uint32_t typeId);
// 0x632df0 (JP SharedDoc unit, on 0xe47dbc): the KR
// CSharedDoc::BuildMyPartTidList(sCharacterInfo&): +0x110cc is
// m_myPartTidList (SetTids with the +0x91d byte m_myLevel, SetDefaultTids,
// IsComboValid), +0x11194 m_myAuxPartTidList (the JP character data is
// 0x204 bytes, its aux parts at +0x1a4; KR sCharacterInfo 0x1bc).
void Jp_GameState_SetShownCharacter(sCharacterInfo* character);
// 0x4d0e90 (on the object at 0xe73e14, which it does not read; unplaced):
// assigns the 15 characters' image name parts, by character id
// ("nuri_old", "hana_old", "azer", "cecilia_old", "max", "kooh", "arin",
// "kaz", "lucia", "nell", "spika", "nuri", "hana", "azer", "cecilia").
void Jp_GetCharacterImageNames(std::string* names);

// RFLobbyDesktop.cpp declares these too; repeated here so that both
// translation units see one signature. 0xa62c60: CProjectG::HidePrivacy.
void ProjectG_DrawWon(CProjectG* game);
// JP CSharedDoc +0x91e (m_myPang) and +0x3858 (m_myCash).
int64_t Jp_GameState_Pang();
int64_t Jp_GameState_Cookies();
// JP CSharedDoc +0x10d30 (m_offlinePlay), +0x10aa9 (m_curServer.name),
// +0x10a54 (m_currentChannel.name).
bool Jp_GameState_IsOfflinePlay();
const char* Jp_GameState_ServerName();
const char* Jp_GameState_ChannelName();
// 0xb30710 and 0x6333c0 (JP time helpers; 0x6333c0 reads the server time,
// JP +0x180 = KR m_serverTime).
void Jp_SystemTimeFromTime(_SYSTEMTIME* out, int64_t time);
int64_t Jp_GameState_HoursUntil(_SYSTEMTIME const& time);

// The JP game code RFLobbyDesktop.cpp calls (names ours).
// 0xf022d0 is the JP g_pFresh (its +0x40 the FrWndManager, GetManager()):
// the bar handlers ignore keys while the manager's modal form count (JP
// +0xac, FrWndManager::GetModalFormCount under JP_PAKS) is not 0.
bool Jp_IsFreshDialogActive();
// DrawDesktop's banner: g_pFresh->GetManager()->GetGDI() (JP +0x84).
FrGraphicInterface* Jp_GetFreshGDI();
// 0xe74210 is the JP CCardManager instance: JP
// CTaskMain::OnOverBar_SpecialCardUp 0x656be0 clears its +0x1380 (KR
// SetSPCardAlarm(false), m_spCardAlarm) and stores the "spcardbuff" form
// in +0x137c (KR SetCardBuffDlg). NotifyTopBar(0xca) shows btn_spcard's
// "on" images while it is set (false without the instance).
bool Jp_IsSpecialCardActive();
// btn_exit's message box in avatar-chat mode when the dword +0x111d4 of
// 0xe47dbc is set (accessor ours, reading it): the KR m_stateTrade, a
// personal shop being set up or open (JP SendTradeOpenShop 0x62fee0
// compares it with STATE_TRADE_READY, InitTradeData 0x63ef60 clears it).
// 0x9a1600 is the JP CChatMsg::AddChatMsg on the CChatMsg instance
// 0xe73e8c; 0xd13338 is the Jp_LookupText key; defined in
// JpLobbyWidgets.cpp.
bool Jp_IsExitMessageBoxMode();
void Jp_ShowExitMessageBox(RFLobbyDesktop* desktop);
// 0x9bf6e0 (in the JP TaskMain/TaskManager unit, on the task manager):
// looks the named task up (0xa4c0d0, 0x4b85f0); none: the current task's
// +0x1fc object is deleted (0x9b7790); else it builds the task's object
// (0x47cc60 with 0xe78d18), sets it as the current task's +0x1fc and calls
// its 0x64f8d0(page) and 0x64f600. Called with the pending layout
// (m_string224) and "TOPPAGE" when the lobby's 0x180 message completes.
// No KR counterpart: declared only.
void TaskManager_9bf6e0(CTaskManager* manager, const char* name, const char* page);
// 0xe47dbc's dword +0x11038, which NotifyMessenger tests with the bytes
// +0x10d80..+0x10d82: the JP-only treasure alarm state (JP_PAKS
// CSharedDoc::m_treasureAlarmState; 3 once the JP CTaskMain::OnTreasureAlarmUp,
// paired with the KR one, opened the alarm). Inline in the JP: no function.
int Jp_GameState_TreasureAlarmState();
// 0x656be0 (in the JP TaskMain unit): the JP CTaskMain's
// OnOverBar_SpecialCardUp, which takes the button's screen rect as a WRect
// (x, y, w, h); the KR one takes none and reads m_barButton[10]. Defined
// on the JP_PAKS CTaskMain::OnOverBar_SpecialCardUp(WRect const&).
void TaskMain_OnOverBar_SpecialCardUp(CTaskMain* task, WRect const& buttonRect);
// RefreshOnShow_5fae60's game state (0xe47dbc; accessors ours, reading it):
// the premium bit (dword +0x812, bit 14: the KR m_userInfo.identity) and
// the item list at +0x375c (the KR m_itemInfoList: node +0xc the
// sItemInfo::m_typeId, +0x28/+0x2c a 64-bit time, the KR sItemInfo +0x20).
bool Jp_GameState_IsPremium();
bool Jp_GameState_FindItemTime(uint32_t typeId, int64_t* time);
// TaskManager_9bf6e0, Task_IsLayoutOpen and Task_GetWidgetForm depend on the
// JP-only CTask +0x1fc slot (0x9b7790 resets it, 0x9b77d0 deletes it,
// 0x9b7780 sets it; the "none" object is 0xf00688): no KR member, declared
// only (ProcessLobbyKeys leaves its SEARCH key, which needs it, out).
// JP 0x65c580 (in the JP TaskMain unit): the JP CTaskMain's key handling
// that the KR CTaskMain::OnProcess does after ProcessOverBar: FUNC_12
// toggles "kbd_manual" (JP +0x158 = KR m_kbdManualDlg), ESCAPE (unless a
// Fresh window takes it or "holeinoneresult" / "uniteresult" is up) goes
// back, plus the JP-only "SEARCH" key with "realmyroom" / "shopmain"
// layout tests. The task's per-frame call while nothing covers the lobby
// (Update_5f9150). Defined on the JP_PAKS CTaskMain::ProcessLobbyKeys.
void TaskMain_ProcessLobbyKeys(CTaskMain* task);

// The JP state the lobby forms (JpRoomListSelector.cpp, JpChatDlg.cpp, which
// declare them too) read and write.
// JP CSharedDoc +0x10b28: the KR m_lobbyStateFlag (R538 map: JP
// CGhostMain::OnExit 0x823700 picks "ROOMLIST" / "TOPPAGE" on it as KR).
bool JpSharedDoc_GetLobbyFlag(void);
// 0x62bf80 / 0x62bf90: JP CSharedDoc +0x11638 (role not proven); no KR
// counterpart.
void JpSharedDoc_SetLobbyMode(int mode); // 0x62bf80
int JpSharedDoc_GetLobbyMode(void);
// 0x99f050 (JP CChatMsg 0xe73e8c): `g_ime->GetState(7, 0) != 0`, the KR
// CChatMsg::IME_IsImeOpened.
bool JpChatMsg_IsHangulMode(void);
// JP COption (WSingleton instance 0xe73e90) dword +0x1cc: the chat window
// folded down (chatwndbtn toggles it); the KR COption has no such member.
int JpOption_GetChatWndDown(void);
void JpOption_SetChatWndDown(int down);
// JP CSharedDoc +0x10c98: the chat-line std::list, the KR m_chatLineList
// (JP 0x490a80, lib, clears it when the chat view is made, as KR
// CChatMsg::ClearHistory chatmsg.cpp:328 does).
void JpSharedDoc_ClearChatLines(void);

// The JP state FrRoomListDlg (JpRoomListDlg.h, JpRoomListDlg.cpp, which
// declare them too) reads and writes.
// JP CSharedDoc +0x10d3c bit 11: the KR m_serverProperty (Jp_GetChannelType).
bool JpSharedDoc_IsGpRoomTabShown(void);
// 0x62bf60 / 0x62bf70: JP CSharedDoc +0x11634 (1 GP room, 0 multiplayer);
// no KR counterpart.
void JpSharedDoc_SetRoomTab(int tab);
int JpSharedDoc_GetRoomTab(void);
// 0xb2ea00: the same function as Jp_GetChannelType.
int JpLobby_GetAvatarChatMode(void);
// 0xbe7870 (JP FrWnd unit): sets the JP-only Fresh byte 0xf1aac8, which
// 0xbe7850 (the form open, from 0xbe6440) and 0xbe7c10 (a per-frame draw)
// test and 0xbe7c10 clears; no KR counterpart.
void JpFrWnd_RaiseFlagF1aac8(void);
bool JpFrWnd_IsFlagF1aac8Raised(void);

// The JP room state FrGameRoomDlg reads (R678). The JP room record starts at
// CSharedDoc +0x10904 (its room number at +0x1094d, Jp_LobbyViewReset).
// JP +0x109b8 = room +0xb4: the KR m_roomInfo.m_roomMode (sRoomInfo +0xb1;
// the JP record is longer before it). JP CGolfBg::DoReLoad 0x72f030 tests it
// where KR tests m_roomMode (JP 0x12, KR 0xe); JP 0xc / 0xe hide the modify
// and assist buttons.
int JpSharedDoc_GetRoomGameMode(void);
// JP byte +0x1094c = room +0x48: the KR m_roomInfo.m_gameType (sRoomInfo
// +0x48, the same offset: the room number follows at +0x49 in both). JP
// FrGameRoomDlg::On30sGuildNumInit 0x5121d0 and FrChatDlg::OnChatTargetInit
// 0x48fe10 test == 6, OnResultRankingTabInit 0x512610 == 0xa: the KR
// FrChangeRoomInfoDlg's m_gameType tests (== 6, == 10).
int JpSharedDoc_GetRoomGameType(void);
// 0xb304d0: 0xb30490 (thiscall on CSharedDoc +0x108f4) == 1, bit 1 of the
// dword +0x109bd; no KR counterpart.
bool Jp_IsShortGameAvailable(void);
// JP +0x10950: the KR m_roomInfo.m_course.
unsigned char JpSharedDoc_GetRoomCourse(void);

// FrChatDlg::OnProc 0x49047a: dword +0x2c of the WSingleton at 0xe73e94
// (Instance() 0x48f210; its ctor 0xb24f70 = KR
// WSingleton<CRankingInfo>::WSingleton): the KR CRankingInfo::m_pRankingDlg
// (GetDlg). Nonzero while the ranking dialog is open: Ctrl+EMOTICON must not
// open the emoticon dialog (KR CLobbyMain::OnProcess tests the same).
bool JpChat_IsEmoticonHotkeyBlocked(void);

// 0x4ceb40 (thiscall on the JP object at 0xe73e14, ctor 0x4d0e00 on
// BaseObject, vtable 0xd045e0; name ours): its const char* table at +0x2c,
// filled by 0x4cead0, indexed by character id: the base image drawn under an
// item's icon ("bar_m_def" ...). No KR counterpart table.
const char* Jp_GetCharacterIconBaseName(int characterId);
// 0xa17890 = CItemManager::FindHairStyle(unsigned long) on the JP item
// manager (CSharedDoc +0x20c), then the byte +0xc1 of the record (JP
// IFF records are 0x30 longer before it): the KR sHairStyle::m_style, the
// character a kind-15 (hair dye) item belongs to (KR CCheckoutDlg compares
// it with the character's typeId & 0x3ffffff).
unsigned char Jp_ItemManager_GetCharacterId(unsigned int typeId);

// The JP room list state FrRoomListDlg and FrMakeRoomDlg read and write
// (R742). JP CSharedDoc +0x109e4 = room record +0xe0: the KR m_roomList
// (room +0xc0), the std::list right after the lobby user map in both. The
// JP sRoomInfo is longer than the KR one (0xb2 bytes): its JP-only fields
// are not in the port's records.
const std::list<sRoomInfo>& JpSharedDoc_GetRoomList(void);
// JP +0x109d8 = room +0xd4: the KR m_userMap (room +0xb4), the lobby's
// players by UID (JP 0x520340 walks it as a std::map, its 0xc8-byte pair
// values copied from node +0x10; KR FrUserListDlg lists m_userMap).
const std::map<uint32_t, sBriefUserInfo>& JpSharedDoc_GetLobbyUserMap(void);
// 0x502f80 (fastcall, name ours): `identity == -1` takes the local
// player's (JP CSharedDoc +0x812 = the KR m_userInfo.identity), then
// (identity & 4) == 4: the KR AdminMode (shareddoc.inl) with an argument.
// JP 0x520340 calls it with -1 before listing the lobby users, as KR
// FrUserListDlg::MakeUserList tests AdminMode().
bool Jp_IsAdminIdentity(int identity);
// 0x59f130 (fastcall, name ours): the room list category of a JP room
// mode (2: 3; 4, 5, 6: 1; 7, 9, 10: 2; 11: 4; else 0).
int Jp_GetRoomCategory(unsigned char mode);
// 0x59fa80 = CLobbyMain::LatestVersion (jp/pairing.tsv): the JP compares
// m_latestVersion (JP +0x10d08, KR +0x4d30) with "JP.R7.974.00": greater,
// the current task's main actor gets MsgObject(0, 0x23, the update text
// through Jp_LookupText) and false is returned. The KR member is protected
// and compares == "645.00": the port runs the JP body here.
bool CLobbyMain_LatestVersion(void);
// JP +0x10a98 = the JP m_currentChannel (name +0x10a54) + 0x44: the KR
// m_currentChannel.id (0xff: no channel; FrRoomListDlg::OnMakeRoomLBtnUp
// 0x51f2e0 tests it).
unsigned char JpSharedDoc_GetCurrentChannelId(void);
// The writes of the room game type (JP byte +0x1094c, the KR
// m_roomInfo.m_gameType) and mode (JP +0x109b8, the KR m_roomInfo.m_roomMode)
// FrMakeRoomDlg makes (0x17 to both before FrRoomListDlg 0x51ca40 / 0x51d6b0).
void JpSharedDoc_SetRoomGameType(unsigned char gameType);
void JpSharedDoc_SetRoomGameMode(unsigned char mode);

// The room dialogs' JP state (JpGameRoomDlg.cpp and JpRoomTypeDlgs.cpp
// declare them too; defined in JpLobbyGlue.cpp).
// JP CSharedDoc +0x11609 (0x503430; no KR member): the hole cup position is
// fixed. The same port-held byte as LobbyMain.cpp's Jp_GameState_11609.
void JpSharedDoc_SetHolecupPosFixed(bool fixed);
bool JpSharedDoc_IsHolecupPosFixed(void);
// 0x63bba0: the assist item 0x1be00016 is on (false in room modes 0xb..0xe).
bool JpSharedDoc_IsAssistOn(void);
// OnStartLBtnUp 0x509a4b-0x509b17: the rookie channel (KR
// m_currentChannel.flags 0x800) forbids the room's course (kind >= 3) for a
// non-admin.
bool JpGameRoom_IsCourseBlocked(void);
// OnStartLBtnUp's other host checks (the card notice 0x509b7e, the artifact
// checker 0x479f40, the local content 0x55 clear 0x509e35): bodies that let
// the start go on (their JP parts are not decompiled; JpLobbyGlue.cpp).
bool JpGameRoom_OpenCardNotice(FrGameRoomDlg* dlg);
const char* JpGameRoom_FindArtifactBlockText(void);
void JpGameRoom_ClearStartState55(void);

// FrGameRoomDlg's JP state (JpGameRoomDlg.cpp declares them too; defined in
// JpLobbyGlue.cpp). JP CSharedDoc +0x109b9 (the JP room record's +0xb5, no
// KR field): the room's artifact, 0 in the port.
uint32_t JpSharedDoc_GetRoomArtifact(void);
// The JP item record's dword +0xc0 bit 1 (the KR sItem byte +0x90 bit 1).
bool JpItem_IsRoomArtifact(const IFF_STRUCT::sItem* item);
// 0xa19320: the JP item manager's +0x348 map text, NULL in the port.
const char* JpItemManager_FindDescText(uint32_t typeId);
// 0x645140: the KR CSharedDoc::SortAllMyItemList.
void JpSharedDoc_SortItemLists(void);
// JP CSharedDoc +0x10a99 bit 0x800 (the KR m_currentChannel.flags).
bool JpSharedDoc_IsRookieChannel(void);
// 0x633340: the course may be picked in the rookie channel.
bool JpCourse_IsRookieAllowed(unsigned char course);
// 0xb303d0 (fastcall): SetIndex / GetIndex of the uid, then the "Gateway"
// actor's message 0x753b (1, index, character data).
void JpRoomPet_Notify(uint32_t uid, const sCharacterInfo* character);
// JP CSharedDoc +0x10488's writer (JpNotifyTikiDlg.h declares it too).
void Jp_GameState_Set10488(bool value);

#endif // PG_JP_JPLOBBYGLUE_H
