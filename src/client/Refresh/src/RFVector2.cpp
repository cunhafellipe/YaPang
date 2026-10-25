// Original: client\Refresh\src\RFVector2.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include <stdio.h>

#include "RFVector2.h"

// lib: 0x4012c0 (RFVector2::operator=, the implicit copy assignment)

// 0xf1ddd4
RFVector2 const RFVector2::ZERO(0.0f, 0.0f);

// 0x4289b0
RFVector2::RFVector2() : x(0.0f), y(0.0f) {}

// 0x4289c0
RFVector2& RFVector2::operator+=(RFVector2 const& other) {
	x += other.x;
	y += other.y;
	return *this;
}

// 0x4289e0
RFVector2& RFVector2::operator-=(RFVector2 const& other) {
	x -= other.x;
	y -= other.y;
	return *this;
}

// 0x428a00
RFVector2& RFVector2::operator*=(RFVector2 const& other) {
	x *= other.x;
	y *= other.y;
	return *this;
}

// 0x428a20
RFVector2& RFVector2::operator/=(RFVector2 const& other) {
	x /= other.x;
	y /= other.y;
	return *this;
}

// 0x428a40
RFVector2& RFVector2::operator*=(float scale) {
	x *= scale;
	y *= scale;
	return *this;
}

// 0x428a60
RFVector2& RFVector2::operator/=(float divisor) {
	float inverse = 1.0f / divisor;
	x *= inverse;
	y *= inverse;
	return *this;
}

// 0x428a90
RFVector2 const RFVector2::operator+(RFVector2 const& other) const {
	return RFVector2(x + other.x, y + other.y);
}

// 0x428ab0
RFVector2 const RFVector2::operator-(RFVector2 const& other) const {
	return RFVector2(x - other.x, y - other.y);
}

// 0x428ad0
RFVector2 const RFVector2::operator*(RFVector2 const& other) const {
	return RFVector2(x * other.x, y * other.y);
}

// 0x428af0
RFVector2 const RFVector2::operator/(RFVector2 const& other) const {
	return RFVector2(x / other.x, y / other.y);
}

// 0x428b10
RFVector2 const RFVector2::operator*(float scale) const {
	return RFVector2(x * scale, y * scale);
}

// 0x428b30
RFVector2 const RFVector2::operator/(float divisor) const {
	float inverse = 1.0f / divisor;
	return RFVector2(x * inverse, y * inverse);
}

// 0x428b60
bool RFVector2::operator==(RFVector2 const& other) const {
	if (x == other.x && y == other.y) {
		return true;
	}
	return false;
}

// 0x428b90
bool RFVector2::operator!=(RFVector2 const& other) const {
	if (x == other.x && y == other.y) {
		return false;
	}
	return true;
}

// 0x428bc0
RFVector2 RFVector2::FromString(std::string const& text) {
	float x;
	float y;
	sscanf_s(text.c_str(), "%f, %f", &x, &y);
	return RFVector2(x, y);
}
