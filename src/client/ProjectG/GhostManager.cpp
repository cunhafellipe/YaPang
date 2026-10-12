// Original: d:\build\custom\temp\code\client\projectg\ghostmanager.cpp
#include <windows.h>
#include <stdio.h>

#include "ghostmanager.h"
#include "type_convert.h"

#include "ghostdocument.h"
#include "ghosthandler.h"
#include "ghosttypes.h"
#include "globalgamedefine.h"
#include "NetResourceManager.h"
#include "SharedDoc.h"
#include "GolfDoc.h"

// ghostmanager.cpp:10
CGhostDocument *CreateDocument(void) {
	return new CGhostDocument;
}

// ghostmanager.cpp:17
CGhostManager::CGhostManager(void)
	: m_pDocument(0), m_pLatestGameBrief(0) {
}

// ghostmanager.cpp:22
CGhostManager::~CGhostManager(void) {
	if (m_pDocument) { delete m_pDocument; m_pDocument = 0; }
	if (m_pLatestGameBrief) { delete m_pLatestGameBrief; m_pLatestGameBrief = 0; }
}

// ghostmanager.cpp:28
CGhostDocument *CGhostManager::GetDocument(void) {
	if (m_pDocument == 0) {
		m_pDocument = CreateDocument();
	}
	return m_pDocument;
}

// ghostmanager.cpp:36
void CGhostManager::ReleaseDocument(void) {
	if (m_pDocument) { delete m_pDocument; m_pDocument = 0; }
}

// ghostmanager.cpp:44
void CGhostManager::SaveDocument(const std::string &fileName) {
	if (m_pDocument) {
		GetTickCount();
		m_pDocument->Export(fileName);

		if (m_pDocument) { delete m_pDocument; m_pDocument = 0; }

		sResource res;
		res.path = fileName;

		res.type = RESOURCE_TYPE_3;
		sprintf(res.data, "%d", MyUID());

		NetResourceManager::Instance()->Upload(res, "http://qa.contents.pangya.gametree.co.kr:50006/GHOST/upload.asp");
	}
}

// ghostmanager.cpp:62
void CGhostManager::LoadDocument(const std::string &fileName) {
	m_latestFileName = fileName;
	if (m_pLatestGameBrief) { delete m_pLatestGameBrief; m_pLatestGameBrief = 0; }
	m_pLatestGameBrief = new GHOST_GAME_BRIEF;
	unsigned long userId = GetGhostGameBriefFromFileName(m_pLatestGameBrief, fileName.c_str());

	sResource res;
	res.path = fileName;

	res.type = RESOURCE_TYPE_3;
	sprintf(res.data, "%d", userId);

	NetResourceManager::Instance()->Download(res, "http://qa.contents.pangya.gametree.co.kr:50006/GHOST/GHOST/");
}

// ghostmanager.cpp:77
void CGhostManager::LoadDocumentFromLocalFile(const std::string &fileName) {
	m_latestFileName = fileName;
	if (m_pLatestGameBrief) { delete m_pLatestGameBrief; m_pLatestGameBrief = 0; }
	m_pLatestGameBrief = new GHOST_GAME_BRIEF;
	GetGhostGameBriefFromFileName(m_pLatestGameBrief, fileName.c_str());

	CGhostHandler::Instance()->LoadingComplete(CGhostLoader().LoadFromFile(fileName));
}

struct sUserInfo;
struct sSlotInfo;
struct sShotProtocol;
struct GHOST_USER_DATA;

