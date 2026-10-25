// The JP 983 game's RFModalRenderer (RFFormModalBase.h), from the JP exe's
// bodies (node scripts/disasm.ts --jp 0x<va>).
// lib: 0x5e5400 (scalar deleting destructor), 0xcd1fc0 (s_modals' dynamic
// initializer), 0x5e5850 (std::list node allocation)
#include "RFFormModalBase.h"

#include <stdint.h>
#include <algorithm>

#include "RFManager.h"
#include "RFSceneManager.h"
#include "RFSize.h"
#include "RFVector2.h"
#include "RFWnd.h"
#include "RefreshRenderer.h"

// 0xe78bf8
std::list<RFModalRenderer*> RFModalRenderer::s_modals;

// 0x5e51f0
RFModalRenderer::RFModalRenderer() {
}

// 0x5e5200
RFModalRenderer::~RFModalRenderer() {
}

// 0x5e58f0: the alpha byte is (min(count, 6) * 10 + 100) * alpha, alpha
// taken as 1 when several modals are open; the area is the root window's
// (800 x 600, 0xd0562c / 0xd05628, without a manager).
void RFModalRenderer::DrawModalBackground(float alpha) {
	if (s_modals.size() == 0 || RFSceneManager::Instance() == NULL || s_modals.back() != this) {
		return;
	}
	RFSize size;
	RFManager* manager = RFSceneManager::Instance()->GetManager();
	if (manager != NULL) {
		size = manager->GetRootWnd()->GetSize();
	} else {
		size = RFSize(800.0f, 600.0f);
	}
	// 6.0f (0xd08c08)
	float count = s_modals.size() < 7 ? (float)s_modals.size() : 6.0f;
	if (s_modals.size() > 1) {
		alpha = 1.0f;
	}
	// 10.0 (0xcff068), 100.0 (0xd66ca8)
	alpha = (float)((count * 10.0 + 100.0) * alpha);
	FillBox(RFVector2(0.0f, 0.0f), size, (uint32_t)(int64_t)alpha << 24);
}

// 0x5e5ab0
void RFModalRenderer::RemoveModal() {
	std::list<RFModalRenderer*>::iterator it = std::find(s_modals.begin(), s_modals.end(), this);
	if (it != s_modals.end()) {
		s_modals.erase(it);
	}
}

// 0x5e5b70
void RFModalRenderer::AddModal() {
	s_modals.push_back(this);
}
