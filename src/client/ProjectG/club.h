// Original: d:\build\custom\temp\code\client\projectg\club.h
#ifndef PG_CLIENT_PROJECTG_CLUB_H
#define PG_CLIENT_PROJECTG_CLUB_H

#include "defines.h"
#include "padding.h"
#include "encrypttypes.h"
#include "wlist.h"
#include "classdefine.h"
#include "wmath.h"
#include "wscene.h"
#include <list>
#include <string.h>

class WVector;
class WPuppet;

// A club set the player owns: its item record and its loaded models
// (released by CClub::ClearClubSet), with the preview camera and light of each
// club type (CScreen::DrawClubInfo indexes them by CClub::GetType).
struct sClubSet {
	IFF_STRUCT::sClubSet *m_pItem;      // 0x0, the item record (FindClubSet matches its typeid)
	WPuppet *m_puppet[2][4];            // 0x4, released through g_resrcmng; [1][type] is the HUD preview model
	WMatrix m_cameras[4];               // 0x24, the HUD preview camera per club type
	LightSet m_lights[4];               // 0xE4, the HUD preview light per club type
};
CHECK_FIELD(sClubSet, m_pItem, 0x0);
CHECK_FIELD(sClubSet, m_puppet, 0x4);
CHECK_FIELD(sClubSet, m_cameras, 0x24);
CHECK_FIELD(sClubSet, m_lights, 0xE4);
CHECK_SIZE(sClubSet, 0x154);

// std::find_if predicate: the club set of a typeid.
struct sFindClubSet {
	// club.h:54
	sFindClubSet(unsigned long typeId) : m_typeId(typeId) {}
	// club.h:57
	bool operator()(sClubSet *clubSet) {
		return clubSet->m_pItem->m_typeId == m_typeId;
	}

	unsigned long m_typeId;             // 0x0
};
CHECK_SIZE(sFindClubSet, 0x4);

// A golf club of the player's set. Its members are transcribed only as far as
// the functions decompiled so far need them.
class CClub {
	LAYOUT_CHECK_FRIEND;
public:
	// The clubs of a set, in order; GetClubString names them.
	enum eClub {
		WOOD_1,
		WOOD_2,
		WOOD_3,
		IRON_2,
		IRON_3,
		IRON_4,
		IRON_5,
		IRON_6,
		IRON_7,
		IRON_8,
		IRON_9,
		WEDGE_P,
		WEDGE_S,
		PUTTER_1,
		PUTTER_2,
	};

	// One club of the set (AddClub); its compiler-generated constructor
	// (0x474050) builds the WCrypticValue<float> members.
	struct sClub {
		WCrypticValue<float> m_range;   // 0x0, AddClub's first value (GetRange)
		WCrypticValue<float> m_lie;     // 0x10, read by GetLie
		unsigned char m_id;             // 0x20
		unsigned char m_index;          // 0x21
		unsigned char m_type;           // 0x22
		PADDING(0x23, 0x24);
		WCrypticValue<float> m_power;   // 0x24 (GetPower)
		WCrypticValue<float> m_curve;   // 0x34 (GetCurve, club.h:184)
		WCrypticValue<float> m_spin;    // 0x44 (GetSpin, club.h:185)
		char m_name[4];                 // 0x54
		float m_pointFactor;            // 0x58
		WCrypticValue<float> m_barSpeed;    // 0x5C, bar_speed[m_id] (GetBarSpeed, club.h:174)
	};

	// club.h:75
	CClub(void) : m_pClub(NULL), m_clubList(16, 8), m_flags(0) {
		Init();
	}
	~CClub(void);

