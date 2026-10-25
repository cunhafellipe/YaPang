// Original: client\Refresh\src\RFXmlLoader.h (JP 983 exe; private to the
// loader, the file name is assumed from src\RFXmlLoader.cpp's asserts)
#ifndef PG_CLIENT_REFRESH_SRC_RFXMLLOADER_H
#define PG_CLIENT_REFRESH_SRC_RFXMLLOADER_H

#include <exception>
#include <map>
#include <string>
#include <vector>

#include "defines.h"
#include "RFEventHandler.h"
#include "RFStringUtil.h"
#include "RFWnd.h"
#include "tinyxml/tinyxml.h"

class ImageSet;
class RFInterface;
struct AnimButtonBase;
struct ButtonBase;
struct CheckBase;
struct ComboBase;
struct EditBase;
struct LabelBase;
struct LabelButtonBase;
struct ListBase;
struct ListSCBase;
struct PictureBase;
struct ProgressBase;
struct ScrollbarBase;
struct SliderBase;
struct StackPanelBase;
struct TextAreaBase;
struct TextAreaSCBase;
struct WndBaseStruct;
struct WndExBase;

// The layout data of a "CUSTOM" child (RTTI ".?AUCustomBase@@", vtable
// 0xd659f0: [0] the implicit scalar deleting destructor 0x4403c0): the
// window's WndBaseStruct and the <custom> element's "type" (reader 0x447be0).
// No control reads it.
struct CustomBase : public WndBaseStruct {
	CustomBase();                            // 0x440330

	// [1] 0x439e80
	virtual WndType GetType() const { return WndType_Custom; }

	std::wstring m_customType;               // 0x17c the <custom> element's "type"
};

#if defined(_MSC_VER)
CHECK_SIZE(CustomBase, 0x198);
CHECK_FIELD(CustomBase, m_customType, 0x17c);
#endif

// The loader's exceptions (RTTI Testing::*), std::exception with the message
// narrowed; vtables 2 slots: [0] scalar deleting destructor, [1] the CRT's what.
// MSVC's std::exception has a `char const* const&` constructor that copies the
// message; libc++'s has none, so in the port each class keeps the message
// itself (m_what) and overrides what().
namespace Testing {

// Vtable 0xd65ad0.
class RFException : public std::exception {
public:
	explicit RFException(std::wstring const& message);

#if !defined(_MSC_VER)
public:
	virtual char const* what() const throw() { return m_what.c_str(); }

private:
	std::string m_what;
#endif
};

// Vtable 0xd65adc.
class RFInvalidArgException : public std::exception {
public:
	explicit RFInvalidArgException(std::wstring const& message);

#if !defined(_MSC_VER)
public:
	virtual char const* what() const throw() { return m_what.c_str(); }

private:
	std::string m_what;
#endif
};

// Vtable 0xd65ae8, size 0x10.
class RFFileException : public std::exception {
public:
	RFFileException(std::wstring const& message, int error);

private:
	int m_error;   // 0xc the second constructor argument
#if !defined(_MSC_VER)

public:
	virtual char const* what() const throw() { return m_what.c_str(); }

private:
	std::string m_what;
#endif
};

} // namespace Testing

// MSVC's std::exception (vptr, message, owns flag) is 0xc; the port's differs.
#if defined(_MSC_VER)
CHECK_SIZE(Testing::RFFileException, 0x10);
#endif

// The layout loader (not exported: RTTI only, vtable 0xd659fc, 1 slot): reads
// a "refresh" XML document into the controls' creation structs. Built on the
// stack by RFManager::CreateWnd 0x417a90 with the manager's RFInterface.
class RFXmlLoader {
	LAYOUT_CHECK_FRIEND;

public:
	RFXmlLoader(RFInterface* rfInterface);   //      0x43dc30
	virtual ~RFXmlLoader();                  // [0]  scalar deleting destructor 0x43dd80, ~RFXmlLoader 0x43de70

