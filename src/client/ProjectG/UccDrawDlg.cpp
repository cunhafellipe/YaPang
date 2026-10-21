// Original: d:\build\custom\temp\code\client\projectg\uccdrawdlg.cpp
// The out-of-line copies of uccbase.h:141/144/212 (CUccBase::GetTexture/GetMask,
// CUccBaseItemDraw::GetTextureName)
// live in this object (see the end of UccBase.h).
#define UCCDRAWDLG_UCCBASE_INLINE
// The out-of-line copy of wtypes.h:79 (WPoint::operator+=) lives in this object.
#define UCCDRAWDLG_WTYPES_INLINE
#include "UccDrawDlg.h"
#include "ObjectFactory.h"
#include "UccBase.h"
#include "UccBrush.h"
#include "OverlayCursor.h"
#include "SecurityKeyManager.h"
#include "shared/globalgamedefine.h"
#include "frlistbox.h"
#include "frbutton.h"
#include "frarea.h"
#include "frgraphicinterface.h"
#include "NotifyForm.h"
#include "UccLibrary.h"
#include "UccPickColorDlg.h"
#include "UccSetInfoDlg.h"
#include "fresh.h"
#include "TaskManager.h"
#include "wresrcmng.h"
#include "NetResourceManager.h"
#include "InputManager.h"
#include "UccClothes.h"
#include "SharedDoc.h"
#include "shared/ItemManager.h"
#include "shared/classdefine.h"
#include "UccManager.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// uccbase.h:106
void CUccBase::SetParent(FrUccDrawDlg* pParent) {
	m_pParent = pParent;
}

// overlaycursor.h:44
void COverlayCursor::SetRect(FrWnd** ppWnd) {
	m_ppWnd = ppWnd;
}

// overlaycursor.h:47
void COverlayCursor::SetCursor(eCursorType type) {
	m_cursorType = type;
}

// uccdrawdlg.h:86
bool FrUccDrawDlg::IsSaved(void) {
	return m_bSaved;
}

// uccbrush.h:41
int CUccBrush::GetBrushNum(void) {
	return m_brushes.size();
}

// uccdrawdlg.cpp:24 (run-time type and factory registration of FrUccDrawDlg)
const WRTTI FrUccDrawDlg::m_RTTI("FrUccDrawDlg", &FrForm::m_RTTI);

IObject* FrUccDrawDlgMakeInstance(void) {
	return new FrUccDrawDlg;
}

struct __sFrUccDrawDlg {
	__sFrUccDrawDlg(void) {
		ObjectFactory().AddObjectFunctor(FrUccDrawDlgMakeInstance, "FrUccDrawDlg");
	}
};
static __sFrUccDrawDlg s_registerFrUccDrawDlg;

// uccdrawdlg.cpp:26
const sFRESH_MSGMAP* FrUccDrawDlg::GetMessageMap(void) const {
	return &FrUccDrawDlg::_MsgMap;
}

sFRESH_MSGMAP FrUccDrawDlg::_MsgMap = { &FrForm::_MsgMap, &FrUccDrawDlg::_MsgEntries[0] };

