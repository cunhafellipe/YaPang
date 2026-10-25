// The JP 983 lobby's character select window (JpLobbyCharacterSelectWnd.h),
// from the JP exe's bodies (node scripts/disasm.ts --jp 0x<va>).
#include "wmath.h"

#include "JpLobbyCharacterSelectWnd.h"

#include "JpLobbyGlue.h"
#include "RFLobbyDesktop.h"

#include "RFButton.h"
#include "RFEventArgs.h"
#include "RFEventHandler.h"
#include "RefreshStringUtil.h"

#include "Packet.h"
#include "SharedDoc.h"
#include "TaskManager.h"
#include "actor.h"
#include "commonutil.h"

// 0xe781ac
RFWnd* RFLobbyCharacterSelectWnd::s_loadParent = NULL;

// The creators are RFManager.h's CreateWnd<T> (instance 0x5d2450) and
// CreateDesktopWnd<T> (instance 0x5d24e0).

// Port glue, no JP function of its own: the part of the JP LobbyPlayer
// actor's ("LobbyAvatar", docs/JP_CLIENT.md) HandleMsg 0x5693c0 case 0x192
// (block 0x5695c7, message param1 the character id) that does not touch the
// 3D avatar. The port has no LobbyPlayer (KR CLobbyTask::Register adds none),
// so GetActor("LobbyAvatar") is NULL and OnCarouselClosed's lobby-mode
// change would stop there. The JP case:
//   1. message 0x186 to itself (the avatar's motion reset; no LobbyPlayer);
//   2. CSharedDoc +0xa2a (KR m_myCharacterId) = param1;
//   3. message 0x185 to itself (the avatar rebuild; no LobbyPlayer);
//   4. 0x566f30 Jp_LobbyViewReset(0): packet 0x0b kind 4, the character id
//      (the server answers 0x0049 kind 4, CTask::OnPacketCommon ->
//      CLobbyMain message 0x49: m_myCharacterId, BuildMyPartTidList);
//   5. message 0x1a0 to "Lobby": the KR CLobbyMain has no case for it, not
//      sent.
static void LobbyAvatarMissing_ChangeCharacter(int uid) {
	WSingleton<CSharedDoc>::Instance()->m_myCharacterId = uid;
	Jp_LobbyViewReset(0);
}

// 0x5d1b30
void RFLobbyCharacterSelectWnd::OnCharacterMouseEnter(EventArgs& args) {
}

// 0x5d1b40
void RFLobbyCharacterSelectWnd::OnCharacterMouseLeave(EventArgs& args) {
}

// 0x5d1b50
void RFLobbyCharacterSelectAnimWnd::OnPlayUIAnimation(EventArgs& args) {
	m_animating = true;
}

// 0x5d2020
const char* RFLobbyCharacterSelectWnd::GetCharacterName(int character) {
	if (character >= 0xf) {
		return NULL;
	}
	return m_strings[character].c_str();
}

// 0x5d2220
RFLobbyCharacterSelectWnd::RFLobbyCharacterSelectWnd()
	: m_selectedCharacter(0xf)
	, m_carouselCharacter(0xf)
	, m_unknown330(false) {
	g_lobbyCharacterSelectWnd = this;
}

// 0x5d22b0
RFLobbyCharacterSelectWnd::~RFLobbyCharacterSelectWnd() {
	g_lobbyCharacterSelectWnd = NULL;
}

// 0x5d2980 (parent in ecx): the window from lobby_char_select_wnd.xml, made
// as g_jpWndCreateMode (0xe73dd8) says; the parent is kept in s_loadParent.
// The constructor stores the instance in g_lobbyCharacterSelectWnd.
RFWnd* CreateLobbyCharacterSelectWnd(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFLobbyCharacterSelectWnd>(&RFManager::GetInstance(), L"lobby_char_select_wnd.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFLobbyCharacterSelectWnd>(&RFManager::GetInstance(), L"lobby_char_select_wnd.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFLobbyCharacterSelectWnd>(&RFManager::GetInstance(), L"lobby_char_select_wnd.xml");
		break;
	}
	RFLobbyCharacterSelectWnd::s_loadParent = parent;
	return wnd;
}

