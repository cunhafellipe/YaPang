// Original: client\Refresh\include\RFBrush.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md; the brush classes and image sets
// have RTTI names but no exports)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFBRUSH_H
#define PG_CLIENT_REFRESH_INCLUDE_RFBRUSH_H

#include <string>

#include "RFRenderer.h"
#include "RFSize.h"
#include "RFVector2.h"
#include "defines.h"
#include "padding.h"

class RFTexture;

// None of these classes is exported, and each vtable's slot 0 is a scalar
// deleting destructor (??_G, tests bit 0 only), not the vector deleting
// destructor cl 16 emits for a dllexport class: they are declared without
// __declspec(dllexport).

// The reference-counted base of the image sets (RTTI RFReferenceCount,
// vtable 0xd641d8, one slot). The owners release a reference inline
// (RFPicture::SetPatchPicture 0x41ede0: decrement while positive, delete at
// zero). Its own header comes with the class's unit.
class RFReferenceCount {
	LAYOUT_CHECK_FRIEND;

public:
	RFReferenceCount() : m_refCount(0) {}
	virtual ~RFReferenceCount();  // [0] scalar deleting destructor 0x41e170, ~RFReferenceCount 0x41e140

	// Drops one reference, never below zero, and returns what is left
	// (0x41e150); the owner deletes the object when it reaches zero.
	int Release() {
		if (m_refCount > 0)
			--m_refCount;
		return m_refCount;
	}
	// Takes one reference and returns the count before it (the unreferenced
	// out-of-line copy 0x43a360; RFXmlLoader::GetImageSet 0x43f7a0 inlines it).
	int AddRef() { return m_refCount++; }
	int GetRefCount() const { return m_refCount; }

protected:
	int m_refCount;  // 0x4 (the constructor, inlined into ImageSet's 0x41e1a0, zeroes it)
};
CHECK_SIZE(RFReferenceCount, 0x8);
BEGIN_LAYOUT_CHECK(RFReferenceCount)
	CHECK_FIELD(RFReferenceCount, m_refCount, 0x4);
END_LAYOUT_CHECK

// The kind ImageSet::GetType returns; CreateBrush (0x42a8b0) switches on it.
// The names are ours, after the image set each kind belongs to and the
// brush CreateBrush builds for it.
struct ImageSetTypes {
	enum Type {
		None,       // ImageSet (0x41e1c0): CreateBrush returns 0
		Single,     // SingleImageSet (0x41e260): RFSingleBrush
		Frame,      // FrameImageSet (0x43a5c0): RFFullFrameBrush
		NinePatch,  // NinePatchImageSet (0x41e350): RFPatchBrush
		Paint,      // no image set seen yet: RFPaintBrush
	};
};

// What a control's layout data names to draw with (the window descriptions
// hold ImageSet pointers, e.g. PictureBase +0x17c): an image description
// CreateBrush turns into an RFBrush. RTTI ImageSet, vtable 0xd641e0.
class ImageSet : public RFReferenceCount {
public:
	ImageSet() {}                              // 0x41e1a0
	virtual ~ImageSet();                       // [0] scalar deleting destructor 0x41e1d0, ~ImageSet 0x41e1b0
	virtual ImageSetTypes::Type GetType() const;  // [1] 0x41e1c0
};
CHECK_SIZE(ImageSet, 0x8);

// One texture file (RTTI SingleImageSet, vtable 0xd641ec, size 0x28 from
// RFPicture::SetPicture's operator new 0x41ed00, which builds it inline).
class SingleImageSet : public ImageSet {
public:
	SingleImageSet() : m_tileMode(TileModeTypes::Stretch) {}
	virtual ~SingleImageSet();                    // [0] scalar deleting destructor 0x41e270
	virtual ImageSetTypes::Type GetType() const;  // [1] 0x41e260

	std::wstring m_fileName;              // 0x8 (RFRenderer::CreateTexture's name)
	TileModeTypes::Type m_tileMode;       // 0x24
};
CHECK_SIZE(SingleImageSet, 0x28);
CHECK_FIELD(SingleImageSet, m_fileName, 0x8);
CHECK_FIELD(SingleImageSet, m_tileMode, 0x24);

// One texture split into nine patches by four margins (RTTI
// NinePatchImageSet, vtable 0xd641f8, size 0x34 from
// RFPicture::SetPatchPicture's operator new 0x41ede0).
class NinePatchImageSet : public ImageSet {
public:
	// The margins are left unset (SetPatchPicture writes them next).
	NinePatchImageSet() {}
	virtual ~NinePatchImageSet();                 // [0] scalar deleting destructor 0x41e360
	virtual ImageSetTypes::Type GetType() const;  // [1] 0x41e350

