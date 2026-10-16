// Original: d:\build\custom\temp\code\client\projectg\questinfodlg.cpp
#include <atlbase.h>	// stand-in for the precompiled header: _AtlWinModule initializer
#include "QuestInfoDlg.h"
#include "Packet.h"
#include "SharedDoc.h"
#include "UccManager.h"
#include "commonutil.h"
#include "ObjectFactory.h"
#include "actor.h"
#include "frarea.h"
#include "frbutton.h"
#include "fredit.h"
#include "fresh.h"
#include "frlistbox.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "bitmap.h"
#include "wfont.h"
#include "wresrcmng.h"
#include "wview.h"

#include <string.h>

// File names of the title fonts of the two dialogs (nameless data).
extern "C" char* data_a852d8[];
extern "C" char* data_a852dc[];

// A quest's completion bit and its reward. The same table as TutorialMain's
// s_questRewards (each unit has its own copy at a different address, so the
// original most likely defined it in a header both include).
struct sQuestReward {
	unsigned long m_mask;   // 0x00
	unsigned long m_typeId; // 0x04, 0 for the quest with a gender-dependent reward (index 7)
	int m_count;            // 0x08
};
CHECK_SIZE(sQuestReward, 0x0C);

static const sQuestReward s_questRewards[20] = {
	{ 0x1, 0x1A00000F, 3 }, { 0x2, 0x18000007, 3 }, { 0x4, 0x18000005, 3 },
	{ 0x8, 0x18000008, 3 }, { 0x10, 0x1A000010, 500 }, { 0x20, 0x18000004, 3 },
	{ 0x40, 0x1A000010, 500 }, { 0x80, 0, 1 }, { 0x100, 0x1A00000F, 3 },
	{ 0x200, 0x18000028, 1 }, { 0x400, 0x18000006, 1 }, { 0x800, 0x18000007, 5 },
	{ 0x1000, 0x18000000, 4 }, { 0x2000, 0x18000001, 4 }, { 0x8000, 0x18000005, 3 },
	{ 0x10000, 0x18000005, 3 }, { 0x20000, 0x18000005, 3 }, { 0x40000, 0x18000005, 3 },
	{ 0x80000, 0x18000005, 3 }, { 0x80000, 0x18000005, 3 },
};

// A tutorial quest's texts (questinfodlg.cpp:31-46, 0x14 bytes; nameless
// data at 0xAF4C08, filled by the unit's dynamic initializer).
struct sQuestText {
	unsigned char m_index;      // 0x00
	bool m_flag;                // 0x01 (always false)
	const char *m_title;        // 0x04, the quest's name ("%d : %s" in FrQuestInfoDlg)
	const char *m_npc;          // 0x08, the NPC who gives it
	const char *m_explane;      // 0x0C, the description
	const char *m_message;      // 0x10, the goal
};
CHECK_SIZE(sQuestText, 0x14);

// questinfodlg.cpp:31
static const char* const s_questNpc = "\xba\xc0\xb4\xd9\xb8\xae";
// The first quest's name: a named constant, which makes the first element's
// name a dynamic store as in the original.
static const char* const s_questTitle0 = "\xc5\xac\xb7\xb4\xbc\xb1\xc5\xc3";

