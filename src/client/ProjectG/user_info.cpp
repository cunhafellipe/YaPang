// Original: d:\build\custom\temp\code\client\projectg\user_info.cpp
#include <algorithm>
#include "NetworkSystem.h"
#include "user_info.h"
#include "AddFriendDlg.h"
#include "ObjectFactory.h"
#include "commonutil.h"
#include "frarea.h"
#include "fredit.h"
#include "frlistbox.h"
#include "frstatic.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "SharedDoc.h"
#include "exhibition.h"
#include "partsinfo.h"
#include "UccManager.h"
#include "parttidlist.h"
#include "frgaugebar.h"
#include "frgraphicinterface.h"
#include "wresrcmng.h"
#include "client/Wangreal/include/wfont.h"
#include "shared/Localize.h"
#include "Packet.h"
#include "TaskManager.h"
#include "actor.h"
#include "GolfTask.h"
#include "LobbyTask.h"
#include "ChatMsg.h"
#include "avatar_task.h"
#include "NetResourceManager.h"
#include "ProjectG.h"
#include "bitmap.h"
#include "buddymanager.h"
#include "messengerdlg.h"
#include "InputManager.h"
#include "SoundManager.h"

extern unsigned long g_CurrentTime;

// user_info.cpp:40 (run-time type and factory registration of FrUserInfoForm)
const WRTTI FrUserInfoForm::m_RTTI("FrUserInfoForm", &FrForm::m_RTTI);

IObject* FrUserInfoFormMakeInstance(void) {
	return new FrUserInfoForm;
}

struct __sFrUserInfoForm {
	__sFrUserInfoForm(void) {
		ObjectFactory().AddObjectFunctor(FrUserInfoFormMakeInstance, "FrUserInfoForm");
	}
};
static __sFrUserInfoForm s_registerFrUserInfoForm;

// user_info.cpp:42
const sFRESH_MSGMAP* FrUserInfoForm::GetMessageMap(void) const {
	return &FrUserInfoForm::_MsgMap;
}

sFRESH_MSGMAP FrUserInfoForm::_MsgMap = { &FrForm::_MsgMap, &FrUserInfoForm::_MsgEntries[0] };

