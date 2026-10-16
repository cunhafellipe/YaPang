// Original: d:\build\custom\temp\code\client\projectg\hatmanager.cpp
#define PG_HATMANAGER_FILE_STRINGS
#include <atlbase.h>	// stand-in for the precompiled header: _AtlWinModule initializer

#include "hatManager.h"
#include "guilddefine.h"
#include "ChatMsg.h"
#include "TaskManager.h"
#include "actor.h"
#include "SharedDoc.h"
#include "commonutil.h"
#include "../Wangreal/source/XUnzip.h"
#include "GolfDoc.h"
#include "NetworkSystem.h"
#include "Packet.h"
#include "avatar_task.h"
#include "messengerdlg.h"
#include "shared/Localize.h"
#include "client/Fresh/include/frwnd.h"
#include "GMToolkit.h"
#include "client/Fresh/include/fresh.h"
#include "client/Fresh/include/frwndmanager.h"
#include "client/Fresh/include/fremoticon.h"

#include <io.h>

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <string>
#include <algorithm>

// <direct.h> (missing from externs/vs-2003, declared as westpak.cpp does)
extern "C" __declspec(dllimport) int __cdecl mkdir(const char *dirname);

extern unsigned long g_CurrentTime;

const char* GetCharsTillSpace(const char* pSrc, char* pDest); // commonutil.cpp

// The emoticon words, three faces of 12 words each.
extern const char *FacialTextTable[][12];

// A chat motion: its name and up to seven "/command" words.
struct sMotionText {
	const char *m_name;          // 0x00
	const char *m_command[7];    // 0x04
};
CHECK_SIZE(sMotionText, 0x20);

// The motion commands, 15 per motion type (no symbol).
extern sMotionText data_00a9e9d8[][15];

// A chat command word (CheckChatCommand).
struct sChatCmd {
	char m_name[20];    // 0x00
};
CHECK_SIZE(sChatCmd, 0x14);

// The chat commands: "identity", "notice", "itemdrop".
sChatCmd ChatCmd[3] = {
	{ "identity" },
	{ "notice" },
	{ "itemdrop" },
};

// The level names, 20 bytes each (no symbol).
extern char data_00a12bc8[][20];

// The Japanese filter's mark for a filtered character: through a const
// variable so WStrStr_JP's static is dynamically initialized (guard bit),
// as in the original.
static const char* const HEART_MARK = "\xA2\xBE";

// The filler characters a filtered word may hide behind.
unsigned short FakeTable[28] = {
	' ', '`', '~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '-',
	'_', '=', '+', '|', ';', ':', '\'', '"', ',', '<', '.', '>', '/', '?'
};

namespace BABO_STR {

// hatmanager.cpp:397
unsigned short TryToLower(unsigned short ch) {
	return (iswascii(ch) && iswupper(ch)) ? towlower(ch) : ch;
}

// hatmanager.cpp:403
// Finds word in text (case-insensitive; FakeTable characters inside the text
// are skipped) and returns whether it was found. Unless bFindOnly, every
// matched character that is not a FakeTable one becomes U+2665 and the text
// is converted back into out.
bool WStrStr(char *out, unsigned short *text, int textLen, unsigned short *word, int wordLen, bool bFindOnly) {
	bool found = false;

	for (int i = 0; i < textLen - 1; i++) {
		if (TryToLower(text[i]) == TryToLower(word[0])) {
			unsigned short *textPos = &text[i + 1];
			unsigned short *wordPos = &word[1];

			while (*textPos != 0 && *wordPos != 0) {
				if (TryToLower(*textPos) != TryToLower(*wordPos)) {
					int k;
					for (k = 0; k < 28; k++) {
						if (*textPos == FakeTable[k])
							break;
					}

					if (k == 28)
						break;
				} else {
					wordPos++;
				}
				textPos++;
			}

			if (wordPos - word == wordLen - 1) {
				if (bFindOnly)
					return true;

				found = true;
				int matchLen = textPos - &text[i];
				for (int j = 0; j < matchLen; j++) {
					int k;
					for (k = 0; k < 28; k++) {
						if (text[i + j] == FakeTable[k])
							break;
					}

					if (k == 28)
						text[i + j] = 0x2665;
				}
			}
		}
	}

	if (found)
		WideCharToMultiByte(0, 0, text, -1, out, 0x400, 0, 0);

	return found;
}

// hatmanager.cpp:499
// Returns true when the two words differ (case-insensitive, without their
// last character).
bool WStrCmp(unsigned short *word, int wordLen, unsigned short *text, int textLen) {
	if (wordLen != textLen)
		return true;

	for (int i = 0; i < wordLen - 1; i++) {
		if (tolower(word[i]) != tolower(text[i]))
			return true;
	}

	return false;
}

// hatmanager.cpp:513
// WStrStr for Japanese: only ASCII characters compare case-insensitively,
// and matched characters become the first character of "\xA2\xBE" (U+2665
// when it cannot be converted).
bool WStrStr_JP(char *out, unsigned short *text, int textLen, unsigned short *word, int wordLen, bool bFindOnly) {
	bool found = false;

	for (int i = 0; i < textLen - 1; i++) {
		if (text[i] < 0x80 && word[0] < 0x80) {
			if (tolower(text[i]) != tolower(word[0]))
				continue;
		} else {
			if (text[i] != word[0])
				continue;
		}

		unsigned short *textPos = &text[i + 1];
		unsigned short *wordPos = &word[1];

		while (*textPos != 0 && *wordPos != 0) {
			bool differ;
			if (*textPos < 0x80 && *wordPos < 0x80)
				differ = tolower(*wordPos) != tolower(*textPos);
			else
				differ = *textPos != *wordPos;
			if (differ) {
				int k;
				for (k = 0; k < 28; k++) {
					if (*textPos == FakeTable[k])
						break;
				}

				if (k == 28)
					break;
			} else {
				wordPos++;
			}
			textPos++;
		}

		if (wordPos - word != wordLen - 1)
			continue;

		if (bFindOnly)
			return true;

		found = true;
		static const char *mark = HEART_MARK;

		unsigned short wideMark[32];
		int converted = MultiByteToWideChar(0, 0, mark, strlen(mark) + 1, wideMark, 32);

		int matchLen = textPos - &text[i];
		if (mark != 0) {
			if (converted != 0) {
				for (int j = 0; j < matchLen; j++) {
					int k;
					for (k = 0; k < 28; k++) {
						if (text[i + j] == FakeTable[k])
							break;
					}

					if (k == 28)
						text[i + j] = wideMark[0];
				}
			} else {
				for (int j = 0; j < matchLen; j++) {
					int k;
					for (k = 0; k < 28; k++) {
						if (text[i + j] == FakeTable[k])
							break;
					}

					if (k == 28)
						text[i + j] = 0x2665;
				}
			}
		} else {
			for (int j = 0; j < matchLen; j++) {
				int k;
				for (k = 0; k < 28; k++) {
					if (text[i + j] == FakeTable[k])
						break;
				}

				if (k == 28)
					text[i + j] = 0x2665;
			}
		}
	}

	if (found)
		WideCharToMultiByte(0, 0, text, -1, out, 0x400, 0, 0);

	return found;
}

// hatmanager.cpp:641
// WStrCmp for Japanese: only ASCII characters compare case-insensitively.
bool WStrCmp_JP(unsigned short *word, int wordLen, unsigned short *text, int textLen) {
	if (wordLen != textLen)
		return true;

	for (int i = 0; i < wordLen - 1; i++) {
		if (word[i] < 0x80 && *text < 0x80) {
			if (tolower(text[i]) != tolower(word[i]))
				return true;
		} else {
			if (word[i] != text[i])
				return true;
		}
	}

	return false;
}

} // namespace BABO_STR