static sQuestText s_questTexts[14] = {
	{ 0, false, s_questTitle0, s_questNpc, "\xb8\xf0\xb8\xa7\xc1\xf6\xb1\xe2 \xb8\xc5\xbb\xe7\xbf\xa3 \xb1\xe2\xc3\xca\xb0\xa1 \xc6\xb0\xc6\xb0\xc7\xd8\xbe\xdf \xc7\xcf\xb4\xc2 \xb9\xfd! \xc6\xce\xbe\xdf\xc0\xc7 \xb1\xe2\xba\xbb\xb0\xfa \xc5\xac\xb7\xb4 \xbc\xb1\xc5\xc3 \xb9\xe6\xb9\xfd\xc0\xbb \xbe\xcb\xbe\xc6\xba\xb8\xc0\xda!", "\xc7\xa5\xbd\xc3\xb5\xc8 \xb0\xf7\xbf\xa1 \xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1\x0a\xbf\xc5\xb1\xe2\xb1\xe2" },
	{ 1, false, "\xc5\xb8\xb1\xb8\xb9\xe6\xb9\xfd", s_questNpc, "\xc6\xce\xbe\xdf! \xb8\xda\xc1\xf8 \xbc\xa6\xc0\xbb \xc7\xcf\xb4\xc2 \xb9\xe6\xb9\xfd\xbf\xa1 \xb4\xeb\xc7\xd8 \xbe\xcb\xbe\xc6\xba\xb8\xc0\xda!", "\xc7\xa5\xbd\xc3\xb5\xc8 \xb0\xf7\xc0\xb8\xb7\xce\x0a\xbc\xa6 \xc3\xc4\xba\xb8\xb1\xe2" },
	{ 2, false, "\xc6\xce\xbe\xdf\xb0\xd4\xc0\xcc\xc1\xf6", s_questNpc, "\xb4\xf5 \xb8\xd6\xb8\xae! \xb4\xf5 \xb0\xad\xc7\xcf\xb0\xd4! \xc6\xce\xbe\xdf\xc4\xde\xba\xb8\xb0\xd4\xc0\xcc\xc1\xf6\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xcf\xbf\xa9 \xc6\xc4\xbf\xf6\xbc\xa6\xc0\xbb \xc3\xc4\xba\xb8\xc0\xda!", "\xc6\xce\xbe\xdf\xc4\xde\xba\xb8\xb0\xd4\xc0\xcc\xc1\xf6\xb8\xa6\x0a\xbb\xe7\xbf\xeb\xc7\xcf\xbf\xa9 \xc6\xc4\xbf\xf6\xbc\xa6 \xc4\xa1\xb1\xe2" },
	{ 3, false, "\xc6\xdb\xc6\xc3\xc0\xc7 \xb1\xe2\xba\xbb", s_questNpc, "\xc8\xa6\xc0\xc7 \xb8\xb6\xb9\xab\xb8\xae! \xc6\xdb\xc6\xc3\xbf\xa1 \xb4\xeb\xc7\xd8 \xb9\xe8\xbf\xf6\xba\xb8\xc0\xda", "\xc6\xdb\xc6\xc3 \xb9\xe8\xbf\xec\xb1\xe2" },
	{ 4, false, "\xbe\xc6\xc0\xcc\xc5\xdb\xbb\xe7\xbf\xeb", s_questNpc, "\xbe\xd1, \xc0\xcc\xb7\xb1 \xbd\xc5\xb1\xe2\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc? \xbe\xc6\xc0\xcc\xc5\xdb \xbb\xe7\xbf\xeb\xb9\xfd\xc0\xbb \xbe\xcb\xbe\xc6\xba\xb8\xc0\xda!", "\xbe\xc6\xc0\xcc\xc5\xdb \xbb\xe7\xbf\xeb\xc0\xb8\xb7\xce\x0a\xc6\xc4\xbf\xf6\xbc\xa6 \xc4\xa1\xb1\xe2" },
	{ 5, false, "\xc1\xf6\xb8\xe9\xbc\xd3\xbc\xba", s_questNpc, "\xc7\xae\xb9\xe7, \xb8\xf0\xb7\xa1\xb9\xe7, \xb9\xd9\xb4\xd9, \xbf\xac\xb8\xf8... \xb4\xd9\xbe\xe7\xc7\xd1 \xc1\xf6\xb8\xe9\xbf\xa1 \xb4\xeb\xc7\xd8 \xb0\xf8\xba\xce\xc7\xcf\xc0\xda!", "\xc4\xfb\xc1\xee\xb7\xce \xbe\xcb\xbe\xc6\xba\xb8\xb4\xc2\x0a\xc1\xf6\xb8\xe9\xc0\xc7 \xbc\xd3\xbc\xba" },
	{ 6, false, "\xbd\xba\xc4\xda\xbe\xee\xb0\xe8\xbb\xea", s_questNpc, "\xbe\xcb\xbd\xf6\xb4\xde\xbd\xf6? \xc6\xce\xbe\xdf\xc0\xc7 \xc1\xa1\xbc\xf6 \xb0\xe8\xbb\xea\xb9\xfd\xc0\xbb \xbe\xcb\xbe\xc6\xba\xb8\xc0\xda!", "\xc4\xfb\xc1\xee\xb7\xce \xbe\xcb\xbe\xc6\xba\xb8\xb4\xc2\x0a\xc6\xce\xbe\xdf\xc0\xc7 \xc1\xa1\xbc\xf6" },
	{ 7, false, "4\xc8\xa6\xbf\xa1\xbc\xad \xc8\xa6\xc0\xce\xc7\xcf\xb1\xe2", s_questNpc, "\xc3\xca\xba\xb8 \xb4\xeb\xc5\xbb\xc3\xe2! \xc1\xf6\xb1\xdd\xb1\xee\xc1\xf6 \xb9\xe8\xbf\xee \xb8\xf0\xb5\xe7 \xb0\xcd\xc0\xbb \xc0\xc0\xbf\xeb\xc7\xcf\xbf\xa9 \xbd\xc7\xbd\xc0 \xb5\xb5\xc0\xfc!", "\xc6\xc4 4\xc8\xa6\xbf\xa1\xbc\xad\x0aPar\xc0\xcc\xbb\xf3 \xb1\xe2\xb7\xcf\xc7\xcf\xb1\xe2" },
	{ 8, false, "\xb0\xf8 \xbf\xc5\xb1\xe2\xb1\xe2", s_questNpc, "\xb5\xb5\xc0\xfa\xc8\xf7 \xbc\xa6\xc0\xbb \xc4\xa5 \xbc\xf6 \xbe\xf8\xb4\xc2 \xbb\xf3\xc8\xb2\xc0\xcc\xb6\xf3\xb8\xe9? \xb0\xf8\xbf\xc5\xb1\xe2\xb1\xe2 \xb1\xe2\xb4\xc9\xc0\xbb \xb9\xe8\xbf\xf6\xba\xb8\xc0\xda!", "\xb0\xf8\xbf\xc5\xb1\xe2\xb1\xe2 \xb1\xe2\xb4\xc9 \xbe\xcb\xbe\xc6\xba\xb8\xb1\xe2" },
	{ 9, false, "\xb0\xf8 \xb1\xe2\xbf\xef\xb1\xe2", s_questNpc, "\xc1\xf6\xb8\xe9\xc0\xc7 \xb1\xe2\xbf\xef\xb1\xe2\xb8\xa6 \xc0\xd0\xb0\xed \xb4\xf5\xbf\xed \xc1\xa4\xb1\xb3\xc7\xd1 \xbc\xa6\xbf\xa1 \xb5\xb5\xc0\xfc\xc7\xcf\xc0\xda!", "\xc1\xf6\xb8\xe9\xc0\xc7 \xb1\xe2\xbf\xef\xb1\xe2\xb8\xa6 \xc0\xd0\xb4\xc2 \xb9\xfd" },
	{ 10, false, "\xb9\xd9\xb6\xf7 \xc0\xd0\xb1\xe2", s_questNpc, "\xb9\xd9\xb6\xf7\xc0\xbb \xc0\xd0\xb0\xed \xb0\xf8\xc0\xc7 \xb1\xcb\xc0\xfb\xc0\xbb \xbf\xb9\xc3\xf8\xc7\xcf\xb6\xf3!", "\xb9\xd9\xb6\xf7\xc0\xc7 \xbc\xbc\xb1\xe2\xbf\xcd \xb9\xe6\xc7\xe2 \xbe\xcb\xbe\xc6\xba\xb8\xb1\xe2" },
	{ 11, false, "\xc5\xb8\xc1\xa1\xc0\xcc\xb5\xbf", s_questNpc, "\xb0\xf8\xc0\xc7 \xc5\xb8\xc1\xa1\xc0\xbb \xc1\xb6\xc0\xfd\xc7\xcf\xbf\xa9 \xb1\xcb\xc0\xfb\xc0\xc7 \xb0\xa2\xb5\xb5\xb8\xa6 \xb9\xd9\xb2\xe3\xba\xb8\xc0\xda!", "\xb0\xf8\xc0\xc7 \xc5\xb8\xc1\xa1 \xc1\xb6\xc0\xfd\xc7\xcf\xb1\xe2" },
	{ 12, false, "\xc6\xc4\xbf\xf6\xbd\xba\xc7\xc9", s_questNpc, "\xb0\xad\xb7\xc2\xc7\xd1 \xbd\xba\xc7\xc9! \xc6\xc4\xbf\xf6\xbd\xba\xc7\xc9\xbf\xa1 \xb4\xeb\xc7\xd8 \xbe\xcb\xbe\xc6\xba\xb8\xc0\xda.", "\xc6\xc4\xbf\xf6 \xbd\xba\xc7\xc9 \xbc\xa6 \xc0\xcd\xc8\xf7\xb1\xe2" },
	{ 13, false, "\xc6\xc4\xbf\xf6\xc4\xbf\xba\xea", s_questNpc, "\xb0\xc5\xb4\xeb\xc7\xd1 \xc0\xe5\xbe\xd6\xb9\xb0\xb5\xb5 \xbf\xe4\xb8\xae\xc1\xb6\xb8\xae \xba\xf1\xc4\xd1\xb0\xa1\xb4\xc2 \xc6\xc4\xbf\xf6 \xc4\xbf\xba\xea\xbc\xa6\xc0\xbb \xb9\xe8\xbf\xf6\xba\xb8\xc0\xda!", "\xc0\xe5\xbe\xd6\xb9\xb0\xc0\xbb \xc7\xc7\xc7\xd8 \xb8\xf1\xc7\xa5\xc1\xf6\xc1\xa1\xc0\xb8\xb7\xce \xb0\xf8 \xba\xb8\xb3\xbb\xb1\xe2" },
};

