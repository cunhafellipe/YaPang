// Original: d:\build\custom\temp\code\client\projectg\ghostmain.cpp
#include "GhostMain.h"
#include "ghostdocument.h"
#include "ObjectFactory.h"
#include "actor.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "exhibition.h"
#include "parttidlist.h"
#include "partsinfo.h"
#include "playerdlg.h"
#include "SharedDoc.h"
#include "UccManager.h"
#include "bitmap.h"
#include "frlistbox.h"
#include "frarea.h"
#include "frbutton.h"
#include "client/Wangreal/include/wfont.h"
#include "client/Wangreal/include/wview.h"
#include "frdesktop.h"
#include "Packet.h"
#include "RankingDlg.h"
#include "user_info.h"
#include "InputManager.h"
#include "ghosthandler.h"
#include "MascotInfoDlg.h"
#include "ClientSetting.h"
#include "BongdariShopDlg.h"
#include "RecycleDlg.h"
#include "EquipDlg.h"
#include "commonutil.h"
#include "ghostmanager.h"
#include "TaskManager.h"
#include "cardsystem.h"
#include "mousecursor.h"
#include "type_convert.h"

// ghostmain.cpp:50 (run-time type and factory registration of CGhostMain)
const WRTTI CGhostMain::m_RTTI("CGhostMain", &CTaskMain::m_RTTI);

IObject *CGhostMainMakeInstance(void) {
	return new CGhostMain;
}

struct __impCGhostMain {
	__impCGhostMain(void) {
		ObjectFactory().AddObjectFunctor(CGhostMainMakeInstance, "CGhostMain");
		ActorFactory().AddActorClass("CGhostMain");
	}
};
static __impCGhostMain s_impCGhostMain;

// ghostmain.cpp:51
const sFRESH_MSGMAP *CGhostMain::GetMessageMap(void) const {
	return &_MsgMap;
}

sFRESH_MSGMAP CGhostMain::_MsgMap = { &CTaskMain::_MsgMap, &CGhostMain::_MsgEntries[0] };

