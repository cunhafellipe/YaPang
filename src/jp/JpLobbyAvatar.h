// The JP 983 lobby avatar LobbyPlayer builds and shows: WRTTI/RTTI name
// "IDummyPlayer", base "IPetScriptObject" (jp/pairing.tsv jp_class), 0xe4
// bytes, ctor 0x4d78f0, vtable 0xd04bbc (17 slots). IPetScriptObject's own
// vtable is 0xd04668 (15 slots: its scalar deleting destructor 0x4d1060 and
// 14 empty bodies 0x5ac830..0x5ac8f0). KR has neither class (no
// IPetScriptObject in yapang/src nor in the KR PDB), so the slot names below
// come from what the JP bodies do; the empty base slots keep the names of
// their argument shape only.
// From the JP exe's bodies (node scripts/disasm.ts --jp 0x<va>).
#ifndef PG_JP_JPLOBBYAVATAR_H
#define PG_JP_JPLOBBYAVATAR_H

#include <stdint.h>
#include <list>
#include <string>

#include <windows.h>

#include "defines.h"
#include "padding.h"
#include "wmath.h"
#include "wscene.h"
#include "RefreshSharedPtr.h"
#include "JpFxInterpreter.h"

class Bitmap;
class CPartTidList;
class CPetFrame;
class WPuppet;
struct sCharacterInfo;
struct sUccClothes;
struct w_motion_data;
namespace lua_system {
class lua_script;
}

// The effects JpFxInterpreter::FindSpray / FindSequence return (0x885570 /
// 0x8856c0; the JP CFxSpray / CFxSequence); only the flag
// AttachScriptEffect sets is declared.
struct JpDummyPlayerSprayEffect {
	PADDING(0x0, 0x110c);
	bool m_kind7;               // 0x110c, set to (kind == 7) by AttachScriptEffect
};
struct JpDummyPlayerSequenceEffect {
	PADDING(0x0, 0x1108);
	bool m_kind7;               // 0x1108, set to (kind == 7) by AttachScriptEffect (only when it differs)
};

// A face animation record (0x94 bytes, zero-filled by RunScriptCommand
// 0x4d8bf0), kept by name in IDummyPlayer::m_faceAnims; +0xc4 points to the
// current one and Display hands it to the bone puppet once (+0xc8 remembers
// the last one applied).
struct JpDummyPlayerFaceAnim {
	uint8_t m_keyBlock[0x40];   // 0x0, zero-filled; role not traced
	char m_name[0x40];          // 0x40, the face texture part; Display cuts it (0xb69e20) while m_bitmap is NULL
	Bitmap* m_bitmap;           // 0x80, the part's bitmap, kept by the first Display, put back (0xb69e30) by the later ones
	tagRECT m_rect;             // 0x84, the part's rectangle in the face texture
};

