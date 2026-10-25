// The JP 983 "TanLines" game data: a JP-only IGameData kind (CGameDataDB
// kind 4, name "TanLines"; kind 5 is "SetAuxpartsStatus", 0x542280) that
// maps a part type id and a motion keyword to a texture name suffix. The
// pet scripts' motion commands (IPetScriptObject::NextMotionCommand
// 0x5ace20) look their texture names up in it. 0x7c bytes, vtable 0xd0971c
// (2 slots), ctor 0x544880 (created by 0xa40ef0, the kind's
// CreateInstance), singleton pointer 0xe75120. KR has no such kind
// (gamedatadb.h stops at eGAMEDATADB_MAX 4).
// From the JP exe's bodies (node scripts/disasm.ts --jp 0x<va>).
#ifndef PG_JP_JPTANLINES_H
#define PG_JP_JPTANLINES_H

#include <stdint.h>
#include <list>
#include <string>

#include "defines.h"
#include "padding.h"
#include "igamedata.h"

class CPartTidList;
class TiXmlNode;

// One "row" of the TanLines data (0x3c bytes, the list's element).
struct JpTanLine {
	uint32_t m_tid;             // 0x0, "typeid" (sscanf "%d")
	std::string m_name;         // 0x4, "name": the motion keyword the row does NOT apply to
	std::string m_suffix;       // 0x20, "value": inserted between the file name and its extension
};

class JpTanLines : public IGameData {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x544880
	JpTanLines();
	virtual ~JpTanLines();                           // [0] 0x5449a0 (scalar deleting destructor; body 0x544910)
	// [1] 0x5449d0: reads the "row" children ("typeid", "name", "value"),
	// then clears the state (0x542cb0, 0x542cf0).
	virtual void Initialize(TiXmlNode* node);

	// 0x542f00: the database's "TanLines" entry (CGameDataDB kind 4), kept
	// in 0xe75120.
	static JpTanLines* Instance();

	// 0x542cb0
	void ResetState();
	// 0x542cf0: clears m_tids and m_hasTids.
	void ClearTids();
	// 0x542d40: copies the list's 24 part tids (no-op for NULL).
	void SetTids(const CPartTidList* parts);
	// 0x542d70: copies 24 tids (no-op for NULL).
	void SetTids(const uint32_t* tids);
	// 0x542c80: whether the resource manager finds the file (opened and
	// closed again). Does not read this.
	bool FileExists(const char* name);
	// 0x543740 (the argument NextMotionCommand passes is not read): whether
	// one of m_tids has a row.
	bool HasTanLine();
	// 0x544200: out = path with the suffix of the first row of one of the
	// tids (parts' when given, after SetTids(parts); m_tids otherwise)
	// whose name differs from keyword, when that file exists; path
	// otherwise. True when a row matched.
	bool FindTexture(const char* keyword, const char* path, std::string& out, const CPartTidList* parts);

	std::list<JpTanLine> m_lines;     // 0x4 (head 0x4, size 0x8)
	int32_t m_stateValue;             // 0x10, zeroed by ResetState (role not traced)
	bool m_stateFlag;                 // 0x14, cleared by ResetState (role not traced)
	PADDING(0x15, 0x18);
	uint32_t m_tids[24];              // 0x18, the worn part tids (SetTids)
	bool m_hasTids;                   // 0x78, set by SetTids
	PADDING(0x79, 0x7a);
	bool m_flag7a;                    // 0x7a, cleared by the ctor (role not traced)
	PADDING(0x7b, 0x7c);
};

CHECK_SIZE(JpTanLines, 0x7c);
BEGIN_LAYOUT_CHECK(JpTanLines)
	CHECK_FIELD(JpTanLines, m_lines, 0x4);
	CHECK_FIELD(JpTanLines, m_stateValue, 0x10);
	CHECK_FIELD(JpTanLines, m_stateFlag, 0x14);
	CHECK_FIELD(JpTanLines, m_tids, 0x18);
	CHECK_FIELD(JpTanLines, m_hasTids, 0x78);
	CHECK_FIELD(JpTanLines, m_flag7a, 0x7a);
END_LAYOUT_CHECK
CHECK_FIELD(JpTanLine, m_name, 0x4);
CHECK_FIELD(JpTanLine, m_suffix, 0x20);
CHECK_SIZE(JpTanLine, 0x3c);

#endif // PG_JP_JPTANLINES_H