// questinfodlg.cpp:48 (run-time type and factory registration of FrQuestInfoDlg)
const WRTTI FrQuestInfoDlg::m_RTTI("FrQuestInfoDlg", &FrForm::m_RTTI);

IObject* FrQuestInfoDlgMakeInstance(void) {
	return new FrQuestInfoDlg;
}

struct __impFrQuestInfoDlg {
	__impFrQuestInfoDlg(void) {
		ObjectFactory().AddObjectFunctor(FrQuestInfoDlgMakeInstance, "FrQuestInfoDlg");
		ActorFactory().AddActorClass("FrQuestInfoDlg");
	}
};
static __impFrQuestInfoDlg s_registerFrQuestInfoDlg;

// questinfodlg.cpp:50
const sFRESH_MSGMAP* FrQuestInfoDlg::GetMessageMap(void) const {
	return &FrQuestInfoDlg::_MsgMap;
}

sFRESH_MSGMAP FrQuestInfoDlg::_MsgMap = { &FrForm::_MsgMap, &FrQuestInfoDlg::_MsgEntries[0] };

sFRESH_ENTRY FrQuestInfoDlg::_MsgEntries[] = {
	// questinfodlg.cpp:53
	{ "quest_info", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrQuestInfoDlg::OnQuestInfoInit },
	// questinfodlg.cpp:54
	{ "reward_item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrQuestInfoDlg::OnRewardItemListInit },
	// questinfodlg.cpp:55
	{ "reward_item", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrQuestInfoDlg::OnRewardItemListOwnerDraw },
	// questinfodlg.cpp:56
	{ "name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrQuestInfoDlg::OnQuestNameEditInit },
	// questinfodlg.cpp:57
	{ "message", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrQuestInfoDlg::OnQuestMessageEditInit },
	// questinfodlg.cpp:58
	{ "explane", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrQuestInfoDlg::OnQuestExplaneEditInit },

	// questinfodlg.cpp:60
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// questinfodlg.cpp:63
FrQuestInfoDlg::FrQuestInfoDlg(void) {
	m_pRewardItemList = 0;
	m_pItemFrame = 0;
	m_bRewardItemAdded = false;
	m_questIndex = 0;
	m_pQuestName = 0;
	m_pQuestMessage = 0;
	m_pQuestExplane = 0;

	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));
	info.filename = data_a852d8;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:";

	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&info);
}

