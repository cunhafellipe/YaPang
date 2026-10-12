// Original: d:\build\custom\temp\code\client\projectg\club.cpp
#define CLUB_WPUPPET_INLINE
#define CLUB_SHAREDDOC_INLINE
#include <math.h>
#include "club.h"
#include "wutil.h"
#include <string.h>
#include "wmath.h"
#include "golfball.h"
#include "GolfDoc.h"
#include "SharedDoc.h"
#include "quadtree.h"
#include "wresrcmng.h"
#include <algorithm>
#include "ChatMsg.h"
#include "Packet.h"
#include "hackingmanager.h"
#include "cardsystem.h"
#include "shared/Localize.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "QuitWindow.h"
#include "ItemWindow.h"
#include "InputManager.h"
#include "TaskManager.h"
#include "actor.h"
#include "SoundManager.h"
#include "HelpMsg.h"
#include "slicestr.h"
#include "wpuppet.h"
#include "AutoTarget.h"

// Declared nowhere yet: SharedDoc.h (PerformPowerBarSetting) and
// s5/Utilities.h (S5::IsUsableCalipers) are other units' headers.
void PerformPowerBarSetting(void);
namespace S5 {
	bool IsUsableCalipers(void);
}

// The club names AddClub looks a club's id up in (17, then a NULL), and the
// bar speed of each id.
char *clubName[] = {
	"1W", "2W", "3W", "4W", "5W",
	"2I", "3I", "4I", "5I", "6I", "7I", "8I", "9I",
	"SW", "PW",
	"1PT", "2PT",
	NULL
};
float bar_speed[] = {
	460.0f, 460.0f, 460.0f, 460.0f, 460.0f,
	440.0f, 440.0f, 410.0f, 410.0f, 390.0f, 390.0f, 390.0f, 390.0f,
	360.0f, 360.0f,
	200.0f, 200.0f
};

// The accuracy each ground type adds, indexed by CGolfBall::m_groundType.
float ground_accuracy[] = {
	8.0f, 8.0f, 0.0f, 1.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 0.0f, 0.0f, 5.0f, 0.0f, 1.0f, 0.0f
};

// The scene camera (declared by no header in src yet).
extern WMatrix g_camera;


// club.cpp:73
CClub &GolfClub(void) {
	static CClub club;
	return club;
}

// club.cpp:79
CClub::~CClub(void) {
	for (sClub *club = m_clubList.Start(); club; club = m_clubList.Next()) { if (club) delete club; } m_clubList.Reset();
}

// club.cpp:84
void CClub::ClearClubSet(void) {
	std::list<sClubSet *>::iterator it;


	for (it = m_clubSetList.begin(); it != m_clubSetList.end(); ++it) {

		sClubSet *clubSet = *it;

		for (int i = 0; i < 4; i++) {

			if (g_resrcmng != NULL && clubSet->m_puppet[0][i] != NULL) { g_resrcmng->Release(clubSet->m_puppet[0][i], false); clubSet->m_puppet[0][i] = NULL; }
			if (g_resrcmng != NULL && clubSet->m_puppet[1][i] != NULL) { g_resrcmng->Release(clubSet->m_puppet[1][i], false); clubSet->m_puppet[1][i] = NULL; }
		}

		delete clubSet;
	}

	m_clubSetList.clear();

	m_calcRandomPower = false;
}

