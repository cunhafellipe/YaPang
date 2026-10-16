// Original: d:\build\custom\temp\code\client\projectg\panggiftdlg.h
#ifndef PG_CLIENT_PROJECTG_PANGGIFTDLG_H
#define PG_CLIENT_PROJECTG_PANGGIFTDLG_H

#include "defines.h"
#include "padding.h"
#include "frform.h"

class FrArea;
class FrEdit;

// A received gift (original header not identified yet; only the members
// the code uses are named). CSharedDoc keeps them in a std::list.
struct sGiftInfo {
	unsigned long m_serial;          // 0x0, the gift's serial (sent with the move-gift request)
	unsigned long m_typeId;          // 0x4, the gift item's type id
	unsigned short m_pang;           // 0x8, item count (pang amount for a pang gift)
	char m_sender[0x80];             // 0xA, sender's name
	char m_message[0x50];            // 0x8A, sender's message
	char m_date[0x20];               // 0xDA, when it was sent, "date time" text
	unsigned char m_timeUnit;        // 0xFA, period unit of a time-limited gift (3 hours, 4 days, 5 months)
};
CHECK_FIELD(sGiftInfo, m_serial, 0x0);
CHECK_FIELD(sGiftInfo, m_typeId, 0x4);
CHECK_FIELD(sGiftInfo, m_pang, 0x8);
CHECK_FIELD(sGiftInfo, m_sender, 0xA);
CHECK_FIELD(sGiftInfo, m_message, 0x8A);
CHECK_FIELD(sGiftInfo, m_date, 0xDA);
CHECK_FIELD(sGiftInfo, m_timeUnit, 0xFA);

// Pang gift dialog; its size (0x128) is the `new` of
// FrPangGiftDlgMakeInstance.
class FrPangGiftDlg : public FrForm {
	LAYOUT_CHECK_FRIEND;

public:
	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }   // panggiftdlg.h:12

	FrPangGiftDlg(void);

	sGiftInfo* GetItemInfo(void);    // panggiftdlg.h:17 (defined in RealMyRoomMainUI.cpp)
	void SetItemInfo(sGiftInfo* pInfo);

private:
	static sFRESH_ENTRY _MsgEntries[];

protected:
	static sFRESH_MSGMAP _MsgMap;
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;

	void OnMyPangInit(intptr_t param);
	void OnSendPangInit(intptr_t param);
	void OnTotalPangInit(intptr_t param);
	void OnItemInit(intptr_t param);
	void OnItemOwnerDraw(PG_MSGARG param);

private:
	FrArea* m_pItemArea;             // 0x110 (OnItemInit)
	FrEdit* m_pMyPangEdit;           // 0x114 (OnMyPangInit)
	FrEdit* m_pSendPangEdit;         // 0x118 (OnSendPangInit)
	FrEdit* m_pTotalPangEdit;        // 0x11c (OnTotalPangInit)
	sGiftInfo* m_itemInfo;           // 0x120
	UNKNOWN(0x124, 0x128);
};
BEGIN_LAYOUT_CHECK(FrPangGiftDlg)
	CHECK_FIELD(FrPangGiftDlg, m_pItemArea, 0x110);
	CHECK_FIELD(FrPangGiftDlg, m_pMyPangEdit, 0x114);
	CHECK_FIELD(FrPangGiftDlg, m_pSendPangEdit, 0x118);
	CHECK_FIELD(FrPangGiftDlg, m_pTotalPangEdit, 0x11c);
	CHECK_FIELD(FrPangGiftDlg, m_itemInfo, 0x120);
END_LAYOUT_CHECK
CHECK_SIZE(FrPangGiftDlg, 0x128);

#endif // PG_CLIENT_PROJECTG_PANGGIFTDLG_H
