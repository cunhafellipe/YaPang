// JP-only game code (no KR counterpart): the lobby's alchemy dialog, layout
// "AlchemyDlg" (the RTTI's FrAlchemyMPBreakDlg), which the JP CTaskMain opens
// from the lobby desktop's pop-up menu (0x659340 -> 0x655d70) unless the
// server closed the service (bit 33 of the JP 64-bit service flags).
//
// JP x86 facts: RTTI 0xe74d8c, size 0x6d0 (the `new` of the JP
// CreateForm<FrAlchemyMPBreakDlg> 0x6548a0), constructor 0x4fa3d0 (KR
// FrForm::FrForm 0xbe5e20, then its members from +0x110: two std::list-like
// heads, sub-objects at +0x188, +0x1e4 (4 x 0x1c, a vector constructor
// iterator), +0x258, six `new`ed helpers at +0x1d0..+0x1e0 (three 0x4c
// objects 0x923410, 0xc objects 0x6f5600 / 0x6f56c0), then 0x4f4a70),
// vtables 0xd05b14 (IObject/FrWnd, 38 slots: overrides [0] 0x4fa660, [1]
// 0x4faa60, [9] 0x4f8000, [32] 0x4fb490), 0xd05b08 (FrCmdTarget: [0]
// 0x4fa670, [1] 0x4f3450), 0xd05b00 ([0] 0x4fa680). Related JP-only RTTI:
// FrAlchemyMPBreakNoticeDlg, CAlchemyMPBreak. The message map, the control
// init handlers, OnInit, the tab / help / magic box / card compose clicks
// and the constructor (the members the port declares), the view state, the
// Tiki talk texts, the "Exchange_ok" click and the break notice are written
// (JpAlchemyDlg.cpp), with the destructor, OnProc, InitGauges and
// InitEffect, the gauge state, the break sequence, the gauge, point and
// effect draws, the own-item list, its draws and the item list clicks
// (with the count dialog's opener and result; the count dialog itself is
// FrItemCountController, JpMPBreakCountSetDlg.h). The port keeps the KR
// FrForm layout, so the JP offsets are comments only (no CHECK_FIELD).
//
// Packets: 0x18d (CAlchemyMPBreak 0x476a20, sent when its state +0x4 is 3:
// the map's size (+0x10) as a byte encoded in a dword, then per selected
// entry three dwords, node +0x14, +0x10, +0x18), sent after the notice
// dialog's confirmation.
#ifndef PG_JP_JPALCHEMYDLG_H
#define PG_JP_JPALCHEMYDLG_H

#include <stdint.h>

#include <list>
#include <map>
#include <string>

#include "defines.h"
#include "frcmdtarget.h"
#include "frform.h"
#include "fx.h"
#include "TitleFontEx.h"
#include "JpMaskGauge.h"
#include "wmath.h"

class CSharedDoc;
class CTaskMain;

class Bitmap;
class FrArea;
class FrButton;
class FrEdit;
class FrItemCountController;
class FrListBox;
struct FrListItem;
class WTitleFontEx;
class WResourceManager;

// The break effect at JP FrAlchemyMPBreakDlg +0x258 is the KR CPvsFxBox
// (fx.h): its vtable 0xcff340 has the RTTI .?AVCPvsFxBox@@ : CFxBox (JP
// size 0x43c, KR 0x364; constructor 0x48e9b0, destructor 0x9731c0). The JP
// calls: Init 0x96c7b0, SetCamera 0x96c7c0, SetScreenCenter 0x96ff00,
// Finished 0x96d7f0, ProcessAttacher [1] 0x87d980, Process 0x971b60,
// LoadSequence 0x9705a0, AddSequence 0x971340, Display 0x96fdd0, ClearAll
// 0x972b20.