// The JP pet script callback interface (vtable 0xd04668). The base bodies
// are all empty (ret 4 / ret 8 / ret 0xc). ParseScript (0x5ac510)
// dispatches the script keywords (table 0xe11820) to slots 1..13: *snd( 1,
// *shot 2, *tail 3, *showclub 4, *hideclub 5, *swing 6, *lockclub 7, *fx(
// and *fxstop( 8 (kind 7 / 15), *ptex( 9, *showbone( / *hidebone( 10 (kind
// 9 / 10), *playif( 11, *opa( 12, *showvis( / *hidevis( 13 (kind 13 / 14);
// every one gets the script cursor by reference.
class IPetScriptObject {
public:
	virtual ~IPetScriptObject() {}                                      // [0] 0x4d1060 (scalar deleting destructor)
	virtual void PlaySound(const char*& name) { (void)name; }           // [1] 0x5ac830
	virtual void ScriptEvent2(const char*& script) { (void)script; }               // [2] 0x5ac840
	virtual void ScriptEvent3(const char*& script) { (void)script; }               // [3] 0x5ac850
	virtual void ScriptEvent4(const char*& script) { (void)script; }               // [4] 0x5ac860
	virtual void HideClub(const char*& script) { (void)script; }                   // [5] 0x5ac870
	virtual void ScriptEvent6(const char*& script) { (void)script; }               // [6] 0x5ac880
	virtual void ScriptEvent7(const char*& script) { (void)script; }               // [7] 0x5ac890
	virtual void AttachScriptEffect(const char*& script, bool skip, int kind) { (void)script; (void)skip; (void)kind; } // [8] 0x5ac8a0
	virtual void RunMotionScript(const char*& script) { (void)script; } // [9] 0x5ac8b0
	virtual void ApplyFrameScript(const char*& script, int kind) { (void)script; (void)kind; } // [10] 0x5ac8c0
	virtual void PlayScriptMotion(const char*& script) { (void)script; } // [11] 0x5ac8d0
	virtual void ScriptEvent12(const char*& script) { (void)script; }              // [12] 0x5ac8e0
	virtual void ApplyFrameKind(const char*& script, int kind) { (void)script; (void)kind; } // [13] 0x5ac8f0

	// The helpers next to the base bodies, called with the pet frame's raw
	// pointer.
	// 0x5ac900: shows the bones the script names (up to 64, until ')'),
	// keeping them hidden-flagged unless kind is 9.
	static void FrameScript(CPetFrame* frame, int kind, const char*& script);
	// 0x5aca50: reads "pet;pet;..." and a motion name from the script; the
	// motion name when the frame has one of the pets loaded ("<pet>.mpet"),
	// NULL otherwise.
	static const char* FindScriptMotion(CPetFrame* frame, const char*& script);
	// 0x5acbf0: the same for the bone groups the script names (the shared
	// doc's named bone lists, 0x641240).
	static void FrameKind(CPetFrame* frame, bool kindIs13, const char*& script);
	// 0x5ac510: runs the script's keywords (sprintf'd into a 0x400
	// buffer); flag goes to AttachScriptEffect. The first *fx( of a
	// "TF_Feather.seq" effect also gets three "plume_twinkle.seq" effects
	// (head_wood or head_iron).
	void ParseScript(const char* script, bool flag);
	// 0x5ace20: fills name with the motion script's next command (an
	// optional "@motion" filter, then the command); false without one.
	// kind 1 takes the command as is; the others look it up in the
	// TanLines data (JpTanLines.h): kind 2 when a worn part has a row, kind
	// 0 when extra is 1, kind 3+ (and negative) with the parts of
	// m_scriptFrame.
	bool NextMotionCommand(const w_motion_data* motion, std::string& name, const char*& script, int kind, bool extra);

	// 0x4: read by NextMotionCommand's kind 3+ path only; IDummyPlayer
	// (kind 2) never writes it. The other subclass calling
	// NextMotionCommand (0x742360, kind -1; its IPetScriptObject is a base
	// at +0x18) is not traced.
	CPetFrame* m_scriptFrame;
};

class IDummyPlayer : public IPetScriptObject {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x4d78f0 (the lobby builders pass 2).
	explicit IDummyPlayer(int kind);

