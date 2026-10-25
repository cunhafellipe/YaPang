// The JP 983 character mastery's status page (JpMasteryStatus.h), from the
// JP exe's bodies (node scripts/disasm.ts --jp 0x<va>), in address order.
// The creators use RFManager.h's CreateWnd<T> / CreateDesktopWnd<T>
// (RFCharMasteryStatusDlg's instances 0x5c6cd0 / 0x5c6d60, the column's
// 0x5c6bb0 / 0x5c6c40).
// lib: 0x5c7210, 0x5c7240 (scalar deleting destructors)
#include "JpMasteryStatus.h"

#include <string>

#include "JpMasteryStatDetail.h"
#include "JpMasterySlot.h"
#include "JpTooltipWnd.h"
#include "RFButton.h"
#include "RFEventArgs.h"
#include "RFList.h"
#include "RFManager.h"
#include "RFPicture.h"
#include "RFSize.h"
#include "RFVector2.h"
#include "RefreshStringUtil.h"
#include "SoundManager.h"
#include "TaskManager.h"
#include "actor.h"
#include "commonutil.h"

// 0xe78158
RFCharMasteryStatusDlg* g_charMasteryStatusDlg = NULL;
// 0xe78154
RFWnd* RFCharMasteryStatusDlg::s_loadParent = NULL;
// 0xe7814c
RFWnd* RFCharMasteryStatColumn::s_loadParent = NULL;

