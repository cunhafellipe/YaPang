// Original: client\Refresh\include\RFProperty.h (JP 983 exe; the classes have
// no exports, only RTTI and vtables; their code sits in the RFXmlLoader object
// 0x438d70-0x439490, their text parsers (slot 3) with RFManager's code
// 0x464f70-0x465240, asserting src\RFManager.cpp. The file name is assumed.)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFPROPERTY_H
#define PG_CLIENT_REFRESH_INCLUDE_RFPROPERTY_H

#include <stdint.h>
#include <string.h>
#include <string>

#include "RFRect.h"
#include "RFTypes.h"
#include "defines.h"

// The ordering of the property maps (WndBaseStruct::m_properties): shorter
// names first, names of one length by _stricmp. 0x439db0 (its callers pass
// the operands in registers: LTCG).
struct NoCaseLess {
	bool operator()(std::string const& lhs, std::string const& rhs) const {
		if (lhs.size() == rhs.size()) {
			return _stricmp(lhs.c_str(), rhs.c_str()) < 0;
		}
		return lhs.size() < rhs.size();
	}
};

// A named value of a window description read from the XML layouts
// (WndBaseStruct and the controls' structs hold one per attribute, 0x24
// apart). Vtable 0xd658e0, 4 slots, size 0x20. Not exported, so its scalar
// deleting destructor is ??_G.
class RFProperty {
	LAYOUT_CHECK_FRIEND;

public:
	RFProperty();
	virtual ~RFProperty();                             // [0]  scalar deleting destructor 0x438dd0, ~RFProperty 0x438d40
	virtual std::string GetName() const;               // [1]  0x438d70
	virtual void SetName(std::string const& name);     // [2]  0x438da0
	virtual void Parse(std::string const& text);       // [3]  0x438dc0 (empty), the derived classes' parsers

protected:
	std::string m_name;                                // 0x4
};
CHECK_SIZE(RFProperty, 0x20);
BEGIN_LAYOUT_CHECK(RFProperty)
	CHECK_FIELD(RFProperty, m_name, 0x4);
END_LAYOUT_CHECK

// "True" / "False". Vtable 0xd65908, size 0x24.
class RFBooleanProperty : public RFProperty {
	LAYOUT_CHECK_FRIEND;

public:
	RFBooleanProperty();
	void SetValue(bool value);                         //      0x438e40
	virtual void Parse(std::string const& text);       // [3]  0x464ff0 (asserts "Invalid Boolean string.")
	bool GetValue() const { return m_value; }  // ours: no exported getter in the exe (no VA, no copy); RFWnd::Create 0x404d30 reads m_value inline

protected:
	bool m_value;                                      // 0x20
};
CHECK_SIZE(RFBooleanProperty, 0x24);
BEGIN_LAYOUT_CHECK(RFBooleanProperty)
	CHECK_FIELD(RFBooleanProperty, m_value, 0x20);
END_LAYOUT_CHECK

// atoi of the text. Vtable 0xd6591c, size 0x24.
class RFIntegerProperty : public RFProperty {
	LAYOUT_CHECK_FRIEND;

public:
	RFIntegerProperty();
	virtual void Parse(std::string const& text);       // [3]  0x465070
	int GetValue() const { return m_value; }  // ours: no exported getter in the exe (no VA, no copy); RFWnd::Create 0x404d30 reads m_value inline

protected:
	int m_value;                                       // 0x20
};
CHECK_SIZE(RFIntegerProperty, 0x24);
BEGIN_LAYOUT_CHECK(RFIntegerProperty)
	CHECK_FIELD(RFIntegerProperty, m_value, 0x20);
END_LAYOUT_CHECK

