// JP-only game code (no KR counterpart class): CShopWidget, the lobby's
// shop page. The JP has no CShopTask: CTaskMain::ShopUp 0x65f9b0 sends
// packet 0x140 and posts Lobby message 0x88 ("CShopWidget", "TOPPAGE"),
// which creates this widget through ObjectFactory (0x9bf6e0) as the current
// task's +0x1fc page, shown over TOPPAGE. Its layout is the Fresh form
// "shopmain" (the JP CShopMain FrTaskForm; JpFrForms.cpp leaves its
// registration out). RTTI 0xe78be0 ("CShopWidget", static init 0xcd1f10,
// base CTaskWidget 0xe78d18); vtables 0xd12380 (IObject), 0xd12378
// (CHandOwner: CTaskWidget's 0x64f5d0), 0xd1236c (FrCmdTarget: the
// deleting-destructor thunk 0x5e0d60, then 0xc08430). Size 0x1f8 (x86):
// no members of its own.
//
// Its OnPacket [6] 0x5e0ff0 (7994 bytes) is the KR CShopTask::OnPacket
// (jp/pairing.tsv, client/ProjectG/ShopTask.cpp) moved onto the widget:
// packets 0x68 (buy), 0x6a (gift), 0xb0, 0xb6, 0x107, 0x12d, 0x194.
#ifndef PG_JP_JPSHOPWIDGET_H
#define PG_JP_JPSHOPWIDGET_H

#include "JpTaskPage.h"

class CShopWidget : public CTaskWidget {
public:
	static const WRTTI m_RTTI;                                        // 0xe78be0
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x5e0cc0

	CShopWidget(void);                                                // 0x5e0ca0
	virtual ~CShopWidget(void);                                       // [1] 0x5e0e50 (deleting), 0x5e0e00
	// Sets m_layoutFile to "shopmain", then CTaskWidget::Load.
	virtual void Load(void);                                          // [2] 0x5e0fd0
	// In the lobby, moves the form right by (screen width - 800) * 0.3;
	// opens it (no callback, flag 1), then CTaskWidget::Open.
	virtual void Open(void);                                          // [3] 0x5e0e80
	// Empty: the shop form is not shut down by the widget.
	virtual void CloseForm(void) {}                                   // [4] 0x5e0cd0
	virtual void Process(float dt);                                   // [5] 0x5e0ce0
	virtual int OnPacket(WReceivedPacket& packet);                    // [6] 0x5e0ff0
	// [7] and [8] are CTaskWidget's (0x64f1c0, 0x64f1d0).
};
CHECK_SIZE(CShopWidget, 0x1f8);

#endif // PG_JP_JPSHOPWIDGET_H
