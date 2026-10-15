// Original: d:\build\custom\temp\code\client\projectg\gmtoolkit.cpp
#include <atlbase.h>	// stand-in for the precompiled header: _AtlWinModule initializer
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <string>

#define GMTOOLKIT_GMTOOLKIT_INLINE	// the out-of-line copy of GetPrizeSwitch lives here
#include "GMToolkit.h"
#include "frlistbox.h"
#include "ChatMsg.h"
#include "commonutil.h"
#include "Packet.h"
#include "SharedDoc.h"
#include "ItemManager.h"
#include "UccManager.h"

// The first help line. The original fills the table from this entry on with
// a dynamic initializer (sub_009b9b90), the rows before it are static data:
// a const char* const value is not a constant expression for cl.
static const char* const s_helpHelp = "/help [cmd] :: \xc1\xf6\xc1\xa4\xc7\xd1 \xb8\xed\xb7\xc9\xc0\xc7 \xbf\xeb\xb5\xb5 \xbc\xb3\xb8\xed.";

// gmtoolkit.cpp:26
// The GM command table, searched by keyword (GetGMCommandTableNumber).
static sGMCommand data_aa53d8[] = {
	{ false, GM_COMMAND_NONE, "dummy", "/dummy :: for Ntreev Soft." },
	{ true, GM_COMMAND_HELP, "help", s_helpHelp },
	{ true, GM_COMMAND_COMMAND, "command", "/command :: \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xc0\xd6\xb4\xc2 \xb8\xed\xb7\xc9\xbe\xee \xb8\xae\xbd\xba\xc6\xae \xc3\xe2\xb7\xc2." },
	{ true, GM_COMMAND_SHOW_STATUS, "status", "/status :: \xc7\xf6\xc0\xe7 \xbc\xb3\xc1\xa4\xb5\xc8 \xbb\xf3\xc5\xc2\xb8\xa6 \xc3\xe2\xb7\xc2\xc7\xd5\xb4\xcf\xb4\xd9." },
	{ true, GM_COMMAND_VISIBLE, "visible", "/visible [on/off] :: \xb7\xce\xba\xf1\xbf\xa1\xbc\xad \xc0\xaf\xc0\xfa\xb0\xa1 \xc0\xda\xbd\xc5\xc0\xbb \xba\xbc \xbc\xf6 \xc0\xd6\xb5\xb5\xb7\xcf \xc7\xd5\xb4\xcf\xb4\xd9." },
	{ true, GM_COMMAND_WHISPER, "whisper", "/whisper [on/off] :: \xb8\xf0\xb5\xe7 \xc0\xaf\xc0\xfa\xc0\xc7 \xb1\xd3\xbc\xd3\xb8\xbb\xc0\xbb \xc7\xe3\xbf\xeb\xc7\xd5\xb4\xcf\xb4\xd9." },
	{ true, GM_COMMAND_CHANNEL, "channel", "/channel [on/off] :: \xc7\xe3\xbf\xeb\xb5\xc8 \xc0\xaf\xc0\xfa\xbf\xcd \xb1\xd3\xbc\xd3\xb8\xbb\xc0\xbb \xb0\xa1\xb4\xc9\xc7\xcf\xb0\xd4 \xc7\xd5\xb4\xcf\xb4\xd9." },
	{ true, GM_COMMAND_LIST, "list", "/list :: \xb1\xd3\xbc\xd3\xb8\xbb\xc0\xcc \xc7\xe3\xbf\xeb\xb5\xc8 \xc0\xaf\xc0\xfa \xb8\xae\xbd\xba\xc6\xae\xb8\xa6 \xc3\xe2\xb7\xc2\xc7\xd5\xb4\xcf\xb4\xd9." },
	{ true, GM_COMMAND_OPEN, "open", "/open [nick] :: \xc1\xf6\xc1\xa4\xb5\xc8 \xc0\xaf\xc0\xfa\xc0\xc7 \xb1\xd3\xbc\xd3\xb8\xbb\xc0\xbb \xc7\xe3\xbf\xeb\xc7\xd5\xb4\xcf\xb4\xd9." },
	{ true, GM_COMMAND_CLOSE, "close", "/close [nick] :: \xc1\xf6\xc1\xa4\xb5\xc8 \xc0\xaf\xc0\xfa\xb8\xa6 \xb1\xd3\xbc\xd3\xb8\xbb \xc7\xe3\xbf\xeb \xb8\xae\xbd\xba\xc6\xae\xbf\xa1\xbc\xad \xc1\xa6\xb0\xc5\xc7\xd5\xb4\xcf\xb4\xd9." },
	{ true, GM_COMMAND_KICK_USER, "kick", "/kick [nick][op] :: \xb4\xeb\xbb\xf3\xc0\xbb \xc5\xb1\xc7\xd5\xb4\xcf\xb4\xd9 '-p' \xbf\xc9\xbc\xc7\xc0\xbb \xc1\xd6\xb8\xe9 \xb0\xad\xc1\xa6 \xc1\xbe\xb7\xe1\xc0\xb2\xc0\xcc \xbf\xc3\xb6\xf3\xb0\xa9\xb4\xcf\xb4\xd9." },
	{ true, GM_COMMAND_DISCONNECT, "disconnect", "/disconnect [nick] :: \xb4\xeb\xbb\xf3\xc0\xbb \xb5\xf0\xbd\xba\xc4\xbf\xb3\xd8\xc6\xae \xbd\xc3\xc5\xb5\xb4\xcf\xb4\xd9." },
	{ true, GM_COMMAND_DISCON_UID, "discon_uid", "/discon_uid [uid] :: \xb4\xeb\xbb\xf3\xc0\xbb \xb5\xf0\xbd\xba\xc4\xbf\xb3\xd8\xc6\xae \xbd\xc3\xc5\xb5\xb4\xcf\xb4\xd9." },
	{ true, GM_COMMAND_DESTROY, "destroy", "/destroy [num] :: \xc1\xf6\xc1\xa4\xb5\xc8 \xb9\xf8\xc8\xa3\xc0\xc7 \xb9\xe6\xc0\xbb \xc6\xf8\xc6\xc4\xc7\xd5\xb4\xcf\xb4\xd9." },
	{ true, GM_COMMAND_WIND, "wind", "/wind [spd][dir] :: \xc1\xf6\xc1\xa4\xb5\xc8 \xbc\xd3\xb5\xb5\xbf\xcd \xb9\xe6\xc7\xe2\xc0\xb8\xb7\xce \xb9\xd9\xb6\xf7\xc0\xbb \xbc\xb3\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9." },
	{ true, GM_COMMAND_WEATHER, "weather", "/weather [type] :: \xb3\xaf\xbe\xbe\xb8\xa6 \xbc\xb3\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9. 'fine', 'rain', 'snow', 'cloud'" },
	{ true, GM_COMMAND_IDENTITY, "identity", "/identity [type] :: \xb0\xe8\xc1\xa4 \xb1\xc7\xc7\xd1\xc0\xbb \xbc\xb3\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9. 'admin', 'user'" },
	{ true, GM_COMMAND_NOTICE, "notice", "/notice [msg] :: \xb8\xde\xbc\xbc\xc1\xf6\xb8\xa6 \xc3\xa4\xc6\xc3\xc3\xa2\xbf\xa1 \xb0\xf8\xc1\xf6\xc7\xd5\xb4\xcf\xb4\xd9." },
	{ true, GM_COMMAND_GIVE_ITEM, "giveitem", "/giveitem [nick][typeid][num] :: \xc1\xf6\xc1\xa4\xb5\xc8 \xbb\xe7\xb6\xf7\xbf\xa1\xb0\xd4 \xbe\xc6\xc0\xcc\xc5\xdb \xbc\xb1\xb9\xb0." },
	{ true, GM_COMMAND_GOLDEN_BELL, "goldenbell", "/goldenbell [typeid][num] :: \xc7\xf6\xc0\xe7 \xb7\xeb\xbe\xc8\xc0\xc7 \xb8\xf0\xb5\xe7 \xbb\xe7\xb6\xf7\xbf\xa1\xb0\xd4 \xbe\xc6\xc0\xcc\xc5\xdb \xbc\xb1\xb9\xb0." },
	{ true, GM_COMMAND_SET_PRIZE, "setprize", "/setprize [switch];[dest][typeid][num]" },
	{ true, GM_COMMAND_UNSET_PRIZE, "unsetprize", "/unsetprize :: setprize\xb7\xce \xbc\xb3\xc1\xa4\xb5\xc8 \xb3\xbb\xbf\xeb\xc0\xbb \xbb\xe8\xc1\xa6 \xc7\xd5\xb4\xcf\xb4\xd9." },
	{ true, GM_COMMAND_SHOW_PRIZE, "showprize", "/showprize :: setprize\xb7\xce \xbc\xb3\xc1\xa4\xb5\xc8 \xb3\xbb\xbf\xeb\xc0\xbb \xc3\xe2\xb7\xc2 \xc7\xd5\xb4\xcf\xb4\xd9." },
	{ false, GM_COMMAND_LOAD_SCRIPT, "loadscript", "/loadscript [file] :: \xc6\xc4\xc0\xcf\xbf\xa1 \xbc\xb3\xc1\xa4\xb5\xc8 \xb3\xbb\xbf\xeb\xc0\xbb \xb0\xd4\xc0\xd3\xbf\xa1 \xc0\xfb\xbf\xeb\xc7\xd5\xb4\xcf\xb4\xd9." },
	{ true, GM_COMMAND_NOTICE_PRIZE, "noticeprize", "/noticeprize :: \xbb\xf3\xc7\xb0\xc1\xf6\xb1\xde \xc1\xb6\xb0\xc7 \xb0\xf8\xc1\xf6." },
	{ false, GM_COMMAND_GET_TID, "gettid", "/gettid ITEM_NAME :: \xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 TID\xb8\xa6 \xc3\xe2\xb7\xc2\xc7\xd5\xb4\xcf\xb4\xd9." },
	{ true, GM_COMMAND_SET_MISSION, "setmission", "/setmission MISSION_NUM :: \xbe\xee\xc7\xc1\xb7\xce\xc4\xa1 \xb9\xcc\xbc\xc7\xc0\xbb \xbc\xb3\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9." },
	{ true, GM_COMMAND_FIND_ITEM, "finditem", "/finditem [category][keyword] :: \xc7\xd8\xb4\xe7\xc7\xcf\xb4\xc2 \xb9\xae\xc0\xda\xbf\xad\xc0\xcc \xb5\xe9\xbe\xee\xc0\xd6\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb0\xcb\xbb\xf6." },
	{ true, GM_COMMAND_CATEGORY, "category", "/category :: finditem \xb8\xed\xb7\xc9\xc0\xbb \xc0\xa7\xc7\xd1 category \xb8\xf1\xb7\xcf \xc3\xe2\xb7\xc2" },
	{ true, GM_COMMAND_MATCH_MAP_SETTING, "matchmap", "/matchmap [mapcount] :: \xb4\xeb\xc8\xb8\xbf\xeb \xb8\xca\xc0\xbb \xbc\xb3\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9. \xb1\xe2\xba\xbb\xbc\xb3\xc1\xa4\xc0\xba \xba\xed\xb7\xe7 \xb6\xf3\xb1\xba\xc0\xd4\xb4\xcf\xb4\xd9." },
	{ true, GM_COMMAND_MATCH_HOLE_SETTING, "matchhole", "/matchhole [playhole] :: \xb4\xeb\xc8\xb8\xbf\xeb \xb4\xeb\xc0\xfc\xc8\xa6\xbc\xf6(count)\xb8\xa6 \xbc\xb3\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9. 3, 6, 9, 18\xc8\xa6 \xc1\xdf\xbf\xa1 \xbc\xb1\xc5\xc3\xc7\xcf\xbc\xbc\xbf\xe4." }
};