// hatmanager.cpp:826
ChatManager::ChatManager(void) {
	m_floodCount = 0;
	m_repeatCount = 0;
	m_lastChatTime = g_CurrentTime;
	m_floodPenalty = false;
	m_repeatPenalty = false;
	m_chatFilter = 0;
	m_nickFilter = 0;
	m_chatFilterCount = 0;
	m_nickFilterCount = 0;
	m_ctorClearedFlag = false;
}

// hatmanager.cpp:840
ChatManager::~ChatManager(void) {
	if (m_chatFilter) { delete[] m_chatFilter; m_chatFilter = 0; }
	if (m_nickFilter) { delete[] m_nickFilter; m_nickFilter = 0; }
}

// hatmanager.cpp:847
bool ChatManager::Load(void) {
	bool loaded = false;

	loaded |= LoadChatFilter();
	loaded |= LoadNickFilter();

	return loaded;
}

// hatmanager.cpp:857
// Reads chat.bin: a zip whose items, bit-inverted, hold a word count and
// the forbidden chat words.
bool ChatManager::LoadChatFilter(void) {
	WFile file("chat.bin");

	int length = file.Length();
	char *data = new char[length];

	file.Read(data, length);
	file.Close();

	HZIP zip = OpenZipU(data, length, 3);
	if (zip == 0) {
		delete[] data;
		return false;
	}

	ZIPENTRY entry;
	GetZipItemA(zip, -1, &entry);
	int count = entry.index;

	for (int i = 0; i < count; i++) {
		GetZipItemA(zip, i, &entry);
		char *item = new char[entry.unc_size];
		UnzipItem(zip, i, item, entry.unc_size, 3);

		for (int k = 0; k < entry.unc_size; k++)
			item[k] = ~item[k];

		m_chatFilterCount = 0;
		int wordCount = *(int *)item;

		m_chatFilterCount = wordCount;
		if (m_chatFilterCount > 0) {
			m_chatFilter = new unsigned short[m_chatFilterCount][15];
			memcpy(m_chatFilter, item + 4, m_chatFilterCount * 30);
		}

		delete[] item;
	}

	CloseZipU(zip);
	delete[] data;

	return true;
}

// hatmanager.cpp:912
// Reads nick.bin, laid out as chat.bin.
bool ChatManager::LoadNickFilter(void) {
	WFile file("nick.bin");

	int length = file.Length();
	char *data = new char[length];

	file.Read(data, length);
	file.Close();

	HZIP zip = OpenZipU(data, length, 3);
	if (zip == 0) {
		delete[] data;
		return false;
	}

	ZIPENTRY entry;
	GetZipItemA(zip, -1, &entry);
	int count = entry.index;

	for (int i = 0; i < count; i++) {
		GetZipItemA(zip, i, &entry);
		char *item = new char[entry.unc_size];
		UnzipItem(zip, i, item, entry.unc_size, 3);

		for (int k = 0; k < entry.unc_size; k++)
			item[k] = ~item[k];

		m_nickFilterCount = 0;
		int wordCount = *(int *)item;

		m_nickFilterCount = wordCount;
		if (m_nickFilterCount > 0) {
			m_nickFilter = new sNickFilterWord[m_nickFilterCount];
			memcpy(m_nickFilter, item + 4, m_nickFilterCount * sizeof(sNickFilterWord));
		}

		delete[] item;
	}

	CloseZipU(zip);
	delete[] data;

	return true;
}

