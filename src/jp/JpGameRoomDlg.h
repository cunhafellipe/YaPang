// JP-only game code (no KR counterpart): FrGameRoomDlg, the base of the JP
// room dialogs (the KR client draws its room inside CLobbyMain). LobbyMain
// 0xab6be0 opens one subclass per room type, layout "gameroom_main" (stroke
// FrStrokeTypeRoomDlg, mass FrMassTypeRoomDlg, GP FrGPTypeRoomDlg) or
// "gameroom_single" (FrSingleTypeDlg), in JpRoomTypeDlgs.h.
// The base holds the controls every room layout shares: title, map and its
// prev/next/combo, start/ready/change_team/modify, the five stat gauges and
// their values, the current and selectable character / caddie / club /
// aztec / mascot viewers, the artifact box, the assist button and the group
// boxes of the game modes.
//
// JP x86 facts: RTTI m_RTTI 0xe74db0 "FrGameRoomDlg" (static init 0xccb900),
// base FrChatDlg (RTTI 0xe73e98, ctor 0x48f0b0, size 0x140); size 0x430,
// constructor 0x5083c0, destructor body 0x5088c0; make instance 0x50ac10,
// registered as "FrGameRoomDlg" by 0xccb920 (and 0x50ac80). Message map
// 0xe0870c (base FrChatDlg's 0xe04210; 68 rows at 0xe08718, handlers stored
// by the static init 0xccb940).
//
// Vtables (38 + 2 + 1 slots, FrChatDlg's otherwise):
//   0xd0642c [0] 0x508890 GetRTTI, [1] 0x50aca0 deleting destructor,
//            [9] 0x50acd0 OnProc (2204 bytes), [32] 0x50d0e0 OnInit
//   0xd0641c (FrCmdTarget, this-4) [0] 0x5088a0 deleting-destructor thunk,
//            [1] 0x4ff1c0 GetMessageMap
//   0xd06414 (this-8) [0] 0x5088b0 deleting-destructor thunk
// The members are protected: the room types (JpRoomTypeDlgs.h) store the
// controls of their own layout rows into them (each = DYNAMIC_CAST in a
// subclass Init handler) and call the base's room setup / release.
// All four subclasses override exactly these slots ([0], [1], [9], [32] and
// the three of the other vtables; FrGPTypeRoomDlg keeps FrMassTypeRoomDlg's
// [9] 0x511c90) and chain their message maps to 0xe0870c (0xe09464,
// 0xe0a99c, 0xe0acc0).
//
// JP x86 offsets in the comments (the port keeps the KR FrForm layout
// under FrChatDlg).
#ifndef PG_JP_JPGAMEROOMDLG_H
#define PG_JP_JPGAMEROOMDLG_H

#include <stdint.h>

#include <list>
#include <string>

#include "defines.h"
#include "padding.h"
#include "frcmdtarget.h"
#include "frform.h"
#include "JpChatDlg.h"
#include "JpContentsContainers.h"
#include "TitleFontEx.h"

class Bitmap;
class FrArea;
class FrButton;
class FrComboCtlEx;
class FrEdit;
class FrGaugeBar;
class FrGaugeBarImage;
class FrGroupBox;
class FrListBox;
class FrStatic;
class FrTreasureGift;
class FrUserListDlg;
class FrViewer;
class FrWnd;
class FrForm;
class FrScoreDlg;
class FrUniteResultDlg;
class WTitleFont;
struct sSlotInfo;

// JP-only, 0x54 bytes, built by the constructor from the room's course
// (JP CSharedDoc +0x10950): constructor 0x53f0d0, destructor 0x53f010.
// Not decompiled (name ours).
class JpGameRoomData {
public:
	explicit JpGameRoomData(unsigned char course);                    // 0x53f0d0
	~JpGameRoomData(void);                                            // 0x53f010
	// Per frame, from FrGameRoomDlg::OnProc (by reference).
	void Update(const float& dt);                                     // 0x53ef40
private:
	UNKNOWN(0x0, 0x54);
};

// JP-only WTitleFontEx (0xc bytes, vtable 0xd1c10c; base constructor
// 0x6f45b0) at FrGameRoomDlg +0x15c: the "[single_font_num.jpg" digits
// (14x16 glyphs on a 154x16 page, "1234567890!"), printed by 0x522cd0.
// Deleted by the destructor through its vtable slot 0 (deleting
// destructor); FrSingleTypeDlg's InitSingleRoom creates it when null
// (name ours).
class JpGameRoomOwned : public WTitleFontEx {
public:
	JpGameRoomOwned(void);                                            // 0x6f4b80
};
CHECK_SIZE(JpGameRoomOwned, 0xc);

