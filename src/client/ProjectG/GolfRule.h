// Original: d:\build\custom\temp\code\client\projectg\golfrule.h
#ifndef PG_CLIENT_PROJECTG_GOLFRULE_H
#define PG_CLIENT_PROJECTG_GOLFRULE_H

#include "defines.h"
#include "TaskMain.h"
#include "GolfRuleBase.h"
#include "encrypttypes.h"
#include "wmath.h"
#include "polysoup.h"
#include "GolfDoc.h"
#include "BongdariShopDlg.h"
#include <deque>
#include <vector>

class FrForm;
class FrWnd;
class FrArea;
class FrButton;
class FrEdit;
class FrComboBox;
class FrEmoticonDlg;
class WMatrix;
class WVector;
class CSimpleUI;
class FrApproachResultDlg;
class FrApproachEndDlg;
class WSendPacket;
class MsgObject;
enum enumClientPacket;

// The round's task (its constructor calls CTaskMain's). The constructor
// (golfrule.cpp:1708) and the factory's `new` (0x5C0) fix the size.
class CGolfRule : public CTaskMain {
public:
	CGolfRule(void);
	virtual ~CGolfRule(void);

	static const WRTTI m_RTTI;
	// golfrule.h:33
	virtual const WRTTI *GetRTTI(void) const { return &m_RTTI; }

	// golfrule.h:617
	void SetFlagCutin(bool flagCutin);
	// golfrule.h:618
	bool GetFlagCutin(void);

	void SaveShot(const char *fileName);
	virtual void OnLoad(void);
	virtual void OnReload(void);
	virtual void OnInit(void);
	virtual void OnDestroy(void);
	void SetCurrentGolfRule(void);
	virtual void OnDisplay(void);
	virtual void OnPreLoadInit(void);
	virtual void HandleMsg(const MsgObject &msg);
	virtual void OnProcess(float elapsed);

private:
	static sFRESH_ENTRY _MsgEntries[];

protected:
	static sFRESH_MSGMAP _MsgMap;
	virtual const sFRESH_MSGMAP *GetMessageMap(void) const;

