// The JP 983 mastery stat rules' game-state helpers (JpMasteryState.cpp).
// JpMasteryStatInfo itself is declared in JpMasterySlot.h.
#ifndef PG_JP_JPMASTERYSTATE_H
#define PG_JP_JPMASTERYSTATE_H

// 0x9d9c50 (fastcall, level in ecx; name ours): the level's class, one step
// every 5 levels from level 6 (0 below 6, 1 from 6, 2 from 11 ... 13 from
// 66). JpMasteryStatInfo::GetPenaltyValue adds 0x14 to it.
int Jp_GetLevelClass(int level);

#endif  // PG_JP_JPMASTERYSTATE_H
