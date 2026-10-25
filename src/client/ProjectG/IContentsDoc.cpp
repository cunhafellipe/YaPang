// Original: d:\build\custom\temp\code\client\projectg\icontentsdoc.cpp
#include "icontentsdoc.h"
#include "contentsdoc.h"
#include "GatewayDoc.h"

#include <windows.h>

#include <utility>

#ifdef JP_PAKS
// JP 983: the daily-quest document (type 0xAC, src/jp/JpQuestForms.h).
namespace _gateway {
namespace _questoneday {
IContentsDataContainer* JpCreateQuestDocument(void);
} // namespace _questoneday
// JP 983: the 2014 Arin birthday event document (type 0xB0,
// src/jp/JpArinEvent.h).
IContentsDataContainer* JpCreateArinDocument(void);
} // namespace _gateway
// JP 983: the lobby data container (type 0xA5, CLobbyContainer,
// src/jp/JpGameRoomDlg.cpp).
IContentsDataContainer* JpCreateLobbyContainer(void);
// JP 983: the tutorial data container (type 0xA4, CTutorialContainer,
// src/jp/JpTutorialWidget.cpp).
IContentsDataContainer* JpCreateTutorialContainer(void);
// JP 983: the creator-vector documents (src/jp/JpChipInContainer.h,
// JpContentsGatewayDocs.h): 0xaa CHoleInOneGameDoc (0x579370), 0xad
// CControllerDoc (0x4baa50), 0xae CWorkShop2013Doc (0x9202c0), 0xaf
// CGrandPrixDocument (0x559c00).
IContentsDataContainer* JpCreateChipInContainer(void);
IContentsDataContainer* JpCreateControllerDoc(void);
IContentsDataContainer* JpCreateWorkShop2013Doc(void);
IContentsDataContainer* JpCreateGrandPrixDocument(void);
#endif

// icontentsdoc.cpp:17
CContentsDoc::CContentsDoc(void) : m_allInserted(TRUE) {
}

// icontentsdoc.cpp:24
CContentsDoc::~CContentsDoc(void) {
	Release();
}

// icontentsdoc.cpp:30
bool CContentsDoc::Initialize(void) {
	InsertContainer((localContentType_t)0x1d, new CGiftBoxEvent, true);
	InsertContainer((localContentType_t)0x1e, new CWorldTourEvent, false);
	InsertContainer((localContentType_t)0x21, new CHalloweenEvent, false);
	InsertContainer((localContentType_t)0x24, new CGiftBoxEvent, false);
	InsertContainer((localContentType_t)0x4c, new CChristmasEvent, false);
	InsertContainer((localContentType_t)0x7a, new CTicketExchange, false);
	InsertContainer((localContentType_t)0x83, new CMissionEvent, false);

	InsertContainer((localContentType_t)0x92, new CPeriodContents, false);

	InsertContainer((localContentType_t)0x97, new _guild::CGuildInfo, false);

	InsertContainer((localContentType_t)0x9c, new CChristmasSockEvent, false);

	InsertContainer((localContentType_t)0x9d, new _gateway::CSecondPwdDoc, false);

	InsertContainer((localContentType_t)0x9e, new _gateway::CGolfItemDoc, false);

	InsertContainer((localContentType_t)0x9f, new _gateway::CTradeDoc, false);

	InsertContainer((localContentType_t)0xa2, new CVectorSlideTestContainer, false);

	InsertContainer((localContentType_t)0xa3, new CGimmickContainer, false);

	InsertContainer((localContentType_t)0xa0, new _gateway::CSpecialBoxDoc, false);

#ifdef JP_PAKS
	// JP 0x915f9f: `new CTutorialContainer` as type 0xA4 through
	// 0x913c70(0xa4, container, false). The KR local-contents bitset
	// (Localize.cpp, 164 bits) cannot hold 0xA4: forced here.
	InsertContainer((localContentType_t)0xa4, JpCreateTutorialContainer(), true);
	// JP 0x915fe1: the last direct insert, `new CLobbyContainer` as type
	// 0xA5 through 0x913c70(0xa5, container, false). The KR local-contents
	// bitset (Localize.cpp, 164 bits) cannot hold 0xA5: forced here.
	InsertContainer((localContentType_t)0xa5, JpCreateLobbyContainer(), true);
	// The creator 0x579370 (registered by 0x579540): new
	// CHoleInOneGameDoc(0xAA).
	InsertContainer((localContentType_t)0xaa, JpCreateChipInContainer(), true);
	// The creators 0x4baa50, 0x9202c0, 0x559c00: CControllerDoc(0xAD),
	// CWorkShop2013Doc(0xAE), CGrandPrixDocument(0xAF).
	InsertContainer((localContentType_t)0xad, JpCreateControllerDoc(), true);
	InsertContainer((localContentType_t)0xae, JpCreateWorkShop2013Doc(), true);
	InsertContainer((localContentType_t)0xaf, JpCreateGrandPrixDocument(), true);
	// JP CContentsDoc::Initialize 0x915da0 ends by running the registered
	// creators (vector 0xe7e140; 0x5ee040 = new CQuestDocument(0xAC)) through
	// 0x913d10, which inserts without the IsLocalContent test: forced here.
	InsertContainer((localContentType_t)0xac, _gateway::_questoneday::JpCreateQuestDocument(), true);
	// The creator 0x920be0 (registered by 0x920c70): new
	// CArinBirthDay2014Doc(0xB0).
	InsertContainer((localContentType_t)0xb0, _gateway::JpCreateArinDocument(), true);
#endif

	return m_allInserted == TRUE ? true : false;
}

// icontentsdoc.cpp:100
bool CContentsDoc::Release(void) {
	bool result = true;

	for (std::map<localContentType_t, IContentsDataContainer *>::iterator it = m_containers.begin(); it != m_containers.end(); ++it) {
		if (it->second != NULL) {
			if (!it->second->Release()) {
				result = false;
			}

			delete it->second;
			it->second = NULL;
		}
	}

	m_containers.clear();

	return result;
}

// icontentsdoc.cpp:143
int CContentsDoc::InsertContainer(localContentType_t type, IContentsDataContainer *container, bool force) {
	int result = TRUE;

	if ((IsLocalContent(type) && container != NULL) || force == true) {
		if (container->Initialize()) {
			m_containers.insert(std::make_pair(type, container));
		} else {
			result = FALSE;
		}
	} else {
		if (container != NULL) {
			delete container;
		} else {
			result = FALSE;
		}
	}

	m_allInserted &= result;

	return result;
}

// icontentsdoc.cpp:181
IContentsDataContainer *CContentsDoc::GetContainer(localContentType_t type) {
	std::map<localContentType_t, IContentsDataContainer *>::iterator it;

	it = m_containers.find(type);

	if (it != m_containers.end()) {
		return it->second;
	}

	return NULL;
}
