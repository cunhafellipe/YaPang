// JP-only game code (no KR counterpart): FrItemSearchDlg, the lobby's item
// search dialog ("ItemSearchDlg" of the Fresh layout), which the SEARCH key
// of CTaskMain (0x65c580, and 0x65e900) opens while the task page shows
// "realmyroom" or "shopmain". A group combo (combo_SearchItemGroup: the six
// groups below), a word edit (edit_SearchWord, Enter searches), a search
// button (btn_Search) and an owner-drawn result list (list_Result). A search
// asks the main actor (message 0x2d3) to fill m_results; clicking a result
// sends it messages 0x2d6 then 0x2d7.
//
// Evidence: node scripts/disasm.ts --jp / jp_hlil.ts. RTTI m_RTTI 0xe75c7c
// (GetRTTI 0x5843b0; name string "FrItemSearchDlg" after the vtable), base
// the JP FrForm (ctor 0xbe5e20, dtor 0xbe5e80, size 0x110 like KR's).
// Vtables 0xd0b2cc (38 slots: [0] GetRTTI, [1] the deleting destructor,
// [32] OnInit; the rest FrForm's), 0xd0b2c0 / 0xd0b2b8 (the other bases'
// deleting-destructor thunks 0x584390 / 0x5843a0). Message map 0xe0cd58
// (rows 0xe0ce20, handlers stored by the initializer 0xccf200). Size 0x150
// (x86), allocated by the creator 0x654700 (KR CreateForm<T>) and by the
// factory 0x584510.
#ifndef PG_JP_JPITEMSEARCHDLG_H
#define PG_JP_JPITEMSEARCHDLG_H

#include <list>
#include <string>

#include "defines.h"
#include "padding.h"
#include "frcmdtarget.h"
#include "frform.h"
#include "wtypes.h"

class Bitmap;
class CTaskMain;
class FrButton;
class FrComboBox;
class FrEdit;
class FrListBox;

// One search result, an element of FrItemSearchDlg::m_results and the pData
// of each list_Result item (list node 0x4c: value 0x44, x86). Filled by the
// main actor's handler of message 0x2d3 (JP, not decompiled).
struct sItemSearchResult {
	unsigned int typeId;        // 0x0 (icon drawn by 0x583a40; messages 0x2d6/0x2d7 param1)
	unsigned int value4;        // 0x4 (messages 0x2d6/0x2d7 param2)
	UNKNOWN(0x8, 0x24);
	std::string name;           // 0x24 (printed by the owner draw)
	unsigned int linkedTypeId;  // 0x40 (a second item, drawn when non-zero; message 0x2d6 param3)
};
CHECK_SIZE(sItemSearchResult, 0x44);
CHECK_FIELD(sItemSearchResult, typeId, 0x0);
CHECK_FIELD(sItemSearchResult, value4, 0x4);
CHECK_FIELD(sItemSearchResult, name, 0x24);
CHECK_FIELD(sItemSearchResult, linkedTypeId, 0x40);

class FrItemSearchDlg : public FrForm {
	LAYOUT_CHECK_FRIEND;
public:
	static const WRTTI m_RTTI;                                        // 0xe75c7c
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x5843b0

	FrItemSearchDlg(void);                                            // 0x584250
	virtual ~FrItemSearchDlg(void);                                   // [1] 0x5845a0 (deleting), 0x5843c0

	virtual bool OnInit(void);                                        // [32] 0x583630

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe0ce20

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe0cd58
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0x5832d0

	// Message-map handlers.
	void OnGroupComboInit(intptr_t param);                           // 0x5833a0
	void OnSearchEditInit(intptr_t param);                           // 0x583410
	void OnSearchButtonInit(intptr_t param);                         // 0x583480
	void OnResultListInit(intptr_t param);                           // 0x5834f0
	void OnResultListLBtnUp(void);                                    // 0x583810
	// Draws a result row: the selection base under the cursor, the item's
	// icon and name, and the linked item's when it has one.
	void OnResultListOwnerDraw(intptr_t param);                      // 0x583e40
	void OnGroupComboLBtnDown(void);                                  // 0x584410 (jmp ClearResults)
	void OnSearchButtonLBtnUp(void);                                  // 0x5845d0
	void OnSearchEditEnterKey(intptr_t param);                       // 0x5846b0 (calls OnSearchButtonLBtnUp)

	// Messages 0x2d6 (typeId, value4, linkedTypeId) and 0x2d7 (typeId,
	// value4) to the current task's main actor.
	void SendResultSelected(const sItemSearchResult* pResult);        // 0x5836c0
	void SendResultChosen(unsigned int typeId, unsigned int value4);  // 0x583730
	void AddResults(std::list<sItemSearchResult>& results);           // 0x583a00
	// Draws an item's icon at pos: its character's base image (by typeId >>
	// 26) into m_iconRect, then its icon into m_linkedIconRect, both scaled
	// to the list's item width.
	void DrawItemIcon(unsigned int typeId, const WPoint* pos);        // 0x583a40
	void ClearResults(void);                                          // 0x584190
	void Search(std::string& word, int group);                        // 0x584420

	FrComboBox* m_pGroupCombo;                 // 0x110 combo_SearchItemGroup
	FrEdit* m_pSearchEdit;                     // 0x114 edit_SearchWord
	FrButton* m_pSearchButton;                 // 0x118 btn_Search
	FrListBox* m_pResultList;                  // 0x11c list_Result
	std::list<sItemSearchResult> m_results;    // 0x120 (head 0x120, size 0x124)
	UNKNOWN(0x128, 0x12c);
	// Reset to the JP global rect 0xf145f8 by ClearResults; the owner draw
	// adds their +0x8 (width) to the text x.
	WRect m_iconRect;                          // 0x12c
	WRect m_linkedIconRect;                    // 0x13c
	const Bitmap* m_pSelectedBase;             // 0x14c "30_noplayer_base" (OnInit)
};
CHECK_SIZE(FrItemSearchDlg, 0x150);

// The JP CTaskMain's item search members (TaskMain.h, JP_PAKS) are defined
// in JpItemSearchDlg.cpp: m_itemSearchDlg (JP +0x1ac), OpenItemSearchDlg
// 0x655d10 (creates "ItemSearchDlg" once and opens it, flag 1, with the
// result callback 0x651060, OnItemSearchDlgResult: clears +0x1ac, returns
// true; reached through FrCmdTarget at +0x18).

#endif
