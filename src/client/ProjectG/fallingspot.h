// Original: d:\build\custom\temp\code\client\projectg\fallingspot.h
#ifndef PG_CLIENT_PROJECTG_FALLINGSPOT_H
#define PG_CLIENT_PROJECTG_FALLINGSPOT_H

#include "defines.h"
#include "padding.h"
#include "actor.h"
#include "RTTI.h"
#include "scenemanager.h"
#include "wmath.h"
#include "wvideo.h"

class IObject;
class C3dFont;
class CSimpleUI;
class CClubRangeInfo;
class WOverlay;
class WFont;

IObject* CSpotMakeInstance(void);

// The falling spot (landing point indicator) actor: IActor at 0x0,
// CRenderFuncPtr at 0x18 (seen in the constructor). Size 0x1a0 (the `new`
// of CSpotMakeInstance). Members are transcribed as far as the decompiled
// functions need them.
class CSpot : public IActor, public CRenderFuncPtr {
	LAYOUT_CHECK_FRIEND;
	friend IObject* CSpotMakeInstance(void);
public:
	static const WRTTI m_RTTI;

	// A rival's landing point (UpdateRivals fills an array of them from
	// CSharedDoc::m_gameResultList), 0x18 bytes.
	struct sRivalPos {
		float m_speed;          // 0x0, 50 / the message's param3 (HandleMsg 0xf8)
		float m_fade;           // 0x4, 0, 1 (gave up in an approach game) or -1 (moved to a tee)
		unsigned char m_hole;   // 0x8
		PADDING(0x9, 0xc);
		WVector m_pos;          // 0xc
	};

	// fallingspot.h:15
	virtual ~CSpot(void) {}
	// fallingspot.h:17
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }

	virtual void OnPreLoadInit(void);
	virtual void OnLoad(void);
	virtual void OnInit(void);
	virtual void OnProcess(float elapsed);
	virtual void OnDestroy(void);
	virtual void Display(void);

protected:
	CSpot(void);

	virtual void HandleMsg(const MsgObject& msg);

	void ClipIndicator(int);
	void DrawRange(void);
	void Rotate(void);
	void DrawHoleIndicator(void);
	void DrawIndicator(const WVector&, int);

private:
	static const unsigned long m_adwColors[5];

	float GetMovingRate(float t);
	bool _ValidTopViewLineDraw(void);
	bool UpdateRivals(void);

protected:
	C3dFont* m_pFont1c;             // 0x1c
	C3dFont* m_pFont20;             // 0x20
	WVector m_spotPos;              // 0x24, the landing point
	WVector m_unknown30;            // 0x30
	WVector m_impactPos;            // 0x3c, HandleMsg 0xf6 copies its param2 here
	float m_angle;                  // 0x48, the aim direction (Rotate: RotMat about y)
	float m_unknown4c;              // 0x4c
	WVector m_unknown50;            // 0x50
	WVector m_dir;                  // 0x5c
	WVector m_unknown68;            // 0x68
	float m_unknown74;              // 0x74
	int m_unknown78;                // 0x78
	float m_rangeRotation;          // 0x7c, DrawRange turns the range quad by 0.7 * this
	bool m_unknown80;               // 0x80
	PADDING(0x81, 0x84);
	float m_unknown84;              // 0x84
	float m_unknown88;              // 0x88
	CClubRangeInfo* m_pRangeInfo;   // 0x8c
	unsigned long m_indicatorColor; // 0x90, DrawHoleIndicator: the arrow's RGB
	float m_unknown94;              // 0x94
	float m_unknown98;              // 0x98
	int m_hTexture9c;               // 0x9c, texture handles
	int m_hTexturea0;               // 0xa0
	int m_hTexturea4;               // 0xa4
	int m_hTexturea8;               // 0xa8
	float m_rangeSpread;            // 0xac, DrawRange: quad half-size factor, alpha (1 - this)
	float m_groundPower;            // 0xb0, OnProcess: CQuadTree min/max power on ground blended by m_unknownb4 squared
	float m_unknownb4;              // 0xb4, OnProcess: ping-pongs between 0 and 1
	float m_unknownb8;              // 0xb8
	WOverlay* m_pOverlaybc;         // 0xbc
	WOverlay* m_pOverlayc0;         // 0xc0
	WOverlay* m_pOverlayc4;         // 0xc4
	WOverlay* m_pOverlayc8;         // 0xc8
	CSimpleUI* m_pSimpleUI;         // 0xcc
	float m_unknownd0;              // 0xd0, OnProcess: angle advanced by pi per second, wrapped at 2 pi
	int m_hTextured4;               // 0xd4
	WTVertex* m_pVertices[5];       // 0xd8, the indicator's fan, NULL-terminated ([4] at 0xe8)
	WTVertex m_vertices[4];         // 0xec
	bool m_unknown18c;              // 0x18c
	UNKNOWN(0x18d, 0x194);
	sRivalPos* m_pRivals;           // 0x194, new[] by UpdateRivals
	bool m_bRegistered;             // 0x198, set by OnPreLoadInit
	PADDING(0x199, 0x19c);
	WFont* m_pMaskedFont;           // 0x19c, CChatMsg's (OnLoad)
};

CHECK_SIZE(CSpot::sRivalPos, 0x18);
CHECK_SIZE(CSpot, 0x1a0);
BEGIN_LAYOUT_CHECK(CSpot)
	CHECK_FIELD(CSpot::sRivalPos, m_hole, 0x8);
	CHECK_FIELD(CSpot::sRivalPos, m_pos, 0xc);
	CHECK_FIELD(CSpot, m_pFont1c, 0x1c);
	CHECK_FIELD(CSpot, m_pFont20, 0x20);
	CHECK_FIELD(CSpot, m_spotPos, 0x24);
	CHECK_FIELD(CSpot, m_impactPos, 0x3c);
	CHECK_FIELD(CSpot, m_angle, 0x48);
	CHECK_FIELD(CSpot, m_dir, 0x5c);
	CHECK_FIELD(CSpot, m_unknown80, 0x80);
	CHECK_FIELD(CSpot, m_pRangeInfo, 0x8c);
	CHECK_FIELD(CSpot, m_indicatorColor, 0x90);
	CHECK_FIELD(CSpot, m_hTexture9c, 0x9c);
	CHECK_FIELD(CSpot, m_pOverlaybc, 0xbc);
	CHECK_FIELD(CSpot, m_pSimpleUI, 0xcc);
	CHECK_FIELD(CSpot, m_unknownb8, 0xb8);
	CHECK_FIELD(CSpot, m_pVertices, 0xd8);
	CHECK_FIELD(CSpot, m_vertices, 0xec);
	CHECK_FIELD(CSpot, m_unknown18c, 0x18c);
	CHECK_FIELD(CSpot, m_pRivals, 0x194);
	CHECK_FIELD(CSpot, m_bRegistered, 0x198);
	CHECK_FIELD(CSpot, m_pMaskedFont, 0x19c);
END_LAYOUT_CHECK

#endif
