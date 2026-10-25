// JP-only game code: the room list forms' room-state dispatcher. See
// JpRoomListState.h.
#include "JpRoomListState.h"

#include <stdio.h>
#include <string.h>

#include "IContentsDoc.h"
#include "JpContentsGatewayDocs.h"
#include "JpGrandPrixAwardDlg.h"
#include "Intrusion.h"
#include "JpLobbyGlue.h"
#include "JpRoomListDlg.h"
#include "Packet.h"
#include "RFTextManager.h"
#include "SharedDoc.h"
#include "TaskManager.h"
#include "frlistbox.h"
#include "globalgamedefine.h"
#include "wfont.h"
#include "wresrcmng.h"
#include "ItemManager.h"
#include "bitmap.h"
#include "NetResourceManager.h"
#include "RFTooltipHelper.h"
#include "fresh.h"
#include "frgraphicinterface.h"
#include "frwndmanager.h"
#include "shared/luasystem/LuaScript.h"
#include "shared/luasystem/LuaScriptManager.h"
#include "shared/luasystem/luatablehelper.h"

// The row layout JpRoomListLayout_Load fills (JpRoomListState.h).
std::vector<JpRoomListOffset> g_roomListOffsets;  // 0xe75e38
std::vector<JpRoomListFrame> g_roomListFrames;    // 0xe75e48
std::vector<uint32_t> g_roomListColors;           // 0xe75e58

// JP callees of the multiplayer OwnerDraw (names ours, bodies below):
// 0xbed090, the JP FrGraphicInterface's title-font print (font, x, y,
// text, spacing, align, scale, colour); 0xa19320, the item description of
// a type id (NULL without one); bit 1 of the JP item's +0xc0 (the custom
// map tag; not in the port's KR sItem). JpSharedDoc_HasMapEvent is port
// glue for 0x63f830 (thiscall on the shared doc, the JP map event record of
// a course type id; +0x8 == 0 and +0xc > 0: the event mark is drawn): the
// port's shared doc has no JP map event records, false (defined below).
void JpGdi_PrintTitleFont(FrGraphicInterface* gdi, WTitleFont* font, float x, float y,
	const char* text, float spacing, int align, float scale, unsigned long color);
const char* JpItemManager_GetItemDescription(unsigned long typeId);
bool JpItem_IsCustomMapTag(const IFF_STRUCT::sItem* item);
bool JpSharedDoc_HasMapEvent(unsigned long courseTypeId);
// 0x502ed0 (fastcall): the game type's name from the shared doc (NULL
// from type 0x16 on), the KR GetGameTypeName (SharedDoc.inl, NULL from 15).
const char* GetGameTypeName(unsigned char gameType);

// JP callees of the GP handler (names ours).
// Keeps the value in a function static (0xe75e7c, guard 0xe75e80) and
// tests mask against it (fastcall).
// 0x598960
static bool JpGrandPrix_TestBits(uint32_t value, uint32_t mask) {
	static uint32_t s_lastValue;
	s_lastValue = value;
	return (mask & value) != 0;
}
// 0x55dcd0 (thiscall on CGrandPrixClock 0xe74e1c): the clock's date fields
// (+0x28..+0x58) converted through 0xb30790 on CSharedDoc +0x180. The
// conversion is not ported: 0.
uint64_t JpGrandPrixClock_GetTime(void);
// DblClick's test for sort 0x63: the record's schedule is set (the first
// dword of openTime, closeTime and endTime non-zero: wYear and wMonth).
static bool JpGrandPrixName_HasSchedule(const sJpGrandPrixName* record) {
	return (record->openTime.wYear != 0 || record->openTime.wMonth != 0)
		&& (record->closeTime.wYear != 0 || record->closeTime.wMonth != 0)
		&& (record->endTime.wYear != 0 || record->endTime.wMonth != 0);
}
// 0x51fbc0 on the JP CSharedDoc +0x10a40 (a std::list clear; no port
// member: nothing to clear).
void JpSharedDoc_ClearGrandPrixInfoList(void);

// 0xe75e30
IContentsDataContainer* JpRoomListHandler::s_lobbyData = NULL;

// 0x598120
void JpRoomListHandler::SetLobbyData(IContentsDataContainer* data) {
	s_lobbyData = data;
}

// 0xe75e34
lua_system::lua_script* JpRoomListHandler::s_script = NULL;

// 0x5980d0
void JpRoomListHandler::LoadScript(const char* file) {
	if (s_script) {
		lua_system::lua_script_manger::GetInstance()->ReleaseScript(s_script);
		s_script = NULL;
	}
	s_script = lua_system::lua_script_manger::GetInstance()->CreateScript(false);
	if (s_script) {
		s_script->RunFile(file);
	}
}

// 0x598130
lua_system::lua_script* JpRoomListHandler::GetScript(void) {
	return s_script;
}

// 0x5981d0
JpRoomListHandler::~JpRoomListHandler(void) {
	if (s_script) {
		lua_system::lua_script_manger::GetInstance()->ReleaseScript(s_script);
		s_script = NULL;
	}
}

// 0xe0f2d4: the multiplayer handler's font file name (0xd064c4).
static char* s_roomListFontFiles[] = { const_cast<char*>("[font_wind.jpg") };

// 0x599700
JpMultiplayRoomListHandler::JpMultiplayRoomListHandler(void) {
	tagWTITLEFONT font;
	memset(&font, 0, sizeof(font));
	font.filename = s_roomListFontFiles;
	font.numPages = 1;
	font.fontw = 0x10;
	font.fonth = 0x10;
	font.texw = 0x80;
	font.texh = 0x40;
	font.pCharSet = const_cast<char*>("1234567890ym%-.?/:,");
	m_numberFont = g_resrcmng->GetTitleFont();
	m_numberFont->Create(&font);
	m_tooltipItem = -1;
}

// 0x5997d0
JpMultiplayRoomListHandler::~JpMultiplayRoomListHandler(void) {
	if (g_resrcmng && m_numberFont) {
		g_resrcmng->Release(m_numberFont);
		m_numberFont = NULL;
	}
}

// 0x598200
void JpMultiplayRoomListHandler::RemoveRoom(FrListBox* list, const sRoomInfo* room) {
	if (!list || !room) {
		return;
	}
	int gameType = JpRoomListData_GetSelection(s_lobbyData);
	if (gameType != -1) {
		if (room->m_gameType != gameType) {
			return;
		}
	} else {
		int category = JpRoomListData_GetCategory(s_lobbyData);
		if (Jp_GetRoomCategory(room->m_gameType) != category && category != 4
			&& Jp_GetRoomCategory(room->m_gameType) != 4) {
			return;
		}
	}
	// The JP compares 0xdd bytes, its record size: the port's records are
	// the KR sRoomInfo.
	list->DelItem(const_cast<sRoomInfo*>(room), sizeof(sRoomInfo));
	int count = list->GetCurrentItemSize(true);
	if (count >= 6) {
		return;
	}
	int missing = 6 - count;
	do {
		list->AddItem(NULL);
	} while (--missing);
}

