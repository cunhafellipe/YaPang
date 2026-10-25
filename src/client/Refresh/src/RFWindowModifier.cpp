// Original: client\Refresh\src\RFWindowModifier.cpp (JP 983 exe, unit bounds
// in jp/refresh_units.tsv; the file name is assumed from the class)
#include "RFWindowModifier.h"

// lib: 0x4047b0 (??_ERFWindowModifier, compiler-generated vector deleting
// destructor, paired with map_symbol.ts), 0x4046a0 (implicit copy
// constructor), 0x404710 (implicit operator=), 0x404760 (??_GRFWindowModifier,
// scalar deleting destructor, paired with map_symbol.ts)

// 0x404560
RFWindowModifier::RFWindowModifier() : m_pos(0.0f, 0.0f) {
	m_visible = false;
	m_alpha = 0.0f;
}

// 0x404590
RFWindowModifier::~RFWindowModifier() {
}

// 0x4045d0
std::wstring const& RFWindowModifier::GetName() const {
	return m_name;
}

// 0x4045e0
void RFWindowModifier::SetName(std::wstring const& name) {
	m_name = name;
}

// 0x404600
float RFWindowModifier::GetAlpha() const {
	return m_alpha;
}

// 0x404610
void RFWindowModifier::SetAlpha(float alpha) {
	m_alpha = alpha;
}

// 0x404620
bool RFWindowModifier::IsVisible() const {
	return m_visible;
}

// 0x404630
void RFWindowModifier::SetVisible(bool visible) {
	m_visible = visible;
}

// 0x404640
RFVector2 const& RFWindowModifier::GetPos() const {
	return m_pos;
}

// 0x404650
void RFWindowModifier::SetPos(RFVector2 const& pos) {
	m_pos = pos;
}

// 0x404670
RFSize const& RFWindowModifier::GetSize() const {
	return m_size;
}

// 0x404680
void RFWindowModifier::SetSize(RFSize const& size) {
	m_size = size;
}
