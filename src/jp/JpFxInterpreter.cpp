// The JP 983 effect interpreter (JpFxInterpreter.h; JP RTTI name
// FxInterpreter), from the JP exe's bodies (node scripts/disasm.ts --jp
// 0x<va>).
#include "JpFxInterpreter.h"

#include <string.h>

#include <algorithm>

#include "fxbox.h"
#include "fxsequence.h"
#include "fxspray.h"

// lib: 0x88e830, 0x883b40 (the scalar deleting destructors)

// The live instances (JP 0xe7d5c4).
int JpFxInterpreter::s_instanceCount = 0;

namespace {

// The JP Attacher's inequality (both limits, then the four matrix /
// offset pointers); KR fx.h has no such member.
// 0x878d60
bool AttacherDiffers(const Attacher& a, const Attacher& b) {
	if (a.m_fLimitA == b.m_fLimitA && a.m_fLimitB == b.m_fLimitB) {
		for (int i = 0; i < 4; ++i) {
			if (a.m_pMatrix[i] != b.m_pMatrix[i] || a.m_pOffset[i] != b.m_pOffset[i])
				return true;
		}
		return false;
	}
	return true;
}

} // namespace

// The functor of ResetPartEffect's std::for_each.
// lib: 0x8868e0 (std::for_each<std::list<void*>::iterator, JpFxDetachFn>, the
// operator() below inlined)
struct JpFxDetachFn {
	explicit JpFxDetachFn(JpFxInterpreter* owner) : m_owner(owner) {}
	void operator()(void* fx) const;
	JpFxInterpreter* m_owner;
};

// 0x883010
JpFxInterpreter::sFxInfo::~sFxInfo() {
}

// 0x8846e0
JpFxInterpreter::sFxInfo::sFxInfo(int effectId, int id, int type, int group, void* fx, const Attacher* attacher, const char* name, bool flag, int param, const char* token)
	: m_unknown20(true), m_effectId(effectId), m_group(group), m_id(id), m_type(type), m_fx(fx),
	  m_attacher(*attacher), m_unknown7c(flag), m_param(param), m_unknown84(false), m_unknown85(true) {
	if (name != NULL)
		m_name.assign(name, strlen(name));
	if (token != NULL)
		m_token.assign(token, strlen(token));
}

// 0x8847e0
JpFxInterpreter::sFxInfo::sFxInfo(int id, int type, int group, const Attacher* attacher, const char* name, int param, const char* token)
	: m_unknown20(true), m_effectId(0), m_group(group), m_id(id), m_type(type), m_fx(NULL),
	  m_attacher(*attacher), m_unknown7c(false), m_param(param), m_unknown84(false), m_unknown85(true) {
	if (name != NULL)
		m_name.assign(name, strlen(name));
	if (token != NULL)
		m_token.assign(token, strlen(token));
}

// 0x8848e0
bool JpFxInterpreter::sFxInfo::IsSame(const sFxInfo& other) const {
	if (m_id != other.m_id || m_type != other.m_type || m_group != other.m_group)
		return false;
	if (m_name.compare(other.m_name) != 0)
		return false;
	if (m_token.compare(other.m_token) != 0)
		return false;
	if (AttacherDiffers(m_attacher, other.m_attacher))
		return false;
	return m_unknown7c == other.m_unknown7c;
}

// 0x884ac0
bool JpFxInterpreter::IsSameFx(sFxInfo query, const sFxInfo* stored) const {
	return query.IsSame(*stored);
}

// 0x884c80
void JpFxInterpreter::ResetFxInfo(void* fx) {
	if (fx != NULL) {
		std::map<void*, RF_SHARED_PTR<sFxInfo> >::iterator it = m_fxInfos.find(fx);
		if (it != m_fxInfos.end())
			it->second.reset();
	}
}

