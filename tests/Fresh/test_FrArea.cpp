// Unit tests for client/Fresh/source/FrArea.cpp.
//
// The real unit is compiled by the host test target; the Fresh/Wangreal
// machinery it links against (FrWnd, the window manager, GDI, the document and
// Bitmap) is provided as inert stand-ins in FrArea_standins.cpp. See tests/
// README.md.
#include <cstdint>
#include <cstring>

#include "catch.hpp"
#include "frarea.h"
#include "client/Wangreal/include/bitmap.h"

// Recording globals defined in FrArea_standins.cpp.
namespace testrec {
extern eFrCmd cmd;
extern int cmdCount;
extern int cursor;
extern bool drew;
extern const Bitmap* drawBitmap;
extern WRect drawSrc;
extern WRect drawDst;
extern unsigned long drawDiffuse;
} // namespace testrec

namespace {

// Exposes the protected surface of FrArea so the fixture can drive it the way
// the real window manager and paint loop do.
class TestFrArea : public FrArea {
public:
	using FrArea::ClippingArea;
	using FrArea::OnDraw;
	using FrArea::OnLButtonDown;
	using FrArea::OnLButtonUp;
	using FrArea::OnSetCursor;

	void SetRect(float x, float y, float w, float h) {
		m_rect.x = x;
		m_rect.y = y;
		m_rect.w = w;
		m_rect.h = h;
	}
	void SetBitmap(const Bitmap* pBitmap) { m_pBitmap = pBitmap; }
	void SetCenter(bool center) { m_center = center; }
	void SetStretch(bool stretch) { m_stretch = stretch; }
	void SetImgName(const char* name) { m_imgName = name; }
	void SetClip(bool clipping, float x, float y, float w, float h) {
		m_isClipping = clipping;
		m_rcClipArea.x = x;
		m_rcClipArea.y = y;
		m_rcClipArea.w = w;
		m_rcClipArea.h = h;
	}

	unsigned long Color() const { return m_color; }
	bool Center() const { return m_center; }
	bool Stretch() const { return m_stretch; }
	bool MouseEvent() const { return m_mouseEvent; }
	bool IsClipping() const { return m_isClipping; }
	const std::string& ImgName() const { return m_imgName; }
	const WRect& ClipArea() const { return m_rcClipArea; }
};

// A Bitmap whose Width()/Height() read a BITMAPINFO that lives next to it.
struct FakeBitmap {
	tagBITMAPINFO info;
	Bitmap bm;

	FakeBitmap(int width, int height) {
		std::memset(&info, 0, sizeof(info));
		info.bmiHeader.biWidth = width;
		info.bmiHeader.biHeight = height;
		bm.bi = &info;
	}
};

bool RectIs(const WRect& rc, float x, float y, float w, float h) {
	return rc.x == x && rc.y == y && rc.w == w && rc.h == h;
}

} // namespace

TEST_CASE("FrArea constructor installs its defaults", "[Fresh][FrArea]")
{
	TestFrArea a;

	REQUIRE(a.GetBgImg() == nullptr);
	REQUIRE(a.Color() == 0xffffffffu);
	REQUIRE_FALSE(a.Center());
	REQUIRE_FALSE(a.Stretch());
	REQUIRE_FALSE(a.MouseEvent());
	REQUIRE_FALSE(a.IsClipping());
	REQUIRE(a.ImgName().empty());
	REQUIRE(RectIs(a.ClipArea(), 0.0f, 0.0f, 0.0f, 0.0f));
}

TEST_CASE("FrArea::SetBgImg(const Bitmap*) stores the bitmap and never clears it with null", "[Fresh][FrArea]")
{
	TestFrArea a;
	FakeBitmap fb(8, 4);

	REQUIRE_FALSE(a.SetBgImg((const Bitmap*)0));
	REQUIRE(a.GetBgImg() == nullptr);

	REQUIRE(a.SetBgImg(&fb.bm));
	REQUIRE(a.GetBgImg() == &fb.bm);

	// A null argument is rejected, so the current bitmap stays.
	REQUIRE_FALSE(a.SetBgImg((const Bitmap*)0));
	REQUIRE(a.GetBgImg() == &fb.bm);
}

TEST_CASE("FrArea::SetBgImg(const char*) clears on null and short-circuits an unchanged name", "[Fresh][FrArea]")
{
	TestFrArea a;

	// Null name clears the background and reports success.
	REQUIRE(a.SetBgImg((const char*)0));
	REQUIRE(a.GetBgImg() == nullptr);

	// The name comparison is case-insensitive (strcmpi), so a matching name
	// returns early -- before it would ask the (absent) document for a bitmap.
	a.SetImgName("background");
	REQUIRE(a.SetBgImg("BACKGROUND"));
	REQUIRE(a.GetBgImg() == nullptr);
}

TEST_CASE("FrArea::SetMouseEvent toggles the no-mouse-event style bit", "[Fresh][FrArea]")
{
	TestFrArea a;

	a.SetMouseEvent(true);
	REQUIRE(a.MouseEvent());
	REQUIRE((a.GetStyle().Get() & 0x40) == 0);

	a.SetMouseEvent(false);
	REQUIRE_FALSE(a.MouseEvent());
	REQUIRE((a.GetStyle().Get() & 0x40) == 0x40);
}

