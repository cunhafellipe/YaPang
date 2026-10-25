// JP-only game code (no KR counterpart): FrAvatarMotionDlg, the lobby's
// avatar motion chat dialog ("motiondlg" of the Fresh layout, script
// "motiondlg.lua"), opened beside the lobby avatar (its button images are
// "motionchat_btn_on_*"). One owner-drawn list (motionlist) shows the
// motions of the categories the script fills; OnInit asks the "LobbyAvatar"
// actor (message 0x197) for its state.
//
// Evidence: node scripts/disasm.ts --jp / jp_xrefs.ts. RTTI m_RTTI 0xe73e18
// (initializer 0xcc7c80, base FrForm's 0xf1aabc; GetRTTI 0x481ed0), base the
// JP FrForm (ctor 0xbe5e20, dtor 0xbe5e80). Vtables 0xcfecc4 (38 slots: [0]
// GetRTTI, [1] the deleting destructor 0x482250, [9] OnProc 0x480450, [32]
// OnInit 0x4832a0; the rest FrForm's), 0xcfecb8 (FrCmdTarget: [0] thunk
// 0x481ee0, [1] GetMessageMap 0x47f2a0), 0xcfecb0 (thunk 0x481ef0). Message
// map 0xe037c8 (rows 0xe037d0, handlers stored by the initializer 0xcc7cc0).
// Size 0x1f0 (x86), allocated by the factory 0x4821c0 and by the creator
// 0x7b1130 (KR CreateForm<T>), which 0x7b1860 and 0xab8350 call.
#ifndef PG_JP_JPAVATARMOTIONDLG_H
#define PG_JP_JPAVATARMOTIONDLG_H

#include <stdint.h>

#include <string>
#include <vector>

#include "defines.h"
#include "padding.h"
#include "frcmdtarget.h"
#include "frform.h"
#include "wtypes.h"

class Bitmap;
struct sCharacterInfo;
class FrListBox;
struct FrListItem;
class FrWnd;
class FrWndManager;
// The avatar actors answer message 0x198 with their CPetFrame (its
// FindMotionData is JP 0x8af590).
class CPetFrame;

// One motion of a category list (x86 0x3c: the element size of the
// category vectors, destroyed by 0x481580). Built by the out-of-line
// constructor 0x480fb0 (kind, caption, motion: the strings by value) and,
// for the script's rows, by the default one (inlined in LoadScript 0x482280
// and 0x4825e0). kind: 0 the basic posture (0x4824c0), 1 the 0x4825e0
// rows, 2 the script's "best" motions (LoadScript), 4 random (0x482fd0).
struct sAvatarMotionItem {
	sAvatarMotionItem(void) : kind(0), caption(""), motion("") {}
	sAvatarMotionItem(unsigned int kind_, std::string caption_, std::string motion_)   // 0x480fb0
		: kind(kind_), caption(caption_), motion(motion_) {}

	unsigned int kind;      // 0x0
	std::string caption;    // 0x4 the row's text (FindMotionName: the name)
	std::string motion;     // 0x20 the motion key (FindMotionName compares it)
};
CHECK_SIZE(sAvatarMotionItem, 0x3c);
CHECK_FIELD(sAvatarMotionItem, kind, 0x0);
CHECK_FIELD(sAvatarMotionItem, caption, 0x4);
CHECK_FIELD(sAvatarMotionItem, motion, 0x20);

// One row of the motion name table at FrAvatarMotionDlg +0x110 (x86 0x8:
// FindMotionName 0x481070 indexes it by 8 and _stricmp's the first
// pointer with a script motion key, then copies both into an item).
struct sAvatarMotionName {
	const char* motion;     // 0x0 the motion key
	const char* name;       // 0x4 its display name
};

class FrAvatarMotionDlg : public FrForm {
	LAYOUT_CHECK_FRIEND;
public:
	static const WRTTI m_RTTI;                                        // 0xe73e18
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x481ed0

	FrAvatarMotionDlg(void);                                          // 0x481dc0
	virtual ~FrAvatarMotionDlg(void);                                 // [1] 0x482250 (deleting), 0x481f00

	virtual void OnProc(const float dt);                              // [9] 0x480450
	virtual bool OnInit(void);                                        // [32] 0x4832a0