sFRESH_ENTRY FrUserInfoForm::_MsgEntries[] = {
	// user_info.cpp:49
	{ "friend", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnFriendInit },
	// user_info.cpp:50
	{ "friend", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnFriendUp },
	// user_info.cpp:51
	{ "whisper", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnWhisperInit },
	// user_info.cpp:52
	{ "whisper", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnWhisperUp },
	// user_info.cpp:53
	{ "ignore", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnIgnoreInit },
	// user_info.cpp:54
	{ "ignore", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnIgnoreUp },
	// user_info.cpp:55
	{ "cancel", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnCloseUp },
	// user_info.cpp:57
	{ "ghost", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnGhostInit },
	// user_info.cpp:58
	{ "ghost", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnGhostdUp },
	// user_info.cpp:60
	{ "pet", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnPetInit },
	// user_info.cpp:61
	{ "pet", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnPetOwnerDraw },
	// user_info.cpp:62
	{ "pet_load", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnPetLoadInit },
	// user_info.cpp:63
	{ "nick", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnNickInit },
	// user_info.cpp:64
	{ "nick", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnNickOwnerDraw },
	// user_info.cpp:66
	{ "general", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnGeneralInfoInit },
	// user_info.cpp:67
	{ "general", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnGeneralInfoDown },
	// user_info.cpp:68
	{ "special", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnSpecialInfoInit },
	// user_info.cpp:69
	{ "special", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnSpecialInfoDown },
	// user_info.cpp:70
	{ "record", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnRecordInfoInit },
	// user_info.cpp:71
	{ "record", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnRecordInfoDown },
	// user_info.cpp:72
	{ "trophy", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnTrophyInfoInit },
	// user_info.cpp:73
	{ "trophy", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnTrophyInfoDown },
	// user_info.cpp:78
	{ "guild", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnGuildInfoInit },
	// user_info.cpp:79
	{ "guild", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnGuildInfoDown },
	// user_info.cpp:79 (same line as the previous row)
	{ "general_back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnGeneralBackInit },
	// user_info.cpp:79 (same line as the previous row)
	{ "rec_course_back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnRecCourseBackInit },
	// user_info.cpp:83
	{ "tro_normal_back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnTroNormalBackInit },
	// user_info.cpp:84
	{ "tro_special_back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnTroSpecialBackInit },
	// user_info.cpp:85
	{ "guild_back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnGuildBackInit },
	// user_info.cpp:87
	{ "btn_season", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnTitleSeasonBtnInit },
	// user_info.cpp:88
	{ "btn_season", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnTitleSeasonBtnUp },
	// user_info.cpp:89
	{ "btn_total", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnTitleTotalBtnInit },
	// user_info.cpp:90
	{ "btn_total", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnTitleTotalBtnUp },
	// user_info.cpp:94
	{ "eg0", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_GI_ID },
	// user_info.cpp:95
	{ "eg1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_GI_SCHOOL },
	// user_info.cpp:96
	{ "eg2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_GI_CLUB },
	// user_info.cpp:97
	{ "eg3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_GI_EXP },
	// user_info.cpp:98
	{ "eg4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_GI_CONNSTATUS },
	// user_info.cpp:102
	{ "eg5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_GI_DISCONN },
	// user_info.cpp:104
	{ "eg6", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_GI_COMBO },
	// user_info.cpp:110
	{ "general_level", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnAreaInit_GI_Level },
	// user_info.cpp:111
	{ "general_guild", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnAreaInit_GI_ClUB },
	// user_info.cpp:112
	{ "general_guild", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnAreaDraw_GI_ClUB },
	// user_info.cpp:114
	{ "general_question", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnAreaInit_Question_mark },
	// user_info.cpp:117
	{ "Btn_rmrvisitor", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnBtn_RmrVisitorInit },
	// user_info.cpp:118
	{ "Btn_rmrvisitor", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnBtn_RmrVisitorBtnUp },
	// user_info.cpp:122
	{ "speclist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnSpecListboxInit },
	// user_info.cpp:123
	{ "speclist", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnSpecListboxOwnerDraw },
	// user_info.cpp:124
	{ "es0_1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_SI_TROPHY_GOLD },
	// user_info.cpp:125
	{ "es0_2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_SI_TROPHY_SILVER },
	// user_info.cpp:126
	{ "es0_3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_SI_TROPHY_BRONZE },
	// user_info.cpp:127
	{ "es1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_SI_MATCH_WIN_PERCENTAGE },
	// user_info.cpp:128
	{ "es2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_SI_AVG_SCORE },
	// user_info.cpp:129
	{ "es3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_SI_MEAN_SHOTTIME },
	// user_info.cpp:130
	{ "es4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_SI_PANGYA_PERCENTAGE },
	// user_info.cpp:131
	{ "es5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_SI_FAIRWAY_PERCENTAGE },
	// user_info.cpp:132
	{ "es6", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_SI_PUTT_PERCENTAGE },
	// user_info.cpp:133
	{ "es7", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_SI_OB_PERCENTAGE },
	// user_info.cpp:134
	{ "es8", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_SI_TOTAL_SKINS_PANG },
	// user_info.cpp:136
	{ "mh1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_SI_MATCH_HISTORY1 },
	// user_info.cpp:137
	{ "mh2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_SI_MATCH_HISTORY2 },
	// user_info.cpp:138
	{ "mh3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_SI_MATCH_HISTORY3 },
	// user_info.cpp:139
	{ "mh4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_SI_MATCH_HISTORY4 },
	// user_info.cpp:140
	{ "mh5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_SI_MATCH_HISTORY5 },
	// user_info.cpp:142
	{ "btn_si1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnMatchHistoryBtn1Init },
	// user_info.cpp:143
	{ "btn_si1", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnMatchHistoryBtn1Up },
	// user_info.cpp:144
	{ "btn_si2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnMatchHistoryBtn2Init },
	// user_info.cpp:145
	{ "btn_si2", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnMatchHistoryBtn2Up },
	// user_info.cpp:146
	{ "btn_si3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnMatchHistoryBtn3Init },
	// user_info.cpp:147
	{ "btn_si3", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnMatchHistoryBtn3Up },
	// user_info.cpp:148
	{ "btn_si4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnMatchHistoryBtn4Init },
	// user_info.cpp:149
	{ "btn_si4", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnMatchHistoryBtn4Up },
	// user_info.cpp:150
	{ "btn_si5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnMatchHistoryBtn5Init },
	// user_info.cpp:151
	{ "btn_si5", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnMatchHistoryBtn5Up },
	// user_info.cpp:154
	{ "recordlist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnRecTotalListboxInit },
	// user_info.cpp:155
	{ "recordlist", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnRecTotalListboxOwnerDraw },
	// user_info.cpp:156
	{ "btn_rec0", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnRecordNormalBtnInit },
	// user_info.cpp:157
	{ "btn_rec0", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnRecordNormalBtnUp },
	// user_info.cpp:163
	{ "btn_rec1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnRecordCourseBtnInit },
	// user_info.cpp:164
	{ "btn_rec1", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnRecordCourseBtnUp },
	// user_info.cpp:165
	{ "btn_rec2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnRecordClassicBtnInit },
	// user_info.cpp:166
	{ "btn_rec2", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnRecordClassicBtnUp },
	// user_info.cpp:168
	{ "er0", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_RI_TOT_PLAYTIME },
	// user_info.cpp:169
	{ "er0", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOn },
	// user_info.cpp:170
	{ "er0", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOff },
	// user_info.cpp:171
	{ "er1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_RI_TOT_SHOTS },
	// user_info.cpp:172
	{ "er1", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOn },
	// user_info.cpp:173
	{ "er1", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOff },
	// user_info.cpp:174
	{ "er2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_RI_TOT_HOLES },
	// user_info.cpp:175
	{ "er2", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOn },
	// user_info.cpp:176
	{ "er2", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOff },
	// user_info.cpp:177
	{ "er3_0", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_RI_EARTH_ROUND },
	// user_info.cpp:178
	{ "er3_0", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOn },
	// user_info.cpp:179
	{ "er3_0", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOff },
	// user_info.cpp:180
	{ "er3_1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_RI_TOT_DRIVE },
	// user_info.cpp:181
	{ "er3_1", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOn },
	// user_info.cpp:182
	{ "er3_1", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOff },
	// user_info.cpp:183
	{ "er4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_RI_BEST_SCORE_3 },
	// user_info.cpp:184
	{ "er4", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOn },
	// user_info.cpp:185
	{ "er4", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOff },
	// user_info.cpp:186
	{ "er5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_RI_BEST_SCORE_2 },
	// user_info.cpp:187
	{ "er6", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_RI_BEST_SCORE_1 },
	// user_info.cpp:188
	{ "er7", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_RI_TOT_HOLEINONES },
	// user_info.cpp:189
	{ "er7", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOn },
	// user_info.cpp:190
	{ "er7", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOff },
	// user_info.cpp:191
	{ "er8", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_RI_TOT_ALBATROSS },
	// user_info.cpp:192
	{ "er8", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOn },
	// user_info.cpp:193
	{ "er8", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOff },
	// user_info.cpp:194
	{ "er9", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_RI_LONGEST_DRIVE },
	// user_info.cpp:195
	{ "er9", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOn },
	// user_info.cpp:196
	{ "er9", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOff },
	// user_info.cpp:197
	{ "er10", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_RI_LONGEST_PUTT },
	// user_info.cpp:198
	{ "er10", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOn },
	// user_info.cpp:199
	{ "er10", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOff },
	// user_info.cpp:228 (no line mark of its own)
	{ "er11", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_RI_LONGEST_CHIPIN },
	// user_info.cpp:229
	{ "er11", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOn },
	// user_info.cpp:230
	{ "er11", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_HoverOff },
	// user_info.cpp:230 (same line as the previous row)
	{ "er12", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_RI_BEST_SCORE_5 },
	// user_info.cpp:230 (same line as the previous row)
	{ "er13", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_RI_BEST_SCORE_4 },
	// user_info.cpp:230 (same line as the previous row)
	{ "scorelist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnScoreListInit },
	// user_info.cpp:230 (same line as the previous row)
	{ "scorelist", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnScoreListOwnerDraw },
	// user_info.cpp:233
	{ "tri_normalnum", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnTrophyNormalNumInit },
	// user_info.cpp:234
	{ "tri_specialnum", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnTrophySpecialNumInit },
	// user_info.cpp:235
	{ "btn_normal", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnTrophyNormalBtnInit },
	// user_info.cpp:236
	{ "btn_normal", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnTrophyNormalBtnUp },
	// user_info.cpp:237
	{ "btn_special", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnTrophySpecialBtnInit },
	// user_info.cpp:238
	{ "btn_special", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnTrophySpecialBtnUp },
	// user_info.cpp:239
	{ "trispecialbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnListBoxInit_TRI_SPECIALBOX },
	// user_info.cpp:240
	{ "trispecialbox", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnListBoxOwnerDraw_TRI_SPECIALBOX },
	// user_info.cpp:241
	{ "trinormalbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnListBoxInit_TRI_NORMALBOX },
	// user_info.cpp:242
	{ "trinormalbox", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnListBoxOwnerDraw_TRI_NORMALBOX },
	// user_info.cpp:243
	{ "trinotrophy", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnStaticInit_TRI_NOTROPHY },
	// user_info.cpp:246
	{ "eguild0", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_GUILD_NAME },
	// user_info.cpp:247
	{ "eguild1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_GUILD_PANG },
	// user_info.cpp:248
	{ "eguild2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnEditInit_GUILD_MYPERCENT },
	// user_info.cpp:249
	{ "guildnotrophy", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnStaticInit_GUILD_NOTROPHY },
	// user_info.cpp:251
	{ "myguild", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnMyGuildInit },
	// user_info.cpp:252
	{ "myguild", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrUserInfoForm::OnMyGuildOwnerDraw },
	// user_info.cpp:254
	{ "guildlist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnGuildTrophyListInit },
	// user_info.cpp:255
	{ "guildlist", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnGuildTrophyListOwnerDraw },
	// user_info.cpp:257
	{ "comment", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUserInfoForm::OnCommentInit },
	// user_info.cpp:259
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// user_info.cpp:263
FrUserInfoForm::FrUserInfoForm(void) {
	m_seasonTab = SEASON_CURRENT;

	memset(&m_pCurSeasonBtn, 0, sizeof(FrButton*) * 2);

	memset(m_pTabBtn, 0, sizeof(m_pTabBtn));
	memset(&m_pGeneralBack, 0, sizeof(FrArea*) * 7);
	memset(&m_pIdEdit, 0, sizeof(FrEdit*) * 8);
	memset(m_pSpecialInfoWnd, 0, sizeof(m_pSpecialInfoWnd));

	memset(&m_pRecordNormalBtn, 0, sizeof(FrButton*) * 4);

	memset(m_pSpecialInfoWnd, 0, sizeof(m_pSpecialInfoWnd));
	memset(m_pRecordEdit, 0, sizeof(m_pRecordEdit));
	memset(&m_pTrophyNormalBtn, 0, sizeof(FrButton*) * 2);
	memset(m_pGuildInfoEdit, 0, sizeof(m_pGuildInfoEdit));
	memset(&m_pRecTotalBack01Bmp, 0, sizeof(const Bitmap*) * 3);
	memset(m_pRecAwardWnd, 0, sizeof(m_pRecAwardWnd));

	m_pRecTotalList = NULL;
	m_pLevelArea = NULL;
	m_pClubArea = NULL;
	m_pDisconnQuestionMark = NULL;
	m_trophyListState = 0;
	m_pTrophySpecialList = NULL;
	m_pTrophyNormalList = NULL;
	m_pNoTrophyStatic = NULL;
	m_pTrophyNormalNum = NULL;
	m_pTrophySpecialNum = NULL;
	m_pGuildNoTrophyStatic = NULL;
	m_recordType = 0;
	m_trophyType = 0;
	m_pGuildTrophyList = NULL;

	m_seasonRecord.Clear();
	m_totalRecord.Clear();

	memset(&m_userInfo, 0, sizeof(sUserInfo));
	m_userInfo.m_guid = 0xFFFFFFFF;
	m_userInfo.m_uid = 0xFFFFFFFF;

	m_pFriendBtn = NULL;
	m_pWhisperBtn = NULL;
	m_pIgnoreBtn = NULL;
	m_pPetArea = NULL;
	m_pPetExhibition = NULL;
	m_pPetLoadGauge = NULL;
	m_pPetTids = NULL;
	m_pCommentEdit = NULL;
	m_pNickArea = NULL;

	for (int axis = 0; axis < 2; axis++) {
		m_skinScrollState[axis] = 0;
		m_skinScroll[axis] = 0;
	}

	m_pScoreList = NULL;
	m_pTitleFont = NULL;
	m_pMyGuildArea = NULL;
	m_pGuildMarkArea = NULL;
	m_pGuildTrophyBmp = NULL;
	m_bOption = false;
	m_bMsnControl = false;
	m_bControlOff = false;
	m_guildPang = 0;
	m_guildMyPercent = 0;
	memset(m_friendExtra.data, 0, sizeof(m_friendExtra.data));
	m_friendExtra.index = 0xFF;
	m_friendExtra.code = 0xFFFF;
	m_friendExtra.id1 = 0xFFFFFFFF;
	m_friendExtra.id2 = 0xFFFFFFFF;

	m_bChatActive = false;

	// clears 0x58C bytes from here, through 0x768 inside m_userInfo
	memset(m_pMatchHistoryBtn, 0, 0x58C);

	m_pSpecList = NULL;
	m_bMatchHistoryRequested = false;
	m_bFriendRequested = false;
	m_bCheckTime = false;
	m_lastElapsed = 0;
	m_pGhostBtn = 0;
	m_pRmrVisitorBtn = NULL;
	m_recvSeasonFlags = 0;
	m_seasonPending = 0;
	memset(m_classicMapStats, 0, sizeof(m_classicMapStats));
	for (int i = 0; i < 20; i++) {
		m_classicMapStats[i].m_bestScore = 0x7F;
	}
	memset(m_seasonClassicMapStats, 0, sizeof(m_seasonClassicMapStats));
	for (int j = 0; j < 20; j++) {
		m_seasonClassicMapStats[j].m_bestScore = 0x7F;
	}
	memset(m_totalClassicMapStats, 0, sizeof(m_totalClassicMapStats));
	for (int k = 0; k < 20; k++) {
		m_totalClassicMapStats[k].m_bestScore = 0x7F;
	}
	memset(m_pBestMapStats, 0, sizeof(m_pBestMapStats));

	m_addFriendDlg = NULL;
}

// user_info.cpp:372
FrUserInfoForm::~FrUserInfoForm(void) {
	Clear();
}

// user_info.cpp:386
void FrUserInfoForm::Clear(void) {
	if (CUserInfo::Instance()) {
		CUserInfo::Instance()->m_dlg = NULL;
	}

	g_resrcmng->UnlockAllByThread(m_anotherHand.GetThreadID());
	m_anotherHand.BreakWorkingThread();
	HidePet(true);

	if (g_resrcmng && m_pTitleFont) {
		g_resrcmng->Release(m_pTitleFont);
		m_pTitleFont = NULL;
	}
}

// user_info.cpp:404
void FrUserInfoForm::OnTitleSeasonBtnInit(PG_MSGARG param) {
	m_pCurSeasonBtn = DYNAMIC_CAST(FrButton, param);

	if (m_seasonTab == SEASON_CURRENT) {
		m_pCurSeasonBtn->SetStatus(FrButton::PRESSED);
	} else {
		m_pCurSeasonBtn->SetStatus(FrButton::NORMAL);
	}
}

// user_info.cpp:414
void FrUserInfoForm::OnTitleSeasonBtnUp(void) {
	if (!(m_recvSeasonFlags & 4)) {
		RequestSeasonData(5);
	}
	OnTitleSeasonVisible();
}

// user_info.cpp:423
void FrUserInfoForm::OnTitleTotalBtnInit(PG_MSGARG param) {
	m_pTotalSeasonBtn = DYNAMIC_CAST(FrButton, param);

	if (m_seasonTab == SEASON_TOTAL) {
		m_pTotalSeasonBtn->SetStatus(FrButton::PRESSED);
	} else {
		m_pTotalSeasonBtn->SetStatus(FrButton::NORMAL);
	}
}

// user_info.cpp:433
void FrUserInfoForm::OnTitleTotalBtnUp(void) {
	if (!(m_recvSeasonFlags & 2)) {
		RequestSeasonData(0);
	}

	m_seasonTab = SEASON_TOTAL;
	m_pCurSeasonBtn->SetStatus(FrButton::NORMAL);
	m_pTotalSeasonBtn->SetStatus(FrButton::PRESSED);

	SetContent(true);
	SetTrophyStatic();

	SetSpecListBoxItem(SEASON_TOTAL);
}

// user_info.cpp:451
void FrUserInfoForm::SetTrophyStatic(void) {
	if (m_trophyType == 1) {
		if (m_pNoTrophyStatic) {
			m_pNoTrophyStatic->SetVisible(m_curTab == TAB_TROPHY_INFO && m_pTrophySpecialList->m_itemList.size() == 0);
		}
	} else if (m_trophyType == 0) {
		if (m_pNoTrophyStatic) {
			m_pNoTrophyStatic->SetVisible(m_curTab == TAB_TROPHY_INFO && m_pTrophyNormalList->m_itemList.size() == 0);
		}
	}

	if (m_pGuildNoTrophyStatic) {
		m_pGuildNoTrophyStatic->SetVisible(m_curTab == TAB_GUILD_INFO && m_pGuildTrophyList->m_itemList.size() == 0);
	}
}

// user_info.cpp:471
void FrUserInfoForm::OnGeneralInfoInit(PG_MSGARG param) {
	m_pTabBtn[TAB_GENERAL_INFO] = DYNAMIC_CAST(FrButton, param);
}

// user_info.cpp:476
void FrUserInfoForm::OnGeneralInfoDown(void) {
	SetFormA(TAB_GENERAL_INFO);
}

// user_info.cpp:481
void FrUserInfoForm::OnSpecialInfoInit(PG_MSGARG param) {
	m_pTabBtn[TAB_SPECIAL_INFO] = DYNAMIC_CAST(FrButton, param);
}

// user_info.cpp:486
void FrUserInfoForm::OnSpecialInfoDown(void) {
	SetFormA(TAB_SPECIAL_INFO);
}

// user_info.cpp:491
void FrUserInfoForm::OnRecordInfoInit(PG_MSGARG param) {
	m_pTabBtn[TAB_RECORD_INFO] = DYNAMIC_CAST(FrButton, param);
}

// user_info.cpp:496
void FrUserInfoForm::OnRecordInfoDown(void) {
	SetFormA(TAB_RECORD_INFO);
}

// user_info.cpp:501
void FrUserInfoForm::OnTrophyInfoInit(PG_MSGARG param) {
	m_pTabBtn[TAB_TROPHY_INFO] = DYNAMIC_CAST(FrButton, param);
}

// user_info.cpp:506
void FrUserInfoForm::OnTrophyInfoDown(void) {
	SetFormA(TAB_TROPHY_INFO);
}

// user_info.cpp:511
void FrUserInfoForm::OnGuildInfoInit(PG_MSGARG param) {
	m_pTabBtn[TAB_GUILD_INFO] = DYNAMIC_CAST(FrButton, param);
}

// user_info.cpp:516
void FrUserInfoForm::OnGuildInfoDown(void) {
	SetFormA(TAB_GUILD_INFO);
}

// user_info.cpp:545
void FrUserInfoForm::OnGeneralBackInit(PG_MSGARG param) {
	m_pGeneralBack = DYNAMIC_CAST(FrArea, param);
}

// user_info.cpp:550
void FrUserInfoForm::OnRecCourseBackInit(PG_MSGARG param) {
	m_pRecCourseBack = DYNAMIC_CAST(FrArea, param);
}

// user_info.cpp:555
void FrUserInfoForm::OnTroNormalBackInit(PG_MSGARG param) {
	m_pTroNormalBack = DYNAMIC_CAST(FrArea, param);
}

// user_info.cpp:560
void FrUserInfoForm::OnTroSpecialBackInit(PG_MSGARG param) {
	m_pTroSpecialBack = DYNAMIC_CAST(FrArea, param);
}

// user_info.cpp:565
void FrUserInfoForm::OnGuildBackInit(PG_MSGARG param) {
	m_pGuildBack = DYNAMIC_CAST(FrArea, param);
}

// user_info.cpp:607
void FrUserInfoForm::OnEditInit_GI_ID(PG_MSGARG param) { m_pIdEdit = DYNAMIC_CAST(FrEdit, param); m_pIdEdit->EnableHover(true, 0.0f); }

// user_info.cpp:608
void FrUserInfoForm::OnEditInit_GI_SCHOOL(PG_MSGARG param) { m_pSchoolEdit = DYNAMIC_CAST(FrEdit, param); m_pSchoolEdit->EnableHover(true, 0.0f); }

// user_info.cpp:609
void FrUserInfoForm::OnEditInit_GI_CLUB(PG_MSGARG param) { m_pClubEdit = DYNAMIC_CAST(FrEdit, param); m_pClubEdit->EnableHover(true, 0.0f); }

// user_info.cpp:610
void FrUserInfoForm::OnEditInit_GI_EXP(PG_MSGARG param) { m_pExpEdit = DYNAMIC_CAST(FrEdit, param); m_pExpEdit->EnableHover(true, 0.0f); }

// user_info.cpp:611
void FrUserInfoForm::OnEditInit_GI_CONNSTATUS(PG_MSGARG param) { m_pConnStatusEdit = DYNAMIC_CAST(FrEdit, param); m_pConnStatusEdit->EnableHover(true, 0.0f); }

// user_info.cpp:615
void FrUserInfoForm::OnEditInit_GI_DISCONN(PG_MSGARG param) { m_pDisconnEdit = DYNAMIC_CAST(FrEdit, param); m_pDisconnEdit->EnableHover(true, 0.0f); }

// user_info.cpp:617
void FrUserInfoForm::OnEditInit_GI_COMBO(PG_MSGARG param) { m_pComboEdit = DYNAMIC_CAST(FrEdit, param); m_pComboEdit->EnableHover(true, 0.0f); }

// user_info.cpp:621
void FrUserInfoForm::OnEditInit_GI_QUIZLEVEL(int param) { m_pQuizLevelEdit = DYNAMIC_CAST(FrEdit, param); m_pQuizLevelEdit->EnableHover(true, 0.0f); }

// user_info.cpp:623
void FrUserInfoForm::OnEditInit_SI_TROPHY_GOLD(PG_MSGARG param) { m_pTrophyGoldEdit = DYNAMIC_CAST(FrEdit, param); m_pTrophyGoldEdit->EnableHover(true, 0.0f); }

// user_info.cpp:624
void FrUserInfoForm::OnEditInit_SI_TROPHY_SILVER(PG_MSGARG param) { m_pTrophySilverEdit = DYNAMIC_CAST(FrEdit, param); m_pTrophySilverEdit->EnableHover(true, 0.0f); }

// user_info.cpp:625
void FrUserInfoForm::OnEditInit_SI_TROPHY_BRONZE(PG_MSGARG param) { m_pTrophyBronzeEdit = DYNAMIC_CAST(FrEdit, param); m_pTrophyBronzeEdit->EnableHover(true, 0.0f); }

// user_info.cpp:626
void FrUserInfoForm::OnEditInit_SI_MATCH_WIN_PERCENTAGE(PG_MSGARG param) { m_pMatchWinPercentEdit = DYNAMIC_CAST(FrEdit, param); m_pMatchWinPercentEdit->EnableHover(true, 0.0f); }

// user_info.cpp:627
void FrUserInfoForm::OnEditInit_SI_AVG_SCORE(PG_MSGARG param) { m_pAvgScoreEdit = DYNAMIC_CAST(FrEdit, param); m_pAvgScoreEdit->EnableHover(true, 0.0f); }

// user_info.cpp:628
void FrUserInfoForm::OnEditInit_SI_MEAN_SHOTTIME(PG_MSGARG param) { m_pMeanShotTimeEdit = DYNAMIC_CAST(FrEdit, param); m_pMeanShotTimeEdit->EnableHover(true, 0.0f); }

// user_info.cpp:629
void FrUserInfoForm::OnEditInit_SI_PANGYA_PERCENTAGE(PG_MSGARG param) { m_pPangyaPercentEdit = DYNAMIC_CAST(FrEdit, param); m_pPangyaPercentEdit->EnableHover(true, 0.0f); }

// user_info.cpp:630
void FrUserInfoForm::OnEditInit_SI_FAIRWAY_PERCENTAGE(PG_MSGARG param) { m_pFairwayPercentEdit = DYNAMIC_CAST(FrEdit, param); m_pFairwayPercentEdit->EnableHover(true, 0.0f); }

// user_info.cpp:631
void FrUserInfoForm::OnEditInit_SI_PUTT_PERCENTAGE(PG_MSGARG param) { m_pPuttPercentEdit = DYNAMIC_CAST(FrEdit, param); m_pPuttPercentEdit->EnableHover(true, 0.0f); }

// user_info.cpp:632
void FrUserInfoForm::OnEditInit_SI_OB_PERCENTAGE(PG_MSGARG param) { m_pObPercentEdit = DYNAMIC_CAST(FrEdit, param); m_pObPercentEdit->EnableHover(true, 0.0f); }

// user_info.cpp:633
void FrUserInfoForm::OnEditInit_SI_TOTAL_SKINS_PANG(PG_MSGARG param) { m_pTotalSkinsPangEdit = DYNAMIC_CAST(FrEdit, param); m_pTotalSkinsPangEdit->EnableHover(true, 0.0f); }

// user_info.cpp:644
void FrUserInfoForm::OnEditInit_RI_TOT_PLAYTIME(PG_MSGARG param) { m_pRecordEdit[0] = DYNAMIC_CAST(FrEdit, param); m_pRecordEdit[0]->EnableHover(true, 0.0f); }

// user_info.cpp:645
void FrUserInfoForm::OnEditInit_RI_TOT_SHOTS(PG_MSGARG param) { m_pRecordEdit[1] = DYNAMIC_CAST(FrEdit, param); m_pRecordEdit[1]->EnableHover(true, 0.0f); }

// user_info.cpp:646
void FrUserInfoForm::OnEditInit_RI_TOT_HOLES(PG_MSGARG param) { m_pRecordEdit[2] = DYNAMIC_CAST(FrEdit, param); m_pRecordEdit[2]->EnableHover(true, 0.0f); }

// user_info.cpp:647
void FrUserInfoForm::OnEditInit_RI_TOT_DRIVE(PG_MSGARG param) { m_pRecordEdit[4] = DYNAMIC_CAST(FrEdit, param); m_pRecordEdit[4]->EnableHover(true, 0.0f); }

// user_info.cpp:648
void FrUserInfoForm::OnEditInit_RI_EARTH_ROUND(PG_MSGARG param) { m_pRecordEdit[3] = DYNAMIC_CAST(FrEdit, param); m_pRecordEdit[3]->EnableHover(true, 0.0f); }

// user_info.cpp:649
void FrUserInfoForm::OnEditInit_RI_BEST_SCORE_5(PG_MSGARG param) { m_pRecordEdit[5] = DYNAMIC_CAST(FrEdit, param); m_pRecordEdit[5]->EnableHover(true, 0.0f); }

// user_info.cpp:650
void FrUserInfoForm::OnEditInit_RI_BEST_SCORE_4(PG_MSGARG param) { m_pRecordEdit[6] = DYNAMIC_CAST(FrEdit, param); m_pRecordEdit[6]->EnableHover(true, 0.0f); }

// user_info.cpp:651
void FrUserInfoForm::OnEditInit_RI_BEST_SCORE_3(PG_MSGARG param) { m_pRecordEdit[7] = DYNAMIC_CAST(FrEdit, param); m_pRecordEdit[7]->EnableHover(true, 0.0f); }

// user_info.cpp:652
void FrUserInfoForm::OnEditInit_RI_BEST_SCORE_2(PG_MSGARG param) { m_pRecordEdit[8] = DYNAMIC_CAST(FrEdit, param); m_pRecordEdit[8]->EnableHover(true, 0.0f); }

// user_info.cpp:653
void FrUserInfoForm::OnEditInit_RI_BEST_SCORE_1(PG_MSGARG param) { m_pRecordEdit[9] = DYNAMIC_CAST(FrEdit, param); m_pRecordEdit[9]->EnableHover(true, 0.0f); }

// user_info.cpp:654
void FrUserInfoForm::OnEditInit_RI_TOT_HOLEINONES(PG_MSGARG param) { m_pRecordEdit[10] = DYNAMIC_CAST(FrEdit, param); m_pRecordEdit[10]->EnableHover(true, 0.0f); }

// user_info.cpp:655
void FrUserInfoForm::OnEditInit_RI_TOT_ALBATROSS(PG_MSGARG param) { m_pRecordEdit[11] = DYNAMIC_CAST(FrEdit, param); m_pRecordEdit[11]->EnableHover(true, 0.0f); }

// user_info.cpp:656
void FrUserInfoForm::OnEditInit_RI_LONGEST_DRIVE(PG_MSGARG param) { m_pRecordEdit[12] = DYNAMIC_CAST(FrEdit, param); m_pRecordEdit[12]->EnableHover(true, 0.0f); }

// user_info.cpp:657
void FrUserInfoForm::OnEditInit_RI_LONGEST_PUTT(PG_MSGARG param) { m_pRecordEdit[13] = DYNAMIC_CAST(FrEdit, param); m_pRecordEdit[13]->EnableHover(true, 0.0f); }

// user_info.cpp:658
void FrUserInfoForm::OnEditInit_RI_LONGEST_CHIPIN(PG_MSGARG param) { m_pRecordEdit[14] = DYNAMIC_CAST(FrEdit, param); m_pRecordEdit[14]->EnableHover(true, 0.0f); }

// user_info.cpp:660
void FrUserInfoForm::OnEditInit_GUILD_NAME(PG_MSGARG param) { m_pGuildNameEdit = DYNAMIC_CAST(FrEdit, param); m_pGuildNameEdit->EnableHover(true, 0.0f); }

// user_info.cpp:661
void FrUserInfoForm::OnEditInit_GUILD_PANG(PG_MSGARG param) { m_pGuildPangEdit = DYNAMIC_CAST(FrEdit, param); m_pGuildPangEdit->EnableHover(true, 0.0f); }

// user_info.cpp:662
void FrUserInfoForm::OnEditInit_GUILD_MYPERCENT(PG_MSGARG param) { m_pGuildMyPercentEdit = DYNAMIC_CAST(FrEdit, param); m_pGuildMyPercentEdit->EnableHover(true, 0.0f); }

// user_info.cpp:668
void FrUserInfoForm::OnAreaInit_GI_Level(PG_MSGARG param) {
	m_pLevelArea = DYNAMIC_CAST(FrArea, param);
	if (m_pLevelArea) {
		m_pLevelArea->SetVisible(false);
	}
}

// user_info.cpp:675
void FrUserInfoForm::OnAreaInit_GI_ClUB(PG_MSGARG param) {
	m_pClubArea = DYNAMIC_CAST(FrArea, param);
	if (m_pClubArea) {
		m_pClubArea->SetVisible(false);
	}
}

// user_info.cpp:682
void FrUserInfoForm::OnAreaInit_Question_mark(PG_MSGARG param) {
	m_pDisconnQuestionMark = DYNAMIC_CAST(FrArea, param);

	if (m_pDisconnQuestionMark) {
		// "The disconnect rate shows once you reach Beginner E or play more than 50 games."
		m_pDisconnQuestionMark->SetToolTipText("\xb0\xad\xc1\xa6\xc1\xbe\xb7\xe1\xc0\xb2\xc0\xba \xba\xf1\xb1\xe2\xb3\xca" "E\xb0\xa1 \xb5\xc7\xb0\xc5\xb3\xaa \xb0\xd4\xc0\xd3\xc8\xbd\xbc\xf6 50\xc8\xb8\xb8\xa6 \xc3\xca\xb0\xfa\xc7\xd8\xbe\xdf \xba\xb8\xc0\xcc\xb0\xd4 \xb5\xcb\xb4\xcf\xb4\xd9.");
	}
	const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("USER_INFO", "user_info_question_mark");
	if (pBmp) {
		if (m_pDisconnQuestionMark) {
			m_pDisconnQuestionMark->SetBgImg(pBmp);
			m_pDisconnQuestionMark->SetVisible(false);
		}
	}
}

// user_info.cpp:700
void FrUserInfoForm::OnAreaDraw_GI_ClUB(PG_MSGARG param) {
	if (m_userInfo.m_guildUID == 0 || (m_userInfo.m_identity & 0x14))
		return;

	const Bitmap *pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(m_userInfo.m_guildMark);

	if (pEmblem == NULL)
		return;

	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	float x = m_pClubEdit->GetRect().w + m_pClubEdit->GetRect().x - pGDI->GetTextExtend(m_userInfo.m_guildName) - (float)pEmblem->Width() - 15.0f;

	pGDI->DrawTexture(pEmblem, WRect(x, m_pClubEdit->GetRect().y + 2.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
}

// user_info.cpp:729
void FrUserInfoForm::OnRecordNormalBtnInit(PG_MSGARG param) {
	m_pRecordNormalBtn = DYNAMIC_CAST(FrButton, param);

	if (m_recordType == 0) {
		m_pRecordNormalBtn->SetStatus(FrButton::PRESSED);
	} else {
		m_pRecordNormalBtn->SetStatus(FrButton::NORMAL);
	}
}

// user_info.cpp:739
void FrUserInfoForm::OnRecordNormalBtnUp(void) {
	m_recordType = 0;
	m_pRecordNormalBtn->SetStatus(FrButton::PRESSED);
	m_pRecordCourseBtn->SetStatus(FrButton::NORMAL);
	m_pRecordClassicBtn->SetStatus(FrButton::NORMAL);

	ChangeRecordType();
}

// user_info.cpp:778
void FrUserInfoForm::OnRecordCourseBtnInit(PG_MSGARG param) {
	m_pRecordCourseBtn = DYNAMIC_CAST(FrButton, param);

	if (m_recordType == 2) {
		m_pRecordCourseBtn->SetStatus(FrButton::PRESSED);
	} else {
		m_pRecordCourseBtn->SetStatus(FrButton::NORMAL);
	}
}

// user_info.cpp:788
void FrUserInfoForm::OnRecordCourseBtnUp(void) {
	m_recordType = 2;
	m_pRecordCourseBtn->SetStatus(FrButton::PRESSED);
	m_pRecordNormalBtn->SetStatus(FrButton::NORMAL);
	m_pRecordClassicBtn->SetStatus(FrButton::NORMAL);

	ChangeRecordType();
	UpdateEditMapStatistics(m_seasonTab, false);
}

// user_info.cpp:804
void FrUserInfoForm::OnRecordClassicBtnInit(PG_MSGARG param) {
	m_pRecordClassicBtn = DYNAMIC_CAST(FrButton, param);

	if (m_recordType == 3) {
		m_pRecordClassicBtn->SetStatus(FrButton::PRESSED);
	} else {
		m_pRecordClassicBtn->SetStatus(FrButton::NORMAL);
	}
}

// user_info.cpp:818
void FrUserInfoForm::OnRecordClassicBtnUp(void) {
	m_recordType = 3;
	m_pRecordClassicBtn->SetStatus(FrButton::PRESSED);
	m_pRecordCourseBtn->SetStatus(FrButton::NORMAL);
	m_pRecordNormalBtn->SetStatus(FrButton::NORMAL);

	ChangeRecordType();
	UpdateEditMapStatistics(m_seasonTab, true);
}

// user_info.cpp:838
void FrUserInfoForm::OnEditInit_SI_MATCH_HISTORY1(PG_MSGARG param) { m_pMatchHistoryEdit[0] = DYNAMIC_CAST(FrEdit, param); m_pMatchHistoryEdit[0]->EnableHover(true, 0.0f); }

// user_info.cpp:839
void FrUserInfoForm::OnEditInit_SI_MATCH_HISTORY2(PG_MSGARG param) { m_pMatchHistoryEdit[1] = DYNAMIC_CAST(FrEdit, param); m_pMatchHistoryEdit[1]->EnableHover(true, 0.0f); }

// user_info.cpp:840
void FrUserInfoForm::OnEditInit_SI_MATCH_HISTORY3(PG_MSGARG param) { m_pMatchHistoryEdit[2] = DYNAMIC_CAST(FrEdit, param); m_pMatchHistoryEdit[2]->EnableHover(true, 0.0f); }

// user_info.cpp:841
void FrUserInfoForm::OnEditInit_SI_MATCH_HISTORY4(PG_MSGARG param) { m_pMatchHistoryEdit[3] = DYNAMIC_CAST(FrEdit, param); m_pMatchHistoryEdit[3]->EnableHover(true, 0.0f); }

// user_info.cpp:842
void FrUserInfoForm::OnEditInit_SI_MATCH_HISTORY5(PG_MSGARG param) { m_pMatchHistoryEdit[4] = DYNAMIC_CAST(FrEdit, param); m_pMatchHistoryEdit[4]->EnableHover(true, 0.0f); }

// user_info.cpp:845
void FrUserInfoForm::OnSpecListboxInit(PG_MSGARG param) {
	m_pSpecList = DYNAMIC_CAST(FrListBox, param);
	m_pSpecList->SetVisible(false);

	m_pSpecBackBmp = g_pFresh->RegisterBitmap("special_back");
	m_pSpecBack00Bmp = g_pFresh->RegisterBitmap("special_back00");

	m_pSpecList->AddItem((void*)m_pSpecBackBmp);
	m_pSpecList->AddItem((void*)m_pSpecBack00Bmp);
}

// user_info.cpp:856
void FrUserInfoForm::OnMatchHistoryBtn1Init(PG_MSGARG param) { m_pMatchHistoryBtn[0] = DYNAMIC_CAST(FrButton, param); }

// user_info.cpp:857
void FrUserInfoForm::OnMatchHistoryBtn2Init(PG_MSGARG param) { m_pMatchHistoryBtn[1] = DYNAMIC_CAST(FrButton, param); }

// user_info.cpp:858
void FrUserInfoForm::OnMatchHistoryBtn3Init(PG_MSGARG param) { m_pMatchHistoryBtn[2] = DYNAMIC_CAST(FrButton, param); }

// user_info.cpp:859
void FrUserInfoForm::OnMatchHistoryBtn4Init(PG_MSGARG param) { m_pMatchHistoryBtn[3] = DYNAMIC_CAST(FrButton, param); }

// user_info.cpp:860
void FrUserInfoForm::OnMatchHistoryBtn5Init(PG_MSGARG param) { m_pMatchHistoryBtn[4] = DYNAMIC_CAST(FrButton, param); }

// user_info.cpp:862
void FrUserInfoForm::OnMatchHistoryBtn1Up(void) { MatchHistoryUserBtnUp(0); }
// user_info.cpp:863
void FrUserInfoForm::OnMatchHistoryBtn2Up(void) { MatchHistoryUserBtnUp(1); }
// user_info.cpp:864
void FrUserInfoForm::OnMatchHistoryBtn3Up(void) { MatchHistoryUserBtnUp(2); }
// user_info.cpp:865
void FrUserInfoForm::OnMatchHistoryBtn4Up(void) { MatchHistoryUserBtnUp(3); }
// user_info.cpp:866
void FrUserInfoForm::OnMatchHistoryBtn5Up(void) { MatchHistoryUserBtnUp(4); }

// user_info.cpp:881
void FrUserInfoForm::MatchHistoryUserBtnUp(int index) {
	WSendPacket packet(GCT_REQUEST_USERINFO);
	packet.Encode4(m_matchHistory[index].uid);
	packet.Encode1(5);
	packet.Send(SEND_TO_DEFAULT);

	m_bMatchHistoryRequested = true;
	m_pMatchHistoryBtn[index]->SetStatus(FrButton::NORMAL);
	CUserInfo::Instance()->m_nick = m_matchHistory[index].nickname;
}

// user_info.cpp:899
void FrUserInfoForm::MsnDisconExeception(float elapsed, FrListItem *pItem) {
	static int s_blinkCount;      // the notice's blinks so far
	static float s_blinkTime;     // time into the current blink

	if (!m_bMatchHistoryRequested)
		return;

	// The notice shows for the first 0.7 s of every second, three times.
	s_blinkTime += elapsed;
	if (s_blinkTime < 0.7f) {
		FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();
		if (pGDI == NULL || pItem == NULL)
			return;

		pGDI->SetTextColor(0xFFC0125C, 0xFFFFFFFF);
		pGDI->SetTextStyle(2);

		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 37.0f, pItem->pos.y + 212.0f), 0, "\xbc\xad\xb9\xf6\xc0\xc7\x20\xc0\xdb\xbe\xf7\xb7\xae\xc0\xcc\x20\xb8\xb9\xbd\xc0\xb4\xcf\xb4\xd9\x2e", -1.0f, 0xFFFFFFFF);
		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 37.0f, pItem->pos.y + 228.0f), 0, "\xc0\xe1\xbd\xc3\x20\xc8\xc4\x20\xb4\xd9\xbd\xc3\x20\xbd\xc3\xb5\xb5\xc7\xd8\x20\xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0\x2e", -1.0f, 0xFFFFFFFF);
	}

	if (s_blinkTime > 1.0f) {
		s_blinkTime = 0.0f;
		s_blinkCount++;

		if (s_blinkCount > 3) {
			s_blinkCount = 0;
			m_bMatchHistoryRequested = false;
		}
	}
}

// user_info.cpp:942
void FrUserInfoForm::SetMatchHistoryContents(void) {
	char text[128] = "";

	if (CMatchHistory::Instance()->IsUpdated() == true) {
		for (int i = 0; i < 5; i++) {
			if (m_matchHistory[i].uid != 0)
				m_pMatchHistoryEdit[i]->SetLine(1, m_matchHistory[i].nickname, 0xFF000000, false, 0);
			else
				m_pMatchHistoryEdit[i]->SetLine(1, "\xb1\xe2\xb7\xcf\x20\xbe\xf8\xc0\xbd", 0xFF000000, false, 0);
		}
	} else {
		const char *unavailable = "\xc8\xae\xc0\xce\x20\xba\xd2\xb0\xa1\x2e";
		strcpy(text, unavailable);
		for (int i = 0; i < 5; i++)
			m_pMatchHistoryEdit[i]->SetLine(1, text, 0xFF000000, false, 0);
	}
}

// user_info.cpp:974
void FrUserInfoForm::SetSpecListBoxItem(eSeasonType season) {
	if (strcmp(m_userInfo.m_name, WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().nickname) == 0) {
		if (m_pTabBtn[TAB_RECORD_INFO]->GetStatus() == FrButton::PRESSED) {
			if (m_pRecordClassicBtn) {
				m_pRecordClassicBtn->SetVisible(true);
			}
		}
	}
}

// user_info.cpp:1001
void FrUserInfoForm::OnSpecListboxOwnerDraw(PG_MSGARG param) {
	FrListItem *pItem = (FrListItem *)param;
	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI == NULL || pItem == NULL)
		return;

	WRect rect;
	rect.x = pItem->pos.x;
	rect.y = pItem->pos.y;

	WRect editRect;
	if (pItem->pData == m_pSpecBackBmp) {
		rect.w = (float)m_pSpecBackBmp->Width();
		rect.h = (float)m_pSpecBackBmp->Height();
		pGDI->DrawTexture(m_pSpecBackBmp, rect, 0xFFFFFFFF, 0);

		// gold, silver and bronze trophy counts
		int i;
		for (i = 0; i < 3; i++) {
			editRect = WRect(i * 60 + rect.x + 82.0f, rect.y + 17.0f, 40.0f, 20.0f);
			m_pSpecialInfoWnd[i]->SetRect(editRect);
			m_pSpecialInfoWnd[i]->SetVisible(true);
		}

		editRect = WRect(rect.x + 130.0f, rect.y + 38.0f, 100.0f, 20.0f);
		m_pMatchWinPercentEdit->SetRect(editRect);
		m_pMatchWinPercentEdit->SetVisible(true);

		// average score to OB rate
		for (i = 0; i < 6; i++) {
			editRect = WRect(rect.x + 130.0f, i * 20 + rect.y + 82.0f, 100.0f, 20.0f);
			m_pSpecialInfoWnd[4 + i]->SetRect(editRect);
			m_pSpecialInfoWnd[4 + i]->SetVisible(true);
		}

		editRect = WRect(rect.x + 130.0f, rect.y + 216.0f, 100.0f, 20.0f);
		m_pTotalSkinsPangEdit->SetRect(editRect);
		m_pTotalSkinsPangEdit->SetVisible(true);

		for (i = 0; i < 5; i++)
			m_pMatchHistoryEdit[i]->SetVisible(false);

		for (i = 0; i < 5; i++)
			m_pMatchHistoryBtn[i]->SetVisible(false);
	} else {
		rect.w = (float)m_pSpecBack00Bmp->Width();
		rect.h = (float)m_pSpecBack00Bmp->Height();
		pGDI->DrawTexture(m_pSpecBack00Bmp, rect, 0xFFFFFFFF, 0);

		int i;
		for (i = 0; i < 11; i++)
			m_pSpecialInfoWnd[i]->SetVisible(false);

		// the last games' players
		for (unsigned char slot = 0; slot < 5; slot++) {
			if (m_matchHistory[slot].uid != 0) {
				const Bitmap *pGenderBmp = m_pGenderBmp[m_matchHistory[slot].sex];

				pGDI->DrawTexture(pGenderBmp, WRect(rect.x + 14.0f, slot * 26 + rect.y + 45.0f, (float)pGenderBmp->Width(), (float)pGenderBmp->Height()), 0xFFFFFFFF, 0);

				m_pMatchHistoryBtn[slot]->SetVisible(true);

				pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
				pGDI->SetTextStyle(1);
				g_pFresh->GetManager()->PrintText(WPoint(rect.x + 51.0f, slot * 26 + rect.y + 51.0f), 0, m_matchHistory[slot].nickname, -1.0f, 0xFFFFFFFF);
				m_pMatchHistoryEdit[slot]->SetVisible(false);
			} else {
				m_pMatchHistoryBtn[slot]->SetVisible(false);

				editRect = WRect(rect.x + 42.0f, slot * 26 + rect.y + 46.0f, 124.0f, 20.0f);
				m_pMatchHistoryEdit[slot]->SetRect(editRect);
				m_pMatchHistoryEdit[slot]->SetVisible(true);
			}
		}

		MsnDisconExeception(m_lastElapsed, pItem);
	}
}

// user_info.cpp:1098
void FrUserInfoForm::UpdateEditUserLocation(void) {
	if (m_userInfo.m_slot != 0xFFFF) {
		std::list<sRoomInfo>::iterator it = std::find(WSingleton<CSharedDoc>::Instance()->m_roomList.begin(), WSingleton<CSharedDoc>::Instance()->m_roomList.end(), m_userInfo.m_slot);

		if (it == WSingleton<CSharedDoc>::Instance()->m_roomList.end()) {
			m_pConnStatusEdit->SetLine(1, "\xbe\xcb \xbc\xf6 \xbe\xf8\xc0\xbd", 0xFF000000, false, 0);
			return;
		}

		int state = 0;
		if (it->m_isPublic || AdminMode()) {
			if (!it->m_intrusionAccess
				|| (IsLocalContent((localContentType_t)0x2E)
				&& it->m_state))
				state = 1;
		} else {
			state = 2;
		}

		switch (state) {
		case 0:
			m_pConnStatusEdit->SetLine(1, MakeStr("[%d]\xb9\xf8 \xb9\xe6 %s", it->m_roomNumber, it->m_option[0] ? "\xc0\xe1\xbc\xf6\xc1\xdf" : "\xb4\xeb\xb1\xe2\xc1\xdf"), 0xFF000000, false, 0);
			break;
		case 1:
			m_pConnStatusEdit->SetLine(1, MakeStr("[%d]\xb9\xf8 \xb9\xe6 %s", it->m_roomNumber, (bool)((m_userInfo.m_identity >> 1) & 1) ? "\xb0\xfc\xc0\xfc\xc1\xdf" : "\xb0\xd4\xc0\xd3\xc1\xdf"), 0xFF000000, false, 0);
			break;
		case 2:
			m_pConnStatusEdit->SetLine(1, "\xba\xf1\xb0\xf8\xb0\xb3\xb9\xe6\xbf\xa1 \xc1\xa2\xbc\xd3\xc1\xdf", 0xFF000000, false, 0);
			break;
		}
	} else if (m_bMsnControl) {
		sFriend *pFriend = BuddyMGR->GetBuddy(m_userInfo.m_uid);

		if (pFriend) {
			if (pFriend->flags & 2) {
				char text[128] = "\0";
				bool bFound = false;
				std::list<sGameServerInfo>::iterator it;
				for (it = WSingleton<CSharedDoc>::Instance()->m_gameServerList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameServerList.end(); ++it) {
					if (it->id == pFriend->extra.id2) {
						wsprintf(text, "%s", it->name);
						bFound = true;
						break;
					}
				}

				if (!bFound) {
					m_pConnStatusEdit->SetLine(1, "\xc1\xa2\xbc\xd3\xc1\xdf", 0xFF000000, false, 0);
					return;
				}

				wsprintf(text, "%s %s", text, pFriend->extra.data);

				if (pFriend->extra.code != 0xFFFF) {
					wsprintf(text, "%s [%d]", text, pFriend->extra.code);
				}

				m_pConnStatusEdit->SetLine(1, text, 0xFF000000, false, 0);
			} else {
				m_pConnStatusEdit->SetLine(1, "\xc1\xa2\xbc\xd3\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xbd", 0xFF000000, false, 0);
			}
		} else {
			if (m_bFriendRequested)
				m_pConnStatusEdit->SetLine(1, "\xbe\xcb \xbc\xf6 \xbe\xf8\xc0\xbd", 0xFF000000, false, 0);
			else
				m_pConnStatusEdit->SetLine(1, "\xc4\xa3\xb1\xb8\xbe\xc6\xb4\xd4", 0xFF000000, false, 0);
		}
	} else {
		m_pConnStatusEdit->SetLine(1, "\xb4\xeb\xb1\xe2\xbd\xc7\xbf\xa1 \xc1\xa2\xbc\xd3\xc1\xdf", 0xFF000000, false, 0);
	}
}

// user_info.cpp:1216
void FrUserInfoForm::OnRecTotalListboxInit(PG_MSGARG param) {
	m_pRecTotalList = DYNAMIC_CAST(FrListBox, param);
	m_pRecTotalList->SetVisible(false);

	m_pRecTotalBack01Bmp = g_pFresh->RegisterBitmap("special_back01");
	m_pRecTotalBack02Bmp = g_pFresh->RegisterBitmap("special_back02");
	m_pRecTotalBack03Bmp = g_pFresh->RegisterBitmap("special_back03");
	m_pRecTotalBack02NdBmp = g_pFresh->RegisterBitmap("special_back02_nd");

	m_pRecTotalList->AddItem((void*)m_pRecTotalBack01Bmp);
	m_pRecTotalList->AddItem((void*)m_pRecTotalBack02Bmp);
	m_pRecTotalList->AddItem((void*)m_pRecTotalBack03Bmp);
}

// user_info.cpp:1231
void FrUserInfoForm::OnRecTotalListboxOwnerDraw(PG_MSGARG param) {
	FrListItem *pItem = (FrListItem *)param;
	if (pItem == NULL)
		return;

	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	WRect rect;
	rect.x = pItem->pos.x;
	rect.y = pItem->pos.y;

	WRect editRect;
	if (pItem->pData == m_pRecTotalBack01Bmp) {
		rect.w = (float)m_pRecTotalBack01Bmp->Width();
		rect.h = (float)m_pRecTotalBack01Bmp->Height();
		pGDI->DrawTexture(m_pRecTotalBack01Bmp, rect, 0xFFFFFFFF, 0);

		editRect = WRect(rect.x + 68.0f, rect.y + 22.0f, 160.0f, 20.0f);
		m_pRecordEdit[0]->SetRect(editRect);
		m_pRecordEdit[0]->SetVisible(true);

		editRect = WRect(rect.x + 68.0f, rect.y + 42.0f, 160.0f, 20.0f);
		m_pRecordEdit[1]->SetRect(editRect);
		m_pRecordEdit[1]->SetVisible(true);

		editRect = WRect(rect.x + 68.0f, rect.y + 62.0f, 160.0f, 20.0f);
		m_pRecordEdit[2]->SetRect(editRect);
		m_pRecordEdit[2]->SetVisible(true);

		editRect = WRect(rect.x + 121.0f, rect.y + 81.0f, 44.0f, 20.0f);
		m_pRecordEdit[3]->SetRect(editRect);
		m_pRecordEdit[3]->SetVisible(true);

		editRect = WRect(rect.x + 68.0f, rect.y + 81.0f, 160.0f, 20.0f);
		m_pRecordEdit[4]->SetRect(editRect);
		m_pRecordEdit[4]->SetVisible(true);

		m_pRecordEdit[12]->SetVisible(false);
		m_pRecordEdit[13]->SetVisible(false);
		m_pRecordEdit[14]->SetVisible(false);
	} else if (pItem->pData == m_pRecTotalBack02Bmp) {
		rect.w = (float)m_pRecTotalBack02Bmp->Width();
		rect.h = (float)m_pRecTotalBack02Bmp->Height();
		pGDI->DrawTexture(m_pRecTotalBack02Bmp, rect, 0xFFFFFFFF, 0);

		editRect = WRect(rect.x + 68.0f, rect.y + 14.0f, 160.0f, 20.0f);
		m_pRecordEdit[5]->SetRect(editRect);
		m_pRecordEdit[5]->SetVisible(true);

		editRect = WRect(rect.x + 68.0f, rect.y + 32.0f, 160.0f, 20.0f);
		m_pRecordEdit[6]->SetRect(editRect);
		m_pRecordEdit[6]->SetVisible(true);

		editRect = WRect(rect.x + 68.0f, rect.y + 50.0f, 160.0f, 20.0f);
		m_pRecordEdit[7]->SetRect(editRect);
		m_pRecordEdit[7]->SetVisible(true);

		editRect = WRect(rect.x + 68.0f, rect.y + 68.0f, 160.0f, 20.0f);
		m_pRecordEdit[8]->SetRect(editRect);
		m_pRecordEdit[8]->SetVisible(true);

		editRect = WRect(rect.x + 68.0f, rect.y + 86.0f, 160.0f, 20.0f);
		m_pRecordEdit[9]->SetRect(editRect);
		m_pRecordEdit[9]->SetVisible(true);

		// the last pair only while it fits in the form
		if (rect.y - m_rect.y < 250.0f) {
			editRect = WRect(rect.x + 38.0f, rect.y + 126.0f, 69.0f, 20.0f);
			m_pRecordEdit[10]->SetRect(editRect);
			m_pRecordEdit[10]->SetVisible(true);

			editRect = WRect(rect.x + 145.0f, rect.y + 126.0f, 69.0f, 20.0f);
			m_pRecordEdit[11]->SetRect(editRect);
			m_pRecordEdit[11]->SetVisible(true);
		} else {
			m_pRecordEdit[10]->SetVisible(false);
			m_pRecordEdit[11]->SetVisible(false);
		}
	} else if (pItem->pData == m_pRecTotalBack03Bmp) {
		rect.w = (float)m_pRecTotalBack03Bmp->Width();
		rect.h = (float)m_pRecTotalBack03Bmp->Height();
		pGDI->DrawTexture(m_pRecTotalBack03Bmp, rect, 0xFFFFFFFF, 0);

		editRect = WRect(rect.x + 68.0f, rect.y + 52.0f, 160.0f, 20.0f);
		m_pRecordEdit[12]->SetRect(editRect);
		m_pRecordEdit[12]->SetVisible(true);

		editRect = WRect(rect.x + 68.0f, rect.y + 74.0f, 160.0f, 20.0f);
		m_pRecordEdit[13]->SetRect(editRect);
		m_pRecordEdit[13]->SetVisible(true);

		editRect = WRect(rect.x + 68.0f, rect.y + 96.0f, 160.0f, 20.0f);
		m_pRecordEdit[14]->SetRect(editRect);
		m_pRecordEdit[14]->SetVisible(true);

		m_pRecordEdit[0]->SetVisible(false);
		m_pRecordEdit[1]->SetVisible(false);
		m_pRecordEdit[2]->SetVisible(false);
		m_pRecordEdit[3]->SetVisible(false);
		m_pRecordEdit[4]->SetVisible(false);
	}
}

// user_info.cpp:1391
void FrUserInfoForm::OnTrophyNormalNumInit(PG_MSGARG param) {
	m_pTrophyNormalNum = DYNAMIC_CAST(FrEdit, param);
	m_pTrophyNormalNum->SetVisible(false);
}

// user_info.cpp:1397
void FrUserInfoForm::OnTrophySpecialNumInit(PG_MSGARG param) {
	m_pTrophySpecialNum = DYNAMIC_CAST(FrEdit, param);
	m_pTrophySpecialNum->SetVisible(false);
}

// user_info.cpp:1403
void FrUserInfoForm::OnTrophyNormalBtnInit(PG_MSGARG param) {
	m_pTrophyNormalBtn = DYNAMIC_CAST(FrButton, param);

	if (m_trophyType == 0) {
		m_pTrophyNormalBtn->SetStatus(FrButton::PRESSED);
	} else {
		m_pTrophyNormalBtn->SetStatus(FrButton::NORMAL);
	}
}

// user_info.cpp:1413
void FrUserInfoForm::OnTrophyNormalBtnUp(void) {
	m_trophyType = 0;
	m_pTrophyNormalBtn->SetStatus(FrButton::PRESSED);
	m_pTrophySpecialBtn->SetStatus(FrButton::NORMAL);

	ChangeTrophyType();
}

// user_info.cpp:1423
void FrUserInfoForm::OnTrophySpecialBtnInit(PG_MSGARG param) {
	m_pTrophySpecialBtn = DYNAMIC_CAST(FrButton, param);

	if (m_trophyType == 1) {
		m_pTrophySpecialBtn->SetStatus(FrButton::PRESSED);
	} else {
		m_pTrophySpecialBtn->SetStatus(FrButton::NORMAL);
	}
}

// user_info.cpp:1433
void FrUserInfoForm::OnTrophySpecialBtnUp(void) {
	m_trophyType = 1;
	m_pTrophySpecialBtn->SetStatus(FrButton::PRESSED);
	m_pTrophyNormalBtn->SetStatus(FrButton::NORMAL);

	ChangeTrophyType();
}

// user_info.cpp:1443
void FrUserInfoForm::ChangeRecordType(void) {
	if (m_recordType == 0) {
		if (m_pRecCourseBack)
			m_pRecCourseBack->SetVisible(false);

		m_pRecTotalList->SetVisible(true);
		m_pScoreList->SetVisible(false);
	} else if (m_recordType == 2 || m_recordType == 3) {
		if (m_pRecCourseBack)
			m_pRecCourseBack->SetVisible(true);

		// Each loop is one source line indexing through a pointer to the array
		// (an indexed loop; indexing the member directly is strength-reduced).
		FrEdit **edits = m_pRecordEdit;
		for (int i = 0; i < 15; i++) {
			if (edits[i])
				edits[i]->SetVisible(false);
		}

		m_pRecTotalList->SetVisible(false);
		m_pScoreList->SetVisible(true);
	} else if (m_recordType == 1) {
		if (m_pRecCourseBack)
			m_pRecCourseBack->SetVisible(false);
		if (m_pRecAwardBack)
			m_pRecAwardBack->SetVisible(true);

		// Each loop is one source line indexing through a pointer to the array
		// (an indexed loop; indexing the member directly is strength-reduced).
		FrWnd **wnds = m_pRecAwardWnd;
		for (int i = 0; i < 6; i++) {
			if (wnds[i])
				wnds[i]->SetVisible(true);
		}
		FrEdit **edits = m_pRecordEdit;
		for (int i = 0; i < 15; i++) {
			if (edits[i])
				edits[i]->SetVisible(false);
		}

		m_pRecTotalList->SetVisible(false);
		m_pScoreList->SetVisible(false);
	}
}

// user_info.cpp:1492
void FrUserInfoForm::ChangeTrophyType(void) {
	if (m_trophyType == 1) {
		m_pTrophyNormalNum->SetVisible(false);
		m_pTrophySpecialNum->SetVisible(true);

		if (m_pTroNormalBack)
			m_pTroNormalBack->SetVisible(false);
		if (m_pTroSpecialBack)
			m_pTroSpecialBack->SetVisible(true);

		if (m_pTrophySpecialList) {
			m_pTrophySpecialList->SetVisible(true);

			if (m_pNoTrophyStatic)
				m_pNoTrophyStatic->SetVisible(m_pTrophySpecialList->m_itemList.size() == 0);
		}

		if (m_pTrophyNormalList)
			m_pTrophyNormalList->SetVisible(false);
	} else if (m_trophyType == 0) {
		m_pTrophyNormalNum->SetVisible(true);
		m_pTrophySpecialNum->SetVisible(false);

		if (m_pTroNormalBack)
			m_pTroNormalBack->SetVisible(true);
		if (m_pTroSpecialBack)
			m_pTroSpecialBack->SetVisible(false);

		if (m_pTrophySpecialList)
			m_pTrophySpecialList->SetVisible(false);

		if (m_pTrophyNormalList) {
			m_pTrophyNormalList->SetVisible(true);

			if (m_pNoTrophyStatic)
				m_pNoTrophyStatic->SetVisible(m_pTrophyNormalList->m_itemList.size() == 0);
		}
	}
}

// user_info.cpp:1538
void FrUserInfoForm::OnListBoxInit_TRI_SPECIALBOX(PG_MSGARG param) {
	m_pTrophySpecialList = DYNAMIC_CAST(FrListBox, param);
}

// user_info.cpp:1545
void FrUserInfoForm::OnListBoxOwnerDraw_TRI_SPECIALBOX(PG_MSGARG param) {
	FrListItem *pItem = (FrListItem *)param;
	if (pItem == NULL)
		return;

	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	sSpecialTrophy *pTrophy = (sSpecialTrophy *)pItem->pData;

	float itemHeight = (float)m_pTrophyNormalList->GetItemHeight();

	IFF_STRUCT::sMatch *pMatch = ItemManager()->FindMatch(pTrophy->m_typeId);
	if (pMatch == NULL)
		return;

	char szName[100];
	strcpy(szName, pMatch->m_name);
	char *pLine = szName;

	// the name is printed one line per '\n'
	int line = 0;
	char *pNewLine = strstr(pLine, "\n");
	while (pNewLine) {
		*pNewLine = '\0';
		pGDI->SetTextStyle(1);
		pGDI->Print(pItem->pos + WPoint(5.0f, line * 15.0f + 30.0f), 0, "%s", pLine);

		line++;
		pLine = pNewLine + 1;
		pNewLine = strstr(pLine, "\n");
	}

	pGDI->SetTextStyle(0);
	pGDI->Print(pItem->pos + WPoint(5.0f, line * 15.0f + 30.0f), 0, "%s", pLine);

	WRect rect;
	rect.y = pItem->pos.y + 3.0f;

	pGDI->SetTextStyle(0);

	const Bitmap *pBmp = g_pFresh->GetManager()->GetBitmap("TROPHY", pMatch->m_trophyIcon[0]);

	rect.x = pItem->pos.x + 185.0f;

	rect.w = (float)pBmp->Width();
	rect.h = (float)pBmp->Height();
	pGDI->DrawTexture(pBmp, rect, pTrophy->m_count ? 0xFFFFFFFF : 0x60000000, 0);

	if (pTrophy->m_count > 1) {
		pGDI->Print(WPoint(float2int(rect.w * 0.5f) + rect.x, itemHeight + pItem->pos.y - 15.0f), 1, "%d", pTrophy->m_count);
	}
}

// user_info.cpp:1609
void FrUserInfoForm::OnListBoxInit_TRI_NORMALBOX(PG_MSGARG param) {
	m_pTrophyNormalList = DYNAMIC_CAST(FrListBox, param);
}

// user_info.cpp:1616
void FrUserInfoForm::OnListBoxOwnerDraw_TRI_NORMALBOX(PG_MSGARG param) {
	FrListItem *pItem = (FrListItem *)param;
	if (pItem == NULL)
		return;

	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	intptr_t course = (intptr_t)pItem->pData;

	// the list's items are the course indices
	if (course < 0 || course >= 13)
		return;

	unsigned short *pMedals;
	switch (m_seasonTab) {
	case SEASON_CURRENT:
		pMedals = m_userInfo.m_trophy.m_medals[course];
		break;
	case SEASON_TOTAL:
		pMedals = m_totalRecord.m_trophy.m_medals[course];
		break;
	default:
		pMedals = m_userInfo.m_trophy.m_medals[course];
		break;
	}

	float itemHeight = (float)m_pTrophyNormalList->GetItemHeight();
	IFF_STRUCT::sMatch *pMatch = ItemManager()->FindMatch((0x2C00 | course) << 16);

	pMatch = ItemManager()->FindMatch((0x2C00 | course) << 16);
	if (pMatch == NULL)
		return;

	pGDI->SetTextStyle(1);
	pGDI->Print(WPoint(pItem->pos.x + 20.0f, pItem->pos.y + 38.0f), 0, "%s", pMatch->m_name);

	WRect rect;
	rect.y = pItem->pos.y + 3.0f;
	pGDI->SetTextStyle(0);

	const Bitmap *pBmps[3];
	for (int i = 0; i < 3; i++) {
		pBmps[i] = g_pFresh->GetManager()->GetBitmap("TROPHY", pMatch->m_trophyIcon[i]);

		rect.x = i * 60.0f + pItem->pos.x + 75.0f;

		if (pBmps[i]) {
			rect.w = (float)pBmps[i]->Width();
			rect.h = (float)pBmps[i]->Height();
			pGDI->DrawTexture(pBmps[i], rect, pMedals[i] ? 0xFFFFFFFF : 0x60000000, 0);

			pGDI->Print(WPoint(float2int(rect.w * 0.5f) + rect.x, itemHeight + pItem->pos.y - 15.0f), 1, "%d", pMedals[i]);
		}
	}
}

// user_info.cpp:1680
void FrUserInfoForm::OnStaticInit_TRI_NOTROPHY(PG_MSGARG param) {
	m_pNoTrophyStatic = DYNAMIC_CAST(FrStatic, param);
	if (m_pNoTrophyStatic) {
		m_pNoTrophyStatic->SetVisible(false);
	}
}

// user_info.cpp:1703
void FrUserInfoForm::OnMyGuildInit(PG_MSGARG param) {
	m_pMyGuildArea = DYNAMIC_CAST(FrArea, param);
	m_pGuildTrophyBmp = g_pFresh->GetManager()->GetBitmap("TROPHY", "guild");
}

// user_info.cpp:1720
void FrUserInfoForm::OnMyGuildOwnerDraw(void) {
	if (m_userInfo.m_guildUID == 0 || (m_userInfo.m_identity & 0x14))
		return;

	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	WRect rect = m_pMyGuildArea->GetRect();

	if (m_pGuildTrophyBmp) {
		pGDI->DrawTexture(m_pGuildTrophyBmp, WRect(rect.x, rect.y, (float)m_pGuildTrophyBmp->Width(), (float)m_pGuildTrophyBmp->Height()), 0xFFFFFFFF, 0);
	}

	const Bitmap *pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(m_userInfo.m_guildMark);

	if (pEmblem == NULL)
		return;

	pGDI->DrawTexture(pEmblem, WRect(rect.x + 21.0f, rect.y + 12.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
}

// user_info.cpp:1751
void FrUserInfoForm::OnGuildTrophyListInit(PG_MSGARG param) {
	m_pGuildTrophyList = DYNAMIC_CAST(FrListBox, param);
	if (m_pGuildTrophyList) {
		m_pGuildTrophyList->SetVisible(false);
	}
}

// user_info.cpp:1762
void FrUserInfoForm::OnGuildTrophyListOwnerDraw(PG_MSGARG param) {
	FrListItem *pItem = (FrListItem *)param;
	if (pItem == NULL)
		return;

	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	sGuildTrophy *pTrophy = (sGuildTrophy *)pItem->pData;
	if (pTrophy == NULL)
		return;

	IFF_STRUCT::sMatch *pMatch = ItemManager()->FindMatch(pTrophy->m_typeId);
	if (pMatch == NULL)
		return;

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	char szName[100];
	strcpy(szName, pMatch->m_name);
	char *pLine = szName;

	// the name is printed one line per '\n'
	int line = 0;
	char *pNewLine = strstr(pLine, "\n");
	while (pNewLine) {
		*pNewLine = '\0';
		pGDI->SetTextStyle(1);
		pGDI->Print(pItem->pos + WPoint(18.0f, line * 15.0f + 30.0f), 0, "%s", pLine);

		line++;
		pLine = pNewLine + 1;
		pNewLine = strstr(pLine, "\n");
	}

	pGDI->SetTextStyle(0);
	pGDI->Print(pItem->pos + WPoint(18.0f, line * 15.0f + 30.0f), 0, "%s", pLine);

	WRect rect;
	rect.y = pItem->pos.y + 3.0f;

	pGDI->SetTextStyle(0);

	const Bitmap *pBmp = g_pFresh->GetManager()->GetBitmap("TROPHY", pMatch->m_trophyIcon[0]);

	rect.x = pItem->pos.x + 130.0f;

	rect.w = (float)pBmp->Width();
	rect.h = (float)pBmp->Height();
	pGDI->DrawTexture(pBmp, rect, 0xFFFFFFFF, 0);
}

// user_info.cpp:1830
void FrUserInfoForm::OnStaticInit_GUILD_NOTROPHY(PG_MSGARG param) {
	m_pGuildNoTrophyStatic = DYNAMIC_CAST(FrStatic, param);
	if (m_pGuildNoTrophyStatic) {
		m_pGuildNoTrophyStatic->SetVisible(false);
	}
}

// user_info.cpp:1842
void FrUserInfoForm::OnWhisperInit(PG_MSGARG param) {
	m_pWhisperBtn = DYNAMIC_CAST(FrButton, param);
}

// user_info.cpp:1847
void FrUserInfoForm::OnNickInit(intptr_t param) {
	m_pNickArea = DYNAMIC_CAST(FrArea, param);
	m_pGenderBmp[0] = g_pFresh->RegisterBitmap("i_male");
	m_pGenderBmp[1] = g_pFresh->RegisterBitmap("i_female");
}

// user_info.cpp:1865
void FrUserInfoForm::OnNickOwnerDraw(PG_MSGARG param) {
	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	if (m_userInfo.m_guid == 0xFFFFFFFF)
		return;

	const Bitmap *pBmp = m_pGenderBmp[m_userInfo.m_eventFlags.gender];
	pGDI->DrawTexture(pBmp, WRect(m_pNickArea->GetRect().x, m_pNickArea->GetRect().y, (float)pBmp->Width(), (float)pBmp->Height()), 0xFFFFFFFF, 0);

	if (WSingleton<CProjectG>::Instance()->HidePrivacy())
		return;

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	pGDI->SetTextStyle(1);
	g_pFresh->GetManager()->PrintText(WPoint(m_pNickArea->GetRect().x + 27.0f, m_pNickArea->GetRect().y + 8.0f), 0, m_userInfo.m_name, -1.0f, 0xFFFFFFFF);
}

// user_info.cpp:1886
void FrUserInfoForm::OnWhisperUp(void) {
	if (m_userInfo.m_uid == 0xFFFFFFFF || m_userInfo.m_uid == WSingleton<CSharedDoc>::Instance()->m_myUID)
		return;

	if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject*)__rtti_obj)->IsKindOf(&CGolfTask::m_RTTI) : false)
		AfxGetTask()->GetActor("GolfRule") << MsgObject(0, 0x37, (intptr_t)m_userInfo.m_name, 0, 0, 0, 0);
	else if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject*)__rtti_obj)->IsKindOf(&CLobbyTask::m_RTTI) : false)
		AfxGetTask()->GetActor("Lobby") << MsgObject(0, 0x37, (intptr_t)m_userInfo.m_name, 0, 0, 0, 0);
	else if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject*)__rtti_obj)->IsKindOf(&ntAvatarChatTask::m_RTTI) : false)
		AfxGetTask()->GetActor("AvatarChat") << MsgObject(0, 0x37, (intptr_t)m_userInfo.m_name, 0, 0, 0, 0);
}

// user_info.cpp:1934
void FrUserInfoForm::OnFriendInit(PG_MSGARG param) {
	m_pFriendBtn = DYNAMIC_CAST(FrButton, param);
}

// user_info.cpp:1939
void FrUserInfoForm::OnFriendUp(void) {
	if (((__rtti_obj = AfxGetTask()) != 0 ? ((IObject*)__rtti_obj)->IsKindOf(&CGolfTask::m_RTTI) : false)
		|| ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject*)__rtti_obj)->IsKindOf(&ntAvatarChatTask::m_RTTI) : false)) {
		m_bChatActive = WSingleton<CChatMsg>::Instance()->IsActive();
	}

	if (m_addFriendDlg == NULL) {
		m_addFriendDlg = CreateForm<FrAddFriendDlg>(g_pFresh->GetManager(), this, "addfriend", 0);
		m_addFriendDlg->CheckRequestNickname(m_userInfo.m_name);
		m_addFriendDlg->Open((FRESH_RESULT_CALLBACK)&FrUserInfoForm::OnAddFriendFormResult, 1);
	}
}

// user_info.cpp:1954
void FrUserInfoForm::OnIgnoreInit(intptr_t param) {
	m_pIgnoreBtn = DYNAMIC_CAST(FrButton, param);
}

// user_info.cpp:1959
void FrUserInfoForm::OnIgnoreUp(void) {
	if (m_userInfo.m_uid == 0xFFFFFFFF || m_userInfo.m_uid == MyUID())
		return;

	bool bIgnored = false;
	std::list<std::string>::iterator it = std::find(WSingleton<CSharedDoc>::Instance()->m_ignoreList.begin(), WSingleton<CSharedDoc>::Instance()->m_ignoreList.end(), m_userInfo.m_name);
	if (it == WSingleton<CSharedDoc>::Instance()->m_ignoreList.end()) {
		WSingleton<CSharedDoc>::Instance()->m_ignoreList.push_back(m_userInfo.m_name);
		bIgnored = true;
	} else {
		WSingleton<CSharedDoc>::Instance()->m_ignoreList.erase(it);
	}

	SetIgnoreButtonImg(bIgnored);
}

// user_info.cpp:1997
void FrUserInfoForm::SetIgnoreButtonImg(bool bIgnored) {
	if (bIgnored) {
		m_pIgnoreBtn->SetButtonImg("btn_i02_02_n", FrButton::NORMAL);
		m_pIgnoreBtn->SetButtonImg("btn_i02_02_o", FrButton::OVER);
	} else {
		m_pIgnoreBtn->SetButtonImg("btn_i02_n", FrButton::NORMAL);
		m_pIgnoreBtn->SetButtonImg("btn_i02_o", FrButton::OVER);
	}
	if (bIgnored) m_pIgnoreBtn->SetButtonImg("btn_i02_02_d", FrButton::PRESSED); else m_pIgnoreBtn->SetButtonImg("btn_i02_d", FrButton::PRESSED);
}

// user_info.cpp:2015
void FrUserInfoForm::OnCloseUp(void) {
	Close(FrNONE, true);
}

// user_info.cpp:2020
bool FrUserInfoForm::Close(eFormRet ret, bool bDestroy) {
	if (m_anotherHand.IsWorking(0))
		return false;

	return FrForm::Close(ret, bDestroy);
}

// user_info.cpp:2028
void FrUserInfoForm::OnProc(const float dt) {
	if (IsLocalContent((localContentType_t)0x57)) {
		RefreshAllUccClothes();
	}

	m_lastElapsed = dt;
	if (m_petLoadTime < 3.0f) {
		m_petLoadTime += dt;

		if (m_petLoadTime >= 3.0f || !m_anotherHand.IsWorking(0)) {
			m_pPetLoadGauge->SetVisible(false);
			m_petLoadTime = 3.0f;
		}
	}
}

// user_info.cpp:2048
void FrUserInfoForm::OnAnotherHand(int nWork, void *pData) {
	if (nWork == 0) {
		ShowPet();
	}
}

// user_info.cpp:2071
void FrUserInfoForm::RefreshAllUccClothes(void) {
	if (m_pPetTids && m_pPetExhibition && m_pPetExhibition->GetPetFrame()) {
		sUccRefreshItem item;

		for (int i = 0; i < 10; i++) {
			if (!UccManager()->GetRefreshItem(item)) {
				break;
			}

			unsigned long typeId = item.m_typeId;

			unsigned char slot = (unsigned char)((typeId >> 13) & 0x1F);
			if (m_pPetTids->m_partTids[slot] != typeId) {
				continue;
			}
			unsigned long serial = m_pPetTids->m_partIds[slot];

			sUccClothes *pClothes = UccManager()->FindClothes(serial);

			if (pClothes != 0 && strcmpi(pClothes->m_uccKey, item.m_uccKey) == 0) {
				if (m_pPetExhibition && m_pPetExhibition->GetPetFrame()) {
					m_pPetExhibition->GetPetFrame()->SetPart(m_pPetTids, pClothes->m_typeId, pClothes->m_serial);
					m_pPetExhibition->GetPetFrame()->SetPart(m_pPetTids, pClothes->m_typeId, pClothes->m_serial);
				}
			}
		}
	}
}

// user_info.cpp:2128
bool FrUserInfoForm::OnInit(void) {
	m_disconnEditRect = m_pDisconnEdit->GetRect();

	// Every page starts hidden.
	m_curTab = TAB_NONE;
	SetVisible(TAB_GENERAL_INFO, false);
	SetVisible(TAB_SPECIAL_INFO, false);
	SetVisible(TAB_RECORD_INFO, false);
	SetVisible(TAB_TROPHY_INFO, false);
	SetVisible(TAB_GUILD_INFO, false);

	m_petLoadTime = 0.0f;
	m_pIdEdit->HidePrivacy(true);

	m_pIdEdit->DisableEditStyle(FrEdit::ES_EMOTICON);

	return true;
}

// user_info.cpp:2158
void FrUserInfoForm::SetFormA(eTabType tab) {
	if (m_curTab != tab) {
		SetVisible(m_curTab, false);
		m_curTab = tab;
		SetVisible(tab, true);

		for (int i = 0; i < 5; i++) {
			if (i == m_curTab) {
				if (m_pTabBtn[i]) {
					m_pTabBtn[i]->SetStatus(FrButton::PRESSED);
				}
			} else {
				if (m_pTabBtn[i]) {
					m_pTabBtn[i]->SetStatus(FrButton::NORMAL);
				}
			}
		}
	}
}

// user_info.cpp:2182
void FrUserInfoForm::HidePet(bool bWait) {
	// Both branches free the pet; the compiler merges them (only the second
	// branch's lines survive), which keeps 0 in a register for the whole
	// function (docs/tough_problems/024).
	if (bWait) {
		m_anotherHand.WaitUntilWorkDone(9999);
		if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = NULL; }
		if (m_pPetTids) { delete m_pPetTids; m_pPetTids = NULL; }
	} else {
		if (m_pPetExhibition) { delete m_pPetExhibition; m_pPetExhibition = NULL; }
		if (m_pPetTids) { delete m_pPetTids; m_pPetTids = NULL; }
	}
}

// user_info.cpp:2198
void FrUserInfoForm::SetInfo(sUserInfoTime *pInfo) {
	int i;

	m_friendExtra.id2 = pInfo->m_friendExtra.id2;
	wsprintf(m_friendExtra.data, "%s", pInfo->m_friendExtra.data);

	m_friendExtra.code = pInfo->m_friendExtra.code;

	m_userInfo = pInfo->m_userInfo;

	if (IsLocalContent((localContentType_t)0x28)) {
		if (strcmpi(m_userInfo.m_name, MyNick()) == 0) {
			WSingleton<CMatchHistory>::Instance()->GetMatchHistory(m_matchHistory);
			if (WSingleton<CMatchHistory>::Instance()->IsUpdated() == false) {
				WSingleton<CMatchHistory>::Instance()->RequestHistory();
			}

			FrListItem *pItem = m_pSpecList->FindItem((void *)m_pSpecBack00Bmp);
			if (pItem == NULL)
				m_pSpecList->AddItem((void *)m_pSpecBack00Bmp);
		} else {
			FrListItem *pItem = m_pSpecList->FindItem((void *)m_pSpecBack00Bmp);
			if (pItem != NULL)
				m_pSpecList->DelItem((void *)m_pSpecBack00Bmp);
		}
	} else {
		m_pSpecList->DelItem((void *)m_pSpecBack00Bmp);
	}

	m_specialTrophies.clear();
	for (i = 0; i < pInfo->m_specialTrophyCount; i++)
		m_specialTrophies.push_back(pInfo->m_pArray1[i]);

	m_guildTrophies.clear();
	for (i = 0; i < pInfo->m_guildTrophyCount; i++)
		m_guildTrophies.push_back(pInfo->m_pArray2[i]);

	m_guildPang = pInfo->m_guildPang;
	m_guildMyPercent = pInfo->m_guildMyPercent;

	memcpy(m_classicMapStats, pInfo->m_userInfo.m_mapStatistics2, sizeof(m_classicMapStats)); memcpy(m_seasonClassicMapStats, pInfo->m_classicMapStatistics, sizeof(m_seasonClassicMapStats));

	m_seasonRecord.m_statistics = pInfo->m_seasonStatistics;
	for (i = 0; i < 20; i++)
		m_seasonRecord.m_mapStats[i] = pInfo->m_mapStatistics[i];

	m_seasonRecord.m_trophy = pInfo->m_trophyStatistics;

	m_seasonRecord.m_specialTrophies.clear();
	for (i = 0; i < pInfo->m_seasonSpecialTrophyCount; i++)
		m_seasonRecord.m_specialTrophies.push_back(pInfo->m_pArray3[i]);

	m_seasonRecord.m_guildTrophies.clear();
	for (i = 0; i < pInfo->m_seasonGuildTrophyCount; i++)
		m_seasonRecord.m_guildTrophies.push_back(pInfo->m_pArray4[i]);

	if (m_userInfo.m_uid != 0xFFFFFFFF) {
		if (m_pFriendBtn) {
			if (m_userInfo.m_uid == WSingleton<CSharedDoc>::Instance()->m_myUID)
				m_pFriendBtn->Enable(false);
			else
				m_pFriendBtn->Enable(m_bControlOff == false);
		}

		if (m_pWhisperBtn)
			m_pWhisperBtn->Enable(m_userInfo.m_uid != WSingleton<CSharedDoc>::Instance()->m_myUID && m_bControlOff == false);

		if (m_pIgnoreBtn) {
			m_pIgnoreBtn->Enable(m_userInfo.m_uid != WSingleton<CSharedDoc>::Instance()->m_myUID && m_bControlOff == false);

			bool bIgnored = false;
			std::list<std::string>::iterator it = std::find(WSingleton<CSharedDoc>::Instance()->m_ignoreList.begin(), WSingleton<CSharedDoc>::Instance()->m_ignoreList.end(), m_userInfo.m_name);
			if (it != WSingleton<CSharedDoc>::Instance()->m_ignoreList.end())
				bIgnored = true;

			SetIgnoreButtonImg(bIgnored);
		}

		if (m_userInfo.m_skinTypeIds[0]) {
			IFF_STRUCT::sSkin *pSkin = ItemManager()->FindSkin(m_userInfo.m_skinTypeIds[0]);

			if (pSkin) {
				m_skinScrollState[0] = pSkin->m_sideSlideMode[0];
				m_skinScrollState[1] = pSkin->m_sideSlideMode[1];
			}

			m_skinScroll[0] = 0.0f;
		}
	}

	SetFormA(TAB_GENERAL_INFO);
	BuildTotalStat();
	SetContent(false);
	m_anotherHand.Work(this, 0, NULL, true, -1);
	m_bOption = false;

	if ((WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80) && m_userInfo.m_uid != WSingleton<CSharedDoc>::Instance()->m_myUID) {
		m_pTabBtn[TAB_SPECIAL_INFO]->Enable(false);
		m_pTabBtn[TAB_RECORD_INFO]->Enable(false);
		m_pTabBtn[TAB_TROPHY_INFO]->Enable(false);
	}
}

// user_info.cpp:2351
bool ScoreListCompare(const void *a, const void *b) {
	const sMapStatistics *pA = (const sMapStatistics *)a;
	const sMapStatistics *pB = (const sMapStatistics *)b;

	return pA->m_bestScore < pB->m_bestScore;
}

// user_info.cpp:2382
void FrUserInfoForm::SelectContents(eSeasonType season, sPangYaUserStatistics **ppStatistics,
		sTrophyStatistics **ppTrophy, std::list<sSpecialTrophy> **ppSpecialTrophies,
		std::list<sGuildTrophy> **ppGuildTrophies) {
	switch (season) {
	default:
		*ppStatistics = &m_userInfo.m_statistics;
		*ppTrophy = &m_userInfo.m_trophy;
		*ppSpecialTrophies = &m_specialTrophies;
		*ppGuildTrophies = &m_guildTrophies;
		break;

	case SEASON_TOTAL:
		*ppStatistics = &m_totalRecord.m_statistics;
		*ppTrophy = &m_totalRecord.m_trophy;
		*ppSpecialTrophies = &m_totalRecord.m_specialTrophies;
		*ppGuildTrophies = &m_totalRecord.m_guildTrophies;
		break;
	}
}

// user_info.cpp:2412
unsigned long FrUserInfoForm::GetSeasonTextColor(eSeasonType season) {
	return 0xFF000000;
}

// user_info.cpp:2421
void FrUserInfoForm::UpdateEditSchoolAndGuild(eSeasonType season, const sPangYaUserInfo *pInfo) {
	char buf[128] = {0};

	unsigned long schoolNo = (m_userInfo.m_uid == MyUID()) ? WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().schoolNo : pInfo->schoolNo;
	WSingleton<CSharedDoc>::Instance()->LoadSchoolName(buf, schoolNo);

	if (buf[0] != '\0')
		m_pSchoolEdit->SetLine(1, buf, 0xFF000000, false, 0);
	else
		m_pSchoolEdit->SetLine(1, "\xbc\xd2\xbc\xd3 \xc7\xd0\xb1\xb3 \xbe\xf8\xc0\xbd", 0xFF000000, false, 0);   // "no school"

	if ((bool)((m_userInfo.m_identity >> 2) & 1)) {
		m_pClubEdit->SetLine(1, "\xbe\xcb \xbc\xf6 \xbe\xf8\xc0\xbd", 0xFF000000, false, 0);   // "unknown"
	} else {
		sprintf(buf, "%s", strlen(pInfo->guildName) ? pInfo->guildName : "\xbc\xd2\xbc\xd3 \xb1\xe6\xb5\xe5 \xbe\xf8\xc0\xbd", 0xFF000000);   // "no guild"
		m_pClubEdit->SetLine(1, buf, 0xFF000000, false, 0);
	}
}

// user_info.cpp:2448
void FrUserInfoForm::UpdateLevelTextImg(unsigned char level) {
	const Bitmap* pBitmap = g_pFresh->GetBitmap(MakeStr("level_%03d", (level + 1 < 71) ? level + 1 : 71));
	if (pBitmap) {
		m_pLevelArea->SetBgImg(pBitmap);
	}
}

// user_info.cpp:2465
bool IsNothingRecord(const sPangYaUserStatistics *pStatistics) {
	if (!pStatistics) return false;
	return (pStatistics->level == 0 || pStatistics->gamesPlayed == 0) ? 1 : 0;
}

// user_info.cpp:2475
bool IsMyRecord(const sUserInfo *pInfo) {
	if (!pInfo) return false;
	return pInfo->m_uid == MyUID() || AdminMode();
}

// user_info.cpp:2485
bool IsHideDisconnectRate(const sPangYaUserStatistics *pStatistics) {
	return (pStatistics->gamesPlayed <= 50 || pStatistics->level < 6) ? 1 : 0;
}

// user_info.cpp:2493
void FrUserInfoForm::UpdateEditExpAndMannerInfo(eSeasonType season, const sPangYaUserStatistics *pStatistics) {
	char buf[128] = {0};

	if ((WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80) && m_userInfo.m_uid != WSingleton<CSharedDoc>::Instance()->m_myUID) {
		sprintf(buf, "???");
	} else if (pStatistics->level < 70) {
		sprintf(buf, "%d / %d", pStatistics->exp, WSingleton<CSharedDoc>::Instance()->m_levelExp[pStatistics->level]);
	} else {
		sprintf(buf, "MAX");
	}

	m_pExpEdit->SetLine(1, buf, 0xFF000000, false, 0);

	if (IsNothingRecord(pStatistics)) {
		SetDisconnQuestionMark(true);
		sprintf(buf, "\xb1\xe2\xb7\xcf \xbe\xf8\xc0\xbd");   // "no record"
	} else {
		SetDisconnQuestionMark(false);
		if (IsMyRecord(&m_userInfo)) {
			if (IsHideDisconnectRate(pStatistics)) {
				SetDisconnQuestionMark(true);
				sprintf(buf, "- - -  [%d / %d]", pStatistics->noMannerGames, pStatistics->gamesPlayed);
			} else {
				sprintf(buf, "%.1f%% [%d / %d]", (float)pStatistics->noMannerGames * 100.0f / (float)pStatistics->gamesPlayed, pStatistics->noMannerGames, pStatistics->gamesPlayed);
			}
		} else {
			if (IsHideDisconnectRate(pStatistics)) {
				SetDisconnQuestionMark(true);
				sprintf(buf, "- - -");
			} else {
				sprintf(buf, "%.1f %%", (float)pStatistics->noMannerGames * 100.0f / (float)pStatistics->gamesPlayed);
			}
		}
	}

	m_pDisconnEdit->SetLine(1, buf, 0xFF000000, false, 0);

	sprintf(buf, "%d / %d", pStatistics->combos, pStatistics->totalCombos);
	m_pComboEdit->SetLine(1, buf, 0xFF000000, false, 0);
}

// user_info.cpp:2569
void FrUserInfoForm::UpdateEditSpecialInfo(eSeasonType season, const sPangYaUserStatistics *pStatistics, const sTrophyStatistics *pTrophy) {
	char buf[128] = {0};
	const char *noRecord = "\xb1\xe2\xb7\xcf \xbe\xf8\xc0\xbd";   // "no record"

	// the gold, silver and bronze medals over all courses
	int medals[3] = {0, 0, 0};
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 13; j++) medals[i] += pTrophy->m_medals[j][i];
	}

	sprintf(buf, "%d ", medals[0]);
	m_pTrophyGoldEdit->SetLine(1, buf, 0xFF000000, false, 0);
	sprintf(buf, "%d ", medals[1]);
	m_pTrophySilverEdit->SetLine(1, buf, 0xFF000000, false, 0);
	sprintf(buf, "%d ", medals[2]);
	m_pTrophyBronzeEdit->SetLine(1, buf, 0xFF000000, false, 0);

	if (pStatistics->rateTotal == 0)
		strcpy(buf, noRecord);
	else
		sprintf(buf, "%.1f %%", (float)pStatistics->rateCount / (float)pStatistics->rateTotal * 100.0f);
	m_pMatchWinPercentEdit->SetLine(1, buf, 0xFF000000, false, 0);

	if (pStatistics->drives - pStatistics->par3Drives == 0) {
		strcpy(buf, noRecord);
	} else {
		if (IsLocalCountry(0x1E)) {
			float avg = 72.0f + pStatistics->scoreToPar * 18.0f / (float)(pStatistics->drives - pStatistics->par3Drives);
			sprintf(buf, "%.2f \xc5\xb8", 100.0f < avg ? 100.0f : avg);
		} else {
			sprintf(buf, "%.2f \xc5\xb8", 72.0f + pStatistics->scoreToPar * 18.0f / (float)(pStatistics->drives - pStatistics->par3Drives));
		}
	}
	m_pAvgScoreEdit->SetLine(1, buf, 0xFF000000, false, 0);

	if (pStatistics->shots + pStatistics->puttAttempts == 0)
		strcpy(buf, noRecord);
	else
		sprintf(buf, "%.1f \xc3\xca", (float)pStatistics->playTime / (float)(pStatistics->shots + pStatistics->puttAttempts));
	m_pMeanShotTimeEdit->SetLine(1, buf, 0xFF000000, false, 0);

	if (pStatistics->shots == 0)
		strcpy(buf, noRecord);
	else
		sprintf(buf, "%.1f %%", (float)pStatistics->pangyas / (float)pStatistics->shots * 100.0f);
	m_pPangyaPercentEdit->SetLine(1, buf, 0xFF000000, false, 0);

	if (pStatistics->drives - pStatistics->par3Drives == 0)
		strcpy(buf, noRecord);
	else
		sprintf(buf, "%.1f %%", (float)pStatistics->fairways / (float)(pStatistics->drives - pStatistics->par3Drives) * 100.0f);
	m_pFairwayPercentEdit->SetLine(1, buf, 0xFF000000, false, 0);

	if (pStatistics->puttAttempts == 0)
		strcpy(buf, noRecord);
	else
		sprintf(buf, "%.1f %%", (float)pStatistics->puttsSunk / (float)pStatistics->puttAttempts * 100.0f);
	m_pPuttPercentEdit->SetLine(1, buf, 0xFF000000, false, 0);

	if (pStatistics->shots + pStatistics->puttAttempts == 0)
		strcpy(buf, noRecord);
	else
		sprintf(buf, "%.1f %%", (float)pStatistics->obCount / (float)(pStatistics->shots + pStatistics->puttAttempts) * 100.0f);
	m_pObPercentEdit->SetLine(1, buf, 0xFF000000, false, 0);

	if (pStatistics->totalSkinsPang == 0)
		strcpy(buf, noRecord);
	else
		sprintf(buf, "%I64d", pStatistics->totalSkinsPang);
	m_pTotalSkinsPangEdit->SetLine(1, buf, 0xFF000000, false, 0);
}

// user_info.cpp:2652
void FrUserInfoForm::UpdateEditMapStatistics(eSeasonType season, bool bClassic) {
	sMapStatistics *pStats;
	switch (season) {
	case SEASON_CURRENT:
		pStats = bClassic ? m_classicMapStats : m_userInfo.m_mapStatistics;
		break;

	case SEASON_TOTAL:
		pStats = bClassic ? m_totalClassicMapStats : m_totalRecord.m_mapStats;
		break;

	default:
		pStats = bClassic ? m_classicMapStats : m_userInfo.m_mapStatistics;
		break;
	}

	if (pStats == NULL)
		return;

	if (m_pScoreList == NULL)
		return;

	m_pScoreList->ClearItem();

	for (int i = 0; i < 20; i++, pStats++) {
		if (pStats->m_course == 0x11)
			continue;

		IFF_STRUCT::sCourse *pCourse = ItemManager()->FindCourse(0x28000000
				| ((pStats->m_course > 0x7F && pStats->m_course != 0xFD) ? pStats->m_course - 0x80 : pStats->m_course));
		if (pCourse == NULL || pCourse->m_active == 0)
			continue;

		m_pScoreList->AddItem(pStats);
	}

	m_pScoreList->SortItem(ScoreListCompare);
}

// user_info.cpp:2701
void FrUserInfoForm::SetDisconntextAlign(void) {
	if (m_pDisconnQuestionMark == NULL)
		return;

	WRect rect;
	rect = m_pDisconnEdit->GetRect();

	if (m_pDisconnQuestionMark->IsVisible()) {
		if (rect.x == m_disconnEditRect.x) {
			rect.x -= m_pDisconnQuestionMark->GetRect().w;
			m_pDisconnEdit->SetRect(rect);
		}
	} else {
		if (rect.x != m_disconnEditRect.x) {
			rect.x += m_pDisconnQuestionMark->GetRect().w;
			m_pDisconnEdit->SetRect(rect);
		}
	}
}

// user_info.cpp:2726
void FrUserInfoForm::SetDisconnQuestionMark(bool bVisible) {
	if (m_pDisconnQuestionMark) {
		m_pDisconnQuestionMark->SetVisible(bVisible);
		SetDisconntextAlign();
	}
}

// user_info.cpp:2736
void SortMapStat(sMapStatistics *ppBest[], sMapStatistics *pStats) {
	for (int i = 0; i < 20; i++) {
		if (i == 0x11)
			continue;

		sMapStatistics *pStat = &pStats[i];
		IFF_STRUCT::sCourse *pCourse = ItemManager()->FindCourse(0x28000000
				| ((pStat->m_course > 0x7F && pStat->m_course != 0xFD) ? pStat->m_course - 0x80 : pStat->m_course));
		if (pCourse == NULL || pCourse->m_active == 0)
			continue;

		unsigned char kind = pCourse->kind;
		sMapStatistics *pBest = ppBest[kind - 1];

		if (pBest == NULL) {
			ppBest[kind - 1] = pStat;
		} else if (pStat->m_bestScore != 0x7F) {
			if (pStat->m_bestScore < pBest->m_bestScore) {
				ppBest[kind - 1] = pStat;
			} else if (pStat->m_bestScore == pBest->m_bestScore) {
				__int64 value = pStat->m_bestPang;
				if (value != 0 && value > pBest->m_bestPang)
					ppBest[kind - 1] = pStat;
			}
		}
	}
}

// user_info.cpp:2784
void FrUserInfoForm::SetContent(bool b) {
	if (m_userInfo.m_uid == 0xFFFFFFFF)
		return;

	m_bOption = b;

	sPangYaUserInfo *pInfo = &m_userInfo.m_pangyaInfo;
	sPangYaUserStatistics *pStatistics = NULL;
	sTrophyStatistics *pTrophy = NULL;
	std::list<sSpecialTrophy> *pSpecialTrophies = NULL;
	std::list<sGuildTrophy> *pGuildTrophies = NULL;
	const char *noRecord = "\xb1\xe2\xb7\xcf \xbe\xf8\xc0\xbd";   // "no record"

	memset(m_pBestMapStats, 0, sizeof(m_pBestMapStats));

	if (m_seasonTab == SEASON_CURRENT)
		SortMapStat(m_pBestMapStats, m_userInfo.m_mapStatistics);
	else if (m_seasonTab == SEASON_TOTAL)
		SortMapStat(m_pBestMapStats, m_totalRecord.m_mapStats);

	SelectContents(m_seasonTab, &pStatistics, &pTrophy, &pSpecialTrophies, &pGuildTrophies);

	m_pIdEdit->SetLine(1, m_userInfo.m_name, 0xFF000000, false, 0);

	UpdateEditSchoolAndGuild(m_seasonTab, pInfo);
	UpdateEditExpAndMannerInfo(m_seasonTab, pStatistics);
	UpdateEditUserLocation();

	UpdateLevelTextImg(pStatistics->level);
	UpdateEditSpecialInfo(m_seasonTab, pStatistics, pTrophy);

	SetMatchHistoryContents();

	char buf[128] = {0};
	char bestBuf[5][128];

	int hours = pStatistics->totalPlayTime / 3600;
	int minutes = (pStatistics->totalPlayTime - hours * 3600) / 60;

	if (hours == 0)
		sprintf(buf, "%d \xba\xd0", minutes);   // "%d min"
	else if (hours < 24)
		sprintf(buf, "%d \xbd\xc3\xb0\xa3 %d \xba\xd0", hours, minutes);   // "%d h %d min"
	else
		sprintf(buf, "%d \xc0\xcf %d \xbd\xc3\xb0\xa3 %d \xba\xd0", hours / 24, hours % 24, minutes);   // "%d d %d h %d min"
	m_pRecordEdit[0]->SetLine(1, buf, 0xFF000000, false, 0);

	sprintf(buf, "%d ", float2int(pStatistics->totalDistance * 2.28595e-05f));
	m_pRecordEdit[3]->SetLine(1, buf, 0xFF000000, false, 0);

	if (pStatistics->totalDistance == 0) {
		strcpy(buf, noRecord);
	} else {
		float distance = pStatistics->totalDistance * 0.0001f;

		sprintf(buf, "%.1f\xb8\xb8 y ", distance);
	}
	m_pRecordEdit[4]->SetLine(1, buf, 0xFF000000, false, 0);

	if (pStatistics->shots + pStatistics->puttAttempts == 0) {
		strcpy(buf, noRecord);
	} else {
		RemakeNumber(pStatistics->shots + pStatistics->puttAttempts, buf);
		strcat(buf, " \xc5\xb8");
	}
	m_pRecordEdit[1]->SetLine(1, buf, 0xFF000000, false, 0);

	if (pStatistics->drives == 0) {
		strcpy(buf, noRecord);
	} else {
		RemakeNumber(pStatistics->drives, buf);
		strcat(buf, " \xc8\xa6");
	}
	m_pRecordEdit[2]->SetLine(1, buf, 0xFF000000, false, 0);

	if (WAbsBySign(pStatistics->bestDrive) < g_EPSILON)
		strcpy(buf, noRecord);
	else
		sprintf(buf, "%.2f y", pStatistics->bestDrive);
	m_pRecordEdit[12]->SetLine(1, buf, 0xFF000000, false, 0);

	if (WAbsBySign(pStatistics->longestPutt) < g_EPSILON)
		strcpy(buf, noRecord);
	else
		sprintf(buf, "%.2f y", pStatistics->longestPutt);
	m_pRecordEdit[13]->SetLine(1, buf, 0xFF000000, false, 0);

	if (WAbsBySign(pStatistics->longestChipIn) < g_EPSILON)
		strcpy(buf, noRecord);
	else
		sprintf(buf, "%.2f y", pStatistics->longestChipIn);
	m_pRecordEdit[14]->SetLine(1, buf, 0xFF000000, false, 0);

	if (m_seasonTab == SEASON_CURRENT || m_seasonTab == SEASON_TOTAL) {
		for (int i = 4; i >= 0; i--) {
			if (m_pBestMapStats[i] == NULL) {
				strcpy(bestBuf[i], noRecord);
			} else if (m_pBestMapStats[i]->m_bestScore == 0) {
				sprintf(bestBuf[i], "%+d [%I64dp]", m_pBestMapStats[i]->m_bestScore, m_pBestMapStats[i]->m_bestPang);
			} else if (m_pBestMapStats[i]->m_bestScore == 0x7F) {
				strcpy(bestBuf[i], noRecord);
			} else {
				sprintf(bestBuf[i], "%+d [%I64dp]", m_pBestMapStats[i]->m_bestScore, m_pBestMapStats[i]->m_bestPang);
			}
		}
	}

	if (m_seasonTab == SEASON_CURRENT || m_seasonTab == SEASON_TOTAL) {
		m_pRecordEdit[5]->SetLine(1, bestBuf[4], 0xFF000000, false, 0);
		m_pRecordEdit[6]->SetLine(1, bestBuf[3], 0xFF000000, false, 0);
	}

	m_pRecordEdit[7]->SetLine(1, bestBuf[2], 0xFF000000, false, 0);
	m_pRecordEdit[8]->SetLine(1, bestBuf[1], 0xFF000000, false, 0);
	m_pRecordEdit[9]->SetLine(1, bestBuf[0], 0xFF000000, false, 0);

	sprintf(buf, "%d", pStatistics->holeInOnes);
	m_pRecordEdit[10]->SetLine(1, buf, 0xFF000000, false, 0);

	sprintf(buf, "%d", pStatistics->albatrosses);
	m_pRecordEdit[11]->SetLine(1, buf, 0xFF000000, false, 0);

	bool bClassic;
	if (m_pRecordClassicBtn->GetStatus() == FrButton::PRESSED)
		bClassic = true;
	else
		bClassic = false;
	UpdateEditMapStatistics(m_seasonTab, bClassic);

	int medalCount = 0;
	if (m_pTrophyNormalList) {
		m_pTrophyNormalList->ClearItem();

		for (int i = 12; i >= 0; i--) {
			if (pTrophy->m_medals[i][0] || pTrophy->m_medals[i][1] || pTrophy->m_medals[i][2]) {
				m_pTrophyNormalList->AddItem((void *)i);

				medalCount += pTrophy->m_medals[i][0] + pTrophy->m_medals[i][1] + pTrophy->m_medals[i][2];
			}
		}
	}

	if (m_pTrophyNormalNum) {
		sprintf(buf, "%d", medalCount);
		m_pTrophyNormalNum->SetLine(1, buf, 0xFF000000, false, 0);
	}

	int specialCount = 0;
	if (m_pTrophySpecialList) {
		m_pTrophySpecialList->ClearItem();

		for (std::list<sSpecialTrophy>::iterator it = pSpecialTrophies->begin(); it != pSpecialTrophies->end(); ++it) {
			m_pTrophySpecialList->AddItem(&*it);
			specialCount += it->m_count;
		}
	}

	if (m_pTrophySpecialNum) {
		sprintf(buf, "%d", specialCount);
		m_pTrophySpecialNum->SetLine(1, buf, 0xFF000000, false, 0);
	}

	if (m_pGuildTrophyList) {
		m_pGuildTrophyList->ClearItem();

		for (std::list<sGuildTrophy>::reverse_iterator it = pGuildTrophies->rbegin(); it != pGuildTrophies->rend(); ++it)
			m_pGuildTrophyList->AddItem(&*it);
	}

	if (m_userInfo.m_guildUID != 0) {
		sprintf(buf, "%s", m_userInfo.m_guildName);
		m_pGuildNameEdit->SetLine(1, buf, 0xFF000000, false, 0);

		sprintf(buf, "%d", m_guildPang);
		m_pGuildPangEdit->SetLine(1, buf, 0xFF000000, false, 0);

		sprintf(buf, "%d", m_guildMyPercent);
		m_pGuildMyPercentEdit->SetLine(1, buf, 0xFF000000, false, 0);
	} else {
		const char *none = "\xbe\xf8\xc0\xbd";   // "none"

		strcpy(buf, none);
		m_pGuildNameEdit->SetLine(1, buf, 0xFF000000, false, 0);

		strcpy(buf, none);
		m_pGuildPangEdit->SetLine(1, buf, 0xFF000000, false, 0);

		strcpy(buf, none);
		m_pGuildMyPercentEdit->SetLine(1, buf, 0xFF000000, false, 0);
	}
}

// user_info.cpp:3078
void FrUserInfoForm::BuildTotalStat(void) {
	m_totalRecord.Clear();

	const sPangYaUserStatistics *pSeason = &m_seasonRecord.m_statistics;
	const sPangYaUserStatistics *pUser = &m_userInfo.m_statistics;

	// The totals take the shown player's record (the current season) plus the
	// other season record, and the better of the two bests.
	m_totalRecord.m_statistics.level = pUser->level;
	m_totalRecord.m_statistics.exp = pUser->exp;
	m_totalRecord.m_statistics.gamesPlayed = pUser->gamesPlayed;
	m_totalRecord.m_statistics.noMannerGames = pUser->noMannerGames;
	m_totalRecord.m_statistics.combos = pUser->combos;
	m_totalRecord.m_statistics.totalCombos = pUser->totalCombos;
	m_totalRecord.m_statistics.rateTotal = pUser->rateTotal + pSeason->rateTotal;
	m_totalRecord.m_statistics.rateCount = pUser->rateCount + pSeason->rateCount;
	m_totalRecord.m_statistics.drives = pUser->drives + pSeason->drives;
	m_totalRecord.m_statistics.par3Drives = pUser->par3Drives + pSeason->par3Drives;
	m_totalRecord.m_statistics.scoreToPar = pUser->scoreToPar + pSeason->scoreToPar;
	m_totalRecord.m_statistics.shots = pUser->shots + pSeason->shots;
	m_totalRecord.m_statistics.puttAttempts = pUser->puttAttempts + pSeason->puttAttempts;
	m_totalRecord.m_statistics.playTime = pUser->playTime + pSeason->playTime;
	m_totalRecord.m_statistics.pangyas = pUser->pangyas + pSeason->pangyas;
	m_totalRecord.m_statistics.fairways = pUser->fairways + pSeason->fairways;
	m_totalRecord.m_statistics.puttsSunk = pUser->puttsSunk + pSeason->puttsSunk;
	m_totalRecord.m_statistics.obCount = pUser->obCount + pSeason->obCount;
	m_totalRecord.m_statistics.totalSkinsPang = pUser->totalSkinsPang + pSeason->totalSkinsPang;
	m_totalRecord.m_statistics.totalPlayTime = pUser->totalPlayTime + pSeason->totalPlayTime;
	m_totalRecord.m_statistics.totalDistance = pUser->totalDistance + pSeason->totalDistance;

	m_totalRecord.m_statistics.bestDrive = pUser->bestDrive > pSeason->bestDrive ? pUser->bestDrive : pSeason->bestDrive;
	m_totalRecord.m_statistics.longestPutt = pUser->longestPutt > pSeason->longestPutt ? pUser->longestPutt : pSeason->longestPutt;
	m_totalRecord.m_statistics.longestChipIn = pUser->longestChipIn > pSeason->longestChipIn ? pUser->longestChipIn : pSeason->longestChipIn;

	m_totalRecord.m_statistics.holeInOnes = pUser->holeInOnes + pSeason->holeInOnes;
	m_totalRecord.m_statistics.albatrosses = pUser->albatrosses + pSeason->albatrosses;

	for (int i = 0; i < 5; i++) {
		m_totalRecord.m_statistics.bestScore[i] = 0x7F;

		if (pSeason->bestScore[i] != 0x7F) {
			m_totalRecord.m_statistics.bestScore[i] = pSeason->bestScore[i];
			m_totalRecord.m_statistics.maxPang[i] = pSeason->maxPang[i];
		}

		if (pUser->bestScore[i] != 0x7F) {
			if (m_totalRecord.m_statistics.bestScore[i] > pUser->bestScore[i]) {
				m_totalRecord.m_statistics.bestScore[i] = pUser->bestScore[i];
				m_totalRecord.m_statistics.maxPang[i] = pUser->maxPang[i];
			} else if (m_totalRecord.m_statistics.bestScore[i] == pUser->bestScore[i]) {
				if (m_totalRecord.m_statistics.maxPang[i] < pUser->maxPang[i])
					m_totalRecord.m_statistics.maxPang[i] = pUser->maxPang[i];
			}
		}
	}

	for (int i = 0; i < 20; i++) {
		m_totalRecord.m_mapStats[i].m_course = i;
		m_totalRecord.m_mapStats[i].m_bestScore = 0x7F;
		if (m_seasonRecord.m_mapStats[i].m_bestScore != 0x7F)
			m_totalRecord.m_mapStats[i] = m_seasonRecord.m_mapStats[i];

		if (m_userInfo.m_mapStatistics[i].m_bestScore != 0x7F) {
			if (m_userInfo.m_mapStatistics[i].m_bestScore < m_totalRecord.m_mapStats[i].m_bestScore)
				m_totalRecord.m_mapStats[i] = m_userInfo.m_mapStatistics[i];
		}
	}

	for (int i = 0; i < 20; i++) {
		m_totalClassicMapStats[i].m_course = i;
		m_totalClassicMapStats[i].m_bestScore = 0x7F;
		if (m_seasonClassicMapStats[i].m_bestScore != 0x7F)
			m_totalClassicMapStats[i] = m_seasonClassicMapStats[i];

		if (m_classicMapStats[i].m_bestScore != 0x7F) {
			if (m_classicMapStats[i].m_bestScore < m_totalClassicMapStats[i].m_bestScore)
				m_totalClassicMapStats[i] = m_classicMapStats[i];
		}
	}

	const sTrophyStatistics *pSeasonTrophy = &m_seasonRecord.m_trophy;
	const sTrophyStatistics *pUserTrophy = &m_userInfo.m_trophy;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 13; j++)
			m_totalRecord.m_trophy.m_medals[j][i] += pUserTrophy->m_medals[j][i] + pSeasonTrophy->m_medals[j][i];
	}

	m_totalRecord.m_specialTrophies.clear();

	std::list<sSpecialTrophy>::iterator itSpecial;
	for (itSpecial = m_seasonRecord.m_specialTrophies.begin(); itSpecial != m_seasonRecord.m_specialTrophies.end(); ++itSpecial)
		m_totalRecord.m_specialTrophies.push_back(*itSpecial);

	for (itSpecial = m_specialTrophies.begin(); itSpecial != m_specialTrophies.end(); ++itSpecial)
		m_totalRecord.m_specialTrophies.push_back(*itSpecial);

	m_totalRecord.m_guildTrophies.clear();

	std::list<sGuildTrophy>::iterator itGuild;
	for (itGuild = m_seasonRecord.m_guildTrophies.begin(); itGuild != m_seasonRecord.m_guildTrophies.end(); ++itGuild)
		m_totalRecord.m_guildTrophies.push_back(*itGuild);

	for (itGuild = m_guildTrophies.begin(); itGuild != m_guildTrophies.end(); ++itGuild)
		m_totalRecord.m_guildTrophies.push_back(*itGuild);
}

// user_info.cpp:3235
unsigned long FrUserInfoForm::GetBestScoreColor(unsigned char course) {
	if (m_userInfo.m_uid == WSingleton<CSharedDoc>::Instance()->m_myUID)
		return 0xFF000000;

	const sPangYaUserStatistics *pStatistics = &m_userInfo.m_statistics;
	const sPangYaUserStatistics *pMyStatistics = &WSingleton<CSharedDoc>::Instance()->m_myStatistics;
	bool bBetter;
	if (pStatistics->bestScore[course] == 0x7F)
		bBetter = false;
	else if (pMyStatistics->bestScore[course] == 0x7F)
		bBetter = true;
	else
		bBetter = pStatistics->bestScore[course] < pMyStatistics->bestScore[course];

	return bBetter ? 0xFFFFA045 : 0xFF000000;
}

// user_info.cpp:3261
unsigned long FrUserInfoForm::GetMaxPangColor(unsigned char course) {
	if (m_userInfo.m_uid == WSingleton<CSharedDoc>::Instance()->m_myUID)
		return 0xFFFFFFFF;

	const sPangYaUserStatistics *pStatistics = &m_userInfo.m_statistics;
	const sPangYaUserStatistics *pMyStatistics = &WSingleton<CSharedDoc>::Instance()->m_myStatistics;
	bool bBetter = pStatistics->maxPang[course] > pMyStatistics->maxPang[course];

	return bBetter ? 0xFFFFA045 : 0xFF000000;
}

// user_info.cpp:3278
unsigned long FrUserInfoForm::Get30sRecordColor(int medal) {
	if (m_userInfo.m_uid == WSingleton<CSharedDoc>::Instance()->m_myUID)
		return 0xFF000000;

	const sTrophyStatistics *pTrophy = &m_userInfo.m_trophy;
	const sTrophyStatistics *pMyTrophy = &WSingleton<CSharedDoc>::Instance()->m_myTrophy;
	int myCount = 0, count = 0;

	for (int i = 0; i < 13; i++) {
		myCount += pMyTrophy->m_medals[i][medal];
		count += pTrophy->m_medals[i][medal];
	}

	return count > myCount ? 0xFFFFA045 : 0;
}

// user_info.cpp:3296
unsigned long FrUserInfoForm::GetColor(eTabType tab, unsigned long index) {
	if (m_userInfo.m_uid == WSingleton<CSharedDoc>::Instance()->m_myUID
			&& (tab != TAB_GENERAL_INFO || index != 5))
		return 0xFF000000;

	const sPangYaUserStatistics *pUser = &m_userInfo.m_statistics;
	const sPangYaUserStatistics *pMy = &WSingleton<CSharedDoc>::Instance()->m_myStatistics;

	bool bBetter = false;

	switch (tab) {
	case TAB_GENERAL_INFO:
		if (index != 5)
			break;

		// the disconnect rate: redder the more games were left
		if (pUser->gamesPlayed == 0)
			return 0xFF000000;

		{
			unsigned char c = 0xFF - (unsigned char)((float)pUser->noMannerGames / (float)pUser->gamesPlayed * 255.0f);
			return 0xFFFF0000 | (c << 8) | c;
		}

	case TAB_RECORD_INFO:
		switch (index) {
		case 3: {
			unsigned long userTotal = pUser->rateTotal;
			if (userTotal == 0) {
				bBetter = false;
			} else {
				unsigned long myTotal = pMy->rateTotal;
				if (myTotal == 0)
					bBetter = true;
				else
					bBetter = (float)pUser->rateCount / (float)userTotal > (float)pMy->rateCount / (float)myTotal;
			}
			break;
		}
		case 4: {
			unsigned long userHoles = pUser->drives - pUser->par3Drives;
			if (userHoles == 0) {
				bBetter = false;
			} else {
				unsigned long myHoles = pMy->drives - pMy->par3Drives;
				if (myHoles == 0)
					bBetter = true;
				else
					bBetter = (float)pUser->scoreToPar / (float)userHoles < (float)pMy->scoreToPar / (float)myHoles;
			}
			break;
		}
		case 5: {
			if (pUser->shots + pUser->puttAttempts == 0) {
				bBetter = false;
			} else {
				if (pMy->shots + pMy->puttAttempts == 0)
					bBetter = true;
				else
					bBetter = (float)pUser->playTime / (float)(pUser->shots + pUser->puttAttempts) < (float)pMy->playTime / (float)(pMy->shots + pMy->puttAttempts);
			}
			break;
		}
		case 6: {
			unsigned long userShots = pUser->shots;
			if (userShots == 0) {
				bBetter = false;
			} else {
				unsigned long myShots = pMy->shots;
				if (myShots == 0)
					bBetter = true;
				else
					bBetter = (float)pUser->pangyas / (float)userShots > (float)pMy->pangyas / (float)myShots;
			}
			break;
		}
		case 7: {
			unsigned long userHoles = pUser->drives - pUser->par3Drives;
			if (userHoles == 0) {
				bBetter = false;
			} else {
				unsigned long myHoles = pMy->drives - pMy->par3Drives;
				if (myHoles == 0)
					bBetter = true;
				else
					bBetter = (float)pUser->fairways / (float)userHoles > (float)pMy->fairways / (float)myHoles;
			}
			break;
		}
		case 8: {
			unsigned long userPutts = pUser->puttAttempts;
			if (userPutts == 0) {
				bBetter = false;
			} else {
				unsigned long myPutts = pMy->puttAttempts;
				if (myPutts == 0)
					bBetter = true;
				else
					bBetter = (float)pUser->puttsSunk / (float)userPutts > (float)pMy->puttsSunk / (float)myPutts;
			}
			break;
		}
		case 9: {
			if (pUser->shots + pUser->puttAttempts == 0) {
				bBetter = false;
			} else {
				if (pMy->shots + pMy->puttAttempts == 0)
					bBetter = true;
				else
					bBetter = (float)pUser->obCount / (float)(pUser->shots + pUser->puttAttempts) < (float)pMy->obCount / (float)(pMy->shots + pMy->puttAttempts);
			}
			break;
		}
		}
		break;

	case TAB_SPECIAL_INFO:
		switch (index) {
		case 0:
			bBetter = pUser->totalPlayTime > pMy->totalPlayTime;
			break;
		case 4:
			bBetter = pUser->totalDistance > pMy->totalDistance;
			break;
		case 1:
			bBetter = pUser->shots + pUser->puttAttempts > pMy->shots + pMy->puttAttempts;
			break;
		case 2:
			bBetter = pUser->drives > pMy->drives;
			break;
		case 12:
			bBetter = pUser->bestDrive > pMy->bestDrive;
			break;
		case 13:
			bBetter = pUser->longestPutt > pMy->longestPutt;
			break;
		case 14:
			bBetter = pUser->longestChipIn > pMy->longestChipIn;
			break;
		case 10:
			bBetter = pUser->holeInOnes > pMy->holeInOnes;
			break;
		case 11:
			bBetter = pUser->albatrosses > pMy->albatrosses;
			break;
		}
		break;
	}

	return bBetter ? 0xFFFFA045 : 0;
}

// user_info.cpp:3487
void FrUserInfoForm::OnPetInit(PG_MSGARG param) {
	m_pPetArea = DYNAMIC_CAST(FrArea, param);
}

// user_info.cpp:3492
void FrUserInfoForm::OnPetOwnerDraw(void) {
	// the pet is drawn on a cleared depth buffer
	g_view->Clear(0, 2);

	if (m_pPetLoadGauge && m_pPetLoadGauge->IsVisible())
		return;

	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();

	if (!pGDI)
		return;

	WRect rect = m_pPetArea->GetRect();

	if (m_userInfo.m_skinTypeIds[0]) {
		// the background skin, tiled over the area from the scroll offset
		IFF_STRUCT::sSkin *pSkin = ItemManager()->FindSkin(m_userInfo.m_skinTypeIds[0]);

		if (pSkin) {
			const Bitmap *pBmp = g_pFresh->GetBitmap(pSkin->m_cutinImage);
			if (pBmp) {
				float width = (float)pBmp->Width();
				float height = (float)pBmp->Height();

				if (m_skinScroll[0] > width)
					m_skinScroll[0] -= width;
				else if (m_skinScroll[0] < 0.0f)
					m_skinScroll[0] += width;

				if (m_skinScroll[1] > height)
					m_skinScroll[1] -= height;
				else if (m_skinScroll[1] < 0.0f)
					m_skinScroll[1] += height;

				WRect src(m_skinScroll[0], m_skinScroll[1], width - m_skinScroll[0], height - m_skinScroll[1]);

				WRect dst;
				dst.x = rect.x;
				dst.y = rect.y;
				dst.w = src.w;
				dst.h = src.h;

				while (dst.y < rect.y + rect.h) {
					while (dst.x < rect.x + rect.w) {
						pGDI->DrawTexture(pBmp, src, dst, 0xFFFFFFFF, 0);

						dst.x += dst.w;
						dst.w = width > rect.x + rect.w - dst.x ? rect.x + rect.w - dst.x : width;

						src.x = 0.0f;
						src.w = dst.w;
					}

					dst.x = rect.x;
					dst.y += dst.h;
					dst.w = width - m_skinScroll[0];
					dst.h = height > rect.y + rect.h - dst.y ? rect.y + rect.h - dst.y : height;

					src.x = m_skinScroll[0];
					src.y = 0.0f;
					src.w = dst.w;
					src.h = dst.h;
				}
			}
		}
	}

	if (!m_anotherHand.IsWorking(0)) {
		m_anotherHand.Enter();
		if (m_pPetExhibition)
			m_pPetExhibition->Display(0.0f, 0.0f);
		m_anotherHand.Leave();
	}

	// the two overlay skins, drawn over the whole area
	if (m_userInfo.m_skinTypeIds[1]) {
		IFF_STRUCT::sSkin *pSkin = ItemManager()->FindSkin(m_userInfo.m_skinTypeIds[1]);

		if (pSkin) {
			const Bitmap *pBmp = g_pFresh->GetBitmap(pSkin->m_cutinImage);
			if (pBmp)
				pGDI->DrawTexture(pBmp, rect, 0xFFFFFFFF, 0);
		}
	}

	if (m_userInfo.m_skinTypeIds[2]) {
		IFF_STRUCT::sSkin *pSkin = ItemManager()->FindSkin(m_userInfo.m_skinTypeIds[2]);

		if (pSkin) {
			const Bitmap *pBmp = g_pFresh->GetBitmap(pSkin->m_cutinImage);
			if (pBmp)
				pGDI->DrawTexture(pBmp, rect, 0xFFFFFFFF, 0);
		}
	}
}

// user_info.cpp:3604
void FrUserInfoForm::OnPetLoadInit(PG_MSGARG param) {
	m_pPetLoadGauge = DYNAMIC_CAST(FrGaugeBar, param);
	if (m_pPetLoadGauge) {
		m_pPetLoadGauge->SetRange(0, 0x78, 0);
		m_pPetLoadGauge->SetPos(0);
		m_pPetLoadGauge->SetDestPos(0x78, false);
		m_pPetLoadGauge->SetVisible(true);
	}
}

// user_info.cpp:3616
void FrUserInfoForm::ShowPet(void) {
	m_anotherHand.Enter();

	if (m_pPetExhibition) {
		delete m_pPetExhibition;
		m_pPetExhibition = NULL;
	}
	if (m_pPetTids) {
		delete m_pPetTids;
		m_pPetTids = NULL;
	}

	if (m_pPetArea) {
		WRect rect = m_pPetArea->GetRect();

		m_pPetExhibition = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.35f, true);
		m_pPetExhibition->SetDistance(g_view->GetWidth() * 0.0234375f, true, false);
		m_pPetExhibition->SetArea(rect.w, rect.h);

		m_pPetTids = new CPartTidList;

		sCharacterInfo *pCharInfo = &m_userInfo.m_characterInfo;
		if (!m_pPetTids->SetTids(pCharInfo, 0xFF) || !m_pPetTids->IsComboValid())
			m_pPetTids->SetDefaultTids();

		m_pPetExhibition->SetModel(*m_pPetTids, pCharInfo->auxParts.tids, NULL, 3.0f, 0);
		if (!m_userInfo.m_eventFlags.eventFlag3 && m_userInfo.m_eventFlags.eventFlag4)
			m_pPetExhibition->AttachAngelWingFx();
	}

	m_anotherHand.Leave();
}

// user_info.cpp:3648
void FrUserInfoForm::OnCustomProc(float elapsed) {
	if (m_anotherHand.IsWorking(0)) {
		// the pet is still loading on the worker thread
		Sleep(20);
	} else {
		m_anotherHand.Enter();

		if (m_pPetExhibition) {
			m_pPetExhibition->Process(elapsed, true);
		}

		m_anotherHand.Leave();
	}

	if ((!(WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80) || m_userInfo.m_uid == WSingleton<CSharedDoc>::Instance()->m_myUID)
			&& g_input->GetDown("TAB", true) && !g_input->Get("LALT", true) && !g_input->Get("RALT", true)) {
		g_audio->PlaySfx("ui_icon_click", NULL, 0, NULL, NULL, 0.5f, 200.0f);

		switch (m_curTab) {
		case TAB_GENERAL_INFO:
			SetFormA(TAB_SPECIAL_INFO);
			break;

		case TAB_SPECIAL_INFO:
			SetFormA(TAB_RECORD_INFO);
			break;

		case TAB_RECORD_INFO:
			SetFormA(TAB_TROPHY_INFO);
			break;

		case TAB_TROPHY_INFO:
			SetFormA(TAB_GUILD_INFO);
			break;

		case TAB_GUILD_INFO:
			SetFormA(TAB_GENERAL_INFO);
			break;
		}
	}

	if (m_userInfo.m_skinTypeIds[0]) {
		for (int i = 0; i < 2; i++) {
			float range = (i == 0) ? 138.0f : 228.0f;

			switch (m_skinScrollState[i]) {
			case 0:
				m_skinScroll[i] = 0;
				break;

			case 1:
				m_skinScroll[i] -= range * elapsed * 0.25f;
				break;

			case 2:
				m_skinScroll[i] += range * elapsed * 0.25f;
				break;
			}
		}
	}
}

// user_info.cpp:3724
void FrUserInfoForm::SetVisible(FrWnd* pWnds[], int count, bool bVisible) {
	for (int i = 0; i < count; i++) {
		if (pWnds[i]) {
			pWnds[i]->SetVisible(bVisible);
		}
	}
}

// user_info.cpp:3744
void FrUserInfoForm::SetVisible(eTabType tab, bool bVisible) {
	switch (tab) {
	case TAB_GENERAL_INFO:
		SetVisible(m_pGeneralInfoWnd, 8, bVisible);
		if (m_pLevelArea)
			m_pLevelArea->SetVisible(bVisible);
		if (m_pClubArea)
			m_pClubArea->SetVisible(bVisible);
		if (m_pGeneralBack)
			m_pGeneralBack->SetVisible(bVisible);
		if (m_pDisconnQuestionMark) {
			if (bVisible && !IsHideDisconnectRate(&m_userInfo.m_statistics))
				SetDisconnQuestionMark(false);
			else
				SetDisconnQuestionMark(bVisible);
		}

		if (m_pRmrVisitorBtn) {
			int bAllowed = 0;
			CTaskManager* pTaskManager = CTaskManager::Instance(); (!pTaskManager ? 0 : pTaskManager->GetCurrentTask())->SendMsgToMainActor(MsgObject(0, 0x231, (intptr_t)&bAllowed, 2, 3, 0, 0));

			if (bAllowed && m_userInfo.m_uid != WSingleton<CSharedDoc>::Instance()->m_myUID)
				m_pRmrVisitorBtn->SetVisible(bVisible);
			else
				m_pRmrVisitorBtn->SetVisible(false);
		}
		break;

	case TAB_SPECIAL_INFO:
		SetVisible(m_pSpecialInfoWnd, 16, bVisible);
		for (int i = 0; i < 5; i++)
			m_pMatchHistoryBtn[i]->SetVisible(false);
		m_pSpecList->SetVisible(bVisible);
		break;

	case TAB_RECORD_INFO:
		if (m_pRecordNormalBtn)
			m_pRecordNormalBtn->SetVisible(bVisible);
		if (m_pRecordCourseBtn)
			m_pRecordCourseBtn->SetVisible(bVisible);
		if (m_pRecordClassicBtn)
			m_pRecordClassicBtn->SetVisible(bVisible);

		if (!bVisible) {
			SetVisible(m_pRecordWnd, 15, false);
			m_pScoreList->SetVisible(false);
			m_pRecTotalList->SetVisible(false);
			if (m_pRecTotalList)
				m_pRecTotalList->SetVisible(false);
			if (m_pRecCourseBack)
				m_pRecCourseBack->SetVisible(false);
		} else {
			ChangeRecordType();
		}
		break;

	case TAB_TROPHY_INFO:
		if (m_pTrophyNormalBtn)
			m_pTrophyNormalBtn->SetVisible(bVisible);
		if (m_pTrophySpecialBtn)
			m_pTrophySpecialBtn->SetVisible(bVisible);

		if (!bVisible) {
			if (m_pTrophyNormalNum)
				m_pTrophyNormalNum->SetVisible(false);
			if (m_pTrophySpecialNum)
				m_pTrophySpecialNum->SetVisible(false);
			if (m_pTrophySpecialList)
				m_pTrophySpecialList->SetVisible(false);
			if (m_pTrophyNormalList)
				m_pTrophyNormalList->SetVisible(false);
			if (m_pNoTrophyStatic)
				m_pNoTrophyStatic->SetVisible(false);
			if (m_pTroNormalBack)
				m_pTroNormalBack->SetVisible(false);
			if (m_pTroSpecialBack)
				m_pTroSpecialBack->SetVisible(false);
		} else {
			ChangeTrophyType();
		}
		break;

	case TAB_GUILD_INFO:
		SetVisible(m_pGuildInfoEdit, 3, bVisible);
		m_pMyGuildArea->SetVisible(bVisible);
		if (m_pGuildBack)
			m_pGuildBack->SetVisible(bVisible);
		if (m_pGuildTrophyList)
			m_pGuildTrophyList->SetVisible(bVisible);
		m_pGuildNoTrophyStatic->SetVisible(bVisible && m_pGuildTrophyList->m_itemList.empty());
		break;
	}
}

// user_info.cpp:3895
void FrUserInfoForm::OnCommentInit(PG_MSGARG param) {
	m_pCommentEdit = DYNAMIC_CAST(FrEdit, param);
	if (m_pCommentEdit) {
		m_pCommentEdit->SetVisible(false);
	}
}

// user_info.cpp:3902
void FrUserInfoForm::OnEditInit_HoverOn(PG_MSGARG param) {
	if (!m_pCommentEdit)
		return;

	FrEdit* pEdit = DYNAMIC_CAST(FrEdit, param);
	if (!pEdit)
		return;

	const char* comment = "";
	int len = strlen(comment);
	if (len) {
		m_pCommentEdit->ClearLine();
		m_pCommentEdit->AddText(comment, false, false);
		m_pCommentEdit->MoveWindow(pEdit->GetRect().TopLeft());
		m_pCommentEdit->SetVisible(true);
	}
}

// user_info.cpp:3942
void FrUserInfoForm::OnEditInit_HoverOff(PG_MSGARG param) {
	if (m_pCommentEdit) {
		m_pCommentEdit->SetVisible(false);
	}
}

// Texture of the score list's title font (data_a97bd8).
static char* s_scoreFontFile = "[font_wind.jpg";

// user_info.cpp:3948
void FrUserInfoForm::OnScoreListInit(PG_MSGARG param) {
	m_pScoreList = DYNAMIC_CAST(FrListBox, param);
	if (m_pScoreList) {
		m_pScoreList->SetVisible(false);

		tagWTITLEFONT info;
		memset(&info, 0, sizeof(info));
		info.filename = &s_scoreFontFile;
		info.fontw = 16;
		info.fonth = 16;
		info.numPages = 1;
		info.texw = 128;
		info.texh = 64;
		info.pCharSet = "1234567890ym%-.?/:";
		m_pTitleFont = g_resrcmng->GetTitleFont();
		m_pTitleFont->Create(&info);
	}
}

// user_info.cpp:3972
void FrUserInfoForm::OnScoreListOwnerDraw(PG_MSGARG param) {
	FrListItem *pItem = (FrListItem *)param;
	if (pItem == NULL)
		return;

	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	sMapStatistics *pStats = (sMapStatistics *)pItem->pData;

	IFF_STRUCT::sCourse *pCourse = ItemManager()->FindCourse(0x28000000
			| ((pStats->m_course > 0x7F && pStats->m_course != 0xFD) ? pStats->m_course - 0x80 : pStats->m_course));
	if (pCourse) {
		const Bitmap *pBmp = g_pFresh->GetManager()->GetBitmap("CAPTION", MakeStr("room_%s", pCourse->m_icon));
		pGDI->DrawTexture(pBmp, WRect(pItem->pos.x, pItem->pos.y + 7.0f, (float)pBmp->Width(), (float)pBmp->Height()), pStats->m_bestScore != 0x7F ? 0xFFFFFFFF : 0x60000000, 0);
	}

	if (pStats->m_bestScore != 0x7F) {
		char *pScore = MakeStr("%d", pStats->m_bestScore);
		m_pTitleFont->Print(g_view, pItem->pos.x + 130.0f, pItem->pos.y + 20.0f, pScore, 0, 0xFFFFFFFF, NULL);

		if (pStats->m_itemTypeId) {
			IFF_ITEM_COMMON *pCommon = ItemManager()->FindCommonItem(pStats->m_itemTypeId);
			if (pCommon) {
				const Bitmap *pIcon = g_pFresh->GetManager()->GetBitmap("ITEMS", MakeStr("bar_%s", pCommon->m_icon));
				if (pIcon) {
					pGDI->DrawTexture(pIcon, WRect(pItem->pos.x + 79.0f, pItem->pos.y + 13.0f, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);
				}
			}
		} else {
			m_pTitleFont->Print(g_view, pItem->pos.x + 88.0f, pItem->pos.y + 20.0f, "-", 0, 0xFFFFFFFF, NULL);
		}

		m_pTitleFont->Print(g_view, pItem->pos.x + 188.0f, pItem->pos.y + 20.0f, MakeStr("%I64d", pStats->m_bestPang), 0, 0xFFFFFFFF, NULL);
	} else {
		m_pTitleFont->Print(g_view, pItem->pos.x + 88.0f, pItem->pos.y + 20.0f, "-", 0, 0xFFFFFFFF, NULL);
		m_pTitleFont->Print(g_view, pItem->pos.x + 140.0f, pItem->pos.y + 20.0f, "-", 0, 0xFFFFFFFF, NULL);
		m_pTitleFont->Print(g_view, pItem->pos.x + 200.0f, pItem->pos.y + 20.0f, "-", 0, 0xFFFFFFFF, NULL);
	}
}

// user_info.cpp:4027
bool FrUserInfoForm::OnAddFriendFormResult(int ret, FrForm *pForm) {
	m_addFriendDlg = NULL;

	if ((__rtti_obj = AfxGetTask()) != 0 && ((IObject*)__rtti_obj)->IsKindOf(&CGolfTask::m_RTTI)) {
		if (AfxGetTask()->GetActor("GolfRule") && !m_bChatActive)
			AfxGetTask()->GetActor("GolfRule") << MsgObject(0, 0x62, 0, 0, 0, 0, 0);
	}
	else if ((__rtti_obj = AfxGetTask()) != 0 && ((IObject*)__rtti_obj)->IsKindOf(&ntAvatarChatTask::m_RTTI)) {
		if (AfxGetTask()->GetActor("AvatarChat") && !m_bChatActive)
			AfxGetTask()->GetActor("AvatarChat") << MsgObject(0, 0x62, 0, 0, 0, 0, 0);
	}

	return true;
}

// user_info.cpp:4045
void FrUserInfoForm::OnTitleSeasonVisible(void) {
	m_seasonTab = SEASON_CURRENT;
	m_pCurSeasonBtn->SetStatus(FrButton::PRESSED);
	m_pTotalSeasonBtn->SetStatus(FrButton::NORMAL);

	SetContent(true);
	SetTrophyStatic();

	SetSpecListBoxItem(SEASON_CURRENT);
}

// user_info.cpp:4058
void FrUserInfoForm::SetRequestFriendData(const char *nick, unsigned long uid) {
	if (nick == NULL) {
		return;
	}

	if (m_addFriendDlg) {
		m_addFriendDlg->SetNick(nick);
		m_addFriendDlg->SetFindUID(uid);
	}
}

// user_info.cpp:4075
CUserInfo::CUserInfo(void) : m_dlg(NULL), m_guid(0xFFFFFFFF), m_uid(0xFFFFFFFF), m_bRequest(false), m_bHasInfo(false) {
}

// user_info.cpp:4079
CUserInfo::~CUserInfo(void) {
}

// user_info.cpp:4083
void CUserInfo::Clear(void) {
	if (m_dlg) {
		m_dlg->Clear();
	}
}

// user_info.cpp:4089
bool CUserInfo::IsVisible(void) {
	return m_dlg ? true : false;
}

// user_info.cpp:4109
void CUserInfo::SetInfo(unsigned long uid, unsigned long guid, bool bRequest, bool bSendPacket, bool bMsnControl,
		bool bControlOff, std::string nick) {
	if (!OnlinePlay())
		return;

	if (uid == 0xFFFFFFFF)
		return;

	if (m_dlg == NULL) {
		m_dlg = CreateForm<FrUserInfoForm>(g_pFresh->GetManager(), this, "userinfo", 0);
		if (m_dlg == NULL)
			return;

		m_dlg->Open(static_cast<FRESH_RESULT_CALLBACK>(&CUserInfo::OnUserInfoFormResult), 1);
		m_dlg->EnableDrag(false);
		m_dlg->SetTopmost(bControlOff);
		m_dlg->SetMsnControl(bMsnControl);
		m_dlg->SetControlOff(bControlOff);
		m_dlg->FindNextTopFocus(false);

		m_nick = nick;
	}

	m_guid = guid;
	m_uid = uid;
	m_bHasInfo = true;
	m_bRequest = bRequest;

	std::map<unsigned long, sUserInfoTime>::iterator it;
	if (WSingleton<CSharedDoc>::Instance()->FindUserInfoTimeUID(uid, it)) {
		if (!bRequest || g_CurrentTime - it->second.m_time < 60000) {
			if (MyUID() == m_uid) {
				std::map<unsigned int, sCharacterInfo>::iterator itChar = WSingleton<CSharedDoc>::Instance()->m_characterMap.find(WSingleton<CSharedDoc>::Instance()->m_myCharacterId);
				if (itChar != WSingleton<CSharedDoc>::Instance()->m_characterMap.end())
					it->second.m_userInfo.m_characterInfo = itChar->second;

				if (!((__rtti_obj = AfxGetTask()) != 0 && ((IObject*)__rtti_obj)->IsKindOf(&CGolfTask::m_RTTI)))
					it->second.m_userInfo.m_pang = WSingleton<CSharedDoc>::Instance()->m_myPang;

				if (IsLocalContent((localContentType_t)0x51)) {
					it->second.m_userInfo.m_value155 = WSingleton<CSharedDoc>::Instance()->m_myExp;
					it->second.m_userInfo.m_level = WSingleton<CSharedDoc>::Instance()->m_myLevel;
				}

				it->second.m_userInfo.m_equip = WSingleton<CSharedDoc>::Instance()->m_myEquip;
			}

			if (bMsnControl)
				it->second.m_userInfo.m_slot = 0xFFFF;
			m_dlg->SetInfo(&it->second);
			m_bHasInfo = false;
			return;
		}
	}

	if (bSendPacket) {
		if (nick.size()) {
			WSendPacket packet(GCT_REQUEST_USERINFO);
			packet.Encode4(uid);
			packet.Encode1(5);
			packet.Send(SEND_TO_DEFAULT);
		}
	}
}

// user_info.cpp:4214
void CUserInfo::Refresh(void) {
	if (m_dlg) {
		m_dlg->SetContent(true);
	}
}

// user_info.cpp:4220
void CUserInfo::Process(float elapsed) {
	if (m_bHasInfo) {
		SetInfo(m_uid, m_guid, m_bRequest, false, false, false, "");

		if (m_bHasInfo && WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_MSN)) {
			SetInfo(m_uid, m_guid, m_bRequest, false, true, false, "");
		}
	}

	if (m_dlg) {
		m_dlg->OnCustomProc(elapsed);
	}
}

// user_info.cpp:4237
bool CUserInfo::OnUserInfoFormResult(int ret, FrForm *pForm) {
	m_bHasInfo = false;
	m_guid = 0xFFFFFFFF;
	m_uid = 0xFFFFFFFF;
	m_dlg = 0;
	return true;
}

// user_info.cpp:4247
void CUserInfo::Close(void) {
	if (m_dlg) {
		m_dlg->Close(FrNONE, true);
	}
}

// user_info.cpp:4253
bool CUserInfo::IsRecvedData(unsigned char season) {
	if (m_dlg) {
		unsigned long flag = 0;
		if (season == 0) {
			flag = 2;
		} else if (season == 5) {
			flag = 4;
		}
		m_dlg->RecvedSeasonData(flag);
		if (m_dlg->IsTotalTab()) {
			if (!m_dlg->IsRecvSeason(2)) {
				m_dlg->RequestSeasonData(0);
				return false;
			}
			if (!m_dlg->IsRecvSeason(4)) {
				m_dlg->RequestSeasonData(5);
				return false;
			}
		}
		return true;
	}
	return false;
}

// user_info.cpp:4286
void FrUserInfoForm::OnRecAwardBackInit(int param) {
	m_pRecAwardBack = DYNAMIC_CAST(FrArea, param);
}

// user_info.cpp:4291
void FrUserInfoForm::OnGhostInit(PG_MSGARG param) {
	m_pGhostBtn = (FrButton*)((__rtti_obj = (void*)param) != 0 ? (((IObject*)__rtti_obj)->IsKindOf(&FrButton::m_RTTI) ? (void*)param : 0) : 0);
	m_pGhostBtn->Enable(false);

	// the ghost button is only offered in the lobby's room list, outside a room
	if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x800)
		return;

	if ((__rtti_obj = AfxGetTask()) != 0 && ((IObject*)__rtti_obj)->IsKindOf(&CLobbyTask::m_RTTI)) {
		if (stricmp("ROOMLIST", g_pFresh->GetManager()->GetLayoutID()) == 0 && WSingleton<CSharedDoc>::Instance()->m_mySlot == 0xFFFF) {
			m_pGhostBtn->Enable(true);
		}
	}
}

// user_info.cpp:4311
void FrUserInfoForm::OnGhostdUp(void) {
	FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), 0, "notify", 0);
	pForm->SetTimeLimit(5.0f);
	// "The service is being prepared."
	pForm->SetMessage("\xBC\xAD\xBA\xF1\xBD\xBA \xC1\xD8\xBA\xF1 \xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9.", false);
	pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
}

// user_info.cpp:4325
void FrUserInfoForm::OnBtn_RmrVisitorInit(PG_MSGARG param) {
	m_pRmrVisitorBtn = DYNAMIC_CAST(FrButton, param);
}

// user_info.cpp:4338
void FrUserInfoForm::OnBtn_RmrVisitorBtnUp(void) {
	m_anotherHand.WaitUntilWorkDone(9999);

	WSingleton<CSharedDoc>::Instance()->m_myRoomOwnerUID = m_userInfo.m_uid;

	if (MyUID() == WSingleton<CSharedDoc>::Instance()->m_myRoomOwnerUID) {
		WSingleton<CSharedDoc>::Instance()->SetRmrMode(1);
	} else {
		WSingleton<CSharedDoc>::Instance()->SetRmrMode(5);
	}

	CTaskManager* pTaskManager = CTaskManager::Instance(); (!pTaskManager ? 0 : pTaskManager->GetCurrentTask())->SendMsgToMainActor(MsgObject(0, 0x222, 0, 0, 0, 0, 0));
}

// user_info.cpp:4357
void FrUserInfoForm::RequestSeasonData(unsigned char season) {
	if (m_seasonPending == 0) {
		WSendPacket packet(GCT_REQUEST_USERINFO);
		packet.Encode4(m_userInfo.m_uid);
		packet.Encode1(season);
		packet.Send(SEND_TO_DEFAULT);

		m_seasonPending = 1;
	}
}

// user_info.cpp:4371
void FrUserInfoForm::RecvedSeasonData(unsigned long season) {
	m_recvSeasonFlags |= season;
	m_seasonPending = 0;
}

// user_info.cpp:4377
int FrUserInfoForm::IsTotalTab(void) {
	return m_seasonTab == 1;
}