// questinfodlg.cpp:91
FrQuestInfoDlg::~FrQuestInfoDlg(void) {
	if (g_resrcmng && m_pTitleFont) { g_resrcmng->Release(m_pTitleFont); m_pTitleFont = 0; }
}

// questinfodlg.cpp:96
void FrQuestInfoDlg::OnQuestInfoInit(int) {
}

// questinfodlg.cpp:101
void FrQuestInfoDlg::OnRewardItemListInit(intptr_t param) {
	m_pRewardItemList = DYNAMIC_CAST(FrListBox, param);
	if (m_pRewardItemList) {
		m_pRewardItemList->SetPushSound(0);

		m_pRewardItemList->ClearItem();

		m_pItemFrame = g_pFresh->RegisterBitmap("re_itemframe_n");
	}
}

// questinfodlg.cpp:115
void FrQuestInfoDlg::OnRewardItemListOwnerDraw(int param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem) {

		FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();

		// Offset of the item's slot: one reward is centred, two sit side by side.
		float offsetX = 0.0f;
		float offsetY = 0.0f;
		if (m_pRewardItemList->m_itemList.size() > 1) {

			switch (pItem->idx) {
			case 0:
				offsetY = 7.0f;
				offsetX = 20.0f;
				break;
			case 1:
				offsetY = 7.0f;
				offsetX = 20.0f;
				break;
			}
		} else {
			offsetY = 7.0f;
			offsetX = 100.0f;
		}

		pGI->SetTextStyle(0);

		if (m_pItemFrame) {
			pGI->DrawTexture(m_pItemFrame, WRect(offsetX + pItem->pos.x, offsetY + pItem->pos.y, (float)m_pItemFrame->Width(), (float)m_pItemFrame->Height()), 0xFFFFFFFF, 0);
		}

		unsigned long typeId = (unsigned long)pItem->pData;
		if (typeId) {









			const char* icon = "caddie_no";
			IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);

			if (pCommon) {

				pGI->SetTextStyle(1);
				pGI->Print(pItem->pos + WPoint(160.0f, 7.0f) + WPoint(offsetX, offsetY), 2, "%s", pCommon->m_name);
				pGI->SetTextStyle(0);

				icon = pCommon->m_icon;
			}

			const Bitmap* pIcon = g_pFresh->GetBitmap(icon);
			if (pIcon) {
				float iconX;
				float iconY;
				switch (typeId >> 26) {
				case 1:
				case 2:
				case 8:
				case 9:
				case 15:
				case 16:
				case 28:
					iconX = 7.0f;
					iconY = 2.0f;
					break;
				case 11:
					iconX = 9.0f;
					iconY = 12.0f;
					break;
				case 14:
					if ((typeId & 0x3C00000) == 0x800000) {
						iconX = 7.0f;
						iconY = 12.0f;
					} else {
						iconX = 16.0f;
						iconY = 27.0f;
					}
					break;
				default:
					iconX = 10.0f;
					iconY = 20.0f;
					break;
				}

				pGI->DrawTexture(pIcon, WRect(iconX + pItem->pos.x + offsetX, iconY + pItem->pos.y + offsetY, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);

				// The reward's count, right-aligned at x + 70.
				int count = s_questRewards[m_questIndex].m_count;
				char* text = MakeStr("%d", count);
				float x = pItem->pos.x + 70.0f;

				for (unsigned int i = 0; i < strlen(text); i++) {
					x -= m_pTitleFont->GetCharWidthA(g_view, text[i] - '0');
				}

				m_pTitleFont->Print(g_view, x + offsetX - 10.0f, offsetY + pItem->pos.y + 50.0f, text, 0, 0xFFFFFFFF, 0);
			}
		}
	}
}

