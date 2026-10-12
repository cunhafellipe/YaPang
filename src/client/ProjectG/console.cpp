// Original: d:\build\custom\temp\code\client\projectg\console.cpp
#include "console.h"
#include "NetworkSystem.h"   // before anything that pulls <windows.h> (it brings <winsock2.h>)

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "ChatMsg.h"
#include "ClientSetting.h"
#include "InputManager.h"
#include "GolfDoc.h"
#include "Packet.h"
#include "camerawork.h"
#include "club.h"
#include "golfball.h"
#include "headicon.h"
#include "polysoup.h"
#include "PowerGauge.h"
#include "SceneManager.h"
#include "SharedDoc.h"
#include "commonutil.h"
#include "quadtree.h"
#include "TaskManager.h"
#include "tweaker.h"
#include "sky.h"
#include "SoundManager.h"
#include "weather.h"
#include "wind.h"
#include "wutil.h"
#include "wvideo.h"
#include "wfont.h"
#include "winput.h"
#include "wmath.h"
#include "woverlay.h"
#include "wtypes.h"
#include "wresrcmng.h"
#include "wvideo.h"
#include "wview.h"
#include "shared/luasystem/LuaScriptManager.h"
#include "contentsdoc.h"
#include "client/Fresh/include/frwnd.h"
#include "client/Fresh/include/fresh.h"
#include "RealMyRoomTask.h"
#include "ShopTask.h"

// The console commands (0x59 of them, 0xA7FB98): the name, the identifier
// BrowseCommand returns and the usage BadArgument prints. The usages are
// code page 949.
struct {
	const char *name;                   // 0x0
	int id;                             // 0x4
	const char *usage;                  // 0x8
} command_table[] = {
	{ "help", 0, "help" },
	{ "clear", 1, "clear" },
	{ "caddie", 2, "caddie [serial no.] [level(1-4)]" },
	{ "map", 3, "map [value(0-2)]" },
	{ "hole", 4, "hole [value(1-18)]" },
	{ "weather", 5, "weather [value(0-3)]" },
	{ "heal", 6, "heal [value]" },
	{ "clubset", 7, "clubset [value(0-3)]" },
	{ "combo", 8, "combo [value]" },
	{ "tag", 9, "tag" },
	{ "render", 10, "render [value(0-3)]" },
	{ "fill", 11, "fill [value(0-2)]" },
	{ "fov", 12, "fov [value(10-120)]" },
	{ "dist", 13, "dist" },
	{ "save", 14, "save [filename]" },
	{ "load", 15, "load [filename]" },
	{ "wind", 16, "wind [value(0-9)] [value(0-360)]" },
	{ "world", 17, "world : \xbf\xf9\xb5\xe5\xc0\xc7 \xc5\xa9\xb1\xe2\xb8\xa6 \xbe\xcb\xb7\xc1 \xc1\xdc" },
	{ "cull", 18, "cull [value]" },
	{ "timeout", 19, "timeout : \xc5\xb8\xc0\xd3 \xbe\xc6\xbf\xf4 \xc5\xe4\xb1\xdb (on:default)" },
	{ "cameracheck", 20, "cameracheck : \xc4\xab\xb8\xde\xb6\xf3 \xc3\xbc\xc5\xb7 \xc5\xe4\xb1\xdb (on:default)" },
	{ "slope", 21, "slope [value] (1.7:default)" },
	{ "texture", 22, "texture [filename]" },
	{ "effectlevel", 23, "effectlevel [value(0-2)] (1:default)" },
	{ "mipfilter", 24, "mipfilter [value(0-5)] (1:default)" },
	{ "mparts", 25, "mparts [part name]" },
	{ "dparts", 26, "dparts [typeid]" },
	{ "statistics", 27, "statistics" },
	{ "pangya", 28, "pangya [value(n)] (1:default)" },
	{ "alpangya", 29, "alpangya [value(n)] (1:default)" },
	{ "ball", 30, "ball [serial no.]" },
	{ "scale", 31, "scale [value(1-\xa1\xc4)] (1:default)" },
	{ "trace", 32, "trace [value] (value=\xc7\xc1\xb7\xb9\xc0\xd3 \xb9\xf8\xc8\xa3)" },
	{ "grid", 33, "grid [value]" },
	{ "gridshow", 34, "gridshow [0-1]" },
	{ "drop", 35, "drop" },
	{ "award", 36, "award" },
	{ "ai", 37, "ai" },
	{ "key", 38, "key" },
	{ "special", 39, "special [shot ID( Tomahawk:1, Cobra:2, Spike:3 )]" },
	{ "lowest", 40, "lowest" },
	{ "highest", 41, "highest" },
	{ "debugline", 42, "debugline [value]" },
	{ "god", 43, "god" },
	{ "togid", 44, "toggle id" },
	{ "power", 45, "power value[0-100]" },
	{ "impact", 46, "impact value[0-30]" },
	{ "control", 47, "control value[0-30]" },
	{ "spin", 48, "spin value[0-30]" },
	{ "curve", 49, "curve value[0-30]" },
	{ "scrmode", 50, "windowed, fullscreen toggle" },
	{ "bright", 51, "bright [value(0-1)] (1:default)" },
	{ "holecup", 52, "holecup [0-3] (0:random, 1-3:specific holecup location)" },
	{ "bounce", 53, "bounce [0-\xa1\xc4] (1:default)" },
	{ "roll", 54, "roll [0-\xa1\xc4] (1:default)" },
	{ "condition", 55, "condition [-50-50] (0:default)" },
	{ "temperature", 56, "temperature" },
	{ "humidity", 57, "humidity [0-100]" },
	{ "unbalance", 58, "unbalance impact control" },
	{ "green", 59, "green [value(0-\xa1\xc4)] (0.2:default)" },
	{ "camvel", 60, "camvel [value(0-\xa1\xc4)] (-1:default)" },
	{ "mouse", 61, "mouse [value(0.01-\xa1\xc4)] (1.0:default)" },
	{ "transparent", 62, "transparent (on:default)" },
	{ "plist", 63, "plist : part list of cur character" },
	{ "camtarget", 64, "camtarget [value(0-\xa1\xc4)] (100.0:defalut)" },
	{ "paint", 65, "paint mesh_index (-1:default)" },
	{ "speed", 66, "speed (5.0:default)" },
	{ "listbres", 67, "list bres" },
	{ "lake", 68, "lake speed alpha" },
	{ "showmethemoney", 69, "user pang and cookie is increase" },
	{ "gravity", 70, "gravity modifier [value]" },
	{ "rm_camera", 71, " camera display [limit(0: nolimit, 1: width, 2 : hight)][-90~90]" },
	{ "rm_furniture", 72, " furniture get  display [limit(0: name, 1: x , 2: y, 3:z)][-50~50, 0~10, -20~20]" },
	{ "pivot", 73, " camera posion %f, %f, %f " },
	{ "bar", 74, "input number [2~9]" },
	{ "tw cmd all", 75, "tw cmd all : \xbb\xe7\xbf\xeb \xb0\xa1\xb4\xc9\xc7\xd1 \xb8\xf0\xb5\xe7 \xc6\xae\xc0\xa7\xc4\xbf \xc3\xb5\xc1\xa6 \xb8\xed\xb7\xc9\xbe\xee\xb5\xe9 \xb8\xf1\xb7\xcf\xc0\xd4\xb4\xcf\xb4\xd9." },
	{ "tw list", 76, "tw list : \xc6\xae\xc0\xa7\xc4\xbf\xb7\xce \xb5\xee\xb7\xcf\xb5\xc8 \xc5\xac\xb7\xa1\xbd\xba \xb8\xf1\xb7\xcf\xc0\xbb \xc7\xa5\xbd\xc3 \xc7\xd5\xb4\xcf\xb4\xd9." },
	{ "tw using", 77, "tw using : tw list\xbf\xa1 \xb5\xee\xb7\xcf\xb5\xc8 \xc5\xac\xb7\xa1\xbd\xba\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xd4\xc0\xb8\xb7\xce\xbd\xe1, \xc0\xfc\xc3\xbc \xb8\xed\xb7\xc9\xbe\xee\xb0\xa1 \xbe\xc6\xb4\xd1 \xc7\xd8\xb4\xe7 \xc6\xae\xc0\xa7\xc4\xbf\xc0\xc7 \xb8\xed\xb7\xc9\xbe\xee\xb8\xa6 \xb9\xd9\xb7\xce \xc7\xd8\xbc\xae\xc7\xd5\xb4\xcf\xb4\xd9." },
	{ "powerbar", 78, "powerbar setting!" },
	{ "posmask", 79, "posmask %d(TypeID), %d(PosMask)" },
	{ "texname", 80, "texname %d(TypeID), %d(0:normal tex 1:original tex), %d(tex num : 0 ~ 2), %s(tex name)" },
	{ "petname", 81, "petname %d(TypeID), %s(PetName)" },
	{ "guild", 82, "guild test" },
	{ "vectortype", 85, "vectortype [0-1]" },
	{ "vectorspeed", 86, "vectorspeed [%d]  default limitspeed 100" },
	{ "vectorheight", 87, "vectorheight [%d] default bound height 30" },
	{ "vectorcount", 88, "vectorcount [%d] default limit bound count 20" },
	{ "luacmd", 83, "luacmd [index]  index(0:reload,1:runfile \x22test.lua\x22 2:peras-only)" },
	{ "lua", 84, "lua \x22" "command string\x22 " },
};

