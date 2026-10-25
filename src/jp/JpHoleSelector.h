// JP-only game code (no KR counterpart): the game room's hole picker,
// layout "hole_selector". A list box ("holeIds") of the holes 1-18, drawn
// as buttons with the hole number; the area "hole_preview" shows the
// preview picture of the hole under the cursor (or of the selected one).
// The previews come from two sheets per course, "<course name>_1.png"
// (holes 1-10) and "<course name>_2.png" (holes 11-19), cut in cells of
// 140 x 93 (CalculateUVPos). Clicking a hole stores it and closes the
// form (FrOK); closing never destroys it (Close override).
//
// Openers (both through the factory 0x5227d0 = CreateForm<FrHoleSelector>(
// g_pFresh's manager, owner = the opener's FrCmdTarget, "hole_selector", 0)):
// - FrSingleTypeDlg 0x522bf0 (layout "gameroom_single"), when its +0x2ac
//   window exists: [30](0), SetCourse(JP CSharedDoc +0x10950, the room's
//   hole 0x633580), then Open(0x522960, pt, 0x89) at (x - 68.0, y + h + 3.0)
//   of that window. The result callback 0x522960 (on FrOK) sends packet 0xa
//   (0xffff, 1, 0xb, hole) when GetHole() differs from 0x633580's hole.
// - 0xacb470 (a JP room dialog, its +0x148 window): the same with
//   SetCourse(+0x1c4, +0x1d8), the point (x - 126.0, y + h + 3.0) and the
//   result callback 0xacace0.
//
// JP x86 facts: RTTI m_RTTI 0xe74e2c named "FrHoleSelector" (static init
// 0xccc380, base FrForm), factory registration static init 0xccc3a0 (make
// instance 0x5107a0), size 0x150 (`push 150h` before the constructor call
// in 0x5107a0 and 0x5227d0), base the JP FrForm (ctor 0xbe5e20, dtor
// 0xbe5e80, size 0x110). Vtables 0xd06bfc (IObject/FrWnd, 38 slots;
// overrides [0] GetRTTI, [1] the deleting destructor 0x510360, [11]
// OnMouseMove 0x510930, [26] Close(ret, destroy) 0x510190; [6] 0x47ccd0,
// [10] 0x47ccf0, [14]-[19] 0x47cd00-0x47cd50, [21] 0x47cd60 and [31]
// 0x47cd70 are the shared copies of the FrWnd/FrForm header inlines),
// 0xd06bf0 (FrCmdTarget: [0] 0x5101c0 destructor thunk, [1] GetMessageMap
// 0x5100c0), 0xd06be8 ([0] 0x5101b0 destructor thunk). Message map 0xe09340
// (base 0xe4343c, rows 0xe09350, handlers stored by the static init
// 0xccc3c0). The port keeps the KR FrForm layout, so the JP offsets are
// comments only (no CHECK_FIELD).
#ifndef PG_JP_JPHOLESELECTOR_H
#define PG_JP_JPHOLESELECTOR_H

#include "defines.h"
#include "frcmdtarget.h"
#include "frform.h"
#include "wtypes.h"

class Bitmap;
class FrArea;
class FrListBox;
class FrWnd;
class FrWndManager;
class WOverlay;

class FrHoleSelector : public FrForm {
public:
	static const WRTTI m_RTTI;                                        // 0xe74e2c
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x510180

	FrHoleSelector(void);                                             // 0x5102a0
	virtual ~FrHoleSelector(void);                                    // [1] 0x510360 (deleting), 0x5100d0

	virtual void OnMouseMove(const WPoint& pt);                       // [11] 0x510930
	using FrForm::Close;
	virtual bool Close(eFormRet ret, bool bDestroy);                  // [26] 0x510190

	// Stores the hole (19 at most), loads the course's two preview sheets
	// once, selects the hole's row and shows its preview.
	void SetCourse(unsigned char course, unsigned char hole);         // 0x510830
	// The hole the player picked (JP +0x148; the result callbacks read it).
	unsigned char GetHole(void) const { return m_hole; }

private:
	static sFRESH_ENTRY _MsgEntries[];

protected:
	static sFRESH_MSGMAP _MsgMap;
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // FrCmdTarget [1] 0x5100c0

	void OnPreviewInit(PG_MSGARG param);                              // 0x510390
	void OnPreviewDraw(PG_MSGARG param);                              // 0x510400
	void OnHoleListInit(PG_MSGARG param);                             // 0x5104d0
	void OnHoleListDraw(PG_MSGARG param);                             // 0x5105a0
	void OnHoleListClick(void);                                       // 0x510980
	void OnHoleListHoverOff(PG_MSGARG param);                         // 0x5109f0

	// Picks the preview sheet and cell of a hole (1-10: sheet 0, 11-19:
	// sheet 1); with no sheet, the whole texture (0, 0, 1, 1).
	void ShowPreview(unsigned char hole);                             // 0x5106f0

private:
	// The constructor clears the control and sheet pointers; the "INIT"
	// handlers fill the two control pointers when the layout loads.
	FrListBox* m_pHoleList;                // JP +0x110 ("holeIds")
	FrArea* m_pPreviewArea;                // JP +0x114 ("hole_preview")
	const Bitmap* m_pButtonNormalImg;      // JP +0x118 ("single_btn_hole_Normal")
	const Bitmap* m_pButtonHoverImg;       // JP +0x11c ("single_btn_hole_Hover")
	const Bitmap* m_pButtonSelectImg;      // JP +0x120 ("single_btn_hole_Select")
	WOverlay* m_pPreview;                  // JP +0x124 (the sheet ShowPreview picked)
	WOverlay* m_pPreviewSheets[2];         // JP +0x128, +0x12c (released by the destructor)
	WRect m_previewUV;                     // JP +0x130 (the hole's cell in m_pPreview)
	WPoint m_textOffset;                   // JP +0x140 (the hole number's place in a row)
	unsigned char m_hole;                  // JP +0x148 (the constructor sets 19)
	// JP +0x149 .. +0x150: padding.
};

// 0x5227d0: CreateForm<FrHoleSelector>(manager, owner, name, parent), which
// also stores the form in a JP thread-local slot (+0x70).
FrForm* CreateHoleSelector(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent);

#endif // PG_JP_JPHOLESELECTOR_H