// club.cpp:107
void CClub::Init(void) {
	// club.cpp:110
	AddClub("1W", 230.0f, 10.0f, 236.0f, 1.61f, 0.55f, 0);
	AddClub("2W", 210.0f, 13.0f, 204.0f, 1.41f, 0.5f, 0);
	AddClub("3W", 190.0f, 16.0f, 176.0f, 1.26f, 0.45f, 0);

	AddClub("2I", 180.0f, 20.0f, 161.0f, 1.07f, 0.45f, 1);
	AddClub("3I", 170.0f, 24.0f, 149.0f, 0.95f, 0.45f, 1);
	AddClub("4I", 160.0f, 28.0f, 139.0f, 0.83f, 0.45f, 1);
	AddClub("5I", 150.0f, 32.0f, 131.0f, 0.73f, 0.45f, 1);
	AddClub("6I", 140.0f, 36.0f, 124.0f, 0.67f, 0.41f, 1);
	AddClub("7I", 130.0f, 40.0f, 118.0f, 0.61f, 0.36f, 1);
	AddClub("8I", 120.0f, 44.0f, 114.0f, 0.57f, 0.3f, 1);
	AddClub("9I", 110.0f, 48.0f, 110.0f, 0.53f, 0.25f, 1);

	AddClub("PW", 100.0f, 52.0f, 107.0f, 0.49f, 0.18f, 2);
	AddClub("SW", 80.0f, 56.0f, 93.0f, 0.42f, 0.17f, 2);

	AddClub("1PT", 20.0f, 0.0f, 30.0f, 0.0f, 0.0f, 3);
	AddClub("2PT", 10.0f, 0.0f, 21.0f, 0.0f, 0.0f, 3);

	// club.cpp:131
	m_unusedHead = 0;
	m_powerShot = 0;
	m_approachMode = 4;

	m_barAlpha = 0.0f;
	m_barPosition = 0.0f;
	m_bar = 2;
	m_itemWindow = false;
	m_prevClubIndex = 0;
	m_randomPower = 0.0f;
	m_calcRandomPower = false;
	m_pClub = m_clubList.Find("1W");
	m_clubIndex = m_pClub->m_index;
}

// club.cpp:147
void CClub::AddCrypticValue(void) {
	if (HackingManager::Instance()) {

		HackingManager::Instance()->AddCrypticIntValue(&m_powerShot);


		for (sClub *club = m_clubList.Start(); club; club = m_clubList.Next()) {

			HackingManager::Instance()->AddCrypticFloatValue(&club->m_range);
			HackingManager::Instance()->AddCrypticFloatValue(&club->m_lie);
			HackingManager::Instance()->AddCrypticFloatValue(&club->m_power);
			HackingManager::Instance()->AddCrypticFloatValue(&club->m_curve);
			HackingManager::Instance()->AddCrypticFloatValue(&club->m_spin);
			HackingManager::Instance()->AddCrypticFloatValue(&club->m_barSpeed);
		}
	}
}

// club.cpp:166
void CClub::Process(float time) {
	if (GOLFDOC()->m_gameState < 1)
		return;


	if (IsLocalContent((localContentType_t)0x4F) && WSingleton<CSharedDoc>::Instance()->m_playingMode == 2)
		return;


	if (IsMyTurn(false)) {

		ClubProcess(time);
		BarProcess(time);
	}


	if (m_prevClubIndex != m_clubIndex) {

		m_flags |= 1;

		if (GetType() != GetType(m_prevClubIndex))
			m_flags |= 2;
		else
			m_flags &= ~2;

		GOLFDOC()->GetBar().m_accuracy = GetType() == 3 ? BAR_START : 105.0f;
		GOLFDOC()->GetBar().m_prevAccuracy = GOLFDOC()->GetBar().m_accuracy;

		if (IsMyTurn(false) && (GOLFDOC()->GameMode() & 0x200) && m_clubIndex < m_prevClubIndex && m_approachMode == 4)
			g_camera = GOLFDOC()->MakeTopViewCameraMatrix(true);

		m_prevClubIndex = m_clubIndex;
	} else {


		m_flags &= ~3;
	}
}

