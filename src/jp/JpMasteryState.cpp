// The JP 983 mastery stat rules (JpMasteryStatInfo, JpMasterySlot.h) and the
// game state they read, from the JP exe's bodies (node scripts/disasm.ts
// --jp 0x<va>), in address order.
//
// The stat object holds nothing but its vptr (0xd12c0c): every rule reads
// the JP CSharedDoc (0xe47dbc). The player's level is its +0x91d (the KR
// CSharedDoc::m_myLevel, filled by the login's player info); the stat
// helpers 0x5e3f60-0x5e4930 read the selected character's record in the
// character map +0x3704 (KR m_characterMap, keyed by +0xa2a, the KR
// m_myCharacterId): the KR sCharacterInfo of the character list packet,
// with the mastery points at +0x1c8 (5 bytes, one per stat), the mastery
// value at +0x1cd and the 12 card slots at +0x1d1.
#include "JpMasterySlot.h"
#include "JpItemGlue.h"
#include "JpMasteryState.h"
#include "JpMasteryCard.h"
#include "JpMasteryMainDlg.h"

#include "SharedDoc.h"

// 0x5e3430
JpMasteryStatInfo::JpMasteryStatInfo() {
}

// 0x5e3440
JpMasteryStatInfo::~JpMasteryStatInfo() {
}

// lib: 0x5e3710 (the scalar deleting destructor)

// 0x5e3850
int JpMasteryStatInfo::GetCharacterBaseStatFor(uint32_t stat, uint32_t characterTid) {
	if (stat <= 4) {
		const JpIffCharacter* data = JpIff_FindCharacter(characterTid);
		if (data != NULL) {
			return JpIffCharacter_GetStat(data, stat);
		}
	}
	return 0;
}

// 0x5e3890
int JpMasteryStatInfo::GetEquipmentStatFor(uint32_t stat, const uint32_t* partTids, const uint32_t* auxPartTids, uint32_t clubTid, uint32_t caddieTid, uint32_t mascotTid) {
	if (stat > 4) {
		return 0;
	}
	int points = 0;
	const JpIffCaddie* caddie = JpIff_FindCaddie(caddieTid);
	const JpIffMascot* mascot = JpIff_FindMascot(mascotTid);
	const JpIffClubSet* clubSet = reinterpret_cast<const JpIffClubSet*>(JpItemManager_FindClubSet(clubTid));
	if (clubSet != NULL) {
		points = Jp_GetClubSetStat(stat, clubSet, NULL, NULL);
	}
	points += JpIff_GetItemLevelStat(stat, partTids, auxPartTids);
	points += Jp_GetCaddieStat(stat, caddie, Jp_GameState_MyLevel());
	return Jp_GetMascotStat(stat, mascot, Jp_GameState_MyLevel()) + points;
}

// 0x5e3970
int JpMasteryStatInfo::GetPenaltyValue() {
	return Jp_GetLevelClass(Jp_GameState_MyLevel()) + 0x14;
}

// 0x5e39d0
int JpMasteryStatInfo::GetOpenSlotCount(uint32_t stat, int level) {
	int count = 0;
	if (stat <= 4) {
		if (level == -1) {
			level = Jp_GameState_MyLevel();
		}
		if (stat == 0 && level >= 6) {
			count = (level - 1) / 5;
		}
	}
	return count;
}

// 0x5e3c00
int JpMasteryStatInfo::GetMasteryPointsOfType(uint32_t stat, uint32_t characterTid) {
	if (stat > 4) {
		return 0;
	}
	for (const sCharacterInfo* character = Jp_GameState_FirstCharacter(); character != NULL;
			character = Jp_GameState_NextCharacter(character)) {
		if (static_cast<uint32_t>(character->typeId) == characterTid) {
			return JpCharacter_GetMasteryPoints(character, stat);
		}
	}
	return 0;
}

// 0x5e3cc0
int JpMasteryStatInfo::GetStatValueFor(uint32_t stat, uint32_t characterTid, const uint32_t* partTids, const uint32_t* auxPartTids, uint32_t clubTid, uint32_t caddieTid, uint32_t mascotTid) {
	if (stat > 4) {
		return 0;
	}
	int value = GetCharacterBaseStatFor(stat, characterTid);
	value += GetEquipmentStatFor(stat, partTids, auxPartTids, clubTid, caddieTid, mascotTid);
	value += GetMasteryPointsOfType(stat, characterTid);
	return value - GetPowerPenaltyFor(stat, characterTid, partTids, auxPartTids, clubTid, caddieTid, mascotTid);
}

