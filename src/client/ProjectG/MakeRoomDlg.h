// Original: d:\build\custom\temp\code\client\projectg\makeroomdlg.h
#ifndef PG_CLIENT_PROJECTG_MAKEROOMDLG_H
#define PG_CLIENT_PROJECTG_MAKEROOMDLG_H

#include "defines.h"
#include "padding.h"
#include "frform.h"

class Bitmap;
class FrArea;
class FrButton;
class FrEdit;
class FrTabButton;
class FrViewer;
class FrGaugeBarImage;
class FrTreasureCourse;

class FrGraphicInterface;
class WRect;

// Hole type of a room (PDB enum name; values not transcribed yet).
enum eHoleType {
	eHoleType_None = 0
};

// Room creation form. Its size (0x1d0) is the `new` of
// FrMakeRoomDlgMakeInstance.
class FrMakeRoomDlg : public FrForm {
	LAYOUT_CHECK_FRIEND;

public:
	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }   // makeroomdlg.h:27

	FrMakeRoomDlg(void);
	virtual ~FrMakeRoomDlg(void);

	virtual bool OnInit(void);
	virtual void OnProc(const float dt);

	bool MakeRoom(bool b);

private:
	static sFRESH_ENTRY _MsgEntries[];

protected:
	static sFRESH_MSGMAP _MsgMap;
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;

	bool OnTitleEnterKey(PG_MSGARG param);
	void ChangeUserLimit(unsigned char userLimit);
	void ChangeHole(unsigned char hole);
	void ChangeTimeLimit(unsigned long timeLimit);
	void ChangeHoleType(eHoleType holeType);
	void ChangeCourse(unsigned char course);
	void ChangeGameType(unsigned char gameType, bool b);
	void SetDefaultTitle(void);
	bool IsBtnEnable(void);

	void OnHoleMinUp(void);
	void OnHolePrevUp(void);
	void OnHoleNextUp(void);
	void OnHoleMaxUp(void);
	void OnCourseUp(void);
	void OnCourseChatUp(void);
	void OnMapPrevUp(void);
	void OnMapNextUp(void);
	void OnGameTypeButtonUp(void);
	void OnUserLimitButtonUp(void);
	void OnHoleButtonUp(void);
	void OnTimeLimitButtonUp(void);
	void OnMakeVsDown(void);
	void OnMakeMassDown(void);
	void OnMakeBattleDown(void);
	void OnMakeChatDown(void);
	void OnMakeChaosDown(void);
	bool OnMapSelectDlgResult(int result, FrForm* pForm);
	void DrawHoleTypeButton(FrGraphicInterface* pDevice, eHoleType holeType, WRect rect, bool b);

	void OnMakeVsInit(PG_MSGARG param);
	void OnMakeMassInit(PG_MSGARG param);
	void OnMakeBattleInit(PG_MSGARG param);
	void OnMakeChatInit(PG_MSGARG param);
	void OnPictureInit(PG_MSGARG param);
	void OnTextInit(intptr_t param);
	void OnTitleInit(intptr_t param);
	void OnPasswordInit(intptr_t param);
	void OnMapPrevInit(intptr_t param);
	void OnMapNextInit(intptr_t param);
	void OnMapPrevChatInit(PG_MSGARG param);
	void OnMapNextChatInit(PG_MSGARG param);
	void OnGameTypeInit(intptr_t param);
	void OnUserLimitInit(intptr_t param);
	void OnHoleInit(intptr_t param);
	void OnTimeLimitInit(intptr_t param);
	void OnCourseInit(intptr_t param);
	void OnHoleTypeInit(intptr_t param);
	void OnCourseChatInit(PG_MSGARG param);
	void OnHoleChatInit(PG_MSGARG param);
	void OnHoleMinInit(PG_MSGARG param);
	void OnHolePrevInit(PG_MSGARG param);
	void OnHoleNextInit(PG_MSGARG param);
	void OnHoleMaxInit(PG_MSGARG param);
	void OnMakeChaosInit(PG_MSGARG param);
	void OnOverGauge_Init(PG_MSGARG param);
