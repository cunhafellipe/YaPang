// Original: client\Refresh\src\RFTexture.cpp (JP 983 exe, unit assigned from
// the exports, jp/refresh_units.tsv; the file name is assumed from the class)
#include <new>

#include "RFTexture.h"

// lib: 0x4034b0 (the vector deleting destructor ??_ERFTexture, compiler-
// generated, paired through map_symbol.ts), 0x403480 (the scalar deleting
// destructor ??_GRFTexture, paired through map_symbol.ts)

// 0x403420
RFTexture::~RFTexture() {
}

// 0x403430
unsigned int RFTexture::GetWidth() const {
	return 0;
}

// 0x403440
unsigned int RFTexture::GetHeight() const {
	return 0;
}

// 0x403450
RFTexture::RFTexture() {
}

// 0x403460
RFTexture::RFTexture(RFTexture const&) {
}

// 0x403470
RFTexture& RFTexture::operator=(RFTexture const&) {
	return *this;
}