// hatmanager.cpp:969
void ChatManager::ParseString(const char *) {
}

// hatmanager.cpp:977
// Lifts the player's chat penalty once 60 s have passed since it started
// (packet 0x4F with 0); nothing while replaying.
void ChatManager::CheckPenaltyTime(void) {
	if (WSingleton<CSharedDoc>::Instance()->m_playingMode == 2)
		return;

	sPlayerData *player = 0;
	if (!((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&ntAvatarChatTask::m_RTTI) : false))
		player = PLAYER(MyIndex());

	if (OnlinePlay() && player && player->m_chatPenalty && g_CurrentTime - m_lastChatTime > 60000) {
		player->m_chatPenalty = false;
		WSendPacket packet(GCT_CHAT_PENALTY);
		packet.Encode1(0);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// hatmanager.cpp:1008
// Sends a chat line: dropped while replaying (local content 0x4F), when
// empty or only spaces, or when chat is blocked; runs the chat commands,
// hands the line to the avatar chat room, applies the flood (5 filtered
// lines) and repeat (3 identical lines) penalties, then sends it: packet
// 0x54 when `broadcast`, a whisper (0x2A) to `target`, a guild line (0x25
// on the messenger server) when `target` is the guild word, else a room
// chat line (0x03, nickname + text) outside game mode 5.
void ChatManager::SendChatMessage(const char *text, const char *target, bool broadcast) {
	if (IsLocalContent((localContentType_t)0x4F) && WSingleton<CSharedDoc>::Instance()->m_playingMode == 2)
		return;

	int len = strlen(text);

	if (len == 0)
		return;

	int i;
	for (i = 0; i < len && text[i] == ' '; i++)
		;

	if (i == len)
		return;

	char lastMessage[0x400];

	if (WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().flagBlock & 8) {
		WSingleton<CChatMsg>::Instance()->AddChatMsg("(\xBE\xCB\xB8\xB2)\xC3\xA4\xC6\xC3 \xC0\xCC\xBF\xEB \xC1\xA6\xC0\xE7 \xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9. \xC3\xA4\xC6\xC3\xC0\xBB \xC0\xCC\xBF\xEB\xC7\xCF\xBD\xC7 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0xFFFF7878, true, false);
		return;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_chatHistory.size() != 0)
		strcpy(lastMessage, WSingleton<CSharedDoc>::Instance()->m_chatHistory.back().c_str());
	else
		lastMessage[0] = 0;

	RecordMyChatHistory(text);

	int command = CheckChatCommand(text, 1);

	if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&ntAvatarChatTask::m_RTTI) : false) {
		MsgObject msg(0, 0x60, 0, (intptr_t)text, 0, 0, 0);
		AfxGetTask()->GetActor("AvatarChat") << msg;
	}

	if (command == 1)
		return;

	unsigned long elapsed = g_CurrentTime - m_lastChatTime;

	if (m_floodPenalty) {
		if (elapsed > 60000) {
			m_floodPenalty = false;
			m_floodCount = 0;
		} else {
			WSingleton<CChatMsg>::Instance()->AddChatMsg("(\xBE\xCB\xB8\xB2)1\xBA\xD0\xB0\xA3 \xC3\xA4\xC6\xC3 \xB1\xDD\xC1\xF6\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9", 0xFFFF7878, true, false);
			return;
		}
	}

	if (m_repeatPenalty) {
		if (elapsed > 60000) {
			m_repeatPenalty = false;
			m_repeatCount = 0;
		} else {
			WSingleton<CChatMsg>::Instance()->AddChatMsg("(\xBE\xCB\xB8\xB2)1\xBA\xD0\xB0\xA3 \xC3\xA4\xC6\xC3 \xB1\xDD\xC1\xF6\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9", 0xFFFF7878, true, false);
			return;
		}
	}

	if (!AdminMode()
		&& (elapsed < 1000 || (elapsed < 10000 && stricmp(text, lastMessage) == 0))) {
		m_repeatCount++;
		if (m_repeatCount == 3) {
			WSingleton<CChatMsg>::Instance()->AddChatMsg("(\xBE\xCB\xB8\xB2)\xC3\xA4\xC6\xC3\xB5\xB5\xB9\xE8 \xB9\xFA\xC4\xA2\xBF\xA1 \xB0\xC9\xB8\xAE\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.", 0xFFFF7878, true, false);
			WSingleton<CChatMsg>::Instance()->AddChatMsg("(\xBE\xCB\xB8\xB2)1\xBA\xD0\xB0\xA3 \xC3\xA4\xC6\xC3\xB1\xDD\xC1\xF6\xC0\xD4\xB4\xCF\xB4\xD9.", 0xFFFF7878, true, false);
			m_lastChatTime = g_CurrentTime;
			m_repeatPenalty = true;

			sPlayerData *player = 0;
			if (!((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&ntAvatarChatTask::m_RTTI) : false))
				player = PLAYER(MyIndex());

			if (OnlinePlay() && player && !player->m_chatPenalty) {
				player->m_chatPenalty = true;
				WSendPacket packet(GCT_CHAT_PENALTY);
				packet.Encode1(1);
				packet.Send(SEND_TO_DEFAULT);
			}
			return;
		}
	} else {
		m_repeatCount = 0;
	}

	char message[0x400] = { 0 };
	strcpy(message, text);

	if (!AdminMode() && Filtering(message)) {
		m_floodCount++;
		if (m_floodCount == 5) {
			WSingleton<CChatMsg>::Instance()->AddChatMsg("(\xBE\xCB\xB8\xB2)5\xC8\xB8\xC0\xCC\xBB\xF3 \xBF\xE5\xBC\xB3\xB4\xDC\xBE\xEE\xB8\xA6 \xBB\xE7\xBF\xEB\xC7\xCF\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.", 0xFFFF7878, true, false);
			WSingleton<CChatMsg>::Instance()->AddChatMsg("(\xBE\xCB\xB8\xB2)1\xBA\xD0\xB0\xA3 \xC3\xA4\xC6\xC3\xB1\xDD\xC1\xF6\xC0\xD4\xB4\xCF\xB4\xD9.", 0xFFFF7878, true, false);
			m_lastChatTime = g_CurrentTime;
			m_floodPenalty = true;

			sPlayerData *player = 0;
			if (!((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&ntAvatarChatTask::m_RTTI) : false))
				player = PLAYER(MyIndex());

			if (OnlinePlay() && player && !player->m_chatPenalty) {
				player->m_chatPenalty = true;
				WSendPacket packet(GCT_CHAT_PENALTY);
				packet.Encode1(1);
				packet.Send(SEND_TO_DEFAULT);
			}
			return;
		}
	}

	if (broadcast) {
		WSendPacket packet(GCT_TEAMCHAT);
		packet.EncodeStr(std::string(message));
		packet.Send(SEND_TO_DEFAULT);
	} else if (target) {
		if (strcmp(target, "\xB1\xE6\xB5\xE5\xBF\xA1\xB0\xD4") == 0) {
			if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_MSN)
				&& WSingleton<CMessengerInfo>::Instance()->m_serverListRequest == 4) {
				std::string line(message);

				WSendPacket packet(GCR_INVITE_TO_GUILD);
				packet.EncodeStr(std::string(line.c_str()));
				packet.Send(SEND_TO_MSN);
			} else {
				MsgObject msg(0, 3, (intptr_t)"\xC1\xF6\xB1\xDD\xC0\xBA \xB1\xE6\xB5\xE5 \xB4\xEB\xC8\xAD\xB8\xA6 \xC7\xCF\xBD\xC7 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0xFFFFCC00, 0, 0, 0);
				AfxGetTask()->GetActor("Lobby") << msg;
			}
		} else {
			std::string to(target);
			std::string line(message);

			WSendPacket packet(GCT_WHISPER);
			packet.EncodeStr(to);
			packet.EncodeStr(line);
			packet.Send(SEND_TO_DEFAULT);
		}
	} else if (WSingleton<CSharedDoc>::Instance()->m_gameMode != 5) {
		std::string nickname(WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().nickname);
		std::string line(message);

		WSendPacket packet(GCT_CHATMSG);
		packet.EncodeStr(nickname);
		packet.EncodeStr(line);
		packet.Send(SEND_TO_DEFAULT);
	}

	m_lastChatTime = g_CurrentTime;
}