	void Clear();                                                  // 0x43df70
	// Reads the document (0x43e030; "XML parse failed" when it does not parse)
	void LoadDocument(std::wstring const& filename, rf::TiXmlDocument* document);
	bool Load(std::wstring const& filename);                       // 0x43e380
	WndBaseStruct* GetRootStruct();                                // 0x43e5c0
	WndBaseStruct* GetChild(WndBaseStruct* parent, int index);     // 0x43e5d0
	int GetRootStructCount();                                      // 0x43e610
	int GetChildCount(WndBaseStruct* parent);                      // 0x43e620 (-1: no entry)
	R_ERRORTYPE LoadProperties(rf::TiXmlElement* root);   // 0x43e660 (R_OK) <properties>/<property name val>, "UIAnimation"
	R_ERRORTYPE LoadTextures(rf::TiXmlElement* root);     // 0x43e820 <Textures>/<Texture Name Type> (R_OK)
	// A <Texture>'s <Image> elements, by its Type, each added with AddImageSet
	void LoadFrameTexture(std::string const& name, rf::TiXmlElement* texture);      // 0x43eb30 FrameImageSet (0x128)
	void LoadSingleTexture(std::string const& name, rf::TiXmlElement* texture);     // 0x43f010 (0x28)
	void LoadNinePatchTexture(std::string const& name, rf::TiXmlElement* texture);  // 0x43f290 (0x34)
	void AddImageSet(std::string const& name, ImageSet* imageSet); // 0x43f670
	// 0x416ee0 (kept in RFManager's code: CreateWnd 0x417a90 calls it with the
	// loader in edx and the result slot in esi, LTCG)
	std::wstring const GetUIAnimation() const { return m_uiAnimation; }
	ImageSet* GetImageSet(std::string const& name);               // 0x43f7a0 (AddRefs it; 0 when unknown)
	// 0x43f7e0 the version 1 layout: R_OK, or R_NO_BASE_ELEMENT without a <base>
	R_ERRORTYPE LoadVersion1(rf::TiXmlElement* root);
	// 0x43f9c0 the version 1 layout's controls under the <base> struct (R_OK, R_NO_CHILD_ELEMENT without a <child>)
	R_ERRORTYPE LoadVersion1Children(WndBaseStruct* parent, rf::TiXmlElement* root);
	R_ERRORTYPE ReadWndBase(rf::TiXmlElement* element, WndBaseStruct* wnd);   // 0x441a50 a "WND" <base>'s attributes (R_OK)
	R_ERRORTYPE ReadWndExBase(rf::TiXmlElement* element, WndExBase* wnd);     // 0x442070 (R_OK) a WndExBase's ("WNDEX")
	// 0x440480 a <child style> file's elements under parent (the last one becomes m_currentBase);
	// false without a "style"
	bool LoadStyle(WndBaseStruct* parent, rf::TiXmlElement* element);
	// 0x440640 wnd's children re-anchored after its rect changed from oldRect (recursive)
	void ReanchorChildren(RFRect const& oldRect, WndBaseStruct* wnd);
	// 0x440730 wnd re-anchored to parent's new rect (its anchor flags; oldRect the parent's old one)
	static void ApplyAnchor(RFRect const& oldRect, WndBaseStruct* wnd, WndBaseStruct* parent);
	// 0x4407f0 the element's own attributes over a styled struct
	void ApplyStyle(WndBaseStruct* wnd, rf::TiXmlElement* element);
	// 0x440b50 a <Style> setter: value as the text of wnd's labels named target
	void SetStyleText(std::wstring const& target, std::wstring const& value, WndBaseStruct* wnd);
	// 0x440cb0 the <child>/<base> elements under element: a struct per "type", its children,
	// added under parent (m_structs at the top); setCurrent keeps the last one in m_currentBase
	R_ERRORTYPE LoadElement(WndBaseStruct* parent, rf::TiXmlElement* element, bool setCurrent);
	void LoadEmbedded(std::wstring const& src, WndBaseStruct* parent);   // 0x43e240 an <embed src> file's elements
	// The per-control readers LoadVersion1Children and LoadElement call
	R_ERRORTYPE ReadLabelBase(rf::TiXmlElement* element, LabelBase* wnd);   // 0x442a50
	R_ERRORTYPE ReadButtonBase(rf::TiXmlElement* element, ButtonBase* wnd);   // 0x442c90
	R_ERRORTYPE ReadLabelButtonBase(rf::TiXmlElement* element, LabelButtonBase* wnd);   // 0x4439e0
	R_ERRORTYPE ReadAnimButtonBase(rf::TiXmlElement* element, AnimButtonBase* wnd);   // 0x443c70
	R_ERRORTYPE ReadCheckBase(rf::TiXmlElement* element, CheckBase* wnd);   // 0x4442f0
	R_ERRORTYPE ReadTextAreaBase(rf::TiXmlElement* element, TextAreaBase* wnd);   // 0x445050
	// 0x445730 the text area's attributes, its two <control> scroll bars and the scroll bar options
	R_ERRORTYPE ReadTextAreaSCBase(rf::TiXmlElement* element, TextAreaSCBase* wnd);
	R_ERRORTYPE ReadEditBase(rf::TiXmlElement* element, EditBase* wnd);   // 0x445e20
	R_ERRORTYPE ReadListBase(rf::TiXmlElement* element, ListBase* wnd);   // 0x444800
	// 0x444d40 the list's attributes and its two <control> scroll bars (LIST, LISTSC, a combo's LISTAREA)
	R_ERRORTYPE ReadListSCBase(rf::TiXmlElement* element, ListSCBase* wnd);
	R_ERRORTYPE ReadComboBase(rf::TiXmlElement* element, ComboBase* wnd);   // 0x446690
	R_ERRORTYPE ReadScrollbarBase(rf::TiXmlElement* element, ScrollbarBase* wnd);   // 0x4469a0
	R_ERRORTYPE ReadSliderBase(rf::TiXmlElement* element, SliderBase* wnd);   // 0x446ce0
	R_ERRORTYPE ReadStackPanelBase(rf::TiXmlElement* element, StackPanelBase* wnd);   // 0x447020 (R_OK)
	R_ERRORTYPE ReadPictureBase(rf::TiXmlElement* element, PictureBase* wnd);   // 0x447040
	R_ERRORTYPE ReadProgressBase(rf::TiXmlElement* element, ProgressBase* wnd);   // 0x447620
	R_ERRORTYPE ReadCustomBase(rf::TiXmlElement* element, CustomBase* wnd);   // 0x447be0 (R_OK)
	// The per-control parts of ApplyStyle (0x4407f0): a style's values over the struct's own
	void ApplyLabelStyle(rf::TiXmlElement* element, LabelBase* wnd);   // 0x442b70
	void ApplyButtonStyle(rf::TiXmlElement* element, ButtonBase* wnd);   // 0x443710
	void ApplyListStyle(rf::TiXmlElement* element, ListBase* wnd);   // 0x444ad0
	void ApplyTextAreaStyle(rf::TiXmlElement* element, TextAreaBase* wnd);   // 0x4455d0
	void ApplyTextAreaSCStyle(rf::TiXmlElement* element, TextAreaSCBase* wnd);   // 0x445bf0
	void ApplyEditStyle(rf::TiXmlElement* element, EditBase* wnd);   // 0x446470

private:
	typedef std::map<WndBaseStruct*, std::vector<WndBaseStruct*> > ChildMap;

