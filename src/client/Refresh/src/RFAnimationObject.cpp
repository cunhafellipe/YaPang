// Original: client\Refresh\src\RFAnimationObject.cpp (JP 983 exe, unit bounds
// in jp/refresh_units.tsv; the file name is assumed from the class)
#include <stdlib.h>
#include <string.h>

#include <new>
#include <string>
#include <vector>

#include "RFAnimationObject.h"

#include "RFAnimation.h"
#include "RFAnimFrame.h"
#include "RFBezierSpline.h"
#include "RFEventArgs.h"
#include "RFInterpolation.h"
#include "RFManager.h"
#include "RFRenderer.h"
#include "RFStringUtil.h"
#include "RFVector2.h"
#include "tinyxml/tinyxml.h"

// The name of the event Render raises (the unnamed static wstring at 0xe470a8).
static std::wstring const EventRender;

// lib: 0x4041a0 (vector deleting destructor), 0x42ae50 (AnimDrawEventArgs'
// scalar deleting destructor), the implicit copy constructor 0x403f10 and
// operator= 0x4040b0, AnimEventArgs' scalar deleting destructor 0x42adf0,
// the scalar deleting destructor 0x404170 (paired as ??_G), AnimEventArgs'
// implicit destructor 0x42ae20 (this in eax, unreferenced; not emitted here,
// so unpaired), AnimDrawEventArgs' implicit destructor 0x42ae80 (paired; this
// in eax, an LTCG copy)
// 0x42ae90 (unreferenced `add eax,0x68; ret`: a const& accessor of a member at
// 0x68, likely RFAnimation::m_size; no such accessor is known, not written)

// 0x403e30
RFAnimationObject::RFAnimationObject(RFAnimation* animation)
	: m_type(0),
	  m_duration(0.0f),
	  m_animFrame(0),
	  m_animation(animation),
	  m_texture(0) {
}

// 0x403ef0
std::wstring const& RFAnimationObject::name() const {
	return m_name;
}

// 0x403f00
std::wstring const& RFAnimationObject::asset() const {
	return m_asset;
}

// 0x42af20 (RFAnimationFrame's, defined here: its copy sits in this file's
// address range and Load inlines it)
// Each default is copied in as a whole RFFloatAnimationFrame (value, no
// interpolation): the original moves every value through a stack slot.
void RFAnimationFrame::Clear() {
	m_time = 0;
	m_visible = true;
	m_sfx.clear();
	m_x = RFFloatAnimationFrame(0.0f);
	m_y = RFFloatAnimationFrame(0.0f);
	m_centerX = RFFloatAnimationFrame(0.5f);
	m_centerY = RFFloatAnimationFrame(0.5f);
	m_scaleX = RFFloatAnimationFrame(1.0f);
	m_scaleY = RFFloatAnimationFrame(1.0f);
	m_rotation = RFFloatAnimationFrame(0.0f);
	m_alpha = RFFloatAnimationFrame(1.0f);
}

// 0x42afc0
RFAnimationObject::~RFAnimationObject() {
	if (m_texture != 0) {
		RFManager::GetInstance().GetRenderer()->DeleteTexture(m_texture);
	}
	if (m_animFrame != 0) {
		delete m_animFrame;
		m_animFrame = 0;
	}
}