// FrAlchemyMPBreakNoticeDlg, layout "AlchemyMPBreakNoticeDlg": the
// confirmation the alchemy dialog opens before the exchange (0x4f6740,
// factory 0x4f60f0 = CreateForm<FrAlchemyMPBreakNoticeDlg>, `new` 0x120).
// RTTI 0xe74d84; vtables 0xd058f4 ([0] 0x4f33a0, [1] 0x4f48f0; [6] 0x47ccd0
// and [10] 0x47ccf0 are FrWnd/FrForm header-inline copies), 0xd058e4
// (FrCmdTarget: [0] 0x4f3bd0 thunk, [1] GetMessageMap 0x4f3350), 0xd058dc
// ([0] 0x4f3be0 thunk). Message map 0xe07ad0 (rows 0xe07f20, handlers
// stored by the static init 0xccb120).
// size 0x120 (JP x86)
class FrAlchemyMPBreakNoticeDlg : public FrForm {
public:
	static const WRTTI m_RTTI;   // 0xe74d84
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }   // [0] 0x4f33a0

	FrAlchemyMPBreakNoticeDlg(void);            // 0x4f3360
	virtual ~FrAlchemyMPBreakNoticeDlg(void);   // [1] 0x4f48f0 (scalar deleting), 0x4f33b0

	// Appends a line to "FocuseMsg" / "Msg" (NULL ignored).
	void AddFocusText(const char* text);        // 0x4f33d0
	void AddMsgText(const char* text);          // 0x4f3400

private:
	static sFRESH_ENTRY _MsgEntries[];          // 0xe07f20

protected:
	static sFRESH_MSGMAP _MsgMap;               // 0xe07ad0
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;   // FrCmdTarget [1] 0x4f3350

	void OnFocusMsgInit(intptr_t param);        // 0x4f4920 "FocuseMsg"
	void OnMsgInit(intptr_t param);             // 0x4f4990 "Msg"
	void OnCautionMsgInit(intptr_t param);      // 0x4f4a00 "CautionMsg"

private:
	FrEdit* m_pFocusMsg;     // JP +0x110
	FrEdit* m_pMsg;          // JP +0x114
	FrEdit* m_pCautionMsg;   // JP +0x118
};

// An entry of CAlchemyMPBreak's selection (the mapped value of its map
// nodes, JP node +0x10..+0x1b): +0x10 the own-list entry's +0xc
// (0x4f9f60), +0x14 the item type id (FindCommonItem in 0x476b20), +0x18
// the selected count (0x476f90 multiplies it with 0x475f80(type id)).
// Packet 0x18d sends typeId, itemId, count.
// size 0xc (JP x86)
struct AlchemyBreakItem {
	int itemId;            // JP +0x0
	unsigned int typeId;   // JP +0x4
	int count;             // JP +0x8
};

// CAlchemyMPBreak: the exchange state of the alchemy dialog (JP-only RTTI),
// embedded at JP FrAlchemyMPBreakDlg +0x188. JP +0x0 is its vtable
// 0xcfe5d0 (not modeled: its slots are not decompiled).
// size 0x38 (JP x86)
class CAlchemyMPBreak {
public:
	CAlchemyMPBreak(void);      // 0x477600
	~CAlchemyMPBreak(void);
	// 0x476ba0: 0 when already in that state, -1 out of range (the
	// original's test, state <= 0 && state >= 4, never holds); otherwise
	// clears the selection, stores the state, 1.
	int SetState(int state);
	// 0x477270: the held MP, the selection's MP and the expected MP.
	void UpdatePoints(void);
	// 0x4f9980: a copy of the selection (0x4f96b0 = the map's copy).
	std::map<int, AlchemyBreakItem> GetSelection(void);

	// 0x475770: m_resultMP, m_resultBonus.
	void SetResult(int mp, int bonus);
	// 0x476be0: empties the selection.
	void ClearSelection(void);

