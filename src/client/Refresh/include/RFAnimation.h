// Original: client\Refresh\include\RFAnimation.h (JP 983 exe; the class is
// not exported, its code (0x45b...-0x45d...) sits after the object of
// src\RFAnimation.cpp's asserts; header path from the library's layout,
// docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFANIMATION_H
#define PG_CLIENT_REFRESH_INCLUDE_RFANIMATION_H

#include <stdint.h>
#include <string>
#include <vector>

#include "defines.h"
#include "padding.h"
#include "RFAnimationObject.h"
#include "RFEventHandler.h"
#include "RFSize.h"

class RFDirector;
class RFRect;
class RFVector2;

// One scene of an RFDirector: vtable 0xd65f90, 2 slots, size 0x8C (the
// operator new of RFDirector::LoadAnimation_ 0x430ee9). Not exported, so its
// members pair as sub_<va>. The library was built with /GL + /LTCG: RFDirector
// calls End, FrameMove and Render with this on the stack or in eax.
class RFAnimation : public RFEventHandler {
	LAYOUT_CHECK_FRIEND;
	friend class RFDirector;
	// RFAnimationObject::Load reads m_size for its canvas.
	friend class RFAnimationObject;

public:
	// [0] RFEventHandler::ToString 0x401a90
	// [1] scalar deleting destructor 0x45baf0 (RFDirector::UnloadAllAnimation
	// deletes through it)
	virtual ~RFAnimation();   // 0x45bb20

	RFAnimation(RFDirector*);  // 0x45ba40 (RFDirector::LoadAnimation_)
	// Moves every object's canvas to pos (size m_size) and
	// regenerates their matrices (the position in a stack argument, this
	// second: LTCG).
	void SetPosition(RFVector2 const&);  // 0x45bbf0
	unsigned int GetObjectCount() const;  // 0x45bd30
	// The object at index (this in esi, index in edi: LTCG register args).
	RFAnimationObject* GetObjectAt(unsigned int);  // 0x45bd50
	RFAnimationObject* FindObject(std::wstring const&);  // 0x45bda0, by its name
	RFAnimationObject const* GetObjectAt(unsigned int) const;  // 0x45be30
	RFAnimationObject const* FindObject(std::wstring const&) const;  // 0x45be80
	// Sets the rect of the object named name; asserts and returns R_FAIL when
	// there is none (the original returns a full eax 0/1, not a bool in al).
	R_ERRORTYPE SetObjectRect(std::wstring const&, RFRect const&);  // 0x45bf10
	void Pause();             // 0x45bd20
	float GetProgress() const;  // 0x45bf60
	// The <param> children of a scene: RES (x, y: the size), LOOP (cnt -1
	// loops, pendulum). The node arrives in eax (LTCG register argument).
	void LoadParam(rf::TiXmlNode const*);  // 0x45bf70
	// The <scene> loader (ecx = the element, edx = this: LTCG register args):
	// the scene's attributes, its <param>s, then one RFAnimationObject per
	// <object> (name, "ani", the <asset> "link"), each then loaded from the
	// scene node; m_endTime grows to the longest object. Returns true.
	bool Load(rf::TiXmlNode const*);  // 0x45c250
	void Play();              // 0x45ca80
	bool IsPlaying() const;   // 0x45cb20
	void TogglePause();       // 0x45cb30
	void Stop();              // 0x45cb40
	void End();               // 0x45cca0
	void SetLoop(bool);       // 0x45cd80
	bool IsLoop() const;      // 0x45cd90
	void FrameMove(float);    // 0x45cda0
	void Render(float);       // 0x45cfb0

private:
	// Regenerates every object's animation matrix from its rect (no caller
	// left: LTCG inlined it into SetPosition).
	void GenAnimMatrix();     // 0x45cf40

	std::wstring m_name;      // 0x30 (RFDirector::GetAnimation 0x431190 compares it)
	uint32_t m_version;       // 0x4c, the director's GetVersion at construction
	RFDirector* m_director;   // 0x50, the owner (FrameMove sends it the finish event)
	float m_time;             // 0x54, seconds into the scene
	unsigned int m_finishedCount;  // 0x58, objects no longer playing (FrameMove)
	bool m_playing;           // 0x5c (RFDirector::IsPlaying 0x431340)
	bool m_loop;              // 0x5d (RFDirector::IsLoop 0x431410)
	bool m_pendulum;          // 0x5e, FrameMove flips m_reverse at each loop
	int m_reverse;            // 0x60, Play uses PlayPendulum, FrameMove plays backwards
	PADDING(0x64, 0x68);
	// The <param type="RES" x y> resolution (RFDirector::SetPosition 0x430fa0;
	// FrameMove 0x431440 copies it whole, two dword moves).
	RFSize m_size;            // 0x68
	float m_startTime;        // 0x70, milliseconds
	float m_endTime;          // 0x74, milliseconds
	float m_progress;         // 0x78, 0..1 (GetProgress)
	std::vector<RFAnimationObject> m_objects;  // 0x7c (RFDirector::SetPosition 0x430fa0)
};
CHECK_SIZE(RFAnimation, 0x8c);
BEGIN_LAYOUT_CHECK(RFAnimation)
	CHECK_FIELD(RFAnimation, m_name, 0x30);
	CHECK_FIELD(RFAnimation, m_version, 0x4c);
	CHECK_FIELD(RFAnimation, m_director, 0x50);
	CHECK_FIELD(RFAnimation, m_time, 0x54);
	CHECK_FIELD(RFAnimation, m_finishedCount, 0x58);
	CHECK_FIELD(RFAnimation, m_playing, 0x5c);
	CHECK_FIELD(RFAnimation, m_loop, 0x5d);
	CHECK_FIELD(RFAnimation, m_pendulum, 0x5e);
	CHECK_FIELD(RFAnimation, m_reverse, 0x60);
	CHECK_FIELD(RFAnimation, m_size, 0x68);
	CHECK_FIELD(RFAnimation, m_startTime, 0x70);
	CHECK_FIELD(RFAnimation, m_endTime, 0x74);
	CHECK_FIELD(RFAnimation, m_progress, 0x78);
	CHECK_FIELD(RFAnimation, m_objects, 0x7c);
END_LAYOUT_CHECK

#endif
