// JP-only game code (no KR counterpart): the 2014 Arin birthday event
// screen (R558 screen #8, source run 0x5d8700-0x5dfa90; the event's handler
// _gateway::CArinBirthDay2014Handler is the run 0x477760-0x478cc2). Six
// classes, RTTI names read from their vtables' complete object locators:
// RF2014ArinBirthdayDlg (event_arinbirth_dialog.xml), the two stage windows
// RF2014ArinStage1Wnd (event_arinbirth_ticket_wnd.xml) and
// RF2014ArinStage2Wnd (event_arinbirth_magic_wnd.xml), and the popups
// RF2014ArinNotifyDlg, RF2014ArinMissionInfoDlg and
// RF2014ArinMapMissionInfoDlg (event_arinbirth_popup_1/2/3_dialog.xml).
// The handler opens the main dialog (0x478720: CreateArinBirthday2014Dlg,
// the event state, the stage from the player's event items, UpdateStage,
// centred, ShowModal 0x4783e0). Declarations and the JP x86 layout from the
// JP exe's bodies (node scripts/disasm.ts --jp 0x<va>); the two mission
// popups are declared with their constructor-proven shape only
// (temps/orchestrator/refresh_notes/JpArinEvent.md lists the next parts).
#ifndef PG_JP_JPARINEVENT_H
#define PG_JP_JPARINEVENT_H

#include <stdint.h>

#include <string>
#include <vector>

#include "defines.h"
#include "GatewayDoc.h"
#include "padding.h"
#include "RFButton.h"
#include "RFFormBase.h"
#include "RFFormModalBase.h"

class EventArgs;
class RFWnd;
class WReceivedPacket;

namespace _gateway {

// One of the event's missions (name ours), a 16-byte record of
// CArinBirthDay2014Doc::m_missions; RF2014ArinStage2Wnd keeps a copy
// (0x5dc540 reads the two flags).
struct ArinBirthday2014Mission {
	// 0x0: the first u32 of the mission's packet record (ReadState
	// 0x920cc0); no reader seen.
	uint32_t m_unread0;
	// 0x4: the progress; the info popup's body formats it (0x5db570), the
	// map mission (index 4) keeps one bit per course in it (0x5db950: 0x20
	// West Wiz, 0x40000 Wiz City, 0x200 White Wiz, 0x10 Wiz Wiz).
	uint32_t m_progress;
	int32_t m_goal;    // 0x8: the info popup's title and body format it (0x5db570)
	bool m_completed;  // 0xc: the mission's button hidden (0x5dc540)
	bool m_rewarded;   // 0xd: the reward taken (0x5dc540)
	PADDING(0xe, 0x10);
};
CHECK_SIZE(ArinBirthday2014Mission, 0x10);
BEGIN_LAYOUT_CHECK(ArinBirthday2014Mission)
	CHECK_FIELD(ArinBirthday2014Mission, m_progress, 0x4);
	CHECK_FIELD(ArinBirthday2014Mission, m_goal, 0x8);
	CHECK_FIELD(ArinBirthday2014Mission, m_completed, 0xc);
	CHECK_FIELD(ArinBirthday2014Mission, m_rewarded, 0xd);
END_LAYOUT_CHECK

// The event's gateway document, document type 0xB0 (RTTI
// _gateway::CArinBirthDay2014Doc, COL 0xd7cc38, vtable 0xd3c898: [0]
// deleting dtor 0x920b40, [1] Initialize 0x921360, [2] Release 0x920050, [3]
// Reset 0x4b5eb0 (the shared `return true`), [4] GetGatewayHandler
// 0x920ab0). Size 0x20: new in the factory creator 0x920be0 (registered by
// 0x920c70), ctor 0x920a90(type) keeps the type at +8 (JP container base:
// vtable +0, the task flags +4 stored by the actor's QueryHandler 0x91e850),
// the handler at +0xc, the missions at +0x10. It is
// CArinBirthDay2014Handler's document (JP +0x1c, JpArinEventHandler.h):
// the gateway registry row 0xB0 / message 0x753F / task flags 1 (the lobby
// task) routes the event's replies to the handler. The port derives from
// the KR CGatewayControlDoc like CQuestDocument (JpQuestForms.h); JP
// offsets are comments.
class CArinBirthDay2014Handler;

class CArinBirthDay2014Doc : public CGatewayControlDoc {
public:
	CArinBirthDay2014Doc(void);                                  // 0x920a90 (type 0xB0)
	virtual ~CArinBirthDay2014Doc(void);                         // [0] 0x920b40 (deleting)

