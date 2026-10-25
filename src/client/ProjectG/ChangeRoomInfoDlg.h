// Original: d:\build\custom\temp\code\client\projectg\changeroominfodlg.h
#ifndef PG_CLIENT_PROJECTG_CHANGEROOMINFODLG_H
#define PG_CLIENT_PROJECTG_CHANGEROOMINFODLG_H

#include "defines.h"
#include "padding.h"
#include "frform.h"
#include "globalgamedefine.h"
#include "MakeRoomDlg.h"

class Bitmap;
class FrArea;
class FrButton;
class FrEdit;
class FrViewer;
class FrWnd;
class FrTreasureCourse;
#ifdef JP_PAKS
class FrGroupBox; class FrStatic;
class WTitleFontEx;
#endif

class FrGraphicInterface;
class WRect;

// Room settings form of the room's owner: edits a copy of the current room
// (title, password, game type, players, holes, time, course, hole type).
// Its size (0x250) is the `new` of FrChangeRoomInfoDlgMakeInstance.
class FrChangeRoomInfoDlg : public FrForm {
	LAYOUT_CHECK_FRIEND;

public:
	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }   // changeroominfodlg.h:57

	FrChangeRoomInfoDlg(void);
	virtual ~FrChangeRoomInfoDlg(void);

	virtual bool OnInit(void);
	virtual void OnProc(const float dt);

private:
	static sFRESH_ENTRY _MsgEntries[];

protected:
	static sFRESH_MSGMAP _MsgMap;
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;

	void ChangeUserLimit(int userLimit);
	void ChangeHole(int hole);
	void ChangeTimeLimit(int timeLimit);
	void ChangeHoleType(eHoleType holeType);
	void ChangeCourse(int course);
	void ChangeGameType(int gameType, bool b1, bool b2);
	bool IsBtnEnable(void);

	void OnTextInit(intptr_t param);
	void OnTitleInit(intptr_t param);
	void OnPasswordInit(intptr_t param);
	void OnMapPrevInit(intptr_t param);
	void OnMapNextInit(intptr_t param);
	void OnGameTypeInit(intptr_t param);
	void OnUserLimitInit(intptr_t param);
	void OnHoleInit(intptr_t param);
	void OnTimeLimitInit(intptr_t param);
	void OnCourseInit(intptr_t param);
	void OnHoleTypeInit(intptr_t param);

	void OnGameTypeOwnerDraw(intptr_t param);
	void OnUserLimitOwnerDraw(intptr_t param);
	void OnHoleOwnerDraw(intptr_t param);
	void OnTimeLimitOwnerDraw(intptr_t param);
	void OnCourseOwnerDraw(PG_MSGARG param);
	void OnHoleTypeOwnerDraw(PG_MSGARG param);

	void DrawGameTypeButton(FrGraphicInterface* pDevice, int gameType, WRect rect, bool b);
	void DrawUserLimitButton(FrGraphicInterface* pDevice, int userLimit, WRect rect);
	void DrawHoleButton(FrGraphicInterface* pDevice, int hole, WRect rect);
	void DrawTimeLimitButton(FrGraphicInterface* pDevice, int timeLimit, WRect rect);
	void DrawHoleTypeButton(FrGraphicInterface* pDevice, eHoleType holeType, WRect rect, bool b);

	void OnOkBtnUp(void);
	void OnMapPrevUp(void);
	void OnMapNextUp(void);
	void OnCourseUp(void);
	bool OnMapSelectDlgResult(int result, FrForm* pForm);

	FrEdit* m_pTitle;                  // 0x110 (OnTitleInit)
	FrEdit* m_pPassword;               // 0x114 (OnPasswordInit)
	FrArea* m_pText;                   // 0x118 (OnTextInit)
	FrWnd* m_pCourseFrame;             // 0x11c, hidden with the course controls in a chat room (ChangeGameType)
	FrButton* m_pMapPrev;              // 0x120 (OnMapPrevInit)
	FrButton* m_pMapNext;              // 0x124 (OnMapNextInit)
	FrArea* m_pGameType;               // 0x128 (OnGameTypeInit)
	FrArea* m_pUserLimit;              // 0x12c (OnUserLimitInit)
	FrArea* m_pHole;                   // 0x130 (OnHoleInit)
	FrArea* m_pTimeLimit;              // 0x134 (OnTimeLimitInit)
	FrViewer* m_pCourse;               // 0x138 (OnCourseInit)
	FrArea* m_pHoleType;               // 0x13c (OnHoleTypeInit)
	const Bitmap* m_pSelectButtonImg[3];   // 0x140, ITEMS select_button_n/_o/_d
	const Bitmap* m_pHoleTypeImg[8];       // 0x14c, hole_front/back/random/shuffle, _n and _d each
	const Bitmap* m_pMapSelectImg;         // 0x16c, MAKEROOM map_select_o
	FrTreasureCourse* m_pMapSelectDlg; // 0x170, open course selection (OnCourseUp); blocks the button delay
	int m_textType;                    // 0x174, 0 text_vs, 1 text_mass, 2 text_vs, 3 text_chat (ChangeGameType)
	int m_gameType;                    // 0x178
	int m_userLimit;                   // 0x17c
	int m_timeLimit;                   // 0x180
	int m_hole;                        // 0x184
	int m_course;                      // 0x188
	unsigned char m_holeType;          // 0x18c, an eHoleType
	PADDING(0x18d, 0x190);         // room values copied by the ctor
	unsigned long m_gameTime;          // 0x190, sRoomInfo::m_value51
	unsigned long m_shotTime;          // 0x194, sRoomInfo::m_value4D, ms (40000 by default in stroke play)
	float m_btnDelay;                  // 0x198, 0.7 s countdown before the buttons enable
	sRoomInfo m_roomInfo;              // 0x19c, copy of the current room
