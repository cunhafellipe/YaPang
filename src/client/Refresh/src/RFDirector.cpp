// Original: client\Refresh\src\RFDirector.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <new>

#include "RFAnimation.h"
#include "RFAnimFrame.h"
#include "RFDirector.h"
#include "RFEventArgs.h"
#include "RFInterface.h"
#include "RFManager.h"
#include "RFVector2.h"
#include "RFWindowModifier.h"
#include "RFWnd.h"
#include "tinyxml/tinyxml.h"

// lib: 0x41bc20 (std::vector<char> allocation), 0x41ac50 (~vector<char>),
// 0x431aa0 (std::vector<RFAnimation*>::push_back)

std::wstring const RFDirector::EventAnimFinished = L"AnimFinished";

// 0x404200
uint32_t RFDirector::GetVersion() const {
	return m_version;
}

// 0x404210
RFRenderer* RFDirector::GetRenderer() {
	return m_renderer;
}

// 0x404220
RFInterface* RFDirector::GetInterface() {
	return m_interface;
}

// 0x4307f0 (the original's split keeps only its first 16 bytes: the body
// continues at 0x430800)
RFDirector::RFDirector()
	: m_version(0),
	  m_renderer(RFManager::GetInstance().GetRenderer()),
	  m_interface(RFManager::GetInstance().GetInterface()),
	  m_playingScene(0) {
}

// 0x430910
RFDirector::~RFDirector() {
	UnloadAllAnimation();
	for (std::map<RFWnd*, RFWindowModifier*>::iterator it = m_modifiers.begin();
	     it != m_modifiers.end(); ++it) {
		delete it->second;
	}
}

// 0x430a10
void RFDirector::SendFinishEvent(RFAnimation* animation) {
	AnimationEventArgs args;
	args.animationName = animation->m_name;
	SendEvent(EventAnimFinished, args);
}

// 0x430ad0
void RFDirector::UnloadAllAnimation() {
	for (unsigned int i = 0; i < m_scenes.size(); ++i) {
		if (m_scenes[i] != 0) {
			delete m_scenes[i];
			m_scenes[i] = 0;
		}
	}
	m_scenes.clear();
}

// 0x430b50 (the original's split keeps its first 132 bytes; the call of
// LoadAnimation_ continues at 0x430bd4).
R_ERRORTYPE RFDirector::LoadAnimation(std::wstring const& name) {
	std::wstring path = m_interface->GetBasePath(RF_BASEPATH_ANIMATION) + name;
	return LoadAnimation_(path);
}

// 0x430c20
R_ERRORTYPE RFDirector::LoadAnimationInDirect(std::wstring const& path) {
	return LoadAnimation_(path);
}

// The file is an XML document:
//   <rfanimation version="major.minor">
//     <scene ...> ... </scene>   (each read by RFAnimation::Load)
//   </rfanimation>
// The version packs major in the high word and minor in the low word; the
// scenes are read only from version 1.2 (0x10002) on. Every failure (no
// file, an empty file, an XML error, no <rfanimation> root) returns
// R_LOAD_FAILED; the scenes loaded are appended to m_scenes.
// The original was built with LTCG: it keeps ~TiXmlDocument out of line
// (0x4306c0), inlines TiXmlAttribute::Next and NextSiblingElement, and
// realigns its frame (and esp,-8).
// 0x430c30
R_ERRORTYPE RFDirector::LoadAnimation_(std::wstring const& path) {
	void* file = m_interface->FOpen(path);
	if (file == 0) {
		return R_LOAD_FAILED;
	}
	m_interface->FSeek(file, 0, SEEK_END);
	int32_t size = m_interface->FTell(file);
	m_interface->FSeek(file, 0, SEEK_SET);
	if (size <= 0) {
		m_interface->FClose(file);
		return R_LOAD_FAILED;
	}
	std::vector<char> buffer(size + 1, 0);
	m_interface->FRead(file, &buffer[0], size);
	m_interface->FClose(file);

	rf::TiXmlDocument document;
	document.Parse(&buffer[0], 0, rf::TIXML_DEFAULT_ENCODING);
	if (document.Error()) {
		return R_LOAD_FAILED;
	}
	rf::TiXmlElement* root = document.FirstChildElement("rfanimation");
	if (root == 0) {
		return R_LOAD_FAILED;
	}
	for (rf::TiXmlAttribute* attribute = root->FirstAttribute(); attribute != 0;
	     attribute = attribute->Next()) {
		if (strcmp(attribute->Name(), "version") == 0) {
			int major;
			int minor;
			sscanf_s(attribute->Value(), "%d.%d", &major, &minor);
			m_version = (uint16_t)major << 16 | (uint16_t)minor;
		}
	}
	if (m_version >= 0x10002) {
		for (rf::TiXmlElement* element = root->FirstChildElement(); element != 0;
		     element = element->NextSiblingElement()) {
			if (strcmp(element->Value(), "scene") == 0) {
				RFAnimation* animation = new RFAnimation(this);
				animation->Load(element);
				m_scenes.push_back(animation);
			}
		}
	}
	return R_OK;
}

