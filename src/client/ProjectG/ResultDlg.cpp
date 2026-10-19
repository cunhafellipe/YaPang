// Original: d:\build\custom\temp\code\client\projectg\resultdlg.cpp
#include <atlbase.h>	// stand-in for the precompiled header: _AtlWinModule initializer
#include "LevelUpItemDlg.h"
#include "ResultDlg.h"
#include "InputManager.h"
#include "ObjectFactory.h"
#include "mousecursor.h"
#include "SharedDoc.h"
#include "shared/Localize.h"
#include "fresh.h"
#include "frarea.h"
#include "frbutton.h"
#include "fredit.h"
#include "frgaugebar.h"
#include "frlistbox.h"
#include "frstatic.h"
#include "user_info.h"
#include "commonutil.h"
#include "NetResourceManager.h"
#include "ScoreDlg.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "bitmap.h"
#include "wview.h"
#include "wresrcmng.h"
#include "wfont.h"
#include "woverlay.h"
#include "exhibition.h"
#include "parttidlist.h"
#include "ProjectG.h"
#include "Treasure.h"
#include "TreasureDlg.h"
#include "UccManager.h"
#include "Intrusion.h"
#include "GMToolkit.h"
#include <string>
#include <math.h>
#include <stdio.h>
#include <string.h>

// resultdlg.cpp:33 (run-time type and factory registration of FrResultDlg)
const WRTTI FrResultDlg::m_RTTI("FrResultDlg", &FrForm::m_RTTI);

IObject* FrResultDlgMakeInstance(void) {
	return new FrResultDlg;
}

struct __sFrResultDlg {
	__sFrResultDlg(void) {
		ObjectFactory().AddObjectFunctor(FrResultDlgMakeInstance, "FrResultDlg");
	}
};
static __sFrResultDlg s_registerFrResultDlg;

// resultdlg.cpp:35
const sFRESH_MSGMAP* FrResultDlg::GetMessageMap(void) const {
	return &FrResultDlg::_MsgMap;
}

sFRESH_MSGMAP FrResultDlg::_MsgMap = { &FrForm::_MsgMap, &FrResultDlg::_MsgEntries[0] };

sFRESH_ENTRY FrResultDlg::_MsgEntries[] = {
	// resultdlg.cpp:37
	{ "stage", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultDlg::OnStageInit },
	// resultdlg.cpp:38
	{ "stage", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrResultDlg::OnStageOwnerDraw },
	// resultdlg.cpp:39
	{ "trophy", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultDlg::OnTrophyInit },
	// resultdlg.cpp:40
	{ "id", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultDlg::OnIdInit },
	// resultdlg.cpp:41
	{ "id", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultDlg::OnIdOwnerDraw },
	// resultdlg.cpp:42
	{ "match", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultDlg::OnMatchInit },
	// resultdlg.cpp:43
	{ "smatch", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultDlg::OnMatchStaticInit },
	// resultdlg.cpp:44
	{ "score", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultDlg::OnScoreInit },
	// resultdlg.cpp:45
	{ "sscore", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultDlg::OnScoreStaticInit },
	// resultdlg.cpp:46
	{ "pang", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultDlg::OnPangInit },
	// resultdlg.cpp:47
	{ "bonus", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultDlg::OnBonusInit },
	// resultdlg.cpp:48
	{ "sbonus", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultDlg::OnBonusStaticInit },
	// resultdlg.cpp:49
	{ "getitem", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultDlg::OnGetItemInit },
	// resultdlg.cpp:50
	{ "getitem", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrResultDlg::OnGetItemOwnerDraw },
	// resultdlg.cpp:51
	{ "level", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultDlg::OnLevelInit },
	// resultdlg.cpp:52
	{ "exp", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultDlg::OnExpInit },
	// resultdlg.cpp:53
	{ "exp_gb", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultDlg::OnExpGaugeBarInit },
	// resultdlg.cpp:54
	{ "levelup", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultDlg::OnLevelUpInit },
	// resultdlg.cpp:56
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// The title font's texture list (no public symbol).
extern "C" {
static char* data_ab7900[] = { "[bigfont.jpg", "[font_wind.jpg" };
}

// resultdlg.cpp:61
FrResultDlg::FrResultDlg(void) {
	m_elapsed = 0;
	m_pStageArea = 0;
	m_pLevelArea = 0;
	m_pExpStatic = 0;
	m_pExpGaugeBar = 0;
	m_pExhibition = 0;
	m_pLevelUpArea = 0;
	m_pPartTidList = 0;
	memset(&m_winnerData, 0, sizeof(m_winnerData));
	FindWinner();

	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));
	info.filename = data_ab7900;
	info.fontw = 32;
	info.fonth = 32;
	info.numPages = 1;
	info.texw = 256;
	info.texh = 128;
	info.pCharSet = "1234567890ym%-./x ";

	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&info);

	if (IsLocalContent((localContentType_t)0x17)) {
		m_pHalloweenOverlay = g_resrcmng->GetOverlay(MakeStr("%s.tga", "z_Halloween_Male"), 0);
	}
}

// resultdlg.cpp:88
FrResultDlg::~FrResultDlg(void) {
	if (IsLocalContent((localContentType_t)0x17)) {
		if (g_resrcmng && m_pHalloweenOverlay) { g_resrcmng->Release(m_pHalloweenOverlay); m_pHalloweenOverlay = 0; }
	}

	if (m_pPartTidList) { delete m_pPartTidList; m_pPartTidList = 0; }
	if (m_pExhibition) { delete m_pExhibition; m_pExhibition = 0; }
	if (g_resrcmng && m_pTitleFont) { g_resrcmng->Release(m_pTitleFont); m_pTitleFont = 0; }
}

// resultdlg.cpp:99
void FrResultDlg::ResetVars(void) {
	m_pStageArea = 0;
	m_pLevelArea = 0;
	m_pExpStatic = 0;
	m_pExpGaugeBar = 0;
	m_pExhibition = 0;
	m_pLevelUpArea = 0;
	m_pPartTidList = 0;
	memset(&m_winnerData, 0, sizeof(m_winnerData));
}

// resultdlg.cpp:112
void FrResultDlg::FindWinner(void) {
	memset(WSingleton<CSharedDoc>::Instance()->m_teamResult, 0, sizeof(WSingleton<CSharedDoc>::Instance()->m_teamResult));

	for (std::vector<sRivalData>::iterator it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
		if (it->m_rank == 1) {
			m_winnerData = *it;

			if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 4)
				return;

			if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 10)
				return;
		}

		WSingleton<CSharedDoc>::Instance()->m_teamResult[it->m_team].m_score += it->m_rank;
		WSingleton<CSharedDoc>::Instance()->m_teamResult[it->m_team].m_pang += it->m_pang;
	}
}

// resultdlg.cpp:137
bool FrResultDlg::OnInit(void) {







	if (GalleryMode()) {

		m_startLevel = m_level = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_level;
		m_exp = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_value155;

	} else {

		m_startLevel = m_level = WSingleton<CSharedDoc>::Instance()->m_myLevel;
		m_exp = WSingleton<CSharedDoc>::Instance()->m_myExp;
	}


	m_expGain = WSingleton<CSharedDoc>::Instance()->m_gameStatistics[0].exp;
	int nextLevelExp = WSingleton<CSharedDoc>::Instance()->m_levelExp[m_level].m_exp;
	m_bExpDone = m_expGain == 0;


	m_exp *= 10;
	m_expGain *= 10;
	nextLevelExp *= 10;

	if (m_pExpStatic) {
		char text[32];
		sprintf(text, "%d / %d", m_exp / 10, nextLevelExp / 10);
		m_pExpStatic->SetCaption(text);
	}

	if (m_pLevelArea)
		m_pLevelArea->SetBgImg(MakeStr("level_%03d", m_level + 1));

	if (m_pExpGaugeBar) {


		m_pExpGaugeBar->SetBorder(1); m_pExpGaugeBar->SetRange(0, nextLevelExp, 0);
		m_pExpGaugeBar->SetPos(m_exp);
	}


	SetDesc(MakeStr("%49s1\xB5\xEE\xC0\xBA %s\xB4\xD4\xC0\xCC \xC2\xF7\xC1\xF6\xC7\xCF\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.", "", m_winnerData.m_nickname));

	if (WSingleton<CSharedDoc>::Instance()->m_value90 == 0) {

		WSingleton<CSharedDoc>::Instance()->m_value90 = 1;

		if (!GalleryMode()) {


			WSendPacket packet((enumClientPacket)6);
			packet.EncodeBuffer(&WSingleton<CSharedDoc>::Instance()->m_gameStatistics[0], sizeof(sPangYaUserStatistics));
			packet.Send(SEND_TO_DEFAULT);

			WSingleton<CSharedDoc>::Instance()->Log30sScore();
		}
	}



	return true;
}

// resultdlg.cpp:202
void FrResultDlg::OnProc(const float dt) {
	m_elapsed += dt;

	if (m_pExhibition) {
		const WRect& rect = m_pStageArea->GetRect();

		if (rect.x != m_stageRect.x || rect.y != m_stageRect.y) {
			m_pExhibition->SetCenter(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f);
			m_stageRect = rect;
		}

		m_pExhibition->Process(dt, true);
	}

	if (!m_pExpGaugeBar || m_bExpDone) {
		if (m_bExpDone && m_pLevelUpArea && m_startLevel < m_level)
			m_pLevelUpArea->SetVisible(true);
		return;
	}

	if (m_elapsed < 1.0f)
		return;

	float over = m_elapsed - 1.0f;
	if (over < 0.01f)
		return;

	int step = float2int(over * 100.0f);
	int level = m_level;
	m_elapsed -= step * 0.01f;
	int nextExp = WSingleton<CSharedDoc>::Instance()->m_levelExp[level].m_exp * 10;

	if (step > Abs(m_expGain)) {
		m_bExpDone = true;
		m_exp += ((m_expGain > 0) * 2 - 1) * Abs(m_expGain);
		m_expGain = 0;
	} else {
		int delta = ((m_expGain > 0) * 2 - 1) * step;
		m_exp += delta;
		m_expGain -= delta;
	}

	if (m_exp < 0 && !m_bExpDone) {
		m_bExpDone = true;
		m_exp = 0;
	}

	if (m_exp >= nextExp) {
		m_exp -= nextExp;
		m_level = level + 1;

		nextExp = WSingleton<CSharedDoc>::Instance()->m_levelExp[m_level].m_exp * 10;

		m_pExpGaugeBar->SetRange(0, nextExp, 0);
		m_pExpGaugeBar->SetPos(m_exp);

		if (m_pLevelArea) {
			char buf[32];
			sprintf(buf, "level_%03d", m_level + 1);
			m_pLevelArea->SetBgImg(buf);
		}
	} else {
		m_pExpGaugeBar->SetPos(m_exp);
	}

	if (m_pExpStatic) {
		char buf[32];
		sprintf(buf, "%d / %d", m_exp / 10, nextExp / 10);
		m_pExpStatic->SetCaption(buf);
	}
}

// resultdlg.cpp:300
void FrResultDlg::OnStageInit(PG_MSGARG param) {
	m_pStageArea = DYNAMIC_CAST(FrArea, param);

	if (m_pStageArea) {
		char bgName[64] = { 0 };
		sprintf(bgName, "30in_back_%s", WSingleton<CSharedDoc>::Instance()->m_pCurCourse ? WSingleton<CSharedDoc>::Instance()->m_pCurCourse->m_mapName : 0);
		m_pStageArea->SetBgImg(bgName);
	}

	ShowPet();
}

// resultdlg.cpp:314
void FrResultDlg::OnStageOwnerDraw(void) {
	char bgName[64] = { 0 };
	sprintf(bgName, "30in_back_%s", WSingleton<CSharedDoc>::Instance()->m_pCurCourse ? WSingleton<CSharedDoc>::Instance()->m_pCurCourse->m_mapName : 0);
	m_pStageArea->SetBgImg(bgName);

	if (m_pExhibition) {
		m_pExhibition->Display(0, 0);
	}

	const WRect& rect = m_pStageArea->GetRect();


	WPoint pos;
	pos.x = rect.x + 333.0f;
	pos.y = rect.y + 34.0f;

	char* text;





	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 10 && (WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_approachPoint == -1 || WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_approachDistance == -1)) {

		text = MakeStr("-");

	} else {

		text = MakeStr("%d", WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_rank);
	}


	if (m_pTitleFont) {
		m_pTitleFont->Print(g_view, pos.x, pos.y, text, 0, 0xFFFFFFFF, 0);
	}
}

// resultdlg.cpp:346
void FrResultDlg::OnTrophyInit(PG_MSGARG param) {
	FrArea* pArea = DYNAMIC_CAST(FrArea, param);
	if (pArea == 0)
		return;


	if (WSingleton<CSharedDoc>::Instance()->m_value456C) {

		IFF_STRUCT::sMatch* pMatch = ItemManager()->FindMatch(WSingleton<CSharedDoc>::Instance()->m_matchTypeId);

		if (pMatch && WSingleton<CSharedDoc>::Instance()->m_value456C >= 1 && WSingleton<CSharedDoc>::Instance()->m_value456C <= 3) {
			pArea->SetBgImg(pMatch->m_trophyIcon[WSingleton<CSharedDoc>::Instance()->m_value456C - 1]);
		} else {
			pArea->SetVisible(false);
		}

		char* text = MakeStr("%d", WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_rank);




		float width = m_pTitleFont->GetTextWidth(g_view, text);
		WPoint pt;
		pt.x = width + m_rect.x + 340.0f;
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 5) {
			pt.y = m_rect.y + 100.0f;
		} else {
			pt.y = m_rect.y + 40.0f;
		}
		pArea->MoveWindow(pt);
	} else {

		pArea->SetVisible(false);
	}
}

// resultdlg.cpp:379
void FrResultDlg::OnMatchStaticInit(PG_MSGARG param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);
	if (pStatic == 0)
		return;

	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 10) {
		pStatic->SetCaption("\xB3\xB2\xC0\xBA \xB0\xC5\xB8\xAE :");
	}
}

// resultdlg.cpp:391
void FrResultDlg::OnMatchInit(PG_MSGARG param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);

	if (pStatic == 0)
		return;


	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 10) {

		if (WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_approachPoint == -1) {



			pStatic->SetCaption("Out");
		} else {
			unsigned int distance = WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_approachDistance;

			if (distance == -1)
				pStatic->SetCaption("Out");
			else
				pStatic->SetCaption(MakeStr("%d.%d y", distance / 10, distance % 10));
		}
	} else {


		IFF_STRUCT::sMatch* pMatch = ItemManager()->FindMatch(WSingleton<CSharedDoc>::Instance()->m_matchTypeId);

		if (pMatch == 0)
			return;

		char name[80] = { 0 };

		if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomMode == 14) {
			const char* text = "\xBC\xC5\xC7\xC3\xC4\xDA\xBD\xBA";
			strcpy(name, text);
		} else {

			strcpy(name, pMatch->m_name);
		}


		pStatic->SetCaption(name);
	}
}

// resultdlg.cpp:436
void FrResultDlg::OnIdInit(intptr_t param) {
	m_pIdArea = DYNAMIC_CAST(FrArea, param);
}

