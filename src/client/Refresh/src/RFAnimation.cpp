// Original: client\Refresh\src\RFAnimation.cpp (JP 983 exe; the file name from its asserts)
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <new>

#include "RFAnimation.h"
#include "RFAnimFrame.h"
#include "RFDirector.h"
#include "RFRect.h"
#include "RFStringUtil.h"
#include "RFVector2.h"
#include "tinyxml/tinyxml.h"
// 0x45b980
int GetAnimationObjectType(std::string const& type) {
	if (type == "IMAGE") {
		return 0;
	}
	if (type == "UI") {
		return 1;
	}
	if (type == "TEXT") {
		return 2;
	}
	if (type == "PLAYER") {
		return 3;
	}
	if (type == "OWNER") {
		return 4;
	}
	assert(!"Should not reach here.");
	return 0;
}

// The layout keeps the asserts on the original's lines 31, 90, 109 and 129.
// lib: 0x45d6c0 (_Uninitialized_copy wrapper of 0x45d8c0), 0x45d7a0 (empty
// _Destroy), 0x45d860 (allocator construct: RFAnimationObject copy
// constructor), 0x45d8b0 (_Ptr_cat-style return 0)
// lib: 0x45d5b0 (std::string operator==); std::vector<RFAnimationObject> helpers:
// 0x45d110, 0x45d120, 0x45d160, 0x45d180, 0x45d190, 0x45d1a0, 0x45d250,
// 0x45d270, 0x45d2e0, 0x45d330, 0x45d360, 0x45d480, 0x45d4a0, 0x45d4b0,
// 0x45d4d0, 0x45d510, 0x45d530, 0x45d540, 0x45d5a0, 0x45d620, 0x45d670,
// 0x45d6e0, 0x45d700, 0x45d750, 0x45d7b0, 0x45d810, 0x45d840, 0x45d8c0

// 0x45ba40 (LTCG inlines RFDirector::GetVersion 0x404200 and RFEventHandler() 0x42f5a0)
RFAnimation::RFAnimation(RFDirector* director)
	: m_version(0),
	  m_director(director),
	  m_time(0.0f),
	  m_playing(false), m_loop(false), m_pendulum(false),
	  m_reverse(0),
	  m_size(1.0f, 1.0f),
	  m_progress(0.0f) {
	m_startTime = m_endTime = 0.0f;
	m_version = m_director->GetVersion();
}

// 0x45baf0 scalar deleting destructor (paired as ??_G; used by RFDirector::UnloadAllAnimation)
// 0x45bb20
RFAnimation::~RFAnimation() {
	m_playing = false;
}

// 0x45bbf0 (LTCG inlines RFAnimFrame::SetCanvas and GenAnimMatrix 0x45cf40)
void RFAnimation::SetPosition(RFVector2 const& pos) {
	RFRect canvas;
	for (unsigned int i = 0; i < m_objects.size(); ++i) {
		canvas.left = pos.x;
		canvas.top = pos.y;
		canvas.right = m_size.width + pos.x;
		canvas.bottom = pos.y + m_size.height;
		m_objects[i].m_animFrame->SetCanvas(canvas);
	}
	GenAnimMatrix();
}

// 0x45bd20
void RFAnimation::Pause() {
	m_playing = false;
}

// 0x45bd30
unsigned int RFAnimation::GetObjectCount() const {
	return m_objects.size();
}

// 0x45bd50
RFAnimationObject* RFAnimation::GetObjectAt(unsigned int index) {
	if (index >= m_objects.size()) {
		assert(!"Parameter 'index' out of range.");
		return 0;
	}
	return &m_objects[index];
}

// 0x45bda0
RFAnimationObject* RFAnimation::FindObject(std::wstring const& name) {
	for (unsigned int i = 0; i < m_objects.size(); ++i) {
		if (m_objects[i].m_name == name) {
			return &m_objects[i];
		}
	}
	return 0;
}

// 0x45be30
RFAnimationObject const* RFAnimation::GetObjectAt(unsigned int index) const {
	if (index >= m_objects.size()) {
		assert(!"Parameter 'index' out of range.");
		return 0;
	}
	return &m_objects[index];
}

// 0x45be80
RFAnimationObject const* RFAnimation::FindObject(std::wstring const& name) const {
	for (unsigned int i = 0; i < m_objects.size(); ++i) {
		if (m_objects[i].m_name == name) {
			return &m_objects[i];
		}
	}
	return 0;
}

// 0x45bf10
R_ERRORTYPE RFAnimation::SetObjectRect(std::wstring const& name, RFRect const& rect) {
	RFAnimationObject* object = FindObject(name);
	if (object == 0) {
		assert(!"Object not found.");
		return R_FAIL;
	}
	object->m_rect = rect;
	return R_OK;
}

// 0x45bf60
float RFAnimation::GetProgress() const {
	return m_progress;
}

// 0x45bf70
void RFAnimation::LoadParam(rf::TiXmlNode const* node) {
	for (rf::TiXmlElement const* param = node->FirstChildElement("param"); param != 0;
	     param = param->NextSiblingElement("param")) {
		char const* type = param->Attribute("type");
		if (strcmp(type, "RES") == 0) {
			float width = (float)atof(param->Attribute("x"));
			float height = (float)atof(param->Attribute("y"));
			m_size.width = width;
			m_size.height = height;
		} else if (strcmp(type, "LOOP") == 0) {
			m_loop = atoi(param->Attribute("cnt")) == -1;
			m_pendulum = atoi(param->Attribute("pendulum")) != 0;
		}
	}
}

