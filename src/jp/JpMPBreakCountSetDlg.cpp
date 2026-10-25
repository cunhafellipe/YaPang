// JP-only game code: FrItemCountController, the count picker of layout
// "MPBreakCountSetDlg". See JpMPBreakCountSetDlg.h.
#include "JpMPBreakCountSetDlg.h"
#include "JpLobbyGlue.h"

#include "ChatMsg.h"
#include "ItemManager.h"
#include "ObjectFactory.h"
#include "SharedDoc.h"
#include "commonutil.h"
#include "frbutton.h"
#include "fredit.h"
#include "frlistbox.h"
#include "frwnd.h"
#include "frwndmanager.h"

#include <stdlib.h>

// lib: 0x4f3c70 (deleting destructor), 0x4f3bb0, 0x4f3bc0 (this-adjusting
// deleting-destructor thunks),
// 0xccac80 (m_RTTI's dynamic initializer), 0xccacc0 (message-map
// initializer), 0xccaca0 (factory registration)

const WRTTI FrItemCountController::m_RTTI("FrItemCountController", &FrForm::m_RTTI);

// 0x4f3110
const sFRESH_MSGMAP* FrItemCountController::GetMessageMap(void) const {
	return &FrItemCountController::_MsgMap;
}

sFRESH_MSGMAP FrItemCountController::_MsgMap = { &FrForm::_MsgMap, &FrItemCountController::_MsgEntries[0] };