// questinfodlg.cpp:217
void FrQuestInfoDlg::OnQuestNameEditInit(intptr_t param) {
	m_pQuestName = DYNAMIC_CAST(FrEdit, param);
}

// questinfodlg.cpp:222
void FrQuestInfoDlg::OnQuestMessageEditInit(intptr_t param) {
	m_pQuestMessage = DYNAMIC_CAST(FrEdit, param);
}

// questinfodlg.cpp:227
void FrQuestInfoDlg::OnQuestExplaneEditInit(intptr_t param) {
	m_pQuestExplane = DYNAMIC_CAST(FrEdit, param);
}

// questinfodlg.cpp:232
void FrQuestInfoDlg::SetQuestExplane(int index) {
	m_pQuestName->AddText(MakeStr("%d : %s", index + 1, s_questTexts[index].m_title), false, true);
	m_pQuestMessage->AddText(s_questTexts[index].m_message, false, true);
	m_pQuestExplane->AddText(s_questTexts[index].m_explane, false, true);

	if (s_questRewards[index].m_typeId != 0) {
		m_pRewardItemList->AddItem((void*)s_questRewards[index].m_typeId);
		m_bRewardItemAdded = true;
	} else if (index == 7) {
		if ((WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().eventFlags >> 1 & 1) % 2 == 0) {
			m_pRewardItemList->AddItem((void*)0x800A010);
		} else {
			m_pRewardItemList->AddItem((void*)0x8048009);
		}
		m_bRewardItemAdded = true;
	}

	m_questIndex = index;
}