// resultdlg.cpp:441
void FrResultDlg::OnIdOwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();

	if (gdi) {
		float offset = 0.0f;
		sGameResultInfo& info = WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()];

		if (info.m_hasEmblem) {
			const Bitmap* emblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(info.m_emblemName);

			if (emblem) {
				gdi->DrawTexture(emblem, WRect(m_pIdArea->GetRect().x, m_pIdArea->GetRect().y - 6.0f, emblem->Width(), emblem->Height()), 0xFFFFFFFF, 0);
				offset = 27.0f;
			}
		}

		if (!WSingleton<CProjectG>::Instance()->HidePrivacy()) {
			gdi->SetTextColor(0xFF000000, 0xFFFFFFFF);
			gdi->SetTextStyle(0);
			g_pFresh->GetManager()->PrintText(WPoint(m_pIdArea->GetRect().x + offset, m_pIdArea->GetRect().y), 0, info.m_nickname, -1.0f, 0xFFFFFFFF);
		}
	}
}

// resultdlg.cpp:477
void FrResultDlg::OnScoreStaticInit(PG_MSGARG param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);
	if (pStatic == 0)
		return;

	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 10) {
		pStatic->SetCaption("\xB3\xB2\xC0\xBA \xBD\xC3\xB0\xA3 :");
	}
}

// resultdlg.cpp:489
void FrResultDlg::OnScoreInit(PG_MSGARG param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);
	if (pStatic == 0)
		return;


	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 10) {

		int point = WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_approachPoint;

		if (point == -1) {
			pStatic->SetCaption("-");
		} else {
			pStatic->SetCaption(MakeStr("%d.%03d \xC3\xCA", point / 1000, point % 1000));
		}
	} else {
		pStatic->SetCaption(MakeStr(WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_score > 0 ? "+%d" : "%d", WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_score));
	}
}

// resultdlg.cpp:508
void FrResultDlg::OnPangInit(PG_MSGARG param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);
	if (pStatic == 0)
		return;


	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 10) {

		pStatic->SetCaption("0");


		for (int i = 6; i < 12; i++) {

			if (WSingleton<CSharedDoc>::Instance()->m_awardPlayers[i].m_uid == MyGuid(true)) {

				pStatic->SetCaption(MakeStr("%d", WSingleton<CSharedDoc>::Instance()->m_awardPlayers[i].m_pang));
				return;
			}
		}
	} else {
		pStatic->SetCaption(MakeStr("%I64d", WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_pang));
	}
}

// resultdlg.cpp:532
void FrResultDlg::OnBonusStaticInit(PG_MSGARG param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);
	if (pStatic == 0)
		return;

	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 10) {
		pStatic->SetVisible(false);
	}
}

// resultdlg.cpp:545
void FrResultDlg::OnBonusInit(PG_MSGARG param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);

	if (pStatic == 0)
		return;


	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 10) {

		pStatic->SetVisible(false);
	}











	__int64 bonus = (int)((float)(WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_pang + WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_extraPang) * WSingleton<CSharedDoc>::Instance()->m_pangPercent * WSingleton<CSharedDoc>::Instance()->m_pangRate * 0.01f - WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_pang);


	pStatic->SetCaption(MakeStr("%I64d", bonus));
}

// resultdlg.cpp:572
void FrResultDlg::OnGetItemInit(PG_MSGARG param) {
	m_pGetItemArea = DYNAMIC_CAST(FrArea, param);
}

// resultdlg.cpp:577
void FrResultDlg::OnGetItemOwnerDraw(void) {
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi == 0)
		return;

	float offset = 0.0f;

	// The award players' prize items (sAwardPlayer::m_pang holds a type id here).
	for (int i = 0; i < 12; i++) {
		if (WSingleton<CSharedDoc>::Instance()->m_awardPlayers[i].m_uid == MyGuid(true) && WSingleton<CSharedDoc>::Instance()->m_awardPlayers[i].m_pang) {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(WSingleton<CSharedDoc>::Instance()->m_awardPlayers[i].m_pang);
			const Bitmap* pBitmap;
			switch ((unsigned long)WSingleton<CSharedDoc>::Instance()->m_awardPlayers[i].m_pang >> 26) {
			case 5:
				pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", pItem->m_icon);
				break;
			case 6:
				pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", pItem->m_icon);
				break;
			case 2:
				pBitmap = g_pFresh->GetManager()->GetBitmap("FASHION", pItem->m_icon);
				break;
			default:
				continue;
			}

			if (pBitmap) {
				gdi->DrawTexture(pBitmap, WRect(offset + m_pGetItemArea->GetRect().x, m_pGetItemArea->GetRect().y, (float)pBitmap->Width(), (float)pBitmap->Height()), 0xFFFFFFFF, 0);
				offset += 50.0f;
			}
		}
	}

	if (IsLocalContent((localContentType_t)0x17) && WSingleton<CSharedDoc>::Instance()->m_wearHalloweenItem) {
		if (m_pHalloweenOverlay) {
			m_pHalloweenOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(offset + m_pIdArea->GetRect().x + 150.0f, m_pIdArea->GetRect().y + 5.0f, m_pHalloweenOverlay->GetWidth(), m_pHalloweenOverlay->GetHeight()), 0, 0xFFFFFFFF, 0, 0);
		}
		gdi->SetTextStyle(1);
		gdi->SetTextColor(0xFF000000, 0xFFFFFFFF);
		gdi->SetScale(2.0f);
		g_pFresh->GetManager()->PrintText(WPoint(m_pIdArea->GetRect().x + 141.0f, m_pIdArea->GetRect().y + 41.0f), 0, "+100", -1.0f, 0xFFFFFFFF);
		gdi->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
		g_pFresh->GetManager()->PrintText(WPoint(m_pIdArea->GetRect().x + 140.0f, m_pIdArea->GetRect().y + 40.0f), 0, "+100", -1.0f, 0xFFFFFFFF);
		gdi->SetScale(1.0f);
	}

	// The items won in the game.
	for (std::vector<unsigned long>::iterator it = WSingleton<CSharedDoc>::Instance()->m_gainedItemList[0].begin(); it != WSingleton<CSharedDoc>::Instance()->m_gainedItemList[0].end(); ++it) {
		const Bitmap* pBitmap;
		if ((*it >> 26) == 6) {
			IFF_STRUCT::sItem* pItem = ItemManager()->FindItem(*it);
			if (pItem == 0)
				continue;

			pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", pItem->m_icon);
		} else if ((*it >> 26) == 0x1D) {
			IFF_STRUCT::sQuestDrop* pDrop = ItemManager()->FindQuestDrop(*it);
			if (pDrop == 0)
				continue;

			pBitmap = g_pFresh->GetManager()->GetBitmap("QUESTDROP", pDrop->m_icon);
		} else {
			continue;
		}

		if (pBitmap) {
			gdi->DrawTexture(pBitmap, WRect(offset + m_pGetItemArea->GetRect().x - 11.0f, m_pGetItemArea->GetRect().y - 5.0f, (float)pBitmap->Width(), (float)pBitmap->Height()), 0xFFFFFFFF, 0);
			offset += 50.0f;
		}
	}
}

// resultdlg.cpp:674
void FrResultDlg::OnLevelInit(intptr_t param) {
	m_pLevelArea = DYNAMIC_CAST(FrArea, param);
}

// resultdlg.cpp:679
void FrResultDlg::OnExpInit(PG_MSGARG param) {
	m_pExpStatic = DYNAMIC_CAST(FrStatic, param);
}

// resultdlg.cpp:684
void FrResultDlg::OnExpGaugeBarInit(PG_MSGARG param) {
	m_pExpGaugeBar = DYNAMIC_CAST(FrGaugeBar, param);
}

// resultdlg.cpp:689
void FrResultDlg::OnLevelUpInit(PG_MSGARG param) {
	m_pLevelUpArea = DYNAMIC_CAST(FrArea, param);

	if (m_pLevelUpArea) {
		m_pLevelUpArea->SetVisible(false);
	}
}

// resultdlg.cpp:697
void FrResultDlg::ShowPet(void) {
	if (m_pExhibition) { delete m_pExhibition; m_pExhibition = 0; }
	if (m_pPartTidList) { delete m_pPartTidList; m_pPartTidList = 0; }

	if (m_pStageArea == 0)
		return;

	WRect rect = m_pStageArea->GetRect();

	m_pExhibition = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.33f, false);
	m_pExhibition->SetDistance(20.0f, true, false);
	m_pExhibition->SetArea(rect.w, rect.h);

	// The motion follows the player's rank: 1 first, 2 middle, 3 last.
	unsigned char rank;
	unsigned char index = MyIndex();
	if (index < WSingleton<CSharedDoc>::Instance()->m_gameResultList.size()) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 5) {
			if (WSingleton<CSharedDoc>::Instance()->m_teamResult[0].m_score < WSingleton<CSharedDoc>::Instance()->m_teamResult[1].m_score ||
				WSingleton<CSharedDoc>::Instance()->m_teamResult[0].m_score == WSingleton<CSharedDoc>::Instance()->m_teamResult[1].m_score && WSingleton<CSharedDoc>::Instance()->m_teamResult[0].m_pang > WSingleton<CSharedDoc>::Instance()->m_teamResult[1].m_pang) {
				rank = WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_team != 0 ? 3 : 1;
			} else if (WSingleton<CSharedDoc>::Instance()->m_teamResult[0].m_score > WSingleton<CSharedDoc>::Instance()->m_teamResult[1].m_score ||
				WSingleton<CSharedDoc>::Instance()->m_teamResult[0].m_score == WSingleton<CSharedDoc>::Instance()->m_teamResult[1].m_score && WSingleton<CSharedDoc>::Instance()->m_teamResult[0].m_pang < WSingleton<CSharedDoc>::Instance()->m_teamResult[1].m_pang) {
				rank = WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_team != 1 ? 3 : 1;
			} else {
				rank = 2;
			}
		} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
			if (WSingleton<CSharedDoc>::Instance()->m_winningTeam == 0) {
				rank = WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_team != 0 ? 3 : 1;
			} else if (WSingleton<CSharedDoc>::Instance()->m_winningTeam == 1) {
				rank = WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_team != 1 ? 3 : 1;
			} else {
				rank = 3;
			}
		} else {
			rank = WSingleton<CSharedDoc>::Instance()->m_gameResultList[index].m_rank;
			if (rank == 1)
				rank = 1;
			else
				rank = rank > WSingleton<CSharedDoc>::Instance()->m_gameResultList.size() * 6 / 10 ? 3 : 2;
		}
	} else {
		rank = 3;
	}

	if (GalleryMode()) {
		m_pPartTidList = new CPartTidList;
		if (!m_pPartTidList->SetTids(&WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_characterInfo, 0xFF) || !m_pPartTidList->IsComboValid())
			m_pPartTidList->SetDefaultTids();
		m_pExhibition->SetModel(*m_pPartTidList, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_characterInfo.auxParts.tids, MakeStr("%d\xB5\xEE\xB8\xF0\xBC\xC7", rank), 3.0f, 0);
	} else {
		m_pExhibition->SetModel(WSingleton<CSharedDoc>::Instance()->GetMyPartTidList(), WSingleton<CSharedDoc>::Instance()->GetMyAuxPartTidList(), MakeStr("%d\xB5\xEE\xB8\xF0\xBC\xC7", rank), 3.0f, 0);
	}
}

// resultdlg.cpp:803 (run-time type and factory registration of FrResultTeamDlg)
const WRTTI FrResultTeamDlg::m_RTTI("FrResultTeamDlg", &FrForm::m_RTTI);

IObject* FrResultTeamDlgMakeInstance(void) {
	return new FrResultTeamDlg;
}

struct __sFrResultTeamDlg {
	__sFrResultTeamDlg(void) {
		ObjectFactory().AddObjectFunctor(FrResultTeamDlgMakeInstance, "FrResultTeamDlg");
	}
};
static __sFrResultTeamDlg s_registerFrResultTeamDlg;

// resultdlg.cpp:805
const sFRESH_MSGMAP* FrResultTeamDlg::GetMessageMap(void) const {
	return &FrResultTeamDlg::_MsgMap;
}

sFRESH_MSGMAP FrResultTeamDlg::_MsgMap = { &FrForm::_MsgMap, &FrResultTeamDlg::_MsgEntries[0] };

sFRESH_ENTRY FrResultTeamDlg::_MsgEntries[] = {
	// resultdlg.cpp:807
	{ "stage", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnStageInit },
	// resultdlg.cpp:808
	{ "stage", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrResultTeamDlg::OnStageOwnerDraw },
	// resultdlg.cpp:809
	{ "trophy", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnTrophyInit },
	// resultdlg.cpp:810
	{ "match", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnMatchInit },
	// resultdlg.cpp:811
	{ "id", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnIdInit },
	// resultdlg.cpp:812
	{ "id", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnIdOwnerDraw },
	// resultdlg.cpp:813
	{ "score", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnScoreInit },
	// resultdlg.cpp:814
	{ "pang", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnPangInit },
	// resultdlg.cpp:815
	{ "bonus", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnBonusInit },
	// resultdlg.cpp:816
	{ "getitem", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnGetItemInit },
	// resultdlg.cpp:817
	{ "getitem", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrResultTeamDlg::OnGetItemOwnerDraw },
	// resultdlg.cpp:818
	{ "level", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnLevelInit },
	// resultdlg.cpp:819
	{ "exp", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnExpInit },
	// resultdlg.cpp:820
	{ "exp_gb", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnExpGaugeBarInit },
	// resultdlg.cpp:821
	{ "levelup", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnLevelUpInit },
	// resultdlg.cpp:822
	{ "red", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnRedInit },
	// resultdlg.cpp:823
	{ "red", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrResultTeamDlg::OnRedOwnerDraw },
	// resultdlg.cpp:824
	{ "blue", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnBlueInit },
	// resultdlg.cpp:825
	{ "blue", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrResultTeamDlg::OnBlueOwnerDraw },
	// resultdlg.cpp:826
	{ "red_score", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnRedScoreInit },
	// resultdlg.cpp:827
	{ "blue_score", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnBlueScoreInit },
	// resultdlg.cpp:828
	{ "red_pang", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnRedPangInit },
	// resultdlg.cpp:829
	{ "blue_pang", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnBluePangInit },
	// resultdlg.cpp:830
	{ "result", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnResultInit },
	// resultdlg.cpp:831
	{ "rank_static", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnRankStaticInit },
	// resultdlg.cpp:832
	{ "result_static", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnResultStaticInit },
	// resultdlg.cpp:833
	{ "win_lose", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnWinLoseInit },
	// resultdlg.cpp:834
	{ "matchtype", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnMatchTypeInit },
	// resultdlg.cpp:835
	{ "bonus_static", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrResultTeamDlg::OnBonusStaticInit },
	// resultdlg.cpp:837
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// resultdlg.cpp:840
void FrResultTeamDlg::OnStageInit(PG_MSGARG param) {
	FrResultDlg::OnStageInit(param);

	m_pTrophyBitmap = 0;

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6 && WSingleton<CSharedDoc>::Instance()->m_winningTeam == GUILDMATCHUP(0)->m_team && WSingleton<CSharedDoc>::Instance()->m_winningTeam != 2) {
		m_pTrophyBitmap = g_pFresh->GetManager()->GetBitmap("TROPHY", "guild");
	}
}

