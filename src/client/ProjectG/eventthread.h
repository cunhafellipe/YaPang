// Original: d:\build\custom\temp\code\client\projectg\eventthread.h
#ifndef PG_CLIENT_PROJECTG_EVENTTHREAD_H
#define PG_CLIENT_PROJECTG_EVENTTHREAD_H

#include <algorithm>
#include <list>

#include "criticalsection.h"
#include "defines.h"
#include "singleton.h"
#include "thread.h"
#include "work.h"

// Runs the registered works every 33 ms on its own thread until Stop sets
// the event.
class CEventThread : public CThread, public WSingleton<CEventThread> {
	LAYOUT_CHECK_FRIEND;
public:
	// eventthread.h:13
	CEventThread(void) : m_hEvent(CreateEventA(NULL, FALSE, FALSE, NULL)) {
		Start();
	}

	// eventthread.h:18
	virtual ~CEventThread(void) {
		Stop();
	}

	// eventthread.h:23
	virtual void Stop(void) {
		SetEvent(m_hEvent);
		CThread::Stop();
	}

	// eventthread.h:29
	void RegisterWork(CWork* work) {
		m_cs.Enter();
		m_works.push_back(work);
		m_cs.Leave();
	}

	// eventthread.h:36
	void UnregisterWork(CWork* work) {
		m_cs.Enter();
		std::list<CWork*>::iterator it = std::find(m_works.begin(), m_works.end(), work);
		if (it != m_works.end()) {
			m_works.erase(it);
		}
		m_cs.Leave();
	}

protected:
	// eventthread.h:50
	virtual void Act(void) {
		while (WaitForSingleObject(m_hEvent, 0) != WAIT_OBJECT_0) {
			Sleep(33);
			m_cs.Enter();
			for (std::list<CWork*>::iterator it = m_works.begin(); it != m_works.end(); ++it) {
				CWork* work = *it;
				work->Act();
			}
			m_cs.Leave();
		}
	}

private:
	HANDLE m_hEvent;          // 0x30, set by Stop
	std::list<CWork*> m_works; // 0x34
	CCriticalSection m_cs;    // 0x40, guards m_works
};
CHECK_SIZE(CEventThread, 0x58);
BEGIN_LAYOUT_CHECK(CEventThread)
	CHECK_FIELD(CEventThread, m_hEvent, 0x30);
	CHECK_FIELD(CEventThread, m_works, 0x34);
	CHECK_FIELD(CEventThread, m_cs, 0x40);
END_LAYOUT_CHECK

#endif // PG_CLIENT_PROJECTG_EVENTTHREAD_H
