// JP-only game code (no KR counterpart): the room-state dispatcher of the
// JP lobby's room list forms (FrRoomListDlg +0x140, JpRoomListDlg.h). It
// forwards the room list's events to one handler per room kind (0
// multiplayer, 1 GP; 2 = "the shared doc's current room tab", read through
// 0x62bf50 and stored back into m_kind). Its functions (0x5a01c0..0x5a0348)
// tail-jump into the handler's vtable with their own arguments. Names ours:
// the class has no RTTI of its own (no vtable).
#ifndef PG_JP_JPROOMLISTSTATE_H
#define PG_JP_JPROOMLISTSTATE_H

#include <stdint.h>

#include <string>
#include <vector>

#include "defines.h"
#include "padding.h"

class FrListBox;
class FrRoomListDlg;
struct IContentsDataContainer;
namespace lua_system {
class lua_script;
}
struct sRoomInfo;

// A per-kind handler (vtable 0xd0c55c, ctor 0x598140, dtor 0x5981d0; its
// RTTI is not traced, names ours). The base's slots 1..7 are empty bodies
// (0x598150..0x5981b0, `ret 4` / `ret 8`); both derived handlers override
// every slot. Slots 2 and 3 are not called by the dispatcher: they take
// the room list and a room record (the multiplayer handler's add one room
// that passes its filter / remove it; callers not traced).
class JpRoomListHandler {
public:
	JpRoomListHandler(void) {}                                        // 0x598140
	// 0x5981d0: releases the handlers' script (s_script).
	virtual ~JpRoomListHandler(void);                                 // [0] 0x598870 (0x5981d0)
	virtual void Attach(FrRoomListDlg* dlg) { (void)dlg; }            // [1] 0x598150
	virtual void AddRoom(FrListBox* list, const sRoomInfo* room) { (void)list; (void)room; }    // [2] 0x598160
	virtual void RemoveRoom(FrListBox* list, const sRoomInfo* room) { (void)list; (void)room; } // [3] 0x598170
	virtual void OwnerDraw(intptr_t param, FrRoomListDlg* dlg) { (void)param; (void)dlg; } // [4] 0x598180
	virtual void LBtnDown(FrRoomListDlg* dlg) { (void)dlg; }          // [5] 0x598190
	virtual void RBtnUp(FrRoomListDlg* dlg) { (void)dlg; }            // [6] 0x5981a0
	virtual void DblClick(FrRoomListDlg* dlg) { (void)dlg; }          // [7] 0x5981b0

	// 0x598120 stores the lobby data container (contents type 0xa5) in
	// 0xe75e30 for the handlers.
	static void SetLobbyData(IContentsDataContainer* data);           // 0x598120
	static IContentsDataContainer* s_lobbyData;                       // 0xe75e30

	// 0x5980d0: releases s_script, creates a new one and runs file in it
	// (the GP handler's slot 8 0x599cd0 with "grandprix_lobby_ui.lua", and
	// JpRoomListState 0x5a06e0, neither ported yet).
	static void LoadScript(const char* file);
	// 0x598130: s_script.
	static lua_system::lua_script* GetScript(void);
	static lua_system::lua_script* s_script;                          // 0xe75e34
};

class WTitleFont;
class WTitleFontEx;

// Handler 0, the multiplayer room list (new 0x28, vtable 0xd0c5d0; JP RTTI
// name CNormalLobbyRoom, per the inventory). Attach is written; the other
// slots are declared only (not decompiled yet).
class JpMultiplayRoomListHandler : public JpRoomListHandler {
	LAYOUT_CHECK_FRIEND;
public:
	JpMultiplayRoomListHandler(void);                                 // 0x599700
	virtual ~JpMultiplayRoomListHandler(void);                        // [0] 0x599870 (0x5997d0)
	virtual void Attach(FrRoomListDlg* dlg);                          // [1] 0x59a250
	virtual void AddRoom(FrListBox* list, const sRoomInfo* room);     // [2] 0x5988a0
	virtual void RemoveRoom(FrListBox* list, const sRoomInfo* room);  // [3] 0x598200
	virtual void OwnerDraw(intptr_t param, FrRoomListDlg* dlg);       // [4] 0x59a600
	virtual void LBtnDown(FrRoomListDlg* dlg);                        // [5] 0x598910
	virtual void RBtnUp(FrRoomListDlg* dlg);                          // [6] 0x598e70
	virtual void DblClick(FrRoomListDlg* dlg);                        // [7] 0x59b980