// resultdlg.cpp:862
bool FrResultTeamDlg::OnInit(void) {
	FrResultDlg::OnInit();

	// a guild match: name the winning guild
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
		memset(&m_guildMatch, 0, sizeof(m_guildMatch));

		if (WSingleton<CSharedDoc>::Instance()->m_winningTeam == 2) {
			SetDesc(MakeStr("%49s \xB4\xD9\xBD\xC3 \xC7\xD1\xB9\xF8 \xC0\xDA\xBF\xF5\xC0\xBB \xB0\xA1\xB8\xAE\xBC\xBC\xBF\xE4^^", ""));
		} else if (WSingleton<CSharedDoc>::Instance()->m_winningTeam == GUILDMATCHUP(0)->m_team) {
			SetDesc(MakeStr("%49s %s\xB1\xE6\xB5\xE5\xB0\xA1 \xBD\xC2\xB8\xAE\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", "", GUILDMATCHUP(0)->m_guildName));
		} else if (WSingleton<CSharedDoc>::Instance()->m_winningTeam == GUILDMATCHUP(1)->m_team) {
			SetDesc(MakeStr("%49s %s\xB1\xE6\xB5\xE5\xB0\xA1 \xBD\xC2\xB8\xAE\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", "", GUILDMATCHUP(1)->m_guildName));
		}

		// The room's guild block (sRoomInfo::m_guild) has sGuildMatchInfo's layout.
		memcpy(&m_guildMatch, &WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_guild, sizeof(m_guildMatch));
	}

	return true;
}

// resultdlg.cpp:891
void FrResultTeamDlg::OnStageOwnerDraw(void) {
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi == 0)
		return;


	float x = m_rect.x, y = m_rect.y;

	g_view->DrawLine2D(WPoint(x, y + 91.0f), WPoint(x + m_rect.w - 10.0f, y + 91.0f), 0xFF000000, 0xFF000000, 0);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {

		if (m_pExhibition)
			m_pExhibition->Display(0, 0);

		const WRect& rect = m_pStageArea->GetRect();

		if (m_pTrophyBitmap) {

			gdi->DrawTexture(m_pTrophyBitmap, WRect(rect.x + 343.0f, rect.y + 10.0f, (float)m_pTrophyBitmap->Width(), (float)m_pTrophyBitmap->Height()), 0xFFFFFFFF, 0);
		}




		const Bitmap* pEmblem = 0;
		switch (WSingleton<CSharedDoc>::Instance()->m_winningTeam) {
		case 0:


			if (GUILDMATCHUP(0)->m_team == 0) {

				pEmblem = m_pRedEmblem;
			}
			break;










		case 1:

			if (GUILDMATCHUP(0)->m_team == 1) {

				pEmblem = m_pBlueEmblem;
			}
			break;
		}











		if (pEmblem) {

			gdi->DrawTexture(pEmblem, WRect(rect.x + 364.0f, rect.y + 22.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
		}
	} else {

		FrResultDlg::OnStageOwnerDraw();
	}
}

// resultdlg.cpp:959
void FrResultTeamDlg::OnTrophyInit(PG_MSGARG param) {
	FrResultDlg::OnTrophyInit(param);
}

// resultdlg.cpp:974
void FrResultTeamDlg::OnMatchTypeInit(PG_MSGARG param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
		pStatic->SetCaption("\xB1\xE6\xB5\xE5 \xB1\xE2\xBF\xA9\xB5\xB5 :");
	} else {
		pStatic->SetCaption("\xB4\xEB   \xC8\xB8 :");
	}
}

// resultdlg.cpp:998
void FrResultTeamDlg::OnMatchInit(PG_MSGARG param) {
	FrStatic* pStatic = (FrStatic*)((__rtti_obj = (void*)param) != 0 ? (((IObject*)__rtti_obj)->IsKindOf(&FrStatic::m_RTTI) ? (void*)param : 0) : 0);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
		if (pStatic == 0)
			return;

		pStatic->SetCaption(MakeStr("%d \xC1\xA1", WSingleton<CSharedDoc>::Instance()->m_matchGameCount));
	} else {
		IFF_STRUCT::sMatch* pMatch = ItemManager()->FindMatch(WSingleton<CSharedDoc>::Instance()->m_matchTypeId);

		if (pStatic == 0 || pMatch == 0)
			return;

		pStatic->SetCaption(MakeStr("%s %s", pMatch->m_name, "\xC6\xC0\xC0\xFC"));
	}
}

// resultdlg.cpp:1020
void FrResultTeamDlg::OnIdInit(intptr_t param) {
	m_pTeamIdArea = DYNAMIC_CAST(FrArea, param);
}

// resultdlg.cpp:1025
void FrResultTeamDlg::OnIdOwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();

	if (gdi) {
		float offset = 0.0f;
		sGameResultInfo& info = WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()];

		if (info.m_hasEmblem) {
			const Bitmap* emblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(info.m_emblemName);

			if (emblem) {
				gdi->DrawTexture(emblem, WRect(m_pTeamIdArea->GetRect().x, m_pTeamIdArea->GetRect().y - 6.0f, emblem->Width(), emblem->Height()), 0xFFFFFFFF, 0);
				offset = 27.0f;
			}
		}

		if (!WSingleton<CProjectG>::Instance()->HidePrivacy()) {
			gdi->SetTextColor(0xFF000000, 0xFFFFFFFF);
			gdi->SetTextStyle(0);
			g_pFresh->GetManager()->PrintText(WPoint(m_pTeamIdArea->GetRect().x + offset, m_pTeamIdArea->GetRect().y), 0, info.m_nickname, -1.0f, 0xFFFFFFFF);
		}
	}
}

// resultdlg.cpp:1061
void FrResultTeamDlg::OnScoreInit(PG_MSGARG param) {
	FrResultDlg::OnScoreInit(param);
}

// resultdlg.cpp:1066
void FrResultTeamDlg::OnPangInit(PG_MSGARG param) {
	FrResultDlg::OnPangInit(param);
}

// resultdlg.cpp:1081
void FrResultTeamDlg::OnBonusStaticInit(PG_MSGARG param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
		pStatic->SetCaption("\xB1\xE6\xB5\xE5 \xC6\xCE :");
	} else {
		pStatic->SetCaption("\xBA\xB8\xB3\xCA\xBD\xBA :");
	}
}

// resultdlg.cpp:1105
void FrResultTeamDlg::OnBonusInit(PG_MSGARG param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);

	if (pStatic == 0)
		return;


	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {

		pStatic->SetCaption(MakeStr("%d \xC6\xCE", WSingleton<CSharedDoc>::Instance()->m_value5018));
		return;
	}












	__int64 bonus = (int)((float)(WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_pang + WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_extraPang) * WSingleton<CSharedDoc>::Instance()->m_pangPercent * WSingleton<CSharedDoc>::Instance()->m_pangRate * 0.01f - WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_pang);


	if (WSingleton<CSharedDoc>::Instance()->m_winningTeam == 2)
		pStatic->SetCaption(MakeStr("%I64d", bonus));
	else if (WSingleton<CSharedDoc>::Instance()->m_winningTeam == WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_team)
		pStatic->SetCaption(MakeStr("%I64d+100", bonus));
	else
		pStatic->SetCaption(MakeStr("%I64d-50", bonus));
}

// resultdlg.cpp:1139
void FrResultTeamDlg::OnGetItemInit(PG_MSGARG param) {
	FrResultDlg::OnGetItemInit(param);
}

// resultdlg.cpp:1144
void FrResultTeamDlg::OnGetItemOwnerDraw(void) {
	FrResultDlg::OnGetItemOwnerDraw();
}

// resultdlg.cpp:1149
void FrResultTeamDlg::OnLevelInit(intptr_t param) {
	FrResultDlg::OnLevelInit(param);
}

// resultdlg.cpp:1154
void FrResultTeamDlg::OnExpInit(PG_MSGARG param) {
	FrResultDlg::OnExpInit(param);
}

// resultdlg.cpp:1159
void FrResultTeamDlg::OnExpGaugeBarInit(PG_MSGARG param) {
	FrResultDlg::OnExpGaugeBarInit(param);
}

// resultdlg.cpp:1164
void FrResultTeamDlg::OnLevelUpInit(PG_MSGARG param) {
	FrResultDlg::OnLevelUpInit(param);
}

// resultdlg.cpp:1170
void FrResultTeamDlg::OnRankStaticInit(PG_MSGARG param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);
	if (pStatic == 0)
		return;

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6)
		pStatic->SetVisible(false);
}

// resultdlg.cpp:1181
void FrResultTeamDlg::OnResultStaticInit(PG_MSGARG param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);
	if (pStatic == 0)
		return;

	if (WSingleton<CSharedDoc>::Instance()->m_roomType != 6)
		pStatic->SetVisible(false);
}

// resultdlg.cpp:1191
void FrResultTeamDlg::OnRedInit(PG_MSGARG param) {
	m_pRedArea = DYNAMIC_CAST(FrArea, param);

	if (m_pRedArea) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
			m_pRedEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_guild.m_guildMark[0]);
		} else {
			m_pRedArea->SetBgImg("30in_team_redteam_dark");
		}
	}
}

// resultdlg.cpp:1218
void FrResultTeamDlg::OnRedOwnerDraw(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
		WRect rect = m_pRedArea->GetRect();

		if (m_pRedEmblem) {
			g_pFresh->GetManager()->GetGDI()->DrawTexture(m_pRedEmblem, WRect(rect.x + 10.0f, rect.y + 1.0f, m_pRedEmblem->Width(), m_pRedEmblem->Height()), 0xFFFFFFFF, 0);
		}

		g_pFresh->GetManager()->PrintText(WPoint(rect.x, rect.y + 30.0f), 0, m_guildMatch.m_guildName[0], -1.0f, 0xFFFFFFFF);
	}
}

// resultdlg.cpp:1234
void FrResultTeamDlg::OnBlueInit(PG_MSGARG param) {
	m_pBlueArea = DYNAMIC_CAST(FrArea, param);

	if (m_pBlueArea) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
			m_pBlueEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_guild.m_guildMark[1]);
		} else {
			m_pBlueArea->SetBgImg("30in_team_blueteam_dark");
		}
	}
}

// resultdlg.cpp:1261
void FrResultTeamDlg::OnBlueOwnerDraw(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
		WRect rect = m_pBlueArea->GetRect();

		if (m_pBlueEmblem) {
			g_pFresh->GetManager()->GetGDI()->DrawTexture(m_pBlueEmblem, WRect(rect.x + 10.0f, rect.y + 1.0f, m_pBlueEmblem->Width(), m_pBlueEmblem->Height()), 0xFFFFFFFF, 0);
		}

		g_pFresh->GetManager()->PrintText(WPoint(rect.x, rect.y + 30.0f), 0, m_guildMatch.m_guildName[1], -1.0f, 0xFFFFFFFF);
	}
}

// resultdlg.cpp:1277
void FrResultTeamDlg::OnRedScoreInit(PG_MSGARG param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);
	if (pStatic == 0)
		return;

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 5)
		pStatic->SetCaption(MakeStr("%d", WSingleton<CSharedDoc>::Instance()->m_teamResult[0].m_score));
	else
		pStatic->SetCaption(MakeStr("%d", WSingleton<CSharedDoc>::Instance()->m_teamPoint[0]));
}

// resultdlg.cpp:1289
void FrResultTeamDlg::OnBlueScoreInit(PG_MSGARG param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);
	if (pStatic == 0)
		return;

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 5)
		pStatic->SetCaption(MakeStr("%d", WSingleton<CSharedDoc>::Instance()->m_teamResult[1].m_score));
	else
		pStatic->SetCaption(MakeStr("%d", WSingleton<CSharedDoc>::Instance()->m_teamPoint[1]));
}

// resultdlg.cpp:1301
void FrResultTeamDlg::OnRedPangInit(PG_MSGARG param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);
	if (pStatic == 0)
		return;
	pStatic->SetCaption(MakeStr("%I64dp", WSingleton<CSharedDoc>::Instance()->m_teamResult[0].m_pang));
}

// resultdlg.cpp:1308
void FrResultTeamDlg::OnBluePangInit(PG_MSGARG param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);
	if (pStatic == 0)
		return;
	pStatic->SetCaption(MakeStr("%I64dp", WSingleton<CSharedDoc>::Instance()->m_teamResult[1].m_pang));
}

// resultdlg.cpp:1315
void FrResultTeamDlg::OnResultInit(PG_MSGARG param) {
	FrArea* pArea = (FrArea*)((__rtti_obj = (void*)param) != 0 ? (((IObject*)__rtti_obj)->IsKindOf(&FrArea::m_RTTI) ? (void*)param : 0) : 0);
	if (pArea == 0)
		return;

	// a guild match shows no team result
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6)
		return;

	switch (WSingleton<CSharedDoc>::Instance()->m_winningTeam) {
	case 0: {
		// the red team won: the local player's team (0 red) picks the image
		unsigned char team = WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_team;
		if (team == 0)
			pArea->SetBgImg("30in_team_redteam_win");
		else
			pArea->SetBgImg("30in_team_blueteam_lose");
		break;
	}
	case 1: {
		// the blue team won
		unsigned char team = WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_team;
		if (team == 0)
			pArea->SetBgImg("30in_team_redteam_lose");
		else
			pArea->SetBgImg("30in_team_blueteam_win");
		break;
	}
	case 2: {
		// a draw
		unsigned char team = WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_team;
		if (team == 0)
			pArea->SetBgImg("30in_team_redteam_draw");
		else
			pArea->SetBgImg("30in_team_blueteam_draw");
		break;
	}
	}
}

// resultdlg.cpp:1355
void FrResultTeamDlg::OnWinLoseInit(PG_MSGARG param) {
	FrArea* pArea = DYNAMIC_CAST(FrArea, param);
	if (pArea == 0)
		return;


	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
		char name[32];


		switch (WSingleton<CSharedDoc>::Instance()->m_winningTeam) {
		case 0:


			if (WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_team == 0)
				strcpy(name, "win2");
			else
				strcpy(name, "lose2");
			break;





		case 1:
			if (WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_team == 0)
				strcpy(name, "lose2");
			else
				strcpy(name, "win2");
			break;



		case 2:
			strcpy(name, "draw2");
			break;
		}


		pArea->SetBgImg(g_pFresh->GetManager()->GetBitmap("SCORECARD", name));
	}
}

// resultdlg.cpp:1402
bool UniteRankCompare(const void* a, const void* b) {
	const sGameResultInfo* pA = (const sGameResultInfo*)a;
	const sGameResultInfo* pB = (const sGameResultInfo*)b;

	if (pA->m_rank < pB->m_rank) {
		return true;
	} else if (pA->m_rank > pB->m_rank) {
		return false;
	}

	if (pA->m_sortGroup > pB->m_sortGroup) {
		return true;
	} else if (pA->m_sortGroup < pB->m_sortGroup) {
		return false;
	}

	if (pA->m_score < pB->m_score) {
		return true;
	} else if (pA->m_score > pB->m_score) {
		return false;
	}

	if (pA->m_pang > pB->m_pang) {
		return true;
	} else if (pA->m_pang < pB->m_pang) {
		return false;
	}

	return pA->m_value13F < pB->m_value13F;
}

