// The JP 983 lobby's small widget calls RFLobbyDesktop makes (names ours),
// from the JP exe's bodies (node scripts/disasm.ts --jp 0x<va>).
//
// The ad ticker label_ad draws (0xe78c58, 0x897c40) is the KR
// COneLineBoard (RTTI COneLineBoard : WSingleton<COneLineBoard>, vtable
// 0xd34c34; 0xe78c58 is WSingleton<COneLineBoard>::m_pInstance): 0x897c40 is
// its Display, 0x8975d0 its DrawBg, 0x897ec0 its destructor, and the
// rectangle +0x4c..+0x58 is m_rect (SetRect). It is not redeclared here:
// client/ProjectG/onelineboard.h is its header.
#ifndef PG_JP_JPLOBBYWIDGETS_H
#define PG_JP_JPLOBBYWIDGETS_H

#include <string>

class RFWnd;

// 0x5d8070 (an RFServerNoticeDlg member; name ours): label_server_name
// shows text (Shift-JIS, widened), then the dialog is centred on the
// screen.
void ServerNoticeDialog_SetText(RFWnd* dialog, std::string const& text);

#endif // PG_JP_JPLOBBYWIDGETS_H