	// 0x475790
	void ClearWarnings(void);
	// 0x475840: one of the four warnings is set.
	bool HasWarning(void);
	// 0x4757a0
	bool GetWarning(int index);
	// 0x4757d0: the text of a set warning (0x18705 + index), else NULL.
	const char* GetWarningText(int index);
	// 0x4757b0
	void SetWarning(int index, char value);
	// 0x476f10: ClearWarnings, then each of the four checks that is set
	// stored by SetWarning.
	void CheckWarnings(void);
	// 0x476bf0, 0x476c70, 0x476d20, 0x476e60: the four warning checks of
	// CheckWarnings (0x476d20 declared only).
	char CheckWarning0(void);
	char CheckWarning1(void);
	char CheckWarning2(void);
	char CheckWarning3(void);
	// 0x475f80: the MP one item of a type id gives.
	int GetItemMP(unsigned int typeId);
	// 0x4772a0: m_overLimit when the selection's MP plus the held 0x1a0002a7
	// count reaches 1000.
	void UpdateOverLimit(void);
	// 0x475740: the selection key of an entry: its itemId in state 3,
	// else 0 (no key).
	int MakeKey(const AlchemyBreakItem& item);
	// 0x477340: the selected entry of that key, NULL without one.
	AlchemyBreakItem* FindSelection(const AlchemyBreakItem& item);
	// 0x4776c0: the selected entry of the item's key; a new one (the item's
	// copy) while at most 5 are selected; NULL without a key or when full.
	AlchemyBreakItem* AddSelection(const AlchemyBreakItem& item);
	// 0x4772d0: the selected entry's count goes to *removedCount and is
	// taken off (the entry is erased at 0); the count left, 0 without the
	// entry.
	int RemoveSelection(const AlchemyBreakItem& item, int* removedCount);
	// 0x476a20: packet 0x18d when the state is 3: the entry count (a
	// byte, encoded as a dword), then per entry typeId, itemId, count.
	void SendBreak(void);
	// 0x476b20: the selection's FindCommonItem +0x98 sum (32-bit, returned
	// zero-extended in eax:edx).
	int64_t SumSelectionPrice(void);
	// 0x476f90: the sum over the selection of GetItemMP(typeId) * count
	// (0 per entry outside state 3).
	int SumSelectionMP(void);
	// 0x475f40: the held count of item 0x1a0002a6 (the JP item lookup
	// 0x63b570 on 0xe47dbc, its +0xc short), 0 without it.
	int GetHeldCountA6(void);
	// 0x475f60: the same for item 0x1a0002a7.
	int GetHeldCountA7(void);
	// 0x475ed0: the break class of a type id's record (JP +0x94: 1 Pang,
	// 2 Cookie, 3 Rare), 0 without a record.
	int GetBreakClass(unsigned int typeId);
	// 0x475f00: the item tab of a type id (break class 1..3), 0 for the
	// other classes.
	int GetItemTab(unsigned int typeId);

	// JP +0x4
	int m_state;
	// JP +0x8 (head +0xc, size +0x10): the selected items; the key is
	// built by 0x4776c0 (not decompiled).
	std::map<int, AlchemyBreakItem> m_selection;
	// JP +0x18: the warnings of CheckWarnings (0x4757b0 stores a char).
	char m_warnings[4];
	int m_heldMP;        // JP +0x1c (FrAlchemyMPBreakDlg +0x1a4, My_TP_Point)
	int m_selectionMP;   // JP +0x24
	int m_expectMP;      // JP +0x28 (FrAlchemyMPBreakDlg +0x1b0)
	// JP +0x2c: the MP the break gave (FinishBreakSequence's text 0x1870d,
	// or with the bonus 0x1870b).
	int m_resultMP;
	// JP +0x30: the bonus of a big break (StartBreakSequence plays
	// "Item_break_B.seq" when it is not 0).
	int m_resultBonus;
	bool m_overLimit;    // JP +0x34
	// JP +0x20: zeroed by the constructor; role not known.
};

// FrMPBreakAboutDlg, layout "mpbreak_about": the help form of the alchemy
// dialog's "Alchemy_Help" button (factory 0x4f61c0 =
// CreateForm<FrMPBreakAboutDlg>, `new` 0x150). RTTI type descriptor
// 0xe08504, vtables 0xd05824 / 0xd05818 / 0xd05810. Not decompiled: the
// constructor and destructor are declared only.
// size 0x150 (JP x86)
class FrMPBreakAboutDlg : public FrForm {
public:
	FrMPBreakAboutDlg(void);            // 0x4f31d0
	virtual ~FrMPBreakAboutDlg(void);
};

