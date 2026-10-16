// Original: d:\build\custom\temp\code\client\projectg\playerdlg.h
#ifndef PG_CLIENT_PROJECTG_PLAYERDLG_H
#define PG_CLIENT_PROJECTG_PLAYERDLG_H

#include "defines.h"
#include "padding.h"
#include "frform.h"
#include "fresh.h"
#include "UccManager.h"
#include <list>
#include <vector>

class FrArea;
class FrButton;
class FrEdit;
class FrListBox;
class FrGaugeBar;
class Bitmap;
class CExhibition;
class CPartTidList;
PG_ENUM_FWD(eLvlType)
PG_ENUM_FWD(PANGYA_ITEM_GROUP)
struct sUserEquip;

// Status bar of the player dialog (one bar per level type). Only the members
// this unit defines so far are declared.
class CStatBar {
	LAYOUT_CHECK_FRIEND;
public:
	// One bar of the status bar.
	struct sBar {
		sBar(const char* name) {   // playerdlg.h:174
			m_curPos = 0.0f;
			m_destPos = 0.0f; m_rangeLimit = 0.0f;
			m_pBitmap = g_pFresh->GetBitmap(name);
		}

		const Bitmap* m_pBitmap;   // 0x0, set from the bitmap name by the constructor
		float m_curPos;            // 0x4, moves towards m_destPos (Process)
		float m_destPos;           // 0x8, never below 0 (SetPos, SetRange)
		float m_rangePos;          // 0xC, clamped to m_rangeLimit by SetRange
		float m_rangeLimit;        // 0x10
		int m_maxValue;            // 0x14, the third value of SetPos/SetRange
	};

	void SetRange(eLvlType type, int pos, int range, int maxValue);   // playerdlg.cpp:64
	void SetPos(eLvlType type, int pos, int range, int maxValue);     // playerdlg.cpp:113
	void Render(const WPoint& pos);     // playerdlg.cpp:120

	CStatBar(float barGap, float barHeight);   // playerdlg.cpp:40
	~CStatBar(void);                    // playerdlg.cpp:58
	void SetDestTime(float destTime);   // playerdlg.cpp:72
	void Process(float dt);

private:
	std::vector<sBar> m_bars;   // 0x0
	float m_barGap;     // 0x10, vertical gap between two bars (Render)
	float m_destTime;   // 0x14
	float m_barHeight;  // 0x18, height of one bar (Render)
};
CHECK_SIZE(CStatBar, 0x1C);
CHECK_SIZE(CStatBar::sBar, 0x18);
CHECK_FIELD(CStatBar::sBar, m_curPos, 0x4);
CHECK_FIELD(CStatBar::sBar, m_destPos, 0x8);
CHECK_FIELD(CStatBar::sBar, m_rangePos, 0xC);
CHECK_FIELD(CStatBar::sBar, m_rangeLimit, 0x10);
CHECK_FIELD(CStatBar::sBar, m_maxValue, 0x14);
BEGIN_LAYOUT_CHECK(CStatBar)
	CHECK_FIELD(CStatBar, m_bars, 0x0);
	CHECK_FIELD(CStatBar, m_barGap, 0x10);
	CHECK_FIELD(CStatBar, m_destTime, 0x14);
	CHECK_FIELD(CStatBar, m_barHeight, 0x18);
END_LAYOUT_CHECK

// Equipment selection bar of the player dialog. Only the members this
// project's code uses so far are declared.
class FrSubEquipBar : public FrForm {
	LAYOUT_CHECK_FRIEND;

public:
	struct _sortitem {
		_sortitem(IFF_ITEM_COMMON* pItem, unsigned long guid) : m_pItem(pItem), m_guid(guid) {}   // playerdlg.h:102
		// playerdlg.h:104
		bool operator<(const _sortitem& rhs) const {
			if (m_pItem->m_typeId == rhs.m_pItem->m_typeId)
				return m_guid < rhs.m_guid;

			return m_pItem->m_typeId < rhs.m_pItem->m_typeId;
		}

		IFF_ITEM_COMMON* m_pItem;   // 0x0
		unsigned long m_guid;       // 0x4
	};

	// Item category shown by the bar; enumerators not recovered yet.
	enum eCategory {
	};

	// Sub type of the bar; enumerators not recovered yet.
	enum eSubType {
	};

