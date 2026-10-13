// Original: d:\build\custom\temp\code\client\projectg\familymain.cpp
#include "FamilyMain.h"
#include "SharedDoc.h"
#include "TreasureDlg.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frwnd.h"
#include "frarea.h"
#include "frbutton.h"
#include "frviewer.h"
#include "frlistbox.h"
#include "ObjectFactory.h"
#include "actor.h"
#include "shared/globalgamedefine.h"

#include "EquipDlg.h"
#include "exhibition.h"
#include "playerdlg.h"
#include "frgraphicinterface.h"
#include "bitmap.h"
#include "commonutil.h"
#include "mousecursor.h"
#include "UccManager.h"
#include "InputManager.h"

#include <algorithm>
#include <string>
#include "shared/ItemManager.h"
#include "shared/classdefine.h"
#include "frdesktop.h"
#include "RankingDlg.h"
#include "user_info.h"
#include "parttidlist.h"
#include "cardsystem.h"
#include "client/Wangreal/include/wview.h"
#include "TaskManager.h"
#include "Packet.h"

unsigned char CFamilyMain::m_mapIndex = 0xFF;
unsigned long CFamilyMain::ms_itemslotBackup[4][10];

// familymain.cpp:47
CFamilyMain::cFamily::cFamily(void)
	: m_exhibition(0), m_partTidList(0), m_charInfo(), m_statBar(0), m_petKind(0) {
	memset(&m_charInfo, 0, sizeof(m_charInfo));
	memset(&m_userEquip, 0, sizeof(m_userEquip));
	memset(&m_mascot, 0, sizeof(m_mascot));
	memset(&m_caddieInfo, 0, sizeof(m_caddieInfo));
	memset(&m_itemInfo, 0, sizeof(m_itemInfo));
}

// familymain.cpp:56
CFamilyMain::cFamily::~cFamily(void) {
	if (m_exhibition) {
		delete m_exhibition;
		m_exhibition = 0;
	}
	if (m_partTidList) {
		delete m_partTidList;
		m_partTidList = 0;
	}
	if (m_statBar) {
		delete m_statBar;
		m_statBar = 0;
	}
}

// familymain.cpp:63
void CFamilyMain::cFamily::SetItemSlot(unsigned long* pItemSlot) {
	memcpy(m_userEquip.itemSlots, pItemSlot, sizeof(m_userEquip.itemSlots));
}

// familymain.cpp:70
void CFamilyMain::cFamily::ConvertUserInfo(sUserInfo& info) {
	info.m_slot = 0;
	memcpy(&info.m_caddieInfo, &m_caddieInfo, sizeof(m_caddieInfo));
	memcpy(&info.m_characterInfo, &m_charInfo, sizeof(m_charInfo));

	sClubSetRecord clubSet;
	clubSet.id = m_itemInfo.m_serial;
	clubSet.typeId = m_itemInfo.m_typeId;

	memcpy(clubSet.statLevel, m_itemInfo.m_clubStatLevel, sizeof(clubSet.statLevel));
	info.m_clubSet = clubSet;

	memcpy(info.m_nickname, &WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo(), 0x10D);
	// CSharedDoc::m_myMapStatistics (0x694 = m_myTrophy + 0xBA) is protected
	// and CSharedDoc does not befriend this class yet.
	memcpy(info.m_mapStatistics, (char*)&WSingleton<CSharedDoc>::Instance()->m_myTrophy + 0xBA, sizeof(info.m_mapStatistics));
	info.m_mascot = m_mascot;

	memcpy(&info.m_statShots, &WSingleton<CSharedDoc>::Instance()->m_myStatShots, sizeof(sPangYaUserStatistics));
	memcpy(&info.m_trophy, &WSingleton<CSharedDoc>::Instance()->m_myTrophy, sizeof(info.m_trophy));
	memcpy(&info.m_equipHead, &m_userEquip, sizeof(m_userEquip));
}

// familymain.cpp:91
const WPoint& CFamilyMain::cFamily::GetFramePos(void) const {
	return m_framePos;
}

// familymain.cpp:93
const sUserEquip& CFamilyMain::cFamily::GetUserEquip(void) const {
	return m_userEquip;
}

// familymain.cpp:95
void CFamilyMain::cFamily::SetUserEquip(int category, unsigned long id) {
	switch (category) {
	case 1: {
		std::map<unsigned int, sCharacterInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_characterMap.find(id);
		if (it != WSingleton<CSharedDoc>::Instance()->m_characterMap.end()) {
			m_userEquip.characterId = id;
			m_charInfo = it->second;
			if (IsLocalContent((localContentType_t)0x51) && IsLocalContent((localContentType_t)0x64))
				WSingleton<CCardManager>::Instance()->m_characterInfo[m_petKind + 1] = m_charInfo;
			PetRebuild();
		}
		return;
	}

	case 0x10: {
		std::map<unsigned int, sMascotInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myMascotMap.find(id);
		if (it != WSingleton<CSharedDoc>::Instance()->m_myMascotMap.end()) {
			m_userEquip.mascotId = id;
			m_mascot = it->second;
		} else {
			memset(&m_mascot, 0, sizeof(m_mascot));
		}
		break;
	}

	case 7: {
		std::map<unsigned int, sCaddieInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_caddieMap.find(id);
		if (it != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end()) {
			m_userEquip.caddieId = id;
			m_caddieInfo = it->second;
		} else {
			memset(&m_caddieInfo, 0, sizeof(m_caddieInfo));
		}
		break;
	}

	case 5:
		m_userEquip.ballTypeId = id;
		return;

	case 4: {
		std::map<unsigned int, sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myItemMap.find(id);
		if (it == WSingleton<CSharedDoc>::Instance()->m_myItemMap.end())
			return;
		m_userEquip.clubSetId = id;
		m_itemInfo = it->second;
		break;
	}

	default:
		return;
	}

	SetStatus();
}

// familymain.cpp:158
void CFamilyMain::cFamily::Init(const WRect& rect, std::vector<int>& petList, unsigned char petKind) {
	m_rect = rect;
	const Bitmap* frame = g_pFresh->GetBitmap("family_info_frm");
	if (frame) {
		m_framePos.x = m_rect.x + (m_rect.w - frame->Width()) * 0.5f;
		m_framePos.y = m_rect.y + m_rect.h - frame->Height();
	}

	m_modelPos.x = m_framePos.x + 45.0f;
	m_modelPos.y = m_framePos.y + 50.0f;
	m_petKind = petKind;

	MakePet(petList);
}

// familymain.cpp:175
void CFamilyMain::cFamily::Process(float dt) {
	if (!m_exhibition)
		return;

	m_statBar->Process(dt);

	if (!FrStaticCast<FrWnd>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop"))) {
		// The pet turns with the mouse unless a dialog sits over it.
		m_exhibition->SetEnableRotate(true);
		if (g_pFresh->GetManager()->GetDesktop()->FindChildByName("treasuremap")
			|| CRankingInfo::Instance()->GetDlg()
			|| (CUserInfo::Instance() && CUserInfo::Instance()->GetDlg()))
			m_exhibition->SetEnableRotate(false);

		m_exhibition->Process(dt, false);
	}
}

