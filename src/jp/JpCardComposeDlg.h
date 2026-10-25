// JP-only game code (no KR counterpart): FrCardComposeDlg, the JP card
// compose dialog (layout "card_compose"). The player puts three cards of his
// list (my_card_list, filtered by the normal / rare / super rare / whole
// card buttons) into three slots (compose_card_btn1..3), sees the pang cost
// (print_pang_area, 1000 / 2000 / 5000 per card by its rarity) and composes
// (compose_card: opens "card_compose_check", FrCardComposeCheckDlg). about
// opens "card_compose_about" (FrCardComposeAboutDlg); MagicBox and Exchange
// post task messages 0x338 / 0x2e3 to the current task (the dialog's
// siblings); CardCompose is the current tab (no-op).
//
// Opener: CTaskMain 0x659490 (TaskMain_OpenCardComposeDlg, below) calls the
// factory 0x654970
// (CreateForm<FrCardComposeDlg>(mgr, owner, "card_compose", parent):
// operator new(0x570), ctor 0x48e400, FrForm::_Init, DynamicCast to m_RTTI
// 0xe73e68). The JP form registry's creator 0x48e920 (`new` + ctor) is
// registered as "FrCardComposeDlg" at static init.
//
// Evidence: node scripts/disasm.ts --jp / jp_hlil.ts. Vtables 0xcff25c
// (38 slots: [0] GetRTTI 0x48de60, [1] deleting destructor 0x48e590, [32]
// OnInit 0x48e5c0, the rest FrForm's), 0xcff24c (FrCmdTarget at +4: [0]
// thunk 0x48de70, [1] GetMessageMap 0x488d60), 0xcff244 ([0] thunk
// 0x48de80). Message map 0xe03948 (32 rows at 0xe03968, handlers stored by
// the initializer 0xcc7df0). Size 0x570 (x86). Member types of the casts
// by use: RTTI 0xf1aae0 FrListBox (SortItem), 0xf1ab38 FrButton
// (SetStatus), 0xf1aad4 FrArea (print_pang_area).
//
// Server packets: compose request 0x155, sent by 0x486f70 when
// FrCardComposeCheckDlg closes with FrYES (result callback 0x489040, after
// the pang check 0x487960 on the slot state): u64 price (0x4870d0: 1000 /
// 2000 / 5000 per card by rarity), then the three slot card type ids
// (u32 each, m_slotCardId); then posts task message 1 to the current
// task. The answer (TaskMain 0x325 / 0x326) opens FrCardComposeResultDlg
// ("result_card_compose", OpenResultDlg 0x48ef20).
#ifndef PG_JP_JPCARDCOMPOSEDLG_H
#define PG_JP_JPCARDCOMPOSEDLG_H

#include "defines.h"
#include "padding.h"
#include "frcmdtarget.h"
#include "frform.h"
#include "wtypes.h"
#include "classdefine.h"
#include "SharedDoc.h"
#include "fx.h"
#include "wmath.h"

#include <stdint.h>
#include <list>
#include <map>
#include <string>

class Bitmap;
class FrArea;
class FrCardComposeDlg;
class FrButton;
class FrGraphicInterface;
class FrListBox;
struct FrListItem;
class FrTextButton;
class FrViewer;
class FrWndManager;
class WOverlay;
class WResource;
class WTitleFont;

// FrCardComposeAboutDlg ("card_compose_about"): the six help pages
// (help_cardmix_01..06.jpg in about_viewer), picked by the index01..06
// tabs or walked with backward / forward. Opened by
// FrCardComposeDlg::OnAboutLBtnDown through its CreateForm 0x48b970
// (operator new(0x140), RTTI 0xe73e7c). Vtables 0xcfef8c ([0] GetRTTI
// 0x489170, [1] deleting destructor 0x48aa70, [32] OnInit 0x48af90),
// 0xcfef7c ([1] GetMessageMap 0x489100), 0xcfef74. Message map 0xe03958
// (17 rows at 0xe03e48, initializer 0xcc82e0). FrViewer RTTI 0xf1ab10,
// FrTextButton 0xf1ab2c. Size 0x140 (x86).
class FrCardComposeAboutDlg : public FrForm {
	LAYOUT_CHECK_FRIEND;
public:
	static const WRTTI m_RTTI;                                        // 0xe73e7c
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x489170

	FrCardComposeAboutDlg(void);                                      // 0x489110
	virtual ~FrCardComposeAboutDlg(void);                             // [1] 0x48aa70 (deleting), 0x489180

	// Shows page 0 and clears the JP lobby window's +0xb0.
	virtual bool OnInit(void);                                        // [32] 0x48af90

