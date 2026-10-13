// Original: d:\build\custom\temp\code\client\projectg\golfbg.cpp
#include "golfbg.h"
#include "autotarget.h"
#include "ObjectFactory.h"
#include "SharedDoc.h"
#include "scenemanager.h"
#include "quadtree.h"
#include "ClientSetting.h"
#include "GolfDoc.h"
#include "Shadow.h"
#include "sky.h"
#include "wresrcmng.h"
#include "shared/classdefine.h"
#include "shared/Localize.h"
#include "polysoup.h"
#include "golfball.h"
#include "fx.h"
#include "TaskManager.h"
#include "Packet.h"
#include "commonutil.h"
#include "wutil.h"
#include "wind.h"

extern bool g_bQuit;
extern unsigned long g_CurrentTime;

extern LightSet g_lightset;

// golfbg.cpp:20
IObject* CGolfBgMakeInstance(void) {
	return new CGolfBg;
}

// golfbg.cpp:20 (run-time type and factory registration of CGolfBg)
const WRTTI CGolfBg::m_RTTI("CGolfBg", &IActor::m_RTTI);

struct __impCGolfBg {
	__impCGolfBg(void) {
		ObjectFactory().AddObjectFunctor(CGolfBgMakeInstance, "CGolfBg");
		ActorFactory().AddActorClass("CGolfBg");
	}
};
static __impCGolfBg s_impCGolfBg;

// golfbg.cpp:23
CGolfBg::CGolfBg(void) {
	m_priority = 1;

	m_renderWeather = 0;
}

// golfbg.cpp:31
void CGolfBg::OnDestroy(void) {
	if (CShadowManager::Instance()) {
		delete CShadowManager::Instance();
	}
}

// golfbg.cpp:37
void CGolfBg::DoReLoad(void) {
	SendMsg(this, "Ambient", 0x1F4, 0, 0, 0, 0);

	if (CFx::Instance()) {
		CFx::Instance()->SetActive(false);
	}

	int hole = GOLFDOC()->m_currentHole;
	if (CSharedDoc::Instance()->m_roomInfo.m_roomMode == 0xE) {
		GOLFDOC()->m_pAutoTarget->Reset();
		if (!GOLFDOC()->m_pPolySoup->Load(MakeStr("%s_%02d.gbin", CSharedDoc::Instance()->m_pCurCourse ? CSharedDoc::Instance()->m_pCurCourse->m_mapName : 0, hole), false)) {
			g_bQuit = true;
			return;
		}

		GOLFDOC()->m_waveInfo.Load(MakeStr("%s_wave.txt", CSharedDoc::Instance()->m_pCurCourse ? CSharedDoc::Instance()->m_pCurCourse->m_mapName : 0));
		GOLFDOC()->m_riverInfo.Load(MakeStr("%s_river.txt", CSharedDoc::Instance()->m_pCurCourse ? CSharedDoc::Instance()->m_pCurCourse->m_mapName : 0));

		unsigned char course = CSharedDoc::Instance()->m_holeCourse[17];
		IFF_STRUCT::sCourse *pCourse = ItemManager()->FindCourse(0x28000000 | ((course > 0x7F && course != 0xFD) ? course - 0x80 : course));
		if (pCourse) {
			char *name = pCourse->m_mapName;

			if (name) {
				if (GOLFDOC()->m_pSky) {
					GOLFDOC()->m_pSky->Clear();
					delete GOLFDOC()->m_pSky;
					GOLFDOC()->m_pSky = 0;
				}

				GOLFDOC()->m_pSky = new CSky;

				GOLFDOC()->m_pSky->LoadSky(name, 0);
				GOLFDOC()->m_pSky->RegisterCloud(MakeStr("%s_cloud.dds", name));
			}
		}

		GOLFDOC()->m_pPolySoup->LoadMapCheckData(hole);
		CSceneManager::Instance()->SetWeather(CSharedDoc::Instance()->m_weather, true);
	} else {
		if (!GOLFDOC()->m_pPolySoup->Load(MakeStr("%s_%02d.gbin", CSharedDoc::Instance()->m_pCurCourse ? CSharedDoc::Instance()->m_pCurCourse->m_mapName : 0, hole), false)) {
			g_bQuit = true;
			return;
		}

		if (GOLFDOC()->m_pSky) {
			GOLFDOC()->m_pSky->Clear();
			delete GOLFDOC()->m_pSky;
			GOLFDOC()->m_pSky = 0;
		}

		GOLFDOC()->m_pSky = new CSky;

		if (GOLFDOC()->m_gameState < 0xF) {
			GOLFDOC()->m_pSky->LoadSky("blue", 0);
			GOLFDOC()->m_pSky->RegisterCloud("blue");
		} else {
			GOLFDOC()->m_pSky->LoadSky(MakeStr("%s", GetSkyBoxName()), 0);
			GOLFDOC()->m_pSky->RegisterCloud(MakeStr("%s_cloud.dds", GetSkyBoxName()));
		}

		GOLFDOC()->m_pPolySoup->LoadMapCheckData(hole);
	}

	DoInitAfterLoad();

	if (CFx::Instance()) {
		CFx::Instance()->SetActive(true);
	}

	GetActor("GroundItemMan") << MsgObject(this, 0x27A, 0, 0, 0, 0, 0);

	AfxGetTask()->Init(0);

	if (OnlinePlay()) {
		WSendPacket packet(GCT_LOAD_OK);
		packet.Send(SEND_TO_DEFAULT);
	} else {
		SendMsg(this, "GolfRule", 0x81, 0, 0, 0, 0);
	}
}