	void Open(FRESH_RESULT_CALLBACK pfnResult, eCategory category, const WPoint& pt, eSubType subType);   // playerdlg.cpp:657

	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }

	FrSubEquipBar(void);            // playerdlg.cpp:156
	virtual ~FrSubEquipBar(void);   // playerdlg.cpp:161

	IFF_ITEM_COMMON *GetResult(void);          // playerdlg.h:120
	unsigned long GetLastSelectedGuid(void);   // playerdlg.h:121
	eCategory GetCategory(void);               // playerdlg.h:122
	bool SetClientEquipInfo(PANGYA_ITEM_GROUP group, unsigned long guid);

private:
	static sFRESH_ENTRY _MsgEntries[];

protected:
	static sFRESH_MSGMAP _MsgMap;
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;   // playerdlg.cpp:147

	void OnInitSubEquipList(intptr_t param);
	void OnDownSubEquipList(PG_MSGARG param);
	void OnDrawSubEquipList(PG_MSGARG param);

	virtual void OnDraw(void);     // playerdlg.cpp:629
	virtual void OnProc(const float dt);   // playerdlg.cpp:634
	virtual void OnOK(void);       // playerdlg.cpp:647
	virtual void OnCancel(void);   // playerdlg.cpp:652

private:
	std::list<_sortitem> m_sortList;   // 0x110
	IFF_ITEM_COMMON *m_result;     // 0x11C
	FrListBox* m_pSubEquipList;    // 0x120, "sub_equip"
	void Initialize(void);

	eCategory m_category;          // 0x124
	eSubType m_subType;            // 0x128
	unsigned long m_lastSelectedGuid;  // 0x12C
};
BEGIN_LAYOUT_CHECK(FrSubEquipBar)
	CHECK_FIELD(FrSubEquipBar, m_sortList, 0x110);
	CHECK_FIELD(FrSubEquipBar, m_result, 0x11C);
	CHECK_FIELD(FrSubEquipBar, m_pSubEquipList, 0x120);
	CHECK_FIELD(FrSubEquipBar, m_category, 0x124);
	CHECK_FIELD(FrSubEquipBar, m_subType, 0x128);
	CHECK_FIELD(FrSubEquipBar, m_lastSelectedGuid, 0x12C);
END_LAYOUT_CHECK

// Singleton helper that picks the equipment of the player dialog.
class CEquipHelper {
public:
	// Equipment kind; the values are those of PANGYA_ITEM_GROUP (TaskMain).
	enum eCategory {
		eCategoryCharacter = 1,
		eCategoryClub = 4,
		eCategoryBall = 5,
		eCategoryCaddie = 7,
		eCategoryMascot = 0x10
	};

	// playerdlg.h:26. Not inlined (guarded local static with a destructor):
	// every caller emits an out-of-line copy; the linker kept FamilyMain's.
	static CEquipHelper* Instance(void) {
		static CEquipHelper _Instance;
		return &_Instance;
	}

	IFF_ITEM_COMMON* GetItemCommon(eCategory category, const sUserEquip& equip);   // playerdlg.cpp:751
	unsigned long GetEquipItemGuid(eCategory category, bool random);   // playerdlg.cpp:799
	unsigned long GetEquipItemTid(eCategory category, bool random);   // playerdlg.cpp:885
	unsigned long GetGuidFromTid(eCategory category, unsigned long tid);   // playerdlg.cpp:963

private:
	CEquipHelper(void);    // playerdlg.cpp:741
	~CEquipHelper(void);   // playerdlg.cpp:746

	// playerdlg.h:45. The key of a random entry of map, 0 when it is empty.
	template<class T> unsigned long RandSelectGuid(T& map) {
		int count = map.size();
		if (count == 0)
			return 0;

		typename T::const_iterator it = map.begin();

		int skip = rand() % count;
		for (int i = 0; i < skip; i++)
			++it;

		return it->first;
	}

	// playerdlg.h:60. The type id of the item keyed id in map, or of a
	// random one when id is 0. The original read one type-id member name of
	// sCharacterInfo/sCaddieInfo/sItemInfo/sMascotInfo; ours differ
	// (sItemInfo::m_typeId), so EquipTypeId (PlayerDlg.cpp, static, always
	// inlined) reads it.
	template<class T> unsigned long MapFindEquipTid(T& map, unsigned long id) {
		if (id == 0) {
			int count = map.size();
			if (count == 0)
				return 0;

			typename T::iterator it = map.begin();

			int skip = rand() % count;
			for (int i = 0; i < skip; i++)
				++it;

			return EquipTypeId(it->second);
		}

		typename T::iterator it = map.find(id);
		if (it != map.end())
			return EquipTypeId(it->second);

		return 0;
	}
};