#ifdef JP_PAKS
	// JP 983 only (layout "makeroom" of data/ui/room.xml): the course
	// controls sit in the group box "map_ctrl" beside the artifact group
	// "artifact_ctrl". The JP make buttons (0xae1780 vs, 0xae17b0 mass,
	// 0xae17e0 battle, 0xae1800 chat, 0xae18c0 event) call
	// ShowCoursePanel after ChangeGameType.
	void OnMapCtrlInit(PG_MSGARG param);      // JP 0xade460
	void OnArtifactCtrlInit(PG_MSGARG param); // JP 0xade580
	void ShowCoursePanel(bool bVisible, bool bBesideArtifact); // JP 0xadf990
	// "make_event": shown and enabled only for an administrator (JP
	// CSharedDoc +0x812 & 4, the KR m_userInfo identity bit 4); LBUTTONDOWN
	// 0xae18c0 = ChangeGameType(0xb) + ShowCoursePanel(1, 0).
	void OnMakeEventInit(PG_MSGARG param);    // JP 0xadec20
	void OnMakeEventDown(void);               // JP 0xae18c0
	// "classictype_btn", the Natural Mode check (flag bit 0 of
	// m_naturalModeFlags, JP +0x20c).
	void OnClassicTypeInit(PG_MSGARG param);  // JP 0xade940
	void OnClassicTypeUp(void);             // JP 0xade9c0
	void SetNaturalModeFlags(unsigned int flags); // JP 0xadcf20
	void UpdateClassicTypeButton(void);       // JP 0xadced0
#endif
	void OnGameTypeOwnerDraw(intptr_t param);
	void OnUserLimitOwnerDraw(intptr_t param);
	void OnHoleOwnerDraw(intptr_t param);
	void OnTimeLimitOwnerDraw(intptr_t param);
	void OnHoleChatOwnerDraw(PG_MSGARG param);
	void OnCourseOwnerDraw(PG_MSGARG param);
	void OnHoleTypeOwnerDraw(PG_MSGARG param);

	FrButton* m_pMakeBtn[5];       // 0x110, vs, mass, battle, chat, chaos (OnMakeXxxInit)
	FrViewer* m_pPicture;          // 0x124
	FrEdit* m_pTitleEdit;          // 0x128, the room title (SetDefaultTitle)
	FrArea* m_pText;               // 0x12c
	PADDING(0x130, 0x138);
	FrButton* m_pMapPrev;          // 0x138
	FrButton* m_pMapNext;          // 0x13c
	FrButton* m_pMapPrevChat;      // 0x140
	FrButton* m_pMapNextChat;      // 0x144
	FrTabButton* m_pGameType;      // 0x148
	FrTabButton* m_pUserLimitTab;  // 0x14c
	FrTabButton* m_pHoleTab;       // 0x150
	FrTabButton* m_pTimeLimitTab;  // 0x154
	FrViewer* m_pCourse;           // 0x158
	FrArea* m_pHoleType;           // 0x15c
	FrViewer* m_pCourseChat;       // 0x160
	FrArea* m_pHoleChat;           // 0x164
	FrButton* m_pHoleMin;          // 0x168
	FrButton* m_pHolePrev;         // 0x16c
	FrButton* m_pHoleNext;         // 0x170
	FrButton* m_pHoleMax;          // 0x174
	FrGaugeBarImage* m_pOverGauge; // 0x178 (OnOverGauge_Init)
	const Bitmap* m_pHoleTypeImg[8];   // 0x17c, hole_front/back/random/shuffle, _n and _d each
	const Bitmap* m_pMapSelectImg; // 0x19c, MAKEROOM/map_select_o
	PADDING(0x1a0, 0x1a4);     // FrMakeRoomDlg members
	FrTreasureCourse* m_pMapSelectDlg; // 0x1a4, the open course picker (OnCourseUp); blocks the button delay
	int m_makeBtnIndex;            // 0x1a8, which m_pMakeBtn shows pressed (OnInit)
	unsigned char m_gameType;      // 0x1ac, 3 or 7 from the channel flags, else 0; GetDefaultTitle's argument
	unsigned char m_userLimit;     // 0x1ad
	PADDING(0x1ae, 0x1b0);
	unsigned long m_timeLimit;     // 0x1b0
	unsigned char m_hole;          // 0x1b4, hole count (18 limits the hole types)
	unsigned char m_course;        // 0x1b5, the selected course (ChangeCourse)
	unsigned char m_holeType;      // 0x1b6, an eHoleType
	PADDING(0x1b7, 0x1b8);
	int m_startHole;           // 0x1b8, hole picked by the min/prev/next/max buttons (1-18)
	unsigned long m_gameTime;      // 0x1bc, game time limit in ms sent by MakeRoom (mass games), cleared by OnInit
	unsigned long m_turnTime;      // 0x1c0, shot time limit in ms sent by MakeRoom (other games), cleared by OnInit
	float m_courseX;               // 0x1c4, m_pCourse's x at OnInit
	float m_btnDelay;              // 0x1c8, 0.7 s countdown before the buttons enable
	PADDING(0x1cc, 0x1d0);
