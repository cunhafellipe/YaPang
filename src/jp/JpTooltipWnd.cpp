// The JP 983 game's tooltip windows (JpTooltipWnd.h), from the JP exe's
// bodies (node scripts/disasm.ts --jp 0x<va>).
#include "JpTooltipWnd.h"

#include "fresh.h"
#include "frgraphicinterface.h"
#include "frwndmanager.h"

#include "RFPicture.h"
#include "RFTextArea.h"
#include "RefreshStringUtil.h"
#include "wmath.h"

// 0xe3375c
bool g_jpTooltipFollowEnabled = true;

// 0xe73dd8
int g_jpWndCreateMode = 0;

// 0xe78180
RFWnd* RFCommonTooltip::s_loadParent = NULL;
// 0xe78188
RFWnd* RFCommonTooltipByImage::s_loadParent = NULL;

// 0x5ce010
void RFTooltip::SetShown(bool shown) {
	m_shown = shown;
}

// 0x5ce020
float RFTooltip::GetLineHeight(int32_t fontSize) {
	if (m_freshLayoutData == NULL) {
		return 0.0f;
	}
	if (fontSize > 11) {
		return m_freshLayoutData->GetFontHeight();
	}
	return m_freshLayoutData->GetFontHeight11();
}

// 0x5ce060
void RFTooltip::SetText(std::wstring const& text) {
	if (m_textArea != NULL) {
		m_textArea->SetText(text);
		m_textArea->FitToContent(false);
	}
	UpdateLayout();
}

// 0x5ce0c0
RFTooltip::RFTooltip()
	: m_shown(false), m_freshLayoutData(NULL), m_position(RFVector2::ZERO),
	  m_ownerRect(RFRect::ZERO), m_owner(NULL), m_option(0), m_frame(NULL),
	  m_textArea(NULL), m_picture(NULL) {
	m_followMouse = true;
}

// 0x5ce150 (the body of the scalar deleting destructor 0x5ce410)
RFTooltip::~RFTooltip() {
}

// 0x5ce160
void RFTooltip::Attach(RFWnd* owner, int option, int style) {
	m_owner = NULL;
	if (owner == NULL) {
		m_followMouse = true;
		m_option = option;
		return;
	}
	if (!m_followMouse) {
		RFVector2 pos = owner->GetScreenPos();
		RFSize size = owner->GetSize();
		m_position.x = pos.x + size.width - size.width * 0.3f;
		m_position.y = pos.y + size.height + 2.0;
	} else {
		RFSize rootSize = RFSceneManager::Instance()->GetManager()->GetRootWnd()->GetSize();
		m_position.x = GetRect().Width() + rootSize.width;
		m_position.y = GetRect().Height() + rootSize.height;
	}
	SetScreenPos(m_position.x, m_position.y);
	m_ownerRect = owner->GetScreenRect();
	m_owner = owner;
	m_option = option;
}

// 0x5ce380 (scalar deleting destructor; the body is ~RFTooltip's)
RFCommonTooltip::~RFCommonTooltip() {
}

// 0x5ce3e0 (scalar deleting destructor; the body is ~RFTooltip's)
RFCommonTooltipByImage::~RFCommonTooltipByImage() {
}

// 0x5ce8c0
void RFTooltip::OnTooltipUpdate(EventArgs& args) {
	if (m_shown) {
		if (!g_jpTooltipFollowEnabled) {
			return;
		}
		WVector const& mouse = RFSceneManager::Instance()->GetMousePosition();
		RFWnd* hit = RFSceneManager::Instance()->GetManager()->HitTest2(
			RFVector2(mouse.x, mouse.y), true, true);
		if (hit != NULL && m_owner != NULL && hit != m_owner) {
			SetShown(false);
		}
		if (m_followMouse) {
			RFSize rootSize = RFSceneManager::Instance()->GetManager()->GetRootWnd()->GetSize();
			RFSize size = GetSize();
			m_position.x = mouse.x;
			m_position.y = mouse.y;
			m_position.x += m_option == 1 ? 15.0f : 20.0f;
			m_position.y += m_option == 1 ? 15.0f : 18.0f;
			if (m_position.x + size.width > rootSize.width) {
				m_position.x = rootSize.width - size.width;
				m_position.y = m_position.y + 14.0;
			}
			if (m_position.y + size.height > rootSize.height) {
				m_position.y = mouse.y - size.height - 10.0;
			}
			SetScreenPos(m_position.x, m_position.y);
		}
	}
	bool shown = m_shown;
	if (shown != IsWindowVisible(false)) {
		ShowActivated(shown);
	}
}

