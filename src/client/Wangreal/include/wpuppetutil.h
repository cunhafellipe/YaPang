// Original: d:\build\custom\temp\code\client\wangreal\include\wpuppetutil.h
#ifndef PG_CLIENT_WANGREAL_INCLUDE_WPUPPETUTIL_H
#define PG_CLIENT_WANGREAL_INCLUDE_WPUPPETUTIL_H

#include "defines.h"
#include "wmath.h"

// A background model description. Only its constructor is known (emitted
// by Polysoup, which never calls it): it clears the first byte and the four
// vectors; the member roles are not known.
struct sBgModel {
	// wpuppetutil.h:38
	sBgModel() { m_v0.x = m_v0.y = m_v0.z = 0; m_v1.x = m_v1.y = m_v1.z = 0; m_v2.x = m_v2.y = m_v2.z = 0; m_v3.x = m_v3.y = m_v3.z = 0; m_name[0] = 0; }

	char m_name[0x28];            // 0x00, cleared by the constructor (a name, assumed)
	WVector m_v0;                 // 0x28, zeroed by the constructor
	WVector m_v1;                 // 0x34, zeroed by the constructor
	WVector m_v2;                 // 0x40, zeroed by the constructor
	WVector m_v3;                 // 0x4C, zeroed by the constructor
};

CHECK_SIZE(sBgModel, 0x58);
CHECK_FIELD(sBgModel, m_name, 0x00);
CHECK_FIELD(sBgModel, m_v0, 0x28);
CHECK_FIELD(sBgModel, m_v1, 0x34);
CHECK_FIELD(sBgModel, m_v2, 0x40);
CHECK_FIELD(sBgModel, m_v3, 0x4C);

#endif
