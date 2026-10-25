// Original: client\Refresh\include\RFEventHandler.h (JP 983 exe; the assert
// of the Subscriber copy constructor 0x401750 names this file, line 147:
// Subscriber is declared here with inline members, and RFEvent with it, since
// RFEventHandler's map needs it complete)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFEVENTHANDLER_H
#define PG_CLIENT_REFRESH_INCLUDE_RFEVENTHANDLER_H

#include <assert.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "EventFunctorBase.h"
#include "RFExport.h"
#include "RFObject.h"
#include "defines.h"

class EventArgs;

// The library's result codes (the exports return them as R_ERRORTYPE). Only
// the values the library returns; the names are ours, from use.
enum R_ERRORTYPE {
	// A missing argument or target: RFManager::DeleteDesktop 0x4183c0 (no
	// desktop), DeleteWnd 0x41841f (null window), RFDirector::Play 0x431265
	// (unknown scene), 0x43130c (index out of range), RFAnimButton::
	// SetAnimation 0x4348d0 and RFPicture::SetAnimation 0x41e8ef (null
	// director). RFDirector::Play(RFWnd*, ...) 0x4312b7 returns it always.
	R_FAIL = 0,
	// Done: DeleteDesktop 0x418400, DeleteWnd 0x418458, Play 0x431281,
	// 0x4312ed, SetAnimation 0x4348c8 / 0x41e8e5, SendEvent 0x42fb00,
	// LoadAnimation_ 0x430f9a.
	R_OK = 1,
	// RFDirector::LoadAnimation_ could not load the file: FOpen failed or the
	// file is empty (0x430cb0), the XML does not parse (0x430d80), it has no
	// "rfanimation" element (0x430dc8).
	R_LOAD_FAILED = 2,
	// RFXmlLoader::LoadVersion1 0x43f7e0: the document has no <base> element.
	R_NO_BASE_ELEMENT = 4,
	// RFXmlLoader::LoadVersion1Children 0x43f9c0: the root has no <child>
	// element.
	R_NO_CHILD_ELEMENT = 5,
	// RFEventHandler::SendEvent 0x42fb00: no event has that name.
	R_EVENT_NOT_FOUND = 7
};

// A reference-counted handle on an event functor: copying it takes a
// reference, destroying it drops one and deletes the functor at zero. Size
// 0x4. The members are header inlines: RFEvent::operator() 0x401990 inlines
// operator(), and the vector helpers inline the copy constructor and the
// destructor. The assignment operator 0x4017d0 is the implicit one (copies
// the pointer, no reference taken).
class RF_API Subscriber {
	LAYOUT_CHECK_FRIEND;

public:
	// 0x401740
	Subscriber() : _impl(0) {
	}
	// Defined after RFEvent: its assert sits on the original's line 147.
	Subscriber(Subscriber const& subscriber);
	// 0x401790
	~Subscriber() {
		if (_impl) {
			_impl->Release();
			if (_impl->GetRefCount() == 0) {
				delete _impl;
			}
		}
	}

	// News the functor and takes the first reference without a null test
	// (inlined by RFButton::PreInitialize 0x420fd0, RFListSC::PreInitialize
	// 0x450270).
	template <class T>
	Subscriber(void (T::*function)(EventArgs&), T* object)
		: _impl(new MemberFunctionFunctor<T>(function, object)) {
		_impl->AddRef();
	}

	// 0x4017c0
	bool operator()(EventArgs& args) {
		return (*_impl)(args);
	}

private:
	EventFunctorBase* _impl;  // 0x0 (named by the assert "subscriber._impl")
};
CHECK_SIZE(Subscriber, 0x4);
BEGIN_LAYOUT_CHECK(Subscriber)
	CHECK_FIELD(Subscriber, _impl, 0x0);
END_LAYOUT_CHECK