template <> CConsole *WSingleton<CConsole>::m_pInstance = NULL;

// The caustic animation of the lakes (pool.cpp; publics ?causticSpeed@@3MA, ?causticAlpha@@3HA).
extern float causticSpeed;
extern int causticAlpha;

// The IME input device (public ?g_ime@@3PAVWInputDev@@A).
extern WInputDev *g_ime;

// The last IME composition InputProcess appended (no symbol).
static int s_lastComposition;

// The tweaker "tw using" selected, the target of the following commands.
static tweaker *s_pTweaker;

// console.cpp:333
int _POSTMSG(const void *pSender, const char *pTarget, int msg, int param1, int param2, int param3, unsigned long param4) {
	return CTaskManager::Instance()->PostMsg(NULL, pTarget, msg, param1, param2, param3, param4);
}

// console.cpp:343
CConsole::CConsole(void)
	: m_active(false), m_keyStates(NULL), m_history(32, 0), m_lines(32, 0), m_repeatDelay(5.0f) {
}

// console.cpp:347
CConsole::~CConsole(void) {
	ShutDown();
}

// console.cpp:352
void CConsole::Init(void) {
	m_bsStack[0] = 0;
	m_input[0] = 0;

	m_keyStates = new ConsoleKeyState[0x59];
	for (unsigned int i = 0; i < 0x59; i++) {
		m_keyStates[i].m_pressed = 0;
	}

	m_alpha = 0.0f;

	m_inputLine[0] = 0;
	m_inputLocked = false;
	m_lineOffset = 0;
	m_state = 1;

	m_pFont = CChatMsg::Instance()->GetMaskedFont();
}

// console.cpp:391
void CConsole::ShutDown(void) {
	if (m_keyStates) {
		delete[] m_keyStates;
		m_keyStates = NULL;
	}

	for (char *line = m_history.Start(); line != NULL; line = m_history.Next()) if (line) delete[] line; m_history.Reset();

	for (char *line = m_lines.Start(); line != NULL; line = m_lines.Next()) if (line) delete[] line; m_lines.Reset();
}

// console.cpp:400
void CConsole::Process(float elapsed) {
	// The original's body (lines 400-450) compiled to nothing: only the
	// `ret 4` remains.
}

// console.cpp:453
void CConsole::InputProcess(float elapsed) {
	int key = 0;
	char text[4];
	char result[0x80];

	text[0] = 0;
	int ch = g_ime->GetState(3, 0);

	if (ch) {
		if (ch == 9) {
			key = ch;
		} else if (ch == 8 || ch == 13 || ch == 6 || ch == 7 || ch == 2 || ch == 3 || ch == 4 || ch == 5) {
			key = ch;
			m_input[0] = 0;
		} else if (ch & 0x80) {
			text[0] = (char)ch;
			text[1] = (char)(ch >> 8);
			text[2] = 0;

			PopFromBsStack();
			PopFromBsStack();
			m_input[0] = 0;
		} else if (ch >= 0x20 && ch <= 0x7A) {
			text[0] = (char)ch;
			text[1] = 0;
			if (ch == 0x20) {
				m_bsStack[0] = 0;
			} else {
				PopFromBsStack();
			}
			m_input[0] = 0;
		}

		if (text[0]) {
			if (m_inputLocked) {
				m_inputLine[strlen(m_inputLine) - 2] = 0;
				strcat(m_inputLine, text);
			} else {
				strcat(m_inputLine, text);
			}
			m_inputLine[0x7F] = 0;
		}

		m_inputLocked = false;
	}

	int composition = g_ime->GetState(3, -2);
	if (composition) {
		if (composition != s_lastComposition) {
			s_lastComposition = composition;
			text[0] = (char)composition;
			text[1] = (char)(composition >> 8);
			text[2] = 0;

			if (m_inputLocked) {
				m_inputLine[strlen(m_inputLine) - 2] = 0;
				strcat(m_inputLine, text);
			} else {
				strcat(m_inputLine, text);
			}
			m_inputLine[0x7F] = 0;
			m_inputLocked = true;
		}
	} else if (m_inputLocked) {
		m_inputLine[strlen(m_inputLine) - 2] = 0;
		m_inputLocked = false;
	}

	switch (key) {
	case 9: {
		char *word = m_inputLine;
		if (!m_input[0]) {
			strcpy(m_input + 1, m_inputLine);
			m_input[0] = 1;
			char *space = strstr(m_inputLine, " ");
			while (space) {
				word = space + 1;
				strcpy(m_input + 1, word);
				space = strstr(word, " ");
			}

			for (unsigned int i = 0; i < 0x59; i++) {
				m_keyStates[i].m_pressed = 0;
			}
		}

		if (word == m_inputLine) {
			if (BrowseCommand(m_input + 1, result) > -1) {
				strcpy(word, result);
			}
		} else {
			char *end = strchr(word, 0);
			if (isalnum(end[-1])) {
				while ((*end = PopFromBsStack()) != 0) {
					end++;
				}
			} else {
				char c;
				while ((c = PopFromBsStack()) != 0) {
					if (isalnum(c)) {
						*end = c;
						while ((*++end = PopFromBsStack()) != 0) {
						}
						break;
					}
				}
			}
			m_input[0] = 0;
		}
		break;
	}
	case 13:
		m_bsStack[0] = 0;
		if (strlen(m_inputLine)) {
			char *line = new char[strlen(m_inputLine) + 1];
			strcpy(line, m_inputLine);
			m_history += line;
			AddText(MakeStr(">%s", line));

			if (!TweakerCommandProc(m_inputLine)) {
				Command(strlwr(m_inputLine));
			}

			m_inputLine[0] = 0;
			int offset = m_lines.GetCount() - 19;
			m_lineOffset = offset > 0 ? offset : 0;
			m_historyPos = NULL;
			m_historyEdge = 1;
		}
		break;
	case 6:
		if (m_historyEdge >= 0 && m_history.GetCount() > 0) {
			m_historyEdge = 0;
			if (m_historyPos) {
				char *entry = m_history.Prev(m_historyPos);
				if (m_historyPos) {
					strcpy(m_inputLine, entry);
				} else {
					m_historyEdge = -1;
					m_history.Start(m_historyPos);
				}
			} else {
				char *entry = m_history.End(m_historyPos);
				strcpy(m_inputLine, entry);
			}
		}
		break;
	case 7:
		if (m_historyEdge <= 0 && m_history.GetCount() > 0) {
			m_historyEdge = 0;
			if (m_historyPos) {
				char *entry = m_history.Next(m_historyPos);
				if (m_historyPos) {
					strcpy(m_inputLine, entry);
				} else {
					m_historyEdge = 1;
					m_inputLine[0] = 0;
				}
			}
		}
		break;
	case 8: {
		int len = strlen(m_inputLine);
		if (len > 0 && !m_inputLocked) {
			if (len > 1 && m_inputLine[len - 1] < 0) {
				PushIntoBsStack(m_inputLine[len - 1]);
				PushIntoBsStack(m_inputLine[len - 2]);
				m_inputLine[len - 2] = 0;
			} else {
				if (m_inputLine[len - 1] == ' ') {
					m_bsStack[0] = 0;
				} else {
					PushIntoBsStack(m_inputLine[len - 1]);
				}
				m_inputLine[len - 1] = 0;
			}
		}
		break;
	}
	case 2:
		if (m_lines.GetCount() > 19) {
			int offset = m_lineOffset - 1;
			m_lineOffset = offset > 0 ? offset : 0;
		}
		break;
	case 3:
		if (m_lines.GetCount() > 19) {
			int offset = m_lineOffset + 1;
			int last = m_lines.GetCount() - 19;
			m_lineOffset = offset > last ? last : offset;
		}
		break;
	}
}