	std::wstring m_fileName;  // 0x8
	// Margins as line coordinates: top and bottom pair with the texture
	// height, left and right with its width (RFPatchBrush::SetPatch).
	float m_top;              // 0x24 (SetPatchPicture's four floats, in order)
	float m_bottom;           // 0x28
	float m_left;             // 0x2c
	float m_right;            // 0x30
};
CHECK_SIZE(NinePatchImageSet, 0x34);
CHECK_FIELD(NinePatchImageSet, m_fileName, 0x8);
CHECK_FIELD(NinePatchImageSet, m_top, 0x24);
CHECK_FIELD(NinePatchImageSet, m_bottom, 0x28);
CHECK_FIELD(NinePatchImageSet, m_left, 0x2c);
CHECK_FIELD(NinePatchImageSet, m_right, 0x30);

// A frame of nine texture files (RTTI FrameImageSet, vtable 0xd65af4; size
// 0x128 from RFFullFrameBrush's constructor, which reads up to +0x124).
class FrameImageSet : public ImageSet {
public:
	// One part of the frame: a texture file and how it is tiled (0x20).
	struct Part {
		Part() : m_tileMode(TileModeTypes::Stretch) {}

		std::wstring m_fileName;         // 0x0
		TileModeTypes::Type m_tileMode;  // 0x1c
	};

	// The constructor (0x43a370) is implicit: nine Part constructors unrolled.
	~FrameImageSet();                             //     0x43a440
	// [0] scalar deleting destructor 0x43a5d0 (calls ~FrameImageSet)
	virtual ImageSetTypes::Type GetType() const;  // [1] 0x43a5c0

	// The corners first, then the edges (RFFullFrameBrush's constructor and
	// Draw 0x45acf0; the XML names BG, EDGE_LT/RT/LB/RB, FRAME_L/R/T/B).
	Part m_center;       // 0x8
	Part m_topLeft;      // 0x28
	Part m_topRight;     // 0x48
	Part m_bottomLeft;   // 0x68
	Part m_bottomRight;  // 0x88
	Part m_left;         // 0xa8
	Part m_right;        // 0xc8
	Part m_top;          // 0xe8
	Part m_bottom;       // 0x108
};
CHECK_SIZE(FrameImageSet::Part, 0x20);
CHECK_FIELD(FrameImageSet::Part, m_tileMode, 0x1c);
CHECK_SIZE(FrameImageSet, 0x128);
CHECK_FIELD(FrameImageSet, m_center, 0x8);
CHECK_FIELD(FrameImageSet, m_topLeft, 0x28);
CHECK_FIELD(FrameImageSet, m_topRight, 0x48);
CHECK_FIELD(FrameImageSet, m_bottomLeft, 0x68);
CHECK_FIELD(FrameImageSet, m_bottomRight, 0x88);
CHECK_FIELD(FrameImageSet, m_left, 0xa8);
CHECK_FIELD(FrameImageSet, m_right, 0xc8);
CHECK_FIELD(FrameImageSet, m_top, 0xe8);
CHECK_FIELD(FrameImageSet, m_bottom, 0x108);

// What a control draws an image with (RTTI RFBrush, vtable 0xd64794, the
// draw and size slots pure). Built by CreateBrush from an ImageSet, deleted
// by the controls' OnDestroy through slot 0.
class RFBrush {
public:
	RFBrush() {}           //     0x42a6b0
	virtual ~RFBrush() {}  // [0] scalar deleting destructor 0x42a6d0, ~RFBrush 0x42a6c0
	// [1] Draws at pos with size; alpha (0-1) becomes the colour's alpha
	// byte (RFPaintBrush 0x45aa00); depth goes to the renderer unchanged.
	virtual void Draw(RFVector2 const& pos, RFSize const& size, float alpha, float depth) = 0;
	// [2] The size of the brush's texture; 0x0 for the brushes without one.
	virtual RFSize GetSize() = 0;
};
CHECK_SIZE(RFBrush, 0x4);

// The brush that draws nothing (RTTI RFNullBrush, vtable 0xd647a4, size 0x4
// from CreateBrush's default case).
class RFNullBrush : public RFBrush {
public:
	RFNullBrush() {}                                                           //     0x42a700
	virtual ~RFNullBrush() {}                                                  // [0] scalar deleting destructor 0x42a750, ~RFNullBrush 0x42a710
	virtual void Draw(RFVector2 const& pos, RFSize const& size, float alpha, float depth) {}  // [1] 0x42a720
	virtual RFSize GetSize() { return RFSize(); }                              // [2] 0x42a730
};
CHECK_SIZE(RFNullBrush, 0x4);

// The brush that fills its rectangle with fixed colours, an outline inset by
// one pixel (RTTI RFPaintBrush, vtable 0xd647b4, size 0x4 from CreateBrush).
class RFPaintBrush : public RFBrush {
public:
	RFPaintBrush() {}                                                          //     0x42a780
	virtual ~RFPaintBrush() {}                                                 // [0] scalar deleting destructor 0x42a7c0, ~RFPaintBrush 0x42a790
	virtual void Draw(RFVector2 const& pos, RFSize const& size, float alpha, float depth);  // [1] 0x45aa00
	virtual RFSize GetSize() { return RFSize(); }                              // [2] 0x42a7a0
};
CHECK_SIZE(RFPaintBrush, 0x4);

