// JP 983 CPetFrame / PartsInfo code with no KR counterpart (names ours):
// the UCC clothes dressing of the lobby avatar (LobbyPlayer message 0x18d,
// 0x566b30). The JP moved the KR CPetFrame UCC texture selection (KR
// PartsInfo.cpp, the IsUccClothes block of the part loop) into a helper
// filling a texture set; KR has no function of it. Declarations and the JP
// x86 layout, from the JP exe's bodies (node scripts/disasm.ts --jp 0x<va>).
#ifndef PG_JP_JPPETFRAMEGLUE_H
#define PG_JP_JPPETFRAMEGLUE_H

#include "defines.h"

class CPetFrame;
class WBoneSet;
class WPuppet;
struct sUccClothes;
namespace IFF_STRUCT {
struct sPart;
}

// A part's texture handles (0x18 bytes, a local of 0x8b0960): the textures
// to put on and, at the same index, the original texture each replaces
// (the KR texA / texB of the part loop).
struct JpPartTextures {
	// 0x8ad8e0: Clear().
	JpPartTextures();
	// 0x8acb20: every handle 0.
	void Clear();
	// 0x8acb40 (thiscall, the puppet on the stack): for each index with
	// both handles, bonePet->ChangeTextureByHandle(texture, original, NULL)
	// (JP 0xb6f100, the KR WPuppet::ChangeTextureByHandle).
	void ApplyTo(WPuppet* bonePet) const;
	// 0x8acb80: g_resrcmng->Release (JP 0xb5a790) of each original handle,
	// each set to 0 (with g_resrcmng; the textures are kept).
	void ReleaseOriginals();

	int m_textures[3];   // 0x0
	int m_originals[3];  // 0xc
};
CHECK_SIZE(JpPartTextures, 0x18);

// 0x8b05a0: the textures of the part `part` for the item `id`: with the
// JP local content 0x58 and a UCC clothes part, the UCC front / back
// textures (GenerateTextureName_F / _B) over the part's first two textures,
// from the UCC manager's clothes `id` once downloaded, else from the my-item
// list's item of the same type and UCC key (GetMyItemInfo(id)) whose UCC
// status bit 0 is clear; otherwise the part's own texture pairs (0x8acbc0).
void Jp_LoadUccPartTextures(const IFF_STRUCT::sPart* part, unsigned long id, JpPartTextures* textures);

// 0x8b0960 (JP CPetFrame, thiscall): dresses the frame in a UCC clothes
// set: the textures of the clothes' part (CItemManager::FindPart of its
// typeid) for its serial, put on the bone puppet, then the originals
// released. The JP returns whatever eax 0x8acb80 leaves: void here.
void Jp_PetFrame_SetUccClothes(CPetFrame* frame, sUccClothes* clothes);

// 0x8acdf0 (JP CPetFrame, thiscall, ret 0xc): poses the frame at `time`
// of the motion `motionName` (NULL: the current animation puppet's
// timeline). The JP current animation puppet (+0x24) fills a bone set
// (0xb86c10, the JP rewrite of WPuppet::GetBones writing into a caller's
// set, the bone puppet +0x4 as the 6th argument) — `bones` when given,
// else the frame's own set (+0x28), which the bone puppet then applies
// (0xb7b0a0, WPuppet::ApplyBones(WBoneSet*)). The KR frame has no set of
// its own: the KR GetBones' set is copied into `bones` or applied.
void Jp_PetFrame_Animate(CPetFrame* frame, WBoneSet* bones, const char* motionName, float time);

#endif // PG_JP_JPPETFRAMEGLUE_H
