// Original: d:\build\custom\temp\code\client\projectg\tutorialmain.cpp
#include <atlbase.h>	// stand-in for the precompiled header: _AtlWinModule initializer
#include "TutorialMain.h"
#include "TutorialMain.inl"
#include "ObjectFactory.h"
#include "actor.h"
#include "fresh.h"
#include "frviewer.h"
#include "frbutton.h"
#include "exhibition.h"
#include "ChatMsg.h"
#include "QuestInfoDlg.h"
#include "frarea.h"
#include "client/Wangreal/include/wview.h"

#include "RankingDlg.h"
#include "user_info.h"
#include "InputManager.h"
#include "frlistbox.h"
#include "frscrollbar.h"
#include "frgraphicinterface.h"
#include "shared/Localize.h"
#include "SharedDoc.h"
#include "client/Wangreal/include/wpuppet.h"
#include "mousecursor.h"
#include "TaskManager.h"
#include "Packet.h"

#include <math.h>
#include <windows.h>
#include <mmsystem.h>

// A quest's completion bit (in CSharedDoc::m_tutorialData) and its reward.
struct sQuestReward {
	unsigned long m_mask;   // 0x00
	unsigned long m_typeId; // 0x04
	int m_count;            // 0x08
};
CHECK_SIZE(sQuestReward, 0x0C);

// No public symbol: paired by address (_data_9f7be8).
extern "C" {
static const sQuestReward data_9f7be8[20] = {
	{ 0x1, 0x1A00000F, 3 }, { 0x2, 0x18000007, 3 }, { 0x4, 0x18000005, 3 },
	{ 0x8, 0x18000008, 3 }, { 0x10, 0x1A000010, 500 }, { 0x20, 0x18000004, 3 },
	{ 0x40, 0x1A000010, 500 }, { 0x80, 0, 1 }, { 0x100, 0x1A00000F, 3 },
	{ 0x200, 0x18000028, 1 }, { 0x400, 0x18000006, 1 }, { 0x800, 0x18000007, 5 },
	{ 0x1000, 0x18000000, 4 }, { 0x2000, 0x18000001, 4 }, { 0x8000, 0x18000005, 3 },
	{ 0x10000, 0x18000005, 3 }, { 0x20000, 0x18000005, 3 }, { 0x40000, 0x18000005, 3 },
	{ 0x80000, 0x18000005, 3 }, { 0x80000, 0x18000005, 3 },
};
}

// StartSelectedTutorial's task change per quest state (m_questState):
// the task and its parameter, then the actor and the message's parameter
// posted once the task runs.
static char s_tutorialTasks[3][16] = { "CGolfTask", "CShopTask", "CRealMyRoomTask" };
static char s_tutorialTaskParams[3][16] = { "CGolfDoc", "", "" };
static char s_tutorialMsgTargets[3][16] = { "", "Shop", "RealMyRoom" };
static char s_tutorialMsgParams[3][16] = { "", "SHOPMAIN", "RealMyRoom" };

// The first caption goes through a const pointer: the table is then
// initialized by code, like the original's (docs/tough_problems/010).
static const char *const s_clubSelectName = "\xc5\xac\xb7\xb4\xbc\xb1\xc5\xc3"; // "클럽선택" (club select)
CTutorialMain::sTutorialMap CTutorialMain::m_Tutorials[14] = {
	{ s_clubSelectName, 0, 1, 0, 0, 0, 0, 0, true },
	{ "\xc5\xb8\xb1\xb8\xb9\xe6\xb9\xfd", 0, 2, 0, 1, 1, 0, 0, true }, // "타구방법" (how to hit)
	{ "\xc6\xce\xbe\xdf\xb0\xd4\xc0\xcc\xc1\xf6", 0, 3, 0, 2, 2, 0, 0, true }, // "팡야게이지" (Pangya gauge)
	{ "\xc6\xdb\xc6\xc3\xc0\xc7 \xb1\xe2\xba\xbb", 0, 4, 0, 2, 3, 0, 0, true }, // "퍼팅의 기본" (putting basics)
	{ "\xbe\xc6\xc0\xcc\xc5\xdb\xbb\xe7\xbf\xeb", 0, 5, 0, 3, 4, 0, 0, true }, // "아이템사용" (using items)
	{ "\xc1\xf6\xb8\xe9\xbc\xd3\xbc\xba", 0, 6, 0, 3, 5, 0, 0, true }, // "지면속성" (ground types)
	{ "\xbd\xba\xc4\xda\xbe\xee\xb0\xe8\xbb\xea", 0, 7, 0, 3, 6, 0, 0, true }, // "스코어계산" (score calculation)
	{ "\xc8\xa6\xc0\xce\xc7\xcf\xb1\xe2", 0, 8, 0, 4, 7, 0, 0, true }, // "홀인하기" (holing out)

	{ "\xb0\xf8\xbf\xc5\xb1\xe2\xb1\xe2", 1, 9, 0, 0, 8, 0, 0, true }, // "공옮기기" (moving the ball)
	{ "\xb0\xf8\xc0\xc7 \xb1\xe2\xbf\xef\xb1\xe2", 1, 10, 0, 1, 9, 0, 0, true }, // "공의 기울기" (ball slope)
	{ "\xb9\xd9\xb6\xf7 \xc0\xd0\xb1\xe2", 1, 11, 0, 1, 10, 0, 0, true }, // "바람 읽기" (reading the wind)
	{ "\xc5\xb8\xc1\xa1\xc0\xcc\xb5\xbf", 1, 12, 0, 2, 11, 0, 0, true }, // "타점이동" (moving the impact point)
	{ "\xc6\xc4\xbf\xf6 \xbd\xba\xc7\xc9", 1, 13, 0, 3, 12, 0, 0, true }, // "파워 스핀" (power spin)
	{ "\xc6\xc4\xbf\xf6 \xc4\xbf\xba\xea", 1, 14, 0, 3, 13, 0, 0, true }, // "파워 커브" (power curve)
};

const CTutorialMain::sChildTable CTutorialMain::m_ChildTable[14] = {
	{ 0, 1, 1 }, { 1, 2, 2 }, { 2, 4, 2 }, { 3, 5, 2 }, { 4, 7, 1 }, { 5, 7, 1 }, { 6, 7, 1 },
	{ 7, 7, 0 }, { 8, 9, 2 }, { 9, 11, 1 }, { 10, 11, 1 }, { 11, 12, 2 }, { 12, 12, 0 }, { 13, 13, 0 },
};

const CTutorialMain::sTutorialCondition CTutorialMain::m_TutoCondition[8] = {
	{ 0, 0, 1, 1 }, { 1, 0, 2, 2 }, { 2, 0, 4, 2 }, { 3, 0, 5, 2 },
	{ 4, 7, 4, 3 }, { 5, 7, 4, 3 }, { 6, 7, 4, 3 }, { 7, 0, 1, 1 },
};

CTutorialMain::sTutorialResult CTutorialMain::m_CompleteItems[3] = {
	{ 0, 3 }, { 1, 3 }, { 2, 3 },
};

CTutorialMain::sArrowInfo CTutorialMain::m_ArrowItems[10] = {
	{ 0, 0, 0 }, { 0, 1, 1 }, { 1, 1, 2 }, { 2, 1, 3 }, { 3, 1, 4 },
	{ 6, 1, 7 }, { 7, 1, 0 }, { 1, 1, 2 }, { 2, 1, 3 }, { 3, 1, 4 },
};

// ProcessAnimateIcon's phase, in degrees.
static float s_iconAngle;

// The images LoadTutorialMainImages registers, indexed by id.
struct sTutorialImage {
	int m_id;            // 0x00
	const char *m_name;  // 0x04
};