// console.cpp:743
void CConsole::Display(void) {
	if (WAbsBySign(m_alpha) < WMATH_EPSILON) {
		return;
	}

	WOverlay::DrawBox(g_view, WRect(0, 0, g_view->GetWidth(), 302), 0, (int)(m_alpha * 128.0f) << 24, 0.001f);

	int count = m_lines.GetCount();
	int empty = (19 - count > 0) ? 19 - count : 0;

	float y = empty * 15.0f + 5.0f;
	int alpha = (int)(m_alpha * 255.0f);
	int i = 0;
	for (char *line = m_lines.Start(); line != NULL; line = m_lines.Next(), i++) {
		if (i == m_lineOffset + 19) {
			break;
		}
		if (i >= m_lineOffset) {
			m_pFont->Print(g_view, 6.0f, y + 1.0f, line, 0, alpha << 24, NULL);
			m_pFont->Print(g_view, 5.0f, y, line, 0, (alpha << 24) | 0xFFFFFF, NULL);
			y += 15.0f;
		}
	}

	float width = m_pFont->Print(g_view, 5.0f, y, MakeStr(">%s", m_inputLine), 0, (alpha << 24) | 0xFFFFFF, NULL);
	m_pFont->Flush(g_view);

	WOverlay::DrawBox(g_view, WRect(width + 5.0f, y + 10.0f, 7, 2), 0, (alpha << 24) | 0xFFFFFF, 0.001f);
	WOverlay::DrawBox(g_view, WRect(g_view->GetWidth() - 12.0f, 5, 10, 292), 0, (alpha << 24) | 0x404040, 0.001f);

	count = m_lines.GetCount();
	if (count > 0 && count > 19) {
		WOverlay::DrawBox(g_view, WRect(g_view->GetWidth() - 10.0f, m_lineOffset * 292.0f / count + 5.0f, 6, 292.0f * 19 / count), 0, (alpha << 24) | 0x707070, 0.001f);
	}
}

// console.cpp:790
bool CConsole::PushIntoBsStack(char c) {
	int len = strlen(m_bsStack);

	if (len < 0x7F) {
		m_bsStack[len] = c;
		m_bsStack[len + 1] = 0;
		return true;
	}

	return false;
}

// console.cpp:804
char CConsole::PopFromBsStack(void) {
	int len = strlen(m_bsStack);
	if (len == 0) {
		return 0;
	}

	char c = m_bsStack[len - 1];
	m_bsStack[len - 1] = 0;
	return c;
}

// console.cpp:817
void CConsole::ClearBsStack(void) {
	m_bsStack[0] = 0;
}

// console.cpp:822
int CConsole::BrowseCommand(const char *partial, char *result) {
	unsigned int i;
	for (i = 0; i < 0x59; i++) {

		m_keyStates[i].m_code = 1;
	}


	int len = strlen(partial);


	int matched;
	for (matched = 0; matched < len; matched++) {

		bool done = true;

		for (i = 0; i < 0x59; i++) {

			if (m_keyStates[i].m_code) {

				int nameLen = strlen(command_table[i].name);

				if (nameLen > matched && partial[matched] == command_table[i].name[matched]) {





					done = false;
				} else {

					m_keyStates[i].m_code = 0;
				}
			}
		}

		if (done) {
			break;
		}
	}







	bool allBrowsed = true;
	for (i = 0; i < 0x59; i++) {

		if (m_keyStates[i].m_code) {

			if (!m_keyStates[i].m_pressed) {
				allBrowsed = false;
				break;
			}
		}
	}




	if (allBrowsed) {

		for (i = 0; i < 0x59; i++) {

			m_keyStates[i].m_pressed = 0;
		}
	}




	int best = -1;

	for (i = 0; i < 0x59; i++) {

		if (m_keyStates[i].m_code && !m_keyStates[i].m_pressed) {

			int nameLen = strlen(command_table[i].name);

			if (nameLen <= matched) {

				if (result) strcpy(result, command_table[i].name);


				return i;
			}


			if (best == -1 || command_table[best].name[matched] > command_table[i].name[matched]) {

				best = i;
			}
		}
	}


	if (best == -1) {

		*result = 0;
		return -1;
	}




	m_keyStates[best].m_pressed = 1;
	if (result) strcpy(result, command_table[best].name);

	return command_table[best].id;
}

// console.cpp:922
int CConsole::TweakerCommandProc(const char *line) {
	if (stricmp(line, "tw cmd all") == 0) {

		std::vector<std::string> list;
		tweaker_database::instance()->get_tweaker_command_all(list);
		if (list.size() == 0) {
			// "사용 가능한 트위커 명령이 없습니다."
			AddText("\xBB\xE7\xBF\xEB \xB0\xA1\xB4\xC9\xC7\xD1 \xC6\xAE\xC0\xA7\xC4\xBF \xB8\xED\xB7\xC9\xC0\xCC \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.");
			return 1;
		}

		for (std::vector<std::string>::iterator it = list.begin(); it != list.end(); ++it) {
			AddText(it->c_str());
		}
		return 1;
	}

	if (stricmp(line, "tw list") == 0) {

		std::vector<std::string> list;
		tweaker_database::instance()->get_tweaker_names(list);

		if (list.size() == 0) {
			// "사용 가능한 트위커가 없습니다."
			AddText("\xBB\xE7\xBF\xEB \xB0\xA1\xB4\xC9\xC7\xD1 \xC6\xAE\xC0\xA7\xC4\xBF\xB0\xA1 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.");
			return 1;
		}

		for (std::vector<std::string>::iterator it = list.begin(); it != list.end(); ++it) {
			AddText(it->c_str());
		}
		return 1;
	}

	if (strncmp(line, "tw using", 8) == 0) {

		char name[128] = {0};
		char command[128] = {0};
		_snscanf(line + 9, 127, "%s %s", name, command);
		std::string fullName(name);
		fullName += " ";
		fullName += command;
		s_pTweaker = tweaker_database::instance()->get_tweaker(fullName);
		if (s_pTweaker) {
			// "입력된 트위커를 시작합니다. 다음은 예약된 명령어들 입니다."
			AddText("\xC0\xD4\xB7\xC2\xB5\xC8 \xC6\xAE\xC0\xA7\xC4\xBF\xB8\xA6 \xBD\xC3\xC0\xDB\xC7\xD5\xB4\xCF\xB4\xD9. \xB4\xD9\xC0\xBD\xC0\xBA \xBF\xB9\xBE\xE0\xB5\xC8 \xB8\xED\xB7\xC9\xBE\xEE\xB5\xE9 \xC0\xD4\xB4\xCF\xB4\xD9.");
			std::vector<std::string> list;
			s_pTweaker->get_command_list(list, false);

			for (std::vector<std::string>::iterator it = list.begin(); it != list.end(); ++it) {
				AddText(it->c_str());
			}
		} else {
			// "트위커가 종료되었습니다.(tw list를 통해 확인하신 후 입력하세요.)"
			AddText("\xC6\xAE\xC0\xA7\xC4\xBF\xB0\xA1 \xC1\xBE\xB7\xE1\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.(tw list\xB8\xA6 \xC5\xEB\xC7\xD8 \xC8\xAE\xC0\xCE\xC7\xCF\xBD\xC5 \xC8\xC4 \xC0\xD4\xB7\xC2\xC7\xCF\xBC\xBC\xBF\xE4.)");
		}
		return 1;
	}

	if (s_pTweaker) {

		if (tweaker_database::instance()->is_exist_tweaker(s_pTweaker)) {

			if (s_pTweaker->is_exist_command(std::string(line))) {

				std::string result = s_pTweaker->run_command(line);
				AddText(result.c_str());
				return 1;
			}
		} else {
			// "이전 사용을 선언한 트위커가 더이상 유효하지 않아서 자동 종료되었습니다."
			AddText("\xC0\xCC\xC0\xFC \xBB\xE7\xBF\xEB\xC0\xBB \xBC\xB1\xBE\xF0\xC7\xD1 \xC6\xAE\xC0\xA7\xC4\xBF\xB0\xA1 \xB4\xF5\xC0\xCC\xBB\xF3 \xC0\xAF\xC8\xBF\xC7\xCF\xC1\xF6 \xBE\xCA\xBE\xC6\xBC\xAD \xC0\xDA\xB5\xBF \xC1\xBE\xB7\xE1\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.");
			s_pTweaker = NULL;
		}
	} else {

		if (tweaker_database::instance()->is_exist_command(line)) {

			std::string result = tweaker_database::instance()->send_command(line);

			AddText(result.c_str());
			return 1;
		}
	}

	return 0;
}

