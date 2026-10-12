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

// cl 13.10 (Visual C++ .NET 2003) has no static_assert, so the layout checks
// use the negative-array-size idiom. A failing check reads
// "error C2118: negative subscript" on the line of the check.
#if PG_ABI32
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
#define LAYOUT_CHECK_FRIEND template <class> friend struct PgLayoutCheck
#define BEGIN_LAYOUT_CHECK(type) template <> struct PgLayoutCheck<type> {
#define END_LAYOUT_CHECK };

#endif // PG_DEFINES_H