	// Both openers (0x7b1860, 0xab8350) push the address of the character
	// type id after setting m_flags; 0x481b20 jumps to LoadMotionNames
	// 0x481960, which passes the same pointer on to the motion table.
	void SetCharacterTypeId(const int& typeId);                       // 0x481b20
	void SetOpenedFrom(unsigned int flags) { m_flags = flags; }       // inline in the openers (name ours)
	// Starts the closing animation (OnProc: ProcStateClosing); inline in
	// 0xab4420 and 0x7b04a0 (name ours).
	void BeginClose(void) { m_stateFlags = 4; }

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe037d0

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe037c8
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0x47f2a0

	// Message-map handlers (motionlist).
	void OnMotionListInit(intptr_t param);                            // 0x47f930
	void OnMotionListOwnerDraw(intptr_t param);                       // 0x480d70
	void OnMotionListLBtnDown(void);                                  // 0x480e00

	// OnInit's steps, in call order (names ours).
	void UpdateCenterX(void);                                         // 0x47f2b0
	void LoadBitmaps(void);                                           // 0x47f2d0
	// The list origin from g_view->SCREEN_XS and the JP FrWnd +0x3c.
	void InitLayout(void);                                            // 0x47f9b0
	// The play/stop button position and the play bitmap's size.
	void InitButtonLayout(void);                                      // 0x47f9f0
	// OnProc's steps: m_stateFlags 8 (set by the ctor: the form grows to
	// fit the rows, then 1) and 4 (it shrinks to 40, then 2 and closes),
	// then every frame (the hover fade).
	void ProcStateOpening(const float& dt);                           // 0x47fa50
	void ProcStateClosing(const float& dt);                           // 0x47fb30
	void ProcList(const float& dt);                                   // 0x47fd70
	// Draws the avatar chat row's hover and play button while the own
	// avatar plays a motion (state 4 or 5; m_flags bit 1, m_bool168).
	void DrawAvatarChatRowButtons(FrListItem* pItem);                 // 0x47fbf0
	// Draws the lobby row's hover and play (m_bool169) or stop button
	// (m_flags bit 2, m_bool168).
	void DrawLobbyRowButtons(FrListItem* pItem);                      // 0x47fca0
	// Draws one list row (OnMotionListOwnerDraw, m_stateFlags bit 1).
	void DrawRow(FrListItem* pItem);                                  // 0x480c30
	// The row's caption at pos (m_flags bit 1: 0x4802b0, bit 2: 0x480360).
	void DrawRowText(const int& index, const WPoint& pos);            // 0x480a60
	void DrawAvatarChatRowText(const int& index, const WPoint& pos);  // 0x4802b0
	// The lobby variant (red on the random row while m_avatarState).
	void DrawLobbyRowText(const int& index, const WPoint& pos);       // 0x480360
	// Plays (or stops) the motion of a list row: the lobby path (m_flags
	// 2, set by the lobby opener 0xab8350) or the avatar chat path (1, set
	// by the avatar chat opener 0x7b1860).
	void SelectMotion(const int& index);                              // 0x480a90
	void SelectLobbyMotion(const int& index);                         // 0x480590
	void SelectAvatarChatMotion(const int& index);                    // 0x480730
	// Asks the avatar actor (message 0x198) for its animation set, then
	// fills m_motionNames with the character's motions the set has that
	// are not part motions (names ours).
	void LoadMotionNames(const int& typeId);                          // 0x481960
	// Fills item's motion and caption from the m_motionNames row whose
	// motion key is motion (_stricmp); leaves item as is without one.
	void FindMotionName(const char* motion, sAvatarMotionItem& item); // 0x481070
	// The character whose parts list their motions: the lobby's (m_flags
	// 2: 0xe47dbc's 0x63f3f0(-1) + 0xc, NULL without one) or the avatar's
	// (the JP ntAvatar +0x495, the KR m_characterInfo).
	const sCharacterInfo* GetCharacterInfo(void);                     // 0x480270
	// Clears the five category lists and m_shownItems.
	void ClearLists(void);                                            // 0x482010
	// The character type id the motions are for: the lobby desktop's
	// (m_flags bit 2) or the avatar's (the inline 0x482280 and 0x4825e0
	// share).
	int GetCharacterTypeId(void);
	// Reads "motiondlg.lua" into m_scriptItems (false on failure).
	bool LoadScript(std::string file);                                // 0x482280
	void AddBasicItem(void);                                          // 0x4824c0 m_categoryItems[0]
	// One row per part motion the character has (kind 1).
	void BuildCategory1(void);                                        // 0x4825e0 m_categoryItems[1]
	void BuildCategory2(void);                                        // 0x4827b0 m_categoryItems[2]
	void BuildCategory3(void);                                        // 0x482a80 m_categoryItems[3]
	void AddRandomItem(void);                                         // 0x482fd0 m_categoryItems[4]
	void BuildLists(void);                                            // 0x483170
	// Clears m_shownItems, appends the category lists to it (the first
	// and the fifth only when m_flags bit 2 is set) and adds one list row
	// per item.
	void RefreshList(void);                                           // 0x4831a0
	// Appends every item of a category to m_shownItems (0x482140 each).
	void AppendItems(std::vector<sAvatarMotionItem>& items);          // 0x4830f0