// resultdlg.cpp:1435
bool ReportRankCompare(const void* a, const void* b) {
	const sGameResultInfo* pA = (const sGameResultInfo*)a;
	const sGameResultInfo* pB = (const sGameResultInfo*)b;

	if (pA->m_rank < pB->m_rank) {
		return true;
	} else if (pA->m_rank > pB->m_rank) {
		return false;
	}

	if (pA->m_sortGroup > pB->m_sortGroup) {
		return true;
	} else if (pA->m_sortGroup < pB->m_sortGroup) {
		return false;
	}

	if (pA->m_score < pB->m_score) {
		return true;
	} else if (pA->m_score > pB->m_score) {
		return false;
	}

	if (pA->m_pang > pB->m_pang) {
		return true;
	} else if (pA->m_pang < pB->m_pang) {
		return false;
	}

	return pA->m_value13F < pB->m_value13F;
}

// resultdlg.cpp:1470
bool UniteGuildIndexCompare(const void* a, const void* b) {
	const sGameResultInfo* pA = (const sGameResultInfo*)a;
	const sGameResultInfo* pB = (const sGameResultInfo*)b;

	if (pA->m_sortGroup > pB->m_sortGroup) {
		return true;
	} else if (pA->m_sortGroup < pB->m_sortGroup) {
		return false;
	}

	if (pA->m_uniteGuildIndex > pB->m_uniteGuildIndex) {
		return true;
	} else if (pA->m_uniteGuildIndex < pB->m_uniteGuildIndex) {
		return false;
	}

	if (pA->m_pang > pB->m_pang) {
		return true;
	} else if (pA->m_pang < pB->m_pang) {
		return false;
	}

	return pA->m_value13F < pB->m_value13F;
}

// resultdlg.cpp:1499 (run-time type and factory registration of FrUniteResultDlg)
const WRTTI FrUniteResultDlg::m_RTTI("FrUniteResultDlg", &FrForm::m_RTTI);

IObject* FrUniteResultDlgMakeInstance(void) {
	return new FrUniteResultDlg;
}

struct __sFrUniteResultDlg {
	__sFrUniteResultDlg(void) {
		ObjectFactory().AddObjectFunctor(FrUniteResultDlgMakeInstance, "FrUniteResultDlg");
	}
};
static __sFrUniteResultDlg s_registerFrUniteResultDlg;

// resultdlg.cpp:1501
const sFRESH_MSGMAP* FrUniteResultDlg::GetMessageMap(void) const {
	return &FrUniteResultDlg::_MsgMap;
}

sFRESH_MSGMAP FrUniteResultDlg::_MsgMap = { &FrForm::_MsgMap, &FrUniteResultDlg::_MsgEntries[0] };

