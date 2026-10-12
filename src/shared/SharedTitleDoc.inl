// Original: d:\build\custom\temp\code\shared\sharedtitledoc.inl
// Title conditions shared with the server; included by the units that
// define titles (Titles_Client).

#include <math.h>

// Player level thresholds (CheckLevel); the original header and names are not known.
enum eLevel {
};

float RecalculatePercent(float percent);

// The local player's information (CSharedDoc 0x3E0; CheckCourseMaster's
// first argument). Its layout follows sUserInfo's up to 0x96C: the
// constructor (??0sMyInfo, emitted by SharedDoc) marks both 0x14-entry
// course arrays empty. Its real header is not known; it belongs next to
// CSharedDoc (SharedDoc.h) once CSharedDoc holds it as a member. Only the
// members CheckCourseMaster uses are named.
struct sMyInfo {
	sMyInfo(void);

	PADDING(0x0, 0x2B4);
	sMapStatistics m_mapStatistics[20];  // 0x2B4, by course (best scores of the normal games)
	sMapStatistics m_mapStatistics2[20]; // 0x610, likewise (second record set)
};
CHECK_FIELD(sMyInfo, m_mapStatistics, 0x2B4);
CHECK_FIELD(sMyInfo, m_mapStatistics2, 0x610);

// sharedtitledoc.inl:8
bool CheckLevel(unsigned long level, eLevel required) {
	if (level >= (unsigned long)required) {
		return true;
	}
	return false;
}

// sharedtitledoc.inl:18
bool CheckPangyaPercent(unsigned long pangya, unsigned long shots, float required) {
	float percent = (float)pangya / (float)shots;
	if (RecalculatePercent(percent * 100.0f) >= required) {
		return true;
	}
	return false;
}

// sharedtitledoc.inl:28
bool CheckFairawayPercent(unsigned long fairway, unsigned long drives, unsigned long par3, float required) {
	float percent = (float)fairway / ((float)drives - (float)par3);
	if (RecalculatePercent(percent * 100.0f) >= required) {
		return true;
	}
	return false;
}

// sharedtitledoc.inl:38
bool CheckPuttinPercent(unsigned long putts, unsigned long attempts, float required) {
	float percent = (float)putts / (float)attempts;

	if (RecalculatePercent(percent * 100.0f) >= required) {
		return true;
	}
	return false;
}

// sharedtitledoc.inl:48
bool CheckNoMannerPercent(unsigned long noManner, unsigned long games, float required) {
	float percent = (float)noManner / (float)games;
	if (RecalculatePercent(percent * 100.0f) <= required) {
		return true;
	}
	return false;
}

// sharedtitledoc.inl:58
bool CheckGoldMedal(sTrophyStatistics &trophy, unsigned long required) {
	unsigned long total = 0;
	for (int course = 0; course < 13; ++course)
	{
		total += trophy.m_medals[course][0];
	}

	if (total >= required)
	{
		return true;
	}
	return false;
}

// sharedtitledoc.inl:75
bool CheckSilverMedal(sTrophyStatistics &trophy, unsigned long required) {
	unsigned long total = 0;
	for (int course = 0; course < 13; ++course)
	{
		total += trophy.m_medals[course][1];
	}

	if (total >= required)
	{
		return true;
	}
	return false;
}

// sharedtitledoc.inl:93
bool CheckBronzeMedal(sTrophyStatistics &trophy, unsigned long required) {
	unsigned long total = 0;
	for (int course = 0; course < 13; ++course)
	{
		total += trophy.m_medals[course][2];
	}

	if (total >= required)
	{
		return true;
	}
	return false;
}

// sharedtitledoc.inl:107
bool CheckCourseMaster(sMyInfo &info, std::map<unsigned char, sMapStatistics> &pastStat, std::map<unsigned char, sMapStatistics> &stat) {
	bool cleared[20];
	int i;
	for (i = 0; i < 20; i++)
	{
		// courses 12 and 17 do not count
		if (i == 12 || i == 17)
		{
			cleared[i] = true;
			continue;
		}

		cleared[i] = false;
		if (info.m_mapStatistics[i].m_bestScore != 0x7F)
		{
			cleared[i] = true;
		}
	}

	std::map<unsigned char, sMapStatistics>::iterator it = pastStat.begin();
	for (; it != pastStat.end(); ++it)
	{
		if (!cleared[it->second.m_course])
		{
			if (it->second.m_bestScore != 0x7F)
			{
				cleared[it->second.m_course] = true;
			}
		}
	}

	for (i = 0; i < 20; i++)
	{
		if (info.m_mapStatistics2[i].m_bestScore != 0x7F)
		{
			cleared[i] = true;
		}
	}

	it = stat.begin();
	for (; it != stat.end(); ++it)
	{
		if (!cleared[it->second.m_course])
		{
			if (it->second.m_bestScore != 0x7F)
			{
				cleared[it->second.m_course] = true;
			}
		}
	}

	for (i = 0; i < 20; i++)
	{
		if (!cleared[i])
		{
			return false;
		}
	}
	return true;
}

// sharedtitledoc.inl:204
float RecalculatePercent(float percent) {
	return (float)floor((percent + 0.05f) * 10.0f) * 0.1f;
}

// sharedtitledoc.inl:210
bool CheckPang(__int64 pang, __int64 required) {
	if (pang >= required) {
		return true;
	}
	return false;
}

// sharedtitledoc.inl:219
bool CheckHoleinCount(unsigned long holeIns, unsigned long required) {
	if (holeIns >= required) {
		return true;
	}
	return false;
}