	void OnChatTargetBtnDown(int);
	void OnMyInfoInit(int);
	void OnOptionInit(int);
	void OnUnplayableInit(int);
	void OnPauseInit(int);
	void OnChatRecordInit(int);
	void OnSpecialCardBtnUp(void);
	void OnCaddieBookBtnUp(void);
	void OnChatRecordBtnUp(void);
	void OnOnelineViewOwnerDraw(int);
	void SetGrade(void);
	bool CheckGiveUp(void);
	bool CheckDropOut(void);
	void SendShotResult(void);
	bool OnForgetDlgResult(int result, FrForm *form);
	unsigned long GetHolePang(unsigned char hole);
	void OnEmoticonInit(intptr_t);
	void OnReportInit(int);
	void OnEmoticonBtnUp(void);
	bool OnEmoticonResult(int result, FrForm *form);
	void OnOnelineViewInit(int);
	void OnChatInputInit(intptr_t);
	void OnCaddieBookInit(int);
	void Replay(bool);
	void ReturnLobby(bool);
	bool OnReturnLobbyResult(int, FrForm *);
	bool OnPlayerInfoDlgResult(int, FrForm *);
	void OnMessengerBtnUp(void);
	void OnMessenger_AlarmBtnUp(void);
	void ChangeGameMode(unsigned long mode);
	void OnRadioPredictNormalShotInit(int);
	void OnRadioPredictTomahwakShotInit(int);
	void OnRadioPredictCobraShotInit(int);
	void OnRadioPredictSpikeShotInit(int);
	void OnRadioPredictNormalShotDown(void);
	void OnRadioPredictTomahwakShotDown(void);
	void OnRadioPredictCobraShotDown(void);
	void OnRadioPredictSpikeShotDown(void);
	void SetReady(void);
	void AddPrize(int);
	void AddExtPrize(void);
	void GetWinningPrize(int);
	void OnMyInfoBtnUp(void);
	void OnOptionBtnUp(void);
	bool CancelDropDlgResult(int, FrForm *);
	void CheckNetMsgWait_Send(char *);
	void CheckNetMsgWait_Recv(char *);
	bool CanCancelShot(void);
	void SetPlayer(void);
	WMatrix BankCamera(float);
	void RecordTeamData(unsigned char team);
	void RecordGiveUp(unsigned char index);
	__int64 CalcWinnersPang(unsigned long uid);
	bool HaveTeeShots(void);
	void OnOnelineBtnUp(void);
	bool OnOnelineReqDlgResult(int result, FrForm *form);
	void DoTopViewCamera(float elapsed);
	void OnMessenger_AlarmBtnInit(int);
	void CalcInputCommand(void);
	void OnLanguageInit(int);
	void ForcedMoveBall(const WVector &pos);
	void DoFixedCamera(float elapsed);
	void DoSpotCamera(float elapsed);
	void DoWaitForStart(float elapsed);
	bool CheckAllHoleOut(void);
	bool OnOnelineReqChkOutDlgResult(int result, FrForm *form);
	void PlayBGM(void);
	void OnUnplayableBtnUp(void);
	void OnPauseBtnUp(void);
	void DoNavigateCamera(float elapsed);
	bool OnReportResult(int result, FrForm *form);
	void SetAngle(float angle);
	void DoGetReadyToCaptureBG(float elapsed);
	void SetNetMsgWait(bool send, int type, char *msg, float timeout);
	void RecordScoreByTeam(unsigned char team);
	void RecordScoreByPlayer(unsigned char player);
	void CancelShot(void);
	bool OnChatTargetEnterKey(int);
	void ToggleCommandBtns(bool show);
	void SetVisibleUpperBtns(bool visible);
	void Warp(const WVector &pos);
	// SetEnvironmentVariable under windows.h's macro: the PDB name ends in A.
	void SetEnvironmentVariableA(void);
	bool SetTurnChangeEvent(void);
	void DoCloseupCaddie(float elapsed);
	void ReactionForShot(void);
	void ResetUiPos_Board(void);
	bool IsMaxWithHGParts(unsigned char index);
	bool PauseDlgResult(int result, FrForm *form);
	void RecordPlayerData(unsigned char player);
	void SendHoleStat(void);
	void ChangeTurn(void);
	void ReadyForNextTurn(void);
	bool GoStopDlgResult(int result, FrForm *form);
	void OnMessenger_GiftAlarmBtnUp(void);
	void OnTopBarHoverOff(int);
	void OnRadioPredictNormalShotDraw(int);
	bool ReturnToLobbyDlgResult(int, FrForm *);
	void OnReportBtnUp(void);
	void OnChatTargetInit(intptr_t);
	void OnSpecialCardInit(int);
	void OnRadioPredictTomahwakShotDraw(int);
	void OnRadioPredictCobraShotDraw(int);
	void OnRadioPredictSpikeShotDraw(int);
	void OnMessenger_GiftAlarmBtnInit(int);
	void OnReplayStatusInit(int);
	bool OnApproachDlgResult(int, FrForm *);
	bool OnApproachEndDlgResult(int, FrForm *);
	void OnTopBarHoverOn(int);
	void OnOnelineBtnInit(int);
	void OnMessengerBubbleEditInit(int);
	void IntergrateFile(const char *fileName);
	void SendReportError(unsigned char error);
	void OnChatViewInit(int);
	bool OnChatInputEnterKey(int);
	void DoDebugCamera(float elapsed);
	bool GetBarCondition(void);
	void SetSpecialShot(void);
	void CheckSpecialShotSpinAndCurve(void);
	void OnMessengerBtnInit(int);
	void RecordPlayerList(void);
	void DispatchMatchItems(void);
	void SetHoleInPose(bool);
	void DoFlyBall_Flying(float elapsed);
	void CaptureProcess(void);
	void DoApproachNextHole(float elapsed);
	void UpdateShotResultStat(void);
	void CorrectShotResult(void);
	unsigned char GetNextTurn(void);
	bool ContinueDlgResult(int result, FrForm *form);
	void DoGameOver(float elapsed);
	void CommandBox(void);
	bool SetHoleInPose_Skins(void);
	void CheckUserBGM(const char *);
	void EncodePacket(enumClientPacket type, WSendPacket &packet);
	void ResetRank(void);
	bool CanMoveToNextHole(void);
	bool SetHoleInPose_Match(void);
	bool SetHoleInPose_Team(void);
	void CalcSkinsHolePang(void);
	void DoFlyBall_Stopped(float elapsed);
	void DoCoursePreview(float elapsed);
	void Skip(void);
	void HitShot(void);
	void DoDespairPose(float elapsed);
	void DoScoreBoard(float elapsed);
	void DoHoleInPose(float elapsed);
	void DoDrive(float elapsed);
	void DoFlyBall(float elapsed);
	void DoStand(float elapsed);
	void DriveGauge(float elapsed, bool);
	void PuttingGauge(float elapsed, bool);
	void DoToDefaultCamera(float elapsed);
	void Drop(const WVector &pos);
	void Cancel(void);
	void DoTimeOut(float elapsed);
	void DoPlayerPreview(float elapsed);
	void DoPowerGauge(float elapsed);

	// The rule strategies drive the game mode (GolfRuleTimeattack::SetReady).
	friend class CGolfRuleTimeattack;
	friend class CGolfRuleQuickrace;
	friend class CGolfRuleTutorial;
	friend class CGolfRuleBase;
	friend class CGolfRuleMatch;
	friend class CGolfRuleSkins;
	friend class CGolfRuleTeam;
	friend class CGolfRuleStroke;
	friend class CGolfRuleGuildMatch;
	friend class CGolfRule30Battle;
	friend class CGolfRuleApproach;
	friend class CGolfRuleNewApproach;

protected:
	void ResetData(const WVector &pos);
	unsigned char GetFirstTurn(void);
public:
	void SendHoleData(void);
	void AddPlayer(unsigned char index, const char *name, unsigned long oid, unsigned long uid, unsigned long caddieTypeId);
	void LoadShot(const char *fileName);

private:
	bool StandViewClick(void);
	void AllReplay(void);
	int HoleReplay2(void);
	bool IsLastShotRelplay(void);
	void InitRadioPredictShotButton(FrButton **button, int wnd, float offset);
	void SetRadioPredictShotPress(FrButton *button);
	bool OnRecordAviResult(int result, FrForm *form);
	void ReplayBoxClose(void);
	bool TopViewClick(void);
	void FlyBy(float elapsed);
	int ShotDataSave(const char *fileName, bool flag);
	void _ImeReset(void);
	void CollectBallStopData(void);
	void ToggleGameUI(void);
	bool OnReplayModeDlgResult(int result, FrForm *form);
	void UpdateRadioPredictShotButtonStatus(void);
	void ReplayBoxOpen(void);
	void ReplayDlgRun(void);
	void SaveErrorFile(void);
	char *GetPangyaInstallDic(void);
	void QuickSave(const char *name);
	void OnRecordAviBtnUp(void);
	void HoleReplay(void);
	void SwingReplay(void);
	void AutoReplay(void);
	bool GetShotCntData(void);
	void SaveGame(int slot);
	void LoadGame(int slot);
	void AddSomeInfo(WSendPacket &packet);
	void HoleComboGaugeSetting(void);
	bool ShotDataLoad(const char *fileName);
	void ReplayTapeCheckAndUse(void);
	void QuickLoad(char *name);
	void ProcessObserver(void);

