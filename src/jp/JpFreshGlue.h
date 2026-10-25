// The JP 983 Fresh window glue the JP game code reaches (names ours): the JP
// FrWnd fields the JP dialogs set inline, mapped to the port's KR FrWnd
// members, and the JP-only Fresh form members the JP dialogs call through
// a free function (JpFreshGlue.cpp).
//
// The JP FrWnd holds FrModalDim as a base at +0x8 (size 0x14; frwnd.h), so
// every KR FrWnd member from m_nFlags (KR +0x8) on sits 0x14 further on in
// the JP object: JP +0x1c is m_nFlags, JP +0x44 is m_dwStyle (the JP
// constructor 0xbe8bf0 clears both; its std::string members at +0x48 and
// +0x64 are the KR m_wndText +0x34 and m_wndName +0x50).
#ifndef PG_JP_JPFRESHGLUE_H
#define PG_JP_JPFRESHGLUE_H

#include <stdint.h>

class FrChangeRoomInfoDlg;
class FrWnd;
class JpGameRoomOwned;

// OnMapInit's (0x503970) inline JP FrWnd +0x44 |= 0x200: the KR
// FWS_NODBLCLICK style bit (FrWnd::UseDblClick(false)).
void JpFrWnd_SetStyleFlag200(FrWnd* wnd);
// UpdateTitle's (0x508a50, at 0x508e4c) inline JP FrWnd +0x1c &= ~1: the KR
// m_nFlags bit 0, the tooltip switch (FrWnd::EnableToolTip(false)).
void JpFrWnd_ClearFlag1cBit0(FrWnd* wnd);

// 0xacab70, the JP-only FrChangeRoomInfoDlg member (KR ChangeRoomInfoDlg.h,
// JP_PAKS SetSingleRoom): the single room's options; the font is the
// room's JpGameRoomOwned digit font.
void JpChangeRoomInfoDlg_SetSingleRoom(FrChangeRoomInfoDlg* dlg, bool holeMode4, bool holecupFixed,
	JpGameRoomOwned* font);

// The JP CUserInfo (instance pointer 0xe74da4): whether it exists (the
// KR WSingleton<CUserInfo> instance).
bool JpUserInfo_IsCreated(void);
// OnUserRBtnUp's tail 0x5098e5-0x50994b (JpGameRoomDlg.cpp): opens the
// user's information from the JP CSharedDoc user map +0x109d4, which no KR
// member holds yet: a port placeholder (G7) that does nothing.
void JpRoomUser_OpenUserInfo(uint32_t userId);
// 0x838400 on the JP instance 0xe74da0 (class not identified): a port
// placeholder (G7) returning 9999, the JP value for a course it lacks.
uint32_t JpCourseTable_GetValue(int course);

#endif // PG_JP_JPFRESHGLUE_H
