// Original: d:\build\custom\temp\code\client\projectg\avataruserlistdlg.cpp
#include "AvatarUserListDlg.h"
#include "ObjectFactory.h"
#include "frlistbox.h"
#include "mousecursor.h"
#include "SharedDoc.h"
#include "globalgamedefine.h"
#include "shared/classdefine.h"
#include "Localize.h"
#include "ProjectG.h"
#include "NetResourceManager.h"
#include "commonutil.h"
#include "fresh.h"
#include "frgraphicinterface.h"
#include "frwndmanager.h"
#include "bitmap.h"

// avataruserlistdlg.cpp:13 (run-time type and factory registration of FrAvatarUserListDlg)
const WRTTI FrAvatarUserListDlg::m_RTTI("FrAvatarUserListDlg", &FrForm::m_RTTI);

IObject* FrAvatarUserListDlgMakeInstance(void) {
	return new FrAvatarUserListDlg;
}

struct __sFrAvatarUserListDlg {
	__sFrAvatarUserListDlg(void) {
		ObjectFactory().AddObjectFunctor(FrAvatarUserListDlgMakeInstance, "FrAvatarUserListDlg");
	}
};
static __sFrAvatarUserListDlg s_registerFrAvatarUserListDlg;

// avataruserlistdlg.cpp:15
const sFRESH_MSGMAP* FrAvatarUserListDlg::GetMessageMap(void) const {
	return &FrAvatarUserListDlg::_MsgMap;
}

sFRESH_MSGMAP FrAvatarUserListDlg::_MsgMap = { &FrForm::_MsgMap, &FrAvatarUserListDlg::_MsgEntries[0] };

