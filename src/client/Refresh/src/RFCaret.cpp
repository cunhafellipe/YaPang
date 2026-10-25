// Original: client\Refresh\src\RFCaret.cpp (JP 983 exe; no function of the
// class asserts, the file name is assumed from the class). The code lies
// together at 0x455f80-0x45612a (jp/units.tsv).
#include <new>

#include "RFCaret.h"

#include "RFBrush.h"

#include <windows.h>

// lib: 0x455fd0 (scalar deleting destructor)

// 0x455f80
RFCaret::RFCaret()
	: m_visible(true), m_blinkOn(true), m_elapsed(0.0f), m_brush(0) {
	m_blinkTime = static_cast<float>(static_cast<float>(GetCaretBlinkTime()) / 1000.0);
}

// 0x456010
RFCaret::~RFCaret() {
	if (m_brush != 0) {
		delete m_brush;
		m_brush = 0;
	}
}

// 0x456040
void RFCaret::SetBrush(ImageSet* images) {
	m_brush = CreateBrush(images);
	m_size = m_brush->GetSize();
}

// 0x456070
void RFCaret::Update(float elapsed) {
	if (!m_visible) {
		return;
	}
	m_elapsed = elapsed + m_elapsed;
	if (m_elapsed > m_blinkTime) {
		m_elapsed = 0.0f;
		m_blinkOn = !m_blinkOn;
	}
}

// 0x4560b0
void RFCaret::Draw(RFVector2 const& pos, float alpha, float depth) {
	if (m_visible && m_blinkOn) {
		m_brush->Draw(pos, m_size, alpha, depth);
	}
}

// 0x4560f0
void RFCaret::SetBlinkTime(unsigned int milliseconds) {
	m_blinkTime = static_cast<float>(static_cast<float>(milliseconds) / 1000.0);
}

// 0x456120
void RFCaret::ResetBlink() {
	m_blinkOn = true;
	m_elapsed = 0.0f;
}