	// Creates the handler (the factory creator 0x9211c0) into +0xc.
	virtual bool Initialize(void);                               // [1] 0x921360
	// Deletes the handler.
	virtual bool Release(void);                                  // [2] 0x920050
	virtual CGatewayHandler* GetGatewayHandler(void);            // [4] 0x920ab0

	// Replaces the missions with the packet's: a u32 count, then per
	// mission three u32 (+0x0, m_progress, m_goal) and two bytes (== 1:
	// m_completed, m_rewarded). The reply 0x027A (gateway key 1).
	void ReadState(WReceivedPacket& packet);                     // 0x920cc0

	CArinBirthDay2014Handler* m_pGatewayHandler;                 // JP +0xc
	std::vector<ArinBirthday2014Mission> m_missions;             // JP +0x10
};

// The container the KR CContentsDoc::Initialize inserts for type 0xB0
// under JP_PAKS (the JP inserts it through the creators run by 0x913d10).
IContentsDataContainer* JpCreateArinDocument(void);

// The event's main dialog (RTTI _gateway::RF2014ArinBirthdayDlg :
// RFFormModalBase<RFWndEx>, vtable 0xd11f0c, 34 slots: no new virtuals; the
// RFModalRenderer sub-object at +0x180 has vtable 0xd11f00), loaded from
// event_arinbirth_dialog.xml. Size 0x1a0. Its stage (0..9) picks what the
// dialog shows (UpdateStage 0x5dcdd0); btn_ok advances it.
class RF2014ArinBirthdayDlg : public RFFormModalBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5ddb40: RFWndEx::RFWndEx, RFModalRenderer (0x5e51f0),
	// m_updateRegistered cleared, the members cleared, the instance kept in
	// g_arinBirthday2014Dlg.
	RF2014ArinBirthdayDlg();
	// [1] 0x5de090 (scalar deleting destructor); ~RF2014ArinBirthdayDlg
	// 0x5ddbe0 clears g_arinBirthday2014Dlg, destroys m_texts (0x449190),
	// then restores RFFormModalBase<RFWndEx>'s vtables and runs
	// ~RFModalRenderer (0x5e5200) and ~RFWndEx.
	virtual ~RF2014ArinBirthdayDlg();

	// 0x5d81f0 (name ours): the handler's event state.
	void SetEventState(CArinBirthDay2014Doc* state);
	// 0x5d8200 (name ours).
	void SetStage(int stage);
	// 0x5dcdd0 (name ours): the text, pictures and stage windows of
	// m_stage (a 12-entry switch, 0..0xb).
	void UpdateStage();
	// 0x5da3f0 (name ours): text_area (RFTextArea) shows
	// Jp_LookupText(m_texts[index]) (the handler's key 4 shows text 10).
	void ShowText(int index);
	// 0x5dc280 (name ours): the notify popup with text 0x30e1e, centred,
	// ShowModal (the handler's key 4).
	void ShowRewardNotify();
	// 0x5dc2f0 (name ours): the notify popup with text 0x30e1f, centred,
	// ShowModal (the handler's key 5).
	void ShowFinalNotify();
	// 0x5dc850 (name ours): wnd_event_ctrl's wnd_event_magic_ctrl
	// (RF2014ArinStage2Wnd) MarkRewarded(reward) (the handler's key 4,
	// reward = the packet's value - 0x15).
	void ApplyReward(int reward);
	// 0x5dc970 (name ours): wnd_event_ctrl's wnd_event_magic_ctrl
	// SetStage(0xb), ApplyEventEnded (the handler's key 5).
	void ApplyFinalReward();

	// 0xe781d4 (name ours): the parent the last CreateArinBirthday2014Dlg
	// was given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5dee70: the three handlers, then LoadTexts.
	virtual void OnInitialUpdate();