sFRESH_ENTRY FrAvatarUserListDlg::_MsgEntries[] = {
	// avataruserlistdlg.cpp:17
	{ "userlist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAvatarUserListDlg::OnUserListInit },
	// avataruserlistdlg.cpp:18
	{ "userlist", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAvatarUserListDlg::OnUserListOwnerDraw },
	// avataruserlistdlg.cpp:19
	{ "userlist", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrAvatarUserListDlg::OnUserListLBtnDown },
	// avataruserlistdlg.cpp:20
	{ "userlist", FRCMD_RBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAvatarUserListDlg::OnUserListRBtnUp },
	// avataruserlistdlg.cpp:22
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// avataruserlistdlg.cpp:27
FrAvatarUserListDlg::FrAvatarUserListDlg(void) {
	m_pUserList = 0;
	m_alpha = 0;
	m_selected = -1;
}

// avataruserlistdlg.cpp:34
FrAvatarUserListDlg::~FrAvatarUserListDlg(void) {
}

// avataruserlistdlg.cpp:39
void FrAvatarUserListDlg::OnUserListInit(intptr_t param) {
	m_pUserList = (FrListBox*)param;
	m_pUserList->UseRightButton(true);

	m_pBitmaps[0] = m_pUserList->GetBitmap("i_male");
	m_pBitmaps[1] = m_pUserList->GetBitmap("i_female");
	m_pBitmaps[2] = m_pUserList->GetBitmap("i_male_02");
	m_pBitmaps[3] = m_pUserList->GetBitmap("i_female_02");
	m_pBitmaps[4] = m_pUserList->GetBitmap("i_male_03");
	m_pBitmaps[5] = m_pUserList->GetBitmap("i_female_03");
	m_pBitmaps[6] = m_pUserList->GetBitmap("i_male_manner");
	m_pBitmaps[7] = m_pUserList->GetBitmap("i_female_manner");
	m_pBitmaps[8] = m_pUserList->GetBitmap("i_male_angel");
	m_pBitmaps[9] = m_pUserList->GetBitmap("i_female_angel");
	m_pBitmaps[10] = m_pUserList->GetBitmap("ingame");
}

// avataruserlistdlg.cpp:81
void FrAvatarUserListDlg::OnUserListOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;

	if (pItem) {
		sSlotInfo* pSlot = (sSlotInfo*)pItem->pData;

		if (pSlot) {
			FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

			// the sex icon: angel, manner, or the plain icon of the user flag
			const Bitmap* pSex;
			if (pSlot->m_flags57.flag0) {
				pSex = m_pBitmaps[8 + pSlot->m_attributeBits.userFlag % 2];
			} else if (pSlot->m_flags55.flag0) {
				pSex = m_pBitmaps[6 + pSlot->m_attributeBits.userFlag % 2];
			} else {
				pSex = m_pBitmaps[pSlot->m_attributeBits.userFlag];
			}

			WRect rect(pItem->pos.x, pItem->pos.y, pSex->Width(), pSex->Height());

			if (pSlot->m_uid == (unsigned long)m_selected) {
				pGDI->Box(WRect(rect.x, rect.y, (float)m_pUserList->GetItemWidth(), (float)m_pUserList->GetItemHeight()), (unsigned long)(unsigned char)(m_alpha * 255.0f) << 24 | 0x9B9BFF, 0, 0.0f);
			}

			pGDI->DrawTexture(pSex, rect, 0xFFFFFFFF, 0);

			if (WSingleton<CProjectG>::Instance()->HidePrivacy()) {
				return;
			}

			// the equipped title's icon, else the level's
			const Bitmap* pLevel;
			if (pSlot->m_titleTypeId) {
				IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(pSlot->m_titleTypeId);
				if (pSkin) {
					pLevel = g_pFresh->GetManager()->GetBitmap("TITLES", pSkin->m_icon);
				} else {
					pLevel = g_pFresh->GetManager()->GetBitmap("LEVELS", MakeStr("level_%03d", pSlot->m_level + 1));
				}
			} else {
				pLevel = g_pFresh->GetManager()->GetBitmap("LEVELS", MakeStr("level_%03d", pSlot->m_level + 1));
			}

			if (pLevel) {
				pGDI->DrawTexture(pLevel, WRect(pItem->pos.x + 50.0f - float2int(pLevel->Width() * 0.5f), pItem->pos.y + 13.0f - float2int(pLevel->Height() * 0.5f), (float)pLevel->Width(), (float)pLevel->Height()), 0xFFFFFFFF, 0);
			}

			if (pSlot->m_guildUID && !pSlot->IsIdentity(0x14)) {
				const Bitmap* pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(pSlot->m_emblemName);

				if (pEmblem) {
					pGDI->DrawTexture(pEmblem, WRect(pItem->pos.x + 85.0f, pItem->pos.y + 7.0f - 6.0f, (float)pEmblem->Width(), (float)pEmblem->Height()), 0xFFFFFFFF, 0);
				}
			}

			// administrators and observers are drawn half transparent
			unsigned long color = ((unsigned char)pSlot->m_identity & 0x14) ? 0x80FFFFFF : 0xFFFFFFFF;

			if (pItem->underCursor) {
				pGDI->SetTextColor(0xFFFFFFFF, 0xFF808080);
				pGDI->SetTextStyle(2);
			} else {
				pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
				pGDI->SetTextStyle(0);
			}

			g_pFresh->GetManager()->PrintText(WPoint(pItem->pos.x + 112.0f, pItem->pos.y + 7.0f), 0, pSlot->m_nickname, -1.0f, color);
		}
	}
}

// avataruserlistdlg.cpp:196
void FrAvatarUserListDlg::OnUserListLBtnDown(void) {
}

// avataruserlistdlg.cpp:210
void FrAvatarUserListDlg::OnUserListRBtnUp(void) {
}

// avataruserlistdlg.cpp:234
void FrAvatarUserListDlg::UpdateUserList(void) {
	m_pUserList->ClearItem();

	for (std::list<sSlotInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.end(); ++it) {
		if (IsLocalContent((localContentType_t)10)) {
			std::map<unsigned long, sBriefUserInfo>::iterator user = WSingleton<CSharedDoc>::Instance()->m_userMap.find(sSlotInfo(*it).m_uid);

			// a hidden administrator is listed only to administrators
			if (!AdminMode()) {
				if ((*it).m_identity & 4) {
					if (!(*user).second.invisible) {
						continue;
					}
				}
			}
		} else if (!AdminMode() && ((*it).m_identity & 0x14)) {
			continue;
		}

		m_pUserList->AddItem(&*it);
	}
}

// avataruserlistdlg.cpp:263
void FrAvatarUserListDlg::OnProc(const float dt) {
	bool inArea = CMouseCursor::Instance()->InArea(m_rect);

	if (inArea) {
		m_alpha += dt * 2.0f;
		if (m_alpha > 1.0f) {
			m_alpha = 1.0f;
		}
	} else {
		m_alpha -= dt * 2.0f;
		if (m_alpha < 0.6f) {
			m_alpha = 0.6f;
		}
	}

	SetAlpha2ToChild(m_alpha);
}
