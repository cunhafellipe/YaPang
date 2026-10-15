// Original: d:\build\custom\temp\code\client\projectg\gui_control.h
#ifndef PG_CLIENT_PROJECTG_GUI_CONTROL_H
#define PG_CLIENT_PROJECTG_GUI_CONTROL_H

#include "defines.h"
#include "padding.h"
#include "frform.h"
#include "TaskMain.h"
#include "wlist.h"
#include "woverlay.h"
#include "wresrcmng.h"
#include <bitset>

class Bitmap;
class CAccuracyInfo;
class CAuxMenuUI;
class CBase;
class CCalipers;
class CClubTypeInfo;
class CExtraHoleInfo;
class CFontOverlay;
class CGuildInfo;
class CHolePoseSkins;
class CMiniScore;
class CPangInfo;
class CPowerInfo;
class CRemainTimeInfo;
class CShotCountInfo;
class CSimpleUI;
class CWindInfo;
class FrButton;
class FrChatFuncDlg;
class FrReportDlg;
class IObject;
class WFont;
class WOverlay;
class WRect;
class WPuppet;
class WTitleFont;

// The round screen (HUD) task; its size (0x40C) is the `new` of
// CScreenMakeInstance, a friend (the constructor is protected). Only the
// members this unit defines so far are named.
class CScreen : public CTaskMain {
	LAYOUT_CHECK_FRIEND;
	friend IObject* CScreenMakeInstance(void);
public:
	virtual ~CScreen(void) {}   // gui_control.h:29

	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }   // gui_control.h:31

	// A named overlay of the screen's overlay list (AddOverlay).
	struct sOverlay {
		sOverlay(void) {   // gui_control.h:104
			m_pOverlay = NULL;
		}

		~sOverlay(void) {   // gui_control.h:109
			if (g_resrcmng && m_pOverlay) { g_resrcmng->Release(m_pOverlay); m_pOverlay = 0; }
		}

		char m_name[0x20];      // 0x00
		WOverlay* m_pOverlay;   // 0x20
	};

	virtual void OnDestroy(void);
	virtual void OnPreLoadInit(void);
	virtual void OnLoad(void);
	virtual void OnInit(void);
	virtual void OnProcess(float elapsed);
	virtual void OnDisplay(void);

protected:
	CScreen(void);

	void AddOverlay(const char* name, const char* file, int flags);
	void DrawOverlay(const char* name, const WRect& rect, unsigned char alpha, int flags);
	void DrawOverlay2(const char* name, const WRect& rect, int color, int flags);

private:
	void DrawClubInfo(void);
	void DrawHitBarInfo(void);
	void DrawCammodeInfo(void);
	void DrawWindInfo(void);
	void DrawApproachInfo(void);
	void DrawRemainTime(void);
	void DrawClubBallUI(void);
	void DrawCommandArrow(void);
	void DrawTutorialUI(void);
	bool OnTrainingOptionDlgResult(int ret, FrForm* pForm);

protected:
	virtual void HandleMsg(const MsgObject& msg);

