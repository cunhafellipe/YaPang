// Original: client/Refresh/src/tinyxml/tinyxml.cpp
//
// Stock TinyXML 2.5.3 (externs/tinyxml-2.5.3/tinyxml.cpp, unmodified), compiled
// inside namespace rf as the Refresh library has it. Third-party code, not
// decompiled (externs/README.md); tinyxml.h explains the namespace wrapping.
// TinyXML's own #include "tinyxml.h" resolves next to it in externs/ and is
// then empty (its guard is already defined by the include below).
#include "tinyxml.h"

namespace rf {
#include "../../../../../../externs/tinyxml-2.5.3/tinyxml.cpp"
}

// The binary's TinyXML functions, all four sources together: the library was
// built with /GL + /LTCG, which interleaved them (0x430600-0x4307C0,
// 0x45DA60-0x464B00). By owner class in port/refresh/inventory.tsv:
// TiXmlAttribute (9)
// lib: 0x45dad0, 0x45db00, 0x45db10, 0x45db80, 0x45e020, 0x45e040, 0x460850, 0x462590
// lib: 0x464070
// TiXmlBase (1)
// lib: 0x45ddd0
// TiXmlComment (8)
// lib: 0x45e1b0, 0x45e1c0, 0x45e1d0, 0x460c20, 0x460c90, 0x460cd0, 0x460cf0, 0x463ed0
// TiXmlDeclaration (10)
// lib: 0x45e470, 0x45e530, 0x45e550, 0x45e560, 0x45e570, 0x4611a0, 0x4612d0, 0x461340
// lib: 0x461360, 0x464450
// TiXmlDocument (27)
// lib: 0x430770, 0x430780, 0x430790, 0x45deb0, 0x45e0f0, 0x45e260, 0x45e430, 0x45e5a0
// lib: 0x45ec80, 0x460640, 0x4606d0, 0x460760, 0x4607c0, 0x460800, 0x4627e0, 0x462800
// lib: 0x462820, 0x462830, 0x462880, 0x462b10, 0x462be0, 0x462c90, 0x462eb0, 0x462fb0
// lib: 0x463160, 0x4633a0, 0x463430
// TiXmlElement (20)
// lib: 0x45da60, 0x45df30, 0x45dff0, 0x45e0d0, 0x45e0e0, 0x45f4a0, 0x45f560, 0x45f590
// lib: 0x45f640, 0x45f660, 0x45f700, 0x45f970, 0x45faf0, 0x45fc80, 0x45fd10, 0x45fd70
// lib: 0x461530, 0x463650, 0x463aa0, 0x464860
// TiXmlNode (13)
// lib: 0x430610, 0x430620, 0x430630, 0x430640, 0x430650, 0x430660, 0x430670, 0x430680
// lib: 0x430690, 0x4306a0, 0x45dee0, 0x45def0, 0x45eb00
// TiXmlText (11)
// lib: 0x45e380, 0x45e390, 0x45e3a0, 0x45e840, 0x460d80, 0x460e80, 0x460ec0, 0x460ee0
// lib: 0x461fd0, 0x464220, 0x464b00
// TiXmlUnknown (8)
// lib: 0x45e660, 0x45e670, 0x45e680, 0x4613c0, 0x461410, 0x461450, 0x461470, 0x463d00
// Not named by the inventory, TinyXML by their code or strings: the
// FirstChild/LastChild/NextSibling/PreviousSibling (const char*) searches and
// the FirstChildElement (const char*) pair, TiXmlAttributeSet::Find, their jump
// thunks, TiXmlString::operator[] ("index < length()"), and the functions
// asserting in src\tinyxml\tinyxml.cpp (InsertBeforeChild, InsertAfterChild,
// IterateChildren, LoadFile, TiXmlAttributeSet::Add/Remove ...).
// lib: 0x430600, 0x45de50, 0x45de90, 0x45e0c0, 0x45f150, 0x45f1a0, 0x45f260, 0x45f2b0
// lib: 0x45f360, 0x45f400, 0x461630, 0x45dc60, 0x45eee0, 0x45efb0, 0x45f0f0, 0x45f1f0
// lib: 0x45f220, 0x4601b0, 0x4615a0, 0x4615e0
