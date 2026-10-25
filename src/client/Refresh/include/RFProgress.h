// Original: client\Refresh\include\RFProgress.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFPROGRESS_H
#define PG_CLIENT_REFRESH_INCLUDE_RFPROGRESS_H

#include <string>

#include "RFBrush.h"
#include "RFExport.h"
#include "RFWindowFactory.h"
#include "RFWnd.h"

#include "defines.h"

// The layout data of a progress bar, read by RFProgress::Create (RTTI
// ".?AUProgressBase@@", vtable 0xd65ac4, emitted in RFXmlLoader; scalar
// deleting destructor 0x43db90 releases m_brushDesc): the window's
// WndBaseStruct followed by the bar's image set.
struct ProgressBase : public WndBaseStruct {
	// [0] 0x43daf0 (scalar deleting destructor 0x43db90)
	virtual ~ProgressBase() {
		if (m_brushDesc) {
			if (m_brushDesc->Release() == 0)
				delete m_brushDesc;
			m_brushDesc = 0;
		}
	}

	// [1] 0x43db80 (the progress bar's WndType)
	virtual WndType GetType() const { return WndType_Progress; }

	ImageSet* m_brushDesc;      // 0x17c
};
CHECK_SIZE(ProgressBase, 0x180);  // RFXmlLoader::LoadElement 0x440cb0 allocates 0x180
CHECK_FIELD(ProgressBase, m_brushDesc, 0x17c);

// A Refresh progress bar: an RFWnd that draws a fill between a range
// (vtable 0xd63d2c, 34 slots; size 0x194, from the factory's new).
// Exported: its implicit copy constructor (0x416d50) and copy assignment
// (0x416dd0) are exports, so they stay implicit.
class RF_API RFProgress : public RFWnd {
	LAYOUT_CHECK_FRIEND;

public:
	// The class name, the static const at 0xe46fc8 that GetTypeName returns.
	static std::wstring const TypeName;

	// [0] 0x416d10, [2] 0x416d20, SetHorizontal 0x416d30
	// and IsHorizontal 0x416d40: header inlines (their copies sit in the
	// COMDAT run of the implicit copy constructor, operator= and ??_E)
	virtual wchar_t const* ToString() const {
		return L"RFProgress";
	}
	virtual ~RFProgress();                                    // [1]  vector deleting destructor 0x416e70, ~RFProgress 0x44d2d0
	virtual std::wstring const& GetTypeName() {
		return TypeName;
	}
	virtual void Create(WndBaseStruct*, RFWnd*);              // [4]  0x44d2e0

	RFProgress();                                             //      0x44d2b0
	void SetHorizontal(bool horizontal) {
		m_horizontal = horizontal;
	}
	bool IsHorizontal() const {
		return m_horizontal;
	}
	void SetRange(int, int);                                  //      0x44d620
	void SetProgress(int);                                    //      0x44d660
	int GetProgress() const;                                  //      0x44d690

protected:
	virtual void LoadResource();                              // [8]  0x44d3a0
	virtual void PreInitialize();                             // [9]  0x44d310
	virtual void OnDestroy();                                 // [11] 0x44d3c0
	virtual void OnDraw();                                    // [32] 0x44d3f0

private:
	bool m_horizontal;          // 0x178
	int m_min;                  // 0x17c
	int m_max;                  // 0x180
	int m_progress;             // 0x184
	bool m_unknown188;          // 0x188 (PreInitialize sets it; use not seen yet)
	RFBrush* m_brush;           // 0x18c
	ImageSet* m_brushDesc;      // 0x190
};
CHECK_SIZE(RFProgress, 0x194);
BEGIN_LAYOUT_CHECK(RFProgress)
	CHECK_FIELD(RFProgress, m_horizontal, 0x178);
	CHECK_FIELD(RFProgress, m_min, 0x17c);
	CHECK_FIELD(RFProgress, m_max, 0x180);
	CHECK_FIELD(RFProgress, m_progress, 0x184);
	CHECK_FIELD(RFProgress, m_brush, 0x18c);
	CHECK_FIELD(RFProgress, m_brushDesc, 0x190);
END_LAYOUT_CHECK

// The factory RFFactoryManager creates progress bars with (vtable 0xd64f84).
class RFProgressFactory : public RFWindowFactory {
public:
	RFProgressFactory(); // 0x4332e0 (RFProgress.cpp, beside Create)
	virtual RFWnd *Create();          // [1] 0x433330
	virtual void Destroy(RFWnd *);    // [2] 0x433390
};

#endif