// JP-only, 0x7c bytes: the GP room object FrGPTypeRoomDlg::LoadGrandPrixRoomUi
// 0x512f80 creates at FrGameRoomDlg +0x150 when null (name ours). The
// constructor 0x55cfa0 zeroes +0x0, +0x4-+0x38 (14 dwords), +0x48, +0x4c,
// +0x64, +0x68, +0x74 (byte) and +0x78, and builds four embedded objects
// with their own vtables (+0x3c and +0x58: vtable 0xd09f50 and two words
// at +0x4/+0x8 of each; +0x50 and +0x6c: vtable 0xd0a16c and a word at
// +0x4), classes not identified: the constructor zeroes the plain fields
// only; the setup 0x55dd40 stays declared only.
class JpGrandPrixRoomObject {
public:
	JpGrandPrixRoomObject(void);                                      // 0x55cfa0
	~JpGrandPrixRoomObject(void);                                     // 0x55d020
	// Stores FrGameRoomDlg +0x154 (the lobby's +0x784 font,
	// FrGPTypeRoomDlg::OnInit 0x516fc0) into +0x0 (name ours).
	void SetLobbyValue(WTitleFontEx* font);                           // 0x55d150
	// FrGPTypeRoomDlg::LoadGrandPrixRoomUi 0x512f80's setup (name ours,
	// declared only).
	void Setup(void);                                                 // 0x55dd40
	// Members with two arguments (ret 8; roles not traced, names ours,
	// declared only).
	void Update55d030(int a, int b);                                  // 0x55d030
	void Update55d480(int a, int b);                                  // 0x55d480
	void Update55d660(int a, int b);                                  // 0x55d660
	// The dwords +0x48 and +0x64 (full dwords, not bytes; names ours).
	int GetValue48(void) const { return m_value48; }                  // 0x55d160
	int GetValue64(void) const { return m_value64; }                  // 0x55d170
	// FrMassTypeRoomDlg 0x511d90 tests it == 1 after the course images and
	// the start / ready buttons are set: then it sends the main actor 0x4f
	// and disables the equipment viewers (EnableEquipViewers(false)): the GP
	// fixes the equipment (name ours; writer not traced).
	bool HasFixedEquipment(void) const { return m_fixedEquipment; }
private:
	WTitleFontEx* m_lobbyValue;          // 0x0 (the lobby's +0x784 font)
	// 0x4-0x38: fourteen dwords the constructor zeroes (roles not traced).
	uint32_t m_headValues[14];           // 0x4
	// 0x3c / 0x58: embedded objects (vtable 0xd09f50, words +0x4 / +0x8);
	// 0x50 / 0x6c: embedded objects (vtable 0xd0a16c, word +0x4). Their
	// classes are not identified: the port does not build them yet.
	UNKNOWN(0x3c, 0x48);
	int m_value48;                       // 0x48 (0x55d160)
	int m_value4c;                       // 0x4c
	UNKNOWN(0x50, 0x64);
	int m_value64;                       // 0x64 (0x55d170)
	int m_value68;                       // 0x68
	UNKNOWN(0x6c, 0x74);
	bool m_fixedEquipment;               // 0x74
	UNKNOWN(0x75, 0x78);
	int m_value78;                       // 0x78
	LAYOUT_CHECK_FRIEND;
};

class FrGameRoomDlg : public FrChatDlg {
	LAYOUT_CHECK_FRIEND;
public:
	static const WRTTI m_RTTI;                                        // 0xe74db0
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x508890

	FrGameRoomDlg(void);                                              // 0x5083c0
	virtual ~FrGameRoomDlg(void);                                     // [1] 0x50aca0 (deleting), 0x5088c0

	// Per frame: FrChatDlg's, CONTROL + REPLAY starts, the blink, ESCAPE,
	// the treasure approach gift, the idle check, the hover tooltips.
	virtual void OnProc(const float dt);                              // [9] 0x50acd0
	virtual bool OnInit(void);                                        // [32] 0x50d0e0

	// The room state (JP LobbyMain 0xab6be0 sets it after creating the
	// room form; 8: close_wnd and ESCAPE go through RequestCloseInState8).
	void SetRoomState(int state);                                     // 0x4ff200
	// Called by JP LobbyMain 0xab6be0 (0xab6d60) before SetRoomState with
	// its fonts +0x784 (CGPTimeCountFont) and +0x794 (CLobbyWindFont,
	// JpLobbyTitleFonts.h) (name ours).
	void SetLobbyValues(WTitleFontEx* font154, WTitleFontEx* titleFont); // 0x4ff300

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe08718

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe0870c
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0x4ff1c0

