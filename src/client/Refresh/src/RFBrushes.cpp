// Original: client\Refresh\src\RFBrushes.cpp (JP 983 exe, unit bounds in
// jp/units.tsv; no assert path names the file: the out-of-line members of
// the brush classes of include/RFBrush.h)
#include <new>

#include "RFBrush.h"
#include "RFManager.h"
#include "RFTexture.h"

// lib: 0x45b360 (`vector constructor iterator', the copy this object kept)
// 0x45b350 is RFSize::RFSize() (include/RFSize.h), the inline copy this object
// kept (paired). The scalar deleting destructors of RFSingleBrush 0x45a860,
// RFFullFrameBrush 0x45ac40 and RFPatchBrush 0x45b2d0.

// 0x45a7f0
RFSingleBrush::RFSingleBrush(SingleImageSet const& imageSet) : m_texture(0) {
	m_texture = RFManager::s_renderer->CreateTexture(imageSet.m_fileName);
	m_tileMode = imageSet.m_tileMode;
}

// 0x45a8e0
RFSingleBrush::~RFSingleBrush() {
	if (m_texture != 0)
		RFManager::s_renderer->DeleteTexture(m_texture);
}

// 0x45a950
void RFSingleBrush::Draw(RFVector2 const& pos, RFSize const& size, float alpha, float depth) {
	RFManager::s_renderer->Draw(m_texture, pos, size, alpha, depth, m_tileMode);
}

// 0x45a990
RFSize RFSingleBrush::GetSize() {
	if (m_texture != 0)
		return RFSize((float)m_texture->GetWidth(), (float)m_texture->GetHeight());
	return RFSize();
}

// 0x45aa00
void RFPaintBrush::Draw(RFVector2 const& pos, RFSize const& size, float alpha, float depth) {
	RFManager::s_renderer->FillRectangle((uint32_t)(alpha * 255.0f) << 24, pos.x, pos.y, size.width, size.height, depth);
	RFManager::s_renderer->FillRectangle(((uint32_t)(alpha * 255.0f) << 24) | 0x225982, pos.x + 1.0f, pos.y + 1.0f,
		size.width - 2.0f, size.height - 2.0f, depth);
}

// 0x45aad0
RFSize RFFullFrameBrush::GetSize() {
	return RFSize();
}

// 0x45aaf0
RFFullFrameBrush::RFFullFrameBrush(FrameImageSet const& imageSet) : m_textures() {
	m_textures[1] = RFManager::s_renderer->CreateTexture(imageSet.m_topLeft.m_fileName);
	m_textures[5] = RFManager::s_renderer->CreateTexture(imageSet.m_top.m_fileName);
	m_textures[3] = RFManager::s_renderer->CreateTexture(imageSet.m_topRight.m_fileName);
	m_textures[7] = RFManager::s_renderer->CreateTexture(imageSet.m_left.m_fileName);
	m_textures[0] = RFManager::s_renderer->CreateTexture(imageSet.m_center.m_fileName);
	m_textures[8] = RFManager::s_renderer->CreateTexture(imageSet.m_right.m_fileName);
	m_textures[2] = RFManager::s_renderer->CreateTexture(imageSet.m_bottomLeft.m_fileName);
	m_textures[6] = RFManager::s_renderer->CreateTexture(imageSet.m_bottom.m_fileName);
	m_textures[4] = RFManager::s_renderer->CreateTexture(imageSet.m_bottomRight.m_fileName);
	m_tileModes[0] = imageSet.m_top.m_tileMode;
	m_tileModes[1] = imageSet.m_bottom.m_tileMode;
	m_tileModes[2] = imageSet.m_left.m_tileMode;
	m_tileModes[3] = imageSet.m_right.m_tileMode;
	m_tileModes[4] = imageSet.m_center.m_tileMode;
}

// 0x45ac70
RFFullFrameBrush::~RFFullFrameBrush() {
	for (int i = 0; i < 9; ++i) {
		if (m_textures[i] != 0) {
			RFManager::s_renderer->DeleteTexture(m_textures[i]);
			m_textures[i] = 0;
		}
	}
}