// questinfodlg.cpp:274 (run-time type and factory registration of FrQuestGift)
const WRTTI FrQuestGift::m_RTTI("FrQuestGift", &FrForm::m_RTTI);

IObject* FrQuestGiftMakeInstance(void) {
	return new FrQuestGift;
}

struct __impFrQuestGift {
	__impFrQuestGift(void) {
		ObjectFactory().AddObjectFunctor(FrQuestGiftMakeInstance, "FrQuestGift");
		ActorFactory().AddActorClass("FrQuestGift");
	}
};
static __impFrQuestGift s_registerFrQuestGift;

// questinfodlg.cpp:276
const sFRESH_MSGMAP* FrQuestGift::GetMessageMap(void) const {
	return &FrQuestGift::_MsgMap;
}

sFRESH_MSGMAP FrQuestGift::_MsgMap = { &FrForm::_MsgMap, &FrQuestGift::_MsgEntries[0] };

sFRESH_ENTRY FrQuestGift::_MsgEntries[] = {
	// questinfodlg.cpp:278
	{ "reward_item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrQuestGift::OnGiftListInit },
	// questinfodlg.cpp:279
	{ "reward_item", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrQuestGift::OnGiftListOwnerDraw },
	// questinfodlg.cpp:280
	{ "reward_item", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrQuestGift::OnGiftListLBtnDown },

	// questinfodlg.cpp:282
	{ "select", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrQuestGift::OnSelectInit },

	// questinfodlg.cpp:284
	{ "itemback4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrQuestGift::OnItemBack4 },
	// questinfodlg.cpp:285
	{ "itemback5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrQuestGift::OnItemBack5 },
	// questinfodlg.cpp:286
	{ "itemback6", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrQuestGift::OnItemBack6 },
	// questinfodlg.cpp:287
	{ "itemback7", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrQuestGift::OnItemBack7 },
	// questinfodlg.cpp:288
	{ "itemback8", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrQuestGift::OnItemBack8 },
	// questinfodlg.cpp:289
	{ "itemback9", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrQuestGift::OnItemBack9 },

	// questinfodlg.cpp:291
	{ "ok", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrQuestGift::OnOkInit },

	// questinfodlg.cpp:293
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// questinfodlg.cpp:297
FrQuestGift::FrQuestGift(void) {
	m_pGiftList = 0;
	m_pSelect = 0;
	m_pOk = 0;

	memset(m_pItemBack, 0, sizeof(m_pItemBack));
	m_questIndex = 0;
	m_pItemFrame = 0;
	m_pNotifyForm = 0;
	m_bGiftAdded = false;

	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));
	info.filename = data_a852dc;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:";

	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&info);
}

// questinfodlg.cpp:326
FrQuestGift::~FrQuestGift(void) {
	if (g_resrcmng && m_pTitleFont) { g_resrcmng->Release(m_pTitleFont); m_pTitleFont = 0; }
}

// questinfodlg.cpp:331
void FrQuestGift::OnGiftListInit(intptr_t param) {
	m_pGiftList = DYNAMIC_CAST(FrListBox, param);

	m_pItemFrame = g_pFresh->RegisterBitmap("re_itemframe_n");
}

// questinfodlg.cpp:338
void FrQuestGift::OnGiftListOwnerDraw(int param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem) {

		FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();

		// Offset of the item's slot by the number of gifts.
		float offsetX = 0.0f;
		float offsetY = 0.0f;
		if (m_pGiftList->m_itemList.size() == 2) {
			switch (pItem->idx) {
			case 0:
				offsetY = 7.0f;
				offsetX = 5.0f;
				break;
			case 1:
				offsetY = 7.0f;
				offsetX = 70.0f;
				break;
			}
		} else if (m_pGiftList->m_itemList.size() == 3) {
			switch (pItem->idx) {
			case 0:
				offsetY = 7.0f;
				offsetX = 5.0f;
				break;
			case 1:
				offsetY = 7.0f;
				offsetX = 70.0f;
				break;
			case 2:
				offsetX = 5.0f;
				offsetY = 10.0f;
				break;
			}
		} else {
			offsetY = 7.0f;
			offsetX = 100.0f;
		}

		pGI->SetTextStyle(0);

		if (m_pItemFrame) {
			pGI->DrawTexture(m_pItemFrame, WRect(offsetX + pItem->pos.x, offsetY + pItem->pos.y, (float)m_pItemFrame->Width(), (float)m_pItemFrame->Height()), 0xFFFFFFFF, 0);
		}

		unsigned long typeId = (unsigned long)pItem->pData;
		if (typeId) {









			const char* icon = "caddie_no";
			IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);

			if (pCommon) {

				pGI->SetTextStyle(1);
				pGI->Print(pItem->pos + WPoint(160.0f, 7.0f) + WPoint(offsetX, offsetY), 2, "%s", pCommon->m_name);
				pGI->SetTextStyle(0);

				icon = pCommon->m_icon;
			}

			const Bitmap* pIcon = g_pFresh->GetBitmap(icon);
			if (pIcon) {
				float iconX;
				float iconY;
				switch (typeId >> 26) {
				case 2:
					iconX = 7.0f;
					iconY = -5.0f;
					break;
				case 1:
				case 8:
				case 9:
				case 15:
				case 16:
				case 28:
					iconX = 7.0f;
					iconY = 2.0f;
					break;
				case 11:
					iconX = 9.0f;
					iconY = 12.0f;
					break;
				case 14:
					if ((typeId & 0x3C00000) == 0x800000) {
						iconX = 7.0f;
						iconY = 12.0f;
					} else {
						iconX = 16.0f;
						iconY = 27.0f;
					}
					break;
				default:
					iconX = 10.0f;
					iconY = 20.0f;
					break;
				}

				pGI->DrawTexture(pIcon, WRect(iconX + pItem->pos.x + offsetX, iconY + pItem->pos.y + offsetY, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);

				if (m_bGiftAdded != true) {

					// The gift's count, right-aligned at x + 70 (none for a single item).
					int count = s_questRewards[m_questIndex].m_count;

					if (count != 1) {

						char* text = MakeStr("%d", count);
						float x = pItem->pos.x + 70.0f;

						for (unsigned int i = 0; i < strlen(text); i++) {
							x -= m_pTitleFont->GetCharWidthA(g_view, text[i] - '0');
						}

						m_pTitleFont->Print(g_view, x + offsetX - 10.0f, offsetY + pItem->pos.y + 50.0f, text, 0, 0xFFFFFFFF, 0);
					}
				}
			}
		}
	}
}