	// Message-map handlers, in row order.
	void OnTitleInit(PG_MSGARG param);                                // 0x5097b0
	void OnFrameBgInit(PG_MSGARG param);                              // 0x500880
	void OnBackgroundInit(PG_MSGARG param);                           // 0x500900
	void OnCloseWndLBtnUp(void);                                      // 0x509880
	void OnMapBackgroundInit(PG_MSGARG param);                        // 0x500bc0
	void OnMapInit(PG_MSGARG param);                                  // 0x503970
	void OnMapLBtnUp(void);                                           // 0x505a70
	void OnMapPrevInit(PG_MSGARG param);                              // 0x500c40
	void OnMapNextInit(PG_MSGARG param);                              // 0x500cf0
	void OnMapPrevLBtnUp(void);                                       // 0x503a50
	void OnMapNextLBtnUp(void);                                       // 0x503b50
	void OnMapComboInit(PG_MSGARG param);                             // 0x503c50
	void OnMapComboLBtnDown(void);                                    // 0x503cf0
	void OnStartTipInit(PG_MSGARG param);                             // 0x500da0
	void OnReadyTipInit(PG_MSGARG param);                             // 0x500e20
	void OnStartInit(PG_MSGARG param);                                // 0x500ea0
	void OnStartLBtnUp(void);                                         // 0x509960
	void OnReadyInit(PG_MSGARG param);                                // 0x500f10
	void OnReadyLBtnUp(void);                                         // 0x505bc0
	void OnChangeTeamInit(PG_MSGARG param);                           // 0x500fa0
	void OnChangeTeamLBtnUp(void);                                    // 0x5076c0
	void OnModifyInit(PG_MSGARG param);                               // 0x504000
	void OnModifyLBtnUp(void);                                        // 0x5040a0
	void OnPowerBarInit(PG_MSGARG param);                             // 0x501030
	void OnControlBarInit(PG_MSGARG param);                           // 0x5010b0
	void OnAccuracyBarInit(PG_MSGARG param);                          // 0x501130
	void OnSpinBarInit(PG_MSGARG param);                              // 0x5011b0
	void OnCurveBarInit(PG_MSGARG param);                             // 0x501230
	void OnEPowerInit(PG_MSGARG param);                               // 0x5012b0
	void OnEControlInit(PG_MSGARG param);                             // 0x501320
	void OnEAccuracyInit(PG_MSGARG param);                            // 0x501390
	void OnESpinInit(PG_MSGARG param);                                // 0x501400
	void OnECurveInit(PG_MSGARG param);                               // 0x501470
	void OnCurCaddieInit(PG_MSGARG param);                            // 0x5014e0
	void OnCurCaddieLBtnDown(void);                                   // 0x5041c0
	void OnCurCaddieOwnerDraw(PG_MSGARG param);                       // 0x509f90
	void OnCurClubInit(PG_MSGARG param);                              // 0x501550
	void OnCurClubLBtnDown(void);                                     // 0x504260
	void OnCurClubOwnerDraw(PG_MSGARG param);                         // 0x50a2a0
	void OnCurAztecInit(PG_MSGARG param);                             // 0x5015c0
	void OnCurAztecLBtnDown(void);                                    // 0x504300
	void OnCurAztecOwnerDraw(PG_MSGARG param);                        // 0x507780
	void OnCurMascotInit(PG_MSGARG param);                            // 0x501630
	void OnCurMascotLBtnDown(void);                                   // 0x5043a0
	void OnCurMascotOwnerDraw(PG_MSGARG param);                       // 0x50a430
	void OnCurEquipItemInit(PG_MSGARG param);                         // 0x5016a0
	void OnCurEquipItemLBtnDown(void);                                // 0x504440
	void OnCurEquipItemOwnerDraw(PG_MSGARG param);                    // 0x501710
	void OnCurCharSelInit(PG_MSGARG param);                           // 0x501820
	void OnCurCaddieSelInit(PG_MSGARG param);                         // 0x5018a0
	void OnCurClubSelInit(PG_MSGARG param);                           // 0x501920
	void OnCurAztecSelInit(PG_MSGARG param);                          // 0x5019a0
	void OnCurMascotSelInit(PG_MSGARG param);                         // 0x501a20
	// The five "cur_*_sel" LBUTTONDOWN rows (0x5044b0, 0x5044c0, 0x5044d0,
	// 0x5044e0, 0x5044f0) each jump to 0x503580.
	void OnSelLBtnDown(void);                                         // 0x503580
	void OnAtftInit(PG_MSGARG param);                                 // 0x501aa0
	void OnArtifactIconInit(PG_MSGARG param);                         // 0x501b20
	void OnAssistInit(PG_MSGARG param);                               // 0x504500
	void OnAssistLBtnUp(void);                                        // 0x5045b0
	void OnAssistTipInit(PG_MSGARG param);                            // 0x50cc20
	void OnAssistTitleInit(PG_MSGARG param);                          // 0x504670
	void OnShortGameInit(PG_MSGARG param);                            // 0x501b90
	void OnGroupStrokeInit(PG_MSGARG param);                          // 0x501c90
	void OnGroup30sInit(PG_MSGARG param);                             // 0x501c10
	void OnGroupResultInit(PG_MSGARG param);                          // 0x501d10
	void OnGroupGPInit(PG_MSGARG param);                              // 0x501d90

	// close_wnd, and ESCAPE in OnProc, while m_roomState is 8:
	// CanLeaveFinishedGame, then SendLeaveFinishedGame or OpenLeaveGameNotice.
	void RequestCloseInState8(void);                                  // 0x508ea0
	// Whether leaving the finished game in state 8 goes straight to the
	// main actor (0x8b): not a gallery / GM player (+0x812 & 0xe), the item
	// 0x1a000041 owned, not a guild game, at most 30 players, no result
	// report open, not a rookie channel, level above 5, fewer than ten
	// 0x1a000042 items (name ours).
	bool CanLeaveFinishedGame(int state);                             // 0x506f30
	// The main actor message 0x8b (0xc) (name ours).
	void SendLeaveFinishedGame(void);                                 // 0x5037b0
	// Opens the "notify_tiki_sub" form (+0x170) once: the game is still on,
	// leaving now forfeits it (name ours).
	void OpenLeaveGameNotice(void);                                   // 0x5055c0
	// Its result callback (style 0x801; name ours).
	bool OnLeaveGameNoticeResult(int ret, FrForm* pForm);             // 0x505410

	// OnInit's first step: adds each room player (JP CSharedDoc +0x109fc,
	// the KR m_roomPlayerList) to the list box at +0x190 and AddPlayer,
	// sends the current task's main actor 0x11 with the course, then
	// OpenNewRecordForm (name ours).
	void FillPlayerList(void);                                        // 0x50c000
	// The JP counterpart of the KR CLobbyMain::OpenNewRecordForm
	// (jp/pairing.tsv, by its strings), a FrGameRoomDlg member in JP (114
	// bytes); not decompiled yet.
	void OpenNewRecordForm(void);                                     // 0x5033b0
	// The JP CLobbyMain::ShowPet (KR lobbymain.cpp:7722, by its shape):
	// builds the room slot's (JP CSharedDoc map +0x3890, the KR
	// m_roomSlotMap) pet exhibition in the pet area of its room position
	// (name ours: called for each player added to the room).
	void AddPlayer(uint32_t key);                                     // 0x50b850
	// The JP CLobbyMain::HidePet (KR lobbymain.cpp:7817): deletes the room
	// slot's pet exhibition.
	void HidePet(uint32_t uid);                                       // 0x508f10

	// Result callbacks (KR CLobbyMain::OnNewRecordDlgResult and
	// OnApproachGiftDlgResult; names from the KR counterparts).
	bool OnNewRecordDlgResult(int ret, FrForm* pForm);                // 0x4ff260
	bool OnApproachGiftDlgResult(int ret, FrForm* pForm);             // 0x505380
	// The score dialog's result callback: clears m_pScoreDlg (name ours).
	bool OnScoreDlgResult(int ret, FrForm* pForm);                    // 0x4ff250
	// The "userlist" form's (OnInviteLBtnUp) result: clears +0x16c.
	bool OnUserListDlgResult(int ret, FrForm* pForm);                 // 0x4ff280
	// Mode 7 fixes the hole cup position (JP CSharedDoc byte +0x11609 = 1,
	// m_pFixHolecupPos "single_font_on"), any other mode clears it
	// ("single_font_off") (name ours).
	void SetHolecupPosFixedMode(int mode);                            // 0x503430
	// The course images of the room ("big_%s", "background%s.tga",
	// "map_background.tga"), the over gauge and the room controls (the JP
	// CLobbyMain::HandleMsg 0x11; name ours).
	void SetCourseImages(unsigned char course);                       // 0x50d120
	// The JP CLobbyMain::SetRoomControls (jp/pairing.tsv, by its strings;
	// 1706 bytes): enables the room master's controls; declared only.
	void SetRoomControls(bool master);                                // 0x50c0b0
	// The JP CLobbyMain::DrawFrameSelectedUserInfo (jp/pairing.tsv; 284
	// bytes; the frame around an enabled "cur_*" viewer); declared only.
	void DrawFrameSelectedUserInfo(WRect rect);                       // 0x500760