// 0x5988a0
void JpMultiplayRoomListHandler::AddRoom(FrListBox* list, const sRoomInfo* room) {
	if (!list) {
		return;
	}
	if (JpRoomInfo_HasSortFlag(room) || (unsigned int)(room->m_roomMode - 0x14) <= 1) {
		return;
	}
	int gameType = JpRoomListData_GetSelection(s_lobbyData);
	int category = JpRoomListData_GetCategory(s_lobbyData);
	if (gameType != -1) {
		if (room->m_gameType != gameType) {
			return;
		}
	} else if (Jp_GetRoomCategory(room->m_gameType) != category && category != 4
		&& Jp_GetRoomCategory(room->m_gameType) != 4) {
		return;
	}
	list->AddItem(const_cast<sRoomInfo*>(room));
}

// 0x598910
void JpMultiplayRoomListHandler::LBtnDown(FrRoomListDlg* dlg) {
	FrListBox* list = dlg->m_pRoomList;
	if (!list) {
		return;
	}
	// JP FrListBox +0x104 = the KR m_pItemUnderCursor (the JP SelectItem
	// 0xbefc00 keeps the selection at +0x108).
	FrListItem* item = list->GetItemUnderCursor();
	if (item) {
		list->SelectItem(item, true);
		dlg->m_pSelectedRoom = (const sRoomInfo*)item->pData;
	} else {
		list->SelectItem(NULL, true);
		dlg->m_pSelectedRoom = NULL;
	}
}

// 0x598e70
void JpMultiplayRoomListHandler::RBtnUp(FrRoomListDlg* dlg) {
	FrListBox* list = dlg->m_pRoomList;
	if (!list) {
		return;
	}
	FrListItem* item = list->GetItemUnderCursor();  // JP +0x104
	if (!item) {
		return;
	}
	const sRoomInfo* room = (const sRoomInfo*)item->pData;
	list->SelectItem(item, true);
	if (!room) {
		return;
	}
	// JP +0x40 = the KR m_isPublic, +0x59 = m_roomNumber; JP CSharedDoc
	// +0x812 = the KR m_userInfo.identity (bit 2: administrator).
	if (!room->m_isPublic && !((WSingleton<CSharedDoc>::Instance()->m_userInfo.identity >> 2) & 1)) {
		// 0xd0c5a8: a private room's information cannot be viewed.
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText(
			"\xba\xf1\xb0\xf8\xb0\xb3 \xb9\xe6\xc0\xba \xb7\xeb\xc1\xa4\xba\xb8\xb8\xa6 \xba\xbc\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9"),
			0, 0, 0, 0));
	} else {
		// The room information request.
		WSendPacket packet(0x2d);
		packet.Encode2(room->m_roomNumber);
		packet.Send(SEND_TO_DEFAULT);
	}
	dlg->m_pSelectedRoom = room;
}

// 0x598990
JpGrandPrixRoomListHandler::JpGrandPrixRoomListHandler(void) {
	m_pFont188 = NULL;
	m_pFont18c = NULL;
	memset(m_uiOffsets, 0, sizeof(sUiOffset) * 18);
	m_uiOffsets[18].x = 0.0f;
	m_uiOffsets[18].y = 0.0f;
	m_uiOffsets[19].x = 0.0f;
	m_uiOffsets[19].y = 0.0f;
}

// 0x598a00
JpGrandPrixRoomListHandler::~JpGrandPrixRoomListHandler(void) {
}

// 0x598a30: selects the GP room list's item under the cursor (none: no
// selection; JP FrListBox +0x104 = the KR GetItemUnderCursor).
void JpGrandPrixRoomListHandler::LBtnDown(FrRoomListDlg* dlg) {
	FrListBox* list = dlg->m_pAltRoomList;
	FrListItem* item = list->GetItemUnderCursor();
	if (item) {
		list->SelectItem(item, true);
	} else {
		list->SelectItem(NULL, true);
	}
}

// 0x598f90: hands the room to the "Gateway" actor (0xe0f284) of the
// current task: MsgObject(0, 0x753e, 0xb, room) through vfn +0x2c.
void JpGrandPrixRoomListHandler::AddRoom(FrListBox* list, const sRoomInfo* room) {
	(void)list;
	AfxGetTask()->PassMsgToActor("Gateway", MsgObject(0, 0x753e, 0xb, (intptr_t)room, 0, 0, 0));
}

// 0x599000: likewise with 0xc (the room's removal).
void JpGrandPrixRoomListHandler::RemoveRoom(FrListBox* list, const sRoomInfo* room) {
	(void)list;
	AfxGetTask()->PassMsgToActor("Gateway", MsgObject(0, 0x753e, 0xc, (intptr_t)room, 0, 0, 0));
}

// The join request of a grand prix entry (fastcall, name ours): Gateway
// message 7 with the entry's state and record and the room list's two
// lobby fonts.
struct sJpGrandPrixJoin {
	int state;                                 // 0x0
	sJpGrandPrixName* record;                  // 0x4
	WTitleFontEx* font15c;                     // 0x8
	WTitleFontEx* font158;                     // 0xc
};
// 0x599070
static void JpGrandPrix_SendJoin(const sJpGrandPrixEntry* entry, WTitleFontEx* font15c, WTitleFontEx* font158) {
	sJpGrandPrixJoin join;
	join.state = entry->state;
	join.record = entry->record;
	join.font15c = font15c;
	join.font158 = font158;
	AfxGetTask()->PassMsgToActor("Gateway", MsgObject(0, 0x753e, 7, (intptr_t)&join, 0, 0, 0));
}

// Gateway message 8: the entry cannot be joined (the gateway says why).
static void JpGrandPrix_SendRefusal(sJpGrandPrixEntry* entry) {
	AfxGetTask()->PassMsgToActor("Gateway", MsgObject(0, 0x753e, 8, (intptr_t)entry, 0, 0, 0));
}