// 0x5e3d40
int JpMasteryStatInfo::GetPowerPenaltyFor(uint32_t stat, uint32_t characterTid, const uint32_t* partTids, const uint32_t* auxPartTids, uint32_t clubTid, uint32_t caddieTid, uint32_t mascotTid) {
	int penalty = 0;
	if (stat <= 4 && (stat == 1 || stat == 2)) {
		int over = GetStatValueFor(0, characterTid, partTids, auxPartTids, clubTid, caddieTid, mascotTid) - GetPenaltyValue();
		penalty = over > 0 ? over : 0;
	}
	return penalty;
}

// 0x5e3f60
int JpMasteryStatInfo::GetCharacterBaseStat(uint32_t stat) {
	if (stat > 4) {
		return 0;
	}
	const sCharacterInfo* character = Jp_GameState_EquippedCharacterData();
	if (character != NULL) {
		const JpIffCharacter* data = JpIff_FindCharacter(static_cast<uint32_t>(character->typeId));
		if (data != NULL) {
			return JpIffCharacter_GetStat(data, stat);
		}
	}
	return 0;
}

// 0x5e3fc0
int JpMasteryStatInfo::GetEquipmentStat(uint32_t stat) {
	if (stat > 4) {
		return 0;
	}
	int points = 0;
	const sCharacterInfo* character = Jp_GameState_EquippedCharacterData();
	const JpCaddieData* caddieData = Jp_GameState_EquippedCaddie();
	const JpIffCaddie* caddie = NULL;
	if (caddieData != NULL) {
		caddie = JpIff_FindCaddie(JpCaddieData_GetTypeId(caddieData));
	}
	const JpMascotData* mascotData = Jp_GameState_EquippedMascot();
	const JpIffMascot* mascot = NULL;
	if (mascotData != NULL) {
		mascot = JpIff_FindMascot(JpMascotData_GetTypeId(mascotData));
	}
	const JpItemData* club = Jp_GameState_EquippedClub();
	if (club != NULL) {
		const JpIffClubSet* clubSet = reinterpret_cast<const JpIffClubSet*>(JpItemManager_FindClubSet(JpItemData_GetTypeId(club)));
		points = Jp_GetClubSetStat(stat, clubSet, JpItemData_GetClubUpgrades(club), JpItemData_GetClubExtra(club));
	}
	points += JpIff_GetItemLevelStat(stat, JpCharacter_GetPartTypeIds(character), JpCharacter_GetAuxPartTypeIds(character));
	points += Jp_GetCaddieStat(stat, caddie, Jp_GameState_MyLevel());
	points += Jp_GetMascotStat(stat, mascot, Jp_GameState_MyLevel());
	int bonus = JpCardManager_GetStatBonus(stat);
	return JpCardManager_GetEffectStatBonus(stat) + points + bonus;
}

// 0x5e4180
int JpMasteryStatInfo::GetPartsStatFor(uint32_t stat, const uint32_t* partTids) {
	if (stat > 4) {
		return 0;
	}
	int points = 0;
	const sCharacterInfo* character = Jp_GameState_EquippedCharacterData();
	if (partTids == NULL) {
		partTids = JpCharacter_GetPartTypeIds(character);
	}
	for (int i = 0; i < 24; i++) {
		if (partTids[i] != 0) {
			const JpIffPart* part = reinterpret_cast<const JpIffPart*>(JpItemManager_FindPart(partTids[i]));
			if (part != NULL) {
				points += JpIffPart_GetStat(part, stat);
			}
		}
	}
	const uint32_t* auxPartTids = JpCharacter_GetAuxPartTypeIds(character);
	for (int i = 0; i < 5; i++) {
		if (auxPartTids[i] != 0) {
			const JpIffAuxPart* auxPart = JpIff_FindAuxPart(auxPartTids[i]);
			if (auxPart != NULL) {
				points += JpIffAuxPart_GetStat(auxPart, stat);
			}
		}
	}
	return JpIff_GetPartSetStat(stat, partTids, auxPartTids) + points;
}

