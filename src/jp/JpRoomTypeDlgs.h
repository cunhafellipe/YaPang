// JP-only game code (no KR counterpart): the four room dialogs derived from
// FrGameRoomDlg (JpGameRoomDlg.h), one per room type. LobbyMain 0xab6be0
// (the room's type, 3..8, through a jump table at 0xab6ddc) closes the
// previous room form (+0x77c / +0x780) and opens
//   3    FrStrokeTypeRoomDlg  CreateForm 0xab5490, layout "gameroom_main"
//   4, 5 FrMassTypeRoomDlg    CreateForm 0xab5560, layout "gameroom_main"
//   6    FrGPTypeRoomDlg      CreateForm 0xab5630, layout "gameroom_main"
//   7    FrSingleTypeDlg      CreateForm 0xab5700, layout "gameroom_single"
//   8    FrMassTypeRoomDlg    CreateForm 0xab5560, layout "gameroom_main"
// each CreateForm<T>(g_pFresh +0x40 manager, LobbyMain +0x18, layout, 0),
// stored at LobbyMain +0x780. jp/pairing.tsv pairs these four CreateForm
// instances to KR CreateForm<FrMakeRoomDlg>, <FrHalloweenEventGiftDlg>,
// <FrTreasureCourse> and <FrUserListDlg>: wrong, they build the classes
// above (R577).
//
// All four keep FrGameRoomDlg's vtable except [0] GetRTTI, [1] the
// deleting destructor, [9] OnProc, [32] OnInit and the FrCmdTarget
// GetMessageMap (this-4 vtable [1]); their message maps chain to
// FrGameRoomDlg's 0xe0870c (FrGPTypeRoomDlg's to FrMassTypeRoomDlg's).
// JP map rows are 0x20 bytes (handler pointer 16 bytes, filled by a static
// init, then name, command, type).
//
// Several rows use FrGameRoomDlg handlers the base map does not list
// ("<group>.user", "overgauge", "invite"; declared in JpGameRoomDlg.h).
// The subclass Init handlers store the controls into FrGameRoomDlg's
// protected members.
#ifndef PG_JP_JPROOMTYPEDLGS_H
#define PG_JP_JPROOMTYPEDLGS_H

#include "defines.h"
#include "padding.h"
#include "JpGameRoomDlg.h"
#include "TitleFontEx.h"

// FrStrokeTypeRoomDlg: stroke rooms. RTTI 0xe74f7c (static init 0xccdd80),
// size 0x430 (no member of its own); vtables 0xd0863c, 0xd08630 (this-4),
// 0xd08628 (this-8); message map 0xe0acc0 (rows 0xe0acc8, handlers stored
// by 0xccddc0); make instance 0x523370, registered as
// "FrStrokeTypeRoomDlg" by 0xccdda0.
class FrStrokeTypeRoomDlg : public FrGameRoomDlg {
public:
	static const WRTTI m_RTTI;                                        // 0xe74f7c
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x5232d0

	// FrGameRoomDlg's constructor, then InitRoom.
	FrStrokeTypeRoomDlg(void);                                        // 0x523260
	virtual ~FrStrokeTypeRoomDlg(void);                               // [1] 0x523400 (deleting), 0x5232e0

	virtual void OnProc(const float dt);                              // [9] 0x523330
	// group_stroke (FrGameRoomDlg +0x2e0) shown, frame_bg (+0x1a4) given
	// "gameroom_background.tga" (FrViewer::Open 0xbf2510), then
	// FrGameRoomDlg's OnInit.
	virtual bool OnInit(void);                                        // [32] 0x523300

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe0acc8

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe0acc0
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0x523250

	void OnPet0Init(PG_MSGARG param);                                 // 0x523430
	void OnPet1Init(PG_MSGARG param);                                 // 0x5234a0
	void OnPet2Init(PG_MSGARG param);                                 // 0x523510
	void OnPet3Init(PG_MSGARG param);                                 // 0x523580
};
// size 0x430 (x86)

// FrMassTypeRoomDlg: the 30-player ("group_30s") and team/guild rooms, with
// the result board ("group_result"). RTTI 0xe74e38, size 0x4b0; vtables
// 0xd06d3c, 0xd06d2c (this-4), 0xd06d24 (this-8); message map 0xe09464
// (rows 0xe09478, handlers stored by 0xccc4e0); make instance 0x515000,
// registered by 0xccc4c0.
class FrMassTypeRoomDlg : public FrGameRoomDlg {
	LAYOUT_CHECK_FRIEND;
public:
	static const WRTTI m_RTTI;                                        // 0xe74e38
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x510ff0