// club.cpp:204
void CClub::ClubProcess(float time) {
	if (g_pFresh->GetManager()->GetWheelFocus() == NULL && !CQuitWindow::Instance()->IsActive() && CItemWindow::IsClosed() && !CChatMsg::Instance()->IsOpened() && !g_input->Get("LALT", true) && !g_input->Get("RALT", true)) {




		int playerState;
		AfxGetTask()->GetActor("Player") << MsgObject(0, 0x199, (intptr_t)&playerState, 0, 0, 0, 0);
		if (playerState == 3)
			return;




		if (GOLFDOC()->m_gameState == 10)
			return;


		WVector delta = g_input->GetMouseDelta();

		if (m_clubIndex < m_clubList.GetCount() - 1) {



			if (delta.z < 0.0f || ((g_input->GetDown("PAGE_DOWN", true) || g_input->GetDown("PAD_PLUS", true)) && !g_input->Get("LSHIFT", true) && !g_input->Get("RSHIFT", true)) || (g_input->GetDown("\xC1\xC2", true) && (g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true)) && !g_input->Get("LCONTROL", true) && !g_input->Get("RCONTROL", true))) {

				if (PowerShotType()) {

					m_pClub = SelectClub(m_clubIndex + 1);

					if (m_pClub == NULL || m_pClub->m_type == 3) {

						m_pClub = SelectClub(m_clubIndex);
						g_audio->PlaySfx("play_unplayable", 0, 0, 0, 0, 0.5f, 200.0f);
						CHelpMsg::Instance()->SetHelpMsg(0x12, NULL);
					} else {

						m_clubIndex++;
					}

				} else {

					m_clubIndex++;
					m_pClub = SelectClub(m_clubIndex);
				}


				if (S5::IsUsableCalipers() == true || IsUsedItembyMe(0x18000009) == true) {

					PerformPowerBarSetting();
				}

				g_audio->PlaySfx("\xC5\xAC\xB7\xB4\xB1\xB3\xC3\xBC", 0, 0, 0, 0, 0.5f, 200.0f);
			}
		}






		if (delta.z > 0.0f || ((g_input->GetDown("PAGE_UP", true) || g_input->GetDown("PAD_MINUS", true)) && !g_input->Get("LSHIFT", true) && !g_input->Get("RSHIFT", true)) || (g_input->GetDown("\xBF\xEC", true) && (g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true)) && !g_input->Get("LCONTROL", true) && !g_input->Get("RCONTROL", true))) {

			if (IsLocalContent((localContentType_t)0x2F)) {

				if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000028)
					m_approachClubIndex = 0;
			}


			if (m_clubIndex > m_approachClubIndex) {

				m_clubIndex--;
				m_pClub = SelectClub(m_clubIndex);

				g_audio->PlaySfx("\xC5\xAC\xB7\xB4\xB1\xB3\xC3\xBC", 0, 0, 0, 0, 0.5f, 200.0f);

				if (S5::IsUsableCalipers() == true || IsUsedItembyMe(0x18000009) == true) {

					PerformPowerBarSetting();
				}
			} else {



				if (CHelpMsg::Instance() && m_approachClubIndex > 0) {

					g_audio->PlaySfx("play_unplayable", 0, 0, 0, 0, 0.5f, 200.0f);
					CHelpMsg::Instance()->SetHelpMsg(0x10, NULL);
				}
			}
		}
	}
}

// club.cpp:290
void CClub::BarProcess(float time) {
	switch (m_bar) {
	case 0:

		if (m_flags & 1)
			m_barDelay = 0.0f;

		m_barDelay += time;
		if (m_barDelay > 0.7f || CChatMsg::Instance()->IsActive()) {




			m_bar = 1;
		}
		break;

	case 1:
		if ((m_flags & 1) && (!CChatMsg::Instance()->IsActive() || !m_itemWindow)) {
			m_bar = 3;
			m_barAlpha = 0.0f;
			m_barPosition = 456.0f;
			break;
		}

		m_barPosition -= time * 1000.0f;
		if (m_barPosition < 0.0f) {

			m_barPosition = 0.0f;
			m_bar = 2;

			if (OnlinePlay()) {

				WSendPacket packet(GCT_CLUB);
				packet.Encode1(m_clubIndex);
				packet.Send(SEND_TO_DEFAULT);
			}
		}
		break;

	case 3:
		m_barAlpha += time * 765.0f;
		if (m_barAlpha > 255.0f) {

			m_barDelay = 0.0f;

			m_bar = 0;
			m_barAlpha = 255.0f;
		}
		break;

	case 2:
		if ((m_flags & 1) && !CChatMsg::Instance()->IsActive() && !m_itemWindow) {

			if (GolfBall().m_groundType == 2) {

				if (OnlinePlay()) {

					WSendPacket packet(GCT_CLUB);
					packet.Encode1(m_clubIndex);
					packet.Send(SEND_TO_DEFAULT);
				}
			} else {
				m_bar = 3;
				m_barAlpha = 0.0f;
				m_barPosition = 456.0f;
			}
		}
		break;
	}
}