// hatmanager.cpp:1262
// Returns true when `text` contains one of chat.bin's words; the query
// characters of `text` are then encoded (FilteringHack).
bool ChatManager::Filtering(char *text) {
	if (text == 0)
		return false;

	int i;
	bool found;
	unsigned short wideWord[0x400];
	unsigned short wideText[0x400];

	found = false;
	int textLen = MultiByteToWideChar(0, 0, text, strlen(text) + 1, wideText, 0x400);

	for (i = 0; i < m_chatFilterCount; i++) {
		char *word = (char *)m_chatFilter[i];
		int wordLen = MultiByteToWideChar(0, 0, word, strlen(word) + 1, wideWord, 0x400);

		if (BABO_STR::WStrStr(text, wideText, textLen, wideWord, wordLen, false))
			found = true;
	}

	FilteringHack(text, true);

	return found;
}

// hatmanager.cpp:1308
int ChatManager::GetFaceIndex(const char *text) {
	for (int face = 0; face < 3; face++) {
		for (int i = 0; i < 12; i++) {
			if (strstr(text, FacialTextTable[face][i]))
				return face;
		}
	}

	return 5;
}

// hatmanager.cpp:1340
const char *ChatManager::GetMotionName(const char *text, unsigned long type) {
	char command[64];

	for (int pass = 0; pass < 9; pass++) {
		sMotionText *motions = data_00a9e9d8[type & 0x3FFFFFF];
		for (int i = 0; i < 14; i++) {
			sMotionText *motion = &motions[i];
			if (motion == 0)
				break;

			for (int k = 0; k < 7; k++) {
				if (motion->m_command[k] == 0)
					break;

				sprintf(command, "/%s", motion->m_command[k]);
				if (strstr(text, command))
					return motions[i].m_name;
			}
		}
	}

	return 0;
}

// hatmanager.cpp:1384
// Encodes (or decodes back) the characters that could break a query:
// ';', '"', '\'' and "--".
void ChatManager::FilteringHack(const char *text, bool encode) {
	if (text == 0)
		return;

	char hackChar[4] = { ';', '"', '\'', '-' };
	char hackCode[4] = { 0x11, 0x12, 0x13, 0x14 };
	char unused[8];	// never used: the original has its /GS cookie (likely a dropped buffer)
	char *str = (char *)text;
	int len = strlen(str);

	for (int i = 0; i < len; i++) {
		if (encode) {
			for (int k = 0; k < 4; k++) {
				if (str[i] == hackChar[k]) {
					if (k == 3) {
						if (i < len - 1) {
							if (str[i + 1] == '-') {
								str[i] = hackCode[k];
								str[i + 1] = hackCode[k];
							}
						}
					} else {
						str[i] = hackCode[k];
					}
				}
			}
		} else {
			for (int k = 0; k < 4; k++) {
				if (str[i] == hackCode[k])
					str[i] = hackChar[k];
			}
		}
	}
}

