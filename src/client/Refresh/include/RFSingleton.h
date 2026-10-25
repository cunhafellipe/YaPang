// Original: client\Refresh\include\RFSingleton.h (JP 983 exe; the path the
// library's assert strings name, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RF_SINGLETON_H
#define PG_CLIENT_REFRESH_INCLUDE_RF_SINGLETON_H
#include <assert.h>
#include <stddef.h>

// The singleton base of the Refresh managers (RFManager, RFWindowFactoryManager).
// Its asserts are live (original lines 21 and 32; the units build with /UNDEBUG).
template <class T>
class RFSingleton {
public:
	// lib: 0x4030a0 (RFSingleton<RFManager>::operator=), 0x417140
	// (RFSingleton<RFWindowFactoryManager>::operator=)
	RFSingleton& operator=(RFSingleton const&) {
		return *this;
	}

	// ctor 0x409580, 0x41aa70: registers the one instance
	RFSingleton() {
		assert(_singleton == NULL);
		_singleton = static_cast<T*>(this);
	}

	// dtor 0x4095c0, 0x41aaa0
	~RFSingleton() {
		_singleton = 0;
	}

	// inst 0x4095d0, 0x41aab0: there must be an instance
	static T& GetInstance() {
		assert(_singleton);
		return *_singleton;
	}

	// 0x409600, 0x41aae0
	static T* GetInstancePtr() {
		return _singleton;
	}

protected:
	// The one instance (protected, as the export's decoration says):
	// 0xF1DDA0 (RFManager), 0xF1DDDC (RFWindowFactoryManager).
	static T* _singleton;
};

template <class T>
T* RFSingleton<T>::_singleton;

#endif