// club.cpp:374
void CClub::AddClub(char *name, float range, float lie, float power, float curve, float spin, unsigned char type) {

	sClub *club = m_clubList.Find(name);

	if (club) {

		club->m_range = range;
		club->m_lie = lie * 0.0174532925f;

		club->m_type = type;
		club->m_power = power;
		club->m_curve = curve;
		club->m_spin = spin;
	} else {





		club = new sClub;


		club->m_pointFactor = 1.0f;
		club->m_range = range;
		club->m_lie = lie * 0.0174532925f;
		club->m_type = type;
		club->m_power = power;
		club->m_curve = curve;
		club->m_spin = spin;
		club->m_index = (unsigned char)m_clubList.GetCount();

		strcpy(club->m_name, name);

		m_clubList.AddItem(club, club->m_name, false);

		for (unsigned char i = 0; i < 0x11; i++) {

			if (!strcmp(club->m_name, clubName[i])) {

				club->m_id = i;
				break;
			}
		}




		club->m_barSpeed = bar_speed[club->m_id];

		if (HackingManager::Instance()) {

			HackingManager::Instance()->AddCrypticFloatValue(&club->m_range);
			HackingManager::Instance()->AddCrypticFloatValue(&club->m_lie);
			HackingManager::Instance()->AddCrypticFloatValue(&club->m_power);
			HackingManager::Instance()->AddCrypticFloatValue(&club->m_curve);
			HackingManager::Instance()->AddCrypticFloatValue(&club->m_spin);
			HackingManager::Instance()->AddCrypticFloatValue(&club->m_barSpeed);
		}
	}
}

// club.cpp:431
CClub::sClub *CClub::SelectClub(unsigned char index) {
	sClub *club = m_clubList.Start();

	for (unsigned char i = 0; i < index; i++) {


		club = m_clubList.Next();
	}

	return club;
}

// club.cpp:445
void CClub::SelectClubByDist(float dist) {
	for (m_clubIndex = m_clubList.GetCount() - 1, m_pClub = m_clubList.End(); m_pClub; m_pClub = m_clubList.Prev(), m_clubIndex--) {

		if (m_clubIndex == m_approachClubIndex)
			break;


		if (PowerShotType() && m_pClub->m_type == 3)
			continue;


		if (GetRange(NULL) > dist)
			break;
	}

	if (m_pClub == NULL) {


		m_clubIndex = 0;
		m_pClub = m_clubList.Find("1W");
	}
}

// club.cpp:467
void CClub::SelectBestClub(void) {
	SelectBestClub(WVector(GOLFDOC()->GetHoleData().m_cupPos));

	if (S5::IsUsableCalipers()) {
		PerformPowerBarSetting();
	}
}

