// JP-only game code: FrGameRoomDlg, the base of the JP room dialogs
// (JpGameRoomDlg.h). Bodies read from the JP exe (node scripts/jp_hlil.ts,
// node scripts/disasm.ts --jp); the handlers that read JP game state not in
// the port are declared only.
#include "JpGameRoomDlg.h"

#include <string.h>

#include "IContentsDoc.h"
#include "ObjectFactory.h"
#include "Packet.h"
#include "RFLobbyDesktop.h"
#include "TaskManager.h"
#include "actor.h"
#include "frarea.h"
#include "frbutton.h"
#include "frcombobox.h"
#include "fredit.h"
#include "fresh.h"
#include "frgaugebar.h"
#include "frgroupbox.h"
#include "frlistbox.h"
#include "frstatic.h"
#include "frviewer.h"
#include "frwnd.h"
#include "shareddoc.h"
#include "wfont.h"
#include "wresrcmng.h"
#include "SoundManager.h"
#include "winput.h"
#include "JpLobbyGlue.h"
#include "JpFreshGlue.h"
#include "JpMasterySlot.h"
#include "JpMasteryStatus.h"
#include "JpRoomListState.h"
#include "RFTextManager.h"
#include "RFTooltipHelper.h"
#include "InputManager.h"
#include "JpAttendanceDlg.h"
#include "NewRecordDlg.h"
#include "ResultDlg.h"
#include "Treasure.h"
#include "TreasureDlg.h"
#include "commonutil.h"
#include "user_info.h"
#include "wtypes.h"
#include "ItemManager.h"
#include "Exhibition.h"
#include "UserListDlg.h"
#include "JpPetFrameGlue.h"
#include "ChangeRoomInfoDlg.h"
#include "UccManager.h"
#include "frgraphicinterface.h"
#include "bitmap.h"
#include "frwndmanager.h"
#include "NetResourceManager.h"
#include "ProjectG.h"

// The JP FrGraphicInterface's title-font print 0xbed090 (defined in
// JpRoomListState.cpp, not in a header yet): OnCurAztecOwnerDraw's count.
void JpGdi_PrintTitleFont(FrGraphicInterface* gdi, WTitleFont* font, float x, float y,
	const char* text, float spacing, int align, float scale, unsigned long color);

// The JP CItemManager member 0xa18f50 (name ours): a ball type id
// (0x14xxxxxx) whose JP ball record's dword +0x324 is 0 (the KR sBall has
// no such field). Port placeholder (body below): false, the count shows.
bool JpItemManager_IsUncountedBall(unsigned long typeId); // 0xa18f50
bool JpItemManager_IsUncountedBall(unsigned long typeId) {
	(void)typeId;
	return false;
}

// OnMapLBtnUp's JP-only step: the JP FrTreasureCourse's window +0x120
// (vtable +0x18, Enable; not identified, no KR member) enabled outside
// the lobby mode 7. Port placeholder (body below): the KR dialog has no
// such window.
void JpTreasureCourse_EnableWnd120(FrTreasureCourse* dlg, bool enable);
void JpTreasureCourse_EnableWnd120(FrTreasureCourse* dlg, bool enable) {
	(void)dlg;
	(void)enable;
}

// The JP 0x507ad0 (fastcall, in-unit; name ours): the room is public (JP room
// +0x30, the KR m_isPublic), of game type 4, and the own list +0x375c holds
// the item 0x1a000137 (count > 0): SetRoomControls then enables the start
// on m_allReady alone, and OnStartLBtnUp opens the card notice. Port
// placeholder (body below): false; the +0x375c list has no proven KR
// counterpart yet.
bool JpGameRoom_IsCardItemRoom(void); // 0x507ad0
bool JpGameRoom_IsCardItemRoom(void) {
	return false;
}

// SetRoomControls' last step with local content 0x55: the master flag into
// the dword +0xd8 of the JP object at 0xe73de8 (not identified; the
// counterpart of JpGameRoom_ClearStartState55). Port placeholder (body
// below): nothing reads it in the port.
void JpGameRoom_SetStartState55(bool master);
void JpGameRoom_SetStartState55(bool master) {
	(void)master;
}

// shareddoc.inl:213 (KR; not declared by SharedDoc.h): JP 0x502ed0, 0x16 names.
const char* GetGameTypeName(unsigned char gameType);

// 0x4ff410 (name ours): pt inside rect, edges included (x, y, w, h).
static bool JpRect_Contains(const WRect& rect, float x, float y) {
	if (rect.x <= x && x <= rect.x + rect.w && rect.y <= y && y <= rect.y + rect.h) {
		return true;
	}
	return false;
}

// 0x503430's JP CSharedDoc byte +0x11609 (the hole cup position is fixed;
// no KR member; name ours, declared only, REQUEST for JpLobbyGlue).
void JpSharedDoc_SetHolecupPosFixed(bool fixed);

// 0x63bba0 (name ours, declared only): the assist mode is on: false in
// the room modes 0xb..0xe (JP CSharedDoc +0x109b8), else the JP item
// 0x1be00016 (0x63b570 on the shared doc) with its +0xc == 1.
bool JpSharedDoc_IsAssistOn(void);

// OnStartLBtnUp's host checks that read JP state the port does not hold yet
// (names ours, declared only; each returns the "go on" value when absent):
// OnStartLBtnUp at 0x509a4b-0x509b17: unless the JP CSharedDoc +0x10a99 bit 0x800 is clear
// or the player is an admin (+0x812 bit 2), true when the room's course
// (+0x10950, 0x80 folded off except 0xfd, | 0x28000000) is a known course
// whose +0x110 & 0xf >= 3 or has the id 1, 0xb or 0xe (then the "rookie
// channel cannot play this map" notice 0xd06628 is shown).
bool JpGameRoom_IsCourseBlocked(void);
// OnStartLBtnUp at 0x509b7e-0x509c6d: not an admin, identity bit 0x8000 clear, 0x507ad0
// == 1, the room player count (+0x10a00) below 5 (+0x10af3 bit 1) or 10,
// and the common item 0x1a000137 owned: opens the "notify_yesno" form
// (callback 0x505e20) and returns true.
bool JpGameRoom_OpenCardNotice(FrGameRoomDlg* dlg);
// OnStartLBtnUp at 0x509c86-0x509d50 (the 0x479f40 checker on the equipped list +0x375c and
// the room's artifact +0x109b9): NULL when the start may go on, else the
// notice text (0xd06570 when the room's artifact is not equipped, else the
// checker's own text, such as "mana_limited").
const char* JpGameRoom_FindArtifactBlockText(void);
// OnStartLBtnUp at 0x509e35: with the local content 0x55, the dword +0xd8 of the JP object
// at 0xe73de8 (not identified) is cleared after the start request.
void JpGameRoom_ClearStartState55(void);

// UpdateTitle's JP state (names ours, declared only; REQUEST for
// JpLobbyGlue): the room's artifact, the dword JP CSharedDoc +0x109b9 (no
// KR member); the JP item record's dword +0xc0 bit 1 (no KR sItem field:
// only such items show in the room's "atft" box); and the JP CItemManager
// 0xa19320 (a map at +0x348; the KR FindDesc by its role): the artifact's
// description text, or NULL.
uint32_t JpSharedDoc_GetRoomArtifact(void);
bool JpItem_IsRoomArtifact(const IFF_STRUCT::sItem* item);
const char* JpItemManager_FindDescText(uint32_t typeId);

// OnTitleInit's 0x645140 (thiscall on the JP CSharedDoc; name ours,
// declared only): sorts the five item lists +0x3734, +0x3750, +0x375c,
// +0x3774 and +0x3780 (0x644b50 with the predicate 0x62b770).
void JpSharedDoc_SortItemLists(void);

// The map prev / next course steps (names ours, declared only; REQUEST):
// the JP CSharedDoc +0x10a99 bit 0x800 (a rookie channel), and 0x633340,
// the course (0x80 folded off except 0xfd) is a known course whose
// +0x110 & 0xf <= 3 (with its other id tests).
bool JpSharedDoc_IsRookieChannel(void);
bool JpCourse_IsRookieAllowed(unsigned char course);

// AddPlayer's JP-only pet pieces (names ours; REQUEST): the JP room slot's
// dword +0x2d4 (the JP sRoomSlot is 0x2dc bytes, longer than the KR 0x290:
// its angel flag sits at +0x2d8): the slot player's uid (MakeRoomUserList
// stores MyUID(), CSharedDoc +0x8cb, into the own slot). The KR sRoomSlot
// has no such member: the port accessors are placeholders (0, ignored)
// until the JP slot layout is in. 0xb303d0 (fastcall: ecx the uid, edx
// the slot's character data, JP sRoomSlot +0xd0 = the KR m_characterInfo)
// notifies the Gateway actor: JpLobbyGlue.h's JpRoomPet_Notify.
uint32_t JpRoomSlot_GetUid(const sRoomSlot& slot);
void JpRoomSlot_SetUid(sRoomSlot& slot, uint32_t uid);

// JP CExhibitionByHardwareTnL (its RTTI name; vtable 0xd05e9c, 0x64c bytes,
// the KR CExhibition is 0x588): the room slot's pet exhibition AddPlayer
// 0x50b850 creates. The JP CExhibition is abstract (vtable 0xd3454c, three
// __purecall slots); this class fills them: [0] 0x4ff0d0 (the scalar
// deleting destructor: ~CExhibition 0x877180, lib), [1] 0x874e70 (0x874af0
// with its two floats and 1), [2] 0x877120 (SetModel: CExhibition::SetModel
// with a leading 1, the hardware T&L path; AddPlayer calls it through the
// vtable). [1] and [2] sit in the JP Exhibition unit and override virtuals
// the KR CExhibition lacks, as does the JP base constructor's fifth
// argument (the slot's uid): REQUESTs for Exhibition.h (JP_PAKS); until
// then the port builds the KR base and drops the uid. Declared here, the
// only user's file, until the JP Exhibition.h has it.
class CExhibitionByHardwareTnL : public CExhibition {
public:
	CExhibitionByHardwareTnL(float x, float y, float screenScale, bool autoRecover, uint32_t uid);
};

// 0x4ff080
CExhibitionByHardwareTnL::CExhibitionByHardwareTnL(float x, float y, float screenScale, bool autoRecover, uint32_t uid)
	: CExhibition(x, y, screenScale, autoRecover) {
	(void)uid;
}
// lib: 0x4ff0d0 (CExhibitionByHardwareTnL's scalar deleting destructor)

uint32_t JpRoomSlot_GetUid(const sRoomSlot& slot) {
	(void)slot;
	return 0;
}

void JpRoomSlot_SetUid(sRoomSlot& slot, uint32_t uid) {
	(void)slot;
	(void)uid;
}

// 0xf023e0 / 0xf023e4: the game's two WInputDev devices (ProjectG.cpp's
// g_keyboard and g_mouse; InitRoom resets both, OnProc reads both, so
// which global is which does not change the behaviour here).
extern WInputDev* g_keyboard;
extern WInputDev* g_mouse;

// 0x62ba40 (JpAlchemyDlg.h, defined in JpLobbyGlue.cpp): the JP CSharedDoc's
// 64-bit IsControlServerService.
uint64_t SharedDoc_IsControlServerService64(CSharedDoc* doc, uint64_t service);

// JP Fresh, declared only (as in JpRoomListDlg.cpp): 0xbe7870 sets the
// FrWnd static flag 0xf1aac8.
void JpFrWnd_RaiseFlagF1aac8(void);

// The JP FrListBox (JP Fresh, not the KR layout past +0xf8) fields
// OnUserInit sets inline: dword +0xfc = 1 and byte +0x114 = 1 (roles not
// identified; declared only, REQUEST for JpLobbyGlue / the JP Fresh).
void JpFrListBox_SetRoomUserListFlags(FrListBox* list);

// The constructor's title font: 0xe0903c points at 0xd064c4 (tagWTITLEFONT
// takes the address of the name pointer); the charset literal is 0xd063fc.
static char s_windFontName[] = "[font_wind.jpg";
static char* s_windFontFile = s_windFontName;
static char s_windFontCharSet[] = "1234567890ym%-.?/:,";
// JpGameRoomOwned's: 0xe16890 points at 0xd1c110.
static char s_singleFontNumName[] = "[single_font_num.jpg";
static char* s_singleFontNumFile = s_singleFontNumName;
static char s_singleFontNumCharSet[] = "1234567890!";

// Port glue (no JP function: JP CContentsDoc::Initialize 0x915da0 inlines
// `new CLobbyContainer` (0x1c bytes, vtable 0xd3c600) and inserts it as type
// 0xa5 through 0x913c70(0xa5, container, false)). The KR IContentsDoc.cpp
// calls it under JP_PAKS without including the port header.
IContentsDataContainer* JpCreateLobbyContainer(void) {
	return new CLobbyContainer;
}

// lib: 0xccb900 (m_RTTI's dynamic initializer, compiler-generated from the definition below)
// Run-time type, base FrChatDlg's 0xe73e98.
const WRTTI FrGameRoomDlg::m_RTTI("FrGameRoomDlg", &FrChatDlg::m_RTTI);

// 0x4ff1c0
const sFRESH_MSGMAP* FrGameRoomDlg::GetMessageMap(void) const {
	return &FrGameRoomDlg::_MsgMap;
}

sFRESH_MSGMAP FrGameRoomDlg::_MsgMap = { &FrChatDlg::_MsgMap, &FrGameRoomDlg::_MsgEntries[0] };

