// The JP 983 game's RFInterface (RTTI RefreshInterface, vtable 0xd13004,
// 15 slots, size 0x4: only the vfptr; constructor 0x5f4f50). Created by
// RFSceneManager::Setup (0x604160) and handed to RFManager::SetInterface.
// Declarations only: the port implements it on the pak files and the
// macOS text/sound code (port/refresh/TODO.md, checkpoint 3).
#ifndef PG_JP_REFRESHINTERFACE_H
#define PG_JP_REFRESHINTERFACE_H

#include <stdint.h>
#include <string>

#include "defines.h"
#include "RFInterface.h"
#include "RFSize.h"

class RefreshInterface : public RFInterface {
public:
	RefreshInterface();  // 0x5f4f50

	// Slots 0-3 override RFInterface's pure virtuals (names and arguments
	// from RFInterface.h).

	// [0] 0x5f51b0: converts the name to a narrow string, strips ".wav" and
	// plays it through the game's sound manager (the global at 0xf023f4,
	// its slot 3, with two float constants as volume/pitch).
	virtual void PlaySfx(const std::wstring& name);
	// [1] 0x5f4f80: draws text through RFTextManager (the WSingleton at
	// 0xe78c2c) at (x - 1.0, y); the font size (FontInfo +0x1c) picks the
	// large (> 11: 0x602c50) or small (0x602c90) path and its +0x20..+0x2c
	// words (colours, flags) are passed through; the alpha is not read.
	// Returns at once when the text is empty.
	virtual void PrintText(const FontInfo& font, int32_t x, int32_t y, const std::wstring& text, float alpha);
	// [2] 0x5f5090: the size of a text, from RFTextManager's RFTextDraw
	// (0x602a40); (0, 0) when there is no text manager.
	virtual RFSize MeasureText(const FontInfo& font, const std::wstring& text);
	// [3] 0x5f4f60: empty (ret 8).
	virtual void OnButtonStateChanged(RFWnd* button, int32_t state);

	// [4] 0x5f5170: returns the wide string at 0xe13088 (empty, L"") for every
	// path type (the paks are read by bare file name).
	virtual std::wstring GetBasePath(RFBASEPATHTYPE type);
	// [5] 0x5f50e0: narrows the name and returns
	// g_resrcmng->GetCFile(name, 0xffff) (0xb59c20 on the resource manager
	// at 0xf023f0).
	virtual void* FOpen(const std::wstring& name);
	// [6] 0x5f4ea0: closes the CFile (0xbadc70).
	virtual void FClose(void* file);
	// [7] 0x5f4ec0: the CFile's slot 4 (position).
	virtual int32_t FTell(void* file);
	// [8] 0x5f4ee0: the CFile's slot 5 (seek offset, origin); returns 0.
	virtual int32_t FSeek(void* file, int32_t offset, int32_t origin);
	// [9] 0x5f4f00: the CFile's slot 2 (read buffer, size); returns 0.
	virtual int32_t FRead(void* file, void* buffer, int32_t size);
	// [10] TranslateText, [11] OnKeyboardAcquired, [12]
	// OnInputMethodStateChanged, [13] DebugPrint: inherited from RFInterface.
	// [14] 0x5f5060: scalar deleting destructor (body 0x5f4f70).
	virtual ~RefreshInterface();
};
CHECK_SIZE(RefreshInterface, 0x4);

#endif