// 0x885570
void* JpFxInterpreter::FindSpray(std::string name, const char* token, const Attacher* attacher, int partTid, int layer, int param) {
	if ((unsigned int)layer < 4 && m_partEffects[layer].size() != 0) {
		std::map<int, std::list<void*> >::iterator it = m_partEffects[layer].find(partTid);
		if (it != m_partEffects[layer].end()) {
			for (std::list<void*>::iterator fx = it->second.begin(); fx != it->second.end(); ++fx) {
				void* effect = *fx;
				std::map<void*, RF_SHARED_PTR<sFxInfo> >::iterator info = m_fxInfos.find(effect);
				if (info != m_fxInfos.end() && info->second.get() != NULL) {
					sFxInfo* stored = info->second.get();
					if (IsSameFx(sFxInfo(stored->m_id, EFFECT_SPRAY, stored->m_group, attacher, name.c_str(), param, token), stored))
						return effect;
				}
			}
		}
	}
	return NULL;
}

// 0x8856c0
void* JpFxInterpreter::FindSequence(std::string name, const char* token, const Attacher* attacher, int partTid, int layer, int param) {
	if ((unsigned int)layer < 4 && m_partEffects[layer].size() != 0) {
		std::map<int, std::list<void*> >::iterator it = m_partEffects[layer].find(partTid);
		if (it != m_partEffects[layer].end()) {
			for (std::list<void*>::iterator fx = it->second.begin(); fx != it->second.end(); ++fx) {
				void* effect = *fx;
				std::map<void*, RF_SHARED_PTR<sFxInfo> >::iterator info = m_fxInfos.find(effect);
				if (info != m_fxInfos.end() && info->second.get() != NULL) {
					sFxInfo* stored = info->second.get();
					if (IsSameFx(sFxInfo(stored->m_id, EFFECT_SEQUENCE, stored->m_group, attacher, name.c_str(), param, token), stored))
						return effect;
				}
			}
		}
	}
	return NULL;
}

void JpFxDetachFn::operator()(void* fx) const {
	m_owner->DetachEffect(fx, NULL, true);
}

// 0x887c10
void JpFxInterpreter::ResetPartEffect(int layer, int partTid) {
	if ((unsigned int)layer < 4) {
		std::map<int, std::list<void*> >::iterator it = m_partEffects[layer].find(partTid);
		if (it != m_partEffects[layer].end()) {
			std::for_each(it->second.begin(), it->second.end(), JpFxDetachFn(this));
			it->second.clear();
		}
	}
}

// 0x887cb0
void JpFxInterpreter::ClearLayerPartTids(int layer) {
	int first = 0;
	int last = 4;
	if (layer > -1 && layer < 4) {
		first = layer;
		last = layer + 1;
	}
	for (int i = first; i < last; ++i) {
		for (std::list<int>::iterator tid = m_layerPartTids[i].begin(); tid != m_layerPartTids[i].end(); ++tid) {
			std::map<int, std::list<void*> >::iterator it = m_partEffects[i].find(*tid);
			if (it != m_partEffects[i].end())
				ResetPartEffect(i, it->first);
		}
		m_layerPartTids[i].clear();
	}
}

// 0x888d40
int JpFxInterpreter::GetEffectType(std::string name) {
	if (name.size() == 0 || name.size() >= 0x40)
		return EFFECT_NONE;
	std::string::size_type dot = name.rfind('.');
	std::string ext(name.begin() + (dot + 1), name.end());
	int type = EFFECT_NONE;
	if (ext.size() != 0 && ext.size() >= 3) {
		if (ext.compare("seq") == 0)
			type = EFFECT_SEQUENCE;
		else if (ext == "spr")
			type = EFFECT_SPRAY;
	}
	return type;
}

// 0x8890f0
void JpFxInterpreter::ReleaseEffects() {
	for (std::map<void*, RF_SHARED_PTR<sFxInfo> >::iterator it = m_fxInfos.begin(); it != m_fxInfos.end(); ++it)
		it->second.reset();
	m_fxInfos.clear();
}

// 0x889df0
void JpFxInterpreter::StopEffect(int effectId) {
	for (int layer = 0; layer < 4; ++layer)
		StopLayerEffect(effectId, layer);
}