// golfbg.cpp:150
void CGolfBg::DoInitAfterLoad(void) {
	GOLFDOC()->m_pPolySoup->MakeDataStructure();
	SendMsg(this, "Camera", 0xEF, 0, 0, 0, 0);

	WPolySoup::sPoint *point;
	if (GetDiffFlag() == 1) {
		point = GOLFDOC()->m_pPolySoup->FindPoint("\xB1\xD7\xB8\xAE\xB5\xE5");
	} else {
		point = GOLFDOC()->m_pPolySoup->FindPoint("\xB1\xD7\xB8\xAE\xB5\xE5" "2");
		if (!point)
			point = GOLFDOC()->m_pPolySoup->FindPoint("\xB1\xD7\xB8\xAE\xB5\xE5");
	}
	SetGridCenter(point->m_pos);

	SetStartPoint(GOLFDOC()->GetHoleData().m_teePos);
	SetHolePoint(GOLFDOC()->GetHoleData().m_cupPos);

	SendMsg(this, "GolfBg", 0x142, GetWeather(), 0, 0, 0);

	if (GOLFDOC()->m_pSky) {
		GOLFDOC()->m_pSky->GenerateStars(-1);
		GOLFDOC()->m_pSky->GenerateLensFlare();
		GOLFDOC()->m_pSky->GenerateMoons();
	}

	CShadowManager::Instance()->UpdateLight("sun", g_lightset);

	if (!OnlinePlay()) {
		float radius = GolfBall().m_radius; WVector tee = GOLFDOC()->GetHoleData().m_teePos; GolfBall().Reset(WVector(tee.x, tee.y + radius, tee.z));
	}

	CSceneManager::Instance()->Load();

	GOLFDOC()->m_pAutoTarget->Reset();

	for (std::vector<WPolySoup::sBoundArea>::iterator it = GOLFDOC()->m_pPolySoup->m_boundAreas.begin(); it != GOLFDOC()->m_pPolySoup->m_boundAreas.end(); ++it) {
		if (it->GetType() == 4) {
			WVector cup = GOLFDOC()->GetHoleData().m_cupPos;
			GOLFDOC()->m_pAutoTarget->SetList(it->GetList(), cup);
		}
	}

	GOLFDOC()->m_pPolySoup->EndLoad(true);
}

// golfbg.cpp:208
void CGolfBg::OnPreLoadInit(void) {
	new CShadowManager(g_resrcmng, COption::Instance()->vIsShadowEnabled());
}