	unsigned int m_version;            // 0x4 the root's "version" (below 2 deprecated; 1 = version 1 layout)
	bool m_loaded;                     // 0x8 set when a load succeeded, reset by Clear
	rf::TiXmlDocument m_document;      // 0xc
	RFInterface* m_interface;          // 0x54 DebugPrint receives the warnings
	std::vector<WndBaseStruct*> m_structs;   // 0x58 the top-level structs; CreateWnd reads the first
	// 0x68 each parsed struct's children, by parent (0x440640, 0x440b50);
	// VS 2010's std::map is 0x10 here (head 0x6c, size 0x70)
	ChildMap m_children;
	std::wstring m_uiAnimation;        // 0x78 the "UIAnimation" element's "val" (0x43e660)
	WndBaseStruct* m_currentBase;      // 0x94 stored by the element parser 0x440cb0
	std::map<std::string, ImageSet*> m_imageSets;  // 0x98 the "Image" elements by name (0x43f670)
};

#if defined(_MSC_VER)
CHECK_SIZE(RFXmlLoader, 0xa8);
BEGIN_LAYOUT_CHECK(RFXmlLoader)
	CHECK_FIELD(RFXmlLoader, m_version, 0x4);
	CHECK_FIELD(RFXmlLoader, m_loaded, 0x8);
	CHECK_FIELD(RFXmlLoader, m_document, 0xc);
	CHECK_FIELD(RFXmlLoader, m_interface, 0x54);
	CHECK_FIELD(RFXmlLoader, m_structs, 0x58);
	CHECK_FIELD(RFXmlLoader, m_children, 0x68);
	CHECK_FIELD(RFXmlLoader, m_uiAnimation, 0x78);
	CHECK_FIELD(RFXmlLoader, m_currentBase, 0x94);
	CHECK_FIELD(RFXmlLoader, m_imageSets, 0x98);
END_LAYOUT_CHECK
#endif

#endif // PG_CLIENT_REFRESH_SRC_RFXMLLOADER_H
