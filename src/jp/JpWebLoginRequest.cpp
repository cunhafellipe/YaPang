// The JP 983 game's web-page login (JpWebLoginRequest.h), from the JP exe's
// bodies (node scripts/disasm.ts --jp 0x<va>), written against the KR
// game's APIs (WSendPacket, CBrowser, CSharedDoc).
//
// Packets: client -> server 0xFB (no payload) asks for a web login key;
// server -> client 0x1AD answers: dword result (1 = OK), then the key
// (WReceivedPacket::DecodeStr: word length, bytes).
//
// lib: 0x8aa050 (WSingleton<CWebPageConnector>::WSingleton), 0x8aa070,
// 0x8aa100 (~WSingleton<CWebPageConnector>, the second the implicit
// ~CWebPageConnector), 0x8aa090, 0x8aa120 (their scalar deleting
// destructors), 0x593130 (WSingleton<CWebPageConnector>::Instance),
// 0xa63060 (WSingleton<CWebPageConnector>::IsInstantiated)
#include "JpWebLoginRequest.h"

#include <stdio.h>
#include <string.h>
#include <string>

#include "Browser.h"
#include "Packet.h"
#include "SharedDoc.h"

// JP game code with no KR counterpart, not decompiled (declared only; names
// ours).

// [0xe74c78]->0x9a7740(the JP CSharedDoc dword +0x82a, onDone, loginId,
// loginKey): hands the login to the JP object's request at +0x28
// (0x9ac550: keeps the dword and onDone, copies both strings, runs the
// thread 0x9ac3c0 with _beginthreadex). The club page's background login,
// no browser. The request object (+0x28 of [0xe74c78]): +0x4 the dword (0:
// no thread), +0xc onDone, +0x10 the login id and +0x2c the key
// (std::string). The thread formats "%s/rss.aspx?ID=%s&KID=%s" (0xd3f560)
// with s_clubLoginUrl, the id and the key, downloads it with WinInet
// (0x9abf10: InternetOpenA "Daemon", InternetOpenUrlA, InternetReadFile
// into CreateFileA "guildlast.xml") and hands the outcome to 0x9ac190 (0 on
// success, 1 on failure), which reads guildlast.xml: the club news feed.
// Not decompiled (needs the [0xe74c78] object and a WinInet port shim).
void JpClubLogin_Request(void (*onDone)(void), const char* loginId, const char* loginKey);

// 0xe75cb0
template <>
CWebPageConnector* WSingleton<CWebPageConnector>::m_pInstance = 0;

// 0xd36108: the club page Open is given bare (CTaskManager::OnPacket
// 0x9c3d5f, page 5), the background login rather than a browser page.
static const char s_clubLoginUrl[] = "https://www5.pangya.jp/on_client/club";

// 0x8aa040: the club login's completion callback, empty.
static void OnClubLoginDone(void) {
}

// 0x8aa0d0
CWebPageConnector::CWebPageConnector(void) {
	memset(m_urlFormat, 0, sizeof(m_urlFormat));
}

// 0x8a9fa0
void CWebPageConnector::Open(const char* urlFormat, bool openFlag, int pageId) {
	memset(m_urlFormat, 0, sizeof(m_urlFormat));
	strcpy(m_urlFormat, urlFormat);
	m_pageId = pageId;
	m_openFlag = openFlag;

	WSendPacket packet(0xFB);
	packet.Send(SEND_TO_DEFAULT);
}

// 0x8aa150: the JP CBrowser::Open 0x870d90 takes (url, flag, pageId): it
// also sends packet 0xA1 with the page id, keeps it at +0x120 and calls
// its PrepareOpen(url, flag). The KR CBrowser::Open(url, arg2, arg3) has no
// page id; arg3 is true as in the KR page handlers (CTaskMain::OnUnderBar_*).
void CWebPageConnector::OnLoginKey(const char* loginId, const char* loginKey) {
	if (strlen(m_urlFormat) == 0) {
		return;
	}

	char url[0x200] = { 0 };
	sprintf(url, m_urlFormat, loginId, loginKey);

	if (strcmp(m_urlFormat, s_clubLoginUrl) == 0) {
		JpClubLogin_Request(OnClubLoginDone, loginId, loginKey);
		return;
	}

	WSingleton<CBrowser>::Instance()->Open(url, m_openFlag, true);
}

// 0x9d32c0 (the 0x1AD case of CTaskManager::OnPacket 0x9c20a0). The JP
// passes its CSharedDoc +0x7a2 as the login id: written as the KR
// m_loginId (sPangYaUserInfo::loginId), the account the KID belongs to;
// not proven from a JP accessor.
void CWebPageConnector_OnLoginKeyPacket(WReceivedPacket& packet) {
	std::string loginKey = "";
	unsigned long result = packet.Decode4();
	loginKey = packet.DecodeStr();

	CWebPageConnector* connector = WSingleton<CWebPageConnector>::Instance();
	if (result == 1 && connector) {
		connector->OnLoginKey(WSingleton<CSharedDoc>::Instance()->m_loginId, loginKey.c_str());
	}
}