// size 0x6d0 (JP x86). The JP offsets of the members are comments only.
class FrAlchemyMPBreakDlg : public FrForm {
public:
	static const WRTTI m_RTTI;   // 0xe74d8c
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }   // [0] 0x4fa660

	FrAlchemyMPBreakDlg(void);            // 0x4fa3d0
	virtual ~FrAlchemyMPBreakDlg(void);   // [1] 0x4faa60 (scalar deleting); thunks 0x4fa670, 0x4fa680

	// 0x4f8000: the exchange sequence (m_procState 1..6), then the gauge
	// state 0x4f34a0 and the gauges' Update 0x4f35b0.
	virtual void OnProc(const float dt);  // [9]
	// 0x4fb490
	virtual bool OnInit(void);            // [32]

	// 0x4f3490: m_procState (1 starts the exchange sequence; HandleMsgCommon
	// case 0x2e5).
	void SetProcState(int state);
	// 0x4f89e0: the break result (HandleMsgCommon case 0x2e4): the
	// CAlchemyMPBreak's SetResult and UpdatePoints, then UpdateView(3) over
	// the limit (m_breaker.m_overLimit, JP +0x1bc), else UpdateView(2).
	void OnBreakResult(int mp, int bonus);
	// 0x4fb460: SetViewState(m_viewState), the selection emptied,
	// UpdateExchangeOk (JP CTaskMain 0x651080, HandleMsgCommon case 0x2e6).
	void ResetView(void);

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe07fe0 (handlers stored by the static init 0xccb220)

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe07ad8
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // FrCmdTarget [1] 0x4f3450

	// FRCMD_INIT handlers: the control pointers (DYNAMIC_CAST of the param).
	void OnMyTPPointInit(intptr_t param);        // 0x4f4d70 "My_TP_Point"
	void OnHelpInit(intptr_t param);             // 0x4f4e70 "Alchemy_Help" (SetPushDelay(0.01f))
	void OnExitInit(intptr_t param);             // 0x4f4f10 "Alchemy_Exit"
	void OnMagicBoxInit(intptr_t param);         // 0x4f4fb0 "MagicBox" (then EnableHover)
	void OnCardComposeInit(intptr_t param);      // 0x4f5060 "CardCompose"
	void OnExchangeInit(intptr_t param);         // 0x4f5110 "Exchange"
	void OnCookieItemInit(intptr_t param);       // 0x4f51c0 "Cookie_Item"
	void OnPangItemInit(intptr_t param);         // 0x4f5260 "Pang_Item"
	void OnRareItemInit(intptr_t param);         // 0x4f5300 "Rare_Item"
	void OnTikiTalkEditInit(intptr_t param);     // 0x4f53a0 "TikiTalk_Edit"
	void OnExchangeListBG1Init(intptr_t param);  // 0x4f5410 "Exchange_ListBG1"
	void OnExchangeListBG2Init(intptr_t param);  // 0x4f5490 "Exchange_ListBG2"
	void OnExchangeListBG3Init(intptr_t param);  // 0x4f5510 "Exchange_ListBG3"
	void OnExchangeListBG4Init(intptr_t param);  // 0x4f5590 "Exchange_ListBG4"
	void OnExchangeOnEffectInit(intptr_t param); // 0x4f5610 "ExchangeOnEffect" (the effect's screen center at the area's x, y)
	void OnMPGaugeInit(intptr_t param);          // 0x4f5850 "MPGauge"
	void OnExpectMPInit(intptr_t param);         // 0x4f58d0 "ExpectMP"
	void OnPrePointInit(intptr_t param);         // 0x4f59c0 "PrePoint"
	void OnExchangeOkInit(intptr_t param);       // 0x4f5b50 "Exchange_ok"
	void OnOwnItemInit(intptr_t param);          // 0x4f5bd0 "Own_item"
	void OnExchangeItemInit(intptr_t param);     // 0x4f5c60 "Exchange_item"

	// FRCMD_OWNERDRAW / FRCMD_LBUTTONUP handlers.
	void OnMyTPPointDraw(void);                  // 0x4f4df0 (the TP points +0x1a4 printed with the font +0x1dc)
	void OnHelpClick(void);                      // 0x4f68a0
	void OnMagicBoxClick(void);                  // 0x4f6900
	void OnCardComposeClick(void);               // 0x4f6960
	void OnExchangeClick(void);                  // 0x4f3610 (empty)
	void OnCookieItemClick(void);                // 0x4fb1a0
	void OnPangItemClick(void);                  // 0x4fb200
	void OnRareItemClick(void);                  // 0x4fb260
	void OnExchangeOnEffectDraw(void);           // 0x4f56a0
	void OnMPGaugeDraw(void);                    // 0x4f3620 (draws the gauge object +0x1d4)
	void OnExpectMPDraw(void);                   // 0x4f5950
	void OnPrePointDraw(void);                   // 0x4f5a40
	// 0x4f9a50: the selected items of the CAlchemyMPBreak (+0x188, 0x4f9980);
	// when the item 0x1a0002a6 is held more than 19999 times, a notice
	// (string 0x1870e, message 0x23 to the current task's main actor);
	// otherwise 0x476f10 and the confirmation 0x4f6740.
	void OnExchangeOkClick(void);
	void OnOwnItemDraw(intptr_t param);          // 0x4f9500
	void OnOwnItemClick(void);                   // 0x4f9f60
	void OnExchangeItemDraw(intptr_t param);     // 0x4f95c0
	void OnExchangeItemClick(void);              // 0x4f9ba0

	// An owned type id of the item tab (the JP +0x110 list's 0x18-byte
	// element; the "Own_item" rows' data): the count still owned and the
	// owned item ids with their counts (the first id is the one selected).
	struct OwnItem {
		unsigned int typeId;               // +0x0 (the "Exchange_item" rows' data starts with a type id too)
		unsigned int count;                // +0x4 (compared unsigned)
		std::map<int, int> itemCounts;     // +0x8 (VS 2010 map: head +0xc, size +0x10)
	};
	// 0x4f4cc0: draws `bmp` (when the GDI and the bitmap exist) over
	// `rect`, at the bitmap's size when `bitmapSize`, text style 0 first.
	void DrawBitmap(const Bitmap* bmp, WRect rect, uint32_t color, bool bitmapSize);
	// 0x4f74d0: the owned entry of a type id, NULL when none.
	OwnItem* FindOwnItem(unsigned int typeId);
	// 0x4f7500: the owned count of a row's entry (0 without one or
	// without item ids).
	unsigned int GetOwnItemCount(FrListItem* item);
	// 0x4f7530: whether an owned record (sItemInfo for a part, club set,
	// ball, item or kind 0x1c; sCards for a card) can be broken: its
	// record has an MP value (0xa19530) and it is not worn, not carrying a
	// period card, not the equipped club set or ball, and some are left
	// past the equipped copies.
	bool IsBreakable(uint32_t typeId, const void* record);
	// 0x4f77b0: the selected count of a type id's first item id, 0 when
	// not selected.
	int GetSelectedCount(unsigned int typeId);
	// 0x4f8b10: draws an item row (a part's character base, the icon, the
	// name cut to `column` characters, "x<count>", the MP) at the row's
	// height.
	void DrawItemRow(FrListItem* item, float height, int column, int count);
	// 0x4f9ed0: opens "MPBreakCountSetDlg" (FrItemCountController, its
	// factory 0x4f6020) into m_pCountSetDlg for the row (SetItem 0x4f64c0
	// with GetOwnItemCount).
	void OpenCountSetDlg(FrListItem* item);
	// 0x4f9d50: the count dialog's result handler.
	bool OnCountSetResult(int result, FrForm* form);
	// 0x4fab30: adds `count` to the entry of `typeId` (a new one at the
	// front or the back of the list when none) and, with a non-zero item
	// id, records the count under it; the entry's count.
	unsigned int AddOwnItem(unsigned int typeId, int itemId, unsigned int count, bool atFront);

	// 0x4f6740: opens FrAlchemyMPBreakNoticeDlg "AlchemyMPBreakNoticeDlg"
	// (factory 0x4f60f0) into m_pNotice, fills its lines and opens it with
	// the result handler 0x4f6680 and 0x11.
	void OpenBreakNotice(void);
	// 0x4f6680: on result 1, with a warning the warnings are cleared and the
	// notice opens again; without one the exchange is sent (packet 0x18d)
	// and message 1 goes to the current task's main actor.
	bool OnBreakNoticeResult(int result, FrForm* form);
	// 0x4f6850: "TikiTalk_Edit" shows m_tikiTalk[m_tikiTalkIndex].
	void ShowTikiTalk(void);
	// 0x4f8210: a text id of [firstId, lastId) picked at
	// random (537 bytes, a std::vector of the table's ids).
	int PickTextId(int firstId, int lastId);

	// 0x4faca0: rebuilds the owned-item list (JP +0x110) of
	// the current item tab from the inventory (0x633000, 0x4f7530, 0x4fab30).
	void CollectOwnItems(void);
	// 0x4f7820: fills "Own_item" from that list (0x4f77b0
	// per entry, FrListBox::AddItem 0xbf0b60).
	void FillOwnItemList(void);
	// 0x4f4b60: the gauge bars (+0x1d0..+0x1d8, "gaugebar_mile.tga" /
	// "gaugebar_respect.tga" / "gaugebar_full.tga" over
	// "gaugebar_nothing.tga") at the "MPGauge" area, targets the expected MP.
	void InitGauges(void);
	// 0x4f34a0: the gauge state (JP +0x124) per frame.
	void UpdateGaugeState(float dt);
	// 0x4f35b0: JpMaskGauge::Update of the three gauges.
	void UpdateGauges(float dt);
	// 0x4f7d50: step 1 of the exchange sequence (the break effect).
	void StartBreakSequence(void);
	// 0x4f7c20: step 6 of the exchange sequence (the result notice).
	void FinishBreakSequence(void);
	// 0x4fb2c0: CAlchemyMPBreak::SetState, then the controls of state 3
	// shown (the others hidden), the lists cleared and refilled.
	void SetViewState(int state);
	// 0x4f8650: the four Tiki talk texts, each picked from its id range.
	void UpdateTexts(void);
	// 0x4f8730: the Tiki talk text index, UpdateTexts, ShowTikiTalk.
	void UpdateView(int tikiTalkIndex);
	// 0x4f99b0: in state 3, "Exchange_ok" is enabled while the selection
	// is not empty.
	void UpdateExchangeOk(void);
	// 0x4f4a70: the effect +0x258 gets g_resrcmng (0xf023f0), g_camera
	// (0xf02430), 33.3 ms frames and its transform (+0x698..+0x6c4).
	void InitEffect(void);

