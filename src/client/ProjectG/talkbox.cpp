// Original: d:\build\custom\temp\code\client\projectg\talkbox.cpp
#include "talkbox.h"

#include <string.h>
#include <string>

#include "ChatMsg.h"
#include "wfont.h"
#include "wresrcmng.h"
#include "wview.h"

// talkbox.cpp:18
CTalkBox::CTalkBox(void) {
	m_pFont = NULL;
	m_pOverlay = NULL;
	m_transparency = 1.0f;
	m_maxLineLength = 30;
	m_pOverlay = g_resrcmng->GetOverlay("[chat_box.jpg", 0);

	m_pFont = CChatMsg::Instance()->GetMaskedFont();
}

// talkbox.cpp:32
CTalkBox::~CTalkBox(void) {
	if (g_resrcmng && m_pOverlay) { g_resrcmng->Release(m_pOverlay); m_pOverlay = 0; }
	m_lines.clear();
}

// talkbox.cpp:38
void CTalkBox::SetText(const char* text, float showTime) {
	std::string all;
	sLine line;
	int len = 0;
	int i;

	if (text == NULL || *text == '\0') {
		return;
	}

	m_lines.clear();

	m_lineCount = 0;
	strcpy(m_text, text);

	i = 0;
	line.color = 0;
	line.text[0] = '\0';

	while (m_text[i] != '\0') {
		bool doubleByte = false;
		if (m_text[i] == '/') {
			i++;

			if (m_text[i] == 'c') {
				i++;
				line.color = 0;
				for (int shift = 20; shift >= 0; shift -= 4, i++) {
					char c = m_text[i];
					if (c >= '0' && c <= '9') {
						line.color |= (c - '0') << shift;
					} else if (c >= 'A' && c <= 'F') {
						line.color |= (c - 'A' + 10) << shift;
					} else if (c >= 'a' && c <= 'f') {
						line.color |= (c - 'a' + 10) << shift;
					}
				}
				line.lineBreak = 0;
			} else if (m_text[i] == 'n') {
				i++;
				m_lineCount++;
				line.lineBreak = 1;
			}

			line.text[len] = '\0';
			line.width = m_pFont->GetTextWidth(g_view, line.text);
			m_lines.push_back(line);
			all += line.text;
			len = 0;
		} else {
			if (m_text[i] & 0x80) {
				line.text[len++] = m_text[i++];
				doubleByte = true;
			}
			line.text[len++] = m_text[i++];

			if (len < m_maxLineLength && m_text[i] != '\0') {
				continue;
			}

			if (m_text[i] == ' ') {
				line.text[len++] = ' ';
				i++;
			} else if (doubleByte && len >= m_maxLineLength) {
				i -= 2;
				line.text[len - 2] = '\0';
			}

			line.text[len] = '\0';
			line.width = m_pFont->GetTextWidth(g_view, line.text);
			line.lineBreak = 1;
			m_lines.push_back(line);
			m_lineCount++;

			all += line.text;
			len = 0;
		}
	}

	m_remainTime = showTime;
}

// talkbox.cpp:149
void CTalkBox::Process(float fDelta) {
	m_remainTime -= fDelta;
}

// talkbox.cpp:154
void CTalkBox::Render(WView* pView, float x, float y, int align) {
	unsigned long color;
	unsigned long alpha;
	float maxWidth;
	float width;
	std::list<sLine>::iterator it;
	WRect tail;
	WPoint boxPos;
	float shear;
	WRect src;
	float boxHeight;
	float topMargin;
	float lineSpacing;
	float lineHeight;

	if (m_lines.size() == 0 || m_transparency == 0.0f) {
		return;
	}

	alpha = (int)(m_transparency * 255.0f) << 24;
	color = 0;

	// The widest line sets the balloon's width.
	maxWidth = 0.0f;
	width = 0.0f;
	for (it = m_lines.begin(); it != m_lines.end(); ++it) {
		width += (*it).width;
		if ((*it).lineBreak) {
			if (maxWidth < width) {
				maxWidth = width;
			}
			width = 0.0f;
		}
	}

	tail = WRect(0.0f, 0.0f, 10.0f, 20.0f);
	switch (align) {
	case 0:
		boxPos.x = x;
		tail.x = x + 6.0f;
		shear = -(tail.w * 0.5f + 6.0f);
		break;
	case 1:
		boxPos.x = x - 6.0f - maxWidth * 0.5f;
		tail.x = x - tail.w * 0.5f;
		shear = 0.0f;
		break;
	case 2:
		boxPos.x = x - 12.0f - maxWidth;
		tail.x = x - 6.0f - tail.w;
		shear = tail.w * 0.5f + 6.0f;
		break;
	default:
		shear = 0.0f;
		break;
	}

	// The box image has one variant per line count.
	switch (m_lineCount) {
	case 1:
		src = WRect(0.0f, 0.0f, 0.5f, 0.234375f);
		// Falls through: one line uses the default size.
	default:
		boxHeight = 30.0f;
		topMargin = 10.0f;
		lineSpacing = 0.0f;
		break;
	case 2:
		boxHeight = 40.0f;
		topMargin = 7.0f;
		src = WRect(0.0f, 0.375f, 0.5f, 0.3125f);
		lineSpacing = 6.0f;
		break;
	case 3:
		boxHeight = 48.0f;
		topMargin = 4.0f;
		src = WRect(0.5f, 0.0f, 0.5f, 0.375f);
		lineSpacing = 5.0f;
		break;
	}

	tail.y = y - tail.h;
	boxPos.y = tail.y - boxHeight + 1.0f;
	lineHeight = lineSpacing + 10.0f;
	m_boxPos = boxPos;

	m_pOverlay->Render(pView, WRect(src.x, src.y, 0.046875f, src.h), WRect(boxPos.x, boxPos.y, 6.0f, boxHeight), 0x80000, alpha | 0xFFFFFF, 0.0f, 0);
	m_pOverlay->Render(pView, WRect(src.x + 0.046875f, src.y, src.w - 0.09375f, src.h), WRect(boxPos.x + 6.0f, boxPos.y, maxWidth, boxHeight), 0x80000, alpha | 0xFFFFFF, 0.0f, 0);
	m_pOverlay->Render(pView, WRect(src.w + src.x - 0.046875f, src.y, 0.046875f, src.h), WRect(boxPos.x + maxWidth + 6.0f, boxPos.y, 6.0f, boxHeight), 0x80000, alpha | 0xFFFFFF, 0.0f, 0);
	m_pOverlay->RenderWithShear(pView, WRect(0.5f, 0.375f, tail.w / 128.0f, tail.h / 128.0f), tail, shear, 0x80000, alpha | 0xFFFFFF);

	WPoint textPos(boxPos.x + 6.0f, boxPos.y + topMargin);
	for (it = m_lines.begin(); it != m_lines.end(); ++it) {
		m_pFont->Print(pView, textPos.x, textPos.y, (*it).text, 0, alpha | color, 0);

		color = (*it).color;
		if ((*it).lineBreak) {
			textPos.y += lineHeight;
			textPos.x = boxPos.x + 6.0f;
		} else {
			textPos.x += (*it).width;
		}
	}

	m_pFont->Flush(pView);
}
