// Original: client\Refresh\include\RFMatrix.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFMATRIX_H
#define PG_CLIENT_REFRESH_INCLUDE_RFMATRIX_H

#include "RFExport.h"
#include "defines.h"

class RFVector2;

// A row-major 4x4 matrix, row vectors (the translation is row 3).
// dllexport as in the original library: cl 16 then emits the header inlines
// and the implicit copy assignment (0x4037c0) where the linker kept them.
class RF_API RFMatrix {
public:
	// 0x403750, leaves the elements uninitialized (the RFAnimFrame
	// constructor calls no constructor for its matrix)
	RFMatrix() {}

	// 0x403760
	float& operator()(unsigned int row, unsigned int col) {
		return m[row][col];
	}
	// 0x403780
	float operator()(unsigned int row, unsigned int col) const {
		return m[row][col];
	}

	// 0x4037a0
	operator float*() {
		return m[0];
	}
	// 0x4037b0
	operator const float*() const {
		return m[0];
	}
	// operator= 0x4037c0 is the implicit copy assignment.

	RFMatrix& operator*=(const RFMatrix& other);      // 0x4620b0
	RFMatrix operator*(const RFMatrix& other) const;  // 0x462100

	float m[4][4];    // 0x0
};
CHECK_SIZE(RFMatrix, 0x40);
CHECK_FIELD(RFMatrix, m, 0x0);

RF_API RFMatrix* RFMatrixIdentity(RFMatrix* out);
RF_API RFMatrix* RFMatrixTranslation(RFMatrix* out, float x, float y, float z);
RF_API RFMatrix* RFMatrixRotationZ(RFMatrix* out, float angle);
RF_API RFMatrix* RFMatrixScaling(RFMatrix* out, float x, float y, float z);
RF_API RFMatrix* RFMatrixTransformation2D(RFMatrix* out, const RFVector2* center, const RFVector2* translation, float rotation);
RF_API RFVector2* RFMatrixTransform2DPoint(RFVector2* out, const RFVector2* point, const RFMatrix* matrix);

#endif