// console.cpp:1016
void CConsole::Command(const char *line) {
	for (unsigned char i = 0; i < 0x59; i++) {
		int len = strlen(command_table[i].name);
		if (strncmp(line, command_table[i].name, len) == 0 && (line[len] == ' ' || line[len] == '\0')) {
			int id = command_table[i].id;

			switch (id) {
			// console.cpp:1034
			case 66: { // "speed"
				float speed;
				sscanf(line + len + 1, "%f", &speed);

				if (speed < 1.0f) {
					BadArgument(66);
					return;
				}
				m_repeatDelay = speed;
				return;
			}
			// console.cpp:1046
			case 65: { // "paint"
				int triangle = -1;
				sscanf(line + len + 1, "%d", &triangle);

				GetPVS().m_markedTriangle = triangle;
				return;
			}
			// console.cpp:1055
			case 64: { // "camtarget"
				int target = -1;
				sscanf(line + len + 1, "%d", &target);

				if (target <= 0) {
					BadArgument(64);
					return;
				}
				CTaskManager::Instance()->PostMsg(0, "GolfRule", 0x9D, target, 0, 0, 0);
				return;
			}
			// console.cpp:1217
			case 63: { // "plist"
				CPartTidList &tids = GOLFDOC()->GetPartTidList(GOLFDOC()->m_myPlayerIndex);
				const std::map<unsigned int, IFF_STRUCT::sPart> &parts = ItemManager()->m_partMap;

				FILE *fp = fopen("PartList.txt", "wt");
				int index = 1;

				std::map<unsigned int, IFF_STRUCT::sPart>::const_iterator part;
				for (part = parts.begin(); part != parts.end(); part++) {
					unsigned long tid = part->second.m_typeId;
					if (((tid >> 18) & 0xFF) == (tids.m_tidChar & 0x3FFFFFF) && (tid & 0x600) == 0) {
						fprintf(fp, "%d(%2d): %s\n", tid, index, part->second.m_name);
						AddText(MakeStr("%d(%2d): %s", part->second.m_typeId, index, part->second.m_name));
						index++;
					}
				}

				const std::map<unsigned int, IFF_STRUCT::sAuxPart> &auxParts = ItemManager()->m_auxPartMap;
				std::map<unsigned int, IFF_STRUCT::sAuxPart>::const_iterator aux;
				for (aux = auxParts.begin(); aux != auxParts.end(); aux++) {
					fprintf(fp, "%d(%2d): %s\n", aux->second.m_typeId, index, aux->second.m_name);
					AddText(MakeStr("%d(%2d): %s", aux->second.m_typeId, index, aux->second.m_name));
					index++;
				}

				fclose(fp);
				return;
			}
			// console.cpp:1261
			case 62: // "transparent"
				AddText((CSceneManager::Instance()->m_overlapCamera = !CSceneManager::Instance()->m_overlapCamera) ? "Transparent ON" : "Transparent OFF");
				return;
			// console.cpp:1268
			case 61: { // "mouse"
				float sensitivity = 0.0f;
				sscanf(line + len + 1, "%f", &sensitivity);

				if (sensitivity < 0.01f) {
					BadArgument(61);
					return;
				}
				// the original stores m_sensitivity directly (protected in InputManager.h)
				g_input->SetSensitivity(sensitivity);
				return;
			}
			// console.cpp:1280
			case 60: { // "camvel"
				int velocity = -1;
				sscanf(line + len + 1, "%d", &velocity);

				if (velocity < 0) {
					BadArgument(60);
					return;
				}
				CTaskManager::Instance()->PostMsg(0, "GolfRule", 0x9B, velocity, 0, 0, 0);
				return;
			}
			// console.cpp:1292
			case 59: { // "green"
				float slope = -1.0f;
				sscanf(line + len + 1, "%f", &slope);

				if (slope <= 0.0f) {
					BadArgument(59);
					return;
				}
				AddText(MakeStr("%.2f->%.2f", GetPVS().m_greenSlope, slope));

				GetPVS().m_greenSlope = slope;
				GetPVS().CalcGreenPlane();
				return;
			}
			// console.cpp:1309
			case 58: // "unbalance"
				{
					float impact, control;
					sscanf(line + len + 1, "%f %f", &impact, &control);

					if (impact > 0.0f) {
						AddText(MakeStr("impact : %.1f->%.1f", GOLFDOC()->m_impactAccuracy, impact));
						GOLFDOC()->m_impactAccuracy = impact;

						GolfClub().SetAccuracy();
					}

					if (control > 0.0f) {
						AddText(MakeStr("control : %.1f->%.1f", GOLFDOC()->m_controlAccuracy, control));
						GOLFDOC()->m_controlAccuracy = control;
					}
				}
				return;
			// console.cpp:1330
			case 55: { // "condition"
				float condition;
				sscanf(line + len + 1, "%f", &condition);

				if (condition < -50.0f || condition > 50.0f) {
					BadArgument(55);
				} else {
					GetPVS().m_condition = condition;
				}
				return;
			}
			// console.cpp:1345
			case 53: { // "bounce"
				float bounce;
				sscanf(line + len + 1, "%f", &bounce);

				if (bounce < 0.0f) {
					BadArgument(53);
					return;
				}
				GetPVS().m_bounceFactor = bounce;
				return;
			}
			// console.cpp:1357
			case 54: { // "roll"
				float roll;
				sscanf(line + len + 1, "%f", &roll);

				if (roll < 0.0f) {
					BadArgument(54);
					return;
				}
				GetPVS().m_rollFactor = roll;
				return;
			}
			// console.cpp:1369
			case 52: { // "holecup"
				int pin = -1;
				sscanf(line + len + 1, "%d", &pin);

				if (pin >= 0 && pin <= 3) {
					// 0: a random pin position per hole, 1-3: that position on every hole
					for (int hole = 0; hole < 18; hole++) {
						CSharedDoc::Instance()->m_holePin[hole] = pin == 0 ? rand() % 3 : pin - 1;
					}
					return;
				}
				BadArgument(52);
				return;
			}
			// console.cpp:1384
			case 51: { // "bright"
				float bright;
				sscanf(line + len + 1, "%f", &bright);

				if (bright < 0.0f || bright > 1.0f) {
					BadArgument(51);
					return;
				}
				GOLFDOC()->UpdateBright(bright);
				return;
			}
			// console.cpp:1396
			case 45: { // "power"
				int level;
				sscanf(line + len + 1, "%d", &level);

				if (level >= 0 && level <= 100) {
					// The stored power is the level less 15.
					AddText(MakeStr("power : Lv%d->Lv%d", PLAYER(GOLFDOC()->m_myPlayerIndex)->m_power + 15, level));
					level -= 15;

					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_power = level;

					CTaskManager::Instance()->PostMsg(0, "Spot", 0xF5, 0, 0, 0, 0);
					GolfClub().SetAccuracy();
					return;
				}
				BadArgument(45);
				return;
			}
			// console.cpp:1416
			case 46: { // "impact"
				int level;
				sscanf(line + len + 1, "%d", &level);

				if (level >= 0 && level <= 30) {
					AddText(MakeStr("impact : Lv%d->Lv%d", (int)PLAYER(GOLFDOC()->m_myPlayerIndex)->m_accuracy, level));

					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_accuracy = level;
					GolfClub().SetAccuracy();
					return;
				}
				BadArgument(46);
				return;
			}
			// console.cpp:1433
			case 47: { // "control"
				int level;
				sscanf(line + len + 1, "%d", &level);

				if (level >= 0 && level <= 30) {
					AddText(MakeStr("control : Lv%d->Lv%d", (int)PLAYER(GOLFDOC()->m_myPlayerIndex)->m_control, level));

					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_control = level;
					return;
				}
				BadArgument(47);
				return;
			}
			// console.cpp:1449
			case 48: { // "spin"
				int level;
				sscanf(line + len + 1, "%d", &level);

				if (level >= 0 && level <= 30) {
					AddText(MakeStr("spin : Lv%d->Lv%d", (int)PLAYER(GOLFDOC()->m_myPlayerIndex)->m_spin, level));

					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_spin = level;
					CTaskManager::Instance()->PostMsg(0, "Spin", 0x271A, 0, 0, 0, 0);
					CTaskManager::Instance()->PostMsg(0, "Spin", 0x163, 0, 0, 0, 0);
					return;
				}
				BadArgument(48);
				return;
			}
			// console.cpp:1467
			case 49: { // "curve"
				int level;
				sscanf(line + len + 1, "%d", &level);

				if (level >= 0 && level <= 30) {
					AddText(MakeStr("curve : Lv%d->Lv%d", (int)PLAYER(GOLFDOC()->m_myPlayerIndex)->m_curve, level));

					// The original shares spin's tail (the two Spin messages).
					PLAYER(GOLFDOC()->m_myPlayerIndex)->m_curve = level;
					CTaskManager::Instance()->PostMsg(0, "Spin", 0x271A, 0, 0, 0, 0);
					CTaskManager::Instance()->PostMsg(0, "Spin", 0x163, 0, 0, 0, 0);
					return;
				}
				BadArgument(49);
				return;
			}
			// console.cpp:1483
			case 50: // "scrmode"
				COption::Instance()->vToggleFullscreen();
				return;
			// console.cpp:1487
			case 44: // "togid"
				if (CHeadIcon::Instance()) {
					CHeadIcon::Instance()->Toggle();
				}
				return;
			// console.cpp:1492
			case 43: // "god"
				GOLFDOC()->m_godMode = !GOLFDOC()->m_godMode;
				PerformPowerBarSetting();
				AddText(GOLFDOC()->m_godMode ? "You are the God now! (~+_+)~" : "You are a man. T_T");
				return;
			// console.cpp:1500
			case 42: { // "debugline"
				int flags;
				sscanf(line + len + 1, "%d", &flags);

				if (CSceneManager::Instance()) {
					CSceneManager::Instance()->m_debugDraw = flags;
				}
				return;
			}
			// console.cpp:1508
			case 40: // "lowest"
				if (CSceneManager::Instance()) {

					if (CSceneManager::Instance()->m_debugDraw >> 4 & 1) {
						CSceneManager::Instance()->m_debugDraw &= ~0x10;
						return;
					}
					CSceneManager::Instance()->m_debugDraw |= 0x10;
				}
				return;
			// console.cpp:1518
			case 41: // "highest"
				if (CSceneManager::Instance()) {

					if (CSceneManager::Instance()->m_debugDraw >> 5 & 1) {
						CSceneManager::Instance()->m_debugDraw &= ~0x20;
						return;
					}
					CSceneManager::Instance()->m_debugDraw |= 0x20;
				}
				return;
			// console.cpp:1528
			case 38: // "key"
				g_input->IncludeKey(0);
				return;
			// console.cpp:1533
			case 37: // "ai"
				if (IsMyTurn(true) && (GOLFDOC()->GameMode() & ~0x402) && CSharedDoc::Instance()->m_playingMode == 1) {
					unsigned char index;

					if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT))
						index = MyIndex();
					else
						index = GOLFDOC()->m_myPlayerIndex;

					if (CSharedDoc::Instance()->m_roomPlayers[index].m_identity & 1)
						AddText("AUTO PLAYER OFF");
					else
						AddText("AUTO PLAYER ON");

					CTaskManager::Instance()->PostMsg(0, "GolfRule", 0x90, index, 0, 0, 0);
				}
				return;
			// console.cpp:1553
			case 35: // "drop"
				CSceneManager::Instance()->DropTest();
				return;
			// console.cpp:1559
			case 33: { // "grid"
				int size;
				sscanf(line + len + 1, "%d", &size);
				CTaskManager::Instance()->PostMsg(0, "Grid", 0x1D3, size, 0, 0, 0);
				return;
			}
			// console.cpp:1567
			case 34: { // "gridshow"
				int show;
				sscanf(line + len + 1, "%d", &show);

				if (show == 1)
					CTaskManager::Instance()->PostMsg(0, "Grid", 0x163, 0, 0, 0, 0);
				else
					CTaskManager::Instance()->PostMsg(0, "Grid", 0x164, 0, 0, 0, 0);
				return;
			}
			// console.cpp:1582
			case 32: { // "trace"
				int frame;
				sscanf(line + len + 1, "%d", &frame);
				if (frame < 0 || frame > GolfBall().m_preDataCount) {
					BadArgument(32);
					return;
				}
				GolfBall().m_pos = GolfBall().PreData(frame)->m_pos;
				return;
			}
			// console.cpp:1592
			case 27: // "statistics"
				GOLFDOC()->m_pPolySoup->MakeStatistics();
				return;
			// console.cpp:1599
			case 22: { // "texture"
				char name[64];
				sscanf(line + len + 1, "%s", name);
				if (g_resrcmng->FindTexture(name) == NULL) {
					AddText("Such Texture Is Not Found");
				}
				return;
			}
			// console.cpp:1614
			case 21: { // "slope"
				float slope;
				sscanf(line + len + 1, "%f", &slope);
				if (slope < 0.0f) {
					BadArgument(21);
					return;
				}
				GetPVS().m_slopeFactor = slope;
				return;
			}
			// console.cpp:1624
			case 18: { // "cull"
				if (COption::Instance()->m_tnlMode) {
					AddText("SoftRender is required");
					return;
				}

				float distance;
				sscanf(line + len + 1, "%f", &distance);
				GOLFDOC()->m_pPolySoup->m_cullDistance = distance * 3.2f;
				return;
			}
			// console.cpp:1639
			case 17: { // "world"
				Waabb box = GOLFDOC()->m_pPolySoup->FindModel(NULL)->m_aabb;
				AddText(MakeStr("%.0f %.0f %.0f", (box.max.x - box.min.x) * 0.3125f, (box.max.y - box.min.y) * 0.3125f, (box.max.z - box.min.z) * 0.3125f));
				return;
			}
			// console.cpp:1646
			case 16: { // "wind"
				float direction = Wind().GetGlobalDirection();
				float intensity;
				int count = sscanf(line + len + 1, "%f %f", &intensity, &direction);

				if (intensity > 10.0f) {
					BadArgument(16);
					return;
				}

				Wind().SetIntensity(floorf(intensity));
				Wind().SetDirection(count == 2 ? direction * 0.017453292f : direction);
				GOLFDOC()->m_pPolySoup->UpdateBgFxWind();
				if (WSingleton<CSceneManager>::Instance()->m_pWeather != NULL) {
					WSingleton<CSceneManager>::Instance()->m_pWeather->UpdateWind();
				}
				return;
			}
			// console.cpp:1665
			case 71: // "rm_camera"
				if (IsLocalContent((localContentType_t)0x56)) {
					int limit = -1;
					float angle1;
					float angle2;
					sscanf(line + len + 1, " %d %f %f", &limit, &angle1, &angle2);
					_POSTMSG(this, "RealMyRoom", 0x209, limit, (int)angle1, (int)angle2, 0);
				}
				return;
			// console.cpp:1677
			case 72: // "rm_furniture"
				if (IsLocalContent((localContentType_t)0x56)) {
					// The name goes to RealMyRoom with the message, which owns it.
					char *name = new char[0x20];
					float x;
					float y;
					float z;
					sscanf(line + len + 1, "%s %f %f %f", name, &x, &y, &z);

					if (*name != '\0') {
						_POSTMSG(this, "RealMyRoom", 0x20A, (int)x, (int)y, (int)name, 0);
					}
				}
				return;
			// console.cpp:1698
			case 73: // "pivot"
				if (IsLocalContent((localContentType_t)0x56)) {
					WVector pos;
					sscanf(line + len + 1, "%f %f %f", &pos.x, &pos.y, &pos.z);
					_POSTMSG(this, "RealMyRoom", 0x22B, (int)pos.x, (int)pos.y, (int)pos.z, 0);
				}
				return;
			// console.cpp:1710
			case 14: { // "save"
				char name[32];
				sscanf(line + len + 1, "%s", name);
				CTaskManager::Instance()->PostMsg(0, "GolfRule", 0x85, (intptr_t)name, 0, 0, 0);
				return;
			}
			// console.cpp:1717
			case 15: { // "load"
				// The name goes to GolfRule with the message, which owns it.
				char *name = new char[0x20];
				sscanf(line + len + 1, "%s", name);

				if (fopen(MakeStr("%s.sav", name), "rb") != NULL) {
					CTaskManager::Instance()->PostMsg(0, "GolfRule", 0x86, (intptr_t)name, 0, 0, 0);
					return;
				}
				AddText("No Save File");
				return;
			}
			// console.cpp:1729
			case 13: { // "dist"
				for (int player = 0; player < GOLFDOC()->GetPlayerNum(); player++) {
					char text[64];
					if (GOLFDOC()->GetPlayer(player)->m_state != 0) {
						sprintf(text, "%d : -", player);
					} else {
						WVector &ball = GOLFDOC()->GetPlayer(player)->m_ballPos;
						WVector cup = GetHoleCupPos();
						float dx = cup.x - ball.x;
						float dz = cup.z - ball.z;
						float dist = sqrtf(dx * dx + dz * dz);
						sprintf(text, "%d : %.2fy(%.2f)", player, dist * 0.3125f, dist);
					}
					AddText(text);
				}
				return;
			}
			// console.cpp:1749
			case 0: // "help"
				for (int k = 1; k < 0x59; k++) {
					AddText(command_table[k].usage);
				}
				return;
			// console.cpp:1755
			case 1: // "clear"
				for (char *text = m_lines.Start(); text != NULL; text = m_lines.Next()) {
					delete text;
				}
				m_lines.Reset();

				m_lineOffset = 0;
				return;
			// console.cpp:1769
			case 2: { // "caddie"
				int serial;
				int level;
				int cadItem;
				int count = sscanf(line + len + 1, "%d %d %d", &serial, &level, &cadItem);

				if (count >= 3 && count != EOF && level >= 1 && level <= 4) {
					unsigned long caddieTypeId;
					bool valid = true;

					if (serial == -1) {
						caddieTypeId = 0;
					} else {
						caddieTypeId = serial | 0x1C000000;
						if (ItemManager()->FindCaddie(caddieTypeId) == NULL) {
							valid = false;
						} else if (caddieTypeId != 0 && cadItem != 0 && ((((unsigned long)cadItem >> 21) & 0x1F) ^ caddieTypeId) & 0x3FFFFFF) {
							// the item belongs to another caddie
							valid = false;
						}
					}

					if (valid && cadItem != 0 && ItemManager()->FindCadItem(cadItem) == NULL) {
						valid = false;
					}

					if (valid) {
						_POSTMSG(this, "Caddie", 0x4A, caddieTypeId, level - 1, cadItem, 0);
						return;
					}
				}
				AddText("caddie [serial no.] [level(1-4)] [caddie item typeid]");
				return;
			}
			// console.cpp:1827
			case 3: { // "map"
				int map = -1;
				sscanf(line + len + 1, "%d", &map);

				if (map == -1) {
					// List the courses.
					for (int course = 0; course < ItemManager()->GetCourseNum(); course++) {
						char text[64];
						int number = (course > 0x7F && course != 0xFD) ? course - 0x80 : course;
						IFF_STRUCT::sCourse *info = ItemManager()->FindCourse(0x28000000 | number);
						sprintf(text, "%d:%s", course, info->m_name);
						AddText(text);
					}
					return;
				}

				if (map == GetCurMap()) {
					return;
				}

				if (!SetCurMap(map)) {
					AddText("map [serial no.]");
					return;
				}

				g_audio->StopSfx(NULL);

				for (int hole = 1; hole <= 18; hole++) {
					GOLFDOC()->GetHoleData(hole).m_kind = 0;
					GOLFDOC()->GetHoleData(hole).m_mapState = 3;
					GOLFDOC()->m_pPolySoup->LoadMapCheckData(hole);
				}

				CTaskManager::Instance()->PostMsg(0, "GolfBg", 0x140, 0, 0, 0, 0);
				CTaskManager::Instance()->PostMsg(0, "GolfBg", 0x13E, 0, 0, 0, 0);
				return;
			}
			// console.cpp:1876
			case 4: { // "hole"
				int hole;
				sscanf(line + len + 1, "%d", &hole);

				if (hole == GOLFDOC()->m_currentHole) {
					return;
				}

				if (hole >= 1 && hole <= 18) {
					g_audio->StopSfx(NULL);

					GOLFDOC()->m_currentHole = hole;
					CTaskManager::Instance()->PostMsg(0, "GolfBg", 0x140, 0, 0, 0, 0);
					return;
				}
				AddText("Invalid Argument");
				return;
			}
			// console.cpp:1895
			case 5: { // "weather"
				int weather;
				sscanf(line + len + 1, "%d", &weather);

				if (weather > 3) {
					BadArgument(5);
					return;
				}

				CTaskManager::Instance()->PostMsg(0, "GolfBg", 0x142, weather, 0, 0, 0);

				CTaskManager::Instance()->PostMsg(0, "GolfBg", 0x13D, weather, 0, 0, 0);
				if (weather < 2) {
					GOLFDOC()->m_pSky->GenerateStars(-1);
					GOLFDOC()->m_pSky->GenerateLensFlare();
					GOLFDOC()->m_pSky->GenerateMoons();
				}

				CTaskManager::Instance()->PostMsg(0, "Screen", 0x1A3, 0, 0, 0, 0);
				return;
			}
			// console.cpp:1919
			case 6: { // "heal"
				int amount;
				sscanf(line + len + 1, "%d", &amount);
				return;
			}
			// console.cpp:1926
			case 7: { // "clubset"
				int clubSet = 0;
				sscanf(line + len + 1, "%d", &clubSet);

				CTaskManager::Instance()->PostMsg(0, "Player", 0x11C, clubSet | 0x10000000, 1, 0, 0);
				return;
			}
			// console.cpp:1935
			case 8: { // "combo"
				float combo;
				sscanf(line + len + 1, "%f", &combo);
				CSharedDoc *doc = CSharedDoc::Instance();

				if (doc->m_roomType == 1) {
					// The team's gauge.
					TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_teamRate = Between(0.0f, combo + TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_teamRate, doc->GetComboGaugeLimit(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute));
					return;
				}
				PLAYER(GOLFDOC()->m_myPlayerIndex)->m_comboGauge = Between(0.0f, combo + PLAYER(GOLFDOC()->m_myPlayerIndex)->m_comboGauge, CSharedDoc::Instance()->GetComboGaugeLimit(GOLFDOC()->m_myPlayerIndex));
				return;
			}
			// console.cpp:1949
			case 9: // "tag"
				CSceneManager::Instance()->ShowTag();
				return;
			// console.cpp:1955
			case 10: // "render"
				if (COption::Instance()->m_tnlMode != 0) {
					int fillMode;
					sscanf(line + len + 1, "%d", &fillMode);

					g_resrcmng->VideoReference()->Command(WX_VDEV_FILLMODE, fillMode, 0);
					return;
				} else {
					int mode;
					sscanf(line + len + 1, "%d", &mode);

					if (mode >= 0 && mode <= 3) {
						CTaskManager::Instance()->PostMsg(0, "GolfBg", 0x118, mode, 0, 0, 0);
						CTaskManager::Instance()->PostMsg(0, "Player", 0x118, mode, 0, 0, 0);
						CTaskManager::Instance()->PostMsg(0, "Caddie", 0x118, mode, 0, 0, 0);
						return;
					}
					BadArgument(10);
					return;
				}
			// console.cpp:1983
			case 11: { // "fill"
				int fillMode;
				if (sscanf(line + len + 1, "%d", &fillMode) != 1 || fillMode < 0 || fillMode > 2) {
					AddText("fill [value(0-2)]");
					return;
				}

				g_resrcmng->VideoReference()->Command(WX_VDEV_FILLMODE, fillMode, 0);
				return;
			}
			// console.cpp:1995
			case 12: { // "fov"
				int fov;
				sscanf(line + len + 1, "%d", &fov);

				if (fov >= 10 && fov <= 120) {
					g_view->SetFOV(fov * 0.017453292f);
					return;
				}
				BadArgument(12);
				return;
			}
			// console.cpp:2006
			case 19: // "timeout"
				if (CSharedDoc::Instance()->m_shotTimeLimit == 0) {
					CSharedDoc::Instance()->m_shotTimeLimit = 40000;
					GOLFDOC()->m_remainShotTime = CSharedDoc::Instance()->m_shotTimeLimit;
					AddText("TIME LIMIT ON");
					return;
				}
				CSharedDoc::Instance()->m_shotTimeLimit = 0;
				AddText("TIME LIMIT OFF");
				return;
			// console.cpp:2022
			case 20: // "cameracheck"
				{
					bool checking = CCameraWork::Instance()->ToggleCameraChecking();

					AddText(MakeStr(" CameraChecking: %s", checking ? "on" : "off"));
				}
				return;
			// console.cpp:2031
			case 23: // "effectlevel"
				{
					int level;
					sscanf(line + len + 1, "%d", &level);

					if (level >= 0 && level <= 2) {
						COption::Instance()->vSetParticleLevel(level);
						return;
					}
					BadArgument(23);
					return;
				}
			// console.cpp:2044
			case 24: // "mipfilter"
				{
					int filter;
					if (sscanf(line + len + 1, "%d", &filter) != 0 && filter >= 0 && filter <= 5) {
						g_resrcmng->VideoReference()->Command(W_VDEV_MIP_TEXSTAGE_FILTER, filter, 0);
						return;
					}
					BadArgument(24);
					AddText("0: D3DTEXF_NONE");
					AddText("1: D3DTEXF_POINT");
					AddText("2: D3DTEXF_LINEAR");
					AddText("3: D3DTEXF_ANISOTROPIC");
					AddText("4: D3DTEXF_FLATCUBIC");
					AddText("5: D3DTEXF_GAUSSIANCUBIC");
					return;
				}
			// console.cpp:2069
			case 25: // "mparts"
				{
					char cmd[32];
					char name[256] = { 0 };
					sscanf(line, "%s %s", cmd, name);

					if (name[0] == 0) {
						BadArgument(25);
						return;
					}

					// The index-th part of the own character, counted over the
					// character's parts, then over every aux part.
					int index = atoi(name);
					CPartTidList &tids = GOLFDOC()->GetPartTidList(GOLFDOC()->m_myPlayerIndex);
					int count = 1;

					std::map<unsigned int, IFF_STRUCT::sPart>::const_iterator part;
					for (part = ItemManager()->m_partMap.begin(); part != ItemManager()->m_partMap.end(); part++) {
						unsigned long tid = part->second.m_typeId;
						if ((tids.m_tidChar & 0x3FFFFFF) == ((tid >> 18) & 0xFF) && (tid & 0x600) == 0) {
							if (count == index) {
								CTaskManager::Instance()->PostMsg(0, "Player", 0x117, part->second.m_typeId, 0, 0, 0);
								return;
							}
							count++;
						}
					}

					std::map<unsigned int, IFF_STRUCT::sAuxPart>::const_iterator aux;
					for (aux = ItemManager()->m_auxPartMap.begin(); aux != ItemManager()->m_auxPartMap.end(); aux++) {
						if (count == index) {
							CTaskManager::Instance()->PostMsg(0, "Player", 0x117, aux->second.m_typeId, 0, 0, 0);
							return;
						}
						count++;
					}

					AddText(MakeStr("can't find %s", name));
					return;
				}
			// console.cpp:2151
			case 26: // "dparts"
				{
					char cmd[64];
					char arg[32] = { 0 };
					sscanf(line, "%s %s", cmd, arg);

					if (arg[0] == 0) {
						BadArgument(26);
						return;
					}

					unsigned long typeId = strtoul(arg, 0, 10);
					CPartTidList &tids = GOLFDOC()->GetPartTidList(GOLFDOC()->m_myPlayerIndex);
					CItemManager *items = ItemManager();

					std::map<unsigned int, IFF_STRUCT::sPart>::const_iterator part;
					for (part = items->m_partMap.begin(); part != items->m_partMap.end(); part++) {
						if (part->second.m_typeId == typeId) {
							if (((part->second.m_typeId >> 18) & 0xFF) == (tids.m_tidChar & 0x3FFFFFF)) {
								CTaskManager::Instance()->PostMsg(0, "Player", 0x117, part->second.m_typeId, 0, 0, 0);
								return;
							}
							AddText(MakeStr("wrong character's typeid : %d", typeId));
						}
					}

					std::map<unsigned int, IFF_STRUCT::sAuxPart>::const_iterator aux;
					for (aux = items->m_auxPartMap.begin(); aux != items->m_auxPartMap.end(); aux++) {
						if (aux->second.m_typeId == typeId) {
							CTaskManager::Instance()->PostMsg(0, "Player", 0x117, aux->second.m_typeId, 0, 0, 0);
							return;
						}
					}

					AddText(MakeStr("can't find %s", arg));
					return;
				}
			// console.cpp:2229
			case 28: // "pangya"
				{
					int area = -1;
					sscanf(line + len + 1, "%d", &area);
					if (area < 1) {
						BadArgument(28);
						return;
					}
					AddText(MakeStr(" Pangya Area: x%d", area));
					CPowerGauge::Instance()->SetDebugPangYaArea((float)area);
					return;
				}
			// console.cpp:2244
			case 29: // "alpangya"
				{
					int area = -1;
					sscanf(line + len + 1, "%d", &area);
					if (area < 1) {
						BadArgument(29);
						return;
					}
					AddText(MakeStr("AirNight Lucky Pangya Area: %d", area));
					CPowerGauge::Instance()->SetDebugAirNightLuckyPangYaArea((float)area);
					return;
				}
			// console.cpp:2261
			case 30: // "ball"
				{
					int serial;
					int count = sscanf(line + len, "%d", &serial);
					if (count == 0 || count == EOF) {
						AddText("ball [serial no.]");
						return;
					}
					unsigned long ballId = serial | 0x14000000;
					CTaskManager::Instance()->PostMsg(0, "Ball", 0x4C, ballId, 0, 0, 0);
					CTaskManager::Instance()->PostMsg(0, "Screen", 0x4C, ballId, 0, 0, 0);
					return;
				}
			// console.cpp:2279
			case 31: // "scale"
				{
					int scale = 0;
					sscanf(line + len + 1, "%d", &scale);
					if (scale < 1) {
						BadArgument(31);
						return;
					}
					AddText(MakeStr("Picture Scale = x%d", scale));
					CTaskManager::Instance()->PostMsg(0, "GolfRule", 0x8C, scale, 0, 0, 0);
					return;
				}
			// console.cpp:2293
			case 36: // "award"
				CTaskManager::Instance()->PostMsg(0, "GolfRule", 0x78, 0x20000, 0, 0, 0);
				return;
			// console.cpp:2299
			case 39: // "special"
				{
					int rule = 0;
					sscanf(line + len + 1, "%d", &rule);

					if (rule == 1) {
						rule = 0x10;
					} else if (rule == 2) {
						rule = 0x20;
					} else if (rule == 3) {
						rule = 0x40;
					}

					CTaskManager::Instance()->PostMsg(0, "GolfRule", 0x91, rule, 0, 0, 0);
					CPowerGauge::Instance()->SetDebugPangYaArea(10.0f);
					AddText(MakeStr(" Pangya Area: x%d", 10));

					if (CSharedDoc::Instance()->m_roomType == 1) {
						// The team's gauge.
						TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_teamRate = Between(0.0f, TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_teamRate + 100.0f, CSharedDoc::Instance()->GetComboGaugeLimit(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute));
					} else {
						PLAYER(GOLFDOC()->m_myPlayerIndex)->m_comboGauge = Between(0.0f, PLAYER(GOLFDOC()->m_myPlayerIndex)->m_comboGauge + 100.0f, CSharedDoc::Instance()->GetComboGaugeLimit(GOLFDOC()->m_myPlayerIndex));
					}

					AddText(MakeStr(" Combo: +%d", 100));
					return;
				}
			// console.cpp:2327
			case 67: { // "listbres"
				std::string name;
				std::map<std::string, resBitmap_t> bres = g_pFresh->ListBres(name);

				int total = 0;
				CConsole::Print(MakeStr("[%s]", name.c_str()));

				std::map<std::string, resBitmap_t>::const_iterator it;
				for (it = bres.begin(); it != bres.end(); it++) {
					CConsole::Print(MakeStr("%-30s: %d bytes", it->second.name.c_str(), it->second.size));
					total += it->second.size;
				}

				CConsole::Print(MakeStr("total %d bytes loaded", total));
				return;
			}
			// console.cpp:2351
			case 68: // "lake"
				sscanf(line + len + 1, "%f %d", &causticSpeed, &causticAlpha);

				if (causticAlpha >= 0xFF) causticAlpha = 0xFF;
				return;
			// console.cpp:2359
			case 69: { // "showmethemoney"
				WSendPacket packet(GCT_LOCATE_USER);
				packet.Send(SEND_TO_DEFAULT);
				return;
			}
			// console.cpp:2368
			case 70: { // "gravity"
				float gravity;
				sscanf(line + len + 1, "%f", &gravity);
				GetPVS().GravityModifier(1.0f / gravity);
				return;
			}
			// console.cpp:2376
			case 74: // "bar"
				{
					int n = 0;
					sscanf(line + len + 1, "%d", &n);

					switch (n) {
					case 2:
						GOLFDOC()->m_debugBar = 138.0f;
						break;
					case 3:
						GOLFDOC()->m_debugBar = 138.5f;
						break;
					case 4:
						GOLFDOC()->m_debugBar = 139.0f;
						break;
					case 5:
						GOLFDOC()->m_debugBar = 139.5f;
						break;
					case 6:
						GOLFDOC()->m_debugBar = 140.0f;
						break;
					case 7:
						GOLFDOC()->m_debugBar = 140.5f;
						break;
					case 8:
						GOLFDOC()->m_debugBar = 141.5f;
						break;
					case 9:
						GOLFDOC()->m_debugBar = 142.0f;
						break;
					default:
						GOLFDOC()->m_debugBar = 0;
						break;
					}
				}
				return;
			// console.cpp:2414
			case 78: { // "powerbar"
				float first = 0.0f;
				float second = 0.0f;
				sscanf(line + len + 1, "%f %f", &first, &second);
				return;
			}
			// console.cpp:2429
			case 79: { // "posmask"
				CTask *pTask = AfxGetTask();
				if (((__rtti_obj = pTask) != 0 ? ((IObject*)__rtti_obj)->IsExactKindOf(&CShopTask::m_RTTI) : false)
					|| ((__rtti_obj = pTask) != 0 ? ((IObject*)__rtti_obj)->IsExactKindOf(&CRealMyRoomTask::m_RTTI) : false)) {
					unsigned long typeId = 0;
					unsigned long posmask = 0;

					sscanf(line + len + 1, "%d %d", &typeId, &posmask);
					pTask->SendMsgToMainActor(MsgObject(0, 0x266, typeId, 0, 0, 0, 0));
					AddText(ItemManager()->ChangePartsPosmask(typeId, posmask));
					pTask->SendMsgToMainActor(MsgObject(0, 0x267, typeId, 0, 0, 0, 0));
				} else {
					AddText("You can use this function only in the shop and myroom");
				}
				return;
			}
			// console.cpp:2444
			case 80: { // "texname"
				CTask *pTask = AfxGetTask();
				if (((__rtti_obj = pTask) != 0 ? ((IObject*)__rtti_obj)->IsExactKindOf(&CShopTask::m_RTTI) : false)
					|| ((__rtti_obj = pTask) != 0 ? ((IObject*)__rtti_obj)->IsExactKindOf(&CRealMyRoomTask::m_RTTI) : false)) {
					unsigned long typeId = 0;
					unsigned long first = 0;
					unsigned long second = 0;
					char texName[256] = { 0 };

					sscanf(line + len + 1, "%d %d %d %s", &typeId, &first, &second, texName);
					pTask->SendMsgToMainActor(MsgObject(0, 0x266, typeId, 0, 0, 0, 0));
					AddText(ItemManager()->ChangePartsTexName(typeId, first, second, texName));
					pTask->SendMsgToMainActor(MsgObject(0, 0x267, typeId, 0, 0, 0, 0));
				} else {
					AddText("You can use this function only in the shop and myroom");
				}
				return;
			}
			// console.cpp:2464
			case 81: { // "petname"
				CTask *pTask = AfxGetTask();
				if (((__rtti_obj = pTask) != 0 ? ((IObject*)__rtti_obj)->IsExactKindOf(&CShopTask::m_RTTI) : false)
					|| ((__rtti_obj = pTask) != 0 ? ((IObject*)__rtti_obj)->IsExactKindOf(&CRealMyRoomTask::m_RTTI) : false)) {
					char petName[256] = { 0 };
					unsigned long typeId = 0;

					sscanf(line + len + 1, "%d %s", &typeId, petName);
					pTask->SendMsgToMainActor(MsgObject(0, 0x266, typeId, 0, 0, 0, 0));
					AddText(ItemManager()->ChangePartsPetName(typeId, petName));
					pTask->SendMsgToMainActor(MsgObject(0, 0x267, typeId, 0, 0, 0, 0));
				}
				return;
			}
			// console.cpp:2495
			case 84: // "lua"
				if (lua_system::lua_script_manger::GetInstance()->RunString(line + len + 1)) {
					AddText("lua code Success!");
				}
				return;
			// console.cpp:2504
			case 83: { // "luacmd"
				int index;
				sscanf(line + len + 1, "%d", &index);

				switch (index) {
				case 0:
					lua_system::lua_script_manger::GetInstance()->ReloadAll();
					AddText("luafile Reload All.");
					break;
				case 1:
					if (lua_system::lua_script_manger::GetInstance()->RunString("dofile(\"test.lua\")")) {
						AddText("luafile load ok. [test.lua]");
					}
					break;
				case 2: {
					if (lua_system::lua_script_manger::GetInstance()->RunString("dofile(\"test.lua\")")) {
						AddText("luafile load ok. [test.lua]");
					}

					CGimmickContainer *pContainer = 0;
					WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0xA3, pContainer);

					pContainer->LoadFromScript("test_table");
					break;
				}
				default:
					AddText("luacmd exec error. plz check luacmd index");
					break;
				}
				return;
			}
			// console.cpp:2701
			case 85: { // "vectortype"
				CVectorSlideTestContainer *pContainer = 0;
				WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0xA2, pContainer);

				if (pContainer) {
					int newType;

					sscanf(line + len + 1, "%d", &newType);
					pContainer->SetNewTypeVectorSlide(newType != 0);
				}
				return;
			}
			// console.cpp:2714
			case 86: { // "vectorspeed"
				CVectorSlideTestContainer *pContainer = 0;
				WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0xA2, pContainer);

				if (pContainer) {
					int speed;

					sscanf(line + len + 1, "%d", &speed);
					pContainer->SetLimitSpeed(speed);
				}
				return;
			}
			// console.cpp:2728
			case 87: { // "vectorheight"
				CVectorSlideTestContainer *pContainer = 0;
				WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0xA2, pContainer);

				if (pContainer) {
					int height;

					sscanf(line + len + 1, "%d", &height);
					pContainer->SetBoundHeight(height);
				}
				return;
			}
			// console.cpp:2741
			case 88: { // "vectorcount"
				CVectorSlideTestContainer *pContainer = 0;
				WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0xA2, pContainer);

				if (pContainer) {
					int count;

					sscanf(line + len + 1, "%d", &count);
					pContainer->SetLimitVectorSlideCount(count);
				}
				return;
			}
			}
			break;
		}
	}

	AddText("Bad Command. Type 'help' -> list command or 'tweaker' -> list tweaker command");
}

// console.cpp:2759
void CConsole::BadArgument(int command) {
	AddText(MakeStr("Bad Argument : %s", command_table[command].usage));
}

// console.cpp:2764
void CConsole::AddText(const char *text) {
	char *line = new char[strlen(text) + 1];
	strcpy(line, text);
	m_lines += line;

	m_lineOffset = m_lines.GetCount() - 19;
}