// 0x45acf0
void RFFullFrameBrush::Draw(RFVector2 const& pos, RFSize const& size, float alpha, float depth) {
	RFSize patch;
	RFVector2 at = pos;
	// The top row: the corners at their texture's size, the edge between
	// stretched or tiled to fill the width.
	if (m_textures[1] != 0) {
		patch.width = (float)m_textures[1]->GetWidth();
		patch.height = (float)m_textures[1]->GetHeight();
		RFManager::s_renderer->Draw(m_textures[1], at, patch, alpha, depth, TileModeTypes::Stretch);
	}
	if (m_textures[5] != 0) {
		at.x += patch.width;
		patch.width = size.width - patch.width - (float)m_textures[3]->GetWidth();
		patch.height = (float)m_textures[5]->GetHeight();
		RFManager::s_renderer->Draw(m_textures[5], at, patch, alpha, depth, m_tileModes[0]);
	}
	if (m_textures[3] != 0) {
		at.x += patch.width;
		patch.width = (float)m_textures[3]->GetWidth();
		patch.height = (float)m_textures[3]->GetHeight();
		RFManager::s_renderer->Draw(m_textures[3], at, patch, alpha, depth, TileModeTypes::Stretch);
	}
	// The middle row.
	if (m_textures[7] != 0) {
		at.x = pos.x;
		at.y += patch.height;
		patch.width = (float)m_textures[7]->GetWidth();
		patch.height = size.height - (float)m_textures[1]->GetHeight() - (float)m_textures[2]->GetHeight();
		RFManager::s_renderer->Draw(m_textures[7], at, patch, alpha, depth, m_tileModes[3]);
	}
	if (m_textures[0] != 0) {
		at.x += patch.width;
		patch.width = size.width - patch.width - (float)m_textures[8]->GetWidth();
		RFManager::s_renderer->Draw(m_textures[0], at, patch, alpha, depth, m_tileModes[4]);
	}
	if (m_textures[8] != 0) {
		at.x += patch.width;
		patch.width = (float)m_textures[8]->GetWidth();
		RFManager::s_renderer->Draw(m_textures[8], at, patch, alpha, depth, m_tileModes[3]);
	}
	// The bottom row.
	if (m_textures[2] != 0) {
		at.x = pos.x;
		at.y += patch.height;
		patch.width = (float)m_textures[2]->GetWidth();
		patch.height = (float)m_textures[2]->GetHeight();
		RFManager::s_renderer->Draw(m_textures[2], at, patch, alpha, depth, TileModeTypes::Stretch);
	}
	if (m_textures[6] != 0) {
		at.x += patch.width;
		patch.width = size.width - patch.width - (float)m_textures[4]->GetWidth();
		RFManager::s_renderer->Draw(m_textures[6], at, patch, alpha, depth, m_tileModes[1]);
	}
	if (m_textures[4] != 0) {
		at.x += patch.width;
		patch.width = (float)m_textures[4]->GetWidth();
		RFManager::s_renderer->Draw(m_textures[4], at, patch, alpha, depth, TileModeTypes::Stretch);
	}
}

// 0x45b1c0
RFSize RFPatchBrush::GetSize() {
	return RFSize();
}

// 0x45b200
RFPatchBrush::RFPatchBrush(NinePatchImageSet const& imageSet) {
	m_texture = RFManager::s_renderer->CreateTexture(imageSet.m_fileName);
	SetPatch(imageSet.m_top, imageSet.m_bottom, imageSet.m_left, imageSet.m_right);
}

// 0x45b390
RFPatchBrush::~RFPatchBrush() {
	if (m_texture != 0)
		RFManager::s_renderer->DeleteTexture(m_texture);
}