// club.cpp:477
// Picks the club for a shot at `target`: on the green a putter by distance;
// elsewhere the club whose range best fits the distance, scored by its bar
// speed and point factor, less what lies along the 1W line (hazards, out of
// bounds) when the drive is blocked; then the tee and game-state overrides.
void CClub::SelectBestClub(const WVector &target) {
	CGolfBall &ball = GolfBall();
	float dx = ball.m_pos.x - target.x;
	float dz = ball.m_pos.z - target.z;

	m_approachClubIndex = 0;
	float dist = (float)sqrt(dx * dx + dz * dz) * 0.3125f;

	if (dist < 10.0f)
		m_approachMode = 0;
	else if (dist < 15.0f)
		m_approachMode = 1;
	else if (dist < 28.0f)
		m_approachMode = 2;
	else if (dist < 58.0f)
		m_approachMode = 3;
	else
		m_approachMode = 4;

	SetAccuracy();

	if (GolfBall().m_groundType == 2) {
		switch (m_approachMode) {
		case 0:
			m_powerPutter = false;
			m_pClub = m_clubList.Find("2PT");
			break;
		case 1:
			m_powerPutter = false;
			m_pClub = m_clubList.Find("1PT");
			break;
		case 2:
			m_powerPutter = true;
			m_pClub = m_clubList.Find("2PT");
			break;
		case 3:
		case 4:
			m_powerPutter = true;
			m_pClub = m_clubList.Find("1PT");
			break;
		}

		m_clubIndex = m_pClub->m_index;
		m_approachClubIndex = 13;
		GOLFDOC()->GetBar().m_accuracy = 140.0f;
		GOLFDOC()->GetBar().m_prevAccuracy = GOLFDOC()->GetBar().m_accuracy;
	} else {
		m_powerPutter = true;
		float best = -10000.0f;
		int meshIndex = GetPVS().GetMeshIndex(GolfBall().m_pos, false);

		WVector dir = GOLFDOC()->m_pAutoTarget->GetPoint("1W") - GolfBall().m_pos;
		dir.y = 0.0f;
		dir.Normalize();

		bool blocked = false;
		if (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_level < 6
			&& (CSharedDoc::Instance()->m_pCurCourse == NULL || (CSharedDoc::Instance()->m_pCurCourse->courseFlags & 0xF) < 3)) {
			float range = GetRange("1W") * 3.2f;

			for (float f = 0.8f; f < 1.01f; f += 0.1f) {
				if (GetPVS().GetGroundType(dir * (range * f) + GolfBall().m_pos, true, NULL) == 10
					|| GOLFDOC()->m_pPolySoup->IsOutOfBound(dir * (range * f) + GolfBall().m_pos)) {
					blocked = true;
					break;
				}
			}
		}

		for (sClub *club = m_clubList.Start(); club; club = m_clubList.Next()) {
			if (club->m_index >= 13)
				continue;

			if ((float)club->m_lie < GetPVS().GetGroundSlope(meshIndex) + 0.06981317f) {
				m_approachClubIndex++;
				continue;
			}

			float range = GetRange(club->m_name);
			float score = (float)club->m_barSpeed * 0.5f - Abs(range - dist);

			if (range > dist)
				score += 10.0f;

			if (blocked) {
				float reach = range * 3.2f;

				for (float f = 0.8f; f < 1.01f; f += 0.1f) {
					if (GOLFDOC()->m_pPolySoup->IsOutOfBound(dir * (reach * f) + GolfBall().m_pos)) {
						score -= 1000.0f;
					} else {
						switch (GetPVS().GetGroundType(dir * (reach * f) + GolfBall().m_pos, true, NULL)) {
						case 3:
							score -= 200.0f;
							break;
						case 4:
						case 6:
						case 7:
						case 11:
							score -= 100.0f;
							break;
						case 10:
							score -= 1000.0f;
							break;
						}
					}
				}
			}

			float value = score * club->m_pointFactor;

			if (value > best) {
				m_pClub = club;
				best = value;
				m_clubIndex = club->m_index;
			}
		}

		if (m_approachMode < 4) {
			m_approachClubIndex = Max((unsigned char)7, m_approachClubIndex);
			SetClub((unsigned char)11);
		}

		if (GolfBall().m_groundType == 3)
			SetClub((unsigned char)12);
	}

	if (GolfBall().m_groundType == 0) {
		if (CSharedDoc::Instance()->m_roomType == 1) {
			if (TEAM(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_attribute)->m_holes[GOLFDOC()->m_currentHole - 1] <= 1 && m_clubIndex > 2)
				SetClub("3W");
		} else {
			if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_holes[GOLFDOC()->m_currentHole - 1] <= 1 && m_clubIndex > 2)
				SetClub("3W");
		}
	}

	if (GOLFDOC()->m_gameState == 10)
		SetClub("1W");

	m_prevClubIndex = m_clubIndex;
}

// club.cpp:652
void CClub::SetAccuracy(void) {
	m_accuracy = (int)PLAYER(GOLFDOC()->m_myPlayerIndex)->m_accuracy + ground_accuracy[GolfBall().m_groundType];
	float wide = m_accuracy + 10.0f;
	if (wide > 35.0f)
		wide = 35.0f;

	m_wideAccuracy = wide;

	if (IsLocalContent((localContentType_t)0x2F)) {

		if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId == 0x18000028)
			m_accuracy = (int)PLAYER(GOLFDOC()->m_myPlayerIndex)->m_accuracy + ground_accuracy[1];
	}
}

// club.cpp:664
void CClub::SetApproachMode(void) {
	WVector cup = GOLFDOC()->GetHoleData().m_cupPos;
	CGolfBall &ball = GolfBall();





	m_approachClubIndex = 7;
	float dx = ball.m_pos.x - cup.x;
	float dz = ball.m_pos.z - cup.z;
	float dist = (float)sqrt(dx * dx + dz * dz) * 0.3125f;

	if (dist < 10.0f) {
		m_powerPutter = false;
		m_approachMode = 0;
	} else if (dist < 15.0f) {

		m_powerPutter = false;
		m_approachMode = 1;

	} else {
		if (dist < 28.0f) {

			m_approachMode = 2;

		} else if (dist < 58.0f) {

			m_approachMode = 3;

		} else {

			m_approachMode = 4;
			m_approachClubIndex = 0;
		}
		m_powerPutter = true;
	}

	if (GetPVS().GetGroundType(GolfBall().m_pos, true, NULL) != 2)
		m_powerPutter = true;
}