// 0xe08718 (the handlers are stored by the static init 0xccb940)
sFRESH_ENTRY FrGameRoomDlg::_MsgEntries[] = {
	{ "title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnTitleInit },
	{ "frame_bg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnFrameBgInit },
	{ "background", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnBackgroundInit },
	{ "close_wnd", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnCloseWndLBtnUp },
	{ "map_background", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnMapBackgroundInit },
	{ "map", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnMapInit },
	{ "map", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnMapLBtnUp },
	{ "mapprev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnMapPrevInit },
	{ "mapnext", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnMapNextInit },
	{ "mapprev", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnMapPrevLBtnUp },
	{ "mapnext", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnMapNextLBtnUp },
	{ "mapcomboex", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnMapComboInit },
	{ "mapcomboex", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnMapComboLBtnDown },
	{ "start_tip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnStartTipInit },
	{ "ready_tip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnReadyTipInit },
	{ "start", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnStartInit },
	{ "start", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnStartLBtnUp },
	{ "ready", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnReadyInit },
	{ "ready", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnReadyLBtnUp },
	{ "change_team", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnChangeTeamInit },
	{ "change_team", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnChangeTeamLBtnUp },
	{ "modify", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnModifyInit },
	{ "modify", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnModifyLBtnUp },
	{ "powerbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnPowerBarInit },
	{ "controlbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnControlBarInit },
	{ "accuracybar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnAccuracyBarInit },
	{ "spinbar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnSpinBarInit },
	{ "curvebar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnCurveBarInit },
	{ "epower", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnEPowerInit },
	{ "econtrol", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnEControlInit },
	{ "eaccuracy", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnEAccuracyInit },
	{ "espin", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnESpinInit },
	{ "ecurve", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnECurveInit },
	{ "cur_caddie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnCurCaddieInit },
	{ "cur_caddie", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnCurCaddieLBtnDown },
	{ "cur_caddie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnCurCaddieOwnerDraw },
	{ "cur_club", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnCurClubInit },
	{ "cur_club", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnCurClubLBtnDown },
	{ "cur_club", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnCurClubOwnerDraw },
	{ "cur_aztec", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnCurAztecInit },
	{ "cur_aztec", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnCurAztecLBtnDown },
	{ "cur_aztec", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnCurAztecOwnerDraw },
	{ "cur_mascot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnCurMascotInit },
	{ "cur_mascot", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnCurMascotLBtnDown },
	{ "cur_mascot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnCurMascotOwnerDraw },
	{ "cur_equip_item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnCurEquipItemInit },
	{ "cur_equip_item", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnCurEquipItemLBtnDown },
	{ "cur_equip_item", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnCurEquipItemOwnerDraw },
	{ "cur_char_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnCurCharSelInit },
	{ "cur_char_sel", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnSelLBtnDown },
	{ "cur_caddie_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnCurCaddieSelInit },
	{ "cur_caddie_sel", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnSelLBtnDown },
	{ "cur_club_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnCurClubSelInit },
	{ "cur_club_sel", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnSelLBtnDown },
	{ "cur_aztec_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnCurAztecSelInit },
	{ "cur_aztec_sel", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnSelLBtnDown },
	{ "cur_mascot_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnCurMascotSelInit },
	{ "cur_mascot_sel", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnSelLBtnDown },
	{ "atft", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnAtftInit },
	{ "atft.artifact_icon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnArtifactIconInit },
	{ "assist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnAssistInit },
	{ "assist", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrGameRoomDlg::OnAssistLBtnUp },
	{ "assist_tip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnAssistTipInit },
	{ "assist_title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnAssistTitleInit },
	{ "shortgame", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnShortGameInit },
	{ "group_stroke", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnGroupStrokeInit },
	{ "group_30s", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnGroup30sInit },
	{ "group_result", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnGroupResultInit },
	{ "group_gp", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGameRoomDlg::OnGroupGPInit },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x4ff200
void FrGameRoomDlg::SetRoomState(int state) {
	m_roomState = state;
}

// 0x4ff250
bool FrGameRoomDlg::OnScoreDlgResult(int ret, FrForm* pForm) {
	m_pScoreDlg = NULL;
	return true;
}

// 0x4ff260
bool FrGameRoomDlg::OnNewRecordDlgResult(int ret, FrForm* pForm) {
	m_pNewRecordDlg = NULL;
	return true;
}

// 0x4ff270
bool FrGameRoomDlg::OnMapSelectDlgResult(int, FrForm*) {
	m_pMapSelectDlg = NULL;
	return true;
}

// 0x4ff280
bool FrGameRoomDlg::OnUserListDlgResult(int ret, FrForm* pForm) {
	m_pUserListDlg = NULL;
	return true;
}

// 0x4ff290
void FrGameRoomDlg::SetAddOwnPlayer(bool add) {
	m_addOwnPlayer = add;
}

// 0x4ff320
// The KR CLobbyMain::Banish (by its shape): packet 0x26 with the picked
// player's +0x10.
void FrGameRoomDlg::Banish(void) {
	if (m_pLobbyData->m_selectedUserId != 0xffffffff) {
		WSendPacket packet(0x26);
		packet.Encode4(m_pLobbyData->m_selectedUserNo);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// 0x4ff300
void FrGameRoomDlg::SetLobbyValues(WTitleFontEx* font154, WTitleFontEx* titleFont) {
	m_lobbyValue = font154;
	m_pLobbyTitleFont = titleFont;
}

// The lobby's call site (0xab6d60), reached from the FrForm* the KR
// LobbyMain.cpp holds (like JpLobbyGlue.cpp's JpRoomTypeForm_SetRoomState).
void JpRoomTypeForm_SetLobbyValues(FrForm* form, WTitleFontEx* font154, WTitleFontEx* titleFont) {
	static_cast<FrGameRoomDlg*>(form)->SetLobbyValues(font154, titleFont);
}

// The JP CLobbyMain::HandleMsg 0xabc060 cases on the room form (+0x780),
// reached from the FrForm* the KR LobbyMain.cpp holds; ids are the KR ones
// (the JP 0x2e / 0x2f / 0x30 are the KR 0x2d / 0x2e / 0x2f).
void JpRoomTypeForm_OnLobbyMsg(FrForm* form, int id, intptr_t param1, intptr_t param2, intptr_t param3) {
	FrGameRoomDlg* room = static_cast<FrGameRoomDlg*>(form);
	switch (id) {
	case 0x07:  // 0xabc5de
		room->OnPlayerEntered((sSlotInfo*)param1, param2);
		break;
	case 0x08:  // 0xabc600
		room->OnPlayerLeft(param1, param2, param3);
		break;
	case 0x09:  // 0xabc626
		room->UpdatePlayerAngelFlags((sSlotInfo*)param1);
		break;
	case 0x0E:  // 0xabc644
		room->MakeRoomUserList();
		room->UpdateRoomMaster();
		break;
	case 0x0F:  // 0xabc667
		room->UpdateRoomMaster();
		break;
	case 0x10:  // 0xabc67f: always from the room data (param1 ignored)
		room->UpdateTitle(NULL);
		break;
	case 0x11:  // 0xabc69b
		room->SetCourseImages((unsigned char)param1);
		break;
	case 0x12:  // 0xabc6ba
		room->FillHoleModeCombo((int)param1);
		break;
	case 0x29:  // JP 0x2a, 0xabcebf
		room->SetPlayerReady(param1, param2);
		break;
	case 0x2A:  // JP 0x2b, 0xabd15a
		room->SortRank();
		break;
	case 0x2B:  // JP 0x2c, 0xabd111
		room->LockRoomControls(param1);
		break;
	case 0x2D:  // JP 0x2e, 0xabd172
		room->ShowGameResults();
		break;
	case 0x2E:  // JP 0x2f, 0xabd18a: always 0
		room->SetAddOwnPlayer(false);
		break;
	case 0x2F:  // JP 0x30, 0xabd2ef
		room->UpdateTeamControls();
		break;
	}
}

// 0x500760
// The KR CLobbyMain::DrawFrameSelectedUserInfo (same shape): the
// "gameroom_select_frame" bitmap 2 px up-left of rect while the mouse
// (g_input +0x54 / +0x5c) is inside it.
void FrGameRoomDlg::DrawFrameSelectedUserInfo(WRect rect) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI) {
		return;
	}
	if (rect.IsInRect(g_input->GetMousePoint().x, g_input->GetMousePoint().y)) {
		const Bitmap* pBmp = g_pFresh->GetBitmap("gameroom_select_frame");
		if (pBmp) {
			pGDI->DrawTexture(pBmp, WRect(rect.x - 2.0f, rect.y - 2.0f, (float)pBmp->Width(), (float)pBmp->Height()), 0xFFFFFFFF, 0);
		}
	}
}

// 0x500880
void FrGameRoomDlg::OnFrameBgInit(PG_MSGARG param) {
	m_pFrameBg = DYNAMIC_CAST(FrViewer, param);
	if (m_pFrameBg) {
		m_pFrameBg->SetMouseEvent(false);
	}
}

// 0x500900
void FrGameRoomDlg::OnBackgroundInit(PG_MSGARG param) {
	m_pBackground = DYNAMIC_CAST(FrViewer, param);
	if (m_pBackground) {
		m_pBackground->SetMouseEvent(false);
	}
}

// 0x500980
void FrGameRoomDlg::OnUserInit(PG_MSGARG param) {
	m_pUserList = DYNAMIC_CAST(FrListBox, param);
	if (m_pUserList) {
		JpFrListBox_SetRoomUserListFlags(m_pUserList);
		m_pTeamBitmap[0] = m_pUserList->GetBitmap("team_red");
		m_pTeamBitmap[1] = m_pUserList->GetBitmap("team_blue");
		m_pTeamBitmap[2] = m_pUserList->GetBitmap("team_base");
		m_pTeamBitmap[3] = m_pUserList->GetBitmap("team_base02");
		m_pInviteBitmap = m_pUserList->GetBitmap("invite");
		m_pMasterSignBitmap = m_pUserList->GetBitmap("master_sign");
		m_pReadySignBitmap = m_pUserList->GetBitmap("ready_sign");
		m_pTeamSelectBitmap = m_pUserList->GetBitmap("team_select");
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
		m_pDiveBitmap = m_pUserList->GetBitmap("dive");
		m_pKickBitmap = g_pFresh->RegisterBitmap("kick");
	}
}

// 0x500bc0
void FrGameRoomDlg::OnMapBackgroundInit(PG_MSGARG param) {
	m_pMapBackground = DYNAMIC_CAST(FrViewer, param);
	if (m_pMapBackground) {
		m_pMapBackground->SetMouseEvent(false);
	}
}

// 0x500c40
void FrGameRoomDlg::OnMapPrevInit(PG_MSGARG param) {
	m_pMapPrev = DYNAMIC_CAST(FrButton, param);
	if (m_pMapPrev) {
		m_pMapPrev->Enable(false);
		m_pMapPrev->SetVisible(false);
		m_pMapPrev->SetPushDelay(0.0f);
		m_pMapPrev->UseDblClick(false);
	}
}

// 0x500cf0
void FrGameRoomDlg::OnMapNextInit(PG_MSGARG param) {
	m_pMapNext = DYNAMIC_CAST(FrButton, param);
	if (m_pMapNext) {
		m_pMapNext->Enable(false);
		m_pMapNext->SetVisible(false);
		m_pMapNext->SetPushDelay(0.0f);
		m_pMapNext->UseDblClick(false);
	}
}

// 0x500da0
void FrGameRoomDlg::OnStartTipInit(PG_MSGARG param) {
	m_pStartTip = DYNAMIC_CAST(FrArea, param);
	if (m_pStartTip) {
		m_pStartTip->SetVisible(false);
	}
}

// 0x500e20
void FrGameRoomDlg::OnReadyTipInit(PG_MSGARG param) {
	m_pReadyTip = DYNAMIC_CAST(FrArea, param);
	if (m_pReadyTip) {
		m_pReadyTip->SetVisible(false);
	}
}

// 0x500ea0
void FrGameRoomDlg::OnStartInit(PG_MSGARG param) {
	m_pStart = DYNAMIC_CAST(FrButton, param);
}

// 0x500f10
void FrGameRoomDlg::OnReadyInit(PG_MSGARG param) {
	m_pReady = DYNAMIC_CAST(FrButton, param);
	if (m_pReady) {
		m_pReady->SetVisible(false);
		m_pReady->Enable(false);
	}
}

// 0x500fa0
void FrGameRoomDlg::OnChangeTeamInit(PG_MSGARG param) {
	m_pChangeTeam = DYNAMIC_CAST(FrButton, param);
	if (m_pChangeTeam) {
		m_pChangeTeam->SetPushDelay(0.5f);
	}
}

// 0x501030
void FrGameRoomDlg::OnPowerBarInit(PG_MSGARG param) {
	m_pPowerBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_pPowerBar->SetRange(0, 50, 0);
}

// 0x5010b0
void FrGameRoomDlg::OnControlBarInit(PG_MSGARG param) {
	m_pControlBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_pControlBar->SetRange(0, 30, 0);
}

// 0x501130
void FrGameRoomDlg::OnAccuracyBarInit(PG_MSGARG param) {
	m_pAccuracyBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_pAccuracyBar->SetRange(0, 30, 0);
}

// 0x5011b0
void FrGameRoomDlg::OnSpinBarInit(PG_MSGARG param) {
	m_pSpinBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_pSpinBar->SetRange(0, 30, 0);
}

// 0x501230
void FrGameRoomDlg::OnCurveBarInit(PG_MSGARG param) {
	m_pCurveBar = DYNAMIC_CAST(FrGaugeBar, param);
	m_pCurveBar->SetRange(0, 30, 0);
}

// 0x5012b0
void FrGameRoomDlg::OnEPowerInit(PG_MSGARG param) {
	m_pEPower = DYNAMIC_CAST(FrStatic, param);
}

// 0x501320
void FrGameRoomDlg::OnEControlInit(PG_MSGARG param) {
	m_pEControl = DYNAMIC_CAST(FrStatic, param);
}

// 0x501390
void FrGameRoomDlg::OnEAccuracyInit(PG_MSGARG param) {
	m_pEAccuracy = DYNAMIC_CAST(FrStatic, param);
}

// 0x501400
void FrGameRoomDlg::OnESpinInit(PG_MSGARG param) {
	m_pESpin = DYNAMIC_CAST(FrStatic, param);
}

// 0x501470
void FrGameRoomDlg::OnECurveInit(PG_MSGARG param) {
	m_pECurve = DYNAMIC_CAST(FrStatic, param);
}

// 0x5014e0
void FrGameRoomDlg::OnCurCaddieInit(PG_MSGARG param) {
	m_pCurCaddie = DYNAMIC_CAST(FrViewer, param);
}

// 0x501550
void FrGameRoomDlg::OnCurClubInit(PG_MSGARG param) {
	m_pCurClub = DYNAMIC_CAST(FrViewer, param);
}

// 0x5015c0
void FrGameRoomDlg::OnCurAztecInit(PG_MSGARG param) {
	m_pCurAztec = DYNAMIC_CAST(FrViewer, param);
}

// 0x501630
void FrGameRoomDlg::OnCurMascotInit(PG_MSGARG param) {
	m_pCurMascot = DYNAMIC_CAST(FrViewer, param);
}

// 0x5016a0
void FrGameRoomDlg::OnCurEquipItemInit(PG_MSGARG param) {
	m_pCurEquipItem = DYNAMIC_CAST(FrViewer, param);
}

// 0x501710
// The JP CLobbyMain::OnGameRoom_EquipItemOwnerDraw (KR, same shape; the
// JP disabled colour is 0x50b0b0b0 as in KR).
void FrGameRoomDlg::OnCurEquipItemOwnerDraw(PG_MSGARG) {
	if (!m_pCurEquipItem) {
		return;
	}
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI) {
		return;
	}
	unsigned long color = m_pCurEquipItem->IsEnabled() ? 0xFFFFFFFF : 0x50B0B0B0;
	const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "gameroom_equip_item_bn");
	if (pBmp) {
		pGDI->DrawTexture(pBmp, WRect(m_pCurEquipItem->GetRect().x, m_pCurEquipItem->GetRect().y, (float)pBmp->Width(), (float)pBmp->Height()), color, 0);
	}
	if (m_pCurEquipItem->IsEnabled()) {
		DrawFrameSelectedUserInfo(m_pCurEquipItem->GetRect());
	}
}

// 0x501820
void FrGameRoomDlg::OnCurCharSelInit(PG_MSGARG param) {
	m_pCurCharSel = DYNAMIC_CAST(FrViewer, param);
	if (m_pCurCharSel) {
		m_pCurCharSel->SetVisible(false);
	}
}

// 0x5018a0
void FrGameRoomDlg::OnCurCaddieSelInit(PG_MSGARG param) {
	m_pCurCaddieSel = DYNAMIC_CAST(FrViewer, param);
	if (m_pCurCaddieSel) {
		m_pCurCaddieSel->SetVisible(false);
	}
}

// 0x501920
void FrGameRoomDlg::OnCurClubSelInit(PG_MSGARG param) {
	m_pCurClubSel = DYNAMIC_CAST(FrViewer, param);
	if (m_pCurClubSel) {
		m_pCurClubSel->SetVisible(false);
	}
}

// 0x5019a0
void FrGameRoomDlg::OnCurAztecSelInit(PG_MSGARG param) {
	m_pCurAztecSel = DYNAMIC_CAST(FrViewer, param);
	if (m_pCurAztecSel) {
		m_pCurAztecSel->SetVisible(false);
	}
}

// 0x501a20
void FrGameRoomDlg::OnCurMascotSelInit(PG_MSGARG param) {
	m_pCurMascotSel = DYNAMIC_CAST(FrViewer, param);
	if (m_pCurMascotSel) {
		m_pCurMascotSel->SetVisible(false);
	}
}

// 0x501aa0
void FrGameRoomDlg::OnAtftInit(PG_MSGARG param) {
	m_pAtft = DYNAMIC_CAST(FrGroupBox, param);
	if (m_pAtft) {
		m_pAtft->SetVisible(false);
	}
}

// 0x501b20
void FrGameRoomDlg::OnArtifactIconInit(PG_MSGARG param) {
	m_pArtifactIcon = DYNAMIC_CAST(FrArea, param);
}

// 0x501b90
void FrGameRoomDlg::OnShortGameInit(PG_MSGARG param) {
	m_pShortGame = DYNAMIC_CAST(FrArea, param);
	if (!Jp_IsShortGameAvailable()) {
		m_pShortGame->SetVisible(false);
	}
}

// 0x501c10
void FrGameRoomDlg::OnGroup30sInit(PG_MSGARG param) {
	m_pGroup30s = DYNAMIC_CAST(FrGroupBox, param);
	if (m_pGroup30s) {
		m_pGroup30s->SetVisible(false);
	}
}

// 0x501c90
void FrGameRoomDlg::OnGroupStrokeInit(PG_MSGARG param) {
	m_pGroupStroke = DYNAMIC_CAST(FrGroupBox, param);
	if (m_pGroupStroke) {
		m_pGroupStroke->SetVisible(false);
	}
}

// 0x501d10
void FrGameRoomDlg::OnGroupResultInit(PG_MSGARG param) {
	m_pGroupResult = DYNAMIC_CAST(FrGroupBox, param);
	if (m_pGroupResult) {
		m_pGroupResult->SetVisible(false);
	}
}

// 0x501d90
void FrGameRoomDlg::OnGroupGPInit(PG_MSGARG param) {
	m_pGroupGP = DYNAMIC_CAST(FrGroupBox, param);
	if (m_pGroupGP) {
		m_pGroupGP->SetVisible(false);
	}
}

// The map prev / next order of the courses (0xd05e80, 21 bytes; name ours).
static const unsigned char s_roomCourseOrder[21] = {
	20, 21, 18, 19, 16, 15, 14, 13, 11, 8, 10, 0, 1, 2, 3, 4, 5, 6, 7, 9, 127
};

// A course the room may pick: any, unless local content 0x3b is on in a
// rookie channel (JP CSharedDoc +0x10a99 bit 0x800), where 0x633340 must
// allow it.
static bool JpCourse_IsSelectable(unsigned char course) {
	if (!IsLocalContent((localContentType_t)0x3b) || !JpSharedDoc_IsRookieChannel()) {
		return true;
	}
	return JpCourse_IsRookieAllowed(course);
}

// 0x502dc0 (fastcall, name ours): the selectable course before course in
// s_roomCourseOrder (a course missing from it counts as the last), else course.
static unsigned char JpCourse_GetPrev(unsigned char course) {
	int index = 20;
	for (int i = 0; i < 21; i++) {
		if (s_roomCourseOrder[i] == course) {
			index = i;
			break;
		}
	}
	for (int i = index - 1; i >= 0; i--) {
		if (JpCourse_IsSelectable(s_roomCourseOrder[i])) {
			return s_roomCourseOrder[i];
		}
	}
	return course;
}

// 0x502e40 (fastcall, name ours): the selectable course after course in
// s_roomCourseOrder (a course missing from it counts as the first), else course.
static unsigned char JpCourse_GetNext(unsigned char course) {
	int index = 0;
	for (int i = 0; i < 21; i++) {
		if (s_roomCourseOrder[i] == course) {
			index = i;
			break;
		}
	}
	for (int i = index + 1; i < 21; i++) {
		if (JpCourse_IsSelectable(s_roomCourseOrder[i])) {
			return s_roomCourseOrder[i];
		}
	}
	return course;
}

// 0x503290
void FrGameRoomDlg::InitRoom(void) {
	// The original also clears the raw byte 0x180.
	m_idleWarned = false;
	m_allReady = false;
	m_ready = false;
	m_readyLocked = false;
	if (WSingleton<CSharedDoc>::Instance()->m_userInfo.identity == 2) {
		m_ready = true;
		m_readyLocked = true;
	}
	m_readyRequested = false;
	m_pLobbyData->m_selectedUserId = 0xffffffff;
	m_pLobbyData->m_selectedUserNo = 0xffffffff;
	m_pBlinkWnd = NULL;
	m_pStartTip = NULL;
	m_pReadyTip = NULL;
	g_keyboard->ResetInputTime();
	g_mouse->ResetInputTime();
	// JP CSharedDoc +0x109b8 0x12: the social shuffle mode (0xd06088: no
	// entering a game in progress and no ticket report in that mode).
	if (JpSharedDoc_GetRoomGameMode() == 0x12) {
		CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0x23, (intptr_t)Jp_LookupText(
			"\xbd\xba\xc6\xf3\xbc\xc8 \xbc\xc5\xc7\xc3 \xb8\xf0\xb5\xe5\xbf\xa1\xbc\xad\xb4\xc2, \n"
			"\\c0xffff0000\\c\xb0\xd4\xc0\xd3 \xc1\xdf \xc0\xd4\xc0\xe5\\c0xff000000\\c \xb9\xd7 "
			"\\c0xffff0000\\c\xc6\xbc\xc5\xb0\xb8\xae\xc6\xf7\xc6\xae\\c0xff000000\\c "
			"\xbb\xe7\xbf\xeb\xc0\xcc \xba\xd2\xb0\xa1\xb4\xc9 \xc7\xd5\xb4\xcf\xb4\xd9."), 0, 0, 0);
	}
	if (RFTooltipHelper::Instance() != NULL) {
		RFTooltipHelper::Instance()->Reset();
	}
}

// 0x503340
void FrGameRoomDlg::ReleaseRoom(void) {
	if (m_pRoomData) {
		delete m_pRoomData;
		m_pRoomData = NULL;
	}
	if (RFLobbyDesktop::s_instance) {
		int lobbyState = RFLobbyDesktop::s_instance->GetLobbyState();
		int value = 0;
		if (lobbyState == 10) {
			value = 9;
		}
		if (lobbyState == 11) {
			value = 1;
		}
		if (value != 0) {
			CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0x59, value, 0, 0, 0);
		}
	}
}

// 0x5033b0
void FrGameRoomDlg::OpenNewRecordForm(void) {
	// JP CSharedDoc +0x10d42: the KR m_value4D7E.
	if (WSingleton<CSharedDoc>::Instance()->m_value4D7E != 0xFF && m_pNewRecordDlg == NULL) {
		m_pNewRecordDlg = CreateForm<FrNewRecordDlg>(g_pFresh->GetManager(), this, "new_record", NULL);
		m_pNewRecordDlg->Open((FRESH_RESULT_CALLBACK)&FrGameRoomDlg::OnNewRecordDlgResult, 0);
		WSingleton<CSharedDoc>::Instance()->m_value4D7E = 0xFF;
	}
}

// 0x503430
void FrGameRoomDlg::SetHolecupPosFixedMode(int mode) {
	if (m_pFixHolecupPos == NULL) {
		return;
	}
	if (mode == 7) {
		JpSharedDoc_SetHolecupPosFixed(true);
		m_pFixHolecupPos->SetButtonImg("single_font_on", FrButton::NORMAL);
		m_pFixHolecupPos->SetButtonImg("single_font_on", FrButton::OVER);
		m_pFixHolecupPos->SetButtonImg("single_font_on", FrButton::PRESSED);
		return;
	}
	JpSharedDoc_SetHolecupPosFixed(false);
	m_pFixHolecupPos->SetButtonImg("single_font_off", FrButton::NORMAL);
	m_pFixHolecupPos->SetButtonImg("single_font_off", FrButton::OVER);
	m_pFixHolecupPos->SetButtonImg("single_font_off", FrButton::PRESSED);
}

// 0x5034e0
void FrGameRoomDlg::FillHoleModeCombo(int selected) {
	if (!m_pMapCombo) {
		return;
	}
	m_pMapCombo->ClearAllListItem();
	m_pMapCombo->AddTexItem("hole_front");
	// JP CSharedDoc +0x1094b (room +0x47): the KR m_roomInfo.m_holeCount.
	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_holeCount < 0x12) {
		m_pMapCombo->AddTexItem("hole_back");
		m_pMapCombo->AddTexItem("hole_random");
	}
	m_pMapCombo->AddTexItem("hole_shuffle");
	if (selected == -1) {
		return;
	}
	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_holeCount == 0x12 && selected > 0) {
		selected = 1;
	}
	m_pMapCombo->SelectItem(selected);
}

// 0x503580
void FrGameRoomDlg::OnSelLBtnDown(void) {
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x50, 0, 0, 0, 0, 0));
	if (m_pCurCharSel) {
		m_pCurCharSel->SetVisible(false);
	}
	if (m_pCurCaddieSel) {
		m_pCurCaddieSel->SetVisible(false);
	}
	if (m_pCurAztecSel) {
		m_pCurAztecSel->SetVisible(false);
	}
	if (m_pCurClubSel) {
		m_pCurClubSel->SetVisible(false);
	}
	if (m_pCurMascotSel) {
		m_pCurMascotSel->SetVisible(false);
	}
}

// 0x503660
void FrGameRoomDlg::EnableEquipViewers(bool enable) {
	if (m_pCurCaddie) {
		m_pCurCaddie->Enable(enable);
	}
	if (m_pCurClub) {
		m_pCurClub->Enable(enable);
	}
	if (m_pCurAztec) {
		m_pCurAztec->Enable(enable);
	}
	if (m_pCurMascot) {
		m_pCurMascot->Enable(enable);
	}
	if (m_pCurEquipItem) {
		m_pCurEquipItem->Enable(enable);
	}
	OnSelLBtnDown();
}

// 0x503720
// The JP CLobbyMain::HandleMsg 0x2c (KR 0x2b): lock is 1 while the room
// is locked (m_readyLocked): the ready button (modes 0x14 / 0x15) and the
// start button are disabled, the blink window shows while unlocked.
void FrGameRoomDlg::LockRoomControls(intptr_t lock) {
	m_readyLocked = lock == 1;
	int mode = JpSharedDoc_GetRoomGameMode();
	if ((mode == 0x14 || mode == 0x15) && m_pReady) {
		m_pReady->Enable(!m_readyLocked);
	}
	if (m_pStart) {
		m_pStart->Enable(!m_readyLocked);
	}
	m_blinking = !m_readyLocked;
	if (m_pBlinkWnd) {
		m_pBlinkWnd->SetVisible(m_blinking);
	}
}

// 0x5037b0
void FrGameRoomDlg::SendLeaveFinishedGame(void) {
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x8b, 0xc, 0, 0, 0, 0));
}

// 0x5038d0
void FrGameRoomDlg::OnUserDblClick(void) {
	// JP CSharedDoc +0x812 bit 1 and +0x816: the KR m_userInfo's gallery
	// identity bit and galleryGuid.
	if (GalleryMode()) {
		if (MyGuid(true) != m_pLobbyData->m_selectedUserId) {
			WSendPacket packet(0x3f);
			packet.Encode4(m_pLobbyData->m_selectedUserNo);
			packet.Send(SEND_TO_DEFAULT);
		}
	}
}

// 0x503970
// The JP FrButton sits 4 bytes lower than the KR one from +0xfc: the stores
// to +0xf8 / +0x104 / +0x108 are the KR SetStyle and SetPushDelay inlines.
void FrGameRoomDlg::OnMapInit(PG_MSGARG param) {
	m_pMap = DYNAMIC_CAST(FrButton, param);
	if (!m_pMap) {
		return;
	}
	m_pMap->SetStyle(FrButton::BT_NORMAL);
	int gameType = JpSharedDoc_GetRoomGameType();
	if (gameType == 6 || gameType == 10) {
		m_pMap->SetStyle(FrButton::BT_ALWAYSON);
		m_pMap->SetPushDelay(0.0f);
		JpFrWnd_SetStyleFlag200(m_pMap);
	} else if (gameType == 0xb) {
		m_pMap->Enable(false);
	}
}

// 0x503a50
// The room option packet 0xa (0xffff, 1 option, option 3 = the course),
// then the main actor message 1.
void FrGameRoomDlg::OnMapPrevLBtnUp(void) {
	if (m_readyLocked) {
		return;
	}
	unsigned char course = JpCourse_GetPrev(JpSharedDoc_GetRoomCourse());
	WSendPacket packet(0xa);
	packet.Encode2(0xffff);
	packet.Encode1(1);
	packet.Encode1(3);
	packet.Encode1(course);
	packet.Send(SEND_TO_DEFAULT);
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
}

// 0x503b50
void FrGameRoomDlg::OnMapNextLBtnUp(void) {
	if (m_readyLocked) {
		return;
	}
	unsigned char course = JpCourse_GetNext(JpSharedDoc_GetRoomCourse());
	WSendPacket packet(0xa);
	packet.Encode2(0xffff);
	packet.Encode1(1);
	packet.Encode1(3);
	packet.Encode1(course);
	packet.Send(SEND_TO_DEFAULT);
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
}

// 0x503c50
void FrGameRoomDlg::OnMapComboInit(PG_MSGARG param) {
	m_pMapCombo = DYNAMIC_CAST(FrComboCtlEx, param);
	FillHoleModeCombo(-1);
	// JP CSharedDoc +0x1094f (room +0x4b): the KR m_roomInfo.m_value4B, the
	// hole mode the combo shows.
	sRoomInfo& room = WSingleton<CSharedDoc>::Instance()->m_roomInfo;
	unsigned char mode = room.m_value4B;
	if (m_pMapCombo) {
		if (room.m_holeCount == 0x12 && mode != 0) {
			mode = 1;
		}
		m_pMapCombo->SelectItem(mode);
	}
}

// 0x503cf0
// The hole mode the combo picked (the JP FrComboCtlEx's index byte +0x2f1,
// the KR GetCurrentIdx inline), sent as room option 5 when it differs from
// the room's (JP CSharedDoc +0x1094f, the KR m_roomInfo.m_value4B); with 18
// holes any mode past 0 is 3.
void FrGameRoomDlg::OnMapComboLBtnDown(void) {
	if (!m_pMapCombo || m_readyLocked) {
		return;
	}
	unsigned char selected = m_pMapCombo->GetCurrentIdx();
	sRoomInfo& room = WSingleton<CSharedDoc>::Instance()->m_roomInfo;
	if (room.m_holeCount == 0x12) {
		if (selected != 0) {
			if (room.m_value4B == 3) {
				return;
			}
			selected = 3;
		} else if (room.m_value4B == 0) {
			return;
		}
	} else if (room.m_value4B == selected) {
		return;
	}
	WSendPacket packet(0xa);
	packet.Encode2(0xffff);
	packet.Encode1(1);
	packet.Encode1(5);
	packet.Encode1(selected);
	packet.Send(SEND_TO_DEFAULT);
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
}

// 0x503e30
void FrGameRoomDlg::OnOverGaugeInit(PG_MSGARG param) {
	m_pOverGauge = DYNAMIC_CAST(FrGaugeBarImage, param);
	if (m_pOverGauge) {
		if (JpSharedDoc_GetRoomGameType() == 0xb) {
			m_pOverGauge->SetVisible(false);
			return;
		}
		m_pOverGauge->SetRange(0, 300, 0);
		unsigned char course = JpSharedDoc_GetRoomCourse();
		if (course < 0x7f) {
			uint32_t value = JpCourseTable_GetValue(course);
			if (value < 700) {
				m_pOverGauge->SetDestPos(0, false);
			} else {
				m_pOverGauge->SetDestPos(value - 700, false);
			}
		} else {
			m_pOverGauge->SetPos(0);
		}
	}
}

// 0x503f30
// The KR CLobbyMain's invite button (lobbymain.cpp:13160): the lobby's
// players (JP CSharedDoc +0x109d4, the KR m_userMap) in the "userlist" form
// (JP CreateForm 0x5027b0, 0x170 bytes, constructor 0xaf9080: the KR
// FrUserListDlg; MakeUserList 0xaf9cc0, EnableRightButton 0xaf96d0) at
// the right edge of the screen, 5 px in; its right button off while ready.
void FrGameRoomDlg::OnInviteLBtnUp(void) {
	if (m_pUserListDlg) {
		return;
	}
	m_pUserListDlg = CreateForm<FrUserListDlg>(g_pFresh->GetManager(), this, "userlist", NULL);
	if (!m_pUserListDlg) {
		return;
	}
	m_pUserListDlg->MakeUserList(WSingleton<CSharedDoc>::Instance()->m_userMap);
	m_pUserListDlg->Open((FRESH_RESULT_CALLBACK)&FrGameRoomDlg::OnUserListDlgResult, 3);
	WPoint pt;
	pt.x = g_view->GetWidth() - m_pUserListDlg->GetRect().w - 5.0;
	pt.y = m_pUserListDlg->GetRect().y;
	m_pUserListDlg->MoveWindow(pt);
	if (m_pUserListDlg) {
		m_pUserListDlg->EnableRightButton(m_ready == false);
	}
}

// 0x504000
void FrGameRoomDlg::OnModifyInit(PG_MSGARG param) {
	m_pModify = DYNAMIC_CAST(FrButton, param);
	if (m_pModify) {
		int mode = JpSharedDoc_GetRoomGameMode();
		if (mode == 0xc || mode == 0xe) {
			m_pModify->Enable(false);
			m_pModify->SetVisible(false);
		}
	}
}

// 0x5040a0
// The JP CLobbyMain::OnGameRoom_RoomOptionBtnUp (KR lobbymain.cpp, by the
// pairing).
void FrGameRoomDlg::OnModifyLBtnUp(void) {
	// "everyone is ready: the room settings can no longer be changed" (CP949)
	if (m_pBlinkWnd && m_pBlinkWnd->IsVisible() && !AdminMode()) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText(
			"\xb8\xf0\xb5\xce \xc1\xd8\xba\xf1\xb0\xa1 \xb5\xc8 \xbb\xf3\xc8\xb2\xbf\xa1\xbc\xad\xb4\xc2 \xb9\xe6\xc1\xa4\xba\xb8\xb8\xa6 \xba\xaf\xb0\xe6\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9"),
			0, 0, 0, 0));
		return;
	}
	FrChangeRoomInfoDlg* pDlg = CreateForm<FrChangeRoomInfoDlg>(g_pFresh->GetManager(), this, "changeroominfo", NULL);
	pDlg->SetDesc(Jp_LookupText("\xb9\xe6\xc0\xc7 \xbf\xc9\xbc\xc7\xc0\xbb \xba\xaf\xb0\xe6\xc7\xcf\xbc\xbc\xbf\xe4."));   // "change the room's options."
	// The single room (mode 0x13): JP CSharedDoc +0x1094f (the KR
	// m_roomInfo.m_value4B) == 4, the fixed hole cup position byte +0x11609
	// and the room's JpGameRoomOwned font.
	if (JpSharedDoc_GetRoomGameMode() == 0x13) {
		JpChangeRoomInfoDlg_SetSingleRoom(pDlg,
			WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_value4B == 4,
			JpSharedDoc_IsHolecupPosFixed(), m_pRoomOwned);
	}
	pDlg->Open(NULL, 1);
	pDlg->MoveCursor("ok");
}

// 0x5041c0
void FrGameRoomDlg::OnCurCaddieLBtnDown(void) {
	WRect rect = m_pCurCaddieSel->GetRect();
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x52, (intptr_t)&rect, 7, 1, 0, 0));
}

// 0x504260
void FrGameRoomDlg::OnCurClubLBtnDown(void) {
	WRect rect = m_pCurClubSel->GetRect();
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x52, (intptr_t)&rect, 4, 1, 0, 0));
}

// 0x504300
void FrGameRoomDlg::OnCurAztecLBtnDown(void) {
	WRect rect = m_pCurAztecSel->GetRect();
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x52, (intptr_t)&rect, 5, 1, 0, 0));
}

// 0x5043a0
void FrGameRoomDlg::OnCurMascotLBtnDown(void) {
	WRect rect = m_pCurMascotSel->GetRect();
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x52, (intptr_t)&rect, 0x10, 1, 0, 0));
}

// 0x504440
void FrGameRoomDlg::OnCurEquipItemLBtnDown(void) {
	OnSelLBtnDown();
	AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 0x8b, 5, 0, 0, 0, 0));
}

// 0x504500
void FrGameRoomDlg::OnAssistInit(PG_MSGARG param) {
	m_pAssist = DYNAMIC_CAST(FrButton, param);
	if (m_pAssist) {
		int mode = JpSharedDoc_GetRoomGameMode();
		if (mode == 0xc || mode == 0xe) {
			m_pAssist->SetVisible(false);
			return;
		}
		m_pAssist->SetStatus(JpSharedDoc_IsAssistOn() ? FrButton::PRESSED : FrButton::NORMAL);
	}
}

// 0x5045b0
void FrGameRoomDlg::OnAssistLBtnUp(void) {
	m_pAssist->Enable(false);
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
	WSendPacket packet(0x184);
	packet.Send(SEND_TO_DEFAULT);
}

// 0x504670
void FrGameRoomDlg::OnAssistTitleInit(PG_MSGARG param) {
	m_pAssistTitle = DYNAMIC_CAST(FrArea, param);
	if (m_pAssistTitle) {
		int mode = JpSharedDoc_GetRoomGameMode();
		if (mode == 0xc || mode == 0xe) {
			m_pAssistTitle->SetVisible(false);
		}
	}
}

// 0x505380
bool FrGameRoomDlg::OnApproachGiftDlgResult(int ret, FrForm* pForm) {
	m_pApproachGiftDlg = NULL;
	WSendPacket packet(0xcb);
	packet.Encode4(MyGuid(false));
	packet.Send(SEND_TO_DEFAULT);
	return true;
}

// 0x505410
// The KR CLobbyMain::OnRoomExtResQuitDlgResult (lobbymain.cpp:7027, by its
// shape): Yes disables "ok" and leaves the room (packet 0xf with the own
// pang of a type 4 / 5 game when tracked), clears the game over flag, sends
// the current task's main actor 1 and closes the room form; then always
// clears the notice and the own slot / room number (JP +0x7a0 / +0x1094d).
bool FrGameRoomDlg::OnLeaveGameNoticeResult(int ret, FrForm* pForm) {
	if (ret == 1) {
		FrWnd* pOk = pForm->FindChildByName("ok");
		if (pOk) {
			pOk->Enable(false);
		}
		WSendPacket packet(GCT_EXIT_ROOM);
		packet.Encode1(0);
		packet.Encode2(0xFFFF);
		sGameResultInfo* pMyResult = NULL;
		if (IsLocalContent((localContentType_t)0x14)) {
			CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
			if (doc->m_roomInfo.m_gameType == 4 || doc->m_roomInfo.m_gameType == 5) {
				for (std::vector<sGameResultInfo>::iterator it = doc->m_gameResultList.begin(); it != doc->m_gameResultList.end(); it++) {
					if (it->m_uid == MyGuid(false)) {
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
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
		Close(FrOK, true);
	}
	m_pLeaveGameNotice = NULL;
	WSingleton<CSharedDoc>::Instance()->m_mySlot = 0xFFFF;
	WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomNumber = 0xFFFF;
	return true;
}

// 0x5055c0
void FrGameRoomDlg::OpenLeaveGameNotice(void) {
	if (m_pLeaveGameNotice) {
		return;
	}
	m_pLeaveGameNotice = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_tiki_sub", 0);
	// "The game is still on. Wait until the other players finish. Going
	// back to the lobby now does not raise the forced-quit rate, but the
	// experience, items, records ... earned in the game are all void.
	// Give up the game and go back to the lobby?" (CP949)
	m_pLeaveGameNotice->SetMessage(Jp_LookupText(
		"\xbe\xc6\xc1\xf7 \xb0\xd4\xc0\xd3\xc0\xcc \xc1\xf8\xc7\xe0 \xc1\xdf \xc0\xd4\xb4\xcf\xb4\xd9.\n"
		"\xb4\xd9\xb8\xa5 \xc7\xc3\xb7\xb9\xc0\xcc\xbe\xee\xb5\xe9\xc0\xcc \xb8\xf0\xb5\xce \xbf\xcf\xc1\xd6\xc7\xd2 \xb6\xa7\xb1\xee\xc1\xf6\n"
		"\xb1\xe2\xb4\xd9\xb7\xc1 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0.\n\n"
		"\xc1\xf6\xb1\xdd \xb7\xce\xba\xf1\xb7\xce \xb3\xaa\xb0\xa1\xb0\xd4 \xb5\xc7\xb8\xe9,\n"
		"\\c0xffff0000\\c\xb0\xad\xc1\xa6\xc1\xbe\xb7\xe1\xc0\xb2\\c0xff000000\\c\xc0\xba \xbf\xc3\xb6\xf3\xb0\xa1\xc1\xf6 \xbe\xca\xc1\xf6\xb8\xb8\n"
		"\xb0\xd4\xc0\xd3 \xc1\xdf \xc8\xb9\xb5\xe6\xc7\xd1 "
		"\\c0xffff0000\\c\xb0\xe6\xc7\xe8\xc4\xa1, \xbe\xc6\xc0\xcc\xc5\xdb, \xb1\xe2\xb7\xcf \xb5\xee\xc0\xcc\n"
		"\xb8\xf0\xb5\xce \xb9\xab\xc8\xbf \xc3\xb3\xb8\xae\\c0xff000000\\c \xb5\xcb\xb4\xcf\xb4\xd9.\n\n"
		"\xb0\xd4\xc0\xd3\xc0\xbb \xc6\xf7\xb1\xe2\xc7\xcf\xb0\xed \xb7\xce\xba\xf1\xb7\xce \xb3\xaa\xb0\xa1\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?"),
		false);
	m_pLeaveGameNotice->Open((FRESH_RESULT_CALLBACK)&FrGameRoomDlg::OnLeaveGameNoticeResult, 0x801);
}

// 0x505800
// The KR CLobbyMain::OnGameRoom_RoomUserLBtnUp (by its shape): picks the
// row's player (in the event rooms, mode 0x14 / 0x15, a player with the
// JP-only record byte +0x159 set clears the pick), then, outside
// "GAMEROOM_EXTRES", the room master's click on another player's kick
// mark (the stroke room, lobby mode 3: at the team bar's right end; else
// at 227, 5) banishes that player, except in guild rooms (type 6) and,
// under local content 0x55, players who cannot be banished (JP record
// +0x154 bit 0, the KR m_banishFlags).
void FrGameRoomDlg::OnUserLBtnDown(void) {
	if (!m_pUserList) {
		return;
	}
	FrListItem* pItem = m_pUserList->GetItemUnderCursor();
	if (!pItem) {
		return;
	}
	sSlotInfo* pSlot = (sSlotInfo*)pItem->pData;
	if (!pSlot) {
		return;
	}
	int mode = JpSharedDoc_GetRoomGameMode();
	if ((mode != 0x14 && mode != 0x15) || !JpSlotInfo_IsEventExcluded(*pSlot)) {
		if (pSlot->m_uid != m_pLobbyData->m_selectedUserId) {
			m_pLobbyData->m_selectedUserId = pSlot->m_uid;
			// JP record +0x6c (the KR m_ruleUID +0x69: the JP record is
			// three bytes longer before it).
			m_pLobbyData->m_selectedUserNo = pSlot->m_ruleUID;
		}
	} else {
		m_pLobbyData->m_selectedUserId = 0xffffffff;
		m_pLobbyData->m_selectedUserNo = 0xffffffff;
	}
	WRect rect;
	if (JpSharedDoc_GetLobbyMode() != 3) {
		rect = WRect(227.0f, 5.0f, (float)m_pKickBitmap->Width(), (float)m_pKickBitmap->Height());
	} else {
		rect = WRect((float)m_pTeamBitmap[pSlot->m_attribute & 3]->Width() - 8.0 - (float)m_pKickBitmap->Width(), 8.0f,
			(float)m_pKickBitmap->Width(), (float)m_pKickBitmap->Height());
	}
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	mode = JpSharedDoc_GetRoomGameMode();
	if ((mode != 0x14 && mode != 0x15) || !JpSlotInfo_IsEventExcluded(*pSlot)) {
		FrWndManager* pManager = g_pFresh->GetManager();
		if (strcmp(pManager->GetLayoutID(), "GAMEROOM_EXTRES") != 0) {
			if (m_pLobbyData->m_hostUserId == doc->m_userInfo.guid
				&& m_pLobbyData->m_selectedUserId != doc->m_userInfo.guid
				&& (rect + pItem->pos).IsInRect(pManager->GetMousePos())
				&& doc->m_roomInfo.m_gameType != 6) {
				if (!IsLocalContent((localContentType_t)0x55) || !(pSlot->m_banishFlags & 1)) {
					Banish();
				}
			}
		}
	}
}

// 0x505a70
// The JP CLobbyMain::OnGameRoom_MapBtnUp (KR lobbymain.cpp, same shape):
// the room master opens the course selection "treasuremap" (alone in the
// room, before everyone is ready, or as an admin; not in the modes 0x14 /
// 0x15, nor in the game types 6, 10 and 11). JP difference: the dialog's
// +0x120 window is enabled outside the lobby mode 7.
void FrGameRoomDlg::OnMapLBtnUp(void) {
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if (m_pLobbyData->m_hostUserId != doc->m_userInfo.guid) {
		return;
	}
	if (m_allReady && doc->m_roomPlayerList.size() > 1 && !((doc->m_userInfo.identity >> 2) & 1)) {
		return;
	}
	int mode = JpSharedDoc_GetRoomGameMode();
	if (mode == 0x14 || mode == 0x15) {
		return;
	}
	int gameType = JpSharedDoc_GetRoomGameType();
	if (gameType == 6 || (gameType >= 10 && gameType <= 11) || m_pMapSelectDlg) {
		return;
	}
	m_pMapSelectDlg = CreateForm<FrTreasureCourse>(g_pFresh->GetManager(), this, "treasuremap", NULL);
	if (m_pMapSelectDlg) {
		m_pMapSelectDlg->SetSelectMap(JpSharedDoc_GetRoomCourse());
		m_pMapSelectDlg->SetMapIndexChanged(false);
		JpTreasureCourse_EnableWnd120(m_pMapSelectDlg, JpSharedDoc_GetLobbyMode() != 7);
		// JP CSharedDoc +0x10594: the KR m_roomType.
		m_pMapSelectDlg->LockRandom(doc->m_roomType == 2);
		m_pMapSelectDlg->EnableDrag(false);
		m_pMapSelectDlg->Open((FRESH_RESULT_CALLBACK)&FrGameRoomDlg::OnMapSelectDlgResult, 3);
	}
}

// 0x505bc0
void FrGameRoomDlg::OnReadyLBtnUp(void) {
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if (SharedDoc_IsControlServerService64(doc, 2) != 0) {
		// 0xd06358: the game functions are under maintenance.
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText(
			"\xb0\xd4\xc0\xd3 \xb0\xfc\xb7\xc3 \xba\xce\xba\xd0 \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9."),
			0, 0, 0, 0));
		return;
	}
	// JP CSharedDoc +0x81a: the KR m_userInfo.guid.
	if (MyGuid(false) == m_pLobbyData->m_hostUserId || m_readyLocked || m_readyRequested) {
		return;
	}
	m_readyRequested = true;
	int mode = JpSharedDoc_GetRoomGameMode();
	if ((mode == 0x14 || mode == 0x15) && JpSharedDoc_GetRoomSort() == 0) {
		WSendPacket packet(0xe);
		packet.Encode4(MyGuid(false));
		packet.Send(SEND_TO_DEFAULT);
		if (m_pReady) {
			m_pReady->Enable(false);
		}
		if (g_audio) {
			g_audio->PlaySfx("gp_in_lobby_start_5sec", NULL, 0, NULL, NULL, 0.5f, 200.0f);
		}
	} else if (!m_ready) {
		m_blinking = false;
		if (m_pBlinkWnd) {
			m_pBlinkWnd->SetVisible(false);
		}
		WSendPacket packet(0xd);
		packet.Encode1(0);
		packet.Send(SEND_TO_DEFAULT);
	} else {
		WSendPacket packet(0xd);
		packet.Encode1(1);
		packet.Send(SEND_TO_DEFAULT);
	}
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x4f, m_ready ? 1 : 0, 0, 0, 0, 0));
	EnableEquipViewers(m_ready);
}

// 0x506d80
// The KR CLobbyMain::RefreshAllUccClothes (lobbymain.cpp:17405, by its
// shape): while no user info dialog is open, up to ten downloaded UCC
// clothes go to the players of both team lists and of the user list.
void FrGameRoomDlg::ProcRoomExtras(void) {
	if (CUserInfo::Instance()->GetDlg()) {
		return;
	}
	sUccRefreshItem item;
	for (int i = 0; i < 10; i++) {
		if (!UccManager()->GetRefreshItem(item)) {
			return;
		}
		std::list<sSlotInfo*>::iterator it;
		for (it = m_slotList2.begin(); it != m_slotList2.end(); ++it) {
			JpRoom_RefreshUccClothes(*it, item.m_typeId, item.m_uccKey);
		}
		for (it = m_slotList1.begin(); it != m_slotList1.end(); ++it) {
			JpRoom_RefreshUccClothes(*it, item.m_typeId, item.m_uccKey);
		}
		if (m_pUserList) {
			for (int j = 0; j < m_pUserList->GetCurrentItemSize(false); j++) {
				FrListItem* listItem = m_pUserList->GetItem(j);
				if (listItem) {
					JpRoom_RefreshUccClothes((sSlotInfo*)listItem->pData, item.m_typeId, item.m_uccKey);
				}
			}
		}
	}
}

// 0x506f30
bool FrGameRoomDlg::CanLeaveFinishedGame(int state) {
	if (state != 8) {
		return false;
	}
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	// JP +0x812 (the KR m_userInfo.identity): gallery, administrator, GM.
	if (doc->m_userInfo.identity & 0xe) {
		return false;
	}
	// JP +0x375c: the KR m_itemInfoList.
	const std::list<sItemInfo>& items = doc->m_itemInfoList;
	std::list<sItemInfo>::const_iterator it = items.begin();
	while (it != items.end() && it->m_typeId != 0x1a000041) {
		++it;
	}
	if (it == items.end()) {
		return false;
	}
	bool result = doc->m_roomInfo.m_gameType != 6;
	if (doc->m_roomInfo.m_userLimit > 0x1e) {
		result = false;
	}
	if (m_pResultReportDlg) {
		result = false;
	}
	if (JpSharedDoc_IsRookieChannel()) {
		result = false;
	}
	// JP +0x91d: the KR m_myLevel.
	if (doc->m_myLevel <= 5) {
		result = false;
	}
	unsigned int count = 0;
	for (it = items.begin(); it != items.end(); ++it) {
		if (it->m_typeId == 0x1a000042) {
			count++;
		}
	}
	if (count >= 10) {
		result = false;
	}
	return result;
}

// 0x5076c0
void FrGameRoomDlg::OnChangeTeamLBtnUp(void) {
	if (m_ready || m_readyLocked) {
		return;
	}
	// JP CSharedDoc +0x109fc: the KR m_roomPlayerList (FillPlayerList); the
	// own player is the one whose uid is +0x81a (the KR m_userInfo.guid).
	// The JP record's byte +0x53 is the KR m_attribute (+0x54): team in
	// bits 0-1, room master in bits 3-4 (OnProc tests 0x18).
	std::list<sSlotInfo>& players = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList;
	std::list<sSlotInfo>::iterator it = players.begin();
	while (it != players.end() && it->m_uid != MyGuid(false)) {
		++it;
	}
	if (it != players.end()) {
		it->m_attributeBits.team = (1 - it->m_attributeBits.team) & 3;
		WSendPacket packet(0x10);
		packet.Encode1(it->m_attributeBits.team);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// 0x507780
// The JP CLobbyMain::OnGameRoom_AztecOwnerDraw (KR, same shape: JP
// CSharedDoc +0xa32 the KR m_myBallId, +0x3750 the KR m_myGearList). JP
// differences: local content 0x84 (KR 0x87); there a ball the item manager
// counts as uncounted (0xa18f50) shows no count; the count is printed with
// the wind font (+0x3e4, 0xbed090), never for the ball 0x140000d8.
void FrGameRoomDlg::OnCurAztecOwnerDraw(PG_MSGARG) {
	if (!m_pCurAztec) {
		return;
	}
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI) {
		return;
	}
	unsigned long color = m_pCurAztec->IsEnabled() ? 0xFFFFFFFF : 0x50B0B0B0;
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if (doc->m_myBallId == 0) {
		const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "w_caddie_no");
		if (pBmp) {
			pGDI->DrawTexture(pBmp, m_pCurAztec->GetRect(), color, 0);
		}
	} else {
		IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(doc->m_myBallId);
		if (pItem) {
			int count = 0;
			bool found = false;
			if (IsLocalContent((localContentType_t)0x84)) {
				std::list<sItemInfo>::iterator it;
				for (it = doc->m_myGearList.begin(); it != doc->m_myGearList.end(); ++it) {
					if (it->m_typeId == doc->m_myBallId) {
						found = true;
						count = JpItemManager_IsUncountedBall(doc->m_myBallId) ? 0 : it->m_count;
						break;
					}
				}
				// No such ball any more: back to the default ball.
				if (!found) {
					doc->m_myBallId = 0x14000000;
					pItem = ItemManager()->FindCommonItem(doc->m_myBallId);
				}
				if (pItem) {
					const Bitmap* pBmp = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->m_icon));
					if (pBmp) {
						pGDI->DrawTexture(pBmp, WRect(m_pCurAztec->GetRect().x, m_pCurAztec->GetRect().y, (float)pBmp->Width(), (float)pBmp->Height()), color, 0);
					}
				}
			} else {
				const Bitmap* pBmp = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->m_icon));
				if (pBmp) {
					pGDI->DrawTexture(pBmp, WRect(m_pCurAztec->GetRect().x, m_pCurAztec->GetRect().y, (float)pBmp->Width(), (float)pBmp->Height()), color, 0);
				}
				std::list<sItemInfo>::iterator it;
				for (it = doc->m_myGearList.begin(); it != doc->m_myGearList.end(); ++it) {
					if (it->m_typeId == doc->m_myBallId) {
						count = it->m_count;
						break;
					}
				}
			}
			if (count > 0 && doc->m_myBallId != 0x140000D8) {
				unsigned long textColor = m_pCurAztec->IsEnabled() ? 0xFFFFFFFF : 0x7FFFFFFF;
				JpGdi_PrintTitleFont(pGDI, m_pWindFont, m_pCurAztec->GetRect().x + 35.0f, m_pCurAztec->GetRect().y + 22.0f, MakeStr("%d", count), 1.0f, 2, 1.0f, textColor);
			}
		}
	}
	if (m_pCurAztec->IsEnabled()) {
		DrawFrameSelectedUserInfo(m_pCurAztec->GetRect());
	}
}

