// Original: client\Refresh\src\RFMatrix.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include <math.h>
#include <string.h>
#include "RFMatrix.h"
#include "RFVector2.h"

// lib: 0x4037c0 (RFMatrix::operator=, the implicit copy assignment)

// 0x4620b0
RFMatrix& RFMatrix::operator*=(const RFMatrix& other) {
	*this = *this * other;
	return *this;
}

// 0x462100
RFMatrix RFMatrix::operator*(const RFMatrix& other) const {
	RFMatrix result;
	memset(&result, 0, sizeof(result));
	for (unsigned int row = 0; row < 4; ++row) {
		for (unsigned int col = 0; col < 4; ++col) {
			for (unsigned int k = 0; k < 4; ++k) {
				result(row, col) += (*this)(row, k) * other(k, col);
			}
		}
	}
	return result;
}

// 0x462190
RFMatrix* RFMatrixIdentity(RFMatrix* out) {
	out->m[0][0] = 1.0f;
	out->m[0][1] = 0.0f;
	out->m[0][2] = 0.0f;
	out->m[0][3] = 0.0f;
	out->m[1][0] = 0.0f;
	out->m[1][1] = 1.0f;
	out->m[1][2] = 0.0f;
	out->m[1][3] = 0.0f;
	out->m[2][0] = 0.0f;
	out->m[2][1] = 0.0f;
	out->m[2][2] = 1.0f;
	out->m[2][3] = 0.0f;
	out->m[3][0] = 0.0f;
	out->m[3][1] = 0.0f;
	out->m[3][2] = 0.0f;
	out->m[3][3] = 1.0f;
	return out;
}

// 0x4621d0
RFMatrix* RFMatrixTranslation(RFMatrix* out, float x, float y, float z) {
	RFMatrixIdentity(out);
	out->m[3][0] = x;
	out->m[3][1] = y;
	out->m[3][2] = z;
	return out;
}

// 0x462220
RFMatrix* RFMatrixRotationZ(RFMatrix* out, float angle) {
	RFMatrixIdentity(out);
	float s = sinf(angle);
	float c = cosf(angle);
	out->m[0][0] = c;
	out->m[0][1] = s;
	out->m[1][0] = -s;
	out->m[1][1] = c;
	return out;
}

// 0x4622a0
RFMatrix* RFMatrixScaling(RFMatrix* out, float x, float y, float z) {
	RFMatrixIdentity(out);
	out->m[0][0] = x;
	out->m[1][1] = y;
	out->m[2][2] = z;
	return out;
}

// 0x4622f0
RFMatrix* RFMatrixTransformation2D(RFMatrix* out, const RFVector2* center, const RFVector2* scaling, float rotation) {
	RFMatrix step;
	RFMatrix back;
	if (center) {
		RFMatrixTranslation(&step, -center->x, -center->y, 0.0f);
		RFMatrixTranslation(&back, center->x, center->y, 0.0f);
		*out = step;
	} else {
		RFMatrixIdentity(out);
	}
	if (scaling) {
		RFMatrixScaling(&step, scaling->x, scaling->y, 1.0f);
		*out = *out * step;
	}
	if (fabsf(rotation) > 0.01f) {
		RFMatrixRotationZ(&step, rotation);
		*out = *out * step;
	}
	if (center) {
		*out = *out * back;
	}
	return out;
}

// 0x462510
RFVector2* RFMatrixTransform2DPoint(RFVector2* out, const RFVector2* point, const RFMatrix* matrix) {
	RFVector2 result(0.0f, 0.0f);
	if (point && matrix) {
		float z = 0.0f;
		result.x = point->x * matrix->m[0][0] + point->y * matrix->m[1][0] + z * matrix->m[2][0] + matrix->m[3][0];
		result.y = point->x * matrix->m[0][1] + point->y * matrix->m[1][1] + z * matrix->m[2][1] + matrix->m[3][1];
		*out = result;
	}
	return out;
}