// The JP writes every pair the tables have (past m_uiOffsets' 20 into the
// rest of the handler); the port stops at UI_OFFSET_COUNT.
// 0x5998a0
void JpGrandPrixRoomListHandler::LoadUiOffsets(FrListBox* list) {
	lua_system::lua_script* script = GetScript();
	if (!script) {
		return;
	}
	{
		lua_system::lua_table_helper table = script->GetTable("grandprixuioffset");
		int count = table.GetArraySize();
		for (int i = 1; i <= count && i <= UI_OFFSET_COUNT; ++i) {
			float x;
			{
				lua_system::lua_table_helper entry(table.GetValueByIndex<lua_tinker::table>(i));
				x = entry.GetValueByIndex<float>(1);
			}
			float y;
			{
				lua_system::lua_table_helper entry(table.GetValueByIndex<lua_tinker::table>(i));
				y = entry.GetValueByIndex<float>(2);
			}
			m_uiOffsets[i - 1].x = x;
			m_uiOffsets[i - 1].y = y;
		}
	}
	// JP 0xbf09d0: FrListBox::SetItemSize (out of line in the JP Fresh).
	if (JpSharedDoc_GetRoomSort() != 0x63) {
		list->SetItemSize((int)m_uiOffsets[18].x, (int)m_uiOffsets[18].y);
		return;
	}
	list->SetItemSize((int)m_uiOffsets[19].x, (int)m_uiOffsets[19].y);
	lua_system::lua_table_helper table = GetScript()->GetTable("gpeventlist");
	int count = table.GetArraySize();
	for (int i = 1; i <= count && i <= UI_OFFSET_COUNT; ++i) {
		float x;
		{
			lua_system::lua_table_helper entry(table.GetValueByIndex<lua_tinker::table>(i));
			x = entry.GetValueByIndex<float>(1);
		}
		float y;
		{
			lua_system::lua_table_helper entry(table.GetValueByIndex<lua_tinker::table>(i));
			y = entry.GetValueByIndex<float>(2);
		}
		m_uiOffsets[i - 1].x = x;
		m_uiOffsets[i - 1].y = y;
	}
}

// 0x5990f0: the join of the entry under the cursor (JP +0x104).
void JpGrandPrixRoomListHandler::DblClick(FrRoomListDlg* dlg) {
	FrListItem* item = dlg->m_pAltRoomList->GetItemUnderCursor();
	if (!item) {
		return;
	}
	sJpGrandPrixEntry* entry = (sJpGrandPrixEntry*)item->pData;
	if (!entry) {
		return;
	}
	entry->flags &= ~0x180u;
	const sJpGrandPrixName* record = entry->record;
	uint32_t sort = (record->flags >> 19) & 0x7f;
	bool joinable = true;
	if (sort == 0) {
		joinable = false;
		if (JpGrandPrix_TestBits(entry->value20, 4)) {
			entry->flags |= 0x100;
			JpGrandPrix_SendRefusal(entry);
			return;
		}
	} else if (sort == 0x63 && !JpGrandPrixName_HasSchedule(record)) {
		joinable = false;
	}
	if (!entry->flag40) {
		return;
	}
	if (joinable) {
		uint64_t clockTime = JpGrandPrixClock_GetTime();
		JpGrandPrixEntry_Update(entry, clockTime, 0);
	}
	if (joinable && entry->state != 2) {
		JpGrandPrix_SendJoin(entry, dlg->m_pFont15c, dlg->m_pFont158);
		return;
	}
	if (JpGrandPrix_TestBits(entry->flags, 3)) {
		JpGrandPrix_SendRefusal(entry);
		return;
	}
	if (JpGrandPrix_TestBits(entry->flags, 0xc)) {
		JpGrandPrix_SendRefusal(entry);
		return;
	}
	if (JpGrandPrix_TestBits(entry->flags, 0x10) && !JpGrandPrix_HasRequiredItem(entry->record->flags)) {
		JpGrandPrix_SendRefusal(entry);
		return;
	}
	if (!JpGrandPrixEntry_HasRequiredItemCount(entry)) {
		JpGrandPrix_SendRefusal(entry);
		return;
	}
	if (entry->state == 2) {
		JpGrandPrix_SendJoin(entry, dlg->m_pFont15c, dlg->m_pFont158);
	}
}

// 0x59a250
void JpMultiplayRoomListHandler::Attach(FrRoomListDlg* dlg) {
	FrListBox* list = dlg->m_pRoomList;
	if (!list) {
		return;
	}
	list->ClearItem();
	// The lobby container's +0x4 (a game type, -1 none) and +0x8 (the
	// category); the room record fields as in FrRoomListDlg::FillRoomList.
	int gameType = JpRoomListData_GetSelection(s_lobbyData);
	const std::list<sRoomInfo>& rooms = JpSharedDoc_GetRoomList();
	if (gameType == -1) {
		int category = JpRoomListData_GetCategory(s_lobbyData);
		for (std::list<sRoomInfo>::const_iterator i = rooms.begin(); i != rooms.end(); ++i) {
			const sRoomInfo& room = *i;
			if (!JpRoomInfo_HasSortFlag(&room) && (unsigned int)(room.m_roomMode - 0x14) > 1
				&& (Jp_GetRoomCategory(room.m_gameType) == category || category == 4)) {
				list->AddItem(const_cast<sRoomInfo*>(&room));
			}
		}
	} else {
		for (std::list<sRoomInfo>::const_iterator i = rooms.begin(); i != rooms.end(); ++i) {
			const sRoomInfo& room = *i;
			if (!JpRoomInfo_HasSortFlag(&room) && (unsigned int)(room.m_roomMode - 0x14) > 1
				&& room.m_gameType == gameType) {
				list->AddItem(const_cast<sRoomInfo*>(&room));
			}
		}
	}
	m_tooltipItem = -1;
}

// 0x59a390: a GP room's information (packet 0x2d), else the grand prix's
// (main-actor message 0x34 with the record id, after clearing the JP
// CSharedDoc +0x10a40 list).
void JpGrandPrixRoomListHandler::RBtnUp(FrRoomListDlg* dlg) {
	FrListBox* list = dlg->m_pAltRoomList;
	FrListItem* item = list->GetItemUnderCursor();  // JP +0x104
	if (!item) {
		return;
	}
	sJpGrandPrixEntry* entry = (sJpGrandPrixEntry*)item->pData;
	if (!entry) {
		return;
	}
	list->SelectItem(item, true);
	if (entry->room) {
		WSendPacket packet(0x2d);
		packet.Encode2(entry->room->m_roomNumber);
		packet.Send(SEND_TO_DEFAULT);
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
		return;
	}
	JpSharedDoc_ClearGrandPrixInfoList();
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x34, 1, (intptr_t)entry->record->id, 0, 0, 0));
}