	// The factory the JP form registry registers under
	// "FrCardComposeAboutDlg".
	static FrForm* Create(void);                                      // 0x48a9e0

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe03e48

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe03958
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0x489100

	void OnAboutViewerInit(intptr_t param);                          // 0x48aaa0
	void OnIndex01Init(intptr_t param);                              // 0x48ab10
	void OnIndex01LBtnUp(void);                                       // 0x48ab80
	void OnIndex02Init(intptr_t param);                              // 0x48aba0
	void OnIndex02LBtnUp(void);                                       // 0x48ac10
	void OnIndex03Init(intptr_t param);                              // 0x48ac30
	void OnIndex03LBtnUp(void);                                       // 0x48aca0
	void OnIndex04Init(intptr_t param);                              // 0x48acc0
	void OnIndex04LBtnUp(void);                                       // 0x48ad30
	void OnIndex05Init(intptr_t param);                              // 0x48ad50
	void OnIndex05LBtnUp(void);                                       // 0x48adc0
	void OnIndex06Init(intptr_t param);                              // 0x48ade0
	void OnIndex06LBtnUp(void);                                       // 0x48ae50
	void OnBackwardInit(intptr_t param);                             // 0x48ae70
	void OnBackwardLBtnUp(void);                                      // 0x48aee0
	void OnForwardInit(intptr_t param);                              // 0x48af00
	void OnForwardLBtnUp(void);                                       // 0x48af70

	// Page 0..5: disables backward on the first page and forward on the
	// last (JP FrWnd vtable +0x18, Enable), selects the page's tab and
	// loads help_cardmix_<m_page+1>.jpg into the viewer.
	void ShowPage(int page);                                          // 0x4891a0

	FrTextButton* m_pIndex[6];                 // 0x110 index01..06
	int m_page;                                // 0x128 0..5
	FrButton* m_pBackward;                     // 0x12c backward
	FrButton* m_pForward;                      // 0x130 forward
	UNKNOWN(0x134, 0x138);
	FrViewer* m_pViewer;                       // 0x138 about_viewer
	UNKNOWN(0x13c, 0x140);
};
CHECK_SIZE(FrCardComposeAboutDlg, 0x140);

// FrCardComposeCheckDlg ("card_compose_check"): shows the three chosen
// cards (card_compose_viewer1..3) and asks; accept_compose closes with
// FrYES, cancel_compose with FrCANCEL (the JP FrForm vtable +0x68, Close).
// Opened by FrCardComposeDlg::RequestCompose through its CreateForm
// 0x48d0a0 (operator new(0x170)), with FrCardComposeDlg's result callback
// 0x489040. Vtables 0xcff0fc ([0] GetRTTI 0x48c580, [1] deleting
// destructor 0x48d430), 0xcff0f0 ([1] GetMessageMap 0x4890d0), 0xcff0e8.
// Message map 0xe03950 (5 rows at 0xe03d88, initializer 0xcc81e0). Size
// 0x170 (x86).
class FrCardComposeCheckDlg : public FrForm {
	LAYOUT_CHECK_FRIEND;
public:
	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x48c580

	FrCardComposeCheckDlg(void);                                      // 0x48c500
	virtual ~FrCardComposeCheckDlg(void);                             // [1] 0x48d430 (deleting), 0x48c5b0

	// The factory the JP form registry registers under
	// "FrCardComposeCheckDlg".
	static FrForm* Create(void);                                      // 0x48d3a0

	// Copies the three card image names (the card records' +0xc1, KR
	// sCard::m_image) and appends ".png".
	void SetCardNames(const std::string* names);                      // 0x48dbb0
	// Opens each image in its viewer, stopping at the first missing
	// viewer.
	void ShowCards(void);                                             // 0x48be60

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe03d88

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe03950
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0x4890d0

	void OnViewer1Init(intptr_t param);                              // 0x48a890
	void OnViewer2Init(intptr_t param);                              // 0x48a900
	void OnViewer3Init(intptr_t param);                              // 0x48a970
	void OnAcceptComposeLBtnUp(void);                                 // 0x4890e0
	void OnCancelComposeLBtnUp(void);                                 // 0x4890f0

	std::string m_cardNames[3];                // 0x110 "<image>.png"
	FrViewer* m_pViewer[3];                    // 0x164 card_compose_viewer1..3
};
CHECK_SIZE(FrCardComposeCheckDlg, 0x170);

