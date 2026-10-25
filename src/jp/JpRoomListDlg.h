// JP-only game code (no KR counterpart): FrRoomListDlg, the lobby's room
// list form. Its Fresh layout has the room list per category (cate_vs_list,
// cate_mass_list, cate_battle_list, picked by the btn_cate_* buttons), the
// sortable column headers (room_no, room_hole, room_course, room_state), the
// lobby user list (userlist, columns user_gender/level/guild/nick), the
// make-room, equipped-item and quest buttons and the GP / multiplayer /
// avatar-chat tabs. It is the base of FrMultiplayRoomDlg (ctor 0x5175f0) and
// FrGrandPrixRoomDlg (ctor 0x50ff90). Nothing opens it by address: a Fresh
// layout creates it by class name through ObjectFactory ("FrRoomListDlg",
// registered at static init 0xccd1e0, make instance 0x51ecc0).
//
// JP x86 facts: RTTI 0xe74f58 "FrRoomListDlg" (static init 0xccd1c0), base
// FrChatDlg (RTTI 0xe73e98, ctor 0x48f0b0, dtor 0x48f120, size 0x140;
// JpChatDlg.h). Vtables 0xd07afc (38 slots: [0] GetRTTI, [1] the
// deleting destructor, [9] OnProc, [32] OnInit; the rest FrChatDlg's),
// 0xd07af0 (FrCmdTarget: [0] deleting-destructor thunk 0x51d470, [1]
// GetMessageMap 0x51c9e0), 0xd07ae8 ([0] thunk 0x51d460). Message map
// 0xe09fdc (base 0xe04210, FrChatDlg's; 60 rows at 0xe09fe8, stride 0x20,
// handlers stored by the static init 0xccd200). Size 0x300.
#ifndef PG_JP_JPROOMLISTDLG_H
#define PG_JP_JPROOMLISTDLG_H

#include <stdint.h>

#include "defines.h"
#include "padding.h"
#include "frcmdtarget.h"
#include "frform.h"
#include "JpChatDlg.h"
#include "wtypes.h"

class Bitmap;
class FrButton;
class JpRoomListState;
class WTitleFontEx;
class FrWnd;
class FrListBox;
struct FrListItem;
class FrViewer;
struct IContentsDataContainer;
struct sRoomInfo;

class FrRoomListDlg : public FrChatDlg {
	LAYOUT_CHECK_FRIEND;
	// The room list handlers fill and draw m_pRoomList (JpRoomListState.h).
	friend class JpMultiplayRoomListHandler;
	friend class JpGrandPrixRoomListHandler;
public:
	static const WRTTI m_RTTI;                                        // 0xe74f58
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x51ca10

	FrRoomListDlg(void);                                              // 0x51d510
	virtual ~FrRoomListDlg(void);                                     // [1] 0x51d680 (deleting), 0x51c9f0

	// Calls FrChatDlg's OnProc (0x490430) and nothing else.
	virtual void OnProc(const float dt);                              // [9] 0x51ca20
	virtual bool OnInit(void);                                        // [32] 0x51ed50

	// Stores the lobby's room-list state (CLobbyMain +0x778) in
	// m_pRoomState; the lobby calls it before Open (JP 0xabcb19).
	void SetRoomState(JpRoomListState* state);                        // 0x51ce40
	// Enables or disables the room list, make-room, equip and quest
	// controls (+0x250, +0x28c, +0x284, +0x288) and the +0x290 object
	// (jp/pairing.tsv pairs it with the KR
	// CLobbyMain::EnableRoomEnterControls it replaces).
	void EnableRoomEnterControls(bool enable);                        // 0x51ed90

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe09fe8

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe09fdc
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // FrCmdTarget [1] 0x51c9e0

