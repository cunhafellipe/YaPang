// The JP 983 game's RFTooltipHelper (RFTooltipHelper.h), from the JP exe's
// bodies (node scripts/disasm.ts --jp 0x<va>).
#include "RFTooltipHelper.h"

#include "RefreshStringUtil.h"

// 0xe74da8
template <> RFTooltipHelper* WSingleton<RFTooltipHelper>::m_pInstance = NULL;

// 0x5dfa30
void RFTooltipHelper::SetTooltipVisible(bool visible) {
	if (m_tooltip != NULL) {
		m_tooltip->SetShown(visible);
	}
}

// 0x5dfad0
void RFTooltipHelper::HideTooltip() {
	SetTooltipVisible(false);
}

// 0x5dfb90
void RFTooltipHelper::Reset() {
	m_tooltip = NULL;
	m_state.owner = NULL;
	m_state.kind = 0;
	m_state.option = 0;
	m_state.text.clear();
	m_state.image.clear();
	m_state.style = 0;
	for (int i = 0; i < 3; i++) {
		if (m_tooltips[i] != NULL) {
			m_tooltips[i]->SetShown(false);
		}
	}
}

// 0x5dfbf0
RFTooltipHelper::RFTooltipHelper() {
	m_tooltip = NULL;
	m_tooltips[0] = NULL;
	m_tooltips[1] = NULL;
	m_tooltips[2] = NULL;
	m_tooltips[1] = RFCommonTooltip::Load(NULL);
	m_tooltips[2] = RFCommonTooltipByImage::Load(NULL);
}

// 0x5dfc80
RFTooltipHelper::~RFTooltipHelper() {
	Reset();
}

// 0x5dfd30
bool RFTooltipState::operator!=(const RFTooltipState& other) const {
	if (style != other.style || option != other.option || kind != other.kind) {
		return true;
	}
	if (kind == 1) {
		if (image != other.image || text != other.text) {
			return true;
		}
	}
	return false;
}

// 0x5dfe20
void RFTooltipHelper::Setup(RFWnd* owner, int kind, int option, const std::string& text,
	const std::string& image, int style) {
	RFTooltipState state;
	state.owner = owner;
	state.kind = kind;
	state.option = option;
	state.text = text;
	state.image = image;
	state.style = style;
	if (!(m_state != state)) {
		return;
	}
	SetTooltipVisible(false);
	m_tooltip = m_tooltips[kind];
	m_state = state;
	switch (kind) {
	case 1:
		if (m_tooltip != NULL) {
			m_tooltip->Attach(owner, option, style);
			m_tooltip->SetText(ShiftJisToWString(text.c_str()));
		}
		break;
	case 2:
		if (m_tooltip != NULL) {
			m_tooltip->Attach(owner, option, style);
			std::wstring wideText = ShiftJisToWString(text.c_str());
			std::wstring wideImage = ShiftJisToWString(image.c_str());
			m_tooltip->SetImageAndText(wideImage, wideText);
		}
		break;
	}
}

// 0x5e01c0
void RFTooltipHelper::ShowTooltip(RFWnd* owner, const std::string& text, int option, int style) {
	Setup(owner, 1, option, text.c_str(), "", style);
	SetTooltipVisible(true);
}
