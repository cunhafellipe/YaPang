// JP-only game code: contents type 0xaa, the hole-in-one (chip-in) game
// document of single mode (RTTI ".?AVCHoleInOneGameDoc@_golfgame@_gateway@@",
// vtable 0xd0aba4). Not created by the JP CContentsDoc::Initialize 0x915da0
// itself: its creator 0x579370 (`new` 0x90 bytes, constructor 0x579140 with
// the type 0xaa) is registered by 0x579540 in the creator vector 0xe7e140,
// which 0x915da0 runs at its end (as the quest document 0xac). The game
// mode that uses it is CSharedDoc +0x10594 == 0xb (CShotCountInfo 0x53e850,
// the form constructor 0x781430 and FrUniteResultDlg::OnInit 0xaec130 keep
// it then). Its handler is _gateway::_golfgame::CHoleInOneGameHandler
// (object-factory name "CHoleInOneGameHandler", creator 0x57e7a0,
// constructor 0x57dfc0, vtable 0xd0ac7c; its message map 0x57e0a0), not
// ported.
#ifndef PG_JP_JPCHIPINCONTAINER_H
#define PG_JP_JPCHIPINCONTAINER_H

#include <stdint.h>

#include <map>
#include <utility>
#include <vector>

#include "defines.h"
#include "padding.h"
#include "GatewayDoc.h"

namespace _gateway {
namespace _golfgame {

// An element of the mission list (+0x2c): FrHoleinoneMissionDlg 0x57fe30
// shows its +0x14 with "%%%d". Not traced further (name ours).
struct sHoleInOneMission;

// An element of the vector +0x48 (8 bytes, two floats; Reset's erase copies
// them as floats). Name ours.
struct sHoleInOnePoint {
	float m_x;                    // +0x0
	float m_y;                    // +0x4
};

// The JP document of contents type 0xaa. The JP container base keeps the
// gateway task flags at +0x4 (as CQuestDocument, JpQuestForms.h); the KR
// base the port derives from has none, so the class declares it. JP layout (VS 2010
// vectors and maps, 0x10 bytes each): +0x4 the gateway flags; +0x8 the contents type; +0xc the
// handler; +0x10 a map with 0x24-byte nodes (a 4-byte key and a 0x10-byte
// value, untraced: not declared; Reset clears it with 0x575610); +0x2c the
// missions; +0x48 the points; +0x5c the hole records (filled by the
// handler's reader 0x57dce0); +0x6c a map with 0x18-byte nodes (cleared by
// Reset 0x575710 and ResetHoles).
class CHoleInOneGameDoc : public CGatewayControlDoc {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x579140: stores the type (0xaa from the creator 0x579370).
	explicit CHoleInOneGameDoc(int contentType);
	virtual ~CHoleInOneGameDoc(void);                                 // [0] 0x579510 (deleting), 0x579270

	virtual bool Initialize(void);                                    // [1] 0x570660
	virtual bool Release(void) { return true; }                       // [2] 0x5706b0
	virtual bool Reset(void);                                         // [3] 0x5793e0
	virtual CGatewayHandler* GetGatewayHandler(void) { return m_pGatewayHandler; } // [4] 0x579260

	// 0x579590 (name ours): clears the hole records (+0x5c), resizes them
	// to `count` (0x579490) and clears the map +0x6c. Called by the
	// handler's reader 0x57dce0 with the packet's hole count.
	void ResetHoles(int count);

	// The chip-in size combo (FrSingleTypeDlg +0x2b8: "normal size", x2 ..
	// x9) stores its selection + 1 (0x5226b0, inlined in the JP);
	// SetupSingleModeControls 0x522f00 and Initialize set 1. The handler's
	// message 0x57b0d0 returns it.
	void SetChipInSize(int chipInSize) { m_chipInSize = chipInSize; }
	int GetChipInSize(void) const { return m_chipInSize; }
	// The slope combo (FrSingleTypeDlg +0x2b4: "apply", "cancel") stores
	// whether "apply" (index 0) is picked (0x522600, inlined in the JP);
	// SetupSingleModeControls 0x522f00 and the dialog's 0x522e60 (outside
	// room mode 0x13) clear it.
	void SetApplySlope(bool applySlope) { m_applySlope = applySlope; }
	bool GetApplySlope(void) const { return m_applySlope; }

private:
	// The JP base's gateway task flags (stored by the JP actor's
	// QueryHandler 0x91e850; the port's KR QueryHandler does not).
	uint32_t m_gatewayFlags;                               // 0x4
	int m_contentType;                                     // 0x8
	CGatewayHandler* m_pGatewayHandler;                    // 0xc
	// +0x10: the untraced map (see above).
	UNKNOWN(0x10, 0x20);
	int m_value20;                                         // 0x20 (Initialize: 0)
	int m_value24;                                         // 0x24 (Reset: 0)
	int m_value28;                                         // 0x28 (Reset: 0)
	std::vector<sHoleInOneMission*> m_missions;            // 0x2c
	float m_value3c;                                       // 0x3c (Reset: 0)
	float m_value40;                                       // 0x40 (Reset: 0)
	float m_value44;                                       // 0x44 (Reset: 0)
	std::vector<sHoleInOnePoint> m_points;                 // 0x48
	int m_value58;                                         // 0x58 (Reset: 0)
	// One record per hole: the (u32, u32) pairs the reader 0x57dce0 reads
	// after each hole's fields.
	std::vector<std::vector<std::pair<int32_t, int32_t> > > m_holeRecords; // 0x5c
	// 0x18-byte nodes: a 4-byte key and a 4-byte value (types untraced).
	std::map<int32_t, int32_t> m_holeValues;               // 0x6c
	int m_chipInSize;                                      // 0x7c (1..9)
	int m_value80;                                         // 0x80 (Initialize: 1)
	int m_value84;                                         // 0x84 (Initialize: 0)
	int m_value88;                                         // 0x88 (Reset: 0)
	bool m_applySlope;                                     // 0x8c
};
// size 0x90 (x86, VS 2010 containers; the checks are off in the 64-bit port)
BEGIN_LAYOUT_CHECK(CHoleInOneGameDoc)
	CHECK_FIELD(CHoleInOneGameDoc, m_contentType, 0x8);
	CHECK_FIELD(CHoleInOneGameDoc, m_pGatewayHandler, 0xc);
	CHECK_FIELD(CHoleInOneGameDoc, m_chipInSize, 0x7c);
	CHECK_FIELD(CHoleInOneGameDoc, m_applySlope, 0x8c);
END_LAYOUT_CHECK
CHECK_SIZE(CHoleInOneGameDoc, 0x90);

} // namespace _golfgame
} // namespace _gateway

// Port glue (JP creator 0x579370: new CHoleInOneGameDoc(0xaa)), for the KR
// IContentsDoc.cpp to call under JP_PAKS without including this header.
IContentsDataContainer* JpCreateChipInContainer(void);

#endif // PG_JP_JPCHIPINCONTAINER_H
