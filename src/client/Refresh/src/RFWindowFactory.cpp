// Original: client\Refresh\src\RFWindowFactory.cpp (JP 983 exe, unit bounds
// in jp/refresh_units.tsv; the file name is assumed from the class)
#include "RFWindowFactory.h"

// 0x416f80 ~RFWindowFactory: the inline in RFWindowFactory.h (dllexport
// emits its out-of-line copy here).
// 0x4170c0 vector deleting destructor ??_ERFWindowFactory (compiler-generated);
// 0x417070 the scalar deleting destructor ??_GRFWindowFactory (unreferenced,
// paired with map_symbol.ts).
// lib: 0x4170c0, 0x417070

// 0x416fc0
std::wstring const& RFWindowFactory::GetTypeName() const {
	return m_typeName;
}

// 0x416fd0
RFWindowFactory::RFWindowFactory(std::wstring const& typeName)
	: m_typeName(typeName) {
}

// 0x417010
RFWindowFactory::RFWindowFactory(RFWindowFactory const& other)
	: m_typeName(other.m_typeName) {
}

// 0x417050
RFWindowFactory& RFWindowFactory::operator=(RFWindowFactory const& other) {
	m_typeName = other.m_typeName;
	return *this;
}