// 0x430fa0 (LTCG inlined RFAnimation::SetPosition 0x45bbf0 here, with
// RFAnimFrame::SetCanvas and RFAnimation::GenAnimMatrix: the body of
// 0x45bbf0 is repeated once per scene)
void RFDirector::SetPosition(RFVector2 const& pos) {
	for (unsigned int i = 0; i < m_scenes.size(); ++i) {
		m_scenes[i]->SetPosition(pos);
	}
}

// 0x431140
int RFDirector::GetAnimationCount() const {
	return m_scenes.size();
}

// 0x431150
RFAnimation* RFDirector::GetAnimation(int index) const {
	if ((unsigned int)index >= m_scenes.size()) {
		assert(!"Parameter 'index' out of range.");
		return 0;
	}
	return m_scenes[index];
}

// The first scene named name (RFAnimation::m_name), 0 when there is none.
// 0x431190
RFAnimation* RFDirector::GetAnimation(std::wstring const& name) const {
	for (unsigned int i = 0; i < m_scenes.size(); ++i) {
		if (m_scenes[i]->m_name == name) {
			return m_scenes[i];
		}
	}
	return 0;
}

// True when a scene is named name.
// 0x431200
bool RFDirector::IsSceneExist(std::wstring const& name) const {
	return GetAnimation(name) != 0;
}

// FindScene is GetAnimation(name), const and not.
// 0x431220
RFAnimation* RFDirector::FindScene(std::wstring const& name) {
	return GetAnimation(name);
}

// 0x431230
RFAnimation const* RFDirector::FindScene(std::wstring const& name) const {
	return GetAnimation(name);
}

// 0x431240
RFAnimation* RFDirector::GetPlayingScene() {
	return m_playingScene;
}

// Restarts the scene named name (Stop, then Play); R_FAIL when there is none.
// 0x431250
R_ERRORTYPE RFDirector::Play(std::wstring const& name) {
	RFAnimation* animation = GetAnimation(name);
	if (animation == 0) {
		return R_FAIL;
	}
	m_playingScene = animation;
	animation->Stop();
	m_playingScene->Play();
	return R_OK;
}

// Plays the named scene for wnd; FrameMove(RFWnd*, float) applies it.
// 0x431290 (returns R_FAIL whether it played or not)
R_ERRORTYPE RFDirector::Play(RFWnd* wnd, std::wstring const& name) {
	if (wnd != 0) {
		RFAnimation* animation = GetAnimation(name);
		if (animation != 0) {
			m_playingScene = animation;
			animation->Play();
		}
	}
	return R_FAIL;
}

// Plays the scene at index without restarting it (LTCG: this in esi).
// 0x4312c0
R_ERRORTYPE RFDirector::Play(int index) {
	if (index >= 0 && index < GetAnimationCount()) {
		m_playingScene = m_scenes[index];
		m_playingScene->Play();
		return R_OK;
	}
	assert(!"Parameter 'index' out of range.");
	return R_FAIL;
}

// Stops the playing scene, if any.
// 0x431310
void RFDirector::Stop() {
	if (m_playingScene) {
		m_playingScene->Stop();
	}
}

// 0x431320
bool RFDirector::IsPlaying(std::wstring const& name) const {
	RFAnimation* animation = GetAnimation(name);
	if (animation == 0) {
		return false;
	}
	return animation->m_playing;
}

// 0x431340
bool RFDirector::IsPlaying() const {
	if (m_playingScene) {
		return m_playingScene->m_playing;
	}
	return false;
}

// Ends the named scene, if any.
// 0x431350
void RFDirector::End(std::wstring const& name) {
	RFAnimation* animation = GetAnimation(name);
	if (animation) {
		animation->End();
	}
}

// 0x431370
void RFDirector::End() {
	if (m_playingScene) {
		m_playingScene->End();
	}
}

// Sets whether the named scene loops.
// 0x431380
void RFDirector::Loop(std::wstring const& name, bool loop) {
	RFAnimation* animation = GetAnimation(name);
	if (animation) {
		animation->m_loop = loop;
	}
}

// 0x4313a0
void RFDirector::Loop(int index, bool loop) {
	if ((unsigned int)index < m_scenes.size()) {
		m_scenes[index]->m_loop = loop;
	}
}

// 0x4313d0
bool RFDirector::IsLoop(std::wstring const& name) const {
	RFAnimation* animation = GetAnimation(name);
	if (animation == 0) {
		assert(!"Animation not found.");
		return false;
	}
	return animation->m_loop;
}