	// OnProc's last per-frame step: the KR CLobbyMain::RefreshAllUccClothes
	// (downloaded UCC clothes onto the room players' pets, 0x59fb40; name
	// ours).
	void ProcRoomExtras(void);                                        // 0x506d80

	// Room setup, run by the stroke and single room constructors (name
	// ours): the ready flags, the picked user, the input devices' idle
	// time, the social shuffle notice (room mode 0x12), the tooltips.
	void InitRoom(void);                                              // 0x503290
	// Refills the hole mode combo ("hole_front", then "hole_back" and
	// "hole_random" below 18 holes, then "hole_shuffle") and selects
	// selected (-1: none; 18 holes fold every mode past 0 to 1) (name ours).
	void FillHoleModeCombo(int selected);                             // 0x5034e0
	// Enables or disables the five "cur_*" viewers, then closes the
	// selection viewers (OnSelLBtnDown) (name ours).
	void EnableEquipViewers(bool enable);                             // 0x503660
	// Room release, run by the single room's ReleaseSingleRoom: deletes
	// the JpGameRoomData at +0x140 and zeroes it, then posts "Lobby" 0x59
	// (9 or 1) when the lobby desktop's state (0xe78c38 +0x1fc) is 10 or
	// 11 (name ours).
	void ReleaseRoom(void);                                           // 0x503340
	// Sets the title edit (+0x288) from text, or, when text is null, from
	// the room data (JP CSharedDoc +0x1094c mode, "[%d]   %s",
	// "%02d:%02d" time limits ...), then the room artifact box (the JP
	// CLobbyMain::HandleMsg 0x10 by the pairing; name ours).
	void UpdateTitle(const char* text);                               // 0x508a50
	// The five stat gauges and their "%d/%d" values (+0x200-+0x224): the
	// card manager's mastery stats recomputed (0x80aac0), then per stat
	// the JpMasteryStatInfo value and value + free points (the JP
	// counterpart of KR CLobbyMain::SetLevelBar, by its shape).
	void SetLevelBar(void);                                           // 0x50b570
	// Fills the user list (+0x190) from the room players, or the two team
	// lists 0x2f0 / 0x2fc (30s / guild rooms); without a user list, rooms
	// of mode 0xc, 0xe and 0x13 add the own player once (m_addOwnPlayer
	// clear) through AddPlayer. JP CLobbyMain::MakeRoomUserList by the
	// pairing (jp/pairing.tsv); 1588 bytes of JP game state (game type
	// +0x1094c and room mode +0x109b8 jump tables, the result list
	// +0x1042c, the guild uids +0x1095f / +0x10963).
	void MakeRoomUserList(void);                                      // 0x50eb20
	// The JP CLobbyMain::RemakeTeam (jp/pairing.tsv, in-unit LobbyMain):
	// lays the two team lists out in the user list; with rebuild, splits
	// the room players first (declared only).
	void RemakeTeam(bool rebuild);                                    // 0x509120

	// Row handlers the room types' maps use ("<group>.user", "overgauge",
	// "invite"); the base map does not list them. JP game state, not
	// decompiled yet.
	void OnUserInit(PG_MSGARG param);                                 // 0x500980
	void OnUserLBtnDown(void);                                        // 0x505800
	// OnUserLBtnDown's kick: packet 0x26 with the picked player (the KR
	// CLobbyMain::Banish by its shape).
	void Banish(void);                                                // 0x4ff320
	void OnUserRBtnUp(void);                                          // 0x5098a0
	void OnUserDblClick(void);                                        // 0x5038d0
	void OnUserOwnerDraw(PG_MSGARG param);                            // 0x50d7d0 (3885 bytes)
	void OnOverGaugeInit(PG_MSGARG param);                            // 0x503e30
	// Opens the "userlist" form (+0x16c) once; FrMassTypeRoomDlg hides it
	// with its own OnInviteLBtnUp 0x516390.
	void OnInviteLBtnUp(void);                                        // 0x503f30