void conv_userinfo_to_ghostdata(const sUserInfo *src, GHOST_USER_DATA *dst);
void conv_ghostdata_to_userinfo(const GHOST_USER_DATA *src, sUserInfo *dst);
void conv_ghostdata_to_slotinfo(const GHOST_USER_DATA *src, sSlotInfo *dst);
// ghostmanager.cpp:94
void conv_ghostdata_to_userinfo(const GHOST_USER_DATA *src, sUserInfo *dst) {
	dst->m_characterInfo.typeId = src->characterTypeId;
	dst->m_characterInfo.hairColor = src->hairColor;
	memcpy(dst->m_characterInfo.partTypeIds, src->partTypeIds, sizeof(dst->m_characterInfo.partTypeIds));
	memcpy(&dst->m_characterInfo.auxParts, src->auxPartTids, sizeof(dst->m_characterInfo.auxParts));
	memcpy(&dst->m_characterInfo.unknown6A, src->characterExtra, sizeof(dst->m_characterInfo.unknown6A));

	dst->m_clubSet.typeId = src->clubSetTypeId;
	memcpy(dst->m_clubSet.statLevel, src->clubStat, sizeof(dst->m_clubSet.statLevel));

	dst->m_ballId = src->ballTypeId;

	dst->m_caddieInfo.typeId = src->caddieTypeId;
	dst->m_caddieInfo.cadItemTypeId = src->cadItemTypeId;
	dst->m_caddieInfo.level = src->caddieLevel;

	dst->m_mascot.typeId = src->mascotTypeId;
	py_strcpy(dst->m_mascot.message, src->mascotMessage);

	memcpy(dst->m_equipItem, src->equipItem, sizeof(dst->m_equipItem));

	memcpy(&dst->m_characterInfo.unknown84, src->characterBlock, sizeof(dst->m_characterInfo.unknown84));

	py_strcpy(dst->m_name, src->name);
	py_strcpy(dst->m_nickname, src->nickname);
	py_strcpy(dst->m_guildName, src->guildName);
	py_strcpy(dst->m_guildMark, src->guildMark);

	dst->m_identity = src->identity;

	dst->m_eventFlags.gender = src->eventFlag1;
	dst->m_eventFlags.eventFlag5 = src->eventFlag5;
	dst->m_level = src->level;
	dst->m_eventFlags.eventFlag3 = src->eventFlag3;
	dst->m_eventFlags.eventFlag4 = src->eventFlag4;
	dst->m_uid = src->uid;
}

// ghostmanager.cpp:144
void conv_userinfo_to_ghostdata(const sUserInfo *src, GHOST_USER_DATA *dst) {
	dst->characterTypeId = src->m_characterInfo.typeId;
	dst->hairColor = src->m_characterInfo.hairColor;
	memcpy(dst->partTypeIds, src->m_characterInfo.partTypeIds, sizeof(dst->partTypeIds));
	memcpy(dst->auxPartTids, &src->m_characterInfo.auxParts, sizeof(dst->auxPartTids));
	memcpy(dst->characterExtra, &src->m_characterInfo.unknown6A, sizeof(dst->characterExtra));

	dst->clubSetTypeId = src->m_clubSet.typeId;
	memcpy(dst->clubStat, src->m_clubSet.statLevel, sizeof(dst->clubStat));

	dst->ballTypeId = src->m_ballId;

	dst->caddieTypeId = src->m_caddieInfo.typeId;
	dst->cadItemTypeId = src->m_caddieInfo.cadItemTypeId;
	dst->caddieLevel = src->m_caddieInfo.level;

	dst->mascotTypeId = src->m_mascot.typeId;
	py_strcpy(dst->mascotMessage, src->m_mascot.message);

	memcpy(dst->equipItem, src->m_equipItem, sizeof(dst->equipItem));

	memcpy(dst->characterBlock, &src->m_characterInfo.unknown84, sizeof(dst->characterBlock));

	py_strcpy(dst->name, src->m_name);
	py_strcpy(dst->nickname, src->m_nickname);
	py_strcpy(dst->guildName, src->m_guildName);
	py_strcpy(dst->guildMark, src->m_guildMark);

	dst->identity = src->m_identity;

	dst->eventFlag1 = src->m_eventFlags.gender;
	dst->eventFlag3 = src->m_eventFlags.eventFlag3;
	dst->eventFlag4 = src->m_eventFlags.eventFlag4;
	dst->eventFlag5 = src->m_eventFlags.eventFlag5;
	dst->level = src->m_level;
	dst->uid = src->m_uid;
}

// ghostmanager.cpp:194
void conv_ghostdata_to_slotinfo(const GHOST_USER_DATA *src, sSlotInfo *dst) {
	py_strcpy(dst->m_nickname, src->name);
	py_strcpy(dst->m_guildName, src->guildName);
	py_strcpy(dst->m_emblemName, src->guildMark);

	dst->m_level = src->level;
	dst->m_identity = src->identity;

	dst->m_attributeBits.userFlag = src->eventFlag1;
	dst->m_flags57.flag0 = src->eventFlag3;
	dst->m_flags57.flag1 = src->eventFlag4;
	dst->m_flags55.flag1 = 1;
	dst->m_connection = 2;
	dst->m_uid = 0xFFFFFFFE; // a ghost
	dst->m_characterTypeId = src->characterTypeId;
	dst->m_mascotTypeId = src->mascotTypeId;
}

