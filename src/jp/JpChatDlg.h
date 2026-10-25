// JP-only game code (no KR counterpart): FrChatDlg, the JP lobby/room chat
// panel form ("chat_group" of the Fresh layout): a background area
// (chatbg), a language indicator (language: "chat_kor"/"chat_eng"), the
// input edit (chatinput), the chat view (chatview), the chat target combo
// (chattarget), and the buttons chatwndbtn (folds the panel up/down),
// emoticon (opens "emoticon", FrEmoticonDlg) and report (opens "report").
// No direct opener: it is the base of FrRoomListDlg (ctor 0x51d510) and
// FrGameRoomDlg (ctor 0x5083c0), and the factory 0x48f520 is registered
// under "FrChatDlg" at static init (0xcc8610).
//
// Evidence: node scripts/disasm.ts --jp / jp_hlil.ts. RTTI m_RTTI 0xe73e98
// (GetRTTI 0x48f110), base the JP FrForm (ctor 0xbe5e20, dtor 0xbe5e80).
// Vtables 0xcff63c (38 slots: [0] GetRTTI, [1] the deleting destructor,
// [9] OnProc, [32] OnInit; the rest FrForm's), 0xcff630 (FrCmdTarget: [0]
// deleting-destructor thunk 0x48f400, [1] GetMessageMap 0x48f0a0), 0xcff628
// ([0] thunk 0x48f3f0). Message map 0xe04210 (rows 0xe04218, handlers
// stored by the initializer 0xcc8630). Size 0x140 (x86).
//
// JP x86 offsets in the comments: the JP FrWnd is 0x14 bigger than KR's up
// to its scroll bar (JP m_nFlags +0x1c, m_rect +0x34, m_pScrBar +0x90),
// while the JP FrForm ends at 0x110 like KR's.
#ifndef PG_JP_JPCHATDLG_H
#define PG_JP_JPCHATDLG_H

#include "defines.h"
#include "padding.h"
#include "frcmdtarget.h"
#include "frform.h"
#include "wtypes.h"

class FrArea;
class FrButton;
class FrComboBox;
class FrEdit;
class FrWnd;
class FrWndManager;

class FrChatDlg : public FrForm {
	LAYOUT_CHECK_FRIEND;
public:
	static const WRTTI m_RTTI;                                        // 0xe73e98
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x48f110

	FrChatDlg(void);                                                  // 0x48f0b0
	virtual ~FrChatDlg(void);                                         // [1] 0x48f5b0 (deleting), 0x48f120

	// Ctrl+EMOTICON opens the emoticon dialog (when no messenger is up);
	// with a chat target and input, PAGE_UP / PAGE_DOWN walk the sent-text
	// history (or the chat targets), ENTER focuses the input; then tints the
	// input by the target (all, guild, whisper).
	virtual void OnProc(const float dt);                              // [9] 0x490430
	virtual bool OnInit(void);                                        // [32] 0x48f140

	// The factory the JP form factory registers under "FrChatDlg".
	static IObject* Create(void);                                     // 0x48f520

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe04218

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe04210
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0x48f0a0

	// Message-map handlers.
	// The report dialog's result: forgets it.
	bool OnReportDlgResult(int ret, FrForm* pForm);                   // 0x48f1a0
	void OnChatTargetLBtnDown(intptr_t param);                       // 0x48f1b0
	bool OnChatTargetEnterKey(intptr_t param);                       // 0x48f1c0
	void OnChatBgInit(intptr_t param);                               // 0x48f5e0
	// A translucent black box over chatbg, when the manager has a GDI.
	void OnChatBgOwnerDraw(intptr_t param);                          // 0x48f650
	void OnLanguageInit(intptr_t param);                             // 0x48f7f0
	void OnChatInputInit(intptr_t param);                            // 0x48f890
	void OnEmoticonInit(intptr_t param);                             // 0x48f960
	// Stores chatwndbtn (no push delay, no double click) and folds the
	// panel by the option's chat-window state.
	void OnChatWndBtnInit(intptr_t param);                           // 0x48fd50
	void OnChatWndBtnLBtnDown(void);                                  // 0x48fdf0
	// Stores chattarget (HidePrivacy, the list opens above: JP FrComboBox
	// +0x2c4 = 1) and fills it with CSharedDoc::InitReservedChatList (JP
	// 0x645bc0), partner team+guild when the room game type is 6, else guild.
	void OnChatTargetInit(intptr_t param);                           // 0x48fe10
	// The emoticon dialog's result: on OK, inserts the picked icon's code
	// at the caret of the chat input (of the report dialog's "id" edit when
	// that dialog is up) if the line stays within the edit's width limit.
	// JP FrEdit text pieces +0x12c/+0x148/+0x164 = KR m_editText[] (same
	// offsets), the float +0x100 compared with the text width = KR
	// m_widthLimit (+0x104); JP FrEmoticonDlg +0x114 = KR m_szSelectedIcon
	// (same offset). CChatMsg masked font: JP CChatMsg +0x34.
	bool OnEmoticonDlgResult(int ret, FrForm* pForm);                 // 0x48ffe0
	// Creates "emoticon" (CreateForm<FrEmoticonDlg>) into m_pEmoticonDlg
	// once and opens it above the emoticon button.
	void OnEmoticonLBtnDown(void);                                    // 0x490170
	// Creates "report" (0x48fab0, CreateForm<FrReportDlg>) into
	// m_pReportDlg and opens it with the three translated rules.
	void OnReportLBtnDown(void);                                      // 0x4907f0
	// Stores chatview (HidePrivacy) and empties the JP CSharedDoc's chat
	// lines.
	void OnChatViewInit(intptr_t param);                             // 0x490b30

public:
	// Keeps a line (in the lobby modes 3, 4, 6, 8) in the CSharedDoc chat
	// lines and adds it to the chat view. Called by CLobbyMain's chat
	// message (JP 0xabc3c5) on the room list and room forms.
	void AddChatMsg(const char* msg, unsigned int color);             // 0x490d10

protected:
	// Sends a non-empty line (CSharedDoc::UTIL_SendChatMessage with the
	// target combo); on failure resets the target to "to all" and prints
	// the wrong-target text.
	bool OnChatInputEnterKey(intptr_t param);                        // 0x490e50

	// Folds the panel down or up by 200 (chatbg and chatview grow or
	// shrink from the top) by the option's chat-window state, with the
	// chatwnd_up/chatwnd_down button image; scrolls the view to the bottom.
	void ToggleChatBtn(bool bMove);                                   // 0x48f6c0

	FrForm* m_pEmoticonDlg;                    // 0x110 "emoticon" (OnEmoticonLBtnDown)
	FrForm* m_pReportDlg;                      // 0x114 "report" (OnReportLBtnDown)
	FrArea* m_pChatBg;                         // 0x118 chat_group.chatbg
	FrButton* m_pChatWndBtn;                   // 0x11c chat_group.chatwndbtn
	FrArea* m_pLanguage;                       // 0x120 chat_group.language
	FrEdit* m_pChatInput;                      // 0x124 chat_group.chatinput
	FrEdit* m_pChatView;                       // 0x128 chat_group.chatview
	FrComboBox* m_pChatTarget;                 // 0x12c chat_group.chattarget
	FrButton* m_pEmoticon;                     // 0x130 chat_group.emoticon
	UNKNOWN(0x134, 0x140);
};
CHECK_SIZE(FrChatDlg, 0x140);

#endif