private:
	CFontOverlay* m_pFontOverlay;  // 0x1E0, OnLoad / OnDestroy
	CBase* m_pBase;                // 0x1E4, OnLoad / OnDestroy
	CWindInfo* m_pWindInfo;        // 0x1E8
	CRemainTimeInfo* m_pRemainTimeInfo; // 0x1EC
	CClubTypeInfo* m_pClubTypeInfo; // 0x1F0
	CPowerInfo* m_pPowerInfo;      // 0x1F4
	CAccuracyInfo* m_pAccuracyInfo; // 0x1F8
	WPuppet* m_pWindPuppet;        // 0x1FC, drawn by CWind::Display
	CMiniScore* m_pMiniScore;      // 0x200
	CAuxMenuUI* m_pAuxMenuUI;      // 0x204
	CPangInfo* m_pPangInfo;        // 0x208
	CHolePoseSkins* m_pHolePoseSkins; // 0x20C
	CExtraHoleInfo* m_pExtraHoleInfo; // 0x210
	CGuildInfo* m_pGuildInfo;      // 0x214, HandleMsg SetScore
	CSimpleUI* m_pCammodeUI;       // 0x218, DrawCammodeInfo
	CSimpleUI* m_pSimpleUI21C;     // 0x21C, created by OnLoad
	CSimpleUI* m_pSimpleUI220;     // 0x220, created by OnLoad
	CSimpleUI* m_pTutoTitleUI;     // 0x224, created by OnLoad; DrawTutorialUI's title
	CSimpleUI* m_pCommandArrowUI;  // 0x228, DrawCommandArrow
	CSimpleUI* m_pTutoEscMsgUI;    // 0x22C, [tuto_esc_msg2.png (OnLoad)
	CSimpleUI* m_pTutorialUIs[12]; // 0x230, DrawTutorialUI renders MissionTitle from one
	WTitleFont* m_pTitleFont;      // 0x260, [font_wind.jpg digits (DrawApproachInfo)
	CSimpleUI* m_pMissionUI;       // 0x264, text_mission.png (DrawApproachInfo)
	float m_missionTitleAlpha;     // 0x268, alpha of DrawTutorialUI's MissionTitle (1 at construction)
	float m_accumValue;            // 0x26C, HandleMsg sets or adds a message's float and copies it on
	float m_fadeValue;             // 0x270, HandleMsg sets 1, OnProcess runs it down
	WList<sOverlay*> m_overlays;   // 0x274
	WFont* m_pFont;                // 0x294, CChatMsg's masked font (OnLoad)
	float m_remainTimeArcShown;    // 0x298, DrawRemainTime copies m_remainTimeArc while not captured
	float m_remainTimeArc;         // 0x29C, start angle of the remaining-time arc
	unsigned long m_remainTimeColor; // 0x2A0
	bool m_hudVisible;             // 0x2A4, OnDisplay draws the HUD while set
	PADDING(0x2A5, 0x2A8);
	unsigned long m_hitBarColor;   // 0x2A8, DrawHitBarInfo's bar background colour mask
	int m_msgColorTime;            // 0x2AC, set by HandleMsg
	unsigned long m_msgColor;      // 0x2B0, alpha-faded colour set by HandleMsg
	PADDING(0x2B4, 0x2B8);
	int m_hitBarFrom;              // 0x2B8, DrawHitBarInfo range start
	int m_hitBarCount;             // 0x2BC, DrawHitBarInfo scale count (10)
	unsigned long m_hitBarRangeColor; // 0x2C0, DrawHitBarInfo's range overlay colour mask
	float m_hitBarTime;            // 0x2C4, DrawHitBarInfo / OnProcess
	float m_remainTimeY;           // 0x2C8, the remaining-time circle's y
	// A scale mark of the big hit bar (DrawHitBarInfo draws entries 1..m_hitBarCount).
	struct sHitMark {
		float y;                   // 0x0
		unsigned long color;       // 0x4
	};
	sHitMark m_hitMarks[11];       // 0x2CC
	PADDING(0x324, 0x374);
	float m_blinkTime;             // 0x374, OnProcess adds the elapsed time (0.25 s period)
	std::bitset<2> m_drawFlags;    // 0x378: bit 0 = draw the mission mark, bit 1 = shot input enabled (OnInit sets it)
	float m_cammodeAlpha;          // 0x37C, alpha of the camera mode text
	float m_ballStateCut;          // 0x380, top fraction of the ball-state icon left out (DrawClubBallUI)
	bool m_missionMarkShown[0x10]; // 0x384, per tutorial game state; OnProcess blinks the current one (DrawClubInfo, OnDisplay)
	unsigned char m_tutoMsgBlinkCount; // 0x394, OnProcess blinks m_tutoMsgShown twice
	unsigned char m_tutoMsgShown;  // 0x395, 0/1 phase of the tutorial message (DrawTutorialUI)
	PADDING(0x396, 0x398);
	float m_tutoMsgBlinkPeriod;    // 0x398, set by OnLoad
	float m_tutoMsgBlinkTime;      // 0x39C
	unsigned char m_missionMarkBlinkCount; // 0x3A0, OnProcess blinks the mission mark up to 3 times
	PADDING(0x3A1, 0x3A4);
	float m_missionMarkBlinkPeriod; // 0x3A4, set by OnLoad
	float m_missionMarkBlinkTime;  // 0x3A8
	int m_selectedIndex;           // 0x3AC, -1 after OnInit (HandleMsg)
	float m_bannerTime;            // 0x3B0, OnDisplay shows a banner while positive
	int m_tutoTitleState;          // 0x3B4, OnProcess: 0 fade in, 1 hold, 2 done
	unsigned char m_tutoTitleAlpha; // 0x3B8, alpha of the tutorial title (DrawTutorialUI)
	PADDING(0x3B9, 0x3BC);
	float m_tutoTitleX;            // 0x3BC
	float m_tutoTitleY;            // 0x3C0
	float m_tutoTitleTime;         // 0x3C4
	int m_missionTitleState;       // 0x3C8, OnProcess: 0, 1, 2; 3 hides the MissionTitle
	float m_missionTitleX;         // 0x3CC
	float m_missionTitleY;         // 0x3D0
	float m_missionTitleTime;      // 0x3D4, cleared by the constructor and by OnLoad's tutorial setup
	int m_tutorialNo;              // 0x3D8, 0-based; DrawTutorialUI shows title _<n + 1>
	float m_turnStartTime;         // 0x3DC, seconds since the turn started: cleared by HandleMsg (game state 0xC), OnProcess re-enables cursor area 0x21 after 5 s
	bool m_turnStarted;            // 0x3E0, HandleMsg sets it in game state 0xC
	bool m_unknown3E1;             // 0x3E1, set by OnPreLoadInit
	bool m_unknown3E2;             // 0x3E2, cleared by OnPreLoadInit
	bool m_unknown3E3;             // 0x3E3, cleared by OnPreLoadInit
	unsigned long m_bannerGameMode; // 0x3E4, CGolfDoc::m_gameMode when OnProcess last restarted the banner
	float m_narrowFlicker[3];      // 0x3E8, 255 each; OnProcess re-rolls them (225..255) while the bar is inside the narrow accuracy
	float m_clubAngle;             // 0x3F4, the club preview's rotation (2 pi at OnInit; DrawClubInfo)
	int m_clubSpinDir;             // 0x3F8, OnProcess: 1, -1 or 0
	float m_clubSwing;             // 0x3FC, the club preview's swing phase (DrawClubInfo)
	CCalipers* m_pCalipers;        // 0x400
	int m_clubBoundingBox;         // 0x404, the club puppet's bounding-box script handle
	unsigned char m_playerCount;   // 0x408, set by HandleMsg, looped over by OnDisplay
	PADDING(0x409, 0x40C);
};