// 0x5d2ac0
void RFLobbyCharacterSelectWnd::OnCarouselClosed(int character, bool changed) {
	if (character >= 0xf) {
		return;
	}
	if (RFLobbyDesktop::s_instance == NULL) {
		return;
	}
	bool shopState = RFLobbyDesktop::s_instance->IsShopState();
	int uid = Jp_GameState_FindCharacterUid(character | 0x4000000);
	const uint32_t* data = Jp_GameState_GetCharacter(uid);
	if (data == NULL) {
		return;
	}
	if (m_carouselCharacter != character && m_strings[character].size() != 0) {
		// 0x5b8740: GetChild then dynamic_cast (see RFLobbyDesktop::GetChildAs).
		RFButton* button = dynamic_cast<RFButton*>(GetChild(std::wstring(L"btn_character")));
		if (button != NULL) {
			std::wstring normal = ShiftJisToWString(std::string(MakeStr("char_selcet_btn_main_%s_n.tga", m_strings[character].c_str())));
			std::wstring over = ShiftJisToWString(std::string(MakeStr("char_selcet_btn_main_%s_o.tga", m_strings[character].c_str())));
			std::wstring down = ShiftJisToWString(std::string(MakeStr("char_selcet_btn_main_%s_p.tga", m_strings[character].c_str())));
			button->ChangeNormalButtonImage(normal.c_str());
			button->ChangeOverButtonImage(over.c_str());
			button->ChangeDownButtonImage(down.c_str());
			button->ChangeDisableButtonImage(normal.c_str());
		}
	}
	if (changed) {
		if (RFLobbyDesktop::s_mode == 1) {
			WSendPacket packet(0xc);
			packet.Encode1(4);
			packet.Encode4(uid);
			packet.Send(SEND_TO_DEFAULT);
		} else if (shopState) {
			IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
			if (actor != NULL) {
				actor << MsgObject(NULL, 0x187, (intptr_t)data, 0, 0, 0, 0);  // vtable +0x2c, IActor::HandleMsg
			}
			Jp_LobbyViewReset(uid);
		} else {
			IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
			if (actor != NULL) {
				actor << MsgObject(NULL, 0x192, uid, (intptr_t)data, 0, 0, 0);
			} else {
				LobbyAvatarMissing_ChangeCharacter(uid);
			}
		}
	} else if (shopState) {
		// JP CSharedDoc +0xa2a: the KR m_myCharacterId (Jp_LobbyViewReset).
		WSingleton<CSharedDoc>::Instance()->m_myCharacterId = uid;
	}
	m_unknown330 = m_selectedCharacter != character;
	m_carouselCharacter = character;
	Jp_GameState_SetShownCharacter(Jp_GameState_GetCharacterInfo(uid));
}

// 0xe781b4
RFWnd* RFLobbyCharacterSelectAnimWnd::s_loadParent = NULL;

// 0x5d2670
RFLobbyCharacterSelectAnimWnd::RFLobbyCharacterSelectAnimWnd()
	: m_open(false)
	, m_animating(false)
	, m_selectedCharacter(0xf)
	, m_shownCharacter(0xf) {
	g_lobbyCharacterCarousel = this;
}

// 0x5d2710
RFLobbyCharacterSelectAnimWnd::~RFLobbyCharacterSelectAnimWnd() {
	g_lobbyCharacterCarousel = NULL;
}