// hatmanager.cpp:1435
// Returns why `nick` cannot be a nickname (one of the hatManager.h reasons,
// or an emoticon's from FilteringEmoticon); 0 when it is fine.
const char *ChatManager::FilteringNick(char *nick) {
	if (nick == 0)
		return s_nickReasonEmpty;

	if (strlen(nick) < 4 || strlen(nick) > 16)
		return s_nickReasonLength;

	unsigned short wideNick[0x400];
	int nickLen = MultiByteToWideChar(0, 0, nick, strlen(nick) + 1, wideNick, 0x400);

	for (int i = 0; i < nickLen - 1; i++) {
		if (wideNick[i] == ' ')
			return s_nickReasonSpace;

		if (wideNick[i] == '(') {
			for (int j = i + 1; j < nickLen - 1; j++) {
				if (wideNick[j] == ')') {
					unsigned short emoticon[256] = { 0 };
					memmove(emoticon, &wideNick[i + 1], (&wideNick[j] - &wideNick[i + 1]) * sizeof(unsigned short));

					const char *reason = FilteringEmoticon(emoticon);
					if (reason)
						return reason;
				}
			}
		}

		if (!((wideNick[i] >= 0x21 && wideNick[i] <= 0x7E) || (wideNick[i] >= 0xAC00 && wideNick[i] <= 0xD7A3)))
			return s_nickReasonBadChar;
	}

	if (WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().identity & 0x14)
		return 0;

	unsigned short wideWord[0x400];
	for (int i = 0; i < m_nickFilterCount; i++) {
		char *word = m_nickFilter[i].m_word;
		int wordLen = MultiByteToWideChar(0, 0, word, strlen(word) + 1, wideWord, 0x400);

		if (m_nickFilter[i].m_exact == 1) {
			if (!BABO_STR::WStrCmp(wideNick, nickLen, wideWord, wordLen))
				return s_nickReasonBadWord;
		} else {
			if (BABO_STR::WStrStr(nick, wideNick, nickLen, wideWord, wordLen, false))
				return s_nickReasonHasBadWord;
		}
	}

	return 0;
}

// hatmanager.cpp:1530
// Checks a guild name or text: 1 when fine, 1000 without text, 0 (after a
// message box) for a bad length, 0x413 for a space (when `noSpace`), 0x414
// for a quote, 0x415 for a nickname filter word.
int ChatManager::FilteringGuildString(const char *text, int minLen, int maxLen, bool noSpace) {
	if (text == 0)
		return 1000;

	int len = strlen(text);
	if (len < minLen || len > maxLen) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)MakeStr("\\c0xffff0000\\c\xBF\xB5\xB9\xAE %d - %d \xB1\xDB\xC0\xDA\\c0xff000000\\c \xB6\xC7\xB4\xC2 \\c0xffff0000\\c\xC7\xD1\xB1\xDB %d - %d\xB1\xDB\xC0\xDA\\c0xff000000\\c\xC0\xC7 \xB1\xE6\xC0\xCC\xB8\xA6 \xB0\xA1\xC1\xAE\xBE\xDF \xC7\xD5\xB4\xCF\xB4\xD9.", minLen, maxLen, minLen >> 1, maxLen >> 1), 0, 0, 0, 0));
		return 0;
	}

	unsigned short wideText[0x400];
	int textLen = MultiByteToWideChar(0, 0, text, strlen(text) + 1, wideText, 0x400);

	for (int i = 0; i < textLen - 1; i++) {
		if (noSpace) {
			if (wideText[i] == ' ')
				return 0x413;
		}

		if (wideText[i] == '\'')
			return 0x414;
	}

	unsigned short wideWord[0x400] = { 0 };
	for (int i = 0; i < m_nickFilterCount; i++) {
		char *word = m_nickFilter[i].m_word;
		int wordLen = MultiByteToWideChar(0, 0, word, strlen(word) + 1, wideWord, 0x400);

		if (m_nickFilter[i].m_exact == 1) {
			if (!BABO_STR::WStrCmp(wideText, textLen, wideWord, wordLen))
				return 0x415;
		} else {
			if (BABO_STR::WStrStr((char *)text, wideText, textLen, wideWord, wordLen, false))
				return 0x415;
		}
	}

	return 1;
}

