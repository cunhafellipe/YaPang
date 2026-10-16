// Original: d:\build\custom\temp\code\client\projectg\frfindeborteventdlg.cpp
#include "FrFindEbortEventDlg.h"
#include "ObjectFactory.h"
#include "commonutil.h"
#include "fresh.h"
#include "frgraphicinterface.h"
#include "frwndmanager.h"
#include "SharedDoc.h"

// frfindeborteventdlg.cpp:14 (run-time type and factory registration of FrFindEbortEventDlg)
const WRTTI FrFindEbortEventDlg::m_RTTI("FrFindEbortEventDlg", &FrForm::m_RTTI);

IObject* FrFindEbortEventDlgMakeInstance(void) {
	return new FrFindEbortEventDlg;
}

struct __sFrFindEbortEventDlg {
	__sFrFindEbortEventDlg(void) {
		ObjectFactory().AddObjectFunctor(FrFindEbortEventDlgMakeInstance, "FrFindEbortEventDlg");
	}
};
static __sFrFindEbortEventDlg s_registerFrFindEbortEventDlg;

// frfindeborteventdlg.cpp:16
const sFRESH_MSGMAP* FrFindEbortEventDlg::GetMessageMap(void) const {
	return &FrFindEbortEventDlg::_MsgMap;
}

sFRESH_MSGMAP FrFindEbortEventDlg::_MsgMap = { &FrForm::_MsgMap, &FrFindEbortEventDlg::_MsgEntries[0] };

sFRESH_ENTRY FrFindEbortEventDlg::_MsgEntries[] = {
	// frfindeborteventdlg.cpp:18
	{ "current_ebort", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrFindEbortEventDlg::OnCurrentItemOwnerDraw },

	// frfindeborteventdlg.cpp:20
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// frfindeborteventdlg.cpp:27
FrFindEbortEventDlg::FrFindEbortEventDlg(void) : m_reserved140(0) {
	for (int i = 0; i < 4; ++i) { m_itemCounts[i] = 0; m_countPositions[i] = WPoint(0.0f, 0.0f); }
}

// frfindeborteventdlg.cpp:36
FrFindEbortEventDlg::~FrFindEbortEventDlg(void) {
}

// frfindeborteventdlg.cpp:40
void FrFindEbortEventDlg::OnCurrentItemOwnerDraw(PG_MSGARG param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI) {

		int oldSpace = pGDI->SetSpace(0);

		WPoint origin(m_rect.x, m_rect.y);

		for (int i = 0; i < 4; ++i) {
			WPoint pos = WPoint(m_countPositions[i]) + origin;

			pGDI->PrintText(pos, 2, MakeStr("%d", m_itemCounts[i]), 0);
		}

		pGDI->SetSpace(oldSpace);
	}
}

// frfindeborteventdlg.cpp:64
bool FrFindEbortEventDlg::OnInit(void) {
	bool found[4] = { false, }; // which of the four event items the list holds

	for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {

		if ((*it).m_typeId == 0x1A000045) {

			m_itemCounts[0] = (*it).m_count;
			found[0] = true;
		}
		else if ((*it).m_typeId == 0x1A000044) {

			m_itemCounts[1] = (*it).m_count;
			found[1] = true;
		}
		else if ((*it).m_typeId == 0x1A000047) {

			m_itemCounts[2] = (*it).m_count;
			found[2] = true;
		}
		else if ((*it).m_typeId == 0x1A000046) {

			m_itemCounts[3] = (*it).m_count;
			found[3] = true;
		}

		if (found[0] && found[1] && found[2] && found[3]) {
			break;
		}
	}

	m_countPositions[0] = WPoint(551.0f, 59.0f);
	m_countPositions[1] = WPoint(551.0f, 74.0f);
	m_countPositions[2] = WPoint(551.0f, 89.0f);
	m_countPositions[3] = WPoint(551.0f, 104.0f);

	return true;
}