// 0x5d2ef0 (parent in ecx): the carousel from lobby_char_select_anim_wnd.xml,
// made as g_jpWndCreateMode (0xe73dd8) says; the parent is kept in
// s_loadParent. The constructor stores the instance in
// g_lobbyCharacterCarousel. The creators are RFManager.h's CreateWnd<T>
// (instance 0x5d2830) and CreateDesktopWnd<T> (instance 0x5d28c0).
RFWnd* CreateLobbyCharacterCarousel(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFLobbyCharacterSelectAnimWnd>(&RFManager::GetInstance(), L"lobby_char_select_anim_wnd.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFLobbyCharacterSelectAnimWnd>(&RFManager::GetInstance(), L"lobby_char_select_anim_wnd.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFLobbyCharacterSelectAnimWnd>(&RFManager::GetInstance(), L"lobby_char_select_anim_wnd.xml");
		break;
	}
	RFLobbyCharacterSelectAnimWnd::s_loadParent = parent;
	return wnd;
}

// 0x5d3030
void RFLobbyCharacterSelectAnimWnd::SelectCharacter(int character) {
	if (m_shownCharacter == character) {
		return;
	}
	if (m_shownCharacter != 0xf) {
		RFButton* button = dynamic_cast<RFButton*>(GetChild(std::wstring(m_strings[m_shownCharacter].c_str())));
		if (button != NULL) {
			button->SetState(RFButton::RFBUTTON_STATE_NORMAL);
		}
	}
	if (g_lobbyCharacterSelectWnd != NULL) {
		g_lobbyCharacterSelectWnd->OnCarouselClosed(character, false);
	}
	m_shownCharacter = character;
}

// 0x5d3170
void RFLobbyCharacterSelectAnimWnd::OnNuriClicked(EventArgs& args) {
	SelectCharacter(0xb);
}

// 0x5d3180
void RFLobbyCharacterSelectAnimWnd::OnNuriOldClicked(EventArgs& args) {
	SelectCharacter(0x0);
}

// 0x5d3190
void RFLobbyCharacterSelectAnimWnd::OnHanaClicked(EventArgs& args) {
	SelectCharacter(0xc);
}

// 0x5d31a0
void RFLobbyCharacterSelectAnimWnd::OnHanaOldClicked(EventArgs& args) {
	SelectCharacter(0x1);
}

// 0x5d31b0
void RFLobbyCharacterSelectAnimWnd::OnCeciliaOldClicked(EventArgs& args) {
	SelectCharacter(0x3);
}

// 0x5d31c0
void RFLobbyCharacterSelectAnimWnd::OnCeciliaClicked(EventArgs& args) {
	SelectCharacter(0xe);
}

// 0x5d31d0
void RFLobbyCharacterSelectAnimWnd::OnArinClicked(EventArgs& args) {
	SelectCharacter(0x6);
}

// 0x5d31e0
void RFLobbyCharacterSelectAnimWnd::OnLuciaClicked(EventArgs& args) {
	SelectCharacter(0x8);
}

// 0x5d31f0
void RFLobbyCharacterSelectAnimWnd::OnNellClicked(EventArgs& args) {
	SelectCharacter(0x9);
}

// 0x5d3200
void RFLobbyCharacterSelectAnimWnd::OnKoohClicked(EventArgs& args) {
	SelectCharacter(0x5);
}

// 0x5d3210
void RFLobbyCharacterSelectAnimWnd::OnSpikaClicked(EventArgs& args) {
	SelectCharacter(0xa);
}

// 0x5d3220
void RFLobbyCharacterSelectAnimWnd::OnKazClicked(EventArgs& args) {
	SelectCharacter(0x7);
}

// 0x5d3230
void RFLobbyCharacterSelectAnimWnd::OnMaxClicked(EventArgs& args) {
	SelectCharacter(0x4);
}

// 0x5d3240
void RFLobbyCharacterSelectAnimWnd::OnAzerClicked(EventArgs& args) {
	SelectCharacter(0x2);
}

// 0x5d3250
void RFLobbyCharacterSelectAnimWnd::EnableSlot(const wchar_t* name, bool enable) {
	RFWnd* slot = GetChild(std::wstring(name));
	if (slot != NULL) {
		slot->EnableWindow(enable);
	}
}

// 0x5d3320
void RFLobbyCharacterSelectAnimWnd::UpdateSlots() {
	for (int i = 0; i < 15; ++i) {
		EnableSlot(m_strings[i].c_str(), false);
	}
	for (size_t i = 0; i < m_characterIds.size(); ++i) {
		EnableSlot(m_strings[m_characterIds[i]].c_str(), true);
	}
}