// 0x88aa60
bool JpFxInterpreter::PlaySequence(const char* name, const WVector* position) {
	int type = GetEffectType(name);
	std::map<std::string, void*>::iterator it = m_playedEffects.find(name);
	void* previous = it != m_playedEffects.end() ? it->second : NULL;
	switch (type) {
	case EFFECT_SPRAY: {
		CFxSpray* spray = static_cast<CFxSpray*>(previous);
		if (spray != NULL && WSingleton<CFx>::Instance()->FindSpray(spray)) {
			WSingleton<CFx>::Instance()->CloseSpray(spray);
			m_playedEffects[name] = NULL;
		}
		spray = WSingleton<CFx>::Instance()->OpenSpray(name);
		if (spray == NULL)
			return false;
		spray->m_pos = *position;
		m_playedEffects[name] = spray;
		// Not written: for "wind_change1.spr", "wind_change4.spr" and
		// "wind_change9.spr" (0xd22dfc / 0xd22e10 / 0xd22e24) the JP then
		// sets the spray's rotation (0x880c50: the 0x30 bytes at +0x1160,
		// KR CFxSpray::m_matrix) to a random one built by 0x6c8080 on the
		// object 0x6cc5f0 returns (0xe78ff4), neither decompiled. No lobby
		// effect is one of them.
		return true;
	}
	case EFFECT_SEQUENCE: {
		CFxSequence* sequence = static_cast<CFxSequence*>(previous);
		if (sequence != NULL && WSingleton<CFx>::Instance()->FindSequence(sequence)) {
			WSingleton<CFx>::Instance()->CloseSequence(sequence, false);
			m_playedEffects[name] = NULL;
		}
		sequence = WSingleton<CFx>::Instance()->OpenSequence(name, true);
		if (sequence == NULL)
			return false;
		sequence->Pos() = *position;
		m_playedEffects[name] = sequence;
		return true;
	}
	}
	return false;
}

// 0x88b140
void JpFxInterpreter::ClearPartEffects(int layer) {
	int first = 0;
	int last = 4;
	if (layer > -1 && layer < 4) {
		first = layer;
		last = layer + 1;
	}
	for (int i = first; i < last; ++i) {
		for (std::map<int, std::list<void*> >::iterator it = m_partEffects[i].begin(); it != m_partEffects[i].end(); ++it)
			ResetPartEffect(i, it->first);
		m_partEffects[i].clear();
	}
}

// 0x88b1f0
void JpFxInterpreter::SetFxInfo(void* fx, sFxInfo* info) {
	if (fx != NULL && info != NULL) {
		ResetFxInfo(fx);
		m_fxInfos[fx] = RF_SHARED_PTR<sFxInfo>(info);
	}
}

// 0x88c420
void JpFxInterpreter::StopAllEffects(int effectId) {
	ClearPartEffects(effectId);
	ClearLayerPartTids(effectId);
	StopSprays(effectId);
	if (effectId == -1)
		m_nextId = 1;
}

// 0x88ca30
bool JpFxInterpreter::AddPartEffect(int id, void* fx, int partTid, int layer, bool flag) {
	(void)id;
	if (flag || (unsigned int)layer >= 4)
		return false;
	std::map<int, std::list<void*> >::iterator it = m_partEffects[layer].find(partTid);
	if (it != m_partEffects[layer].end()) {
		it->second.push_back(fx);
		return true;
	}
	std::list<void*> effects;
	effects.push_back(fx);
	m_partEffects[layer].insert(std::pair<const int, std::list<void*> >(partTid, effects));
	return true;
}

// 0x88cb80
bool JpFxInterpreter::StartFx(int id, int type, void* fx, const Attacher* attacher, const char* name, int partTid, int layer, int param5, const char* token) {
	if (param5 != 0)
		return false;
	if (!AddPartEffect(id, fx, partTid, layer, false))
		return false;
	SetFxInfo(fx, new sFxInfo(m_nextId, 0, type, partTid, fx, attacher, name, false, 0, token));
	if (partTid != 0)
		m_layerPartTids[layer].push_back(partTid);
	return true;
}

