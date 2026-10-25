// The JP game's members are VS 2010 std::tr1::shared_ptr (pointer, control
// block: 0x8 on x86). libc++ has no std::tr1; the port uses std::shared_ptr.
#ifndef PG_JP_REFRESHSHAREDPTR_H
#define PG_JP_REFRESHSHAREDPTR_H

#include <memory>

#if defined(_MSC_VER) && _MSC_VER < 1700
#define RF_SHARED_PTR std::tr1::shared_ptr
#else
#define RF_SHARED_PTR std::shared_ptr
#endif

#endif
