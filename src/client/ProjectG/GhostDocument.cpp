// Original: d:\build\custom\temp\code\client\projectg\ghostdocument.cpp
#define GHOSTDOCUMENT_SHAREDDOC_INLINE

#include <windows.h>

#include "ghostdocument.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <iterator>

#include "GolfDoc.h"
#include "actor.h"
#include "club.h"
#include "golfball.h"
#include "SharedDoc.h"
#include "ghosttypes.h"
#include "westpak.h"
#include "s5/Utilities.h"
#include "shared/globalgamedefine.h"
#include "type_convert.h"
#include "wind.h"
#include "../Wangreal/source/XUnzip.h"

cFile *__cdecl GetCFile(const char *fileName, int a, int b);
void __cdecl CloseCFile(cFile *file);

// XZip (Wangreal), not declared by XZip.h yet.
HZIP CreateZipZ(void *z, unsigned int len, unsigned long flags, unsigned long attributes);
ZRESULT ZipAdd(HZIP hz, const char *dstzn, void *src, unsigned int len, unsigned long flags);
ZRESULT CloseZipZ(HZIP hz);

namespace {

// The level names and the exp each level needs, printed by WriteTextFile.
struct LEVEL_INFO {
	char name[16];     // 0x0
	unsigned long exp; // 0x10
};
CHECK_SIZE(LEVEL_INFO, 0x14);
CHECK_FIELD(LEVEL_INFO, name, 0x0);
CHECK_FIELD(LEVEL_INFO, exp, 0x10);

LEVEL_INFO LEVEL_TABLE[71] = {
	{ "\xb7\xe7\xc5\xb0" "F", 30 }, // 0
	{ "\xb7\xe7\xc5\xb0" "E", 40 }, // 1
	{ "\xb7\xe7\xc5\xb0" "D", 50 }, // 2
	{ "\xb7\xe7\xc5\xb0" "C", 60 }, // 3
	{ "\xb7\xe7\xc5\xb0" "B", 70 }, // 4
	{ "\xb7\xe7\xc5\xb0" "A", 100 }, // 5
	{ "\xba\xf1\xb1\xe2\xb3\xca" "E", 133 }, // 6
	{ "\xba\xf1\xb1\xe2\xb3\xca" "D", 148 }, // 7
	{ "\xba\xf1\xb1\xe2\xb3\xca" "C", 163 }, // 8
	{ "\xba\xf1\xb1\xe2\xb3\xca" "B", 178 }, // 9
	{ "\xba\xf1\xb1\xe2\xb3\xca" "A", 213 }, // 10
	{ "\xc1\xd6\xb4\xcf\xbe\xee" "E", 267 }, // 11
	{ "\xc1\xd6\xb4\xcf\xbe\xee" "D", 288 }, // 12
	{ "\xc1\xd6\xb4\xcf\xbe\xee" "C", 309 }, // 13
	{ "\xc1\xd6\xb4\xcf\xbe\xee" "B", 330 }, // 14
	{ "\xc1\xd6\xb4\xcf\xbe\xee" "A", 491 }, // 15
	{ "\xbd\xc3\xb4\xcf\xbe\xee" "E", 660 }, // 16
	{ "\xbd\xc3\xb4\xcf\xbe\xee" "D", 716 }, // 17
	{ "\xbd\xc3\xb4\xcf\xbe\xee" "C", 772 }, // 18
	{ "\xbd\xc3\xb4\xcf\xbe\xee" "B", 828 }, // 19
	{ "\xbd\xc3\xb4\xcf\xbe\xee" "A", 1224 }, // 20
	{ "\xbe\xc6\xb8\xb6\xc3\xdf\xbe\xee" "E", 1656 }, // 21
	{ "\xbe\xc6\xb8\xb6\xc3\xdf\xbe\xee" "D", 1742 }, // 22
	{ "\xbe\xc6\xb8\xb6\xc3\xdf\xbe\xee" "C", 1828 }, // 23
	{ "\xbe\xc6\xb8\xb6\xc3\xdf\xbe\xee" "B", 1914 }, // 24
	{ "\xbe\xc6\xb8\xb6\xc3\xdf\xbe\xee" "A", 2346 }, // 25
	{ "\xbc\xbc\xb9\xcc\xc7\xc1\xb7\xce" "E", 2871 }, // 26
	{ "\xbc\xbc\xb9\xcc\xc7\xc1\xb7\xce" "D", 2994 }, // 27
	{ "\xbc\xbc\xb9\xcc\xc7\xc1\xb7\xce" "C", 3117 }, // 28
	{ "\xbc\xbc\xb9\xcc\xc7\xc1\xb7\xce" "B", 3240 }, // 29
	{ "\xbc\xbc\xb9\xcc\xc7\xc1\xb7\xce" "A", 6480 }, // 30
	{ "\xc7\xc1\xb7\xce" "E", 4536 }, // 31
	{ "\xc7\xc1\xb7\xce" "D", 4684 }, // 32
	{ "\xc7\xc1\xb7\xce" "C", 4832 }, // 33
	{ "\xc7\xc1\xb7\xce" "B", 4980 }, // 34
	{ "\xc7\xc1\xb7\xce" "A", 14940 }, // 35
	{ "\xb3\xbb\xbc\xc5\xb3\xaf\xc7\xc1\xb7\xce" "E", 6972 }, // 36
	{ "\xb3\xbb\xbc\xc5\xb3\xaf\xc7\xc1\xb7\xce" "D", 7168 }, // 37
	{ "\xb3\xbb\xbc\xc5\xb3\xaf\xc7\xc1\xb7\xce" "C", 7364 }, // 38
	{ "\xb3\xbb\xbc\xc5\xb3\xaf\xc7\xc1\xb7\xce" "B", 7560 }, // 39
	{ "\xb3\xbb\xbc\xc5\xb3\xaf\xc7\xc1\xb7\xce" "A", 15120 }, // 40
	{ "\xbf\xf9\xb5\xe5\xc7\xc1\xb7\xce" "E", 9072 }, // 41
	{ "\xbf\xf9\xb5\xe5\xc7\xc1\xb7\xce" "D", 9322 }, // 42
	{ "\xbf\xf9\xb5\xe5\xc7\xc1\xb7\xce" "C", 9572 }, // 43
	{ "\xbf\xf9\xb5\xe5\xc7\xc1\xb7\xce" "B", 9822 }, // 44
	{ "\xbf\xf9\xb5\xe5\xc7\xc1\xb7\xce" "A", 19644 }, // 45
	{ "\xb8\xb6\xbd\xba\xc5\xcd" "E", 9822 }, // 46
	{ "\xb8\xb6\xbd\xba\xc5\xcd" "D", 10122 }, // 47
	{ "\xb8\xb6\xbd\xba\xc5\xcd" "C", 10422 }, // 48
	{ "\xb8\xb6\xbd\xba\xc5\xcd" "B", 10722 }, // 49
	{ "\xb8\xb6\xbd\xba\xc5\xcd" "A", 21444 }, // 50
	{ "\xc5\xbe\xb8\xb6\xbd\xba\xc5\xcd" "E", 10722 }, // 51
	{ "\xc5\xbe\xb8\xb6\xbd\xba\xc5\xcd" "D", 11122 }, // 52
	{ "\xc5\xbe\xb8\xb6\xbd\xba\xc5\xcd" "C", 11522 }, // 53
	{ "\xc5\xbe\xb8\xb6\xbd\xba\xc5\xcd" "B", 11922 }, // 54
	{ "\xc5\xbe\xb8\xb6\xbd\xba\xc5\xcd" "A", 23844 }, // 55
	{ "\xbf\xf9\xb5\xe5\xb8\xb6\xbd\xba\xc5\xcd" "E", 23844 }, // 56
	{ "\xbf\xf9\xb5\xe5\xb8\xb6\xbd\xba\xc5\xcd" "D", 24394 }, // 57
	{ "\xbf\xf9\xb5\xe5\xb8\xb6\xbd\xba\xc5\xcd" "C", 24944 }, // 58
	{ "\xbf\xf9\xb5\xe5\xb8\xb6\xbd\xba\xc5\xcd" "B", 25494 }, // 59
	{ "\xbf\xf9\xb5\xe5\xb8\xb6\xbd\xba\xc5\xcd" "A", 50988 }, // 60
	{ "\xb7\xb9\xc0\xfc\xb5\xe5" "E", 101978 }, // 61
	{ "\xb7\xb9\xc0\xfc\xb5\xe5" "D", 203952 }, // 62
	{ "\xb7\xb9\xc0\xfc\xb5\xe5" "C", 407904 }, // 63
	{ "\xb7\xb9\xc0\xfc\xb5\xe5" "B", 815808 }, // 64
	{ "\xb7\xb9\xc0\xfc\xb5\xe5" "A", 1631616 }, // 65
	{ "\xc0\xce\xc7\xc7\xb4\xcf\xc6\xbc\xb7\xb9\xc0\xfc\xb5\xe5" "E", 1631616 }, // 66
	{ "\xc0\xce\xc7\xc7\xb4\xcf\xc6\xbc\xb7\xb9\xc0\xfc\xb5\xe5" "D", 1631616 }, // 67
	{ "\xc0\xce\xc7\xc7\xb4\xcf\xc6\xbc\xb7\xb9\xc0\xfc\xb5\xe5" "C", 1631616 }, // 68
	{ "\xc0\xce\xc7\xc7\xb4\xcf\xc6\xbc\xb7\xb9\xc0\xfc\xb5\xe5" "B", 1631616 }, // 69
	{ "\xc0\xce\xc7\xc7\xb4\xcf\xc6\xbc\xb7\xb9\xc0\xfc\xb5\xe5" "A", 1631616 }, // 70
};

// The exception log written next to a ghost file and zipped into it.
namespace {
const char *const GHOST_EXCEPTION_LOG_FILENAME = "ghost_exception_log.txt";
}

// ghostdocument.cpp:27
// Copies the bits of a value into a variable of another type of the same
// size (float <-> int), without numeric conversion.
template <typename T, typename U>
void py_data_cast(T &src, U &dst) {
	dst = reinterpret_cast<const U &>(src);
}

// The callers are not decompiled yet; these instantiate the two conversions
// the original object emits.
template void py_data_cast<float, int>(float &src, int &dst);
template void py_data_cast<const int, float>(const int &src, float &dst);

// Function object: converts a legacy card into its ghost-file form.
struct card_converter {
	card_converter(void) {}
	GHOST_CARD_DATA operator()(const sSCardAvilityPeriodInfo &card) const;
};

// ghostdocument.cpp:34
GHOST_CARD_DATA card_converter::operator()(const sSCardAvilityPeriodInfo &card) const {
	GHOST_CARD_DATA ghostCard;
	memset(&ghostCard, 0, sizeof(ghostCard));
	type_convert(&card, &ghostCard);

	return ghostCard;
}

// ghostdocument.cpp:46
const char *GetCardTypeString(int type) {
	if (type == 0) return "CARD_CHARACTER";
	if (type == 1) return "CARD_CADDIE";
	if (type == 2) return "CARD_SPECIAL";
	if (type == 3) return "CARD_PACK";
	if (type == 4) return "CARD_BOX";

	return "NOT DEFINED";
}

// ghostdocument.cpp:61
const char *GetCardAbilityString(int ability) {
	if (ability == -1) return "SPECIAL_AVILITY_NONE";
	if (ability == 0) return "SPECIAL_EXP_ADD";
	if (ability == 1) return "SPECIAL_PANG_RATE_UP_TIME";
	if (ability == 2) return "SPECIAL_EXP_REAE_UP_TIME";
	if (ability == 3) return "SPECIAL_PANG_ADD";
	if (ability == 4) return "SPECIAL_POWER_UP_TIME";
	if (ability == 5) return "SPECIAL_CONTROL_UP_TIME";
	if (ability == 6) return "SPECIAL_ACCCURACY_UP_TIME";
	if (ability == 7) return "SPECIAL_SPIN_UP_TIME";
	if (ability == 8) return "SPECIAL_CURVE_UP_TIME";
	if (ability == 9) return "SPECIAL_START_COMBOGAUGE_UP_TIME";
	if (ability == 10) return "SPECIAL_SLOT_UP_TIME";
	if (ability == 11) return "SPECIAL_PANGYAZONE_UP";
	if (ability == 12) return "SPECIAL_CLEARBONUS_UP_SEPIAWIND";
	if (ability == 13) return "SPECIAL_CLEARBONUS_UP_WINDHILL";
	if (ability == 14) return "SPECIAL_CLEARBONUS_UP_PINKWIND";
	if (ability == 15) return "SPECIAL_CLEARBONUS_UP_BLUEMOON";
	if (ability == 16) return "SPECIAL_RANDOMPANG_ADD";
	if (ability == 17) return "SPECIAL_TREASUREPOINT_UP";
	if (ability == 18) return "SPECIAL_RAINFALL_PROBABILITY_UP";

	return "NOT DEFINED";
}

// Function object for std::for_each: writes one GHOST_CARD_DATA to a text
// file.
struct print_ghost_card_data {
	explicit print_ghost_card_data(FILE *fp);
	void operator()(const GHOST_CARD_DATA &card) const;