// A card of my_card_list (FrCardComposeDlg::m_cardMap's value, the list
// items' pData): the held card (the JP CSharedDoc +0x37e0 entry, KR sCards,
// copied 0xe dwords + a word) and its card record (0x48de90 copies the JP
// record, 0x60 dwords, 0x180 bytes, from CItemManager::FindCard 0xa17a10).
// The JP record fields used map onto the KR sCard: +0x8 the name (KR
// m_name 0x8; the JP name is 0x18 bytes longer), +0x49 the list image (KR
// m_icon 0x31, as JpAttendanceDlg's sItem +0x49), +0xc0 the rarity 0..3
// (KR m_grade 0x90, by meaning), +0xc1 the slot / check image (KR m_image
// 0xc8, as JpLobbyGlue's JpCardData_GetImageName). The port keeps the KR
// sCard (0x148), so the size differs (JP 0x1bc).
struct CardComposeCard {
	sCards m_card;                             // 0x0
	IFF_STRUCT::sCard m_iff;                   // 0x3c
};
CHECK_FIELD(CardComposeCard, m_card, 0x0);
CHECK_FIELD(CardComposeCard, m_iff, 0x3c);

// A slot of CardComposeImageCache (0x28 bytes x86). The destructor
// 0x487370 is implicit.
struct CardComposeImageSlot {
	CardComposeImageSlot(void) : m_lastUse(0) {}   // 0x487350

	std::string m_name;                        // 0x0  the image file name
	int64_t m_lastUse;                         // 0x20 time() of the last Add / Load (JP __time64_t), 0 = free
};
CHECK_SIZE(CardComposeImageSlot, 0x28);
CHECK_FIELD(CardComposeImageSlot, m_name, 0x0);
CHECK_FIELD(CardComposeImageSlot, m_lastUse, 0x20);

// FrCardComposeDlg's slot card images (+0x170, 0x330 bytes x86; no vtable,
// so no RTTI name: named by use). Keeps up to 20 WOverlay by file name,
// loaded through the resource manager (0xf023f0); when the 20 slots are
// taken, Add replaces the least recently used one (deleting its overlay).
// Unlike the JP RFTextureManager (one std::map entry per image, no limit)
// it is a small LRU of its own. Constructor 0x488920 and destructor
// 0x488740 are implicit (the slot array and the map).
class CardComposeImageCache {
	LAYOUT_CHECK_FRIEND;
public:
	// The overlay of `name`, loaded and added on first use (null when the
	// resource manager has no such image); a cached one refreshes its
	// slot's m_lastUse.
	WOverlay* Load(const char* name);                                 // 0x488b60
	// Releases every overlay through the resource manager (while it
	// lives) and empties the map; the slots keep their names.
	void Clear(void);                                                 // 0x4885d0

private:
	// Stores `overlay` under `name` in the first free slot, else in the
	// oldest one (its overlay deleted and its map entry erased).
	void Add(const char* name, WOverlay* overlay);                    // 0x4889f0

	CardComposeImageSlot m_slots[20];          // 0x0
	std::map<std::string, WOverlay*> m_overlays; // 0x320 (head 0x324, size 0x328)
};
CHECK_SIZE(CardComposeImageCache, 0x330);
BEGIN_LAYOUT_CHECK(CardComposeImageCache)
	CHECK_FIELD(CardComposeImageCache, m_slots, 0x0);
	CHECK_FIELD(CardComposeImageCache, m_overlays, 0x320);
END_LAYOUT_CHECK

// FrCardComposeResultDlg ("result_card_compose"): the compose result. The
// effect sequence of the new card's grade (FrCardComposeDlg::m_resultSeq)
// plays at the centre of synthesized_card (fx_player draws it, clipped to
// fx_player's rect); the card image (synthesized_card, hidden at init)
// shows 2 seconds after the viewer loaded it, and the "o" (OK) button is
// enabled after 4. Opened by FrCardComposeDlg::OpenResultDlg 0x48ef20
// through its CreateForm 0x48ee50 (operator new(0x5c0), RTTI 0xe73e84).
// Vtables 0xcff374 ([0] GetRTTI 0x48ec80, [1] deleting destructor
// 0x48ee20, [9] OnProc 0x48b0e0, the rest FrForm's), 0xcff368
// (FrCmdTarget: [0] thunk 0x48ec90, [1] GetMessageMap 0x489250), 0xcff360
// ([0] thunk 0x48eca0). Message map 0xe03960 (5 rows at 0xe04088, handlers
// stored by the initializer 0xcc8530). Casts by use: RTTI 0xf1ab10
// FrViewer, 0xf1aad4 FrArea, 0xf1ab38 FrButton. Size 0x5c0 (x86).
class FrCardComposeResultDlg : public FrForm {
	LAYOUT_CHECK_FRIEND;
public:
	static const WRTTI m_RTTI;                                        // 0xe73e84
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x48ec80