// gmtoolkit.h:180
eGMCommandTag CGMToolkit::GetConfirmWaitTag(void) const {
	return m_confirmWaitTag;
}

// gmtoolkit.h:184
eGMToolkitStatus CGMToolkit::GetToolKitStatus(void) const {
	return m_toolkitStatus;
}

// gmtoolkit.h:188
void CGMToolkit::SetConfirmWaitTag(eGMCommandTag tag) {
	m_confirmWaitTag = tag;
	m_toolkitStatus = GM_TOOLKIT_CONFIRM_WAIT;
}

// gmtoolkit.h:193
void CGMToolkit::SetToolKitStatus(eGMToolkitStatus status) {
	m_toolkitStatus = status;
}

// gmtoolkit.cpp:75
CGMToolkit::CGMToolkit(void) {
	m_isGM = false;
	m_statusFlags = 0;
	m_arguments = 0;
	m_argumentCount = 0;
	m_toolkitStatus = GM_TOOLKIT_NORMAL;
	m_confirmWaitTag = (eGMCommandTag)0;
	m_confirmPacket = 0;

	memset(m_clearedBuffer0, 0, sizeof(m_clearedBuffer0));
	memset(m_clearedBuffer1, 0, sizeof(m_clearedBuffer1));

	m_prizeList.clear();

	m_pendingPrize = 0;
	m_prizeSwitch = false;
}

// gmtoolkit.cpp:96
CGMToolkit::~CGMToolkit(void) {
	ReleaseStringBuffer();

	if (m_confirmPacket) {
		delete m_confirmPacket;
		m_confirmPacket = 0;
	}

	if (m_pendingPrize) {
		delete m_pendingPrize;
		m_pendingPrize = 0;
	}

	for (std::list<sPrize *>::iterator it = m_prizeList.begin(); it != m_prizeList.end(); ++it) {
		if (*it) {
			delete *it;
			*it = 0;
		}
	}

	m_prizeList.clear();
}

// gmtoolkit.cpp:130
bool CGMToolkit::GetStatus(eGMStatusFlag flag) {
	return (m_statusFlags & flag) ? true : false;
}

// gmtoolkit.cpp:147
void CGMToolkit::SetStatus(eGMStatusFlag flag, eGMStatusSwitch statusSwitch) {
	if (statusSwitch == GM_STATUS_ON) {
		m_statusFlags |= flag;
	} else if (statusSwitch == GM_STATUS_OFF) {
		m_statusFlags &= ~flag;
	}
}

// gmtoolkit.cpp:161
void CGMToolkit::ReleaseStringBuffer(void) {
	for (int i = 0; i < m_argumentCount; i++) {
		if (m_arguments[i]) {
			delete[] m_arguments[i];
			m_arguments[i] = 0;
		}
	}

	if (m_arguments) {
		delete[] m_arguments;
		m_arguments = 0;
	}

	m_argumentCount = 0;
}

// gmtoolkit.cpp:184
bool CGMToolkit::PhraseSaperator(const char *command) {
	bool result = false;
	ReleaseStringBuffer();

	int length = strlen(command);

	if (command != 0) {
		if (command[0] == '/') {
			for (int i = 0; command[i] != '\0'; i++) {
				if (command[i] == command[0]) {
					if (i != 0) {
						m_argumentCount = 0;
						break;
					}
				} else {
					m_argumentCount = 1;
				}
			}
		}

		if (m_argumentCount > 0) {
			for (int i = 0; command[i] != '\0'; i++) {
				if (command[i] == ' ' && i < length) {
					if (command[i + 1] != ' ' && command[i + 1] != '\0') {
						m_argumentCount++;
					}
				}
			}

			m_arguments = new char *[m_argumentCount];
			for (int i = 0; i < m_argumentCount; i++) {
				m_arguments[i] = 0;
			}

			int remaining = m_argumentCount;
			char word[128] = "\0";
			int position = 0;
			bool inWord = false;

			for (int i = 1; command[i] != '\0'; i++) {
				if (command[i] != ' ' && command[i] != '\0') {
					word[position] = command[i];
					position++;
					inWord = true;
				}

				if ((command[i] == ' ' || command[i + 1] == '\0') && inWord == true) {
					if (remaining > 0) {
						int wordLength = strlen(word) + 1;

						m_arguments[m_argumentCount - remaining] = new char[wordLength];
						memset(m_arguments[m_argumentCount - remaining], 0, wordLength);

						strcpy(m_arguments[m_argumentCount - remaining], word);

						memset(word, 0, 64);

						remaining--;
						position = 0;
						inWord = false;
					}
				}
			}

			result = true;
		}
	}

	return result;
}

// gmtoolkit.cpp:276
int CGMToolkit::GetGMCommandTableNumber(void) {
	for (int i = 0; i < 31; i++) {
		if (strcmp(m_arguments[0], data_aa53d8[i].keyword) == 0) {
			return i;
		}
	}

	return 0;
}

// gmtoolkit.cpp:299
eGMToolResult CGMToolkit::Parser(void) {
	eGMToolkitStatus status = GetToolKitStatus();
	eGMCommandTag tag = GM_COMMAND_NONE;
	int count = 0;
	eGMToolResult result = GM_RESULT_SUCCEEDED;

	if (status == GM_TOOLKIT_NORMAL) {
		count = 31;
		int index = GetGMCommandTableNumber();
		tag = data_aa53d8[index].tag;

		if (data_aa53d8[index].enabled == false && tag != GM_COMMAND_NONE) {
			Display("\xc7\xf6\xc0\xe7 \xbb\xe7\xbf\xeb \xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb8\xed\xb7\xc9\xbe\xee \xc0\xd4\xb4\xcf\xb4\xd9.", 0xFF7802);
			return result;
		}
	} else if (status == GM_TOOLKIT_CONFIRM_WAIT) {
		tag = GetConfirmWaitTag();
	}

	switch (tag) {
	case GM_COMMAND_NONE:
		result = GM_RESULT_SYNTAX_ERROR;
		break;
	case GM_COMMAND_HELP:
		result = CommandHelp(count);
		break;
	case GM_COMMAND_COMMAND:
		result = CommandCommand(count);
		break;
	case GM_COMMAND_VISIBLE:
		result = CommandVisible(status);
		break;
	case GM_COMMAND_WHISPER:
		result = CommandWhisper(status);
		break;
	case GM_COMMAND_CHANNEL:
		result = CommandChannel(status);
		break;
	case GM_COMMAND_LIST:
		result = CommandList(status);
		break;
	case GM_COMMAND_OPEN:
		result = CommandOpen(status);
		break;
	case GM_COMMAND_CLOSE:
		result = CommandClose(status);
		break;
	case GM_COMMAND_KICK_USER:
		result = CommandKickUser(status);
		break;
	case GM_COMMAND_DISCONNECT:
		result = CommandDisconnect(status);
		break;
	case GM_COMMAND_DISCON_UID:
		result = CommandDiscon_UID(status);
		break;
	case GM_COMMAND_SHOW_STATUS:
		result = CommandShowStatus(status);
		break;
	case GM_COMMAND_DESTROY:
		result = CommandDestroy(status);
		break;
	case GM_COMMAND_WIND:
		result = CommandWind(status);
		break;
	case GM_COMMAND_WEATHER:
		result = CommandWeather(status);
		break;
	case GM_COMMAND_IDENTITY:
		result = CommandIdentity(status);
		break;
	case GM_COMMAND_NOTICE:
		result = CommandNotice(status);
		break;
	case GM_COMMAND_GIVE_ITEM:
		result = CommandGiveItem(status);
		break;
	case GM_COMMAND_GOLDEN_BELL:
		result = CommandGoldenBell(status);
		break;
	case GM_COMMAND_SET_PRIZE:
		result = CommandSetPrize(status);
		break;
	case GM_COMMAND_UNSET_PRIZE:
		result = CommandUnsetPrize(status);
		break;
	case GM_COMMAND_SHOW_PRIZE:
		result = CommandShowPrize(status);
		break;
	case GM_COMMAND_NOTICE_PRIZE:
		result = CommandNoticePrize(status);
		break;
	case GM_COMMAND_GET_TID:
		result = CommandGetTID(status);
		break;
	case GM_COMMAND_SET_MISSION:
		result = CommandSetMission(status);
		break;
	case GM_COMMAND_FIND_ITEM:
		result = CommandFindItem(status);
		break;
	case GM_COMMAND_CATEGORY:
		result = CommandCategory(status);
		break;
	case GM_COMMAND_MATCH_MAP_SETTING:
		result = CommandMatchMapSetting(status);
		break;
	case GM_COMMAND_MATCH_HOLE_SETTING:
		result = CommandMatchHoleSetting(status);
		break;
	}

	return result;
}

