// JP-only game code (no KR counterpart): the lobby's single practice mode
// start form, layout "single_mode_start" (CLobbyMain opens it with
// OnSingleModeStartDlgResult). The player picks the course, repeat or chip-in
// practice (kind 0, 4, 6; the form closes FrYES) or the tutorial (the lobby
// actor gets message 0x63, the kind stays 0xff); "expl" flips the two
// course/chip-in pictures between their images and their explanations.
//
// JP x86 facts: RTTI 0xe74f64 named "FrSingleModeStarter" (static init
// 0xccd8c0, base FrForm 0xf1aabc), factory registration 0x521eb0 (make
// instance 0x521e40), size 0x170, vtables 0xd0815c (IObject/FrWnd), 0xd08150
// (FrCmdTarget), 0xd08148; message map 0xe0a788 (rows 0xe0a790, filled by
// the static init 0xccd900, base 0xe4343c). The port keeps the KR FrForm
// layout, so the JP offsets are comments only (no CHECK_FIELD).
#ifndef PG_JP_JPSINGLEMODESTARTDLG_H
#define PG_JP_JPSINGLEMODESTARTDLG_H

#include "defines.h"
#include "frcmdtarget.h"
#include "frform.h"
#include "wtypes.h"

class Bitmap;
class FrArea;
class FrButton;

class CSingleModeStartDlg : public FrForm {
public:
	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }   // [0] 0x5218b0

	CSingleModeStartDlg(void);
	virtual ~CSingleModeStartDlg(void);   // [1] 0x5218f0 (scalar deleting)

	// The practice kind the player picked: 0 course, 4 repeat, 6 chip-in;
	// 0xff (the constructor's) when the form closed another way.
	unsigned char GetKind(void) const { return m_kind; }

private:
	static sFRESH_ENTRY _MsgEntries[];

protected:
	static sFRESH_MSGMAP _MsgMap;
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;   // FrCmdTarget [1] 0x521710

	void OnClose(void);
	void OnCourseClick(void);
	void OnRepeatClick(void);
	void OnChipinClick(void);
	void OnExplClick(void);
	void OnExplInit(PG_MSGARG param);
	void OnCourseImgInit(PG_MSGARG param);
	void OnChipinImgInit(PG_MSGARG param);
	void OnCourseBtnInit(PG_MSGARG param);
	void OnRepeatBtnInit(PG_MSGARG param);
	void OnChipinBtnInit(PG_MSGARG param);
	void OnTutorialBtnInit(PG_MSGARG param);
	void OnTutorialClick(void);

private:
	// What one page of the explanation toggle shows: the pictures of the
	// course and chip-in areas, and how far each area moves when the page
	// turns to it.
	struct sPage {
		const Bitmap* courseImg;   // JP +0x0 (page 0: "single_start_img_course", 1: "single_start_expl_course")
		const Bitmap* chipinImg;   // JP +0x4 (page 0: "single_start_img_chipin", 1: "single_start_expl_chipin")
	};
	struct sPageOffset {
		WPoint course;   // JP +0x0
		WPoint chipin;   // JP +0x8
	};

	FrButton* m_pExplButton;      // JP +0x110 ("expl")
	FrArea* m_pCourseImg;         // JP +0x114 ("img_course")
	FrArea* m_pChipinImg;         // JP +0x118 ("img_chipin")
	FrButton* m_pCourseButton;    // JP +0x11c ("btn_course")
	FrButton* m_pRepeatButton;    // JP +0x120 ("btn_repeat")
	FrButton* m_pChipinButton;    // JP +0x124 ("btn_chipin")
	FrButton* m_pTutorialButton;  // JP +0x128 ("btn_tutorial")
	int m_page;                   // JP +0x12c (0 pictures, 1 explanations)
	sPage m_pages[2];             // JP +0x130
	sPageOffset m_offsets[2];     // JP +0x140
	unsigned char m_kind;         // JP +0x160
};

#endif // PG_JP_JPSINGLEMODESTARTDLG_H
