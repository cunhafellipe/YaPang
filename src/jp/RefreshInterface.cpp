// The JP 983 game's RFInterface (RefreshInterface.h), from the JP exe's
// bodies (node scripts/jp_hlil.ts on the vtable 0xd13004 slots).
#include "RefreshInterface.h"

#include <string>

#include "RFLabel.h"
#include "RFTextDraw.h"
#include "RFTextManager.h"
#include "RefreshStringUtil.h"
#include "SoundManager.h"
#include "westpak.h"
#include "wresrcmng.h"

// 0x5f4f50
RefreshInterface::RefreshInterface() {
}

// 0x5f51b0
void RefreshInterface::PlaySfx(const std::wstring& name) {
	if (g_audio != NULL && name.size() != 0) {
		std::string sound = WStringToShiftJis(std::wstring(name.c_str()));
		std::string::size_type pos = sound.find(".wav", 0, 4);
		if (pos != std::string::npos) {
			// The original assigns the range [begin, begin + pos) (0x4d3f80).
			sound.erase(pos);
		}
		// Constants 0xd66d08 (0.5f) and 0xd00d90 (200.0f).
		g_audio->PlaySfx(sound.c_str(), NULL, 0, NULL, NULL, 0.5f, 200.0f);
	}
}

// 0x5f4f80
void RefreshInterface::PrintText(const FontInfo& font, int32_t x, int32_t y, const std::wstring& text, float alpha) {
	if (text.size() == 0) {
		return;
	}
	RFTextManager* textManager = RFTextManager::Instance();
	if (textManager == NULL) {
		return;
	}
	// 0xd66cb8: the double 1.0.
	if (font.size <= 11) {
		textManager->DrawTextSmall((float)(x - 1.0), (float)y, text, font.style, font.color, font.outlineColor, font.shadowColor);
		return;
	}
	textManager->DrawTextLarge((float)(x - 1.0), (float)y, text, font.style, font.color, font.outlineColor, font.shadowColor);
}

// 0x5f5090
RFSize RefreshInterface::MeasureText(const FontInfo& font, const std::wstring& text) {
	RFTextManager* textManager = RFTextManager::Instance();
	if (textManager == NULL) {
		return RFSize(0.0f, 0.0f);
	}
	RFTextDraw* textDraw = textManager->GetTextDraw();
	if (textDraw != NULL) {
		return textDraw->MeasureText(&font, text);
	}
	return RFSize(0.0f, 0.0f);
}

// 0x5f4f60
void RefreshInterface::OnButtonStateChanged(RFWnd* button, int32_t state) {
}

// 0x5f5170
std::wstring RefreshInterface::GetBasePath(RFBASEPATHTYPE type) {
	// 0xe13088: an empty wide string; the paks are read by bare file name.
	return std::wstring(L"");
}

// 0x5f50e0
void* RefreshInterface::FOpen(const std::wstring& name) {
	std::string file = WStringToShiftJis(name);
	return g_resrcmng->GetCFile(file.c_str(), 0xffff);
}

// 0x5f4ea0
void RefreshInterface::FClose(void* file) {
	CloseCFile(static_cast<cFile*>(file));
}

// 0x5f4ec0
int32_t RefreshInterface::FTell(void* file) {
	return static_cast<cFile*>(file)->Tell();
}

// 0x5f4ee0
int32_t RefreshInterface::FSeek(void* file, int32_t offset, int32_t origin) {
	static_cast<cFile*>(file)->Seek(offset, origin);
	return 0;
}

// 0x5f4f00
int32_t RefreshInterface::FRead(void* file, void* buffer, int32_t size) {
	static_cast<cFile*>(file)->Read(buffer, size);
	return 0;
}

// 0x5f4f70 (scalar deleting destructor 0x5f5060)
RefreshInterface::~RefreshInterface() {
}
