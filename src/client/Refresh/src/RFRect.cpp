// Original: client\Refresh\src\RFRect.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include "RFRect.h"

// lib: 0x401510 (RFRect::operator=, the implicit copy assignment)

// 0xf1ddc4
RFRect const RFRect::ZERO;

// 0x41dd10
bool RFRect::PtInRect(float x, float y) const {
	if (x >= left && x <= right && y >= top && y <= bottom) {
		return true;
	}
	return false;
}

// 0x41dd60
bool RFRect::PtInRect(RFVector2 const& point) const {
	return PtInRect(point.x, point.y);
}

// 0x41ddc0
bool RFRect::IntersectRect(RFRect const& a, RFRect const& b) {
	left = a.left > b.left ? a.left : b.left;
	top = a.top > b.top ? a.top : b.top;
	right = a.right < b.right ? a.right : b.right;
	bottom = a.bottom < b.bottom ? a.bottom : b.bottom;
	if (left < right && top < bottom) {
		return true;
	}
	set(0.0f, 0.0f, 0.0f, 0.0f);
	return false;
}

// 0x41de80
RFRect RFRect::MoveRect(float dx, float dy) const {
	return RFRect(left + dx, top + dy, right + dx, bottom + dy);
}

// 0x41dec0
void RFRect::ApplyPadding(RFRect const& padding) {
	left += padding.left;
	top += padding.top;
	right -= padding.right;
	bottom -= padding.bottom;
}

// 0x41def0
RFRect RFRect::InflateRect(float dx, float dy) {
	return RFRect(left - dx, top - dy, right + dx, bottom + dy);
}

// 0x41df30
RFRect RFRect::DeflateRect(float dx, float dy) {
	return RFRect(left + dx, top + dy, right - dx, bottom - dy);
}

// 0x41df70
void RFRect::operator+=(RFRect const& other) {
	left += other.left;
	right += other.right;
	top += other.top;
	bottom += other.bottom;
}

// 0x41dfa0
void RFRect::operator-=(RFRect const& other) {
	left -= other.left;
	right -= other.right;
	top -= other.top;
	bottom -= other.bottom;
}

// 0x41dfd0
void RFRect::operator*=(float scale) {
	left *= scale;
	right *= scale;
	top *= scale;
	bottom *= scale;
}

// 0x41e000
RFRect RFRect::operator+(RFRect const& other) const {
	return RFRect(left + other.left, top + other.top, right + other.right, bottom + other.bottom);
}

// 0x41e030
RFRect RFRect::operator-(RFRect const& other) const {
	return RFRect(left - other.left, top - other.top, right - other.right, bottom - other.bottom);
}

// 0x41e060
RFRect RFRect::operator*(RFRect const& other) const {
	return RFRect(left * other.left, top * other.top, right * other.right, bottom * other.bottom);
}

// 0x41e090
RFRect RFRect::operator/(RFRect const& other) const {
	return RFRect(left / other.left, top / other.top, right / other.right, bottom / other.bottom);
}