// 0x5074b0
// The JP CLobbyMain::SetStartBtn (KR, same shape: the JP CSharedDoc
// +0x109fc the KR m_roomPlayerList, record +0x54 bit 1 the KR
// m_mannerFlag ready bit, +0x53 & 0x18 the master). The JP game types 4,
// 5, 6, 9, 10, 11 and 18 (KR 4, 5, 6, 9, 10, 14) only need every guest
// ready: both JP branches of that case (the 0xe73dec +0x40 / capacity
// test) run the same loop.
void FrGameRoomDlg::SetStartBtn(bool check) {
	if (!check) {
		return;
	}
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	std::list<sSlotInfo>& players = doc->m_roomPlayerList;
	std::list<sSlotInfo>::iterator it;
	bool admin = ((doc->m_userInfo.identity >> 2) & 1) != 0;
	switch (JpSharedDoc_GetRoomGameType()) {
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
	case 11:
	case 18:
		m_allReady = true;
		for (it = players.begin(); it != players.end(); ++it) {
			if (!(it->m_mannerFlag & 2) && !(it->m_attribute & 0x18)) {
				m_allReady = false;
				break;
			}
		}
		break;
	default:
		if (players.size() == 1 && !admin) {
			m_allReady = false;
			break;
		}
		m_allReady = true;
		if (JpSharedDoc_GetRoomGameType() == 1) {
			// The team game: a full room with even teams.
			if (doc->m_roomInfo.m_userLimit > players.size()) {
				m_allReady = false;
			}
			if (m_allReady) {
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
				if (red != blue) {
					m_allReady = false;
				}
			}
		}
		if (m_allReady) {
			for (it = players.begin(); it != players.end(); ++it) {
				if (!(it->m_mannerFlag & 2) && !(it->m_attribute & 0x18)) {
					m_allReady = false;
					break;
				}
			}
		}
		break;
	}
	if (m_allReady && players.size() > 1) {
		if (m_pBlinkWnd) {
			m_blinkTimer = 0.0f;
			m_blinking = true;
		}
		if (m_pMapSelectDlg) {
			m_pMapSelectDlg->Close(true);
		}
	} else {
		m_blinking = false;
		if (m_pBlinkWnd) {
			m_pBlinkWnd->SetVisible(false);
		}
	}
}

