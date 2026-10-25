// The JP 983 lobby's character select window (RTTI
// RFLobbyCharacterSelectWnd : RFFormBase<RFWndEx>, vtable 0xd10f9c, 34
// slots: no new virtuals), loaded from lobby_char_select_wnd.xml by
// CreateLobbyCharacterSelectWnd 0x5d2980 under wnd_char_select_area.
// Declarations and the JP x86 layout, from the JP exe's bodies
// (node scripts/disasm.ts --jp 0x<va>); the members not written in
// JpLobbyCharacterSelectWnd.cpp are declared JP dependencies.
#ifndef PG_JP_JPLOBBYCHARACTERSELECTWND_H
#define PG_JP_JPLOBBYCHARACTERSELECTWND_H

#include <string>
#include <vector>
#include <stdint.h>

#include "defines.h"
#include "padding.h"
#include "JpTooltipWnd.h"
#include "JpLobbyGlue.h"

// Size 0x334.
class RFLobbyCharacterSelectWnd : public RFFormBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
	friend class RFLobbyDesktop;  // GetSelectedCharacterTypeId 0x5f6410 reads m_carouselCharacter
public:
	// 0x5d2220: RFWndEx::RFWndEx, the two characters 0xf, the 15 strings,
	// m_unknown330 cleared; then the instance is kept in
	// g_lobbyCharacterSelectWnd.
	RFLobbyCharacterSelectWnd();
	// [1] 0x5d2570 (scalar deleting destructor); ~RFLobbyCharacterSelectWnd
	// 0x5d22b0 clears g_lobbyCharacterSelectWnd, destroys the strings, then
	// restores RFFormBase<RFWndEx>'s vtable and runs ~RFWndEx.
	virtual ~RFLobbyCharacterSelectWnd();

	// 0x5d2020 (name ours): m_strings[character].c_str(), NULL for an id
	// past the 15.
	const char* GetCharacterName(int character);

	// 0xe781ac (name ours): the parent the last
	// CreateLobbyCharacterSelectWnd was given.
	static RFWnd* s_loadParent;

	// 0x5d2ac0 (name ours): what the carousel calls when it closes, with
	// the character it shows and whether that differs from the selected
	// one: btn_character takes the character's images, the change is sent
	// (avatar chat: packet 0xc; shop: LobbyAvatar 0x187 and the equip
	// packet; otherwise LobbyAvatar 0x192), and the game state shows it.
	void OnCarouselClosed(int character, bool changed);
	// 0x5d3b90 (name ours): takes the equipped character
	// as the selected and the shown one, centres the window horizontally
	// and resets the carousel to the shown character.
	void OnLobbyShown();

protected:
	// [10] 0x5d5240: subscribes btn_character's Clicked, MouseEnter and
	// MouseLeave, fills m_strings, runs OnLobbyShown and sets pre-draw.
	virtual void OnInitialUpdate();

	// btn_character's handlers (names ours): 0x5d3900 toggles the carousel
	// unless it animates; 0x5d1b30 and 0x5d1b40 are empty.
	void OnCharacterClicked(EventArgs& args);
	void OnCharacterMouseEnter(EventArgs& args);
	void OnCharacterMouseLeave(EventArgs& args);

	// 0x0-0x184: RFFormBase<RFWndEx> / RFWndEx.
	int m_selectedCharacter;     // 0x184 0xf at construction; OnLobbyShown 0x5d3b90 stores the equipped character's id (& 0x3ffffff) and passes it to 0x5d2ac0
	int m_carouselCharacter;     // 0x188 0xf at construction; 0x5d3b90 passes it to the carousel (0x5d3920); OnCarouselClosed stores the shown one
	std::string m_strings[15];   // 0x18c the characters' image name parts, by character id (OnCarouselClosed)
	bool m_unknown330;           // 0x330 cleared by the constructor; OnCarouselClosed sets it when the character differs from m_selectedCharacter (readers not traced)
	PADDING(0x331, 0x334);
};
CHECK_SIZE(RFLobbyCharacterSelectWnd, 0x334);
BEGIN_LAYOUT_CHECK(RFLobbyCharacterSelectWnd)
	CHECK_FIELD(RFLobbyCharacterSelectWnd, m_selectedCharacter, 0x184);
	CHECK_FIELD(RFLobbyCharacterSelectWnd, m_carouselCharacter, 0x188);
	CHECK_FIELD(RFLobbyCharacterSelectWnd, m_strings, 0x18c);
	CHECK_FIELD(RFLobbyCharacterSelectWnd, m_unknown330, 0x330);