sFRESH_ENTRY FrUccDrawDlg::_MsgEntries[] = {
	// uccdrawdlg.cpp:29
	{ "ucccanvas", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnUccCanvasInit },
	// uccdrawdlg.cpp:30
	{ "undo_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnUndoBtnInit },
	// uccdrawdlg.cpp:31
	{ "undo_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnUndoBtnUp },
	// uccdrawdlg.cpp:32
	{ "redo_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnRedoBtnInit },
	// uccdrawdlg.cpp:33
	{ "redo_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnRedoBtnUp },
	// uccdrawdlg.cpp:34
	{ "flip_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnFlipBtnInit },
	// uccdrawdlg.cpp:35
	{ "flip_btn", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnFlipBtnOwnerDraw },
	// uccdrawdlg.cpp:36
	{ "flip_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnFlipBtnUp },
	// uccdrawdlg.cpp:37
	{ "reset_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnResetBtnInit },
	// uccdrawdlg.cpp:38
	{ "reset_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnResetBtnUp },
	// uccdrawdlg.cpp:39
	{ "side1_area", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnSide1AreaInit },
	// uccdrawdlg.cpp:40
	{ "side2_area", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnSide2AreaInit },
	// uccdrawdlg.cpp:41
	{ "temp_save_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnTempSaveBtnInit },
	// uccdrawdlg.cpp:42
	{ "temp_save_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnTempSaveBtnUp },
	// uccdrawdlg.cpp:45
	{ "grid_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnGridBtnInit },
	// uccdrawdlg.cpp:46
	{ "grid_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnGridBtnUp },
	// uccdrawdlg.cpp:47
	{ "drag_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnDragBtnInit },
	// uccdrawdlg.cpp:48
	{ "drag_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnDragBtnUp },
	// uccdrawdlg.cpp:49
	{ "zoomin_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnZoomInBtnInit },
	// uccdrawdlg.cpp:50
	{ "zoomin_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnZoomInBtnUp },
	// uccdrawdlg.cpp:51
	{ "zoomout_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnZoomOutBtnInit },
	// uccdrawdlg.cpp:52
	{ "zoomout_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnZoomOutBtnUp },
	// uccdrawdlg.cpp:53
	{ "draw_tab_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnTabDrawBtnInit },
	// uccdrawdlg.cpp:54
	{ "draw_tab_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnTabDrawBtnUp },
	// uccdrawdlg.cpp:55
	{ "pattern_tab_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnTabPatternBtnInit },
	// uccdrawdlg.cpp:56
	{ "pattern_tab_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnTabPatternBtnUp },
	// uccdrawdlg.cpp:57
	{ "emoticon_tab_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnTabEmoticonBtnInit },
	// uccdrawdlg.cpp:58
	{ "emoticon_tab_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnTabEmoticonBtnUp },
	// uccdrawdlg.cpp:59
	{ "text_tab_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnTabTextBtnInit },
	// uccdrawdlg.cpp:60
	{ "text_tab_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnTabTextBtnUp },
	// uccdrawdlg.cpp:64
	{ "finish_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnFinishBtnInit },
	// uccdrawdlg.cpp:65
	{ "finish_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnFinishBtnUp },
	// uccdrawdlg.cpp:66
	{ "close_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnCloseBtnInit },
	// uccdrawdlg.cpp:67
	{ "close_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnCloseBtnUp },
	// uccdrawdlg.cpp:72
	{ "paint_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnPaintBtnInit },
	// uccdrawdlg.cpp:73
	{ "paint_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnPaintBtnUp },
	// uccdrawdlg.cpp:74
	{ "pick_color_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnPickColorBtnInit },
	// uccdrawdlg.cpp:75
	{ "pick_color_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnPickColorBtnUp },
	// uccdrawdlg.cpp:76
	{ "cur_color_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnCurrentColorBtnInit },
	// uccdrawdlg.cpp:77
	{ "cur_color_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnCurrentColorBtnUp },
	// uccdrawdlg.cpp:78
	{ "cur_color_area", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnCurrentColorAreaInit },
	// uccdrawdlg.cpp:79
	{ "color_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnColorListInit },
	// uccdrawdlg.cpp:80
	{ "color_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnColorListOwnerDraw },
	// uccdrawdlg.cpp:81
	{ "color_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnColorListBtnDown },
	// uccdrawdlg.cpp:82
	{ "eraser_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnEraserListInit },
	// uccdrawdlg.cpp:83
	{ "eraser_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnEraserListOwnerDraw },
	// uccdrawdlg.cpp:84
	{ "eraser_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnEraserListBtnDown },
	// uccdrawdlg.cpp:85
	{ "toolinfo_eraser_area", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnToolInfoEraserAreaInit },
	// uccdrawdlg.cpp:90
	{ "brush_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnBrushListInit },
	// uccdrawdlg.cpp:91
	{ "brush_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnBrushListOwnerDraw },
	// uccdrawdlg.cpp:92
	{ "brush_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnBrushListBtnDown },
	// uccdrawdlg.cpp:93
	{ "pen_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnPenListInit },
	// uccdrawdlg.cpp:94
	{ "pen_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnPenListOwnerDraw },
	// uccdrawdlg.cpp:95
	{ "pen_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnPenListBtnDown },
	// uccdrawdlg.cpp:96
	{ "toolinfo_pen_area", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnToolInfoPenAreaInit },
	// uccdrawdlg.cpp:97
	{ "toolinfo_brush_area", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnToolInfoBrushAreaInit },
	// uccdrawdlg.cpp:98
	{ "color_list_area", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnColorListsAreaInit },
	// uccdrawdlg.cpp:102
	{ "arrow_down_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnArrowDownBtnInit },
	// uccdrawdlg.cpp:103
	{ "arrow_down_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnArrowDownBtnUp },
	// uccdrawdlg.cpp:104
	{ "arrow_right_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnArrowRightBtnInit },
	// uccdrawdlg.cpp:105
	{ "arrow_right_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnArrowRightBtnUp },
	// uccdrawdlg.cpp:106
	{ "arrow_left_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnArrowLeftBtnInit },
	// uccdrawdlg.cpp:107
	{ "arrow_left_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnArrowLeftBtnUp },
	// uccdrawdlg.cpp:108
	{ "delete_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnDeleteBtnInit },
	// uccdrawdlg.cpp:109
	{ "delete_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnDeleteBtnUp },
	// uccdrawdlg.cpp:110
	{ "modify_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnModifyBtnInit },
	// uccdrawdlg.cpp:111
	{ "modify_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnModifyBtnUp },
	// uccdrawdlg.cpp:112
	{ "make_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnMakeBtnInit },
	// uccdrawdlg.cpp:113
	{ "make_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnMakeBtnUp },
	// uccdrawdlg.cpp:114
	{ "rate_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnRateListInit },
	// uccdrawdlg.cpp:115
	{ "rate_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnRateListOwnerDraw },
	// uccdrawdlg.cpp:116
	{ "rate_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnRateListBtnDown },
	// uccdrawdlg.cpp:119
	{ "pattern_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnPatternListInit },
	// uccdrawdlg.cpp:120
	{ "pattern_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnPatternListOwnerDraw },
	// uccdrawdlg.cpp:121
	{ "pattern_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrUccDrawDlg::OnPatternListBtnDown },
	// uccdrawdlg.cpp:121 (same line as the previous row)
	{ "base_color_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnBaseColorListInit },
	// uccdrawdlg.cpp:121 (same line as the previous row)
	{ "extended_base_pattern_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnExtendedBasePatternListInit },
	// uccdrawdlg.cpp:125
	{ "etc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnEtcInit },
	// uccdrawdlg.cpp:126
	{ "etc", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnEtcOwnerDraw },
	// uccdrawdlg.cpp:127
	{ "tooltip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnToolTipInit },
	// uccdrawdlg.cpp:128
	{ "tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUccDrawDlg::OnToolTipOnwerDraw },
	// uccdrawdlg.cpp:130
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// uccdrawdlg.cpp:133
FrUccDrawDlg::FrUccDrawDlg(void) {
	Init();
}

// uccdrawdlg.cpp:138
FrUccDrawDlg::~FrUccDrawDlg(void) {
	Clear();
}

// uccdrawdlg.cpp:143
void FrUccDrawDlg::Init(void) {
	m_pSpareDlg = NULL;
	m_pUccSetInfoDlg = NULL;
	m_pUccPickColorDlg = NULL;
	m_pAskResetDlg = NULL;
	m_pAskCompleteDlg = NULL;
	m_pAskCancelDlg = NULL;
	m_pWaitingForm = NULL;
	m_bSavingTemporarily = false;
	m_waitingTime = 0.0f;
	m_pUccBase[0] = NULL;
	m_pUccBase[1] = NULL;
	m_pItemInfo = NULL;
	m_pCanvasWnd = NULL;
	m_pUndoBtn = NULL;
	m_pRedoBtn = NULL;
	m_pResetBtn = NULL;
	m_pDragBtn = NULL;
	m_pZoomInBtn = NULL;
	m_pZoomOutBtn = NULL;
	m_pDrawBtn = NULL;
	m_pTabPatternBtn = NULL;
	m_pTabEmoticonBtn = NULL;
	m_pTabTextBtn = NULL;
	m_eraserIndex = 0;
	m_brushType = 0;
	m_curSide = 0;
	m_drawMode = (eDrawMode)0;
	m_bSaved = true;
	m_bCloseAfterSave = false;
	m_name[0] = '\0';
	m_patternIndex = -1;
	m_patternRate = 2;
	m_bPatternListExpanded = true;
	m_bShowGrid = false;
	m_initialized = true;
	m_canvasRect = WRect(0, 0, 0, 0);

	m_pToolBox = g_pFresh->RegisterBitmap("ucc_tool_box");
	m_pSelectBox = g_pFresh->RegisterBitmap("ucc_select_box");
	m_pSideTextBg[0][0] = g_pFresh->RegisterBitmap("ucc_bg_text_front1");
	m_pSideTextBg[0][1] = g_pFresh->RegisterBitmap("ucc_bg_text_back1");
	m_pSideTextBg[1][0] = g_pFresh->RegisterBitmap("ucc_bg_text_front2");
	m_pSideTextBg[1][1] = g_pFresh->RegisterBitmap("ucc_bg_text_back2");
	m_pToolChangeIcon = g_pFresh->RegisterBitmap("ucc_tool_change_icon");
	m_pSelectSBox2 = g_pFresh->RegisterBitmap("ucc_select_sbox2");
	m_pSelectBoxYellow = g_pFresh->RegisterBitmap("ucc_select_box_yellow");
	m_pSelectBoxPink = g_pFresh->RegisterBitmap("ucc_select_box_pink");
	m_pSelectColor = g_pFresh->RegisterBitmap("ucc_select_color");

	// The first nine palette colours; the other nine start white and clear.
	unsigned char colors[9][3] = {
		{ 0xFF, 0x1C, 0x1C },
		{ 0xFF, 0xB1, 0x08 },
		{ 0xFF, 0xE7, 0x19 },
		{ 0x7A, 0xB3, 0x37 },
		{ 0x47, 0xC0, 0xBD },
		{ 0x0E, 0x39, 0x87 },
		{ 0x04, 0x25, 0x63 },
		{ 0x92, 0x1C, 0xBE },
		{ 0x00, 0x00, 0x00 }
	};

	for (int i = 0; i < 18; i++) {
		m_palette[i].Create(1, 1, 32);
		if (i < 9) {
			m_palette[i].SetPixel(0, 0, colors[i][0], colors[i][1], colors[i][2], 0xFF);
		} else {
			m_palette[i].SetPixel(0, 0, 0xFF, 0xFF, 0xFF, 0);
		}
	}

	m_toolTipManager.Add((FrWnd**)&m_pUndoBtn, (eBarButton)0x33);
	m_toolTipManager.Add((FrWnd**)&m_pRedoBtn, (eBarButton)0x32);
	m_toolTipManager.Add((FrWnd**)&m_pResetBtn, (eBarButton)0x34);
	m_toolTipManager.Add((FrWnd**)&m_pDragBtn, (eBarButton)0x2F);
	m_toolTipManager.Add((FrWnd**)&m_pZoomInBtn, (eBarButton)0x35);
	m_toolTipManager.Add((FrWnd**)&m_pZoomOutBtn, (eBarButton)0x36);
	m_toolTipManager.Add((FrWnd**)&m_pTempSaveBtn, (eBarButton)0x3D);

	m_tabManager.Add(UCC_TAB_DRAW, &m_pPickColorBtn, &m_pCurrentColorBtn, &m_pPaintBtn, &m_pCurrentColorArea, &m_pColorListsArea, NULL);
	m_tabManager.Add(UCC_TAB_DRAW, &m_pColorList, &m_pEraserList, &m_pBrushList, &m_pPenList, NULL);

	m_tabManager.Add(UCC_TAB_PATTERN, &m_pPickColorBtn, &m_pCurrentColorBtn, &m_pPaintBtn, &m_pCurrentColorArea, NULL);
	m_tabManager.Add(UCC_TAB_PATTERN, &m_pColorList, &m_pEraserList, NULL);
	m_tabManager.Add(UCC_TAB_PATTERN, &m_pArrowDownBtn, &m_pPatternRightBtn, &m_pPatternLeftBtn, NULL);
	m_tabManager.Add(UCC_TAB_PATTERN, &m_pDeleteBtn, &m_pModifyBtn, &m_pMakeBtn, NULL);
	m_tabManager.Add(UCC_TAB_PATTERN, &m_pRateList, &m_pPatternList, &m_pBaseColorListArea, NULL);

	m_patternHighlight.Create(18, 18, 32);
	for (int y = 0; y < 18; y++) {
		for (int x = 0; x < 18; x++) {
			m_patternHighlight.SetPixel(x, y, 0xF4, 0xF4, 0xF4, 0xFF);
		}
	}

	m_blankPattern.Create(88, 88, 32);
	for (int y = 0; y < 88; y++) {
		for (int x = 0; x < 88; x++) {
			m_blankPattern.SetPixel(x, y, 0xFF, 0xFF, 0xFF, 0xFF);
		}
	}

	for (int i = 0; i < 24; i++) {
		Bitmap* pBrush = UccBrushList((eBrushType)3)->GetBrush(i);
		if (pBrush != NULL) {
			GetResizedImage(pBrush, NULL, &m_patternThumbnails[i], 18, 18);
		} else {
			m_patternThumbnails[i] = m_patternHighlight;
			UccBrushList((eBrushType)3)->AddBrush(&m_blankPattern);
		}
	}

	m_overlayCursor.SetRect(&m_pCanvasWnd);
	m_overlayCursor.Add((COverlayCursor::eCursorType)1, "ucc_cursor_drag.tga", 12, 14);
	m_overlayCursor.Add((COverlayCursor::eCursorType)2, "ucc_cursor_eraser.tga", 5, 6);
	m_overlayCursor.Add((COverlayCursor::eCursorType)3, "ucc_cursor_eyedropper.tga", 6, 22);
	m_overlayCursor.Add((COverlayCursor::eCursorType)4, "ucc_cursor_paint.tga", 3, 18);
	m_overlayCursor.Add((COverlayCursor::eCursorType)5, "ucc_cursor_select_color.tga", 10, 10);
	m_overlayCursor.Add((COverlayCursor::eCursorType)6, "ucc_cursor_zoom_in.tga", 11, 10);
	m_overlayCursor.Add((COverlayCursor::eCursorType)7, "ucc_cursor_zoom_out.tga", 11, 10);
}

// uccdrawdlg.cpp:286
void FrUccDrawDlg::Clear(void) {
	UccCurrentBrush()->SetBrush((eBrushType)0, 0, 0, 0, 0);
	SetCurrentColor(0, 0, 0);

	if (m_pBrushList->GetSelected() != NULL) {
		m_pBrushList->GetSelected()->selected = false;
	}
	if (m_pPenList->GetSelected() != NULL) {
		m_pPenList->GetSelected()->selected = false;
	}
	if (m_pPatternList->GetSelected() != NULL) {
		m_pPatternList->GetSelected()->selected = false;
	}

	m_pBrushList->SelectItem(m_pBrushList->GetItem(0), true);

	if (m_pUccBase[0] != NULL) { delete m_pUccBase[0]; m_pUccBase[0] = NULL; }
	if (m_pUccBase[1] != NULL) { delete m_pUccBase[1]; m_pUccBase[1] = NULL; }

	m_bShowGrid = false;
}

// uccdrawdlg.cpp:317
bool FrUccDrawDlg::Open(FRESH_RESULT_CALLBACK pfnResult, const WPoint& pt, unsigned long flag) {
	bool bRet = FrForm::Open(pfnResult, pt, flag);

	WPoint pos(GetRect().x, GetRect().y);

	m_canvasRect = m_pCanvasWnd->GetRect() - pos;

	MoveCanvasToCenter();

	return bRet;
}

// uccdrawdlg.cpp:330
void FrUccDrawDlg::OnCancel(void) {
	if (m_bSaved) {
		m_pAskCancelDlg = NULL;
		Clear();
		FrForm::OnCancel();
	} else if (m_pAskCancelDlg == NULL) {
		m_pAskCancelDlg = CreateForm<FrNotifyForm>(g_pFresh->GetManager(), this, "notify_yesnocancel", 0);
		m_pAskCancelDlg->SetMessage("\xB1\xD7\xB8\xAE\xB4\xF8 \xC0\xC7\xBB\xF3\xC0\xBB \xC0\xD3\xBD\xC3\xC0\xFA\xC0\xE5\xC7\xCF\xB0\xED \xB3\xA1\xB3\xBB\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);
		m_pAskCancelDlg->Open((FRESH_RESULT_CALLBACK)&FrUccDrawDlg::OnAskCancelDlgResult, 1);
	}
}

// uccdrawdlg.cpp:347
bool FrUccDrawDlg::Close(eFormRet ret, bool bDestroy) {
	CloseWaitingForm();
	return FrForm::Close(ret, bDestroy);
}

// uccdrawdlg.cpp:354
void FrUccDrawDlg::SetName(const char* name) {
	strcpy(m_name, name);
}

// netresourcemanager.cpp:46 (no header declares it)
void G_CreateDirectory(const char *path);

// uccdrawdlg.cpp:359
void FrUccDrawDlg::Upload(void) {
	G_CreateDirectory(MakeStr("%s\\", GetUccTempFoler()));

	size_t len = strlen(m_name);

	if (len == 0) {
		std::string path = GenerateTextureFullPath(0, m_pItemInfo->m_typeId, m_pItemInfo->m_uccKey);

		if (!ZipUccClothes(m_pUccBase[0]->GetTexture32BPP(true), m_pUccBase[1]->GetTexture32BPP(true), NULL, path.c_str())) {
			Notify(MakeStr("\xbe\xf7\xb7\xce\xb5\xe5\xb0\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.(\xc4\xda\xb5\xe5:%d)", 4));
			return;
		}

		WSingleton<NetResourceManager>::Instance()->UploadUccTempClothes(m_pItemInfo->m_typeId, m_pItemInfo->m_uccKey);
	} else {
		std::string path = GenerateTextureFullPath(0, m_pItemInfo->m_typeId, m_pItemInfo->m_uccKey);
		Bitmap* pIcon = new Bitmap(0x40, 0x54, 0x20);
		CreateItemThumbnail(m_pUccBase[0]->GetTexture(), m_pUccBase[0]->GetMask(), pIcon);

		if (!ZipUccClothes(m_pUccBase[0]->GetTexture(), m_pUccBase[1]->GetTexture(), pIcon, path.c_str())) {
			Notify(MakeStr("\xbe\xf7\xb7\xce\xb5\xe5\xb0\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.(\xc4\xda\xb5\xe5:%d)", 3));
			return;
		}

		std::string iconName = GenerateIconFilename(m_pItemInfo->m_typeId, m_pItemInfo->m_uccKey);
		UccManager()->m_iconMap.insert(std::pair<const std::string, Bitmap*>(iconName, pIcon));

		WSingleton<NetResourceManager>::Instance()->UploadUccClothes(m_pItemInfo->m_typeId, m_pItemInfo->m_uccKey, m_name);
	}
}

// uccdrawdlg.cpp:394
void FrUccDrawDlg::Notify(const char* text) {
	if (AfxGetTask()->GetActor("RealMyRoom") != NULL) {
		AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)text, 0, 0, 0, 0);
	} else {
		AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)text, 0, 0, 0, 0);
	}
}

// uccdrawdlg.cpp:406
void FrUccDrawDlg::SetCurrentColor(unsigned char r, unsigned char g, unsigned char b) {
	m_currentColor.SetPixel(0, 0, r, g, b, 0xff);

	RefreshTexCache(m_currentColor);

	UccCurrentBrush()->SetBrush(r, g, b);
}

// uccdrawdlg.cpp:415
void FrUccDrawDlg::MoveCanvasToCenter(void) {
	WPoint pos(GetRect().x, GetRect().y);

	WPoint pt(m_canvasRect.x, m_canvasRect.y);
	pt += pos;

	Bitmap* pTexture = m_pUccBase[m_curSide]->GetTexture32BPP(false);
	if (pTexture != NULL) {
		int offsetX = (256 - pTexture->Width()) / 2;
		int offsetY = (256 - pTexture->Height()) / 2;
		pt.x += offsetX;
		pt.y += offsetY;
	}

	m_pCanvasWnd->MoveWindow(pt);
}

// uccdrawdlg.cpp:442
void FrUccDrawDlg::OnProc(const float dt) {
	if (m_pUccBase[m_curSide] == NULL) {
		return;
	}

	const WPoint& mousePos = g_pFresh->GetManager()->GetMousePos();

	if (m_pUccBase[m_curSide]->m_mode == 0 && (m_brushType == 0 || m_brushType == 1)) {
		m_overlayCursor.SetCursor(COverlayCursor::CURSOR_NONE);
	}

	if (m_pUccBase[m_curSide]->IsMouseOver(mousePos)) {
		if (g_input->GetButton(LEFT_BUTTON) > 0) {
			m_pUccBase[m_curSide]->MouseEventHandler(LEFT_BUTTON, g_input->GetButton(LEFT_BUTTON), mousePos);
		}
	} else if (m_pUccBase[m_curSide]->m_bInitialized && g_input->GetButton(LEFT_BUTTON) == 2) {
		m_pUccBase[m_curSide]->MouseEventHandler(LEFT_BUTTON, 2, WPoint());
	}

	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) && g_input->GetDown("UNDO", false)) {
		OnUndoBtnUp();
	}

	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) && g_input->GetDown("REDO", false)) {
		OnRedoBtnUp();
	}

	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) && g_input->GetDown("FLIP", false)) {
		OnFlipBtnUp();
	}

	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) && g_input->GetDown("PAINT", false)) {
		OnPaintBtnUp();
	}

	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) && g_input->GetDown("DRAG", false)) {
		OnDragBtnUp();
	}

	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) && g_input->GetDown("GRID", false)) {
		OnGridBtnUp();
	}

	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) && g_input->GetDown("SAVE", false)) {
		OnTempSaveBtnUp();
	}

	m_pUccBase[m_curSide]->Process();

	if (m_pUndoBtn != NULL && m_pRedoBtn != NULL && m_pUccBase[m_curSide] != NULL) {
		m_pUndoBtn->Enable(m_pUccBase[m_curSide]->IsUndoPossible());
		m_pRedoBtn->Enable(m_pUccBase[m_curSide]->IsRedoPossible());
	}

	if (m_bSavingTemporarily) {
		m_waitingTime += dt;
		if (m_pWaitingForm == NULL) {
			m_pWaitingForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "wait", 0);
			if (strlen(m_name) == 0) {
				m_pWaitingForm->SetMessage("\xbc\xbf\xc7\xc1\xb5\xf0\xc0\xda\xc0\xce \xc0\xc7\xbb\xf3\xc0\xbb \xc0\xd3\xbd\xc3 \xc0\xfa\xc0\xe5\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.", false);
			} else {
				m_pWaitingForm->SetMessage("\xbc\xbf\xc7\xc1\xb5\xf0\xc0\xda\xc0\xce \xc0\xc7\xbb\xf3\xc0\xbb \xbe\xf7\xb7\xce\xb5\xe5 \xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.", false);
			}
			m_pWaitingForm->Open(NULL, 0);

			FrButton* pCancelBtn = DYNAMIC_CAST(FrButton, m_pWaitingForm->FindChildByName("cancel"));
			if (pCancelBtn != NULL) {
				pCancelBtn->Enable(false);
			}
		}

		eResourceType resourceType = m_drawMode != 0 ? RESOURCE_TYPE_1 : RESOURCE_TYPE_UCC_CLOTHES;
		if (m_waitingTime > 20.0f && WSingleton<NetResourceManager>::Instance()->IsFailedToUpload(m_pItemInfo->m_typeId, m_pItemInfo->m_uccKey, resourceType)) {
			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)MakeStr("\xbe\xf7\xb7\xce\xb5\xe5\xb0\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.(\xc4\xda\xb5\xe5:%d)", 2), 0, 0, 0, 0);
			m_bSavingTemporarily = false;
		}
	} else {
		CloseWaitingForm();
	}
}

// uccdrawdlg.cpp:592
void FrUccDrawDlg::SetUccDrawInfo(eDrawMode mode, sItemInfo* pItemInfo, bool b) {
	m_pItemInfo = pItemInfo;
	m_drawMode = mode;
	unsigned long typeId = pItemInfo->m_typeId;

	char uccKey[9];
	memset(uccKey, 0, sizeof(uccKey));
	strncpy(uccKey, pItemInfo->m_uccKey, 9);
	Bitmap tempTextures[2];

	bool bTempLoaded = false;
	if (typeId == 0 || strlen(uccKey) == 0 || !IsUccClothes(typeId)) {
		Notify(MakeStr("\xbc\xad\xb9\xf6\xb7\xce\xba\xce\xc5\xcd \xc7\xca\xbf\xe4\xc7\xd1 \xc1\xa4\xba\xb8\xb8\xa6 \xbe\xf2\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9(\xc4\xda\xb5\xe5:%d)", 1));
		m_initialized = false;
		OnCancel();
		return;
	}

	if (!b) {
		bTempLoaded = UccManager()->GetUccTempClothes(m_pItemInfo->m_typeId, m_pItemInfo->m_uccKey, tempTextures[0], tempTextures[1]);
	}

	if (mode == 0) {
		m_pUccBase[0] = new CUccClothes;
		m_pUccBase[1] = new CUccClothes;
	} else if (mode == 1) {
		// mode 1 creates no canvas here
	} else {
		Notify(MakeStr("\xbf\xb9\xbb\xf3\xc4\xa1 \xb8\xf8\xc7\xd1 \xbf\xc0\xb7\xf9\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9(\xc4\xda\xb5\xe5:%d)", 1));
		m_initialized = false;
		OnCancel();
		return;
	}

	m_curSide = 0;
	if (mode == 0) {
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(typeId);

		if (pPart != NULL) {
			for (int i = 0; i < 2; i++) {
				if (!m_pUccBase[i]->IsInitialized()) {
					Bitmap* pTexture = g_resrcmng->LoadBitmap(pPart->texture[i], 0, false);
					Bitmap* pSource = g_resrcmng->LoadBitmap(pPart->texture[i], 0, false);

					Bitmap mask;
					if (pSource != NULL) {
						mask = *pSource;
					} else {
						mask.Create(pTexture->Width(), pTexture->Height(), 0x18);
						for (int y = 0; y < mask.Height(); y++) {
							for (int x = 0; x < mask.Width(); x++) {
								mask.SetPixel(x, y, 0xff, 0xff, 0xff, 0xff);
							}
						}
					}

					Bitmap* pMask = g_resrcmng->LoadBitmap(GetMaskFilename(pPart->texture[i]).c_str(), 0, false);
					if (pMask != NULL) {
						for (int y = 0; y < mask.Height(); y++) {
							for (int x = 0; x < mask.Width(); x++) {
								unsigned char r, g, b;
								pMask->GetPixel(x, y, r, g, b);
								if (r < 5 && g < 5 && b < 5) {
									mask.SetPixel(x, y, 0, 0, 0, 0xff);
								}
							}
						}
					}

					((CUccBaseItemDraw*)m_pUccBase[i])->InitializeItemDraw(i, (FrArea*)m_pCanvasWnd, pTexture, &mask, m_pItemInfo->m_typeId, m_pItemInfo->m_uccKey);

					ClearTexCache(pTexture);
					delete pTexture;
					ClearTexCache(pSource);
					delete pSource;
					ClearTexCache(pMask);
					delete pMask;

					if (m_pUccBase[i]->GetTexture() != NULL) {
						g_resrcmng->UploadTexture(((CUccBaseItemDraw*)m_pUccBase[i])->GetTextureName(), m_pUccBase[i]->GetTexture(), 0, 0);
					}

					m_pUccBase[i]->SetParent(this);

					if ((m_pItemInfo->m_uccStatus & 2) && bTempLoaded) {
						Bitmap* pDest = m_pUccBase[i]->GetTexture();
						Bitmap* pTexture32 = m_pUccBase[i]->GetTexture32BPP(true);
						*pTexture32 = tempTextures[i];
						Convert32to24(pTexture32, pDest, &mask);
					}
				}
			}
		} else {
			Notify("\xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xb4\xc2 \xc0\xc7\xbb\xf3\xc0\xd4\xb4\xcf\xb4\xd9.");
			m_initialized = false;
			OnCancel();
			return;
		}
	}

	if (m_pUccBase[0] == NULL || m_pUccBase[0]->GetTexture() == NULL || m_pUccBase[0]->GetTexture32BPP(false) == NULL) {
		Notify("\xc5\xd8\xbd\xba\xc3\xc4\xb8\xa6 \xba\xd2\xb7\xaf\xbf\xc0\xb4\xc2 \xb5\xa5\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_initialized = false;
		OnCancel();
		return;
	}

	if (m_pUccBase[1] != NULL) {
		if (m_pUccBase[1]->GetTexture() == NULL || m_pUccBase[1]->GetTexture32BPP(false) == NULL) {
			Notify("\xc5\xd8\xbd\xba\xc3\xc4\xb8\xa6 \xba\xd2\xb7\xaf\xbf\xc0\xb4\xc2 \xb5\xa5\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.");
			m_initialized = false;
			OnCancel();
			return;
		}
	}

	int zoom = 256 / m_pUccBase[m_curSide]->GetTexture32BPP(false)->Width();
	int zoomY = 256 / m_pUccBase[m_curSide]->GetTexture32BPP(false)->Height();
	if (zoom > zoomY) {
		zoom = zoomY;
	}

	while (m_pUccBase[m_curSide]->GetZoomRate() < zoom) {
		bool bZoomed = true;
		if (m_pUccBase[0] != NULL) {
			bZoomed = bZoomed && m_pUccBase[0]->ZoomIn(-1, -1);
		}
		if (m_pUccBase[1] != NULL) {
			bZoomed = bZoomed && m_pUccBase[1]->ZoomIn(-1, -1);
		}
		if (!bZoomed) {
			break;
		}
	}

	m_pZoomInBtn->Enable(m_pUccBase[m_curSide]->GetZoomRate() < 8);
	m_pZoomOutBtn->Enable(m_pUccBase[m_curSide]->GetZoomRate() > 1);

	if (m_canvasRect.w != 0.0f) {
		MoveCanvasToCenter();
	}

	for (int i = 1; i >= 0; i--) {
		CUccBase* pBase = m_pUccBase[i];
		if (pBase != NULL) {
			pBase->m_refreshFlags |= 2;
			m_pUccBase[i]->Process();
		}
	}
}

// uccdrawdlg.cpp:815
void FrUccDrawDlg::OnUccCanvasInit(PG_MSGARG param) {
	m_pCanvasWnd = DYNAMIC_CAST(FrArea, param);
}

// uccdrawdlg.cpp:820
void FrUccDrawDlg::OnUndoBtnInit(PG_MSGARG param) {
	m_pUndoBtn = DYNAMIC_CAST(FrButton, param);
}

// uccdrawdlg.cpp:825
void FrUccDrawDlg::OnUndoBtnUp(void) {
	if (m_pUccBase[m_curSide] != NULL) {
		m_pUccBase[m_curSide]->Undo();
	}
}

// uccdrawdlg.cpp:833
void FrUccDrawDlg::OnRedoBtnInit(PG_MSGARG param) {
	m_pRedoBtn = DYNAMIC_CAST(FrButton, param);
}

// uccdrawdlg.cpp:838
void FrUccDrawDlg::OnRedoBtnUp(void) {
	if (m_pUccBase[m_curSide] != NULL) {
		m_pUccBase[m_curSide]->Redo();
	}
}

// uccdrawdlg.cpp:846
void FrUccDrawDlg::OnFlipBtnInit(PG_MSGARG param) {
	m_pFlipBtn = DYNAMIC_CAST(FrButton, param);
}

// uccdrawdlg.cpp:851
void FrUccDrawDlg::OnFlipBtnOwnerDraw(PG_MSGARG param) {
	WRect rect = m_pFlipBtn->GetRect();

	FrGraphicInterface* pGDI = GetGDI();
	if (pGDI == NULL) {
		return;
	}

	if (m_pToolChangeIcon != NULL) {
		pGDI->DrawTexture(m_pToolChangeIcon, WRect(rect.x + 6.0f, rect.y + 6.0f, (float)m_pToolChangeIcon->Width(), (float)m_pToolChangeIcon->Height()), 0xffffffff, 0);
	}
}

// uccdrawdlg.cpp:866
void FrUccDrawDlg::OnFlipBtnUp(void) {
	eMode mode = m_pUccBase[m_curSide]->GetMode();
	m_pUccBase[m_curSide]->SetMode(UCC_MODE_DRAW);

	if (m_curSide == 0) {
		m_curSide = 1;
	} else {
		m_curSide = 0;
	}

	m_pUccBase[m_curSide]->m_refreshFlags |= 1;

	if (m_pSide1Area != NULL) {
		m_pSide1Area->SetBgImg(m_pSideTextBg[0][m_curSide]);
	}
	if (m_pSide2Area != NULL) {
		m_pSide2Area->SetBgImg(m_pSideTextBg[1][1 - m_curSide]);
	}

	m_pZoomInBtn->Enable(m_pUccBase[m_curSide]->m_zoomRate < 8);
	m_pZoomOutBtn->Enable(m_pUccBase[m_curSide]->m_zoomRate > 1);

	MoveCanvasToCenter();

	m_pUccBase[m_curSide]->SetMode(mode);
}

// uccdrawdlg.cpp:902
void FrUccDrawDlg::OnResetBtnInit(PG_MSGARG param) {
	m_pResetBtn = DYNAMIC_CAST(FrButton, param);
}

// uccdrawdlg.cpp:907
void FrUccDrawDlg::OnResetBtnUp(void) {
	if (m_pAskResetDlg == NULL) {
		m_pAskResetDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);
		m_pAskResetDlg->SetMessage("\xC1\xA4\xB8\xBB \xC3\xB3\xC0\xBD\xBA\xCE\xC5\xCD \xB4\xD9\xBD\xC3 \xB1\xD7\xB8\xAE\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE? \n\n(\xC7\xF6\xC0\xE7 \xC0\xCC \xB1\xD7\xB8\xAE\xB1\xE2\xBF\xEB \xC0\xC7\xBB\xF3\xBF\xA1 \xB1\xD7\xB7\xC1\xC1\xAE \xC0\xD6\xB4\xC2 \n   \xB8\xF0\xB5\xE7 \xB1\xD7\xB8\xB2\xC0\xCC \xC1\xF6\xBF\xF6\xC1\xFD\xB4\xCF\xB4\xD9)", false);
		m_pAskResetDlg->Open((FRESH_RESULT_CALLBACK)&FrUccDrawDlg::OnAskResetDlgResult, 1);
	}
}

// uccdrawdlg.cpp:917
void FrUccDrawDlg::OnGridBtnInit(PG_MSGARG param) {
	m_pGridBtn = DYNAMIC_CAST(FrButton, param);
}

// uccdrawdlg.cpp:922
void FrUccDrawDlg::OnGridBtnUp(void) {
	m_bShowGrid = !m_bShowGrid;
}

// uccdrawdlg.cpp:927
void FrUccDrawDlg::OnDragBtnInit(PG_MSGARG param) {
	m_pDragBtn = DYNAMIC_CAST(FrButton, param);
}

// uccdrawdlg.cpp:932
void FrUccDrawDlg::OnDragBtnUp(void) {
	CUccBase* pUccBase = m_pUccBase[m_curSide];
	if (pUccBase->GetMode() == UCC_MODE_MOVE) {
		pUccBase->SetMode(UCC_MODE_DRAW);
		m_overlayCursor.SetCursor((COverlayCursor::eCursorType)UCC_MODE_DRAW);
	} else {
		pUccBase->SetMode(UCC_MODE_MOVE);
		m_overlayCursor.SetCursor((COverlayCursor::eCursorType)UCC_MODE_MOVE);
	}
	SetToolbarStatus(m_pDragBtn);
}

// uccdrawdlg.cpp:948
void FrUccDrawDlg::OnZoomInBtnInit(PG_MSGARG param) {
	m_pZoomInBtn = DYNAMIC_CAST(FrButton, param);
}

// uccdrawdlg.cpp:953
void FrUccDrawDlg::OnZoomInBtnUp(void) {
	SetToolbarStatus(m_pZoomInBtn);
	m_overlayCursor.SetCursor((COverlayCursor::eCursorType)6);
	m_pUccBase[m_curSide]->SetMode(UCC_MODE_ZOOM_IN);
}

// uccdrawdlg.cpp:962
void FrUccDrawDlg::OnZoomOutBtnInit(PG_MSGARG param) {
	m_pZoomOutBtn = DYNAMIC_CAST(FrButton, param);
}

// uccdrawdlg.cpp:967
void FrUccDrawDlg::OnZoomOutBtnUp(void) {
	SetToolbarStatus(m_pZoomOutBtn);
	m_overlayCursor.SetCursor((COverlayCursor::eCursorType)7);
	m_pUccBase[m_curSide]->SetMode(UCC_MODE_ZOOM_OUT);
}

// uccdrawdlg.cpp:976
void FrUccDrawDlg::AfterZoom(void) {
	m_pZoomInBtn->Enable(m_pUccBase[m_curSide]->m_zoomRate < 8);
	m_pZoomOutBtn->Enable(m_pUccBase[m_curSide]->m_zoomRate > 1);

	MoveCanvasToCenter();

	if (m_pUccBase[m_curSide]->GetMode() == UCC_MODE_DRAW) {
		m_overlayCursor.SetCursor((COverlayCursor::eCursorType)0);
		SetToolbarStatus(m_pDrawBtn);
	}
}

// uccdrawdlg.cpp:991
void FrUccDrawDlg::SetSaved(bool bSaved) {
	m_bSaved = bSaved;

	if (bSaved) {
		m_fadingText.Print("\xBC\xBF\xC7\xC1\xB5\xF0\xC0\xDA\xC0\xCE \xC0\xC7\xBB\xF3\xC0\xCC \xC0\xD3\xBD\xC3 \xC0\xFA\xC0\xE5\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.", 3.0f, this, 105.0f, 20.0f, eFADE_IN_OUT, 0xff000000);
	}

	if (m_bCloseAfterSave) {
		Close(FrOK, true);
	}
}

// uccdrawdlg.cpp:1006
void FrUccDrawDlg::OnSide1AreaInit(PG_MSGARG param) {
	m_pSide1Area = DYNAMIC_CAST(FrArea, param);
}

// uccdrawdlg.cpp:1011
void FrUccDrawDlg::OnSide2AreaInit(PG_MSGARG param) {
	m_pSide2Area = DYNAMIC_CAST(FrArea, param);
}

// uccdrawdlg.cpp:1016
void FrUccDrawDlg::OnTempSaveBtnInit(PG_MSGARG param) {
	m_pTempSaveBtn = DYNAMIC_CAST(FrButton, param);
}

// uccdrawdlg.cpp:1021
void FrUccDrawDlg::OnTempSaveBtnUp(void) {
	SecurityKeyManager()->ReqCreateKey(SK_CONTENT_UCC, m_pItemInfo->m_serial);
	m_bSavingTemporarily = true;
}

// uccdrawdlg.cpp:1030
void FrUccDrawDlg::OnTabDrawBtnInit(PG_MSGARG param) {
	m_pDrawBtn = DYNAMIC_CAST(FrButton, param);
	m_pDrawBtn->SetStatus(FrButton::PRESSED);
}

// uccdrawdlg.cpp:1036
void FrUccDrawDlg::OnTabDrawBtnUp(void) {
	ChangeTab(UCC_TAB_DRAW);
	m_pUccBase[m_curSide]->SetMode(UCC_MODE_DRAW);
	m_overlayCursor.SetCursor((COverlayCursor::eCursorType)0);
	SetToolbarStatus(m_pDrawBtn);
}

// uccdrawdlg.cpp:1045
void FrUccDrawDlg::OnTabPatternBtnInit(PG_MSGARG param) {
	m_pTabPatternBtn = DYNAMIC_CAST(FrButton, param);
}

// uccdrawdlg.cpp:1050
void FrUccDrawDlg::OnTabPatternBtnUp(void) {
	ChangeTab(UCC_TAB_PATTERN);
	m_pUccBase[m_curSide]->SetMode(UCC_MODE_DRAW);

	m_overlayCursor.SetCursor((COverlayCursor::eCursorType)0);
	SetToolbarStatus(m_pTabPatternBtn);
}

// uccdrawdlg.cpp:1059
void FrUccDrawDlg::OnTabEmoticonBtnInit(PG_MSGARG param) {
	m_pTabEmoticonBtn = DYNAMIC_CAST(FrButton, param);
	m_pTabEmoticonBtn->Enable(false);
}

// uccdrawdlg.cpp:1065
void FrUccDrawDlg::OnTabEmoticonBtnUp(void) {
}

// uccdrawdlg.cpp:1070
void FrUccDrawDlg::OnTabTextBtnInit(PG_MSGARG param) {
	m_pTabTextBtn = DYNAMIC_CAST(FrButton, param);
	m_pTabTextBtn->Enable(false);
}

// uccdrawdlg.cpp:1076
void FrUccDrawDlg::OnTabTextBtnUp(void) {
}

// uccdrawdlg.cpp:1082
void FrUccDrawDlg::OnFinishBtnInit(PG_MSGARG param) {
	m_pFinishBtn = DYNAMIC_CAST(FrButton, param);
}

// uccdrawdlg.cpp:1087
void FrUccDrawDlg::Finish(void) {
	if (m_pItemInfo == NULL) {
		return;
	}
	if (m_pUccSetInfoDlg == NULL) {
		WRect rect = GetRect();
		m_pUccSetInfoDlg = CreateForm<FrUccSetInfoDlg>(g_pFresh->GetManager(), this, "ucc_setinfo", 0);
		m_pUccSetInfoDlg->SetParent(this);
		m_pUccSetInfoDlg->Open((FRESH_RESULT_CALLBACK)&FrUccDrawDlg::OnUccSetInfoDlgResult, WPoint(rect.x + 30.0f, rect.y + 30.0f), 0);
	}
}

// uccdrawdlg.cpp:1105
void FrUccDrawDlg::OnFinishBtnUp(void) {
	Finish();
}

// uccdrawdlg.cpp:1135
bool FrUccDrawDlg::OnAskCompleteDlgResult(int ret, FrForm* pForm) {
	m_pAskCompleteDlg = NULL;
	if (ret == FrOK) {
		Finish();
	}
	return true;
}

// uccdrawdlg.cpp:1147
bool FrUccDrawDlg::OnAskCancelDlgResult(int ret, FrForm* pForm) {
	m_pAskCancelDlg = NULL;

	if (ret == FrYES) {
		m_bCloseAfterSave = true;
		SecurityKeyManager()->ReqCreateKey(SK_CONTENT_UCC, m_pItemInfo->m_serial);
		m_bSavingTemporarily = true;
	} else if (ret == FrNO) {
		Clear();

		FrForm::OnCancel();
	}

	return true;
}

// uccdrawdlg.cpp:1171
bool FrUccDrawDlg::OnAskResetDlgResult(int ret, FrForm* pForm) {
	if (ret == FrOK) {
		Reset();
	}
	m_pAskResetDlg = NULL;
	return true;
}

// uccdrawdlg.cpp:1183
bool FrUccDrawDlg::OnUccSetInfoDlgResult(int ret, FrForm* pForm) {
	m_pUccSetInfoDlg = NULL;
	if (ret == FrOK) {
		SecurityKeyManager()->ReqCreateKey(SK_CONTENT_UCC, m_pItemInfo->m_serial);
		m_bSavingTemporarily = true;
	}
	return true;
}

// uccdrawdlg.cpp:1196
void FrUccDrawDlg::OnCloseBtnInit(intptr_t param) {
	m_pCloseBtn = DYNAMIC_CAST(FrButton, param);
}

// uccdrawdlg.cpp:1201
void FrUccDrawDlg::OnCloseBtnUp(void) {
	OnCancel();
}

// uccdrawdlg.cpp:1210
void FrUccDrawDlg::OnPaintBtnInit(PG_MSGARG param) {
	m_pPaintBtn = DYNAMIC_CAST(FrButton, param);
}

// uccdrawdlg.cpp:1215
void FrUccDrawDlg::OnPaintBtnUp(void) {
	m_pUccBase[m_curSide]->SetMode(UCC_MODE_PAINT);
	m_overlayCursor.SetCursor((COverlayCursor::eCursorType)4);
	SetToolbarStatus(m_pDrawBtn);
}

// uccdrawdlg.cpp:1223
void FrUccDrawDlg::OnPickColorBtnInit(PG_MSGARG param) {
	m_pPickColorBtn = DYNAMIC_CAST(FrButton, param);
}

// uccdrawdlg.cpp:1228
void FrUccDrawDlg::OnPickColorBtnUp(void) {
	m_pUccBase[m_curSide]->SetMode(UCC_MODE_SPOID);
	m_overlayCursor.SetCursor((COverlayCursor::eCursorType)3);
	SetToolbarStatus(m_pDrawBtn);
}

// uccdrawdlg.cpp:1236
void FrUccDrawDlg::OnCurrentColorBtnInit(PG_MSGARG param) {
	m_pCurrentColorBtn = DYNAMIC_CAST(FrButton, param);
}

// uccdrawdlg.cpp:1241
void FrUccDrawDlg::OnCurrentColorBtnUp(void) {
	if (m_pUccPickColorDlg == NULL) {
		WRect rect = GetRect();
		m_pUccPickColorDlg = CreateForm<FrUccPickColorDlg>(g_pFresh->GetManager(), this, "ucc_pickcolor", 0);
		m_pUccPickColorDlg->SetParent(this);
		m_pUccPickColorDlg->Open((FRESH_RESULT_CALLBACK)&FrUccDrawDlg::OnUccPickColorDlgResult, WPoint(rect.x + 30.0f, rect.y + 30.0f), 0);
	}
}

// uccdrawdlg.cpp:1252
bool FrUccDrawDlg::OnUccPickColorDlgResult(int ret, FrForm* pForm) {
	m_pUccPickColorDlg = NULL;
	return true;
}

// uccdrawdlg.cpp:1259
void FrUccDrawDlg::OnCurrentColorAreaInit(PG_MSGARG param) {
	m_pCurrentColorArea = DYNAMIC_CAST(FrArea, param);

	m_currentColor.Create(1, 1, 32);
	m_currentColor.SetPixel(0, 0, 0, 0, 0, 0xff);
	m_pCurrentColorArea->SetBgImg(&m_currentColor);
}

// uccdrawdlg.cpp:1268
void FrUccDrawDlg::OnColorListInit(intptr_t param) {
	m_pColorList = DYNAMIC_CAST(FrListBox, param);

	BuildColorList();
}

// uccdrawdlg.cpp:1275
void FrUccDrawDlg::OnColorListOwnerDraw(intptr_t param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == NULL) {
		return;
	}

	FrGraphicInterface* pGDI = GetGDI();
	if (pGDI == NULL) {
		return;
	}

	// the palette entry, inside its one-pixel border
	WRect colorRect(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f, 9.0f, 9.0f);
	pGDI->DrawTexture((Bitmap*)pItem->pData, colorRect, 0xffffffff, 0);

	if (pItem->selected) {
		WRect selectRect(pItem->pos.x - 3.0f, pItem->pos.y - 3.0f, (float)m_pSelectColor->Width(), (float)m_pSelectColor->Height());
		pGDI->DrawTexture(m_pSelectColor, selectRect, 0xffffffff, 0);
	}
}

// uccdrawdlg.cpp:1301
void FrUccDrawDlg::OnColorListBtnDown(void) {
	FrListItem* pItem = m_pColorList->GetItemUnderCursor();
	if (pItem == NULL) {
		return;
	}

	Bitmap* pColor = (Bitmap*)pItem->pData;
	if (pColor == NULL) {
		return;
	}

	unsigned char r, g, b, a;
	pColor->GetPixel(0, 0, r, g, b, a);

	if (a == 0xff) {
		SetCurrentColor(r, g, b);
	}

	if (m_brushType == 2) {
		UccCurrentBrush()->SetBrush((eBrushType)2, m_pEraserList->GetSelected()->idx);
	}
}

// uccdrawdlg.cpp:1325
void FrUccDrawDlg::OnEraserListInit(PG_MSGARG param) {
	m_pEraserList = DYNAMIC_CAST(FrListBox, param);

	BuildEraserList();
}

// uccdrawdlg.cpp:1331
void FrUccDrawDlg::OnEraserListOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == NULL) {
		return;
	}

	FrGraphicInterface* pGDI = GetGDI();
	if (pGDI == NULL) {
		return;
	}

	if (m_pEraserList->GetSelected() == NULL && pItem->idx == m_eraserIndex) {
		m_pEraserList->SelectItem(pItem, true);
	}

	if (pItem->selected) {
		if (m_brushType != 2) {
			pItem->selected = false;
		}
	}

	if (pItem->selected) {
		const Bitmap* pSelect = m_pSelectBoxYellow;
		pGDI->DrawTexture(pSelect, WRect(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f, pSelect->Width(), pSelect->Height()), 0xffffffff, 0);
	}
}

// uccdrawdlg.cpp:1364
void FrUccDrawDlg::OnEraserListBtnDown(void) {
	FrListItem* pItem = m_pEraserList->GetItemUnderCursor();
	if (pItem == NULL) {
		return;
	}
	if (pItem->pData == NULL) {
		return;
	}

	FrListItem* pSelected = m_pBrushList->GetSelected();
	if (pSelected != NULL) {
		pSelected->selected = false;
	}

	pSelected = m_pPenList->GetSelected();
	if (pSelected != NULL) {
		pSelected->selected = false;
	}

	pSelected = m_pPatternList->GetSelected();
	if (pSelected != NULL) {
		pSelected->selected = false;
		m_patternIndex = -1;
	}

	m_brushType = 2;

	UccCurrentBrush()->SetBrush((eBrushType)2, pItem->idx);
	m_overlayCursor.SetCursor((COverlayCursor::eCursorType)2);

	OnTabDrawBtnUp();
}

// uccdrawdlg.cpp:1397
void FrUccDrawDlg::OnToolInfoEraserAreaInit(PG_MSGARG param) {
	m_pToolInfoEraserArea = DYNAMIC_CAST(FrArea, param);
}

// uccdrawdlg.cpp:1402
void FrUccDrawDlg::OnBaseColorListInit(PG_MSGARG param) {
	m_pBaseColorListArea = DYNAMIC_CAST(FrArea, param);
}

// uccdrawdlg.cpp:1407
void FrUccDrawDlg::OnExtendedBasePatternListInit(PG_MSGARG param) {
	m_pExtendedBasePatternListArea = DYNAMIC_CAST(FrArea, param);
}

// uccdrawdlg.cpp:1414
void FrUccDrawDlg::OnBrushListInit(PG_MSGARG param) {
	m_pBrushList = DYNAMIC_CAST(FrListBox, param);

	BuildBrushList();
}

// uccdrawdlg.cpp:1420
void FrUccDrawDlg::OnBrushListOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == NULL) {
		return;
	}

	FrGraphicInterface* pGDI = GetGDI();
	if (pGDI == NULL) {
		return;
	}

	if (m_pBrushList->GetSelected() == NULL && m_pPenList->GetSelected() == NULL && pItem->idx == UccCurrentBrush()->m_brushIndex && UccCurrentBrush()->m_brushType == 0) {
		m_pBrushList->SelectItem(pItem, true);
	}

	if (pItem->selected) {
		if (m_brushType == 2) {
			pItem->selected = false;
		}
	}

	if (pItem->selected) {
		const Bitmap* pSelect = m_pSelectBoxYellow;
		pGDI->DrawTexture(pSelect, WRect(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f, pSelect->Width(), pSelect->Height()), 0xffffffff, 0);
	}
}

// uccdrawdlg.cpp:1454
void FrUccDrawDlg::OnBrushListBtnDown(void) {
	FrListItem* pItem = m_pBrushList->GetItemUnderCursor();
	if (pItem == NULL) {
		return;
	}
	if (pItem->pData == NULL) {
		return;
	}

	FrListItem* pSelected = m_pPenList->GetSelected();
	if (pSelected != NULL) {
		pSelected->selected = false;
	}

	m_brushType = 0;

	UccCurrentBrush()->SetBrush((eBrushType)0, pItem->idx);

	OnTabDrawBtnUp();
}

// uccdrawdlg.cpp:1475
void FrUccDrawDlg::OnPenListInit(PG_MSGARG param) {
	m_pPenList = DYNAMIC_CAST(FrListBox, param);

	BuildPenList();
}

// uccdrawdlg.cpp:1481
void FrUccDrawDlg::OnPenListOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == NULL) {
		return;
	}

	FrGraphicInterface* pGDI = GetGDI();
	if (pGDI == NULL) {
		return;
	}

	if (m_pBrushList->GetSelected() == NULL && m_pPenList->GetSelected() == NULL && pItem->idx == UccCurrentBrush()->m_brushIndex && UccCurrentBrush()->m_brushType == 1) {
		m_pPenList->SelectItem(pItem, true);
	}

	if (pItem->selected) {
		if (m_brushType == 2) {
			pItem->selected = false;
		}
	}

	if (pItem->selected) {
		const Bitmap* pSelect = m_pSelectBoxYellow;
		pGDI->DrawTexture(pSelect, WRect(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f, pSelect->Width(), pSelect->Height()), 0xffffffff, 0);
	}
}

// uccdrawdlg.cpp:1515
void FrUccDrawDlg::OnPenListBtnDown(void) {
	FrListItem* pItem = m_pPenList->GetItemUnderCursor();
	if (pItem == NULL) {
		return;
	}
	if (pItem->pData == NULL) {
		return;
	}

	FrListItem* pSelected = m_pBrushList->GetSelected();
	if (pSelected != NULL) {
		pSelected->selected = false;
	}

	m_brushType = 1;

	UccCurrentBrush()->SetBrush((eBrushType)1, pItem->idx);

	OnTabDrawBtnUp();
}

// uccdrawdlg.cpp:1536
void FrUccDrawDlg::OnToolInfoPenAreaInit(PG_MSGARG param) {
	m_pToolInfoPenArea = DYNAMIC_CAST(FrArea, param);
}

// uccdrawdlg.cpp:1541
void FrUccDrawDlg::OnToolInfoBrushAreaInit(PG_MSGARG param) {
	m_pToolInfoBrushArea = DYNAMIC_CAST(FrArea, param);
}

// uccdrawdlg.cpp:1546
void FrUccDrawDlg::OnColorListsAreaInit(PG_MSGARG param) {
	m_pColorListsArea = DYNAMIC_CAST(FrArea, param);
}

// uccdrawdlg.cpp:1553
void FrUccDrawDlg::OnArrowDownBtnInit(PG_MSGARG param) {
	m_pArrowDownBtn = DYNAMIC_CAST(FrButton, param);
}

// uccdrawdlg.cpp:1558
void FrUccDrawDlg::OnArrowDownBtnUp(void) {
	m_bPatternListExpanded = !m_bPatternListExpanded;

	if (m_bPatternListExpanded) {
		m_pPatternLeftBtn->Enable(m_patternIndex > 0);
		m_pPatternRightBtn->Enable(m_patternIndex + 6 < 24);
		m_pExtendedBasePatternListArea->SetVisible(false);

		WRect listRect = m_pPatternList->GetRect();
		listRect.h = 27.0f;
		m_pPatternList->SetRect(listRect);

		WRect arrowRect = m_pArrowDownBtn->GetRect();
		arrowRect.y -= 77.0f;
		m_pArrowDownBtn->SetRect(arrowRect);

		m_pArrowDownBtn->SetButtonImg("ucc_arrow_down_n", FrButton::NORMAL);
		m_pArrowDownBtn->SetButtonImg("ucc_arrow_down_o", FrButton::OVER);
	} else {
		m_pPatternLeftBtn->Enable(false);
		m_pPatternRightBtn->Enable(false);
		m_pExtendedBasePatternListArea->SetVisible(true);

		WRect listRect = m_pPatternList->GetRect();
		listRect.h = 101.0f;
		m_pPatternList->SetRect(listRect);

		WRect arrowRect = m_pArrowDownBtn->GetRect();
		arrowRect.y += 77.0f;
		m_pArrowDownBtn->SetRect(arrowRect);

		m_pArrowDownBtn->SetButtonImg("ucc_arrow_up_n", FrButton::NORMAL);
		m_pArrowDownBtn->SetButtonImg("ucc_arrow_up_o", FrButton::OVER);
	}

	BuildPatternList(m_patternIndex, m_bPatternListExpanded);
}

// uccdrawdlg.cpp:1600
void FrUccDrawDlg::OnArrowRightBtnInit(PG_MSGARG param) {
	m_pPatternRightBtn = DYNAMIC_CAST(FrButton, param);
}

// uccdrawdlg.cpp:1605
void FrUccDrawDlg::OnArrowRightBtnUp(void) {
	if (m_pPatternList->m_itemList.size() != 0) {
		// the items hold the pattern index itself, stored in the data pointer
		FrListItem* pFirst = m_pPatternList->m_itemList.front();

		int first = (int)(intptr_t)pFirst->pData;

		if (first + 6 < 24) {
			m_pPatternLeftBtn->Enable(first + 1 > 0);
			m_pPatternRightBtn->Enable(first + 7 < 24);

			m_pPatternList->ClearItem();
			for (int i = 0; i < 6; i++) {
				m_pPatternList->AddItem((void*)(intptr_t)(first + 1 + i));
			}
		}
	}
}

// uccdrawdlg.cpp:1629
void FrUccDrawDlg::OnArrowLeftBtnInit(PG_MSGARG param) {
	m_pPatternLeftBtn = DYNAMIC_CAST(FrButton, param);
	m_pPatternLeftBtn->Enable(false);
}

// uccdrawdlg.cpp:1635
void FrUccDrawDlg::OnArrowLeftBtnUp(void) {
	if (m_pPatternList->m_itemList.size() != 0) {
		// the items hold the pattern index itself, stored in the data pointer
		FrListItem* pFirst = m_pPatternList->m_itemList.front();

		int first = (int)(intptr_t)pFirst->pData;

		if (first != 0) {
			m_pPatternLeftBtn->Enable(first - 1 > 0);
			m_pPatternRightBtn->Enable(first + 5 < 24);

			m_pPatternList->ClearItem();
			for (int i = 0; i < 6; i++) {
				m_pPatternList->AddItem((void*)(intptr_t)(first - 1 + i));
			}
		}
	}
}

// uccdrawdlg.cpp:1659
void FrUccDrawDlg::OnDeleteBtnInit(PG_MSGARG param) {
	m_pDeleteBtn = DYNAMIC_CAST(FrButton, param);
	m_pDeleteBtn->Enable(false);
}

// uccdrawdlg.cpp:1665
void FrUccDrawDlg::OnDeleteBtnUp(void) {
}

// uccdrawdlg.cpp:1669
void FrUccDrawDlg::OnModifyBtnInit(PG_MSGARG param) {
	m_pModifyBtn = DYNAMIC_CAST(FrButton, param);
	m_pModifyBtn->Enable(false);
}

// uccdrawdlg.cpp:1675
void FrUccDrawDlg::OnModifyBtnUp(void) {
}

// uccdrawdlg.cpp:1679
void FrUccDrawDlg::OnMakeBtnInit(PG_MSGARG param) {
	m_pMakeBtn = DYNAMIC_CAST(FrButton, param);
	m_pMakeBtn->Enable(false);
}

// uccdrawdlg.cpp:1685
void FrUccDrawDlg::OnMakeBtnUp(void) {
}

// uccdrawdlg.cpp:1689
void FrUccDrawDlg::OnRateListInit(PG_MSGARG param) {
	m_pRateList = DYNAMIC_CAST(FrListBox, param);
	m_pRateList->AddItem((void*)-1);
	m_pRateList->AddItem((void*)-1);
	m_pRateList->AddItem((void*)-1);
	m_pRateList->AddItem((void*)-1);
}

// uccdrawdlg.cpp:1698
void FrUccDrawDlg::OnRateListOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == NULL) {
		return;
	}

	FrGraphicInterface* pGDI = GetGDI();
	if (pGDI == NULL) {
		return;
	}

	int rate = (pItem->idx + 1) * 2;
	char filename[40];
	sprintf(filename, "ucc_tool_scale_%d.tga", rate);
	Bitmap* pScale = g_resrcmng->LoadBitmap(filename, 0, false);

	if (pScale == NULL) {
		return;
	}

	pGDI->DrawTexture(pScale, WRect(pItem->pos.x, pItem->pos.y, pScale->Width(), pScale->Height()), 0xffffffff, 0);

	if (rate == m_patternRate) {
		const Bitmap* pSelect = m_pSelectSBox2;
		pGDI->DrawTexture(pSelect, WRect(pItem->pos.x, pItem->pos.y, (float)pSelect->Width(), (float)pSelect->Height()), 0xffffffff, 0);
	}
}

// uccdrawdlg.cpp:1734
void FrUccDrawDlg::OnRateListBtnDown(void) {
	FrListItem* pItem = m_pRateList->GetItemUnderCursor();
	if (pItem != NULL) {
		m_patternRate = (pItem->idx + 1) * 2;
		BuildCurrentPattern();
	}
}

// uccdrawdlg.cpp:1746
void FrUccDrawDlg::OnPatternListInit(PG_MSGARG param) {
	m_pPatternList = DYNAMIC_CAST(FrListBox, param);

	BuildPatternList(0, true);
}

// uccdrawdlg.cpp:1753
void FrUccDrawDlg::OnPatternListOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == NULL) {
		return;
	}

	FrGraphicInterface* pGDI = GetGDI();
	if (pGDI == NULL) {
		return;
	}

	bool bSelected = (int)(intptr_t)pItem->pData == m_patternIndex;

	if (bSelected) {
		if (m_brushType == 2) {
			pItem->selected = false;
		}
	}

	pGDI->DrawTexture(m_pToolBox, WRect(pItem->pos.x, pItem->pos.y, m_pToolBox->Width(), m_pToolBox->Height()), 0xffffffff, 0);

	const Bitmap* pPattern = &m_patternThumbnails[(int)(intptr_t)pItem->pData];
	if (pPattern != NULL) {
		if (bSelected) {
			pGDI->DrawTexture(&m_patternHighlight, WRect(pItem->pos.x, pItem->pos.y, (float)m_patternHighlight.Width(), (float)m_patternHighlight.Height()), 0xffffffff, 0);
		}

		pGDI->DrawTexture(pPattern, WRect(pItem->pos.x + 3.0f, pItem->pos.y + 3.0f, pPattern->Width(), pPattern->Height()), 0xffffffff, 0);
	}

	if (bSelected) {
		const Bitmap* pSelect = m_pSelectBox;
		pGDI->DrawTexture(pSelect, WRect(pItem->pos.x - 2.0f, pItem->pos.y - 2.0f, pSelect->Width(), pSelect->Height()), 0xffffffff, 0);
	}
}

// uccdrawdlg.cpp:1799
void FrUccDrawDlg::OnPatternListBtnDown(void) {
	FrListItem* pItem = m_pPatternList->GetItemUnderCursor();
	if (pItem != NULL) {
		// the items hold the pattern index itself, stored in the data pointer
		m_patternIndex = (int)(intptr_t)pItem->pData;

		m_brushType = 3;
		BuildCurrentPattern();

		if (!m_bPatternListExpanded) {
			OnArrowDownBtnUp();
		}
	}
}

// uccdrawdlg.cpp:1816
void FrUccDrawDlg::OnEtcInit(PG_MSGARG param) {
	m_pEtcArea = DYNAMIC_CAST(FrArea, param);
	ChangeTab(UCC_TAB_DRAW);
}

// uccdrawdlg.cpp:1822
void FrUccDrawDlg::OnEtcOwnerDraw(PG_MSGARG param) {
	// The canvas frame, around the texture of the current side.
	WRect rect = m_pCanvasWnd->GetRect();

	if (m_pUccBase[m_curSide] == NULL) {
		return;
	}

	rect.w = (float)m_pUccBase[m_curSide]->GetTexture32BPP(false)->Width();
	rect.h = (float)m_pUccBase[m_curSide]->GetTexture32BPP(false)->Height();
	rect = WRect(rect.x - 3.0f, rect.y - 3.0f, rect.w + 6.0f, rect.h + 3.0f);

	FrGraphicInterface* pGDI = GetGDI();
	pGDI->LineBox(rect, 0x33000000, 0, 0.0f);

	// The grid, one line every five zoomed pixels.
	if (m_bShowGrid) {
		float step = (float)m_pUccBase[m_curSide]->m_zoomRate * 5.0f;

		float pos;
		for (pos = rect.x; pos <= rect.x + rect.w; pos += step) {
			pGDI->Line(WPoint(pos, rect.y), WPoint(pos, rect.y + rect.h), 0x0f000000, 0x0f000000, 0);
		}

		for (pos = rect.y; pos <= rect.y + rect.h; pos += step) {
			pGDI->Line(WPoint(rect.x, pos), WPoint(rect.x + rect.w, pos), 0x0f000000, 0x0f000000, 0);
		}
	}

	// "Front" / "back" and the zoom rate.
	WPoint pos = GetRect().TopLeft() + WPoint(191.0f, 298.0f);

	pGDI->SetTextStyle(1);

	float width = pGDI->PrintText11(pos, 0, m_curSide == 0 ? "\xBE\xD5\xB8\xE9" : "\xB5\xDE\xB8\xE9", 0);

	pGDI->SetTextStyle(0);

	char text[16];
	sprintf(text, ": %d%%", m_pUccBase[m_curSide]->m_zoomRate * 100);

	pGDI->PrintText11(WPoint(pos.x + width, pos.y), 0, text, 0);

	// "Flip"
	WPoint flipPos = GetRect().TopLeft() + WPoint(26.0f, 297.0f);

	pGDI->PrintText11(flipPos, 0, "\xB5\xDA\xC1\xFD\xB1\xE2", 0);

	m_overlayCursor.Render();

	m_fadingText.Process();
}

// uccdrawdlg.cpp:1905
void FrUccDrawDlg::OnToolTipInit(intptr_t param) {
	m_pToolTipArea = DYNAMIC_CAST(FrArea, param);
}

// uccdrawdlg.cpp:1910
void FrUccDrawDlg::OnToolTipOnwerDraw(PG_MSGARG param) {
	m_toolTipManager.Process();
}

// uccdrawdlg.cpp:1918
void FrUccDrawDlg::BuildBrushList(void) {
	if (m_pBrushList != NULL) {
		m_pBrushList->ClearItem();
		for (int i = 0; i < UccBrushList((eBrushType)0)->GetBrushNum(); i++) {
			m_pBrushList->AddItem(UccBrushList((eBrushType)0)->GetBrush(i));
		}
	}
}

// uccdrawdlg.cpp:1931
void FrUccDrawDlg::BuildPenList(void) {
	if (m_pPenList != NULL) {
		m_pPenList->ClearItem();
		for (int i = 0; i < UccBrushList((eBrushType)1)->GetBrushNum(); i++) {
			m_pPenList->AddItem(UccBrushList((eBrushType)1)->GetBrush(i));
		}
	}
}

// uccdrawdlg.cpp:1944
void FrUccDrawDlg::BuildEraserList(void) {
	if (m_pEraserList != NULL) {
		m_pEraserList->ClearItem();
		for (int i = 0; i < UccBrushList((eBrushType)2)->GetBrushNum(); i++) {
			m_pEraserList->AddItem(UccBrushList((eBrushType)2)->GetBrush(i));
		}
	}
}

// uccdrawdlg.cpp:1957
void FrUccDrawDlg::BuildPatternList(int first, bool b) {
	if (m_pPatternList == NULL) {
		return;
	}

	int start = (first + 1) / 6 * 6;
	int end;

	if (b) {
		int count = UccBrushList((eBrushType)3)->GetBrushNum();
		count = count < 6 ? count : 6;

		end = start + count;

		if (end > 24) {
			start -= end;
			end = 24;
			start += end;
		}
	} else {
		start = 0;
		end = 24;
	}

	m_pPatternList->ClearItem();
	for (int i = start; i < end; i++) {
		m_pPatternList->AddItem((void*)(intptr_t)i);
	}
}

// uccdrawdlg.cpp:1992
void FrUccDrawDlg::BuildColorList(void) {
	if (m_pColorList != NULL) {
		m_pColorList->ClearItem();
		for (int i = 0; i < 18; i++) {
			m_pColorList->AddItem(&m_palette[i]);
		}
	}
}

// uccdrawdlg.cpp:2005
void FrUccDrawDlg::BuildCurrentPattern(void) {
	Bitmap* pPattern = UccBrushList((eBrushType)3)->GetBrush(m_patternIndex);
	Bitmap pattern;

	if (pPattern == NULL) {
		return;
	}

	// the tiled preview of the pattern is not built in this version
}

// uccdrawdlg.cpp:2024
void FrUccDrawDlg::ChangeTab(eUccTab tab) {
	m_tabManager.Open(tab);

	m_curTab = tab;

	if (!m_bPatternListExpanded) {
		OnArrowDownBtnUp();
	}

	switch (tab) {
	case UCC_TAB_DRAW:
		m_pColorList->Enable(true);
		m_pPickColorBtn->Enable(true);
		m_pCurrentColorBtn->Enable(true);

		if (m_pPenList->GetSelected()) {
			m_brushType = 1;
		} else if (m_pBrushList->GetSelected()) {
			m_brushType = 0;
		} else {
			// nothing selected in either list: the brush list's type too
			m_brushType = 0;
		}
		break;
	case UCC_TAB_PATTERN:
		m_pColorList->Enable(false);
		m_pPickColorBtn->Enable(false);
		m_pCurrentColorBtn->Enable(false);
		m_brushType = 3;
		break;
	}
}

// uccdrawdlg.cpp:2080
void FrUccDrawDlg::Reset(void) {
	eDrawMode mode = m_drawMode;
	sItemInfo* pItemInfo = m_pItemInfo;

	Clear();
	SetUccDrawInfo(mode, pItemInfo, true);
	ChangeTab(UCC_TAB_DRAW);

	for (int i = 1; i >= 0; i--) {
		CUccBase* pBase = m_pUccBase[i];
		if (pBase != NULL) {
			pBase->m_refreshFlags |= 2;
			m_pUccBase[i]->Process();
		}
	}
}

// uccdrawdlg.cpp:2102
void FrUccDrawDlg::CloseWaitingForm(void) {
	if (m_pWaitingForm != NULL) {
		m_pWaitingForm->Close(FrOK, true);
		m_pWaitingForm = NULL;
		m_waitingTime = 0;
		m_bSavingTemporarily = false;
	}
}

// uccdrawdlg.cpp:2113
void FrUccDrawDlg::SetToolbarStatus(FrButton* pButton) {
	m_pGridBtn->SetStatus(FrButton::NORMAL);
	m_pDragBtn->SetStatus(FrButton::NORMAL);
	m_pZoomInBtn->SetStatus(FrButton::NORMAL);
	m_pZoomOutBtn->SetStatus(FrButton::NORMAL);
	if (m_pDrawBtn != NULL) {
		m_pDrawBtn->SetStatus(FrButton::NORMAL);
	}
	if (m_pTabPatternBtn != NULL) {
		m_pTabPatternBtn->SetStatus(FrButton::NORMAL);
	}
	if (m_pTabEmoticonBtn != NULL) {
		m_pTabEmoticonBtn->SetStatus(FrButton::NORMAL);
	}
	if (m_pTabTextBtn != NULL) {
		m_pTabTextBtn->SetStatus(FrButton::NORMAL);
	}

	if (pButton != NULL) {
		pButton->SetStatus(FrButton::PRESSED);
	}
}

// uccdrawdlg.cpp:2134
bool FrUccDrawDlg::IsSavingTemporarily(void) {
	return strlen(m_name) == 0;
}