	// The "[font_wind.jpg" number font (g_resrcmng's GetTitleFont, given
	// back by the destructor; OwnerDraw prints the room numbers with it).
	WTitleFont* m_numberFont;                  // 0x4
	// OwnerDraw's custom-map tooltip: its text and the list item (FrListItem
	// idx) it was shown for (-1 none, set by the ctor).
	std::string m_tooltipText;                 // 0x8
	int m_tooltipItem;                         // 0x24
};
BEGIN_LAYOUT_CHECK(JpMultiplayRoomListHandler)
	CHECK_FIELD(JpMultiplayRoomListHandler, m_numberFont, 0x4);
	CHECK_FIELD(JpMultiplayRoomListHandler, m_tooltipText, 0x8);
	CHECK_FIELD(JpMultiplayRoomListHandler, m_tooltipItem, 0x24);
	CHECK_SIZE(JpMultiplayRoomListHandler, 0x28);
END_LAYOUT_CHECK

// Handler 1, the Grand Prix room list (new 0x194, vtable 0xd0c584; JP RTTI
// name CGrandPrixLobbyRoom). Its list (FrRoomListDlg::m_pAltRoomList)
// holds the room-sort container's sJpGrandPrixEntry* (JpGrandPrixAwardDlg.h).
// OwnerDraw is declared only.
class JpGrandPrixRoomListHandler : public JpRoomListHandler {
	LAYOUT_CHECK_FRIEND;
public:
	JpGrandPrixRoomListHandler(void);                                 // 0x598990
	virtual ~JpGrandPrixRoomListHandler(void);                        // [0] 0x598a00
	virtual void Attach(FrRoomListDlg* dlg);                          // [1] 0x59c0c0
	virtual void AddRoom(FrListBox* list, const sRoomInfo* room);     // [2] 0x598f90
	virtual void RemoveRoom(FrListBox* list, const sRoomInfo* room);  // [3] 0x599000
	virtual void OwnerDraw(intptr_t param, FrRoomListDlg* dlg);       // [4] 0x59c710
	virtual void LBtnDown(FrRoomListDlg* dlg);                        // [5] 0x598a30
	virtual void RBtnUp(FrRoomListDlg* dlg);                          // [6] 0x59a390
	virtual void DblClick(FrRoomListDlg* dlg);                        // [7] 0x5990f0

	// 0x5998a0: with the handlers' script, reads the pairs of its
	// "grandprixuioffset" into m_uiOffsets and sets the list's item size
	// from [18] (sort 0x63: from [19], then the "gpeventlist" pairs over
	// m_uiOffsets).
	void LoadUiOffsets(FrListBox* list);

	// A pair of the lua tables.
	struct sUiOffset {
		float x;    // 0x0
		float y;    // 0x4
	};
	enum { UI_OFFSET_COUNT = 20 };
	// Zeroed by the constructor (a 0x90-byte memset, then four dwords);
	// LoadUiOffsets; the other readers (OwnerDraw 0x59c710) are not ported.
	sUiOffset m_uiOffsets[UI_OFFSET_COUNT];    // 0x4..0xa4
	UNKNOWN(0xa4, 0x188);
	// Attach (0x59c0c0) copies FrRoomListDlg's two lobby fonts here.
	WTitleFontEx* m_pFont188;                  // 0x188 (NULL by the ctor)
	WTitleFontEx* m_pFont18c;                  // 0x18c (NULL by the ctor)
	// Attach sets it to -1 (use not traced; the multiplayer handler's -1 is
	// its tooltip item).
	int m_dw190;                               // 0x190
};
BEGIN_LAYOUT_CHECK(JpGrandPrixRoomListHandler)
	CHECK_FIELD(JpGrandPrixRoomListHandler, m_uiOffsets, 0x4);
	CHECK_FIELD(JpGrandPrixRoomListHandler, m_pFont188, 0x188);
	CHECK_FIELD(JpGrandPrixRoomListHandler, m_dw190, 0x190);
	CHECK_SIZE(JpGrandPrixRoomListHandler, 0x194);
END_LAYOUT_CHECK