// 0x45b400
void RFPatchBrush::Draw(RFVector2 const& pos, RFSize const& size, float alpha, float depth) {
	RFVector2 at = pos;
	RFSize patch(m_patchSizes[0].width, m_patchSizes[0].height);
	// The top row: the corners keep their size, the middle stretches.
	RFManager::s_renderer->Draw(m_texture, m_cornerUVs[0].width, m_cornerUVs[0].height,
		m_marginUVs[0].width, m_marginUVs[0].height, at, patch, alpha, depth, TileModeTypes::Stretch);
	at.x += patch.width;
	patch.width = size.width - patch.width - m_patchSizes[2].width;
	patch.height = m_patchSizes[0].height;
	RFManager::s_renderer->Draw(m_texture, m_marginUVs[0].width, m_cornerUVs[0].height,
		m_marginUVs[1].width, m_marginUVs[1].height, at, patch, alpha, depth, TileModeTypes::Stretch);
	at.x += patch.width;
	patch.width = m_patchSizes[2].width;
	patch.height = m_patchSizes[2].height;
	RFManager::s_renderer->Draw(m_texture, m_marginUVs[1].width, m_cornerUVs[1].height,
		m_cornerUVs[1].width, m_marginUVs[1].height, at, patch, alpha, depth, TileModeTypes::Stretch);
	// The middle row.
	at.x = pos.x;
	at.y += patch.height;
	patch.width = m_patchSizes[0].width;
	patch.height = size.height - m_patchSizes[0].height - m_patchSizes[1].height;
	RFManager::s_renderer->Draw(m_texture, m_cornerUVs[0].width, m_marginUVs[0].height,
		m_marginUVs[2].width, m_marginUVs[2].height, at, patch, alpha, depth, TileModeTypes::Stretch);
	at.x += patch.width;
	patch.width = size.width - patch.width - m_patchSizes[2].width;
	RFManager::s_renderer->Draw(m_texture, m_texelWidth + m_marginUVs[0].width,
		m_marginUVs[0].height + m_texelHeight, m_marginUVs[3].width - m_texelWidth,
		m_marginUVs[3].height - m_texelHeight, at, patch, alpha, depth, TileModeTypes::Stretch);
	at.x += patch.width;
	patch.width = m_patchSizes[2].width;
	RFManager::s_renderer->Draw(m_texture, m_marginUVs[1].width, m_marginUVs[1].height,
		m_cornerUVs[1].width, m_marginUVs[3].height, at, patch, alpha, depth, TileModeTypes::Stretch);
	// The bottom row.
	at.x = pos.x;
	at.y += patch.height;
	patch.width = m_patchSizes[1].width;
	patch.height = m_patchSizes[1].height;
	RFManager::s_renderer->Draw(m_texture, m_cornerUVs[2].width, m_marginUVs[2].height,
		m_marginUVs[2].width, m_cornerUVs[2].height, at, patch, alpha, depth, TileModeTypes::Stretch);
	at.x += patch.width;
	patch.width = size.width - patch.width - m_patchSizes[3].width;
	RFManager::s_renderer->Draw(m_texture, m_marginUVs[2].width, m_marginUVs[2].height,
		m_marginUVs[3].width, m_cornerUVs[3].height, at, patch, alpha, depth, TileModeTypes::Stretch);
	at.x += patch.width;
	patch.width = m_patchSizes[3].width;
	RFManager::s_renderer->Draw(m_texture, m_marginUVs[3].width, m_marginUVs[3].height,
		m_cornerUVs[3].width, m_cornerUVs[3].height, at, patch, alpha, depth, TileModeTypes::Stretch);
}

// 0x45b830
void RFPatchBrush::SetPatch(float top, float bottom, float left, float right) {
	if (m_texture == 0)
		return;
	float width = (float)m_texture->GetWidth();
	float height = (float)m_texture->GetHeight();
	float texelWidth = 1.0f / width;
	m_texelWidth = texelWidth;
	float texelHeight = 1.0f / height;
	m_texelHeight = texelHeight;
	m_cornerUVs[0].height = texelHeight;
	m_cornerUVs[0].width = texelWidth;
	float innerRight = (width - 1.0f) / width;
	m_cornerUVs[1].width = innerRight;
	m_cornerUVs[1].height = texelHeight;
	m_cornerUVs[2].width = texelWidth;
	float innerBottom = (height - 1.0f) / height;
	m_cornerUVs[2].height = innerBottom;
	m_cornerUVs[3].height = innerBottom;
	m_cornerUVs[3].width = innerRight;
	float leftU = left / width;
	m_marginUVs[0].width = leftU;
	float topV = top / height;
	m_marginUVs[0].height = topV;
	float rightU = right / width;
	m_marginUVs[1].width = rightU;
	m_marginUVs[1].height = topV;
	m_marginUVs[2].width = leftU;
	float bottomV = bottom / height;
	m_marginUVs[2].height = bottomV;
	m_marginUVs[3].height = bottomV;
	m_marginUVs[3].width = rightU;
	float leftWidth = left - 1.0f;
	m_patchSizes[0].width = leftWidth;
	float topHeight = top - 1.0f;
	m_patchSizes[0].height = topHeight;
	m_patchSizes[1].width = leftWidth;
	float bottomHeight = height - bottom - 1.0f;
	m_patchSizes[1].height = bottomHeight;
	float rightWidth = width - right - 1.0f;
	m_patchSizes[2].width = rightWidth;
	m_patchSizes[3].width = rightWidth;
	m_patchSizes[2].height = topHeight;
	m_patchSizes[3].height = bottomHeight;
}