	// 0x5d8210 (name ours), btn_event_page_out_n RFButton::EventClicked:
	// Close(2).
	void OnCloseClicked(EventArgs& args);
	// 0x5d86e0 (name ours), its own EventKeyDown: on Escape (key code
	// 0x1b) OnCloseClicked, args handled.
	void OnKeyDownForm(EventArgs& args);
	// 0x5dd210 (name ours), btn_ok RFButton::EventClicked: the next stage
	// (stage 4 sends the event request instead), then UpdateStage.
	void OnOkClicked(EventArgs& args);
	// 0x5de6b0 (name ours): m_texts cleared and refilled from the text
	// table (0x540a10), ids 0x30e08-0x30e14.
	void LoadTexts();
	// 0x5da550 (name ours): btn_ok (RFButton) shown or hidden.
	void ShowOkButton(bool show);
	// 0x5da620 (name ours): pic_arinbirth_bg = pic_arinbirth_bg_02.png.
	void SetSecondBackground();
	// 0x5da720 (name ours): pic_arinbirth_bg = pic_arinbirth_bg_01.png.
	void SetFirstBackground();
	// 0x5da820 (name ours): pic_char = pic_tiki_<index + 1>.png.
	void SetTikiPicture(int index);
	// 0x5da970 (name ours): pic_char = pic_arin_<index + 1>.png.
	void SetArinPicture(int index);

	// 0x0-0x184: RFFormModalBase<RFWndEx> (RFModalRenderer at 0x180,
	// m_updateRegistered at 0x184).
	CArinBirthDay2014Doc* m_eventState;  // 0x188 SetEventState
	int m_stage;                          // 0x18c SetStage; 0..9
	std::vector<std::string> m_texts;     // 0x190 LoadTexts
};
CHECK_SIZE(RF2014ArinBirthdayDlg, 0x1a0);
BEGIN_LAYOUT_CHECK(RF2014ArinBirthdayDlg)
	CHECK_FIELD(RF2014ArinBirthdayDlg, m_eventState, 0x188);
	CHECK_FIELD(RF2014ArinBirthdayDlg, m_stage, 0x18c);
	CHECK_FIELD(RF2014ArinBirthdayDlg, m_texts, 0x190);
END_LAYOUT_CHECK

// 0x5ddf50: the main dialog, made as g_jpModalCreateMode (0xe03260) says
// (CreateWnd<RF2014ArinBirthdayDlg> 0x5dde30, CreateDesktopWnd 0x5ddec0);
// the parent kept in RF2014ArinBirthdayDlg::s_loadParent.
RFWnd* CreateArinBirthday2014Dlg(RFWnd* parent);

// 0x5d9020 (name ours; declared only, part 2): what btn_ok does at stage 4;
// it ignores its this and builds a request from the game object 0xe73df0
// (0x9b79c0).
void SendArinBirthday2014Request();

// 0xd11768 (name ours): the five ticket items' type ids
// (0x1a0002bb..0x1a0002bf).
extern const uint32_t g_arinTicketItemTypeIds[5];

// 0xe781d8: the RF2014ArinBirthdayDlg instance.
extern RF2014ArinBirthdayDlg* g_arinBirthday2014Dlg;

class RF2014ArinStage1Wnd;
class RF2014ArinStage2Wnd;

// 0x5daac0: the ticket stage window, made as g_jpWndCreateMode (0xe73dd8)
// says (CreateWnd 0x5d89c0, CreateDesktopWnd 0x5d8a50); the parent kept in
// RF2014ArinStage1Wnd::s_loadParent.
RF2014ArinStage1Wnd* CreateArinStage1Wnd(RFWnd* parent);
// 0x5dc390: the magic stage window, the same way (CreateWnd 0x5dbfd0,
// CreateDesktopWnd 0x5dc060); the parent kept in
// RF2014ArinStage2Wnd::s_loadParent.
RF2014ArinStage2Wnd* CreateArinStage2Wnd(RFWnd* parent);