// familymain.cpp:194
void CFamilyMain::cFamily::Render(void) {
	if (!FrStaticCast<FrWnd>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop"))) {
		// The pet is drawn unless its model is being rebuilt or a dialog sits over it.
		if (!m_anotherHand.IsWorking(0)
			&& !CRankingInfo::Instance()->GetDlg()
			&& !(CUserInfo::Instance() && CUserInfo::Instance()->GetDlg())) {
			m_anotherHand.Enter();
			if (m_exhibition)
				m_exhibition->Display(0.0f, 0.0f);
			m_anotherHand.Leave();
		}
	}

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	const Bitmap* frame = g_pFresh->GetBitmap("family_info_frm");
	if (frame)
		pGDI->DrawTexture(frame, WRect(m_framePos, WSize((float)frame->Width(), (float)frame->Height())), 0xFFFFFFFF, 0);

	const Bitmap* charFrame = g_pFresh->GetBitmap("family_info_char");
	IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(m_charInfo.typeId);
	if (item && charFrame) {
		const Bitmap* text = g_pFresh->GetBitmap(MakeStr("text_%s", item->m_icon));
		if (text) {
			WPoint pos = m_framePos;
			pos.y -= 30.0f;
			pos.x += (m_rect.w - charFrame->Width()) * 0.5f;
			pGDI->DrawTexture(charFrame, WRect(pos.x, pos.y, (float)charFrame->Width(), (float)charFrame->Height()), 0xFFFFFFFF, 0);

			pos.y += (charFrame->Height() - text->Height()) * 0.5f;
			pos.x += (charFrame->Width() - text->Width()) * 0.5f;
			pGDI->DrawTexture(text, WRect(pos.x, pos.y, (float)text->Width(), (float)text->Height()), 0xFFFFFFFF, 0);
		}
	}

	m_statBar->Render(m_modelPos);
}

// familymain.cpp:244
void CFamilyMain::cFamily::MakePet(std::vector<int>& petList) {
	static int index = 0;

	int count = (int)WSingleton<CSharedDoc>::Instance()->m_characterMap.size();
	if (++index >= count) {
		index = 0;
		std::random_shuffle(petList.begin(), petList.end());
	}

	int pick = petList[index];

	std::map<unsigned int, sCharacterInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin();
	for (int i = 0; i < pick; i++)
		++it;

	sCharacterInfo* pCharInfo = &it->second;
	m_charInfo = *pCharInfo;
	m_userEquip = WSingleton<CSharedDoc>::Instance()->m_myEquip;

	m_userEquip.characterId = pCharInfo->id;
	m_userEquip.caddieId = CEquipHelper::Instance()->GetEquipItemGuid(CEquipHelper::eCategoryCaddie, true);
	m_userEquip.clubSetId = CEquipHelper::Instance()->GetEquipItemGuid(CEquipHelper::eCategoryClub, true);
	m_userEquip.ballTypeId = CEquipHelper::Instance()->GetEquipItemTid(CEquipHelper::eCategoryBall, true);
	m_userEquip.mascotId = CEquipHelper::Instance()->GetEquipItemGuid(CEquipHelper::eCategoryMascot, true);

	std::map<unsigned int, sItemInfo>::iterator itItem = WSingleton<CSharedDoc>::Instance()->m_myItemMap.find(m_userEquip.clubSetId);
	if (itItem != WSingleton<CSharedDoc>::Instance()->m_myItemMap.end())
		m_itemInfo = itItem->second;

	std::map<unsigned int, sCaddieInfo>::iterator itCaddie = WSingleton<CSharedDoc>::Instance()->m_caddieMap.find(m_userEquip.caddieId);
	if (itCaddie != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end())
		m_caddieInfo = itCaddie->second;

	std::map<unsigned int, sMascotInfo>::iterator itMascot = WSingleton<CSharedDoc>::Instance()->m_myMascotMap.find(m_userEquip.mascotId);
	if (itMascot != WSingleton<CSharedDoc>::Instance()->m_myMascotMap.end())
		m_mascot = itMascot->second;

	if (IsLocalContent((localContentType_t)0x51) && IsLocalContent((localContentType_t)0x64)) {
		unsigned char slot = m_petKind + 1;
		WSingleton<CCardManager>::Instance()->m_characterInfo[slot] = m_charInfo;
		WSingleton<CCardManager>::Instance()->m_playerIndexMap[slot] = slot;
	}

	SetStatus();

	m_anotherHand.Enter();

	m_exhibition = new CExhibition(m_rect.x + m_rect.w * 0.5f, m_rect.y + m_rect.h * 0.5f - 70.0f, 0.35f, false);
	m_exhibition->SetArea(m_rect.w, m_rect.h);

	if (pCharInfo->typeId == 0x4000005 || pCharInfo->typeId == 0x4000009)
		m_exhibition->SetDistance(11.0f, true, false);
	else
		m_exhibition->SetDistance(13.0f, true, false);

	m_partTidList = new CPartTidList;
	if (!m_partTidList->SetTids(pCharInfo, 0xFF) || !m_partTidList->IsComboValid())
		m_partTidList->SetDefaultTids();

	m_exhibition->SetModel(*m_partTidList, pCharInfo->auxParts.tids, 0, 3.0f, 0);
	if (WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().eventFlags & 0x10)
		m_exhibition->AttachAngelWingFx();

	m_anotherHand.Leave();
}

// familymain.cpp:313
void CFamilyMain::cFamily::SetStatus(void) {
	if (IsLocalContent((localContentType_t)0x51) && IsLocalContent((localContentType_t)0x64)) {
		WSingleton<CCardManager>::Instance()->CalcCardPeriodAndStatus();
		WSingleton<CCardManager>::Instance()->SetPlayerIndex(m_petKind);
	}

	if (m_statBar) {
		delete m_statBar;
		m_statBar = 0;
	}
	m_statBar = new CStatBar(9.0f, 6.0f);
	CItemManager* pItemManager = ItemManager();
	unsigned char level[5] = { 0 }, penalty[5] = { 0 }, capacity[5] = { 0 };

	// sCharacterInfo::unknown6A holds the character's five stat levels.
	for (int i = 0; i < 5; i++) {
		status_t status = pItemManager->GetLevelWithPenalty((eLvlType)i, m_charInfo.typeId,
			(const char*)&m_charInfo.unknown6A, m_charInfo.partTypeIds, m_charInfo.auxParts.tids,
			m_itemInfo.m_typeId, m_itemInfo.m_clubStatLevel, m_caddieInfo.typeId,
			WSingleton<CSharedDoc>::Instance()->m_myLevel);
		level[i] = status.m_level;
		penalty[i] = status.m_penalty;
		capacity[i] = pItemManager->GetCapacity((eLvlType)i, m_charInfo.typeId,
			(const char*)&m_charInfo.unknown6A, m_charInfo.partTypeIds,
			m_itemInfo.m_typeId, m_itemInfo.m_clubStatLevel, m_caddieInfo.typeId,
			WSingleton<CSharedDoc>::Instance()->m_myLevel);
	}

	m_statBar->SetDestTime(1.2f);
	for (int i = 0; i < 5; i++)
		m_statBar->SetRange((eLvlType)i, level[i], capacity[i], penalty[i]);
}

// familymain.cpp:365
void CFamilyMain::cFamily::PetRebuild(void) {
	if (m_exhibition) {
		delete m_exhibition;
		m_exhibition = 0;
	}
	if (m_partTidList) {
		delete m_partTidList;
		m_partTidList = 0;
	}
	if (m_statBar) {
		delete m_statBar;
		m_statBar = 0;
	}

	SetStatus();

	m_anotherHand.Enter();

	m_exhibition = new CExhibition(m_rect.x + m_rect.w * 0.5f, m_rect.y + m_rect.h * 0.5f - 70.0f, 0.35f, false);
	m_exhibition->SetArea(m_rect.w, m_rect.h);

	if (m_charInfo.typeId == 0x4000005 || m_charInfo.typeId == 0x4000009)
		m_exhibition->SetDistance(11.0f, true, false);
	else
		m_exhibition->SetDistance(13.0f, true, false);

	m_partTidList = new CPartTidList;
	if (!m_partTidList->SetTids(&m_charInfo, 0xFF) || !m_partTidList->IsComboValid())
		m_partTidList->SetDefaultTids();

	m_exhibition->SetModel(*m_partTidList, m_charInfo.auxParts.tids, 0, 3.0f, 0);
	if (WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().eventFlags & 0x10)
		m_exhibition->AttachAngelWingFx();

	m_anotherHand.Leave();
}

