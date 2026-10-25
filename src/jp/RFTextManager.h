// The JP 983 game's text service for Refresh (RTTI RFTextManager :
// WSingleton<RFTextManager> : BaseObject, vtable 0xd13d2c, size 0x30;
// constructor 0x602fd0, destructor 0x602d80; instance pointer 0xe78c2c).
// Created by CProjectG::Ready (0xa69400) right after RFSceneManager::Setup.
// Declarations and the JP x86 layout only.
#ifndef PG_JP_RFTEXTMANAGER_H
#define PG_JP_RFTEXTMANAGER_H

#include <stdint.h>
#include "RefreshSharedPtr.h"
#include <string>

#include "defines.h"
#include "singleton.h"

class RFTextDraw;

class RFTextManager : public WSingleton<RFTextManager> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x602fd0: m_textDraw = shared_ptr(new RFTextDraw(g_view 0xf023e8)).
	RFTextManager();
	virtual ~RFTextManager();  // [0] 0x602f00 scalar deleting destructor (body 0x602d80)

	// 0x602c50 / 0x602c90: draw a text at (x, y) with the style's words
	// (RefreshInterface slot 1: font size > 11 → 0x602c50, else 0x602c90).
	void DrawTextLarge(float x, float y, const std::wstring& text, uint32_t, uint32_t, uint32_t, uint32_t);
	void DrawTextSmall(float x, float y, const std::wstring& text, uint32_t, uint32_t, uint32_t, uint32_t);

	// Ours: RefreshInterface and RefreshRenderer read +0x28 directly.
	RFTextDraw* GetTextDraw() const { return m_textDraw.get(); }

protected:
	RF_SHARED_PTR<RFTextDraw> m_textDraw;  // 0x28 (pointer 0x28, control block 0x2c)
};
CHECK_SIZE(RFTextManager, 0x30);
BEGIN_LAYOUT_CHECK(RFTextManager)
	CHECK_FIELD(RFTextManager, m_textDraw, 0x28);
END_LAYOUT_CHECK

// 0xb4eb80 (__fastcall, key in ecx; name ours): the JP game's text for a
// Korean source text, through a function-local translator built on first
// use from korea.dat and japan.dat (RFTextManager.cpp); the top-bar
// tooltips and btn_exit's message box pass it their keys.
const char* Jp_LookupText(const char* key);

#endif