// 0x5083c0
FrGameRoomDlg::FrGameRoomDlg(void) {
	// The original also zeroes raw bytes the header keeps as UNKNOWN (0x1b4, 0x1fc, 0x240-0x244, 0x24c-0x258,
	// 0x270, 0x374, 0x378, 0x380-0x390, 0x3c0, word 0x3dc); the port reads
	// none of them yet. It leaves m_roomState, m_pGroup30s,
	// m_pGroupGP, m_pFrameBg, m_pTeamBitmap, m_p30sSituBitmap,
	// m_pGenderIcon and m_gpWaiting unset (their Init handlers store them).
	m_pNewRecordDlg = NULL;
	m_artifactDropSent = false;
	m_pMapSelectDlg = NULL;
	m_pRoomControlWnd = NULL;
	m_pRoomSettingWnd = NULL;
	m_pGrandPrixRoom = NULL;
	m_lobbyValue = NULL;
	m_pLobbyTitleFont = NULL;
	m_pRoomOwned = NULL;
	m_pScoreDlg = NULL;
	m_pResultReportDlg = NULL;
	m_pApproachGiftDlg = NULL;
	m_pUserListDlg = NULL;
	m_pNotifyForm = NULL;
	m_blinkTimer = 0.0f;
	m_blinkCount = 0;
	m_blinking = false;
	m_addOwnPlayer = true;
	m_pUserList = NULL;
	m_pPet0 = NULL;
	m_pPet1 = NULL;
	m_pPet2 = NULL;
	m_pPet3 = NULL;
	m_pBackground = NULL;
	m_pMapBackground = NULL;
	m_pMap = NULL;
	m_pMapPrev = NULL;
	m_pMapNext = NULL;
	m_pMapCombo = NULL;
	m_pOverGauge = NULL;
	m_pCurCaddie = NULL;
	m_pCurAztec = NULL;
	m_pCurClub = NULL;
	m_pCurMascot = NULL;
	m_pCurEquipItem = NULL;
	m_pCurCharSel = NULL;
	m_pCurCaddieSel = NULL;
	m_pCurAztecSel = NULL;
	m_pCurClubSel = NULL;
	m_pCurMascotSel = NULL;
	m_pStart = NULL;
	m_pReady = NULL;
	m_pPowerBar = NULL;
	m_pControlBar = NULL;
	m_pAccuracyBar = NULL;
	m_pSpinBar = NULL;
	m_pCurveBar = NULL;
	m_pEPower = NULL;
	m_pEControl = NULL;
	m_pEAccuracy = NULL;
	m_pESpin = NULL;
	m_pECurve = NULL;
	m_pInvite = NULL;
	m_pChangeTeam = NULL;
	m_pResultScore = NULL;
	m_p30sTeamNum = NULL;
	m_p30sGuildNum = NULL;
	m_pResultRankFrame = NULL;
	m_p30sTeam1 = NULL;
	m_p30sTeam2 = NULL;
	m_p30sGuild1 = NULL;
	m_p30sGuild2 = NULL;
	m_pResultScoreGb = NULL;
	m_pStartTip = NULL;
	m_pReadyTip = NULL;
	m_pBlinkWnd = NULL;
	m_p30sCountdown = NULL;
	m_pResultRankingTab = NULL;
	m_pTitle = NULL;
	m_pModify = NULL;
	m_pResultUserInfo = NULL;
	m_pModeTitle = NULL;
	m_pCourseCtrl = NULL;
	m_pChipinCtrl = NULL;
	m_pBgCourse = NULL;
	m_pFixHole = NULL;
	m_pFixHolecupPos = NULL;
	m_pChipinSlope = NULL;
	m_pChipinSize = NULL;
	m_pChipinTicket = NULL;
	m_pAtft = NULL;
	m_pArtifactIcon = NULL;
	m_pAssist = NULL;
	m_pAssistTip = NULL;
	m_pAssistTitle = NULL;
	m_pShortGame = NULL;
	m_pGrandPrixInfo = NULL;
	m_pGPDetail = NULL;
	m_pGroupStroke = NULL;
	m_pGroupResult = NULL;
	m_pInviteBitmap = NULL;
	m_pMasterSignBitmap = NULL;
	m_pReadySignBitmap = NULL;
	m_pTeamSelectBitmap = NULL;
	m_pDiveBitmap = NULL;
	m_pKickBitmap = NULL;
	m_pResultRanking = NULL;
	m_pResultGuildRanking = NULL;
	m_p30sNoPlayerBase = NULL;
	m_p30sPlayerBase = NULL;
	m_pRoomEventBitmap = NULL;
	m_gpWaitTick0 = 0;
	m_gpWaitTick1 = 0;
	m_gpAddSlotTimeSec = 0.0f;
	m_gpGameStartWaitingTimeSec = 0.0f;
	m_assistTipShown = false;
	m_artifactTipShown = false;
	m_approachGiftTimer = 1.0f;
	m_roomTab = JpSharedDoc_GetCurrentRoomTab();
	// Left unset when the contents doc has no container 0xa5.
	WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0xa5, m_pLobbyData);
	m_pRoomData = new JpGameRoomData(JpSharedDoc_GetRoomCourse());

	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));
	info.filename = &s_windFontFile;
	info.numPages = 1;
	info.fontw = 16;
	info.fonth = 16;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = s_windFontCharSet;
	m_pWindFont = g_resrcmng->GetTitleFont();
	m_pWindFont->Create(&info);
}