	// Message-map handlers, in row order. INIT handlers keep the control;
	// the column INIT handlers also clear the column's sort flag.
	void OnRoomNoInit(intptr_t param);                                // 0x51d950
	void OnRoomNoLBtnDown(void);                                      // 0x521510
	void OnRoomNoOwnerDraw(intptr_t param);                           // 0x51d9d0
	void OnCateTotalInit(intptr_t param);                             // 0x51da60
	void OnCateTotalLBtnUp(void);                                     // 0x51ffa0
	void OnCateVsInit(intptr_t param);                                // 0x51dae0
	void OnCateVsLBtnUp(void);                                        // 0x51ffd0
	void OnCateMassInit(intptr_t param);                              // 0x51db60
	void OnCateMassLBtnUp(void);                                      // 0x520000
	void OnCateBattleInit(intptr_t param);                            // 0x51dbe0
	void OnCateBattleLBtnUp(void);                                    // 0x520030
	void OnCateChatInit(intptr_t param);                              // 0x51dc60
	void OnCateChatLBtnUp(void);                                      // 0x520060
	void OnRoomHoleInit(intptr_t param);                              // 0x51dce0
	void OnRoomHoleLBtnDown(void);                                    // 0x521590
	void OnRoomHoleOwnerDraw(intptr_t param);                         // 0x51dd60
	void OnRoomCourseInit(intptr_t param);                            // 0x51ddf0
	void OnRoomCourseLBtnDown(void);                                  // 0x521610
	void OnRoomCourseOwnerDraw(intptr_t param);                       // 0x51de70
	void OnRoomStateInit(intptr_t param);                             // 0x51df00
	void OnRoomStateLBtnDown(void);                                   // 0x521690
	void OnRoomStateOwnerDraw(intptr_t param);                        // 0x51df80
	void OnVsListInit(intptr_t param);                                // 0x51e020
	void OnVsListLBtnDown(void);                                      // 0x520090
	void OnVsListOwnerDraw(intptr_t param);                           // 0x51eec0
	void OnMassListInit(intptr_t param);                              // 0x51e0d0
	void OnMassListLBtnDown(void);                                    // 0x520170
	void OnMassListOwnerDraw(intptr_t param);                         // 0x51f020
	void OnBattleListInit(intptr_t param);                            // 0x51e180
	void OnBattleListLBtnDown(void);                                  // 0x520250
	void OnBattleListOwnerDraw(intptr_t param);                       // 0x51f180
	void OnMakeRoomInit(intptr_t param);                              // 0x51e230
	void OnMakeRoomLBtnUp(void);                                      // 0x51f2e0
	void OnCloseLBtnUp(void);                                         // 0x51d0d0
	void OnCurEquipItemInit(intptr_t param);                          // 0x51e2a0
	void OnCurEquipItemLBtnUp(void);                                  // 0x51f490
	void OnQuestInit(intptr_t param);                                 // 0x51e310
	void OnQuestLBtnUp(void);                                         // 0x51f4f0
	void OnUserListInit(intptr_t param);                              // 0x520340
	void OnUserListLBtnDown(void);                                    // 0x51e380
	void OnUserListRBtnUp(void);                                      // 0x520c40
	void OnUserListOwnerDraw(intptr_t param);                         // 0x51f5a0
	void OnUserGenderInit(intptr_t param);                            // 0x51e3d0
	void OnUserGenderLBtnDown(void);                                  // 0x51d0e0
	void OnUserGenderOwnerDraw(intptr_t param);                       // 0x51e450
	void OnUserLevelInit(intptr_t param);                             // 0x51e570
	void OnUserLevelLBtnDown(void);                                   // 0x51d160
	void OnUserLevelOwnerDraw(intptr_t param);                        // 0x51e5f0
	void OnUserGuildInit(intptr_t param);                             // 0x51e710
	void OnUserGuildLBtnDown(void);                                   // 0x51d1e0
	void OnUserGuildOwnerDraw(intptr_t param);                        // 0x51e790
	void OnUserNickInit(intptr_t param);                              // 0x51e8b0
	void OnUserNickLBtnDown(void);                                    // 0x51d260
	void OnUserNickOwnerDraw(intptr_t param);                         // 0x51e930
	void OnGpRoomTabInit(intptr_t param);                             // 0x51fa10
	void OnGpRoomTabLBtnUp(void);                                     // 0x51fa90
	void OnMtRoomTabInit(intptr_t param);                             // 0x51ea50
	void OnMtRoomTabLBtnUp(void);                                     // 0x51fad0
	void OnAvatarChatTabInit(intptr_t param);                         // 0x51eac0
	void OnAvatarChatTabLBtnUp(void);                                 // 0x51d2e0