// The ticket stage window (RTTI _gateway::RF2014ArinStage1Wnd :
// RFFormBase<RFWndEx>, vtable 0xd11784), event_arinbirth_ticket_wnd.xml
// (created by UpdateStage at stage 2 inside wnd_event_ctrl). Size 0x1a0.
// Five tickets: btn_ticket_01..05 (RFButton), label_ticket_01..05 (RFLabel,
// the held count), pic_event_ticket1..5 and pic_event_ticket (RFPicture);
// the ticket items are g_arinTicketItemTypeIds.
class RF2014ArinStage1Wnd : public RFFormBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5d8700: RFWndEx::RFWndEx, then the five tickets' flags and counts
	// cleared (one loop; m_updateRegistered left as it is).
	RF2014ArinStage1Wnd();

	// 0xe781dc (name ours): the parent the last CreateArinStage1Wnd was
	// given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5dd2e0: the five tickets' EventLButtonDblClick handlers,
	// pic_event_ticket hidden, then per ticket its picture hidden, its
	// button enabled and its count shown from the held item, then
	// UseHeldTickets.
	virtual void OnInitialUpdate();

	// 0x5d8220..0x5d8260 (names ours): btn_ticket_01..05
	// RFWnd::EventLButtonDblClick; empty.
	void OnTicket1DblClick(EventArgs& args);
	void OnTicket2DblClick(EventArgs& args);
	void OnTicket3DblClick(EventArgs& args);
	void OnTicket4DblClick(EventArgs& args);
	void OnTicket5DblClick(EventArgs& args);
	// 0x5d99b0 (name ours): m_ticketCounts[index] = count, shown in
	// label_ticket_<index + 1> (RFLabel::SetLabel(count, false)).
	void SetTicketCount(int index, int count);
	// 0x5d9ae0 (name ours): pic_event_ticket<index + 1> shown or hidden.
	void ShowTicketPicture(int index, bool show);
	// 0x5dac00 (name ours): pic_event_ticket shown or hidden.
	void ShowTicketBase(bool show);
	// 0x5d8270 (name ours): m_ticketCounts[index] > 0.
	bool HasTicket(int index);
	// 0x5d8290 (name ours): with a count left, one taken and the ticket
	// marked used.
	void ConsumeTicket(int index);
	// 0x5d9100 (name ours): once all five tickets are used, main actor
	// message 1 and the request 0x0191 (no body).
	void OnAllTicketsUsed();
	// 0x5dc360 (name ours): with all five used, pic_event_ticket shown and
	// OnAllTicketsUsed.
	void CheckAllTicketsUsed();
	// 0x5dca90 (name ours): with a ticket held, one consumed, its count and
	// picture shown, then CheckAllTicketsUsed.
	void UseTicket(int index);
	// 0x5dcad0 (name ours): per ticket held, UseTicket; its button disabled
	// once none is left.
	void UseHeldTickets();

	// 0x0-0x184: RFFormBase<RFWndEx> (m_updateRegistered at 0x180).
	bool m_ticketUsed[5];    // 0x184 ConsumeTicket, CheckAllTicketsUsed
	PADDING(0x189, 0x18c);
	int m_ticketCounts[5];   // 0x18c the held count of each ticket item (SetTicketCount)
};
CHECK_SIZE(RF2014ArinStage1Wnd, 0x1a0);
BEGIN_LAYOUT_CHECK(RF2014ArinStage1Wnd)
	CHECK_FIELD(RF2014ArinStage1Wnd, m_ticketUsed, 0x184);
	CHECK_FIELD(RF2014ArinStage1Wnd, m_ticketCounts, 0x18c);
END_LAYOUT_CHECK

