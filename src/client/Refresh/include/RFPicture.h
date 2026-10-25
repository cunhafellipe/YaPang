// Original: client\Refresh\include\RFPicture.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFPICTURE_H
#define PG_CLIENT_REFRESH_INCLUDE_RFPICTURE_H

#include <string>

#include "RFBrush.h"
#include "RFDirector.h"
#include "RFExport.h"
#include "RFSize.h"
#include "RFWindowFactory.h"
#include "RFWnd.h"
#include "defines.h"
#include "padding.h"

// The layout data of a picture, read by RFPicture::Create (RTTI
// ".?AUPictureBase@@", vtable 0xd65ab8, emitted in RFXmlLoader; scalar
// deleting destructor 0x43da50 releases m_brushSource): the window's
// WndBaseStruct followed by the picture's fields.
struct PictureBase : public WndBaseStruct {
	// [0] 0x43d9b0 (scalar deleting destructor 0x43da50)
	virtual ~PictureBase() {
		if (m_brushSource) {
			if (m_brushSource->Release() == 0)
				delete m_brushSource;
			m_brushSource = 0;
		}
	}

	// [1] 0x43da40 (the picture's WndType)
	virtual WndType GetType() const { return WndType_Picture; }

	ImageSet* m_brushSource;         // 0x17c
	bool m_usePicture;               // 0x180
};
CHECK_SIZE(PictureBase, 0x184);  // RFXmlLoader::LoadElement 0x440cb0 allocates 0x184
CHECK_FIELD(PictureBase, m_brushSource, 0x17c);
CHECK_FIELD(PictureBase, m_usePicture, 0x180);

// A Refresh picture: an RFWnd that draws an animation (an RFDirector) or a
// picture (a brush); vtable 0xd63c9c, 34 slots, size 0x188 (the factory's
// operator new 0x4331e0). Exported: its implicit copy constructor (0x416bd0)
// and copy assignment (0x416c20) are exports, so they stay implicit.
class RF_API RFPicture : public RFWnd {
	LAYOUT_CHECK_FRIEND;

public:
	// The class name, the static const at 0xe46c80 that GetTypeName returns.
	static std::wstring const TypeName;

	// [0] 0x416ba0, [2] 0x416bb0 and GetDirector 0x416bc0: header inlines
	// (their copies sit in the COMDAT run of the implicit copy constructor,
	// operator= and ??_E)
	virtual wchar_t const* ToString() const {
		return L"RFPicture";
	}
	virtual ~RFPicture();                                     // [1]  vector deleting destructor 0x416ca0, ~RFPicture 0x41e420
	virtual std::wstring const& GetTypeName() {
		return TypeName;
	}
	virtual void Create(WndBaseStruct*, RFWnd*);              // [4]  0x41e430
	virtual void Destroy();                                   // [5]  0x41e4d0

	RFPicture();                                              //      0x41e400
	RFDirector* GetDirector() {
		return m_director;
	}
	void LoadAnimation(std::wstring const&);                  //      0x41e6c0
	void UnloadAnimation();                                   //      0x41e7d0
	R_ERRORTYPE SetAnimation(RFDirector*);                    //      0x41e800
	void SetAnimPos(float, float);                            //      0x41e9d0
	void Play(std::wstring const&);                           //      0x41ea00
	void Stop();                                              //      0x41ea80
	bool IsPlaying() const;                                   //      0x41eaa0
	void SetPicture(std::wstring const&);                     //      0x41ed00
	void SetPatchPicture(std::wstring const&, float, float, float, float);  // 0x41ede0
	RFSize GetTextureSize() const;                            //      0x41eef0
	void FitToContent();                                      //      0x41ef30

protected:
	virtual void LoadResource();                              // [8]  0x41e520
	virtual void PreInitialize();                             // [9]  0x41e470
	virtual void OnDestroy();                                 // [11] 0x41e630
	virtual void OnLayoutChanged(uint32_t);                   // [14] 0x41e660
	virtual void OnUpdate(UpdateEventArgs&);                  // [31] 0x41eac0
	virtual void OnDraw();                                    // [32] 0x41eb20

private:
	void LoadAnimation_(std::wstring const&, std::wstring const&);  // 0x41e900

	RFDirector* m_director;          // 0x178 (GetDirector 0x416bc0)
	bool m_usePicture;               // 0x17c (SetPicture 0x41ed00 sets it; else the director plays)
	RFBrush* m_brush;                // 0x180 (LoadResource 0x41e520)
	ImageSet* m_brushSource;         // 0x184
};
CHECK_SIZE(RFPicture, 0x188);
BEGIN_LAYOUT_CHECK(RFPicture)
	CHECK_FIELD(RFPicture, m_director, 0x178);
	CHECK_FIELD(RFPicture, m_usePicture, 0x17c);
	CHECK_FIELD(RFPicture, m_brush, 0x180);
	CHECK_FIELD(RFPicture, m_brushSource, 0x184);
END_LAYOUT_CHECK

// The window factory of the "Picture" type (vtable of RFPictureFactory,
// slots 0x433260, 0x4331e0, 0x433240).
class RFPictureFactory : public RFWindowFactory {
public:
	RFPictureFactory(); // 0x433190 (RFPicture.cpp, beside Create)
	// [0] scalar deleting destructor 0x433260
	virtual RFWnd* Create();             // [1] 0x4331e0
	virtual void Destroy(RFWnd*);        // [2] 0x433240
};

#endif
