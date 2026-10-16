// Original: d:\build\custom\temp\code\client\projectg\standinginline.h
#ifndef PG_CLIENT_PROJECTG_STANDINGINLINE_H
#define PG_CLIENT_PROJECTG_STANDINGINLINE_H

#include "defines.h"
#include "padding.h"
#include "frform.h"

// Names of the standing-in-line list's numbers (standinginline.h:24-43),
// indexed by the number: 0 has none, then the ten characters, "club",
// "item" and "other". Static: every includer fills its own copy. The null
// first entry is static data and the first name the value of a const
// variable, so the rest is a dynamic initializer as in the original
// (docs/tough_problems/010, 018).
static const char* const STANDING_FIRST_NAME = "\xb4\xa9\xb8\xae";   // Nuri

static const char* s_standingNames[14] = {
	0,
	STANDING_FIRST_NAME,   // Nuri
	"\xc7\xcf\xb3\xaa",   // Hana
	"\xbe\xc6\xc0\xfa",   // Azer
	"\xbc\xbc\xbd\xc7\xb8\xae\xbe\xc6",   // Cecilia
	"\xb8\xc6\xbd\xba",   // Max
	"\xc4\xed",   // Kooh
	"\xbe\xc6\xb8\xb0",   // Arin
	"\xc4\xab\xc1\xee",   // Kaz
	"\xb7\xe7\xbd\xc3\xbe\xc6",   // Lucia
	"\xb3\xda",   // Nell
	"\xc5\xac\xb7\xb4",   // club
	"\xbe\xc6\xc0\xcc\xc5\xdb",   // item
	"\xb1\xe2\xc5\xb8"   // other
};

class FrListBox;
class FrViewer;

// Number picker form: a "total" viewer and a list of the numbers 1-13; the
// chosen number is read back with GetSelectNum (0 = "total"). Its size
// (0x120) is the `new` of CStandingInLineMakeInstance.
class CStandingInLine : public FrForm {
	LAYOUT_CHECK_FRIEND;

public:
	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }   // standinginline.h:53

	CStandingInLine(void);
	virtual ~CStandingInLine(void);

	int GetSelectNum(void) { return m_selectNum; }   // standinginline.h:82

private:
	static sFRESH_ENTRY _MsgEntries[];

protected:
	static sFRESH_MSGMAP _MsgMap;
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;

	void OnView_TotalInit(intptr_t param);
	void OnView_TotalOwnerDraw(PG_MSGARG param);
	void OnView_TotalButtonUp(void);
	void OnArrayListInit(intptr_t param);
	void OnArrayListOwnerDraw(intptr_t param);
	void OnArrayListBtnDown(void);
	void OnArrayListRBtnDown(void);

	FrViewer* m_pViewTotal;    // 0x110, the "total" viewer
	FrListBox* m_pArrayList;   // 0x114
	int m_selectNum;           // 0x118, chosen number (0 = total)
	PADDING(0x11c, 0x120);     // CStandingInLine members
};

CHECK_SIZE(CStandingInLine, 0x120);
BEGIN_LAYOUT_CHECK(CStandingInLine)
	CHECK_FIELD(CStandingInLine, m_pViewTotal, 0x110);
	CHECK_FIELD(CStandingInLine, m_pArrayList, 0x114);
	CHECK_FIELD(CStandingInLine, m_selectNum, 0x118);
END_LAYOUT_CHECK

#endif // PG_CLIENT_PROJECTG_STANDINGINLINE_H