// The magic stage window (RTTI _gateway::RF2014ArinStage2Wnd :
// RFFormBase<RFWndEx>, vtable 0xd11ac4), event_arinbirth_magic_wnd.xml
// (created by UpdateStage at stages 9 and 0xb inside wnd_event_ctrl). Size
// 0x1a0. One btn_mission_0n / btn_reward_0n pair per mission, btn_final,
// pic_final_1/2, pic_event_magic_bg (pic_event_magic_bg_%02d.png).
class RF2014ArinStage2Wnd : public RFFormBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5dacd0: RFWndEx::RFWndEx, then the members cleared. Its
	// destructor is 0x5dad10.
	RF2014ArinStage2Wnd();

	// 0x5d82c0 (name ours): the main dialog's stage (9 or 0xb).
	void SetStage(int stage);
	// 0x5d82d0 (name ours): how many of m_rewarded are set.
	int CountRewarded();
	// 0x5dcd40 (name ours): m_missions = missions (0x5dbee0, the vector's
	// operator=), then RefreshMissions, SetMagicBackground(CountRewarded())
	// and ApplyEventEnded.
	void SetMissions(std::vector<ArinBirthday2014Mission> const* missions);

	// 0x5dc600 (name ours): m_rewarded[index] set, its reward button
	// disabled, btn_final and the final pictures from
	// PlayFinalIfAllRewarded, SetMagicBackground(CountRewarded()).
	void MarkRewarded(int index);
	// 0x5dc650 (name ours): at stage 0xb the missions and the final
	// controls hidden, pic_event_magic_bg_05.png, the animation stopped.
	void ApplyEventEnded();

	// 0xe781e4 (name ours): the parent the last CreateArinStage2Wnd was
	// given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5df230: btn_mission_01..05 -> OnMission1..5Clicked,
	// btn_reward_01..05 -> OnReward1..5Clicked, btn_final ->
	// OnFinalClicked (RFButton::EventClicked).
	virtual void OnInitialUpdate();

	// 0x5d91b0 (name ours): a completed mission whose reward is not taken:
	// main actor message 1 and the request 0x0193 (u32 index + 0x15).
	void RequestReward(int index);
	// 0x5d9290 (name ours): main actor message 1 and the request 0x0194
	// (no body).
	void RequestFinalReward();
	// 0x5d97f0..0x5d9830 (names ours): RequestReward(0..4).
	void OnReward1Clicked(EventArgs& args);
	void OnReward2Clicked(EventArgs& args);
	void OnReward3Clicked(EventArgs& args);
	void OnReward4Clicked(EventArgs& args);
	void OnReward5Clicked(EventArgs& args);
	// 0x5d9840 (name ours): RequestFinalReward.
	void OnFinalClicked(EventArgs& args);
	// 0x5df110 (name ours): with five missions, mission index neither
	// completed nor rewarded: the mission info popup, SetMission, centred,
	// ShowModal.
	void ShowMissionInfo(int index);
	// 0x5df180 (name ours): with five missions, the map mission popup,
	// SetMission(&m_missions[index]), centred, ShowModal.
	void ShowMapMissionInfo(int index);
	// 0x5df1e0..0x5df210 (names ours): ShowMissionInfo(0..3); 0x5df220:
	// ShowMapMissionInfo(4).
	void OnMission1Clicked(EventArgs& args);
	void OnMission2Clicked(EventArgs& args);
	void OnMission3Clicked(EventArgs& args);
	void OnMission4Clicked(EventArgs& args);
	void OnMission5Clicked(EventArgs& args);

	// 0x5d9c00 (name ours): btn_mission_<index + 1> (%02d, the name
	// through Jp_LookupText) shown or hidden.
	void ShowMissionButton(int index, bool show);
	// 0x5d9d20 (name ours): btn_reward_<index + 1> shown or hidden.
	void ShowRewardButton(int index, bool show);
	// 0x5d9f80 (name ours): btn_reward_<index + 1>'s RFButton::SetState.
	void SetRewardState(int index, RFButton::RFBUTTON_STATE state);
	// 0x5dadb0 (name ours): pic_event_magic_bg =
	// pic_event_magic_bg_<count>.png (%02d).
	void SetMagicBackground(int count);
	// 0x5daf00 (name ours): btn_final shown or hidden.
	void ShowFinalButton(bool show);
	// 0x5dafd0 (name ours): pic_final_1 and pic_final_2 shown or hidden.
	void ShowFinalPictures(bool show);
	// 0x5db110 (name ours): outside stage 0xb, with all five rewards
	// taken, plays "_Final_" and answers true.
	bool PlayFinalIfAllRewarded();
	// 0x5dc500 (name ours): every mission, reward and final control shown
	// or hidden.
	void ShowAll(bool show);
	// 0x5dc540 (name ours): per mission, its mission or reward button from
	// m_completed / m_rewarded (m_rewarded[i] kept), then btn_final and the
	// final pictures from PlayFinalIfAllRewarded.
	void RefreshMissions();

	// 0x0-0x184: RFFormBase<RFWndEx> (m_updateRegistered at 0x180).
	std::vector<ArinBirthday2014Mission> m_missions;  // 0x184 SetMissions
	int m_stage;                                       // 0x194 SetStage
	bool m_rewarded[5];                                // 0x198 RefreshMissions, CountRewarded
	bool m_clearedByCtor;                              // 0x19d only the ctor writes it (no reader in the window's run yet)
	PADDING(0x19e, 0x1a0);
};
CHECK_SIZE(RF2014ArinStage2Wnd, 0x1a0);
BEGIN_LAYOUT_CHECK(RF2014ArinStage2Wnd)
	CHECK_FIELD(RF2014ArinStage2Wnd, m_missions, 0x184);
	CHECK_FIELD(RF2014ArinStage2Wnd, m_stage, 0x194);
	CHECK_FIELD(RF2014ArinStage2Wnd, m_rewarded, 0x198);
	CHECK_FIELD(RF2014ArinStage2Wnd, m_clearedByCtor, 0x19d);