// 0x5d33b0
void RFLobbyCharacterSelectAnimWnd::OnUpdate(EventArgs& args) {
	if (IsAnimationPlaying()) {
		for (int i = 0; i < 15; ++i) {
			EnableSlot(m_strings[i].c_str(), false);
		}
		return;
	}
	// m_shownCharacter 0xf reads past m_strings, as the original does.
	RFButton* button = dynamic_cast<RFButton*>(GetChild(std::wstring(m_strings[m_shownCharacter].c_str())));
	if (button != NULL) {
		button->SetState(RFButton::RFBUTTON_STATE_OVER);
	}
	if (m_animating) {
		if (!m_open) {
			s_loadParent->SetHitTestVisible(false);
		}
		PlayAnimation(m_open ? L"_Normal_" : L"_Init_");
		StopAnimation();
		m_animating = false;
		UpdateSlots();
	}
}

// 0x5d35b0
void RFLobbyCharacterSelectAnimWnd::SetOpen(int action, bool cancel) {
	if (m_animating) {
		return;
	}
	switch (action) {
	case OpenAction_Toggle:
		m_open = !m_open;
		break;
	case OpenAction_Open:
		m_open = true;
		break;
	case OpenAction_Close:
		m_open = false;
		break;
	}
	PlayAnimation(m_open ? L"_Opening_" : L"_Closing_");
	UpdateSlots();
	if (cancel) {
		m_shownCharacter = m_selectedCharacter;
	}
	if (m_open) {
		const uint32_t* character = Jp_GameState_GetCharacter(-1);
		int id = 0;
		if (character != NULL) {
			id = *character & 0x3ffffff;
		}
		m_selectedCharacter = id;
		s_loadParent->SetHitTestVisible(true);
	} else if (g_lobbyCharacterSelectWnd != NULL) {
		g_lobbyCharacterSelectWnd->OnCarouselClosed(m_shownCharacter, m_shownCharacter != m_selectedCharacter);
	}
}

// 0x5d3720
void RFLobbyCharacterSelectAnimWnd::OnCloserClicked(EventArgs& args) {
	if (!m_animating) {
		SetOpen(OpenAction_Close, true);
	}
}

// 0x5d3740
void RFLobbyCharacterSelectAnimWnd::OnNuriDblClick(EventArgs& args) {
	OnNuriClicked(args);
	SetOpen(OpenAction_Close, false);
}

// 0x5d3760
void RFLobbyCharacterSelectAnimWnd::OnNuriOldDblClick(EventArgs& args) {
	OnNuriOldClicked(args);
	SetOpen(OpenAction_Close, false);
}

// 0x5d3780
void RFLobbyCharacterSelectAnimWnd::OnHanaDblClick(EventArgs& args) {
	OnHanaClicked(args);
	SetOpen(OpenAction_Close, false);
}

// 0x5d37a0
void RFLobbyCharacterSelectAnimWnd::OnHanaOldDblClick(EventArgs& args) {
	OnHanaOldClicked(args);
	SetOpen(OpenAction_Close, false);
}

// 0x5d37c0
void RFLobbyCharacterSelectAnimWnd::OnCeciliaOldDblClick(EventArgs& args) {
	OnCeciliaOldClicked(args);
	SetOpen(OpenAction_Close, false);
}

// 0x5d37e0
void RFLobbyCharacterSelectAnimWnd::OnCeciliaDblClick(EventArgs& args) {
	OnCeciliaClicked(args);
	SetOpen(OpenAction_Close, false);
}

// 0x5d3800
void RFLobbyCharacterSelectAnimWnd::OnArinDblClick(EventArgs& args) {
	OnArinClicked(args);
	SetOpen(OpenAction_Close, false);
}

// 0x5d3820
void RFLobbyCharacterSelectAnimWnd::OnLuciaDblClick(EventArgs& args) {
	OnLuciaClicked(args);
	SetOpen(OpenAction_Close, false);
}