	// 0x110-0x140: FrChatDlg's members.
	// 0x140: a 0x54-byte object built from the room data (0x53f0d0);
	// 0x148: the course object the map click selects; the constructor
	// zeroes most of 0x144-0x174.
	JpGameRoomData* m_pRoomData;         // 0x140 (deleted by ReleaseRoom)
	// "new_record" (OpenNewRecordForm; KR CLobbyMain::m_newRecordDlg),
	// cleared by OnNewRecordDlgResult; OnProc closes it when the approach
	// gift ends.
	FrForm* m_pNewRecordDlg;             // 0x144
	UNKNOWN(0x148, 0x14c);
	// Contents type 0xa5 (the constructor's CContentsDoc::GetContainer;
	// left unset when missing): the picked room user.
	CLobbyContainer* m_pLobbyData;       // 0x14c
	JpGrandPrixRoomObject* m_pGrandPrixRoom; // 0x150 (GP room: LoadGrandPrixRoomUi 0x512f80)
	// SetLobbyValues (zeroed by the constructor): the lobby's +0x784 font
	// (CGPTimeCountFont), which FrGPTypeRoomDlg::OnInit 0x516fc0 stores into
	// the GP object's +0x0 (0x55d150), and its title font +0x794.
	WTitleFontEx* m_lobbyValue;          // 0x154
	WTitleFontEx* m_pLobbyTitleFont;     // 0x158
	JpGameRoomOwned* m_pRoomOwned;       // 0x15c
	// "score_guild" / "score" (CreateForm 0x511a90: new 0x1d0, constructor
	// 0xaf1480, RTTI 0xf03318 "FrScoreDlg", the KR CLobbyMain::m_scoreDlg);
	// its members 0xaf2920, 0xaf2be0, 0xaf0530 fill it. Zeroed by the
	// constructor.
	FrScoreDlg* m_pScoreDlg;             // 0x160
	// The game result report OnProc's approach gift end closes (KR
	// CLobbyMain::m_resultReportDlg: ESCAPE ends the gift unless the report
	// is done (+0x26c) with a child dialog visible (0xae7a80)).
	FrUniteResultDlg* m_pResultReportDlg; // 0x164
	// "trgift" (CreateForm<FrTreasureGift> 0x502460), the treasure approach
	// gift OnProc opens (KR CLobbyMain::m_approachGiftDlg); cleared by
	// OnApproachGiftDlgResult 0x505380.
	FrTreasureGift* m_pApproachGiftDlg;  // 0x168
	// "userlist" (OnInviteLBtnUp 0x503f30: the KR FrUserListDlg, the KR
	// CLobbyMain::m_userListDlg), cleared by OnUserListDlgResult 0x4ff280.
	FrUserListDlg* m_pUserListDlg;       // 0x16c
	// "notify_tiki_sub" (OpenLeaveGameNotice 0x5055c0).
	FrForm* m_pLeaveGameNotice;          // 0x170
	// The "notify" form FrMassTypeRoomDlg::OnInviteLBtnUp 0x516390 creates.
	FrForm* m_pNotifyForm;               // 0x174
	int m_roomState;                     // 0x178 (8: close_wnd and ESCAPE go through RequestCloseInState8)
	// 0x17c-0x181: InitRoom clears the dword 0x17c and the bytes 0x180,
	// 0x181; 0x17f and 0x180 are not read here yet.
	// Set when no input came for 60 s in the room states 3, 4 and 6
	// (OnProc then sends 0x32 with it, and 0x0a for the room master);
	// cleared by the next input. InitRoom clears it.
	bool m_idleWarned;                   // 0x17c
	// The own ready state (OnReadyLBtnUp sends packet 0xd with 1 when set,
	// 0 when clear) and the lock that ignores the button; InitRoom sets both for a
	// gallery player (JP CSharedDoc +0x812 == 2, the KR m_userInfo.identity).
	bool m_ready;                        // 0x17d
	bool m_readyLocked;                  // 0x17e
	UNKNOWN(0x17f, 0x181);
	// Set by OnReadyLBtnUp once it has sent its request (the button is
	// ignored while set); InitRoom clears it.
	bool m_readyRequested;               // 0x181
	UNKNOWN(0x182, 0x184);
	// OnProc's blink of m_pBlinkWnd while m_blinking: the timer adds dt,
	// each period toggles the window, three toggles end it
	// (OnReadyLBtnUp clears m_blinking when m_ready is clear). The constructor zeroes
	// all three.
	float m_blinkTimer;                  // 0x184
	int m_blinkCount;                    // 0x188
	bool m_blinking;                     // 0x18c
	// Set by the constructor; while clear, MakeRoomUserList adds the own
	// player (rooms without a user list, modes 0xc, 0xe, 0x13).
	bool m_addOwnPlayer;                 // 0x18d
	UNKNOWN(0x18e, 0x190);
	FrListBox* m_pUserList;              // 0x190 "<group>.user" (OnUserInit)
	FrArea* m_pPet0;                     // 0x194 "group_stroke.pet0" (stroke room)
	FrArea* m_pPet1;                     // 0x198 "group_stroke.pet1"
	FrArea* m_pPet2;                     // 0x19c "group_stroke.pet2"
	FrArea* m_pPet3;                     // 0x1a0 "group_stroke.pet3"
	FrViewer* m_pFrameBg;                // 0x1a4 "frame_bg"
	FrViewer* m_pBackground;             // 0x1a8 "background"
	FrViewer* m_pMapBackground;          // 0x1ac "map_background"
	FrButton* m_pMap;                    // 0x1b0 "map"
	UNKNOWN(0x1b4, 0x1bc);
	FrButton* m_pMapPrev;                // 0x1bc "mapprev"
	FrButton* m_pMapNext;                // 0x1c0 "mapnext"
	FrComboCtlEx* m_pMapCombo;           // 0x1c4 "mapcomboex"
	// "group_stroke.overgauge": RTTI 0xf1ab9c = FrGaugeBarImage (the
	// AddControl<FrGaugeBarImage> 0x4fe4f0 tests it; OnOverGaugeInit calls
	// FrGaugeBar::SetRange 0xc02c20, SetDestPos 0xc02d50, SetPos [25]).
	FrGaugeBarImage* m_pOverGauge;       // 0x1c8
	FrViewer* m_pCurCaddie;              // 0x1cc "cur_caddie"
	FrViewer* m_pCurAztec;               // 0x1d0 "cur_aztec"
	FrViewer* m_pCurClub;                // 0x1d4 "cur_club"
	FrViewer* m_pCurMascot;              // 0x1d8 "cur_mascot"
	FrViewer* m_pCurEquipItem;           // 0x1dc "cur_equip_item"
	FrViewer* m_pCurCharSel;             // 0x1e0 "cur_char_sel"
	FrViewer* m_pCurCaddieSel;           // 0x1e4 "cur_caddie_sel"
	FrViewer* m_pCurAztecSel;            // 0x1e8 "cur_aztec_sel"
	FrViewer* m_pCurClubSel;             // 0x1ec "cur_club_sel"
	FrViewer* m_pCurMascotSel;           // 0x1f0 "cur_mascot_sel"
	FrButton* m_pStart;                  // 0x1f4 "start"
	FrButton* m_pReady;                  // 0x1f8 "ready"
	UNKNOWN(0x1fc, 0x200);
	FrGaugeBar* m_pPowerBar;             // 0x200 "powerbar"
	FrGaugeBar* m_pControlBar;           // 0x204 "controlbar"
	FrGaugeBar* m_pAccuracyBar;          // 0x208 "accuracybar"
	FrGaugeBar* m_pSpinBar;              // 0x20c "spinbar"
	FrGaugeBar* m_pCurveBar;             // 0x210 "curvebar"
	FrStatic* m_pEPower;                 // 0x214 "epower"
	FrStatic* m_pEControl;               // 0x218 "econtrol"
	FrStatic* m_pEAccuracy;              // 0x21c "eaccuracy"
	FrStatic* m_pESpin;                  // 0x220 "espin"
	FrStatic* m_pECurve;                 // 0x224 "ecurve"
	FrButton* m_pInvite;                 // 0x228 "invite" (mass room)
	FrButton* m_pChangeTeam;             // 0x22c "change_team"
	UNKNOWN(0x230, 0x234);
	FrButton* m_pResultScore;            // 0x234 "group_result.score" (mass room)
	FrArea* m_p30sTeamNum;               // 0x238 "group_30s.30_team_num" (mass room)
	FrArea* m_p30sGuildNum;              // 0x23c "group_30s.30_guild_num" (mass room)
	UNKNOWN(0x240, 0x248);
	// "group_result.rankframe" (mass room): RTTI 0xf1aba4, class not identified yet.
	FrWnd* m_pResultRankFrame;           // 0x248
	UNKNOWN(0x24c, 0x25c);
	FrArea* m_p30sTeam1;                 // 0x25c "group_30s.30_team1" (mass room)
	FrArea* m_p30sTeam2;                 // 0x260 "group_30s.30_team2"
	FrArea* m_p30sGuild1;                // 0x264 "group_30s.30_guild1"
	FrArea* m_p30sGuild2;                // 0x268 "group_30s.30_guild2"
	FrGaugeBar* m_pResultScoreGb;        // 0x26c "group_result.score_gb" (mass room)
	UNKNOWN(0x270, 0x274);
	FrArea* m_pStartTip;                 // 0x274 "start_tip"
	FrArea* m_pReadyTip;                 // 0x278 "ready_tip"
	// The window OnProc blinks (SetVisible toggled); OnReadyLBtnUp hides
	// it when m_ready is clear. Zeroed by the constructor and InitRoom; the
	// handler that stores it is not traced yet.
	FrWnd* m_pBlinkWnd;                  // 0x27c
	FrArea* m_p30sCountdown;             // 0x280 "group_30s.countdown" (mass room)
	FrArea* m_pResultRankingTab;         // 0x284 "group_result.rankingtab" (mass room)
	FrEdit* m_pTitle;                    // 0x288 "title"
	FrButton* m_pModify;                 // 0x28c "modify"
	FrButton* m_pResultUserInfo;         // 0x290 "group_result.user_info" (mass room)
	FrListBox* m_pResultRanking;         // 0x294 "group_result.ranking" (mass room, OnResultRankingInit 0x513910)
	FrListBox* m_pResultGuildRanking;    // 0x298 "group_result.guildranking" (mass room, 0x5140c0)
	FrArea* m_pModeTitle;                // 0x29c "mode_title" (single room)
	FrGroupBox* m_pCourseCtrl;           // 0x2a0 "course_ctrl"
	FrGroupBox* m_pChipinCtrl;           // 0x2a4 "chipin_ctrl"
	FrArea* m_pBgCourse;                 // 0x2a8 "course_ctrl.bg_course"
	FrButton* m_pFixHole;                // 0x2ac "course_ctrl.fix_hole"
	FrButton* m_pFixHolecupPos;          // 0x2b0 "course_ctrl.fix_holecup_pos"
	FrComboCtlEx* m_pChipinSlope;        // 0x2b4 "chipin_ctrl.slope"
	FrComboCtlEx* m_pChipinSize;         // 0x2b8 "chipin_ctrl.size"
	FrArea* m_pChipinTicket;             // 0x2bc "chipin_ctrl.ticket"
	FrGroupBox* m_pAtft;                 // 0x2c0 "atft"
	FrArea* m_pArtifactIcon;             // 0x2c4 "atft.artifact_icon"
	FrButton* m_pAssist;                 // 0x2c8 "assist"
	FrArea* m_pAssistTip;                // 0x2cc "assist_tip"
	FrArea* m_pAssistTitle;              // 0x2d0 "assist_title"
	FrArea* m_pShortGame;                // 0x2d4 "shortgame"
	FrArea* m_pGrandPrixInfo;            // 0x2d8 "group_gp.grandprixinfo" (GP room)
	FrButton* m_pGPDetail;               // 0x2dc "group_gp.gp_detail" (GP room)
	FrGroupBox* m_pGroupStroke;          // 0x2e0 "group_stroke"
	FrGroupBox* m_pGroup30s;             // 0x2e4 "group_30s"
	FrGroupBox* m_pGroupResult;          // 0x2e8 "group_result"
	FrGroupBox* m_pGroupGP;              // 0x2ec "group_gp"
	// The two team lists of the 30s / guild rooms (KR CLobbyMain::
	// m_slotList1/2): MakeRoomUserList clears both (0x5062f0), push_backs
	// (0x507d50) the address of each finished room player (JP CSharedDoc
	// +0x109fc nodes +0x8) by guild uid (+0x1095f first, +0x10963 second)
	// or team (record +0x53 & 3: 0 first, else second), then sorts both
	// (0x50e700) with 0x59fbd0. VS2010 lists, 0xc each.
	std::list<sSlotInfo*> m_slotList1;   // 0x2f0
	std::list<sSlotInfo*> m_slotList2;   // 0x2fc
	// The user list's images (OnUserInit: FrListBox::GetBitmap of the list,
	// m_pKickBitmap from Fresh::RegisterBitmap).
	const Bitmap* m_pTeamBitmap[4];      // 0x308 "team_red", "team_blue", "team_base", "team_base02" (OnUserLBtnDown indexes it by the player's team & 3)
	const Bitmap* m_pInviteBitmap;       // 0x318 "invite"
	const Bitmap* m_pMasterSignBitmap;   // 0x31c "master_sign"
	const Bitmap* m_pReadySignBitmap;    // 0x320 "ready_sign"
	// "30in_situ03", "30in_situ01", "30in_situ02", "30in_situ_red_cp",
	// "30in_situ_red", "30in_situ_blue_cp", "30in_situ_blue"
	// (FrMassTypeRoomDlg::On30sUserInit 0x515090).
	const Bitmap* m_p30sSituBitmap[7];   // 0x324
	const Bitmap* m_pTeamSelectBitmap;   // 0x340 "team_select"
	const Bitmap* m_pDiveBitmap;         // 0x344 "dive"
	UNKNOWN(0x348, 0x350);
	// FrMassTypeRoomDlg::OnResultRankingInit 0x513910 (RegisterBitmap).
	const Bitmap* m_p30sRedTeamRankBitmap;  // 0x350 "30in_team_redteam_rank"
	const Bitmap* m_p30sBlueTeamRankBitmap; // 0x354 "30in_team_blueteam_rank"
	const Bitmap* m_p30sRankBitmap;      // 0x358 "30in_rank"
	const Bitmap* m_pRankGoldBitmap;     // 0x35c "rank_gold"
	const Bitmap* m_pRankSilverBitmap;   // 0x360 "rank_silver"
	const Bitmap* m_pRankBronzeBitmap;   // 0x364 "rank_bronze"
	const Bitmap* m_pRankNoAwardBitmap;  // 0x368 "rank_noaward"
	const Bitmap* m_p30sNoPlayerBase;    // 0x36c "30_noplayer_base" (On30sUserInit)
	const Bitmap* m_p30sPlayerBase;      // 0x370 "30_player_base"
	UNKNOWN(0x374, 0x37c);
	const Bitmap* m_pKickBitmap;         // 0x37c "kick"
	UNKNOWN(0x380, 0x394);
	// "i_male", "i_female", "i_male_02", "i_female_02", "i_male_03",
	// "i_female_03", "i_male_manner", "i_female_manner", "i_male_angel",
	// "i_female_angel".
	const Bitmap* m_pGenderIcon[10];     // 0x394
	const Bitmap* m_pRoomEventBitmap;    // 0x3bc "room_event02" (zeroed by the constructor)
	UNKNOWN(0x3c0, 0x3c4);
	// The GP waiting room (FrGPTypeRoomDlg::OnGameStartWaitingInit 0x514a80
	// clears the byte and both ticks, reads the two times from
	// "grandprix_ai_player.lua", table "ai_player_waiting_room_slot_add_table").
	bool m_gpWaiting;                    // 0x3c4
	unsigned long m_gpWaitTick0;         // 0x3c8
	unsigned long m_gpWaitTick1;         // 0x3cc
	float m_gpAddSlotTimeSec;            // 0x3d0 "add_slot_time_sec"
	float m_gpGameStartWaitingTimeSec;   // 0x3d4 "game_start_waiting_time_sec"
	// The approach gift timer (KR CLobbyMain::m_approachGiftTimer): 1.0f
	// when idle; OnProc sets -9, -12 or -20 by the gift count when it opens
	// the gift, counts it up and ends the gift past 0.
	float m_approachGiftTimer;           // 0x3d8
	// RequestCloseInState8's CanLeaveFinishedGame result (the constructor
	// zeroes the dword).
	bool m_canLeaveFinishedGame;         // 0x3dc
	UNKNOWN(0x3dd, 0x3e0);
	// The current room tab when the room opened (JpSharedDoc_GetCurrentRoomTab
	// 0x62bf50: 1 GP, 0 multiplayer).
	int m_roomTab;                       // 0x3e0
	// The bitmap font "[font_wind.jpg" (16x16 glyphs on a 128x64 page,
	// "1234567890ym%-.?/:,"), created by the constructor, released by the
	// destructor.
	WTitleFont* m_pWindFont;             // 0x3e4
	// OnProc's tooltip hover state of m_pAssistTip and m_pArtifactIcon
	// (the constructor zeroes the word).
	bool m_assistTipShown;               // 0x3e8
	bool m_artifactTipShown;             // 0x3e9
	UNKNOWN(0x3ea, 0x3ec);
	// Two VS2010 std::string (0x1c each; the constructor stores the
	// capacity 0xf at +0x14): the texts OnProc's hover tooltips show.
	std::string m_assistTipText;         // 0x3ec (m_pAssistTip)
	// The GP detail / artifact icon tooltip (FrGPTypeRoomDlg::OnGPDetailInit
	// 0x515ac0 fills it).
	std::string m_artifactTipText;       // 0x408 (m_pArtifactIcon)
	UNKNOWN(0x424, 0x430);
};
// size 0x430 (x86)
BEGIN_LAYOUT_CHECK(FrGameRoomDlg)
	CHECK_FIELD(FrGameRoomDlg, m_pRoomData, 0x140);
	CHECK_FIELD(FrGameRoomDlg, m_pNewRecordDlg, 0x144);
	CHECK_FIELD(FrGameRoomDlg, m_pLobbyData, 0x14c);
	CHECK_FIELD(FrGameRoomDlg, m_pGrandPrixRoom, 0x150);
	CHECK_FIELD(FrGameRoomDlg, m_lobbyValue, 0x154);
	CHECK_FIELD(FrGameRoomDlg, m_pLobbyTitleFont, 0x158);
	CHECK_FIELD(FrGameRoomDlg, m_pRoomOwned, 0x15c);
	CHECK_FIELD(FrGameRoomDlg, m_pScoreDlg, 0x160);
	CHECK_FIELD(FrGameRoomDlg, m_pResultReportDlg, 0x164);
	CHECK_FIELD(FrGameRoomDlg, m_pApproachGiftDlg, 0x168);
	CHECK_FIELD(FrGameRoomDlg, m_pUserListDlg, 0x16c);
	CHECK_FIELD(FrGameRoomDlg, m_pLeaveGameNotice, 0x170);
	CHECK_FIELD(FrGameRoomDlg, m_pNotifyForm, 0x174);
	CHECK_FIELD(FrGameRoomDlg, m_roomState, 0x178);
	CHECK_FIELD(FrGameRoomDlg, m_idleWarned, 0x17c);
	CHECK_FIELD(FrGameRoomDlg, m_addOwnPlayer, 0x18d);
	CHECK_FIELD(FrGameRoomDlg, m_slotList1, 0x2f0);
	CHECK_FIELD(FrGameRoomDlg, m_slotList2, 0x2fc);
	CHECK_FIELD(FrGameRoomDlg, m_pResultRanking, 0x294);
	CHECK_FIELD(FrGameRoomDlg, m_pResultGuildRanking, 0x298);
	CHECK_FIELD(FrGameRoomDlg, m_p30sSituBitmap, 0x324);
	CHECK_FIELD(FrGameRoomDlg, m_pTeamSelectBitmap, 0x340);
	CHECK_FIELD(FrGameRoomDlg, m_p30sRedTeamRankBitmap, 0x350);
	CHECK_FIELD(FrGameRoomDlg, m_pRankNoAwardBitmap, 0x368);
	CHECK_FIELD(FrGameRoomDlg, m_p30sNoPlayerBase, 0x36c);
	CHECK_FIELD(FrGameRoomDlg, m_p30sPlayerBase, 0x370);
	CHECK_FIELD(FrGameRoomDlg, m_pRoomEventBitmap, 0x3bc);
	CHECK_FIELD(FrGameRoomDlg, m_gpWaiting, 0x3c4);
	CHECK_FIELD(FrGameRoomDlg, m_gpWaitTick0, 0x3c8);
	CHECK_FIELD(FrGameRoomDlg, m_gpWaitTick1, 0x3cc);
	CHECK_FIELD(FrGameRoomDlg, m_gpAddSlotTimeSec, 0x3d0);
	CHECK_FIELD(FrGameRoomDlg, m_gpGameStartWaitingTimeSec, 0x3d4);
	CHECK_FIELD(FrGameRoomDlg, m_approachGiftTimer, 0x3d8);
	CHECK_FIELD(FrGameRoomDlg, m_assistTipShown, 0x3e8);
	CHECK_FIELD(FrGameRoomDlg, m_artifactTipShown, 0x3e9);
	CHECK_FIELD(FrGameRoomDlg, m_assistTipText, 0x3ec);
	CHECK_FIELD(FrGameRoomDlg, m_artifactTipText, 0x408);