	// The effect: Init on the resource manager, the game camera (JP
	// 0xf02430), frame time 33.333f (JP +0x138); the draw camera is the
	// identity moved along its z axis by -30 * screen height / 600.
	FrCardComposeResultDlg(void);                                     // 0x48ecb0
	// Clears the effect (ClearAll(false)).
	virtual ~FrCardComposeResultDlg(void);                            // [1] 0x48ee20 (deleting), 0x48ebc0

	// Runs the effect (ms = dt * 1000) while it plays and counts
	// m_showTime: the card viewer shows past 2 seconds, the OK button is
	// enabled past 4 (m_showTime back to -1).
	virtual void OnProc(const float dt);                              // [9] 0x48b0e0

	// The factory the JP form registry registers under
	// "FrCardComposeResultDlg".
	static FrForm* Create(void);                                      // 0x48efd0

	// The opener's calls (FrCardComposeDlg::OpenResultDlg).
	void SetOwnerDlg(FrCardComposeDlg* dlg);                          // 0x489260
	// Loads `name` (a .seq) into the effect at the centre of the card
	// viewer, both coordinates rounded to an int.
	void PlaySequence(const char* name);                              // 0x48b380
	// Opens m_cardImage in the card viewer; m_showTime = 1 when it loaded.
	void ShowCard(void);                                              // 0x48bea0
	// The card image of `cardId` (CItemManager::FindCard, KR sCard::m_image
	// = JP record +0xc1) + ".png" into m_cardImage; nothing for an unknown
	// card.
	void SetCardImage(const unsigned int& cardId);                    // 0x48d850

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe04088

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe03960
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0x489250

	// "o": OnFreshOkay (0xbe5e10, tail jump).
	void OnOLBtnUp(void);                                             // 0x489270
	// Stores synthesized_card and hides it (JP vtable +0x1c, SetVisible).
	void OnSynthesizedCardInit(intptr_t param);                      // 0x48afc0
	// Copies fx_player's rect (no NULL test) and centres the effect's
	// screen at its x, y.
	void OnFxPlayerInit(intptr_t param);                             // 0x48b030
	// The effect drawn in perspective from m_effectCamera inside m_fxRect,
	// then the view restored (as FrAlchemyMPBreakDlg 0x4f56a0, plus the
	// clipping area).
	void OnFxPlayerOwnerDraw(void);                                   // 0x48b1c0
	// Stores "o" and disables it (JP vtable +0x18, Enable).
	void OnOInit(intptr_t param);                                    // 0x48b420

	std::string m_cardImage;                   // 0x110 "<image>.png"
	FrViewer* m_pSynthesizedCard;              // 0x12c synthesized_card
	FrButton* m_pOk;                           // 0x130 o
	// JP size 0x43c (KR 0x364): the members below are at their JP offsets
	// in the comments only.
	CPvsFxBox m_effect;                        // 0x134
	UNKNOWN(0x570, 0x574);
	WMatrix m_effectCamera;                    // 0x574 the camera OnFxPlayerOwnerDraw draws from
	// fx_player's rect (0, 0, 1, 1 until its init, JP 0xe428fc).
	WRect m_fxRect;                            // 0x5a4
	FrCardComposeDlg* m_pOwnerDlg;             // 0x5b4 (SetOwnerDlg; not set by the ctor)
	// Seconds since the card image loaded (ShowCard: 1), -1 = not counting.
	float m_showTime;                          // 0x5b8
	UNKNOWN(0x5bc, 0x5c0);
};
CHECK_SIZE(FrCardComposeResultDlg, 0x5c0);
BEGIN_LAYOUT_CHECK(FrCardComposeResultDlg)
	CHECK_FIELD(FrCardComposeResultDlg, m_cardImage, 0x110);
	CHECK_FIELD(FrCardComposeResultDlg, m_pSynthesizedCard, 0x12c);
	CHECK_FIELD(FrCardComposeResultDlg, m_pOk, 0x130);
	CHECK_FIELD(FrCardComposeResultDlg, m_effect, 0x134);
	CHECK_FIELD(FrCardComposeResultDlg, m_effectCamera, 0x574);
	CHECK_FIELD(FrCardComposeResultDlg, m_fxRect, 0x5a4);
	CHECK_FIELD(FrCardComposeResultDlg, m_pOwnerDlg, 0x5b4);
	CHECK_FIELD(FrCardComposeResultDlg, m_showTime, 0x5b8);
END_LAYOUT_CHECK

