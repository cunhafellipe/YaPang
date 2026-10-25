// Original: client\Refresh\include\RFInterface.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFINTERFACE_H
#define PG_CLIENT_REFRESH_INCLUDE_RFINTERFACE_H

#include <stdint.h>
#include <string>

#include "RFExport.h"
#include "RFSize.h"
#include "defines.h"

class RFWnd;
struct FontInfo;

// Input-method state change, the argument of OnInputMethodStateChanged
// (slot 12). The names are ours, from what RFIme reports: composition start
// and end (WM_IME_STARTCOMPOSITION / ENDCOMPOSITION), candidate list open and
// close (IMN_OPENCANDIDATE / IMN_CLOSECANDIDATE).
struct InputMethodStateTypes {
	enum Type {
		StartComposition = 0,
		EndComposition = 1,
		OpenCandidate = 2,
		CloseCandidate = 3
	};
};

// Argument of GetBasePath (slot 4): which base directory is wanted. The
// names are ours; the game's override (RefreshInterface 0x5f5170) ignores the
// argument and returns one global path.
enum RFBASEPATHTYPE {
	// RFDirector::LoadAnimation 0x430b8e: the directory of the animation
	// files.
	RF_BASEPATH_ANIMATION = 0
};

// The interface the game gives Refresh: sound, text output, file access,
// text translation, debug output and input notifications; the game
// implements it as RefreshInterface (vtable 0xd13004). Slots 0-3 are pure
// virtual and not exported: their names are ours, their arguments come from
// the callers and the game's overrides (docs/JP_REFRESH_UI.md). The virtual
// destructor is declared last: its vector deleting destructor 0x4036f0 is
// slot 14.
class RF_API RFInterface {
public:
	// [0] RFAnimFrame::PlaySfx (0x42d740) passes a temporary copy of the
	// sound name; the game's 0x5f51b0 strips ".wav" and plays it.
	virtual void PlaySfx(const std::wstring&) = 0;
	// [1] RFWnd::PrintText (0x405700) forwards its arguments and the
	// window's alpha (+0x170); the game's 0x5f4f80 draws with its font
	// (small when size <= 11).
	virtual void PrintText(const FontInfo&, int32_t, int32_t, const std::wstring&, float) = 0;
	// [2] The text's width and height in that font (the game's 0x5f5090,
	// zero without a font).
	virtual RFSize MeasureText(const FontInfo&, const std::wstring&) = 0;
	// [3] A button's mouse state changed: RFButton (MouseEnter_Impl,
	// MouseLeave_Impl, OnLButtonDown/Up, OnMouseLeave) and
	// RFAnimButton::OnLButtonDown pass the button and 0 (normal: left or
	// released outside), 1 (mouse over), 2 (pressed) or 3 (entered while
	// disabled). Empty in the game (0x5f4f60: ret 8).
	virtual void OnButtonStateChanged(RFWnd*, int32_t) = 0;
	virtual std::wstring GetBasePath(RFBASEPATHTYPE);  // [4]  0x42ac40
	virtual void* FOpen(const std::wstring&);          // [5]  0x42ac80
	virtual void FClose(void*);                        // [6]  0x42acb0
	virtual int32_t FTell(void*);                      // [7]  0x42acd0
	virtual int32_t FSeek(void*, int32_t, int32_t);    // [8]  0x42acf0
	virtual int32_t FRead(void*, void*, int32_t);      // [9]  0x42ad10
	virtual std::wstring TranslateText(const std::wstring&);  // [10] 0x42ad40
	virtual void OnKeyboardAcquired(bool);             // [11] 0x403660
	virtual void OnInputMethodStateChanged(InputMethodStateTypes::Type);  // [12] 0x403670
	virtual void DebugPrint(const wchar_t*);           // [13] 0x42ad80
	virtual ~RFInterface();                            // [14] 0x4036f0 vector deleting destructor; body 0x403680

	RFInterface();                               // 0x403690
	RFInterface(const RFInterface&);             // 0x4036a0
	RFInterface& operator=(const RFInterface&);  // 0x4036b0
};
CHECK_SIZE(RFInterface, 0x4);

#endif
