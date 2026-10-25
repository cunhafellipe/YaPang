// Original: client\Refresh\src\RFInputMethodProvider.cpp (JP 983 exe; no
// assert names the .cpp, the file name is assumed from the class). The TU's
// code lies together at 0x455d00-0x455f7a (jp/refresh_placed.tsv): the
// constructor 0x455d00 (called, not inlined, by RFManager::InitializeInputMethod),
// the scalar deleting destructor 0x455d60, the destructor 0x455d90, twelve
// forwarders to m_service 0x455db0-0x455f10 and the
// RFSingleton<RFInputMethodProvider> constructor 0x455f30 and destructor
// 0x455f70. Its asserts spell the include directory in upper case
// (scripts/lib/msvc.ts REFRESH_UPPER_INCLUDE_SOURCES). The class is declared
// in src/RFIme.h, where its constructor and destructor are still inline: they
// are defined here once that header declares them.
#include <new>

#include "RFIme.h"

// 0x455d00
RFInputMethodProvider::RFInputMethodProvider()
	: m_service(0) {
}

// 0x455d90
RFInputMethodProvider::~RFInputMethodProvider() {
}

// 0x455db0
void RFInputMethodProvider::SetService(RFInputMethodService* service) {
	m_service = service;
}

// 0x455dc0
void RFInputMethodProvider::Enable(bool enable) {
	if (m_service) {
		m_service->Enable(enable);
	}
}

// 0x455de0
bool RFInputMethodProvider::IsEnabled() {
	if (m_service) {
		return m_service->IsEnabled();
	}
	return false;
}

// 0x455e00
void RFInputMethodProvider::RestoreFocus() {
	if (m_service) {
		m_service->RestoreFocus();
	}
}

// 0x455e20
void RFInputMethodProvider::SetNativeMode(bool native) {
	if (m_service) {
		m_service->SetNativeMode(native);
	}
}

// 0x455e40
bool RFInputMethodProvider::IsNativeMode() {
	if (m_service) {
		return m_service->IsNativeMode();
	}
	return false;
}

// 0x455e60
void RFInputMethodProvider::GetStatus(int* language, int* conversion) {
	if (m_service) {
		m_service->GetStatus(language, conversion);
	}
}

// 0x455e80
void RFInputMethodProvider::ActivateContext(bool activate) {
	if (m_service) {
		m_service->ActivateContext(activate);
	}
}

// 0x455ea0
void RFInputMethodProvider::SetCompositionPos(int x, int y) {
	if (m_service) {
		m_service->SetCompositionPos(x, y);
	}
}

// 0x455ec0
void RFInputMethodProvider::EndComposition(bool complete) {
	if (m_service) {
		m_service->EndComposition(complete);
	}
}

// 0x455ee0
int RFInputMethodProvider::PopResult(wchar_t* buffer, int length, bool* composing) {
	if (m_service) {
		return m_service->PopResult(buffer, length, composing);
	}
	return -1;
}

// 0x455f10
int RFInputMethodProvider::GetResultCount() {
	if (m_service) {
		return m_service->GetResultCount();
	}
	return 0;
}