// 0x42b0f0
void RFAnimationObject::Load(rf::TiXmlNode const* node) {
	rf::TiXmlElement const* asset = node->FirstChildElement("asset");
	while (asset != 0) {
		std::wstring name = Utf8ToWString(asset->Attribute("name"));
		if (m_asset == name) {
			char const* type = asset->Attribute("type");
			if (strcmp(type, "IMAGE") == 0) {
				float left = 0.0f;
				float top = 0.0f;
				float right = 0.0f;
				float bottom = 0.0f;
				for (rf::TiXmlAttribute const* attribute = asset->FirstChildElement("rect")->FirstAttribute();
				     attribute != 0; attribute = attribute->Next()) {
					if (strcmp(attribute->Name(), "l") == 0) {
						left = (float)atoi(attribute->Value());
					} else if (strcmp(attribute->Name(), "t") == 0) {
						top = (float)atoi(attribute->Value());
					} else if (strcmp(attribute->Name(), "r") == 0) {
						right = (float)atoi(attribute->Value());
					} else if (strcmp(attribute->Name(), "b") == 0) {
						bottom = (float)atoi(attribute->Value());
					}
				}
				m_rect = RFRect(left, top, right, bottom);
				m_type = 0;
				m_texture = RFManager::GetInstance().GetRenderer()->CreateTexture(
				    Utf8ToWString(asset->Attribute("path")));
			} else if (strcmp(type, "UI") == 0) {
				float left = 0.0f;
				float top = 0.0f;
				float right = 0.0f;
				float bottom = 0.0f;
				for (rf::TiXmlAttribute const* attribute = asset->FirstChildElement("rect")->FirstAttribute();
				     attribute != 0; attribute = attribute->Next()) {
					if (strcmp(attribute->Name(), "l") == 0) {
						left = (float)atoi(attribute->Value());
					} else if (strcmp(attribute->Name(), "t") == 0) {
						top = (float)atoi(attribute->Value());
					} else if (strcmp(attribute->Name(), "r") == 0) {
						right = (float)atoi(attribute->Value());
					} else if (strcmp(attribute->Name(), "b") == 0) {
						bottom = (float)atoi(attribute->Value());
					}
				}
				m_rect = RFRect(left, top, right, bottom);
				m_type = 1;
			} else if (strcmp(type, "TEXT") == 0) {
				// The text and its font are read but kept nowhere.
				std::wstring text = Utf8ToWString(asset->Attribute("text"));
				std::wstring faceName;
				int size;
				int style;
				int color;
				if (rf::TiXmlElement const* element = asset->FirstChildElement("facename")) {
					faceName = Utf8ToWString(element->Attribute("val"));
				}
				if (rf::TiXmlElement const* element = asset->FirstChildElement("size")) {
					element->Attribute("val", &size);
				}
				if (rf::TiXmlElement const* element = asset->FirstChildElement("style")) {
					element->Attribute("val", &style);
				}
				if (rf::TiXmlElement const* element = asset->FirstChildElement("color")) {
					element->Attribute("val", &color);
				}
				m_type = 2;
			}
		}
		asset = asset->NextSiblingElement("asset");
	}

	rf::TiXmlElement const* anim = node->FirstChildElement("anim");
	while (anim != 0) {
		std::wstring name = Utf8ToWString(anim->Attribute("name"));
		if (m_animName == name) {
			rf::TiXmlElement const* frameNode = anim->FirstChildElement("frame");
			if (frameNode != 0) {
				RFAnimFrame* animFrame = new RFAnimFrame;
				RFSize size = m_animation->m_size;
				animFrame->SetCanvas(RFRect(0.0f, 0.0f, size.width, size.height));
				while (frameNode != 0) {
					RFAnimationFrame frame;
					for (rf::TiXmlAttribute const* attribute = frameNode->FirstAttribute(); attribute != 0;
					     attribute = attribute->Next()) {
						if (strcmp(attribute->Name(), "cnt") == 0) {
							frame.m_time = atoi(attribute->Value());
						}
					}
					if (m_duration < (float)frame.m_time) {
						m_duration = (float)frame.m_time;
					}
					if (frameNode->FirstChildElement("posx") != 0) {
						GetXmlValue(frameNode, "posx", &frame.m_x);
					}
					if (frameNode->FirstChildElement("posy") != 0) {
						GetXmlValue(frameNode, "posy", &frame.m_y);
					}
					if (frameNode->FirstChildElement("centerx") != 0) {
						GetXmlValue(frameNode, "centerx", &frame.m_centerX);
					}
					if (frameNode->FirstChildElement("centery") != 0) {
						GetXmlValue(frameNode, "centery", &frame.m_centerY);
					}
					if (frameNode->FirstChildElement("scalex") != 0) {
						GetXmlValue(frameNode, "scalex", &frame.m_scaleX);
					}
					if (frameNode->FirstChildElement("scaley") != 0) {
						GetXmlValue(frameNode, "scaley", &frame.m_scaleY);
					}
					if (frameNode->FirstChildElement("rotate") != 0) {
						GetXmlValue(frameNode, "rotate", &frame.m_rotation);
					}
					if (frameNode->FirstChildElement("alpha") != 0) {
						GetXmlValue(frameNode, "alpha", &frame.m_alpha);
					}
					if (rf::TiXmlElement const* element = frameNode->FirstChildElement("sound")) {
						frame.m_sfx = Utf8ToWString(element->Attribute("val"));
					}
					if (rf::TiXmlElement const* element = frameNode->FirstChildElement("visible")) {
						frame.m_visible = atoi(element->Attribute("val")) != 0;
					}
					animFrame->AddFrame(frame);
					frameNode = frameNode->NextSiblingElement("frame");
				}
				m_animFrame = animFrame;
			}
		}
		anim = anim->NextSiblingElement("anim");
	}
}

// 0x42c7d0
void RFAnimationObject::Render(float alpha) {
	AnimDrawEventArgs args(this, alpha);
	SendEvent(EventRender, args);
	if (!args.handled && (m_type == 0 || m_type == 2)) {
		m_animFrame->Render(m_texture, alpha);
	}
}

// 0x42c890
void RFAnimationObject::GetXmlValue(rf::TiXmlNode const* node, std::string const& name,
                                    RFFloatAnimationFrame* frame) {
	frame->m_value = (float)atof(node->FirstChildElement(name.c_str())->Attribute("val"));
	std::string easeValue;
	if (node->FirstChildElement(name.c_str())->Attribute("easeValue") != 0) {
		easeValue = node->FirstChildElement(name.c_str())->Attribute("easeValue");
		if (easeValue.size() != 0) {
			std::vector<char> buffer(easeValue.size() + 1);
			memcpy(&buffer[0], easeValue.c_str(), easeValue.size());
			char const delimiter[] = ",";
			char* context;
			float x1 = (float)atof(strtok_s(&buffer[0], delimiter, &context));
			float y1 = (float)atof(strtok_s(0, delimiter, &context));
			float x2 = (float)atof(strtok_s(0, delimiter, &context));
			float y2 = (float)atof(strtok_s(0, delimiter, &context));
			frame->m_interpolation =
			    new RFBezierSpline(0x400, RFVector2(x1, y1), RFVector2(x2, y2));
		}
	} else {
		frame->m_interpolation = new RFInterpolation;
	}
}
