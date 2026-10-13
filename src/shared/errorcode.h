// Original: d:\build\custom\temp\code\shared\errorcode.h
#ifndef PG_SHARED_ERRORCODE_H
#define PG_SHARED_ERRORCODE_H

namespace Cm {

// Result code of a server reply (the trade gateway replies 0x1D6-0x1D8 open
// with one). Only the success value is known; any other value is an error
// code passed on to the trade handler.
enum eErrorCode {
	ERROR_CODE_SUCCESS = 0
};

}

// errorcode.h:347
inline bool IS_SUCCESSFUL(Cm::eErrorCode code) {
	// errorcode.h:348
	return code == Cm::ERROR_CODE_SUCCESS ? true : false;
}

#endif // PG_SHARED_ERRORCODE_H
