// Original: d:\build\custom\temp\code\client\projectg\scoreboard.cpp
#define SCOREBOARD_EXHIBITION_INLINE
#define SCOREBOARD_SHAREDDOC_INLINE
#define SCOREBOARD_TREASURE_INLINE
#include <atlbase.h>	// stand-in for the precompiled header: _AtlWinModule initializer
#include <string.h>

#include "scoreboard.h"
#include "basicdebug.h"
#include "bitmap.h"
#include "QuitWindow.h"
#include "InputManager.h"
#include "LevelUpItemDlg.h"
#include "ChatMsg.h"
#include "ClientSetting.h"
#include "CapturedBg.h"
#include "commonutil.h"
#include "exhibition.h"
#include "fresh.h"
#include "frbutton.h"
#include "frdesktop.h"
#include "frarea.h"
#include "fredit.h"
#include "frframe.h"
#include "frgaugebar.h"
#include "frgraphicinterface.h"
#include "frlistbox.h"
#include "frscrollbar.h"
#include "frstatic.h"
#include "frwndmanager.h"
#include "GolfDoc.h"
#include "HeadIcon.h"
#include "Intrusion.h"
#include "NetResourceManager.h"
#include "NetworkSystem.h"
#include "ObjectFactory.h"
#include "Packet.h"
#include "partsinfo.h"
#include "ProjectG.h"
#include "ScoreDlg.h"
#include "SharedDoc.h"
#include "shared/Localize.h"
#include "SimpleOverlay.h"
#include "SoundManager.h"
#include "TaskManager.h"
#include "Treasure.h"
#include "TreasureDlg.h"
#include "UccManager.h"
#include "user_info.h"
#include "voiceitem.h"
#include "wfont.h"
#include "woverlay.h"
#include "wutil.h"
#include "wview.h"
#include "wbone.h"
#include "wpuppet.h"

// Directory of the score board's character images (cl 13.10 decorates it
// ?SCORE_PPL_DIR@@3PBDB, the original's).
const char* SCORE_PPL_DIR = "./score_ppl";

// Hole kind points by difficulty and hole count (CScore::HandleMsg 0x13a);
// no symbol, its contents are not transcribed yet.
extern const int data_9f1f48[][19][11];

// The title font's texture names (no symbol, contents not transcribed).
extern "C" char* data_a82930[];
extern const char data_9f19c4[][20];

// shareddoc.h:1143
int CSharedDoc::GetUserInfoMod(void) {
	return m_userInfoMod;
}

// scoreboard.h:670
void FrMatchItems::Reset(void) {
	m_bRedraw = m_bRefresh = true;
}

// scoreboard.cpp:67
bool ApproachRankCompare(const void* lhs, const void* rhs) {
	const sRivalData* pLhs = (const sRivalData*)lhs;
	const sRivalData* pRhs = (const sRivalData*)rhs;

	if (pLhs->m_sortGroup > pRhs->m_sortGroup) {
		return true;
	}
	if (pLhs->m_sortGroup < pRhs->m_sortGroup) {
		return false;
	}
	if (pLhs->m_approachDistanceCopy == pLhs->m_approachDistance && pRhs->m_approachDistanceCopy != pRhs->m_approachDistance) {
		return true;
	}
	if (pLhs->m_approachDistanceCopy != pLhs->m_approachDistance && pRhs->m_approachDistanceCopy == pRhs->m_approachDistance) {
		return false;
	}
	if (pLhs->m_approachDistanceCopy == pLhs->m_approachDistance && pRhs->m_approachDistanceCopy == pRhs->m_approachDistance) {
		if (pLhs->m_approachDistance < pRhs->m_approachDistance) {
			return true;
		}
		if (pLhs->m_approachDistance > pRhs->m_approachDistance) {
			return false;
		}
	}
	if (pLhs->m_approachPoint > pRhs->m_approachPoint) {
		return true;
	}
	if (pLhs->m_approachPoint < pRhs->m_approachPoint) {
		return false;
	}
	return pLhs->m_value13F < pRhs->m_value13F;
}

// scoreboard.cpp:102
bool RankCompare(const void* a, const void* b) {
	const sGameResultInfo* pA = (const sGameResultInfo*)a;
	const sGameResultInfo* pB = (const sGameResultInfo*)b;

	if (pA->m_sortGroup > pB->m_sortGroup) {
		return true;
	} else if (pA->m_sortGroup < pB->m_sortGroup) {
		return false;
	}

	if (pA->m_rank < pB->m_rank) {
		return true;
	} else if (pA->m_rank > pB->m_rank) {
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

// scoreboard.cpp:138 (run-time type and factory registration of FrRankForm)
const WRTTI FrRankForm::m_RTTI("FrRankForm", &FrForm::m_RTTI);

IObject* FrRankFormMakeInstance(void) {
	return new FrRankForm;
}

struct __impFrRankForm {
	__impFrRankForm(void) {
		ObjectFactory().AddObjectFunctor(FrRankFormMakeInstance, "FrRankForm");
		ActorFactory().AddActorClass("FrRankForm");
	}
};
static __impFrRankForm s_registerFrRankForm;

// scoreboard.cpp:140
const sFRESH_MSGMAP* FrRankForm::GetMessageMap(void) const {
	return &FrRankForm::_MsgMap;
}

sFRESH_MSGMAP FrRankForm::_MsgMap = { &FrForm::_MsgMap, &FrRankForm::_MsgEntries[0] };

sFRESH_ENTRY FrRankForm::_MsgEntries[] = {
	// scoreboard.cpp:142
	{ "list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankForm::OnRankInit },
	// scoreboard.cpp:143
	{ "list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankForm::OnRankOwnerDraw },
	// scoreboard.cpp:144
	{ "list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrRankForm::OnRankLBtnDown },
	// scoreboard.cpp:145
	{ "list", FRCMD_RBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRankForm::OnRankRBtnUp },
	// scoreboard.cpp:146
	{ "list", FRCMD_DBLCLICK, FrFuncVV, (FRESH_PMSG)&FrRankForm::OnRankDClick },
	// scoreboard.cpp:147
	{ "center", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRankForm::OnCenterBtnUp },
	// scoreboard.cpp:148
	{ "info", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRankForm::OnInfoBtnUp },
	// scoreboard.cpp:149
	{ "size", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRankForm::OnSizeBtnUp },
	// scoreboard.cpp:150
	{ "billboard", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankForm::OnBoardInit },
	// scoreboard.cpp:151
	{ "bar01", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankForm::OnBar01Init },
	// scoreboard.cpp:152
	{ "bar02", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankForm::OnBar02Init },

	// scoreboard.cpp:154
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// scoreboard.cpp:157
FrRankForm::FrRankForm(void) {
	m_pRankList = 0;
	m_pBoard = 0;
	m_rankIndex = -1;
	m_rankPage = -1;
	m_selectedUID = -1;
	m_announcedHole = 0;
	m_scrollCount = 0;

	memset(m_rankText, 0, sizeof(m_rankText));
}

// scoreboard.cpp:171
void FrRankForm::OnProc(const float dt) {
	// scoreboard.cpp:172
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 10) {
		if (g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true)) {
			if (g_input->GetDown("PAGE_DOWN", true)) {
				m_pRankList->GetScrollBar()->ScrollDown(1);
			} else if (g_input->GetDown("PAGE_UP", true)) {
				m_pRankList->GetScrollBar()->ScrollUp(1);
			}
		}
	}

	// scoreboard.cpp:184
	switch (m_captionType) {
	case 0: {
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 5) {
			// scoreboard.cpp:190
			m_pBoard->SetCaption(MakeStr("RED %d\xC1\xA1/%I64d\xC6\xCE  vs  BLUE %d\xC1\xA1/%I64d\xC6\xCE",
				WSingleton<CSharedDoc>::Instance()->m_teamResult[0].m_score, WSingleton<CSharedDoc>::Instance()->m_teamResult[0].m_pang, WSingleton<CSharedDoc>::Instance()->m_teamResult[1].m_score, WSingleton<CSharedDoc>::Instance()->m_teamResult[1].m_pang));
		} else if (m_rankIndex != -1) {
			// scoreboard.cpp:196
			if (WSingleton<CSharedDoc>::Instance()->m_roomType == 10) {
				m_pBoard->SetCaption(MakeStr("\xC7\xF6\xC0\xE7 1\xC0\xA7 %s\xB4\xD4, \xB3\xB2\xC0\xBA\xB0\xC5\xB8\xAE : %d.%d y",
					WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(m_rankIndex)].m_nickname,
					WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(m_rankIndex)].m_approachDistance / 10,
					WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(m_rankIndex)].m_approachDistance % 10));
			} else {
				// scoreboard.cpp:202
				m_pBoard->SetCaption(MakeStr("\xC7\xF6\xC0\xE7 1\xC0\xA7 %s\xB4\xD4, \xBD\xBA\xC4\xDA\xBE\xEE %d",
					WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(m_rankIndex)].m_nickname,
					WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(m_rankIndex)].m_score));
			}
		} else {
			// scoreboard.cpp:206
			m_pBoard->SetCaption(MakeStr("\xC7\xF6\xC0\xE7 \xB0\xD4\xC0\xD3\xC1\xDF\xC0\xCE \xC0\xCE\xBF\xF8\xC0\xBA %d\xB8\xED\xC0\xD4\xB4\xCF\xB4\xD9.", m_rankCount));
		}

		// scoreboard.cpp:209
		m_boardMessage = 4;
		break;
	}

	case 2: {
		// scoreboard.cpp:216
		WRect rect;
		m_pBoard->GetClientRect(rect);
		rect.x -= dt * 30.0f;
		m_pBoard->SetClientRect(rect);

		// scoreboard.cpp:221
		if (rect.x + rect.w < g_view->GetWidth() - 239.0f) {
			m_scrollCount++;
			if (m_scrollCount == 2) {
				m_scrollCount = 0;
				SetCaptionType(0);
			} else {
				// scoreboard.cpp:230
				SetCaptionType(2);
			}
		}
		break;
	}

	case 1:
		// scoreboard.cpp:237
		m_captionTime += dt;
		if (m_captionTime > 10.0f) {
			m_captionTime = 0;
			SetCaptionType(0);
		}
		break;
	}
}

// scoreboard.cpp:249
void FrRankForm::OnRankInit(intptr_t param) {
	m_pRankList = (FrListBox*)((__rtti_obj = (void*)param) != 0 ? (((IObject*)__rtti_obj)->IsKindOf(&FrListBox::m_RTTI) ? (void*)param : 0) : 0);

	for (std::vector<sGameResultInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
		m_pRankList->AddItem(&*it);
	}

	m_pRankList->UseRightButton(true);

	m_rankCount = WSingleton<CSharedDoc>::Instance()->m_gameResultList.size();
	m_rankText[0] = WSingleton<CSharedDoc>::Instance()->m_gameResultList.size();

	SortRank();

	m_sizeType = WSingleton<CSharedDoc>::Instance()->m_roomType == 10 ? 2 : 1;

	m_pRankGold = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_gold");
	m_pRankSilver = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_silver");
	m_pRankBronze = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_bronze");
	m_pRankNoAward = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_noaward");

	m_pRedTeamRank = g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_team_redteam_rank");
	m_pBlueTeamRank = g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_team_blueteam_rank");
	m_pRankBg = g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_rank");

	m_pMassIcon = g_pFresh->GetManager()->GetBitmap("golf", "pc_icon_mass");
}

// scoreboard.cpp:291
int FrRankForm::GetMedalIndex(unsigned long uid) {
	unsigned char memberSize;
	if (IsLocalContent((localContentType_t)0x2E)) {
		memberSize = WSingleton<CIntrusion>::Instance()->GetCalcExpMemberSize();
	} else {
		memberSize = 0;
	}

	if (memberSize == 0) {
		if (WSingleton<CSharedDoc>::Instance()->m_gameResultList.size() < 10)
			return 3;
	} else {
		if (memberSize < 10)
			return 3;
	}

	unsigned char index = WSingleton<CSharedDoc>::Instance()->GetIndex(uid);

	if (index == 0xFF || WSingleton<CSharedDoc>::Instance()->m_gameResultList[index].m_characterType > 3)
		return 3;

	unsigned char rank = WSingleton<CSharedDoc>::Instance()->m_gameResultList[index].m_rank;
	unsigned char count = 0, better = 0;

	if (memberSize == 0) {
		for (std::vector<sGameResultInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
			unsigned char state = it->m_state;
			if (state != 3 || GetHoleIndex(it->m_hole) >= 4)
				count++;

			if (state != 3 && it->m_rank < rank && it->m_characterType > 3)
				better++;
		}
	} else {
		count = memberSize;
		for (std::vector<sGameResultInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
			if (it->m_state != 3 && it->m_rank < rank && it->m_characterType > 3)
				better++;
		}
	}

	rank -= better;
	if (WSingleton<CSharedDoc>::Instance()->m_holes == 9) {
		if (count <= 14) {
			return 3;
		} else if (count <= 18) {
			if (rank == 1)
				return 2;
		} else if (count <= 22) {
			if (rank == 1)
				return 1;
		} else if (count <= 26) {
			if (rank == 1)
				return 1;
			else if (rank == 2)
				return 2;
		} else {
			if (rank == 1)
				return 0;
			else if (rank == 2)
				return 1;
		}
	} else if (WSingleton<CSharedDoc>::Instance()->m_holes == 18) {
		if (count <= 14) {
			if (rank == 1)
				return 2;
		} else if (count <= 18) {
			if (rank == 1)
				return 1;
			else if (rank == 2)
				return 2;
		} else if (count <= 22) {
			if (rank == 1)
				return 0;
			else if (rank == 2)
				return 1;
			else if (rank == 3)
				return 2;
		} else if (count <= 26) {
			if (rank == 1)
				return 0;
			else if (rank == 2)
				return 1;
			else if (rank <= 4)
				return 2;
		} else {
			if (rank == 1)
				return 0;
			else if (rank <= 3)
				return 1;
			else if (rank <= 6)
				return 2;
		}
	}

	return 3;
}

// scoreboard.cpp:444
void FrRankForm::OnRankOwnerDraw(intptr_t param) {
	if (!m_sizeType)
		return;

	FrListItem* pItem = (FrListItem*)param;
	if (pItem == 0)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	_WRECT rect = { pItem->pos.x + 7.0f, pItem->pos.y, (float)m_pRankList->GetItemWidth() - 10.0f, (float)m_pRankList->GetItemHeight() };

	int space = pGDI->SetSpace(0);

	sGameResultInfo* pInfo = (sGameResultInfo*)pItem->pData;
	if (m_selectedUID == pInfo->m_uid)
		pGDI->Box(rect, 0x999B9DFF, 0, 0.0f);

	pGDI->SetTextStyle(0);
	pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 5) {
		if ((&m_pRedTeamRank)[pInfo->m_team])
			pGDI->DrawTexture((&m_pRedTeamRank)[pInfo->m_team], WRect(pItem->pos.x + 12.0f, pItem->pos.y + 2.0f, (float)(&m_pRedTeamRank)[pInfo->m_team]->Width(), (float)(&m_pRedTeamRank)[pInfo->m_team]->Height()), 0xFFFFFFFF, 0);

		pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1, "%d", pInfo->m_rank);
	} else {
		if (m_pRankBg)
			pGDI->DrawTexture(m_pRankBg, WRect(pItem->pos.x + 12.0f, pItem->pos.y + 2.0f, (float)m_pRankBg->Width(), (float)m_pRankBg->Height()), 0xFFFFFFFF, 0);

		if (pInfo->m_state == 3)
			pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1, "-");
		else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 10 && pInfo->m_approachDistanceCopy == 0xFFFFFFFF)
			pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1, "-");
		else
			pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1, "%d", pInfo->m_rank);
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType != 10 && WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_userLimit < 100) {
		if (pInfo->m_hole != WSingleton<CSharedDoc>::Instance()->m_holeOrder[0]) {
			int medal = GetMedalIndex(pInfo->m_uid);

			if (medal >= 0 && medal <= 2) {
				if (pInfo->m_characterType > 3)
					medal = 3;

				pGDI->DrawTexture((&m_pRankGold)[medal], WRect(pItem->pos.x + 2.0f, pItem->pos.y + 1.0f, (float)(&m_pRankGold)[medal]->Width(), (float)(&m_pRankGold)[medal]->Height()), 0xFFFFFFFF, 0);
			}
		}
	}

	if (pInfo->m_uid == MyGuid(true)) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 5)
			pGDI->SetTextColor(pInfo->m_team ? 0xFF0060FF : 0xFFFF0000, 0xFFFFFFFF);
		else
			pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	} else if (pInfo->m_state == 3) {
		pGDI->SetTextColor(0xFF808080, 0xFFFFFFFF);
	} else {
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	}

	if (pInfo->m_hasEmblem) {
		const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pInfo->m_emblemName);

		if (pEmblem)
			pGDI->DrawTexture(pEmblem, WRect(pItem->pos.x + 33.0f, pItem->pos.y + 3.0f - 6.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
	}

	if (!WSingleton<CProjectG>::Instance()->HidePrivacy())
		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 60.0f, pItem->pos.y + 3.0f), 0, pInfo->m_nickname, 90.0f, 0xFFFFFFFF);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 10) {
		// the approach ranking: distance and points
		if (pInfo->m_approachPoint == -1) {
			if (pInfo->m_approachDistanceCopy == pInfo->m_approachDistance)
				pGDI->Print(WPoint(pItem->pos.x + 210.0f, pItem->pos.y + 3.0f), 2, "Out");
			else
				pGDI->Print(WPoint(pItem->pos.x + 202.0f, pItem->pos.y + 3.0f), 2, "-");

			pGDI->Print(WPoint(pItem->pos.x + 254.0f, pItem->pos.y + 3.0f), 2, "-");
		} else {
			if (pInfo->m_approachDistanceCopy == pInfo->m_approachDistance) {
				if (pInfo->m_approachDistance == 0xFFFFFFFF)
					pGDI->Print(WPoint(pItem->pos.x + 210.0f, pItem->pos.y + 3.0f), 2, "Out");
				else
					pGDI->Print(WPoint(pItem->pos.x + 217.0f, pItem->pos.y + 3.0f), 2, "%d.%d y", pInfo->m_approachDistance / 10, pInfo->m_approachDistance % 10);
			} else if (pInfo->m_uid == MyGuid(false) && (GOLFDOC()->GameMode() & 0x40)) {
				pGDI->Print(WPoint(pItem->pos.x + 202.0f, pItem->pos.y + 3.0f), 2, "-");
			} else {
				pGDI->Print(WPoint(pItem->pos.x + 217.0f, pItem->pos.y + 3.0f), 2, "%d.%d y", pInfo->m_approachDistanceCopy / 10, pInfo->m_approachDistanceCopy % 10);
			}

			pGDI->Print(WPoint(pItem->pos.x + 273.0f, pItem->pos.y + 3.0f), 2, "%d.%03d", pInfo->m_approachPoint / 1000, pInfo->m_approachPoint % 1000);
		}
	} else {
		pGDI->Print(WPoint(pItem->pos.x + 179.0f, pItem->pos.y + 3.0f), 2, "%d", pInfo->m_score);
		pGDI->Print(WPoint(pItem->pos.x + 222.0f, pItem->pos.y + 3.0f), 2, "%I64d", pInfo->m_pang);

		if (pInfo->m_state == 2 || GetHoleIndex(pInfo->m_hole) == 19)
			pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 3.0f), 2, "End");
		else if (pInfo->m_state == 3)
			pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 3.0f), 2, "Out");
		else
			pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 3.0f), 2, "%d\xC8\xA6", GetHoleIndex(pInfo->m_hole));

		if ((pInfo->m_identity & 0x80) && m_pMassIcon)
			pGDI->DrawTexture(m_pMassIcon, WRect(pItem->pos.x + 259.0f, pItem->pos.y + 3.0f, (float)m_pMassIcon->Width(), (float)m_pMassIcon->Height()), 0xFFFFFFFF, 0);
	}

	pGDI->SetSpace(space);
}

// scoreboard.cpp:619
void FrRankForm::OnRankLBtnDown(void) {
	FrListItem* pItem = m_pRankList->GetItemUnderCursor();
	if (pItem) {
		m_selectedUID = ((sGameResultInfo*)pItem->pData)->m_uid;
	}
}

// scoreboard.cpp:630
void FrRankForm::OnRankRBtnUp(void) {
	FrListItem* pItem = m_pRankList->GetItemUnderCursor();
	if (!pItem)
		return;

	sGameResultInfo* pInfo = (sGameResultInfo*)pItem->pData;
	if (!pInfo)
		return;

	m_selectedUID = pInfo->m_uid;

	if (WSingleton<CUserInfo>::Instance() && pInfo->m_state != 3)
		WSingleton<CUserInfo>::Instance()->SetInfo(pInfo->m_id, m_selectedUID, false, true, false, false, pInfo->m_nickname);
}

// scoreboard.cpp:648
void FrRankForm::OnRankDClick(void) {
	OnRankRBtnUp();
}

// scoreboard.cpp:653
void FrRankForm::OnBoardInit(intptr_t param) {
	m_pBoard = DYNAMIC_CAST(FrStatic, param);
	m_pBoard->m_nFlags.Enable(0x1000);

	m_pBoard->SetCaption("");
	SetCaptionType(0);
	m_boardMessage = 4;

	m_captionTime = 0;
}

// scoreboard.cpp:665
void FrRankForm::OnBar01Init(intptr_t param) {
	FrArea* pArea = DYNAMIC_CAST(FrArea, param);
	if (pArea) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 10) {
			pArea->SetBgImg("approach_base_01");
		}
	}
}

// scoreboard.cpp:677
void FrRankForm::OnBar02Init(intptr_t param) {
	FrArea* pArea = DYNAMIC_CAST(FrArea, param);
	if (pArea) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 10) {
			pArea->SetBgImg("approach_base_02");
		}
	}
}

// scoreboard.cpp:689
void FrRankForm::OnCenterBtnUp(void) {
	Align();
}

// scoreboard.cpp:694
void FrRankForm::Align(void) {
	if (m_sizeType) {
		int index = 0;

		for (FrListBox::ITEM_LIST::iterator it = m_pRankList->m_itemList.begin(); it != m_pRankList->m_itemList.end(); ++it) {
			sGameResultInfo* pInfo = (sGameResultInfo*)(*it)->pData;

			if (pInfo->m_uid == MyGuid(true))
				break;

			index++;
		}

		if (FrScrollBar* pScrollBar = m_pRankList->GetScrollBar()) {
			if (m_sizeType == 1) {
				int last = (int)WSingleton<CSharedDoc>::Instance()->m_gameResultList.size() - 5;
				int top = index - 2;
				pScrollBar->SetCurTopRow(top < 0 ? 0 : (top > last ? last : top));
			} else {
				int last = (int)WSingleton<CSharedDoc>::Instance()->m_gameResultList.size() - 10;
				int top = index - 4;
				pScrollBar->SetCurTopRow(top < 0 ? 0 : (top > last ? last : top));
			}
		}
	}
}

// scoreboard.cpp:722
void FrRankForm::OnInfoBtnUp(void) {
	std::map<unsigned long, sBriefUserInfo>::iterator it = CSharedDoc::Instance()->m_userMap.find(m_selectedUID);
	if (it != CSharedDoc::Instance()->m_userMap.end()) {
		CUserInfo::Instance()->SetInfo(it->second.id, m_selectedUID, true, true, false, false, std::string(it->second.nick));
	}
}

// scoreboard.cpp:731
void FrRankForm::OnSizeBtnUp(void) {
	m_sizeType = (m_sizeType + 1) % 3;
	Resize();
}

// scoreboard.cpp:738
void FrRankForm::Resize(void) {
	if (m_pRankList) {
		WRect rect;
		m_pRankList->GetClientRect(rect);

		switch (m_sizeType) {
		case 0:
			rect.h = 0;
			break;
		case 1:
			rect.h = 90.0f;
			break;
		case 2:
			rect.h = 180.0f;
			break;
		}
		m_pRankList->SetClientRect(rect);

		rect.h += 70.0f;
		rect.y = 0;
		rect.w = m_pBaseFrm->GetRect().w;
		m_pBaseFrm->SetClientRect(rect);
	}
}

// scoreboard.cpp:773
void FrRankForm::SortRank(void) {
	int leaderCount;

	m_rankIndex = m_rankPage = -1;
	leaderCount = 0;
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 10) {
		for (std::vector<sGameResultInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
			if (it->m_state == 3) {
				it->m_rank = (unsigned char)WSingleton<CSharedDoc>::Instance()->m_gameResultList.size() - it->m_sortGroup;
				continue;
			}

			it->m_rank = 1;
			for (std::vector<sGameResultInfo>::iterator other = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); other != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++other) {
				if (it != other && other->m_state != 3) {
					if (other->m_approachDistanceCopy != other->m_approachDistance) {
						if (it->m_approachDistanceCopy == it->m_approachDistance)
							continue;
					} else if (it->m_approachDistanceCopy != it->m_approachDistance || it->m_approachDistance > other->m_approachDistance) {
						it->m_rank++;
						continue;
					} else if (it->m_approachDistance != other->m_approachDistance) {
						continue;
					}

					if (it->m_approachPoint < other->m_approachPoint)
						it->m_rank++;
				}
			}

			if (it->m_rank == 1) {
				if (it->m_approachDistanceCopy == it->m_approachDistance && it->m_approachDistance != 0xFFFFFFFF) {
					m_rankIndex = it->m_uid;
					leaderCount++;
				}
			} else if (it->m_rank == m_rankCount) {
				m_rankPage = it->m_uid;
			}
		}

		m_pRankList->SortItem(ApproachRankCompare);
	} else {
		memset(WSingleton<CSharedDoc>::Instance()->m_teamResult, 0, sizeof(WSingleton<CSharedDoc>::Instance()->m_teamResult));

		for (std::vector<sGameResultInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
			if (it->m_state == 3) {
				it->m_rank = (unsigned char)WSingleton<CSharedDoc>::Instance()->m_gameResultList.size() - it->m_sortGroup;
			} else {
				it->m_rank = 1;
				for (std::vector<sGameResultInfo>::iterator other = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); other != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++other) {
					if (it != other && other->m_state != 3) {
						if (it->m_score > other->m_score || (it->m_score == other->m_score && it->m_pang < other->m_pang))
							it->m_rank++;
					}
				}

				if (it->m_rank == 1) {
					m_rankIndex = it->m_uid;
					leaderCount++;
				} else if (it->m_rank == m_rankCount) {
					m_rankPage = it->m_uid;
				}
			}

			WSingleton<CSharedDoc>::Instance()->m_teamResult[it->m_team].m_score += it->m_rank;
			WSingleton<CSharedDoc>::Instance()->m_teamResult[it->m_team].m_pang += it->m_pang;
		}

		m_pRankList->SortItem(RankCompare);
	}

	if (leaderCount != 1)
		m_rankIndex = -1;
}

// scoreboard.cpp:888
void FrRankForm::SetCaptionType(unsigned char type) {
	m_captionType = type;

	if (m_pBoard == 0) {
		return;
	}

	WRect rect;
	m_pBoard->GetClientRect(rect);

	if (m_captionType == 2) {
		WRect clip(rect.x, rect.y, 219.0f, 16.0f);
		m_pBoard->SetClippingArea(&clip);

		rect.x = 800.0f;
		rect.w = m_pBoard->GDI()->GetTextExtend(m_pBoard->GetCaption());
		m_pBoard->SetClientRect(rect);
	} else {
		m_pBoard->SetClippingArea(0);
		m_pBoard->SetClientRect(WRect(rect.x, rect.y, 219.0f, 16.0f));
	}
}

// scoreboard.cpp:915 (run-time type and factory registration of FrRankForm2)
const WRTTI FrRankForm2::m_RTTI("FrRankForm2", &FrForm::m_RTTI);

IObject* FrRankForm2MakeInstance(void) {
	return new FrRankForm2;
}

struct __impFrRankForm2 {
	__impFrRankForm2(void) {
		ObjectFactory().AddObjectFunctor(FrRankForm2MakeInstance, "FrRankForm2");
		ActorFactory().AddActorClass("FrRankForm2");
	}
};
static __impFrRankForm2 s_registerFrRankForm2;

// scoreboard.cpp:917
const sFRESH_MSGMAP* FrRankForm2::GetMessageMap(void) const {
	return &FrRankForm2::_MsgMap;
}

sFRESH_MSGMAP FrRankForm2::_MsgMap = { &FrForm::_MsgMap, &FrRankForm2::_MsgEntries[0] };