// ghostmain.cpp:53
sFRESH_ENTRY CGhostMain::_MsgEntries[] = {
	{ "GHOST", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGhostMain::OnInitGhostMain },
	{ "GHOST", FRCMD_FINISH, FrFuncVV, (FRESH_PMSG)&CGhostMain::OnInitGhostFinish },
	{ "GHOST.start", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGhostMain::OnInitStart },
	{ "GHOST.start", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CGhostMain::OnDownStart },
	{ "GHOST.pet_user", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGhostMain::OnInitPlayerZone },
	{ "GHOST.pet_user", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&CGhostMain::OnDrawPlayerZone },
	{ "GHOST.pet_ghost", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGhostMain::OnInitGhostZone },
	{ "GHOST.pet_ghost", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&CGhostMain::OnDrawGhostZone },
	{ "GHOST.tip_ghost", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGhostMain::OnInitTipZone },
	{ "GHOST.tip_ghost", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&CGhostMain::OnDrawTipZone },
	{ "GHOST.user_stat", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGhostMain::OnInitAreaUserStat },
	{ "GHOST.user_stat", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&CGhostMain::OnDrawAreaUserStat },
	{ "GHOST.ghost_stat", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGhostMain::OnInitAreaGhostStat },
	{ "GHOST.ghost_stat", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&CGhostMain::OnDrawAreaGhostStat },
	{ "GHOST.select_course", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGhostMain::OnInitSelectCourse },
	{ "GHOST.select_course", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CGhostMain::OnDownSelectCourse },
	{ "GHOST.user_equip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGhostMain::OnInitUserEquip },
	{ "GHOST.user_equip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGhostMain::OnDrawUserEquip },
	{ "GHOST.user_equip", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CGhostMain::OnDownUserEquip },
	{ "GHOST.ghost_equip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGhostMain::OnInitGhostEquip },
	{ "GHOST.ghost_equip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CGhostMain::OnDrawGhostEquip },
	{ "GHOST.ubar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBarInit },
	{ "GHOST.ubar.back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_BackInit },
	{ "GHOST.ubar.back", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_BackUp },
	{ "GHOST.ubar.back", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.ubar.back", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.ubar.front", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_FrontInit },
	{ "GHOST.ubar.front", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_FrontUp },
	{ "GHOST.ubar.front", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.ubar.front", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.ubar.shop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ShopInit },
	{ "GHOST.ubar.shop", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ShopUp },
	{ "GHOST.ubar.shop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.ubar.shop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.ubar.myroom", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MyRoomInit },
	{ "GHOST.ubar.myroom", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MyRoomUp },
	{ "GHOST.ubar.myroom", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.ubar.myroom", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.ubar.itemstorage", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ItemStorageInit },
	{ "GHOST.ubar.itemstorage", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ItemStorageUp },
	{ "GHOST.ubar.itemstorage", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.ubar.itemstorage", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.ubar.gift", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GiftInit },
	{ "GHOST.ubar.gift", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GiftUp },
	{ "GHOST.ubar.gift", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.ubar.gift", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.ubar.cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_CookieInit },
	{ "GHOST.ubar.cookie", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_CookieUp },
	{ "GHOST.ubar.cookie", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.ubar.cookie", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.ubar.game", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GameInit },
	{ "GHOST.ubar.game", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GameUp },
	{ "GHOST.ubar.game", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.ubar.game", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.ubar.server", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ServerInit },
	{ "GHOST.ubar.server", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ServerUp },
	{ "GHOST.ubar.server", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.ubar.server", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.ubar.exit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ExitInit },
	{ "GHOST.ubar.exit", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ExitUp },
	{ "GHOST.ubar.exit", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.ubar.exit", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.ubar.tabbtn_b", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_TabBtnInit },
	{ "GHOST.ubar.tabbtn_b", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_TabBtnUp },
	{ "GHOST.utab", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderTab_Init },
	{ "GHOST.utab.magicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MagicBoxInit },
	{ "GHOST.utab.magicbox", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MagicBoxUp },
	{ "GHOST.utab.magicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.utab.magicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.utab.tikimagicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_TikiMagicBoxInit },
	{ "GHOST.utab.tikimagicbox", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_TikiMagicBoxUp },
	{ "GHOST.utab.tikimagicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.utab.tikimagicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.utab.scratch", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ScratchInit },
	{ "GHOST.utab.scratch", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ScratchUp },
	{ "GHOST.utab.scratch", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.utab.scratch", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.utab.bongdarishop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_BongdariShopInit },
	{ "GHOST.utab.bongdarishop", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_BongdariShopUp },
	{ "GHOST.utab.bongdarishop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.utab.bongdarishop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.utab.maket", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MaketInit },
	{ "GHOST.utab.maket", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MaketUp },
	{ "GHOST.utab.maket", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.utab.maket", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.utab.ranking", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_RankingInit },
	{ "GHOST.utab.ranking", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_RankingUp },
	{ "GHOST.utab.ranking", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.utab.ranking", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.utab.guild", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GuildInit },
	{ "GHOST.utab.guild", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GuildUp },
	{ "GHOST.utab.guild", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.utab.guild", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.utab.notice", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_NoticeInit },
	{ "GHOST.utab.notice", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_NoticeUp },
	{ "GHOST.utab.notice", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.utab.notice", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.obar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_Init },
	{ "GHOST.obar.title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_TitleInit },
	{ "GHOST.obar.site", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_SiteInit },
	{ "GHOST.obar.char", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharInit },
	{ "GHOST.obar.char", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_CharDown },
	{ "GHOST.obar.char", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharOwnerDraw },
	{ "GHOST.obar.caddie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieInit },
	{ "GHOST.obar.caddie", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_CaddieDown },
	{ "GHOST.obar.caddie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieOwnerDraw },
	{ "GHOST.obar.club", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubInit },
	{ "GHOST.obar.club", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_ClubDown },
	{ "GHOST.obar.club", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubOwnerDraw },
	{ "GHOST.obar.aztec", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecInit },
	{ "GHOST.obar.aztec", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_AztecDown },
	{ "GHOST.obar.aztec", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecOwnerDraw },
	{ "GHOST.obar.mascot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotInit },
	{ "GHOST.obar.mascot", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MascotDown },
	{ "GHOST.obar.mascot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotOwnerDraw },
	{ "GHOST.obar.char_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharSelInit },
	{ "GHOST.obar.char_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharSelOwnerDraw },
	{ "GHOST.obar.caddie_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieSelInit },
	{ "GHOST.obar.caddie_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieSelOwnerDraw },
	{ "GHOST.obar.club_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubSelInit },
	{ "GHOST.obar.club_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubSelOwnerDraw },
	{ "GHOST.obar.aztec_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecSelInit },
	{ "GHOST.obar.aztec_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecSelOwnerDraw },
	{ "GHOST.obar.mascot_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotSelInit },
	{ "GHOST.obar.mascot_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotSelOwnerDraw },
	{ "GHOST.obar.pang_cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_PangCookieInit },
	{ "GHOST.obar.pang_cookie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_PangCookieOwnerDraw },
	{ "GHOST.obar.onelinemsg_req", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineReqBtnInit },
	{ "GHOST.obar.onelinemsg_req", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_OneLineReqBtnUp },
	{ "GHOST.obar.onelineview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineViewInit },
	{ "GHOST.obar.onelineview", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineViewOwnerDraw },
	{ "GHOST.obar.spcard", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_SpecialCardInit },
	{ "GHOST.obar.spcard", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_SpecialCardUp },
	{ "GHOST.obar.spcard", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.obar.spcard", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.obar.messenger", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MessengerInit },
	{ "GHOST.obar.messenger", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MessengerUp },
	{ "GHOST.obar.messenger", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.obar.messenger", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.obar.myinfo", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MyInfoInit },
	{ "GHOST.obar.myinfo", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MyInfoUp },
	{ "GHOST.obar.myinfo", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.obar.myinfo", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.obar.option", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OptionInit },
	{ "GHOST.obar.option", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_OptionUp },
	{ "GHOST.obar.option", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	{ "GHOST.obar.option", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	{ "GHOST.obar.messenger_bubble", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleInit },
	{ "GHOST.obar.messenger_bubble", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleOwnerDraw },
	{ "GHOST.obar.messenger_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleEditInit },
	{ "GHOST.obar.messenger_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerAlarmBtnInit },
	{ "GHOST.obar.messenger_alarm", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnMessengerAlarmBtnUp },
	{ "GHOST.obar.gift_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerGiftAlarmBtnInit },
	{ "GHOST.obar.gift_alarm", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnMessengerGiftAlarmBtnUp },
	{ "GHOST.obar.tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarTooltipOwnerDraw },
	{ 0, FRCMD_NONE, FrFuncNone, 0 }
};

// ghostmain.cpp:257
std::string __stdcall GetIconName(const unsigned long &typeId) {
	std::string name("w_");
	IFF_ITEM_COMMON *item = ItemManager()->FindCommonItem(typeId);
	if (item)
		name += item->m_icon;
	else
		name = "w_caddie_no";
	return name;
}

// ghostmain.cpp:269
void __stdcall GetEquipIcons(std::vector<std::string> &icons, const sUserInfo &info) {
	icons.clear();

	icons.reserve(6);
	icons.assign(6, "");
	icons[0] = GetIconName(info.m_characterInfo.typeId);
	icons[1] = GetIconName(info.m_caddieInfo.typeId);
	icons[2] = GetIconName(info.m_clubSet.typeId);
	icons[3] = GetIconName(info.m_ballId);
	icons[4] = GetIconName(info.m_mascot.typeId);
	icons[5] = std::string("gameroom_equip_item_bn");
}

// ghostmain.cpp:296
CGhostMain::cImgBinder::sImgInfo::sImgInfo(const std::string &strImage, const WRect &rect)
	: m_strImage(strImage), m_rect(rect) {
}

// ghostmain.cpp:303
CGhostMain::cImgBinder::cImgBinder(int nReserve) {
	m_vecImage.reserve(nReserve);
}

// ghostmain.cpp:306
CGhostMain::cImgBinder::~cImgBinder(void) {}

// ghostmain.cpp:308
void CGhostMain::cImgBinder::AddImage(std::string strImage, const WPoint &pos) {
	const Bitmap *bitmap = g_pFresh->GetBitmap(strImage.c_str());
	if (bitmap)
		m_vecImage.push_back(sImgInfo(strImage, WRect(pos, WSize((float)bitmap->Width(), (float)bitmap->Height()))));
}

// ghostmain.cpp:317
WPoint CGhostMain::cImgBinder::GetPos(void) {
	WPoint pos(0.0f, 0.0f);
	if (m_vecImage.size() != 0)
		pos = WPoint(m_vecImage[0].m_rect.x, m_vecImage[0].m_rect.y);
	return pos;
}

// ghostmain.cpp:335
void CGhostMain::cImgBinder::SetPosX(float x) {
	float curX = 0.0f;
	if (m_vecImage.size() != 0)
		curX = m_vecImage[0].m_rect.x;

	for (std::vector<sImgInfo>::iterator it = m_vecImage.begin(); it != m_vecImage.end(); ++it)
		it->m_rect.x += x - curX;
}

// ghostmain.cpp:344
void CGhostMain::cImgBinder::AddPos(float dx, float dy) {
	std::vector<sImgInfo>::iterator it = m_vecImage.begin();
	for (; it != m_vecImage.end(); ++it) {
		it->m_rect.x += dx;
		it->m_rect.y += dy;
	}
}

// ghostmain.cpp:356
void CGhostMain::cImgBinder::AddPos(const WPoint &delta) {
	std::vector<sImgInfo>::iterator it = m_vecImage.begin();
	for (; it != m_vecImage.end(); ++it) {
		it->m_rect.x += delta.x;
		it->m_rect.y += delta.y;
	}
}

// ghostmain.cpp:368
void CGhostMain::cImgBinder::Render(unsigned char alpha) {
	FrGraphicInterface *gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi == 0)
		return;

	std::vector<sImgInfo>::iterator it = m_vecImage.begin();
	for (; it != m_vecImage.end(); ++it) {
		const Bitmap *bitmap = g_pFresh->GetBitmap(it->m_strImage.c_str());
		if (bitmap)
			gdi->DrawTexture(bitmap, it->m_rect, (alpha << 24) | 0xFFFFFF, 0);
	}
}

// ghostmain.cpp:393
CGhostMain::CGhostMain(void)
	: m_userPet(0), m_ghostPet(0), m_userParts(0), m_ghostParts(0), m_ghostAlpha(0), m_ghostVisible(false),
	  m_userStatImage(0), m_ghostStatImage(0), m_userStatAlpha(0), m_userStatStopX(0), m_ghostStatStopX(0), m_userStatSpeed(0),
	  m_ghostStatSpeed(0), m_showUserStat(false), m_statTime(0), m_statSpeedTime(0), m_selectedIcon(0),
	  m_equipAlpha(0), m_equipDlg(0), m_userStat(0), m_ghostStat(0), m_courseBitmap(0), m_tipImage(0) {
	Initialize();
}

// ghostmain.cpp:398
CGhostMain::~CGhostMain(void) {
}

// ghostmain.cpp:403
void CGhostMain::Initialize(void) {
	WSingleton<CSharedDoc>::Instance()->m_gameMode = 5;

	if (IsLocalContent((localContentType_t)0x59)) {
		WSendPacket packet(GCT_UCC_REQUEST);
		packet.Encode1(2);
		packet.Send(SEND_TO_DEFAULT);
	}

	memset(WSingleton<CSharedDoc>::Instance()->m_roomPlayers, 0, sizeof(WSingleton<CSharedDoc>::Instance()->m_roomPlayers));

	CCardManager::Instance()->CalcCardPeriodAndStatus();

	m_areas.reserve(5);
	m_areas.assign(5, (FrArea *)0);

	m_userParts = new CPartTidList;
	m_ghostParts = new CPartTidList;

	m_tipImage = new cImgBinder(2);
	m_tipImage->AddImage("ghost_main_tip1", WPoint(0.0f, 0.0f));
	m_tipImage->AddImage("ghost_main_tip2", WPoint(256.0f, 0.0f));
	m_tipImage->AddPos(200.0f, -120.0f);

	m_tipWait = 5.0f;
	m_tipHiding = false;
	m_userStatImage = new cImgBinder(2);
	m_userStatImage->AddImage("ghost_user_stat1", WPoint(0.0f, 0.0f));
	m_userStatImage->AddImage("ghost_user_stat2", WPoint(256.0f, 0.0f));
	m_userStatImage->AddPos(-(g_view->GetWidth() * 0.5f), g_view->GetHeight() - 162.0f - 77.0f);

	m_ghostStatImage = new cImgBinder(2);
	m_ghostStatImage->AddImage("ghost_ghost_stat1", WPoint(0.0f, 0.0f));
	m_ghostStatImage->AddImage("ghost_ghost_stat2", WPoint(256.0f, 0.0f));
	m_ghostStatImage->AddPos(g_view->GetWidth(), g_view->GetHeight() - 162.0f - 77.0f);

	m_userStatStopX = 17.0f;
	m_ghostStatStopX = g_view->GetWidth() - 129.0f - 256.0f - 17.0f;
	m_userStatSpeed = g_view->GetWidth() * 0.5f + 17.0f;
	m_ghostStatSpeed = 402.0f;

	m_userStat = BuildStatusBar(0);
	m_ghostStat = BuildStatusBar(1);
}

// ghostmain.cpp:499
CStatBar *CGhostMain::BuildStatusBar(int index) {
	sUserInfo &info = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index];

	if (IsLocalContent((localContentType_t)0x51)) {
		if (index == 1)
			WSingleton<CCardManager>::Instance()->m_characterInfo[1] = info.m_characterInfo;
		WSingleton<CCardManager>::Instance()->SetPlayerIndex(index);
	}

	CStatBar *statBar = new CStatBar(10.0f, 5.0f);
	CItemManager *pItemManager = ItemManager();
	unsigned char level[5] = { 0 }, penalty[5] = { 0 }, capacity[5] = { 0 };

	for (int i = 0; i < 5; i++) {
		status_t status = pItemManager->GetLevelWithPenalty((eLvlType)i, info.m_characterInfo.typeId,
			(const char *)&info.m_characterInfo.unknown6A, info.m_characterInfo.partTypeIds, info.m_characterInfo.auxParts.tids,
			info.m_clubSet.typeId, info.m_clubSet.statLevel, info.m_caddieInfo.typeId, info.m_level);
		level[i] = status.m_level;
		penalty[i] = status.m_penalty;
		capacity[i] = pItemManager->GetCapacity((eLvlType)i, info.m_characterInfo.typeId,
			(const char *)&info.m_characterInfo.unknown6A, info.m_characterInfo.partTypeIds,
			info.m_clubSet.typeId, info.m_clubSet.statLevel, info.m_caddieInfo.typeId, info.m_level);
	}

	statBar->SetDestTime(1.2f);
	for (int i = 0; i < 5; i++)
		statBar->SetRange((eLvlType)i, level[i], capacity[i], penalty[i]);
	return statBar;
}

// ghostmain.cpp:557
void CGhostMain::StatProcess(float dt) {
	m_userStat->Process(dt);
	m_ghostStat->Process(dt);
}

// ghostmain.cpp:564
CExhibition *CGhostMain::MakePet(FrArea *area, CPartTidList *parts, int playerIndex) {
	m_hand.Enter();
	if (area == 0) {
		m_hand.Leave();
		return 0;
	}

	WRect rect = area->GetRect();
	CExhibition *pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f + 15.0f, 0.35f, false);
	pet->SetArea(rect.w, rect.h);

	sUserInfo &info = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[playerIndex];
	sCharacterInfo *character = &info.m_characterInfo;

	switch (character->typeId) {
	case 0x4000005:
		pet->SetDistance(9.0f, true, false);
		break;
	case 0x4000000:
		pet->SetDistance(11.0f, true, false);
		break;
	default:
		pet->SetDistance(10.0f, true, false);
		break;
	}

	if (!parts->SetTids(character, 0xFF) || !parts->IsComboValid())
		parts->SetDefaultTids();

	pet->SetModel(*parts, character->auxParts.tids, 0, 3.0f, 0);

	if (info.m_eventFlags.eventFlag4)
		pet->AttachAngelWingFx();

	m_hand.Leave();

	return pet;
}

// ghostmain.cpp:606
void CGhostMain::HidePet(bool wait) {
	if (wait)
		m_hand.WaitUntilWorkDone(9999);

	if (m_userPet) { delete m_userPet; m_userPet = 0; }
	if (m_ghostPet) { delete m_ghostPet; m_ghostPet = 0; }
}

// ghostmain.cpp:614
void CGhostMain::OnPreLoadInit(void) {
	CTaskMain::OnPreLoadInit();
}

// ghostmain.cpp:618
void CGhostMain::OnLoad(void) {
	CTaskMain::OnLoad();
}

// ghostmain.cpp:622
void CGhostMain::OnInit(void) {
	CTaskMain::OnInit();
}

// ghostmain.cpp:627
void CGhostMain::OnDestroy(void) {
	m_areas.clear();

	if (m_userPet) { delete m_userPet; m_userPet = 0; }
	if (m_ghostPet) { delete m_ghostPet; m_ghostPet = 0; }
	if (m_userParts) { delete m_userParts; m_userParts = 0; }
	if (m_ghostParts) { delete m_ghostParts; m_ghostParts = 0; }

	if (m_tipImage) { delete m_tipImage; m_tipImage = 0; }
	if (m_userStatImage) { delete m_userStatImage; m_userStatImage = 0; }
	if (m_ghostStatImage) { delete m_ghostStatImage; m_ghostStatImage = 0; }

	if (m_userStat) { delete m_userStat; m_userStat = 0; }
	if (m_ghostStat) { delete m_ghostStat; m_ghostStat = 0; }

	CTaskMain::OnDestroy();
}

// ghostmain.cpp:646
void CGhostMain::OnProcess(float dt) {
	if (IsLocalContent((localContentType_t)0x57))
		RefreshAllUccClothes();

	OnProcTip(dt);

	if (m_userStatAlpha < 255.0f) {
		m_userStatAlpha += dt * 204.0f;
		if (m_userStatAlpha > 255.0f)
			m_userStatAlpha = 255.0f;
	}

	if (m_showUserStat) {
		m_userStat->Process(dt);
		m_ghostStat->Process(dt);
		if (m_equipAlpha < 255.0f) {
			m_equipAlpha += dt * 510.0f;
			if (m_equipAlpha > 255.0f)
				m_equipAlpha = 255.0f;
		}
	} else {
		if (m_statTime < 3.0f)
			m_statTime += dt;

		if (m_statSpeedTime < 4.0f)
			m_statSpeedTime += dt * 1.2f;

		if (m_userStatImage->GetPos().x < m_userStatStopX) {
			m_userStatImage->AddPos(m_userStatSpeed * m_statTime * m_statTime * dt, 0.0f);
			if (m_userStatImage->GetPos().x > m_userStatStopX)
				m_userStatImage->SetPosX(m_userStatStopX);
		}

		if (m_ghostStatImage->GetPos().x > m_ghostStatStopX) {
			m_ghostStatImage->AddPos(-(m_ghostStatSpeed * m_statSpeedTime * dt), 0.0f);
			if (m_ghostStatImage->GetPos().x < m_ghostStatStopX)
				m_ghostStatImage->SetPosX(m_ghostStatStopX);
		}

		if (m_userStatImage->GetPos().x >= m_userStatStopX && m_ghostStatImage->GetPos().x <= m_ghostStatStopX)
			m_showUserStat = true;
	}

	if (FrStaticCast<FrWnd>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop")))
		return;

	bool equipOpen = ((__rtti_obj = g_pFresh->GetManager()->GetDesktop()->FindChildByName("equip")) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&FrEquipDlg::m_RTTI) : false) != 0;
	bool recycleOpen = ((__rtti_obj = g_pFresh->GetManager()->GetDesktop()->FindChildByName("recycledlg")) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&FrRecycleDlg::m_RTTI) : false) != 0;

	if (g_input->GetDown("ESCAPE", true) && !equipOpen && !recycleOpen) {
		OnExit();
		return;
	}

	if (m_hand.IsWorkDone()) {
		if (m_userPet) {
			m_userPet->SetEnableRotate(!equipOpen && !recycleOpen);
			m_userPet->Process(dt, false);
		}

		if (m_ghostPet) {
			if (m_ghostAlpha < 255.0f) {
				m_ghostAlpha += dt * 127.5f;
				if (m_ghostAlpha > 255.0f)
					m_ghostAlpha = 255.0f;
			}

			if (!m_ghostVisible && m_ghostAlpha > 32.0f)
				m_ghostVisible = true;

			m_ghostPet->SetAlpha((unsigned char)m_ghostAlpha);

			if (!equipOpen && !recycleOpen)
				m_ghostPet->Process(dt, false);
		}
	}

	CTaskMain::OnProcess(dt);
}

// ghostmain.cpp:753
void CGhostMain::OnDisplay(void) {
}

// ghostmain.cpp:758
void CGhostMain::OnExit(void) {
	WSingleton<CSharedDoc>::Instance()->m_gameMode = 0;
	if (IsLocalContent((localContentType_t)0x59)) {
		WSendPacket packet(GCT_UCC_REQUEST);
		packet.Encode1(1);
		packet.Send(SEND_TO_DEFAULT);
	}

	WSingleton<CSharedDoc>::Instance()->m_mySlot = 0xFFFF;
	WSingleton<CSharedDoc>::Instance()->m_roomInfo = sRoomInfo();

	WSendPacket packet((int)GCT_GAME_GHOST);
	packet.Encode1(10);
	packet.Encode4(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[1].m_uid);
	packet.Encode1(m_exitFlag);
	packet.Send(SEND_TO_DEFAULT);

	CGhostManager::Instance()->ReleaseDocument();

	if (WSingleton<CSharedDoc>::Instance()->m_lobbyStateFlag) {
		CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);
		CTaskManager::Instance()->PostMsg(0, "Lobby", 0, (intptr_t)"ROOMLIST", 0, 0, 0);
	} else {
		CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);
		CTaskManager::Instance()->PostMsg(0, "Lobby", 0, (intptr_t)"TOPPAGE", 0, 0, 0);
	}
}

// ghostmain.cpp:795
void CGhostMain::OnInitGhostFinish(void) {
	m_titleArea->SetBgImg("title_ghost_main");
	m_pangCookieWnd->SetVisible(false);

	GetEquipIcons(m_userIcons, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0]);
	m_userEquip->AddItem(&m_userIcons[0]);
	m_userEquip->AddItem(&m_userIcons[1]);
	m_userEquip->AddItem(&m_userIcons[2]);
	m_userEquip->AddItem(&m_userIcons[3]);
	m_userEquip->AddItem(&m_userIcons[4]);
	m_userEquip->AddItem(&m_userIcons[5]);

	GetEquipIcons(m_ghostIcons, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[1]);
	m_ghostEquip->AddItem(&m_ghostIcons[0]);
	m_ghostEquip->AddItem(&m_ghostIcons[1]);
	m_ghostEquip->AddItem(&m_ghostIcons[2]);
	m_ghostEquip->AddItem(&m_ghostIcons[3]);
	m_ghostEquip->AddItem(&m_ghostIcons[4]);
	m_ghostEquip->AddItem(&m_ghostIcons[5]);

	m_barButton[2]->Enable(false);
	m_barButton[3]->Enable(false);
	m_barButton[16]->Enable(false);
	m_barButton[5]->Enable(false);

	WSendPacket packet((int)GCT_GAME_GHOST);
	packet.Encode1(1);
	packet.Encode4(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[1].m_uid);
	packet.EncodeStr(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[1].m_name);
	packet.Encode1(m_exitFlag);
	packet.Send(SEND_TO_DEFAULT);
}

// ghostmain.cpp:833
void CGhostMain::OnInitStart(PG_MSGARG) {
}

// ghostmain.cpp:838
void CGhostMain::OnDownStart(void) {
	if (FrStaticCast<FrWnd>(g_pFresh->GetDesktop()->FindChildByName("bongdari_shop")))
		return;

	CGhostDocument *doc = CGhostManager::Instance()->GetDocument();
	if (doc->IsInvalidate()) {
		CGhostHandler::Instance()->OpenNotifyDlg(8);
		return;
	}

	std::list<sSlotInfo> &players = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList;
	players.clear();

	sSlotInfo mySlot;
	memset(&mySlot, 0, sizeof(mySlot));
	sSlotInfo ghostSlot;
	memset(&ghostSlot, 0, sizeof(ghostSlot));

	mySlot.m_uid = WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().guid;
	mySlot.m_connection = 1;
	mySlot.m_level = WSingleton<CSharedDoc>::Instance()->m_myLevel;
	strcpy(mySlot.m_nickname, WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().nickname);
	players.push_back(mySlot);

	type_convert(doc->GetUserData(), &ghostSlot);
	players.push_back(ghostSlot);

	const GHOST_GAME_DATA *game = doc->GetGameData();
	SetCurMap(game->m_curMap);
	SetGameType(0);
	SetHoleType(game->m_gameKind);
	SetHoles(18);
	SetShotTimeLimit(0);
	SetGameTimeLimit(0);

	for (int i = 0; i < 18; i++) {
		WSingleton<CSharedDoc>::Instance()->m_holeRandom[i] = game->m_holeRandom[i];
		WSingleton<CSharedDoc>::Instance()->m_holePin[i] = (unsigned char)(WSingleton<CSharedDoc>::Instance()->m_holeRandom[i] % 3);
		WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] = game->m_holeOrder[i];
	}

	CMouseCursor::Instance()->SetMode(1);
	CMouseCursor::Instance()->SetActive(false);
	CTaskManager::Instance()->ChangeTask("CGolfTask", "CGolfDoc", false);

	WSendPacket packet((int)GCT_GAME_GHOST);
	packet.Encode1(2);
	packet.Send(SEND_TO_DEFAULT);
}

// ghostmain.cpp:895
void CGhostMain::OnInitPlayerZone(PG_MSGARG param) {
	m_areas[0] = DYNAMIC_CAST(FrArea, param);

	FrArea *area = m_areas[0];
	area->SetVisible(true);

	m_userPet = MakePet(area, m_userParts, 0);
}

// ghostmain.cpp:906
void CGhostMain::OnDrawPlayerZone(void) {
	FrWnd *shop = g_pFresh->GetDesktop()->FindChildByName("bongdari_shop");
	if (FrStaticCast<FrWnd>(shop))
		return;

	if (!m_hand.IsWorking(0)) {
		if (CRankingInfo::Instance()->GetDlg() == 0 && USERINFODLG() == 0) {
			m_hand.Enter();
			if (m_userPet)
				m_userPet->Display(0.0f, 0.0f);
			m_hand.Leave();
		}
	}
}

// ghostmain.cpp:922
void CGhostMain::OnInitSelectCourse(PG_MSGARG param) {
	FrButton *button = DYNAMIC_CAST(FrButton, param);
	CGhostDocument *doc = CGhostManager::Instance()->GetDocument();
	if (doc == 0)
		return;

	IFF_STRUCT::sCourse *course = ItemManager()->FindCourse(0x28000000 | doc->GetGameData()->m_curMap);
	if (course == 0)
		return;

	std::string name("mid_");
	name += course->m_icon;
	m_courseBitmap = g_pFresh->GetBitmap(name.c_str());
	m_courseRect = button->GetRect();

	m_courseRect.x += abs((int)((m_courseBitmap->Width() - m_courseRect.w) * 0.5f));
	m_courseRect.y += abs((int)((m_courseBitmap->Height() - m_courseRect.h) * 0.5f));
	m_courseRect.w = (float)m_courseBitmap->Width();
	m_courseRect.h = (float)m_courseBitmap->Height();
}

// ghostmain.cpp:945
void CGhostMain::OnDownSelectCourse(void) {
}

// ghostmain.cpp:951
void CGhostMain::OnInitUserEquip(PG_MSGARG param) {
	m_userEquip = DYNAMIC_CAST(FrListBox, param);
}

// ghostmain.cpp:957
void CGhostMain::OnDrawUserEquip(PG_MSGARG param) {
	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();
	FrListItem *pItem = (FrListItem *)param;
	if (pItem == 0 || pGDI == 0)
		return;

	const Bitmap *icon = g_pFresh->GetBitmap(((std::string *)pItem->pData)->c_str());
	if (icon == 0)
		return;

	WRect rect(pItem->pos.x, pItem->pos.y, (float)icon->Width(), (float)icon->Height());

	pGDI->DrawTexture(icon, rect, ((unsigned char)m_equipAlpha << 24) | 0xFFFFFF, 0);

	if (rect.IsInRect(WPoint(g_input->GetMousePoint().x, g_input->GetMousePoint().y))) {
		const Bitmap *frame = g_pFresh->GetBitmap("select_frame");
		if (frame)
			pGDI->DrawTexture(frame, WRect(rect.x - 3.0f, rect.y - 3.0f, rect.w + 6.0f, rect.h + 6.0f), 0xFFFFFFFF, 0);
	}
}

// ghostmain.cpp:983
void CGhostMain::OnDownUserEquip(void) {
	if (!m_showUserStat)
		return;

	FrListItem *pItem = m_userEquip->GetSelected();
	if (pItem == 0)
		return;

	if (pItem->idx == 5) {
		FrEquipDlg *dlg = CreateForm<FrEquipDlg>(g_pFresh->GetManager(), this, "equip", 0);
		if (dlg) {
			dlg->Init(WSingleton<CSharedDoc>::Instance()->m_equipItem, true);
			WRect rect = dlg->GetRect();
			rect.x = g_view->GetWidth() - rect.w - 17.0f;
			rect.y = (g_view->GetHeight() - rect.h) * 0.5f;
			dlg->SetRect(rect);
			dlg->EnableDrag(false);
			dlg->Open((FRESH_RESULT_CALLBACK)&CGhostMain::CallBackEquipItemResult, 3);
		}
		return;
	}
	{
		unsigned int count = 1;
		switch (pItem->idx) {
		case 1:
			count = WSingleton<CSharedDoc>::Instance()->m_caddieMap.size();
			break;
		case 4:
			if (WSingleton<CSharedDoc>::Instance()->m_myMascotMap.size() != 0)
				break;
			return;
		}
		if (count == 0)
			return;

		FrSubEquipBar *bar = CreateForm<FrSubEquipBar>(g_pFresh->GetManager(), this, "EQUIPBAR_SUB", 0);
		if (bar == 0)
			return;

		m_selectedIcon = pItem;

		int category = 1;
		WPoint pt = pItem->pos;
		switch (pItem->idx) {
		case 4: category = 0x10; break;
		case 1: category = 7; break;
		case 0: category = 1; break;
		case 2: category = 4; break;
		case 3: category = 5; break;
		}

		bar->Open((FRESH_RESULT_CALLBACK)&CGhostMain::OnResultSubEquipBar,
			(FrSubEquipBar::eCategory)category, pt, (FrSubEquipBar::eSubType)1);

		WRect rect = bar->GetRect();
		rect.y -= rect.h;

		WView *view = g_view;

		if (rect.x + rect.w + 5.0f > view->GetWidth())
			rect.x = view->GetWidth() - rect.w - 5.0f;
		bar->SetRect(rect);
	}
}

// ghostmain.cpp:1063
bool CGhostMain::OnResultSubEquipBar(int result, FrForm *form) {
	if (result != 1)
		return true;

	FrSubEquipBar *bar = (FrSubEquipBar *)form;
	IFF_ITEM_COMMON *item = bar->GetResult();
	if (item != NULL) {
		unsigned long typeId = item->m_typeId;
		unsigned long itemGroup = typeId >> 26;
		if (itemGroup == 5)
			SendPacketEquipResult((PANGYA_ITEM_GROUP)itemGroup, typeId);
		else
			SendPacketEquipResult((PANGYA_ITEM_GROUP)itemGroup, bar->GetLastSelectedGuid());
	} else {
		switch (bar->GetCategory()) {
		case 0x10: {
			SendPacketEquipResult((PANGYA_ITEM_GROUP)0x10, 0);
			std::string *icon = (std::string *)m_selectedIcon->pData;
			unsigned long none = 0;
			*icon = GetIconName(none);
			break;
		}
		case 7: {
			SendPacketEquipResult((PANGYA_ITEM_GROUP)7, 0);
			std::string *icon = (std::string *)m_selectedIcon->pData;
			unsigned long none = 0;
			*icon = GetIconName(none);
			break;
		}
		}
	}
	return true;
}

// ghostmain.cpp:1108
void CGhostMain::SetUserEquip(PANGYA_ITEM_GROUP group, unsigned long id) {
	CSharedDoc *doc = WSingleton<CSharedDoc>::Instance();
	sUserInfo *info = &WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0];
	switch (group) {
	case 1: {
		std::map<unsigned int, sCharacterInfo>::iterator it = doc->m_characterMap.find(id);
		if (it == doc->m_characterMap.end())
			return;

		doc->m_myCharacterId = id;
		WSingleton<CSharedDoc>::Instance()->BuildMyPartTidList(it->second);

		info->m_characterId = id;
		info->m_characterInfo = it->second;
		UserPetRebuild();
		if (m_userStat) { delete m_userStat; m_userStat = 0; }
		m_userStat = BuildStatusBar(0);

		m_userIcons[0] = GetIconName(info->m_characterInfo.typeId);
		break;
	}
	case 16: {
		std::map<unsigned int, sMascotInfo>::iterator it = doc->m_myMascotMap.find(id);
		if (it != doc->m_myMascotMap.end()) {
			doc->m_myMascotId = id;
			info->m_mascotId = id;
			info->m_mascot = it->second;
		} else {
			doc->m_myMascotId = 0;
			memset(&info->m_mascot, 0, sizeof(info->m_mascot));
		}

		if (m_userStat) { delete m_userStat; m_userStat = 0; }
		m_userStat = BuildStatusBar(0);
		m_userIcons[4] = GetIconName(info->m_mascot.typeId);
		break;
	}
	case 7: {
		std::map<unsigned int, sCaddieInfo>::iterator it = doc->m_caddieMap.find(id);
		if (it != doc->m_caddieMap.end()) {
			doc->m_myCaddieId = id;
			info->m_equipHead = id;
			info->m_caddieInfo = it->second;
		} else {
			doc->m_myCaddieId = 0;
			memset(&info->m_caddieInfo, 0, sizeof(info->m_caddieInfo));
		}

		if (m_userStat) { delete m_userStat; m_userStat = 0; }
		m_userStat = BuildStatusBar(0);
		m_userIcons[1] = GetIconName(info->m_caddieInfo.typeId);
		break;
	}
	case 5:
		doc->m_myBallId = id;
		info->m_ballId = id;
		m_userIcons[3] = GetIconName(id);
		break;
	case 4: {
		std::map<unsigned int, sItemInfo>::iterator it = doc->m_myItemMap.find(id);
		if (it == doc->m_myItemMap.end())
			return;

		doc->m_myClubId = id;

		info->m_clubId = id;

		info->m_clubSet.id = it->second.m_serial;
		memcpy(info->m_clubSet.statLevel, it->second.m_clubStatLevel, sizeof(info->m_clubSet.statLevel));
		info->m_clubSet.typeId = it->second.m_typeId;

		if (m_userStat) { delete m_userStat; m_userStat = 0; }
		m_userStat = BuildStatusBar(0);
		m_userIcons[2] = GetIconName(info->m_clubSet.typeId);
		break;
	}
	}
}

// ghostmain.cpp:1199
void CGhostMain::UserPetRebuild(void) {
	if (m_userPet) { delete m_userPet; m_userPet = 0; }
	if (m_userParts) { delete m_userParts; m_userParts = 0; }

	FrArea *area = m_areas[0];
	m_userParts = new CPartTidList;
	m_userPet = MakePet(area, m_userParts, 0);
}

// ghostmain.cpp:1209
bool CGhostMain::CallBackEquipItemResult(int result, FrForm *form) {
	if (form != 0) {
		m_equipDlg = form;
		unsigned long *equipItem = WSingleton<CSharedDoc>::Instance()->m_equipItem;
		memcpy(equipItem, ((FrEquipDlg *)form)->GetMyItemSlot(), sizeof(WSingleton<CSharedDoc>::Instance()->m_equipItem));
		// the first room player is the local one
		memcpy(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_equipItem, equipItem, sizeof(WSingleton<CSharedDoc>::Instance()->m_equipItem));
	}

	m_equipDlg = 0;
	this << MsgObject(0, 1, 0, 0, 0, 0, 0);
	WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
	packet.Encode1(2);
	packet.EncodeBuffer(WSingleton<CSharedDoc>::Instance()->m_equipItem, sizeof(WSingleton<CSharedDoc>::Instance()->m_equipItem));
	packet.Send(SEND_TO_DEFAULT);

	return true;
}

// ghostmain.cpp:1231
bool CGhostMain::IsFocusToMain(void) {
	if (FrStaticCast<FrWnd>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop")))
		return false;

	return true;
}

// ghostmain.cpp:1241
void CGhostMain::OnInitGhostEquip(PG_MSGARG param) {
	m_ghostEquip = DYNAMIC_CAST(FrListBox, param);
}

// ghostmain.cpp:1247
void CGhostMain::OnDrawGhostEquip(PG_MSGARG param) {
	if (!m_showUserStat)
		return;

	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();
	FrListItem *pItem = (FrListItem *)param;
	if (pItem == 0 || pGDI == 0)
		return;

	const Bitmap *icon = g_pFresh->GetBitmap(((std::string *)pItem->pData)->c_str());
	const Bitmap *hide = g_pFresh->GetBitmap("ghost_equipicon_hide");
	if (icon)
		pGDI->DrawTexture(icon, WRect(pItem->pos.x, pItem->pos.y, (float)icon->Width(), (float)icon->Height()), ((unsigned char)m_equipAlpha << 24) | 0xFFFFFF, 0);

	if (hide)
		pGDI->DrawTexture(hide, WRect(pItem->pos.x, pItem->pos.y, (float)icon->Width(), (float)icon->Height()), ((unsigned char)m_equipAlpha << 24) | 0xFFFFFF, 0);

	m_titleFont->Print(g_view, m_courseRect.x + 40.0f, m_courseRect.y - 40.0f, MakeStr("-%d", m_ghostScore), 0, ((unsigned char)m_equipAlpha << 24) | 0xFFFFFF, 0);

	float width = m_titleFont->Print(g_view, m_courseRect.x + 40.0f, m_courseRect.y - 25.0f, MakeStr("%d", m_ghostPang), 0, 0xFFFFFFFF, 0);

	const Bitmap *pang = g_pFresh->GetBitmap("ghost_pang");
	if (pang)
		pGDI->DrawTexture(pang, WRect(width + m_courseRect.x + 50.0f, m_courseRect.y - 25.0f, (float)pang->Width(), (float)pang->Height()), ((unsigned char)m_equipAlpha << 24) | 0xFFFFFF, 0);

	if (m_courseBitmap)
		pGDI->DrawTexture(m_courseBitmap, m_courseRect, 0xFFFFFFFF, 0);
}

// ghostmain.cpp:1297
void CGhostMain::OnInitGhostZone(PG_MSGARG param) {
	m_areas[1] = DYNAMIC_CAST(FrArea, param);

	FrArea *area = m_areas[1];
	area->SetVisible(true);
	m_ghostPet = MakePet(area, m_ghostParts, 1);
	m_ghostPet->SetAlpha(0);
}

// ghostmain.cpp:1308
void CGhostMain::OnDrawGhostZone(void) {
	if (((__rtti_obj = g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop")) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&FrBongdariShopDlg::m_RTTI) : false)
		|| ((__rtti_obj = g_pFresh->GetManager()->GetDesktop()->FindChildByName("recycledlg")) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&FrRecycleDlg::m_RTTI) : false)
		|| ((__rtti_obj = g_pFresh->GetManager()->GetDesktop()->FindChildByName("equip")) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&FrEquipDlg::m_RTTI) : false))
		return;

	if (m_hand.IsWorking(0))
		return;
	if (CRankingInfo::Instance()->GetDlg() == 0 && USERINFODLG() == 0) {
		m_hand.Enter();
		if (m_ghostPet && m_ghostVisible) {
			m_ghostPet->Display(0.0f, 0.0f);
		}
		m_hand.Leave();
	}
}

// ghostmain.cpp:1336
void CGhostMain::OnInitTipZone(PG_MSGARG param) {
	m_areas[4] = DYNAMIC_CAST(FrArea, param);

	m_tipAlpha = 0.0f;
}

// ghostmain.cpp:1344
void CGhostMain::OnDrawTipZone(void) {
	m_tipImage->Render((unsigned char)m_tipAlpha);
}

// ghostmain.cpp:1350
void CGhostMain::OnProcTip(float dt) {
	if (!m_showUserStat)
		return;

	float y = m_tipImage->GetPos().y;
	if (!m_tipHiding) {
		if (y < 30.0f)
			m_tipImage->AddPos(WPoint(0.0f, dt * 240.0f));

		if (m_tipAlpha < 240.0f) {
			m_tipAlpha += dt * 240.0f;
			if (m_tipAlpha > 240.0f)
				m_tipAlpha = 240.0f;
		}

		if (m_tipAlpha == 240.0f && m_tipImage->GetPos().y >= 30.0f) {
			m_tipWait -= dt;
			if (m_tipWait < 0.0f) {
				m_tipWait = 5.0f;
				m_tipHiding = true;
			}
		}
	} else {
		if (y > -120.0f)
			m_tipImage->AddPos(WPoint(0.0f, dt * -240.0f));

		if (m_tipAlpha > 0.0f) {
			m_tipAlpha -= dt * 0.0f;
			if (m_tipAlpha < 0.0f)
				m_tipAlpha = 0.0f;
		}
	}
}

// ghostmain.cpp:1399
void CGhostMain::OnInitAreaUserStat(PG_MSGARG param) {
	m_areas[2] = DYNAMIC_CAST(FrArea, param);
}

// ghostmain.cpp:1405
void CGhostMain::OnDrawAreaUserStat(void) {
	m_userStatImage->Render((unsigned char)m_userStatAlpha);
	if (!m_showUserStat)
		return;

	WRect rect = m_areas[2]->GetRect();
	m_userStat->Render(WPoint(rect.x + 86.0f, rect.y + 86.0f));

	FrGraphicInterface *gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi == 0)
		return;

	CSharedDoc *doc = WSingleton<CSharedDoc>::Instance();
	int level = doc->m_roomPlayers[0].m_level + 1;
	const Bitmap *bitmap = g_pFresh->GetBitmap(MakeStr("level_%03d", level < 0x47 ? level : 0x47));
	if (bitmap == 0)
		return;

	gdi->DrawTexture(bitmap, WRect(rect.x + 30.0f, rect.y + 3.0f, (float)bitmap->Width(), (float)bitmap->Height()), 0xFFFFFFFF, 0);

	gdi->SetTextColor(0xFFFFFFFF, 0xFF000000);
	gdi->SetTextStyle(2);

	g_pFresh->GetManager()->PrintText(WPoint((float)bitmap->Width() + rect.x + 35.0f, rect.y + 5.0f), 0, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_name, -1.0f, 0xFFFFFFFF);
	gdi->SetTextStyle(0);
	gdi->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
}

// ghostmain.cpp:1435
void CGhostMain::OnInitAreaGhostStat(PG_MSGARG param) {
	m_areas[3] = DYNAMIC_CAST(FrArea, param);
}

// ghostmain.cpp:1441
void CGhostMain::OnDrawAreaGhostStat(void) {
	m_ghostStatImage->Render((unsigned char)m_userStatAlpha);
	if (g_pFresh->GetManager()->GetGDI() == 0)
		return;
	if (!m_showUserStat)
		return;

	WRect rect = m_areas[3]->GetRect();
	m_ghostStat->Render(WPoint(rect.x + 210.0f, rect.y + 86.0f));

	FrGraphicInterface *gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi == 0)
		return;

	CSharedDoc *doc = WSingleton<CSharedDoc>::Instance();
	int level = doc->m_roomPlayers[1].m_level + 1;
	const Bitmap *bitmap = g_pFresh->GetBitmap(MakeStr("level_%03d", level < 0x47 ? level : 0x47));
	if (bitmap == 0)
		return;

	gdi->DrawTexture(bitmap, WRect(rect.x + 230.0f, rect.y + 3.0f, (float)bitmap->Width(), (float)bitmap->Height()), 0xFFFFFFFF, 0);

	gdi->SetTextColor(0xFFFFFFFF, 0xFF000000);
	gdi->SetTextStyle(2);

	g_pFresh->GetManager()->PrintText(WPoint((float)bitmap->Width() + rect.x + 235.0f, rect.y + 5.0f), 0, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[1].m_name, -1.0f, 0xFFFFFFFF);
	gdi->SetTextStyle(0);
	gdi->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
}

// ghostmain.cpp:1477
void CGhostMain::OnInitGhostMain(PG_MSGARG) {
	SetTimeVariableBGM();
	TurnOff3DBackground();
}

// ghostmain.cpp:1485
void CGhostMain::HandleMsg(const MsgObject &msg) {
	switch (msg.m_msg) {
	case 0: {
		if (g_pFresh == 0)
			return;

		const char *layout = (const char *)msg.m_param1;
		g_pFresh->OpenLayout(layout, this, true);
		switch (msg.m_param2) {
		case 1:
			CGhostHandler::Instance()->OpenNotifyDlg(5);
			break;
		}
		break;
	}

	case 73:
		SetUserEquip((PANGYA_ITEM_GROUP)1, ((const sCharacterInfo *)msg.m_param2)->id);
		break;

	case 74: {
		const sCaddieInfo *caddie = (const sCaddieInfo *)msg.m_param2;
		SetUserEquip((PANGYA_ITEM_GROUP)7, caddie->id);
		break;
	}

	case 75: {
		const sItemInfo *clubSet = (const sItemInfo *)msg.m_param2;
		SetUserEquip((PANGYA_ITEM_GROUP)4, clubSet->m_serial);
		break;
	}

	case 76:
		SetUserEquip((PANGYA_ITEM_GROUP)5, msg.m_param2);
		break;

	case 78: {
		const sMascotInfo *mascot = (const sMascotInfo *)msg.m_param2;
		if (mascot->pcBang)
			COption::Instance()->mSetPCBangMascotMsg(mascot->message);
		SetUserEquip((PANGYA_ITEM_GROUP)16, mascot->id);
		break;
	}

	default:
		HandleMsgCommon(msg);
		break;
	}
}

// ghostmain.cpp:1546
void CGhostMain::SendPacketEquipResult(PANGYA_ITEM_GROUP group, unsigned long typeId) {
	WSendPacket packet(GCT_LOBBY_USERINFO_CHANGED);
	unsigned char kind = 0;
	switch (group) {
	case 1: kind = 4; break;
	case 7: kind = 1; break;
	case 4: kind = 3; break;
	case 5: kind = 2; break;
	case 16: kind = 5; break;
	}
	packet.Encode1(kind);
	packet.Encode4(typeId);
	packet.Send(SEND_TO_DEFAULT);
}

// ghostmain.cpp:1563
void CGhostMain::RefreshAllUccClothes(void) {
	sUccRefreshItem item;

	for (int i = 0; i < 10; i++) {
		if (!UccManager()->GetRefreshItem(item))
			return;

		if (m_userPet && m_userPet->GetPetFrame() && m_userParts) {
			unsigned char slot = (unsigned char)((item.m_typeId >> 13) & 0x1F);
			if (m_userParts->m_partTids[slot] == item.m_typeId) {
				unsigned long serial = m_userParts->m_partIds[slot];

				sUccClothes *pClothes = UccManager()->FindClothes(serial);

				if (pClothes && strcmpi(pClothes->m_uccKey, item.m_uccKey) == 0) {
					m_userPet->GetPetFrame()->SetPart(m_userParts, pClothes->m_typeId, pClothes->m_serial);
					m_userPet->GetPetFrame()->SetPart(m_userParts, pClothes->m_typeId, pClothes->m_serial);
				}
			}
		}

		if (m_ghostPet && m_ghostPet->GetPetFrame() && m_ghostParts) {
			unsigned char slot = (unsigned char)((item.m_typeId >> 13) & 0x1F);
			if (m_ghostParts->m_partTids[slot] == item.m_typeId) {
				unsigned long serial = m_ghostParts->m_partIds[slot];

				sUccClothes *pClothes = UccManager()->FindClothes(serial);

				if (pClothes && strcmpi(pClothes->m_uccKey, item.m_uccKey) == 0) {
					m_ghostPet->GetPetFrame()->SetPart(m_ghostParts, pClothes->m_typeId, pClothes->m_serial);
					m_ghostPet->GetPetFrame()->SetPart(m_ghostParts, pClothes->m_typeId, pClothes->m_serial);
				}
			}
		}
	}
}