// questinfodlg.cpp:470
void FrQuestGift::OnGiftListLBtnDown(void) {
	if (!m_pGiftList || m_bGiftAdded) return;

	// A single reward is given without a choice.
	if (m_pGiftList->m_itemList.size() == 1) return;

	FrListItem* pItem = m_pGiftList->GetItemUnderCursor();
	if (!pItem) return;

	unsigned long typeId = (unsigned long)pItem->pData;
	if (!typeId) return;

	WSendPacket packet((enumClientPacket)0xA8);   // choice of the quest reward (no named id)
	packet.Encode4(typeId);
	packet.Send(SEND_TO_DEFAULT);

	m_pNotifyForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
	// "The chosen gift has been requested."
	m_pNotifyForm->SetMessage("\xbc\xb1\xc5\xc3\xc7\xd1 \xbc\xb1\xb9\xb0\xc0\xbb \xbf\xe4\xc3\xbb\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", false);
	m_pNotifyForm->Open((FRESH_RESULT_CALLBACK)&FrQuestGift::OnNotifyResult,
		WPoint((g_view->GetWidth() - m_pNotifyForm->GetRect().w) * 0.5f, 80.0f), 2);
	m_pNotifyForm->EnableDrag(false);
	m_pNotifyForm->MoveCursor("ok");
}