// 0x5d3840
void RFLobbyCharacterSelectAnimWnd::OnNellDblClick(EventArgs& args) {
	OnNellClicked(args);
	SetOpen(OpenAction_Close, false);
}

// 0x5d3860
void RFLobbyCharacterSelectAnimWnd::OnKoohDblClick(EventArgs& args) {
	OnKoohClicked(args);
	SetOpen(OpenAction_Close, false);
}

// 0x5d3880
void RFLobbyCharacterSelectAnimWnd::OnSpikaDblClick(EventArgs& args) {
	OnSpikaClicked(args);
	SetOpen(OpenAction_Close, false);
}

// 0x5d38a0
void RFLobbyCharacterSelectAnimWnd::OnKazDblClick(EventArgs& args) {
	OnKazClicked(args);
	SetOpen(OpenAction_Close, false);
}

// 0x5d38c0
void RFLobbyCharacterSelectAnimWnd::OnMaxDblClick(EventArgs& args) {
	OnMaxClicked(args);
	SetOpen(OpenAction_Close, false);
}

// 0x5d38e0
void RFLobbyCharacterSelectAnimWnd::OnAzerDblClick(EventArgs& args) {
	OnAzerClicked(args);
	SetOpen(OpenAction_Close, false);
}

// 0x5d3900
void RFLobbyCharacterSelectWnd::OnCharacterClicked(EventArgs& args) {
	if (g_lobbyCharacterCarousel != NULL && !g_lobbyCharacterCarousel->IsAnimating()) {
		g_lobbyCharacterCarousel->SetOpen(RFLobbyCharacterSelectAnimWnd::OpenAction_Toggle, false);
	}
}

// 0x5d3920
void RFLobbyCharacterSelectAnimWnd::ResetCharacters(int character) {
	m_characterIds.clear();
	std::vector<uint32_t> typeIds = Jp_GameState_CharacterTypeIds();
	for (size_t i = 0; i < typeIds.size(); ++i) {
		m_characterIds.push_back(typeIds[i] & 0x3ffffff);
	}
	PlayAnimation(L"_Init_");
	UpdateSlots();
	if (character != 0xf) {
		m_shownCharacter = character;
		m_selectedCharacter = character;
	}
	if (m_open) {
		SetOpen(OpenAction_Close, false);
	}
}

// 0x5d3b90
void RFLobbyCharacterSelectWnd::OnLobbyShown() {
	const uint32_t* character = Jp_GameState_GetCharacter(-1);
	int id = 0;
	if (character != NULL) {
		id = *character & 0x3ffffff;
	}
	m_selectedCharacter = id;
	OnCarouselClosed(id, false);
	CenterWindow_Horizontal(NULL);
	if (g_lobbyCharacterCarousel != NULL) {
		g_lobbyCharacterCarousel->ResetCharacters(m_carouselCharacter);
	}
}

