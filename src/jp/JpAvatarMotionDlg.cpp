// JP-only game code: FrAvatarMotionDlg, the lobby's avatar motion chat
// dialog, and its creator CreateAvatarMotionDlg (JP 0x7b1130). See
// JpAvatarMotionDlg.h.
#include "JpAvatarMotionDlg.h"

#include <algorithm>
#include <string>

#include <stdlib.h>
#include <string.h>

#include "InputManager.h"
#include "ObjectFactory.h"
#include "RFLobbyDesktop.h"
#include "RFTextManager.h"
#include "TaskManager.h"
#include "JpLobbyGlue.h"
#include "JpLobbyPlayer.h"
#include "actor.h"
#include "avatar_avatar.h"
#include "bitmap.h"
#include "commonutil.h"
#include "fresh.h"
#include "frgraphicinterface.h"
#include "frlistbox.h"
#include "frwnd.h"
#include "frwndmanager.h"
#include "mousecursor.h"
#include "partsinfo.h"
#include "parttidlist.h"
#include "wview.h"
#include "shared/luasystem/LuaScript.h"
#include "shared/luasystem/LuaScriptManager.h"
#include "shared/luasystem/luatablehelper.h"

// JP-only calls this dialog makes that have no KR counterpart (names ours,
// not decompiled).
// The JP ntAvatar +0x784, read inline by DrawAvatarChatRowButtons 0x47fbf0
// (4 and 5 while a chat motion plays): the KR m_motionType (0x694, the
// public GetMotion()). The JP ntAvatar has 0xf0 more bytes before it: JP
// ProcessMotion 0x7a70f0 (paired with the KR ntAvatar::ProcessMotion) opens
// with the KR switch on +0x694 (0, 2, 3, 1) on +0x784, then reads +0x780 as
// the KR reads m_pMotion +0x690; the JP adds the motion type 5. The JP does
// not test the avatar for NULL; ours returns 0 (no motion) then.
static int JpAvatar_GetMotionState(const ntAvatar* avatar) {
	if (avatar == 0) {
		return 0;
	}
	return avatar->GetMotion();
}
// The number of part motion slots (defined below).
unsigned int JpPartMotions_GetCount(void);