sFRESH_ENTRY FrUniteResultDlg::_MsgEntries[] = {
	// resultdlg.cpp:1503
	{ "cancel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUniteResultDlg::OnCancelButtonInit },
	// resultdlg.cpp:1504
	{ "awardinfo", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUniteResultDlg::OnAwardInfoInit },
	// resultdlg.cpp:1505
	{ "awardinfo", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUniteResultDlg::OnAwardInfoClick },
	// resultdlg.cpp:1506
	{ "myinfoarea", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUniteResultDlg::OnMyInfoAreaInit },
	// resultdlg.cpp:1507
	{ "myinfoarea", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUniteResultDlg::OnMyInfoAreaDraw },
	// resultdlg.cpp:1508
	{ "exp_gb", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUniteResultDlg::OnExpGaugeBarInit },
	// resultdlg.cpp:1509
	{ "exp_area", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUniteResultDlg::OnExpAreaInit },
	// resultdlg.cpp:1510
	{ "exp_area", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUniteResultDlg::OnExpAreaOwnerDraw },
	// resultdlg.cpp:1511
	{ "resultlist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUniteResultDlg::OnRankListBoxInit },
	// resultdlg.cpp:1512
	{ "resultlist", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUniteResultDlg::OnRankListOwnerDraw },
	// resultdlg.cpp:1513
	{ "resultlist", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUniteResultDlg::OnRankListLBtnUp },
	// resultdlg.cpp:1514
	{ "resultlist", FRCMD_RBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUniteResultDlg::OnRankListRBtnUp },
	// resultdlg.cpp:1515
	{ "resultwinner", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUniteResultDlg::OnDescAreaInit },
	// resultdlg.cpp:1516
	{ "resultwinner", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUniteResultDlg::OnDescAreaOwnerDraw },
	// resultdlg.cpp:1517
	{ "awardarea", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUniteResultDlg::OnAwardAreaInit },
	// resultdlg.cpp:1518
	{ "awardarea", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUniteResultDlg::OnAwardAreaOwnerDraw },
	// resultdlg.cpp:1519
	{ "itemarea", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUniteResultDlg::OnItemAreaInit },
	// resultdlg.cpp:1520
	{ "itemarea", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUniteResultDlg::OnItemAreaOwnerDraw },
	// resultdlg.cpp:1521
	{ "teamarea", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUniteResultDlg::OnTeamResultInit },
	// resultdlg.cpp:1522
	{ "teamarea", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUniteResultDlg::OnTeamResultOwnerDraw },
	// resultdlg.cpp:1523
	{ "tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUniteResultDlg::OnTooltipOwnerDraw },
	// resultdlg.cpp:1524
	{ "nextlevel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUniteResultDlg::OnNextLevelTextInit },
	// resultdlg.cpp:1526
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// resultdlg.cpp:1532
void FrUniteResultDlg::OnCancelButtonInit(PG_MSGARG param) {
	if (m_pCancelButton == 0) {
		m_pCancelButton = DYNAMIC_CAST(FrButton, param);
	}
}

// resultdlg.cpp:1542
void FrUniteResultDlg::OnAwardInfoInit(PG_MSGARG param) {
	if (m_pAwardInfoButton == 0) {
		m_pAwardInfoButton = DYNAMIC_CAST(FrButton, param);
	}
}

// resultdlg.cpp:1552
void FrUniteResultDlg::OnAwardInfoClick(void) {
	if (m_pAwardInfoDlg == 0) {
		m_pAwardInfoDlg = CreateForm<FrAwardInfoDlg>(g_pFresh->GetManager(), this, "awardinfomation", 0);
		m_pAwardInfoDlg->Open((FRESH_RESULT_CALLBACK)&FrUniteResultDlg::OnAwardInfoDlgResult, 0);
	}
}

// resultdlg.cpp:1565
void FrUniteResultDlg::OnMyInfoAreaInit(PG_MSGARG param) {
	if (m_pMyInfoArea == 0) {
		m_pMyInfoArea = DYNAMIC_CAST(FrArea, param);
	}
}

// resultdlg.cpp:1577
void FrUniteResultDlg::OnMyInfoAreaDraw(PG_MSGARG param) {
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();

	if (gdi == 0)
		return;

	sRivalData myData;
	if (IsLocalContent((localContentType_t)0x14) && m_bTikiReport) {
		myData = WSingleton<CSharedDoc>::Instance()->m_gameResultList[m_tikiReportRank];
	} else {
		if (WSingleton<CSharedDoc>::Instance()->m_gameResultList.size() <= MyIndex())
			return;

		myData = WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()];
	}

	WPoint areaPos(m_pMyInfoArea->GetRect().x, m_pMyInfoArea->GetRect().y);

	float lineY = 0.0f;
	int space = gdi->SetSpace(0);

	unsigned char bgIndex;
	if (m_resultDlgType == 0)
		bgIndex = 2;
	else
		bgIndex = myData.m_team;

	if (m_pRankBgBitmaps[bgIndex]) {
		float bgX = areaPos.x + 13.0f;
		float bgY = (11.0f - (float)(m_pRankBgBitmaps[bgIndex]->Height() / 2)) + areaPos.y;
		WRect bgRect(bgX, bgY, (float)m_pRankBgBitmaps[bgIndex]->Width(), (float)m_pRankBgBitmaps[bgIndex]->Height());

		gdi->DrawTexture(m_pRankBgBitmaps[bgIndex], bgRect, 0xFFFFFFFF, 0);

		gdi->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
		gdi->SetTextStyle(1);

		float rankX = (float)(m_pRankBgBitmaps[bgIndex]->Width() / 2) + bgX;
		bgY += 2.0f;

		unsigned char rank;
		if (m_resultDlgType == 2)
			rank = m_uniteGuildRank;
		else
			rank = myData.m_rank;

		WPoint rankPos(rankX, bgY);
		if (rank < 0xFF)
			gdi->Print(rankPos, 1, "%d", rank);
		else
			gdi->Print(rankPos, 1, "-");

		gdi->SetTextStyle(0);
		gdi->SetTextColor(0xFF000000, 0xFFFFFFFF);

		lineY = bgY;
	}

	m_pLevelBitmap = g_pFresh->GetBitmap(MakeStr("level_%03d", myData.m_level + 1));

	if (m_pLevelBitmap) {
		WRect levelRect(areaPos.x + 47.0f, (11.0f - (float)(m_pLevelBitmap->Height() / 2)) + areaPos.y, (float)m_pLevelBitmap->Width(), (float)m_pLevelBitmap->Height());

		gdi->DrawTexture(m_pLevelBitmap, levelRect, 0xFFFFFFFF, 0);
	}

	if (myData.m_hasEmblem) {
		m_pEmblemBitmap = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(myData.m_emblemName);

		if (m_pEmblemBitmap) {
			WRect emblemRect(areaPos.x + 110.0f, (11.0f - (float)(m_pEmblemBitmap->Height() / 2)) + areaPos.y, (float)m_pEmblemBitmap->Width(), (float)m_pEmblemBitmap->Height());

			gdi->DrawTexture(m_pEmblemBitmap, emblemRect, 0xFFFFFFFF, 0);
		}
	}

	if (m_bTikiReport) {
		if (myData.m_id == WSingleton<CSharedDoc>::Instance()->m_myUID) {
			gdi->SetTextColor(0xFF000000, 0xFFFFFFFF);
			gdi->SetTextStyle(1);
		} else {
			if (myData.m_state == 3)
				gdi->SetTextColor(0xFF808080, 0xFFFFFFFF);
			else
				gdi->SetTextColor(0xFF000000, 0xFFFFFFFF);
			gdi->SetTextStyle(0);
		}
	} else {
		if (myData.m_uid == WSingleton<CSharedDoc>::Instance()->m_userInfo.guid) {
			gdi->SetTextColor(0xFF000000, 0xFFFFFFFF);
			gdi->SetTextStyle(1);
		} else {
			if (myData.m_state == 3)
				gdi->SetTextColor(0xFF808080, 0xFFFFFFFF);
			else
				gdi->SetTextColor(0xFF000000, 0xFFFFFFFF);
			gdi->SetTextStyle(0);
		}
	}

	WPoint namePos(areaPos.x + 141.0f, lineY);
	g_pFresh->GetManager()->PrintText(namePos, 0, myData.m_nickname, -1.0f, 0xFFFFFFFF);

	WPoint scorePos(areaPos.x + 270.0f, lineY);
	if (m_resultDlgType == 2)
		gdi->Print(scorePos, 2, "%d", myData.m_uniteGuildIndex);
	else
		gdi->Print(scorePos, 2, "%d", myData.m_score);

	WPoint pangPos(areaPos.x + 340.0f, lineY);
	gdi->Print(pangPos, 2, "%I64d", myData.m_pang);

	WPoint bonusPos(areaPos.x + 347.0f, lineY);
	gdi->SetTextColor(0xFF006493, 0xFFFFFFFF);

	if (myData.m_extraPang > 0) {
		gdi->SetTextStyle(1);
		gdi->Print(bonusPos, 0, "(+%I64d)", myData.m_extraPang);
	} else {
		gdi->Print(bonusPos, 0, "(0)");
	}

	gdi->SetTextStyle(0);
	gdi->SetTextColor(0xFF000000, 0xFFFFFFFF);

	bool drawn = false;

	if (myData.m_identity & 0x80) {
		if (m_pBonusIcon) {
			WRect iconRect(areaPos.x + 408.0f, (11.0f - (float)(m_pBonusIcon->Height() / 2)) + areaPos.y, (float)m_pBonusIcon->Width(), (float)m_pBonusIcon->Height());

			gdi->DrawTexture(m_pBonusIcon, iconRect, 0xFFFFFFFF, 0);
			drawn = true;
		}
	}

	IFF_ITEM_COMMON* pMascot = ItemManager()->FindCommonItem(myData.m_mascotTypeId);

	if (pMascot) {
		WRect mascotRect(areaPos.x + 453.0f, (11.0f - (float)(m_pBonusMascot01->Height() / 2)) + areaPos.y, (float)m_pBonusMascot01->Width(), (float)m_pBonusMascot01->Height());

		if (pMascot->m_typeId == 0x40000001) {
			gdi->DrawTexture(m_pBonusMascot01, mascotRect, 0xFFFFFFFF, 0);
			drawn = true;
		} else if (pMascot->m_typeId == 0x40000002) {
			gdi->DrawTexture(m_pBonusMascot02, mascotRect, 0xFFFFFFFF, 0);
			drawn = true;
		} else if (pMascot->m_typeId == 0x40000009) {
			gdi->DrawTexture(m_pBonusMascot09, mascotRect, 0xFFFFFFFF, 0);
			drawn = true;
		} else if (pMascot->m_typeId == 0x4000000A) {
			gdi->DrawTexture(m_pBonusMascot10, mascotRect, 0xFFFFFFFF, 0);
			drawn = true;
		} else if (pMascot->m_typeId == 0x4000000B) {
			gdi->DrawTexture(m_pBonusMascot11, mascotRect, 0xFFFFFFFF, 0);
			drawn = true;
		} else if (pMascot->m_typeId == 0x4000000C) {
			gdi->DrawTexture(m_pBonusMascot12, mascotRect, 0xFFFFFFFF, 0);
			drawn = true;
		}
	}

	if (myData.m_itemBoostFlags == 1) {
		WRect maRect(areaPos.x + 470.0f, (11.0f - (float)(m_pBonusMa->Height() / 2)) + areaPos.y, (float)m_pBonusMa->Width(), (float)m_pBonusMa->Height());

		gdi->DrawTexture(m_pBonusMa, maRect, 0xFFFFFFFF, 0);
		drawn = true;
	}

	if (myData.m_extraFlags == 1) {
		WRect ma2Rect(0.0f, 0.0f, (float)m_pBonusMa2->Width(), (float)m_pBonusMa2->Height());

		if (myData.m_itemBoostFlags == 1) {
			ma2Rect.x = (float)(m_pBonusMa2->Width() / 2) + areaPos.x + 470.0f;
			ma2Rect.y = (11.0f - (float)(m_pBonusMa2->Height() / 2)) + areaPos.y;
		} else {
			ma2Rect.x = areaPos.x + 470.0f;
			ma2Rect.y = (11.0f - (float)(m_pBonusMa2->Height() / 2)) + areaPos.y;
		}

		gdi->DrawTexture(m_pBonusMa2, ma2Rect, 0xFFFFFFFF, 0);
	} else if (drawn != true) {
		if (m_pZeroIcon) {
			WRect zeroRect(areaPos.x + 440.0f, (11.0f - (float)(m_pZeroIcon->Height() / 2)) + areaPos.y, (float)m_pZeroIcon->Width(), (float)m_pZeroIcon->Height());

			gdi->DrawTexture(m_pZeroIcon, zeroRect, 0xFFFFFFFF, 0);
		}
	}

	gdi->SetSpace(space);
}

// resultdlg.cpp:1921
void FrUniteResultDlg::OnExpGaugeBarInit(PG_MSGARG param) {
	if (m_pExpGaugeBar == 0) {
		m_pExpGaugeBar = DYNAMIC_CAST(FrGaugeBar, param);
	}
}

// resultdlg.cpp:1931
void FrUniteResultDlg::OnExpAreaInit(PG_MSGARG param) {
	if (m_pExpArea == 0) {
		m_pExpArea = DYNAMIC_CAST(FrArea, param);
	}
}

// resultdlg.cpp:1941
void FrUniteResultDlg::OnExpAreaOwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi) {
		int space = gdi->SetSpace(0);

		WRect rect = m_pExpGaugeBar->GetRect();
		WPoint pos(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f - 5.0f);

		if (!m_bLevelUp) {
			gdi->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
			gdi->SetTextStyle(1);

			gdi->Print(pos, 1, "%d / %d", m_exp / 10, m_nextLevelExp / 10);

			gdi->SetTextColor(0xFF000000, 0xFFFFFFFF);
			gdi->SetTextStyle(0);
		} else {
			const Bitmap* bitmap = g_pFresh->GetBitmap("font_levelup");

			if (bitmap) {
				gdi->DrawTexture(bitmap, WRect(pos.x - bitmap->Width() / 2, pos.y - (rect.h * 0.5f - 5.0f), bitmap->Width(), bitmap->Height()), 0xFFFFFFFF, 0);
			}
		}

		gdi->SetSpace(space);
	}
}

// resultdlg.cpp:1988
void FrUniteResultDlg::OnRankListBoxInit(PG_MSGARG param) {
	if (m_pRankListBox == 0) {
		m_pRankListBox = DYNAMIC_CAST(FrListBox, param);
	}

	m_pRankListBox->UseRightButton(true);
}

// resultdlg.cpp:2001
void FrUniteResultDlg::OnRankListOwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi == 0)
		return;

	FrListItem* pItem = (FrListItem*)param;

	if (pItem == 0)
		return;

	sRivalData* pData = (sRivalData*)pItem->pData;
	if (pData == 0)
		return;

	float lineY = 0.0f;
	int space = gdi->SetSpace(0);

	if (pItem->selected == true && !(m_bTikiReport && IsLocalContent((localContentType_t)0x14))) {
		WRect selRect(pItem->pos.x + 4.0f, pItem->pos.y, (float)m_pRankListBox->GetItemWidth(), (float)m_pRankListBox->GetItemHeight());

		gdi->Box(selRect, 0x999B9DFF, 0, 0.0f);
	}

	unsigned char bgIndex = 2;
	if (m_resultDlgType)
		bgIndex = pData->m_team;

	const Bitmap* pBg;
	if (bgIndex < 3 && (pBg = m_pRankBgBitmaps[bgIndex]) != 0) {
		WPoint pos = pItem->pos;

		float bgX = pos.x + 13.0f;
		float bgY = (11.0f - (float)pBg->Height() * 0.5f) + pos.y;

		WRect bgRect(bgX, bgY, (float)m_pRankBgBitmaps[bgIndex]->Width(), (float)m_pRankBgBitmaps[bgIndex]->Height());

		gdi->DrawTexture(pBg, bgRect, 0xFFFFFFFF, 0);

		gdi->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
		gdi->SetTextStyle(1);

		float rankX = (float)m_pRankBgBitmaps[bgIndex]->Width() * 0.5f + bgX;
		bgY += 2.0f;

		int rank;
		if (m_resultDlgType == 2) {
			if (pData->m_uid == MyGuid(false))
				m_uniteGuildRank = pItem->no + 1;

			rank = pItem->no + 1;
		} else {
			rank = pData->m_rank;
		}

		rank < 0xFF ? gdi->Print(WPoint(rankX, bgY), 1, "%d", rank) : gdi->Print(WPoint(rankX, bgY), 1, "-");

		gdi->SetTextStyle(0);
		gdi->SetTextColor(0xFF000000, 0xFFFFFFFF);

		lineY = bgY;
	}

	m_pLevelBitmap = g_pFresh->GetBitmap(MakeStr("level_%03d", pData->m_level + 1));

	if (m_pLevelBitmap) {
		WRect levelRect(pItem->pos.x + 47.0f, (11.0f - (float)(m_pLevelBitmap->Height() / 2)) + pItem->pos.y, (float)m_pLevelBitmap->Width(), (float)m_pLevelBitmap->Height());

		gdi->DrawTexture(m_pLevelBitmap, levelRect, 0xFFFFFFFF, 0);
	}

	if (pData->m_hasEmblem) {
		m_pEmblemBitmap = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pData->m_emblemName);

		if (m_pEmblemBitmap) {
			WRect emblemRect(pItem->pos.x + 110.0f, (11.0f - (float)(m_pEmblemBitmap->Height() / 2)) + pItem->pos.y, (float)m_pEmblemBitmap->Width(), (float)m_pEmblemBitmap->Height());

			gdi->DrawTexture(m_pEmblemBitmap, emblemRect, 0xFFFFFFFF, 0);
		}
	}

	if (m_bTikiReport) {
		if (pData->m_id == MyUID()) {
			gdi->SetTextColor(0xFF000000, 0xFFFFFFFF);
			gdi->SetTextStyle(1);
		} else {
			if (pData->m_state == 3)
				gdi->SetTextColor(0xFF808080, 0xFFFFFFFF);
			else
				gdi->SetTextColor(0xFF000000, 0xFFFFFFFF);
			gdi->SetTextStyle(0);
		}
	} else {
		if (pData->m_uid == MyGuid(false)) {
			gdi->SetTextColor(0xFF000000, 0xFFFFFFFF);
			gdi->SetTextStyle(1);
		} else {
			if (pData->m_state == 3)
				gdi->SetTextColor(0xFF808080, 0xFFFFFFFF);
			else
				gdi->SetTextColor(0xFF000000, 0xFFFFFFFF);
			gdi->SetTextStyle(0);
		}
	}

	WPoint namePos(pItem->pos.x + 141.0f, lineY);
	g_pFresh->GetManager()->PrintText(namePos, 0, pData->m_nickname, -1.0f, 0xFFFFFFFF);

	WPoint scorePos(pItem->pos.x + 270.0f, lineY);
	if (m_resultDlgType == 2)
		gdi->Print(scorePos, 2, "%d", pData->m_uniteGuildIndex);
	else
		gdi->Print(scorePos, 2, "%d", pData->m_score);

	WPoint pangPos(pItem->pos.x + 340.0f, lineY);
	gdi->Print(pangPos, 2, "%I64d", pData->m_pang);

	WPoint bonusPos(pItem->pos.x + 347.0f, lineY);
	gdi->SetTextColor(0xFF006493, 0xFFFFFFFF);

	if (pData->m_extraPang > 0) {
		gdi->SetTextStyle(1);
		gdi->Print(bonusPos, 0, "(+%I64d)", pData->m_extraPang);
	} else {
		gdi->Print(bonusPos, 0, "(0)");
	}

	gdi->SetTextColor(0xFF000000, 0xFFFFFFFF);
	gdi->SetTextStyle(0);

	bool drawn = false;

	if (pData->m_identity & 0x80) {
		if (m_pBonusIcon) {
			WRect iconRect(pItem->pos.x + 408.0f, (11.0f - (float)(m_pBonusIcon->Height() / 2)) + pItem->pos.y, (float)m_pBonusIcon->Width(), (float)m_pBonusIcon->Height());

			gdi->DrawTexture(m_pBonusIcon, iconRect, 0xFFFFFFFF, 0);
			drawn = true;
		}
	}

	IFF_ITEM_COMMON* pMascot = ItemManager()->FindCommonItem(pData->m_mascotTypeId);

	if (pMascot) {
		WRect mascotRect(pItem->pos.x + 453.0f, (11.0f - (float)(m_pBonusMascot01->Height() / 2)) + pItem->pos.y, (float)m_pBonusMascot01->Width(), (float)m_pBonusMascot01->Height());

		if (pMascot->m_typeId == 0x40000001) {
			gdi->DrawTexture(m_pBonusMascot01, mascotRect, 0xFFFFFFFF, 0);
			drawn = true;
		} else if (pMascot->m_typeId == 0x40000002) {
			gdi->DrawTexture(m_pBonusMascot02, mascotRect, 0xFFFFFFFF, 0);
			drawn = true;
		} else if (pMascot->m_typeId == 0x40000009) {
			gdi->DrawTexture(m_pBonusMascot09, mascotRect, 0xFFFFFFFF, 0);
			drawn = true;
		} else if (pMascot->m_typeId == 0x4000000A) {
			gdi->DrawTexture(m_pBonusMascot10, mascotRect, 0xFFFFFFFF, 0);
			drawn = true;
		} else if (pMascot->m_typeId == 0x4000000B) {
			gdi->DrawTexture(m_pBonusMascot11, mascotRect, 0xFFFFFFFF, 0);
			drawn = true;
		} else if (pMascot->m_typeId == 0x4000000C) {
			gdi->DrawTexture(m_pBonusMascot12, mascotRect, 0xFFFFFFFF, 0);
			drawn = true;
		}
	}

	if (pData->m_itemBoostFlags == 1) {
		WRect maRect(pItem->pos.x + 470.0f, (11.0f - (float)(m_pBonusMa->Height() / 2)) + pItem->pos.y, (float)m_pBonusMa->Width(), (float)m_pBonusMa->Height());

		gdi->DrawTexture(m_pBonusMa, maRect, 0xFFFFFFFF, 0);
		drawn = true;
	}

	if (pData->m_extraFlags == 1) {
		WRect ma2Rect(0.0f, 0.0f, (float)m_pBonusMa2->Width(), (float)m_pBonusMa2->Height());

		if (pData->m_itemBoostFlags == 1) {
			ma2Rect.x = pItem->pos.x + ((float)(m_pBonusMa2->Width() / 2) + 470.0f);
			ma2Rect.y = (11.0f - (float)(m_pBonusMa2->Height() / 2)) + pItem->pos.y;
		} else {
			ma2Rect.x = pItem->pos.x + 470.0f;
			ma2Rect.y = (11.0f - (float)(m_pBonusMa2->Height() / 2)) + pItem->pos.y;
		}

		gdi->DrawTexture(m_pBonusMa2, ma2Rect, 0xFFFFFFFF, 0);
	} else if (drawn != true) {
		if (m_pZeroIcon) {
			WRect zeroRect(pItem->pos.x + 440.0f, (11.0f - (float)(m_pZeroIcon->Height() / 2)) + pItem->pos.y, (float)m_pZeroIcon->Width(), (float)m_pZeroIcon->Height());

			gdi->DrawTexture(m_pZeroIcon, zeroRect, 0xFFFFFFFF, 0);
		}
	}

	int medal = -1;
	if (m_resultDlgType != 3 && !(IsLocalContent((localContentType_t)0x2B) && WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_userLimit >= 100)) {
		if (pData->m_hole == GetStartHole()) {
			medal = -1;
		} else {
			medal = GetMedalIndex(pData->m_uid);

			if (medal >= 0 && medal <= 2) {
				if (pData->m_characterType > 3 && !m_bTikiReport)
					medal = 3;
			}
		}
	}

	float offsetX = 0.0f;
	if (medal >= 0 && medal <= 2) {
		WRect medalRect(pItem->pos.x + 515.0f, (11.0f - (float)(m_pRankMedals[medal]->Height() / 2)) + pItem->pos.y, (float)m_pRankMedals[medal]->Width(), (float)m_pRankMedals[medal]->Height());

		gdi->DrawTexture(m_pRankMedals[medal], medalRect, 0xFFFFFFFF, 0);

		offsetX += (float)m_pRankMedals[medal]->Width();
	}

	for (int i = 0; i <= 5; i++) {
		if (WSingleton<CSharedDoc>::Instance()->m_awardPlayers[i].m_uid == pData->m_uid) {
			WRect prizeRect(pItem->pos.x + 515.0f + offsetX, (11.0f - (float)(m_pPrizeBitmaps[i]->Height() / 2)) + pItem->pos.y, (float)m_pPrizeBitmaps[i]->Width(), (float)m_pPrizeBitmaps[i]->Height());

			gdi->DrawTexture(m_pPrizeBitmaps[i], prizeRect, 0xFFFFFFFF, 0);
			if (!m_bKeepTooltip[0])
				m_bKeepTooltip[0] = SetTooltip(prizeRect, i);

			offsetX += (float)m_pPrizeBitmaps[i]->Width();
		}
	}

	if ((int)offsetX == 0) {
		if (m_pZeroIcon) {
			WRect zeroRect(pItem->pos.x + 515.0f, (11.0f - (float)(m_pZeroIcon->Height() / 2)) + pItem->pos.y, (float)m_pZeroIcon->Width(), (float)m_pZeroIcon->Height());

			gdi->DrawTexture(m_pZeroIcon, zeroRect, 0xFFFFFFFF, 0);
		}
	}

	gdi->SetSpace(space);
}

// resultdlg.cpp:2493
void FrUniteResultDlg::OnRankListLBtnUp(void) {
	if (m_bTikiReport)
		return;

	FrListItem* pItem = m_pRankListBox->GetItemUnderCursor();

	if (pItem == 0)
		return;

	sGameResultInfo* pInfo = (sGameResultInfo*)pItem->pData;

	if (pInfo == 0)
		return;

	m_selectedUid = pInfo->m_uid;

	m_pRankListBox->SelectItem(pItem, true);

	if (m_pScoreDlg == 0) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
			m_pScoreDlg = CreateForm<FrScoreDlg>(g_pFresh->GetManager(), this, "score_guild", 0);
			m_pScoreDlg->Open((FRESH_RESULT_CALLBACK)&FrUniteResultDlg::OnScoreDlgResult, 0);
			m_pScoreDlg->EnableDrag(false);

			m_pScoreDlg->SetGuildPlayer(m_selectedUid);
			m_pScoreDlg->SetVisibleCloseBtn(false);
		} else {
			m_pScoreDlg = CreateForm<FrScoreDlg>(g_pFresh->GetManager(), this, "score", 0);
			m_pScoreDlg->Open((FRESH_RESULT_CALLBACK)&FrUniteResultDlg::OnScoreDlgResult, 0);
			m_pScoreDlg->EnableDrag(false);

			if (m_selectedUid == 0xFFFFFFFF)
				m_selectedUid = WSingleton<CSharedDoc>::Instance()->m_userInfo.guid;

			m_pScoreDlg->SetPlayer(m_selectedUid);
			m_pScoreDlg->SetVisibleCloseBtn(true);
		}
	}

	if (m_pScoreDlg)
		m_pScoreDlg->SetPlayer(m_selectedUid);
}