// 0x45c250 (LTCG inlines TiXmlElement::Attribute, the std::wstring move
// assignment and the vector's push_back.)
bool RFAnimation::Load(rf::TiXmlNode const* node) {
	for (rf::TiXmlAttribute const* attribute = node->ToElement()->FirstAttribute();
	     attribute != 0; attribute = attribute->Next()) {
		if (strcmp(attribute->Name(), "name") == 0) {
			m_name = Utf8ToWString(attribute->Value());
		} else if (strcmp(attribute->Name(), "loop") == 0) {
			m_loop = atoi(attribute->Value()) == 0;
		} else if (strcmp(attribute->Name(), "pingpong") == 0) {
			m_pendulum = strcmp(attribute->Value(), "true") == 0;
		}
	}
	LoadParam(node);
	rf::TiXmlElement const* element = node->FirstChildElement("object");
	while (element != 0) {
		std::wstring name = Utf8ToWString(element->Attribute("name"));
		RFAnimationObject object(this);
		object.m_name = name;
		object.m_animName = Utf8ToWString(element->Attribute("ani"));
		for (rf::TiXmlElement const* asset = element->FirstChildElement("asset"); asset != 0;
		     asset = asset->NextSiblingElement("asset")) {
			for (rf::TiXmlAttribute const* attribute = asset->FirstAttribute(); attribute != 0;
			     attribute = attribute->Next()) {
				if (strcmp(attribute->Name(), "link") == 0) {
					object.m_asset = Utf8ToWString(attribute->Value());
				}
			}
		}
		m_objects.push_back(object);
		element = element->NextSiblingElement("object");
	}
	for (int i = 0; i < (int)m_objects.size(); ++i) {
		m_objects[i].Load(node);
		if (m_objects[i].m_duration > m_endTime) {
			m_endTime = m_objects[i].m_duration;
		}
	}
	return true;
}

// 0x45ca80
void RFAnimation::Play() {
	unsigned int count = m_objects.size();
	if (count == 0) {
		m_playing = false;
		m_time = 0.0f;
		m_finishedCount = 0;
		return;
	}
	for (unsigned int i = 0; i < count; ++i) {
		RFAnimationObject& object = m_objects[i];
		if (!m_reverse) {
			object.m_animFrame->Play();
		} else {
			object.m_animFrame->PlayPendulum();
		}
	}
	if (!m_reverse) {
		m_time = 0.0f;
		m_progress = 0.0f;
	} else {
		m_time = m_endTime / 1000.0;
		m_progress = 1.0f;
	}
	m_playing = true;
	m_finishedCount = 0;
}

// 0x45cb20
bool RFAnimation::IsPlaying() const {
	return m_playing;
}

// 0x45cb30
void RFAnimation::TogglePause() {
	m_playing = !m_playing;
}

// 0x45cb40 (LTCG inlines RFAnimFrame::Stop and RFDirector::SendFinishEvent.)
void RFAnimation::Stop() {
	m_time = 0.0f;
	m_playing = false;
	m_reverse = 0;
	m_finishedCount = 0;
	for (unsigned int i = 0; i < m_objects.size(); ++i) {
		m_objects[i].m_animFrame->Stop();
	}
	m_director->SendFinishEvent(this);
}

// 0x45cca0 (LTCG inlines RFAnimFrame::End)
void RFAnimation::End() {
	m_time = 0.0f;
	m_playing = false;
	unsigned int count = m_objects.size();
	if (count == 0) {
		return;
	}
	for (unsigned int i = 0; i < count; ++i) {
		m_objects[i].m_animFrame->End(m_objects[i].m_rect);
	}
	m_time = m_endTime / 1000.0;
	m_progress = 1.0f;
}

// 0x45cd80
void RFAnimation::SetLoop(bool loop) {
	m_loop = loop;
}

// 0x45cd90
bool RFAnimation::IsLoop() const {
	return m_loop;
}

// 0x45cda0 (LTCG inlines RFAnimFrame::IsPlaying)
void RFAnimation::FrameMove(float elapsed) {
	if (!m_playing) {
		return;
	}
	if (m_finishedCount == m_objects.size()) {
		if (m_loop) {
			if (m_pendulum) {
				m_reverse = m_reverse == 0;
			}
			Play();
		} else {
			m_director->SendFinishEvent(this);
			m_progress = 1.0f;
			m_playing = false;
			m_finishedCount = 0;
			return;
		}
	}
	unsigned int ms = (unsigned int)(m_time * 1000.0 + 0.5);
	m_progress = (ms - m_startTime) / (m_endTime - m_startTime);
	m_finishedCount = 0;
	for (unsigned int i = 0; i < m_objects.size(); ++i) {
		RFAnimationObject& object = m_objects[i];
		RFAnimFrame* frame = object.m_animFrame;
		if (frame->IsPlaying()) {
			if (!m_reverse) {
				frame->FrameMove(ms, object.m_rect);
			} else {
				frame->FrameMovePendulum(ms, object.m_rect);
			}
		} else {
			++m_finishedCount;
		}
	}
	m_time += (m_reverse == 0 ? 1 : -1) * elapsed;
	if (m_time < 0.0f) {
		m_time = 0.0f;
	}
}

// 0x45cf40
void RFAnimation::GenAnimMatrix() {
	for (unsigned int i = 0; i < m_objects.size(); ++i) {
		RFAnimationObject& object = m_objects[i];
		object.m_animFrame->GenAnimMatrix(object.m_rect);
	}
}

// 0x45cfb0 (LTCG inlines RFAnimationObject::Render 0x42c7d0)
void RFAnimation::Render(float alpha) {
	for (unsigned int i = 0; i < m_objects.size(); ++i) {
		m_objects[i].Render(alpha);
	}
}