// familymain.cpp:396 (run-time type and factory registration of CFamilyMain)
const WRTTI CFamilyMain::m_RTTI("CFamilyMain", &CTaskMain::m_RTTI);

IObject* CFamilyMainMakeInstance(void) {
	return new CFamilyMain;
}

struct __impCFamilyMain {
	__impCFamilyMain(void) {
		ObjectFactory().AddObjectFunctor(CFamilyMainMakeInstance, "CFamilyMain");
		ActorFactory().AddActorClass("CFamilyMain");
	}
};
static __impCFamilyMain s_impCFamilyMain;

// familymain.cpp:397
const sFRESH_MSGMAP* CFamilyMain::GetMessageMap(void) const {
	return &_MsgMap;
}

sFRESH_MSGMAP CFamilyMain::_MsgMap = { &CTaskMain::_MsgMap, &CFamilyMain::_MsgEntries[0] };

sFRESH_ENTRY CFamilyMain::_MsgEntries[] = {
	// familymain.cpp:399
	{ "FAMILY", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnInitFamilyMain },
	// familymain.cpp:400
	{ "FAMILY", FRCMD_FINISH, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnInitFamilyFinish },
	// familymain.cpp:402
	{ "FAMILY.pet_area1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnInitAreaPet1 },
	// familymain.cpp:403
	{ "FAMILY.pet_area2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnInitAreaPet2 },
	// familymain.cpp:404
	{ "FAMILY.pet_area3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnInitAreaPet3 },
	// familymain.cpp:405
	{ "FAMILY.pet_area4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnInitAreaPet4 },
	// familymain.cpp:406
	{ "FAMILY.globalgui", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnInitAreaGlobal },
	// familymain.cpp:408
	{ "FAMILY.pet_area1", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnDrawAreaPet1 },
	// familymain.cpp:409
	{ "FAMILY.pet_area2", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnDrawAreaPet2 },
	// familymain.cpp:410
	{ "FAMILY.pet_area3", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnDrawAreaPet3 },
	// familymain.cpp:411
	{ "FAMILY.pet_area4", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnDrawAreaPet4 },
	// familymain.cpp:412
	{ "FAMILY.globalgui", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnDrawAreaGlobal },
	// familymain.cpp:414
	{ "FAMILY.start_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnInitBtnStart },
	// familymain.cpp:415
	{ "FAMILY.course_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnInitBtnSelMap },
	// familymain.cpp:416
	{ "FAMILY.limit_time_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnInitBtnHoleType },
	// familymain.cpp:417
	{ "FAMILY.hole_type_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnInitBtnHoleNum },
	// familymain.cpp:418
	{ "FAMILY.hole_num_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnInitBtnTimeLimit },
	// familymain.cpp:419
	{ "FAMILY.create_pet3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnInitCreatePet3 },
	// familymain.cpp:420
	{ "FAMILY.create_pet4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnInitCreatePet4 },
	// familymain.cpp:421
	{ "FAMILY.delete_pet3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnInitDeletePet3 },
	// familymain.cpp:422
	{ "FAMILY.delete_pet4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnInitDeletePet4 },
	// familymain.cpp:424
	{ "FAMILY.start_btn", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnDownBtnStart },
	// familymain.cpp:425
	{ "FAMILY.course_btn", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnDownBtnSelMap },
	// familymain.cpp:426
	{ "FAMILY.limit_time_btn", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnDownBtnHoleType },
	// familymain.cpp:427
	{ "FAMILY.hole_type_btn", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnDownBtnHoleNum },
	// familymain.cpp:428
	{ "FAMILY.hole_num_btn", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnDownBtnTimeLimit },
	// familymain.cpp:429
	{ "FAMILY.create_pet3", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnDownCreatePet3 },
	// familymain.cpp:430
	{ "FAMILY.create_pet4", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnDownCreatePet4 },
	// familymain.cpp:431
	{ "FAMILY.delete_pet3", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnDownDeletePet3 },
	// familymain.cpp:432
	{ "FAMILY.delete_pet4", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnDownDeletePet4 },
	// familymain.cpp:434
	{ "FAMILY.start_btn", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnDrawBtnStart },
	// familymain.cpp:435
	{ "FAMILY.course_btn", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnDrawBtnSelMap },
	// familymain.cpp:436
	{ "FAMILY.limit_time_btn", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnDrawBtnHoleType },
	// familymain.cpp:437
	{ "FAMILY.hole_type_btn", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnDrawBtnHoleNum },
	// familymain.cpp:438
	{ "FAMILY.hole_num_btn", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&CFamilyMain::OnDrawBtnTimeLimit },
	// familymain.cpp:440
	{ "FAMILY.pet_equip1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnInitListEquip1 },
	// familymain.cpp:441
	{ "FAMILY.pet_equip2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnInitListEquip2 },
	// familymain.cpp:442
	{ "FAMILY.pet_equip3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnInitListEquip3 },
	// familymain.cpp:443
	{ "FAMILY.pet_equip4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnInitListEquip4 },
	// familymain.cpp:445
	{ "FAMILY.pet_equip1", FRCMD_LBUTTONDOWN, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnDownListEquip1 },
	// familymain.cpp:446
	{ "FAMILY.pet_equip2", FRCMD_LBUTTONDOWN, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnDownListEquip2 },
	// familymain.cpp:447
	{ "FAMILY.pet_equip3", FRCMD_LBUTTONDOWN, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnDownListEquip3 },
	// familymain.cpp:448
	{ "FAMILY.pet_equip4", FRCMD_LBUTTONDOWN, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnDownListEquip4 },
	// familymain.cpp:450
	{ "FAMILY.pet_equip1", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnDrawListEquip1 },
	// familymain.cpp:451
	{ "FAMILY.pet_equip2", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnDrawListEquip2 },
	// familymain.cpp:452
	{ "FAMILY.pet_equip3", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnDrawListEquip3 },
	// familymain.cpp:453
	{ "FAMILY.pet_equip4", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CFamilyMain::OnDrawListEquip4 },
	// familymain.cpp:456
	{ "FAMILY.ubar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBarInit },
	// familymain.cpp:457
	{ "FAMILY.ubar.back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_BackInit },
	// familymain.cpp:458
	{ "FAMILY.ubar.back", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_BackUp },
	// familymain.cpp:459
	{ "FAMILY.ubar.back", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:460
	{ "FAMILY.ubar.back", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:461
	{ "FAMILY.ubar.front", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_FrontInit },
	// familymain.cpp:462
	{ "FAMILY.ubar.front", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_FrontUp },
	// familymain.cpp:463
	{ "FAMILY.ubar.front", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:464
	{ "FAMILY.ubar.front", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:465
	{ "FAMILY.ubar.shop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ShopInit },
	// familymain.cpp:466
	{ "FAMILY.ubar.shop", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ShopUp },
	// familymain.cpp:467
	{ "FAMILY.ubar.shop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:468
	{ "FAMILY.ubar.shop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:469
	{ "FAMILY.ubar.myroom", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MyRoomInit },
	// familymain.cpp:470
	{ "FAMILY.ubar.myroom", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MyRoomUp },
	// familymain.cpp:471
	{ "FAMILY.ubar.myroom", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:472
	{ "FAMILY.ubar.myroom", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:473
	{ "FAMILY.ubar.itemstorage", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ItemStorageInit },
	// familymain.cpp:474
	{ "FAMILY.ubar.itemstorage", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ItemStorageUp },
	// familymain.cpp:475
	{ "FAMILY.ubar.itemstorage", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:476
	{ "FAMILY.ubar.itemstorage", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:477
	{ "FAMILY.ubar.gift", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GiftInit },
	// familymain.cpp:478
	{ "FAMILY.ubar.gift", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GiftUp },
	// familymain.cpp:479
	{ "FAMILY.ubar.gift", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:480
	{ "FAMILY.ubar.gift", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:481
	{ "FAMILY.ubar.cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_CookieInit },
	// familymain.cpp:482
	{ "FAMILY.ubar.cookie", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_CookieUp },
	// familymain.cpp:483
	{ "FAMILY.ubar.cookie", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:484
	{ "FAMILY.ubar.cookie", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:485
	{ "FAMILY.ubar.game", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GameInit },
	// familymain.cpp:486
	{ "FAMILY.ubar.game", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GameUp },
	// familymain.cpp:487
	{ "FAMILY.ubar.game", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:488
	{ "FAMILY.ubar.game", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:489
	{ "FAMILY.ubar.server", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ServerInit },
	// familymain.cpp:490
	{ "FAMILY.ubar.server", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ServerUp },
	// familymain.cpp:491
	{ "FAMILY.ubar.server", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:492
	{ "FAMILY.ubar.server", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:493
	{ "FAMILY.ubar.exit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ExitInit },
	// familymain.cpp:494
	{ "FAMILY.ubar.exit", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ExitUp },
	// familymain.cpp:495
	{ "FAMILY.ubar.exit", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:496
	{ "FAMILY.ubar.exit", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:497
	{ "FAMILY.ubar.tabbtn_b", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_TabBtnInit },
	// familymain.cpp:498
	{ "FAMILY.ubar.tabbtn_b", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_TabBtnUp },
	// familymain.cpp:501
	{ "FAMILY.utab", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderTab_Init },
	// familymain.cpp:503
	{ "FAMILY.utab.magicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MagicBoxInit },
	// familymain.cpp:504
	{ "FAMILY.utab.magicbox", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MagicBoxUp },
	// familymain.cpp:505
	{ "FAMILY.utab.magicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:506
	{ "FAMILY.utab.magicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:508
	{ "FAMILY.utab.tikimagicbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_TikiMagicBoxInit },
	// familymain.cpp:509
	{ "FAMILY.utab.tikimagicbox", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_TikiMagicBoxUp },
	// familymain.cpp:510
	{ "FAMILY.utab.tikimagicbox", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:511
	{ "FAMILY.utab.tikimagicbox", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:533
	{ "FAMILY.utab.scratch", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_ScratchInit },
	// familymain.cpp:534
	{ "FAMILY.utab.scratch", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_ScratchUp },
	// familymain.cpp:535
	{ "FAMILY.utab.scratch", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:536
	{ "FAMILY.utab.scratch", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:538
	{ "FAMILY.utab.bongdarishop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_BongdariShopInit },
	// familymain.cpp:539
	{ "FAMILY.utab.bongdarishop", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_BongdariShopUp },
	// familymain.cpp:540
	{ "FAMILY.utab.bongdarishop", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:541
	{ "FAMILY.utab.bongdarishop", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:542
	{ "FAMILY.utab.ranking", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_RankingInit },
	// familymain.cpp:543
	{ "FAMILY.utab.ranking", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_RankingUp },
	// familymain.cpp:544
	{ "FAMILY.utab.ranking", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:545
	{ "FAMILY.utab.ranking", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:546
	{ "FAMILY.utab.guild", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_GuildInit },
	// familymain.cpp:547
	{ "FAMILY.utab.guild", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_GuildUp },
	// familymain.cpp:548
	{ "FAMILY.utab.guild", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:549
	{ "FAMILY.utab.guild", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:550
	{ "FAMILY.utab.notice", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_NoticeInit },
	// familymain.cpp:551
	{ "FAMILY.utab.notice", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_NoticeUp },
	// familymain.cpp:552
	{ "FAMILY.utab.notice", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:553
	{ "FAMILY.utab.notice", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:556
	{ "FAMILY.utab.maket", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnUnderBar_MaketInit },
	// familymain.cpp:557
	{ "FAMILY.utab.maket", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnUnderBar_MaketUp },
	// familymain.cpp:558
	{ "FAMILY.utab.maket", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:559
	{ "FAMILY.utab.maket", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:570
	{ "FAMILY.obar", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_Init },
	// familymain.cpp:571
	{ "FAMILY.obar.title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_TitleInit },
	// familymain.cpp:572
	{ "FAMILY.obar.site", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_SiteInit },
	// familymain.cpp:573
	{ "FAMILY.obar.char", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharInit },
	// familymain.cpp:574
	{ "FAMILY.obar.char", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_CharDown },
	// familymain.cpp:575
	{ "FAMILY.obar.char", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharOwnerDraw },
	// familymain.cpp:576
	{ "FAMILY.obar.caddie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieInit },
	// familymain.cpp:577
	{ "FAMILY.obar.caddie", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_CaddieDown },
	// familymain.cpp:578
	{ "FAMILY.obar.caddie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieOwnerDraw },
	// familymain.cpp:579
	{ "FAMILY.obar.club", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubInit },
	// familymain.cpp:580
	{ "FAMILY.obar.club", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_ClubDown },
	// familymain.cpp:581
	{ "FAMILY.obar.club", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubOwnerDraw },
	// familymain.cpp:582
	{ "FAMILY.obar.aztec", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecInit },
	// familymain.cpp:583
	{ "FAMILY.obar.aztec", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_AztecDown },
	// familymain.cpp:584
	{ "FAMILY.obar.aztec", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecOwnerDraw },
	// familymain.cpp:585
	{ "FAMILY.obar.mascot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotInit },
	// familymain.cpp:586
	{ "FAMILY.obar.mascot", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MascotDown },
	// familymain.cpp:587
	{ "FAMILY.obar.mascot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotOwnerDraw },
	// familymain.cpp:588
	{ "FAMILY.obar.char_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharSelInit },
	// familymain.cpp:589
	{ "FAMILY.obar.char_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CharSelOwnerDraw },
	// familymain.cpp:590
	{ "FAMILY.obar.caddie_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieSelInit },
	// familymain.cpp:591
	{ "FAMILY.obar.caddie_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_CaddieSelOwnerDraw },
	// familymain.cpp:592
	{ "FAMILY.obar.club_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubSelInit },
	// familymain.cpp:593
	{ "FAMILY.obar.club_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_ClubSelOwnerDraw },
	// familymain.cpp:594
	{ "FAMILY.obar.aztec_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecSelInit },
	// familymain.cpp:595
	{ "FAMILY.obar.aztec_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_AztecSelOwnerDraw },
	// familymain.cpp:596
	{ "FAMILY.obar.mascot_sel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotSelInit },
	// familymain.cpp:597
	{ "FAMILY.obar.mascot_sel", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MascotSelOwnerDraw },
	// familymain.cpp:598
	{ "FAMILY.obar.pang_cookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_PangCookieInit },
	// familymain.cpp:599
	{ "FAMILY.obar.pang_cookie", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_PangCookieOwnerDraw },
	// familymain.cpp:600
	{ "FAMILY.obar.onelinemsg_req", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineReqBtnInit },
	// familymain.cpp:601
	{ "FAMILY.obar.onelinemsg_req", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_OneLineReqBtnUp },
	// familymain.cpp:602
	{ "FAMILY.obar.onelineview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineViewInit },
	// familymain.cpp:603
	{ "FAMILY.obar.onelineview", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OneLineViewOwnerDraw },
	// familymain.cpp:604
	{ "FAMILY.obar.spcard", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_SpecialCardInit },
	// familymain.cpp:605
	{ "FAMILY.obar.spcard", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_SpecialCardUp },
	// familymain.cpp:606
	{ "FAMILY.obar.spcard", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:607
	{ "FAMILY.obar.spcard", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:608
	{ "FAMILY.obar.messenger", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MessengerInit },
	// familymain.cpp:609
	{ "FAMILY.obar.messenger", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MessengerUp },
	// familymain.cpp:610
	{ "FAMILY.obar.messenger", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:611
	{ "FAMILY.obar.messenger", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:612
	{ "FAMILY.obar.myinfo", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_MyInfoInit },
	// familymain.cpp:613
	{ "FAMILY.obar.myinfo", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_MyInfoUp },
	// familymain.cpp:614
	{ "FAMILY.obar.myinfo", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:615
	{ "FAMILY.obar.myinfo", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:616
	{ "FAMILY.obar.option", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnOverBar_OptionInit },
	// familymain.cpp:617
	{ "FAMILY.obar.option", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnOverBar_OptionUp },
	// familymain.cpp:618
	{ "FAMILY.obar.option", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOn },
	// familymain.cpp:619
	{ "FAMILY.obar.option", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarHoverOff },
	// familymain.cpp:622
	{ "FAMILY.obar.messenger_bubble", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleInit },
	// familymain.cpp:623
	{ "FAMILY.obar.messenger_bubble", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleOwnerDraw },
	// familymain.cpp:624
	{ "FAMILY.obar.messenger_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerBubbleEditInit },
	// familymain.cpp:625
	{ "FAMILY.obar.messenger_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerAlarmBtnInit },
	// familymain.cpp:626
	{ "FAMILY.obar.messenger_alarm", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnMessengerAlarmBtnUp },
	// familymain.cpp:627
	{ "FAMILY.obar.gift_alarm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnMessengerGiftAlarmBtnInit },
	// familymain.cpp:628
	{ "FAMILY.obar.gift_alarm", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&CTaskMain::OnMessengerGiftAlarmBtnUp },
	// familymain.cpp:629
	{ "FAMILY.obar.tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CTaskMain::OnBarTooltipOwnerDraw },

	// familymain.cpp:631
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// familymain.cpp:634
void CFamilyMain::OnInitFamilyMain(int param) {
	SetTimeVariableBGM();
	TurnOff3DBackground();

	WSingleton<CSharedDoc>::Instance()->m_gameMode = 1;

	m_families.reserve(4);
	m_areas.reserve(5);
	m_buttons.reserve(9);
	m_equipLists.reserve(4);

	m_families.assign(4, (cFamily*)0);
	m_areas.assign(5, (FrArea*)0);
	m_buttons.assign(9, (FrButton*)0);
	m_equipLists.assign(4, (FrListBox*)0);

	sUserInfo info;

	for (int i = 1; i < 4; i++)
		WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i] = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[0];

	m_timeLimit = 0;
	if (m_mapIndex == 0xFF)
		m_mapIndex = rand() % 20;

	if (m_mapIndex == 0x11 || m_mapIndex == 0x0C)
		m_mapIndex = 0;

	if (ItemManager()->FindCourse(0x28000000 | m_mapIndex) == 0) {
		do {
			if (++m_mapIndex >= 20)
				m_mapIndex = 0;
		} while (ItemManager()->FindCourse(0x28000000 | m_mapIndex) == 0);
	}

	m_holeNum = 3;
	m_holeType = 0;
	m_timeLimit = 0;
	SetGameType(0);
	WSingleton<CSharedDoc>::Instance()->m_gameOver = false;

	int count = WSingleton<CSharedDoc>::Instance()->m_characterMap.size();

	m_petList.reserve(count);

	for (int i = 0; i < count; i++)
		m_petList.push_back(i);

	std::random_shuffle(m_petList.begin(), m_petList.end());
}

// familymain.cpp:708
void CFamilyMain::OnInitFamilyFinish(void) {
	m_titleArea->SetBgImg("title_family_renew");

	for (int i = 0; i < 2; i++) {
		m_families[i] = new cFamily;
		m_families[i]->Init(m_areas[i]->GetRect(), m_petList, i);

		cFamily* family = m_families[i];
		FrListBox* list = m_equipLists[i];

		list->AddItem(CEquipHelper::Instance()->GetItemCommon(CEquipHelper::eCategoryCharacter, family->GetUserEquip()));
		list->AddItem(CEquipHelper::Instance()->GetItemCommon(CEquipHelper::eCategoryCaddie, family->GetUserEquip()));
		list->AddItem(CEquipHelper::Instance()->GetItemCommon(CEquipHelper::eCategoryClub, family->GetUserEquip()));
		list->AddItem(CEquipHelper::Instance()->GetItemCommon(CEquipHelper::eCategoryBall, family->GetUserEquip()));
		list->AddItem(CEquipHelper::Instance()->GetItemCommon(CEquipHelper::eCategoryMascot, family->GetUserEquip()));
		list->AddItem(0);

		WRect rect = list->GetRect();

		rect.x = family->GetFramePos().x;
		rect.y = family->GetFramePos().y;
		list->SetRect(rect);
	}

	TurnOffControls();
	m_barButton[15]->Enable(true);
}

// familymain.cpp:739
void CFamilyMain::TurnOffControls(void) {
	m_barButton[14]->Enable(false);
	m_barButton[2]->Enable(false);
	m_barButton[3]->Enable(false);
#ifdef JP_PAKS
	// No "itemstorage" button in the JP under bar (CTaskMain::EnableUnderBar).
	if (m_barButton[4]) {
		m_barButton[4]->Enable(false);
	}
#else
	m_barButton[4]->Enable(false);
#endif
	m_barButton[6]->Enable(false);
	m_barButton[8]->Enable(false);
	m_barButton[16]->Enable(false);
	m_barButton[17]->Enable(false);
	m_barButton[10]->Enable(false);
	m_barButton[5]->Enable(false);
	m_barButton[7]->Enable(false);

	m_charViewer->SetVisible(false);
	m_caddieViewer->SetVisible(false);
	m_aztecViewer->SetVisible(false);
	m_mascotViewer->SetVisible(false);
	m_clubViewer->SetVisible(false);
}

// familymain.cpp:766
void CFamilyMain::OnInitAreaPet1(int param) {
	m_areas[0] = DYNAMIC_CAST(FrArea, param);
}

// familymain.cpp:770
void CFamilyMain::OnInitAreaPet2(int param) {
	m_areas[1] = DYNAMIC_CAST(FrArea, param);
}

// familymain.cpp:774
void CFamilyMain::OnInitAreaPet3(int param) {
	m_areas[2] = DYNAMIC_CAST(FrArea, param);
}

// familymain.cpp:778
void CFamilyMain::OnInitAreaPet4(int param) {
	m_areas[3] = DYNAMIC_CAST(FrArea, param);
}

// familymain.cpp:782
void CFamilyMain::OnInitAreaGlobal(int param) {
	m_areas[4] = DYNAMIC_CAST(FrArea, param);
}

// familymain.cpp:787
void CFamilyMain::OnDrawAreaPet1(void) {
	if (g_pFresh->GetManager()->GetGDI() == 0)
		return;

	if (m_families[0])
		m_families[0]->Render();
}

// familymain.cpp:797
void CFamilyMain::OnDrawAreaPet2(void) {
	if (g_pFresh->GetManager()->GetGDI() == 0)
		return;

	if (m_families[1])
		m_families[1]->Render();
}

// familymain.cpp:806
void CFamilyMain::OnDrawAreaPet3(void) {
	if (g_pFresh->GetManager()->GetGDI() == 0)
		return;

	if (m_families[2])
		m_families[2]->Render();
}

// familymain.cpp:815
void CFamilyMain::OnDrawAreaPet4(void) {
	if (g_pFresh->GetManager()->GetGDI() == 0)
		return;

	if (m_families[3])
		m_families[3]->Render();
}

// familymain.cpp:824
void CFamilyMain::OnDrawAreaGlobal(void) {
}

// familymain.cpp:831
void CFamilyMain::OnInitBtnStart(int param) {
	m_buttons[0] = DYNAMIC_CAST(FrButton, param);
}

// familymain.cpp:835
void CFamilyMain::OnInitBtnSelMap(int param) {
	m_buttons[1] = DYNAMIC_CAST(FrButton, param);
}

// familymain.cpp:839
void CFamilyMain::OnInitBtnHoleType(int param) {
	m_buttons[2] = DYNAMIC_CAST(FrButton, param);
	m_buttons[2]->SetPushDelay(0.1f);
}

// familymain.cpp:844
void CFamilyMain::OnInitBtnHoleNum(int param) {
	m_buttons[3] = DYNAMIC_CAST(FrButton, param);
	m_buttons[3]->SetPushDelay(0.1f);
}

// familymain.cpp:849
void CFamilyMain::OnInitBtnTimeLimit(int param) {
	m_buttons[4] = DYNAMIC_CAST(FrButton, param);
	m_buttons[4]->SetPushDelay(0.1f);
}

// familymain.cpp:855
void CFamilyMain::OnInitCreatePet3(int param) {
	m_buttons[5] = DYNAMIC_CAST(FrButton, param);
}

// familymain.cpp:859
void CFamilyMain::OnInitCreatePet4(int param) {
	m_buttons[6] = DYNAMIC_CAST(FrButton, param);
}

// familymain.cpp:863
void CFamilyMain::OnInitDeletePet3(int param) {
	m_buttons[7] = DYNAMIC_CAST(FrButton, param);
	m_buttons[7]->SetVisible(false);
}

// familymain.cpp:868
void CFamilyMain::OnInitDeletePet4(int param) {
	m_buttons[8] = DYNAMIC_CAST(FrButton, param);
	m_buttons[8]->SetVisible(false);
}

// familymain.cpp:875
void CFamilyMain::OnDownBtnSelMap(void) {
	if (m_selMapDlg == 0) {
		m_selMapDlg = CreateForm<FrTreasureCourse>(g_pFresh->GetManager(), this, "treasuremap", 0);
		if (m_selMapDlg) {
			m_selMapDlg->SetSelectMap(m_mapIndex);
			m_selMapDlg->SetMapIndexChanged(false);
			m_selMapDlg->EnableDrag(false);
			m_selMapDlg->EnableRandomBtn(false);

			m_selMapDlg->Open((FRESH_RESULT_CALLBACK)&CFamilyMain::OnResultSelMap, 3);
		}
	}
}

// familymain.cpp:892
bool CFamilyMain::OnResultSelMap(int, FrForm*) {
	if (m_selMapDlg) {
		m_mapIndex = m_selMapDlg->GetCurMapIdx();
		SetCurMap(m_mapIndex);
		m_selMapDlg = 0;
	}
	return true;
}

// familymain.cpp:903
void CFamilyMain::OnDownBtnHoleType(void) {
	m_holeType++;

	if (m_holeType > 3)
		m_holeType = 0;

	WSingleton<CSharedDoc>::Instance()->m_gameKind = m_holeType;
}

// familymain.cpp:913
void CFamilyMain::OnDownBtnHoleNum(void) {
	unsigned char holes[4] = { 3, 6, 9, 18 };

	for (int i = 0; i < 4; ++i) {
		if (holes[i] == m_holeNum) {
			++i;
			if (i >= 4)
				i = 0;

			m_holeNum = holes[i];
			break;
		}
	}

	WSingleton<CSharedDoc>::Instance()->m_holes = m_holeNum;
}

// familymain.cpp:932
void CFamilyMain::OnDownBtnTimeLimit(void) {
	unsigned long limits[6] = { 0, 30000, 40000,
		50000, 60000,
		90000 };

	for (int i = 0; i < 6; ++i) {
		if (limits[i] == m_timeLimit) {
			++i;
			if (i >= 6)
				i = 0;

			m_timeLimit = limits[i];
			break;
		}
	}

	WSingleton<CSharedDoc>::Instance()->m_shotTimeLimit = m_timeLimit;
}

// familymain.cpp:956
void CFamilyMain::OnDownCreatePet3(void) {
	if (m_unknown248)
		return;

	m_unknown248 = true;
	m_buttons[5]->SetVisible(false);

	m_families[2] = new cFamily;
	m_families[2]->Init(m_areas[2]->GetRect(), m_petList, 2);

	cFamily* family = m_families[2];
	FrListBox* list = m_equipLists[2];
	m_equipLists[2]->SetVisible(true);

	list->AddItem(CEquipHelper::Instance()->GetItemCommon(CEquipHelper::eCategoryCharacter, family->GetUserEquip()));
	list->AddItem(CEquipHelper::Instance()->GetItemCommon(CEquipHelper::eCategoryCaddie, family->GetUserEquip()));
	list->AddItem(CEquipHelper::Instance()->GetItemCommon(CEquipHelper::eCategoryClub, family->GetUserEquip()));
	list->AddItem(CEquipHelper::Instance()->GetItemCommon(CEquipHelper::eCategoryBall, family->GetUserEquip()));
	list->AddItem(CEquipHelper::Instance()->GetItemCommon(CEquipHelper::eCategoryMascot, family->GetUserEquip()));
	list->AddItem(0);

	WRect rect = list->GetRect();

	rect.x = family->GetFramePos().x;
	rect.y = family->GetFramePos().y;
	list->SetRect(rect);
	m_unknown248 = false;
	m_buttons[7]->SetVisible(true);
}

// familymain.cpp:987
void CFamilyMain::OnDownCreatePet4(void) {
	if (m_unknown248)
		return;

	m_unknown248 = true;
	m_buttons[6]->SetVisible(false);

	m_families[3] = new cFamily;
	m_families[3]->Init(m_areas[3]->GetRect(), m_petList, 3);

	cFamily* family = m_families[3];
	FrListBox* list = m_equipLists[3];
	m_equipLists[3]->SetVisible(true);

	list->AddItem(CEquipHelper::Instance()->GetItemCommon(CEquipHelper::eCategoryCharacter, family->GetUserEquip()));
	list->AddItem(CEquipHelper::Instance()->GetItemCommon(CEquipHelper::eCategoryCaddie, family->GetUserEquip()));
	list->AddItem(CEquipHelper::Instance()->GetItemCommon(CEquipHelper::eCategoryClub, family->GetUserEquip()));
	list->AddItem(CEquipHelper::Instance()->GetItemCommon(CEquipHelper::eCategoryBall, family->GetUserEquip()));
	list->AddItem(CEquipHelper::Instance()->GetItemCommon(CEquipHelper::eCategoryMascot, family->GetUserEquip()));
	list->AddItem(0);

	WRect rect = list->GetRect();

	rect.x = family->GetFramePos().x;
	rect.y = family->GetFramePos().y;
	list->SetRect(rect);
	m_unknown248 = false;
	m_buttons[8]->SetVisible(true);
}

// familymain.cpp:1018
void CFamilyMain::OnDownDeletePet3(void) {
	cFamily* family = m_families[2];
	delete family;
	m_families[2] = 0;

	m_buttons[7]->SetVisible(false);
	m_buttons[5]->SetVisible(true);
	m_buttons[5]->EnableHover(true, 0.0f);
	m_equipLists[2]->ClearItem();
	m_equipLists[2]->SetVisible(false);
}

// familymain.cpp:1031
void CFamilyMain::OnDownDeletePet4(void) {
	cFamily* family = m_families[3];
	delete family;
	m_families[3] = 0;

	m_buttons[8]->SetVisible(false);
	m_buttons[6]->SetVisible(true);
	m_buttons[6]->EnableHover(true, 0.0f);
	m_equipLists[3]->ClearItem();
	m_equipLists[3]->SetVisible(false);
}

// familymain.cpp:1044
void CFamilyMain::OnDrawBtnStart(void) {
}

// familymain.cpp:1049
void CFamilyMain::OnDrawBtnSelMap(void) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0)
		return;

	IFF_STRUCT::sCourse* course = ItemManager()->FindCourse(0x28000000 | m_mapIndex);
	if (course == 0)
		return;

	std::string name("mid_");
	name += course->m_icon;
	const Bitmap* icon = g_pFresh->GetBitmap(name.c_str());

	if (icon) {
		WRect rect = m_buttons[1]->GetRect();
		rect.x += (rect.w - icon->Width() + 1.0f) * 0.5f;
		rect.y += (rect.h - icon->Height() + 1.0f) * 0.5f;
		rect.w = (float)icon->Width();
		rect.h = (float)icon->Height();

		pGDI->DrawTexture(icon, rect, 0xFFFFFFFF, 0);
	}
}

// familymain.cpp:1075
void CFamilyMain::OnDrawBtnHoleType(void) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0)
		return;

	const WRect& rect = m_buttons[2]->GetRect();

	pGDI->SetTextColor(0xFFFFFFFF, 0xFF000000);
	pGDI->SetTextStyle(4);

	std::string holeTypes[4] = { "FRONT", "BACK", "RANDOM", "SHUFFLE" };
	pGDI->PrintText(WPoint(rect.x + 15.0f, rect.y + 2.0f), 0, MakeStr("\xc1\xf8\xc7\xe0\xb9\xe6\xbd\xc4:%s", holeTypes[m_holeType].c_str()), 0);
	pGDI->SetTextStyle(0);
}

// familymain.cpp:1090
void CFamilyMain::OnDrawBtnHoleNum(void) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0)
		return;

	const WRect& rect = m_buttons[3]->GetRect();

	pGDI->SetTextColor(0xFFFFFFFF, 0xFF000000);
	pGDI->SetTextStyle(4);
	pGDI->PrintText(WPoint(rect.x + 15.0f, rect.y + 2.0f), 0, MakeStr("\xc8\xa6 \xbc\xf6 : %d \xc8\xa6", m_holeNum), 0);
	pGDI->SetTextStyle(0);
}

// familymain.cpp:1103
void CFamilyMain::OnDrawBtnTimeLimit(void) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0)
		return;

	const WRect& rect = m_buttons[4]->GetRect();

	pGDI->SetTextColor(0xFFFFFFFF, 0xFF000000);
	pGDI->SetTextStyle(4);
	unsigned long seconds = m_timeLimit / 1000;
	pGDI->PrintText(WPoint(rect.x + 15.0f, rect.y + 2.0f), 0,
		seconds == 0 ? "\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3:\xb9\xab\xc1\xa6\xc7\xd1" : MakeStr("\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3:%d\xc3\xca", seconds), 0);
	pGDI->SetTextStyle(0);
}

// familymain.cpp:1118
void CFamilyMain::OnInitListEquip1(int param) {
	m_equipLists[0] = DYNAMIC_CAST(FrListBox, param);
}

// familymain.cpp:1122
void CFamilyMain::OnInitListEquip2(int param) {
	m_equipLists[1] = DYNAMIC_CAST(FrListBox, param);
}

// familymain.cpp:1126
void CFamilyMain::OnInitListEquip3(int param) {
	m_equipLists[2] = DYNAMIC_CAST(FrListBox, param);
}

// familymain.cpp:1130
void CFamilyMain::OnInitListEquip4(int param) {
	m_equipLists[3] = DYNAMIC_CAST(FrListBox, param);
}

// familymain.cpp:1135
void CFamilyMain::OnDownListEquip1(int) {
	OnDownEquipListBox(0);
}

// familymain.cpp:1139
void CFamilyMain::OnDownListEquip2(int) {
	OnDownEquipListBox(1);
}

// familymain.cpp:1143
void CFamilyMain::OnDownListEquip3(int) {
	OnDownEquipListBox(2);
}

// familymain.cpp:1147
void CFamilyMain::OnDownListEquip4(int) {
	OnDownEquipListBox(3);
}

// familymain.cpp:1152
void CFamilyMain::OnDownEquipListBox(int index) {
	FrListItem* pItem = m_equipLists[index]->GetSelected();
	if (pItem == 0)
		return;

	bool open = false;
	if (pItem->pData == 0) {
		if (pItem->idx == 5) {
			m_curPet = index;
			FrEquipDlg* dlg = CreateForm<FrEquipDlg>(g_pFresh->GetManager(), this, "equip", 0);
			if (dlg) {
				dlg->Init(ms_itemslotBackup[m_curPet], true);
				dlg->Open((FRESH_RESULT_CALLBACK)&CFamilyMain::OnItemEquipResult, 3);
			}
		} else {
			switch (pItem->idx) {
			case 1:
				if (WSingleton<CSharedDoc>::Instance()->m_caddieMap.size() != 0)
					open = true;
				break;
			case 4:
				if (WSingleton<CSharedDoc>::Instance()->m_myMascotMap.size() != 0)
					open = true;
				break;
			}
		}
	} else {
		open = true;
	}

	if (m_subEquipBar == 0 && open) {
		m_selectedItem = pItem;
		m_curPet = index;

		m_subEquipBar = CreateForm<FrSubEquipBar>(g_pFresh->GetManager(), this, "EQUIPBAR_SUB", 0);
		if (m_subEquipBar) {
			int category = 1;
			WPoint pt(pItem->pos.x, pItem->pos.y);
			switch (pItem->idx) {
			case 4: category = 0x10; break;
			case 1: category = 7; break;
			case 0: category = 1; break;
			case 2: category = 4; break;
			case 3: category = 5; break;
			}

			m_subEquipBar->Open((FRESH_RESULT_CALLBACK)&CFamilyMain::OnResultSubEquipBar,
				(FrSubEquipBar::eCategory)category, pt, (FrSubEquipBar::eSubType)2);

			WRect rect = m_subEquipBar->GetRect();
			rect.y -= rect.h;

			WView* view = g_view;

			if (rect.x + rect.w + 5.0f > view->GetWidth())
				rect.x = view->GetWidth() - rect.w - 5.0f;
			m_subEquipBar->SetRect(rect);
		}
	}
}

// familymain.cpp:1229
bool CFamilyMain::OnItemEquipResult(int result, FrForm* pForm) {
	if (result == 1 || result == 2) {
		cFamily* family = m_families[m_curPet];
		family->SetItemSlot(((FrEquipDlg*)pForm)->GetMyItemSlot());

		memcpy(ms_itemslotBackup[m_curPet], ((FrEquipDlg*)pForm)->GetMyItemSlot(), sizeof(ms_itemslotBackup[0]));
	}
	return true;
}

// familymain.cpp:1242
bool CFamilyMain::OnResultSubEquipBar(int result, FrForm* pForm) {
	if (result == 1) {
		cFamily* family = m_families[m_curPet];
		IFF_ITEM_COMMON* item = m_subEquipBar->GetResult();
		if (item) {
			int group = item->m_typeId >> 26;
			if (group == 5)
				family->SetUserEquip(group, item->m_typeId);
			else
				family->SetUserEquip(group, m_subEquipBar->GetLastSelectedGuid());

			m_selectedItem->pData = item;
		} else {
			switch (m_subEquipBar->GetCategory()) {
			case 0x10:
				family->SetUserEquip(0x10, 0);
				m_selectedItem->pData = 0;
				break;
			case 7:
				family->SetUserEquip(7, 0);
				m_selectedItem->pData = 0;
				break;
			}
		}
	}

	m_subEquipBar = 0;
	m_selectedItem = 0;
	return true;
}

// familymain.cpp:1286
void CFamilyMain::OnDrawListEquip1(int item) {
	OnDrawEquipListBox(reinterpret_cast<FrListItem*>(item));
}

// familymain.cpp:1290
void CFamilyMain::OnDrawListEquip2(int item) {
	OnDrawEquipListBox(reinterpret_cast<FrListItem*>(item));
}

// familymain.cpp:1294
void CFamilyMain::OnDrawListEquip3(int item) {
	OnDrawEquipListBox(reinterpret_cast<FrListItem*>(item));
}

// familymain.cpp:1298
void CFamilyMain::OnDrawListEquip4(int item) {
	OnDrawEquipListBox(reinterpret_cast<FrListItem*>(item));
}

// familymain.cpp:1303
void CFamilyMain::OnDrawEquipListBox(FrListItem* pItem) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pItem == 0 || pGDI == 0)
		return;

	const Bitmap* bar;
	IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)pItem->pData;
	if (item) {
		bar = g_pFresh->GetBitmap(MakeStr("bar_%s", item->m_icon));
	} else {
		bar = g_pFresh->GetBitmap("bar_no");
	}

	if (bar == 0)
		return;

	WRect rect(pItem->pos.x + 5.0f, pItem->pos.y + 5.0f, bar->Width() + 0.5f, bar->Height() + 0.5f);
	if (pItem->idx != 5) {
		pGDI->DrawTexture(bar, rect, 0xFFFFFFFF, 0);
	} else {
		const Bitmap* button = g_pFresh->GetBitmap("gameroom_equip_item_bn");
		if (button)
			pGDI->DrawTexture(button, rect, 0xFFFFFFFF, 0);
	}

	if (rect.IsInRect(WPoint(g_input->GetMousePoint().x, g_input->GetMousePoint().y))) {
		const Bitmap* frame = g_pFresh->GetBitmap("select_frame");
		if (frame)
			pGDI->DrawTexture(frame, WRect(rect.x - 3.0f, rect.y - 3.0f, rect.w + 6.0f, rect.h + 6.0f), 0xFFFFFFFF, 0);
	}
}

// familymain.cpp:1344
void CFamilyMain::OnDownBtnStart(void) {
	WSingleton<CSharedDoc>::Instance()->m_shotTimeLimit = m_timeLimit;
	SetCurMap(m_mapIndex);
	WSingleton<CSharedDoc>::Instance()->m_roomType = 0; strcpy(WSingleton<CSharedDoc>::Instance()->m_roomName, WSingleton<CSharedDoc>::Instance()->m_gameTypeNames[0].name);
	WSingleton<CSharedDoc>::Instance()->m_holes = m_holeNum;
	WSingleton<CSharedDoc>::Instance()->m_gameKind = m_holeType;
	WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.clear();

	if ((0x28000000 | ((m_mapIndex > 0x7F && m_mapIndex != 0xFD) ? m_mapIndex - 0x80 : m_mapIndex)) == 0x11 ||
		(0x28000000 | ((m_mapIndex > 0x7F && m_mapIndex != 0xFD) ? m_mapIndex - 0x80 : m_mapIndex)) == 0x0C) {
		m_mapIndex = 0;
		SetCurMap(m_mapIndex);
	}

	for (int i = 0; i < 4; i++) {
		sSlotInfo slot;
		memset(&slot, 0, sizeof(slot));
		memset(&WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i], 0, sizeof(sUserInfo));

		cFamily* pFamily = m_families[i];
		if (pFamily == NULL) {
			memset(ms_itemslotBackup[i], 0, sizeof(ms_itemslotBackup[i]));
			slot.m_uid = (unsigned long)-1;
			WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.push_back(slot);
			continue;
		}

		pFamily->ConvertUserInfo(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i]);
		slot.m_uid = i;

		slot.m_connection = (unsigned char)(i + 1);

		slot.m_level = WSingleton<CSharedDoc>::Instance()->m_myLevel; strncpy(slot.m_nickname, MakeStr("PLAYER%d", i + 1), 0x15);
		strcpy(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_name, MakeStr("PLAYER%d", i + 1));

		memcpy(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_equipItem, ms_itemslotBackup[i], sizeof(ms_itemslotBackup[i]));
		IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_caddieInfo.typeId);

		if (pCaddie != NULL && (WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_level < (pCaddie->m_level & 0x7F) ||
			((WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_caddieInfo.flags & 2) && WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_caddieInfo.days == 0 && pCaddie->fee > 0)))
			WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_caddieInfo.typeId = 0;

		IFF_STRUCT::sClubSet* pClubSet = ItemManager()->FindClubSet(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_clubSet.typeId);

		if (pClubSet != NULL && WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_level < (pClubSet->m_level & 0x7F) && !(pClubSet->m_level >> 7) ||
			WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_level > (pClubSet->m_level & 0x7F) && (pClubSet->m_level >> 7))
			WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_clubSet.typeId = 0x10000000;

		WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.push_back(slot);
	}

	switch (WSingleton<CSharedDoc>::Instance()->m_gameKind) {
	case 0: {
		int hole = rand() % 18;
		for (int i = 0; i < 18; i++) {
			WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] = (unsigned char)(hole + 1);
			hole++;
			if (hole >= 18)
				hole = 0;
		}
		break;
	}

	case 1: {
		for (int i = 0; i < 18; i++)
			WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] = (unsigned char)(i + 1);
		break;
	}

	case 2: {
		int hole = 9;
		for (int i = 0; i < 18; i++) {
			WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] = (unsigned char)(hole + 1);
			hole++;
			if (hole >= 18)
				hole = 0;
		}
		break;
	}

	case 3: {
		for (unsigned char i = 0; i < 18; i++)
			WSingleton<CSharedDoc>::Instance()->m_holeOrder[i] = i + 1;

		std::random_shuffle(WSingleton<CSharedDoc>::Instance()->m_holeOrder, WSingleton<CSharedDoc>::Instance()->m_holeOrder + 17);
		break;
	}
	}

	for (int i = 0; i < 18; i++)
		WSingleton<CSharedDoc>::Instance()->m_holePin[i] = (unsigned char)(rand() % 3);

	WSingleton<CSharedDoc>::Instance()->m_gameMode = 1;

	WSingleton<CMouseCursor>::Instance()->SetMode(1);
	WSingleton<CMouseCursor>::Instance()->SetActive(false);
	WSingleton<CTaskManager>::Instance()->ChangeTask("CGolfTask", "CGolfDoc", false);

	WSendPacket packet(0x5A);
	packet.Encode1(1);
	packet.Send(SEND_TO_DEFAULT);
}

// familymain.cpp:1471
CFamilyMain::CFamilyMain(void)
	: m_selMapDlg(0), m_subEquipBar(0), m_unknown248(false) {
	m_curPet = 0;
	WSingleton<CSharedDoc>::Instance()->m_gameMode = 1;
	g_pFresh->GetManager()->SetExclusiveKey(true);
}

// familymain.cpp:1478
CFamilyMain::~CFamilyMain(void) {
	g_pFresh->GetManager()->SetExclusiveKey(false);
	this << MsgObject(0, 2, 0, 0, 0, 0, 0);
}

// familymain.cpp:1484
void CFamilyMain::OnPreLoadInit(void) {
	CTaskMain::OnPreLoadInit();

	WSingleton<CMouseCursor>::Instance()->SetAutoHide(false);
}

// familymain.cpp:1491
void CFamilyMain::OnLoad(void) {
	CTaskMain::OnLoad();
}

// familymain.cpp:1496
void CFamilyMain::OnInit(void) {
	CTaskMain::OnInit();
}

// familymain.cpp:1501
void CFamilyMain::OnDestroy(void) {
	std::for_each(m_families.begin(), m_families.end(), Delete_Object());
	m_families.clear();
	m_areas.clear();
	m_buttons.clear();

	CTaskMain::OnDestroy();
}

// familymain.cpp:1513
void CFamilyMain::OnProcess(float dt) {
	for (std::vector<cFamily*>::iterator it = m_families.begin(); it != m_families.end(); ++it) {
		if (*it)
			(*it)->Process(dt);
	}

	CTaskMain::OnProcess(dt);
}

// familymain.cpp:1528
void CFamilyMain::OnDisplay(void) {
}

// familymain.cpp:1532
void CFamilyMain::ReFresh(bool) {
}

// familymain.cpp:1537
void CFamilyMain::HandleMsg(const MsgObject& msg) {
	switch (msg.m_msg) {
	case 0: {
		if (g_pFresh == 0)
			return;

		const char* layout = (const char*)msg.m_param1;
		if (WSingleton<CUserInfo>::Instance())
			WSingleton<CUserInfo>::Instance()->Close();
		g_pFresh->OpenLayout(layout, this, true);
		break;
	}

	case 1:
	case 2:
	case 0xE2:
	case 0xE3:
		break;

	default:
		HandleMsgCommon(msg);
		break;
	}
}