// 0x59a600: one row of the multiplayer room list (the JP form of KR
// CLobbyMain::OnRoomList_RoomListOwnerDraw, its positions and colours from
// the row layout JpRoomListLayout_Load always sizes: 17 offsets, 3 frames,
// 6 colours). param is the FrListItem; JP room record fields as the KR
// sRoomInfo (+0x10 from m_isPublic on): +0x40 m_isPublic, +0x41
// m_intrusionAccess, +0x42 m_state, +0x43 m_userLimit, +0x44 m_userCount,
// +0x57 m_holeCount, +0x58 m_gameType, +0x59 m_roomNumber, +0x5c m_course,
// +0x65 m_trophy, +0x6a m_option[1], +0x6b/+0x6f m_guild.m_guildUID[0]/[1],
// +0x73/+0x7f m_guild.m_guildMark[0]/[1], +0xc4 m_roomMode; +0xbf, +0xc5
// and +0xc9 are JP-only (JpRoomInfo_* glue). The JP centres the light,
// lock and course images in the list's item height (JP FrListBox +0xe4).
// JP text keys go through Jp_LookupText 0xb4eb80.
void JpMultiplayRoomListHandler::OwnerDraw(intptr_t param, FrRoomListDlg* dlg) {
	FrListItem* item = (FrListItem*)param;
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi) {
		return;
	}
	const sRoomInfo* room = (const sRoomInfo*)item->pData;
	int state = 0;
	int color = 1;
	if (room) {
		color = 0;
		if (item->selected) {
			state = 2;
		} else if (item->underCursor) {
			state = 1;
		}
		if (Jp_GetRoomCategory(room->m_gameType) != 1) {
			if (Jp_GetRoomCategory(room->m_gameType) == 4) {
				color = 4;
			}
		} else {
			color = (room->m_roomMode != 0x12) + 2;
		}
	}

	const JpRoomListOffset* offsets = &g_roomListOffsets[0];
	float x = item->pos.x;
	float y = item->pos.y;
	for (int i = 0; i < 3; ++i) {
		const JpRoomListFrame& frame = g_roomListFrames[i];
		gdi->DrawTexture(dlg->m_roomRowImages[i][state],
			WRect(x + frame.x, y + frame.y, frame.w, frame.h), g_roomListColors[color], 0);
	}
	if (!room) {
		return;
	}
	if (item->selected) {
		for (int i = 0; i < 3; ++i) {
			const JpRoomListFrame& frame = g_roomListFrames[i];
			gdi->DrawTexture(dlg->m_roomRowImages[i][3],
				WRect(x + frame.x, y + frame.y, frame.w, frame.h), 0xFFFFFFFF, 0);
		}
	}

	// The intrusion light: red when closed or full, else yellow in game,
	// green waiting.
	const char* lightName;
	if (!room->m_intrusionAccess || room->m_userCount >= room->m_userLimit) {
		lightName = "room_light_red";
	} else if (room->m_state) {
		lightName = "room_light_yellow";
	} else {
		lightName = "room_light_green";
	}
	const Bitmap* light = g_pFresh->GetBitmap(lightName);
	FrListBox* list = dlg->m_pRoomList;
	if (room->m_intrusionAccess && light) {
		float top = (float)(int)((float)(list->GetItemHeight() - (int)light->Height()) * 0.5f);
		gdi->DrawTexture(light, WRect(offsets[12].x + x, top + offsets[12].y + y,
			(float)light->Width(), (float)light->Height()), 0xFFFFFFFF, 0);
	}
	if (!room->m_isPublic) {
		const Bitmap* lock = g_pFresh->GetBitmap("room_lock");
		if (lock) {
			float top = (float)(int)((float)(list->GetItemHeight() - (int)lock->Height()) * 0.5f);
			gdi->DrawTexture(lock, WRect(offsets[7].x + x - (float)lock->Width(), top + offsets[7].y + y,
				(float)lock->Width(), (float)lock->Height()), 0xFFFFFFFF, 0);
		}
	}

	// The custom map tag (its item name and description as a tooltip while
	// the cursor is on it) and the custom map's small icon.
	unsigned long customMap = JpRoomInfo_GetCustomMapId(room);
	if (customMap) {
		IFF_STRUCT::sItem* mapItem = ItemManager()->FindItem(customMap);
		if (mapItem && JpItem_IsCustomMapTag(mapItem)) {
			const Bitmap* tag = g_pFresh->GetBitmap("roomlist_custommap_tag");
			if (tag) {
				WRect rect(offsets[0].x + x, offsets[0].y + y, (float)tag->Width(), (float)tag->Height());
				if (item->underCursor && rect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
					const char* description = JpItemManager_GetItemDescription(customMap);
					if (!description) {
						description = "";
					}
					m_tooltipText = MakeStr("<font color=\"#ffaa00\">%s</font>\n%s", mapItem->m_name, description);
					if (RFTooltipHelper::Instance() != NULL) {
						RFTooltipHelper::Instance()->ShowTooltip(NULL, m_tooltipText, 1, 0xb);
						m_tooltipItem = item->idx;
					}
				} else if (item->idx == m_tooltipItem && !m_tooltipText.empty()) {
					m_tooltipText.clear();
					if (RFTooltipHelper::Instance() != NULL) {
						RFTooltipHelper::Instance()->HideTooltip();
						m_tooltipItem = -1;
					}
				}
				gdi->DrawTexture(tag, rect, 0xFFFFFFFF, 0);
			}
			const Bitmap* icon = g_pFresh->GetBitmap(MakeStr("s_%s", mapItem->m_icon));
			if (icon) {
				WRect rect(offsets[1].x + x, y + offsets[1].y, (float)icon->Width(), (float)icon->Height());
				if (item->underCursor) {
					rect.x -= 1.0f;
				}
				gdi->DrawTexture(icon, rect, 0xFFFFFFFF, 0);
			}
		}
	}

	// The room number, or the event mark of a lounge-sized event room.
	if (room->m_gameType == 0xb || room->m_option[1] == 0 || room->m_userLimit <= 99) {
		JpGdi_PrintTitleFont(gdi, m_numberFont, offsets[3].x + x, y + offsets[3].y,
			MakeStr("%03d", room->m_roomNumber), 1.0f, 0, 1.0f, 0xFFFFB258);
	} else {
		const Bitmap* eventMark = g_pFresh->GetBitmap("room_event");
		if (eventMark) {
			gdi->DrawTexture(eventMark, WRect(offsets[2].x + x, y + offsets[2].y,
				(float)eventMark->Width(), (float)eventMark->Height()), 0xFFFFFFFF, 0);
		}
		gdi->SetTextStyle(1);
	}
	gdi->SetTextColor(0xFF000000, 0xFFFFFFFF);
	const char* category = JpRoomInfo_GetCategoryText(room);
	// Port: the KR PrintText takes no NULL text (the JP passes NULL for
	// game types 3, 8 and above 11).
	if (category) {
		gdi->PrintText(WPoint(offsets[4].x + x, offsets[4].y + y), 0, category, NULL);
	}

	WPoint typePos(offsets[6].x + x, offsets[6].y + y);
	switch (room->m_gameType) {
	case 6:
		if (room->m_guild.m_guildUID[0]) {
			const Bitmap* emblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(room->m_guild.m_guildMark[0]);
			if (emblem) {
				gdi->DrawTexture(emblem, WRect(offsets[9].x + x, offsets[9].y + y,
					(float)emblem->Width(), (float)emblem->Height()), 0xFFFFFFFF, 0);
			}
		}
		if (room->m_guild.m_guildUID[1]) {
			const Bitmap* emblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(room->m_guild.m_guildMark[1]);
			if (emblem) {
				gdi->DrawTexture(emblem, WRect(offsets[10].x + x, offsets[10].y + y,
					(float)emblem->Width(), (float)emblem->Height()), 0xFFFFFFFF, 0);
			}
		}
		gdi->Print(typePos, 0, "%s", GetGameTypeName(room->m_gameType));
		break;
	case 9:
		gdi->Print(typePos, 0, "%s", GetGameTypeName(room->m_gameType));
		break;
	case 11:
		gdi->Print(typePos, 0, "%s", Jp_LookupText(room->m_roomMode == 0xd
			? "\xBB\xF3\xB1\xDE" : "\xC1\xDF\xB1\xDE"));
		break;
	default: {
		IFF_STRUCT::sMatch* match = ItemManager()->FindMatch(room->m_trophy);
		const char* team = room->m_gameType != 5 ? "" : Jp_LookupText("\xC6\xC0\xC0\xFC");
		if (match) {
			char name[80] = { 0 };
			if (room->m_roomMode == 0x12) {
				strcpy(name, Jp_LookupText("\xBC\xC5\xC7\xC3\xC4\xDA\xBD\xBA"));
			} else {
				strcpy(name, match->m_name);
			}
			gdi->Print(typePos, 0, "%s %s", name, team);
		} else {
			gdi->Print(typePos, 0, "%s %s", GetGameTypeName(room->m_gameType), team);
		}
		break;
	}
	}

	if (JpRoomInfo_IsGpHost(room)) {
		const Bitmap* host = g_pFresh->GetManager()->GetBitmap("LEVELS", "gp_host");
		if (host) {
			gdi->DrawTexture(host, WRect(offsets[5].x + x, offsets[5].y + y,
				(float)host->Width(), (float)host->Height()), 0xFFFFFFFF, 0);
		}
	}
	gdi->SetTextStyle(0);

	const char* title = NULL;
	if (room->m_gameType == 6) {
		title = Jp_LookupText("            VS             ");
	} else if (room->m_gameType == 0xb) {
		if (room->m_roomMode == 0xb) {
			title = Jp_LookupText("\xC8\xA6\xC0\xCE\xBF\xF8 \xC0\xCC\xBA\xA5\xC6\xAE(\xC1\xDF\xB1\xDE)");
		} else if (room->m_roomMode == 0xd) {
			title = Jp_LookupText("\xC8\xA6\xC0\xCE\xBF\xF8 \xC0\xCC\xBA\xA5\xC6\xAE(\xBB\xF3\xB1\xDE)");
		}
	} else {
		title = room->m_title;
	}
	// Port: as the category, no NULL title to the KR PrintText (the JP
	// passes NULL for a hole-in-one room of another mode).
	if (title) {
		g_pFresh->GetManager()->PrintText(WPoint(offsets[8].x + x, offsets[8].y + y), 0, title, -1.0f, 0xFFFFFFFF);
	}

	// The course image, tinted with colour 0 open to intrusion, else 5.
	int courseColor = room->m_intrusionAccess ? 0 : 5;
	float courseX = offsets[14].x + x;
	if (room->m_roomMode == 0x12) {
		if (ItemManager()->FindCourse(0x2800007F)) {
			const Bitmap* courseBmp = g_pFresh->GetBitmap("room_map_17_01");
			if (courseBmp) {
				float top = (float)(int)((float)(list->GetItemHeight() - (int)courseBmp->Height()) * 0.5f);
				gdi->DrawTexture(courseBmp, WRect(courseX, top + y, (float)courseBmp->Width(),
					(float)courseBmp->Height()), g_roomListColors[courseColor], 0);
			}
		}
	} else {
		IFF_STRUCT::sCourse* course = ItemManager()->FindCourse(0x28000000 | (room->m_course < 0x7F ? room->m_course : 0x7F));
		if (course) {
			const Bitmap* courseBmp = g_pFresh->GetBitmap(MakeStr("room_%s", course->m_icon));
			if (courseBmp) {
				float top = (float)(int)((float)(list->GetItemHeight() - (int)courseBmp->Height()) * 0.5f);
				gdi->DrawTexture(courseBmp, WRect(courseX, top + y, (float)courseBmp->Width(),
					(float)courseBmp->Height()), g_roomListColors[courseColor], 0);
				if (JpSharedDoc_HasMapEvent(course->m_typeId & 0x3FFFFFF)) {
					const Bitmap* mark = g_pFresh->GetBitmap("lobby_mapevent_mark");
					if (mark) {
						gdi->DrawTexture(mark, WRect(courseX + (float)(int)(courseBmp->Width() - mark->Width()), y,
							(float)mark->Width(), (float)mark->Height()), 0xFFFFFFFF, 0);
					}
				}
			}
		}
	}

	gdi->Print(WPoint(offsets[11].x + x, offsets[11].y + y), 2, Jp_LookupText("%d\xC8\xA6"), room->m_holeCount);
	gdi->Print(WPoint(offsets[13].x + x, offsets[13].y + y), 2, "%d/%d", room->m_userCount, room->m_userLimit);

	if (room->m_gameType != 0x11) {
		CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
		if (doc->m_curServer.m_flags2 & 0x20) {
			unsigned char courseBit = (room->m_course > 0x7F && room->m_course != 0xFD) ? room->m_course - 0x80 : room->m_course;
			if (doc->m_curServer.eventCourseMask & (1 << (courseBit & 0x1F))) {
				const Bitmap* icon = g_pFresh->GetBitmap("icon_pangevent");
				gdi->DrawTexture(icon, WRect(offsets[15].x + x, offsets[15].y + y,
					(float)icon->Width(), (float)icon->Height()), 0xFFFFFFFF, 0);
			}
		}
	}
	if (JpRoomInfo_IsClassicRoom(room)) {
		const Bitmap* icon = g_pFresh->GetBitmap("icon_classic_room");
		gdi->DrawTexture(icon, WRect(offsets[16].x + x, offsets[16].y + y,
			(float)icon->Width(), (float)icon->Height()), 0xFFFFFFFF, 0);
	}
}