// 0x5088c0
FrGameRoomDlg::~FrGameRoomDlg(void) {
	ReleaseRoom();
	if (m_pRoomOwned) {
		delete m_pRoomOwned;
		m_pRoomOwned = NULL;
	}
	if (g_resrcmng && m_pWindFont) {
		g_resrcmng->Release(m_pWindFont);
		m_pWindFont = NULL;
	}
}

// 0x508a50
// The JP CLobbyMain::HandleMsg 0x10 (KR lobbymain.cpp:3635). The JP room
// record (CSharedDoc +0x108f4 title, +0x10934 m_isPublic, +0x10937
// m_userLimit, +0x10938 m_userCount, +0x1094b m_holeCount, +0x1094c
// m_gameType, +0x1094d m_roomNumber, +0x10951 / +0x10955 the two time
// limits, +0x10959 m_trophy) is the KR m_roomInfo; the game type names
// (+0xc8, 0x16 entries) are GetGameTypeName's (0x502ed0, inlined here).
void FrGameRoomDlg::UpdateTitle(const char* text) {
	if (!m_pTitle) {
		return;
	}
	char timeText[0x20] = { 0 };
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	sRoomInfo& room = doc->m_roomInfo;
	if (text) {
		strcpy(timeText, text);
	} else if (IsMassGame(room.m_gameType)) {
		// The two event rooms (room mode 0x14 / 0x15) show the number and
		// the title only.
		int mode = JpSharedDoc_GetRoomGameMode();
		if (mode == 0x14 || mode == 0x15) {
			m_pTitle->SetLine(1, MakeStr(Jp_LookupText("[%d]   %s"), room.m_roomNumber, room.m_title), 0, false, 0);
			return;
		}
		if (room.m_gameType == 10) {
			sprintf(timeText, Jp_LookupText("%d\xC3\xCA"), room.m_value51 / 1000);   // "%d s"
		} else if (room.m_gameType == 6) {
			sprintf(timeText, "");
		} else {
			sprintf(timeText, Jp_LookupText("%d\xBA\xD0"), room.m_value51 / 60000);   // "%d min"
		}
	} else if (room.m_value4D == 0) {
		sprintf(timeText, Jp_LookupText("\xBD\xC3\xB0\xA3\xC1\xA6\xC7\xD1\xBE\xF8\xC0\xBD"));   // "no time limit"
	} else {
		sprintf(timeText, Jp_LookupText("%d\xC3\xCA"), room.m_value4D / 1000);   // "%d s"
	}

	// The mode name; the guild-free mass rooms (game type 11) also look the
	// room title up.
	const char* title = room.m_title;
	const char* modeName;
	if (IsMassGame(room.m_gameType)) {
		if (room.m_gameType == 10) {
			modeName = GetGameTypeName(room.m_gameType);
		} else if (room.m_gameType == 11) {
			modeName = GetGameTypeName(room.m_gameType);
			title = Jp_LookupText(room.m_title);
		} else {
			IFF_STRUCT::sMatch* match = ItemManager()->FindMatch(room.m_trophy);
			if (match == NULL) {
				modeName = GetGameTypeName(room.m_gameType);
			} else if (JpSharedDoc_GetRoomGameMode() != 0x12) {
				modeName = match->m_name;
			} else {
				modeName = Jp_LookupText("\xBC\xC5\xC7\xC3\xC4\xDA\xBD\xBA");   // "shuffle course"
			}
		}
	} else {
		modeName = GetGameTypeName(room.m_gameType);
	}
	const char* privateMark = room.m_isPublic != 0 ? "" : Jp_LookupText("[\xBA\xF1]");   // "[private]"
	// The JP drops the KR team marker (an empty string in its place).
	m_pTitle->SetLine(1, MakeStr(Jp_LookupText("[%d]   %s %s    %s %s  %2d\xC8\xA6  (%d/%d)  %s"),
		room.m_roomNumber, privateMark, title, modeName, "", room.m_holeCount,
		room.m_userCount, room.m_userLimit, timeText), 0, false, 0);

	// The room's artifact (JP CSharedDoc +0x109b9): the "atft" box and its
	// icon, and the icon's tooltip text.
	uint32_t artifact = JpSharedDoc_GetRoomArtifact();
	IFF_STRUCT::sItem* item = ItemManager()->FindItem(artifact);
	if (item == NULL || !JpItem_IsRoomArtifact(item)) {
		if (m_pAtft) {
			m_pAtft->SetVisible(false);
		}
		if (m_pArtifactIcon) {
			JpFrWnd_ClearFlag1cBit0(m_pArtifactIcon);
		}
		return;
	}
	if (m_pAtft) {
		m_pAtft->SetVisible(true);
	}
	if (!m_pArtifactIcon) {
		return;
	}
	const Bitmap* bitmap = g_pFresh->GetBitmap(MakeStr("s_%s", item->m_icon));
	if (bitmap) {
		m_pArtifactIcon->SetBgImg(bitmap);
	}
	const char* desc = JpItemManager_FindDescText(JpSharedDoc_GetRoomArtifact());
	if (desc == NULL) {
		desc = "";
	}
	m_artifactTipText = MakeStr("<font color=\"#ffaa00\">%s</font>\n%s", item->m_name, desc);
}

// 0x508ea0
void FrGameRoomDlg::RequestCloseInState8(void) {
	m_canLeaveFinishedGame = CanLeaveFinishedGame(m_roomState);
	if (m_pResultReportDlg) {
		return;
	}
	if (m_canLeaveFinishedGame && JpSharedDoc_GetRoomGameMode() != 0x12) {
		SendLeaveFinishedGame();
	} else {
		OpenLeaveGameNotice();
	}
	g_input->ExclusiveGetDownUseDone("ESCAPE");
}

// 0x508f10
void FrGameRoomDlg::HidePet(uint32_t uid) {
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	std::map<DWORD, sRoomSlot>::iterator it = doc->m_roomSlotMap.find(uid);
	if (it != doc->m_roomSlotMap.end()) {
		if (it->second.m_pPet) {
			delete it->second.m_pPet;
			it->second.m_pPet = NULL;
		}
	}
}

// 0x509440
// The JP CLobbyMain::HandleMsg 8, a player left the room (KR
// lobbymain.cpp:3396, same shape). JP differences: only the guild match
// (game type 6) rebuilds the teams; the moved players' pet areas come from
// their room position (JP record +0x2e).
void FrGameRoomDlg::OnPlayerLeft(intptr_t param1, intptr_t uid, intptr_t param3) {
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	int gameType = JpSharedDoc_GetRoomGameType();
	if (gameType != 6) {
		if (m_pUserList && param1) {
			m_pUserList->DelItem((void*)param1);
		}
	} else if (!doc->m_gameOver) {
		RemakeTeam(true);
	} else {
		std::list<sSlotInfo*>& slots = param3 ? m_slotList2 : m_slotList1;
		std::list<sSlotInfo*>::iterator it;
		for (it = slots.begin(); it != slots.end(); ++it) {
			if ((*it)->m_uid == (uint32_t)uid) {
				slots.erase(it);
				break;
			}
		}
		RemakeTeam(false);
	}
	if (gameType == 4 || gameType == 10) {
		// Keep 24 lines.
		if (m_pUserList && m_pUserList->GetCurrentItemSize(true) < 24) {
			m_pUserList->AddItem(NULL);
		}
		return;
	}
	if (IsMassGame((unsigned char)gameType)) {
		return;
	}
	HidePet((uint32_t)uid);
	std::map<DWORD, sRoomSlot>::iterator slot = doc->m_roomSlotMap.find((DWORD)uid);
	if (slot != doc->m_roomSlotMap.end()) {
		if (slot->second.m_pPetArea) {
			slot->second.m_pPetArea->SetMouseEvent(false);
		}
		doc->m_roomSlotMap.erase(slot);
	}
	// The players after the one who left moved up one place: each pet goes
	// to its new place's area.
	for (slot = doc->m_roomSlotMap.begin(); slot != doc->m_roomSlotMap.end(); ++slot) {
		std::list<sSlotInfo>& players = doc->m_roomPlayerList;
		std::list<sSlotInfo>::iterator player;
		for (player = players.begin(); player != players.end(); ++player) {
			if (player->m_uid == slot->first) {
				break;
			}
		}
		if (player != players.end()) {
			FrArea* area = (&m_pPet0)[JpSlotInfo_GetWaitingSlot(*player) - 1];
			slot->second.m_pPetArea = area;
			if (slot->second.m_pPet && area) {
				slot->second.m_pPet->SetCenter(area->GetRect().x + area->GetRect().w * 0.5f,
					area->GetRect().y + area->GetRect().h * 0.5f);
				if (slot->second.m_angelFlags.flag1) {
					slot->second.m_pPet->AttachAngelWingFx();
				}
			}
		}
	}
	// Keep 4 lines.
	if (m_pUserList && m_pUserList->GetCurrentItemSize(true) < 4) {
		m_pUserList->AddItem(NULL);
	}
}

// 0x509700
// The JP CLobbyMain::HandleMsg 9 (KR 9): the player's angel wing bits
// (record +0x56 bits 0-1, the KR m_flags57) into its room slot and into
// its row of the user list.
void FrGameRoomDlg::UpdatePlayerAngelFlags(sSlotInfo* player) {
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	std::map<DWORD, sRoomSlot>::iterator slot = doc->m_roomSlotMap.find(player->m_uid);
	if (slot != doc->m_roomSlotMap.end()) {
		slot->second.m_angelFlags.flag1 = player->m_flags57.flag1;
		slot->second.m_angelFlags.flag0 = player->m_flags57.flag0;
	}
	if (!m_pUserList) {
		return;
	}
	FrListBox::ITEM_LIST::iterator it;
	for (it = m_pUserList->m_itemList.begin(); it != m_pUserList->m_itemList.end(); ++it) {
		sSlotInfo* row = (sSlotInfo*)(*it)->pData;
		if (row && row->m_uid == player->m_uid) {
			row->m_flags57.flag0 = player->m_flags57.flag0;
			row->m_flags57.flag1 = player->m_flags57.flag1;
			break;
		}
	}
}

// 0x5097b0
void FrGameRoomDlg::OnTitleInit(PG_MSGARG param) {
	m_pTitle = DYNAMIC_CAST(FrEdit, param);
	if (!m_pTitle) {
		return;
	}
	UpdateTitle(NULL);
	JpSharedDoc_SortItemLists();
	// JP CSharedDoc +0x1095e (room +0x5a): the KR m_roomInfo.m_option[1].
	// The JP FrEdit's colour dword +0xe8 (the KR m_fontColor, 4 bytes lower).
	sRoomInfo& room = WSingleton<CSharedDoc>::Instance()->m_roomInfo;
	if (room.m_gameType != 0xb && room.m_option[1] != 0 && room.m_userLimit > 0x63) {
		m_pTitle->SetFontColor(0xff47a5dd);
		return;
	}
	m_pTitle->SetFontColor(0xffffffff);
}

// 0x509880
void FrGameRoomDlg::OnCloseWndLBtnUp(void) {
	if (m_roomState == 8) {
		RequestCloseInState8();
		return;
	}
	Close(FrOK, true);
}

// 0x5098a0
void FrGameRoomDlg::OnUserRBtnUp(void) {
	if (m_roomState == 6 && m_pLobbyData->m_userMenuBlocked) {
		return;
	}
	OnUserLBtnDown();
	// JP FrListBox +0x104: the KR m_pSelected offset.
	FrListItem* selected = m_pUserList->GetSelected();
	if (selected && selected->pData && JpUserInfo_IsCreated()) {
		JpRoomUser_OpenUserInfo(m_pLobbyData->m_selectedUserId);
	}
}

// 0x509960
void FrGameRoomDlg::OnStartLBtnUp(void) {
	if (!m_pStart || !m_pStart->IsEnabled()) {
		return;
	}
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if (SharedDoc_IsControlServerService64(doc, 2) != 0) {
		// 0xd06358: the game functions are under maintenance.
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText(
			"\xb0\xd4\xc0\xd3 \xb0\xfc\xb7\xc3 \xba\xce\xba\xd0 \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9."),
			0, 0, 0, 0));
		return;
	}
	// JP CSharedDoc +0x81a: the KR m_userInfo.guid.
	if (MyGuid(false) == m_pLobbyData->m_hostUserId) {
		if (!CLobbyMain_LatestVersion()) {
			return;
		}
		if (JpGameRoom_IsCourseBlocked()) {
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText(
				"\xb7\xe7\xc5\xb0\xc3\xa4\xb3\xce\xbf\xa1\xbc\xad\xb4\xc2 \xc0\xcc \xb8\xca\xc0\xbb "
				"\xc7\xc3\xb7\xb9\xc0\xcc \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."),
				0, 0, 0, 0));
			return;
		}
		if (JpGameRoom_OpenCardNotice(this)) {
			return;
		}
		const char* blockText = JpGameRoom_FindArtifactBlockText();
		if (blockText) {
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)blockText, 0, 0, 0, 0));
			return;
		}
		if (m_pStart) {
			m_pStart->Enable(false);
		}
		// JP +0x812 bit 2: the KR m_userInfo.identity admin bit
		// (Jp_IsAdminIdentity); 0x62bf30: JpSharedDoc_GetRoomSort.
		int mode = JpSharedDoc_GetRoomGameMode();
		if (!Jp_IsAdminIdentity(-1) || (mode != 0x14 && mode != 0x15) || JpSharedDoc_GetRoomSort() == 0) {
			WSendPacket packet(0xe);
			packet.Encode4(MyGuid(false));
			packet.Send(SEND_TO_DEFAULT);
			if (IsLocalContent((localContentType_t)0x55)) {
				JpGameRoom_ClearStartState55();
			}
		}
		return;
	}
	if (m_readyLocked || m_readyRequested) {
		return;
	}
	m_readyRequested = true;
	if (!m_ready) {
		m_blinking = false;
		if (m_pBlinkWnd) {
			m_pBlinkWnd->SetVisible(false);
		}
		WSendPacket packet(0xd);
		packet.Encode1(0);
		packet.Send(SEND_TO_DEFAULT);
	} else {
		WSendPacket packet(0xd);
		packet.Encode1(1);
		packet.Send(SEND_TO_DEFAULT);
	}
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x4f, m_ready ? 1 : 0, 0, 0, 0, 0));
	EnableEquipViewers(m_ready);
}

// 0x509f90
// The JP CLobbyMain::OnGameRoom_CaddieOwnerDraw (KR, same shape: JP
// CSharedDoc +0xa26 the KR m_myCaddieId, +0x3714 the KR m_caddieMap): the
// equipped caddie's icon, crossed by three red lines when its paid
// contract ran out (flag 2, no days left, no caddie item, a fee).
void FrGameRoomDlg::OnCurCaddieOwnerDraw(PG_MSGARG) {
	if (!m_pCurCaddie) {
		return;
	}
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI) {
		return;
	}
	unsigned long color = m_pCurCaddie->IsEnabled() ? 0xFFFFFFFF : 0x50B0B0B0;
	WRect rect = m_pCurCaddie->GetRect();
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if (doc->m_myCaddieId == 0) {
		const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "w_caddie_no");
		if (pBmp) {
			pGDI->DrawTexture(pBmp, WRect(m_pCurCaddie->GetRect().x, m_pCurCaddie->GetRect().y, (float)pBmp->Width(), (float)pBmp->Height()), color, 0);
		}
	} else {
		std::map<unsigned int, sCaddieInfo>::iterator it = doc->m_caddieMap.find(doc->m_myCaddieId);
		// The JP reads the found node without an end test (as KR); the port
		// does not dereference end().
		if (it != doc->m_caddieMap.end()) {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.typeId);
			if (pItem) {
				const Bitmap* pBmp = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->m_icon));
				if (pBmp) {
					pGDI->DrawTexture(pBmp, WRect(m_pCurCaddie->GetRect().x, m_pCurCaddie->GetRect().y, (float)pBmp->Width(), (float)pBmp->Height()), color, 0);
				}
			}
			IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(it->second.typeId);
			if (pCaddie && (it->second.flags & 2) && it->second.days <= 0 && it->second.cadItemTypeId == 0 && pCaddie->fee != 0) {
				g_view->DrawLine2D(WPoint(rect.x, rect.y + rect.h * 0.5f - 1.0f), WPoint(rect.x + rect.w - 1.0f, rect.y + rect.h * 0.5f - 1.0f), 0xFFFF0000, 0xFFFF0000, 0);
				g_view->DrawLine2D(WPoint(rect.x, rect.y + rect.h * 0.5f), WPoint(rect.x + rect.w - 1.0f, rect.y + rect.h * 0.5f), 0xFFFF0000, 0xFFFF0000, 0);
				g_view->DrawLine2D(WPoint(rect.x, rect.y + rect.h * 0.5f + 1.0f), WPoint(rect.x + rect.w - 1.0f, rect.y + rect.h * 0.5f + 1.0f), 0xFFFF0000, 0xFFFF0000, 0);
			}
		}
	}
	if (m_pCurCaddie->IsEnabled()) {
		DrawFrameSelectedUserInfo(m_pCurCaddie->GetRect());
	}
}

