// The JP 983 lobby pet IDummyPet (jp/pairing.tsv jp_class, code
// 0x4d1190-0x4d3fc0): the 3D object LobbyCaddie (JpLobbyCaddie.h, kind 0)
// and LobbyMascot (JpLobbyMascot.h, kind 5) build and show. A sibling of
// IDummyPlayer (JpLobbyAvatar.h) on a bare WPuppet ("<model>.pet") instead
// of a pet frame. KR has no such class; the caddie / mascot IFF records and
// the puppet calls are the KR ones (KR CGolfCaddie::AddCaddie, CMascot).
// From the JP exe's bodies (node scripts/disasm.ts --jp 0x<va>).
#ifndef PG_JP_JPDUMMYPET_H
#define PG_JP_JPDUMMYPET_H

#include <list>
#include <string>
#include <vector>
#include <stdint.h>

#include "defines.h"
#include "padding.h"
#include "wmath.h"
#include "wtypes.h"
#include "JpLobbyAvatar.h"

class WPuppet;
class Bitmap;
class JpFxInterpreter;
struct w_motion_data;

// One texture part change of the pet (0x94 bytes, zero-filled by
// SetTexturePart 0x4d3a50): the puppet's texture animation name ("face",
// "body"), the texture file, and the bitmap / rectangle the first
// ChangeTexturePart fills (reused afterwards).
struct JpPetTexturePart {
	char m_animName[0x40];      // 0x0
	char m_texName[0x40];       // 0x40, the list is searched by it (_stricmp, 0x4d27d0)
	Bitmap* m_bitmap;           // 0x80
	tagRECT m_rect;             // 0x84
};

// 0x4d34b0: reads a texture part command of the motion script ("[@motion]
// part path"; the path through the TanLines data, JpTanLines.h) into part /
// texture; false without one, or for another motion.
bool Jp_DummyPet_ParseTexturePart(const w_motion_data* motion, std::string& part, std::string& texture, const char*& script);
// 0x5ac9b0 / 0x5acd10: IPetScriptObject's FrameScript / FrameKind helpers on
// a bare puppet (0x5ac900 / 0x5acbf0 take the pet frame); defined in
// JpDummyPet.cpp next to their callers.
void Jp_PetScript_PuppetFrameScript(WPuppet* puppet, int kind, const char*& script);
void Jp_PetScript_PuppetFrameKind(WPuppet* puppet, bool kindIs13, const char*& script);
// 0x88e860 (1134 bytes): JpFxInterpreter's attach of a puppet's effects
// (the pet's motion reset); the effect id. Not decompiled (port body: 0);
// a JpFxInterpreter member once its owner declares it.
int Jp_FxInterpreter_AttachPuppet(JpFxInterpreter* fx, WPuppet* puppet);

// 0xc4 bytes, vtable 0xd046dc (17 slots: IPetScriptObject's 14 + 3), ctor
// 0x4d37d0. Slots [2]-[4], [6], [7], [12] are IPetScriptObject's empty bodies.
class IDummyPet : public IPetScriptObject {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x4d37d0 (caddie 0, mascot 5)
	explicit IDummyPet(int kind);

	// Virtuals in vtable order (0xd046dc).
	virtual ~IDummyPet();                                   // [0] 0x4d3d40 (scalar deleting destructor; body 0x4d38c0)
	// [1] 0x4d1be0: resolves the name (GetStrConst into a 0x40 buffer) and
	// plays it at the pet's position relative to the listener.
	virtual void PlaySound(const char*& name);
	// [5] 0x4d1c80: *hideclub, the scene element 0xd046b0 ("클럽") hidden (as IDummyPlayer's [5]).
	virtual void PlaySceneSound(const char*& script);
	virtual void AttachScriptEffect(const char*& script, bool skip, int kind);  // [8] 0x4d1190 (empty)
	// [9] 0x4d3d70: the script's texture part command (0x4d34b0) applied.
	virtual void RunMotionScript(const char*& script);
	// [10] 0x4d11a0: 0x5ac9b0 on the puppet.
	virtual void ApplyFrameScript(const char*& script, int kind);
	virtual void PlayScriptMotion(const char*& script);     // [11] 0x4d11c0 (empty)
	// [13] 0x4d11d0: 0x5acd10 on the puppet (kind 13: show).
	virtual void ApplyFrameKind(const char*& script, int kind);
	// [14] 0x4d2890: resets the matrix and the motion state, creates the
	// effect host, plays the named motion ("기다리기" for the caddie,
	// "Default" for the mascot) and resets the effects; always true.
	virtual bool SetMotionByName(const char* name);
	// [15] 0x4d2980 (LobbyCaddie / LobbyMascot::OnProcess): advances the
	// motion, runs its frame scripts, chains the next motion at its end.
	virtual void Process(float dt);
	// [16] 0x4d2500 (OnDisplay): transforms and renders the puppet when it
	// is in the view.
	virtual void Display();