// 0x59b980: the join (the JP form of KR CLobbyMain::OnRoomList_RoomListDClick).
// JP room record fields as the KR sRoomInfo (+0x10 from m_isPublic on):
// +0x41 m_intrusionAccess, +0x42 m_state, +0x43 m_userLimit, +0x44
// m_userCount, +0x6b/+0x6f m_guild.m_guildUID[0]/[1]. JP CSharedDoc +0x812
// = the KR m_userInfo.identity, +0x91d = m_myLevel, +0x82a =
// m_userInfo.guildUID, +0x109d4 = m_userMap, +0x108f4 = m_roomInfo (0xdc
// bytes + 1 copied: the JP record). Message boxes go to the current task's
// main actor (vfn +0x30, message 0x23); the join messages to the "Lobby"
// actor (vfn +0x2c).
void JpMultiplayRoomListHandler::DblClick(FrRoomListDlg* dlg) {
	if (!dlg->m_pSelectedRoom) {
		return;
	}
	if (dlg->m_pRoomList && !dlg->m_pRoomList->IsEnabled()) {
		return;
	}
	WSingleton<CIntrusion>::Instance()->FinishGame();
	const sRoomInfo* room = dlg->m_pSelectedRoom;
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if (!room->m_intrusionAccess) {
		if (room->m_state) {
			doc->m_roomInfo = *room;
			WSingleton<CIntrusion>::Instance()->DoJoinRoom(doc->m_roomInfo.m_roomNumber);
			return;
		}
		// 0xd0c99c: the game is in progress.
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText(
			"\xb0\xd4\xc0\xd3\x20\xc1\xf8\xc7\xe0\xc1\xdf\xc0\xce\x20\xb9\xe6\xc0\xd4\xb4\xcf\xb4\xd9"),
			0, 0, 0, 0));
		return;
	}
	if (!((doc->m_userInfo.identity >> 2) & 1)) {
		if (room->m_userCount == room->m_userLimit) {
			// 0xd0caa0: the room is full.
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText(
				"\xc1\xa4\xbf\xf8\xc0\xcc\x20\xc3\xca\xb0\xfa\x20\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9"),
				0, 0, 0, 0));
			return;
		}
		switch (room->m_gameType) {
		case 10:
			if (doc->m_myLevel == 0) {
				// 0xd0ca70: rookie F cannot play an approach game.
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText(
					"\xb7\xe7\xc5\xb0\x46\x20\xb7\xb9\xba\xa7\xc0\xba\x20\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1\x20\xb0\xd4\xc0\xd3\xc0\xbb\x20\xc7\xd2\x20\xbc\xf6\x20\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9\x2e"),
					0, 0, 0, 0));
				return;
			}
			break;
		case 5: {
			std::map<DWORD, sBriefUserInfo>::iterator it = doc->m_userMap.find(MyGuid(false));
			if (it != doc->m_userMap.end() && it->second.charType > 1) {
				// 0xd0c9f8: a high-completion user cannot play a 30-player team game.
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText(
					"\xb0\xad\xc1\xa6\x20\xc1\xbe\xb7\xe1\xc0\xb2\xc0\xcc\x20\xb3\xf4\xc0\xba\x20\xc0\xaf\xc0\xfa\xb4\xc2\x20\x33\x30\xc0\xce\x20\xc6\xc0\xc0\xfc\x20\xb0\xd4\xc0\xd3\xc0\xbb\x20\xc7\xd2\x20\xbc\xf6\x20\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9\x2e"),
					0, 0, 0, 0));
				return;
			}
		}
			// fall through: the level test of type 4
		case 4:
			if (doc->m_myLevel == 0) {
				// 0xd0ca34: rookie F cannot play a 30-player match or team game.
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText(
					"\xb7\xe7\xc5\xb0\x46\x20\xb7\xb9\xba\xa7\xc0\xba\x20\x33\x30\xc0\xce\xb4\xeb\xc0\xfc\x20\xb9\xd7\x20\x33\x30\xc0\xce\xc6\xc0\xc0\xfc\x20\xb0\xd4\xc0\xd3\xc0\xbb\x20\xc7\xd2\x20\xbc\xf6\x20\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9\x2e"),
					0, 0, 0, 0));
				return;
			}
			break;
		case 6: {
			std::string message;
			unsigned long guildUID = doc->m_userInfo.guildUID;
			bool refuse = false;
			if (guildUID == 0) {
				// 0xd0c9cc: join a guild to play a guild match.
				message = Jp_LookupText(
					"\xb1\xe6\xb5\xe5\xbf\xa1\x20\xb0\xa1\xc0\xd4\xc7\xcf\xbc\xc5\xbe\xdf\x20\xb1\xe6\xb5\xe5\xb4\xeb\xc0\xfc\xc0\xcc\x20\xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9\x2e");
				refuse = true;
			}
			if (room->m_guild.m_guildUID[0] != guildUID && room->m_guild.m_guildUID[0] != 0
				&& room->m_guild.m_guildUID[1] != guildUID && room->m_guild.m_guildUID[1] != 0) {
				// 0xd0c9b4: another guild's match.
				message = Jp_LookupText("\xc5\xb8\xb1\xe6\xb5\xe5\xc0\xc7\x20\xb4\xeb\xc0\xfc\xc0\xd4\xb4\xcf\xb4\xd9\x2e");
				refuse = true;
			}
			if (refuse) {
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)message.c_str(), 0, 0, 0, 0));
				return;
			}
			break;
		}
		}
	}
	dlg->EnableRoomEnterControls(false);
	if ((doc->m_userInfo.identity & 0xe) == 0xe) {
		// The observer join.
		dlg->EnableRoomEnterControls(false);
		WSendPacket packet(0x3e);
		packet.Encode2(room->m_roomNumber);
		packet.EncodeStr(std::string(""));
		packet.Send(SEND_TO_DEFAULT);
		AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 0x16, 0, 0, 0, 0, 0));
		AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 1, 0, 0, 0, 0, 0));
		return;
	}
	bool manager = (doc->m_userInfo.identity & 0x14) != 0;
	// JP content 0x55 and CSharedDoc +0x11380: the KR content 0x54 and
	// IsInviteMode (the invited player's join).
	bool invited = false;
	if (IsLocalContent((localContentType_t)0x55) && doc->IsInviteMode()) {
		invited = true;
	}
	if (room->m_isPublic || manager || invited) {
		// JP content 0x2f: the KR 0x2e (joining a game in progress).
		if (IsLocalContent((localContentType_t)0x2f) && room->m_state == 1 && !manager) {
			WSingleton<CIntrusion>::Instance()->DoJoinRoom(room->m_roomNumber);
			return;
		}
		WSendPacket packet(9);
		packet.Encode2(room->m_roomNumber);
		packet.EncodeStr(std::string(""));
		packet.Send(SEND_TO_DEFAULT);
		AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 0x16, 0, 0, 0, 0, 0));
		AfxGetTask()->PassMsgToActor("Lobby", MsgObject(0, 1, 0, 0, 0, 0, 0));
	} else {
		// A private room: the main actor opens the password form (message
		// 0x8b, 3).
		doc->m_roomInfo = *room;
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x8b, 3, 0, 0, 0, 0));
	}
}

