// JP-only game code (no KR counterpart): FrItemCountController, the count
// picker of layout "MPBreakCountSetDlg". FrAlchemyMPBreakDlg::OpenCountSetDlg
// (0x4f9ed0) creates it for an owned stack of 5 or more, sets the row and
// its owned count (SetItem) and opens it with OnCountSetResult (0x4f9d50),
// which reads the chosen count and the row back.
//
// The form shows the item's name ("item_name"), a fixed text from the JP
// string table ("dlg_desc", id 0x1870f) and the count ("item_count", an
// edit the player can also type in, capped at the owned count by OnProc);
// "increase_cnt" / "Decrease_cnt" step it by one on release, and repeat
// while held (OnProc).
//
// JP x86 facts: RTTI 0xe74d70 "FrItemCountController" (static init
// 0xccac80, base FrForm 0xf1aabc), registered with the JP form factory
// (static init 0xccaca0: 0xa4c0d0 / 0xa4bcf0, make instance 0x4f6430),
// size 0x140. Base the JP FrForm (ctor 0xbe5e20, dtor 0xbe5e80, 38 slots).
// Vtables 0xd05774 (38 slots: [0] GetRTTI, [1] the deleting destructor,
// [9] OnProc; the rest FrForm's), 0xd05768 (FrCmdTarget: [0] thunk
// 0x4f3bb0, [1] GetMessageMap 0x4f3110), 0xd05760 ([0] thunk 0x4f3bc0).
// Message map 0xe07ac0 (rows 0xe07ae0, 0x20 bytes each, handlers stored by
// the static init 0xccacc0, base 0xe4343c). The port keeps the KR FrForm
// layout, so the JP offsets are comments only (no CHECK_FIELD), like
// JpPointShopDlg.h.
#ifndef PG_JP_JPMPBREAKCOUNTSETDLG_H
#define PG_JP_JPMPBREAKCOUNTSETDLG_H

#include "defines.h"
#include "frcmdtarget.h"
#include "frform.h"
#include "wtypes.h"

class FrButton;
class FrEdit;
class FrListItem;
class FrWnd;
class FrWndManager;

class FrItemCountController : public FrForm {
public:
	static const WRTTI m_RTTI;                                        // 0xe74d70
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }      // [0] 0x4f3140

	FrItemCountController(void);                                      // 0x4f3c40
	virtual ~FrItemCountController(void);                             // [1] 0x4f3c70 (deleting), 0x4f3120

	// Keeps the row and its owned count (the count's maximum) and shows
	// the item's name (the row's data starts with its type id) in
	// "item_name". Caller 0x4f9ed0.
	void SetItem(FrListItem* item, int maxCount);                     // 0x4f64c0

	// The chosen count and the row, read by OnCountSetResult (0x4f9d50).
	int GetCount(void) const { return m_count; }
	FrListItem* GetItem(void) const { return m_pItem; }

protected:
	// Holding "increase_cnt" or "Decrease_cnt" (button status PRESSED)
	// accumulates dt; once 0.5 - repeats * held time drops below the held
	// time the count steps (the release then does not step again). The
	// count edit's text is read back every frame and capped at the owned
	// count.
	virtual void OnProc(const float dt);                              // [9] 0x4f6530

private:
	static sFRESH_ENTRY _MsgEntries[];                                // 0xe07ae0

protected:
	static sFRESH_MSGMAP _MsgMap;                                     // 0xe07ac0
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;           // 0x4f3110

	// The count the edit shows (atoi of its first line).
	int GetEditCount(void);                                           // 0x4f3150
	// Clears the fields (the constructor's body; count 1).
	void Init(void);                                                  // 0x4f3170
	// One step up / down from the edit's count: up is capped at the owned
	// count (nothing more without one) and at 999, down stops at 0. The
	// new count goes to the chat input (CChatMsg::SetChatText) and to the
	// count edit.
	void Increase(void);                                              // 0x4f3ca0
	void Decrease(void);                                              // 0x4f3d50

	void OnItemNameInit(PG_MSGARG param);                             // [0] 0x4f3de0
	void OnDlgDescInit(PG_MSGARG param);                              // [1] 0x4f3e50
	void OnItemCountInit(PG_MSGARG param);                            // [2] 0x4f3ef0
	void OnIncreaseInit(PG_MSGARG param);                             // [3] 0x4f3f90
	// Release: ends the repeat; steps once unless the hold already did.
	void OnIncreaseClick(void);                                       // [4] 0x4f4000
	void OnDecreaseInit(PG_MSGARG param);                             // [5] 0x4f4030
	void OnDecreaseClick(void);                                       // [6] 0x4f40a0

private:
	FrEdit* m_pItemName;                    // JP +0x110 "item_name"
	FrEdit* m_pDesc;                        // JP +0x114 "dlg_desc"
	FrEdit* m_pCountEdit;                   // JP +0x118 "item_count"
	FrButton* m_pIncreaseBtn;               // JP +0x11c "increase_cnt"
	FrButton* m_pDecreaseBtn;               // JP +0x120 "Decrease_cnt"
	int m_maxCount;                         // JP +0x124 (SetItem: the owned count)
	int m_count;                            // JP +0x128 (1 from Init)
	FrListItem* m_pItem;                    // JP +0x12c (SetItem)
	bool m_repeated;                        // JP +0x130 (the hold stepped: the release does not)
	int m_holdMode;                         // JP +0x134 (0 none, 1 increase held, 2 decrease held)
	float m_holdTime;                       // JP +0x138 (time held since the last step)
	int m_repeatCount;                      // JP +0x13c (steps since the press)
};

// 0x4f6430: the JP form factory's make instance for "FrItemCountController".
IObject* FrItemCountControllerMakeInstance(void);
// 0x4f6020: CreateForm<FrItemCountController>(manager, owner, name,
// parent), with the thread-local slot (+0x48) store and the cast.
FrItemCountController* CreateItemCountController(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent);

#endif // PG_JP_JPMPBREAKCOUNTSETDLG_H