END_LAYOUT_CHECK

CHECK_SIZE(JpGrandPrixRoomObject, 0x7c);
BEGIN_LAYOUT_CHECK(JpGrandPrixRoomObject)
	CHECK_FIELD(JpGrandPrixRoomObject, m_lobbyValue, 0x0);
	CHECK_FIELD(JpGrandPrixRoomObject, m_fixedEquipment, 0x74);
	CHECK_FIELD(JpGrandPrixRoomObject, m_headValues, 0x4);
	CHECK_FIELD(JpGrandPrixRoomObject, m_value48, 0x48);
	CHECK_FIELD(JpGrandPrixRoomObject, m_value64, 0x64);
	CHECK_FIELD(JpGrandPrixRoomObject, m_value78, 0x78);
END_LAYOUT_CHECK

// JP 0x59f3c0 (the JP ConnectionUserCompare of KR CLobbyMain, same shape;
// name ours): the user list sort predicate, rows without a player last,
// by JpSlotInfo_GetWaitingSlot (JpGameRoomDlg.cpp).
bool JpRoomUserCompare(const void* lhs, const void* rhs);
// The JP room player record's byte +0x2e, which 0x59f3c0 sorts by: the KR
// sSlotInfo::m_connection (0x2f; the JP record sits one byte lower there,
// as the team bits at JP +0x53 / KR +0x54 show); the GP waiting room uses
// it as the 1-based slot it fills the player into (accessor ours).
unsigned char JpSlotInfo_GetWaitingSlot(const sSlotInfo& slot);
// JP 0x59fb40 (the KR CLobbyMain::_RefreshUccClothes, a free function in
// JP; name ours): ProcRoomExtras' per-player step (JpGameRoomDlg.cpp).
void JpRoom_RefreshUccClothes(sSlotInfo* slot, unsigned long typeId, const char* uccKey);
// JP 0x59fbd0 (the JP FinishUserCompare of KR CLobbyMain; name ours): the
// finished players' sort predicate (JpGameRoomDlg.cpp).
bool JpRoomFinishCompare(const void* lhs, const void* rhs);
// The JP-only record byte +0x161 (port placeholder, JpGameRoomDlg.cpp).
bool JpSlotInfo_IsSkippedInResults(const sSlotInfo& slot);
// The JP-only record byte +0x159 (port placeholder, JpGameRoomDlg.cpp).
bool JpSlotInfo_IsEventExcluded(const sSlotInfo& slot);

// JP LobbyMain 0xab6be0 at 0xab6d60: FrGameRoomDlg::SetLobbyValues 0x4ff300
// on the room form the lobby just created, with its fonts +0x784 and +0x794
// (KR LobbyMain.cpp under JP_PAKS declares it; JpGameRoomDlg.cpp).
void JpRoomTypeForm_SetLobbyValues(FrForm* form, WTitleFontEx* font154, WTitleFontEx* titleFont);

#endif // PG_JP_JPGAMEROOMDLG_H