sFRESH_ENTRY FrRankForm2::_MsgEntries[] = {
	// scoreboard.cpp:919
	{ "list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankForm2::OnRankInit },
	// scoreboard.cpp:920
	{ "list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankForm2::OnRankOwnerDraw },

	// scoreboard.cpp:922
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// scoreboard.cpp:925
FrRankForm2::FrRankForm2(void) : m_pRankList(0) {
}

// scoreboard.cpp:929
void FrRankForm2::OnRankInit(intptr_t param) {
	m_pRankList = (FrListBox*)((__rtti_obj = (void*)param) != 0 ? (((IObject*)__rtti_obj)->IsKindOf(&FrListBox::m_RTTI) ? (void*)param : 0) : 0);

	for (std::vector<sGameResultInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
		m_pRankList->AddItem(&*it);
	}

	SortRank();

	m_pMedal[0] = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_gold");
	m_pMedal[1] = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_silver");
	m_pMedal[2] = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_bronze");
	m_pMedal[3] = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_noaward");

	m_pTeamRank[0] = g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_team_redteam_rank");
	m_pTeamRank[1] = g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_team_blueteam_rank");
	m_pRankBg = g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_rank");

	m_pMassIcon = g_pFresh->GetManager()->GetBitmap("golf", "pc_icon_mass");
}

// scoreboard.cpp:956
void FrRankForm2::SortRank(void) {
	memset(WSingleton<CSharedDoc>::Instance()->m_teamResult, 0, sizeof(WSingleton<CSharedDoc>::Instance()->m_teamResult));

	for (std::vector<sGameResultInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
		if (it->m_state == 3) {
			it->m_rank = (unsigned char)WSingleton<CSharedDoc>::Instance()->m_gameResultList.size() - it->m_sortGroup;
			continue;
		}

		it->m_rank = 1;
		for (std::vector<sGameResultInfo>::iterator other = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); other != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++other) {
			if (it != other && other->m_state != 3) {
				if (it->m_score > other->m_score || (it->m_score == other->m_score && it->m_pang < other->m_pang))
					it->m_rank++;
			}
		}

		WSingleton<CSharedDoc>::Instance()->m_teamResult[it->m_team].m_score += it->m_rank;
		WSingleton<CSharedDoc>::Instance()->m_teamResult[it->m_team].m_pang += it->m_pang;
	}

	m_pRankList->SortItem(RankCompare);
}

// scoreboard.cpp:989
int FrRankForm2::GetMedalIndex(unsigned long uid) {
	if (WSingleton<CSharedDoc>::Instance()->m_gameResultList.size() < 10)
		return 3;

	unsigned char index = WSingleton<CSharedDoc>::Instance()->GetIndex(uid);

	if (index == 0xFF || WSingleton<CSharedDoc>::Instance()->m_gameResultList[index].m_characterType > 3)
		return 3;

	unsigned char rank = WSingleton<CSharedDoc>::Instance()->m_gameResultList[index].m_rank;
	unsigned char count = 0, better = 0;

	for (std::vector<sGameResultInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
		unsigned char state = it->m_state;
		if (state != 3 || GetHoleIndex(it->m_hole) >= 4)
			count++;

		if (state != 3 && it->m_rank < rank && it->m_characterType > 3)
			better++;
	}

	rank -= better;
	if (WSingleton<CSharedDoc>::Instance()->m_holes == 9) {
		if (count <= 14) {
			return 3;
		} else if (count <= 18) {
			if (rank == 1)
				return 2;
		} else if (count <= 22) {
			if (rank == 1)
				return 1;
		} else if (count <= 26) {
			if (rank == 1)
				return 1;
			else if (rank == 2)
				return 2;
		} else {
			if (rank == 1)
				return 0;
			else if (rank == 2)
				return 1;
		}
	} else if (WSingleton<CSharedDoc>::Instance()->m_holes == 18) {
		if (count <= 14) {
			if (rank == 1)
				return 2;
		} else if (count <= 18) {
			if (rank == 1)
				return 1;
			else if (rank == 2)
				return 2;
		} else if (count <= 22) {
			if (rank == 1)
				return 0;
			else if (rank == 2)
				return 1;
			else if (rank == 3)
				return 2;
		} else if (count <= 26) {
			if (rank == 1)
				return 0;
			else if (rank == 2)
				return 1;
			else if (rank <= 4)
				return 2;
		} else {
			if (rank == 1)
				return 0;
			else if (rank <= 3)
				return 1;
			else if (rank <= 6)
				return 2;
		}
	}

	return 3;
}

// scoreboard.cpp:1103
void FrRankForm2::OnRankOwnerDraw(intptr_t param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == 0)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	int space = pGDI->SetSpace(0);

	sGameResultInfo* pInfo = (sGameResultInfo*)pItem->pData;

	pGDI->SetTextStyle(0);
	pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 5) {
		if (m_pTeamRank[pInfo->m_team])
			pGDI->DrawTexture(m_pTeamRank[pInfo->m_team], WRect(pItem->pos.x + 12.0f, pItem->pos.y + 2.0f, (float)m_pTeamRank[pInfo->m_team]->Width(), (float)m_pTeamRank[pInfo->m_team]->Height()), 0xFFFFFFFF, 0);

		pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1, "%d", pInfo->m_rank);
	} else {
		if (m_pRankBg)
			pGDI->DrawTexture(m_pRankBg, WRect(pItem->pos.x + 12.0f, pItem->pos.y + 2.0f, (float)m_pRankBg->Width(), (float)m_pRankBg->Height()), 0xFFFFFFFF, 0);

		if (pInfo->m_state == 3)
			pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1, "-");
		else
			pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1, "%d", pInfo->m_rank);
	}

	if (pInfo->m_hole != WSingleton<CSharedDoc>::Instance()->m_holeOrder[0]) {
		int medal = GetMedalIndex(pInfo->m_uid);

		if (medal >= 0 && medal <= 2) {
			if (pInfo->m_characterType > 3)
				medal = 3;

			pGDI->DrawTexture(m_pMedal[medal], WRect(pItem->pos.x + 2.0f, pItem->pos.y + 1.0f, (float)m_pMedal[medal]->Width(), (float)m_pMedal[medal]->Height()), 0xFFFFFFFF, 0);
		}
	}

	if (pInfo->m_uid == MyGuid(true)) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 5)
			pGDI->SetTextColor(pInfo->m_team ? 0xFF0060FF : 0xFFFF0000, 0xFFFFFFFF);
		else
			pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	} else if (pInfo->m_state == 3) {
		pGDI->SetTextColor(0xFF808080, 0xFFFFFFFF);
	} else {
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	}

	if (pInfo->m_hasEmblem) {
		const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pInfo->m_emblemName);

		if (pEmblem)
			pGDI->DrawTexture(pEmblem, WRect(pItem->pos.x + 33.0f, pItem->pos.y - 3.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
	}

	if (!WSingleton<CProjectG>::Instance()->HidePrivacy())
		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 60.0f, pItem->pos.y + 3.0f), 0, pInfo->m_nickname, 90.0f, 0xFFFFFFFF);

	pGDI->Print(WPoint(pItem->pos.x + 179.0f, pItem->pos.y + 3.0f), 2, "%d", pInfo->m_score);
	pGDI->Print(WPoint(pItem->pos.x + 222.0f, pItem->pos.y + 3.0f), 2, "%I64d", pInfo->m_pang);

	if (pInfo->m_state == 2 || GetHoleIndex(pInfo->m_hole) == 19)
		pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 3.0f), 2, "End");
	else
		pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 3.0f), 2, "%d\xC8\xA6", GetHoleIndex(pInfo->m_hole));

	if ((pInfo->m_identity & 0x80) && m_pMassIcon)
		pGDI->DrawTexture(m_pMassIcon, WRect(pItem->pos.x + 259.0f, pItem->pos.y + 3.0f, (float)m_pMassIcon->Width(), (float)m_pMassIcon->Height()), 0xFFFFFFFF, 0);

	pGDI->SetSpace(space);
}

// scoreboard.cpp:1210 (run-time type and factory registration of FrRankForm3)
const WRTTI FrRankForm3::m_RTTI("FrRankForm3", &FrForm::m_RTTI);

IObject* FrRankForm3MakeInstance(void) {
	return new FrRankForm3;
}

struct __impFrRankForm3 {
	__impFrRankForm3(void) {
		ObjectFactory().AddObjectFunctor(FrRankForm3MakeInstance, "FrRankForm3");
		ActorFactory().AddActorClass("FrRankForm3");
	}
};
static __impFrRankForm3 s_registerFrRankForm3;

// scoreboard.cpp:1212
const sFRESH_MSGMAP* FrRankForm3::GetMessageMap(void) const {
	return &FrRankForm3::_MsgMap;
}

sFRESH_MSGMAP FrRankForm3::_MsgMap = { &FrForm::_MsgMap, &FrRankForm3::_MsgEntries[0] };

sFRESH_ENTRY FrRankForm3::_MsgEntries[] = {
	// scoreboard.cpp:1214
	{ "list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankForm3::OnRankInit },
	// scoreboard.cpp:1215
	{ "list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankForm3::OnRankOwnerDraw },
	// scoreboard.cpp:1216
	{ "size", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRankForm3::OnSizeBtnUp },
	// scoreboard.cpp:1217
	{ "billboard", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankForm3::OnBoardInit },
	// scoreboard.cpp:1218
	{ "score_gb", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankForm3::OnScoreGaugeInit },
	// scoreboard.cpp:1219
	{ "score_info", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankForm3::OnScoreInfoInit },
	// scoreboard.cpp:1220
	{ "score_info", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrRankForm3::OnScoreInfoOwnerDraw },

	// scoreboard.cpp:1222
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// scoreboard.cpp:1225
FrRankForm3::FrRankForm3(void) {
	m_pRankList = 0;
	m_pBoard = 0;
	m_pScoreGauge = 0;
	m_pScoreInfo = 0;
	m_bFlag129 = false;
	m_captionCycle = 0;
	m_index124 = -1;
	m_sizeType = 1;
	LoadBitmaps();
}

// scoreboard.cpp:1241
void FrRankForm3::OnProc(const float dt) {
	switch (m_captionType) {
	case 0:
		// "There are %d players in the game now."
		m_pBoard->SetCaption(MakeStr("\xC7\xF6\xC0\xE7 \xB0\xD4\xC0\xD3\xC1\xDF\xC0\xCE \xC0\xCE\xBF\xF8\xC0\xBA %d\xB8\xED\xC0\xD4\xB4\xCF\xB4\xD9.", m_playerCount));
		break;
	case 2: {
		// the caption scrolls left until it leaves the board
		WRect rect;
		m_pBoard->GetClientRect(rect);
		rect.x -= dt * 30.0f;
		m_pBoard->SetClientRect(rect);

		if (rect.x + rect.w < g_view->GetWidth() - 239.0f) {
			m_captionCycle++;
			if (m_captionCycle == 2) {
				m_captionCycle = 0;
				SetCaptionType(0);
			} else {
				SetCaptionType(2);
			}
		}
		break;
	}
	case 1:
		m_captionTime += dt;
		if (m_captionTime > 10.0f) {
			m_captionTime = 0;
			SetCaptionType(0);
		}
		break;
	}
}

// scoreboard.cpp:1296
void FrRankForm3::OnRankInit(intptr_t param) {
	m_pRankList = (FrListBox*)((__rtti_obj = (void*)param) != 0 ? (((IObject*)__rtti_obj)->IsKindOf(&FrListBox::m_RTTI) ? (void*)param : 0) : 0);

	for (std::vector<sGuildMatchup>::iterator it = WSingleton<CSharedDoc>::Instance()->m_vector4524.begin(); it != WSingleton<CSharedDoc>::Instance()->m_vector4524.end(); ++it) {
		m_pRankList->AddItem(&*it);
	}
	m_pRankList->UseRightButton(false);

	m_playerCount = (unsigned char)(WSingleton<CSharedDoc>::Instance()->m_vector4524.size() * 2);

	m_pRedEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_guild.m_guildMark[0]);
	m_pBlueEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_guild.m_guildMark[1]);

	m_pRankBg = g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_rank_02");

	m_pMassIcon = g_pFresh->GetManager()->GetBitmap("golf", "pc_icon_mass");

	m_sizeType = (m_sizeType + 1) % 2;
	OnSizeBtnUp();
}

// scoreboard.cpp:1343
void FrRankForm3::OnRankOwnerDraw(intptr_t param) {
	if (!m_sizeType)
		return;

	FrListItem* pItem = (FrListItem*)param;
	if (pItem == 0)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	WRect rect(pItem->pos.x + 2.0f, pItem->pos.y, (float)m_pRankList->GetItemWidth(), (float)m_pRankList->GetItemHeight());

	int space = pGDI->SetSpace(0);

	sGuildMatchup* pMatch = (sGuildMatchup*)pItem->pData;

	pGDI->SetTextStyle(0);
	pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);

	sGameResultInfo* pRed = &WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(pMatch->m_guidA)];
	sGameResultInfo* pBlue = &WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(pMatch->m_guidB)];

	// the line of the player's own match is selected and highlighted
	if (WSingleton<CSharedDoc>::Instance()->m_userInfo.guid == pRed->m_uid || WSingleton<CSharedDoc>::Instance()->m_userInfo.guid == pBlue->m_uid) {
		m_pRankList->SelectItem(pItem, true);
		WOverlay::DrawBox(g_view, WRect(rect), 0, 0x999B9DFF, 0.0f);
	}

	if (m_pRankBg) {
		pGDI->DrawTexture(m_pRankBg, WRect(pItem->pos.x + 5.0f, pItem->pos.y + 8.0f, (float)m_pRankBg->Width(), (float)m_pRankBg->Height()), 0xFFFFFFFF, 0);
		// "%d" + "group"
		pGDI->Print(WPoint(pItem->pos.x + 18.0f, pItem->pos.y + 12.0f), 1, "%d \xC1\xB6", pMatch->m_kind);
	}

	// the red guild's player
	if (m_pRedEmblem)
		pGDI->DrawTexture(m_pRedEmblem, WRect(pItem->pos.x + 39.0f, pItem->pos.y + 3.0f, (float)(m_pRedEmblem->Width() / 2), (float)(m_pRedEmblem->Height() / 2)), 0xFFFFFFFF, 0);

	if (pRed->m_uid == MyGuid(true))
		pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	else if (pRed->m_state == 3)
		pGDI->SetTextColor(0xFF808080, 0xFFFFFFFF);
	else
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	if (!WSingleton<CProjectG>::Instance()->HidePrivacy()) {
		pGDI->SetTextColor(0xFFCD1836, 0xFFFFFFFF);
		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 60.0f, pItem->pos.y + 3.0f), 0, pRed->m_nickname, 90.0f, 0xFFFFFFFF);
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	}

	pGDI->Print(WPoint(pItem->pos.x + 179.0f, pItem->pos.y + 3.0f), 2, "%d", pRed->m_uniteGuildIndex);   // the guild match points (0x170)
	pGDI->Print(WPoint(pItem->pos.x + 222.0f, pItem->pos.y + 3.0f), 2, "%I64d", pRed->m_pang);

	if (pRed->m_state == 2 || GetHoleIndex(pRed->m_hole) == 19)
		pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 3.0f), 2, "End");
	else if (pRed->m_state == 3)
		pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 3.0f), 2, "Out");
	else
		pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 3.0f), 2, "%d\xC8\xA6", GetHoleIndex(pRed->m_hole));

	if ((pRed->m_identity & 0x80) && m_pMassIcon)
		pGDI->DrawTexture(m_pMassIcon, WRect(pItem->pos.x + 259.0f, pItem->pos.y + 3.0f, (float)m_pMassIcon->Width(), (float)m_pMassIcon->Height()), 0xFFFFFFFF, 0);

	// the blue guild's player
	if (m_pBlueEmblem)
		pGDI->DrawTexture(m_pBlueEmblem, WRect(pItem->pos.x + 39.0f, pItem->pos.y + 21.0f, (float)(m_pBlueEmblem->Width() / 2), (float)(m_pBlueEmblem->Height() / 2)), 0xFFFFFFFF, 0);

	if (pBlue->m_uid == MyGuid(true))
		pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	else if (pBlue->m_state == 3)
		pGDI->SetTextColor(0xFF808080, 0xFFFFFFFF);
	else
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	if (!WSingleton<CProjectG>::Instance()->HidePrivacy()) {
		pGDI->SetTextColor(0xFF1D26AB, 0xFFFFFFFF);
		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 60.0f, pItem->pos.y + 20.0f), 0, pBlue->m_nickname, 90.0f, 0xFFFFFFFF);
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	}

	pGDI->Print(WPoint(pItem->pos.x + 179.0f, pItem->pos.y + 20.0f), 2, "%d", pBlue->m_uniteGuildIndex);
	pGDI->Print(WPoint(pItem->pos.x + 222.0f, pItem->pos.y + 20.0f), 2, "%I64d", pBlue->m_pang);

	if (pBlue->m_state == 2 || GetHoleIndex(pBlue->m_hole) == 19)
		pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 20.0f), 2, "End");
	else if (pBlue->m_state == 3)
		pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 20.0f), 2, "Out");
	else
		pGDI->Print(WPoint(pItem->pos.x + 255.0f, pItem->pos.y + 20.0f), 2, "%d\xC8\xA6", GetHoleIndex(pBlue->m_hole));

	if ((pBlue->m_identity & 0x80) && m_pMassIcon)
		pGDI->DrawTexture(m_pMassIcon, WRect(pItem->pos.x + 259.0f, pItem->pos.y + 3.0f, (float)m_pMassIcon->Width(), (float)m_pMassIcon->Height()), 0xFFFFFFFF, 0);

	// the separator under the match
	WPoint p1, p2;
	p1.x = pItem->pos.x + 1.0f;
	p2.x = (float)m_pRankList->GetItemWidth() + pItem->pos.x;
	p1.y = p2.y = (float)m_pRankList->GetItemHeight() + pItem->pos.y;
	g_view->DrawLine2D(p1, p2, 0xFF000000, 0xFF000000, 0);

	pGDI->SetSpace(space);
}

// scoreboard.cpp:1477
void FrRankForm3::OnBoardInit(intptr_t param) {
	m_pBoard = DYNAMIC_CAST(FrStatic, param);
	m_pBoard->m_nFlags.Enable(0x1000);

	m_pBoard->SetCaption("");
	SetCaptionType(0);

	m_captionTime = 0;
}

// scoreboard.cpp:1489
void FrRankForm3::OnScoreGaugeInit(intptr_t param) {
	m_pScoreGauge = DYNAMIC_CAST(FrGaugeBar, param);
	m_pScoreGauge->SetRange(0, 10, 5);
}

// scoreboard.cpp:1496
void FrRankForm3::SetScore(int score, int remain) {
	m_score = score;
	m_scoreRemain = remain;

	int pos = score + 1;

	m_pScoreGauge->SetRange(0, pos + remain + 1, pos);
}

// scoreboard.cpp:1509
void FrRankForm3::DrawDigit(FrGraphicInterface* pGDI, char* text, WPoint pos, unsigned long color, FrGDI::eTextAlign align, bool bShadow) {
	int len = (int)strlen(text);
	const Bitmap* bitmaps[16];
	int totalWidth = 0;

	if (align != FrGDI::ALIGN_LEFT) {
		totalWidth = 0;
		for (int i = 0; i < len; i++) {
			switch (text[i]) {
			case '+':
				bitmaps[i] = m_pPlus;
				break;
			case '-':
				bitmaps[i] = m_pMinus;
				break;
			default:
				bitmaps[i] = m_pDigit[text[i] - '0'];
				break;
			}
			totalWidth += bitmaps[i]->Width() - (bShadow ? (len < 4 ? 0 : 2) : 0);
		}
	}

	WRect rc;
	switch (align) {
	case FrGDI::ALIGN_LEFT:
		rc.x = (float)(int)pos.x;
		break;
	case FrGDI::ALIGN_CENTER:
		rc.x = (float)(int)(pos.x - totalWidth * 0.5f);
		break;
	case FrGDI::ALIGN_RIGHT:
		rc.x = (float)(int)(pos.x - totalWidth);
		break;
	}

	rc.y = pos.y;
	for (int i = 0; i < len; i++) {
		rc.w = (float)bitmaps[i]->Width();
		rc.h = (float)bitmaps[i]->Height();

		pGDI->DrawTexture(bitmaps[i], rc, color, 0);
		rc.x += rc.w - (bShadow ? (len < 4 ? 0 : 2) : 0);
	}
}

// scoreboard.cpp:1563
void FrRankForm3::OnSizeBtnUp(void) {
	if (m_pRankList == 0) {
		return;
	}

	m_sizeType = (m_sizeType + 1) % 2;

	WRect rect;
	m_pRankList->GetClientRect(rect);

	switch (m_sizeType) {
	case 0:
		rect.h = 0;
		break;
	case 1:
		rect.h = m_pRankList->GetItemHeight() * 3.0f;
		break;
	}
	m_pRankList->SetClientRect(rect);

	rect.h += 113.0f;
	rect.w = 296.0f;
	rect.y = 0;
	m_pBaseFrm->SetClientRect(rect);
}

// scoreboard.cpp:1596
void FrRankForm3::SetCaptionType(unsigned char type) {
	m_captionType = type;

	if (m_pBoard == 0) {
		return;
	}

	WRect rect;
	m_pBoard->GetClientRect(rect);

	if (m_captionType == 2) {
		WRect clip(rect.x, rect.y, 219.0f, 16.0f);
		m_pBoard->SetClippingArea(&clip);

		rect.x = 800.0f;
		rect.w = m_pBoard->GDI()->GetTextExtend(m_pBoard->GetCaption());
		m_pBoard->SetClientRect(rect);
	} else {
		m_pBoard->SetClippingArea(0);
		m_pBoard->SetClientRect(WRect(rect.x, rect.y, 219.0f, 16.0f));
	}
}

// scoreboard.cpp:1630
void FrRankForm3::OnScoreInfoInit(intptr_t param) {
	m_pScoreInfo = DYNAMIC_CAST(FrArea, param);

	m_pRedEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_guild.m_guildMark[0]);
	m_pBlueEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_guild.m_guildMark[1]);

	memset(&m_score, 0, sizeof(m_score) + sizeof(m_scoreRemain));

	m_pScoreInfo->SetVisible(true);
}

// scoreboard.cpp:1657
void FrRankForm3::OnScoreInfoOwnerDraw(void) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	WRect rect = m_pScoreInfo->GetRect();

	if (m_pRedEmblem != 0) {
		pGDI->DrawTexture(m_pRedEmblem, WRect(rect.x + 10.0f, rect.y + 1.0f, (float)m_pRedEmblem->Width(), (float)m_pRedEmblem->Height()), 0xFFFFFFFF, 0);
	}

	if (m_pBlueEmblem != 0) {
		pGDI->DrawTexture(m_pBlueEmblem, WRect(rect.x + 260.0f, rect.y + 1.0f, (float)m_pBlueEmblem->Width(), (float)m_pBlueEmblem->Height()), 0xFFFFFFFF, 0);
	}

	DrawDigit(pGDI, MakeStr("%d", m_score), WPoint(rect.x + 50.0f, rect.y + 3.0f), 0xFFFF0000, FrGDI::ALIGN_CENTER, false);
	DrawDigit(pGDI, MakeStr("%d", m_scoreRemain), WPoint(rect.x + 240.0f, rect.y + 3.0f), 0xFF0000FF, FrGDI::ALIGN_CENTER, false);
}

// scoreboard.cpp:1679
void FrRankForm3::LoadBitmaps(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_roomType != 6) {
		return;
	}

	for (int i = 0; i < 10; i++) {
		m_pDigit[i] = g_pFresh->GetManager()->GetBitmap("SCORECARD", MakeStr("%d", i));
	}
	m_pPlus = g_pFresh->GetManager()->GetBitmap("SCORECARD", "+");
	m_pMinus = g_pFresh->GetManager()->GetBitmap("SCORECARD", "-");
}

// scoreboard.cpp:1694 (run-time type and factory registration of FrRankFormEx)
const WRTTI FrRankFormEx::m_RTTI("FrRankFormEx", &FrForm::m_RTTI);

IObject* FrRankFormExMakeInstance(void) {
	return new FrRankFormEx;
}

struct __sFrRankFormEx {
	__sFrRankFormEx(void) {
		ObjectFactory().AddObjectFunctor(FrRankFormExMakeInstance, "FrRankFormEx");
	}
};
static __sFrRankFormEx s_registerFrRankFormEx;

// scoreboard.cpp:1696
const sFRESH_MSGMAP* FrRankFormEx::GetMessageMap(void) const {
	return &FrRankFormEx::_MsgMap;
}

sFRESH_MSGMAP FrRankFormEx::_MsgMap = { &FrForm::_MsgMap, &FrRankFormEx::_MsgEntries[0] };

sFRESH_ENTRY FrRankFormEx::_MsgEntries[] = {
	// scoreboard.cpp:1698
	{ "stage", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankFormEx::OnStageAreaInit },
	// scoreboard.cpp:1699
	{ "stage", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrRankFormEx::OnStageAreaOwnerDraw },
	// scoreboard.cpp:1700
	{ "list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankFormEx::OnRankInit },
	// scoreboard.cpp:1701
	{ "list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankFormEx::OnRankOwnerDraw },
	// scoreboard.cpp:1702
	{ "billboard", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRankFormEx::OnBoardInit },

	// scoreboard.cpp:1704
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// scoreboard.cpp:1712
CRankExData::CRankExData(void) : m_pRival(0), m_motionLength(3.0f), m_motionTime(0), m_leader(-1), m_showLeader(-1),
	m_pRankList(0), m_pAreaStage(0), m_pBoard(0) {
}

// scoreboard.cpp:1717
CRankExData::~CRankExData(void) {
}

// scoreboard.cpp:1722
void CRankExData::InitAreaStage(FrArea* pArea) {
	m_pAreaStage = pArea;
}

// scoreboard.cpp:1727
void CRankExData::Clear(void) {
	if (m_pExhibition) { delete m_pExhibition; m_pExhibition = 0; }
	if (m_pRival) { delete[] m_pRival; m_pRival = 0; }
}

// scoreboard.cpp:1734
void CRankExData::OnProc(float dt) {
	m_motionTime += dt;

	if (m_leader == -1) {
		SetLeader();
	} else if (m_showLeader != -1) {
		ShowLeader();
	} else if (m_pExhibition) {
		m_pExhibition->Process(dt, false);
	}
}

// scoreboard.cpp:1753
int CRankExData::GetMedalIndex(unsigned long uid) {
	if (WSingleton<CSharedDoc>::Instance()->m_gameResultList.size() < 10)
		return 3;

	unsigned char index = WSingleton<CSharedDoc>::Instance()->GetIndex(uid);

	if (index == 0xFF || WSingleton<CSharedDoc>::Instance()->m_gameResultList[index].m_characterType > 3)
		return 3;

	unsigned char rank = WSingleton<CSharedDoc>::Instance()->m_gameResultList[index].m_rank;
	unsigned char count = 0, better = 0;

	for (std::vector<sGameResultInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
		unsigned char state = it->m_state;
		if (state != 3 || GetHoleIndex(it->m_hole) >= 4)
			count++;

		if (state != 3 && it->m_rank < rank && it->m_characterType > 3)
			better++;
	}

	rank -= better;
	if (WSingleton<CSharedDoc>::Instance()->m_holes == 9) {
		if (count <= 14) {
			return 3;
		} else if (count <= 18) {
			if (rank == 1)
				return 2;
		} else if (count <= 22) {
			if (rank == 1)
				return 1;
		} else if (count <= 26) {
			if (rank == 1)
				return 1;
			else if (rank == 2)
				return 2;
		} else {
			if (rank == 1)
				return 0;
			else if (rank == 2)
				return 1;
		}
	} else if (WSingleton<CSharedDoc>::Instance()->m_holes == 18) {
		if (count <= 14) {
			if (rank == 1)
				return 2;
		} else if (count <= 18) {
			if (rank == 1)
				return 1;
			else if (rank == 2)
				return 2;
		} else if (count <= 22) {
			if (rank == 1)
				return 0;
			else if (rank == 2)
				return 1;
			else if (rank == 3)
				return 2;
		} else if (count <= 26) {
			if (rank == 1)
				return 0;
			else if (rank == 2)
				return 1;
			else if (rank <= 4)
				return 2;
		} else {
			if (rank == 1)
				return 0;
			else if (rank <= 3)
				return 1;
			else if (rank <= 6)
				return 2;
		}
	}

	return 3;
}

// scoreboard.cpp:1866
void CRankExData::CreateLeaderPet(const WRect& rect) {
	m_pExhibition = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.3f, false);
	m_pExhibition->SetDistance(g_view->GetWidth() * 0.028125f, true, false);
	m_pExhibition->SetArea(rect.w, rect.h);
}

// scoreboard.cpp:1873
void CRankExData::SetActive(bool bActive) {
	m_showLeader = -1;
	m_leader = -1;
	m_motionLength = 3.0f;
	m_motionTime = 0;
	if (bActive) {
		SetLeader();
	}
}

// scoreboard.cpp:1883
bool CRankExData::IsMotionFinished(void) {
	return m_motionTime > (m_motionLength + 1.0f > 10.0f ? 10.0f : m_motionLength + 1.0f);
}

// scoreboard.cpp:1888
void CRankExData::ResetStage(void) {
	if (m_pExhibition && m_pAreaStage) {
		const WRect& rect = m_pAreaStage->GetRect();
		m_pExhibition->SetCenter(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f);
		m_pExhibition->SetDistance(g_view->GetWidth() * 0.028125f, true, false);
	}
}

// scoreboard.cpp:1899
void CRankExData::InitRivalData(void) {
	int count = WSingleton<CSharedDoc>::Instance()->m_gameResultList.size();
	m_pRival = new sRivalData[count];

	for (int i = 0; i < count; i++) {
		m_pRival[i] = WSingleton<CSharedDoc>::Instance()->m_gameResultList[i];

		m_pRankList->AddItem(&m_pRival[i]);
	}
}

// scoreboard.cpp:1912
void CRankExData::InitMedalData(void) {
	m_pRankGold = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_gold");
	m_pRankSilver = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_silver");
	m_pRankBronze = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_bronze");
	m_pRankNoAward = g_pFresh->GetManager()->GetBitmap("FRAMES4", "rank_noaward");

	m_pRedTeamRank = g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_team_redteam_rank");
	m_pBlueTeamRank = g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_team_blueteam_rank");
	m_pRank = g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_rank");

	m_pMassListIcon = g_pFresh->GetManager()->GetBitmap("30s", "pc_icon_mass_list");

	m_pZeroIcon = g_pFresh->GetManager()->GetBitmap("30s", "icon_zero");
}

// scoreboard.cpp:1931
// Draws one row of the ranking list: rank, medal, title, emblem, nickname,
// score, pang, hole and the mass/zero icon.
void CRankExData::DrawRankItem(FrListItem* pItem) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	int space = pGDI->SetSpace(0);

	sGameResultInfo* pInfo = (sGameResultInfo*)pItem->pData;

	pGDI->SetTextStyle(0);
	pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 5) {
		if ((&m_pRedTeamRank)[pInfo->m_team])
			pGDI->DrawTexture((&m_pRedTeamRank)[pInfo->m_team], WRect(pItem->pos.x + 12.0f, pItem->pos.y + 2.0f, (float)(&m_pRedTeamRank)[pInfo->m_team]->Width(), (float)(&m_pRedTeamRank)[pInfo->m_team]->Height()), 0xFFFFFFFF, 0);

		pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1, "%d", pInfo->m_rank);
	} else {
		if (m_pRank)
			pGDI->DrawTexture(m_pRank, WRect(pItem->pos.x + 12.0f, pItem->pos.y + 2.0f, (float)m_pRank->Width(), (float)m_pRank->Height()), 0xFFFFFFFF, 0);

		if (pInfo->m_state == 3)
			pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1, "-");
		else
			pGDI->Print(WPoint(pItem->pos.x + 21.0f, pItem->pos.y + 3.0f), 1, "%d", pInfo->m_rank);
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_userLimit < 100) {
		if (pInfo->m_hole != WSingleton<CSharedDoc>::Instance()->m_holeOrder[0]) {
			int medal = GetMedalIndex(pInfo->m_uid);

			if (medal >= 0 && medal <= 2) {
				if (pInfo->m_characterType > 3)
					medal = 3;

				pGDI->DrawTexture((&m_pRankGold)[medal], WRect(pItem->pos.x + 2.0f, pItem->pos.y + 1.0f, (float)(&m_pRankGold)[medal]->Width(), (float)(&m_pRankGold)[medal]->Height()), 0xFFFFFFFF, 0);
			}
		}
	}

	if (pInfo->m_uid == MyGuid(true)) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 5)
			pGDI->SetTextColor(pInfo->m_team ? 0xFF0060FF : 0xFFFF0000, 0xFFFFFFFF);
		else
			pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	} else if (pInfo->m_state == 3) {
		pGDI->SetTextColor(0xFF808080, 0xFFFFFFFF);
	} else {
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	}

	const Bitmap* pTitle;
	IFF_STRUCT::sSkin* pSkin;
	if (pInfo->m_titleTypeId
		&& (pSkin = ItemManager()->FindSkin(pInfo->m_titleTypeId)) != 0) {
		pTitle = g_pFresh->GetManager()->GetBitmap("TITLES", pSkin->m_icon);
	} else {
		pTitle = g_pFresh->GetManager()->GetBitmap("LEVELS", MakeStr("level_%03d", pInfo->m_level + 1));
	}

	if (pTitle) {
		WRect rect;
		rect.w = (float)pTitle->Width();
		rect.h = (float)pTitle->Height();

		rect.x = float2int((60.0f - rect.w) * 0.5f) + pItem->pos.x + 35.0f;
		rect.y = pItem->pos.y + 8.0f - float2int(rect.h * 0.5f);
		pGDI->DrawTexture(pTitle, rect, 0xFFFFFFFF, 0);
	}

	if (pInfo->m_hasEmblem) {
		const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pInfo->m_emblemName);

		if (pEmblem)
			pGDI->DrawTexture(pEmblem, WRect(pItem->pos.x + 100.0f, pItem->pos.y + 3.0f - 6.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
	}

	if (!WSingleton<CProjectG>::Instance()->HidePrivacy())
		g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 127.0f, pItem->pos.y + 3.0f), 0, pInfo->m_nickname, 90.0f, 0xFFFFFFFF);

	pGDI->Print(WPoint(pItem->pos.x + 240.0f, pItem->pos.y + 3.0f), 2, "%d", pInfo->m_score);
	pGDI->Print(WPoint(pItem->pos.x + 285.0f, pItem->pos.y + 3.0f), 2, "%I64d", pInfo->m_pang);

	if (pInfo->m_state == 2 || GetHoleIndex(pInfo->m_hole) == 19)
		pGDI->Print(WPoint(pItem->pos.x + 323.0f, pItem->pos.y + 3.0f), 2, "End");
	else if (pInfo->m_state == 3)
		pGDI->Print(WPoint(pItem->pos.x + 323.0f, pItem->pos.y + 3.0f), 2, "Out");
	else
		pGDI->Print(WPoint(pItem->pos.x + 323.0f, pItem->pos.y + 3.0f), 2, "%d\xC8\xA6", GetHoleIndex(pInfo->m_hole));

	if ((pInfo->m_identity & 0x80) && m_pMassListIcon)
		pGDI->DrawTexture(m_pMassListIcon, WRect(WPoint(pItem->pos.x + 329.0f, pItem->pos.y + 3.0f), (float)m_pMassListIcon->Width(), (float)m_pMassListIcon->Height()), 0xFFFFFFFF, 0);
	else if (m_pZeroIcon)
		pGDI->DrawTexture(m_pZeroIcon, WRect(WPoint(pItem->pos.x + 331.0f, pItem->pos.y + 6.0f), (float)m_pZeroIcon->Width(), (float)m_pZeroIcon->Height()), 0xFFFFFFFF, 0);

	pGDI->SetSpace(space);
}

