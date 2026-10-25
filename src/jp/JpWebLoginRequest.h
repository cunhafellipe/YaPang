// The JP 983 game's web-page login (RTTI CWebPageConnector :
// WSingleton<CWebPageConnector> : BaseObject; vtable 0xd36104, constructor
// 0x8aa0d0, instance WSingleton<CWebPageConnector>::m_pInstance 0xe75cb0),
// a JP-only singleton with no KR counterpart. Open keeps the page's URL
// format ("...?ID=%s&KID=%s") and sends packet 0xFB; the server answers
// with 0x1AD (CTaskManager::OnPacket 0x9c20a0: result dword, login key
// string) and OnLoginKey fills the format with the login id and the key and
// opens the page. The KR game opens its pages with CBrowser::Open directly.
//
// Created by CProjectG's initialisation (JP 0xa66d80, `new` of 0x230 bytes
// between the KR CCardManager and CPangFBI), deleted by ~CProjectG 0xa66700.
#ifndef PG_JP_JPWEBLOGINREQUEST_H
#define PG_JP_JPWEBLOGINREQUEST_H

#include "defines.h"
#include "singleton.h"

class WReceivedPacket;

// size 0x230
class CWebPageConnector : public WSingleton<CWebPageConnector> {
	LAYOUT_CHECK_FRIEND;
public:
	CWebPageConnector(void);
	// [0] 0x8aa120 (scalar deleting); the destructor itself (0x8aa100) is
	// the implicit one (~WSingleton<CWebPageConnector>, then ~BaseObject).

	void Open(const char* urlFormat, bool openFlag, int pageId);
	void OnLoginKey(const char* loginId, const char* loginKey);

protected:
	char m_urlFormat[0x200];   // 0x28, the page's sprintf format (ID, KID), empty until Open
	bool m_openFlag;           // 0x228, CBrowser's open flag (every caller passes true)
	int m_pageId;              // 0x22C, the page number the JP CBrowser::Open 0x870d90 reports (packet 0xA1)
};

CHECK_SIZE(CWebPageConnector, 0x230);
BEGIN_LAYOUT_CHECK(CWebPageConnector)
	CHECK_FIELD(CWebPageConnector, m_urlFormat, 0x28);
	CHECK_FIELD(CWebPageConnector, m_openFlag, 0x228);
	CHECK_FIELD(CWebPageConnector, m_pageId, 0x22C);
END_LAYOUT_CHECK

// The 0x1AD case of the JP CTaskManager::OnPacket 0x9c20a0 (jump table
// 0x9d8320 [5], 0x9d32c0): decodes the result dword and the login key; on
// result 1, when the connector exists, OnLoginKey(the login id, the key).
// Names ours.
void CWebPageConnector_OnLoginKeyPacket(WReceivedPacket& packet);

#endif