	// x86 0x110 (first, last, end, allocator): the motion name table
	// (FindMotionName), filled by LoadMotionNames.
	std::vector<sAvatarMotionName> m_motionNames; // 0x110
	FrListBox* m_pMotionList;                  // 0x120 motionlist
	// The avatar actor's answer to 0x198 (LoadMotionNames).
	CPetFrame* m_pAnimSet;                     // 0x124
	int m_motionCount;                         // 0x128 m_motionNames' row count (0 in the ctor)
	int m_selected;                            // 0x12c (-1 in the ctor)
	float m_alpha;                             // 0x130 (OnProc: +5 per frame up to 255)
	float m_centerX;                           // 0x134 (UpdateCenterX)
	float m_childAlpha;                        // 0x138 (0 in the ctor; ProcList: the children's alpha, 0.7 to 1 under the cursor)
	// The row rectangle (InitLayout; DrawRow moves y to the drawn row and
	// draws the hover bitmap in it).
	WRect m_listRect;                          // 0x13c
	// The play/stop button rectangle (InitButtonLayout: x m_listRect.x - 13,
	// the play bitmap's size; the row draws move y).
	WRect m_buttonRect;                        // 0x14c
	const Bitmap* m_pPlayBitmap;               // 0x15c "motionchat_list_play"
	const Bitmap* m_pStopBitmap;               // 0x160 "motionchat_list_stop"
	const Bitmap* m_pHoverBitmap;              // 0x164 "motionchat_list_Hover"
	bool m_bool168;                            // 0x168 (false; the ctor stores 0x100 as one dword)
	bool m_bool169;                            // 0x169 (true)
	bool m_bool16a;                            // 0x16a (false)
	// The LobbyAvatar actor's answer to 0x197 (OnInit); a click on the
	// list's last row sets it, any other click clears it.
	bool m_avatarState;                        // 0x16b
	// 2: opened from the lobby (0xab8350), 1: from the avatar chat
	// (0x7b1860); 2 also makes RefreshList show the first and fifth lists.
	unsigned int m_flags;                      // 0x16c
	unsigned int m_stateFlags;                 // 0x170 (8 in the ctor; OnProc tests 8 and 4)
	std::vector<sAvatarMotionItem> m_categoryItems[5]; // 0x174, 0x184, 0x194, 0x1a4, 0x1b4
	std::vector<sAvatarMotionItem> m_shownItems;       // 0x1c4 (RefreshList)
	std::vector<sAvatarMotionItem> m_scriptItems;      // 0x1d4 (LoadScript)
	UNKNOWN(0x1e4, 0x1f0);
};
CHECK_SIZE(FrAvatarMotionDlg, 0x1f0);
CHECK_FIELD(FrAvatarMotionDlg, m_motionNames, 0x110);
CHECK_FIELD(FrAvatarMotionDlg, m_pMotionList, 0x120);
CHECK_FIELD(FrAvatarMotionDlg, m_selected, 0x12c);
CHECK_FIELD(FrAvatarMotionDlg, m_alpha, 0x130);
CHECK_FIELD(FrAvatarMotionDlg, m_listRect, 0x13c);
CHECK_FIELD(FrAvatarMotionDlg, m_buttonRect, 0x14c);
CHECK_FIELD(FrAvatarMotionDlg, m_pPlayBitmap, 0x15c);
CHECK_FIELD(FrAvatarMotionDlg, m_avatarState, 0x16b);
CHECK_FIELD(FrAvatarMotionDlg, m_flags, 0x16c);
CHECK_FIELD(FrAvatarMotionDlg, m_stateFlags, 0x170);
CHECK_FIELD(FrAvatarMotionDlg, m_categoryItems, 0x174);
CHECK_FIELD(FrAvatarMotionDlg, m_shownItems, 0x1c4);
CHECK_FIELD(FrAvatarMotionDlg, m_scriptItems, 0x1d4);

// JP 0x7b1130 (KR CreateForm<FrAvatarMotionDlg>): creates and opens the
// dialog; called by 0x7b1860 and 0xab8350.
FrAvatarMotionDlg* CreateAvatarMotionDlg(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent);

#endif
