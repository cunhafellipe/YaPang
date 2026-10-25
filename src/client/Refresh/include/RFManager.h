// Original: client\Refresh\include\RFManager.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFMANAGER_H
#define PG_CLIENT_REFRESH_INCLUDE_RFMANAGER_H

#include <stdint.h>

#include <list>
#include <string>
#include <vector>

#include "defines.h"
#include "padding.h"
#include "RFEventArgs.h"
#include "RFEventHandler.h"
#include "RFExport.h"
#include "RFRect.h"
#include "RFSingleton.h"
#include "RFSize.h"
#include "RFTypes.h"
#include "RFVector2.h"
#include "RFWnd.h"

struct HWND__;

class RFPoint;
class RFRenderer;
class RFInterface;
class RFInputMethodProvider;
class RFVisualEffect;
class RFWindowFactoryManager;
class RFXmlLoader;
// The holder SetRenderer(wstring const&, void*) 0x418500 keeps the renderer
// DLL's HMODULE in (size 0x4; defined in RFManager.cpp).
struct RFRendererLibrary;

// The Refresh manager singleton (GetInstance 0x417450): vtable 0xd630b8,
// size 0x98 (from its scalar deleting destructor; the members come with
// the layout). Exported (every member is, the implicit copy constructor
// 0x4030b0 and copy assignment 0x403230 too): dllexport, so cl 16 emits
// the header inlines where they pair. An event handler (slot 0 is
// RFEventHandler::ToString; the constructor 0x417490 builds the event map
// at +0x4 first) and a singleton (the empty base at +0x30).
class RF_API RFManager: public RFEventHandler, public RFSingleton<RFManager> {
	LAYOUT_CHECK_FRIEND;
	// RFWnd::Activate 0x406030 and Deactivate 0x406090 read and write
	// m_activeWnd directly; RFIme posts PostIMStatusEvent. The game calls
	// the protected HitTest2 from RFSceneManager::UpdateMouseOver (0x603be7)
	// and RFFormDesktopBase::IsMouseOverOtherWindow (0x5f8a3b), and
	// RFTooltip::OnTooltipUpdate (0x5ce8c0).
	friend class RFWnd;
	friend class RFIme;
	friend class RFTextArea;
	friend class RFSceneManager;
	template <class T> friend class RFFormDesktopBase;
	friend class RFTooltip;

public:
	RFManager();                      //      0x417490
	// [0] RFEventHandler::ToString 0x401a90
	virtual ~RFManager();            // [1]  vector deleting destructor 0x403360, ~RFManager 0x4177f0
	// The manager's own copy (src\RFManager.cpp line 58), not
	// RFSingleton<RFManager>::GetInstance.
	static RFManager& GetInstance();  //      0x417450
	static RFManager *GetInstancePtr();  // 0x417480
	// The header inlines 0x402f10-0x4033f0: their copies sit in the
	// header-inline COMDAT run with CreateDesktopA/W and the implicit copy
	// constructor 0x4030b0 / assignment 0x403230 (L17).
	RFWnd *GetFocusWnd() const { return m_focusWnd; }  // 0x402f10
	void SetCapturedWindow(RFWnd *wnd) { m_capturedWindow = wnd; }  // 0x402f20
	RFWnd *GetCapturedWindow() const { return m_capturedWindow; }  // 0x402f30
	void SetTopMost(RFWnd *wnd) { wnd->SetPriority(-1); }  // 0x402f40
	void SetGlobalPadding(RFRect const& padding) { m_globalPadding = padding; }  // 0x402f70
	RFRect GetGlobalPadding() const { return m_globalPadding; }  // 0x402fa0
	void EnableGlobalPadding(bool enable) { m_globalPaddingEnabled = enable; }  // 0x402fd0
	bool IsGlobalPaddingEnabled() const { return m_globalPaddingEnabled; }  // 0x402fe0
	RFSize GetResolution() const { return m_root->GetSize(); }  // 0x402ff0
	// 0x403010
	void SetResolution(RFSize const& size) {
		RFWnd *root = m_root;
		if (root->m_localSize.width != size.width || root->m_localSize.height != size.height) {
			root->m_localSize = size;
			root->OnLayoutChanged(2);
		}
	}
	// 0x403060
	void UpdateLayout() {
		RFWnd *root = m_root;
		root->OnLayoutChanged(0x10000000);
		std::vector<RFWnd*>::iterator end = root->_children.end();
		for (std::vector<RFWnd*>::iterator it = root->_children.begin(); it != end; ++it) {
			(*it)->UpdateLayout();
		}
	}
	RFVector2 const& GetCursorPos() const { return m_cursorPos; }  // 0x4033c0
	void SetCursorPos(RFVector2 const& pos) { m_cursorPos = pos; }  // 0x4033d0
	RFWnd *FindWnd(float x, float y) { return HitTest2(RFVector2(x, y), true, true); }  // 0x4033f0
	// 0x417a20, 0x402e30 (CreateDesktopA), 0x402ea0 (CreateDesktopW): a
	// header inline, emitted per including object (windows.h's CreateDesktop
	// macro renamed it where it was included first). Only one desktop.
	RFWnd *CreateDesktop(std::wstring const& name, RFWnd *parent) {
		if (m_desktop != 0) {
			return 0;
		}
		RFWnd *desktop = CreateWnd(name, parent, 0);
		if (desktop == 0) {
			return desktop;
		}
		RegisterDesktop(desktop);
		return desktop;
	}
	RFWnd *CreateWnd(std::wstring const&, RFWnd *, RFWnd *);  // 0x417a90
	void FreezeInputs(bool);          //      0x417e70
	bool IsInputsFroze() const;       //      0x417e80
	void ActivateWindow(RFWnd *);     //      0x4182f0
	void FocusWindow(RFWnd *);        //      0x418340
	R_ERRORTYPE DeleteDesktop();      //      0x4183b0
	R_ERRORTYPE DeleteWnd(RFWnd *);   //      0x418410
	void DeleteAllWnd();              //      0x418460
	void RegisterDesktop(RFWnd *);    //      0x418470
	RFWnd *GetDesktop();              //      0x4184b0
	RFWnd *GetWnd(std::wstring const&);   //  0x4184c0
	RFWnd *GetInputTargetWindow() const;  //  0x4184d0
	void SetRenderer(RFRenderer *);   //      0x4184e0
	// Loads the renderer DLL `library` and builds the renderer with its
	// createRenderer(param, interface) export.
	void SetRenderer(std::wstring const& library, void *param);  // 0x418500
	// 0x418630 (inlined into the RFDirector constructor)
	RFRenderer *GetRenderer() {
		return m_renderer;
	}
	void SetInterface(RFInterface *); //      0x418640
	// 0x418660 (inlined into the RFDirector constructor)
	RFInterface *GetInterface() {
		return m_interface;
	}
	// `custom` picks the library's RFCustomIme over the system RFIme.
	void InitializeInputMethod(HWND__ *, bool custom);  // 0x418670
	void RestoreInputMethod();        //      0x4187a0
	RFInputMethodProvider *GetInputMethod();  // 0x4187c0
	void BringWndToTop(RFWnd *);      //      0x4187d0
	void BringControlToTop(RFWnd *);  //      0x4187e0
	// A pointer below 0x10 is an effect index (CreateVisualEffect 0x42cfa0).
	void SetVisualEffect(RFVisualEffect *);  // 0x418860
	wchar_t const* GetVersion() const;        //      0x4188a0
	RFWnd *GetLastActiveModal() const;  //    0x418950
	bool IsModalExist() const;        //      0x418990
	RFWnd *FindActiveFocusableWnd(RFWnd *);  //  0x4189d0
	bool InjectLButtonDown(float, float);      // 0x418ad0
	bool InjectLButtonUp(float, float);        // 0x418be0
	bool InjectLButtonDblClick(float, float);  // 0x418ce0
	bool InjectRButtonDown(float, float);      // 0x418df0
	bool InjectRButtonUp(float, float);        // 0x418f00
	bool InjectMouseMove(float, float);        // 0x419010
	bool InjectMouseWheel(int32_t, float, float);  // 0x419180
	bool InjectKeyDown(uint32_t, uint32_t);  // 0x419280
	bool InjectKeyUp(uint32_t, uint32_t);    // 0x419340
	bool InjectChar(wchar_t *, bool, RFPoint const&);  // 0x419400
	bool InjectPreDraw();             //      0x4196d0
	bool InjectDraw();                //      0x419780
	bool InjectUpdate(float);         //      0x419830
	void NotifyWindowGotFocus(RFWnd *);     // 0x4198d0
	void NotifyWindowLostFocus(RFWnd *);    // 0x419900
	void NotifyWindowActivated(RFWnd *);    // 0x419920
	void NotifyWindowDeactivated(RFWnd *);  // 0x419950
	void NotifyWindowDestroyed(RFWnd *);    // 0x419970
	std::wstring const& GetCompatibleControlName(WndType);  // 0x4199f0
	bool IsUsingCustomInputMethod();  //      0x419c30
	void UpdateInputMethod();         //      0x419c40

