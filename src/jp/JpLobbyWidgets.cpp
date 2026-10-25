// The JP 983 lobby's small widget calls (JpLobbyWidgets.h), from the JP
// exe's bodies (node scripts/disasm.ts --jp 0x<va>).
#include "JpLobbyWidgets.h"

#include <string>

#include "ChatMsg.h"
#include "JpLobbyGlue.h"
#include "RFLabel.h"
#include "RFTextManager.h"
#include "RFWnd.h"
#include "RefreshStringUtil.h"

// 0xd13338: Jp_LookupText's key for btn_exit's chat line, the KR text
// ntAvatarChatTask::Process prints on ESCAPE while the personal shop is in
// use: "You cannot leave the chat room while using the shop."
static const char s_jpTextKeyExitWhileTrading[] =
	"\xbb\xf3\xc1\xa1 \xc0\xcc\xbf\xeb\xc1\xdf\xbf\xa1\xb4\xc2 \xb4\xeb\xc8\xad\xb9\xe6\xbf\xa1\xbc\xad "
	"\xb3\xaa\xb0\xa5\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";

// 0x5d7e30 (dialog in esi, name in edx; only 0x5d8070 calls it; name ours):
// the child label named name takes text, when there is one.
static void SetChildLabel(RFWnd* dialog, const wchar_t* name, std::wstring const& text) {
	RFLabel* label = dynamic_cast<RFLabel*>(dialog->GetChild(std::wstring(name)));
	if (label != NULL) {
		label->SetLabel(std::wstring(text.c_str()));
	}
}

// 0x5d8070
void ServerNoticeDialog_SetText(RFWnd* dialog, std::string const& text) {
	SetChildLabel(dialog, L"label_server_name", ShiftJisToWString(std::string(text.c_str())));
	dialog->CenterWindow(NULL);
}

// Inside RFLobbyDesktop::OnExitClicked 0x5f8470 (name ours; the desktop is
// unused): the JP text for the key 0xd13338 (Jp_LookupText 0xb4eb80) goes to
// the chat through 0x9a1600, the JP CChatMsg::AddChatMsg (Lobby 3 / GolfRule
// 0x71 / Screen / AvatarChat messages, then the history list, as KR), on the
// CChatMsg instance 0xe73e8c, with 0xffff00, alwaysShow true, removeColor
// false: the KR ntAvatarChatTask::Process call.
void Jp_ShowExitMessageBox(RFLobbyDesktop* desktop) {
	(void)desktop;
	std::string text(Jp_LookupText(s_jpTextKeyExitWhileTrading));
	CChatMsg::Instance()->AddChatMsg(text, 0xFFFF00, true, false);
}