class FrCardComposeDlg : public FrForm {
	LAYOUT_CHECK_FRIEND;
public:
	static const WRTTI m_RTTI;                                        // 0xe73e68
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x48de60

	// Builds the price font (0x48bb90) and empties the compose price
	// string at +0x54c.
	FrCardComposeDlg(void);                                           // 0x48e400
	// Clears the card map (0x48d460, lib), releases +0x158, the slot images
	// (CardComposeImageCache::Clear) and the price font while the resource
	// manager (0xf023f0) lives.
	virtual ~FrCardComposeDlg(void);                                  // [1] 0x48e590 (deleting), 0x48dd20

	// Loads the bitmaps and the icon sizes, fills the list with every card
	// (FillCardList(cards, 3)) and sorts it, presses the CardCompose tab,
	// and in avatar-chat mode (0xe78c3c == 1) gives the player's avatar
	// state 0x20 (ntAvatarManager 0xe73e10 +0xa4, ntAvatar::AddState:
	// SetState 0x7a4820) and makes the avatar manager inactive (+0xb0).
	// Returns true without FrForm::OnInit.
	virtual bool OnInit(void);                                        // [32] 0x48e5c0

	// The factory the JP form registry registers under
	// "FrCardComposeDlg".
	static FrForm* Create(void);                                      // 0x48e920

	// The compose answer (JP CTaskMain::HandleMsgCommon cases 0x325 /
	// 0x326, posted by the TaskManager.cpp JP_PAKS readers of packets
	// 0x229 / 0x22a).
	// The result effect of the new card's grade (0 normal, 1 rare, 2 super
	// rare, 3 secret; others leave it): "card_compose_N.seq" /
	// "card_compose_R.seq" / "card_compose_SR.seq" / "card_compose_SC.seq"
	// into m_resultSeq.
	void SetResultSeq(const int& grade);                              // 0x48d7c0
	// The new card's type id into m_resultCardId; false (nothing stored)
	// for 0.
	bool SetResultCard(const unsigned int& cardId);                   // 0x488fa0
	// "result_card_compose" (CreateForm<FrCardComposeResultDlg>, JP
	// 0x48ee50) into m_pResultDlg, owner the dialog's FrCmdTarget base; on
	// it SetCardImage(m_resultCardId), PlaySequence(m_resultSeq),
	// SetOwnerDlg(this), ShowCard, and Open(OnResultDlgResult, 1).
	void OpenResultDlg(void);                                         // 0x48ef20

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe03968

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe03948
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0x488d60