CHECK_SIZE(CScreen::sOverlay, 0x24);
CHECK_SIZE(CScreen, 0x40C);
BEGIN_LAYOUT_CHECK(CScreen)
	CHECK_FIELD(CScreen, m_pFontOverlay, 0x1E0);
	CHECK_FIELD(CScreen, m_pBase, 0x1E4);
	CHECK_FIELD(CScreen, m_pWindInfo, 0x1E8);
	CHECK_FIELD(CScreen, m_pRemainTimeInfo, 0x1EC);
	CHECK_FIELD(CScreen, m_pClubTypeInfo, 0x1F0);
	CHECK_FIELD(CScreen, m_pPowerInfo, 0x1F4);
	CHECK_FIELD(CScreen, m_pAccuracyInfo, 0x1F8);
	CHECK_FIELD(CScreen, m_pWindPuppet, 0x1FC);
	CHECK_FIELD(CScreen, m_pMiniScore, 0x200);
	CHECK_FIELD(CScreen, m_pAuxMenuUI, 0x204);
	CHECK_FIELD(CScreen, m_pPangInfo, 0x208);
	CHECK_FIELD(CScreen, m_pHolePoseSkins, 0x20C);
	CHECK_FIELD(CScreen, m_pExtraHoleInfo, 0x210);
	CHECK_FIELD(CScreen, m_pGuildInfo, 0x214);
	CHECK_FIELD(CScreen, m_pCammodeUI, 0x218);
	CHECK_FIELD(CScreen, m_pSimpleUI21C, 0x21C);
	CHECK_FIELD(CScreen, m_pSimpleUI220, 0x220);
	CHECK_FIELD(CScreen, m_pTutoTitleUI, 0x224);
	CHECK_FIELD(CScreen, m_pCommandArrowUI, 0x228);
	CHECK_FIELD(CScreen, m_pTutoEscMsgUI, 0x22C);
	CHECK_FIELD(CScreen, m_pTutorialUIs, 0x230);
	CHECK_FIELD(CScreen, m_pTitleFont, 0x260);
	CHECK_FIELD(CScreen, m_pMissionUI, 0x264);
	CHECK_FIELD(CScreen, m_missionTitleAlpha, 0x268);
	CHECK_FIELD(CScreen, m_accumValue, 0x26C);
	CHECK_FIELD(CScreen, m_ballStateCut, 0x380);
	CHECK_FIELD(CScreen, m_fadeValue, 0x270);
	CHECK_FIELD(CScreen, m_overlays, 0x274);
	CHECK_FIELD(CScreen, m_pFont, 0x294);
	CHECK_FIELD(CScreen, m_remainTimeArcShown, 0x298);
	CHECK_FIELD(CScreen, m_remainTimeArc, 0x29C);
	CHECK_FIELD(CScreen, m_remainTimeColor, 0x2A0);
	CHECK_FIELD(CScreen, m_hudVisible, 0x2A4);
	CHECK_FIELD(CScreen, m_msgColorTime, 0x2AC);
	CHECK_FIELD(CScreen, m_msgColor, 0x2B0);
	CHECK_FIELD(CScreen, m_hitBarFrom, 0x2B8);
	CHECK_FIELD(CScreen, m_hitBarCount, 0x2BC);
	CHECK_FIELD(CScreen, m_hitBarTime, 0x2C4);
	CHECK_FIELD(CScreen, m_hitBarColor, 0x2A8);
	CHECK_FIELD(CScreen, m_hitBarRangeColor, 0x2C0);
	CHECK_FIELD(CScreen, m_hitMarks, 0x2CC);
	CHECK_FIELD(CScreen, m_remainTimeY, 0x2C8);
	CHECK_FIELD(CScreen, m_blinkTime, 0x374);
	CHECK_FIELD(CScreen, m_drawFlags, 0x378);
	CHECK_FIELD(CScreen, m_cammodeAlpha, 0x37C);
	CHECK_FIELD(CScreen, m_selectedIndex, 0x3AC);
	CHECK_FIELD(CScreen, m_bannerTime, 0x3B0);
	CHECK_FIELD(CScreen, m_missionMarkShown, 0x384);
	CHECK_FIELD(CScreen, m_tutoMsgBlinkCount, 0x394);
	CHECK_FIELD(CScreen, m_tutoMsgShown, 0x395);
	CHECK_FIELD(CScreen, m_tutoMsgBlinkPeriod, 0x398);
	CHECK_FIELD(CScreen, m_tutoMsgBlinkTime, 0x39C);
	CHECK_FIELD(CScreen, m_missionMarkBlinkCount, 0x3A0);
	CHECK_FIELD(CScreen, m_missionMarkBlinkPeriod, 0x3A4);
	CHECK_FIELD(CScreen, m_missionMarkBlinkTime, 0x3A8);
	CHECK_FIELD(CScreen, m_tutoTitleState, 0x3B4);
	CHECK_FIELD(CScreen, m_tutoTitleAlpha, 0x3B8);
	CHECK_FIELD(CScreen, m_tutoTitleX, 0x3BC);
	CHECK_FIELD(CScreen, m_tutoTitleY, 0x3C0);
	CHECK_FIELD(CScreen, m_tutoTitleTime, 0x3C4);
	CHECK_FIELD(CScreen, m_missionTitleState, 0x3C8);
	CHECK_FIELD(CScreen, m_missionTitleX, 0x3CC);
	CHECK_FIELD(CScreen, m_missionTitleY, 0x3D0);
	CHECK_FIELD(CScreen, m_missionTitleTime, 0x3D4);
	CHECK_FIELD(CScreen, m_tutorialNo, 0x3D8);
	CHECK_FIELD(CScreen, m_turnStarted, 0x3E0);
	CHECK_FIELD(CScreen, m_unknown3E1, 0x3E1);
	CHECK_FIELD(CScreen, m_unknown3E2, 0x3E2);
	CHECK_FIELD(CScreen, m_unknown3E3, 0x3E3);
	CHECK_FIELD(CScreen, m_bannerGameMode, 0x3E4);
	CHECK_FIELD(CScreen, m_clubAngle, 0x3F4);
	CHECK_FIELD(CScreen, m_clubSpinDir, 0x3F8);
	CHECK_FIELD(CScreen, m_clubSwing, 0x3FC);
	CHECK_FIELD(CScreen, m_narrowFlicker, 0x3E8);
	CHECK_FIELD(CScreen, m_pCalipers, 0x400);
	CHECK_FIELD(CScreen, m_clubBoundingBox, 0x404);
	CHECK_FIELD(CScreen, m_playerCount, 0x408);
