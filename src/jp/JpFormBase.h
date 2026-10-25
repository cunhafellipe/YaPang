// JP-only game code (no KR counterpart): FrFormBase, the JP notice form
// base. A concrete form (the "notify_ok2" notices are plain FrFormBase,
// CreateForm<FrFormBase> 0x5476b0) and the base of FrGrandPrixAwardDlg
// (ctor 0x54dc90, RTTI 0xe75130). It keeps its named controls ("ok",
// "cancel", "cancel2", "message", "image", "caption") in a name map, lays
// them out around the screen centre, draws a list of images into "image",
// and can carry a sub-form whose init and per-frame step it forwards.
//
// JP x86 facts: RTTI 0xe74d94 "FrFormBase" (static init 0xccb690, base
// FrForm 0xf1aabc), registered with the JP form factory (0x4ff020: make
// instance 0x4fefb0, 0xa4c0d0 / 0xa4bcf0; static init 0xccb6b0), size
// 0x150, vtables 0xd05dbc (IObject/FrWnd, 42 slots), 0xd05dac
// (FrCmdTarget), 0xd05da4; message map 0xe08570 (rows 0xe08578, filled by
// the static init 0xccb6d0, base FrForm 0xe4343c). The JP FrForm has 38
// slots (one more than KR's 37, before slot 25): JP slot n >= 25 is KR
// slot n - 1. The port keeps the KR FrForm and its std:: containers, so
// the JP offsets are comments (the checks are inert on LP64).
#ifndef PG_JP_JPFORMBASE_H
#define PG_JP_JPFORMBASE_H

#include <map>
#include <string>
#include <vector>

#include <windows.h>

#include "defines.h"
#include "frcmdtarget.h"
#include "frform.h"
#include "padding.h"
#include "RefreshSharedPtr.h"
#include "wtypes.h"

class Bitmap;
class FrEdit;
class FrWnd;
class FrWndManager;

class FrFormBase : public FrForm {
	LAYOUT_CHECK_FRIEND;
public:
	// One image "image" draws (0x4fd050): its bitmap at an offset from the
	// image area's centred origin. Allocated by AddImage (0x10 bytes, the
	// constructor writes only bitmap and flag) and held by an
	// RF_SHARED_PTR (its control block 0xd05d74, built by 0x4fbf60).
	struct sImage {
		sImage(void) : bitmap(0), flag(false) {}
		const Bitmap* bitmap;   // 0x0 (its +4 holds the size: +4 width, +8 height)
		float x;                // 0x4
		float y;                // 0x8
		bool flag;              // 0xc (false from the constructor; FrFormBase does not read it)
	};

	static const WRTTI m_RTTI;                                        // 0xe74d94
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }   // [0] 0x4fdc50

	FrFormBase(void);                                                 // 0x4fe910
	virtual ~FrFormBase(void);                                        // [1] 0x4fea60 (scalar deleting), 0x4fdb60

	// JP overrides that only forward to FrForm's slot.
	virtual bool Close(bool bDestroy);                                // [2] 0x4fdc90
	virtual bool Open(FRESH_RESULT_CALLBACK pfnResult, DWORD flag);   // [25] 0x4fb600 (KR FrForm: unsigned long, mirrored to the port as unsigned int)
	// The point overload stays FrForm's (not overridden).
	using FrForm::Open;
	virtual bool Close(eFormRet ret, bool bDestroy);                  // [26] 0x4fdca0
	virtual void SetMessage(const char* message, bool b);             // [29] 0x4fb690
	virtual bool OnInit(void);                                        // [32] 0x4fd380

	// The control the message map registered under a name, NULL for none.
	FrWnd* FindControl(std::string name);                             // 0x4fc2b0
	// The same lookup for the "message" edit control.
	FrEdit* FindEdit(std::string name);                               // 0x4fc410
	// Moves the named control to (x, y), keeping its size; false when the
	// form has no such control.
	bool MoveControl(const std::string& name, float x, float y);      // 0x4fcfb0
	// Shows / hides, enables or fades out the named control of form (false
	// for a NULL form or no such control). Callers 0x549a90 / 0x549c20.
	static bool SetControlVisible(FrFormBase* form, std::string name, bool bVisible);   // 0x4fc7b0
	static bool SetControlEnable(FrFormBase* form, std::string name, bool bEnable);     // 0x4fc870
	static bool SetControlFadeOut(FrFormBase* form, std::string name, bool bFadeout);   // 0x4fc930
	// Sets the named control's tool tip; false when there is no such control.
	bool SetControlToolTip(std::string name, std::string text);       // 0x4fc9f0
	// Adds bitmap to the images "image" draws, after the previous ones
	// (10 apart), growing the image area; returns the new image, an empty
	// pointer for a NULL bitmap. Caller 0x549d80.
	RF_SHARED_PTR<sImage> AddImage(const Bitmap* bitmap);              // 0x4fea90
	// 0x4fd3e0 / 0x4fd4f0 (no callers): MoveControl to a position read from
	// a JP Lua table (0x49a4e0 child table, 0x49a000 / 0x470f20 numbers).
	// The JP Lua binding is not decompiled: not declared yet.

	// New virtuals (JP slots 38-41).
	// Sets the sub-form (a NULL keeps the current one) and returns it.
	virtual FrFormBase* SetSubForm(FrFormBase* pSubForm);             // [38] 0x4fb670
	virtual void ClearSubForm(void);                                  // [39] 0x4fdcb0
	// Whether the form may initialise / open (FrFormBase: always).
	virtual bool IsReady(void);                                       // [40] 0x4fdcc0
	// Draws m_images into the "image" control (its owner-draw).
	virtual void DrawImages(void);                                    // [41] 0x4fd050

