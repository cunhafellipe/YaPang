// Original: d:\build\custom\temp\code\client\projectg\fxbox.h
#ifndef PG_CLIENT_PROJECTG_FXBOX_H
#define PG_CLIENT_PROJECTG_FXBOX_H

#include <list>
#include <map>
#include <string>

#include "defines.h"
#include "padding.h"
#include "fxsequence.h"
#include "wvideo.h"

class CFxSequence;
class CFxSpray;
class CFxKeeper;
class W3dAniSpr;
class WMatrix;
class WResourceManager;
class WVector;
class WView;

struct sFxKeeper;

// The effect container: loads, runs and draws effect sequences and sprays.
// Layout from the constructor (fxbox.cpp:32); the members it initialises
// whose use is not decompiled yet stay UNKNOWN. Virtual slots in the order of
// ??_7CFxBox@@6B@ (0xA117A4).
class CFxBox {
	LAYOUT_CHECK_FRIEND;
	friend class CFxSequence;
	friend class CFx;
	friend class CSky;
	friend class CFxSpray;   // ParseLine calls ParseNumber and LoadSprite
	friend class CBase;   // AttachFx; the ctor sets m_frameTime
	friend class CRealMyRoom_Avatar;   // the ctor sets m_frameTime
	friend class ntAvatar;   // the ctor sets m_frameTime
public:
	CFxBox(void);
	virtual ~CFxBox(void);
	virtual void ProcessAttacher(float fDelta) {
	}

	void Init(WResourceManager* pResourceManager);
	void SetCamera(const WMatrix* pCamera);
	void ClearAll(bool bKeepActive);
	void ClearSpray(bool bKeepActive);
	void ClearSequence(bool bKeepActive);
	void ClearSprite(void);
	void ClearOriginalSpray(void);
	void ClearOriginalSequence(void);
	bool Finished(void);
	void AddSpray(CFxSpray* pSpray);
	bool PreprocessDefines(defines_t& defines, const char* line);
	void ProcessDefines(defines_t& defines, char* token);
	void ClearDefines(defines_t& defines, defines_t* pParent);
	void Clear(void);
	void Display(WView* pView);
	bool Process(float fDelta);
	const CFxSequence::sLine& GetLastError(void);
	CFxSequence* LoadSequence(const char* name, bool bOriginal, defines_t* pDefines);
	void AddSequence(CFxSequence* pSequence);
	CFxSpray* LoadSpray(const char* name, defines_t* pDefines);
	void DeleteSequence(CFxSequence* pSequence, bool bImmediate);
	void DeleteSpray(CFxSpray* pSpray);
	void CallFxKeeperCallBack(void* pParam);
	void AddFxKeeper(sFxKeeper& keeper);
	void EraseFxKeeper(CFxKeeper* pKeeper);
	void ClearFxKeeper(void);
	void SetScreenCenter(float x, float y);
	// fxbox.h:83
	void SetGroundHeightArea(float high, float low) { m_groundAreaHigh = high; m_groundAreaLow = low; }
	std::map<CFxSequence*, CFxSequence*>& GetSequenceList(void);   // fxbox.inl:9
	std::map<CFxSpray*, CFxSpray*>& GetSprayList(void);            // fxbox.inl:15
	CFxSpray* GetSpray(int index);                                 // fxbox.inl:23
	bool FindSequence(CFxSequence* pSequence);                     // fxbox.inl:39
	bool FindSpray(CFxSpray* pSpray);                              // fxbox.inl:45
	bool GetOutput(WVector& pos, unsigned char& alpha);            // fxbox.inl:53

protected:
	float ParseNumber(const char* str);
	W3dAniSpr* LoadSprite(const char* name, int width, int height);
	float Random(float min, float max);                                             // fxbox.inl:70
	WVector Random(const WVector& min, const WVector& max);                         // fxbox.inl:77
	WVector Random(const WVector& center, float minDist, float maxDist, float angle);   // fxbox.inl:84

