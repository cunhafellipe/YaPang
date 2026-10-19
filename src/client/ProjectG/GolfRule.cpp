// Original: d:\build\custom\temp\code\client\projectg\golfrule.cpp
#define GOLFRULE_GOLFDOC_INLINE
#define GOLFRULE_GLOBALGAMEDEFINE_INLINE
#define GOLFRULE_CHATMSG_INLINE
#define GOLFRULE_LOGININFO_INLINE
#define GOLFRULE_CLUB_INLINE
#define GOLFRULE_GOLFBALL_INLINE
#define GOLFRULE_MOUSECURSOR_INLINE
#define GOLFRULE_SHAREDDOC_INLINE
#define GOLFRULE_WMATH_INLINE
#define GOLFRULE_FRWND_INLINE
#define GOLFRULE_TREASURE_INLINE
#define GOLFRULE_QUADTREE_INLINE
#include <atlbase.h>	// stand-in for the precompiled header
#include "NetworkSystem.h"
#include "GolfRule.h"
#include "encryption.h"
#include "ClientSetting.h"
#include <mmsystem.h>
#include "GolfDoc.h"
#include "Fx.h"
#include "club.h"
#include "quadtree.h"
#include "wutil.h"
#include "mousecursor.h"
#include "headicon.h"
#include "messengerdlg.h"
#include "logininfo.h"
#include "ChatMsg.h"
#include "frwnd.h"
#include "frdesktop.h"
#include "frbutton.h"
#include "onelineboard.h"
#include "InputManager.h"
#include "fresh.h"
#include "Treasure.h"
#include "wind.h"
#include "cardsystem.h"
#include "globalgamedefine.h"
#include "SharedDoc.h"
#include "TaskManager.h"
#include "TaskMain.h"
#include "RTTI.h"
#include "ObjectFactory.h"
#include "QuitWindow.h"
#include "ShuffleBonus.h"
#include "hackingmanager.h"
#include "user_info.h"
#include "ItemWindow.h"
#include "wfont.h"
#include "wmath.h"
#include "Localize.h"
#include "NetResourceManager.h"
#include "Packet.h"
#include "s5/Utilities.h"
#include "s5/classicserver/ClassicServer.h"
#include "shared/s5/SharedUtilities.h"
#include "golfball.h"
#include "winput.h"
#include "HelpMsg.h"
#include "CapturedBg.h"
#include "rival.h"
#include "RandomHouse.h"
#include "fredit.h"
#include "frscrollbar.h"
#include "frcombobox.h"
#include "parttidlist.h"
#include "camerawork.h"
#include "SoundManager.h"
#include "ProjectG.h"
#include "VoiceItem.h"
#include "EffectManager.h"
#include "fxspray.h"
#include "CodeTinker.h"
#include "UccManager.h"
#include "PowerGauge.h"
#include "scenemanager.h"
#include "ReplayDlg.h"
#include "GiftAlarmDlg.h"
#include "EmoticonDlg.h"
#include "OnelineReqDlg.h"
#include "frarea.h"
#include "ApproachResultDlg.h"
#include "autotarget.h"
#include <io.h>
#include <new>
#include <stdio.h>
#include "commonutil.h"
#include "letter_effect.h"
#include "contentsdoc.h"
#include "Shadow.h"
#include "ToolTip.h"
#include "SimpleOverlay.h"
#include "client/Wangreal/include/wpuppet.h"
#include "client/Wangreal/include/wview.h"
#include "client/Wangreal/include/bitmap.h"
#include "client/Wangreal/include/wresrcmng.h"
#include "Registry.h"
#include "UccLibrary.h"
#include "slicestr.h"
#include "GolfRuleTeam.h"
#include "GolfRuleSkins.h"
#include "GolfRuleMatch.h"
#include "GolfRuleStroke.h"
#include "GolfRuleApproach.h"
#include "GolfRuleNewApproach.h"
#include "GolfRuleGuildMatch.h"
#include "GolfRule30Battle.h"
#include "GolfRuleTutorial.h"
#include "KbdManualDlg.h"
#include "RankingDlg.h"

// Defined in another unit (TutorialTask also uses them).
// Name of the gateway actor (file-local data).
static const char* s_gatewayActorName = "Gateway";

extern int g_iSendPacketType;
extern int g_iRecvPacketType;
extern int g_iSendPacketGR;
extern int g_iRecvPacketGR;
extern int g_iSendFailType;     // ClientSocket.cpp
extern int g_iRecvDisconCode;   // TaskManager.cpp
extern bool g_bQuit;   // ProjectG.cpp
extern WMatrix g_camera;
extern char g_executeDirectory[];
// Two adjacent file statics with no symbol, cut by the split as one 8-byte
// block (references to the second read data_af1d84+4).
extern "C" {
static struct {
	long loadGamePos;          // 0x0, the read position of the next LoadGame part in save/save_000.dat
	unsigned int flyByIndex;   // 0x4, the fly-by's current point of m_flyByPoints (FlyBy)
} data_af1d84;
}
// The divisor of the power bar excess HandleMsg (0x8B) reports (no symbol).
extern "C" { static float data_00af1c38; }
// The clear bonus per hole weight by course, indexed by GetCurMap (no symbol).
// The CFx control serial of the power spin effect DoFlyBall_Flying starts
// (-1 before the first one).
extern "C" { static int data_a7df58 = -1; }
extern "C" { static const int data_9e8f98[20] = { 20, 50, 55, 80, 65, 24, 50, 70, 40, 55, 40, 20, 0, 80, 20, 20, 40, 40, 0, 40 }; }

// The original's max (wutil Max uses >= and gives the other branch).
template <class T> T MaxOf(T a, T b) { return a < b ? b : a; }
// The IME input device (public ?g_ime@@3PAVWInputDev@@A).
extern WInputDev *g_ime;
// The bug report file of a ball stop replay (public ?ball_stop_replay_data_name@@3PBDB);
// defined at the end of the file: the original reads it from memory.
extern const char *const ball_stop_replay_data_name;
// The putting green musics (public ?g_bgmPutting@@3PAPBDA).
const char *g_bgmPutting[] = {
	"putting.mp3", "putting2.mp3"
};
// The character motion names by shot timing (publics ?g_szCommonCharMotion@@3PAPBDA, ?g_szLuciaCharMotion@@3PAPBDA).
const char *g_szCommonCharMotion[] = {
	"\xbe\xcb\xb9\xd9\xc8\xa6\xc0\xce\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee", "\xbe\xcb\xb9\xd9\xc8\xa6\xc0\xce\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee",
	"\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee", "\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee",
	"\xbc\xbc\xc0\xcc\xba\xea\xc6\xc4\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee", "\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee",
	"\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee", "\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee"
};
const char *g_szLuciaCharMotion[] = {
	"\xbe\xcb\xb9\xd9\xc8\xa6\xc0\xce\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee", "\xbe\xcb\xb9\xd9\xc8\xa6\xc0\xce\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee",
	"\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee", "\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee",
	"\xbc\xbc\xc0\xcc\xba\xea\xc6\xc4\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee", "\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee",
	"\xb4\xf5\xba\xed\xba\xb8\xb1\xe2\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee", "\xb4\xf5\xba\xed\xba\xb8\xb1\xe2\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee"
};
// The bar click sound effects, indexed by accuracy grade (public ?g_plSound@@3PAPBDA).
const char *g_plSound[] = {
	"shot_best_timing", "shot_best_timing",
	"shot_good_timing", "shot_normal_timing",
	"shot_bad_timing", "\xb0\xb6\xb7\xaf\xb8\xae_\xb9\xda\xbc\xf6",
	"\xc6\xce\xbe\xdf", "pangya_mike"
};
// wmath (?WisZero@@YIHABMM@Z); no header declares it yet.
int WisZero(const float &value, float eps);
// commonutil.cpp (commonutil.h does not declare it yet).
std::string GetMyComputerName();
// commonutil.cpp:1106 (commonutil.h does not declare it yet).
float Random(float min, float max, int seed);
// <direct.h>'s mkdir (the toolchain has no direct.h yet).
extern "C" __declspec(dllimport) int __cdecl mkdir(const char *);
// commonutil.cpp (commonutil.h does not declare it yet).
void SaveBMP(const char *fileName, const Bitmap *bitmap);
// The back buffer size (ProjectG.cpp).
extern int g_last_screen_width;
extern int g_last_screen_height;

// clientsetting.h:345
int COption::vGetTnLMode(void) const {
	return m_tnlMode;
}

// encryption.h:134
CEncryption::CEncryption() {
	(void)&CEncryption::Instance;	// odr-use: emit Instance's local-static helper (SendShotResult's call site)
}

// encryption.h:135
CEncryption::~CEncryption() {
}

// headicon.h:30
bool CHeadIcon::GetVisible(void) const {
	return m_visible;
}

// headicon.h:31
void CHeadIcon::Show(void) {
	m_visible = true;
}

// headicon.h:32
void CHeadIcon::Hide(void) {
	m_visible = false;
}

// messengerdlg.h:369
bool CMessengerInfo::IsGameFocus(void) {
	return m_gameFocus;
}

// logininfo.h:481
bool IsWebLogin(void) {
	return CLoginInfo::Instance()->IsWebLogin() ? true : false;
}

// quadtree.h:112
void CQuadTree::SetSlopeFactor(float factor) {
	m_slopeFactor = factor;
}

// quadtree.h:157
float CQuadTree::GetLowest(void) const {
	return m_lowest;
}

// quadtree.h:423
WVector CQuadTree::GetPenaltyPos(void) {
	return m_penaltyPos;
}

// quadtree.h:497/505: CQuadTree::SetFrame / GetFrame, emitted here from
// quadtree.h (GOLFRULE_QUADTREE_INLINE).

// golfdoc.h:298
float sBar::Persent(void) const {
	return (m_power - BAR_START) / BAR_LENGTH;
}

// golfdoc.h:300
float sBar::Delta(void) const {
	return m_position - m_startPosition;
}

// golfdoc.h:650
void CGolfDoc::SetStatUpdated(bool updated) {
	m_statUpdated = updated;
}

// golfdoc.h:651
bool CGolfDoc::GetStatUpdated(void) const {
	return m_statUpdated;
}

// golfdoc.h:671
bool CGolfDoc::GetApproachReloaded(void) {
	return m_approachReloaded;
}

// golfdoc.h:672
void CGolfDoc::SetApproachReloaded(bool reloaded) {
	m_approachReloaded = reloaded;
}

// treasure.h:97-98: CTHunter::IsReturnLobby / SetReturnLobby, emitted here
// from Treasure.h (GOLFRULE_TREASURE_INLINE).

// treasure.h:101
void CTHunter::SetReceiveGiftPacket(bool receive) {
	m_bReceiveGiftPacket = receive;
}

// cardsystem.h:115
std::list<unsigned long> &CCardManager::GetActiveCardList(void) {
	return m_activeCardList;
}

// onelineboard.h:22
void COneLineBoard::ShowBg(bool show) {
	m_showBg = show;
}

// globalgamedefine.h:1132
unsigned char EncryptKey::GetKey(int index) {
	return m_key[index];
}

// taskmanager.h:106
void CTask::SetPause(bool pause) {
	m_pause = pause;
}

// shareddoc.h:1018
void CSharedDoc::SetReplayState(int state) {
	m_replayState = state;
}

// shareddoc.h:1019
int CSharedDoc::GetReplayState(void) {
	return m_replayState;
}

// shareddoc.h:1021
char *CSharedDoc::GetFileName(void) {
	return m_fileName;
}

// shareddoc.h:1022
int CSharedDoc::GetReplayPlayType(void) {
	return m_replayPlayType;
}

// shareddoc.h:1023
void CSharedDoc::SetReplayPlayType(int type) {
	m_replayPlayType = type;
}

// shareddoc.h:1150
void CSharedDoc::SetGoStopMode(bool goStop) {
	m_goStopMode = goStop;
}

// shareddoc.h:1151
bool CSharedDoc::GetGoStopMode(void) {
	return m_goStopMode;
}

// shareddoc.inl:134
bool IsOthersTurn(bool b) {
	return !IsMyTurn(b);
}

// shareddoc.inl:249
unsigned char GetEndHole(void) {
	CSharedDoc *doc = WSingleton<CSharedDoc>::Instance();
	return doc->m_holeOrder[doc->m_holes - 1];
}

// shareddoc.inl:416
void SetGameOver(bool gameOver) {
	WSingleton<CSharedDoc>::Instance()->m_gameOver = gameOver;
}

// shareddoc.inl:421
bool IsGameOver(void) {
	return WSingleton<CSharedDoc>::Instance()->m_gameOver;
}

// shareddoc.inl:540
int IsObserver(unsigned long identity) {
	if (identity == 0xFFFFFFFF)
		identity = WSingleton<CSharedDoc>::Instance()->m_userInfo.identity;

	return (identity & 0xE) == 0xE;
}

// golfrule.h:617
void CGolfRule::SetFlagCutin(bool flagCutin) {
	m_flagCutin = flagCutin;
}

// golfrule.h:618
bool CGolfRule::GetFlagCutin(void) {
	return m_flagCutin;
}

// Unreferenced in the exe and without a public: anonymous-namespace functions
// (cl 13.10 emits those even when unreferenced, a `static` one it drops).
namespace {

// golfrule.cpp:258
// The motion grade of a shot: power shot 4, else 2 for the second flag, plus
// one for the bonus flag (at most 5), looked up in the special or normal table.
int __stdcall GetShotMotionGrade(bool special, bool powerShot, bool secondFlag, bool bonus) {
	int grade = 0;

	if (powerShot)
		grade = 4;
	else if (secondFlag)
		grade = 2;

	if (bonus)
		grade++;

	if (grade >= 5)
		grade = 5;

	if (special) {
		int grades[6] = { 12, 3, 14, 3, 3, 3 };
		return grades[grade];
	} else {
		int grades[6] = { 4, 3, 4, 3, 3, 3 };
		return grades[grade];
	}
}

// golfrule.cpp:296
// The character motion name of a shot timing (-3..3, GetBarClickTiming).
const char *GetCharMotionByTiming(int timing, int, int charType, int motionMode) {
	int index = 0;

	if (motionMode != 1) {
		if (timing < -3 || timing > 3)
			index = 7;
		else
			index = timing + 4;

		if (index >= 7)
			index = 7;
	}

	bool lucia = charType == 8;

	return lucia ? g_szLuciaCharMotion[index] : g_szCommonCharMotion[index];
}

}

namespace {

// golfrule.cpp:329
// The ball's state at impact: a putter rolls it; a low-grade click past the
// start position rolls or stops it depending on the club and the character.
CGolfBall::eBallState GetInitBallState(void) {
	unsigned char clubType = GolfClub().GetType();

	if (clubType == 3)
		return CGolfBall::BALL_ROLL;

	unsigned long charType = CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_characterInfo.typeId & 0x3FFFFFF;

	if (GOLFDOC()->GetBar().m_accuracyGrade < 2) {
		bool forward = GOLFDOC()->GetBar().m_position > GOLFDOC()->GetBar().m_startPosition ? true : false;
		if (clubType == 2)
			return forward ? CGolfBall::BALL_ROLL : CGolfBall::BALL_FLY;

		if (clubType == 0) {
			if (forward || charType == 1 || charType == 5 || charType == 6)
				return CGolfBall::BALL_STOP;
			else
				return CGolfBall::BALL_ROLL;
		} else if (clubType == 1) {
			if (forward)
				return CGolfBall::BALL_ROLL;

			if (charType == 5 || charType == 6)
				return CGolfBall::BALL_STOP;
		}
	}

	return CGolfBall::BALL_FLY;
}

// golfrule.cpp:370
int GetBarClickTiming(void) {
	CGolfDoc *doc = GOLFDOC();

	switch (doc->GetBar().m_accuracyGrade) {
	case 3:
		return doc->GetBar().m_position - doc->GetBar().m_startPosition > 0.0f ? 1 : -1;
	case 2:
		return doc->GetBar().m_position > BAR_START ? 2 : -2;
	case 1:
	case 0:
		return doc->GetBar().m_position - doc->GetBar().m_startPosition > 0.0f ? 3 : -3;
	}
	return 0;
}

// golfrule.cpp:392
// The power factor of a bar click: full power for a putter or an unknown
// grade, else the root of the ground power blended by the click's error.
float GetBarClickPowerFactor(void) {
	if (GolfClub().GetType() != 3) {
		CGolfDoc *doc = GOLFDOC();

		switch (doc->GetBar().m_accuracyGrade) {
		case 4:
			return sqrtf(GetPVS().GetMaxPowerOnGround());
		case 3: {
			float delta = doc->GetBar().m_position - doc->GetBar().m_startPosition;
			float ratio;
			if (delta > 0.0f)
				ratio = delta / (GolfClub().GetNarrowAccuracy() + 140.0f - doc->GetBar().m_startPosition);
			else
				ratio = delta / (doc->GetBar().m_startPosition - (140.0f - GolfClub().GetNarrowAccuracy()));

			return sqrtf(GetPVS().GetMaxPowerOnGround() * (1.0f - Abs(ratio)) + GetPVS().GetMinPowerOnGround() * Abs(ratio));
		}
		case 2: {
			float ratio = (Abs(doc->GetBar().m_position - 140.0f) - GolfClub().GetNarrowAccuracy()) / (GolfClub().GetWideAccuracy() - GolfClub().GetNarrowAccuracy());

			return sqrtf(GetPVS().GetMinPowerOnGround() * (1.0f - ratio) + GetPVS().GetMinPowerOnGround() * ratio);
		}
		}
	}

	return 1.0f;
}

// The launch adjustments of a shot: the launch angle, the power bonus, the
// spin and curve taken from the ball, and an extra velocity, corrected by the
// bar click's accuracy grade.
struct BallSpeedFactor {
	BallSpeedFactor(unsigned long shotType, const CGolfBall &ball, float powerBonus);

	float m_angle;      // 0x0, the launch angle offset (radians)
	float m_power;      // 0x4, the power bonus ("Player" message 0x110 fills it)
	float m_spin;       // 0x8, from CGolfBall::m_lieSlope
	float m_curve;      // 0xC, from CGolfBall::m_lieAngle
	WVector m_velocity; // 0x10, an extra velocity of a miss-hit
};
CHECK_SIZE(BallSpeedFactor, 0x1C);
CHECK_FIELD(BallSpeedFactor, m_angle, 0x0);
CHECK_FIELD(BallSpeedFactor, m_power, 0x4);
CHECK_FIELD(BallSpeedFactor, m_spin, 0x8);
CHECK_FIELD(BallSpeedFactor, m_curve, 0xC);
CHECK_FIELD(BallSpeedFactor, m_velocity, 0x10);

// golfrule.cpp:582
BallSpeedFactor::BallSpeedFactor(unsigned long shotType, const CGolfBall &ball, float powerBonus)
	: m_angle(0.0f), m_power(0.0f), m_spin(ball.m_lieSlope), m_curve(ball.m_lieAngle) {
	m_velocity.Reset();
	AfxGetTask()->GetMainActor()->SendMsg(NULL, "Player", 0x110, (intptr_t)&m_power, 0, 0, 0);
	m_power += powerBonus;

	if (GolfClub().GetType() == 3)
		return;

	if (GOLFDOC()->GetBar().m_accuracyGrade == 4)
		m_angle -= m_spin * 0.34906584f;

	CGolfDoc *doc = GOLFDOC();
	if (doc->GetBar().m_accuracyGrade == 3) {
		float ratio = doc->GetBar().m_position - doc->GetBar().m_startPosition;

		if (ratio > 0.0f)
			ratio = ratio / (GolfClub().GetNarrowAccuracy() + 140.0f - doc->GetBar().m_startPosition);
		else
			ratio = ratio / (doc->GetBar().m_startPosition - (140.0f - GolfClub().GetNarrowAccuracy()));

		m_angle -= m_spin * 0.34906584f;

		if (!IsLocalContent((localContentType_t)0x50) || ball.m_powerFlags == 0)
			m_spin -= ratio * 0.2f;
	}

	if (GOLFDOC()->GetBar().m_accuracyGrade == 2) {
		float ratio = (Abs(GOLFDOC()->GetBar().m_position - 140.0f) - GolfClub().GetNarrowAccuracy()) / (GolfClub().GetWideAccuracy() - GolfClub().GetNarrowAccuracy());

		if (!IsLocalContent((localContentType_t)0x50) || ball.m_powerFlags == 0) {
			m_spin = 0.2f;
			m_curve = 0.0f;
		}

		if (GOLFDOC()->GetBar().m_position > 140.0f) {
			m_spin *= -1.0f;
			m_angle -= ratio * 0.17453292f;
		} else {
			m_angle += ratio * 0.17453292f;
		}
	}

	if (GOLFDOC()->GetBar().m_accuracyGrade == 0 || GOLFDOC()->GetBar().m_accuracyGrade == 1) {
		m_spin = 0.0f;
		m_curve = 0.0f;

		switch (GolfClub().GetType()) {
		case 0:
			if (GOLFDOC()->GetBar().m_position > GOLFDOC()->GetBar().m_startPosition) {
				m_velocity.Reset();
			} else if (shotType == 0x4000001 || shotType == 0x4000006 || shotType == 0x4000005) {
				m_velocity.Reset();
			} else {
				m_velocity = WVector(0.0f, 0.0f, 15.0f);
				m_angle += 0.69813168f;
			}
			break;
		case 1:
			if (GOLFDOC()->GetBar().m_position > GOLFDOC()->GetBar().m_startPosition)
				m_velocity = WVector(0.0f, 0.0f, 20.0f);
			else
				m_velocity = RotMat(1.3962634f, 0).za * 50.0f;

			if (shotType == 0x4000006 || shotType == 0x4000005)
				m_velocity.Reset();
			break;
		case 2:
			if (GOLFDOC()->GetBar().m_position > GOLFDOC()->GetBar().m_startPosition)
				m_velocity = WVector(0.0f, 0.0f, 5.0f);
			else
				m_velocity = RotMat(1.3962634f, 0).za * 20.0f;
			break;
		}
	}

	int grade = GOLFDOC()->GetBar().m_accuracyGrade;
	if (grade > 1) {
		if (grade == 4 && GOLFDOC()->GetBar().m_stopped && GolfClub().PowerShotType() != 0 && !(ball.m_powerFlags & 0x70) && IsLocalContent((localContentType_t)0x2F) && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000028)
			return;

		// The base's up axis (CBase::HandleMsg 0x1C1 writes a whole WVector).
		WVector spinLoss;
		AfxGetTask()->GetMainActor()->SendMsg(NULL, "Screen", 0x1C1, (intptr_t)&spinLoss, 0, 0, 0);

		if (ball.m_powerFlags & 0x40)
			m_spin -= spinLoss.x * 0.15f;
		else
			m_spin -= spinLoss.x * 0.5f;
	}
}

// A scope guard of CGolfRule::SetTurnChangeEvent: when it leaves the scope it
// asks the GroundItemMan how many field items are on the ground and, if any,
// enables them only while the player's hole score is under par (telling the
// player when it is at par).
struct sDetermineWhetherToEnableFiledItem {
	// golfrule.cpp:840
	~sDetermineWhetherToEnableFiledItem(void) {
		int count = 0;
		AfxGetTask()->GetActor("GroundItemMan") << MsgObject(NULL, 0x27C, (intptr_t)&count, 0, 0, 0, 0);

		if (count > 0) {
			unsigned char score = GetGameType() == 1 ? TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] : PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1];

			int diff = score - GOLFDOC()->GetHoleData().m_par;
			int enable = 1;
			if (diff >= 0) {
				enable = 0;

				if (diff == 0) {
					CChatMsg::Instance()->AddChatMsg("(\xBE\xCB\xB8\xB2) \xC0\xCC\xBA\xEC\xC6\xC4\xB0\xA1 \xB5\xC7\xBE\xEE \xC7\xCA\xB5\xE5 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xCC \xBB\xE7\xB6\xF3\xC1\xFD\xB4\xCF\xB4\xD9.", 0xFFFF7878, false, false);
				}
			}

			AfxGetTask()->GetActor("GroundItemMan") << MsgObject(NULL, 0x27B, enable, 0, 0, 0, 0);
		}
	}
};

} // namespace

// golfrule.cpp:870 (run-time type and factory registration of CGolfRule)
const WRTTI CGolfRule::m_RTTI("CGolfRule", &CTaskMain::m_RTTI);

IObject *CGolfRuleMakeInstance(void);

struct __impCGolfRule {
	__impCGolfRule(void) {
		ObjectFactory().AddObjectFunctor(CGolfRuleMakeInstance, "CGolfRule");
		ActorFactory().AddActorClass("CGolfRule");
	}
};
static __impCGolfRule s_impCGolfRule;

// golfrule.cpp:870
IObject *CGolfRuleMakeInstance(void) {
	return new CGolfRule;
}

// golfrule.cpp:872
const sFRESH_MSGMAP *CGolfRule::GetMessageMap(void) const {
	return &CGolfRule::_MsgMap;
}

sFRESH_MSGMAP CGolfRule::_MsgMap = { &CTaskMain::_MsgMap, &CGolfRule::_MsgEntries[0] };

sFRESH_ENTRY CGolfRule::_MsgEntries[] = {
	// golfrule.cpp:877
	{ "MAIN.chatview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnChatViewInit },
	// golfrule.cpp:878
	{ "MAIN.emoticon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnEmoticonInit },
	// golfrule.cpp:879
	{ "MAIN.emoticon", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CGolfRule::OnEmoticonBtnUp },
	// golfrule.cpp:880
	{ "MAIN.language", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnLanguageInit },
	// golfrule.cpp:881
	{ "MAIN.report", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnReportInit },
	// golfrule.cpp:882
	{ "MAIN.report", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CGolfRule::OnReportBtnUp },
	// golfrule.cpp:883
	{ "MAIN.chattarget", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnChatTargetInit },
	// golfrule.cpp:884
	{ "MAIN.chattarget", FRCMD_LBUTTONDOWN, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnChatTargetBtnDown },
	// golfrule.cpp:885
	{ "MAIN.chattarget", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&CGolfRule::OnChatTargetEnterKey },
	// golfrule.cpp:886
	{ "MAIN.chatinput", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnChatInputInit },
	// golfrule.cpp:887
	{ "MAIN.chatinput", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&CGolfRule::OnChatInputEnterKey },
	// golfrule.cpp:888
	{ "MAIN.spcard", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnSpecialCardInit },
	// golfrule.cpp:889
	{ "MAIN.spcard", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CGolfRule::OnSpecialCardBtnUp },
	// golfrule.cpp:890
	{ "MAIN.spcard", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnTopBarHoverOn },
	// golfrule.cpp:891
	{ "MAIN.spcard", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnTopBarHoverOff },
	// golfrule.cpp:892
	{ "MAIN.myinfo", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnMyInfoInit },
	// golfrule.cpp:893
	{ "MAIN.myinfo", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CGolfRule::OnMyInfoBtnUp },
	// golfrule.cpp:894
	{ "MAIN.myinfo", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnTopBarHoverOn },
	// golfrule.cpp:895
	{ "MAIN.myinfo", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnTopBarHoverOff },
	// golfrule.cpp:896
	{ "MAIN.option", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnOptionInit },
	// golfrule.cpp:897
	{ "MAIN.option", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CGolfRule::OnOptionBtnUp },
	// golfrule.cpp:898
	{ "MAIN.option", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnTopBarHoverOn },
	// golfrule.cpp:899
	{ "MAIN.option", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnTopBarHoverOff },
	// golfrule.cpp:900
	{ "MAIN.caddiebook", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnCaddieBookInit },
	// golfrule.cpp:901
	{ "MAIN.caddiebook", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CGolfRule::OnCaddieBookBtnUp },
	// golfrule.cpp:902
	{ "MAIN.unplayable", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnUnplayableInit },
	// golfrule.cpp:903
	{ "MAIN.unplayable", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CGolfRule::OnUnplayableBtnUp },
	// golfrule.cpp:904
	{ "MAIN.pause", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnPauseInit },
	// golfrule.cpp:905
	{ "MAIN.pause", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CGolfRule::OnPauseBtnUp },
	// golfrule.cpp:906
	{ "MAIN.record", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnChatRecordInit },
	// golfrule.cpp:907
	{ "MAIN.record", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CGolfRule::OnChatRecordBtnUp },
	// golfrule.cpp:908
	{ "MAIN.onelineview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnOnelineViewInit },
	// golfrule.cpp:909
	{ "MAIN.onelineview", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnOnelineViewOwnerDraw },
	// golfrule.cpp:910
	{ "MAIN.onelinebtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnOnelineBtnInit },
	// golfrule.cpp:911
	{ "MAIN.onelinebtn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CGolfRule::OnOnelineBtnUp },
	// golfrule.cpp:912
	{ "MAIN.tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnBarTooltipOwnerDraw },
	// golfrule.cpp:914
	{ "MAIN.messenger", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnMessengerBtnInit },
	// golfrule.cpp:915
	{ "MAIN.messenger", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CGolfRule::OnMessengerBtnUp },
	// golfrule.cpp:916
	{ "MAIN.messenger", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnTopBarHoverOn },
	// golfrule.cpp:917
	{ "MAIN.messenger", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnTopBarHoverOff },
	// golfrule.cpp:918
	{ "MAIN.messenger_bubble", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnMessengerBubbleInit },
	// golfrule.cpp:919
	{ "MAIN.messenger_bubble", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnMessengerBubbleOwnerDraw },
	// golfrule.cpp:920
	{ "MAIN.messenger_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnMessengerBubbleEditInit },
	// golfrule.cpp:921
	{ "MAIN.messenger_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnMessenger_AlarmBtnInit },
	// golfrule.cpp:922
	{ "MAIN.messenger_alarm", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CGolfRule::OnMessenger_AlarmBtnUp },
	// golfrule.cpp:923
	{ "MAIN.gift_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnMessenger_GiftAlarmBtnInit },
	// golfrule.cpp:924
	{ "MAIN.gift_alarm", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CGolfRule::OnMessenger_GiftAlarmBtnUp },
	// golfrule.cpp:925
	{ "MAIN.replay_status", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnReplayStatusInit },
	// golfrule.cpp:928
	{ "MAIN.predict_normal", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnRadioPredictNormalShotInit },
	// golfrule.cpp:929
	{ "MAIN.predict_tomahwak", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnRadioPredictTomahwakShotInit },
	// golfrule.cpp:930
	{ "MAIN.predict_cobra", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnRadioPredictCobraShotInit },
	// golfrule.cpp:931
	{ "MAIN.predict_spike", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnRadioPredictSpikeShotInit },
	// golfrule.cpp:932
	{ "MAIN.predict_normal", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CGolfRule::OnRadioPredictNormalShotDown },
	// golfrule.cpp:933
	{ "MAIN.predict_tomahwak", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CGolfRule::OnRadioPredictTomahwakShotDown },
	// golfrule.cpp:934
	{ "MAIN.predict_cobra", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CGolfRule::OnRadioPredictCobraShotDown },
	// golfrule.cpp:935
	{ "MAIN.predict_spike", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CGolfRule::OnRadioPredictSpikeShotDown },
	// golfrule.cpp:936
	{ "MAIN.predict_normal", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnRadioPredictNormalShotDraw },
	// golfrule.cpp:937
	{ "MAIN.predict_tomahwak", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnRadioPredictTomahwakShotDraw },
	// golfrule.cpp:938
	{ "MAIN.predict_cobra", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnRadioPredictCobraShotDraw },
	// golfrule.cpp:939
	{ "MAIN.predict_spike", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGolfRule::OnRadioPredictSpikeShotDraw },
	// golfrule.cpp:942
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// golfrule.cpp:960
void CGolfRule::OnChatViewInit(PG_MSGARG param) {
	m_chatViewWnd = (FrEdit *)((__rtti_obj = (void *)param) != 0 ? (((IObject *)__rtti_obj)->IsKindOf(&FrEdit::m_RTTI) ? (void *)param : 0) : 0);
	if (m_chatViewWnd) {
		m_chatViewWnd->m_nFlags.Enable(0x1000);

		WRect rect = m_chatViewWnd->GetRect();
		int lines = (COption::Instance()->gIsChatLineEntended() || WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_userLimit >= 100) ? 10 : 5;

		rect.h = lines * 19.0f + 8.0f;
		rect.y = COption::Instance()->gGetBar_Y() - 100.0f - rect.h;
		m_chatViewWnd->SetRect(rect);
		m_chatViewWnd->GetScrollBar()->Resize(lines, 1, false);
		m_chatViewWnd->GetScrollBar()->SetVisible(false);
		HandleMsg(MsgObject(0, 0x63, 0, 0, 0, 0, 0));
	}
}

// golfrule.cpp:998
void CGolfRule::OnEmoticonInit(intptr_t param) {
	m_emoticonBtn = DYNAMIC_CAST(FrButton, param);
	if (m_emoticonBtn) {
		m_emoticonBtn->SetVisible(false);

		WPoint pt;
		pt.x = m_emoticonBtn->GetRect().x;
		pt.y = COption::Instance()->gGetBar_Y() + 16.0f;
		m_emoticonBtn->MoveWindow(pt);
	}
}

// golfrule.cpp:1015
void CGolfRule::OnEmoticonBtnUp(void) {
	if (m_emoticonDlg == NULL) {
		m_emoticonDlg = CreateForm<FrEmoticonDlg>(g_pFresh->GetManager(), this, "emoticon", 0);
		if (m_emoticonDlg) {
			WPoint pt(0.0f, 0.0f);
			if (m_emoticonBtn) {
				pt.x = m_emoticonBtn->GetRect().x - 200.0f;
				pt.y = m_emoticonBtn->GetRect().y - 260.0f;
			}

			if (!g_pFresh->GetManager()->IsKeyExclusive())
				g_pFresh->GetManager()->SetExclusiveKey(true);

			m_emoticonDlg->Open((FRESH_RESULT_CALLBACK)&CGolfRule::OnEmoticonResult, pt, 3);
		}
	} else {
		m_emoticonDlg->Close(FrCANCEL, true);
	}
}

// golfrule.cpp:1044
void CGolfRule::OnLanguageInit(PG_MSGARG param) {
	m_languageArea = DYNAMIC_CAST(FrArea, param);
	if (m_languageArea) {
		m_languageArea->SetVisible(false);

		if (IsLocalCountry(1) && g_ime->IsAlphaNumericMode())
			m_languageArea->SetBgImg("chat_eng");
		else
			m_languageArea->SetBgImg("chat_kor");

		WPoint pt;
		pt.x = m_languageArea->GetRect().x;
		pt.y = COption::Instance()->gGetBar_Y() + 16.0f;
		m_languageArea->MoveWindow(pt);
	}
}

// golfrule.cpp:1076
void CGolfRule::OnReportInit(PG_MSGARG param) {
	m_reportBtn = DYNAMIC_CAST(FrButton, param);
	if (m_reportBtn) {
		m_reportBtn->SetVisible(false);

		WPoint pt;
		pt.x = m_reportBtn->GetRect().x;
		pt.y = COption::Instance()->gGetBar_Y() + 16.0f;
		m_reportBtn->MoveWindow(pt);
	}
}

// golfrule.cpp:1092
void CGolfRule::OnReportBtnUp(void) {
	if (GOLFDOC()->m_reportSent) {
		FrForm *form = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "A report on the current game has already been received."
		form->SetMessage("\xC7\xF6\xC0\xE7 \xB0\xD4\xC0\xD3\xBF\xA1 \xB4\xEB\xC7\xD8\xBC\xAD \xC0\xCC\xB9\xCC \xBD\xC5\xB0\xED\xB0\xA1 \xC1\xA2\xBC\xF6\xB5\xC7\xBE\xEE \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.", false);
		form->Open((FRESH_RESULT_CALLBACK)0, 0);
		return;
	}

	if (m_reportDlg == NULL) {
		m_reportDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);

		// "Reporting sends the whole chat of the current game to the operators..."
		m_reportDlg->SetMessage("\xBD\xC5\xB0\xED\xB8\xA6 \xC7\xCF\xBD\xC3\xB8\xE9 \xC7\xF6\xC0\xE7\xB0\xD4\xC0\xD3\xC0\xC7 \xB4\xEB\xC8\xAD \xB3\xBB\xBF\xEB\xC0\xCC \xB8\xF0\xB5\xCE \xBF\xEE\xBF\xB5\xC0\xDA\xBF\xA1\xB0\xD4 \xBD\xC5\xB0\xED\xB0\xA1 \xB5\xCB\xB4\xCF\xB4\xD9.\n\n\xB4\xEB\xC8\xAD \xB3\xBB\xBF\xEB \xC1\xDF \xC3\xB3\xB9\xFA\xC7\xD8\xB4\xE7\xBB\xE7\xC7\xD7\xC0\xCC \xC0\xD6\xB4\xC2 \\c0xffff0000\\c\xB8\xF0\xB5\xE7 \xBE\xC6\xC0\xCC\xB5\xF0\\c0xff000000\\c\xB4\xC2 \xBA\xED\xB7\xB0 \xC3\xB3\xB8\xAE\xB8\xA6 \xB4\xE7\xC7\xCF\xB0\xD4 \xB5\xCB\xB4\xCF\xB4\xD9.\n\n\xBD\xC5\xB0\xED \xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);
		m_reportDlg->Open((FRESH_RESULT_CALLBACK)&CGolfRule::OnReportResult, 0);
	}
}

// golfrule.cpp:1112
void CGolfRule::OnChatTargetInit(intptr_t param) {
	m_chatEdit = DYNAMIC_CAST(FrComboBox, param);
	if (m_chatEdit) {
		m_chatEdit->SetVisible(false);
		WSingleton<CSharedDoc>::Instance()->InitReservedChatList(m_chatEdit,
			WSingleton<CSharedDoc>::Instance()->m_roomType == 5 || WSingleton<CSharedDoc>::Instance()->m_roomType == 6
				? (eReservedChatPartner)(RESERVED_CHAT_PARTNER_TEAM | RESERVED_CHAT_PARTNER_GUILD)
				: RESERVED_CHAT_PARTNER_GUILD);

		WPoint pt;
		pt.x = m_chatEdit->GetRect().x;
		pt.y = COption::Instance()->gGetBar_Y() + 14.0f;
		m_chatEdit->MoveWindow(pt);
	}
}

// golfrule.cpp:1131
void CGolfRule::OnChatTargetBtnDown(PG_MSGARG) {
	if (m_chatInput)
		m_chatInput->SetKeyFocus(true);
}

// golfrule.cpp:1138
bool CGolfRule::OnChatTargetEnterKey(PG_MSGARG) {
	if (m_chatEdit && strlen(m_chatEdit->GetLine(1, false)) == 0)
		m_chatEdit->SetLine(1, "\xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4", 0, false, 0);

	if (m_chatInput)
		m_chatInput->SetKeyFocus(true);

	return false;
}

// golfrule.cpp:1150
void CGolfRule::OnChatInputInit(intptr_t param) {
	m_chatInput = DYNAMIC_CAST(FrEdit, param);
	if (m_chatInput) {
		m_chatInput->SetVisible(false);

		WPoint pt;
		pt.x = m_chatInput->GetRect().x;
		pt.y = COption::Instance()->gGetBar_Y() + 14.0f;
		m_chatInput->MoveWindow(pt);
	}
}

// golfrule.cpp:1175
bool CGolfRule::OnChatInputEnterKey(PG_MSGARG param) {
	const char *text = (const char *)param;

	if (strlen(text)) {
		if (!GOLFDOC()->IsShowChat()) {
			// "the chat window is hidden" (CP949)
			CChatMsg::Instance()->AddChatMsg("\xC1\xF6\xB1\xDD\xC0\xBA \xB4\xEB\xC8\xAD\xC3\xA2\xC0\xBB \xBC\xFB\xB1\xE4 \xBB\xF3\xC5\xC2\xC0\xD4\xB4\xCF\xB4\xD9.", 0xFFFF7878, true, false);
			return true;
		}

		if (!CSharedDoc::Instance()->UTIL_SendChatMessage(m_chatEdit, text, false)) {
			// "to everyone" (CP949)
			m_chatEdit->SetLine(1, "\xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4", 0, false, 0);
			// "wrong whisper target, switched to \"to everyone\"" (CP949)
			CChatMsg::Instance()->AddChatMsg("\xB1\xD3\xB8\xBB \xBB\xF3\xB4\xEB\xB0\xA1 \xC0\xDF\xB8\xF8\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9. \"\xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4\"\xB7\xCE \xBA\xAF\xB0\xE6\xC7\xD5\xB4\xCF\xB4\xD9.", 0xFFFF7878, true, false);
		}

		if (OnlinePlay()) {
			CHeadIcon::Instance()->SendIcon(0xFFFF);
			return true;
		}

		std::string target = m_chatEdit->GetLine(1, false);
		if (target == "\xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4") {
			std::string msg = CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_name;
			msg += " : \x08";
			msg += text;

			if (CSharedDoc::Instance()->m_gameMode == 5) {
				char filtered[1024] = { 0 };
				strncpy(filtered, text, strlen(text) < 0x400 ? strlen(text) : 0x3FF);
				if (CSharedDoc::Instance()->m_chatManager.Filtering(filtered)) {
					// "(notice) swearing is not allowed" (CP949)
					CChatMsg::Instance()->AddChatMsg(std::string("(\xBE\xCB\xB8\xB2)\xBF\xE5\xBC\xB3\xC0\xBA \xBB\xE7\xBF\xEB\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9."), 0xFFFF8000, false, false);
				} else {
					CChatMsg::Instance()->AddChatMsg(msg, 0xFFFFCC00, false, false);
				}
			} else {
				CChatMsg::Instance()->AddChatMsg(msg, 0xFFFFCC00, false, false);
			}
		}
	} else {
		CChatMsg::Instance()->Close(true, true);
	}

	return true;
}

// golfrule.cpp:1240
void CGolfRule::OnSpecialCardInit(PG_MSGARG param) {
	m_barButton[10] = DYNAMIC_CAST(FrButton, param);
	if (m_barButton[10]) {
		m_barButton[10]->SetVisible(false);
		m_barButton[10]->EnableHover(true, 0.0f);
		m_barButton[10]->EnableBlink(CCardManager::Instance()->ShowSPCardAlarm());

		WPoint pt;
		pt.x = g_view->GetWidth() - 106.0f;
		pt.y = m_barButton[10]->GetRect().y;
		m_barButton[10]->MoveWindow(pt);
	}
}

// golfrule.cpp:1256
void CGolfRule::OnSpecialCardBtnUp(void) {
	CTaskMain::OnOverBar_SpecialCardUp();
}

// golfrule.cpp:1261
void CGolfRule::OnMyInfoInit(PG_MSGARG param) {
	m_myInfoBtn = DYNAMIC_CAST(FrButton, param);
	if (m_myInfoBtn) {
		m_myInfoBtn->SetVisible(false);
		m_myInfoBtn->EnableHover(true, 0.0f);

		WPoint pt;
		pt.x = g_view->GetWidth() - 54.0f;
		pt.y = m_myInfoBtn->GetRect().y;
		m_myInfoBtn->MoveWindow(pt);
	}
}

// golfrule.cpp:1276
void CGolfRule::OnMyInfoBtnUp(void) {
	if (CUserInfo::Instance()) {
		CUserInfo::Instance()->SetInfo(CSharedDoc::Instance()->GetPangyaUserInfo().uid, CSharedDoc::Instance()->GetPangyaUserInfo().guid, true, true, false, false, CSharedDoc::Instance()->GetPangyaUserInfo().nickname);
	}
	OnBarHoverOff(0);
}

// golfrule.cpp:1286
void CGolfRule::OnOptionInit(PG_MSGARG param) {
	m_optionBtn = DYNAMIC_CAST(FrButton, param);
	if (m_optionBtn) {
		m_optionBtn->SetVisible(false);
		m_optionBtn->EnableHover(true, 0.0f);

		WPoint pt;
		pt.x = g_view->GetWidth() - 29.0f;
		pt.y = m_myInfoBtn->GetRect().y;
		m_optionBtn->MoveWindow(pt);
	}
}

// golfrule.cpp:1301
void CGolfRule::OnOptionBtnUp(void) {
	if (CQuitWindow::Instance()) {
		CQuitWindow::Instance()->CreateOptionWindow();
	}
	OnBarHoverOff(0);
}

// golfrule.cpp:1311
void CGolfRule::OnCaddieBookInit(PG_MSGARG param) {
	m_commandBtns[0] = DYNAMIC_CAST(FrButton, param);
	if (m_commandBtns[0]) {
		m_commandBtns[0]->SetVisible(false);

		WPoint pt;
		pt.x = g_view->GetWidth() - 423.0f;
		pt.y = m_commandBtns[0]->GetRect().y;
		m_commandBtns[0]->MoveWindow(pt);

		ResetUiPos_Board();
	}
}

// golfrule.cpp:1327
void CGolfRule::OnCaddieBookBtnUp(void) {
	SendMsg(this, "GuideBook", 0x12A, 0, 0, 0, 0);
}

// golfrule.cpp:1332
void CGolfRule::OnUnplayableInit(PG_MSGARG param) {
	m_commandBtns[1] = DYNAMIC_CAST(FrButton, param);
	if (m_commandBtns[1]) {
		m_commandBtns[1]->SetVisible(false);

		WPoint pt;
		pt.x = g_view->GetWidth() - 398.0f;
		pt.y = m_commandBtns[1]->GetRect().y;
		m_commandBtns[1]->MoveWindow(pt);
	}
}

// golfrule.cpp:1346
void CGolfRule::OnUnplayableBtnUp(void) {
	if (GOLFDOC()->m_approachMode) {
		if (CHelpMsg::IsInstantiated())
			CHelpMsg::Instance()->SetHelpMsg(0x17, 0);
		return;
	}

	if (GOLFDOC()->GameMode() & 0x48108)
		SendMsg(this, "Screen", 0x1CB, 0, 0, 0, 0);
}

// golfrule.cpp:1360
void CGolfRule::OnPauseInit(PG_MSGARG param) {
	m_commandBtns[2] = DYNAMIC_CAST(FrButton, param);
	if (m_commandBtns[2]) {
		m_commandBtns[2]->SetVisible(false);

		WPoint pt;
		pt.x = g_view->GetWidth() - 373.0f;
		pt.y = m_commandBtns[2]->GetRect().y;
		m_commandBtns[2]->MoveWindow(pt);
	}
}

// golfrule.cpp:1374
void CGolfRule::OnPauseBtnUp(void) {
	switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
	case 9:
		if (CHelpMsg::IsInstantiated())
			CHelpMsg::Instance()->SetHelpMsg(0x18, 0);
		break;

	default:
		if (GOLFDOC()->GameMode() & 0x48108)
			SendMsg(this, "Screen", 0x1CC, 0, 0, 0, 0);
		break;
	}
}

// golfrule.cpp:1388
void CGolfRule::OnChatRecordInit(PG_MSGARG param) {
	m_commandBtns[3] = DYNAMIC_CAST(FrButton, param);
	if (m_commandBtns[3]) {
		m_commandBtns[3]->SetVisible(false);

		WPoint pt;
		pt.x = g_view->GetWidth() - 348.0f;
		pt.y = m_commandBtns[3]->GetRect().y;
		m_commandBtns[3]->MoveWindow(pt);
	}
}

// golfrule.cpp:1402
void CGolfRule::OnChatRecordBtnUp(void) {
	SendMsg(this, "Screen", 0x1CD, 0, 0, 0, 0);
}

// golfrule.cpp:1407
void CGolfRule::OnOnelineViewInit(PG_MSGARG param) {
	m_onelineBoardWnd = DYNAMIC_CAST(FrArea, param);
	m_onelineBoardWnd->SetVisible(false);

	COneLineBoard::Instance()->SetRect(m_onelineBoardWnd->GetRect());
	COneLineBoard::Instance()->LoadBg();
}

// golfrule.cpp:1417
void CGolfRule::OnOnelineViewOwnerDraw(PG_MSGARG) {
	COneLineBoard::Instance()->Display();
}

// golfrule.cpp:1422
void CGolfRule::OnOnelineBtnInit(PG_MSGARG param) {
	m_boardWnd = (FrButton *)((__rtti_obj = (void *)param) != 0 ? (((IObject *)__rtti_obj)->IsKindOf(&FrButton::m_RTTI) ? (void *)param : 0) : 0);

	WRect rect = m_boardWnd->GetRect();
	rect.x = g_view->GetWidth() - 135.0f;
	m_boardWnd->SetRect(rect);

	m_boardWnd->EnableHover(true, 0.0f);

	m_boardWnd->SetVisible(false);
	COneLineBoard::Instance()->ShowBg(false);
}

// golfrule.cpp:1436
bool CGolfRule::OnOnelineReqChkOutDlgResult(int result, FrForm *) {
	if (result == 1) {
		WSendPacket packet(GCT_ONELINE_REQUEST);
		packet.EncodeStr(COneLineBoard::Instance()->GetMessageA());
		packet.Send(SEND_TO_DEFAULT);
	}
	return true;
}

// golfrule.cpp:1447
bool CGolfRule::OnOnelineReqDlgResult(int result, FrForm *form) {
	if (!CChatMsg::Instance()->IsOpened())
		g_pFresh->GetManager()->SetExclusiveKey(false);

	if (result == 1) {
		char *text = (char *)((FrOnelineReqDlg *)form)->GetInputString();

		if (WSingleton<CSharedDoc>::Instance()->m_chatManager.Filtering(text)) {
			// "contains an inappropriate word" (CP949)
			FrForm *notify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			notify->SetMessage("\xC0\xFB\xB4\xE7\xC7\xCF\xC1\xF6\xBE\xCA\xC0\xBA \xB4\xDC\xBE\xEE\xB0\xA1 \xC6\xF7\xC7\xD4\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			notify->Open(0, 3);
			m_onelineReqDlg = NULL;
			return true;
		}

		if (*text == '\0') {
			// "there is nothing to send" (CP949)
			FrForm *notify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			notify->SetMessage("\xBA\xB8\xB3\xBE \xB3\xBB\xBF\xEB\xC0\xCC \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			notify->Open(0, 3);
			m_onelineReqDlg = NULL;
			return true;
		}

		WSingleton<COneLineBoard>::Instance()->SetMessage(text);

		WSendPacket packet(GCT_ONELINE_QUERY);
		packet.Send(SEND_TO_DEFAULT);
	}

	m_onelineReqDlg = NULL;

	return true;
}

// golfrule.cpp:1488
void CGolfRule::OnOnelineBtnUp(void) {
	if (WSingleton<CSharedDoc>::Instance()->IsControlServerService(0x20000)) {
		// "the one-line board is under maintenance" (CP949)
		this << MsgObject(this, 0x23, (intptr_t)"\xC0\xFC\xB1\xA4\xC6\xC7 \xB0\xFC\xB7\xC3 \xBA\xCE\xBA\xD0 \xC1\xA1\xB0\xCB\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
		return;
	}

	m_onelineReqDlg = CreateForm<FrOnelineReqDlg>(g_pFresh->GetManager(), this, "oneline_req", 0);
	if (m_onelineReqDlg) {
		m_onelineReqDlg->Open((FRESH_RESULT_CALLBACK)&CGolfRule::OnOnelineReqDlgResult, 0x11);

		if (!g_pFresh->GetManager()->IsKeyExclusive())
			g_pFresh->GetManager()->SetExclusiveKey(true);

		m_onelineReqDlg->SetIconRect(m_boardWnd->GetRect());
	}
}

// golfrule.cpp:1519
void CGolfRule::OnTopBarHoverOn(PG_MSGARG param) {
	m_hoverButton = DYNAMIC_CAST(FrButton, param);

	if (m_hoverButton) {
		if (m_hoverButton == m_messengerWnd)
			CIconToolTip::GetInstance()->SetToolTip((eBarButton)11);
		else if (m_hoverButton == m_myInfoBtn)
			CIconToolTip::GetInstance()->SetToolTip((eBarButton)12);
		else if (m_hoverButton == m_optionBtn)
			CIconToolTip::GetInstance()->SetToolTip((eBarButton)13);
		else if (m_hoverButton == m_barButton[10])
			CIconToolTip::GetInstance()->SetToolTip((eBarButton)10);
	}
}

// golfrule.cpp:1536
void CGolfRule::OnTopBarHoverOff(PG_MSGARG param) {
	FrButton *btn = DYNAMIC_CAST(FrButton, param);

	if (btn == NULL || btn == m_hoverButton) {
		m_hoverButton = NULL;
		CIconToolTip::GetInstance()->ClearToolTip();
	}
}

// golfrule.cpp:1548
void CGolfRule::InitRadioPredictShotButton(FrButton **button, int wnd, float offset) {
	FrButton *btn = DYNAMIC_CAST(FrButton, wnd);

	if (btn) {
		WPoint pt(g_view->GetWidth() - 180.0f, g_view->GetHeight() - offset);
		btn->MoveWindow(pt);
		btn->SetPushDelay(0.1f);
	}
	*button = btn;
}

// golfrule.cpp:1568
void CGolfRule::SetRadioPredictShotPress(FrButton *button) {
	if (m_predictNormalShotBtn) m_predictNormalShotBtn->SetStatus(FrButton::NORMAL);
	if (m_predictTomahwakShotBtn) m_predictTomahwakShotBtn->SetStatus(FrButton::NORMAL);
	if (m_predictCobraShotBtn) m_predictCobraShotBtn->SetStatus(FrButton::NORMAL);
	if (m_predictSpikeShotBtn) m_predictSpikeShotBtn->SetStatus(FrButton::NORMAL);

	if (button) button->SetStatus(FrButton::PRESSED);

	int type = 0;
	if (button == m_predictTomahwakShotBtn) type = 1;
	if (button == m_predictCobraShotBtn) type = 2;
	if (button == m_predictSpikeShotBtn) type = 3;
	SendMsg(this, "ShotPreview", -1, (intptr_t)"CHANGE_PREDICT_TYPE", type, 0, 0);
}

// golfrule.cpp:1584
void CGolfRule::UpdateRadioPredictShotButtonStatus(void) {
	bool visible = GolfClub().GetType() != 3;

	if (m_predictNormalShotBtn) m_predictNormalShotBtn->SetVisible(visible);
	if (m_predictTomahwakShotBtn) m_predictTomahwakShotBtn->SetVisible(visible);
	if (m_predictCobraShotBtn) m_predictCobraShotBtn->SetVisible(visible);
	if (m_predictSpikeShotBtn) m_predictSpikeShotBtn->SetVisible(visible);
}

// golfrule.cpp:1596
void CGolfRule::OnRadioPredictNormalShotInit(PG_MSGARG wnd) {
	InitRadioPredictShotButton(&m_predictNormalShotBtn, wnd, 100.0f);
	if (m_predictNormalShotBtn)
		m_predictNormalShotBtn->SetStatus(FrButton::PRESSED);
}

// golfrule.cpp:1600
void CGolfRule::OnRadioPredictTomahwakShotInit(PG_MSGARG wnd) {
	InitRadioPredictShotButton(&m_predictTomahwakShotBtn, wnd, 80.0f);
}

// golfrule.cpp:1603
void CGolfRule::OnRadioPredictCobraShotInit(PG_MSGARG wnd) {
	InitRadioPredictShotButton(&m_predictCobraShotBtn, wnd, 60.0f);
}

// golfrule.cpp:1606
void CGolfRule::OnRadioPredictSpikeShotInit(PG_MSGARG wnd) {
	InitRadioPredictShotButton(&m_predictSpikeShotBtn, wnd, 40.0f);
}

// golfrule.cpp:1613
void CGolfRule::OnRadioPredictNormalShotDown(void) {
	if (m_predictNormalShotBtn) {
		if (m_predictNormalShotBtn->GetStatus() != FrButton::PRESSED)
			SetRadioPredictShotPress(m_predictNormalShotBtn);
	}
}

// golfrule.cpp:1618
void CGolfRule::OnRadioPredictTomahwakShotDown(void) {
	if (m_predictTomahwakShotBtn) {
		if (GolfClub().PowerShotType()) SetRadioPredictShotPress(m_predictTomahwakShotBtn);
		else m_predictTomahwakShotBtn->SetStatus(FrButton::NORMAL);
	}
}

// golfrule.cpp:1623
void CGolfRule::OnRadioPredictCobraShotDown(void) {
	if (m_predictCobraShotBtn) {
		if (GolfClub().PowerShotType()) SetRadioPredictShotPress(m_predictCobraShotBtn);
		else m_predictCobraShotBtn->SetStatus(FrButton::NORMAL);
	}
}

// golfrule.cpp:1628
void CGolfRule::OnRadioPredictSpikeShotDown(void) {
	if (m_predictSpikeShotBtn) {
		if (GolfClub().PowerShotType()) SetRadioPredictShotPress(m_predictSpikeShotBtn);
		else m_predictSpikeShotBtn->SetStatus(FrButton::NORMAL);
	}
}

// golfrule.cpp:1638
void CGolfRule::OnRadioPredictNormalShotDraw(PG_MSGARG) {
	FrButton *button = m_predictNormalShotBtn;
	if (button) {
		button->MoveWindow(WPoint(g_view->GetWidth() - 180.0f, g_view->GetHeight() - 100.0f));

		WRect rect = button->GetRect();
		float x = rect.x + rect.w - 35.0f;
		if (button->GetStatus() == FrButton::PRESSED)
			m_predictShotUI->SetColor(0xFFFFFFFF);
		else
			m_predictShotUI->SetColor(0x80808080);

		m_predictShotUI->Render("Pangya", x, rect.y, 1.0f, 0);
	}
}

// golfrule.cpp:1653
void CGolfRule::OnRadioPredictTomahwakShotDraw(PG_MSGARG) {
	FrButton *button = m_predictTomahwakShotBtn;
	if (button) {
		if (!GolfClub().PowerShotType())
			SetRadioPredictShotPress(m_predictNormalShotBtn);

		button->MoveWindow(WPoint(g_view->GetWidth() - 180.0f, g_view->GetHeight() - 80.0f));
		button->Enable(GolfClub().PowerShotType() != 0);

		WRect rect = button->GetRect();
		float x = rect.x + rect.w - 23.0f;
		if (button->GetStatus() == FrButton::PRESSED)
			m_predictShotUI->SetColor(0xFFFFFFFF);
		else
			m_predictShotUI->SetColor(0x80808080);

		m_predictShotUI->Render("Tomahawk", x, rect.y, 1.0f, 0);
	}
}

// golfrule.cpp:1671
void CGolfRule::OnRadioPredictCobraShotDraw(PG_MSGARG) {
	FrButton *button = m_predictCobraShotBtn;
	if (button) {
		if (!GolfClub().PowerShotType())
			SetRadioPredictShotPress(m_predictNormalShotBtn);

		button->MoveWindow(WPoint(g_view->GetWidth() - 180.0f, g_view->GetHeight() - 60.0f));
		button->Enable(GolfClub().PowerShotType() != 0);

		WRect rect = button->GetRect();
		float x = rect.x + rect.w - 40.0f;
		if (button->GetStatus() == FrButton::PRESSED)
			m_predictShotUI->SetColor(0xFFFFFFFF);
		else
			m_predictShotUI->SetColor(0x80808080);

		m_predictShotUI->Render("Cobra", x, rect.y, 1.0f, 0);
	}
}

// golfrule.cpp:1689
void CGolfRule::OnRadioPredictSpikeShotDraw(PG_MSGARG) {
	FrButton *button = m_predictSpikeShotBtn;
	if (button) {
		if (!GolfClub().PowerShotType())
			SetRadioPredictShotPress(m_predictNormalShotBtn);

		button->MoveWindow(WPoint(g_view->GetWidth() - 180.0f, g_view->GetHeight() - 40.0f));
		button->Enable(GolfClub().PowerShotType() != 0);

		WRect rect = button->GetRect();
		float x = rect.x + rect.w - 40.0f;
		if (button->GetStatus() == FrButton::PRESSED)
			m_predictShotUI->SetColor(0xFFFFFFFF);
		else
			m_predictShotUI->SetColor(0x80808080);

		m_predictShotUI->Render("Spike", x, rect.y, 1.0f, 0);
	}
}

// golfrule.cpp:1708
CGolfRule::CGolfRule(void) {
	m_priority = 0;
	m_chatViewWnd = NULL;
	m_emoticonBtn = NULL;
	m_languageArea = NULL;
	m_reportBtn = NULL;
	m_chatEdit = NULL;
	m_chatInput = NULL;
	m_emoticonDlg = NULL;
	m_reportDlg = NULL;
	m_myInfoBtn = NULL;
	m_optionBtn = NULL;
	m_commandBtns[0] = NULL;
	m_commandBtns[1] = NULL;
	m_commandBtns[2] = NULL;
	m_commandBtns[3] = NULL;
	m_onelineBoardWnd = NULL;
	m_boardWnd = NULL;
	m_forgetDlg = NULL;
	m_forgetPending = false;
	m_forgetElapsed = 0.0f;
	m_cancelShotItemSlot = 0x1800002D;
	m_playerInfoDlg = NULL;
	m_onelineReqDlg = NULL;
	m_approachResultDlg = NULL;
	m_approachEndDlg = NULL;
	m_flag4C4 = false;
	memset(m_closedBonusBalls, 0, sizeof(m_closedBonusBalls));
	m_pRule = NULL;

	if (IsLocalContent((localContentType_t)0x50)) {
		m_inputCommandHead = m_inputCommandTail = 0;
		m_value1E8 = 0;
	}

	if (IsLocalContent((localContentType_t)0x4F)) {
		m_replayBox = NULL;
		m_recordAviDlg = NULL;

		CReplayControlManager::Instance()->Init();
		m_value59C = 0;
		m_value5A0 = 0;
	}

	CSharedDoc::Instance()->SetGoStopMode(false);

	m_predictShotUI = new CSimpleUI("[impact.png");
	m_predictShotUI->Add("Pangya", WRect(0.0f, 18.0f, 128.0f, 18.0f), false);
	m_predictShotUI->Add("Tomahawk", WRect(0.0f, 72.0f, 128.0f, 18.0f), false);
	m_predictShotUI->Add("Cobra", WRect(0.0f, 126.0f, 128.0f, 18.0f), false);
	m_predictShotUI->Add("Spike", WRect(0.0f, 144.0f, 128.0f, 18.0f), false);

	m_predictNormalShotBtn = NULL;
	m_predictCobraShotBtn = NULL;
	m_predictSpikeShotBtn = NULL;
	m_predictTomahwakShotBtn = NULL;
	m_topViewMode = 0;
}

// golfrule.cpp:1787
CGolfRule::~CGolfRule(void) {
	if (m_predictShotUI) { delete m_predictShotUI; m_predictShotUI = NULL; }

	if (m_pRule) { delete m_pRule; m_pRule = NULL; }
}

// golfrule.cpp:1798
void CGolfRule::SendHoleData(void) {
	// The 0x19-byte hole record of packet 0x1A.
#pragma pack(push, 1)
	struct sHoleRecord {
		int m_reserved;             // 0x0, always 0
		unsigned long m_holeKey;    // 0x4, sHoleData::m_holeKey
		unsigned char m_par;        // 0x8
		float m_teeX;               // 0x9
		float m_teeZ;               // 0xD
		float m_cupX;               // 0x11
		float m_cupZ;               // 0x15
	};
#pragma pack(pop)
	float teeX = GOLFDOC()->GetHoleData().m_teePos.x;
	float teeZ = GOLFDOC()->GetHoleData().m_teePos.z;
	float cupX = GOLFDOC()->GetHoleData().m_cupPos.x;
	float cupZ = GOLFDOC()->GetHoleData().m_cupPos.z;
	unsigned char par = GOLFDOC()->GetHoleData().m_par;

	sHoleRecord record;
	record.m_teeX = teeX;
	record.m_teeZ = teeZ;
	record.m_cupX = cupX;
	record.m_cupZ = cupZ;
	record.m_par = par;
	record.m_holeKey = GOLFDOC()->GetHoleData().m_holeKey;
	record.m_reserved = 0;

	WSendPacket packet(0x1A);
	packet.Encode1(GOLFDOC()->m_currentHole);
	packet.EncodeBuffer(&record, sizeof(record));
	packet.Send(SEND_TO_DEFAULT);
}

// golfrule.cpp:1825
void CGolfRule::OnPreLoadInit(void) {
	CTaskMain::OnPreLoadInit();

	for (unsigned char i = 0; i < 12; i++) {
		CSharedDoc::Instance()->m_awardPlayers[i].m_uid = 0xFFFFFFFF;
		CSharedDoc::Instance()->m_awardPlayers[i].m_pang = 0;
	}

	CSharedDoc::Instance()->m_quitGamePang = 0;

	m_shotFrame = 0;

	GOLFDOC()->MakeHoleData();

	if (!WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
		SetGameType(COption::Instance()->dGetGameType());

		GOLFDOC()->m_currentHole = COption::Instance()->dGetStartHole();
	} else {
		// Residue (99.7%): the original keeps the doc in eax and the hole in
		// cl here, ours the reverse. Not moved by if/else (the original's
		// merged tails), a temporary, operand order or the access forms.
		GOLFDOC()->m_currentHole = CSharedDoc::Instance()->m_value4D7C > 0
			? CSharedDoc::Instance()->m_value4D7C
			: CSharedDoc::Instance()->m_holeOrder[0];
	}

	GOLFDOC()->m_gameState = 15;

	g_view->SetFOV(0.95993108f);

	if (GetCurMap() == 13)
		g_view->SetClip(0.1f, 26214.4f, true);
	else
		g_view->SetClip(0.1f, 1920.0f, true);

	g_camera.Reset(); GOLFDOC()->m_cameraData.m_matrix.Reset();

	m_replayFlags = 0;
	m_flag3C9 = false;
	m_continueDlgOpen = false;
	m_netMsgWaitSend = false;

	m_gaugePaused = false;
	GOLFDOC()->m_clearedFlag15A = false;

	if (IsLocalContent((localContentType_t)0x58))
		CTHunter::Instance()->SetTreasurePoint(0);

	m_netMsgWait = NULL;
	m_netMsgWaitRecv = NULL;
	m_netMsgSendTime = 0;
	m_netMsgRecvTime = 0;
	m_netMsgWaitType = -1;
	m_showGameUI = true;
	m_bgmIndex = 0;
	m_pendingPowerFlags = 0;
	m_observerSwitchTime = 0;
	m_spot = 0;
	m_msgValue430 = -1.0f;
	m_msgValue434 = 100.0f;

	m_value550 = 0;
	m_captureGrid = 1;
	m_updateClipAlways = 0;

	m_shotKeyAlways = false;
	m_shotReady = false;
	m_nextTurnPlayer = 0;
	m_pauseDlg = NULL;
	m_goStopDlg = NULL;
	m_dropDlg = NULL;
	m_shotKeyLocked = false;
	m_shotKeyLockTime = 0;

	m_typingIconShown = false;
	m_shotDataCorrupt = false;

	CMouseCursor::Instance()->SetAutoHide(true);

	new CCameraWork;
	new CRival;
	new CRandomHouse;

	char path[0x200];
	strcpy(path, g_executeDirectory);
	sprintf(path, "%s/data/Sound/bgm", path);

	CheckUserBGM(path);

	CSharedDoc::Instance()->m_pangRate = 1.0f;
	if (!GalleryMode()) {
		std::list<sItemInfo>::iterator it = std::find(CSharedDoc::Instance()->m_itemInfoList.begin(), CSharedDoc::Instance()->m_itemInfoList.end(), 0x1A000001);
		if (it == CSharedDoc::Instance()->m_itemInfoList.end())
			it = std::find(CSharedDoc::Instance()->m_itemInfoList.begin(), CSharedDoc::Instance()->m_itemInfoList.end(), 0x1A000002);

		if (it != CSharedDoc::Instance()->m_itemInfoList.end()) {
			if (CLoginInfo::Instance()->IsPcBang())
				CSharedDoc::Instance()->m_pangRate *= 2.5f;
			else
				CSharedDoc::Instance()->m_pangRate *= 2.0f;

		} else {
			if (CLoginInfo::Instance()->IsPcBang())
				CSharedDoc::Instance()->m_pangRate *= 1.2f;
		}

		if (IsLocalContent((localContentType_t)0x51)) {
			CCardManager::Instance()->SetPlayerIndex(0xFF);
			CSharedDoc::Instance()->m_pangRate *= CCardManager::Instance()->GetCardPeriodPangRate() * 0.01f + 1.0f;
		}

		it = std::find(CSharedDoc::Instance()->m_itemInfoList.begin(), CSharedDoc::Instance()->m_itemInfoList.end(), 0x1A000005);
		if (it != CSharedDoc::Instance()->m_itemInfoList.end())
			CSharedDoc::Instance()->m_pangRate *= 4.0f;

		it = std::find(CSharedDoc::Instance()->m_itemInfoList.begin(), CSharedDoc::Instance()->m_itemInfoList.end(), 0x1A000009);
		if (it != CSharedDoc::Instance()->m_itemInfoList.end())
			CSharedDoc::Instance()->m_pangRate *= 1.2f;

		it = std::find(CSharedDoc::Instance()->m_itemInfoList.begin(), CSharedDoc::Instance()->m_itemInfoList.end(), 0x1A00000C);
		if (it != CSharedDoc::Instance()->m_itemInfoList.end())
			CSharedDoc::Instance()->m_pangRate *= 1.2f;

		if (IsLocalContent((localContentType_t)3)) {
			sMascotInfo *pMascot = CSharedDoc::Instance()->GetMyEquippedMascotInfo();
			if (pMascot) {
				IFF_STRUCT::sMascot *pRecord = ItemManager()->FindMascot(pMascot->typeId);

				if (pRecord) {
					if (pRecord->m_pangRate > 100)
						CSharedDoc::Instance()->m_pangRate = CSharedDoc::Instance()->m_pangRate * (pRecord->m_pangRate * 0.01f);
				}
			}
		}

		if (IsLocalContent((localContentType_t)0x39))
			CSharedDoc::Instance()->m_pangRate *= CSharedDoc::Instance()->GetMapEventPangRate(GetCurMap()) * 0.01f;
	}

	CSharedDoc::Instance()->m_timeBooster = false;
	if (!GalleryMode()) {
		std::list<sItemInfo>::iterator it = std::find(CSharedDoc::Instance()->m_itemInfoList.begin(), CSharedDoc::Instance()->m_itemInfoList.end(), 0x1A000011);
		if (it != CSharedDoc::Instance()->m_itemInfoList.end())
			CSharedDoc::Instance()->m_timeBooster = true;
	}

	for (unsigned char i = 0; i < 8; i++)
		g_audio->LoadSfx(g_plSound[i], NULL, 0, NULL, NULL, 0.5f, 200.0f);

	m_shotBoardNextHole = false; m_effectTime = 0;
	m_effectCount = 0;
	m_effectParam = 0;

	for (unsigned char hole = 0; hole < CSharedDoc::Instance()->m_holes; hole++) {
		unsigned long pang = GetHolePang(hole + 1);
		GOLFDOC()->GetHoleData(CSharedDoc::Instance()->m_holeOrder[hole]).m_basePang = pang;
		GOLFDOC()->GetHoleData(CSharedDoc::Instance()->m_holeOrder[hole]).m_pang = pang;
	}
}

// golfrule.cpp:2074
void CGolfRule::OnLoad(void) {
	CTaskMain::OnLoad();
	OnReload();
	CTaskMain::OnInit();
	g_pFresh->OpenLayout("MAIN", this, true);
}

// golfrule.cpp:2109
void CGolfRule::OnInit(void) {
	if (GetCurMapProp()) {
		GetPVS().SetSlopeFactor(GetCurMapProp()->m_rate);
	}
	CSceneManager::Instance()->UpdateWind();

	m_flag42C = false;
	CalcUIOffset(false);

	SendMsg(this, "Score", 0x12F, 0, 0, 0, 0);

	m_replayBox = NULL;
	m_recordAviDlg = NULL;
	m_flag5B8 = false;
	m_waitingForStart = false;
	IActor *pGateway = GetActor(s_gatewayActorName);
	pGateway << MsgObject(0, 0x26F, 0xC, 0, 0, 0, 0);
}

// golfrule.cpp:2148
void CGolfRule::OnReload(void) {
	SetCurrentGolfRule();

	if (!m_pRule) {
		ReturnLobby(false);
		return;
	}

	CSharedDoc::Instance()->ClearGameVars();
	GOLFDOC()->ClearShotResultMsg();

	if (CSharedDoc::Instance()->m_playingMode == 1) {
		if (IsLocalContent((localContentType_t)0x4F))
			CSharedDoc::Instance()->m_shotDataList.clear();

		for (unsigned char i = 0; i < CSharedDoc::Instance()->m_holes; i++) {
			GOLFDOC()->GetHoleData(CSharedDoc::Instance()->m_holeOrder[i]).m_kind = 0;
			GOLFDOC()->GetHoleData(CSharedDoc::Instance()->m_holeOrder[i]).m_mapState = 3;

			if (CSharedDoc::Instance()->m_roomInfo.m_roomMode == 14)
				SetCurMap(CSharedDoc::Instance()->m_holeCourse[i]);

			if (!GOLFDOC()->m_pPolySoup->LoadMapCheckData(CSharedDoc::Instance()->m_holeOrder[i]) && WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
				g_bQuit = true;
				return;
			}

			GetCurMap();
		}
	} else if (IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == 2) {
		CSharedDoc::Instance()->m_shotTimeLimit = 0;
		COption::Instance()->dSetNumPlayers(1);
		GOLFDOC()->MakePlayer(1);

		if (ShotDataLoad(CSharedDoc::Instance()->m_fileName) == true) {
			if (CSharedDoc::Instance()->m_replayPlayType == 1) {
				PLAYER(0)->m_extraPang = 0;
				PLAYER(0)->m_ghostAward = 0;
			} else {
				CSharedDoc::Instance()->m_replayStartB = 0;
			}

			GOLFDOC()->m_pPolySoup->LoadMapCheckData(m_hole);
		}
	}

	SetPlayer();
	SetGrade();

	if (IsObserver(0xFFFFFFFF)) {
		RecordPlayerList();

		for (std::vector<sGameResultInfo>::iterator it = CSharedDoc::Instance()->m_gameResultList.begin(); it != CSharedDoc::Instance()->m_gameResultList.end(); ++it) {
			it->m_joined = 0;
		}
	}

	ToggleGameUI();

	CRandomHouse::Instance()->SetRandomSeed(g_CurrentTime);
	CCameraWork::Instance()->Load();

	CChatMsg::Instance()->ClearHistory();
	CMouseCursor::Instance()->Reset(true);
	CMouseCursor::Instance()->SetMode(1);
	COneLineBoard::Instance()->Clear();
	CMessengerInfo::Instance()->SetGameFocus(true);
}

// golfrule.cpp:2265
void CGolfRule::SetPlayer(void) {
	if (HackingManager::Instance()) {
		HackingManager::Instance()->ClearCrypticList();
	}
	if (m_pRule) {
		m_pRule->SetPlayer();
	}
}

// golfrule.cpp:2285
void CGolfRule::SetGrade(void) {
	if (m_pRule != NULL)
		m_pRule->SetGrade();
}

// golfrule.cpp:2295
void CGolfRule::SetReady(void) {
	if (CSharedDoc::Instance()->m_playingMode == 2) {
		ChangeGameMode(0x10000);
	} else {
		if (m_pRule) {
			m_pRule->SetReady();
		}
	}
}

// golfrule.cpp:2310
void CGolfRule::RecordPlayerList(void) {
	mkdir("history");
	mkdir("history/ongamenet");

	SYSTEMTIME st;
	GetLocalTime(&st);

	char sz[256] = { 0 };
	FILE *fp = fopen(MakeStr("history/ongamenet/player(%02d\xBD\xC3%02d\xBA\xD0).txt", st.wHour, st.wMinute), "wt");

	if (fp) {
		sprintf(sz, "\xBE\xC6\xC0\xCC\xB5\xF0 : \xB4\xEB\xC8\xAD\xB8\xED\n");
		fwrite(sz, strlen(sz), 1, fp);

		sprintf(sz, "------------------------------------------------------------\n");
		fwrite(sz, strlen(sz), 1, fp);

		switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
		case 4:
		case 5:
		case 6:
		case 9:
		case 10:
			for (std::vector<sGameResultInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
				sprintf(sz, "%s\n", it->m_nickname);
				fwrite(sz, strlen(sz), 1, fp);
			}
			break;
		default:
			for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
				sprintf(sz, "%s\n", WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_name);
				fwrite(sz, strlen(sz), 1, fp);
			}
			break;
		}

		fclose(fp);
	}
}

// golfrule.cpp:2350
void CGolfRule::SetVisibleUpperBtns(bool visible) {
	if (m_chatViewWnd)
		m_chatViewWnd->SetVisible(visible);

	if (m_boardWnd)
		m_boardWnd->SetVisible(visible);

	if (m_myInfoBtn)
		m_myInfoBtn->SetVisible(visible);

	if (m_optionBtn)
		m_optionBtn->SetVisible(visible);

	if (m_messengerWnd)
		m_messengerWnd->SetVisible(visible);
}

// golfrule.cpp:2369
void CGolfRule::AddPlayer(unsigned char index, const char *name, unsigned long oid, unsigned long uid, unsigned long caddieTypeId) {
	if (m_pRule) {
		int connected = WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT);

		if (connected) {
			m_pRule->AddPlayer(index, name, oid, uid, caddieTypeId);
		} else {
			m_pRule->AddPlayerOffline(index, name, uid, caddieTypeId);
		}
	}

	if (OnlinePlay() && HackingManager::Instance()) {
		WCrypticValue<unsigned char> *pByteValue;
		WCrypticValue<int> *pIntValue;
		WCrypticValue<float> *pFloatValue;

		GolfBall().AddCrypticValue();
		GolfClub().AddCrypticValue();

		for (unsigned char i = 0; i < 18; i++) {
			pByteValue = &GOLFDOC()->GetPlayer(index)->m_holes[i];
			HackingManager::Instance()->AddCrypticByteValue(pByteValue);
			pByteValue = &GOLFDOC()->GetPlayer(index)->m_holeIn[i];
			HackingManager::Instance()->AddCrypticByteValue(pByteValue);
			pIntValue = &GOLFDOC()->GetPlayer(index)->m_holeRecord[i];
			HackingManager::Instance()->AddCrypticIntValue(pIntValue);
		}

		pIntValue = &GOLFDOC()->GetPlayer(index)->m_ghostAward;
		HackingManager::Instance()->AddCrypticIntValue(pIntValue);
		pIntValue = &GOLFDOC()->GetPlayer(index)->m_extraPang;
		HackingManager::Instance()->AddCrypticIntValue(pIntValue);
		pFloatValue = &GOLFDOC()->GetPlayer(index)->m_comboGauge;
		HackingManager::Instance()->AddCrypticFloatValue(pFloatValue);
		pIntValue = &GOLFDOC()->GetPlayer(index)->m_power;
		HackingManager::Instance()->AddCrypticIntValue(pIntValue);
		pIntValue = &GOLFDOC()->GetPlayer(index)->m_control;
		HackingManager::Instance()->AddCrypticIntValue(pIntValue);
		pIntValue = &GOLFDOC()->GetPlayer(index)->m_accuracy;
		HackingManager::Instance()->AddCrypticIntValue(pIntValue);
		pIntValue = &GOLFDOC()->GetPlayer(index)->m_spin;
		HackingManager::Instance()->AddCrypticIntValue(pIntValue);
		pIntValue = &GOLFDOC()->GetPlayer(index)->m_curve;
		HackingManager::Instance()->AddCrypticIntValue(pIntValue);
	}
}

// golfrule.cpp:2410
void CGolfRule::OnDestroy(void) {
	CTaskMain::OnDestroy();

	if (CCameraWork::IsInstantiated())
		delete CCameraWork::Instance();
	if (CRival::IsInstantiated())
		delete CRival::Instance();
	if (CRandomHouse::IsInstantiated())
		delete CRandomHouse::Instance();
	delete CCapturedBg::Instance();

	while (!m_backUps.empty())
		m_backUps.pop_back();
}

// golfrule.cpp:2423
void CGolfRule::OnDisplay(void) {
	if ((GOLFDOC()->GameMode() & 0x20) && CCapturedBg::Instance())
		CCapturedBg::Instance()->Render();

	if (!(GOLFDOC()->GameMode() & 0xFFDDDDDF) && !m_updateClipAlways)
		return;

	int index;
	float height = g_camera.pivot.y - GetPVS().GetGroundPoint(g_camera.pivot, true, &index, false).y;

	if (index == -1)
		height = g_camera.pivot.y - GetPVS().m_lowest;

	float nearDist;
	if (GOLFDOC()->GameMode() != 0x80 && !g_view->InFrustum(WSphere(GolfBall().m_pos, GolfBall().GetRadius()))) {
		nearDist = height;
	} else {
		float distance = (g_camera.pivot - GolfBall().m_pos).Magnitude();
		nearDist = height > distance ? distance : height;
	}

	Waabb *box = &GOLFDOC()->m_pPolySoup->FindModel(NULL)->m_aabb;

	if (GOLFDOC()->GameMode() & 0x200) {
		g_view->SetClip(3.2f, 3840.0f, true);
	} else {
		float range;
		if (GetCurMap() == 13) {
			range = 8192.0f;
		} else {
			range = 600.0f;
		}

		float size = box->max.x - box->min.x;
		size = size < box->max.y - box->min.y ? box->max.y - box->min.y : size;
		size = size < box->max.z - box->min.z ? box->max.z - box->min.z : size;
		float clipFar = MaxOf(range * 3.2f, size + 160.0f);
		float clipNear = nearDist < 0.1f ? 0.1f : (nearDist > 1.0f ? 1.0f : nearDist);
		g_view->SetClip(clipNear, clipFar, true);
	}

	g_view->UpdateCamera();
}

// golfrule.cpp:2493
// Every frame of the round: the debug and UI keys, the chat input, the
// network wait watchdog, the course's quadtree and the club, the time-attack
// end, the replay keys, then the handler of the current game mode.
void CGolfRule::OnProcess(float elapsed) {
	UpdateRadioPredictShotButtonStatus();

	if (g_input->GetDown("FUNC_8", true)) {
		mkdir("report");
		std::list<sShotDataStorage> shotDataList(CSharedDoc::Instance()->m_shotDataList);
		int replayPlayType = CSharedDoc::Instance()->m_replayPlayType;
		CSharedDoc::Instance()->m_replayPlayType = 2;

		SYSTEMTIME time;
		GetLocalTime(&time);
		const char *fileName = MakeStr("report/pangya_report_repaly.%04d%02d%02d-%02d%02d%02d.sav", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
		ShotDataSave(fileName, false);
		CSharedDoc::Instance()->m_shotDataList = shotDataList;
		CSharedDoc::Instance()->m_replayPlayType = replayPlayType;
	}

	if (IsLocalContent((localContentType_t)0x4F)) {
		if (CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK) {
			ReplayDlgRun();

			// The arrow keys, the WASD keys and ENTER do nothing in a replay.
			g_input->BlockKey("\xC1\xC2");
			g_input->BlockKey("\xBF\xEC");
			g_input->BlockKey("\xBB\xF3");
			g_input->BlockKey("\xC7\xCF");

			g_input->BlockKey("W");
			g_input->BlockKey("A");
			g_input->BlockKey("S");
			g_input->BlockKey("D");

			g_input->BlockKey("ENTER");
		} else {
			if (m_replayBoxWnd)
				m_replayBoxWnd->SetVisible(false);
		}
	}

	if (m_boardWnd)
		m_boardWnd->SetAlpha(COneLineBoard::Instance()->GetFadeFactor());

	if (!(GOLFDOC()->m_gameMode & 0x30) && CCapturedBg::Instance()->IsCaptured())
		CCapturedBg::Instance()->Reset();

	if (m_shotKeyLocked) {
		m_shotKeyLockTime += elapsed;
		if (m_shotKeyLockTime > 5.0f)
			m_shotKeyLocked = false;
	}

	ProcessObserver();

	if (GalleryMode() && (GOLFDOC()->m_clockHold == 1 || GOLFDOC()->m_clockHold == 2))
		DoPowerGauge(elapsed);

	if (GOLFDOC()->m_gameMode != 0x200 && g_input->GetDown("FUNC_12", true)) {
		if (m_kbdManualDlg)
			m_kbdManualDlg->Close(true);
		else {
			m_kbdManualDlg = CreateForm<FrKbdManualDlg>(g_pFresh->GetManager(), this, "kbd_manual", 0);
			if (m_kbdManualDlg) {
				m_kbdManualDlg->Open((FRESH_RESULT_CALLBACK)&CGolfRule::OnKbdManualDlgResult, 1);
				m_kbdManualDlg->EnableDrag(false);
			}
		}
	}

	if (!CQuitWindow::Instance()->IsActive()) {
		if (((g_input->Get("LCONTROL", true) || g_input->Get("RCONTROL", true)) && g_input->GetDown("HIDE", true))
			|| (!m_showGameUI && (g_input->GetDown("ENTER", true) || g_input->GetDown("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", true)))) {
			m_showGameUI = !m_showGameUI;
			ToggleGameUI();
		}
	} else if (!m_showGameUI) {
		m_showGameUI = true;
		ToggleGameUI();
	}

	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) && g_input->GetDown("EMOTICON", false)) {
		if (RANKINGDLG() == NULL && !CMessengerInfo::Instance()->IsVisible())
			OnEmoticonBtnUp();
	}

	if (m_languageArea) {
		// The language indicator follows the IME's mode.
		static bool isAlphaNumeric = true;
		if (IsLocalCountry(1) && g_ime->IsAlphaNumericMode() != isAlphaNumeric) {
			isAlphaNumeric = g_ime->IsAlphaNumericMode();
			if (isAlphaNumeric)
				m_languageArea->SetBgImg("chat_eng");
			else
				m_languageArea->SetBgImg("chat_kor");
		}
	}

	if (m_chatInput && m_chatEdit) {
		if (!CChatMsg::Instance()->IsClosed()) {
			// PAGE_UP/PAGE_DOWN walk the chat history in the input box, or
			// the chat targets in the target box.
			if (g_input->GetDown("PAGE_UP", true)) {
				if (m_chatInput->HasKeyFocus() && CSharedDoc::Instance()->m_chatHistory.size() > 0) {
					const char *line = m_chatInput->GetLine(1, false);
					std::list<std::string>::iterator it = std::find(CSharedDoc::Instance()->m_chatHistory.begin(), CSharedDoc::Instance()->m_chatHistory.end(), line);
					if (it != CSharedDoc::Instance()->m_chatHistory.end()) {
						if (it != CSharedDoc::Instance()->m_chatHistory.begin()) {
							--it;
							CChatMsg::Instance()->SetChatText(it->c_str(), false);
						}
					} else {
						CChatMsg::Instance()->SetChatText(CSharedDoc::Instance()->m_chatHistory.back().c_str(), false);
					}
				} else if (m_chatEdit->HasKeyFocus() && CSharedDoc::Instance()->m_list4CD8.size() > 0) {
					CChatMsg::Instance()->SetChatText(CSharedDoc::Instance()->GetPrevChatTarget(m_chatEdit->GetLine(1, false)), false);
				}
			} else if (g_input->GetDown("PAGE_DOWN", true)) {
				if (m_chatInput->HasKeyFocus() && CSharedDoc::Instance()->m_chatHistory.size() > 0) {
					const char *line = m_chatInput->GetLine(1, false);
					std::list<std::string>::iterator it = std::find(CSharedDoc::Instance()->m_chatHistory.begin(), CSharedDoc::Instance()->m_chatHistory.end(), line);
					if (it != CSharedDoc::Instance()->m_chatHistory.end()) {
						if (++it != CSharedDoc::Instance()->m_chatHistory.end())
							CChatMsg::Instance()->SetChatText(it->c_str(), false);
						else
							CChatMsg::Instance()->SetChatText("", false);
					}
				} else if (m_chatEdit->HasKeyFocus() && CSharedDoc::Instance()->m_list4CD8.size() > 0) {
					CChatMsg::Instance()->SetChatText(CSharedDoc::Instance()->GetNextChatTarget(m_chatEdit->GetLine(1, false)), false);
				}
			}
		}

		if (m_chatInput->IsVisible() && g_input->GetDown("ENTER", true) && !CMessengerInfo::Instance()->IsVisible())
			m_chatInput->SetKeyFocus(true);

		if (OnlinePlay()) {
			// While the player types, the others see the typing head icon;
			// the input's colour follows the chat target.
			const char *line = m_chatInput->GetLine(1, false);
			if (line && strlen(line) != 0) {
				if (!m_typingIconShown) {
					m_typingIconShown = true;
					CHeadIcon::Instance()->SendIcon(1);
				}

				// "to all", "to guild", else a whisper.
				if (strlen(m_chatEdit->GetLine(1, false)) == 0 || strcmp(m_chatEdit->GetLine(1, false), "\xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4") == 0)
					m_chatInput->SetFontColor(0xFFFFFFFF);
				else if (strlen(m_chatEdit->GetLine(1, false)) == 0 || strcmp(m_chatEdit->GetLine(1, false), "\xB1\xE6\xB5\xE5\xBF\xA1\xB0\xD4") == 0)
					m_chatInput->SetFontColor(0xFF5BFFB0);
				else
					m_chatInput->SetFontColor(0xFFCF67FF);
			} else if (m_typingIconShown == true) {
				m_typingIconShown = false;
				CHeadIcon::Instance()->SendIcon(0xFFFF);
			}
		}
	}

	if (m_netMsgWaitSend) {
		// No answer to the awaited packet: report it and go back to the lobby.
		m_netMsgWaitTimeout -= elapsed;
		if (m_netMsgWaitTimeout < 0.0f) {
			unsigned long waited = GetTickCount();
			waited -= m_netMsgSendTime;
			if (waited > (m_netMsgWait && strcmpi(m_netMsgWait, "DoScoreBoard") == 0 ? 40000 : 20000)) {
				CProjectG::Instance()->ReportError(1, "%s_%d_%d_%d_%d(%d_%d)", m_netMsgWait ? m_netMsgWait : "UnknownLock", g_iSendPacketGR, g_iRecvPacketGR, g_iSendFailType, WSAGetLastError(), CSharedDoc::Instance()->m_roomInfo.m_roomNumber, CSharedDoc::Instance()->m_userInfo.guid);

				g_iSendPacketGR = 0;
				g_iRecvPacketGR = 0;
				g_iRecvDisconCode = 0;

				SendReportError(1);
				SetNetMsgWait(false, 0, "", 0.0f);

				FrForm *form = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				// "The connection to the server was lost."
				form->SetMessage("\xBC\xAD\xB9\xF6\xBF\xCD\xC0\xC7 \xBF\xAC\xB0\xE1\xC0\xCC \xB2\xF7\xB0\xE5\xBD\xC0\xB4\xCF\xB4\xD9.", false);
				form->Open((FRESH_RESULT_CALLBACK)&CGolfRule::ReturnToLobbyDlgResult, 0x203);
			}
		}
	}

	if (m_shotDataCorrupt) {
		m_shotDataCorrupt = false;
		SetNetMsgWait(false, 0, "", 0.0f);

		FrForm *form = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "The file is damaged."
		form->SetMessage("\xC6\xC4\xC0\xCF\xC0\xCC \xBC\xD5\xBB\xF3\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9", false);
		form->Open((FRESH_RESULT_CALLBACK)&CGolfRule::ReturnToLobbyDlgResult, 3);
	}

	if (GOLFDOC()->m_gameMode != 0x20 && m_updateClipAlways == 0)
		GetPVS().PreProcess(elapsed, false);

	if (GOLFDOC()->m_gameMode & 0x48308) {
		if (!m_shotKeyLocked)
			GolfClub().Process(elapsed);
	}

	unsigned char roomType = CSharedDoc::Instance()->m_roomType;
	if (roomType == 10) {
		// The time attack ends once its time has run out, between shots.
		bool betweenShots = false;
		bool started = false;
		bool timeUp = false;
		if (!(GOLFDOC()->m_gameMode & 0x800CC0))
			betweenShots = true;

		if (CSharedDoc::Instance()->m_gameStartTime > 0)
			started = true;

		if (g_CurrentTime > GetGameTimeLimit() + CSharedDoc::Instance()->m_gameStartTime)
			timeUp = true;

		if (betweenShots && started && timeUp) {
			GolfBall().m_preDataIndex = 0;

			SendShotResult();

			if (CCapturedBg::Instance()->IsCaptured())
				CCapturedBg::Instance()->Reset();

			SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Spin", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Player", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Caddie", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Mascot", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Bubble", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x1A4, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x1A5, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x1A8, 0, 0, 0, 0);

			ChangeGameMode(0x800);
		}
	}

	if ((GOLFDOC()->m_gameMode & 8) && GolfClub().PowerShotType() == 0 && (g_input->Get("LALT", true) || g_input->Get("RALT", true)))
		return;

	// The skip key's handling is empty in the original.
	if (g_input->GetDown("SKIP", true)) {
	}

	if (g_input->GetDown("FASTFORWARD", true) && m_canFastForward && (GOLFDOC()->m_gameMode & 0xC0)) {
		if (OnlinePlay() && IsMyTurn(false)) {
			// PC bang players and time booster owners may speed the ball up.
			if (CLoginInfo::Instance()->IsPcBang() || CSharedDoc::Instance()->m_timeBooster) {
				m_canFastForward = false;
				float speed = 3.0f;
				WSendPacket packet((enumClientPacket)0x65);
				packet.EncodeBuffer(&speed, 4);
				packet.Send(SEND_TO_DEFAULT);
			}
		} else {
			m_canFastForward = false;
		}
	}

	if (CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_GAME) {
		if (OnlinePlay() || CSharedDoc::Instance()->m_gameMode == 5) {
			if (!WisEqual(CProjectG::Instance()->GetGameSpeed(), 1.0f, g_EPSILON) && (GOLFDOC()->m_gameMode & 0xFFFFF73F))
				CProjectG::Instance()->SetGameSpeed(1.0f);
		}
	}

	if (IsLocalContent((localContentType_t)0x4F)) {
		if (CSharedDoc::Instance()->m_replayPlayType == 4 && g_input->GetDown("REPLAY", true) && IsMyTurn(false)
			&& GOLFDOC()->m_gameState == 15 && CSharedDoc::Instance()->m_gameMode == 0) {
			if (GetCurMap() != 7) {
				CSharedDoc::Instance()->m_replayPlayType = 2;
				ReplayTapeCheckAndUse();
			}
		}
	}

	if (IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK) {
		if (CSharedDoc::Instance()->m_replayState == 7 && m_replayBox)
			ReplayBoxClose();
	}

	m_modeTime += elapsed;

	if (m_goStopDlg) {
		m_goStopTime += elapsed;
		char text[50];
		// "Leaving the game automatically in %d seconds. %d seconds"
		sprintf(text, "%d\xC3\xCA \xC8\xC4\xBF\xA1 \xC0\xDA\xB5\xBF\xC0\xB8\xB7\xCE \xB0\xD4\xC0\xD3\xBF\xA1\xBC\xAD \xB3\xAA\xB0\xA9\xB4\xCF\xB4\xD9. %d\xC3\xCA", 10.0, (int)m_goStopTime);
		m_goStopDlg->SetDesc(text);
	}

	switch (GOLFDOC()->m_gameMode) {
	case 2: DoPlayerPreview(elapsed); break;
	case 4: DoCloseupCaddie(elapsed); break;
	case 8: DoStand(elapsed); break;
	case 0x10: DoGetReadyToCaptureBG(elapsed); break;
	case 0x20: DoPowerGauge(elapsed); break;
	case 0x100: DoNavigateCamera(elapsed); break;
	case 0x200: DoTopViewCamera(elapsed); break;
	case 0x400: DoDebugCamera(elapsed); break;
	case 0x800: DoToDefaultCamera(elapsed); break;
	case 0x40: DoDrive(elapsed); break;
	case 0x80: DoFlyBall(elapsed); break;
	case 0x1000: DoHoleInPose(elapsed); break;
	case 0x2000: DoScoreBoard(elapsed); break;
	case 0x4000: DoTimeOut(elapsed); break;
	case 0x8000: DoFixedCamera(elapsed); break;
	case 0x20000: DoGameOver(elapsed); break;
	case 1: DoWaitForStart(elapsed); break;
	case 0x40000: DoSpotCamera(elapsed); break;
	case 0x80000: DoDespairPose(elapsed); break;
	case 0x200000: DoCoursePreview(elapsed); break;

	case 0x800000: DoApproachNextHole(elapsed); break;
	}

	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) && g_input->GetDown("CANCEL", false)) {
		if (GOLFDOC()->m_showChat) {
			// "The in-game chat window is hidden."
			CChatMsg::Instance()->AddChatMsg("\xB0\xD4\xC0\xD3\xB3\xBB \xB4\xEB\xC8\xAD\xC3\xA2\xC0\xBB \xBC\xFB\xB1\xE9\xB4\xCF\xB4\xD9.", 0xFFFF7878, true, false);
			GOLFDOC()->m_showChat = false;
		} else {
			// "The in-game chat window is activated."
			CChatMsg::Instance()->AddChatMsg("\xB0\xD4\xC0\xD3\xB3\xBB \xB4\xEB\xC8\xAD\xC3\xA2\xC0\xBB \xC8\xB0\xBC\xBA\xC8\xAD\xC7\xD5\xB4\xCF\xB4\xD9.", 0xFFFF7878, true, false);
			GOLFDOC()->m_showChat = true;
		}
	}

	if (m_effectTime != 0.0f) {
		m_effectTime -= elapsed;
		if (m_effectTime < 0.0f) {
			m_effectCount--;
			m_effectTime = 0.0f;
			if (m_effectCount > 0)
				m_effectTime = 0.1f;

			SendMsg(this, "Effect", 0x17D, m_effectParam, 0, 0, 0);
		}
	}

	CMouseCursor::Instance()->SetCursor(0);
}

// golfrule.cpp:3210
void CGolfRule::DoFixedCamera(float) {
	if (g_input->GetDown("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", true)) {
		CMouseCursor::Instance()->Store();
		ChangeGameMode(8);
	} else if (TopViewClick()) {
		ChangeGameMode(0x200);
	}
}

// golfrule.cpp:3226
void CGolfRule::DoSpotCamera(float) {
	if (g_input->GetDown("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", true)) {
		CMouseCursor::Instance()->Store();
		ChangeGameMode(8);
	} else if (TopViewClick()) {
		ChangeGameMode(0x200);
	}
}

// golfrule.cpp:3243
bool CGolfRule::CheckDropOut(void) {
	if (m_pRule) {
		int numTimeout = m_pRule->GetNumTimeout();

		if (numTimeout >= 3) {
			if (!OnlinePlay())
				SendMsg(this, "GolfRule", 0x82, PLAYER(GOLFDOC()->m_myPlayerIndex)->m_uid, 0, 0, 0);

			return true;
		}
	}

	return false;
}

// golfrule.cpp:3268
bool CGolfRule::CheckGiveUp(void) {
	if (m_pRule)
		return m_pRule->CheckGiveUp();

	return false;
}

// golfrule.cpp:3278
// The give-up pose: let the camera run, tell the server once, then go on to
// the next hole, the tutorial or the score board.
void CGolfRule::DoDespairPose(float elapsed) {
	if (m_modeTime < m_gameOverDelay) {
		CCameraWork::Instance()->Process(elapsed, 0.0f);
		return;
	}

	if (OnlinePlay() && !m_goStopSent) {
		m_goStopSent = true;

		if (!IsMassGame())
			SetNetMsgWait(true, 0xCDA, "DoDespairPose", 20.0f);

		if (CSharedDoc::Instance()->m_goStopMode == 1 && GOLFDOC()->m_holeData[GOLFDOC()->m_currentHole - 1].m_kind <= 2) {
			CSharedDoc::Instance()->m_goStopMode = 0;
		} else {
			WSendPacket packet((enumClientPacket)0x1C);
			EncodePacket((enumClientPacket)0x1C, packet);
			packet.Send(SEND_TO_DEFAULT);
		}

		CheckNetMsgWait_Send("DoDespairPose");
	}

	if (!m_continueDlgOpen && !m_netMsgWaitSend) {
		sPlayerData *pMe = PLAYER(GOLFDOC()->m_myPlayerIndex);
		if (pMe->m_uid == MyGuid(true)) {
			if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
				if (GOLFDOC()->m_gameState == 8) {
					IActor *pTutorial = GetActor("Tutorial");
					if (!pTutorial)
						return;

					pTutorial << MsgObject(NULL, 0x154, 0, 0, 0, 0, 0);
					return;
				} else if (GOLFDOC()->m_gameState == 7) {
					ReturnLobby(true);
					return;
				} else if (GOLFDOC()->m_gameState == 15 && CSharedDoc::Instance()->m_gameMode == 0) {
					if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_state == 3)
						return;

					if (CSharedDoc::Instance()->m_roomType == 1) {
						if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_numTimeout + PLAYER(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_partnerIndex)->m_numTimeout >= 3
							|| PLAYER(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_partnerIndex)->m_giveUpCount + PLAYER(GOLFDOC()->m_myPlayerIndex)->m_giveUpCount >= MaxOf(3, (CSharedDoc::Instance()->m_holes + 1) / 2))
							return;
					} else if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_numTimeout >= 3
						|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_giveUpCount >= MaxOf(3, (CSharedDoc::Instance()->m_holes + 1) / 2)) {
						return;
					}
				}
			} else {
				ChangeGameMode(0x2000);
				return;
			}
		}

		if (CanMoveToNextHole()) {
			if (GOLFDOC()->m_gameState == 8) {
				IActor *pTutorial = GetActor("Tutorial");
				if (!pTutorial)
					return;

				pTutorial << MsgObject(NULL, 0x14B, 0, 0, 0, 0, 0);
			} else if (GOLFDOC()->m_gameState == 7) {
				ReturnLobby(false);
			} else if (!OnlinePlay() || IsMassGame()) {
				ChangeGameMode(0x2000);
			}
		} else {
			SendMsg(this, "Score", 0x12F, 0, 0, 0, 0);
			if (g_pFresh)
				g_pFresh->GetManager()->CloseForm(false);
			ChangeGameMode(0x800);
		}
	}
}

// golfrule.cpp:3440
// A turn ran out of time: tell the server once, then charge the stroke and
// either replay the shot setup (our turn) or hand the turn to the next player.
void CGolfRule::DoTimeOut(float) {
	if (m_modeTime > 3.0f) {
		if (OnlinePlay() && !m_goStopSent) {
			m_goStopSent = true;

			if (!IsMassGame())
				SetNetMsgWait(true, 0xD78, "DoTimeOut", 20.0f);

			WSendPacket packet((enumClientPacket)0x1C);
			EncodePacket((enumClientPacket)0x1C, packet);
			packet.Send(SEND_TO_DEFAULT);

			CheckNetMsgWait_Send("DoTimeOut");
		}

		if (!m_continueDlgOpen && !m_netMsgWaitSend && m_goStopDlg == 0) {
			m_goStopSent = false;

			if (m_nextTurnPlayer == GOLFDOC()->m_myPlayerIndex) {
				PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;
				if (CSharedDoc::Instance()->m_roomType == 1)
					TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;

				SendMsg(this, "Spot", 0x163, 0, 0, 0, 0);
				SendMsg(this, "Grid", 0x163, 0, 0, 0, 0);
				SendMsg(this, "Clock", 0x1E9, 0, 0, 0, 0);

				SendMsg(this, "Player", 0x10F, 0, 0, 0, 0);
				SendMsg(this, "Caddie", 0x2716, (intptr_t)"\xB1\xE2\xBA\xBB", 0, 0, 0);
				SendMsg(this, "Caddie", 0x1DB, 0, 0, 0, 0);
				ChangeGameMode(8);
			} else {
				GOLFDOC()->m_myPlayerIndex = m_nextTurnPlayer;
				PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;
				if (CSharedDoc::Instance()->m_roomType == 1)
					TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;

				SetEnvironmentVariableA();
				SetTurnChangeEvent();
				ReadyForNextTurn();
			}
		}
	}
}

// golfrule.cpp:3503
void CGolfRule::DoGameOver(float elapsed) {
	if (m_modeTime < m_gameOverDelay) {
		CCameraWork::Instance()->Process(elapsed, 0.0f);
		return;
	}

	if (!(m_modeTime < 10.0f)) {
		if (CSharedDoc::Instance()->m_roomType <= 1) {
			if (OnlinePlay()) {
				if (!CTHunter::Instance()->IsReturnLobby()) {
					return;
				}
			}
		}

		if (!m_continueDlgOpen && !m_netMsgWaitSend) {
			if (OnlinePlay()) {
				SetNetMsgWait(true, 0xDD3, "DoGameOver", 20.0f);
				this << MsgObject(NULL, 0x8F, 1, 0, 0, 0, 0);
				CheckNetMsgWait_Send("DoGameOver");
			} else if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
				switch (CSharedDoc::Instance()->m_gameMode) {
				case 1:
					CSharedDoc::Instance()->m_gameOver = true;
					CProjectG::Instance()->FromGolfToLobbySettings();
					CTaskManager::Instance()->ChangeTask("CFamilyTask", "", false);
					PostMsg(this, "Family", 0, (intptr_t)"FAMILY", 0, 0, 0);
					break;
				case 2: {
					m_continueDlgOpen = true;
					FrForm *form = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);
					form->Open((FRESH_RESULT_CALLBACK)&CGolfRule::ContinueDlgResult, 0);
					// "Continue practising?"
					form->SetMessage("\xBF\xAC\xBD\xC0\xC0\xBB \xB0\xE8\xBC\xD3 \xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);
					form->EnableDrag(false);
					break;
				}
				case 5:
					CSharedDoc::Instance()->m_gameOver = true;
					CProjectG::Instance()->FromGolfToLobbySettings();
					CTaskManager::Instance()->ChangeTask("CGhostTask", "", CTaskManager::Instance()->GetPreservedTask() != 0);
					CTaskManager::Instance()->PostMsg(0, "Ghost", 0, (intptr_t)"GHOST", 0, 0, 0);
					break;
				}
			}
		}
	}
}

// golfrule.cpp:3583
bool CGolfRule::HaveTeeShots(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1)
		return TEAM(1 - m_firstTeam)->m_holes[GOLFDOC()->m_currentHole - 1] == (unsigned char)0 ? true : false;

	for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (PLAYER(i)->m_state == 0 && PLAYER(i)->m_holes[GOLFDOC()->m_currentHole - 1] == (unsigned char)0)
			return true;
	}

	return false;
}

// golfrule.cpp:3604
void CGolfRule::RecordPlayerData(unsigned char player) {
	if (GOLFDOC()->GetPlayer(player)->m_state == 3)
		return;

	GOLFDOC()->GetPlayer(player)->m_shotItemTypeId = 0;

	sPlayerData* pPlayer = GOLFDOC()->GetPlayer(player);
	if (pPlayer->m_recordedHoles == GetHoleIndex(GOLFDOC()->m_currentHole) - 1) {
		if (!GOLFDOC()->IsNetGameEnd()) {
			GOLFDOC()->GetPlayer(player)->m_totalScore += PLAYER(player)->m_holes[GOLFDOC()->m_currentHole - 1];

			if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1)
				GOLFDOC()->GetPlayer(player)->m_ghostResult = TEAM(GOLFDOC()->GetPlayer(player)->m_attribute)->m_resetCount;
			else if (WSingleton<CSharedDoc>::Instance()->m_roomType != 3 && WSingleton<CSharedDoc>::Instance()->m_roomType != 7)
				PLAYER(player)->m_ghostResult += (int)PLAYER(player)->m_holes[GOLFDOC()->m_currentHole - 1] - GOLFDOC()->GetHoleData().m_par;
		}

		GOLFDOC()->GetPlayer(player)->m_recordedHoles++;
	}

	GOLFDOC()->GetPlayer(player)->m_state = 1;
}

// golfrule.cpp:3641
void CGolfRule::RecordTeamData(unsigned char team) {
	TEAM(team)->m_totalScore += TEAM(team)->m_holes[GOLFDOC()->m_currentHole - 1];
	TEAM(team)->m_state = 1;
}

// golfrule.cpp:3649
// Ranks the players after a hole. Each sPlayerData keeps two ranks in its
// nibbles at 0x4E0: m_joinState counts the players strictly ahead (ties share
// it), m_turnOrder also counts the tied players with a lower index, so it is
// unique. The order: the round's pang (room type 7), m_ghostResult then the
// pang (stroke, 0: higher is behind; match, 3: lower is behind). In a match,
// guild or team game the hole's winner goes in sHoleData::m_mapState (0 the
// first side, 1 the second, 2 halved) and the winner's hole count grows.
// Offline, the hole's ranks (stroke) or the team that tees off first (team)
// follow.
void CGolfRule::ResetRank(void) {
	unsigned char i;
	unsigned char j;

	switch (CSharedDoc::Instance()->m_roomType) {
	case 7:
		for (i = 0; i < GOLFDOC()->m_playerNum; i++) {
			if (PLAYER(i)->m_state == 3)
				continue;

			PLAYER(i)->m_joinState = 1;
			PLAYER(i)->m_turnOrder = 1;

			for (j = 0; j < GOLFDOC()->m_playerNum; j++) {
				if (i == j || PLAYER(j)->m_state == 3)
					continue;

				if ((int)PLAYER(j)->m_roundPang > (int)PLAYER(i)->m_roundPang) {
					PLAYER(i)->m_joinState++;
					PLAYER(i)->m_turnOrder++;
				} else if ((int)PLAYER(j)->m_roundPang == (int)PLAYER(i)->m_roundPang
					&& PLAYER(i)->m_index > PLAYER(j)->m_index) {
					PLAYER(i)->m_turnOrder++;
				}
			}
		}
		break;

	case 6:
		if (GOLFDOC()->m_currentHole >= 1 && GOLFDOC()->m_currentHole <= 18) {
			sRivalData *first = GUILDMATCHUP(0);
			sRivalData *second = GUILDMATCHUP(1);

			for (int k = 0; k < GOLFDOC()->m_currentHole; k++) {
				if (first->m_holeShots[k] == 0 || second->m_holeShots[k] == 0)
					break;

				if (first->m_holeShots[k] < second->m_holeShots[k])
					GOLFDOC()->GetHoleData(CSharedDoc::Instance()->m_holeOrder[k]).m_mapState = 0;
				else if (first->m_holeShots[k] > second->m_holeShots[k])
					GOLFDOC()->GetHoleData(CSharedDoc::Instance()->m_holeOrder[k]).m_mapState = 1;
				else
					GOLFDOC()->GetHoleData(CSharedDoc::Instance()->m_holeOrder[k]).m_mapState = 2;
			}
		}
		break;

	case 3:
		if (GOLFDOC()->m_currentHole < 1 || GOLFDOC()->m_currentHole > 18)
			break;

		if (PLAYER(0)->m_holes[GOLFDOC()->m_currentHole - 1] < PLAYER(1)->m_holes[GOLFDOC()->m_currentHole - 1]) {
			GOLFDOC()->GetHoleData().m_mapState = 0;

			if (!GOLFDOC()->IsNetGameEnd())
				PLAYER(0)->m_ghostResult += 1;
		} else if (PLAYER(0)->m_holes[GOLFDOC()->m_currentHole - 1] > PLAYER(1)->m_holes[GOLFDOC()->m_currentHole - 1]) {
			GOLFDOC()->GetHoleData().m_mapState = 1;

			if (!GOLFDOC()->IsNetGameEnd())
				PLAYER(1)->m_ghostResult += 1;
		} else {
			GOLFDOC()->GetHoleData().m_mapState = 2;
		}

		for (i = 0; i < GOLFDOC()->m_playerNum; i++) {
			if (PLAYER(i)->m_state == 3)
				continue;

			PLAYER(i)->m_turnOrder = 1;
			PLAYER(i)->m_joinState = 1;

			for (j = 0; j < GOLFDOC()->m_playerNum; j++) {
				if (i == j || PLAYER(j)->m_state == 3)
					continue;

				if ((int)PLAYER(i)->m_ghostResult < (int)PLAYER(j)->m_ghostResult
					|| (PLAYER(i)->m_ghostResult == PLAYER(j)->m_ghostResult && PLAYER(i)->m_ghostAward < PLAYER(j)->m_ghostAward)) {
					PLAYER(i)->m_turnOrder++;
					PLAYER(i)->m_joinState++;
				}

				if (PLAYER(i)->m_ghostResult == PLAYER(j)->m_ghostResult
					&& PLAYER(i)->m_ghostAward == PLAYER(j)->m_ghostAward && PLAYER(i)->m_index > PLAYER(j)->m_index)
					PLAYER(i)->m_turnOrder++;
			}
		}
		break;

	case 0:
		for (i = 0; i < GOLFDOC()->m_playerNum; i++) {
			if (PLAYER(i)->m_state == 3)
				continue;

			PLAYER(i)->m_turnOrder = 1;
			PLAYER(i)->m_joinState = 1;

			for (j = 0; j < GOLFDOC()->m_playerNum; j++) {
				if (i == j || PLAYER(j)->m_state == 3)
					continue;

				if ((int)PLAYER(i)->m_ghostResult > (int)PLAYER(j)->m_ghostResult
					|| (PLAYER(i)->m_ghostResult == PLAYER(j)->m_ghostResult
						&& PLAYER(i)->m_ghostAward < PLAYER(j)->m_ghostAward)) {
					PLAYER(i)->m_turnOrder++;
					PLAYER(i)->m_joinState++;
				}

				if (PLAYER(i)->m_ghostResult == PLAYER(j)->m_ghostResult
					&& PLAYER(i)->m_ghostAward == PLAYER(j)->m_ghostAward
					&& PLAYER(i)->m_index > PLAYER(j)->m_index)
					PLAYER(i)->m_turnOrder++;
			}
		}
		break;

	case 1:
		if (GOLFDOC()->m_currentHole < 1 || GOLFDOC()->m_currentHole > 18)
			break;

		if (TEAM(0)->m_state == 3 || TEAM(1)->m_state == 3) {
			unsigned char winner = TEAM(0)->m_state == 3;

			GOLFDOC()->GetHoleData().m_mapState = winner;

			if (!GOLFDOC()->IsNetGameEnd())
				PLAYER(winner)->m_ghostResult += 1;
		} else if (TEAM(0)->m_holes[GOLFDOC()->m_currentHole - 1] < TEAM(1)->m_holes[GOLFDOC()->m_currentHole - 1]) {
			GOLFDOC()->GetHoleData().m_mapState = 0;

			if (!GOLFDOC()->IsNetGameEnd())
				TEAM(0)->m_resetCount += 1;
		} else if (TEAM(0)->m_holes[GOLFDOC()->m_currentHole - 1] > TEAM(1)->m_holes[GOLFDOC()->m_currentHole - 1]) {
			GOLFDOC()->GetHoleData().m_mapState = 1;

			if (!GOLFDOC()->IsNetGameEnd())
				TEAM(1)->m_resetCount += 1;
		} else {
			GOLFDOC()->GetHoleData().m_mapState = 2;
		}
		break;
	}

	SendMsg(this, "Score", 0x131, 0, 0, 0, 0);

	if (OnlinePlay())
		return;

	switch (CSharedDoc::Instance()->m_roomType) {
	case 0:
		for (i = 0; i < GOLFDOC()->m_playerNum; i++) {
			if (PLAYER(i)->m_state == 3) {
				GOLFDOC()->GetHoleData().m_playerRank[i] = GOLFDOC()->m_playerNum;
				continue;
			}

			GOLFDOC()->GetHoleData().m_playerRank[i] = 1;

			for (j = 0; j < GOLFDOC()->m_playerNum; j++) {
				if (i == j || PLAYER(j)->m_state == 3)
					continue;

				if (PLAYER(i)->m_holes[GOLFDOC()->m_currentHole - 1] > PLAYER(j)->m_holes[GOLFDOC()->m_currentHole - 1]
					|| (PLAYER(i)->m_holes[GOLFDOC()->m_currentHole - 1] == PLAYER(j)->m_holes[GOLFDOC()->m_currentHole - 1]
						&& PLAYER(i)->m_turnIndex > PLAYER(j)->m_turnIndex))
					GOLFDOC()->GetHoleData().m_playerRank[i]++;
			}
		}

		for (i = 0; i < GOLFDOC()->m_playerNum; i++)
			PLAYER(i)->m_turnIndex = GOLFDOC()->GetHoleData().m_playerRank[i] - 1;
		break;

	case 1:
		if (TEAM(m_firstTeam)->m_holes[GOLFDOC()->m_currentHole - 1] > TEAM(1 - m_firstTeam)->m_holes[GOLFDOC()->m_currentHole - 1])
			m_firstTeam = 1 - m_firstTeam;

		for (unsigned char t = 0; t < 2; t++) {
			TEAM(t)->m_firstTurnSlot = 1 - TEAM(t)->m_firstTurnSlot;
			TEAM(t)->m_nextTurnSlot = TEAM(t)->m_firstTurnSlot;
		}
		break;
	}
}

// golfrule.cpp:3902
// Whether the round may go on to the next hole: every player still playing
// has finished it, or the match/team result is decided.
bool CGolfRule::CanMoveToNextHole(void) {
	switch (CSharedDoc::Instance()->m_roomType) {
	case 0:
		for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			if (PLAYER(i)->m_state == 0 || (PLAYER(i)->m_state == 2 && PLAYER(i)->m_holes[GOLFDOC()->m_currentHole - 1] == (unsigned char)0))
				return false;
		}

		ResetRank();
		return true;

	case 3:
	case 7:
		if (PLAYER(0)->m_state == 1 && PLAYER(1)->m_state == 1)
			return true;

		if (PLAYER(0)->m_state == 1) {
			if (PLAYER(0)->m_holes[GOLFDOC()->m_currentHole - 1] <= PLAYER(1)->m_holes[GOLFDOC()->m_currentHole - 1])
				return true;
		} else if (PLAYER(1)->m_state == 1) {
			if (PLAYER(0)->m_holes[GOLFDOC()->m_currentHole - 1] >= PLAYER(1)->m_holes[GOLFDOC()->m_currentHole - 1])
				return true;
		}
		break;

	case 1: {
		// The two finished flags are an array: cl overlaps it with the
		// argument temporary of the case 4 GetPlayer call (4-byte frame).
		bool finished[2];
		finished[0] = TEAM(0)->m_state == 1;
		finished[1] = TEAM(1)->m_state == 1;

		if (finished[0] && finished[1])
			return true;

		unsigned char score0 = TEAM(0)->m_holes[GOLFDOC()->m_currentHole - 1];
		unsigned char score1 = TEAM(1)->m_holes[GOLFDOC()->m_currentHole - 1];

		if (finished[0]) {
			if (score0 <= score1)
				return true;
		} else if (finished[1]) {
			if (score0 >= score1)
				return true;
		}
		break;
	}

	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
		return PLAYER(GOLFDOC()->m_myPlayerIndex)->m_state == 1;
	}

	return false;
}

// golfrule.cpp:3991
void CGolfRule::RecordGiveUp(unsigned char index) {
	if (index >= GOLFDOC()->GetPlayerNum())
		return;

	unsigned char limit = GOLFDOC()->GetHoleData().m_par + 4;

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1)
		TEAM(GOLFDOC()->GetPlayer(index)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] = limit + 1;
	else
		PLAYER(index)->m_holes[GOLFDOC()->m_currentHole - 1] = limit + 1;

	PLAYER(index)->m_giveUpCount++;
}

// golfrule.cpp:4010
void CGolfRule::RecordScoreByPlayer(unsigned char player) {
	if (player < GOLFDOC()->GetPlayerNum()) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1)
			RecordTeamData(GOLFDOC()->GetPlayer(player)->m_attribute);

		RecordPlayerData(player);

		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1 && PLAYER(player)->m_partnerIndex != 0xFF)
			RecordPlayerData(PLAYER(player)->m_partnerIndex);

		SendHoleStat();
	}
}

// golfrule.cpp:4045
void CGolfRule::RecordScoreByTeam(unsigned char team) {
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1 && team < 2) {
		for (int i = 0; i < 4; i++) {
			if (GOLFDOC()->GetPlayer(i)->m_attribute == team) {
				RecordScoreByPlayer(i);
				return;
			}
		}
	}
}

// golfrule.cpp:4074
// Starts a hole: every playing player (and each team in a team game) puts
// his ball at pos and clears his per-hole state; the hole records the count
// of playing players and the shot back-ups are dropped.
void CGolfRule::ResetData(const WVector &pos) {
	unsigned char playing = 0;

	for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (GOLFDOC()->GetPlayer(i)->m_state == 3)
			continue;

		GOLFDOC()->GetPlayer(i)->m_ghostAwardBase.Reset();
		GOLFDOC()->GetPlayer(i)->m_rivalLevel = 0;
		GOLFDOC()->GetPlayer(i)->m_lastShotPos = pos;
		GOLFDOC()->GetPlayer(i)->m_ballPos = pos;
		GOLFDOC()->GetPlayer(i)->m_numTimeout = 0;
		GOLFDOC()->GetPlayer(i)->m_holeInKind = 0;
		GOLFDOC()->GetPlayer(i)->m_holeInCount = 0;
		GOLFDOC()->GetPlayer(i)->m_powerShotLevel = CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK ? m_powerShotLevel : 0;

		if (GOLFDOC()->m_maxPangPerLoser == 0)
			GOLFDOC()->GetPlayer(i)->m_rewardPang = 0;

		playing++;
	}

	if (CSharedDoc::Instance()->m_roomType == 1) {
		for (unsigned char team = 0; team < 2; team++) {
			GOLFDOC()->GetTeam(team)->m_ballPos = pos;
			GOLFDOC()->GetTeam(team)->m_holeInKind = 0;
			GOLFDOC()->GetTeam(team)->m_holeInCount = 0;
			GOLFDOC()->GetTeam(team)->m_powerShotLevel = 0;
		}
	}

	GOLFDOC()->GetHoleData().m_kind = playing;

	m_replaying = false;
	m_goStopSent = false;
	m_flag266 = false;
	m_flag267 = false;
	m_canFastForward = false;

	while (!m_backUps.empty())
		m_backUps.pop_back();

	SendMsg(this, "Ball", 0x1EB, 0, 0, 0, 0);
}

// golfrule.cpp:4128
unsigned long CGolfRule::GetHolePang(unsigned char hole) {
	CSharedDoc *sharedDoc = WSingleton<CSharedDoc>::Instance();
	float channelRate;

	if (sharedDoc->m_currentChannel.flags & 0x800)
		channelRate = 1.0f;
	else if (sharedDoc->m_currentChannel.flags & 0x1000)
		channelRate = 2.5f;
	else if (sharedDoc->m_currentChannel.flags & 0x2000)
		channelRate = 5.0f;
	else
		channelRate = 1.0f;

	int step = (hole - 1) / 3;

	float pang = channelRate * (step * 0.5f + 1.0f) * 20.0f;

	if (hole == sharedDoc->m_holes)
		pang += pang;

	return (unsigned long)pang;
}

// golfrule.cpp:4158
// The score board after a hole: once it is done (the "Score" actor answers,
// or three seconds pass), move on to the next hole of the order, or leave a
// finished game.
void CGolfRule::DoScoreBoard(float elapsed) {
	switch (CSharedDoc::Instance()->m_roomType) {
	case 4:
	case 5:
	case 6:
	case 9:
	case 10: {
		bool done = false;
		SendMsg(this, "Score", 0x138, (intptr_t)&done, 0, 0, 0);

		if (!done) {
			return;
		}
		break;
	}
	default:
		if (m_modeTime < 3.0f) {
			return;
		}
		break;
	}

	if (OnlinePlay()) {
		if (CSharedDoc::Instance()->m_gameOver || GOLFDOC()->IsNetGameEnd()) {
			if (IsMassGame()) {
				if (!GOLFDOC()->GetStatUpdated()) {
					UpdateShotResultStat();
				}

				ReturnLobby(false);
				return;
			}

			ChangeGameMode(0x20000);
			return;
		}

		if (m_continueDlgOpen || m_netMsgWaitSend) {
			return;
		}

		GolfBall().SetHoleIn(0);

		if (GetHoleIndex(GOLFDOC()->m_currentHole) == CSharedDoc::Instance()->m_holes) {
			GOLFDOC()->m_currentHole = 1;
			GOLFDOC()->m_approachMode = true;
		} else {
			if (CSharedDoc::Instance()->m_roomInfo.m_roomMode == 14) {
				SetCurMap(CSharedDoc::Instance()->m_holeCourse[GetHoleIndex(GOLFDOC()->m_currentHole)]);
			}

			GOLFDOC()->m_currentHole = CSharedDoc::Instance()->m_holeOrder[GetHoleIndex(GOLFDOC()->m_currentHole)];
		}

		GOLFDOC()->m_pPolySoup->LoadMapCheckData(GOLFDOC()->m_currentHole);

		SetNetMsgWait(true, 0x1079, "DoScoreBoard", 40.0f);

		SendHoleData();
		CheckNetMsgWait_Send("DoScoreBoard");

		SendMsg(this, "GolfBg", 0x140, 0, 0, 0, 0);
		CheckNetMsgWait_Send("DoScoreBoard");

		unsigned char roomType = CSharedDoc::Instance()->m_roomType;
		if (roomType == 0 || roomType == 1 || roomType == 4 || roomType == 5 || roomType == 6) {
			SendMsg(this, "Score", 0x12F, 2, 0, 0, 0);
		}
	} else {
		if (CSharedDoc::Instance()->m_gameOver) {
			ChangeGameMode(0x20000);
			return;
		}

		if (GetHoleIndex(GOLFDOC()->m_currentHole) == CSharedDoc::Instance()->m_holes) {
			GOLFDOC()->m_currentHole = 1;
			GOLFDOC()->m_approachMode = true;
			GOLFDOC()->m_pPolySoup->LoadMapCheckData(GOLFDOC()->m_currentHole);
		} else {
			GOLFDOC()->m_currentHole = CSharedDoc::Instance()->m_holeOrder[GetHoleIndex(GOLFDOC()->m_currentHole)];
		}

		SendMsg(this, "GolfBg", 0x140, 0, 0, 0, 0);

		unsigned char roomType = CSharedDoc::Instance()->m_roomType;
		if (roomType == 0 || roomType == 1 || roomType == 4 || roomType == 5 || roomType == 6) {
			SendMsg(this, "Score", 0x12F, 2, 0, 0, 0);
		}
	}
}

// golfrule.cpp:4267
// The hole-in celebration: let the camera run, tell the server once, then
// hand over to the tutorial, the next hole or the score board.
void CGolfRule::DoHoleInPose(float elapsed) {
	if (m_modeTime < m_gameOverDelay) {
		CCameraWork::Instance()->Process(elapsed, 0.0f);
		return;
	}

	if (OnlinePlay() && !m_goStopSent && CSharedDoc::Instance()->m_playingMode == 1) {
		m_goStopSent = true;

		if (!IsMassGame())
			SetNetMsgWait(true, 0x10C4, "DoHoleInPose", 20.0f);

		WSendPacket packet((enumClientPacket)0x1C);
		EncodePacket((enumClientPacket)0x1C, packet);
		packet.Send(SEND_TO_DEFAULT);

		CheckNetMsgWait_Send("DoHoleInPose");
	}

	if (IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == 2) {
		m_continueDlgOpen = false;
		m_netMsgWaitSend = false;

		if (CSharedDoc::Instance()->m_replayState == 7) {
			CSharedDoc::Instance()->m_replayState = 9;
			g_input->SetFakeInput("FUNC_11", false);
		}
	}

	if (!m_continueDlgOpen && !m_netMsgWaitSend) {
		if (GOLFDOC()->m_gameState < 15) {
			SendMsg(this, "GolfRule", 0x88, 1, 0, 0, 0);

			IActor *tutorial = GetActor("Tutorial");
			if (tutorial) {
				if (GOLFDOC()->m_gameState == 8) {
					unsigned char score = PLAYER(0)->m_holes[GOLFDOC()->m_currentHole - 1];

					if (score < 5)
						tutorial << MsgObject(NULL, 0x149, 0, 0, 0, 0, 0);
					else
						tutorial << MsgObject(NULL, 0x154, 0, 0, 0, 0, 0);
				} else if (GOLFDOC()->m_gameState == 12) {
					tutorial << MsgObject(NULL, 0x154, 0, 0, 0, 0, 0);
				} else {
					tutorial << MsgObject(NULL, 0x148, 0, 0, 0, 0, 0);
				}
			}
		} else if (CSharedDoc::Instance()->m_roomType == 7) {
			if (m_shotBoardNextHole)
				ChangeGameMode(0x2000);
		} else if (CanMoveToNextHole()) {
			if (!OnlinePlay() || IsMassGame())
				ChangeGameMode(0x2000);
		} else if (!IsLocalContent((localContentType_t)0x4F) || CSharedDoc::Instance()->m_playingMode != 2) {
			SendMsg(this, "Score", 0x12F, 0, 0, 0, 0);
			if (g_pFresh)
				g_pFresh->GetManager()->CloseForm(false);
			ChangeGameMode(0x800);
		}
	}
}

// golfrule.cpp:4387
void CGolfRule::DoWaitForStart(float elapsed) {
	FlyBy(elapsed);

	m_waitingForStart = true;

	if (GOLFDOC()->m_currentHole != GetStartHole()) {
		ChangeGameMode(2);
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.size() == 1) {
		ReturnLobby(false);
	}
}

// golfrule.cpp:4429
// The camera's preview of the player: once it has run its course, report the
// tee shot ready and start the turn.
void CGolfRule::DoPlayerPreview(float elapsed) {
	CCameraWork::Instance()->Process(elapsed, 0.0f);

	if (CCameraWork::Instance()->m_transitionTime > 0.0f && CCameraWork::Instance()->m_cameraTime > CCameraWork::Instance()->m_transitionTime) {
		if (IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == 2) {
			ChangeGameMode(8);
			if (!GOLFDOC()->IsTeeShotReady())
				GOLFDOC()->SetTeeShotReady(true);
			return;
		}

		if (OnlinePlay()) {
			if (!GOLFDOC()->IsTeeShotReady()) {
				GOLFDOC()->SetTeeShotReady(true);

				if (GetGameType() == 10 || !IsMassGame())
					SetNetMsgWait(true, 0x1162, "DoPlayerPreview", 20.0f);

				WSendPacket packet((enumClientPacket)0x34);
				packet.Send(SEND_TO_DEFAULT);

				CheckNetMsgWait_Send("DoPlayerPreview");
			}
		}

		CSharedDoc *pDoc = CSharedDoc::Instance();
		unsigned char roomType = pDoc->m_roomType;
		if ((roomType != 10 && IsMassGame()) || (!m_continueDlgOpen && !m_netMsgWaitSend && m_goStopDlg == 0)) {
			if (roomType == 10)
				pDoc->m_gameStartTime = g_CurrentTime;

			SetTurnChangeEvent();
			ReadyForNextTurn();

			if (GOLFDOC()->m_currentHole == CSharedDoc::Instance()->m_holeOrder[CSharedDoc::Instance()->m_holes - 1])
				SendMsg(this, "Score", 0x135, 0, 0, 0, 0);

			CVoiceItem::Instance()->PlaySound(CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId, (CVoiceItem::eWaveState)0x20, -1);
		}
	}
}

// golfrule.cpp:4487
void CGolfRule::DoCloseupCaddie(float elapsed) {
	if (m_modeTime < 3.0f) {
		CCameraWork::Instance()->Process(elapsed, 0.0f);
	} else {
		CMouseCursor::Instance()->Reset(true);
		SendMsg(this, "Clock", 0x1E9, 0, 0, 0, 0);
		g_audio->PlaySfx("play_my_turn", 0, 0, 0, 0, 0.5f, 200.0f);
		ChangeGameMode(8);
	}
}

// golfrule.cpp:4501
void CGolfRule::ReadyForNextTurn(void) {
	CMouseCursor::Instance()->Reset(true);

	SendMsg(this, "Clock", 0x1E9, 0, 0, 0, 0);

	ChangeTurn();

	GOLFDOC()->MakeDefaultCamera(true);
	WPuppet *ball = CSceneManager::Instance()->GetPuppet("\xB0\xF8");

	if (ball && ball->GetRootBone())
		ball->SetAlpha(0xFF);
	g_audio->StopSfx("shot_dist_count");
	g_audio->PlaySfx("play_my_turn", 0, 0, 0, 0, 0.5f, 200.0f);
	CCameraWork::Instance()->StopBallSound();
	ChangeGameMode(8);
}

// The ladder (match) item drawn on every few holes: the item's type index
// and its chance out of 100 (CGolfRule::DispatchMatchItems).
struct sLadderItem {
	unsigned long itemIndex;  // 0x0, ORed with 0x18000000 into an item type id
	unsigned long rate;       // 0x4, chance out of 100
};
CHECK_SIZE(sLadderItem, 0x8);
CHECK_FIELD(sLadderItem, itemIndex, 0x0);
CHECK_FIELD(sLadderItem, rate, 0x4);

sLadderItem g_ladderItem[] = {
	{ 0x0, 15 },
	{ 0x1, 13 },
	{ 0xA, 18 },
	{ 0xB, 12 },
	{ 0x4, 18 },
	{ 0xC, 24 },
};

// golfrule.cpp:4521
void CGolfRule::DoDrive(float elapsed) {
	unsigned char powerShot = GolfClub().PowerShotType();

	if (CCameraWork::Instance()->GetCameraType() >= 0xC8 || (powerShot == 2 && m_replayDone) || (powerShot == 3 && m_replayDone)) {
		CCameraWork::Instance()->Process(elapsed, 0.0f);
	}

	if (m_continueDlgOpen || m_netMsgWaitSend)
		return;

	if (!m_replayDone) {
		if (!(m_replayFlags & 2)) {
			if (!g_input->Get("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", true))
				CCameraWork::Instance()->Unlock();
		}

		if (m_modeTime > 1.0f) {
			m_replayDone = true;
			SendMsg(this, "Player", 0x10E, 0, 0, 0, 0);

			if ((powerShot == 2 || powerShot == 3) && GolfClub().GetType() == 0) {
				WVector target;
				AfxGetTask()->GetActor("Player") << MsgObject(0, 0x127, (intptr_t)&target, 0, 0, 0, 0);

				char *camera = MakeStr("\xBF\xEC\xB5\xE5\xC6\xC4\xBF\xF6\xBC\xA6" "2_%02d", rand() % 4 + 1);
				m_gameOverDelay = CCameraWork::Instance()->LoadCamera((CCameraWork::eCamMode)2, camera, target);

				m_powerShotCamera = false;
			}
		}
	} else {
		if (m_modeTime > 11.0f)
			ChangeGameMode(0x80);
	}
}

// golfrule.cpp:4571
void CGolfRule::HandleMsg(const MsgObject &msg) {
	CGolfBall &ball = GolfBall();

	switch (msg.m_msg) {
	case 0x24A: {
		FrWnd *pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName((const char *)msg.m_param1);
		if (!pWnd)
			break;

		FrWnd **ppWnd = (FrWnd **)msg.m_param2;
		*ppWnd = pWnd;
		break;
	}

	case 0x21E:
		if (IsLocalContent((localContentType_t)0x58)) {
			m_modeTime = 0.0f;
		}
		break;

	case 0x29:
		SetReady();

		if (IsLocalContent((localContentType_t)1)) {
			if (m_flag4C4) {
				m_flag4C4 = false;

				FrClosedBonusBallResultDlg *pDlg = CreateForm<FrClosedBonusBallResultDlg>(g_pFresh->GetManager(), this, "closed_bonusball", 0);

				if (pDlg) {
					pDlg->Open((FRESH_RESULT_CALLBACK)0, 3);
					pDlg->SetClosedBonusBall((sBonusBall *)m_closedBonusBalls);
					pDlg->SetFixed(true);
				}
			}
		}
		break;

	case 0x9D:
		m_msgValue434 = (float)msg.m_param1;
		break;

	case 0x9B:
		m_msgValue430 = (float)msg.m_param1;
		break;

	case 0x9A:
		m_flag3F0 = true;
		break;

	case 0x271A:
		OnPreLoadInit();
		break;

	case 0x99: {
		m_gameOverDelay = 0;

		GOLFDOC()->m_teeShotReady = false;
		float radius = GolfBall().m_radius;
		WVector tee = GetTeeShotPos();
		GolfBall().Reset(WVector(tee.x, radius + tee.y, tee.z));
		GolfBall().m_groundArea = GetPVS().GetMeshIndex(GolfBall().m_pos, false);
		ResetData(GolfBall().m_pos);
		break;
	}

	case 0x98:
		if (m_chatViewWnd != NULL) {
			int lines = COption::Instance()->gIsChatLineEntended() ? 10 : 5;

			WPoint pt;
			pt.x = m_chatViewWnd->GetRect().x;
			pt.y = COption::Instance()->gGetBar_Y() - 100.0f - (lines * 19.0f + 8.0f);
			m_chatViewWnd->MoveWindow(pt);

			m_chatViewWnd->GetScrollBar()->Resize(lines, 1, false);

			if (COption::Instance()->gIsChatLineEntended()) {
				for (int i = 0; i < 5; i++) {
					const char *line = m_chatViewWnd->GetLine(i + 1, false);
					if (line)
						m_chatViewWnd->SetLine(i + 6, line, 0, false, 0);

					m_chatViewWnd->SetLine(i + 1, "", 0, false, 0);
				}
			}
			else {
				for (int i = 0; i < 5; i++) {
					const char *line = m_chatViewWnd->GetLine(i + 6, false);
					if (line)
						m_chatViewWnd->SetLine(i + 1, line, 0, false, 0);

					m_chatViewWnd->SetLine(i + 6, "", 0, false, 0);
				}
			}
		}
		break;

	case 0x69:
		switch (GOLFDOC()->GameMode()) {
		case 0x1000: {
			WCrypticValue<unsigned char> *pScore;
			if (WSingleton<CSharedDoc>::Instance()->m_roomType != 1)
				pScore = &PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1];
			else
				pScore = &TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1];

			int diff = (unsigned char)*pScore - GOLFDOC()->GetHoleData().m_par;
			if (diff <= 0)
				SetConstantBGM("bgm_under_par.mp3", true, true);
			else
				SetConstantBGM("bgm_over_par.mp3", true, true);
			break;
		}
		case 0x2000:
			SetConstantBGM("bgm_scoreboard.mp3", false, true);
			break;
		case 0x20000:
			SetConstantBGM("bgm_scoreboard.mp3", true, true);
			break;
		default:
			PlayBGM();
			break;
		}
		break;

	case 0x271B:
		if (m_chatViewWnd) {
			WPoint pt;
			pt.x = m_chatViewWnd->GetRect().x;
			pt.y = COption::Instance()->gGetBar_Y() - 100.0f
				- ((COption::Instance()->gIsChatLineEntended() ? 10 : 5) * 19.0f + 8.0f);
			m_chatViewWnd->MoveWindow(pt);
		}

		if (m_chatEdit) {
			WPoint pt;
			pt.x = m_chatEdit->GetRect().x;
			pt.y = COption::Instance()->gGetBar_Y() + 14.0f;
			m_chatEdit->MoveWindow(pt);
		}

		if (m_chatInput) {
			WPoint pt;
			pt.x = m_chatInput->GetRect().x;
			pt.y = COption::Instance()->gGetBar_Y() + 14.0f;
			m_chatInput->MoveWindow(pt);
		}

		if (m_emoticonBtn) {
			WPoint pt;
			pt.x = m_emoticonBtn->GetRect().x;
			pt.y = COption::Instance()->gGetBar_Y() + 16.0f;
			m_emoticonBtn->MoveWindow(pt);
		}

		if (m_languageArea) {
			WPoint pt;
			pt.x = m_languageArea->GetRect().x;
			pt.y = COption::Instance()->gGetBar_Y() + 16.0f;
			m_languageArea->MoveWindow(pt);
		}

		if (m_reportBtn) {
			WPoint pt;
			pt.x = m_reportBtn->GetRect().x;
			pt.y = COption::Instance()->gGetBar_Y() + 16.0f;
			m_reportBtn->MoveWindow(pt);
		}

		if (m_barButton[10]) {
			WPoint pt;
			pt.x = g_view->GetWidth() - 106.0f;
			pt.y = m_barButton[10]->GetRect().y;
			m_barButton[10]->MoveWindow(pt);
		}

		if (m_myInfoBtn) {
			WPoint pt;
			pt.x = g_view->GetWidth() - 54.0f;
			pt.y = m_myInfoBtn->GetRect().y;
			m_myInfoBtn->MoveWindow(pt);
		}

		if (m_messengerWnd) {
			WPoint pt;
			pt.x = g_view->GetWidth() - 81.0f;
			pt.y = m_messengerWnd->GetRect().y;
			m_messengerWnd->MoveWindow(pt);
			CMessengerInfo::Instance()->m_rcAlarmBtn = m_messengerWnd->GetRect();
		}

		if (m_messengerBubbleEdit) {
			WPoint pt;
			pt.x = g_view->GetWidth() - 167.0f;
			pt.y = m_messengerBubbleEdit->GetRect().y;
			m_messengerBubbleEdit->MoveWindow(pt);
		}

		if (m_messengerAlarmBtn) {
			WPoint pt;
			pt.x = g_view->GetWidth() - 110.0f;
			pt.y = m_messengerAlarmBtn->GetRect().y;
			m_messengerAlarmBtn->MoveWindow(pt);
		}

		if (m_messengerGiftAlarmBtn) {
			WPoint pt;
			pt.x = g_view->GetWidth() - 75.0f;
			pt.y = m_messengerGiftAlarmBtn->GetRect().y;
			m_messengerGiftAlarmBtn->MoveWindow(pt);
		}

		if (m_optionBtn) {
			WPoint pt;
			pt.x = g_view->GetWidth() - 29.0f;
			pt.y = m_myInfoBtn->GetRect().y;
			m_optionBtn->MoveWindow(pt);
		}

		if (m_commandBtns[0]) {
			WPoint pt;
			pt.x = g_view->GetWidth() - 423.0f;
			pt.y = m_commandBtns[0]->GetRect().y;
			m_commandBtns[0]->MoveWindow(pt);
		}

		if (m_commandBtns[1]) {
			WPoint pt;
			pt.x = g_view->GetWidth() - 398.0f;
			pt.y = m_commandBtns[1]->GetRect().y;
			m_commandBtns[1]->MoveWindow(pt);
		}

		if (m_commandBtns[2]) {
			WPoint pt;
			pt.x = g_view->GetWidth() - 373.0f;
			pt.y = m_commandBtns[2]->GetRect().y;
			m_commandBtns[2]->MoveWindow(pt);
		}

		if (m_commandBtns[3]) {
			WPoint pt;
			pt.x = g_view->GetWidth() - 348.0f;
			pt.y = m_commandBtns[3]->GetRect().y;
			m_commandBtns[3]->MoveWindow(pt);
		}

		ResetUiPos_Board();
		break;
	case 0x8B:
		if (GOLFDOC()->GetBar().m_accuracyGrade > 2) {
			*(float *)msg.m_param1 = (GOLFDOC()->GetBar().m_power - 140.0f) / data_00af1c38;
		} else {
			*(float *)msg.m_param1 = 0.01f;
		}
		break;


	case 0x8C:
		m_captureGrid = msg.m_param1;
		break;

	case 0x8A:
		*(WPoint *)msg.m_param1 = WPoint(GOLFDOC()->GetBar().m_prevAccuracy, COption::Instance()->gGetBar_Y());
		break;

	case 0xA2:
		if (m_gaugePaused == true) {
			m_gaugePaused = false;
			m_gaugeTime = m_pauseTime;
		} else {
			m_gaugePaused = true;
		}
		break;

	case 0x89: {
		PLAYER(0)->m_state = 0;

		WPolySoup::sPoint *pStart = GOLFDOC()->m_pPolySoup->FindPoint((const char *)msg.m_param1);
		float radius = GolfBall().m_radius;
		WVector ground = GetPVS().GetGroundPoint(pStart->m_pos, false, NULL, false);
		GolfBall().m_pos = WVector(ground.x, radius + ground.y, ground.z);
		GolfBall().m_groundArea = GetPVS().GetMeshIndex(GolfBall().m_pos, false);

		GolfBall().m_groundType = GetPVS().GetGroundType(GolfBall().m_pos, true, NULL);

		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos = GolfBall().m_pos;

		const char *target = (const char *)msg.m_param2;
		if (target) {
			WPolySoup::sPoint *pTarget = GOLFDOC()->m_pPolySoup->FindPoint(target);
			m_targetPoint = pTarget->m_pos;
		} else {
			m_targetPoint = GOLFDOC()->m_pAutoTarget->GetPoint(0);
		}

		ChangeTurn();

		SendMsg(this, "Screen", 0x1A3, 0, 0, 0, 0);
		SendMsg(this, "Effect", 0x177, 0, 0, 0, 0);
		SendMsg(this, "Trajectory", 0x18C, 0, 0, 0, 0);
		CFx::Instance()->ClearAll(true);

		if (GOLFDOC()->m_gameState < 15 && GOLFDOC()->m_gameState != 11) {
			Wind().SetIntensity(0.0f);
			Wind().SetDirection(0.0f);
		}

		GOLFDOC()->m_pPolySoup->UpdateBgFxWind();

		SendMsg(this, "GolfRule", 0x88, 0, 0, 0, 0);

		SetEnvironmentVariableA();
		GOLFDOC()->MakeDefaultCamera(true);

		ChangeGameMode(8);
		break;
	}

	case 0x206:
		if (IsLocalContent((localContentType_t)0x38)) {
			GolfClub().SetClub((unsigned char)msg.m_param1);
		}
		break;

	case 0x20D:
		if (IsLocalContent((localContentType_t)0x38)) {
			ToggleCommandBtns(true);
		}
		break;

	case 0x88:
		m_continueDlgOpen = msg.m_param1 != 0;
		break;

	case 0x7A: {
		if (IsObserver(0xFFFFFFFF)) {
			CSceneManager::Instance()->SetVisible(MakeStr("PLAYER_%d", GOLFDOC()->m_myPlayerIndex), true);
			CSceneManager::Instance()->SetVisible("\xC5\xAC\xB7\xB4", true);
			CSceneManager::Instance()->SetVisible("\xC4\xB3\xB5\xF0", true);
			CShadowManager::Instance()->Enable(COption::Instance()->vIsShadowEnabled());

			SendMsg(this, "Score", 0x13B, 0, 0, 0, 0);
		}

		if (GalleryMode())
			SendMsg(this, "ItemWindow", 0x195, 0, 0, 0, 0);

		GOLFDOC()->m_clockHold = 3;

		g_audio->StopSfx("\xC5\xB8\xC0\xD3");

		CMouseCursor::Instance()->SetCamMode(false);

		SendMsg(this, "Camera", 0x19B, 0, 0, 0, 0);
		SendMsg(this, "Letter", 0x18C, 0, 0, 0, 0);

		if (WSingleton<CSharedDoc>::Instance()->m_roomType != 3)
			SendMsg(this, "Screen", 0x1A7, 0, 0, 0, 0);
		GOLFDOC()->m_uiAlpha = 0x80;
		CChatMsg::Instance()->ReplaceBackupUiAlpha();

		sShotProtocol &proto = GOLFDOC()->m_shotProtocol;

		ball.m_lieSlope = proto.lieSlope;
		ball.m_lieAngle = proto.lieAngle;
		ball.m_powerFlags = proto.powerFlags;
		GOLFDOC()->GetBar().m_power = proto.power;
		GOLFDOC()->GetBar().m_position = proto.accuracy;
		GOLFDOC()->GetBar().m_prevAccuracy = proto.accuracy;
		GOLFDOC()->GetBar().m_accuracy = proto.accuracy;
		GOLFDOC()->GetBar().m_accuracyGrade = proto.accuracyGrade;
		GOLFDOC()->GetBar().m_stopped = WisEqual(GOLFDOC()->GetBar().m_power, BAR_END, 1e-5f) != 0;
		GOLFDOC()->GetBar().m_startPosition = proto.barStartPosition;
		GOLFDOC()->m_shotElapsed = proto.shotElapsed;

		if (GetShotTimeLimit()) {
			if ((unsigned long)GOLFDOC()->m_shotElapsed < GetShotTimeLimit())
				GOLFDOC()->m_remainShotTime = GetShotTimeLimit() - GOLFDOC()->m_shotElapsed;
			else
				GOLFDOC()->m_remainShotTime = 0;
		}

		GOLFDOC()->GetCameraData().m_yaw = proto.angle;
		GetPVS().SetFrame(proto.frame);
		GolfClub().SetClub(proto.club);
		GolfClub().SetRandomPower(proto.randomPower);

		m_spot = proto.spot;

		if (WSingleton<CSharedDoc>::Instance()->m_roomType != 3)
			SendMsg(this, "Spin", 0x165, 0, (int)GolfBall().m_lieSlope, (int)GolfBall().m_lieAngle, 0);

		SetAngle(GOLFDOC()->GetCameraData().m_yaw);
		SendMsg(this, "Player", 0x2718, (intptr_t)&GOLFDOC()->GetCameraData().m_yaw, 0, 0, 0);

		if (GOLFDOC()->m_approachMode)
			SendMsg(this, "Screen", 0x1C9, 0, 0, 0, 0);

		m_targetPoint = ball.m_pos + RotMat(GOLFDOC()->GetCameraData().m_yaw, 1).za;
		GOLFDOC()->MakeDefaultCamera(true);
		ChangeGameMode(0x40);
		break;
	}

	// TODO case 0x7B (golfrule.cpp:5032)
	case 0x74:
		m_point258 = *(const WVector *)msg.m_param1;
		break;

	case 0x9E:
		m_shotKeyLocked = msg.m_param1 == 1;
		if (m_shotKeyLocked) {
			m_shotKeyLockTime = 0;
		}
		break;

	case 0x76:
		if (GOLFDOC()->GameMode() == 8) {
			if (m_shotKeyLocked)
				break;

			if (!m_shotPlayable) {
				g_audio->PlaySfx("play_unplayable", 0, 0, 0, 0, 0.5f, 200.0f);
				break;
			}

			ChangeGameMode(0x10);
		} else if (GOLFDOC()->GameMode() & 0x300) {
			g_camera = GOLFDOC()->GetDefaultCamera();
			CMouseCursor::Instance()->Store();
			ChangeGameMode(8);
		}
		break;

	case 0x77:
		m_shotPlayable = msg.m_param1 != 0;
		break;

	case 0x78:
		ChangeGameMode(msg.m_param1);
		break;

	case 0xA4:
		if (GOLFDOC()->GameMode() == 0x80) {
			GolfBall().m_state = 0;
			break;
		}

		GolfBall().m_state = 0;
		GOLFDOC()->GameMode() = 0x80;

		if (WSingleton<CSharedDoc>::Instance()->m_replayPlayType == 1) {
			PLAYER(0)->m_extraPang = 0;
			PLAYER(0)->m_ghostAward = 0;
		}
		break;

	case 0x81:
		SetNetMsgWait(false, 0, "PM_NET_GAMESYNC_START", 0.0f);

		GOLFDOC()->m_myPlayerIndex = (unsigned char)msg.m_param1;
		SendMsg(this, "Score", 0x130, 0, 0, 0, 0);

		GOLFDOC()->InitShotResultMsg();

		if (OnlinePlay() == 1 && ((!GOLFDOC()->m_approachMode && GOLFDOC()->m_currentHole == WSingleton<CSharedDoc>::Instance()->m_holeOrder[0]) || WSingleton<CSharedDoc>::Instance()->m_roomType == 10)) {
			ChangeGameMode(2);
		} else {
			ChangeGameMode(1);
		}

		if (IsLocalContent((localContentType_t)0x39)) {
			if (WSingleton<CSharedDoc>::Instance()->IsMapEventActive(GetCurMap()) && WSingleton<CSharedDoc>::Instance()->m_holeOrder[0] == GOLFDOC()->m_currentHole) {
				// "(Notice) A map event is running. Finishing the game gives a bonus."
				CChatMsg::Instance()->AddChatMsg(std::string("(\xBE\xCB\xB8\xB2)\xB8\xCA\xC0\xCC\xBA\xA5\xC6\xAE\xB0\xA1 \xC1\xF8\xC7\xE0\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9.\xB0\xD4\xC0\xD3\xBF\xCF\xC1\xD6\xBD\xC3 \xBA\xB8\xB3\xCA\xBD\xBA\xB0\xA1 \xC1\xD6\xBE\xEE\xC1\xFD\xB4\xCF\xB4\xD9."), 0xFFFF7878, true, false);
			}
		}

		CShuffleBonus::Instance()->ShowNoticeMsg();
		break;

	case 0x243:
		HoleComboGaugeSetting();
		break;

	case 0x7C:
		GOLFDOC()->GetCameraData().m_yaw = CalcDeltaAngle(m_targetPoint - GolfBall().m_pos, WVector::UNIT_POS_Z);
		GOLFDOC()->MakeDefaultCamera(true);

		SetAngle(GOLFDOC()->GetCameraData().m_yaw);
		SendMsg(this, "Player", 0x2718, (intptr_t)&GOLFDOC()->GetCameraData().m_yaw, 0, 0, 0);

		if (IsMassGame())
			m_nextTurnPlayer = 0;
		else
			m_nextTurnPlayer = WSingleton<CSharedDoc>::Instance()->GetIndex(msg.m_param1);

		ChangeGameMode(0x4000);
		break;

	case 0x85:
		QuickSave((const char *)msg.m_param1);
		break;

	case 0x86:
		QuickLoad((char *)msg.m_param1);
		break;

	case 0x7D:
		SetNetMsgWait(false, 0, "PM_NET_NEXT_TURN", 0.0f);

		if (IsMassGame())
			m_nextTurnPlayer = 0;
		else
			m_nextTurnPlayer = WSingleton<CSharedDoc>::Instance()->GetIndex(msg.m_param1);

		ChangeGameMode(0x800);

		GolfClub().SetCalcRandomPower(false);
		break;

	case 0xA6:
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 10) {
			if (m_approachResultDlg == NULL) {
				WSingleton<CSharedDoc>::Instance()->m_gameStartTime = 0;

				m_approachResultDlg = CreateForm<FrApproachResultDlg>(g_pFresh->GetManager(), this, "approach_result", 0);
				m_approachResultDlg->SetApproachEnd(true);
				m_approachResultDlg->EnableDrag(false);

				WRect rect(m_approachResultDlg->GetRect());
				m_approachResultDlg->Open((FRESH_RESULT_CALLBACK)&CGolfRule::OnApproachDlgResult,
					WPoint(g_view->GetWidth() * 0.5f - rect.w * 0.5f, 35.0f), 0);
				m_approachResultDlg->SetTimeLimit(8.0f);

				rect.w += 30.0f;
				m_approachResultDlg->SetRect(rect);

				WSingleton<CProjectG>::Instance()->SetGameSpeed(1.0f);
			}
		}
		break;

	case 0x80:
		if (!GOLFDOC()->IsNetGameEnd()) {
			GOLFDOC()->NetGameEnd();
			GOLFDOC()->RefreshPlayerNum();

			for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
				if (PLAYER(i)->m_state == 3)
					continue;

				PLAYER(i)->m_joinState = GOLFDOC()->m_roundResults[i].m_joinState;
				PLAYER(i)->m_ghostResult = GOLFDOC()->m_roundResults[i].m_ghostResult;
				PLAYER(i)->m_totalScore = GOLFDOC()->m_roundResults[i].m_totalScore;

				PLAYER(i)->m_ghostAward = GOLFDOC()->m_roundResults[i].m_ghostAward;
				PLAYER(i)->m_extraPang = GOLFDOC()->m_roundResults[i].m_extraPang;

				if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
					TEAM(PLAYER(i)->m_attribute)->m_teamValue = PLAYER(i)->m_ghostAward;
					TEAM(PLAYER(i)->m_attribute)->m_teamValue2 = PLAYER(i)->m_extraPang;
				} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
					PLAYER(i)->m_itemPang = GOLFDOC()->m_roundResults[i].m_itemPang;
				}
			}

			SetNetMsgWait(false, 0, "PM_NET_GAME_END", 0.0f);

			g_view->SetFOV(0.95993108f);
			SendMsg(this, "Ball", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x1A8, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x1A5, 0, 0, 0, 0);
			SendMsg(this, "Effect", 0x177, 0, 0, 0, 0);
			SendMsg(this, "Trajectory", 0x18C, 0, 0, 0, 0);
			SendMsg(this, "Trajectory", 0x160, 0, 0, 0, 0);

			CFx::Instance()->ClearAll(true);

			if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
				if (WSingleton<CSharedDoc>::Instance()->m_userInfo.guid == GOLFDOC()->m_selectedIndexB && PLAYER(MyIndex())->m_itemPang > 0) {
					__int64 donation = (int)(PLAYER(MyIndex())->m_itemPang * 0.05f);
					PLAYER(MyIndex())->m_itemPang -= donation;
					// "Notice: 5% (%I64d pang) of the pang won in the pang battle is donated."
					CChatMsg::Instance()->AddChatMsg(std::string(MakeStr("\xBE\xCB\xB8\xB2 : \xC6\xCE\xB9\xE8\xC6\xB2 \xC8\xB9\xB5\xE6\xC6\xCE\xBF\xA1\xBC\xAD 5%%(%I64d\xC6\xCE)\xB8\xA6 \xB1\xE2\xBA\xCE \xC7\xD5\xB4\xCF\xB4\xD9.", donation)), 0xFFFF7878, false, false);
				}

				CalcSkinsHolePang();
			}

			ChangeGameMode(0x2000);
		}

		if (m_goStopDlg != NULL)
			m_goStopDlg->Close(FrNONE, true);

		if (m_dropDlg != NULL)
			m_dropDlg->Close(FrCANCEL, true);

		if (CTHunter::IsInstantiated())
			CTHunter::Instance()->SetReturnLobby(true);

		WSingleton<CSharedDoc>::Instance()->m_gameOver = true;
		break;

	case 0x7E:
		SetNetMsgWait(false, 0, "PM_NET_NEXT_HOLE", 0.0f);

		switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
		case 7:
			m_shotBoardNextHole = true;
			ChangeGameMode(0x1000);
			CalcSkinsHolePang();
			break;

		case 10:
			if (m_approachResultDlg == NULL) {
				WSingleton<CSharedDoc>::Instance()->m_gameStartTime = 0;

				m_approachResultDlg = CreateForm<FrApproachResultDlg>(g_pFresh->GetManager(), this, "approach_result", 0);
				m_approachResultDlg->EnableDrag(false);

				WRect rect(m_approachResultDlg->GetRect());
				m_approachResultDlg->Open((FRESH_RESULT_CALLBACK)&CGolfRule::OnApproachDlgResult,
					WPoint(g_view->GetWidth() * 0.5f - rect.w * 0.5f, 35.0f), 0);
				m_approachResultDlg->SetTimeLimit(8.0f);

				rect.w += 30.0f;
				m_approachResultDlg->SetRect(rect);

				WSingleton<CProjectG>::Instance()->SetGameSpeed(1.0f);
			}
			break;

		default:
			ChangeGameMode(0x2000);
			break;
		}

		GOLFDOC()->ClearShotResultMsg();
		GolfClub().SetCalcRandomPower(false);
		break;

	case 0x79:
		if (GOLFDOC()->m_gameMode & 0x40) {
			if (OnlinePlay()) {
				SendMsg(this, "Cutin", 0x78, 0x10, 0, 0, 0);
			}
			else {
				SetNetMsgWait(false, 0, "PM_NET_SHOT_RESULT", 0.0f);
			}

			m_replayFlags &= GOLFDOC()->m_shotResultMsg[GOLFDOC()->m_myPlayerIndex].m_replayMask;

			if (WSingleton<CSharedDoc>::Instance()->m_roomType == 0
				&& (GOLFDOC()->m_shotResultMsg[GOLFDOC()->m_myPlayerIndex].m_bonusFlags & 0x2000)) {
				if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty())) {
					unsigned long bonus = 0;
					for (unsigned char i = 0; i < WSingleton<CSharedDoc>::Instance()->m_holes; i++) {
						bonus = S5::CLASSICSRV::GetClearBonus(GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]).m_kind, GetCurMap());
					}

					GOLFDOC()->m_shotResultMsg[GOLFDOC()->m_myPlayerIndex].m_clearBonus += bonus;
				}
				else {
					unsigned char kind2 = 0, kind3 = 0, kind4 = 0;
					for (unsigned char i = 0; i < WSingleton<CSharedDoc>::Instance()->m_holes; i++) {
						switch (GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]).m_kind) {
						case 2: kind2++; break;
						case 3: kind3++; break;
						case 4: kind4++; break;
						}
					}

					GOLFDOC()->m_shotResultMsg[GOLFDOC()->m_myPlayerIndex].m_clearBonus
#ifdef JP_PAKS
						+= (kind4 * 3 + kind3 * 2 + kind2) * data_9e8f98[JpCourseRow(GetCurMap())];   // JP data: bounded row
#else
						+= (kind4 * 3 + kind3 * 2 + kind2) * data_9e8f98[GetCurMap()];
#endif
				}
			}

			m_replaying = true;
			m_canFastForward = GolfBall().m_groundType != 2;

			CorrectShotResult();

			SendMsg(this, "Player", 0x77, 0, 0, 0, 0);
		}
		break;

	case 0x87: {
		switch (msg.m_param1) {
		case 0:
			m_shotKeyAlways = false;
			break;

		case 1:
		case 2:
			m_shotKeyAlways = true;
			g_input->SetFakeInput("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", false);
			m_fakeShotTargets[0] = (float)msg.m_param2;
			m_fakeShotTargets[1] = (float)msg.m_param3;
			break;
		}

		IActor *pTutorial = GetActor("Tutorial");
		if (m_shotKeyAlways) {
			if (pTutorial != NULL) {
				pTutorial << MsgObject(0, 0x20F, 0, 0, 0, 0, 0);
			}
		}
		else {
			if (pTutorial != NULL) {
				pTutorial << MsgObject(0, 0x20F, 1, 0, 0, 0, 0);
			}
		}
		break;
	}

	case 0xA3:
		GOLFDOC()->m_clearedFlag15A = !GOLFDOC()->m_clearedFlag15A;
		break;

	// TODO case 0x82 (golfrule.cpp:5561)
	case 0x83:
		if (m_replaying)
			Skip();
		break;

	case 0x8D:
		Drop(*(const WVector *)msg.m_param1);
		break;

	case 0xA7:
		ForcedMoveBall(*(const WVector *)msg.m_param1);
		break;

	case 0x8E:
		Warp(*(const WVector *)msg.m_param1);
		break;

	case 0x8F:
		if (GOLFDOC()->m_gameState < 15) {
			if (msg.m_param1 == 0 && WSingleton<CSharedDoc>::Instance()->m_roomType == 12) {
				WSendPacket packet((enumClientPacket)0x2E);
				packet.Send(SEND_TO_DEFAULT);
			}

			ReturnLobby(false);
		} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 3) {
			WSendPacket packet(GCT_EXIT_ROOM);
			packet.Encode1(0);
			packet.Encode2(0xFFFF);
			packet.Encode8(0);
			packet.Encode8(0);
			packet.Send(SEND_TO_DEFAULT);
		} else {
			switch (msg.m_param1) {
			case 1:
				ReturnLobby(false);
				break;

			case 2: {
				FrForm *form = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				// "The opponent left the game."
				form->SetMessage("\xBB\xF3\xB4\xEB\xB0\xA1 \xB0\xD4\xC0\xD3\xBF\xA1\xBC\xAD \xB3\xAA\xB0\xAC\xBD\xC0\xB4\xCF\xB4\xD9", false);
				form->Open((FRESH_RESULT_CALLBACK)&CGolfRule::OnReturnLobbyResult, 3);
				break;
			}
			}
		}
		break;

	// TODO case 0x90 (golfrule.cpp:5703)
	case 0x91:
		m_pendingPowerFlags |= msg.m_param1;
		break;

	// TODO case 0x92 (golfrule.cpp:5725)
	case 0x93:
		SendShotResult();
		break;

	case 0x84:
		if (msg.m_param1) {
			if (m_pauseDlg == NULL) {
				m_pauseDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				m_pauseDlg->SetMessage((const char *)msg.m_param2, false);
				m_pauseDlg->Open((FRESH_RESULT_CALLBACK)&CGolfRule::PauseDlgResult, 0);
				m_pauseDlg->EnableDrag(false);

				GOLFDOC()->SetPause(true);
				CMouseCursor::Instance()->m_uiEnable[3] = false;
			}
		} else {
			GOLFDOC()->SetPause(false);

			if (m_pauseDlg)
				m_pauseDlg->Close((eFormRet)0, true);
		}
		break;

	case 0x94:
		SetNetMsgWait(false, 0, "PM_NET_TEESHOT_READY_ACK", 0.0f);
		GOLFDOC()->SetApproachReloaded(false);
		break;

	case 0x95: {
		GOLFDOC()->m_clockStopped = true;

		if (m_goStopDlg == NULL) {
			m_goStopTime = 0.0f;
			m_goStopDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);
			m_goStopDlg->Open((FRESH_RESULT_CALLBACK)&CGolfRule::GoStopDlgResult, 0);
			m_goStopDlg->EnableDrag(false);
		}

		int exp;
		SendMsg(this, "Score", 0x13A, (intptr_t)&exp, 0, 0, 0);

		// "The opponent left, so the match cannot go on; to keep your record, you can
		// carry on the game with the %d experience and %d pang earned so far.
		// Finishing the game also earns the clear bonus pang. Continue?"
		char text[1024];
		sprintf(text, "\xBB\xF3\xB4\xEB\xB0\xA1 \xC5\xF0\xC0\xE5\xC7\xD8\xBC\xAD \xB4\xF5 \xC0\xCC\xBB\xF3\xC0\xC7 \xB4\xEB\xC0\xFC\xC0\xBA \xBA\xD2\xB0\xA1\xB4\xC9\xC7\xCF\xB3\xAA, \xB0\xB3\xC0\xCE \xB1\xE2\xB7\xCF \xBC\xF6\xB8\xB3\xC0\xBB \xC0\xA7\xC7\xD8\xBC\xAD \xC7\xF6\xC0\xE7\xB1\xEE\xC1\xF6 \xBE\xF2\xC0\xBA\n\\c0xffff0000\\c\xB0\xE6\xC7\xE8\xC4\xA1 %d\\c0xff000000\\c\xBF\xCD \\c0xffff0000\\c%d \xC6\xCE\\c0xff000000\\c\xB8\xB8\xC0\xBB \xB9\xDE\xB0\xED \xB0\xD4\xC0\xD3\xC0\xBB \xB0\xE8\xBC\xD3 \xC1\xF8\xC7\xE0 \xC7\xD2 \xBC\xF6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.\n\xB1\xD7\xB8\xAE\xB0\xED, \xB0\xD4\xC0\xD3\xC0\xBB \xBF\xCF\xC1\xD6\xC7\xCF\xB8\xE9 \xC5\xAC\xB8\xAE\xBE\xEE \xBA\xB8\xB3\xCA\xBD\xBA \xC6\xCE\xC0\xBB \xB9\xDE\xC0\xBB \xBC\xF6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.\n\xB0\xE8\xBC\xD3 \xC1\xF8\xC7\xE0 \xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", exp, (int)PLAYER(MyIndex())->m_ghostAward);
		m_goStopDlg->SetMessage(text, false);
		m_goStopDlg->SetTimeLimit(10.0f);
		break;
	}

	case 0x62:
		if (m_emoticonBtn)
			m_emoticonBtn->SetVisible(msg.m_param1 != 0);

		if (m_languageArea)
			m_languageArea->SetVisible(msg.m_param1 != 0);

		if (m_reportBtn)
			m_reportBtn->SetVisible(msg.m_param1 != 0);

		if (m_chatEdit)
			m_chatEdit->SetVisible(msg.m_param1 != 0);

		if (m_chatInput) {
			if (msg.m_param1) {
				m_chatInput->SetVisible(true);
				m_chatInput->SetKeyFocus(true);
			} else {
				m_chatInput->SetVisible(false);
			}

			g_pFresh->GetManager()->SetExclusiveKey(msg.m_param1 != 0);
		}
		break;

	case 0x3F:
		*(const char **)msg.m_param1 = m_chatInput ? m_chatInput->GetLine(1, false) : 0;
		break;

	case 0x37:
		if (WSingleton<CSharedDoc>::Instance()->AddWhisperPartner(m_chatEdit, (const char *)msg.m_param1))
			m_chatEdit->SetLine(1, (const char *)msg.m_param1, 0, false, 0);

		if (CUserInfo::Instance()->IsVisible())
			CUserInfo::Instance()->Close();

		if (m_chatInput)
			m_chatInput->SetKeyFocus(true);
		break;

	case 0x65:
		if (WSingleton<CSharedDoc>::Instance()->m_recentWhisperList.size() > 0)
			m_chatEdit->SetLine(1, WSingleton<CSharedDoc>::Instance()->m_recentWhisperList.front().c_str(), 0, false, 0);
		break;

	case 0x38:
		WSingleton<CSharedDoc>::Instance()->AddWhisperPartner(m_chatEdit, (const char *)msg.m_param1);
		break;

	case 0x47:
		WSingleton<CSharedDoc>::Instance()->InitRecentWhisperList(m_chatEdit);
		break;

	case 0x64:
		if (m_chatViewWnd) {
			int lines = m_chatViewWnd->GetScrollBar()->GetRowCapacity();
			for (int i = 1; i < lines; i++) {
				const char *line = m_chatViewWnd->GetLine(i + 1, false);
				if (line)
					m_chatViewWnd->SetLine(i, line, 0, false, m_chatViewWnd->GetAnimTime(i + 1));
			}

			m_chatViewWnd->SetLine(lines, (const char *)msg.m_param1, msg.m_param2, false, timeGetTime());
		}
		break;

	case 0x96:
		if (m_chatViewWnd) {
			int lines = m_chatViewWnd->GetScrollBar()->GetRowCapacity();
			for (int i = 0; i < lines; i++) {
				const char *line = m_chatViewWnd->GetLine(i + 1, false);
				if (line != NULL && line[0] != '\0') {
					m_chatViewWnd->SetLine(i + 1, "", 0, false, 0);
					break;
				}
			}
		}
		break;

	case 0x63:
		if (m_chatViewWnd) {
			int lines = m_chatViewWnd->GetScrollBar()->GetRowCapacity();
			for (int i = 0; i < lines; i++)
				m_chatViewWnd->SetLine(i + 1, "", 0, false, 0);
		}
		break;

	case 0x97:
		if (IsLocalContent((localContentType_t)0x4F)) {
			if (WSingleton<CSharedDoc>::Instance()->m_replayPlayType != 2)
				CHelpMsg::Instance()->CheckReplayAvailable();
		} else {
			Replay(false);
			CHelpMsg::Instance()->CheckReplayAvailable();
		}
		break;

	case 0x9C:
		SetNetMsgWait(false, 0, "PM_NET_CANCEL_SHOT", 0.0f);
		Cancel();
		break;

	case 0x9F:
		if (msg.m_param1 == WSingleton<CSharedDoc>::Instance()->m_userInfo.guid)
			ChangeGameMode(0x2000);
		break;

	case 0x5E: {
		unsigned long uid = msg.m_param1;

		if (WSingleton<CSharedDoc>::Instance()->m_trophyUID[0] == uid || WSingleton<CSharedDoc>::Instance()->m_trophyUID[1] == uid) {
			sRivalData *pRed = GUILDMATCHUP(0);
			sRivalData *pBlue = GUILDMATCHUP(1);

			if (pRed == NULL || pBlue == NULL)
				break;

			int hole = msg.m_param2;

			if (pRed->m_holeShots[hole - 1] == 0 || pBlue->m_holeShots[hole - 1] == 0)
				break;

			char text[1024];
			if (pRed->m_holeShots[hole - 1] < pBlue->m_holeShots[hole - 1]) {
				// "%s won hole %d"
				sprintf(text, "%s\xB4\xD4\xC0\xCC %d\xC8\xA6\xBF\xA1\xBC\xAD \xBD\xC2\xB8\xAE\xC7\xCF\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9", pRed->m_nickname, hole);
			} else if (pRed->m_holeShots[hole - 1] > pBlue->m_holeShots[hole - 1]) {
				// "%s lost hole %d"
				sprintf(text, "%s\xB4\xD4\xC0\xBA %d\xC8\xA6\xBF\xA1\xBC\xAD \xC6\xD0\xB9\xE8\xC7\xCF\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9", pRed->m_nickname, hole);
			} else {
				// "Hole %d was a draw"
				sprintf(text, "%d\xC8\xA6\xC0\xBA \xB9\xAB\xBD\xC2\xBA\xCE\xB0\xA1 \xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9", hole);
			}

			CChatMsg::Instance()->AddChatMsg(text, 0xFFFF7878, true, false);

			if (pRed->m_holeShots[hole - 1] < pBlue->m_holeShots[hole - 1]) {
				GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[hole - 1]).m_mapState = 0;
			} else if (pRed->m_holeShots[hole - 1] > pBlue->m_holeShots[hole - 1]) {
				GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[hole - 1]).m_mapState = 1;
			} else {
				GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[hole - 1]).m_mapState = 2;
			}
		}
		break;
	}

	case 0xB1: {
		const char *pBalls = (const char *)msg.m_param1;

		if (pBalls) {
			m_flag4C4 = true;
			memcpy(m_closedBonusBalls, pBalls, sizeof(m_closedBonusBalls));
		}
		break;
	}

	case 0x1F9: {
		if (CQuitWindow::Instance()->IsActive())
			break;

		bool show = !WSingleton<CProjectG>::Instance()->HideGUI() && m_showGameUI;
		SendMsg(this, "Screen", 0x190, show ? 1 : 0, 0, 0, 0);
		SendMsg(this, "Letter", 0x190, show ? 1 : 0, 0, 0, 0);

		if (g_pFresh)
			g_pFresh->Toggle(show);

		CChatMsg::Instance()->Toggle(!WSingleton<CProjectG>::Instance()->HidePI() && m_showGameUI);

		CMouseCursor::Instance()->Toggle(!WSingleton<CProjectG>::Instance()->HideGUI() && !WSingleton<CProjectG>::Instance()->HidePI() && m_showGameUI);
		break;
	}

	case 0x24D: {
		bool send = false;
		if (msg.m_param1 == 1)
			send = true;

		SetNetMsgWait(send, 0x17D7, (char *)msg.m_param2, 20.0f);
		break;
	}

	default:
		HandleMsgCommon(msg);
		break;
	}
}

// golfrule.cpp:6132
void CGolfRule::DispatchMatchItems(void) {
	unsigned char interval = WSingleton<CSharedDoc>::Instance()->m_holes / 9;
	unsigned char holeIndex = GetHoleIndex(GOLFDOC()->m_currentHole);
	if (interval == 0)
		interval = 1;

	if (holeIndex > 0
		&& holeIndex % interval == 0
		&& WSingleton<CSharedDoc>::Instance()->m_roomType == 3) {
		unsigned long itemId = 0;
		unsigned long sum = 0;
		// The ladder round: the hole's position divided by the rounds' interval.
		unsigned char round = holeIndex / interval;
		unsigned long rnd = WSingleton<CSharedDoc>::Instance()->m_holeRandom[round] % 100;
		for (unsigned int i = 0; i < 6; i++) {
			sum += g_ladderItem[i].rate;
			if (sum > rnd) {
				itemId = g_ladderItem[i].itemIndex | 0x18000000;
				break;
			}
		}

		int slotCount = 10;
		if (IsLocalContent((localContentType_t)3))
			slotCount = 8;

		for (int player = 0; player < GOLFDOC()->GetPlayerNum(); player++) {
			if (GOLFDOC()->GetPlayer(player)->m_state == 3)
				continue;

			for (int slot = 0; slot < slotCount; slot++) {
				if (WSingleton<CSharedDoc>::Instance()->m_roomPlayers[player].m_equipItem[slot] == 0x1800000D) {
					WSingleton<CSharedDoc>::Instance()->m_roomPlayers[player].m_equipItem[slot] = itemId;
					break;
				}
			}
		}
	}
}

// golfrule.cpp:6190
// FIRST PASS IN PROGRESS (23 KB, golfrule.cpp:6190-8679): the prologue, the
// mode switch and the short cases are written; each empty case names the
// source lines still to write.
void CGolfRule::ChangeGameMode(unsigned long mode) {
	unsigned long oldMode = GOLFDOC()->GameMode();

	if (oldMode == 0x200 && GOLFDOC()->m_dropPending) {
		if (mode == 0x4000) {
			GOLFDOC()->m_dropPending = false;

			if (m_dropDlg != NULL)
				m_dropDlg->Close(FrNONE, false);
		} else {
			if (m_dropDlg == NULL) {
				m_dropDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);
				m_dropDlg->Open((FRESH_RESULT_CALLBACK)&CGolfRule::CancelDropDlgResult, 0);
				// "Cancel the ball moving mode."
				m_dropDlg->SetMessage("\xB0\xF8 \xBF\xC5\xB1\xE2\xB1\xE2 \xB8\xF0\xB5\xE5\xB8\xA6 \xC3\xEB\xBC\xD2\xC7\xD5\xB4\xCF\xB4\xD9.", false);
				m_dropDlg->EnableDrag(false);
			}

			return;
		}
	}

	GOLFDOC()->GameMode() = mode;
	m_modeTime = 0.0f;

	CGolfBall &ball = GolfBall();

	switch (mode) {
	case 0x200000:
		GOLFDOC()->m_clockHold = 0;
		m_gameOverDelay = Random(0.0f, 36.0f, -1);
		m_previewCenter = (GetHoleCupPos() + GetTeeShotPos()) * 0.5f;
		g_view->SetClip(32.0f, 3200.0f, true);

		CSceneManager::Instance()->SetVisible(MakeStr("PLAYER_%d", GOLFDOC()->m_myPlayerIndex), false);
		CSceneManager::Instance()->SetVisible("\xC5\xAC\xB7\xB4", false);	// "club"
		CSceneManager::Instance()->SetVisible("\xC4\xB3\xB5\xF0", false);	// "caddie"
		CShadowManager::Instance()->Enable(false);

		SendMsg(this, "Score", 0x13B, 1, 0, 0, 0);
		break;
	case 0x40000:
		GOLFDOC()->GetUIAlpha() = 0x80;
		CChatMsg::Instance()->ReplaceBackupUiAlpha();
		break;
	case 1: {
		IActor *pGateway = GetActor(s_gatewayActorName);
		pGateway << MsgObject(0, 0x26F, 0xC, 0, 0, 0, 0);

		GetPVS().ResetHoleData();
		SendMsg(this, "Letter", 0x18C, 0, 0, 0, 0);
		SendMsg(this, "Score", 0x12E, 0, 0, 0, 0);
		SendMsg(this, "Caddie", 0x164, 0, 0, 0, 0);

		m_gameOverDelay = 0;

		GOLFDOC()->m_teeShotReady = false;
		// DRAFT: 6257 is one line in the original (radius load, GetTeeShotPos, Reset);
		// written like HandleMsg 0x99 (golfrule.cpp:4639), which has the same code.
		float radius = ball.m_radius;
		WVector tee = GetTeeShotPos();
		ball.Reset(WVector(tee.x, radius + tee.y, tee.z));
		ball.m_groundArea = GetPVS().GetMeshIndex(ball.m_pos, false);
		ResetData(ball.m_pos);

		CTHunter::Instance()->SetReceiveGiftPacket(false);
		CSharedDoc::Instance()->m_value4843 = 0;
		CSharedDoc::Instance()->m_value4844 = 0;

		if (OnlinePlay() && GOLFDOC()->m_currentHole == CSharedDoc::Instance()->m_holeOrder[0]
			&& (CSharedDoc::Instance()->m_roomType == 10 || !GOLFDOC()->m_approachMode)) {
			SendMsg(this, "GolfBg", 0x1E5, (intptr_t)&m_flyByPoints, 0, 0, 0);
		} else {
			ChangeGameMode(2);
		}
		break;
	}
	case 0x10000: {
		if (GetCurMapProp() && GetCurMapProp()->m_ambientSound[0])
			g_audio->PlaySfx(GetCurMapProp()->m_ambientSound, 0, 1, 0, 0, 0.5f, 200.0f);

		SendMsg(this, "GolfBg", 0x142, m_weather, 0, 0, 0);

		Wind().Init(0xFF, 0xFF, 0, 1);
		Wind().SetWind(m_holeWind);
		GOLFDOC()->m_pPolySoup->UpdateBgFxWind();
		CSceneManager::Instance()->UpdateWind();

		GetPVS().ResetHoleData();

		m_shotBallPos = GetPVS().GetGroundPoint(m_shotBallPos, false, NULL, false);
		m_shotBallPos.y += ball.m_radius;

		ball.Reset(m_shotBallPos);
		ball.m_groundArea = GetPVS().GetMeshIndex(ball.m_pos, false);
		ResetData(ball.m_pos);

		PLAYER(0)->m_powerShotLevel = m_powerShotLevel;

		GetPVS().SetFrame(0);
		SetEnvironmentVariableA();

		float powerShot = m_shotPowerShot != 0 ? 1.0f : 0.0f;
		SendMsg(this, "Screen", 0x1AA, (intptr_t)&powerShot, 0, 0, 0);
		SendMsg(this, "Screen", 0x1B3, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1C5, 0, 0, 0, 0);
		SendMsg(this, "Logo", 0x18B, 0, 0, 0, 0);
		SendMsg(this, "Item", 0x19B, 0, 0, 0, 0);

		ChangeTurn();
		GOLFDOC()->MakeDefaultCamera(false);

		CShadowManager::Instance()->SetActive(true);

		GOLFDOC()->m_myPlayerIndex = 0;

		if (CSharedDoc::Instance()->m_replayPlayType == 2 || (CSharedDoc::Instance()->m_replayPlayType == 1 && CSharedDoc::Instance()->m_replayStartB == 0))
			ChangeGameMode(2);
		else
			ChangeGameMode(8);
		break;
	}
	case 0x100000: {
		// DRAFT: the buffer size is not visible (path at [ebp-290h]); 0x200 as in OnPreLoadInit.
		char path[0x200];
		strcpy(path, g_executeDirectory);
		sprintf(path, "%s/data/Sound/bgm", path);

		CheckUserBGM(path);
		m_bgmIndex = rand() % m_bgmList.size();

		g_audio->StopSfx(NULL);

		// DRAFT: `movzx; sub eax,2; je` is a switch (one case + default), not an if.
		switch (GetWeather()) {
		case 2:
			// "\xBA\xF1": "rain"
			if (GetCurMap() != 9 && !IsCurMapSnow())
				g_audio->PlaySfx("\xBA\xF1", 0, 1, "AmbientSound", 0, 0.5f, 200.0f);
			break;
		default:
			if (GetCurMapProp() && GetCurMapProp()->m_ambientSound[0])
				g_audio->PlaySfx(GetCurMapProp()->m_ambientSound, 0, 1, "AmbientSound", 0, 0.5f, 200.0f);
			else
				g_audio->StopSfx("AmbientSound");
			break;
		}

		GOLFDOC()->m_clockHold = 0;

		GetPVS().ResetHoleData();
		ball.Reset(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos);
		ball.m_groundArea = GetPVS().GetMeshIndex(ball.m_pos, false);

		// DRAFT: these five stores carry no line mark (scheduled into 6372's pushes); lines 6365-6371 by position.
		m_replaying = false;
		m_goStopSent = false;
		m_flag266 = false;
		m_flag267 = false;
		m_canFastForward = false;

		SendMsg(this, "Ball", 0x1EB, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1A3, 0, 0, 0, 0);
		SendMsg(this, "Clock", 0x1E9, 0, 0, 0, 0);
		SendMsg(this, "GolfBg", 0x13D, CSharedDoc::Instance()->m_weather, 0, 0, 0);

		m_targetPoint = ball.m_pos + RotMat(GOLFDOC()->GetCameraData().m_yaw, 1).za;

		SetAngle(GOLFDOC()->GetCameraData().m_yaw);
		SendMsg(this, "Player", 0xE2, 0, 0, 0, 0);
		SendMsg(this, "Caddie", 0x13, 0, 0, 0, 0);

		SendMsg(this, "Spot", 0xF5, 0, 0, 0, 0);

		// DRAFT: float local at [ebp-294h]; `1.0f` stored, then 0 when the power shot is off (a ?: or if/else on one line).
		float powerShot = GolfClub().PowerShotType() ? 1.0f : 0.0f;
		SendMsg(this, "Screen", 0x1AA, (intptr_t)&powerShot, 0, 0, 0);
		SendMsg(this, "Screen", 0x1B3, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1C5, 0, 0, 0, 0);
		SendMsg(this, "Logo", 0x18B, 0, 0, 0, 0);
		SendMsg(this, "Trajectory", 0x18C, 0, 0, 0, 0);
		SendMsg(this, "Letter", 0x18C, 0, 0, 0, 0);

		SendMsg(this, "Effect", 0x177, 0, 0, 0, 0);
		SendMsg(this, "Effect", 0x17F, 0, 0, 0, 0);
		SendMsg(this, "Effect", 0x178, 0, 0, 0, 0);

		SendMsg(this, "Score", 0x12F, 0, 0, 0, 0);

		ChangeTurn();
		GOLFDOC()->MakeDefaultCamera(false);
		GOLFDOC()->m_commandArrows.clear();

		// "\xB0\xF8": "ball"
		WPuppet *pBall = CSceneManager::Instance()->GetPuppet("\xB0\xF8");
		if (pBall && pBall->GetRootBone())
			pBall->SetAlpha(0xFF);

		CShadowManager::Instance()->SetActive(true);
		CFx::Instance()->ClearAll(true);

		ChangeGameMode(8);

		Wind().MakeWindList();
		Wind().CheckSpecialWind();
		GOLFDOC()->m_pPolySoup->UpdateBgFxWind();

		break;
	}
	case 2: {
		if (CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_GAME) {
			// DRAFT: 6428-6442 hold no code (comments in the original?).
			if (oldMode == 0x400000)
				m_flag5B8 = true;

			if (!m_flag5B8) {
				ChangeGameMode(0x400000);
				// DRAFT: return or break, both jump to the epilogue (0x45bc1f).
				return;
			}

			CSharedDoc::Instance()->m_shotDataList.clear();

			// DRAFT: 6456 reads [+0x4F0] ^ [+0x4E8], 6457 reads [+0x500] ^ [+0x4F8]: operand order differs,
			// check that both conversions schedule like the original.
			m_value59C = PLAYER(MyIndex())->m_ghostAward;
			m_value5A0 = PLAYER(MyIndex())->m_extraPang;
		} else if (CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK) {
			PLAYER(0)->m_extraPang = 0;
			PLAYER(0)->m_ghostAward = 0;

			CSharedDoc::Instance()->m_replayState = 1;

			CSharedDoc::Instance()->m_replayStartB = 1;
		}

		// DRAFT: `sub eax,1; je case 1; sub eax,1; jne default`: case 2's body comes first in the
		// source (6479 < 6486) and case 1 falls through into default (its GetCurMap() == 9 jumps to 6490).
		switch (GetWeather()) {
		case 2:
			// "\xBA\xF1": "rain"
			if (GetCurMap() != 9 && !IsCurMapSnow())
				g_audio->PlaySfx("\xBA\xF1", 0, 1, "AmbientSound", 0, 0.5f, 200.0f);
			break;
		case 1:
			// "\xB9\xF8\xB0\xB3\xC1\xD8\xBA\xF1%d": "lightning ready %d"
			if (GetCurMap() != 9)
				g_audio->PlaySfx(MakeStr("\xB9\xF8\xB0\xB3\xC1\xD8\xBA\xF1%d", rand() % 2 + 1), 0, 0, 0, 0, 0.5f, 200.0f);
		default:
			if (GetCurMapProp() && GetCurMapProp()->m_ambientSound[0])
				g_audio->PlaySfx(GetCurMapProp()->m_ambientSound, 0, 1, "AmbientSound", 0, 0.5f, 200.0f);
			else
				g_audio->StopSfx("AmbientSound");
			break;
		}

		m_bgmIndex = rand() % m_bgmList.size();

		SendMsg(this, "GolfBg", 0x13D, CSharedDoc::Instance()->m_weather, 0, 0, 0);
		SendMsg(this, "Effect", 0x17A, 0, 0, 0, 0);
		SendMsg(this, "Effect", 0x178, 0, 0, 0, 0);

		if (!OnlinePlay())
			this << MsgObject(NULL, 0x243, 0, 0, 0, 0, 0);

		for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			if (PLAYER(i)->m_state == 3)
				continue;

			if (!GOLFDOC()->m_approachMode)
				CSharedDoc::Instance()->m_gameStatistics[i].drives++;

			PLAYER(i)->m_state = 0;
			// DRAFT: one source line (radius read before the GetTeeShotPos call, no ??H call); written like golfbg.cpp:171.
			float radius = GolfBall().m_radius; WVector tee = GetTeeShotPos(); PLAYER(i)->m_ballPos = WVector(tee.x, radius + tee.y, tee.z);
		}

		GOLFDOC()->m_myPlayerIndex = GetFirstTurn();

		// DRAFT: the stores [+0x21C0] = 0 then [+0x21BC] = 0 are SetFrame's (inlined, its quadtree.h
		// marks lost to scheduling into 6528); one GetPVS call.
		GetPVS().SetFrame(0);
		SetEnvironmentVariableA();

		if (!IsMyTurn(false))
			SendMsg(this, "Spin", 0x164, 0, 0, 0, 0);

		SendMsg(this, "Caddie", 0x163, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1B2, 0, 0, 0, 0);
		SendMsg(this, "Logo", 0x18B, 0, 0, 0, 0);
		SendMsg(this, "NPC", 0x18A, 0, 0, 0, 0);

		if ((IsMassGame() && GOLFDOC()->m_currentHole != CSharedDoc::Instance()->m_holeOrder[0]) || GOLFDOC()->m_approachMode) {
			// "\xB5\xEE\xC0\xE5\xB8\xF0\xBC\xC7": "entrance motion"
			if (!GOLFDOC()->m_approachMode)
				SendMsg(this, "Player", 0x2716, (intptr_t)"\xB5\xEE\xC0\xE5\xB8\xF0\xBC\xC7", -3, 1, 0);

			SendMsg(this, "Player", 0x125, 1, 0, 0, 0);
		} else {
			SendMsg(this, "Player", 0x2716, (intptr_t)"\xB5\xEE\xC0\xE5\xB8\xF0\xBC\xC7", 0, 0, 0);
		}

		ChangeTurn();
		GOLFDOC()->MakeDefaultCamera(false);

		CCameraWork::Instance()->SelectCamera(0xCB);

		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] = 1;

		if (CSharedDoc::Instance()->m_roomType == 1) {
			for (int team = 0; team < 2; team++)
				TEAM(team)->m_state = 0;

			TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] = 1;
			TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_clubHolder = GOLFDOC()->m_myPlayerIndex;
			TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_clubHolderClub = GolfClub().GetType();
		} else if (CSharedDoc::Instance()->m_roomType == 7) {
			if (!GOLFDOC()->m_approachMode) {
				// DRAFT: 6573-6577 and 6579-6582 hold no code.
				if (GOLFDOC()->m_maxPangPerLoser) {
					GOLFDOC()->m_skinsPangTotal += GetHolePang(GetHoleIndex(GOLFDOC()->m_currentHole));

					// DRAFT: one store, `add eax,eax; cmp eax,8; jb` = min(x * 2, 8) (windows.h macro, as CompoundDlg.cpp uses).
					GOLFDOC()->m_skinsCarryCount = min(GOLFDOC()->m_skinsCarryCount * 2, 8);

					// DRAFT: __allmul gets m_skinsCarryCount (zero-extended) as its first operand; swap if it pushes the other way.
					GOLFDOC()->m_maxPangPerLoser = GOLFDOC()->m_skinsCarryCount * GOLFDOC()->m_skinsPangTotal;
					GOLFDOC()->GetHoleData(GetHoleIndex(GOLFDOC()->m_currentHole)).m_pang = GOLFDOC()->m_maxPangPerLoser;

				} else {
					GOLFDOC()->m_skinsPangTotal = GetHolePang(GetHoleIndex(GOLFDOC()->m_currentHole));
					GOLFDOC()->m_maxPangPerLoser = GetHolePang(GetHoleIndex(GOLFDOC()->m_currentHole));
					GOLFDOC()->m_skinsCarryCount = 1;
				}

				// DRAFT: 6601-6611 hold no code.
				GOLFDOC()->m_totalPang = 0;

				for (int i = 0; i < CSharedDoc::Instance()->m_holes; i++) {
					unsigned char hole = CSharedDoc::Instance()->m_holeOrder[i];

					if (i < GetHoleIndex(GOLFDOC()->m_currentHole) && GOLFDOC()->GetHoleData(hole).m_pang) {
						__int64 pang = GOLFDOC()->GetHoleData(hole).m_pang;
						int record = PLAYER(MyIndex())->m_holeRecord2[hole - 1];

						if (record != -1)
							GOLFDOC()->m_totalPang += pang;

					} else {
						GOLFDOC()->m_totalPang += GOLFDOC()->GetHoleData(hole).m_basePang;
					}
				}
			}
		}

		CShadowManager::Instance()->SetActive(true);

		Wind().MakeWindList();
		Wind().CheckSpecialWind();
		if (GOLFDOC()->m_pPolySoup)
			GOLFDOC()->m_pPolySoup->UpdateBgFxWind();

		break;
	}
	case 4: {
		g_view->SetFOV(0.9599311f);

		SendMsg(this, "Screen", 0x1A4, 1, 0, 0, 0);
		SendMsg(this, "Caddie", 0x2716, (intptr_t)"\xC2\xF9\xBD\xBA", 0, 0, 0);	// "chance"

		if (CSharedDoc::Instance()->m_roomType != 1)
			SendMsg(this, "Caddie", 0x1DC, 0x22, 0, 0, 0);

		WVector caddiePos;
		SendMsg(this, "Caddie", 0xF4, (intptr_t)&caddiePos, 0, 0, 0);
		CCameraWork::Instance()->SetCaddieCamera(g_camera, caddiePos);
		break;
	}
	case 8:
		if (IsLocalContent((localContentType_t)0x4F)) {
			if (CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_GAME) {
				CSharedDoc::Instance()->m_replayPlayType = 0;
				// DRAFT: one decode after the merge, so the ?: picks the WCrypticValue<float> lvalue and converts once.
				m_shotStartComboGauge = CSharedDoc::Instance()->m_roomType == 1 ? TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_teamRate : PLAYER(GOLFDOC()->m_myPlayerIndex)->m_comboGauge;
			} else if (CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK) {
				SetEnvironmentVariableA();
			}
		}

		if (IsLocalContent((localContentType_t)0x51)) {
			CCardManager::Instance()->SetPlayerIndex(GOLFDOC()->m_myPlayerIndex);
			CCardManager::Instance()->CalcCardPeriodAndStatus();
		}

		_ImeReset();

		// DRAFT: 6752 holds the jump over the else and 6754 the `== 9` test, so the source may read
		// `else { if (== 9) ... else if (...) ... }`; the else-if chain below compiles the same.
		if (GOLFDOC()->m_gameState == 15) {
			if (m_barButton[10])
				m_barButton[10]->SetVisible(true);

			if (m_myInfoBtn)
				m_myInfoBtn->SetVisible(true);

			if (m_optionBtn)
				m_optionBtn->SetVisible(true);

			ToggleCommandBtns(true);

			if (m_boardWnd)
				m_boardWnd->SetVisible(true);

			if (m_onelineBoardWnd)
				m_onelineBoardWnd->SetVisible(true);

			COneLineBoard::Instance()->ShowBg(true);

			if (m_messengerWnd)
				m_messengerWnd->SetVisible(true);

			if (CSharedDoc::Instance()->m_value4DBD && m_messengerAlarmBtn) {
				m_messengerAlarmBtn->SetTopmost(true);
				m_messengerAlarmBtn->SetVisible(true);
			}

			if (CSharedDoc::Instance()->m_tutorialFinish && m_messengerGiftAlarmBtn) {
				m_messengerGiftAlarmBtn->SetTopmost(true);
				m_messengerGiftAlarmBtn->SetVisible(true);
			}
		} else if (GOLFDOC()->m_gameState == 9) {
			ToggleCommandBtns(true);
		} else if (CSharedDoc::Instance()->m_tutorialFinish && m_messengerGiftAlarmBtn) {
			m_messengerGiftAlarmBtn->SetTopmost(false);
			m_messengerGiftAlarmBtn->SetVisible(false);
		}

		m_flag267 = true;

		if (ball.m_landed) {
			ball.m_landed = false;
			GOLFDOC()->MakeDefaultCamera(true);
			ball.m_lie = 0;
		} else {
			g_camera = GOLFDOC()->GetDefaultCamera();
		}

		// DRAFT: the original lays the else (Screen 0x1A8, line 6790) right after the
		// condition and the if-body (6783-6786) after it; check the block order.
		if (IsMyTurn(true) || (IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK)) {
			SendMsg(this, "Spin", 0x163, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x1A7, 0, 0, 0, 0);
			GOLFDOC()->GetUIAlpha() = 0xFF;
			CChatMsg::Instance()->ReplaceBackupUiAlpha();
		} else {
			SendMsg(this, "Screen", 0x1A8, 0, 0, 0, 0);
		}

		if (!(CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_identity & 1))
			SendMsg(this, "Camera", 0x19A, 0, 0, 0, 0);

		SendMsg(this, "Camera", 0xEE, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1A4, 1, 0, 0, 0);

		SendMsg(this, "Spot", 0x163, 0, 0, 0, 0);

		// 55 degrees in radians.
		g_view->SetFOV(0.95993109f);

		if (CSharedDoc::Instance()->m_replayState == 1) {
			CSharedDoc::Instance()->m_replayState = 0;
			// "space bar"
			g_input->SetFakeInput("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", false);
		} else if (CSharedDoc::Instance()->m_replayState == 7) {
			// "space bar"
			g_input->SetFakeInput("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", false);
		}
		break;
	case 0x10:
		g_audio->StopSfx("\xC5\xB8\xC0\xD3");	// "time"

		CMouseCursor::Instance()->SetActive(false);
		CMouseCursor::Instance()->SetCamMode(false);

		SendMsg(this, "Score", 0x12F, 0, 0, 0, 0);
		if (g_pFresh)
			g_pFresh->GetManager()->CloseForm(false);

		SendMsg(this, "Camera", 0x19B, 0, 0, 0, 0);
		SendMsg(this, "Letter", 0x18C, 0, 0, 0, 0);

		GOLFDOC()->m_clockHold++;

		SendMsg(this, "Spin", 0x161, (intptr_t)&GolfBall().m_lieSlope, (intptr_t)&GolfBall().m_lieAngle, 0, 0);
		SendMsg(this, "Spot", 0xFA, (intptr_t)&m_spot, 0, 0, 0);

		GolfBall().m_powerFlags = 0;
		GOLFDOC()->m_commandArrows.clear();
		m_commandTime = 0;

		if (OnlinePlay()) {
			struct {
				unsigned char type;
				float startPosition;
			} __declspec(align(1)) data;
			data.type = 1;
			data.startPosition = GOLFDOC()->GetBar().m_startPosition;

			WSendPacket packet(GCT_CLICK);
			packet.EncodeBuffer(&data, 5);
			packet.Send(SEND_TO_DEFAULT);
		}
		break;
	case 0x20:
		if (m_chatViewWnd && CCapturedBg::Instance()->IsCaptured())
			m_chatViewWnd->SetVisible(false);

		GOLFDOC()->GetBar().m_state = 1;

		m_flag267 = false;
		m_gaugeTime = 0;
		m_gaugeStartTime = 0;
		m_pauseTime = 0;

		if (IsMyTurn(false)) {
			timeBeginPeriod(1);
			CFx::Instance()->SetActive(false);
		}
		break;
	case 0x100:
		GOLFDOC()->GetUIAlpha() = 0x80;
		CChatMsg::Instance()->ReplaceBackupUiAlpha();
		break;
	case 0x400:
		m_debugCameraStart = g_camera;
		m_debugCameraEye = g_camera.pivot;
		m_previewCenter = g_camera.za + m_debugCameraEye;
		SendMsg(this, "Camera", 0x19B, 0, 0, 0, 0);

		if (m_barButton[10])
			m_barButton[10]->SetVisible(false);

		if (m_myInfoBtn)
			m_myInfoBtn->SetVisible(false);

		if (m_optionBtn)
			m_optionBtn->SetVisible(false);

		ToggleCommandBtns(false);

		if (m_messengerWnd)
			m_messengerWnd->SetVisible(false);

		if (m_messengerAlarmBtn) {
			m_messengerAlarmBtn->SetVisible(false);
			m_messengerAlarmBtn->SetTopmost(false);
		}

		if (m_messengerGiftAlarmBtn) {
			m_messengerGiftAlarmBtn->SetVisible(false);
			m_messengerGiftAlarmBtn->SetTopmost(false);
		}
		break;
	case 0x800:
		// Back to the default camera; a player who gave up is done.
		if (!GOLFDOC()->m_netGameEnd) {
			if (CCameraWork::Instance()->GetCameraType() == 0x15)
				CCameraWork::Instance()->UninstallCamera();

			m_gameOverDelay = 0;

			if (GOLFDOC()->GetPlayer(GOLFDOC()->m_myPlayerIndex)->m_state == 3) {
				m_goStopSent = true;
				break;
			}

			if (IsLocalContent((localContentType_t)0x30))
				AfxGetTask()->GetActor("Screen") << MsgObject(0, 0xA0, 0, 0, 0, 0, 0);
		}
		break;
	case 0x200:
		// The top view: hide the buttons, then a wide fixed camera above the hole.
		if (m_barButton[10])
			m_barButton[10]->SetVisible(false);

		if (m_myInfoBtn)
			m_myInfoBtn->SetVisible(false);

		if (m_optionBtn)
			m_optionBtn->SetVisible(false);

		ToggleCommandBtns(false);

		if (m_messengerWnd)
			m_messengerWnd->SetVisible(false);

		if (m_messengerAlarmBtn)
			m_messengerAlarmBtn->SetVisible(false);

		if (m_messengerGiftAlarmBtn) {
			m_messengerGiftAlarmBtn->SetVisible(false);
			m_messengerGiftAlarmBtn->SetTopmost(false);
		}

		GOLFDOC()->GetUIAlpha() = 0x80;
		CChatMsg::Instance()->ReplaceBackupUiAlpha();

		g_view->SetFOV(1.0471976f);
		g_view->SetClip(3.2f, 3200.0f, true);

		g_camera = GOLFDOC()->MakeTopViewCameraMatrix(false);
		SendMsg(this, "Camera", 0xF1, 0, 0, 0, 0);
		CSceneManager::Instance()->UpdateFog(false);

		if (m_kbdManualDlg)
			m_kbdManualDlg->Close(true);
		break;
	case 0x40: {
		// The shot: the bar is clicked, the ball's launch is computed and the
		// flight predicted.
		if (CCapturedBg::Instance())
			CCapturedBg::Instance()->Reset();

		if (GolfClub().GetType() != 3) {
			int timing = GetBarClickTiming();
			SendMsg(this, "Screen", 0x1AC, 0, 0, 0, 0);
			SendMsg(this, "Caddie", 0x1DF, timing, 0, 0, 0);

			if (GolfClub().PowerShotType()) {
				g_audio->PlaySfx("powershot_ball_effect", 0, 0, 0, 0, 0.5f, 200.0f);
				SendMsg(this, "Effect", 0x17E, 1, 0, 0, 0);
			} else {
				SendMsg(this, "Player", 0x10F, 0, 0, 0, 0);
			}
		}

		SendMsg(this, "Clock", 0x1EA, 0, 0, 0, 0);
		SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
		SendMsg(this, "ShotReview", -1, (intptr_t)"SAVE_SPOT_INDICATOR", 0, 0, 0);

		// DRAFT: golfrule.cpp:7027-7072 carry no line marks (the PDB gives this
		// stretch to :8392); the statement split below is a guess.
		if (GolfClub().GetType() != 3 && Bar().m_accuracyGrade == 4)
			CSharedDoc::Instance()->m_gameStatistics[GOLFDOC()->m_myPlayerIndex].pangyas++;

		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_playTime += GOLFDOC()->m_shotElapsed;

		if (IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK)
			GolfBall().m_powerFlags = m_shotPowerFlags;

		float persent = Bar().Persent();
		float powerRate = sqrtf(persent); // DRAFT: [ebp-0x298], reused by the flight loop below
		float power = GolfClub().GetPower() * powerRate;
		power *= GolfBall().GetPowerFactor();
		power *= GetBarClickPowerFactor();

		CGolfBall::sExpected *expected = ball.PreDataToWrite(0);
		expected->m_groundType = 0;
		expected->m_pos = ball.m_pos;
		expected->m_contactType = GetInitBallState();

		// DRAFT: m_spot (0x278) is pushed raw as BallSpeedFactor's float
		// powerBonus: the member is a float (see the names list).
		BallSpeedFactor factor(CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_characterInfo.typeId, GolfBall(), m_spot);
		expected->m_velocity = factor.m_velocity;
		ball.m_lieSlope = factor.m_spin;
		ball.m_lieAngle = factor.m_curve;
		float angle = factor.m_angle;
		float yaw = factor.m_power;

		if (IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK) {
			ball.m_lieSlope = m_shotLieSlope;
			yaw = m_shotYaw;
			angle = m_shotAngle;
		}

		if (Bar().m_accuracyGrade > 1)
			expected->m_velocity = power * (Abs(ball.m_lieSlope) * 0.1f + 1.0f) * RotMat(GolfClub().GetLie(NULL), 0).za * RotMat(yaw + angle, 1);

		switch (CSharedDoc::Instance()->m_playingMode) {
		case REPLAY_MODE_GAME:
			if (!WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT) || IsMyTurn(false)) {
				memset(&m_hole, 0, sizeof(sShotData));
				m_hole = GOLFDOC()->m_currentHole;
				m_holeWind = Wind().GetGlobalWind();
				m_weather = CSharedDoc::Instance()->m_weather;
				m_shotNumber = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1];
				m_shotYaw = yaw;
				m_shotFrame = GetPVS().GetFrame(); // DRAFT: read inline ([eax+0x21C0]); SetFrame below is a call
				m_shotBallPos = ball.m_pos;
				m_shotBar = Bar();
				m_shotLieSlope = ball.m_lieSlope;
				m_shotLieAngle = ball.m_lieAngle;
				m_shotPowerFlags = ball.m_powerFlags;
				m_shotPowerShot = GolfClub().PowerShotType();
				m_powerShotLevel = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel;
				strcpy(m_clubName, GolfClub().GetName());
				m_shotItemTypeId = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId;
				m_shotAngle = angle;

				m_shotCountKey = CSharedDoc::Instance()->m_roomType == 1 ? TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] : PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1];

				m_ghostAward = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ghostAward - m_value59C;
				m_extraPang = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_extraPang - m_value5A0;
				m_comboGauge = m_shotStartComboGauge;

				CSharedDoc::Instance()->m_shotDataList.push_back(*(sShotDataStorage *)&m_hole); // DRAFT: the original pushes a CGolfRule::sShotData (list<sShotData>::push_back)
			}
			break;
		case REPLAY_MODE_PLAYBACK:
			GolfBall().m_lieSlope = m_shotLieSlope;
			GolfBall().m_lieAngle = m_shotLieAngle;
			GolfBall().m_powerFlags = m_shotPowerFlags;

			GetPVS().SetFrame(m_shotFrame);
			break;
		}

		float impactTime; // DRAFT: [ebp-0x2C8], filled by the "Player" actor (CGolfPlayer 0x126)
		SendMsg(this, "Player", 0x126, (intptr_t)&impactTime, 0, 0, 0);

		if (!OnlinePlay() || IsMassGame()) {
			SendMsg(this, "Player", 0x77, 0, 0, 0, 0);
			m_replaying = true;
			m_canFastForward = ball.m_groundType != 2;
		} else if (IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK) {
			SendMsg(this, "Player", 0x77, 0, 0, 0, 0);
		}

		unsigned long frame = GetPVS().GetFrame(); // DRAFT: :7147 is the GetPVS call alone; the m_frame read is scheduled into :7149

		m_shotStartFrame = frame;
		m_replayFrame = frame + 50;
		m_flightFrame = m_replayFrame - (int)(impactTime * -50.0f); // DRAFT: truncated inline (fnstcw/fistp, tough problem 014), not __ftol2

		GetPVS().SetFrame(m_flightFrame);

		ball.m_velocity = expected->m_velocity;
		ball.m_state = expected->m_contactType;

		ball.m_flags = ball.m_powerFlags;

		// DRAFT: the three stores below carry the one mark :7160.
		m_powerShotCamera = true;
		m_replayDone = false;
		m_replayMode = 0;
		ball.m_lie = 0;
		ball.SetHoleIn(0);
		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_value4C8 = -1;
		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags = 0;

		if (GolfBall().m_state != 0)
			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel = 0;

		GetPVS().ResetVariable();

		ball.ClearBuffer();

		// DRAFT: :7174-7187 hold no other mark; one statement each, in the
		// order the original stores them.
		ball.m_vectorSlideCount = 0; // DRAFT: may be ball.ResetVectorSlideCount() (golfball.h:192, GolfRule emits its copy)
		ball.m_preDataCount = 0;
		ball.m_eventPreDataIndex[0] = -1;
		ball.m_eventPreDataIndex[1] = -1;
		ball.m_eventPreDataIndex[2] = -1;
		ball.m_eventPreDataIndex[3] = -1;
		ball.m_eventPreDataIndex[4] = -1;
		ball.m_eventPreDataIndex[5] = -1;
		ball.m_eventPreDataIndex[8] = -1;
		ball.m_eventPreDataIndex[6] = -1;
		ball.m_eventPreDataIndex[7] = ball.m_state == 3 ? 0 : -1;

		ball.m_shotStage = 0;
		ball.m_bounceCount = 0;
		ball.m_firstContactIndex = -1;
		ball.m_firstContactId = -1;
		ball.m_lastContactIndex = -1;
		ball.m_lastContactId = -1;
		ball.m_groundArea = -1;
		ball.m_eventPreDataIndex[9] = -1;
		ball.m_eventPreDataIndex[10] = -1;
		ball.m_spinImpulse.Reset(); // DRAFT: stores z, y, x
		// DRAFT: :7199-7204 have no marks; the [11]-[14] stores sit under :7205.
		ball.m_eventPreDataIndex[12] = -1;
		ball.m_eventPreDataIndex[13] = -1;
		ball.m_eventPreDataIndex[14] = -1;
		ball.m_eventPreDataIndex[11] = -1;

		unsigned char groundType = ball.m_groundType;

		if (Bar().m_accuracyGrade == 1) {
			ball.m_spin = 0.0f;
			ball.m_curve = 0.0f;
		} else {
			if (IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK)
				GolfClub().SetApproachMode();

			if (GolfClub().GetApproachMode() == 4) {
				float rangeUp = CSharedDoc::Instance()->GetAuxPartProperty(GOLFDOC()->m_myPlayerIndex, 0) * 0.5f;
				if (IsLocalContent((localContentType_t)0x70)) {
					float buff = (float)CSharedDoc::Instance()->GetBuffValue(GOLFDOC()->m_myPlayerIndex, 0x10);
					if (GolfClub().PowerShotType())
						buff += CSharedDoc::Instance()->GetBuffValue(GOLFDOC()->m_myPlayerIndex, 0x11);

					rangeUp += buff; // DRAFT: tail-merged with :7243
				} else if (IsLocalContent((localContentType_t)0x51)) {
					CCardManager::Instance()->SetPlayerIndex(GOLFDOC()->m_myPlayerIndex);
					float cardRangeUp = CCardManager::Instance()->GetCardRangeUp();
					if (GolfClub().PowerShotType())
						cardRangeUp += CCardManager::Instance()->GetCardPowerRangeUp();

					rangeUp += cardRangeUp;
				}

				// DRAFT: the (int) picks WCrypticValue<int>::operator int (inline
				// xor/not, then fild); the factor is 0x3BC0C0C1 = 1/170.
				ball.m_spin = (Max(0.0f, ((int)PLAYER(GOLFDOC()->m_myPlayerIndex)->m_power + rangeUp) * (1.0f / 170.0f)) + 1.5f) * persent * persent;
			} else {
				ball.m_spin = 0.0f;
			}

			ball.m_curve = persent * ball.m_lieSlope;
		}

		while (ball.m_state != 0) {
			CGolfBall::sExpected *point = ball.NewPreData();
			// DRAFT: the three clears sit under :7264 with the PreProcess call.
			point->m_sound[0] = '\0';
			point->m_groundType = 0;
			point->m_groundSubType = 0;
			GetPVS().PreProcess(0.02f, true);
			GetPVS().BallProcess(0.02f);

			if (GolfClub().GetApproachMode() == 4) {
				if (ball.m_flags & 0x20) {
					if (ball.m_eventPreDataIndex[9] < 0)
						ball.m_pos.y = Max(ball.m_pos.y, GetPVS().GetBasePosY(ball.m_pos, ball.GetRadius() * 2.0f));
					else
						ball.m_spin -= 0.02f;
				} else if (ball.m_flags & 0x40) {
					if (ball.m_eventPreDataIndex[12] < 0)
						ball.m_pos.y = Max(ball.m_pos.y, GetPVS().GetBasePosY(ball.m_pos, ball.GetRadius() * 2.0f));
					else
						ball.m_spin -= 0.02f;
				} else {
					if (ball.m_eventPreDataIndex[0] < 0 && GetPVS().GetObjColNum() == 0)
						ball.m_spin -= (0.5f - ball.m_lieAngle * 0.1f) * 0.02f;
					else
						ball.m_spin -= 0.002f;
				}
			}

			if (ball.m_preDataCount >= 3000 || (ball.m_preDataCount >= 1500 && (ball.m_velocity.Magnitude() < 0.1f || GetPVS().GetObjColNum() >= 10 || GetPVS().GetMeshIndex(ball.m_pos, false) == -1))) {
				ball.m_velocity.Reset(); // DRAFT: stores z, y, x under :7296
				ball.m_pos = GetPVS().GetGroundPoint(ball.m_pos, false, NULL, false);
				ball.m_pos.y += ball.GetRadius(); // DRAFT: loads m_radius first, then adds m_pos.y

				if (IsMyTurn(false) && ball.m_state == 4 && CSharedDoc::Instance()->m_playingMode != REPLAY_MODE_PLAYBACK)
					CollectBallStopData();

				ball.m_state = 0;
			}

			point->m_pos = ball.m_pos;
			point->m_velocity = ball.m_velocity;
			point->m_rotation.x = ball.m_rotation.x; point->m_rotation.y = ball.m_rotation.y; // DRAFT: both on :7309
			point->m_contactType = ball.m_state;

			if (ball.m_state == 0)
				ball.m_flags &= ~0x70;

			if (ball.m_state == 1 && ball.m_velocity.y < 0.0f && ball.m_eventPreDataIndex[1] < 0)
				ball.m_eventPreDataIndex[1] = ball.m_preDataCount;

			if (ball.m_eventPreDataIndex[10] < 0 && (ball.m_powerFlags & 0x10) && !(ball.m_flags & 0x10))
				ball.m_eventPreDataIndex[10] = ball.m_preDataCount;

			if (ball.m_flags & 0x20) {
				if (ball.m_preDataCount == 1) {
					ball.m_velocity.y = 0.0f;
					ball.m_velocity = power * ball.m_velocity.Normalize();
				}

				// DRAFT: PreData(0) is inlined here and at :7367 (NULL test + &m_preData[0]), called elsewhere.
				WVector delta = ball.m_pos - ball.PreData(0)->m_pos;
				delta.y = 0.0f;
				if (ball.m_eventPreDataIndex[9] < 0 && delta.Magnitude() > (GolfClub().GetRange(NULL) * powerRate - 100.0f) * 3.2f) {
					ball.m_eventPreDataIndex[9] = ball.m_preDataCount;
					ball.m_velocity.Normalize();

					switch (GolfClub().GetID()) {
					case 0:
						ball.m_velocity *= 74.0f;
						break;
					case 1:
						ball.m_velocity *= 76.0f;
						break;
					case 2:
						ball.m_velocity *= 80.0f;
						break;
					}

					ball.m_velocity *= powerRate;
					ball.m_spin = 2.5f;
				}
			}

			if (ball.m_eventPreDataIndex[14] < 0 && (ball.m_powerFlags & 0x40) && !(ball.m_flags & 0x40) && ball.m_eventPreDataIndex[13] > 0)
				ball.m_eventPreDataIndex[14] = ball.m_preDataCount;

			if (ball.m_flags & 0x40) {
				if (ball.m_preDataCount == 1) {
					ball.m_velocity.y = 0.0f;
					ball.m_velocity = power * ball.m_velocity.Normalize();
				}

				WVector delta = ball.m_pos - ball.PreData(0)->m_pos;
				delta.y = 0.0f;
				if (ball.m_eventPreDataIndex[12] < 0 && delta.Magnitude() > 0.0f) {
					ball.m_eventPreDataIndex[12] = ball.m_preDataCount;
					ball.m_velocity.Normalize();

					ball.m_velocity *= 72.5f;

					ball.m_velocity *= powerRate * 2.0f;
					ball.m_spin = 3.1f;
				}

				if (ball.m_eventPreDataIndex[1] >= 0 && ball.m_preDataCount > ball.m_eventPreDataIndex[1] + 60 && ball.m_eventPreDataIndex[13] < 0) {
					ball.m_eventPreDataIndex[13] = ball.m_preDataCount;

					// DRAFT: each case is windows.h min(power, limit) spelled out
					// (the power is read twice when under the limit). The
					// original's constants are c12ccccc and c124cccc, one ULP
					// short of -10.8f/-10.3f, hence -10.799999f/-10.299999f; the
					// factors are 43/224, 41/210 and 0.202 as float literals.
					switch (GolfClub().GetID()) {
					case 0:
						ball.m_velocity.y = -8.0f - (344.0f - (GolfClub().GetPower() * powerRate < 344.0f ? GolfClub().GetPower() * powerRate : 344.0f)) * 0.19196428f;
						break;
					case 1:
						ball.m_velocity.y = -10.799999f - (306.0f - (GolfClub().GetPower() * powerRate < 306.0f ? GolfClub().GetPower() * powerRate : 306.0f)) * 0.1952381f;
						break;
					case 2:
						ball.m_velocity.y = -10.299999f - (273.0f - (GolfClub().GetPower() * powerRate < 273.0f ? GolfClub().GetPower() * powerRate : 273.0f)) * 0.202f;
						break;
					}

					ball.m_velocity *= powerRate * 7.0f;
					ball.m_spin = ball.m_lieAngle;
				}
			}

			if (point->m_groundType == 6 && ball.m_eventPreDataIndex[11] == -1)
				ball.m_eventPreDataIndex[11] = ball.m_preDataCount;
		}

		ball.m_groundType = groundType;
		ball.m_flags = ball.m_powerFlags;

		if (OnlinePlay()) {
			if (IsMassGame() && CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_GAME) {
				if (GolfClub().GetType() != 3 && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_uid == CSharedDoc::Instance()->m_userInfo.guid && GolfBall().m_groundType != 2) {
					// DRAFT: :7452 reads IsHoleIn alone and :7456 tests it (its
					// own mark on the jne): a local, threaded into the store.
					int holeIn = GolfBall().IsHoleIn();

					if (!holeIn) {
						// DRAFT: toCup is assigned (temporary + operator=), not
						// constructed in place, so it is declared apart; it
						// shares [ebp-0x2D8] with the loop's second delta.
						WVector toCup;

						toCup = GetHoleCupPos() - GolfBall().m_pos;
						toCup.y = 0.0f;
						float dist = toCup.Magnitude();

						if (dist < 15.0f)
							holeIn = 1;
					}

					if (holeIn)
						m_flagCutin = true;
				}

				if (CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK)
					m_flagCutin = false;

				int cutinMode = CSharedDoc::Instance()->m_gameMode;
				if (GOLFDOC()->m_gameState < 15)
					cutinMode = 6;

				if (m_flagCutin) {
					if (CSharedDoc::Instance()->m_mySkinList.size() > 0) {
						for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_mySkinList.begin(); it != CSharedDoc::Instance()->m_mySkinList.end(); ++it) {
							if (S5::ISCutinItem(it->m_typeId)) {
								IFF_STRUCT::S5::sCutinInformation *info = CSharedDoc::Instance()->m_itemManager.FindCutinInformation(it->m_typeId);

								if (info && (info->m_condition & 8)) {
									SendMsg(this, "Cutin", 0x78, 1, 0, 0, 0);

									S5::UTIL::SendCutInPacket(CSharedDoc::Instance()->m_myUID, 8, cutinMode, CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_characterInfo.typeId);
									break; // DRAFT: its jmp carries :7530
								}
							}

							m_flagCutin = false;
						}
					} else {
						m_flagCutin = false;
					}
				}
			}
		}

		ball.DbgTraceOutput(GolfClub().GetName(), GolfClub().GetRange(NULL));

		SendMsg(this, "NPC", 0x186, 0, 0, 0, 0);

		if (GetPVS().GetObjColNum() > 0)
			ball.m_eventPreDataIndex[2] = -1;

		GetPVS().SetFrame(m_shotStartFrame - 1);
		GetPVS().PreProcess(0.02f, true);

		// DRAFT: the m_curve/m_spin stores sit under :7553 after the m_landed
		// test; :7548-7552 hold only the fild of :7551.
		ball.m_moveScaleBase = ball.m_preDataCount * 0.02f;
		ball.m_curve = ball.m_lieSlope;
		ball.m_spin = ball.m_lieAngle;
		if (!ball.m_landed)
			ball.m_landed = GOLFDOC()->m_pPolySoup->IsOutOfBound(ball.m_pos);

		if (CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_GAME)
			AutoReplay();

		SendMsg(this, "Player", 0x115, (intptr_t)ball.PreData(ball.m_preDataCount), 0, 0, 0);

		m_backUp.m_ballPos = PLAYER(0)->m_ballPos;
		m_backUp.m_yaw = yaw;
		m_backUp.m_landed = ball.m_landed;
		m_backUp.m_powerShotLevel = PLAYER(0)->m_powerShotLevel;
		m_backUp.m_comboGauge = PLAYER(0)->m_comboGauge;

		m_backUp.m_ghostAward = PLAYER(0)->m_ghostAward;
		m_backUp.m_extraPang = PLAYER(0)->m_extraPang;
		m_backUp.m_holeRecord = PLAYER(0)->m_holeRecord[GOLFDOC()->m_currentHole - 1];

		m_backUp.m_holeScore = PLAYER(0)->m_holes[GOLFDOC()->m_currentHole - 1];
		m_backUp.m_rivalLevel = PLAYER(0)->m_rivalLevel;
		strcpy(m_backUp.m_clubName, GolfClub().GetName());

		m_backUps.push_back(m_backUp);

		if (ball.m_landed) {
			// DRAFT: the original passes the PreData pointer straight to
			// WVector::operator= (no copy of the lvalue arm).
			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos = ball.m_lie ? GetPVS().GetPenaltyPos() : ball.PreData(0)->m_pos;
			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel = 0;
		} else if (GolfBall().IsHoleIn() || GolfBall().m_bounceCount > 0) {
			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos = ball.PreData(ball.m_preDataCount)->m_pos;

			unsigned char ground = GetPVS().GetGroundType(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos, true, NULL);

			if (ground <= 7 || ground == 11)
				PLAYER(GOLFDOC()->m_myPlayerIndex)->m_lastShotPos = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos;
		} else {
			GOLFDOC()->m_pAutoTarget->CalcDropPos(ball.PreData(ball.m_preDataCount)->m_pos);
			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos = GOLFDOC()->m_pAutoTarget->GetDropPos(); // DRAFT: the operator= call is tail-merged with :7587
		}

		if (!ball.m_landed && GetPVS().IsOverWater(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos)) {
			ball.m_landed = true;
			ball.m_lie = 0;

			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos = ball.PreData(0)->m_pos;
			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel = 0;
		}

		if (CSharedDoc::Instance()->m_roomType == 10)
			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos = ball.PreData(ball.m_preDataCount)->m_pos;

		if (CSharedDoc::Instance()->m_roomType == 1) {
			if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_partnerIndex != 0xFF) {
				PLAYER(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_partnerIndex)->m_ballPos = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos;
				PLAYER(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_partnerIndex)->m_powerShotLevel = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel;
			}

			TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_ballPos = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos;
			TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_powerShotLevel = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel;
		}

		if (GOLFDOC()->m_gameState == 15 || GOLFDOC()->m_gameState == 8)
			SendMsg(this, "Screen", 0x1B4, 0, 0, 0, 0);

		m_forgetPending = false;
		m_forgetElapsed = 0.0f;

		if (ball.m_landed && OnlinePlay() && IsMassGame() && CSharedDoc::Instance()->m_roomType != 10 && CanCancelShot())
			m_forgetPending = true;

		if (!m_forgetPending)
			SendShotResult();

		ball.m_pos = expected->m_pos;
		ball.m_velocity = expected->m_velocity;
		ball.m_groundArea = GetPVS().GetMeshIndex(ball.m_pos, false);

		// DRAFT: :7665-7679 carry no marks (the PDB gives the stat increments
		// to :8392); the increment is tail-merged between the two arms.
		if (GolfClub().GetType() == 3) {
			if (ball.m_groundType == 2)
				PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holeIn[GOLFDOC()->m_currentHole - 1] += 1;

			CSharedDoc::Instance()->m_gameStatistics[GOLFDOC()->m_myPlayerIndex].puttAttempts++;
		} else {
			CSharedDoc::Instance()->m_gameStatistics[GOLFDOC()->m_myPlayerIndex].shots++;
		}

		if (GolfClub().GetType() != 3) {
			int gaugeUp = 0;
			float auxPower = CSharedDoc::Instance()->GetAuxPartProperty(GOLFDOC()->m_myPlayerIndex, 4);

			switch (Bar().m_accuracyGrade) {
			case 4: {
				bool itemBonus = false;
				if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x1800000A || PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000007 || PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000002 || PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000012)
					itemBonus = true;

				if (IsLocalContent((localContentType_t)0x4B) && auxPower == 11.0f) {
					bool spinShot = false;
					if ((GolfBall().m_powerFlags & 3) || (GolfBall().m_powerFlags & 0xC))
						spinShot = true;

					gaugeUp = GetShotMotionGrade(true, GolfClub().PowerShotType() != 0, spinShot, itemBonus);
				} else if (!GolfClub().PowerShotType() && !itemBonus) {
					// DRAFT: :7726 and :7730 are the same expression, tail-merged.
					if (GolfBall().m_powerFlags & 3)
						gaugeUp = (auxPower == 11.0f ? 1 : 0) + 14;
					else if (GolfBall().m_powerFlags & 0xC)
						gaugeUp = (auxPower == 11.0f ? 1 : 0) + 14;
					else
						gaugeUp = (auxPower == 11.0f ? 1 : 0) + 12;
				}

				if (!GolfClub().PowerShotType() && !itemBonus) {
					if (IsLocalContent((localContentType_t)0x51)) {
						CCardManager::Instance()->SetPlayerIndex(GOLFDOC()->m_myPlayerIndex);
						gaugeUp += CCardManager::Instance()->GetCardComboGaugeUp();
					}

					if (!IsMassGame() && CSharedDoc::Instance()->m_roomType != 3) {
						unsigned char level = 0;
						if (CSharedDoc::Instance()->m_roomType == 0 || CSharedDoc::Instance()->m_roomType == 7) {
							if (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_statistics.level < 6)
								level = 1;
							else
								level = (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_statistics.level - 1) / 5 + 1;
						} else if (CSharedDoc::Instance()->m_roomType == 1) {
							if (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_statistics.level < 6)
								level = 1;
							else
								level = (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_statistics.level - 1) / 5 + 1;

							if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_partnerIndex != 0xFF) {
								if (CSharedDoc::Instance()->m_roomPlayers[PLAYER(GOLFDOC()->m_myPlayerIndex)->m_partnerIndex].m_statistics.level < 6)
									level += 1;
								else
									level += (CSharedDoc::Instance()->m_roomPlayers[PLAYER(GOLFDOC()->m_myPlayerIndex)->m_partnerIndex].m_statistics.level - 1) / 5 + 1;

								level >>= 1; // DRAFT: shr byte ptr [mem], 1; `level /= 2` may also give it
							}
						}

						if (level < GOLFDOC()->m_maxGrade)
							gaugeUp += (GOLFDOC()->m_maxGrade - level) * 2;
					}
				}
				break;
			}
			case 3: {
				bool itemBonus = false;
				if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x1800000A || PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000007 || PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000002 || PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000012)
					itemBonus = true;

				if (IsLocalContent((localContentType_t)0x4B) && auxPower == 11.0f)
					gaugeUp = GetShotMotionGrade(false, GolfClub().PowerShotType() != 0, false, itemBonus);
				else if (!GolfClub().PowerShotType())
					gaugeUp = (auxPower == 11.0f ? 3 : 0) + 4;
				break;
			}
			case 2:
				if (!GolfClub().PowerShotType())
					gaugeUp = -4;
				break;
			case 0:
			case 1:
				if (!GolfClub().PowerShotType())
					gaugeUp = -17;
				break;
			}

			if ((CSharedDoc::Instance()->m_roomType == 3 || CSharedDoc::Instance()->m_roomType == 7) && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] > GOLFDOC()->GetHoleData().m_par && gaugeUp > 0)
				gaugeUp = 0;

			if (GolfClub().PowerShotType() && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId != 0x18000004 && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId != 0x18000010 && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId != 0x18000022 && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId != 0x18000027) {
				if (IsLocalContent((localContentType_t)0x31)) {
					if (GolfClub().PowerShotType() == 1)
						gaugeUp -= 33;
					else if (GolfClub().PowerShotType() == 2 || GolfClub().PowerShotType() == 3)
						gaugeUp -= 66; // DRAFT: tail-merged with :7866
				} else {
					if (GolfClub().PowerShotType() == 1)
						gaugeUp -= 33;
					else if (GolfClub().PowerShotType() == 2)
						gaugeUp -= 66;
				}
			}

			if (CSharedDoc::Instance()->m_currentChannel.flags & 8)
				gaugeUp = 0;

			if ((GOLFDOC()->m_gameState == 15 || GOLFDOC()->m_gameState == 3) && gaugeUp != 0) {
				float gaugeAdd;
				if (gaugeUp > 0)
					gaugeAdd = CSharedDoc::Instance()->GetAuxPartProperty(GOLFDOC()->m_myPlayerIndex, 2) * gaugeUp;
				else
					gaugeAdd = (float)gaugeUp;

				if (CSharedDoc::Instance()->m_roomType == 1)
					TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_teamRate = Between(0.0f, TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_teamRate + gaugeAdd, CSharedDoc::Instance()->GetComboGaugeLimit(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute));
				else
					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_comboGauge = Between(0.0f, PLAYER(GOLFDOC()->m_myPlayerIndex)->m_comboGauge + gaugeAdd, CSharedDoc::Instance()->GetComboGaugeLimit(GOLFDOC()->m_myPlayerIndex));
			}
		}

		m_nextTurnPlayer = GetNextTurn();
		// DRAFT: :7902-7909 carry no marks; the stores sit under :7910.
		m_goStopSent = false;
		m_flag267 = true;
		m_flag3F0 = false;

		if (!(m_replayFlags & 2)) {
			ReactionForShot();
			CCameraWork::Instance()->SetReplayCount(0);

			// DRAFT: the two SelectCamera calls are tail-merged into :7926.
			if (IsLocalContent((localContentType_t)0x31)) {
				if ((GolfClub().PowerShotType() != 2 && GolfClub().PowerShotType() != 3) || GolfClub().GetType() != 0)
					CCameraWork::Instance()->SelectCamera(-1);
			} else {
				if (GolfClub().PowerShotType() != 2 || GolfClub().GetType() != 0)
					CCameraWork::Instance()->SelectCamera(-1);
			}
		}

		CMouseCursor::Instance()->SetActive(true);

		if (CSharedDoc::Instance()->m_roomType == 10)
			SendMsg(this, "Screen", 0x1A5, 1, 0, 0, 0);
		break;
	}
	case 0x80: {
		// DRAFT: 7947 holds only `xor edi,edi`; the two stores below sit inside
		// the 7951 mark (scheduled). They are probably lines 7948-7950.
		m_cameraArea = NULL;
		m_flag5B1 = false;
		SendMsg(this, "Effect", 0x17F, 0, 0, 0, 0);

		if (m_replayMode == 0) {
			SendMsg(this, "Screen", 0x1A8, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x1A4, 0, 0, 0, 0);
			SendMsg(this, "Spin", 0x164, 0, 0, 0, 0);
		}

		GOLFDOC()->m_commandArrows.clear();

		const CGolfBall::sExpected *pStart = ball.PreData(0);
		// DRAFT: the four stores share the 7967 mark (lines 7963-7967 scheduled
		// together). The original order of the stores is m_preDataIndex, m_state,
		// m_rotation.x, then the call.
		ball.m_preDataIndex = 0;
		ball.m_state = pStart->m_contactType;
		ball.m_rotation.x = 0.0f;
		ball.m_rotation.y = CalcDeltaAngle(WVector::UNIT_POS_Z, ball.m_velocity);

		// DRAFT: the m_gameOverDelay store sits inside the 7973 mark (between the
		// push of m_flightFrame and the GetPVS call): a separate line just before.
		m_gameOverDelay = 0;
		GetPVS().SetFrame(m_flightFrame);

		if (ball.m_eventPreDataIndex[7] > 0)
			SendMsg(this, "Ball", 0x1EC, 0, 0, 0, 0);

		if (!(m_replayFlags & 2) || m_replayMode == 2) {
			SendMsg(this, "Screen", 0x1A5, 1, 0, 0, 0);

			if (!g_audio->IsPlaying("shot_dist_count"))
				g_audio->PlaySfx("shot_dist_count", 0, 1, "shot_dist_count", 0, 0.5f, 200.0f);
		}

		// DRAFT: the original loads m_state once (eax) and tests 1 (taken to 7993),
		// then 3 (7989), then jumps to the end of the switch with a 7990 mark on
		// the jmp.
		if (ball.m_state != 1) {
			if (ball.m_state == 3)
				SendMsg(this, "Trajectory", 0x15D, 0, 0, 0, 0);
			break;
		}

		float power = (Bar().m_power - 140.0f) / data_00af1c38;

		unsigned char ground = GetPVS().GetGroundType(ball.m_pos, false, NULL);
		if (GolfClub().GetType() != 3 && power > 0.8f && ground != 1 && ground != 6) {
			// DRAFT: 0xA7DF68 is an int initialised to -1 in .data with no symbol and
			// no reference outside this function: a function-local static. The file
			// convention for symbol-less data, `extern "C" { static int
			// data_00a7df68 = -1; }` at file scope, would also fit.
			static int s_shotImpactFx = -1;

			if (CFx::Instance()->FindControl(s_shotImpactFx) == NULL)
				s_shotImpactFx = CFx::Instance()->MakeShotImpact(ball);
		}

		CFx::Instance()->MakeClubImpact(ball, CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId);

		WVector flight = ball.PreData(ball.m_preDataCount)->m_pos - pStart->m_pos;

		m_flySound[0] = '\0';

		// DRAFT: the store flight.y = 0 ([ebp-2B8h]) sits inside the 8040 mark,
		// between the m_myPlayerIndex load and the compare: probably its own line
		// somewhere in 8011-8039 (the rest of that range has no code).
		flight.y = 0.0f;
		// DRAFT: the original places the "then" branch (8042-8043) after the else
		// chain (8047-8052).
		if (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId == 0x14000005 || CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId == 0x14000080) {
			if (flight.Magnitude() > 320.0f)
				strcpy(m_flySound, "wb_fly");
		} else {
			if (flight.Magnitude() > 640.0f)
				// "ball flying 1"
				strcpy(m_flySound, "\xB0\xF8\xB3\xAF\xBE\xC6\xB0\xA1\xB1\xE2" "1");
			else if (flight.Magnitude() > 480.0f)
				// "ball flying 2"
				strcpy(m_flySound, "\xB0\xF8\xB3\xAF\xBE\xC6\xB0\xA1\xB1\xE2" "2");
			else if (flight.Magnitude() > 320.0f)
				// "ball flying 3"
				strcpy(m_flySound, "\xB0\xF8\xB3\xAF\xBE\xC6\xB0\xA1\xB1\xE2" "3");
		}

		if (m_flySound[0] != '\0') {
			g_audio->StopSfx(m_flySound);
			g_audio->PlaySfx(m_flySound, 0, 0, m_flySound, 0, 0.5f, 200.0f);
		}

		// DRAFT: the original stores the whole dword (mov [ebp-29Ch],ecx) but
		// compares one byte (cmp byte ptr [ebp-29Ch],2): unsigned char as
		// DoFlyBall writes it. Check the store.
		unsigned char powerShot = GolfClub().PowerShotType();
		// DRAFT: the movzx of m_type carries a golfrule.cpp:8392 mark (an artifact;
		// the switch itself is 8062).
		switch (GolfClub().GetType()) {
		case 0:
		case 1:
			if (power > 0.8f) {
				if (ball.m_powerFlags & 0x70) {
					// The two PlaySound calls are tail-merged (8089 jumps into 8081's).
					if (GolfClub().PowerShotType() == 2 || GolfClub().PowerShotType() == 3) {
						CVoiceItem::Instance()->PlaySound(CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId, (CVoiceItem::eWaveState)8, -1);
					} else if (m_replayMode == 2 || IsMassGame() || GetCurMap() == 7) {
						CVoiceItem::Instance()->PlaySound(CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId, (CVoiceItem::eWaveState)4, -1);
					}

					// m_powerFlags is loaded once (eax) for the three tests.
					if (ball.m_powerFlags & 0x10) {
						bool bVariant = powerShot == 2 || powerShot == 3;
						CFx::Instance()->MakeTomahawk(ball, bVariant);
					} else if (ball.m_powerFlags & 0x20) {
						CFx::Instance()->MakeCobra(ball);

						if (powerShot == 2 || powerShot == 3)
							AfxGetTask()->GetActor("Trajectory") << MsgObject(0, 0x15E, 0, 0, 0, 0, 0);
					} else if (ball.m_powerFlags & 0x40) {
						bool bVariant = powerShot == 2 || powerShot == 3;
						CFx::Instance()->MakeSpike(ball, bVariant);
					}
				} else {
					if (Bar().m_accuracyGrade == 4) {
						if (GolfClub().PowerShotType() != 0) {
							SendMsg(this, "Trajectory", 0x158, 0, 0, 0, 0);
							SendMsg(this, "Effect", 0x181, 0, 0, 0, 0);
							// "comet"
							g_audio->StopSfx("\xC7\xFD\xBC\xBA");
							// "flying_comet", "comet"
							g_audio->PlaySfx("\xB3\xAF\xBE\xC6\xB0\xA1\xB1\xE2_\xC7\xFD\xBC\xBA", 0, 0, "\xC7\xFD\xBC\xBA", 0, 0.5f, 200.0f);
						} else {
							if (GOLFDOC()->m_gameState > 9 && GOLFDOC()->m_gameState < 15) {
								SendMsg(this, "Trajectory", 0x159, 0, 0, 0, 0);
							} else {
								SendMsg(this, "Effect", 0x180, 0, 0, 0, 0);
								// "stardust"
								g_audio->StopSfx("\xBA\xB0\xB0\xA1\xB7\xE7");
								// "flying_stardust", "stardust"
								g_audio->PlaySfx("\xB3\xAF\xBE\xC6\xB0\xA1\xB1\xE2_\xBA\xB0\xB0\xA1\xB7\xE7", 0, 0, "\xBA\xB0\xB0\xA1\xB7\xE7", 0, 0.5f, 200.0f);
							}
						}

						if (!(m_replayFlags & 2) || m_replayMode == 2) {
							char text[64] = { 0 };
							// DRAFT: the 2.0f store sits inside the 8151 mark (between
							// the stosw and stosb of 8148's zeroing): its own line
							// 8149-8150.
							float scale = 2.0f;
							// "Pangya%02d"
							sprintf(text, "\xC6\xCE\xBE\xDF%02d", CSharedDoc::Instance()->GetIndexPangyaLogoImage());
							SendMsg(this, "Letter", 0x18D, (intptr_t)text, (intptr_t)&scale, 0, 0);
							SendMsg(this, "Gallery", 0xFF, 0, 0, 0, 0);
							CSoundItemManager::Instance()->PlayCurrentSoundItem();
						}
					} else {
						// The four zero pushes and ecx = this of both SendMsg calls
						// are hoisted above the compare.
						if (Wind().GetGlobalWind().Magnitude() > 4.0f) {
							SendMsg(this, "Trajectory", 0x15B, 0, 0, 0, 0);
							// "wind"
							g_audio->StopSfx("\xB9\xD9\xB6\xF7");
							// "flying_wind", "wind"
							g_audio->PlaySfx("\xB3\xAF\xBE\xC6\xB0\xA1\xB1\xE2_\xB9\xD9\xB6\xF7", 0, 0, "\xB9\xD9\xB6\xF7", 0, 0.5f, 200.0f);
						} else {
							SendMsg(this, "Trajectory", 0x159, 0, 0, 0, 0);
						}
					}

					if (GolfClub().PowerShotType() != 0) {
						if (GolfClub().PowerShotType() == 1)
							SendMsg(this, "Effect", 0x16C, 0, 0, 0, 0);
						else if (powerShot == 2 || powerShot == 3)
							CFx::Instance()->MakePowerShot2(ball);
						CCameraWork::Instance()->Quake();
						// "power shot_effect"
						g_audio->PlaySfx("\xC6\xC4\xBF\xF6\xBC\xA6_\xC0\xCC\xC6\xE5\xC6\xAE", 0, 0, 0, 0, 0.5f, 200.0f);
					}
				}

				if (Bar().m_accuracyGrade > 2)
					SendMsg(this, "Effect", 0x16E, 0, 0, 0, 0);
			} else if (power > 0.2f) {
				SendMsg(this, "Trajectory", 0x159, 0, 0, 0, 0);
			}

			SendMsg(this, "Ball", 0x1EF, 0, 0, 0, 0);
			break;
		case 2:
			if (power > 0.8f) {
				if (ball.m_powerFlags & 0x10) {
					bool bVariant = powerShot == 2 || powerShot == 3;
					// DRAFT: lines 8213-8232 hold no code (commented out?).
					CFx::Instance()->MakeTomahawk(ball, bVariant);
				// DRAFT: the 8235 jmp goes straight to 8291, skipping 8271: so 8271
				// is inside this else (or 8235 is a `break;` and 8271 follows the
				// if/else). Same code either way.
				} else {
					if (Bar().m_accuracyGrade == 4) {
						if (!(m_replayFlags & 2) || m_replayMode == 2) {
							char text[64] = { 0 };
							// DRAFT: as at 8151, the 2.0f store sits inside the 8249 mark.
							float scale = 2.0f;
							// "Pangya%02d"
							sprintf(text, "\xC6\xCE\xBE\xDF%02d", CSharedDoc::Instance()->GetIndexPangyaLogoImage());
							SendMsg(this, "Letter", 0x18D, (intptr_t)text, (intptr_t)&scale, 0, 0);
							SendMsg(this, "Gallery", 0xFF, 0, 0, 0, 0);
							CSoundItemManager::Instance()->PlayCurrentSoundItem();
						}
					} else {
						SendMsg(this, "Trajectory", 0x159, 0, 0, 0, 0);
					}

					if (Bar().m_accuracyGrade > 2)
						SendMsg(this, "Effect", 0x16E, 0, 0, 0, 0);
				}
			} else if (power > 0.2f) {
				SendMsg(this, "Trajectory", 0x159, 0, 0, 0, 0);
			}
			break;
		case 3:
			SendMsg(this, "Trajectory", 0x15D, 0, 0, 0, 0);
			break;
		}

		if ((ball.m_powerFlags & 3) && ball.m_eventPreDataIndex[6] > 0)
			SendMsg(this, "Trajectory", 0x15A, 0, 0, 0, 0);
		break;
	}
	case 0x2000: {
		if (IsLocalContent((localContentType_t)0x4F))
			CSharedDoc::Instance()->m_replayPlayType = 0;

		ResetRank();

		// DRAFT: these two stores are scheduled inside line 8306's pushes; they stand on their own lines (8302-8305).
		m_flag267 = false;
		m_shotBoardNextHole = false;

		SetConstantBGM("bgm_scoreboard.mp3", false, true);
		g_audio->StopSfx(NULL);

		if (!GOLFDOC()->m_netGameEnd)
			DispatchMatchItems();

		SendMsg(this, "Score", 0x12F, 1, 0, 0, 0);

		SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);

		if (!OnlinePlay() || IsMassGame())
			// DRAFT: GetEndHole() is called before CSharedDoc/GOLFDOC load (`cmp [doc+168h],al`); if the order
			// differs, try `GOLFDOC()->m_currentHole == GetEndHole()`.
			CSharedDoc::Instance()->m_gameOver = GetEndHole() == GOLFDOC()->m_currentHole;

		// DRAFT: the count's `= 0` store is scheduled inside line 8330; its declaration line is not marked.
		int count = 0;
		GetActor(s_gatewayActorName) << MsgObject(0, 0x26F, 6, 0, 0, 0, 0);
		GetActor(s_gatewayActorName) << MsgObject(0, 0x26F, 4, (intptr_t)&count, 0, 0, 0);
		if (count <= 0)
			GetActor(s_gatewayActorName) << MsgObject(0, 0x26F, 3, 0, 0, 0, 0);

		// DRAFT: line 8366 is only `lea ecx,[ebp-49Ch]` then (8372) a jmp into case 0x800000's
		// tail at 0x45bb37 (golfrule.cpp:8674): the same "Screen" 0xA0 statement, tail-merged.
		// Here it is unconditional (no IsLocalContent(0x30) test).
		AfxGetTask()->GetActor("Screen") << MsgObject(0, 0xA0, 0, 0, 0, 0, 0);
		break;
	}
	case 0x400000:
		if (!m_waitingForStart) {
			WVector pos = GetTeeShotPos();

			pos.y = GetPVS().GetGroundPoint(pos, false, NULL, false).y + 100.0f;

			WVector dir = GetHoleCupPos() - pos;
			dir.Normalize();

			g_camera = MakeCameraMatrix(pos, dir);
		}
		break;
	case 0x1000:
		if (IsLocalContent((localContentType_t)0x4F)) {
			if (CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_GAME) {
				CSharedDoc::Instance()->m_replayPlayType = 4;
			} else if (CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK && CSharedDoc::Instance()->m_replayState != 9 && CSharedDoc::Instance()->m_replayState != 7) {
				CSharedDoc::Instance()->m_replayStartB += 1;
				CSharedDoc::Instance()->m_replayState = 0;
			}
		}

		// DRAFT: these three stores are scheduled inside line 8415's pushes (lines 8408-8414).
		m_replaying = false;
		m_flag266 = false;
		m_canFastForward = false;

		SendMsg(this, "Letter", 0x18C, 0, 0, 0, 0);
		if (!IsLocalContent((localContentType_t)0x4F) || CSharedDoc::Instance()->m_playingMode != REPLAY_MODE_PLAYBACK)
			SendMsg(this, "Ball", 0x164, 0, 0, 0, 0);

		SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1A8, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1A5, 0, 0, 0, 0);
		SendMsg(this, "Effect", 0x177, 0, 0, 0, 0);
		SendMsg(this, "Trajectory", 0x18C, 0, 0, 0, 0);
		CFx::Instance()->ClearAll(true);

		if (CSharedDoc::Instance()->m_roomType == 1) {
			if (!SetHoleInPose_Team()) {
				ChangeGameMode(0x800);
				return;
			}
		} else if (CSharedDoc::Instance()->m_roomType == 3) {
			if (!SetHoleInPose_Match()) {
				ChangeGameMode(0x800);
				return;
			}
		} else if (CSharedDoc::Instance()->m_roomType == 7) {
			if (m_shotBoardNextHole) {
				SetHoleInPose_Skins();
			} else {
				ChangeGameMode(0x800);
				return;
			}
		} else {
			SetHoleInPose(false);
		}

		// 55 degrees in radians.
		g_view->SetFOV(0.95993109f);
		break;
	case 0x4000: {
		if (IsLocalContent((localContentType_t)0x2F)) {
			if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000028)
				SendMsg(this, "Player", 0x203, 0, 0, 0, 0);
		}

		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_playTime += GOLFDOC()->m_shotElapsed;
		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId = 0;

		// DRAFT: one GOLFDOC() load (esi) is shared by both branches and kept across the
		// PLAYER/TEAM/operator+/Max calls (only m_myPlayerIndex is re-read); the
		// common head (GOLFDOC load, index, push -30.0f) is hoisted above the jne.
		if (CSharedDoc::Instance()->m_roomType == 1)
			TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_teamRate = Max<float>(0.0f, TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_teamRate + -30.0f);
		else
			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_comboGauge = Max<float>(0.0f, PLAYER(GOLFDOC()->m_myPlayerIndex)->m_comboGauge + -30.0f);

		// "Time" / "Time over"
		g_audio->StopSfx("\xC5\xB8\xC0\xD3");
		g_audio->PlaySfx("\xC5\xB8\xC0\xD3\xBF\xC0\xB9\xF6", 0, 0, "\xC5\xB8\xC0\xD3\xBF\xC0\xB9\xF6", 0, 0.5f, 200.0f);

		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_numTimeout++;
		CSharedDoc::Instance()->m_gameStatistics[GOLFDOC()->m_myPlayerIndex].timeouts++;

		// DRAFT: the store spot = 0 ([ebp-2ACh]) sits under the 8489 mark, inside
		// SetPowerShot's call sequence; declaration placement may need moving.
		float spot = 0.0f;
		GolfClub().SetPowerShot(0);
		SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1AA, (intptr_t)&spot, 0, 0, 0);
		SendMsg(this, "Clock", 0x271A, 0, 0, 0, 0);
		SendMsg(this, "GuideLine", 0x1A2, 0, 0, 0, 0);

		if (IsMyTurn(true)) {
			UIAlpha() = 0xFF;
			CChatMsg::Instance()->ReplaceBackupUiAlpha();
		}

		m_nextTurnPlayer = GetNextTurn();

		// DRAFT: the original lays the else block (8518-8528) as the fall-through
		// and the then block (8506-8514) after it (both CheckDropOut/CheckGiveUp
		// tests `jne` to 8506). If cl lays it the other way, try
		// `if (!CheckDropOut() && !CheckGiveUp()) { else-body } else { then-body }`.
		if (CheckDropOut() || CheckGiveUp()) {
			RecordGiveUp(GOLFDOC()->m_myPlayerIndex);

			if (!CheckDropOut())
				RecordScoreByPlayer(GOLFDOC()->m_myPlayerIndex);

			if (CSharedDoc::Instance()->m_roomType == 1)
				ChangeGameMode(0x1000);
			else
				ChangeGameMode(0x80000);
		} else {
			// DRAFT: `cmp [golfDoc+0x180], al` with al = CheckGiveUp()'s 0.
			if (!GOLFDOC()->m_approachMode) {
				float letterTime = 1.5f;
				// "Time out"
				SendMsg(this, "Letter", 0x18D, (intptr_t)"\xC5\xB8\xC0\xD3\xBE\xC6\xBF\xF4", (intptr_t)&letterTime, 0, 0);
				SendMsg(this, "Screen", 0x1B9, 0, 0, 0, 0);
			}

			g_camera = GOLFDOC()->GetDefaultCamera();

			// "Time over / bunker / OB despair pose", the caddie's "Bunker"
			SendMsg(this, "Player", 0x2716, (intptr_t)"\xC5\xB8\xC0\xD3\xBF\xC0\xB9\xF6\xBA\xA1\xC4\xBFOB\xBD\xC7\xB8\xC1\xC6\xF7\xC1\xEE", 0, 0, 0);
			SendMsg(this, "Caddie", 0x2716, (intptr_t)"\xBA\xA1\xC4\xBF", 0, 0, 0);
		}
		break;
	}
	case 0x20000: {
		CCapturedBg::Instance()->Reset();

		ResetRank();

		SendMsg(this, "Score", 0x12E, 1, 0, 0, 0);
		SendMsg(this, "GolfBg", 0x1E7, 0, 0, 0, 0);

		if (CSceneManager::Instance())
			SendMsg(this, "GolfBg", 0x142, 0, 0, 0, 0);

		// "Award ceremony camera"
		WPolySoup::sCamera *cam = GOLFDOC()->m_pPolySoup->FindCamera("\xBD\xC3\xBB\xF3\xBD\xC4\xC4\xAB\xB8\xDE\xB6\xF3");
		if (cam) {
			g_camera = cam->m_matrix;
			g_view->SetFOV(cam->m_fov * 0.017453292f);
		} else {
			g_camera.Reset();
			g_view->SetFOV(0.95993108f);
		}

		SendMsg(this, "Score", 0x130, 0, 0, 0, 0);
		SendMsg(this, "Player", 0x119, 0, 0, 0, 0);

		SetConstantBGM("bgm_scoreboard.mp3", true, true);

		// DRAFT: 12-byte inline copy of the literal (3 dword loads from .rdata, the
		// stores tail-merged) = strcpy intrinsic into a stack buffer; the buffer's
		// size is not known (only [ebp-90h]..[ebp-85h] is written): 32 is a guess.
		char cameraName[32];
		if (CSharedDoc::Instance()->m_roomType == 1) {
			if (!OnlinePlay()) {
				// Offline team game: the team with more m_resetCount wins, then more
				// m_teamValue. DRAFT: TEAM(1) is called before TEAM(0) in each test
				// (cl's evaluation order for `TEAM(0)->x > TEAM(1)->x`); line 8579's
				// SetTeamGameWinner(0) is tail-merged into 8573's code.
				if (TEAM(0)->m_resetCount > TEAM(1)->m_resetCount)
					GOLFDOC()->SetTeamGameWinner((eTeamName)0);
				else if (TEAM(0)->m_resetCount < TEAM(1)->m_resetCount)
					GOLFDOC()->SetTeamGameWinner((eTeamName)1);
				else {
					if (TEAM(0)->m_teamValue > TEAM(1)->m_teamValue)
						GOLFDOC()->SetTeamGameWinner((eTeamName)0);
					else if (TEAM(0)->m_teamValue < TEAM(1)->m_teamValue)
						GOLFDOC()->SetTeamGameWinner((eTeamName)1);
				}
			}

			// "Award ceremony_couple"
			strcpy(cameraName, "\xBD\xC3\xBB\xF3\xBD\xC4_\xC4\xBF\xC7\xC3");
		} else {
			// "Award ceremony_individual"
			strcpy(cameraName, "\xBD\xC3\xBB\xF3\xBD\xC4_\xB0\xB3\xC0\xCE");
		}

		m_gameOverDelay = CCameraWork::Instance()->LoadCamera((CCameraWork::eCamMode)1, cameraName, WVector::ZERO);

		if (OnlinePlay() && !IsMassGame() && !CSharedDoc::Instance()->m_value90 && !GalleryMode()) {
			// DRAFT: the `m_value90 = 1` store sits under the 8599 mark with the
			// packet constructor (scheduled); it uses the singleton pointer the
			// condition loaded.
			CSharedDoc::Instance()->m_value90 = 1;
			WSendPacket packet((enumClientPacket)6);
			CSharedDoc::Instance()->m_gameStatistics[MyIndex()].playTime = PLAYER(MyIndex())->m_playTime / 1000;
			packet.EncodeBuffer(&CSharedDoc::Instance()->m_gameStatistics[MyIndex()], sizeof(sPangYaUserStatistics));
			packet.Send(SEND_TO_DEFAULT);
		}
		break;
	}
	case 0x80000:
		g_view->SetFOV(0.95993108f);

		if (CSharedDoc::Instance()->m_roomType != 1) {
			if (GOLFDOC()->m_gameState == 0xF || GOLFDOC()->m_gameState == 8) {
				SendMsg(this, "Screen", 0x1B4, 1, 0, 0, 0);
				SendMsg(this, "Score", 0x132, 0, 0, 0, 0);
			}
		}

		SendMsg(this, "Ball", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1A8, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1A5, 0, 0, 0, 0);
		SendMsg(this, "Effect", 0x177, 0, 0, 0, 0);
		SendMsg(this, "Trajectory", 0x18C, 0, 0, 0, 0);
		CFx::Instance()->ClearAll(true);

		SetHoleInPose(false);

		m_nextTurnPlayer = GetNextTurn();
		break;
	case 0x800000:
		if (IsLocalContent((localContentType_t)0x4F))
			CSharedDoc::Instance()->m_replayPlayType = 0;

		// DRAFT: the m_flag267 store sits under the 8652 mark between `mov ecx,ebx`
		// and the ResetRank call (scheduled, like m_nextTurnPlayer's store under
		// 8504 in case 0x4000); it cannot follow the call in source.
		m_flag267 = false;
		ResetRank();

		SetConstantBGM("bgm_scoreboard.mp3", false, true);

		g_audio->StopSfx(NULL);

		SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Spin", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Player", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Caddie", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Mascot", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Ball", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Bubble", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1A4, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1A5, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1A8, 0, 0, 0, 0);

		// 8672-8677: the tail the 0x800 case's 6926-6927 share (same source text).
		if (IsLocalContent((localContentType_t)0x30))
			AfxGetTask()->GetActor("Screen") << MsgObject(0, 0xA0, 0, 0, 0, 0, 0);
		break;
	}
	(void)oldMode;
	(void)ball;
}

// golfrule.cpp:8682
// A random variant name of a motion: "<name>_01" to "<name>_04".
// Unreferenced in the exe and without a public: an anonymous-namespace function
// (cl 13.10 emits those even when unreferenced, a `static` one it drops).
namespace {
char *__stdcall MakeRandomMotionName(const char *name) {
	return MakeStr("%s_%02d", name, rand() % 4 + 1);
}
}

// golfrule.cpp:8687
// The hole is finished: the player and the caddie take their pose for the
// score (unless noPose: the poses were already shown), the background music
// follows the score against par, the camera of the pose is loaded (a default
// one behind the cup when the pose has none) and the score's letter shows.
//   Player motions: ALB "albatross/hole-in-one win pose", EAGLE/BIRDIE/SAVE
//   (par) "... win pose", FAIL "bogey/fail disappointed pose".
//   Caddie motions: "hole in one", "hole in", "fail".
//   Letters: hole in one, albatross, eagle, birdie, par, bogey, double bogey,
//   triple bogey, "+N", give up.
void CGolfRule::SetHoleInPose(bool noPose) {
	char letter[0x20];
	char caddieMotion[0x20];
	char playerMotion[0x20];

	if (!noPose) {
		const WVector &cup = m_point258;
		float angle = 3.1415927f;
		WVector pos = cup;

		SendMsg(this, "Player", 0x2719, (intptr_t)&cup, 0, 0, 0);
		SendMsg(this, "Player", 0x2718, (intptr_t)&angle, 0, 0, 0);
		SendMsg(this, "Player", 0x114, 0, 0, 0, 0);

		angle = 5.4977875f;
		pos = GetPVS().GetGroundPoint(WVector(cup.x + 10.0f, cup.y, cup.z + 10.0f), false, NULL, false);
		SendMsg(this, "Caddie", 0x1D9, 0, 0, 0, 0);
		SendMsg(this, "Caddie", 0x1E2, (intptr_t)&angle, 0, 0, 0);
		SendMsg(this, "Caddie", 0x2719, (intptr_t)&pos, 0, 0, 0);
	}

	int toPar = (CSharedDoc::Instance()->m_roomType != 1 ? (unsigned char)PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1]
		: (unsigned char)TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1]) - GOLFDOC()->GetHoleData().m_par;

	int score = CSharedDoc::Instance()->m_roomType == 1 ? TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] : PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1];

	if (IsLocalContent((localContentType_t)0x6D)) {
		int charType = CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_characterInfo.typeId & 0x3FFFFFF;
		strcpy(playerMotion, GetCharMotionByTiming(toPar, 0, charType, score));

		if (score == 1) {
			strcpy(caddieMotion, "\xC8\xA6\xC0\xCE\xBF\xF8");
			strcpy(letter, "\xC8\xA6\xC0\xCE\xBF\xF8");
		} else {
			switch (toPar) {
			case -3:
				strcpy(caddieMotion, "\xC8\xA6\xC0\xCE\xBF\xF8");
				strcpy(letter, "\xBE\xCB\xB9\xD9\xC6\xAE\xB7\xCE\xBD\xBA");
				break;
			case -2:
				strcpy(caddieMotion, "\xC8\xA6\xC0\xCE");
				strcpy(letter, "\xC0\xCC\xB1\xDB");
				break;
			case -1:
				strcpy(caddieMotion, "\xC8\xA6\xC0\xCE");
				strcpy(letter, "\xB9\xF6\xB5\xF0");
				break;
			case 0:
				strcpy(caddieMotion, "\xC8\xA6\xC0\xCE");
				strcpy(letter, "\xC6\xC4");
				break;
			case 1:
				strcpy(caddieMotion, "\xBD\xC7\xB0\xDD");
				strcpy(letter, "\xBA\xB8\xB1\xE2");
				break;
			case 2:
				strcpy(caddieMotion, "\xBD\xC7\xB0\xDD");
				strcpy(letter, "\xB4\xF5\xBA\xED\xBA\xB8\xB1\xE2");
				break;
			case 3:
				strcpy(caddieMotion, "\xBD\xC7\xB0\xDD");
				strcpy(letter, "\xC6\xAE\xB8\xAE\xC7\xC3\xBA\xB8\xB1\xE2");
				break;
			default:
				strcpy(caddieMotion, "\xBD\xC7\xB0\xDD");

				if (GolfBall().m_holeIn == 1)
					sprintf(letter, "+%d", toPar);
				else
					strcpy(letter, "\xC6\xF7\xB1\xE2");
				break;
			}
		}
	} else if (score == 1) {
		strcpy(playerMotion, "\xBE\xCB\xB9\xD9\xC8\xA6\xC0\xCE\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
		strcpy(caddieMotion, "\xC8\xA6\xC0\xCE\xBF\xF8");
		strcpy(letter, "\xC8\xA6\xC0\xCE\xBF\xF8");
	} else {
		switch (toPar) {
		case -3:
			strcpy(playerMotion, "\xBE\xCB\xB9\xD9\xC8\xA6\xC0\xCE\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
			strcpy(caddieMotion, "\xC8\xA6\xC0\xCE\xBF\xF8");
			strcpy(letter, "\xBE\xCB\xB9\xD9\xC6\xAE\xB7\xCE\xBD\xBA");
			break;
		case -2:
			strcpy(playerMotion, "\xC0\xCC\xB1\xDB\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
			strcpy(caddieMotion, "\xC8\xA6\xC0\xCE");
			strcpy(letter, "\xC0\xCC\xB1\xDB");
			break;
		case -1:
			strcpy(playerMotion, "\xB9\xF6\xB5\xF0\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
			strcpy(caddieMotion, "\xC8\xA6\xC0\xCE");
			strcpy(letter, "\xB9\xF6\xB5\xF0");
			break;
		case 0:
			strcpy(playerMotion, "\xBC\xBC\xC0\xCC\xBA\xEA\xC6\xC4\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
			strcpy(caddieMotion, "\xC8\xA6\xC0\xCE");
			strcpy(letter, "\xC6\xC4");
			break;
		case 1:
			strcpy(playerMotion, "\xBA\xB8\xB1\xE2\xBD\xC7\xB0\xDD\xBD\xC7\xB8\xC1\xC6\xF7\xC1\xEE");
			strcpy(caddieMotion, "\xBD\xC7\xB0\xDD");
			strcpy(letter, "\xBA\xB8\xB1\xE2");
			break;
		case 2:
			strcpy(playerMotion, "\xBA\xB8\xB1\xE2\xBD\xC7\xB0\xDD\xBD\xC7\xB8\xC1\xC6\xF7\xC1\xEE");
			strcpy(caddieMotion, "\xBD\xC7\xB0\xDD");
			strcpy(letter, "\xB4\xF5\xBA\xED\xBA\xB8\xB1\xE2");
			break;
		case 3:
			strcpy(playerMotion, "\xBA\xB8\xB1\xE2\xBD\xC7\xB0\xDD\xBD\xC7\xB8\xC1\xC6\xF7\xC1\xEE");
			strcpy(caddieMotion, "\xBD\xC7\xB0\xDD");
			strcpy(letter, "\xC6\xAE\xB8\xAE\xC7\xC3\xBA\xB8\xB1\xE2");
			break;
		default:
			strcpy(playerMotion, "\xBA\xB8\xB1\xE2\xBD\xC7\xB0\xDD\xBD\xC7\xB8\xC1\xC6\xF7\xC1\xEE");
			strcpy(caddieMotion, "\xBD\xC7\xB0\xDD");

			if (GolfBall().m_holeIn == 1)
				sprintf(letter, "+%d", toPar);
			else
				strcpy(letter, "\xC6\xF7\xB1\xE2");
			break;
		}
	}

	if (toPar <= 0)
		SetConstantBGM("bgm_under_par.mp3", true, true);
	else
		SetConstantBGM("bgm_over_par.mp3", true, true);

	if (!noPose) {
		SendMsg(this, "Player", 0x2716, (intptr_t)playerMotion, 0, 0, 0);
		SendMsg(this, "Player", 0x2717, (intptr_t)"", 0, 0, 0);
		SendMsg(this, "Caddie", 0x2716, (intptr_t)caddieMotion, 0, 0, 0);

		char cameraName[0x40] = { 0 };
		const char *poseCamera = NULL;
		bool itemCamera = false;

		SendMsg(this, "Player", 0x2715, (intptr_t)&poseCamera, 0, 0, 0);
		if (strstr(poseCamera, "_item"))
			itemCamera = true;

		const char *camera;
		if ((CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_characterInfo.typeId & 0x3FFFFFF) == 0)
			camera = MakeStr("%s_%02d", poseCamera, rand() % 2 + 3);
		else
			camera = MakeStr("%s_%02d", poseCamera, rand() % 4 + 1);

		strncpy(cameraName, camera, 0x3F);

		const WVector &cup = m_point258;

		m_gameOverDelay = CCameraWork::Instance()->LoadCamera((CCameraWork::eCamMode)0, cameraName, cup);

		if (itemCamera == true)
			CCameraWork::Instance()->SetAbsAnim();

		if (m_gameOverDelay < 0.0f) {
			WVector eye, at;
			eye = WVector(cup.x, cup.y + 6.4f, cup.z - 32.0f);
			at = WVector(cup.x, cup.y + 6.4f, cup.z);
			g_camera = MakeCameraMatrix(eye, at);
			m_gameOverDelay = 3.5f;
		}
	}

	SendMsg(this, "Letter", 0x18D, (intptr_t)letter, 0, 0, 0);
}

// golfrule.cpp:8905
// The match game's end of hole poses (room type 3, two players): a player
// who gave the hole up concedes it unless he could still win the match; then
// the local player's win, draw or loss pose, camera, letter and screen, and
// the hole in result sent to the server.
bool CGolfRule::SetHoleInPose_Match(void) {
	if (CSharedDoc::Instance()->m_roomType == 3)
	{
		bool playing[2];
		playing[0] = PLAYER(0)->m_state == 1;
		playing[1] = PLAYER(1)->m_state == 1;

		if (!playing[0] && !playing[1])
			return false;

		if (playing[0] ^ playing[1]) {
			if (playing[0]) {
				if (PLAYER(0)->m_holes[GOLFDOC()->m_currentHole - 1] > PLAYER(1)->m_holes[GOLFDOC()->m_currentHole - 1]) {
					if (PLAYER(0)->m_holes[GOLFDOC()->m_currentHole - 1] != PLAYER(1)->m_holes[GOLFDOC()->m_currentHole - 1] + (unsigned char)1
						|| (int)PLAYER(0)->m_ghostResult <= (int)PLAYER(1)->m_ghostResult + GetHoles() - GetHoleIndex(GOLFDOC()->m_currentHole))
						return false;
				}

				PLAYER(1)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;
				RecordScoreByPlayer(1);
			} else {
				if (PLAYER(0)->m_holes[GOLFDOC()->m_currentHole - 1] < PLAYER(1)->m_holes[GOLFDOC()->m_currentHole - 1]) {
					if (PLAYER(1)->m_holes[GOLFDOC()->m_currentHole - 1] != PLAYER(0)->m_holes[GOLFDOC()->m_currentHole - 1] + (unsigned char)1
						|| (int)PLAYER(1)->m_ghostResult <= (int)PLAYER(0)->m_ghostResult + GetHoles() - GetHoleIndex(GOLFDOC()->m_currentHole))
						return false;
				}

				PLAYER(0)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;
				RecordScoreByPlayer(0);
			}
		}

		playing[0] = PLAYER(0)->m_state == 1;
		playing[1] = PLAYER(1)->m_state == 1;

		char motion[0x20];
		char caddieMotion[0x20];
		char letter[0x20];
		int result;
		bool lose = true;

		if (playing[0] && playing[1]) {
			if (PLAYER(MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1] <= PLAYER(1 - MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1]) {
				result = PLAYER(1 - MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1] != PLAYER(MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1];
				lose = false;
			}
		} else if (playing[MyIndex()]) {
			result = 1;
			lose = false;
		}

		if (lose) {
			strcpy(motion, "\xBA\xB8\xB1\xE2\xBD\xC7\xB0\xDD\xBD\xC7\xB8\xC1\xC6\xF7\xC1\xEE");
			strcpy(caddieMotion, "\xBD\xC7\xB0\xDD");
			strcpy(letter, "lose");

			result = -1;
		} else {
			if (PLAYER(MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1] == (unsigned char)1) {
				strcpy(motion, "\xBE\xCB\xB9\xD9\xC8\xA6\xC0\xCE\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
				strcpy(caddieMotion, "\xC8\xA6\xC0\xCE\xBF\xF8");
			} else {
				int score = (int)PLAYER(MyIndex())->m_holes[GOLFDOC()->m_currentHole - 1] - GOLFDOC()->GetHoleData().m_par;

				switch (score) {
				case -3:
					strcpy(motion, "\xBE\xCB\xB9\xD9\xC8\xA6\xC0\xCE\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
					strcpy(caddieMotion, "\xC8\xA6\xC0\xCE\xBF\xF8");
					break;
				case -2:
					strcpy(motion, "\xC0\xCC\xB1\xDB\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
					strcpy(caddieMotion, "\xC8\xA6\xC0\xCE");
					break;
				case -1:
					strcpy(motion, "\xB9\xF6\xB5\xF0\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
					strcpy(caddieMotion, "\xC8\xA6\xC0\xCE");
					break;
				default:
					strcpy(motion, "\xBC\xBC\xC0\xCC\xBA\xEA\xC6\xC4\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
					strcpy(caddieMotion, "\xC8\xA6\xC0\xCE");
					break;
				}
			}

			if (result == 0)
				strcpy(letter, "draw");
			else
				strcpy(letter, "win");
		}

		const WVector &cup = m_point258;
		SendMsg(this, "Player", 0x121, (intptr_t)&cup, (intptr_t)motion, 0, 0);
		SendMsg(this, "Caddie", 0x121, (intptr_t)&GetPVS().GetGroundPoint(WVector(cup.x + 10.0f, cup.y, cup.z + 10.0f), false, NULL, false), (intptr_t)caddieMotion, 0, 0);

		m_gameOverDelay = CCameraWork::Instance()->LoadCamera((CCameraWork::eCamMode)0, motion, cup);

		if (m_gameOverDelay < 0.0f) {
			WVector eye, at;
			eye = WVector(cup.x, cup.y + 6.4f, cup.z - 32.0f);
			at = WVector(cup.x, cup.y + 6.4f, cup.z);
			g_camera = MakeCameraMatrix(eye, at);
			m_gameOverDelay = 3.5f;
		}

		SendMsg(this, "Letter", 0x18D, (intptr_t)letter, 0, 0, 0);
		SendMsg(this, "Screen", 0x1B7, 0, 0, 0, 0);

		if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT) && (result == 0 || result == 1)) {
			WSendPacket packet((enumClientPacket)0x52);

			packet.Encode1(PLAYER(MyIndex())->m_holeInKind);
			packet.Encode1(PLAYER(MyIndex())->m_holeInCount);
			packet.Send(SEND_TO_DEFAULT);
		}
	}

	return true;
}

// golfrule.cpp:9053
// The team game's end of hole poses (room type 1, two teams): a team that
// gave the hole up concedes it unless it could still win the match; then the
// local team's win, draw or loss poses (the club holder plays the main
// motion, the partner the rank motion), camera, letter and screen, and the
// hole in result sent to the server.
bool CGolfRule::SetHoleInPose_Team(void) {
	if (GetGameType() != 1)
		return true;

	unsigned char myTeam = PLAYER(MyIndex())->m_attribute;

	bool playing[2];
	playing[0] = TEAM(0)->m_state == 1;
	playing[1] = TEAM(1)->m_state == 1;

	if (!playing[0] && !playing[1])
		return false;

	if (playing[0] ^ playing[1]) {
		if (playing[0]) {
			if (TEAM(0)->m_holes[GOLFDOC()->m_currentHole - 1] > TEAM(1)->m_holes[GOLFDOC()->m_currentHole - 1]) {
				if (TEAM(0)->m_holes[GOLFDOC()->m_currentHole - 1] != TEAM(1)->m_holes[GOLFDOC()->m_currentHole - 1] + (unsigned char)1
					|| TEAM(0)->m_resetCount <= GetHoles() - GetHoleIndex(GOLFDOC()->m_currentHole) + TEAM(1)->m_resetCount)
					return false;
			}

			TEAM(1)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;
			RecordScoreByTeam(1);
		} else {
			if (TEAM(0)->m_holes[GOLFDOC()->m_currentHole - 1] < TEAM(1)->m_holes[GOLFDOC()->m_currentHole - 1]) {
				if (TEAM(1)->m_holes[GOLFDOC()->m_currentHole - 1] != TEAM(0)->m_holes[GOLFDOC()->m_currentHole - 1] + (unsigned char)1
					|| TEAM(1)->m_resetCount <= GetHoles() - GetHoleIndex(GOLFDOC()->m_currentHole) + TEAM(0)->m_resetCount)
					return false;
			}

			TEAM(0)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;
			RecordScoreByTeam(0);
		}
	}

	SendMsg(this, "Caddie", 0x164, 0, 0, 0, 0);

	playing[0] = TEAM(0)->m_state == 1;
	playing[1] = TEAM(1)->m_state == 1;

	char names[4][0x20]; // the motion, the rank motion, the camera and the letter
	int result;
	bool lose = true;

	if (playing[0] && playing[1]) {
		if (TEAM(myTeam)->m_holes[GOLFDOC()->m_currentHole - 1] <= TEAM(1 - myTeam)->m_holes[GOLFDOC()->m_currentHole - 1]) {
			result = TEAM(1 - myTeam)->m_holes[GOLFDOC()->m_currentHole - 1] != TEAM(myTeam)->m_holes[GOLFDOC()->m_currentHole - 1];
			lose = false;
		}
	} else if (playing[myTeam]) {
		result = 1;
		lose = false;
	}

	if (lose) {
		strcpy(names[0], "\xBA\xB8\xB1\xE2\xBD\xC7\xB0\xDD\xBD\xC7\xB8\xC1\xC6\xF7\xC1\xEE");
		strcpy(names[1], "3\xB5\xEE\xB8\xF0\xBC\xC7");
		strcpy(names[2], "\xBA\xB8\xB1\xE2\xBD\xC7\xB0\xDD\xBD\xC7\xB8\xC1\xC6\xF7\xC1\xEE");
		strcpy(names[3], "lose");

		result = -1;
	} else {
		if (TEAM(myTeam)->m_holes[GOLFDOC()->m_currentHole - 1] == (unsigned char)1) {
			strcpy(names[0], "\xBE\xCB\xB9\xD9\xC8\xA6\xC0\xCE\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
		} else {
			int score = (int)TEAM(myTeam)->m_holes[GOLFDOC()->m_currentHole - 1] - GOLFDOC()->GetHoleData().m_par;

			switch (score) {
			case -3:
				strcpy(names[0], "\xBE\xCB\xB9\xD9\xC8\xA6\xC0\xCE\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
				break;
			case -2:
				strcpy(names[0], "\xC0\xCC\xB1\xDB\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
				break;
			case -1:
				strcpy(names[0], "\xB9\xF6\xB5\xF0\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
				break;
			default:
				strcpy(names[0], "\xBC\xBC\xC0\xCC\xBA\xEA\xC6\xC4\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
				break;
			}
		}

		if (result == 0)
			strcpy(names[1], "2\xB5\xEE\xB8\xF0\xBC\xC7");
		else
			strcpy(names[1], "1\xB5\xEE\xB8\xF0\xBC\xC7");

		strcpy(names[2], names[0]);

		if (result == 0)
			strcpy(names[3], "draw");
		else
			strcpy(names[3], "win");
	}

	int partner = TEAM(myTeam)->m_clubHolder != MyIndex();

	const WVector &cup = m_point258;
	if (partner == 0) {
		SendMsg(this, "Player", 0x120, (intptr_t)names[0], (intptr_t)names[1], (intptr_t)&cup, 0);
	} else {
		SendMsg(this, "Player", 0x120, (intptr_t)names[1], (intptr_t)names[0], (intptr_t)&cup, 0);
	}

	m_gameOverDelay = CCameraWork::Instance()->LoadCamera((CCameraWork::eCamMode)0, names[2], cup);

	if (m_gameOverDelay < 0.0f) {
		WVector eye, at;
		eye = WVector(cup.x, cup.y + 6.4f, cup.z - 32.0f);
		at = WVector(cup.x, cup.y + 6.4f, cup.z);
		g_camera = MakeCameraMatrix(eye, at);
		m_gameOverDelay = 3.5f;
	}

	SendMsg(this, "Letter", 0x18D, (intptr_t)names[3], 0, 0, 0);
	SendMsg(this, "Screen", 0x1B7, 0, 0, 0, 0);

	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT) && (result == 0 || result == 1)) {
		WSendPacket packet((enumClientPacket)0x35);

		packet.Encode1(TEAM(PLAYER(MyIndex())->m_attribute)->m_holeInKind);
		packet.Encode1(TEAM(PLAYER(MyIndex())->m_attribute)->m_holeInCount);
		packet.Send(SEND_TO_DEFAULT);
	}

	return true;
}

// golfrule.cpp:9216
__int64 CGolfRule::CalcWinnersPang(unsigned long uid) {
	__int64 pang = 0;
	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (PLAYER(i)->m_state != 3) {
			if (PLAYER(i)->m_uid != uid) {
				pang += GOLFDOC()->m_maxPangPerLoser < CSharedDoc::Instance()->m_roomPlayers[i].m_pang ? GOLFDOC()->m_maxPangPerLoser : CSharedDoc::Instance()->m_roomPlayers[i].m_pang;
			}
		}
	}

	return GOLFDOC()->m_winnerPangBonus + pang;
}

// golfrule.cpp:9236
// The skins game's hole result: with no winner the hole's pang carries over
// to the next hole (doubled count, at most 8); with a winner every other
// player pays it at most m_maxPangPerLoser and the winner takes the total.
void CGolfRule::CalcSkinsHolePang(void) {
	unsigned long winnerUid = GOLFDOC()->m_selectedIndexA;

	if (winnerUid == 0xFFFFFFFF) {
		for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			if (PLAYER(i)->m_state != 3) {
				PLAYER(i)->m_holeRecord2[GOLFDOC()->m_currentHole - 1] = -1;
				PLAYER(i)->m_rewardPang = 0;

				if (PLAYER(i)->m_state == 0)
					RecordScoreByPlayer(i);
			}
		}

		if (!GOLFDOC()->m_approachMode) {
			GOLFDOC()->m_clearedFlag1AC++;

			GOLFDOC()->GetHoleData(GOLFDOC()->m_currentHole).m_redPang = true;

			if (GOLFDOC()->m_currentHole != CSharedDoc::Instance()->m_holeOrder[CSharedDoc::Instance()->m_holes - 1]) {
				GOLFDOC()->GetHoleData(GOLFDOC()->m_currentHole).m_pang = GOLFDOC()->GetHoleData(GOLFDOC()->m_currentHole).m_basePang;

				__int64 total = GetHolePang(GetHoleIndex(GOLFDOC()->m_currentHole + 1))
					+ GOLFDOC()->m_skinsPangTotal;
				unsigned long carry = min(GOLFDOC()->m_skinsCarryCount * 2, 8);

				GOLFDOC()->GetHoleData(GOLFDOC()->m_currentHole + 1).m_redPang = true;
				GOLFDOC()->GetHoleData(GOLFDOC()->m_currentHole + 1).m_pang = carry * total;
			}
		}
	} else {
		__int64 winnersPang = CalcWinnersPang(winnerUid);

		for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			if (PLAYER(i)->m_state != 3) {
				int hole = GOLFDOC()->m_approachMode ? CSharedDoc::Instance()->m_holeOrder[CSharedDoc::Instance()->m_holes - 1] : GOLFDOC()->m_currentHole;

				if (PLAYER(i)->m_uid != winnerUid) {
					PLAYER(i)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;

					__int64 loss = GOLFDOC()->m_maxPangPerLoser < CSharedDoc::Instance()->m_roomPlayers[i].m_pang ? GOLFDOC()->m_maxPangPerLoser : CSharedDoc::Instance()->m_roomPlayers[i].m_pang;
					PLAYER(i)->m_rewardPang = -(int)loss;
					PLAYER(i)->m_holeRecord2[hole - 1] = -(int)loss;
					PLAYER(i)->m_roundPang -= loss;

					CSharedDoc::Instance()->m_roomPlayers[i].m_pang -= loss;
				} else {
					PLAYER(i)->m_ghostResult++;
					PLAYER(i)->m_rewardPang = winnersPang;
					PLAYER(i)->m_holeRecord2[hole - 1] = (int)winnersPang;
					PLAYER(i)->m_roundPang += winnersPang;
					CSharedDoc::Instance()->m_roomPlayers[i].m_pang += winnersPang;
				}

				if (PLAYER(i)->m_state == 0)
					RecordScoreByPlayer(i);
			}
		}

		GOLFDOC()->m_clearedFlag1AC = 0;

		GOLFDOC()->m_maxPangPerLoser = 0;
		GOLFDOC()->m_winnerPangBonus = 0;
	}
}

// golfrule.cpp:9333
// The skins game's end of hole poses (room type 7): the winner's, a loser's
// or a draw's player and caddie motions, camera, music and score screens.
bool CGolfRule::SetHoleInPose_Skins(void) {
	if (CSharedDoc::Instance()->m_roomType == 7)
	{
		char playerMotion[0x20];
		char caddieMotion[0x20];
		char cameraName[0x20];

		unsigned long winnerUid = GOLFDOC()->m_selectedIndexA;

		// 1 won, 0 drawn (no winner), -1 lost
		int result;
		int noItemPose = 1;
		if (winnerUid == 0xFFFFFFFF)
			result = 0;
		else if (winnerUid == PLAYER(MyIndex())->m_uid)
			result = 1;
		else
			result = -1;

		if (result == -1) {
			if (GOLFDOC()->m_clearedFlag1AC < 3) {
				strcpy(playerMotion, "\xBA\xB8\xB1\xE2\xBD\xC7\xB0\xDD\xBD\xC7\xB8\xC1\xC6\xF7\xC1\xEE");
			} else {
				strcpy(playerMotion, "3\xB5\xEE\xB8\xF0\xBC\xC7");
				noItemPose = 0;
			}

			if (GOLFDOC()->m_clearedFlag1AC < 3)
				strcpy(caddieMotion, "\xBD\xC7\xB0\xDD");
			else
				strcpy(caddieMotion, "\xBA\xA1\xC4\xBF");

			strcpy(cameraName, "\xBA\xB8\xB1\xE2\xBD\xC7\xB0\xDD\xBD\xC7\xB8\xC1\xC6\xF7\xC1\xEE");
		} else if (result == 1) {
			if (GOLFDOC()->m_clearedFlag1AC < 2) {
				strcpy(playerMotion, "\xB9\xF6\xB5\xF0\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
			} else if (GOLFDOC()->m_clearedFlag1AC < 4) {
				strcpy(playerMotion, "1\xB5\xEE\xB8\xF0\xBC\xC7");
				noItemPose = 0;
			} else {
				strcpy(playerMotion, "\xC0\xCC\xB1\xDB\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
			}

			if (GOLFDOC()->m_clearedFlag1AC < 3)
				strcpy(caddieMotion, "\xC8\xA6\xC0\xCE");
			else
				strcpy(caddieMotion, "\xC8\xA6\xC0\xCE\xBF\xF8");

			if (GOLFDOC()->m_clearedFlag1AC < 3)
				strcpy(cameraName, "\xB9\xF6\xB5\xF0\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
			else
				strcpy(cameraName, "\xC0\xCC\xB1\xDB\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
		} else {
			if (GOLFDOC()->m_clearedFlag1AC < 3) {
				strcpy(playerMotion, "\xBC\xBC\xC0\xCC\xBA\xEA\xC6\xC4\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
			} else {
				strcpy(playerMotion, "2\xB5\xEE\xB8\xF0\xBC\xC7");
				noItemPose = 0;
			}

			strcpy(caddieMotion, "\xC8\xA6\xC0\xCE");
			strcpy(cameraName, "\xBC\xBC\xC0\xCC\xBA\xEA\xC6\xC4\xBD\xC2\xB8\xAE\xC6\xF7\xC1\xEE");
		}

		if (result == 1)
			SetConstantBGM("skins_win.mp3", true, true);
		else if (result == 0)
			SetConstantBGM("skins_draw.mp3", true, true);
		else
			SetConstantBGM("skins_lose.mp3", true, true);

		const WVector &cup = m_point258;
		SendMsg(this, "Player", 0x122, (intptr_t)&cup, (intptr_t)playerMotion, noItemPose, 0);
		SendMsg(this, "Caddie", 0x121, (intptr_t)&GetPVS().GetGroundPoint(WVector(cup.x + 10.0f, cup.y, cup.z + 10.0f), false, NULL, false), (intptr_t)caddieMotion, 0, 0);

		m_gameOverDelay = CCameraWork::Instance()->LoadCamera((CCameraWork::eCamMode)0, cameraName, cup);

		if (m_gameOverDelay < 0.0f) {
			WVector eye, at;
			eye = WVector(cup.x, cup.y + 6.4f, cup.z - 32.0f);
			at = WVector(cup.x, cup.y + 6.4f, cup.z);
			g_camera = MakeCameraMatrix(eye, at);
			m_gameOverDelay = 3.5f;
		}

		SendMsg(this, "Letter", 0x18C, 0, 0, 0, 0);

		SendMsg(this, "Screen", 0x1C7, result, (int)CalcWinnersPang(CSharedDoc::Instance()->m_userInfo.guid), 0, 0);
		SendMsg(this, "Screen", 0x1C8, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1B7, 0, 0, 0, 0);
	}

	return true;
}

// golfrule.cpp:9458
void CGolfRule::DoFlyBall(float elapsed) {
	if (m_continueDlgOpen || m_netMsgWaitSend)
		return;

	unsigned char powerShot = GolfClub().PowerShotType();

	if ((powerShot == 2 || powerShot == 3) && GolfClub().GetType() == 0 && !m_powerShotCamera && CCameraWork::Instance()->Finished()) {
		CCameraWork::Instance()->SelectCamera(-1);
		m_powerShotCamera = true;
	}

	if (!(m_replayFlags & 2) && !g_input->Get("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", true) && m_replayMode == 0) {
		CCameraWork::Instance()->Unlock();
	}

	WPolySoup *pPolySoup = GOLFDOC()->m_pPolySoup;
	CCameraWork *pCameraWork = CCameraWork::Instance();

	WPolySoup::sArea *pArea = pPolySoup->GetAreaByPos(GolfBall().m_pos);

	if (pArea && m_cameraArea != pArea) {
		char cameraName[0x40];
		WSliceStr slice = pArea->m_name;
		slice.Get("*cam", "%s", cameraName);

		WPolySoup::sCamera *pCamera = pPolySoup->GetCameraByName(cameraName);

		if (pCamera) {
			pCameraWork->SetInstalledCamera(pCamera->m_matrix.pivot);
		}

		m_cameraArea = pArea;
	} else {
		pArea = pPolySoup->GetAreaByPos(GolfBall().m_pos);

		if (pArea == 0) {
			m_cameraArea = pArea;
			if (pCameraWork->GetCameraType() == 0x15)
				pCameraWork->UninstallCamera();
		}
	}

	CGolfBall &ball = GolfBall();

	if (GolfBall().m_state != 0) {
		DoFlyBall_Flying(elapsed);
	} else {
		DoFlyBall_Stopped(elapsed);
	}
}

// golfrule.cpp:9531
// The ball's flight, one step per 1/50 s of accumulated time: each predicted
// point the ball passes raises its events (the first ground contact's effect
// and sound, the gallery's reactions, the magic carpet and booster warps), the
// power spin starts after the spin point, and once the last point is reached
// the ball stops. In between two points the ball's position, velocity and
// rotation are interpolated, and the camera follows.
void CGolfRule::DoFlyBall_Flying(float elapsed) {
	CGolfBall &ball = GolfBall();
	float t;

	m_gameOverDelay = elapsed + m_gameOverDelay;

	if (OnlinePlay()) {
		if (ball.m_preDataIndex > (ball.m_preDataCount - ball.m_eventPreDataIndex[6]) * 0.8f
			&& m_flagCutin && GOLFDOC()->m_gameMode == 0xF) {
			PostMsg(this, "GolfRule", 0x24D, 1, (intptr_t)"S5::mode_flyBall", 0, 0);
			SendMsg(this, "Cutin", 0x78, 0x20, 0, 0, 0);
			m_flagCutin = false;
		}
	}

	if (m_gameOverDelay >= 0.02f) {
		int steps = (int)(m_gameOverDelay * 50.0f);
		if (ball.m_preDataIndex + steps > ball.m_preDataCount)
			steps = ball.m_preDataCount - ball.m_preDataIndex;

		bool first = true;
		for (int i = 0; i < steps; i++) {
			GetTask()->ProcessNetSyncActor(true, 0.02f, ball.m_preDataIndex + i + 1);

			const CGolfBall::sExpected *data = ball.PreData(ball.m_preDataIndex + i + 1);

			if (data->m_groundType > 0)
				SendMsg(this, "ShotReview", -1, (intptr_t)"RECORD_REAL_BOUNCE_BALL", (intptr_t)data, 0, 0);

			if (data->m_groundType != 0) {
				if (data->m_groundType == 6) {
					SendMsg(this, "MagicCarpetHelper", 0x280, 0, 0, 0, 0);
					CFx::Instance()->MakeBoosterWarp(ball, 6);
				}
			}

			if (first) {
				first = false;

				if (data->m_groundType != 0) {
					ball.m_flags &= ~0x70;

					switch (data->m_groundSubType) {
					case 1:
						SendMsg(this, "Effect", 0x173, 0, 0, 0, 0);
						break;
					case 2:
						if (data->m_velocity.Magnitude() > 30.0f)
							SendMsg(this, "Effect", 0x174, ball.m_preDataIndex + i + 1, 0, 0, 0);
						break;
					case 3:
						SendMsg(this, "Effect", 0x171, 0, 0, 0, 0);
						break;
					case 4:
						SendMsg(this, "Effect", 0x176, 0, 0, 0, 0);
						break;
					}

					if (data->m_groundType == 2)
						SendMsg(this, "Gallery", 0x102, 0, 0, 0, 0);

					if (data->m_groundType == 5) {
						CFx::Instance()->MakeBoosterWarp(ball, 0);

						if (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId == 0x14000005)
							SendMsg(this, "Ball", 0x1EE, 0, 0, 0, 0);
					}

					if (ball.m_preDataIndex + i + 1 == ball.m_eventPreDataIndex[5])
						SendMsg(this, "Gallery", 0x101, 0, 0, 0, 0);

					if (m_flySound[0])
						g_audio->StopSfx(m_flySound);

					if (data->m_sound[0] && !g_audio->IsPlaying(data->m_sound)) {
						WVector pos;
						pos = ball.m_pos - g_camera.pivot;
						g_audio->PlaySfx(data->m_sound, &pos, 5, data->m_sound, 0, 0.5f, 200.0f);
					}
				}
			}

			if (ball.m_preDataIndex + i + 1 == ball.m_eventPreDataIndex[2])
				SendMsg(this, "Gallery", 0x10D, 0, 0, 0, 0);

			if (ball.m_holeIn != 1 && ball.m_preDataIndex + i + 1 == ball.m_eventPreDataIndex[4])
				SendMsg(this, "Gallery", 0x109, 0, 0, 0, 0);

			int lead = Min(80, ball.m_eventPreDataIndex[11]);
			if (ball.m_eventPreDataIndex[11] - lead == ball.m_preDataIndex + i + 1) {
				SendMsg(this, "MagicCarpetHelper", 0x27E, 0, 0, 0, 0);
			}

			if (ball.m_preDataIndex + i + 1 == ball.m_eventPreDataIndex[7]) {
				SendMsg(this, "MagicCarpetHelper", 0x27F, 0, 0, 0, 0);
			}

			if (ball.m_eventPreDataIndex[11] == ball.m_preDataIndex + i + 1) {
				SendMsg(this, "MagicCarpetHelper", 0x27D, 0, 0, 0, 0);
			}
		}

		ball.m_preDataIndex = Min(ball.m_preDataIndex + steps, ball.m_preDataCount);
		m_gameOverDelay -= steps * 0.02f;

		if (ball.m_state == CGolfBall::BALL_ROLL
			&& GetPVS().GetGroundType(ball.PreData(ball.m_preDataIndex)->m_pos, true, NULL) == 2
			&& GetPVS().GetGroundType(ball.PreData(Min(ball.m_preDataIndex + steps, ball.m_preDataCount))->m_pos, true, NULL) != 2)
			SendMsg(this, "Gallery", 0x10A, 0, 0, 0, 0);

		if (ball.m_eventPreDataIndex[6] > 0 && ball.m_preDataIndex > ball.m_eventPreDataIndex[6]
			&& ball.m_preDataIndex < ball.m_eventPreDataIndex[6] + 50) {
			g_audio->PlaySfx("spin_super", NULL, 0, NULL, NULL, 0.5f, 200.0f);

			if (!CFx::Instance()->FindControl(data_a7df58)) {
				data_a7df58 = CFx::Instance()->MakePowerSpin(ball);
			}
		}

		if (ball.m_preDataIndex == ball.m_preDataCount) {
			if (ball.m_lie) {
				if (GetCurMap() == 13) {
					CFxSequence *seq = CFx::Instance()->OpenSequence("hell_ob.seq", true);

					if (seq)
						seq->Pos() = GolfBall().m_pos;
				} else {
					SendMsg(this, "Effect", 0x176, 0, 0, 0, 0);
				}
			}

			m_gameOverDelay = 0;
			ball.m_velocity.Reset();
			ball.m_state = CGolfBall::BALL_STOP;

			g_audio->StopSfx("shot_dist_count");

			SendMsg(this, "Screen", 0x1CA, 0, 0, 0, 0);

			SendMsg(this, "ShotReview", -1, (intptr_t)"SAVE_BALL_TRACE", (intptr_t)&GolfBall(), 0, 0);
		}

		if ((m_replayFlags & 2) && ball.m_preDataIndex > 0.3f * 50.0f) {
			m_replayMode++;

			if (m_replayMode == 3) {
				m_replayFlags &= ~2;
				m_replayMode = 0;
				ReactionForShot();

				if (g_input->Get("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", true))
					g_camera = GOLFDOC()->GetDefaultCamera();

				CCameraWork::Instance()->SelectCamera(-1);
			} else {
				SwingReplay();
			}
		}
	}

	if (GolfClub().GetType() != 3 && (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId & 0x3FFFFFF) == 0x20) {
		if (!m_flag5B1 && ball.m_preDataIndex > ball.m_eventPreDataIndex[1]) {
			CFxSequence *seq = CFx::Instance()->OpenSequence("rainbow_002.seq", true);

			if (seq)
				seq->Pos() = ball.m_pos;

			m_flag5B1 = true;
		}
	}

	if (ball.m_preDataIndex < ball.m_preDataCount && GOLFDOC()->m_gameMode == 0x80) {
		t = m_gameOverDelay > 0.0f ? (0.02f - m_gameOverDelay) * 50.0f : 1.0f;

		ball.m_pos = ball.PreData(ball.m_preDataIndex)->m_pos * t + ball.PreData(ball.m_preDataIndex + 1)->m_pos * (1.0f - t);
		ball.m_velocity = ball.PreData(ball.m_preDataIndex)->m_velocity * t + ball.PreData(ball.m_preDataIndex + 1)->m_velocity * (1.0f - t);
		ball.m_rotation = ball.PreData(ball.m_preDataIndex)->m_rotation * t + ball.PreData(ball.m_preDataIndex + 1)->m_rotation * (1.0f - t);
	} else {
		t = 0;
		ball.m_pos = ball.PreData(ball.m_preDataIndex)->m_pos;
		ball.m_velocity = ball.PreData(ball.m_preDataIndex)->m_velocity;
		ball.m_rotation = ball.PreData(ball.m_preDataIndex)->m_rotation;
	}

	if (GolfClub().GetType() != 3 && GetCurMap() == 10 && !m_flag42C
		&& ball.m_preDataIndex >= ball.m_eventPreDataIndex[7]) {
		m_flag42C = true;
		SendMsg(this, "Trajectory", 0x15D, 0, 0, 0, 0);
	}

	if (!(m_replayFlags & 2) || m_replayMode != 0)
		CCameraWork::Instance()->Process(elapsed, 1.0f - t);
}

// golfrule.cpp:9844
// Replay: steps CSharedDoc::m_replayStartB to the next recorded shot (clamped
// to the count of recorded shots) and restores that shot's hole block, wind,
// combo gauge, quadtree frame and pangs.
bool CGolfRule::GetShotCntData(void) {
	int count = CSharedDoc::Instance()->m_shotDataList.size();
	int index = 1;
	bool result = false;

	if (CSharedDoc::Instance()->m_replayState == 14) {
		CSharedDoc::Instance()->m_replayStartB = 0;
		return true;
	}

	if (CSharedDoc::Instance()->m_replayStartB <= count) {
		if (CSharedDoc::Instance()->m_replayStartB != count)
			CSharedDoc::Instance()->m_replayStartB += 1;
	} else {
		CSharedDoc::Instance()->m_replayStartB = count;
	}

	result = true;

	for (std::list<sShotDataStorage>::iterator it = CSharedDoc::Instance()->m_shotDataList.begin(); it != CSharedDoc::Instance()->m_shotDataList.end(); ++it) {
		if (index == CSharedDoc::Instance()->m_replayStartB) {
			sShotDataStorage &data = *it;

			// CSharedDoc::m_shotDataList still holds the sShotDataStorage
			// stand-in for sShotData: copy the 0x88 bytes into the hole block.
			memcpy(&m_hole, &data, sizeof(sShotData)); Wind().SetWind(m_holeWind);
			CSharedDoc::Instance()->m_shotCountKey = m_shotCountKey;
			GOLFDOC()->GetPlayer(0)->m_comboGauge = m_comboGauge;

			GetPVS().SetFrame(m_shotFrame);

			if (CSharedDoc::Instance()->m_replayStartB == 0) {
				GOLFDOC()->GetPlayer(0)->m_extraPang = 0;
				GOLFDOC()->GetPlayer(0)->m_ghostAward = 0;
			} else {
				GOLFDOC()->GetPlayer(0)->m_extraPang = m_extraPang;
				GOLFDOC()->GetPlayer(0)->m_ghostAward = m_ghostAward;
			}
			break;
		}

		index++;
	}

	return result;
}

// A BMP file header with its "BM" signature as characters: the original
// stores the signature byte by byte and IntergrateFile carries a /GS cookie.
#pragma pack(push, 1)
struct sBmpFileHeader {
	char type[2];               // 0x0, "BM"
	unsigned long size;         // 0x2
	unsigned short reserved1;   // 0x6
	unsigned short reserved2;   // 0x8
	unsigned long offBits;      // 0xA
};
#pragma pack(pop)
CHECK_SIZE(sBmpFileHeader, 0xE);

// golfrule.cpp:9911
// The ball came to rest. A replay in playback first steps to its next
// recorded shot. Holed: the hole's result is announced (sounds, effects, the
// voice of the club set, the score's prize) and after 2 s (3 s in a team game)
// the round moves on, through the replay of the hole when one was asked. Not
// holed: the gallery reacts, an out-of-bounds or water ball costs a stroke,
// and after 2 s (3.5 s when not landed) the next shot starts, unless the
// "flower of forgetting" dialog offers to take the shot back.
void CGolfRule::DoFlyBall_Stopped(float elapsed) {
	m_flag42C = false;

	CGolfBall &ball = GolfBall();

	m_flag266 = true;

	if (IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK) {
		if (CSharedDoc::Instance()->m_replayPlayType == 2) {
			bool next = GetShotCntData();

			if (next != true)
				return;

			CSharedDoc *doc = CSharedDoc::Instance();

			if (doc->m_replayState == 7) {
				if (ball.IsHoleIn())
					doc->m_replayState = 8;
			} else if (ball.IsHoleIn() && doc->m_replayState != 11) {
				doc->m_replayState = 13;
			} else {
				doc->m_replayState = 10;
			}
		} else if (CSharedDoc::Instance()->m_replayPlayType == 1) {
			if (CSharedDoc::Instance()->m_replayState == 7)
				CSharedDoc::Instance()->m_replayState = 6;

			if (CSharedDoc::Instance()->m_replayState == 11 || CSharedDoc::Instance()->m_replayState == 12)
				CSharedDoc::Instance()->m_replayState = 10;
		}

		// "\xB0\xF8": the ball
		WPuppet *puppet = CSceneManager::Instance()->GetPuppet("\xB0\xF8");
		if (puppet && puppet->GetRootBone())
			puppet->SetAlpha(0xFF);

		CCameraWork::Instance()->StopBallSound();
		SendMsg(this, "Score", 0x132, 0, 0, 0, 0);
		g_audio->StopSfx("shot_dist_count");

		if (CSharedDoc::Instance()->m_replayState == 8) {
			CSharedDoc::Instance()->m_replayState = 7;
			ChangeGameMode(0x1000);
		} else if (CSharedDoc::Instance()->m_replayState == 13) {
			ChangeGameMode(0x1000);
		} else {
			ChangeGameMode(0x800);
		}

		m_replaying = true;
	}

	int score = CSharedDoc::Instance()->m_roomType == 1 ? TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] : PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1];
	bool roundDone = GOLFDOC()->m_gameState >= 8 || GOLFDOC()->m_gameState == 4 || GOLFDOC()->m_gameState == 10;

	if (ball.IsHoleIn() && roundDone && CSharedDoc::Instance()->m_roomType != 10) {
		if (m_replaying) {
			m_replaying = false;

			WPuppet *puppet = CSceneManager::Instance()->GetPuppet("\xB0\xF8");
			if (puppet && puppet->GetRootBone())
				puppet->SetAlpha(0xFF);

			CCameraWork::Instance()->StopBallSound();
			SendMsg(this, "Score", 0x132, 0, 0, 0, 0);
			g_audio->StopSfx("shot_dist_count");

			// "\xC6\xDB\xC6\xC3\xBC\xBA\xB0\xF8": putt holed
			if (m_flag3F0)
				SendMsg(this, "Player", 0x2716, (intptr_t)"\xC6\xDB\xC6\xC3\xBC\xBA\xB0\xF8", 0, 0, 0);

			if (IsLocalContent((localContentType_t)0x55)) {
				unsigned long ballId = CSharedDoc::Instance()->m_myBallId;

				if (ballId)
					CAztecHoleinEffect::Instance()->PlayEffect(ballId);
			}

			if (m_replayFlags & 1) {
				m_replayMode++;

				m_replaying = true;
				switch (m_replayMode) {
				case 1:
					CCameraWork::Instance()->Lock();

					if (IsMassGame() && GOLFDOC()->m_currentHole == GetEndHole()
						&& !m_goStopSent) {
						m_goStopSent = true;
						WSendPacket packet((enumClientPacket)0x1C);
						EncodePacket((enumClientPacket)0x1C, packet);
						packet.Send(SEND_TO_DEFAULT);
					}
				case 2:
					HoleReplay();
					return;
				case 3:
					m_replayFlags &= ~1;
					CCameraWork::Instance()->Unlock();
					break;
				}
			} else {
				if (IsMassGame() && GOLFDOC()->m_currentHole == GetEndHole()
					&& !m_goStopSent) {
					m_goStopSent = true;
					WSendPacket packet((enumClientPacket)0x1C);
					EncodePacket((enumClientPacket)0x1C, packet);
					packet.Send(SEND_TO_DEFAULT);
				}

				SendMsg(this, "Effect", 0x179, 0, 0, 0, 0);

				int toPar = score - GOLFDOC()->GetHoleData().m_par;

				GetWinningPrize(toPar);

				if (toPar <= 0) {
					int coins = toPar < 0 ? 13 : 6;

					if (CSharedDoc::Instance()->m_serverProperty.GetFlag(0x80))
						coins = S5::UTIL::GetHoleinCoinCount(coins, 2);

					if (CSharedDoc::Instance()->m_roomType == 7 && GOLFDOC()->m_clearedShort194 == 2) {
						m_effectParam = 30;
						m_effectTime = 0.1f;
						m_effectCount = GOLFDOC()->m_clearedFlag1AC;
					} else {
						SendMsg(this, "Effect", 0x17D, coins, 0, 0, 0);
					}
				}

				unsigned long waveState = 0;
				if (score == 1) {
					waveState = 0x400;
				} else {
					switch (toPar) {
					case -3:
						waveState = 0x800;
						break;
					case -2:
						waveState = 0x1000;
						break;
					case -1:
						waveState = 0x2000;
						break;
					case 0:
						waveState = 0x4000;
						break;
					case 1:
						waveState = 0x8000;
						break;
					case 2:
						waveState = 0x10000;
						break;
					case 3:
						waveState = 0x20000;
						break;
					case 4:
						waveState = 0x40000;
						break;
					}
				}

				if (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId == 0x14000004) {
					// t_Ex7(birdie), t_Ex7(par), t_Ex7(over par)
					if (toPar < 0)
						g_audio->PlaySfx(MakeStr("t_Ex7(\xB9\xF6\xB5\xF0)-%d", rand() % 4 + 1), NULL, 0, NULL, NULL, 0.5f, 200.0f);
					else if (toPar == 0)
						g_audio->PlaySfx(MakeStr("t_Ex7(\xC6\xC4)-%d", rand() % 4 + 1), NULL, 0, NULL, NULL, 0.5f, 200.0f);
					else if (toPar > 0)
						g_audio->PlaySfx(MakeStr("t_Ex7(\xBF\xC0\xB9\xF6\xC6\xC4)-%d", rand() % 4 + 1), NULL, 0, NULL, NULL, 0.5f, 200.0f);
				} else {
					if (!CVoiceItem::Instance()->PlaySound(CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId, (CVoiceItem::eWaveState)waveState, -1)) {
						if (toPar < 0)
							g_audio->PlaySfx("Birdie", NULL, 0, NULL, NULL, 0.5f, 200.0f);
						else if (toPar == 0)
							g_audio->PlaySfx("par", NULL, 0, NULL, NULL, 0.5f, 200.0f);
						else if (toPar > 0)
							g_audio->PlaySfx("Bogey", NULL, 0, NULL, NULL, 0.5f, 200.0f);
					}
				}

				if (GetGameType() == 1 || GetGameType() == 3)
					SetHoleInPose(true);
			}
		}

		m_gameOverDelay += elapsed;

		if (m_gameOverDelay > (CSharedDoc::Instance()->m_roomType == 1 ? 3.0f : 2.0f)) {
			bool next;

			if (GOLFDOC()->m_gameState == 15)
				SendMsg(this, "Screen", 0x1B8, (intptr_t)&next, 0, 0, 0);
			else
				next = true;

			if (next || m_gameOverDelay > (CSharedDoc::Instance()->m_roomType == 1 ? 7.0f : 6.0f)) {
				if (m_replayFlags & 4) {
					m_replayFlags &= ~4;
					Replay(false);
					return;
				}

				m_gameOverDelay = 0;
				UpdateShotResultStat();

				if (GOLFDOC()->m_gameState == 4) {
					if (!m_shotKeyAlways) {
						SendMsg(this, "GolfRule", 0x88, 1, 0, 0, 0);

						IActor *tutorial = GetActor("Tutorial");
						if (tutorial) {
							tutorial << MsgObject(NULL, 0x149, 0, 0, 0, 0, 0);
						}
					} else {
						ChangeGameMode(0x800);
					}
				} else {
					if (!IsLocalContent((localContentType_t)0x4F) || CSharedDoc::Instance()->m_playingMode != REPLAY_MODE_PLAYBACK)
						RecordScoreByPlayer(GOLFDOC()->m_myPlayerIndex);

					ChangeGameMode(0x1000);
				}
			}
		}
	} else {
		if (m_replaying) {
			m_replaying = false;

			WPuppet *puppet = CSceneManager::Instance()->GetPuppet("\xB0\xF8");
			if (puppet && puppet->GetRootBone())
				puppet->SetAlpha(0xFF);

			g_audio->StopSfx("shot_dist_count");
			SendMsg(this, "NPC", 0x183, 0, 0, 0, 0);

			// "\xC6\xDB\xC6\xC3\xBD\xC7\xC6\xD0": putt missed
			if (m_flag3F0)
				SendMsg(this, "Player", 0x2716, (intptr_t)"\xC6\xDB\xC6\xC3\xBD\xC7\xC6\xD0", 0, 0, 0);

			WVector toCup = ball.PreData(0)->m_pos - GetHoleCupPos();
			toCup.y = 0.0f;

			// "\xC6\xDB\xC6\xC3\xC8\xC4\xBD\xC7\xB8\xC1\xC6\xF7\xC1\xEE": the disappointed pose after a putt
			if (!PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ghostAwardBase.GetFlag(1) && !GolfBall().IsHoleIn() && !m_flag3F0
				&& GetPVS().GetGroundType(ball.PreData(0)->m_pos, true, NULL) == 2
				&& (GOLFDOC()->m_remainShotTime < 10000 || toCup.Magnitude() < 32.0f
					|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holeIn[GOLFDOC()->m_currentHole - 1] >= (unsigned char)2)) {
				PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ghostAwardBase.Enable(1);
				SendMsg(this, "Player", 0x2717, (intptr_t)"\xC6\xDB\xC6\xC3\xC8\xC4\xBD\xC7\xB8\xC1\xC6\xF7\xC1\xEE", 0, 0, 0);
			}

			toCup = GetHoleCupPos() - ball.m_pos;
			float distance = sqrtf(toCup.z * toCup.z + toCup.x * toCup.x);
			unsigned char ground = GetPVS().GetGroundType(ball.m_pos, true, NULL);

			if (ground == 3
				|| ground == 14) {
				SendMsg(this, "Gallery", 0x106, 0, 0, 0, 0);

				CVoiceItem::Instance()->PlaySound(CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId, (CVoiceItem::eWaveState)0x80, -1);
			} else if (score <= 2 && GetPVS().GetGroundType(ball.m_pos, true, NULL) == 2) {
				if (distance > 22.4f)
					SendMsg(this, "Gallery", 0x100, 0, 0, 0, 0);
				else
					SendMsg(this, "Gallery", 0x10B, 0, 0, 0, 0);
			}

			if (GolfClub().GetType() == 3 && distance < 3.2f && ball.m_eventPreDataIndex[3] < 0)
				SendMsg(this, "Gallery", 0x104, 0, 0, 0, 0);

			if (ball.m_landed) {
				if (!GOLFDOC()->m_clearedFlag158) {
					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_rivalLevel++;
					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;

					if (CSharedDoc::Instance()->m_roomType == 1)
						TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;
				}

				// "\xBF\xF6\xC5\xCD\xC7\xD8\xC0\xFA\xB5\xE5": water hazard
				if (GolfBall().m_lie == 3 || GolfBall().m_lie == 2) {
					SendMsg(this, "Letter", 0x18D, (intptr_t)"\xBF\xF6\xC5\xCD\xC7\xD8\xC0\xFA\xB5\xE5", 0, 0, 0);
					g_audio->PlaySfx("ball_fall_into_water", NULL, 0, NULL, NULL, 0.5f, 200.0f);
					CVoiceItem::Instance()->PlaySound(CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId, (CVoiceItem::eWaveState)0x100, -1);
				} else {
					SendMsg(this, "Letter", 0x18D, (intptr_t)"OB", 0, 0, 0);

					if (GolfBall().m_lie == 1) {
						g_audio->PlaySfx("ball_fall_into_water", NULL, 0, NULL, NULL, 0.5f, 200.0f);
						CVoiceItem::Instance()->PlaySound(CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId, (CVoiceItem::eWaveState)0x100, -1);
					} else {
						g_audio->PlaySfx("ball_OB_area", NULL, 0, NULL, NULL, 0.5f, 200.0f);
						CVoiceItem::Instance()->PlaySound(CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId, (CVoiceItem::eWaveState)0x10, -1);
					}
				}

				SendMsg(this, "Gallery", 0x105, 0, 0, 0, 0);
			} else if (ball.m_bounceCount == 0) {
				m_replayFlags &= ~4;

				if (!GOLFDOC()->m_clearedFlag158) {
					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_rivalLevel++;
					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;

					if (CSharedDoc::Instance()->m_roomType == 1)
						TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;
				}

				g_audio->PlaySfx("ball_OB_area", NULL, 0, NULL, NULL, 0.5f, 200.0f);
				CVoiceItem::Instance()->PlaySound(CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId, (CVoiceItem::eWaveState)0x10, -1);

				if (CSharedDoc::Instance()->m_roomType != 10)
					SendMsg(this, "Letter", 0x18D, (intptr_t)"Retry", 0, 0, 0);
			} else {
				PLAYER(GOLFDOC()->m_myPlayerIndex)->m_rivalLevel = 0;
			}

			if (m_pRule && m_pRule->CheckGiveUp() && (GetGameType() == 1 || GetGameType() == 3))
				SetHoleInPose(true);

			if (CSharedDoc::Instance()->m_roomType == 10) {
				CSharedDoc::Instance()->m_gameResultList[MyIndex()].m_approachDistanceCopy = CSharedDoc::Instance()->m_gameResultList[MyIndex()].m_approachDistance;
				SendMsg(this, "Score", 0x2A, 1, 0, 0, 0);
			}
		}

		if (ball.m_lie) {
			float fall = elapsed * 3.2f;

			if (ball.m_pos.y - fall > ball.GetRadius() + GetPVS().GetGroundPoint(ball.m_pos, true, NULL, false).y)
				ball.m_pos.y -= fall;
		}

		m_gameOverDelay += elapsed;

		if ((ball.m_landed && m_gameOverDelay > 2.0f) || (!ball.m_landed && m_gameOverDelay > 3.5f)) {
			bool next;

			if (GOLFDOC()->m_gameState == 15)
				SendMsg(this, "Screen", 0x1B8, (intptr_t)&next, 0, 0, 0);
			else
				next = true;

			if (next || m_gameOverDelay > (ball.m_landed ? 6.0f : 7.5f)) {
				if (m_replayFlags & 4) {
					m_replayFlags &= ~4;
					Replay(false);
					return;
				}

				if (m_forgetPending) {
					if (!m_forgetDlg) {
						m_forgetDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "flowerofforgetting", 0);
						// "The flower of forgetting you carry can erase a bad memory. Use it to take back the bad shot?"
						m_forgetDlg->SetMessage("\xB4\xE7\xBD\xC5\xC0\xCC \xB0\xA1\xC1\xF6\xB0\xED \xC0\xD6\xB4\xC2 \xB8\xC1\xB0\xA2\xC8\xAD\xB4\xC2 \\c0xffff0000\\c\xB3\xAA\xBB\xDB \xB1\xE2\xBE\xEF\xC0\xBB \xC1\xF6\xBF\xEF \xBC\xF6 \\c0xff000000\\c\xC0\xD6\xB4\xC2 \xB4\xC9\xB7\xC2\xC0\xCC \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9. \\c0xffff0000\\c\xB8\xC1\xB0\xA2\xC8\xAD\\c0xff000000\\c\xB8\xA6 \xBB\xE7\xBF\xEB\xC7\xCF\xBF\xA9 \xC0\xDF\xB8\xF8\xB5\xC8 \xBC\xA6\xC0\xBB \xB5\xC7\xB5\xB9\xB8\xAE\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);
						m_forgetDlg->Open((FRESH_RESULT_CALLBACK)&CGolfRule::OnForgetDlgResult, 3);

						m_forgetElapsed = 0;
						SendMsg(this, "Item", 0x196, 0, 0, 0, 0);
					}

					m_forgetElapsed += elapsed / CProjectG::Instance()->GetGameSpeed();

					if (m_forgetElapsed > 10.0f && m_forgetDlg)
						m_forgetDlg->Close(FrCANCEL, true);
					return;
				}

				UpdateShotResultStat();

				m_flag266 = false;
				m_gameOverDelay = 0;

				if (m_pRule && m_pRule->CheckGiveUp()) {
					SendMsg(this, "GuideLine", 0x1A2, 0, 0, 0, 0);
					RecordGiveUp(GOLFDOC()->m_myPlayerIndex);
					RecordScoreByPlayer(GOLFDOC()->m_myPlayerIndex);

					if (GetGameType() == 1 || GetGameType() == 3)
						ChangeGameMode(0x1000);
					else
						ChangeGameMode(0x80000);
				} else {
					if (GetGameType() == 1 || GetGameType() == 3)
						ChangeGameMode(0x1000);
					else
						ChangeGameMode(0x800);
				}
			}
		}
	}

	CCameraWork::Instance()->Process(elapsed, 0.0f);
}

// The screen capture state of CaptureProcess (no symbols): the round UI and
// head icon visibility to restore, the field of view in degrees of a
// cinematic capture, and the number of the capture series.
extern "C" { static bool data_a7df5c; }
extern "C" { static bool data_a7df5d; }
extern "C" { static int data_a7df60; }
extern "C" { static int data_a7df64; }

// golfrule.cpp:10566
// GM screen captures. Ctrl+FUNC_10 with the GM bit starts a cinematic series
// (a full turn of the camera, one frame per field of view); Ctrl+FUNC_10
// alone a high quality shot (m_captureGrid squared tiles, stitched together
// by IntergrateFile). While a capture runs, each call saves one frame.
void CGolfRule::CaptureProcess(void) {
	if (m_updateClipAlways == 0) {
		if ((g_input->Get("LCONTROL", true) || g_input->Get("RCONTROL", true))
			&& g_input->GetDown("FUNC_10", true) && (GOLFDOC()->m_gameMode & 0x400)) {
			mkdir("capture");
			mkdir("capture/cinematic");

			m_updateClipAlways = 2;
			m_value550 = 1;

			data_a7df64 = 1;
			data_a7df60 = (int)ceil(g_view->GetFOV() * 57.29578f);
			while (_access(MakeStr("capture/cinematic/fov(%d)_%03d_frame(1).bmp", data_a7df60, data_a7df64), 0) != -1)
				data_a7df64++;

			SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Player", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Caddie", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Ball", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x190, 0, 0, 0, 0);
			SendMsg(this, "Effect", 0x17B, 0, 0, 0, 0);

			CShadowManager::Instance()->SetActive(false);
			Wind().SetActive(false);
			CSceneManager::Instance()->SetLOD(false, false);
			GetTask()->SetPause(true);
		}

		if (!g_input->Get("LCONTROL", true) && !g_input->Get("RCONTROL", true)
			&& g_input->GetDown("FUNC_10", true)) {
			if (COption::Instance()->vGetTnLMode() == 0) {
				// "Only possible in HW TnL acceleration mode." / "Notice"
				MessageBoxA(0, "HW TnL \xB0\xA1\xBC\xD3 \xB8\xF0\xB5\xE5\xBF\xA1\xBC\xAD\xB8\xB8 \xB0\xA1\xB4\xC9\xC7\xD5\xB4\xCF\xB4\xD9.", "\xC1\xD6\xC0\xC7", 0);
				return;
			}

			mkdir("capture");
			mkdir("capture/high quality");

			m_updateClipAlways = 1;
			m_value550 = 0;

			data_a7df64 = 1;
			while (_access(MakeStr("capture/high quality/shot%03d.bmp", data_a7df64), 0) != -1)
				data_a7df64++;

			data_a7df5d = m_showGameUI;
			m_showGameUI = false;
			ToggleGameUI();

			SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);

			SendMsg(this, "ShotPreview", -1, (intptr_t)"ENABLE_DISPLAY", 0, 0, 0);

			if (CHeadIcon::IsInstantiated()) {
				data_a7df5c = CHeadIcon::Instance()->GetVisible();
				CHeadIcon::Instance()->Hide();
			}

			Wind().SetActive(false);
			CSceneManager::Instance()->SetLOD(false, false);
			GetTask()->SetPause(true);
		}
		return;
	}

	char fileName[MAX_PATH];
	Bitmap *bitmap = new Bitmap(g_last_screen_width, g_last_screen_height, 24);

	switch (m_updateClipAlways) {
	case 2: {
		sprintf(fileName, "capture/cinematic/fov(%d)_%03d_frame(%d).bmp", data_a7df60, data_a7df64, m_value550);

		bool finished = true;
		if (g_resrcmng->VideoReference()->Command(W_VDEV_CAPTURE_SCREEN, 0, (intptr_t)bitmap) == 1) {
			SaveBMP(fileName, bitmap);

			if (m_value550 * data_a7df60 < 360) {
				g_camera *= RotMat(g_view->GetFOV(), 1);
				g_camera.pivot = m_debugCameraEye;
				finished = false;
			}
		} else {
			// "%s screenshot failed." / "Warning"
			MessageBoxA(0, MakeStr("%s \xBD\xBA\xC5\xA9\xB8\xB0 \xC4\xB8\xC3\xC4\xBF\xA1 \xBD\xC7\xC6\xD0\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.", strrchr(fileName, '/')), "\xB0\xE6\xB0\xED", 0);
		}

		if (finished) {
			SendMsg(this, "Spot", 0x163, 0, 0, 0, 0);
			SendMsg(this, "Grid", 0x163, 0, 0, 0, 0);
			SendMsg(this, "Player", 0x163, 0, 0, 0, 0);
			SendMsg(this, "Caddie", 0x163, 0, 0, 0, 0);
			SendMsg(this, "Ball", 0x163, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x190, 1, 0, 0, 0);
			SendMsg(this, "Effect", 0x17A, 0, 0, 0, 0);

			CShadowManager::Instance()->SetActive(true);
			Wind().SetActive(true);
			CSceneManager::Instance()->SetLOD(true, false);
			GetTask()->SetPause(false);

			m_updateClipAlways = 0;

			m_previewCenter = g_camera.za + m_debugCameraEye;

			m_showGameUI = data_a7df5d;

			ToggleGameUI();

			SendMsg(this, "Spot", 0x163, 0, 0, 0, 0);

			SendMsg(this, "ShotPreview", -1, (intptr_t)"ENABLE_DISPLAY", 1, 0, 0);

			if (CHeadIcon::IsInstantiated() && data_a7df5c)
				CHeadIcon::Instance()->Show();

			m_value550 = -1;
			break;
		}
		m_value550++;
		break;
	}

	case 1: {
		if (m_value550 == 0)
			sprintf(fileName, "capture/high quality/shot%03d_origin.bmp", data_a7df64);
		else
			sprintf(fileName, "capture/high quality/shot%03d_%03d.bmp", data_a7df64, m_value550);

		bool finished = false;
		if (g_resrcmng->VideoReference()->Command(W_VDEV_CAPTURE_SCREEN, 0, (intptr_t)bitmap) == 1) {
			SaveBMP(fileName, bitmap);

			if (m_value550 >= m_captureGrid * m_captureGrid)
				finished = true;
		} else {
			// "%s screenshot failed." / "Warning"
			MessageBoxA(0, MakeStr("%s \xBD\xBA\xC5\xA9\xB8\xB0 \xC4\xB8\xC3\xC4\xBF\xA1 \xBD\xC7\xC6\xD0\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.", strrchr(fileName, '/')), "\xB0\xE6\xB0\xED", 0);
			finished = true;
		}

		if (finished) {
			m_showGameUI = data_a7df5d;

			ToggleGameUI();

			SendMsg(this, "Spot", 0x163, 0, 0, 0, 0);

			if (CHeadIcon::IsInstantiated() && data_a7df5c)
				CHeadIcon::Instance()->Show();

			Wind().SetActive(true);
			CSceneManager::Instance()->SetLOD(true, false);
			GetTask()->SetPause(false);
			g_view->xScaleProjMat(0, 1);

			m_updateClipAlways = 0;
			if (m_value550 >= m_captureGrid * m_captureGrid)
				IntergrateFile(MakeStr("capture/high quality/shot%03d", data_a7df64));

			for (int i = 1; i <= m_captureGrid * m_captureGrid; i++)
				DeleteFileA(MakeStr("capture/high quality/shot%03d_%03d.bmp", data_a7df64, i));

			m_value550 = -1;
			break;
		}

		m_value550++;
		g_view->xScaleProjMat(m_value550 - 1, m_captureGrid);
		break;
	}
	}

	delete bitmap;
}

// golfrule.cpp:10779
void CGolfRule::IntergrateFile(const char *name) {
	BITMAPINFOHEADER infoHeader = { sizeof(BITMAPINFOHEADER), 0, 0, 1, 24, 0, 0, 0, 0, 0, 0 };
	sBmpFileHeader fileHeader = { { 'B', 'M' }, sizeof(sBmpFileHeader), 0, 0, sizeof(sBmpFileHeader) + sizeof(BITMAPINFOHEADER) };

	FILE *fp = fopen(MakeStr("%s.bmp", name), "wb");
	if (fp == NULL)
		return;

	Bitmap **tiles = new Bitmap *[m_captureGrid];

	infoHeader.biWidth = (int)(m_captureGrid * g_view->GetWidth());
	infoHeader.biHeight = (int)(m_captureGrid * g_view->GetHeight());
	fwrite(&fileHeader, 1, sizeof(sBmpFileHeader), fp);
	fwrite(&infoHeader, 1, sizeof(BITMAPINFOHEADER), fp);

	for (int row = 0; row < m_captureGrid; row++) {
		for (int i = 0; i < m_captureGrid; i++) {
			tiles[i] = g_resrcmng->LoadBMP(MakeStr("%s_%03d.bmp", name, (m_captureGrid - row - 1) * m_captureGrid + i + 1));
			if (tiles[i] == NULL) {
				delete[] tiles;
				fclose(fp);
				return;
			}
		}

		for (int y = tiles[0]->Height() - 1; y >= 0; y--) {
			for (int col = 0; col < m_captureGrid; col++) {
				for (int x = 0; x < (int)tiles[col]->Width(); x++) {
					// The row address is spelled out, not GetVram(y): the two
					// tiles[col] reads keep &tiles[col] in ebx instead of fputc.
					unsigned char *pixel = tiles[col]->vram + tiles[col]->pitch * y + x * 3;
					fputc(pixel[0], fp);
					fputc(pixel[1], fp);
					fputc(pixel[2], fp);
				}
			}
		}

		for (int j = 0; j < m_captureGrid; j++)
			delete tiles[j];
	}

	delete[] tiles;

	fclose(fp);
}

// golfrule.cpp:10851
// The debug camera (game mode 0x400): the space bar fires a test sequence,
// PARTICLE_2 a test spray; the leave key restores the camera and the buttons
// ChangeGameMode hid. Otherwise the mouse turns the camera, W/S, A/D and Z/X
// move it (left Alt slows it down) and ROTATE_CW/CCW orbit m_orbitCenter.
void CGolfRule::DoDebugCamera(float elapsed) {
	static float s_forwardSpeed;
	static float s_strafeSpeed;
	static float s_riseSpeed;

	if (g_input->GetDown("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", true)) {
		CFxSequence *sequence = CFx::Instance()->OpenSequence("bullet.seq", true);
		if (sequence) {
			sequence->Pos() = g_camera.za * 10.0f + g_camera.pivot - g_camera.ya * 3.0f;
			sequence->m_matrix = RotMat((float)atan(0.0093749994f), 0) * g_camera;
		}
	}

	if (g_input->GetDown("PARTICLE_2", true)) {
		CFxSpray *spray = CFx::Instance()->OpenSpray("bullet.spr");
		if (spray) {
			spray->Pos() = g_camera.za * 10.0f + g_camera.pivot;
			spray->Vel() = RotVec(spray->Vel(), g_camera);
		}
	}

	if (g_input->GetDown("\xB5\xF0\xB9\xF6\xB1\xD7\xC5\xB0", true)) {
		GOLFDOC()->m_gameMode = m_debugCameraGameMode;
		g_camera = m_debugCameraStart;
		SendMsg(this, "Camera", 0x19A, 0, 0, 0, 0);
		s_forwardSpeed = s_strafeSpeed = s_riseSpeed = 0.0f;

		if (m_barButton[10])
			m_barButton[10]->SetVisible(true);

		if (m_myInfoBtn)
			m_myInfoBtn->SetVisible(true);

		if (m_optionBtn)
			m_optionBtn->SetVisible(true);

		ToggleCommandBtns(true);

		if (m_messengerWnd)
			m_messengerWnd->SetVisible(true);

		if (CSharedDoc::Instance()->m_value4DBD && m_messengerAlarmBtn) {
			m_messengerAlarmBtn->SetTopmost(true);
			m_messengerAlarmBtn->SetVisible(true);
		}

		if (CSharedDoc::Instance()->m_tutorialFinish && m_messengerGiftAlarmBtn) {
			m_messengerGiftAlarmBtn->SetTopmost(true);
			m_messengerGiftAlarmBtn->SetVisible(true);
		}
		return;
	}

	WVector delta;
	WVector dir;
	delta = g_input->GetMouseDelta();

	if (!WisEqual(delta, WVector::ZERO, 1e-5f)) {
		WVector move = g_camera.xa * delta.x - g_camera.ya * delta.y;
		m_previewCenter += move * 0.01f; m_previewCenter = (m_previewCenter - m_debugCameraEye).Normalize() + m_debugCameraEye;
	}

	elapsed = WisZero(elapsed, g_EPSILON)
		? 0.03f
		: elapsed;

	float speed = elapsed * 300.0f;

	float scale;
	if (g_input->Get("LALT", true))
		scale = 0.05f;
	else
		scale = 1.0f;

	if (g_input->Get('W') && !g_input->Get('S'))
		s_forwardSpeed += scale * speed;
	else if (g_input->Get('S') && !g_input->Get('W'))
		s_forwardSpeed -= scale * speed;
	else
		s_forwardSpeed = 0.0f;

	if (g_input->Get('A') && !g_input->Get('D'))
		s_strafeSpeed -= scale * speed;
	else if (g_input->Get('D') && !g_input->Get('A'))
		s_strafeSpeed += scale * speed;
	else
		s_strafeSpeed = 0.0f;

	if (g_input->Get('Z') && !g_input->Get('X'))
		s_riseSpeed += scale * speed;
	else if (g_input->Get('X') && !g_input->Get('Z'))
		s_riseSpeed -= scale * speed;
	else
		s_riseSpeed = 0.0f;

	if (m_msgValue430 > 0.0f) {
		if (s_forwardSpeed < -m_msgValue430) s_forwardSpeed = -m_msgValue430; else if (s_forwardSpeed > m_msgValue430) s_forwardSpeed = m_msgValue430;
		if (s_strafeSpeed < -m_msgValue430) s_strafeSpeed = -m_msgValue430; else if (s_strafeSpeed > m_msgValue430) s_strafeSpeed = m_msgValue430;
		if (s_riseSpeed < -m_msgValue430) s_riseSpeed = -m_msgValue430; else if (s_riseSpeed > m_msgValue430) s_riseSpeed = m_msgValue430;
	}

	if (g_input->Get("ROTATE_CW", true)) {
		if (g_input->GetDown("ROTATE_CW", true) && !g_input->GetDown("ROTATE_CCW", true))
			m_orbitCenter = m_msgValue434 * WVector(g_camera.za.x, 0.0f, g_camera.za.z).Normalize() + g_camera.pivot;

		m_debugCameraEye = (g_camera.pivot - m_orbitCenter) * RotMat(-(m_msgValue430 / m_msgValue434 * elapsed), 1) + m_orbitCenter;

		dir = m_orbitCenter - m_debugCameraEye;
		float upSq = g_camera.za.y * g_camera.za.y;
		dir.y = m_msgValue434 / (float)sqrt(1.0f - upSq) * g_camera.za.y;
		m_previewCenter = m_debugCameraEye + dir.Normalize();
	}

	if (g_input->Get("ROTATE_CCW", true)) {
		if (g_input->GetDown("ROTATE_CCW", true) && !g_input->GetDown("ROTATE_CW", true))
			m_orbitCenter = m_msgValue434 * WVector(g_camera.za.x, 0.0f, g_camera.za.z).Normalize() + g_camera.pivot;

		m_debugCameraEye = (g_camera.pivot - m_orbitCenter) * RotMat(m_msgValue430 / m_msgValue434 * elapsed, 1) + m_orbitCenter;

		dir = m_orbitCenter - m_debugCameraEye;
		float upSq = g_camera.za.y * g_camera.za.y;
		dir.y = m_msgValue434 / (float)sqrt(1.0f - upSq) * g_camera.za.y;
		m_previewCenter = m_debugCameraEye + dir.Normalize();
	}

	if (!WisZero(s_forwardSpeed, g_EPSILON)) {
		m_debugCameraEye += g_camera.za * s_forwardSpeed * elapsed;
		m_previewCenter += g_camera.za * s_forwardSpeed * elapsed;
	}

	if (!WisZero(s_strafeSpeed, g_EPSILON)) {
		m_debugCameraEye += g_camera.xa * s_strafeSpeed * elapsed;
		m_previewCenter += g_camera.xa * s_strafeSpeed * elapsed;
	}

	if (!WisZero(s_riseSpeed, g_EPSILON)) {
		m_debugCameraEye.y += s_riseSpeed * elapsed;
		m_previewCenter.y += s_riseSpeed * elapsed;
	}

	g_camera = MakeCameraMatrix(m_debugCameraEye, m_previewCenter);
}

// golfrule.cpp:11051
// Waiting for the shot: the rival shoots by himself, the top view takes the
// keys, otherwise the space bar starts the shot (or replays the saved one).
void CGolfRule::DoStand(float) {
	if (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_identity & 1) {
		CRival::Instance()->Shot();
		HitShot();
		CRival::Instance()->SetResult();
		return;
	}

	if (TopViewClick()) {
		if (CSharedDoc::Instance()->m_value4DBD) {
			if (m_messengerAlarmBtn) {
				m_messengerAlarmBtn->SetVisible(false);
				m_messengerAlarmBtn->SetTopmost(false);
			}
		}

		if (CSharedDoc::Instance()->m_tutorialFinish) {
			if (m_messengerGiftAlarmBtn) {
				m_messengerGiftAlarmBtn->SetVisible(false);
				m_messengerGiftAlarmBtn->SetTopmost(false);
			}
		}

		ChangeGameMode(0x200);
		return;
	}

	if (CSharedDoc::Instance()->m_playingMode != 2) {
		if (!IsMyTurn(false))
			return;
	}

	bool quitActive = CQuitWindow::Instance()->IsActive();
	bool canShot = !quitActive;
	if (GOLFDOC()->m_gameState < 15)
		canShot = m_shotKeyAlways || canShot;

	if (!g_input->Get("LSHIFT", true) && !g_input->Get("RSHIFT", true)
		&& g_input->GetDown("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", true) && canShot
		&& !GOLFDOC()->m_pause && !CChatMsg::Instance()->IsActive()
		&& GOLFDOC()->m_gameState >= 2 && GolfClub().BarIs(2)) {
		if (m_shotKeyLocked)
			return;

		_ImeReset();

		if (!m_shotPlayable) {
			g_audio->PlaySfx("play_unplayable", 0, 0, 0, 0, 0.5f, 200.0f);
			return;
		}

		int result;
		SendMsg(this, "Item", 0x199, (intptr_t)&result, 0, 0, 0);
		if (result != 3)
			return;

		SendMsg(this, "Player", 0x199, (intptr_t)&result, 0, 0, 0);
		if (result == 3)
			return;

		switch (CSharedDoc::Instance()->m_playingMode) {
		case 1:
			ChangeGameMode(0x10);
			break;
		case 2:
			GOLFDOC()->GetBar() = m_shotBar;
			SetEnvironmentVariableA();
			GOLFDOC()->MakeDefaultCamera(true);
			HitShot();
			break;
		}
	}
}

// golfrule.cpp:11150
// The course preview: the camera circles the hole until NUM_9/PAD_9 shows
// the players again and starts the round.
void CGolfRule::DoCoursePreview(float) {
	if (g_input->GetDown("NUM_9", true) || g_input->GetDown("PAD_9", true)) {
		CSceneManager::Instance()->SetVisible(MakeStr("PLAYER_%d", GOLFDOC()->m_myPlayerIndex), true);
		CSceneManager::Instance()->SetVisible("\xC5\xAC\xB7\xB4", true);
		CSceneManager::Instance()->SetVisible("\xC4\xB3\xB5\xF0", true);
		CShadowManager::Instance()->Enable(COption::Instance()->vIsShadowEnabled());

		SendMsg(this, "Score", 0x13B, 0, 0, 0, 0);

		ChangeGameMode(8);
		return;
	}

	// -20 and 10 degrees in radians.
	g_camera = RotMat(-0.34906584f, 0) * RotMat((m_gameOverDelay + m_modeTime) * 0.17453292f, 1);
	WVector tee = GOLFDOC()->GetHoleData().m_teePos;
	WVector cup = GOLFDOC()->GetHoleData().m_cupPos;
	g_camera.pivot = m_previewCenter - g_camera.za * ((cup - tee).Magnitude() * 0.5f + 640.0f);
}

// golfrule.cpp:11169
void CGolfRule::DoGetReadyToCaptureBG(float) {
	if (CCapturedBg::Instance()->IsCaptured()) {
		if (COption::Instance()->dGetClickSystem()) {
			if ((g_input->GetButton(LEFT_BUTTON) == 2) ^ g_input->GetUp("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9")) {
				CancelShot();
				return;
			}
		}

		ChangeGameMode(0x20);
	}
}

// The click a power gauge reports to the server (packet 0x14): the gauge
// stage (1 power, 2 accuracy, 3 a putt's accuracy) and the bar position.
#pragma pack(push, 1)
struct sGaugeClick {
	unsigned char m_stage;      // 0x0
	float m_position;           // 0x1
};
#pragma pack(pop)

// golfrule.cpp:11188
// The power gauge: read the click (space bar or mouse, by the click system
// option), let the tutorial drive or stop it, report clicks online, then run
// the putting or the drive gauge for the time since the last frame.
void CGolfRule::DoPowerGauge(float elapsed) {
	if (m_gaugePaused) {
		// The tutorial holds the gauge: keep the clock still.
		m_pauseTime = timeGetTime();
		IActor *tutorial = GetActor("Tutorial");
		if (tutorial)
			tutorial << MsgObject(0, 0x156, 0, 0, 0, 0, 0);
		return;
	}

	if (m_continueDlgOpen || m_netMsgWaitSend)
		return;

	bool click;
	if (COption::Instance()->dGetClickSystem()) {
		// Click system: the power stage stops on release, the accuracy stage on press.
		click = (GOLFDOC()->m_clockHold == 1 && ((g_input->GetButton(LEFT_BUTTON) == 2) ^ g_input->GetUp("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9")))
			|| (GOLFDOC()->m_clockHold == 2 && (g_input->GetDown("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", true) ^ (g_input->GetButton(LEFT_BUTTON) == 1)));
	} else {
		click = (g_input->GetDown("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", true) ^ (g_input->GetButton(LEFT_BUTTON) == 1)) != 0;
	}

	if (GalleryMode())
		click = false;

	if (IsMyTurn(false)) {
		float accuracy = GolfClub().GetType() == 3 ? 10.0f : GolfClub().GetNarrowAccuracy();
		if (GOLFDOC()->m_clockHold == 1 && click && GOLFDOC()->m_bar.m_accuracy < BAR_START + accuracy) {
			// Stopped before the bar's start: no shot.
			CancelShot();
			return;
		}

		if (m_shotKeyAlways && !g_input->GetDown("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", true))
			click = false;

		if (click) {
			if (OnlinePlay()) {
				sGaugeClick gaugeClick;
				gaugeClick.m_stage = GOLFDOC()->m_clockHold + 1;
				gaugeClick.m_position = GOLFDOC()->m_bar.m_prevAccuracy;

				if (GolfClub().GetType() == 3 && gaugeClick.m_stage == 2)
					gaugeClick.m_stage = 3;

				WSendPacket packet((enumClientPacket)0x14);
				packet.EncodeBuffer(&gaugeClick, 5);
				packet.Send(SEND_TO_DEFAULT);
			}
		} else if (m_shotKeyAlways && ((GOLFDOC()->m_clockHold == 1 && GOLFDOC()->m_bar.m_accuracy - m_fakeShotTargets[0] > 0.0f)
			|| (GOLFDOC()->m_clockHold == 2 && GOLFDOC()->m_bar.m_accuracy - m_fakeShotTargets[1] < 0.0f))) {
			// The tutorial's fake space bar: the bar reached its target.
			IActor *tutorial = GetActor("Tutorial");
			if (tutorial) {
				tutorial << MsgObject(0, 0x20F, 2, (intptr_t)&m_fakeShotTargets[GOLFDOC()->m_clockHold - 1], GOLFDOC()->m_clockHold, 0, 0);

				tutorial << MsgObject(0, 0x148, 0, 0, 0, 0, 0);

				if (g_input->GetDown("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", true) == 1)
					click = true;

				if (m_continueDlgOpen)
					return;
			}
		}

		if (GOLFDOC()->m_gameState > 12)
			CommandBox();
	} else if (m_shotReady) {
		m_shotReady = false;
		click = true;
	}

	if (m_gaugeTime == 0) {
		m_gaugeTime = timeGetTime();
		if (m_gaugeStartTime == 0)
			m_gaugeStartTime = m_gaugeTime;
	}

	unsigned long now;
	float delta;
	if (click) {
		if (g_input->GetDown("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", true))
			now = g_input->GetEventTime("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9");
		else
			now = g_input->GetEventTime(LEFT_BUTTON);

		now = timeGetTime();

		delta = (now - m_gaugeTime) * 0.001f;
	} else {
		now = timeGetTime();

		delta = (now - m_gaugeTime) * 0.001f;
	}

	m_gaugeTime = now;
	if (!IsMyTurn(false))
		delta *= 0.85f;

	if (GolfClub().GetType() == 3)
		PuttingGauge(delta, click);
	else
		DriveGauge(delta, click);

	if (GOLFDOC()->m_bar.m_state == 1)
		GOLFDOC()->m_bar.m_prevAccuracy = (float)floor(GOLFDOC()->m_bar.m_accuracy);
	else
		GOLFDOC()->m_bar.m_prevAccuracy = (float)ceil(GOLFDOC()->m_bar.m_accuracy);
}

// golfrule.cpp:11361
// Approach mode: once per reload, advance to the next hole of the order (back
// to the first after the last), reset the scene and announce the hole.
void CGolfRule::DoApproachNextHole(float) {
	if (OnlinePlay()) {
		if (GOLFDOC()->GetApproachReloaded() == true)
			return;

		SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Spin", 0x164, 0, 0, 0, 0);

		if (GetHoleIndex(GOLFDOC()->m_currentHole) == CSharedDoc::Instance()->m_holes) {
			GOLFDOC()->m_currentHole = 1;
			GOLFDOC()->m_approachMode = true;
			GOLFDOC()->m_pPolySoup->LoadMapCheckData(GOLFDOC()->m_currentHole);
		} else {
			GOLFDOC()->m_currentHole = CSharedDoc::Instance()->m_holeOrder[GetHoleIndex(GOLFDOC()->m_currentHole)];
			GOLFDOC()->m_approachMode = true;
			GOLFDOC()->m_pPolySoup->LoadMapCheckData(GOLFDOC()->m_currentHole);
		}

		m_continueDlgOpen = false;
		m_netMsgWaitSend = false;
		SendMsg(this, "GolfBg", 0x140, 0, 0, 0, 0);

		m_gameOverDelay = 0;
		CSharedDoc::Instance()->m_gameStartTime = 0;

		GolfBall().SetHoleIn(0);

		SendMsg(this, "Player", 0x163, 0, 0, 0, 0);
		SendMsg(this, "Mascot", 0x163, 0, 0, 0, 0);
		SendMsg(this, "Caddie", 0x163, 0, 0, 0, 0);

		SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Spin", 0x164, 0, 0, 0, 0);

		SendMsg(this, "Screen", 0x1A8, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1A4, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1A5, 0, 0, 0, 0);

		GOLFDOC()->RefreshPlayerNum();
		SendHoleData();

		AfxGetTask()->GetActor("GolfRule") << MsgObject(0, 0x99, 0, 0, 0, 0, 0);

		GOLFDOC()->SetApproachReloaded(true);
	}
}

// golfrule.cpp:11422
void CGolfRule::CommandBox(void) {
	if (GOLFDOC()->m_clockHold >= 2
		&& !(BAR_LENGTH * 0.5f + BAR_START < GOLFDOC()->GetBar().m_accuracy)
		&& !(BAR_LENGTH * 0.8f + BAR_START > GOLFDOC()->GetBar().m_power)
		&& !(GOLFDOC()->m_commandArrows.size() > 0 && (GOLFDOC()->m_commandArrows.back() & 0x70))) {
		unsigned long command = 0;
		int count = 0;
		if (g_input->Get("\xBB\xF3", true)) {
			count++;
			command |= 1;
		}

		if (g_input->Get("\xC7\xCF", true)) {
			count++;
			command |= 2;
		}

		if (g_input->Get("\xC1\xC2", true)) {
			count++;
			command |= 4;
		}

		if (g_input->Get("\xBF\xEC", true)) {
			count++;
			command |= 8;
		}

		if (count == 0) {
			m_commandTime = count;
			return;
		}

		if (count == 1) {
			if (m_commandTime > 0) {
				if (GOLFDOC()->m_gameState <= 12 || GOLFDOC()->m_gameState >= 15) {
					if (timeGetTime() - m_commandTime <= 100)
						return;

					command = 0x10;
					GOLFDOC()->m_commandArrows.push_back(command);
					return;
				} else {
					if (timeGetTime() - m_commandTime == 0)
						return;

					command = 0x10;
				}
			} else {
				m_commandTime = timeGetTime();
				GOLFDOC()->m_commandArrows.push_back(command);
				return;
			}
		} else {
			command = 0x40;
		}

		GOLFDOC()->m_commandArrows.push_back(command);
	}
}

// golfrule.cpp:11491
void CGolfRule::ToggleCommandBtns(bool show) {
	if (m_commandBtns[0])
		m_commandBtns[0]->SetVisible(show);

	if (m_commandBtns[1])
		m_commandBtns[1]->SetVisible(show);

	if (m_commandBtns[2])
		m_commandBtns[2]->SetVisible(show);

	if (m_commandBtns[3])
		m_commandBtns[3]->SetVisible(show);
}

// golfrule.cpp:11506
// Grades the bar click from its distance to the pang-ya area (4) and the
// club's narrow (3) and wide (2) accuracy zones, plays the grade's sound and
// reports whether the shot was good.
bool CGolfRule::GetBarCondition(void) {
	GOLFDOC()->GetBar().m_position = GOLFDOC()->GetBar().m_prevAccuracy;

	GOLFDOC()->GetBar().m_state = 0;

	float error = Abs(GOLFDOC()->GetBar().m_position - 140.0f);
	float pangYaArea = CPowerGauge::Instance()->GetPangYaArea();

	bool halfArea = false;
	if (IsLocalContent((localContentType_t)0x6E)) {
		if ((int)(pangYaArea + pangYaArea) % 2 == 1) {
			halfArea = true;
			pangYaArea -= 0.5;
		}
	}

	if (Abs(GOLFDOC()->GetBar().m_position - GOLFDOC()->GetBar().m_startPosition) < pangYaArea + 0.5f) {
		GOLFDOC()->GetBar().m_accuracyGrade = 4;
		g_audio->PlaySfx(g_plSound[1], 0, 0, 0, 0, 0.5f, 200.0f);
	} else if (IsLocalContent((localContentType_t)0x6E) && halfArea == true
		&& (pangYaArea + 1.5f) * -1.0f < GOLFDOC()->GetBar().m_position - GOLFDOC()->GetBar().m_startPosition
		&& GOLFDOC()->GetBar().m_position - GOLFDOC()->GetBar().m_startPosition < 0.0f) {
		GOLFDOC()->GetBar().m_accuracyGrade = 4;
		g_audio->PlaySfx(g_plSound[1], 0, 0, 0, 0, 0.5f, 200.0f);
	} else if (GolfClub().GetNarrowAccuracy() + 0.5f > error) {
		GOLFDOC()->GetBar().m_accuracyGrade = 3;
		g_audio->PlaySfx(g_plSound[2], 0, 0, 0, 0, 0.5f, 200.0f);
	} else if (GolfClub().GetWideAccuracy() + 0.5f > error) {
		GOLFDOC()->GetBar().m_accuracyGrade = 2;
		g_audio->PlaySfx(g_plSound[3], 0, 0, 0, 0, 0.5f, 200.0f);
	} else {
		GOLFDOC()->GetBar().m_accuracyGrade = 1;
		g_audio->PlaySfx(g_plSound[4], 0, 0, 0, 0, 0.5f, 200.0f);
	}

	if (GOLFDOC()->GetBar().m_accuracyGrade == 4 || GOLFDOC()->GetBar().m_accuracyGrade == 3 || GOLFDOC()->GetBar().m_accuracyGrade == 2)
		return true;
	else
		return false;
}

// golfrule.cpp:11626
void CGolfRule::CalcInputCommand(void) {
	int count = 0;
	m_inputCommandHead = m_inputCommandTail = 0;
	if (GOLFDOC()->m_commandArrows.size() > 0 && !(GOLFDOC()->m_commandArrows.back() & 0x40)) {
		for (std::list<unsigned long>::iterator it = GOLFDOC()->m_commandArrows.begin(); it != GOLFDOC()->m_commandArrows.end(); ++it) {
			if (count < 2) {
				if (m_inputCommandHead > 0 && *it != 0x10)
					m_inputCommandHead <<= 8;
				m_inputCommandHead |= *it;
			} else if (count > 1) {
				if (m_inputCommandTail > 0 && *it != 0x10)
					m_inputCommandTail <<= 8;
				m_inputCommandTail |= *it;
			}

			count++;
		}
	}
}

// golfrule.cpp:11682
// Turns the input command into the ball's special shot flags: the head picks
// the shot (0x10/0x20/0x40, kept in m_value1E8 as the fallback), the tail the
// spin/curve variant, both only on a perfect-grade impact.
void CGolfRule::SetSpecialShot(void) {
	m_value1E8 = 0;

	switch (m_inputCommandHead) {
	case 0x11:
		if (GOLFDOC()->GetBar().m_accuracyGrade == 4)
			GolfBall().m_powerFlags = 1;
		break;
	case 0x12:
		if (GOLFDOC()->GetBar().m_accuracyGrade == 4)
			GolfBall().m_powerFlags = 2;
		break;
	case 0x14:
		if (GOLFDOC()->GetBar().m_accuracyGrade == 4)
			GolfBall().m_powerFlags = 4;
		break;
	case 0x18:
		if (GOLFDOC()->GetBar().m_accuracyGrade == 4)
			GolfBall().m_powerFlags = 8;
		break;
	case 0x102:
	case 0x112:
	case 0x1102:
	case 0x1112:
		m_value1E8 = 0x10;
		if (GOLFDOC()->GetBar().m_accuracyGrade == 4) {
			if (m_inputCommandTail == 0x11)
				GolfBall().m_powerFlags = 0x11;
			else if (m_inputCommandTail == 0x12)
				GolfBall().m_powerFlags = 0x12;
			else if (m_inputCommandTail == 0x14)
				GolfBall().m_powerFlags = 0x14;
			else if (m_inputCommandTail == 0x18)
				GolfBall().m_powerFlags = 0x18;
			else
				GolfBall().m_powerFlags = 0x10;
		} else {
			GolfBall().m_powerFlags = 0x10;
		}
		break;
	case 0x801:
	case 0x811:
	case 0x1801:
	case 0x1811:
		m_value1E8 = 0x20;
		if (GOLFDOC()->GetBar().m_accuracyGrade == 4) {
			if (m_inputCommandTail == 0x11)
				GolfBall().m_powerFlags = 0x21;
			else if (m_inputCommandTail == 0x12)
				GolfBall().m_powerFlags = 0x22;
			else if (m_inputCommandTail == 0x14)
				GolfBall().m_powerFlags = 0x24;
			else if (m_inputCommandTail == 0x18)
				GolfBall().m_powerFlags = 0x28;
			else
				GolfBall().m_powerFlags = 0x20;
		} else {
			GolfBall().m_powerFlags = 0x20;
		}
		break;
	case 0x802:
	case 0x812:
	case 0x1802:
	case 0x1812:
		m_value1E8 = 0x40;
		if (GOLFDOC()->GetBar().m_accuracyGrade == 4) {
			if (m_inputCommandTail == 0x11)
				GolfBall().m_powerFlags = 0x41;
			else if (m_inputCommandTail == 0x12)
				GolfBall().m_powerFlags = 0x42;
			else if (m_inputCommandTail == 0x14)
				GolfBall().m_powerFlags = 0x44;
			else if (m_inputCommandTail == 0x18)
				GolfBall().m_powerFlags = 0x48;
			else
				GolfBall().m_powerFlags = 0x40;
		} else {
			GolfBall().m_powerFlags = 0x40;
		}
		break;
	}
}

// golfrule.cpp:11806
// Drops the special shot flags the club cannot play, keeps a spin or curve
// shot only when the spin/curve gauges agree, and adds the pending flags.
void CGolfRule::CheckSpecialShotSpinAndCurve(void) {
	if (GolfBall().m_powerFlags != 0) {
		if (GolfBall().m_powerFlags & 0x70) {
			if (GolfClub().PowerShotType() == 0 || ((GolfBall().m_powerFlags & 0x20) && GolfClub().GetType() != 0))
				GolfBall().m_powerFlags = 0;

			if (GolfClub().PowerShotType() == 0 || ((GolfBall().m_powerFlags & 0x40) && GolfClub().GetType() != 0))
				GolfBall().m_powerFlags = 0;
		}

		float curve;
		float spin;
		SendMsg(this, "Spin", 0x162, (intptr_t)&curve, (intptr_t)&spin, 0, 0);

		if (GolfBall().m_powerFlags & 1) {
			if (spin > 0.0f || WAbsBySign(spin) < 1e-5f)
				GolfBall().m_powerFlags = m_value1E8;
		} else if (GolfBall().m_powerFlags & 2) {
			if (spin < 0.0f || WisZero(spin, 1e-5f))
				GolfBall().m_powerFlags = m_value1E8;
		} else if (GolfBall().m_powerFlags & 4) {
			if (!WisEqual(curve, -1.0f, 1e-5f))
				GolfBall().m_powerFlags = m_value1E8;
		} else if (GolfBall().m_powerFlags & 8) {
			if (!WisEqual(curve, 1.0f, 1e-5f))
				GolfBall().m_powerFlags = m_value1E8;
		}

		if (GolfBall().m_powerFlags & 4)
			GolfBall().m_lieSlope = -1.5f;
		else if (GolfBall().m_powerFlags & 8)
			GolfBall().m_lieSlope = 1.5f;
	}

	GolfBall().m_powerFlags |= m_pendingPowerFlags;
	m_pendingPowerFlags = 0;
}

// golfrule.cpp:11864
// The drive's power/accuracy bar. The bar speed is the club's less the
// player's control, raised in the rough (ground 3/14) and the bunker (7) by
// club type, shaped by the bar position and halved by some shot items and
// the tutorial. The first click (m_clockHold 1) stops the power, the second
// (2) grades the accuracy, reads the arrow command into the ball's special
// shot flags and hits the shot; then the bar moves (m_state 1 up, -1 back)
// or waits (0).
void CGolfRule::DriveGauge(float elapsed, bool click) {
	float barSpeed = GolfClub().GetBarSpeed();
	float speed = barSpeed - (int)PLAYER(GOLFDOC()->m_myPlayerIndex)->m_control * 10.0f;

	unsigned char groundType = GetPVS().GetGroundType(GolfBall().m_pos, false, NULL);

	if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId != 0x18000028) {
		if (groundType == 3 || groundType == 14) {
			switch (GolfClub().GetType()) {
			case 0:
				if (CSharedDoc::Instance()->m_roomType == 3)
					speed *= 1.7f;
				else
					speed *= 1.5f;
				break;
			case 1:
				if (CSharedDoc::Instance()->m_roomType == 3)
					speed *= 1.5f;
				else
					speed *= 1.3f;
				break;
			case 2:
				if (CSharedDoc::Instance()->m_roomType == 3)
					speed *= 1.3f;
				else
					speed *= 1.2f;
				break;
			}
		} else if (groundType == 7) {
			switch (GolfClub().GetType()) {
			case 0:
				// DRAFT: cases 0 and 1 are tail-merged into the first switch's cases 1
				// and 2 (identical code), so their own lines (11904-11918) have no
				// marks; written as those bodies.
				if (CSharedDoc::Instance()->m_roomType == 3)
					speed *= 1.5f;
				else
					speed *= 1.3f;
				break;
			case 1:
				if (CSharedDoc::Instance()->m_roomType == 3)
					speed *= 1.3f;
				else
					speed *= 1.2f;
				break;
			case 2:
				if (CSharedDoc::Instance()->m_roomType == 3)
					speed *= 1.1f;
				break;
			}
		}
	}

	if (GOLFDOC()->GetBar().m_state == 1) {
		if (COption::Instance()->dGetClickSystem()) {
			// Min is wutil.h's Min<float>, inlined here.
			float pos = Min((GOLFDOC()->GetBar().m_accuracy - 105.0f) / 395.0f, 0.5f);
			speed *= pos * pos * 2.0f + 0.5f;
		}
	} else if (GOLFDOC()->m_clockHold > 1) {
		if (GOLFDOC()->GetBar().m_stopped)
			speed *= 0.9375f;
		else {
			float pos = (GOLFDOC()->GetBar().m_power - BAR_START) / BAR_LENGTH;

			if (pos < 0.5f)
				speed *= 0.875f;
			else
				speed = ((pos - 0.5f) * (pos - 0.5f) + 1.75f) * speed * 0.5f;
		}
	}

	// DRAFT: the first five PLAYER calls are inlined (direct GetPlayer calls),
	// the last three call the out-of-line PLAYER: the inliner's budget, not
	// the source; if they do not reproduce, try GOLFDOC()->GetPlayer(...) for
	// the first five.
	if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x1800000B
		|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000008
		|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000003
		|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x1800000F
		|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000010
		|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000011
		|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000012
		|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x1800001F)
		speed *= 0.5f;

	if (m_shotKeyAlways && GOLFDOC()->m_gameState != 4)
		speed *= 0.5f;

	if (click) {
		switch (GOLFDOC()->m_clockHold) {
		case 1:
			GOLFDOC()->m_clockHold++;

			if (GOLFDOC()->GetBar().m_impact > BAR_START) {
				GOLFDOC()->GetBar().m_prevAccuracy = GOLFDOC()->GetBar().m_impact;
				GOLFDOC()->GetBar().m_accuracy = GOLFDOC()->GetBar().m_impact;

				// DRAFT: neg/sbb/neg/sub 1 on WisEqual's result; WisEqual gets
				// the address of the unnamed .rdata 500.0f (BAR_END, as line 3476
				// of GolfRule.cpp passes it), not a stack temporary.
				GOLFDOC()->GetBar().m_state = WisEqual(GOLFDOC()->GetBar().m_impact, BAR_END, 1e-5f) ? 0 : -1;
			}

			GOLFDOC()->GetBar().m_power = GOLFDOC()->GetBar().m_prevAccuracy;

			// DRAFT: the `if` and `m_state = -1` may sit on 12036-12040 (scheduled
			// under 12041's mark; the PlaySfx pushes common to both branches are
			// hoisted above the test).
			if (GOLFDOC()->GetBar().m_state == 0) {
				GOLFDOC()->GetBar().m_state = -1;
				g_audio->PlaySfx(g_plSound[0], 0, 0, 0, 0, 0.5f, 200.0f);
				GOLFDOC()->GetBar().m_stopped = true;
				SendMsg(this, "Screen", 0x1AB, 0, 0, 0, 0);
			} else {
				g_audio->PlaySfx(g_plSound[2], 0, 0, 0, 0, 0.5f, 200.0f);
			}
			break;
		case 2:
			// DRAFT: the failing test jumps straight to the epilogue (the
			// `return` below has no mark); `!(Min(...) > m_accuracy)` and
			// `<=` give the same x87 test.
			if (GOLFDOC()->GetBar().m_state == -1 && Min(GOLFDOC()->GetBar().m_power, 240.0f) > GOLFDOC()->GetBar().m_accuracy) {
				GOLFDOC()->m_clockHold++;

				if (IsLocalContent((localContentType_t)0x50)) {
					bool barGood = GetBarCondition();
					if (barGood == true) {
						CalcInputCommand();
						SetSpecialShot();

						CheckSpecialShotSpinAndCurve();
					}
					HitShot();
					return;
				}

				GOLFDOC()->GetBar().m_position = GOLFDOC()->GetBar().m_prevAccuracy;

				GOLFDOC()->GetBar().m_state = 0;

				float error = Abs(GOLFDOC()->GetBar().m_position - 140.0f);
				float pangYaArea = CPowerGauge::Instance()->GetPangYaArea();

				if (Abs(GOLFDOC()->GetBar().m_position - GOLFDOC()->GetBar().m_startPosition) < pangYaArea + 0.5f) {
					unsigned long command = 0;
					if (GOLFDOC()->m_commandArrows.size() > 0 && !(GOLFDOC()->m_commandArrows.back() & 0x40)) {
						for (std::list<unsigned long>::iterator it = GOLFDOC()->m_commandArrows.begin(); it != GOLFDOC()->m_commandArrows.end(); ++it) {
							if (command > 0 && *it != 0x10)
								command <<= 8;
							command |= *it;
						}
					}

					switch (command) {
					case 0x11:
						GolfBall().m_powerFlags = 1;
						break;
					case 0x12:
						GolfBall().m_powerFlags = 2;
						break;
					case 0x14:
						GolfBall().m_powerFlags = 4;
						break;
					case 0x18:
						GolfBall().m_powerFlags = 8;
						break;
					case 0x102:
						GolfBall().m_powerFlags = 0x10;
						break;
					case 0x801:
						GolfBall().m_powerFlags = 0x20;
						break;
					case 0x10211:
						GolfBall().m_powerFlags = 0x11;
						break;
					case 0x10212:
						GolfBall().m_powerFlags = 0x12;
						break;
					case 0x10214:
						GolfBall().m_powerFlags = 0x14;
						break;
					case 0x10218:
						GolfBall().m_powerFlags = 0x18;
						break;
					case 0x80111:
						GolfBall().m_powerFlags = 0x21;
						break;
					case 0x80112:
						GolfBall().m_powerFlags = 0x22;
						break;
					case 0x80114:
						GolfBall().m_powerFlags = 0x24;
						break;
					case 0x80118:
						GolfBall().m_powerFlags = 0x28;
						break;
					case 0x802:
						GolfBall().m_powerFlags = 0x40;
						break;
					case 0x80211:
						GolfBall().m_powerFlags = 0x41;
						break;
					case 0x80212:
						GolfBall().m_powerFlags = 0x42;
						break;
					case 0x80214:
						GolfBall().m_powerFlags = 0x44;
						break;
					case 0x80218:
						GolfBall().m_powerFlags = 0x48;
						break;
					}

					if (GolfBall().m_powerFlags != 0) {
						if (GolfBall().m_powerFlags & 0x70) {
							if (GolfClub().PowerShotType() == 0 || ((GolfBall().m_powerFlags & 0x20) && GolfClub().GetType() != 0))
								GolfBall().m_powerFlags = 0;

							if (GolfClub().PowerShotType() == 0 || ((GolfBall().m_powerFlags & 0x40) && GolfClub().GetType() != 0))
								GolfBall().m_powerFlags = 0;
						}

						float curve;
						float spin;
						SendMsg(this, "Spin", 0x162, (intptr_t)&curve, (intptr_t)&spin, 0, 0);

						// DRAFT: unlike CheckSpecialShotSpinAndCurve, flag 1 also
						// calls WisZero (one tail-merged call serves flags 1 and 2).
						if (GolfBall().m_powerFlags & 1) {
							if (spin > 0.0f || WisZero(spin, 1e-5f))
								GolfBall().m_powerFlags = 0;
						} else if (GolfBall().m_powerFlags & 2) {
							if (spin < 0.0f || WisZero(spin, 1e-5f))
								GolfBall().m_powerFlags = 0;
						} else if (GolfBall().m_powerFlags & 4) {
							if (!WisEqual(curve, -1.0f, 1e-5f))
								GolfBall().m_powerFlags = 0;
						} else if (GolfBall().m_powerFlags & 8) {
							if (!WisEqual(curve, 1.0f, 1e-5f))
								GolfBall().m_powerFlags = 0;
						}

						if (GolfBall().m_powerFlags & 4)
							GolfBall().m_lieSlope = -1.5f;
						else if (GolfBall().m_powerFlags & 8)
							GolfBall().m_lieSlope = 1.5f;
					}

					// DRAFT: 12266-12270 are scheduled together (one 12270 mark
					// after the OR); the split over lines is a guess.
					GolfBall().m_powerFlags |= m_pendingPowerFlags;
					m_pendingPowerFlags = 0;

					GOLFDOC()->GetBar().m_accuracyGrade = 4;
					g_audio->PlaySfx(g_plSound[1], 0, 0, 0, 0, 0.5f, 200.0f);
				} else if (GolfClub().GetNarrowAccuracy() + 0.5f > error) {
					GOLFDOC()->GetBar().m_accuracyGrade = 3;
					g_audio->PlaySfx(g_plSound[2], 0, 0, 0, 0, 0.5f, 200.0f);
				} else if (GolfClub().GetWideAccuracy() + 0.5f > error) {
					GOLFDOC()->GetBar().m_accuracyGrade = 2;
					g_audio->PlaySfx(g_plSound[3], 0, 0, 0, 0, 0.5f, 200.0f);
				} else {
					GOLFDOC()->GetBar().m_accuracyGrade = 1;
					g_audio->PlaySfx(g_plSound[4], 0, 0, 0, 0, 0.5f, 200.0f);
				}

				GolfBall().m_powerFlags |= m_pendingPowerFlags;
				m_pendingPowerFlags = 0;

				HitShot();
			}
			return;
		}
	}

	switch (GOLFDOC()->GetBar().m_state) {
	case -1:
		GOLFDOC()->GetBar().m_accuracy -= speed * elapsed;

		if (GOLFDOC()->GetBar().m_accuracy < 105.0f)
			CancelShot();
		break;
	case 1:
		GOLFDOC()->GetBar().m_accuracy += speed * elapsed;

		if (GOLFDOC()->GetBar().m_accuracy > 500.0f) {
			GOLFDOC()->GetBar().m_accuracy = 500.0f;
			m_gameOverDelay = 0.0f;

			GOLFDOC()->GetBar().m_state = 0;
		}
		break;
	case 0:
		m_gameOverDelay += elapsed;
		if (GOLFDOC()->m_clockHold > 0 && GOLFDOC()->m_clockHold < 3) {
			if ((COption::Instance()->GetOption()->m_powerGauge != 0 ? 0.03f : 0.02f) < m_gameOverDelay)
				GOLFDOC()->GetBar().m_state = -1;
		}
		break;
	}

	if (m_shotKeyAlways && GOLFDOC()->m_gameState >= 13 && GOLFDOC()->GetBar().m_state == -1 && !GOLFDOC()->m_clearedFlag15A) {
		IActor *tutorial = GetActor("Tutorial");
		if (tutorial)
			tutorial << MsgObject(0, 0x155, (int)GOLFDOC()->GetBar().m_accuracy, 0, 0, 0, 0);
	}
}

// golfrule.cpp:12345
void CGolfRule::PuttingGauge(float elapsed, bool click) {
	float barSpeed = GolfClub().GetBarSpeed();

	float pos = (GOLFDOC()->GetBar().m_accuracy - BAR_START) / BAR_LENGTH;
	float speed = (pos * pos + 1.0f) * barSpeed * 0.5f;

	if (click) {
		switch (GOLFDOC()->m_clockHold) {
		case 1:
			if (GOLFDOC()->GetBar().m_impact > BAR_START) {
				GOLFDOC()->GetBar().m_prevAccuracy = GOLFDOC()->GetBar().m_impact;
				GOLFDOC()->GetBar().m_accuracy = GOLFDOC()->GetBar().m_impact;
				if (WAbsBySign(GOLFDOC()->GetBar().m_impact - 500.0f) < 1e-5f)
					GOLFDOC()->GetBar().m_state = 0;
			}
			g_audio->PlaySfx(GOLFDOC()->GetBar().m_state == 0 ? g_plSound[0] : g_plSound[3], 0, 0, 0, 0, 0.5f, 200.0f);

			GOLFDOC()->m_clockHold++;
			GOLFDOC()->GetBar().m_power = GOLFDOC()->GetBar().m_prevAccuracy;

			GOLFDOC()->GetBar().m_state = 0;
			GOLFDOC()->GetBar().m_accuracyGrade = 4;

			HitShot();
			return;
		}
	}

	switch (GOLFDOC()->GetBar().m_state) {
	case 1:
		GOLFDOC()->GetBar().m_accuracy += speed * elapsed;

		if (GOLFDOC()->GetBar().m_accuracy > 500.0f) {
			GOLFDOC()->GetBar().m_accuracy = 500.0f;
			GOLFDOC()->GetBar().m_state = 0;
			m_gameOverDelay = 0.0f;
		}
		break;
	case 0:
		if (GOLFDOC()->m_clockHold > 0) {
			m_gameOverDelay += elapsed;

			if ((COption::Instance()->GetOption()->m_powerGauge != 0 ? 0.03f : 0.02f) < m_gameOverDelay)
				CancelShot();
		}
		break;
	}
}

// golfrule.cpp:12413
void CGolfRule::DoNavigateCamera(float) {
	if (g_input->GetDown("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", true)) {
		CMouseCursor::Instance()->Store();
		ChangeGameMode(8);
		SendMsg(this, "Camera", 0xF0, 0, 0, 0, 0);
	}

	if (TopViewClick())
		ChangeGameMode(0x200);
}

// golfrule.cpp:12426
void CGolfRule::DoTopViewCamera(float elapsed) {
	if (TopViewClick()) {
		if (m_topViewMode) {
			g_camera = GOLFDOC()->MakeTopViewCameraMatrix(false);
			return;
		}

		if (WSingleton<CSharedDoc>::Instance()->m_value4DBD && m_messengerAlarmBtn) {
			m_messengerAlarmBtn->SetTopmost(true);
			m_messengerAlarmBtn->SetVisible(true);
		}

		if (WSingleton<CSharedDoc>::Instance()->m_tutorialFinish && m_messengerGiftAlarmBtn) {
			if (GOLFDOC()->m_gameState == 15) {
				m_messengerGiftAlarmBtn->SetTopmost(true);
				m_messengerGiftAlarmBtn->SetVisible(true);
			} else {
				m_messengerGiftAlarmBtn->SetTopmost(false);
				m_messengerGiftAlarmBtn->SetVisible(false);
			}
		}

		ChangeGameMode(8);
		WSingleton<CSceneManager>::Instance()->UpdateFog(false);
	}
}

// golfrule.cpp:12462
bool CGolfRule::StandViewClick(void) {
	return g_input->GetDown("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", true) ? true : false;
}

// golfrule.cpp:12470
// The top view key or a free left click: picks the top view mode (0 off,
// 1 PAD_0, 2 shift/mouse on my turn, 3 control); the same mode again, or
// PAD_0 from a shift/control mode, turns it off.
bool CGolfRule::TopViewClick(void) {
	if (CQuitWindow::Instance()->IsActive() || GOLFDOC()->m_gameState < 1 || !CItemWindow::IsClosed() || CUserInfo::Instance()->IsVisible()
		|| (GOLFDOC()->m_gameState == 15 && g_pFresh->GetManager()->HasValidWindow() && m_kbdManualDlg == NULL))
		return false;

	if (g_input->GetDown("PAD_0", true) || g_input->GetDown("NUM_0", true)
		|| (g_input->GetDown("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", true) && (GOLFDOC()->GameMode() & 0x200))
		|| (CMouseCursor::Instance()->GetArea() == 0 && g_input->GetButton(RIGHT_BUTTON) == 1 && CMessengerInfo::Instance()->m_gameFocus == true)) {
		if ((CSharedDoc::Instance()->m_currentChannel.flags & 0x80) && !IsMyTurn(true)) {
			if (CHelpMsg::IsInstantiated())
				CHelpMsg::Instance()->SetHelpMsg(0x16, 0);
			return false;
		}

		int oldMode = m_topViewMode;

		if (g_input->GetDown("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", true) && (GOLFDOC()->GameMode() & 0x200)) {
			m_topViewMode = 0;
		} else if (g_input->Get("LCONTROL", true) || g_input->Get("RCONTROL", true)) {
			m_topViewMode = 3;
		} else if ((g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true) || g_input->Get(1)) && IsMyTurn(true)) {
			m_topViewMode = 2;
		} else if (g_input->GetDown("PAD_0", true) || g_input->GetDown("NUM_0", true)) {
			m_topViewMode = 1;
		} else {
			m_topViewMode = 0;
		}

		if (m_topViewMode == oldMode || (m_topViewMode == 1 && oldMode >= 2 && oldMode <= 3))
			m_topViewMode = 0;

		return true;
	}

	return false;
}

// golfrule.cpp:12549
// Adds the local player's finished shot to his game statistics: the longest
// drive, out of bounds, holed putts and chip-ins with their lengths, hole in
// ones and albatrosses, bunker and fairway landings and the total distance
// (in yards: 0.3125 per unit, measured flat from where the shot started).
void CGolfRule::UpdateShotResultStat(void) {
	if (GOLFDOC()->m_approachMode
		|| (IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK))
		return;

	CGolfBall &ball = GolfBall();

	WVector delta = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos - ball.PreData(0)->m_pos;
	float distance;

	if (IsLocalContent((localContentType_t)0x2F) && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000028) {
		distance = 0.0f;
	} else {
		distance = sqrtf(delta.x * delta.x + delta.z * delta.z) * 0.3125f;
	}

	if (ball.m_holeIn == 1 || ball.m_bounceCount > 0)
		CSharedDoc::Instance()->m_gameStatistics[GOLFDOC()->m_myPlayerIndex].bestDrive = Max(distance, CSharedDoc::Instance()->m_gameStatistics[GOLFDOC()->m_myPlayerIndex].bestDrive);

	if (ball.m_landed) {
		CSharedDoc::Instance()->m_gameStatistics[GOLFDOC()->m_myPlayerIndex].obCount++;
	} else {
		if (ball.IsHoleIn()) {
			if (GolfClub().GetType() == 3) {
				CSharedDoc::Instance()->m_gameStatistics[GOLFDOC()->m_myPlayerIndex].puttsSunk++;
				CSharedDoc::Instance()->m_gameStatistics[GOLFDOC()->m_myPlayerIndex].longestPutt = Max(distance, CSharedDoc::Instance()->m_gameStatistics[GOLFDOC()->m_myPlayerIndex].longestPutt);
			} else {
				CSharedDoc::Instance()->m_gameStatistics[GOLFDOC()->m_myPlayerIndex].longestChipIn = Max(distance, CSharedDoc::Instance()->m_gameStatistics[GOLFDOC()->m_myPlayerIndex].longestChipIn);
			}
			CSharedDoc::Instance()->m_gameStatistics[GOLFDOC()->m_myPlayerIndex].holedShots++;

			if ((CSharedDoc::Instance()->m_roomType != 1 && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] == (unsigned char)1)
				|| (CSharedDoc::Instance()->m_roomType == 1 && TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] == (unsigned char)1)) {
				CSharedDoc::Instance()->m_gameStatistics[GOLFDOC()->m_myPlayerIndex].holeInOnes++;
			} else if (CSharedDoc::Instance()->m_roomType != 1
				&& (unsigned char)PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] - GOLFDOC()->GetHoleData().m_par == -3) {
				CSharedDoc::Instance()->m_gameStatistics[GOLFDOC()->m_myPlayerIndex].albatrosses++;
			}
		}

		unsigned char ground = GetPVS().GetGroundType(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos, true, NULL);

		if (ground == 3) {
			CSharedDoc::Instance()->m_gameStatistics[GOLFDOC()->m_myPlayerIndex].bunkerShots++;
		} else if ((ground == 1 || ground == 2) && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] == (unsigned char)1) {
			CSharedDoc::Instance()->m_gameStatistics[GOLFDOC()->m_myPlayerIndex].fairways++;
		}

		CSharedDoc::Instance()->m_gameStatistics[GOLFDOC()->m_myPlayerIndex].totalDistance += (int)(sqrtf(delta.x * delta.x + delta.z * delta.z) * 0.3125f);
	}

	GOLFDOC()->m_statUpdated = true;
}

// golfrule.cpp:12626
void CGolfRule::SendHoleStat(void) {
	if (OnlinePlay() && !(unsigned char)(CSharedDoc::Instance()->m_userInfo.identity >> 1 & 1)) {
		bool send;
		unsigned char index;

		switch (CSharedDoc::Instance()->m_roomType) {
		case 0:
		case 3:
		case 7:
			send = GOLFDOC()->m_myPlayerIndex == MyIndex();
			index = GOLFDOC()->m_myPlayerIndex;
			break;
		case 1:
			send = GOLFDOC()->m_myPlayerIndex == MyIndex() || (PLAYER(MyIndex())->m_partnerIndex != 0xFF && GOLFDOC()->m_myPlayerIndex == PLAYER(MyIndex())->m_partnerIndex);
			index = MyIndex();
			break;
		case 4:
		case 5:
		case 6:
		case 9:
		case 10:
			send = true;
			index = 0;
			break;
		default:
			return;
		}

		if (send) {
			WSendPacket packet(GCT_HOLE_STAT);

			sPlayerData *pPlayer = GOLFDOC()->GetPlayer(index);
			CSharedDoc::Instance()->m_gameStatistics[index].playTime = pPlayer->m_playTime / 1000;
			packet.EncodeBuffer(&CSharedDoc::Instance()->m_gameStatistics[index], sizeof(sPangYaUserStatistics));
			packet.Send(SEND_TO_DEFAULT);
		}
	}
}

// golfrule.cpp:12677
void CGolfRule::SetNetMsgWait(bool send, int type, char *msg, float timeout) {
	if (send) {
		m_netMsgWaitSend = true;
		m_netMsgWaitType = type;
		m_netMsgWaitTimeout = timeout;
		m_netMsgWait = msg;
		m_netMsgSendTime = GetTickCount();

		g_iSendPacketGR = g_iSendPacketType;
		g_iRecvPacketGR = 0;
		g_iRecvPacketType = 0;
	} else {
		m_netMsgWaitSend = false;
		m_netMsgWaitRecv = msg;
		m_netMsgRecvTime = GetTickCount();

		g_iRecvPacketGR = g_iRecvPacketType;
	}
}

// golfrule.cpp:12706
void CGolfRule::CheckNetMsgWait_Send(char *msg) {
	if (m_netMsgWait && !_strcmpi(m_netMsgWait, msg)) {
		g_iSendPacketGR = g_iSendPacketType;
	}
}

// golfrule.cpp:12714
void CGolfRule::CheckNetMsgWait_Recv(char *msg) {
	if (m_netMsgWait && !_strcmpi(m_netMsgWait, msg)) {
		g_iRecvPacketGR = g_iRecvPacketType;
	}
}

// golfrule.cpp:12722
// After the shot the camera returns to its default: online the shot is
// reported to the server (packet 0x1C, once), the effects of the shot are
// cleared, the ball is put where it stopped, and the next turn starts (game
// mode 8 for the same player before game state 15 outside state 8, otherwise
// the turn passes to m_nextTurnPlayer). The tutorial gets its next step, and
// a replay in playback resumes on its recorded position.
void CGolfRule::DoToDefaultCamera(float elapsed) {
	if (OnlinePlay() && !m_goStopSent && CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_GAME) {
		m_goStopSent = true;

		if (!IsMassGame())
			SetNetMsgWait(true, 0x31BA, "DoToDefaultCamera", 20.0f);

		WSendPacket packet((enumClientPacket)0x1C);
		EncodePacket((enumClientPacket)0x1C, packet);
		packet.Send(SEND_TO_DEFAULT);

		CheckNetMsgWait_Send("DoToDefaultCamera");
	}

	if (GetGameType() == 10 && !m_continueDlgOpen) {
		m_continueDlgOpen = true;
		m_replaying = false;
		m_goStopSent = false;
		m_flag266 = false;
		m_canFastForward = false;

		SendMsg(this, "Effect", 0x177, 0, 0, 0, 0);
		SendMsg(this, "Effect", 0x178, 0, 0, 0, 0);
		SendMsg(this, "Letter", 0x18C, 0, 0, 0, 0);
		SendMsg(this, "Trajectory", 0x18C, 0, 0, 0, 0);
		CFx::Instance()->ClearAll(true);

		g_view->SetFOV(0.95993108f);

		if (IsLocalContent((localContentType_t)0x2F)) {
			if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000028)
				SendMsg(this, "Player", 0x203, 0, 0, 0, 0);
		}

		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId = 0;

		CCameraWork::Instance()->SelectCamera(0xD0);

		SendMsg(this, "Player", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Caddie", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Mascot", 0x164, 0, 0, 0, 0);
		SendMsg(this, "Bubble", 0x164, 0, 0, 0, 0);

		CHeadIcon::Instance()->Hide();

		CProjectG::Instance()->SetGameSpeed(1.0f);
	} else {
		CHeadIcon::Instance()->Show();
	}

	if (CSharedDoc::Instance()->m_roomType == 10)
		CCameraWork::Instance()->Process(elapsed, 0.0f);
	else
		CCameraWork::Instance()->InputProcess(elapsed);

	if (m_continueDlgOpen || m_netMsgWaitSend || m_goStopDlg)
		return;

	if (IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK) {
		if (CSharedDoc::Instance()->m_replayState == 7) {
			g_input->SetFakeInput("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", false);
		} else if (CSharedDoc::Instance()->m_replayState == 12) {
			if (!GolfBall().IsHoleIn()) {
				CSharedDoc::Instance()->m_replayState = 0;
				g_input->SetFakeInput("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", false);
			}
		} else if (CSharedDoc::Instance()->m_replayState == 10) {
			CSharedDoc::Instance()->m_replayState = 0;
			g_input->SetFakeInput("\xBD\xBA\xC6\xE4\xC0\xCC\xBD\xBA\xB9\xD9", false);
		} else if (CSharedDoc::Instance()->m_replayState != 14) {
			return;
		}
	}

	m_replaying = false;
	m_goStopSent = false;
	m_flag266 = false;
	m_canFastForward = false;

	SendMsg(this, "Effect", 0x177, 0, 0, 0, 0);
	SendMsg(this, "Effect", 0x178, 0, 0, 0, 0);
	SendMsg(this, "Letter", 0x18C, 0, 0, 0, 0);
	SendMsg(this, "Trajectory", 0x18C, 0, 0, 0, 0);
	CFx::Instance()->ClearAll(true);

	g_view->SetFOV(0.95993108f);

	CGolfBall &ball = GolfBall();
	CGolfBall::sExpected *start = ball.PreDataToWrite(0);

	if (IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK) {
		ball.m_preDataIndex = -1;
		start->m_pos = m_shotBallPos;
		ball.m_pos = m_shotBallPos;
		ball.m_groundArea = GetPVS().GetMeshIndex(ball.m_pos, false);

		PLAYER(0)->m_powerShotLevel = m_powerShotLevel;
		SendMsg(this, "Ball", 0x1ED, 0, 0, 0, 0);

		GOLFDOC()->MakeDefaultCamera(true);

		if (CSharedDoc::Instance()->m_replayStartB == 0)
			ChangeGameMode(2);
		else
			ChangeGameMode(8);
		return;
	}

	if (IsLocalContent((localContentType_t)0x2F)) {
		if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000028)
			SendMsg(this, "Player", 0x203, 0, 0, 0, 0);
	}

	unsigned long shotItem = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId;
	PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId = 0;

	if (GOLFDOC()->m_gameState < 15)
		SendMsg(this, "Screen", 0x1B5, 0, 0, 0, 0);

	ball.m_preDataIndex = -1;
	ball.m_preDataCount = 0;
	ball.m_lieSlope = 0;
	ball.m_lieAngle = 0;
	ball.m_rotation.Reset();

	if (GOLFDOC()->m_gameState < 15
		&& GOLFDOC()->m_gameState != 8) {
		if (m_shotKeyAlways) {
			IActor *tutorial = GetActor("Tutorial");
			if (tutorial)
				tutorial << MsgObject(NULL, 0x148, 0, 0, 0, 0, 0);
		} else {
			WVector toTarget = GOLFDOC()->m_target.m_pos - ball.m_pos;
			toTarget.y = 0.0f;
			float distance = toTarget.Magnitude();

			IActor *tutorial = GetActor("Tutorial");
			if (!tutorial)
				return;

			if (GOLFDOC()->m_gameState == 2 || GOLFDOC()->m_gameState == 10 || GOLFDOC()->m_gameState == 11
				|| GOLFDOC()->m_gameState == 13 || GOLFDOC()->m_gameState == 14) {
				float scale = GOLFDOC()->m_gameState == 2 ? 2.0f : 1.0f;

				if (distance < 44.8f * scale) {
					SendMsg(this, "GolfRule", 0x88, 1, 0, 0, 0);

					switch (GOLFDOC()->m_gameState) {
					case 2:
					case 10:
					case 11:
						tutorial << MsgObject(NULL, 0x149, 0, 0, 0, 0, 0);
						return;
					case 13:
						if ((GolfBall().m_powerFlags & 1) || (GolfBall().m_powerFlags & 2)) {
							tutorial << MsgObject(NULL, 0x149, 0, 0, 0, 0, 0);
							return;
						}
						break;
					case 14:
						if ((GolfBall().m_powerFlags & 4) || (GolfBall().m_powerFlags & 8)) {
							tutorial << MsgObject(NULL, 0x149, 0, 0, 0, 0, 0);
							return;
						}
						break;
					}
				}
			} else if (GOLFDOC()->m_gameState == 3) {
				if (GolfClub().PowerShotType() == 1 && GOLFDOC()->GetBar().m_accuracyGrade > 1) {
					SendMsg(this, "GolfRule", 0x88, 1, 0, 0, 0);
					tutorial << MsgObject(NULL, 0x149, 0, 0, 0, 0, 0);
					return;
				}
			} else if (GOLFDOC()->m_gameState == 5) {
				if (shotItem == 0x18000004 && GOLFDOC()->GetBar().m_accuracyGrade > 1) {
					SendMsg(this, "GolfRule", 0x88, 1, 0, 0, 0);
					tutorial << MsgObject(NULL, 0x149, 0, 0, 0, 0, 0);
					return;
				}
			}
		}

		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel = 0;
		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos = ball.PreData(0)->m_pos;
		ball.m_pos = ball.PreData(0)->m_pos;
		ball.m_groundArea = GetPVS().GetMeshIndex(ball.m_pos, false);

		if (GOLFDOC()->m_gameState == 2 || GOLFDOC()->m_gameState == 6 || GOLFDOC()->m_gameState == 4
			|| GOLFDOC()->m_gameState == 10) {
			SendMsg(this, "Screen", 0x1A3, 0, 0, 0, 0);
			GOLFDOC()->m_cameraData.m_yaw = m_shotYaw;
			GolfClub().SetPowerShot(0);
			SetAngle(GOLFDOC()->m_cameraData.m_yaw);
			SendMsg(this, "Player", 0xE2, 0, 0, 0, 0);

			if (GOLFDOC()->m_gameState == 2)
				SendMsg(this, "Caddie", 0x13, 0, 0, 0, 0);

			float spot = 0.0f;
			SendMsg(this, "Screen", 0x1AA, (intptr_t)&spot, 0, 0, 0);
		} else {
			SetEnvironmentVariableA();
		}

		CMouseCursor::Instance()->Reset(true);

		ChangeTurn();
		ChangeGameMode(8);
	} else {
		SendMsg(this, "Clock", 0x271A, 0, 0, 0, 0);

		if (CSharedDoc::Instance()->m_roomType == 1) {
			TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_clubHolder = GOLFDOC()->m_myPlayerIndex;
			TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_clubHolderClub = GolfClub().GetType();
		}

		GOLFDOC()->m_myPlayerIndex = m_nextTurnPlayer;

		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;
		start->m_pos = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos;
		ball.m_pos = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos;
		ball.m_groundArea = GetPVS().GetMeshIndex(ball.m_pos, false);

		if (IsLocalContent((localContentType_t)0x2F) && PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000028)
			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel = 0;

		if (CSharedDoc::Instance()->m_roomType == 1)
			TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;

		SetEnvironmentVariableA();

		if (CSharedDoc::Instance()->m_roomType == 1) {
			TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_clubHolder = GOLFDOC()->m_myPlayerIndex;
			TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_clubHolderClub = GolfClub().GetType();
		}

		if (SetTurnChangeEvent()) {
			ChangeTurn();
			GOLFDOC()->MakeDefaultCamera(true);
			ChangeGameMode(4);

			if (IsMassGame())
				SendMsg(this, "Score", 0x134, (unsigned char)PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] - GOLFDOC()->GetHoleData().m_par, 0, 0, 0);
		} else {
			ReadyForNextTurn();
		}
	}

	SendMsg(this, "Screen", 0x1A5, 0, 0, 0, 0);

	if (!m_shotKeyAlways) {
		IActor *tutorial = GetActor("Tutorial");
		if (tutorial) {
			if (GOLFDOC()->m_gameState != 8)
				tutorial << MsgObject(NULL, 0x154, 0, 0, 0, 0, 0);
		}
	}
}

// golfrule.cpp:13099
// Announces the turn change: in match play, a shot that can win the hole or
// the match; otherwise a birdie or eagle chance, the approach range, or the
// waiting sign when it is not the player's turn. Returns whether the
// player's caddie is set for the turn.
bool CGolfRule::SetTurnChangeEvent(void) {
	sDetermineWhetherToEnableFiledItem determineFieldItem;
	CGolfBall &ball = GolfBall();
	bool result = false;
	float letterTime = 2.0f;

	if (OnlinePlay())
		m_flagCutin = false;

	if (CSharedDoc::Instance()->m_roomType == 1) {
		if (ball.m_groundType == 2) {
			int team = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute;
			bool winningShot = false;
			bool matchShot = false;
			sTeamData *myTeam = GOLFDOC()->GetTeam(team);
			int diff = myTeam->m_resetCount - GOLFDOC()->GetTeam(1 - team)->m_resetCount;
			if (GOLFDOC()->GetTeam(1 - team)->m_state == 1) {
				if (GOLFDOC()->GetTeam(team)->m_holes[GOLFDOC()->m_currentHole - 1] < GOLFDOC()->GetTeam(1 - team)->m_holes[GOLFDOC()->m_currentHole - 1]) {
					winningShot = true;
					if (diff + 1 > CSharedDoc::Instance()->m_holes - GetHoleIndex(GOLFDOC()->m_currentHole))
						matchShot = true;
				} else {
					if (diff > CSharedDoc::Instance()->m_holes - GetHoleIndex(GOLFDOC()->m_currentHole))
						matchShot = true;
				}
			} else {
				if (!(GOLFDOC()->GetTeam(team)->m_holes[GOLFDOC()->m_currentHole - 1] > GOLFDOC()->GetTeam(1 - team)->m_holes[GOLFDOC()->m_currentHole - 1])) {
					winningShot = true;
					if (diff + 1 > CSharedDoc::Instance()->m_holes - GetHoleIndex(GOLFDOC()->m_currentHole))
						matchShot = true;
				} else {
					if (diff > CSharedDoc::Instance()->m_holes - GetHoleIndex(GOLFDOC()->m_currentHole))
						matchShot = true;
				}
			}

			if (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_caddieInfo.id != 0)
				result = true;

			if (IsMyTurn(true)) {
				if (matchShot) {
					SendMsg(this, "Letter", 0x18D, (intptr_t)"matchshot", (intptr_t)&letterTime, 0, 0);
					return result;
				} else if (winningShot) {
					SendMsg(this, "Letter", 0x18D, (intptr_t)"winningshot", (intptr_t)&letterTime, 0, 0);
					return result;
				}
			}
		}
	} else if (CSharedDoc::Instance()->m_roomType == 3) {
		if (ball.m_groundType == 2) {
			bool matchShot = false;
			bool winningShot = false;
			int diff = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ghostResult - PLAYER(1 - GOLFDOC()->m_myPlayerIndex)->m_ghostResult;
			if (PLAYER(1 - GOLFDOC()->m_myPlayerIndex)->m_state == 1) {
				if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] < PLAYER(1 - GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1]) {
					winningShot = true;
					if (diff + 1 > CSharedDoc::Instance()->m_holes - GetHoleIndex(GOLFDOC()->m_currentHole))
						matchShot = true;
				} else {
					if (diff > CSharedDoc::Instance()->m_holes - GetHoleIndex(GOLFDOC()->m_currentHole))
						matchShot = true;
				}
			} else {
				if (!(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] > PLAYER(1 - GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1])) {
					winningShot = true;
					if (diff + 1 > CSharedDoc::Instance()->m_holes - GetHoleIndex(GOLFDOC()->m_currentHole))
						matchShot = true;
				} else {
					if (diff > CSharedDoc::Instance()->m_holes - GetHoleIndex(GOLFDOC()->m_currentHole))
						matchShot = true;
				}
			}

			if (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_caddieInfo.id != 0)
				result = true;

			if (IsMyTurn(true)) {
				if (matchShot) {
					SendMsg(this, "Letter", 0x18D, (intptr_t)"matchshot2", (intptr_t)&letterTime, 0, 0);
					return result;
				} else if (winningShot) {
					SendMsg(this, "Letter", 0x18D, (intptr_t)"winningshot2", (intptr_t)&letterTime, 0, 0);
					return result;
				}
			}
		}
	} else if (CSharedDoc::Instance()->m_roomType == 7) {
		if (ball.m_groundType == 2) {
			if (GOLFDOC()->m_clearedShort194 == 2 && CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_caddieInfo.id != 0)
				result = true;

			if (IsMyTurn(true)) {
				if (GOLFDOC()->m_clearedShort194 == 1) {
					SendMsg(this, "Letter", 0x18D, (intptr_t)"matchshot3", (intptr_t)&letterTime, 0, 0);
				} else if (GOLFDOC()->m_clearedShort194 == 2) {
					SendMsg(this, "Letter", 0x18D, (intptr_t)"winningshot3", (intptr_t)&letterTime, 0, 0);

					CLetter::sScore score;
					score.m_value = (int)GOLFDOC()->m_maxPangPerLoser;
					score.m_scale = 0.8f;
					score.m_color = 0xFFFFDE44;
					SendMsg(this, "Letter", 0x191, (intptr_t)"winningshot3", (intptr_t)&score, 0, 0);
				} else {
					SendMsg(this, "Letter", 0x18D, (intptr_t)"\xC2\xF7\xB7\xCA\xC7\xA5\xBD\xC3", (intptr_t)&letterTime, 0, 0);
				}
			}
		} else if (IsMyTurn(true) && GOLFDOC()->m_approachMode) {
			if (GolfClub().GetApproachMode() <= 2)
				SendMsg(this, "Letter", 0x18D, (intptr_t)"\xBE\xEE\xC7\xC1\xB7\xCE\xC4\xA1" "30_\xBF\xAC\xC0\xE5", (intptr_t)&letterTime, 0, 0);
			else if (GolfClub().GetApproachMode() <= 3)
				SendMsg(this, "Letter", 0x18D, (intptr_t)"\xBE\xEE\xC7\xC1\xB7\xCE\xC4\xA1" "60_\xBF\xAC\xC0\xE5", (intptr_t)&letterTime, 0, 0);
		}
		return result;
	} else if (CSharedDoc::Instance()->m_roomType == 10) {
		return false;
	} else {
		if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] < GOLFDOC()->GetHoleData().m_par && ball.m_groundType == 2) {
			result = CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_caddieInfo.typeId != 0;

			if (IsMyTurn(true)) {
				switch ((unsigned char)PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] - GOLFDOC()->GetHoleData().m_par) {
				case -2:
					SendMsg(this, "Letter", 0x18D, (intptr_t)"\xC0\xCC\xB1\xDB\xC2\xF9\xBD\xBA", (intptr_t)&letterTime, 0, 0);
					return result;
				case -1:
					SendMsg(this, "Letter", 0x18D, (intptr_t)"\xB9\xF6\xB5\xF0\xC2\xF9\xBD\xBA", (intptr_t)&letterTime, 0, 0);
					return result;
				}
			}
		}
	}

	if (CSharedDoc::Instance()->m_playingMode == 1) {
		if (IsMyTurn(true)) {
			if (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_identity & 1)
				SendMsg(this, "Letter", 0x18D, (intptr_t)"AI", 0, 0, 0);
			else if ((ball.m_groundType == 2 || GolfClub().GetApproachMode() == 4) && GOLFDOC()->m_gameState != 9)
				SendMsg(this, "Letter", 0x18D, (intptr_t)"\xC2\xF7\xB7\xCA\xC7\xA5\xBD\xC3", (intptr_t)&letterTime, 0, 0);
			else if (GolfClub().GetApproachMode() <= 2)
				SendMsg(this, "Letter", 0x18D, (intptr_t)"\xBE\xEE\xC7\xC1\xB7\xCE\xC4\xA1" "30", (intptr_t)&letterTime, 0, 0);
			else if (GolfClub().GetApproachMode() <= 3)
				SendMsg(this, "Letter", 0x18D, (intptr_t)"\xBE\xEE\xC7\xC1\xB7\xCE\xC4\xA1" "60", (intptr_t)&letterTime, 0, 0);

			SendMsg(this, "Score", 0x12F, 0, 0, 0, 0);
			if (g_pFresh)
				g_pFresh->GetManager()->CloseForm(false);
		} else {
			SendMsg(this, "Spin", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Letter", 0x18D, (intptr_t)"Wait", 0, 0, 0);
		}
	}

	return result;
}

// golfrule.cpp:13322
// The server's shot result (CGolfTask's message 0x62, CGolfDoc's
// m_shotResultMsg) overrides the local player's ball: its position, power
// shot level and pangs (copied to the team partner and the team's ball in a
// team game), how the shot ended, and online whether the next shot shows a
// cut-in (a hole-in or a ball stopped within 15 units of the cup, off the
// green, with an equipped cut-in item). A missing result is reported.
void CGolfRule::CorrectShotResult(void) {
	if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_state == 3 || GOLFDOC()->m_shotResultMsg[GOLFDOC()->m_myPlayerIndex].m_resultType == 0) {
		CProjectG::Instance()->ReportError(1, "CorrectShotResult_%d_%d_%d(%d_%d)", g_iSendPacketGR, g_iRecvPacketGR, GOLFDOC()->m_gameMode,
			CSharedDoc::Instance()->m_roomInfo.m_roomNumber, CSharedDoc::Instance()->m_userInfo.guid);
		return;
	}

	for (unsigned char i = 0; i < 3; i++)
		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos.p[i] = GOLFDOC()->m_shotResultMsg[GOLFDOC()->m_myPlayerIndex].m_pos.p[i];

	PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel = GOLFDOC()->m_shotResultMsg[GOLFDOC()->m_myPlayerIndex].m_powerShotLevel;

	PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ghostAward = GOLFDOC()->m_shotResultMsg[GOLFDOC()->m_myPlayerIndex].m_pang;
	PLAYER(GOLFDOC()->m_myPlayerIndex)->m_extraPang = GOLFDOC()->m_shotResultMsg[GOLFDOC()->m_myPlayerIndex].m_clearBonus;

	if (CSharedDoc::Instance()->m_roomType == 1) {
		if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_partnerIndex != 0xFF) {
			PLAYER(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_partnerIndex)->m_ballPos = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos;
			PLAYER(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_partnerIndex)->m_powerShotLevel = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel;
		}

		TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_ballPos = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos;
		TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_powerShotLevel = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel;
	}

	switch (GOLFDOC()->m_shotResultMsg[GOLFDOC()->m_myPlayerIndex].m_resultType) {
	case 3:
		GolfBall().m_landed = true;
		GolfBall().SetHoleIn(0);
		break;
	case 5:
		GolfBall().m_bounceCount = 0;
		GolfBall().SetHoleIn(0);
		break;
	case 4:
		GolfBall().SetHoleIn(1);
		break;
	case 2:
		GolfBall().SetHoleIn(0);
		break;
	default:
		GolfBall().m_holeIn = 0;
		break;
	}

	if (OnlinePlay()) {
		if (GolfClub().GetType() != 3 && GolfClub().GetType() != 2
			&& PLAYER(GOLFDOC()->m_myPlayerIndex)->m_uid == CSharedDoc::Instance()->m_userInfo.guid
			&& GolfBall().m_groundType != 2 && CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_GAME) {
			if (GolfBall().IsHoleIn()) {
				m_flagCutin = true;
			} else {
				WVector toCup;
				toCup = GetHoleCupPos() - GolfBall().m_pos;
				toCup.y = 0.0f;

				if (toCup.Magnitude() < 15.0f)
					m_flagCutin = true;
			}
		}

		int cutinMode = CSharedDoc::Instance()->m_gameMode;
		if (GOLFDOC()->m_gameState < 15)
			cutinMode = 6;

		if (CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK)
			m_flagCutin = false;

		if (m_flagCutin) {
			if (CSharedDoc::Instance()->m_mySkinList.size() > 0) {
				for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_mySkinList.begin(); it != CSharedDoc::Instance()->m_mySkinList.end(); ++it) {
					if (S5::ISCutinItem(it->m_typeId)) {
						IFF_STRUCT::S5::sCutinInformation *info = ItemManager()->FindCutinInformation(it->m_typeId);

						if (info && (info->m_condition & 8)) {
							SendMsg(this, "Cutin", 0x78, 1, 0, 0, 0);
							S5::UTIL::SendCutInPacket(CSharedDoc::Instance()->m_myUID, 8, cutinMode, CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_characterInfo.typeId);
							return;
						}
					}

					m_flagCutin = false;
				}
			} else {
				m_flagCutin = false;
			}
		}
	}
}

// golfrule.cpp:13473
// Sets up the shot environment: a replayed shot restores its yaw, power shot
// and club; otherwise the rival or the auto-target picks the target point.
void CGolfRule::SetEnvironmentVariableA(void) {
	SendMsg(this, "Screen", 0x1A3, 0, 0, 0, 0);

	if (IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == 2) {
		GOLFDOC()->GetCameraData().m_yaw = m_shotYaw;
		m_targetPoint = GolfBall().m_pos + RotMat(GOLFDOC()->GetCameraData().m_yaw, 1).za;
		GolfClub().SetPowerShot(m_shotPowerShot);
		GolfClub().SelectBestClub();
		GolfClub().SetClub(m_clubName);
	} else {
		if (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_identity & 1) {
			CRival::Instance()->SetVariable(GOLFDOC());
			m_targetPoint = CRival::Instance()->GetPos();
		} else {
			if (GOLFDOC()->m_gameState == 2 || GOLFDOC()->m_gameState == 6)
				GolfClub().SelectBestClub(m_targetPoint);
			else
				GolfClub().SelectBestClub();

			if (GOLFDOC()->m_gameState == 15 || GOLFDOC()->m_gameState == 8)
				m_targetPoint = GOLFDOC()->m_pAutoTarget->GetPoint(0);
		}

		GOLFDOC()->GetCameraData().m_yaw = CalcDeltaAngle(m_targetPoint - GolfBall().m_pos, WVector::UNIT_POS_Z);
		GolfClub().SetPowerShot(0);
	}

	SetAngle(GOLFDOC()->GetCameraData().m_yaw);
	SendMsg(this, "Player", 0xE2, 0, 0, 0, 0);
	SendMsg(this, "Caddie", 0x13, 0, 0, 0, 0);

	float spot = 0.0f;

	if (IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == 2) {
		if (m_shotPowerShot == 1 || m_shotPowerShot == 2)
			spot = 1.0f;
	}

	SendMsg(this, "Spot", 0xF5, 0, 0, 0, 0);
	SendMsg(this, "Screen", 0x1AA, (intptr_t)&spot, 0, 0, 0);
}

// golfrule.cpp:13531
void CGolfRule::SetAngle(float angle) {
	SendMsg(this, "Camera", 0x2718, (intptr_t)&angle, 0, 0, 0);
	SendMsg(this, "Spot", 0x2718, (intptr_t)&angle, 0, 0, 0);
	SendMsg(this, "Screen", 0x2718, (intptr_t)&angle, 0, 0, 0);
	SendMsg(this, "Grid", 0x2718, (intptr_t)&angle, 0, 0, 0);
	SendMsg(this, "Caddie", 0x2718, (intptr_t)&angle, 0, 0, 0);
}

// golfrule.cpp:13540
WMatrix CGolfRule::BankCamera(float angle) {
	return RotMat(angle, 2) * g_camera;
}

// golfrule.cpp:13545
void CGolfRule::Replay(bool keep) {
	CGolfBall &ball = GolfBall();
	const CGolfBall::sExpected *pFirst = ball.PreData(0);

	m_replayDone = false;
	m_replaying = true;

	ball.m_preDataIndex = 0;
	ball.m_pos = pFirst->m_pos;
	ball.m_velocity = pFirst->m_velocity;
	ball.m_state = pFirst->m_contactType;
	ball.m_flags = ball.m_powerFlags;

	SendMsg(this, "ShotPreview", -1, (intptr_t)"PM_REPLAY", 0, 0, 0);

	SendMsg(this, "NPC", 0x185, 0, 0, 0, 0);
	SendMsg(this, "Screen", 0x1B5, 0, 0, 0, 0);
	SendMsg(this, "Caddie", 0x13, 0, 0, 0, 0);

	ReactionForShot();

	if ((CSharedDoc::Instance()->m_currentChannel.flags & 0x80) && !IsMyTurn(true)) {
		SendMsg(this, "Screen", 0x1A8, 0, 0, 0, 0);
	} else {
		SendMsg(this, "Screen", 0x1A7, 0, 0, 0, 0);
	}

	SendMsg(this, "Screen", 0x1C4, 0, 0, 0, 0);
	SendMsg(this, "Trajectory", 0x18C, 0, 0, 0, 0);
	SendMsg(this, "Letter", 0x18C, 0, 0, 0, 0);
	SendMsg(this, "Effect", 0x177, 0, 0, 0, 0);
	SendMsg(this, "Effect", 0x178, 0, 0, 0, 0);
	CFx::Instance()->ClearAll(true);
	SendMsg(this, "Ball", 0x1EE, 0, 0, 0, 0);

	g_view->SetFOV(0.95993108f);
	g_camera = GOLFDOC()->GetDefaultCamera();
	g_audio->StopSfx("shot_dist_count");

	SendMsg(this, "Letter", 0x192, 0, 0, 0, 0);

	SendMsg(this, "Player", 0x10F, 0, 0, 0, 0);
	m_modeTime = 1.0f;
	GOLFDOC()->GameMode() = 0x40;
	GetPVS().SetFrame(m_replayFrame);

	CCameraWork::Instance()->ReplayCamera(keep);
	GOLFDOC()->m_clearedFlag158 = true;

	if (CHelpMsg::IsInstantiated()) {
		CHelpMsg::Instance()->OnChangeTurn();
	}
}

// golfrule.cpp:13600
// Skips the rest of the shot's flight: records the shot's statistics and
// result (hole-in: the score and the prizes; out of bounds: a penalty stroke)
// and moves on to the next turn, or to the give-up when the rule says so.
void CGolfRule::Skip(void) {
	m_replaying = false;
	m_flag266 = false;
	m_canFastForward = false;
	SendMsg(this, "Screen", 0x1B6, 0, 0, 0, 0);
	SendMsg(this, "Score", 0x132, 0, 0, 0, 0);
	SendMsg(this, "NPC", 0x182, 0, 0, 0, 0);
	CGolfBall &ball = GolfBall();

	if (ball.m_landed && OnlinePlay() && IsMassGame() && CanCancelShot())
		SendShotResult();

	if (CSharedDoc::Instance()->m_roomType == 10) {
		CSharedDoc::Instance()->m_gameResultList[MyIndex()].m_approachDistanceCopy = CSharedDoc::Instance()->m_gameResultList[MyIndex()].m_approachDistance;
		SendMsg(this, "Score", 0x2A, 1, 0, 0, 0);
	}

	if (GOLFDOC()->GameMode() == 0x40) {
		if (ball.m_eventPreDataIndex[7] > 0)
			SendMsg(this, "Ball", 0x1EC, 0, 0, 0, 0);

		SendMsg(this, "Screen", 0x1A8, 0, 0, 0, 0);
		SendMsg(this, "Spin", 0x164, 0, 0, 0, 0);
	}

	WPuppet *pBall = CSceneManager::Instance()->GetPuppet("\xB0\xF8");
	if (pBall && pBall->GetRootBone())
		pBall->SetAlpha(0xFF);

	g_audio->StopSfx("shot_dist_count");
	CCameraWork::Instance()->StopBallSound();

	UpdateShotResultStat();

	if (ball.IsHoleIn() == 1) {
		RecordScoreByPlayer(GOLFDOC()->m_myPlayerIndex);
		ChangeGameMode(0x1000);

		unsigned char score = CSharedDoc::Instance()->m_roomType == 1 ? TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] : PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1];
		AddPrize(score - GOLFDOC()->GetHoleData().m_par);
		AddExtPrize();
	} else {
		if (ball.m_landed) {
			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_rivalLevel += 1;
			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;
			if (CSharedDoc::Instance()->m_roomType == 1)
				TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;
		} else {
			if (ball.IsHoleIn() != 1 && ball.m_bounceCount == 0) {
				PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;
				if (CSharedDoc::Instance()->m_roomType == 1)
					TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;
			}

			PLAYER(GOLFDOC()->m_myPlayerIndex)->m_rivalLevel = 0;
		}

		if (m_pRule && m_pRule->CheckGiveUp()) {
			SendMsg(this, "GuideLine", 0x1A2, 0, 0, 0, 0);
			RecordGiveUp(GOLFDOC()->m_myPlayerIndex);
			RecordScoreByPlayer(GOLFDOC()->m_myPlayerIndex);

			if (CSharedDoc::Instance()->m_roomType == 1)
				ChangeGameMode(0x1000);
			else
				ChangeGameMode(0x80000);
			return;
		}

		if (CSharedDoc::Instance()->m_roomType == 1)
			ChangeGameMode(0x1000);
		else
			ChangeGameMode(0x800);
	}
}

// golfrule.cpp:13707
// Drops the ball at the given position with a one-stroke penalty and starts
// the next shot from there.
void CGolfRule::Drop(const WVector &pos) {
	if (GOLFDOC()->m_gameState == 9) {
		IActor *pTutorial = GetActor("Tutorial");
		if (pTutorial)
			pTutorial << MsgObject(0, 0x149, 0, 0, 0, 0, 0);
	}

	CGolfBall::sExpected *pData = GolfBall().PreDataToWrite(0);

	PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;

	if (CSharedDoc::Instance()->m_roomType == 1)
		TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] += 1;

	GolfBall().m_pos = pos;
	PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos = pos;
	PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel = 0;

	pData->m_pos = pos;
	GolfBall().m_groundArea = GetPVS().GetMeshIndex(pos, false);

	SendMsg(this, "Screen", 0x1A3, 0, 0, 0, 0);

	GolfClub().SelectBestClub();
	m_targetPoint = GOLFDOC()->m_pAutoTarget->GetPoint(0);
	GOLFDOC()->GetCameraData().m_yaw = CalcDeltaAngle(m_targetPoint - pos, WVector::UNIT_POS_Z);
	GolfClub().SetPowerShot(0);

	SetAngle(GOLFDOC()->GetCameraData().m_yaw);
	SendMsg(this, "Player", 0xE2, 0, 0, 0, 0);
	SendMsg(this, "Caddie", 0x13, 0, 0, 0, 0);

	float spot = 0.0f;
	SendMsg(this, "Spot", 0xF5, 0, 0, 0, 0);
	SendMsg(this, "Screen", 0x1AA, (intptr_t)&spot, 0, 0, 0);
	SendMsg(this, "Clock", 0x271A, 0, 0, 0, 0);

	SetTurnChangeEvent();
	ReadyForNextTurn();
}

// golfrule.cpp:13752
void CGolfRule::ForcedMoveBall(const WVector &pos) {
	CGolfBall::sExpected *preData = GolfBall().PreDataToWrite(0);

	GolfBall().m_pos = pos;
	PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos = pos;
	PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel = 0;

	preData->m_pos = pos;
	GolfBall().m_groundArea = GetPVS().GetMeshIndex(pos, false);

	SendMsg(this, "Screen", 0x1A3, 0, 0, 0, 0);

	GolfClub().SelectBestClub();
	m_targetPoint = GOLFDOC()->m_pAutoTarget->GetPoint(NULL);

	SendMsg(this, "Player", 0xE2, 0, 0, 0, 0);
	SendMsg(this, "Caddie", 0x13, 0, 0, 0, 0);

	int zero = 0;
	SendMsg(this, "Spot", 0xF5, 0, 0, 0, 0);
	SendMsg(this, "Screen", 0x1AA, (intptr_t)&zero, 0, 0, 0);
}

// golfrule.cpp:13781
// Takes the last shot back (the shot cancel item): the first player's ball,
// power shot level, combo gauge, pangs, hole record, score and rival level
// come back from the last m_backUps entry, his item slots are counted again,
// the ball and the camera return to where the shot was played, the club is
// selected again, and the turn restarts.
void CGolfRule::Cancel(void) {
	if (m_backUps.empty())
		return;

	CCameraWork::Instance()->Unlock();

	CGolfBall &ball = GolfBall();
	CGolfBall::sExpected *start = ball.PreDataToWrite(0);

	PLAYER(0)->m_ballPos = m_backUps.back().m_ballPos;
	PLAYER(0)->m_powerShotLevel = m_backUps.back().m_powerShotLevel;
	PLAYER(0)->m_comboGauge = m_backUps.back().m_comboGauge;

	PLAYER(0)->m_ghostAward = (int)m_backUps.back().m_ghostAward;
	PLAYER(0)->m_extraPang = (int)m_backUps.back().m_extraPang;
	PLAYER(0)->m_holeRecord[GOLFDOC()->m_currentHole - 1] = (int)m_backUps.back().m_holeRecord;

	PLAYER(0)->m_holes[GOLFDOC()->m_currentHole - 1] = m_backUps.back().m_holeScore;
	PLAYER(0)->m_rivalLevel = m_backUps.back().m_rivalLevel;
	PLAYER(0)->m_state = 0;

	PLAYER(0)->m_numItems = 0;

	for (int i = 0; i < 10; i++) {
		if (CSharedDoc::Instance()->m_roomPlayers[0].m_equipItem[i])
			PLAYER(0)->m_numItems++;
	}

	g_audio->StopSfx("shot_dist_count");

	start->m_pos = PLAYER(0)->m_ballPos;
	ball.m_pos = PLAYER(0)->m_ballPos;
	ball.m_groundArea = GetPVS().GetMeshIndex(ball.m_pos, false);
	ball.m_preDataIndex = -1;
	GOLFDOC()->m_cameraData.m_yaw = m_backUps.back().m_yaw;

	SendMsg(this, "Screen", 0x1A3, 0, 0, 0, 0);

	GolfClub().SetPowerShot(0);
	GolfClub().SelectBestClub();
	GolfClub().SetClub(m_backUps.back().m_clubName);

	m_backUps.pop_back();

	m_targetPoint = GolfBall().m_pos + RotMat(GOLFDOC()->m_cameraData.m_yaw, 1).za;

	SetAngle(GOLFDOC()->m_cameraData.m_yaw);
	SendMsg(this, "Player", 0xE2, 0, 0, 0, 0);
	SendMsg(this, "Caddie", 0x13, 0, 0, 0, 0);

	float spot = 0.0f;
	SendMsg(this, "Spot", 0xF5, 0, 0, 0, 0);
	SendMsg(this, "Screen", 0x1AA, (intptr_t)&spot, 0, 0, 0);
	SendMsg(this, "Clock", 0x271A, 0, 0, 0, 0);

	SendMsg(this, "Effect", 0x177, 0, 0, 0, 0);
	SendMsg(this, "Trajectory", 0x18C, 0, 0, 0, 0);
	CFx::Instance()->ClearAll(true);
	SendMsg(this, "Score", 0x132, 0, 0, 0, 0);

	SendMsg(this, "ShotPreview", -1, (intptr_t)"POP_BACK_ITEM", 0, 0, 0);
	SendMsg(this, "ShotReview", -1, (intptr_t)"POP_BACK_ITEM", 0, 0, 0);

	SendMsg(this, "MagicCarpetHelper", 0x27F, 0, 0, 0, 0);

	SetTurnChangeEvent();
	ReadyForNextTurn();

	m_gameOverDelay = 0;
	m_canFastForward = false;
	m_replaying = false;
	m_flag266 = false;
}

// golfrule.cpp:13859
void CGolfRule::Warp(const WVector &pos) {
	int meshIndex = GetPVS().GetMeshIndex(pos, true);

	if (meshIndex >= 0 && !GOLFDOC()->m_pPolySoup->IsOutOfBound(pos)) {
		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos = pos;
		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel = 0;
		GolfBall().m_pos = pos;
		GolfBall().m_groundArea = meshIndex;

		SetEnvironmentVariableA();
		SetTurnChangeEvent();
		ReadyForNextTurn();
	}
}

// golfrule.cpp:13876
// The course fly-by before the round: the camera follows a quadratic Bezier
// through m_flyByPoints, five seconds per segment, looking at the same curve
// one segment ahead.
void CGolfRule::FlyBy(float elapsed) {
	m_gameOverDelay += elapsed;
	if (m_gameOverDelay > 5.0f) {
		m_gameOverDelay -= 5.0;
		data_af1d84.flyByIndex += 2;
		if (data_af1d84.flyByIndex >= m_flyByPoints.size() - 1) {
			data_af1d84.flyByIndex = 0;
			SendMsg(this, "GolfBg", 0x1E5, (intptr_t)&m_flyByPoints, 0, 0, 0);
		}
	}

	float t = m_gameOverDelay;
	t *= 0.2f;
	float t2 = t * t;
	float s = 1.0f - t;
	float s2 = s * s;

	WVector pos;
	pos = s2 * m_flyByPoints[data_af1d84.flyByIndex] + s * t * 2.0f * m_flyByPoints[data_af1d84.flyByIndex + 1] + t2 * m_flyByPoints[data_af1d84.flyByIndex + 2];

	unsigned int next = data_af1d84.flyByIndex + 2;
	if (next >= m_flyByPoints.size() - 1)
		next = 0;

	// The point one segment ahead, then the direction to it.
	WVector dir;
	dir = s2 * m_flyByPoints[next] + s * t * 2.0f * m_flyByPoints[next + 1] + t2 * m_flyByPoints[next + 2];
	dir = (dir - pos).Normalize();
	dir.y = -0.1f;

	unsigned int half = (m_flyByPoints.size() - 1) / 2;
	if (data_af1d84.flyByIndex == 0 || data_af1d84.flyByIndex == half - 4 || data_af1d84.flyByIndex == half)
		dir.y -= 0.8f - m_gameOverDelay * 0.16f;
	else if (data_af1d84.flyByIndex == half - 6 || data_af1d84.flyByIndex == half - 2 || data_af1d84.flyByIndex == m_flyByPoints.size() - 3)
		dir.y -= m_gameOverDelay * 0.16f;

	if (dir.y < -0.9f)
		dir.y = -0.9f;
	else if (dir.y > -0.1f)
		dir.y = -0.1f;

	g_camera = MakeCameraMatrix(pos, pos + dir);
}

// golfrule.cpp:13931
unsigned char CGolfRule::GetFirstTurn(void) {
	if (OnlinePlay()) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
			m_firstTeam = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute;
		}
		return GOLFDOC()->m_myPlayerIndex;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
		return TEAM(m_firstTeam)->m_firstTurnSlot + m_firstTeam * 2;
	}

	unsigned char i;
	for (i = 0; i < 4; i++) {
		if (PLAYER(i)->m_turnIndex == 0) {
			break;
		}
	}
	return i;
}

// golfrule.cpp:13960
// Offline: the next player to shoot. Team games alternate the team members
// on the tee and otherwise send the team farther from the cup; single games
// follow the tee order on the tee and otherwise the playing player farthest
// from the cup (ground distance).
unsigned char CGolfRule::GetNextTurn(void) {
	if (OnlinePlay())
		return m_nextTurnPlayer;

	unsigned char next = 0;

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
		if (HaveTeeShots()) {
			next = TEAM(1 - m_firstTeam)->m_firstTurnSlot + (1 - m_firstTeam) * 2;
		} else {
			WVector toCup[2];
			toCup[0] = GetHoleCupPos() - TEAM(0)->m_ballPos;
			toCup[0].y = 0.0f;
			toCup[1] = GetHoleCupPos() - TEAM(1)->m_ballPos;
			toCup[1].y = 0.0f;

			int team = toCup[0].Magnitude() > toCup[1].Magnitude() ? 0 : 1;

			TEAM(team)->m_nextTurnSlot = 1 - TEAM(team)->m_nextTurnSlot;
			next = TEAM(team)->m_nextTurnSlot + team * 2;
		}
	} else {
		if (GOLFDOC()->GetPlayerNum() == 1)
			return 0;

		if (HaveTeeShots()) {
			unsigned char turn = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_turnIndex;

			for (; next < GOLFDOC()->GetPlayerNum(); next++) {
				if (PLAYER(next)->m_state == 3)
					continue;

				if (PLAYER(next)->m_turnIndex == turn + 1) {
					if (PLAYER(next)->m_state == 0)
						break;

					turn++;
					next = 0xFF;
				}
			}
		} else {
			for (; next < GOLFDOC()->GetPlayerNum(); next++) {
				if (PLAYER(next)->m_state == 0)
					break;
			}

			if (next == GOLFDOC()->GetPlayerNum())
				return GOLFDOC()->m_myPlayerIndex;

			WVector toCup = GetHoleCupPos() - PLAYER(next)->m_ballPos;
			toCup.y = 0.0f;
			float farthest = toCup.Magnitude();

			for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
				if (PLAYER(i)->m_state == 0 && i != next) {
					WVector playerToCup;
					playerToCup = GetHoleCupPos() - PLAYER(i)->m_ballPos;

					float distance = sqrtf(playerToCup.x * playerToCup.x + playerToCup.z * playerToCup.z);
					if (distance > farthest) {
						next = i;
						farthest = distance;
					}
				}
			}
		}
	}

	return next;
}

// golfrule.cpp:14066
void CGolfRule::AllReplay(void) {
	m_replayFlags &= ~4;
	Replay(false);
}

// golfrule.cpp:14072
// Replays the last 50 frames of the hole's final shot, the camera looking at
// the cup from beside the ball's line.
void CGolfRule::HoleReplay(void) {
	CGolfBall &ball = GolfBall();

	ball.m_preDataIndex = ball.m_preDataCount - 50;
	ball.m_pos = ball.PreData(ball.m_preDataIndex)->m_pos;
	ball.m_velocity = ball.PreData(ball.m_preDataIndex)->m_velocity;
	ball.m_state = ball.PreData(ball.m_preDataIndex)->m_contactType;
	GetPVS().SetFrame(GetPVS().GetFrame() - 50);

	WVector dir;
	dir = GetHoleCupPos() - ball.PreData(0)->m_pos;
	dir.y = 0.0f;
	dir.Normalize();

	switch (m_replayMode) {
	case 1: {
		WVector eye;
		eye = GetHoleCupPos() + dir * 16.0f * RotMat(0.7853982f, 1);
		eye.y += 9.6f;
		g_camera = MakeCameraMatrix(eye, GetHoleCupPos());
		CCameraWork::Instance()->SetCamera(g_camera);
		break;
	}
	case 2: {
		WVector eye;
		eye = GetHoleCupPos() + dir * 9.6f * RotMat(-0.7853982f, 1);
		g_camera = MakeCameraMatrix(eye, GetHoleCupPos());
		g_camera.pivot.y += 1.0f;
		CCameraWork::Instance()->SetCamera(g_camera);
		break;
	}
	}
}

// golfrule.cpp:14104
void CGolfRule::SwingReplay(void) {
	CGolfBall &ball = GolfBall();
	const CGolfBall::sExpected *pFirst = ball.PreData(0);

	SendMsg(this, "Effect", 0x177, 0, 0, 0, 0);
	SendMsg(this, "Effect", 0x178, 0, 0, 0, 0);
	SendMsg(this, "Trajectory", 0x18C, 0, 0, 0, 0);
	CFx::Instance()->ClearAll(true);

	if (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId == 0x14000005) {
		SendMsg(this, "Ball", 0x1EE, 0, 0, 0, 0);
	}

	SendMsg(this, "Player", 0x10F, 0, 0, 0, 0);
	m_replayDone = false;
	m_modeTime = 1.0f;
	GOLFDOC()->GameMode() = 0x40;
	GetPVS().SetFrame(m_replayFrame);

	ball.m_preDataIndex = 0;
	ball.m_pos = pFirst->m_pos;
	ball.m_velocity = pFirst->m_velocity;
	ball.m_state = pFirst->m_contactType;

	CCameraWork::Instance()->SetReplayCount(m_replayMode);
	CCameraWork::Instance()->SelectCamera(0xCE);
}

// golfrule.cpp:14135
// Decides which replays the shot just played earns (m_replayFlags): bit 2 for
// power shots, bit 4 for long hole-ins, bit 1 for mid-range or special ones.
void CGolfRule::AutoReplay(void) {
	m_replayFlags = 0;

	switch (CSharedDoc::Instance()->m_roomType) {
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
		return;
	}

	if (GetCurMap() == 7) {
		return;
	}

	if (GOLFDOC()->m_gameState < 15 && GOLFDOC()->m_gameState != 8) {
		return;
	}

	WVector toCup = GetHoleCupPos() - GolfBall().PreData(0)->m_pos;
	float distance = sqrtf(toCup.x * toCup.x + toCup.z * toCup.z);

	if (GolfBall().m_powerFlags & 0x70) {
		m_replayFlags |= 2;
	}

	if (GolfClub().PowerShotType() != 0 && GOLFDOC()->m_bar.m_accuracyGrade == 4 && GOLFDOC()->m_bar.m_stopped) {
		m_replayFlags |= 2;
	}

	if (GolfBall().IsHoleIn() == 1) {
		if (GolfClub().GetType() == 3 && distance > 64.0f) {
			m_replayFlags |= 4;
		}

		if (distance > 320.0f) {
			m_replayFlags |= 4;
		}

		if (!(m_replayFlags & 4)) {
			if (distance > 121.6f && distance < 320.0f) {
				m_replayFlags |= 1;
			}

			if (GolfBall().m_flags & 3) {
				m_replayFlags |= 1;
			}
		}
	}

	if (GolfBall().m_landed) {
		m_replayFlags &= ~4;
	}

	unsigned char powerShot = GolfClub().PowerShotType();

	if (powerShot == 2 || powerShot == 3 || GolfBall().m_preDataCount < 15.000001f) {
		m_replayFlags &= ~2;
	}
}

// golfrule.cpp:14199
// Writes the recorded shot to a replay file: the given name, or the first free
// save/pangya_NNN.sav.
void CGolfRule::SaveShot(const char *fileName) {
	if (m_shotFrame == 0)
		return;

	if (fileName == 0) {
		mkdir("save");
		int index = 0;

		do {
			sprintf(m_saveFileName, "save/pangya_%03d.sav", index++);
		} while (_access(m_saveFileName, 0) != -1);
	} else {
		strcpy(m_saveFileName, fileName);
	}

	FILE *fp = fopen(m_saveFileName, "wb");

	if (fp == 0)
		return;

	char header[32] = "";

	SYSTEMTIME time;
	GetLocalTime(&time);
	unsigned char index = IsMassGame() ? 0 : MyIndex();
	CPartTidList &partTidList = GOLFDOC()->GetPartTidList(index);
	int playType = CSharedDoc::Instance()->m_replayPlayType;

	fwrite(header, 0x20, 1, fp);
	unsigned char map = GetCurMap();
	fwrite(&map, 1, 1, fp);
	fwrite(&time, 0x10, 1, fp);
	fwrite(&playType, 4, 1, fp);
	fwrite(&CSharedDoc::Instance()->m_roomPlayers[index], 0xB92, 1, fp);
	fwrite(&partTidList, 0xC8, 1, fp);

	fwrite(GOLFDOC()->GetPlayer(index), 0xB98, 1, fp);
	fwrite(&m_hole, 0x88, 1, fp);
	fwrite(&CSharedDoc::Instance()->m_holePin[GetHoleIndex(m_hole) - 1], 1, 1, fp);
	fwrite(&CSharedDoc::Instance()->m_currentChannel.flags, 4, 1, fp);

	fclose(fp);

	WSingleton<CChatMsg>::Instance()->AddChatMsg(MakeStr("%s\xB4\xD4\xC0\xC7 %d\xC8\xA6 %d\xB9\xF8\xC2\xB0 \xBC\xA6 \xC1\xA4\xBA\xB8\xB0\xA1 '\xB1\xE2\xB7\xCF\xB5\xC8 \xB8\xAE\xC7\xC3\xB7\xB9\xC0\xCC\xC5\xD7\xC0\xCC\xC7\xC1'\xBF\xA1 \xC0\xFA\xC0\xE5\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.", CSharedDoc::Instance()->m_userInfo.nickname, m_hole, m_shotNumber), 0xFFFF7878, true, false);
}

// golfrule.cpp:14251
// Reads a replay file SaveShot wrote back into the documents.
void CGolfRule::LoadShot(const char *fileName) {
	FILE *fp = fopen(fileName, "rb");

	if (fp == 0) {
		MessageBoxA(0, MakeStr("\xBC\xBC\xC0\xCC\xBA\xEA \xC6\xC4\xC0\xCF(%s)\xC0\xBB \xC3\xA3\xC0\xBB \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", fileName), "\xB0\xE6\xB0\xED", 0);
		g_bQuit = true;
		return;
	}

	char header[32] = "";
	unsigned char map = 0;
	SYSTEMTIME time;
	CPartTidList partTidList;
	int playType = 0;

	fread(header, 0x20, 1, fp);
	fread(&map, 1, 1, fp);
	fread(&time, 0x10, 1, fp);
	fread(&playType, 4, 1, fp);
	CSharedDoc::Instance()->m_replayPlayType = playType;
	fread(&CSharedDoc::Instance()->m_roomPlayers[0], 0xB92, 1, fp);
	fread(&partTidList, 0xC8, 1, fp);

	fread(GOLFDOC()->GetPlayer(0), 0xB98, 1, fp);
	fread(&m_hole, 0x88, 1, fp);
	fread(&CSharedDoc::Instance()->m_holePin[GetHoleIndex(m_hole) - 1], 1, 1, fp);
	fread(&CSharedDoc::Instance()->m_currentChannel.flags, 4, 1, fp);

	fclose(fp);

	SetCurMap(map);
	GOLFDOC()->m_currentHole = m_hole;
	CSharedDoc::Instance()->m_holeOrder[0] = m_hole;
	GolfClub().SetClub(m_clubName);
	GOLFDOC()->SetPartTidList(0, partTidList.m_tidChar, partTidList.m_hairClr, partTidList.m_shirtsClr, partTidList.m_partTids, partTidList.m_partIds);

	SetGameType(0);

	CSharedDoc::Instance()->m_replayPrevAccuracy = m_shotBar.m_prevAccuracy;

	CSharedDoc::Instance()->m_replayPlayType = playType;

	if (IsLocalContent((localContentType_t)0x57)) {
		RequestUccInfo(CSharedDoc::Instance()->m_roomPlayers[0]);
	}
}

// golfrule.cpp:14307
// Writes the shot data replay file (the given name, or the first free
// save/pangya_NNN.sav out of 100). anonymous hides the player's id, name and
// UID. Returns 1 on success, 0 without a shot or a file, -1 with no free name.
int CGolfRule::ShotDataSave(const char *fileName, bool anonymous) {
	if (m_shotFrame == 0)
		return 0;

	if (fileName == 0) {
		mkdir("save");

		int index = 0;
		while (true) {
			if (index >= 100)
				return -1;

			sprintf(m_saveFileName, "save/pangya_%03d.sav", index++);
			if (_access(m_saveFileName, 0) == -1)
				break;
		}
	} else {
		strcpy(m_saveFileName, fileName);
	}

	FILE *fp = fopen(m_saveFileName, "wb");
	if (fp == 0)
		return 0;

	unsigned char map = GetCurMap();

	SYSTEMTIME time;
	GetLocalTime(&time);
	unsigned char index = IsMassGame() ? 0 : MyIndex();
	CPartTidList &partTidList = GOLFDOC()->GetPartTidList(index);
	int shotCount = CSharedDoc::Instance()->m_shotDataList.size();
	int playType = CSharedDoc::Instance()->m_replayPlayType;
	unsigned char holePin = CSharedDoc::Instance()->m_holePin[GetHoleIndex(GOLFDOC()->m_currentHole) - 1];

	unsigned long uid = 0xFFFFFFFF;
	if (!anonymous)
		uid = CSharedDoc::Instance()->m_myUID;

	fwrite(&CSharedDoc::Instance()->m_value5BD4, 4, 1, fp);
	fwrite(&uid, 4, 1, fp);
	fwrite(&map, 1, 1, fp);
	fwrite(&time, 0x10, 1, fp);
	fwrite(&playType, 4, 1, fp);

	if (anonymous) {
		char nickname[0x16];
		char name[0x16];

		strcpy(nickname, CSharedDoc::Instance()->m_roomPlayers[index].m_nickname);
		strcpy(name, CSharedDoc::Instance()->m_roomPlayers[index].m_name);
		unsigned long playerUid = CSharedDoc::Instance()->m_roomPlayers[index].m_uid;
		sprintf(CSharedDoc::Instance()->m_roomPlayers[index].m_nickname, "NOID");
		sprintf(CSharedDoc::Instance()->m_roomPlayers[index].m_name, "NONICKNAME");
		CSharedDoc::Instance()->m_roomPlayers[index].m_uid = 0xFFFFFFFF;

		fwrite(&CSharedDoc::Instance()->m_roomPlayers[index], 0xB92, 1, fp);

		strcpy(CSharedDoc::Instance()->m_roomPlayers[index].m_nickname, nickname);
		strcpy(CSharedDoc::Instance()->m_roomPlayers[index].m_name, name);
		CSharedDoc::Instance()->m_roomPlayers[index].m_uid = playerUid;
	} else {
		fwrite(&CSharedDoc::Instance()->m_roomPlayers[index], 0xB92, 1, fp);
	}

	fwrite(&partTidList, 0xC8, 1, fp);
	fwrite(&holePin, 1, 1, fp);
	fwrite(GOLFDOC()->GetPlayer(index), 0xB98, 1, fp);
	fwrite(&CSharedDoc::Instance()->m_currentChannel.flags, 4, 1, fp);

	if (playType == 1) {
		fwrite(&m_hole, 0x88, 1, fp);
	} else if (playType == 2) {
		fwrite(&shotCount, 4, 1, fp);

		for (std::list<sShotDataStorage>::iterator it = CSharedDoc::Instance()->m_shotDataList.begin(); it != CSharedDoc::Instance()->m_shotDataList.end(); ++it)
			fwrite(&*it, 0x88, 1, fp);

		CSharedDoc::Instance()->m_shotDataList.clear();
	}

	if (IsLocalContent((localContentType_t)0x51)) {
		int cardCount = CSharedDoc::Instance()->m_cardPeriodList[0].size();
		fwrite(&cardCount, 4, 1, fp);

		if (cardCount > 0) {
			for (std::list<sSCardAvilityPeriodInfo>::iterator it = CSharedDoc::Instance()->m_cardPeriodList[0].begin(); it != CSharedDoc::Instance()->m_cardPeriodList[0].end(); ++it)
				fwrite(&*it, 0x41, 1, fp);
		}
	}

	fclose(fp);
	return 1;
}

// golfrule.cpp:14428
// Reads a shot data replay file ShotDataSave wrote (an empty name reads the
// developer's fixed file) back into the documents. false, and the game
// quits, without the file.
bool CGolfRule::ShotDataLoad(const char *fileName) {
	FILE *fp;
	if (strlen(fileName) == 0)
		fp = fopen("D:/work/ProjectG_S4/bin/save/pangya_000.sav", "rb");
	else
		fp = fopen(fileName, "rb");

	if (fp == 0) {
		g_bQuit = true;
		return false;
	}

	int version = 0;
	unsigned char map = 0;
	SYSTEMTIME time;
	CPartTidList partTidList;

	int shotCount = 0;
	unsigned char holePin = 0;
	int playType = 0;
	unsigned long uid = 0xFFFFFFFF;
	sShotData shotData;
	memset(&m_hole, 0, sizeof(sShotData));
	CSharedDoc::Instance()->m_shotDataList.clear();
	CSharedDoc::Instance()->m_replayStartB = 0;

	fread(&version, 4, 1, fp);
	fread(&uid, 4, 1, fp);
	fread(&map, 1, 1, fp);
	fread(&time, 0x10, 1, fp);
	fread(&playType, 4, 1, fp);
	CSharedDoc::Instance()->m_replayPlayType = playType;

	fread(&CSharedDoc::Instance()->m_roomPlayers[0], 0xB92, 1, fp);
	fread(&partTidList, 0xC8, 1, fp);
	fread(&holePin, 1, 1, fp);
	fread(GOLFDOC()->GetPlayer(0), 0xB98, 1, fp);

	fread(&CSharedDoc::Instance()->m_currentChannel.flags, 4, 1, fp);

	if (playType == 1) {
		fread(&m_hole, 0x88, 1, fp);
		CSharedDoc::Instance()->m_replayStartB = m_shotCountKey;
	} else if (playType == 2) {
		fread(&shotCount, 4, 1, fp);
		for (int i = 0; i < shotCount; i++) {
			fread(&shotData, 0x88, 1, fp);
			if (i == 0) {
				memcpy(&m_hole, &shotData, sizeof(sShotData));
				CSharedDoc::Instance()->m_replayStartB = 0;
				CSharedDoc::Instance()->m_shotCountKey = m_shotCountKey;
				GOLFDOC()->GetPlayer(0)->m_comboGauge = m_comboGauge;
				GetPVS().SetFrame(m_shotFrame);
			}
			CSharedDoc::Instance()->m_shotDataList.push_back(*(sShotDataStorage *)&shotData);
			memset(&shotData, 0, sizeof(sShotData));
		}
	}

	if (IsLocalContent((localContentType_t)0x51)) {
		int cardCount = 0;
		fread(&cardCount, 4, 1, fp);
		if (cardCount > 0) {
			CSharedDoc::Instance()->m_cardPeriodList[2].clear();

			for (int i = 0; i < cardCount; i++) {
				sSCardAvilityPeriodInfo info;
				fread(&info, 0x41, 1, fp);
				CSharedDoc::Instance()->m_cardPeriodList[2].push_back(info);
			}
		}

		WSingleton<CCardManager>::Instance()->m_characterInfo[2] = CSharedDoc::Instance()->m_roomPlayers[0].m_characterInfo;
		WSingleton<CCardManager>::Instance()->CalcCardPeriodAndStatus();
		WSingleton<CCardManager>::Instance()->SetPlayerIndex(2);
	}

	fclose(fp);

	SetCurMap(map);
	CSharedDoc::Instance()->m_holeOrder[0] = m_hole;
	CSharedDoc::Instance()->m_holePin[GetHoleIndex(m_hole) - 1] = holePin;
	GOLFDOC()->m_currentHole = m_hole;

	GOLFDOC()->SetPartTidList(0, partTidList.m_tidChar, partTidList.m_hairClr, partTidList.m_shirtsClr, partTidList.m_partTids, partTidList.m_partIds);

	SetGameType(0);

	CSharedDoc::Instance()->m_replayPrevAccuracy = m_shotBar.m_prevAccuracy;

	CSharedDoc::Instance()->m_replayPlayType = playType;

	if (IsLocalContent((localContentType_t)0x57)) {
		RequestUccInfo(CSharedDoc::Instance()->m_roomPlayers[0]);
	}

	return true;
}


// golfrule.cpp:14545
// Appends one part of the game state to a save/save_NNN.dat file (part 0
// opens a new one): 0 the course and players, 1 the wind, 2 the hole block.
void CGolfRule::SaveGame(int part) {
	if (part == 0) {
		mkdir("save");
		int index = 0;

		do {
			sprintf(m_saveFileName, "save/save_%03d.dat", index++);
		} while (_access(m_saveFileName, 0) != -1);
	}

	FILE *fp = fopen(m_saveFileName, "ab");
	if (fp) {
		switch (part) {
		case 0: {
			unsigned char map = GetCurMap();
			fwrite(&map, 1, 1, fp);

			int holeOrder = CSharedDoc::Instance()->m_holeOrder[0];
			fwrite(&holeOrder, 4, 1, fp);

			unsigned char playerNum = GOLFDOC()->GetPlayerNum();
			fwrite(&playerNum, 1, 1, fp);
			sPlayerData *players = GOLFDOC()->GetPlayer(0);
			fwrite(players, 0xB98, playerNum, fp);
			break;
		}
		case 1:
			fwrite(&Wind().GetGlobalWind(), 0xC, 1, fp);
			break;
		case 2:
			fwrite(&m_hole, 0x88, 1, fp);
			break;
		}

		fclose(fp);
	}
}

// golfrule.cpp:14592
// Reads the next part SaveGame wrote, from save/save_000.dat.
void CGolfRule::LoadGame(int part) {
	FILE *fp = fopen("save/save_000.dat", "rb");
	if (!fp) {
		g_bQuit = true;
		return;
	}

	fseek(fp, data_af1d84.loadGamePos, SEEK_SET);

	switch (part) {
	case 0: {
		unsigned char map;
		fread(&map, 1, 1, fp);
		SetCurMap(map);

		unsigned char playerNum = GOLFDOC()->GetPlayerNum();
		fread(&GOLFDOC()->m_currentHole, 1, 1, fp);
		fread(&playerNum, 1, 1, fp);
		sPlayerData *players = GOLFDOC()->GetPlayer(0);
		fread(players, 0xB98, playerNum, fp);
		break;
	}
	case 1:
		fread(&m_wind, 0xC, 1, fp);
		Wind().SetWind(m_wind);
		break;
	case 2:
		fread(&m_hole, 0x88, 1, fp);
		break;
	}

	data_af1d84.loadGamePos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	ftell(fp);

	fclose(fp);
}

// golfrule.cpp:14644
void CGolfRule::QuickSave(const char *name) {
	FILE *fp = fopen(MakeStr("%s.sav", name), "wb");

	unsigned char map = GetCurMap();
	fwrite(&map, 1, 1, fp);
	fwrite(&GOLFDOC()->m_currentHole, 1, 1, fp);

	unsigned char playerNum = GOLFDOC()->GetPlayerNum();
	fwrite(&playerNum, 1, 1, fp);
	fwrite(PLAYER(0), 0xB98, playerNum, fp);
	fwrite(&GOLFDOC()->m_myPlayerIndex, 1, 1, fp);

	fclose(fp);
}

// golfrule.cpp:14660
// Restores a QuickSave file and frees the name (new[] by the caller).
void CGolfRule::QuickLoad(char *name) {
	FILE *fp = fopen(MakeStr("%s.sav", name), "rb");
	if (fp) {
		unsigned char map;
		unsigned char hole;
		fread(&map, 1, 1, fp);
		fread(&hole, 1, 1, fp);

		unsigned char playerNum = GOLFDOC()->GetPlayerNum();
		fread(&playerNum, 1, 1, fp);
		sPlayerData *players = GOLFDOC()->GetPlayer(0);
		fread(players, 0xB98, playerNum, fp);
		fread(&GOLFDOC()->m_myPlayerIndex, 1, 1, fp);

		fclose(fp);

		if (map != GetCurMap()) {
			SetCurMap(map);
			GOLFDOC()->m_currentHole = hole;

			SendMsg(this, "GolfBg", 0x140, 0, 0, 0, 0);
			SendMsg(this, "GolfBg", 0x13E, 0, 0, 0, 0);
		} else if (hole != GOLFDOC()->m_currentHole) {
			GOLFDOC()->m_currentHole = hole;
			SendMsg(this, "GolfBg", 0x140, 0, 0, 0, 0);
		}

		SendMsg(this, "Player", 0x116, 0, 0, 0, 0);
		SendMsg(this, "Caddie", 0x116, 0, 0, 0, 0);

		SetEnvironmentVariableA();
		GOLFDOC()->MakeDefaultCamera(true);
		ChangeGameMode(8);

		delete[] name;
	}
}

// golfrule.cpp:14705
// The power bar finished: the spin is set (a random one for a power shot with
// the 0x50 content), a cut-in is announced, and online the shot goes to the
// server (packet 0x12, then the arrow commands as 0x42) before the swing
// starts (game mode 0x40).
void CGolfRule::HitShot(void) {
	if (CSharedDoc::Instance()->GetPlayingMode() != REPLAY_MODE_PLAYBACK
		&& !IsMyTurn(false))
		return;

	timeEndPeriod(1);
	CFx::Instance()->SetActive(true);

	if (IsLocalContent((localContentType_t)0x50)) {
		if (GolfBall().m_powerFlags > 0 && GOLFDOC()->GetBar().m_accuracyGrade != 4 && GolfClub().GetType() != 3) {
			float lieAngle = (float)(rand() % 3 + 2);

			if (rand() % 2 == 0)
				lieAngle *= -1.0f;
			GolfBall().m_lieAngle = lieAngle * 0.1f;

			float lieSlope = (float)(rand() % 3 + 2);

			if (rand() % 2 == 0)
				lieSlope *= -1.0f;
			GolfBall().m_lieSlope = lieSlope * 0.1f;

			SendMsg(this, "Spin", 0x165, 0, (int)GolfBall().m_lieSlope, (int)GolfBall().m_lieAngle, 0);
		}
	}

	if (CSharedDoc::Instance()->GetPlayingMode() == REPLAY_MODE_GAME) {
		if (OnlinePlay()) {
			int cutinMode = CSharedDoc::Instance()->m_gameMode;

			if (GOLFDOC()->m_gameState < 15)
				cutinMode = 6;

			if (GolfClub().PowerShotType() && GolfBall().m_powerFlags) {
				short cutinType = 4;
				if ((GolfBall().m_powerFlags & 0x70) && GOLFDOC()->GetBar().m_accuracyGrade == 4) {
					if (GolfClub().PowerShotType() == 2 || (GolfClub().PowerShotType() == 3 && GolfClub().GetType() == 0))
						cutinType = 2;
					else
						cutinType = 1;
				}

				bool sent = false;
				if (CSharedDoc::Instance()->m_mySkinList.size() > 0) {
					for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_mySkinList.begin(); it != CSharedDoc::Instance()->m_mySkinList.end(); ++it) {
						if (S5::ISCutinItem(it->m_typeId)) {
							IFF_STRUCT::S5::sCutinInformation *info = ItemManager()->FindCutinInformation(it->m_typeId);

							if (info && (info->m_condition & cutinType)) {
								S5::UTIL::SendCutInPacket(CSharedDoc::Instance()->m_myUID, cutinType, cutinMode, CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_characterInfo.typeId);
								sent = true;
								break;
							}
						}
					}
				}

				if (!sent)
					S5::UTIL::SendCutInPacket(CSharedDoc::Instance()->m_myUID, 0, cutinMode, CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_characterInfo.typeId);
			} else {
				S5::UTIL::SendCutInPacket(CSharedDoc::Instance()->m_myUID, 0, cutinMode, CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_characterInfo.typeId);
			}
		}
	}

	if (!OnlinePlay()) {
		WSendPacket packet((enumClientPacket)0);
		SendMsg(this, "GroundItemMan", 0x278, (intptr_t)&packet, 0, 0, 0);
	}

	if (OnlinePlay() && CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_GAME) {
		sShotProtocol shot;

		shot.lieSlope = GolfBall().m_lieSlope;
		shot.lieAngle = GolfBall().m_lieAngle;
		shot.powerFlags = GolfBall().m_powerFlags;
		shot.accuracy = GOLFDOC()->GetBar().m_position;
		shot.power = GOLFDOC()->GetBar().m_power;
		shot.accuracyGrade = GOLFDOC()->GetBar().m_accuracyGrade;
		shot.barStartPosition = GOLFDOC()->GetBar().m_startPosition;

		if (CSharedDoc::Instance()->m_roomType == 10) {
			unsigned long limit = GetGameTimeLimit();
			shot.shotElapsed = CSharedDoc::Instance()->m_gameStartTime - g_CurrentTime + limit;
		} else {
			shot.shotElapsed = GOLFDOC()->m_shotElapsed;
		}

		shot.frame = GetPVS().GetFrame();
		SendMsg(this, "Player", 0x110, (intptr_t)&shot.angle, 0, 0, 0);
		shot.club = GolfClub().GetIndex();
		shot.randomPower = GolfClub().GetRandomPower();
		GolfClub().SetCalcRandomPower(false);
		shot.spot = m_spot;

		if (IsLocalContent((localContentType_t)0x87)) {
			sItemInfo *ball = NULL;

			for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_myGearList.begin(); it != CSharedDoc::Instance()->m_myGearList.end(); ++it) {
				if (it->m_typeId == CSharedDoc::Instance()->m_myBallId) {
					ball = &*it;
					break;
				}
			}

			if (!ball)
				CSharedDoc::Instance()->m_myBallId = 0x14000000;
		}

		WSendPacket packet(0x12);
		AddSomeInfo(packet);
		packet.EncodeBuffer(&shot, sizeof(shot));
		SendMsg(this, "GroundItemMan", 0x278, (intptr_t)&packet, 0, 0, 0);
		packet.Send(SEND_TO_DEFAULT);

		if (OnlinePlay())
			SendMsg(this, "Cutin", 0x78, 0, 0, 0, 0);

		if (GOLFDOC()->m_commandArrows.size() > 0) {
			WSendPacket command((enumClientPacket)0x42);
			command.Encode1((unsigned char)GOLFDOC()->m_commandArrows.size());

			for (std::list<unsigned long>::iterator it = GOLFDOC()->m_commandArrows.begin(); it != GOLFDOC()->m_commandArrows.end(); ++it)
				command.Encode4(*it);
			command.Send(SEND_TO_DEFAULT);

			if (OnlinePlay())
				GetActor("Cutin") << MsgObject(NULL, 0x78, 8, 0, 0, 0, 0);
		}

		if (OnlinePlay()) {
			switch (CSharedDoc::Instance()->m_roomType) {
			case 4:
			case 5:
			case 6:
			case 9:
			case 10:
				SendMsg(this, "Cutin", 0x78, 0x10, 0, 0, 0);
				SetNetMsgWait(true, 0x3A76, "modeDriveMotion_MassGame", 20.0f);
				ChangeGameMode(0x40);
				break;
			default:
				SetNetMsgWait(true, 0x3A7A, "HitShot", 20.0f);
				break;
			}
		} else {
			switch (CSharedDoc::Instance()->m_roomType) {
			case 4:
			case 5:
			case 6:
			case 9:
			case 10:
				ChangeGameMode(0x40);
				break;
			default:
				SetNetMsgWait(true, 0x3A81, "HitShot", 20.0f);
				break;
			}
		}
	} else {
		if (CSharedDoc::Instance()->m_roomType == 10) {
			unsigned long limit = GetGameTimeLimit();
			CSharedDoc::Instance()->m_gameResultList[MyIndex()].m_approachPoint = CSharedDoc::Instance()->m_gameStartTime - g_CurrentTime + limit;
		}

		if (IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == REPLAY_MODE_PLAYBACK) {
			if (CSharedDoc::Instance()->m_replayState != 7)
				CSharedDoc::Instance()->m_replayState = 3;
			GolfBall().m_lieSlope = m_shotLieSlope;
			GolfBall().m_lieAngle = m_shotLieAngle;
			SendMsg(this, "Spin", 0x165, 0, (int)GolfBall().m_lieSlope, (int)GolfBall().m_lieAngle, 0);
		}

		if (OnlinePlay()) {
			if (GOLFDOC()->m_gameState == 15 && CSharedDoc::Instance()->m_playingMode != REPLAY_MODE_PLAYBACK) {
				SendMsg(this, "Cutin", 0x78, 0x10, 0, 0, 0);
				SetNetMsgWait(true, 0x3A99, "modeDriveMotion_MassGame", 20.0f);
			}
		}

		ChangeGameMode(0x40);
	}

	if (m_chatViewWnd)
		m_chatViewWnd->SetVisible(true);
}

// golfrule.cpp:15013
void CGolfRule::ReactionForShot(void) {
	SendMsg(this, "Caddie", 0x1D6, 0, 0, 0, 0);

	const CGolfBall::sExpected *pFirst = GolfBall().PreData(0);
	const CGolfBall::sExpected *pLast = GolfBall().PreData(GolfBall().m_preDataCount);
	WVector distance;
	distance.x = pLast->m_pos.x - pFirst->m_pos.x;
	distance.z = pLast->m_pos.z - pFirst->m_pos.z;

	if (!GolfBall().m_landed && sqrtf(distance.x * distance.x + distance.z * distance.z) > 416.0f) {
		SendMsg(this, "Caddie", 0x1D7, 0, 0, 0, 0);
	} else {
		CCameraWork::Instance()->SetCaddieChase(false);
	}
}

// golfrule.cpp:15027
// The turn passes: the actors reset for the next player, the bar and the
// ball's hole-in state are cleared and the background music follows.
void CGolfRule::ChangeTurn(void) {
	SendMsg(this, "Spin", 0x271A, 0, 0, 0, 0);
	SendMsg(this, "Grid", 0x163, 0, 0, 0, 0);
	SendMsg(this, "Ball", 0x163, 0, 0, 0, 0);
	SendMsg(this, "Ball", 0x1EB, 0, 0, 0, 0);
	SendMsg(this, "Ball", 0x1ED, 0, 0, 0, 0);
	SendMsg(this, "Screen", 0x1A0, 0, 0, 0, 0);
	SendMsg(this, "Item", 0x1A0, 0, 0, 0, 0);
	SendMsg(this, "Ball", 0x1A0, 0, 0, 0, 0);
	SendMsg(this, "Score", 0x1A0, 0, 0, 0, 0);
	SendMsg(this, "GuideLine", 0x1A0, 0, 0, 0, 0);
	SendMsg(this, "NPC", 0x182, 0, 0, 0, 0);
	SendMsg(this, "Caddie", 0x13, 0, 0, 0, 0);
	SendMsg(this, "Caddie", 0x1E3, CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_caddieInfo.typeId,
		CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_caddieInfo.cadItemTypeId, 0, 0);

	GOLFDOC()->m_clockHold = 0;
	GOLFDOC()->GetBar().Reset();
	GOLFDOC()->GetBar().m_accuracy = GolfClub().GetType() == 3 ? 140.0f : 105.0f;
	GOLFDOC()->GetBar().m_prevAccuracy = GOLFDOC()->GetBar().m_accuracy;

	GolfBall().SetHoleIn(0);

	GetPVS().CalcGreenPlane();
	GolfClub().SetAccuracy();

	PlayBGM();

	GOLFDOC()->m_clearedFlag158 = false;
}

// golfrule.cpp:15229
void CGolfRule::GetWinningPrize(int prize) {
	AddPrize(prize);
	AddExtPrize();
}

// golfrule.cpp:15238
void CGolfRule::AddPrize(int score) {
	unsigned long typeId;
	switch (score) {
	case -3: typeId = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_prizeItems.m_typeId[1]; break;
	case -2: typeId = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_prizeItems.m_typeId[2]; break;
	case -1: typeId = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_prizeItems.m_typeId[3]; break;
	case 0: typeId = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_prizeItems.m_typeId[4]; break;
	case 1: typeId = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_prizeItems.m_typeId[5]; break;
	default: typeId = 0; break;
	}

	if (GOLFDOC()->GetHoleData().m_par + score == 1)
		typeId = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_prizeItems.m_typeId[0];

	IFF_ITEM_COMMON *pItem = ItemManager()->FindCommonItem(typeId);
	if (pItem) {
		SendMsg(this, "Caddie", 0x1DE, 0x23, 0, 0, 0);

		CFxSpray *pSpray = WSingleton<CFx>::Instance()->OpenSpray("prize.spr");
		if (pSpray) {
			WVector pos = GolfBall().m_pos;
			pSpray->m_pos = pos;
		}
	}

	for (int i = 0; i < 6; i++)
		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_prizeItems.m_typeId[i] = 0;
}

// golfrule.cpp:15354
// The extra prizes the local player won on the hole: each one plays its
// effect at the cup (the quest drop's own effect, the medal and four-gods
// sequences, coins for the pang item) and, without an effect of its own, the
// hole drop spray at the ball; the caddie cheers, and on the player's turn
// the chat says "Congratulations! <item> obtained". The list is then emptied.
void CGolfRule::AddExtPrize(void) {
	bool dropSpray = true;

	PLAYER(GOLFDOC()->m_myPlayerIndex);
	PLAYER(GOLFDOC()->m_myPlayerIndex);

	for (int i = 0; i < PLAYER(GOLFDOC()->m_myPlayerIndex)->m_extPrizeCount; i++) {
		int kind = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_extPrizes[i].m_kind;
		if (IsNoItemPrizeKind(kind))
			continue;

		unsigned long typeId = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_extPrizes[i].m_typeId;

		if (typeId == 0)
			continue;

		IFF_ITEM_COMMON *item = ItemManager()->FindCommonItem(typeId);
		if (!item)
			continue;

		const char *effect;

		if ((typeId & 0xFC000000) != 0x74000000) {
			effect = "thanxgiving.seq";

			if (typeId == 0x1A0000AA) {
				effect = "coin_medal01.seq";
			} else if (typeId == 0x1A0000AB) {
				effect = "coin_medal02.seq";
			} else if (typeId == 0x1A0000AC) {
				effect = "coin_medal03.seq";
			} else if (typeId == 0x1A000010) {
				int coins = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_extPrizes[i].m_quantity;
				if (coins <= 10)
					coins = 10;
				else
					coins = coins <= 100 ? 20 : 30;

				SendMsg(this, "Effect", 0x17D, coins, 0, 0, 0);

				dropSpray = false;
			} else if (typeId == 0x1A000143) {
				effect = "4god_Bluedragon.seq";
				dropSpray = false;
			} else if (typeId == 0x1A000144) {
				effect = "4god_tiger.seq";
				dropSpray = false;
			} else if (typeId == 0x1A000145) {
				effect = "4god_FireBird.seq";
				dropSpray = false;
			} else if (typeId == 0x1A000146) {
				effect = "4god_Turtle.seq";
				dropSpray = false;
			}
		} else {
			IFF_STRUCT::sQuestDrop *drop = ItemManager()->FindQuestDrop(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_extPrizes[i].m_typeId);
			if (!drop)
				continue;
			effect = drop->m_effect;
		}

		if (!effect)
			continue;

		const char *ext = strrchr(effect, '.');
		if (ext && strlen(ext) >= 4) {
			CGolfBall &ball = GolfBall();

			if (stricmp(ext + 1, "spr") == 0) {
				CFxSpray *spray = CFx::Instance()->OpenSpray(effect);
				if (spray)
					spray->Pos() = GetHoleCupPos();
			} else {
				CFxSequence *seq = CFx::Instance()->OpenSequence(effect, true);
				if (seq)
					seq->Pos() = GetHoleCupPos();
			}

			if (dropSpray) {
				char sprayName[0x80] = "xmas_prize.spr";
				CCodeTinker tinker;
				const char *name = tinker.GetHoleDropEffectName(typeId);
				if (name)
					strcpy(sprayName, name);

				CFxSpray *spray = CFx::Instance()->OpenSpray(sprayName);
				if (spray)
					spray->Pos() = GolfBall().m_pos;
			}

			SendMsg(this, "Caddie", 0x1DE, 0x23, 0, 0, 0);

			if (IsMyTurn(true)) {
				char text[0x80] = { 0 };

				if (typeId == 0x1A000010)
					sprintf(text, "%d %s", PLAYER(GOLFDOC()->m_myPlayerIndex)->m_extPrizes[i].m_quantity, item->m_name);
				else
					sprintf(text, "%s", item->m_name);

				// "Congratulations! %s obtained"
				CChatMsg::Instance()->AddChatMsg(MakeStr("\xC3\xE0! %s \xC8\xB9\xB5\xE6", text), 0xFFFF7878, false, false);
			}
		}
	}

	PLAYER(GOLFDOC()->m_myPlayerIndex)->m_extPrizeCount = 0;
}

// The GCT_CLICK payload of a cancelled shot: the bar's start position right
// after the type byte.
#pragma pack(push, 1)
struct sCancelClickData {
	unsigned char type;         // 0x0
	float startPosition;        // 0x1
};
#pragma pack(pop)
CHECK_SIZE(sCancelClickData, 0x5);

// golfrule.cpp:15526
void CGolfRule::CancelShot(void) {
	timeEndPeriod(1);
	CFx::Instance()->SetActive(true);

	if (OnlinePlay() && IsMyTurn(false)) {
		sCancelClickData data;
		data.type = 0;
		data.startPosition = GOLFDOC()->GetBar().m_startPosition;

		WSendPacket packet(GCT_CLICK);
		packet.EncodeBuffer(&data, 5);
		packet.Send(SEND_TO_DEFAULT);
	}

	m_shotReady = false;
	GOLFDOC()->m_clockHold = 0;
	GOLFDOC()->GetBar().m_accuracy = GolfClub().GetType() == 3 ? 140.0f : 105.0f;
	GOLFDOC()->GetBar().m_prevAccuracy = GOLFDOC()->GetBar().m_accuracy;
	GOLFDOC()->GetBar().m_power = 140.0f;
	GOLFDOC()->GetBar().m_state = 0;
	GOLFDOC()->GetBar().m_stopped = false;
	CMouseCursor::Instance()->SetActive(true);

	if (CCapturedBg::Instance() != NULL)
		CCapturedBg::Instance()->Reset();

	SendMsg(this, "Camera", 0x19A, 0, 0, 0, 0);
	GOLFDOC()->GameMode() = 8;

	if (m_chatViewWnd != NULL)
		m_chatViewWnd->SetVisible(true);
}

// golfrule.cpp:15578
void CGolfRule::SaveErrorFile(void) {
	if (IsMyTurn(false)) {
		mkdir("error");
		int index = 0;

		do {
			sprintf(m_saveFileName, "error/%s_%03d.rep", GetMyComputerName().c_str(), index++);
		} while (_access(m_saveFileName, 0) != -1);

		SaveShot(m_saveFileName);
	}
}

// golfrule.cpp:15594
// Leaves the round for the lobby (online: the room or the room list), or for
// the family, ghost or tutorial task offline.
void CGolfRule::ReturnLobby(bool roomList) {
	if (CSharedDoc::Instance()->m_roomType != 10) {
		CSharedDoc::Instance()->m_gameOver = true;

		if (CTHunter::IsInstantiated()) {
			CTHunter::Instance()->SetReturnLobby(true);
		}
	}

	if (OnlinePlay()) {
		if (m_pRule) {
			if (IsMassGame()) {
				unsigned long playTime = GOLFDOC()->GetPlayer(0)->m_playTime / 1000;
				CSharedDoc::Instance()->m_gameStatistics[0].playTime = playTime;
			}
			CProjectG::Instance()->FromGolfToLobbySettings();
		}

		CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);

		if (roomList) {
			PostMsg(this, "Lobby", 0, (intptr_t)"ROOMLIST", 0, 0, 0);
		} else {
			if (IsMassGame()) {
				if (CSharedDoc::Instance()->m_roomType == 10) {
					PostMsg(this, "Lobby", 0, (intptr_t)"GAMEROOM_EXT", 0, 0, 0);
				} else {
					PostMsg(this, "Lobby", 0, (intptr_t)"GAMEROOM_EXTRES", 0, 0, 0);
				}
			} else {
				PostMsg(this, "Lobby", 0, (intptr_t)"GAMEROOM", 0, 0, 0);
			}

			PostMsg(this, "Lobby", 0x2F, 0, 0, 0, 0);
			PostMsg(this, "Lobby", 0xF, 0, 0, 0, 0);
			PostMsg(this, "Lobby", 0xE, 0, 0, 0, 0);

			if (CSharedDoc::Instance()->m_roomType == 6) {
				if (CSharedDoc::Instance()->m_gameStatistics[0].exp != 0xFFFFFFFF) {
					PostMsg(this, "Lobby", 0x2D, 0, 0, 0, 0);
				}
			}
		}

		PostMsg(this, "Lobby", 0x1A, 0, 0, 0, 0);

		if (CSceneManager::Instance()) {
			SendMsg(this, "GolfBg", 0x142, 0, 0, 0, 0);
		}
	} else {
		if (!WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
			g_bQuit = true;
			return;
		}

		if (m_pRule) {
			CProjectG::Instance()->FromGolfToLobbySettings();
		}

		if (CSharedDoc::Instance()->m_roomType == 11 || CSharedDoc::Instance()->m_roomType == 12) {
			CSharedDoc::Instance()->m_gameMode = 0;
			CTaskManager::Instance()->ChangeTask("CTutorialTask", "", false);
			CTaskManager::Instance()->PostMsg(0, "TutorialMain", 0, (intptr_t)"TUTORIAL_MAIN", 0, 0, 0);
		} else if (CSharedDoc::Instance()->m_gameMode == 1) {
			CTaskManager::Instance()->ChangeTask("CFamilyTask", "", false);
			PostMsg(this, "Family", 0, (intptr_t)"FAMILY", 0, 0, 0);
		} else if (CSharedDoc::Instance()->m_gameMode == 5) {
			CTaskManager::Instance()->ChangeTask("CGhostTask", "", CTaskManager::Instance()->GetPreservedTask() != 0);
			CTaskManager::Instance()->PostMsg(0, "Ghost", 0, (intptr_t)"GHOST", 0, 0, 0);
		}
	}
}

// golfrule.cpp:15704
// Leaving the round early: a web login quits the client; otherwise the
// player's game pang is kept for the lobby (0 without a player).
bool CGolfRule::ReturnToLobbyDlgResult(int, FrForm *) {
	if (CLoginInfo::Instance()->IsWebLogin()) {
		g_bQuit = true;
	} else {
		if (PLAYER(MyIndex()))
			CSharedDoc::Instance()->m_quitGamePang = PLAYER(MyIndex())->m_ghostAward;
		else
			CSharedDoc::Instance()->m_quitGamePang = 0;

		ReturnLobby(true);
	}

	return true;
}

// golfrule.cpp:15720
void CGolfRule::ToggleGameUI(void) {
	SendMsg(this, "Screen", 0x190, m_showGameUI ? 1 : 0, 0, 0, 0);
	SendMsg(this, "Letter", 0x190, m_showGameUI ? 1 : 0, 0, 0, 0);

	SendMsg(this, "VideoRecorder", 0x190, m_showGameUI ? 1 : 0, 0, 0, 0);

	if (g_pFresh)
		g_pFresh->Toggle(m_showGameUI);

	CMouseCursor::Instance()->Toggle(m_showGameUI);
	CChatMsg::Instance()->Toggle(m_showGameUI);
}

// golfrule.cpp:15739
bool CGolfRule::CheckAllHoleOut(void) {
	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (GOLFDOC()->GetPlayer(i)->m_state == 0)
			return false;
	}
	return true;
}

// golfrule.cpp:15750
bool CGolfRule::PauseDlgResult(int result, FrForm *) {
	CMouseCursor::Instance()->m_uiEnable[3] = true;

	m_pauseDlg = NULL;

	if (OnlinePlay()) {
		if (result) {
			WSendPacket packet(GCT_PAUSE);
			packet.Encode1(0);
			packet.Send(SEND_TO_DEFAULT);
		}
	} else {
		GOLFDOC()->m_pause = false;
	}

	return true;
}

// golfrule.cpp:15773
// The "continue practice?" dialog: yes restarts the round for player 0 from
// the first hole, no returns to the lobby.
bool CGolfRule::ContinueDlgResult(int result, FrForm *) {
	m_continueDlgOpen = false;

	if (result == 1) {
		GOLFDOC()->GetPlayer(0)->m_state = 2;
		GOLFDOC()->GetPlayer(0)->m_comboGauge = 0.0f;
		GOLFDOC()->GetPlayer(0)->m_totalScore = 0;
		GOLFDOC()->GetPlayer(0)->m_playTime = 0;
		GOLFDOC()->GetPlayer(0)->m_joinState = 1;
		GOLFDOC()->GetPlayer(0)->m_comboGauge = 0.0f;
		GOLFDOC()->GetPlayer(0)->m_ghostResult = 0;
		GOLFDOC()->GetPlayer(0)->m_ghostAward = 0;
		GOLFDOC()->GetPlayer(0)->m_extraPang = 0;

		GOLFDOC()->GetPlayer(0)->m_recordedHoles = 0;

		for (int i = 0; i < 18; i++) {
			GOLFDOC()->GetPlayer(0)->m_holes[i] = 0;
			GOLFDOC()->GetPlayer(0)->m_holeIn[i] = 0;
			GOLFDOC()->GetPlayer(0)->m_holeRecord[i] = 0;
		}

		if (CSharedDoc::Instance()->m_gameMode == 2) {
			memcpy(CSharedDoc::Instance()->m_roomPlayers[0].m_equipItem, CSharedDoc::Instance()->m_equipItem, sizeof(CSharedDoc::Instance()->m_equipItem));

			PLAYER(0)->m_numItems = CSharedDoc::Instance()->CollapseItemSlot(0);
		}

		GOLFDOC()->m_currentHole = CSharedDoc::Instance()->m_holeOrder[0];
		SendMsg(this, "GolfBg", 0x140, 0, 0, 0, 0);
	} else if (result == 2) {
		ReturnLobby(false);
	}

	return true;
}

// golfrule.cpp:15827
bool CGolfRule::CancelDropDlgResult(int result, FrForm *) {
	m_dropDlg = NULL;
	if (result == 1) {
		GOLFDOC()->m_dropPending = false;
		ChangeGameMode(8);
	}
	return true;
}

// golfrule.cpp:15840
bool CGolfRule::GoStopDlgResult(int result, FrForm *) {
	m_goStopDlg = NULL;
	if (result) {
		WSendPacket packet(GCT_ANSWER_GOSTOP);
		packet.Encode1(result == 1);
		packet.Send(SEND_TO_DEFAULT);

		GOLFDOC()->m_clockStopped = false;
		m_goStopSent = true;
		SetNetMsgWait(true, 0x3DEB, "GoStopDlgResult", 20.0f);
	}

	return true;
}

// golfrule.cpp:15858
// The emoticon picker closed: with OK, the chosen icon goes into the visible
// chat input when it still fits, else straight to the chat (and the local
// chat view offline).
bool CGolfRule::OnEmoticonResult(int result, FrForm *form) {
	if (!CChatMsg::Instance()->IsOpened())
		g_pFresh->GetManager()->SetExclusiveKey(false);

	if (m_emoticonDlg && result == FrOK) {
		FrEmoticonDlg *dlg = DYNAMIC_CAST(FrEmoticonDlg, form);
		if (dlg) {
			const char *icon = dlg->GetSelectedIcon();
			if (icon) {
				if (m_chatInput && m_chatInput->IsVisible()) {
					const char *front = m_chatInput->GetEditText_Front();
					const char *comp = m_chatInput->GetEditText_Comp();
					const char *end = m_chatInput->GetEditText_End();

					if (CChatMsg::Instance()->GetMaskedFont()->GetTextWidth(g_view, MakeStr("%s%s%s%s", front, comp, icon, end)) < m_chatInput->GetWidthLimit())
						m_chatInput->SetLine(1, MakeStr("%s%s%s%s", front, comp, icon, end), 0, false, 0);
					m_chatInput->SetKeyFocus(true);
				} else {
					CSharedDoc::Instance()->m_chatManager.SendChatMessage(icon, NULL, false);

					if (!OnlinePlay()) {
						std::string msg(CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_name);

						msg += " : \x08";
						msg += icon;
						CChatMsg::Instance()->AddChatMsg(msg, 0xFFFFCC00, false, false);
					}
				}
			}
		}
	}

	m_emoticonDlg = NULL;
	return true;
}

// golfrule.cpp:15909
bool CGolfRule::OnReportResult(int result, FrForm *) {
	if (result == 1) {
		GOLFDOC()->SetReport(true);

		WSendPacket packet(GCT_REPORT);
		packet.Send(SEND_TO_DEFAULT);
	}

	m_reportDlg = NULL;
	return true;
}

// golfrule.cpp:15923
bool CGolfRule::OnReturnLobbyResult(int, FrForm *) {
	ReturnLobby(false);
	return true;
}

// golfrule.cpp:15929
bool CGolfRule::OnForgetDlgResult(int result, FrForm *) {
	if (result == 1) {
		SendMsg(this, "Item", 0x19E, m_cancelShotItemSlot, 0, 0, 0);
		SetNetMsgWait(true, 0x3E3D, "PM_NET_CANCEL_SHOT", 20.0f);
	} else if (result == 2) {
		SendShotResult();
		SendMsg(this, "Item", 0x195, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1AF, 0, 0, 0, 0);
	}

	m_forgetPending = false;
	m_forgetElapsed = 0;
	m_forgetDlg = NULL;

	return true;
}

// golfrule.cpp:15952
bool CGolfRule::OnPlayerInfoDlgResult(int, FrForm *) {
	m_playerInfoDlg = NULL;
	return true;
}

// golfrule.cpp:15959
bool CGolfRule::OnApproachEndDlgResult(int, FrForm *) {
	if (m_approachEndDlg) {
		if (!CSharedDoc::Instance()->m_value90) {
			CSharedDoc::Instance()->m_value90 = 1;
		}

		if (!GOLFDOC()->IsNetGameEnd()) {
			SendMsg(this, "Ball", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Spot", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Grid", 0x164, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x1A8, 0, 0, 0, 0);
			SendMsg(this, "Screen", 0x1A5, 0, 0, 0, 0);
			SendMsg(this, "Effect", 0x177, 0, 0, 0, 0);
			SendMsg(this, "Trajectory", 0x18C, 0, 0, 0, 0);
			SendMsg(this, "Trajectory", 0x160, 0, 0, 0, 0);

			CFx::Instance()->ClearAll(true);

			ChangeGameMode(0x2000);
		}

		if (CTHunter::IsInstantiated()) {
			CTHunter::Instance()->SetReturnLobby(true);
		}

		CSharedDoc::Instance()->m_gameOver = true;

		m_approachEndDlg = NULL;
	}

	return true;
}

// golfrule.cpp:16009
bool CGolfRule::OnApproachDlgResult(int, FrForm *) {
	if (m_approachResultDlg) {
		if (m_approachResultDlg->GetApproachEnd()) {
			if (m_approachEndDlg == NULL) {
				m_approachEndDlg = CreateForm<FrApproachEndDlg>(g_pFresh->GetManager(), this, "approach_end", 0);
				m_approachEndDlg->EnableDrag(false);

				WRect rect(m_approachEndDlg->GetRect());
				m_approachEndDlg->Open((FRESH_RESULT_CALLBACK)&CGolfRule::OnApproachEndDlgResult,
					WPoint(g_view->GetWidth() * 0.5f - rect.w * 0.5f, 35.0f), 0);
				m_approachEndDlg->SetTimeLimit(15.0f);
				// "Approach final result"
				m_approachEndDlg->SetCaption("\xBE\xEE\xC7\xC1\xB7\xCE\xC4\xA1 \xC3\xD6\xC1\xBE\xB0\xE1\xB0\xFA");
			}
		} else {
			ChangeGameMode(0x800000);
		}

		m_approachResultDlg = NULL;
	}

	return true;
}

// golfrule.cpp:16045
// The round's background musics by course (PlayBGM picks one). The original
// takes the user music directory but no longer reads it.
void CGolfRule::CheckUserBGM(const char *directory) {
	m_bgmList.clear();

	switch (GetCurMap()) {
	case 4:
	case 5:
		m_bgmList.push_back("shiny.mp3");
		m_bgmList.push_back("bunny.mp3");
		break;

	case 7:
		m_bgmList.push_back("navy_blue.mp3");
		m_bgmList.push_back("rising_sun.mp3");
		break;

	case 8:
		m_bgmList.push_back("happy flight.mp3");
		m_bgmList.push_back("crystal waver.mp3");
		break;

	case 14:
		m_bgmList.push_back("crystal lake.mp3");
		m_bgmList.push_back("fade into white.mp3");
		break;

	case 9:
		m_bgmList.push_back("snowscape.mp3");
		m_bgmList.push_back("winter_ride.mp3");
		break;

	case 10:
		m_bgmList.push_back("somewhere.mp3");
		m_bgmList.push_back("nowhere.mp3");
		break;

	case 13:
		m_bgmList.push_back("volcano.mp3");
		m_bgmList.push_back("vermilion.mp3");
		break;

	case 15:
		m_bgmList.push_back("the mystery of the lost seaway.mp3");
		m_bgmList.push_back("voyage the sky.mp3");
		break;

	case 16:
		m_bgmList.push_back("eastern_valley.mp3");
		m_bgmList.push_back("river.mp3");
		break;

	case 17:
		m_bgmList.push_back("volcano.mp3");
		m_bgmList.push_back("river.mp3");
		break;

	case 19:
		m_bgmList.push_back("SecretWish.mp3");
		m_bgmList.push_back("ADayInTheWizCity.mp3");
		break;

	default:
		m_bgmList.push_back("samba3.mp3");
		m_bgmList.push_back("daydream.mp3");
		m_bgmList.push_back("frog.mp3");
		m_bgmList.push_back("spring.mp3");
		break;
	}
}

// golfrule.cpp:16177
bool CGolfRule::CanCancelShot(void) {
	for (int i = 0; i < 10; i++) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_equipItem[i] == 0x1800000E) {
			m_cancelShotItemSlot = i;
			return true;
		}
	}
	return false;
}

// golfrule.cpp:16190
// Online, sends the local player's shot result to the server (packet 0x1B):
// where the ball stopped, how the shot ended, the pang and bonus flags and
// the ball's shot flags, encrypted with the room key when content 0x2D is on,
// followed by the key.
void CGolfRule::SendShotResult(void) {
	if (!OnlinePlay())
		return;

	WSendPacket packet((enumClientPacket)0x1B);
	sShotResult result;

	CGolfBall &ball = GolfBall();

	result.m_uid = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_uid;

	for (int i = 0; i < 3; i++)
		result.m_pos.p[i] = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ballPos.p[i];

	result.m_powerShotLevel = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel;

	if (ball.m_landed) {
		result.m_resultType = 3;
	} else if (ball.m_holeIn == 1) {
		result.m_resultType = 4;
	} else {
		result.m_resultType = ball.m_bounceCount ? 2 : 5;
	}

	result.m_replayMask = (unsigned char)m_replayFlags;

	result.m_pang = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ghostAward;
	result.m_clearBonus = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_extraPang;

	result.m_bonusFlags = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_bonusFlags;
	result.m_preDataCount = (short)ball.m_preDataCount;

	result.m_shotFlags = 0;

	if (ball.m_flags & 0x10) {
		result.m_shotFlags = 2;
	} else if (ball.m_flags & 0x40) {
		result.m_shotFlags = 4;
	} else if (ball.m_flags & 0x20) {
		result.m_shotFlags = 8;
	}

	if (ball.m_flags & 1) {
		result.m_shotFlags |= 0x10;
	} else if (ball.m_flags & 2) {
		result.m_shotFlags |= 0x20;
	}

	if (ball.m_flags & 4) {
		result.m_shotFlags |= 0x40;
	} else if (ball.m_flags & 8) {
		result.m_shotFlags |= 0x80;
	}

	if (IsLocalContent((localContentType_t)0x2D))
		CEncryption::Instance()->Encrypt(&result, WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_key);

	packet.EncodeBuffer(&result, sizeof(result));
	packet.EncodeBuffer(&WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_key, sizeof(EncryptKey));

	packet.Send(SEND_TO_DEFAULT);
}

// golfrule.cpp:16287
// Reports a stuck network wait to the server: the lock and unlock packet
// names with their tick counts and the time since the lock.
void CGolfRule::SendReportError(unsigned char error) {
	WSendPacket packet(0x33);

	if (m_netMsgWait == NULL)
		m_netMsgWait = "UnknownLock";

	if (m_netMsgWaitRecv == NULL)
		m_netMsgWaitRecv = "UnknownUnlock";

	std::string report;
	char text[128];
	report = m_netMsgWait;
	sprintf(text, "(%d) / ", m_netMsgSendTime);
	report += text;
	report += m_netMsgWaitRecv;
	sprintf(text, "(%d)", m_netMsgRecvTime);
	report += text;
	sprintf(text, "/ ET(%d)", GetTickCount() - m_netMsgSendTime);
	report += text;

	if (error == 3) {
		sprintf(text, "[Pang Hacker]");
		report += text;
	}

	packet.Encode1(error);
	packet.EncodeStr(report);
	packet.Send(SEND_TO_DEFAULT);
}

// golfrule.cpp:16321
void CGolfRule::ResetUiPos_Board(void) {
	WRect rect = m_boardWnd->GetRect();
	rect.x = g_view->GetWidth() - 140.0f;
	m_boardWnd->SetRect(rect);

	rect = m_onelineBoardWnd->GetRect();
	rect.x = g_view->GetWidth() - 317.0f;
	m_onelineBoardWnd->SetRect(rect);
	COneLineBoard::Instance()->SetRect(m_onelineBoardWnd->GetRect());
}

// golfrule.cpp:16333
void CGolfRule::PlayBGM(void) {
	if (GolfBall().m_groundType == 2)
		SetConstantBGM(g_bgmPutting[rand() % 2], false, true);
	else
		SetConstantBGM(m_bgmList[m_bgmIndex].c_str(), false, true);
}

// golfrule.cpp:16354
int CGolfRule::HoleReplay2(void) {
	return 0;
}

// golfrule.cpp:16359
void CGolfRule::OnMessengerBtnInit(PG_MSGARG param) {
	FrButton *button = (FrButton *)((__rtti_obj = (void *)param) != 0 ? (((IObject *)__rtti_obj)->IsKindOf(&FrButton::m_RTTI) ? (void *)param : 0) : 0);
	if ((m_messengerWnd = button) != 0) {
		if (!WSingleton<CSharedDoc>::Instance()->m_value4DBE)
			button->EnableBlink(false);

		m_messengerWnd->SetVisible(false);
		m_messengerWnd->EnableHover(true, 0.0f);

		WPoint pt;
		pt.x = g_view->GetWidth() - 81.0f;
		pt.y = m_messengerWnd->GetRect().y;
		m_messengerWnd->MoveWindow(pt);

		CMessengerInfo::Instance()->m_rcAlarmBtn = m_messengerWnd->GetRect();
	}
}

// golfrule.cpp:16381
void CGolfRule::OnMessengerBtnUp(void) {
	CMessengerInfo::Instance()->Open(true);

	OnBarHoverOff(0);
}

// golfrule.cpp:16390
void CGolfRule::OnMessengerBubbleEditInit(PG_MSGARG param) {
	m_messengerBubbleEdit = (FrEdit *)((__rtti_obj = (void *)param) != 0 ? (((IObject *)__rtti_obj)->IsKindOf(&FrEdit::m_RTTI) ? (void *)param : 0) : 0);
	if (m_messengerBubbleEdit) {
		m_messengerBubbleEdit->SetVisible(false);

		WPoint pt;
		pt.x = g_view->GetWidth() - 167.0f;
		pt.y = m_messengerBubbleEdit->GetRect().y;
		m_messengerBubbleEdit->MoveWindow(pt);
	}
}

// golfrule.cpp:16404
void CGolfRule::OnMessenger_AlarmBtnInit(PG_MSGARG param) {
	m_messengerAlarmBtn = DYNAMIC_CAST(FrButton, param);
	if (m_messengerAlarmBtn) {
		m_messengerAlarmBtn->SetTopmost(true);

		if (m_messengerAlarmBtn && !WSingleton<CSharedDoc>::Instance()->m_value4DBD) {
			m_messengerAlarmBtn->SetVisible(false);
			m_messengerAlarmBtn->SetTopmost(false);
		}

		WPoint pt;
		pt.x = g_view->GetWidth() - 110.0f;
		pt.y = m_messengerAlarmBtn->GetRect().y;
		m_messengerAlarmBtn->MoveWindow(pt);
	}
}

// golfrule.cpp:16424
void CGolfRule::OnMessenger_AlarmBtnUp(void) {
	CMessengerInfo::Instance()->AlarmBtnUp(0xFFFFFFFF);
}

// golfrule.cpp:16430
void CGolfRule::OnMessenger_GiftAlarmBtnInit(PG_MSGARG param) {
	m_messengerGiftAlarmBtn = DYNAMIC_CAST(FrButton, param);
	if (m_messengerGiftAlarmBtn) {
		m_messengerGiftAlarmBtn->SetVisible(false);
		m_messengerGiftAlarmBtn->SetTopmost(false);

		WPoint pt;
		pt.x = g_view->GetWidth() - 75.0f;
		pt.y = m_messengerGiftAlarmBtn->GetRect().y;
		m_messengerGiftAlarmBtn->MoveWindow(pt);
	}
}

// golfrule.cpp:16445
void CGolfRule::OnMessenger_GiftAlarmBtnUp(void) {
	m_messengerGiftAlarmBtn->SetVisible(false);
	m_messengerGiftAlarmBtn->SetTopmost(false);

	FrGiftAlarmDlg *dlg = CreateForm<FrGiftAlarmDlg>(g_pFresh->GetManager(), this, "gift_alarm", 0);
	dlg->Open((FRESH_RESULT_CALLBACK)&CGolfRule::OnNotifyDlgResult, 0x11);
	dlg->SetTopmost(true);
	dlg->FindNextTopFocus(false);
	dlg->SetIconRect(m_messengerGiftAlarmBtn->GetRect());
}

// golfrule.cpp:16458
void CGolfRule::OnReplayStatusInit(PG_MSGARG param) {
	m_replayBoxWnd = DYNAMIC_CAST(FrArea, param);

	if (!IsLocalContent((localContentType_t)0x4F))
		m_replayBoxWnd->SetVisible(false);

	if (m_replayBoxWnd) {
		WRect rect = m_replayBoxWnd->GetRect();
		rect.x = g_view->GetWidth() - (800.0f - rect.x);
		m_replayBoxWnd->SetRect(rect);
	}
}

// golfrule.cpp:16477
// Creates the rule object of the room's game type.
void CGolfRule::SetCurrentGolfRule(void) {
	if (m_pRule) { delete m_pRule; m_pRule = NULL; }

	switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
	case 1:
		m_pRule = new CGolfRuleTeam(this);
		break;
	case 7:
		m_pRule = new CGolfRuleSkins(this);
		break;
	case 3:
		m_pRule = new CGolfRuleMatch(this);
		break;
	case 0:
		m_pRule = new CGolfRuleStroke(this);
		break;
	case 9:
		m_pRule = new CGolfRuleApproach(this);
		break;
	case 10:
		m_pRule = new CGolfRuleNewApproach(this);
		break;
	case 6:
		m_pRule = new CGolfRuleGuildMatch(this);
		break;
	case 4:
	case 5:
		m_pRule = new CGolfRule30Battle(this);
		break;
	case 11:
	case 12:
		m_pRule = new CGolfRuleTutorial(this);
		// One source line in the original (golfrule.cpp:16535).
		if (WSingleton<CSharedDoc>::Instance()->m_value4D98 == 14) GOLFDOC()->m_currentHole = 2; else GOLFDOC()->m_currentHole = 1;
		break;
	default:
		m_pRule = NULL;
		break;
	}
}

// golfrule.cpp:16547
bool CGolfRule::IsMaxWithHGParts(unsigned char index) {
	// The four parts of the HG set.
	static const unsigned long s_hgParts[] = {
		0x08104011, 0x0810A00C, 0x08100008, 0x08110152
	};

	CPartTidList *partList = &GOLFDOC()->GetPartTidList(index);

	if (partList == NULL)
		return false;

	bool equipped = false;
	for (int i = 0; i < sizeof(s_hgParts) / sizeof(s_hgParts[0]); i++) {
		if (partList->IsEquipped(s_hgParts[i], 0)) {
			equipped = true;
			break;
		}
	}

	return equipped;
}

// golfrule.cpp:16582
void CGolfRule::_ImeReset(void) {
	if (IsLocalContent((localContentType_t)0x2A)) {
		if (!CChatMsg::Instance()->IsActive()) {
			g_ime->SetActive(true);
			g_ime->SetActive(false);
			g_ime->Reset();
		}
	}
}

// golfrule.cpp:16596
void CGolfRule::ReplayBoxOpen(void) {
	if (!m_replayBox) {
		float x = 571.0f - (800.0f - g_view->GetWidth());

		m_replayBox = CreateForm<FrReplayControlDlg>(g_pFresh->GetManager(), this, "replaycontroldlg", 0);
		m_replayBox->Open((FRESH_RESULT_CALLBACK)&CGolfRule::OnReplayModeDlgResult, WPoint(x, 55.0f), 0x11);
	} else {
		m_replayBox->SetVisible(true);
	}

	if (m_replayBoxWnd)
		m_replayBoxWnd->SetVisible(false);
}

// golfrule.cpp:16616
void CGolfRule::ReplayBoxClose(void) {
	if (m_replayBox) {
		m_replayBox->Close(true);
	}
	if (m_replayBoxWnd) {
		m_replayBoxWnd->SetVisible(true);
	}
}

// golfrule.cpp:16627
bool CGolfRule::OnReplayModeDlgResult(int, FrForm *) {
	if (WSingleton<CSharedDoc>::Instance()->m_playingMode == 2 && WSingleton<CSharedDoc>::Instance()->m_replayState == 5) {
		WSingleton<CSharedDoc>::Instance()->m_replayState = 0;

		WSingleton<CSharedDoc>::Instance()->SetRmrMode(1);
		WSingleton<CSharedDoc>::Instance()->m_myRoomOwnerUID = MyUID();
		WSingleton<CSharedDoc>::Instance()->m_playingMode = (eReplayModeType)3;

		SetBGMNone();
		CProjectG::Instance()->FromGolfToLobbySettings();
		CTaskMain::OnUnderBar_MyRoomUp();

		CTaskManager::Instance()->PostMsg(NULL, "RealMyRoom", 0xC6, 2, 0, 0, 0);
	}

	m_replayBox = NULL;
	return true;
}

// golfrule.cpp:16656
// The "record replay" button: checks the replay tape item and the count of
// saved replays, then saves the shot data and asks the server to use a tape.
void CGolfRule::ReplayTapeCheckAndUse(void) {
	if (GetGameType() == 10 || WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomMode == 14) {
		if (WSingleton<CHelpMsg>::IsInstantiated())
			WSingleton<CHelpMsg>::Instance()->SetHelpMsg(0x19, 0);
		return;
	}

	unsigned long typeId;

	if (IsLocalContent((localContentType_t)0x4F)) {
		typeId = WSingleton<CSharedDoc>::Instance()->GetReplayPlayType() == 2 ? 0x1A00004F : 0;
	} else {
		typeId = 0x1A000000;
	}

	std::list<sItemInfo>::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end() && (*it).m_typeId != typeId; ++it)
		;

	WSingleton<CSharedDoc>::Instance()->ReplaySavFileCheck();

	if (it == WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end()) {
		if (IsLocalContent((localContentType_t)0x4F)) {
			if (typeId == 0x1A00004F) {
				WSingleton<CSharedDoc>::Instance()->m_replayPlayType = 0;
				WSingleton<CChatMsg>::Instance()->AddChatMsg("\xB8\xAE\xC7\xC3\xB7\xB9\xC0\xCC \xC5\xD7\xC0\xCC\xC7\xC1\xB0\xA1 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0xFFFF7878, true, false);
			} else if (typeId == 0x1A000000) {
				WSingleton<CSharedDoc>::Instance()->m_replayPlayType = 0;
				WSingleton<CChatMsg>::Instance()->AddChatMsg("\xB8\xAE\xC7\xC3\xB7\xB9\xC0\xCC \xC5\xD7\xC0\xCC\xC7\xC1\xB0\xA1 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0xFFFF7878, true, false);
			}
		} else {
			WSingleton<CChatMsg>::Instance()->AddChatMsg("\xB8\xAE\xC7\xC3\xB7\xB9\xC0\xCC \xC5\xD7\xC0\xCC\xC7\xC1\xB0\xA1 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0xFFFF7878, true, false);
		}
	} else if (IsLocalContent((localContentType_t)0x4F) && WSingleton<CSharedDoc>::Instance()->m_replayFileList.size() >= 10) {
		WSingleton<CSharedDoc>::Instance()->m_replayPlayType = 0;
		WSingleton<CChatMsg>::Instance()->AddChatMsg("\xB1\xE2\xB7\xCF\xB5\xC8 \xB8\xAE\xC7\xC3\xB7\xB9\xC0\xCC \xC5\xD7\xC0\xCC\xC7\xC1\xB8\xA6 10\xB0\xB3 \xB3\xD1\xB0\xD4 \xBB\xFD\xBC\xBA \xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0xFFFF7878, true, false);
	} else if (OnlinePlay()) {
		if (IsLocalContent((localContentType_t)0x4F)) {
			int result;

			switch (GetGameType()) {
			case 0:
			case 1:
			case 4:
			case 5:
			case 6:
			case 7:
				result = ShotDataSave(0, false);
				break;
			default:
				result = 0;
				break;
			}

			if (result == 1) {
				WSendPacket packet((enumClientPacket)0x4A);
				packet.Encode4(typeId);
				packet.Send(SEND_TO_DEFAULT);
			} else if (result == -1) {
				WSingleton<CChatMsg>::Instance()->AddChatMsg(MakeStr("\xB8\xAE\xC7\xC3\xB7\xB9\xC0\xCC \xC6\xC4\xC0\xCF\xC0\xCC 100\xB0\xB3\xB8\xA6 \xC3\xCA\xB0\xFA\xC7\xCF\xBF\xA9 \xC0\xFA\xC0\xE5\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9. \xBB\xE8\xC1\xA6 \xC8\xC4 \xB4\xD9\xBD\xC3 \xBD\xC3\xB5\xB5\xC7\xCF\xBF\xA9 \xC1\xD6\xBD\xC3\xB1\xE2 \xB9\xD9\xB6\xF8\xB4\xCF\xB4\xD9."), 0xFFFF7878, true, false);
			} else if (result == 0) {
				WSingleton<CChatMsg>::Instance()->AddChatMsg(MakeStr("\xB8\xAE\xC7\xC3\xB7\xB9\xC0\xCC\xB8\xA6 \xC0\xFA\xC0\xE5\xC7\xCF\xB4\xC2 \xB5\xB5\xC1\xDF \xB9\xAE\xC1\xA6\xB0\xA1 \xBF\xC0\xB7\xF9\xB0\xA1 \xB9\xDF\xBB\xFD \xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9. \xB4\xD9\xBD\xC3 \xBD\xC3\xB5\xB5\xC7\xCF\xBF\xA9 \xC1\xD6\xBD\xC3\xB1\xE2 \xB9\xD9\xB6\xF8\xB4\xCF\xB4\xD9."), 0xFFFF7878, true, false);
			}
		} else {
			WSendPacket packet((enumClientPacket)0x4A);
			packet.Encode4(typeId);
			packet.Send(SEND_TO_DEFAULT);
		}
	} else {
		WSendPacket packet((enumClientPacket)0x49);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// golfrule.cpp:16773
void CGolfRule::OnRecordAviBtnUp(void) {
	if (m_recordAviDlg == NULL) {
		char *installDir = GetPangyaInstallDic();
		char text[255] = "";
		m_recordAviDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);

		// "Save the replay as a video file?  Save path: %s/replay"
		sprintf(text, "\xB8\xAE\xC7\xC3\xB7\xB9\xC0\xCC \xC5\xD7\xC0\xD9\xC0\xC7 \xB3\xBB\xBF\xEB\xC0\xBB\n\xB5\xBF\xBF\xB5\xBB\xF3 \xC6\xC4\xC0\xCF\xB7\xCE \xC0\xFA\xC0\xE5\xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?\n\n\t\t\t\t\t\t   \xC0\xFA\xC0\xE5 \xB0\xE6\xB7\xCE\n\t\t\t\t\t\t   %s/replay", installDir);
		m_recordAviDlg->SetMessage(text, false);
		m_recordAviDlg->Open((FRESH_RESULT_CALLBACK)&CGolfRule::OnRecordAviResult, 0);
	}
}

// golfrule.cpp:16788
bool CGolfRule::OnRecordAviResult(int result, FrForm *) {
	if (result == 1)
		WSingleton<CSharedDoc>::Instance()->SetReplayState(9);

	m_recordAviDlg = NULL;
	return true;
}

// golfrule.cpp:16803
char *CGolfRule::GetPangyaInstallDic(void) {
	static char s_installDir[MAX_PATH];

	CRegistry registry;
	registry.SetRootKey(HKEY_LOCAL_MACHINE);
	registry.SetKey("Software\\Ntreev\\Pangya_QA", TRUE);

	WTL::CString installDir = "";
	installDir = registry.ReadString("Install_Dir", "");
	sprintf(s_installDir, "%s", (const char *)installDir);

	return s_installDir;
}

// golfrule.cpp:16819
void CGolfRule::ReplayDlgRun(void) {
	if (m_replayBoxWnd && m_replayBoxWnd->IsVisible() && CMouseCursor::Instance()->InArea(m_replayBoxWnd->GetRect())) {
		if (m_replayBox && !m_replayBox->IsVisible()) {
			m_replayBox->SetVisible(true);
		} else {
			ReplayBoxOpen();
		}
	}

	if (!m_replayBox || !m_replayBox->IsVisible()) {
		if (m_replayBoxWnd)
			m_replayBoxWnd->SetVisible(true);
	}

	if (m_replayBoxWnd && m_replayBoxWnd->IsVisible()) {
		switch (WSingleton<CSharedDoc>::Instance()->m_replayState) {
		case 2:
		case 3:
			m_replayBoxWnd->SetBgImg("replay_icon_play_n");
			break;
		case 4:
			m_replayBoxWnd->SetBgImg("replay_icon_stop_n");
			break;
		}
	}
}

// golfrule.cpp:16883
// Appends, three times in ten, the active cards (type, effect, value) and the
// global wind's intensity to a shot packet; otherwise a zero marker.
void CGolfRule::AddSomeInfo(WSendPacket &packet) {
	unsigned char chance = rand() % 100;

	if (chance < 30) {
		packet.Encode2(1);

		std::list<unsigned long> &cards = CCardManager::Instance()->GetActiveCardList();
		unsigned long count = cards.size();
		unsigned int countOffset = packet.GetLengthData();
		packet.Encode4(count);
		std::list<unsigned long>::iterator it = cards.begin();
		for (; it != cards.end(); ++it) {
			IFF_STRUCT::sCard *pCard = ItemManager()->FindCard(*it);
			if (pCard) {
				struct {
					unsigned long typeId;
					unsigned short effectType;
					unsigned short effectValue;
				} info;
				memset(&info, 0, sizeof(info));
				info.typeId = pCard->m_typeId;
				info.effectType = pCard->m_effectType;
				info.effectValue = pCard->m_effectValue;
				packet.EncodeBuffer(&info, 8);
			} else {
				count--;
			}
		}

		if (count != cards.size())
			packet.EncodeData(countOffset, (char *)&count, 4);

		if (IsLocalContent((localContentType_t)0x87)) {
			int value;
			if (Wind().IsGlobalWind_Compare() == true) {
				if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000006
					|| PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000020) {
					packet.Encode1(1);
					value = 1;
				} else {
					unsigned long intensity = (int)Wind().GetGlobalIntensity();

					if (Wind().IsActiveSpecialWind()) {
						packet.Encode1(2);
						value = intensity;
					} else {
						packet.Encode1(0);
						float level = intensity - 1.0f;

						if (!(level < 1.0f))
							intensity--;
						value = intensity;
					}
				}
			} else {
				value = 0;
			}
			packet.Encode4(value);
		} else {
			packet.Encode4((int)Wind().GetGlobalIntensity());
		}
	} else {
		packet.Encode2(0);
	}
}

// golfrule.cpp:16973
// The observer's keys: NUM_9/PAD_9 leaves the observer view (mode 0x200000);
// in a mass game, at most every 3 seconds, keys 1 to 8 (pad or numpad) ask
// the server (packet 0x3F) to follow a player: 1 the first whose lobby
// identity has flag 0x10, 2 a random one outside the top three, 3 to 5 the
// first to third, 6 and 7 the last and second-to-last, 8 (local country 1)
// the first wearing title 0x30000001. Before choosing, each player still
// playing (m_state 0) gets its rank in m_present: 1 + the players ahead by
// score, then pang, then m_value13F.
// The observer test goes through a local: tested directly, cl folds it into
// the branch, the original materializes it (sete; test eax, eax).
// Residue (86.65%): register allocation only. The original keeps uid in ebx
// and playerCount in memory/dl; ours gives ebx to playerCount and keeps uid
// in memory. Declaration scope and order, uid/playerCount types, loop and
// iterator scoping, if (key < 9) after the loop, and per-case found blocks
// were tried without moving it.
void CGolfRule::ProcessObserver(void) {
	int observer = IsObserver(0xFFFFFFFF);
	if (!observer)
		return;

	if (g_input->GetDown("NUM_9", true) || g_input->GetDown("PAD_9", true)) {
		if (GOLFDOC()->m_gameMode & 0x8338) {
			ChangeGameMode(0x200000);
			return;
		}
	}

	if (GetGameType() == 0) {
		if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) && g_input->GetDown("INSERT", true)) {
			WSendPacket packet((enumClientPacket)0x5D);
			packet.Send(SEND_TO_DEFAULT);
		}
	}

	if (IsMassGame() && GOLFDOC()->m_gameMode != 0x2000 && g_CurrentTime - m_observerSwitchTime > 3000) {
		unsigned long uid = 0xFFFFFFFF;
		for (int key = 1; key < 9; key++) {
			if (g_input->GetDown(MakeStr("PAD_%d", key), true) || g_input->GetDown(MakeStr("NUM_%d", key), true)) {
				unsigned char playerCount = 0;
				std::vector<sGameResultInfo>::iterator it;
				std::vector<sGameResultInfo>::iterator other;
				for (it = CSharedDoc::Instance()->m_gameResultList.begin(); it != CSharedDoc::Instance()->m_gameResultList.end(); ++it) {
					if (it->m_state != 0) {
						it->m_present = 0;
					} else {
						it->m_present = 1;
						playerCount++;
						for (other = CSharedDoc::Instance()->m_gameResultList.begin(); other != CSharedDoc::Instance()->m_gameResultList.end(); ++other) {
							if (other->m_state == 0 && it != other) {
								if (it->m_score > other->m_score
									|| (it->m_score == other->m_score && it->m_pang < other->m_pang)
									|| (it->m_score == other->m_score && it->m_pang == other->m_pang && it->m_value13F < other->m_value13F))
									it->m_present++;
							}
						}
					}
				}

				switch (key) {
				case 1:
					for (it = CSharedDoc::Instance()->m_gameResultList.begin(); it != CSharedDoc::Instance()->m_gameResultList.end(); ++it) {
						if (CSharedDoc::Instance()->m_userMap[it->m_uid].IsIdentity(0x10)) {
							uid = it->m_uid;
							break;
						}
					}
					break;
				case 2: {
					unsigned char pick = rand() % playerCount;

					for (it = CSharedDoc::Instance()->m_gameResultList.begin(); it != CSharedDoc::Instance()->m_gameResultList.end(); ++it, pick--) {
						if (it->m_uid != CSharedDoc::Instance()->m_userInfo.galleryGuid && it->m_joined != 1 && it->m_state == 0 && !CSharedDoc::Instance()->m_userMap[it->m_uid].IsIdentity(0x10) && it->m_titleTypeId != 0x30000001) {
							if (it->m_present != 1 && it->m_present != 2 && it->m_present != 3) {
								if (pick <= 0) {
									uid = it->m_uid;
									break;
								}
							}
						}
					}
					break;
				}
				case 3:
					if (playerCount >= 1) {
						for (it = CSharedDoc::Instance()->m_gameResultList.begin(); it != CSharedDoc::Instance()->m_gameResultList.end(); ++it) {
							if (it->m_present == 1) {
								uid = it->m_uid;
								break;
							}
						}
					}
					break;
				case 4:
					if (playerCount >= 2) {
						for (it = CSharedDoc::Instance()->m_gameResultList.begin(); it != CSharedDoc::Instance()->m_gameResultList.end(); ++it) {
							if (it->m_present == 2) {
								uid = it->m_uid;
								break;
							}
						}
					}
					break;
				case 5:
					if (playerCount >= 3) {
						for (it = CSharedDoc::Instance()->m_gameResultList.begin(); it != CSharedDoc::Instance()->m_gameResultList.end(); ++it) {
							if (it->m_present == 3) {
								uid = it->m_uid;
								break;
							}
						}
					}
					break;
				case 6:
					if (playerCount >= 1) {
						for (it = CSharedDoc::Instance()->m_gameResultList.begin(); it != CSharedDoc::Instance()->m_gameResultList.end(); ++it) {
							if (it->m_present == playerCount) {
								uid = it->m_uid;
								break;
							}
						}
					}
					break;
				case 7:
					if (playerCount >= 2) {
						for (it = CSharedDoc::Instance()->m_gameResultList.begin(); it != CSharedDoc::Instance()->m_gameResultList.end(); ++it) {
							if (it->m_present == playerCount - 1) {
								uid = it->m_uid;
								break;
							}
						}
					}
					break;
				case 8:
					if (IsLocalCountry(1)) {
						for (it = CSharedDoc::Instance()->m_gameResultList.begin(); it != CSharedDoc::Instance()->m_gameResultList.end(); ++it) {
							if (it->m_titleTypeId == 0x30000001) {
								uid = it->m_uid;
								break;
							}
						}
					}
					break;
				}

				sGameResultInfo info = CSharedDoc::Instance()->m_gameResultList[CSharedDoc::Instance()->GetIndex(uid)];
				if (uid != 0xFFFFFFFF && uid != CSharedDoc::Instance()->m_userInfo.galleryGuid && info.m_joined == 0 && info.m_state == 0) {
					m_observerSwitchTime = g_CurrentTime;

					WSendPacket packet((enumClientPacket)0x3F);
					packet.Encode4(uid);
					packet.Send(SEND_TO_DEFAULT);
				}
				break;
			}
		}
	}
}

// golfrule.cpp:17182
bool CGolfRule::IsLastShotRelplay(void) {
	return m_replayMode == 2 ? true : false;
}

// golfrule.cpp:17192
// Start of a hole: raises the combo gauge of my player (or of every player,
// or of every team in a team game) by one per combo gauge aux part the
// player's character wears, within the gauge limit.
void CGolfRule::HoleComboGaugeSetting(void) {
	// The aux parts that raise the combo gauge by one each.
	const unsigned long comboGaugeParts[2] = { 0x70010010, 0x70010012 };

	switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
	case 4:
	case 5:
	case 6:
	case 9:
	case 10: {
		sCharacterInfo *pCharacter = &WSingleton<CSharedDoc>::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_characterInfo;

		float bonus = 0.0f;
		for (int k = 0; k < 2; k++) {
			if (pCharacter->auxParts.tids[0] == comboGaugeParts[k])
				bonus += 1.0f;
		}

		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_comboGauge = Between(0.0f, (float)PLAYER(GOLFDOC()->m_myPlayerIndex)->m_comboGauge + bonus, WSingleton<CSharedDoc>::Instance()->GetComboGaugeLimit(GOLFDOC()->m_myPlayerIndex));
		break;
	}
	default:
		for (int i = 0; i < GOLFDOC()->m_playerNum; i++) {
			float bonus = 0.0f;
			for (int k = 0; k < 2; k++) {
				if (WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_characterInfo.auxParts.tids[0] == comboGaugeParts[k])
					bonus += 1.0f;
			}

			if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
				TEAM(PLAYER(i)->m_attribute)->m_teamRate = Between(0.0f, (float)TEAM(PLAYER(i)->m_attribute)->m_teamRate + bonus, WSingleton<CSharedDoc>::Instance()->GetComboGaugeLimit(PLAYER(i)->m_attribute));
			} else {
				PLAYER(i)->m_comboGauge = Between(0.0f, (float)PLAYER(i)->m_comboGauge + bonus, WSingleton<CSharedDoc>::Instance()->GetComboGaugeLimit(i));
			}
		}
		break;
	}
}

// golfrule.cpp:17267
void CGolfRule::CollectBallStopData(void) {
	int playType = CSharedDoc::Instance()->GetReplayPlayType();
	CSharedDoc::Instance()->SetReplayPlayType(2);
	if (ShotDataSave(ball_stop_replay_data_name, false) == 1) {
		NetResManager()->UploadBugReportData(ball_stop_replay_data_name, 1, (GolfBall().m_eventPreDataIndex[7] != -1) + 1, true);
	}
	CSharedDoc::Instance()->SetReplayPlayType(playType);
}

// golfrule.cpp:17286
// The hole-end packet's extra data: whether the local player is the viewed
// one, then the items picked up on the course (GroundItemMan hands its list,
// emptied once sent).
void CGolfRule::EncodePacket(enumClientPacket type, WSendPacket &packet) {
	if (type == 0x1C) {
		if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_uid == PLAYER(MyIndex())->m_uid)
			packet.Encode1(1);
		else
			packet.Encode1(0);

		std::vector<FieldItem::AcquiredItemInfo> *items = 0;
		SendMsg(this, "GroundItemMan", 0x277, (intptr_t)&items, 0, 0, 0);

		if (items) {
			packet.Encode1((unsigned char)items->size());

			std::vector<FieldItem::AcquiredItemInfo>::iterator it = items->begin();
			for (; it != items->end(); ++it) {
				it->EncodeTo(packet);
			}

			items->clear();
		} else {
			packet.Encode1(0);
		}
	}
}

// wutil.h:218: the original holds the out-of-line Min<float> in GolfRule's object.
template float Min<float>(float a, float b);

// Initialized from another constant (not a constant expression): the
// original reads the pointer from memory, a literal initializer folds it.
static const char *const BALL_STOP_REPLAY_FILE = "save/pangya_bug_report.sav";
const char *const ball_stop_replay_data_name = BALL_STOP_REPLAY_FILE;
