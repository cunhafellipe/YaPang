// The JP 983 character mastery's status page (JP-only, no KR counterpart;
// R558 screen "Mastery stat", 0x5c6400-0x5ccbd0): the status dialog
// RFCharMasteryStatusDlg (kam_mastery_status.xml, created by the mastery
// main dialog's CreateStatusDlg 0x5be6c0) fills its status_list with five
// RFCharMasteryStatColumn (kam_status_column.xml), one per stat; each
// column draws its stat as a MasteryGaugeBar and opens its
// RFCharMasteryStatDetailWnd (kam_status_detail_wnd.xml, JpMasteryStatDetail.*,
// another owner) on its status_button. RTTI names read from the vtables'
// complete object locators; declarations and the JP x86 layout from the JP
// exe's bodies (node scripts/disasm.ts --jp 0x<va>). The mastery game state
// is reached through the declared-only glue below (JP VA in its comment),
// never emulated.
#ifndef PG_JP_JPMASTERYSTATUS_H
#define PG_JP_JPMASTERYSTATUS_H

#include <stdint.h>
#include <string>

#include "defines.h"
#include "padding.h"
#include "JpMasteryMainDlg.h"
#include "RFFormBase.h"

class RFButton;
class RFCharMasteryStatDetailWnd;
class RFList;
class RFPicture;
class RFWnd;

// A stat's gauge (RTTI MasteryGaugeBar, vtable 0xd0fc78, one slot), built
// by the column's OnInitialUpdate from its three status_bar_* pictures.
// Size 0x18. Not a window: a plain object the column owns.
class MasteryGaugeBar {
	LAYOUT_CHECK_FRIEND;
public:
	enum Part {
		Part_Stat = 0,  // status_bar_stat, the gauge itself
		Part_Slot = 1,  // status_bar_slot
		Part_Bg = 2,    // status_bar_bg
		PartCount = 3
	};

	// Inlined in RFCharMasteryStatColumn::OnInitialUpdate: the three parts
	// kept, m_value = 0, m_fullWidth = the stat picture's width, the three
	// parts shown.
	MasteryGaugeBar(RFPicture* stat, RFPicture* slot, RFPicture* bg);
	// [0] 0x5c63d0 (scalar deleting destructor; the destructor is empty).
	virtual ~MasteryGaugeBar();

	// 0x5c7610 (name ours; part passed in a register under /LTCG): the
	// picture of part (0..2) set to image.
	void SetPartPicture(std::wstring image, uint32_t part);
	// Inlined in RFCharMasteryStatColumn::UpdateGaugeValues 0x5c5d60 (names
	// ours): the gauge's full-scale value, and part's width set to
	// m_fullWidth * value / m_value, at most m_fullWidth (nothing while
	// m_fullWidth is 0).
	void SetMaxValue(int32_t value) { m_value = value; }
	void SetPartValue(uint32_t part, int32_t value);

protected:
	RFPicture* m_parts[PartCount];  // 0x4 stat, slot, bg
	int32_t m_value;                // 0x10 0 at construction
	float m_fullWidth;              // 0x14 the stat picture's width at construction
};

// One stat of the status dialog (RTTI RFCharMasteryStatColumn :
// RFFormBase<RFWndEx>, vtable 0xd0fdb4, 34 slots: no new virtuals), loaded
// from kam_status_column.xml by CreateCharMasteryStatColumn 0x5c9580.
// Size 0x198.
class RFCharMasteryStatColumn : public RFFormBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	enum {
		NoStat = 5  // m_index at construction: no stat yet
	};

	// 0x5c6680: RFWndEx::RFWndEx, the members cleared, m_index = NoStat.
	RFCharMasteryStatColumn();
	// [1] 0x5c7210 (scalar deleting destructor); ~RFCharMasteryStatColumn
	// 0x5c66c0 deletes m_gauge, then restores RFFormBase<RFWndEx>'s vtable
	// and runs ~RFWndEx.
	virtual ~RFCharMasteryStatColumn();

	// 0x5cca30 (name ours): m_index = index, then the button images, the
	// gauge and max images, the detail window's stat and the values.
	void SetIndex(int32_t index);
	// 0x5cc970 (name ours): UpdateGaugeValues, then UpdateDetailValues
	// (RFCharMasteryStatusDlg::RefreshCardList runs it on every column).
	void UpdateValues();

	// 0xe7814c (name ours): the parent the last
	// CreateCharMasteryStatColumn was given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5cacd0: the three status_button handlers, the button, max
	// picture and gauge, the detail window over the placeholder
	// status_detail_wnd (deleted afterwards).
	virtual void OnInitialUpdate();

	// 0x5c7be0 (name ours): with a stat, status_button's four images
	// "button_mastery_stat_<index>_n.tga".
	void UpdateButtonImages();
	// 0x5c7d20 (name ours): with a stat, the gauge's stat picture
	// "mastery_stat_bar_<index>.tga" and max_img "max_60.tga" (stat 0) or
	// "max_30.tga".
	void UpdateGaugeImages();
	// 0x5c96c0 (name ours): the detail window given m_index.
	void UpdateDetailStat();
	// 0x5c5d60 (name ours): with a stat and a gauge, the gauge's m_value = 60
	// (stat 0) or 30, and its stat and slot pictures sized to the stat
	// value and value + free points (SetPartValue).
	void UpdateGaugeValues();
	// 0x5cbcb0 (name ours): the detail window's values for m_index.
	void UpdateDetailValues();
	// status_button's handlers (names ours): the detail window's
	// animations while it is closed, and the open/close toggle.
	void OnButtonMouseEnter(EventArgs& args);  // 0x5c96e0 "_over_status_detail_"
	void OnButtonMouseLeave(EventArgs& args);  // 0x5c9790 "_out_status_detail_"
	void OnButtonClicked(EventArgs& args);     // 0x5c9840 open / close

	// 0x0-0x184: RFFormBase<RFWndEx> / RFWndEx.
	int32_t m_index;                     // 0x184 the stat (0..4), NoStat at construction
	RFButton* m_button;                  // 0x188 child "status_button"
	RFPicture* m_maxImg;                 // 0x18c child "max_img"
	MasteryGaugeBar* m_gauge;            // 0x190 owned, deleted by the destructor
	RFCharMasteryStatDetailWnd* m_detailWnd;  // 0x194 CreateMasteryStatDetailWnd 0x5c9020
};