static const sTutorialImage s_tutorialImages[64] = {
	{ 0, "tu_btn_01_n" }, { 1, "tu_btn_02_n" }, { 2, "tu_btn_03_n" }, { 3, "tu_btn_04_n" },
	{ 4, "tu_btn_05_n" }, { 5, "tu_btn_06_n" }, { 6, "tu_btn_07_n" }, { 7, "tu_btn_08_n" },
	{ 8, "tu_btn_09_n" }, { 9, "tu_btn_10_n" }, { 10, "tu_btn_11_n" }, { 11, "tu_btn_12_n" },
	{ 12, "tu_btn_13_n" }, { 13, "tu_btn_14_n" },
	{ 14, "tu_btn_01_o" }, { 15, "tu_btn_02_o" }, { 16, "tu_btn_03_o" }, { 17, "tu_btn_04_o" },
	{ 18, "tu_btn_05_o" }, { 19, "tu_btn_06_o" }, { 20, "tu_btn_07_o" }, { 21, "tu_btn_08_o" },
	{ 22, "tu_btn_09_o" }, { 23, "tu_btn_10_o" }, { 24, "tu_btn_11_o" }, { 25, "tu_btn_12_o" },
	{ 26, "tu_btn_13_o" }, { 27, "tu_btn_14_o" },
	{ 28, "tu_btn_01_d" }, { 29, "tu_btn_02_d" }, { 30, "tu_btn_03_d" }, { 31, "tu_btn_04_d" },
	{ 32, "tu_btn_05_d" }, { 33, "tu_btn_06_d" }, { 34, "tu_btn_07_d" }, { 35, "tu_btn_08_d" },
	{ 36, "tu_btn_09_d" }, { 37, "tu_btn_10_d" }, { 38, "tu_btn_11_d" }, { 39, "tu_btn_12_d" },
	{ 40, "tu_btn_13_d" }, { 41, "tu_btn_14_d" },
	{ 42, "tu_btn_lock" }, { 43, "tu_empty_first" }, { 44, "tu_empty_first_n" },
	{ 45, "tu_off_frame01" }, { 46, "tu_off_frame02" }, { 47, "tu_off_frame03" },
	{ 48, "message_lock_n" }, { 49, "tu_icon_clear" }, { 50, "tu_icon_new" },
	{ 51, "tu_arrow_n" }, { 52, "tu_arrow_o" }, { 53, "tu_icon_lock_01" }, { 54, "tu_icon_lock_02" },
	{ 55, "tu_obtain_01" }, { 56, "tu_obtain_02_nuri" }, { 57, "tu_obtain_02_hana" },
	{ 58, "tu_class_01" }, { 59, "tu_class_02" }, { 60, "tu_class_03" },
	{ 61, "tu_tip_01" }, { 62, "tu_tip_02" }, { 63, "tu_tip_03" },
};

extern unsigned long g_CurrentTime;
extern WMatrix g_camera;

// tutorialmain.cpp:174 (run-time type and factory registration of CTutorialMain)
const WRTTI CTutorialMain::m_RTTI("CTutorialMain", &CTaskMain::m_RTTI);

IObject *CTutorialMainMakeInstance(void) {
	return new CTutorialMain;
}

struct __impCTutorialMain {
	__impCTutorialMain(void) {
		ObjectFactory().AddObjectFunctor(CTutorialMainMakeInstance, "CTutorialMain");
		ActorFactory().AddActorClass("CTutorialMain");
	}
};
static __impCTutorialMain s_impCTutorialMain;

// tutorialmain.cpp:176
const sFRESH_MSGMAP *CTutorialMain::GetMessageMap(void) const {
	return &_MsgMap;
}

sFRESH_MSGMAP CTutorialMain::_MsgMap = { &CTaskMain::_MsgMap, &CTutorialMain::_MsgEntries[0] };

