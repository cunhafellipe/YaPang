// The JP 983 lobby event banner's page list (JpNoticePages.h): the
// CExtraResource notice list (global 0xe74d14, +0x98), from the JP exe's
// bodies (node scripts/jp_hlil.ts 0x<va>).
#include "JpNoticePages.h"

// CExtraResource +0x98 (std::vector<std::string>, freed by the dtor 0x4e09f0).
static std::vector<std::string> s_noticeList;

// 0x4dbb40
const std::vector<std::string>& JpExtraResource_GetNoticeList(void) {
	return s_noticeList;
}

// 0x4e0f90 (the push of each "file" child's resolved path)
void JpExtraResource_AddNoticeFile(const std::string& path) {
	if (path.size() > 0) {
		s_noticeList.push_back(path);
	}
}