protected:
	virtual void OnDraw(void);                                        // [8] 0x4fdc60
	virtual void OnProc(const float dt);                              // [9] 0x4fb640
	virtual void OnOK(void);                                          // [33] 0x4fdc70
	virtual void OnCancel(void);                                      // [34] 0x4fdc80

	// Reads the screen size (COption's resolution, capped by g_view's
	// width / height, 800 x 600 without a view) into m_screenWidth / m_screenHeight, unless
	// the current task is a CGolfTask (JP RTTI 0xe78f3c).
	void UpdateScreenSize(void);                                      // 0x4fba30
	// Centres a width x height rect on the screen (FrForm::SetRect, JP
	// slot 35) and keeps flag in m_centred.
	void CenterOnScreen(float width, float height, bool flag);        // 0x4fbd50
	// Places "cancel2", "caption", "image", "message", re-centres the form
	// on their height, then places "ok" and "cancel" along its bottom.
	void LayoutControls(void);                                        // 0x4fcb50
	// The message map's INIT handler of every named control: a FrWnd param
	// is added under its name (JP FrWnd +0x64, 0xbe75d0) by AddControl<T>
	// for its JP GUI type (FrWnd +0xc8): 1 FrForm, 2 FrStatic,
	// 3 FrTextButton, 4 FrEdit, 5 FrComboBox, 6 FrComboCtlEx, 7 FrButton,
	// 8 FrFrame, 11 FrArea, 12 FrListBox, 13 FrGaugeBar, 14 FrGaugeBarEx,
	// 15 FrGaugeBarImage, 16 FrViewer, 17 FrWnd, 18 FrTabButton,
	// 19 FrGroupBox (9, 10: none). Needs a JP_PAKS build (FrWnd's
	// GetWndName / GetGuiType, set by FrWndManager::DoCreate).
	void OnControlInit(PG_MSGARG param);                              // 0x4fedd0
	// Adds the control under name when the name is free and pObject is a
	// T; returns it, NULL otherwise. One instance per OnControlInit case.
	template <class T>
	T* AddControl(const std::string& name, IObject* pObject);         // 0x4fdd60..0x4fe860
	void OnImageOwnerDraw(void);                                      // 0x4fb8b0

private:
	static sFRESH_ENTRY _MsgEntries[];

protected:
	static sFRESH_MSGMAP _MsgMap;
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;          // FrCmdTarget [1] 0x4fb5f0

	std::map<std::string, FrWnd*> m_controls;   // 0x110 (x86: head 0x114, size 0x118)
	UNKNOWN(0x11c, 0x120);
	std::vector<RF_SHARED_PTR<sImage> > m_images;   // 0x120 (x86: 0x10, 8-byte elements)
	float m_imageWidth;                         // 0x130 (the images' total width, 10 after each; DrawImages centres on it)
	float m_imageHeight;                        // 0x134 (the tallest image: the "image" area's height when images exist)
	float m_nextImageX;                         // 0x138 (x of the next image AddImage places)
	float m_screenWidth;                        // 0x13c
	float m_screenHeight;                       // 0x140
	FrFormBase* m_pSubForm;                     // 0x144
	bool m_centred;                             // 0x148 (CenterOnScreen's flag)
	UNKNOWN(0x149, 0x150);
};
// size 0x150 (x86)
CHECK_SIZE(FrFormBase, 0x150);
BEGIN_LAYOUT_CHECK(FrFormBase)
	CHECK_FIELD(FrFormBase, m_controls, 0x110);
	CHECK_FIELD(FrFormBase, m_images, 0x120);
	CHECK_FIELD(FrFormBase, m_imageWidth, 0x130);
	CHECK_FIELD(FrFormBase, m_screenWidth, 0x13c);
	CHECK_FIELD(FrFormBase, m_pSubForm, 0x144);
	CHECK_FIELD(FrFormBase, m_centred, 0x148);
END_LAYOUT_CHECK

// 0x4fefb0: the JP form factory's make instance for "FrFormBase".
IObject* FrFormBaseMakeInstance(void);
// 0x5476b0: CreateForm<FrFormBase> (the "notify_ok2" notices).
FrFormBase* CreateFormBase(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent);

#endif // PG_JP_JPFORMBASE_H
