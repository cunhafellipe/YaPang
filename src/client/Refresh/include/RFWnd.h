// Original: client\Refresh\include\RFWnd.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFWND_H
#define PG_CLIENT_REFRESH_INCLUDE_RFWND_H

#include <stdint.h>
#include <map>
#include <string>
#include <vector>

#include "RFEventHandler.h"
#include "RFExport.h"
#include "RFProperty.h"
#include "RFRect.h"
#include "RFRenderContext.h"
#include "RFSize.h"
#include "defines.h"
#include "padding.h"

struct FontInfo;
class EventArgs;
class MouseEventArgs;
class KeyEventArgs;
class CharEventArgs;
class UpdateEventArgs;
class RFManager;
class RFRenderer;
class RFInterface;
class RFDirector;
class RFVisualEffect;
class RFWindowModifier;

// The kind of a window (GetType 0x401ea0): what each creation struct's
// GetType returns (WndBaseStruct 0x439cf0, the controls' *Base structs);
// RFManager::GetCompatibleControlName 0x4199f0 asserts it is at most 0x11
// and maps 1 like 0 (an RFWndEx).
enum WndType {
	WndType_Base = 0,
	WndType_WndEx = 1,                 // WndExBase::GetType 0x43aa40
	WndType_Label = 2,
	WndType_Button = 3,
	WndType_LabelButton = 4,
	WndType_AnimButton = 5,
	WndType_Check = 6,
	WndType_Picture = 7,
	WndType_Progress = 8,
	WndType_Scrollbar = 9,
	WndType_Slider = 0xa,
	WndType_Edit = 0xb,
	WndType_List = 0xc,
	WndType_ListSC = 0xd,
	WndType_StackPanel = 0xe,
	WndType_TextArea = 0xf,
	WndType_TextAreaSC = 0x10,
	WndType_Combo = 0x11,
	WndType_Custom = 0x12              // CustomBase::GetType 0x439e80 (the XML <custom> element)
};

// How a window was closed (GetCloseState 0x401fc0, Close 0x408080); the
// values come with Close.
enum RFWNDCLOSESTATE {
};

// Which children ChildWindowFromPoint 0x407420 considers; FindWnd passes 1.
enum RFCWPFlags {
};

// The visual state of a window; the attribute values come with the XML
// loader.
struct VisualStateTypes {
	// RFVisualEffect's Default::Reset moves Opening to Opened and Closing to
	// Closed; PreInitialize starts a window Opened.
	enum Type {
		Closed = 0,
		Opening = 1,
		Opened = 2,
		Closing = 3
	};
};

// What a window is created from (Create 0x404d30 reads it, and keeps the
// pointer in m_createStruct): 0x17c bytes, each control's struct derives from
// it and adds its fields past 0x17c. Polymorphic: Create reads the type
// through vtable slot 1, RFAnimButton::Create 0x434570 dynamic_casts it.
struct WndBaseStruct {
	LAYOUT_CHECK_FRIEND;

	// 0x4394c0 (in the RFXmlLoader object, like the destructor)
	WndBaseStruct();
	// vtable 0xd659e4 (not exported: the struct is not dllexport)
	virtual ~WndBaseStruct();                 // [0]  scalar deleting destructor 0x439df0, ~WndBaseStruct 0x439a80
	// [1] 0x439cf0; Create stores it in m_type
	virtual WndType GetType() const {
		return WndType_Base;
	}
	// Files a property in m_properties under its name (null is ignored).
	// 0x439d00 (this in edi, property in esi: LTCG)
	void AddProperty(RFProperty* property);
	// The property registered under name, or 0. 0x439d80 (this in edi: LTCG)
	RFProperty* GetProperty(std::string const& name);

