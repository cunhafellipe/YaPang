// JP-only game code: FrChatDlg, the JP lobby/room chat panel form (base of
// FrRoomListDlg and FrGameRoomDlg). See JpChatDlg.h.
#include "JpChatDlg.h"

#include <string.h>

#include <algorithm>
#include <list>
#include <string>

#include "ChatMsg.h"
#include "EmoticonDlg.h"
#include "InputManager.h"
#include "JpGameRoomDlg.h"
#include "JpLobbyGlue.h"
#include "ObjectFactory.h"
#include "RFTextManager.h"
#include "ReportDlg.h"
#include "SharedDoc.h"
#include "commonutil.h"
#include "frarea.h"
#include "frbutton.h"
#include "frcombobox.h"
#include "fredit.h"
#include "fresh.h"
#include "frscrollbar.h"
#include "frwnd.h"
#include "frwndmanager.h"
#include "messengerdlg.h"
#include "wfont.h"
#include "woverlay.h"
#include "wview.h"

// lib: 0x48f3f0, 0x48f400 (this-adjusting deleting-destructor thunks),
// 0xcc8630 (message-map initializer), 0x490a80 (std::list clear)

const WRTTI FrChatDlg::m_RTTI("FrChatDlg", &FrForm::m_RTTI);

// 0x48f0a0
const sFRESH_MSGMAP* FrChatDlg::GetMessageMap(void) const {
	return &FrChatDlg::_MsgMap;
}

sFRESH_MSGMAP FrChatDlg::_MsgMap = { &FrForm::_MsgMap, &FrChatDlg::_MsgEntries[0] };