// 0x50a2a0
// The JP CLobbyMain::OnGameRoom_ClubOwnerDraw (KR, same shape: JP
// CSharedDoc +0xa2e the KR m_myClubId, +0x3740 the KR m_myItemMap).
void FrGameRoomDlg::OnCurClubOwnerDraw(PG_MSGARG) {
	if (!m_pCurClub) {
		return;
	}
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI) {
		return;
	}
	unsigned long color = m_pCurClub->IsEnabled() ? 0xFFFFFFFF : 0x50B0B0B0;
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if (doc->m_myClubId == 0) {
		const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "w_caddie_no");
		if (pBmp) {
			pGDI->DrawTexture(pBmp, m_pCurClub->GetRect(), color, 0);
		}
	} else {
		std::map<unsigned int, sItemInfo>::iterator it = doc->m_myItemMap.find(doc->m_myClubId);
		if (it != doc->m_myItemMap.end()) {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.m_typeId);
			if (pItem) {
				const Bitmap* pBmp = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->m_icon));
				if (pBmp) {
					pGDI->DrawTexture(pBmp, WRect(m_pCurClub->GetRect().x, m_pCurClub->GetRect().y, (float)pBmp->Width(), (float)pBmp->Height()), color, 0);
				}
			}
		}
	}
	if (m_pCurClub->IsEnabled()) {
		DrawFrameSelectedUserInfo(m_pCurClub->GetRect());
	}
}

// 0x50a430
// The JP CLobbyMain::OnGameRoom_MascotOwnerDraw (KR, same shape: JP
// CSharedDoc +0xa8e the KR m_myMascotId, +0x3724 the KR m_myMascotMap).
void FrGameRoomDlg::OnCurMascotOwnerDraw(PG_MSGARG) {
	if (!m_pCurMascot) {
		return;
	}
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI) {
		return;
	}
	unsigned long color = m_pCurMascot->IsEnabled() ? 0xFFFFFFFF : 0x50B0B0B0;
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if (doc->m_myMascotId == 0) {
		const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", "w_caddie_no");
		if (pBmp) {
			pGDI->DrawTexture(pBmp, WRect(m_pCurMascot->GetRect().x, m_pCurMascot->GetRect().y, (float)pBmp->Width(), (float)pBmp->Height()), color, 0);
		}
	} else {
		std::map<unsigned int, sMascotInfo>::iterator it = doc->m_myMascotMap.find(doc->m_myMascotId);
		// No end test in the JP (as KR); the port does not dereference end().
		if (it != doc->m_myMascotMap.end()) {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(it->second.typeId);
			if (pItem) {
				const Bitmap* pBmp = g_pFresh->GetBitmap(MakeStr("w_%s", pItem->m_icon));
				if (pBmp) {
					pGDI->DrawTexture(pBmp, WRect(m_pCurMascot->GetRect().x, m_pCurMascot->GetRect().y, (float)pBmp->Width(), (float)pBmp->Height()), color, 0);
				}
			}
		}
	}
	if (m_pCurMascot->IsEnabled()) {
		DrawFrameSelectedUserInfo(m_pCurMascot->GetRect());
	}
}

// 0x50ac10
IObject* FrGameRoomDlgMakeInstance(void) {
	return new FrGameRoomDlg;
}

// 0xccb920 (and 0x50ac80)
struct __sFrGameRoomDlg {
	__sFrGameRoomDlg(void) {
		ObjectFactory().AddObjectFunctor(FrGameRoomDlgMakeInstance, "FrGameRoomDlg");
	}
};
static __sFrGameRoomDlg s_registerFrGameRoomDlg;

// 0x50acd0
void FrGameRoomDlg::OnProc(const float dt) {
	FrChatDlg::OnProc(dt);
	if (g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) {
		if (g_input->GetDown("REPLAY", false)) {
			OnStartLBtnUp();
		}
	}

	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	// JP CSharedDoc +0x10488 (the KR m_gameOver, by m_roomType's
	// +0x10594 / 0x4668 delta).
	if (!doc->m_gameOver && m_blinking) {
		m_blinkTimer += dt;
		if (m_blinkTimer > 0.2) {
			m_blinkCount++;
			m_blinkTimer -= 0.2;
			if (m_pBlinkWnd) {
				m_pBlinkWnd->SetVisible(!m_pBlinkWnd->IsVisible());
			}
			if (m_blinkCount == 3) {
				m_blinking = false;
				m_blinkCount = 0;
			}
		}
	}

	if (g_input->GetDown("ESCAPE", true) == 1 && m_roomState == 8) {
		int mode = JpSharedDoc_GetRoomGameMode();
		if (mode != 0x14 && mode != 0x15 && IsMassGame() && doc->m_gameOver) {
			RequestCloseInState8();
		}
	}

	// The treasure approach gift (KR CLobbyMain::ProcessLobbyUI).
	if (m_approachGiftTimer <= 0.0f) {
		m_approachGiftTimer += dt;
		bool expired = false;
		if (m_approachGiftTimer > 0.0f) {
			expired = true;
		}
		if (g_input->GetDown("ESCAPE", true) == 1) {
			if (m_pResultReportDlg == NULL) {
				expired = true;
			} else if (m_pResultReportDlg->GetDone() && !m_pResultReportDlg->CheckChildDlgVisible()) {
				expired = true;
			}
		}
		if (expired) {
			m_approachGiftTimer = 1.0f;
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x51, 0, 0, 0, 0, 0));
			if (m_pResultReportDlg) {
				m_pResultReportDlg->Close(false);
			}
			if (m_pNewRecordDlg) {
				m_pNewRecordDlg->Close(true);
			}
			if (CUserInfo::Instance() && CUserInfo::Instance()->GetDlg()) {
				CUserInfo::Instance()->Close();
			}
			if (m_roomState == 8) {
				if (JpSharedDoc_GetRoomGameMode() == 0x12) {
					// 0xb2ef90(0): the room exit packet.
					JpAttendance_OnCheckInDone();
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
				} else {
					CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0x8b, 7, 0, 0, 0);
					CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0xf, 0, 0, 0, 0);
					CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0x30, 0, 0, 0, 0);
					Close(FrNONE, false);
				}
			}
		}
	} else if (m_pApproachGiftDlg) {
		m_pApproachGiftDlg->Close(true);
		m_pApproachGiftDlg = NULL;
	}

	// JP CSharedDoc +0x11424 / +0x10594: the KR m_value5D08 / m_roomType.
	if (doc->GetAppGiftDownLoadState() == CSharedDoc::APP_GIFT_DOWNLOAD_READY && doc->m_roomType == 10
		&& m_pApproachGiftDlg == NULL) {
		unsigned int giftCount = CTHunter::Instance()->GetGiftSize();
		if (giftCount <= 8) {
			m_approachGiftTimer = -9.0f;
		} else if (giftCount <= 16) {
			m_approachGiftTimer = -12.0f;
		} else {
			m_approachGiftTimer = -20.0f;
		}
		m_pApproachGiftDlg = CreateForm<FrTreasureGift>(g_pFresh->GetManager(), this, "trgift", NULL);
		m_pApproachGiftDlg->SetApproachMode();
		m_pApproachGiftDlg->Open((FRESH_RESULT_CALLBACK)&FrGameRoomDlg::OnApproachGiftDlgResult, 0x0C);
		m_pApproachGiftDlg->SetTimeLimit(12.0f);
		m_pApproachGiftDlg->EnableDrag(false);
		doc->m_value5D08 = 0;
	}

	// The idle check of the waiting states: a warning after 60 s without
	// input, the room master's kick after 300 s more.
	if (m_roomState == 3 || m_roomState == 4 || m_roomState == 6) {
		bool send = false;
		if (m_idleWarned) {
			if (g_keyboard->IsUpdated() || g_mouse->IsUpdated()) {
				send = true;
				m_idleWarned = false;
			}
			if (timeGetTime() - g_keyboard->GetLastInputTime() > 300000
				&& timeGetTime() - g_mouse->GetLastInputTime() > 300000) {
				std::list<sSlotInfo>& players = doc->m_roomPlayerList;
				std::list<sSlotInfo>::iterator it = players.begin();
				while (it != players.end() && it->m_uid != MyGuid(false)) {
					++it;
				}
				if (it != players.end() && it->m_attributeBits.roomMaster) {
					Close(FrOK, true);
					AfxGetTask()->GetActor("Lobby") << MsgObject(0, 1, 0, 0, 0, 0, 0);
				}
			}
		} else if (timeGetTime() - g_keyboard->GetLastInputTime() > 60000
			&& timeGetTime() - g_mouse->GetLastInputTime() > 60000) {
			m_idleWarned = true;
			send = true;
		}
		if (send) {
			std::list<sSlotInfo>& players = doc->m_roomPlayerList;
			std::list<sSlotInfo>::iterator it = players.begin();
			while (it != players.end() && it->m_uid != MyGuid(false)) {
				++it;
			}
			if (it != players.end()) {
				if (it->m_attributeBits.roomMaster) {
					WSendPacket packet(0xa);
					packet.Encode2(0xffff);
					packet.Encode1(1);
					packet.Encode1(9);
					packet.Encode1(m_idleWarned ? 1 : 0);
					packet.Send(SEND_TO_DEFAULT);
				}
				WSendPacket packet(0x32);
				packet.Encode1(m_idleWarned ? 1 : 0);
				packet.Send(SEND_TO_DEFAULT);
			}
		}
	}

	// The hover tooltips of the assist tip and the artifact icon.
	WVector mouse = g_input->GetMousePoint();
	if (m_pAssistTip && m_pAssistTip->IsVisible() && RFTooltipHelper::Instance() != NULL) {
		if (JpRect_Contains(m_pAssistTip->GetRect(), mouse.x, mouse.y)) {
			RFTooltipHelper::Instance()->ShowTooltip(NULL, m_assistTipText, 1, 0xb);
			m_assistTipShown = true;
		} else if (m_assistTipShown) {
			RFTooltipHelper::Instance()->HideTooltip();
			m_assistTipShown = false;
		}
	}
	if (m_pArtifactIcon && m_pArtifactIcon->IsVisible() && RFTooltipHelper::Instance() != NULL) {
		if (JpRect_Contains(m_pArtifactIcon->GetRect(), mouse.x, mouse.y)) {
			RFTooltipHelper::Instance()->ShowTooltip(NULL, m_artifactTipText, 1, 0xb);
			m_artifactTipShown = true;
		} else if (m_artifactTipShown) {
			RFTooltipHelper::Instance()->HideTooltip();
			m_artifactTipShown = false;
		}
	}

	ProcRoomExtras();
	if (m_pRoomData) {
		m_pRoomData->Update(dt);
	}
}

// 0x50b570
void FrGameRoomDlg::SetLevelBar(void) {
	Jp_CardManager_RefreshMasteryStats();
	JpMasteryStatInfo statInfo;
	// The original walks +0x200 and +0x214 as two arrays of five.
	FrGaugeBar* bars[5] = { m_pPowerBar, m_pControlBar, m_pAccuracyBar, m_pSpinBar, m_pCurveBar };
	FrStatic* values[5] = { m_pEPower, m_pEControl, m_pEAccuracy, m_pESpin, m_pECurve };
	for (int i = 0; i < 5; i++) {
		int value = statInfo.GetStatValue(i);
		int capacity = value + statInfo.GetFreeMasteryPoints(i);
		if (bars[i]) {
			bars[i]->SetDestPos(value, false);
			bars[i]->SetExpand(capacity - value);
		}
		if (values[i]) {
			values[i]->SetCaption11(MakeStr("%d/%d", value, capacity));
		}
	}
}

// 0x50b850
void FrGameRoomDlg::AddPlayer(uint32_t key) {
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	std::map<DWORD, sRoomSlot>::iterator it = doc->m_roomSlotMap.find(key);
	if (it == doc->m_roomSlotMap.end()) {
		return;
	}
	HidePet(key);
	if (key == MyGuid(true)) {
		SetLevelBar();
	}
	sRoomSlot* slot = &it->second;
	// The pet area: by the player's room position in the waiting room
	// (state 3), the first one in the states 2, 4 and 7.
	if (m_roomState == 3) {
		std::list<sSlotInfo>::iterator player = doc->m_roomPlayerList.begin();
		while (player != doc->m_roomPlayerList.end() && player->m_uid != key) {
			++player;
		}
		if (player == doc->m_roomPlayerList.end()) {
			return;
		}
		// The JP reads this +0x190 + 4 * position: the 1-based position picks
		// m_pPet0..m_pPet3 (the KR m_petArea[m_connection - 1]).
		slot->m_pPetArea = (&m_pPet0)[JpSlotInfo_GetWaitingSlot(*player) - 1];
	} else if (m_roomState == 2 || m_roomState == 4) {
		slot->m_pPetArea = m_pPet0;
	}
	if (m_roomState == 7) {
		slot->m_pPetArea = m_pPet0;
	}
	if (!slot->m_pPetArea || !g_pFresh->GetManager()->IsValidWindow(slot->m_pPetArea)) {
		return;
	}
	slot->m_pPetArea->SetMouseEvent(key == doc->m_userInfo.guid);
	if (GalleryMode()) {
		if (doc->m_userInfo.galleryGuid == 0xFFFFFFFF) {
			return;
		}
		if (!slot->m_partTidList.SetTids(&slot->m_characterInfo, 0xFF) || !slot->m_partTidList.IsComboValid()) {
			slot->m_partTidList.m_tidChar = slot->m_characterInfo.typeId;
			slot->m_partTidList.SetDefaultTids();
		}
	} else {
		if (!slot->m_partTidList.SetTids(&slot->m_characterInfo, 0xFF) || !slot->m_partTidList.IsComboValid()) {
			slot->m_partTidList.m_tidChar = slot->m_characterInfo.typeId;
			slot->m_partTidList.SetDefaultTids();
		}
		if (key == doc->m_userInfo.guid) {
			doc->BuildMyPartTidList();
		}
	}
	WRect rect(slot->m_pPetArea->GetRect());
	uint32_t petValue = JpRoomSlot_GetUid(*slot);
	JpRoomPet_Notify(petValue, &slot->m_characterInfo);
	slot->m_pPet = new CExhibitionByHardwareTnL(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.35f,
		key != doc->m_userInfo.guid, petValue);
	slot->m_pPet->SetDistanceRange(10.0f, 14.0f);
	slot->m_pPet->SetDistance(12.0f, true, false);
	slot->m_pPet->SetArea(rect.w, rect.h);
	slot->m_pPet->SetModel(slot->m_partTidList, slot->m_characterInfo.auxParts.tids, 0, 3.0f, key);
	if (slot->m_angelFlag & 2) {
		slot->m_pPet->AttachAngelWingFx();
	}
	if (key == doc->m_userInfo.guid) {
		if (!strcmpi(g_pFresh->GetManager()->GetLayoutID(), "CREATE")) {
			slot->m_pPet->SetControlFlag(0x1f);
		} else {
			slot->m_pPet->SetControlFlag(0x1a);
		}
	} else {
		slot->m_pPet->SetControlFlag(0x10);
	}
}

// 0x50c000
void FrGameRoomDlg::FillPlayerList(void) {
	// JP CSharedDoc +0x109fc (room record +0xf8, 0x18 after the room list
	// +0x109e4): the KR m_roomPlayerList, 0x18 after m_roomList too.
	std::list<sSlotInfo>& players = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList;
	for (std::list<sSlotInfo>::iterator it = players.begin(); it != players.end(); ++it) {
		if (m_pUserList) {
			m_pUserList->AddItem(&*it);
		}
		AddPlayer(it->m_uid);
	}
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x11, JpSharedDoc_GetRoomCourse(), 0, 0, 0, 0));
	OpenNewRecordForm();
}

// 0x50c0b0
// The JP CLobbyMain::SetRoomControls (KR lobbymain.cpp:7987, same shape).
// JP differences: the player count is the room's byte (JP CSharedDoc
// +0x10a00, the KR m_roomPlayerList.size()); the admin test is the
// identity bit 2 everywhere; room mode 0x12 (KR 0xe) and game type 0xb
// lock the settings too, the channel flag 0x80 does not; no map lock area;
// the next course test is != LastMap(hole mode 4); the start button of
// the master follows the room (a 100-player room with the option byte
// +0x5a: from one player; else from two players once m_allReady is set,
// one in the lobby mode 7, or m_allReady alone while the card item room
// test 0x507ad0 holds); a guest's ready button is disabled in the modes
// 0x14 / 0x15 by the room sort; the room setting window replaces the KR
// channel room close button; local content 0x55 stores the master flag
// into the JP object 0xe73de8 (+0xd8).
void FrGameRoomDlg::SetRoomControls(bool master) {
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	sRoomInfo& room = doc->m_roomInfo;
	unsigned char playerCount = (unsigned char)doc->m_roomPlayerList.size();
	int gameType = JpSharedDoc_GetRoomGameType();
	int roomMode = JpSharedDoc_GetRoomGameMode();
	unsigned char course = JpSharedDoc_GetRoomCourse();
	// A rookie channel locks the courses past the rookie ones.
	bool lockedCourse = false;
	if (doc->m_currentChannel.flags & 0x800) {
		IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(0x28000000 | ((course > 0x7f && course != 0xfd) ? course - 0x80 : course));
		if (pCourse && (pCourse->courseFlags & 0xf) >= 3) {
			lockedCourse = true;
		}
	}
	bool admin = ((doc->m_userInfo.identity >> 2) & 1) != 0;
	if (admin) {
		lockedCourse = false;
	}
	// The settings may change alone in the room, before everyone is ready,
	// or as an admin.
	bool canChange = playerCount == 1 || !m_allReady || admin;

	if (m_pModify) {
		if (master) {
			m_pModify->Enable(canChange && roomMode != 0x12 && gameType != 0xb);
		} else {
			m_pModify->Enable(false);
		}
	}
	if (m_pRoomControlWnd) {
		if (gameType == 0xa || roomMode == 0x12 || !master) {
			m_pRoomControlWnd->Enable(false);
		} else {
			m_pRoomControlWnd->Enable(canChange);
		}
	}
	if (m_pMapPrev) {
		if (master) {
			m_pMapPrev->SetVisible(canChange && roomMode != 0x12 && gameType != 0xb);
			if (gameType != 0xa) {
				m_pMapPrev->Enable(course != FirstMap());
			} else {
				m_pMapPrev->Enable(false);
			}
		} else {
			m_pMapPrev->SetVisible(false);
		}
	}
	if (m_pMapNext) {
		if (master) {
			m_pMapNext->SetVisible(canChange && roomMode != 0x12 && gameType != 0xb);
			if (gameType != 0xa) {
				// JP CSharedDoc +0x1094f: the KR m_roomInfo.m_value4B (hole mode).
				m_pMapNext->Enable(course != LastMap(room.m_value4B == 4));
			} else {
				m_pMapNext->Enable(false);
			}
		} else {
			m_pMapNext->SetVisible(false);
		}
	}
	if (m_pMapCombo) {
		if (lockedCourse || (doc->m_currentChannel.flags & 0x10)
			|| gameType == 0xa || roomMode == 0x12 || gameType == 0xb || !master) {
			m_pMapCombo->Enable(false);
		} else {
			m_pMapCombo->Enable(canChange);
		}
	}

	if (m_pStart) {
		if (master) {
			m_pStart->SetVisible(true);
			m_pStart->Enable(true);
			if (m_pReady) {
				m_pReady->SetVisible(false);
				if (m_pReady) {
					m_pReady->Enable(false);
				}
			}
			// JP room +0x33 (the KR m_userLimit) and +0x5a (m_option[1]).
			if ((gameType != 0xb && room.m_option[1] != 0 && room.m_userLimit > 99)
				|| (doc->m_userInfo.identity & 4) || (doc->m_userInfo.identity & 0x8000)) {
				m_pStart->Enable(playerCount >= 1);
			} else if (!JpGameRoom_IsCardItemRoom()) {
				if (JpSharedDoc_GetLobbyMode() == 7) {
					m_pStart->Enable(m_allReady && playerCount >= 1);
				} else {
					m_pStart->Enable(m_allReady && playerCount > 1);
				}
			} else {
				m_pStart->Enable(m_allReady);
			}
		} else if (!m_readyRequested) {
			m_pReady->SetButtonImg(m_ready ? "btn_ready_cancel_n" : "btn_ready_n", FrButton::NORMAL);
			m_pReady->SetButtonImg(m_ready ? "btn_ready_cancel_o" : "btn_ready_o", FrButton::OVER);
			m_pReady->SetVisible(true);
			m_pReady->Enable(true);
			m_pStart->SetVisible(false);
			m_pStart->Enable(true);
			if ((roomMode == 0x14 || roomMode == 0x15) && m_addOwnPlayer && JpSharedDoc_GetRoomSort() != 0) {
				m_pReady->Enable(false);
			}
		}
	}

	if (m_pRoomSettingWnd) {
		// JP +0x812 bit 1: GalleryMode (the KR m_userInfo.identity).
		if (!(doc->m_userInfo.identity & 2)) {
			if (master) {
				m_pRoomSettingWnd->Enable(true);
			} else {
				m_pRoomSettingWnd->Enable(!m_ready);
			}
		} else {
			m_pRoomSettingWnd->Enable((doc->m_userInfo.identity & 0xe) == 0xe && doc->m_userInfo.galleryGuid != 0xffffffff);
		}
		if (doc->m_currentChannel.flags & 8) {
			m_pRoomSettingWnd->Enable(false);
		}
	}

	SetRoomCharControl(2);
	SetRoomCaddieControl(2);

	// The guild match fixes the course.
	if (gameType == 6) {
		if (m_pRoomControlWnd) {
			m_pRoomControlWnd->Enable(false);
		}
		if (m_pMapPrev) {
			m_pMapPrev->Enable(false);
		}
		if (m_pMapNext) {
			m_pMapNext->Enable(false);
		}
		if (m_pMapCombo) {
			m_pMapCombo->Enable(false);
		}
		if (m_roomState == 8) {
			RemakeTeam(true);
		}
	}
	if (IsLocalContent((localContentType_t)0x55)) {
		JpGameRoom_SetStartState55(master);
	}
}