	// Virtuals in vtable order (0xd04bbc).
	virtual ~IDummyPlayer();                                // [0] 0x4d8ef0 (scalar deleting destructor; body 0x4d7a30)
	// [1] 0x4d5500: resolves the name (GetStrConst into a 0x40 buffer) and
	// plays it at the avatar's position relative to the listener.
	virtual void PlaySound(const char*& name);
	// [5] 0x4d55a0 (*hideclub): hides the scene element "\xc5\xac\xb7\xb4"
	// (0xd046b0, the club) through the scene manager (0xe741c0,
	// CSceneManager::SetVisible 0x9b0e70); the script is not read.
	virtual void HideClub(const char*& script);
	// [8] 0x4d8580: the script's effect command: ["@motion"] effect file,
	// ["&part tid"...] bone or bound box names (up to 4); starts the effect
	// on the effect host or flags the running one (kind == 7).
	virtual void AttachScriptEffect(const char*& script, bool skip, int kind);
	// [9] 0x4d8f20: runs the current motion's next script command.
	virtual void RunMotionScript(const char*& script);
	// [10] 0x4d5f60
	virtual void ApplyFrameScript(const char*& script, int kind);
	// [11] 0x4d7200: plays the motion a script index names.
	virtual void PlayScriptMotion(const char*& script);
	// [13] 0x4d5f80
	virtual void ApplyFrameKind(const char*& script, int kind);
	// [14] 0x4d6f80: the attach 0x566390 calls it with ("기본자세", 0) and
	// attaches only when it returns true.
	virtual bool SetMotionByName(const char* name, int param);
	virtual void Process(float dt);                         // [15] 0x4d70c0, LobbyPlayer::OnProcess
	virtual void Display();                                 // [16] 0x4d5ed0, LobbyPlayer::OnDisplay

	// 0x4d55c0: rebuilds m_matrix (up = 0xe424c8, (0, 1, 0)) on the ground
	// point under the position.
	void SetPosition(const WVector* position);
	// 0x4d5fa0: restarts the effect host's effect after a part change.
	void RefreshAfterPartChange(int keepEffects, int attachFrame, int param3, bool flag);
	// 0x4d6040: runs the frame scripts of the motion between two times (KR
	// ntAvatar::ScriptProcess; all = the frame at from only); flag goes to
	// ParsePuppetScript.
	void ScriptProcess(const w_motion_data* data, float from, float to, bool all, bool flag);
	// 0x4d4940: a tail jump to ParseScript (0x5ac510).
	void ParsePuppetScript(const char* script, bool flag);
	// 0x4d6aa0: plays the named motion of the pet frame; false without one.
	bool PlayMotion(const char* name);
	// 0x4d7c60: builds the pet frame from a part list (partTids in edx under
	// LTCG; ret 0x18). skipRegister keeps it out of the scene manager;
	// altList fills the list with the 0x661fb0 variant (kept in
	// m_altPartList).
	bool BuildParts(DWORD* partTids, DWORD* auxTids, DWORD* partIds, uint8_t hairColor, uint8_t shirtColor, bool skipRegister, bool altList);
	// Port adapter for the LobbyPlayer callers, which pass a character
	// record: the JP callers pass its parts (+0xc), aux parts (+0x1a4), part
	// ids (+0x6c) and colours (+8, +9) one by one; the KR sCharacterInfo
	// carries the same fields.
	bool BuildParts(const sCharacterInfo& info, bool skipRegister, bool altList);
	// 0x4d7fb0 (318 bytes): builds from the local player's my-parts lists;
	// not decompiled.
	bool BuildFromPartTidList(CPartTidList& parts, DWORD* auxTids, bool flag);
	// 0x4d80f0: the default outfit of m_typeId (the item manager's default
	// combo) into partTids (24) and auxTids (5, zeroed); false when the part
	// map holds no part of the character. Reads the character's row of
	// "essentialPosition" (grandprix_ceremony.lua, loaded and released here
	// without a script; script in edx under LTCG) into a vector it never
	// uses.
	bool RandomDressUp(lua_system::lua_script* script, DWORD* partTids, DWORD* auxTids);
	// Port adapter for the LobbyPlayer caller, which fills a character
	// record (the JP caller passes two local arrays).
	void RandomDressUp(sCharacterInfo* info);
	// 0x4d8bf0: makes the named face animation current (creating it).
	void RunScriptCommand(const char* command);

