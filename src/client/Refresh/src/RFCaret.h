// Original: client\Refresh\src\RFCaret.h (JP 983 exe; RFCaret is not
// exported, the file name is assumed from the class)
#ifndef PG_CLIENT_REFRESH_SRC_RFCARET_H
#define PG_CLIENT_REFRESH_SRC_RFCARET_H

#include "RFSize.h"
#include "RFVector2.h"
#include "defines.h"

class RFBrush;
class ImageSet;

// The blinking text caret of RFEdit (m_caret, 0x1bc) and RFTextArea (0x1b0):
// vtable 0xd65d7c, size 0x1c, built by `new` in the owners' LoadResource. Its
// code lies together at 0x455f80-0x45612a. The blink toggles m_blinkOn every
// m_blinkTime seconds; the caret is drawn only while both flags are set.
class RFCaret {
	LAYOUT_CHECK_FRIEND;

public:
	// 0x455f80: both flags true, size and elapsed time 0, no brush, blink
	// time GetCaretBlinkTime() / 1000.0
	RFCaret();
	virtual ~RFCaret();                     // [0] scalar deleting destructor 0x455fd0; plain 0x456010 (deletes m_brush)

	// 0x456040: m_brush = CreateBrush(images) (0x42a8b0), m_size
	// = its size (RFBrush slot 2)
	void SetBrush(ImageSet* images);
	// 0x456070: while m_visible, adds the elapsed seconds; at m_blinkTime the
	// timer restarts and m_blinkOn flips
	void Update(float elapsed);
	// 0x4560b0: while m_visible and m_blinkOn, m_brush slot 1 (pos, m_size,
	// alpha, the owner's DrawCaret float)
	void Draw(RFVector2 const& pos, float alpha, float arg);
	// 0x4560f0: m_blinkTime = milliseconds / 1000.0
	void SetBlinkTime(unsigned int milliseconds);
	// 0x456120: m_blinkOn set, timer restarted (RFEdit and RFTextArea call it
	// on focus, enable and caret moves)
	void ResetBlink();

private:
	bool m_visible;                         // 0x04 (gates Update and Draw; only the constructor writes it)
	bool m_blinkOn;                         // 0x05 (the blink phase: drawn while set)
	RFSize m_size;                          // 0x08 (the brush's size)
	float m_elapsed;                        // 0x10 (seconds since the last toggle)
	float m_blinkTime;                      // 0x14 (seconds per phase)
	RFBrush* m_brush;                       // 0x18 (owned)
};
CHECK_SIZE(RFCaret, 0x1c);
BEGIN_LAYOUT_CHECK(RFCaret)
	CHECK_FIELD(RFCaret, m_visible, 0x04);
	CHECK_FIELD(RFCaret, m_blinkOn, 0x05);
	CHECK_FIELD(RFCaret, m_size, 0x08);
	CHECK_FIELD(RFCaret, m_elapsed, 0x10);
	CHECK_FIELD(RFCaret, m_blinkTime, 0x14);
	CHECK_FIELD(RFCaret, m_brush, 0x18);
END_LAYOUT_CHECK

#endif