	FILE *m_fp; // 0x0
};
CHECK_SIZE(print_ghost_card_data, 0x4);
CHECK_FIELD(print_ghost_card_data, m_fp, 0x0);

// ghostdocument.cpp:89
const char *GetActionString(GHOST::eGHOSTACTMSG act) {
	if (act == GHOST::CHANGE_WIND) return "GHOST::CHANGE_WIND";
	if (act == GHOST::CHANGE_WEATHER) return "GHOST::CHANGE_WEATHER";
	if (act == GHOST::CHANGE_DIRECTION) return "GHOST::CHANGE_DIRECTION";
	if (act == GHOST::CHANGE_CLUB) return "GHOST::CHANGE_CLUB";
	if (act == GHOST::USE_ITEM) return "GHOST::USE_ITEM";
	if (act == GHOST::BALL_DROP) return "GHOST::BALL_DROP";
	if (act == GHOST::TIME_OUT) return "GHOST::TIME_OUT";
	if (act == GHOST::GAME_TIME_OVER) return "GHOST::GAME_TIME_OVER";
	if (act == GHOST::SAVE_SHOT_DATA) return "GHOST::SAVE_SHOT_DATA";

	return "NOT DEFINED";
}

// ghostdocument.cpp:107
print_ghost_card_data::print_ghost_card_data(FILE *fp) : m_fp(fp) {}

// ghostdocument.cpp:109
void print_ghost_card_data::operator()(const GHOST_CARD_DATA &card) const {
	if (card.partsTypeId) {
		fprintf(m_fp, "\tATTACHED : %d(%s), PARTS_ITEMID : %d\n", card.partsTypeId,
			ItemManager()->GetItemName(card.partsTypeId), card.partsId);
	}

	fprintf(m_fp, "\tCARD TYPEID : %d(%s)\n", card.typeId, ItemManager()->GetItemName(card.typeId));
	fprintf(m_fp, "\tCARD ITEMID : %d\n", card.id);
	fprintf(m_fp, "\tCARD TYPE : %d(%s)\n", card.kind, GetCardTypeString(card.kind));
	fprintf(m_fp, "\tCARD ABILITY : %d(%s)\n", card.effect, GetCardAbilityString(card.effect));
	fprintf(m_fp, "\tABILITY VALUE : %d\n", card.effectValue);
	fprintf(m_fp, "\tCARD SLOTNUM : %d\n", card.slot);

	fprintf(m_fp, "\n");
}

} // namespace