// gmtoolkit.cpp:462
bool CGMToolkit::IsAdministrator(void) const {
	return WSingleton<CSharedDoc>::Instance()->m_userInfo.identityBits.administrator ? true : false;
}

// gmtoolkit.cpp:470
bool CGMToolkit::IsGM(void) const {
	return m_isGM;
}

// gmtoolkit.cpp:478
bool CGMToolkit::Command(const char *command) {
	bool result = false;

	if (PhraseSaperator(command)) {
		Display(MakeStr("GM_Command:%s", command), 0x6EECFF);

		switch (Parser()) {
		case GM_RESULT_SYNTAX_ERROR:
			result = false;
			Display("Syntax error.", 0xFF7802);
			break;
		case GM_RESULT_BAD_ARGUMENT:
			result = false;
			Display("Argument error.", 0xFF7802);
			break;
		case GM_RESULT_SUCCEEDED:
			result = true;
			break;
		}
	}

	return result;
}

// gmtoolkit.cpp:512
void CGMToolkit::Display(const char *message, unsigned long color) {
	WSingleton<CChatMsg>::Instance()->AddChatMsg(message, color, true, false);
}

// gmtoolkit.cpp:519
void CGMToolkit::SendStatus(eGMCommandTag tag) {
	WSendPacket packet(GCT_GM_COMMAND);
	packet.Encode2(tag);
	packet.Encode2(m_statusFlags);
	packet.Send(SEND_TO_DEFAULT);
}

// gmtoolkit.cpp:530
void CGMToolkit::SendStringBuffer(eGMCommandTag tag, const char *buffer) {
	std::string str(buffer);

	WSendPacket packet(GCT_GM_COMMAND);
	packet.Encode2(tag);
	packet.EncodeStr(str);
	packet.Send(SEND_TO_DEFAULT);
}

// gmtoolkit.cpp:540
void CGMToolkit::SendCharacterBuffer(eGMCommandTag tag, const char *buffer) {
	int length = strlen(buffer) + 1;

	WSendPacket packet(GCT_GM_COMMAND);
	packet.Encode2(tag);
	packet.EncodeBuffer(buffer, length);
	packet.Send(SEND_TO_DEFAULT);
}

// gmtoolkit.cpp:552
void CGMToolkit::SendPrize(FrListBox *listBox) {
	int count = listBox->GetCurrentItemSize(true);

	for (std::list<sPrize *>::iterator it = m_prizeList.begin(); it != m_prizeList.end(); ++it) {
		sPrize *prize = *it;

		if (prize->m_type == 0) {
			// The players ranked m_firstRank to m_lastRank.
			for (int rank = prize->m_firstRank; rank <= prize->m_lastRank; rank++) {
				if (rank > count) {
					break;
				}

				FrListItem *item = listBox->GetItem(rank - 1);

				if (item == 0) {
					break;
				}

				sRivalData *player = (sRivalData *)item->pData;

				if (player->m_state == 3) {
					continue;
				}

				WSendPacket packet(GCT_GM_COMMAND);
				packet.Encode2(GM_COMMAND_SET_PRIZE);
				packet.Encode4(player->m_uid);
				packet.Encode4(prize->m_itemTypeId);
				packet.Encode4(prize->m_itemCount);
				packet.Encode1(prize->m_type);
				packet.Encode1(rank);
				packet.Send(SEND_TO_DEFAULT);
			}
		} else {
			// The winner of an award of the ceremony.
			for (int i = 0; i < count; i++) {
				FrListItem *item = listBox->GetItem(i);

				if (item == 0) {
					break;
				}

				sRivalData *player = (sRivalData *)item->pData;
				unsigned char award;

				switch (prize->m_type) {
				case 1:
					if (WSingleton<CSharedDoc>::Instance()->m_awardPlayers[1].m_uid != player->m_uid) {
						continue;
					}
					award = 0x65;
					break;

				case 2:
					if (WSingleton<CSharedDoc>::Instance()->m_awardPlayers[2].m_uid != player->m_uid) {
						continue;
					}
					award = 0x66;
					break;

				case 3:
					if (WSingleton<CSharedDoc>::Instance()->m_awardPlayers[5].m_uid != player->m_uid) {
						continue;
					}
					award = 0x67;
					break;

				case 4:
					if (WSingleton<CSharedDoc>::Instance()->m_awardPlayers[0].m_uid != player->m_uid) {
						continue;
					}
					award = 0x68;
					break;

				case 5:
					if (WSingleton<CSharedDoc>::Instance()->m_awardPlayers[4].m_uid != player->m_uid) {
						continue;
					}
					award = 0x69;
					break;

				case 6:
					if (WSingleton<CSharedDoc>::Instance()->m_awardPlayers[3].m_uid != player->m_uid) {
						continue;
					}
					award = 0x6A;
					break;

				default:
					continue;
				}

				if (player->m_state != 3) {
					WSendPacket packet(GCT_GM_COMMAND);
					packet.Encode2(GM_COMMAND_SET_PRIZE);
					packet.Encode4(player->m_uid);
					packet.Encode4(prize->m_itemTypeId);
					packet.Encode4(prize->m_itemCount);
					packet.Encode1(prize->m_type);
					packet.Encode1(award);
					packet.Send(SEND_TO_DEFAULT);
					// One winner per award.
					break;
				}
			}
		}
	}
}

// gmtoolkit.cpp:685
bool CGMToolkit::FindItem(unsigned long typeId, char *name) const {
	IFF_ITEM_COMMON *item = ItemManager()->FindCommonItem(typeId);

	if (item == 0) {
		return false;
	}

	strcpy(name, item->m_name);
	return true;
}

// gmtoolkit.cpp:704
bool CGMToolkit::Confirm(void) {
	bool result = false;

	if (m_argumentCount == 1) {
		if (strcmp(m_arguments[0], "y") == 0 || strcmp(m_arguments[0], "Y") == 0) {
			result = true;
		}
	} else {
		result = false;
	}

	if (result && m_confirmPacket) {
		m_confirmPacket->Send(SEND_TO_DEFAULT);
	}

	if (m_confirmPacket) {
		delete m_confirmPacket;
		m_confirmPacket = 0;
	}

	SetToolKitStatus(GM_TOOLKIT_NORMAL);
	return result;
}

// gmtoolkit.cpp:738
bool CGMToolkit::FindKeywordInString(const char *const string, const char *const keyword) {
	int stringLength = strlen(string);
	int keywordLength = strlen(keyword);
	int matched = 0;
	int i;

	for (i = 0; i < stringLength; i++) {
		if (keyword[matched] == string[i]) {
			matched++;
			if (matched == keywordLength) {
				return true;
			}
		} else {
			matched = 0;
		}
	}

	return false;
}

// gmtoolkit.cpp:769
eGMToolResult CGMToolkit::CommandHelp(int count) {
	eGMToolResult result = GM_RESULT_SUCCEEDED;

	if (m_argumentCount > 1) {
		for (int i = 1; i < count; i++) {
			if (strcmp(m_arguments[1], data_aa53d8[i].keyword) == 0) {
				if (data_aa53d8[i].enabled == true) {
					Display(data_aa53d8[i].help, 0xC6FF85);
				} else {
					Display("\xc7\xf6\xc0\xe7 \xbb\xe7\xbf\xeb \xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb8\xed\xb7\xc9\xbe\xee \xc0\xd4\xb4\xcf\xb4\xd9.", 0xFF7802);
				}

				return GM_RESULT_SUCCEEDED;
			} else {
				result = GM_RESULT_BAD_ARGUMENT;
			}
		}
	} else {
		if (m_argumentCount != 1) {
			return GM_RESULT_BAD_ARGUMENT;
		}

		Display("command \xb8\xed\xb7\xc9\xc0\xbb \xc0\xcc\xbf\xeb\xc7\xcf\xbf\xa9 \xb8\xed\xb7\xc9\xbe\xee \xc8\xae\xc0\xce\xc0\xcc \xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.", 0xC6FF85);
	}

	return result;
}

// gmtoolkit.cpp:814
eGMToolResult CGMToolkit::CommandCommand(int count) {
	for (int i = 1; i < count; i++) {
		if (data_aa53d8[i].enabled == true) {
			Display(data_aa53d8[i].keyword, 0xC6FF85);
		} else {
			Display(data_aa53d8[i].keyword, 0xFF7802);
		}
	}

	return GM_RESULT_SUCCEEDED;
}

