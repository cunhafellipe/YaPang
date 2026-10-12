// Original: d:\build\custom\temp\code\client\projectg\stdafx.cpp
// The precompiled-header object. Its code is what the original stdafx.h
// emits on its own: the header statics' dynamic initializers (golfdoc.h:282
// and 283, hatmanager.h:38-43) and stGameClass's constructor
// (globalgamedefine.h:663). The PCH's effect on the other units is
// reproduced by toolchain-shim/pch/projectg.h (docs/COMPILER.md).
#define PG_HATMANAGER_FILE_STRINGS
#include <atlbase.h>	// stand-in for the precompiled header: _AtlWinModule initializer
			// (it keeps BAR_LENGTH and the reasons dynamically initialized,
			// docs/tough_problems/010)

#include "GolfDoc.h"
#include "hatManager.h"