// ghostdocument.cpp:137
CGhostHoleData::CGhostHoleData(void) : m_holeNum(0) {
	m_pRecordingShot = new GHOST_SHOT_DATA;
	memset(m_pRecordingShot, 0, sizeof(GHOST_SHOT_DATA));
}

// ghostdocument.cpp:143
CGhostHoleData::~CGhostHoleData(void) {
	std::for_each(m_shotDataList.begin(), m_shotDataList.end(), Delete_Object());
	m_shotDataList.clear();

	if (m_pRecordingShot) { delete m_pRecordingShot; m_pRecordingShot = NULL; }
}

// ghostdocument.cpp:151
void CGhostHoleData::RecordAct(int type, int arg, int const *pParams, int count) {
	GHOST_ACTION &action = m_pRecordingShot->m_actions[type];

	action.m_changed = true; action.m_arg = (unsigned char)arg;
	memset(action.m_param, 0xFF, sizeof(action.m_param));

	if (count != 0 && pParams != NULL) {
		memcpy(action.m_param, pParams, sizeof(int) * (count < 1 ? 1 : (count > 5 ? 5 : count)));
	}
}

// ghostdocument.cpp:165
void CGhostHoleData::RecordDummyShot(unsigned int shotNum) {
	m_pRecordingShot->m_type = 1; m_pRecordingShot->m_shotNum = shotNum;
	m_pRecordingShot->m_holeNum = GOLFDOC()->m_currentHole;

	GHOST_SHOT_DATA *pShotData = new GHOST_SHOT_DATA;
	*pShotData = *m_pRecordingShot;
	m_shotDataList.push_back(pShotData);

	memset(m_pRecordingShot, 0, sizeof(GHOST_SHOT_DATA));
}