	// The mirrors SetRenderer and SetInterface keep of the manager's
	// renderer and interface; RFWnd::GetRenderer and GetInterface read them.
	static RFRenderer *s_renderer;    // 0xF1DDB0 (SetRenderer 0x4184e0)
	static RFInterface *s_interface;  // 0xF1DDA4 (SetInterface 0x418640)
	// The registered desktop's mirror (RegisterDesktop 0x418470 sets it,
	// DeleteDesktop 0x4183b0 clears it).
	static RFWnd *s_desktop;          // 0xF1DDAC
	// The root window's mirror (m_root +0x4c; the constructor 0x417490 stores
	// it): CreateWnd's default anchor parent; RFWnd::RegisterChild,
	// UpdateAnchorValue, Draw and PreDraw read it. Not exported.
	static RFWnd *s_root;             // 0xF1DDA8

	// Ours: the JP game's RFFormDesktopBase<RFWndEx>::Initialize (0x5f7840)
	// reads m_root (+0x4c) directly. No out-of-line copy in Refresh.
	RFWnd *GetRootWnd() const { return m_root; }

	// The manager's events (exported statics; the constructor 0x417490 adds
	// them).
	static std::wstring const EventHyperLink;                 // 0xE46BF4
	static std::wstring const EventGlobalHyperLinkEnter;      // 0xE46C10
	static std::wstring const EventGlobalHyperLinkLeave;      // 0xE46C2C
	static std::wstring const EventInputMethodStatusChanged;  // 0xE46C48

protected:
	// Builds `parent`'s children from the loader's structs, depth first.
	void CreateChild(RFXmlLoader&, RFWnd *parent, WndBaseStruct *parentStruct);  // 0x417e90
	void PostHyperLinkEvent(RFWnd *, std::wstring const&, std::wstring const&);  // 0x417fd0
	void PostGlobalHyperLinkEnterEvent(RFWnd *, std::wstring const&, std::wstring const&);  // 0x4180e0
	void PostGlobalHyperLinkLeaveEvent(RFWnd *);  // 0x4181f0
	void PostIMStatusEvent(LocaleIndicatorTypes::Type const&, uint32_t const&);  // 0x418270
	RFWnd *HitTest(RFVector2 const&, bool);         // 0x4188b0
	RFWnd *HitTest2(RFVector2 const&, bool, bool);  // 0x4188d0
	void UpdateWindowContainingMouse(RFVector2 const&);  // 0x418a60

private:
	// A new'd 4-byte holder of the renderer DLL's HMODULE: SetRenderer(wstring
	// const&, void*) 0x418500 FreeLibrary's and deletes the old one, then
	// LoadLibraryW's the named DLL into a new one; zeroed by the constructor.
	RFRendererLibrary *m_rendererLibrary;  // 0x34
	RFRenderer *m_renderer;           // 0x38
	RFInterface *m_interface;         // 0x3c (GetInterface 0x418660)
	RFInputMethodProvider *m_inputMethod;  // 0x40 (GetInputMethod 0x4187c0)
	RFWindowFactoryManager *m_windowFactoryManager;  // 0x44 (the constructor 0x417490 news it and AddFactory's each control)
	bool m_inputsFroze;               // 0x48
	RFWnd *m_root;                    // 0x4c the root window holding the desktops (GetWnd 0x4184c0)
	RFWnd *m_desktop;                 // 0x50
	RFWnd *m_activeWnd;               // 0x54 (NotifyWindowDeactivated 0x419950)
	RFWnd *m_focusWnd;                // 0x58
	RFWnd *m_capturedWindow;          // 0x5c
	RFWnd *m_wndContainingMouse;      // 0x60 (UpdateWindowContainingMouse 0x418a60)
	RFVisualEffect *m_visualEffect;   // 0x64 (SetVisualEffect 0x418860)
	// No reader identified yet; the implicit copy assignment 0x403230 copies
	// each as a dword.
	uint32_t m_reserved68;            // 0x68
	uint32_t m_reserved6c;            // 0x6c
	RFVector2 m_cursorPos;            // 0x70
	RFRect m_globalPadding;           // 0x78 (SetGlobalPadding 0x402f70)
	bool m_globalPaddingEnabled;      // 0x88
	bool m_usingCustomInputMethod;    // 0x89
	// Destroyed by ~RFManager 0x4177f0 (clear 0x40c170 + the head node),
	// copied by the implicit copy constructor 0x4030b0 and assignment
	// 0x403230; no reader identified yet.
	std::list<std::wstring> m_stringList;  // 0x8c
};
CHECK_SIZE(RFManager, 0x98);
BEGIN_LAYOUT_CHECK(RFManager)
	CHECK_FIELD(RFManager, m_rendererLibrary, 0x34);
	CHECK_FIELD(RFManager, m_renderer, 0x38);
	CHECK_FIELD(RFManager, m_interface, 0x3c);
	CHECK_FIELD(RFManager, m_inputMethod, 0x40);
	CHECK_FIELD(RFManager, m_windowFactoryManager, 0x44);
	CHECK_FIELD(RFManager, m_inputsFroze, 0x48);
	CHECK_FIELD(RFManager, m_root, 0x4c);
	CHECK_FIELD(RFManager, m_desktop, 0x50);
	CHECK_FIELD(RFManager, m_activeWnd, 0x54);
	CHECK_FIELD(RFManager, m_focusWnd, 0x58);
	CHECK_FIELD(RFManager, m_capturedWindow, 0x5c);
	CHECK_FIELD(RFManager, m_wndContainingMouse, 0x60);
	CHECK_FIELD(RFManager, m_visualEffect, 0x64);
	CHECK_FIELD(RFManager, m_reserved68, 0x68);
	CHECK_FIELD(RFManager, m_reserved6c, 0x6c);
	CHECK_FIELD(RFManager, m_cursorPos, 0x70);
	CHECK_FIELD(RFManager, m_globalPadding, 0x78);
	CHECK_FIELD(RFManager, m_globalPaddingEnabled, 0x88);
	CHECK_FIELD(RFManager, m_usingCustomInputMethod, 0x89);
	CHECK_FIELD(RFManager, m_stringList, 0x8c);