	// Sets each category button PRESSED when it is the shown category
	// (name ours).
	void UpdateCategoryButtons(void);                                 // 0x51cc90
	// Shows the GP room tab when the shared doc allows it (name ours).
	// Shows the avatar-chat tab, and hides the multiplayer tab, when
	// 0xb2ea00 returns 0x10 (name ours).
	void UpdateAvatarChatTab(void);                                   // 0x51ce80
	// Disables the category button of one channel-type bit (2 vs, 4 mass,
	// 8 battle, 0x10 chat; name ours).
	void DisableCategoryButton(int bit);                              // 0x51cee0
	void UpdateGpRoomTab(void);                                       // 0x51ee90
	// Refills m_pRoomList from the shared doc's room list: mode 1 sets
	// the category and lists its rooms, mode 3 the selection and lists
	// the rooms of that game type; pads to six rows (name ours).
	void FillRoomList(int mode, int value);                           // 0x51fd30
	// Stores the lobby's title font and two more lobby fonts (the lobby
	// switch 0xabca60 passes the lobby's +0x794, +0x784, +0x79c, all
	// WTitleFontEx*; name ours).
	void SetLobbyValues(WTitleFontEx* titleFont, WTitleFontEx* font158, WTitleFontEx* font15c); // 0x51ce50
	// Clears the room sort flags and hides the three category lists; the
	// column argument is unused (name ours).
	void HideCategoryLists(int column);                               // 0x51cdf0
	// Sorts m_pRoomList by column (0 room_no, 4 hole, 5 course, 6 state,
	// stored in m_value2ec; 1 does nothing); the second argument is unused
	// (name ours).
	void SortRoomList(int column, int arg);                           // 0x521290
	// Sorts m_pUserList by column (0 gender, 1 level, 2 guild, 3 nick,
	// stored in m_value2f0), descending when m_flag2ea is set (name ours).
	void SortUserList(int column);                                    // 0x51cd20
	// Loads the user list's images (run by FrGrandPrixRoomDlg's
	// InitGpRoomState and at the end of ResetRoomListImages; name ours).
	void LoadUserListImages(void);                                    // 0x51ca40
	// Loads the room list's images, then LoadUserListImages (run by
	// FrMultiplayRoomDlg's ResetRoomState; name ours).
	void ResetRoomListImages(void);                                   // 0x51d6b0
	// The per-frame update, called by both derived OnProc (not decompiled,
	// name ours).
	void UpdateRoomList(float dt);                                    // 0x520520
	// Enables the category buttons the channel type (0xb2ea00) allows and
	// shows its category (also called by the lobby switch 0xabc168; name
	// ours).
	void ReloadRoomList(void);                                        // 0x520900
	// Pads room list 0 (m_pRoomList) or 1 (m_pAltRoomList) to six rows,
	// then re-sorts it (FrMultiplayRoomDlg::OnInit passes 0; name ours).
	void PadRoomList(int which);                                      // 0x521470