END_LAYOUT_CHECK

IObject* CScreenMakeInstance(void);

// Mini score board of the round screen; its size (0x7C) is the `new` of
// CScreen's constructor. Only the members this unit defines so far are
// named.
class CMiniScore {
	LAYOUT_CHECK_FRIEND;
public:
	CMiniScore(void);
	~CMiniScore(void);

	void ChangeCharacter(void);
	WRect GetIconRect(int flags);
	void Flicker(void);
	void ResetUiPosition(void);
	void Show(void);
	void Hide(void);
	void StartFloating(void);
	void Process(float elapsed);
	void Display(void);

private:
	friend class CScreen;   // CScreen::HandleMsg resets the state and floats the shot count
	CShotCountInfo* m_pShotCountInfo;   // 0x00
	WOverlay* m_pOverlay04;             // 0x04
	const Bitmap* m_pCharBitmap[4];     // 0x08, ChangeCharacter
	const Bitmap* m_pVsIcon;            // 0x18, golf/pc_icon_vs
	WOverlay* m_pOverlay1C;             // 0x1C
	WOverlay* m_pOverlay20;             // 0x20
	WOverlay* m_pBestRecord[2];         // 0x24, [best_record_l.png / [best_record_r.png, by team (Display)
	CSimpleUI* m_pSimpleUI;             // 0x2C
	WFont* m_pFont;                     // 0x30, CChatMsg's masked font
	WRect m_iconUV;                     // 0x34, (0, 0, 0.46875, 1) after construction
	WRect m_iconRect;                   // 0x44, the board's backdrop (Display); w/h set by the constructor
	WRect m_charRect;                   // 0x54, a player's character icon (Display)
	float m_offsetY;                    // 0x64
	float m_posX;                       // 0x68
	int m_state;                        // 0x6C, 0 flicker, 1 hide, 2 show
	float m_stateTime;                  // 0x70
	unsigned char m_alpha;              // 0x74
	PADDING(0x75, 0x78);
	unsigned long m_gameMode;           // 0x78, the CGolfDoc game mode Process last saw
};