	// Zeroes FrGameRoomDlg +0x2e4 / +0x2ec; unless +0x3e0 is set, runs
	// 0x511d20 when the shared doc (0xe47dbc, 0x62bf90) is in state 8,
	// else 0x510a20.
	FrMassTypeRoomDlg(void);                                          // 0x512ee0
	virtual ~FrMassTypeRoomDlg(void);                                 // [1] 0x511c60 (deleting), 0x510fd0

	// In state 8 the title shows the "%02d:%02d" time left (UpdateTitle).
	virtual void OnProc(const float dt);                              // [9] 0x511c90
	virtual bool OnInit(void);                                        // [32] 0x5162c0 (195 bytes)

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe09478

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe09464
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0x510a10

	void OnInviteInit(PG_MSGARG param);                               // 0x511290 (FrButton -> +0x228)
	void OnInviteLBtnUp(void);                                        // 0x516390
	void OnRoomEventOwnerDraw(PG_MSGARG param);                       // 0x511f10
	void On30sUserInit(PG_MSGARG param);                              // 0x515090
	void On30sUserOwnerDraw(PG_MSGARG param);                         // 0x513270
	void On30sTeam1Init(PG_MSGARG param);                             // 0x511060 (FrArea -> +0x25c)
	void On30sTeam2Init(PG_MSGARG param);                             // 0x5110d0 (-> +0x260)
	void On30sGuild1Init(PG_MSGARG param);                            // 0x511140 (-> +0x264)
	void On30sGuild2Init(PG_MSGARG param);                            // 0x5111b0 (-> +0x268)
	void On30sTeamNumInit(PG_MSGARG param);                           // 0x511220 (-> +0x238)
	void On30sTeamNumOwnerDraw(PG_MSGARG param);                      // 0x511fc0
	void On30sGuildNumInit(PG_MSGARG param);                          // 0x5121d0
	void On30sGuildNumOwnerDraw(PG_MSGARG param);                     // 0x512250
	void On30sCountdownInit(PG_MSGARG param);                         // 0x511300
	void OnResultUserInfoInit(PG_MSGARG param);                       // 0x511380
	void OnResultUserInfoLBtnDown(void);                              // 0x515810
	void OnResultRankFrameInit(PG_MSGARG param);                      // 0x511400 (-> +0x248)
	void OnResultScoreGbInit(PG_MSGARG param);                        // 0x5125a0
	void OnResultRankingTabInit(PG_MSGARG param);                     // 0x512610
	void OnResultRankingInit(PG_MSGARG param);                        // 0x513910
	void OnResultRankingOwnerDraw(PG_MSGARG param);                   // 0x513ac0
	void OnResultRankingLBtnDown(void);                               // 0x511470
	void OnResultRankingRBtnUp(void);                                 // 0x5158c0
	void OnResultGuildRankingInit(PG_MSGARG param);                   // 0x5140c0
	void OnResultGuildRankingOwnerDraw(PG_MSGARG param);              // 0x5169b0
	void OnResultGuildRankingLBtnDown(void);                          // 0x5114f0
	void OnResultGuildRankingRBtnUp(void);                            // 0x5153d0
	void OnResultScoreInit(PG_MSGARG param);                          // 0x511540
	void OnResultScoreLBtnDown(void);                                 // 0x514200

	// The constructor's room setup in lobby mode 8 (106 bytes): clears
	// +0x17c..+0x181 (+0x17d/+0x17e set when the JP CSharedDoc +0x812 is
	// 2), the two scene calls (0xf023e0 / 0xf023e4 vtable +0x3c), the lobby
	// container's picked user to -1; InitRoom without its posts. JP game
	// state, not decompiled yet (name ours).
	void InitRoomInLobbyMode8(void);                                  // 0x511d20
	// OnInit in room states 4 and 5 (327 bytes): when +0x18d is set, adds
	// the own player record (JP game state, not written yet), then the
	// course images and the title (name ours).
	void SetupStateWaiting(void);                                     // 0x5160a0
	// OnInit in room state 8 (322 bytes): OpenNewRecordForm, then hides the
	// map, mapprev, mapnext, mapcomboex, start, ready, modify, shortgame and
	// cur_* controls (name ours).
	void SetupStateResult(void);                                      // 0x510a40