	// The room-state dispatcher (JpRoomListState.h): the derived OnInit
	// set its kind and the room list's mouse handlers forward to it.
	JpRoomListState* m_pRoomState;             // 0x140 (NULL by the constructor)
	// Contents container 0xa5 (JP-only type): +0x4 is reset to -1 by the
	// category buttons, +0x8 is the shown room category (0 vs, 1 mass,
	// 2 battle, 3 chat, 4 total).
	IContentsDataContainer* m_pRoomListData;   // 0x144
	// The JP room detail form: EnableRoomEnterControls(true) enables its
	// join button (0xace330), where the KR CLobbyMain enables
	// m_dtRoomDlg's (who sets it is not traced).
	FrWnd* m_pDtRoomDlg;                       // 0x148 (NULL by the constructor)
	int m_value14c;                            // 0x14c (zeroed by the constructor)
	// The picked room (the list item's data, a room record of the shared
	// doc's list): the multiplayer handler's LBtnDown sets it, its
	// DblClick joins it.
	const sRoomInfo* m_pSelectedRoom;          // 0x150 (NULL by the constructor)
	// The lobby's title font (FrGrandPrixRoomDlg's gpstate draws the
	// count with WTitleFontEx::Print 0x6f4600).
	WTitleFontEx* m_pTitleFont;                // 0x154 (NULL by the constructor)
	// The lobby's +0x784 and +0x79c fonts (SetLobbyValues; NULL by the
	// constructor); the GP room list handler's Attach copies them.
	WTitleFontEx* m_pFont158;                  // 0x158
	WTitleFontEx* m_pFont15c;                  // 0x15c
	// The room rows' images (ResetRoomListImages): [piece 01..03][state]
	// roomlistbox_n, _o, _n (again), _e.
	const Bitmap* m_roomRowImages[3][4];       // 0x160
	// Column header images, drawn by the OWNERDRAW handlers: [0] normal,
	// [1] under the cursor, [2] sorted.
	const Bitmap* m_roomNoImages[3];           // 0x190
	const Bitmap* m_roomCateImages[3];         // 0x19c room_sort_cate_n_down, _o, _d
	const Bitmap* m_roomHoleImages[3];         // 0x1a8
	const Bitmap* m_roomCourseImages[3];       // 0x1b4
	// Two sets, picked by m_flag2e9.
	const Bitmap* m_roomStateImages[2][3];     // 0x1c0
	// LoadUserListImages: the user list's gender icons, [pair][0 male,
	// 1 female]: i_male/i_female, _02, _03, _manner, _angel.
	const Bitmap* m_userGenderIcons[5][2];     // 0x1d8
	// The user column header images (user_sort_<column>_normal, _over,
	// _click): [0] normal, [1] under the cursor, [2] sorted.
	const Bitmap* m_userGenderImages[3];       // 0x200
	const Bitmap* m_userLevelImages[3];        // 0x20c
	const Bitmap* m_userGuildImages[3];        // 0x218
	const Bitmap* m_userNickImages[3];         // 0x224
	UNKNOWN(0x230, 0x23c);
	const Bitmap* m_pInGameImage;              // 0x23c ingame (NULL by the constructor)
	// The sorted user column's arrow: [0] user_sort_up (m_flag2ea set),
	// [1] user_sort_down.
	const Bitmap* m_userSortArrowImages[2];    // 0x240
	const Bitmap* m_pAiPlayerImage;            // 0x248 i_aiplayer
	FrListBox* m_pUserList;                    // 0x24c userlist
	// The shown category's room list (FillRoomList refills it).
	FrListBox* m_pRoomList;                    // 0x250
	FrButton* m_pCateTotalButton;              // 0x254 btn_cate_total
	FrButton* m_pCateVsButton;                 // 0x258 btn_cate_vs
	FrButton* m_pCateMassButton;               // 0x25c btn_cate_mass
	FrButton* m_pCateBattleButton;             // 0x260 btn_cate_battle
	FrButton* m_pCateChatButton;               // 0x264 btn_cate_chat
	FrListBox* m_pVsList;                      // 0x268 cate_vs_list
	FrListBox* m_pMassList;                    // 0x26c cate_mass_list
	FrListBox* m_pBattleList;                  // 0x270 cate_battle_list
	FrViewer* m_pRoomNoColumn;                 // 0x274 room_no
	FrViewer* m_pRoomHoleColumn;               // 0x278 room_hole
	FrViewer* m_pRoomCourseColumn;             // 0x27c room_course
	FrViewer* m_pRoomStateColumn;              // 0x280 room_state
	FrButton* m_pCurEquipItemButton;           // 0x284 cur_equip_item
	FrButton* m_pQuestButton;                  // 0x288 quest_btn
	FrButton* m_pMakeRoomButton;               // 0x28c makeroom
	// Disabled by EnableRoomEnterControls whatever its argument, and for
	// identity 0xe: the place of the KR CLobbyMain::m_guildInfoBtn (who
	// sets it is not traced).
	FrWnd* m_pGuildInfoButton;                 // 0x290 (NULL by the constructor)
	// Enabled when a lobby user is picked in userlist.
	FrWnd* m_pUserSelectWnd;                   // 0x294 (NULL by the constructor)
	FrViewer* m_pUserGenderColumn;             // 0x298 user_gender
	FrViewer* m_pUserLevelColumn;              // 0x29c user_level
	FrViewer* m_pUserGuildColumn;              // 0x2a0 user_guild
	FrViewer* m_pUserNickColumn;               // 0x2a4 user_nick
	// FrGrandPrixRoomDlg's sort buttons: btn_sort_event, _rookie, _c1,
	// _c2, _c3 (sorts 0x63, 0, 1, 2, 3).
	FrButton* m_pSortButtons[5];               // 0x2a8
	// The second room list PadRoomList pads (which 1).
	FrListBox* m_pAltRoomList;                 // 0x2bc (NULL by the constructor)
	FrButton* m_pGpRoomTab;                    // 0x2c0 def_bg.tab_gproom
	FrButton* m_pMtRoomTab;                    // 0x2c4 def_bg.tab_mtroom
	FrButton* m_pAvatarChatTab;                // 0x2c8 def_bg.tab_avatarchat
	float m_value2cc;                          // 0x2cc (0.0f by the constructor; the room columns ignore clicks while > 0, a click sets 0.3)
	// The room columns' sort flags (room_no, room_hole, room_course,
	// room_state), cleared by their INIT handlers.
	bool m_roomSortFlag[4];                    // 0x2d0
	float m_value2d4;                          // 0x2d4 (0.0f by the constructor; the user columns ignore clicks while > 0, a click sets 0.3)
	float m_value2d8;                          // 0x2d8 (0.0f by the constructor, btn_cate_vs)
	float m_value2dc;                          // 0x2dc (0.0f by the constructor, btn_cate_mass)
	float m_value2e0;                          // 0x2e0 (0.0f by the constructor, btn_cate_battle)
	// The user columns' sort flags (user_gender, user_level, user_guild,
	// user_nick), cleared by their INIT handlers.
	bool m_userSortFlag[4];                    // 0x2e4
	// Set when a category list row is picked (the list LBUTTONDOWN handlers).
	bool m_listRowPicked;                      // 0x2e8
	// Toggled by a second click on the sorted room column: the sort order.
	bool m_flag2e9;                            // 0x2e9 (false by the constructor)
	bool m_flag2ea;                            // 0x2ea (false by the constructor)
	UNKNOWN(0x2eb, 0x2ec);
	int m_value2ec;                            // 0x2ec (zeroed by the constructor; the room sort column, SortRoomList)
	int m_value2f0;                            // 0x2f0 (3 by the constructor)
	int m_value2f4;                            // 0x2f4 (zeroed by the constructor)
	UNKNOWN(0x2f8, 0x300);
};
CHECK_SIZE(FrRoomListDlg, 0x300);
BEGIN_LAYOUT_CHECK(FrRoomListDlg)
	CHECK_FIELD(FrRoomListDlg, m_pRoomState, 0x140);
	CHECK_FIELD(FrRoomListDlg, m_pRoomListData, 0x144);
	CHECK_FIELD(FrRoomListDlg, m_pSortButtons, 0x2a8);
	CHECK_FIELD(FrRoomListDlg, m_pUserList, 0x24c);
	CHECK_FIELD(FrRoomListDlg, m_pCateTotalButton, 0x254);
	CHECK_FIELD(FrRoomListDlg, m_pVsList, 0x268);
	CHECK_FIELD(FrRoomListDlg, m_pRoomNoColumn, 0x274);
	CHECK_FIELD(FrRoomListDlg, m_pMakeRoomButton, 0x28c);
	CHECK_FIELD(FrRoomListDlg, m_pUserGenderColumn, 0x298);
	CHECK_FIELD(FrRoomListDlg, m_roomNoImages, 0x190);
	CHECK_FIELD(FrRoomListDlg, m_roomHoleImages, 0x1a8);
	CHECK_FIELD(FrRoomListDlg, m_roomCourseImages, 0x1b4);
	CHECK_FIELD(FrRoomListDlg, m_roomStateImages, 0x1c0);
	CHECK_FIELD(FrRoomListDlg, m_roomRowImages, 0x160);
	CHECK_FIELD(FrRoomListDlg, m_roomCateImages, 0x19c);
	CHECK_FIELD(FrRoomListDlg, m_userGenderIcons, 0x1d8);
	CHECK_FIELD(FrRoomListDlg, m_userGenderImages, 0x200);
	CHECK_FIELD(FrRoomListDlg, m_userNickImages, 0x224);
	CHECK_FIELD(FrRoomListDlg, m_pInGameImage, 0x23c);
	CHECK_FIELD(FrRoomListDlg, m_userSortArrowImages, 0x240);
	CHECK_FIELD(FrRoomListDlg, m_pAiPlayerImage, 0x248);
	CHECK_FIELD(FrRoomListDlg, m_pUserSelectWnd, 0x294);
	CHECK_FIELD(FrRoomListDlg, m_pAltRoomList, 0x2bc);
	CHECK_FIELD(FrRoomListDlg, m_pGpRoomTab, 0x2c0);
	CHECK_FIELD(FrRoomListDlg, m_roomSortFlag, 0x2d0);
	CHECK_FIELD(FrRoomListDlg, m_userSortFlag, 0x2e4);
	CHECK_FIELD(FrRoomListDlg, m_value2f0, 0x2f0);