CHECK_SIZE(CMiniScore, 0x7C);
BEGIN_LAYOUT_CHECK(CMiniScore)
	CHECK_FIELD(CMiniScore, m_pShotCountInfo, 0x00);
	CHECK_FIELD(CMiniScore, m_pOverlay04, 0x04);
	CHECK_FIELD(CMiniScore, m_pCharBitmap, 0x08);
	CHECK_FIELD(CMiniScore, m_pVsIcon, 0x18);
	CHECK_FIELD(CMiniScore, m_pOverlay1C, 0x1C);
	CHECK_FIELD(CMiniScore, m_pBestRecord, 0x24);
	CHECK_FIELD(CMiniScore, m_pSimpleUI, 0x2C);
	CHECK_FIELD(CMiniScore, m_pFont, 0x30);
	CHECK_FIELD(CMiniScore, m_iconUV, 0x34);
	CHECK_FIELD(CMiniScore, m_iconRect, 0x44);
	CHECK_FIELD(CMiniScore, m_charRect, 0x54);
	CHECK_FIELD(CMiniScore, m_offsetY, 0x64);
	CHECK_FIELD(CMiniScore, m_posX, 0x68);
	CHECK_FIELD(CMiniScore, m_state, 0x6C);
	CHECK_FIELD(CMiniScore, m_stateTime, 0x70);
	CHECK_FIELD(CMiniScore, m_alpha, 0x74);
	CHECK_FIELD(CMiniScore, m_gameMode, 0x78);