// scoreboard.cpp:2096
// Draws the leader's title and name over the head of the model on the stage.
void CRankExData::DrawStageArea(void) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	if (m_leader == -1 || m_showLeader != -1 || !m_pExhibition)
		return;

	m_pExhibition->Display(0, 0);

	if (!m_pExhibition->GetPetFrame())
		return;

	WBone* pHead = m_pExhibition->GetPetFrame()->GetBonePet()->FindBone("Bip01 Head");
	if (!pHead)
		return;

	const WVector& head = pHead->GetMatrix().pivot;
	const WVector2D& center = m_pExhibition->GetCenter();

	float ratio = g_view->GetRatio();
	float fov = g_view->GetFOV();
	g_view->SetFOV(ratio * 2.0943952f);
	g_view->UpdateCamera();

	const Bitmap* pTitle;
	IFF_STRUCT::sSkin* pSkin;
	if (WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(m_leader)].m_titleTypeId
		&& (pSkin = ItemManager()->FindSkin(WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(m_leader)].m_titleTypeId)) != 0) {
		pTitle = g_pFresh->GetManager()->GetBitmap("TITLES", pSkin->m_icon);
	} else {
		pTitle = g_pFresh->GetManager()->GetBitmap("LEVELS", MakeStr("level_%03d", WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(m_leader)].m_level + 1));
	}

	if (pTitle) {
		WVector screen = g_view->Projection(head);

		if (0.0f < screen.z && screen.z < 1.0f) {
			sGameResultInfo* pInfo = &WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(m_leader)];
			const char* pName = pInfo->m_nickname;
			float textWidth = pGDI->GetTextExtend(pName);

			float titleWidth = (float)pTitle->Width();
			float titleHeight = (float)pTitle->Height();

			float x = (float)float2int(screen.x + center.x - (titleWidth + g_view->GetWidth() + textWidth + 5.0f) * 0.5f);
			float y = (float)float2int(screen.y - 52.0f + center.y - g_view->GetHeight() * 0.5f);
			pGDI->DrawTexture(pTitle, WRect(x, y + 8.0f - (float)float2int(titleHeight * 0.5f), titleWidth, titleHeight), 0xFFFFFFFF, 0);

			WPoint pt(x + titleWidth + 5.0f, y + 2.0f);
			pGDI->SetTextColor(0xFFFFFFFF, 0xFF000000);
			pGDI->SetTextStyle(2);

			if (pInfo->m_hasEmblem) {
				const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pInfo->m_emblemName);
				if (pEmblem) {
					pGDI->DrawTexture(pEmblem, WRect(pt.x, pt.y - 6.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
				}
				pt.x += 27.0f;
			}

			if (!WSingleton<CProjectG>::Instance()->HidePrivacy()) {
				g_pFresh->GetManager()->PrintText(pt, 0, pName, -1.0f, 0xFFFFFFFF);
			}
		}
	}

	g_view->SetFOV(fov);
	g_view->UpdateCamera();
}

// scoreboard.cpp:2210
// Picks the leader at random among the players ranked first, asks the server
// for the leader's user info if it is not known yet and shows the leader.
void CRankExData::SetLeader(void) {
	unsigned char myHole = WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_hole;

	if (GetHoleIndex(myHole) <= GetHoleIndex(GOLFDOC()->m_currentHole)) {
		if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT))
			return;
	}

	std::vector<sGameResultInfo>& resultList = WSingleton<CSharedDoc>::Instance()->m_gameResultList;

	for (int i = 0; i < (int)resultList.size(); i++) {
		sGameResultInfo& result = resultList[i];
		m_pRival[i].m_state = result.m_state;
		m_pRival[i].m_pang = result.m_pang;
		m_pRival[i].m_score = result.m_score;
		m_pRival[i].m_hole = result.m_hole;
		m_pRival[i].m_rank = result.m_rank;
		m_pRival[i].m_sortGroup = result.m_sortGroup;
		m_pRival[i].m_finishOrder = result.m_finishOrder;
	}

	m_pRankList->SortItem(RankCompare);

	int firstCount = 0;

	for (int i = 0; i < (int)resultList.size(); i++) {
		if (m_pRival[i].m_rank == 1)
			firstCount++;
	}

	if (firstCount == 0)
		return;

	int pick = rand() % firstCount;

	firstCount = 0; int leaderIndex = resultList.size();

	for (int i = 0; i < (int)resultList.size(); i++) {
		if (m_pRival[i].m_rank == 1) {
			if (firstCount == pick) {
				leaderIndex = i;
				m_leader = m_pRival[i].m_uid;
				break;
			}

			firstCount++;
		}
	}

	if (m_leader == -1)
		return;

	if (WSingleton<CSharedDoc>::Instance()->GetUserInfoMod() == 0) {
		std::map<unsigned long, sUserInfoTime>::iterator itTime;
		if (!WSingleton<CSharedDoc>::Instance()->FindUserInfoTimeUID(m_pRival[leaderIndex].m_id, itTime)) {
			WSendPacket packet(0x2F);
			packet.Encode4(m_pRival[leaderIndex].m_id);
			packet.Encode1(5);
			packet.Send(SEND_TO_DEFAULT);

			WSingleton<CSharedDoc>::Instance()->SetUserInfoMod(1);
		}
	}

	ShowLeader();

	char text[256];
	sRivalData* pLeader = &m_pRival[leaderIndex];

	sprintf(text, "\xC7\xF6\xC0\xE7 1\xC0\xA7 %s\xB4\xD4, \xBD\xBA\xC4\xDA\xBE\xEE %d",
		pLeader->m_nickname, pLeader->m_score);
	m_pBoard->SetCaption(text);
}

// scoreboard.cpp:2310
void CRankExData::ShowLeader(void) {
	std::map<unsigned long, sUserInfoTime>::iterator itTime;
	unsigned long leader = m_showLeader == -1 ? m_leader : m_showLeader;

	int found = 0;
	std::map<unsigned long, sBriefUserInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_userMap.find(leader);
	if (it != WSingleton<CSharedDoc>::Instance()->m_userMap.end())
		found = WSingleton<CSharedDoc>::Instance()->FindUserInfoTimeUID(it->second.id, itTime);

	if (!found) {
		if (m_showLeader == -1)
			m_showLeader = m_leader;
		return;
	}

	if (WSingleton<CSharedDoc>::Instance()->GetUserInfoMod() == 1)
		return;

	m_showLeader = -1;
	CPartTidList tidList;

	sCharacterInfo* pCharInfo = &itTime->second.m_userInfo.m_characterInfo;

	tidList.SetTids(pCharInfo, 0xFF);

	IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(pCharInfo->typeId);

	if (m_pExhibition) {
		m_pExhibition->SetDistance(g_view->GetWidth() * 0.028125f, true, false);
		if (IsLocalCountry(30)) {
			if (pChar == 0 || pChar->m_awardPose[0] == '\0') {
				m_pExhibition->SetModel(tidList, pCharInfo->auxParts.tids, "1\265\356\270\360\274\307", 0.5f, leader);
			} else if (stricmp(pChar->m_awardPose, "1st_place") == 0) {
				m_pExhibition->SetModel(tidList, pCharInfo->auxParts.tids, "1\265\356\270\360\274\307", 0.5f, leader);
			} else if (stricmp(pChar->m_awardPose, "2nd_place") == 0) {
				m_pExhibition->SetModel(tidList, pCharInfo->auxParts.tids, "2\265\356\270\360\274\307", 0.5f, leader);
			} else {
				m_pExhibition->SetModel(tidList, pCharInfo->auxParts.tids, "1\265\356\270\360\274\307", 0.5f, leader);
			}
		} else {
			m_pExhibition->SetModel(tidList, pCharInfo->auxParts.tids,
				(pChar && pChar->m_awardPose[0]) ? pChar->m_awardPose : "1\265\356\270\360\274\307", 0.5f, leader);
		}

		m_motionLength = m_pExhibition->GetInitialMotionLength();
	}
}

// scoreboard.cpp:2391
void CRankExData::ClearRival(void) {
	if (m_pRival) { delete[] m_pRival; m_pRival = 0; }
}

// scoreboard.cpp:2396
void CRankExData::SetRivalData(void) {
	int count = WSingleton<CSharedDoc>::Instance()->m_gameResultList.size();

	m_pRival = new sRivalData[count];
	m_pRankList->ClearItem();
	for (int i = 0; i < count; i++) {
		m_pRival[i] = WSingleton<CSharedDoc>::Instance()->m_gameResultList[i];
		m_pRankList->AddItem(&m_pRival[i]);
	}
}

// scoreboard.cpp:2414
FrRankFormEx::FrRankFormEx(void) {
}

// scoreboard.cpp:2425
FrRankFormEx::~FrRankFormEx(void) {
}

// scoreboard.cpp:2432
void FrRankFormEx::OnStageAreaInit(intptr_t param) {
	FrArea* pArea = DYNAMIC_CAST(FrArea, param);
	if (!pArea) {
		return;
	}

	char bgName[64] = { 0 };
	sprintf(bgName, "30in_back_%s", WSingleton<CSharedDoc>::Instance()->m_pCurCourse ? WSingleton<CSharedDoc>::Instance()->m_pCurCourse->m_mapName : 0);
	pArea->SetBgImg(bgName);

	// the leader's pet stands on the stage area
	m_rankEx.m_pAreaStage = pArea;
	m_rankEx.CreateLeaderPet(pArea->GetRect());
}

// scoreboard.cpp:2447
void FrRankFormEx::OnStageAreaOwnerDraw(void) {
	if (IsVisible()) {
		char bgName[64] = { 0 };
		sprintf(bgName, "30in_back_%s", WSingleton<CSharedDoc>::Instance()->m_pCurCourse ? WSingleton<CSharedDoc>::Instance()->m_pCurCourse->m_mapName : 0);
		m_rankEx.m_pAreaStage->SetBgImg(bgName);

		m_rankEx.DrawStageArea();
	}
}

// scoreboard.cpp:2463
void FrRankFormEx::OnRankInit(intptr_t param) {
	m_rankEx.m_pRankList = DYNAMIC_CAST(FrListBox, param);
	m_rankEx.InitRivalData();
	m_rankEx.InitMedalData();
}

// scoreboard.cpp:2480
void FrRankFormEx::SortRank(void) {
	memset(WSingleton<CSharedDoc>::Instance()->m_teamResult, 0, sizeof(WSingleton<CSharedDoc>::Instance()->m_teamResult));

	for (std::vector<sGameResultInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
		if (it->m_state == 3) {
			it->m_rank = (unsigned char)WSingleton<CSharedDoc>::Instance()->m_gameResultList.size() - it->m_sortGroup;
			continue;
		}

		it->m_rank = 1;
		for (std::vector<sGameResultInfo>::iterator other = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); other != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++other) {
			if (it != other && other->m_state != 3) {
				if (it->m_score > other->m_score || (it->m_score == other->m_score && it->m_pang < other->m_pang))
					it->m_rank++;
			}
		}

		WSingleton<CSharedDoc>::Instance()->m_teamResult[it->m_team].m_score += it->m_rank;
		WSingleton<CSharedDoc>::Instance()->m_teamResult[it->m_team].m_pang += it->m_pang;
	}

	m_rankEx.m_pRankList->SortItem(RankCompare);
}

// scoreboard.cpp:2513
sRivalData* FrRankFormEx::GetRival(void) {
	return m_rankEx.m_pRival;
}

// scoreboard.cpp:2517
void FrRankFormEx::OnRankOwnerDraw(intptr_t param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == 0) {
		return;
	}
	m_rankEx.DrawRankItem(pItem);
}

// scoreboard.cpp:2526
void FrRankFormEx::OnBoardInit(intptr_t param) {
	m_rankEx.m_pBoard = DYNAMIC_CAST(FrStatic, param);
	m_rankEx.m_pBoard->m_nFlags.Enable(0x1000);
}

// scoreboard.cpp:2532
void FrRankFormEx::Clear(void) {
	m_rankEx.Clear();
}

// scoreboard.cpp:2537
void FrRankFormEx::SetActive(bool bActive) {
	m_rankEx.SetActive(bActive);
	Enable(bActive);
	SetVisible(bActive);
}

// scoreboard.cpp:2545
void FrRankFormEx::SetLeader(void) {
	m_rankEx.SetLeader();
}

// scoreboard.cpp:2550
void FrRankFormEx::ShowLeader(void) {
	m_rankEx.ShowLeader();
}

// scoreboard.cpp:2555
bool FrRankFormEx::OnInit(void) {
	Enable(false);
	SetVisible(false);
	return true;
}

// scoreboard.cpp:2562
void FrRankFormEx::OnProc(const float dt) {
	if (!m_dwStyle.GetFlag(0x1)) {
		return;
	}

	m_rankEx.OnProc(dt);
}

// scoreboard.cpp:2570
bool FrRankFormEx::IsMotionFinished(void) {
	return m_rankEx.IsMotionFinished();
}

// scoreboard.cpp:2575
void FrRankFormEx::ResetStage(void) {
	m_rankEx.ResetStage();
}

// scoreboard.cpp:2582
// Fills slots (the player index of each row of the award list, best first)
// by the room type.
static void __stdcall SortAwardSlots(int* slots) {
	unsigned char roomType = WSingleton<CSharedDoc>::Instance()->m_roomType;
	if (roomType == 1) {
		if (TEAM(0)->m_resetCount > TEAM(1)->m_resetCount
			|| (TEAM(0)->m_resetCount == TEAM(1)->m_resetCount
				&& TEAM(0)->m_teamValue >= (int)TEAM(1)->m_teamValue)) {
			slots[0] = 0;
			slots[1] = 1;
		} else {
			slots[0] = 1;
			slots[1] = 0;
		}
	} else if (roomType == 3) {
		if (GOLFDOC()->GetPlayer(0)->m_state == 3) {
			slots[0] = 1;
			slots[1] = 0;
		} else if (PLAYER(1)->m_state == 3) {
			slots[0] = 0;
			slots[1] = 1;
		} else if ((int)PLAYER(0)->m_ghostResult > (int)PLAYER(1)->m_ghostResult
			|| (PLAYER(0)->m_ghostResult == PLAYER(1)->m_ghostResult
				&& PLAYER(0)->m_ghostAward >= (int)PLAYER(1)->m_ghostAward)) {
			slots[0] = 0;
			slots[1] = 1;
		} else {
			slots[0] = 1;
			slots[1] = 0;
		}
	} else if (roomType == 7) {
		int n = 0;
		for (int rank = 0; rank < GOLFDOC()->GetPlayerNum(); rank++) {
			for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
				if (GOLFDOC()->GetPlayer(i)->m_state != 3 && GOLFDOC()->GetPlayer(i)->m_joinState == rank + 1) {
					slots[n++] = i;
				}
			}
		}
		for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			if (GOLFDOC()->GetPlayer(i)->m_state == 3) {
				slots[n++] = i;
			}
		}
	} else if (roomType == 6) {
		slots[0] = 0;
		slots[1] = 1;
	} else {
		for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			if (GOLFDOC()->GetPlayer(i)->m_state == 3
				&& (WSingleton<CSharedDoc>::Instance()->m_gameMode == 1 || WSingleton<CSharedDoc>::Instance()->m_gameMode == 5)) {
				continue;
			}
			slots[PLAYER(i)->m_turnOrder - 1] = i;
		}
	}
}

// scoreboard.cpp:2672 (run-time type and factory registration of FrAwardForm)
const WRTTI FrAwardForm::m_RTTI("FrAwardForm", &FrForm::m_RTTI);

IObject* FrAwardFormMakeInstance(void) {
	return new FrAwardForm;
}

struct __sFrAwardForm {
	__sFrAwardForm(void) {
		ObjectFactory().AddObjectFunctor(FrAwardFormMakeInstance, "FrAwardForm");
	}
};
static __sFrAwardForm s_registerFrAwardForm;

// scoreboard.cpp:2674
const sFRESH_MSGMAP* FrAwardForm::GetMessageMap(void) const {
	return &FrAwardForm::_MsgMap;
}

sFRESH_MSGMAP FrAwardForm::_MsgMap = { &FrForm::_MsgMap, &FrAwardForm::_MsgEntries[0] };