// 0x431410
bool RFDirector::IsLoop() const {
	if (m_playingScene) {
		return m_playingScene->m_loop;
	}
	return false;
}

// 0x431420
void RFDirector::FrameMove(float elapsed) {
	if (m_playingScene) {
		m_playingScene->FrameMove(elapsed);
	}
}

// The original's LTCG build inlines RFAnimation::GetObjectAt (with its
// assert), RFAnimFrame::GetNowFrame, the RFWnd accessors, ShowWindow_ and the
// RFWindowModifier constructor here, and realigns its frame (and esp,-8).
// 0x431440
void RFDirector::FrameMove(RFWnd* wnd, float elapsed) {
	if (IsPlaying() && wnd != 0) {
		FrameMove(elapsed);
		RFAnimation* animation = GetPlayingScene();
		std::wstring name;
		RFSize canvas = animation->m_size;
		for (unsigned int i = 0; i < animation->GetObjectCount(); ++i) {
			RFAnimationObject* object = animation->GetObjectAt(i);
			name = object->m_name;
			RFWnd* target;
			if (name == wnd->GetName()) {
				target = wnd;
			} else {
				target = wnd->GetChild(name);
			}
			if (target != 0) {
				RFAnimationFrame frame;
				object->m_animFrame->GetNowFrame(&frame);
				RFWindowModifier* modifier;
				std::map<RFWnd*, RFWindowModifier*>::iterator it = m_modifiers.find(target);
				if (it == m_modifiers.end()) {
					modifier = new RFWindowModifier;
					modifier->SetName(L"AnimationWindowModifier");
					target->AddModifier(modifier);
					m_modifiers.insert(std::make_pair(target, modifier));
				} else {
					modifier = it->second;
				}
				modifier->SetAlpha(frame.m_alpha.m_value);
				if (target->GetAnimationMask() & 1) {
					target->ShowWindow_(frame.m_visible);
				}
				WndType type = target->GetType();
				if (type == WndType_Base || type == WndType_WndEx || type == WndType_Picture) {
					RFRect rect = object->m_rect;
					RFSize initialSize = target->GetInitialSize();
					rect.right *= frame.m_scaleX.m_value;
					rect.bottom *= frame.m_scaleY.m_value;
					RFSize size((float)(int)rect.Width() - (int)initialSize.width,
					            (float)(int)rect.Height() - (int)initialSize.height);
					modifier->SetSize(size);
					target->OnLayoutChanged(2);
				}
				RFVector2 localPos = target->GetLocalPos();
				RFVector2 initialPos = target->GetInitialPos();
				RFVector2 offset = localPos - initialPos;
				// Each step is stored as a float (the original's fstp points).
				float x = frame.m_x.m_value * canvas.width + 0.5;
				x -= localPos.x;
				x += offset.x;
				float y = frame.m_y.m_value * canvas.height + 0.5;
				y -= localPos.y;
				y += offset.y;
				RFVector2 pos((float)(int)x, (float)(int)y);
				modifier->SetPos(pos);
				target->OnLayoutChanged(1);
			}
		}
	}
}

// 0x431a50
void RFDirector::Render(float elapsed) {
	if (m_playingScene) {
		m_playingScene->Render(elapsed);
	}
}

// lib: 0x4043a0 (RFDirector's scalar deleting destructor ??_G: calls
// ~RFDirector 0x430910, then operator delete when bit 0 is set; no caller,
// the vtable holds the vector deleting destructor 0x4043d0; our cl does not
// emit an unreferenced ??_G)
// lib: 0x431d40 (std::_Tree node value accessor, node+0xc in eax, unreferenced)
// lib: std:: instantiations of m_scenes (std::vector<RFAnimation*>) and
// m_modifiers (std::map<RFWnd*, RFWindowModifier*>): 0x431a70 (_Vector_val
// ctor), 0x431a80, 0x431a90 (operator[]), 0x431b00, 0x431ca0 (_Tree_val ctor:
// head node 0x18), 0x431bd0, 0x431d80, 0x431e80 (null iterator), 0x431be0,
// 0x431d70, 0x431e70 (iterator operator->), 0x431bf0 (iterator ++),
// 0x432220, 0x432260 (iterator --), 0x431c00 (vector _Inside), 0x431c80,
// 0x431c90, 0x431d50, 0x431ed0, 0x431ef0, 0x432320 (empty members),
// 0x431d10 (lower_bound), 0x431e40 (_Lbound), 0x431d60 (std::less<RFWnd*>),
// 0x431e10 (vector _Grow_to), 0x431e90, 0x432300 (pair ctor), 0x432230,
// 0x432270 (pair<iterator, bool> ctor), 0x431eb0 (_Tree insert), 0x431ee0
// (allocator construct), 0x431f00 (_Umove: memmove), 0x432250, 0x432290
// (max_size 0x1fffffff)