END_LAYOUT_CHECK

// Option menu of the round screen (pause, drop, chat record); its size
// (0x30) is the `new` of CScreen's constructor.
class CAuxMenuUI : public FrCmdTarget {
	LAYOUT_CHECK_FRIEND;
public:
	CAuxMenuUI(void);
	virtual ~CAuxMenuUI(void);

	void Display(void);
	void SetActive(bool active, int index);
	void ChangeTurn(void);
	void SetPause(void);
	void OpenChatRecord(void);
	void SetUnplayable(void);
	void Process(float elapsed);

private:
	friend class CScreen;   // CScreen::HandleMsg reads m_pChatFuncDlg and resets m_index
	bool OnChatFuncDlgResult(int ret, FrForm* pForm);
	bool OnDropFailDlgResult(int ret, FrForm* pForm);
	bool OnDropDlgResult(int ret, FrForm* pForm);

	WOverlay* m_pBase;                  // 0x04, [option_base.png
	// 0x08: menu item background, [0] [option_black.png, [1]
	// [option_white.png for the item under the cursor or the current one.
	WOverlay* m_pItemOverlay[2];        // 0x08
	int m_hoverIndex;                   // 0x10, the menu item under the cursor (cursor area - 0x27)
	int m_active;                       // 0x14, 0 closed, 1 opening, 2 open, 3 closing
	int m_index;                        // 0x18
	float m_fade;                       // 0x1C, 0..1 while opening/closing
	FrForm* m_pDropDlg;                 // 0x20
	FrForm* m_pDropFailDlg;             // 0x24
	unsigned char m_unknown28;          // 0x28, 2 after construction
	FrChatFuncDlg* m_pChatFuncDlg;      // 0x2C
};

CHECK_SIZE(CAuxMenuUI, 0x30);
BEGIN_LAYOUT_CHECK(CAuxMenuUI)
	CHECK_FIELD(CAuxMenuUI, m_pBase, 0x04);
	CHECK_FIELD(CAuxMenuUI, m_pItemOverlay, 0x08);
	CHECK_FIELD(CAuxMenuUI, m_hoverIndex, 0x10);
	CHECK_FIELD(CAuxMenuUI, m_active, 0x14);
	CHECK_FIELD(CAuxMenuUI, m_index, 0x18);
	CHECK_FIELD(CAuxMenuUI, m_fade, 0x1C);
	CHECK_FIELD(CAuxMenuUI, m_pDropDlg, 0x20);
	CHECK_FIELD(CAuxMenuUI, m_pDropFailDlg, 0x24);
	CHECK_FIELD(CAuxMenuUI, m_unknown28, 0x28);
	CHECK_FIELD(CAuxMenuUI, m_pChatFuncDlg, 0x2C);
END_LAYOUT_CHECK

// Driving distance board of the round screen. Its size is not known from
// a `new`; 0x20 covers every member its methods touch.
class CDrivingDistInfo {
	LAYOUT_CHECK_FRIEND;
public:
	CDrivingDistInfo(void);
	~CDrivingDistInfo(void);

	void OnChangeTurn(void);
	void Process(float elapsed);
	void Display(WFont* pFont);

private:
	WOverlay* m_pPlayerOverlay;   // 0x00, [nplayer.jpg
	WOverlay* m_pTopOverlay;      // 0x04, [top_drv.jpg
	bool m_unknown08;             // 0x08, true after construction
	bool m_unknown09;             // 0x09
	unsigned char m_bestPlayer;   // 0x0A, player index of the longest drive, 0xFF after construction
	unsigned char m_unknown0B;    // 0x0B, 0xFF after construction
	unsigned long m_bestColor;    // 0x0C, colour of the longest drive's text, -1 after construction
	unsigned long m_myColor;      // 0x10, colour of this player's drive text, -1 after construction
	float m_shownDistance;       // 0x14, copy of m_bestDistance (OnChangeTurn)
	float m_bestDistance;        // 0x18, the longest drive (yards)
	float m_myDistance;          // 0x1C, this player's drive (yards)
};

