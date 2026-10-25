// Original: client\Refresh\include\RFDirector.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFDIRECTOR_H
#define PG_CLIENT_REFRESH_INCLUDE_RFDIRECTOR_H

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "defines.h"
#include "padding.h"
#include "RFEventHandler.h"
#include "RFExport.h"

class RFAnimation;
class RFInterface;
class RFRenderer;
class RFVector2;
class RFWindowModifier;
class RFWnd;

// The animation director: vtable 0xd62fac, 2 slots, size 0x60 (from its
// scalar deleting destructor 0x4043d0). dllexport as in the original library:
// cl 16 then emits the header inlines and the implicit copy members.
class RF_API RFDirector: public RFEventHandler {
	LAYOUT_CHECK_FRIEND;

public:
	RFDirector();                  // 0x4307f0
	// [0] RFEventHandler::ToString 0x401a90
	virtual ~RFDirector();         // [1] scalar deleting destructor 0x4043d0, ~RFDirector 0x430910

	// The event raised when a scene ends (0xe46eb0).
	static std::wstring const EventAnimFinished;

	uint32_t GetVersion() const;   // 0x404200
	RFRenderer* GetRenderer();     // 0x404210
	RFInterface* GetInterface();   // 0x404220
	void SendFinishEvent(RFAnimation*);  // 0x430a10
	void UnloadAllAnimation();           // 0x430ad0
	R_ERRORTYPE LoadAnimation(std::wstring const&);          // 0x430b50
	R_ERRORTYPE LoadAnimationInDirect(std::wstring const&);  // 0x430c20
	void SetPosition(RFVector2 const&);  // 0x430fa0
	int GetAnimationCount() const;       // 0x431140
	RFAnimation* GetAnimation(int) const;                  // 0x431150
	RFAnimation* GetAnimation(std::wstring const&) const;  // 0x431190
	bool IsSceneExist(std::wstring const&) const;          // 0x431200
	RFAnimation* FindScene(std::wstring const&);           // 0x431220
	RFAnimation const* FindScene(std::wstring const&) const;  // 0x431230
	RFAnimation* GetPlayingScene();               // 0x431240
	R_ERRORTYPE Play(std::wstring const&);        // 0x431250
	R_ERRORTYPE Play(RFWnd*, std::wstring const&);  // 0x431290
	R_ERRORTYPE Play(int);                        // 0x4312c0
	void Stop();                                  // 0x431310
	bool IsPlaying(std::wstring const&) const;    // 0x431320
	bool IsPlaying() const;                       // 0x431340
	void End(std::wstring const&);                // 0x431350
	void End();                                   // 0x431370
	void Loop(std::wstring const&, bool);         // 0x431380
	void Loop(int, bool);                         // 0x4313a0
	bool IsLoop(std::wstring const&) const;       // 0x4313d0
	bool IsLoop() const;                          // 0x431410
	void FrameMove(float);                        // 0x431420
	// Plays the scene onto wnd: each object drives the window it names (wnd
	// or wnd's child) through an RFWindowModifier from m_modifiers.
	void FrameMove(RFWnd*, float);                // 0x431440
	void Render(float);                           // 0x431a50

private:
	R_ERRORTYPE LoadAnimation_(std::wstring const&);  // 0x430c30

	std::vector<RFAnimation*> m_scenes;  // 0x30 (GetAnimationCount 0x431140)
	uint32_t m_version;   // 0x40
	RFRenderer* m_renderer;    // 0x44
	RFInterface* m_interface;  // 0x48 (GetInterface 0x404220)
	RFAnimation* m_playingScene;  // 0x4c (GetPlayingScene 0x431240)
	// The modifier FrameMove(RFWnd*, float) 0x431440 applies to each window
	// (found by the window, created on first use); the destructor deletes them.
	// 0x50 (VS 2010 map: comparator, head 0x54, size 0x58, allocators 0x5c)
	std::map<RFWnd*, RFWindowModifier*> m_modifiers;
};
CHECK_SIZE(RFDirector, 0x60);
BEGIN_LAYOUT_CHECK(RFDirector)
	CHECK_FIELD(RFDirector, m_scenes, 0x30);
	CHECK_FIELD(RFDirector, m_version, 0x40);
	CHECK_FIELD(RFDirector, m_renderer, 0x44);
	CHECK_FIELD(RFDirector, m_interface, 0x48);
	CHECK_FIELD(RFDirector, m_playingScene, 0x4c);
	CHECK_FIELD(RFDirector, m_modifiers, 0x50);
END_LAYOUT_CHECK

#endif