sFRESH_ENTRY FrAwardForm::_MsgEntries[] = {
	// scoreboard.cpp:2676
	{ "tab0", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnTab0Init },
	// scoreboard.cpp:2677
	{ "tab1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnTab1Init },
	// scoreboard.cpp:2678
	{ "userlist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnUserListInit },
	// scoreboard.cpp:2679
	{ "userlist", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnUserListOwnerDraw },
	// scoreboard.cpp:2680
	{ "expbar0", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnExpBar0Init },
	// scoreboard.cpp:2681
	{ "expbar1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnExpBar1Init },
	// scoreboard.cpp:2682
	{ "expbar2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnExpBar2Init },
	// scoreboard.cpp:2683
	{ "expbar3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnExpBar3Init },
	// scoreboard.cpp:2684
	{ "exp0", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnExp0Init },
	// scoreboard.cpp:2685
	{ "exp1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnExp1Init },
	// scoreboard.cpp:2686
	{ "exp2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnExp2Init },
	// scoreboard.cpp:2687
	{ "exp3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnExp3Init },
	// scoreboard.cpp:2688
	{ "expend0", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnExpEnd0Init },
	// scoreboard.cpp:2689
	{ "expend1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnExpEnd1Init },
	// scoreboard.cpp:2690
	{ "expend2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnExpEnd2Init },
	// scoreboard.cpp:2691
	{ "expend3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnExpEnd3Init },
	// scoreboard.cpp:2692
	{ "level0", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnLevel0Init },
	// scoreboard.cpp:2693
	{ "level1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnLevel1Init },
	// scoreboard.cpp:2694
	{ "level2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnLevel2Init },
	// scoreboard.cpp:2695
	{ "level3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnLevel3Init },
	// scoreboard.cpp:2696
	{ "levelup0", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnLevelUp0Init },
	// scoreboard.cpp:2697
	{ "levelup1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnLevelUp1Init },
	// scoreboard.cpp:2698
	{ "levelup2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnLevelUp2Init },
	// scoreboard.cpp:2699
	{ "levelup3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnLevelUp3Init },
	// scoreboard.cpp:2700
	{ "match_res0", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnMatchRes0Init },
	// scoreboard.cpp:2701
	{ "match_res1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnMatchRes1Init },
	// scoreboard.cpp:2702
	{ "GainedLadderPoint0", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnGainedLadderPoint0Init },
	// scoreboard.cpp:2703
	{ "GainedLadderPoint1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnGainedLadderPoint1Init },
	// scoreboard.cpp:2704
	{ "LadderPoint0", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnLadderPoint0Init },
	// scoreboard.cpp:2705
	{ "LadderPoint1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnLadderPoint1Init },
	// scoreboard.cpp:2706
	{ "nextlevel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAwardForm::OnNextLevelTextInit },

	// scoreboard.cpp:2708
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// scoreboard.cpp:2712
FrAwardForm::FrAwardForm(void) {
	m_sfx = 0;
	m_expTick = 0.0f;
	m_pUserList = 0;
	m_numRows = 0;
	memset(m_pExpBar, 0, sizeof(m_pExpBar));
	memset(m_pExp, 0, sizeof(m_pExp));
	memset(m_pExpEnd, 0, sizeof(m_pExpEnd));
	memset(m_pLevel, 0, sizeof(m_pLevel));
	memset(m_pLevelUp, 0, sizeof(m_pLevelUp));
	memset(m_pMatchRes, 0, sizeof(m_pMatchRes));

	memset(m_slotPlayer, 0, sizeof(m_slotPlayer));
	memset(m_level, 0, sizeof(m_level));
	memset(m_gainExp, 0, sizeof(m_gainExp));
	memset(m_exp, 0, sizeof(m_exp));
	memset(m_expDone, 0, sizeof(m_expDone));

	memset(m_pGainedLadderPoint, 0, sizeof(m_pGainedLadderPoint));
	memset(m_pLadderPoint, 0, sizeof(m_pLadderPoint));
	m_pNextLevelText = 0;
	m_elapsed = 0.0f;

	m_pMascotBonus01 = 0;
	m_pMascotBonus02 = 0;
	m_pMascotBonus09 = 0;
	m_pMascotBonus10 = 0;
	m_pMascotBonus11 = 0;
	m_pMascotBonus12 = 0;
}

// scoreboard.cpp:2740
FrAwardForm::~FrAwardForm(void) {
}

// scoreboard.cpp:2744
bool FrAwardForm::Close(bool bDestroy) {
	if (m_sfx) {
		g_audio->StopSfx(m_sfx);
	}
	return FrForm::Close(bDestroy);
}

// scoreboard.cpp:2752
void FrAwardForm::OnUserListInit(intptr_t param) {
	m_pUserList = DYNAMIC_CAST(FrListBox, param);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
		m_numRows = GOLFDOC()->GetPlayerNum();
		unsigned char team = GOLFDOC()->GetTeamGameWinner() == 2 ? 0 : (unsigned char)GOLFDOC()->GetTeamGameWinner();
		for (int i = 0; i < m_numRows; i++) {
			if (GOLFDOC()->GetPlayer(i)->m_partnerIndex != 0xFF) {
				if (GOLFDOC()->GetPlayer(i)->m_attribute == team) {
					m_slotPlayer[i >= GOLFDOC()->GetPlayer(i)->m_partnerIndex] = i;
					GOLFDOC()->GetPlayer(i)->m_turnOrder = GOLFDOC()->GetPlayer(i)->m_joinState = i >= GOLFDOC()->GetPlayer(i)->m_partnerIndex ? 2 : 1;
				} else {
					m_slotPlayer[i >= GOLFDOC()->GetPlayer(i)->m_partnerIndex ? 3 : 2] = i;
					GOLFDOC()->GetPlayer(i)->m_turnOrder = GOLFDOC()->GetPlayer(i)->m_joinState = i >= GOLFDOC()->GetPlayer(i)->m_partnerIndex ? 4 : 3;
				}
			} else {
				if (GOLFDOC()->GetPlayer(i)->m_attribute == team) {
					m_slotPlayer[0] = i;
					GOLFDOC()->GetPlayer(i)->m_joinState = 1;
					GOLFDOC()->GetPlayer(i)->m_turnOrder = 1;
				} else {
					m_slotPlayer[1] = i;
					GOLFDOC()->GetPlayer(i)->m_joinState = 2;
					GOLFDOC()->GetPlayer(i)->m_turnOrder = 2;
				}
			}
		}
	} else {
		m_numRows = GOLFDOC()->GetPlayerNum();
		SortAwardSlots(m_slotPlayer);
	}

	for (int i = 0; i < m_numRows; i++) {
		m_pUserList->AddItem((void*)m_slotPlayer[i]);
	}
}

// scoreboard.cpp:2810
// Draws a row of the award list: the player's name and scores, the team
// totals in a team game, then the bonus icons (item boost, 20% bonus,
// mascot bonus, or the "zero" mark when the row has none).
void FrAwardForm::OnUserListOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == 0)
		return;

	int idx = (int)pItem->pData;
	bool bFirst;
	float textY;

	if (idx >= GOLFDOC()->GetPlayerNum())
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0)
		return;

	_WRECT rect = { pItem->pos.x, pItem->pos.y, (float)m_pUserList->GetItemWidth(), (float)m_pUserList->GetItemHeight() };
	float yOff = 0.0f;

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
		// a team game: both rows of the local player's team are highlighted
		if (idx == MyIndex() || (PLAYER(MyIndex())->m_partnerIndex != 0xFF && idx == PLAYER(MyIndex())->m_partnerIndex))
			pGDI->Box(rect, 0x999B9DFF, 0, 0.0f);

		// the first row of a pair of partners
		if (PLAYER(MyIndex())->m_partnerIndex != 0xFF && idx < PLAYER(idx)->m_partnerIndex)
			bFirst = true;
		else
			bFirst = false;

		if (m_numRows < 4) {
			textY = 8.0f;
		} else {
			textY = bFirst ? 11.0f : 5.0f;
			yOff = bFirst ? 3.0f : -3.0f;
		}

		if (!WSingleton<CProjectG>::Instance()->HidePrivacy())
			g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 98.0f, textY + pItem->pos.y), 0, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[idx].m_name, 70.0f, 0xFFFFFFFF);

		if (!bFirst) {
			// the team's totals, once per team
			if (m_numRows < 4) {
				pGDI->Print(WPoint(pItem->pos.x + 194.0f, pItem->pos.y + 8.0f), 2, "%d", TEAM(PLAYER(idx)->m_attribute)->m_resetCount);
				pGDI->Print(WPoint(pItem->pos.x + 237.0f, pItem->pos.y + 8.0f), 2, "%d", (int)TEAM(PLAYER(idx)->m_attribute)->m_teamValue);

				pGDI->SetTextColor(0xFF006493, 0xFFFFFFFF);
				pGDI->Print(WPoint(pItem->pos.x + 243.0f, pItem->pos.y + 8.0f), 0, "(+%d)", (int)TEAM(PLAYER(idx)->m_attribute)->m_teamValue2);
				pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
			} else {
				pGDI->Print(WPoint(pItem->pos.x + 194.0f, pItem->pos.y - 6.0f), 2, "%d", TEAM(PLAYER(idx)->m_attribute)->m_resetCount);
				pGDI->Print(WPoint(pItem->pos.x + 237.0f, pItem->pos.y - 6.0f), 2, "%d", (int)TEAM(PLAYER(idx)->m_attribute)->m_teamValue);

				pGDI->SetTextColor(0xFF006493, 0xFFFFFFFF);
				pGDI->Print(WPoint(pItem->pos.x + 243.0f, pItem->pos.y - 6.0f), 0, "(+%d)", (int)TEAM(PLAYER(idx)->m_attribute)->m_teamValue2);
				pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
			}
		}
	} else {
		sPlayerData* pPlayer = GOLFDOC()->GetPlayer(idx); if (pPlayer->m_uid == MyGuid(true))
			pGDI->Box(rect, 0x999B9DFF, 0, 0.0f);
		else if (PLAYER(idx)->m_state == 3)
			pGDI->Box(rect, 0x80808080, 0, 0.0f);

		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 0)
			pGDI->Print(WPoint(pItem->pos.x + 19.0f, pItem->pos.y + 8.0f), 1, "%d", PLAYER(idx)->m_turnOrder);

		if (!WSingleton<CProjectG>::Instance()->HidePrivacy())
			g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 98.0f, pItem->pos.y + 8.0f), 0, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[idx].m_name, 70.0f, 0xFFFFFFFF);

		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 0) {
			if (PLAYER(idx)->m_ghostResult == 0)
				pGDI->Print(WPoint(pItem->pos.x + 194.0f, pItem->pos.y + 8.0f), 2, "0");
			else
				pGDI->Print(WPoint(pItem->pos.x + 194.0f, pItem->pos.y + 8.0f), 2, "%+d", PLAYER(idx)->m_ghostResult);
		} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
			pGDI->Print(WPoint(pItem->pos.x + 194.0f, pItem->pos.y + 8.0f), 2, "%I64d", PLAYER(idx)->m_itemPang);
		} else {
			pGDI->Print(WPoint(pItem->pos.x + 194.0f, pItem->pos.y + 8.0f), 2, "%d", PLAYER(idx)->m_ghostResult);
		}

		pGDI->Print(WPoint(pItem->pos.x + 237.0f, pItem->pos.y + 8.0f), 2, "%d", (int)GOLFDOC()->GetPlayer(idx)->m_ghostAward);
		pGDI->SetTextColor(0xFF006493, 0xFFFFFFFF);
		pGDI->Print(WPoint(pItem->pos.x + 243.0f, pItem->pos.y + 8.0f), 0, "(%+d)", (int)GOLFDOC()->GetPlayer(idx)->m_extraPang);
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	}

	bool bIcon = false;
	sSlotInfo* pSlot = 0;
	std::list<sSlotInfo>& players = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList;
	for (std::list<sSlotInfo>::iterator it = players.begin(); it != players.end(); ++it) {
		if (it->m_uid == WSingleton<CSharedDoc>::Instance()->m_roomPlayers[idx].m_guid) {
			pSlot = &*it;
			break;
		}
	}

	if (pSlot) {
		for (int i = 0; i < 10; i++) {
			if (pSlot->m_extraFlags && m_pBonusMa2) {
				WRect rc(pItem->pos.x + 477.0f, yOff + pItem->pos.y + 3.0f, (float)m_pBonusMa2->Width(), (float)m_pBonusMa2->Height());
				pGDI->DrawTexture(m_pBonusMa2, rc, 0xFFFFFFFF, 0);
				bIcon = true;
			} else if (pSlot->m_itemBoostFlags && m_pBonusMa) {
				WRect rc(pItem->pos.x + 474.0f, yOff + pItem->pos.y + 3.0f, (float)m_pBonusMa->Width(), (float)m_pBonusMa->Height());
				pGDI->DrawTexture(m_pBonusMa, rc, 0xFFFFFFFF, 0);
				bIcon = true;
			}
		}
	}

	if ((WSingleton<CSharedDoc>::Instance()->m_roomPlayers[idx].m_identity & 0x80) ? true : false) {
		if (m_pBonus20Icon) {
			WRect rc(pItem->pos.x + 412.0f, yOff + pItem->pos.y + 6.0f, (float)m_pBonus20Icon->Width(), (float)m_pBonus20Icon->Height());
			pGDI->DrawTexture(m_pBonus20Icon, rc, 0xFFFFFFFF, 0);
		}
		bIcon = true;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomPlayers[idx].m_mascot.typeId) {
		IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[idx].m_mascot.typeId);
		if (pMascot) {
			if (strcmpi(pMascot->m_name, "\xc7\xcf\xbe\xe7\xc1\xa6\xb8\xae") == 0) {
				const Bitmap* pIcon = m_pMascotBonus01;
				if (pIcon)
					pGDI->DrawTexture(pIcon, WRect(pItem->pos.x + 458.0f, yOff + pItem->pos.y + 1.0f, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);
				bIcon = true;
			} else if (strcmpi(pMascot->m_name, "\xb1\xee\xb8\xc1\xc1\xa6\xb8\xae") == 0) {
				const Bitmap* pIcon = m_pMascotBonus02;
				if (pIcon)
					pGDI->DrawTexture(pIcon, WRect(pItem->pos.x + 458.0f, yOff + pItem->pos.y + 1.0f, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);
				bIcon = true;
			} else if (pMascot->m_typeId == 0x40000009) {
				const Bitmap* pIcon = m_pMascotBonus09;
				if (pIcon)
					pGDI->DrawTexture(pIcon, WRect(pItem->pos.x + 458.0f, yOff + pItem->pos.y + 1.0f, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);
				bIcon = true;
			} else if (pMascot->m_typeId == 0x4000000A) {
				const Bitmap* pIcon = m_pMascotBonus10;
				if (pIcon)
					pGDI->DrawTexture(pIcon, WRect(pItem->pos.x + 458.0f, yOff + pItem->pos.y + 1.0f, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);
				bIcon = true;
			} else if (pMascot->m_typeId == 0x4000000B) {
				const Bitmap* pIcon = m_pMascotBonus11;
				if (pIcon)
					pGDI->DrawTexture(pIcon, WRect(pItem->pos.x + 458.0f, yOff + pItem->pos.y + 1.0f, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);
				bIcon = true;
			} else if (pMascot->m_typeId == 0x4000000C) {
				const Bitmap* pIcon = m_pMascotBonus12;
				if (pIcon)
					pGDI->DrawTexture(pIcon, WRect(pItem->pos.x + 458.0f, yOff + pItem->pos.y + 1.0f, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);
				bIcon = true;
			}
		}
	}

	if (!bIcon) {
		const Bitmap* pIcon = m_pZeroVsIcon;
		if (pIcon)
			pGDI->DrawTexture(pIcon, WRect(pItem->pos.x + 442.0f, yOff + pItem->pos.y + 10.0f, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);
	}
}

// scoreboard.cpp:3054
void FrAwardForm::OnTab0Init(PG_MSGARG param) {
	if ((__rtti_obj = (void*)param) != 0 && ((IObject*)__rtti_obj)->IsKindOf(&FrArea::m_RTTI)) {
	}
}

// scoreboard.cpp:3061
void FrAwardForm::OnTab1Init(PG_MSGARG param) {
	if ((__rtti_obj = (void*)param) != 0 && ((IObject*)__rtti_obj)->IsKindOf(&FrArea::m_RTTI)) {
	}
}

// scoreboard.cpp:3068
void FrAwardForm::OnExpBar0Init(intptr_t param) {
	m_pExpBar[0] = DYNAMIC_CAST(FrGaugeBar, param);
	m_pExpBar[0]->SetVisible(false);
}

// scoreboard.cpp:3074
void FrAwardForm::OnExpBar1Init(intptr_t param) {
	m_pExpBar[1] = DYNAMIC_CAST(FrGaugeBar, param);
	m_pExpBar[1]->SetVisible(false);
}

// scoreboard.cpp:3080
void FrAwardForm::OnExpBar2Init(intptr_t param) {
	m_pExpBar[2] = DYNAMIC_CAST(FrGaugeBar, param);
	m_pExpBar[2]->SetVisible(false);
}

// scoreboard.cpp:3086
void FrAwardForm::OnExpBar3Init(intptr_t param) {
	m_pExpBar[3] = DYNAMIC_CAST(FrGaugeBar, param);
	m_pExpBar[3]->SetVisible(false);
}

// scoreboard.cpp:3092
void FrAwardForm::OnExp0Init(intptr_t param) {
	m_pExp[0] = DYNAMIC_CAST(FrStatic, param);
	m_pExp[0]->SetVisible(false);
}

// scoreboard.cpp:3098
void FrAwardForm::OnExp1Init(intptr_t param) {
	m_pExp[1] = DYNAMIC_CAST(FrStatic, param);
	m_pExp[1]->SetVisible(false);
}

// scoreboard.cpp:3104
void FrAwardForm::OnExp2Init(intptr_t param) {
	m_pExp[2] = DYNAMIC_CAST(FrStatic, param);
	m_pExp[2]->SetVisible(false);
}

// scoreboard.cpp:3110
void FrAwardForm::OnExp3Init(intptr_t param) {
	m_pExp[3] = DYNAMIC_CAST(FrStatic, param);
	m_pExp[3]->SetVisible(false);
}

// scoreboard.cpp:3116
void FrAwardForm::OnExpEnd0Init(intptr_t param) {
	m_pExpEnd[0] = DYNAMIC_CAST(FrStatic, param);
	m_pExpEnd[0]->SetVisible(false);
}

// scoreboard.cpp:3122
void FrAwardForm::OnExpEnd1Init(intptr_t param) {
	m_pExpEnd[1] = DYNAMIC_CAST(FrStatic, param);
	m_pExpEnd[1]->SetVisible(false);
}

// scoreboard.cpp:3128
void FrAwardForm::OnExpEnd2Init(intptr_t param) {
	m_pExpEnd[2] = DYNAMIC_CAST(FrStatic, param);
	m_pExpEnd[2]->SetVisible(false);
}

// scoreboard.cpp:3134
void FrAwardForm::OnExpEnd3Init(intptr_t param) {
	m_pExpEnd[3] = DYNAMIC_CAST(FrStatic, param);
	m_pExpEnd[3]->SetVisible(false);
}

// scoreboard.cpp:3140
void FrAwardForm::OnLevel0Init(intptr_t param) {
	m_pLevel[0] = DYNAMIC_CAST(FrArea, param);
	m_pLevel[0]->SetVisible(false);
}

// scoreboard.cpp:3146
void FrAwardForm::OnLevel1Init(intptr_t param) {
	m_pLevel[1] = DYNAMIC_CAST(FrArea, param);
	m_pLevel[1]->SetVisible(false);
}

// scoreboard.cpp:3152
void FrAwardForm::OnLevel2Init(intptr_t param) {
	m_pLevel[2] = DYNAMIC_CAST(FrArea, param);
	m_pLevel[2]->SetVisible(false);
}

// scoreboard.cpp:3158
void FrAwardForm::OnLevel3Init(intptr_t param) {
	m_pLevel[3] = DYNAMIC_CAST(FrArea, param);
	m_pLevel[3]->SetVisible(false);
}

// scoreboard.cpp:3164
void FrAwardForm::OnLevelUp0Init(intptr_t param) {
	m_pLevelUp[0] = DYNAMIC_CAST(FrArea, param);
	m_pLevelUp[0]->SetVisible(false);
}

// scoreboard.cpp:3170
void FrAwardForm::OnLevelUp1Init(intptr_t param) {
	m_pLevelUp[1] = DYNAMIC_CAST(FrArea, param);
	m_pLevelUp[1]->SetVisible(false);
}

// scoreboard.cpp:3176
void FrAwardForm::OnLevelUp2Init(intptr_t param) {
	m_pLevelUp[2] = DYNAMIC_CAST(FrArea, param);
	m_pLevelUp[2]->SetVisible(false);
}

// scoreboard.cpp:3182
void FrAwardForm::OnLevelUp3Init(intptr_t param) {
	m_pLevelUp[3] = DYNAMIC_CAST(FrArea, param);
	m_pLevelUp[3]->SetVisible(false);
}

// scoreboard.cpp:3188
void FrAwardForm::OnMatchRes0Init(intptr_t param) {
	m_pMatchRes[0] = DYNAMIC_CAST(FrArea, param);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 0) {
		m_pMatchRes[0]->SetVisible(false);
	}
}

// scoreboard.cpp:3196
void FrAwardForm::OnMatchRes1Init(intptr_t param) {
	m_pMatchRes[1] = DYNAMIC_CAST(FrArea, param);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 0) {
		m_pMatchRes[1]->SetVisible(false);
	}
}

// scoreboard.cpp:3205
void FrAwardForm::OnGainedLadderPoint0Init(intptr_t param) {
	m_pGainedLadderPoint[0] = DYNAMIC_CAST(FrStatic, param);
}

// scoreboard.cpp:3211
void FrAwardForm::OnGainedLadderPoint1Init(intptr_t param) {
	m_pGainedLadderPoint[1] = DYNAMIC_CAST(FrStatic, param);
}

// scoreboard.cpp:3217
void FrAwardForm::OnLadderPoint0Init(intptr_t param) {
	m_pLadderPoint[0] = DYNAMIC_CAST(FrStatic, param);
}

// scoreboard.cpp:3223
void FrAwardForm::OnLadderPoint1Init(intptr_t param) {
	m_pLadderPoint[1] = DYNAMIC_CAST(FrStatic, param);
}

// scoreboard.cpp:3228
void FrAwardForm::OnNextLevelTextInit(PG_MSGARG param) {
	if (!m_pNextLevelText) {
		m_pNextLevelText = DYNAMIC_CAST(FrEdit, param);
	}
}

// scoreboard.cpp:3236
bool FrAwardForm::OnInit(void) {
	FrForm::OnInit();

	WPoint pos;

	if (m_pUserList) {
		m_pUserList->EnableScrollBar(false);

		WRect rect = m_pUserList->GetRect();
		float itemHeight = m_pUserList->GetItemHeight();
		float height = itemHeight * m_numRows;
		float grow = height - rect.h;
		rect.h = height;
		m_pUserList->SetRect(rect);

		float formHeight = grow + m_rect.h;
		pos.x = m_rect.x;
		m_rect.h = formHeight;
		pos.y = g_view->GetHeight() - formHeight + 5.0f;
		MoveWindow(pos);
		SetRect(m_rect);

		float textHeight = m_pNextLevelText->GetRect().h;
		pos.x = m_rect.x + 25.0f;
		pos.y = g_view->GetHeight() - textHeight;
		m_pNextLevelText->MoveWindow(pos);
	}

	unsigned char gameType = WSingleton<CSharedDoc>::Instance()->m_roomType;
	if (gameType == 0) {
		CalcResult();
	} else if (gameType == 1) {
		// Team game: each row moves up or down by its team.
		float offset = 0.0f;
		if (m_numRows < 4)
			offset = -3.0f;

		for (int i = 0; i < 4; i++) {
			bool upper;
			if (PLAYER((unsigned char)m_slotPlayer[i])->m_partnerIndex != 0xFF)
				upper = m_slotPlayer[i] < PLAYER((unsigned char)m_slotPlayer[i])->m_partnerIndex;
			else
				upper = true;


			const WRect& barRect = m_pExpBar[i]->GetRect();
			pos = WPoint(barRect.x, barRect.y + (upper ? offset + 3.0f : offset - 3.0f));
			m_pExpBar[i]->MoveWindow(pos);

			const WRect& expRect = m_pExp[i]->GetRect();
			pos = WPoint(expRect.x, expRect.y + (upper ? offset + 3.0f : offset - 3.0f));
			m_pExp[i]->MoveWindow(pos);

			const WRect& expEndRect = m_pExpEnd[i]->GetRect();
			pos = WPoint(expEndRect.x, expEndRect.y + (upper ? offset + 3.0f : offset - 3.0f));
			m_pExpEnd[i]->MoveWindow(pos);

			const WRect& levelRect = m_pLevel[i]->GetRect();
			pos = WPoint(levelRect.x, levelRect.y + (upper ? 3.0f : -3.0f));
			pos = WPoint(levelRect.x, levelRect.y + (upper ? offset + 3.0f : offset - 3.0f));
			m_pLevel[i]->MoveWindow(pos);

			const WRect& levelUpRect = m_pLevelUp[i]->GetRect();
			pos = WPoint(levelUpRect.x, levelUpRect.y + (upper ? offset + 3.0f : offset - 3.0f));
			m_pLevelUp[i]->MoveWindow(pos);
		}

		if ((unsigned char)GOLFDOC()->GetTeamGameWinner() == 2) {
			m_pMatchRes[0]->SetBgImg("draw");
			m_pMatchRes[1]->SetBgImg("draw");
		} else {
			m_pMatchRes[0]->SetBgImg("win");
			m_pMatchRes[1]->SetBgImg("lose");
		}

		if (m_numRows < 4) {
			for (int i = 0; i < 2; i++) {
				m_pMatchRes[i]->MoveWindow(WPoint(m_pMatchRes[i]->GetRect().x, m_pMatchRes[i]->GetRect().y + (i == 0 ? -18.0f : -43.0f)));
			}
		}

		CalcResult();
	} else if (gameType == 7) {
		if (PLAYER(0)->m_joinState == PLAYER(1)->m_joinState) {
			m_pMatchRes[0]->SetBgImg("draw");
			m_pMatchRes[1]->SetBgImg("draw");

			const WRect& resRect = m_pMatchRes[0]->GetRect();
			m_pMatchRes[0]->MoveWindow(WPoint(resRect.x, resRect.y - 18.0f));
			m_pMatchRes[1]->SetVisible(false);
		} else {
			m_pMatchRes[0]->SetBgImg("win");
			m_pMatchRes[1]->SetBgImg("lose");

			for (int i = 0; i < 2; i++) {
				m_pMatchRes[i]->MoveWindow(WPoint(m_pMatchRes[i]->GetRect().x, m_pMatchRes[i]->GetRect().y + (i == 0 ? -18.0f : -43.0f)));
			}
		}

		CalcResult();
	} else if (gameType == 3) {
		if (PLAYER(0)->m_joinState == PLAYER(1)->m_joinState) {
			m_pMatchRes[0]->SetBgImg("draw");
			m_pMatchRes[1]->SetBgImg("draw");
		} else {
			m_pMatchRes[0]->SetBgImg("win");
			m_pMatchRes[1]->SetBgImg("lose");
		}

		for (int i = 0; i < 2; i++) {
			m_pMatchRes[i]->MoveWindow(WPoint(m_pMatchRes[i]->GetRect().x, m_pMatchRes[i]->GetRect().y + (i == 0 ? -18.0f : -43.0f)));
		}

		if (GOLFDOC()->GetPlayer(0)->m_state == 3 || GOLFDOC()->GetPlayer(1)->m_state == 3)
			m_pMatchRes[1]->SetVisible(false);

		CalcResultMatch();
	}

	m_pVsIcon = g_pFresh->GetManager()->GetBitmap("golf", "pc_icon_vs");
	m_pBonus20Icon = g_pFresh->GetManager()->GetBitmap("golf", "pc_icon_bonus_20");

	m_pMascotBonus01 = g_pFresh->GetManager()->GetBitmap("golf", "bonus_mascot_01");
	m_pMascotBonus02 = g_pFresh->GetManager()->GetBitmap("golf", "bonus_mascot_02");

	m_pMascotBonus09 = g_pFresh->GetManager()->GetBitmap("golf", "bonus_mascot_09");
	m_pMascotBonus10 = g_pFresh->GetManager()->GetBitmap("golf", "bonus_mascot_10");
	m_pMascotBonus11 = g_pFresh->GetManager()->GetBitmap("golf", "bonus_mascot_11");
	m_pMascotBonus12 = g_pFresh->GetManager()->GetBitmap("golf", "bonus_mascot_12");

	m_pBonusMa = g_pFresh->GetManager()->GetBitmap("golf", "bonus_ma");
	m_pBonusMa2 = g_pFresh->GetManager()->GetBitmap("golf", "bonus_ma2");
	m_pZeroVsIcon = g_pFresh->GetManager()->GetBitmap("golf", "icon_zero_vs");

	m_pNextLevelText->SetVisible(IsLocalContent((localContentType_t)0x29));

	// The next level's name for the local player, unless the round's exp
	// already passes it.
	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		sPlayerData* pPlayer = GOLFDOC()->GetPlayer((unsigned char)i);
		if (pPlayer->m_uid == WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().guid) {
			unsigned long exp = GOLFDOC()->m_roundResults[i].m_exp + WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_value155;

			if (exp > WSingleton<CSharedDoc>::Instance()->m_levelExp[m_level[i]].m_exp) {
				m_pNextLevelText->SetVisible(false);
			} else {
				std::string text;

				if (m_level[i] < 21) {
					text.assign(s_levelUpGuide[m_level[i]].m_notice);
				} else if (m_level[i] <= 65) {
					text.assign(data_9f19c4[m_level[i]]);
					text += s_levelUpGuide[21].m_notice;
				}

				m_pNextLevelText->AddLine(text.c_str(), 0, false);
			}
		}
	}

	// The result voice of the local player's character.
	int result = 0;   // 0 won, 1 draw, 2 lost
	unsigned char roomType = WSingleton<CSharedDoc>::Instance()->m_roomType;
	if (roomType == 1) {
		unsigned char me = GalleryMode() ? GOLFDOC()->m_myPlayerIndex : WSingleton<CSharedDoc>::Instance()->GetIndex(WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().guid);
		if (TEAM(PLAYER(me)->m_attribute)->m_resetCount < TEAM(PLAYER(MyIndex())->m_attribute == 0)->m_resetCount)
			result = 2;
		if (TEAM(PLAYER(MyIndex())->m_attribute)->m_resetCount == TEAM(PLAYER(MyIndex())->m_attribute == 0)->m_resetCount)
			result = 1;
	} else if (roomType == 0) {
		unsigned char me = GalleryMode() ? GOLFDOC()->m_myPlayerIndex : WSingleton<CSharedDoc>::Instance()->GetIndex(WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().guid);
		int myScore = (int)PLAYER(me)->m_ghostResult;
		int myPang = PLAYER(MyIndex())->m_ghostAward;

		int best = 0;
		for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			if (i == MyIndex())
				continue;

			if ((int)PLAYER((unsigned char)i)->m_ghostResult < (int)PLAYER((unsigned char)best)->m_ghostResult
				|| (PLAYER((unsigned char)i)->m_ghostResult == PLAYER((unsigned char)best)->m_ghostResult
					&& PLAYER((unsigned char)i)->m_ghostAward > PLAYER((unsigned char)best)->m_ghostAward))
				best = i;

			if ((int)GOLFDOC()->GetPlayer((unsigned char)i)->m_ghostResult < myScore
				|| ((int)GOLFDOC()->GetPlayer((unsigned char)i)->m_ghostResult == myScore
					&& GOLFDOC()->GetPlayer((unsigned char)i)->m_ghostAward > myPang))
				result = 2;
		}

		if (result != 0) {
			if ((int)GOLFDOC()->GetPlayer((unsigned char)best)->m_ghostResult == myScore
				&& GOLFDOC()->GetPlayer((unsigned char)best)->m_ghostAward == myPang)
				result = 1;
		}
	} else if (roomType == 7) {
		// In room type 7 m_selectedIndexB holds the winner's UID, 0 for a draw.
		unsigned long winner = GOLFDOC()->m_selectedIndexB;
		if (WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().guid != winner)
			result = 2;
		else if (winner == 0)
			result = 1;
	}

	int waveState;
	switch (result) {
	case 0:
		waveState = 0x40;
		break;
	case 2:
		waveState = 0x200;
		break;
	default:
		return true;
	}

	WSingleton<CVoiceItem>::Instance()->PlaySound(
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[MyIndex()].m_clubSet.typeId,
		(CVoiceItem::eWaveState)waveState,
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[MyIndex()].m_characterInfo.typeId & 0x3FFFFFF);

	return true;
}

// scoreboard.cpp:3537
void FrAwardForm::OnProc(const float deltaTime) {
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 3)
		return;

	m_expTick += deltaTime;
	if (m_expTick < 1.0f)
		return;

	if (!m_sfx && !m_expDone[MyIndex()]) {
		if (m_gainExp[MyIndex()] > 0 || m_gainExp[MyIndex()] < 0)
			m_sfx = "player_exp_up";

		if (m_sfx && !g_audio->IsPlaying(m_sfx))
			g_audio->PlaySfx(m_sfx, 0, 1, m_sfx, 0, 0.5f, 200.0f);
	}

	if (m_expTick - 1.0f < 0.01f)
		return;

	// The exp counts one tenth per 10 ms.
	int steps = (int)((m_expTick - 1.0f) * 100.0f);
	m_expTick -= steps * 0.01f;

	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (m_expDone[i] || (WSingleton<CSharedDoc>::Instance()->m_roomType != 1 && GOLFDOC()->GetPlayer((unsigned char)i)->m_state == 3))
			continue;

		int levelExp = WSingleton<CSharedDoc>::Instance()->m_levelExp[m_level[i]].m_exp * 10;

		if (steps > Abs(m_gainExp[i])) {
			m_expDone[i] = 1;
			steps = Abs(m_gainExp[i]);
			int sign = (m_gainExp[i] > 0) * 2 - 1;
			m_gainExp[i] = 0;
			m_exp[i] += sign * steps;
		} else {
			int delta = ((m_gainExp[i] > 0) * 2 - 1) * steps;
			m_exp[i] += delta;
			m_gainExp[i] -= delta;
		}

		if (m_exp[i] < 0 && !m_expDone[i]) {
			m_expDone[i] = 1;
			m_exp[i] = 0;
		}

		int row = GOLFDOC()->GetPlayer((unsigned char)i)->m_turnOrder - 1;
		if (row < 0 || m_numRows <= row)
			continue;

		char buf[32];
		if (m_exp[i] >= levelExp) {
			g_audio->PlaySfx("level_up", 0, 0, 0, 0, 0.5f, 200.0f);

			m_exp[i] -= levelExp;
			m_level[i]++;

			if (m_pExpBar[row]) {
				m_pExpBar[row]->SetRange(0, WSingleton<CSharedDoc>::Instance()->m_levelExp[m_level[i]].m_exp * 10, 0);
				m_pExpBar[row]->SetPos(m_exp[i]);
			}

			if (m_pExpEnd[row]) {
				sprintf(buf, "/ %d", WSingleton<CSharedDoc>::Instance()->m_levelExp[m_level[i]].m_exp);
				m_pExpEnd[row]->SetCaption(buf);
			}

			if (m_pLevel[row]) {
				sprintf(buf, "level_%03d", m_level[i] + 1);
				m_pLevel[row]->SetBgImg(buf);
			}

			AfxGetTask()->GetActor("Player") << MsgObject(0, 0x1F8, i, 0, 0, 0, 0);
		} else if (m_pExpBar[row]) {
			m_pExpBar[row]->SetPos(m_exp[i]);
		}

		sprintf(buf, "%d", m_exp[i] / 10);
		m_pExp[row]->SetCaption(buf);

		if (m_expDone[i]) {
			if (m_level[i] != WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_level) {
				if (m_pExp[row])
					m_pExp[row]->SetVisible(false);
				if (m_pExpEnd[row])
					m_pExpEnd[row]->SetVisible(false);
				if (m_pLevelUp[row])
					m_pLevelUp[row]->SetVisible(true);

				std::list<sSlotInfo>& players = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList;
				for (std::list<sSlotInfo>::iterator it = players.begin(); it != players.end(); ++it) {
					if (it->m_uid == WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_guid) {
						it->m_level = (unsigned char)m_level[i];
						break;
					}
				}

				// The head icons show the new levels; the room keeps its own.
				if (WSingleton<CHeadIcon>::Instance()) {
					unsigned char savedLevels[128];
					for (int j = 0; j < GOLFDOC()->GetPlayerNum(); j++) {
						savedLevels[j] = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[j].m_level;
						WSingleton<CSharedDoc>::Instance()->m_roomPlayers[j].m_level = (unsigned char)m_level[j];
					}

					WSingleton<CHeadIcon>::Instance()->SetLevel();

					for (int j = 0; j < GOLFDOC()->GetPlayerNum(); j++)
						WSingleton<CSharedDoc>::Instance()->m_roomPlayers[j].m_level = savedLevels[j];
				}
			}

			if (i == MyIndex() && m_sfx)
				g_audio->StopSfx(m_sfx);
		}
	}

	m_elapsed += deltaTime;
	if (WSingleton<CTHunter>::Instance()->IsReceiveGiftPacket() && m_elapsed > 2.0f)
		WSingleton<CSharedDoc>::Instance()->m_value4844 = 1;
}

// scoreboard.cpp:3707
void FrAwardForm::CalcResult(void) {
	char text[32];

	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomType != 1 && GOLFDOC()->GetPlayer((unsigned char)i)->m_state == 3)
			continue;

		sPlayerData* pPlayer = GOLFDOC()->GetPlayer((unsigned char)i);
		int row = pPlayer->m_turnOrder - 1;

		if (row < 0 || m_numRows <= row)
			continue;

		if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
			m_level[i] = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_level;
			m_exp[i] = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_value155;
			m_gainExp[i] = GOLFDOC()->m_roundResults[i].m_exp;
		} else {
			unsigned char teamPar5 = 0;
			unsigned char par4 = 0;
			unsigned char par5 = 0;
			unsigned char par3 = 0;

			for (int j = 0; j < WSingleton<CSharedDoc>::Instance()->m_holes; j++) {
				if (WSingleton<CSharedDoc>::Instance()->m_roomType == 0) {
					switch (GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[j]).m_kind) {
					case 2:
						par3++;
						break;
					case 3:
						par4++;
						break;
					case 4:
						par5++;
						break;
					}
				} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
					if (GOLFDOC()->GetHoleData((unsigned char)j).m_kind == 4)
						teamPar5++;
				}
			}

			m_level[i] = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_level;
			m_exp[i] = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_value155;

			if (WSingleton<CSharedDoc>::Instance()->m_roomType == 0) {
				IFF_STRUCT::sCourse* pCourse = WSingleton<CSharedDoc>::Instance()->m_pCurCourse;
				unsigned char difficulty = (pCourse == 0 ? (unsigned char)1 : (unsigned char)(pCourse->courseFlags & 0xF)) - 1;
				// The table rows start 0x10 bytes before data_9f1f48's first entry.
				m_gainExp[i] = data_9f1f48[difficulty][par5][pPlayer->m_joinState + 4]
					+ data_9f1f48[difficulty][par4][pPlayer->m_joinState + 1]
					+ data_9f1f48[difficulty][par3][pPlayer->m_joinState - 1];
			} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
				bool lost;
				if (GOLFDOC()->GetTeamGameWinner() == 2 || pPlayer->m_attribute == GOLFDOC()->GetTeamGameWinner())
					lost = false;
				else
					lost = true;

				IFF_STRUCT::sCourse* pCourse = WSingleton<CSharedDoc>::Instance()->m_pCurCourse;
				unsigned char difficulty = (pCourse == 0 ? (unsigned char)1 : (unsigned char)(pCourse->courseFlags & 0xF)) - 1;
				m_gainExp[i] = data_9f1f48[difficulty][teamPar5][lost - 2];
			}
		}

		m_exp[i] *= 10;
		m_gainExp[i] *= 10;
		int maxExp = WSingleton<CSharedDoc>::Instance()->m_levelExp[m_level[i]].m_exp * 10;

		if (m_gainExp[i] == 0)
			m_expDone[i] = 1;

		if (m_pExpBar[row]) {
			m_pExpBar[row]->SetRange(0, maxExp, 0);
			m_pExpBar[row]->SetPos(m_exp[i]);
		}

		if (m_pExp[row]) {
			sprintf(text, "%d", m_exp[i] / 10);
			m_pExp[row]->SetCaption(text);
		}

		if (m_pExpEnd[row]) {
			sprintf(text, "/ %d", WSingleton<CSharedDoc>::Instance()->m_levelExp[m_level[i]].m_exp);
			m_pExpEnd[row]->SetCaption(text);
		}

		if (m_pLevel[row]) {
			sprintf(text, "level_%03d", m_level[i] + 1);
			m_pLevel[row]->SetBgImg(text);
		}

		if (m_pExpBar[row])
			m_pExpBar[row]->SetVisible(true);

		if (m_pExp[row])
			m_pExp[row]->SetVisible(true);

		if (m_pExpEnd[row])
			m_pExpEnd[row]->SetVisible(true);

		if (m_pLevel[row])
			m_pLevel[row]->SetVisible(true);
	}
}

// scoreboard.cpp:3867
void FrAwardForm::CalcResultMatch(void) {
	char text[32] = "";

	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (GOLFDOC()->GetPlayer((unsigned char)i)->m_state == 3)
			continue;

		sPlayerData* pPlayer = GOLFDOC()->GetPlayer((unsigned char)i);
		int row = pPlayer->m_turnOrder - 1;

		if (row < 0 || m_numRows <= row)
			continue;

		if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
			m_exp[i] = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_ladderPoint;
			m_gainExp[i] = GOLFDOC()->m_roundResults[i].m_exp;
			m_exp[i] += m_gainExp[i];
		} else {
			unsigned char teamPar5 = 0;
			unsigned char par4 = 0;
			unsigned char par5 = 0;
			unsigned char par3 = 0;

			for (int j = 0; j < WSingleton<CSharedDoc>::Instance()->m_holes; j++) {
				if (WSingleton<CSharedDoc>::Instance()->m_roomType == 0) {
					switch (GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[j]).m_kind) {
					case 2:
						par3++;
						break;
					case 3:
						par4++;
						break;
					case 4:
						par5++;
						break;
					}
				} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
					if (GOLFDOC()->GetHoleData((unsigned char)j).m_kind == 4)
						teamPar5++;
				}
			}

			m_level[i] = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_level;
			m_exp[i] = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_value155;

			IFF_STRUCT::sCourse* pCourse = WSingleton<CSharedDoc>::Instance()->m_pCurCourse;
			unsigned char difficulty = (pCourse == 0 ? (unsigned char)1 : (unsigned char)(pCourse->courseFlags & 0xF)) - 1;

			if (WSingleton<CSharedDoc>::Instance()->m_roomType == 0) {
				// The table rows start 0x10 bytes before data_9f1f48's first entry.
				m_gainExp[i] = data_9f1f48[difficulty][par5][pPlayer->m_joinState + 4]
					+ data_9f1f48[difficulty][par4][pPlayer->m_joinState + 1]
					+ data_9f1f48[difficulty][par3][pPlayer->m_joinState - 1];
			} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
				bool lost;
				if (GOLFDOC()->GetTeamGameWinner() == 2 || pPlayer->m_attribute == GOLFDOC()->GetTeamGameWinner())
					lost = false;
				else
					lost = true;

				m_gainExp[i] = data_9f1f48[difficulty][teamPar5][lost - 2];
			}
		}

		if (m_gainExp[i] == 0)
			m_expDone[i] = 1;

		if (m_pGainedLadderPoint[row]) {
			sprintf(text, "%d", m_gainExp[i]);
			m_pGainedLadderPoint[row]->SetCaption(text);
			m_pGainedLadderPoint[row]->SetVisible(true);
		}

		if (m_pLadderPoint[row]) {
			sprintf(text, "%d", m_exp[i]);
			m_pLadderPoint[row]->SetCaption(text);
			m_pLadderPoint[row]->SetVisible(true);
		}
	}
}

// scoreboard.cpp:3992
cAquireItem::cAquireItem(void) : m_maxValue(8.0f) {
	m_pTitleFont = 0;
	m_value = 0;

	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		std::vector<unsigned long>& items = WSingleton<CSharedDoc>::Instance()->m_gainedItemList[i];
		for (std::vector<unsigned long>::iterator it = items.begin(); it != items.end(); ++it) {
			unsigned long typeId = *it;

			std::map<unsigned long, int>::iterator found = m_prizeSlots[i].m_counts.find(typeId);
			if (found == m_prizeSlots[i].m_counts.end()) {
				m_prizeSlots[i].m_counts.insert(std::map<unsigned long, int>::value_type(typeId, 1));
			} else {
				found->second++;
			}
		}
	}

	m_pItemFrame[0] = g_pFresh->GetManager()->GetBitmap("golf", "itemframe_item_one");
	m_pItemFrame[1] = g_pFresh->GetManager()->GetBitmap("golf", "itemframe_item_two");
	m_pItemFrame[2] = g_pFresh->GetManager()->GetBitmap("golf", "itemframe_item_three");

	m_pBubble[0] = g_pFresh->GetBitmap("golf_text_bubble1");
	m_pBubble[1] = g_pFresh->GetBitmap("golf_text_bubble2");
	m_pBubble[2] = g_pFresh->GetBitmap("golf_text_bubble3");

	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));
	info.filename = data_a82930;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:";
	m_pTitleFont = 0;
	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&info);

	m_pUI = new CSimpleUI("[emoti_object.png");
	m_pUI->Add("0", WRect(0.0f, 128.0f, 64.0f, 64.0f), false);
	m_pUI->Add("1", WRect(64.0f, 128.0f, 64.0f, 64.0f), false);
	m_pUI->Add("2", WRect(128.0f, 128.0f, 64.0f, 64.0f), false);
	m_pUI->Add("3", WRect(192.0f, 128.0f, 64.0f, 64.0f), false);
	m_pUI->Add("4", WRect(0.0f, 192.0f, 64.0f, 64.0f), false);
	m_pUI->Add("5", WRect(64.0f, 192.0f, 64.0f, 64.0f), false);
	m_pUI->Add("6", WRect(128.0f, 192.0f, 64.0f, 64.0f), false);
	m_pUI->Add("7", WRect(192.0f, 192.0f, 64.0f, 64.0f), false);
}

// scoreboard.cpp:4086
cAquireItem::~cAquireItem(void) {
	if (m_pUI) { delete m_pUI; m_pUI = 0; }

	if (m_pTitleFont) {
		m_pTitleFont->Erase();
	}
	if (g_resrcmng && m_pTitleFont) { g_resrcmng->Release(m_pTitleFont); m_pTitleFont = 0; }

	for (int i = 0; i < 4; i++) {
		for (std::map<unsigned long, WOverlay*>::iterator it = m_overlays[i].begin(); it != m_overlays[i].end(); ++it) {
			if (g_resrcmng && it->second) { g_resrcmng->Release(it->second); it->second = 0; }
		}
		m_overlays[i].clear();
	}
}

// scoreboard.cpp:4106
void cAquireItem::Reset(void) {
	m_value = 0;
}

// scoreboard.cpp:4111
void cAquireItem::OnProc(float dt) {
	if (m_value < m_maxValue) {
		m_value += dt * 20.0f;
	}
}