// gmtoolkit.cpp:835
eGMToolResult CGMToolkit::CommandVisible(eGMToolkitStatus status) {
	if (m_argumentCount > 1) {
		if (strcmp(m_arguments[1], "on") == 0) {
			SetStatus(GM_STATUS_VISIBLE, GM_STATUS_ON);
			SendStatus(GM_COMMAND_VISIBLE);
			// "Status flag changed, visible on."
			Display(MakeStr("\xbb\xf3\xc5\xc2\xc7\xc3\xb7\xa1\xb1\xd7 \xba\xaf\xb0\xe6, %s", "visible on."), 0xC6FF85);
		} else if (strcmp(m_arguments[1], "off") == 0) {
			SetStatus(GM_STATUS_VISIBLE, GM_STATUS_OFF);
			SendStatus(GM_COMMAND_VISIBLE);
			Display(MakeStr("\xbb\xf3\xc5\xc2\xc7\xc3\xb7\xa1\xb1\xd7 \xba\xaf\xb0\xe6, %s", "visible off."), 0xC6FF85);
		} else {
			return GM_RESULT_BAD_ARGUMENT;
		}
	} else {
		return GM_RESULT_BAD_ARGUMENT;
	}

	return GM_RESULT_SUCCEEDED;
}

// gmtoolkit.cpp:868
eGMToolResult CGMToolkit::CommandWhisper(eGMToolkitStatus status) {
	if (m_argumentCount > 1) {
		if (strcmp(m_arguments[1], "on") == 0) {
			SetStatus(GM_STATUS_WHISPER, GM_STATUS_ON);
			SendStatus(GM_COMMAND_WHISPER);
			// "Status flag changed, whisper on." (and, as in the original,
			// "channel off.")
			Display("\xbb\xf3\xc5\xc2\xc7\xc3\xb7\xa1\xb1\xd7 \xba\xaf\xb0\xe6, whisper on.", 0xC6FF85);
			Display("\xbb\xf3\xc5\xc2\xc7\xc3\xb7\xa1\xb1\xd7 \xba\xaf\xb0\xe6, channel off.", 0xC6FF85);
		} else if (strcmp(m_arguments[1], "off") == 0) {
			SetStatus(GM_STATUS_WHISPER, GM_STATUS_OFF);
			SendStatus(GM_COMMAND_WHISPER);
			Display("\xbb\xf3\xc5\xc2\xc7\xc3\xb7\xa1\xb1\xd7 \xba\xaf\xb0\xe6, whisper off.", 0xC6FF85);
		} else {
			return GM_RESULT_BAD_ARGUMENT;
		}
	} else {
		return GM_RESULT_BAD_ARGUMENT;
	}

	return GM_RESULT_SUCCEEDED;
}

// gmtoolkit.cpp:902
eGMToolResult CGMToolkit::CommandChannel(eGMToolkitStatus status) {
	if (m_argumentCount > 1) {
		if (strcmp(m_arguments[1], "on") == 0) {
			// Channel chat and whisper exclude each other.
			m_statusFlags = (m_statusFlags & ~GM_STATUS_WHISPER) | GM_STATUS_CHANNEL;
			SendStatus(GM_COMMAND_CHANNEL);
			// "Status flag changed, channel on." and "whisper off."
			Display("\xbb\xf3\xc5\xc2\xc7\xc3\xb7\xa1\xb1\xd7 \xba\xaf\xb0\xe6, channel on.", 0xC6FF85);
			Display("\xbb\xf3\xc5\xc2\xc7\xc3\xb7\xa1\xb1\xd7 \xba\xaf\xb0\xe6, whisper off.", 0xC6FF85);
		} else if (strcmp(m_arguments[1], "off") == 0) {
			SetStatus(GM_STATUS_CHANNEL, GM_STATUS_OFF);
			SendStatus(GM_COMMAND_CHANNEL);
			Display("\xbb\xf3\xc5\xc2\xc7\xc3\xb7\xa1\xb1\xd7 \xba\xaf\xb0\xe6, channel off.", 0xC6FF85);
		} else {
			return GM_RESULT_BAD_ARGUMENT;
		}
	} else {
		return GM_RESULT_BAD_ARGUMENT;
	}

	return GM_RESULT_SUCCEEDED;
}