END_LAYOUT_CHECK

// The user list's sort predicates (JP LobbyMain unit, file-local there,
// FrListBox::SortItem callbacks on the 0xc8-byte user records, which start
// like the KR sBriefUserInfo; names ours). Desc = a > b, used when m_flag2ea
// is set: gender = the charType bits (+0x2d & 0xe), level +0x20, guildUID
// +0x2e (unsigned), nick +0xa (lstrcmpiA).
bool JpUserList_CompareGenderDesc(const void* a, const void* b);  // 0x59f280
bool JpUserList_CompareGenderAsc(const void* a, const void* b);   // 0x59f2b0
bool JpUserList_CompareLevelDesc(const void* a, const void* b);   // 0x59f2e0
bool JpUserList_CompareLevelAsc(const void* a, const void* b);    // 0x59f300
bool JpUserList_CompareGuildDesc(const void* a, const void* b);   // 0x59f320
bool JpUserList_CompareGuildAsc(const void* a, const void* b);    // 0x59f340
bool JpUserList_CompareNickDesc(const void* a, const void* b);    // 0x59f360
bool JpUserList_CompareNickAsc(const void* a, const void* b);     // 0x59f390

// The room list's own sort (JpRoomListDlg.cpp; names ours): a predicate
// that keeps the empty rows (NULL data) away from the room predicates below,
// which read their records unchecked.
class JpRoomListGreater {
public:
	JpRoomListGreater(bool (*compare)(const void*, const void*));    // 0x51eba0
	bool operator()(FrListItem* const& a, FrListItem* const& b) const; // (inlined in 0x520df0)