// scoreboard.cpp:4117
// Draws the speech bubble over a player with the items the player picked up
// during the round, and the remaining time.
void cAquireItem::DisplayPrizeList(int player) {
	unsigned int count = m_prizeSlots[player].m_counts.size();

	if (count <= 0)
		return;

	WVector pos = GetPlayer2DPos(player);

	if (pos.z <= 0.0f || 1.0f <= pos.z)
		return;

	unsigned long color = ((int)((m_maxValue < m_value ? m_maxValue : m_value) / m_maxValue * 255.0f) << 24) | 0xFFFFFF;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI && m_pBubble[0]) {
		pGDI->DrawTexture(m_pBubble[0], WRect(pos.x, pos.y + 65.0f, (float)m_pBubble[0]->Width(), (float)m_pBubble[0]->Height()), color, 0);

		float left = (float)m_pBubble[0]->Width();
		WRect middle(pos.x + left, pos.y + 65.0f, (float)m_pBubble[1]->Width() * (float)(count >> 1), (float)m_pBubble[1]->Height());
		pGDI->DrawTexture(m_pBubble[1], WRect(0.0f, 0.0f, (float)m_pBubble[1]->Width(), (float)m_pBubble[1]->Height()), middle, color, 0);

		pGDI->DrawTexture(m_pBubble[2], WRect((float)m_pBubble[1]->Width() * (float)(count >> 1) + left + pos.x, pos.y + 65.0f,
			(float)m_pBubble[2]->Width(), (float)m_pBubble[2]->Height()), color, 0);
	}

	float offset = 5.0f;
	for (std::map<unsigned long, int>::const_iterator it = m_prizeSlots[player].m_counts.begin(); it != m_prizeSlots[player].m_counts.end(); ++it) {
		unsigned long typeId = it->first;
		WOverlay* pOverlay = 0;

		std::map<unsigned long, WOverlay*>::iterator found = m_overlays[player].find(typeId);
		if (found != m_overlays[player].end()) {
			pOverlay = found->second;
		} else {
			IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(typeId);

			if (pItem == 0)
				continue;

			switch (typeId >> 26) {
			case 2:
			case 5:
			case 6:
			case 0x1D:
				pOverlay = g_resrcmng->GetOverlay(MakeStr("%s.tga", pItem->m_icon), 0);

				if (pOverlay == 0)
					continue;

				m_overlays[player].insert(std::pair<const unsigned long, WOverlay*>(typeId, pOverlay));
				break;
			}
		}

		if (pOverlay) {
			pOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f),
				WRect(WPoint(pos.x, pos.y) + WPoint(offset, 82.0f), (float)pOverlay->GetWidth() * 0.65f, (float)pOverlay->GetHeight() * 0.65f), 0, color, 0.0f, 0);

			if (it->second > 1) {
				char* text = MakeStr("%d", it->second);
				m_pTitleFont->Print(g_view, pos.x + offset + 17.0f, 12.0f + (pos.y + 82.0f), text, 0, color, 0);
			}

			offset += 40.0f;
		}
	}

	if (m_pUI && m_value < m_maxValue) {
		m_pUI->Render(MakeStr("%d", (int)m_value), pos.x - 64.0f, pos.y - 16.0f, 2.0f, 0);
	}
}

// scoreboard.cpp:4258
WVector cAquireItem::GetPlayer2DPos(int player) {
	WMatrix mat;
	AfxGetTask()->GetActor("Player") << MsgObject(0, 0x124, player, (intptr_t)&mat, 0, 0, 0);

	WVector pos = g_view->Projection(mat.pivot);
	return WVector(pos.x, pos.y, pos.z);
}

// scoreboard.cpp:4273 (run-time type and factory registration of FrScoreForm)
const WRTTI FrScoreForm::m_RTTI("FrScoreForm", &FrForm::m_RTTI);

IObject* FrScoreFormMakeInstance(void) {
	return new FrScoreForm;
}

struct __sFrScoreForm {
	__sFrScoreForm(void) {
		ObjectFactory().AddObjectFunctor(FrScoreFormMakeInstance, "FrScoreForm");
	}
};
static __sFrScoreForm s_registerFrScoreForm;

// scoreboard.cpp:4275
const sFRESH_MSGMAP* FrScoreForm::GetMessageMap(void) const {
	return &FrScoreForm::_MsgMap;
}

sFRESH_MSGMAP FrScoreForm::_MsgMap = { &FrForm::_MsgMap, &FrScoreForm::_MsgEntries[0] };

sFRESH_ENTRY FrScoreForm::_MsgEntries[] = {
	// scoreboard.cpp:4278
	{ "bar2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnBar2Init },
	// scoreboard.cpp:4279
	{ "bar3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnBar3Init },
	// scoreboard.cpp:4280
	{ "front9btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnFront9BtnInit },
	// scoreboard.cpp:4281
	{ "front9btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScoreForm::OnFront9BtnUp },
	// scoreboard.cpp:4282
	{ "back9btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnBack9BtnInit },
	// scoreboard.cpp:4283
	{ "back9btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScoreForm::OnBack9BtnUp },
	// scoreboard.cpp:4284
	{ "shotbtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnShotBtnInit },
	// scoreboard.cpp:4285
	{ "shotbtn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScoreForm::OnShotBtnUp },
	// scoreboard.cpp:4286
	{ "graphbtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnGraphBtnInit },
	// scoreboard.cpp:4287
	{ "graphbtn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScoreForm::OnGraphBtnUp },
	// scoreboard.cpp:4288
	{ "versusbtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnVersusBtnInit },
	// scoreboard.cpp:4289
	{ "versusbtn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScoreForm::OnVersusBtnUp },
	// scoreboard.cpp:4290
	{ "puttbtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnPuttBtnInit },
	// scoreboard.cpp:4291
	{ "puttbtn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScoreForm::OnPuttBtnUp },
	// scoreboard.cpp:4292
	{ "pangbtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnPangBtnInit },
	// scoreboard.cpp:4293
	{ "pangbtn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScoreForm::OnPangBtnUp },
	// scoreboard.cpp:4294
	{ "hole_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnHoleListInit },
	// scoreboard.cpp:4295
	{ "hole_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnHoleListOwnerDraw },
	// scoreboard.cpp:4296
	{ "par_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnParListInit },
	// scoreboard.cpp:4297
	{ "par_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnParListOwnerDraw },
	// scoreboard.cpp:4298
	{ "pang_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnPangListInit },
	// scoreboard.cpp:4299
	{ "pang_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnPangListOwnerDraw },
	// scoreboard.cpp:4300
	{ "golfer_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnGolferListInit },
	// scoreboard.cpp:4301
	{ "golfer_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnGolferListOwnerDraw },
	// scoreboard.cpp:4302
	{ "record_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnRecordListInit },
	// scoreboard.cpp:4303
	{ "record_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnRecordListOwnerDraw },
	// scoreboard.cpp:4304
	{ "total_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnTotalListInit },
	// scoreboard.cpp:4305
	{ "total_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnTotalListOwnerDraw },
	// scoreboard.cpp:4306
	{ "total", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnTotalInit },
	// scoreboard.cpp:4307
	{ "ppl", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnPPLInit },
	// scoreboard.cpp:4308
	{ "board1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnBoard1Init },
	// scoreboard.cpp:4309
	{ "board2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnBoard2Init },
	// scoreboard.cpp:4310
	{ "drawlines", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnDrawLinesInit },
	// scoreboard.cpp:4311
	{ "drawlines", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnDrawLinesOwnerDraw },
	// scoreboard.cpp:4312
	{ "cancel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnCancelBtnInit },
	// scoreboard.cpp:4313
	{ "cancel", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScoreForm::OnCancelBtnUp },
	// scoreboard.cpp:4314
	{ "close", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnCloseBtnInit },
	// scoreboard.cpp:4315
	{ "close", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrScoreForm::OnCloseBtnUp },
	// scoreboard.cpp:4317
	{ "blankpanel1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnBlankPanel01Init },
	// scoreboard.cpp:4318
	{ "blankpanel1", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnBlankPanel01OwnerDraw },
	// scoreboard.cpp:4319
	{ "blankpanel2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnBlankPanel02Init },
	// scoreboard.cpp:4320
	{ "blankpanel2", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnBlankPanel02OwnerDraw },
	// scoreboard.cpp:4321
	{ "blankpanel3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnBlankPanel03Init },
	// scoreboard.cpp:4322
	{ "blankpanel3", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrScoreForm::OnBlankPanel03OwnerDraw },

	// scoreboard.cpp:4325
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// scoreboard.cpp:4330
FrScoreForm::FrScoreForm(void) {
	m_state = STATE_NORMAL;

	m_pFront9Btn = 0;
	m_pBack9Btn = 0;
	m_pHoleList = 0;
	m_pParList = 0;
	m_bFront9 = true;

	m_pCancelBtn = 0;
	m_pCloseBtn = 0;

	m_pBar2 = 0;
	m_pBar3 = 0;
	m_pPPLArea = 0;

	unsigned char roomType = WSingleton<CSharedDoc>::Instance()->m_roomType;
	if (roomType == 7) {
		m_scoreType = 4;
	} else if (roomType == 6 || roomType == 1 || roomType == 3) {
		m_scoreType = 5;
	} else {
		m_scoreType = 0;
	}

	m_typeTime = 0;
	m_pPoint = 0;

	memset(m_pTypeBtn, 0, sizeof(m_pTypeBtn));
	memset(m_pScoreList, 0, sizeof(m_pScoreList));
	memset(m_rowData, 0, sizeof(m_rowData));
	memset(m_pDigit, 0, sizeof(m_pDigit));

	m_holePang = 0;
	m_totalPang = 0;
	m_rowCount = 1;

	m_bShowBlankPanel = WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomMode == 14;

	m_pBlankPanel01 = 0;
	m_pBlankPanel02 = 0;
	m_pBlankPanel03 = 0;
}

// scoreboard.cpp:4383
void FrScoreForm::LoadBitmaps(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
		for (int i = 0; i < 2; i++) {
			const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("SCORECARD", i == 0 ? "red" : "blue");
			if (pBmp)
				m_bitmaps.insert(std::pair<const unsigned long, const Bitmap*>(i, pBmp));
		}

		m_pWin = g_pFresh->GetManager()->GetBitmap("SCORECARD", "win");
		m_pLose = g_pFresh->GetManager()->GetBitmap("SCORECARD", "lose");
		m_pDraw = g_pFresh->GetManager()->GetBitmap("SCORECARD", "draw");
		m_pDash = g_pFresh->GetManager()->GetBitmap("SCORECARD", "-");
		m_pQuestion = g_pFresh->GetManager()->GetBitmap("SCORECARD", "question");
	} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
		m_pWin = g_pFresh->GetManager()->GetBitmap("SCORECARD", "win");
		m_pLose = g_pFresh->GetManager()->GetBitmap("SCORECARD", "lose");
		m_pDraw = g_pFresh->GetManager()->GetBitmap("SCORECARD", "draw");
		m_pDash = g_pFresh->GetManager()->GetBitmap("SCORECARD", "-");
		m_pQuestion = g_pFresh->GetManager()->GetBitmap("SCORECARD", "question");
	} else {
		char name[8];

		for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			if (GOLFDOC()->GetPlayer(i)->m_state != 3) {
				unsigned long typeId = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_characterInfo.typeId;

				if (m_bitmaps.find(typeId) == m_bitmaps.end()) {
					IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(typeId);

					if (pChar) {
						const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("ITEMS", pChar->m_icon);
						if (pBmp)
							m_bitmaps.insert(std::pair<const unsigned long, const Bitmap*>(typeId, pBmp));
					}
				}
			}
		}

		for (int i = 0; i < 4; i++) {
			switch (i) {
			case 0: strcpy(name, "1st"); break;
			case 1: strcpy(name, "2nd"); break;
			case 2: strcpy(name, "3rd"); break;
			case 3: strcpy(name, "4th"); break;
			}

			const Bitmap* pBmp = g_pFresh->GetManager()->GetBitmap("SCORECARD", name);
			if (pBmp)
				m_bitmaps.insert(std::pair<const unsigned long, const Bitmap*>(i, pBmp));

			switch (i) {
			case 0: strcpy(name, "1st_01"); break;
			case 1: strcpy(name, "2nd_01"); break;
			case 2: strcpy(name, "3rd_01"); break;
			case 3: strcpy(name, "4th_01"); break;
			}

			pBmp = g_pFresh->GetManager()->GetBitmap("SCORECARD", name);
			if (pBmp)
				m_bitmaps.insert(std::pair<const unsigned long, const Bitmap*>(i + 4, pBmp));
		}

		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 3) {
			m_pWin = g_pFresh->GetManager()->GetBitmap("SCORECARD", "win");
			m_pLose = g_pFresh->GetManager()->GetBitmap("SCORECARD", "lose");
			m_pDraw = g_pFresh->GetManager()->GetBitmap("SCORECARD", "draw");
			m_pDash = g_pFresh->GetManager()->GetBitmap("SCORECARD", "-");
			m_pQuestion = g_pFresh->GetManager()->GetBitmap("SCORECARD", "question");
		}
	}

	for (int i = 0; i < 10; i++)
		m_pDigit[i] = g_pFresh->GetManager()->GetBitmap("SCORECARD", MakeStr("%d", i));

	m_pPlus = g_pFresh->GetManager()->GetBitmap("SCORECARD", "+");
	m_pMinus = g_pFresh->GetManager()->GetBitmap("SCORECARD", "-");

	m_pPoint = g_pFresh->GetManager()->GetBitmap("SCORECARD", "point");

	m_pArrow = g_pFresh->GetManager()->GetBitmap("SCORECARD", "arrow");

	m_pMassListIcon = g_pFresh->GetManager()->GetBitmap("golf", "pc_icon_mass_list");
}

// scoreboard.cpp:4497
void FrScoreForm::SetState(eState state) {
	if (m_state == state) {
		return;
	}
	m_state = state;

	if (state == STATE_HIDDEN) {
		Enable(false);
		SetVisible(false);
	} else {
		Enable(true);
		SetVisible(true);

		if (WSingleton<CSharedDoc>::Instance()->m_holes == 18) {
			CGolfDoc* pGolfDoc = GOLFDOC();
			m_bFront9 = (GetHoleIndex(pGolfDoc->m_currentHole) < 10 || pGolfDoc->m_approachMode) ? true : false;
		} else {
			m_bFront9 = true;
		}
		SetFrontBackBtn();
	}

	if (m_pPPLArea) {
		m_pPPLArea->SetVisible(m_bPPL && m_state == 3);
	}
}

// scoreboard.cpp:4537
void FrScoreForm::Resize(void) {
	float listTop = 0.0f;
	float itemHeight = 0.0f;
	WRect rect = m_rect;

	if (m_pScoreList[0] != 0) {
		listTop = 139.0f;
		itemHeight = (float)m_pScoreList[0]->GetItemHeight();
	}

	switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
	case 1:
	case 6:
		rect.w = 547.0f; rect.h = itemHeight * 2 + listTop;
		break;
	case 3:
		rect.w = 547.0f;
		rect.h = GOLFDOC()->GetCurrentPlayerNum() * itemHeight + listTop;
		break;
	case 7:
		rect.w = 648.0f;

		rect.h = GOLFDOC()->GetPlayerNum() * itemHeight + listTop + 22.0f;
		break;
	default:
		rect.w = 612.0f;
		rect.h = GOLFDOC()->GetCurrentPlayerNum() * itemHeight + listTop;
		break;
	}

	ResizeScoreBar23(m_pBar2, m_pBar3, rect.x, rect.w);
	SetRect(rect);

	MoveWindow(WPoint((float)(int)((g_view->GetWidth() - rect.w) * 0.5f),
		(float)(int)((itemHeight * 4 + listTop - rect.h) * 0.5f) + 20.0f));
}

// scoreboard.cpp:4588
void FrScoreForm::SortLists(bool bSort) {
	unsigned char roomType = WSingleton<CSharedDoc>::Instance()->m_roomType;
	if (roomType == 0 || roomType == 3) {
		m_rowCount = GOLFDOC()->GetCurrentPlayerNum();
	} else if (roomType == 1 || roomType == 6) {
		m_rowCount = 2;
	} else if (roomType == 7) {
		m_rowCount = GOLFDOC()->GetPlayerNum();
	} else {
		m_rowCount = GOLFDOC()->GetCurrentPlayerNum();
	}

	if (bSort) {
		Resize();
	}
	for (int i = 0; i < 3; i++) {
		if (m_pScoreList[i]) {
			m_pScoreList[i]->ClearItem();
		}
	}

	SortAwardSlots((int*)m_rowData);

	for (int i = 0; i < 3; i++) {
		if (m_pScoreList[i]) {
			for (int j = 0; j < m_rowCount; j++) {
				m_pScoreList[i]->AddItem(m_rowData[j]);
			}
		}
	}
}

// scoreboard.cpp:4624
void FrScoreForm::ChangeTurn(void) {
	UpdatePang();
}

// scoreboard.cpp:4629
void FrScoreForm::UpdatePang(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
		m_holePang = TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holeRecord[GOLFDOC()->m_currentHole - 1];
		m_totalPang = TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_teamValue;
	} else {
		m_holePang = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holeRecord[GOLFDOC()->m_currentHole - 1];
		m_totalPang = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_ghostAward;
	}
}

// scoreboard.cpp:4647
void FrScoreForm::OnBar2Init(intptr_t param) {
	m_pBar2 = DYNAMIC_CAST(FrArea, param);
}

// scoreboard.cpp:4652
void FrScoreForm::OnBar3Init(intptr_t param) {
	m_pBar3 = DYNAMIC_CAST(FrArea, param);
}

// scoreboard.cpp:4659
void FrScoreForm::OnFront9BtnInit(intptr_t param) {
	m_pFront9Btn = DYNAMIC_CAST(FrButton, param);
	if (WSingleton<CSharedDoc>::Instance()->m_holes != 18) {
		m_pFront9Btn->SetVisible(false);
	}
}

// scoreboard.cpp:4666
void FrScoreForm::OnFront9BtnUp(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_holes != 18) {
		return;
	}
	m_bFront9 = true;
	SetFrontBackBtn();
}

// scoreboard.cpp:4675
void FrScoreForm::OnBack9BtnInit(intptr_t param) {
	m_pBack9Btn = DYNAMIC_CAST(FrButton, param);
	if (WSingleton<CSharedDoc>::Instance()->m_holes != 18) {
		m_pBack9Btn->SetVisible(false);
	}
}

// scoreboard.cpp:4682
void FrScoreForm::OnBack9BtnUp(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_holes != 18) {
		return;
	}
	m_bFront9 = false;
	SetFrontBackBtn();
}

// scoreboard.cpp:4691
void FrScoreForm::OnShotBtnUp(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		if (m_scoreType == 4) {
			return;
		}
		m_scoreType = 4;
	} else {
		if (m_scoreType == 0) {
			return;
		}
		m_scoreType = 0;
	}
	SetTypeBtn();
}

// scoreboard.cpp:4709
void FrScoreForm::OnGraphBtnUp(void) {
	if (m_scoreType == 1) {
		return;
	}
	m_scoreType = 1;
	SetTypeBtn();
}

// scoreboard.cpp:4718
void FrScoreForm::OnVersusBtnUp(void) {
	if (m_scoreType == 5) {
		return;
	}
	m_scoreType = 5;
	SetTypeBtn();
}

// scoreboard.cpp:4727
void FrScoreForm::OnPuttBtnUp(void) {
	if (m_scoreType == 2) {
		return;
	}
	m_scoreType = 2;
	SetTypeBtn();
}

// scoreboard.cpp:4736
void FrScoreForm::OnPangBtnUp(void) {
	if (m_scoreType == 3) {
		return;
	}
	m_scoreType = 3;
	SetTypeBtn();
}

// scoreboard.cpp:4755
void FrScoreForm::OnShotBtnInit(intptr_t param) {
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		m_pTypeBtn[4] = DYNAMIC_CAST(FrButton, param);
		if (m_pTypeBtn[4]) {
			m_pTypeBtn[4]->SetButtonImg("skin_n", FrButton::NORMAL);
			m_pTypeBtn[4]->SetButtonImg("skin_o", FrButton::OVER);
			m_pTypeBtn[4]->SetButtonImg("skin_o", FrButton::PRESSED);
		}
	} else {
		m_pTypeBtn[0] = DYNAMIC_CAST(FrButton, param);
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6 || WSingleton<CSharedDoc>::Instance()->m_roomType == 1 || WSingleton<CSharedDoc>::Instance()->m_roomType == 3) {
		m_pTypeBtn[0]->SetVisible(false);
	}
}

// scoreboard.cpp:4779
void FrScoreForm::OnGraphBtnInit(intptr_t param) {
	m_pTypeBtn[1] = DYNAMIC_CAST(FrButton, param);
	if (!m_pTypeBtn[1]) {
		return;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6 || WSingleton<CSharedDoc>::Instance()->m_roomType == 1 || WSingleton<CSharedDoc>::Instance()->m_roomType == 3 || WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		m_pTypeBtn[1]->SetVisible(false);
	}
}

// scoreboard.cpp:4789
void FrScoreForm::OnPuttBtnInit(intptr_t param) {
	m_pTypeBtn[2] = DYNAMIC_CAST(FrButton, param);
	if (!m_pTypeBtn[2]) {
		return;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6 || WSingleton<CSharedDoc>::Instance()->m_roomType == 1 || WSingleton<CSharedDoc>::Instance()->m_roomType == 3 || WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		m_pTypeBtn[2]->SetVisible(false);
	}
}

// scoreboard.cpp:4800
void FrScoreForm::OnPangBtnInit(intptr_t param) {
	m_pTypeBtn[3] = DYNAMIC_CAST(FrButton, param);
}

// scoreboard.cpp:4819
void FrScoreForm::OnVersusBtnInit(intptr_t param) {
	m_pTypeBtn[5] = DYNAMIC_CAST(FrButton, param);
	if (!m_pTypeBtn[5]) {
		return;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6 || WSingleton<CSharedDoc>::Instance()->m_roomType == 1 || WSingleton<CSharedDoc>::Instance()->m_roomType == 3) {
		// the versus button takes the shot button's place
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1 || WSingleton<CSharedDoc>::Instance()->m_roomType == 3) {
			m_pTypeBtn[5]->MoveWindow(WPoint(m_pTypeBtn[0]->GetRect().x, m_pTypeBtn[0]->GetRect().y));
		}
		m_pTypeBtn[5]->SetVisible(true);
	} else {
		m_pTypeBtn[5]->SetVisible(false);
	}
}

// scoreboard.cpp:4846
void FrScoreForm::OnHoleListInit(intptr_t param) {
	m_pHoleList = DYNAMIC_CAST(FrListBox, param);
	if (m_pHoleList) {
		m_pHoleList->AddItem(0);
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		m_pHoleList->SetItemSize(470, 22);
		m_pHoleList->SetRect(WRect(m_pHoleList->GetRect().x - 22.0f, m_pHoleList->GetRect().y, m_pHoleList->GetRect().w, m_pHoleList->GetRect().h));
	}
}

// scoreboard.cpp:4870
void FrScoreForm::OnHoleListOwnerDraw(intptr_t param) {
	if (m_bShowBlankPanel) {
		return;
	}

	FrListItem* pItem = (FrListItem*)param;
	if (pItem == 0) {
		return;
	}

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0) {
		return;
	}

	float width = m_pHoleList->GetItemWidth();
	float height = m_pHoleList->GetItemHeight();

	WPoint pt;
	pt.x = width * (1.0f / 18.0f) + pItem->pos.x;
	pt.y = height * 0.5f + pItem->pos.y - 7.0f;
	int start = m_bFront9 ? 0 : 9;
	int end = min(start + 9, GetHoles());

	for (int i = start; i < end; i++) {
		DrawDigit(pGDI, MakeStr("%d", WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]), pt, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
		pt.x = (float)(int)(width * (1.0f / 9.0f) + pt.x);
	}
}

// scoreboard.cpp:4912
void FrScoreForm::OnParListInit(intptr_t param) {
	m_pParList = DYNAMIC_CAST(FrListBox, param);
	if (m_pParList) {
		m_pParList->AddItem(0);
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		m_pParList->SetItemSize(470, 22);
		m_pParList->SetRect(WRect(m_pParList->GetRect().x - 22.0f, m_pParList->GetRect().y, m_pParList->GetRect().w, m_pParList->GetRect().h));
	}
}

// scoreboard.cpp:4936
void FrScoreForm::OnParListOwnerDraw(intptr_t param) {
	if (m_bShowBlankPanel) {
		return;
	}

	FrListItem* pItem = (FrListItem*)param;
	if (pItem == 0) {
		return;
	}

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0) {
		return;
	}

	float width = m_pParList->GetItemWidth();
	float height = m_pParList->GetItemHeight();

	WPoint pt;
	float offset = width * (1.0f / 18.0f);
	pt.x = offset + pItem->pos.x;
	pt.y = height * 0.5f + pItem->pos.y - 7.0f;
	int start = m_bFront9 ? 0 : 9;
	int end = min(start + 9, GetHoles());

	int total = 0;
	for (int i = start; i < end; i++) {
		total += GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]).m_par;
		DrawDigit(pGDI, MakeStr("%d", GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]).m_par), pt, 0xFF9FF5B8, FrGDI::ALIGN_CENTER, false);
		pt.x = (float)(int)(width * (1.0f / 9.0f) + pt.x);
	}

	switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
	case 1:
	case 3:
	case 6:
		break;

	case 7:
		pt.x = (float)float2int(offset + pItem->pos.x + 473.0f);
		DrawDigit(pGDI, MakeStr("%d", total), pt, 0xFF9FF5B8, FrGDI::ALIGN_CENTER, false);
		break;

	default:
		pt.x = (float)(int)(offset + pItem->pos.x + 355.0f);
		DrawDigit(pGDI, MakeStr("%d", total), pt, 0xFF9FF5B8, FrGDI::ALIGN_CENTER, false);
		break;
	}
}

// scoreboard.cpp:4984
void FrScoreForm::OnPangListInit(PG_MSGARG param) {
	if (CSharedDoc::Instance()->m_roomType == 7) {
		m_pPangList = DYNAMIC_CAST(FrListBox, param);
		m_pPangList->AddItem(0);

		m_pPangList->SetItemSize(0x1d6, 0x16);
	}
}

// scoreboard.cpp:5009
void FrScoreForm::OnBlankPanel01Init(intptr_t param) {
	m_pBlankPanel01 = DYNAMIC_CAST(FrArea, param);
	if (m_pBlankPanel01) {
		m_pBlankPanel01->SetVisible(m_bShowBlankPanel);
	}
}

// scoreboard.cpp:5018
void FrScoreForm::OnBlankPanel01OwnerDraw(PG_MSGARG param) {
	if (m_pBlankPanel01) {
		m_pBlankPanel01->SetVisible(m_bShowBlankPanel);
	}
}

// scoreboard.cpp:5027
void FrScoreForm::OnBlankPanel02Init(intptr_t param) {
	m_pBlankPanel02 = DYNAMIC_CAST(FrArea, param);
	if (m_pBlankPanel02) {
		m_pBlankPanel02->SetVisible(m_bShowBlankPanel);
	}
}

// scoreboard.cpp:5036
void FrScoreForm::OnBlankPanel02OwnerDraw(PG_MSGARG param) {
	if (m_pBlankPanel02) {
		m_pBlankPanel02->SetVisible(m_bShowBlankPanel);
	}
}

// scoreboard.cpp:5045
void FrScoreForm::OnBlankPanel03Init(intptr_t param) {
	m_pBlankPanel03 = DYNAMIC_CAST(FrArea, param);
	if (m_pBlankPanel03) {
		m_pBlankPanel03->SetVisible(m_bShowBlankPanel);
	}
}

// scoreboard.cpp:5054
void FrScoreForm::OnBlankPanel03OwnerDraw(PG_MSGARG param) {
	if (m_pBlankPanel03) {
		m_pBlankPanel03->SetVisible(m_bShowBlankPanel);
	}
}

// scoreboard.cpp:5074
void FrScoreForm::OnPangListOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == 0) {
		return;
	}

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0) {
		return;
	}

	float width = m_pPangList->GetItemWidth();
	float height = m_pPangList->GetItemHeight();

	WPoint pt;
	float offset = width * (1.0f / 18.0f);
	pt.x = offset + pItem->pos.x;
	pt.y = height * 0.5f + pItem->pos.y - 7.0f;
	int start = m_bFront9 ? 0 : 9;
	int end = start + 9 < WSingleton<CSharedDoc>::Instance()->m_holes ? start + 9 : WSingleton<CSharedDoc>::Instance()->m_holes;

	for (int i = start; i < end; i++) {
		unsigned long color = GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]).m_redPang ? 0xFFFF0000 : 0xFF000000;

		pGDI->SetTextStyle(1);
		pGDI->SetTextColor(color, 0xFFFFFFFF);
		// The original passes only the low dword of the __int64 to %I64d.
		pGDI->PrintText(pt, 1, MakeStr("%I64d", (unsigned long)GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]).m_pang), 0);

		pt.x = (float)(int)(width * (1.0f / 9.0f) + pt.x);
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		pt.x = (float)float2int(offset + pItem->pos.x + 473.0f);

		pGDI->SetTextStyle(1);
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGDI->PrintText(pt, 1, MakeStr("%I64d", GOLFDOC()->m_totalPang), 0);
	}
}

// scoreboard.cpp:5142
void FrScoreForm::OnGolferListInit(intptr_t param) {
	m_pScoreList[0] = DYNAMIC_CAST(FrListBox, param);

	for (int i = 0; i < m_rowCount; i++) {
		m_pScoreList[0]->AddItem(m_rowData[i]);
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		m_pScoreList[0]->SetItemSize(103, 58);
		m_pScoreList[0]->SetRect(WRect(m_pScoreList[0]->GetRect().x - 1.0f, m_pScoreList[0]->GetRect().y + 22.0f, m_pScoreList[0]->GetRect().w, m_pScoreList[0]->GetRect().h));
	}
}

// scoreboard.cpp:5166
// Draws a row of the golfer list: the highlight, the team or player mark,
// the emblems and the names.
void FrScoreForm::OnGolferListOwnerDraw(intptr_t param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == 0)
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0)
		return;

	unsigned char index = (unsigned char)(intptr_t)pItem->pData;
	int row;
	for (row = 0; row < m_rowCount; row++) {
		if ((intptr_t)m_rowData[row] == index)
			break;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
		// team game: one row per team, the members' names below the mark
		sPlayerData* pMe;
		if (OnlinePlay())
			pMe = PLAYER(MyIndex());
		else
			pMe = PLAYER(GOLFDOC()->m_myPlayerIndex);

		if (index == pMe->m_attribute)
			pGDI->Box(WRect(pItem->pos.x, pItem->pos.y, (float)m_pScoreList[0]->GetItemWidth(), (float)m_pScoreList[0]->GetItemHeight()), 0x809B9DFF, 0, 0.0f);

		std::map<unsigned long, const Bitmap*>::iterator it;
		it = m_bitmaps.find(index);
		if (it != m_bitmaps.end()) {
			const Bitmap* pBmp = it->second;

			if (pBmp) {
				WRect rect(pItem->pos.x, pItem->pos.y + 3.0f, (float)pBmp->Width(), (float)pBmp->Height());
				pGDI->DrawTexture(pBmp, rect, 0xFFFFFFFF, 0);
			}
		}

		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGDI->SetTextStyle(0);

		WPoint pt(pItem->pos.x, pItem->pos.y + 28.0f);

		for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			float x = pt.x;
			float y = pt.y;

			if (GOLFDOC()->GetPlayer(i)->m_attribute == index) {
				if (WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_guildUID && !(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_identity & 0x14)) {
					const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_guildMark);

					if (pEmblem)
						pGDI->DrawTexture(pEmblem, WRect(x, y - 6.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
				}

				if (!WSingleton<CProjectG>::Instance()->HidePrivacy())
					g_pFresh->GetManager()->PrintText(WPoint(x + 27.0f, y), 0, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_name, 100.0f, 0xFFFFFFFF);

				pt.y += 16.0f;
			}
		}
		return;
	}

	if (OnlinePlay()) {
		unsigned long color;
		unsigned char roomType = WSingleton<CSharedDoc>::Instance()->m_roomType;
		if (roomType == 6) {
			color = 0x809B9DFF;
		} else if (roomType == 7) {
			if (GOLFDOC()->GetPlayer(index)->m_state == 3) {
				color = 0x80808080;
			} else {
				sPlayerData* pPlayer = GOLFDOC()->GetPlayer(index);
				if (pPlayer->m_uid == MyGuid(true))
					color = 0x809B9DFF;
				else
					goto names;
			}
		} else {
			sPlayerData* pPlayer = GOLFDOC()->GetPlayer(index);
			if (pPlayer->m_uid == MyGuid(true))
				color = 0x809B9DFF;
			else if (GOLFDOC()->GetPlayer(index)->m_state == 3)
				color = 0x80808080;
			else
				goto names;
		}

		pGDI->Box(WRect(pItem->pos.x, pItem->pos.y, (float)m_pScoreList[0]->GetItemWidth(), (float)m_pScoreList[0]->GetItemHeight()), color, 0, 0.0f);
	} else {
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
			if (GOLFDOC()->GetPlayer(index)->m_state == 3)
				pGDI->Box(WRect(pItem->pos.x, pItem->pos.y, (float)m_pScoreList[0]->GetItemWidth(), (float)m_pScoreList[0]->GetItemHeight()), 0x80808080, 0, 0.0f);
		}

		if (index == GOLFDOC()->m_myPlayerIndex)
			pGDI->Box(WRect(pItem->pos.x, pItem->pos.y, (float)m_pScoreList[0]->GetItemWidth(), (float)m_pScoreList[0]->GetItemHeight()), 0x809B9DFF, 0, 0.0f);
	}

names:
	WPoint pt(pItem->pos.x, pItem->pos.y + 36.0f);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
		// guild match: the team's trophy holder
		sGameResultInfo* pInfo = &WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(WSingleton<CSharedDoc>::Instance()->m_trophyUID[index])];

		if (pInfo->m_hasEmblem) {
			const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pInfo->m_emblemName);

			if (pEmblem)
				pGDI->DrawTexture(pEmblem, WRect(pt.x, pt.y - 6.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
		}

		pt.x += 27.0f;

		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGDI->SetTextStyle(0);

		if (WSingleton<CProjectG>::Instance()->HidePrivacy())
			g_pFresh->GetManager()->PrintText(pt, 0, pInfo->m_nickname, 100.0f, 0xFFFFFFFF);
	} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		if (GOLFDOC()->GetPlayer(index)->m_state == 3)
			row += 4;

		std::map<unsigned long, const Bitmap*>::iterator it;
		it = m_bitmaps.find(row);
		if (it != m_bitmaps.end()) {
			const Bitmap* pBmp = it->second;

			if (pBmp) {
				WRect rect(pItem->pos.x + 43.0f, pItem->pos.y + 12.0f, (float)pBmp->Width(), (float)pBmp->Height());
				pGDI->DrawTexture(pBmp, rect, 0xFFFFFFFF, 0);

				if (m_pMassListIcon && (bool)((WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_identity >> 7) & 1)) {
					rect.x = pItem->pos.x + 87.0f;
					rect.y += 4.0f;
					rect.w = (float)m_pMassListIcon->Width();
					rect.h = (float)m_pMassListIcon->Height();
					pGDI->DrawTexture(m_pMassListIcon, rect, 0xFFFFFFFF, 0);
				}
			}
		}

		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGDI->SetTextStyle(0);

		pt.x += 52.0f;

		if (!WSingleton<CProjectG>::Instance()->HidePrivacy())
			g_pFresh->GetManager()->PrintText(pt, 1, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_name, 100.0f, 0xFFFFFFFF);
	} else {
		std::map<unsigned long, const Bitmap*>::iterator it;
		it = m_bitmaps.find(row);
		if (it != m_bitmaps.end()) {
			const Bitmap* pBmp = it->second;

			if (pBmp) {
				WRect rect(pItem->pos.x + 43.0f, pItem->pos.y + 12.0f, (float)pBmp->Width(), (float)pBmp->Height());
				pGDI->DrawTexture(pBmp, rect, 0xFFFFFFFF, 0);

				if (m_pMassListIcon && (bool)((WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_identity >> 7) & 1)) {
					rect.x = pItem->pos.x + 87.0f;
					rect.y += 4.0f;
					rect.w = (float)m_pMassListIcon->Width();
					rect.h = (float)m_pMassListIcon->Height();
					pGDI->DrawTexture(m_pMassListIcon, rect, 0xFFFFFFFF, 0);
				}
			}
		}

		if (WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_guildUID && !(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_identity & 0x14)) {
			const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_guildMark);

			if (pEmblem)
				pGDI->DrawTexture(pEmblem, WRect(pt.x, pt.y - 6.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
		}

		pt.x += 27.0f;

		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGDI->SetTextStyle(0);

		if (!WSingleton<CProjectG>::Instance()->HidePrivacy())
			g_pFresh->GetManager()->PrintText(pt, 0, WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_name, 100.0f, 0xFFFFFFFF);
	}
}

// scoreboard.cpp:5468
void FrScoreForm::OnRecordListInit(intptr_t param) {
	m_pScoreList[1] = DYNAMIC_CAST(FrListBox, param);

	if (m_pScoreList[1]) {
		for (int i = 0; i < m_rowCount; i++) {
			m_pScoreList[1]->AddItem(m_rowData[i]);
		}
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		m_pScoreList[1]->SetItemSize(470, 58);
		m_pScoreList[1]->SetRect(WRect(m_pScoreList[1]->GetRect().x - 24.0f, m_pScoreList[1]->GetRect().y + 22.0f, m_pScoreList[1]->GetRect().w, m_pScoreList[1]->GetRect().h));
	}
}

// scoreboard.cpp:5495
void FrScoreForm::OnRecordListOwnerDraw(intptr_t param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == 0)
		return;

	int index = (int)pItem->pData;
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
		if (OnlinePlay()) {
			if (index == PLAYER(MyIndex())->m_attribute) {
				g_pFresh->GetManager()->GetGDI()->Box(WRect(pItem->pos.x, pItem->pos.y, (float)m_pScoreList[1]->GetItemWidth(), (float)m_pScoreList[1]->GetItemHeight()), 0x809B9DFF, 0, 0);
			}
		} else {
			if (index == PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute) {
				g_pFresh->GetManager()->GetGDI()->Box(WRect(pItem->pos.x, pItem->pos.y, (float)m_pScoreList[1]->GetItemWidth(), (float)m_pScoreList[1]->GetItemHeight()), 0x809B9DFF, 0, 0);
			}
		}
	} else if (OnlinePlay()) {
		unsigned long color;
		sPlayerData* pPlayer = GOLFDOC()->GetPlayer(index);
		if (pPlayer->m_uid == MyGuid(true))
			color = 0x809B9DFF;
		else if (GOLFDOC()->GetPlayer(index)->m_state == 3)
			color = 0x80808080;
		else
			goto draw;

		g_pFresh->GetManager()->GetGDI()->Box(WRect(pItem->pos.x, pItem->pos.y, (float)m_pScoreList[1]->GetItemWidth(), (float)m_pScoreList[1]->GetItemHeight()), color, 0, 0);
	} else {
		if (GOLFDOC()->GetPlayer(index)->m_state == 3) {
			g_pFresh->GetManager()->GetGDI()->Box(WRect(pItem->pos.x, pItem->pos.y, (float)m_pScoreList[1]->GetItemWidth(), (float)m_pScoreList[1]->GetItemHeight()), 0x80808080, 0, 0);
		}

		if (index == GOLFDOC()->m_myPlayerIndex) {
			g_pFresh->GetManager()->GetGDI()->Box(WRect(pItem->pos.x, pItem->pos.y, (float)m_pScoreList[1]->GetItemWidth(), (float)m_pScoreList[1]->GetItemHeight()), 0x809B9DFF, 0, 0);
		}
	}

draw:
	switch (m_scoreType) {
	case 0:
		DrawShot(pItem);
		break;
	case 1:
		DrawGraph(pItem);
		break;
	case 2:
		DrawPutt(pItem);
		break;
	case 3:
		DrawPang(pItem);
		break;
	case 4:
		DrawSkins(pItem);
		break;
	case 5:
		DrawVersus(pItem);
		break;
	}
}

// scoreboard.cpp:5618
void FrScoreForm::OnTotalListInit(intptr_t param) {
	m_pScoreList[2] = DYNAMIC_CAST(FrListBox, param);

	if (!m_pScoreList[2]) {
		return;
	}

	for (int i = 0; i < m_rowCount; i++) {
		m_pScoreList[2]->AddItem(m_rowData[i]);
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		m_pScoreList[2]->SetItemSize(63, 58);
		m_pScoreList[2]->SetRect(WRect(m_pScoreList[2]->GetRect().x + 104.0f, m_pScoreList[2]->GetRect().y + 22.0f, m_pScoreList[2]->GetRect().w, m_pScoreList[2]->GetRect().h));
	}
}

// scoreboard.cpp:5649
void FrScoreForm::OnTotalListOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == 0)
		return;

	float width = m_pScoreList[2]->GetItemWidth();
	float height = m_pScoreList[2]->GetItemHeight();

	int index = (int)pItem->pData;
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (GetGameType() == 1) {
		// the row of this client's team
		if (index == GOLFDOC()->GetPlayer(OnlinePlay() ? MyIndex() : GOLFDOC()->m_myPlayerIndex)->m_attribute) {
			WRect rect(pItem->pos.x, pItem->pos.y, (float)m_pScoreList[2]->GetItemWidth() * 0.5f, (float)m_pScoreList[2]->GetItemHeight());
			pGDI->Box(rect, 0x809B9DFF, 0, 0.0f);
		}
	} else if (OnlinePlay()) {
		unsigned long color = 0;
		sPlayerData* pPlayer = GOLFDOC()->GetPlayer(index);
		if (pPlayer->m_uid == MyGuid(true)) {
			color = 0x809B9DFF;
		} else if (GOLFDOC()->GetPlayer(index)->m_state == 3) {
			color = 0x80808080;
		}

		if (color != 0) {
			WRect rect(pItem->pos.x, pItem->pos.y, (float)m_pScoreList[2]->GetItemWidth(), (float)m_pScoreList[2]->GetItemHeight());
			if (WSingleton<CSharedDoc>::Instance()->m_roomType == 3 || WSingleton<CSharedDoc>::Instance()->m_roomType == 6)
				rect.w *= 0.5f;

			pGDI->Box(rect, color, 0, 0.0f);
		}
	} else {
		if (GOLFDOC()->GetPlayer(index)->m_state == 3) {
			WRect rect(pItem->pos.x, pItem->pos.y, (float)m_pScoreList[2]->GetItemWidth(), (float)m_pScoreList[2]->GetItemHeight());
			pGDI->Box(rect, 0x80808080, 0, 0.0f);
		}

		if (index == GOLFDOC()->m_myPlayerIndex) {
			WRect rect(pItem->pos.x, pItem->pos.y, (float)m_pScoreList[2]->GetItemWidth(), (float)m_pScoreList[2]->GetItemHeight());
			if (WSingleton<CSharedDoc>::Instance()->m_roomType == 3 || WSingleton<CSharedDoc>::Instance()->m_roomType == 6)
				rect.w *= 0.5f;

			pGDI->Box(rect, 0x809B9DFF, 0, 0.0f);
		}
	}

	WPoint pt;
	pt.y = height * 0.5f + pItem->pos.y - 6.0f;
	if (GetGameType() == 7)
		pt.x = width * 0.5f + pItem->pos.x;
	else
		pt.x = width * 0.25f + pItem->pos.x;

	switch (m_scoreType) {
	case 3:
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
			if (index == PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)
				DrawDigit(pGDI, MakeStr("%I64d", m_totalPang), pt, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
			else
				DrawDigit(pGDI, MakeStr("%d", (int)TEAM(index)->m_teamValue), pt, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
		} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
			DrawDigit(pGDI, MakeStr("%I64d", (int)GUILDMATCHUP(index)->m_pang), pt, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
		} else if (index == GOLFDOC()->m_myPlayerIndex) {
			DrawDigit(pGDI, MakeStr("%I64d", m_totalPang), pt, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
		} else {
			DrawDigit(pGDI, MakeStr("%d", (int)PLAYER(index)->m_ghostAward), pt, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
		}
		break;

	case 1:
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6 || WSingleton<CSharedDoc>::Instance()->m_roomType == 1
			|| WSingleton<CSharedDoc>::Instance()->m_roomType == 3 || WSingleton<CSharedDoc>::Instance()->m_roomType == 7)
			break;
		// fall through
	case 0:
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6)
			DrawDigit(pGDI, "?", pt, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
		else
			DrawDigit(pGDI, MakeStr("%d", PLAYER(index)->m_totalScore), pt, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
		break;

	case 5:
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6)
			DrawDigit(pGDI, MakeStr("%d", GUILDMATCHUP(index)->m_uniteGuildIndex), pt, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
		else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1)
			DrawDigit(pGDI, MakeStr("%d", TEAM(index)->m_resetCount), pt, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
		else
			DrawDigit(pGDI, MakeStr("%d", PLAYER(index)->m_totalScore), pt, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
		break;

	case 2:
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6 || WSingleton<CSharedDoc>::Instance()->m_roomType == 1
			|| WSingleton<CSharedDoc>::Instance()->m_roomType == 3 || WSingleton<CSharedDoc>::Instance()->m_roomType == 7)
			break;
		{
			int total = 0;
			for (int i = 0; i < (m_bFront9 ? 9 : 18); i++)
				total += PLAYER(index)->m_holeIn[i];

			DrawDigit(pGDI, MakeStr("%d", total), pt, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
		}
		break;

	case 4:
		pGDI->SetTextStyle(1);
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGDI->PrintText(pt, 1, MakeStr("%I64d", PLAYER(index)->m_roundPang), 0);
		break;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 0 || IsMassGame()) {
		pt.x = width * 0.75f + pItem->pos.x;
		DrawDigit(pGDI, MakeStr(GOLFDOC()->GetPlayer(index)->m_ghostResult == 0 ? "%d" : "%+d", GOLFDOC()->GetPlayer(index)->m_ghostResult), pt, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
	}
}

// scoreboard.cpp:5846
bool FrScoreForm::OnInit(void) {
	Enable(m_state != STATE_HIDDEN);
	SetVisible(m_state != STATE_HIDDEN);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		WRect rect = m_rect;
		rect.w += 36.0f;
		SetRect(rect);
	}
	SortLists(false);
	LoadBitmaps();

	return true;
}

// scoreboard.cpp:5860
void FrScoreForm::OnProc(const float dt) {
	if (m_state == STATE_HIDDEN) {
		return;
	}
	if (m_typeTime < 5.0f) {
		m_typeTime += dt;
	}
}

// scoreboard.cpp:5869
void FrScoreForm::OnDraw(void) {
	// scoreboard.cpp:5876
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		return;
	}

	// scoreboard.cpp:5882
	WPoint p1, p2;
	p1.x = p2.x = m_rect.x + 129.0f;
	p1.y = m_rect.y + 116.0f;
	p2.y = m_rowCount * 58.0f + p1.y;

	// scoreboard.cpp:5887
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		p1.x = p2.x - 24.0f;
		p2.x = p1.x;
		p1.y += 22.0f;
		p2.y += 22.0f;
	}

	// scoreboard.cpp:5895
	for (int i = 0; i < 10; i++) {
		g_view->DrawLine2D(p1, p2, 0xFF000000, 0xFF000000, 0);
		p1.x += 38.0f;
		p2.x += 38.0f;

		// scoreboard.cpp:5902
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
			p1.x += 9.0f;
			p2.x += 9.0f;
		}
	}

	// scoreboard.cpp:5909
	p1.x += 27.0f;
	p2.x += 27.0f;

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		p1.x -= 13.0f;
		p2.x -= 13.0f;
	}

	// scoreboard.cpp:5919
	g_view->DrawLine2D(p1, p2, 0xFF000000, 0xFF000000, 0);

	// scoreboard.cpp:5922
	p1.y = p2.y = m_rect.y + 174.0f;

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		p1.y = p2.y + 22.0f;
		p2.y = p1.y;
	}

	// scoreboard.cpp:5930
	for (int i = 0; i < m_rowCount - 1; i++) {
		p1.x = m_rect.x + 3.0f;
		p2.x = m_rect.x + 28.0f;
		g_view->DrawLine2D(p1, p2, 0, 0xFF000000, 0);

		// scoreboard.cpp:5937
		p1.x = m_rect.x + m_rect.w - 36.0f;
		p2.x = m_rect.x + m_rect.w - 11.0f;
		g_view->DrawLine2D(p1, p2, 0xFF000000, 0, 0);

		// scoreboard.cpp:5941
		p1.x = m_rect.x + 28.0f;
		p2.x = m_rect.x + m_rect.w - 36.0f;
		g_view->DrawLine2D(p1, p2, 0xFF000000, 0xFF000000, 0);

		// scoreboard.cpp:5945
		p1.y += 58.0f;
		p2.y += 58.0f;
	}
}

// scoreboard.cpp:5951
void FrScoreForm::SetFrontBackBtn(void) {
	m_typeTime = 0;
	if (WSingleton<CSharedDoc>::Instance()->m_holes == 18) {
		if (m_pFront9Btn) {
			m_pFront9Btn->SetVisible(true);
			m_pFront9Btn->SetStatus(m_bFront9 ? FrButton::PRESSED : FrButton::NORMAL);
		}

		if (m_pBack9Btn) {
			m_pBack9Btn->SetVisible(true);
			m_pBack9Btn->SetStatus(m_bFront9 ? FrButton::NORMAL : FrButton::PRESSED);
		}
	} else {
		if (m_pFront9Btn) {
			m_pFront9Btn->SetVisible(false);
		}
		if (m_pBack9Btn) {
			m_pBack9Btn->SetVisible(false);
		}
	}
}

// scoreboard.cpp:5983
void FrScoreForm::SetTypeBtn(void) {
	m_typeTime = 0.0f;

	for (int i = 0; i < 6; i++) {
		if (m_pTypeBtn[i]) {
			m_pTypeBtn[i]->SetStatus(m_scoreType == i ? FrButton::PRESSED : FrButton::NORMAL);
		}
	}
}

// scoreboard.cpp:6004
void FrScoreForm::DrawShot(FrListItem* pItem) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0)
		return;

	FrListBox* pList = m_pScoreList[1];

	float width = pList->GetItemWidth();
	float height = pList->GetItemHeight();
	int index = (int)pItem->pData;
	int start = m_bFront9 ? 0 : 9;
	// the windef.h min macro, which evaluates its arguments twice
	int end = min(min(1 - (int)(m_typeTime * -20.0f), 9) + start, GetHoles());
	WPoint pos;
	pos.x = (float)(int)(width * (1.0f / 18.0f) + pItem->pos.x);
	pos.y = (float)(int)(height * 0.5f + pItem->pos.y - 6.0f);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
		// the trophy players' shots, from the game's results
		for (int i = start; i < end; i++) {
			if (GUILDMATCHUP(index)->m_holeShots[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] == 0
				&& i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
				break;

			if (i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1
				&& GUILDMATCHUP(index)->m_state != 1
				&& (!(GOLFDOC()->GameMode() & 0x22000) || GOLFDOC()->IsNetGameEnd())) {
				DrawDigit(pGDI, "?", pos, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
			} else if (GUILDMATCHUP(index)->m_holeShots[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] == 0
				|| (GUILDMATCHUP(index)->m_uid != MyGuid(false)
					&& i > GetHoleIndex(GOLFDOC()->m_currentHole) - 1)) {
				DrawDigit(pGDI, "?", pos, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
			} else {
				int par = GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]).m_par;
				int shots = GUILDMATCHUP(index)->m_holeShots[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1];
				int score = shots - par;
				score = score < -3 ? -3 : (score > 3 ? 3 : score);

				unsigned long color;
				switch (score) {
				case -4:
				case -3:
				case -2:
					color = 0xFFFF4F4F;
					break;
				case -1:
					color = 0xFFFFBD42;
					break;
				case 0:
					color = 0xFFFFFFFF;
					break;
				case 1:
					color = 0xFFB9B9B9;
					break;
				default:
					color = 0xFF6F6F71;
					break;
				}

				DrawDigit(pGDI, MakeStr("%d", shots), pos, color, FrGDI::ALIGN_CENTER, false);
			}

			pos.x = (float)(int)(width * (1.0f / 9.0f) + pos.x);
		}
	} else {
		for (int i = start; i < end; i++) {
			if (PLAYER(index)->m_holes[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] == (unsigned char)0
				&& i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
				break;

			if (i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1 && PLAYER(index)->m_state != 1
				&& (!(GOLFDOC()->GameMode() & 0x22000) || GOLFDOC()->IsNetGameEnd())) {
				DrawDigit(pGDI, "-", pos, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
			} else if (PLAYER(index)->m_holes[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] == (unsigned char)0) {
				DrawDigit(pGDI, "-", pos, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
			} else {
				int par = GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]).m_par;
				int shots = PLAYER(index)->m_holes[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1];
				int score = shots - par;
				score = score < -3 ? -3 : (score > 3 ? 3 : score);

				unsigned long color;
				switch (score) {
				case -4:
				case -3:
				case -2:
					color = 0xFFFF4F4F;
					break;
				case -1:
					color = 0xFFFFBD42;
					break;
				case 0:
					color = 0xFFFFFFFF;
					break;
				case 1:
					color = 0xFFB9B9B9;
					break;
				default:
					color = 0xFF6F6F71;
					break;
				}

				DrawDigit(pGDI, MakeStr("%d", shots), pos, color, FrGDI::ALIGN_CENTER, false);
			}

			pos.x = (float)(int)(width * (1.0f / 9.0f) + pos.x);
		}
	}
}