// 0x59c0c0: lists the room-sort container's entries for the GP clock's
// time and selects the first one in state 2, else in state 0, else the
// current grand prix's (0x599500 finds it by record id).
void JpGrandPrixRoomListHandler::Attach(FrRoomListDlg* dlg) {
	FrListBox* list = dlg->m_pAltRoomList;
	list->ClearItem();
	m_dw190 = -1;
	m_pFont188 = dlg->m_pFont158;
	m_pFont18c = dlg->m_pFont15c;
	LoadUiOffsets(list);
	IContentsDataContainer* data = NULL;
	_gateway::_grandprix::CGrandPrixDocument* doc = NULL;
	if (WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0xaf, data)) {
		doc = static_cast<_gateway::_grandprix::CGrandPrixDocument*>(data);
	}
	uint64_t clockTime = JpGrandPrixClock_GetTime();
	std::vector<sJpGrandPrixEntry*> entries;
	doc->CollectEntries(entries, clockTime);
	for (std::vector<sJpGrandPrixEntry*>::iterator i = entries.begin(); i != entries.end(); ++i) {
		list->AddItem(*i);
	}
	FrListItem* selected = NULL;
	for (FrListBox::ITEM_LIST::iterator i = list->m_itemList.begin(); i != list->m_itemList.end(); ++i) {
		FrListItem* item = *i;
		if (item && item->pData && ((sJpGrandPrixEntry*)item->pData)->state == 2) {
			selected = item;
			break;
		}
	}
	if (!selected) {
		for (FrListBox::ITEM_LIST::iterator i = list->m_itemList.begin(); i != list->m_itemList.end(); ++i) {
			FrListItem* item = *i;
			if (item && item->pData && ((sJpGrandPrixEntry*)item->pData)->state == 0) {
				selected = item;
				break;
			}
		}
	}
	if (!selected) {
		uint32_t id = doc->GetCurrentEntryId();
		for (FrListBox::ITEM_LIST::iterator i = list->m_itemList.begin(); i != list->m_itemList.end(); ++i) {
			FrListItem* item = *i;
			const sJpGrandPrixEntry* entry = item ? (const sJpGrandPrixEntry*)item->pData : NULL;
			if (entry && entry->record && entry->record->id == id) {
				selected = item;
				break;
			}
		}
	}
	if (selected) {
		list->SelectItem(selected, true);
	}
}