	// Message-map handlers.
	// Stores my_card_list and turns its double click off (JP FrWnd style
	// +0x44 |= 0x200, UseDblClick(false)).
	void OnMyCardListInit(intptr_t param);                           // 0x489dd0
	// Draws the row highlight under the cursor (m_pCardIcon at the list's
	// x + 2, the row's y + 1), the card's image, name and slot count.
	void OnMyCardListOwnerDraw(intptr_t param);                      // 0x48bd40
	// Puts the card under the cursor into the first free slot, shows that
	// slot, updates the price and enables compose_card once the slots are
	// full; when all three slots are already full, opens "notify" instead.
	void OnMyCardListLBtnDown(void);                                  // 0x48da90
	// Stores compose_card (FrButton) and disables it (JP vtable +0x18,
	// Enable).
	void OnComposeCardInit(intptr_t param);                          // 0x48a1f0
	// With three cards set: if the player's pang (CSharedDoc 0xe47dbc
	// +0x91e/+0x922) covers the price, remembers the list's scroll
	// position (+0x548) and calls RequestCompose; otherwise opens "notify".
	void OnComposeCardLBtnDown(void);                                 // 0x48e650
	// Store compose_card_btn1..3 and hide them (JP vtable +0x1c,
	// SetVisible).
	void OnComposeCardBtn1Init(intptr_t param);                      // 0x48a370
	void OnComposeCardBtn2Init(intptr_t param);                      // 0x48a3f0
	void OnComposeCardBtn3Init(intptr_t param);                      // 0x48a470
	void OnComposeCardBtn1LBtnUp(void);                               // 0x48a3e0
	void OnComposeCardBtn2LBtnUp(void);                               // 0x48a460
	void OnComposeCardBtn3LBtnUp(void);                               // 0x48a4e0
	void OnComposeCardBtn1OwnerDraw(intptr_t param);                 // 0x48bde0
	void OnComposeCardBtn2OwnerDraw(intptr_t param);                 // 0x48bdf0
	void OnComposeCardBtn3OwnerDraw(intptr_t param);                 // 0x48be00
	void OnAboutInit(intptr_t param);                                // 0x48a650
	// Opens "card_compose_about" (0x48b970, CreateForm of
	// FrCardComposeAboutDlg) with no result callback, flag 3 (JP FrForm
	// vtable +0x64, Open).
	void OnAboutLBtnDown(void);                                       // 0x48be10
	void OnWholeCardButtonInit(intptr_t param);                      // 0x48a6c0
	void OnWholeCardButtonLBtnDown(void);                             // 0x48e740
	void OnNormalCardButtonInit(intptr_t param);                     // 0x48a740
	void OnNormalCardButtonLBtnDown(void);                            // 0x48e750
	void OnRareCardButtonInit(intptr_t param);                       // 0x48a7b0
	void OnRareCardButtonLBtnDown(void);                              // 0x48e760
	void OnSuperRareCardButtonInit(intptr_t param);                  // 0x48a820
	void OnSuperRareCardButtonLBtnDown(void);                         // 0x48e770
	void OnPrintPangAreaInit(intptr_t param);                        // 0x489e50
	void OnPrintPangAreaOwnerDraw(intptr_t param);                   // 0x48bc40
	// Store the tab button, show it (JP vtable +0x1c), set its push delay
	// (JP FrButton +0x104 / +0x108: 0.01f, 0.11f) and its hover
	// (0xbe7a40, EnableHover(true, 0.0f)).
	void OnMagicBoxInit(intptr_t param);                             // 0x489ec0
	void OnCardComposeInit(intptr_t param);                          // 0x489f70
	void OnExchangeInit(intptr_t param);                             // 0x48a020
	// Close the dialog (0xbe5e00, OnFreshCancel) and send message 0x338
	// (MagicBox) / 0x2e3 (Exchange) to the current task's main actor
	// (CTaskManager 0xe73df0, vtable +0x30, SendMsgToMainActor).
	void OnMagicBoxLBtnUp(void);                                      // 0x48bc80
	void OnCardComposeLBtnUp(void);                                   // 0x488e90
	void OnExchangeLBtnUp(void);                                      // 0x48bce0