// golfbg.cpp:213
void CGolfBg::OnLoad(void) {
	if (CFx::Instance()) {
		CFx::Instance()->SetActive(false);
	}

	GOLFDOC()->m_pAutoTarget = new CAutoTarget;

	if (CSharedDoc::Instance()->m_roomInfo.m_roomMode == 0xE) {
		unsigned char i;
		for (i = 0; i < 18;) {
			if (CSharedDoc::Instance()->m_holeOrder[i++] == GOLFDOC()->m_currentHole) break;
		}
		i = Between<unsigned char>(1, i, 18);
		SetCurMap(CSharedDoc::Instance()->m_holeCourse[i - 1]);
	}

	if (!GOLFDOC()->m_pPolySoup->Load(MakeStr("%s_%02d.gbin", CSharedDoc::Instance()->m_pCurCourse ? CSharedDoc::Instance()->m_pCurCourse->m_mapName : 0, GOLFDOC()->m_currentHole), false)) {
		g_bQuit = true;
		return;
	}

	GOLFDOC()->m_waveInfo.Load(MakeStr("%s_wave.txt", CSharedDoc::Instance()->m_pCurCourse ? CSharedDoc::Instance()->m_pCurCourse->m_mapName : 0));
	GOLFDOC()->m_riverInfo.Load(MakeStr("%s_river.txt", CSharedDoc::Instance()->m_pCurCourse ? CSharedDoc::Instance()->m_pCurCourse->m_mapName : 0));

	GOLFDOC()->m_pSky = new CSky;

	if (GOLFDOC()->m_gameState < 0xF) {
		GOLFDOC()->m_pSky->LoadSky("blue", 0);
		GOLFDOC()->m_pSky->RegisterCloud("blue");
	} else if (CSharedDoc::Instance()->m_roomInfo.m_gameType == 0xE || CSharedDoc::Instance()->m_roomInfo.m_roomMode == 0xE) {
		unsigned char course = CSharedDoc::Instance()->m_holeCourse[17];
		IFF_STRUCT::sCourse *pCourse = ItemManager()->FindCourse(0x28000000 | ((course > 0x7F && course != 0xFD) ? course - 0x80 : course));
		if (pCourse) {
			char *name = pCourse->m_mapName;

			if (name) {
				GOLFDOC()->m_pSky->LoadSky(name, 0);
				GOLFDOC()->m_pSky->RegisterCloud(MakeStr("%s_cloud.dds", name));
			}
		}
	} else {
		GOLFDOC()->m_pSky->LoadSky(MakeStr("%s", GetSkyBoxName()), 0);
		GOLFDOC()->m_pSky->RegisterCloud(MakeStr("%s_cloud.dds", GetSkyBoxName()));
	}

	DoInitAfterLoad();

	if (CFx::Instance()) {
		CFx::Instance()->SetActive(true);
	}
}

// golfbg.cpp:280
void CGolfBg::OnInit(void) {
	CSceneManager::Instance()->UpdateFog(true);
}

