// The JP 983 game's common use-item and notice dialogs (JpCommonDialogs.h),
// from the JP exe's bodies (node scripts/disasm.ts --jp 0x<va>), in address
// order. Their creators use RFManager.h's CreateWnd<T> / CreateDesktopWnd<T>
// (instances 0x5ccde0 / 0x5cce70 for RFUseItemDlg, 0x5ccf00 / 0x5ccf90 for
// RFNoticeDlg); their handlers are bound through
// MemberFunctionFunctor<RFUseItemDlg> (vtable 0xd107d0) and
// MemberFunctionFunctor<RFNoticeDlg> (vtable 0xd107e8).
// lib: 0x5ccc60, 0x5ccd60 (this-adjusting thunks of the RFModalRenderer
// sub-objects' destructors), 0x5cd140, 0x5cd170 (scalar deleting
// destructors), 0x5ccde0, 0x5cce70, 0x5ccf00, 0x5ccf90 (CreateWnd<T> /
// CreateDesktopWnd<T>), 0x5cd020, 0x5cd050, 0x5cd070, 0x5cd080, 0x5cd090,
// 0x5cd0a0, 0x5cd0d0, 0x5cd0f0, 0x5cd100, 0x5cd110, 0x5cd260, 0x5cd290
// (MemberFunctionFunctor<T> members), 0x5cd120, 0x5cd130 (inlined
// ~EventFunctorBase), 0x5cd1a0, 0x5cd200 (Subscriber's template
// constructor).
#include "JpCommonDialogs.h"

#include <string>

#include "commonutil.h"
#include "SharedDoc.h"
#include "SoundManager.h"

#include "JpLobbyDialogs.h"
#include "RFButton.h"
#include "RFLabel.h"
#include "RFManager.h"
#include "RFPicture.h"
#include "RFTextArea.h"
#include "RefreshStringUtil.h"

// 0xe78174
RFUseItemDlg* g_useItemDlg = NULL;
// 0xe78170
RFWnd* g_useItemDlgParent = NULL;
// 0xe78178
RFWnd* g_noticeDlgParent = NULL;

// 0x5ccba0
void RFUseItemDlg::OnOkClicked(EventArgs& args) {
	Close(static_cast<RFWNDCLOSESTATE>(1));
}

// 0x5ccbb0
void RFUseItemDlg::OnCancelClicked(EventArgs& args) {
	Close(static_cast<RFWNDCLOSESTATE>(2));
}

// 0x5ccbc0
void RFNoticeDlg::OnOkClicked(EventArgs& args) {
	Close(static_cast<RFWNDCLOSESTATE>(1));
}

// 0x5ccbd0
RFUseItemDlg::RFUseItemDlg() {
	g_useItemDlg = this;
	m_itemPicture = NULL;
	m_itemCountLabel = NULL;
	m_messageText = NULL;
}

// 0x5ccc70
RFUseItemDlg::~RFUseItemDlg() {
	g_useItemDlg = NULL;
}

// 0x5ccce0
RFNoticeDlg::RFNoticeDlg()
	: m_messageLabel(NULL) {
}

// 0x5ccd70
RFNoticeDlg::~RFNoticeDlg() {
}

// 0x5cd2c0: the IFF record is read unchecked, as in the exe. JP +0x49 is
// the KR sItem::m_icon (the JP name field is 0x18 bytes longer).
void RFUseItemDlg::SetItem(sItemInfo const* item) {
	if (item == NULL) {
		return;
	}
	if (m_itemPicture != NULL) {
		IFF_STRUCT::sItem* record = ItemManager()->FindItem(item->m_typeId);
		m_itemPicture->SetPicture(ShiftJisToWString(std::string(MakeStr("%s.tga", record->m_icon))));
	}
	if (m_itemCountLabel != NULL) {
		m_itemCountLabel->SetLabel(ShiftJisToWString(std::string(MakeStr("x%d", item->m_count))));
	}
}

// 0x5cd470: JP +0x49 / +0xec are taken as the KR sItem's m_icon and
// quantity (0x31 / 0xbc there; the JP record's name and model strings are
// longer).
void RFUseItemDlg::SetItem(IFF_STRUCT::sItem const* item) {
	if (item == NULL) {
		return;
	}
	if (m_itemPicture != NULL) {
		m_itemPicture->SetPicture(ShiftJisToWString(std::string(MakeStr("%s.tga", item->m_icon))));
	}
	if (m_itemCountLabel != NULL) {
		m_itemCountLabel->SetLabel(ShiftJisToWString(std::string(MakeStr("x%d", item->quantity))));
	}
}

// 0x5cd610
void RFNoticeDlg::SetMessage(char const* text) {
	if (m_messageLabel != NULL) {
		m_messageLabel->SetLabel(ShiftJisToWString(std::string(text)));
	}
}

// 0x5cd6f0 (parent in ecx)
RFWnd* CreateUseItemDialog(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpModalCreateMode) {
	case 0:
		wnd = CreateWnd<RFUseItemDlg>(&RFManager::GetInstance(), L"common_use_item_dialog.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFUseItemDlg>(&RFManager::GetInstance(), L"common_use_item_dialog.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFUseItemDlg>(&RFManager::GetInstance(), L"common_use_item_dialog.xml");
		break;
	}
	g_useItemDlgParent = parent;
	return wnd;
}

// 0x5cd830
void RFUseItemDlg::SetMessage(std::wstring const& text) {
	if (m_messageText != NULL) {
		m_messageText->SetText(std::wstring(text.c_str()));
	}
}

// 0x5cd900
void RFUseItemDlg::SetMessage(char const* text) {
	if (m_messageText != NULL) {
		m_messageText->SetText(std::wstring(ShiftJisToWString(std::string(text)).c_str()));
	}
}

// 0x5cda40: the sound goes through the game's sound manager (0xf023f4,
// read unchecked) with the constants 0xd66d08 (0.5f) and 0xd00d90 (200.0f).
void RFUseItemDlg::OnInitialUpdate() {
	RegisterTo(L"ok_btn", RFButton::EventClicked, Subscriber(&RFUseItemDlg::OnOkClicked, this));
	RegisterTo(L"cancel_btn", RFButton::EventClicked, Subscriber(&RFUseItemDlg::OnCancelClicked, this));
	m_itemPicture = dynamic_cast<RFPicture*>(GetChild(L"item_img"));
	m_itemCountLabel = dynamic_cast<RFLabel*>(GetChild(L"item_num_label"));
	m_messageText = dynamic_cast<RFTextArea*>(GetChild(L"message"));
	g_audio->PlaySfx("ui_dialog_open", NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

// 0x5cdd60 (parent in ecx)
RFWnd* CreateNoticeDialog(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpModalCreateMode) {
	case 0:
		wnd = CreateWnd<RFNoticeDlg>(&RFManager::GetInstance(), L"common_notice_dialog.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFNoticeDlg>(&RFManager::GetInstance(), L"common_notice_dialog.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFNoticeDlg>(&RFManager::GetInstance(), L"common_notice_dialog.xml");
		break;
	}
	g_noticeDlgParent = parent;
	return wnd;
}

// 0x5cdea0
void RFNoticeDlg::OnInitialUpdate() {
	RegisterTo(L"ok_btn", RFButton::EventClicked, Subscriber(&RFNoticeDlg::OnOkClicked, this));
	m_messageLabel = dynamic_cast<RFLabel*>(GetChild(L"message"));
}