	bool (*m_compare)(const void*, const void*);  // 0x0
};
CHECK_SIZE(JpRoomListGreater, 0x4);
void JpRoomList_SortItem(FrListBox* list, JpRoomListGreater greater);  // 0x521260

// The room list's sort predicates (JP LobbyMain, file-local there; names
// ours): SortRoomList passes the first of each pair when
// m_flag2e9 is set. Column 2's pair compares with lstrcmpiA.
bool JpRoomList_CompareNo(const void* a, const void* b);          // 0x59f510
bool JpRoomList_CompareNoRev(const void* a, const void* b);       // 0x59f580
bool JpRoomList_CompareColumn2(const void* a, const void* b);     // 0x59f220
bool JpRoomList_CompareColumn2Rev(const void* a, const void* b);  // 0x59f230
bool JpRoomList_CompareColumn3(const void* a, const void* b);     // 0x59f240
bool JpRoomList_CompareColumn3Rev(const void* a, const void* b);  // 0x59f260
bool JpRoomList_CompareHole(const void* a, const void* b);        // 0x59f5f0
bool JpRoomList_CompareHoleRev(const void* a, const void* b);     // 0x59f660
bool JpRoomList_CompareCourse(const void* a, const void* b);      // 0x59f6d0
bool JpRoomList_CompareCourseRev(const void* a, const void* b);   // 0x59f740
bool JpRoomList_CompareState(const void* a, const void* b);       // 0x59f7b0
bool JpRoomList_CompareStateRev(const void* a, const void* b);    // 0x59f8b0

// JP game state (port glue in JpRoomListDlg.cpp): the shown room category of the 0xa5
// container (its +0x8), and resetting its +0x4 to -1.
int JpRoomListData_GetCategory(const IContentsDataContainer* data);
void JpRoomListData_SetCategory(IContentsDataContainer* data, int category);
// The container's +0x4: the game type FillRoomList mode 3 shows (-1 none).
void JpRoomListData_SetSelection(IContentsDataContainer* data, int value);
int JpRoomListData_GetSelection(const IContentsDataContainer* data);
void JpRoomListData_ResetSelection(IContentsDataContainer* data);
// The container's +0xc: the picked lobby user's UID (OnUserListOwnerDraw
// highlights its row; name ours).
uint32_t JpRoomListData_GetSelectedUid(const IContentsDataContainer* data);
// Picking a lobby user: the user record's +0x4 goes to the container's +0xc,
// its +0x0 to +0x10 (the record is the userlist item data, 0xc8 bytes).
void JpRoomListData_SelectUser(IContentsDataContainer* data, const void* userRecord);
// Bit 11 of the shared doc (0xe47dbc) +0x10d3c: the GP room tab is shown.
bool JpSharedDoc_IsGpRoomTabShown(void);
// 0x62bf60 on the shared doc (0xe47dbc): stores its +0x11634, the room tab
// the lobby opens next (1 GP room, 0 multiplayer; name ours).
void JpSharedDoc_SetRoomTab(int tab);
// 0xb2ea00 (JP, unplaced, 635 bytes; name ours): 0x10 shows the
// avatar-chat tab.
int JpLobby_GetAvatarChatMode(void);

#endif