END_LAYOUT_CHECK

// The first popup (RTTI _gateway::RF2014ArinNotifyDlg :
// RFFormModalBase<RFWndEx>, vtable 0xd1181c, renderer vtable 0xd11810),
// event_arinbirth_popup_1_dialog.xml (creator 0x5db1d0, instance 0xe781f0;
// opened by the handler 0x477c30/0x477c80). Size 0x188: no members of its
// own.
class RF2014ArinNotifyDlg : public RFFormModalBase<RFWndEx> {
public:
	// 0x5d8750: RFWndEx::RFWndEx, RFModalRenderer (0x5e51f0),
	// m_updateRegistered cleared, the instance kept in g_arinNotifyDlg.
	RF2014ArinNotifyDlg();

	// 0xe781ec (name ours): the parent the last CreateArinNotifyDlg was
	// given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5db470: btn_evert_arinbirth_ckeck (sic) RFButton::EventClicked
	// -> OnCheckClicked.
	virtual void OnInitialUpdate();

	// 0x5d8310 (name ours): Close(2).
	void OnCheckClicked(EventArgs& args);

public:
	// 0x5db310 (name ours): the TextArea child shows the text table entry
	// `text` names (Jp_LookupText; a std::string by value).
	void SetText(std::string text);
};
CHECK_SIZE(RF2014ArinNotifyDlg, 0x188);

// 0x5db1d0: the first popup, made as g_jpModalCreateMode (0xe03260) says
// (CreateWnd 0x5d8ae0, CreateDesktopWnd 0x5d8b70); the parent kept in
// RF2014ArinNotifyDlg::s_loadParent.
RFWnd* CreateArinNotifyDlg(RFWnd* parent);

// 0xe781f0: the RF2014ArinNotifyDlg instance.
extern RF2014ArinNotifyDlg* g_arinNotifyDlg;

// One text pair of the mission popups (name ours, 0x54 bytes; the
// popups' constructors fill it from the text table 0x540a10, the vector's
// push_back is 0x5dddb0, the local's destructor 0x5d9880).
struct ArinBirthday2014MissionText {
	std::string m_title;     // 0x0 the Label's format (0x5db570) or text (0x5db950)
	std::string m_body;      // 0x1c the TextArea's format (0x5db570)
	std::string m_reserved;  // 0x38 left empty by both constructors; no reader seen
};

