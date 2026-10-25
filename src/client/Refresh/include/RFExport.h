// Original: none (project helper: the JP library's classes are dllexport)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFEXPORT_H
#define PG_CLIENT_REFRESH_INCLUDE_RFEXPORT_H

// The matching build needs dllexport: only an exported class gets the exported
// copies, the ??_E vector deleting destructor and the implicit copy members.
#if defined(_MSC_VER)
#define RF_API __declspec(dllexport)
#else
#define RF_API
#endif

#endif // PG_CLIENT_REFRESH_INCLUDE_RFEXPORT_H