	// 0x430-0x4b0: the mass and GP rooms' own members, filled by
	// FrGPTypeRoomDlg::LoadGrandPrixRoomUi from "grandprix_room_ui.lua"
	// and drawn by OnGrandPrixInfoOwnerDraw 0x5126d0 (which reads the
	// images +0x430..+0x440 and the offsets).
	// "grandprixroomuiname": the GP info images, zeroed first (0x14 bytes).
	const Bitmap* m_gpUiImages[5];       // 0x430
	// "grandprixroomuioffset": {x, y} pairs, added to the info box's
	// position (the info draw reads them up to +0x498).
	WPoint m_gpUiOffsets[11];            // 0x444
	// Zeroed by FrGPTypeRoomDlg::SetupGrandPrixCourse, which hands its
	// address to the "Gateway" actor (message 0x753e, 4, item type
	// 0x1a000264) to fill; OnGrandPrixInfoOwnerDraw 0x5126d0 prints it
	// (name ours: the count of that item, role inferred).
	uint32_t m_gpEntryItemCount;         // 0x49c
	// While set, OnResultUserInfoLBtnDown 0x515810 does nothing;
	// OnResultRankingInit clears it (writer not traced).
	int m_resultUserInfoBlocked;         // 0x4a0
	UNKNOWN(0x4a4, 0x4b0);
};
// size 0x4b0 (x86)
BEGIN_LAYOUT_CHECK(FrMassTypeRoomDlg)
	CHECK_FIELD(FrMassTypeRoomDlg, m_gpUiImages, 0x430);
	CHECK_FIELD(FrMassTypeRoomDlg, m_gpUiOffsets, 0x444);
	CHECK_FIELD(FrMassTypeRoomDlg, m_gpEntryItemCount, 0x49c);
	CHECK_FIELD(FrMassTypeRoomDlg, m_resultUserInfoBlocked, 0x4a0);
END_LAYOUT_CHECK
CHECK_SIZE(FrMassTypeRoomDlg, 0x4b0);

// FrGPTypeRoomDlg: Grand Prix rooms ("group_gp"). Derives from
// FrMassTypeRoomDlg (its constructor calls 0x512ee0, its destructor ends in
// 0x510fd0) and keeps its OnProc 0x511c90; no member of its own (size
// 0x4b0). RTTI 0xe74e44; vtables 0xd06dec, 0xd06de0 (this-4), 0xd06dd8
// (this-8); message map 0xe0946c (base 0xe09464; rows 0xe09898, handlers
// stored by 0xccc8d0); make instance 0x5153e0, registered by 0xccc8b0.
class FrGPTypeRoomDlg : public FrMassTypeRoomDlg {
public:
	static const WRTTI m_RTTI;                                        // 0xe74e44
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x511630

	// FrMassTypeRoomDlg's constructor, then LoadGrandPrixRoomUi.
	FrGPTypeRoomDlg(void);                                            // 0x514340
	// ReleaseGrandPrixRoomUi, then FrMassTypeRoomDlg's destructor.
	virtual ~FrGPTypeRoomDlg(void);                                   // [1] 0x5126a0 (deleting), 0x5115c0

	virtual bool OnInit(void);                                        // [32] 0x516fc0 (377 bytes)

	// OnInit's GP room setup (382 bytes): the course images (the room's
	// course, or 0x11 for a known grand prix), the title, the start button
	// hidden and the ready button shown, the entry item count query, the
	// fixed equipment, the level bar and the "gp_lobby_amb" sound (name
	// ours).
	void SetupGrandPrixCourse(void);                                  // 0x511d90

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe09898

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe0946c
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0x510bf0

	void OnGPUserOwnerDraw(PG_MSGARG param);                          // 0x5143b0
	void OnGPDetailInit(PG_MSGARG param);                             // 0x515ac0
	void OnGPDetailHoverOn(PG_MSGARG param);                          // 0x5116b0
	void OnGPDetailHoverOff(PG_MSGARG param);                         // 0x5116c0
	void OnGrandPrixInfoInit(PG_MSGARG param);                        // 0x5116d0
	void OnGrandPrixInfoOwnerDraw(void);                              // 0x5126d0
	void OnGameStartWaitingInit(void);                                // 0x514a80
	void OnGameStartWaitingOwnerDraw(void);                           // 0x515470

	// 745 bytes: clears +0x17c/+0x180/+0x274/+0x278, two sound/scene calls
	// (0xf023e0, 0xf023e4), creates the 0x7c-byte object at +0x150
	// (0x55cfa0, 0x55dd40) and reads "grandprix_room_ui.lua"
	// ("grandprixroomuiname", "grandprixroomuioffset") into +0x430..;
	// JP Lua and game state, not decompiled yet.
	void LoadGrandPrixRoomUi(void);                                   // 0x512f80
	// Stops the "gp_lobby_amb" sound and deletes the +0x150 object
	// (0x55d020; the delete is not written yet).
	void ReleaseGrandPrixRoomUi(void);                                // 0x511020
};
// size 0x4b0 (x86)

// JP-only WTitleFontEx (0xc bytes, vtable 0xd1c0e0, deleting destructor
// 0x6f5b00) FrSingleTypeDlg owns at +0x430 and prints the chip-in ticket
// count with: digits "0123456789x" (10 x 13 in a 128 x 16 page) from
// "[single_num_course.jpg" (JP font file entry 0xe16868). Name ours.
class JpSingleNumFont : public WTitleFontEx {
public:
	JpSingleNumFont(void);                                            // 0x6f4ac0
};
CHECK_SIZE(JpSingleNumFont, 0xc);

