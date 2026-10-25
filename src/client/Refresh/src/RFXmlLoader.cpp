// Original: client\Refresh\src\RFXmlLoader.cpp (JP 983 exe, unit bounds in
// jp/units.tsv; the file name comes from its asserts, src\RFXmlLoader.cpp)
#include <new>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "RFAnimButton.h"
#include "RFBrush.h"
#include "RFButton.h"
#include "RFCheck.h"
#include "RFComboBox.h"
#include "RFEdit.h"
#include "RFInterface.h"
#include "RFLabel.h"
#include "RFLabelButton.h"
#include "RFList.h"
#include "RFListSC.h"
#include "RFPicture.h"
#include "RFProgress.h"
#include "RFProperty.h"
#include "RFScrollBar.h"
#include "RFSlider.h"
#include "RFStackPanel.h"
#include "RFTextArea.h"
#include "RFTextAreaSC.h"
#include "RFWnd.h"
#include "RFWndEx.h"
#include "RFXmlLoader.h"

// lib: 0x438dd0, 0x438e50, 0x438f10, 0x4390f0, 0x4391b0, 0x43a920, 0x43bbe0, 0x439f20, 0x439ff0,
// 0x43a0d0 (the scalar deleting destructors)
// lib: 0x4388b0, 0x4388d0, 0x4388e0, 0x438910, 0x438a00, 0x438a10, 0x438a30, 0x438b20, 0x438b50,
// 0x438bd0, 0x438be0, 0x438c10 (std:: pair / map value_type constructors, via the wstring copy 0x409ff0)
// lib: 0x438c90, 0x438ca0, 0x438cd0, 0x438ce0, 0x438d10, 0x438d20 (rf::TiXml* header inlines: sibling
// and child element walks, attribute-set First/Next sentinel tests, a thunk to 0x45f400)
// lib: 0x43dd80 (scalar deleting destructor)
// lib: 0x438fb0, 0x439050, 0x439250, 0x4392f0, 0x4393a0, 0x439440 (the enum properties' scalar
// lib: 0x43a5d0 deleting destructors, then FrameImageSet's)
// lib: 0x43ddb0, 0x43de10 (the RFXmlLoader maps' out-of-line std::_Tree destructors)
// lib: 0x4402f0, 0x440310 (the implicit PictureBase, ProgressBase constructors)
// lib: 0x438ec0, 0x438f60, 0x439000, 0x4390a0, 0x439140, 0x439200, 0x4392a0, 0x439340, 0x4393f0,
// 0x439490, 0x43a8c0, 0x43b900 (the properties' implicit destructors, this in esi: LTCG)
// lib: 0x439f50, 0x43a020, 0x43a100, 0x43a1b0, 0x43a270, 0x43a290 (the Testing exceptions' implicit
// destructors and copy constructors)
// lib: 0x439e20 (a std::_Tree destructor body, called from an unwind funclet)
// lib: 0x43a360 (an unreferenced out-of-line copy of RFReferenceCount::AddRef, RFBrush.h:
// `return m_refCount++;`)
// lib: 0x438970, 0x438a90, 0x44b120, 0x44b880, 0x44c840, 0x44d170 (std:: pieces: a wstring pair's
// move construction, a forwarding thunk, string _Tidy, the memmove copy helper, vector moves)
// The end of the unit, 0x447ec0-0x44d2b0, is the std:: instantiations the loader uses
// (basic_string copies and at(), _Tree insert/erase/find/ctor of its maps, vector growth):
// lib: 0x447ec0, 0x447ef0, 0x447f20, 0x447f30, 0x447f50, 0x447fa0, 0x448000, 0x448020, 0x448090, 0x448190
// lib: 0x4481f0, 0x448200, 0x448280, 0x448290, 0x4482a0, 0x4482b0, 0x4482e0, 0x4482f0, 0x448300, 0x448360
// lib: 0x448390, 0x448400, 0x4484c0, 0x448520, 0x448530, 0x448560, 0x4485c0, 0x448630, 0x448690, 0x4486a0
// lib: 0x4486d0, 0x448760, 0x448770, 0x448780, 0x448790, 0x448860, 0x4488b0, 0x4488d0, 0x4488e0, 0x4488f0
// lib: 0x448920, 0x448950, 0x448980, 0x4489b0, 0x448a30, 0x448aa0, 0x448b10, 0x448b20, 0x448b40, 0x448b50
// lib: 0x448b60, 0x448b70, 0x448b80, 0x448b90, 0x448ba0, 0x448bb0, 0x448bc0, 0x448c20, 0x448c40, 0x448ca0
// lib: 0x448cd0, 0x448ce0, 0x448cf0, 0x448d00, 0x448d10, 0x448d80, 0x448db0, 0x448e10, 0x448e20, 0x448e30
// lib: 0x448e40, 0x448e60, 0x448e70, 0x448e80, 0x448e90, 0x448ea0, 0x448f10, 0x448f30, 0x448fa0, 0x448fb0
// lib: 0x448fc0, 0x448fd0, 0x448ff0, 0x449000, 0x449010, 0x449020, 0x449080, 0x449090, 0x4490a0, 0x4490b0
// lib: 0x4490f0, 0x449110, 0x449190, 0x4491e0, 0x4491f0, 0x449200, 0x449210, 0x449220, 0x449230, 0x449240
// lib: 0x449250, 0x449260, 0x449290, 0x4492a0, 0x449360, 0x4493d0, 0x449440, 0x449450, 0x449460, 0x449470
// lib: 0x449480, 0x449490, 0x449510, 0x449520, 0x449530, 0x449540, 0x449550, 0x449580, 0x449590, 0x4495a0
// lib: 0x4495b0, 0x4495c0, 0x449680, 0x4496b0, 0x449720, 0x449730, 0x449740, 0x449750, 0x449760, 0x449770
// lib: 0x449780, 0x449790, 0x4497a0, 0x449860, 0x449960, 0x4499d0, 0x4499e0, 0x4499f0, 0x449a00, 0x449a10
// lib: 0x449a20, 0x449a30, 0x449a40, 0x449a50, 0x449a90, 0x449bb0, 0x449bd0, 0x449bf0, 0x449c00, 0x449c20
// lib: 0x449c60, 0x449c70, 0x449c80, 0x449ca0, 0x449cb0, 0x449cc0, 0x449cf0, 0x449d00, 0x449d10, 0x449d20
// lib: 0x449fb0, 0x449fe0, 0x449ff0, 0x44a000, 0x44a010, 0x44a020, 0x44a030, 0x44a040, 0x44a050, 0x44a060
// lib: 0x44a080, 0x44a0a0, 0x44a0b0, 0x44a0f0, 0x44a100, 0x44a110, 0x44a120, 0x44a160, 0x44a1c0, 0x44a1d0
// lib: 0x44a450, 0x44a460, 0x44a470, 0x44a4b0, 0x44a740, 0x44a750, 0x44a760, 0x44a7a0, 0x44a7b0, 0x44a7c0
// lib: 0x44a7d0, 0x44a840, 0x44a850, 0x44a860, 0x44a870, 0x44a880, 0x44a890, 0x44a900, 0x44a960, 0x44a9c0
// lib: 0x44a9d0, 0x44a9e0, 0x44aa20, 0x44aa80, 0x44aae0, 0x44ab00, 0x44ab20, 0x44ab30, 0x44ab90, 0x44abf0
// lib: 0x44ac10, 0x44ac30, 0x44ac40, 0x44ac50, 0x44ac60, 0x44ac70, 0x44acc0, 0x44ad20, 0x44ad30, 0x44ad80
// lib: 0x44add0, 0x44ade0, 0x44adf0, 0x44ae00, 0x44ae10, 0x44ae40, 0x44ae60, 0x44ae70, 0x44ae80, 0x44aea0
// lib: 0x44aeb0, 0x44aec0, 0x44aed0, 0x44af00, 0x44af10, 0x44af70, 0x44af80, 0x44afa0, 0x44afb0, 0x44afe0
// lib: 0x44aff0, 0x44b000, 0x44b030, 0x44b040, 0x44b0a0, 0x44b0b0, 0x44b0c0, 0x44b0f0, 0x44b100, 0x44b140
// lib: 0x44b150, 0x44b180, 0x44b190, 0x44b1e0, 0x44b1f0, 0x44b240, 0x44b250, 0x44b2a0, 0x44b2b0, 0x44b300
// lib: 0x44b360, 0x44b370, 0x44b380, 0x44b390, 0x44b3a0, 0x44b520, 0x44b580, 0x44b5b0, 0x44b5c0, 0x44b5d0
// lib: 0x44b5f0, 0x44b710, 0x44b740, 0x44b8b0, 0x44b910, 0x44ba70, 0x44bca0, 0x44bcb0, 0x44bcc0, 0x44bcd0
// lib: 0x44bda0, 0x44bfe0, 0x44c210, 0x44c220, 0x44c230, 0x44c240, 0x44c260, 0x44c270, 0x44c280, 0x44c290
// lib: 0x44c2f0, 0x44c300, 0x44c310, 0x44c320, 0x44c330, 0x44c390, 0x44c3a0, 0x44c3c0, 0x44c3e0, 0x44c400
// lib: 0x44c410, 0x44c420, 0x44c430, 0x44c490, 0x44c4b0, 0x44c4d0, 0x44c4e0, 0x44c4f0, 0x44c520, 0x44c530
// lib: 0x44c610, 0x44c620, 0x44c700, 0x44c710, 0x44c720, 0x44c730, 0x44c740, 0x44c810, 0x44c820, 0x44c870
// lib: 0x44c880, 0x44c8a0, 0x44c8b0, 0x44c8f0, 0x44c900, 0x44c930, 0x44c940, 0x44c970, 0x44c990, 0x44c9c0
// lib: 0x44c9e0, 0x44ca00, 0x44ca10, 0x44ca20, 0x44ca30, 0x44ca60, 0x44ca90, 0x44cac0, 0x44caf0, 0x44cb20
// lib: 0x44cb50, 0x44cb80, 0x44cbe0, 0x44cc40, 0x44cca0, 0x44cd00, 0x44cd80, 0x44cd90, 0x44cdc0, 0x44cdd0
// lib: 0x44ce90, 0x44cec0, 0x44ced0, 0x44cef0, 0x44cf00, 0x44cf20, 0x44cf30, 0x44cf50, 0x44cfd0, 0x44d000
// lib: 0x44d060, 0x44d0c0, 0x44d100, 0x44d130, 0x44d1b0, 0x44d1e0, 0x44d210, 0x44d220, 0x44d230, 0x44d240
// lib: 0x44d270, 0x44d280

// An attribute's value, or defaultValue when the attribute is missing
// (inlined everywhere; the parsers read every attribute through it).
static char const* GetAttribute(rf::TiXmlElement* element, char const* name,
                                char const* defaultValue = "") {
	char const* value = element->Attribute(name);
	if (value != 0)
		return value;
	return defaultValue;
}

