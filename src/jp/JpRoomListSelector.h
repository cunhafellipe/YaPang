// JP-only game code (no KR counterpart): the lobby kind selector, layout
// "roomlistselector". CTaskMain 0x6586c0 (KR CTaskMain::OnUnderBar_GameUp)
// opens it through CreateRoomListSelector when the JP CSharedDoc's +0x10d3c
// bit 11 is set. Two buttons pick the lobby: "normallobby" (the normal room
// list) or "grandprixlobby" (the Grand Prix lobby); "expl" flips the two
// pictures ("img_multi", "img_gp") between their images and their
// explanations.
//
// JP x86 facts: RTTI 0xe75e88 named "FrRoomListSelector" (static init
// 0xcd12b0, base FrForm 0xf1aabc), factory registration 0x59e4b0 (static
// init 0xcd12d0, make instance 0x59e440), size 0x130 (`push 130h` before the
// constructor call in 0x654490), vtables 0xd0cb6c (IObject/FrWnd), 0xd0cb60
// (FrCmdTarget), 0xd0cb58; message map 0xe0f2f8 (rows 0xe0f308, handlers
// stored by the static init 0xcd12f0, base 0xe4343c). No virtual past the
// JP FrForm's 38 slots is overridden except [0] GetRTTI and [1] the
// destructor. The port keeps the KR FrForm layout, so the JP offsets are
// comments only (no CHECK_FIELD).
#ifndef PG_JP_JPROOMLISTSELECTOR_H
#define PG_JP_JPROOMLISTSELECTOR_H

#include "defines.h"
#include "frcmdtarget.h"
#include "frform.h"
#include "wtypes.h"

class FrArea;
class FrButton;
class FrWndManager;

class FrRoomListSelector : public FrForm {
public:
	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }   // [0] 0x59e2a0

	FrRoomListSelector(void);
	virtual ~FrRoomListSelector(void);   // [1] 0x59e4d0 (scalar deleting)

private:
	static sFRESH_ENTRY _MsgEntries[];

protected:
	static sFRESH_MSGMAP _MsgMap;
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;   // FrCmdTarget [1] 0x59e260

	void OnExplClick(void);
	void OnNormalLobbyInit(PG_MSGARG param);
	void OnGrandPrixLobbyInit(PG_MSGARG param);
	void OnMultiImgInit(PG_MSGARG param);
	void OnGrandPrixImgInit(PG_MSGARG param);
	void OnNormalLobbyClick(void);
	void OnGrandPrixLobbyClick(void);

private:
	// The constructor sets only m_showingExpl; the "INIT" handlers fill the
	// four control pointers when the layout loads.
	FrButton* m_pNormalLobbyButton;     // JP +0x110 ("normallobby")
	FrButton* m_pGrandPrixLobbyButton;  // JP +0x114 ("grandprixlobby")
	FrArea* m_pMultiImg;                // JP +0x118 ("img_multi")
	FrArea* m_pGrandPrixImg;            // JP +0x11c ("img_gp")
	// Whether the pictures show their explanations ("start_expl_*") rather
	// than their images ("start_img_*"); the layout starts on the images.
	bool m_showingExpl;                 // JP +0x120
	// JP +0x121 .. +0x130: not touched by any of the class's functions.
};

// 0x654490: CreateForm<FrRoomListSelector>(manager, owner, name, parent),
// which also stores the form in a JP thread-local slot (+0xc0) and runs
// IsKindOf(&FrRoomListSelector::m_RTTI) on it (result unused).
FrForm* CreateRoomListSelector(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent);

#endif // PG_JP_JPROOMLISTSELECTOR_H
