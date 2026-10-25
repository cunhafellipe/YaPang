// Original: client\Refresh\src\RFRenderer.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include "RFRenderer.h"

// lib: 0x403600 (the vector deleting destructor ??_ERFRenderer, emitted by cl
// for the exported class), 0x4035d0 (the scalar deleting destructor
// ??_GRFRenderer, paired with map_symbol.ts)

// 0x403510
RFTexture* RFRenderer::CreateTexture(std::wstring const&) {
	return 0;
}

// 0x403520
void RFRenderer::DeleteTexture(RFTexture*) {
}

// 0x403530
void RFRenderer::Draw(RFTexture*, RFVector2 const&, RFSize const&, float, float, TileModeTypes::Type const&) {
}

// 0x403540
void RFRenderer::Draw(RFTexture*, float, float, float, float, RFVector2 const&, RFSize const&, float, float, TileModeTypes::Type const&) {
}

// 0x403550
void RFRenderer::Draw(RFTexture*, float, RFMatrix const&) {
}

// 0x403560
void RFRenderer::FillRectangle(uint32_t, float, float, float, float, float) {
}

// 0x403570
void RFRenderer::OnPreResetDevice() {
}

// 0x403580
void RFRenderer::OnPostResetDevice() {
}

// 0x403590
RFRenderer::~RFRenderer() {
}

// 0x4035a0
RFRenderer::RFRenderer() {
}

// 0x4035b0
RFRenderer::RFRenderer(RFRenderer const&) {
}

// 0x4035c0
RFRenderer& RFRenderer::operator=(RFRenderer const&) {
	return *this;
}