// resultdlg.cpp:2543
void FrUniteResultDlg::OnRankListRBtnUp(void) {
	if (m_bTikiReport)
		return;

	FrListItem* pItem = m_pRankListBox->GetItemUnderCursor();

	if (pItem == 0)
		return;

	sGameResultInfo* pInfo = (sGameResultInfo*)pItem->pData;

	if (pInfo == 0)
		return;

	m_selectedUid = pInfo->m_uid;

	if (m_pScoreDlg)
		m_pScoreDlg->SetPlayer(m_selectedUid);

	if (WSingleton<CUserInfo>::Instance() && pInfo->m_state != 3) {
		CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
		std::map<unsigned long, sBriefUserInfo>::iterator it = doc->m_userMap.find(pInfo->m_uid);
		if (it != doc->m_userMap.end())
			it->second.roomNumber = doc->m_roomInfo.m_roomNumber;

		WSingleton<CUserInfo>::Instance()->SetInfo(pInfo->m_id, pInfo->m_uid, true, true, false, false, pInfo->m_nickname);

		FrUserInfoForm* pDlg = USERINFODLG();
		if (pDlg)
			pDlg->DisableFlag(1);
	}
}

// resultdlg.cpp:2580
void FrUniteResultDlg::OnDescAreaInit(PG_MSGARG param) {
	if (m_pDescArea == 0) {
		m_pDescArea = DYNAMIC_CAST(FrArea, param);
	}
}

// resultdlg.cpp:2590
void FrUniteResultDlg::OnDescAreaOwnerDraw(PG_MSGARG param) {
	WRect rect = m_pDescArea->GetRect();

	rect.x += 15.0f;
	rect.y -= 1.0f;
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	pGDI->SetTextStyle(0);

	const char* text;
	if (m_resultDlgType == 0) {
		text = MakeStr("1\xb5\xee\xc0\xba %s\xb4\xd4\xc0\xcc \xc2\xf7\xc1\xf6\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.", m_winnerData.m_nickname);
	} else {
		if (WSingleton<CSharedDoc>::Instance()->m_winningTeam == 0) {
			text = MakeStr("RED \xc6\xc0\xc0\xcc \xbd\xc2\xb8\xae\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", m_winnerData.m_nickname);
		} else {
			text = MakeStr("BLUE \xc6\xc0\xc0\xcc \xbd\xc2\xb8\xae\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", m_winnerData.m_nickname);
		}
	}

	WPoint textPos = rect.TopLeft();
	if (text) {
		g_pFresh->GetManager()->PrintText(textPos, 0, text, -1.0f, 0xFFFFFFFF);
	}
}

// resultdlg.cpp:2630
void FrUniteResultDlg::OnAwardAreaInit(PG_MSGARG param) {
	if (m_pAwardArea == 0) {
		m_pAwardArea = DYNAMIC_CAST(FrArea, param);
	}
}

// resultdlg.cpp:2640
void FrUniteResultDlg::OnAwardAreaOwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi == 0)
		return;

	sRivalData myData;
	if (m_bTikiReport && IsLocalContent((localContentType_t)0x14))
		myData = WSingleton<CSharedDoc>::Instance()->m_gameResultList[m_tikiReportRank];
	else
		myData = WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()];

	IFF_STRUCT::sMatch* pMatch = ItemManager()->FindMatch(WSingleton<CSharedDoc>::Instance()->m_matchTypeId);

	WRect rc = GetRect();
	float x = GetRect().x + 505.0f;

	bool drawn = false;
	if (pMatch) {
		if (WSingleton<CSharedDoc>::Instance()->m_value456C >= 1 && WSingleton<CSharedDoc>::Instance()->m_value456C <= 3) {
			const Bitmap* pTrophy = g_pFresh->GetBitmap(pMatch->m_trophyIcon[WSingleton<CSharedDoc>::Instance()->m_value456C - 1]);

			if (pTrophy) {
				WRect rect(GetRect().x + (505.0f - (float)(pTrophy->Width() / 2)), GetRect().y + (381.0f - (float)(pTrophy->Height() / 2)), (float)pTrophy->Width(), (float)pTrophy->Height());

				gdi->DrawTexture(pTrophy, rect, 0xFFFFFFFF, 0);

				x += (float)(pTrophy->Width() / 2);
				drawn = true;
			}
		}
	}

	for (int i = 0; i <= 5; i++) {
		if (m_bTikiReport) {
			if (WSingleton<CSharedDoc>::Instance()->m_awardPlayers[i].m_uid != myData.m_id)
				continue;

			WRect rect(x, (381.0f - (float)(m_pPrizeBitmaps[i]->Height() / 2)) + rc.y, (float)m_pPrizeBitmaps[i]->Width(), (float)m_pPrizeBitmaps[i]->Height());

			gdi->DrawTexture(m_pPrizeBitmaps[i], rect, 0xFFFFFFFF, 0);

			x += (float)m_pPrizeBitmaps[i]->Width();

			if (!m_bKeepTooltip[1])
				m_bKeepTooltip[1] = SetTooltip(rect, i);
		} else {
			if (WSingleton<CSharedDoc>::Instance()->m_awardPlayers[i].m_uid != MyGuid(false))
				continue;

			WRect rect(x, (381.0f - (float)(m_pPrizeBitmaps[i]->Height() / 2)) + rc.y, (float)m_pPrizeBitmaps[i]->Width(), (float)m_pPrizeBitmaps[i]->Height());

			gdi->DrawTexture(m_pPrizeBitmaps[i], rect, 0xFFFFFFFF, 0);

			x += (float)m_pPrizeBitmaps[i]->Width();

			if (!m_bKeepTooltip[1])
				m_bKeepTooltip[1] = SetTooltip(rect, i);
		}

		drawn = true;
	}

	if (!drawn) {
		const Bitmap* pZero = g_pFresh->GetBitmap("icon_zero_vs");

		if (pZero) {
			WRect rect(GetRect().x + 522.0f, GetRect().y + (381.0f - (float)(pZero->Height() / 2)), (float)pZero->Width(), (float)pZero->Height());

			gdi->DrawTexture(pZero, rect, 0xFFFFFFFF, 0);
		}
	}
}

// resultdlg.cpp:2802
void FrUniteResultDlg::OnItemAreaInit(PG_MSGARG param) {
	if (m_pItemArea == 0) {
		m_pItemArea = DYNAMIC_CAST(FrArea, param);
	}
}

// resultdlg.cpp:2812
void FrUniteResultDlg::OnItemAreaOwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();

	if (gdi == 0)
		return;

	WRect rect = GetRect();

	float offset = 0.0f;
	float couponOffset = 0.0f;
	const Bitmap* pBitmap = 0;

	// The award players' prize items (sAwardPlayer::m_pang holds a type id here).
	for (int i = 0; i < 12; i++) {
		if (WSingleton<CSharedDoc>::Instance()->m_awardPlayers[i].m_uid == MyGuid(true) && WSingleton<CSharedDoc>::Instance()->m_awardPlayers[i].m_pang) {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(WSingleton<CSharedDoc>::Instance()->m_awardPlayers[i].m_pang);
			if (pItem) {
				switch ((unsigned long)WSingleton<CSharedDoc>::Instance()->m_awardPlayers[i].m_pang >> 26) {
				case 5:
					pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", pItem->m_icon);
					break;
				case 6:
					pBitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", pItem->m_icon);
					break;
				case 2:
					pBitmap = g_pFresh->GetManager()->GetBitmap("FASHION", pItem->m_icon);
					break;
				default:
					pBitmap = 0;
					continue;
				}
			}

			if (pBitmap) {
				if (ItemManager()->GetCouponKind(WSingleton<CSharedDoc>::Instance()->m_awardPlayers[i].m_pang) == 1)
					couponOffset = -10.0f;

				gdi->DrawTexture(pBitmap, WRect(372.0f - ((float)pBitmap->Width() + offset) + rect.x, 382.0f - (float)((pBitmap->Height() - 23) / 2) + rect.y + couponOffset, (float)pBitmap->Width(), (float)pBitmap->Height()), 0xFFFFFFFF, 0);

				offset += (float)pBitmap->Width();
			}
		}
	}

	// The prizes won, with their count.
	for (std::map<unsigned long, int>::iterator it = m_prizeCountMap.begin(); it != m_prizeCountMap.end(); ++it) {
		unsigned long typeId = it->first;
		int count = it->second;

		IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(typeId);
		if (pItem == 0)
			continue;

		const Bitmap* pIcon = g_pFresh->GetBitmap(pItem->m_icon);
		if (pIcon == 0)
			continue;

		WRect iconRect(372.0f - ((float)pIcon->Width() + offset) + rect.x, 382.0f - (float)((pIcon->Height() - 23) / 2) + rect.y, (float)pIcon->Width(), (float)pIcon->Height());
		gdi->DrawTexture(pIcon, iconRect, 0xFFFFFFFF, 0);

		if (count > 1) {
			char text[8] = "";
			itoa(count, text, 10);
			m_pTitleFont2->Print(g_view, iconRect.x + 30.0f, iconRect.y + 35.0f, text, 0, 0xFFFFFFFF, 0);
		}

		offset += (float)pIcon->Width() * 0.75f;
	}

	if (offset < 0.01f) {
		const Bitmap* pZero = g_pFresh->GetBitmap("icon_zero_vs");

		gdi->DrawTexture(pZero, WRect(rect.x + 260.0f, 382.0f - (float)(pZero->Height() / 2) + rect.y, (float)pZero->Width(), (float)pZero->Height()), 0xFFFFFFFF, 0);
	}
}

// resultdlg.cpp:2960
void FrUniteResultDlg::OnTeamResultInit(PG_MSGARG param) {
	if (m_pTeamResultArea == 0) {
		m_pTeamResultArea = DYNAMIC_CAST(FrArea, param);
	}
}

// resultdlg.cpp:2970
void FrUniteResultDlg::OnTeamResultOwnerDraw(PG_MSGARG param) {
	if (m_resultDlgType == 0)
		return;

	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();

	if (gdi == 0)
		return;

	WRect rect = GetRect();

	WRect redBgRect(rect.x + 13.0f, (468.0f - (float)(m_pRankBgBitmaps[0]->Height() / 2)) + rect.y, (float)m_pRankBgBitmaps[0]->Width(), (float)m_pRankBgBitmaps[0]->Height());
	gdi->DrawTexture(m_pRankBgBitmaps[0], redBgRect, 0xFFFFFFFF, 0);

	WRect blueBgRect(rect.x + 13.0f, (492.0f - (float)(m_pRankBgBitmaps[1]->Height() / 2)) + rect.y, (float)m_pRankBgBitmaps[1]->Width(), (float)m_pRankBgBitmaps[1]->Height());
	gdi->DrawTexture(m_pRankBgBitmaps[1], blueBgRect, 0xFFFFFFFF, 0);

	if (m_resultDlgType == 1) {
		const Bitmap* pRed = g_pFresh->GetBitmap("icon_redteam");
		if (pRed) {
			gdi->DrawTexture(pRed, WRect((117.0f - (float)(pRed->Width() / 2)) + rect.x, (468.0f - (float)(pRed->Height() / 2)) + rect.y, (float)pRed->Width(), (float)pRed->Height()), 0xFFFFFFFF, 0);
		}

		const Bitmap* pBlue = g_pFresh->GetBitmap("icon_blueteam");
		if (pBlue) {
			gdi->DrawTexture(pBlue, WRect((117.0f - (float)(pBlue->Width() / 2)) + rect.x, (492.0f - (float)(pBlue->Height() / 2)) + rect.y, (float)pBlue->Width(), (float)pBlue->Height()), 0xFFFFFFFF, 0);
		}
	} else {
		const Bitmap* pRedMark = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_guild.m_guildMark[0]);
		if (pRedMark) {
			gdi->DrawTexture(pRedMark, WRect((117.0f - (float)(pRedMark->Width() / 2)) + rect.x, (471.0f - (float)(pRedMark->Height() / 2)) + rect.y, (float)pRedMark->Width(), (float)pRedMark->Height()), 0xFFFFFFFF, 0);
		}

		const Bitmap* pBlueMark = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_guild.m_guildMark[1]);
		if (pBlueMark) {
			gdi->DrawTexture(pBlueMark, WRect((117.0f - (float)(pBlueMark->Width() / 2)) + rect.x, (492.0f - (float)(pBlueMark->Height() / 2)) + rect.y, (float)pBlueMark->Width(), (float)pBlueMark->Height()), 0xFFFFFFFF, 0);
		}

		const WRect& areaRect = m_pTeamResultArea->GetRect();
		g_pFresh->GetManager()->PrintText(WPoint(areaRect.x + 120.0f, areaRect.y + 17.0f), 0, WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_guild.m_guildName[0], -1.0f, 0xFFFFFFFF);
		g_pFresh->GetManager()->PrintText(WPoint(areaRect.x + 120.0f, areaRect.y + 39.0f), 0, WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_guild.m_guildName[1], -1.0f, 0xFFFFFFFF);
	}

	rect = GetRect();

	gdi->Print(WPoint(rect.x + 277.0f, rect.y + 463.0f), 1, "%d", WSingleton<CSharedDoc>::Instance()->m_teamResult[0].m_score);
	gdi->Print(WPoint(rect.x + 277.0f, rect.y + 487.0f), 1, "%d", WSingleton<CSharedDoc>::Instance()->m_teamResult[1].m_score);

	gdi->Print(WPoint(rect.x + 357.0f, rect.y + 463.0f), 1, "%I64d", WSingleton<CSharedDoc>::Instance()->m_teamResult[0].m_pang);
	gdi->Print(WPoint(rect.x + 357.0f, rect.y + 487.0f), 1, "%I64d", WSingleton<CSharedDoc>::Instance()->m_teamResult[1].m_pang);

	const Bitmap* pResult;
	if (WSingleton<CSharedDoc>::Instance()->m_winningTeam == 0) {
		pResult = g_pFresh->GetBitmap("30in_team_redteam_win");
	} else if (WSingleton<CSharedDoc>::Instance()->m_winningTeam == 1) {
		pResult = g_pFresh->GetBitmap("30in_team_blueteam_win");
	} else {
		if (IsLocalContent((localContentType_t)0x14) && m_bTikiReport) {
			if (WSingleton<CSharedDoc>::Instance()->m_gameResultList[m_tikiReportRank].m_team == 0)
				pResult = g_pFresh->GetBitmap("30in_team_redteam_draw");
			else
				pResult = g_pFresh->GetBitmap("30in_team_blueteam_draw");
		} else {
			if (WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_team == 0)
				pResult = g_pFresh->GetBitmap("30in_team_redteam_draw");
			else
				pResult = g_pFresh->GetBitmap("30in_team_blueteam_draw");
		}
	}

	if (pResult) {
		m_teamAnimTime = m_teamAnimTime < 3.14f ? m_teamAnimTime : 3.14f;

		float scale = (float)sin(m_teamAnimTime) * 0.2f + 1.0f;

		gdi->DrawTexture(pResult, WRect(522.0f - (float)pResult->Width() * scale * 0.5f + rect.x, 482.0f - (float)pResult->Height() * scale * 0.5f + rect.y, (float)pResult->Width() * scale, (float)pResult->Height() * scale), 0xFFFFFFFF, 0);
	}
}

// resultdlg.cpp:3121
void FrUniteResultDlg::OnTooltipOwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI == 0)
		return;

	if (m_pCurTooltip == 0)
		return;

	WVector mousePos = g_input->GetMousePoint();
	WVector cursorPos = g_input->GetMousePoint();
	float width = (float)m_pCurTooltip->Width();
	float height = (float)m_pCurTooltip->Height();

	WRect rect(mousePos.x + 12.0f, cursorPos.y + 23.0f, width, height);
	if (rect.x >= g_view->GetWidth() - width)
		rect.x = g_view->GetWidth() - width;
	if (rect.y >= g_view->GetHeight() - height)
		rect.y = g_view->GetHeight() - height;

	pGDI->DrawTexture(m_pCurTooltip, rect, 0xFFFFFFFF, 0);
}

