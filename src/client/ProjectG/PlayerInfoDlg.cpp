// Original: d:\build\custom\temp\code\client\projectg\playerinfodlg.cpp
#include <stdio.h>

#include "PlayerInfoDlg.h"
#include "ObjectFactory.h"
#include "fredit.h"
#include "SharedDoc.h"
#include "GolfDoc.h"

// The level names, 20 bytes each (a per-unit copy of a header table; no symbol).
extern "C" const char data_a0c170[][20];

// playerinfodlg.cpp:21 (run-time type and factory registration of FrPlayerInfoDlg)
const WRTTI FrPlayerInfoDlg::m_RTTI("FrPlayerInfoDlg", &FrForm::m_RTTI);

IObject* FrPlayerInfoDlgMakeInstance(void) {
	return new FrPlayerInfoDlg;
}

struct __sFrPlayerInfoDlg {
	__sFrPlayerInfoDlg(void) {
		ObjectFactory().AddObjectFunctor(FrPlayerInfoDlgMakeInstance, "FrPlayerInfoDlg");
	}
};
static __sFrPlayerInfoDlg s_registerFrPlayerInfoDlg;

// playerinfodlg.cpp:23
const sFRESH_MSGMAP* FrPlayerInfoDlg::GetMessageMap(void) const {
	return &FrPlayerInfoDlg::_MsgMap;
}

sFRESH_MSGMAP FrPlayerInfoDlg::_MsgMap = { &FrForm::_MsgMap, &FrPlayerInfoDlg::_MsgEntries[0] };