	std::wstring m_name;                      // 0x4
	RFVector2 m_pos;                          // 0x20 initial and local position
	RFRect m_rect;                            // 0x28 the size is its width and height
	std::wstring m_showSound;                 // 0x38 copied to RFWnd 0x104 (ShowWindow plays it on show)
	std::wstring m_hideSound;                 // 0x54 copied to RFWnd 0x120 (played on hide)
	// The XML attributes, each RFProperty registered in m_properties under
	// its name by the constructor ("Dragable", "IsHitTestVisible",
	// "IsHitTestThrough", "StartPosition", "EscKeyAction", "Anchor",
	// "ClipChildren"); their values sit at +0x20.
	RFBooleanProperty m_dragable;             // 0x70
	RFBooleanProperty m_hitTestVisible;       // 0x94 (the constructor sets it true)
	RFBooleanProperty m_hitTestThrough;       // 0xb8
	RFStartPositionProperty m_startPosition;  // 0xdc
	RFEscKeyActionProperty m_escKeyAction;    // 0x100
	RFAnchorProperty m_anchor;                // 0x124
	RFBooleanProperty m_clipChildren;         // 0x148 also the render context's clip flag (RFWnd 0x15c)
	// The properties by name (AddProperty, GetProperty); the
	// derived structs register theirs here too.
	std::map<std::string, RFProperty*, NoCaseLess> m_properties; // 0x16c
};
CHECK_SIZE(WndBaseStruct, 0x17c);
BEGIN_LAYOUT_CHECK(WndBaseStruct)
	CHECK_FIELD(WndBaseStruct, m_name, 0x4);
	CHECK_FIELD(WndBaseStruct, m_pos, 0x20);
	CHECK_FIELD(WndBaseStruct, m_rect, 0x28);
	CHECK_FIELD(WndBaseStruct, m_showSound, 0x38);
	CHECK_FIELD(WndBaseStruct, m_hideSound, 0x54);
	CHECK_FIELD(WndBaseStruct, m_dragable, 0x70);
	CHECK_FIELD(WndBaseStruct, m_hitTestVisible, 0x94);
	CHECK_FIELD(WndBaseStruct, m_hitTestThrough, 0xb8);
	CHECK_FIELD(WndBaseStruct, m_startPosition, 0xdc);
	CHECK_FIELD(WndBaseStruct, m_escKeyAction, 0x100);
	CHECK_FIELD(WndBaseStruct, m_anchor, 0x124);
	CHECK_FIELD(WndBaseStruct, m_clipChildren, 0x148);
	CHECK_FIELD(WndBaseStruct, m_properties, 0x16c);
END_LAYOUT_CHECK

// The base of every Refresh control: vtable 0xd62fbc, 34 slots, size 0x178
// (from its vector deleting destructor). Derives from RFEventHandler: the
// constructor builds the base first, and the event map is at 0x4.
// Exported: its implicit copy constructor (0x402150) and copy assignment
// (0x4024f0) are exports, so they stay implicit.
class RF_API RFWnd : public RFEventHandler {
	LAYOUT_CHECK_FRIEND;
	friend class RFManager;   // SetResolution writes m_localSize, calls OnLayoutChanged/OnKillFocus
	friend class RFSlider;    // reads another window's m_lButtonDown (no exported accessor)
	friend class RFListItemWnd; // GetPos 0x4221d0 walks m_super directly
	friend class RFStackPanel; // Arrange calls its children's protected ApplyAnchor 0x4051f0 directly
	friend class RFDirector;  // FrameMove 0x431440 calls the protected OnLayoutChanged [14]

public:
	// The class name, the static const at 0xe4691c that GetTypeName returns.
	static std::wstring const TypeName;