// scoreboard.cpp:6140
void FrScoreForm::DrawVersus(FrListItem* pItem) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0)
		return;

	FrListBox* pList = m_pScoreList[1];

	float width = pList->GetItemWidth();
	float height = pList->GetItemHeight();
	int index = (int)pItem->pData;
	int start = m_bFront9 ? 0 : 9;
	// the windef.h min macro, which evaluates its arguments twice
	int end = min(min(1 - (int)(m_typeTime * -20.0f), 9) + start, GetHoles());
	WPoint pos;
	pos.x = (float)(int)(width * (1.0f / 18.0f) + pItem->pos.x);
	pos.y = (float)(int)(height * 0.5f + pItem->pos.y - 6.0f);

	for (int i = start; i < end; i++) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
			if (TEAM(index)->m_holes[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] == (unsigned char)0
				&& i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
				break;
		} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
			if (GUILDMATCHUP(index)->m_holeShots[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] == 0
				&& i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
				break;
		} else {
			if (PLAYER(index)->m_holes[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] == (unsigned char)0
				&& i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
				break;
		}

		if (i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1 && (GOLFDOC()->GameMode() & ~0x22000)) {
			DrawDigit(pGDI, "?", pos, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
		} else {
			WRect rect;
			const Bitmap* pBmp = 0;
			switch (GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]).m_mapState) {
			case 0:
			case 1:
				pBmp = (&m_pWin)[index != GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]).m_mapState];
				break;
			case 2:
				pBmp = m_pDraw;
				break;
			case 3:
				pBmp = m_pQuestion;
				break;
			}

			if (pBmp) {
				rect.x = (float)(int)(pos.x - (float)pBmp->Width() * 0.5f);
				rect.y = pos.y;
				rect.w = (float)pBmp->Width();
				rect.h = (float)pBmp->Height();
				pGDI->DrawTexture(pBmp, rect, 0xFFFFFFFF, 0);
			}
		}

		pos.x = (float)(int)(width * (1.0f / 9.0f) + pos.x);
	}
}

// scoreboard.cpp:6217
void FrScoreForm::DrawGraph(FrListItem* pItem) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0)
		return;

	FrListBox* pList = m_pScoreList[1];

	float width = pList->GetItemWidth();
	float height = pList->GetItemHeight();
	int index = (int)pItem->pData;
	float y = height * 0.5f + pItem->pos.y;
	float step = (height - 5.0f) * (1.0f / 6.0f);
	int start = m_bFront9 ? 0 : 9;
	// the windef.h min macro, which evaluates its arguments twice
	int end = min(min(1 - (int)(m_typeTime * -20.0f), 9) + start, GetHoles());

	// the par line
	WPoint p1, p2;
	p1.x = (float)(int)pItem->pos.x;
	p2.x = (float)(int)((float)(end - start) * width * (1.0f / 9.0f) + p1.x);
	p1.y = p2.y = y;
	g_view->DrawLine2D(p1, p2, 0xFF95FFB3, 0xFF95FFB3, 0);

	// the score of each hole against its par
	int i = start;
	int diff;
	int score;
	if (PLAYER(index)->m_holes[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] > (unsigned char)0
		&& (PLAYER(index)->m_state != 0 || i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1 || GOLFDOC()->GameMode() == 0x2000)) {
		int par = GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]).m_par;
		diff = (int)PLAYER(index)->m_holes[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] - par;
		score = diff < -3 ? -3 : (diff > 3 ? 3 : diff);
		unsigned long color = GetScoreColor(score);
		p1.x = (float)(int)(width * (1.0f / 18.0f) + pItem->pos.x);
		p1.y = (float)(int)((float)score * step + y);

		for (i++; i < end; i++) {
			if (PLAYER(index)->m_holes[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] == (unsigned char)0)
				break;
			if (PLAYER(index)->m_state == 0 && i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1 && GOLFDOC()->GameMode() != 0x2000)
				break;

			int par = GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]).m_par;
			diff = (int)PLAYER(index)->m_holes[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] - par;
			score = diff < -3 ? -3 : (diff > 3 ? 3 : diff);
			unsigned long nextColor = GetScoreColor(score);

			p2.x = (float)(int)(width * (1.0f / 9.0f) + p1.x);
			p2.y = (float)(int)((float)score * step + y);
			g_view->DrawLine2D(p1, p2, color, nextColor, 0);

			color = nextColor;
			p1 = p2;
		}
	}

	// a point on each hole
	if (m_pPoint) {
		WRect rect;
		rect.x = (float)(int)(width * (1.0f / 18.0f) + pItem->pos.x - 2.0f);
		rect.y = 0.0f;
		rect.w = 4.0f;
		rect.h = 4.0f;

		for (i = start; i < end; i++) {
			if (PLAYER(index)->m_holes[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] == (unsigned char)0)
				break;
			if (PLAYER(index)->m_state == 0 && i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1 && GOLFDOC()->GameMode() != 0x2000)
				break;

			int par = GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]).m_par;
			diff = (int)PLAYER(index)->m_holes[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] - par;
			score = diff < -3 ? -3 : (diff > 3 ? 3 : diff);

			rect.y = (float)(int)((float)score * step + y - 2.0f);
			pGDI->DrawTexture(m_pPoint, rect, 0xFF696969, 0);
			rect.x = (float)(int)(width * (1.0f / 9.0f) + rect.x);
		}
	}
}

// scoreboard.cpp:6312
void FrScoreForm::DrawPutt(FrListItem* pItem) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0)
		return;

	FrListBox* pList = m_pScoreList[1];

	float width = pList->GetItemWidth();
	float height = pList->GetItemHeight();
	int index = (int)pItem->pData;
	int start = m_bFront9 ? 0 : 9;
	// the windef.h min macro, which evaluates its arguments twice
	int end = min(min(1 - (int)(m_typeTime * -20.0f), 9) + start, GetHoles());
	WPoint pos;
	pos.x = (float)(int)(width * (1.0f / 18.0f) + pItem->pos.x);
	pos.y = (float)(int)(height * 0.5f + pItem->pos.y - 6.0f);

	for (int i = start; i < end; i++) {
		if (PLAYER(index)->m_holes[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] == (unsigned char)0
			&& i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
			break;

		int putts = PLAYER(index)->m_holeIn[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1];

		if (i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1 && PLAYER(index)->m_state != 1
			&& (!(GOLFDOC()->GameMode() & 0x22000) || GOLFDOC()->IsNetGameEnd())) {
			DrawDigit(pGDI, "-", pos, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
		} else if (PLAYER(index)->m_holes[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] == (unsigned char)0) {
			DrawDigit(pGDI, "-", pos, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
		} else {
			DrawDigit(pGDI, MakeStr("%d", putts), pos, 0xFF40C967, FrGDI::ALIGN_CENTER, false);
		}

		pos.x = (float)(int)(width * (1.0f / 9.0f) + pos.x);
	}
}

// scoreboard.cpp:6353
void FrScoreForm::DrawPang(FrListItem* pItem) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0)
		return;

	FrListBox* pList = m_pScoreList[1];

	float width = pList->GetItemWidth();
	float height = pList->GetItemHeight();
	int index = (int)pItem->pData;
	int start = m_bFront9 ? 0 : 9;
	// the windef.h min macro, which evaluates its arguments twice
	int end = min(min(1 - (int)(m_typeTime * -20.0f), 9) + start, GetHoles());
	WPoint pos;
	pos.x = (float)(int)(width * (1.0f / 18.0f) + pItem->pos.x);
	pos.y = (float)(int)(height * 0.5f + pItem->pos.y - 6.0f);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
		// a team game: the team's record
		for (int i = start; i < end; i++) {
			if (TEAM(index)->m_holes[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] == (unsigned char)0
				&& i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
				break;

			__int64 pang;
			if (i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1 && index == PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)
				pang = m_holePang;
			else
				pang = (int)TEAM(index)->m_holeRecord[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1];

			if (TEAM(index)->m_holes[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] == (unsigned char)0)
				DrawDigit(pGDI, "-", pos, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
			else
				DrawDigit(pGDI, MakeStr("%I64d", pang), pos, 0xFFE98919, FrGDI::ALIGN_CENTER, true);

			pos.x = (float)(int)(width * (1.0f / 9.0f) + pos.x);
		}
	} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
		// the trophy players, from the game's results
		for (int i = start; i < end; i++) {
			if (WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(WSingleton<CSharedDoc>::Instance()->m_trophyUID[(unsigned char)index])].m_holeShots[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] == 0
				&& i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
				break;

			__int64 pang;
			if (i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1 && (GOLFDOC()->GameMode() & ~0x22000))
				pang = m_holePang;
			else
				pang = WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(WSingleton<CSharedDoc>::Instance()->m_trophyUID[(unsigned char)index])].m_holePang[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1];

			if (WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(WSingleton<CSharedDoc>::Instance()->m_trophyUID[(unsigned char)index])].m_holeShots[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] == 0
				|| (WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(WSingleton<CSharedDoc>::Instance()->m_trophyUID[(unsigned char)index])].m_uid != MyGuid(false)
					&& i > GetHoleIndex(GOLFDOC()->m_currentHole) - 1))
				DrawDigit(pGDI, "?", pos, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
			else
				DrawDigit(pGDI, MakeStr("%d", pang), pos, 0xFFE98919, FrGDI::ALIGN_CENTER, true);

			pos.x = (float)(int)(width * (1.0f / 9.0f) + pos.x);
		}
	} else {
		for (int i = start; i < end; i++) {
			if (PLAYER(index)->m_holes[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] == (unsigned char)0
				&& i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
				break;

			__int64 pang;
			if (i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1 && index == GOLFDOC()->m_myPlayerIndex)
				pang = m_holePang;
			else
				pang = (int)PLAYER(index)->m_holeRecord[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1];

			if (PLAYER(index)->m_holes[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] == (unsigned char)0)
				DrawDigit(pGDI, "-", pos, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
			else
				DrawDigit(pGDI, MakeStr("%d", pang), pos, 0xFFE98919, FrGDI::ALIGN_CENTER, true);

			pos.x = (float)(int)(width * (1.0f / 9.0f) + pos.x);
		}
	}
}

// scoreboard.cpp:6444
void FrScoreForm::DrawSkins(FrListItem* pItem) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0)
		return;

	FrListBox* pList = m_pScoreList[1];

	float width = pList->GetItemWidth();
	float height = pList->GetItemHeight();
	int index = (int)pItem->pData;
	int start = m_bFront9 ? 0 : 9;
	// the windef.h min macro, which evaluates its arguments twice
	int end = min(min(1 - (int)(m_typeTime * -20.0f), 9) + start, GetHoles());
	WPoint pos;
	pos.x = (float)(int)(width * (1.0f / 18.0f) + pItem->pos.x);
	pos.y = (float)(int)(height * 0.5f + pItem->pos.y - 6.0f);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType != 7)
		return;

	for (int i = start; i < end; i++) {
		if (PLAYER(index)->m_holes[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] == (unsigned char)0
			&& i != GetHoleIndex(GOLFDOC()->m_currentHole) - 1)
			break;

		bool bDash = false;
		if (i == GetHoleIndex(GOLFDOC()->m_currentHole) - 1) {
			if (PLAYER(index)->m_state == 3) {
				if (PLAYER(index)->m_holes[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1] == (unsigned char)0)
					bDash = true;
			} else if (!GOLFDOC()->m_approachMode && m_state != 3) {
				bDash = true;
			}
		}

		if (!bDash) {
			int skins = PLAYER(index)->m_holeRecord2[WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] - 1];

			unsigned long color;
			if (PLAYER(index)->m_state == 3) {
				color = 0xFF282828;
			} else if (GOLFDOC()->GetHoleData(WSingleton<CSharedDoc>::Instance()->m_holeOrder[i]).m_redPang && skins > 0) {
				color = 0xFFFF0000;
			} else {
				color = 0xFF000000;
			}

			pGDI->SetTextStyle(1);
			pGDI->SetTextColor(color, 0xFFFFFFFF);

			if (skins == -1) {
				WRect rect;
				float arrowWidth = (float)m_pArrow->Width();
				rect.x = pos.x - arrowWidth * 0.5f;
				rect.y = pos.y;
				rect.w = (float)m_pArrow->Width();
				rect.h = (float)m_pArrow->Height();

				pGDI->DrawTexture(m_pArrow, rect, 0xFFFFFFFF, 0);
			} else {
				pGDI->PrintText(pos, 1, MakeStr("%d", skins), 0);
			}
		} else {
			DrawDigit(pGDI, "-", pos, 0xFFFFFFFF, FrGDI::ALIGN_CENTER, false);
		}

		pos.x = (float)(int)(width * (1.0f / 9.0f) + pos.x);
	}
}

