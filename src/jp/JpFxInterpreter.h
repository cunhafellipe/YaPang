// The JP 983 effect interpreter: JP RTTI name "FxInterpreter"
// (.?AVFxInterpreter@@ 0xe25080, COL 0xd7a3a8, vtable 0xd34928, 2 slots),
// 0x1e0 bytes, code about 0x879000-0x890000. It is a JP rewrite of the KR
// FxInterpreter (yapang fxinterpreter.h, 0x14 bytes), which the JP port
// also compiles, hence the Jp prefix. The lobby avatar (IDummyPlayer +0xd0)
// owns one built with (NULL, 0).
// From the JP exe's bodies (node scripts/disasm.ts --jp 0x<va>).
#ifndef PG_JP_JPFXINTERPRETER_H
#define PG_JP_JPFXINTERPRETER_H

#include <stdint.h>
#include <list>
#include <map>
#include <string>

#include "defines.h"
#include "padding.h"
#include "fx.h"
#include "RefreshSharedPtr.h"

class CFxBox;
class CPetFrame;
class WVector;

class JpFxInterpreter {
	LAYOUT_CHECK_FRIEND;
public:
	// What an effect is (AddEffect's type; FindSpray matches type 2,
	// FindSequence type 1), as in the KR FxInterpreter::eEffectType.
	enum eEffectType {
		EFFECT_NONE = 0,
		EFFECT_SEQUENCE = 1,                   // ".seq"
		EFFECT_SPRAY = 2                       // ".spr"
	};

	// JP RTTI .?AUsFxInfo@FxInterpreter@@ (vtable 0xd34910, 1 slot), 0x88
	// bytes: what started an effect. m_fxInfos maps each running effect to
	// its own; FindSpray / FindSequence build one for the query and compare.
	struct sFxInfo {
		// 0x8847e0
		sFxInfo(int id, int type, int group, const Attacher* attacher, const char* name, int param, const char* token);
		// 0x8846e0: StartFx's (m_nextId, 0, type, part tid, the effect,
		// attacher, name, false, 0, token).
		sFxInfo(int effectId, int id, int type, int group, void* fx, const Attacher* attacher, const char* name, bool flag, int param, const char* token);
		virtual ~sFxInfo();                    // slot 0 (its scalar deleting destructor is lib)
		// 0x8848e0
		bool IsSame(const sFxInfo& other) const;

		std::string m_name;                    // 0x4, the effect file
		bool m_unknown20;                      // 0x20, 1 from the ctor
		int m_effectId;                        // 0x24, StartFx: the id AddEffect returns; 0 from ctor 0x8847e0
		int m_group;                           // 0x28, compared; role not traced
		int m_id;                              // 0x2c, compared (AddEffect's id)
		int m_type;                            // 0x30, eEffectType
		void* m_fx;                            // 0x34, StartFx: the effect; NULL from ctor 0x8847e0
		std::string m_token;                   // 0x38
		Attacher m_attacher;                   // 0x54, a copy of the caller's
		bool m_unknown7c;                      // 0x7c, 0 from the ctors, compared
		int m_param;                           // 0x80
		bool m_unknown84;                      // 0x84, 0 from the ctor
		bool m_unknown85;                      // 0x85, 1 from the ctor
		PADDING(0x86, 0x88);
	};

	// 0x88e520: fxBox NULL = the effects go to the CFx singleton.
	JpFxInterpreter(CFxBox* fxBox, int firstId);
	virtual ~JpFxInterpreter();                // slot 0 (its scalar deleting destructor is lib)
	// [1] 0x884e10: not decompiled (takes one pointer; looks it up in
	// m_fxInfos).
	virtual void OnFxDeleted(void* fx);

	// 0x88aa60: opens a ".seq" or ".spr" file on the CFx singleton at a
	// world position (not attached), closing the one this name opened
	// before; false when it is neither or could not be opened. The lobby
	// avatar attach 0x566390 plays "avatar_d.seq", the caddie attach
	// 0x560a60 "Talk_Room.seq".
	bool PlaySequence(const char* name, const WVector* position);
	// 0x889df0: stops the effect in each of the four layers.
	void StopEffect(int effectId);
	// 0x88c420: resets the parts' effects of one layer (0..3), or of every
	// layer with -1 (then the ids restart at 1); StopSprays takes the same
	// layer argument.
	void StopAllEffects(int effectId);
	// 0x88ecd0 (1496 bytes): (pet frame, a two-float range, 1, 0) -> effect
	// id. Not decompiled.
	int AttachRange(CPetFrame* frame, const float* range, int param1, int param2);
	// 0x88f2b0 (1575 bytes): (pet frame, NULL, NULL, flag, 0) -> effect id.
	// Not decompiled.
	int AttachFrame(CPetFrame* frame, const void* param1, const void* param2, bool flag, int param3);
	// 0x88fe40 (165 bytes + tail chunk 0x88fecd): binds a named handler of
	// the owner (layer in 0..3); returns whether it did. Not decompiled.
	bool BindOwner(void* owner, const char* name, void* param);
	// 0x885570: the running spray (name, token, attacher) bound to part tid
	// in a layer; NULL without one.
	void* FindSpray(std::string name, const char* token, const Attacher* attacher, int partTid, int layer, int param);
	// 0x8856c0: the same for sequences.
	void* FindSequence(std::string name, const char* token, const Attacher* attacher, int partTid, int layer, int param);
	// 0x88d3c0: starts an effect; its id, 0 when it could not be loaded.
	// keepId does not advance the next id.
	int AddEffect(const char* name, const char* token, const Attacher* attacher, int type, int param5, int partTid, int param7, bool keepId);
	// 0x887c10: detaches the effects bound to part tid in a layer.
	void ResetPartEffect(int layer, int partTid);

private:
	friend struct JpFxDetachFn;