	// Zeroed by the constructor when IsLocalContent(0x50). CalcInputCommand
	// packs the arrow commands one byte each: the first two, then the rest.
	unsigned long m_inputCommandHead;   // 0x1E0
	unsigned long m_inputCommandTail;   // 0x1E4
	unsigned long m_value1E8;   // 0x1E8, role not known
	WVector m_debugCameraEye;   // 0x1EC, the debug camera's eye (ChangeGameMode 0x400)
	WVector m_previewCenter;    // 0x1F8, the point the course preview camera circles (DoCoursePreview)
	float m_modeTime;           // 0x204, time in the current game mode (ChangeGameMode zeroes it)
	float m_goStopTime;         // 0x208, zeroed when HandleMsg (0x95) opens the go/stop dialog (type not traced)
	PADDING(0x20C, 0x210);
	WMatrix m_debugCameraStart; // 0x210, the camera the debug camera starts from (ChangeGameMode 0x400)
	PADDING(0x240, 0x248);
	bool m_replayDone;          // 0x248, cleared when Replay starts a replay
	bool m_shotPlayable;        // 0x249, DoStand only starts the shot when set (else it plays "play_unplayable")
	PADDING(0x24A, 0x24C);
	WVector m_targetPoint;      // 0x24C, the auto-target point (ForcedMoveBall asks CAutoTarget::GetPoint)
	WVector m_point258;         // 0x258, a point message 0x74 copies in (role not known)
	bool m_replaying;           // 0x264, set by Replay
	bool m_goStopSent;          // 0x265, set once GoStopDlgResult sent the answer
	// 0x266-0x268, cleared per hole by ResetData with m_replaying/m_goStopSent.
	bool m_flag266;             // 0x266, role not known
	bool m_flag267;             // 0x267, role not known
	bool m_canFastForward;      // 0x268, the ball may be fast-forwarded (FASTFORWARD key, OnProcess): set when it did not land in group 2
	PADDING(0x269, 0x26C);
	float m_gameOverDelay;      // 0x26C, DoGameOver keeps the camera running until m_modeTime reaches it
	WPolySoup::sArea *m_cameraArea; // 0x270, the course area whose "*cam" camera DoFlyBall installed
	bool m_powerShotCamera;     // 0x274, DoFlyBall selected the power shot camera of the driver shot
	PADDING(0x275, 0x278);
	float m_spot;               // 0x278, the value ChangeGameMode hands the "Spot" actor; HandleMsg 0x7A restores it from sShotProtocol::spot
	// A saved shot state (0x60 bytes, copied whole by the deque).
	struct sBackUp {
		WVector m_ballPos;          // 0x0, PLAYER(0)->m_ballPos
		float m_yaw;                // 0xC, the shot's yaw
		bool m_landed;              // 0x10, CGolfBall::m_landed
		unsigned char m_powerShotLevel; // 0x11
		PADDING(0x12, 0x14);
		float m_comboGauge;         // 0x14
		__int64 m_ghostAward;       // 0x18
		__int64 m_extraPang;        // 0x20
		__int64 m_holeRecord;       // 0x28, PLAYER(0)->m_holeRecord of the hole
		UNKNOWN(0x30, 0x58);
		char m_clubName[4];         // 0x58
		unsigned char m_holeScore;  // 0x5C, PLAYER(0)->m_holes of the hole
		unsigned char m_rivalLevel; // 0x5D
		PADDING(0x5E, 0x60);
	};
	std::deque<sBackUp> m_backUps;  // 0x27C, emptied by OnDestroy
	sBackUp m_backUp;           // 0x290, the shot's state ChangeGameMode (0x40) fills and pushes into m_backUps
	unsigned char m_nextTurnPlayer; // 0x2F0, the player whose turn comes after a time-out (DoTimeOut makes him the current player)
	PADDING(0x2F1, 0x2F4);
	// A recorded shot (0x88 bytes), the element of CSharedDoc::m_shotDataList:
	// the layout of the hole block below, which GetShotCntData fills whole
	// from the list (rep movsd) and SaveGame/SaveShot/LoadShot write and read
	// whole. The members are the hole block's, rebased to 0.
	struct sShotData {
		unsigned char m_hole;       // 0x0
		PADDING(0x1, 0x4);
		WVector m_wind;             // 0x4
		unsigned char m_weather;    // 0x10
		unsigned char m_shotNumber; // 0x11
		PADDING(0x12, 0x14);
		sBar m_shotBar;             // 0x14
		WVector m_ballPos;          // 0x38
		float m_shotYaw;            // 0x44
		float m_lieSlope;           // 0x48
		float m_lieAngle;           // 0x4C
		unsigned long m_shotFrame;  // 0x50
		unsigned long m_powerFlags; // 0x54
		int m_shotPowerShot;        // 0x58
		char m_clubName[4];         // 0x5C
		unsigned char m_powerShotLevel; // 0x60
		PADDING(0x61, 0x64);
		unsigned long m_shotItemTypeId; // 0x64
		float m_angle;              // 0x68
		unsigned char m_shotCountKey; // 0x6C
		PADDING(0x6D, 0x70);
		int m_ghostAward;           // 0x70
		int m_extraPang;            // 0x74
		WCrypticValue<float> m_comboGauge; // 0x78
	};
	// 0x2F4-0x37B: the hole block, a sShotData (see above). It stays flat
	// because the GolfRule* units read m_pRule->m_hole.
	unsigned char m_hole;       // 0x2F4, the current hole (CGolfRuleTutorial::SetPlayer passes it to LoadMapCheckData)
	PADDING(0x2F5, 0x2F8);
	WVector m_holeWind;         // 0x2F8, the shot's wind GetShotCntData hands CWind::SetWind
	unsigned char m_weather;    // 0x304, the hole's weather a replayed shot restores (ChangeGameMode 0x10000)
	unsigned char m_shotNumber; // 0x305, the shot of the hole SaveShot reports in its chat message
	PADDING(0x306, 0x308);
	// 0x308, the bar of the replayed shot: DoStand copies it into CGolfDoc::m_bar.
	// LoadShot copies its 0xC dword into CSharedDoc::m_replayValue5BC0 as an
	// int; that member is likely a float (gui_control copies it back into
	// sBar::m_prevAccuracy), and the union goes once it is one.
	union {
		sBar m_shotBar;
		struct {
			UNKNOWN(0x308, 0x314);
			int m_savedReplayValue; // 0x314, LoadShot restores CSharedDoc::m_replayValue5BC0 from it
		} m_shotBarRaw;
	};
	WVector m_shotBallPos;      // 0x32C, the ball's position of the recorded shot (ChangeGameMode 0x10000)
	float m_shotYaw;            // 0x338, the camera yaw a replayed shot restores (SetEnvironmentVariableA)
	float m_shotLieSlope;       // 0x33C, CGolfBall::m_lieSlope at the shot, restored in playback (ChangeGameMode 0x40)
	float m_shotLieAngle;       // 0x340, CGolfBall::m_lieAngle at the shot
	unsigned long m_shotFrame;  // 0x344, the quadtree frame (CQuadTree::m_frame) of the recorded shot; SaveShot saves nothing without it
	unsigned long m_shotPowerFlags; // 0x348, CGolfBall::m_powerFlags at the shot
	int m_shotPowerShot;        // 0x34C, the power shot a replayed shot restores (CClub::SetPowerShot)
	char m_clubName[4];         // 0x350, the selected club's name LoadShot passes to CClub::SetClub
	unsigned char m_powerShotLevel; // 0x354, the power shot level ResetData restores in replay playback
	PADDING(0x355, 0x358);
	unsigned long m_shotItemTypeId; // 0x358, sPlayerData::m_shotItemTypeId at the shot
	float m_shotAngle;          // 0x35C, the shot's launch angle, restored in playback
	unsigned char m_shotCountKey; // 0x360, GetShotCntData restores CSharedDoc::m_shotCountKey from it
	PADDING(0x361, 0x364);
	int m_ghostAward;           // 0x364, GetShotCntData restores sPlayerData::m_ghostAward from it
	int m_extraPang;            // 0x368, GetShotCntData restores sPlayerData::m_extraPang from it
	WCrypticValue<float> m_comboGauge; // 0x36C, the combo gauge GetShotCntData restores into sPlayerData::m_comboGauge
	WVector m_wind;             // 0x37C, the hole's wind LoadGame restores
	char m_flySound[0x20];      // 0x388, the flight sound the ball flight start picks by distance ("wb_fly" for two balls), stopped and played at once
	std::vector<WVector> m_flyByPoints; // 0x3A8, the fly-by camera path, quadratic Bezier triples FlyBy walks two points at a time (the destructor frees it)
	unsigned char m_firstTeam;  // 0x3B8, the team that tees off first: my player's team online (GetFirstTurn)
	PADDING(0x3B9, 0x3BC);
	unsigned long m_gaugeTime;  // 0x3BC, timeGetTime() of the power gauge's last frame (DoPowerGauge); message 0xA2 resumes it from m_pauseTime
	unsigned long m_gaugeStartTime; // 0x3C0, timeGetTime() of the power gauge's first frame (DoPowerGauge); cleared at the impact (ChangeGameMode 0x20)
	unsigned long m_pauseTime;  // 0x3C4, timeGetTime() while the gauge is paused (DoPowerGauge)
	bool m_gaugePaused;         // 0x3C8, the power gauge is paused (message 0xA2 toggles it, the tutorial)
	bool m_flag3C9;             // 0x3C9, cleared by OnPreLoadInit
	bool m_continueDlgOpen;     // 0x3CA, DoGameOver opened the "continue practice?" dialog
	bool m_netMsgWaitSend;      // 0x3CB, SetNetMsgWait: waiting on a sent (true) or received packet
	int m_netMsgWaitType;       // 0x3CC
	char *m_netMsgWait;         // 0x3D0, the packet name CheckNetMsgWait_Send/_Recv compare with
	char *m_netMsgWaitRecv;     // 0x3D4, the packet name of a receive wait
	unsigned long m_netMsgSendTime; // 0x3D8, GetTickCount of the send wait
	unsigned long m_netMsgRecvTime; // 0x3DC, GetTickCount of the receive wait
	float m_netMsgWaitTimeout;  // 0x3E0
	PADDING(0x3E4, 0x3E5);
	bool m_shotKeyAlways;       // 0x3E5, DoStand takes the space bar before game state 15 even while the quit window is open
	PADDING(0x3E6, 0x3E8);
	float m_fakeShotTargets[2]; // 0x3E8, the bar positions the tutorial's fake space bar stops at, by gauge stage (message 0x87 (1/2) m_param2/m_param3; DoPowerGauge)
	bool m_flag3F0;             // 0x3F0, set by message 0x9A
	PADDING(0x3F1, 0x3F4);
	unsigned long m_pendingPowerFlags; // 0x3F4, OR-ed into the ball's m_powerFlags (then cleared) by CheckSpecialShotSpinAndCurve
	unsigned char m_bgmIndex;   // 0x3F8, the m_bgmList entry PlayBGM plays off the green
	PADDING(0x3F9, 0x3FC);
	unsigned long m_commandTime; // 0x3FC, timeGetTime() when CommandBox saw a single arrow pressed (0 = none)
	PADDING(0x400, 0x401);
	bool m_shotKeyLocked;       // 0x401, DoStand ignores the space bar while set
	PADDING(0x402, 0x404);
	float m_shotKeyLockTime;    // 0x404, zeroed when message 0x9E locks the shot key; OnProcess unlocks it after 5 seconds
	bool m_shotReady;           // 0x408, cleared by CancelShot
	PADDING(0x409, 0x40C);
	unsigned long m_observerSwitchTime; // 0x40C, g_CurrentTime of the observer's last camera target request (ProcessObserver); zeroed by OnPreLoadInit
	bool m_typingIconShown;     // 0x410, the chat input holds text and the typing head icon was sent (OnProcess)
	PADDING(0x411, 0x414);
	FrForm *m_pauseDlg;         // 0x414, cleared by PauseDlgResult
	FrForm *m_goStopDlg;        // 0x418, cleared by GoStopDlgResult
	FrForm *m_dropDlg;          // 0x41C, cleared by CancelDropDlgResult
	FrForm *m_playerInfoDlg;    // 0x420, cleared by OnPlayerInfoDlgResult
	FrApproachResultDlg *m_approachResultDlg; // 0x424, cleared by OnApproachDlgResult
	FrApproachEndDlg *m_approachEndDlg; // 0x428, opened by OnApproachDlgResult once the approach ends
	bool m_flag42C;             // 0x42C, cleared by OnInit
	PADDING(0x42D, 0x430);
	float m_msgValue430;        // 0x430, set (as a float) by message 0x9B
	float m_msgValue434;        // 0x434, set (as a float) by message 0x9D
	PADDING(0x438, 0x444);
	bool m_forgetPending;       // 0x444, the "flower of forgetting" dialog runs (DoFlyBall_Stopped)
	PADDING(0x445, 0x448);
	float m_forgetElapsed;      // 0x448, seconds the forget dialog has been open; closed at 10 (DoFlyBall_Stopped)
	int m_cancelShotItemSlot;   // 0x44C, the equip slot of the shot cancel item (CanCancelShot)
	bool m_shotBoardNextHole;   // 0x450, in a shot board room (room type 7) DoHoleInPose moves to the next hole once set
	PADDING(0x451, 0x454);
	float m_effectTime;         // 0x454, seconds to the next "Effect" message 0x17D (OnProcess; 0 = none pending)
	int m_effectCount;          // 0x458, "Effect" messages still to send, one every 0.1 seconds
	int m_effectParam;          // 0x45C, the parameter of those messages
	std::vector<std::string> m_bgmList; // 0x460, the round's background musics (PlayBGM)
	FrComboBox *m_chatEdit;     // 0x470, the chat target box (OnChatTargetInit) OnChatTargetEnterKey resets
	FrEdit *m_chatInput;        // 0x474, the chat input box (OnChatInputInit), focused by OnChatTargetBtnDown
	FrWnd *m_emoticonBtn;       // 0x478, set by OnEmoticonInit
	FrArea *m_languageArea;     // 0x47C, the chat input's language indicator (OnLanguageInit)