// ghostmanager.cpp:216
void conv_legacycard_to_ghostcard(const sSCardAvilityPeriodInfo *src, GHOST_CARD_DATA *dst) {
	dst->typeId = src->typeId;
	dst->id = src->id;
	dst->partsTypeId = src->partsTypeId;
	dst->partsId = src->partsId;
	dst->effect = src->effect;
	dst->effectValue = src->effectValue;
	dst->slot = src->slot;
	dst->kind = src->kind;
}

// ghostmanager.cpp:228
void conv_ghostcard_to_legacycard(const GHOST_CARD_DATA *src, sSCardAvilityPeriodInfo *dst) {
	dst->effect = src->effect;
	dst->effectValue = src->effectValue;
	dst->kind = src->kind;
	dst->partsTypeId = src->partsTypeId;
	dst->partsId = src->partsId;
	dst->slot = src->slot;
	dst->typeId = src->typeId;
	dst->id = src->id;
	dst->use = 1;
}

// ghostmanager.cpp:241
void conv_ghostshot_to_shotprotocol(const sShotProtocol *src, GHOST_SHOT_PROTOCOL *dst) {
	dst->power = src->power;
	dst->accuracy = src->accuracy;
	dst->lieSlope = src->lieSlope;
	dst->lieAngle = src->lieAngle;
	dst->accuracyGrade = src->accuracyGrade;
	dst->powerFlags = src->powerFlags;
	dst->frame = src->frame;
	dst->angle = src->angle;
	dst->shotElapsed = src->shotElapsed;
	dst->barStartPosition = src->barStartPosition;
	dst->club = src->club;
	dst->randomPower = src->randomPower;
	dst->spot = src->spot;
}

// ghostmanager.cpp:258
void conv_shotprotocol_to_ghostshot(const GHOST_SHOT_PROTOCOL *src, sShotProtocol *dst) {
	dst->power = src->power;
	dst->accuracy = src->accuracy;
	dst->lieSlope = src->lieSlope;
	dst->lieAngle = src->lieAngle;
	dst->accuracyGrade = src->accuracyGrade;
	dst->powerFlags = src->powerFlags;
	dst->frame = src->frame;
	dst->angle = src->angle;
	dst->shotElapsed = src->shotElapsed;
	dst->barStartPosition = src->barStartPosition;
	dst->club = src->club;
	dst->randomPower = src->randomPower;
	dst->spot = src->spot;
}

// Registration of the converters (one dynamic initializer per line).
// ghostmanager.cpp:274
static bool s_userInfoToGhostData = type_convert::insert_converter<sUserInfo, GHOST_USER_DATA>((type_convert::converter)conv_userinfo_to_ghostdata);
// ghostmanager.cpp:275
static bool s_ghostDataToUserInfo = type_convert::insert_converter<GHOST_USER_DATA, sUserInfo>((type_convert::converter)conv_ghostdata_to_userinfo);
// ghostmanager.cpp:276
static bool s_ghostDataToSlotInfo = type_convert::insert_converter<GHOST_USER_DATA, sSlotInfo>((type_convert::converter)conv_ghostdata_to_slotinfo);
// ghostmanager.cpp:277
static bool s_ghostCardToLegacyCard = type_convert::insert_converter<GHOST_CARD_DATA, sSCardAvilityPeriodInfo>((type_convert::converter)conv_ghostcard_to_legacycard);
// ghostmanager.cpp:278
static bool s_legacyCardToGhostCard = type_convert::insert_converter<sSCardAvilityPeriodInfo, GHOST_CARD_DATA>((type_convert::converter)conv_legacycard_to_ghostcard);
// ghostmanager.cpp:279
static bool s_ghostShotToShotProtocol = type_convert::insert_converter<sShotProtocol, GHOST_SHOT_PROTOCOL>((type_convert::converter)conv_ghostshot_to_shotprotocol);
// ghostmanager.cpp:280
static bool s_shotProtocolToGhostShot = type_convert::insert_converter<GHOST_SHOT_PROTOCOL, sShotProtocol>((type_convert::converter)conv_shotprotocol_to_ghostshot);