// 0x5ceb30
RFCommonTooltip* RFCommonTooltip::Load(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFCommonTooltip>(&RFManager::GetInstance(), L"tooltip_common_wnd.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFCommonTooltip>(&RFManager::GetInstance(), L"tooltip_common_wnd.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFCommonTooltip>(&RFManager::GetInstance(), L"tooltip_common_wnd.xml");
		break;
	}
	s_loadParent = parent;
	return static_cast<RFCommonTooltip*>(wnd);
}

// 0x5cec70
RFCommonTooltipByImage* RFCommonTooltipByImage::Load(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFCommonTooltipByImage>(&RFManager::GetInstance(), L"tooltip_common_wnd2.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFCommonTooltipByImage>(&RFManager::GetInstance(), L"tooltip_common_wnd2.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFCommonTooltipByImage>(&RFManager::GetInstance(), L"tooltip_common_wnd2.xml");
		break;
	}
	s_loadParent = parent;
	return static_cast<RFCommonTooltipByImage*>(wnd);
}

// 0x5cedb0
void RFTooltip::OnInitialUpdate() {
	RegisterTo(L"", RFWnd::EventUpdate, Subscriber(&RFTooltip::OnTooltipUpdate, this));
	if (g_pFresh != NULL && g_pFresh->GetManager() != NULL) {
		m_freshLayoutData = g_pFresh->GetManager()->GetGDI();
	}
}

// 0x5ceec0
float RFTooltip::GetLineWidth(std::wstring const& line, int32_t fontSize) {
	if (m_freshLayoutData == NULL) {
		return 0.0f;
	}
	if (fontSize > 11) {
		return m_freshLayoutData->GetTextExtend(WStringToShiftJis(line.c_str()).c_str());
	}
	return m_freshLayoutData->GetTextExtend11(WStringToShiftJis(line.c_str()).c_str());
}

// 0x5cf080
void RFTooltip::SetImageAndText(std::wstring const& image, std::wstring const& text) {
	if (m_picture != NULL) {
		m_picture->SetPicture(image.c_str());
		m_picture->FitToContent();
	}
	if (m_textArea != NULL) {
		m_textArea->SetText(text);
		m_textArea->FitToContent(false);
	}
	UpdateLayout();
}

// 0x5cf1f0
void RFCommonTooltip::ShowChild(wchar_t const* name, bool show) {
	RFWnd* child = GetChild(name);
	if (child != NULL) {
		child->ShowWindow(show);
	}
}

// 0x5cf2c0
void RFCommonTooltipByImage::ShowChild(wchar_t const* name, bool show) {
	RFWnd* child = GetChild(name);
	if (child != NULL) {
		child->ShowWindow(show);
	}
}

// 0x5cf390
RFSize RFTooltip::MeasureText(std::wstring const& text, int32_t fontSize) {
	float lineHeight = GetLineHeight(fontSize);
	float width = 0.0f;
	float height = 0.0f;
	size_t pos = 0;
	size_t next = text.find(L'\n');
	for (;;) {
		std::wstring line;
		if (pos != std::wstring::npos) {
			if (next != std::wstring::npos) {
				line.assign(text.begin() + pos, text.begin() + next);
			} else {
				line.assign(text.begin() + pos, text.end());
			}
			float lineWidth = GetLineWidth(line.c_str(), fontSize);
			if (width < lineWidth) {
				width = lineWidth;
			}
		}
		height += lineHeight;
		if (next == std::wstring::npos) {
			break;
		}
		height += m_textArea->GetTextHeight() - lineHeight;
		pos = text.find(L'\n', next) + 1;
		next = text.find(L'\n', pos);
	}
	return RFSize(width, height);
}