// ghostdocument.cpp:178
unsigned int CGhostHoleData::RollBackShotData(unsigned int count) {
	unsigned int remain = count;

	memset(m_pRecordingShot, 0, sizeof(GHOST_SHOT_DATA));

	std::list<GHOST_SHOT_DATA *>::reverse_iterator it = m_shotDataList.rbegin();
	while (it != m_shotDataList.rend() && remain != 0) {
		--remain;
		delete *it;

		it = std::list<GHOST_SHOT_DATA *>::reverse_iterator(m_shotDataList.erase((++it).base()));
	}

	return count - remain;
}

// ghostdocument.cpp:200
GHOST_SHOT_DATA const *CGhostHoleData::RecordShotData(sShotProtocol const *pProtocol, unsigned int shotNum) {
	m_pRecordingShot->m_type = 0; m_pRecordingShot->m_shotNum = shotNum;
	m_pRecordingShot->m_holeNum = GOLFDOC()->m_currentHole;

	type_convert(pProtocol, &m_pRecordingShot->m_protocol);

	if (GetCurMap() == 7) {
		// The special wind's start frame is recorded as the special wind
		// action.
		int startFrame = Wind().GetSpWindStartFrame();
		RecordAct(3, 2, &startFrame, 1);
	}

	m_pRecordingShot->m_windDirection = Wind().GetGlobalDirection();
	m_pRecordingShot->m_windPower = Wind().GetGlobalIntensity();
	m_pRecordingShot->m_globalWind[0] = Wind().GetGlobalWind().x;
	m_pRecordingShot->m_globalWind[1] = Wind().GetGlobalWind().y;
	m_pRecordingShot->m_globalWind[2] = Wind().GetGlobalWind().z;

	m_pRecordingShot->m_ballPos[0] = GolfBall().m_pos.x;
	m_pRecordingShot->m_ballPos[1] = GolfBall().m_pos.y;
	m_pRecordingShot->m_ballPos[2] = GolfBall().m_pos.z;

	int intensity = (int)Wind().GetGlobalIntensity();
	m_pRecordingShot->m_windPowerFraction = (unsigned char)((Wind().GetGlobalIntensity() - intensity) * 10.0f);

	m_pRecordingShot->m_cameraAngle = GOLFDOC()->GetCameraData().m_yaw;

	GHOST_SHOT_DATA *pShotData = new GHOST_SHOT_DATA;
	*pShotData = *m_pRecordingShot;
	m_shotDataList.push_back(pShotData);

	memset(m_pRecordingShot, 0, sizeof(GHOST_SHOT_DATA));
	return pShotData;
}

// ghostdocument.cpp:248
void CGhostHoleData::AddShotData(GHOST_SHOT_DATA const *pShotData) {
	GHOST_SHOT_DATA *pCopy = new GHOST_SHOT_DATA;
	*pCopy = *pShotData;
	m_shotDataList.push_back(pCopy);
}

// ghostdocument.cpp:264
CGhostDocument::CGhostDocument(void) : m_shotCount(0), m_bInvalidate(0) {
	m_pUserData = new GHOST_USER_DATA;
	memset(m_pUserData, 0, sizeof(GHOST_USER_DATA));

	m_pGameData = new GHOST_GAME_DATA;
	memset(m_pGameData, 0, sizeof(GHOST_GAME_DATA));

	m_pHoleData = new CGhostHoleData[18];
	for (int i = 0; i < 18; i++)
		m_pHoleData[i].SetHoleNum(i + 1);
}

// ghostdocument.cpp:277
CGhostDocument::~CGhostDocument(void) {
	if (m_pUserData) { delete m_pUserData; m_pUserData = NULL; }
	if (m_pGameData) { delete m_pGameData; m_pGameData = NULL; }
	if (m_pHoleData) { delete[] m_pHoleData; m_pHoleData = NULL; }
}

// ghostdocument.cpp:284
void CGhostDocument::RecordGameData(void) {
	m_pGameData->m_curMap = GetCurMap();
	m_pGameData->m_roomType = WSingleton<CSharedDoc>::Instance()->m_roomType;
	m_pGameData->m_gameKind = WSingleton<CSharedDoc>::Instance()->m_gameKind;
	m_pGameData->m_serverProperty = S5::UTIL::GetServerProperty();

	for (int i = 0; i < 18; i++) {
		m_pGameData->m_holeRandom[i] = WSingleton<CSharedDoc>::Instance()->m_holeRandom[i];
		m_pGameData->m_holeOrder[i] = WSingleton<CSharedDoc>::Instance()->m_holeOrder[i];
	}
}

// ghostdocument.cpp:298
void CGhostDocument::RecordUserData(void) {
	const sUserInfo &userInfo = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[MyIndex()];

	memset(m_pUserData, 0, sizeof(GHOST_USER_DATA));
	type_convert(&userInfo, m_pUserData);

	// Only the period cards still attached to an equipped part are recorded.
	const std::list<sSCardAvilityPeriodInfo> &periodList = WSingleton<CSharedDoc>::Instance()->m_cardPeriodList[MyIndex()];
	std::list<sSCardAvilityPeriodInfo> cards;
	std::copy(periodList.begin(), periodList.end(), std::back_inserter(cards));

	std::list<sSCardAvilityPeriodInfo>::iterator it = cards.begin();
	while (it != cards.end()) {
		if (it->kind == 0 || it->kind == 1) {
			bool attached = false;
			for (int i = 0; i < 24; i++) {
				if (it->partsTypeId == userInfo.m_characterInfo.partTypeIds[i] && it->partsId == userInfo.m_characterInfo.partIds[i]) {
					attached = true;
					break;
				}
			}

			if (!attached) {
				it = cards.erase(it);
				continue;
			}
		}

		++it;
	}

	if (cards.size() != 0) {
		m_cardList.reserve(cards.size());
		std::transform(cards.begin(), cards.end(), std::back_inserter(m_cardList), card_converter());
	}
}