sFRESH_ENTRY FrChatDlg::_MsgEntries[] = {
	{ "chat_group.chatbg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChatDlg::OnChatBgInit },
	{ "chat_group.chatbg", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChatDlg::OnChatBgOwnerDraw },
	{ "chat_group.chatwndbtn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChatDlg::OnChatWndBtnInit },
	{ "chat_group.chatwndbtn", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrChatDlg::OnChatWndBtnLBtnDown },
	{ "chat_group.emoticon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChatDlg::OnEmoticonInit },
	{ "chat_group.emoticon", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrChatDlg::OnEmoticonLBtnDown },
	{ "chat_group.language", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChatDlg::OnLanguageInit },
	{ "chat_group.chatinput", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChatDlg::OnChatInputInit },
	{ "chat_group.chatinput", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&FrChatDlg::OnChatInputEnterKey },
	{ "chat_group.chattarget", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChatDlg::OnChatTargetInit },
	{ "chat_group.chattarget", FRCMD_LBUTTONDOWN, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChatDlg::OnChatTargetLBtnDown },
	{ "chat_group.chattarget", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&FrChatDlg::OnChatTargetEnterKey },
	{ "chat_group.chatview", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChatDlg::OnChatViewInit },
	{ "chat_group.report", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrChatDlg::OnReportLBtnDown },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x48f0b0
FrChatDlg::FrChatDlg(void) {
	m_pEmoticonDlg = 0;
	m_pReportDlg = 0;
	m_pChatBg = 0;
	m_pChatWndBtn = 0;
	m_pLanguage = 0;
	m_pChatInput = 0;
	m_pChatView = 0;
	m_pChatTarget = 0;
	m_pEmoticon = 0;
}

// 0x48f120
FrChatDlg::~FrChatDlg(void) {
}

// 0x48f140
bool FrChatDlg::OnInit(void) {
	return FrForm::OnInit();
}

// 0x48f1a0
bool FrChatDlg::OnReportDlgResult(int ret, FrForm* pForm) {
	m_pReportDlg = 0;
	return true;
}

// 0x48f1b0
void FrChatDlg::OnChatTargetLBtnDown(intptr_t param) {
	m_pChatInput->SetKeyFocus(false);
}

// 0x48f1c0
bool FrChatDlg::OnChatTargetEnterKey(intptr_t param) {
	m_pChatInput->SetKeyFocus(true);
	return false;
}

// 0x48f520
IObject* FrChatDlg::Create(void) {
	return new FrChatDlg;
}

// 0x48f590 (static init 0xcc8610): registration with the form factory
struct __sFrChatDlg {
	__sFrChatDlg(void) {
		ObjectFactory().AddObjectFunctor(FrChatDlg::Create, "FrChatDlg");
	}
};
static __sFrChatDlg s_registerFrChatDlg;

// 0x48f5e0
void FrChatDlg::OnChatBgInit(intptr_t param) {
	m_pChatBg = DYNAMIC_CAST(FrArea, param);
}

// 0x48f650
void FrChatDlg::OnChatBgOwnerDraw(intptr_t param) {
	if (g_pFresh->GetManager()->GetGDI()) {
		const WRect& rect = m_pChatBg->GetRect();
		WOverlay::DrawBox(g_view, WRect(rect.x, rect.y, rect.w, rect.h), 0, 0xa0000000, 0.0f);
	}
}

// 0x48f6c0
void FrChatDlg::ToggleChatBtn(bool bMove) {
	if (!JpOption_GetChatWndDown()) {
		m_pChatWndBtn->SetButtonImg("chatwnd_up", FrButton::NORMAL);
		if (!bMove) {
			return;
		}
		const WRect& bgRect = m_pChatBg->GetRect();
		m_pChatBg->SetRect(WRect(bgRect.x, bgRect.y + 200.0, bgRect.w, bgRect.h - 200.0));
		const WRect& viewRect = m_pChatView->GetRect();
		m_pChatView->SetRect(WRect(viewRect.x, viewRect.y + 200.0, viewRect.w, viewRect.h - 200.0));
	} else {
		m_pChatWndBtn->SetButtonImg("chatwnd_down", FrButton::NORMAL);
		if (!bMove) {
			return;
		}
		const WRect& bgRect = m_pChatBg->GetRect();
		m_pChatBg->SetRect(WRect(bgRect.x, bgRect.y - 200.0, bgRect.w, 200.0 + bgRect.h));
		const WRect& viewRect = m_pChatView->GetRect();
		m_pChatView->SetRect(WRect(viewRect.x, viewRect.y - 200.0, viewRect.w, 200.0 + viewRect.h));
	}
	if (m_pChatView->GetScrollBar()) {
		m_pChatView->GetScrollBar()->ScrollToBottom();
	}
}

// 0x48f7f0
void FrChatDlg::OnLanguageInit(intptr_t param) {
	m_pLanguage = DYNAMIC_CAST(FrArea, param);
	if (!m_pLanguage) {
		return;
	}
	if (JpChatMsg_IsHangulMode()) {
		m_pLanguage->SetBgImg("chat_kor");
	} else {
		m_pLanguage->SetBgImg("chat_eng");
	}
}

// 0x48f890
void FrChatDlg::OnChatInputInit(intptr_t param) {
	m_pChatInput = DYNAMIC_CAST(FrEdit, param);
	if (m_pChatInput) {
		m_pChatInput->SetCharLimit(0x2e, true);
		const WRect& rect = m_pChatInput->GetRect();
		m_pChatInput->SetRect(WRect(rect.x, rect.y, rect.w - 45.0, rect.h));
		m_pChatInput->SetKeyFocus(true);
	}
}

// 0x48f960
void FrChatDlg::OnEmoticonInit(intptr_t param) {
	m_pEmoticon = DYNAMIC_CAST(FrButton, param);
}

// 0x48fd50
void FrChatDlg::OnChatWndBtnInit(intptr_t param) {
	m_pChatWndBtn = DYNAMIC_CAST(FrButton, param);
	m_pChatWndBtn->SetPushDelay(0.0f);
	m_pChatWndBtn->UseDblClick(false);
	ToggleChatBtn(JpOption_GetChatWndDown() != 0);
}

// 0x48fdf0
void FrChatDlg::OnChatWndBtnLBtnDown(void) {
	JpOption_SetChatWndDown(JpOption_GetChatWndDown() == 0);
	ToggleChatBtn(true);
}

// 0x48fe10
void FrChatDlg::OnChatTargetInit(intptr_t param) {
	m_pChatTarget = DYNAMIC_CAST(FrComboBox, param);
	m_pChatTarget->HidePrivacy(true);
	m_pChatTarget->SetListDirection(FrComboBox::LIST_ABOVE);
	// InitReservedChatList (0x645bc0) fills the combo with the
	// Jp_LookupText labels; the target compares (UTIL_SendChatMessage
	// 0x6456b0, AddWhisperPartner 0x645420, OnProc, the whisper-fail reset
	// in OnChatInputEnterKey) use the same translated text.
	WSingleton<CSharedDoc>::Instance()->InitReservedChatList(m_pChatTarget,
		(eReservedChatPartner)((JpSharedDoc_GetRoomGameType() == 6) + RESERVED_CHAT_PARTNER_GUILD));
}

// 0x48ffe0
bool FrChatDlg::OnEmoticonDlgResult(int ret, FrForm* pForm) {
	if (m_pEmoticonDlg && ret == 1) {
		FrEmoticonDlg* pDlg = DYNAMIC_CAST(FrEmoticonDlg, pForm);
		if (pDlg) {
			const char* icon = pDlg->GetSelectedIcon();
			if (icon) {
				// With the report dialog up, the icon goes into its "id" edit.
				FrEdit* pEdit = m_pChatInput;
				if (m_pReportDlg) {
					pEdit = DYNAMIC_CAST(FrEdit, m_pReportDlg->FindChildByName("id"));
				}

				if (pEdit) {
					const char* front = pEdit->GetEditText_Front();
					const char* comp = pEdit->GetEditText_Comp();
					const char* end = pEdit->GetEditText_End();

					if (WSingleton<CChatMsg>::Instance()->GetMaskedFont()->GetTextWidth(g_view, MakeStr("%s%s%s%s", front, comp, icon, end)) < pEdit->GetWidthLimit())
						pEdit->SetLine(1, MakeStr("%s%s%s%s", front, comp, icon, end), 0, false, 0);
					pEdit->SetKeyFocus(true);
				}
			}
		}
	}

	m_pEmoticonDlg = 0;
	return true;
}

// 0x490170
void FrChatDlg::OnEmoticonLBtnDown(void) {
	if (m_pEmoticon && !m_pEmoticonDlg) {
		m_pEmoticonDlg = CreateForm<FrEmoticonDlg>(g_pFresh->GetManager(), this, "emoticon", 0);
		if (m_pEmoticonDlg) {
			WPoint pt(0.0f, 0.0f);
			if (m_pEmoticon) {
				const WRect& rect = m_pEmoticon->GetRect();
				pt.x = rect.x - 200.0;
				pt.y = rect.y - 260.0;
			}
			m_pEmoticonDlg->Open((FRESH_RESULT_CALLBACK)&FrChatDlg::OnEmoticonDlgResult, pt, 3);
		}
	}
}

// 0x490430
void FrChatDlg::OnProc(const float dt) {
	if (g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) {
		if (g_input->GetDown("EMOTICON", false) && !JpChat_IsEmoticonHotkeyBlocked()
			&& !WSingleton<CMessengerInfo>::Instance()->IsVisible()) {
			OnEmoticonLBtnDown();
		}
	}

	if (m_pChatTarget && m_pChatInput) {
		if (g_input->GetDown("PAGE_UP", true)) {
			if (m_pChatInput->HasKeyFocus() && WSingleton<CSharedDoc>::Instance()->m_chatHistory.size() > 0) {
				const char* line = m_pChatInput->GetLine(1, false);
				std::list<std::string>::iterator it = std::find(WSingleton<CSharedDoc>::Instance()->m_chatHistory.begin(),
					WSingleton<CSharedDoc>::Instance()->m_chatHistory.end(), line);
				if (it != WSingleton<CSharedDoc>::Instance()->m_chatHistory.end()) {
					if (it != WSingleton<CSharedDoc>::Instance()->m_chatHistory.begin()) {
						--it;
						WSingleton<CChatMsg>::Instance()->SetChatText(it->c_str(), false);
					}
				} else {
					WSingleton<CChatMsg>::Instance()->SetChatText(
						WSingleton<CSharedDoc>::Instance()->m_chatHistory.back().c_str(), false);
				}
			} else if (m_pChatTarget->HasKeyFocus() && WSingleton<CSharedDoc>::Instance()->m_list4CD8.size() > 0) {
				WSingleton<CChatMsg>::Instance()->SetChatText(
					WSingleton<CSharedDoc>::Instance()->GetPrevChatTarget(m_pChatTarget->GetLine(1, false)), false);
			}
		} else if (g_input->GetDown("PAGE_DOWN", true)) {
			if (m_pChatInput->HasKeyFocus() && WSingleton<CSharedDoc>::Instance()->m_chatHistory.size() > 0) {
				const char* line = m_pChatInput->GetLine(1, false);
				std::list<std::string>::iterator it = std::find(WSingleton<CSharedDoc>::Instance()->m_chatHistory.begin(),
					WSingleton<CSharedDoc>::Instance()->m_chatHistory.end(), line);
				if (it != WSingleton<CSharedDoc>::Instance()->m_chatHistory.end()) {
					++it;
					if (it != WSingleton<CSharedDoc>::Instance()->m_chatHistory.end()) {
						WSingleton<CChatMsg>::Instance()->SetChatText(it->c_str(), false);
					} else {
						WSingleton<CChatMsg>::Instance()->SetChatText("", false);
					}
				}
			} else if (m_pChatTarget->HasKeyFocus() && WSingleton<CSharedDoc>::Instance()->m_list4CD8.size() > 0) {
				WSingleton<CChatMsg>::Instance()->SetChatText(
					WSingleton<CSharedDoc>::Instance()->GetNextChatTarget(m_pChatTarget->GetLine(1, false)), false);
			}
		} else if (g_input->GetDown("ENTER", true) && !WSingleton<CMessengerInfo>::Instance()->IsVisible()) {
			if (!m_pChatInput->HasKeyFocus()) {
				m_pChatInput->SetKeyFocus(true);
			}
		}

		if (strlen(m_pChatTarget->GetLine(1, false)) == 0
			|| strcmp(m_pChatTarget->GetLine(1, false), Jp_LookupText("\xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4")) == 0) {   // "to all"
			m_pChatInput->SetFontColor(0xFFFFFFFF);
		} else if (strlen(m_pChatTarget->GetLine(1, false)) == 0
			|| strcmp(m_pChatTarget->GetLine(1, false), Jp_LookupText("\xB1\xE6\xB5\xE5\xBF\xA1\xB0\xD4")) == 0) {   // "to the guild"
			m_pChatInput->SetFontColor(0xFF5BFFB0);
		} else {
			m_pChatInput->SetFontColor(0xFFCF67FF);
		}
	}
}

// 0x4907f0
void FrChatDlg::OnReportLBtnDown(void) {
	m_pReportDlg = CreateForm<FrReportDlg>(g_pFresh->GetManager(), this, "report", 0);

	// CP949: "1. past chat is also logged on the server. 2. fake or repeated
	// reports are punished. 3. reports are handled within 48 hours."
	std::string text;
	text = Jp_LookupText("1. \xB4\xEB\xC8\xAD\xC3\xA2\xC0\xC7 \xC1\xF6\xB3\xAD \xB3\xBB\xBF\xEB\xB5\xB5 \xB8\xF0\xB5\xCE \xBC\xAD\xB9\xF6\xBF\xA1 \xB1\xE2\xB7\xCF\xB5\xCB\xB4\xCF\xB4\xD9.\n");
	text += Jp_LookupText("2. \xC1\xB6\xC0\xDB, \xC7\xE3\xC0\xA7, \xB9\xDD\xBA\xB9\xBD\xC5\xB0\xED\xB4\xC2 \xC3\xB3\xB9\xFA\xC0\xBB \xB9\xDE\xBD\xC0\xB4\xCF\xB4\xD9.\n");
	text += Jp_LookupText("3. \xBD\xC5\xB0\xED\xC7\xCF\xBD\xC5 \xB3\xBB\xBF\xEB\xC0\xBA 48\xBD\xC3\xB0\xA3 \xC0\xCC\xB3\xBB\xBF\xA1 \xC3\xB3\xB8\xAE\xB5\xCB\xB4\xCF\xB4\xD9.");

	m_pReportDlg->SetMessage(text.c_str(), false);
	m_pReportDlg->Open((FRESH_RESULT_CALLBACK)&FrChatDlg::OnReportDlgResult, 3);
}

// 0x490b30
void FrChatDlg::OnChatViewInit(intptr_t param) {
	m_pChatView = DYNAMIC_CAST(FrEdit, param);
	m_pChatView->HidePrivacy(true);
	JpSharedDoc_ClearChatLines();
}

// 0x490d10
void FrChatDlg::AddChatMsg(const char* msg, unsigned int color) {
	int mode = JpSharedDoc_GetLobbyMode();
	if (mode == 3 || mode == 4 || mode == 8 || mode == 6) {
		sChatLine line;
		line.m_text = msg;
		line.m_color = color;
		WSingleton<CSharedDoc>::Instance()->m_chatLineList.push_back(line);
	}
	// The JP FrEdit::AddLine (0xbfac70) takes a fourth argument, 0 here.
	if (m_pChatView) {
		m_pChatView->AddLine(msg, color, false);
	}
}

// 0x490e50
bool FrChatDlg::OnChatInputEnterKey(intptr_t param) {
	const char* msg = (const char*)param;
	if (strlen(msg)) {
		if (!WSingleton<CSharedDoc>::Instance()->UTIL_SendChatMessage(m_pChatTarget, msg, true)) {
			// The JP SetLine (chattarget's vtable +0x60) takes a sixth
			// argument, 0 here.
			m_pChatTarget->SetLine(1, Jp_LookupText("\xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4"), 0, false, 0);   // "to all"
			// "Wrong whisper target. Changed to "to all"."
			AddChatMsg(Jp_LookupText("\xB1\xD3\xB8\xBB \xBB\xF3\xB4\xEB\xB0\xA1 \xC0\xDF\xB8\xF8\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9. \"\xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4\"\xB7\xCE \xBA\xAF\xB0\xE6\xC7\xD5\xB4\xCF\xB4\xD9."), 0xFFAF00A1);
		}
	}
	return true;
}

// JP 0xabc3c5 (CLobbyMain::HandleMsg case 3, glue): the chat line handed to
// the open room form (CLobbyMain +0x780), an FrGameRoomDlg subclass; the
// JP calls FrChatDlg::AddChatMsg 0x490d10 on it directly.
void JpRoomTypeForm_AddChatMsg(FrForm* form, const char* text, unsigned int color) {
	static_cast<FrGameRoomDlg*>(form)->AddChatMsg(text, color);
}