END_LAYOUT_CHECK

// RFManager.cpp's converters of an XML attribute (not exported), called by
// the properties' Parse in RFProperty.cpp.
bool ParseBoolean(std::string const& string);                           // 0x419d50
uint32_t ParseColor(std::string const& string);                         // 0x419db0
StartPositionTypes::Type ParseStartPosition(std::string const& string); // 0x419ec0
EscKeyActionTypes::Type ParseEscKeyAction(std::string const& string);   // 0x419f10
uint32_t ParseAnchor(std::string const& string);                        // 0x419f30 (name ours, not exported)
HorzAlignTypes::Type ParseHorzAlign(std::string const& string);        // 0x41a170
VertAlignTypes::Type ParseVertAlign(std::string const& string);        // 0x41a240
TextTrimmingTypes::Type ParseTextTrimming(std::string const& string);  // 0x41a310
OrientationTypes::Type ParseOrientation(std::string const& string);    // 0x41a370

// The window creator template: allocate a T, let the manager create it from
// its XML; on failure free it. Instances: 0x4251a0 (RFWndEx, RFList's
// InsertWindowItem), 0x5ffe10 (the game's RFLobbyDesktop). LTCG passes all
// three arguments on the stack.
template <class T>
RFWnd *CreateWnd(RFManager *manager, std::wstring const& name, RFWnd *parent) {
	T *wnd = new T;
	RFWnd *result = manager->CreateWnd(name, wnd, parent);
	if (result == 0) {
		delete wnd;
		result = 0;
	}
	return result;
}

// Its desktop twin: allocate a T, let the manager create it as the desktop
// from its XML; on failure free it. Instances: 0x5ce530
// (RFCommonTooltip), 0x5ce660 (RFCommonTooltipByImage), both in the game
// (LTCG passes the manager in ecx).
template <class T>
RFWnd *CreateDesktopWnd(RFManager *manager, std::wstring const& name) {
	T *wnd = new T;
	RFWnd *result = manager->CreateDesktop(name, wnd);
	if (result == 0) {
		delete wnd;
		result = 0;
	}
	return result;
}

#endif