// A named event: the subscribers it calls when raised. No vtable, size 0x2c
// (the constructors write the name and the three vector pointers). The copy
// constructor 0x4019e0 and the copy assignment 0x401a60 are the implicit
// ones, exported with the class. Every member is a header inline: the
// exported copies sit in the run of inlines at 0x401280-0x401bb0, away from
// RFEventHandler's code at 0x42f5a0, and RFEventHandler::AddEvent 0x42f760
// inlines the constructor.
class RF_API RFEvent {
	LAYOUT_CHECK_FRIEND;

public:
	// 0x4017e0
	RFEvent(std::wstring const& name) : m_name(name) {
	}
	// 0x401820
	~RFEvent() {
		Cleanup();
	}

	// 0x4018f0
	void Cleanup() {
		m_subscribers.clear();
	}
	// 0x401930
	void Subscribe(Subscriber const& subscriber) {
		m_subscribers.push_back(subscriber);
	}
	// 0x401950
	void ClearSubscribers() {
		m_subscribers.clear();
	}
	// 0x401990
	bool operator()(EventArgs& args) {
		bool handled = false;
		for (unsigned int i = 0; i < m_subscribers.size(); ++i) {
			handled |= m_subscribers[i](args);
		}
		return handled;
	}

private:
	std::wstring m_name;                   // 0x0
	std::vector<Subscriber> m_subscribers;  // 0x1c
};
CHECK_SIZE(RFEvent, 0x2c);
BEGIN_LAYOUT_CHECK(RFEvent)
	CHECK_FIELD(RFEvent, m_name, 0x0);
	CHECK_FIELD(RFEvent, m_subscribers, 0x1c);
END_LAYOUT_CHECK

// Subscriber's copy constructor: every copy of its assert pushes line 147.
// 0x401750
inline Subscriber::Subscriber(Subscriber const& subscriber) {
	assert(subscriber._impl);
	_impl = subscriber._impl;
	_impl->AddRef();
}

// Base of the Refresh objects that own events: vtable 0xd63054, 2 slots
// like RFObject's, size 0x30 (from its vector deleting destructor
// 0x401bb0). An RFObject: the constructors enter EH state 0 (base built)
// before building the map. The events are a std::unordered_map by name
// (its constructor 0x42fc80 copies the 2-byte _Umap_traits; find returns
// the list node: the RFEvent sits at node+0x24, after the two links and
// the 0x1c key). The copy constructor
// 0x401aa0 and the copy assignment 0x401b60 are the implicit ones.
class RF_API RFEventHandler : public RFObject {
	LAYOUT_CHECK_FRIEND;

public:
	// [0] 0x401a90 (a header inline: its copy sits in the run of inlines
	// at 0x401280-0x401bb0 with the implicit copy constructor and operator=)
	virtual wchar_t const* ToString() const {
		return L"RFEventHandler";
	}
	// [1] vector deleting destructor 0x401bb0, ~RFEventHandler 0x42f600
	virtual ~RFEventHandler();

	RFEventHandler();

	void AddEvent(std::wstring const& name);
	void RemoveEvent(std::wstring const& name);
	bool IsEventPresent(std::wstring const& name) const;
	void SubscribeEvent(std::wstring const& name, Subscriber const& subscriber);
	void ClearEventSubscribers(std::wstring const& name);
	R_ERRORTYPE SendEvent(std::wstring const& name, EventArgs& args);

protected:
	// Protected: RFManager::PostGlobalHyperLinkLeaveEvent 0x4181f0 and
	// PostIMStatusEvent 0x418270 look events up in it directly.
	typedef std::unordered_map<std::wstring, RFEvent> EventMap;

	EventMap m_events;  // 0x4
};
CHECK_SIZE(RFEventHandler, 0x30);
BEGIN_LAYOUT_CHECK(RFEventHandler)
	CHECK_FIELD(RFEventHandler, m_events, 0x4);
END_LAYOUT_CHECK

#endif