	FrWnd *m_reportBtn;         // 0x480, set by OnReportInit
	FrEdit *m_chatViewWnd;      // 0x484, the chat log box (OnChatViewInit)
	FrEmoticonDlg *m_emoticonDlg; // 0x488, opened and closed by OnEmoticonBtnUp
	FrForm *m_reportDlg;        // 0x48C, cleared by OnReportResult
	FrForm *m_forgetDlg;        // 0x490, the "flowerofforgetting" form, cleared by OnForgetDlgResult
	FrForm *m_onelineReqDlg;    // 0x494, the one-line request dialog OnOnelineBtnUp opens

	FrForm *m_replayBox;        // 0x498, opened by ReplayBoxOpen, closed by ReplayBoxClose
	FrForm *m_recordAviDlg;     // 0x49C, cleared by OnRecordAviResult
	FrArea *m_replayBoxWnd;     // 0x4A0, the replay icon: shown by ReplayBoxClose, its image follows the replay state (ReplayDlgRun)
	FrWnd *m_myInfoBtn;         // 0x4A4, set by OnMyInfoInit
	FrWnd *m_optionBtn;         // 0x4A8, set by OnOptionInit
	FrWnd *m_commandBtns[4];    // 0x4AC, shown or hidden together by ToggleCommandBtns
	FrWnd *m_onelineBoardWnd;   // 0x4BC, the one-line board's window (ResetUiPos_Board)
	FrWnd *m_boardWnd;          // 0x4C0, kept 140 pixels from the right edge (ResetUiPos_Board)
	bool m_flag4C4;             // 0x4C4, cleared by the constructor
	// 0x4C5, five sBonusBall records (0x14 bytes each) at an unaligned offset,
	// shown by the "closed_bonusball" dialog (message 0x29); emptied by the
	// constructor (one memset of 100 bytes). sBonusBall is 4-aligned here, so
	// the bytes stay a char array until its alignment is known.
	char m_closedBonusBalls[5 * sizeof(sBonusBall)];
	PADDING(0x529, 0x52C);
	unsigned long m_shotStartFrame; // 0x52C, the quadtree frame at the shot; the replay rewinds to it (ChangeGameMode 0x40)
	unsigned long m_replayFrame; // 0x530, Replay restores the quadtree's frame counter (CQuadTree::m_frame) from it
	unsigned long m_flightFrame; // 0x534, the quadtree frame the ball flight starts from (ChangeGameMode 0x40 sets it, 0x80 restores it)
	FrButton *m_predictNormalShotBtn;   // 0x538, radio button of the normal shot prediction
	FrButton *m_predictTomahwakShotBtn; // 0x53C
	FrButton *m_predictCobraShotBtn;   // 0x540
	FrButton *m_predictSpikeShotBtn;   // 0x544
	CSimpleUI *m_predictShotUI;        // 0x548, the "Pangya" label OnRadioPredictNormalShotDraw draws by the normal shot button
	int m_captureGrid;          // 0x54C, tiles per side of a composite screenshot IntergrateFile stitches together
	int m_value550;             // 0x550, zeroed by OnPreLoadInit
	int m_updateClipAlways;     // 0x554, OnDisplay sets the view clip planes whatever the game mode while it is non-zero
	bool m_showGameUI;          // 0x558, ToggleGameUI shows or hides the round's UI with it
	char m_saveFileName[0x40]; // 0x559, the free save/... or error/..._NNN.rep name SaveGame and SaveErrorFile pick
	PADDING(0x599, 0x59C);
	int m_value59C;             // 0x59C, zeroed by the constructor; subtracted from PLAYER's ghost award in the shot record (ChangeGameMode 0x40)
	int m_value5A0;             // 0x5A0, zeroed by the constructor; subtracted from PLAYER's extra pang in the shot record (ChangeGameMode 0x40)
	float m_shotStartComboGauge; // 0x5A4, my combo gauge (my team's in a team room) when the stand mode begins (ChangeGameMode 8); the shot record keeps it (ChangeGameMode 0x40)
	unsigned long m_replayFlags; // 0x5A8, AllReplay clears bit 0x4
	int m_replayMode;           // 0x5AC, 2 = last shot replay
	bool m_shotDataCorrupt;     // 0x5B0, OnProcess tells the player the file is damaged and returns to the lobby
	bool m_flag5B1;             // 0x5B1, cleared when the ball flight starts (ChangeGameMode 0x80)
	bool m_flagCutin;           // 0x5B2
	PADDING(0x5B3, 0x5B4);
	CGolfRuleBase* m_pRule;     // 0x5B4
	bool m_flag5B8;             // 0x5B8, cleared by OnInit
	bool m_waitingForStart;     // 0x5B9, set by DoWaitForStart once the fly-by has run
	PADDING(0x5BA, 0x5BC);
	// 0x5BC, the top view camera: 0 off, 1 PAD_0, 2 shift/mouse, 3 control
	// (TopViewClick); zeroed by the constructor.
	int m_topViewMode;