// 0x59dbe0: the room list row layout from "roomlist_layout_ui.lua" (JP
// 0xc1ff80 the script manager, 0xc1feb0 CreateScript, 0xc20530 RunFile,
// 0xc20240 GetTable, 0x4d5250 a table entry as a table, 0x49a000 /
// 0x46e1f0 the float reads, 0x483530 the string read, 0xc1fd50
// ReleaseScript). The JP sizes the three vectors (17 offsets, 3 frames, 6
// colours) only once the script exists, and the row drawing indexes them
// unchecked: the port sizes them first, so a missing script leaves zeroed
// entries instead of empty vectors.
void JpRoomListLayout_Load(void) {
	g_roomListOffsets.resize(0x11);
	g_roomListFrames.resize(3);
	g_roomListColors.resize(6);
	lua_system::lua_script* script = lua_system::lua_script_manger::GetInstance()->CreateScript(false);
	if (!script) {
		return;
	}
	script->RunFile("roomlist_layout_ui.lua");
	{
		lua_system::lua_table_helper table = script->GetTable("roomlist_layout_ui_offset");
		int count = table.GetArraySize();
		for (int i = 1; i <= count; ++i) {
			float x;
			{
				lua_system::lua_table_helper entry(table.GetValueByIndex<lua_tinker::table>(i));
				x = entry.GetValueByIndex<float>(1);
			}
			float y;
			{
				lua_system::lua_table_helper entry(table.GetValueByIndex<lua_tinker::table>(i));
				y = entry.GetValueByIndex<float>(2);
			}
			g_roomListOffsets[i - 1].x = x;
			g_roomListOffsets[i - 1].y = y;
		}
	}
	{
		lua_system::lua_table_helper table = script->GetTable("roomlist_layout_ui_base_frame");
		int count = table.GetArraySize();
		for (int i = 1; i <= count; ++i) {
			float value[4];
			for (int k = 0; k < 4; ++k) {
				lua_system::lua_table_helper entry(table.GetValueByIndex<lua_tinker::table>(i));
				value[k] = entry.GetValueByIndex<float>(k + 1);
			}
			JpRoomListFrame& frame = g_roomListFrames[i - 1];
			frame.x = value[0];
			frame.y = value[1];
			frame.w = value[2];
			frame.h = value[3];
		}
	}
	{
		lua_system::lua_table_helper table = script->GetTable("roomlist_layout_ui_color");
		int count = table.GetArraySize();
		for (int i = 1; i <= count; ++i) {
			unsigned int color = 0;
			sscanf(table.GetValueByIndex<const char*>(i), "%x", &color);
			g_roomListColors[i - 1] = color;
		}
	}
	lua_system::lua_script_manger::GetInstance()->ReleaseScript(script);
}

// 0x59f190 (fastcall, name ours): the category text of a room's game type.
const char* JpRoomInfo_GetCategoryText(const sRoomInfo* room) {
	switch (room->m_gameType) {
	case 0:
	case 1:
		return Jp_LookupText("\xB4\xEB\xC0\xFC");
	case 2:
		return Jp_LookupText("\xB4\xEB\xC8\xAD");
	case 4:
	case 5:
	case 6:
		if (JpRoomInfo_IsShortGame(room)) {
			return Jp_LookupText("\xBC\xF4\xB0\xD4\xC0\xD3");
		}
		return Jp_LookupText("\xB4\xEB\xC8\xB8");
	case 7:
	case 9:
	case 10:
		return Jp_LookupText("\xB9\xE8\xC6\xB2");
	case 11:
		return Jp_LookupText("\xC8\xA6\xC0\xCE\xBF\xF8");
	}
	return NULL;
}

// 0x5a0780
JpRoomListState::JpRoomListState(IContentsDataContainer* lobbyData) : m_kind(2) {
	JpRoomListLayout_Load();
	JpRoomListHandler::SetLobbyData(lobbyData);
	m_handlers.push_back(new JpMultiplayRoomListHandler);
	m_handlers.push_back(new JpGrandPrixRoomListHandler);
}

