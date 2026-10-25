// Original: client\Refresh\src\RFTextComponent.h (JP 983 exe; RFTextComponent
// is not exported and no assert path names its file: the name is assumed
// from the class)
#ifndef PG_CLIENT_REFRESH_SRC_RFTEXTCOMPONENT_H
#define PG_CLIENT_REFRESH_SRC_RFTEXTCOMPONENT_H

#include <stdint.h>
#include <string>

#include "defines.h"

struct FontInfo;

// The abstract piece of laid-out text an RFText holds: vtable 0xd66abc,
// 24 slots, every one but the destructor _purecall. No data of its own (the
// derived TextComponent::TextBlock starts its members at 0x4). Not exported:
// its scalar deleting destructor pairs as sub_00468c40.
//
// The slot names come from TextComponent::TextBlock's overrides. The
// parameters of slots 3, 4, 19, 21 and 22 are read from the callee's stack
// cleanup and use only; their roles are noted where they are known.
class RFTextComponent {
public:
	RFTextComponent() {}                        // 0x468d70 (LTCG: this in eax)
	virtual ~RFTextComponent() {}               // [0] 0x468c30 (emitted by TextBlock.cpp; scalar deleting destructor 0x468c40)

	virtual int32_t GetType() const = 0;        // [1]
	// Whether a visible segment contains the point.
	virtual bool HitTest(int32_t x, int32_t y) = 0;     // [2]
	// The id of the visible segment whose box, hitWidth wide, contains the
	// point; *pastEnd tells whether the point lies past the segment's own
	// width.
	virtual bool GetSegmentAt(int32_t x, int32_t y, uint32_t hitWidth, bool* pastEnd,
		int32_t* segmentId) = 0;                    // [3]
	// The hyperlink under the point, when the component is a link.
	virtual bool GetHyperLinkAt(int32_t x, int32_t y, int32_t const* linkState,
		std::wstring* link) = 0;                    // [4]
	virtual void HideSegments() = 0;            // [5]
	// One stack argument the override does not read (ret 4).
	virtual void ShowSegments(int32_t) = 0;     // [6]
	virtual void ClearSelected() = 0;           // [7]
	virtual void SetSelected(bool selected) = 0;        // [8]
	// Removes up to count characters at pos; returns how many were not.
	virtual uint32_t Erase(uint32_t pos, uint32_t count) = 0;   // [9]
	virtual uint32_t EraseFront(uint32_t count) = 0;    // [10]
	virtual uint32_t GetLength() const = 0;     // [11]
	virtual uint32_t GetWidth() const = 0;      // [12]
	virtual bool IsEmpty() const = 0;           // [13]
	virtual bool HasSameStyle(RFTextComponent const* other) const = 0;    // [14]
	virtual uint32_t GetSegmentFirstChar(int32_t segmentId) const = 0;   // [15]
	virtual uint32_t GetSegmentLastChar(int32_t segmentId) const = 0;    // [16]
	virtual uint32_t GetSegmentWidth(int32_t segmentId) const = 0;       // [17]
	virtual uint32_t GetSegmentHeight(int32_t segmentId) const = 0;      // [18]
	// The character of segment segmentId at pixel offset *offset (measured
	// one character at a time); a past-the-segment offset is reduced by the
	// segment's width and the segment's length returned (0x4699e0).
	virtual uint32_t SplitSegment(int32_t segmentId, uint32_t* offset) = 0; // [19]
	virtual void GetSegmentIdRange(int32_t* firstId, int32_t* lastId) const = 0; // [20]
	// The segment holding character charPos (searched from the first or the
	// last segment), the character's offset in it and its pixel offset from
	// the segment's start.
	virtual void GetCharPos(uint32_t charPos, int32_t* segmentId, uint32_t* charOffset,
		uint32_t* x, bool fromFront) = 0;           // [21]
	// Splits the text into segments that fit: the first one in available
	// pixels, the next ones in lineWidth, numbering them from *segmentId on
	// (left at the last id); returns the room left after the last segment,
	// -1 for an empty text.
	virtual int32_t Layout(FontInfo const& font, FontInfo const& linkFont, uint32_t available,
		uint32_t lineWidth, int32_t* segmentId) = 0;    // [22]
	virtual void SetSegmentPosition(int32_t segmentId, int32_t x, int32_t y) = 0;  // [23]
};
CHECK_SIZE(RFTextComponent, 0x4);

#endif