	float GetBarSpeed(void);                // club.h:174
	unsigned char GetType(void);            // club.h:179
	char *GetName(void);                    // club.h:181
	// club.h:184 (QuadTree's object holds an out-of-line copy)
	float GetCurve(void) { return m_pClub->m_curve; }
	// club.h:185 (QuadTree's object holds an out-of-line copy)
	float GetSpin(void) { return m_pClub->m_spin; }
	// club.h:191 (GolfDoc's object holds an out-of-line copy)
	unsigned char GetApproachMode(void) { return m_approachMode; }
	unsigned char GetIndex(void);           // club.h:193
	unsigned char GetID(void);              // club.h:194
	bool Updated(void);                     // club.h:195
	bool TypeUpdated(void);   // club.h:196
	// club.h:199 (grid's object holds an out-of-line copy)
	bool IsPowerPutter(void) { return m_powerPutter; }
	// club.h:200
	bool Is(char *name, sClub *club) {
		if (club)
			return strstr(club->m_name, name) ? true : false;
		return strstr(m_pClub->m_name, name) ? true : false;
	}
	float GetBarWidth(void);                // club.h:204 (gui_control's object holds a copy)
	unsigned char GetBarAlpha(void);        // club.h:205 (gui_control's object holds a copy)
	// club.h:202
	void SetBar(int bar) { m_bar = bar; }
	bool BarIs(int bar);                    // club.h:203
	// club.h:207
	void SetItemWindow(bool itemWindow) { m_itemWindow = itemWindow; }
	void SetRandomPower(float power);       // club.h:209
	float GetRandomPower(void) const;       // club.h:210
	void SetCalcRandomPower(bool calc);     // club.h:211
	// club.h:212 (fallingspot's object holds an out-of-line copy)
	bool IsCalcRandomPower(void) { return m_calcRandomPower; }
	void ClearClubSet(void);
	sClubSet *AddClubSet(unsigned long typeId);
	void AddCrypticValue(void);
	float GetPower(void);
	sClubSet *FindClubSet(unsigned long typeId);
	void SetAccuracy(void);
	void SetApproachMode(void);
	void SelectClubByDist(float dist);
	void Process(float time);
	void SetClub(unsigned char index);
	void SetClub(const char *name);
	void SetPointFactor(float factor);
	float GetPowerShotFactor(void);
	void SelectBestClub(void);
	void SelectBestClub(const WVector &target);
	float GetNarrowAccuracy(void);
	float GetRange(const char *name);
	float GetWideAccuracy(void);
	float GetLie(const char *name);
	float GetAirNightLuckyRange(char *name);
	float GetAirNightLuckyPower(char *name);
	void ClearPointFactor(void);
	unsigned char GetType(unsigned char index);
	const char *GetClubString(eClub club);
	void SetPowerShot(int powerShot);       // club.h:187
	int PowerShotType(void);                // club.h:188

private:
	friend class CGolfPlayer;   // Shot_PowerShot clears m_calcRandomPower
	void AddClub(char *name, float range, float lie, float power, float curve, float spin, unsigned char type);
	void Init(void);
	sClub *SelectClub(unsigned char index);
	void BarProcess(float time);
	void ClubProcess(float time);

	unsigned long m_unusedHead;         // 0x0, only Init writes it (zero); no reader in club's object
	sClub *m_pClub;                     // 0x4
	WList<sClub *> m_clubList;          // 0x8
	unsigned char m_clubIndex;          // 0x28, the current club's index
	unsigned char m_approachClubIndex;  // 0x29, set by SetApproachMode: 7, 0 in approach mode 4
	PADDING(0x2A, 0x2C);
	WCrypticValue<int> m_powerShot;     // 0x2C
	unsigned char m_approachMode;       // 0x3C, 0-4 by distance to the cup (SetApproachMode); 4 ignores the ball's lie angle (GetLie)
	PADDING(0x3D, 0x40);
	unsigned long m_flags;              // 0x40
	bool m_powerPutter;                 // 0x44 (IsPowerPutter), set by SetApproachMode (false within 15 yards of the cup, true off the green)
	PADDING(0x45, 0x48);
	float m_barDelay;                   // 0x48, seconds waited in bar state 0 before the bar shows (BarProcess)
	float m_barAlpha;                   // 0x4C, rises to 255 in bar state 3 (BarProcess)
	float m_barPosition;                // 0x50, falls from 456 to 0 in bar state 1 (BarProcess)
	int m_bar;                          // 0x54
	bool m_itemWindow;                  // 0x58 (set by the item window)
	unsigned char m_prevClubIndex;      // 0x59, m_clubIndex at the previous Process
	PADDING(0x5A, 0x5C);
	float m_accuracy;                   // 0x5C, the player's accuracy plus the ball's ground's (SetAccuracy)
	float m_wideAccuracy;               // 0x60, m_accuracy + 10, at most 35 (SetAccuracy)
	float m_randomPower;                // 0x64
	bool m_calcRandomPower;             // 0x68
	std::list<sClubSet *> m_clubSetList;    // 0x6C
};