// Player (character selection) dialog. Only the members this unit defines so
// far are declared.
class FrPlayerDlg : public FrForm {
	LAYOUT_CHECK_FRIEND;
public:
	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }   // playerdlg.h:207

	FrPlayerDlg(void) : m_pExhibition(0), m_pPartTidList(0), m_team(2), m_playerIndex(-1), m_pLinkedWnd(0) {}   // playerdlg.h:210

	void Init(int index);          // playerdlg.cpp:1104
	void OnCustomProc(float dt);   // playerdlg.cpp:1173
	void SetTeam(unsigned char team);   // playerdlg.cpp:1185
	void Select(void);     // playerdlg.cpp:1224
	void UnSelect(void);   // playerdlg.cpp:1230
	virtual ~FrPlayerDlg(void);   // playerdlg.cpp:1099
	void HidePet(void);           // playerdlg.cpp:1142
	const char* GetName(void);   // playerdlg.cpp:1378
	void SetName(const char* name);   // playerdlg.cpp:1386
	void SetState(void);         // playerdlg.cpp:1148
	void SetRandomState(void);   // playerdlg.cpp:1162
	void ShowPet(bool show);

protected:
	// Direction of the caddie browse buttons.
	enum eButtonDir {
		eButtonDirPrev = 0,
		eButtonDirNext = 1,
		eButtonDirCurrent = 2,   // SetState: show the player's own selection
		eButtonDirRandom = 3     // SetRandomState
	};

	static sFRESH_ENTRY _MsgEntries[];
	static sFRESH_MSGMAP _MsgMap;
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;   // playerdlg.cpp:1053

	void SetCharControl(eButtonDir dir);
	void SetCaddieControl(eButtonDir dir);
	void SetClubControl(eButtonDir dir);
	void SetBallControl(eButtonDir dir);
	void OnBaseInit(intptr_t param);         // playerdlg.cpp:1208
	void OnBaseBtnUp(void);             // playerdlg.cpp:1213
	void OnSelectInit(intptr_t param);       // playerdlg.cpp:1218
	void OnIndexInit(intptr_t param);        // playerdlg.cpp:1395
	void OnPowerBarInit(intptr_t param);     // playerdlg.cpp:1956
	void OnControlBarInit(intptr_t param);   // playerdlg.cpp:1961
	void OnImpactBarInit(intptr_t param);    // playerdlg.cpp:1967
	void OnSpinBarInit(intptr_t param);      // playerdlg.cpp:1973
	void OnCurveBarInit(intptr_t param);     // playerdlg.cpp:1979
	void OnPowerEditInit(PG_MSGARG param);    // playerdlg.cpp:1987
	void OnControlEditInit(PG_MSGARG param);  // playerdlg.cpp:1992
	void OnImpactEditInit(PG_MSGARG param);   // playerdlg.cpp:1997
	void OnSpinEditInit(PG_MSGARG param);     // playerdlg.cpp:2002
	void OnCurveEditInit(PG_MSGARG param);    // playerdlg.cpp:2007
	void OnPetInit(PG_MSGARG param);          // playerdlg.cpp:1236
	void OnCharPrevInit(PG_MSGARG param);     // playerdlg.cpp:1241
	void OnCharNextInit(PG_MSGARG param);     // playerdlg.cpp:1252
	void OnCaddiePrevInit(PG_MSGARG param);   // playerdlg.cpp:1273
	void OnCaddieNextInit(PG_MSGARG param);   // playerdlg.cpp:1284
	void OnClubPrevInit(PG_MSGARG param);     // playerdlg.cpp:1305
	void OnClubNextInit(PG_MSGARG param);     // playerdlg.cpp:1316
	void OnBallPrevInit(PG_MSGARG param);     // playerdlg.cpp:1332
	void OnBallNextInit(PG_MSGARG param);     // playerdlg.cpp:1343
	void OnRemoveInit(PG_MSGARG param);       // playerdlg.cpp:1354
	void OnRemoveBtnUp(void);           // playerdlg.cpp:1360
	void SetClub(unsigned long typeId); // playerdlg.cpp:1926
	void OnCaddieInit(PG_MSGARG param);       // playerdlg.cpp:1263
	void OnCaddieLimitInit(PG_MSGARG param);  // playerdlg.cpp:1268
	void OnClubInit(PG_MSGARG param);         // playerdlg.cpp:1295
	void OnClubLimitInit(PG_MSGARG param);    // playerdlg.cpp:1300
	void OnBallInit(PG_MSGARG param);         // playerdlg.cpp:1327
	void OnNameInit(intptr_t param);         // playerdlg.cpp:1373
	void SetCharacter(void);            // playerdlg.cpp:1849
	void SetCaddie(unsigned long typeId);   // playerdlg.cpp:1860
	void SetBall(unsigned long typeId); // playerdlg.cpp:1942
	void SetLevelBar(void);
	void OnCharPrevBtnUp(void);     // playerdlg.cpp:1247
	void OnCharNextBtnUp(void);     // playerdlg.cpp:1258
	void OnCaddiePrevBtnUp(void);   // playerdlg.cpp:1279
	void OnCaddieNextBtnUp(void);   // playerdlg.cpp:1290
	void OnClubPrevBtnUp(void);     // playerdlg.cpp:1311
	void OnClubNextBtnUp(void);     // playerdlg.cpp:1322
	void OnBallPrevBtnUp(void);     // playerdlg.cpp:1338
	void OnBallNextBtnUp(void);     // playerdlg.cpp:1349
	void OnPetOwnerDraw(void);      // playerdlg.cpp:1179
	void SetLevelNum(unsigned char type, unsigned char level, unsigned char maxLevel);   // playerdlg.cpp:1484