TEST_CASE("FrArea::ClippingArea lays the bitmap out over the window rect", "[Fresh][FrArea]")
{
	TestFrArea a;
	FakeBitmap fb(40, 20);
	a.SetRect(10, 20, 100, 60);

	SECTION("no bitmap means nothing to draw")
	{
		WRect src, dst;
		REQUIRE_FALSE(a.ClippingArea(src, dst));
	}

	SECTION("the source is the whole bitmap and the destination keeps the bitmap size")
	{
		a.SetBitmap(&fb.bm);
		WRect src, dst;
		REQUIRE(a.ClippingArea(src, dst));
		REQUIRE(RectIs(src, 0, 0, 40, 20));
		REQUIRE(RectIs(dst, 10, 20, 40, 20));
	}

	SECTION("stretch makes the destination the whole window rect")
	{
		a.SetBitmap(&fb.bm);
		a.SetStretch(true);
		WRect src, dst;
		REQUIRE(a.ClippingArea(src, dst));
		REQUIRE(RectIs(src, 0, 0, 40, 20));
		REQUIRE(RectIs(dst, 10, 20, 100, 60));
	}

	SECTION("center centers the bitmap inside the window rect")
	{
		a.SetBitmap(&fb.bm);
		a.SetCenter(true);
		WRect src, dst;
		REQUIRE(a.ClippingArea(src, dst));
		REQUIRE(RectIs(src, 0, 0, 40, 20));
		REQUIRE(RectIs(dst, 40, 40, 40, 20));
	}
}

TEST_CASE("FrArea::ClippingArea intersects a partial clip area", "[Fresh][FrArea]")
{
	TestFrArea a;
	FakeBitmap fb(40, 20);
	a.SetBitmap(&fb.bm);
	a.SetRect(10, 20, 100, 60);

	SECTION("a clip area that fully covers the destination leaves it unchanged")
	{
		a.SetClip(true, 0, 0, 200, 200);
		WRect src, dst;
		REQUIRE(a.ClippingArea(src, dst));
		REQUIRE(RectIs(src, 0, 0, 40, 20));
		REQUIRE(RectIs(dst, 10, 20, 40, 20));
	}

	SECTION("a partial clip shrinks the destination and scales the source")
	{
		a.SetClip(true, 20, 25, 20, 10);
		WRect src, dst;
		REQUIRE(a.ClippingArea(src, dst));
		REQUIRE(RectIs(dst, 20, 25, 20, 10));
		REQUIRE(RectIs(src, 10, 5, 20, 10));
	}

	SECTION("a clip area outside the destination collapses it")
	{
		a.SetClip(true, 100, 100, 10, 10);
		WRect src, dst;
		REQUIRE(a.ClippingArea(src, dst));
		REQUIRE(RectIs(dst, 100, 100, 0, 0));
		REQUIRE(RectIs(src, 90, 80, 0, 0));
	}
}

TEST_CASE("FrArea::OnDraw blits the clipped background then notifies the owner", "[Fresh][FrArea]")
{
	TestFrArea a;
	FakeBitmap fb(40, 20);
	a.SetRect(10, 20, 100, 60);

	SECTION("a background is drawn with the blended alpha and the owner draw command")
	{
		a.SetBitmap(&fb.bm);
		testrec::drew = false;
		testrec::cmd = FRCMD_NONE;
		testrec::cmdCount = 0;

		a.OnDraw();

		REQUIRE(testrec::drew);
		REQUIRE(testrec::drawBitmap == &fb.bm);
		REQUIRE(RectIs(testrec::drawSrc, 0, 0, 40, 20));
		REQUIRE(RectIs(testrec::drawDst, 10, 20, 40, 20));
		// FrALPHA(0xffffffff, 1 * 1) keeps the colour intact.
		REQUIRE(testrec::drawDiffuse == 0xffffffffu);
		REQUIRE(testrec::cmd == FRCMD_OWNERDRAW);
		REQUIRE(testrec::cmdCount == 1);
	}

	SECTION("without a bitmap only the owner draw command is sent")
	{
		a.SetBitmap(0);
		testrec::drew = false;
		testrec::cmd = FRCMD_NONE;
		testrec::cmdCount = 0;

		a.OnDraw();

		REQUIRE_FALSE(testrec::drew);
		REQUIRE(testrec::cmd == FRCMD_OWNERDRAW);
		REQUIRE(testrec::cmdCount == 1);
	}
}

TEST_CASE("FrArea mouse handlers forward the owner commands", "[Fresh][FrArea]")
{
	TestFrArea a;
	WPoint pt(0.0f, 0.0f);

	testrec::cmd = FRCMD_NONE;
	testrec::cmdCount = 0;

	REQUIRE_FALSE(a.OnLButtonDown(pt));
	REQUIRE(testrec::cmd == FRCMD_LBUTTONDOWN);

	REQUIRE_FALSE(a.OnLButtonUp(pt));
	REQUIRE(testrec::cmd == FRCMD_LBUTTONUP);

	REQUIRE(testrec::cmdCount == 2);
}

TEST_CASE("FrArea::OnSetCursor shows the hand cursor only for mouse-event areas", "[Fresh][FrArea]")
{
	TestFrArea a;
	WPoint pt(0.0f, 0.0f);

	testrec::cursor = -1;
	a.OnSetCursor(false, pt);
	REQUIRE(testrec::cursor == -1);

	a.SetMouseEvent(true);
	testrec::cursor = -1;
	a.OnSetCursor(true, pt);
	REQUIRE(testrec::cursor == 1);

	a.SetMouseEvent(false);
	testrec::cursor = -1;
	a.OnSetCursor(true, pt);
	REQUIRE(testrec::cursor == -1);
}