// 0x5d3bf0
void RFLobbyCharacterSelectAnimWnd::OnInitialUpdate() {
	RegisterTo(L"", RFWnd::EventUpdate, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnUpdate, this));
	RegisterTo(L"", RFWnd::EventPlayUIAnimation, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnPlayUIAnimation, this));
	RegisterTo(L"btn_closer_left", RFButton::EventClicked, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnCloserClicked, this));
	RegisterTo(L"btn_closer_right", RFButton::EventClicked, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnCloserClicked, this));
	RegisterTo(L"btn_char_nuri", RFButton::EventClicked, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnNuriClicked, this));
	RegisterTo(L"btn_char_nuri_old", RFButton::EventClicked, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnNuriOldClicked, this));
	RegisterTo(L"btn_char_hana", RFButton::EventClicked, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnHanaClicked, this));
	RegisterTo(L"btn_char_hana_old", RFButton::EventClicked, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnHanaOldClicked, this));
	RegisterTo(L"btn_char_cecilia_old", RFButton::EventClicked, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnCeciliaOldClicked, this));
	RegisterTo(L"btn_char_cecilia", RFButton::EventClicked, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnCeciliaClicked, this));
	RegisterTo(L"btn_char_arin", RFButton::EventClicked, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnArinClicked, this));
	RegisterTo(L"btn_char_lucia", RFButton::EventClicked, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnLuciaClicked, this));
	RegisterTo(L"btn_char_nell", RFButton::EventClicked, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnNellClicked, this));
	RegisterTo(L"btn_char_kooh", RFButton::EventClicked, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnKoohClicked, this));
	RegisterTo(L"btn_char_spika", RFButton::EventClicked, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnSpikaClicked, this));
	RegisterTo(L"btn_char_kaz", RFButton::EventClicked, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnKazClicked, this));
	RegisterTo(L"btn_char_max", RFButton::EventClicked, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnMaxClicked, this));
	RegisterTo(L"btn_char_azer", RFButton::EventClicked, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnAzerClicked, this));
	RegisterTo(L"btn_char_nuri", RFWnd::EventLButtonDblClick, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnNuriDblClick, this));
	RegisterTo(L"btn_char_nuri_old", RFWnd::EventLButtonDblClick, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnNuriOldDblClick, this));
	RegisterTo(L"btn_char_hana", RFWnd::EventLButtonDblClick, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnHanaDblClick, this));
	RegisterTo(L"btn_char_hana_old", RFWnd::EventLButtonDblClick, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnHanaOldDblClick, this));
	RegisterTo(L"btn_char_cecilia_old", RFWnd::EventLButtonDblClick, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnCeciliaOldDblClick, this));
	RegisterTo(L"btn_char_cecilia", RFWnd::EventLButtonDblClick, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnCeciliaDblClick, this));
	RegisterTo(L"btn_char_arin", RFWnd::EventLButtonDblClick, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnArinDblClick, this));
	RegisterTo(L"btn_char_lucia", RFWnd::EventLButtonDblClick, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnLuciaDblClick, this));
	RegisterTo(L"btn_char_nell", RFWnd::EventLButtonDblClick, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnNellDblClick, this));
	RegisterTo(L"btn_char_kooh", RFWnd::EventLButtonDblClick, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnKoohDblClick, this));
	RegisterTo(L"btn_char_spika", RFWnd::EventLButtonDblClick, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnSpikaDblClick, this));
	RegisterTo(L"btn_char_kaz", RFWnd::EventLButtonDblClick, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnKazDblClick, this));
	RegisterTo(L"btn_char_max", RFWnd::EventLButtonDblClick, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnMaxDblClick, this));
	RegisterTo(L"btn_char_azer", RFWnd::EventLButtonDblClick, Subscriber(&RFLobbyCharacterSelectAnimWnd::OnAzerDblClick, this));
	if (g_lobbyCharacterSelectWnd != NULL) {
		for (int i = 0; i < 15; ++i) {
			m_strings[i] = ShiftJisToWString(std::string(MakeStr("btn_char_%s", g_lobbyCharacterSelectWnd->GetCharacterName(i))));
		}
	}
	CenterWindow_Horizontal(NULL);
	const uint32_t* character = Jp_GameState_GetCharacter(-1);
	int id = 0;
	if (character != NULL) {
		id = *character & 0x3ffffff;
	}
	ResetCharacters(id);
	SetPreDraw(true);
}

// 0x5d5240
void RFLobbyCharacterSelectWnd::OnInitialUpdate() {
	RegisterTo(L"btn_character", RFButton::EventClicked, Subscriber(&RFLobbyCharacterSelectWnd::OnCharacterClicked, this));
	RegisterTo(L"btn_character", RFWnd::EventMouseEnter, Subscriber(&RFLobbyCharacterSelectWnd::OnCharacterMouseEnter, this));
	RegisterTo(L"btn_character", RFWnd::EventMouseLeave, Subscriber(&RFLobbyCharacterSelectWnd::OnCharacterMouseLeave, this));
	Jp_GetCharacterImageNames(m_strings);
	OnLobbyShown();
	SetPreDraw(true);
}