private:
	FrArea* m_pPetArea;           // 0x110, "pet"
	FrButton* m_pCharPrevBtn;     // 0x114
	FrButton* m_pCharNextBtn;     // 0x118
	FrArea* m_pCaddieArea;        // 0x11C, "caddie"
	FrArea* m_pCaddieLimitArea;   // 0x120, "caddie_limit"
	FrButton* m_pCaddiePrevBtn;   // 0x124
	FrButton* m_pCaddieNextBtn;   // 0x128
	FrArea* m_pClubArea;          // 0x12C, "club"
	FrArea* m_pClubLimitArea;     // 0x130, "club_limit", shown while the club set's level is above the player's
	FrButton* m_pClubPrevBtn;     // 0x134
	FrButton* m_pClubNextBtn;     // 0x138
	FrArea* m_pBallArea;          // 0x13C, "ball"
	FrButton* m_pBallPrevBtn;     // 0x140
	FrButton* m_pBallNextBtn;     // 0x144
	FrButton* m_pRemoveBtn;       // 0x148, shown for the players from index 2 on
	FrEdit* m_pNameEdit;          // 0x14C
	FrArea* m_pIndexArea;         // 0x150, "index"
	FrButton* m_pTeamBtn;         // 0x154, team colour button
	FrWnd* m_pSelectWnd;          // 0x158, shown while the dialog is selected
	FrGaugeBar* m_pStatGauge[5];  // 0x15C, gauge per level type (power, control, impact, spin, curve)
	FrEdit* m_pLevelEdit[5];      // 0x170, level text per level type
	CExhibition* m_pExhibition;   // 0x184, character model
	CPartTidList* m_pPartTidList; // 0x188, parts of the character model
	unsigned char m_team;         // 0x18C
	int m_playerIndex;            // 0x190, index into CSharedDoc::m_roomPlayers, -1 after the constructor
	FrWnd* m_pLinkedWnd;          // 0x194, shown again when the player is removed (OnRemoveBtnUp); cleared by the constructor
};
CHECK_SIZE(FrPlayerDlg, 0x198);
BEGIN_LAYOUT_CHECK(FrPlayerDlg)
	CHECK_FIELD(FrPlayerDlg, m_pPetArea, 0x110);
	CHECK_FIELD(FrPlayerDlg, m_pCharPrevBtn, 0x114);
	CHECK_FIELD(FrPlayerDlg, m_pCharNextBtn, 0x118);
	CHECK_FIELD(FrPlayerDlg, m_pCaddiePrevBtn, 0x124);
	CHECK_FIELD(FrPlayerDlg, m_pCaddieNextBtn, 0x128);
	CHECK_FIELD(FrPlayerDlg, m_pClubPrevBtn, 0x134);
	CHECK_FIELD(FrPlayerDlg, m_pClubNextBtn, 0x138);
	CHECK_FIELD(FrPlayerDlg, m_pBallPrevBtn, 0x140);
	CHECK_FIELD(FrPlayerDlg, m_pBallNextBtn, 0x144);
	CHECK_FIELD(FrPlayerDlg, m_pRemoveBtn, 0x148);
	CHECK_FIELD(FrPlayerDlg, m_pCaddieArea, 0x11C);
	CHECK_FIELD(FrPlayerDlg, m_pCaddieLimitArea, 0x120);
	CHECK_FIELD(FrPlayerDlg, m_pClubArea, 0x12C);
	CHECK_FIELD(FrPlayerDlg, m_pClubLimitArea, 0x130);
	CHECK_FIELD(FrPlayerDlg, m_pBallArea, 0x13C);
	CHECK_FIELD(FrPlayerDlg, m_pNameEdit, 0x14C);
	CHECK_FIELD(FrPlayerDlg, m_pPartTidList, 0x188);
	CHECK_FIELD(FrPlayerDlg, m_pIndexArea, 0x150);
	CHECK_FIELD(FrPlayerDlg, m_pStatGauge, 0x15C);
	CHECK_FIELD(FrPlayerDlg, m_pTeamBtn, 0x154);
	CHECK_FIELD(FrPlayerDlg, m_pSelectWnd, 0x158);
	CHECK_FIELD(FrPlayerDlg, m_pLevelEdit, 0x170);
	CHECK_FIELD(FrPlayerDlg, m_pExhibition, 0x184);
	CHECK_FIELD(FrPlayerDlg, m_team, 0x18C);
	CHECK_FIELD(FrPlayerDlg, m_playerIndex, 0x190);
	CHECK_FIELD(FrPlayerDlg, m_pLinkedWnd, 0x194);