// 0x50c760
// The JP CLobbyMain::HandleMsg 0xf room master update (KR CLobbyMain 0xf
// by its place): the host is the master of the first list (+0x10a08, the
// KR m_roomPlayerList2), then of the room players; an admin is always the
// master. Then the ready state is reset, the blink window becomes the
// start tip, a room artifact the master cannot use is dropped (packet 0xa,
// option 0xd = 0, once, in the states 3 and 4), and the map button, the
// start state, the room controls and the room control window follow.
void FrGameRoomDlg::UpdateRoomMaster(void) {
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	m_pLobbyData->m_hostUserId = 0xffffffff;
	bool master = false;
	if (!((doc->m_userInfo.identity >> 2) & 1)) {
		std::list<sSlotInfo>::iterator it;
		for (it = doc->m_roomPlayerList2.begin(); it != doc->m_roomPlayerList2.end(); ++it) {
			if (it->m_attribute & 0x18) {
				m_pLobbyData->m_hostUserId = it->m_uid;
				if (it->m_uid == doc->m_userInfo.guid) {
					master = true;
				}
				break;
			}
		}
		for (it = doc->m_roomPlayerList.begin(); it != doc->m_roomPlayerList.end(); ++it) {
			if (it->m_attribute & 0x18) {
				m_pLobbyData->m_hostUserId = it->m_uid;
				if (it->m_uid == doc->m_userInfo.guid) {
					master = true;
				}
				break;
			}
		}
	} else {
		master = true;
		m_pLobbyData->m_hostUserId = doc->m_userInfo.guid;
	}
	m_ready = false;
	m_readyRequested = false;
	m_pBlinkWnd = m_pStartTip;
	if (JpSharedDoc_GetRoomArtifact() != 0 && !m_artifactDropSent) {
		if (JpGameRoom_FindArtifactBlockText() && (m_roomState == 3 || m_roomState == 4)) {
			WSendPacket packet(0xa);
			packet.Encode2(0xffff);
			packet.Encode1(1);
			packet.Encode1(0xd);
			packet.Encode4(0);
			packet.Send(SEND_TO_DEFAULT);
			m_artifactDropSent = true;
		}
	}
	if (m_pMap) {
		m_pMap->Enable(JpSharedDoc_GetRoomGameMode() != 0x12 ? master : false);
	}
	SetStartBtn(master);
	SetRoomControls(master);
	if (m_pRoomControlWnd) {
		m_pRoomControlWnd->Enable(master && JpSharedDoc_GetRoomGameType() != 6);
	}
}

// 0x50ca30
// The JP CLobbyMain::HandleMsg 0x2a (KR 0x29, same shape): state 2 is a
// refused ready ("you cannot get ready now, try again in a moment"); for
// the own player (or every player, uid -1) the own ready state, the blink,
// the team change button (team game), the user list's and the "userlist"
// form's right button; then the player record's ready bit (JP record +0x54
// bit 1, the KR m_mannerFlag), the master test, SetStartBtn and the room
// controls.
void FrGameRoomDlg::SetPlayerReady(intptr_t uid, intptr_t state) {
	if (state == 2) {
		m_readyRequested = false;
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText(
			"\xc1\xf6\xb1\xdd\xc0\xba \xb7\xb9\xb5\xf0\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xe1\xbd\xc3\xc8\xc4 \xb4\xd9\xbd\xc3 \xb7\xb9\xb5\xf0\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4."),
			0, 0, 0, 0));
		return;
	}
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if ((uint32_t)uid == doc->m_userInfo.guid || (uint32_t)uid == 0xffffffff) {
		m_readyRequested = false;
		m_ready = state == 0;
		m_blinking = state == 1;
		if (JpSharedDoc_GetRoomGameType() == 1 && m_pChangeTeam) {
			m_pChangeTeam->Enable(!m_ready);
		}
		if (m_pUserList) {
			m_pUserList->UseRightButton(!m_ready);
		}
		if (m_pUserListDlg) {
			m_pUserListDlg->EnableRightButton(!m_ready);
		}
	}
	std::list<sSlotInfo>& players = doc->m_roomPlayerList;
	std::list<sSlotInfo>::iterator it;
	if ((uint32_t)uid != 0xffffffff) {
		for (it = players.begin(); it != players.end(); ++it) {
			if (it->m_uid == (uint32_t)uid) {
				it->m_mannerFlag = (it->m_mannerFlag & ~2) | (state == 0 ? 2 : 0);
				break;
			}
		}
	} else {
		for (it = players.begin(); it != players.end(); ++it) {
			it->m_mannerFlag = (it->m_mannerFlag & ~2) | (state == 0 ? 2 : 0);
		}
	}
	bool master = false;
	for (it = players.begin(); it != players.end(); ++it) {
		if (it->m_attribute & 0x18) {
			if (it->m_uid == doc->m_userInfo.guid) {
				master = true;
			}
			break;
		}
	}
	if ((doc->m_userInfo.identity >> 2) & 1) {
		master = true;
	}
	SetStartBtn(master);
	SetRoomControls(master);
}

// 0x50cc20
void FrGameRoomDlg::OnAssistTipInit(PG_MSGARG param) {
	m_pAssistTip = DYNAMIC_CAST(FrArea, param);
	if (m_pAssistTip) {
		int mode = JpSharedDoc_GetRoomGameMode();
		if (mode == 0xc || mode == 0xe) {
			m_pAssistTip->SetVisible(false);
		}
		m_assistTipText = std::string(Jp_GetTableText(0x187cd));
	}
}

// 0x50d0e0
bool FrGameRoomDlg::OnInit(void) {
	FillPlayerList();
	JpFrWnd_RaiseFlagF1aac8();
	if (RFLobbyDesktop::s_instance) {
		CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0x59, 10, 0, 0, 0);
	}
	// FrForm's (0xbe6570), not FrChatDlg's.
	return FrForm::OnInit();
}

// 0x50d120
// The JP CLobbyMain::HandleMsg 0x11 (KR lobbymain.cpp:3711).
void FrGameRoomDlg::SetCourseImages(unsigned char course) {
	IFF_STRUCT::sCourse* pCourse = ItemManager()->FindCourse(0x28000000 | (course >= 0x7F ? 0x7F : course));
	if (pCourse) {
		if (m_pMap) {
			m_pMap->SetButtonImg(MakeStr("big_%s", pCourse->m_icon), FrButton::NORMAL);
		}
		if (m_pBackground) {
			m_pBackground->Open(MakeStr("background%s.tga", strstr(pCourse->m_icon, "_")));
		}
		if (m_pMapBackground) {
			m_pMapBackground->Open("map_background.tga");
		}
	}
	if (m_pOverGauge) {
		m_pOverGauge->ResetGauge();
		uint32_t pos;
		if (course < 0x7F) {
			// The room's course (JP CSharedDoc +0x10950), not the argument.
			uint32_t value = JpCourseTable_GetValue(JpSharedDoc_GetRoomCourse());
			pos = value >= 701 ? value - 700 : 0;
		} else {
			pos = 0;
		}
		m_pOverGauge->SetPos(pos);
	}
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	std::list<sSlotInfo>::iterator it = doc->m_roomPlayerList.begin();
	while (it != doc->m_roomPlayerList.end() && it->m_uid != doc->m_userInfo.guid) {
		++it;
	}
	if (it != doc->m_roomPlayerList.end()) {
		// JP record +0x53 & 0x18 (the KR m_attribute) or the admin bit.
		SetRoomControls((it->m_attribute & 0x18) || AdminMode());
	}
}

// 0x50d4c0
// The JP CLobbyMain::HandleMsg 7, a player entered the room (KR
// lobbymain.cpp:3293, same shape). JP differences: only the guild match
// (game type 6) rebuilds the teams (after the game the player joins the
// list of its team, 0x50bbe0); the slot's pet area is its room position
// (JP record +0x2e) and the pet is built by AddPlayer (local content 0x55:
// not for an invitation).
void FrGameRoomDlg::OnPlayerEntered(sSlotInfo* slot, intptr_t param2) {
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	int gameType = JpSharedDoc_GetRoomGameType();
	if (gameType != 6) {
		if (m_pUserList) {
			m_pUserList->AddItem(slot);
			m_pUserList->SortItem(JpRoomUserCompare);
		}
	} else if (!doc->m_gameOver) {
		RemakeTeam(true);
	} else {
		if ((slot->m_attribute & 3) == 0) {
			m_slotList1.push_back(slot);
		} else {
			m_slotList2.push_back(slot);
		}
		RemakeTeam(false);
	}
	unsigned int lines;
	if (gameType == 4 || gameType == 10) {
		lines = 24;
	} else {
		if (IsMassGame((unsigned char)gameType)) {
			return;
		}
		sRoomSlot roomSlot;
		roomSlot.m_pPetArea = (&m_pPet0)[JpSlotInfo_GetWaitingSlot(*slot) - 1];
		roomSlot.m_pPet = NULL;
		roomSlot.m_characterInfo = *(sCharacterInfo*)param2;
		roomSlot.m_angelFlags.flag0 = slot->m_flags57.flag0;
		roomSlot.m_angelFlags.flag1 = slot->m_flags57.flag1;
		doc->m_roomSlotMap[slot->m_uid] = roomSlot;
		if (IsLocalContent((localContentType_t)0x55)) {
			if (!slot->m_flags151.flag) {
				AddPlayer(slot->m_uid);
			}
		} else {
			AddPlayer(slot->m_uid);
		}
		lines = 4;
	}
	// Drop one empty line past the list's size.
	if (m_pUserList && (unsigned int)m_pUserList->GetCurrentItemSize(true) > lines) {
		FrListBox::ITEM_LIST::iterator it;
		for (it = m_pUserList->m_itemList.begin(); it != m_pUserList->m_itemList.end(); ++it) {
			if ((*it)->pData == NULL) {
				m_pUserList->_DelItem(it);
				break;
			}
		}
		m_pUserList->SortItem(JpRoomUserCompare);
	}
}