// The UI sounds the column's handler plays through the game's sound
// manager (0xf023f4, slot 3; constants 0xd66d08 0.5f and 0xd00d90 200.0f).
static void PlayMasterySound(const char* name) {
	g_audio->PlaySfx(name, NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

// Inlined in RFCharMasteryStatColumn::OnInitialUpdate (0x5cb115).
MasteryGaugeBar::MasteryGaugeBar(RFPicture* stat, RFPicture* slot, RFPicture* bg) {
	m_parts[Part_Slot] = slot;
	m_parts[Part_Stat] = stat;
	m_parts[Part_Bg] = bg;
	m_value = 0;
	m_fullWidth = m_parts[Part_Stat]->GetSize().width;
	m_parts[Part_Stat]->ShowWindow(true);
	m_parts[Part_Slot]->ShowWindow(true);
	m_parts[Part_Bg]->ShowWindow(true);
}

// 0x5c5d60
void RFCharMasteryStatColumn::UpdateGaugeValues() {
	if (m_index == NoStat) {
		return;
	}
	JpMasteryStatInfo info;
	if (m_gauge != NULL) {
		int32_t value = info.GetStatValue(m_index);
		int32_t freePoints = info.GetFreeMasteryPoints(m_index);
		m_gauge->SetMaxValue(m_index == 0 ? 60 : 30);
		m_gauge->SetPartValue(MasteryGaugeBar::Part_Stat, value);
		m_gauge->SetPartValue(MasteryGaugeBar::Part_Slot, value + freePoints);
	}
}

// 0x5c63d0 (scalar deleting destructor)
MasteryGaugeBar::~MasteryGaugeBar() {
}

// 0x5c6680
RFCharMasteryStatColumn::RFCharMasteryStatColumn()
	: m_button(NULL), m_maxImg(NULL), m_gauge(NULL), m_detailWnd(NULL) {
	m_index = NoStat;
}

// 0x5c66c0
RFCharMasteryStatColumn::~RFCharMasteryStatColumn() {
	if (m_gauge != NULL) {
		delete m_gauge;
		m_gauge = NULL;
	}
}

// 0x5c6740
RFCharMasteryStatusDlg::RFCharMasteryStatusDlg() {
	g_charMasteryStatusDlg = this;
	m_statusList = NULL;
}

// 0x5c6770
RFCharMasteryStatusDlg::~RFCharMasteryStatusDlg() {
	g_charMasteryStatusDlg = NULL;
}

// 0x5c7610
void MasteryGaugeBar::SetPartPicture(std::wstring image, uint32_t part) {
	if (part <= Part_Bg) {
		RFPicture* picture = m_parts[part];
		if (picture != NULL) {
			picture->SetPicture(image);
		}
	}
}

// Inlined twice in RFCharMasteryStatColumn::UpdateGaugeValues (0x5c5df9,
// 0x5c5e4c).
void MasteryGaugeBar::SetPartValue(uint32_t part, int32_t value) {
	if (m_fullWidth != 0.0f) {
		float ratio = static_cast<float>(value) / m_value;
		if (ratio > 1.0f) {
			ratio = 1.0f;
		}
		RFSize size = m_parts[part]->GetSize();
		size.width = m_fullWidth * ratio;
		m_parts[part]->SetSize(size);
	}
}

// 0x5c7be0
void RFCharMasteryStatColumn::UpdateButtonImages() {
	if (m_index == NoStat || m_button == NULL) {
		return;
	}
	std::string name("");
	name = MakeStr("button_mastery_stat_%d_n.tga", m_index);
	m_button->ChangeNormalButtonImage(ShiftJisToWString(name));
	m_button->ChangeDisableButtonImage(ShiftJisToWString(name));
	m_button->ChangeOverButtonImage(ShiftJisToWString(name));
	m_button->ChangeDownButtonImage(ShiftJisToWString(name));
}

// 0x5c7d20
void RFCharMasteryStatColumn::UpdateGaugeImages() {
	if (m_index == NoStat) {
		return;
	}
	if (m_gauge != NULL) {
		std::string name(MakeStr("mastery_stat_bar_%d.tga", m_index));
		m_gauge->SetPartPicture(ShiftJisToWString(name), MasteryGaugeBar::Part_Stat);
	}
	if (m_maxImg != NULL) {
		std::string name(MakeStr("max_%d.tga", m_index == 0 ? 60 : 30));
		m_maxImg->SetPicture(ShiftJisToWString(name));
	}
}

// 0x5c9580
RFWnd* CreateCharMasteryStatColumn(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFCharMasteryStatColumn>(&RFManager::GetInstance(), L"kam_status_column.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFCharMasteryStatColumn>(&RFManager::GetInstance(), L"kam_status_column.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFCharMasteryStatColumn>(&RFManager::GetInstance(), L"kam_status_column.xml");
		break;
	}
	RFCharMasteryStatColumn::s_loadParent = parent;
	return wnd;
}

// 0x5c96c0
void RFCharMasteryStatColumn::UpdateDetailStat() {
	if (m_detailWnd != NULL) {
		m_detailWnd->SetStat(m_index);
	}
}

// 0x5c96e0
void RFCharMasteryStatColumn::OnButtonMouseEnter(EventArgs& args) {
	if (m_detailWnd != NULL && m_detailWnd->GetDetailOpen() == 0) {
		m_detailWnd->PlayAnimation(L"_over_status_detail_");
	}
}

// 0x5c9790
void RFCharMasteryStatColumn::OnButtonMouseLeave(EventArgs& args) {
	if (m_detailWnd != NULL && m_detailWnd->GetDetailOpen() == 0) {
		m_detailWnd->PlayAnimation(L"_out_status_detail_");
	}
}

// 0x5c9840
void RFCharMasteryStatColumn::OnButtonClicked(EventArgs& args) {
	if (m_detailWnd == NULL) {
		return;
	}
	if (m_detailWnd->GetDetailOpen() == 1) {
		m_detailWnd->SetDetailOpen(0);
		m_detailWnd->PlayAnimation(L"_close_status_detail_");
		PlayMasterySound("mastery_stat_closed");
	} else {
		m_detailWnd->SetDetailOpen(1);
		m_detailWnd->PlayAnimation(L"_push_status_detail_");
		PlayMasterySound("mastery_stat_open");
	}
}

// 0x5c99c0
RFWnd* CreateCharMasteryStatusDlg(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFCharMasteryStatusDlg>(&RFManager::GetInstance(), L"kam_mastery_status.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFCharMasteryStatusDlg>(&RFManager::GetInstance(), L"kam_mastery_status.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFCharMasteryStatusDlg>(&RFManager::GetInstance(), L"kam_mastery_status.xml");
		break;
	}
	RFCharMasteryStatusDlg::s_loadParent = parent;
	return wnd;
}

// 0x5cacd0
void RFCharMasteryStatColumn::OnInitialUpdate() {
	RegisterTo(L"status_button", RFWnd::EventMouseEnter, Subscriber(&RFCharMasteryStatColumn::OnButtonMouseEnter, this));
	RegisterTo(L"status_button", RFWnd::EventMouseLeave, Subscriber(&RFCharMasteryStatColumn::OnButtonMouseLeave, this));
	RegisterTo(L"status_button", RFButton::EventClicked, Subscriber(&RFCharMasteryStatColumn::OnButtonClicked, this));
	m_button = dynamic_cast<RFButton*>(GetChild(L"status_button"));
	m_maxImg = dynamic_cast<RFPicture*>(GetChild(L"max_img"));
	RFPicture* stat = dynamic_cast<RFPicture*>(GetChild(L"status_bar_stat"));
	RFPicture* slot = dynamic_cast<RFPicture*>(GetChild(L"status_bar_slot"));
	RFPicture* bg = dynamic_cast<RFPicture*>(GetChild(L"status_bar_bg"));
	m_gauge = new MasteryGaugeBar(stat, slot, bg);

	RFWndEx* placeholder = dynamic_cast<RFWndEx*>(GetChild(L"status_detail_wnd"));
	if (placeholder != NULL) {
		m_detailWnd = static_cast<RFCharMasteryStatDetailWnd*>(CreateMasteryStatDetailWnd(this));
		if (m_detailWnd != NULL) {
			m_detailWnd->SetPos(placeholder->GetPos().x, placeholder->GetPos().y);
			m_detailWnd->ShowForm(true);
		}
		DeleteChild(placeholder);
	}
}

// 0x5cbcb0
void RFCharMasteryStatColumn::UpdateDetailValues() {
	if (m_detailWnd != NULL) {
		m_detailWnd->UpdateValues(m_index);
	}
}

// 0x5cc970
void RFCharMasteryStatColumn::UpdateValues() {
	UpdateGaugeValues();
	UpdateDetailValues();
}

// 0x5cc980
void RFCharMasteryStatusDlg::RefreshCardList(void) {
	Jp_CardManager_RefreshMasteryStats();
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x109, 0, 0, 0, 0, 0));
	if (m_statusList == NULL) {
		return;
	}
	int32_t count = m_statusList->GetItemCount();
	for (int32_t i = 0; i < count; ++i) {
		RFCharMasteryStatColumn* column = static_cast<RFCharMasteryStatColumn*>(m_statusList->GetItemData(i));
		if (column != NULL) {
			column->UpdateValues();
		}
	}
}

// 0x5cca30
void RFCharMasteryStatColumn::SetIndex(int32_t index) {
	m_index = index;
	UpdateButtonImages();
	UpdateGaugeImages();
	UpdateDetailStat();
	UpdateValues();
}

// 0x5cca60
void RFCharMasteryStatusDlg::InitStatusList() {
	m_statusList = dynamic_cast<RFList*>(GetChild(L"status_list"));
	if (m_statusList == NULL) {
		return;
	}
	for (int32_t i = 0; i < StatCount; ++i) {
		RFCharMasteryStatColumn* column = static_cast<RFCharMasteryStatColumn*>(CreateCharMasteryStatColumn(m_statusList));
		if (column != NULL) {
			column->SetIndex(i);
			column->ShowForm(true);
			m_statusList->InsertWindowItem(column);
		}
	}
}

// 0x5ccb60
void RFCharMasteryStatusDlg::OnInitialUpdate() {
	Jp_CardManager_RefreshMasteryStats();
	InitStatusList();
}
