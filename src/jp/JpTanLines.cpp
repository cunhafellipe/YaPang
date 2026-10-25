// The JP 983 "TanLines" game data (JpTanLines.h), from the JP exe's bodies
// (node scripts/disasm.ts --jp 0x<va>).
#include "JpTanLines.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gamedatadb.h"
#include "parttidlist.h"
#include "tinyxml.h"
#include "westpak.h"
#include "wresrcmng.h"

extern WResourceManager* g_resrcmng;

// The JP CGameDataDB kind of the TanLines data (GetGameData 0xa41a30 with
// 4, "TanLines").
static const int kJpGameDataTanLines = 4;

// JP 0xe75120
static JpTanLines* s_tanLines = NULL;

// Port adapter for the JP CGameDataDB::GetGameData (0xa41a30): the KR
// database the port runs has the kinds 0..3 only (eGAMEDATADB_MAX) and
// does not read the JP kinds' tags, so a JP kind gets an empty instance of
// its own (no rows: every TanLines lookup then falls back to the plain
// texture name, as the JP data does for a part without a row) until
// gamedatadb.h gains the JP kinds.
static IGameData* Jp_GameDataDB_GetGameData(int kind, const char* name) {
	if (kind < eGAMEDATADB_MAX) {
		return CGameDataDB::Instance()->GetGameData(static_cast<eGameDataDB>(kind), name);
	}
	static JpTanLines s_emptyTanLines;
	return &s_emptyTanLines;
}

// 0x542c80
bool JpTanLines::FileExists(const char* name) {
	cFile* file = g_resrcmng->GetCFile(name, 0xffff);
	if (file == NULL) {
		return false;
	}
	CloseCFile(file);
	return true;
}

// 0x542cb0
void JpTanLines::ResetState() {
	m_stateValue = 0;
	m_stateFlag = false;
}

// 0x542cf0
void JpTanLines::ClearTids() {
	for (int i = 0; i < 24; i++) {
		m_tids[i] = 0;
	}
	m_hasTids = false;
}

// 0x542d40
void JpTanLines::SetTids(const CPartTidList* parts) {
	if (parts == NULL) {
		return;
	}
	for (int i = 0; i < 24; i++) {
		m_tids[i] = static_cast<uint32_t>(parts->m_partTids[i]);
	}
	m_hasTids = true;
}

// 0x542d70
void JpTanLines::SetTids(const uint32_t* tids) {
	if (tids == NULL) {
		return;
	}
	for (int i = 0; i < 24; i++) {
		m_tids[i] = tids[i];
	}
	m_hasTids = true;
}

// 0x542f00
JpTanLines* JpTanLines::Instance() {
	if (s_tanLines == NULL) {
		s_tanLines = static_cast<JpTanLines*>(Jp_GameDataDB_GetGameData(kJpGameDataTanLines, "TanLines"));
	}
	return s_tanLines;
}

// 0x543740
bool JpTanLines::HasTanLine() {
	const uint32_t* tids = m_tids;
	if (tids == NULL) {
		return false;
	}
	for (int i = 0; i < 24; i++) {
		uint32_t tid = tids[i];
		if (tid == 0) {
			continue;
		}
		JpTanLine line;
		for (std::list<JpTanLine>::iterator it = m_lines.begin(); it != m_lines.end(); ++it) {
			line = *it;
			if (line.m_tid == tid) {
				return true;
			}
		}
	}
	return false;
}

// 0x544200
bool JpTanLines::FindTexture(const char* keyword, const char* path, std::string& out, const CPartTidList* parts) {
	SetTids(parts);
	JpTanLine line;
	for (int i = 0; i < 24; i++) {
		uint32_t tid = parts != NULL ? static_cast<uint32_t>(parts->m_partTids[i]) : m_tids[i];
		if (tid == 0) {
			continue;
		}
		for (std::list<JpTanLine>::iterator it = m_lines.begin(); it != m_lines.end(); ++it) {
			line = *it;
			if (line.m_tid == tid && line.m_name != keyword) {
				char fname[0x100];
				char ext[0x100];
				_splitpath(path, NULL, NULL, fname, ext);
				out = fname + line.m_suffix + ext;
				if (!FileExists(out.c_str())) {
					out = path;
				}
				return true;
			}
		}
	}
	out = path;
	return false;
}

// 0x544880
JpTanLines::JpTanLines()
	: m_flag7a(false) {
}

// 0x544910
JpTanLines::~JpTanLines() {
	s_tanLines = NULL;
}

// 0x5449d0
void JpTanLines::Initialize(TiXmlNode* node) {
	TiXmlNode* row = node->FirstChild("row");
	while (row != NULL) {
		JpTanLine line;
		line.m_tid = 0;
		std::string typeId = row->ToElement()->Attribute("typeid");
		line.m_name = row->ToElement()->Attribute("name");
		line.m_suffix = row->ToElement()->Attribute("value");
		sscanf(typeId.c_str(), "%d", reinterpret_cast<int*>(&line.m_tid));
		m_lines.push_back(line);
		row = row->NextSibling("row");
	}
	ResetState();
	ClearTids();
}
