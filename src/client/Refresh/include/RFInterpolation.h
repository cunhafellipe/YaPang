// Original: client\Refresh\include\RFInterpolation.h (JP 983 exe; VS 2010 SP1)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFINTERPOLATION_H
#define PG_CLIENT_REFRESH_INCLUDE_RFINTERPOLATION_H

#include "RFExport.h"
#include "defines.h"

// The interpolation base of the animation keyframes (docs/JP_REFRESH_UI.md,
// "Animations: RFDirector"): vtable 0xd630d8, 2 slots, size 0x4 (the ctors
// write only the vptr, the copy ctor copies nothing and the vector deleting
// destructor 0x403860 passes the element size 4 to the array-delete helper:
// no members). The base GetValue returns its argument unchanged. The class is
// dllexport (docs/JP_CLIENT.md): that is what puts the vector deleting
// destructor ??_E, not the scalar ??_G, in slot 0.
// Every member is a header inline: their kept copies lie in the header-inline
// cluster at 0x4037e0, next to RFBezierSpline's implicit copy members and
// RFMatrix's / RFAnimFrame's inlines, and the class has no code elsewhere;
// RFBezierSpline's constructors, assignment and destructor inline them.
class RF_API RFInterpolation {
	LAYOUT_CHECK_FRIEND;

public:
	// [0] vector deleting destructor 0x403860
	// 0x4037e0
	virtual ~RFInterpolation() {}
	// [1]
	// 0x4037f0
	virtual float GetValue(float t) const { return t; }

	// 0x403800
	RFInterpolation() {}
	// 0x403810
	RFInterpolation(RFInterpolation const&) {}
	// 0x403820
	RFInterpolation& operator=(RFInterpolation const&) { return *this; }
};
CHECK_SIZE(RFInterpolation, 0x4);

#endif