// 0x50d7d0
// The JP CLobbyMain::OnGameRoom_RoomUserOwnerDraw (KR lobbymain.cpp:13190,
// same shape and positions): one player card of the room. The row data is
// the port's KR sSlotInfo (the JP record is the KR one a byte lower: +0x2e
// room position, +0x2f identity, +0x33 title, +0x37 character, +0x47 room
// card skin, +0x4b icon skin, +0x53 attribute, +0x54 manner / ready, +0x55
// level, +0x56 angel, +0x58 guild UID, +0x154 bit 0 the invitation). JP
// differences: local content 0x55 (KR 0x54), the master and the picked
// player from the 0xa5 container (+0x14 host, +0xc picked), the pet
// display gated by JP CSharedDoc +0x11410 (the KR m_bongdariShop[2]), no
// ladder icon, and a GP host (identity 0x8000) shows LEVELS gp_host.
void FrGameRoomDlg::OnUserOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem) {
		return;
	}
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI) {
		return;
	}
	// An empty slot: "team_base02".
	if (!pItem->pData) {
		pGDI->DrawTexture(m_pTeamBitmap[3], WRect(pItem->pos.x, pItem->pos.y, (float)m_pTeamBitmap[3]->Width(), (float)m_pTeamBitmap[3]->Height()), 0xFFFFFFFF, 0);
		return;
	}
	if (IsLocalContent((localContentType_t)0x55)) {
		if (((sSlotInfo*)pItem->pData)->m_flags151.flag) {
			pGDI->DrawTexture(m_pInviteBitmap, WRect(pItem->pos.x, pItem->pos.y, (float)m_pInviteBitmap->Width(), (float)m_pInviteBitmap->Height()), 0xFFFFFFFF, 0);
		}
	}
	sSlotInfo* pSlot = (sSlotInfo*)pItem->pData;
	if (!pSlot) {
		return;
	}
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	// The slot's pet, built on its first draw in the pet area of the
	// player's room position (+0x190 + 4 * position: m_pPet0..m_pPet3).
	if (doc->m_roomSlotMap[pSlot->m_uid].m_pPet == NULL) {
		doc->m_roomSlotMap[pSlot->m_uid].m_pPetArea = (&m_pPet0)[JpSlotInfo_GetWaitingSlot(*pSlot) - 1];
		doc->m_roomSlotMap[pSlot->m_uid].m_pPet = NULL;
		doc->m_roomSlotMap[pSlot->m_uid].m_angelFlags.flag0 = pSlot->m_flags57.flag0;
		doc->m_roomSlotMap[pSlot->m_uid].m_angelFlags.flag1 = pSlot->m_flags57.flag1;
		if (IsLocalContent((localContentType_t)0x55)) {
			if (!pSlot->m_flags151.flag) {
				AddPlayer(pSlot->m_uid);
			}
		} else {
			AddPlayer(pSlot->m_uid);
		}
	}

	if (IsLocalContent((localContentType_t)0x55)) {
		WRect rect(pItem->pos.x, pItem->pos.y, (float)m_pTeamBitmap[2]->Width(), (float)m_pTeamBitmap[2]->Height());
		if (!pSlot->m_flags151.flag) {
			if (!Jp_GameState_11410()) {
				std::map<DWORD, sRoomSlot>::iterator it = doc->m_roomSlotMap.find(pSlot->m_uid);
				if (it != doc->m_roomSlotMap.end()) {
					if (it->second.m_pPet) {
						it->second.m_pPet->Display(0.0f, 0.0f);
					}
				}
			}
			// JP CSharedDoc +0x1094c: the KR m_roomInfo.m_gameType (1: team game).
			if (doc->m_roomInfo.m_gameType == 1) {
				pGDI->DrawTexture(m_pTeamBitmap[pSlot->m_attribute & 3], rect, 0xFFFFFFFF, 0);
			} else {
				pGDI->DrawTexture(m_pTeamBitmap[2], rect, 0xFFFFFFFF, 0);
			}
		} else {
			pGDI->DrawTexture(m_pInviteBitmap, rect, 0xFFFFFFFF, 0);
		}
		if (pSlot->m_roomCardSkinTypeId) {
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pSlot->m_roomCardSkinTypeId);
			if (pSkin) {
				const Bitmap* pCard = g_pFresh->GetBitmap(pSkin->m_cutinImage);
				pGDI->DrawTexture(pCard, rect, 0xFFFFFFFF, 0);
			}
		}
		// The kick mark: the room master's view of the other players.
		if (doc->m_userInfo.guid == m_pLobbyData->m_hostUserId && !(pSlot->m_attribute & 0x18)) {
			pGDI->DrawTexture(m_pKickBitmap, WRect(rect.x + rect.w - 8.0f - (float)m_pKickBitmap->Width(), rect.y + 8.0f, (float)m_pKickBitmap->Width(), (float)m_pKickBitmap->Height()), 0xFFFFFFFF, 0);
		}
		if (pSlot->m_uid == m_pLobbyData->m_selectedUserId) {
			pGDI->DrawTexture(m_pTeamSelectBitmap, rect, 0xFFFFFFFF, 0);
		}
		// The master / ready banner, centred on the card.
		int x = (int)pItem->pos.x + (m_pTeamBitmap[pSlot->m_attribute & 3]->Width() - m_pReadySignBitmap->Width()) / 2;
		if (pSlot->m_attribute & 0x18) {
			rect = WRect((float)x, pItem->pos.y + 6.0f, (float)m_pMasterSignBitmap->Width(), (float)m_pMasterSignBitmap->Height());
			pGDI->DrawTexture(m_pMasterSignBitmap, rect, 0xFFFFFFFF, 0);
		} else if (pSlot->m_flags55.flag1) {
			rect = WRect((float)x, pItem->pos.y + 6.0f, (float)m_pReadySignBitmap->Width(), (float)m_pReadySignBitmap->Height());
			pGDI->DrawTexture(m_pReadySignBitmap, rect, 0xFFFFFFFF, 0);
		}
		// The player's own character portrait (or its icon skin).
		const char* iconName = NULL;
		if (pSlot->m_iconSkinTypeId) {
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pSlot->m_iconSkinTypeId);
			if (pSkin) {
				iconName = pSkin->m_cutinImage;
			}
		} else {
			IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(pSlot->m_characterTypeId);
			if (pChar) {
				iconName = pChar->m_icon;
			}
		}
		if (iconName) {
			const Bitmap* pIcon = g_pFresh->GetBitmap(iconName);
			if (pIcon) {
				rect.x = pItem->pos.x + 8.0f;
				rect.y = pItem->pos.y + 36.0f;
				rect.w = (float)pIcon->Width();
				rect.h = (float)pIcon->Height();
				pGDI->DrawTexture(pIcon, rect, 0xFFFFFFFF, 0);
			}
		}
		if (pSlot->m_attribute & 4) {
			pGDI->DrawTexture(m_pDiveBitmap, WRect(pItem->pos.x + 6.0f, pItem->pos.y + 100.0f, (float)m_pDiveBitmap->Width(), (float)m_pDiveBitmap->Height()), 0xFFFFFFFF, 0);
		}
		const Bitmap* pGender;
		if (pSlot->m_flags57.flag0) {
			pGender = m_pGenderIcon[8 + pSlot->m_attributeBits.userFlag % 2];
		} else if (pSlot->m_flags55.flag0) {
			pGender = m_pGenderIcon[6 + pSlot->m_attributeBits.userFlag % 2];
		} else {
			pGender = m_pGenderIcon[pSlot->m_attributeBits.userFlag];
		}
		if (pGender) {
			pGDI->DrawTexture(pGender, WRect(pItem->pos.x + 2.0f, pItem->pos.y + 98.0f, (float)pGender->Width(), (float)pGender->Height()), 0xFFFFFFFF, 0);
		}
		// The level badge: the equipped title's icon, else the level's
		// (not on an invitation card); a GP host shows "gp_host".
		const Bitmap* pLevel = NULL;
		if (pSlot->m_titleTypeId) {
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pSlot->m_titleTypeId);
			if (pSkin) {
				pLevel = g_pFresh->GetBitmap(pSkin->m_icon);
			}
		} else if (!((sSlotInfo*)pItem->pData)->m_flags151.flag) {
			pLevel = g_pFresh->GetBitmap(MakeStr("level_%03d", pSlot->m_level + 1));
		}
		if (pSlot->m_identity & 0x8000) {
			pLevel = g_pFresh->GetManager()->GetBitmap("LEVELS", "gp_host");
		}
		if (pLevel) {
			pGDI->DrawTexture(pLevel, WRect(pItem->pos.x + 90.0f - float2int(pLevel->Width() * 0.5f), pItem->pos.y + 68.0f, (float)pLevel->Width(), (float)pLevel->Height()), 0xFFFFFFFF, 0);
		}
		if (pSlot->m_guildUID && pSlot->m_flags151.flag) {
			const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pSlot->m_emblemName);
			if (pEmblem) {
				pGDI->DrawTexture(pEmblem, WRect(pItem->pos.x + 60.0f, pItem->pos.y + 44.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
			}
		}
	} else {
		if (!Jp_GameState_11410()) {
			std::map<DWORD, sRoomSlot>::iterator it = doc->m_roomSlotMap.find(pSlot->m_uid);
			if (it != doc->m_roomSlotMap.end()) {
				if (it->second.m_pPet) {
					it->second.m_pPet->Display(0.0f, 0.0f);
				}
			}
		}
		WRect rect(pItem->pos.x, pItem->pos.y, (float)m_pTeamBitmap[2]->Width(), (float)m_pTeamBitmap[2]->Height());
		if (doc->m_roomInfo.m_gameType == 1) {
			pGDI->DrawTexture(m_pTeamBitmap[pSlot->m_attribute & 3], rect, 0xFFFFFFFF, 0);
		} else {
			pGDI->DrawTexture(m_pTeamBitmap[2], rect, 0xFFFFFFFF, 0);
		}
		if (pSlot->m_roomCardSkinTypeId) {
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pSlot->m_roomCardSkinTypeId);
			if (pSkin) {
				const Bitmap* pCard = g_pFresh->GetBitmap(pSkin->m_cutinImage);
				pGDI->DrawTexture(pCard, rect, 0xFFFFFFFF, 0);
			}
		}
		if (doc->m_userInfo.guid == m_pLobbyData->m_hostUserId && !(pSlot->m_attribute & 0x18)) {
			pGDI->DrawTexture(m_pKickBitmap, WRect(rect.x + rect.w - 8.0f - (float)m_pKickBitmap->Width(), rect.y + 8.0f, (float)m_pKickBitmap->Width(), (float)m_pKickBitmap->Height()), 0xFFFFFFFF, 0);
		}
		if (pSlot->m_uid == m_pLobbyData->m_selectedUserId) {
			pGDI->DrawTexture(m_pTeamSelectBitmap, rect, 0xFFFFFFFF, 0);
		}
		int x = (int)pItem->pos.x + (m_pTeamBitmap[pSlot->m_attribute & 3]->Width() - m_pReadySignBitmap->Width()) / 2;
		if (pSlot->m_attribute & 0x18) {
			rect = WRect((float)x, pItem->pos.y + 6.0f, (float)m_pMasterSignBitmap->Width(), (float)m_pMasterSignBitmap->Height());
			pGDI->DrawTexture(m_pMasterSignBitmap, rect, 0xFFFFFFFF, 0);
		} else if (pSlot->m_flags55.flag1) {
			rect = WRect((float)x, pItem->pos.y + 6.0f, (float)m_pReadySignBitmap->Width(), (float)m_pReadySignBitmap->Height());
			pGDI->DrawTexture(m_pReadySignBitmap, rect, 0xFFFFFFFF, 0);
		}
		const char* iconName = NULL;
		if (pSlot->m_iconSkinTypeId) {
			IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pSlot->m_iconSkinTypeId);
			if (pSkin) {
				iconName = pSkin->m_cutinImage;
			}
		} else {
			IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(pSlot->m_characterTypeId);
			if (pChar) {
				iconName = pChar->m_icon;
			}
		}
		if (iconName) {
			const Bitmap* pIcon = g_pFresh->GetBitmap(iconName);
			if (pIcon) {
				rect.x = pItem->pos.x + 8.0f;
				rect.y = pItem->pos.y + 36.0f;
				rect.w = (float)pIcon->Width();
				rect.h = (float)pIcon->Height();
				pGDI->DrawTexture(pIcon, rect, 0xFFFFFFFF, 0);
			}
		}
		if (pSlot->m_attribute & 4) {
			pGDI->DrawTexture(m_pDiveBitmap, WRect(pItem->pos.x + 6.0f, pItem->pos.y + 100.0f, (float)m_pDiveBitmap->Width(), (float)m_pDiveBitmap->Height()), 0xFFFFFFFF, 0);
		}
		const Bitmap* pGender;
		if (pSlot->m_flags57.flag0) {
			pGender = m_pGenderIcon[8 + pSlot->m_attributeBits.userFlag % 2];
		} else if (pSlot->m_flags55.flag0) {
			pGender = m_pGenderIcon[6 + pSlot->m_attributeBits.userFlag % 2];
		} else {
			pGender = m_pGenderIcon[pSlot->m_attributeBits.userFlag];
		}
		if (pGender) {
			pGDI->DrawTexture(pGender, WRect(pItem->pos.x + 2.0f, pItem->pos.y + 98.0f, (float)pGender->Width(), (float)pGender->Height()), 0xFFFFFFFF, 0);
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
		if (pLevel) {
			pGDI->DrawTexture(pLevel, WRect(pItem->pos.x + 60.0f, pItem->pos.y + 68.0f, (float)pLevel->Width(), (float)pLevel->Height()), 0xFFFFFFFF, 0);
		}
		if (pSlot->m_guildUID) {
			const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pSlot->m_emblemName);
			if (pEmblem) {
				pGDI->DrawTexture(pEmblem, WRect(pItem->pos.x + 60.0f, pItem->pos.y + 44.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
			}
		}
	}

	if (WSingleton<CProjectG>::Instance()->HidePrivacy()) {
		return;
	}
	// The nick: red for the own player, black for the others.
	if (pItem->underCursor) {
		pGDI->SetTextColor(0xFFFFFFFF, 0xFF808080);
		pGDI->SetTextStyle(2);
	} else {
		if (pSlot->m_uid == MyGuid(true)) {
			pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
		} else {
			pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		}
		pGDI->SetTextStyle(0);
	}
	g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 20.0f, pItem->pos.y + 107.0f), 0, pSlot->m_nickname, -1.0f, 0xFFFFFFFF);
}

// 0x50eb20
// The JP CLobbyMain::MakeRoomUserList (KR lobbymain.cpp:8513). JP
// differences: with more than one player in the room the ready tip blinks
// (the event rooms, mode 0x14 / 0x15, keep the blink window), else the
// start tip; the result list of types 4, 9, 10 and 11 has 24 rows (26 in
// the event rooms) and skips the players whose JP-only record byte +0x161
// is set; the unfinished check posts 0x2e (KR 0x2d); without a user list,
// rooms of mode 0xc, 0xe and 0x13 add the own room slot too; the own slot
// (JP 0x2dc bytes) gets angel bits 5 of the event flags (KR 3) and the
// player's uid (JP +0x2d4, CSharedDoc +0x8cb = MyUID()).
void FrGameRoomDlg::MakeRoomUserList(void) {
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if (doc->m_roomInfo.m_userCount > 1) {
		m_blinking = true;
		m_blinkTimer = -10.0f;
		int mode = JpSharedDoc_GetRoomGameMode();
		if (mode != 0x14 && mode != 0x15) {
			m_pBlinkWnd = m_pReadyTip;
		}
	} else {
		m_pBlinkWnd = m_pStartTip;
	}

	bool ownSlot = false;
	if (!m_pUserList) {
		int mode = JpSharedDoc_GetRoomGameMode();
		ownSlot = mode == 0xc || mode == 0xe || mode == 0x13;
	} else {
		std::list<sSlotInfo>::iterator it;
		switch (doc->m_roomInfo.m_gameType) {
		case 4:
		case 9:
		case 10:
		case 11: {
			m_pUserList->ClearItem();
			int rows = 24;
			int mode = JpSharedDoc_GetRoomGameMode();
			if (mode == 0x14 || mode == 0x15) {
				rows = 26;
			}
			if (doc->m_gameOver) {
				int unfinished = 0;
				if (doc->m_gameResultList.size() != 0) {
					for (it = doc->m_roomPlayerList.begin(); it != doc->m_roomPlayerList.end(); ++it) {
						sSlotInfo* player = &*it;
						if (JpSlotInfo_IsSkippedInResults(*player)) {
							continue;
						}
						unsigned char index = doc->GetIndex(player->m_uid);
						if (doc->m_gameResultList[index].m_state == 2) {
							m_pUserList->AddItem(player);
						} else if (doc->m_gameResultList[index].m_state == 0) {
							unfinished++;
						}
					}
				}
				for (int size = m_pUserList->GetCurrentItemSize(true); size < rows; size++) {
					m_pUserList->AddItem(NULL);
				}
				m_pUserList->SortItem(JpRoomFinishCompare);
				if (unfinished == 0) {
					CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0x2e, 0, 0, 0, 0);
				}
			} else {
				for (it = doc->m_roomPlayerList.begin(); it != doc->m_roomPlayerList.end(); ++it) {
					m_pUserList->AddItem(&*it);
				}
				for (int size = m_pUserList->GetCurrentItemSize(true); size < rows; size++) {
					m_pUserList->AddItem(NULL);
				}
				if (doc->m_roomPlayerList.size() > 1) {
					m_pUserList->SortItem(JpRoomUserCompare);
				}
			}
			break;
		}
		case 6:
			if (doc->m_gameOver) {
				m_slotList1.clear();
				m_slotList2.clear();
				for (it = doc->m_roomPlayerList.begin(); it != doc->m_roomPlayerList.end(); ++it) {
					sSlotInfo* player = &*it;
					unsigned char index = doc->GetIndex(player->m_uid);
					if (doc->m_gameResultList[index].m_state == 2) {
						if (player->m_guildUID == doc->m_roomInfo.m_guild.m_guildUID[0]) {
							m_slotList1.push_back(player);
						} else if (player->m_guildUID == doc->m_roomInfo.m_guild.m_guildUID[1]) {
							m_slotList2.push_back(player);
						}
					}
				}
				m_slotList1.sort(JpRoomFinishCompare);
				m_slotList2.sort(JpRoomFinishCompare);
				RemakeTeam(false);
			} else {
				RemakeTeam(true);
			}
			break;
		case 5:
			if (doc->m_gameOver) {
				m_slotList1.clear();
				m_slotList2.clear();
				int unfinished = 0;
				for (it = doc->m_roomPlayerList.begin(); it != doc->m_roomPlayerList.end(); ++it) {
					sSlotInfo* player = &*it;
					unsigned char index = doc->GetIndex(player->m_uid);
					if (doc->m_gameResultList[index].m_state == 2) {
						if (!(player->m_attribute & 3)) {
							m_slotList1.push_back(player);
						} else {
							m_slotList2.push_back(player);
						}
					} else if (doc->m_gameResultList[index].m_state == 0) {
						unfinished++;
					}
				}
				m_slotList1.sort(JpRoomFinishCompare);
				m_slotList2.sort(JpRoomFinishCompare);
				RemakeTeam(false);
				if (unfinished == 0) {
					CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0x2e, 0, 0, 0, 0);
				}
			} else {
				RemakeTeam(true);
			}
			break;
		default:
			m_pUserList->ClearItem();
			for (it = doc->m_roomPlayerList.begin(); it != doc->m_roomPlayerList.end(); ++it) {
				m_pUserList->AddItem(&*it);
			}
			for (unsigned char size = (unsigned char)m_pUserList->GetCurrentItemSize(true); size < 4; size++) {
				m_pUserList->AddItem(NULL);
			}
			if (doc->m_roomPlayerList.size() > 1) {
				m_pUserList->SortItem(JpRoomUserCompare);
			}
			break;
		}

		switch (doc->m_roomInfo.m_gameType) {
		case 4:
		case 5:
		case 6:
		case 9:
		case 10:
		case 11:
		case 18:
			ownSlot = true;
			break;
		}
	}

	if (ownSlot && !m_addOwnPlayer) {
		sRoomSlot slot;
		slot.m_pPetArea = m_pPet0;
		slot.m_pPet = NULL;
		slot.m_partTidList = doc->GetMyPartTidList();
		slot.m_characterInfo = doc->m_characterMap[doc->m_myCharacterId];
		slot.m_angelFlag = slot.m_angelFlag ^ ((slot.m_angelFlag ^ (doc->m_userInfo.eventFlags >> 5)) & 1);
		slot.m_angelFlag = slot.m_angelFlag ^ ((slot.m_angelFlag ^ (doc->m_userInfo.eventFlags >> 5)) & 2);
		JpRoomSlot_SetUid(slot, MyUID());
		doc->m_roomSlotMap[MyGuid(false)] = slot;
		AddPlayer(MyGuid(false));
	}
}

// 0x55cfa0
// The embedded objects at 0x3c, 0x50, 0x58 and 0x6c (their vtables and
// words) are not built: their classes are not identified.
JpGrandPrixRoomObject::JpGrandPrixRoomObject(void) {
	m_lobbyValue = NULL;
	m_value48 = 0;
	m_value4c = 0;
	m_value64 = 0;
	m_value68 = 0;
	m_fixedEquipment = false;
	m_value78 = 0;
	for (int i = 0; i < 14; i++) {
		m_headValues[i] = 0;
	}
}

// 0x55d020
JpGrandPrixRoomObject::~JpGrandPrixRoomObject(void) {
}

// 0x55d150
void JpGrandPrixRoomObject::SetLobbyValue(WTitleFontEx* font) {
	m_lobbyValue = font;
}

// 0x59f3c0
bool JpRoomUserCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}
	return JpSlotInfo_GetWaitingSlot(*(const sSlotInfo*)lhs) < JpSlotInfo_GetWaitingSlot(*(const sSlotInfo*)rhs);
}

// JP record byte +0x2e (inline in 0x59f3c0 and the GP waiting room).
unsigned char JpSlotInfo_GetWaitingSlot(const sSlotInfo& slot) {
	return slot.m_connection;
}

// 0x59fb40
// The KR CLobbyMain::_RefreshUccClothes (lobbymain.cpp:5272; a free
// function in JP, name ours): the player's room slot pet, when the slot's
// part of the clothes' type is that UCC item with that key, dresses its
// frame in it (0x8b0960; no local content 0x5f test, one call).
void JpRoom_RefreshUccClothes(sSlotInfo* slot, unsigned long typeId, const char* uccKey) {
	if (!slot) {
		return;
	}
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	std::map<DWORD, sRoomSlot>::iterator it = doc->m_roomSlotMap.find(slot->m_uid);
	if (it == doc->m_roomSlotMap.end()) {
		return;
	}
	sRoomSlot* pSlot = &it->second;
	if (!pSlot || !pSlot->m_pPet) {
		return;
	}
	unsigned char index = (unsigned char)((typeId >> 13) & 0x1F);
	if (pSlot->m_partTidList.m_partTids[index] != typeId) {
		return;
	}
	sUccClothes* clothes = UccManager()->FindClothes(pSlot->m_partTidList.m_partIds[index]);
	if (clothes && strcmpi(clothes->m_uccKey, uccKey) == 0) {
		Jp_PetFrame_SetUccClothes(pSlot->m_pPet->GetPetFrame(), clothes);
	}
}

// 0x59fbd0 (the JP FinishUserCompare of KR CLobbyMain, same shape; name
// ours): rows without a player last, by the result list's finish order.
bool JpRoomFinishCompare(const void* lhs, const void* rhs) {
	if (!lhs) {
		return false;
	}
	if (!rhs) {
		return true;
	}
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	return doc->m_gameResultList[doc->GetIndex(((const sSlotInfo*)lhs)->m_uid)].m_finishOrder
		< doc->m_gameResultList[doc->GetIndex(((const sSlotInfo*)rhs)->m_uid)].m_finishOrder;
}

// The JP record byte +0x161 MakeRoomUserList 0x50eb20 skips finished
// players by (the JP sSlotInfo is longer than the KR 0x152; role not
// traced). Port placeholder: the KR record has no such byte.
bool JpSlotInfo_IsSkippedInResults(const sSlotInfo& slot) {
	(void)slot;
	return false;
}

// The JP record byte +0x159 OnUserLBtnDown 0x505800 tests in the event
// rooms (mode 0x14 / 0x15; role not traced). Port placeholder: the KR
// record (0x152 bytes) has no such byte.
bool JpSlotInfo_IsEventExcluded(const sSlotInfo& slot) {
	(void)slot;
	return false;
}

// 0x6f4b80
JpGameRoomOwned::JpGameRoomOwned(void) {
	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));
	info.filename = &s_singleFontNumFile;
	info.numPages = 1;
	info.fontw = 14;
	info.fonth = 16;
	info.texw = 154;
	info.texh = 16;
	info.pCharSet = s_singleFontNumCharSet;
	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}
