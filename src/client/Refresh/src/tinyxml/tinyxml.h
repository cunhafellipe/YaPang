// Original: client/Refresh/src/tinyxml/tinyxml.h
//
// The Refresh library's own TinyXML: stock TinyXML 2.5.3, built without
// TIXML_USE_STL, in namespace rf (the binary's rf::TiXml*). It is third-party
// code and is not decompiled: the declarations are externs/tinyxml-2.5.3/
// (vendored unmodified, version evidence in externs/README.md), included here
// inside namespace rf.
//
// The C headers TinyXML includes come first, at global scope: their include
// guards then make TinyXML's own #includes of them no-ops, so the CRT stays in
// the global namespace and only TinyXML's declarations land in rf.
// (<assert.h> has no guard by design; included again inside rf it only
// redefines the assert macro and redeclares extern "C" functions, which name
// the same global functions.)
#ifndef PG_CLIENT_REFRESH_SRC_TINYXML_TINYXML_H
#define PG_CLIENT_REFRESH_SRC_TINYXML_TINYXML_H

#ifdef TIXML_USE_STL
#error "the Refresh library's TinyXML is built without TIXML_USE_STL"
#endif

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tinystr.h"

namespace rf {
#include "../../../../../../externs/tinyxml-2.5.3/tinyxml.h"
}

#endif // PG_CLIENT_REFRESH_SRC_TINYXML_TINYXML_H
