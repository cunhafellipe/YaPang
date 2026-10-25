// JP-only game code: the hole-in-one (chip-in) game document, contents type
// 0xaa (_gateway::_golfgame::CHoleInOneGameDoc, JpChipInContainer.h).
// lib: 0x575610 (+0x10 map clear), 0x575710 (+0x6c map clear), 0x578f00
// (+0x5c erase), 0x579490 (+0x5c resize), 0x5758e0, 0x575840, 0x578f70
// (destructor member teardown)
#include "JpChipInContainer.h"

namespace _gateway {
namespace _golfgame {

// 0x579140
CHoleInOneGameDoc::CHoleInOneGameDoc(int contentType)
	: m_gatewayFlags(0), m_contentType(contentType), m_pGatewayHandler(0),
	  m_value20(0), m_value24(0), m_value28(0), m_value3c(0.0f), m_value40(0.0f),
	  m_value44(0.0f), m_value58(0), m_chipInSize(0), m_value80(0), m_value84(0),
	  m_value88(0), m_applySlope(false) {
	// The JP constructor leaves the scalars to Initialize / Reset; the port
	// zeroes them (the containers are built as in the JP).
}

// 0x579270 (the members' destruction; 0x579510 is the deleting form)
CHoleInOneGameDoc::~CHoleInOneGameDoc(void) {
}

// 0x570660: the handler comes from the object factory by the name
// *0xe0c748 ("CHoleInOneGameHandler", 0xa49100 / 0xa48210). That handler is
// not ported: the port keeps NULL (GetGatewayHandler returns it).
bool CHoleInOneGameDoc::Initialize(void) {
	Reset();
	m_pGatewayHandler = 0;
	m_chipInSize = 1;
	m_value80 = 1;
	m_value84 = 0;
	m_value20 = 0;
	return true;
}

// 0x579590
void CHoleInOneGameDoc::ResetHoles(int count) {
	m_holeRecords.clear();
	m_holeRecords.resize(count);
	m_holeValues.clear();
}

// 0x5793e0: the untraced map +0x10 is cleared first (0x575610; not
// declared, see the header).
bool CHoleInOneGameDoc::Reset(void) {
	m_missions.clear();
	m_points.clear();
	m_value24 = 0;
	m_value28 = 0;
	m_value44 = 0.0f;
	m_value3c = 0.0f;
	m_value40 = 0.0f;
	m_holeRecords.clear();
	m_holeValues.clear();
	m_value58 = 0;
	m_value88 = 0;
	return true;
}

} // namespace _golfgame
} // namespace _gateway

// Port glue (JP creator 0x579370, registered by 0x579540 in the creator
// vector 0xe7e140 that CContentsDoc::Initialize 0x915da0 runs last).
IContentsDataContainer* JpCreateChipInContainer(void) {
	return new _gateway::_golfgame::CHoleInOneGameDoc(0xaa);
}
