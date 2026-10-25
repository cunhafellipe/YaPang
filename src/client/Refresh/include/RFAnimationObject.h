// Original: client\Refresh\include\RFAnimationObject.h (JP 983 exe; path from
// the library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFANIMATIONOBJECT_H
#define PG_CLIENT_REFRESH_INCLUDE_RFANIMATIONOBJECT_H

#include <string>

#include "defines.h"
#include "padding.h"
#include "RFEventHandler.h"
#include "RFExport.h"
#include "RFRect.h"

class RFAnimation;
class RFAnimFrame;
class RFTexture;
struct RFFloatAnimationFrame;
namespace rf {
class TiXmlNode;
}

// One animation instance handled by the director: vtable 0xd630f8, 2 slots,
// size 0xA8 (from its scalar deleting destructor 0x4041a0). Load reads the
// <asset> (name, type IMAGE/TEXT, rect, texture path or text) and the <anim>
// (name, frames) of an XML node; Render draws the keyframe animation
// m_animFrame with the asset's texture.
class RF_API RFAnimationObject: public RFEventHandler {
	LAYOUT_CHECK_FRIEND;
	// RFDirector::SetPosition 0x430fa0 reads m_animFrame and m_rect.
	friend class RFDirector;
	// RFAnimation's Play, Stop, End, FrameMove and SetPosition read m_animFrame and m_rect.
	friend class RFAnimation;

public:
	virtual ~RFAnimationObject();       // [1] scalar deleting destructor 0x4041a0, ~RFAnimationObject 0x42afc0

	RFAnimationObject(RFAnimation*);    // 0x403e30
	std::wstring const& name() const;   // 0x403ef0
	std::wstring const& asset() const;  // 0x403f00
	void Load(rf::TiXmlNode const*);    // 0x42b0f0
	void Render(float);                 // 0x42c7d0

private:
	// Reads the "val" of the child element name into frame's value, and its
	// "easeValue" (four comma-separated floats, a Bezier curve's control
	// points) into frame's interpolation.
	void GetXmlValue(rf::TiXmlNode const*, std::string const&, RFFloatAnimationFrame*);  // 0x42c890

	std::wstring m_name;         // 0x30, the asset's "name"
	int m_type;                  // 0x4c, Load: 0 IMAGE, 1 and 2 (TEXT); Render draws 0 and 2
	std::wstring m_animName;     // 0x50, the anim's "name"
	std::wstring m_asset;        // 0x6c
	RFRect m_rect;               // 0x88, the asset's "rect" (ltrb)
	float m_duration;            // 0x98, Load: the largest frame time
	RFAnimFrame* m_animFrame;    // 0x9c, owned
	RFAnimation* m_animation;    // 0xa0, the owner given to the constructor
	RFTexture* m_texture;        // 0xa4, created and deleted through the manager's renderer
};
CHECK_SIZE(RFAnimationObject, 0xa8);
BEGIN_LAYOUT_CHECK(RFAnimationObject)
	CHECK_FIELD(RFAnimationObject, m_name, 0x30);
	CHECK_FIELD(RFAnimationObject, m_type, 0x4c);
	CHECK_FIELD(RFAnimationObject, m_animName, 0x50);
	CHECK_FIELD(RFAnimationObject, m_asset, 0x6c);
	CHECK_FIELD(RFAnimationObject, m_rect, 0x88);
	CHECK_FIELD(RFAnimationObject, m_duration, 0x98);
	CHECK_FIELD(RFAnimationObject, m_animFrame, 0x9c);
	CHECK_FIELD(RFAnimationObject, m_animation, 0xa0);
	CHECK_FIELD(RFAnimationObject, m_texture, 0xa4);
END_LAYOUT_CHECK

#endif
