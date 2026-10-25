// JP-only game code: FrGrandPrixAwardDlg (JpGrandPrixAwardDlg.h).
#include "JpGrandPrixAwardDlg.h"

#include <string.h>

#include "IContentsDoc.h"
#include "ItemManager.h"
#include "JpContentsGatewayDocs.h"
#include "JpLobbyGlue.h"
#include "Packet.h"
#include "SharedDoc.h"
#include "SimpleOverlay.h"
#include "SoundManager.h"
#include "TitleFontEx.h"
#include "uccmanager.h"
#include "bitmap.h"
#include "classdefine.h"
#include "commonutil.h"
#include "frarea.h"
#include "fresh.h"
#include "frstatic.h"
#include "frgraphicinterface.h"
#include "frwnd.h"
#include "wfont.h"
#include "wresrcmng.h"
#include "wview.h"
#include "shared/luasystem/LuaScript.h"
#include "shared/luasystem/LuaScriptManager.h"
#include "shared/luasystem/luatablehelper.h"

// 0x9d8b70 (JP CItemManager, on CSharedDoc 0xe47dbc +0x20c; name ours;
// defined below, no JpItemGlue copy yet): 1-5 for the 0x18000000 item ids
// of five special classes (bit 0x2000000 set, 0x1f00000 clear), 0
// otherwise.
int JpItemManager_GetSpecialItemKind(uint32_t typeId);

// Glue OnInit 0x54d110 reads. The JP CSharedDoc is 0xe47dbc, its JP
// CItemManager +0x20c; the JP contents container 0xaf (CContentsDoc::
// GetContainer 0x9137e0 on 0xe47db4) is the grand prix document
// (JpContentsGatewayDocs.h).
using _gateway::_grandprix::CGrandPrixDocument;

// JP CSharedDoc +0xffc8: the local player's JP game statistics, 0x109
// bytes (the KR sPangYaUserStatistics is 0xeb; the KR CGolfRule sends
// m_gameStatistics[MyIndex()] in the same packet 6). The JP block has no
// KR member and no JP writer is ported: the port holds it here, zeroed.
static unsigned char s_jpMyGameStatistics[0x109];

const void* JpSharedDoc_GetMyGameStatistics(void) {
	return s_jpMyGameStatistics;
}

// JP CSharedDoc +0x109c1 / +0x109c5 (room record 0x10904 +0xbd / +0xc1):
// the grand prix entry key and the grand prix type id (bits 19-25: the
// grade); no KR counterpart and no JP writer ported: the port holds them
// here (0: no grand prix, OnInit then shows no results nor rewards).
static uint32_t s_jpGrandPrixKey = 0;
static uint32_t s_jpGrandPrixTypeId = 0;

uint32_t JpSharedDoc_GetGrandPrixKey(void) {
	return s_jpGrandPrixKey;
}

uint32_t JpSharedDoc_GetGrandPrixTypeId(void) {
	return s_jpGrandPrixTypeId;
}

// JP CSharedDoc +0x1042c / +0x10430: the room's result players, the KR
// m_gameResultList (sRivalData, the JpRoomTypeDlgs.cpp pairing).
unsigned int JpSharedDoc_GetRoomPlayerCount(void) {
	return (unsigned int)CSharedDoc::Instance()->m_gameResultList.size();
}

const sRivalData* JpSharedDoc_GetRoomPlayer(unsigned int index) {
	return &CSharedDoc::Instance()->m_gameResultList[index];
}

// 0x9137e0(0xaf) on the JP CContentsDoc 0xe47db4: the grand prix document
// the creator 0x559c00 made (IContentsDoc.cpp under JP_PAKS), NULL without
// it.
CGrandPrixDocument* JpContents_GetGrandPrixContainer(void) {
	IContentsDataContainer* data = NULL;
	if (!WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0xaf, data)) {
		return NULL;
	}
	return static_cast<CGrandPrixDocument*>(data);
}

// CGrandPrixDocument::FindEntry 0x5551f0; the port has no container
// before the contents doc made it: NULL then.
sJpGrandPrixEntry* JpGrandPrixContainer_FindEntry(CGrandPrixDocument* container, uint32_t key) {
	if (!container) {
		return NULL;
	}
	return container->FindEntry(key);
}

// The container's result vector +0x40 / +0x44 (0xe4-byte rows, read
// inline by OnInit): CGrandPrixDocument keeps +0x3c..+0x50 unknown and no
// port code fills it: empty.
unsigned int JpGrandPrixContainer_GetResultCount(const CGrandPrixDocument* container) {
	(void)container;
	return 0;
}

const sJpGrandPrixResult* JpGrandPrixContainer_GetResult(const CGrandPrixDocument* container, unsigned int index) {
	(void)container;
	(void)index;
	return 0;
}

// 0x5587b0: the reward rate of grade (1-3) for the grand prix type (a
// set of the map +0x30 entries of that grade, 600 bytes). Reached only
// with an entry, which the port never has: -1 (OnInit's no-grade rate).
float JpGrandPrixContainer_GetRewardRate(CGrandPrixDocument* container, int grade, uint32_t typeId) {
	(void)container;
	(void)grade;
	(void)typeId;
	return -1.0f;
}

// CGrandPrixDocument::FindEntryByType 0x553dd0 (NULL without the
// container).
sJpGrandPrixEntry* JpGrandPrixContainer_FindEntryByType(CGrandPrixDocument* container, uint32_t typeId) {
	if (!container) {
		return NULL;
	}
	return container->FindEntryByType(typeId);
}

// 0xa17cb0 (JP CItemManager map +0x4c4, keyed by the low 16 bits): a
// grand prix NPC, NULL for none. The KR CItemManager has no grand prix
// NPC table: NULL.
const sJpGrandPrixNpc* JpItemManager_FindGrandPrixNpc(uint32_t id) {
	(void)id;
	return 0;
}

// 0xa17bd0 (JP CItemManager map +0x524): the per-rank rewards of a grand
// prix type, NULL for none. The KR CItemManager has no such table: NULL.
const std::vector<sJpGrandPrixRankReward>* JpItemManager_FindGrandPrixRankRewards(uint32_t typeId) {
	(void)typeId;
	return 0;
}

// 0xa173e0 (JP CItemManager map +0xc0, the node value at +0x10): the KR
// CItemManager::FindMatch.
const IFF_STRUCT::sMatch* JpItemManager_FindMatch(uint32_t typeId) {
	return ItemManager()->FindMatch(typeId);
}

// The JP-only WTitleFontEx subclass of the rank number (size 0xc, vtable
// 0xd1c2c4, name ours from its texture; the CTPNumberFont shape of
// JpAlchemyDlg.cpp). OnInit 0x54d110 news it into m_pRankFont.
class CGrandPrixRankFont : public WTitleFontEx {
public:
	CGrandPrixRankFont(void);
};

// 0xe16a8c: the font's texture entry (0xd1c2c8).
static char s_rankFontName[] = "[font_gp_rankingnum.tga";
static char* s_rankFontFile = s_rankFontName;
// 0xd1c040: its character set.
static char s_rankFontChars[] = "1234567890";