private:
	std::list<OwnItem> m_ownItems;  // JP +0x110 (head +0x110, size +0x114, 0x20-byte nodes)
	// JP +0x11c: set when an item went into "Exchange_item" (only written).
	bool m_exchangeAdded;
	FrItemCountController* m_pCountSetDlg;   // JP +0x1c4 ("MPBreakCountSetDlg")
	// JP +0x128..+0x14f, +0x164..+0x17f: zeroed control pointers.
	FrArea* m_pExchangeListBG[4];   // JP +0x130
	FrArea* m_pMyTPPoint;           // JP +0x140
	FrArea* m_pMPGauge;             // JP +0x144
	FrArea* m_pExpectMP;            // JP +0x148
	FrArea* m_pPrePoint;            // JP +0x14c
	FrButton* m_pHelp;              // JP +0x154
	FrButton* m_pExit;              // JP +0x158
	FrButton* m_pMagicBox;          // JP +0x15c
	FrButton* m_pCardCompose;       // JP +0x160
	FrButton* m_pExchange;          // JP +0x164
	FrButton* m_pExchangeOk;        // JP +0x168
	FrListBox* m_pOwnItem;          // JP +0x16c
	FrListBox* m_pExchangeItem;     // JP +0x170
	FrButton* m_pCookieItem;        // JP +0x174
	FrButton* m_pPangItem;          // JP +0x178
	FrButton* m_pRareItem;          // JP +0x17c
	FrEdit* m_pTikiTalkEdit;        // JP +0x180
	FrArea* m_pExchangeOnEffect;    // JP +0x184
	const Bitmap* m_pListButtonLeft;  // JP +0x128 "list_button_o_left"
	const Bitmap* m_pListButtonRight; // JP +0x12c "list_button_o_right"
	CAlchemyMPBreak m_breaker;      // JP +0x188 (packet 0x18d)
	int m_viewState;                // JP +0x1c0
	// JP +0x1a4: the TP points shown by My_TP_Point.
	FrAlchemyMPBreakNoticeDlg* m_pNotice;   // JP +0x1c8
	int m_itemTab;                  // JP +0x1cc: 1 Pang_Item, 2 Cookie_Item, 3 Rare_Item
	JpMaskGauge* m_pMileGauge;      // JP +0x1d0 ("gaugebar_mile.tga")
	JpMaskGauge* m_pRespectGauge;   // JP +0x1d4 ("gaugebar_respect.tga")
	JpMaskGauge* m_pFullGauge;      // JP +0x1d8 ("gaugebar_full.tga")
	std::string m_tikiTalk[4];      // JP +0x1e4 (ids 0x186a1.., 0x186ab.., 0x186b5.., 0x186bf..)
	int m_tikiTalkIndex;            // JP +0x254
	int m_procState;                // JP +0x120 (OnProc's exchange sequence, 0 idle)
	int m_gaugeState;               // JP +0x124 (0x4f34a0's state, 0 idle)
	CPvsFxBox m_effect;             // JP +0x258 (JP size 0x43c)
	WMatrix m_effectCamera;         // JP +0x698 (the camera OnExchangeOnEffectDraw draws the effect from)
	// JP +0x1dc / +0x1e0: the point fonts the owner-draws print with (the
	// constructor `new`s the JP-only WTitleFontEx subclasses 0x6f5600
	// "number_tp.tga" and 0x6f56c0 "number_mi.tga", JpAlchemyDlg.cpp).
	WTitleFontEx* m_pPointFont;     // JP +0x1dc
	WTitleFontEx* m_pSmallFont;     // JP +0x1e0
};