// club.cpp:703
float CClub::GetNarrowAccuracy(void) {
	if (m_pClub->m_type == 3)
		return 0.0f;

	return Max((float)ceil(m_accuracy * (GolfClub().PowerShotType() ? 0.6f : 1.0f)), 2.0f);
}

// club.cpp:711
float CClub::GetWideAccuracy(void) {
	if (m_pClub->m_type == 3)
		return 0.0f;

	return Min(Max((float)ceil(m_accuracy * (GolfClub().PowerShotType() ? 0.6f : 1.0f)), 2.0f) + 10.0f, 35.0f);
}

// club.cpp:719
float CClub::GetRange(const char *name) {
	sClub *club;
	if (name)
		club = m_clubList.Find(name);
	else
		club = m_pClub;

	float range = club->m_range;

	switch (club->m_type) {
	case 0:
		// The air night club set (0x10000012) has its own wood ranges.
		if (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId == 0x10000012) {
			float luckyRange = GetAirNightLuckyRange(club->m_name);
			if (luckyRange > 0.0f) {
				int power = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_power;
				range = GetPowerShotFactor() + power + luckyRange;
			} else {
				range = club->m_range + GetPowerShotFactor(); range = (int)PLAYER(GOLFDOC()->m_myPlayerIndex)->m_power + range;
			}
		} else {
			range = range + GetPowerShotFactor(); range = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_power * 2.0f + range;
		}
		break;

	case 1:
		range = range + GetPowerShotFactor();
		break;

	case 2:
		switch (m_approachMode) {
		case 0:
		case 1:
		case 2:
			range = (PowerShotType() ? 30.0f : 0.0f) + 30.0f;
			break;
		case 3:
			range = (PowerShotType() ? 20.0f : 0.0f) + 60.0f;
			break;
		case 4:
			range = range + GetPowerShotFactor();
			break;
		}
		break;

	case 3:
		if (m_powerPutter) {
			range = Is("1PT", club) ? 40.0f : 30.0f;
		}
		break;
	}

	if (GetType() != 3) {
		range += CSharedDoc::Instance()->GetAuxPartProperty(GOLFDOC()->m_myPlayerIndex, 0);

		if (IsLocalContent((localContentType_t)0x70)) {
			float buff = (float)CSharedDoc::Instance()->GetBuffValue(GOLFDOC()->m_myPlayerIndex, 0x10);
			if (GolfClub().PowerShotType())
				buff += CSharedDoc::Instance()->GetBuffValue(GOLFDOC()->m_myPlayerIndex, 0x11);
			return range + buff;
		}

		CCardManager::Instance()->SetPlayerIndex(GOLFDOC()->m_myPlayerIndex);
		bool withPowerRange = false;
		if (GolfClub().PowerShotType())
			withPowerRange = true;
		return range + CCardManager::Instance()->GetTotalRange(withPowerRange);
	}

	return range;
}

// club.cpp:827
float CClub::GetLie(const char *name) {
	sClub *club;
	if (name)
		club = m_clubList.Find(name);
	else
		club = m_pClub;

	if (m_approachMode == 4)
		return club->m_lie;

	return club->m_lie + GolfBall().m_lieAngle * (3.14159265f / 45.0f);
}

