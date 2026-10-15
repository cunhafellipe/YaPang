// Original: d:\build\custom\temp\code\client\projectg\newrecorddlg.cpp
#include "NewRecordDlg.h"
#include "ObjectFactory.h"
#include "SharedDoc.h"
#include "shared/classdefine.h"
#include "frlistbox.h"
#include "fresh.h"
#include "bitmap.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "commonutil.h"
#include "s5/Utilities.h"
#include "s5/classicserver/ClassicServer.h"

// Course order of the list (low byte of the course type ids, 0x28000000 | n).
static const unsigned char data_00a244fc[18] = {
	19, 16, 15, 14, 13, 11, 8, 10, 0, 1, 2, 3, 4, 5, 6, 7, 9, 127
};

// newrecorddlg.cpp:18 (run-time type and factory registration of FrNewRecordDlg)
const WRTTI FrNewRecordDlg::m_RTTI("FrNewRecordDlg", &FrForm::m_RTTI);

IObject* FrNewRecordDlgMakeInstance(void) {
	return new FrNewRecordDlg;
}

struct __sFrNewRecordDlg {
	__sFrNewRecordDlg(void) {
		ObjectFactory().AddObjectFunctor(FrNewRecordDlgMakeInstance, "FrNewRecordDlg");
	}
};
static __sFrNewRecordDlg s_registerFrNewRecordDlg;

// newrecorddlg.cpp:26
const sFRESH_MSGMAP* FrNewRecordDlg::GetMessageMap(void) const {
	return &FrNewRecordDlg::_MsgMap;
}

sFRESH_MSGMAP FrNewRecordDlg::_MsgMap = { &FrForm::_MsgMap, &FrNewRecordDlg::_MsgEntries[0] };

sFRESH_ENTRY FrNewRecordDlg::_MsgEntries[] = {
	// newrecorddlg.cpp:28
	{ "courselist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrNewRecordDlg::OnCourseListInit },
	// newrecorddlg.cpp:29
	{ "courselist", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrNewRecordDlg::OnCourseListOwnerDraw },

	// newrecorddlg.cpp:31
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// newrecorddlg.cpp:34
void FrNewRecordDlg::OnCourseListInit(intptr_t param) {
	m_pCourseList = DYNAMIC_CAST(FrListBox, param);

	if (m_pCourseList == 0) {
		return;
	}

	std::map<unsigned int, IFF_STRUCT::sCourse>& courseMap = ItemManager()->m_courseMap;

#ifdef JP_PAKS
	// JP data: the lobby's JP order (SharedDoc.h), which has every JP course.
	for (int i = 0; i < JP_COURSE_ORDER_COUNT; i++) {
		std::map<unsigned int, IFF_STRUCT::sCourse>::iterator it = courseMap.find(s_courseOrder[i] | 0x28000000);
#else
	for (int i = 0; i < 18; i++) {
		std::map<unsigned int, IFF_STRUCT::sCourse>::iterator it = courseMap.find(data_00a244fc[i] | 0x28000000);
#endif
		if (it == courseMap.end()) {
			continue;
		}

		IFF_STRUCT::sCourse* pCourse = &it->second;

		if (!pCourse->m_active) {
			continue;
		}

		m_pCourseList->AddItem(pCourse);
	}
}

// newrecorddlg.cpp:62
void FrNewRecordDlg::OnCourseListOwnerDraw(int param) {
	FrListItem* pItem = (FrListItem*)param;

	if (pItem == 0)
		return;

	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();

	if (pGDI == 0)
		return;

	IFF_STRUCT::sCourse* pCourse = (IFF_STRUCT::sCourse*)pItem->pData;

	if (pCourse == 0)
		return;

	const Bitmap* pBitmap = pManager->GetBitmap("COURSE", MakeStr("small_%s", pCourse->m_icon));

	if (pBitmap == 0)
		return;

	int course = pCourse->m_typeId & 0x3FFFFFF;
#ifdef JP_PAKS
	// JP data: the statistics have 20 rows (courses 0-19, the packet's
	// layout); a later course (0x14, 0x15, 0x40) or random has no record.
	if (course >= 20) {
		pGDI->DrawTexture(pBitmap, WRect(pItem->pos.x, pItem->pos.y, (float)pBitmap->Width(), (float)pBitmap->Height()), 0x60000000, 0);
		return;
	}
#endif

	sMapStatistics* pStat = S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty())
		? &WSingleton<CSharedDoc>::Instance()->m_myMapStatistics2[course]
		: &WSingleton<CSharedDoc>::Instance()->m_myMapStatistics[course];

	int score = pStat->m_bestScore;
	WRect rect(pItem->pos.x, pItem->pos.y, (float)pBitmap->Width(), (float)pBitmap->Height());

	pGDI->DrawTexture(pBitmap, rect, (score < 0 && score != 0x7F) ? 0xFFFFFFFF : 0x60000000, 0);
}
