// Original: d:\build\custom\temp\code\client\projectg\workalivecheck.h
#ifndef PG_CLIENT_PROJECTG_WORKALIVECHECK_H
#define PG_CLIENT_PROJECTG_WORKALIVECHECK_H

#include <windows.h>

#include "defines.h"
#include "eventthread.h"
#include "Packet.h"
#include "work.h"

// Sends the alive-check packet 0xF6 with a running counter every m_interval
// milliseconds, from the CEventThread it registers with.
class CWorkAliveCheck : public CWork {
	LAYOUT_CHECK_FRIEND;
public:
	// workalivecheck.h:15
	CWorkAliveCheck(unsigned long interval)
	  : m_lastTick(GetTickCount()), m_interval(interval), m_counter(0) {
		CEventThread::Instance()->RegisterWork(this);
	}

	// workalivecheck.h:20
	virtual ~CWorkAliveCheck(void) {
		CEventThread::Instance()->UnregisterWork(this);
	}

	// workalivecheck.h:25
	virtual void Act(void) {
		if (m_interval < GetTickCount() - m_lastTick) {
			WSendPacket packet((enumClientPacket)0xF6);
			packet.Encode4(m_counter++);
			packet.Send(SEND_TO_DEFAULT);

			m_lastTick = GetTickCount();
		}
	}

private:
	unsigned long m_lastTick; // 0x4, GetTickCount of the last send
	unsigned long m_interval; // 0x8, milliseconds between sends
	unsigned long m_counter;  // 0xC, sent with each packet
};
CHECK_SIZE(CWorkAliveCheck, 0x10);
BEGIN_LAYOUT_CHECK(CWorkAliveCheck)
	CHECK_FIELD(CWorkAliveCheck, m_lastTick, 0x4);
	CHECK_FIELD(CWorkAliveCheck, m_interval, 0x8);
	CHECK_FIELD(CWorkAliveCheck, m_counter, 0xC);
END_LAYOUT_CHECK

#endif // PG_CLIENT_PROJECTG_WORKALIVECHECK_H