// club.cpp:841
float CClub::GetPower(void) {
	float power = m_pClub->m_power;
	float aux = CSharedDoc::Instance()->GetAuxPartProperty(GOLFDOC()->m_myPlayerIndex, 0);
	float powerShot = 0.0f;

	if (GolfClub().PowerShotType()) {

		powerShot = GetPowerShotFactor() + m_randomPower;
	}

	if (IsLocalContent((localContentType_t)0x70)) {


		float buff = (float)CSharedDoc::Instance()->GetBuffValue(GOLFDOC()->m_myPlayerIndex, 0x10);
		if (GolfClub().PowerShotType())
			buff += CSharedDoc::Instance()->GetBuffValue(GOLFDOC()->m_myPlayerIndex, 0x11);
		aux += buff;

	} else {


		CCardManager::Instance()->SetPlayerIndex(GOLFDOC()->m_myPlayerIndex);
		bool withPowerRange = false;
		if (GolfClub().PowerShotType())
			withPowerRange = true;

		aux += CCardManager::Instance()->GetTotalRange(withPowerRange);
	}

	float range;
	switch (m_pClub->m_type) {
	case 0:






		if (CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_clubSet.typeId == 0x10000012) {


			range = GetAirNightLuckyRange(m_pClub->m_name);
			power = GetAirNightLuckyPower(m_pClub->m_name);
			if (range > 0.0f && power > 0.0f) {


				return (((int)PLAYER(GOLFDOC()->m_myPlayerIndex)->m_power + aux * 0.5f + powerShot) / range * 1.5f + 1.0f) * power;

			} else {


				return (((int)PLAYER(GOLFDOC()->m_myPlayerIndex)->m_power + aux * 0.5f + powerShot) / m_pClub->m_range * 1.5f + 1.0f) * m_pClub->m_power;
			}
		}






		return ((((int)PLAYER(GOLFDOC()->m_myPlayerIndex)->m_power + aux * 0.5f) * 2.0f + powerShot) / m_pClub->m_range * 1.5f + 1.0f) * m_pClub->m_power;




	case 1:
		power = (powerShot / m_pClub->m_range + 1.0f) * m_pClub->m_power;
		return power + m_pClub->m_power / m_pClub->m_range * aux * 1.3f;







	case 2:
		switch (m_approachMode) {




		case 0:
		case 1:
		case 2:
			power = ((PowerShotType() ? 28.0f : 0.0f) + 52.0f) * (GetLie(NULL) * 0.51156944f + 0.5f);
			break;
		case 3:
			power = ((PowerShotType() ? 18.0f : 0.0f) + 80.0f) * (GetLie(NULL) * 0.51156944f + 0.5f);
			break;
		case 4:
			power = (powerShot / m_pClub->m_range + 1.0f) * m_pClub->m_power;
			break;
		}



		return power + m_pClub->m_power / m_pClub->m_range * aux;



	case 3:
		if (m_powerPutter)
			return strstr(m_pClub->m_name, "1PT") ? 42.0f : 36.0f;

		return m_pClub->m_power;
	}

	return power;
}

// club.cpp:950
void CClub::ClearPointFactor(void) {
	for (sClub *club = m_clubList.Start(); club; club = m_clubList.Next())
		club->m_pointFactor = 1.0f;
}

// club.cpp:957
void CClub::SetPointFactor(float factor) {
	m_pClub->m_pointFactor = factor;
}

// club.cpp:962
unsigned char CClub::GetType(unsigned char index) {
	for (sClub *club = m_clubList.Start(); club; club = m_clubList.Next()) {
		if (club->m_index == index)
			return club->m_type;
	}

	return 0;
}

// club.cpp:977
void CClub::SetClub(const char *name) {
	m_pClub = m_clubList.Find(name);
	m_clubIndex = m_pClub->m_index;
}

// club.cpp:983
void CClub::SetClub(unsigned char index) {
	m_pClub = SelectClub(index);
	m_clubIndex = m_pClub->m_index;
}

// Nameless per-unit rdata constant: degrees to radians.
extern "C" {
static const float data_9eb1f0 = 0.017453292f;
}

