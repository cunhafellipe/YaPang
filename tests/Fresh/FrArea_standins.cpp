// Inert stand-ins for the Fresh/Wangreal units that FrArea.cpp references but
// that are not host-buildable (FrWnd pulls the whole window manager, GDI and
// the Wangreal view stack). The host unit test compiles the real
// src/client/Fresh/source/FrArea.cpp and links these definitions in its place,
// exactly like the Valkyrie tests do for their heavy dependencies. Nothing here
// is part of src/.
//
// A few of the stubs also record their arguments so the fixture can observe the
// calls FrArea makes (the owner commands, the cursor, the texture draw).
#include <cstdint>

#include "frarea.h"
#include "frgraphicinterface.h"
#include "frwndmanager.h"
#include "frelement.h"
#include "ObjectFactory.h"
#include "RTTI.h"
#include "client/Wangreal/include/bitmap.h"

namespace testrec {
// Last eFrCmd handed to FrWnd::SendCmdToOwnerTarget (FRCMD_NONE = none yet).
eFrCmd cmd = FRCMD_NONE;
intptr_t cmdParam = 0;
int cmdCount = 0;

// Last cursor passed to FrWnd::SetCursor (-1 = never called).
int cursor = -1;

// Last texture draw GDI()->DrawTexture was asked to perform.
bool drew = false;
const Bitmap* drawBitmap = 0;
WRect drawSrc;
WRect drawDst;
unsigned long drawDiffuse = 0;
} // namespace testrec

// --- RTTI ------------------------------------------------------------------
const WRTTI IObject::m_RTTI("IObject", 0);
const WRTTI FrWnd::m_RTTI("FrWnd", &IObject::m_RTTI);

WRTTI::WRTTI(const char* pName, const WRTTI* pBaseRTTI)
	: m_pName(pName), m_pBaseRTTI(pBaseRTTI) {
}

// --- IObject / FrCmdTarget -------------------------------------------------
IObject::IObject(void) {
}

IObject::~IObject(void) {
}

FrCmdTarget::FrCmdTarget(void) {
}

FrCmdTarget::~FrCmdTarget(void) {
}

const sFRESH_MSGMAP* FrCmdTarget::GetMessageMap(void) const {
	return 0;
}

// --- FrWnd -----------------------------------------------------------------
// Only the state the tested FrArea paths read is initialised; the rest of the
// base window is left default-constructed.
FrWnd::FrWnd(void) {
	m_pParentWnd = 0;
	m_pOwner = 0;
	m_pWndManager = 0;
	m_pScrBar = 0;
	m_szPushSound = 0;
	m_pToolTipData = 0;
	m_wndAlpha = 1.0f;
	m_wndAlpha2 = 1.0f;
	m_dwStyle.Reset();
	m_nFlags.Reset();
}

FrWnd::~FrWnd(void) {
}

bool FrWnd::Close(bool) { return true; }
bool FrWnd::Create(const char*, const char*, FrWndManager*, unsigned long, const WRect&, FrWnd*) { return true; }
void FrWnd::PreCreateWindow(FrWndManager*, unsigned long, const WRect&, FrWnd*) {}
void FrWnd::MoveWindow(const WPoint&) {}
void FrWnd::SetVisible(bool) {}
void FrWnd::OnDraw(void) {}
void FrWnd::OnResize(void) {}
void FrWnd::SetCursor(int cursor) { testrec::cursor = cursor; }

bool FrWnd::SendCmdToOwnerTarget(eFrCmd cmd, intptr_t param, sFRESH_HANDLER*) {
	testrec::cmd = cmd;
	testrec::cmdParam = param;
	testrec::cmdCount++;
	return false;
}

FrGraphicInterface* FrWnd::GDI(void) const {
	static FrGraphicInterface gdi;
	return &gdi;
}

// --- document / factory ----------------------------------------------------
FrElementDoc* FrWndManager::GetDocument(void) const { return 0; }
const Bitmap* FrElementDoc::GetBitmap(const std::string&) { return 0; }

void CObjectFactory::AddObjectFunctor(IObjectFunc, const char*) {}

CObjectFactory& ObjectFactory(void) {
	// Leaked on purpose: CObjectFactory::~CObjectFactory calls Reset(), which is
	// defined in ObjectFactory.cpp (not host-buildable) and unused here.
	static CObjectFactory* factory = new CObjectFactory;
	return *factory;
}

// --- GDI -------------------------------------------------------------------
void FrGraphicInterface::DrawTexture(const Bitmap* pBitmap, const WRect& src, const WRect& dest, unsigned long diffuse, int) const {
	testrec::drew = true;
	testrec::drawBitmap = pBitmap;
	testrec::drawSrc = src;
	testrec::drawDst = dest;
	testrec::drawDiffuse = diffuse;
}

// --- Bitmap ----------------------------------------------------------------
// bitmap.cpp is not compiled on the host; the fixture only needs an object whose
// Width()/Height() read a BITMAPINFO the test owns. The fixture fills bi in.
Bitmap::Bitmap(void) : bi(0), pitch(0), vram(0), m_lock(false) {
}

Bitmap::~Bitmap(void) {
}
