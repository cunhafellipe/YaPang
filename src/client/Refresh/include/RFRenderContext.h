// Original: client\Refresh\include\RFRenderContext.h (JP 983 exe; the class
// name comes from RFWnd::GetRC's decoration, the header path is assumed)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFRENDERCONTEXT_H
#define PG_CLIENT_REFRESH_INCLUDE_RFRENDERCONTEXT_H

#include "RFRect.h"
#include "defines.h"

// The drawing state of a window, embedded in RFWnd at 0x158 (GetRC 0x401fe0
// returns it): vtable 0xd63060, 1 slot (the scalar deleting destructor
// 0x402910), size 0x1c. Not exported: its constructor and destructor are
// inlined into RFWnd's.
class RFRenderContext {
	LAYOUT_CHECK_FRIEND;

public:
	// The RFWnd constructor 0x404840 inlines it.
	RFRenderContext() : m_clip(false), m_depth(-1.0f) {}
	// [0] scalar deleting destructor 0x402910
	virtual ~RFRenderContext() {}

	bool m_clip;      // 0x4 (RFWnd::Create copies WndBaseStruct::m_clipChildren)
	RFRect m_clipRect; // 0x8
	float m_depth;    // 0x18 (-1 by default; RFWnd::PrintText 0x405700 passes it to RFInterface::PrintText as its float)
};
CHECK_SIZE(RFRenderContext, 0x1c);
BEGIN_LAYOUT_CHECK(RFRenderContext)
	CHECK_FIELD(RFRenderContext, m_clip, 0x4);
	CHECK_FIELD(RFRenderContext, m_clipRect, 0x8);
	CHECK_FIELD(RFRenderContext, m_depth, 0x18);
END_LAYOUT_CHECK

#endif