// gmtoolkit.cpp:937
eGMToolResult CGMToolkit::CommandList(eGMToolkitStatus status) {
	int count = m_chatPartnerList.size();

	if (count > 0) {
		for (int i = 0; i < count; i++) {
			WSingleton<CChatMsg>::Instance()->AddChatMsg(m_chatPartnerList[i], 0xC6FF85, true, false);
		}
	} else {
		// "No chat partner has been added."
		Display("\xc3\xdf\xb0\xa1\xb5\xc8 \xb4\xeb\xc8\xad\xbb\xf3\xb4\xeb\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0xFF7802);
	}

	return GM_RESULT_SUCCEEDED;
}

// gmtoolkit.cpp:960
eGMToolResult CGMToolkit::CommandOpen(eGMToolkitStatus status) {
	eGMToolResult result = GM_RESULT_SUCCEEDED;
	if (m_argumentCount > 1) {
		std::string name(m_arguments[1]);

		std::vector<std::string>::iterator it;
		it = std::find(m_chatPartnerList.begin(), m_chatPartnerList.end(), name);

		if (it != m_chatPartnerList.end()) {
			// "Already added to the chat partner list."
			Display("\xb4\xeb\xc8\xad\xbb\xf3\xb4\xeb \xb8\xae\xbd\xba\xc6\xae\xbf\xa1 \xc0\xcc\xb9\xcc \xc3\xdf\xb0\xa1\xb5\xc8 \xbb\xf3\xb4\xeb\xc0\xd4\xb4\xcf\xb4\xd9.", 0xFF7802);
		} else {
			m_chatPartnerList.push_back(name);
			SendStringBuffer(GM_COMMAND_OPEN, m_arguments[1]);
			// "Added to the chat partner list."
			Display("\xb4\xeb\xc8\xad\xbb\xf3\xb4\xeb \xb8\xae\xbd\xba\xc6\xae\xbf\xa1 \xc3\xdf\xb0\xa1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", 0xC6FF85);
		}
	} else {
		result = GM_RESULT_BAD_ARGUMENT;
	}
	return result;
}

// gmtoolkit.cpp:991
eGMToolResult CGMToolkit::CommandClose(eGMToolkitStatus status) {
	eGMToolResult result = GM_RESULT_SUCCEEDED;
	if (m_argumentCount > 1) {
		std::string name(m_arguments[1]);

		std::vector<std::string>::iterator it;
		it = std::find(m_chatPartnerList.begin(), m_chatPartnerList.end(), name);

		if (it != m_chatPartnerList.end()) {
			m_chatPartnerList.erase(it);
			// "Removed from the chat partner list."
			Display("\xb4\xeb\xc8\xad\xbb\xf3\xb4\xeb \xb8\xae\xbd\xba\xc6\xae\xbf\xa1\xbc\xad \xbb\xe8\xc1\xa6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", 0xC6FF85);
			SendStringBuffer(GM_COMMAND_CLOSE, m_arguments[1]);
		} else {
			// "Not in the chat partner list."
			Display("\xb4\xeb\xc8\xad\xbb\xf3\xb4\xeb \xb8\xae\xbd\xba\xc6\xae\xbf\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0xFF7802);
		}
	} else {
		result = GM_RESULT_BAD_ARGUMENT;
	}
	return result;
}

// gmtoolkit.cpp:1022
eGMToolResult CGMToolkit::CommandKickUser(eGMToolkitStatus status) {
	eGMToolResult result = GM_RESULT_SUCCEEDED;
	if (m_argumentCount <= 1) {
		return GM_RESULT_BAD_ARGUMENT;
	}

	sBriefUserInfo info;
	std::map<unsigned long, sBriefUserInfo>::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_userMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_userMap.end(); ++it) {
		info = it->second;
		if (strcmp(info.nick, m_arguments[1]) == 0) {
			break;
		}
	}

	info = it->second;
	if (it != WSingleton<CSharedDoc>::Instance()->m_userMap.end()) {
		if (info.identityBits.administrator) {
			// "Administrators cannot be kicked."
			Display("\xbf\xee\xbf\xb5\xc0\xda\xb4\xc2 \xb0\xad\xc1\xa6\xc5\xf0\xc0\xe5 \xbd\xc3\xc5\xb3 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0xFF7802);
		} else {
			WSendPacket packet(GCT_GM_COMMAND);
			packet.Encode2(GM_COMMAND_KICK_USER);
			packet.Encode4(info.uid);
			if (m_argumentCount > 2) {
				if (strcmp(m_arguments[2], "-p") == 0) {
					packet.Encode1(1);
				} else {
					return GM_RESULT_BAD_ARGUMENT;
				}
			} else {
				packet.Encode1(0);
			}
			packet.Send(SEND_TO_DEFAULT);
		}
	} else {
		// "This user is not connected to the server or does not exist."
		Display("\xc7\xf6\xc0\xe7 \xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xcf\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xb0\xc5\xb3\xaa \xbe\xf8\xb4\xc2 \xc0\xaf\xc0\xfa\xc0\xd4\xb4\xcf\xb4\xd9.", 0xFF7802);
	}

	return result;
}

// gmtoolkit.cpp:1075
eGMToolResult CGMToolkit::CommandDisconnect(eGMToolkitStatus status) {
	if (m_argumentCount <= 1) {
		return GM_RESULT_BAD_ARGUMENT;
	}

	sBriefUserInfo info;
	std::map<unsigned long, sBriefUserInfo>::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_userMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_userMap.end(); ++it) {
		info = it->second;
		if (strcmp(info.nick, m_arguments[1]) == 0) {
			break;
		}
	}

	info = it->second;
	if (it != WSingleton<CSharedDoc>::Instance()->m_userMap.end()) {
		if (info.identityBits.administrator) {
			Display("\xbf\xee\xbf\xb5\xc0\xda\xb4\xc2 \xb5\xf0\xbd\xba\xc4\xbf\xb3\xd8\xc6\xae \xbd\xc3\xc5\xb3 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0xFF7802);
		} else {
			WSendPacket packet(GCT_GM_COMMAND);
			packet.Encode2(0xB);
			packet.Encode4(info.uid);
			packet.Send(SEND_TO_DEFAULT);

			WSendPacket disconnectPacket(GCT_REQUEST_KICK);
			disconnectPacket.Encode4(info.uid);
			disconnectPacket.Send(SEND_TO_DEFAULT);
		}
	} else {
		Display("\xc7\xf6\xc0\xe7 \xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xcf\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xb0\xc5\xb3\xaa \xbe\xf8\xb4\xc2 \xc0\xaf\xc0\xfa\xc0\xd4\xb4\xcf\xb4\xd9.", 0xFF7802);
	}

	return GM_RESULT_SUCCEEDED;
}

// gmtoolkit.cpp:1126
eGMToolResult CGMToolkit::CommandDiscon_UID(eGMToolkitStatus status) {
	eGMToolResult result = GM_RESULT_SUCCEEDED;

	if (m_argumentCount > 1) {
		std::map<unsigned long, sBriefUserInfo>::iterator it;
		if (!WSingleton<CSharedDoc>::Instance()->FindUserInfoUID(atoi(m_arguments[1]), it)) {
			return GM_RESULT_BAD_ARGUMENT;
		}

		sBriefUserInfo info = it->second;

		if (it != WSingleton<CSharedDoc>::Instance()->m_userMap.end()) {
			if (info.identityBits.administrator) {
				Display("\xbf\xee\xbf\xb5\xc0\xda\xb4\xc2 \xb5\xf0\xbd\xba\xc4\xbf\xb3\xd8\xc6\xae \xbd\xc3\xc5\xb3 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0xFF7802);
			} else {
				WSendPacket packet(GCT_GM_COMMAND);
				packet.Encode2(0xB);
				packet.Encode4(info.uid);
				packet.Send(SEND_TO_DEFAULT);

				WSendPacket disconnectPacket(GCT_REQUEST_KICK);
				disconnectPacket.Encode4(info.uid);
				disconnectPacket.Send(SEND_TO_DEFAULT);
			}
		} else {
			Display("\xc7\xf6\xc0\xe7 \xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xcf\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xb0\xc5\xb3\xaa \xbe\xf8\xb4\xc2 \xc0\xaf\xc0\xfa\xc0\xd4\xb4\xcf\xb4\xd9.", 0xFF7802);
		}
	} else {
		result = GM_RESULT_BAD_ARGUMENT;
	}

	return result;
}

// gmtoolkit.cpp:1176
eGMToolResult CGMToolkit::CommandDestroy(eGMToolkitStatus status) {
	eGMToolResult result = GM_RESULT_SUCCEEDED;
	if (m_argumentCount > 1) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomNumber == 0xFFFF) {
			int roomNumber = atoi(m_arguments[1]);
			WSendPacket destroyPacket(GCT_DESTROY_ROOM);
			destroyPacket.Encode2(roomNumber);
			destroyPacket.Send(SEND_TO_DEFAULT);

			WSendPacket packet(GCT_GM_COMMAND);
			packet.Encode2(GM_COMMAND_DESTROY);
			packet.Send(SEND_TO_DEFAULT);
		} else {
			// "Cannot be used outside the lobby."
			Display("\xb7\xce\xba\xf1 \xc0\xcc\xbf\xdc\xc0\xc7 \xc0\xe5\xbc\xd2\xbf\xa1\xbc\xad \xbb\xe7\xbf\xeb \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0xFF7802);
		}
	} else {
		result = GM_RESULT_BAD_ARGUMENT;
	}
	return result;
}

// gmtoolkit.cpp:1209
eGMToolResult CGMToolkit::CommandShowStatus(eGMToolkitStatus status) {
	if (m_statusFlags & GM_STATUS_VISIBLE) {
		Display("Visible : on", 0xC6FF85);
	} else {
		Display("Visible : off", 0xC6FF85);
	}

	if (m_statusFlags & GM_STATUS_WHISPER) {
		Display("Whisper : on", 0xC6FF85);
	} else {
		Display("Whisper : off", 0xC6FF85);
	}

	if (m_statusFlags & GM_STATUS_CHANNEL) {
		Display("Channel : on", 0xC6FF85);
	} else {
		Display("Channel : off", 0xC6FF85);
	}

	return GM_RESULT_SUCCEEDED;
}

// gmtoolkit.cpp:1240
eGMToolResult CGMToolkit::CommandWind(eGMToolkitStatus status) {
	eGMToolResult result = GM_RESULT_SUCCEEDED;

	if (m_argumentCount > 2) {
		unsigned char roomType = WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType;
		switch (roomType) {
		case 4:
		case 5:
		case 6:
		case 9:
		case 10:
		case 14:
			break;
		default:
			if (roomType != 2) {
				unsigned char speed = atoi(m_arguments[1]);
				unsigned char direction = atoi(m_arguments[2]);

				if (speed > 18) {
					Display("Wind speed value [0-18]", 0xFF7802);
					result = GM_RESULT_BAD_ARGUMENT;
				}

				if (direction > 360) {
					Display("Wind direction value [0-360]", 0xFF7802);
					result = GM_RESULT_BAD_ARGUMENT;
				} else if (result != GM_RESULT_BAD_ARGUMENT) {
					WSendPacket packet(GCT_GM_COMMAND);
					packet.Encode2(GM_COMMAND_WIND);
					packet.Encode1(speed - 1);
					packet.Encode1(direction);
					packet.Send(SEND_TO_DEFAULT);
				}
			} else {
				// "Cannot be used in a chat room."
				Display("\xb4\xeb\xc8\xad\xb9\xe6\xbf\xa1\xbc\xad\xb4\xc2 \xbb\xe7\xbf\xeb \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0xFF7802);
			}
			break;
		}
	} else {
		result = GM_RESULT_BAD_ARGUMENT;
	}

	return result;
}

// gmtoolkit.cpp:1290
eGMToolResult CGMToolkit::CommandWeather(eGMToolkitStatus status) {
	eGMToolResult result = GM_RESULT_SUCCEEDED;
	if (m_argumentCount > 1) {
		int weather = -1;
		if (strcmp(m_arguments[1], "fine") == 0) {
			weather = 0;
		} else if (strcmp(m_arguments[1], "cloud") == 0) {
			weather = 1;
		} else if (strcmp(m_arguments[1], "rain") == 0) {
			weather = 2;
		} else if (strcmp(m_arguments[1], "snow") == 0) {
			weather = 3;
		}

		if (weather != -1) {
			WSendPacket packet(GCT_GM_COMMAND);
			packet.Encode2(GM_COMMAND_WEATHER);
			packet.Encode1(weather);
			packet.Send(SEND_TO_DEFAULT);
		} else {
			result = GM_RESULT_BAD_ARGUMENT;
		}
	} else {
		result = GM_RESULT_BAD_ARGUMENT;
	}
	return result;
}

// gmtoolkit.cpp:1343
eGMToolResult CGMToolkit::CommandIdentity(eGMToolkitStatus status) {
	eGMToolResult result = GM_RESULT_SUCCEEDED;
	if (m_argumentCount > 1) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomNumber == 0xFFFF) {
			unsigned long identity = 0;
			if (strcmp(m_arguments[1], "admin") == 0) {
				identity = 0x484;
				// "Setting : Administrator"
				Display("\xbc\xb3\xc1\xa4 : Administrator", 0xC6FF85);
			} else if (strcmp(m_arguments[1], "user") == 0) {
				identity = 0x80;
				// "Setting : Normal user"
				Display("\xbc\xb3\xc1\xa4 : Normal user", 0xC6FF85);
				m_isGM = true;
			}

			if (identity != 0) {
				if (!AdminMode()) {
					if (identity != 0x80) {
						WSendPacket packet(GCT_REQUEST_IDENTITY);
						packet.Encode4(identity);
						packet.EncodeStr(std::string(WSingleton<CSharedDoc>::Instance()->m_userInfo.nickname));
						packet.Send(SEND_TO_DEFAULT);
					}
				} else {
					if (identity != 0x484) {
						WSendPacket packet(GCT_REQUEST_IDENTITY);
						packet.Encode4(identity);
						packet.EncodeStr(std::string(WSingleton<CSharedDoc>::Instance()->m_userInfo.nickname));
						packet.Send(SEND_TO_DEFAULT);
					}
				}
			} else {
				result = GM_RESULT_BAD_ARGUMENT;
			}
		} else {
			// "Identity changes are only possible in the lobby."
			Display("\xb1\xc7\xc7\xd1 \xba\xaf\xb0\xe6\xc0\xba \xb7\xce\xba\xf1\xbf\xa1\xbc\xad\xb8\xb8 \xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.", 0xFF7802);
		}
	} else if (m_argumentCount == 1) {
		if (AdminMode()) {
			Display("You are the Administrator now! (~+_+)~", 0xC6FF85);
		} else {
			Display("You are a normal user T_T", 0xC6FF85);
		}
	} else {
		result = GM_RESULT_BAD_ARGUMENT;
	}
	return result;
}

// gmtoolkit.cpp:1447
eGMToolResult CGMToolkit::CommandNotice(eGMToolkitStatus status) {
	if (m_argumentCount > 1) {
		std::string notice("");

		for (int i = 1; i < m_argumentCount; i++) {
			notice = notice + m_arguments[i];
			notice = notice + " ";
		}

		WSendPacket packet(GCT_NOTICE);
		packet.EncodeStr(notice);
		packet.Send(SEND_TO_DEFAULT);
	} else {
		return GM_RESULT_BAD_ARGUMENT;
	}

	return GM_RESULT_SUCCEEDED;
}

// gmtoolkit.cpp:1475
eGMToolResult CGMToolkit::CommandGiveItem(eGMToolkitStatus status) {
	eGMToolResult result = GM_RESULT_SUCCEEDED;

	if (status == GM_TOOLKIT_NORMAL) {
		if (m_argumentCount > 1) {
			char itemName[0x40] = {0};

			std::map<unsigned long, sBriefUserInfo>::iterator it;
			for (it = WSingleton<CSharedDoc>::Instance()->m_userMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_userMap.end(); ++it) {
				sBriefUserInfo info = it->second;
				if (strcmp(info.nick, m_arguments[1]) == 0) {
					break;
				}
			}

			sBriefUserInfo user = it->second;
			if (it != WSingleton<CSharedDoc>::Instance()->m_userMap.end()) {
				unsigned long count = 0;
				if (m_argumentCount == 3) {
					count = 1;
				} else if (m_argumentCount > 3) {
					count = atoi(m_arguments[3]);
				}

				if (count > 0) {
					unsigned long typeId = atoi(m_arguments[2]);

					if (!FindItem(typeId, itemName)) {
						// "Invalid item TypeID."
						Display("\xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb TypeID \xc0\xd4\xb4\xcf\xb4\xd9.", 0xFF7802);
					} else if (m_confirmPacket == 0) {
						m_confirmPacket = new WSendPacket(GCT_GM_COMMAND);
						m_confirmPacket->Encode2(GM_COMMAND_GIVE_ITEM);
						m_confirmPacket->Encode4(user.uid);
						m_confirmPacket->Encode4(typeId);
						m_confirmPacket->Encode4(count);

						char text[0x80] = {0};
						// "TypeID : %d, item : %s, count : %d"
						sprintf(text, "\xc5\xb8\xc0\xd4ID : %d, \xbe\xc6\xc0\xcc\xc5\xdb : %s, \xbc\xf6\xb7\xae : %d", typeId, itemName, count);
						Display(text, 0xC6FF85);
						memset(text, 0, sizeof(text));
						// "Give the item above to '%s'? [/Y, /N]"
						sprintf(text, "'%s' \xb4\xd4\xbf\xa1\xb0\xd4 \xc0\xa7\xc0\xc7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xc1\xf6\xb1\xde \xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee? [/Y, /N]", m_arguments[1]);
						Display(text, 0xC6FF85);

						SetConfirmWaitTag(GM_COMMAND_GIVE_ITEM);
					} else {
						result = GM_RESULT_SYNTAX_ERROR;
					}
				} else {
					result = GM_RESULT_BAD_ARGUMENT;
				}
			} else {
				// "This user is not connected to the server or does not exist."
				Display("\xc7\xf6\xc0\xe7 \xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xcf\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xb0\xc5\xb3\xaa \xbe\xf8\xb4\xc2 \xc0\xaf\xc0\xfa\xc0\xd4\xb4\xcf\xb4\xd9.", 0xFF7802);
			}
		} else {
			result = GM_RESULT_BAD_ARGUMENT;
		}
	} else {
		if (Confirm()) {
			// "The item was given."
			Display("\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xc1\xf6\xb1\xde\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", 0xC6FF85);
		} else {
			// "The 'giveitem' command was cancelled."
			Display("'giveitem' \xb8\xed\xb7\xc9\xc0\xcc \xc3\xeb\xbc\xd2 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", 0xC6FF85);
		}

		SetToolKitStatus(GM_TOOLKIT_NORMAL);
	}

	return result;
}

// gmtoolkit.cpp:1593
eGMToolResult CGMToolkit::CommandGoldenBell(eGMToolkitStatus status) {
	eGMToolResult result = GM_RESULT_SUCCEEDED;

	if (WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_roomNumber != 0xFFFF) {
		if (status == GM_TOOLKIT_NORMAL) {
			if (m_argumentCount > 1) {
				char itemName[64] = { 0 };
				unsigned long count = 0;

				if (m_argumentCount == 2) {
					count = 1;
				} else if (m_argumentCount > 2) {
					count = atoi(m_arguments[2]);
				}

				if (count > 0) {
					unsigned long typeId = atoi(m_arguments[1]);

					if (!FindItem(typeId, itemName)) {
						// "Invalid item type ID."
						Display("\xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb \xc5\xb8\xc0\xd4ID \xc0\xd4\xb4\xcf\xb4\xd9.", 0xFF7802);
					} else if (m_confirmPacket == 0) {
						m_confirmPacket = new WSendPacket(GCT_GM_COMMAND);
						m_confirmPacket->Encode2(GM_COMMAND_GOLDEN_BELL);
						m_confirmPacket->Encode4(typeId);
						m_confirmPacket->Encode4(count);

						char text[128] = { 0 };
						// "Type ID : %d, item : %s, quantity : %d"
						sprintf(text, "\xc5\xb8\xc0\xd4ID : %d, \xbe\xc6\xc0\xcc\xc5\xdb : %s, \xbc\xf6\xb7\xae : %d", typeId, itemName, count);
						Display(text, 0xC6FF85);
						memset(text, 0, sizeof(text));
						// "Give the item above to the users in the current room? [/Y, /N]"
						sprintf(text, "\xc7\xf6\xc0\xe7 \xb7\xeb\xbe\xc8\xc0\xc7 \xc0\xaf\xc0\xfa\xbf\xa1\xb0\xd4 \xc0\xa7\xc0\xc7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xc1\xf6\xb1\xde \xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?. [/Y, /N]");
						Display(text, 0xC6FF85);

						SetConfirmWaitTag(GM_COMMAND_GOLDEN_BELL);
					} else {
						result = GM_RESULT_SYNTAX_ERROR;
					}
				}
			} else {
				result = GM_RESULT_BAD_ARGUMENT;
			}
		} else {
			if (Confirm()) {
				// "The item was given."
				Display("\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xc1\xf6\xb1\xde\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", 0xC6FF85);
			} else {
				// "The 'goldenbell' command was cancelled."
				Display("'goldenbell' \xb8\xed\xb7\xc9\xc0\xcc \xc3\xeb\xbc\xd2 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", 0xC6FF85);
			}

			SetToolKitStatus(GM_TOOLKIT_NORMAL);
		}
	} else {
		// "Not available in the lobby."
		Display("\xb7\xce\xba\xf1\xbf\xa1\xbc\xad\xb4\xc2 \xbb\xe7\xbf\xeb \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0xFF7802);
	}

	return result;
}

// gmtoolkit.cpp:1691
eGMToolResult CGMToolkit::CommandSetPrize(eGMToolkitStatus status) {
	eGMToolResult result = GM_RESULT_SUCCEEDED;

	if (status == GM_TOOLKIT_NORMAL) {
		if (m_argumentCount == 2) {
			if (strcmp("on", m_arguments[1]) == 0) {
				m_prizeSwitch = true;
				// "SetPrize enabled."
				Display("SetPrize \xb1\xe2\xb4\xc9 \xc8\xb0\xbc\xba\xc8\xad.", 0xC6FF85);
			} else if (strcmp("off", m_arguments[1]) == 0) {
				m_prizeSwitch = false;
				// "SetPrize disabled."
				Display("SetPrize \xb1\xe2\xb4\xc9 \xba\xf1\xc8\xb0\xbc\xba\xc8\xad.", 0xC6FF85);
			} else {
				result = GM_RESULT_BAD_ARGUMENT;
			}
		} else if (m_argumentCount >= 3) {
			sPrize *prize = new sPrize;

			char itemName[0x40] = {0};

			// The award names of the ceremony give prize types 1-6; otherwise
			// the first argument is a rank, optionally followed by "to" and a
			// last rank.
			int itemIndex = 0;
			bool valid = true;
			if (strcmp(m_arguments[1], "speeder") == 0) {
				prize->m_type = 1;
				itemIndex = 2;
			} else if (strcmp(m_arguments[1], "runner") == 0) {
				prize->m_type = 2;
				itemIndex = 2;
			} else if (strcmp(m_arguments[1], "chipin") == 0) {
				prize->m_type = 3;
				itemIndex = 2;
			} else if (strcmp(m_arguments[1], "longputt") == 0) {
				prize->m_type = 4;
				itemIndex = 2;
			} else if (strcmp(m_arguments[1], "recovery") == 0) {
				prize->m_type = 5;
				itemIndex = 2;
			} else if (strcmp(m_arguments[1], "lucky") == 0) {
				prize->m_type = 6;
				itemIndex = 2;
			} else {
				char number[0x10] = {0};
				int firstRank = atoi(m_arguments[1]);

				memset(number, 0, sizeof(number));
				itoa(firstRank, number, 10);

				if (strlen(number) == strlen(m_arguments[1]) && firstRank > 0 && firstRank < 301) {
					prize->m_firstRank = firstRank;

					if (strcmp(m_arguments[2], "to") == 0) {
						int lastRank = atoi(m_arguments[3]);

						memset(number, 0, sizeof(number));
						itoa(lastRank, number, 10);

						if (strlen(number) == strlen(m_arguments[3]) && m_argumentCount >= 5 && lastRank > 0 && lastRank < 301) {
							prize->m_lastRank = lastRank;
							itemIndex = 4;
							prize->m_type = 0;
						} else {
							result = GM_RESULT_BAD_ARGUMENT;
							valid = false;
						}
					} else {
						prize->m_lastRank = firstRank;
						prize->m_type = 0;
						itemIndex = 2;
					}
				} else {
					result = GM_RESULT_BAD_ARGUMENT;
					valid = false;
				}
			}

			if (itemIndex > m_argumentCount || !valid) {
				result = GM_RESULT_BAD_ARGUMENT;
			} else {
				unsigned long typeId = atoi(m_arguments[itemIndex]);

				if (!FindItem(typeId, itemName)) {
					result = GM_RESULT_BAD_ARGUMENT;
				} else {
					prize->m_itemTypeId = typeId;

					if (itemIndex + 1 < m_argumentCount) {
						int count = atoi(m_arguments[itemIndex + 1]);

						if (count > 0) {
							prize->m_itemCount = count;
							SetConfirmWaitTag(GM_COMMAND_SET_PRIZE);
						} else {
							result = GM_RESULT_BAD_ARGUMENT;
						}
					} else {
						prize->m_itemCount = 1;
						SetConfirmWaitTag(GM_COMMAND_SET_PRIZE);
					}

					if (GetToolKitStatus() == GM_TOOLKIT_CONFIRM_WAIT) {
						char text[0x80];

						if (prize->m_type == 0) {
							if (prize->m_lastRank == prize->m_firstRank) {
								memset(text, 0, sizeof(text));
								// "range : rank %d"
								sprintf(text, "\xb9\xfc   \xc0\xa7 : %d\xb5\xee", prize->m_firstRank);
							} else {
								memset(text, 0, sizeof(text));
								// "range : rank %d to rank %d "
								sprintf(text, "\xb9\xfc   \xc0\xa7 : %d\xb5\xee \xba\xce\xc5\xcd %d\xb5\xee \xb1\xee\xc1\xf6 ", prize->m_firstRank, prize->m_lastRank);
							}
							Display(text, 0xC6FF85);

							memset(text, 0, sizeof(text));
							// "item : %s"
							sprintf(text, "\xbe\xc6\xc0\xcc\xc5\xdb : %s", itemName);
							Display(text, 0xC6FF85);

							memset(text, 0, sizeof(text));
							// "count : %d"
							sprintf(text, "\xbc\xf6   \xb7\xae : %d\xb0\xb3", prize->m_itemCount);
							Display(text, 0xC6FF85);
						} else {
							char awardName[0x20] = {0};
							// The copies go through a pointer: the original copies with a byte loop.
							const char *name;

							switch (prize->m_type) {
							case 1:
								name = "\xba\xa3\xbd\xba\xc6\xae \xbd\xba\xc7\xc7\xb4\xf5";
								strcpy(awardName, name);
								break;

							case 2:
								name = "\xba\xa3\xbd\xba\xc6\xae \xb7\xaf\xb3\xca";
								strcpy(awardName, name);
								break;

							case 3:
								name = "\xba\xa3\xbd\xba\xc6\xae \xb8\xae\xc4\xbf\xb9\xf6\xb8\xae";
								strcpy(awardName, name);
								break;

							case 4:
								name = "\xc7\xe0\xbf\xee\xbb\xf3";
								strcpy(awardName, name);
								break;

							case 5:
								name = "\xba\xa3\xbd\xba\xc6\xae \xb7\xd5\xc6\xdb\xc5\xcd";
								strcpy(awardName, name);
								break;

							case 6:
								name = "\xba\xa3\xbd\xba\xc6\xae \xc4\xa8\xc0\xce";
								strcpy(awardName, name);
								break;
							}

							memset(text, 0, sizeof(text));
							// "range : %s"
							sprintf(text, "\xb9\xfc   \xc0\xa7 : %s", awardName);
							Display(text, 0xC6FF85);

							memset(text, 0, sizeof(text));
							sprintf(text, "\xbe\xc6\xc0\xcc\xc5\xdb : %s", itemName);
							Display(text, 0xC6FF85);

							memset(text, 0, sizeof(text));
							sprintf(text, "\xbc\xf6   \xb7\xae : %d\xb0\xb3", prize->m_itemCount);
							Display(text, 0xC6FF85);
						}

						// "Set it? [/Y, /N]"
						sprintf(text, "\xbc\xb3\xc1\xa4\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?. [/Y, /N]");
						Display(text, 0xC6FF85);

						m_pendingPrize = prize;
					}
				}
			}
		} else {
			result = GM_RESULT_BAD_ARGUMENT;
		}
	} else {
		if (Confirm()) {
			if (m_pendingPrize != 0) {
				m_prizeList.push_back(m_pendingPrize);
				m_pendingPrize = 0;
			}

			// "Prize list set."
			Display("\xbd\xc3\xbb\xf3\xc7\xb0\xb8\xf1 \xb8\xae\xbd\xba\xc6\xae \xbc\xb3\xc1\xa4\xbf\xcf\xb7\xe1.", 0xC6FF85);
		} else {
			if (m_pendingPrize != 0) {
				delete m_pendingPrize;
				m_pendingPrize = 0;
			}

			// "The 'setprize' command was cancelled."
			Display("'setprize' \xb8\xed\xb7\xc9\xc0\xcc \xc3\xeb\xbc\xd2 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", 0xC6FF85);
		}

		SetToolKitStatus(GM_TOOLKIT_NORMAL);
	}

	return result;
}

// gmtoolkit.cpp:1967
eGMToolResult CGMToolkit::CommandUnsetPrize(eGMToolkitStatus status) {
	if (status == GM_TOOLKIT_NORMAL) {
		if (m_argumentCount >= 1) {
			// "Delete the whole prize list? [/Y, /N]"
			Display("\xbd\xc3\xbb\xf3\xc7\xb0\xb8\xf1 \xb8\xae\xbd\xba\xc6\xae\xb8\xa6 \xc0\xfc\xba\xce \xbb\xe8\xc1\xa6\xc7\xcf\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?. [/Y, /N]", 0xC6FF85);
			SetConfirmWaitTag(GM_COMMAND_UNSET_PRIZE);
		} else {
			return GM_RESULT_SYNTAX_ERROR;
		}
	} else {
		if (Confirm()) {
			for (std::list<sPrize *>::iterator it = m_prizeList.begin(); it != m_prizeList.end(); ++it) {
				if (*it != 0) {
					delete *it;
					*it = 0;
				}
			}

			m_prizeList.clear();

			// "Prize list deleted."
			Display("\xbd\xc3\xbb\xf3\xc7\xb0\xb8\xf1 \xb8\xae\xbd\xba\xc6\xae \xbb\xe8\xc1\xa6\xbf\xcf\xb7\xe1.", 0xC6FF85);
			m_prizeSwitch = false;
		} else {
			// "The 'unsetprize' command was cancelled."
			Display("'unsetprize' \xb8\xed\xb7\xc9\xc0\xcc \xc3\xeb\xbc\xd2 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", 0xC6FF85);
		}

		SetToolKitStatus(GM_TOOLKIT_NORMAL);
	}

	return GM_RESULT_SUCCEEDED;
}

// gmtoolkit.cpp:2017
eGMToolResult CGMToolkit::CommandShowPrize(eGMToolkitStatus status) {
	char itemName[0x40] = {0};
	char text[0x80] = {0};

	if (m_prizeSwitch) {
		// "SetPrize enabled"
		Display("SetPrize \xb1\xe2\xb4\xc9 \xc8\xb0\xbc\xba\xc8\xad", 0xC6FF85);
	} else {
		// "SetPrize disabled"
		Display("SetPrize \xb1\xe2\xb4\xc9 \xba\xf1\xc8\xb0\xbc\xba\xc8\xad", 0xC6FF85);
	}

	int index = 0;
	for (std::list<sPrize *>::iterator it = m_prizeList.begin(); it != m_prizeList.end(); ++it) {
		sPrize *prize = *it;

		if (prize == 0) {
			continue;
		}

		if (FindItem(prize->m_itemTypeId, itemName)) {
			index++;

			memset(text, 0, sizeof(text));

			if (prize->m_type == 0) {
				if (prize->m_firstRank == prize->m_lastRank) {
					// "[%02d] range : rank %d, item : %s, count : %d"
					sprintf(text, "[%02d] \xb9\xfc\xc0\xa7 : %d\xb5\xee, \xbe\xc6\xc0\xcc\xc5\xdb : %s, \xbc\xf6\xb7\xae : %d",
						index, prize->m_firstRank, itemName, prize->m_itemCount);
				} else {
					// "[%02d] range : rank %d to rank %d, item : %s, count : %d"
					sprintf(text, "[%02d] \xb9\xfc\xc0\xa7 : %d\xb5\xee \xba\xce\xc5\xcd %d\xb5\xee \xb1\xee\xc1\xf6, \xbe\xc6\xc0\xcc\xc5\xdb : %s, \xbc\xf6\xb7\xae : %d",
						index, prize->m_firstRank, prize->m_lastRank, itemName, prize->m_itemCount);
				}
			} else {
				char awardName[0x20] = {0};
				// The copies go through a pointer: the original copies with a byte loop.
				const char *name;

				// The awards of the ceremony (SendPrize), in Korean.
				switch (prize->m_type) {
				case 1:
					name = "\xba\xa3\xbd\xba\xc6\xae \xbd\xba\xc7\xc7\xb4\xf5";
					strcpy(awardName, name);
					break;

				case 2:
					name = "\xba\xa3\xbd\xba\xc6\xae \xb7\xaf\xb3\xca";
					strcpy(awardName, name);
					break;

				case 3:
					name = "\xba\xa3\xbd\xba\xc6\xae \xb8\xae\xc4\xbf\xb9\xf6\xb8\xae";
					strcpy(awardName, name);
					break;

				case 4:
					name = "\xc7\xe0\xbf\xee\xbb\xf3";
					strcpy(awardName, name);
					break;

				case 5:
					name = "\xba\xa3\xbd\xba\xc6\xae \xb7\xd5\xc6\xdb\xc5\xcd";
					strcpy(awardName, name);
					break;

				case 6:
					name = "\xba\xa3\xbd\xba\xc6\xae \xc4\xa8\xc0\xce";
					strcpy(awardName, name);
					break;
				}

				// "[%02d] range : %s, item : %s, count : %d"
				sprintf(text, "[%02d] \xb9\xfc\xc0\xa7 : %s, \xbe\xc6\xc0\xcc\xc5\xdb : %s, \xbc\xf6\xb7\xae : %d",
					index, awardName, itemName, prize->m_itemCount);
			}

			Display(text, 0xC6FF85);
		} else {
			// "Invalid condition."
			Display("\xc0\xdf\xb8\xf8\xb5\xc8 \xc1\xb6\xb0\xc7\xb9\xae.", 0xFF7802);
		}
	}

	return GM_RESULT_SUCCEEDED;
}

// gmtoolkit.cpp:2110
eGMToolResult CGMToolkit::CommandLoadScript(eGMToolkitStatus status) {
	return GM_RESULT_SUCCEEDED;
}

// gmtoolkit.cpp:2119
eGMToolResult CGMToolkit::CommandNoticePrize(eGMToolkitStatus status) {
	int count = m_prizeList.size();

	if (count != 0) {
		WSendPacket packet(GCT_GM_COMMAND);
		packet.Encode2(GM_COMMAND_NOTICE_PRIZE);
		packet.Encode4(count);

		for (std::list<sPrize *>::iterator it = m_prizeList.begin(); it != m_prizeList.end(); ++it) {
			sPrize *prize = *it;

			if (prize != 0) {
				packet.EncodeBuffer(prize, sizeof(sPrize));
			}
		}

		packet.Send(SEND_TO_DEFAULT);
	} else {
		// "No registered conditions."
		Display("\xb5\xee\xb7\xcf\xb5\xc8 \xc1\xb6\xb0\xc7\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0xC6FF85);
	}

	return GM_RESULT_SUCCEEDED;
}

// gmtoolkit.cpp:2157
eGMToolResult CGMToolkit::CommandGetTID(eGMToolkitStatus status) {
	eGMToolResult result = GM_RESULT_SUCCEEDED;
	if (m_argumentCount > 1) {
		Display("Succeeded command.", 0xC6FF85);
	} else {
		result = GM_RESULT_BAD_ARGUMENT;
	}
	return result;
}

// gmtoolkit.cpp:2177
eGMToolResult CGMToolkit::CommandSetMission(eGMToolkitStatus status) {
	eGMToolResult result = GM_RESULT_SUCCEEDED;
	if (m_argumentCount > 1) {
		int mission = atoi(m_arguments[1]);

		WSendPacket packet(GCT_GM_COMMAND);
		packet.Encode2(GM_COMMAND_SET_MISSION);
		packet.Encode4(mission);
		packet.Send(SEND_TO_DEFAULT);

		Display("Succeeded command.", 0xC6FF85);
	} else {
		result = GM_RESULT_BAD_ARGUMENT;
	}
	return result;
}

// gmtoolkit.cpp:2232
eGMToolResult CGMToolkit::CommandFindItem(eGMToolkitStatus status) {
	if (m_argumentCount > 2) {
		bool found;

		// The categories CommandCategory lists, in Korean: item, card, part,
		// aux part, club, aztec (ball), set, furniture, all.
		if (strcmp(m_arguments[1], "\xbe\xc6\xc0\xcc\xc5\xdb") == 0) {
			found = FindItem<IFF_STRUCT::sItem>(m_arguments[2], ItemManager()->m_itemMap);
		} else if (strcmp(m_arguments[1], "\xc4\xab\xb5\xe5") == 0) {
			found = FindItem<IFF_STRUCT::sCard>(m_arguments[2], ItemManager()->m_cardMap);
		} else if (strcmp(m_arguments[1], "\xc6\xc4\xc3\xf7") == 0) {
			found = FindItem<IFF_STRUCT::sPart>(m_arguments[2], ItemManager()->m_partMap);
		} else if (strcmp(m_arguments[1], "\xba\xb8\xc1\xb6\xc6\xc4\xc3\xf7") == 0) {
			found = FindItem<IFF_STRUCT::sAuxPart>(m_arguments[2], ItemManager()->m_auxPartMap);
		} else if (strcmp(m_arguments[1], "\xc5\xac\xb7\xb4") == 0) {
			found = FindItem<IFF_STRUCT::sClubSet>(m_arguments[2], ItemManager()->m_clubSetMap);
		} else if (strcmp(m_arguments[1], "\xbe\xc6\xc1\xee\xc5\xd8") == 0) {
			found = FindItem<IFF_STRUCT::sBall>(m_arguments[2], ItemManager()->m_ballMap);
		} else if (strcmp(m_arguments[1], "\xbc\xbc\xc6\xae") == 0) {
			found = FindItem<IFF_STRUCT::sSetItem>(m_arguments[2], ItemManager()->m_setItemMap);
		} else if (strcmp(m_arguments[1], "\xb0\xa1\xb1\xb8") == 0) {
			found = FindItem<IFF_STRUCT::sFurniture>(m_arguments[2], ItemManager()->m_furnitureMap);
		} else if (strcmp(m_arguments[1], "\xb8\xf0\xb5\xce") == 0) {
			found = FindItem<IFF_STRUCT::sItem>(m_arguments[2], ItemManager()->m_itemMap);
			found |= FindItem<IFF_STRUCT::sCard>(m_arguments[2], ItemManager()->m_cardMap);
			found |= FindItem<IFF_STRUCT::sPart>(m_arguments[2], ItemManager()->m_partMap);
			found |= FindItem<IFF_STRUCT::sAuxPart>(m_arguments[2], ItemManager()->m_auxPartMap);
			found |= FindItem<IFF_STRUCT::sClubSet>(m_arguments[2], ItemManager()->m_clubSetMap);
			found |= FindItem<IFF_STRUCT::sBall>(m_arguments[2], ItemManager()->m_ballMap);
			found |= FindItem<IFF_STRUCT::sSetItem>(m_arguments[2], ItemManager()->m_setItemMap);
			found |= FindItem<IFF_STRUCT::sFurniture>(m_arguments[2], ItemManager()->m_furnitureMap);
		} else {
			return GM_RESULT_BAD_ARGUMENT;
		}

		if (!found) {
			// "Not found."
			Display("\xc3\xa3\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0xFF7802);
		}
	} else {
		return GM_RESULT_BAD_ARGUMENT;
	}

	return GM_RESULT_SUCCEEDED;
}

// gmtoolkit.cpp:2314
eGMToolResult CGMToolkit::CommandCategory(eGMToolkitStatus status) {
	eGMToolResult result = GM_RESULT_SUCCEEDED;
	if (m_argumentCount > 0) {
		// The item categories, in Korean.
		Display("\xbe\xc6\xc0\xcc\xc5\xdb", 0xC6FF85);
		Display("\xc4\xab\xb5\xe5", 0xC6FF85);
		Display("\xc6\xc4\xc3\xf7", 0xC6FF85);
		Display("\xba\xb8\xc1\xb6\xc6\xc4\xc3\xf7", 0xC6FF85);
		Display("\xc5\xac\xb7\xb4", 0xC6FF85);
		Display("\xbe\xc6\xc1\xee\xc5\xd8", 0xC6FF85);
		Display("\xbc\xbc\xc6\xae", 0xC6FF85);
		Display("\xb0\xa1\xb1\xb8", 0xC6FF85);
		Display("\xb8\xf0\xb5\xce", 0xC6FF85);
	} else {
		result = GM_RESULT_BAD_ARGUMENT;
	}
	return result;
}

// gmtoolkit.cpp:2338
eGMToolResult CGMToolkit::CommandMatchMapSetting(eGMToolkitStatus status) {
	if (m_argumentCount != 2) {
		return GM_RESULT_BAD_ARGUMENT;
	}

	int map = atoi(m_arguments[1]);
	if (map < 0 || map > 20) {
		return GM_RESULT_BAD_ARGUMENT;
	}

	WSendPacket packet(GCT_GM_COMMAND);
	packet.Encode2(GM_COMMAND_MATCH_MAP_SETTING);
	packet.Encode4(map);
	packet.Send(SEND_TO_DEFAULT);

	return GM_RESULT_SUCCEEDED;
}

// gmtoolkit.cpp:2360
eGMToolResult CGMToolkit::CommandMatchHoleSetting(eGMToolkitStatus status) {
	if (m_argumentCount != 2) {
		return GM_RESULT_BAD_ARGUMENT;
	}

	int holeCount = atoi(m_arguments[1]);
	switch (holeCount) {
	case 3:
	case 6:
	case 9:
	case 18: {
		WSendPacket packet(GCT_GM_COMMAND);
		packet.Encode2(GM_COMMAND_MATCH_HOLE_SETTING);
		packet.Encode4(holeCount);
		packet.Send(SEND_TO_DEFAULT);
		break;
	}
	default:
		return GM_RESULT_BAD_ARGUMENT;
	}

	return GM_RESULT_SUCCEEDED;
}