	// Shows the cards of one rarity (0 normal, 1 rare, 2 super rare, 3
	// all): releases the previous filter button, refills and sorts the
	// list.
	void SetCardFilter(int filter);                                   // 0x48e260
	// Fills my_card_list with the player's cards (CSharedDoc +0x37e0) of
	// the rarity filter; true when it added any. JP game state: declared.
	// Each held card with a card record gets (or refreshes the count of)
	// its m_cardMap entry; the cards with a type id, a rarity other than 3
	// and a kind (type id bits 22-25) other than 3 / 4 whose rarity
	// matches the filter (3: any) are added to the list.
	bool FillCardList(std::list<sCards>& cards, int filter);           // 0x48de90
	// Makes the price font (WResourceManager::GetTitleFont, "[font_bong.jpg",
	// 9 x 16 digits "1234567890,-") and the two price strings (player
	// pang, 0).
	void InitPriceFont(void);                                         // 0x48bb90
	// The list's sort predicate (CardComposeCard): rarity, then the kind
	// order (GetCardKindOrder), then the type id, ascending.
	static bool CompareCardItems(const void* a, const void* b);       // 0x488db0
	// The kind (type id bits 22-25) sort order: 2 -> 0, 0 -> 1, 1 -> 2,
	// 5 -> 3, others -1.
	static int GetCardKindOrder(const IFF_STRUCT::sCard* card);       // 0x488d70
	// Empties slot `slot` (0x488f30 removes the card); re-enables
	// compose_card when the slots are no longer full, updates the price
	// and hides the slot button.
	void RemoveSlotCard(int slot);                                    // 0x488fc0
	bool RemoveSlotCardData(int slot);                                // 0x488f30
	// OnInit's set-up: the list and slot bitmaps, the two offsets, and the
	// list icon / slot icon sizes (Bitmap::Width / Height, unsigned, when
	// the bitmap is loaded).
	void LoadBitmaps(void);                                           // 0x488e20
	void InitCardImageSize(void);                                     // 0x488e70
	void InitCardIconSize(void);                                      // 0x489d30
	void InitSlotIconSize(void);                                      // 0x489d80
	// The checks before sending (0x487960, on +0x160 with the price and
	// +0x140): three slots, enough pang, no super rare card, no card of
	// kind 3 or 4, and enough held copies of each slot card.
	bool CheckComposePrice(long long& price);                         // 0x487960
	// A list row's card image (m_iff.m_icon), name (m_iff.m_name) and the
	// held count minus the slots holding it (CountSlotCard).
	void DrawCardImage(FrListItem* item, FrGraphicInterface* gdi, const CardComposeCard* card); // 0x48a260
	static void DrawCardName(FrListItem* item, const CardComposeCard* card); // 0x48a2c0
	static void DrawCardCount(FrListItem* item, int count);           // 0x48a310
	int CountSlotCard(const CardComposeCard* card) const;             // 0x488f60
	// The slot-state helpers. The original calls them on the sub-object
	// at +0x140 (card ids) / +0x160 (count): declared on the dialog.
	// True when the three slots are filled (0x486720, on +0x160).
	bool AreSlotsFull(void) const;                                    // 0x486720
	// The compose price of the slot cards (0x4870d0, on +0x140: 1000 /
	// 2000 / 5000 per card by rarity).
	long long GetComposePrice(void) const;                            // 0x4870d0 (compared signed with the pang)
	// Takes the last free slot for `card` (stores its type id, counts the
	// slot) while the held count exceeds the slots already holding it;
	// returns the slot, or -1.
	int FindFreeSlot(const CardComposeCard* card);                    // 0x488ea0
	// Loads the slot image (m_iff.m_image + ".png", m_cardImages.Load)
	// into m_slotCardView[slot].
	void SetSlotCard(const CardComposeCard* card, int slot);          // 0x48d6f0
	// Draws slot `slot`'s card (DrawSlotCardView), then the slot frame
	// when its button is not NORMAL (DrawSlotIcon).
	void DrawSlotCard(int slot);                                      // 0x48a5f0
	// Renders the slot image (WOverlay::Render, the whole texture) at the
	// slot button + 1.5, 173 x 352.
	void DrawSlotCardView(int slot);                                  // 0x48a4f0
	// m_pSlotIcon (OVER) or m_pSlotIconDown (PRESSED) at the slot button.
	void DrawSlotIcon(int slot, FrButton* btn, FrGraphicInterface* gdi); // 0x48a580
	// The player's pang (y + 5) and the compose price (y + 31) into
	// print_pang_area, right-aligned at x + 155 (the price font's digit
	// width - 1 per character).
	void DrawOwnPang(FrGraphicInterface* gdi);                        // 0x48a0d0
	void DrawComposePrice(FrGraphicInterface* gdi);                   // 0x48a160
	// Opens "card_compose_check" (0x48d0a0, owner this) into m_pCheckDlg
	// with the result callback OnCheckResult (flag 1), hands it the three
	// slot cards' image names (CItemManager::FindCard; stops when one is
	// missing) and shows them.
	void RequestCompose(void);                                        // 0x48e000
	// FrCardComposeCheckDlg's result: on FrYES, when the pang check
	// 0x487960 passes, sends the compose request and disables
	// compose_card; always forgets m_pCheckDlg. Declared (JP game state).
	bool OnCheckResult(int ret, FrForm* form);                        // 0x489040
	// Empties the slots (ids and buttons), resets both price strings
	// (player pang, 0), refills and sorts the list with the current
	// filter, zeroes the slot count and restores the list's scroll
	// position (m_listScrollPos).
	void ResetCompose(void);                                          // 0x48e160
	// FrCardComposeResultDlg's result: FrOK or FrCANCEL reset the compose
	// (ResetCompose); always forgets m_pResultDlg.
	bool OnResultDlgResult(int ret, FrForm* form);                    // 0x48e230
	// Sends packet 0x155 (u64 price, the three card ids as u32) and posts
	// task message 1 to the current task's main actor.
	static void SendComposeRequest(unsigned long long price, const unsigned int* cardIds); // 0x486f70