// resultdlg.cpp:3147
void FrUniteResultDlg::OnNextLevelTextInit(PG_MSGARG param) {
	if (m_pNextLevelText == 0) {
		m_pNextLevelText = DYNAMIC_CAST(FrEdit, param);
	}
}

// resultdlg.cpp:3156
FrUniteResultDlg::FrUniteResultDlg(void) {
	m_pCancelButton = 0;
	m_pAwardInfoButton = 0;
	m_pRankListBox = 0;
	m_pMyInfoArea = 0;
	m_pAwardInfoDlg = 0;
	m_pScoreDlg = 0;

	m_pTreasureScoreDlg = 0;
	m_pTreasureGiftDlg = 0;

	m_pExpGaugeBar = 0;
	m_pExpArea = 0;
	m_pDescArea = 0;
	m_pAwardArea = 0;
	m_pItemArea = 0;
	m_pTeamResultArea = 0;

	m_pNextLevelText = 0;

	m_pRankBgBitmaps[0] = 0;
	m_pRankBgBitmaps[1] = 0;
	m_pRankBgBitmaps[2] = 0;

	m_pLevelBitmap = 0;
	m_pEmblemBitmap = 0;
	m_pBonusIcon = 0;
	m_pBonusMascot02 = 0;
	m_pBonusMascot01 = 0;

	m_pBonusMascot09 = 0;
	m_pBonusMascot10 = 0;
	m_pBonusMascot11 = 0;
	m_pBonusMascot12 = 0;

	m_pBonusMa = 0;
	m_pBonusMa2 = 0;

	m_pRankMedals[0] = 0;
	m_pRankMedals[1] = 0;
	m_pRankMedals[2] = 0;
	m_pRankMedals[3] = 0;

	m_pPrizeBitmaps[0] = 0;
	m_pPrizeBitmaps[1] = 0;
	m_pPrizeBitmaps[2] = 0;
	m_pPrizeBitmaps[3] = 0;
	m_pPrizeBitmaps[4] = 0;
	m_pPrizeBitmaps[5] = 0;

	m_pZeroIcon = 0;
	m_pCurTooltip = 0;

	for (int i = 0; i < 6; i++) {
		m_pTooltips[i] = 0;
	}

	m_expTickTime = 0.0f;
	m_resultDlgType = 0;

	m_bLevelUp = false;
	m_level = 0;
	m_exp = 0;
	m_nextLevelExp = 0;
	m_expGain = 0;
	m_selectedUid = 0xffffffff;
	m_teamAnimTime = 0.0f;
	m_elapsedTime = 0.0f;
	m_uniteGuildRank = 0;
	m_bKeepTooltip[0] = false;
	m_bKeepTooltip[1] = false;

	m_pTitleFont1 = g_resrcmng->GetTitleFont();

	char* angelFontFile = "[font_angel.jpg";
	m_titleFontInfo1.filename = &angelFontFile;
	m_titleFontInfo1.flag = 0;
	m_titleFontInfo1.fonth = 11;
	m_titleFontInfo1.fontw = 6;
	m_titleFontInfo1.numPages = 1;
	m_titleFontInfo1.texw = 128;
	m_titleFontInfo1.texh = 32;
	m_titleFontInfo1.pCharSet = "1234567890";
	m_bTikiReport = false;
	GetLocalTime(&m_tikiReportTime);
	m_tikiReportRank = 0;

	m_pTitleFont1->Create(&m_titleFontInfo1);

	memset(&m_titleFontInfo2, 0, sizeof(m_titleFontInfo2));
	m_titleFontInfo2.filename = &data_ab7900[1];
	m_titleFontInfo2.fontw = 16;
	m_titleFontInfo2.fonth = 16;
	m_titleFontInfo2.numPages = 1;
	m_titleFontInfo2.texw = 128;
	m_titleFontInfo2.texh = 64;
	m_titleFontInfo2.pCharSet = "1234567890ym%-.?/:,";

	m_pTitleFont2 = g_resrcmng->GetTitleFont();
	m_pTitleFont2->Create(&m_titleFontInfo2);
}

// resultdlg.cpp:3276
FrUniteResultDlg::~FrUniteResultDlg(void) {
	if (g_resrcmng && m_pTitleFont2) { g_resrcmng->Release(m_pTitleFont2); m_pTitleFont2 = 0; }
	if (g_resrcmng && m_pTitleFont1) { g_resrcmng->Release(m_pTitleFont1); m_pTitleFont1 = 0; }

	m_bTikiReport = false;
	m_tikiReportRank = 0;
	if (USERINFODLG()) {
		WSingleton<CUserInfo>::Instance()->Close();
	}

	m_pCancelButton = 0;
}

// resultdlg.cpp:3295
bool FrUniteResultDlg::Close(bool bDestroy) {
	if (m_pTreasureScoreDlg) {
		m_pTreasureScoreDlg->Close(bDestroy);
	}

	if (m_pTreasureGiftDlg) {
		m_pTreasureGiftDlg->Close(bDestroy);
	}

	return FrForm::Close(bDestroy);
}

// resultdlg.cpp:3311
bool FrUniteResultDlg::CheckChildDlgVisible(void) {
	if (m_pAwardInfoDlg) {
		if (m_pAwardInfoDlg->IsVisible()) {
			m_pAwardInfoDlg->Close(true);
			return true;
		}
	}

	if (m_pScoreDlg) {
		if (m_pScoreDlg->IsVisible()) {
			m_pScoreDlg->Close(true);
			return true;
		}
	}

	if (m_pTreasureScoreDlg || m_pTreasureGiftDlg)
		return true;

	if (USERINFODLG()) {
		CUserInfo::Instance()->Close();
		return true;
	}

	return false;
}

// The level titles FrUniteResultDlg::OnInit shows for levels below 21, and
// at index 21 the suffix appended to the higher titles (filled at run time;
// 12-byte entries).
struct sLevelTitleEntry {
	const char* m_text;   // 0x0
	UNKNOWN(0x4, 0xC);
};
CHECK_SIZE(sLevelTitleEntry, 0xC);

extern "C" {
	static sLevelTitleEntry data_ab6f00[22];
	// The level titles from level 21 on (20-byte entries indexed by level).
	static char data_a24954[71][20];
}

// resultdlg.cpp:3348
bool FrUniteResultDlg::OnInit(void) {
	LoadBitmap();
	SetResultDlgType();
	SetDataToRankListBox();
	FindWinner();


	SetPrizeResult();



	if (IsLocalContent((localContentType_t)10)) {

		if (IsAdministrator(0xFFFFFFFF)) {

			CGMToolkit* pToolkit = WSingleton<CGMToolkit>::Instance();


			if (pToolkit) {

				if (pToolkit->GetPrizeSwitch()) {

					pToolkit->SendPrize(m_pRankListBox);
				}
			}
		}
	}

	m_pCancelButton->Enable(false);
	DisableFlag(1);


	m_level = WSingleton<CSharedDoc>::Instance()->m_myLevel;
	m_exp = WSingleton<CSharedDoc>::Instance()->m_myExp * 10;
	m_nextLevelExp = WSingleton<CSharedDoc>::Instance()->m_levelExp[m_level].m_exp * 10;
	m_expGain = WSingleton<CSharedDoc>::Instance()->m_gameStatistics[0].exp * 10;



	m_escClosable = m_expGain == 0;
	if (m_pExpGaugeBar) {

		m_pExpGaugeBar->SetBorder(1);
		m_pExpGaugeBar->SetRange(0, m_nextLevelExp, 0);
		m_pExpGaugeBar->SetPos(m_exp);
	}




	WRect rect = m_pRankListBox->GetRect();
	WPoint pt;
	pt.x = m_pNextLevelText->GetRect().x;
	pt.y = rect.y + rect.h + 100.0f;
	m_pNextLevelText->MoveWindow(pt);


	m_pNextLevelText->SetVisible(IsLocalContent((localContentType_t)0x29));

	unsigned long total = WSingleton<CSharedDoc>::Instance()->m_gameStatistics[0].exp + WSingleton<CSharedDoc>::Instance()->m_myExp;
	if (total > WSingleton<CSharedDoc>::Instance()->m_levelExp[m_level].m_exp) {

		m_pNextLevelText->SetVisible(false);

	} else {














		std::string text;
		if (m_level < 21) {

			text.assign(data_ab6f00[m_level].m_text);

		} else if (m_level <= 65) {

			text.assign(data_a24954[m_level]);
			text += data_ab6f00[21].m_text;
		}


		m_pNextLevelText->AddLine(text.c_str(), 0, false);
	}


	if (m_bTikiReport && IsLocalContent((localContentType_t)0x14)) {


		WSingleton<CSharedDoc>::Instance()->m_value90 = 1;

		if (GalleryMode())
			return true;

		WSingleton<CSharedDoc>::Instance()->m_myExp += WSingleton<CSharedDoc>::Instance()->m_gameStatistics[0].exp;
		WSingleton<CSharedDoc>::Instance()->Log30sScore();

	} else if (WSingleton<CSharedDoc>::Instance()->m_value90 == 0) {

		WSingleton<CSharedDoc>::Instance()->m_value90 = 1;

		if (GalleryMode())
			return true;


		WSendPacket packet((enumClientPacket)6);
		packet.EncodeBuffer(&WSingleton<CSharedDoc>::Instance()->m_gameStatistics[0], sizeof(sPangYaUserStatistics));
		packet.Send(SEND_TO_DEFAULT);

		WSingleton<CSharedDoc>::Instance()->Log30sScore();
	}





	// A block of its own: the packet above shares text's stack slot.
	{
		const char* name;
		char text[256];
		if (m_bTikiReport && IsLocalContent((localContentType_t)0x14)) {








			sprintf(text, "30\xC0\xCE \xB4\xEB\xC8\xB8 %d-%d-%d %d:%d", m_tikiReportTime.wYear, m_tikiReportTime.wMonth, m_tikiReportTime.wDay, m_tikiReportTime.wHour, m_tikiReportTime.wMinute);

			name = text;
		} else {

			unsigned char gameType = WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType;
			switch (gameType) {
			case 4:
			case 5:
			case 6:
			case 9:
			case 10:
			case 14:
				if (gameType != 10) {
					IFF_STRUCT::sMatch* pMatch = ItemManager()->FindMatch(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_trophy);

					if (pMatch) {
						const char* p;

						if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomMode == 14) {
							p = "\xBC\xC5\xC7\xC3\xC4\xDA\xBD\xBA";
						} else {
							p = pMatch->m_name;
						}

						name = p;
					} else {
						unsigned char type = WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType;
						name = type >= 15 ? 0 : WSingleton<CSharedDoc>::Instance()->m_gameTypeNames[type].name;
					}
					break;
				}
				// game type 10 shows the game type name
			default:
				name = gameType >= 15 ? 0 : WSingleton<CSharedDoc>::Instance()->m_gameTypeNames[gameType].name;
				break;
			}
		}

		char typeText[32];
		memset(typeText, 0, sizeof(typeText));

		switch (m_resultDlgType) {
		case 0: {
			const char* p = "\xB0\xB3\xC0\xCE\xC0\xFC";
			strcpy(typeText, p);
			break;
		}
		case 2: {
			const char* p = "\xB1\xE6\xB5\xE5\xC0\xFC";
			strcpy(typeText, p);
			break;
		}
		case 1: {
			const char* p = "\xC6\xC0\xC0\xFC";
			strcpy(typeText, p);
			break;
		}
		}

		SetCaption(MakeStr("            (%s %s)", name, typeText));
	}


	return true;
}

// resultdlg.cpp:3543
void FrUniteResultDlg::OnProc(const float dt) {
	m_bKeepTooltip[0] = false;
	m_bKeepTooltip[1] = false;
	m_expTickTime += dt;
	m_elapsedTime += dt;

	if (m_expTickTime < 1.0f)
		return;

	float over = m_expTickTime - 1.0f;
	if (over < 0.01f)
		return;

	int step = float2int(over * 100.0f);
	int level = m_level;
	m_expTickTime -= step * 0.01f;
	int nextExp = WSingleton<CSharedDoc>::Instance()->m_levelExp[level].m_exp * 10;

	if (step > Abs(m_expGain)) {
		m_escClosable = true;
		m_exp += ((m_expGain > 0) * 2 - 1) * Abs(m_expGain);
		m_expGain = 0;
	} else {
		int delta = ((m_expGain > 0) * 2 - 1) * step;
		m_exp += delta;
		m_expGain -= delta;
	}

	if (m_exp < 0 && !m_escClosable) {
		m_escClosable = true;
		m_exp = 0;
	}

	if (m_exp >= nextExp) {
		m_exp -= nextExp;
		m_level = level + 1;
		m_bLevelUp = true;

		if (m_pExpGaugeBar) {
			m_pExpGaugeBar->SetRange(0, WSingleton<CSharedDoc>::Instance()->m_levelExp[m_level].m_exp * 10, 0);
			m_pExpGaugeBar->SetPos(m_exp);
		}
	} else if (m_pExpGaugeBar) {
		m_pExpGaugeBar->SetPos(m_exp);
	}

	m_teamAnimTime += dt * 2.0f;

	if (m_elapsedTime >= 17.0f)
		m_pRankListBox->UseRightButton(false);

	if (m_escClosable == true) {
		bool bGift = WSingleton<CTHunter>::Instance()->IsReceiveGiftPacket();
		if (!bGift || m_bTikiReport == true) {
			m_pCancelButton->Enable(true);
			m_flag |= 1;
			return;
		}

		if (bGift == true) {
			if (!WSingleton<CSharedDoc>::Instance()->m_value4843) {
				if (!m_pTreasureScoreDlg) {
					m_pTreasureScoreDlg = CreateForm<FrTreasureScore>(g_pFresh->GetManager(), this, "trscore", 0);
					m_pTreasureScoreDlg->Open((FRESH_RESULT_CALLBACK)&FrUniteResultDlg::OnTreasureScoreDlgResult, 0);
					m_pTreasureScoreDlg->EnableDrag(false);
					m_pTreasureScoreDlg->SetFixed(true);
				}

				if (!m_pTreasureGiftDlg) {
					m_pTreasureGiftDlg = CreateForm<FrTreasureGift>(g_pFresh->GetManager(), this, "trgift", 0);
					m_pTreasureGiftDlg->MoveWindow(m_pTreasureGiftDlg->GetCalcPoint());
					m_pTreasureGiftDlg->Open((FRESH_RESULT_CALLBACK)&FrUniteResultDlg::OnTreasureGiftDlgResult, 1);
					m_pTreasureGiftDlg->EnableDrag(false);
					m_pTreasureGiftDlg->SetFixed(true);
				}

				if (m_pTreasureScoreDlg && m_pTreasureGiftDlg) {
					WSingleton<CSharedDoc>::Instance()->m_value4843 = 1;
				}
			}
		}
	}
}