// 0x5cf700
void RFCommonTooltip::Attach(RFWnd* owner, int option, int style) {
	ShowChild(L"wnd_tooltip_simple", false);
	ShowChild(L"wnd_frame_dark", false);
	ShowChild(L"textarea_text_dark", false);
	ShowChild(L"textarea_text_dark11", false);
	ShowChild(L"textarea_text_simple", false);
	ShowChild(L"textarea_text_simple11", false);
	switch (option) {
	case 0:
		m_frame = GetChild(L"wnd_tooltip_simple");
		m_textArea = dynamic_cast<RFTextArea*>(GetChild(
			style > 11 ? L"textarea_text_simple" : L"textarea_text_simple11"));
		ShowChild(L"wnd_tooltip_simple", true);
		break;
	case 1:
		m_frame = GetChild(L"wnd_frame_dark");
		m_textArea = dynamic_cast<RFTextArea*>(GetChild(
			style > 11 ? L"textarea_text_dark" : L"textarea_text_dark11"));
		ShowChild(L"wnd_frame_dark", true);
		break;
	}
	if (m_textArea != NULL) {
		m_textArea->ShowWindow(true);
	}
	RFTooltip::Attach(owner, option, style);
}

// 0x5cf9d0
void RFCommonTooltip::UpdateLayout() {
	if (m_freshLayoutData == NULL || m_frame == NULL || m_textArea == NULL) {
		return;
	}
	RFVector2 pos = m_textArea->GetPos();
	RFSize textSize = MeasureText(m_textArea->GetText(), m_textArea->GetFontInfo().size);
	m_textArea->GetSize();
	m_textArea->SetSize(textSize);
	RFSize size(0.0f, 0.0f);
	RFSize fitted = m_textArea->GetSize();
	size.width = pos.x * 2.0 + fitted.width;
	size.height = pos.y * 2.0 + fitted.height - 1.0;
	SetSize(size);
	m_frame->SetSize(size);
}

// 0x5cfab0
void RFCommonTooltipByImage::Attach(RFWnd* owner, int option, int style) {
	ShowChild(L"wnd_frame_dark", true);
	ShowChild(L"textarea_text_dark", false);
	ShowChild(L"textarea_text_dark11", false);
	m_frame = GetChild(L"wnd_frame_dark");
	m_picture = dynamic_cast<RFPicture*>(GetChild(L"pic_dark"));
	m_textArea = dynamic_cast<RFTextArea*>(GetChild(
		style > 11 ? L"textarea_text_dark" : L"textarea_text_dark11"));
	if (m_picture != NULL) {
		m_picture->ShowWindow(true);
	}
	if (m_textArea != NULL) {
		m_textArea->ShowWindow(true);
	}
	RFTooltip::Attach(owner, option, style);
}

// 0x5cfcb0
void RFCommonTooltipByImage::UpdateLayout() {
	if (m_freshLayoutData == NULL || m_frame == NULL || m_textArea == NULL) {
		return;
	}
	RFVector2 picturePos = m_picture->GetPos();
	RFSize pictureSize = m_picture->GetSize();
	RFVector2 textPos = m_textArea->GetPos();
	textPos.x = picturePos.x + pictureSize.width + 10.0;
	m_textArea->SetPos(textPos.x, textPos.y);
	RFSize textSize = MeasureText(m_textArea->GetText(), m_textArea->GetFontInfo().size);
	m_textArea->SetSize(textSize);
	RFSize size(0.0f, 0.0f);
	float height = m_textArea->GetSize().height;
	if (pictureSize.height >= height) {
		height = pictureSize.height;
	}
	RFSize fitted = m_textArea->GetSize();
	size.width = fitted.width + textPos.x + picturePos.x;
	size.height = picturePos.y * 2 + height - 1.0;
	SetSize(size);
	m_frame->SetSize(size);
}