// The JP motion table (names ours): 15 character types (the IFF typeid's
// index) of 19 rows of 0x20 at 0xe2dd08, filled by the static initializer
// 0xce6b10; the rows of a type end at the first without a motion key. Only
// the two fields the motion dialog reads are kept: +0 the motion key and +4
// its display name, stored as a text key through Jp_LookupText 0xb4eb80
// (lookUpName) or as is. (+8 to +0x1c: six more strings 0x9a3990 searches
// for "/%s"; unused here.)
struct sJpMotionRow {
	const char* motion;
	const char* name;
	bool lookUpName;
};
static const sJpMotionRow s_jpMotionTable[15][19] = {
	{ // 0
		{ "\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee", "\xc1\xc2\xc0\xfd", true },
		{ "1\xb5\xee\xb8\xf0\xbc\xc7", "\xa4\xbb\xa4\xbb", true },
		{ "3\xb5\xee\xb8\xf0\xbc\xc7", "OTL", false },
		{ "\xc5\xb8\xc0\xd3\xbf\xc0\xb9\xf6\xba\xa1\xc4\xbfOB\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee", "\xbd\xc7\xb8\xc1", true },
		{ "\xc6\xdb\xc6\xc3\xbc\xba\xb0\xf8", "\xb3\xaa\xc0\xcc\xbd\xba", true },
		{ "\xc6\xdb\xc6\xc3\xbd\xc7\xc6\xd0", "\xbe\xee\xc0\xcc\xc4\xed", true },
		{ "\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee", "\xb8\xb8\xbc\xbc", true },
		{ "chat_\xb9\xda\xbc\xf6", "\xb9\xda\xbc\xf6", true },
		{ "chat_\xc0\xce\xbb\xe7", "\xc0\xce\xbb\xe7", true },
		{ "chat_\xb4\xed\xbd\xba", "\xb4\xed\xbd\xba", true },
		{ "chat_\xb3\xaf\xb1\xe2", "\xb3\xaf\xb1\xe2", true },
		{ "chat_\xbe\xc6\xc0\xcc\xc5\xdb\xb4\xed\xbd\xba", "\xb8\xae\xb5\xeb", true },
		{ "chat_\xb3\xaf\xb1\xe2" "2", "\xb3\xaf\xb1\xe2", true },
	},
	{ // 1
		{ "\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee", "\xc2\xa5\xc1\xf5", true },
		{ "1\xb5\xee\xb8\xf0\xbc\xc7", "\xb8\xb8\xbc\xbc", true },
		{ "2\xb5\xee\xb8\xf0\xbc\xc7", "\xc8\xaf\xc8\xa3", true },
		{ "3\xb5\xee\xb8\xf0\xbc\xc7", "\xbe\xfb\xbe\xfb", true },
		{ "chat_\xbb\xdf\xc1\xfc", "\xc8\xef", true },
		{ "\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee", "\xbe\xee\xb8\xd3", true },
		{ "\xc5\xb8\xc0\xd3\xbf\xc0\xb9\xf6\xba\xa1\xc4\xbfOB\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee", "\xc8\xe6\xc8\xe6", true },
		{ "\xc6\xdb\xc6\xc3\xbc\xba\xb0\xf8", "\xb3\xaa\xc0\xcc\xbd\xba", true },
		{ "\xc6\xdb\xc6\xc3\xbd\xc7\xc6\xd0", "OTL", false },
		{ "\xc6\xdb\xc6\xc3\xc8\xc4\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee", "\xba\xf1\xc6\xb2", true },
		{ "\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee", "\xc1\xc1\xbe\xc6", true },
		{ "chat_\xb4\xed\xbd\xba", "\xb4\xed\xbd\xba", true },
		{ "chat_\xb3\xaf\xb1\xe2", "\xb3\xaf\xb1\xe2", true },
		{ "chat_\xbe\xc6\xc0\xcc\xc5\xdb\xb4\xed\xbd\xba", "\xb8\xae\xb5\xeb", true },
		{ "chat_\xb3\xaf\xb1\xe2" "2", "\xb3\xaf\xb1\xe2", true },
	},
	{ // 2
		{ "\xbc\xbc\xc0\xcc\xba\xea\xc6\xc4\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee", "\xbf\xc0\xba\xfc", true },
		{ "1\xb5\xee\xb8\xf0\xbc\xc7", "\xb8\xb8\xbc\xbc", true },
		{ "2\xb5\xee\xb8\xf0\xbc\xc7", "\xbf\xb9\xbd\xba", true },
		{ "\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee", "\xc8\xaf\xc8\xa3", true },
		{ "\xc6\xdb\xc6\xc3\xbc\xba\xb0\xf8", "\xb3\xaa\xc0\xcc\xbd\xba", true },
		{ "\xc6\xdb\xc6\xc3\xbd\xc7\xc6\xd0", "\xc4\xe2\xb4\xe7", true },
		{ "\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee", "\xc1\xa8\xc0\xe5", true },
		{ "chat_\xb8\xd3\xbe\xb3", "\xb8\xd3\xbe\xb3", true },
		{ "chat_\xb4\xed\xbd\xba", "\xb4\xed\xbd\xba", true },
		{ "chat_\xb3\xaf\xb1\xe2", "\xb3\xaf\xb1\xe2", true },
		{ "chat_\xbe\xc6\xc0\xcc\xc5\xdb\xb4\xed\xbd\xba", "\xb8\xae\xb5\xeb", true },
		{ "chat_\xb3\xaf\xb1\xe2" "2", "\xb3\xaf\xb1\xe2", true },
	},
	{ // 3
		{ "\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee", "\xb8\xf4\xb6\xf3", true },
		{ "\xbc\xbc\xc0\xcc\xba\xea\xc6\xc4\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee", "\xbe\xc8\xb3\xe7", true },
		{ "\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee", "\xbf\xb9\xbd\xba", true },
		{ "\xc6\xdb\xc6\xc3\xbc\xba\xb0\xf8", "\xb3\xaa\xc0\xcc\xbd\xba", true },
		{ "\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee", "\xb8\xb8\xbc\xbc", true },
		{ "3\xb5\xee\xb8\xf0\xbc\xc7", "\xc5\xd0\xbd\xe2", true },
		{ "\xc5\xb8\xc0\xd3\xbf\xc0\xb9\xf6\xba\xa1\xc4\xbfOB\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee", "\xbd\xc7\xb8\xc1", true },
		{ "\xc6\xdb\xc6\xc3\xbd\xc7\xc6\xd0", "\xbb\xdf\xc1\xfc", true },
		{ "chat_\xc8\xa3\xc8\xa3", "\xc8\xa3\xc8\xa3", true },
		{ "chat_\xba\xea\xc0\xcc", "\xba\xea\xc0\xcc", true },
		{ "chat_\xb4\xed\xbd\xba", "\xb4\xed\xbd\xba", true },
		{ "chat_\xb3\xaf\xb1\xe2", "\xb3\xaf\xb1\xe2", true },
		{ "chat_\xbe\xc6\xc0\xcc\xc5\xdb\xb4\xed\xbd\xba", "\xb8\xae\xb5\xeb", true },
		{ "chat_\xb3\xaf\xb1\xe2" "2", "\xb3\xaf\xb1\xe2", true },
	},
	{ // 4
		{ "1\xb5\xee\xb8\xf0\xbc\xc7", "Yes", false },
		{ "2\xb5\xee\xb8\xf0\xbc\xc7", "\xc0\xce\xbb\xe7", true },
		{ "\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee", "\xc1\xc2\xc0\xfd", true },
		{ "\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee", "\xc8\xca", true },
		{ "\xc6\xdb\xc6\xc3\xc8\xc4\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee", "\xc7\xe3\xb9\xab", true },
		{ "\xc6\xdb\xc6\xc3\xbc\xba\xb0\xf8", "\xb3\xaa\xc0\xcc\xbd\xba", true },
		{ "\xc5\xb8\xc0\xd3\xbf\xc0\xb9\xf6\xba\xa1\xc4\xbfOB\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee", "\xbd\xc7\xb8\xc1", true },
		{ "\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee", "\xc5\xd7\xb4\xcf\xbd\xba", true },
		{ "chat_\xc2\xf7\xc0\xce", "NO", false },
		{ "chat_\xb4\xed\xbd\xba", "\xb4\xed\xbd\xba", true },
		{ "chat_\xb3\xaf\xb1\xe2", "\xb3\xaf\xb1\xe2", true },
		{ "chat_\xbe\xc6\xc0\xcc\xc5\xdb\xb4\xed\xbd\xba", "\xb8\xae\xb5\xeb", true },
		{ "chat_\xb3\xaf\xb1\xe2" "2", "\xb3\xaf\xb1\xe2", true },
	},
	{ // 5
		{ "2\xb5\xee\xb8\xf0\xbc\xc7", "\xb8\xd3\xb8\xae", true },
		{ "2\xb5\xee\xb8\xf0\xbc\xc7" "2", "\xb8\xb8\xbc\xbc", true },
		{ "3\xb5\xee\xb8\xf0\xbc\xc7", "\xbf\xd5\xb5\xfb", true },
		{ "\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee", "\xc3\xe3", true },
		{ "\xbf\xec\xb5\xe5\xbc\xa6\xc8\xc4\xb8\xf0\xbc\xc7", "\xb8\xc1\xbf\xf8\xb0\xe6", true },
		{ "\xc5\xb8\xc0\xd3\xbf\xc0\xb9\xf6\xba\xa1\xc4\xbfOB\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee", "\xc1\xc2\xc0\xfd", true },
		{ "\xc6\xdb\xc6\xc3\xc8\xc4\xb4\xeb\xb1\xe2", "\xb3\xaa\xc0\xcc\xbd\xba", true },
		{ "\xc6\xdb\xc6\xc3\xbc\xba\xb0\xf8", "\xbf\xb9\xbd\xba", true },
		{ "\xc6\xdb\xc6\xc3\xbd\xc7\xc6\xd0", "\xc5\xd0\xbd\xe2", true },
		{ "\xc6\xdb\xc6\xc3\xc8\xc4\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee", "\xbd\xc7\xb8\xc1", true },
		{ "chat_\xb4\xed\xbd\xba", "\xb4\xed\xbd\xba", true },
		{ "chat_\xb3\xaf\xb1\xe2", "\xb3\xaf\xb1\xe2", true },
		{ "chat_\xbe\xc6\xc0\xcc\xc5\xdb\xb4\xed\xbd\xba", "\xb8\xae\xb5\xeb", true },
		{ "chat_\xb3\xaf\xb1\xe2" "2", "\xb3\xaf\xb1\xe2", true },
	},
	{ // 6
		{ "\xb9\xf6\xb5\xf0\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee", "\xb8\xb8\xbc\xbc", true },
		{ "\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee", "\xb4\xe4\xb4\xe4", true },
		{ "\xc6\xdb\xc6\xc3\xc8\xc4\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee", "\xb8\xf4\xb6\xf3", true },
		{ "2\xb5\xee\xb8\xf0\xbc\xc7", "\xbd\xc2\xb8\xae", true },
		{ "3\xb5\xee\xb8\xf0\xbc\xc7", "\xc5\xd0\xbd\xe2", true },
		{ "\xc6\xdb\xc6\xc3\xc8\xc4\xb4\xeb\xb1\xe2", "\xc7\xcf\xc0\xcc", true },
		{ "\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee", "\xba\xf9\xb1\xdb", true },
		{ "chat_\xc5\xb0\xbd\xba", "\xc5\xb0\xbd\xba", true },
		{ "chat_\xbd\xc7\xb8\xc1", "\xbd\xc7\xb8\xc1", true },
		{ "chat_\xbd\xc8\xbe\xee", "\xbd\xc8\xbe\xee", true },
		{ "chat_\xb4\xed\xbd\xba", "\xb4\xed\xbd\xba", true },
		{ "chat_\xb3\xaf\xb1\xe2", "\xb3\xaf\xb1\xe2", true },
		{ "chat_\xbe\xc6\xc0\xcc\xc5\xdb\xb4\xed\xbd\xba", "\xb8\xae\xb5\xeb", true },
		{ "chat_\xb3\xaf\xb1\xe2" "2", "\xb3\xaf\xb1\xe2", true },
	},
	{ // 7
		{ "3\xb5\xee\xb8\xf0\xbc\xc7", "\xc5\xd0\xbd\xe2", true },
		{ "\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee", "\xba\xd0\xb3\xeb", true },
		{ "\xc6\xdb\xc6\xc3\xbd\xc7\xc6\xd0", "\xbe\xc8\xb5\xc5", true },
		{ "2\xb5\xee\xb8\xf0\xbc\xc7", "\xbf\xb9\xbd\xba", true },
		{ "chat_\xb8\xb8\xbc\xbc", "\xb8\xb8\xbc\xbc", true },
		{ "chat_\xb4\xed\xbd\xba", "\xb4\xed\xbd\xba", true },
		{ "chat_\xb3\xaf\xb1\xe2", "\xb3\xaf\xb1\xe2", true },
		{ "chat_\xbe\xc6\xc0\xcc\xc5\xdb\xb4\xed\xbd\xba", "\xb8\xae\xb5\xeb", true },
		{ "chat_\xb3\xaf\xb1\xe2" "2", "\xb3\xaf\xb1\xe2", true },
	},
	{ // 8
		{ "chat_\xbd\xc7\xb8\xc1", "\xbd\xc7\xb8\xc1", true },
		{ "chat_\xc8\xad\xb3\xb2", "\xc8\xad\xb3\xb2", true },
		{ "chat_\xc7\xcf\xc6\xae", "\xc7\xcf\xc6\xae", true },
		{ "chat_\xc3\xe3", "\xc3\xe3", true },
		{ "chat_\xc7\xcf\xc0\xcc", "\xc7\xcf\xc0\xcc", true },
		{ "chat_\xb8\xb8\xbc\xbc", "\xb8\xb8\xbc\xbc", true },
		{ "chat_\xb3\xaa\xc0\xcc\xbd\xba", "\xb3\xaa\xc0\xcc\xbd\xba", true },
		{ "chat_\xbe\xc8\xb3\xe7", "\xbe\xc8\xb3\xe7", true },
		{ "chat_\xbe\xfb\xbe\xfb", "\xbe\xfb\xbe\xfb", true },
		{ "chat_\xc5\xb0\xbd\xba", "\xc5\xb0\xbd\xba", true },
		{ "chat_\xba\xce\xb2\xf4", "\xba\xce\xb2\xf4", true },
		{ "chat_\xb4\xed\xbd\xba", "\xb4\xed\xbd\xba", true },
		{ "chat_\xb3\xaf\xb1\xe2", "\xb3\xaf\xb1\xe2", true },
		{ "chat_\xbe\xc6\xc0\xcc\xc5\xdb\xb4\xed\xbd\xba", "\xb8\xae\xb5\xeb", true },
		{ "chat_\xb3\xaf\xb1\xe2" "2", "\xb3\xaf\xb1\xe2", true },
	},
	{ // 9
		{ "chat_\xb3\xee\xb6\xf7", "\xb3\xee\xb6\xf7", true },
		{ "chat_\xba\xf1\xc6\xb2", "\xba\xf1\xc6\xb2", true },
		{ "chat_\xc6\xc4\xc0\xcc\xc6\xc3", "\xc6\xc4\xc0\xcc\xc6\xc3", true },
		{ "chat_\xb8\xb8\xbc\xbc", "\xb8\xb8\xbc\xbc", true },
		{ "chat_\xb9\xda\xbc\xf6", "\xb9\xda\xbc\xf6", true },
		{ "chat_\xc5\xe4\xb3\xa2", "\xc5\xe4\xb3\xa2", true },
		{ "chat_\xbe\xc8\xb3\xe7", "\xbe\xc8\xb3\xe7", true },
		{ "chat_\xb4\xed\xbd\xba", "\xb4\xed\xbd\xba", true },
		{ "chat_\xb3\xaf\xb1\xe2", "\xb3\xaf\xb1\xe2", true },
		{ "chat_\xbe\xc6\xc0\xcc\xc5\xdb\xb4\xed\xbd\xba", "\xb8\xae\xb5\xeb", true },
		{ "chat_\xb3\xaf\xb1\xe2" "2", "\xb3\xaf\xb1\xe2", true },
	},
	{ // 10
		{ "chat_\xb1\xe2\xbb\xdd", "\xb1\xe2\xbb\xdd", true },
		{ "chat_\xb1\xe2\xc1\xf6\xb0\xb3", "\xb1\xe2\xc1\xf6\xb0\xb3", true },
		{ "chat_\xb6\xa7\xb8\xae\xb1\xe2", "\xb6\xa7\xb8\xae\xb1\xe2", true },
		{ "chat_\xb8\xde\xb7\xd5", "\xb8\xde\xb7\xd5", true },
		{ "chat_\xbd\xc5\xb9\xdf", "\xbd\xc5\xb9\xdf", true },
		{ "chat_\xb8\xc2\xb1\xe2", "\xb8\xc2\xb1\xe2", true },
		{ "chat_\xc0\xce\xbb\xe7", "\xc0\xce\xbb\xe7", true },
		{ "chat_\xc1\xd6\xc0\xc7", "\xc1\xd6\xc0\xc7", true },
		{ "chat_\xc7\xd1\xbd\xc9", "\xc7\xd1\xbd\xc9", true },
		{ "chat_\xba\xf1\xbf\xf4\xb1\xe2", "\xba\xf1\xbf\xf4\xb1\xe2", true },
		{ "chat_\xc1\xc2\xc0\xfd", "\xc1\xc2\xc0\xfd", true },
		{ "chat_\xba\xd0\xb3\xeb", "\xba\xd0\xb3\xeb", true },
		{ "chat_\xb9\xda\xbc\xf6", "\xb9\xda\xbc\xf6", true },
		{ "chat_\xb3\xaf\xb1\xe2", "\xb3\xaf\xb1\xe2", true },
		{ "chat_\xbe\xc6\xc0\xcc\xc5\xdb\xb4\xed\xbd\xba", "\xb8\xae\xb5\xeb", true },
		{ "chat_\xb3\xaf\xb1\xe2" "2", "\xb3\xaf\xb1\xe2", true },
	},
	{ // 11
		{ "chat_\xb1\xe2\xbb\xdd", "\xb1\xe2\xbb\xdd", true },
		{ "chat_\xb9\xda\xbc\xf6", "\xb9\xda\xbc\xf6", true },
		{ "chat_\xbb\xdf\xc1\xfc", "\xbb\xdf\xc1\xfc", true },
		{ "chat_\xba\xd0\xb3\xeb", "\xba\xd0\xb3\xeb", true },
		{ "chat_\xbd\xbd\xc7\xc4", "\xbd\xbd\xc7\xc4", true },
		{ "chat_\xbe\xc6\xc0\xcc\xc5\xdb\xb4\xed\xbd\xba", "\xb8\xae\xb5\xeb", true },
	},
	{ // 12
		{ "chat_\xb1\xe2\xbb\xdd", "\xb1\xe2\xbb\xdd", true },
		{ "chat_\xb9\xda\xbc\xf6", "\xb9\xda\xbc\xf6", true },
		{ "chat_\xba\xd0\xb3\xeb", "\xba\xd0\xb3\xeb", true },
		{ "chat_\xbb\xdf\xc1\xfc", "\xbb\xdf\xc1\xfc", true },
		{ "chat_\xbd\xbd\xc7\xc4", "\xbd\xbd\xc7\xc4", true },
		{ "chat_\xc0\xce\xbb\xe7", "\xc0\xce\xbb\xe7", true },
		{ "chat_\xbe\xc6\xc0\xcc\xc5\xdb\xb4\xed\xbd\xba", "\xb8\xae\xb5\xeb", true },
	},
	{ // 13
		{ "chat_\xb1\xe2\xbb\xdd", "\xb1\xe2\xbb\xdd", true },
		{ "chat_\xb9\xda\xbc\xf6", "\xb9\xda\xbc\xf6", true },
		{ "chat_\xbb\xdf\xc1\xfc", "\xbb\xdf\xc1\xfc", true },
		{ "chat_\xba\xd0\xb3\xeb", "\xba\xd0\xb3\xeb", true },
		{ "chat_\xbd\xbd\xc7\xc4", "\xbd\xbd\xc7\xc4", true },
		{ "chat_\xc0\xce\xbb\xe7", "\xc0\xce\xbb\xe7", true },
	},
	{ // 14
		{ "chat_\xb1\xe2\xbb\xdd", "\xb1\xe2\xbb\xdd", true },
		{ "chat_\xb5\xb5\xb9\xdf", "\xb5\xb5\xb9\xdf", true },
		{ "chat_\xbd\xbd\xc7\xc4", "\xbd\xbd\xc7\xc4", true },
		{ "chat_\xc2\xea\xc2\xea", "\xc2\xea\xc2\xea", true },
		{ "chat_\xc8\xad\xb3\xb2", "\xc8\xad\xb3\xb2", true },
		{ "chat_\xb3\xaf\xb1\xe2" "2", "\xb3\xaf\xb1\xe2", true },
	},
};