// ghostdocument.cpp:359
void CGhostDocument::RecordHoleData(const MsgObject &msg) {
	CGhostHoleData *pHoleData = &m_pHoleData[GOLFDOC()->m_currentHole - 1];

	switch (msg.m_msg) {
	case GHOST::CHANGE_WEATHER:
#ifdef PG_PORT
		{ int pg_params[1] = {(int)msg.m_param1}; pHoleData->RecordAct(GHOST::CHANGE_WEATHER, 0, pg_params, 1); }
#else
		pHoleData->RecordAct(GHOST::CHANGE_WEATHER, 0, &msg.m_param1, 1);
#endif
		break;
	case GHOST::CHANGE_WIND:
#ifdef PG_PORT
		{ int pg_params[4] = {(int)msg.m_param1, (int)msg.m_param2, (int)msg.m_param3, (int)msg.m_param4}; pHoleData->RecordAct(GHOST::CHANGE_WIND, 0, pg_params, 4); }
#else
		pHoleData->RecordAct(GHOST::CHANGE_WIND, 0, &msg.m_param1, 4);
#endif
		break;
	case GHOST::CHANGE_DIRECTION:
#ifdef PG_PORT
		{ int pg_params[1] = {(int)msg.m_param1}; pHoleData->RecordAct(GHOST::CHANGE_DIRECTION, 1, pg_params, 1); }
#else
		pHoleData->RecordAct(GHOST::CHANGE_DIRECTION, 1, &msg.m_param1, 1);
#endif
		break;
	case GHOST::BALL_DROP: {
		WVector *pPos = (WVector *)msg.m_param1;
		int params[3];
		py_data_cast(pPos->x, params[0]);
		py_data_cast(pPos->y, params[1]);
		py_data_cast(pPos->z, params[2]);
		pHoleData->RecordAct(GHOST::BALL_DROP, 1, params, 3);
		pHoleData->RecordDummyShot(++m_shotCount);
		break;
	}
	case GHOST::USE_ITEM:
#ifdef PG_PORT
		{ int pg_params[2] = {(int)msg.m_param1, (int)msg.m_param2}; pHoleData->RecordAct(GHOST::USE_ITEM, 2, pg_params, 2); }
#else
		pHoleData->RecordAct(GHOST::USE_ITEM, 2, &msg.m_param1, 2);
#endif
		// Using this item takes the last shot back.
		if (msg.m_param1 == 0x1800000E) {
			pHoleData->RollBackShotData(1);
			m_shotCount--;
		}
		break;
	case 6: // the power shot action (no GHOST:: name known)
#ifdef PG_PORT
		{ int pg_params[1] = {(int)msg.m_param1}; pHoleData->RecordAct(6, 0, pg_params, 1); }
#else
		pHoleData->RecordAct(6, 0, &msg.m_param1, 1);
#endif
		break;
	case GHOST::SAVE_SHOT_DATA: {
		const sShotProtocol *pProtocol = (const sShotProtocol *)msg.m_param1;
		pHoleData->RecordShotData(pProtocol, ++m_shotCount);
		break;
	}
	case GHOST::TIME_OUT:
		pHoleData->RecordAct(GHOST::TIME_OUT, 0, NULL, 0);
		pHoleData->RecordDummyShot(++m_shotCount);
		break;
	}
}

// ghostdocument.cpp:437
void CGhostDocument::RecordException(const char *message) {
	if (message) {
		m_exceptionList.push_back(std::string(message));
	}
}