// "Left|Top|Right|Bottom" / "None" flags, parsed by 0x419f30 (asserts
// "Invalid Anchor string."); the WndBaseStruct anchor. Vtable 0xd65958, size 0x24.
class RFAnchorProperty : public RFProperty {
	LAYOUT_CHECK_FRIEND;

public:
	RFAnchorProperty();
	// 0x439190 or 0x4391a0 (two identical dword setters between
	// RFColorProperty's constructor 0x439170 and its destructor 0x4391b0;
	// which one is whose is not proven. this in eax, value in ecx: LTCG).
	void SetValue(uint32_t value);
	virtual void Parse(std::string const& text);       // [3]  0x465140
	uint32_t GetValue() const { return m_value; }  // ours: no exported getter in the exe (no VA, no copy); RFWnd::Create 0x404d30 reads m_value inline

protected:
	uint32_t m_value;                                  // 0x20
};
CHECK_SIZE(RFAnchorProperty, 0x24);
BEGIN_LAYOUT_CHECK(RFAnchorProperty)
	CHECK_FIELD(RFAnchorProperty, m_value, 0x20);
END_LAYOUT_CHECK

// A "#rrggbb" / "#aarrggbb" colour, parsed by 0x419db0 (asserts
// "string.size() == 7 || string.size() == 9"). Vtable 0xd6596c, size 0x24.
class RFColorProperty : public RFProperty {
	LAYOUT_CHECK_FRIEND;

public:
	RFColorProperty();
	void SetValue(uint32_t value);                     //      0x439190 or 0x4391a0 (see RFAnchorProperty)
	virtual void Parse(std::string const& text);       // [3]  0x465160
	uint32_t GetValue() const { return m_value; }  // ours: no exported getter in the exe (no VA, no copy); RFWnd::Create 0x404d30 reads m_value inline

protected:
	uint32_t m_value;                                  // 0x20 ARGB
};
CHECK_SIZE(RFColorProperty, 0x24);
BEGIN_LAYOUT_CHECK(RFColorProperty)
	CHECK_FIELD(RFColorProperty, m_value, 0x20);
END_LAYOUT_CHECK

// A text attribute: Parse widens the UTF-8/ANSI text (0x464f40) into the
// value. Vtable 0xd658f4, size 0x3c.
class RFStringProperty : public RFProperty {
	LAYOUT_CHECK_FRIEND;

public:
	RFStringProperty();
	virtual void Parse(std::string const& text);       // [3]  0x464f70
	std::wstring const& GetValue() const { return m_value; }  // ours: no exported getter in the exe (no VA, no copy); RFWnd::Create 0x404d30 reads m_value inline

protected:
	std::wstring m_value;                              // 0x20
};
CHECK_SIZE(RFStringProperty, 0x3c);
BEGIN_LAYOUT_CHECK(RFStringProperty)
	CHECK_FIELD(RFStringProperty, m_value, 0x20);
END_LAYOUT_CHECK

// The StartPositionTypes attribute. Vtable 0xd65930, size 0x24.
class RFStartPositionProperty : public RFProperty {
	LAYOUT_CHECK_FRIEND;

public:
	RFStartPositionProperty();
	virtual void Parse(std::string const& text);       // [3]  0x4650a0
	StartPositionTypes::Type GetValue() const { return m_value; }  // ours: no exported getter in the exe (no VA, no copy); RFWnd::Create 0x404d30 reads m_value inline

protected:
	StartPositionTypes::Type m_value;                  // 0x20
};
CHECK_SIZE(RFStartPositionProperty, 0x24);
BEGIN_LAYOUT_CHECK(RFStartPositionProperty)
	CHECK_FIELD(RFStartPositionProperty, m_value, 0x20);
END_LAYOUT_CHECK

// The EscKeyActionTypes attribute. Vtable 0xd65944, size 0x24.
class RFEscKeyActionProperty : public RFProperty {
	LAYOUT_CHECK_FRIEND;

public:
	RFEscKeyActionProperty();
	virtual void Parse(std::string const& text);       // [3]  0x465110
	EscKeyActionTypes::Type GetValue() const { return m_value; }  // ours: no exported getter in the exe (no VA, no copy); RFWnd::Create 0x404d30 reads m_value inline

protected:
	EscKeyActionTypes::Type m_value;                   // 0x20
};
CHECK_SIZE(RFEscKeyActionProperty, 0x24);
BEGIN_LAYOUT_CHECK(RFEscKeyActionProperty)
	CHECK_FIELD(RFEscKeyActionProperty, m_value, 0x20);
END_LAYOUT_CHECK

