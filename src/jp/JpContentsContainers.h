// JP-only game code: the JP contents containers the Refresh-era dialogs
// read through the JP CContentsDoc (WSingleton instance 0xe47db4,
// GetContainer 0x9137e0 = KR CContentsDoc::GetContainer: map +0x28 find,
// the node's +0x10, NULL when missing). The JP base IContentsDataContainer
// (RTTI vtable 0xd02394: [0] 0x4b5ec0 destructor, [1] Initialize and [2]
// Release pure, [3] Reset 0x4b5eb0) has no slot 4: the KR
// GetGatewayHandler slot (icontentsdoc.h) is absent in JP. The port derives
// from the KR base.
//
// The JP CContentsDoc's container table is built by 0x915da0 (each entry
// `new X` with X's constructor inlined, then 0x913c70(type, container, 0)).
// Type 0xaf (JpRoomListState.h's room-sort container, 0x559700) is not in
// that table: it is _gateway::_grandprix::CGrandPrixDocument, made by the
// creator 0x559c00 from the creator vector 0xe7e140 (JpContentsGatewayDocs.h);
// the port creates it too (IContentsDoc.cpp under JP_PAKS, since R909).
#ifndef PG_JP_JPCONTENTSCONTAINERS_H
#define PG_JP_JPCONTENTSCONTAINERS_H

#include <stdint.h>

#include "defines.h"
#include "padding.h"
#include "icontentsdoc.h"

// Contents type 0xa5 (0x915da0: `new` 0x1c bytes, vtable 0xd3c600, RTTI
// "CLobbyContainer"): the lobby's selection state. FrRoomListDlg keeps it at
// +0x144 (its m_pRoomListData, JpRoomListDlg.h), FrGameRoomDlg at +0x14c
// (the constructor 0x5083c0 fetches it with GetContainer(0xa5)).
class CLobbyContainer : public IContentsDataContainer {
	LAYOUT_CHECK_FRIEND;
public:
	CLobbyContainer(void) {}                                          // 0x911cd0
	virtual ~CLobbyContainer(void) {}                                 // [0] 0x912380 (deleting)

	virtual bool Initialize(void) { Reset(); return true; }           // [1] 0x907aa0
	virtual bool Release(void) { return true; }                       // [2] 0x907ab0
	virtual bool Reset(void) {                                        // [3] 0x907ac0
		m_selectedRoom = -1;
		m_selectedUserId = 0xffffffff;
		m_selectedUserNo = 0xffffffff;
		m_hostUserId = 0xffffffff;
		m_roomCategory = 4;
		m_userMenuBlocked = false;
		return true;
	}

	// The room list's selection; FrRoomListDlg's category buttons reset it
	// to -1.
	int m_selectedRoom;                  // 0x4
	// The shown room category (0 vs, 1 mass, 2 battle, 3 chat, 4 total;
	// Reset: 4).
	int m_roomCategory;                  // 0x8
	// The picked user: FrRoomListDlg stores the lobby user record's +0x4
	// and +0x0, FrGameRoomDlg::OnUserLBtnDown the room player's +0x0 and
	// +0x6c (-1 when none, or for an excluded player in room modes
	// 0x14/0x15). OnUserDblClick compares +0xc with the own JP CSharedDoc
	// +0x816 and sends packet 0x3f with +0x10.
	uint32_t m_selectedUserId;           // 0xc
	uint32_t m_selectedUserNo;           // 0x10
	// FrGameRoomDlg::OnUserLBtnDown opens the user's action menu (0x4ff320)
	// only when this equals the own JP CSharedDoc +0x81a (role inferred:
	// the room's host).
	uint32_t m_hostUserId;               // 0x14
	// FrGameRoomDlg::OnUserRBtnUp ignores the click while it is set and the
	// room state (+0x178) is 6.
	bool m_userMenuBlocked;              // 0x18
};
// size 0x1c (x86)
BEGIN_LAYOUT_CHECK(CLobbyContainer)
	CHECK_FIELD(CLobbyContainer, m_selectedRoom, 0x4);
	CHECK_FIELD(CLobbyContainer, m_roomCategory, 0x8);
	CHECK_FIELD(CLobbyContainer, m_selectedUserId, 0xc);
	CHECK_FIELD(CLobbyContainer, m_selectedUserNo, 0x10);
	CHECK_FIELD(CLobbyContainer, m_hostUserId, 0x14);
	CHECK_FIELD(CLobbyContainer, m_userMenuBlocked, 0x18);
END_LAYOUT_CHECK
CHECK_SIZE(CLobbyContainer, 0x1c);

#endif // PG_JP_JPCONTENTSCONTAINERS_H