END_LAYOUT_CHECK

// Empty player slot dialog; its size (0x128) is the `new` of
// FrPlayerEmptyDlgMakeInstance.
class FrPlayerEmptyDlg : public FrForm {
	LAYOUT_CHECK_FRIEND;
public:
	static const WRTTI m_RTTI;
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }   // playerdlg.h:331

	FrPlayerEmptyDlg(void) : m_pPlayerDlg(0), m_index(-1) {}   // playerdlg.h:334

	void Init(int index);   // playerdlg.cpp:2026

private:
	static sFRESH_ENTRY _MsgEntries[];

protected:
	static sFRESH_MSGMAP _MsgMap;
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;   // playerdlg.cpp:2016

	void OnBaseInit(intptr_t param);
	void OnCreateInit(intptr_t param);
	void OnCreateBtnUp(void);
	void OnIndexInit(intptr_t param);

private:
	FrButton* m_pCreateBtn; // 0x110, "create"
	FrArea* m_pBgArea;      // 0x114, "index", background image of the slot
	FrArea* m_pBaseArea;    // 0x118, "base"
	FrPlayerDlg* m_pPlayerDlg;   // 0x11C, the player dialog shown in place of the slot (OnCreateBtnUp); cleared by the constructor
	int m_index;            // 0x120, slot index set by Init
	UNKNOWN(0x124, 0x128);
};
CHECK_SIZE(FrPlayerEmptyDlg, 0x128);
BEGIN_LAYOUT_CHECK(FrPlayerEmptyDlg)
	CHECK_FIELD(FrPlayerEmptyDlg, m_pCreateBtn, 0x110);
	CHECK_FIELD(FrPlayerEmptyDlg, m_pBgArea, 0x114);
	CHECK_FIELD(FrPlayerEmptyDlg, m_pBaseArea, 0x118);
	CHECK_FIELD(FrPlayerEmptyDlg, m_pPlayerDlg, 0x11C);
	CHECK_FIELD(FrPlayerEmptyDlg, m_index, 0x120);
END_LAYOUT_CHECK

// The original kept TaskMain's out-of-line copies of these accessors:
// TaskMain.cpp defines TASKMAIN_PLAYERDLG_INLINE and the bodies itself;
// every other unit inlines them.
#ifndef TASKMAIN_PLAYERDLG_INLINE
// playerdlg.h:120
inline IFF_ITEM_COMMON *FrSubEquipBar::GetResult(void) {
	return m_result;
}

// playerdlg.h:121
inline unsigned long FrSubEquipBar::GetLastSelectedGuid(void) {
	return m_lastSelectedGuid;
}

// playerdlg.h:122
inline FrSubEquipBar::eCategory FrSubEquipBar::GetCategory(void) {
	return m_category;
}
#endif

#endif // PG_CLIENT_PROJECTG_PLAYERDLG_H