// 0x438d40
RFProperty::~RFProperty() {
}

// 0x438d70
std::string RFProperty::GetName() const {
	return m_name;
}

// 0x438da0
void RFProperty::SetName(std::string const& name) {
	m_name = name;
}

// 0x438dc0
void RFProperty::Parse(std::string const& text) {
}

// 0x438e20
RFBooleanProperty::RFBooleanProperty() : m_value(false) {
}

// 0x438e40
void RFBooleanProperty::SetValue(bool value) {
	m_value = value;
}

// 0x438ea0
RFProperty::RFProperty() {
}

// 0x438ef0
RFIntegerProperty::RFIntegerProperty() : m_value(0) {
}

// 0x438f90
RFStartPositionProperty::RFStartPositionProperty() : m_value(static_cast<StartPositionTypes::Type>(0)) {
}

// 0x439030
RFEscKeyActionProperty::RFEscKeyActionProperty() : m_value(static_cast<EscKeyActionTypes::Type>(0)) {
}

// 0x4390d0
RFAnchorProperty::RFAnchorProperty() : m_value(0) {
}

// 0x439170
RFColorProperty::RFColorProperty() : m_value(0) {
}

// 0x439190
void RFColorProperty::SetValue(uint32_t value) {
	m_value = value;
}

// The second identical dword setter beside 0x439190 (see RFProperty.h).
// 0x4391a0
void RFAnchorProperty::SetValue(uint32_t value) {
	m_value = value;
}

// 0x439230
RFHAlignProperty::RFHAlignProperty() : m_value(HorzAlignTypes::Left) {
}

// 0x4392d0
RFVAlignProperty::RFVAlignProperty() : m_value(VertAlignTypes::Top) {
}

// 0x439370
RFTextTrimmingProperty::RFTextTrimmingProperty() : m_value(TextTrimmingTypes::None) {
}

// 0x439390
void RFTextTrimmingProperty::SetValue(TextTrimmingTypes::Type value) {
	m_value = value;
}

// 0x439420
RFOrientationProperty::RFOrientationProperty() : m_value(OrientationTypes::Vertical) {
}

// The original's split cuts this constructor into pieces (0x4394c0, then one
// per property from 0x43969c to 0x4399d4).
// 0x4394c0
WndBaseStruct::WndBaseStruct() {
	m_hitTestVisible.SetValue(true);
	m_hitTestThrough.SetValue(false);
	m_clipChildren.SetValue(false);
	m_dragable.SetName("Dragable");
	AddProperty(&m_dragable);
	m_hitTestVisible.SetName("IsHitTestVisible");
	AddProperty(&m_hitTestVisible);
	m_hitTestThrough.SetName("IsHitTestThrough");
	AddProperty(&m_hitTestThrough);
	m_startPosition.SetName("StartPosition");
	AddProperty(&m_startPosition);
	m_escKeyAction.SetName("EscKeyAction");
	AddProperty(&m_escKeyAction);
	m_anchor.SetName("Anchor");
	AddProperty(&m_anchor);
	m_clipChildren.SetName("ClipChildren");
	AddProperty(&m_clipChildren);
}

// 0x439a80
WndBaseStruct::~WndBaseStruct() {
}

// 0x439d00
void WndBaseStruct::AddProperty(RFProperty* property) {
	if (property)
		m_properties[property->GetName()] = property;
}

// 0x439d80
RFProperty* WndBaseStruct::GetProperty(std::string const& name) {
	std::map<std::string, RFProperty*, NoCaseLess>::iterator it = m_properties.find(name);
	if (it == m_properties.end())
		return 0;
	return it->second;
}