// 0x9a3ca0 (thiscall on the JP CSharedDoc +0x10c24): its +4 counts, the
// number of rows of each type (a type with all 19 rows keeps its count).
static int JpMotionTable_CountRows(int charTypeId) {
	int count = 0;
	while (count < 19 && s_jpMotionTable[charTypeId][count].motion) {
		count++;
	}
	return count;
}

// Thiscall on the JP CSharedDoc +0x10c24 (names ours): the number of motions
// of a character type (0 outside 0 to 14).
// 0x9a3c80
int JpMotionTable_GetCount(const int& charTypeId) {
	if (charTypeId >= 0 && charTypeId < 15) {
		return JpMotionTable_CountRows(charTypeId);
	}
	return 0;
}

// The motion key of a row (NULL outside the type's rows).
// 0x9a3c00
const char* JpMotionTable_GetMotion(const int& charTypeId, int& index) {
	if (charTypeId >= 0 && charTypeId < 15 && index >= 0 && index < JpMotionTable_CountRows(charTypeId)) {
		return s_jpMotionTable[charTypeId][index].motion;
	}
	return 0;
}

// The display name of a row (NULL outside the type's rows).
// 0x9a3c40
const char* JpMotionTable_GetName(const int& charTypeId, int& index) {
	if (charTypeId >= 0 && charTypeId < 15 && index >= 0 && index < JpMotionTable_CountRows(charTypeId)) {
		const sJpMotionRow& row = s_jpMotionTable[charTypeId][index];
		return row.lookUpName ? Jp_LookupText(row.name) : row.name;
	}
	return 0;
}