END_LAYOUT_CHECK

// The lobby's character carousel (RTTI RFLobbyCharacterSelectAnimWnd :
// RFFormBase<RFWndEx>, vtable 0xd1102c, 34 slots: no new virtuals), loaded
// from lobby_char_select_anim_wnd.xml by CreateLobbyCharacterCarousel
// 0x5d2ef0 under wnd_char_select_anim_area. Size 0x344.
class RFLobbyCharacterSelectAnimWnd : public RFFormBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// SetOpen's action.
	enum OpenAction {
		OpenAction_Toggle = 0,
		OpenAction_Open = 1,
		OpenAction_Close = 2
	};

	// 0x5d2670: RFWndEx::RFWndEx, m_open / m_animating cleared, both
	// characters 0xf, the 15 strings; then the instance is kept in
	// g_lobbyCharacterCarousel.
	RFLobbyCharacterSelectAnimWnd();
	// [1] 0x5d2950 (scalar deleting destructor);
	// ~RFLobbyCharacterSelectAnimWnd 0x5d2710 clears
	// g_lobbyCharacterCarousel, destroys the strings and the id list, then
	// restores RFFormBase<RFWndEx>'s vtable and runs ~RFWndEx.
	virtual ~RFLobbyCharacterSelectAnimWnd();

	// The desktop reads the two bytes directly (accessors ours).
	bool IsOpen() const { return m_open; }
	bool IsAnimating() const { return m_animating; }

	// 0x5d35b0 (name ours): opens, closes or toggles the carousel (nothing
	// while it animates) and plays "_Opening_" / "_Closing_". cancel puts
	// the selected character back as the shown one. Opening takes the
	// equipped character as the selected one and sets the parent's
	// hit-test flag; closing reports the shown character to the character
	// select window.
	void SetOpen(int action, bool cancel);
	// 0x5d3920 (name ours): rebuilds m_characterIds from the JP game
	// state's character map (0xe47dbc +0x3708, ids & 0x3ffffff), plays
	// "_Init_", updates the slots (0x5d3320), takes character as both the
	// selected and the shown one unless it is 0xf, and closes the carousel
	// (SetOpen(Close, false)) if it was open.
	void ResetCharacters(int character);

	// 0xe781b4 (name ours): the parent the last
	// CreateLobbyCharacterCarousel was given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5d3bf0: subscribes the window's Update and PlayUIAnimation,
	// the two closers and every btn_char_<name>'s Clicked and
	// LButtonDblClick; names the 15 slots "btn_char_<the character select
	// window's name part>", centres the window horizontally and resets the
	// characters to the equipped one.
	virtual void OnInitialUpdate();

	// 0x5d1b50 (name ours): EventPlayUIAnimation sets m_animating.
	void OnPlayUIAnimation(EventArgs& args);
	// 0x5d3030 (name ours): makes character the shown one: the previous
	// slot's button goes back to normal and the character select window
	// takes it (OnCarouselClosed(character, false)).
	void SelectCharacter(int character);
	// btn_char_<name> EventClicked (names ours): SelectCharacter(id).
	void OnNuriClicked(EventArgs& args);        // 0x5d3170 id 0xb
	void OnNuriOldClicked(EventArgs& args);     // 0x5d3180 id 0
	void OnHanaClicked(EventArgs& args);        // 0x5d3190 id 0xc
	void OnHanaOldClicked(EventArgs& args);     // 0x5d31a0 id 1
	void OnCeciliaOldClicked(EventArgs& args);  // 0x5d31b0 id 3
	void OnCeciliaClicked(EventArgs& args);     // 0x5d31c0 id 0xe
	void OnArinClicked(EventArgs& args);        // 0x5d31d0 id 6
	void OnLuciaClicked(EventArgs& args);       // 0x5d31e0 id 8
	void OnNellClicked(EventArgs& args);        // 0x5d31f0 id 9
	void OnKoohClicked(EventArgs& args);        // 0x5d3200 id 5
	void OnSpikaClicked(EventArgs& args);       // 0x5d3210 id 0xa
	void OnKazClicked(EventArgs& args);         // 0x5d3220 id 7
	void OnMaxClicked(EventArgs& args);         // 0x5d3230 id 4
	void OnAzerClicked(EventArgs& args);        // 0x5d3240 id 2
	// 0x5d33b0 (name ours): EventUpdate: while an animation plays the
	// slots stay disabled; otherwise the shown slot's button is drawn
	// over, and an animation that ended puts the carousel in its rest
	// state ("_Normal_" open, "_Init_" closed and the parent not hit-test
	// visible) and re-enables the owned slots.
	void OnUpdate(EventArgs& args);
	// 0x5d3720 (name ours): btn_closer_left / btn_closer_right: closes the
	// carousel, cancelling the choice, unless it animates.
	void OnCloserClicked(EventArgs& args);
	// btn_char_<name> EventLButtonDblClick (names ours): the Clicked
	// handler, then SetOpen(Close, false).
	void OnNuriDblClick(EventArgs& args);        // 0x5d3740
	void OnNuriOldDblClick(EventArgs& args);     // 0x5d3760
	void OnHanaDblClick(EventArgs& args);        // 0x5d3780
	void OnHanaOldDblClick(EventArgs& args);     // 0x5d37a0
	void OnCeciliaOldDblClick(EventArgs& args);  // 0x5d37c0
	void OnCeciliaDblClick(EventArgs& args);     // 0x5d37e0
	void OnArinDblClick(EventArgs& args);        // 0x5d3800
	void OnLuciaDblClick(EventArgs& args);       // 0x5d3820
	void OnNellDblClick(EventArgs& args);        // 0x5d3840
	void OnKoohDblClick(EventArgs& args);        // 0x5d3860
	void OnSpikaDblClick(EventArgs& args);       // 0x5d3880
	void OnKazDblClick(EventArgs& args);         // 0x5d38a0
	void OnMaxDblClick(EventArgs& args);         // 0x5d38c0
	void OnAzerDblClick(EventArgs& args);        // 0x5d38e0

	// 0x5d3250 (name ours; this in ecx, name in edx): enables or disables
	// the child named name, when there is one.
	void EnableSlot(const wchar_t* name, bool enable);
	// 0x5d3320 (name ours): what SetOpen and ResetCharacters call after
	// their animation: disables the 15 slots named by m_strings, then
	// enables those of the owned characters (m_characterIds).
	void UpdateSlots();

	// 0x0-0x184: RFFormBase<RFWndEx> / RFWndEx.
	bool m_open;                       // 0x184 set while the carousel is open
	bool m_animating;                  // 0x185 set while it animates (SetOpen does nothing then)
	PADDING(0x186, 0x188);
	int m_selectedCharacter;           // 0x188 0xf at construction; the equipped character when opened
	int m_shownCharacter;              // 0x18c 0xf at construction; reported to the character select window on close
	std::vector<int> m_characterIds;   // 0x190 the owned characters' ids (ResetCharacters)
	std::wstring m_strings[15];        // 0x1a0 the slot children's names, by character id (UpdateSlots)
};
CHECK_SIZE(RFLobbyCharacterSelectAnimWnd, 0x344);
BEGIN_LAYOUT_CHECK(RFLobbyCharacterSelectAnimWnd)
	CHECK_FIELD(RFLobbyCharacterSelectAnimWnd, m_open, 0x184);
	CHECK_FIELD(RFLobbyCharacterSelectAnimWnd, m_animating, 0x185);
	CHECK_FIELD(RFLobbyCharacterSelectAnimWnd, m_selectedCharacter, 0x188);
	CHECK_FIELD(RFLobbyCharacterSelectAnimWnd, m_shownCharacter, 0x18c);
	CHECK_FIELD(RFLobbyCharacterSelectAnimWnd, m_characterIds, 0x190);
	CHECK_FIELD(RFLobbyCharacterSelectAnimWnd, m_strings, 0x1a0);
END_LAYOUT_CHECK

#endif // PG_JP_JPLOBBYCHARACTERSELECTWND_H
