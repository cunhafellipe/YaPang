// Original: client\Refresh\include\EventFunctorBase.h (JP 983 exe; path
// from the library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_EVENTFUNCTORBASE_H
#define PG_CLIENT_REFRESH_INCLUDE_EVENTFUNCTORBASE_H

#include "RFExport.h"
#include "defines.h"

class EventArgs;

// The base of the event functors: vtable 0xcfe638, 5 slots, size 0x4 (the
// ctors write only the vptr and the copy ctor copies nothing: no members).
// Its members are header inlines: their copies sit in the run of inlines at
// 0x401280-0x401bb0 (with RFVector2's, Subscriber's and RFEvent's), and
// MemberFunctionFunctor<RFWnd>'s constructor 0x40fbe0 inlines the base one.
// Slots [1]-[4] are pure virtuals the exports do not name; their shapes come
// from MemberFunctionFunctor<RFWnd> (vtable 0xd63104: 0x40fc00 calls the
// member function and returns true, 0x40fc20/0x40fc30 increment/decrement
// the count at +0xc, 0x40fc40 returns it) and from ~Subscriber 0x401790
// (calls [3], then deletes the functor when [4] returns 0).
class RF_API EventFunctorBase {
	LAYOUT_CHECK_FRIEND;

public:
	// [0] vector deleting destructor 0x4016e0 (scalar 0x4016b0)
	// 0x401670
	virtual ~EventFunctorBase() {
	}
	virtual bool operator()(EventArgs& args) = 0;  // [1] purecall
	virtual void AddRef() = 0;                     // [2] purecall
	virtual void Release() = 0;                    // [3] purecall
	virtual int GetRefCount() const = 0;           // [4] purecall

	// 0x401680
	EventFunctorBase() {
	}
	// 0x401690
	EventFunctorBase(EventFunctorBase const&) {
	}
	// 0x4016a0
	EventFunctorBase& operator=(EventFunctorBase const&) {
		return *this;
	}
};
CHECK_SIZE(EventFunctorBase, 0x4);

// An event functor calling a member function of T, size 0x10. Built with a
// zero count by its constructor (MemberFunctionFunctor<RFWnd> 0x40fbe0, also
// inlined at 0x404c97 and 0x40632b, vtable 0xd63104: [1] 0x40fc00, [2]
// 0x40fc20, [3] 0x40fc30, [4] 0x40fc40); the Subscriber holding it takes the
// first reference. The handlers are `void (T::*)(EventArgs&)`
// (RFWnd::KeyDownEventHandler 0x405760). Other instantiations: <RFSlider>
// 0xd65ba8, <RFTextAreaSC> 0xd65b58, <RFListSC> 0xd65bf4, <RFComboBox>
// 0xd65d5c; their code is emitted and paired in their users' units.
template <class T>
class MemberFunctionFunctor : public EventFunctorBase {
public:
	typedef void (T::*Function)(EventArgs&);

	MemberFunctionFunctor(Function function, T* object)
		: m_function(function), m_object(object), m_refCount(0) {
	}

	// [1]
	virtual bool operator()(EventArgs& args) {
		(m_object->*m_function)(args);
		return true;
	}
	// [2]
	virtual void AddRef() {
		++m_refCount;
	}
	// [3]
	virtual void Release() {
		--m_refCount;
	}
	// [4]
	virtual int GetRefCount() const {
		return m_refCount;
	}

private:
	Function m_function;  // 0x4
	T* m_object;          // 0x8
	int m_refCount;       // 0xc
};

#endif