sFRESH_ENTRY CTutorialMain::_MsgEntries[] = {
	// tutorialmain.cpp:179
	{ "TUTORIAL_MAIN", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnTutorialMainInit },
	// tutorialmain.cpp:180
	{ "TUTORIAL_MAIN", FRCMD_FINISH, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnTutorialMainFinish },
	// tutorialmain.cpp:182
	{ "TUTORIAL_MAIN.pet_pipin", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnTutorialAreaPipin },
	// tutorialmain.cpp:183
	{ "TUTORIAL_MAIN.pet_pipin", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnTutorialAreaPipinOwnerDraw },
	// tutorialmain.cpp:187
	{ "TUTORIAL_MAIN.tutorial_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnTutorialListInit },
	// tutorialmain.cpp:188
	{ "TUTORIAL_MAIN.tutorial_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnTutorialListOwnerDraw },
	// tutorialmain.cpp:189
	{ "TUTORIAL_MAIN.tutorial_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnTutorialListLBtnDown },
	// tutorialmain.cpp:190
	{ "TUTORIAL_MAIN.tutorial_list", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnTutorialListLBtnUp },
	// tutorialmain.cpp:193
	{ "TUTORIAL_MAIN.ubar.back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBar_BackInit },
	// tutorialmain.cpp:194
	{ "TUTORIAL_MAIN.ubar.back", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnUnderBar_BackUp },
	// tutorialmain.cpp:195
	{ "TUTORIAL_MAIN.ubar.back", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:196
	{ "TUTORIAL_MAIN.ubar.back", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:197
	{ "TUTORIAL_MAIN.ubar.front", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBar_FrontInit },
	// tutorialmain.cpp:198
	{ "TUTORIAL_MAIN.ubar.front", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnUnderBar_FrontUp },
	// tutorialmain.cpp:199
	{ "TUTORIAL_MAIN.ubar.front", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:200
	{ "TUTORIAL_MAIN.ubar.front", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:201
	{ "TUTORIAL_MAIN.ubar.shop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBar_ShopInit },
	// tutorialmain.cpp:202
	{ "TUTORIAL_MAIN.ubar.shop", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnUnderBar_ShopUp },
	// tutorialmain.cpp:203
	{ "TUTORIAL_MAIN.ubar.shop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:204
	{ "TUTORIAL_MAIN.ubar.shop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:205
	{ "TUTORIAL_MAIN.ubar.myroom", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBar_MyRoomInit },
	// tutorialmain.cpp:206
	{ "TUTORIAL_MAIN.ubar.myroom", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnUnderBar_MyRoomUp },
	// tutorialmain.cpp:207
	{ "TUTORIAL_MAIN.ubar.myroom", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:208
	{ "TUTORIAL_MAIN.ubar.myroom", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:209
	{ "TUTORIAL_MAIN.ubar.itemstorage", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBar_ItemStorageInit },
	// tutorialmain.cpp:210
	{ "TUTORIAL_MAIN.ubar.itemstorage", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnUnderBar_ItemStorageUp },
	// tutorialmain.cpp:211
	{ "TUTORIAL_MAIN.ubar.itemstorage", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:212
	{ "TUTORIAL_MAIN.ubar.itemstorage", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:213
	{ "TUTORIAL_MAIN.ubar.gift", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBar_GiftInit },
	// tutorialmain.cpp:214
	{ "TUTORIAL_MAIN.ubar.gift", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnUnderBar_GiftUp },
	// tutorialmain.cpp:215
	{ "TUTORIAL_MAIN.ubar.gift", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:216
	{ "TUTORIAL_MAIN.ubar.gift", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:217
	{ "TUTORIAL_MAIN.ubar.cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBar_CookieInit },
	// tutorialmain.cpp:218
	{ "TUTORIAL_MAIN.ubar.cookie", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnUnderBar_CookieUp },
	// tutorialmain.cpp:219
	{ "TUTORIAL_MAIN.ubar.cookie", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:220
	{ "TUTORIAL_MAIN.ubar.cookie", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:221
	{ "TUTORIAL_MAIN.ubar.game", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBar_GameInit },
	// tutorialmain.cpp:222
	{ "TUTORIAL_MAIN.ubar.game", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnUnderBar_GameUp },
	// tutorialmain.cpp:223
	{ "TUTORIAL_MAIN.ubar.game", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:224
	{ "TUTORIAL_MAIN.ubar.game", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:225
	{ "TUTORIAL_MAIN.ubar.server", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBar_ServerInit },
	// tutorialmain.cpp:226
	{ "TUTORIAL_MAIN.ubar.server", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnUnderBar_ServerUp },
	// tutorialmain.cpp:227
	{ "TUTORIAL_MAIN.ubar.server", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:228
	{ "TUTORIAL_MAIN.ubar.server", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:229
	{ "TUTORIAL_MAIN.ubar.exit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBar_ExitInit },
	// tutorialmain.cpp:230
	{ "TUTORIAL_MAIN.ubar.exit", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnUnderBar_ExitUp },
	// tutorialmain.cpp:231
	{ "TUTORIAL_MAIN.ubar.exit", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:232
	{ "TUTORIAL_MAIN.ubar.exit", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:233
	{ "TUTORIAL_MAIN.ubar.tabbtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBar_TabBtnInit },
	// tutorialmain.cpp:234
	{ "TUTORIAL_MAIN.ubar.tabbtn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnUnderBar_TabBtnUp },
	// tutorialmain.cpp:237
	{ "TUTORIAL_MAIN.obar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_Init },
	// tutorialmain.cpp:238
	{ "TUTORIAL_MAIN.utab", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderTab_Init },
	// tutorialmain.cpp:239
	{ "TUTORIAL_MAIN.ubar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBarInit },
	// tutorialmain.cpp:241
	{ "TUTORIAL_MAIN.utab.magicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBar_MagicBoxInit },
	// tutorialmain.cpp:242
	{ "TUTORIAL_MAIN.utab.magicbox", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnUnderBar_MagicBoxUp },
	// tutorialmain.cpp:243
	{ "TUTORIAL_MAIN.utab.magicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:244
	{ "TUTORIAL_MAIN.utab.magicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:246
	{ "TUTORIAL_MAIN.utab.tikimagicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBar_TikiMagicBoxInit },
	// tutorialmain.cpp:247
	{ "TUTORIAL_MAIN.utab.tikimagicbox", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnUnderBar_TikiMagicBoxUp },
	// tutorialmain.cpp:248
	{ "TUTORIAL_MAIN.utab.tikimagicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:249
	{ "TUTORIAL_MAIN.utab.tikimagicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:271
	{ "TUTORIAL_MAIN.utab.scratch", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBar_ScratchInit },
	// tutorialmain.cpp:272
	{ "TUTORIAL_MAIN.utab.scratch", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnUnderBar_ScratchUp },
	// tutorialmain.cpp:273
	{ "TUTORIAL_MAIN.utab.scratch", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:274
	{ "TUTORIAL_MAIN.utab.scratch", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:276
	{ "TUTORIAL_MAIN.utab.bongdarishop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBar_BongdariShopInit },
	// tutorialmain.cpp:277
	{ "TUTORIAL_MAIN.utab.bongdarishop", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnUnderBar_BongdariShopUp },
	// tutorialmain.cpp:278
	{ "TUTORIAL_MAIN.utab.bongdarishop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:279
	{ "TUTORIAL_MAIN.utab.bongdarishop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:282
	{ "TUTORIAL_MAIN.utab.maket", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBar_MaketInit },
	// tutorialmain.cpp:283
	{ "TUTORIAL_MAIN.utab.maket", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnUnderBar_MaketUp },
	// tutorialmain.cpp:284
	{ "TUTORIAL_MAIN.utab.maket", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:285
	{ "TUTORIAL_MAIN.utab.maket", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:295
	{ "TUTORIAL_MAIN.utab.ranking", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBar_RankingInit },
	// tutorialmain.cpp:296
	{ "TUTORIAL_MAIN.utab.ranking", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnUnderBar_RankingUp },
	// tutorialmain.cpp:297
	{ "TUTORIAL_MAIN.utab.ranking", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:298
	{ "TUTORIAL_MAIN.utab.ranking", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:299
	{ "TUTORIAL_MAIN.utab.guild", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBar_GuildInit },
	// tutorialmain.cpp:300
	{ "TUTORIAL_MAIN.utab.guild", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnUnderBar_GuildUp },
	// tutorialmain.cpp:301
	{ "TUTORIAL_MAIN.utab.guild", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:302
	{ "TUTORIAL_MAIN.utab.guild", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:303
	{ "TUTORIAL_MAIN.utab.notice", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnUnderBar_NoticeInit },
	// tutorialmain.cpp:304
	{ "TUTORIAL_MAIN.utab.notice", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnUnderBar_NoticeUp },
	// tutorialmain.cpp:305
	{ "TUTORIAL_MAIN.utab.notice", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:306
	{ "TUTORIAL_MAIN.utab.notice", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:310
	{ "TUTORIAL_MAIN.obar.title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_TitleInit },
	// tutorialmain.cpp:311
	{ "TUTORIAL_MAIN.obar.site", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_SiteInit },
	// tutorialmain.cpp:312
	{ "TUTORIAL_MAIN.obar.char", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_CharInit },
	// tutorialmain.cpp:313
	{ "TUTORIAL_MAIN.obar.char", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnOverBar_CharDown },
	// tutorialmain.cpp:314
	{ "TUTORIAL_MAIN.obar.char", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_CharOwnerDraw },
	// tutorialmain.cpp:315
	{ "TUTORIAL_MAIN.obar.caddie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_CaddieInit },
	// tutorialmain.cpp:316
	{ "TUTORIAL_MAIN.obar.caddie", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnOverBar_CaddieDown },
	// tutorialmain.cpp:317
	{ "TUTORIAL_MAIN.obar.caddie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_CaddieOwnerDraw },
	// tutorialmain.cpp:318
	{ "TUTORIAL_MAIN.obar.club", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_ClubInit },
	// tutorialmain.cpp:319
	{ "TUTORIAL_MAIN.obar.club", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnOverBar_ClubDown },
	// tutorialmain.cpp:320
	{ "TUTORIAL_MAIN.obar.club", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_ClubOwnerDraw },
	// tutorialmain.cpp:321
	{ "TUTORIAL_MAIN.obar.aztec", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_AztecInit },
	// tutorialmain.cpp:322
	{ "TUTORIAL_MAIN.obar.aztec", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnOverBar_AztecDown },
	// tutorialmain.cpp:323
	{ "TUTORIAL_MAIN.obar.aztec", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_AztecOwnerDraw },
	// tutorialmain.cpp:324
	{ "TUTORIAL_MAIN.obar.mascot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_MascotInit },
	// tutorialmain.cpp:325
	{ "TUTORIAL_MAIN.obar.mascot", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnOverBar_MascotDown },
	// tutorialmain.cpp:326
	{ "TUTORIAL_MAIN.obar.mascot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_MascotOwnerDraw },
	// tutorialmain.cpp:327
	{ "TUTORIAL_MAIN.obar.char_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_CharSelInit },
	// tutorialmain.cpp:328
	{ "TUTORIAL_MAIN.obar.char_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_CharSelOwnerDraw },
	// tutorialmain.cpp:329
	{ "TUTORIAL_MAIN.obar.caddie_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_CaddieSelInit },
	// tutorialmain.cpp:330
	{ "TUTORIAL_MAIN.obar.caddie_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_CaddieSelOwnerDraw },
	// tutorialmain.cpp:331
	{ "TUTORIAL_MAIN.obar.club_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_ClubSelInit },
	// tutorialmain.cpp:332
	{ "TUTORIAL_MAIN.obar.club_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_ClubSelOwnerDraw },
	// tutorialmain.cpp:333
	{ "TUTORIAL_MAIN.obar.aztec_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_AztecSelInit },
	// tutorialmain.cpp:334
	{ "TUTORIAL_MAIN.obar.aztec_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_AztecSelOwnerDraw },
	// tutorialmain.cpp:335
	{ "TUTORIAL_MAIN.obar.mascot_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_MascotSelInit },
	// tutorialmain.cpp:336
	{ "TUTORIAL_MAIN.obar.mascot_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_MascotSelOwnerDraw },
	// tutorialmain.cpp:338
	{ "TUTORIAL_MAIN.obar.pang_cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_PangCookieInit },
	// tutorialmain.cpp:339
	{ "TUTORIAL_MAIN.obar.pang_cookie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_PangCookieOwnerDraw },
	// tutorialmain.cpp:341
	{ "TUTORIAL_MAIN.obar.onelinemsg_req", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_OneLineReqBtnInit },
	// tutorialmain.cpp:342
	{ "TUTORIAL_MAIN.obar.onelinemsg_req", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnOverBar_OneLineReqBtnUp },
	// tutorialmain.cpp:343
	{ "TUTORIAL_MAIN.obar.onelineview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_OneLineViewInit },
	// tutorialmain.cpp:344
	{ "TUTORIAL_MAIN.obar.onelineview", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_OneLineViewOwnerDraw },
	// tutorialmain.cpp:345
	{ "TUTORIAL_MAIN.obar.spcard", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_SpecialCardInit },
	// tutorialmain.cpp:346
	{ "TUTORIAL_MAIN.obar.spcard", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnOverBar_SpecialCardUp },
	// tutorialmain.cpp:347
	{ "TUTORIAL_MAIN.obar.spcard", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:348
	{ "TUTORIAL_MAIN.obar.spcard", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:349
	{ "TUTORIAL_MAIN.obar.messenger", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_MessengerInit },
	// tutorialmain.cpp:350
	{ "TUTORIAL_MAIN.obar.messenger", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnOverBar_MessengerUp },
	// tutorialmain.cpp:351
	{ "TUTORIAL_MAIN.obar.messenger", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:352
	{ "TUTORIAL_MAIN.obar.messenger", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:353
	{ "TUTORIAL_MAIN.obar.myinfo", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_MyInfoInit },
	// tutorialmain.cpp:354
	{ "TUTORIAL_MAIN.obar.myinfo", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnOverBar_MyInfoUp },
	// tutorialmain.cpp:355
	{ "TUTORIAL_MAIN.obar.myinfo", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:356
	{ "TUTORIAL_MAIN.obar.myinfo", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:360
	{ "TUTORIAL_MAIN.obar.option", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnOverBar_OptionInit },
	// tutorialmain.cpp:363
	{ "TUTORIAL_MAIN.obar.option", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnOverBar_OptionUp },
	// tutorialmain.cpp:365
	{ "TUTORIAL_MAIN.obar.option", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOn },
	// tutorialmain.cpp:366
	{ "TUTORIAL_MAIN.obar.option", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarHoverOff },
	// tutorialmain.cpp:366 (same line as the previous row)
	{ "TUTORIAL_MAIN.obar.messenger_bubble", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnMessengerBubbleInit },
	// tutorialmain.cpp:366 (same line as the previous row)
	{ "TUTORIAL_MAIN.obar.messenger_bubble", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnMessengerBubbleOwnerDraw },
	// tutorialmain.cpp:366 (same line as the previous row)
	{ "TUTORIAL_MAIN.obar.messenger_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnMessengerBubbleEditInit },
	// tutorialmain.cpp:366 (same line as the previous row)
	{ "TUTORIAL_MAIN.obar.messenger_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnMessengerAlarmBtnInit },
	// tutorialmain.cpp:367
	{ "TUTORIAL_MAIN.obar.messenger_alarm", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnMessengerAlarmBtnUp },
	// tutorialmain.cpp:368
	{ "TUTORIAL_MAIN.obar.gift_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnMessengerGiftAlarmBtnInit },
	// tutorialmain.cpp:369
	{ "TUTORIAL_MAIN.obar.gift_alarm", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTutorialMain::OnMessengerGiftAlarmBtnUp },
	// tutorialmain.cpp:370
	{ "TUTORIAL_MAIN.obar.tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTutorialMain::OnBarTooltipOwnerDraw },
	// tutorialmain.cpp:372
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// tutorialmain.cpp:375
CTutorialMain::CTutorialMain(void) {
	m_questState = -1;
	m_questCategory = -1;
	m_questNextIndex = -1;
	m_questIndex = -1;
	m_tutorialListBox = 0;
	m_tutorialInfoDlg = 0;
	m_questGift = 0;
	m_listState = 0;
	m_pipinArea = 0;
	m_pet = 0;
	m_bongdariShop[2] = false;
	m_questReward = false;

	m_gradeGift = 0;

	m_camDirection = 1.0f;
	m_camPaused = false;
	m_camMoveTime = g_CurrentTime;

	m_overBarBtnPos[0] = WPoint(40.0f, 90.0f);
	m_overBarBtnPos[1] = WPoint(129.0f, 90.0f);
	m_overBarBtnPos[2] = WPoint(218.0f, 58.0f);
	m_overBarBtnPos[3] = WPoint(218.0f, 119.0f);
	m_overBarBtnPos[4] = WPoint(307.0f, 46.0f);
	m_overBarBtnPos[5] = WPoint(307.0f, 90.0f);
	m_overBarBtnPos[6] = WPoint(307.0f, 134.0f);
	m_overBarBtnPos[7] = WPoint(396.0f, 90.0f);

	m_underBarBtnPos[0] = WPoint(129.0f, 91.0f);
	m_underBarBtnPos[1] = WPoint(218.0f, 64.0f);
	m_underBarBtnPos[2] = WPoint(218.0f, 117.0f);
	m_underBarBtnPos[3] = WPoint(307.0f, 91.0f);
	m_underBarBtnPos[4] = WPoint(396.0f, 64.0f);
	m_underBarBtnPos[5] = WPoint(396.0f, 117.0f);

	m_columnX[0] = 121.0f;
	m_columnX[1] = 215.0f;
	m_columnX[2] = 303.0f;
	m_columnX[3] = 395.0f;
	m_columnX[4] = 486.0f;
	m_pendingTutorial = 0xFF;
	m_iconOffsetX = 0.0f;
	m_iconOffset = 0.0f;
	m_iconOffsetZ = 0.0f;

	m_categoryCount = IsLocalContent((localContentType_t)70) ? 2 : 1;
}

// tutorialmain.cpp:438
CTutorialMain::~CTutorialMain(void) {
	if (m_pet) { delete m_pet; m_pet = 0; }
}

// tutorialmain.cpp:443
void CTutorialMain::OnPreserveBack(const char *name) {
	HidePet();

	CTaskMain::OnPreserveBack(name);
}

// tutorialmain.cpp:450
void CTutorialMain::OnRestorePreserved(void) {
	CTaskMain::OnRestorePreserved();

	WSingleton<CChatMsg>::Instance()->SetActive(true, true, true);
}

// tutorialmain.cpp:457
void CTutorialMain::OnLoad(void) {
	CTaskMain::OnLoad();

	LoadTutorialMainImages();
}

// tutorialmain.cpp:464
void CTutorialMain::OnInit(void) {
	m_tutorialList.clear();

	for (int i = 0; i < 14; i++) {
		m_Tutorials[i].m_status = 0;
	}

	int parents = 0;
	for (int category = 0; category < 3; category++) {
		unsigned char end = 0;
		switch ((unsigned char)category) {
		case 0: end = 8; break;
		case 1: end = 14; break;
		case 2: end = 0; break;
		}
		unsigned char start = 0;
		switch ((unsigned char)category) {
		case 0: start = 0; break;
		case 1: start = 8; break;
		case 2: start = 0; break;
		}
		for (int i = start; i < end; i++) {
			int next = m_ChildTable[i].m_first;
			for (int j = 0; j < m_ChildTable[i].m_count; j++)
				m_questInfo[i].m_items.push_back(next++);

			unsigned char first = 0;
			switch ((unsigned char)category) {
			case 0: first = 0; break;
			case 1: first = 8; break;
			case 2: first = 0; break;
			}
			for (int k = first; k < end; k++) {
				if (i == k)
					continue;
				if (m_Tutorials[i].m_level - m_Tutorials[k].m_level < -1)
					break;

				if (m_Tutorials[i].m_level - 1 == m_Tutorials[k].m_level) {
					for (unsigned int j = 0; j < m_questInfo[k].m_items.size(); j++) {
						if (m_Tutorials[i].m_quest == m_questInfo[k].m_items[j])
							parents++;
					}
				}
			}

			m_questInfo[i].m_parentCount = parents;
			m_questInfo[i].m_parentsCleared = 0;
			parents = 0;
		}
	}

	CTaskMain::OnInit();
}

// tutorialmain.cpp:517
void CTutorialMain::LoadTutorialMainImages(void) {
	for (int i = 0; i < 64; i++)
		m_images[i] = g_pFresh->RegisterBitmap(s_tutorialImages[i].m_name);
}

// tutorialmain.cpp:525
void CTutorialMain::EnableAllBtn(void) {
	for (int i = 0; i < 14; i++)
		m_Tutorials[i].m_enabled = true;
}

// tutorialmain.cpp:536
void CTutorialMain::SetCompleteStates(void) {
	for (int category = 0; category < m_categoryCount; category++) {
		CSharedDoc *doc = WSingleton<CSharedDoc>::Instance();
		unsigned char end = 0;
		switch ((unsigned char)category) {
		case 0: end = 8; break;
		case 1: end = 14; break;
		case 2: end = 0; break;
		}
		unsigned char start = 0;
		switch ((unsigned char)category) {
		case 0: start = 0; break;
		case 1: start = 8; break;
		case 2: start = 0; break;
		}
		for (int i = start; i < end; i++) {
			m_Tutorials[i].m_status = 0;
			switch (category) {
			case 0:
				if (doc->m_tutorialData[0] & data_9f7be8[i].m_mask)
					m_Tutorials[i].m_status = 1;
				break;

			case 1:
				if (doc->m_tutorialData[0] == 0xFF) {
					if (doc->m_tutorialData[1] & data_9f7be8[i].m_mask)
						m_Tutorials[i].m_status = 1;
				} else {
					m_Tutorials[i].m_status = 2;
				}
				break;

			case 2:
				if (doc->m_tutorialData[0] == 0xFF && doc->m_tutorialData[1] == 0x3F00) {
					if (doc->m_tutorialData[2] & data_9f7be8[i].m_mask)
						m_Tutorials[i].m_status = 1;
				} else {
					m_Tutorials[i].m_status = 2;
				}
				break;
			}
		}
	}
}

// tutorialmain.cpp:590
void CTutorialMain::CheckNodeStates(void) {
	int sameLevel = 0;

	for (int category = 0; category < m_categoryCount; category++) {
		switch ((unsigned char)category) {
		case 0:
			break;
		case 1:
			if (WSingleton<CSharedDoc>::Instance()->m_tutorialData[0] != 0xFF)
				return;
			break;
		case 2:
			if (WSingleton<CSharedDoc>::Instance()->m_tutorialData[0] != 0xFF || WSingleton<CSharedDoc>::Instance()->m_tutorialData[1] != 0x3F00)
				return;
			break;
		default:
			return;
		}

		unsigned char end = 0;
		switch ((unsigned char)category) {
		case 0: end = 8; break;
		case 1: end = 14; break;
		case 2: end = 0; break;
		}

		unsigned char start = 0;
		switch ((unsigned char)category) {
		case 0: start = 0; break;
		case 1: start = 8; break;
		case 2: start = 0; break;
		}
		for (int i = start; i < end; i++)
			m_questInfo[i].m_parentsCleared = 0;

		start = 0;
		switch ((unsigned char)category) {
		case 0: start = 0; break;
		case 1: start = 8; break;
		case 2: start = 0; break;
		}
		for (int i = start; i < end; i++) {
			for (unsigned int j = 0; j < m_questInfo[i].m_items.size(); j++) {
				int child = m_questInfo[i].m_items[j];
				if (m_Tutorials[i].m_status != 1) {
					if (m_Tutorials[child].m_status == 0)
						m_Tutorials[child].m_status = 2;
				}
				else {
					if (m_questInfo[child].m_parentCount > m_questInfo[child].m_parentsCleared)
						m_questInfo[child].m_parentsCleared++;
				}

				if (m_questInfo[child].m_parentCount == m_questInfo[child].m_parentsCleared) {
					if (m_Tutorials[child].m_status != 1)
						m_Tutorials[child].m_status = 0;
				}

				if (m_questInfo[child].m_parentCount >= 2 && m_Tutorials[child].m_status != 1) {
					unsigned char first = 0;
					switch ((unsigned char)category) {
					case 0: first = 0; break;
					case 1: first = 8; break;
					case 2: first = 0; break;
					}
					for (int k = first; k < end; k++) {
						if (m_Tutorials[child].m_level == m_Tutorials[k].m_level)
							sameLevel++;
					}

					if (sameLevel > 2 && m_questInfo[child].m_parentsCleared >= 1)
						m_Tutorials[child].m_status = 0;

					sameLevel = 0;
				}
			}
		}

		for (int c = 0; c < 3; c++)
			m_CompleteItems[c].m_count = IsCompleteCategory(m_CompleteItems[c].m_category) == true ? 4 : 3;
	}
}

// tutorialmain.cpp:656
void CTutorialMain::ShowPet(void) {
	if (m_pet) { delete m_pet; m_pet = 0; }

	bool loaded;
	CExhibition* pet = new CExhibition(85.0f, 245.0f, 0.35f, false);
	if (pet) {
		pet->SetArea(150.0f, 340.0f);
		pet->SetDistanceRange(9.0f, 13.0f);
		pet->SetDistance(11.0f, true, false);
		loaded = pet->SetModel("pipin", EXHIBIT_CADDIE);
		pet->SetControlFlag(0);
		pet->SetDirection(-0.45f);
		m_pet = pet;
		m_pipinTime.m_nextMotion = "\xB1\xDE\xC1\xA4\xB0\xC5";

		m_pipinTime.Reset();
		m_pipinTime.SetTime();
	}

	if (!loaded) {
		if (pet)
			delete pet;
	}
}

// tutorialmain.cpp:689
void CTutorialMain::HidePet(void) {
	if (m_pet) { delete m_pet; m_pet = 0; }
}

// tutorialmain.cpp:698
void CTutorialMain::ProcessLobbyBgCamera(float dt) {
	if (WSingleton<CSharedDoc>::Instance()->m_3DBackgroundOff)
		return;

	if (m_camPaused) {
		if (g_CurrentTime - m_camPauseTime > 2000) {
			m_camDirection *= -1.0f;
			m_camPaused = false;
			m_camMoveTime = g_CurrentTime;
		}
		return;
	}

	unsigned long elapsed = g_CurrentTime - m_camMoveTime;
	if (elapsed > 80000) {
		m_camPaused = true;
		m_camPauseTime = g_CurrentTime;
		return;
	}

	float speed = 1.0f;
	if (elapsed < 2000)
		speed = elapsed * 0.0005f;
	else if (elapsed > 78000)
		speed = 1.0f - (elapsed - 78000) * 0.0005f;

	g_camera.pivot += g_camera.xa * m_camDirection * dt * 0.3f * speed;
}

// tutorialmain.cpp:737
void CTutorialMain::OnProcess(float dt) {
	static float s_motionTime;
	static int s_idleElapsed;
	static int s_motionElapsed;

	if (m_pet) {
		m_pet->Process(dt, false);

		w_motion_data *motion = m_pet->GetMotionData();
		s_motionTime += dt;
		if (s_motionTime > motion->e - motion->s) {
			if (m_pipinTime.m_nextMotion.size() > 2 && strcmp(m_pipinTime.m_nextMotion.c_str(), motion->name) != 0) {
				m_pet->SetMotion(m_pipinTime.m_nextMotion.c_str(), false, 0.0f);
				s_motionTime = 0.0f;
				m_pipinTime.m_nextMotion = "";
			}
		}

		s_idleElapsed = timeGetTime() - m_pipinTime.m_lastIdleTime;
		if (s_idleElapsed > 16000) {
			DecideNextMotion(3);
			m_pipinTime.m_lastIdleTime = timeGetTime();
			s_idleElapsed = 0;
		} else {
			s_motionElapsed = timeGetTime() - m_pipinTime.m_lastMotionTime;
			if (s_motionElapsed > 5000) {
				DecideNextMotion(rand() % 3);
				m_pipinTime.m_lastMotionTime = timeGetTime();
			}
		}
	}

	m_iconOffset = cos(s_iconAngle * (3.14159265f / 180.0f)) * 5.0f;
	s_iconAngle += 1.0f;
	if (s_iconAngle > 360.0f)
		s_iconAngle = 0.0f;

	CTaskMain::OnProcess(dt);
}

// tutorialmain.cpp:784
void CTutorialMain::ProcessAnimateIcon(void) {
	m_iconOffset = cos(s_iconAngle * (3.14159265f / 180.0f)) * 5.0f;
	s_iconAngle += 1.0f;
	if (s_iconAngle > 360.0f)
		s_iconAngle = 0.0f;
}

// tutorialmain.cpp:798
void CTutorialMain::OnDestroy(void) {
	m_pipinTime.Reset();
	m_tutorialList.clear();
	CTaskMain::OnDestroy();
}

// tutorialmain.cpp:805
void CTutorialMain::HandleMsg(const MsgObject &msg) {
	switch (msg.m_msg) {
	case 0: {
		if (g_pFresh == 0)
			return;

		const char *layout = (const char *)msg.m_param1;
		g_pFresh->OpenLayout(layout, this, true);

		ShowPet();

		SetCompleteStates();
		CheckNodeStates();
		DrawCourseArrow();
		break;
	}

	case 0x151: {
		unsigned char openGift = (unsigned char)msg.m_param2;

		SetCompleteStates();
		CheckNodeStates();
		for (int i = 0; i < 14; i++)
			m_Tutorials[i].m_enabled = true;

		DrawCourseArrow();

		if (openGift)
			OpenQuestGift();
		break;
	}

	case 0x14E: {
		// The actor class and name the tutorial task registers.
		static char s_actorClass[] = "CTutorial";
		static char s_actorName[] = "Tutorial";
		*(char **)msg.m_param1 = s_actorClass;
		*(char **)msg.m_param2 = s_actorName;
		break;
	}

	case 0x14F:
		*(int *)msg.m_param1 = m_questState;
		*(int *)msg.m_param2 = m_questCategory;
		*(int *)msg.m_param3 = m_questNextIndex;
		*(int *)msg.m_param4 = m_questIndex;
		*(int *)msg.m_param5 = m_questValue;
		break;

	case 0x150: {
		int index = msg.m_param1;
		unsigned long result = msg.m_param2;

		m_listState = result;
		m_Tutorials[index].m_enabled = false;

		if (msg.m_param3 == 1 && m_questReward) {
			m_questReward = false;
			WSendPacket packet((enumClientPacket)0xA6);
			packet.Encode1((unsigned char)msg.m_param4);
			packet.Encode1(m_Tutorials[index].m_category);
			packet.Encode4(result);
			packet.Send(SEND_TO_DEFAULT);
		}
		else {
			m_Tutorials[index].m_enabled = true;
		}
		break;
	}

	case 0x153:
		m_gradeGift = (unsigned char)msg.m_param1;
		break;

	case 0x18:
		if (msg.m_param1 == 0)
			return;

		m_questState = 0;
		m_questCategory = 0;
		m_questNextIndex = 1;
		m_questIndex = 0;
		StartSelectedTutorial();
		break;

	default:
		HandleMsgCommon(msg);
		break;
	}
}

// tutorialmain.cpp:914
void CTutorialMain::DisableOverBarButtons(void) {
	if (m_charViewer) {
		m_charViewer->Enable(false);
	}

	if (m_caddieViewer) {
		m_caddieViewer->Enable(false);
	}

	if (m_clubViewer) {
		m_clubViewer->Enable(false);
	}

	if (m_aztecViewer) {
		m_aztecViewer->Enable(false);
	}

	if (m_mascotViewer) {
		m_mascotViewer->Enable(false);
	}

	if (m_oneLineReqBtn) {
		m_oneLineReqBtn->Enable(false);
	}
}

// tutorialmain.cpp:953
void CTutorialMain::DisableUnderBarButtons(void) {
	if (m_barButton[15]) {
		m_barButton[15]->SetPushSound(0);
		m_barButton[15]->EnableHover(false, 0.0f);
		m_barButton[15]->Enable(false);
	}

	if (m_barButton[8]) {
		m_barButton[8]->SetPushSound(0);
		m_barButton[8]->EnableHover(false, 0.0f);
		m_barButton[8]->Enable(false);
	}

	if (m_barButton[14]) {
		m_barButton[14]->EnableHover(false, 0.0f);
		m_barButton[14]->Enable(false);
	}

	if (m_barButton[20]) {
		m_barButton[20]->EnableHover(false, 0.0f);
		m_barButton[20]->Enable(false);
	}
}

// tutorialmain.cpp:1007
void CTutorialMain::OnTutorialMainInit(int) {
	SetTimeVariableBGM();
	TurnOff3DBackground();
}

// tutorialmain.cpp:1013
void CTutorialMain::OnTutorialMainFinish(void) {
	DisableOverBarButtons();
	DisableUnderBarButtons();

	if (WSingleton<CSharedDoc>::Instance()->m_tutorialFinish) {
		this << MsgObject(0, 0xAA, 1, 0, 0, 0, 0);
	}
}

// tutorialmain.cpp:1024
void CTutorialMain::OnTutorialAreaPipin(int param) {
	m_pipinArea = DYNAMIC_CAST(FrArea, param);
}

// tutorialmain.cpp:1029
void CTutorialMain::OnTutorialAreaPipinOwnerDraw(int) {
	if (!m_bongdariShop[2] && !WSingleton<CRankingInfo>::Instance()->GetDlg() && !USERINFODLG()) {
		if (m_pet)
			m_pet->Display(0.0f, 0.0f);
	}
}

// tutorialmain.cpp:1040
void CTutorialMain::OnTutorialListInit(int param) {
	m_tutorialListBox = DYNAMIC_CAST(FrListBox, param);
	if (m_tutorialListBox == 0)
		return;

	m_tutorialListBox->UseRightButton(true);
	m_tutorialListBox->UseDummy(true);
	m_tutorialListBox->SetPushSound(0);

	m_tutorialListBox->ClearItem();
	for (int i = 0; i < 3; i++) {
		m_tutorialListBox->AddItem(&m_CompleteItems[i]);
	}

	memset(m_categoryClear, 0, sizeof(m_categoryClear));
}

// tutorialmain.cpp:1062
void CTutorialMain::OnTutorialListOwnerDraw(int param) {
	FrListItem *item = (FrListItem *)param;
	if (item == 0)
		return;

	float x = item->pos.x;
	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();

	float rowY[3] = { 0.0f };
	int topRow = m_tutorialListBox->GetScrollBar()->GetCurTopRow_Int();
	if (topRow == 0) {
		rowY[0] = 119.0f;
		rowY[1] = 314.0f;
	}
	else if (topRow == 1) {
		rowY[1] = 119.0f;
		rowY[2] = 314.0f;
	}

	switch (item->idx) {
	case 0: {
		pGDI->DrawTexture(m_images[45], WRect(185.0f, rowY[0], 30.0f, 196.0f), 0xFFFFFFFF, 0);
		pGDI->DrawTexture(m_images[46], WRect(215.0f, rowY[0], 526.0f, 196.0f), 0xFFFFFFFF, 0);
		pGDI->DrawTexture(m_images[47], WRect(741.0f, rowY[0], 30.0f, 196.0f), 0xFFFFFFFF, 0);

		pGDI->DrawTexture(m_images[58], WRect(200.0f, rowY[0] + 6.0f, 153.0f, 51.0f), 0xFFFFFFFF, 0);

		pGDI->DrawTexture(m_images[61], WRect(550.0f, rowY[0] + 21.0f, 194.0f, 19.0f), 0xFFFFFFFF, 0);

		float mx = g_input->GetMousePoint().x - x;
		float my = g_input->GetMousePoint().y - rowY[0];

		int hit = IsInArea(item->idx, WPoint(mx, my));
		for (int i = 0; i < 8; i++) {
			if (m_Tutorials[i].m_pending == 0) {
				const Bitmap *img;
				if (m_Tutorials[i].m_status == 2) {
					img = m_images[42];
				}
				else
					img = m_images[i];

				if (hit != -1 && hit == i && m_Tutorials[i].m_status != 2) {
					pGDI->DrawTexture(m_images[hit + 14], WRect(x + m_overBarBtnPos[hit].x, rowY[0] + m_overBarBtnPos[hit].y, 86.0f, 48.0f), 0xFFFFFFFF, 0);
				}
				else {
					pGDI->DrawTexture(img, WRect(x + m_overBarBtnPos[i].x, rowY[0] + m_overBarBtnPos[i].y, 85.0f, 48.0f), 0xFFFFFFFF, 0);
				}
			}
			else if (m_pendingTutorial != 0xFF) {
				pGDI->DrawTexture(m_images[i + 28], WRect(x + m_overBarBtnPos[i].x, rowY[0] + m_overBarBtnPos[i].y, 85.0f, 48.0f), 0xFFFFFFFF, 0);
			}

			if (m_Tutorials[i].m_status == 1) {
				pGDI->DrawTexture(m_images[49], WRect(x + m_overBarBtnPos[i].x + 60.0f, rowY[0] + m_overBarBtnPos[i].y + 22.0f, 19.0f, 19.0f), 0xFFFFFFFF, 0);
			}
		}

		if (m_CompleteItems[0].m_count == 3) {
			pGDI->DrawTexture(m_images[53], WRect(668.0f, rowY[0] + 60.0f, 79.0f, 97.0f), 0xFFFFFFFF, 0);
			pGDI->DrawTexture(m_images[54], WRect(668.0f, rowY[0] + m_iconOffset + 58.0f, 79.0f, 97.0f), 0xFFFFFFFF, 0);
		}
		else
			pGDI->DrawTexture(m_images[55], WRect(668.0f, rowY[0] + 73.0f, 85.0f, 85.0f), 0xFFFFFFFF, 0);

		float offsetY = 0.0f;
		float arrowX = 121.0f;
		float arrowY = rowY[0] + 107.0f;
		for (int i = 0; i < 6; i++) {
			if (m_ArrowItems[i].m_show == 1) {
				pGDI->DrawTexture(m_images[51], WRect(arrowX + x, arrowY + offsetY, 10.0f, 10.0f), 0xFFFFFFFF, 0);
			}
			else {
				pGDI->DrawTexture(m_images[52], WRect(arrowX + x, arrowY + offsetY, 10.0f, 10.0f), 0xFFFFFFFF, 0);
			}

			if (i == 1) {
				offsetY = -22.0f;
				arrowX += 89.0f;
			}
			else if (i == 2) {
				offsetY = 22.0f;
			}
			else {
				offsetY = 0.0f;
				arrowX += 89.0f;
			}
		}
		break;
	}
	case 1: {
		pGDI->DrawTexture(m_images[45], WRect(185.0f, rowY[1], 30.0f, 196.0f), 0xFFFFFFFF, 0);
		pGDI->DrawTexture(m_images[46], WRect(215.0f, rowY[1], 526.0f, 196.0f), 0xFFFFFFFF, 0);
		pGDI->DrawTexture(m_images[47], WRect(741.0f, rowY[1], 30.0f, 196.0f), 0xFFFFFFFF, 0);

		pGDI->DrawTexture(m_images[59], WRect(200.0f, rowY[1] + 6.0f, 153.0f, 51.0f), 0xFFFFFFFF, 0);

		if (!IsLocalContent((localContentType_t)0x46)) {

			pGDI->DrawTexture(m_images[63], WRect(550.0f, rowY[1] + 21.0f, 194.0f, 19.0f), 0xFFFFFFFF, 0);

			pGDI->DrawTexture(m_images[48], WRect(x + 234.0f, rowY[1] + 67.0f, 123.0f, 80.0f), 0xFFFFFFFF, 0);
			return;
		}

		pGDI->DrawTexture(m_images[62], WRect(550.0f, rowY[1] + 21.0f, 194.0f, 19.0f), 0xFFFFFFFF, 0);

		const Bitmap *course;
		if (WSingleton<CSharedDoc>::Instance()->m_tutorialData[0] == 0xFF)
			course = m_images[44];
		else
			course = m_images[43];
		pGDI->DrawTexture(course, WRect(x + 40.0f, rowY[1] + 91.0f, 85.0f, 48.0f), 0xFFFFFFFF, 0);

		float mx = g_input->GetMousePoint().x - x;
		float my = g_input->GetMousePoint().y - rowY[1];

		int hit = IsInArea(item->idx, WPoint(mx, my));
		for (int i = 0; i < 6; i++) {
			if (m_Tutorials[i + 8].m_pending == 0) {
				const Bitmap *img;
				if (m_Tutorials[i + 8].m_status == 2) {
					img = m_images[42];
				}
				else {
					img = m_images[i + 8];
				}

				if (hit != -1 && hit == i && m_Tutorials[i + 8].m_status != 2) {
					pGDI->DrawTexture(m_images[i + 22], WRect(x + m_underBarBtnPos[i].x, rowY[1] + m_underBarBtnPos[i].y, 85.0f, 48.0f), 0xFFFFFFFF, 0);
				}
				else {
					pGDI->DrawTexture(img, WRect(x + m_underBarBtnPos[i].x, rowY[1] + m_underBarBtnPos[i].y, 85.0f, 48.0f), 0xFFFFFFFF, 0);
				}
			}
			else if (m_pendingTutorial != 0xFF) {
				pGDI->DrawTexture(m_images[i + 36], WRect(x + m_underBarBtnPos[i].x, rowY[1] + m_underBarBtnPos[i].y, 85.0f, 48.0f), 0xFFFFFFFF, 0);
			}

			if (m_Tutorials[i + 8].m_status == 1) {
				pGDI->DrawTexture(m_images[49], WRect(x + m_underBarBtnPos[i].x + 60.0f, rowY[1] + m_underBarBtnPos[i].y + 22.0f, 19.0f, 19.0f), 0xFFFFFFFF, 0);
			}
		}

		if (m_CompleteItems[1].m_count == 3) {
			pGDI->DrawTexture(m_images[53], WRect(668.0f, rowY[1] + 60.0f, 79.0f, 97.0f), 0xFFFFFFFF, 0);
			pGDI->DrawTexture(m_images[54], WRect(668.0f, rowY[1] + m_iconOffset + 58.0f, 79.0f, 97.0f), 0xFFFFFFFF, 0);
		}
		else {
			// Bit 1 of the user's event flags (the recycle event).
			int mode = ((WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().eventFlags >> 1) & 1) % 2;

			if (mode == 0) {
				pGDI->DrawTexture(m_images[56], WRect(668.0f, rowY[1] + 73.0f, 85.0f, 85.0f), 0xFFFFFFFF, 0);
			}
			else
				pGDI->DrawTexture(m_images[57], WRect(668.0f, rowY[1] + 73.0f, 85.0f, 85.0f), 0xFFFFFFFF, 0);
		}

		float arrowX = 211.0f;
		float arrowY = rowY[1] + 108.0f;
		for (int i = 6; i < 10; i++) {
			if (m_ArrowItems[i].m_show == 1) {
				pGDI->DrawTexture(m_images[51], WRect(arrowX + x, arrowY, 10.0f, 10.0f), 0xFFFFFFFF, 0);
			}
			else {
				pGDI->DrawTexture(m_images[52], WRect(arrowX + x, arrowY, 10.0f, 10.0f), 0xFFFFFFFF, 0);
			}

			arrowX += 89.0f;
		}
		break;
	}
	case 2:
		pGDI->DrawTexture(m_images[45], WRect(185.0f, rowY[2], 30.0f, 196.0f), 0xFFFFFFFF, 0);
		pGDI->DrawTexture(m_images[46], WRect(215.0f, rowY[2], 526.0f, 196.0f), 0xFFFFFFFF, 0);
		pGDI->DrawTexture(m_images[47], WRect(741.0f, rowY[2], 30.0f, 196.0f), 0xFFFFFFFF, 0);

		pGDI->DrawTexture(m_images[60], WRect(200.0f, rowY[2] + 6.0f, 153.0f, 51.0f), 0xFFFFFFFF, 0);

		pGDI->DrawTexture(m_images[63], WRect(550.0f, rowY[2] + 21.0f, 194.0f, 19.0f), 0xFFFFFFFF, 0);

		pGDI->DrawTexture(m_images[48], WRect(x + 234.0f, rowY[2] + 67.0f, 123.0f, 80.0f), 0xFFFFFFFF, 0);
		break;
	}

	pGDI->DrawTexture(m_images[50], WRect(669.0f, 63.0f, 18.0f, 19.0f), 0xFFFFFFFF, 0);
}

// tutorialmain.cpp:1309
void CTutorialMain::DrawCourseArrow(void) {
	for (int i = 0; i < 10; i++) {
		for (int j = m_ArrowItems[i].m_first; j <= m_ArrowItems[i].m_last; j++) {
			int first = GetGradeBegin_Grade(m_Tutorials[j].m_category);
			if (m_Tutorials[j].m_status == 1 || m_Tutorials[j].m_status == 0) {
				m_ArrowItems[first + i].m_show = 0;
			}
			else if (m_Tutorials[j].m_status == 2) {
				m_ArrowItems[first + i].m_show = 1;
			}
		}
	}
}

// tutorialmain.cpp:1332
int CTutorialMain::GetSelectedBtn(int type, WPoint pt) {
	return IsInArea(type, pt);
}

// tutorialmain.cpp:1339
int CTutorialMain::IsInArea(int type, WPoint pt) {
	WPoint *pos;
	unsigned char count = 0;

	switch (type) {
	case 0:
		pos = m_overBarBtnPos;
		count = 8;
		break;
	case 1:
		pos = m_underBarBtnPos;
		count = 6;
		break;
	}

	for (unsigned char i = 0; i < count; i++) {
		if (pt.x > pos[i].x && pos[i].x + 85.0f > pt.x && pt.y > pos[i].y && pos[i].y + 48.0f > pt.y)
			return i;
	}

	return -1;
}

// tutorialmain.cpp:1368
void CTutorialMain::OnTutorialListLBtnDown(void) {
	if (m_tutorialListBox == 0)
		return;

	FrListItem *item = m_tutorialListBox->GetItemUnderCursor();
	if (item == 0)
		return;

	if (!IsLocalContent((localContentType_t)0x46)) {
		if (item->idx >= 1)
			return;
	}

	float rowY[3] = { 0.0f };
	int topRow = m_tutorialListBox->GetScrollBar()->GetCurTopRow_Int();
	if (topRow == 0) {
		rowY[0] = 119.0f;
		rowY[1] = 314.0f;
	}
	else if (topRow == 1) {
		rowY[1] = 119.0f;
		rowY[2] = 314.0f;
	}

	float x = g_input->GetMousePoint().x - item->pos.x;
	float y = g_input->GetMousePoint().y - rowY[item->idx];

	int btn = IsInArea(item->idx, WPoint(x, y));
	if (btn == -1)
		return;

	switch (item->idx) {
	case 0:
		break;
	case 1:
		btn += 8;
		break;
	}

	if (m_Tutorials[btn].m_status != 2) {
		m_Tutorials[btn].m_pending = 1;
		m_pendingTutorial = btn;
	}

	OpenQuest(m_Tutorials[btn].m_category, btn);
}

// tutorialmain.cpp:1425
void CTutorialMain::OnTutorialListLBtnUp(void) {
	if (m_tutorialListBox == 0)
		return;

	FrListItem *item = m_tutorialListBox->GetItemUnderCursor();
	if (item == 0)
		return;

	int end = GetAccumulateLimit(item->idx);
	for (int i = GetGradeBegin_Grade(item->idx); i < end; i++) {
		if (m_Tutorials[i].m_pending == 1) {
			m_Tutorials[i].m_pending = 0;
			break;
		}
	}
}

// tutorialmain.cpp:1446
void CTutorialMain::SetQuestInformation(sTutorialMap *map) {
	m_questState = map->m_state;
	m_questCategory = map->m_category;
	m_questNextIndex = map->m_index;
	m_questIndex = map->m_quest;
}

// tutorialmain.cpp:1457
void CTutorialMain::SetQuestInformation(int category, int index) {
	m_questCategory = category;
	m_questState = 0;
	m_questNextIndex = index + 1;
	m_questIndex = index;
}

// tutorialmain.cpp:1467
void CTutorialMain::OpenQuest(int category, int index) {
	if (!m_Tutorials[index].m_enabled)
		return;

	if (m_Tutorials[index].m_status == 2) {
		m_questionForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "You can do it after finishing the previous grade."
		m_questionForm->SetMessage("\xC0\xCC\xC0\xFC \xB5\xEE\xB1\xDE\xC0\xBB \xB8\xB6\xC4\xA3 \xC8\xC4 \xC7\xCF\xBD\xC7 \xBC\xF6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.", false);
		m_questionForm->Open((FRESH_RESULT_CALLBACK)0,
			WPoint((g_view->GetWidth() - m_questionForm->GetRect().w) * 0.5f, 80.0f), 3);
		m_questionForm->EnableDrag(false);

		m_Tutorials[index].m_pending = 0;
		m_pendingTutorial = 0xFF;
		return;
	}

	SetQuestInformation(category, index);
	if (IsCompleteQuest(category, index)) {
		m_questReward = false;

		m_questionForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);
		// "This quest is already complete, so the clear gift cannot be received.\nPlay it again anyway?"
		m_questionForm->SetMessage("\xC0\xCC\xB9\xCC \xBF\xCF\xB7\xE1\xB5\xC8 \xC4\xF9\xBD\xBA\xC6\xAE\xC0\xCC\xB9\xC7\xB7\xCE, \xC5\xAC\xB8\xAE\xBE\xEE \xB1\xE2\xB3\xE4 \xBC\xB1\xB9\xB0\xC0\xBB \xB9\xDE\xC0\xBB \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9. \n\xB1\xD7\xB7\xA1\xB5\xB5 \xB4\xD9\xBD\xC3 \xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);
		m_questionForm->EnableDrag(false);
		m_questionForm->Open((FRESH_RESULT_CALLBACK)&CTutorialMain::OnQuestionFormResult,
			WPoint((g_view->GetWidth() - m_questionForm->GetRect().w) * 0.5f, 80.0f), 3);
		m_questionForm->MoveCursor("ok");
	}
	else {
		m_questReward = true;
		OpenTutorialInfoDlg();
	}
}

// tutorialmain.cpp:1509
void CTutorialMain::OpenQuestGift(void) {
	m_questGift = CreateForm<FrQuestGift>(g_pFresh->GetManager(), this, "quest_gift", 0);
	m_questGift->SetQuestExplane(m_questIndex);
	m_questGift->Open((FRESH_RESULT_CALLBACK)&CTutorialMain::OnGiftDlgResult,
		WPoint((g_view->GetWidth() - m_questGift->GetRect().w) * 0.5f, 80.0f), 2);
}

// tutorialmain.cpp:1517
void CTutorialMain::OpenGradeGift(unsigned char grade) {
	m_questGift = CreateForm<FrQuestGift>(g_pFresh->GetManager(), this, "quest_gift", 0);
	m_questGift->SetData(grade);
	m_questGift->Open((FRESH_RESULT_CALLBACK)0,
		WPoint((g_view->GetWidth() - m_questGift->GetRect().w + 10.0f) * 0.5f, 80.0f), 2);

	m_gradeGift = 0;
}

// tutorialmain.cpp:1526
void CTutorialMain::DecideNextMotion(unsigned char grade) {
	switch (grade) {
	case 0:
		m_pipinTime.m_nextMotion = "\xB1\xE2\xBA\xBB";
		break;
	case 1:
		m_pipinTime.m_nextMotion = "\xB1\xE2\xB4\xD9\xB8\xAE\xB1\xE2";
		break;
	case 2:
		m_pipinTime.m_nextMotion = "\xC2\xF9\xBD\xBA";
		break;
	case 3:
		m_pipinTime.m_nextMotion = "\xC0\xE7\xC3\xCB\xC7\xCF\xB1\xE2";
		break;
	case 4:
		m_pipinTime.m_nextMotion = "\xB1\xDE\xC1\xA4\xB0\xC5";
		break;
	case 5:
		m_pipinTime.m_nextMotion = "\xC8\xA6\xC0\xCE";
		break;
	case 6:
		m_pipinTime.m_nextMotion = "\xC8\xA6\xC0\xCE\xBF\xF8";
		break;
	}
}

// tutorialmain.cpp:1557
void CTutorialMain::MissionComplete(unsigned long flags) {
	for (int i = 0; i < 8; i++) {
		if (data_9f7be8[i].m_mask & flags)
			MissionClear(i, flags);

		if (i == 7 && (flags & 0x80))
			m_categoryClear[0] = true;
	}
}

// tutorialmain.cpp:1569
void CTutorialMain::MissionClear(int &index, unsigned long &flags) {
	sTutorialInfo *info = FindTutorialList(index);
	if (info) {
		if (flags & data_9f7be8[index].m_mask) {
			info->m_cleared = true;
			CheckEnable(index);
		}

		if (index == 7 && (flags & 0x80))
			m_categoryClear[0] = true;
	}
}

// tutorialmain.cpp:1591
void CTutorialMain::OpenTutorialInfoDlg(void) {
	if (m_tutorialInfoDlg == 0) {
		m_tutorialInfoDlg = CreateForm<FrQuestInfoDlg>(g_pFresh->GetManager(), this, "quest_info", 0);
		m_tutorialInfoDlg->SetQuestExplane(m_questIndex);
		m_tutorialInfoDlg->Open((FRESH_RESULT_CALLBACK)&CTutorialMain::OnTutorialInfoDlgResult, 3);
	}
}

// tutorialmain.cpp:1601
bool CTutorialMain::OnTutorialInfoDlgResult(int result, FrForm *form) {
	if (result == 0)
		return true;

	m_tutorialInfoDlg = 0;
	if (result == 1)
		StartSelectedTutorial();

	if (m_pendingTutorial != 0xFF) {
		m_Tutorials[m_pendingTutorial].m_pending = 0;
		m_pendingTutorial = 0xFF;
	}

	return true;
}

// tutorialmain.cpp:1628
bool CTutorialMain::OnGiftDlgResult(int result, FrForm *form) {
	if (result == 1) {
		if (m_gradeGift)
			OpenGradeGift(m_gradeGift);
	}
	return true;
}

// tutorialmain.cpp:1641
void CTutorialMain::StartSelectedTutorial(void) {
	bool valid = true;
	if (m_questState < 0 || (unsigned int)m_questState > 3)
		valid = false;

	if (m_questCategory < 0 || m_questNextIndex < 0 || m_questIndex < 0)
		valid = false;

	if (!valid) {
		HandleMsg(MsgObject(0, 0x23, (intptr_t)"\xBF\xAC\xBD\xC0\xB8\xF0\xB5\xE5 \xBC\xB3\xC1\xA4\xC0\xCC \xC0\xDF\xB8\xF8\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
		return;
	}

	if (m_questState > 0) {
		HandleMsg(MsgObject(0, 0x23, (intptr_t)"Not supported yet.", 0, 0, 0, 0));
		return;
	}

	if (m_pet)
		HidePet();

	switch (m_questState) {
	case 0:
		InitGolfTutorial();
		WSingleton<CMouseCursor>::Instance()->m_mode = 1;
		WSingleton<CMouseCursor>::Instance()->SetActive(false);
		break;
	}

	WSingleton<CTaskManager>::Instance()->ChangeTask(s_tutorialTasks[m_questState], s_tutorialTaskParams[m_questState], true);

	int length = strlen(s_tutorialMsgParams[m_questState]);
	if (length != 0)
		WSingleton<CTaskManager>::Instance()->PostMsg(0, s_tutorialMsgTargets[m_questState], 0, (intptr_t)s_tutorialMsgParams[m_questState], 0, 0, 0);
}

// tutorialmain.cpp:1702
void CTutorialMain::InitGolfTutorial(void) {
	// CSharedDoc 0x4D98 (inside PADDING(0x4D98, 0x4DBC) in SharedDoc.h, not
	// owned by this unit): the tutorial quest the golf task plays.
	*(int *)((char *)WSingleton<CSharedDoc>::Instance() + 0x4D98) = m_questNextIndex;
	WSingleton<CSharedDoc>::Instance()->m_gameMode = 0;

	SetCurMap(0xFD);
	SetGameType(12);

	for (unsigned char i = 0; i < 18; i++)
		WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] = 0;

	SetShotTimeLimit(0);

	WSingleton<CSharedDoc>::Instance()->m_holes = 1;
	WSingleton<CSharedDoc>::Instance()->m_holeOrder[0] = m_questNextIndex < 14 ? 1 : 2;

	// SetWeather (shareddoc.inl) is only declared in SharedDoc.h, not inline,
	// and CSharedDoc::m_weather (0x476E) is protected.
	*((unsigned char *)WSingleton<CSharedDoc>::Instance() + 0x476E) = 0;

	for (std::list<sSlotInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.end(); ++it)
		it->m_attribute &= ~4;

	WSingleton<CSharedDoc>::Instance()->m_gameOver = false;
}

// tutorialmain.cpp:1734
CTutorialMain::sTutorialInfo *CTutorialMain::FindTutorialList(int &id) {
	sTutorialInfo *found = 0;
	for (std::list<sTutorialInfo>::iterator it = m_tutorialList.begin(); it != m_tutorialList.end(); ++it) {
		sTutorialInfo *info = &(*it);
		if (info->m_id == id) {
			found = info;
			break;
		}
	}
	return found;
}

// tutorialmain.cpp:1752
void CTutorialMain::CheckEnable(int index) {
	int count;
	int unlock = m_TutoCondition[index].m_unlock;
	if (unlock == 0) {
		for (int i = 0; i < m_TutoCondition[index].m_count; i++) {
			int id = m_TutoCondition[index].m_start + i;
			sTutorialInfo *info = FindTutorialList(id);
			if (info == 0)
				return;

			info->m_enabled = true;
		}
	} else {
		int cleared = 0;
		count = m_TutoCondition[index].m_count;
		for (int i = 0; i < count; i++) {
			int id = m_TutoCondition[index].m_start + i;
			sTutorialInfo *info = FindTutorialList(id);
			if (info == 0)
				return;

			if (info->m_cleared == true)
				cleared++;
		}

		if (cleared == count) {
			sTutorialInfo *info = FindTutorialList(unlock);
			if (info == 0)
				return;

			info->m_enabled = true;
		}
	}
}

// tutorialmain.cpp:1800
bool CTutorialMain::IsCompleteQuest(int category, int index) {
	bool complete = false;
	if (m_Tutorials[index].m_category == category) {
		if (WSingleton<CSharedDoc>::Instance()->m_tutorialData[category] & data_9f7be8[index].m_mask)
			complete = true;
	}
	return complete;
}

// tutorialmain.cpp:1815
bool CTutorialMain::IsCompleteCategory(int category) {
	int start = category;
	int end = category;
	switch (category) {
	case 0:
		start = 0;
		end = 8;
		break;
	case 1:
		start = 8;
		end = 14;
		break;
	case 2:
		start = 14;
		end = 20;
		break;
	}

	bool complete = true;
	for (int i = start; i < end; ++i) {
		if ((WSingleton<CSharedDoc>::Instance()->m_tutorialData[category] & data_9f7be8[i].m_mask) == 0) {
			complete = false;
			break;
		}
	}

	return complete;
}

// tutorialmain.cpp:1848
bool CTutorialMain::OnQuestionFormResult(int result, FrForm *form) {
	if (result == 0)
		return true;

	m_questionForm = 0;
	if (result == 1)
		OpenTutorialInfoDlg();

	if (m_pendingTutorial != 0xFF) {
		m_Tutorials[m_pendingTutorial].m_pending = 0;
		m_pendingTutorial = 0xFF;
	}

	return true;
}