#ifdef JP_PAKS
	// JP 983 members (the JP class is larger; port only, 64-bit layout).
	FrWnd* m_pMapCtrl;             // JP +0x180, "map_ctrl"
	FrWnd* m_pArtifactCtrl;        // JP +0x188, "artifact_ctrl"
	float m_mapCtrlOffsetX;        // JP global 0xf031f8: map_ctrl x - form x at its init
	float m_mapCtrlOffsetY;        // JP global 0xf031fc: map_ctrl y - form y at its init
	FrButton* m_pMakeEventBtn;     // JP +0x124, "make_event" (the JP's sixth make button)
	FrButton* m_pClassicTypeBtn;   // JP +0x15c, "classictype_btn"
	unsigned int m_naturalModeFlags; // JP +0x20c, bit 0: Natural Mode
#endif
};

#ifndef JP_PAKS
CHECK_SIZE(FrMakeRoomDlg, 0x1d0);
#endif
BEGIN_LAYOUT_CHECK(FrMakeRoomDlg)
	CHECK_FIELD(FrMakeRoomDlg, m_pMakeBtn, 0x110);
	CHECK_FIELD(FrMakeRoomDlg, m_pTitleEdit, 0x128);
	CHECK_FIELD(FrMakeRoomDlg, m_pPicture, 0x124);
	CHECK_FIELD(FrMakeRoomDlg, m_pText, 0x12c);
	CHECK_FIELD(FrMakeRoomDlg, m_pMapPrev, 0x138);
	CHECK_FIELD(FrMakeRoomDlg, m_pMapNext, 0x13c);
	CHECK_FIELD(FrMakeRoomDlg, m_pMapPrevChat, 0x140);
	CHECK_FIELD(FrMakeRoomDlg, m_pMapNextChat, 0x144);
	CHECK_FIELD(FrMakeRoomDlg, m_pGameType, 0x148);
	CHECK_FIELD(FrMakeRoomDlg, m_pUserLimitTab, 0x14c);
	CHECK_FIELD(FrMakeRoomDlg, m_pHoleTab, 0x150);
	CHECK_FIELD(FrMakeRoomDlg, m_pTimeLimitTab, 0x154);
	CHECK_FIELD(FrMakeRoomDlg, m_pCourse, 0x158);
	CHECK_FIELD(FrMakeRoomDlg, m_pHoleType, 0x15c);
	CHECK_FIELD(FrMakeRoomDlg, m_pCourseChat, 0x160);
	CHECK_FIELD(FrMakeRoomDlg, m_pHoleChat, 0x164);
	CHECK_FIELD(FrMakeRoomDlg, m_pHoleMin, 0x168);
	CHECK_FIELD(FrMakeRoomDlg, m_pHolePrev, 0x16c);
	CHECK_FIELD(FrMakeRoomDlg, m_pHoleNext, 0x170);
	CHECK_FIELD(FrMakeRoomDlg, m_pHoleMax, 0x174);
	CHECK_FIELD(FrMakeRoomDlg, m_pOverGauge, 0x178);
	CHECK_FIELD(FrMakeRoomDlg, m_pHoleTypeImg, 0x17c);
	CHECK_FIELD(FrMakeRoomDlg, m_gameType, 0x1ac);
	CHECK_FIELD(FrMakeRoomDlg, m_pMapSelectDlg, 0x1a4);
	CHECK_FIELD(FrMakeRoomDlg, m_pMapSelectImg, 0x19c);
	CHECK_FIELD(FrMakeRoomDlg, m_makeBtnIndex, 0x1a8);
	CHECK_FIELD(FrMakeRoomDlg, m_hole, 0x1b4);
	CHECK_FIELD(FrMakeRoomDlg, m_course, 0x1b5);
	CHECK_FIELD(FrMakeRoomDlg, m_gameTime, 0x1bc);
	CHECK_FIELD(FrMakeRoomDlg, m_turnTime, 0x1c0);
	CHECK_FIELD(FrMakeRoomDlg, m_courseX, 0x1c4);
	CHECK_FIELD(FrMakeRoomDlg, m_userLimit, 0x1ad);
	CHECK_FIELD(FrMakeRoomDlg, m_timeLimit, 0x1b0);
	CHECK_FIELD(FrMakeRoomDlg, m_holeType, 0x1b6);
	CHECK_FIELD(FrMakeRoomDlg, m_startHole, 0x1b8);
	CHECK_FIELD(FrMakeRoomDlg, m_btnDelay, 0x1c8);
END_LAYOUT_CHECK

#endif // PG_CLIENT_PROJECTG_MAKEROOMDLG_H