// scoreboard.cpp:6526
void FrScoreForm::DrawDigit(FrGraphicInterface* pGDI, char* text, WPoint pos, unsigned long color, FrGDI::eTextAlign align, bool bShadow) {
	int len = (int)strlen(text);
	const Bitmap* bitmaps[16];
	int totalWidth = 0;

	if (align != FrGDI::ALIGN_LEFT) {
		totalWidth = 0;
		for (int i = 0; i < len; i++) {
			switch (text[i]) {
			case '+':
				bitmaps[i] = m_pPlus;
				break;
			case '-':
				bitmaps[i] = m_pMinus;
				break;
			case '?':
				bitmaps[i] = m_pQuestion;
				break;
			default:
				bitmaps[i] = m_pDigit[text[i] - '0'];
				break;
			}
			totalWidth += bitmaps[i]->Width() - (bShadow ? (len < 4 ? 0 : 2) : 0);
		}
	}

	WRect rc;
	switch (align) {
	case FrGDI::ALIGN_LEFT:
		rc.x = (float)(int)pos.x;
		break;
	case FrGDI::ALIGN_CENTER:
		rc.x = (float)(int)(pos.x - totalWidth * 0.5f);
		break;
	case FrGDI::ALIGN_RIGHT:
		rc.x = (float)(int)(pos.x - totalWidth);
		break;
	}

	rc.y = pos.y;
	for (int i = 0; i < len; i++) {
		rc.w = (float)bitmaps[i]->Width();
		rc.h = (float)bitmaps[i]->Height();

		pGDI->DrawTexture(bitmaps[i], rc, color, 0);
		rc.x += rc.w - (bShadow ? (len < 4 ? 0 : 2) : 0);
	}
}

// scoreboard.cpp:6582
unsigned long FrScoreForm::GetScoreColor(int score) {
	switch (score) {
	case -4:
	case -3:
	case -2:
		return 0xffff0000;
	case -1:
		return 0xffffbb4f;
	case 0:
		return 0xffffffff;
	case 1:
		return 0xff82c3ff;
	default:
		return 0xff0036ff;
	}
}

// scoreboard.cpp:6601
void FrScoreForm::OnTotalInit(intptr_t param) {
	FrArea* pArea = DYNAMIC_CAST(FrArea, param);
	if (!pArea) {
		return;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1 || WSingleton<CSharedDoc>::Instance()->m_roomType == 3 || WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
		pArea->SetBgImg("header04");
	} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		pArea->SetBgImg("header03_skins");
		pArea->SetRect(pArea->GetRect() + WRect(-52.0f, 0.0f, 0.0f, 0.0f));
	} else {
		pArea->SetBgImg("header03");
	}
}

// scoreboard.cpp:6621
void FrScoreForm::OnPPLInit(intptr_t param) {
	FrArea* pArea = DYNAMIC_CAST(FrArea, param);
	m_pPPLArea = pArea;
	if (!pArea) {
		return;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1 || WSingleton<CSharedDoc>::Instance()->m_roomType == 6 || WSingleton<CSharedDoc>::Instance()->m_roomType == 3) {
		m_pPPLArea->SetRect(WRect(m_pPPLArea->GetRect().x - 65.0f, m_pPPLArea->GetRect().y, m_pPPLArea->GetRect().w, m_pPPLArea->GetRect().h));
	} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		pArea->SetRect(pArea->GetRect() + WRect(43.0f, 0.0f, 0.0f, 0.0f));
	}

	m_pPPLArea->SetVisible(false);
	m_bPPL = false;
}

// scoreboard.cpp:6641
void FrScoreForm::SetPPL(const char* name) {
	if (m_pPPLArea) {
		const Bitmap* pOldBg = m_pPPLArea->GetBgImg();
		if (m_pPPLArea->SetBgImg(name)) {
			m_bPPL = true;
			m_pPPLArea->SetVisible(true);
		} else {
			m_bPPL = false;
			m_pPPLArea->SetBgImg(pOldBg);
			m_pPPLArea->SetVisible(false);
		}
	}
}

// scoreboard.cpp:6663
void FrScoreForm::OnBoard1Init(intptr_t param) {
	FrArea* pArea = DYNAMIC_CAST(FrArea, param);

	if (pArea && WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		pArea->SetBgImg("header01_skins");
		WRect rect = pArea->GetRect(); rect.x -= 1.0f;
		pArea->SetRect(rect);
	} else {
		pArea->SetBgImg("header01");
	}
}

// scoreboard.cpp:6676
void FrScoreForm::OnBoard2Init(intptr_t param) {
	FrArea* pArea = DYNAMIC_CAST(FrArea, param);

	if (pArea && WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		pArea->SetBgImg("header02_skins");
		WRect rect = pArea->GetRect(); rect.x -= 32.0f;
		pArea->SetRect(rect);
	} else {
		pArea->SetBgImg("header02");
	}
}

// scoreboard.cpp:6689
void FrScoreForm::OnDrawLinesInit(intptr_t param) {
	FrArea* pArea = DYNAMIC_CAST(FrArea, param);

	if (WSingleton<CSharedDoc>::Instance()->m_roomType != 7) {
		pArea->SetVisible(false);
	}
}

// scoreboard.cpp:6697
void FrScoreForm::OnDrawLinesOwnerDraw(PG_MSGARG param) {
	WPoint a, b;

	// The vertical lines between the hole columns
	a.x = b.x = m_rect.x + 129.0f;
	a.y = m_rect.y + 126.0f;
	b.y = a.y + m_rowCount * 58.0f;

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		a.x -= 22.0f;
		b.x -= 22.0f;
		a.y += 22.0f;
		b.y += 22.0f;
	}

	for (int i = 0; i < 10; i++) {
		g_view->DrawLine2D(a, b, 0xFF9C9C9C, 0xFF9C9C9C, 0);
		a.x += 38.0f;
		b.x += 38.0f;

		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
			a.x += 14.0f;
			b.x += 14.0f;
		}
	}

	a.x += 27.0f;
	b.x += 27.0f;

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		a.x -= 13.0f;
		b.x -= 13.0f;
	}

	// The horizontal lines between the rows
	a.y = b.y = m_rect.y + 184.0f;

	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		a.y += 22.0f;
		b.y += 22.0f;
	}

	for (int row = 0; row < m_rowCount - 1; row++) {
		a.x = m_rect.x + 3.0f;
		b.x = m_rect.x + 28.0f;
		g_view->DrawLine2D(a, b, 0xFF9C9C9C, 0xFF9C9C9C, 0);

		a.x = m_rect.x + m_rect.w - 36.0f;
		b.x = m_rect.x + m_rect.w - 11.0f;
		g_view->DrawLine2D(a, b, 0xFF9C9C9C, 0xFF9C9C9C, 0);

		a.x = m_rect.x + 28.0f;
		b.x = m_rect.x + m_rect.w - 36.0f;
		g_view->DrawLine2D(a, b, 0xFF9C9C9C, 0xFF9C9C9C, 0);

		a.y += 58.0f;
		b.y += 58.0f;
	}
}

// scoreboard.cpp:6774
void FrScoreForm::OnCancelBtnInit(intptr_t param) {
	m_pCancelBtn = DYNAMIC_CAST(FrButton, param);
	m_pCancelBtn->SetVisible(false);
}

// scoreboard.cpp:6780
void FrScoreForm::OnCancelBtnUp(void) {
	Close(FrOK, true);
}

// scoreboard.cpp:6785
void FrScoreForm::OnCloseBtnInit(intptr_t param) {
	if (!m_pCloseBtn) {
		m_pCloseBtn = DYNAMIC_CAST(FrButton, param);
	}

	if (m_pCloseBtn) {
		m_pCloseBtn->SetVisible(false);
	}
}

// scoreboard.cpp:6827
void FrScoreForm::OnCloseBtnUp(void) {
	SetVisible(false);
}

// scoreboard.cpp:6833
void FrScoreForm::SetDesc(void) {
	char text[256];
	unsigned long leader = 0xFFFFFFFF;

	switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
	case 0:
		for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			if (GOLFDOC()->GetPlayer(i)->m_state == 3)
				continue;

			if (GOLFDOC()->GetPlayer(i)->m_joinState != 1)
				continue;

			if (leader == 0xFFFFFFFF) {
				leader = GOLFDOC()->GetPlayer(i)->m_uid;

				__int64 pang = 0;
				unsigned char holeCount = GetHoleIndex(GOLFDOC()->m_currentHole);
				for (unsigned char j = 0; j < holeCount; j++) {
					if (j == holeCount - 1 && i == GOLFDOC()->m_myPlayerIndex)
						pang += m_holePang;
					else
						pang += (int)PLAYER(i)->m_holeRecord[WSingleton<CSharedDoc>::Instance()->m_holeOrder[j] - 1];
				}

				sprintf(text, "\xC7\xF6\xC0\xE7 %s\xB4\xD4\xC0\xCC \xBD\xBA\xC4\xDA\xBE\xEE %d, \xC6\xCE %I64d",
					WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_name, PLAYER(i)->m_ghostResult, pang);
				ConditionalStrCat(text, "\xB7\xCE \xBC\xB1\xB5\xCE\xC0\xD4\xB4\xCF\xB4\xD9.");
			} else {
				sprintf(text, "\xC7\xF6\xC0\xE7 \xC7\xC3\xB7\xB9\xC0\xCC\xBE\xEE\xB5\xE9\xC0\xC7 \xBD\xBA\xC4\xDA\xBE\xEE\xB8\xA6 \xBA\xB8\xBF\xA9\xC1\xD6\xB4\xC2 \xC4\xAB\xB5\xE5\xC0\xD4\xB4\xCF\xB4\xD9.");
				break;
			}
		}

		if (leader == 0xFFFFFFFF)
			sprintf(text, "\xC7\xF6\xC0\xE7 \xC7\xC3\xB7\xB9\xC0\xCC\xBE\xEE\xB5\xE9\xC0\xC7 \xBD\xBA\xC4\xDA\xBE\xEE\xB8\xA6 \xBA\xB8\xBF\xA9\xC1\xD6\xB4\xC2 \xC4\xAB\xB5\xE5\xC0\xD4\xB4\xCF\xB4\xD9.");
		break;

	case 3: {
		sPlayerData *player0 = GOLFDOC()->GetPlayer(0);
		if ((int)player0->m_ghostResult > (int)GOLFDOC()->GetPlayer(1)->m_ghostResult) {
			sprintf(text, "\xC7\xF6\xC0\xE7 %s\xB4\xD4\xC0\xCC \xBD\xBA\xC4\xDA\xBE\xEE %d vs %d",
				WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_name, PLAYER(0)->m_ghostResult, PLAYER(1)->m_ghostResult);
			ConditionalStrCat(text, "\xB7\xCE \xBC\xB1\xB5\xCE\xC0\xD4\xB4\xCF\xB4\xD9.");
		} else if (player0 = GOLFDOC()->GetPlayer(0), (int)player0->m_ghostResult < (int)GOLFDOC()->GetPlayer(1)->m_ghostResult) {
			sprintf(text, "\xC7\xF6\xC0\xE7 %s\xB4\xD4\xC0\xCC \xBD\xBA\xC4\xDA\xBE\xEE %d vs %d",
				WSingleton<CSharedDoc>::Instance()->m_roomPlayers[1].m_name, PLAYER(1)->m_ghostResult, PLAYER(0)->m_ghostResult);
			ConditionalStrCat(text, "\xB7\xCE \xBC\xB1\xB5\xCE\xC0\xD4\xB4\xCF\xB4\xD9.");
		} else {
			__int64 pang[2] = { 0, 0 };
			unsigned char holeCount = GetHoleIndex(GOLFDOC()->m_currentHole);

			for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
				for (unsigned char j = 0; j < holeCount; j++) {
					if (j == holeCount - 1 && i == GOLFDOC()->m_myPlayerIndex) {
						pang[i] += m_holePang;
					} else {
						pang[i] += (int)PLAYER(i)->m_holeRecord[WSingleton<CSharedDoc>::Instance()->m_holeOrder[j] - 1];
					}
				}
			}

			if (pang[0] > pang[1]) {
				sprintf(text, "\xC7\xF6\xC0\xE7 \xBD\xBA\xC4\xDA\xBE\xEE\xB4\xC2 \xB5\xBF\xC1\xA1\xC0\xCC\xC1\xF6\xB8\xB8, %s\xB4\xD4\xC0\xCC \xC6\xCE %d",
					WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0].m_name, pang[0]);
				ConditionalStrCat(text, "\xB7\xCE \xC0\xCC\xB1\xE2\xB0\xED \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.");
			} else if (pang[0] < pang[1]) {
				sprintf(text, "\xC7\xF6\xC0\xE7 \xBD\xBA\xC4\xDA\xBE\xEE\xB4\xC2 \xB5\xBF\xC1\xA1\xC0\xCC\xC1\xF6\xB8\xB8, %s\xB4\xD4\xC0\xCC \xC6\xCE %d",
					WSingleton<CSharedDoc>::Instance()->m_roomPlayers[1].m_name, pang[1]);
				ConditionalStrCat(text, "\xB7\xCE \xC0\xCC\xB1\xE2\xB0\xED \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.");
			} else {
				sprintf(text, "\xB5\xBF\xC1\xA1\xC0\xCF \xB0\xE6\xBF\xEC\xBF\xA1\xB4\xC2 \xC6\xCE\xC0\xB8\xB7\xCE \xBC\xF8\xC0\xA7\xB8\xA6 \xB0\xE1\xC1\xA4\xC7\xD5\xB4\xCF\xB4\xD9.");
			}
		}
		break;
	}

	case 4:
	case 5:
	case 9:
	case 10:
		for (unsigned char i = 0; i < WSingleton<CSharedDoc>::Instance()->m_gameResultList.size(); i++) {
			if (WSingleton<CSharedDoc>::Instance()->m_gameResultList[i].m_rank == 1) {
				if (leader != 0xFFFFFFFF) {
					sprintf(text, "\xC7\xF6\xC0\xE7 \xC7\xC3\xB7\xB9\xC0\xCC\xBE\xEE\xB5\xE9\xC0\xC7 \xBD\xBA\xC4\xDA\xBE\xEE\xB8\xA6 \xBA\xB8\xBF\xA9\xC1\xD6\xB4\xC2 \xC4\xAB\xB5\xE5\xC0\xD4\xB4\xCF\xB4\xD9.");
					break;
				}

				leader = WSingleton<CSharedDoc>::Instance()->m_gameResultList[i].m_uid;
				if (WSingleton<CSharedDoc>::Instance()->m_roomType == 10) {
					sprintf(text, "\xC7\xF6\xC0\xE7 %s\xB4\xD4\xC0\xCC \xB3\xB2\xC0\xBA\xB0\xC5\xB8\xAE %d.%d y, \xB3\xB2\xC0\xBA\xBD\xC3\xB0\xA3 %d.%03d",
						WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(leader)].m_nickname,
						WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(leader)].m_approachDistance / 10,
						WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(leader)].m_approachDistance % 10,
						WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(leader)].m_approachPoint / 1000,
						WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(leader)].m_approachPoint % 1000);
				} else {
					sprintf(text, "\xC7\xF6\xC0\xE7 %s\xB4\xD4\xC0\xCC \xBD\xBA\xC4\xDA\xBE\xEE %d, \xC6\xCE %I64d",
						WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(leader)].m_nickname,
						WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(leader)].m_score,
						WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex(leader)].m_pang);
				}
				ConditionalStrCat(text, "\xB7\xCE \xBC\xB1\xB5\xCE\xC0\xD4\xB4\xCF\xB4\xD9.");
			}
		}
		break;

	case 1:
		if (TEAM(0)->m_resetCount == TEAM(1)->m_resetCount) {
			__int64 pang[2] = { 0, 0 };
			unsigned char holeCount = GetHoleIndex(GOLFDOC()->m_currentHole);

			for (unsigned char t = 0; t < 2; t++) {
				for (unsigned char j = 0; j < holeCount; j++) {
					if (j == holeCount - 1 && t == PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute) {
						pang[t] += m_holePang;
					} else {
						pang[t] += TEAM(t)->m_holeRecord[WSingleton<CSharedDoc>::Instance()->m_holeOrder[j] - 1];
					}
				}
			}

			if (pang[0] == pang[1]) {
				sprintf(text, "\xB5\xBF\xC1\xA1\xC0\xCF \xB0\xE6\xBF\xEC\xBF\xA1\xB4\xC2 \xC6\xCE\xC0\xB8\xB7\xCE \xBC\xF8\xC0\xA7\xB8\xA6 \xB0\xE1\xC1\xA4\xC7\xD5\xB4\xCF\xB4\xD9.");
				break;
			}

			if (pang[0] > pang[1])
				sprintf(text, "\xC7\xF6\xC0\xE7 \xBD\xBA\xC4\xDA\xBE\xEE\xB4\xC2 \xB5\xBF\xC1\xA1\xC0\xCC\xC1\xF6\xB8\xB8, RED \xC6\xC0\xC0\xCC \xC6\xCE %d", pang[0]);
			else
				sprintf(text, "\xC7\xF6\xC0\xE7 \xBD\xBA\xC4\xDA\xBE\xEE\xB4\xC2 \xB5\xBF\xC1\xA1\xC0\xCC\xC1\xF6\xB8\xB8, BLUE \xC6\xC0\xC0\xCC \xC6\xCE %d", pang[1]);
			ConditionalStrCat(text, "\xB7\xCE \xC0\xCC\xB1\xE2\xB0\xED \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.");
		} else {
			if (TEAM(0)->m_resetCount > TEAM(1)->m_resetCount)
				sprintf(text, "\xC7\xF6\xC0\xE7 RED \xC6\xC0\xC0\xCC %d vs %d", TEAM(0)->m_resetCount, TEAM(1)->m_resetCount);
			else
				sprintf(text, "\xC7\xF6\xC0\xE7 BLUE \xC6\xC0\xC0\xCC %d vs %d", TEAM(1)->m_resetCount, TEAM(0)->m_resetCount);
			ConditionalStrCat(text, "\xB7\xCE \xC0\xCC\xB1\xE2\xB0\xED \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.");
		}
		break;

	default:
		sprintf(text, "\xC7\xF6\xC0\xE7 \xC7\xC3\xB7\xB9\xC0\xCC\xBE\xEE\xB5\xE9\xC0\xC7 \xBD\xBA\xC4\xDA\xBE\xEE\xB8\xA6 \xBA\xB8\xBF\xA9\xC1\xD6\xB4\xC2 \xC4\xAB\xB5\xE5\xC0\xD4\xB4\xCF\xB4\xD9.");
		break;
	}

	FrForm::SetDesc(text);
}

// scoreboard.cpp:7018
void FrScoreForm::ConditionalStrCat(char* dst, const char* src) {
	int len = strlen(dst);

	char last = dst[len - 1];
	if (last == '0' || last == '3' || last == '6') {
		strcat(dst, "\xC0\xB8");
	}

	strcat(dst, src);
}

// scoreboard.cpp:7038 (run-time type and factory registration of FrMatchItems)
const WRTTI FrMatchItems::m_RTTI("FrMatchItems", &FrForm::m_RTTI);

IObject* FrMatchItemsMakeInstance(void) {
	return new FrMatchItems;
}

struct __sFrMatchItems {
	__sFrMatchItems(void) {
		ObjectFactory().AddObjectFunctor(FrMatchItemsMakeInstance, "FrMatchItems");
	}
};
static __sFrMatchItems s_registerFrMatchItems;

// scoreboard.cpp:7040
const sFRESH_MSGMAP* FrMatchItems::GetMessageMap(void) const {
	return &FrMatchItems::_MsgMap;
}

sFRESH_MSGMAP FrMatchItems::_MsgMap = { &FrForm::_MsgMap, &FrMatchItems::_MsgEntries[0] };

sFRESH_ENTRY FrMatchItems::_MsgEntries[] = {
	// scoreboard.cpp:7042
	{ "view", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMatchItems::OnViewInit },
	// scoreboard.cpp:7043
	{ "view", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMatchItems::OnViewOwnerDraw },

	// scoreboard.cpp:7045
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// scoreboard.cpp:7053
FrMatchItems::FrMatchItems(void) {
	m_pSimpleUI = 0;
}

// scoreboard.cpp:7058
FrMatchItems::~FrMatchItems(void) {
	if (m_pSimpleUI) { delete m_pSimpleUI; m_pSimpleUI = 0; }
}

// scoreboard.cpp:7063
bool FrMatchItems::OnInit(void) {
	// scoreboard.cpp:7064
	m_pos.x = (g_view->GetWidth() - 440.0f) * 0.5f;
	m_pos.y = g_view->GetHeight() - 100.0f;

	// scoreboard.cpp:7067
	m_pSimpleUI = new CSimpleUI("[emoti_object.png");
	m_pSimpleUI->Add("0", WRect(0.0f, 128.0f, 64.0f, 64.0f), false);
	m_pSimpleUI->Add("1", WRect(64.0f, 128.0f, 64.0f, 64.0f), false);
	m_pSimpleUI->Add("2", WRect(128.0f, 128.0f, 64.0f, 64.0f), false);
	m_pSimpleUI->Add("3", WRect(192.0f, 128.0f, 64.0f, 64.0f), false);
	m_pSimpleUI->Add("4", WRect(0.0f, 192.0f, 64.0f, 64.0f), false);
	m_pSimpleUI->Add("5", WRect(64.0f, 192.0f, 64.0f, 64.0f), false);
	m_pSimpleUI->Add("6", WRect(128.0f, 192.0f, 64.0f, 64.0f), false);
	m_pSimpleUI->Add("7", WRect(192.0f, 192.0f, 64.0f, 64.0f), false);

	// scoreboard.cpp:7084
	m_failFrame = 0;
	m_bRefresh = true;
	m_bRedraw = true;
	m_unknown150 = 0;
	m_failTime = -3.0f;
	return true;
}

// scoreboard.cpp:7087
void FrMatchItems::OnViewInit(intptr_t param) {
	m_pView = DYNAMIC_CAST(FrArea, param);
}

// scoreboard.cpp:7092
void FrMatchItems::OnViewOwnerDraw(PG_MSGARG param) {
	unsigned long* pItems = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[MyIndex()].m_equipItem;

	WRect rect;
	rect.x = m_pos.x + 8.0f;
	rect.y = m_pos.y + 28.0f;
	rect.w = rect.h = 54.0f;

	int failIndex = 7;
	WOverlay* pFailOverlay = 0;
	IFF_STRUCT::sItem* pFailItem = ItemManager()->FindItem(0x1800000D);
	if (pFailItem)
		pFailOverlay = g_resrcmng->GetOverlay(MakeStr("%s.tga?items.bres", pFailItem->m_icon), 0);

	int count = 10;
	if (IsLocalContent((localContentType_t)3))
		count = 8;

	for (int i = 0; i < count; i++) {
		if (pItems[i] == 0)
			continue;

		std::map<unsigned long, WOverlay*>::iterator it = m_overlays.find(pItems[i]);
		if (it == m_overlays.end()) {
			IFF_STRUCT::sItem* pItem = ItemManager()->FindItem(pItems[i]);
			if (pItem) {
				WOverlay* pOverlay = g_resrcmng->GetOverlay(MakeStr("%s.tga?items.bres", pItem->m_icon), 0);
				if (pOverlay)
					m_overlays.insert(std::map<unsigned long, WOverlay*>::value_type(pItems[i], pOverlay));
			}
		}

		if (pItems[i] == 0x1800000D) {
			failIndex = i - 1;
			break;
		}
	}

	for (int i = 0; i < count; i++) {
		if (pItems[i]) {
			std::map<unsigned long, WOverlay*>::iterator it = m_overlays.find(pItems[i]);
			if (it != m_overlays.end()) {
				WOverlay* pOverlay = it->second;
				if (i == failIndex && m_failTime < 0.0f && m_bRefresh) {
					if (pFailOverlay)
						pFailOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), rect, 0, 0xFFFFFFFF, 0, 0);
				} else {
					pOverlay->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), rect, 0, 0xFFFFFFFF, 0, 0);
				}
			}

			rect.x += 45.0f;
		}
	}

	if (m_bRefresh) {
		m_failTime += 0.1f;
		m_failFrame = (int)(m_failTime * 5.0f);

		if (m_failFrame == 8) {
			m_failTime = -3.0f;
			m_bRefresh = false;
		} else if (m_failFrame >= 0) {
			if (m_bRedraw) {
				g_audio->PlaySfx("play_fail_item", 0, 0, 0, 0, 0.5f, 200.0f);
				m_bRedraw = false;
			}

			if (m_pSimpleUI)
				m_pSimpleUI->Render(MakeStr("%d", m_failFrame), failIndex * 45.0f + m_pos.x, m_pos.y + 8.0f, 1.0f, 0);
		}
	}
}

// scoreboard.cpp:7269 (run-time type and factory registration of CScore)
const WRTTI CScore::m_RTTI("CScore", &IActor::m_RTTI);

IObject* CScoreMakeInstance(void) {
	return new CScore;
}

struct __impCScore {
	__impCScore(void) {
		ObjectFactory().AddObjectFunctor(CScoreMakeInstance, "CScore");
		ActorFactory().AddActorClass("CScore");
	}
};
static __impCScore s_registerCScore;

// scoreboard.cpp:7271
const sFRESH_MSGMAP* CScore::GetMessageMap(void) const {
	return &CScore::_MsgMap;
}

sFRESH_MSGMAP CScore::_MsgMap = { &FrCmdTarget::_MsgMap, &CScore::_MsgEntries[0] };

sFRESH_ENTRY CScore::_MsgEntries[] = {
	// scoreboard.cpp:7275
	{ ".score.close", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CScore::OnCloseBtnUp },

	// scoreboard.cpp:7277
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// scoreboard.cpp:7284
CScore::CScore(void) : m_unknown1c(0), m_awardTime(0), m_bOpen(0), m_unknown28(1), m_unknown2c(1), m_unknown30(0),
	m_pScoreForm(0), m_pRankForm(0), m_pRankForm2(0), m_pRankForm3(0), m_pRankFormEx(0), m_pAwardForm(0), m_pMatchItems(0),
	m_pGuildScoreForm(0), m_pTreasureGauge(0), m_pNextHole(0), m_pTreasureScore(0), m_pTreasureGift(0), m_fadeTime(0),
	m_pAquireItem(0) {
	m_priority = 5;
	m_noticeImages.reserve(10);

	LoadNoticeImageFile(MakeStr("%s/score_ppl.ini", SCORE_PPL_DIR), m_noticeImages, 10, 0);

	ImageSortByCharacterIndex(m_noticeImages);
}

// scoreboard.cpp:7296
void CScore::ImageSortByCharacterIndex(std::vector<sNoticeImage>& images) {
	if (images.size() < 10) {
		return;
	}

	// Each image's name ends in its character index ("score_notice_<n>").
	std::vector<sNoticeImage> sorted;
	sorted.reserve(images.size());

	for (int i = 0; i < 10; i++) {
		sorted.push_back(sNoticeImage());
	}

	for (unsigned int i = 0; i < images.size(); i++) {
		int index = 0;
		const char* prefix = "score_notice_";
		sscanf(images[i].name + strlen(prefix), "%d", &index);

		if (index < 10) {
			strncpy(sorted[index].name, images[i].name, 0x20);
		}
	}

	images.assign(sorted.begin(), sorted.end());
}

// scoreboard.cpp:7329
void CScore::OnDestroy(void) {
	if (m_pRankFormEx) {
		m_pRankFormEx->Close(FrOK, true);
		m_pRankFormEx = 0;
	}

	if (m_pGuildScoreForm) {
		m_pGuildScoreForm->Close(true);
		m_pGuildScoreForm = 0;
	}

	if (m_pAquireItem) { delete m_pAquireItem; m_pAquireItem = 0; }
}

// scoreboard.cpp:7518
void CScore::OnLoad(void) {
	// The rank form's position: a function-scope local in the original (its
	// own frame slot, not shared with the block-scoped points below).
	WPoint pt;

	// scoreboard.cpp:7528
	switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
			break;
		}

		if ((WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().identity & 0xE) == 0xE) {
			m_pRankForm2 = CreateForm<FrRankForm2>(g_pFresh->GetManager(), this, "rank2", 0);
			m_pRankForm2->SetFixed(true);
			m_pRankForm2->EnableDrag(false);
			m_pRankForm2->Open(0, 0xC);
			m_pRankForm2->SetVisible(false);
		} else {
			m_pRankForm = CreateForm<FrRankForm>(g_pFresh->GetManager(), this, "rank", 0);

			m_rankPos.x = g_view->GetWidth() - m_pRankForm->GetRect().w;
			m_rankPos.y = 40.0f;

			m_pRankForm->SetFixed(true);
			m_pRankForm->EnableDrag(false);
			m_pRankForm->Open(0, m_rankPos, 0xC);
			m_pRankForm->SetVisible(false);
			m_pRankForm->Resize();
		}

		if (WSingleton<CSharedDoc>::Instance()->m_roomType != 10) {
			m_pRankFormEx = CreateForm<FrRankFormEx>(g_pFresh->GetManager(), this, "rankex", 0);
			m_pRankFormEx->EnableDrag(false);

			pt.x = (float)(int)((g_view->GetWidth() - m_pRankFormEx->GetRect().w) * 0.5f);
			pt.y = (float)(int)((g_view->GetHeight() - m_pRankFormEx->GetRect().h) * 0.5f);
			m_pRankFormEx->SetFixed(true);
			m_pRankFormEx->Open((FRESH_RESULT_CALLBACK)&CScore::OnRankFormExResult, pt, 4);
			m_pRankFormEx->SetVisible(false);
			m_pRankFormEx->ResetStage();
		}
		break;
	}

	// scoreboard.cpp:7568
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
		m_pRankForm3 = CreateForm<FrRankForm3>(g_pFresh->GetManager(), this, "rank3", 0);

		m_rankPos.x = g_view->GetWidth() - m_pRankForm3->GetRect().w + 5.0f;
		m_rankPos.y = 40.0f;

		m_pRankForm3->SetFixed(true);
		m_pRankForm3->EnableDrag(false);
		m_pRankForm3->Open(0, m_rankPos, 0xC);
		m_pRankForm3->SetVisible(false);

		m_pGuildScoreForm = CreateForm<FrScoreDlg>(g_pFresh->GetManager(), this, "score_guild", 0);

		WPoint guildPt;
		guildPt.x = (g_view->GetWidth() - m_pGuildScoreForm->GetRect().w + 1.0f) * 0.5f;
		guildPt.y = m_pGuildScoreForm->GetRect().y;
		m_pGuildScoreForm->EnableDrag(false);
		m_pGuildScoreForm->SetFixed(true);
		m_pGuildScoreForm->Open((FRESH_RESULT_CALLBACK)&CScore::OnGuildScoreFormResult, guildPt, 0xC);
		m_pGuildScoreForm->SetVisible(false);

		// scoreboard.cpp:7591
		m_pGuildScoreForm->SetVisibleCloseBtn(false);
		return;
	}

	// scoreboard.cpp:7599
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
		m_pScoreForm = CreateForm<FrScoreForm>(g_pFresh->GetManager(), this, "score_skins", 0);
		m_pScoreForm->EnableDrag(false);
		m_pScoreForm->SetFixed(true);
		m_pScoreForm->Open(0, 0xC);
		m_pScoreForm->Resize();
		m_pScoreForm->SetState(FrScoreForm::STATE_HIDDEN);
		m_pScoreForm->SetTypeBtn();
		m_pScoreForm->SetVisible(false);
		m_pScoreForm->SetCaptionOffset(true);
		m_pScoreForm->DescHidePrivacy();
	} else {
		// scoreboard.cpp:7613
		m_pScoreForm = CreateForm<FrScoreForm>(g_pFresh->GetManager(), this, "score", 0);
		m_pScoreForm->EnableDrag(false);
		m_pScoreForm->SetFixed(true);
		m_pScoreForm->Open(0, 0xC);
		m_pScoreForm->Resize();
		m_pScoreForm->SetState(FrScoreForm::STATE_HIDDEN);
		m_pScoreForm->SetTypeBtn();
		m_pScoreForm->SetVisible(false);
		m_pScoreForm->SetCaptionOffset(true);
		m_pScoreForm->DescHidePrivacy();

		// scoreboard.cpp:7626
		m_pTreasureGauge = CreateForm<FrTreasureGauge>(g_pFresh->GetManager(), this, "tPointdlg", 0);
		m_pTreasureGauge->EnableDrag(false);
		m_pTreasureGauge->SetFixed(true);
		m_pTreasureGauge->Open(0, 0xC);
		m_pTreasureGauge->SetState((FrTreasureGauge::eState)1);

		// scoreboard.cpp:7633
		m_pNextHole = CreateForm<FrNextHole>(g_pFresh->GetManager(), this, "nexthole", 0);
		m_pNextHole->EnableDrag(false);
		m_pNextHole->SetFixed(true);
		m_pNextHole->Open(0, 0xC);
		m_pNextHole->SetState(FrNextHole::STATE_HIDE);
	}

	// scoreboard.cpp:7641
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 3) {
		m_pMatchItems = CreateForm<FrMatchItems>(g_pFresh->GetManager(), this, "items", 0);
		m_pMatchItems->SetFixed(true);

		m_pMatchItems->MoveWindow(WPoint((g_view->GetWidth() - 440.0f) * 0.5f, g_view->GetHeight() - 100.0f));
		m_pMatchItems->Open(0, 0xC);
		m_pMatchItems->SetVisible(false);
	}
}