// golfbg.cpp:285
void CGolfBg::HandleMsg(const MsgObject& msg) {
	if ((unsigned long)msg.m_reserved > g_CurrentTime) {
		StoreDelayedMsg(msg);
		return;
	}

	switch (msg.m_msg) {
	case 0x1E8:
		if (!GOLFDOC()->m_pPolySoup->Load(MakeStr("%s_%02d.gbin", CSharedDoc::Instance()->m_pCurCourse ? CSharedDoc::Instance()->m_pCurCourse->m_mapName : 0, GOLFDOC()->m_currentHole), false)) {
			g_bQuit = true;
			return;
		}

		DoInitAfterLoad();

		AfxGetTask()->Init(0);

		SendMsg(this, "GolfRule", 0x81, 0, 0, 0, 0);
		break;

	case 0x142:
		SetRenderWeather((unsigned char)msg.m_param1);
		CSceneManager::Instance()->SetWeather(GetRenderWeather(), msg.m_param2 != 0);
		CSharedDoc::Instance()->m_weather = (unsigned char)msg.m_param1;
		break;

	case 0x13D: {
		int weather = msg.m_param1;

		if (weather > 1) {
			GOLFDOC()->m_pSky->ClearSky();
		}
		GOLFDOC()->m_pSky->GenerateClouds(0x32, Wind().GetGlobalIntensity());
		GOLFDOC()->m_pSky->ChangeWeatherTex(weather);
		GOLFDOC()->m_pSky->ShowClouds(weather > 0);
		break;
	}

	case 0x2712: {
		char text[256];
		strcpy(text, (const char*)msg.m_param1);
		break;
	}

	case 0x13E:
		GOLFDOC()->m_pSky->LoadSky(MakeStr("%s", GetSkyBoxName()), 0);
		GOLFDOC()->m_pSky->RegisterCloud(MakeStr("%s_cloud.dds", GetSkyBoxName()));

		GOLFDOC()->m_pSky->GenerateMoons();
		GOLFDOC()->m_pSky->GenerateStars(-1);
		GOLFDOC()->m_pSky->GenerateLensFlare();
		break;

	case 0x13F:
		GOLFDOC()->m_pSky->SetColor(msg.m_param1);
		break;

	case 0x140:
		DoReLoad();
		break;

	case 0x1E5: {
		const std::vector<WVector>& list = GOLFDOC()->m_pAutoTarget->GetPointList();
		std::vector<WVector>* pOut = (std::vector<WVector>*)msg.m_param1;

		unsigned int count = list.size() * 4 + 7;

		if (pOut->capacity() < count) {
			pOut->reserve(count);
		} else {
			pOut->clear();
		}

		WVector tee; tee = GetTeeShotPos();
		tee.y = GetPVS().GetGroundPoint(tee, false, 0, false).y + 100.0f;
		WVector first; first = list[0];
		first.y = GetPVS().GetGroundPoint(first, true, 0, true).y + 100.0f;
		WVector dir = tee - first; WVector side = tee + WVector(dir).Normalize() * 250.0f;

		pOut->push_back((side + tee) * 0.5f);
		pOut->push_back(tee);
		pOut->push_back((first + tee) * 0.5f);
		pOut->push_back(first);

		WVector point;
		int i;
		for (i = 1; i < (int)list.size() - 1; i++) {
			point = (list[i - 1] + list[i]) * 0.5f;
			point.y = GetPVS().GetGroundPoint(point, true, 0, true).y + 100.0f;
			pOut->push_back(point);

			point = list[i];
			point.y = GetPVS().GetGroundPoint(point, true, 0, true).y + 100.0f;
			pOut->push_back(point);
		}

		float up = rand() % 2 ? 1.0f : -1.0f;

		WVector across = WCrossProduct(WVector(0.0f, up, 0.0f), list.back() - list[i - 1]);
		across.Normalize();
		across *= 30.0f;

		WVector last = list.back() + across;
		last.y = GetPVS().GetGroundPoint(last, false, 0, false).y + 100.0f;
		WVector prev = list[i - 1];
		prev.y = GetPVS().GetGroundPoint(prev, true, 0, true).y + 100.0f;
		point = WVector(last - prev).Normalize() * 250.0f;
		side = point + last;
		across = last + WCrossProduct(WVector(0.0f, -up, 0.0f), point);

		pOut->push_back((last + prev) * 0.5f);
		pOut->push_back(last);
		pOut->push_back((last + side) * 0.5f);
		pOut->push_back(side);
		pOut->push_back((across + side) * 0.5f);
		pOut->push_back(across);
		pOut->push_back((prev + across) * 0.5f);
		pOut->push_back(prev);

		for (int j = (int)list.size() - 3; j >= 0; j--) {
			point = (list[j + 1] + list[j]) * 0.5f;
			point.y = GetPVS().GetGroundPoint(point, true, 0, true).y + 100.0f;
			pOut->push_back(point);

			point = list[j];
			point.y = GetPVS().GetGroundPoint(point, true, 0, true).y + 100.0f;
			pOut->push_back(point);
		}

		point = WVector(dir).Normalize() * 250.0f;
		side = point + tee;
		across = tee + WCrossProduct(point, WVector(0.0f, rand() % 2 ? 1.0f : -1.0f, 0.0f));

		pOut->push_back((across + first) * 0.5f);
		pOut->push_back(across);
		pOut->push_back((across + side) * 0.5f);
		pOut->push_back(side);
		pOut->push_back((side + tee) * 0.5f);
		break;
	}

	case 0x118:
		GOLFDOC()->m_pPolySoup->SetRenderMode(msg.m_param1);
		break;

	case 0x1E7:
		if (!GOLFDOC()->m_pPolySoup->Load(MakeStr("%s_rank.gbin", CSharedDoc::Instance()->m_pCurCourse ? CSharedDoc::Instance()->m_pCurCourse->m_mapName : 0), false)) {
			g_bQuit = true;
			return;
		}

		GOLFDOC()->m_pPolySoup->EndLoad(false);
		GOLFDOC()->m_pPolySoup->MakeDataStructure();
		SetLowestPoint();

		CShadowManager::Instance()->UpdateLight("sun", g_lightset);

		GOLFDOC()->m_pSky->LoadSky(GetSkyBoxName(), 0);
		GOLFDOC()->m_pSky->ClearClouds();
		GOLFDOC()->m_pSky->GenerateLensFlare();
		GOLFDOC()->m_pSky->GenerateMoons();
		GOLFDOC()->m_pSky->GenerateStars(-1);

		CSceneManager::Instance()->Load();
		CSceneManager::Instance()->UpdateFog(true);

		SendMsg(this, "Effect", 0x17B, 0, 0, 0, 0);
		SendMsg(this, "Caddie", 0x164, 0, 0, 0, 0);
		break;

	case 0x141:
		if (GOLFDOC()->m_pPolySoup) {
			GOLFDOC()->m_pPolySoup->UpdateBgFxWind();
		}
		break;
	}
}