// hatmanager.cpp:1605
// Returns why `text` cannot be used as a nickname because of emoticons: a
// '-', or an emoticon's alias; 0 when it is fine (or for an emoticon's own
// name).
const char *ChatManager::FilteringEmoticon(unsigned short *text) {
	if (!IsLocalContent((localContentType_t)0x81))
		return 0;

	FrEmoticon *emoticon = g_pFresh->GetManager()->GetEmoticon();
	int count = emoticon->GetIconNum();

	std::string alias[2];
	char name[256];
	WideCharToMultiByte(0, 0, text, -1, name, 256, 0, 0);

	if (strstr(name, "-"))
		return "\xBF\xAA\xC0\xA7 \xC0\xCC\xB8\xF0\xC6\xBC\xC4\xDC\xC0\xBA \xB4\xD0\xB3\xD7\xC0\xD3\xBF\xA1 \xBB\xE7\xBF\xEB\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9";

	for (int i = 0; i < count; i++) {
		if (strcmp(name, emoticon->GetIconName(i)) == 0)
			return 0;

		emoticon->GetAlias(&alias, i);
		if (alias[0] == name)
			return "\xC0\xD4\xB7\xC2\xC7\xCF\xBD\xC5 \xC0\xCC\xB8\xF0\xC6\xBC\xC4\xDC \xC1\xF6\xBD\xC3\xBE\xEE\xB4\xC2 \xB4\xD0\xB3\xD7\xC0\xD3\xBF\xA1 \xBB\xE7\xBF\xEB\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9";

		if (alias[1] == name)
			return "\xC0\xD4\xB7\xC2\xC7\xCF\xBD\xC5 \xC0\xCC\xB8\xF0\xC6\xBC\xC4\xDC \xC1\xF6\xBD\xC3\xBE\xEE\xB4\xC2 \xB4\xD0\xB3\xD7\xC0\xD3\xBF\xA1 \xBB\xE7\xBF\xEB\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9";
	}

	return 0;
}

// hatmanager.cpp:1652
// The older chat line sender (CChatMsg::ProcessMacro): SendChatMessage
// without the replay and avatar chat room steps, its notices going to the
// lobby. Chat commands run with Display type 0; the room chat line (0x03)
// is sent in every game mode.
void ChatManager::SendChatMessage2(const char *text, const char *target, bool broadcast) {
	int len = strlen(text);

	if (len == 0)
		return;

	int i;
	for (i = 0; i < len; i++) {
		if (text[i] != ' ')
			break;
	}

	if (i == len)
		return;

	if (WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().flagBlock & 8) {
		WSingleton<CChatMsg>::Instance()->AddChatMsg("(\xBE\xCB\xB8\xB2)\xC3\xA4\xC6\xC3 \xC0\xCC\xBF\xEB \xC1\xA6\xC0\xE7 \xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9. \xC3\xA4\xC6\xC3\xC0\xBB \xC0\xCC\xBF\xEB\xC7\xCF\xBD\xC7 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0xFFFF7878, true, false);
		return;
	}

	char lastMessage[0x400];

	if (WSingleton<CSharedDoc>::Instance()->m_chatHistory.size() != 0)
		strcpy(lastMessage, WSingleton<CSharedDoc>::Instance()->m_chatHistory.back().c_str());
	else
		lastMessage[0] = 0;

	RecordMyChatHistory(text);

	int command = CheckChatCommand(text, 0);

	if (command == 1)
		return;

	unsigned long elapsed = g_CurrentTime - m_lastChatTime;

	if (m_floodPenalty) {
		if (elapsed > 60000) {
			m_floodPenalty = false;
			m_floodCount = 0;
		} else {
			std::string notice("\xBE\xCB\xB8\xB2 : 1\xBA\xD0\xB0\xA3 \xC3\xA4\xC6\xC3 \xB1\xDD\xC1\xF6\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9");
			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 3, (intptr_t)notice.c_str(), 0xFFFF7878, 0, 0, 0);
			return;
		}
	}

	if (m_repeatPenalty) {
		if (elapsed > 60000) {
			m_repeatPenalty = false;
			m_repeatCount = 0;
		} else {
			std::string notice("\xBE\xCB\xB8\xB2 : 1\xBA\xD0\xB0\xA3 \xC3\xA4\xC6\xC3 \xB1\xDD\xC1\xF6\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9");
			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 3, (intptr_t)notice.c_str(), 0xFFFF7878, 0, 0, 0);
			return;
		}
	}

	if (!AdminMode()
		&& (elapsed < 1000 || (elapsed < 10000 && stricmp(text, lastMessage) == 0))) {
		m_repeatCount++;
		if (m_repeatCount == 3) {
			std::string notice("\xBE\xCB\xB8\xB2 : \xC3\xA4\xC6\xC3\xB5\xB5\xB9\xE8 \xB9\xFA\xC4\xA2\xBF\xA1 \xB0\xC9\xB8\xAE\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.");
			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 3, (intptr_t)notice.c_str(), 0xFFFF7878, 0, 0, 0);

			notice = "\xBE\xCB\xB8\xB2 : 1\xBA\xD0\xB0\xA3 \xC3\xA4\xC6\xC3\xB1\xDD\xC1\xF6\xC0\xD4\xB4\xCF\xB4\xD9.";
			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 3, (intptr_t)notice.c_str(), 0xFFFF7878, 0, 0, 0);

			m_lastChatTime = g_CurrentTime;
			m_repeatPenalty = true;
			return;
		}
	} else {
		m_repeatCount = 0;
	}

	char message[0x400] = { 0 };
	strcpy(message, text);

	if (!AdminMode() && Filtering(message)) {
		m_floodCount++;
		if (m_floodCount == 5) {
			std::string notice("\xBE\xCB\xB8\xB2 : 5\xC8\xB8\xC0\xCC\xBB\xF3 \xBF\xE5\xBC\xB3\xB4\xDC\xBE\xEE\xB8\xA6 \xBB\xE7\xBF\xEB\xC7\xCF\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.");
			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 3, (intptr_t)notice.c_str(), 0xFFFF7878, 0, 0, 0);

			notice = "\xBE\xCB\xB8\xB2 : 1\xBA\xD0\xB0\xA3 \xC3\xA4\xC6\xC3\xB1\xDD\xC1\xF6\xC0\xD4\xB4\xCF\xB4\xD9.";
			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 3, (intptr_t)notice.c_str(), 0xFFFF7878, 0, 0, 0);

			m_lastChatTime = g_CurrentTime;
			m_floodPenalty = true;
			return;
		}
	}

	if (broadcast) {
		WSendPacket packet(GCT_TEAMCHAT);
		packet.EncodeStr(std::string(message));
		packet.Send(SEND_TO_DEFAULT);
	} else if (target) {
		if (strcmp(target, "\xB1\xE6\xB5\xE5\xBF\xA1\xB0\xD4") == 0) {
			if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_MSN)
				&& WSingleton<CMessengerInfo>::Instance()->m_serverListRequest == 4) {
				std::string line(message);

				WSendPacket packet(GCR_INVITE_TO_GUILD);
				packet.EncodeStr(std::string(line.c_str()));
				packet.Send(SEND_TO_MSN);
			} else {
				MsgObject msg(0, 3, (intptr_t)"\xC1\xF6\xB1\xDD\xC0\xBA \xB1\xE6\xB5\xE5 \xB4\xEB\xC8\xAD\xB8\xA6 \xC7\xCF\xBD\xC7 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0xFFFFCC00, 0, 0, 0);
				AfxGetTask()->GetActor("Lobby") << msg;
			}
		} else {
			// The whisper encodes fresh copies; these two stay unused.
			std::string to(target);
			std::string line(message);

			WSendPacket packet(GCT_WHISPER);
			packet.EncodeStr(std::string(target));
			packet.EncodeStr(std::string(message));
			packet.Send(SEND_TO_DEFAULT);
		}
	} else {
		std::string nickname(WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().nickname);
		std::string line(message);

		WSendPacket packet(GCT_CHATMSG);
		packet.EncodeStr(nickname);
		packet.EncodeStr(line);
		packet.Send(SEND_TO_DEFAULT);
	}

	m_lastChatTime = g_CurrentTime;
}