	// 0x4d1200 (ret 0xc; only the first argument is read): stops the
	// effects (all when param1 is set, else the last one) and attaches the
	// puppet's effects again.
	void ResetMotion(int param1, int param2, bool flag);
	// 0x4d1ca0: runs the frame scripts of the motion between two times (the
	// same code as IDummyPlayer's 0x4d6040, on the puppet).
	void ScriptProcess(const w_motion_data* data, float from, float to, bool all, bool flag);
	// 0x4d1e50: rebuilds m_matrix (up = 0xe424c8, (0, 1, 0)) at the
	// position (no ground point, unlike IDummyPlayer's 0x4d55c0).
	void SetPosition(const WVector* position);
	// 0x4d25a0: plays the puppet's named motion; false without a puppet, a
	// root bone or the motion.
	bool PlayMotion(const char* name);
	// 0x4d3a50: changes the puppet's texture animation part to the texture
	// (kind 12: by handle), remembering the change by texture name.
	void SetTexturePart(const char* part, const char* texture);
	// 0x4d3150 (typeId in edx under LTCG, ret 4): loads the mascot's
	// "<model>.pet", writes the owned mascot's message into its texture, and
	// registers it in the scene unless skipRegister.
	bool BuildMascot(uint32_t typeId, bool skipRegister);
	// 0x4d3fc0 (typeId in edx under LTCG, ret 8): loads the caddie's puppet
	// from its sCaddie type id and its sCadItem type id (0: none; else the
	// item's body texture picks an "_ex" model and both textures are
	// applied), and registers it in the scene unless skipRegister.
	bool BuildCaddie(uint32_t typeId, uint32_t cadItemTypeId, bool skipRegister);

	WPuppet* m_puppet;                 // 0x8, GetPuppet's puppet; SetAlpha (0xb699a0) target of messages 0x1ce / 0x1cf
	int m_kind;                        // 0xc, the ctor's argument (12: SetTexturePart swaps by handle)
	std::string m_name;                // 0x10, MakeStr("LOBBY_CADDIE_%d_%d" / "LOBBY_MASCOT_%d_%d"), the scene element name
	WMatrix m_matrix;                  // 0x2c, the pet's matrix (LobbyCaddie::RotateCaddie 0x560330)
	float m_lightAngle;                // 0x5c, pi from SetMotionByName (IDummyPlayer's +0x60)
	int m_msgTexHandle;                // 0x60, the mascot's message texture (ChangeMascotMsgInTex), released by the destructor
	WVector m_position;                // 0x64, SetPosition's argument
	std::vector<uint32_t> m_list70;    // 0x70 (destroyed by the destructor; element type and use not traced)
	std::vector<uint32_t> m_list80;    // 0x80 (the same)
	PADDING(0x90, 0x94);
	w_motion_data* m_motion;           // 0x94, PlayMotion's motion
	float m_motionTime;                // 0x98, the motion time (PlayMotion: the smaller of s and e; Process adds dt)
	std::list<JpPetTexturePart*> m_texParts; // 0x9c (head 0x9c, size 0xa0), SetTexturePart's records
	JpPetTexturePart* m_curTexPart;    // 0xa8
	int m_unknownAC;                   // 0xac, zeroed by SetMotionByName
	bool m_motionPlaying;              // 0xb0, Display draws while set; set by the actors' attach
	bool m_motionHold;                 // 0xb1, Process runs while set; set by the actors' attach, cleared at a still motion's end
	bool m_motionReady;                // 0xb2, set by SetMotionByName, cleared by the destructor
	JpFxInterpreter* m_effectHost;     // 0xb4, the JP FxInterpreter (NULL, 0); the caddie attach plays "Talk_Room.seq" on it
	int m_effectId;                    // 0xb8, ResetMotion's effect
	PADDING(0xbc, 0xc4);
};

CHECK_SIZE(JpPetTexturePart, 0x94);
CHECK_FIELD(JpPetTexturePart, m_bitmap, 0x80);
CHECK_SIZE(IDummyPet, 0xc4);
BEGIN_LAYOUT_CHECK(IDummyPet)
	CHECK_FIELD(IDummyPet, m_puppet, 0x8);
	CHECK_FIELD(IDummyPet, m_kind, 0xc);
	CHECK_FIELD(IDummyPet, m_name, 0x10);
	CHECK_FIELD(IDummyPet, m_matrix, 0x2c);
	CHECK_FIELD(IDummyPet, m_lightAngle, 0x5c);
	CHECK_FIELD(IDummyPet, m_msgTexHandle, 0x60);
	CHECK_FIELD(IDummyPet, m_position, 0x64);
	CHECK_FIELD(IDummyPet, m_list70, 0x70);
	CHECK_FIELD(IDummyPet, m_list80, 0x80);
	CHECK_FIELD(IDummyPet, m_motion, 0x94);
	CHECK_FIELD(IDummyPet, m_texParts, 0x9c);
	CHECK_FIELD(IDummyPet, m_curTexPart, 0xa8);
	CHECK_FIELD(IDummyPet, m_motionPlaying, 0xb0);
	CHECK_FIELD(IDummyPet, m_effectHost, 0xb4);
	CHECK_FIELD(IDummyPet, m_effectId, 0xb8);
END_LAYOUT_CHECK

#endif // PG_JP_JPDUMMYPET_H