// club.cpp:990
// Returns the club set of a typeid, loading it the first time: the normal
// and HUD preview models of its four clubs, and for each club type the
// preview camera (from the model's "camera" and "target" bound boxes, rolled
// by the camera's "*angle") and a directional light along the camera.
sClubSet *CClub::AddClubSet(unsigned long typeId) {
	sClubSet *clubSet = FindClubSet(typeId);

	if (clubSet != NULL)
		return clubSet;

	IFF_STRUCT::sClubSet *pItem = ItemManager()->FindClubSet(typeId);

	if (pItem != NULL) {
		clubSet = new sClubSet;

		clubSet->m_pItem = pItem;

		for (int i = 0; i < 4; i++) {
			IFF_STRUCT::sClub *pClub = ItemManager()->FindClub(pItem->m_clubTypeIds[i]);

			clubSet->m_puppet[0][i] = g_resrcmng->GetPuppet(MakeStr("%s.pet", pClub->m_model), false, true, false);

			clubSet->m_puppet[0][i]->CalcBound(NULL, true);

			clubSet->m_puppet[1][i] = g_resrcmng->GetPuppet(MakeStr("%s.pet", pClub->m_model), false, true, false);

			clubSet->m_puppet[1][i]->CalcBound(NULL, true);

			w_bound_box *camera = clubSet->m_puppet[0][i]->m_bbList.Find("camera");
			w_bound_box *target = clubSet->m_puppet[0][i]->m_bbList.Find("target");

			clubSet->m_lights[i].diffuse = 0xFFFFFF;
			clubSet->m_lights[i].ambient = 0x808080;
			clubSet->m_lights[i].ambient2 = 0xFFFFFF;
			clubSet->m_lights[i].type = LightSet::W_LIGHT_DIRECTIONAL;

			if (camera != NULL && target != NULL) {
				float angle = 0.0f;
				WSliceStr slice(camera->info->option);
				slice.Get("*angle", "%f", &angle);

				clubSet->m_cameras[i].pivot = camera->info->spherePivot;
				clubSet->m_cameras[i].za = (target->info->spherePivot - camera->info->spherePivot).Normalize();
				clubSet->m_cameras[i].xa = WCrossProduct(WVector(0.0f, cosf(angle * data_9eb1f0), sinf(angle * data_9eb1f0)), clubSet->m_cameras[i].za).Normalize();
				clubSet->m_cameras[i].ya = WCrossProduct(clubSet->m_cameras[i].za, clubSet->m_cameras[i].xa);

				clubSet->m_lights[i].nearone = clubSet->m_cameras[i].ya * 10.0f + clubSet->m_cameras[i].pivot - target->info->spherePivot;
			} else {
				clubSet->m_cameras[i].Reset();

				clubSet->m_lights[i].nearone = clubSet->m_cameras[i].ya * 10.0f + clubSet->m_cameras[i].pivot;
			}

			clubSet->m_lights[i].nearone.Negate();
			clubSet->m_lights[i].nearone.Normalize();
		}

		m_clubSetList.push_back(clubSet);
	}

	return clubSet;
}

// club.cpp:1076
sClubSet *CClub::FindClubSet(unsigned long typeId) {


	std::list<sClubSet *>::iterator it = std::find_if(m_clubSetList.begin(), m_clubSetList.end(), sFindClubSet(typeId));

	if (it != m_clubSetList.end())
		return *it;

	return NULL;
}

// club.cpp:1088
float CClub::GetPowerShotFactor(void) {
	float factor[4] = {
		0.0f,
		10.0f,
		20.0f,
		15.0f,
	};

	return factor[m_powerShot];
}

// club.cpp:1098
float CClub::GetAirNightLuckyRange(char *name) {
	float range = 0.0f;

	if (!stricmp(name, "1W"))
		range = 216.0f;
	else if (!stricmp(name, "2W"))
		range = 206.0f;
	else if (!stricmp(name, "3W"))
		range = 196.0f;

	return range;
}

// club.cpp:1115
float CClub::GetAirNightLuckyPower(char *name) {
	float power = 0.0f;

	if (!stricmp(name, "1W"))
		power = 216.0f;
	else if (!stricmp(name, "2W"))
		power = 194.0f;
	else if (!stricmp(name, "3W"))
		power = 176.0f;

	return power;
}

// club.cpp:1129
const char *CClub::GetClubString(eClub club) {
	switch (club) {
	case WOOD_1: return "WOOD_1";
	case WOOD_2: return "WOOD_2";
	case WOOD_3: return "WOOD_3";
	case IRON_2: return "IRON_2";
	case IRON_3: return "IRON_3";
	case IRON_4: return "IRON_4";
	case IRON_5: return "IRON_5";
	case IRON_6: return "IRON_6";
	case IRON_7: return "IRON_7";
	case IRON_8: return "IRON_8";
	case IRON_9: return "IRON_9";
	case WEDGE_P: return "WEDGE_P";
	case WEDGE_S: return "WEDGE_S";
	case PUTTER_1: return "PUTTER_1";
	case PUTTER_2: return "PUTTER_2";
	}

	return "NOT DEFINED";
}
