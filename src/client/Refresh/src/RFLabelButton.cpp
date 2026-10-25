// Original: client\Refresh\src\RFLabelButton.cpp (JP 983 exe, unit bounds
// in jp/refresh_units.tsv; the file name is assumed from the class)
#include <assert.h>

#include "RFLabelButton.h"
#include "RFManager.h"
#include "RFWindowFactoryManager.h"

// lib: 0x414780 (implicit copy constructor), 0x414800 (implicit operator=),
// 0x4148a0 (vector deleting destructor), 0x414870 (scalar deleting destructor)

// The class name, the static const at 0xe46dd0.
std::wstring const RFLabelButton::TypeName = L"LabelButton";

// 0x414760
wchar_t const* RFLabelButton::ToString() const {
	return L"RFLabelButton";
}

// 0x414770
std::wstring const& RFLabelButton::GetTypeName() {
	return TypeName;
}

// 0x425f50
RFLabelButton::RFLabelButton() {
}

// 0x425fb0; vector deleting destructor 0x4148a0
RFLabelButton::~RFLabelButton() {
}

// 0x425fc0 (RFManager::GetInstance, RFWindowFactoryManager::GetInstance and
// RFWnd::SetSubControl are inlined by LTCG in the original)
void RFLabelButton::Create(WndBaseStruct* wndStruct, RFWnd* parent) {
	RFButton::Create(wndStruct, parent);
	LabelButtonBase* data = static_cast<LabelButtonBase*>(wndStruct);
	if (data != 0) {
		m_useStateColors = data->m_useStateColors;
		m_normalColor = data->m_normalColor.GetValue();
		m_overColor = data->m_overColor.GetValue();
		m_downColor = data->m_downColor.GetValue();
		m_disableColor = data->m_disableColor.GetValue();
		std::wstring const& name =
			RFManager::GetInstance().GetCompatibleControlName(data->m_labelStruct->GetType());
		RFWindowFactory* factory = RFWindowFactoryManager::GetInstance().GetFactory(name);
		assert(factory);
		RFWnd* wnd = factory->Create();
		wnd->Create(data->m_labelStruct, this);
		wnd->Initialize();
		wnd->SetSubControl(true);
		m_label = static_cast<RFLabel*>(wnd);
	}
}

// 0x4260e0
void RFLabelButton::PreInitialize() {
	RFButton::PreInitialize();
	m_lastState = GetState();
	if (m_useStateColors && m_label) {
		m_label->SetColor(m_normalColor);
	}
}

// 0x426120
void RFLabelButton::SetLabel(std::wstring const& label) {
	m_label->SetLabel(label);
}

// 0x426160
std::wstring const& RFLabelButton::GetLabel() const {
	return m_label->GetLabel();
}

// 0x426170
void RFLabelButton::OnUpdate(UpdateEventArgs&) {
	if (m_label && m_useStateColors && m_lastState != GetState()) {
		switch (GetState()) {
		case RFBUTTON_STATE_NORMAL:
			m_label->SetColor(m_normalColor);
			break;
		case RFBUTTON_STATE_DOWN:
			m_label->SetColor(m_downColor);
			break;
		case RFBUTTON_STATE_OVER:
			m_label->SetColor(m_overColor);
			break;
		case RFBUTTON_STATE_DISABLE:
			m_label->SetColor(m_disableColor);
			break;
		}
		m_lastState = GetState();
	}
}

