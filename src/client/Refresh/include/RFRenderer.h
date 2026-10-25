// Original: client\Refresh\include\RFRenderer.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFRENDERER_H
#define PG_CLIENT_REFRESH_INCLUDE_RFRENDERER_H

#include <stdint.h>
#include <string>

#include "RFExport.h"
#include "RFMatrix.h"
#include "RFSize.h"
#include "RFVector2.h"
#include "defines.h"

class RFRect;
class RFTexture;

// How a texture is tiled when drawn; the XML loader (the "Image" element
// handler at 0x43eb79) reads the TileMode attribute and recognizes the
// values "Stretch" and "Tile"; the full list comes with the loader.
struct TileModeTypes {
	enum Type {
		Stretch,  // "Stretch"
		Tile      // "Tile"
	};
};

// The renderer Refresh draws through; the game implements it as
// RefreshRenderer (vtable 0xd1307c, 11 slots). Slots 5-6 are pure virtual,
// not exported: their names are ours, from the callers and the game's
// overrides. The class is exported like every Refresh class
// (__declspec(dllexport)): that makes cl emit the vector deleting destructor
// (??_E, slot 10, 0x403600) and the implicit members in this unit.
class RF_API RFRenderer {
public:
	virtual RFTexture* CreateTexture(std::wstring const&);    // [0]  0x403510
	virtual void DeleteTexture(RFTexture*);                   // [1]  0x403520
	// cl groups overloaded virtuals and gives them slots in reverse
	// declaration order: the slot-4 overload is declared first.
	virtual void Draw(RFTexture*, RFVector2 const&, RFSize const&, float, float, TileModeTypes::Type const&);  // [4]  0x403530
	virtual void Draw(RFTexture*, float, float, float, float, RFVector2 const&, RFSize const&, float, float, TileModeTypes::Type const&);  // [3]  0x403540
	virtual void Draw(RFTexture*, float, RFMatrix const&);    // [2]  0x403550
	// [5] 0xc3c2f8 _purecall: RFButton/RFCheck/RFWndEx::OnDraw push the
	// window's screen rect intersected with its clip rect (RFWnd +0x160)
	// before drawing; the game's 0x5f5950 sets its clip rects (ret 4).
	virtual void PushClipRect(RFRect const&) = 0;
	// [6] 0xc3c2f8 _purecall: undoes PushClipRect after drawing; the game's
	// 0x5f59d0 resets the clip to the desktop size (no arguments).
	virtual void PopClipRect() = 0;
	virtual void FillRectangle(uint32_t, float, float, float, float, float);  // [7]  0x403560
	virtual void OnPreResetDevice();                          // [8]  0x403570
	virtual void OnPostResetDevice();                         // [9]  0x403580
	virtual ~RFRenderer();                                    // [10] vector deleting destructor 0x403600, ~RFRenderer 0x403590

	RFRenderer();                                             // 0x4035a0
	RFRenderer(RFRenderer const&);                            // 0x4035b0
	RFRenderer& operator=(RFRenderer const&);                 // 0x4035c0
};
CHECK_SIZE(RFRenderer, 0x4);

#endif
