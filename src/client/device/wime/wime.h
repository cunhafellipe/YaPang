// Original: d:\build\custom\temp\code\client\device\wime\wime.h
#ifndef PG_CLIENT_DEVICE_WIME_WIME_H
#define PG_CLIENT_DEVICE_WIME_WIME_H

#include <windows.h>
#include <imm.h>

#include "defines.h"
#include "winput.h"
#include "wproc.h"

// The IME input device of the Wangreal DLL (PDB layout, size 0x550).
class WIme : public WInputDev, public WProc {
	LAYOUT_CHECK_FRIEND;
public:
	WIme(HWND__ *hWnd);
	WIme(void);
	virtual ~WIme(void);

	virtual int GetState(int state, int arg);
	virtual void Update(unsigned long time);
	virtual void Reset(void);
	virtual WInputDev *MakeClone(char *name, HWND__ *hWnd);
	// wime.h:49
	virtual char *GetDeviceName(void) { return "Ime"; }
	// wime.h:51
	virtual WProc *ExternProc(void) { return this; }
	virtual int WinProc(unsigned int msg, unsigned long wParam, unsigned long lParam);
	char *GetTextComp(void);
	// wime.h:61
	virtual void SetActive(bool stat) { m_bActive = stat; pos = 0; current = 0; }
	virtual void SetAlphaNumericMode(bool on);
	virtual bool IsAlphaNumericMode(void);

private:
	void InitIme(HWND__ *hWnd);
	int IME_Enter(void);
	void Leave(void);
	int Check(void);
	bool IsProcessEvent(void);
	void SetState(bool state);
	void ClearData(void);
	void PutString(char *str);

protected:
	void OnInputLangChange(unsigned int wParam, long lParam);
	void OnSetContext(unsigned int wParam, long lParam);
	void OnStartComposition(unsigned int wParam, long lParam);
	void OnComposition(unsigned int wParam, long lParam);
	void OnEndComposition(unsigned int wParam, long lParam);
	void OnCompositionFull(unsigned int dwCommand, long dwData);
	int OnNotify(unsigned int dwCommand, long dwData);
	void OnControl(unsigned int dwCommand, long dwData);
	void OnChar(unsigned int ch);
	int GetResultString(void);
	int GetCompString(long flag);
	virtual void ProcessResultString(char *str);              // vtable 0x4C
	virtual void ProcessCompString(char *comp, char *attr);   // vtable 0x50

	unsigned long m_property;           // 0x14
	HKL__ *m_hKeyLayout;                // 0x18
	int m_nState;                       // 0x1C
	int m_nCompLen;                     // 0x20
	HWND__ *m_hwndCand[32];             // 0x24
	tagCANDIDATELIST *m_candList[32];   // 0xA4
	int m_charWidth;                    // 0x124
	int m_charHeight;                   // 0x128
	bool m_bActive;                     // 0x12C
	HIMC__ *m_hIMC;                     // 0x130
	HWND__ *m_hWnd;                     // 0x134
	char m_TextComp[3];                 // 0x138
	unsigned long m_dwConvMode;         // 0x13C
	unsigned char *buff;                // 0x140
	int current;                        // 0x144
	int old_current;                    // 0x148
	unsigned char ringBuf[1024];        // 0x14C
	int pos;                            // 0x54C
};
BEGIN_LAYOUT_CHECK(WIme)
	CHECK_SIZE(WIme, 0x550);
	CHECK_FIELD(WIme, m_property, 0x14);
	CHECK_FIELD(WIme, m_hKeyLayout, 0x18);
	CHECK_FIELD(WIme, m_nState, 0x1c);
	CHECK_FIELD(WIme, m_nCompLen, 0x20);
	CHECK_FIELD(WIme, m_hwndCand, 0x24);
	CHECK_FIELD(WIme, m_candList, 0xa4);
	CHECK_FIELD(WIme, m_charWidth, 0x124);
	CHECK_FIELD(WIme, m_charHeight, 0x128);
	CHECK_FIELD(WIme, m_bActive, 0x12c);
	CHECK_FIELD(WIme, m_hIMC, 0x130);
	CHECK_FIELD(WIme, m_hWnd, 0x134);
	CHECK_FIELD(WIme, m_TextComp, 0x138);
	CHECK_FIELD(WIme, m_dwConvMode, 0x13c);
	CHECK_FIELD(WIme, buff, 0x140);
	CHECK_FIELD(WIme, current, 0x144);
	CHECK_FIELD(WIme, old_current, 0x148);
	CHECK_FIELD(WIme, ringBuf, 0x14c);
	CHECK_FIELD(WIme, pos, 0x54c);
END_LAYOUT_CHECK

// wime.cpp defines WIME_WIME_INLINE empty before its includes: the original
// emits these out of line in WIme.obj although nothing references them,
// while device.cpp, which includes this header too, keeps them inline.
#ifndef WIME_WIME_INLINE
#define WIME_WIME_INLINE inline
#endif

// wime.h:59
WIME_WIME_INLINE char *WIme::GetTextComp(void) { return m_TextComp; }

// wime.h:75
WIME_WIME_INLINE int WIme::IME_Enter(void) { return m_hWnd ? ((m_hIMC = ImmGetContext(m_hWnd)) != 0) : 0; }
// wime.h:76
WIME_WIME_INLINE void WIme::Leave(void) { ImmReleaseContext(m_hWnd, m_hIMC); }
// wime.h:77
WIME_WIME_INLINE int WIme::Check(void) { return (!(m_property & IME_PROP_SPECIAL_UI) && (m_property & IME_PROP_AT_CARET)) ? 1 : 0; }

#endif // PG_CLIENT_DEVICE_WIME_WIME_H