#ifdef JP_PAKS
	// JP 983 only: the JP class differs; the port appends these (the comments give the JP offsets).
public:
	void SetSingleRoom(bool holeMode4, bool holecupFixed, WTitleFontEx* pFont);
protected:
	void OnSingleCtrlInit(intptr_t param); void OnRepeatInit(intptr_t param);
	void OnFixHoleInit(intptr_t param); void OnFixHoleUp(void); void OnFixHoleOwnerDraw(void);
	bool OnHoleSelectorResult(int result, FrForm* pForm); void SetRoomOptions(unsigned long options);
	void OnFixHolecupPosInit(intptr_t param); void OnFixHolecupPosUp(void); void UpdateFixHolecupPosImg(void);
	void OnMapCtrlInit(intptr_t param); void OnTextMapInit(intptr_t param); void OnClassicTypeInit(intptr_t param);
	void OnArtifactCtrlInit(intptr_t param); void OnArtiFrameInit(intptr_t param); void OnArtiIconInit(intptr_t param);
	void OnArtiBanInit(intptr_t param); void OnArtiNameInit(intptr_t param); void OnArtiDescInit(intptr_t param);
	void OnArtiPrevInit(intptr_t param); void OnArtiNextInit(intptr_t param); void OnClassicTypeUp(void);
	FrGroupBox* m_pSingleCtrl; FrGroupBox* m_pRepeat;    // JP +0x140 "cover"/"single_ctrl", +0x144 "single_ctrl.repeat"
	FrButton* m_pFixHole; FrButton* m_pFixHolecupPos;    // JP +0x148 ".repeat.fix_hole", +0x14c ".repeat.fix_holecup_pos"
	FrGroupBox* m_pMapCtrl; FrArea* m_pTextMap;          // JP +0x150 "map_ctrl", +0x154 "map_ctrl.text_map"
	FrGroupBox* m_pArtifactCtrl; FrArea* m_pArtiFrame;   // JP +0x158 "artifact_ctrl", +0x15c ".arti_frame"
	FrArea* m_pArtiIcon; FrArea* m_pArtiBan;             // JP +0x160 "artifact_ctrl.arti_icon", +0x164 ".arti_ban"
	FrStatic* m_pArtiName; FrStatic* m_pArtiDesc;        // JP +0x168 "artifact_ctrl.arti_name", +0x16c ".arti_desc"
	FrButton* m_pArtiPrev; FrButton* m_pArtiNext;        // JP +0x170 "artifact_ctrl.arti_prev", +0x174 ".arti_next"
	FrButton* m_pClassicType; unsigned long m_roomOptions;   // JP +0x178 "classictype_btn", +0x1d4 (bit 0: classic)
	unsigned char m_fixedHole; bool m_holecupPosFixed; bool m_isSingleRoom;   // JP +0x1d8, +0x1d9, +0x1f0
	WTitleFontEx* m_pSingleFont;       // JP +0x2d0, the room's digit font (fix_hole's owner draw)
#endif
};

CHECK_SIZE(FrChangeRoomInfoDlg, 0x250);
BEGIN_LAYOUT_CHECK(FrChangeRoomInfoDlg)
	CHECK_FIELD(FrChangeRoomInfoDlg, m_pTitle, 0x110);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_pPassword, 0x114);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_pText, 0x118);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_pCourseFrame, 0x11c);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_pMapPrev, 0x120);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_pMapNext, 0x124);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_pGameType, 0x128);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_pUserLimit, 0x12c);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_pHole, 0x130);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_pTimeLimit, 0x134);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_pCourse, 0x138);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_pHoleType, 0x13c);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_pSelectButtonImg, 0x140);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_pHoleTypeImg, 0x14c);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_pMapSelectImg, 0x16c);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_pMapSelectDlg, 0x170);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_textType, 0x174);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_gameType, 0x178);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_userLimit, 0x17c);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_timeLimit, 0x180);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_hole, 0x184);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_course, 0x188);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_holeType, 0x18c);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_gameTime, 0x190);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_shotTime, 0x194);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_btnDelay, 0x198);
	CHECK_FIELD(FrChangeRoomInfoDlg, m_roomInfo, 0x19c);
END_LAYOUT_CHECK

#endif // PG_CLIENT_PROJECTG_CHANGEROOMINFODLG_H