// scoreboard.cpp:7667
void CScore::OnReload(void) {
	this << MsgObject(0, 0x131, 0, 0, 0, 0, 0);
	this << MsgObject(0, 0x2a, 0, 0, 0, 0, 0);
}

// scoreboard.cpp:7673
void CScore::OnProcess(float dt) {
	// scoreboard.cpp:7674
	if (GOLFDOC()->m_gameState < 7) {
		return;
	}

	// scoreboard.cpp:7677
	if (IsLocalContent((localContentType_t)0x4F) && WSingleton<CSharedDoc>::Instance()->m_playingMode == 2) {
		return;
	}

	// scoreboard.cpp:7681
	if (m_pRankForm) {
		if (WSingleton<CCapturedBg>::Instance()->IsCaptured() || (GOLFDOC()->GameMode() & 0x1E84)) {
			// scoreboard.cpp:7685
			if (WSingleton<CSharedDoc>::Instance()->m_roomType != 10 || !(GOLFDOC()->GameMode() & 0x880)) {
				// scoreboard.cpp:7695
				m_pRankForm->SetVisible(false);
			}
		} else {
			// scoreboard.cpp:7700
			bool bShow = g_pFresh->GetManager()->GetDesktop()->IsWallPaperVisible() == false;
			if (GetGameType() == 10) {
				bShow = false;
			}

			// scoreboard.cpp:7707
			m_pRankForm->SetVisible(bShow);

			if (bShow) {
				// scoreboard.cpp:7711
				switch (GOLFDOC()->GameMode()) {
				case 0x40:
					if (WSingleton<CSharedDoc>::Instance()->m_roomType == 10) {
						m_rankPos.x = g_view->GetWidth() - m_pRankForm->GetRect().w;
						m_pRankForm->MoveWindow(m_rankPos);
						break;
					}
					// fall through
				case 0x100:
				case 0x40000:
					// scoreboard.cpp:7722
					m_rankPos.x = Min(m_rankPos.x + dt * 600.0f, g_view->GetWidth());
					m_pRankForm->MoveWindow(m_rankPos);
					break;

				case 8: {
					// scoreboard.cpp:7737
					float mouseX, mouseY;
					SendMsg(this, "Screen", 0x1C6, (intptr_t)&mouseX, (intptr_t)&mouseY, 0, 0);

					float x = g_view->GetWidth() - m_pRankForm->GetRect().w;
					if (x - 60.0f > mouseX) {
						m_rankPos.x = Max(m_rankPos.x - dt * 600.0f, x);
					} else {
						m_rankPos.x = Min(m_rankPos.x + dt * 600.0f, g_view->GetWidth());
					}
					m_pRankForm->MoveWindow(m_rankPos);
					break;
				}

				case 0x10:
				case 0x20:
					break;

				default:
					// scoreboard.cpp:7754
					m_pRankForm->SetVisible(false);
					break;
				}
			}
		}
	}

	// scoreboard.cpp:7762
	if (m_pRankForm3) {
		m_pRankForm3->SetVisible(m_unknown28 == 1);

		// scoreboard.cpp:7771
		if (!WSingleton<CQuitWindow>::Instance()->IsActive() && !WSingleton<CUserInfo>::Instance()->IsVisible()
			&& WSingleton<CChatMsg>::Instance()->GetOpenState() != 3 && !g_pFresh->GetManager()->IsKeyExclusive()
			&& g_input->GetDown("RANK", true) && GOLFDOC()->m_gameState >= 15) {
			if (m_unknown28 == 0) {
				if (GOLFDOC()->GameMode() & ~0x401) {
					m_unknown2c = 1;
					m_unknown28 = 1;
				}
			} else {
				m_unknown2c = 0;
				m_unknown28 = 0;
			}
		}
	}

	// scoreboard.cpp:7799
	if (m_pScoreForm && !WSingleton<CQuitWindow>::Instance()->IsActive() && !WSingleton<CUserInfo>::Instance()->IsVisible()
		&& WSingleton<CChatMsg>::Instance()->GetOpenState() != 3 && !g_pFresh->GetManager()->IsKeyExclusive()
		&& g_input->GetDown("TAB", true) && !g_input->Get("LALT", true) && !g_input->Get("RALT", true)
		&& !g_input->Get("RCONTROL", true) && !g_input->Get("LCONTROL", true) && GOLFDOC()->m_gameState >= 15) {
		if (m_bOpen == 0) {
			if (GOLFDOC()->GameMode() & ~0x401) {
				m_bOpen = 1;
				m_pScoreForm->SetState((FrScoreForm::eState)2);
				m_pScoreForm->SetDesc();

				// scoreboard.cpp:7809
				if (GOLFDOC()->GameMode() & ~0x421) {
					if (m_pTreasureGauge) {
						m_pTreasureGauge->SetState((FrTreasureGauge::eState)2);
					}
					if (m_pNextHole) {
						m_pNextHole->SetState((FrNextHole::eState)2);
					}
				}
			}
		} else if (m_bOpen == 1) {
			// scoreboard.cpp:7847
			m_bOpen = 0;
			m_pScoreForm->SetState(FrScoreForm::STATE_HIDDEN);

			// scoreboard.cpp:7850
			if (GOLFDOC()->GameMode() & ~0x421) {
				if (m_pTreasureGauge) {
					m_pTreasureGauge->SetState((FrTreasureGauge::eState)1);
				}
				if (m_pNextHole) {
					m_pNextHole->SetState(FrNextHole::STATE_HIDE);
				}
			}
		}
	}

	// scoreboard.cpp:7892
	if (m_pGuildScoreForm && !WSingleton<CQuitWindow>::Instance()->IsActive() && !WSingleton<CUserInfo>::Instance()->IsVisible()
		&& WSingleton<CChatMsg>::Instance()->GetOpenState() != 3 && !g_pFresh->GetManager()->IsKeyExclusive()
		&& g_input->GetDown("TAB", true) && !g_input->Get("LALT", true) && !g_input->Get("RALT", true)
		&& !g_input->Get("RCONTROL", true) && !g_input->Get("LCONTROL", true) && GOLFDOC()->m_gameState >= 15) {
		if (m_bOpen == 0) {
			if (GOLFDOC()->GameMode() & ~0x401) {
				m_bOpen = 1;
				m_pGuildScoreForm->SetVisible(true);
				m_pGuildScoreForm->SetGuildPlayer(WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().guid);

				// scoreboard.cpp:7904
				m_pGuildScoreForm->SetVisibleCloseBtn(false);
				m_pGuildScoreForm->SetVisibleCancelBtn(false);
			}
		} else if (m_bOpen == 1) {
			// scoreboard.cpp:7912
			m_bOpen = 0;
			m_pGuildScoreForm->SetVisible(false);
		}
	}

	// scoreboard.cpp:7943
	if (m_bOpen == 2) {
		if (m_fadeTime < 1.0f) {
			m_fadeTime += dt;
			if (!(1.0f > m_fadeTime)) {
				m_fadeTime = 1.0f;
			}
			unsigned char alpha = (unsigned char)(int)(m_fadeTime * 255.0f);
			g_pFresh->GetManager()->GetDesktop()->SetBgColor((alpha << 24) | 0xFFFFFF);
		}
	} else if (m_bOpen == 3) {
		if (m_fadeTime > 0.0f) {
			m_fadeTime -= dt;
			unsigned char alpha = (unsigned char)float2int(Max(0.0f, m_fadeTime) * 255.0f);
			g_pFresh->GetManager()->GetDesktop()->SetBgColor((alpha << 24) | 0xFFFFFF);

			// scoreboard.cpp:7961
			if (m_fadeTime < 0.0f) {
				m_bOpen = 0;
				m_fadeTime = 0;
			}
		}
	}

	// scoreboard.cpp:7969
	if (m_unknown1c == 1 && WSingleton<CSharedDoc>::Instance()->m_gameMode == 0) {
		m_awardTime += dt;

		if (m_awardTime > 3.0f && m_pAwardForm == 0) {
			m_pAwardForm = CreateForm<FrAwardForm>(g_pFresh->GetManager(), this, "award", 0);
			m_pAwardForm->SetFixed(true);
			m_pAwardForm->Open(0, 0xC);
			m_pAwardForm->EnableDrag(false);

			// scoreboard.cpp:7982
			if (m_pAquireItem) {
				m_pAquireItem->m_value = 0;
			} else {
				m_pAquireItem = new cAquireItem;
			}
		}

		// scoreboard.cpp:7989
		if (m_pAquireItem) {
			m_pAquireItem->OnProc(dt);
		}
	}

	// scoreboard.cpp:7994
	if (WSingleton<CSharedDoc>::Instance()->m_value4844 == 1 && WSingleton<CTHunter>::Instance()->IsReceiveGiftPacket()) {
		m_awardTime += dt;

		if (m_awardTime > 2.0f) {
			// scoreboard.cpp:8000
			if (m_pTreasureScore == 0) {
				m_pTreasureScore = CreateForm<FrTreasureScore>(g_pFresh->GetManager(), this, "trscore", 0);
				m_pTreasureScore->Open(0, 0xC);
				m_pTreasureScore->EnableDrag(false);
				m_pTreasureScore->SetFixed(true);
			}

			// scoreboard.cpp:8008
			if (m_pTreasureGift == 0) {
				m_pTreasureGift = CreateForm<FrTreasureGift>(g_pFresh->GetManager(), this, "trgift", 0);
				m_pTreasureGift->Open(0, 0xC);
				m_pTreasureGift->EnableDrag(false);
				m_pTreasureGift->SetFixed(true);
			}
		}
	}
}

// scoreboard.cpp:8020
void CScore::OnDisplay(void) {
	switch (WSingleton<CSharedDoc>::Instance()->m_roomType) {
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
		return;
	}

	if (m_unknown1c == 1) {
		if (m_pAquireItem && m_bOpen != 1) {
			for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
				m_pAquireItem->DisplayPrizeList(i);
			}
		}
	}
}

// scoreboard.cpp:8035
void CScore::OnCloseBtnUp(void) {
	m_bOpen = 0;
}

// scoreboard.cpp:8041
void CScore::CheckOpenedDlg(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_roomType == 10) {
		if (m_pRankForm) {
			m_pRankForm->Close(true);
			m_pRankForm = 0;
		}
		return;
	}

	m_awardTime = 0;
	m_bOpen = 3;
	m_fadeTime = 1.0f;

	if (!m_levelupForms.empty()) {
		for (unsigned int i = 0; i < m_levelupForms.size(); i++) {
			if (m_levelupForms[i]) {
				m_levelupForms[i]->Close(FrNONE, true);
				m_levelupForms[i] = 0;
			}
		}
		m_levelupForms.clear();
	}

	if (m_pScoreForm) {
		m_pScoreForm->SetState(FrScoreForm::STATE_HIDDEN);
	}

	if (m_pTreasureGauge) {
		m_pTreasureGauge->SetState((FrTreasureGauge::eState)1);
	}
	if (m_pNextHole) {
		m_pNextHole->SetState(FrNextHole::STATE_HIDE);
	}

	if (m_pAwardForm) {
		m_pAwardForm->Close(true);
		m_pAwardForm = 0;
	}

	if (m_pAquireItem) { delete m_pAquireItem; m_pAquireItem = 0; }
}

// scoreboard.cpp:8097
void CScore::HandleMsg(const MsgObject& msg) {
	// scoreboard.cpp:8098
	switch (msg.m_msg) {
	case 0x1A0:
		// The same body as case 0x132; written first, the compiler keeps
		// one copy at 0x132's place in the case layout, as the original.
		if (m_pScoreForm) {
			m_pScoreForm->UpdatePang();
		}
		break;

	case 0x12D: {
		// scoreboard.cpp:8102
		FrLevelupItemForm* pForm = CreateForm<FrLevelupItemForm>(g_pFresh->GetManager(), this, "levelupitem", 0);
		if (pForm) {
			pForm->SetData(*(sLevelUpDone*)msg.m_param1);
			pForm->SetTopmost(true);
			pForm->Open((FRESH_RESULT_CALLBACK)&CScore::OnLevelupFormResult, 3);

			// scoreboard.cpp:8110
			m_levelupForms.push_back(pForm);
		}
		break;
	}

	case 0x12E:
		// scoreboard.cpp:8116
		if (msg.m_param1 == 1) {
			CheckOpenedDlg();
		}
		m_unknown1c = msg.m_param1;
		break;

	case 0x12F:
		// scoreboard.cpp:8125
		if (msg.m_param1 == 0) {
			m_bOpen = 0;

			if (IsMassGame() && m_pRankFormEx) {
				m_pRankFormEx->SetActive(false);
			}
			if (m_pScoreForm) {
				m_pScoreForm->SetState(FrScoreForm::STATE_HIDDEN);
			}
			if (m_pTreasureGauge) {
				m_pTreasureGauge->SetState((FrTreasureGauge::eState)1);
			}
			if (m_pNextHole) {
				m_pNextHole->SetState(FrNextHole::STATE_HIDE);
			}

			// scoreboard.cpp:8155
			if (WSingleton<CSharedDoc>::Instance()->m_roomType == 3 && m_pMatchItems) {
				m_pMatchItems->SetVisible(false);
				m_pMatchItems->Refresh();
			}

			// scoreboard.cpp:8161
			if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
				if (m_pGuildScoreForm) {
					m_pGuildScoreForm->SetVisible(false);
				}
				if (m_unknown2c == 1) {
					m_unknown28 = 1;
				}
			}
		} else if (msg.m_param1 == 1) {
			// scoreboard.cpp:8180
			m_bOpen = 2;
			m_fadeTime = 0;
			if (m_pRankForm) {
				m_pRankForm->SetVisible(false);
			}
			if (m_pRankForm3) {
				m_pRankForm3->SetVisible(m_unknown28 == 1);
			}

			// scoreboard.cpp:8186
			if (IsMassGame()) {
				if (m_pRankFormEx) {
					m_pRankFormEx->SetActive(true);
				}
				if (m_pScoreForm) {
					m_pScoreForm->SetState(FrScoreForm::STATE_HIDDEN);
				}

				// scoreboard.cpp:8241
				if (m_pTreasureGauge) {
					m_pTreasureGauge->SetState((FrTreasureGauge::eState)3);
				}
				if (m_pNextHole) {
					m_pNextHole->SetState((FrNextHole::eState)2);
				}

				// scoreboard.cpp:8252
				if (WSingleton<CSharedDoc>::Instance()->m_roomType == 3 && m_pMatchItems) {
					m_pMatchItems->SetVisible(false);
					m_pMatchItems->Refresh();
				}

				// scoreboard.cpp:8258
				if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
					if (m_pGuildScoreForm) {
						m_pGuildScoreForm->SetVisible(true);
						m_pGuildScoreForm->SetGuildPlayer(WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().guid);
						m_pGuildScoreForm->SetVisibleCloseBtn(false);
						m_pGuildScoreForm->SetVisibleCancelBtn(false);
					}
					m_unknown28 = 0;
				}
			} else {
				// scoreboard.cpp:8273
				if (m_pScoreForm) {
					if (m_noticeImages.size() > 0) {
						const char* name = m_noticeImages[WSingleton<CSharedDoc>::Instance()->GetMyPartTidList().m_tidChar & 0x3FFFFFF].name;
						if (name) {
							m_pScoreForm->SetPPL(MakeStr("%s/%s", SCORE_PPL_DIR, name));
						}
					}

					// scoreboard.cpp:8291
					m_pScoreForm->SetState((FrScoreForm::eState)3);
					m_pScoreForm->SetDesc();
				}

				// scoreboard.cpp:8342
				if (m_pTreasureGauge) {
					m_pTreasureGauge->SetState((FrTreasureGauge::eState)3);
				}
				if (m_pNextHole) {
					m_pNextHole->SetState((FrNextHole::eState)2);
				}

				// scoreboard.cpp:8353
				// The match items show every ninth of the course in a match.
				if (m_pMatchItems) {
					if (WSingleton<CSharedDoc>::Instance()->m_roomType == 3 && !GOLFDOC()->IsNetGameEnd()) {
						unsigned char interval = WSingleton<CSharedDoc>::Instance()->m_holes / 9;
						unsigned char hole = GetHoleIndex(GOLFDOC()->m_currentHole);

						if (interval == 0)
							interval = 1;

						if (hole > 0) {
							int rest = hole % interval;

							if (rest == 0) {
								m_pMatchItems->SetVisible(true);
							} else {
								m_pMatchItems->SetVisible(false);
							}
						} else {
							m_pMatchItems->SetVisible(false);
						}
					}
				}
			}
		} else if (msg.m_param1 == 4) {
			// scoreboard.cpp:8385
			if (m_pTreasureGauge) {
				m_pTreasureGauge->SetTPoint();
			}
		}

		// scoreboard.cpp:8391
		m_scoreBg.ScoreBgCommand(msg.m_param1);
		break;

	case 0x130:
		// scoreboard.cpp:8398
		m_bOpen = 3;
		m_fadeTime = 1.0f;
		if (m_pRankForm && WSingleton<CSharedDoc>::Instance()->m_roomType != 10) {
			m_pRankForm->SetVisible(true);
		}

		// scoreboard.cpp:8410
		if (m_pRankForm3) {
			m_pRankForm3->SetVisible(m_unknown28 == 1);
		}
		if (m_pRankFormEx) {
			m_pRankFormEx->SetActive(false);
		}
		if (m_pScoreForm) {
			m_pScoreForm->SetState(FrScoreForm::STATE_HIDDEN);
		}
		if (m_pMatchItems) {
			m_pMatchItems->SetVisible(false);
		}
		break;

	case 0x131:
		// scoreboard.cpp:8425
		if (m_pScoreForm) {
			m_pScoreForm->SortLists(true);
		}
		break;

	case 0x132:
		// scoreboard.cpp:8436
		if (m_pScoreForm) {
			m_pScoreForm->UpdatePang();
		}
		break;

	case 0x2A:
		// scoreboard.cpp:8442
		if (IsMassGame()) {
			if (m_pRankForm) {
				m_pRankForm->SortRank();
				if (msg.m_param1) {
					m_pRankForm->Align();
				}
			}
			if (m_pRankForm2) {
				m_pRankForm2->SortRank();
			}
			if (m_pRankFormEx) {
				m_pRankFormEx->SortRank();
			}
		}
		break;

	case 0x133:
		// scoreboard.cpp:8462
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 4 && m_pRankForm) {
			m_pRankForm->m_rankCount--;
			m_pRankForm->m_rankText[msg.m_param1]--;
		}
		break;

	case 0x134:
		// scoreboard.cpp:8475
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 4 && m_pRankForm) {
			unsigned char myRank = WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_rank;
			int score = WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_score + msg.m_param1;
			__int64 pang = WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_pang;
			unsigned char rank = 1;

			// scoreboard.cpp:8485
			for (std::vector<sRivalData>::iterator it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin();
				it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
				if (score > it->m_score || (score == it->m_score && pang < it->m_pang)) {
					rank++;
				}
			}

			// scoreboard.cpp:8491
			int gain = myRank - rank;
			if (gain >= 3) {
				m_pRankForm->m_pBoard->SetCaption(MakeStr("%s\xB4\xD4 \xC0\xCC\xB9\xF8\xBF\xA1 \xC8\xA6\xC0\xCE\xC7\xCF\xB8\xE9 %d\xB8\xED\xC0\xBB \xC0\xCC\xB1\xE6 \xBC\xF6 \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.",
					WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().nickname, gain));
				m_pRankForm->m_boardMessage = 1;
				m_pRankForm->SetCaptionType(2);
			}
		}
		break;

	case 0x135:
		// scoreboard.cpp:8501
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 4) {
			if (m_pRankForm) {
				m_pRankForm->m_pBoard->SetCaption("\xC0\xDA~ \xB8\xB6\xC1\xF6\xB8\xB7 \xC8\xA6\xC0\xD4\xB4\xCF\xB4\xD9. \xB8\xB7\xC6\xC7 \xC6\xC4\xC0\xCC\xC6\xC3!");
				m_pRankForm->m_boardMessage = 1;
				m_pRankForm->SetCaptionType(1);
			}

			// scoreboard.cpp:8511
			if (m_pRankForm3) {
				m_pRankForm3->m_pBoard->SetCaption("\xC0\xDA~ \xB8\xB6\xC1\xF6\xB8\xB7 \xC8\xA6\xC0\xD4\xB4\xCF\xB4\xD9. \xB8\xB7\xC6\xC7 \xC6\xC4\xC0\xCC\xC6\xC3!");
				m_pRankForm3->SetCaptionType(1);
			}
		}
		break;

	case 0x136:
		// scoreboard.cpp:8520
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 4 && m_pRankForm) {
			m_pRankForm->m_rankText[msg.m_param1]++;
			m_pRankForm->m_rankText[msg.m_param1 - 1]--;

			// scoreboard.cpp:8526
			unsigned char hole = GOLFDOC()->m_currentHole;
			if (m_pRankForm->m_boardMessage > 2 && m_pRankForm->m_announcedHole != hole) {
				unsigned char maxCount = 0;
				unsigned char maxHole = 0;
				for (int i = 0; i < 0x13; i++) {
					if ((unsigned char)m_pRankForm->m_rankText[i] > maxCount) {
						maxHole = i + 1;
						maxCount = m_pRankForm->m_rankText[i];
					}
				}

				// scoreboard.cpp:8541
				if (GetHoleIndex(maxHole) >= GetHoleIndex(hole) + 5) {
					m_pRankForm->m_announcedHole = hole;
					m_pRankForm->m_pBoard->SetCaption(MakeStr("%s\xB4\xD4 \xB8\xB9\xC0\xCC \xB5\xDA\xC3\xC4\xC1\xAE \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9. \xBC\xAD\xB5\xCE\xB8\xA3\xBC\xBC\xBF\xE4.",
						WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().nickname));
					m_pRankForm->m_boardMessage = 2;
					m_pRankForm->SetCaptionType(2);
				}
			}
		}
		break;

	case 0x137:
		// scoreboard.cpp:8554
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 4 && m_pRankForm) {
			// scoreboard.cpp:8557
			if (m_pRankForm->m_boardMessage > 3) {
				if ((unsigned long)m_pRankForm->m_rankPage == ((bool)((WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().identity >> 1) & 1) ? WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().galleryGuid : WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().guid)) {
					// scoreboard.cpp:8561
					m_pRankForm->m_pBoard->SetCaption(MakeStr("%s\xB4\xD4 \xB2\xC3\xC2\xEE\xBF\xB9\xBF\xE4. \xC8\xFB\xB3\xBB\xBC\xBC\xBF\xE4.",
						WSingleton<CSharedDoc>::Instance()->m_gameResultList[WSingleton<CSharedDoc>::Instance()->GetIndex((bool)((WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().identity >> 1) & 1) ? WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().galleryGuid : WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().guid)].m_nickname));
					m_pRankForm->m_boardMessage = 3;
					m_pRankForm->SetCaptionType(1);
				}
			}
		}
		break;

	case 0x138:
		// scoreboard.cpp:8569
		if (m_pRankFormEx) {
			*(bool*)msg.m_param1 = m_pRankFormEx->IsMotionFinished();
		} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
			// scoreboard.cpp:8572
			*(bool*)msg.m_param1 = m_fadeTime >= 1.0f ? true : false;
		} else {
			// scoreboard.cpp:8574
			*(bool*)msg.m_param1 = true;
		}
		break;

	case 0x271B: {
		// scoreboard.cpp:8582
		WPoint pt;
		if (m_pRankForm) {
			pt.x = g_view->GetWidth() - m_pRankForm->GetRect().w;
			pt.y = 40.0f;
			m_pRankForm->MoveWindow(pt);
		}

		// scoreboard.cpp:8590
		if (m_pRankForm2) {
			pt.x = (g_view->GetWidth() - m_pRankForm2->GetRect().w) * 0.5f;
			pt.y = 40.0f;
			m_pRankForm->MoveWindow(pt);   // the original moves m_pRankForm here
		}

		// scoreboard.cpp:8598
		if (m_pRankForm3) {
			pt.x = g_view->GetWidth() - m_pRankForm3->GetRect().w + 5.0f;
			pt.y = 40.0f;
			m_pRankForm3->MoveWindow(pt);
		}

		// scoreboard.cpp:8606
		if (m_pRankFormEx) {
			pt.x = (float)float2int((g_view->GetWidth() - m_pRankFormEx->GetRect().w) * 0.5f);
			pt.y = (float)float2int((g_view->GetHeight() - m_pRankFormEx->GetRect().h) * 0.5f);
			m_pRankFormEx->MoveWindow(pt);
			m_pRankFormEx->ResetStage();
		}

		// scoreboard.cpp:8615
		if (m_pScoreForm) {
			pt.x = (float)float2int((g_view->GetWidth() - m_pScoreForm->GetRect().w) * 0.5f);
			pt.y = 20.0f;
			m_pScoreForm->MoveWindow(pt);
		}

		// scoreboard.cpp:8623
		if (m_pAwardForm) {
			float barY = WSingleton<COption>::Instance()->gGetBar_Y();
			pt.x = (float)float2int((g_view->GetWidth() - m_pAwardForm->GetRect().w) * 0.5f);
			pt.y = barY - 100.0f;
			m_pAwardForm->MoveWindow(pt);
		}
		break;
	}

	case 0x139: {
		// scoreboard.cpp:8638
		bool* pResult = (bool*)msg.m_param1;
		*pResult = m_bOpen != 0;

		if (m_pRankForm) {
			WRect rect;
			m_pRankForm->GetClientRect(rect);
			if (rect.IsInRect(WPoint(g_input->GetMousePoint().x, g_input->GetMousePoint().y))) {
				*pResult = true;
			}
		}

		// scoreboard.cpp:8648
		if (m_pRankForm3) {
			WRect rect;
			m_pRankForm3->GetClientRect(rect);
			if (rect.IsInRect(WPoint(g_input->GetMousePoint().x, g_input->GetMousePoint().y))) {
				*pResult = true;
			}
		}
		break;
	}

	case 0x13A: {
		// scoreboard.cpp:8663
		CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
		unsigned char par3 = 0;
		unsigned char par4 = 0;
		unsigned char par5 = 0;
		for (unsigned char i = pDoc->m_holeOrder[0]; i <= pDoc->m_holeOrder[pDoc->m_holes - 1]; i++) {
			switch (GOLFDOC()->GetHoleData(i).m_kind) {
			case 2:
				par3++;
				break;
			case 3:
				par4++;
				break;
			case 4:
				par5++;
				break;
			}
		}

		// scoreboard.cpp:8685
		unsigned char difficulty = GetDifficulty() - 1;
		*(int*)msg.m_param1 = data_9f1f48[difficulty][par5][5] + data_9f1f48[difficulty][par4][2] + data_9f1f48[difficulty][par3][0];
		break;
	}

	case 0x13B:
		// scoreboard.cpp:8705
		if (m_pRankForm2) {
			switch (msg.m_param1) {
			case -1:
				m_pRankForm2->SetVisible(!m_pRankForm2->IsVisible());
				break;

			case 0:
				m_pRankForm2->SetVisible(false);
				break;

			case 1:
				m_pRankForm2->SetVisible(true);
				break;
			}
		}
		break;

	case 0x57:
		// scoreboard.cpp:8725
		m_pRankForm3->SetScore(msg.m_param1, msg.m_param2);
		break;

	case 0x13C:
		// scoreboard.cpp:8729
		UpdateRivals();
		break;
	}
}

// scoreboard.cpp:8735
bool CScore::OnLevelupFormResult(int ret, FrForm* pForm) {
	if (m_levelupForms.size() != 0) {
		m_levelupForms[m_levelupForms.size() - 1] = 0;
		m_levelupForms.erase(m_levelupForms.end() - 1);
	}
	return true;
}

// scoreboard.cpp:8746
bool CScore::OnRankFormExResult(int ret, FrForm* pForm) {
	if (m_pRankFormEx) {
		m_pRankFormEx->Clear();
		m_pRankFormEx = 0;
	}
	return true;
}

// scoreboard.cpp:8758
bool CScore::UpdateRivals(void) {
	if (m_pRankForm) {
		FrListBox* pList = m_pRankForm->m_pRankList;

		if (pList) {
			pList->ClearItem();
			for (std::vector<sGameResultInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
				pList->AddItem(&*it);
			}

			pList->UseRightButton(true);

			m_pRankForm->m_rankCount = WSingleton<CSharedDoc>::Instance()->m_gameResultList.size();
			m_pRankForm->m_rankText[0] = WSingleton<CSharedDoc>::Instance()->m_gameResultList.size();
			m_pRankForm->SortRank();
		}
	}

	FrRankFormEx* pRankEx = m_pRankFormEx;
	if (pRankEx) {
		if (pRankEx->m_rankEx.m_pRival) {
			pRankEx->m_rankEx.ClearRival();
		}

		m_pRankFormEx->m_rankEx.SetRivalData();
	}

	return true;
}

// scoreboard.cpp:8802
CScore::CScoreBg::CScoreBg(void) {
	m_pImage = 0;
	m_bShow = false;
}

// scoreboard.cpp:8808
CScore::CScoreBg::~CScoreBg(void) {
	if (m_pImage) { delete m_pImage; m_pImage = 0; }
	m_bShow = false;
	g_pFresh->GetDesktop()->LockWallPaper(false);
}

// scoreboard.cpp:8815
void CScore::CScoreBg::SetDesktopWallPaper(bool bShow, bool bDefault) {
	if (m_bShow) {
		g_pFresh->GetManager()->GetDesktop()->SetWallPaperFromBitmap(m_pImage, bShow);
		if (m_pImage) { delete m_pImage; m_pImage = 0; }
	} else if (bDefault && bShow) {
		g_pFresh->GetManager()->GetDesktop()->SetWallPaper("scoreboard_background.jpg", bShow);
	}
}

// scoreboard.cpp:8828
void CScore::CScoreBg::ScoreBgCommand(int command) {
	switch (command) {
	case 0:
		SetDesktopWallPaper(false, false);
		break;

	case 1:
	case 2:
		SetDesktopWallPaper(true, true);
		break;

	case 3:
		if (GOLFDOC()->GameMode() == 0x400000) {
			if (m_pImage) { delete m_pImage; m_pImage = 0; }
			m_pImage = new Bitmap((int)g_view->GetWidth(), (int)g_view->GetHeight(), 24);

			if (g_resrcmng->VideoReference()->Command(W_VDEV_CAPTURE_SCREEN, 0, (intptr_t)m_pImage) == 1) {
				m_bShow = true;
				g_pFresh->GetManager()->GetDesktop()->LockWallPaper(true);
			} else {
				if (m_pImage) { delete m_pImage; m_pImage = 0; }
				m_bShow = false;
				g_pFresh->GetManager()->GetDesktop()->LockWallPaper(false);
			}
		}
		break;
	}
}

// scoreboard.cpp:8880
bool CScore::OnGuildScoreFormResult(int ret, FrForm* pForm) {
	if (m_pGuildScoreForm) {
		m_pGuildScoreForm = 0;
	}
	return true;
}

// scoreboard.h:717
CScore::~CScore(void) {
}