	FrListBox* m_pMyCardList;                  // 0x110 my_card_list
	FrButton* m_pComposeCard;                  // 0x114 compose_card
	FrButton* m_pSlotBtn[3];                   // 0x118 compose_card_btn1..3
	FrButton* m_pAbout;                        // 0x124 about
	FrArea* m_pPrintPangArea;                  // 0x128 print_pang_area
	FrButton* m_pMagicBox;                     // 0x12c MagicBox
	FrButton* m_pCardCompose;                  // 0x130 CardCompose
	FrButton* m_pExchange;                     // 0x134 Exchange
	int m_selectedIndex;                       // 0x138 -1 (ctor)
	unsigned int m_resultCardId;               // 0x13c the composed card (SetResultCard), 0 (ctor)
	// The slots' card type ids (summed by the price 0x4870d0) and the
	// slots' card images (m_cardImages; drawn by DrawSlotCardView, cleared
	// by RemoveSlotCard).
	unsigned int m_slotCardId[3];              // 0x140
	WOverlay* m_slotCardView[3];               // 0x14c
	WResource* m_pResource;                    // 0x158 released by the dtor
	UNKNOWN(0x15c, 0x160);                     // 0 (ctor)
	// Filled slots; 3 = full (0x486720).
	int m_slotCount;                           // 0x160
	// 0 normal, 1 rare, 2 super rare, 3 all (ctor 3).
	int m_cardFilter;                          // 0x164
	UNKNOWN(0x168, 0x170);                     // JP sub-object, ctor 0x486700 (empty)
	CardComposeImageCache m_cardImages;        // 0x170
	FrCardComposeCheckDlg* m_pCheckDlg;        // 0x4a0 "card_compose_check"
	FrCardComposeResultDlg* m_pResultDlg;      // 0x4a4 "result_card_compose" (OpenResultDlg)
	// normal, rare, super rare, whole: indexed by m_cardFilter.
	FrButton* m_pFilterBtn[4];                 // 0x4a8
	const Bitmap* m_pSlotIcon;                 // 0x4b8 cardmix_btn_slot_my_o
	const Bitmap* m_pSlotIconDown;             // 0x4bc cardmix_btn_slot_my_d
	const Bitmap* m_pCardIcon;                 // 0x4c0 cardmix_btn_list_my_o, drawn by OnMyCardListOwnerDraw
	WTitleFont* m_pPriceFont;                  // 0x4c4 (0x48bb90, digits "1234567890,-")
	char m_ownPangText[0x20];                  // 0x4c8 MakePriceStrA(player pang)
	char m_priceText[0x20];                    // 0x4e8 MakePriceStrA(compose price)
	// The card image of a list row (0x48a260; size 21.5 x 26.5, 0x488e70).
	WRect m_cardImageRect;                     // 0x508
	// The row highlight m_pCardIcon (OnMyCardListOwnerDraw; size 0x489d30).
	WRect m_cardIconRect;                      // 0x518
	// m_pSlotIcon's size (0x489d80); x, y not traced.
	WRect m_slotIconRect;                      // 0x528
	// The cards by type id (find 0x48d4c0, insert 0x48d170; JP node 0x1d0,
	// head at +0x53c).
	std::map<unsigned int, CardComposeCard> m_cardMap; // 0x538
	UNKNOWN(0x544, 0x548);
	int m_listScrollPos;                       // 0x548
	// The result effect (SetResultSeq), "" (ctor).
	std::string m_resultSeq;                   // 0x54c
	UNKNOWN(0x568, 0x570);
};
CHECK_SIZE(FrCardComposeDlg, 0x570);

class CTaskMain;
class FrWnd;

// The JP CTaskMain's card compose task code (names ours; JpCardComposeDlg.cpp).
// The opener is the JP HandleMsgCommon's case 0x324 (0x65f34b, jump table
// 0x65f8c0 entry 13), the message FrAlchemyMPBreakDlg's CardCompose tab
// (0x4f6960) posts to the current task's main actor.
// 0x659490: service bit 32 closed -> the 0x2EE notice; else CloseAllDialogs,
// "card_compose" (0x654970, CreateForm<FrCardComposeDlg>) into the JP
// CTaskMain +0x18c, EnableDrag(false), Open(0x653ae0, 0x11), its icon rect
// from the JP CTaskMain +0x6c window, OnBarHoverOff(0).
void TaskMain_OpenCardComposeDlg(CTaskMain* task);

// The JP CTaskMain +0x18c read (no function of its own in the JP exe): the
// open card compose dialog, NULL when none.
FrCardComposeDlg* TaskMain_GetCardComposeDlg(const CTaskMain* task);

// The JP CTaskMain +0x6c (a window, NULL-tested; the card compose form
// zooms from its rect, JP FrWnd +0x34): KR m_barButton[0x13] (the JP keeps
// the KR bar button array at +0x20: the notice opener 0x659290 reads the
// KR m_barButton[0x12] at JP +0x68).
FrWnd* TaskMain_GetCardComposeIconWnd(const CTaskMain* task);

// The JP CTaskMain::HandleMsgCommon (0x65d8e0) cases of the compose answer:
// 0x65f357 (case 0x325, param1 the grade's address: read before the dialog
// test) SetResultSeq on the open dialog; 0x65f383 (case 0x326, param1 the
// new card's type id's address) SetResultCard, and when it took the id,
// OpenResultDlg.
void TaskMain_SetCardComposeResultSeq(CTaskMain* task, int grade);
void TaskMain_OnCardComposeResult(CTaskMain* task, unsigned int cardId);

#endif