	virtual bool CheckGroundCollision(const WVector& from, const WVector& to, float& height, float radius);
	virtual void OnSequenceCreated(CFxSequence* pSequence);
	virtual void OnSequenceDeleted(CFxSequence* pSequence);
	virtual void OnSprayCreated(CFxSpray* pSpray);
	virtual void OnSprayDeleted(CFxSpray* pSpray);

private:
	void DeleteSpray(std::map<CFxSpray*, CFxSpray*>::iterator& it);
	void DeleteSequence(std::map<CFxSequence*, CFxSequence*>::iterator& it, bool bImmediate);

protected:
	float m_frameTime;                         // 0x4, 33.333f: the "f" suffix of ParseNumber
	float m_timeScale;                         // 0x8, 1.0f
	bool m_bPaused;                            // 0xC
	UNKNOWN(0xD, 0x110);
	bool m_flag110;                            // 0x110
	int m_value114;                            // 0x114
	bool m_flag118;                            // 0x118, true
	int m_displayedCount;                      // 0x11C, Display sums CFxSpray +0x1130
	int m_displayedCount2;                     // 0x120, Display sums CFxSpray +0x1134
	bool m_flag124;                            // 0x124
	PADDING(0x125, 0x128);
	std::map<std::string, CFxSpray*> m_originalSprays;        // 0x128
	std::map<std::string, CFxSequence*> m_originalSequences;  // 0x134
	WResourceManager* m_pResourceManager;      // 0x140, Init
	std::map<std::string, W3dAniSpr*> m_sprites;               // 0x144
	std::map<CFxSpray*, CFxSpray*> m_sprays;                   // 0x150, GetSprayList
	std::map<CFxSequence*, CFxSequence*> m_sequences;          // 0x15C, GetSequenceList
	const WMatrix* m_pCamera;                  // 0x168, SetCamera
	WTVertex* m_pCorners[5];                   // 0x16C, the entries of m_corners, NULL-terminated ([4] at 0x17C)
	WTVertex m_corners[4];                     // 0x180, CFxSpray::DrawBandana's quad, drawn as a polygon fan
	CFxSequence::sLine m_lastError;            // 0x220, GetLastError
	UNKNOWN(0x334, 0x338);
	std::list<sFxKeeper> m_fxKeepers;          // 0x338
	float m_groundAreaHigh;                    // 0x344, 99.0f, SetGroundHeightArea
	float m_groundAreaLow;                     // 0x348, -99.0f
};
CHECK_SIZE(CFxBox, 0x34C);
BEGIN_LAYOUT_CHECK(CFxBox)
	CHECK_FIELD(CFxBox, m_frameTime, 0x4);
	CHECK_FIELD(CFxBox, m_bPaused, 0xC);
	CHECK_FIELD(CFxBox, m_flag110, 0x110);
	CHECK_FIELD(CFxBox, m_displayedCount, 0x11C);
	CHECK_FIELD(CFxBox, m_flag124, 0x124);
	CHECK_FIELD(CFxBox, m_pCorners, 0x16C);
	CHECK_FIELD(CFxBox, m_corners, 0x180);
	CHECK_FIELD(CFxBox, m_groundAreaHigh, 0x344);
	CHECK_FIELD(CFxBox, m_groundAreaLow, 0x348);
	CHECK_FIELD(CFxBox, m_originalSprays, 0x128);
	CHECK_FIELD(CFxBox, m_originalSequences, 0x134);
	CHECK_FIELD(CFxBox, m_sprites, 0x144);
	CHECK_FIELD(CFxBox, m_sprays, 0x150);
	CHECK_FIELD(CFxBox, m_pResourceManager, 0x140);
	CHECK_FIELD(CFxBox, m_sequences, 0x15C);
	CHECK_FIELD(CFxBox, m_pCamera, 0x168);
	CHECK_FIELD(CFxBox, m_lastError, 0x220);
	CHECK_FIELD(CFxBox, m_fxKeepers, 0x338);
END_LAYOUT_CHECK

// An object registered with CFxBox::AddFxKeeper: the box remembers itself in
// it and the keeper unregisters on destruction. CFxBox calls EmptyFx (its
// vtable 0x9ED7CC slot 1, pure here) through sFxKeeper::m_pfnCallBack.
class CFxKeeper {
	LAYOUT_CHECK_FRIEND;
public:
	// fxbox.h:156
	CFxKeeper(void) : m_pFxManager(0) {
	}
	// fxbox.h:158
	virtual ~CFxKeeper(void) {
		if (m_pFxManager) {
			m_pFxManager->EraseFxKeeper(this);
		}
	}
	// fxbox.h (pure: no line record; vtable 0x9ED7CC slot 1 = _purecall)
	virtual void EmptyFx(void* pFx) = 0;

	// fxbox.h:164
	void SetFxManager(CFxBox* pFxManager) {
		m_pFxManager = pFxManager;
	}

protected:
	CFxBox* m_pFxManager;                      // 0x4
};
CHECK_SIZE(CFxKeeper, 0x8);
BEGIN_LAYOUT_CHECK(CFxKeeper)
	CHECK_FIELD(CFxKeeper, m_pFxManager, 0x4);
END_LAYOUT_CHECK

// A keeper and the member CFxBox::CallFxKeeperCallBack calls on it. Defined
// after CFxKeeper so the member pointer has the single-inheritance size.
struct sFxKeeper {
	CFxKeeper* m_pKeeper;                      // 0x0, told its CFxBox by AddFxKeeper
	void (CFxKeeper::*m_pfnCallBack)(void* pParam);   // 0x4
};
CHECK_SIZE(sFxKeeper, 0x8);
CHECK_FIELD(sFxKeeper, m_pfnCallBack, 0x4);

#include "FxBox.inl"

#endif // PG_CLIENT_PROJECTG_FXBOX_H