// hatmanager.cpp:1947
// Runs a "/command" line: the GM toolkit's commands for an administrator,
// then ChatCmd's: "/identity [n] [nickname]" (packet 0x41) and, with
// identity bit 6, "/itemdrop [rate]" (packet 0x4E). Returns 1 when the line
// starts with '/' (it is not sent as chat), else 0. `type` is Display's.
int ChatManager::CheckChatCommand(const char *text, int type) {
	if (text[0] == '/') {
		CGMToolkit *toolkit = WSingleton<CGMToolkit>::Instance();

		if (toolkit->IsAdministrator()) {
			if (toolkit->Command(text))
				return 1;
		}
	}

	if (text[0] == '/') {
		char command[0x200];
		const char *args = text + 1;

		GetCharsTillSpace(args, command);

		if (command[0] == 0)
			return 1;

		int i;
		for (i = 0; i < sizeof(ChatCmd) / sizeof(ChatCmd[0]); i++) {
			if (stricmp(command, ChatCmd[i].m_name) == 0)
				break;
		}

		if (i < sizeof(ChatCmd) / sizeof(ChatCmd[0])) {
			switch (i) {
			case 0: {
				char name[0x18];
				int identity = 0;

				int count = sscanf(args, "%s %d %s", command, &identity, name);

				switch (count) {
				case 1: {
					WSendPacket packet(GCT_REQUEST_IDENTITY);
					packet.Encode4(0xFFFFFFFF);
					packet.EncodeStr(std::string(WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().nickname));
					packet.Send(SEND_TO_DEFAULT);
					break;
				}
				case 2: {
					WSendPacket packet(GCT_REQUEST_IDENTITY);
					packet.Encode4(identity);
					packet.EncodeStr(std::string(WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().nickname));
					packet.Send(SEND_TO_DEFAULT);
					break;
				}
				case 3: {
					WSendPacket packet(GCT_REQUEST_IDENTITY);
					packet.Encode4(identity);
					packet.EncodeStr(std::string(name));
					packet.Send(SEND_TO_DEFAULT);
					break;
				}
				}
				break;
			}
			case 2:
				if (((unsigned long)WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().identity >> 6) & 1) {
					int rate = 0;

					int count = sscanf(args, "%s %d", command, &rate);

					switch (count) {
					case 1: {
						WSendPacket packet(GCT_SET_SYSTEM);
						packet.Encode4(3);
						packet.Encode1(0);
						packet.Send(SEND_TO_DEFAULT);
						break;
					}
					case 2:
						if (rate < 100) {
							Display("\xBE\xC6\xC0\xCC\xC5\xDB\xB5\xE5\xB7\xD3 \xC8\xAE\xB7\xFC\xC0\xCC \xB1\xE2\xBA\xBB(100%)\xBA\xB8\xB4\xD9 \xC0\xDB\xBD\xC0\xB4\xCF\xB4\xD9. \xB4\xD9\xBD\xC3 \xBC\xB3\xC1\xA4\xC7\xD8 \xC1\xD6\xBC\xBC\xBF\xE4.", type);
						} else {
							WSendPacket packet(GCT_SET_SYSTEM);
							packet.Encode4(3);
							packet.Encode1(1);
							packet.Encode4(rate);
							packet.Send(SEND_TO_DEFAULT);
						}
						break;
					default:
						Display(MakeStr("\xB4\xD9\xBD\xC3 \xC0\xD4\xB7\xC2\xC7\xD8 \xC1\xD6\xBC\xBC\xBF\xE4. \xBF\xB9)/itemdrop \xB9\xE8\xC0\xB2"), type);
						break;
					}
				}
				break;
			}
		}

		return 1;
	}

	return 0;
}