// ghostdocument.cpp:445
bool CGhostDocument::WriteTextFile(const char *fileName) const {
	if (strlen(m_pUserData->name) == 0) return false;

	FILE *fp;
	if (fileName == NULL) {
		fp = fopen(MakeTextFileName().c_str(), "wt");
	} else {
		fp = fopen(fileName, "wt");
	}

	if (fp == NULL) return false;

	SYSTEMTIME time;
	GetLocalTime(&time);
	fprintf(fp, "=================================================================================\n");
	fprintf(fp, "SAVE DATE      : %04d-%02d-%02d %02d:%02d:%02d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	fprintf(fp, "DATA VERSION   : %d\n", 1001);
	fprintf(fp, "HEADER SIZE    : %d\n", sizeof(GHOST_DATA_HEADER));
	fprintf(fp, "DATA SIZE      : GHOST_GAME_DATA(%d), GHOST_USER_DATA(%d), GHOST_SHOT_DATA(%d)\n", sizeof(GHOST_GAME_DATA), sizeof(GHOST_USER_DATA), sizeof(GHOST_SHOT_DATA));
	fprintf(fp, "---------------------------------------------------------------------------------\n");

	fprintf(fp, "USER NICK      : %s\n", m_pUserData->name);
	fprintf(fp, "USER ID        : %s\n", m_pUserData->nickname);
	fprintf(fp, "GUILD          : %s\n", strlen(m_pUserData->guildName) ? m_pUserData->guildName : "-");
	fprintf(fp, "EMBLEM         : %s\n", strlen(m_pUserData->guildMark) ? m_pUserData->guildMark : "-");

	fprintf(fp, "IDENTITY       : %d\n", m_pUserData->identity);
	fprintf(fp, "GENDER         : %s\n", m_pUserData->eventFlag1 ? "FEMALE" : "MALE");
	fprintf(fp, "ANGEL WING     : %d\n", m_pUserData->eventFlag3);
	fprintf(fp, "ANGEL EFFECT   : %d\n", m_pUserData->eventFlag4);
	fprintf(fp, "DEVIL WING     : %d\n", m_pUserData->eventFlag5);
	int level = m_pUserData->level;
	fprintf(fp, "LEVEL          : %d(%s)\n", level, LEVEL_TABLE[level < 0 ? 0 : (level > 70 ? 70 : level)].name);
	fprintf(fp, "---------------------------------------------------------------------------------\n");

	fprintf(fp, "CHAR TYPEID    : %d(%s)\n", m_pUserData->characterTypeId, ItemManager()->GetItemName(m_pUserData->characterTypeId));
	fprintf(fp, "HAIR COLOR     : %d\n", m_pUserData->hairColor);
	fprintf(fp, "SHIRTS COLOR   : %d\n", m_pUserData->shirtColor);
	for (int i = 0; i < 24; i++) {
		if (m_pUserData->partTypeIds[i]) {
			fprintf(fp, "PARTS(%02d)      : %d(%s)\n", i, m_pUserData->partTypeIds[i], ItemManager()->GetItemName(m_pUserData->partTypeIds[i]));
		}
	}

	for (int i = 0; i < 5; i++) {
		if (m_pUserData->auxPartTids[i]) {
			fprintf(fp, "AUXPARTS(%02d)   : %d(%s)\n", i, m_pUserData->auxPartTids[i], ItemManager()->GetItemName(m_pUserData->auxPartTids[i]));
		}
	}

	fprintf(fp, "CHAR PCL       : ");
	for (int i = 0; i < 5; i++)
		fprintf(fp, "%d ", m_pUserData->characterExtra[i]);
	fprintf(fp, "\n");

	fprintf(fp, "CLUB TYPEID    : %d(%s)\n", m_pUserData->clubSetTypeId, ItemManager()->GetItemName(m_pUserData->clubSetTypeId));
	fprintf(fp, "CLUB PCL       : ");
	for (int i = 0; i < 5; i++)
		fprintf(fp, "%d ", m_pUserData->clubStat[i]);
	fprintf(fp, "\n");

	fprintf(fp, "BALL TYPEID    : %d(%s)\n", m_pUserData->ballTypeId, ItemManager()->GetItemName(m_pUserData->ballTypeId));

	fprintf(fp, "CADDIE TYPEID  : %d(%s)\n", m_pUserData->caddieTypeId, m_pUserData->caddieTypeId ? ItemManager()->GetItemName(m_pUserData->caddieTypeId) : "NULL");
	fprintf(fp, "CADDIE PARTS   : %d(%s)\n", m_pUserData->cadItemTypeId, m_pUserData->cadItemTypeId ? ItemManager()->GetItemName(m_pUserData->cadItemTypeId) : "NULL");
	fprintf(fp, "CADDIE LEVEL   : %d\n", m_pUserData->caddieLevel);

	fprintf(fp, "MASCOT TYPEID  : %d(%s)\n", m_pUserData->mascotTypeId, m_pUserData->mascotTypeId ? ItemManager()->GetItemName(m_pUserData->mascotTypeId) : "NULL");
	fprintf(fp, "MASCOT MSG     : %s\n", strlen(m_pUserData->mascotMessage) ? m_pUserData->mascotMessage : "-");

	for (int i = 0; i < 10; i++) {
		if (m_pUserData->equipItem[i]) {
			fprintf(fp, "ITEMSLOT(%02d)   : %d(%s)\n", i, m_pUserData->equipItem[i], ItemManager()->GetItemName(m_pUserData->equipItem[i]));
		}
	}
	fprintf(fp, "---------------------------------------------------------------------------------\n");

	fprintf(fp, "CARD SIZE      : %d\n", m_cardList.size());
	std::for_each(m_cardList.begin(), m_cardList.end(), print_ghost_card_data(fp));
	fprintf(fp, "---------------------------------------------------------------------------------\n");

	unsigned char course = m_pGameData->m_curMap;
	fprintf(fp, "COURSE         : %s\n", ItemManager()->GetItemName(0x28000000 | ((course > 0x7F && course != 0xFD) ? course - 0x80 : course)));
	const char *gameType; switch (m_pGameData->m_roomType) { case 4: case 5: case 6: case 9: case 10: case 14: gameType = "30S_BATTLE"; break; default: gameType = "STROKE"; break; } fprintf(fp, "GAME TYPE      : %d(%s)\n", m_pGameData->m_roomType, gameType);
	fprintf(fp, "HOLE TYPE      : %d\n", m_pGameData->m_gameKind);
	fprintf(fp, "HOLE INDEX     : ");
	for (int i = 0; i < 18; i++)
		fprintf(fp, "%d ", m_pGameData->m_holeOrder[i]);
	fprintf(fp, "\n");
	fprintf(fp, "---------------------------------------------------------------------------------\n");

	for (int i = 0; i < 18; i++)
		WriteTextFile(fp, &m_pHoleData[i]);

	fclose(fp);

	return true;
}

// ghostdocument.cpp:575
bool CGhostDocument::WriteTextFile(FILE *fp, CGhostHoleData const *pHoleData) const {
	std::list<GHOST_SHOT_DATA *> const &shotDataList = pHoleData->GetShotDataList();
	std::list<GHOST_SHOT_DATA *>::const_iterator it = shotDataList.begin();
	while (it != shotDataList.end()) {
		WriteTextFile(fp, *it);
		++it;
	}

	return true;
}

// ghostdocument.cpp:587
bool CGhostDocument::WriteTextFile(FILE *fp, GHOST_SHOT_DATA const *pShotData) const {
	fprintf(fp, "HOLE INDEX     : %d\n", pShotData->m_holeNum);
	fprintf(fp, "SHOT INDEX     : %d\n", pShotData->m_shotNum);
	fprintf(fp, "DUMMY SHOT     : %s\n", pShotData->m_type ? "TRUE" : "FALSE");

	const GHOST_SHOT_PROTOCOL &protocol = pShotData->m_protocol;
	fprintf(fp, "MAX CLICK      : %.02f\n", protocol.power);
	fprintf(fp, "MIN CLICK      : %.02f\n", protocol.accuracy);
	fprintf(fp, "CURVE, SPIN    : %.02f, %.02f\n", protocol.lieSlope, protocol.lieAngle);
	fprintf(fp, "ACCURACY       : %d\n", protocol.accuracyGrade);
	fprintf(fp, "SPECIAL        : %d\n", protocol.powerFlags);
	fprintf(fp, "FRAME          : %d\n", protocol.frame);
	fprintf(fp, "SHOT ANGLE     : %.02f\n", protocol.angle);
	fprintf(fp, "SHOT TIME      : %d\n", protocol.shotElapsed);
	fprintf(fp, "CENTER         : %.02f\n", protocol.barStartPosition);
	fprintf(fp, "CLUB INDEX     : %d(%s)\n", protocol.club, GolfClub().GetClubString((CClub::eClub)protocol.club));
	fprintf(fp, "RANDOM POWER   : %.02f\n", protocol.randomPower);
	fprintf(fp, "RANDOM ANGLE   : %.02f\n", protocol.spot);
	fprintf(fp, "CAMERA ANGLE   : %.02f\n", pShotData->m_cameraAngle);
	fprintf(fp, "\n");

	for (int i = 0; i < 9; i++) {
		const GHOST_ACTION &action = pShotData->m_actions[i];

		if (action.m_changed) {
			fprintf(fp, "\tACTION : [%s] ", GetActionString((GHOST::eGHOSTACTMSG)i));

			for (int j = 0; j < 5; j++) {
				if (action.m_param[j].m_dw != 0xFFFFFFFF) {
					if (action.m_arg == 1) {
						fprintf(fp, "%.02f ", action.m_param[j].m_f);
					} else {
						fprintf(fp, "%d ", action.m_param[j].m_dw);
					}
				}
			}

			fprintf(fp, "\n");
		}
	}

	fprintf(fp, "\n");
	fprintf(fp, "\t\tBALL POSITION : %.02f, %.02f, %.02f\n", pShotData->m_ballPos[0], pShotData->m_ballPos[1], pShotData->m_ballPos[2]);
	fprintf(fp, "\t\tWIND DIR : %.02f\n", pShotData->m_windDirection);
	fprintf(fp, "\t\tWIND PWR : %.02f\n", pShotData->m_windPower);
	fprintf(fp, "\t\tGLOBAL WIND : %.02f, %.02f, %.02f\n", pShotData->m_globalWind[0], pShotData->m_globalWind[1], pShotData->m_globalWind[2]);
	fprintf(fp, "---------------------------------------------------------------------------------\n");

	return true;
}

// ghostdocument.cpp:647
std::string CGhostDocument::MakeTextFileName(void) const {
	char fileName[MAX_PATH] = { 0 };
	sprintf(fileName, "ghost_save_data(%s).txt", MyNick());

	return fileName;
}

// ghostdocument.cpp:655
CGhostHoleData const *CGhostDocument::GetHoleData(unsigned char holeNum) const {
	CGhostHoleData const *pHoleData = NULL;
	if (m_pHoleData) {
		pHoleData = &m_pHoleData[holeNum - 1];
	}

	return pHoleData;
}

// A growable byte buffer: encode() appends at m_length, decode() reads from
// m_position.
class memory_buffer {
	LAYOUT_CHECK_FRIEND;
	friend class CGhostDocument; // Import reads the buffer in place
public:
	explicit memory_buffer(unsigned int capacity);
	memory_buffer(char *buffer, unsigned int length);
	~memory_buffer(void);

	unsigned int reserve(unsigned int capacity);
	void encode(void const *data, unsigned int size);
	void decode(void *data, unsigned int size);
	void const *get_buffer(void) const;
	unsigned int length(void) const;

private:
	char *m_buffer;          // 0x0
	unsigned int m_length;   // 0x4
	unsigned int m_position; // 0x8
	unsigned int m_capacity; // 0xC
};
CHECK_SIZE(memory_buffer, 0x10);
BEGIN_LAYOUT_CHECK(memory_buffer)
	CHECK_FIELD(memory_buffer, m_buffer, 0x0);
	CHECK_FIELD(memory_buffer, m_length, 0x4);
	CHECK_FIELD(memory_buffer, m_position, 0x8);
	CHECK_FIELD(memory_buffer, m_capacity, 0xC);
END_LAYOUT_CHECK

// ghostdocument.cpp:669
memory_buffer::memory_buffer(unsigned int capacity) : m_buffer(0), m_length(0), m_position(0) {
	m_buffer = (char *)calloc(m_capacity = capacity, 1);
}

// ghostdocument.cpp:674
memory_buffer::memory_buffer(char *buffer, unsigned int length)
	: m_buffer(0), m_length(0), m_position(0) {
	m_buffer = (char *)calloc(m_capacity = length, 1);
	encode(buffer, length);
}

// ghostdocument.cpp:681
memory_buffer::~memory_buffer(void) {
	if (m_buffer) free(m_buffer);
}

// ghostdocument.cpp:686
unsigned int memory_buffer::reserve(unsigned int capacity) {
	m_capacity = capacity;
	if (m_buffer == 0) {
		m_buffer = (char *)calloc(capacity, 1);
	} else {
		m_buffer = (char *)realloc(m_buffer, capacity);
	}

	return m_capacity;
}

// ghostdocument.cpp:698
void memory_buffer::encode(void const *data, unsigned int size) {
	if (m_length + size > m_capacity) {
		for (m_capacity = 1; m_capacity < m_length + size; m_capacity <<= 1) {
		}
		reserve(m_capacity);
	}

	memcpy(m_buffer + m_length, data, size);
	m_length += size;
}

// ghostdocument.cpp:708
void memory_buffer::decode(void *data, unsigned int size) {
	memcpy(data, m_buffer + m_position, size);
	m_position += size;
}

// ghostdocument.cpp:714
void const *memory_buffer::get_buffer(void) const { return m_buffer; }

// ghostdocument.cpp:715
unsigned int memory_buffer::length(void) const { return m_length; }

// ghostdocument.cpp:730
void CGhostDocument::Export(const std::string &fileName) {
	GHOST_DATA_HEADER header = { 0 };
	header.m_version = 1001;
	header.m_userDataSize = sizeof(GHOST_USER_DATA);
	header.m_gameDataSize = sizeof(GHOST_GAME_DATA);
	header.m_shotDataSize = sizeof(GHOST_SHOT_DATA);
	header.m_cardDataSize = sizeof(GHOST_CARD_DATA);

	header.m_cardCount = m_cardList.size();

	for (int hole = 0; hole < 18; hole++) {
		header.m_shotCount += m_pHoleData[hole].GetShotDataList().size();
	}

	unsigned int size = sizeof(GHOST_DATA_HEADER) + header.GetDataBodySize();
	void *pUnused = calloc(size, 1); // never used nor freed by the original

	memory_buffer buffer(size);

	int version = 1001;
	buffer.encode(&version, sizeof(version));

	int headerSize = sizeof(GHOST_DATA_HEADER);
	buffer.encode(&headerSize, sizeof(headerSize));
	buffer.encode(&header, headerSize);

	buffer.encode(m_pUserData, sizeof(GHOST_USER_DATA));
	buffer.encode(m_pGameData, sizeof(GHOST_GAME_DATA));

	for (int i = 0; i < 18; i++) {
		const std::list<GHOST_SHOT_DATA *> &shotDataList = m_pHoleData[i].GetShotDataList();
		unsigned int shotCount = shotDataList.size();
		buffer.encode(&shotCount, sizeof(shotCount));

		std::list<GHOST_SHOT_DATA *>::const_iterator it = shotDataList.begin();
		while (it != shotDataList.end()) {
			buffer.encode(*it, sizeof(GHOST_SHOT_DATA));
			++it;
		}
	}

	unsigned int cardCount = m_cardList.size();
	buffer.encode(&cardCount, sizeof(cardCount));
	if (cardCount != 0) {
		buffer.encode(&m_cardList[0], m_cardList.size() * sizeof(GHOST_CARD_DATA));
	}

	HZIP hZip = CreateZipZ((void *)fileName.c_str(), 0, 2, 0);
	CreateDirectory(".\\temporary_save\\", NULL);

	ZipAdd(hZip, "ghost_document_data", (void *)buffer.get_buffer(), buffer.length(), 3);

	std::string logFileName(".\\temporary_save\\");
	logFileName += GHOST_EXCEPTION_LOG_FILENAME;
	if (_access(logFileName.c_str(), 0) != -1) {
		ZipAdd(hZip, GHOST_EXCEPTION_LOG_FILENAME, (void *)logFileName.c_str(), 0, 2);
	}

	IsZipHandleU(hZip) ? CloseZipU(hZip) : CloseZipZ(hZip);

	DeleteFile(logFileName.c_str());
	RemoveDirectory(".\\temporary_save\\");
}

// ghostdocument.cpp:835
bool CGhostDocument::Import(const std::string &fileName) {
	cFile *pFile = GetCFile(fileName.c_str(), -1, 0);
	if (pFile == NULL)
		return false;

	unsigned int size = pFile->Length();
	unsigned char *pData = new unsigned char[size];
	pFile->Read(pData, size);
	CloseCFile(pFile);

	bool result = Import(pData, size);
	delete[] pData;

	return result;
}

// ghostdocument.cpp:852
bool CGhostDocument::Import(unsigned char *data, unsigned int size) {
	if (data == NULL || size == 0) return false;

	int index;
	ZIPENTRY entry;

	index = 0; HZIP hZip = OpenZipU(data, size, 3);
	if (FindZipItemA(hZip, "ghost_document_data", true, &index, &entry) != 0) {
		IsZipHandleU(hZip) ? CloseZipU(hZip) : CloseZipZ(hZip);
		return false;
	}

	char *pData = new char[entry.unc_size];
	UnzipItem(hZip, index, pData, entry.unc_size, 3);
	IsZipHandleU(hZip) ? CloseZipU(hZip) : CloseZipZ(hZip);

	memory_buffer buffer(pData, entry.unc_size);
	delete[] pData;

	int version = *(int *)(buffer.m_buffer + buffer.m_position); buffer.m_position += sizeof(int);

	if (version != 1001) {
		return false;
	}

	unsigned int headerSize = *(unsigned int *)(buffer.m_buffer + buffer.m_position); buffer.m_position += sizeof(unsigned int);

	GHOST_DATA_HEADER header;
	memset(&header, 0, sizeof(header)); memcpy(&header, buffer.m_buffer + buffer.m_position, headerSize); buffer.m_position += headerSize;

	buffer.decode(m_pUserData, header.m_userDataSize);
	buffer.decode(m_pGameData, header.m_gameDataSize);

	for (int hole = 0; hole < 18; hole++) {
		unsigned int shotCount = *(unsigned int *)(buffer.m_buffer + buffer.m_position); buffer.m_position += sizeof(unsigned int);

		for (unsigned int i = 0; i < shotCount; i++) {
			GHOST_SHOT_DATA shotData; memset(&shotData, 0, sizeof(shotData)); memcpy(&shotData, buffer.m_buffer + buffer.m_position, header.m_shotDataSize); buffer.m_position += header.m_shotDataSize;
			m_pHoleData[hole].AddShotData(&shotData);
		}
	}

	unsigned int cardCount = *(unsigned int *)(buffer.m_buffer + buffer.m_position); buffer.m_position += sizeof(unsigned int);

	if (cardCount != 0) {
		m_cardList.reserve(cardCount);

		for (unsigned int i = 0; i < cardCount; i++) {
			GHOST_CARD_DATA card; memset(&card, 0, sizeof(card)); memcpy(&card, buffer.m_buffer + buffer.m_position, header.m_cardDataSize); buffer.m_position += header.m_cardDataSize;
			m_cardList.push_back(card);
		}
	}

	return true;
}

// ghostdocument.cpp:940
bool CGhostDocument::WriteExceptionLogFile(void) const {
	return true;
}
