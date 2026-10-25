#ifndef PG_DEFINES_H
#define PG_DEFINES_H

#include <stddef.h>

#include "padding.h"

// The original target is PE32 (i386): every size and offset checked below is
// its ABI. Any 64-bit build — Windows x64/arm64 (`_WIN64`) or Apple/Linux
// (`__LP64__`) — is a separate, non-matching configuration with 8-byte
// pointers, so an expected offset only holds there once the type's real
// members are transcribed (docs/PORTABILITY.md). PG_ABI32 selects the check.
#if defined(_WIN64) || defined(__LP64__)
#define PG_ABI32 0
#else
#define PG_ABI32 1
#endif

// A word the game spells `int` but that is pointer-sized in the message and
// video interfaces (window message parameters, WVideoDev::Command). The
// original 32-bit build has int == pointer; a 64-bit build carries 8-byte
// pointers through them. Default: `int`, byte-identical to the original. A
// platform defines PG_PTRINT (e.g. intptr_t) before this header is reached.
#ifndef PG_PTRINT
#define PG_PTRINT int
#endif

// A Fresh message-map handler's argument: the map invokes handlers as
// `void (FrCmdTarget::*)(intptr_t)` (frcmdtarget.h), so the handler's single
// parameter is pointer-sized. The original 32-bit build has int == pointer and
// spells it `int`; a 64-bit port defines PG_MSGARG=intptr_t. Default: `int`,
// byte-identical to the original. Only the argument of a handler registered in
// a message map uses this -- never `int` elsewhere.
#ifndef PG_MSGARG
#define PG_MSGARG int
#endif

// A forward-declared enumeration: `enum E;`. cl 13.10 accepts it (int-sized);
// clang requires a fixed underlying type, here and at the definition. Default:
// the original spelling, byte-identical to the original. A platform defines
// PG_PORT to get the fixed underlying type clang needs; the definition then
// uses PG_ENUM_DEF the same way.
#ifdef PG_PORT
#define PG_ENUM_FWD(name) enum name : int;
#define PG_ENUM_DEF(name) enum name : int {
#else
#define PG_ENUM_FWD(name) enum name;
#define PG_ENUM_DEF(name) enum name {
#endif

// Declarations the port spells differently from the original. Access and
// constness are part of the decorated names, so the original build keeps the
// original spelling. PG_PROTECTED: a `protected:` label whose members the
// port reaches from outside the class. PG_PORT_CONST: `const` on a reference
// parameter the original declares non-const (clang and GCC bind temporaries
// only to const references). PG_ORIG_CONST: `const` the original has and
// the port drops. The JP game code (PG_JP_GAME, written for the port) takes them too.
#if defined(PG_PORT) || defined(PG_JP_GAME)
#define PG_PROTECTED public
#define PG_PORT_CONST const
#define PG_ORIG_CONST
#else
#define PG_PROTECTED protected
#define PG_PORT_CONST
#define PG_ORIG_CONST const
#endif

// Frees a vector's storage and leaves it empty and usable. The original does
// it by calling the destructor (`v.~vector();`) and going on using the vector:
// cl 13.10's destructor (_Tidy) frees the buffer and nulls the pointers. Other
// libraries (libc++, libstdc++) free the buffer and leave the pointers
// dangling, so the next push_back writes into freed memory; every compiler but
// the original's swaps with an empty vector instead. Never write `.~vector()`
// in src/: scripts/check_container_dtors.ts rejects it.
#if defined(_MSC_VER) && _MSC_VER == 1310
#define PG_VECTOR_FREE_STORAGE(v) (v).~vector()
#else
template <class Vector>
inline void PgVectorFreeStorage(Vector& v) {
	Vector().swap(v);
}
#define PG_VECTOR_FREE_STORAGE(v) PgVectorFreeStorage(v)
#endif

// cl 13.10 has no static_assert: the checks use the negative-array-size idiom
// ("error C2118: negative subscript"). Off for the JP game code (src/jp under VS 2010,
// PG_JP_GAME): VS 2010's STL resizes the KR classes (std::map 12 -> 8 bytes).
#if PG_ABI32 && !defined(PG_JP_GAME)
#define CHECK_SIZE(type, expected_size) \
    typedef char CONCAT(check_size_, __LINE__)[(sizeof(type) == (expected_size)) ? 1 : -1]
#define CHECK_FIELD(type, field, expected_offset) \
    typedef char CONCAT(check_field_, __LINE__)[(offsetof(type, field) == (expected_offset)) ? 1 : -1]
#else
// 64-bit: the expected offsets are the 32-bit ABI's, so the check is off. The
// macro still expands to a declaration, so the same headers compile.
#define CHECK_SIZE(type, expected_size) \
    typedef char CONCAT(check_size64_, __LINE__)[1]
#define CHECK_FIELD(type, field, expected_offset) \
    typedef char CONCAT(check_field64_, __LINE__)[1]
#endif

// CHECK_FIELD outside the class cannot name a protected or private member,
// and the access must stay as the original's (cl mangles it into member
// function names: `IAE` = protected). Such a class declares
// LAYOUT_CHECK_FRIEND, and its field checks go in a specialisation that the
// friend declaration lets in (no code is generated for either):
//
//     class CFoo { LAYOUT_CHECK_FRIEND; protected: int m_x; // 0x4 };
//     CHECK_SIZE(CFoo, 0x8);
//     BEGIN_LAYOUT_CHECK(CFoo)
//         CHECK_FIELD(CFoo, m_x, 0x4);
//     END_LAYOUT_CHECK
template <class T> struct PgLayoutCheck;
#ifdef PG_PORT
// clang requires the explicit specialisation at the template's (global) scope;
// the game writes it inside namespaces. The port expands the check to an inert
// named struct instead. Default: the original cl 13.10 macros.
#define LAYOUT_CHECK_FRIEND
#define BEGIN_LAYOUT_CHECK(type) struct CONCAT(pg_layout_check_, __COUNTER__) {
#else
#define LAYOUT_CHECK_FRIEND template <class> friend struct PgLayoutCheck
#define BEGIN_LAYOUT_CHECK(type) template <> struct PgLayoutCheck<type> {
#endif
#define END_LAYOUT_CHECK };

#endif // PG_DEFINES_H
