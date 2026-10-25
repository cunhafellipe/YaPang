// Original: client\Refresh\src\RFEventHandler.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include "RFEventHandler.h"

// lib: 0x401b80 (RFEventHandler scalar deleting destructor ??_G, unreferenced)
// lib: 0x401bb0 (vector deleting destructor), 0x42fd70, 0x42fe50
// (unordered_map<wstring, RFEvent>::find), 0x42fb60 (erase), 0x40b8e0
// (operator=), 0x40cae0, 0x40e270, 0x40d760, 0x430000, 0x430310; the
// implicit copy constructor 0x401aa0 and operator= 0x401b60
// lib: unordered_map<wstring, RFEvent> internals: 0x42f8d0, 0x42f940
// (~pair<const wstring, RFEvent>), 0x42fb40 (map constructor), 0x42fbd0
// (_Hash clear), 0x42fc30, 0x42fc50 (find), 0x42fc70, 0x430130 (list
// iterator dereference), 0x42ff30 (bucket fix-up on erase), 0x42ff90,
// 0x42ffb0, 0x42ffd0, 0x42ffe0, 0x42fff0 (bucket vector accessors),
// 0x430140, 0x4301d0 (pair move constructor), 0x4303e0, 0x430430
// (allocator construct), 0x42ff70, 0x42ff80, 0x430260, 0x430270, 0x4302c0
// (empty std helpers)

// ToString 0x401a90 is inline in RFEventHandler.h.

// 0x42f5a0
RFEventHandler::RFEventHandler() {
}

// 0x42f600
RFEventHandler::~RFEventHandler() {
	for (EventMap::iterator it = m_events.begin(); it != m_events.end(); ++it) {
		it->second.ClearSubscribers();
	}
	m_events.clear();
}

// 0x42f760
void RFEventHandler::AddEvent(std::wstring const& name) {
	EventMap::iterator it = m_events.find(name);
	if (it == m_events.end()) {
		m_events.insert(EventMap::value_type(name, RFEvent(name)));
	}
}

// 0x42f9b0
void RFEventHandler::RemoveEvent(std::wstring const& name) {
	EventMap::iterator it = m_events.find(name);
	if (it != m_events.end()) {
		it->second.ClearSubscribers();
		m_events.erase(it);
	}
}

// 0x42fa20
bool RFEventHandler::IsEventPresent(std::wstring const& name) const {
	// The inverted sense (true when absent) is the original's (sete).
	EventMap::const_iterator it = m_events.find(name);
	return it == m_events.end();
}

// 0x42fa50
void RFEventHandler::SubscribeEvent(std::wstring const& name, Subscriber const& subscriber) {
	EventMap::iterator it = m_events.find(name);
	if (it == m_events.end()) {
		AddEvent(name);
	}
	m_events.find(name)->second.Subscribe(subscriber);
}

// 0x42faa0
void RFEventHandler::ClearEventSubscribers(std::wstring const& name) {
	EventMap::iterator it = m_events.find(name);
	if (it != m_events.end()) {
		it->second.ClearSubscribers();
	}
}

// 0x42fb00
R_ERRORTYPE RFEventHandler::SendEvent(std::wstring const& name, EventArgs& args) {
	EventMap::iterator it = m_events.find(name);
	if (it == m_events.end()) {
		return R_EVENT_NOT_FOUND;
	}
	it->second(args);
	return R_OK;
}