// The JP service flags are 64-bit (CSharedDoc +0x10d28/+0x10d2c); the KR
// CSharedDoc::IsControlServerService takes and keeps 32 bits
// (m_controlServerService 0x4D68), so the high-dword services have no KR
// accessor.
// 0x62ba40: the JP CSharedDoc's IsControlServerService(service): the flags
// AND service, as 64 bits. Defined in JpLobbyGlue.cpp (low 32 bits = KR
// m_controlServerService; the port has no high dword).
uint64_t SharedDoc_IsControlServerService64(CSharedDoc* doc, uint64_t service);

// 0xb2e7e0: the "service closed" notice: the current task's main actor gets
// MsgObject(0, 0x2EE, high dword, low dword) of the service.
void Jp_NotifyServerServiceClosed(uint64_t service);

// 0x655d70: opens the alchemy dialog (see JpAlchemyDlg.cpp).
void TaskMain_CreateAlchemyDlg(CTaskMain* task);

// The JP CTaskMain +0x1a8 read (no function of its own in the JP exe): the
// open alchemy dialog, NULL when none (the card compose result handler
// 0x653ae0 tests it).
FrForm* TaskMain_GetAlchemyDlg(const CTaskMain* task);

// The JP CTaskMain::HandleMsgCommon cases the alchemy break replies post
// (TaskManager.cpp JP_PAKS: 0x2e5, 0x2e6, then 0x2e4): 0x65f259 (0x2e4,
// FrAlchemyMPBreakDlg::OnBreakResult on the open dialog), 0x65f27b (0x2e5,
// SetProcState(1)) and the JP CTaskMain member 0x651080 (0x2e6, ResetView,
// no NULL test).
void TaskMain_OnAlchemyBreakResult(CTaskMain* task, int mp, int bonus);
void TaskMain_StartAlchemyBreak(CTaskMain* task);
void TaskMain_ResetAlchemyDlg(CTaskMain* task);

#endif // PG_JP_JPALCHEMYDLG_H
