// Original: client\Refresh\src\RFSize.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include <stdio.h>

#include "RFSize.h"

// lib: 0x4013b0 (RFSize::operator=, the implicit copy assignment)

// 0x41dc50
bool RFSize::operator==(RFSize const& other) const {
	if (width == other.width && height == other.height) {
		return true;
	}
	return false;
}

// 0x41dc80
bool RFSize::operator!=(RFSize const& other) const {
	if (width == other.width && height == other.height) {
		return false;
	}
	return true;
}

// 0x41dcb0
RFSize RFSize::operator-(RFSize const& other) const {
	return RFSize(width - other.width, height - other.height);
}

// 0x41dcd0
RFSize RFSize::FromString(std::string const& text) {
	float width;
	float height;
	sscanf_s(text.c_str(), "%f, %f", &width, &height);
	return RFSize(width, height);
}