// The HorzAlignTypes attribute. Vtable 0xd65980, size 0x24.
class RFHAlignProperty : public RFProperty {
	LAYOUT_CHECK_FRIEND;

public:
	RFHAlignProperty();
	virtual void Parse(std::string const& text);       // [3]  0x465180
	HorzAlignTypes::Type GetValue() const { return m_value; }  // ours: no exported getter in the exe (no VA, no copy); RFWnd::Create 0x404d30 reads m_value inline

protected:
	HorzAlignTypes::Type m_value;                      // 0x20
};
CHECK_SIZE(RFHAlignProperty, 0x24);
BEGIN_LAYOUT_CHECK(RFHAlignProperty)
	CHECK_FIELD(RFHAlignProperty, m_value, 0x20);
END_LAYOUT_CHECK

// The VertAlignTypes attribute. Vtable 0xd65994, size 0x24.
class RFVAlignProperty : public RFProperty {
	LAYOUT_CHECK_FRIEND;

public:
	RFVAlignProperty();
	virtual void Parse(std::string const& text);       // [3]  0x4651a0
	VertAlignTypes::Type GetValue() const { return m_value; }  // ours: no exported getter in the exe (no VA, no copy); RFWnd::Create 0x404d30 reads m_value inline

protected:
	VertAlignTypes::Type m_value;                      // 0x20
};
CHECK_SIZE(RFVAlignProperty, 0x24);
BEGIN_LAYOUT_CHECK(RFVAlignProperty)
	CHECK_FIELD(RFVAlignProperty, m_value, 0x20);
END_LAYOUT_CHECK

// The TextTrimmingTypes attribute. Vtable 0xd659a8, size 0x24.
class RFTextTrimmingProperty : public RFProperty {
	LAYOUT_CHECK_FRIEND;

public:
	RFTextTrimmingProperty();
	// 0x439390: every caller passes 0, folded into the body (LTCG); this in eax.
	void SetValue(TextTrimmingTypes::Type value);
	virtual void Parse(std::string const& text);       // [3]  0x4651c0
	TextTrimmingTypes::Type GetValue() const { return m_value; }  // ours: no exported getter in the exe (no VA, no copy); RFWnd::Create 0x404d30 reads m_value inline

protected:
	TextTrimmingTypes::Type m_value;                   // 0x20
};
CHECK_SIZE(RFTextTrimmingProperty, 0x24);
BEGIN_LAYOUT_CHECK(RFTextTrimmingProperty)
	CHECK_FIELD(RFTextTrimmingProperty, m_value, 0x20);
END_LAYOUT_CHECK

// A rectangle; slot 3 is the base's empty Parse. Vtable 0xd659d0, size 0x30.
class RFRectProperty : public RFProperty {
	LAYOUT_CHECK_FRIEND;

public:
	RFRectProperty();
	RFRect const& GetValue() const { return m_value; }  // ours: no exported getter in the exe (no VA, no copy); RFWnd::Create 0x404d30 reads m_value inline

protected:
	RFRect m_value;                                    // 0x20
};
CHECK_SIZE(RFRectProperty, 0x30);
BEGIN_LAYOUT_CHECK(RFRectProperty)
	CHECK_FIELD(RFRectProperty, m_value, 0x20);
END_LAYOUT_CHECK

// The OrientationTypes attribute (RFList). Vtable 0xd659bc, size 0x24.
class RFOrientationProperty : public RFProperty {
	LAYOUT_CHECK_FRIEND;

public:
	RFOrientationProperty();
	virtual void Parse(std::string const& text);       // [3]  0x4651e0
	OrientationTypes::Type GetValue() const { return m_value; }  // ours: no exported getter in the exe (no VA, no copy); RFWnd::Create 0x404d30 reads m_value inline

protected:
	OrientationTypes::Type m_value;                    // 0x20
};
CHECK_SIZE(RFOrientationProperty, 0x24);
BEGIN_LAYOUT_CHECK(RFOrientationProperty)
	CHECK_FIELD(RFOrientationProperty, m_value, 0x20);
END_LAYOUT_CHECK

#endif // PG_CLIENT_REFRESH_INCLUDE_RFPROPERTY_H
