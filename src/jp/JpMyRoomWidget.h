// JP-only game code (no KR counterpart class): CMyRoomWidget, the lobby's
// MY ROOM page. The JP has no CRealMyRoomTask switch from the lobby:
// CTaskMain::MyRoomUp 0x65c290 -> 0x65b8a0 sends packet 0xb5 and posts Lobby
// message 0x88 ("CMyRoomWidget", "TOPPAGE" or "AVATARCHAT_MAIN"), which
// creates this widget through ObjectFactory (0x9bf6e0) as the current task's
// +0x1fc page. Its layout is the Fresh form "realmyroom" (the JP data has no
// REALMYROOM task layout). The widget owns the CItemStorage singleton the KR
// CRealMyRoomTask constructor creates (Load news it, CloseForm deletes it).
// RTTI 0xe76c10 ("CMyRoomWidget", static init 0xcd15f0, base CTaskWidget
// 0xe78d18); vtables 0xd0ceb8 (IObject), 0xd0ceb0 (CHandOwner: CTaskWidget's
// 0x64f5d0), 0xd0cea4 (FrCmdTarget: the deleting-destructor thunk 0x5a73c0,
// then 0xc08430). Size 0x1f8 (x86, the factory's new): no members of its own.
//
// Its OnPacket [6] 0x5a8360 (10409 bytes) is the KR CRealMyRoomTask::OnPacket
// (client/ProjectG/RealMyRoomTask.cpp) moved onto the widget, on JP packet
// ids (the 0xcd.. jump table 0x5aac3c / index table 0x5aaca0): see
// JpMyRoomWidget.cpp for the cases written and the ones left.
#ifndef PG_JP_JPMYROOMWIDGET_H
#define PG_JP_JPMYROOMWIDGET_H

#include <list>

#include "JpTaskPage.h"

class CMyRoomWidget : public CTaskWidget {
public:
	static const WRTTI m_RTTI;                                        // 0xe76c10
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x5a7030

	CMyRoomWidget(void);                                              // 0x5a7010
	virtual ~CMyRoomWidget(void);                                     // [1] 0x5a7d10 (deleting), 0x5a7b00
	// Sets m_layoutFile to "realmyroom", creates the CItemStorage singleton,
	// then CTaskWidget::Load.
	virtual void Load(void);                                          // [2] 0x5a7f60
	// In the lobby, moves the form right by (screen width - 800) * 0.3;
	// opens it (no callback, flag 1), then CTaskWidget::Open.
	virtual void Open(void);                                          // [3] 0x5a7790
	// Resumes the lobby avatar and caddie (message 0x2af) and deletes the
	// CItemStorage singleton; the form itself is not shut down here.
	virtual void CloseForm(void);                                     // [4] 0x5a7870
	virtual void Process(float dt);                                   // [5] 0x5a7040
	// The KR CRealMyRoomTask::OnPacket on the JP ids; 1 = handled.
	virtual int OnPacket(WReceivedPacket& packet);                    // [6] 0x5a8360
	// [7] and [8] are CTaskWidget's (0x64f1c0, 0x64f1d0).
};
CHECK_SIZE(CMyRoomWidget, 0x1f8);

// One prize of a pouch open (packet 0x129): 0x14 bytes the JP copies whole
// (its fields are read by the pouch result readers 0xaaa0b0 / 0xaaa240 /
// 0x623500, not decompiled yet).
struct sJpPouchPrize {
	unsigned char m_data[0x14];  // 0x0
};
CHECK_SIZE(sJpPouchPrize, 0x14);

// The pouch result the JP fills on packet 0x129 before posting message
// 0x279: a function-local static (0xe76890, guard bit 0xe768a0, atexit
// destructor 0xcf9da0) behind the getter 0x5a8160. x86 size 0x10 (the
// std::list is head + size there; no CHECK_SIZE: the std::list differs on
// LP64).
struct sJpPouchResult {
	std::list<sJpPouchPrize> m_prizes;  // 0x0
	unsigned long m_value8;             // 0x8, not written by the 0x129 path
	unsigned long m_itemTypeId;         // 0xc, the opened pouch's type id

	// Empties the prize list and zeroes the item type id.
	void Clear(void);                           // 0xaa9860
	void SetItem(unsigned long itemTypeId);     // 0xaa8eb0
	// Appends a prize; returns the new prize count.
	unsigned int AddPrize(const sJpPouchPrize& prize);  // 0xaaa1f0
};
sJpPouchResult* JpPouchResult_Instance(void);   // 0x5a8160

#endif // PG_JP_JPMYROOMWIDGET_H