	// The event names (0xe46938..0xe46ba0) and the two UI animation names,
	// built by the dynamic initializers at 0xcc7270..0xcc7570.
	static std::wstring const EventClose;              // 0xe46938
	static std::wstring const EventFinalize;           // 0xe46954
	static std::wstring const EventLButtonUp;          // 0xe46970
	static std::wstring const EventLButtonDown;        // 0xe4698c
	static std::wstring const EventLButtonDblClick;    // 0xe469a8
	static std::wstring const EventRButtonUp;          // 0xe469c4
	static std::wstring const EventRButtonDown;        // 0xe469e0
	static std::wstring const EventMouseMove;          // 0xe469fc
	static std::wstring const EventMouseWheel;         // 0xe46a18
	static std::wstring const EventMouseEnter;         // 0xe46a34
	static std::wstring const EventMouseLeave;         // 0xe46a50
	static std::wstring const EventContextMenu;        // 0xe46a6c
	static std::wstring const EventKeyUp;              // 0xe46a88
	static std::wstring const EventKeyDown;            // 0xe46aa4
	static std::wstring const EventChar;               // 0xe46ac0
	static std::wstring const EventActivated;          // 0xe46adc
	static std::wstring const EventDeactivated;        // 0xe46af8
	static std::wstring const EventSetFocus;           // 0xe46b14
	static std::wstring const EventKillFocus;          // 0xe46b30
	static std::wstring const EventUpdate;             // 0xe46b4c
	static std::wstring const EventDraw;               // 0xe46b68
	static std::wstring const EventPlayUIAnimation;    // 0xe46b84
	static std::wstring const EventVisibleChanged;     // 0xe46ba0
	static std::wstring const AnimOpening;             // 0xe46bbc
	static std::wstring const AnimClosing;             // 0xe46bd8
	static uint32_t const AnimMask_Visible;            // 0xd629e0

	// A pair of iterators over the child list, returned by value
	// (GetWindowList 0x4020a0, GetReverseWindowList 0x4020c0): 8 bytes.
	template <class Iterator>
	class IteratorBase {
	public:
		IteratorBase(Iterator first, Iterator last) : m_first(first), m_last(last) {}

		Iterator m_first; // 0x0
		Iterator m_last;  // 0x4
	};