// 0x439e90
Testing::RFException::RFException(std::wstring const& message)
#if defined(_MSC_VER)
	: std::exception(WStringToMultiByte(message).c_str()) {
#else
	: m_what(WStringToMultiByte(message)) {
#endif
}

// 0x439f60
Testing::RFInvalidArgException::RFInvalidArgException(std::wstring const& message)
#if defined(_MSC_VER)
	: std::exception(WStringToMultiByte(message).c_str()) {
#else
	: m_what(WStringToMultiByte(message)) {
#endif
}

// 0x43a030
Testing::RFFileException::RFFileException(std::wstring const& message, int error)
#if defined(_MSC_VER)
	: std::exception(WStringToMultiByte(message).c_str()), m_error(error) {
#else
	: m_error(error), m_what(WStringToMultiByte(message)) {
#endif
}

// The loader's throw helpers: every throw site of the unit builds the message
// and calls one of them (LTCG inlined the exception constructor into each).
// 0x43a110
static void ThrowException(std::wstring const& message) {
	throw Testing::RFException(message);
}

// 0x43a1d0
static void ThrowInvalidArgException(std::wstring const& message) {
	throw Testing::RFInvalidArgException(message);
}

// 0x43a2c0
static void ThrowFileException(std::wstring const& filename, int error) {
	throw Testing::RFFileException(filename, error);
}

// 0x43a440
FrameImageSet::~FrameImageSet() {
}

// 0x43a5c0
ImageSetTypes::Type FrameImageSet::GetType() const {
	return ImageSetTypes::Frame;
}

// 0x43a600
CheckBase::CheckBase() {
	m_clickSound.SetName("PushSound");
	AddProperty(&m_clickSound);
	m_brushDescs[0] = 0;
	m_brushDescs[1] = 0;
}

// 0x43a720
CheckBase::~CheckBase() {
	if (m_brushDescs[0]) {
		if (m_brushDescs[0]->Release() == 0)
			delete m_brushDescs[0];
		m_brushDescs[0] = 0;
	}
	if (m_brushDescs[1]) {
		if (m_brushDescs[1]->Release() == 0)
			delete m_brushDescs[1];
		m_brushDescs[1] = 0;
	}
}

// 0x43a890
RFStringProperty::RFStringProperty() {
}

// 0x43aaf0
EditBase::EditBase() {
	m_color.SetValue(0xffffffff);
	m_fontName.SetName("FontName");
	AddProperty(&m_fontName);
	m_fontSize.SetName("FontSize");
	AddProperty(&m_fontSize);
	m_color.SetName("Color");
	AddProperty(&m_color);
	m_shadowColor.SetName("ShadowColor");
	AddProperty(&m_shadowColor);
	m_outlineColor.SetName("OutlineColor");
	AddProperty(&m_outlineColor);
}

// 0x43aef0
EditBase::~EditBase() {
	if (m_caretBrushDesc) {
		if (m_caretBrushDesc->Release() == 0)
			delete m_caretBrushDesc;
		m_caretBrushDesc = 0;
	}
}

// 0x43b140
LabelBase::LabelBase() {
	m_color.SetValue(0xffffffff);
	m_textTrimmingType.SetValue(TextTrimmingTypes::None);
	m_padding.SetName("Padding");
	AddProperty(&m_padding);
	m_fontName.SetName("FontName");
	AddProperty(&m_fontName);
	m_fontSize.SetName("FontSize");
	AddProperty(&m_fontSize);
	m_color.SetName("Color");
	AddProperty(&m_color);
	m_shadowColor.SetName("ShadowColor");
	AddProperty(&m_shadowColor);
	m_outlineColor.SetName("OutlineColor");
	AddProperty(&m_outlineColor);
	m_hAlign.SetName("HAlign");
	AddProperty(&m_hAlign);
	m_vAlign.SetName("VAlign");
	AddProperty(&m_vAlign);
	m_textTrimmingType.SetName("TextTrimming");
	AddProperty(&m_textTrimmingType);
	m_autoFit.SetName("FitToContent");
	AddProperty(&m_autoFit);
}

// 0x43b8d0
RFRectProperty::RFRectProperty() {
}

// 0x43bc30
ButtonBase::ButtonBase() {
	m_defaultHandler.SetName("IsDefaultButton");
	AddProperty(&m_defaultHandler);
	for (int i = 0; i < 4; ++i)
		m_stateImages[i] = 0;
}

// 0x43bd90
ButtonBase::~ButtonBase() {
	if (m_stateImages[0]) {
		if (m_stateImages[0]->Release() == 0)
			delete m_stateImages[0];
		m_stateImages[0] = 0;
	}
	if (m_stateImages[1]) {
		if (m_stateImages[1]->Release() == 0)
			delete m_stateImages[1];
		m_stateImages[1] = 0;
	}
	if (m_stateImages[2]) {
		if (m_stateImages[2]->Release() == 0)
			delete m_stateImages[2];
		m_stateImages[2] = 0;
	}
	if (m_stateImages[3]) {
		if (m_stateImages[3]->Release() == 0)
			delete m_stateImages[3];
		m_stateImages[3] = 0;
	}
}

// 0x43bfc0
LabelButtonBase::LabelButtonBase() {
	m_useStateColors = true;
	m_normalColor.SetValue(0xffffffff);
	m_overColor.SetValue(0xffffffff);
	m_downColor.SetValue(0xffffffff);
	m_disableColor.SetValue(0xffffffff);
	m_normalColor.SetName("NormalColor");
	AddProperty(&m_normalColor);
	m_overColor.SetName("HoverColor");
	AddProperty(&m_overColor);
	m_downColor.SetName("PushColor");
	AddProperty(&m_downColor);
	m_disableColor.SetName("DisableColor");
	AddProperty(&m_disableColor);
}

// 0x43c420
AnimButtonBase::AnimButtonBase() {
	m_state4Image = 0;
	m_state5Image = 0;
}

// 0x43c440
AnimButtonBase::~AnimButtonBase() {
	if (m_state4Image) {
		if (m_state4Image->Release() == 0)
			delete m_state4Image;
		m_state4Image = 0;
	}
	if (m_state5Image) {
		if (m_state5Image->Release() == 0)
			delete m_state5Image;
		m_state5Image = 0;
	}
}

// 0x43c570
StackPanelBase::StackPanelBase() {
	m_hAlign.SetName("HAlign");
	AddProperty(&m_hAlign);
	m_vAlign.SetName("VAlign");
	AddProperty(&m_vAlign);
	m_orientation.SetName("Orientation");
	AddProperty(&m_orientation);
}

// 0x43c8d0
TextAreaBase::TextAreaBase() {
	m_fontName.SetName("FontName");
	AddProperty(&m_fontName);
	m_fontSize.SetName("FontSize");
	AddProperty(&m_fontSize);
	m_color.SetName("Color");
	AddProperty(&m_color);
	m_shadowColor.SetName("ShadowColor");
	AddProperty(&m_shadowColor);
	m_outlineColor.SetName("OutlineColor");
	AddProperty(&m_outlineColor);
	m_showCaret.SetName("Editable");
	AddProperty(&m_showCaret);
	m_text.SetName("Text");
	AddProperty(&m_text);
	m_autoFit.SetName("FitToContent");
	AddProperty(&m_autoFit);
}

// 0x43d1e0
TextAreaSCBase::TextAreaSCBase() {
	m_hScrollBar = 0;
	m_vScrollBar = 0;
}

// 0x43d270
ListBase::ListBase() {
	m_orientation.SetName("Orientation");
	AddProperty(&m_orientation);
}

// 0x43d4a0
ListSCBase::ListSCBase() : m_hScrollBar(0), m_vScrollBar(0) {
}

// 0x43d510
ComboBase::ComboBase() {
	m_itemTextColor.SetName("NormalColor");
	AddProperty(&m_itemTextColor);
	m_itemSelTextColor.SetName("SelectColor");
	AddProperty(&m_itemSelTextColor);
	m_itemFontName.SetName("FontName");
	AddProperty(&m_itemFontName);
	m_itemFontSize.SetName("FontSize");
	AddProperty(&m_itemFontSize);
	m_itemTextColor.SetValue(0xff0000ff);
	m_itemSelTextColor.SetValue(0xffff0000);
}

// 0x43dc30
RFXmlLoader::RFXmlLoader(RFInterface* rfInterface)
	: m_version(0), m_loaded(false), m_interface(rfInterface), m_currentBase(0) {
}

// 0x43de70
RFXmlLoader::~RFXmlLoader() {
	Clear();
}

// 0x43df70
void RFXmlLoader::Clear() {
	m_loaded = false;
	m_structs.clear();
	m_children.clear();
	m_imageSets.clear();
	m_document.Clear();
}

// The original was built with LTCG: it passes `this` and the filename in
// registers.
// 0x43e030
void RFXmlLoader::LoadDocument(std::wstring const& filename, rf::TiXmlDocument* document) {
	void* file = m_interface->FOpen(filename.c_str());
	if (file == 0)
		ThrowFileException(filename, 0);
	m_interface->FSeek(file, 0, SEEK_END);
	int32_t size = m_interface->FTell(file);
	m_interface->FSeek(file, 0, SEEK_SET);
	if (size <= 0) {
		m_interface->FClose(file);
		ThrowFileException(filename, 1);
	}
	std::vector<char> buffer(size + 1);
	m_interface->FRead(file, &buffer[0], size);
	m_interface->FClose(file);
	document->Parse(&buffer[0], 0, rf::TIXML_DEFAULT_ENCODING);
	if (document->Error())
		ThrowException(L"XML parse failed");
}
// An <embed src>: the file under the base path, its root's elements loaded
// under parent
// 0x43e240
void RFXmlLoader::LoadEmbedded(std::wstring const& src, WndBaseStruct* parent) {
	if (!src.empty()) {
		std::wstring path = m_interface->GetBasePath(RF_BASEPATH_ANIMATION) + src;
		rf::TiXmlDocument document;
		LoadDocument(path, &document);
		rf::TiXmlElement* root = document.FirstChildElement("refresh");
		if (root == 0)
			ThrowException(L"Couldn't find root node");
		LoadElement(parent, root, false);
	}
}


// The original was built with LTCG: it inlines TiXmlElement::FirstAttribute
// and TiXmlAttribute::Next.
// 0x43e380
bool RFXmlLoader::Load(std::wstring const& filename) {
	if (filename.empty())
		ThrowInvalidArgException(L"Parameter 'filename' is empty");
	m_loaded = false;
	Clear();
	LoadDocument(filename, &m_document);
	rf::TiXmlElement* root = m_document.FirstChildElement("refresh");
	if (root == 0)
		ThrowException(L"Couldn't find root node");
	for (rf::TiXmlAttribute* attribute = root->FirstAttribute(); attribute != 0;
	     attribute = attribute->Next()) {
		if (strcmp(attribute->Name(), "version") == 0)
			m_version = atoi(attribute->Value());
	}
	LoadProperties(root);
	LoadTextures(root);
	if (m_version < 2) {
		wchar_t message[256];
		wsprintfW(message, L"%s : warning: XML version %d was declared deprecated.\n",
		          filename.c_str(), m_version);
		m_interface->DebugPrint(message);
	}
	if (m_version >= 2) {
		LoadElement(0, root, false);
	} else if (m_version == 1) {
		if (LoadVersion1(root) != R_OK)
			ThrowException(L"XML Version 1 read failed");
	} else {
		ThrowException(L"Unknown XML version");
	}
	m_loaded = true;
	return true;
}

// 0x43e5c0
WndBaseStruct* RFXmlLoader::GetRootStruct() {
	return m_structs[0];
}

// 0x43e5d0
WndBaseStruct* RFXmlLoader::GetChild(WndBaseStruct* parent, int index) {
	ChildMap::iterator it = m_children.find(parent);
	if (it == m_children.end())
		return 0;
	return it->second[index];
}

// 0x43e610
int RFXmlLoader::GetRootStructCount() {
	return m_structs.size();
}

// 0x43e620
int RFXmlLoader::GetChildCount(WndBaseStruct* parent) {
	ChildMap::iterator it = m_children.find(parent);
	if (it == m_children.end())
		return -1;
	return it->second.size();
}

// 0x43e660
R_ERRORTYPE RFXmlLoader::LoadProperties(rf::TiXmlElement* root) {
	rf::TiXmlElement* properties = root->FirstChildElement("properties");
	if (properties != 0) {
		for (rf::TiXmlElement* property = properties->FirstChildElement("property"); property != 0;
		     property = property->NextSiblingElement("property")) {
			if (Utf8ToWString(GetAttribute(property, "name")) == L"UIAnimation")
				m_uiAnimation = Utf8ToWString(GetAttribute(property, "val"));
		}
	}
	return R_OK;
}

// The original inlines TiXmlElement::Attribute (LTCG).
// 0x43e820
R_ERRORTYPE RFXmlLoader::LoadTextures(rf::TiXmlElement* root) {
	rf::TiXmlElement* textures = root->FirstChildElement("Textures");
	if (textures != 0) {
		rf::TiXmlElement* texture = textures->FirstChildElement("Texture");
		while (texture != 0) {
			std::string name(texture->Attribute("Name"));
			std::string type(texture->Attribute("Type"));
			if (type == "Frame")
				LoadFrameTexture(name, texture);
			else if (type == "Single")
				LoadSingleTexture(name, texture);
			else if (type == "NinePatch")
				LoadNinePatchTexture(name, texture);
			texture = texture->NextSiblingElement("Texture");
		}
	}
	return R_OK;
}

// The original inlines TiXmlAttribute::Next and the strcmp calls (LTCG) and
// calls FrameImageSet's constructor out of line (0x43a370). The <Image>
// elements fill the parts in document order 0, 1, 3, 2, 4, 7, 8, 5, 6; the
// path and the tile mode carry over from the previous <Image>.
// 0x43eb30
void RFXmlLoader::LoadFrameTexture(std::string const& name, rf::TiXmlElement* texture) {
	std::wstring path;
	TileModeTypes::Type tileMode = TileModeTypes::Stretch;
	int index = 0;
	FrameImageSet* imageSet = 0;
	rf::TiXmlElement* image = texture->FirstChildElement("Image");
	if (image != 0) {
		imageSet = new FrameImageSet;
		do {
			for (rf::TiXmlAttribute* attribute = image->FirstAttribute(); attribute != 0;
			     attribute = attribute->Next()) {
				if (strcmp(attribute->Name(), "Path") == 0) {
					path = Utf8ToWString(attribute->Value());
				} else if (strcmp(attribute->Name(), "TileMode") == 0) {
					std::string mode(attribute->Value());
					if (mode == "Stretch")
						tileMode = TileModeTypes::Stretch;
					else if (mode == "Tile")
						tileMode = TileModeTypes::Tile;
					else
						tileMode = TileModeTypes::Stretch;
				}
			}
			switch (index) {
			case 0:
				imageSet->m_center.m_fileName = path;
				imageSet->m_center.m_tileMode = tileMode;
				break;
			case 1:
				imageSet->m_topLeft.m_fileName = path;
				imageSet->m_topLeft.m_tileMode = tileMode;
				break;
			case 2:
				imageSet->m_bottomLeft.m_fileName = path;
				imageSet->m_bottomLeft.m_tileMode = tileMode;
				break;
			case 3:
				imageSet->m_topRight.m_fileName = path;
				imageSet->m_topRight.m_tileMode = tileMode;
				break;
			case 4:
				imageSet->m_bottomRight.m_fileName = path;
				imageSet->m_bottomRight.m_tileMode = tileMode;
				break;
			case 5:
				imageSet->m_top.m_fileName = path;
				imageSet->m_top.m_tileMode = tileMode;
				break;
			case 6:
				imageSet->m_bottom.m_fileName = path;
				imageSet->m_bottom.m_tileMode = tileMode;
				break;
			case 7:
				imageSet->m_left.m_fileName = path;
				imageSet->m_left.m_tileMode = tileMode;
				break;
			case 8:
				imageSet->m_right.m_fileName = path;
				imageSet->m_right.m_tileMode = tileMode;
				break;
			}
			++index;
			image = image->NextSiblingElement("Image");
		} while (image != 0);
	}
	if (imageSet != 0)
		AddImageSet(name, imageSet);
}

// The original inlines TiXmlAttribute::Next and the strcmp calls (LTCG).
// 0x43f010
void RFXmlLoader::LoadSingleTexture(std::string const& name, rf::TiXmlElement* texture) {
	rf::TiXmlElement* image = texture->FirstChildElement("Image");
	if (image != 0) {
		SingleImageSet* imageSet = new SingleImageSet;
		for (rf::TiXmlAttribute* attribute = image->FirstAttribute(); attribute != 0;
		     attribute = attribute->Next()) {
			if (strcmp(attribute->Name(), "Path") == 0) {
				imageSet->m_fileName = Utf8ToWString(attribute->Value());
			} else if (strcmp(attribute->Name(), "TileMode") == 0) {
				std::string mode(attribute->Value());
				TileModeTypes::Type tileMode;
				if (mode == "Stretch")
					tileMode = TileModeTypes::Stretch;
				else if (mode == "Tile")
					tileMode = TileModeTypes::Tile;
				else
					tileMode = TileModeTypes::Stretch;
				imageSet->m_tileMode = tileMode;
			}
		}
		AddImageSet(name, imageSet);
	}
}

// The original inlines TiXmlAttribute::Next and the strcmp calls (LTCG); the
// split cuts its "ControlPoint" tail (0x43f5d8).
// 0x43f290
void RFXmlLoader::LoadNinePatchTexture(std::string const& name, rf::TiXmlElement* texture) {
	rf::TiXmlElement* image = texture->FirstChildElement("Image");
	if (image != 0) {
		NinePatchImageSet* imageSet = new NinePatchImageSet;
		for (rf::TiXmlAttribute* attribute = image->FirstAttribute(); attribute != 0;
		     attribute = attribute->Next()) {
			if (strcmp(attribute->Name(), "Path") == 0) {
				imageSet->m_fileName = Utf8ToWString(attribute->Value());
			} else if (strcmp(attribute->Name(), "ControlPoint") == 0) {
				std::string points(attribute->Value());
				if (!points.empty()) {
					std::string delimiter(",");
					std::vector<std::string> values;
					std::string::size_type start = 0;
					std::string::size_type end;
					while ((end = points.find(delimiter, start)) != std::string::npos) {
						values.push_back(points.substr(start, end - start));
						start = end + delimiter.size();
					}
					values.push_back(points.substr(start, end - start));
					imageSet->m_top = static_cast<float>(atof(values[0].c_str()));
					imageSet->m_bottom = static_cast<float>(atof(values[1].c_str()));
					imageSet->m_left = static_cast<float>(atof(values[2].c_str()));
					imageSet->m_right = static_cast<float>(atof(values[3].c_str()));
				}
			}
		}
		AddImageSet(name, imageSet);
	}
}

// 0x43f670
void RFXmlLoader::AddImageSet(std::string const& name, ImageSet* imageSet) {
	if (m_imageSets.find(name) == m_imageSets.end()) {
		m_imageSets.insert(std::pair<std::string, ImageSet*>(name, imageSet));
	} else if (imageSet != 0) {
		delete imageSet;
	}
}

// 0x43f7a0
ImageSet* RFXmlLoader::GetImageSet(std::string const& name) {
	std::map<std::string, ImageSet*>::iterator it = m_imageSets.find(name);
	if (it != m_imageSets.end()) {
		it->second->AddRef();
		return it->second;
	}
	return 0;
}

// The original's split cuts this function in three (0x43f7e0, 0x43f865,
// 0x43f900; the last two are its tail) and inlines the "WND" comparison.
// 0x43f7e0
R_ERRORTYPE RFXmlLoader::LoadVersion1(rf::TiXmlElement* root) {
	rf::TiXmlElement* base = root->FirstChildElement("base");
	if (base == 0)
		return R_NO_BASE_ELEMENT;
	std::string type(base->Attribute("type"));
	if (type == "WND") {
		WndExBase* wnd = new WndExBase;
		ReadWndBase(base, wnd);
		m_structs.push_back(wnd);
		LoadVersion1Children(wnd, root);
	} else if (type == "WNDEX") {
		WndExBase* wnd = new WndExBase;
		ReadWndExBase(base, wnd);
		m_structs.push_back(wnd);
		LoadVersion1Children(wnd, root);
	}
	return R_OK;
}

// The version 1 layout's <child> elements, by their "type" (TEXTAREA and
// TEXTAREASC both build a TextAreaSCBase); R_OK, or
// R_NO_CHILD_ELEMENT without a <child>.
// 0x43f9c0
R_ERRORTYPE RFXmlLoader::LoadVersion1Children(WndBaseStruct* parent, rf::TiXmlElement* root) {
	rf::TiXmlElement* child = root->FirstChildElement("child");
	if (child == 0)
		return R_NO_CHILD_ELEMENT;
	do {
		std::string type(child->Attribute("type"));
		if (type == "LABEL") {
			LabelBase* wnd = new LabelBase;
			ReadLabelBase(child, wnd);
			m_children[parent].push_back(wnd);
		} else if (type == "BUTTON") {
			ButtonBase* wnd = new ButtonBase;
			ReadButtonBase(child, wnd);
			m_children[parent].push_back(wnd);
		} else if (type == "LABELBUTTON") {
			LabelButtonBase* wnd = new LabelButtonBase;
			ReadLabelButtonBase(child, wnd);
			m_children[parent].push_back(wnd);
		} else if (type == "ANIMBUTTON") {
			AnimButtonBase* wnd = new AnimButtonBase;
			ReadAnimButtonBase(child, wnd);
			m_children[parent].push_back(wnd);
		} else if (type == "CHECK") {
			CheckBase* wnd = new CheckBase;
			ReadCheckBase(child, wnd);
			m_children[parent].push_back(wnd);
		} else if (type == "TEXTAREA") {
			TextAreaSCBase* wnd = new TextAreaSCBase;
			ReadTextAreaSCBase(child, wnd);
			m_children[parent].push_back(wnd);
		} else if (type == "TEXTAREASC") {
			TextAreaSCBase* wnd = new TextAreaSCBase;
			ReadTextAreaSCBase(child, wnd);
			m_children[parent].push_back(wnd);
		} else if (type == "EDIT") {
			EditBase* wnd = new EditBase;
			ReadEditBase(child, wnd);
			m_children[parent].push_back(wnd);
		} else if (type == "LIST") {
			ListSCBase* wnd = new ListSCBase;
			ReadListSCBase(child, wnd);
			m_children[parent].push_back(wnd);
		} else if (type == "LISTSC") {
			ListSCBase* wnd = new ListSCBase;
			ReadListSCBase(child, wnd);
			m_children[parent].push_back(wnd);
		} else if (type == "COMBO") {
			ComboBase* wnd = new ComboBase;
			ReadComboBase(child, wnd);
			m_children[parent].push_back(wnd);
		} else if (type == "SCROLLBAR") {
			ScrollbarBase* wnd = new ScrollbarBase;
			ReadScrollbarBase(child, wnd);
			m_children[parent].push_back(wnd);
		} else if (type == "SLIDER") {
			SliderBase* wnd = new SliderBase;
			ReadSliderBase(child, wnd);
			m_children[parent].push_back(wnd);
		} else if (type == "PICTURE") {
			PictureBase* wnd = new PictureBase;
			ReadPictureBase(child, wnd);
			m_children[parent].push_back(wnd);
		} else if (type == "PROGRESS") {
			ProgressBase* wnd = new ProgressBase;
			ReadProgressBase(child, wnd);
			m_children[parent].push_back(wnd);
		} else if (type == "CUSTOM") {
			CustomBase* wnd = new CustomBase;
			ReadCustomBase(child, wnd);
			m_children[parent].push_back(wnd);
		}
		child = child->NextSiblingElement("child");
	} while (child != 0);
	return R_OK;
}

// 0x440290
ScrollbarBase::ScrollbarBase() {
}

// 0x4402c0
SliderBase::SliderBase() {
}

// 0x440330
CustomBase::CustomBase() {
}

// A <child style="file">: the style file under the base path, its textures,
// and its elements under parent; the last one becomes m_currentBase
// (the split cuts the tail at 0x44055b)
// 0x440480
bool RFXmlLoader::LoadStyle(WndBaseStruct* parent, rf::TiXmlElement* element) {
	std::wstring style = Utf8ToWString(GetAttribute(element, "style"));
	if (style.empty())
		return false;
	std::wstring path = m_interface->GetBasePath(RF_BASEPATH_ANIMATION) + style;
	rf::TiXmlDocument document;
	LoadDocument(path, &document);
	rf::TiXmlElement* root = document.FirstChildElement("refresh");
	if (root == 0)
		ThrowException(L"Couldn't find root node");
	LoadTextures(root);
	LoadElement(parent, root, true);
	return true;
}

// After a style moved wnd's rect from oldRect: its children re-anchored, then
// theirs (a label button's label struct when it has no children)
// 0x440640
void RFXmlLoader::ReanchorChildren(RFRect const& oldRect, WndBaseStruct* wnd) {
	int count = GetChildCount(wnd);
	if (count == -1) {
		if (wnd->GetType() == WndType_LabelButton)
			ApplyAnchor(oldRect, static_cast<LabelButtonBase*>(wnd)->m_labelStruct, wnd);
		// the original calls it a second time and discards the result
		wnd->GetType();
		return;
	}
	for (int i = 0; i < count; ++i) {
		WndBaseStruct* child = GetChild(wnd, i);
		ApplyAnchor(oldRect, child, wnd);
		ReanchorChildren(child->m_rect, child);
	}
}

// wnd's anchors against its parent's new rect: anchor bit 4 keeps the right
// margin it had in oldRect (bit 1 stretches the rect, else it moves), bit 8
// the bottom margin (bit 2 stretches). wnd in eax, oldRect in ecx, parent in
// esi (LTCG).
// 0x440730
void RFXmlLoader::ApplyAnchor(RFRect const& oldRect, WndBaseStruct* wnd, WndBaseStruct* parent) {
	uint32_t anchor = wnd->m_anchor.GetValue();
	if (anchor & 4) {
		if (anchor & 1) {
			float margin = oldRect.right - (wnd->m_rect.Width() + wnd->m_pos.x);
			wnd->m_rect.right = parent->m_rect.right - wnd->m_pos.x - margin;
		} else {
			float width = wnd->m_rect.Width();
			float margin = oldRect.right - (width + wnd->m_pos.x);
			wnd->m_pos.x = parent->m_rect.right - (width + margin);
		}
	}
	if (anchor & 8) {
		if (anchor & 2) {
			float margin = oldRect.bottom - (wnd->m_rect.Height() + wnd->m_pos.y);
			wnd->m_rect.bottom = parent->m_rect.bottom - wnd->m_pos.y - margin;
		} else {
			float height = wnd->m_rect.Height();
			float margin = oldRect.bottom - (height + wnd->m_pos.y);
			wnd->m_pos.y = parent->m_rect.bottom - (height + margin);
		}
	}
}

// A styled struct's own element over the style: the WND attributes, the
// control's part, the children re-anchored to the new rect, the <Style>
// setters (Target, Property, Value), then the element's own children.
// this in ecx, wnd in edx (LTCG).
// 0x4407f0
void RFXmlLoader::ApplyStyle(WndBaseStruct* wnd, rf::TiXmlElement* element) {
	RFRect oldRect(wnd->m_rect);
	ReadWndBase(element, wnd);
	switch (wnd->GetType()) {
	case WndType_Label:
		ApplyLabelStyle(element, static_cast<LabelBase*>(wnd));
		break;
	case WndType_Button:
		ApplyButtonStyle(element, static_cast<ButtonBase*>(wnd));
		break;
	case WndType_List:
		ApplyListStyle(element, static_cast<ListBase*>(wnd));
		break;
	case WndType_TextArea:
		ApplyTextAreaStyle(element, static_cast<TextAreaBase*>(wnd));
		break;
	case WndType_TextAreaSC:
		ApplyTextAreaSCStyle(element, static_cast<TextAreaSCBase*>(wnd));
		break;
	case WndType_Edit:
		ApplyEditStyle(element, static_cast<EditBase*>(wnd));
		break;
	default:
		break;
	}
	ReanchorChildren(oldRect, wnd);
	rf::TiXmlElement* style = element->FirstChildElement("Style");
	if (style != 0) {
		rf::TiXmlElement* setter = style->FirstChildElement();
		while (setter != 0) {
			std::wstring target = Utf8ToWString(setter->Attribute("Target"));
			std::wstring property = Utf8ToWString(setter->Attribute("Property"));
			std::wstring value = Utf8ToWString(setter->Attribute("Value"));
			SetStyleText(target, value, wnd);
			setter = setter->NextSiblingElement();
		}
	}
	LoadElement(wnd, element, false);
}

// A <Style> setter's value as the text of the label named target: wnd's own
// label struct when wnd (childless) is a label button of that name, else
// each label child of that name. this on the stack, target in edi (LTCG).
// 0x440b50
void RFXmlLoader::SetStyleText(std::wstring const& target, std::wstring const& value,
                               WndBaseStruct* wnd) {
	int count = GetChildCount(wnd);
	if (count == -1) {
		if (wnd->m_name == target && wnd->GetType() == WndType_LabelButton)
			static_cast<LabelBase*>(static_cast<LabelButtonBase*>(wnd)->m_labelStruct)->m_label = value;
		return;
	}
	for (int i = 0; i < count; ++i) {
		WndBaseStruct* child = GetChild(wnd, i);
		if (child->GetType() == WndType_Label && child->m_name == target)
			static_cast<LabelBase*>(child)->m_label = value;
	}
}

// The original was built with LTCG: it compares the first three types inline
// and the others through an out-of-line std::string::compare (0x447fa0), and
// calls the *Base constructors out of line.
// 0x440cb0
R_ERRORTYPE RFXmlLoader::LoadElement(WndBaseStruct* parent, rf::TiXmlElement* element,
                                     bool setCurrent) {
	rf::TiXmlElement* child = element->FirstChildElement();
	if (child == 0)
		ThrowException(L"Counld't found child element");
	WndBaseStruct* wnd = 0;
	do {
		std::string value(child->Value());
		if (value == "child" || value == "base") {
			std::string type(child->Attribute("type"));
			if (LoadStyle(parent, child)) {
				if (m_currentBase != 0)
					ApplyStyle(m_currentBase, child);
			} else {
				if (type == "WND" || type == "PANEL") {
					WndExBase* wndEx = new WndExBase;
					ReadWndBase(child, wndEx);
					wnd = wndEx;
				} else if (type == "WNDEX") {
					WndExBase* wndEx = new WndExBase;
					ReadWndExBase(child, wndEx);
					wnd = wndEx;
				} else if (type == "LABEL") {
					LabelBase* label = new LabelBase;
					ReadLabelBase(child, label);
					wnd = label;
				} else if (type == "BUTTON") {
					ButtonBase* button = new ButtonBase;
					ReadButtonBase(child, button);
					wnd = button;
				} else if (type == "LABELBUTTON") {
					LabelButtonBase* labelButton = new LabelButtonBase;
					ReadLabelButtonBase(child, labelButton);
					wnd = labelButton;
				} else if (type == "ANIMBUTTON") {
					AnimButtonBase* animButton = new AnimButtonBase;
					ReadAnimButtonBase(child, animButton);
					wnd = animButton;
				} else if (type == "CHECK") {
					CheckBase* check = new CheckBase;
					ReadCheckBase(child, check);
					wnd = check;
				} else if (type == "TEXTAREA") {
					TextAreaSCBase* textArea = new TextAreaSCBase;
					ReadTextAreaSCBase(child, textArea);
					wnd = textArea;
				} else if (type == "TEXTAREASC") {
					TextAreaSCBase* textArea = new TextAreaSCBase;
					ReadTextAreaSCBase(child, textArea);
					wnd = textArea;
				} else if (type == "EDIT") {
					EditBase* edit = new EditBase;
					ReadEditBase(child, edit);
					wnd = edit;
				} else if (type == "LIST") {
					ListSCBase* list = new ListSCBase;
					ReadListSCBase(child, list);
					wnd = list;
				} else if (type == "LISTSC") {
					ListSCBase* list = new ListSCBase;
					ReadListSCBase(child, list);
					wnd = list;
				} else if (type == "COMBO") {
					ComboBase* combo = new ComboBase;
					ReadComboBase(child, combo);
					wnd = combo;
				} else if (type == "SCROLLBAR") {
					ScrollbarBase* scrollbar = new ScrollbarBase;
					ReadScrollbarBase(child, scrollbar);
					wnd = scrollbar;
				} else if (type == "SLIDER") {
					SliderBase* slider = new SliderBase;
					ReadSliderBase(child, slider);
					wnd = slider;
				} else if (type == "STACKPANEL") {
					StackPanelBase* stackPanel = new StackPanelBase;
					ReadStackPanelBase(child, stackPanel);
					wnd = stackPanel;
				} else if (type == "PICTURE") {
					PictureBase* picture = new PictureBase;
					ReadPictureBase(child, picture);
					wnd = picture;
				} else if (type == "PROGRESS") {
					ProgressBase* progress = new ProgressBase;
					ReadProgressBase(child, progress);
					wnd = progress;
				} else if (type == "CUSTOM") {
					CustomBase* custom = new CustomBase;
					ReadCustomBase(child, custom);
					wnd = custom;
				}
				if (wnd != 0) {
					LoadElement(wnd, child, false);
					if (parent == 0)
						m_structs.push_back(wnd);
					else
						m_children[parent].push_back(wnd);
				}
				if (setCurrent)
					m_currentBase = wnd;
			}
		}
		child = child->NextSiblingElement();
	} while (child != 0);
	return R_OK;
}

// The "x" and "y" attributes of the child element name
// 0x441800
static void ReadVector2(rf::TiXmlElement* element, std::string const& name, RFVector2* value) {
	rf::TiXmlElement* child = element->FirstChildElement(name.c_str());
	if (child == 0)
		return;
	for (rf::TiXmlAttribute* attribute = child->FirstAttribute(); attribute != 0;
	     attribute = attribute->Next()) {
		if (strcmp(attribute->Name(), "x") == 0)
			value->x = (float)atof(attribute->Value());
		else if (strcmp(attribute->Name(), "y") == 0)
			value->y = (float)atof(attribute->Value());
	}
}

// The "l", "t", "r" and "b" attributes of the child element name
// 0x4418e0
static void ReadRect(rf::TiXmlElement* element, std::string const& name, RFRect* value) {
	rf::TiXmlElement* child = element->FirstChildElement(name.c_str());
	if (child == 0)
		return;
	for (rf::TiXmlAttribute* attribute = child->FirstAttribute(); attribute != 0;
	     attribute = attribute->Next()) {
		if (strcmp(attribute->Name(), "l") == 0)
			value->left = (float)atof(attribute->Value());
		else if (strcmp(attribute->Name(), "t") == 0)
			value->top = (float)atof(attribute->Value());
		else if (strcmp(attribute->Name(), "r") == 0)
			value->right = (float)atof(attribute->Value());
		else if (strcmp(attribute->Name(), "b") == 0)
			value->bottom = (float)atof(attribute->Value());
	}
}

// 0x441a50
R_ERRORTYPE RFXmlLoader::ReadWndBase(rf::TiXmlElement* element, WndBaseStruct* wnd) {
	wnd->m_name = Utf8ToWString(element->Attribute("name"));
	ReadVector2(element, std::string("pos"), &wnd->m_pos);
	ReadRect(element, std::string("rect"), &wnd->m_rect);
	for (rf::TiXmlElement* sound = element->FirstChildElement("snd"); sound != 0;
	     sound = sound->NextSiblingElement("snd")) {
		rf::TiXmlNode* text = sound->FirstChild();
		if (text != 0) {
			std::string type(sound->Attribute("type"));
			if (type == "SHOW")
				wnd->m_showSound = Utf8ToWString(text->Value());
			else if (type == "HIDE")
				wnd->m_hideSound = Utf8ToWString(text->Value());
		}
	}
	char const* src = "";
	if (element != 0) {
		rf::TiXmlElement* embed = element->FirstChildElement("embed");
		if (embed != 0)
			src = GetAttribute(embed, "src");
	}
	LoadEmbedded(Utf8ToWString(src), wnd);
	for (rf::TiXmlElement* child = element->FirstChildElement(); child != 0;
	     child = child->NextSiblingElement()) {
		std::map<std::string, RFProperty*, NoCaseLess>::iterator it =
			wnd->m_properties.find(std::string(child->Value()));
		RFProperty* property = it != wnd->m_properties.end() ? it->second : 0;
		if (property != 0)
			property->Parse(std::string(child->Attribute("val")));
	}
	return R_OK;
}

// The brush: the <Assets>' "Default" <Asset>'s texture, or a FrameImageSet
// built from the <asset> elements (type BG, EDGE_*, FRAME_*; path, tileMode).
// 0x442070
R_ERRORTYPE RFXmlLoader::ReadWndExBase(rf::TiXmlElement* element, WndExBase* wnd) {
	ReadWndBase(element, wnd);
	rf::TiXmlElement* assets = element->FirstChildElement("Assets");
	if (assets != 0) {
		rf::TiXmlElement* asset = assets->FirstChildElement("Asset");
		while (asset != 0) {
			std::string name(asset->Attribute("Name"));
			if (name == "Default")
				wnd->m_brushDesc = GetImageSet(std::string(asset->Attribute("TextureName")));
			asset = asset->NextSiblingElement("Asset");
		}
		return R_OK;
	}
	FrameImageSet* imageSet = 0;
	rf::TiXmlElement* asset = element->FirstChildElement("asset");
	if (asset != 0) {
		imageSet = new FrameImageSet;
		do {
			std::string type(asset->Attribute("type"));
			std::string mode(GetAttribute(asset, "tileMode", "Stretch"));
			TileModeTypes::Type tileMode;
			if (mode == "Stretch")
				tileMode = TileModeTypes::Stretch;
			else if (mode == "Tile")
				tileMode = TileModeTypes::Tile;
			else
				tileMode = TileModeTypes::Stretch;
			if (type == "BG") {
				imageSet->m_center.m_fileName = Utf8ToWString(asset->Attribute("path"));
				imageSet->m_center.m_tileMode = tileMode;
			} else if (type == "EDGE_LT") {
				imageSet->m_topLeft.m_fileName = Utf8ToWString(asset->Attribute("path"));
				imageSet->m_topLeft.m_tileMode = tileMode;
			} else if (type == "EDGE_RT") {
				imageSet->m_topRight.m_fileName = Utf8ToWString(asset->Attribute("path"));
				imageSet->m_topRight.m_tileMode = tileMode;
			} else if (type == "EDGE_LB") {
				imageSet->m_bottomLeft.m_fileName = Utf8ToWString(asset->Attribute("path"));
				imageSet->m_bottomLeft.m_tileMode = tileMode;
			} else if (type == "EDGE_RB") {
				imageSet->m_bottomRight.m_fileName = Utf8ToWString(asset->Attribute("path"));
				imageSet->m_bottomRight.m_tileMode = tileMode;
			} else if (type == "FRAME_T") {
				imageSet->m_top.m_fileName = Utf8ToWString(asset->Attribute("path"));
				imageSet->m_top.m_tileMode = tileMode;
			} else if (type == "FRAME_B") {
				imageSet->m_bottom.m_fileName = Utf8ToWString(asset->Attribute("path"));
				imageSet->m_bottom.m_tileMode = tileMode;
			} else if (type == "FRAME_L") {
				imageSet->m_left.m_fileName = Utf8ToWString(asset->Attribute("path"));
				imageSet->m_left.m_tileMode = tileMode;
			} else if (type == "FRAME_R") {
				imageSet->m_right.m_fileName = Utf8ToWString(asset->Attribute("path"));
				imageSet->m_right.m_tileMode = tileMode;
			}
			asset = asset->NextSiblingElement("asset");
		} while (asset != 0);
	}
	wnd->m_brushDesc = imageSet;
	return R_OK;
}

// The "val" of the child element name, converted from an int; defaultValue
// when the element, the child or the attribute is missing (inlined by its
// callers)
template <typename T>
static void ReadValue(rf::TiXmlElement* element, char const* name, T* value, T defaultValue) {
	if (element != 0) {
		rf::TiXmlElement* child = element->FirstChildElement(name);
		if (child != 0) {
			char const* text = child->Attribute("val");
			if (text != 0) {
				*value = static_cast<T>(atoi(text));
				return;
			}
		}
	}
	if (value != 0)
		*value = defaultValue;
}

// The attribute of the child element name as a flag (an int, not 0);
// defaultValue when the element, the child or the attribute is missing
// (inlined by its callers)
static void ReadValue(rf::TiXmlElement* element, char const* name, char const* attribute, bool* value,
                      bool defaultValue) {
	if (element != 0) {
		rf::TiXmlElement* child = element->FirstChildElement(name);
		if (child != 0) {
			char const* text = child->Attribute(attribute);
			if (text != 0) {
				*value = atoi(text) != 0;
				return;
			}
		}
	}
	if (value != 0)
		*value = defaultValue;
}

// The WndBaseStruct attributes, the label's text and its style
// (the split cuts the tail at 0x442afb)
// 0x442a50
R_ERRORTYPE RFXmlLoader::ReadLabelBase(rf::TiXmlElement* element, LabelBase* wnd) {
	ReadWndBase(element, wnd);
	wnd->m_label = Utf8ToWString(element->FirstChildElement("label")->Attribute("val"));
	ReadValue(element, "style", &wnd->m_style, 0);
	return R_OK;
}

// A style's label over the label's own: the text and the style when the
// element gives them (the split cuts the tail at 0x442c12)
// 0x442b70
void RFXmlLoader::ApplyLabelStyle(rf::TiXmlElement* element, LabelBase* wnd) {
	rf::TiXmlElement* label = element->FirstChildElement("label");
	if (label != 0)
		wnd->m_label = Utf8ToWString(label->Attribute("val"));
	ReadValue(element, "style", &wnd->m_style, wnd->m_style);
}

// The state images: <Assets>' <Asset> Name/TextureName pairs naming loaded
// textures, or <asset type="NORMAL|DOWN|OVER|DISABLE"> elements whose text
// is a file (one SingleImageSet per state); then the <snd> elements' sounds
// 0x442c90
R_ERRORTYPE RFXmlLoader::ReadButtonBase(rf::TiXmlElement* element, ButtonBase* wnd) {
	ReadWndBase(element, wnd);
	rf::TiXmlElement* assets = element->FirstChildElement("Assets");
	if (assets != 0) {
		rf::TiXmlElement* asset = assets->FirstChildElement("Asset");
		while (asset != 0) {
			std::string name(asset->Attribute("Name"));
			std::string textureName(asset->Attribute("TextureName"));
			if (name == "Normal")
				wnd->m_stateImages[RFButton::RFBUTTON_STATE_NORMAL] = GetImageSet(textureName);
			else if (name == "Push")
				wnd->m_stateImages[RFButton::RFBUTTON_STATE_DOWN] = GetImageSet(textureName);
			else if (name == "Hover")
				wnd->m_stateImages[RFButton::RFBUTTON_STATE_OVER] = GetImageSet(textureName);
			else if (name == "Disable")
				wnd->m_stateImages[RFButton::RFBUTTON_STATE_DISABLE] = GetImageSet(textureName);
			asset = asset->NextSiblingElement("Asset");
		}
	} else {
		SingleImageSet* normal = 0;
		SingleImageSet* down = 0;
		SingleImageSet* over = 0;
		SingleImageSet* disable = 0;
		rf::TiXmlElement* asset = element->FirstChildElement("asset");
		if (asset != 0) {
			normal = new SingleImageSet;
			down = new SingleImageSet;
			over = new SingleImageSet;
			disable = new SingleImageSet;
			do {
				rf::TiXmlNode* path = asset->FirstChild();
				if (path != 0) {
					std::string type(asset->Attribute("type"));
					if (type == "NORMAL")
						normal->m_fileName = Utf8ToWString(path->Value());
					else if (type == "DOWN")
						down->m_fileName = Utf8ToWString(path->Value());
					else if (type == "OVER")
						over->m_fileName = Utf8ToWString(path->Value());
					else if (type == "DISABLE")
						disable->m_fileName = Utf8ToWString(path->Value());
				}
				asset = asset->NextSiblingElement("asset");
			} while (asset != 0);
		}
		wnd->m_stateImages[RFButton::RFBUTTON_STATE_NORMAL] = normal;
		wnd->m_stateImages[RFButton::RFBUTTON_STATE_DOWN] = down;
		wnd->m_stateImages[RFButton::RFBUTTON_STATE_OVER] = over;
		wnd->m_stateImages[RFButton::RFBUTTON_STATE_DISABLE] = disable;
	}
	for (rf::TiXmlElement* sound = element->FirstChildElement("snd"); sound != 0;
	     sound = sound->NextSiblingElement("snd")) {
		rf::TiXmlNode* path = sound->FirstChild();
		if (path != 0) {
			std::string type(sound->Attribute("type"));
			if (type == "OVER")
				wnd->m_overSound = Utf8ToWString(path->Value());
			else if (type == "DOWN")
				wnd->m_pushSound = Utf8ToWString(path->Value());
			else if (type == "RELEASE")
				wnd->m_releaseSound = Utf8ToWString(path->Value());
		}
	}
	return R_OK;
}

// A style's <snd> elements over a button's sounds (OVER, DOWN, RELEASE).
// wnd in ecx (LTCG).
// 0x443710
void RFXmlLoader::ApplyButtonStyle(rf::TiXmlElement* element, ButtonBase* wnd) {
	for (rf::TiXmlElement* sound = element->FirstChildElement("snd"); sound != 0;
	     sound = sound->NextSiblingElement("snd")) {
		rf::TiXmlNode* path = sound->FirstChild();
		if (path != 0) {
			std::string type(sound->Attribute("type"));
			if (type == "OVER")
				wnd->m_overSound = Utf8ToWString(path->Value());
			else if (type == "DOWN")
				wnd->m_pushSound = Utf8ToWString(path->Value());
			else if (type == "RELEASE")
				wnd->m_releaseSound = Utf8ToWString(path->Value());
		}
	}
}

// The button's attributes; no "NormalColor" value turns the state colours
// off; a <control type="LABEL"> is the child label's layout data
// 0x4439e0
R_ERRORTYPE RFXmlLoader::ReadLabelButtonBase(rf::TiXmlElement* element, LabelButtonBase* wnd) {
	ReadButtonBase(element, wnd);
	char const* normalColor = "";
	if (element != 0) {
		rf::TiXmlElement* color = element->FirstChildElement("NormalColor");
		if (color != 0)
			normalColor = GetAttribute(color, "val");
	}
	if (strcmp(normalColor, "") == 0)
		wnd->m_useStateColors = false;
	rf::TiXmlElement* control = element->FirstChildElement("control");
	while (control != 0) {
		std::string type(control->Attribute("type"));
		if (type == "LABEL") {
			LabelBase* label = new LabelBase;
			ReadLabelBase(control, label);
			wnd->m_labelStruct = label;
		}
		control = control->NextSiblingElement("control");
	}
	return R_OK;
}

// The text of an element: its first text child's value (0 without one).
// Inlined by its callers.
static rf::TiXmlText* FirstText(rf::TiXmlElement* element) {
	rf::TiXmlNode* node = element->FirstChild();
	while (node != 0 && node->ToText() == 0)
		node = node->NextSibling();
	if (node == 0)
		return 0;
	return node->ToText();
}

// The animation button: <Assets>' "Group" <Asset> names the animation of
// the normal state; <asset type="ENTER|LEAVE"> are the files of the enter
// and leave animations, and <asset type="GROUP"> makes it one animation
// played from the normal state image (R_FAIL without a file)
// 0x443c70
R_ERRORTYPE RFXmlLoader::ReadAnimButtonBase(rf::TiXmlElement* element, AnimButtonBase* wnd) {
	if (ReadButtonBase(element, wnd) == R_FAIL)
		return R_FAIL;
	wnd->m_singleAnimation = false;
	rf::TiXmlElement* assets = element->FirstChildElement("Assets");
	if (assets != 0) {
		rf::TiXmlElement* asset = assets->FirstChildElement("Asset");
		while (asset != 0) {
			std::string name(asset->Attribute("Name"));
			std::string textureName(asset->Attribute("TextureName"));
			if (name == "Group") {
				wnd->m_stateImages[RFButton::RFBUTTON_STATE_NORMAL] = GetImageSet(textureName);
				wnd->m_singleAnimation = true;
			}
			asset = asset->NextSiblingElement("Asset");
		}
		return R_OK;
	}
	SingleImageSet* enter = 0;
	SingleImageSet* leave = 0;
	rf::TiXmlElement* asset = element->FirstChildElement("asset");
	if (asset != 0) {
		enter = new SingleImageSet;
		leave = new SingleImageSet;
		do {
			std::string type(asset->Attribute("type"));
			if (type == "GROUP") {
				delete enter;
				delete leave;
				if (asset->FirstChild() == 0)
					return R_FAIL;
				static_cast<SingleImageSet*>(wnd->m_stateImages[RFButton::RFBUTTON_STATE_NORMAL])->m_fileName =
					Utf8ToWString(asset->FirstChild()->Value());
				wnd->m_singleAnimation = true;
				return R_OK;
			} else if (type == "ENTER") {
				rf::TiXmlText* text = FirstText(asset);
				if (text != 0)
					enter->m_fileName = Utf8ToWString(text->Value());
			} else if (type == "LEAVE") {
				rf::TiXmlText* text = FirstText(asset);
				if (text != 0)
					leave->m_fileName = Utf8ToWString(text->Value());
			}
			asset = asset->NextSiblingElement("asset");
		} while (asset != 0);
	}
	wnd->m_state4Image = enter;
	wnd->m_state5Image = leave;
	return R_OK;
}

// The check box images: <Assets>' <Asset> "Normal" and "Check" name loaded
// textures, or <asset type="NORMAL|CHECK"> elements whose text is a file
// (one SingleImageSet each)
// 0x4442f0
R_ERRORTYPE RFXmlLoader::ReadCheckBase(rf::TiXmlElement* element, CheckBase* wnd) {
	ReadWndBase(element, wnd);
	rf::TiXmlElement* assets = element->FirstChildElement("Assets");
	if (assets != 0) {
		rf::TiXmlElement* asset = assets->FirstChildElement("Asset");
		while (asset != 0) {
			std::string name(asset->Attribute("Name"));
			std::string textureName(asset->Attribute("TextureName"));
			if (name == "Normal")
				wnd->m_brushDescs[0] = GetImageSet(textureName);
			else if (name == "Check")
				wnd->m_brushDescs[1] = GetImageSet(textureName);
			asset = asset->NextSiblingElement("Asset");
		}
		return R_OK;
	}
	SingleImageSet* normal = 0;
	SingleImageSet* checked = 0;
	rf::TiXmlElement* asset = element->FirstChildElement("asset");
	if (asset != 0) {
		normal = new SingleImageSet;
		checked = new SingleImageSet;
		do {
			char const* type = asset->Attribute("type");
			if (strcmp(type, "NORMAL") == 0)
				normal->m_fileName = Utf8ToWString(asset->FirstChild()->Value());
			else if (strcmp(type, "CHECK") == 0)
				checked->m_fileName = Utf8ToWString(asset->FirstChild()->Value());
			asset = asset->NextSiblingElement("asset");
		} while (asset != 0);
	}
	wnd->m_brushDescs[0] = normal;
	wnd->m_brushDescs[1] = checked;
	return R_OK;
}

// The frame's attributes and the list's: padding, columns, item rect,
// selection mode, deselectable, right click action.
// The item rect must not be empty. The original's asserts sit on lines
// 1572 and 1573 of src\RFXmlLoader.cpp (__LINE__ 0x624, 0x625; the source
// before this function is longer than ours) and spell the expression
// "base->_itemrect.Width() > 0" / "base->_itemrect.Height() > 0": the
// parameter is `base`, the ListBase member `_itemrect`.
// Our lines are kept on the original's: the asserts below are on 1572-1573.
//
// 0x444800
R_ERRORTYPE RFXmlLoader::ReadListBase(rf::TiXmlElement* element, ListBase* base) {
	ReadWndExBase(element, base);
	ReadRect(element, std::string("padding"), &base->m_padding);
	ReadValue(element, "column", &base->m_columnCount, 0);
	ReadRect(element, std::string("itemrect"), &base->_itemrect);
	assert(base->_itemrect.Width() > 0);
	assert(base->_itemrect.Height() > 0);
	ReadValue(element, "selection", &base->m_selectionMode, static_cast<ListSelectionTypes::Type>(0));
	ReadValue(element, "deselectable", &base->m_deselectable, false);
	ReadValue(element, "rclickaction", &base->m_rClickAction, static_cast<RFRCLICKACTION>(0));
	return R_OK;
}

// A style's padding, item rect, columns and selection options over a list's
// own (the current values as the defaults). element in ecx (LTCG).
// 0x444ad0
void RFXmlLoader::ApplyListStyle(rf::TiXmlElement* element, ListBase* wnd) {
	ReadRect(element, std::string("padding"), &wnd->m_padding);
	ReadRect(element, std::string("itemrect"), &wnd->_itemrect);
	ReadValue(element, "column", &wnd->m_columnCount, wnd->m_columnCount);
	ReadValue(element, "selection", &wnd->m_selectionMode, wnd->m_selectionMode);
	ReadValue(element, "deselectable", &wnd->m_deselectable, wnd->m_deselectable);
	ReadValue(element, "rclickaction", &wnd->m_rClickAction, wnd->m_rClickAction);
}

// The list's attributes, then its <control> scroll bars (dropped when
// they do not read)
// 0x444d40
R_ERRORTYPE RFXmlLoader::ReadListSCBase(rf::TiXmlElement* element, ListSCBase* wnd) {
	ReadListBase(element, wnd);
	rf::TiXmlElement* control = element->FirstChildElement("control");
	while (control != 0) {
		std::string type(control->Attribute("type"));
		if (type == "HSCROLLBAR") {
			ScrollbarBase* scrollBar = new ScrollbarBase;
			if (ReadScrollbarBase(control, scrollBar) != R_FAIL)
				wnd->m_hScrollBar = scrollBar;
			else
				delete scrollBar;
		} else if (type == "VSCROLLBAR") {
			ScrollbarBase* scrollBar = new ScrollbarBase;
			if (ReadScrollbarBase(control, scrollBar) != R_FAIL)
				wnd->m_vScrollBar = scrollBar;
			else
				delete scrollBar;
		}
		control = control->NextSiblingElement("control");
	}
	return R_OK;
}

// The window's attributes, the caret's texture (an <Asset Name="Caret">
// of the textures, or the "path" of an <asset type="CARET">), the padding,
// the style and the horizontal alignment
// 0x445050
R_ERRORTYPE RFXmlLoader::ReadTextAreaBase(rf::TiXmlElement* element, TextAreaBase* wnd) {
	ReadWndExBase(element, wnd);
	wnd->m_style = 0;
	rf::TiXmlElement* assets = element->FirstChildElement("Assets");
	if (assets != 0) {
		rf::TiXmlElement* asset = assets->FirstChildElement("Asset");
		while (asset != 0) {
			std::string name(asset->Attribute("Name"));
			std::string textureName(asset->Attribute("TextureName"));
			if (name == "Caret")
				wnd->m_caretBrushDesc = GetImageSet(textureName);
			asset = asset->NextSiblingElement("Asset");
		}
	} else {
		SingleImageSet* caret = 0;
		rf::TiXmlElement* asset = element->FirstChildElement("asset");
		if (asset != 0) {
			caret = new SingleImageSet;
			do {
				std::string type(asset->Attribute("type"));
				if (type == "CARET")
					caret->m_fileName = Utf8ToWString(asset->Attribute("path"));
				asset = asset->NextSiblingElement("asset");
			} while (asset != 0);
		}
		wnd->m_caretBrushDesc = caret;
	}
	ReadRect(element, std::string("padding"), &wnd->m_padding);
	ReadValue(element, "style", &wnd->m_style, 0u);
	ReadValue(element, "halign", &wnd->m_hAlign, static_cast<HorzAlignTypes::Type>(0));
	return R_OK;
}

// A style's padding, style and horizontal alignment over a text area's own
// (the current values as the defaults). element in ecx, wnd in edx (LTCG).
// 0x4455d0
void RFXmlLoader::ApplyTextAreaStyle(rf::TiXmlElement* element, TextAreaBase* wnd) {
	ReadRect(element, std::string("padding"), &wnd->m_padding);
	ReadValue(element, "style", &wnd->m_style, wnd->m_style);
	ReadValue(element, "halign", &wnd->m_hAlign, wnd->m_hAlign);
}

// The text area's attributes, its <control> scroll bars (dropped when they
// do not read), which scroll bars always show or show by themselves, and
// the rectangle of the automatic ones
// 0x445730
R_ERRORTYPE RFXmlLoader::ReadTextAreaSCBase(rf::TiXmlElement* element, TextAreaSCBase* wnd) {
	ReadTextAreaBase(element, wnd);
	rf::TiXmlElement* control = element->FirstChildElement("control");
	while (control != 0) {
		std::string type(control->Attribute("type"));
		if (type == "HSCROLLBAR") {
			ScrollbarBase* scrollBar = new ScrollbarBase;
			if (ReadScrollbarBase(control, scrollBar) != R_FAIL)
				wnd->m_hScrollBar = scrollBar;
			else
				delete scrollBar;
		} else if (type == "VSCROLLBAR") {
			ScrollbarBase* scrollBar = new ScrollbarBase;
			if (ReadScrollbarBase(control, scrollBar) != R_FAIL)
				wnd->m_vScrollBar = scrollBar;
			else
				delete scrollBar;
		}
		control = control->NextSiblingElement("control");
	}
	ReadValue(element, "showalways", "hscroll", &wnd->m_useHScrollBar, false);
	ReadValue(element, "showalways", "vscroll", &wnd->m_enableVScrollBar, false);
	ReadValue(element, "autovisible", "hscroll", &wnd->m_unusedScrollFlag, false);
	ReadValue(element, "autovisible", "vscroll", &wnd->m_autoShowVScrollBar, false);
	ReadRect(element, std::string("autovisiblerect"), &wnd->m_scrollBarRect);
	return R_OK;
}

// A style's scroll bar options over a scrolled text area's own (the current
// values as the defaults). element in ecx (LTCG).
// 0x445bf0
void RFXmlLoader::ApplyTextAreaSCStyle(rf::TiXmlElement* element, TextAreaSCBase* wnd) {
	ReadValue(element, "showalways", "hscroll", &wnd->m_useHScrollBar, wnd->m_useHScrollBar);
	ReadValue(element, "showalways", "vscroll", &wnd->m_enableVScrollBar, wnd->m_enableVScrollBar);
	ReadValue(element, "autovisible", "hscroll", &wnd->m_unusedScrollFlag, wnd->m_unusedScrollFlag);
	ReadValue(element, "autovisible", "vscroll", &wnd->m_autoShowVScrollBar, wnd->m_autoShowVScrollBar);
	ReadRect(element, std::string("autovisiblerect"), &wnd->m_scrollBarRect);
}

// The window's attributes, the caret's texture (as ReadTextAreaBase), the
// padding, the style, the alignments and the password character (the first
// character of its "val")
// 0x445e20
R_ERRORTYPE RFXmlLoader::ReadEditBase(rf::TiXmlElement* element, EditBase* wnd) {
	ReadWndExBase(element, wnd);
	wnd->m_style = 0;
	wnd->m_passwordChar = 0;
	rf::TiXmlElement* assets = element->FirstChildElement("Assets");
	if (assets != 0) {
		rf::TiXmlElement* asset = assets->FirstChildElement("Asset");
		while (asset != 0) {
			std::string name(asset->Attribute("Name"));
			std::string textureName(asset->Attribute("TextureName"));
			if (name == "Caret")
				wnd->m_caretBrushDesc = GetImageSet(textureName);
			asset = asset->NextSiblingElement("Asset");
		}
	} else {
		SingleImageSet* caret = 0;
		rf::TiXmlElement* asset = element->FirstChildElement("asset");
		if (asset != 0) {
			caret = new SingleImageSet;
			do {
				std::string type(asset->Attribute("type"));
				if (type == "CARET")
					caret->m_fileName = Utf8ToWString(asset->Attribute("path"));
				asset = asset->NextSiblingElement("asset");
			} while (asset != 0);
		}
		wnd->m_caretBrushDesc = caret;
	}
	ReadRect(element, std::string("padding"), &wnd->m_padding);
	ReadValue(element, "style", &wnd->m_style, wnd->m_style);
	ReadValue(element, "halign", &wnd->m_hAlign, static_cast<HorzAlignTypes::Type>(0));
	ReadValue(element, "valign", &wnd->m_vAlign, static_cast<VertAlignTypes::Type>(0));
	rf::TiXmlElement* passwordChar = element->FirstChildElement("passwordchar");
	if (passwordChar != 0) {
		std::wstring text = Utf8ToWString(passwordChar->Attribute("val"));
		if (!text.empty())
			wnd->m_passwordChar = text[0];
	}
	return R_OK;
}

// A style's padding, style, alignments and password character over an edit's
// own (the current values as the defaults). element in ecx, wnd in edx (LTCG).
// 0x446470
void RFXmlLoader::ApplyEditStyle(rf::TiXmlElement* element, EditBase* wnd) {
	ReadRect(element, std::string("padding"), &wnd->m_padding);
	ReadValue(element, "style", &wnd->m_style, wnd->m_style);
	ReadValue(element, "halign", &wnd->m_hAlign, wnd->m_hAlign);
	ReadValue(element, "valign", &wnd->m_vAlign, wnd->m_vAlign);
	rf::TiXmlElement* passwordChar = element->FirstChildElement("passwordchar");
	if (passwordChar != 0) {
		std::wstring text = Utf8ToWString(passwordChar->Attribute("val"));
		if (!text.empty())
			wnd->m_passwordChar = text[0];
	}
}

// The window's attributes and the <control> elements of its parts: the
// button, the text area (a TextAreaSCBase) and the list
// 0x446690
R_ERRORTYPE RFXmlLoader::ReadComboBase(rf::TiXmlElement* element, ComboBase* wnd) {
	ReadWndBase(element, wnd);
	rf::TiXmlElement* control = element->FirstChildElement("control");
	while (control != 0) {
		std::string type(control->Attribute("type"));
		if (type == "BUTTON") {
			ButtonBase* button = new ButtonBase;
			ReadButtonBase(control, button);
			wnd->m_button = button;
		} else if (type == "TEXTAREA") {
			TextAreaSCBase* textArea = new TextAreaSCBase;
			ReadTextAreaSCBase(control, textArea);
			wnd->m_textArea = textArea;
		} else if (type == "LISTAREA") {
			ListSCBase* listArea = new ListSCBase;
			ReadListSCBase(control, listArea);
			wnd->m_listArea = listArea;
		}
		control = control->NextSiblingElement("control");
	}
	return R_OK;
}

// The window's attributes, the orientation, the <control> buttons (both
// arrows and the thumb) and the track rect; R_FAIL without a <control>
// 0x4469a0
R_ERRORTYPE RFXmlLoader::ReadScrollbarBase(rf::TiXmlElement* element, ScrollbarBase* wnd) {
	ReadWndBase(element, wnd);
	rf::TiXmlElement* control = element->FirstChildElement("control");
	if (control == 0)
		return R_FAIL;
	wnd->m_horizontal = atoi(element->Attribute("horizontal")) != 0;
	do {
		std::string type(control->Attribute("type"));
		if (type == "BUTTON_LT") {
			ButtonBase* button = new ButtonBase;
			ReadButtonBase(control, button);
			wnd->m_ltButton = button;
		} else if (type == "BUTTON_RB") {
			ButtonBase* button = new ButtonBase;
			ReadButtonBase(control, button);
			wnd->m_rbButton = button;
		} else if (type == "THUMB") {
			ButtonBase* button = new ButtonBase;
			ReadButtonBase(control, button);
			wnd->m_thumb = button;
		}
		control = control->NextSiblingElement("control");
	} while (control != 0);
	ReadRect(element, std::string("scrollrect"), &wnd->m_trackRect);
	return R_OK;
}

// The same layout as a scroll bar's: the window's attributes, the
// orientation, the <control> buttons and the track rect; R_FAIL without a
// <control>
// 0x446ce0
R_ERRORTYPE RFXmlLoader::ReadSliderBase(rf::TiXmlElement* element, SliderBase* wnd) {
	ReadWndBase(element, wnd);
	rf::TiXmlElement* control = element->FirstChildElement("control");
	if (control == 0)
		return R_FAIL;
	wnd->m_horizontal = atoi(element->Attribute("horizontal")) != 0;
	do {
		std::string type(control->Attribute("type"));
		if (type == "BUTTON_LT") {
			ButtonBase* button = new ButtonBase;
			ReadButtonBase(control, button);
			wnd->m_ltButton = button;
		} else if (type == "BUTTON_RB") {
			ButtonBase* button = new ButtonBase;
			ReadButtonBase(control, button);
			wnd->m_rbButton = button;
		} else if (type == "THUMB") {
			ButtonBase* button = new ButtonBase;
			ReadButtonBase(control, button);
			wnd->m_thumb = button;
		}
		control = control->NextSiblingElement("control");
	} while (control != 0);
	ReadRect(element, std::string("sliderect"), &wnd->m_trackRect);
	return R_OK;
}

// The stack panel has no attributes of its own: the WndBaseStruct's
// 0x447020
R_ERRORTYPE RFXmlLoader::ReadStackPanelBase(rf::TiXmlElement* element, StackPanelBase* wnd) {
	ReadWndBase(element, wnd);
	return R_OK;
}

// The picture: <Assets>' <Asset> names a loaded texture (Name "Static" draws
// it, "Animation" plays an animation), or <asset type="STATIC|ANIMATION">
// elements whose text is the file of one SingleImageSet
// 0x447040
R_ERRORTYPE RFXmlLoader::ReadPictureBase(rf::TiXmlElement* element, PictureBase* wnd) {
	ReadWndBase(element, wnd);
	wnd->m_usePicture = true;
	rf::TiXmlElement* assets = element->FirstChildElement("Assets");
	if (assets != 0) {
		rf::TiXmlElement* asset = assets->FirstChildElement("Asset");
		while (asset != 0) {
			std::string name(asset->Attribute("Name"));
			std::string textureName(asset->Attribute("TextureName"));
			wnd->m_brushSource = GetImageSet(textureName);
			if (name == "Static")
				wnd->m_usePicture = true;
			else if (name == "Animation")
				wnd->m_usePicture = false;
			asset = asset->NextSiblingElement("Asset");
		}
	} else {
		SingleImageSet* image = 0;
		rf::TiXmlElement* asset = element->FirstChildElement("asset");
		if (asset != 0) {
			image = new SingleImageSet;
			do {
				std::string type(asset->Attribute("type"));
				if (type == "STATIC") {
					if (asset->FirstChild() != 0) {
						image->m_fileName = Utf8ToWString(asset->FirstChild()->Value());
						wnd->m_usePicture = true;
					}
				} else if (type == "ANIMATION") {
					image->m_fileName = Utf8ToWString(asset->FirstChild()->Value());
					wnd->m_usePicture = false;
				}
				asset = asset->NextSiblingElement("asset");
			} while (asset != 0);
		}
		wnd->m_brushSource = image;
	}
	return R_OK;
}

// The progress bar's image: <Assets>' "Bar" <Asset> names a loaded
// texture, or an <asset type="BAR" tileMode> element whose text is the file
// of one SingleImageSet
// 0x447620
R_ERRORTYPE RFXmlLoader::ReadProgressBase(rf::TiXmlElement* element, ProgressBase* wnd) {
	ReadWndBase(element, wnd);
	rf::TiXmlElement* assets = element->FirstChildElement("Assets");
	if (assets != 0) {
		rf::TiXmlElement* asset = assets->FirstChildElement("Asset");
		while (asset != 0) {
			std::string name(asset->Attribute("Name"));
			std::string textureName(asset->Attribute("TextureName"));
			if (name == "Bar")
				wnd->m_brushDesc = GetImageSet(textureName);
			asset = asset->NextSiblingElement("Asset");
		}
		return R_OK;
	}
	SingleImageSet* image = 0;
	rf::TiXmlElement* asset = element->FirstChildElement("asset");
	if (asset != 0) {
		image = new SingleImageSet;
		do {
			std::string type(asset->Attribute("type"));
			std::string mode(GetAttribute(asset, "tileMode", "Stretch"));
			TileModeTypes::Type tileMode;
			if (mode == "Stretch")
				tileMode = TileModeTypes::Stretch;
			else if (mode == "Tile")
				tileMode = TileModeTypes::Tile;
			else
				tileMode = TileModeTypes::Stretch;
			if (type == "BAR" && asset->FirstChild() != 0) {
				image->m_fileName = Utf8ToWString(asset->FirstChild()->Value());
				image->m_tileMode = tileMode;
			}
			asset = asset->NextSiblingElement("asset");
		} while (asset != 0);
	}
	wnd->m_brushDesc = image;
	return R_OK;
}

// The WndBaseStruct attributes, then the <custom> element's "type"
// 0x447be0
R_ERRORTYPE RFXmlLoader::ReadCustomBase(rf::TiXmlElement* element, CustomBase* wnd) {
	ReadWndBase(element, wnd);
	wnd->m_customType = Utf8ToWString(element->FirstChildElement("custom")->Attribute("type"));
	return R_OK;
}

// The attribute of element as an int; defaultValue when the element or the
// attribute is missing (no caller left: every use is inlined)
// 0x447d20
void ReadAttribute(rf::TiXmlElement* element, char const* attribute, int* value, int defaultValue) {
	if (element != 0) {
		char const* text = element->Attribute(attribute);
		if (text != 0) {
			*value = atoi(text);
			return;
		}
	}
	if (value != 0)
		*value = defaultValue;
}

// The attribute of element as a float; defaultValue when the element or the
// attribute is missing (no caller left: every use is inlined)
// 0x447d60
void ReadAttribute(rf::TiXmlElement* element, char const* attribute, float* value, float defaultValue) {
	if (element != 0) {
		char const* text = element->Attribute(attribute);
		if (text != 0) {
			*value = static_cast<float>(atof(text));
			return;
		}
	}
	if (value != 0)
		*value = defaultValue;
}

// The attribute of the child element name, or "" when the child or the
// attribute is missing (no caller left: every use is inlined)
// 0x447da0
char const* GetChildAttribute(rf::TiXmlElement* element, char const* name, char const* attribute) {
	rf::TiXmlElement* child = element->FirstChildElement(name);
	if (child != 0) {
		char const* text = child->Attribute(attribute);
		if (text != 0)
			return text;
	}
	return "";
}

// The attribute of the child element name as a float; defaultValue when the
// element, the child or the attribute is missing (no caller left: every use
// is inlined)
// 0x447e20
void ReadValue(rf::TiXmlElement* element, char const* name, char const* attribute, float* value,
               float defaultValue) {
	if (element != 0) {
		rf::TiXmlElement* child = element->FirstChildElement(name);
		if (child != 0) {
			char const* text = child->Attribute(attribute);
			if (text != 0) {
				*value = static_cast<float>(atof(text));
				return;
			}
		}
	}
	if (value != 0)
		*value = defaultValue;
}
