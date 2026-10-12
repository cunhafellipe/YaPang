// Original: d:\build\custom\temp\code\client\fresh\source\frcursor.cpp
#include "FrCursor.h"
#include "mousecursor.h"

// WPoint lives in client\wangreal\include\wtypes.h, but a src/ header of
// that name would shadow the Platform SDK's wtypes.h (pulled in by
// windows.h) for every unit, so the part used here is declared locally.
struct _WPOINT {
	float x;    // 0x0
	float y;    // 0x4
};
CHECK_SIZE(_WPOINT, 0x8);

class WPoint : public _WPOINT {
};
CHECK_SIZE(WPoint, 0x8);

// frcursor.cpp:24
FrCursor::FrCursor() {
}

// frcursor.cpp:28
FrCursor::~FrCursor() {
}

// frcursor.cpp:32
void FrCursor::Init() {
	CMouseCursor::Instance()->Reset(true);
	CMouseCursor::Instance()->SetMode(CMouseCursor::SCREEN_TEST);
}

// frcursor.cpp:38
void FrCursor::SetCursor(eCursor cursor) {
	CMouseCursor::Instance()->SetCursor(cursor);
}

// frcursor.cpp:43
int FrCursor::GetCursor() {
	return CMouseCursor::Instance()->GetCursor();
}

// frcursor.cpp:48
void FrCursor::MoveCursor(const WPoint& pt) {
	CMouseCursor::Instance()->ForceMove(pt.x, pt.y, -1.0f);
}

// frcursor.cpp:53
void FrCursor::ShowCursor(bool show) {
	m_bShow = show;
}
