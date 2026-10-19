// Original: d:\build\custom\temp\code\shared\localize.h
#ifndef PG_SHARED_LOCALIZE_H
#define PG_SHARED_LOCALIZE_H

#include "defines.h"

// Contents switched on or off per service region; the service header
// (localize_kor.h) sets one bit of a 164-bit set per content, so the
// enumerators are indices below 164. The S3_/S4_ names are the original's,
// recorded by the reference tree (externs/pangea, quoting the original
// shared/localize.h); REAL_MYROOM names only the value, from what it gates.
// The PDB has no records for this enum (ProjectG's objects are /Zd), so the
// remaining values are known by number only.
enum localContentType_t {
	S3_BONGDARISHOP = 1,
	S3_SCRATCH = 2,
	S3_RECYCLE = 5,
	S3_BS_USABLE_TIMES = 6,
	S3_BS_4TH_RARE_ITEM = 7,
	S3_CADDIE_REPORT = 0x14,
	S3_INTRUSION = 0x2E,
	S3_STRENGTH = 0x31,
	S3_TIKI_REPORT = 0x3D,
	S4_CARD_SYSTEM = 0x51,
	REAL_MYROOM = 0x56,
	S4_UCC = 0x57,
	S4_TREASURE_HUNTER = 0x58,
	S4_MATCHING_SYSTEM = 0x59,
	S4_GHOST = 0x5D,
	S4_MAILBOX_FOR_REAL_MYROOM = 0x60,
	S4_OFFLINE_SHOP = 0x66,
	S4_TREASURE_HUNTER_AD = 0x67,
	S4_MASCOT_NO_LIMIT = 0x6C,
	S4NT_VIEW_OFFLINE_GUILD = 0x7D,
	S4_NT_NICKNAME_CHANGE_2ND = 0x82
};

bool IsLocalCountry(__int64 country);
bool IsLocalContent(localContentType_t type);
int GetLocalCountry();
void InitLocalizeSystem();

#endif // PG_SHARED_LOCALIZE_H