	RF_SHARED_PTR<CPetFrame> m_petFrame;     // 0x8, reset to an empty pointer by the builders (0x4d7320 / 0x4d7630)
	int m_kind;                               // 0x10, the ctor's argument
	std::string m_name;                       // 0x14, MakeStr("LOBBY_PLAYER_%d_%d"), the scene element name
	WMatrix m_matrix;                         // 0x30, written by SetPosition, copied to LobbyPlayer +0x3dc
	float m_lightAngle;                       // 0x60, pi from SetMotionByName
	LightSet m_light;                         // 0x64, directional (SetMotionByName), diffuse from the shadow intensity (Process); the frame's UpdateLightSource
	WVector m_position;                       // 0x80, SetPosition's argument
	int m_unknown8c[3];                       // 0x8c, zeroed by the ctor
	PADDING(0x98, 0x9c);
	int m_unknown9c[3];                       // 0x9c, zeroed by the ctor
	PADDING(0xa8, 0xac);
	uint32_t m_typeId;                        // 0xac, the character type id (tid & 0x3ffffff; rand() % 15 on the random path)
	w_motion_data* m_motionState;             // 0xb0, PlayMotion's motion
	float m_motionEnd;                        // 0xb4, the motion time (PlayMotion: the larger of s and e; Process adds dt)
	std::list<JpDummyPlayerFaceAnim*> m_faceAnims; // 0xb8, head 0xb8, size 0xbc (RunScriptCommand)
	PADDING(0xc0, 0xc4);
	JpDummyPlayerFaceAnim* m_pendingFace;     // 0xc4
	JpDummyPlayerFaceAnim* m_appliedFace;     // 0xc8
	bool m_motionHold;                        // 0xcc, cleared by message 0x186, set by the attach
	bool m_motionPlaying;                     // 0xcd, set by 0x199/0x19a and the attach, cleared by 0x186
	bool m_effectFlag;                        // 0xce, 1 from the ctor, SetMotionByName's param
	bool m_motionReady;                       // 0xcf, set by SetMotionByName, cleared by the destructor
	JpFxInterpreter* m_effectHost;            // 0xd0, the JP FxInterpreter (NULL, 0)
	int m_effectId;                           // 0xd4, RefreshAfterPartChange's effect
	PADDING(0xd8, 0xe0);
	bool m_altPartList;                       // 0xe0, BuildParts' altList; passed to NextMotionCommand
	PADDING(0xe1, 0xe4);
};

CHECK_SIZE(IDummyPlayer, 0xe4);
BEGIN_LAYOUT_CHECK(IDummyPlayer)
	CHECK_FIELD(IDummyPlayer, m_petFrame, 0x8);
	CHECK_FIELD(IDummyPlayer, m_kind, 0x10);
	CHECK_FIELD(IDummyPlayer, m_name, 0x14);
	CHECK_FIELD(IDummyPlayer, m_matrix, 0x30);
	CHECK_FIELD(IDummyPlayer, m_lightAngle, 0x60);
	CHECK_FIELD(IDummyPlayer, m_light, 0x64);
	CHECK_FIELD(IDummyPlayer, m_position, 0x80);
	CHECK_FIELD(IDummyPlayer, m_typeId, 0xac);
	CHECK_FIELD(IDummyPlayer, m_motionState, 0xb0);
	CHECK_FIELD(IDummyPlayer, m_motionEnd, 0xb4);
	CHECK_FIELD(IDummyPlayer, m_faceAnims, 0xb8);
	CHECK_FIELD(IDummyPlayer, m_pendingFace, 0xc4);
	CHECK_FIELD(IDummyPlayer, m_motionHold, 0xcc);
	CHECK_FIELD(IDummyPlayer, m_effectHost, 0xd0);
	CHECK_FIELD(IDummyPlayer, m_altPartList, 0xe0);
END_LAYOUT_CHECK
CHECK_FIELD(IPetScriptObject, m_scriptFrame, 0x4);
CHECK_FIELD(JpDummyPlayerFaceAnim, m_bitmap, 0x80);
CHECK_SIZE(JpDummyPlayerFaceAnim, 0x94);

#endif // PG_JP_JPLOBBYAVATAR_H