// The status page (RTTI RFCharMasteryStatusDlg : RFFormBase<RFWndEx>,
// vtable 0xd0fe44, 34 slots: no new virtuals), loaded from
// kam_mastery_status.xml by CreateCharMasteryStatusDlg 0x5c99c0. Size 0x188.
class RFCharMasteryStatusDlg : public RFFormBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	enum {
		StatCount = 5
	};

	// 0x5c6740: RFWndEx::RFWndEx, g_charMasteryStatusDlg = this,
	// m_statusList = NULL.
	RFCharMasteryStatusDlg();
	// [1] 0x5c7240 (scalar deleting destructor); ~RFCharMasteryStatusDlg
	// 0x5c6770 clears g_charMasteryStatusDlg, then restores
	// RFFormBase<RFWndEx>'s vtable and runs ~RFWndEx.
	virtual ~RFCharMasteryStatusDlg();

	// 0x5cc980 (name ours; TaskManager's JP case 0x271/0x272 calls it):
	// the card manager's mastery stats refreshed
	// (Jp_CardManager_RefreshMasteryStats), MsgObject{0, 0x109} sent to the
	// current task's main actor, then UpdateValues on every column of
	// m_statusList.
	void RefreshCardList(void);

	// 0xe78154 (name ours): the parent the last CreateCharMasteryStatusDlg
	// was given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5ccb60: the card manager's mastery stats refreshed
	// (Jp_CardManager_RefreshMasteryStats), then InitStatusList.
	virtual void OnInitialUpdate();

	// 0x5cca60 (name ours): m_statusList = the RFList child "status_list";
	// one column per stat, given its index, shown and inserted.
	void InitStatusList();

	// 0x0-0x184: RFFormBase<RFWndEx> / RFWndEx.
	RFList* m_statusList;  // 0x184 child "status_list"
};

// 0x5c9580 (parent in ecx): kam_status_column.xml, mode g_jpWndCreateMode
// (0xe73dd8); the parent is kept in RFCharMasteryStatColumn::s_loadParent.
RFWnd* CreateCharMasteryStatColumn(RFWnd* parent);
// 0x5c99c0 (parent in ecx): kam_mastery_status.xml, mode g_jpWndCreateMode;
// the parent is kept in RFCharMasteryStatusDlg::s_loadParent (the mastery
// main dialog's CreateStatusDlg 0x5be6c0 calls it).
RFWnd* CreateCharMasteryStatusDlg(RFWnd* parent);

// 0xe78158 (name ours): the status page, set by its constructor and
// cleared by its destructor.
extern RFCharMasteryStatusDlg* g_charMasteryStatusDlg;

// --- Declared-only glue (JP callees outside this class pair; never emulated) ---

// 0x80aac0 on the JP CCardManager instance (0xe74210; name ours): the
// equipped character's mastery stats recomputed from its cards (read from
// the CSharedDoc 0xe47dbc).
void Jp_CardManager_RefreshMasteryStats();

CHECK_SIZE(MasteryGaugeBar, 0x18);
BEGIN_LAYOUT_CHECK(MasteryGaugeBar)
	CHECK_FIELD(MasteryGaugeBar, m_parts, 0x4);
	CHECK_FIELD(MasteryGaugeBar, m_value, 0x10);
	CHECK_FIELD(MasteryGaugeBar, m_fullWidth, 0x14);
END_LAYOUT_CHECK

CHECK_SIZE(RFCharMasteryStatColumn, 0x198);
BEGIN_LAYOUT_CHECK(RFCharMasteryStatColumn)
	CHECK_FIELD(RFCharMasteryStatColumn, m_index, 0x184);
	CHECK_FIELD(RFCharMasteryStatColumn, m_button, 0x188);
	CHECK_FIELD(RFCharMasteryStatColumn, m_maxImg, 0x18c);
	CHECK_FIELD(RFCharMasteryStatColumn, m_gauge, 0x190);
	CHECK_FIELD(RFCharMasteryStatColumn, m_detailWnd, 0x194);
END_LAYOUT_CHECK

CHECK_SIZE(RFCharMasteryStatusDlg, 0x188);
BEGIN_LAYOUT_CHECK(RFCharMasteryStatusDlg)
	CHECK_FIELD(RFCharMasteryStatusDlg, m_statusList, 0x184);
END_LAYOUT_CHECK

#endif // PG_JP_JPMASTERYSTATUS_H