// 0x5a05e0
JpRoomListState::~JpRoomListState(void) {
	for (std::vector<JpRoomListHandler*>::iterator i = m_handlers.begin(); i != m_handlers.end(); ++i) {
		if (*i) {
			delete *i;
		}
	}
}

// 0x5a01c0
void JpRoomListState::Attach(FrRoomListDlg* dlg) {
	if (m_kind == 2) {
		m_kind = JpSharedDoc_GetCurrentRoomTab();
	}
	m_handlers[m_kind]->Attach(dlg);
}

// 0x5a0250
void JpRoomListState::OwnerDraw(intptr_t param, FrRoomListDlg* dlg) {
	if (m_kind == 2) {
		m_kind = JpSharedDoc_GetCurrentRoomTab();
	}
	m_handlers[m_kind]->OwnerDraw(param, dlg);
}

// 0x5a0280
void JpRoomListState::LBtnDown(FrRoomListDlg* dlg) {
	if (m_kind == 2) {
		m_kind = JpSharedDoc_GetCurrentRoomTab();
	}
	m_handlers[m_kind]->LBtnDown(dlg);
}

// 0x5a02b0
void JpRoomListState::RBtnUp(FrRoomListDlg* dlg) {
	if (m_kind == 2) {
		m_kind = JpSharedDoc_GetCurrentRoomTab();
	}
	m_handlers[m_kind]->RBtnUp(dlg);
}

// 0x5a02e0
void JpRoomListState::DblClick(FrRoomListDlg* dlg) {
	if (m_kind == 2) {
		m_kind = JpSharedDoc_GetCurrentRoomTab();
	}
	m_handlers[m_kind]->DblClick(dlg);
}

// 0x5a0310
void JpRoomListState::Sort(int sort, FrRoomListDlg* dlg) {
	IContentsDataContainer* sortData = NULL;
	WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0xaf, sortData);
	static_cast<_gateway::_grandprix::CGrandPrixDocument*>(sortData)->SetSort(sort);
	Attach(dlg);
}

// The JP CSharedDoc's dwords +0x1163c (the room-list sort) and +0x11630 (the
// current room tab) have no KR member: the port holds them here (zero start
// values).
static int s_jpRoomSort = 0;
static int s_jpCurrentRoomTab = 0;

// 0x62bf20
void JpSharedDoc_SetRoomSort(int sort) {
	s_jpRoomSort = sort;
}

// 0x62bf30
int JpSharedDoc_GetRoomSort(void) {
	return s_jpRoomSort;
}

// 0x62bf40
void JpSharedDoc_SetCurrentRoomTab(int tab) {
	s_jpCurrentRoomTab = tab;
}

// 0x62bf50
int JpSharedDoc_GetCurrentRoomTab(void) {
	return s_jpCurrentRoomTab;
}

// The JP room record's +0xd9 has no field in the port's KR sRoomInfo (the
// offline server fills KR records): no room carries the flag.
bool JpRoomInfo_HasSortFlag(const sRoomInfo* room) {
	(void)room;
	return false;
}

// The other JP-only record fields (JpRoomListState.h): no port room has
// them.
bool JpRoomInfo_IsGpHost(const sRoomInfo* room) {
	(void)room;
	return false;
}

unsigned long JpRoomInfo_GetCustomMapId(const sRoomInfo* room) {
	(void)room;
	return 0;
}

bool JpRoomInfo_IsClassicRoom(const sRoomInfo* room) {
	(void)room;
	return false;
}

bool JpRoomInfo_IsShortGame(const sRoomInfo* room) {
	(void)room;
	return false;
}

// Port glue for 0x63f830 (see the declaration): no JP map event records.
bool JpSharedDoc_HasMapEvent(unsigned long courseTypeId) {
	(void)courseTypeId;
	return false;
}

// 0xbed090 (thiscall on the JP FrGraphicInterface; a JP-only member, name
// ours): prints a text with a title font, each character the glyph of its
// position in the font's character set, `spacing` added after each one;
// align 1 centres it on x, 2 ends it on x, other values start it at x. The
// font's scale is set for the print and restored after it, then the font
// is flushed. The JP measures through g_view (0xf023e8) and draws on the
// gdi's view (+0x4, protected in the port's FrGraphicInterface): the port's
// Fresh gdi is built on g_view (FrWndManager), so both are g_view here. The
// JP GetCharWidthA (0xb5e080) takes one more argument (0) than the port's.
void JpGdi_PrintTitleFont(FrGraphicInterface* gdi, WTitleFont* font, float x, float y,
	const char* text, float spacing, int align, float scale, unsigned long color) {
	(void)gdi;
	if (!font || !text || strlen(text) == 0) {
		return;
	}
	float oldScale = font->GetScale();
	const char* charSet = font->GetFontInfo()->pCharSet;
	font->SetScale(scale);
	float width = 0.0f;
	const char* p;
	if (align == 1) {
		for (p = text; *p; p++) {
			width += font->GetCharWidthA(g_view, (int)(strchr(charSet, *p) - charSet))
				+ (double)(scale * spacing);
		}
		width = (float)((width - spacing) * 0.5);
	} else if (align == 2) {
		for (p = text; *p; p++) {
			width += font->GetCharWidthA(g_view, (int)(strchr(charSet, *p) - charSet))
				+ (double)(scale * spacing);
		}
		width -= spacing;
	}
	x -= width;
	for (p = text; *p; p++) {
		x += font->PutChar(g_view, x, y, (int)(strchr(charSet, *p) - charSet), 0, color) + spacing;
	}
	font->SetScale(oldScale);
	font->Flush(g_view);
}

// 0xa19320 (thiscall on the shared doc's item manager +0x20c; name ours):
// the description text of an item type id, from the JP item manager's
// description map (+0x348, keyed by type id; the text at the node's +0x1c),
// NULL without an entry. The port's KR CItemManager has no description
// map: NULL.
const char* JpItemManager_GetItemDescription(unsigned long typeId) {
	(void)typeId;
	return NULL;
}

// Bit 1 of the JP item record's +0xc0 (the custom map tag). The port's KR
// sItem keeps 0xc0..0xc6 unknown and the JP record's layout there is not
// proven to match: false.
bool JpItem_IsCustomMapTag(const IFF_STRUCT::sItem* item) {
	(void)item;
	return false;
}

// 0x55dcd0
uint64_t JpGrandPrixClock_GetTime(void) {
	return 0;
}


// 0x51fbc0
void JpSharedDoc_ClearGrandPrixInfoList(void) {
}
