// Original: d:\build\custom\temp\code\shared\thirties\xtdebug.h
#ifndef PG_SHARED_THIRTIES_XTDEBUG_H
#define PG_SHARED_THIRTIES_XTDEBUG_H

// A constant false the compiler does not warn about as a loop condition
// (C4127): the `do { ... } while (xtAlways<0>::Get())` of the xt macros.
template <int N>
struct xtAlways {
	// xtdebug.h:136
	static bool Get(void) { return false; }
};

// The release form of the xt assertion: the expression is not evaluated, only
// the do-while (whose condition still emits xtAlways<0>::Get) remains.
#define xtASSERT(expr) do { } while (xtAlways<0>::Get())

#endif // PG_SHARED_THIRTIES_XTDEBUG_H
