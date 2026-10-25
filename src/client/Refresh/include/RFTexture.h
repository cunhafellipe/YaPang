// Original: client\Refresh\include\RFTexture.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFTEXTURE_H
#define PG_CLIENT_REFRESH_INCLUDE_RFTEXTURE_H

#include "RFExport.h"
#include "defines.h"

// Refresh's base texture: vtable 0xd13d34, 3 slots, no members (its
// constructor only stores the vtable pointer). The game's renderer creates
// the real textures (RFRenderer::CreateTexture). The class is dllexport, as
// the library's exports show: that is what makes cl put the vector deleting
// destructor (0x4034b0, the __ehvec_dtor path) in slot 0 instead of the scalar
// one; clang ignores the attribute in the port.
class RF_API RFTexture {
public:
	virtual ~RFTexture();                       // [0] vector deleting destructor 0x4034b0, ~RFTexture 0x403420
	virtual unsigned int GetWidth() const;      // [1]  0x403430
	virtual unsigned int GetHeight() const;     // [2]  0x403440

	RFTexture();                                // 0x403450
	RFTexture(RFTexture const&);                // 0x403460
	RFTexture& operator=(RFTexture const&);     // 0x403470
};
CHECK_SIZE(RFTexture, 4);

#endif