BEGIN_LAYOUT_CHECK(CClub)
	CHECK_FIELD(CClub::sClub, m_id, 0x20);
	CHECK_FIELD(CClub::sClub, m_index, 0x21);
	CHECK_FIELD(CClub::sClub, m_type, 0x22);
	CHECK_FIELD(CClub::sClub, m_lie, 0x10);
	CHECK_FIELD(CClub::sClub, m_name, 0x54);
	CHECK_FIELD(CClub::sClub, m_pointFactor, 0x58);
	CHECK_FIELD(CClub::sClub, m_range, 0x0);
	CHECK_FIELD(CClub::sClub, m_power, 0x24);
	CHECK_FIELD(CClub::sClub, m_curve, 0x34);
	CHECK_FIELD(CClub::sClub, m_spin, 0x44);
	CHECK_FIELD(CClub::sClub, m_barSpeed, 0x5C);
	CHECK_SIZE(CClub::sClub, 0x6C);
	CHECK_FIELD(CClub, m_pClub, 0x4);
	CHECK_FIELD(CClub, m_clubList, 0x8);
	CHECK_FIELD(CClub, m_clubIndex, 0x28);
	CHECK_FIELD(CClub, m_powerShot, 0x2C);
	CHECK_FIELD(CClub, m_approachMode, 0x3C);
	CHECK_FIELD(CClub, m_flags, 0x40);
	CHECK_FIELD(CClub, m_barDelay, 0x48);
	CHECK_FIELD(CClub, m_barAlpha, 0x4C);
	CHECK_FIELD(CClub, m_barPosition, 0x50);
	CHECK_FIELD(CClub, m_bar, 0x54);
	CHECK_FIELD(CClub, m_itemWindow, 0x58);
	CHECK_FIELD(CClub, m_accuracy, 0x5C);
	CHECK_FIELD(CClub, m_wideAccuracy, 0x60);
	CHECK_FIELD(CClub, m_randomPower, 0x64);
	CHECK_FIELD(CClub, m_calcRandomPower, 0x68);
	CHECK_FIELD(CClub, m_approachClubIndex, 0x29);
	CHECK_FIELD(CClub, m_powerPutter, 0x44);
	CHECK_FIELD(CClub, m_unusedHead, 0x0);
	CHECK_FIELD(CClub, m_prevClubIndex, 0x59);
	CHECK_FIELD(CClub, m_clubSetList, 0x6C);
END_LAYOUT_CHECK

// GolfRule.cpp defines GOLFRULE_CLUB_INLINE empty before its includes: the
// original holds the out-of-line copy of these inline accessors in GolfRule's
// object; every other unit inlines them.
#ifndef GOLFRULE_CLUB_INLINE
#define GOLFRULE_CLUB_INLINE inline
#endif

// club.h:174
GOLFRULE_CLUB_INLINE float CClub::GetBarSpeed(void) {
	return m_pClub->m_barSpeed;
}

// club.h:179
GOLFRULE_CLUB_INLINE unsigned char CClub::GetType(void) {
	return m_pClub->m_type;
}

// club.h:181
GOLFRULE_CLUB_INLINE char *CClub::GetName(void) {
	return m_pClub->m_name;
}

// club.h:187
GOLFRULE_CLUB_INLINE void CClub::SetPowerShot(int powerShot) {
	m_powerShot = powerShot;
}

// club.h:188
GOLFRULE_CLUB_INLINE int CClub::PowerShotType(void) {
	return m_powerShot;
}

// club.h:193
GOLFRULE_CLUB_INLINE unsigned char CClub::GetIndex(void) {
	return m_pClub->m_index;
}

// club.h:194
GOLFRULE_CLUB_INLINE unsigned char CClub::GetID(void) {
	return m_pClub->m_id;
}

// club.h:203
GOLFRULE_CLUB_INLINE bool CClub::BarIs(int bar) {
	return m_bar == bar;
}

// club.h:209
GOLFRULE_CLUB_INLINE void CClub::SetRandomPower(float power) {
	m_randomPower = power;
}

// club.h:210
GOLFRULE_CLUB_INLINE float CClub::GetRandomPower(void) const {
	return m_randomPower;
}

// club.h:211
GOLFRULE_CLUB_INLINE void CClub::SetCalcRandomPower(bool calc) {
	m_calcRandomPower = calc;
}

// camera.cpp defines CAMERA_CLUB_INLINE empty before its includes: the
// original holds the out-of-line copy of Updated in camera's object.
#ifndef CAMERA_CLUB_INLINE
#define CAMERA_CLUB_INLINE inline
#endif

// club.h:195
CAMERA_CLUB_INLINE bool CClub::Updated(void) {
	return (m_flags & 1) ? true : false;
}

// golfplayer.cpp defines GOLFPLAYER_CLUB_INLINE empty before its includes:
// the original holds the out-of-line copy of TypeUpdated in golfplayer's
// object.
#ifndef GOLFPLAYER_CLUB_INLINE
#define GOLFPLAYER_CLUB_INLINE inline
#endif

// club.h:196
GOLFPLAYER_CLUB_INLINE bool CClub::TypeUpdated(void) {
	return ((m_flags >> 1) & 1) ? true : false;
}

// gui_control.cpp defines GUI_CONTROL_CLUB_INLINE empty before its includes:
// the original holds the out-of-line copy of these accessors in gui_control's
// object.
#ifndef GUI_CONTROL_CLUB_INLINE
#define GUI_CONTROL_CLUB_INLINE inline
#endif

// club.h:204
GUI_CONTROL_CLUB_INLINE float CClub::GetBarWidth(void) {
	return m_barPosition;
}

// club.h:205
GUI_CONTROL_CLUB_INLINE unsigned char CClub::GetBarAlpha(void) {
	return (unsigned char)m_barAlpha;
}

CClub &GolfClub(void);

#endif // PG_CLIENT_PROJECTG_CLUB_H