// 0x5e4260
int JpMasteryStatInfo::GetPartsStat(uint32_t stat) {
	if (stat > 4) {
		return 0;
	}
	int points = 0;
	const sCharacterInfo* character = Jp_GameState_EquippedCharacterData();
	const uint32_t* partTids = JpCharacter_GetPartTypeIds(character);
	for (int i = 0; i < 24; i++) {
		if (partTids[i] != 0) {
			const JpIffPart* part = reinterpret_cast<const JpIffPart*>(JpItemManager_FindPart(partTids[i]));
			if (part != NULL) {
				points += JpIffPart_GetStat(part, stat);
			}
		}
	}
	const uint32_t* auxPartTids = JpCharacter_GetAuxPartTypeIds(character);
	for (int i = 0; i < 5; i++) {
		if (auxPartTids[i] != 0) {
			const JpIffAuxPart* auxPart = JpIff_FindAuxPart(auxPartTids[i]);
			if (auxPart != NULL) {
				points += JpIffAuxPart_GetStat(auxPart, stat);
			}
		}
	}
	return JpIff_GetPartSetStat(stat, partTids, auxPartTids) + points;
}

// 0x5e4340
int JpMasteryStatInfo::GetCardStat(uint32_t stat) {
	if (stat > 4) {
		return 0;
	}
	int points = 0;
	const sCharacterInfo* character = Jp_GameState_EquippedCharacterData();
	for (int i = 0; i < 12; i++) {
		const JpCardData* card = Jp_FindCard(JpCharacterData_GetCardTypeId(character, i));
		if (card != NULL) {
			points += JpCardData_GetStat(card, stat);
		}
	}
	return points;
}

// 0x5e43c0 (the selection is a std::for_each functor, 0x5e3dd0)
void JpMasteryStatInfo::GetSlotRecords(std::vector<const JpMasterySlotRecord*>& records, uint32_t characterTid, int32_t masterySlot) {
	records.clear();
	if (characterTid == 0) {
		const sCharacterInfo* character = Jp_GameState_EquippedCharacterData();
		if (character != NULL) {
			characterTid = static_cast<uint32_t>(character->typeId);
		}
	}
	const std::vector<JpMasterySlotRecord>& table = Jp_GameState_MasterySlotRecords();
	for (std::vector<JpMasterySlotRecord>::const_iterator it = table.begin(); it != table.end(); ++it) {
		if (characterTid == it->characterTid && (masterySlot < 0 || masterySlot >= it->slotNumber)) {
			records.push_back(&*it);
		}
	}
}

// 0x5e44c0 (the count is a std::for_each functor, 0x5e3a20)
int JpMasteryStatInfo::GetSlotPoints(uint32_t stat, const sCharacterInfo* character) {
	if (stat > 4) {
		return 0;
	}
	int count = 0;
	if (character == NULL) {
		character = Jp_GameState_EquippedCharacterData();
	}
	std::vector<const JpMasterySlotRecord*> records;
	GetSlotRecords(records, static_cast<uint32_t>(character->typeId), JpCharacter_GetMasterySlotNumber(character));
	for (std::vector<const JpMasterySlotRecord*>::const_iterator it = records.begin(); it != records.end(); ++it) {
		if (stat == static_cast<uint32_t>((*it)->stat - 1)) {
			count++;
		}
	}
	return count;
}

// 0x5e45c0 (the count is a std::for_each functor, 0x5e3a70)
int JpMasteryStatInfo::GetSlotPointsFor(uint32_t stat, uint32_t characterTid, int32_t masterySlot) {
	if (stat > 4) {
		return 0;
	}
	int count = 0;
	std::vector<const JpMasterySlotRecord*> records;
	GetSlotRecords(records, characterTid, masterySlot);
	for (std::vector<const JpMasterySlotRecord*>::const_iterator it = records.begin(); it != records.end(); ++it) {
		if (stat == static_cast<uint32_t>((*it)->stat - 1)) {
			count++;
		}
	}
	return count;
}