// resultdlg.cpp:3665
void FrUniteResultDlg::LoadBitmap(void) {

	m_pRankBgBitmaps[0] = g_pFresh->GetBitmap("30in_team_redteam_rank");
	m_pRankBgBitmaps[1] = g_pFresh->GetBitmap("30in_team_blueteam_rank");
	m_pRankBgBitmaps[2] = g_pFresh->GetBitmap("30in_rank");


	m_pLevelBitmap = 0;


	m_pEmblemBitmap = 0;


	m_pBonusIcon = g_pFresh->GetBitmap("pc_icon_bonus_20");





	m_pBonusMascot02 = g_pFresh->GetBitmap("bonus_mascot_02");
	m_pBonusMascot01 = g_pFresh->GetBitmap("bonus_mascot_01");

	m_pBonusMascot09 = g_pFresh->GetBitmap("bonus_mascot_09");
	m_pBonusMascot10 = g_pFresh->GetBitmap("bonus_mascot_10");
	m_pBonusMascot11 = g_pFresh->GetBitmap("bonus_mascot_11");
	m_pBonusMascot12 = g_pFresh->GetBitmap("bonus_mascot_12");



	m_pBonusMa = g_pFresh->GetBitmap("bonus_ma");
	m_pBonusMa2 = g_pFresh->GetBitmap("bonus_ma2");


	m_pRankMedals[0] = g_pFresh->GetBitmap("rank_gold");
	m_pRankMedals[1] = g_pFresh->GetBitmap("rank_silver");
	m_pRankMedals[2] = g_pFresh->GetBitmap("rank_bronze");
	m_pRankMedals[3] = g_pFresh->GetBitmap("rank_noaward");


















	m_pPrizeBitmaps[0] = g_pFresh->GetBitmap("prize_goodluck");
	m_pPrizeBitmaps[1] = g_pFresh->GetBitmap("prize_speeder");
	m_pPrizeBitmaps[2] = g_pFresh->GetBitmap("prize_runner");
	m_pPrizeBitmaps[3] = g_pFresh->GetBitmap("prize_chipin");
	m_pPrizeBitmaps[4] = g_pFresh->GetBitmap("prize_long_putting");
	m_pPrizeBitmaps[5] = g_pFresh->GetBitmap("prize_recovery");



	m_pZeroIcon = g_pFresh->GetBitmap("icon_zero");


	m_pTooltips[0] = g_pFresh->GetBitmap("tooltip_goodluck");
	m_pTooltips[1] = g_pFresh->GetBitmap("tooltip_speeder");
	m_pTooltips[2] = g_pFresh->GetBitmap("tooltip_runner");
	m_pTooltips[3] = g_pFresh->GetBitmap("tooltip_chipin");
	m_pTooltips[4] = g_pFresh->GetBitmap("tooltip_longputter");
	m_pTooltips[5] = g_pFresh->GetBitmap("tooltip_recovery");
	m_pTooltips[6] = g_pFresh->GetBitmap("tooltip_winning_contents");
}

// resultdlg.cpp:3744
int FrUniteResultDlg::GetMedalIndex(unsigned long uid) {
	unsigned char memberSize = 0;
	if (IsLocalContent((localContentType_t)0x2E))
		memberSize = WSingleton<CIntrusion>::Instance()->GetCalcExpMemberSize();

	if (memberSize == 0) {
		if (WSingleton<CSharedDoc>::Instance()->m_gameResultList.size() < 10)
			return 3;
	} else {
		if (memberSize < 10)
			return 3;
	}

	unsigned char index = 0xFF;
	int i = 0;
	std::vector<sRivalData>::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it, ++i) {
		if (it->m_uid == uid) {
			index = i;
			break;
		}
	}

	if (index == 0xFF || WSingleton<CSharedDoc>::Instance()->m_gameResultList[index].m_characterType > 3) {
		if (!m_bTikiReport)
			return 3;
	}

	unsigned char myRank;
	if (IsLocalContent((localContentType_t)0x14) && m_bTikiReport) {
		int j = 0;
		for (it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it, ++j) {
			if (it->m_uid == uid)
				return WSingleton<CSharedDoc>::Instance()->m_gameResultList[j].m_holeShots[0] - 1;
		}
	} else {
		myRank = WSingleton<CSharedDoc>::Instance()->m_gameResultList[index].m_rank;
	}

	unsigned char count = 0;
	unsigned char higherCount = 0;
	if (memberSize == 0) {
		for (it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
			unsigned char state = it->m_state;
			if (state == 3 && GetHoleIndex(it->m_hole) < 4 && it->m_state == 3)
				continue;
			count++;

			if (state != 3 && it->m_rank < myRank && it->m_characterType > 3)
				higherCount++;
		}
	} else {
		count = memberSize;

		for (it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
			if (it->m_state != 3 && it->m_rank < myRank && it->m_characterType > 3)
				higherCount++;
		}
	}

	myRank -= higherCount;
	if (WSingleton<CSharedDoc>::Instance()->m_holes == 9) {
		if (count <= 14) {
			return 3;
		} else if (count <= 18) {
			if (myRank == 1)
				return 2;
		} else if (count <= 22) {
			if (myRank == 1)
				return 1;
		} else if (count <= 26) {
			if (myRank == 1)
				return 1;
			if (myRank == 2)
				return 2;
		} else {
			if (myRank == 1)
				return 0;
			if (myRank == 2)
				return 1;
		}
	} else if (WSingleton<CSharedDoc>::Instance()->m_holes == 18) {
		if (count <= 14) {
			if (myRank == 1)
				return 2;
		} else if (count <= 18) {
			if (myRank == 1)
				return 1;
			if (myRank == 2)
				return 2;
		} else if (count <= 22) {
			if (myRank == 1)
				return 0;
			if (myRank == 2)
				return 1;
			if (myRank == 3)
				return 2;
		} else if (count <= 26) {
			if (myRank == 1)
				return 0;
			if (myRank == 2)
				return 1;
			if (myRank <= 4)
				return 2;
		} else {
			if (myRank == 1)
				return 0;
			if (myRank <= 3)
				return 1;
			if (myRank <= 6)
				return 2;
		}
	}

	return 3;
}

// resultdlg.cpp:3926
void FrUniteResultDlg::SetResultDlgType(void) {
	switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
	case 4:
		m_resultDlgType = 0;
		break;
	case 5:
		m_resultDlgType = 1;
		break;
	case 6:
		m_resultDlgType = 2;
		break;
	}
}

// resultdlg.cpp:3945
void FrUniteResultDlg::SetTikiReportState(SYSTEMTIME& time) {
	if (!IsLocalContent((localContentType_t)0x14))
		return;

	m_bTikiReport = true;
	m_tikiReportTime = time;
	m_tikiReportRank = 0;

	int index = 0;
	for (std::vector<sGameResultInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it, ++index) {
		if (it->m_id == WSingleton<CSharedDoc>::Instance()->m_myUID) {
			m_tikiReportRank = index;
			break;
		}
	}
}

// resultdlg.cpp:3969
void FrUniteResultDlg::SetDataToRankListBox(void) {



	std::vector<sGameResultInfo>::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {

		if (it->m_state == 3) {

			it->m_rank = 0xFF;
			continue;
		}

		it->m_rank = 1;

		for (std::vector<sGameResultInfo>::iterator other = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); other != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++other) {

			if (it != other && other->m_state != 3) {




				if (it->m_score > other->m_score || (it->m_score == other->m_score && it->m_pang < other->m_pang)) {

					it->m_rank++;
				}
			}
		}
	}



	for (it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {

		m_pRankListBox->AddItem(&*it);
	}


	if (m_resultDlgType == 2) {

		m_pRankListBox->SortItem(UniteGuildIndexCompare);

	} else {

		if (g_pFresh->IsCurrentLayout("REALMYROOM"))
			m_pRankListBox->SortItem(ReportRankCompare);
		else
			m_pRankListBox->SortItem(UniteRankCompare);
	}


	if ((WSingleton<CSharedDoc>::Instance()->m_userInfo.identity & 4) && IsLocalContent((localContentType_t)0x2B) && WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_userLimit >= 100)
		SaveGMEvent();
}

// resultdlg.cpp:4024
bool FrUniteResultDlg::SetTooltip(WRect rect, int index) {
	if (index >= 0 && index <= 6) {
		if (rect.IsInRect(WPoint(g_input->GetMousePoint().x, g_input->GetMousePoint().y))) {
			m_pCurTooltip = m_pTooltips[index];
			CMouseCursor::Instance()->SetCursor(1);
			return true;
		}

		if (!m_bKeepTooltip[0] && !m_bKeepTooltip[1]) {
			m_pCurTooltip = 0;
		}
	}

	return false;
}

// resultdlg.cpp:4044
void FrUniteResultDlg::FindWinner(void) {

	memset(WSingleton<CSharedDoc>::Instance()->m_teamResult, 0, sizeof(WSingleton<CSharedDoc>::Instance()->m_teamResult));

	for (std::vector<sRivalData>::iterator it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {

		if (it->m_rank == 1) {

			m_winnerData = *it;

			if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 4)
				return;


			if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType == 10)
				return;
		}



		switch (m_resultDlgType) {
		case 2:

			WSingleton<CSharedDoc>::Instance()->m_teamResult[it->m_team].m_score += it->m_uniteGuildIndex;
			WSingleton<CSharedDoc>::Instance()->m_teamResult[it->m_team].m_pang += it->m_pang;
			break;
		case 1:

			WSingleton<CSharedDoc>::Instance()->m_teamResult[it->m_team].m_score += it->m_rank;
			WSingleton<CSharedDoc>::Instance()->m_teamResult[it->m_team].m_pang += it->m_pang;
			break;
		}
	}
}

// resultdlg.cpp:4080
bool FrUniteResultDlg::OnAwardInfoDlgResult(int ret, FrForm* pForm) {
	m_pAwardInfoDlg = 0;
	return true;
}

// resultdlg.cpp:4088
bool FrUniteResultDlg::OnScoreDlgResult(int ret, FrForm* pForm) {
	m_pScoreDlg = 0;
	return true;
}

// resultdlg.cpp:4095
bool FrUniteResultDlg::OnTreasureScoreDlgResult(int ret, FrForm* pForm) {
	m_pTreasureScoreDlg = 0;
	return true;
}

// resultdlg.cpp:4103
bool FrUniteResultDlg::OnTreasureGiftDlgResult(int ret, FrForm* pForm) {
	g_input->ExclusiveGetDownUseDone("ESCAPE");

	if (m_pTreasureScoreDlg) {
		m_pTreasureScoreDlg->Close(true);
	}

	if (m_pCancelButton) {
		m_pCancelButton->Enable(true);
	}

	EnableFlag(1);
	m_pTreasureGiftDlg = 0;

	return true;
}

// resultdlg.cpp:4126
void FrUniteResultDlg::SetPrizeResult(void) {
	m_prizeCountMap.clear();

	std::vector<unsigned long>& gainedItems = WSingleton<CSharedDoc>::Instance()->m_gainedItemList[0];

	for (std::vector<unsigned long>::iterator it = gainedItems.begin(); it != gainedItems.end(); ++it) {
		unsigned long typeId = *it;

		std::map<unsigned long, int>::iterator found = m_prizeCountMap.find(typeId);
		if (found == m_prizeCountMap.end()) {
			m_prizeCountMap.insert(std::pair<const unsigned long, int>(typeId, 1));
		} else {
			found->second++;
		}
	}
}

// <direct.h>'s mkdir (the toolchain has no direct.h yet).
extern "C" __declspec(dllimport) int __cdecl mkdir(const char* dirname);

// resultdlg.cpp:4149
bool FrUniteResultDlg::SaveGMEvent(void) {
	mkdir("log");
	mkdir("log/event");

	SYSTEMTIME st;
	GetLocalTime(&st);

	FILE* fp = fopen(MakeStr("log/event/Result_%04d_%02d_%02d_%02d_%02d.txt", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute), "wt");
	if (!fp)
		return false;

	char buf[512] = { 0 };

	sprintf(buf, "Room Info : %s > %s > %s(%d)\n", WSingleton<CSharedDoc>::Instance()->m_curServer.name, WSingleton<CSharedDoc>::Instance()->m_currentChannel.name, WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_title, WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomNumber);
	fwrite(buf, strlen(buf), 1, fp);

	sprintf(buf, "Rank : uid\tnick\t\thole\tscore\tgamepang\tbonuspang\n");
	fwrite(buf, strlen(buf), 1, fp);

	sprintf(buf, "------------------------------------------------------------\n");
	fwrite(buf, strlen(buf), 1, fp);

	char holeText[32] = { 0 };
	int rank = 1;
	for (FrListBox::ITEM_LIST::iterator it = m_pRankListBox->m_itemList.begin(); it != m_pRankListBox->m_itemList.end(); ++it) {
		FrListItem* pItem = *it;
		sGameResultInfo* pInfo = (sGameResultInfo*)pItem->pData;

		if (pInfo->m_state == 2 || GetHoleIndex(pInfo->m_hole) == 19)
			sprintf(holeText, "End");
		else if (pInfo->m_state == 3)
			sprintf(holeText, "Out");
		else
			sprintf(holeText, "%d", GetHoleIndex(pInfo->m_hole));

		sprintf(buf, "%03d : %d\t%s\t%s\t%d\t%I64d\t%I64d\n", rank++, pInfo->m_id, pInfo->m_nickname, holeText, pInfo->m_score, pInfo->m_pang, pInfo->m_extraPang);
		fwrite(buf, strlen(buf), 1, fp);
	}

	sprintf(buf, "------------------------------------------------------------\n");
	fwrite(buf, strlen(buf), 1, fp);

	fclose(fp);

	return true;
}

// resultdlg.cpp:4205 (run-time type and factory registration of FrAwardInfoDlg)
const WRTTI FrAwardInfoDlg::m_RTTI("FrAwardInfoDlg", &FrForm::m_RTTI);

IObject* FrAwardInfoDlgMakeInstance(void) {
	return new FrAwardInfoDlg;
}

struct __sFrAwardInfoDlg {
	__sFrAwardInfoDlg(void) {
		ObjectFactory().AddObjectFunctor(FrAwardInfoDlgMakeInstance, "FrAwardInfoDlg");
	}
};
static __sFrAwardInfoDlg s_registerFrAwardInfoDlg;

// resultdlg.cpp:4207
const sFRESH_MSGMAP* FrAwardInfoDlg::GetMessageMap(void) const {
	return &FrAwardInfoDlg::_MsgMap;
}

sFRESH_MSGMAP FrAwardInfoDlg::_MsgMap = { &FrForm::_MsgMap, &FrAwardInfoDlg::_MsgEntries[0] };

sFRESH_ENTRY FrAwardInfoDlg::_MsgEntries[] = {
	// resultdlg.cpp:4210
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// resultdlg.cpp:4215
FrAwardInfoDlg::FrAwardInfoDlg(void) {
	m_elapsed = 0;
}

// resultdlg.cpp:4222
FrAwardInfoDlg::~FrAwardInfoDlg(void) {
}

// resultdlg.cpp:4229
bool FrAwardInfoDlg::OnInit(void) {
	SetCaption("\xbc\xf6\xbb\xf3\xb3\xbb\xbf\xeb");
	return true;
}

// resultdlg.cpp:4237
void FrAwardInfoDlg::OnProc(const float dt) {
	m_elapsed += dt;
}
