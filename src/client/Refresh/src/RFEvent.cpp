// Original: client\Refresh\src\RFEvent.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include "RFEventHandler.h"

// lib: 0x408630, 0x408730, 0x408880, 0x410a10, 0x40a090, 0x4086f0, 0x40a400, 0x40b870, 0x411840, 0x4118d0, 0x411910
// (std::vector<Subscriber> copy constructor, operator=, push_back, _Destroy,
// std::wstring::assign, vector<Subscriber>::_Tidy, _Buy,
// allocator<Subscriber>::allocate, and the uninitialized copies of
// Subscriber ranges with Subscriber's copy constructor inlined); RFEvent's
// implicit copy constructor 0x4019e0 and operator= 0x401a60

// Every member is inline in RFEventHandler.h: the constructor 0x4017e0, the
// destructor 0x401820, Cleanup 0x4018f0, Subscribe 0x401930,
// ClearSubscribers 0x401950, operator() 0x401990, and the implicit copy
// constructor 0x4019e0 and assignment 0x401a60.