// The brush of a SingleImageSet: one texture (RTTI RFSingleBrush, vtable
// 0xd65e4c, size 0xc from CreateBrush).
class RFSingleBrush : public RFBrush {
	LAYOUT_CHECK_FRIEND;

public:
	explicit RFSingleBrush(SingleImageSet const& imageSet);                    //     0x45a7f0
	virtual ~RFSingleBrush();                                                  // [0] scalar deleting destructor 0x45a860, ~RFSingleBrush 0x45a8e0
	virtual void Draw(RFVector2 const& pos, RFSize const& size, float alpha, float depth);  // [1] 0x45a950
	virtual RFSize GetSize();                                                  // [2] 0x45a990

private:
	RFTexture* m_texture;            // 0x4 (RFRenderer::CreateTexture, DeleteTexture in the destructor)
	TileModeTypes::Type m_tileMode;  // 0x8
};
CHECK_SIZE(RFSingleBrush, 0xc);
BEGIN_LAYOUT_CHECK(RFSingleBrush)
	CHECK_FIELD(RFSingleBrush, m_texture, 0x4);
	CHECK_FIELD(RFSingleBrush, m_tileMode, 0x8);
END_LAYOUT_CHECK

// The brush of a FrameImageSet: nine textures (RTTI RFFullFrameBrush, vtable
// 0xd65e5c, size 0x3c from CreateBrush).
class RFFullFrameBrush : public RFBrush {
	LAYOUT_CHECK_FRIEND;

public:
	explicit RFFullFrameBrush(FrameImageSet const& imageSet);                  //     0x45aaf0
	virtual ~RFFullFrameBrush();                                               // [0] scalar deleting destructor 0x45ac40, ~RFFullFrameBrush 0x45ac70
	virtual void Draw(RFVector2 const& pos, RFSize const& size, float alpha, float depth);  // [1] 0x45acf0
	virtual RFSize GetSize();                                                  // [2] 0x45aad0

private:
	// From the image set's parts 0, 1, 3, 2, 4, 7, 8, 5, 6 in that order.
	RFTexture* m_textures[9];             // 0x4
	// From the parts 7, 8, 5, 6, 0 in that order.
	TileModeTypes::Type m_tileModes[5];   // 0x28
};
CHECK_SIZE(RFFullFrameBrush, 0x3c);
BEGIN_LAYOUT_CHECK(RFFullFrameBrush)
	CHECK_FIELD(RFFullFrameBrush, m_textures, 0x4);
	CHECK_FIELD(RFFullFrameBrush, m_tileModes, 0x28);
END_LAYOUT_CHECK

// The brush of a NinePatchImageSet: one texture drawn as nine patches (RTTI
// RFPatchBrush, vtable 0xd65e6c, size 0x70 from CreateBrush).
class RFPatchBrush : public RFBrush {
	LAYOUT_CHECK_FRIEND;

public:
	explicit RFPatchBrush(NinePatchImageSet const& imageSet);                  //     0x45b200
	virtual ~RFPatchBrush();                                                   // [0] scalar deleting destructor 0x45b2d0, ~RFPatchBrush 0x45b390
	virtual void Draw(RFVector2 const& pos, RFSize const& size, float alpha, float depth);  // [1] 0x45b400
	virtual RFSize GetSize();                                                  // [2] 0x45b1c0

	// Computes the patches' texture coordinates and sizes from the four
	// margins (top and bottom pair with the height, left and right with the
	// width; the constructor passes NinePatchImageSet's 0x24-0x30 in order).
	void SetPatch(float top, float bottom, float left, float right);          //     0x45b830

private:
	RFTexture* m_texture;     // 0x4
	// All written by SetPatch; the constructor zeroes only the RFSize
	// members (0x10-0x6c).
	float m_texelWidth;       // 0x8 (1 / texture width)
	float m_texelHeight;      // 0xc (1 / texture height)
	// One texel in from each corner (u, v): top-left, top-right,
	// bottom-left, bottom-right.
	RFSize m_cornerUVs[4];    // 0x10
	// The margins as texture coordinates (u, v), same corner order.
	RFSize m_marginUVs[4];    // 0x30
	// The patches' sizes in pixels: top-left, bottom-left, top-right,
	// bottom-right.
	RFSize m_patchSizes[4];   // 0x50
};
CHECK_SIZE(RFPatchBrush, 0x70);
BEGIN_LAYOUT_CHECK(RFPatchBrush)
	CHECK_FIELD(RFPatchBrush, m_texture, 0x4);
	CHECK_FIELD(RFPatchBrush, m_texelWidth, 0x8);
	CHECK_FIELD(RFPatchBrush, m_texelHeight, 0xc);
	CHECK_FIELD(RFPatchBrush, m_cornerUVs, 0x10);
	CHECK_FIELD(RFPatchBrush, m_marginUVs, 0x30);
	CHECK_FIELD(RFPatchBrush, m_patchSizes, 0x50);
END_LAYOUT_CHECK

// Builds the brush an image set describes (0x42a8b0, unexported): a switch
// on imageSet->GetType() (ImageSetTypes), 0 for ImageSetTypes::None, an
// RFNullBrush for a null set or an unknown kind.
RFBrush* CreateBrush(ImageSet* imageSet);

#endif
