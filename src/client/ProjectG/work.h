// Original: d:\build\custom\temp\code\client\projectg\work.h
#ifndef PG_CLIENT_PROJECTG_WORK_H
#define PG_CLIENT_PROJECTG_WORK_H

// A job run by CEventThread (RegisterWork, UnregisterWork). Only the
// members this project's code uses so far are declared.
class CWork {
public:
	// work.h:7
	virtual ~CWork(void) {}

	// Slot 1: run once per CEventThread tick (CWorkAliveCheck::Act).
	virtual void Act(void) = 0;
};

#endif // PG_CLIENT_PROJECTG_WORK_H