CHECK_SIZE(CDrivingDistInfo, 0x20);
BEGIN_LAYOUT_CHECK(CDrivingDistInfo)
	CHECK_FIELD(CDrivingDistInfo, m_pPlayerOverlay, 0x00);
	CHECK_FIELD(CDrivingDistInfo, m_pTopOverlay, 0x04);
	CHECK_FIELD(CDrivingDistInfo, m_unknown08, 0x08);
	CHECK_FIELD(CDrivingDistInfo, m_unknown09, 0x09);
	CHECK_FIELD(CDrivingDistInfo, m_bestPlayer, 0x0A);
	CHECK_FIELD(CDrivingDistInfo, m_unknown0B, 0x0B);
	CHECK_FIELD(CDrivingDistInfo, m_bestColor, 0x0C);
	CHECK_FIELD(CDrivingDistInfo, m_myColor, 0x10);
	CHECK_FIELD(CDrivingDistInfo, m_shownDistance, 0x14);
	CHECK_FIELD(CDrivingDistInfo, m_bestDistance, 0x18);
	CHECK_FIELD(CDrivingDistInfo, m_myDistance, 0x1C);
END_LAYOUT_CHECK

// Chat function dialog (show/hide chat, save the chat history, report a
// player); its size (0x130) is the `new` of FrChatFuncDlgMakeInstance.
// Only the members this unit defines so far are declared.
class FrChatFuncDlg : public FrForm {
	LAYOUT_CHECK_FRIEND;
public:
	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }   // gui_control.h:353

	FrChatFuncDlg(void);
	virtual ~FrChatFuncDlg(void);

	void AddChatMsg(const char* msg, int color);

protected:
	void OnBtnShowChatUp(void);
	void OnBtnHideChatUp(void);
	void OnBtnSaveChatUp(void);
	bool OnReportDlgResult(int ret, FrForm* pForm);
	void OnBtnAccuseUp(void);
	void OnBtnCloseDlgUp(void);
	void OnHideChatInit(intptr_t param);
	void OnShowChatInit(intptr_t param);
	void OnChatViewInit(intptr_t param);

private:
	static sFRESH_ENTRY _MsgEntries[];

protected:
	static sFRESH_MSGMAP _MsgMap;
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;

private:
	void* m_pUnknown110;           // 0x110, cleared by the constructor
	FrButton* m_pBtnShowChat;      // 0x114, OnShowChatInit
	FrButton* m_pBtnHideChat;      // 0x118, OnHideChatInit
	void* m_pUnknown11C;           // 0x11C, cleared by the constructor
	void* m_pUnknown120;           // 0x120, cleared by the constructor
	FrEdit* m_pChatView;           // 0x124, OnChatViewInit
	FrReportDlg* m_pReportDlg;     // 0x128, OnBtnAccuseUp
	PADDING(0x12C, 0x130);
};

CHECK_SIZE(FrChatFuncDlg, 0x130);
BEGIN_LAYOUT_CHECK(FrChatFuncDlg)
	CHECK_FIELD(FrChatFuncDlg, m_pUnknown110, 0x110);
	CHECK_FIELD(FrChatFuncDlg, m_pBtnShowChat, 0x114);
	CHECK_FIELD(FrChatFuncDlg, m_pBtnHideChat, 0x118);
	CHECK_FIELD(FrChatFuncDlg, m_pUnknown11C, 0x11C);
	CHECK_FIELD(FrChatFuncDlg, m_pUnknown120, 0x120);
	CHECK_FIELD(FrChatFuncDlg, m_pChatView, 0x124);
	CHECK_FIELD(FrChatFuncDlg, m_pReportDlg, 0x128);
END_LAYOUT_CHECK

#endif // PG_CLIENT_PROJECTG_GUI_CONTROL_H
