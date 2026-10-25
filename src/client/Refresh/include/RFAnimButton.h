// Original: client\Refresh\include\RFAnimButton.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFANIMBUTTON_H
#define PG_CLIENT_REFRESH_INCLUDE_RFANIMBUTTON_H

#include <string>

#include "defines.h"
#include "padding.h"
#include "RFBrush.h"
#include "RFButton.h"
#include "RFEventHandler.h"
#include "RFExport.h"
#include "RFWindowFactory.h"

class RFDirector;

// The layout data of an animation button (RTTI ".?AUAnimButtonBase@@",
// vtable 0xd65a4c), read by RFAnimButton::Create 0x434570 through a
// dynamic_cast: a button's data followed by the two extra state images and
// the single-animation flag.
struct AnimButtonBase : public ButtonBase {
	AnimButtonBase();                       // 0x43c420 (RFXmlLoader.cpp)
	// [0] 0x43c440 (RFXmlLoader.cpp): releases the two extra state images
	virtual ~AnimButtonBase();

	// [1] 0x43c510 (the animation button's WndType)
	virtual WndType GetType() const { return WndType_AnimButton; }

	ImageSet *m_state4Image;                // 0x204
	ImageSet *m_state5Image;                // 0x208
	bool m_singleAnimation;                 // 0x20c
};
CHECK_SIZE(AnimButtonBase, 0x210);  // RFXmlLoader::LoadElement 0x440cb0 allocates 0x210
CHECK_FIELD(AnimButtonBase, m_state4Image, 0x204);
CHECK_FIELD(AnimButtonBase, m_state5Image, 0x208);
CHECK_FIELD(AnimButtonBase, m_singleAnimation, 0x20c);

// A Refresh button that plays an animation: RFButton with the animation
// virtuals (vtable 0xd637dc, 39 slots), size 0x208 (from the vector
// deleting destructor 0x414a30). Each state plays the scene GetAnimName
// names in the director.
class RF_API RFAnimButton : public RFButton {
	LAYOUT_CHECK_FRIEND;

public:
	// The class name, the static const at 0xe46f74 that GetTypeName returns.
	static std::wstring const TypeName;

	RFAnimButton();                                           // 0x4344f0

	// The header inlines (their copies sit in the COMDAT run of the implicit
	// copy constructor, operator= and ??_E, 0x414910-0x414a90).
	virtual wchar_t const* ToString() const { return L"RFAnimButton"; }  // [0]  0x414910
	virtual ~RFAnimButton();                                  // [1]  vector deleting destructor 0x414a30, ~RFAnimButton 0x434560
	virtual std::wstring const& GetTypeName() { return TypeName; }    // [2]  0x414920
	virtual void Create(WndBaseStruct*, RFWnd*);              // [4]  0x434570

protected:
	virtual void LoadResource();                              // [8]  0x4346a0
	virtual void PreInitialize();                             // [9]  0x4345d0
	virtual void OnDestroy();                                 // [11] 0x434650

private:
	virtual void OnEnabled(bool);                             // [12] 0x434f50

protected:
	virtual void OnLButtonDown(MouseEventArgs&);              // [15] 0x4349f0
	virtual void OnLButtonUp(MouseEventArgs&);                // [16] 0x434aa0
	virtual void OnMouseEnter();                              // [22] 0x434b60
	virtual void OnMouseLeave();                              // [23] 0x434c60
	virtual void OnUpdate(UpdateEventArgs&);                  // [31] 0x434d10
	virtual void OnDraw();                                    // [32] 0x434db0
	virtual void OnStateChanged();                            // [34] 0x4349a0

public:
	virtual void LoadAnimation(std::wstring const&);          // [35] 0x434770
	virtual void UnloadAnimation();                           // [36] 0x434840
	virtual R_ERRORTYPE SetAnimation(RFDirector*);            // [37] 0x434870

private:
	virtual void LoadAnimation_(std::wstring const&);         // [38] 0x434f30

public:

	RFDirector* GetDirector() { return m_director; }          // 0x414930
	void SetAnimPos(float, float);                            // 0x4348e0

protected:
	// The animation (scene) name of a state (0-5), an empty string above.
	std::wstring const& GetAnimName(int) const;               // 0x434910

protected:
	RFDirector* m_director;       // 0x1f4 (GetDirector 0x414930; PreInitialize creates it)
	int m_pendingState;           // 0x1f8 (a state whose animation OnUpdate plays when the current one ends)
	// Two more state images after RFButton's four (m_stateImages, 0x1e4);
	// LoadResource loads the animation named by each image (+8), Create
	// copies them from the creation struct's +0x204/+0x208.
	ImageSet* m_state4Image;      // 0x1fc
	ImageSet* m_state5Image;      // 0x200
	bool m_singleAnimation;       // 0x204 (LoadResource loads only the first resource's animation)
};
CHECK_SIZE(RFAnimButton, 0x208);
BEGIN_LAYOUT_CHECK(RFAnimButton)
	CHECK_FIELD(RFAnimButton, m_director, 0x1f4);
	CHECK_FIELD(RFAnimButton, m_pendingState, 0x1f8);
	CHECK_FIELD(RFAnimButton, m_state4Image, 0x1fc);
	CHECK_FIELD(RFAnimButton, m_state5Image, 0x200);
	CHECK_FIELD(RFAnimButton, m_singleAnimation, 0x204);
END_LAYOUT_CHECK

// The window factory of RFAnimButton (RTTI name RFAnimButtonFactory).
class RFAnimButtonFactory : public RFWindowFactory {
public:
	RFAnimButtonFactory(); // 0x432660 (RFControlFactories.cpp)
	// [0] scalar deleting destructor 0x432740 (implicit destructor)
	virtual RFWnd* Create();                                  // [1]  0x4326b0
	virtual void Destroy(RFWnd*);                             // [2]  0x432720
};

#endif