// FrSingleTypeDlg: the single-player room ("gameroom_single": course,
// holes, chip-in options). RTTI 0xe74f70, size 0x440; vtables 0xd08304,
// 0xd082f4 (this-4), 0xd082ec (this-8); message map 0xe0a99c (rows
// 0xe0a9a8, handlers stored by 0xccdae0); make instance 0x5228a0,
// registered by 0xccdac0. LobbyMain 0xab6be0 also moves it after
// creation (its rect +0x34..+0x40 and 0xf023e8 +0x19c).
class FrSingleTypeDlg : public FrGameRoomDlg {
public:
	static const WRTTI m_RTTI;                                        // 0xe74f70
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x522190

	// FrGameRoomDlg's constructor, then InitSingleRoom.
	FrSingleTypeDlg(void);                                            // 0x522120
	// ReleaseSingleRoom, then FrGameRoomDlg's destructor.
	virtual ~FrSingleTypeDlg(void);                                   // [1] 0x522930 (deleting), 0x5221c0

	// FrGameRoomDlg's OnProc; ESCAPE (CInputManager 0xf00510, 0x9a6ce0 /
	// 0x9a6d30) closes the form through vtable +0x68; JP input glue, not
	// decompiled yet.
	virtual void OnProc(const float dt);                              // [9] 0x521f80
	// SetupSingleModeControls, JpFrWnd_RaiseFlagF1aac8 (0xbe7870), "Lobby"
	// 0x59/0xb post when the lobby desktop is up, AddPlayer(shared doc
	// +0x81a), then FrForm::OnInit (0xbe6570).
	virtual bool OnInit(void);                                        // [32] 0x523160

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe0a9a8

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe0a99c
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0x521f70

	void OnModeTitleInit(PG_MSGARG param);                            // 0x5222d0
	void OnCourseCtrlInit(PG_MSGARG param);                           // 0x5223b0
	void OnBgCourseInit(PG_MSGARG param);                             // 0x522490
	void OnBgCourseOwnerDraw(void);                                   // 0x522a40
	void OnFixHoleInit(PG_MSGARG param);                              // 0x522500
	void OnFixHoleLBtnUp(void);                                       // 0x522bf0
	// The "hole_selector" form's result (FrHoleSelector, JpHoleSelector.h):
	// on FrOK a hole other than the room's is sent as room-info field 0xb
	// (name ours).
	bool OnHoleSelectorResult(int ret, FrForm* form);                 // 0x522960
	void OnFixHoleOwnerDraw(void);                                    // 0x522cd0
	void OnFixHolecupPosInit(PG_MSGARG param);                        // 0x522580
	void OnFixHolecupPosLBtnUp(void);                                 // 0x522d90
	void OnChipinCtrlInit(PG_MSGARG param);                           // 0x522420
	void OnChipinSlopeInit(PG_MSGARG param);                          // 0x522e60
	void OnChipinSlopeLBtnDown(void);                                 // 0x522600
	void OnChipinSizeInit(PG_MSGARG param);                           // 0x522640
	void OnChipinSizeLBtnDown(void);                                  // 0x5226b0
	void OnChipinTicketInit(PG_MSGARG param);                         // 0x5226f0
	void OnChipinTicketOwnerDraw(void);                               // 0x522760
	void OnCloseSingleLBtnUp(void);                                   // 0x5220d0

	// FrGameRoomDlg::InitRoom, then m_pNumFont = 0 and the ticket text
	// zeroed; m_pNumFont = new JpSingleNumFont when null, the base's
	// m_pRoomOwned = new JpGameRoomOwned when null, the ticket text zeroed
	// again.
	void InitSingleRoom(void);                                        // 0x521fd0
	// m_pNumFont deleted (vtable [0] with 1) and zeroed, then
	// FrGameRoomDlg::ReleaseRoom.
	void ReleaseSingleRoom(void);                                     // 0x5220a0
	// OnInit's first step (595 bytes): the background's alpha 0, then by
	// the room mode (JP CSharedDoc +0x109b8; 0x13 the hole room, which
	// hides the chip-in box, else the chip-in room, which hides the course
	// box and fills the slope / size combos and the ticket count) the
	// title images, then the course images (name ours).
	void SetupSingleModeControls(void);                               // 0x522f00

private:
	// 0x430: the chip-in ticket font InitSingleRoom creates.
	JpSingleNumFont* m_pNumFont;         // 0x430
	// 0x434: the chip-in ticket count ("x%d", strncpy 7) the "chipin_ctrl.ticket"
	// owner draw prints; zeroed (two dwords) twice by InitSingleRoom.
	char m_chipinTicketText[8];          // 0x434
	UNKNOWN(0x43c, 0x440);
};
// size 0x440 (x86)

#endif // PG_JP_JPROOMTYPEDLGS_H