// 0x98c460 (?Random@@YIMMMH@Z): the KR commonutil.cpp Random (defined
// there, declared in no header): a value between min and max picked by
// seed % 1023 (rand() when seed is negative).
float Random(float min, float max, int seed);

// lib: 0x481ee0, 0x481ef0 (this-adjusting deleting-destructor thunks),
// 0x481580 (sAvatarMotionItem destruction), 0x481b30 (vector erase),
// 0x481ba0 (vector tidy), 0x482140 and 0x4820d0 (vector push_back), 0xcc7c80, 0xcc7cc0
// (static initializers)

// 0x4d0ce0 (on the JP object at 0xe73e14, which it does not read): the
// number of part motion slots.
unsigned int JpPartMotions_GetCount(void) {
	return 5;
}

// The JP object 0xe73e14's part motion tables as 0x4cf830 fills them (names
// ours). +0x1d8: one {caption, motion} pair of text keys per slot, stored
// through Jp_LookupText 0xb4eb80 (slots 0 and 2 share the caption key).
static const char* const s_jpPartMotionKeys[5][2] = {
	{ "\xb3\xaf\xb1\xe2", "chat_\xb3\xaf\xb1\xe2" },
	{ "\xb8\xae\xb5\xeb", "chat_\xbe\xc6\xc0\xcc\xc5\xdb\xb4\xed\xbd\xba" },
	{ "\xb3\xaf\xb1\xe2", "chat_\xb3\xaf\xb1\xe2" "2" },
	{ "SP1", "chat_sp1" },
	{ "SP2", "chat_sp2" },
};

