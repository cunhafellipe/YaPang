// The JP 983 lobby's "avatar loaded" message to the lobby desktop
// (JpLobbyLoading.h has the JP call chain).
#include "JpLobbyLoading.h"

#include "actor.h"
#include "RFLobbyDesktop.h"

// 0x568860 / 0x5688e0, from 0x568886 / 0x568905: MsgObject(NULL, 0x57) to
// the "Lobby" actor, which forwards it to the desktop.
void JpLobbyAvatarLoaded() {
	if (RFLobbyDesktop::s_instance != NULL)
		RFLobbyDesktop::s_instance->HandleMsg(MsgObject(NULL, 0x57, 0, 0, 0, 0, 0));
}
