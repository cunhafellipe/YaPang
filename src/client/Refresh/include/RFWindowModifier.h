// Original: client\Refresh\include\RFWindowModifier.h (JP 983 exe; path from
// the library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFWINDOWMODIFIER_H
#define PG_CLIENT_REFRESH_INCLUDE_RFWINDOWMODIFIER_H

#include <string>

#include "RFExport.h"
#include "RFSize.h"
#include "RFVector2.h"
#include "defines.h"
#include "padding.h"

// Window modifier: name, alpha, visibility, position and size; vtable
// 0xd63080, size 0x3c (from its vector deleting destructor 0x4047b0). The
// copy constructor 0x4046a0 and operator= 0x404710 are the implicit ones
// (dllexport makes cl emit them).
class RF_API RFWindowModifier {
	LAYOUT_CHECK_FRIEND;

public:
	RFWindowModifier();                          // 0x404560
	virtual ~RFWindowModifier();                 // [0] vector deleting destructor 0x4047b0, ~RFWindowModifier 0x404590
	virtual std::wstring const& GetName() const; // [1] 0x4045d0
	virtual void SetName(std::wstring const&);   // [2] 0x4045e0
	virtual float GetAlpha() const;              // [3] 0x404600
	virtual void SetAlpha(float);                // [4] 0x404610
	virtual bool IsVisible() const;              // [5] 0x404620
	virtual void SetVisible(bool);               // [6] 0x404630
	virtual RFVector2 const& GetPos() const;     // [7] 0x404640
	virtual void SetPos(RFVector2 const&);       // [8] 0x404650
	virtual RFSize const& GetSize() const;       // [9] 0x404670
	virtual void SetSize(RFSize const&);         // [10] 0x404680

	std::wstring m_name;   // 0x4
	float m_alpha;         // 0x20
	bool m_visible;        // 0x24
	RFVector2 m_pos;       // 0x28
	RFSize m_size;         // 0x30
	int m_reserved;        // 0x38, a 4-byte scalar (copied as one dword by the copy
	                       // constructor 0x4046a0 and operator= 0x404710), left
	                       // uninitialized by the default constructor; no reader seen
};
CHECK_SIZE(RFWindowModifier, 0x3c);
BEGIN_LAYOUT_CHECK(RFWindowModifier)
	CHECK_FIELD(RFWindowModifier, m_name, 0x4);
	CHECK_FIELD(RFWindowModifier, m_alpha, 0x20);
	CHECK_FIELD(RFWindowModifier, m_visible, 0x24);
	CHECK_FIELD(RFWindowModifier, m_pos, 0x28);
	CHECK_FIELD(RFWindowModifier, m_size, 0x30);
	CHECK_FIELD(RFWindowModifier, m_reserved, 0x38);
END_LAYOUT_CHECK

#endif
