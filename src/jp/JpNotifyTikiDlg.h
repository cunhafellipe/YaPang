// JP-only game code (no KR counterpart): FrUseTikiReportDlg, the form of
// layout "notify_tiki" that offers to end a finished game early with a
// "tiki report" item (type id 0x1A000041). CLobbyMain's message 0x8b (12)
// (JP 0xabcbaa) creates it (0xab4b30) once, fills its "notice" edit with a
// fixed text and its "amount" edit with the player's count of the item, and
// opens it with OnUseTikiReportDlgResult (JP 0xab2ec0), which closes the
// open room form.
//
// JP x86 facts: RTTI 0xf0336c "FrUseTikiReportDlg" (static init 0xcf1b90,
// base FrForm 0xf1aabc), registered with the JP form factory (static init
// 0xcf1bb0: 0xa4c0d0 / 0xa4bcf0, make instance 0xafa380), size 0x120.
// Base the JP FrForm (ctor 0xbe5e20, dtor 0xbe5e80, 38 slots). Vtables
// 0xd4ea04 (38 slots: [0] GetRTTI 0xafa300, [1] the deleting destructor
// 0xafa410; the rest FrForm's), 0xd4e9f4 (FrCmdTarget: [0] thunk 0xafa370,
// [1] GetMessageMap 0xafa2b0), 0xd4e9ec ([0] thunk 0xafa360). Message map
// 0xe3c018 (rows 0xe3c020, 0x20 bytes each, handlers stored by the static
// init 0xcf1bd0, base 0xe4343c). The port keeps the KR FrForm layout, so
// the JP offsets are comments only (no CHECK_FIELD), like
// JpMPBreakCountSetDlg.h.
#ifndef PG_JP_JPNOTIFYTIKIDLG_H
#define PG_JP_JPNOTIFYTIKIDLG_H

#include "defines.h"
#include "frcmdtarget.h"
#include "frform.h"
#include "wtypes.h"

class FrEdit;
class FrWnd;
class FrWndManager;

class FrUseTikiReportDlg : public FrForm {
public:
	static const WRTTI m_RTTI;                                        // 0xf0336c
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0xafa300

	FrUseTikiReportDlg(void);                                         // 0xafa2c0
	virtual ~FrUseTikiReportDlg(void);                                // [1] 0xafa410 (deleting)

	// The JP FrEdit::AddText(text, 0, 1, 0, 0) / (text, 1, 1, 0, 0) on
	// "notice" / "amount" (names ours).
	void SetNotice(const char* text);                                 // 0xafa320
	void SetAmount(const char* text);                                 // 0xafa340

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe3c020

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe3c018
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0xafa2b0

	void OnNoticeInit(PG_MSGARG param);                               // [0] 0xafa440
	void OnAmountInit(PG_MSGARG param);                               // [1] 0xafa4b0
	// Uses one tiki report: the item's count (JP CSharedDoc +0x375c, the
	// KR m_itemInfoList) goes down by one and the item leaves the list
	// below 1; packet 0xaa carries the local player's JP game statistics
	// (JP CSharedDoc +0xffc8, 0x109 bytes); "yes" is disabled, JP
	// CSharedDoc +0x10488 cleared, the main actor gets message 1 (the
	// current task's +0x30 object, vtable +0x2c: the KR
	// SendMsgToMainActor) and the form closes with FrOK.
	void OnYesClick(void);                                            // [2] 0xafa730
	// Opens the "notify_tiki_sub" message box (JP 0x48b8a0) with the text
	// of key 0xd061e0 (Jp_LookupText), result OnSubDlgResult, flags 0x801.
	void OnNoClick(void);                                             // [3] 0xafa6b0
	// "notify_tiki_sub"'s result: on FrOK, its "ok" button disabled, then
	// packet 0xf (the room's result players' scores when the room kind,
	// JP CSharedDoc +0x1094c, is 4 or 5, matched by 0x47dd30), +0x10488
	// cleared, main actor message 1 and the form closed with FrNO.
	bool OnSubDlgResult(int result, FrForm* form);                    // 0xafa520

private:
	// JP +0x110: cleared by the constructor; no reader found.
	void* m_unknown110;                     // JP +0x110
	FrEdit* m_pNotice;                      // JP +0x114 "notice"
	FrEdit* m_pAmount;                      // JP +0x118 "amount"
};

// 0xafa380: the JP form factory's make instance for "FrUseTikiReportDlg".
IObject* FrUseTikiReportDlgMakeInstance(void);
// 0xab4b30: CreateForm<FrUseTikiReportDlg>(manager, owner, name, parent),
// with the thread-local slot (+0xb2b4) store and the cast.
FrUseTikiReportDlg* CreateUseTikiReportDlg(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent);

// JP CSharedDoc +0x10488's writer (the reader is Jp_GameState_10488,
// JpLobbyGlue.cpp).
void Jp_GameState_Set10488(bool value);

#endif // PG_JP_JPNOTIFYTIKIDLG_H