sFRESH_ENTRY FrItemCountController::_MsgEntries[] = {
	{ "item_name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemCountController::OnItemNameInit },
	{ "dlg_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemCountController::OnDlgDescInit },
	{ "item_count", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemCountController::OnItemCountInit },
	{ "increase_cnt", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemCountController::OnIncreaseInit },
	{ "increase_cnt", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrItemCountController::OnIncreaseClick },
	{ "Decrease_cnt", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrItemCountController::OnDecreaseInit },
	{ "Decrease_cnt", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrItemCountController::OnDecreaseClick },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x4f3120
FrItemCountController::~FrItemCountController(void) {
}

// 0x4f3150
int FrItemCountController::GetEditCount(void) {
	return atoi(m_pCountEdit->GetLine(1, false));
}

// 0x4f3170
void FrItemCountController::Init(void) {
	m_holdTime = 0.0f;
	m_maxCount = 0;
	m_count = 1;
	m_pItem = NULL;
	m_pDesc = NULL;
	m_pItemName = NULL;
	m_pIncreaseBtn = NULL;
	m_pDecreaseBtn = NULL;
	m_holdMode = 0;
	m_repeated = false;
	m_repeatCount = 0;
}

// 0x4f3c40 (m_pCountEdit is not cleared: the "item_count" init sets it)
FrItemCountController::FrItemCountController(void) {
	Init();
}

// 0x4f3ca0. The JP SetLine (the edit's vtable +0x60) takes a sixth
// argument, 0 here.
void FrItemCountController::Increase(void) {
	m_repeatCount++;
	m_count = GetEditCount() + 1;
	if (m_maxCount == 0) {
		return;
	}
	if (m_count > m_maxCount) {
		m_count = m_maxCount;
	}
	if (m_count > 999) {
		m_count = 999;
	}
	WSingleton<CChatMsg>::Instance()->SetChatText(MakeStr("%d", m_count), false);
	m_pCountEdit->ClearLine();
	m_pCountEdit->SetLine(1, MakeStr("%d", m_count), 0, false, 0);
}

// 0x4f3d50
void FrItemCountController::Decrease(void) {
	m_repeatCount++;
	m_count = GetEditCount() - 1;
	if (m_count < 1) {
		m_count = 0;
	}
	WSingleton<CChatMsg>::Instance()->SetChatText(MakeStr("%d", m_count), false);
	m_pCountEdit->ClearLine();
	m_pCountEdit->SetLine(1, MakeStr("%d", m_count), 0, false, 0);
}

// 0x4f3de0
void FrItemCountController::OnItemNameInit(PG_MSGARG param) {
	m_pItemName = DYNAMIC_CAST(FrEdit, param);
}

// 0x4f3e50 (the edit is used without a NULL test)
void FrItemCountController::OnDlgDescInit(PG_MSGARG param) {
	m_pDesc = DYNAMIC_CAST(FrEdit, param);
	m_pDesc->ClearLine();
	m_pDesc->SetLine(1, Jp_GetTableText(0x1870f), 0, false, 0);
}

// 0x4f3ef0 (the edit is used without a NULL test)
void FrItemCountController::OnItemCountInit(PG_MSGARG param) {
	m_pCountEdit = DYNAMIC_CAST(FrEdit, param);
	m_pCountEdit->ClearLine();
	m_pCountEdit->SetLine(1, MakeStr("%d", m_count), 0, false, 0);
}

// 0x4f3f90
void FrItemCountController::OnIncreaseInit(PG_MSGARG param) {
	m_pIncreaseBtn = DYNAMIC_CAST(FrButton, param);
}

// 0x4f4000
void FrItemCountController::OnIncreaseClick(void) {
	m_holdMode = 0;
	m_repeatCount = 0;
	if (m_repeated) {
		m_repeated = false;
		return;
	}
	Increase();
}

// 0x4f4030
void FrItemCountController::OnDecreaseInit(PG_MSGARG param) {
	m_pDecreaseBtn = DYNAMIC_CAST(FrButton, param);
}

// 0x4f40a0
void FrItemCountController::OnDecreaseClick(void) {
	m_holdMode = 0;
	m_repeatCount = 0;
	if (m_repeated) {
		m_repeated = false;
		return;
	}
	Decrease();
}

// 0x4f6020
FrItemCountController* CreateItemCountController(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent) {
	return CreateForm<FrItemCountController>(manager, owner, name, parent);
}

// 0x4f6430
IObject* FrItemCountControllerMakeInstance(void) {
	return new FrItemCountController;
}

struct __sFrItemCountController {
	__sFrItemCountController(void) {
		ObjectFactory().AddObjectFunctor(FrItemCountControllerMakeInstance, "FrItemCountController");
	}
};
static __sFrItemCountController s_registerFrItemCountController;

// 0x4f64c0 (the item name through the JP CItemManager 0xa18400, KR
// GetItemName; the JP SetLine's sixth argument 0)
void FrItemCountController::SetItem(FrListItem* item, int maxCount) {
	m_pItem = item;
	m_maxCount = maxCount;
	if (item == NULL) {
		return;
	}
	unsigned int typeId = *static_cast<const unsigned int*>(item->pData);
	m_pItemName->ClearLine();
	m_pItemName->SetLine(1, ItemManager()->GetItemName(typeId), 0, false, 0);
}

// 0x4f6530 (FrForm's OnProc is not called)
void FrItemCountController::OnProc(const float dt) {
	if (m_pIncreaseBtn && m_pIncreaseBtn->GetStatus() == FrButton::PRESSED) {
		m_holdMode = 1;
	} else if (m_pDecreaseBtn && m_pDecreaseBtn->GetStatus() == FrButton::PRESSED) {
		m_holdMode = 2;
	} else {
		m_holdMode = 0;
	}
	int mode = m_holdMode;
	if (mode == 0) {
		m_holdTime = 0.0f;
	} else if (mode == 1 || mode == 2) {
		m_holdTime += dt;
	}
	if (m_holdTime != 0.0f) {
		float elapsed = m_repeatCount * m_holdTime;
		float left = 0.5 - elapsed;
		if (left < m_holdTime) {
			m_repeated = true;
			if (mode == 1) {
				Increase();
				m_holdTime = 0.0f;
			} else if (mode == 2) {
				Decrease();
				m_holdTime = 0.0f;
			}
		}
	}
	if (m_pCountEdit) {
		m_count = atoi(m_pCountEdit->GetLine(1, false));
		if (m_count > m_maxCount) {
			m_pCountEdit->SetLine(1, MakeStr("%d", m_maxCount), 0, false, 0);
			m_count = m_maxCount;
		}
	}
}
