// Original: d:\build\custom\temp\code\client\projectg\treasure.cpp

#include <algorithm>
#include <map>
#include "Treasure.h"
#include "SharedDoc.h"

// treasure.cpp:14
CTMapInfo::CTMapInfo() : m_TMapIndex(0), m_TMark(2), m_Gauge(0) {
}

// treasure.cpp:19
CTMapInfo::CTMapInfo(const CTMapInfo& other) {
	m_TMapIndex = other.m_TMapIndex;
	m_TMark = other.m_TMark;
	m_Gauge = other.m_Gauge;
}

// treasure.cpp:26
CTMapInfo::~CTMapInfo() {
}

// treasure.cpp:31
unsigned char CTMapInfo::GetTMapIndex() const {
	return m_TMapIndex;
}

// treasure.cpp:36
void CTMapInfo::SetTMapIndex(unsigned char index) {
	m_TMapIndex = index;
}

// treasure.cpp:41
unsigned char CTMapInfo::GetTMark() const {
	return m_TMark;
}

// treasure.cpp:46
void CTMapInfo::SetTMark(unsigned char mark) {
	m_TMark = mark;
}

// treasure.cpp:51
unsigned long CTMapInfo::GetGauge() const {
	return m_Gauge;
}

// treasure.cpp:56
void CTMapInfo::SetGauge(unsigned long gauge) {
	m_Gauge = gauge;
}

// treasure.cpp:61
CTMapInfo& CTMapInfo::operator=(const CTMapInfo& other) {
	m_TMapIndex = other.m_TMapIndex;
	m_TMark = other.m_TMark;
	m_Gauge = other.m_Gauge;

	return *this;
}

// treasure.cpp:78
CTHunter::CTHunter() {
	m_TreasurePoint = 0;
	m_bReturnLobby = false;
	m_bReceiveGiftPacket = false;
	m_usedTikiReport = false;
	m_TreasureMapSize = 24;
}

// treasure.cpp:83
CTHunter::~CTHunter() {
	std::vector<CTMapInfo*>::iterator it;
	for (it = m_TMapList.begin(); it != m_TMapList.end(); ++it) {
		if (*it) {
			delete *it;
			*it = 0;
		}
	}
	PG_VECTOR_FREE_STORAGE(m_TMapList);
}

// treasure.cpp:93
void CTHunter::Initialize(int count) {
	for (int i = 0; i < count; i++) {
		AddTreasure(i, 0);
	}

	m_GiftList.reserve(m_TreasureMapSize);
}

// treasure.cpp:108
void CTHunter::AddTreasure(unsigned char index, unsigned long gauge) {
	CTMapInfo* info = new CTMapInfo;

	info->SetTMapIndex(index);
	info->SetGauge(gauge);
	m_TMapList.push_back(info);
}

// treasure.cpp:118
bool CTHunter::DeleteTreasure(unsigned char index) {
	std::vector<CTMapInfo*>::iterator it = m_TMapList.begin();
	while (it != m_TMapList.end()) {
		if (index == (*it)->GetTMapIndex()) {
			delete *it;
			m_TMapList.erase(it);
			return true;
		}
		++it;
	}

	return false;
}

// treasure.cpp:138
unsigned char CTHunter::FindTreasure(unsigned char index) {
	std::vector<CTMapInfo*>::iterator it = std::find_if(m_TMapList.begin(), m_TMapList.end(), THunterFinder(index));

	return (*it)->GetTMapIndex();
}

// treasure.cpp:145
void CTHunter::SetTHunterGauge(unsigned char index, unsigned long gauge) {
	if (m_TMapList.size() > index) {
		m_TMapList[index]->SetGauge(gauge);
	}
}

// treasure.cpp:156
unsigned long CTHunter::GetTHunterGauge(eMapType type) {
	CMapTypeAdapter::eMapIndex index = CMapTypeAdapter::GetMapIndex(type);
	if (index == -1) {
		return 9999;
	}
#ifdef JP_PAKS
	// JP data: the JP courses' slots (17, 18, 20, 21) may be past the
	// list's 20 entries (ProjectG.cpp's Initialize(20)): no gauge.
	if ((unsigned int)index >= m_TMapList.size()) {
		return 9999;
	}
#endif

	return m_TMapList[index]->GetGauge();
}

// treasure.cpp:165
unsigned char CTHunter::GetTHunterMark(eMapType type) {
	CMapTypeAdapter::eMapIndex index = CMapTypeAdapter::GetMapIndex(type);
	if (index == -1) {
		return 2;
	}
#ifdef JP_PAKS
	// JP data: as GetTHunterGauge, no mark past the list.
	if ((unsigned int)index >= m_TMapList.size()) {
		return 2;
	}
#endif

	return m_TMapList[index]->GetTMark();
}

// treasure.cpp:175
unsigned char CTHunter::GetTreasureMapSize() {
	unsigned char size = (unsigned char)m_TMapList.size();

	return size;
}

// treasure.cpp:181
void CTHunter::SetTreasurePoint(unsigned long point) {
	if (point >= 1000) {
		m_TreasurePoint = 1000;
	} else {
		m_TreasurePoint = point;
	}
}

// treasure.cpp:192
unsigned long CTHunter::GetTreasurePoint() const {
	return m_TreasurePoint;
}

// treasure.cpp:197
void CTHunter::ResetTGift() {
	PG_VECTOR_FREE_STORAGE(m_GiftList);
}