// questinfodlg.cpp:497
void FrQuestGift::OnSelectInit(intptr_t param) {
	m_pSelect = DYNAMIC_CAST(FrArea, param);

	if (m_pSelect) {
		m_pSelect->SetVisible(false);
	}
}

// questinfodlg.cpp:507
void FrQuestGift::OnItemBack4(intptr_t param) {
	m_pItemBack[0] = DYNAMIC_CAST(FrArea, param);
}

// questinfodlg.cpp:512
void FrQuestGift::OnItemBack5(intptr_t param) {
	m_pItemBack[1] = DYNAMIC_CAST(FrArea, param);
}

// questinfodlg.cpp:517
void FrQuestGift::OnItemBack6(intptr_t param) {
	m_pItemBack[2] = DYNAMIC_CAST(FrArea, param);
}

// questinfodlg.cpp:522
void FrQuestGift::OnItemBack7(intptr_t param) {
	m_pItemBack[3] = DYNAMIC_CAST(FrArea, param);
}

// questinfodlg.cpp:527
void FrQuestGift::OnItemBack8(intptr_t param) {
	m_pItemBack[4] = DYNAMIC_CAST(FrArea, param);
}

// questinfodlg.cpp:532
void FrQuestGift::OnItemBack9(intptr_t param) {
	m_pItemBack[5] = DYNAMIC_CAST(FrArea, param);
}

// questinfodlg.cpp:537
void FrQuestGift::OnOkInit(intptr_t param) {
	m_pOk = DYNAMIC_CAST(FrButton, param);
}

// questinfodlg.cpp:542
bool FrQuestGift::OnNotifyResult(int ret, FrForm* pForm) {
	Close(FrOK, true);
	return true;
}

// questinfodlg.cpp:548
void FrQuestGift::SetQuestExplane(int index) {
	if (s_questRewards[index].m_typeId != 0) {
		m_pGiftList->AddItem((void*)s_questRewards[index].m_typeId);
	} else if (index == 7) {
		if (m_pSelect) {
			m_pSelect->SetVisible(true);
		}

		if ((WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().eventFlags >> 1 & 1) % 2 == 0) {
			m_pGiftList->AddItem((void*)0x800A010);
		} else {
			m_pGiftList->AddItem((void*)0x8048009);
		}
	}

	m_questIndex = index;
}

// questinfodlg.cpp:579
void FrQuestGift::SetData(int index) {
	switch (index) {
	case 0:
		m_pGiftList->AddItem((void*)0x10000012);
		m_pGiftList->AddItem((void*)0x1C000000);
		break;
	case 1:
		if ((WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().eventFlags >> 1 & 1) % 2 == 0) {
			m_pGiftList->AddItem((void*)0x8006012);
			m_pGiftList->AddItem((void*)0x8010001);
		} else {
			m_pGiftList->AddItem((void*)0x8044013);
			m_pGiftList->AddItem((void*)0x804E002);
		}

		m_pGiftList->AddItem((void*)0x18000027);

		SetSize();
		break;
	}

	m_bGiftAdded = true;
}

// questinfodlg.cpp:622
void FrQuestGift::SetSize(void) {
	int height = m_pGiftList->GetItemHeight();

	WRect rect = m_rect;
	rect.h += (float)height;
	SetRect(rect);

	rect = m_pGiftList->GetRect();
	rect.h = (float)height + rect.h;
	m_pGiftList->SetRect(rect);

	WPoint pt;
	rect = m_pOk->GetRect();
	pt.x = rect.x;
	pt.y = (float)height + rect.y;
	m_pOk->MoveWindow(pt);

	int i;
	for (i = 0; i < 3; i++) {
		rect = m_pItemBack[i]->GetRect();
		rect.h = rect.h + (height + 10);
		m_pItemBack[i]->SetRect(rect);
	}

	for (i = 3; i < 6; i++) {
		rect = m_pItemBack[i]->GetRect();
		pt.x = rect.x;
		pt.y = (float)height + 10.0f + rect.y;
		m_pItemBack[i]->MoveWindow(pt);
	}
}