sFRESH_ENTRY FrPlayerInfoDlg::_MsgEntries[] = {
	// playerinfodlg.cpp:25
	{ "user", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerInfoDlg::OnUserInfoInit },
	// playerinfodlg.cpp:26
	{ "character", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerInfoDlg::OnCharInfoInit },
	// playerinfodlg.cpp:27
	{ "caddie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPlayerInfoDlg::OnCadInfoInit },

	// playerinfodlg.cpp:29
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// playerinfodlg.cpp:33
FrPlayerInfoDlg::FrPlayerInfoDlg(void) {
}

// playerinfodlg.cpp:38
FrPlayerInfoDlg::~FrPlayerInfoDlg(void) {
}

// playerinfodlg.cpp:43
void FrPlayerInfoDlg::WritePlayerInfoDlg(void) {
	char buf[200];

	// playerinfodlg.cpp:61
	_snprintf(buf, 200, "[\xc0\xaf\xc0\xfa]");
	m_pUserInfo->AddLine(buf, 0, false);
	_snprintf(buf, 200, "\xb4\xeb\xc8\xad\xb8\xed: %s", WSingleton<CSharedDoc>::Instance()->m_roomPlayers[MyIndex()].m_name);
	m_pUserInfo->AddLine(buf, 0, false);

	// playerinfodlg.cpp:66
	unsigned char *pLevel = &WSingleton<CSharedDoc>::Instance()->m_roomPlayers[MyIndex()].m_level;
	_snprintf(buf, 200, "\xb7\xb9  \xba\xa7: %s", data_a0c170[*pLevel]);
	m_pUserInfo->AddLine(buf, 0, false);
	_snprintf(buf, 200, "\xb0\xe6\xc7\xe8\xc4\xa1: %d", WSingleton<CSharedDoc>::Instance()->m_roomPlayers[MyIndex()].m_value155);
	m_pUserInfo->AddLine(buf, 0, false);

	// playerinfodlg.cpp:77
	_snprintf(buf, 200, "[\xc4\xb3\xb8\xaf\xc5\xcd(TID)]");
	m_pCharInfo->AddLine(buf, 0, false);

	sCharacterInfo *pCharInfo = &WSingleton<CSharedDoc>::Instance()->m_roomPlayers[MyIndex()].m_characterInfo;
	IFF_STRUCT::sChar *pChar = ItemManager()->FindChar(pCharInfo->typeId);
	if (pChar) {
		_snprintf(buf, 200, "\xc0\xcc  \xb8\xa7: %s( %d )", pChar->m_name, pCharInfo->typeId);
		m_pCharInfo->AddLine(buf, 0, false);
	}

	// playerinfodlg.cpp:114
	sPlayerData *pPlayer = PLAYER(MyIndex());
	if (pPlayer) {
		_snprintf(buf, 200, "Power: %d", pPlayer->m_power + 15);
		m_pCharInfo->AddLine(buf, 0, false);
		_snprintf(buf, 200, "Control: %d", (int)pPlayer->m_control);
		m_pCharInfo->AddLine(buf, 0, false);
		_snprintf(buf, 200, "Impact: %d", (int)pPlayer->m_accuracy);
		m_pCharInfo->AddLine(buf, 0, false);
		_snprintf(buf, 200, "Spin: %d", (int)pPlayer->m_spin);
		m_pCharInfo->AddLine(buf, 0, false);
		_snprintf(buf, 200, "Curve: %d", (int)pPlayer->m_curve);
		m_pCharInfo->AddLine(buf, 0, false);
	}

	// playerinfodlg.cpp:140
	unsigned long *pParts = pCharInfo->partTypeIds;
	if (pParts) {
		m_pCharInfo->AddLine("", 0, false);
		_snprintf(buf, 200, "[Parts(TID)]");
		m_pCharInfo->AddLine(buf, 0, false);
		for (int i = 0; i < 24; i++) {
			if (pParts[i]) {
				IFF_STRUCT::sPart *pPart = ItemManager()->FindPart(pParts[i]);
				if (pPart) {
					_snprintf(buf, 200, "%s( %d )", pPart->m_name, pParts[i]);
					m_pCharInfo->AddLine(buf, 0, false);
				}
			}
		}
	}

	// playerinfodlg.cpp:160
	unsigned long *pAuxParts = pCharInfo->auxParts.tids;
	if (pAuxParts) {
		m_pCharInfo->AddLine("", 0, false);
		_snprintf(buf, 200, "[AuxParts(TID)]");
		m_pCharInfo->AddLine(buf, 0, false);
		for (int i = 0; i < 5; i++) {
			if (pAuxParts[i]) {
				IFF_STRUCT::sAuxPart *pAuxPart = ItemManager()->FindAuxPart(pAuxParts[i]);
				if (pAuxPart) {
					_snprintf(buf, 200, "%s( %d )", pAuxPart->m_name, pAuxParts[i]);
					m_pCharInfo->AddLine(buf, 0, false);
				}
			}
		}
	}

	// playerinfodlg.cpp:184
	m_pCharInfo->AddLine("", 0, false);
	_snprintf(buf, 200, "[\xc0\xe5\xba\xf1(TID)]");
	m_pCharInfo->AddLine(buf, 0, false);

	unsigned long clubId = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[MyIndex()].m_clubId;
	std::map<unsigned int, sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_myItemMap.find(clubId);
	IFF_STRUCT::sClubSet *pClubSet = ItemManager()->FindClubSet(it->second.m_typeId);
	if (pClubSet) {
		_snprintf(buf, 200, "%s( %d )", pClubSet->m_name, clubId);
		m_pCharInfo->AddLine(buf, 0, false);
	}

	// playerinfodlg.cpp:197
	unsigned long ballId = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[MyIndex()].m_ballId;
	IFF_STRUCT::sBall *pBall = ItemManager()->FindBall(ballId);
	if (pBall) {
		_snprintf(buf, 200, "%s( %d )", pBall->m_name, ballId);
		m_pCharInfo->AddLine(buf, 0, false);
	}

	// playerinfodlg.cpp:212
	_snprintf(buf, 200, "[\xc4\xb3\xb5\xf0(TID)]");
	m_pCadInfo->AddLine(buf, 0, false);

	sCaddieInfo *pCaddieInfo = &WSingleton<CSharedDoc>::Instance()->m_roomPlayers[MyIndex()].m_caddieInfo;
	IFF_STRUCT::sCaddie *pCaddie = ItemManager()->FindCaddie(pCaddieInfo->typeId);
	if (pCaddie) {
		_snprintf(buf, 200, "\xc0\xcc  \xb8\xa7: %s( %d )", pCaddie->m_name, pCaddieInfo->typeId);
		m_pCadInfo->AddLine(buf, 0, false);
		_snprintf(buf, 200, "\xb7\xb9  \xba\xa7: %d", pCaddieInfo->level);
		m_pCadInfo->AddLine(buf, 0, false);
		_snprintf(buf, 200, "\xb0\xe6\xc7\xe8\xc4\xa1: %d", pCaddieInfo->experience);
		m_pCadInfo->AddLine(buf, 0, false);

		if (pCaddieInfo->cadItemTypeId) {
			IFF_STRUCT::sCadItem *pCadItem = ItemManager()->FindCadItem(pCaddieInfo->cadItemTypeId);
			if (pCadItem) {
				m_pCadInfo->AddLine("", 0, false);
				_snprintf(buf, 200, "[Parts(TID)]", pCadItem->m_name, pCaddieInfo->cadItemTypeId);
				m_pCadInfo->AddLine(buf, 0, false);
				_snprintf(buf, 200, "%s( %d )", pCadItem->m_name, pCaddieInfo->cadItemTypeId);
				m_pCadInfo->AddLine(buf, 0, false);
			}
		}
	} else {
		m_pCadInfo->AddLine("\xbe\xf8\xc0\xbd", 0, false);
	}
}

// playerinfodlg.cpp:254
void FrPlayerInfoDlg::OnUserInfoInit(intptr_t param) {
	m_pUserInfo = DYNAMIC_CAST(FrEdit, param);
}

// playerinfodlg.cpp:260
void FrPlayerInfoDlg::OnCharInfoInit(intptr_t param) {
	m_pCharInfo = DYNAMIC_CAST(FrEdit, param);
}

// playerinfodlg.cpp:266
void FrPlayerInfoDlg::OnCadInfoInit(intptr_t param) {
	m_pCadInfo = DYNAMIC_CAST(FrEdit, param);
}
