// The JP 983 game's RFTextManager (RFTextManager.h), from the JP exe's
// bodies (node scripts/jp_hlil.ts).
#include "RFTextManager.h"

#include <hash_map>
#include <io.h>
#include <stdio.h>
#include <string.h>

#include "RFTextDraw.h"
#include "wview.h"

// 0xe78c2c
template <> RFTextManager* WSingleton<RFTextManager>::m_pInstance = NULL;

// 0x602fd0
RFTextManager::RFTextManager() {
	// g_view: 0xf023e8.
	m_textDraw = RF_SHARED_PTR<RFTextDraw>(new RFTextDraw(g_view));
}

// 0x602d80 (scalar deleting destructor 0x602f00)
RFTextManager::~RFTextManager() {
}

// 0x602c50
void RFTextManager::DrawTextLarge(float x, float y, const std::wstring& text, uint32_t style, uint32_t color, uint32_t outlineColor, uint32_t shadowColor) {
	if (m_textDraw.get() != NULL) {
		m_textDraw->DrawTextLarge(x, y, text, style, color, outlineColor, shadowColor);
	}
}

// 0x602c90
void RFTextManager::DrawTextSmall(float x, float y, const std::wstring& text, uint32_t style, uint32_t color, uint32_t outlineColor, uint32_t shadowColor) {
	if (m_textDraw.get() != NULL) {
		m_textDraw->DrawTextSmall(x, y, text, style, color, outlineColor, shadowColor);
	}
}

// The translator Jp_LookupText 0xb4eb80 builds once (static 0xf041e8,
// guard 0xf04310 bit 0, destroyed through the atexit thunk 0xcfb8d0; class and
// member names ours, the binary has no RTTI for it): korea.dat and
// japan.dat hold the same number of NUL-terminated strings, the Korean
// source texts and their Japanese texts, paired in order in m_texts; two
// 30-entry caches (keys, texts) are scanned before the map. Size 0x124.
class JpTextTranslator {
public:
	typedef stdext::hash_map<std::string, std::string> TextMap;

	// Inlined in Jp_LookupText: both data pointers cleared, m_texts
	// constructed (0xb4e5d0), then Load.
	JpTextTranslator() : m_koreanData(NULL), m_japaneseData(NULL) {
		Load();
	}
	~JpTextTranslator();

	int ReadDataFile(const char* path, char** data);
	const char* Translate(const char* text);
	void Load();

private:
	char* m_koreanData;              // 0x0 korea.dat's contents
	char* m_japaneseData;            // 0x4 japan.dat's contents
	const char* m_cacheKeys[30];     // 0x8
	const char* m_cacheTexts[30];    // 0x80
	int m_cacheIndex;                // 0xf8
	TextMap m_texts;                 // 0xfc (list head +0x100, size +0x104)
};

// 0xf041e0 (name ours): the Korean texts Load found twice.
static int s_duplicateTextCount;

// 0xb4ca90: reads the whole file into a new[] buffer; its length, 0
// when the file does not open.
int JpTextTranslator::ReadDataFile(const char* path, char** data) {
	FILE* file = fopen(path, "rb");
	if (file == NULL) {
		return 0;
	}
	int length = _filelength(_fileno(file));
	*data = new char[length];
	fread(*data, length, 1, file);
	fclose(file);
	return length;
}

// 0xb4e7c0 (atexit thunk 0xcfb8d0); then m_texts' destructor, inlined.
// The buffers are new[]'d by ReadDataFile; the exe frees them with scalar
// delete (operator delete 0xc3c0b0), UB in C++ but harmless with the MSVC
// allocator, so the port uses delete[].
JpTextTranslator::~JpTextTranslator() {
	if (m_koreanData != NULL) {
		delete[] m_koreanData;
		m_koreanData = NULL;
	}
	if (m_japaneseData != NULL) {
		delete[] m_japaneseData;
		m_japaneseData = NULL;
	}
}

// 0xb4de60
const char* JpTextTranslator::Translate(const char* text) {
	int first;
	if (m_cacheIndex < 15) {
		first = 30 - m_cacheIndex;
	} else {
		first = m_cacheIndex - 15;
	}
	for (int i = first; i < first + 30; ++i) {
		const char* key = m_cacheKeys[i % 30];
		if (key != NULL && strcmp(key, text) == 0) {
			return m_cacheTexts[i % 30];
		}
	}
	TextMap::iterator it = m_texts.find(std::string(text));
	if (it != m_texts.end()) {
		return it->second.c_str();
	}
	return text;
}

// 0xb4e8a0
void JpTextTranslator::Load() {
	memset(m_cacheKeys, 0, sizeof(m_cacheKeys));
	memset(m_cacheTexts, 0, sizeof(m_cacheTexts));
	m_cacheIndex = 0;
	int koreanLength = ReadDataFile("korea.dat", &m_koreanData);
	ReadDataFile("japan.dat", &m_japaneseData);
	int koreanOffset = 0;
	int japaneseOffset = 0;
	while (koreanOffset < koreanLength) {
		const char* korean = m_koreanData + koreanOffset;
		koreanOffset += static_cast<int>(strlen(korean)) + 1;
		const char* japanese = m_japaneseData + japaneseOffset;
		japaneseOffset += static_cast<int>(strlen(japanese)) + 1;
		if (m_texts.find(std::string(korean)) == m_texts.end()) {
			m_texts.insert(TextMap::value_type(korean, japanese));
		} else {
			++s_duplicateTextCount;
		}
	}
}

// 0xb4eb80
const char* Jp_LookupText(const char* key) {
	static JpTextTranslator s_translator;
	return s_translator.Translate(key);
}