	// 0x88c6b0: adds the script tokens ("*playif" ...) to the containers at
	// +0x8. Not decompiled.
	void InitTokens();
	// 0x880d40: registers the creation callback 0x880c10 on the box (or on
	// the CFx singleton). Not decompiled.
	void RegisterCallback(CFxBox* fxBox);
	// 0x87f2d0: keeps the box and sets its light. Not decompiled.
	void SetFxBox(CFxBox* fxBox);
	// 0x888d40: EFFECT_SEQUENCE for a ".seq" name, EFFECT_SPRAY for ".spr",
	// EFFECT_NONE otherwise (empty or 0x40+ characters too).
	static int GetEffectType(std::string name);
	// 0x884c80: drops the info kept for an effect (its entry stays).
	void ResetFxInfo(void* fx);
	// 0x88b1f0: keeps the info of an effect (owned from then on).
	void SetFxInfo(void* fx, sFxInfo* info);
	// 0x88ca30: binds an effect to part tid in a layer; false for a layer
	// past 3 or with flag set (id is not read).
	bool AddPartEffect(int id, void* fx, int partTid, int layer, bool flag);
	// 0x884ac0: info against stored (the query by value).
	bool IsSameFx(sFxInfo query, const sFxInfo* stored) const;
	// 0x885840: detaches one effect. Not decompiled.
	void DetachEffect(void* fx, void* param, bool param3);
	// 0x888f00: stops the effect in one layer. Not decompiled.
	void StopLayerEffect(int effectId, int layer);
	// 0x88b140: ResetPartEffect on every part of a layer (-1: of every
	// layer), then empties its map.
	void ClearPartEffects(int layer);
	// 0x887cb0: the same for the part tids StartFx bound in a layer, then
	// empties its list.
	void ClearLayerPartTids(int layer);
	// 0x88b8d0: the third pass of StopAllEffects, over the map arrays at
	// +0xa8 / +0xe8 (untyped). Not decompiled.
	void StopSprays(int layer);
	// 0x8890f0: drops every kept info and empties m_fxInfos.
	void ReleaseEffects();
	// 0x887f80: a std::for_each (0x8851f0) over the list at +0x128, then
	// its clear. Not decompiled (the list is untyped).
	void ReleasePartEffects();
	// 0x883bb0: loads the effect file (name, attacher, type, param5, 1).
	// Not decompiled.
	void* LoadFx(const char* name, const Attacher* attacher, int type, int param5, bool flag);
	// 0x88cb80: binds a loaded effect to part tid in a layer and keeps what
	// started it; does nothing (false) when param5 is set, which every
	// lobby AddEffect passes.
	bool StartFx(int id, int type, void* fx, const Attacher* attacher, const char* name, int partTid, int layer, int param5, const char* token);

	// 0xe7d5c4: live instances (the destructor clears the CFx effects only
	// with the last one gone).
	static int s_instanceCount;

	int m_unknown4;                            // 0x4, 0 from the ctor
	// 0x8: two 0x10-byte containers (ctor 0x448780, dtor 0x448860), the
	// script tokens of InitTokens.
	PADDING(0x8, 0x28);
	std::map<int, std::list<void*> > m_partEffects[4]; // 0x28, by layer: part tid -> its effects
	std::list<int> m_layerPartTids[4];         // 0x68, by layer: the part tids StartFx bound (ctor 0x5b51c0)
	std::map<void*, RF_SHARED_PTR<sFxInfo> > m_fxInfos; // 0x98, effect -> what started it
	// 0xa8, 0xe8: two arrays of four maps (ctor 0x88c520, cleared by
	// 0x88a1c0 in the destructor); 0x128 list (0x10 nodes), 0x134 map
	// (0x24 nodes), 0x144 list (0x10 nodes).
	PADDING(0xa8, 0x150);
	CFxBox* m_fxBox;                           // 0x150, NULL = the CFx singleton
	int m_nextId;                              // 0x154, firstId + 1; AddEffect's ids
	int m_unknown158;                          // 0x158, 1 from the ctor
	// 0x15c: four std::string (ctor 0x41a3b0).
	PADDING(0x15c, 0x1d0);
	// 0x1d0: what PlaySequence last opened under each file name (a
	// CFxSequence* or a CFxSpray*; NULL once closed).
	std::map<std::string, void*> m_playedEffects;
	PADDING(0x1dc, 0x1e0);
};

CHECK_SIZE(JpFxInterpreter, 0x1e0);
BEGIN_LAYOUT_CHECK(JpFxInterpreter)
	CHECK_FIELD(JpFxInterpreter, m_partEffects, 0x28);
	CHECK_FIELD(JpFxInterpreter, m_layerPartTids, 0x68);
	CHECK_FIELD(JpFxInterpreter, m_fxInfos, 0x98);
	CHECK_FIELD(JpFxInterpreter, m_fxBox, 0x150);
	CHECK_FIELD(JpFxInterpreter, m_nextId, 0x154);
	CHECK_FIELD(JpFxInterpreter, m_playedEffects, 0x1d0);
END_LAYOUT_CHECK
CHECK_SIZE(JpFxInterpreter::sFxInfo, 0x88);
CHECK_FIELD(JpFxInterpreter::sFxInfo, m_group, 0x28);
CHECK_FIELD(JpFxInterpreter::sFxInfo, m_token, 0x38);
CHECK_FIELD(JpFxInterpreter::sFxInfo, m_attacher, 0x54);
CHECK_FIELD(JpFxInterpreter::sFxInfo, m_param, 0x80);

#endif // PG_JP_JPFXINTERPRETER_H