// +0x200: 10 rows of 0x10 per character type (the IFF typeid's index): a
// part typeid, the slot it gives, then the slot's caption and motion
// copied from the pairs above. Rows 0x4cf830 never stores are zero (the
// part typeid 0 matches nothing); character type 13 has none.
struct sJpPartMotionRow {
	uint32_t partTypeId;
	unsigned int slot;
};
static const sJpPartMotionRow s_jpPartMotionRows[15][10] = {
	{ { 0x801680a, 0 }, { 0x801680b, 0 }, { 0x801680c, 0 }, { 0x801680e, 0 }, { 0x8022817, 1 }, { 0x801680f, 2 }, { 0x8016810, 2 }, { 0x8016815, 2 }, { 0x8016816, 2 }, { 0, 0 } },
	{ { 0x805880a, 0 }, { 0x805880b, 0 }, { 0x805880c, 0 }, { 0x805880e, 0 }, { 0x8062817, 1 }, { 0x805880f, 2 }, { 0x8058810, 2 }, { 0x8058815, 2 }, { 0x8058816, 2 }, { 0, 0 } },
	{ { 0x809880a, 0 }, { 0x809880b, 0 }, { 0x809880c, 0 }, { 0x809880e, 0 }, { 0x80a6817, 1 }, { 0x809880f, 2 }, { 0x8098810, 2 }, { 0x8098815, 2 }, { 0x8098816, 2 }, { 0, 0 } },
	{ { 0x80dc80a, 0 }, { 0x80dc80b, 0 }, { 0x80dc80c, 0 }, { 0x80dc80e, 0 }, { 0x80e2817, 1 }, { 0x80dc80f, 2 }, { 0x80dc810, 2 }, { 0x80dc815, 2 }, { 0x80dc816, 2 }, { 0, 0 } },
	{ { 0x811880a, 0 }, { 0x811880b, 0 }, { 0x811880c, 0 }, { 0x811880e, 0 }, { 0x8128817, 1 }, { 0x811880f, 2 }, { 0x8118810, 2 }, { 0x8118815, 2 }, { 0x8118816, 2 }, { 0, 0 } },
	{ { 0x816080a, 0 }, { 0x816080b, 0 }, { 0x816080c, 0 }, { 0x816080e, 0 }, { 0x816a817, 1 }, { 0x816080f, 2 }, { 0x8160810, 2 }, { 0x8160816, 2 }, { 0x8160817, 2 }, { 0, 0 } },
	{ { 0x819080a, 0 }, { 0x819080b, 0 }, { 0x819080c, 0 }, { 0x819080e, 0 }, { 0x81a0817, 1 }, { 0x819080f, 2 }, { 0x8190810, 2 }, { 0x8190815, 2 }, { 0x8190816, 2 }, { 0, 0 } },
	{ { 0x81e280a, 0 }, { 0x81e280b, 0 }, { 0x81e280c, 0 }, { 0x81e280e, 0 }, { 0x81e6817, 1 }, { 0x81e280f, 2 }, { 0x81e2810, 2 }, { 0x81e2815, 2 }, { 0x81e2816, 2 }, { 0, 0 } },
	{ { 0x821480a, 0 }, { 0x821480b, 0 }, { 0x821480c, 0 }, { 0x821480e, 0 }, { 0x8222817, 1 }, { 0x821480f, 2 }, { 0x8214810, 2 }, { 0x8214816, 2 }, { 0x8214817, 2 }, { 0x821481c, 2 } },
	{ { 0x825480a, 0 }, { 0x825480b, 0 }, { 0x825480c, 0 }, { 0x825480e, 0 }, { 0x8262817, 1 }, { 0x825480f, 2 }, { 0x8254810, 2 }, { 0x8254815, 2 }, { 0x8254816, 2 }, { 0, 0 } },
	{ { 0x829480a, 0 }, { 0x829480b, 0 }, { 0x829480c, 0 }, { 0x829480e, 0 }, { 0x82a2817, 1 }, { 0x8294811, 2 }, { 0x8294812, 2 }, { 0x8294817, 2 }, { 0x8294818, 2 }, { 0, 0 } },
	{ { 0x82e0800, 1 }, { 0x82d4804, 2 }, { 0x82d4803, 2 }, { 0x82d480f, 2 }, { 0x82d4810, 2 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ { 0x831e80c, 1 }, { 0x831480c, 2 }, { 0x831480d, 2 }, { 0x8314811, 2 }, { 0x8314812, 2 }, { 0x8304021, 3 }, { 0x8304021, 4 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ { 0x839c80c, 2 }, { 0x839c80d, 2 }, { 0x839c811, 2 }, { 0x839c812, 2 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
};

// On the JP object at 0xe73e14 (unplaced): the part typeid of the first row
// of the character type (tid & 0x3ffffff) that gives `slot` and whose part
// is one of the list's 24 m_partTids; 0 when none.
// 0x4d0bd0
uint32_t Jp_MotionPartRule_Check(uint32_t characterTid, const CPartTidList* parts, unsigned int slot) {
	uint32_t charTypeId = characterTid & 0x3ffffff;
	// Ours: the JP indexes the table unchecked.
	if (charTypeId >= 15) {
		return 0;
	}
	const sJpPartMotionRow* rows = s_jpPartMotionRows[charTypeId];
	for (int r = 0; r < 10; r++) {
		if (rows[r].slot == slot) {
			for (int i = 0; i < 24; i++) {
				if (rows[r].partTypeId != 0 && parts->m_partTids[i] == rows[r].partTypeId) {
					return rows[r].partTypeId;
				}
			}
		}
	}
	return 0;
}

// On the JP object at 0xe73e14: whether the motion is the motion key of one
// of the 5 part motion slots; slot receives its index.
// 0x4d0cf0
bool JpPartMotions_Contains(const char* motion, unsigned int& slot) {
	for (unsigned int i = 0; i < 5; i++) {
		if (_stricmp(Jp_LookupText(s_jpPartMotionKeys[i][1]), motion) == 0) {
			slot = i;
			return true;
		}
	}
	return false;
}

// The 24 dwords 0x4d0fa0 compares with a row's part typeid, from the start
// of the character data (the sCharacterInfo typeId, id, then partTypeIds).
static uint32_t JpCharacterInfo_GetDword(const sCharacterInfo* info, int i) {
	if (i == 0) {
		return (uint32_t)info->typeId;
	}
	if (i == 1) {
		return (uint32_t)info->id;
	}
	return (uint32_t)info->partTypeIds[i - 2];
}

// On the JP object at 0xe73e14: the motion key and caption of a part motion
// slot for a character type: every row of the type that gives the slot and
// whose part the character wears assigns them (the last one wins); both are
// left as they are without one.
// 0x4d0fa0
void JpPartMotions_Get(int charTypeId, const sCharacterInfo* info, unsigned int slot, std::string& motion, std::string& caption) {
	// Ours: the JP indexes the table unchecked and reads info without a
	// NULL test (GetCharacterInfo can return NULL from the lobby).
	if ((unsigned int)charTypeId >= 15 || !info) {
		return;
	}
	const sJpPartMotionRow* rows = s_jpPartMotionRows[charTypeId];
	for (int r = 0; r < 10; r++) {
		for (int i = 0; i < 24; i++) {
			if (rows[r].partTypeId != 0 && slot == rows[r].slot && JpCharacterInfo_GetDword(info, i) == rows[r].partTypeId) {
				motion = Jp_LookupText(s_jpPartMotionKeys[rows[r].slot][1]);
				caption = Jp_LookupText(s_jpPartMotionKeys[rows[r].slot][0]);
				break;
			}
		}
	}
}

const WRTTI FrAvatarMotionDlg::m_RTTI("FrAvatarMotionDlg", &FrForm::m_RTTI);

// 0x47f2a0
const sFRESH_MSGMAP* FrAvatarMotionDlg::GetMessageMap(void) const {
	return &FrAvatarMotionDlg::_MsgMap;
}

sFRESH_MSGMAP FrAvatarMotionDlg::_MsgMap = { &FrForm::_MsgMap, &FrAvatarMotionDlg::_MsgEntries[0] };

sFRESH_ENTRY FrAvatarMotionDlg::_MsgEntries[] = {
	{ "motionlist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAvatarMotionDlg::OnMotionListInit },
	{ "motionlist", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAvatarMotionDlg::OnMotionListOwnerDraw },
	{ "motionlist", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrAvatarMotionDlg::OnMotionListLBtnDown },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x47f2b0
void FrAvatarMotionDlg::UpdateCenterX(void) {
	m_centerX = (m_listRect.w + m_listRect.x + m_listRect.x) * 0.5;
}

// 0x47f2d0
void FrAvatarMotionDlg::LoadBitmaps(void) {
	m_pHoverBitmap = g_pFresh->GetBitmap("motionchat_list_Hover");
	m_pPlayBitmap = g_pFresh->GetBitmap("motionchat_list_play");
	m_pStopBitmap = g_pFresh->GetBitmap("motionchat_list_stop");
}

// 0x47f930
void FrAvatarMotionDlg::OnMotionListInit(intptr_t param) {
	m_pMotionList = DYNAMIC_CAST(FrListBox, param);
	// JP: or [list+0x44], 0x200 (the JP FrWnd m_dwStyle).
	m_pMotionList->UseDblClick(false);
}

// 0x47f9b0
void FrAvatarMotionDlg::InitLayout(void) {
	m_listRect.x = g_view->GetWidth() - 76.0;
	m_listRect.y = 0.0f;
	m_listRect.w = GetRect().w - 11.0;
	m_listRect.h = 24.0f;
}

// 0x47f9f0
void FrAvatarMotionDlg::InitButtonLayout(void) {
	m_buttonRect.x = m_listRect.x - 13.0;
	m_buttonRect.y = 0.0f;
	m_buttonRect.w = (float)m_pPlayBitmap->Width();
	m_buttonRect.h = (float)m_pPlayBitmap->Height();
}

// 0x47fa50
void FrAvatarMotionDlg::ProcStateOpening(const float& dt) {
	WRect rect(GetRect().x, GetRect().y, GetRect().w, dt * 3000.0 + GetRect().h);
	SetRect(rect);
	float height = (int)m_shownItems.size() * 24.0 + 25.0;
	if (height <= GetRect().h) {
		WRect full(GetRect().x, GetRect().y, GetRect().w, height);
		SetRect(full);
		m_stateFlags = 1;
	}
}

// 0x47fb30
void FrAvatarMotionDlg::ProcStateClosing(const float& dt) {
	WRect rect(GetRect().x, GetRect().y, GetRect().w, GetRect().h - dt * 3000.0);
	SetRect(rect);
	if (GetRect().h <= 40.0f) {
		WRect closed(GetRect().x, GetRect().y, GetRect().w, 40.0f);
		SetRect(closed);
		m_alpha = 0.0f;
		m_stateFlags = 2;
		Close(true);
	}
}

// 0x47fbf0: the original passes the JP ntAvatar +0x82 (the nickname) to
// FindAvatarByNickName 0x7a32a0.
void FrAvatarMotionDlg::DrawAvatarChatRowButtons(FrListItem* pItem) {
	ntAvatarManager* manager = WSingleton<ntAvatarManager>::Instance();
	ntAvatar* avatar = manager->FindAvatarByNickName(manager->m_pMyAvatar->m_nickName);
	int selected = m_selected;
	if (selected == pItem->idx) {
		int state = JpAvatar_GetMotionState(avatar);
		if (state == 4 || state == 5) {
			m_buttonRect.y = selected * 24.0 + m_pMotionList->GetRect().y;
			g_pFresh->GetManager()->GetGDI()->DrawTexture(m_pHoverBitmap, m_listRect, 0xffffffff, 0);
			g_pFresh->GetManager()->GetGDI()->DrawTexture(m_pPlayBitmap, m_buttonRect, 0xffffffff, 0);
		}
	}
}

// 0x47fca0
void FrAvatarMotionDlg::DrawLobbyRowButtons(FrListItem* pItem) {
	int selected = m_selected;
	if (selected != pItem->idx) {
		return;
	}
	m_buttonRect.y = selected * 24.0 + m_pMotionList->GetRect().y;
	g_pFresh->GetManager()->GetGDI()->DrawTexture(m_pHoverBitmap, m_listRect, 0xffffffff, 0);
	g_pFresh->GetManager()->GetGDI()->DrawTexture(m_bool169 ? m_pPlayBitmap : m_pStopBitmap, m_buttonRect, 0xffffffff, 0);
}

// 0x47fd70
void FrAvatarMotionDlg::ProcList(const float& dt) {
	if (CMouseCursor::Instance()->InArea(GetRect())) {
		m_childAlpha += dt + dt;
		if (m_childAlpha > 1.0f) {
			m_childAlpha = 1.0f;
		}
		// JP 0xbe7740, a JP FrWnd member: m_pWndManager +0x8c = b (the
		// KR FrWndManager m_hidePrivacy offset).
		WndManager()->HidePrivacy(true);
	} else {
		m_childAlpha -= dt + dt;
		if (m_childAlpha < 0.7f) {
			m_childAlpha = 0.7f;
		}
	}
	SetAlpha2ToChild(m_childAlpha);
}

// 0x480270: the JP returns the character data's part list (JP +0xc, the
// KR partTypeIds); ours the whole KR record.
const sCharacterInfo* FrAvatarMotionDlg::GetCharacterInfo(void) {
	if (m_flags & 2) {
		return Jp_GameState_GetCharacterInfo(-1);
	}
	return &WSingleton<ntAvatarManager>::Instance()->m_pMyAvatar->GetCharacterInfo();
}

// 0x4802b0
void FrAvatarMotionDlg::DrawAvatarChatRowText(const int& index, const WPoint& pos) {
	int row = (int)m_shownItems.size();
	if (index < 0) {
		row = 0;
	} else if (index <= row) {
		row = index;
	}
	g_pFresh->GetManager()->PrintText(pos, 1, m_shownItems[row].caption.c_str(), 70.0f, (unsigned long)(unsigned char)(int)m_alpha << 24);
}

// 0x480360: the random row (the last) is red while m_avatarState is set.
void FrAvatarMotionDlg::DrawLobbyRowText(const int& index, const WPoint& pos) {
	if (m_avatarState && (int)m_shownItems.size() - 1 == index) {
		g_pFresh->GetManager()->GetGDI()->SetTextColor(0xff0000, 0xffffffff);
	}
	int row = (int)m_shownItems.size();
	if (index < 0) {
		row = 0;
	} else if (index <= row) {
		row = index;
	}
	g_pFresh->GetManager()->PrintText(pos, 1, m_shownItems[row].caption.c_str(), 70.0f, (unsigned long)(unsigned char)(int)m_alpha << 24);
}

// 0x480450
void FrAvatarMotionDlg::OnProc(const float dt) {
	if (m_stateFlags & 8) {
		ProcStateOpening(dt);
	} else if (m_stateFlags & 4) {
		ProcStateClosing(dt);
	}
	ProcList(dt);
	if (m_alpha != 255.0) {
		if (m_alpha < 255.0) {
			m_alpha += 5.0;
		} else {
			m_alpha = 255.0f;
		}
	}
	if (g_input->GetDown("ESCAPE", true) == 1) {
		Close(true);
	}
}

// 0x480590: the last row (random) plays a random motion of the rows after
// the first two categories when m_avatarState is set, otherwise asks the
// lobby avatar to stop (0x196).
void FrAvatarMotionDlg::SelectLobbyMotion(const int& index) {
	int row = index;
	if (row == (int)m_shownItems.size() - 1) {
		if (!m_avatarState) {
			AfxGetTask()->GetActor("LobbyAvatar") << MsgObject(0, 0x196, 0, 0, 0, 0, 0);
			return;
		}
		int first = (int)m_categoryItems[1].size() + (int)m_categoryItems[0].size();
		row = float2int(Random((float)first, (float)(row - 1), -1));
	}
	int shown = (int)m_shownItems.size();
	if (row < 0) {
		shown = 0;
	} else if (row <= shown) {
		shown = row;
	}
	AfxGetTask()->GetActor("LobbyAvatar") << MsgObject(0, 0x199, (intptr_t)m_shownItems[shown].motion.c_str(), index, m_avatarState, 0, 0);
}

// 0x480730
void FrAvatarMotionDlg::SelectAvatarChatMotion(const int& index) {
	int row = (int)m_shownItems.size();
	if (index < 0) {
		row = 0;
	} else if (index <= row) {
		row = index;
	}
	AfxGetTask()->GetActor("AvatarChat") << MsgObject(0, 0x199, (intptr_t)m_shownItems[row].motion.c_str(), 0, 0, 0, 0);
}

// 0x480a60
void FrAvatarMotionDlg::DrawRowText(const int& index, const WPoint& pos) {
	if (m_flags & 1) {
		DrawAvatarChatRowText(index, pos);
	} else if (m_flags & 2) {
		DrawLobbyRowText(index, pos);
	}
}

// 0x480a90
void FrAvatarMotionDlg::SelectMotion(const int& index) {
	if (m_flags & 2) {
		SelectLobbyMotion(index);
	} else if (m_flags & 1) {
		SelectAvatarChatMotion(index);
	}
}

// 0x480c30
void FrAvatarMotionDlg::DrawRow(FrListItem* pItem) {
	unsigned long textColor = g_pFresh->GetManager()->GetGDI()->GetTextColor();
	g_pFresh->GetManager()->GetGDI()->SetTextStyle(1);
	m_listRect.y = pItem->idx * 24.0 + m_pMotionList->GetRect().y;
	if (m_pMotionList->GetSelected() == pItem) {
		g_pFresh->GetManager()->GetGDI()->DrawTexture(m_pHoverBitmap, m_listRect, 0xffffffff, 0);
		if (m_flags & 2) {
			m_buttonRect.y = m_listRect.y;
			g_pFresh->GetManager()->GetGDI()->DrawTexture(m_pPlayBitmap, m_buttonRect, 0xffffffff, 0);
		}
	}
	if ((m_flags & 1) && m_bool168) {
		DrawAvatarChatRowButtons(pItem);
	} else if ((m_flags & 2) && m_bool168) {
		DrawLobbyRowButtons(pItem);
	}
	if (pItem->pData) {
		WPoint pos(m_centerX, m_listRect.y + 7.0);
		DrawRowText(pItem->idx, pos);
		g_pFresh->GetManager()->GetGDI()->SetTextColor(textColor, 0xffffffff);
		g_pFresh->GetManager()->GetGDI()->SetTextStyle(0);
	}
}

// 0x480d70
void FrAvatarMotionDlg::OnMotionListOwnerDraw(intptr_t param) {
	if (param && (m_stateFlags & 1)) {
		DrawRow(reinterpret_cast<FrListItem*>(param));
	}
}

// 0x480e00
void FrAvatarMotionDlg::OnMotionListLBtnDown(void) {
	FrListItem* pItem = m_pMotionList->GetSelected();
	if (!pItem || !pItem->pData) {
		return;
	}
	m_selected = pItem->idx;
	if (!m_avatarState && pItem->idx == (int)m_shownItems.size() - 1) {
		m_avatarState = true;
		m_bool168 = true;
		SelectMotion(pItem->idx);
		return;
	}
	m_avatarState = false;
	m_bool168 = true;
	SelectMotion(pItem->idx);
}

// 0x481070
void FrAvatarMotionDlg::FindMotionName(const char* motion, sAvatarMotionItem& item) {
	for (unsigned int i = 0; i < m_motionNames.size(); i++) {
		if (_stricmp(m_motionNames[i].motion, motion) == 0) {
			item.motion = m_motionNames[i].motion;
			item.caption = m_motionNames[i].name;
			return;
		}
	}
}

// 0x481960
void FrAvatarMotionDlg::LoadMotionNames(const int& typeId) {
	MsgObject msg(0, 0x198, 0, 0, 0, 0, 0);
	if (m_flags & 1) {
		AfxGetTask()->GetActor("AvatarChat") << msg;
	} else if (m_flags & 2) {
		AfxGetTask()->GetActor("LobbyAvatar") << msg;
	}
	// The actor answers in the message's first parameter.
	m_pAnimSet = reinterpret_cast<CPetFrame*>(msg.m_param1);
	if (!m_pAnimSet) {
		return;
	}
	m_motionCount = JpMotionTable_GetCount(typeId);
	int count = 0;
	for (int i = 0; i < m_motionCount; i++) {
		const char* motion = JpMotionTable_GetMotion(typeId, i);
		// 0x8af590: KR CPetFrame::FindMotionData.
		if (m_pAnimSet->FindMotionData(motion, true, 0)) {
			const char* name = JpMotionTable_GetName(typeId, i);
			unsigned int partSlot = 5;
			if (!JpPartMotions_Contains(motion, partSlot)) {
				sAvatarMotionName row;
				row.motion = motion;
				row.name = name;
				m_motionNames.push_back(row);
				count++;
			}
		}
	}
	m_motionCount = count;
}

// 0x481b20
void FrAvatarMotionDlg::SetCharacterTypeId(const int& typeId) {
	LoadMotionNames(typeId);
}

// 0x481dc0
FrAvatarMotionDlg::FrAvatarMotionDlg(void) {
	m_pMotionList = 0;
	m_motionCount = 0;
	m_selected = -1;
	m_alpha = 0.0f;
	m_centerX = 0.0f;
	m_childAlpha = 0.0f;
	m_pPlayBitmap = 0;
	m_pHoverBitmap = 0;
	m_bool168 = false;
	m_bool169 = true;
	m_bool16a = false;
	m_avatarState = false;
	m_flags = 0;
	m_stateFlags = 8;
}

// 0x481f00
FrAvatarMotionDlg::~FrAvatarMotionDlg(void) {
}

// 0x482010
void FrAvatarMotionDlg::ClearLists(void) {
	m_categoryItems[0].clear();
	m_categoryItems[1].clear();
	m_categoryItems[2].clear();
	m_categoryItems[3].clear();
	m_categoryItems[4].clear();
	m_shownItems.clear();
}

// 0x4821c0
IObject* FrAvatarMotionDlgMakeInstance(void) {
	return new FrAvatarMotionDlg;
}

// 0x482230 (static init): registration with the form factory
struct __sFrAvatarMotionDlg {
	__sFrAvatarMotionDlg(void) {
		ObjectFactory().AddObjectFunctor(FrAvatarMotionDlgMakeInstance, "FrAvatarMotionDlg");
	}
};
static __sFrAvatarMotionDlg s_registerFrAvatarMotionDlg;

// Inlined in 0x482280 and 0x4825e0.
int FrAvatarMotionDlg::GetCharacterTypeId(void) {
	if (m_flags & 2) {
		if (RFLobbyDesktop::s_instance) {
			return RFLobbyDesktop::s_instance->GetSelectedCharacterTypeId();
		}
		return 0;
	}
	return WSingleton<ntAvatarManager>::Instance()->m_pMyAvatar->m_charTypeId & 0x3ffffff;
}

// 0x482280
bool FrAvatarMotionDlg::LoadScript(std::string file) {
	m_scriptItems.clear();
	lua_system::lua_script* script = lua_system::lua_script_manger::GetInstance()->CreateScript(false);
	if (!script) {
		return false;
	}
	if (!script->RunFile(file.c_str())) {
		return false;
	}
	lua_system::lua_table_helper table = script->GetTable(MakeStr("best_motion_name_char%d", GetCharacterTypeId()));
	for (int i = 0; i < table.GetArraySize();) {
		sAvatarMotionItem item;
		++i;
		FindMotionName(table.GetValueByIndex<const char*>(i), item);
		item.kind = 2;
		m_scriptItems.push_back(item);
	}
	lua_system::lua_script_manger::GetInstance()->ReleaseScript(script);
	return true;
}

// 0x4824c0: "\xb1\xe2\xba\xbb" and "\xb1\xe2\xba\xbb\xc0\xda\xbc\xbc" are the
// EUC-KR text keys "basic" and "basic posture".
void FrAvatarMotionDlg::AddBasicItem(void) {
	m_categoryItems[0].push_back(sAvatarMotionItem(0, Jp_LookupText("\xb1\xe2\xba\xbb"), Jp_LookupText("\xb1\xe2\xba\xbb\xc0\xda\xbc\xbc")));
}

// 0x4825e0
void FrAvatarMotionDlg::BuildCategory1(void) {
	int typeId = GetCharacterTypeId();
	for (unsigned int i = 0; i < JpPartMotions_GetCount(); i++) {
		sAvatarMotionItem item;
		JpPartMotions_Get(typeId, GetCharacterInfo(), i, item.motion, item.caption);
		if (!item.motion.empty() && !item.caption.empty()) {
			item.kind = 1;
			m_categoryItems[1].push_back(item);
		}
	}
}

// 0x4827b0: the script's "best" motions (LoadScript), copied.
void FrAvatarMotionDlg::BuildCategory2(void) {
	for (unsigned int i = 0; i < m_scriptItems.size(); i++) {
		m_categoryItems[2].push_back(sAvatarMotionItem(m_scriptItems[i].kind, m_scriptItems[i].caption, m_scriptItems[i].motion));
	}
}

// 0x482a80: up to 14 rows in all with the first two shown categories (12
// from the lobby) of the character's other motions, in random order:
// m_motionNames' rows that are neither in m_categoryItems[2] nor part
// motions (kind 3).
void FrAvatarMotionDlg::BuildCategory3(void) {
	std::vector<sAvatarMotionName> candidates;
	int limit;
	if (m_flags & 2) {
		limit = 12 - (int)m_categoryItems[2].size() - (int)m_categoryItems[1].size();
	} else {
		limit = 14 - (int)m_categoryItems[2].size() - (int)m_categoryItems[1].size();
	}
	for (unsigned int i = 0; i < m_motionNames.size(); i++) {
		unsigned int j;
		for (j = 0; j < m_categoryItems[2].size(); j++) {
			if (_stricmp(m_motionNames[i].motion, m_categoryItems[2][j].motion.c_str()) == 0) {
				break;
			}
		}
		if (j < m_categoryItems[2].size()) {
			continue;
		}
		unsigned int partSlot = 5;
		if (!JpPartMotions_Contains(m_motionNames[i].motion, partSlot)) {
			candidates.push_back(m_motionNames[i]);
		}
	}
	if (candidates.empty()) {
		return;
	}
	// lib: 0x480af0 (std::random_shuffle)
	std::random_shuffle(candidates.begin(), candidates.end());
	for (int i = 0; i < limit; i++) {
		if ((unsigned int)i > candidates.size() - 1) {
			break;
		}
		sAvatarMotionItem item;
		item.kind = 3;
		item.motion = candidates[i].motion;
		item.caption = candidates[i].name;
		m_categoryItems[3].push_back(item);
	}
}

// 0x482fd0: "(\xb7\xa3\xb4\xfd)" is the EUC-KR text key "(random)".
void FrAvatarMotionDlg::AddRandomItem(void) {
	m_categoryItems[4].push_back(sAvatarMotionItem(4, Jp_LookupText("(\xb7\xa3\xb4\xfd)"), Jp_LookupText("")));
}

// 0x4830f0
void FrAvatarMotionDlg::AppendItems(std::vector<sAvatarMotionItem>& items) {
	for (unsigned int i = 0; i < items.size(); i++) {
		m_shownItems.push_back(items[i]);
	}
}

// 0x483170
void FrAvatarMotionDlg::BuildLists(void) {
	ClearLists();
	AddBasicItem();
	BuildCategory1();
	BuildCategory2();
	BuildCategory3();
	AddRandomItem();
}

// 0x4831a0
void FrAvatarMotionDlg::RefreshList(void) {
	m_shownItems.clear();
	if (m_flags & 2) {
		AppendItems(m_categoryItems[0]);
	}
	AppendItems(m_categoryItems[1]);
	AppendItems(m_categoryItems[2]);
	AppendItems(m_categoryItems[3]);
	if (m_flags & 2) {
		AppendItems(m_categoryItems[4]);
	}
	for (unsigned int i = 0; i < m_shownItems.size(); i++) {
		m_pMotionList->AddItem(&m_shownItems[i]);
	}
}

// 0x4832a0
bool FrAvatarMotionDlg::OnInit(void) {
	LoadScript("motiondlg.lua");
	BuildLists();
	RefreshList();
	InitLayout();
	UpdateCenterX();
	LoadBitmaps();
	InitButtonLayout();
	// The original reads m_flags before asking the actor and stores it back
	// after the answer (written to m_avatarState).
	unsigned int flags = m_flags;
	IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
	if (actor) {
		actor << MsgObject(0, 0x197, (intptr_t)&m_avatarState, 0, 0, 0, 0);
	}
	m_flags = flags;
	return FrForm::OnInit();
}

// 0x7b1130
FrAvatarMotionDlg* CreateAvatarMotionDlg(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent) {
	return CreateForm<FrAvatarMotionDlg>(manager, owner, name, parent);
}