// CLobbyMain +0x778 (new 0x24 in CLobbyMain::OnPreLoadInit 0xab9720, deleted
// by ~CLobbyMain 0xaba7cf).
class JpRoomListState {
	LAYOUT_CHECK_FRIEND;
public:
	// m_kind = 2, the room list layout script (0x59e250), the handlers'
	// lobby data container (0x598120), then the two handlers.
	explicit JpRoomListState(IContentsDataContainer* lobbyData);      // 0x5a0780
	~JpRoomListState(void);                                           // 0x5a05e0
	void Attach(FrRoomListDlg* dlg);                                  // 0x5a01c0
	void OwnerDraw(intptr_t param, FrRoomListDlg* dlg);               // 0x5a0250
	void LBtnDown(FrRoomListDlg* dlg);                                // 0x5a0280
	void RBtnUp(FrRoomListDlg* dlg);                                  // 0x5a02b0
	void DblClick(FrRoomListDlg* dlg);                                // 0x5a02e0
	// Stores the sort in the room-sort container (0xaf, 0x559700), then
	// Attach(dlg).
	void Sort(int sort, FrRoomListDlg* dlg);                          // 0x5a0310

	// 0 multiplayer, 1 GP, 2 = read the current room tab (0x62bf50).
	int m_kind;                                // 0x0
	// Indexed by m_kind: [0] multiplayer, [1] GP (`new`ed by the ctor,
	// deleted through vfn 0 by the dtor).
	std::vector<JpRoomListHandler*> m_handlers; // 0x4
	// Freed by the dtor; element type and use not traced (trivial elements).
	std::vector<uint32_t> m_vec14;             // 0x14
};
BEGIN_LAYOUT_CHECK(JpRoomListState)
	CHECK_FIELD(JpRoomListState, m_kind, 0x0);
	CHECK_FIELD(JpRoomListState, m_handlers, 0x4);
	CHECK_FIELD(JpRoomListState, m_vec14, 0x14);
	CHECK_SIZE(JpRoomListState, 0x24);
END_LAYOUT_CHECK

// JP game state, declared only (thiscall on the shared doc 0xe47dbc; names
// ours): the room-list sort (0x62bf20 stores, 0x62bf30 reads) and the
// current room tab (0x62bf40 stores, 0x62bf50 reads: 1 GP, 0 multiplayer).
void JpSharedDoc_SetRoomSort(int sort);
int JpSharedDoc_GetRoomSort(void);
void JpSharedDoc_SetCurrentRoomTab(int tab);
int JpSharedDoc_GetCurrentRoomTab(void);
// Bit 0 of the JP room record's +0xd9 (a JP-only room field, not in the
// port's KR sRoomInfo; name ours). Port glue (JpRoomListState.cpp): false.
bool JpRoomInfo_HasSortFlag(const sRoomInfo* room);
// The other JP-only room record fields the multiplayer OwnerDraw reads (not
// in the port's KR sRoomInfo; names ours). Port glue (JpRoomListState.cpp):
// false / 0. +0xbf: the "gp_host" mark; +0xc5: the custom map item type id
// (0: none); +0xc9 bit 0: the classic room icon, bit 1: the short-game
// category text (0x59f190).
bool JpRoomInfo_IsGpHost(const sRoomInfo* room);
unsigned long JpRoomInfo_GetCustomMapId(const sRoomInfo* room);
bool JpRoomInfo_IsClassicRoom(const sRoomInfo* room);
bool JpRoomInfo_IsShortGame(const sRoomInfo* room);
// 0x59f190 (fastcall, name ours): the room list category text of a room
// (Jp_LookupText; NULL for game types 3, 8 and above 11).
const char* JpRoomInfo_GetCategoryText(const sRoomInfo* room);

// The room list row layout 0x59dbe0 ("roomlist_layout_ui.lua", names ours)
// fills: 17 element offsets from the row's top left (0xe75e38, table
// roomlist_layout_ui_offset), the 3 background pieces (0xe75e48, table
// roomlist_layout_ui_base_frame) and 6 row colours (0xe75e58, table
// roomlist_layout_ui_color, "%x" strings; [0] default, [1] empty row, [2]
// [3] category 1 (mode 0x12 / other), [4] category 4, [5] the course image
// of a room closed to intrusion).
struct JpRoomListOffset {
	float x;                                   // 0x0
	float y;                                   // 0x4
};
CHECK_SIZE(JpRoomListOffset, 0x8);
struct JpRoomListFrame {
	float x;                                   // 0x0
	float y;                                   // 0x4
	float w;                                   // 0x8
	float h;                                   // 0xc
};
CHECK_SIZE(JpRoomListFrame, 0x10);
extern std::vector<JpRoomListOffset> g_roomListOffsets;  // 0xe75e38
extern std::vector<JpRoomListFrame> g_roomListFrames;    // 0xe75e48
extern std::vector<uint32_t> g_roomListColors;           // 0xe75e58
// 0x59dbe0 (0x59e250 jumps to it).
void JpRoomListLayout_Load(void);

#endif
