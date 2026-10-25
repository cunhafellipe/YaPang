// The JP 983 lobby event banner's page list (TOPPAGE.notice, "notice_popup").
// JP CExtraResource (WSingleton, global 0xe74d14, 0x104 bytes, ctor
// 0x4e0e30, dtor 0x4e09f0) holds it at +0x98 as a std::vector<std::string>
// of image paths. CProjectG's init (0xa69400) builds it and runs its load
// 0x4e1060: "extracontents.xml" fetched over HTTP (0xa4cb40, WinINet) from
// "http://dl.pangya.jp/pangya/season4/patch/" (0x4df070), root element
// "extracontents" -> "themes" -> the theme's "src"/"url" (0x4e0070), the
// theme document fetched the same way (0x4e02b0), then its "notice" element
// (or "weblogin_notice" for a web login) read by 0x4e0f90: each "file"
// child resolved to a local path in "extrares\" (0x4df170: attributes
// "name", "version", "url"; downloaded when missing) and pushed when not
// empty (0x483e60). CTaskMain's notice handler (0x659290) passes the list
// to FrNoticeDlg's page setter (0xae5630). No game packet carries it, and
// dl.pangya.jp is gone: the port's list stays empty. The HTTP fetch and
// 0x4df170 are not decompiled.
#ifndef PG_JP_JPNOTICEPAGES_H
#define PG_JP_JPNOTICEPAGES_H

#include <string>
#include <vector>

// 0x4dbb40: CExtraResource's notice page list (+0x98).
const std::vector<std::string>& JpExtraResource_GetNoticeList(void);

// 0x4e0f90's store: appends one resolved page path, skipped when empty.
void JpExtraResource_AddNoticeFile(const std::string& path);

#endif // PG_JP_JPNOTICEPAGES_H
