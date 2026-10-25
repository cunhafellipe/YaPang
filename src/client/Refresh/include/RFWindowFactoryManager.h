// Original: client\Refresh\include\RFWindowFactoryManager.h (JP 983 exe; the
// library's layout; its source is src\RFFactoryManager.cpp, the assert paths)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFWINDOWFACTORYMANAGER_H
#define PG_CLIENT_REFRESH_INCLUDE_RFWINDOWFACTORYMANAGER_H

#include <map>
#include <string>

#include "RFExport.h"
#include "RFSingleton.h"
#include "defines.h"

class RFWindowFactory;

// The registry from a layout's type name to the factory that creates it, and
// the type aliases (AddTypeAlias 0x4379d0). RFManager owns the one instance
// (new 0x20 in its ctor 0x417490, at its +0x44) and registers it as the
// RFSingleton<RFWindowFactoryManager> singleton; no vtable, size 0x20 (the
// new of its ctor). dllexport as in the original library: cl then emits the
// implicit copy constructor (0x417320) and operator= (0x417390) and the
// singleton base's members (GetInstance 0x41aab0, GetInstancePtr 0x41aae0).
class RF_API RFWindowFactoryManager: public RFSingleton<RFWindowFactoryManager> {
	LAYOUT_CHECK_FRIEND;

public:
	RFWindowFactoryManager();                              // 0x417150
	~RFWindowFactoryManager();                             // 0x417280

	void AddFactory(RFWindowFactory *);                    // 0x437650
	void RemoveFactory(RFWindowFactory *);                 // 0x4377b0
	void RemoveFactory(std::wstring const&);               // 0x437820
	RFWindowFactory *GetFactory(std::wstring const&) const; // 0x437890
	void AddTypeAlias(std::wstring const&, std::wstring const&); // 0x4379d0
	void RemoveTypeAlias(std::wstring const&);             // 0x437ba0

private:
	// The alias name to the real type name (the assert of AddTypeAlias
	// 0x4379d0 names _aliases; its nodes are 0x48 = the two strings).
	std::map<std::wstring, std::wstring> _aliases;         // 0x0
	// The type name to its factory, keyed by RFWindowFactory::m_typeName
	// (AddFactory 0x437650; its nodes are 0x30 = the string and the pointer).
	std::map<std::wstring, RFWindowFactory *> _factories;  // 0x10
};
CHECK_SIZE(RFWindowFactoryManager, 0x20);
BEGIN_LAYOUT_CHECK(RFWindowFactoryManager)
	CHECK_FIELD(RFWindowFactoryManager, _aliases, 0x0);
	CHECK_FIELD(RFWindowFactoryManager, _factories, 0x10);
END_LAYOUT_CHECK

#endif