// 0x6f5480
CGrandPrixRankFont::CGrandPrixRankFont(void) {
	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = &s_rankFontFile;
	info.numPages = 1;
	info.fontw = 40;
	info.fonth = 44;
	info.texw = 240;
	info.texh = 88;
	info.pCharSet = s_rankFontChars;

	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

// The room player of uid, NULL for none: a loop OnInit 0x54d110 has
// twice inline (no out-of-line copy found).
static const sRivalData* FindRoomPlayer(uint32_t uid) {
	for (unsigned int i = 0; i < JpSharedDoc_GetRoomPlayerCount(); ++i) {
		const sRivalData* player = JpSharedDoc_GetRoomPlayer(i);
		// JP +0x0: the KR m_id.
		if (player->m_id == uid) {
			return player;
		}
	}
	return 0;
}

// lib: 0x54f2e0 (scalar deleting destructor), 0x54dc00, 0x54dc10, 0x54dc20 (this-adjusting thunks)

// 0xe75130 (static init not traced)
const WRTTI FrGrandPrixAwardDlg::m_RTTI("FrGrandPrixAwardDlg", &FrFormBase::m_RTTI);

// 0xe75128
FrGrandPrixAwardDlg* FrGrandPrixAwardDlg::s_pAwardDlg = 0;

// 0xe7512c: set by 0x69cd90 (the award scene), read by OnInit 0x54d110,
// cleared by the destructor: the match's type id.
uint32_t data_e7512c = 0;

// 0xe0b56c (static init 0xcce470): the award timer's name
static std::string s_awardTimerName("grandprix_award_ui");

// 0x546b60
FrFormBase* FrGrandPrixAwardDlg::SetSubForm(FrFormBase* pSubForm) {
	s_pAwardDlg = static_cast<FrGrandPrixAwardDlg*>(pSubForm);
	return FrFormBase::SetSubForm(pSubForm);
}

// 0x546b80
void FrGrandPrixAwardDlg::ClearSubForm(void) {
	s_pAwardDlg = 0;
	m_pSubForm = 0;
}

// 0x546b90
const sFRESH_MSGMAP* FrGrandPrixAwardDlg::GetMessageMap(void) const {
	return &FrGrandPrixAwardDlg::_MsgMap;
}

// 0x546ba0
bool FrGrandPrixAwardDlg::UpdateCloseDelay(float dt) {
	if (0.0f < m_closeDelay) {
		m_closeDelay -= dt;
		if (m_closeDelay < 0.0f) {
			m_closeDelay = 0.0f;
			return true;
		}
	}
	return false;
}

// 0x546bf0
bool FrGrandPrixAwardDlg::UpdateMyRankDelay(float dt) {
	if (m_drawMyRank) {
		m_myRankDelay -= dt;
		if (m_myRankDelay < 0.0f) {
			m_myRankDelay = 0.0f;
			return true;
		}
	}
	return false;
}

// 0x546c40
bool FrGrandPrixAwardDlg::UpdateStepTime(float dt) {
	if (!m_drawListRank) {
		m_stepTimeLeft -= dt;
		if (m_stepTimeLeft < 0.0f) {
			m_stepTimeLeft = 0.0f;
			return true;
		}
	}
	return false;
}

// 0x546c90
bool FrGrandPrixAwardDlg::UpdateNextDelay(float dt) {
	m_nextDelay -= dt;
	if (0.0f < m_nextDelay) {
		m_nextDelay = 0.0f;
		return true;
	}
	return false;
}

// 0x546cd0
void FrGrandPrixAwardDlg::OnWinnerShown(void) {
	++m_shownWinners;
	m_nextPending = false;
	float delay;
	if (m_shownWinners < 3) {
		delay = 0.2f;
	} else {
		delay = m_lastWinnerDelay;
	}
	m_nextDelay = delay;
}

// 0x546d20
void CGrandPrixRewardList::OnSlotDrawn(bool last) {
	if (last && !m_stepPending) {
		if (m_step == 0) {
			m_stepPending = true;
			m_stepDelay = m_step0Delay;
			return;
		}
		if (m_step == 1) {
			m_stepDelay = m_step1Delay;
		}
		m_stepPending = true;
	}
}

// lib: 0x546d70 (CTimerManager::IsInstantiated, WSingleton header inline)

// 0xe0b564
sFRESH_MSGMAP FrGrandPrixAwardDlg::_MsgMap = { &FrFormBase::_MsgMap, &FrGrandPrixAwardDlg::_MsgEntries[0] };

// 0xe0b588 (the handlers are stored by the static init 0xccdfc0)
sFRESH_ENTRY FrGrandPrixAwardDlg::_MsgEntries[] = {
	{ "common_area", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "area_bg1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "area_bg2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "area_bg3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "area_bg4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "outback0", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "outback1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "outback2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "outback3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "outback4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "outback5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "outback6", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "outback7", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "outback8", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "inback0", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "inback1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "inback2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "inback3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "inback4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "inback5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "inback6", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "inback7", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "inback8", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "fbar0", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "fbar1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "rewardlist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "char1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "char2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "char3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "myrank123", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "myrank4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "char1_name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "char2_name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "char3_name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "rewardlist", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnRewardListDraw },
	{ "common_area", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnCommonAreaDraw },
	{ "trophy", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "trophy_attr1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "trophy_attr2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "trophy_other1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ "trophy_other2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixAwardDlg::OnControlInit },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x547390
void CGrandPrixRewardList::SetPosition(int x, int y) {
	m_x = (float)x;
	m_y = (float)y;
	const Bitmap* box = g_pFresh->GetBitmap("th_item_box_n");
	if (box != 0) {
		for (int i = 0; i < m_itemCount; ++i) {
			// The bitmap's size is read as unsigned.
			unsigned int width = (unsigned int)box->bi->bmiHeader.biWidth;
			unsigned int height = (unsigned int)box->bi->bmiHeader.biHeight;
			m_itemPos[i].x = (float)(i % 5) * ((float)width + m_boxGapX) + m_boxGapX + m_x;
			m_itemPos[i].y = ((float)height + m_boxGapY) * (float)(i > 4) + m_boxGapY + m_y;
		}
	}
}

// 0x547b40
void FrGrandPrixAwardDlg::OnCommonAreaDraw(PG_MSGARG param) {
	if (m_drawListRank && m_myRank > 3 && m_pRankFont) {
		int x = (int)m_listRankPos.x;
		int y = (int)m_listRankPos.y;
		m_pRankFont->Print((float)x, (float)y, MakeStr("%d", m_myRank), TITLEFONT_ALIGN_CENTER, 0.0f, m_listRankPos.scale);
	}
	if (m_drawMyRank && m_pRankFont) {
		if (m_myRank > 3) {
			int x = (int)m_myRank4Pos.x;
			int y = (int)m_myRank4Pos.y;
			m_pRankFont->Print((float)x, (float)y, MakeStr("%d", m_myRank), TITLEFONT_ALIGN_LEFT, 0.0f, m_myRank4Pos.scale);
		} else if (m_myRank != 0) {
			int x = (int)m_myRank123Pos.x;
			int y = (int)m_myRank123Pos.y;
			m_pRankFont->Print((float)x, (float)y, MakeStr("%d", m_myRank), TITLEFONT_ALIGN_CENTER, 0.0f, m_myRank123Pos.scale);
		}
	}
}

// 0x547ca0
void CGrandPrixRewardList::Draw(FrGraphicInterface* gdi, float dt) {
	if (m_shown) {
		for (int i = 0; i < m_itemCount; ++i) {
			const sItemPos& pos = m_itemPos[i];
			const Bitmap* box;
			if ((unsigned int)i >= m_rewards.size() || m_step != 2) {
				box = m_pBox;
			} else {
				box = m_pRewardBox;
			}
			if (box != 0) {
				gdi->DrawTexture(box, WRect(pos.x, pos.y, box->Width(), box->Height()), 0xffffffff, 0);
			}
			int x = 0;
			int y = 0;
			switch (m_step) {
			case 0:
				x = (int)(m_cardOffset.x + pos.x);
				y = (int)(m_cardOffset.y + pos.y);
				m_colors[i] = 0xffffffff;
				// fall through
			case 1:
				if ((double)i * 0.1 < m_stepTime) {
					const Bitmap* bmp = m_slotBitmaps[i].step[m_step];
					if (bmp != 0) {
						m_colors[i] = 0xffffffff;
						if (m_step != 1) {
							gdi->DrawTexture(bmp, WRect((float)x, (float)y, bmp->Width(), bmp->Height()), 0xffffffff, 0);
						} else {
							if ((unsigned int)i < m_rewards.size()) {
								m_colors[i] = 0xa0202020;
								x = (int)((float)((m_boxWidth - m_widths[i]) / 2) + m_itemOffset.x + pos.x);
								y = (int)((float)((m_boxHeight - m_heights[i]) / 2) + m_itemOffset.y + pos.y);
								const sAwardReward& reward = m_rewards[i];
								if ((reward.typeId & 0xfc000000) == 0x18000000) {
									x = (int)((float)x + m_classOffset.x);
									y = (int)((float)y + m_classOffset.y);
								}
								if (JpItemManager_GetSpecialItemKind(reward.typeId) != 0) {
									x = (int)((float)x - 10.0f);
									y = (int)((float)y - 9.0f);
								}
							} else {
								x = (int)(m_emptyOffset.x + pos.x);
								y = (int)(m_emptyOffset.y + pos.y);
							}
							gdi->DrawTexture(bmp, WRect((float)x, (float)y, m_widths[i], m_heights[i]), m_colors[i], 0);
						}
						OnSlotDrawn(i == m_itemCount - 1);
					}
				} else {
					const Bitmap* bmp = m_slotBitmaps[i].step[0];
					if (bmp != 0) {
						int cardX = (int)(m_cardOffset.x + pos.x);
						int cardY = (int)(m_cardOffset.y + pos.y);
						gdi->DrawTexture(bmp, WRect((float)cardX, (float)cardY, bmp->Width(), bmp->Height()), 0xffffffff, 0);
					}
				}
				break;
			case 2: {
				const Bitmap* bmp = m_slotBitmaps[i].step[2];
				if (bmp != 0) {
					float fx;
					float fy;
					unsigned long color;
					if ((unsigned int)i < m_rewards.size()) {
						fx = (float)((m_boxWidth - m_widths[i]) / 2) + m_itemOffset.x + pos.x;
						fy = (float)((m_boxHeight - m_heights[i]) / 2) + m_itemOffset.y + pos.y;
						const sAwardReward& reward = m_rewards[i];
						if ((reward.typeId & 0xfc000000) == 0x18000000) {
							fx = m_classOffset.x + fx;
							fy = m_classOffset.y + fy;
						}
						if (JpItemManager_GetSpecialItemKind(reward.typeId) != 0) {
							fx = (float)(fx - 10.0);
							fy = (float)(fy - 9.0);
						}
						color = m_colors[i];
					} else {
						color = 0xffffffff;
						fx = m_emptyOffset.x + pos.x;
						fy = m_emptyOffset.y + pos.y;
					}
					gdi->DrawTexture(bmp, WRect(fx, fy, m_widths[i], m_heights[i]), color, 0);
				}
				if ((double)i * 0.2 < m_stepTime) {
					if ((unsigned int)i < m_rewards.size()) {
						if (m_counts[i] == 0.0f) {
							g_audio->PlaySfx("play_fail_item", 0, 0, 0, 0, 0.5f, 200.0f);
						}
						m_colors[i] = 0xffffffff;
						if (m_pCountUI && 0.0f <= m_counts[i]) {
							int count = (int)m_counts[i];
							if (count < 8) {
								m_pCountUI->Render(MakeStr("%d", count), m_countUIOffset.x + pos.x, m_countUIOffset.y + pos.y, 1.5f, 0);
								m_counts[i] = (float)(dt * 15.0 + m_counts[i]);
							}
						}
						const sAwardReward& reward = m_rewards[i];
						if (reward.kind == sAwardReward::SHOW_COUNT) {
							if (m_countFonts[i]) {
								int textX = (int)(m_countTextOffset.x + pos.x);
								int textY = (int)(m_countTextOffset.y + pos.y);
								m_countFonts[i]->Print((float)textX, (float)textY, MakeStr("%d", reward.count), TITLEFONT_ALIGN_CENTER, 0.0f, 1.0f);
							}
						} else if (reward.kind == sAwardReward::SHOW_BITMAP) {
							const Bitmap* item = m_itemBitmaps[i];
							if (item != 0) {
								int itemX = (int)(m_itemBitmapOffset.x + pos.x);
								int itemY = (int)(m_itemBitmapOffset.y + pos.y);
								gdi->DrawTexture(item, WRect((float)itemX, (float)itemY, item->Width(), item->Height()), 0xffffffff, 0);
							}
						}
						if ((int)m_counts[m_rewards.size() - 1] >= 8) {
							m_ready = true;
						}
					}
					if (m_rewards.size() == 0 && i == m_itemCount - 1) {
						m_ready = true;
					}
				} else {
					m_colors[i] = (unsigned int)i < m_rewards.size() ? 0xa0202020 : 0xffffffff;
				}
				break;
			}
			}
		}
	}
	if (m_stepPending && 0.0f < m_stepDelay) {
		m_stepDelay -= dt;
		if (m_stepDelay <= 0.0f) {
			if (m_step < 2) {
				++m_step;
				m_stepTime = 0.0f;
			}
			m_stepDelay = 0.0f;
			m_stepPending = false;
		}
	}
	m_stepTime += dt;
}

// 0x548d10
void FrGrandPrixAwardDlg::StartAwardTimer(float time, std::string name) {
	const char* timerName;
	if (!name.empty()) {
		timerName = name.c_str();
	} else {
		timerName = s_awardTimerName.c_str();
	}
	AddTimer(time, timerName);
}

// 0x548da0
void FrGrandPrixAwardDlg::OnRewardListDraw(PG_MSGARG param) {
	if (m_pRewardList) {
		m_pRewardList->Draw(GDI(), m_frameDt);
	}
}

// 0x549210 (CTimerEvent [1]: this is the CTimerEvent at +0x150)
void FrGrandPrixAwardDlg::OnTimer(void) {
	if (GetName() == s_awardTimerName) {
		m_state = 2;
		m_stepTimeLeft = m_stepDuration;
		ClearTimer(s_awardTimerName.c_str());
		ClearName();
	}
}

// 0x549290: the reward box layout from "grandprix_award_ui.lua" (JP
// 0xc1ff80 the script manager, 0xc1feb0 CreateScript, 0xc20530 RunFile,
// 0xc20240 GetTable, 0x470e60 / 0x470f20 the int / float field reads,
// 0x49a4e0 a field as a table, 0x49a000 the float index read). The JP
// does not release the script here.
void FrGrandPrixAwardDlg::LoadRewardBoxLayout(void) {
	lua_system::lua_script* script = lua_system::lua_script_manger::GetInstance()->CreateScript(false);
	if (!script) {
		return;
	}
	script->RunFile("grandprix_award_ui.lua");
	lua_system::lua_table_helper table = script->GetTable("grandprix_award_ui_rewardbox");
	m_rewardBoxWidth = table.GetValueByName<int>("box_width");
	const char* heightName = "box_height2";
	if (m_rewards.size() <= 5) {
		heightName = "box_height1";
	}
	m_rewardBoxHeight = table.GetValueByName<int>(heightName);
	m_rewardBoxX = lua_system::lua_table_helper(table.GetValueByName<lua_tinker::table>("box_pos")).GetValueByIndex<float>(1) + m_bgX;
	m_rewardBoxY = lua_system::lua_table_helper(table.GetValueByName<lua_tinker::table>("box_pos")).GetValueByIndex<float>(2) + m_bgY;
	m_rewardBoxPaddingW = table.GetValueByName<int>("padding_w");
	m_rewardBoxPaddingH = table.GetValueByName<int>("padding_h");
	m_stepDuration = table.GetValueByName<float>("rewardbox_show_delaytime");
}

// 0x549410: the item layout from "grandprix_award_ui.lua" (the calls of
// LoadRewardBoxLayout; the ints are stored as floats). The JP reads the
// table even when CreateScript failed, and releases the script
// (0xc1f430) at the end.
void CGrandPrixRewardList::LoadItemLayout(void) {
	lua_system::lua_script* script = lua_system::lua_script_manger::GetInstance()->CreateScript(false);
	if (script) {
		script->RunFile("grandprix_award_ui.lua");
	}
	lua_system::lua_table_helper table = script->GetTable("grandprix_award_ui_rewardbox_item");
	m_boxGapX = (float)table.GetValueByName<int>("RewardItemPaddingX");
	m_boxGapY = (float)table.GetValueByName<int>("RewardItemPaddingY");
	m_itemOffset.x = (float)table.GetValueByName<int>("ItemPosX");
	m_itemOffset.y = (float)table.GetValueByName<int>("ItemPosY");
	m_classOffset.x = (float)table.GetValueByName<int>("ItemOffsetX");
	m_classOffset.y = (float)table.GetValueByName<int>("ItemOffsetY");
	m_cardOffset.x = (float)table.GetValueByName<int>("ReadyPosX");
	m_cardOffset.y = (float)table.GetValueByName<int>("ReadyPosY");
	m_emptyOffset.x = (float)table.GetValueByName<int>("EmptyPosX");
	m_emptyOffset.y = (float)table.GetValueByName<int>("EmptyPosY");
	m_countUIOffset.x = (float)table.GetValueByName<int>("SmokePosX");
	m_countUIOffset.y = (float)table.GetValueByName<int>("SmokePosY");
	m_step0Delay = table.GetValueByName<float>("ReadyPlayTime");
	m_step1Delay = table.GetValueByName<float>("MaskPlayTime");
	m_countTextOffset.x = lua_system::lua_table_helper(table.GetValueByName<lua_tinker::table>("ItemDigitPos")).GetValueByIndex<float>(1);
	m_countTextOffset.y = lua_system::lua_table_helper(table.GetValueByName<lua_tinker::table>("ItemDigitPos")).GetValueByIndex<float>(2);
	m_itemBitmapOffset.x = lua_system::lua_table_helper(table.GetValueByName<lua_tinker::table>("ItemPeriodImgPos")).GetValueByIndex<float>(1);
	m_itemBitmapOffset.y = lua_system::lua_table_helper(table.GetValueByName<lua_tinker::table>("ItemPeriodImgPos")).GetValueByIndex<float>(2);
	m_boxWidth = table.GetValueByName<int>("ItemWidth");
	m_boxHeight = table.GetValueByName<int>("ItemHeight");
	lua_system::lua_script_manger::GetInstance()->ReleaseScript(script);
}

// 0x549a90
void FrGrandPrixAwardDlg::SetListRankVisible(bool show, bool bFade) {
	CGrandPrixRewardList* list = m_pRewardList.get();
	m_drawListRank = show;
	list->m_shown = show;
	list->SetPosition((int)list->m_x, (int)list->m_y);
	if (show) {
		g_audio->PlayBGM("Grandprix_Result_2.mp3", false, true);
	}
	for (unsigned int i = 0; i < m_listControls.size(); ++i) {
		const sAwardControl& control = m_listControls[i];
		bool applies = true;
		if (control.condition == sAwardControl::SHOW_ALWAYS) {
			applies = true;
		} else if (control.condition == sAwardControl::SHOW_TOP3) {
			applies = m_myRank <= 3;
		} else if (control.condition == sAwardControl::SHOW_OTHER) {
			applies = m_myRank > 3;
		}
		if (!bFade || !applies) {
			SetControlVisible(this, control.name.c_str(), applies && show);
		} else {
			SetControlFadeOut(this, control.name.c_str(), !show);
		}
	}
}

// 0x549c20
void FrGrandPrixAwardDlg::SetMyRankVisible(bool show, bool bFade) {
	m_drawMyRank = show;
	for (unsigned int i = 0; i < m_myRankControls.size(); ++i) {
		const sAwardControl& control = m_myRankControls[i];
		bool applies = true;
		if (control.condition == sAwardControl::SHOW_ALWAYS) {
			applies = true;
		} else if (control.condition == sAwardControl::SHOW_TOP3) {
			applies = m_myRank <= 3;
		} else if (control.condition == sAwardControl::SHOW_OTHER) {
			applies = m_myRank > 3;
		}
		if (m_myRank == 0) {
			applies = false;
		}
		if (!bFade || !applies) {
			SetControlVisible(this, control.name.c_str(), applies && show);
		} else {
			SetControlFadeOut(this, control.name.c_str(), !show);
		}
	}
}

// 0x549d80
void FrGrandPrixAwardDlg::ShowNextWinner(void) {
	std::string text;
	const Bitmap* bitmap = 0;
	switch (m_shownWinners) {
	case 0:
		if (m_matchRecord != 0 && m_myRank <= 3) {
			bitmap = g_pFresh->GetBitmap(m_matchRecord->m_trophyIcon[0]);
			text = MakeStr(Jp_GetTableText(0x7551), m_matchRecord->m_name);
		}
		break;
	case 1:
		if (0.0f < m_rewardRate) {
			text = MakeStr(Jp_GetTableText(0x754d), m_rewardGrade, (int)(m_rewardRate * 100.0));
		}
		break;
	case 2:
		if (m_grandPrixEntry != 0) {
			text = MakeStr(Jp_GetTableText(0x754e), m_grandPrixEntry->record->name);
		}
		break;
	}
	if (text.empty()) {
		OnWinnerShown();
		return;
	}
	FrFormBase* notice = CreateFormBase(g_pFresh->GetManager(), 0, "notify_ok2", 0);
	if (notice != 0) {
		if (bitmap != 0) {
			notice->AddImage(bitmap);
		}
		notice->SetMessage(text.c_str(), false);
		notice->SetCaption(Jp_GetTableText(0x7553));
		notice->Open((FRESH_RESULT_CALLBACK)&FrGrandPrixAwardDlg::OnWinnerNoticeResult, 0x803);
	}
}

// 0x54a2e0
void FrGrandPrixAwardDlg::OnProc(const float dt) {
	m_frameDt = dt;
	if (UpdateCloseDelay(dt)) {
		m_done = true;
	}
	if (m_state == 1) {
		if (UpdateMyRankDelay(dt)) {
			SetMyRankVisible(false, true);
		}
	} else if (m_state == 2) {
		if (UpdateStepTime(dt)) {
			SetListRankVisible(true, true);
		}
		if (!m_done && !m_nextPending && m_pRewardList->m_ready && UpdateNextDelay(dt)) {
			if (m_shownWinners < 3) {
				m_nextPending = true;
				m_nextDelay = 1.0f;
				ShowNextWinner();
			} else {
				m_done = true;
			}
		}
	}
}

// 0x54a530
FrArea* FrGrandPrixAwardDlg::FindArea(std::string name) {
	std::map<std::string, FrWnd*>::iterator it = m_controls.find(name);
	if (it != m_controls.end()) {
		return static_cast<FrArea*>(it->second);
	}
	return 0;
}

// 0x54a690
FrStatic* FrGrandPrixAwardDlg::FindStatic(std::string name) {
	std::map<std::string, FrWnd*>::iterator it = m_controls.find(name);
	if (it != m_controls.end()) {
		return static_cast<FrStatic*>(it->second);
	}
	return 0;
}

// 0x54a7f0
FrWnd* FrGrandPrixAwardDlg::FindControl(std::string name) {
	std::map<std::string, FrWnd*>::iterator it = m_controls.find(name);
	if (it != m_controls.end()) {
		return it->second;
	}
	return 0;
}

// 0x54a9f0: the calls of LoadRewardBoxLayout (0x483880 the string field
// read). The JP reads the tables even when CreateScript failed, and
// releases the script (0xc1f430) at the end.
bool FrGrandPrixAwardDlg::IsReady(void) {
	lua_system::lua_script* script = lua_system::lua_script_manger::GetInstance()->CreateScript(false);
	if (script) {
		script->RunFile("grandprix_award_ui.lua");
	}
	lua_system::lua_table_helper mainTable = script->GetTable("grandprix_award_ui_main");
	lua_system::lua_table_helper cutscene = script->GetTable("grandprix_award_ui_cutscene");
	int bgCount = mainTable.GetValueByName<int>("bg_file_count");
	const char* bgFormat = mainTable.GetValueByName<const char*>("bg_file_name");
	int bgX = (int)lua_system::lua_table_helper(mainTable.GetValueByName<lua_tinker::table>("bg_pos")).GetValueByIndex<float>(1);
	int bgY = (int)lua_system::lua_table_helper(mainTable.GetValueByName<lua_tinker::table>("bg_pos")).GetValueByIndex<float>(2);
	for (int i = 0; i < bgCount; ++i) {
		FrArea* area = FindArea(MakeStr("area_bg%d", i + 1));
		if (area != 0) {
			const Bitmap* bitmap = g_pFresh->GetBitmap(MakeStr(bgFormat, i + 1));
			if (bitmap != 0) {
				area->SetBgImg(bitmap);
				WRect rect = area->GetRect();
				rect.x = (float)bgX + rect.x;
				rect.y = (float)bgY + rect.y;
				area->SetRect(rect);
				if (i == 0) {
					m_bgX = rect.x;
					m_bgY = rect.y;
				}
			}
		}
	}
	for (int i = 1; i <= 4; ++i) {
		if (i <= 3) {
			m_charPicPos[i - 1].x = lua_system::lua_table_helper(mainTable.GetValueByName<lua_tinker::table>(MakeStr("char%d_pic_pos", i))).GetValueByIndex<float>(1) + m_bgX;
			m_charPicPos[i - 1].y = m_bgY + lua_system::lua_table_helper(mainTable.GetValueByName<lua_tinker::table>(MakeStr("char%d_pic_pos", i))).GetValueByIndex<float>(2);
			m_charNamePos[i - 1].x = lua_system::lua_table_helper(mainTable.GetValueByName<lua_tinker::table>(MakeStr("char%d_name_pos", i))).GetValueByIndex<float>(1) + m_bgX;
			m_charNamePos[i - 1].y = m_bgY + lua_system::lua_table_helper(mainTable.GetValueByName<lua_tinker::table>(MakeStr("char%d_name_pos", i))).GetValueByIndex<float>(2);
		}
		m_myRankPicPos[i - 1].x = lua_system::lua_table_helper(mainTable.GetValueByName<lua_tinker::table>(MakeStr("myrank%d_pic_pos", i))).GetValueByIndex<float>(1) + m_bgX;
		m_myRankPicPos[i - 1].y = m_bgY + lua_system::lua_table_helper(mainTable.GetValueByName<lua_tinker::table>(MakeStr("myrank%d_pic_pos", i))).GetValueByIndex<float>(2);
	}
	m_listRankPos.x = lua_system::lua_table_helper(mainTable.GetValueByName<lua_tinker::table>("rank_digit_pos")).GetValueByIndex<float>(1) + m_bgX;
	m_listRankPos.y = m_bgY + lua_system::lua_table_helper(mainTable.GetValueByName<lua_tinker::table>("rank_digit_pos")).GetValueByIndex<float>(2);
	m_listRankPos.scale = mainTable.GetValueByName<float>("rank_digit_scale");
	m_lastWinnerDelay = mainTable.GetValueByName<float>("close_delaytime");
	int trophyX = (int)(lua_system::lua_table_helper(cutscene.GetValueByName<lua_tinker::table>("trophy_pos")).GetValueByIndex<float>(1) + m_bgX);
	int trophyY = (int)(m_bgY + lua_system::lua_table_helper(cutscene.GetValueByName<lua_tinker::table>("trophy_pos")).GetValueByIndex<float>(2));
	float trophyLeft = (float)trophyX;
	m_trophyPicPos.x = trophyLeft + lua_system::lua_table_helper(cutscene.GetValueByName<lua_tinker::table>("trophy_pic_pos")).GetValueByIndex<float>(1);
	float trophyTop = (float)trophyY;
	m_trophyPicPos.y = trophyTop + lua_system::lua_table_helper(cutscene.GetValueByName<lua_tinker::table>("trophy_pic_pos")).GetValueByIndex<float>(2);
	for (int k = 1; k <= 2; ++k) {
		m_trophyAttrPos[k - 1].x = lua_system::lua_table_helper(cutscene.GetValueByName<lua_tinker::table>(MakeStr("trophy_attr%d_pos", k))).GetValueByIndex<float>(1);
		m_trophyAttrPos[k - 1].y = lua_system::lua_table_helper(cutscene.GetValueByName<lua_tinker::table>(MakeStr("trophy_attr%d_pos", k))).GetValueByIndex<float>(2);
		m_trophyOtherPos[k - 1].x = lua_system::lua_table_helper(cutscene.GetValueByName<lua_tinker::table>(MakeStr("trophy_other%d_pos", k))).GetValueByIndex<float>(1);
		m_trophyOtherPos[k - 1].y = lua_system::lua_table_helper(cutscene.GetValueByName<lua_tinker::table>(MakeStr("trophy_other%d_pos", k))).GetValueByIndex<float>(2);
	}
	m_myRank123Pos.x = trophyLeft + lua_system::lua_table_helper(cutscene.GetValueByName<lua_tinker::table>("rank_digit_pos")).GetValueByIndex<float>(1);
	m_myRank123Pos.y = trophyTop + lua_system::lua_table_helper(cutscene.GetValueByName<lua_tinker::table>("rank_digit_pos")).GetValueByIndex<float>(2);
	m_myRank4Pos.x = trophyLeft + lua_system::lua_table_helper(cutscene.GetValueByName<lua_tinker::table>("rank_other_digit_pos")).GetValueByIndex<float>(1);
	m_myRank4Pos.y = trophyTop + lua_system::lua_table_helper(cutscene.GetValueByName<lua_tinker::table>("rank_other_digit_pos")).GetValueByIndex<float>(2);
	m_myRank123Pos.scale = cutscene.GetValueByName<float>("rank_digit_scale");
	m_myRank4Pos.scale = cutscene.GetValueByName<float>("rank_other_digit_scale");
	m_myRankDelay = cutscene.GetValueByName<float>("cutscene_playtime");
	lua_system::lua_script_manger::GetInstance()->ReleaseScript(script);
	return true;
}

// 0x54b900
void FrGrandPrixAwardDlg::LayoutNinePatch(const char* format, float x, float y, int width, int height) {
	int cornerWidth = 0;
	int cornerHeight = 0;
	FrArea* first = FindArea(MakeStr(format, 0));
	if (first != 0) {
		cornerWidth = (int)first->GetRect().w;
		cornerHeight = (int)first->GetRect().h;
	}
	int endWidth;
	int endHeight;
	FrArea* last = FindArea(MakeStr(format, 8));
	if (last == 0) {
		endWidth = 0;
		endHeight = 0;
	} else {
		endWidth = (int)last->GetRect().w;
		endHeight = (int)last->GetRect().h;
	}
	int middleWidth = width - endWidth - cornerWidth;
	int middleHeight = height - endHeight - cornerHeight;
	for (int i = 0; i < 9; ++i) {
		FrArea* area = FindArea(MakeStr(format, i));
		if (area != 0) {
			WRect rect(x, y, area->GetRect().w, area->GetRect().h);
			unsigned int column = (unsigned int)i % 3;
			if (column != 0) {
				rect.x = x + (float)cornerWidth;
				if (column == 1) {
					rect.w = (float)middleWidth;
				} else if (column == 2) {
					rect.x = (float)middleWidth + rect.x;
				}
			}
			if (i > 2) {
				rect.y = y + (float)cornerHeight;
				if (i < 6) {
					rect.h = (float)middleHeight;
				} else {
					rect.y = (float)middleHeight + rect.y;
				}
			}
			area->SetRect(rect);
		}
	}
}

// 0x54baf0, 0x54c7d0 (jmp 0x54baf0): the trophy centred on the screen
// (g_view 0xf023e8 +0x19c / +0x1a0, 800 x 600 without a view), its two
// attribute pictures and the two "other" ones offset from the centred
// corner.
bool FrGrandPrixAwardDlg::SetupTrophies(void) {
	FrArea* trophy = FindArea("trophy");
	if (trophy != 0) {
		if (m_myRank > 3 || m_myRank == 0) {
			trophy->SetVisible(false);
		} else {
			WRect rect = trophy->GetRect();
			float screenWidth = g_view != 0 ? g_view->GetWidth() : 800.0f;
			rect.x = (float)(screenWidth * 0.5 - rect.w * 0.5);
			float screenHeight = g_view != 0 ? g_view->GetHeight() : 600.0f;
			rect.y = (float)(screenHeight * 0.5 - 0.5 * rect.h);
			trophy->SetRect(rect);
			trophy->SetBgImg(MakeStr("gp_awards_winner_%d", m_myRank));
			trophy->SetVisible(true);
		}
	}
	for (int k = 1; k <= 2; ++k) {
		FrArea* attr = FindArea(MakeStr("trophy_attr%d", k));
		if (attr != 0) {
			bool show;
			if (m_myRank > 3 || m_myRank == 0) {
				show = false;
			} else {
				WRect rect = attr->GetRect();
				float screenWidth = g_view != 0 ? g_view->GetWidth() : 800.0f;
				rect.x = (float)(screenWidth * 0.5 - rect.w * 0.5);
				float screenHeight = g_view != 0 ? g_view->GetHeight() : 600.0f;
				rect.y = (float)(screenHeight * 0.5 - 0.5 * rect.h);
				rect.x = m_trophyAttrPos[k - 1].x + rect.x;
				rect.y = rect.y + m_trophyAttrPos[k - 1].y;
				attr->SetRect(rect);
				show = true;
			}
			attr->SetVisible(show);
		}
		FrArea* other = FindArea(MakeStr("trophy_other%d", k));
		if (other != 0) {
			bool show;
			if (m_myRank <= 3) {
				show = false;
			} else {
				WRect rect = other->GetRect();
				float screenWidth = g_view != 0 ? g_view->GetWidth() : 800.0f;
				rect.x = (float)(screenWidth * 0.5 - rect.w * 0.5);
				float screenHeight = g_view != 0 ? g_view->GetHeight() : 600.0f;
				rect.y = (float)(screenHeight * 0.5 - 0.5 * rect.h);
				rect.x = m_trophyOtherPos[k - 1].x + rect.x;
				rect.y = m_trophyOtherPos[k - 1].y + rect.y;
				other->SetRect(rect);
				show = true;
			}
			other->SetVisible(show);
		}
	}
	return true;
}

// 0x54be50
CGrandPrixRewardList::CGrandPrixRewardList(void) {
	// (m_pCountUI: the shared_ptr's two words cleared)
	m_shown = false;
	m_ready = false;
	for (int i = 0; i < 10; ++i) {
		m_counts[i] = 0.0f;
		// (the original also zeroes a float pair at +0x104 + 8 * i here:
		// see the header)
		m_countFonts[i] = RF_SHARED_PTR<WTitleFontEx>((WTitleFontEx*)0);
		m_itemBitmaps[i] = 0;
		m_colors[i] = 0xffffffff;
		m_widths[i] = 0;
		m_heights[i] = 0;
		m_slotBitmaps[i].step[0] = 0;
		m_slotBitmaps[i].step[1] = 0;
		m_slotBitmaps[i].step[2] = 0;
	}
	m_step = 0;
	m_stepTime = 0.0f;
	m_stepPending = false;
	m_stepDelay = 0.0f;
	m_pBox = 0;
	m_boxGapX = 0.0f;
	m_pRewardBox = 0;
	m_boxGapY = 0.0f;
	m_itemCount = 0;
	m_itemOffset.x = 0.0f;
	m_boxWidth = 0;
	m_itemOffset.y = 0.0f;
	m_boxHeight = 0;
	m_classOffset.x = 0.0f;
	m_classOffset.y = 0.0f;
	m_cardOffset.x = 0.0f;
	m_cardOffset.y = 0.0f;
	m_emptyOffset.x = 0.0f;
	m_emptyOffset.y = 0.0f;
	m_countUIOffset.x = 0.0f;
	m_countUIOffset.y = 0.0f;
	m_countTextOffset.x = 0.0f;
	m_countTextOffset.y = 0.0f;
	m_itemBitmapOffset.x = 0.0f;
	m_itemBitmapOffset.y = 0.0f;
}

// 0x54c360
void FrGrandPrixAwardDlg::LayoutAwardControls(void) {
	// The original reuses two rectangles through the function (the bars'
	// height of the first one sizes the inner frame below, whichever
	// control set it last); the port starts them empty.
	WRect rect(0.0f, 0.0f, 0, 0);
	WRect rect2(0.0f, 0.0f, 0, 0);
	for (int i = 1; i <= 3; ++i) {
		FrArea* picture = FindArea(MakeStr("char%d", i));
		if (picture != 0) {
			rect = WRect(m_charPicPos[i - 1].x, m_charPicPos[i - 1].y, picture->GetRect().w, picture->GetRect().h);
			picture->SetRect(rect);
		}
		FrStatic* name = FindStatic(MakeStr("char%d_name", i));
		if (name != 0) {
			rect2 = WRect(m_charNamePos[i - 1].x, m_charNamePos[i - 1].y, name->GetRect().w, name->GetRect().h);
			name->SetRect(rect2);
		}
	}
	FrArea* myRank123 = FindArea(MakeStr("myrank123"));
	if (myRank123 != 0) {
		bool show;
		if (m_myRank >= 4 || m_myRank <= 0) {
			show = false;
		} else {
			rect = myRank123->GetRect();
			rect.x = m_myRankPicPos[m_myRank - 1].x;
			rect.y = m_myRankPicPos[m_myRank - 1].y;
			myRank123->SetRect(rect);
			myRank123->SetBgImg("gp_awards_me_in");
			show = true;
		}
		myRank123->SetVisible(show);
	}
	FrArea* myRank4 = FindArea(MakeStr("myrank4"));
	if (myRank4 != 0) {
		bool show;
		if (m_myRank <= 3) {
			show = false;
		} else {
			rect2 = WRect(m_myRankPicPos[3].x, m_myRankPicPos[3].y, myRank4->GetRect().w, myRank4->GetRect().h);
			myRank4->SetRect(rect2);
			myRank4->SetBgImg("gp_awards_me_out");
			show = true;
		}
		myRank4->SetVisible(show);
	}
	LayoutNinePatch("outback%d", m_rewardBoxX, m_rewardBoxY, m_rewardBoxWidth, m_rewardBoxHeight);
	FrArea* bar0 = FindArea("fbar0");
	if (bar0 != 0) {
		rect = bar0->GetRect();
		rect.x = (float)m_rewardBoxPaddingW + m_rewardBoxX;
		rect.y = (float)m_rewardBoxPaddingH + m_rewardBoxY;
		bar0->SetRect(rect);
		FrArea* bar1 = FindArea("fbar1");
		if (bar1 != 0) {
			rect2 = WRect(rect.w + rect.x, rect.y, bar1->GetRect().w, bar1->GetRect().h);
			bar1->SetRect(rect2);
		}
	}
	int x = (int)((float)m_rewardBoxPaddingW + m_rewardBoxX);
	int y = (int)((float)m_rewardBoxPaddingH + m_rewardBoxY + rect.h);
	int width = m_rewardBoxWidth - (m_rewardBoxPaddingW * 2 + m_rewardBoxPaddingW / 2);
	int height = (int)((float)m_rewardBoxHeight - (rect.h + (float)(m_rewardBoxPaddingH / 2 + m_rewardBoxPaddingH * 2)));
	LayoutNinePatch("inback%d", (float)x, (float)y, width, height);
	FrWnd* list = FindControl("rewardlist");
	if (list != 0) {
		if (m_pRewardList) {
			m_pRewardList->SetPosition(x, y);
		}
		list->SetRect(WRect((float)x, (float)y, width, height));
	}
}

// 0x54cab0
void CGrandPrixRewardList::SetRewards(const std::vector<sAwardReward>* rewards) {
	m_rewards = *rewards;
	m_itemCount = m_rewards.size() > 5 ? 10 : 5;
	LoadItemLayout();
	// The rolling count's eight frames: 64x64 cells of the texture's lower
	// half, four to a row.
	m_pCountUI = RF_SHARED_PTR<CSimpleUI>(new CSimpleUI("[emoti_object.png"));
	m_pCountUI->Add("0", WRect(0.0f, 128.0f, 64.0f, 64.0f), false);
	m_pCountUI->Add("1", WRect(64.0f, 128.0f, 64.0f, 64.0f), false);
	m_pCountUI->Add("2", WRect(128.0f, 128.0f, 64.0f, 64.0f), false);
	m_pCountUI->Add("3", WRect(192.0f, 128.0f, 64.0f, 64.0f), false);
	m_pCountUI->Add("4", WRect(0.0f, 192.0f, 64.0f, 64.0f), false);
	m_pCountUI->Add("5", WRect(64.0f, 192.0f, 64.0f, 64.0f), false);
	m_pCountUI->Add("6", WRect(128.0f, 192.0f, 64.0f, 64.0f), false);
	m_pCountUI->Add("7", WRect(192.0f, 192.0f, 64.0f, 64.0f), false);
	m_pBox = g_pFresh->GetBitmap("th_item_box_n");
	m_pRewardBox = g_pFresh->GetBitmap("th_item_box_o");
	for (int i = 0; i < m_itemCount; ++i) {
		m_slotBitmaps[i].step[0] = g_pFresh->GetBitmap("th_item_mark1");
		std::string name;
		if ((unsigned int)i < m_rewards.size()) {
			// JP 0xa17cf0 (CSharedDoc +0x20c): the KR FindCommonItem; the
			// JP icon name +0x49 is the KR m_icon.
			const IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(m_rewards[i].typeId);
			if (item != 0) {
				name = item->m_icon;
			}
		} else {
			name = "th_item_mark2";
		}
		const Bitmap* bitmap = g_pFresh->GetManager()->GetBitmap("ITEMS", name.c_str());
		if (bitmap != 0) {
			m_slotBitmaps[i].step[1] = bitmap;
			m_slotBitmaps[i].step[2] = bitmap;
			// The bitmap's size is read as unsigned.
			unsigned int width = (unsigned int)bitmap->bi->bmiHeader.biWidth;
			unsigned int height = (unsigned int)bitmap->bi->bmiHeader.biHeight;
			if ((unsigned int)i < m_rewards.size()) {
				if (width > (unsigned int)m_boxWidth || height > (unsigned int)m_boxHeight) {
					float scale;
					if ((int)(width - height) < 0) {
						if (height <= (unsigned int)m_boxHeight) {
							scale = (float)height / (float)m_boxHeight;
						} else {
							scale = (float)m_boxHeight / (float)height;
						}
					} else {
						if (width <= (unsigned int)m_boxWidth) {
							scale = (float)width / (float)m_boxWidth;
						} else {
							scale = (float)m_boxWidth / (float)width;
						}
					}
					m_widths[i] = (int)((float)width * scale);
					m_heights[i] = (int)(scale * (float)height);
				} else {
					m_widths[i] = (int)width;
					m_heights[i] = (int)height;
				}
			} else {
				m_widths[i] = (int)width;
				m_heights[i] = (int)height;
			}
		}
		if ((unsigned int)i < m_rewards.size()) {
			int kind = m_rewards[i].kind;
			if (kind == sAwardReward::SHOW_COUNT) {
				m_countFonts[i] = RF_SHARED_PTR<WTitleFontEx>(new CWindFont);
			} else if (kind == sAwardReward::SHOW_BITMAP) {
				m_itemBitmaps[i] = g_pFresh->GetBitmap("recycle_in_period");
			}
		}
	}
}

// 0x54d110
bool FrGrandPrixAwardDlg::OnInit(void) {
	JpFrWnd_RaiseFlagF1aac8();
	// JP CSharedDoc +0x90: the KR m_value90 (the KR CGolfRule sends packet
	// 6 under the same flag).
	if (!CSharedDoc::Instance()->m_value90) {
		CSharedDoc::Instance()->m_value90 = 1;
		WSendPacket packet((enumClientPacket)6);
		packet.EncodeBuffer(JpSharedDoc_GetMyGameStatistics(), 0x109);
		packet.Send(SEND_TO_DEFAULT);
	}
	EnableDrag(false);

	CGrandPrixDocument* container = JpContents_GetGrandPrixContainer();
	sJpGrandPrixEntry* entry = JpGrandPrixContainer_FindEntry(container, JpSharedDoc_GetGrandPrixKey());
	if (JpSharedDoc_GetGrandPrixKey() != 0) {
		for (unsigned int i = 0; i < JpGrandPrixContainer_GetResultCount(container); ++i) {
			const sJpGrandPrixResult* result = JpGrandPrixContainer_GetResult(container, i);
			int rank = result->rank;
			if (rank > 3) {
				// JP +0x8cb: the KR MyUID().
				if (result->uid == MyUID()) {
					const sRivalData* player = FindRoomPlayer(result->uid);
					if (player != 0) {
						m_myRank = player->m_rank;
					}
				}
				continue;
			}
			// JP sChar +0x49: the KR m_icon (the JP name is longer).
			const IFF_STRUCT::sChar* character = 0;
			std::string name;
			if (result->isRoomPlayer != 0) {
				const sRivalData* player = FindRoomPlayer(result->uid);
				if (player != 0) {
					// The JP looks the picture up here with
					// FindChar(player +0x1ac): the KR sRivalData has no
					// character type id, the picture stays unset.
					if (result->uid == MyUID()) {
						m_myRank = player->m_rank;
					}
					name = player->m_nickname;
				}
			} else {
				const sJpGrandPrixNpc* npc = JpItemManager_FindGrandPrixNpc(result->uid);
				if (npc != 0) {
					const IFF_STRUCT::sChar* found = ItemManager()->FindChar(npc->characterId | 0x4000000);
					if (found != 0) {
						character = found;
					}
					name = npc->name;
				}
			}
			FrArea* picture = FindArea(MakeStr("char%d", rank));
			if (picture != 0 && character != 0) {
				picture->SetBgImg(MakeStr("w_%s", character->m_icon));
			}
			FrStatic* text = FindStatic(MakeStr("char%d_name", rank));
			if (text != 0) {
				text->SetWindowTextA(name.c_str());
			}
		}
		if (entry != 0) {
			sJpGrandPrixName* record = entry->record;
			if (record != 0) {
				for (int k = 0; k < 5; ++k) {
					if (record->rewardTypeIds[k] != 0) {
						sAwardReward reward;
						reward.typeId = record->rewardTypeIds[k];
						reward.kind = record->rewardKinds[k];
						reward.count = record->rewardCounts[k];
						m_rewards.push_back(reward);
					}
				}
			}
			if (m_myRank != 0) {
				const std::vector<sJpGrandPrixRankReward>* rows = JpItemManager_FindGrandPrixRankRewards(JpSharedDoc_GetGrandPrixTypeId());
				if (rows != 0) {
					for (unsigned int i = 0; i < rows->size(); ++i) {
						const sJpGrandPrixRankReward& row = (*rows)[i];
						if (m_myRank == row.rank) {
							for (int k = 0; k < 5; ++k) {
								if (row.typeIds[k] != 0) {
									sAwardReward reward;
									reward.typeId = row.typeIds[k];
									reward.kind = row.kinds[k];
									reward.count = row.counts[k];
									m_rewards.push_back(reward);
								}
							}
							break;
						}
					}
				}
			}
		}
	}

	m_matchRecord = JpItemManager_FindMatch(data_e7512c);
	uint32_t typeId = JpSharedDoc_GetGrandPrixTypeId();
	int grade = (int)((typeId >> 19) & 0x7f);
	if ((unsigned int)(grade - 1) <= 2 && entry != 0) {
		m_rewardRate = JpGrandPrixContainer_GetRewardRate(container, grade, typeId);
		m_rewardGrade = grade;
		m_grandPrixName = entry->record->name;
	} else {
		m_rewardRate = -1.0f;
		m_rewardGrade = 0;
	}
	if (entry != 0) {
		if (entry->remainingCount > 0) {
			m_grandPrixEntry = 0;
		} else {
			m_grandPrixEntry = JpGrandPrixContainer_FindEntryByType(container, JpSharedDoc_GetGrandPrixTypeId());
		}
	} else {
		m_grandPrixEntry = 0;
	}

	LoadRewardBoxLayout();
	LayoutAwardControls();
	SetupTrophies();
	m_pRewardList->SetRewards(&m_rewards);
	m_pRankFont = RF_SHARED_PTR<WTitleFontEx>(new CGrandPrixRankFont);
	m_state = 1;
	SetListRankVisible(false, false);
	SetMyRankVisible(true, true);
	g_audio->PlayBGM("Grandprix_Result_1.mp3", true, true);
	return true;
}

// 0x54da40
FrGrandPrixAwardDlg::~FrGrandPrixAwardDlg(void) {
	if (CTimerManager::IsInstantiated()) {
		ClearTimer(s_awardTimerName.c_str());
	}
	data_e7512c = 0;
	ClearSubForm();
}

// 0x54dc90
FrGrandPrixAwardDlg::FrGrandPrixAwardDlg(void) {
	SetSubForm(this);
	m_done = false;
	m_pRewardList = RF_SHARED_PTR<CGrandPrixRewardList>(new CGrandPrixRewardList);
	m_frameDt = 0.0f;
	m_myRank = 0;
	m_drawListRank = false;
	m_drawMyRank = false;
	m_nextDelay = 2.0f;
	m_myRankDelay = 0.0f;
	m_stepTimeLeft = 0.0f;
	m_rewards.clear();
	m_animTime = 0.0f;
	m_animScale = 1.0f;
	m_shownWinners = 0;
	m_closeDelay = 45.0f;
	m_nextPending = false;
	m_state = 0;
	m_listControls.push_back(sAwardControl(std::string("area_bg1"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("area_bg2"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("area_bg3"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("area_bg4"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("outback0"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("outback1"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("outback2"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("outback3"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("outback4"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("outback5"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("outback6"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("outback7"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("outback8"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("inback0"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("inback1"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("inback2"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("inback3"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("inback4"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("inback5"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("inback6"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("inback7"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("inback8"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("fbar0"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("fbar1"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("rewardlist"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("char1"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("char2"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("char3"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("char1_name"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("char2_name"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("char3_name"), sAwardControl::SHOW_ALWAYS));
	m_listControls.push_back(sAwardControl(std::string("myrank123"), sAwardControl::SHOW_TOP3));
	m_listControls.push_back(sAwardControl(std::string("myrank4"), sAwardControl::SHOW_OTHER));
	m_myRankControls.push_back(sAwardControl(std::string("trophy"), sAwardControl::SHOW_TOP3));
	m_myRankControls.push_back(sAwardControl(std::string("trophy_attr1"), sAwardControl::SHOW_TOP3));
	m_myRankControls.push_back(sAwardControl(std::string("trophy_attr2"), sAwardControl::SHOW_TOP3));
	m_myRankControls.push_back(sAwardControl(std::string("trophy_other1"), sAwardControl::SHOW_OTHER));
	m_myRankControls.push_back(sAwardControl(std::string("trophy_other2"), sAwardControl::SHOW_OTHER));
}

// 0x54f310
FrGrandPrixAwardDlg* CreateGrandPrixAwardDlg(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent) {
	return CreateForm<FrGrandPrixAwardDlg>(manager, owner, name, parent);
}

// 0x54f3e0
IObject* FrGrandPrixAwardDlgMakeInstance(void) {
	return new FrGrandPrixAwardDlg;
}

// 0x54f470
FrGrandPrixAwardDlg* OpenGrandPrixAwardDlg(FrWndManager* manager, FrCmdTarget* owner) {
	return CreateGrandPrixAwardDlg(manager, owner, "GrandPrix_Award", 0);
}

// 0x671310
bool FrGrandPrixAwardDlg::OnWinnerNoticeResult(int ret, FrForm* form) {
	if (s_pAwardDlg != 0) {
		s_pAwardDlg->OnWinnerShown();
	}
	return true;
}

// 0x9d8b70 (thiscall on the JP CItemManager, which it does not read; the
// switch is the byte table 0x9d8c0c on (low 20 bits - 0x15) into the jump
// table 0x9d8bf4, plus 0x2ac tested first).
int JpItemManager_GetSpecialItemKind(uint32_t typeId) {
	if ((typeId & 0xfc000000) != 0x18000000 || (typeId & 0x1f00000) != 0) {
		return 0;
	}
	if ((typeId & 0x2000000) == 0) {
		return 0;
	}
	switch (typeId & 0xfffff) {
	case 0x15: case 0x16: case 0x17:
	case 0x1d: case 0x1e:
	case 0x2b: case 0x2c: case 0x2d: case 0x2e:
	case 0x3a: case 0x3c:
	case 0x4e:
		return 1;
	case 0x28: case 0x29: case 0x2a:
		return 2;
	case 0x30: case 0x33: case 0x34:
	case 0xa3:
	case 0x2ac:
		return 3;
	case 0x36:
	case 0x63: case 0x64: case 0x65: case 0x66: case 0x67: case 0x68: case 0x69:
	case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6e: case 0x6f: case 0x70:
	case 0x71: case 0x72: case 0x73: case 0x74: case 0x75:
	case 0xd2:
		return 4;
	case 0x3d: case 0x3f:
	case 0x53:
		return 5;
	default:
		return 0;
	}
}