	// [0] 0x401dc0
	virtual wchar_t const* ToString() const {
		return L"RFWnd";
	}
	virtual ~RFWnd();                                         // [1]  scalar deleting destructor 0x4027f0, ~RFWnd 0x404a40
	virtual std::wstring const& GetTypeName();                // [2]  0x404830
	virtual void Initialize();                                // [3]  0x404c20
	virtual void Create(WndBaseStruct*, RFWnd*);              // [4]  0x404d30
	virtual void Destroy();                                   // [5]  0x404e50
	virtual bool HitTest(RFVector2 const&, bool);             // [6]  0x407290
	RFWnd();                                                  //      0x404840
	// 0x401dd0
	std::wstring const& GetName() const {
		return m_name;
	}
	// 0x401de0
	RFVector2 const& GetInitialPos() const {
		return m_initialPos;
	}
	// 0x401df0
	RFSize const& GetLocalSize() const {
		return m_localSize;
	}
	// 0x401e00
	RFSize const& GetInitialSize() const {
		return m_initialSize;
	}
	// 0x401e10
	void SetStartPosition(StartPositionTypes::Type position) {
		m_startPosition = position;
	}
	// 0x401e20
	StartPositionTypes::Type GetStartPosition() const {
		return m_startPosition;
	}
	// 0x401e30
	void SetEscKeyAction(EscKeyActionTypes::Type action) {
		m_escKeyAction = action;
	}
	// 0x401e40
	EscKeyActionTypes::Type GetEscKeyAction() const {
		return m_escKeyAction;
	}
	// 0x401e50
	void SetDragable(bool dragable) {
		m_dragable = dragable;
	}
	// 0x401e60
	bool IsDragable() const {
		return m_dragable;
	}
	// 0x401e70
	void SetAnchor(uint32_t anchor) {
		m_anchor = anchor;
	}
	// 0x401e80
	uint32_t GetAnchor() const {
		return m_anchor;
	}
	// 0x401e90
	RFRect const& GetAnchorValue() const {
		return m_anchorValue;
	}
	// 0x401ea0
	WndType GetType() const {
		return m_type;
	}
	// 0x401eb0
	void SetSuper(RFWnd *super) {
		m_super = super;
	}
	// 0x401ec0
	void SetSubControl(bool subControl) {
		m_subControl = subControl;
	}
	// 0x401ed0
	bool IsSubControl() const {
		return m_subControl;
	}
	// [7] 0x401ee0
	virtual bool IsFocusable() const {
		return false;
	}
	// 0x401ef0
	void SetInheritsAlpha(bool inheritsAlpha) {
		m_inheritsAlpha = inheritsAlpha;
	}
	// 0x401f00
	bool InheritsAlpha() const {
		return m_inheritsAlpha;
	}
	// 0x401f10
	void _ApplyAnchor() {
		ApplyAnchor();
	}
	// 0x401f20
	void ShowWindow_(bool show) {
		if (m_visible != show) {
			m_visualState = (VisualStateTypes::Type)(show ? 2 : 0);
			m_visible = show;
			OnShow(show);
		}
	}
	// 0x401f50
	bool IsModal() const {
		return m_modal;
	}
	// 0x401f60
	void SetInputLock(bool lock) {
		m_inputLocked = lock;
	}
	// 0x401f70
	bool IsInputLocked() const {
		return m_inputLocked;
	}
	// 0x401f80
	bool IsPreDraw() {
		return m_preDraw;
	}
	// 0x401f90
	VisualStateTypes::Type GetVisualState() const {
		return m_visualState;
	}
	// 0x401fa0
	void SetVisualState(VisualStateTypes::Type state) {
		if (m_visualEffect != 0) {
			m_visualState = state;
		}
	}
	// 0x401fc0
	RFWNDCLOSESTATE GetCloseState() const {
		return m_closeState;
	}
	// 0x401fd0
	RFDirector *GetAnimation() {
		return m_animation;
	}
	// 0x401fe0
	RFRenderContext *GetRC() {
		return &m_rc;
	}
	// 0x401ff0
	void ClearUserStrings() {
		m_userStrings.clear();
	}
	// 0x402030
	int32_t GetPriority() const {
		return m_priority;
	}
	// 0x402040
	void SetOverlapped(bool overlapped) {
		m_overlapped = overlapped;
	}
	// 0x402050
	bool IsOverlapped() const {
		return m_overlapped;
	}
	// 0x402060
	void SetClipChildren(bool clipChildren) {
		m_clipChildren = clipChildren;
	}
	// 0x402070
	bool IsClipChildren() const {
		return m_clipChildren;
	}
	RFWnd *FindWnd(float, float);                             //      0x402940
	RFWnd *GetSuper(bool);                                    //      0x402960
	RFWnd *GetChild(uint32_t index);                          //      0x402990 (inline, at the end of the header)
	void DeleteChild(RFWnd*);                                 //      0x404f40
	void DeleteAllChildren();                                 //      0x404fc0
	void EjectChild_(RFWnd*);                                 //      0x405020
	void SetHitTestVisible(bool);                             //      0x4029e0
	bool IsHitTestVisible() const;                            //      0x4029f0
	void SetHitTestThrough(bool);                             //      0x402a00
	bool IsHitTestThrough() const;                            //      0x402a10
	uint32_t GetChildCount() const;                           //      0x402a20
	uint32_t GetAnimationMask() const;                        //      0x402a30
	void RemoveAnimationMask(uint32_t);                       //      0x402a40
	void AddAnimationMask(uint32_t);                          //      0x402a60
	void UpdateLayout();                                      //      0x405140
	RFWnd *GetOverlapped();                                   //      0x4057f0
	RFWnd *GetActiveChild();                                  //      0x405810
	bool IsActive() const;                                    //      0x405880
	RFWnd *GetChild(std::wstring const&);                     //      0x4058a0
	RFWnd const *GetSuper(bool) const;                        //      0x405790
	RFWnd *GetControlSuper();                                 //      0x4057c0
	void RegisterChild(RFWnd*);                               //      0x405960
	void CenterWindow(RFWnd*);                                //      0x4059c0
	void CenterWindow_Horizontal(RFWnd*);                     //      0x405b20
	void CenterWindow_Vertical(RFWnd*);                       //      0x405c20
	RFWnd *GetWindow_(std::wstring const&);                   //      0x405d20
	// 0x405fc0 (inlined into RFListSC's LockScrollbar/UpdateScrollbar)
	void EnableWindow(bool enable) {
		if (m_enabled != enable) {
			m_enabled = enable;
			OnEnabled(enable);
			if (!enable && m_active == true) {
				m_active = false;
			}
		}
	}
	bool IsWindowEnabled(bool) const;                         //      0x405ff0
	void Activate();                                          //      0x406030
	void Deactivate();                                        //      0x406090
	void SetFocus();                                          //      0x4060e0
	void KillFocus();                                         //      0x4060f0
	void SetCapture();                                        //      0x406100
	void ReleaseCapture();                                    //      0x406130
	void AddModifier(RFWindowModifier*);                      //      0x406160
	void RemoveModifier(RFWindowModifier*);                   //      0x4061f0
	RFWindowModifier *GetModifier(std::wstring const&) const; //      0x406240
	void LoadUIAnimation(std::wstring const&);               //      0x4062b0
	void RegisterTo(std::wstring const&, std::wstring const&, Subscriber const&); // 0x406430
	void SetPos(float, float);                                //      0x4064c0
	RFVector2 GetPos() const;                                 //      0x406550
	RFVector2 const& GetLocalPos() const;                     //      0x4065c0
	void SetScreenPos(float, float);                          //      0x4065d0
	RFVector2 GetScreenPos() const;                           //      0x406670
	void SetSize(RFSize const&);                              //      0x406700
	RFSize GetSize() const;                                   //      0x406750
	void SetRect(RFRect const&);                              //      0x4067b0
	RFRect GetRect() const;                                   //      0x406810
	void SetScreenRect(RFRect const&);                        //      0x406860
	RFRect GetScreenRect() const;                             //      0x4068c0
	void Draw(RFRenderContext const&);                        //      0x406960
	void Draw2();                                             //      0x406af0
	void PreDraw(RFRenderContext const&);                     //      0x406c00
	bool IsWindowVisible(bool) const;                         //      0x406d90
	void ShowWindow(bool);                                    //      0x406de0
	void ShowModal();                                         //      0x407070
	void BringChildWndToTop(RFWnd*);                          //      0x407110
	void SetPreDraw(bool);                                    //      0x4071f0
	void SetVisualEffect(RFVisualEffect*);                    //      0x407240
	RFWnd *HitTestChild(RFVector2 const&, bool, bool);        //      0x407340
	RFWnd *ChildWindowFromPoint(float, float, RFCWPFlags);    //      0x407420
	bool IsMouseOver();                                       //      0x407520
	RFManager *GetMgr();                                      //      0x407560
	void SetAlpha(float);                                     //      0x407570
	float GetAlpha(bool) const;                               //      0x4075c0
	void SetUserString(std::wstring const&, std::wstring const&); //  0x408320
	std::wstring const& GetUserString(std::wstring const&) const; // 0x408350
	void Close(RFWNDCLOSESTATE);                              //      0x408080
	void PlayAnimation(std::wstring const&);                  //      0x408150
	void StopAnimation();                                     //      0x4082e0
	bool IsAnimationPlaying() const;                          //      0x408300
	bool IsUserStringDefined(std::wstring const&) const;      //      0x4083d0
	// 0x408450 (inlined into RFComboBox)
	void SetPriority(int32_t priority) {
		m_priority = priority;
		if (m_super != 0) {
			m_super->InvalidatePriority();
		}
	}

protected:
	virtual void LoadResource();                              // [8]  0x4020e0
	// [9] 0x404bc0 (inlined into the overrides, RFCheck::PreInitialize 0x425b50)
	virtual void PreInitialize() {
		m_alpha = 1.0f;
		m_animation = 0;
		m_priorityDirty = false;
		m_lButtonDown = false;
		m_unusedFlag = false;
		m_inputLocked = false;
		m_visible = true;
		m_hidePending = false;
		m_enabled = true;
		m_inheritsAlpha = true;
		m_closeState = static_cast<RFWNDCLOSESTATE>(0);
		m_modal = false;
		m_visualState = VisualStateTypes::Opened;
		m_visualEffect = 0;
		m_priority = 0;
		UpdateAnchorValue();
		m_clipChildren = true;
	}
	virtual void OnInitialUpdate();                           // [10] 0x4020f0
	virtual void OnDestroy();                                 // [11] 0x402100
	virtual void OnEnabled(bool);                             // [12] 0x402110
	virtual void OnShow(bool);                                // [13] 0x407650
	virtual void OnLayoutChanged(uint32_t);                   // [14] 0x407630
	virtual void OnLButtonDown(MouseEventArgs&);              // [15] 0x4076d0
	virtual void OnLButtonUp(MouseEventArgs&);                // [16] 0x407760
	virtual void OnLButtonDblClick(MouseEventArgs&);          // [17] 0x4077d0
	virtual void OnRButtonDown(MouseEventArgs&);              // [18] 0x407810
	virtual void OnRButtonUp(MouseEventArgs&);                // [19] 0x407850
	virtual void OnMouseMove(MouseEventArgs&);                // [20] 0x4078c0
	virtual void OnMouseWheel(MouseEventArgs&);               // [21] 0x407940
	virtual void OnMouseEnter();                              // [22] 0x407980
	virtual void OnMouseLeave();                              // [23] 0x4079f0
	virtual void OnKeyDown(KeyEventArgs&);                    // [24] 0x407a60
	virtual void OnKeyUp(KeyEventArgs&);                      // [25] 0x407aa0
	virtual void OnChar(CharEventArgs&);                      // [26] 0x407ae0
	virtual void OnActivated();                               // [27] 0x407b20
	virtual void OnDeactivated();                             // [28] 0x407b90
	virtual void OnSetFocus();                                // [29] 0x407c00
	virtual void OnKillFocus();                               // [30] 0x407d30
	virtual void OnUpdate(UpdateEventArgs&);                  // [31] 0x407de0
	virtual void OnDraw();                                    // [32] 0x408060
	virtual void OnAnimFinished(EventArgs&);                  // [33] 0x408070
	// 0x402080
	void SetName(std::wstring const& name) {
		m_name = name;
	}
	// 0x4020a0
	IteratorBase<std::vector<RFWnd*>::iterator> GetWindowList() {
		return IteratorBase<std::vector<RFWnd*>::iterator>(_children.begin(), _children.end());
	}
	// 0x4020c0
	IteratorBase<std::vector<RFWnd*>::reverse_iterator> GetReverseWindowList() {
		return IteratorBase<std::vector<RFWnd*>::reverse_iterator>(_children.rbegin(), _children.rend());
	}
	void ValidatePriority();                                  //      0x405080
	// 0x405130
	void InvalidatePriority() {
		m_priorityDirty = true;
	}
	RFRenderer *GetRenderer();                                //      0x4051d0
	RFInterface *GetInterface();                              //      0x4051e0
	void ApplyAnchor();                                       //      0x4051f0
	void UpdateAnchorValue();                                 //      0x4053f0
	RFRect ClientToScreen() const;                            //      0x405470
	RFVector2 ScreenToClient(RFVector2 const&) const;         //      0x405540
	RFVector2 ClientToScreen(RFVector2 const&) const;         //      0x4055b0
	void PlaySfx(std::wstring const&);                        //      0x405620
	void PrintText(FontInfo const&, int32_t, int32_t, std::wstring const&); // 0x405700
	void KeyDownEventHandler(EventArgs&);                     //      0x405760
	RFVector2 GetScreenPos_Impl() const;                      //      0x4066c0

protected:
	WndBaseStruct *m_createStruct;          // 0x30 (Create keeps its argument)
	RFWnd *m_super;                         // 0x34
	bool m_inheritsAlpha;                   // 0x38
	float m_alpha;                          // 0x3c (GetAlpha; PreInitialize sets 1)
	RFRect m_anchorValue;                   // 0x40
	std::vector<RFWindowModifier*> m_modifiers; // 0x50
	bool m_active;                          // 0x60 (IsActive)
	bool m_visible;                         // 0x61
	bool m_hidePending;                     // 0x62 (ShowWindow(false) while a visual effect plays; IsWindowVisible)
	bool m_enabled;                         // 0x63 (EnableWindow, IsWindowEnabled)
	bool m_overlapped;                      // 0x64
	bool m_hitTestVisible;                  // 0x65
	bool m_hitTestThrough;                  // 0x66
	PADDING(0x67, 0x68);
	RFVector2 m_dragOffset;                 // 0x68 (OnLButtonDown keeps the click offset, OnMouseMove subtracts it)
	bool m_lButtonDown;                     // 0x70 (OnLButtonDown sets it)
	bool m_unusedFlag;                      // 0x71 (only PreInitialize resets it; no reader found in RFWnd)
	bool m_inputLocked;                     // 0x72
	PADDING(0x73, 0x74);
	bool m_clipChildren;                    // 0x74
	VisualStateTypes::Type m_visualState;   // 0x78
	RFVisualEffect *m_visualEffect;         // 0x7c
	RFWNDCLOSESTATE m_closeState;           // 0x80
	bool m_modal;                           // 0x84
	// No reader or writer in RFWnd's functions; the ctor leaves it unset and the
	// implicit copy ctor 0x402150 / operator= 0x4024f0 copy it as one dword.
	uint32_t m_reserved88;                  // 0x88
	std::vector<RFWnd*> _children;          // 0x8c (name from OnUpdate's assert text)
	std::vector<RFWnd*> m_closedChildren;   // 0x9c (OnUpdate queues the children whose close finished, then destroys and deletes them)
	bool m_priorityDirty;                   // 0xac (InvalidatePriority)
	std::map<std::wstring, std::wstring> m_userStrings; // 0xb0
	int32_t m_priority;                     // 0xc0
	RFVector2 m_initialPos;                 // 0xc4
	RFSize m_initialSize;                   // 0xcc
	WndType m_type;                         // 0xd4
	std::wstring m_name;                    // 0xd8
	RFVector2 m_localPos;                   // 0xf4
	RFSize m_localSize;                     // 0xfc
	std::wstring m_showSound;               // 0x104 (Create copies WndBaseStruct::m_showSound)
	std::wstring m_hideSound;               // 0x120 (Create copies WndBaseStruct::m_hideSound)
	bool m_dragable;                        // 0x13c
	StartPositionTypes::Type m_startPosition; // 0x140
	EscKeyActionTypes::Type m_escKeyAction; // 0x144
	uint32_t m_anchor;                      // 0x148
	bool m_subControl;                      // 0x14c
	RFDirector *m_animation;                // 0x150
	uint32_t m_animationMask;               // 0x154
	RFRenderContext m_rc;                   // 0x158 (GetRC)
	bool m_preDraw;                         // 0x174
};
CHECK_SIZE(RFWnd, 0x178);
BEGIN_LAYOUT_CHECK(RFWnd)
	CHECK_FIELD(RFWnd, m_createStruct, 0x30);
	CHECK_FIELD(RFWnd, m_super, 0x34);
	CHECK_FIELD(RFWnd, m_inheritsAlpha, 0x38);
	CHECK_FIELD(RFWnd, m_alpha, 0x3c);
	CHECK_FIELD(RFWnd, m_anchorValue, 0x40);
	CHECK_FIELD(RFWnd, m_modifiers, 0x50);
	CHECK_FIELD(RFWnd, m_active, 0x60);
	CHECK_FIELD(RFWnd, m_visible, 0x61);
	CHECK_FIELD(RFWnd, m_hidePending, 0x62);
	CHECK_FIELD(RFWnd, m_enabled, 0x63);
	CHECK_FIELD(RFWnd, m_overlapped, 0x64);
	CHECK_FIELD(RFWnd, m_hitTestVisible, 0x65);
	CHECK_FIELD(RFWnd, m_hitTestThrough, 0x66);
	CHECK_FIELD(RFWnd, m_dragOffset, 0x68);
	CHECK_FIELD(RFWnd, m_lButtonDown, 0x70);
	CHECK_FIELD(RFWnd, m_unusedFlag, 0x71);
	CHECK_FIELD(RFWnd, m_clipChildren, 0x74);
	CHECK_FIELD(RFWnd, m_inputLocked, 0x72);
	CHECK_FIELD(RFWnd, m_visualState, 0x78);
	CHECK_FIELD(RFWnd, m_visualEffect, 0x7c);
	CHECK_FIELD(RFWnd, m_closeState, 0x80);
	CHECK_FIELD(RFWnd, m_modal, 0x84);
	CHECK_FIELD(RFWnd, m_reserved88, 0x88);
	CHECK_FIELD(RFWnd, _children, 0x8c);
	CHECK_FIELD(RFWnd, m_closedChildren, 0x9c);
	CHECK_FIELD(RFWnd, m_priorityDirty, 0xac);
	CHECK_FIELD(RFWnd, m_userStrings, 0xb0);
	CHECK_FIELD(RFWnd, m_priority, 0xc0);
	CHECK_FIELD(RFWnd, m_initialPos, 0xc4);
	CHECK_FIELD(RFWnd, m_initialSize, 0xcc);
	CHECK_FIELD(RFWnd, m_type, 0xd4);
	CHECK_FIELD(RFWnd, m_name, 0xd8);
	CHECK_FIELD(RFWnd, m_localPos, 0xf4);
	CHECK_FIELD(RFWnd, m_localSize, 0xfc);
	CHECK_FIELD(RFWnd, m_showSound, 0x104);
	CHECK_FIELD(RFWnd, m_hideSound, 0x120);
	CHECK_FIELD(RFWnd, m_dragable, 0x13c);
	CHECK_FIELD(RFWnd, m_startPosition, 0x140);
	CHECK_FIELD(RFWnd, m_escKeyAction, 0x144);
	CHECK_FIELD(RFWnd, m_anchor, 0x148);
	CHECK_FIELD(RFWnd, m_subControl, 0x14c);
	CHECK_FIELD(RFWnd, m_animation, 0x150);
	CHECK_FIELD(RFWnd, m_animationMask, 0x154);
	CHECK_FIELD(RFWnd, m_rc, 0x158);
	CHECK_FIELD(RFWnd, m_preDraw, 0x174);
END_LAYOUT_CHECK

// The header inlines defined past the class, at their original line: the
// original's _wassert calls push line 682 (0x2aa) of the upper-case
// E:\...\client\Refresh\include\RFWnd.h for GetChild's assert, in GetChild
// 0x402990 and where ShowWindow 0x406de0 and ChildWindowFromPoint 0x407420
// inline it; it is the header's only assert. The original's text before it
// is longer than this file's: the blank lines stand in for it.
















































// 0x402990
inline RFWnd *RFWnd::GetChild(uint32_t index) {
	if (index >= _children.size()) {
		assert(!"Parameter 'index' out of range.");
		return 0;
	}
	return _children[index];
}

#endif