// golfbg.cpp:500
void CGolfBg::SetStartPoint(const WVector& point) {
	GOLFDOC()->GetHoleData().m_teePos = GetPVS().GetGroundPoint(point, false, &GolfBall().m_groundArea, false);
}

// golfbg.cpp:505
void CGolfBg::SetHolePoint(const WVector& point) {
	GOLFDOC()->GetHoleData().m_cupPos = GetPVS().GetGroundPoint(point, false, 0, false);
}

// golfbg.cpp:510
void CGolfBg::SetGridCenter(WVector& center) {
	center = GetPVS().GetGroundPoint(center, false, 0, false);

	SendMsg(this, "Grid", 0x74, (intptr_t)&center, 0, 0, 0);
	SendMsg(this, "GolfRule", 0x74, (intptr_t)&center, 0, 0, 0);
}

// golfbg.cpp:518
void CGolfBg::SetSkyColor(unsigned long color) {
	if (GOLFDOC()->m_pSky) {
		GOLFDOC()->m_pSky->SetColor(color);
	}
}

// golfbg.cpp:524
void CGolfBg::SetLowestPoint(void) {
	SendMsg(this, "Camera", 0xEF, 0, 0, 0, 0);
}

// golfbg.cpp:529
const char* CGolfBg::GetSkyBoxName(void) {
	if ((CSharedDoc::Instance()->m_pCurCourse ? CSharedDoc::Instance()->m_pCurCourse->m_mapName : 0) == 0) {
		return "blue";
	}

	bool halloween = IsLocalContent((localContentType_t)0x9A);

	if (!halloween) {
		return CSharedDoc::Instance()->m_pCurCourse ? CSharedDoc::Instance()->m_pCurCourse->m_mapName : 0;
	}
	return "Halloween";
}

// golfbg.cpp:549
void CGolfBg::SetRenderWeather(unsigned char weather) {
	m_renderWeather = weather;

	if (IsLocalContent((localContentType_t)0x9A)) {
		if (CSharedDoc::Instance()->m_roomType < 0xB) {
			if (m_renderWeather <= 0) {
				m_renderWeather = 1;
			}

			if (GOLFDOC()->m_pPolySoup) {
				GOLFDOC()->m_pPolySoup->ChangePointType(2, 3);
			}
		}
	}
}