	LAYOUT_CHECK_FRIEND;
};
CHECK_SIZE(CGolfRule, 0x5C0);
BEGIN_LAYOUT_CHECK(CGolfRule)
	CHECK_FIELD(CGolfRule, m_modeTime, 0x204);
	CHECK_FIELD(CGolfRule, m_previewCenter, 0x1F8);
	CHECK_FIELD(CGolfRule, m_goStopSent, 0x265);
	CHECK_FIELD(CGolfRule, m_flag266, 0x266);
	CHECK_FIELD(CGolfRule, m_flag267, 0x267);
	CHECK_FIELD(CGolfRule, m_canFastForward, 0x268);
	CHECK_FIELD(CGolfRule, m_gameOverDelay, 0x26C);
	CHECK_FIELD(CGolfRule, m_continueDlgOpen, 0x3CA);
	CHECK_FIELD(CGolfRule, m_backUps, 0x27C);
	CHECK_FIELD(CGolfRule, m_cameraArea, 0x270);
	CHECK_FIELD(CGolfRule, m_powerShotCamera, 0x274);
	CHECK_FIELD(CGolfRule, m_spot, 0x278);
	CHECK_FIELD(CGolfRule, m_nextTurnPlayer, 0x2F0);
	CHECK_FIELD(CGolfRule, m_hole, 0x2F4);
	CHECK_FIELD(CGolfRule, m_shotNumber, 0x305);
	CHECK_FIELD(CGolfRule, m_shotBar, 0x308);
	CHECK_FIELD(CGolfRule, m_shotPlayable, 0x249);
	CHECK_FIELD(CGolfRule, m_shotKeyAlways, 0x3E5);
	CHECK_FIELD(CGolfRule, m_shotKeyLocked, 0x401);
	CHECK_FIELD(CGolfRule, m_shotBoardNextHole, 0x450);
	CHECK_FIELD(CGolfRule, m_clubName, 0x350);
	CHECK_FIELD(CGolfRule, m_holeWind, 0x2F8);
	CHECK_FIELD(CGolfRule, m_powerShotLevel, 0x354);
	CHECK_FIELD(CGolfRule, m_shotCountKey, 0x360);
	CHECK_FIELD(CGolfRule, m_ghostAward, 0x364);
	CHECK_FIELD(CGolfRule, m_extraPang, 0x368);
	CHECK_SIZE(CGolfRule::sShotData, 0x88);
	CHECK_FIELD(CGolfRule::sShotData, m_wind, 0x4);
	CHECK_FIELD(CGolfRule::sShotData, m_shotNumber, 0x11);
	CHECK_FIELD(CGolfRule::sShotData, m_shotBar, 0x14);
	CHECK_FIELD(CGolfRule::sShotData, m_shotYaw, 0x44);
	CHECK_FIELD(CGolfRule::sShotData, m_shotFrame, 0x50);
	CHECK_FIELD(CGolfRule::sShotData, m_shotPowerShot, 0x58);
	CHECK_FIELD(CGolfRule::sShotData, m_clubName, 0x5C);
	CHECK_FIELD(CGolfRule::sShotData, m_powerShotLevel, 0x60);
	CHECK_FIELD(CGolfRule::sShotData, m_shotCountKey, 0x6C);
	CHECK_FIELD(CGolfRule::sShotData, m_ghostAward, 0x70);
	CHECK_FIELD(CGolfRule::sShotData, m_extraPang, 0x74);
	CHECK_FIELD(CGolfRule::sShotData, m_comboGauge, 0x78);
	CHECK_FIELD(CGolfRule, m_shotYaw, 0x338);
	CHECK_FIELD(CGolfRule, m_shotPowerShot, 0x34C);
	CHECK_FIELD(CGolfRule, m_firstTeam, 0x3B8);
	CHECK_FIELD(CGolfRule, m_netMsgWaitSend, 0x3CB);
	CHECK_FIELD(CGolfRule, m_netMsgWaitType, 0x3CC);
	CHECK_FIELD(CGolfRule, m_netMsgWait, 0x3D0);
	CHECK_FIELD(CGolfRule, m_netMsgWaitRecv, 0x3D4);
	CHECK_FIELD(CGolfRule, m_netMsgSendTime, 0x3D8);
	CHECK_FIELD(CGolfRule, m_netMsgRecvTime, 0x3DC);
	CHECK_FIELD(CGolfRule, m_netMsgWaitTimeout, 0x3E0);
	CHECK_FIELD(CGolfRule, m_pendingPowerFlags, 0x3F4);
	CHECK_FIELD(CGolfRule, m_bgmIndex, 0x3F8);
	CHECK_FIELD(CGolfRule, m_commandTime, 0x3FC);
	CHECK_FIELD(CGolfRule, m_shotReady, 0x408);
	CHECK_FIELD(CGolfRule, m_pauseDlg, 0x414);
	CHECK_FIELD(CGolfRule, m_goStopTime, 0x208);
	CHECK_FIELD(CGolfRule, m_goStopDlg, 0x418);
	CHECK_FIELD(CGolfRule, m_dropDlg, 0x41C);
	CHECK_FIELD(CGolfRule, m_playerInfoDlg, 0x420);
	CHECK_FIELD(CGolfRule, m_approachResultDlg, 0x424);
	CHECK_FIELD(CGolfRule, m_approachEndDlg, 0x428);
	CHECK_FIELD(CGolfRule, m_forgetPending, 0x444);
	CHECK_FIELD(CGolfRule, m_emoticonBtn, 0x478);
	CHECK_FIELD(CGolfRule, m_reportBtn, 0x480);
	CHECK_FIELD(CGolfRule, m_emoticonDlg, 0x488);
	CHECK_FIELD(CGolfRule, m_forgetElapsed, 0x448);
	CHECK_FIELD(CGolfRule, m_cancelShotItemSlot, 0x44C);
	CHECK_FIELD(CGolfRule, m_forgetDlg, 0x490);
	CHECK_FIELD(CGolfRule, m_bgmList, 0x460);
	CHECK_FIELD(CGolfRule, m_chatEdit, 0x470);
	CHECK_FIELD(CGolfRule, m_chatInput, 0x474);
	CHECK_FIELD(CGolfRule, m_chatViewWnd, 0x484);
	CHECK_FIELD(CGolfRule, m_reportDlg, 0x48C);
	CHECK_FIELD(CGolfRule, m_replayBox, 0x498);
	CHECK_FIELD(CGolfRule, m_recordAviDlg, 0x49C);
	CHECK_FIELD(CGolfRule, m_replayBoxWnd, 0x4A0);
	CHECK_FIELD(CGolfRule, m_myInfoBtn, 0x4A4);
	CHECK_FIELD(CGolfRule, m_optionBtn, 0x4A8);
	CHECK_FIELD(CGolfRule, m_commandBtns, 0x4AC);
	CHECK_FIELD(CGolfRule, m_onelineBoardWnd, 0x4BC);
	CHECK_FIELD(CGolfRule, m_boardWnd, 0x4C0);
	CHECK_FIELD(CGolfRule, m_predictNormalShotBtn, 0x538);
	CHECK_FIELD(CGolfRule, m_predictShotUI, 0x548);
	CHECK_FIELD(CGolfRule, m_captureGrid, 0x54C);
	CHECK_FIELD(CGolfRule, m_predictTomahwakShotBtn, 0x53C);
	CHECK_FIELD(CGolfRule, m_predictCobraShotBtn, 0x540);
	CHECK_FIELD(CGolfRule, m_predictSpikeShotBtn, 0x544);
	CHECK_FIELD(CGolfRule, m_updateClipAlways, 0x554);
	CHECK_FIELD(CGolfRule, m_showGameUI, 0x558);
	CHECK_FIELD(CGolfRule, m_saveFileName, 0x559);
	CHECK_FIELD(CGolfRule, m_replayFlags, 0x5A8);
	CHECK_FIELD(CGolfRule, m_replayMode, 0x5AC);
	CHECK_FIELD(CGolfRule, m_flagCutin, 0x5B2);
	CHECK_FIELD(CGolfRule, m_pRule, 0x5B4);
	CHECK_FIELD(CGolfRule, m_waitingForStart, 0x5B9);
	CHECK_FIELD(CGolfRule, m_inputCommandHead, 0x1E0);
	CHECK_FIELD(CGolfRule, m_targetPoint, 0x24C);
	CHECK_FIELD(CGolfRule, m_value1E8, 0x1E8);
	CHECK_FIELD(CGolfRule, m_comboGauge, 0x36C);
	CHECK_FIELD(CGolfRule, m_flyByPoints, 0x3A8);
	CHECK_FIELD(CGolfRule, m_flag42C, 0x42C);
	CHECK_FIELD(CGolfRule, m_languageArea, 0x47C);
	CHECK_FIELD(CGolfRule, m_onelineReqDlg, 0x494);
	CHECK_FIELD(CGolfRule, m_flag4C4, 0x4C4);
	CHECK_FIELD(CGolfRule, m_closedBonusBalls, 0x4C5);
	CHECK_FIELD(CGolfRule, m_point258, 0x258);
	CHECK_FIELD(CGolfRule, m_gaugeTime, 0x3BC);
	CHECK_FIELD(CGolfRule, m_pauseTime, 0x3C4);
	CHECK_FIELD(CGolfRule, m_gaugePaused, 0x3C8);
	CHECK_FIELD(CGolfRule, m_flag3C9, 0x3C9);
	CHECK_FIELD(CGolfRule, m_observerSwitchTime, 0x40C);
	CHECK_FIELD(CGolfRule, m_typingIconShown, 0x410);
	CHECK_FIELD(CGolfRule, m_effectTime, 0x454);
	CHECK_FIELD(CGolfRule, m_effectCount, 0x458);
	CHECK_FIELD(CGolfRule, m_effectParam, 0x45C);
	CHECK_FIELD(CGolfRule, m_value550, 0x550);
	CHECK_FIELD(CGolfRule, m_shotDataCorrupt, 0x5B0);
	CHECK_FIELD(CGolfRule, m_flag3F0, 0x3F0);
	CHECK_FIELD(CGolfRule, m_fakeShotTargets, 0x3E8);
	CHECK_FIELD(CGolfRule, m_shotKeyLockTime, 0x404);
	CHECK_FIELD(CGolfRule, m_msgValue430, 0x430);
	CHECK_FIELD(CGolfRule, m_msgValue434, 0x434);
	CHECK_FIELD(CGolfRule, m_value59C, 0x59C);
	CHECK_FIELD(CGolfRule, m_value5A0, 0x5A0);
	CHECK_FIELD(CGolfRule, m_shotStartComboGauge, 0x5A4);
	CHECK_FIELD(CGolfRule, m_flag5B8, 0x5B8);
	CHECK_FIELD(CGolfRule, m_topViewMode, 0x5BC);
	CHECK_FIELD(CGolfRule, m_replayDone, 0x248);
	CHECK_FIELD(CGolfRule, m_replaying, 0x264);
	CHECK_FIELD(CGolfRule, m_shotFrame, 0x344);
	CHECK_FIELD(CGolfRule, m_replayFrame, 0x530);
	CHECK_FIELD(CGolfRule, m_wind, 0x37C);
END_LAYOUT_CHECK

#endif // PG_CLIENT_PROJECTG_GOLFRULE_H