// 0x5e4690
int JpMasteryStatInfo::GetMaxMasteryPoints(uint32_t stat) {
	if (stat > 4) {
		return 0;
	}
	int points = GetSlotPoints(stat, NULL);
	points += GetPartsStat(stat);
	points += GetCardStat(stat);
	return GetOpenSlotCount(stat, -1) + points;
}

// 0x5e46e0
int JpMasteryStatInfo::GetMaxMasteryPointsFor(uint32_t stat, const uint32_t* partTids, uint32_t characterTid, int32_t masterySlot) {
	if (stat > 4) {
		return 0;
	}
	int points = GetSlotPointsFor(stat, characterTid, masterySlot);
	points += GetPartsStatFor(stat, partTids);
	points += GetCardStat(stat);
	return GetOpenSlotCount(stat, -1) + points;
}

// 0x5e4740
int JpMasteryStatInfo::GetMasteryPoints(uint32_t stat, bool raw, const sCharacterInfo* character) {
	if (stat > 4) {
		return 0;
	}
	int points = 0;
	if (character == NULL) {
		character = Jp_GameState_EquippedCharacterData();
		if (character == NULL) {
			return points;
		}
	}
	if (raw) {
		return JpCharacter_GetMasteryPoints(character, stat);
	}
	int limit = GetMaxMasteryPointsFor(stat, JpCharacter_GetPartTypeIds(character),
		static_cast<uint32_t>(character->typeId), JpCharacter_GetMasterySlotNumber(character));
	int value = JpCharacter_GetMasteryPoints(character, stat);
	if (value <= limit) {
		limit = value;
	}
	return limit;
}

// 0x5e47e0
int JpMasteryStatInfo::GetExcessMasteryPoints(uint32_t stat) {
	if (stat > 4) {
		return 0;
	}
	const sCharacterInfo* character = Jp_GameState_EquippedCharacterData();
	if (character == NULL) {
		return 0;
	}
	int limit = GetMaxMasteryPoints(stat);
	int excess = JpCharacter_GetMasteryPoints(character, stat) - limit;
	return excess > 0 ? excess : 0;
}

// 0x5e48a0
int JpMasteryStatInfo::GetFreeMasteryPoints(uint32_t stat) {
	if (stat > 4) {
		return 0;
	}
	int points = GetMasteryPoints(stat, true, NULL);
	int left = GetMaxMasteryPoints(stat) - points;
	return left > 0 ? left : 0;
}

// 0x5e48e0
int JpMasteryStatInfo::GetStatValue(uint32_t stat) {
	if (stat > 4) {
		return 0;
	}
	int value = GetCharacterBaseStat(stat);
	value += GetEquipmentStat(stat);
	value += GetMasteryPoints(stat, false, NULL);
	return value - GetPowerPenalty(stat);
}

// 0x5e4930
int JpMasteryStatInfo::GetPowerPenalty(uint32_t stat) {
	int penalty = 0;
	if (stat <= 4 && (stat == 1 || stat == 2)) {
		int over = GetStatValue(0) - GetPenaltyValue();
		penalty = over > 0 ? over : 0;
	}
	return penalty;
}

// 0x9d9c50
int Jp_GetLevelClass(int level) {
	if (level >= 0x42) {
		return 13;
	}
	if (level >= 0x3d) {
		return 12;
	}
	if (level >= 0x38) {
		return 11;
	}
	if (level >= 0x33) {
		return 10;
	}
	if (level >= 0x2e) {
		return 9;
	}
	if (level >= 0x29) {
		return 8;
	}
	if (level >= 0x24) {
		return 7;
	}
	if (level >= 0x1f) {
		return 6;
	}
	if (level >= 0x1a) {
		return 5;
	}
	if (level >= 0x15) {
		return 4;
	}
	if (level >= 0x10) {
		return 3;
	}
	if (level >= 0xb) {
		return 2;
	}
	return level >= 6;
}

// JP CSharedDoc (0xe47dbc) +0x91d: the KR CSharedDoc::m_myLevel (R538 map,
// JP BuildMyPartTidList 0x632df0). Read inline by every JP caller (0x5e3970,
// 0x5e39d0, 0x5e4109): no function of its own.
uint8_t Jp_GameState_MyLevel() {
	return WSingleton<CSharedDoc>::Instance()->m_myLevel;
}
