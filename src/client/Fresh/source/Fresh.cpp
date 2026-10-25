// Original: d:\build\custom\temp\code\client\fresh\source\fresh.cpp
#include <string.h>

#include "fresh.h"
#include "frdesktop.h"
#include "frwndmanager.h"

#ifdef JP_PAKS
// The game's Fresh (JP 0xf022d0; ProjectG.cpp creates it): defined for the JP port build only, whose Refresh game code
// (src/jp) reads it; the matching build links nothing and
// leaves it undefined.
Fresh* g_pFresh = NULL;
#endif

// fresh.cpp:19
Fresh::Fresh(void) : m_pWndManager(NULL), m_bShow(true) {
}

// fresh.cpp:23
Fresh::~Fresh(void) {
	if (m_pWndManager) { delete m_pWndManager; m_pWndManager = NULL; }
}

// fresh.cpp:28
void Fresh::ShutDown(void) {
	if (m_pWndManager) { delete m_pWndManager; m_pWndManager = NULL; }
}

// fresh.cpp:33
bool Fresh::Init(const char* pszXmlFilename, bool exclusiveKey, const char* wallPaper) {
	if (!m_doc.Load(pszXmlFilename))
		return false;

	m_xmlFileName = pszXmlFilename;
	m_pWndManager = new FrWndManager(&m_doc);
	m_pWndManager->Init(wallPaper, exclusiveKey);
	return true;
}

// fresh.cpp:47
void Fresh::OnProcess(const float deltaTime) {
	if (m_pWndManager && m_bShow)
		m_pWndManager->Process(deltaTime);
}

// fresh.cpp:54
void Fresh::OnDisplay(bool drawDesktop) {
	if (m_pWndManager && m_bShow)
		m_pWndManager->Display(drawDesktop);
}

// fresh.cpp:62
bool Fresh::OpenLayout(const char* layout, FrCmdTarget* owner, bool first) {
	if (m_pWndManager == NULL)
		return false;

	m_pWndManager->CloseLayout();
	return m_pWndManager->CreateLayout(layout, owner, first);
}

// fresh.cpp:84
void Fresh::CloseLayout(void) {
	if (m_pWndManager)
		m_pWndManager->CloseLayout();
}

// fresh.cpp:91
bool Fresh::IsDesktopFocused(void) const {
	if (m_pWndManager == NULL)
		return false;

	FrDesktop* pDesktop = m_pWndManager->GetDesktop();
	if (pDesktop == NULL)
		return false;

	return pDesktop->m_nFlags.GetFlag(0x8) ? true : false;
}

// fresh.cpp:105
const std::map<std::string, resBitmap_t>& Fresh::ListBres(std::string& name) {
	name = m_xmlFileName;
	return m_doc.GetBitmapList();
}

// fresh.cpp:111
void Fresh::HidePrivacy(bool hide) {
	m_pWndManager->HidePrivacy(hide);
}

// fresh.cpp:116
bool Fresh::IsCurrentLayout(const char* layout) const {
	return stricmp(m_pWndManager->GetLayoutID(), layout) == 0 ? true : false;
}

// fresh.cpp:124
const Bitmap* Fresh::RegisterBitmap(const char* bitmap) {
	return m_pWndManager->GetBitmap("", bitmap);
}

// fresh.cpp:130
const Bitmap* Fresh::GetBitmap(const char* bitmap) {
	return m_pWndManager->GetBitmap("", bitmap);
}
