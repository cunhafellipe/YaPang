// The JP 983 game's RFTextureManager (RFTextureManager.h), from the JP exe's
// bodies (node scripts/jp_hlil.ts).
#include "RFTextureManager.h"

#include <string>

#include "woverlay.h"
#include "wresrcmng.h"

// 0xe78c30
template <> RFTextureManager* WSingleton<RFTextureManager>::m_pInstance = NULL;

// 0x600a70: the image type of a file name, from its extension; pos receives
// the extension's position (npos when neither is found).
static int32_t GetImageType(const std::string& name, std::string::size_type* pos) {
	int32_t type = 0;
	std::string copy(name, 0, std::string::npos);
	*pos = copy.find(".png", 0, 4);
	if (*pos != std::string::npos) {
		type = 2;
	} else {
		*pos = copy.find(".tga", 0, 4);
		if (*pos != std::string::npos) {
			type = 1;
		}
	}
	return type;
}

// 0x601dc0
RFTextureManager::RFTextureManager() {
}

// 0x601ae0 (scalar deleting destructor 0x601e80)
RFTextureManager::~RFTextureManager() {
	for (std::map<std::string, RFTextureEntry>::iterator it = m_textures.begin(); it != m_textures.end(); ++it) {
		if (it->second.m_texture != NULL) {
			delete it->second.m_texture;
			it->second.m_texture = NULL;
		}
		if (g_resrcmng != NULL && it->second.m_resource != NULL) {
			// 0xb55a50: the resource manager's release of a resource (it
			// deletes it through slot 0).
			g_resrcmng->Release(it->second.m_resource);
			it->second.m_resource = NULL;
		}
	}
	m_textures.clear();
}

// 0x6020d0
WOverlay* RFTextureManager::LoadTexture(const std::string& name) {
	if (g_resrcmng == NULL) {
		return NULL;
	}
	std::string::size_type pos = std::string::npos;
	int32_t type = GetImageType(std::string(name.c_str()), &pos);
	std::string file(name.c_str());
	// 0xb55ab0 with the type 0x80000000.
	WOverlay* overlay = g_resrcmng->GetOverlay(file.c_str(), 0x80000000);
	RFTextureEntry entry;
	entry.m_name.assign(name, 0, std::string::npos);
	entry.m_texture = NULL;
	RFTextureEntry& slot = m_textures[std::string(name.c_str())];
	slot.m_imageType = type;
	slot.m_name.assign(entry.m_name, 0, std::string::npos);
	slot.m_texture = entry.m_texture;
	slot.m_resource = overlay;
	return overlay;
}

// 0x6022f0
WOverlay* RFTextureManager::GetTexture(const std::string& name) {
	std::map<std::string, RFTextureEntry>::iterator it = m_textures.find(std::string(name.c_str()));
	if (it == m_textures.end()) {
		return LoadTexture(std::string(name.c_str()));
	}
	return it->second.m_resource;
}
