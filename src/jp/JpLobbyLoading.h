// The end of the JP 983 lobby's loading wait: the message that closes the
// lobby desktop's "Loading..." dialog (RFLoadingProgressDlg, opened by
// RFLobbyDesktop::ShowLobby in lobby mode).
//
// In the JP exe the sender is the JP-only LobbyPlayer actor ("LobbyAvatar",
// vtable 0xd0a644; docs/JP_CLIENT.md, "LobbyPlayer"). Its OnPreLoadInit
// 0x568c10 binds two loader callbacks, 0x568860 (own character, flag +0x88)
// and 0x5688e0 (another player, flag +0xb0). Each builds the avatar through
// 0x568730 (message 0x53 to "Lobby", build 0x568480, message 0x54), sleeps
// 15 ms, then sends MsgObject 0x57 to CTask::GetActor("Lobby") with
// operator<<; CLobbyMain has no case for it, CTaskMain::HandleMsgCommon
// (0x65d8e0) hands it to RFLobbyDesktop::HandleMsg (slot [35] 0x5d5520),
// whose case 0x57 closes the dialog and sends 0x182 to "LobbyBg".
//
// KR has no LobbyPlayer, and KR already uses MsgObject 0x57 to "Lobby" for
// the guild-battle team points (LobbyTask.cpp), so the port does not route
// the JP message through the KR task: it hands it to the desktop directly.
#ifndef PG_JP_JPLOBBYLOADING_H
#define PG_JP_JPLOBBYLOADING_H

// The tail of 0x568860 / 0x5688e0 (the 0x57 send), delivered straight to
// RFLobbyDesktop::s_instance; nothing without a desktop.
void JpLobbyAvatarLoaded();

#endif // PG_JP_JPLOBBYLOADING_H