// treasure.cpp:202
void CTHunter::DecideTMark() {
	unsigned long gauge = m_TMapList[0]->GetGauge();
	int count = m_TMapList.size();
	std::vector<unsigned char> markList;

	for (int i = 1; i < count; i++) {
		unsigned long current = m_TMapList[i]->GetGauge();
		if ((current > gauge ? current : gauge) != 0) {
			gauge = current;
		}
	}

	for (int j = 0; j < count; j++) {
		if (m_TMapList[j]->GetGauge() == gauge) {
			markList.push_back(j);
		}
	}

	for (unsigned int k = 0; k < markList.size(); k++) {
		m_TMapList[markList[k]]->SetTMark(1);
	}

	markList.clear();

	gauge = m_TMapList[0]->GetGauge();
	for (int i2 = 1; i2 < count; i2++) {
		unsigned long current = m_TMapList[i2]->GetGauge();
		if ((current < gauge ? current : gauge) != 0) {
			gauge = current;
		}
	}

	for (int j2 = 0; j2 < count; j2++) {
		if (m_TMapList[j2]->GetGauge() == gauge) {
			markList.push_back(j2);
		}
	}

	for (unsigned int k2 = 0; k2 < markList.size(); k2++) {
		m_TMapList[markList[k2]]->SetTMark(0);
	}
}

// treasure.cpp:254
void CTHunter::ReceiveAllGift(std::vector<sTreasureGift>& gifts) {
	int count = m_GiftList.size();
	if (count != 0) {
		PG_VECTOR_FREE_STORAGE(m_GiftList);
	}

	m_GiftList.swap(gifts);
}

// treasure.cpp:262
// Adds the received treasure gifts to the player's pang, items and cards.
void CTHunter::UpdateItemList(std::string name) {
	int count = m_GiftList.size();
	for (int i = 0; i < count; i++) {
		sTreasureGift gift;
		gift = m_GiftList[i];

		sItemInfo info;
		memset(&info, 0, sizeof(info));

		switch (gift.m_value4 >> 26) {
		case 6:
			if (gift.m_value4 == 0x1A000010) {
				WSingleton<CSharedDoc>::Instance()->m_myPang += gift.m_valueC;
				break;
			}

			{
				std::list<sItemInfo>::iterator it;
				for (it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {
					if (it->m_serial == gift.m_value8) {
						it->m_count += gift.m_valueC;
						break;
					}
				}

				if (it == WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end()) {
					info.m_typeId = gift.m_value4;
					info.m_serial = gift.m_value8;
					info.m_count = 1;
					if (gift.m_valueC >= 1) {
						info.m_count = gift.m_valueC;
					}
					info.m_stateBits.flag0 = 0;
					info.m_stateBits.expired = 0;
					info.m_stateBits.kind = gift.m_valueE;
					info.m_remainHours = gift.m_valueF;
					memset(info.m_reserved18, 0, sizeof(info.m_reserved18));
					info.m_flags28.flags01 |= 2;
					info.m_reserved16 = 0;
					WSingleton<CSharedDoc>::Instance()->m_itemInfoList.push_back(info);
				}
			}
			break;

		case 0x1C:
			{
				std::list<sItemInfo>::iterator it;
				for (it = WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3.end(); ++it) {
					if (it->m_serial == gift.m_value8) {
						it->m_count += gift.m_valueC;
						break;
					}
				}

				if (it == WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3.end()) {
					info.m_typeId = gift.m_value4;
					info.m_serial = gift.m_value8;
					info.m_count = 1;
					if (gift.m_valueC >= 1) {
						info.m_count = gift.m_valueC;
					}
					info.m_stateBits.flag0 = 0;
					info.m_stateBits.expired = 0;
					info.m_stateBits.kind = gift.m_valueE;
					memset(info.m_reserved18, 0, sizeof(info.m_reserved18));
					info.m_flags28.flags01 |= 2;
					info.m_reserved16 = 0;
					WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3.push_back(info);
				}
			}
			break;

		case 0x1F:
			WSingleton<CSharedDoc>::Instance()->AddCard(gift.m_value4, gift.m_value8, gift.m_valueC);
			break;
		}
	}
}

// treasure.cpp:403
void CTHunter::ReceiveRepository(std::vector<sTreasureHunt>& repository) {
	int count = m_Repository.size();
	if (count != 0) {
		PG_VECTOR_FREE_STORAGE(m_Repository);
	}

	m_Repository.swap(repository);
}

// treasure.cpp:413
sTreasureHunt CTHunter::GetGiftInfo(unsigned char index) {
	return m_Repository[index];
}

// treasure.cpp:419
// Merges the repository items of the same type id, summing their counts.
void CTHunter::ReAdjustRepository(void) {
	std::map<unsigned long, sTreasureHunt> merged;




	sTreasureHunt item;
	std::map<unsigned long, sTreasureHunt>::iterator found;
	for (std::vector<sTreasureHunt>::iterator it = m_Repository.begin(); it != m_Repository.end(); ++it) {

		item = *it;
		found = merged.find(item.m_typeId);

		if (found == merged.end()) {

			merged.insert(std::map<unsigned long, sTreasureHunt>::value_type(item.m_typeId, item));

		} else {

			found->second.m_count += item.m_count;
		}
	}






	unsigned int size = merged.size();
	PG_VECTOR_FREE_STORAGE(m_Repository);




	m_Repository.reserve(size);


	for (std::map<unsigned long, sTreasureHunt>::iterator it2 = merged.begin(); it2 != merged.end(); ++it2) {




		m_Repository.push_back(it2->second);
	}


	merged.clear();
}