// The second popup (RTTI _gateway::RF2014ArinMissionInfoDlg :
// RFFormModalBase<RFWndEx>, vtable 0xd11fa4, renderer vtable 0xd11f98),
// event_arinbirth_popup_2_dialog.xml (creator 0x5debf0, instance 0xe781f8;
// opened by RF2014ArinStage2Wnd::ShowMissionInfo). Size 0x198.
class RF2014ArinMissionInfoDlg : public RFFormModalBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5de0c0: RFWndEx::RFWndEx, RFModalRenderer (0x5e51f0),
	// m_updateRegistered cleared, the instance kept in
	// g_arinMissionInfoDlg, then the four texts of missions 0..3 (ids
	// 0x30e15..0x30e1c, title and body each).
	RF2014ArinMissionInfoDlg();
	// 0x5ddc70: g_arinMissionInfoDlg cleared, m_texts freed (0x5dd950).
	virtual ~RF2014ArinMissionInfoDlg();

	// 0x5db570 (name ours): Label = Jp_LookupText(MakeStr(title, goal)),
	// TextArea = Jp_LookupText(MakeStr(body, progress, goal)).
	void SetMission(int index, const ArinBirthday2014Mission* mission);

	// 0xe781f4 (name ours): the parent the last CreateArinMissionInfoDlg
	// was given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5db850: btn_evert_arinbirth_ckeck -> OnCheckClicked.
	virtual void OnInitialUpdate();
	// 0x5d8320 (name ours): Close(2).
	void OnCheckClicked(EventArgs& args);

	// 0x0-0x188: RFFormModalBase<RFWndEx>.
	std::vector<ArinBirthday2014MissionText> m_texts;  // 0x188
	PADDING(0x194, 0x198);
};
CHECK_SIZE(RF2014ArinMissionInfoDlg, 0x198);
BEGIN_LAYOUT_CHECK(RF2014ArinMissionInfoDlg)
	CHECK_FIELD(RF2014ArinMissionInfoDlg, m_texts, 0x188);
END_LAYOUT_CHECK

// 0x5debf0: the mission info popup, made as g_jpModalCreateMode says
// (CreateWnd 0x5de470, CreateDesktopWnd 0x5de500); the parent kept in
// RF2014ArinMissionInfoDlg::s_loadParent.
RF2014ArinMissionInfoDlg* CreateArinMissionInfoDlg(RFWnd* parent);

// 0xe781f8: the RF2014ArinMissionInfoDlg instance.
extern RF2014ArinMissionInfoDlg* g_arinMissionInfoDlg;

// The third popup (RTTI _gateway::RF2014ArinMapMissionInfoDlg :
// RFFormModalBase<RFWndEx>, vtable 0xd1203c, renderer vtable 0xd12030),
// event_arinbirth_popup_3_dialog.xml (creator 0x5ded30, instance 0xe78200;
// opened by RF2014ArinStage2Wnd::ShowMapMissionInfo). Size 0x198.
class RF2014ArinMapMissionInfoDlg : public RFFormModalBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5de330: as RF2014ArinMissionInfoDlg's, one text (0x30e1d, empty
	// body); the instance kept in g_arinMapMissionInfoDlg.
	RF2014ArinMapMissionInfoDlg();
	// 0x5ddd10: g_arinMapMissionInfoDlg cleared, m_texts freed.
	virtual ~RF2014ArinMapMissionInfoDlg();

	// 0x5db950 (name ours): Label = Jp_LookupText(m_texts[0].m_title),
	// then each visited course's picture set to its _o.png.
	void SetMission(const ArinBirthday2014Mission* mission);

	// 0xe781fc (name ours): the parent the last CreateArinMapMissionInfoDlg
	// was given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5dbde0: btn_evert_arinbirth_ckeck -> OnCheckClicked.
	virtual void OnInitialUpdate();
	// 0x5d8330 (name ours): Close(2).
	void OnCheckClicked(EventArgs& args);
	// (0x5db950's four course checks, one helper of ours)
	void SetCoursePicture(const wchar_t* name, const wchar_t* picture);

	// 0x0-0x188: RFFormModalBase<RFWndEx>.
	std::vector<ArinBirthday2014MissionText> m_texts;  // 0x188
	PADDING(0x194, 0x198);
};
CHECK_SIZE(RF2014ArinMapMissionInfoDlg, 0x198);
BEGIN_LAYOUT_CHECK(RF2014ArinMapMissionInfoDlg)
	CHECK_FIELD(RF2014ArinMapMissionInfoDlg, m_texts, 0x188);
END_LAYOUT_CHECK

// 0x5ded30: the map mission popup, the same way (g_jpModalCreateMode).
RF2014ArinMapMissionInfoDlg* CreateArinMapMissionInfoDlg(RFWnd* parent);

// 0xe78200: the RF2014ArinMapMissionInfoDlg instance.
extern RF2014ArinMapMissionInfoDlg* g_arinMapMissionInfoDlg;

} // namespace _gateway

#endif // PG_JP_JPARINEVENT_H
