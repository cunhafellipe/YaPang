// The JP 983 game's RFTooltipHelper (RFTooltipHelper :
// WSingleton<RFTooltipHelper> : BaseObject, vtable 0xd122f4, size 0x80;
// constructor 0x5dfbf0; instance pointer 0xe74da8). Created last by
// RFSceneManager::Setup (0x604160) and deleted first by RFSceneManager's
// destructor (0x603d90), through its [0] scalar deleting destructor.
// Declarations and the JP x86 layout (the strings are VS 2010's 0x1c).
#ifndef PG_JP_RFTOOLTIPHELPER_H
#define PG_JP_RFTOOLTIPHELPER_H

#include <string>

#include "defines.h"
#include "padding.h"
#include "singleton.h"

#include "JpTooltipWnd.h"

// What a tooltip is shown for (names ours), size 0x48: Setup builds one from
// its arguments and refreshes the tooltip only when it differs from the
// shown one (operator!= 0x5dfd30).
struct RFTooltipState {
	// 0x5dfd30: style, option and kind first; for kind 1 also the image
	// (0x524f00) and the text (compare 0x448a30). The owner is not compared.
	bool operator!=(const RFTooltipState& other) const;

	RFWnd* owner;          // 0x00 the hovered window it is shown for
	int kind;              // 0x04 index into m_tooltips (1: text, 2: image and text)
	int option;            // 0x08 RFTooltip::Attach's option
	std::string text;      // 0x0c
	std::string image;     // 0x28 the picture of a kind 2 tooltip (RFTooltip::SetImageAndText)
	int style;             // 0x44 RFTooltip::Attach's style
};
CHECK_SIZE(RFTooltipState, 0x48);
BEGIN_LAYOUT_CHECK(RFTooltipState)
	CHECK_FIELD(RFTooltipState, owner, 0x00);
	CHECK_FIELD(RFTooltipState, kind, 0x04);
	CHECK_FIELD(RFTooltipState, option, 0x08);
	CHECK_FIELD(RFTooltipState, text, 0x0c);
	CHECK_FIELD(RFTooltipState, image, 0x28);
	CHECK_FIELD(RFTooltipState, style, 0x44);
END_LAYOUT_CHECK

class RFTooltipHelper : public WSingleton<RFTooltipHelper> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5dfbf0: BaseObject (0xb67ba0), the instance pointer, m_tooltip = 0,
	// the two empty strings, m_tooltips[] = 0, then m_tooltips[1] and
	// m_tooltips[2] loaded (RFCommonTooltip::Load 0x5ceb30,
	// RFCommonTooltipByImage::Load 0x5cec70, no parent).
	RFTooltipHelper();
	// 0x5dfc80 ([0] scalar deleting destructor 0x5dfda0): Reset (0x5dfb90),
	// then the two strings (+0x54 first), WSingleton and BaseObject.
	virtual ~RFTooltipHelper();

	// The members (names ours), from the JP game's lobby desktop
	// (RFLobbyDesktop's top-bar EventMouseEnter / EventMouseLeave and
	// EventFinalize handlers).
	// 0x5dfb90: the state cleared, then every loaded tooltip window hidden.
	void Reset();
	// 0x5e01c0: Setup(owner, 1, option, text.c_str(), "", style), then
	// SetTooltipVisible(true).
	void ShowTooltip(RFWnd* owner, const std::string& text, int option, int style);
	// 0x5dfad0: SetTooltipVisible(false).
	void HideTooltip();

protected:
	// 0x5dfa30: m_tooltip's SetShown(visible) when there is one.
	void SetTooltipVisible(bool visible);
	// 0x5dfe20: when the new state differs from m_state, hides the shown
	// tooltip, takes m_tooltips[kind] and the state, attaches the window
	// (RFTooltip::Attach) and gives it its text (kind 1) or image and text
	// (kind 2), widened from Shift-JIS (0x5e4ef0).
	void Setup(RFWnd* owner, int kind, int option, const std::string& text,
		const std::string& image, int style);

	// 0x0-0x28: WSingleton<RFTooltipHelper> / BaseObject.
	RFTooltip* m_tooltip;                                    // 0x28 the shown window, m_tooltips[m_state.kind]
	RFTooltipState m_state;                                  // 0x2c
	RFTooltip* m_tooltips[3];                                // 0x74 ([0] stays null; [1] RFCommonTooltip, [2] RFCommonTooltipByImage)
};
CHECK_SIZE(RFTooltipHelper, 0x80);
BEGIN_LAYOUT_CHECK(RFTooltipHelper)
	CHECK_FIELD(RFTooltipHelper, m_tooltip, 0x28);
	CHECK_FIELD(RFTooltipHelper, m_state, 0x2c);
	CHECK_FIELD(RFTooltipHelper, m_tooltips, 0x74);
END_LAYOUT_CHECK

#endif