// 0x88d3c0
int JpFxInterpreter::AddEffect(const char* name, const char* token, const Attacher* attacher, int type, int param5, int partTid, int param7, bool keepId) {
	if (name == NULL || attacher == NULL)
		return 0;
	if (type == EFFECT_SPRAY) {
		void* fx = LoadFx(name, attacher, EFFECT_SPRAY, param5, true);
		if (fx == NULL)
			return 0;
		StartFx(m_nextId, EFFECT_SPRAY, fx, attacher, name, partTid, param7, param5, token);
		int id = m_nextId;
		if (!keepId)
			m_nextId = id + 1;
		return id;
	}
	if (type == EFFECT_SEQUENCE) {
		void* fx = LoadFx(name, attacher, EFFECT_SEQUENCE, param5, true);
		if (fx == NULL)
			return 0;
		StartFx(m_nextId, EFFECT_SEQUENCE, fx, attacher, name, partTid, param7, param5, token);
		int id = m_nextId;
		if (!keepId)
			m_nextId = id + 1;
		return id;
	}
	return 0;
}

// The body of the scalar deleting destructor 0x88e830. Not written yet:
// the 0x88a1c0 pass over the map arrays at +0xa8 / +0xe8 (after
// ReleasePartEffects) and 0x888060 on the +0x1d0 map (after the ClearAll),
// whose element types are not traced (PADDING in the header).
// 0x88dee0
JpFxInterpreter::~JpFxInterpreter() {
	--s_instanceCount;
	StopAllEffects(-1);
	ReleaseEffects();
	ReleasePartEffects();
	if (m_fxBox != NULL)
		m_fxBox->ClearAll(false);
	else
		WSingleton<CFx>::Instance()->ClearAll(s_instanceCount != 0);
}

// 0x88e520
JpFxInterpreter::JpFxInterpreter(CFxBox* fxBox, int firstId)
	: m_unknown4(0), m_nextId(firstId + 1), m_unknown158(1) {
	m_fxBox = fxBox;
	InitTokens();
	RegisterCallback(fxBox);
	if (fxBox != NULL)
		SetFxBox(fxBox);
	++s_instanceCount;
}

// Port placeholders (G7) for the value-returning members the lobby avatar
// reaches (IDummyPlayer::RefreshAfterPartChange, AddEffect) until their JP
// bodies are decompiled: no effect is attached or bound.
// Port body (JP 0x883bb0, JpFxInterpreter.h): the CFx singleton path is the
// JP one; the own-box path (m_fxBox != NULL: CFxBox::LoadSpray /
// LoadSequence, the attacher copied into the effect's protected members,
// AddSpray / AddSequence) needs CFxSpray / CFxSequence to befriend this
// class and returns NULL (G7) until they do. The lobby avatar's host has no
// box.
void* JpFxInterpreter::LoadFx(const char* name, const Attacher* attacher, int type, int param5, bool flag) {
	if ((int)strlen(name) < 5)
		return NULL;
	if (m_fxBox != NULL)
		return NULL;
	void* fx = WSingleton<CFx>::Instance()->Attach(*attacher, name, param5 != 0, NULL);
	if (fx == NULL)
		return NULL;
	if (type == EFFECT_SPRAY)
		static_cast<CFxSpray*>(fx)->SetActive(flag);
	else
		static_cast<CFxSequence*>(fx)->SetActive(flag, false);
	return fx;
}

// Port placeholder (G7; JP 0x88ecd0, JpFxInterpreter.h): effect id 0 (none).
int JpFxInterpreter::AttachRange(CPetFrame* frame, const float* range, int param1, int param2) {
	(void)frame;
	(void)range;
	(void)param1;
	(void)param2;
	return 0;
}

// Port placeholder (G7; JP 0x88f2b0, JpFxInterpreter.h): effect id 0 (none).
int JpFxInterpreter::AttachFrame(CPetFrame* frame, const void* param1, const void* param2, bool flag, int param3) {
	(void)frame;
	(void)param1;
	(void)param2;
	(void)flag;
	(void)param3;
	return 0;
}

// Port placeholder (G7; JP 0x88fe40, JpFxInterpreter.h).
bool JpFxInterpreter::BindOwner(void* owner, const char* name, void* param) {
	(void)owner;
	(void)name;
	(void)param;
	return false;
}