// hatmanager.cpp:2103
// Writes the chat view's lines to history/chatting/chat<NNN>.txt: the
// first free number when `index` is negative, else index + 1. Returns the
// number used.
int ChatManager::SaveChatHistory(int index, int type) {
	if (WSingleton<CSharedDoc>::Instance()->m_chatLineList.size() == 0)
		return 0;

	mkdir("history");
	mkdir("history/chatting");

	char fileName[MAX_PATH];

	if (index < 0) {
		int i = 1;
		do {
			sprintf(fileName, "history/chatting/chat%03d.txt", i++);
		} while (_access(fileName, 0) != -1);
		index = i - 1;
	} else {
		index++;
		sprintf(fileName, "history/chatting/chat%03d.txt", index);
	}

	FILE *fp = fopen(fileName, "wt");
	if (fp == 0)
		return 0;

	for (std::list<sChatLine>::iterator it = WSingleton<CSharedDoc>::Instance()->m_chatLineList.begin();
		it != WSingleton<CSharedDoc>::Instance()->m_chatLineList.end(); ++it) {
		fwrite(it->m_text.c_str(), strlen(it->m_text.c_str()), 1, fp);
		fwrite("\n", 1, 1, fp);
	}

	fclose(fp);

	Display(MakeStr("\xC3\xA4\xC6\xC3 \xB3\xBB\xBF\xEB\xC0\xCC %s\xBF\xA1 \xC0\xFA\xC0\xE5\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.", fileName), type);

	return index;
}

// hatmanager.cpp:2147
void ChatManager::Display(const char *text, int type) {
	if (type == 0) {
		AfxGetTask()->GetActor("Lobby") << MsgObject(0, 3, (intptr_t)text, 0xFFFF7878, 0, 0, 0);
	} else if (type == 1) {
		WSingleton<CChatMsg>::Instance()->AddChatMsg(text, 0xFFFF7878, true, false);
	}
}

// hatmanager.cpp:2158
// Writes the finished tournament's results to the first free
// history/tournament/result<NNN>.txt, unless a player has not finished.
void ChatManager::RecordTournament(void) {
	char path[MAX_PATH];

	std::vector<sGameResultInfo>::iterator it;
	for (it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
		if (it->m_state == 0)
			return;
	}

	int count = WSingleton<CSharedDoc>::Instance()->m_gameResultList.size();
	if (count == 0)
		return;

	mkdir("history");
	mkdir("history/tournament");

	unsigned char index = 1;
	do {
		sprintf(path, "history/tournament/result%03d.txt", index++);
	} while (_access(path, 0) != -1);

	FILE *fp = fopen(path, "wt");
	if (fp == 0)
		return;

	fprintf(fp, "\xB9\xE6\xC0\xE5 : %s\n\n", WSingleton<CSharedDoc>::Instance()->m_gameResultList[0].m_nickname);
	fprintf(fp, "\xBC\xF8\xC0\xA7     \xB7\xB9  \xBA\xA7           \xB4\xEB \xC8\xAD \xB8\xED       \xBD\xBA\xC4\xDA\xBE\xEE   \xC6\xCE\n");
	fprintf(fp, "--------------------------------------------------------\n");

	for (it = WSingleton<CSharedDoc>::Instance()->m_gameResultList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_gameResultList.end(); ++it) {
		fprintf(fp, " %2d    %s    %s    %3d    %I64d\n", it->m_state == 3 ? 0 : it->m_rank,
			GetAlignedString(data_00a12bc8[it->m_level], 11, 0), GetAlignedString(it->m_nickname, 16, 0),
			it->m_score, it->m_pang);
	}

	fclose(fp);

	Display(MakeStr("30\xC0\xCE\xB4\xEB\xC8\xB8 \xB0\xE1\xB0\xFA\xB0\xA1 %s\xBF\xA1 \xC0\xFA\xC0\xE5\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.", path), 0);
}

// The ring of GetAlignedString's results.
static char s_alignedString[8][128];
static int s_alignedIndex;

// hatmanager.cpp:2205
// Pads `text` with spaces to `width` characters: 0 left, 1 centred, 2 right.
const char *ChatManager::GetAlignedString(const char *text, int width, int align) {
	if (width > 128)
		return text;

	s_alignedIndex++;

	memset(s_alignedString[s_alignedIndex], ' ', width);

	int offset;
	switch (align) {
	case 0:
		offset = 0;
		break;
	case 1:
		offset = (width - strlen(text)) / 2;
		break;
	case 2:
		offset = width - strlen(text);
		break;
	}

	strncpy(s_alignedString[s_alignedIndex] + offset, text, strlen(text));
	s_alignedString[s_alignedIndex][width] = 0;

	return s_alignedString[s_alignedIndex];
}

// hatmanager.cpp:2241
// Remembers a sent line: moved to the end when already known, else the
// oldest of 20 is dropped.
void ChatManager::RecordMyChatHistory(const char *text) {
	std::list<std::string> &history = WSingleton<CSharedDoc>::Instance()->m_chatHistory;
	std::list<std::string>::iterator it = std::find(history.begin(), history.end(), text);

	if (it != history.end()) {
		history.erase(it);
	} else if (WSingleton<CSharedDoc>::Instance()->m_chatHistory.size() == 20) {
		WSingleton<CSharedDoc>::Instance()->m_chatHistory.erase(history.begin());
	}

	history.push_back(text);
}
