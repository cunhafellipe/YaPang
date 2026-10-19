// Original: d:\build\custom\temp\code\client\projectg\npc.cpp
#include "NetworkSystem.h"
#include "npc.h"
#include "ObjectFactory.h"
#include "wmath.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "polysoup.h"
#include "wresrcmng.h"
#include "golfball.h"
#include "quadtree.h"
#include "wpuppet.h"
#include "wview.h"
#include "commonutil.h"
#include "ClientSetting.h"
#include "wutil.h"
#include "slicestr.h"
#include "fx.h"
#include "wbone.h"
#include "wlist.h"
#include "SoundManager.h"
#include <string.h>
#include <stdio.h>
#include <string>
#include <algorithm>
#include <float.h>
#include <malloc.h>

float Random(float min, float max, int seed);  // commonutil.cpp

extern WMatrix g_camera;

// ?g_lightset@@3ULightSet@@A; its header (wscene.h) does not declare it yet.
extern LightSet g_lightset;

// One degree in radians, a file-static float of the original (read from
// memory, not folded); its name is not known. wmath.h's g_DEGTORAD has the
// value but is referenced under its own name, which check_diff does not pair.
static float data_9f0fc0 = 0.017453292f;

// FLT_MAX, a file-static float of the original (FindParameterByDistance reads
// it from memory); its name is not known.
static float data_9f0fc4 = FLT_MAX;

// npc.cpp:24 (run-time type and factory registration of CNPC)
const WRTTI CNPC::m_RTTI("CNPC", &IActor::m_RTTI);

IObject *CNPCMakeInstance(void) {
	return new CNPC;
}

struct __impCNPC {
	__impCNPC(void) {
		ObjectFactory().AddObjectFunctor(CNPCMakeInstance, "CNPC");
		ActorFactory().AddActorClass("CNPC");
	}
};
static __impCNPC s_impCNPC;

// npc.cpp:27
void CNPC::OnDestroy(void) {
	Destroy();
}

// npc.cpp:32
void CNPC::Destroy(void) {
	for (std::list<CCreature *>::iterator it = m_creatures.begin(); it != m_creatures.end(); ++it) {
		if (*it) { delete *it; *it = 0; }
	}
	m_creatures.clear();
}

// npc.cpp:41
void CNPC::OnInit(void) {
	Destroy();

	for (int i = 0; i < GOLFDOC()->m_pPolySoup->GetAreaNum(); i++) {
		WPolySoup::sArea *area = GOLFDOC()->m_pPolySoup->GetArea(i);

		if (area->m_type == 1)
			SetNPC(area->m_aabb, area->m_name);
	}

	for (std::vector<CNodeContainer>::iterator it = GOLFDOC()->m_pPolySoup->m_boundAreas.begin(); it != GOLFDOC()->m_pPolySoup->m_boundAreas.end(); ++it) {
		if (it->GetType() == (CNodeContainer::eType)0xC)
			SetNPC(it->GetList(), it->GetName().c_str());
	}
}

// npc.cpp:76
void CNPC::OnProcess(float dt) {
	for (std::list<CCreature *>::iterator it = m_creatures.begin(); it != m_creatures.end(); ++it) {
		(*it)->Process(dt);
	}
}

// npc.cpp:87
void CNPC::HandleMsg(const MsgObject &msg) {
	switch (msg.m_msg) {
	case 392: // is there a creature of type param1? answer through param2
		for (std::list<CCreature *>::iterator it = m_creatures.begin(); it != m_creatures.end(); ++it) {
			if ((*it)->GetType() == msg.m_param1) {
				*(int *)msg.m_param2 = 1;
				break;
			}
		}
		break;

	case 386: // switch off the active creatures
		for (std::list<CCreature *>::iterator it = m_creatures.begin(); it != m_creatures.end(); ++it) {
			if (((*it)->m_flags & 1) && (*it)->GetType() != 9) {
				if ((*it)->GetActive())
					(*it)->SetActive(false);
			}
		}
		break;

	case 387: // play the event of the creatures whose area holds the ball
		for (std::list<CCreature *>::iterator it = m_creatures.begin(); it != m_creatures.end(); ++it) {
			if ((*it)->m_flags & 1) {
				Waabb area = (*it)->GetArea();
				if (area.IsInclude(GolfBall().m_pos)) {
					(*it)->PlayEvent();
					break;
				}
			}
		}
		break;

	case 388: // param1 = whether the ball is inside the area of a creature
		for (std::list<CCreature *>::iterator it = m_creatures.begin(); it != m_creatures.end(); ++it) {
			if ((*it)->m_flags & 1) {
				Waabb area = (*it)->GetArea();
				if (area.IsInclude(GolfBall().m_pos)) {
					*(int *)msg.m_param1 = 0;
					break;
				}
			}
		}
		break;

	case 390:
		for (std::list<CCreature *>::iterator it = m_creatures.begin(); it != m_creatures.end(); ++it) {
			if (!((*it)->m_flags & 1))
				(*it)->Record();
		}
		break;

	case 389:
		for (std::list<CCreature *>::iterator it = m_creatures.begin(); it != m_creatures.end(); ++it) {
			(*it)->Replay();
		}
		break;

	case 394:
		for (std::list<CCreature *>::iterator it = m_creatures.begin(); it != m_creatures.end(); ++it) {
			if ((*it)->GetType() == 9)
				(*it)->PlayEvent();
		}
		break;
	}
}

// npc.cpp:189
void CNPC::SetNPC(const std::vector<WVector> &points, const char *name) {
	CCreature *creature;

	if (strstr(name, "santa")) {
		if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
			CSharedDoc *shared = WSingleton<CSharedDoc>::Instance();
			if (shared->m_roomType != 0 || shared->m_gameMode != 0)
				return;
			// m_holeRandom is indexed by the 1-based hole index here
			if (shared->m_holeRandom[GetHoleIndex(GOLFDOC()->m_currentHole) - 1] % 100 >= 3)
				return;
		}

		float radius;
		WSliceStr slice(name);
		if (!slice.Get("*r", "%f", &radius))
			radius = 32.0f;

		int i;
		for (i = 0; i < 3; i++) {
			creature = new CSanta("NPC_Mozzi.pet");
			creature->SetRadius(radius);
			creature->SetArea(points, (float)i * -0.05f, 0);
			creature->SetType(7);
			m_creatures.push_back(creature);
		}

		creature = new CSanta("NPC_Santa.pet");
		creature->SetRadius(radius);
		creature->SetArea(points, (float)(i + 1) * -0.05f, 0);
		creature->SetType(7);
	} else if (strstr(name, "flight")) {
		creature = new CFighter("NPC_Fighter.pet");
		creature->SetArea(points, 0, 0);
		creature->SetType(8);
	} else if (strstr(name, "ufo")) {
		creature = new CUfo("sand_house12.pet");
		creature->SetArea(points, 0, 0);
		creature->SetType(0xB);
	} else if (strstr(name, "penguinwalk")) {
		creature = new ntPenguin("NPC_penguin02.pet");
		creature->SetArea(points, 0, 0);
		creature->SetType(0xC);
	} else if (strstr(name, "drg")) {
		float value;
		WSliceStr slice(name);
		if (!slice.Get("*d", "%f", &value))
			value = 0;
		creature = new ntNpc_Dragon("NPC_dragon.pet", value);
		creature->SetArea(points, 0, 0);
		creature->SetType(0xF);
	} else if (strstr(name, "ship")) {
		float value;
		WSliceStr slice(name);
		if (!slice.Get("*d", "%f", &value))
			value = 0;
		creature = new ntNpc_PolaShip("NPC_ship.pet", value);
		creature->SetArea(points, 0, 0);
		creature->SetType(0x11);
	} else if (strstr(name, "vessel")) {
		float value = 0;
		WSliceStr slice(name);
		char petName[64] = "";
		int index = 2;
		if (strstr(name, "vessel4"))
			index = 4;
		if (!slice.Get("*t", "%f", &value))
			value = 1.0f;
		sprintf(petName, "gs_ship_0%d.pet", index);
		creature = new CNPC_TRSHIP(petName, value);
		creature->SetArea(points, 0, 0);
		creature->SetType(0x14);
	} else if (strstr(name, "walex")) {
		float value;
		WSliceStr slice(name);
		char petName[64] = "";
		int index = 1;
		value = 0;
		if (strstr(name, "walex03"))
			index = 3;
		if (!slice.Get("*vel", "%f", &value))
			value = 1.0f;
		sprintf(petName, "npc_alex0%d.pet", index);
		creature = new CNPC_TORTOISE(petName, value);
		creature->SetArea(points, 0, 0);
		creature->SetType(0x17);
	} else if (strstr(name, "herald")) {
		float value;
		WSliceStr slice(name);
		if (!slice.Get("*vel", "%f", &value))
			value = 1.0f;
		if (strstr(name, "herald01"))
			creature = new CNPC_TORTOISE("NPC_Herald01.pet", value);
		else
			creature = new CNPC_TORTOISE("NPC_Herald03.pet", value);
		creature->SetArea(points, 0, 0);
		creature->SetType(0x17);
	} else if (strstr(name, "crn") || strstr(name, "gns") || strstr(name, "pnd") || strstr(name, "toy")) {
		float value;
		WSliceStr slice(name);
		char prefix[32] = "";
		char petName[256] = "";
		value = 0;
		bool tortoise = false;

		if (!slice.Get("*v", "%f", &value))
			value = 1.0f;
		if (strstr(name, "#"))
			tortoise = true;

		if (strstr(name, "crn"))
			strcpy(prefix, "NPC_crane");
		else if (strstr(name, "gns"))
			strcpy(prefix, "NPC_ginseng");
		else if (strstr(name, "pnd"))
			strcpy(prefix, "NPC_panda");
		else if (strstr(name, "toy"))
			strcpy(prefix, "NPC_toy");

		std::string full(name);
		std::string suffix("");
		std::string::size_type pos = strstr(name, "*") ? full.find('*') : full.find('#');
		if (pos == 0)
			pos = full.length();
		suffix = full.substr(7, pos - 7);
		sprintf(petName, "%s%s.pet", prefix, suffix.c_str());

		if (tortoise) {
			creature = new CNPC_TORTOISE(petName, value);
			creature->SetArea(points, 0, 0);
			creature->SetType(0x17);
		} else {
			creature = new CNPC_TRSHIP(petName, value);
			creature->SetArea(points, 0, 0);
			creature->SetType(0x14);
		}
	} else if (strstr(name, "npc_t")) {
		float value;
		const char *petName = "";
		WSliceStr slice(name);
		value = 1.0f;
		slice.Get("*v", "%f", &value);

		if (strstr(name, "npc_t0"))
			petName = "npc_insam_ani_N.pet";
		else if (strstr(name, "npc_t1"))
			petName = "npc_cat_ani02_N.pet";

		ntNpc_WizCityNode *node = new ntNpc_WizCityNode(petName, value);
		value = 1.0f;
		slice.Get("*s", "%f", &value);
		node->SetScale(value);
		creature = node;
		creature->SetArea(points, 0, 0);
		creature->SetType(0x20);
	} else {
		const char *suffix = strstr(name, "npc_");
		if (!suffix)
			return;
		creature = new CDisposable(MakeStr("%s.pet", suffix + 4));
		creature->SetArea(points, 0, 0);
		creature->SetType(9);
	}

	m_creatures.push_back(creature);
}

// npc.cpp:449
void CNPC::SetNPC(const Waabb &area, char *name) {
	WSliceStr slice(name);
	int type;
	char petName[32];
	int num;
	CCreature *creature = NULL;
	unsigned long flags = 0;

	slice.Get("*type", "%d", &type);
	slice.Get("*pet", "%s", petName);

	switch (type) {
	case 0:
		if (slice.Get("*num", "%d", &num)) {
			for (int i = 0; i < num; i++) {
				creature = new CGull(petName);
				creature->SetArea(area, 0);
				creature->SetType(type);
				m_creatures.push_back(creature);
			}
			return;
		}
		creature = new CGull(petName);
		break;
	case 1:
		if (slice.Get("*num", "%d", &num)) {
			for (int i = 0; i < num; i++) {
				creature = new CButterfly(petName);
				creature->SetArea(area, 4);
				creature->SetType(type);
				m_creatures.push_back(creature);
			}
			return;
		}
		creature = new CButterfly(petName);
		flags = 4;
		break;
	case 2:
		if (slice.Get("*num", "%d", &num))
			creature = new CDuck(petName, num + 1);
		else
			creature = new CDuck(petName, 0);
		flags = 1;
		break;
	case 3:
		creature = new CMole(petName);
		break;
	case 4:
		creature = new CDolphin(petName);
		break;
	case 5:
		creature = new CCrab(petName);
		break;
	case 6:
		// the original dereferences a null creature without "*num"
		if (slice.Get("*num", "%d", &num))
			creature = new CSparrow(petName, num);
		break;
	case 7:
		// no creature: the original dereferences a null pointer
		break;
	case 8:
		creature = new CFighter(petName);
		break;
	case 9:
		creature = new CDisposable(petName);
		break;
	case 10:
		// no creature: the original dereferences a null pointer
		break;
	case 13:
		if (slice.Get("*num", "%d", &num)) {
			for (int i = 0; i < num; i++) {
				creature = new ntPenguin_Sailor(petName);
				creature->SetArea(area, 0);
				creature->SetType(type);
				m_creatures.push_back(creature);
			}
			return;
		}
		creature = new ntPenguin_Sailor(petName);
		break;
	case 14:
		if (slice.Get("*num", "%d", &num))
			creature = new ntPenguin_Duck(petName, num + 1);
		else
			creature = new ntPenguin_Duck(petName, 0);
		flags = 1;
		break;
	case 16:
		if (slice.Get("*num", "%d", &num)) {
			for (int i = 0; i < num; i++) {
				creature = new ntNpc_BlackSparrow("NPC_BTsparrow.pet");
				creature->SetArea(area, 0);
				creature->SetType(type);
				m_creatures.push_back(creature);
			}
			return;
		}
		creature = new ntNpc_BlackSparrow("NPC_BTsparrow.pet");
		break;
	case 18: {
		float vel = 2.0f;
		slice.Get("*vel", "%f", &vel);
		num = 1;
		slice.Get("*num", "%d", &num);
		for (int i = 0; i < num; i++) {
			creature = new ntNpc_PolaPenguin1(petName, vel);
			creature->SetArea(area, 0);
			creature->SetType(type);
			m_creatures.push_back(creature);
		}
		return;
	}
	case 19:
		num = 1;
		slice.Get("*num", "%d", &num);
		for (int i = 0; i < num; i++) {
			creature = new ntNpc_PolaPenguin2(petName);
			creature->SetArea(area, 0);
			creature->SetType(type);
			m_creatures.push_back(creature);
		}
		return;
	case 31: {
		num = 1;
		slice.Get("*num", "%d", &num);
		float scale = 1.0f;
		slice.Get("*s", "%f", &scale);
		for (int i = 0; i < num; i++) {
			ntNpc_WizCityArea *wizCity = new ntNpc_WizCityArea(petName);
			wizCity->SetScale(scale);
			creature = wizCity;
			creature->SetArea(area, 0);
			creature->SetType(type);
			m_creatures.push_back(creature);
		}
		return;
	}
	case 21: {
		num = 1;
		slice.Get("*num", "%d", &num);
		float vel; // the original reads "*vel" without a default
		slice.Get("*vel", "%f", &vel);
		for (int i = 0; i < num; i++) {
			creature = new CNPC_BIRD(petName, vel);
			creature->SetArea(area, 0);
			creature->SetType(type);
			m_creatures.push_back(creature);
		}
		return;
	}
	case 22:
		if (!slice.Get("num", "%d", &num))
			num = 1;
		for (int i = 0; i < num; i++) {
			creature = new CNPC_PARROT(petName);
			creature->SetArea(area, 0);
			creature->SetType(type);
			m_creatures.push_back(creature);
		}
		return;
	default:
		return;
	}

	creature->SetArea(area, flags);
	creature->SetType(type);
	m_creatures.push_back(creature);
}

// npc.cpp:742
CCreature::~CCreature(void) {
	if (g_resrcmng) {
		if (m_addTex) {
			g_resrcmng->Release(m_addTex);
			m_addTex = 0;
		}
	}
	if (g_resrcmng && m_pPet) {
		g_resrcmng->Release(m_pPet, false);
		m_pPet = 0;
	}
	if (m_pBuffer) {
		delete[] m_pBuffer;
		m_pBuffer = 0;
	}

	m_nodes.clear();
	m_recordedPath.clear();
	m_alphaList.clear();
}

// npc.cpp:844
void CCreature::SetArea(const Waabb &area, unsigned long flags) {
	m_area = area;

	if (m_flags & 1)
		return;

	m_matrix = RotMat(Random(-180.0f, 180.0f, -1) * data_9f0fc0, 1);

	for (int i = 0; i < 3; i++) {
		float r = Random(0, 1.0f, -1);
		(&m_matrix.pivot.x)[i] = (1.0f - r) * (&m_area.max.x)[i] + r * (&m_area.min.x)[i];
	}

	if (flags & 1) {
		m_matrix.pivot = GetPVS().GetGroundPoint(m_matrix.pivot, false, 0, true);
	}
	if (flags & 2) {
		m_matrix.pivot = GetPVS().GetGroundPoint(m_matrix.pivot, false, 0, false);
	}

	m_pos = m_matrix.pivot;

	float cellsX = (float)ceil((m_area.max.x - m_area.min.x) / m_unk84);
	float cellsY;
	if (flags & 4)
		cellsY = (float)ceil((m_area.max.y - m_area.min.y) / m_unk84);
	else
		cellsY = 1.0f;
	float cellsZ = (float)ceil((m_area.max.z - m_area.min.z) / m_unk84);

	m_gridX = (int)cellsX;
	m_gridY = (int)cellsY;
	m_gridZ = (int)cellsZ;

	m_gridCount = m_gridY > 1 ? 4 : 2;
	m_cellX = (m_area.max.x - m_area.min.x) / cellsX;
	m_cellY = (m_area.max.y - m_area.min.y) / cellsY;
	m_cellZ = (m_area.max.z - m_area.min.z) / cellsZ;
	m_pBuffer = (unsigned char *)new int[m_gridCount];

	int index = ((int)((m_pos.y - m_area.min.y) / m_cellY) * m_gridZ
		+ (int)((m_pos.z - m_area.min.z) / m_cellZ)) * m_gridX
		+ (int)((m_pos.x - m_area.min.x) / m_cellX);
	for (int i = 0; i < m_gridCount; i++) {
		((int *)m_pBuffer)[i] = index;
	}
}

// npc.cpp:894
void CCreature::SetArea(const std::vector<WVector> &points, float radius, unsigned long flags) {
	m_nodes.clear();

	m_nodes.reserve(points.size());

	m_nodes.assign(points.begin(), points.end());

	m_matrix.za = -1.0f * (m_nodes[1] - m_nodes[0]).Normalize();
	m_matrix.ya = WVector::UNIT_POS_Y;
	m_matrix.xa = WCrossProduct(m_matrix.ya, m_matrix.za);

	m_matrix.pivot = points[0];

	m_pos = m_nodes[1];

	m_motionTime = radius;
}

// npc.cpp:968
void CCreature::Display(void) {
	if (!WSingleton<COption>::Instance()->vIsNpcEnabled() && GOLFDOC()->m_currentHole != 0x13) {
		return;
	}

	if (m_pPet && m_active && !(m_motionTime < 0.0f)) {
		if (m_flags & 1) {
			m_pPet->SetAlpha(m_alpha);
		}

		m_pPet->ApplyBones(m_pPet->GetBones(0, m_motionTime, 0, 0, false), Abs(m_motionParam) > 0.0f ? RotMat(m_motionParam, 2) * m_matrix : m_matrix);
		m_pPet->Transform(g_view, 1.0f, 0);
		m_pPet->UpdateBSphere();

		if (g_view->InFrustum(m_pPet->GetBoundSphere())) {
			m_pPet->UpdateLightSource(&g_lightset, false, 0);
			m_pPet->Render(g_view, true, 1.0f, g_view->InFrustumSafe(m_pPet->GetBoundBox()), false, false);
		}
	}
}

// npc.cpp:753
void CCreature::Init(const char *name, unsigned long flags) {
	m_addTex = 0;
	m_pPet = g_resrcmng->GetPuppet(name, false, true, false);
	if (!m_pPet) {
		return;
	}

	CSceneManager::Instance()->RegisterElement((eObjType)4, m_pPet, this, name, flags, 0);

	m_motionParam = 0;
	m_recorded = false;
	m_alpha = 0xFF;
	m_replayIndex = 0;
	m_rollRate = 0.7853982f;
	SetMotion("NPC_Default", -1.0f);

	if (m_flags & 1) {
		m_active = false;
		m_alphaList.clear();

		int first, last;
		if (m_pPet->CheckFrameData((int)floor(m_pMotion->s * 30.0f), (int)ceil(m_pMotion->e * 30.0f), first, last)) {
			for (int i = first; i <= last; ++i) {
				sAlpha alpha;
				if (sscanf(m_pPet->m_frame[i].data, "*opa %f", &alpha.m_value)) {
					alpha.m_value *= 2.55f;
					alpha.m_time = m_pPet->GetFrameDataTime(i);
					m_alphaList.push_back(alpha);
				}
			}
		}
	} else {
		m_active = true;
		m_turnRate = 1.0471976f;
	}

	for (w_bound_box *box = m_pPet->m_bbList.Start(); box; box = m_pPet->m_bbList.Next()) {
		const char *option = box->info->option;
		if (!option) {
			continue;
		}
		const char *fx = strstr(option, "*fx");
		if (!fx) {
			continue;
		}

		Attacher unusedAttacher; // constructed, never used
		char sequence[8];
		char file[32] = "", extra[32] = "";
		sscanf(fx, "%s %s %s", sequence, file, extra);

		if (!box->bone || box->bone->IsInvisible()) {
			continue;
		}
		char *dot = strrchr(file, '.');
		if (!dot || strlen(dot) < 4) {
			continue;
		}
		const char *ext = dot + 1;
		if (stricmp(ext, "spr") && stricmp(ext, "seq")) {
			continue;
		}

		Attacher attacher;
		attacher.m_pMatrix[0] = &box->bone->GetMatrix();
		attacher.m_pOffset[0] = &box->info->spherePivot;
		// both kinds attach with the same arguments
		if (stricmp(ext, "spr") == 0)
			WSingleton<CFx>::Instance()->Attach(attacher, file, false, 0);
		else
			WSingleton<CFx>::Instance()->Attach(attacher, file, false, 0);
	}
}

const char *GetStrConst(const char *pSrc, char *pDest);

// Nameless data: the token ParsePuppetScript cuts out of a script.
static char data_af3540[256];

// npc.cpp:995
void CCreature::DrawDebugLine(void) {
	g_view->DrawSphere(m_pPet->GetBoundSphere(), 0xFF00FF00, 20, 0);

	if (!(m_flags & 1)) {
		g_view->DrawLine(m_matrix.pivot, 0xFFFFFF00, m_matrix.pivot - m_matrix.za * 5.0f, 0xFFFFFF00, 0);
		g_view->DrawLine(m_matrix.pivot, 0xFFFF0000, m_pos, 0xFFFF0000, 0);
	}
}

// npc.cpp:1006
void CCreature::SetMotion(const char *name, float time) {
	if (m_pPet) {
		const w_motion_data *motion = m_pPet->FindMotionData(name);

		if (motion) {
			m_pMotion = motion;
			if (time >= 0.0f) {
				float end = motion->e;
				time += motion->s;
				m_motionTime = time > end ? end : time;
			} else {
				m_motionTime = Random(motion->s, motion->e, -1);
			}
		} else {
			m_motionTime = 0;
		}
	}
}

// Minimum helper of an unidentified header (also local in camerawork.cpp).
template <class T> T MinOf(T a, T b) { return a > b ? b : a; }

// npc.cpp:1028
void CCreature::ScriptProcess(const w_motion_data *motion, float from, float to) {
	if (motion == 0)
		return;

	if (!(from <= motion->e) || from == to)
		return;

	float origTo = to;
	from *= 30.0f;
	to = MinOf(motion->e, to) * 30.0f;
	int first = Ceil(from, 0.03f);
	int last = to == (float)floor(to) ? float2int(to) - 1 : float2int((float)floor(to));

	if (first <= last) {
		int start, end;
		if (m_pPet->CheckFrameData(first, last, start, end)) {
			for (int i = start; i <= end; i++)
				ParsePuppetScript(m_pPet->GetFrameData(i));
		}
	}

	// The motion looped: process the part past its end from its start.
	if (origTo > motion->e) {
		from = motion->s * 30.0f;
		to = MinOf(motion->e, origTo - motion->e) * 30.0f;
		first = Ceil(from, 0.03f);
		last = to == (float)floor(to) ? float2int(to) - 1 : float2int((float)floor(to));

		if (first <= last) {
			int start, end;
			if (m_pPet->CheckFrameData(first, last, start, end)) {
				for (int i = start; i <= end; i++)
					ParsePuppetScript(m_pPet->GetFrameData(i));
			}
		}
	}
}

// npc.cpp:1076
void CCreature::ParsePuppetScript(const char *script) {
	char token[0x40];
	char name[0x40];
	char boneName[0x40];

	if (script) {
		const char *p = script;

		while (p) {
			p = strchr(p, '*');

			if (p == NULL)
				break;

			int len = 0;

			while (p[len]) {
				if (p[++len] == ' ')
					break;
				if (p[len] == '(') {
					len++;
					break;
				}
			}

			if (len <= 1)
				break;

			strncpy(data_af3540, p, len);
			data_af3540[len] = 0;
			p += len;

			if (strcmpi(data_af3540, "*fx(") == 0) {
				token[0] = 0;
				p = GetStrConst(p, token);

				if (strlen(token) == 0 || strlen(token) >= sizeof(token))
					return;

				if (token[0] == '@') {
					if (m_pMotion == NULL)
						return;
					if (strcmp(&token[1], m_pMotion->name) != 0)
						return;

					p = GetStrConst(p, name);
					if (strlen(name) == 0 || strlen(name) >= sizeof(name))
						return;
				} else {
					strcpy(name, token);
				}

				Attacher attacher;

				int count = 0;
				do {
					p = GetStrConst(p, boneName);

					if (strlen(boneName) == 0 || strlen(boneName) >= sizeof(boneName))
						break;

					WBone *bone = m_pPet->FindBone(boneName);
					if (bone) {
						attacher.m_pMatrix[count] = &bone->GetMatrix();
					} else {
						w_bound_box *box = m_pPet->m_bbList.Find(boneName);
						if (box) {
							if (box->bone)
								attacher.m_pMatrix[count] = &box->bone->GetMatrix();
							attacher.m_pOffset[count] = &box->info->spherePivot;
						}
					}
				} while (++count < sizeof(attacher.m_pMatrix) / sizeof(attacher.m_pMatrix[0]));

				if (count > 0)
					WSingleton<CFx>::Instance()->Attach(attacher, name, true, 0);
			}
		}
	}
}

// npc.cpp:1185
void CCreature::CheckTarget(float dt) {
	if ((m_matrix.pivot - m_pos).Magnitude() < 1.0f) {
		if (m_replayIndex >= m_recordedPath.size() || m_recorded) {
			int tries = 0;
			int cell[3];
			int index;
			int i;
			bool found;

			do {
				tries++;

				int rnd[3];
				for (i = 0; i < 3; i++) {
					rnd[i] = rand() % 3 - 1;
				}

				int pos[3];
				pos[0] = ((int *)m_pBuffer)[0] % m_gridX;
				pos[1] = ((int *)m_pBuffer)[0] / (m_gridZ * m_gridX);
				pos[2] = ((int *)m_pBuffer)[0] % (m_gridZ * m_gridX) / m_gridX;

				cell[0] = Between(0, rnd[0] + pos[0], m_gridX - 1);
				cell[1] = Between(0, rnd[1] + pos[1], m_gridY - 1);
				cell[2] = Between(0, rnd[2] + pos[2], m_gridZ - 1);
				index = (m_gridZ * cell[1] + cell[2]) * m_gridX + cell[0];

				found = false;
				for (i = 0; i < m_gridCount; i++) {
					if (index == ((int *)m_pBuffer)[i]) {
						found = true;
						break;
					}
				}
			} while (found && tries < 5);

			for (i = m_gridCount - 1; i >= 1; i--) {
				((int *)m_pBuffer)[i] = ((int *)m_pBuffer)[i - 1];
			}
			((int *)m_pBuffer)[0] = index;

			m_pos.x = Random(0.0f, m_cellX, -1) + cell[0] * m_cellX + m_area.min.x;
			if (m_gridY > 1)
				m_pos.y = Random(0.0f, m_cellY, -1) + cell[1] * m_cellY + m_area.min.y;
			m_pos.z = Random(0.0f, m_cellZ, -1) + cell[2] * m_cellZ + m_area.min.z;

			if (m_recorded)
				m_recordedPath.push_back(m_pos);
		} else {
			m_pos = m_recordedPath[m_replayIndex];
			m_replayIndex++;
		}
	}

	if (m_gridY > 1) {
		m_matrix.pivot += m_unk74 * (m_pos - m_matrix.pivot).Normalize() * dt;
	} else {
		float angle = CalcDeltaAngle(m_pos - m_matrix.pivot, m_matrix.za * -1.0f);

		if (Abs(angle) > data_9f0fc0) {
			if (Abs(angle) > m_rollRate) {
				if (Abs(m_motionParam) < m_unk7C)
					m_motionParam -= angle * dt;
			} else if (Abs(m_motionParam) > Abs(angle * dt)) {
				m_motionParam += (angle > 0 ? m_rollRate : -m_rollRate) * dt;
			} else {
				m_motionParam = 0;
			}

			m_matrix = RotMat(Between(-(dt * m_turnRate), angle, dt * m_turnRate), 1) * m_matrix;
		} else if (Abs(m_motionParam) > 0) {
			if (Abs(m_motionParam) > dt * m_rollRate)
				m_motionParam += (m_motionParam > 0 ? -m_rollRate : m_rollRate) * dt;
			else
				m_motionParam = 0;
		}

		m_matrix.pivot += m_matrix.za * -1.0f * m_unk74 * dt;
	}
}

// npc.cpp:1292
void CCreature::PlayEvent(void) {
	m_motionTime = 0;
	m_active = true;
	m_alpha = 0xFF;
	m_matrix = RotMat(Random(-3.1415927f, 3.1415927f, -1), 1);
	m_matrix.pivot = GetPVS().GetGroundPoint(GolfBall().m_pos + m_unkAC * m_matrix.za, false, 0, true);
}

// npc.cpp:918
void CCreature::Process(float dt) {
	if (m_pPet && m_active) {
		float t = dt + m_motionTime;
		m_motionTime = t;

		if (!(t < 0.0f)) {
			ScriptProcess(m_pMotion, t - dt, t);

			if (m_motionTime > m_pMotion->e) {
				if (m_flags & 1) {
					m_active = false;
					m_motionTime = m_pMotion->s;
				} else {
					m_motionTime -= m_pMotion->e;
				}
			}

			if (m_flags & 1) {
				for (std::list<sAlpha>::iterator it = m_alphaList.begin(); it != m_alphaList.end();) {
					std::list<sAlpha>::iterator cur = it;
					++it;

					if (it == m_alphaList.end()) {
						m_alpha = (unsigned char)(int)cur->m_value;
					} else if (m_motionTime > cur->m_time && m_motionTime < it->m_time) {
						float k = (m_motionTime - cur->m_time) / (it->m_time - cur->m_time);
						m_alpha = (unsigned char)(int)((1.0f - k) * cur->m_value + k * it->m_value);
						break;
					}
				}
			} else {
				CheckTarget(dt);
			}
		}
	}
}

// npc.cpp:1304
void CCreature::Record(void) {
	m_recordedPath.clear();
	m_recordedPath.reserve(10);
	m_recordedPath.push_back(m_pos);
	m_recorded = true;
	m_recordedMotionTime = m_motionTime;
	m_recordedMatrix = m_matrix;
	m_recordedMotionParam = m_motionParam;
	m_recordedActive = m_active;
}

// npc.cpp:1322
void CCreature::Replay(void) {
	if (m_flags & 1) {
		m_active = false;
		return;
	}
	m_pos = m_recordedPath[0];
	m_replayIndex = 1;
	m_motionTime = m_recordedMotionTime;
	m_recorded = false;
	m_matrix = m_recordedMatrix;
	m_motionParam = m_recordedMotionParam;
	m_active = m_recordedActive;
}

// npc.cpp:1346
CGull::CGull(const char *name) {
	m_unk74 = 10.0f;
	Init(name, 0);
	m_unk7C = 0.7853982f;
	m_unk84 = 48.0f;
}

// npc.cpp:1354
CGull::~CGull(void) {
}

// npc.cpp:1362
CButterfly::CButterfly(const char *name) {
	m_unk74 = 7.0f;
	Init(name, 0);
	m_unk7C = 0;
	m_unk84 = 16.0f;
}

// npc.cpp:1370
CButterfly::~CButterfly(void) {
}

// npc.cpp:1376
CDuck::CDuck(const char *name, int count) : m_pMatrices(0) {
	m_unk74 = 2.0f;
	Init(name, 0);
	m_turnRate = 0.5235988f;
	m_unk7C = 0;
	m_unk84 = 32.0f;
	m_count = count;
	if (count > 0) {
		m_pMatrices = new WMatrix[count];
	}
}

// npc.cpp:1392
void CDuck::SetArea(const Waabb &area, unsigned long flags) {
	CCreature::SetArea(area, flags);

	if (m_count == 0)
		return;

	m_pMatrices[0] = m_matrix;

	for (int i = 1; i < m_count; i++) {
		m_pMatrices[i] = m_pMatrices[i - 1];
		m_pMatrices[i].pivot = m_pMatrices[i - 1].pivot + m_pMatrices[i - 1].za * 6.4f;
	}
}

// npc.cpp:1408
void CDuck::Process(float dt) {
	CheckTarget(dt);

	if (m_count == 0)
		return;

	m_pMatrices[0] = m_matrix;

	for (int i = 1; i < m_count; i++) {
		WVector toPrev;
		toPrev = m_pMatrices[i - 1].pivot - m_pMatrices[i].pivot;

		if (toPrev.Magnitude() < 3.2f)
			continue;

		float angle = Between(dt * m_turnRate * -0.7f, CalcDeltaAngle(m_pMatrices[i - 1].pivot - m_pMatrices[i].pivot, m_pMatrices[i].za * -1.0f), dt * m_turnRate * 0.7f);

		WMatrix &matrix = m_pMatrices[i];
		matrix = RotMat(angle, 1) * matrix;
		m_pMatrices[i].pivot += m_pMatrices[i].za * -1.0f * m_unk74 * dt;
	}
}


// npc.cpp:1437
void CDuck::Display(void) {
	if (m_count == 0) {
		CCreature::Display();
		return;
	}

	for (int i = 0; i < m_count; i++) {
		m_pPet->ApplyBones(0, m_pMatrices[i] * (i == 0 ? 1.0f : 0.5f));
		m_pPet->Transform(g_view, 1.0f, 0);

		m_pPet->UpdateBSphere();

		if (g_view->InFrustum(m_pPet->GetBoundSphere())) {
			m_pPet->UpdateLightSource(&g_lightset, false, 0);
			m_pPet->Render(g_view, false, 1.0f, g_view->InFrustumSafe(m_pPet->GetBoundBox()), true, false);
		}
	}
}

// npc.cpp:1465
CMole::CMole(const char *name) {
	unsigned long &flags = m_flags;
	flags |= 1;
	Init(name, 0);
	m_unkAC = 6.4f;
}

// npc.cpp:1472
void CMole::PlayEvent(void) {
	if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel < 1) {
		return;
	}
	CCreature::PlayEvent();

	if (GetPVS().GetGroundType(m_matrix.pivot, true, 0) == 3) {
		return;
	}
	m_matrix.za = GolfBall().m_pos - 0.5f * (m_area.max + m_area.min);

	m_matrix.za.y = 0; m_matrix.za.Normalize();

	m_matrix.xa = WCrossProduct(m_matrix.ya, m_matrix.za);
	m_matrix.xa.Normalize();

	m_matrix.pivot = GetPVS().GetGroundPoint(m_unkAC * m_matrix.za + GolfBall().m_pos, false, 0, true);
}

// npc.cpp:1497
CDolphin::CDolphin(const char *name) {
	unsigned long &flags = m_flags;
	flags |= 1;
	Init(name, 0);
	m_unkAC = 0;
}

// npc.cpp:1504
void CDolphin::PlayEvent(void) {
	if (GolfBall().m_landed && GolfBall().m_lie == 1) {
		CCreature::PlayEvent();
	}
}

// npc.cpp:1513
CCrab::CCrab(const char *name) {
	unsigned long &flags = m_flags;
	flags |= 1;
	Init(name, 0);
	m_unkAC = -3.2f;
}

// npc.cpp:1520
void CCrab::PlayEvent(void) {
	if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_powerShotLevel >= 1) {
		CCreature::PlayEvent();
	}
}

// npc.cpp:1529
CSparrow::CSparrow(const char *name, int count) : m_pMatrices(0) {
	m_pPet = g_resrcmng->GetPuppet(name, false, true, false);
	m_motionParam = 0;
	m_recorded = false;
	m_alpha = 0xFF;
	m_active = false;
	m_maxCount = count;
	m_pMatrices = new WMatrix[count];
}

// npc.cpp:1542
void CSparrow::PlayEvent(void) {
	m_active = true;

	float speed = GolfBall().m_velocity.Magnitude();

	m_count = (int)(m_maxCount / GolfBall().PreData(0)->m_velocity.Magnitude() * speed);
	for (int i = 0; i < m_count; i++) {
		m_pMatrices[i] = RotMat(Random(-180.0f, 180.0f, -1) * data_9f0fc0, 1);
		m_pMatrices[i].pivot = (m_area.min + m_area.max) * 0.5f;
	}
}

// npc.cpp:1556
void CSparrow::Display(void) {
	if (m_pPet && m_active) {
		for (int i = 0; i < m_count; i++) {
			m_pPet->ApplyBones(m_pPet->GetBones(0, m_motionTime, 0, 0, false), m_pMatrices[i]);
			m_pPet->Transform(g_view, 1.0f, 0);

			m_pPet->UpdateBSphere();

			if (g_view->InFrustum(m_pPet->GetBoundSphere())) {
				m_pPet->UpdateLightSource(&g_lightset, false, 0);
				m_pPet->Render(g_view, false, 1.0f, g_view->InFrustumSafe(m_pPet->GetBoundBox()), true, false);
			}
		}
	}
}

// npc.cpp:1580
CSanta::CSanta(const char *name) {
	Init(name, 0);
	m_nodeIndex = 1;
	m_unk74 = 120.0f;
	m_unk7C = 0.7853982f;
	m_turnRate = 2.0943952f;
	m_climbRate = 0;
	m_rollRate = 1.5707964f;
	m_state = 2;
}

// npc.cpp:1593
CSanta::~CSanta(void) {
}

// npc.cpp:1597
void CSanta::Process(float dt) {
	if (m_pPet && m_active) {
		float time = dt + m_motionTime;
		m_motionTime = time;
		if (!(time < 0.0f)) {
			ScriptProcess(m_pMotion, time - dt, time);
			if (m_motionTime > m_pMotion->e) {
				m_motionTime -= m_pMotion->e;
				if (rand() % 10 == 0 && strcmpi(m_pMotion->name, "NPC_Default") == 0)
					SetMotion("NPC_Default1", 0);
				else
					SetMotion("NPC_Default", 0);
			}
			CheckTarget(dt);
		}
	}
}

// npc.cpp:1623
void CSanta::CheckTarget(float dt) {
	WVector d = m_pos - m_matrix.pivot;
	d.y = 0.0f;

	if (d.Magnitude() < Max(dt, 0.1f) * m_unk74) {
		if (m_replayIndex >= m_recordedPath.size() || m_recorded) {
			if (m_nodeIndex == m_nodes.size() - 1) {
				m_active = false;
				return;
			}

			unsigned char prev = m_nodeIndex;
			m_nodeIndex = (m_nodeIndex + 1) % m_nodes.size();
			m_pos = m_nodes[m_nodeIndex];
			m_pos += (m_unkAC * RotMat(Random(-1.5707964f, 1.5707964f, -1), 0).za) * RotMat(Random(-3.1415927f, 3.1415927f, -1), 1);

			d = m_pos - m_matrix.pivot;
			d.y = 0.0f;
			WVector seg = m_nodes[m_nodeIndex] - m_nodes[prev];
			seg.y = 0.0f;

			float dist = d.Magnitude();
			m_unk74 = dist * 120.0f / seg.Magnitude();
			m_climbRate = (m_pos.y - m_matrix.pivot.y) / dist * m_unk74;

			m_unk7C = Min(Abs(CalcDeltaAngle(d, m_matrix.za * -1.0f)), 1.5707964f);
			m_state = 1;

			if (m_recorded)
				m_recordedPath.push_back(m_pos);
		} else {
			if (m_replayIndex < m_recordedPath.size()) {
				m_pos = m_recordedPath[m_replayIndex];
				m_replayIndex++;
			} else {
				m_active = false;
			}
		}
	}

	float angle = CalcDeltaAngle(d, m_matrix.za * -1.0f);

	if (Abs(angle) > data_9f0fc0) {
		float limit = dt * m_turnRate;
		m_matrix = RotMat(Between(-limit, angle, limit), 1) * m_matrix;
	}

	switch (m_state) {
	case 0:
		if (Abs(m_motionParam) > dt * m_rollRate) {
			m_motionParam += (m_motionParam > 0 ? -m_rollRate : m_rollRate) * dt;
		} else {
			m_motionParam = 0;
			m_state = 2;
		}
		break;
	case 1:
		if (Abs(m_motionParam) < m_unk7C) {
			m_motionParam += (angle > 0 ? -m_rollRate : m_rollRate) * dt;
		} else {
			m_state = 0;
		}
		break;
	}

	m_matrix.pivot += m_matrix.za * -1.0f * m_unk74 * dt;
	m_matrix.pivot.y += dt * m_climbRate;

	float dist = (g_camera.pivot - m_matrix.pivot).Magnitude();

	if (dist < 640.0f) {
		if (!g_audio->IsPlaying("Santa"))
			g_audio->PlaySfx("santaboost", 0, 1, "Santa", 0, 0.5f, 200.0f);
		g_audio->SetSFXVolumeWhilePlaying("Santa", (640.0f - dist) * 0.0015625f);
	} else {
		g_audio->StopSfx("Santa");
	}
}

// npc.cpp:1724
CFighter::CFighter(const char *name) {
	Init(name, 0);
	m_nodeIndex = 1;
	m_unk74 = 320.0f;
	m_unk7C = 0.7853982f;
	m_turnRate = 2.0943952f;
	m_climbRate = 0;
	m_rollRate = 1.5707964f;
	m_state = 2;
}

// npc.cpp:1737
CFighter::~CFighter(void) {
}

// npc.cpp:1741
void CFighter::CheckTarget(float dt) {
	WVector d = m_pos - m_matrix.pivot;
	d.y = 0.0f;

	if (d.Magnitude() < Max(dt, 0.1f) * m_unk74) {
		if (m_replayIndex >= m_recordedPath.size() || m_recorded) {
			if (m_nodeIndex == m_nodes.size() - 1) {
				m_active = false;
				return;
			}

			m_nodeIndex = (m_nodeIndex + 1) % m_nodes.size();
			m_pos = m_nodes[m_nodeIndex];

			d = m_pos - m_matrix.pivot;
			d.y = 0.0f;

			float dist = d.Magnitude();
			m_unk74 = dist;
			m_climbRate = (m_pos.y - m_matrix.pivot.y) / dist * m_unk74;

			m_unk7C = Min(Abs(CalcDeltaAngle(d, m_matrix.za * -1.0f)), 1.5707964f);
			m_state = 1;

			if (m_recorded)
				m_recordedPath.push_back(m_pos);
		} else {
			if (m_replayIndex < m_recordedPath.size()) {
				m_pos = m_recordedPath[m_replayIndex];
				m_replayIndex++;
			} else {
				m_active = false;
			}
		}
	}

	float angle = CalcDeltaAngle(d, m_matrix.za * -1.0f);

	if (Abs(angle) > data_9f0fc0) {
		float limit = dt * m_turnRate;
		m_matrix = RotMat(Between(-limit, angle, limit), 1) * m_matrix;
	}

	switch (m_state) {
	case 0:
		if (Abs(m_motionParam) > dt * m_rollRate) {
			m_motionParam += (m_motionParam > 0 ? -m_rollRate : m_rollRate) * dt;
		} else {
			m_motionParam = 0;
			m_state = 2;
		}
		break;
	case 1:
		if (Abs(m_motionParam) < m_unk7C) {
			m_motionParam += (angle > 0 ? -m_rollRate : m_rollRate) * dt;
		} else {
			m_state = 0;
		}
		break;
	}

	m_matrix.pivot += m_matrix.za * -1.0f * m_unk74 * dt;
	m_matrix.pivot.y += dt * m_climbRate;
}

// npc.cpp:1821
CUfo::CUfo(const char *name) {
	Init(name, 0);
	m_nodeIndex = 1;
	m_unk74 = 2.857143f;
	m_unk7C = 0.7853982f;
	m_turnRate = 2.0943952f;
	m_climbRate = 0;
	m_rollRate = 1.5707964f;
	m_state = 2;
}

// npc.cpp:1834
CUfo::~CUfo(void) {
}

// npc.cpp:1838
void CUfo::Process(float dt) {
	if (m_pPet && m_active) {
		float time = dt + m_motionTime;
		m_motionTime = time;
		if (!(time < 0.0f)) {
			ScriptProcess(m_pMotion, time - dt, time);
			if (m_motionTime > m_pMotion->e) {
				m_motionTime -= m_pMotion->e;
				SetMotion("NPC_Default", 0);
			}
			CheckTarget(dt);
		}
	}
}

// npc.cpp:1861
void CUfo::CheckTarget(float dt) {
	WVector d = m_pos - m_matrix.pivot;
	d.y = 0.0f;

	if (d.Magnitude() < Max(dt, 0.1f) * m_unk74) {
		if (m_replayIndex >= m_recordedPath.size() || m_recorded) {
			if (m_nodeIndex == m_nodes.size() - 1) {
				m_active = false;
				return;
			}

			unsigned char prev = m_nodeIndex;
			m_nodeIndex = (m_nodeIndex + 1) % m_nodes.size();
			m_pos = m_nodes[m_nodeIndex];
			m_pos += (m_unkAC * RotMat(Random(-1.5707964f, 1.5707964f, -1), 0).za) * RotMat(Random(-3.1415927f, 3.1415927f, -1), 1);

			d = m_pos - m_matrix.pivot;
			d.y = 0.0f;

			WVector e = m_nodes[m_nodeIndex] - m_nodes[prev];
			e.y = 0.0f;

			float len = d.Magnitude();
			m_unk74 = len * 120.0f / e.Magnitude();
			m_climbRate = (m_pos.y - m_matrix.pivot.y) / len * m_unk74;

			m_unk7C = Min(Abs(CalcDeltaAngle(d, m_matrix.za * -1.0f)), 1.5707964f);
			m_state = 1;

			if (m_recorded)
				m_recordedPath.push_back(m_pos);
		} else {
			if (m_replayIndex < m_recordedPath.size()) {
				m_pos = m_recordedPath[m_replayIndex];
				m_replayIndex++;
			} else {
				m_active = false;
			}
		}
	}

	float angle = CalcDeltaAngle(d, m_matrix.za * -1.0f);

	if (Abs(angle) > data_9f0fc0) {
		float limit = dt * m_turnRate;
		m_matrix = RotMat(Between(-limit, angle, limit), 1) * m_matrix;
	}

	switch (m_state) {
	case 0:
		if (Abs(m_motionParam) > dt * m_rollRate) {
			m_motionParam += (m_motionParam > 0 ? -m_rollRate : m_rollRate) * dt;
		} else {
			m_motionParam = 0;
			m_state = 2;
		}
		break;
	case 1:
		if (Abs(m_motionParam) < m_unk7C) {
			m_motionParam += (angle > 0 ? -m_rollRate : m_rollRate) * dt;
		} else {
			m_state = 0;
		}
		break;
	}

	m_matrix.pivot += m_matrix.za * -1.0f * m_unk74 * dt;
	m_matrix.pivot.y += dt * m_climbRate;
}

// npc.cpp:1982
ntPenguin::ntPenguin(const char *name) {
	Init(name, 0);
	m_nodeIndex = 1;
	m_unk74 = 2.857143f;
	m_unk7C = 0.7853982f;
	m_turnRate = 2.0943952f;
	m_climbRate = 0;
	m_rollRate = 1.5707964f;
	m_state = 2;
}

// npc.cpp:1998
ntPenguin::~ntPenguin(void) {
}

// npc.cpp:2004
void ntPenguin::Process(float dt) {
	if (m_pPet && m_active) {
		float time = dt + m_motionTime;
		m_motionTime = time;
		if (!(time < 0.0f)) {
			ScriptProcess(m_pMotion, time - dt, time);
			if (m_motionTime > m_pMotion->e) {
				m_motionTime -= m_pMotion->e;
				SetMotion("NPC_Default", 0);
			}
			CheckTarget(dt);
		}
	}
}

// npc.cpp:2027
void ntPenguin::CheckTarget(float dt) {
	WVector d = m_pos - m_matrix.pivot;
	d.y = 0.0f;

	if (d.Magnitude() < Max(dt, 0.1f) * m_unk74) {
		if (m_replayIndex >= m_recordedPath.size() || m_recorded) {
			if (m_nodeIndex == m_nodes.size() - 1)
				m_nodeIndex = 0;

			m_nodeIndex = (m_nodeIndex + 1) % m_nodes.size();
			m_pos = m_nodes[m_nodeIndex];
			m_pos += (m_unkAC * RotMat(Random(-1.5707964f, 1.5707964f, -1), 0).za) * RotMat(Random(-3.1415927f, 3.1415927f, -1), 1);

			d = m_pos - m_matrix.pivot;
			d.y = 0.0f;

			if (m_recorded)
				m_recordedPath.push_back(m_pos);
		} else {
			if (m_replayIndex < m_recordedPath.size()) {
				m_pos = m_recordedPath[m_replayIndex];
				m_replayIndex++;
			} else {
				m_replayIndex = 0;
			}
		}
	}

	float angle = CalcDeltaAngle(d, m_matrix.za * -1.0f);

	if (Abs(angle) > data_9f0fc0) {
		m_matrix = RotMat(Between(-(dt * m_turnRate), angle, dt * m_turnRate), 1) * m_matrix;
	}

	m_matrix.pivot += m_matrix.za * -1.0f * m_unk74 * dt;
	m_matrix.pivot.y += dt * m_climbRate;
	m_matrix.pivot = GetPVS().GetGroundPoint(m_matrix.pivot, true, 0, false);
}

// npc.cpp:2087
ntPenguin_Sailor::ntPenguin_Sailor(const char *name) {
	m_unk74 = 2.0f;
	Init(name, 0);
	m_unk7C = 0.7853982f;
	m_unk84 = 48.0f;
	SetMotion("NPC_Act0", 0);
	m_scale = Random(0.7f, 1.0f, -1);
	m_act0Motion = true;
}

// npc.cpp:2106
ntPenguin_Sailor::~ntPenguin_Sailor(void) {
}

// npc.cpp:2112
void ntPenguin_Sailor::Process(float dt) {
	if (m_pPet && m_active) {
		float time = dt + m_motionTime;
		m_motionTime = time;

		if (!(time < 0.0f)) {
			ScriptProcess(m_pMotion, time - dt, time);

			if (m_motionTime > m_pMotion->e) {
				if (m_flags & 1) {
					m_active = false;
					m_motionTime = m_pMotion->s;
				} else {
					m_motionTime -= m_pMotion->e - m_pMotion->s;
					switch (rand() % 10) {
					case 0:
						SetMotion("NPC_Act1", 0);
						m_act0Motion = false;
						break;
					case 1:
						SetMotion("NPC_Default", 0);
						m_act0Motion = false;
						break;
					default:
						SetMotion("NPC_Act0", 0);
						m_act0Motion = true;
						break;
					}
				}
			}

			if (m_flags & 1) {
				for (std::list<sAlpha>::iterator it = m_alphaList.begin(); it != m_alphaList.end();) {
					std::list<sAlpha>::iterator cur = it;
					++it;

					if (it == m_alphaList.end()) {
						m_alpha = (unsigned char)(int)cur->m_value;
					} else if (m_motionTime > cur->m_time && m_motionTime < it->m_time) {
						float k = (m_motionTime - cur->m_time) / (it->m_time - cur->m_time);
						m_alpha = (unsigned char)(int)((1.0f - k) * cur->m_value + k * it->m_value);
						break;
					}
				}
			} else if (m_act0Motion) {
				CheckTarget(dt);
			}
		}
	}
}

// npc.cpp:2189
void ntPenguin_Sailor::Display(void) {
	if (!WSingleton<COption>::Instance()->vIsNpcEnabled() && GOLFDOC()->m_currentHole != 0x13) {
		return;
	}

	if (m_pPet && m_active && !(m_motionTime < 0.0f)) {
		if (m_flags & 1) {
			m_pPet->SetAlpha(m_alpha);
		}

		m_pPet->ApplyBones(m_pPet->GetBones(0, m_motionTime, 0, 0, false), Abs(m_motionParam) > 0.0f ? RotMat(m_motionParam, 2) * m_matrix * m_scale : m_matrix * m_scale);
		m_pPet->Transform(g_view, 1.0f, 0);
		m_pPet->UpdateBSphere();

		if (g_view->InFrustum(m_pPet->GetBoundSphere())) {
			m_pPet->UpdateLightSource(&g_lightset, false, 0);
			m_pPet->Render(g_view, true, 1.0f, g_view->InFrustumSafe(m_pPet->GetBoundBox()), false, false);
		}
	}
}

// npc.cpp:2217
void ntPenguin_Sailor::CheckTarget(float dt) {
	if ((m_matrix.pivot - m_pos).Magnitude() < 1.0f) {
		if (m_replayIndex >= m_recordedPath.size() || m_recorded) {
			int tries = 0;
			int cell[3];
			int index;
			int i;
			bool found;

			do {
				tries++;

				int rnd[3];
				for (i = 0; i < 3; i++) {
					rnd[i] = rand() % 3 - 1;
				}

				int pos[3];
				pos[0] = ((int *)m_pBuffer)[0] % m_gridX;
				pos[1] = ((int *)m_pBuffer)[0] / (m_gridZ * m_gridX);
				pos[2] = ((int *)m_pBuffer)[0] % (m_gridZ * m_gridX) / m_gridX;

				cell[0] = Between(0, rnd[0] + pos[0], m_gridX - 1);
				cell[1] = Between(0, rnd[1] + pos[1], m_gridY - 1);
				cell[2] = Between(0, rnd[2] + pos[2], m_gridZ - 1);
				index = (m_gridZ * cell[1] + cell[2]) * m_gridX + cell[0];

				found = false;
				for (i = 0; i < m_gridCount; i++) {
					if (index == ((int *)m_pBuffer)[i]) {
						found = true;
						break;
					}
				}
			} while (found && tries < 5);

			for (i = m_gridCount - 1; i >= 1; i--) {
				((int *)m_pBuffer)[i] = ((int *)m_pBuffer)[i - 1];
			}
			((int *)m_pBuffer)[0] = index;

			m_pos.x = Random(0.0f, m_cellX, -1) + cell[0] * m_cellX + m_area.min.x;
			if (m_gridY > 1)
				m_pos.y = Random(0.0f, m_cellY, -1) + cell[1] * m_cellY + m_area.min.y;
			m_pos.z = Random(0.0f, m_cellZ, -1) + cell[2] * m_cellZ + m_area.min.z;

			if (m_recorded)
				m_recordedPath.push_back(m_pos);
		} else {
			m_pos = m_recordedPath[m_replayIndex];
			m_replayIndex++;
		}
	}

	if (m_gridY > 1) {
		m_matrix.pivot += m_unk74 * (m_pos - m_matrix.pivot).Normalize() * dt;
	} else {
		float angle = CalcDeltaAngle(m_pos - m_matrix.pivot, m_matrix.za * -1.0f);

		if (Abs(angle) > data_9f0fc0) {
			m_matrix = RotMat(Between(-(dt * m_turnRate), angle, dt * m_turnRate), 1) * m_matrix;
		}

		m_matrix.pivot += m_matrix.za * -1.0f * m_unk74 * dt;
		m_matrix.pivot = GetPVS().GetGroundPoint(m_matrix.pivot, true, 0, false);
	}
}

// npc.cpp:2311
ntPenguin_Duck::ntPenguin_Duck(const char *name, int count) : m_pMatrices(0) {
	m_unk74 = 2.0f;
	Init(name, 0);
	m_turnRate = 0.5235988f;
	m_unk7C = 0;
	m_unk84 = 32.0f;
	m_count = count;

	if (count > 0) {
		m_pMatrices = new WMatrix[m_count];
		m_pScales = new float[m_count];

		m_pScales[0] = 1.0f;
		for (int i = 1; i < m_count; i++)
			m_pScales[i] = Random(0.6f, 0.8f, -1);
	}

	SetMotion("NPC_Act0", 0);
	m_playingAct0 = false;
}

// npc.cpp:2341
ntPenguin_Duck::~ntPenguin_Duck(void) {
	if (m_pMatrices) { delete[] m_pMatrices; m_pMatrices = 0; }
	if (m_pScales) { delete[] m_pScales; m_pScales = 0; }
}

// npc.cpp:2348
void ntPenguin_Duck::SetArea(const Waabb &area, unsigned long flags) {
	CCreature::SetArea(area, flags);

	if (m_count == 0)
		return;

	m_pMatrices[0] = m_matrix;

	for (int i = 1; i < m_count; i++) {
		m_pMatrices[i] = m_pMatrices[i - 1];
		m_pMatrices[i].pivot = m_pMatrices[i - 1].pivot + m_pMatrices[i - 1].za * 6.4f;
	}
}

// npc.cpp:2365
void ntPenguin_Duck::Process(float dt) {
	if (m_pPet && m_active) {
		float time = dt + m_motionTime;
		m_motionTime = time;

		if (!(time < 0.0f)) {
			ScriptProcess(m_pMotion, time - dt, time);

			if (m_motionTime > m_pMotion->e) {
				if (m_flags & 1) {
					m_active = false;
					m_motionTime = m_pMotion->s;
				} else {
					m_motionTime -= m_pMotion->e - m_pMotion->s;
					SetMotion("NPC_Act0", 0);

					if (rand() % 10 != 0) {
						SetMotion("NPC_Act0", 0);
						m_playingAct0 = true;
					} else {
						SetMotion("NPC_Act1", 0);
						m_playingAct0 = false;
					}
				}
			}

			if (m_playingAct0)
				CheckTarget(dt);

			m_matrix.pivot = GetPVS().GetGroundPoint(m_matrix.pivot, true, 0, false);

			if (m_count == 0)
				return;

			if (!m_playingAct0)
				return;

			m_pMatrices[0] = m_matrix;

			for (int i = 1; i < m_count; i++) {
				WVector toPrev;
				toPrev = m_pMatrices[i - 1].pivot - m_pMatrices[i].pivot;

				if (!(toPrev.Magnitude() < 4.16f)) {
					float angle = Between(dt * m_turnRate * -0.7f, CalcDeltaAngle(m_pMatrices[i - 1].pivot - m_pMatrices[i].pivot, m_pMatrices[i].za * -1.0f), dt * m_turnRate * 0.7f);

					WMatrix &matrix = m_pMatrices[i];
					matrix = RotMat(angle, 1) * matrix;
					m_pMatrices[i].pivot += m_pMatrices[i].za * -1.0f * m_unk74 * dt;
					m_pMatrices[i].pivot = GetPVS().GetGroundPoint(m_pMatrices[i].pivot, true, 0, false);
				}
			}
		}
	}
}

// npc.cpp:2448
void ntPenguin_Duck::Display(void) {
	if (!WSingleton<COption>::Instance()->vIsNpcEnabled() && GOLFDOC()->m_currentHole != 0x13) {
		return;
	}

	if (m_pPet && m_active && !(m_motionTime < 0.0f)) {
		if (m_count == 0) {
			CCreature::Display();
			return;
		}

		for (int i = 0; i < m_count; i++) {
			m_pPet->ApplyBones(m_pPet->GetBones(0, m_motionTime, 0, 0, false), m_pMatrices[i] * m_pScales[i]);
			m_pPet->Transform(g_view, 1.0f, 0);
			m_pPet->UpdateBSphere();

			if (g_view->InFrustum(m_pPet->GetBoundSphere())) {
				m_pPet->UpdateLightSource(&g_lightset, false, 0);
				m_pPet->Render(g_view, false, 1.0f, g_view->InFrustumSafe(m_pPet->GetBoundBox()), true, false);
			}
		}
	}
}

// npc.cpp:2482
ntNpc_Dragon::ntNpc_Dragon(const char *name, float speed) {
	Init(name, 0);
	m_hidden = true;
	m_distance = 0;
	m_speed = speed;
}

// npc.cpp:2492
ntNpc_Dragon::~ntNpc_Dragon(void) {
}

// npc.cpp:2496
void ntNpc_Dragon::Process(float dt) {
	if (m_active && m_pPet) {
		m_motionTime += dt;

		if (m_hidden) {
			if (!(m_motionTime > m_speed)) {
				return;
			}

			m_motionTime = 0;
			m_hidden = false;
			SetMotion("NPC_Act02", -1.0f);
		}

		ScriptProcess(m_pMotion, m_motionTime - dt, m_motionTime);

		if (m_motionTime > m_pMotion->e) {
			int r = rand() % 101;

			if (r < 5)
				SetMotion("NPC_Act02", 0);
			else if (r < 10)
				SetMotion("NPC_Act01", 0);
			else
				SetMotion("NPC_Default", 0);
		}

		m_distance += dt * 100.0f;

		CheckTarget(dt);
	}
}

// npc.cpp:2534
void ntNpc_Dragon::Display(void) {
	if (m_active && m_hidden != true) {
		CCreature::Display();
	}
}

// npc.cpp:2542
void ntNpc_Dragon::CheckTarget(float dt) {
	WVector next = m_spline.Evaluate(m_distance + 0.1f);
	m_matrix.pivot = m_spline.Evaluate(m_distance);

	if (WisEqual(next, m_nodes[m_nodes.size() - 1], 1e-5f)) {
		m_distance = 0;
	}

	m_matrix = MakeCameraMatrix(next, m_matrix.pivot);
}

// npc.cpp:2553
void ntNpc_Dragon::SetArea(const std::vector<WVector> &points, float radius, unsigned long flags) {
	CCreature::SetArea(points, radius, flags);

	WVector *pPoints = (WVector *)_alloca(points.size() * sizeof(WVector));
	float *pTimes = (float *)_alloca(points.size() * sizeof(float));

	pPoints[0] = points[0];
	pTimes[0] = 0;

	for (unsigned int i = 1; i < points.size(); i++) {
		pPoints[i] = points[i];
		pTimes[i] = pTimes[i - 1] + (points[i] - points[i - 1]).Magnitude();
	}

	m_spline.Init(pPoints, pTimes, points.size());
}

// npc.cpp:2575
ntNpc_BlackSparrow::ntNpc_BlackSparrow(const char *name) {
	m_unk74 = 5.5f;
	Init(name, 0);
	m_unk7C = 0.7853982f;
	m_unk84 = 48.0f;
	SetMotion("NPC_Default", 0);
	m_defaultMotion = true;
}

// npc.cpp:2591
ntNpc_BlackSparrow::~ntNpc_BlackSparrow(void) {
}

// npc.cpp:2596
void ntNpc_BlackSparrow::Process(float dt) {
	if (m_pPet && m_active) {
		float time = dt + m_motionTime;
		m_motionTime = time;

		if (!(time < 0.0f)) {
			ScriptProcess(m_pMotion, time - dt, time);

			if (m_motionTime > m_pMotion->e) {
				if (m_flags & 1) {
					m_active = false;
					m_motionTime = m_pMotion->s;
				} else {
					m_motionTime -= m_pMotion->e - m_pMotion->s;
					if (rand() % 101 < 20) {
						SetMotion("NPC_Act01", 0);
						m_defaultMotion = false;
					} else {
						SetMotion("NPC_Default", 0);
						m_defaultMotion = true;
					}
				}
			}

			if (m_flags & 1) {
				for (std::list<sAlpha>::iterator it = m_alphaList.begin(); it != m_alphaList.end();) {
					std::list<sAlpha>::iterator cur = it;
					++it;

					if (it == m_alphaList.end()) {
						m_alpha = (unsigned char)(int)cur->m_value;
					} else if (m_motionTime > cur->m_time && m_motionTime < it->m_time) {
						float k = (m_motionTime - cur->m_time) / (it->m_time - cur->m_time);
						m_alpha = (unsigned char)(int)((1.0f - k) * cur->m_value + k * it->m_value);
						break;
					}
				}
			} else if (m_defaultMotion) {
				CheckTarget(dt);
			}
		}
	}
}

// npc.cpp:2656
void ntNpc_BlackSparrow::Display(void) {
	if (!WSingleton<COption>::Instance()->vIsNpcEnabled() && GOLFDOC()->m_currentHole != 0x13) {
		return;
	}

	if (m_pPet && m_active && !(m_motionTime < 0.0f)) {
		if (m_flags & 1) {
			m_pPet->SetAlpha(m_alpha);
		}

		m_pPet->ApplyBones(m_pPet->GetBones(0, m_motionTime, 0, 0, false), Abs(m_motionParam) > 0.0f ? RotMat(m_motionParam, 2) * m_matrix * 1.0f : m_matrix * 1.0f);
		m_pPet->Transform(g_view, 1.0f, 0);
		m_pPet->UpdateBSphere();

		if (g_view->InFrustum(m_pPet->GetBoundSphere())) {
			m_pPet->UpdateLightSource(&g_lightset, false, 0);
			m_pPet->Render(g_view, true, 1.0f, g_view->InFrustumSafe(m_pPet->GetBoundBox()), false, false);
		}
	}
}

// npc.cpp:2683
void ntNpc_BlackSparrow::CheckTarget(float dt) {
	if ((m_matrix.pivot - m_pos).Magnitude() < 1.0f) {
		if (m_replayIndex >= m_recordedPath.size() || m_recorded) {
			int tries = 0;
			int cell[3];
			int index;
			int i;
			bool found;

			do {
				tries++;

				int rnd[3];
				for (i = 0; i < 3; i++) {
					rnd[i] = rand() % 3 - 1;
				}

				int pos[3];
				pos[0] = ((int *)m_pBuffer)[0] % m_gridX;
				pos[1] = ((int *)m_pBuffer)[0] / (m_gridZ * m_gridX);
				pos[2] = ((int *)m_pBuffer)[0] % (m_gridZ * m_gridX) / m_gridX;

				cell[0] = Between(0, rnd[0] + pos[0], m_gridX - 1);
				cell[1] = Between(0, rnd[1] + pos[1], m_gridY - 1);
				cell[2] = Between(0, rnd[2] + pos[2], m_gridZ - 1);
				index = (m_gridZ * cell[1] + cell[2]) * m_gridX + cell[0];

				found = false;
				for (i = 0; i < m_gridCount; i++) {
					if (index == ((int *)m_pBuffer)[i]) {
						found = true;
						break;
					}
				}
			} while (found && tries < 5);

			for (i = m_gridCount - 1; i >= 1; i--) {
				((int *)m_pBuffer)[i] = ((int *)m_pBuffer)[i - 1];
			}
			((int *)m_pBuffer)[0] = index;

			m_pos.x = Random(0.0f, m_cellX, -1) + cell[0] * m_cellX + m_area.min.x;
			if (m_gridY > 1)
				m_pos.y = Random(0.0f, m_cellY, -1) + cell[1] * m_cellY + m_area.min.y;
			m_pos.z = Random(0.0f, m_cellZ, -1) + cell[2] * m_cellZ + m_area.min.z;

			if (m_recorded)
				m_recordedPath.push_back(m_pos);
		} else {
			m_pos = m_recordedPath[m_replayIndex];
			m_replayIndex++;
		}
	}

	if (m_gridY > 1) {
		m_matrix.pivot += m_unk74 * (m_pos - m_matrix.pivot).Normalize() * dt;
	} else {
		float angle = CalcDeltaAngle(m_pos - m_matrix.pivot, m_matrix.za * -1.0f);

		if (Abs(angle) > data_9f0fc0) {
			m_matrix = RotMat(Between(-(dt * m_turnRate), angle, dt * m_turnRate), 1) * m_matrix;
		}

		m_matrix.pivot += m_matrix.za * -1.0f * m_unk74 * dt;
		m_matrix.pivot = GetPVS().GetGroundPoint(m_matrix.pivot, true, 0, false);
	}
}

// npc.cpp:2769
ntNpc_PolaShip::ntNpc_PolaShip(const char *name, float speed) {
	Init(name, 0);
	m_hidden = true;
	m_distance = 0;
	m_speed = speed;
}

// npc.cpp:2779
ntNpc_PolaShip::~ntNpc_PolaShip(void) {
}

// npc.cpp:2783
void ntNpc_PolaShip::Process(float dt) {
	if (m_active && m_pPet) {
		m_motionTime += dt;

		if (m_hidden) {
			if (!(m_motionTime > m_speed)) {
				return;
			}

			m_motionTime = 0;
			m_hidden = false;
			SetMotion("NPC_Act02", -1.0f);
		}

		ScriptProcess(m_pMotion, m_motionTime - dt, m_motionTime);

		if (m_motionTime > m_pMotion->e) {
			m_motionTime -= m_pMotion->e - m_pMotion->s;
		}

		m_distance += dt * 10.0f;

		CheckTarget(dt);
	}
}

// npc.cpp:2814
void ntNpc_PolaShip::Display(void) {
	if (m_active && m_hidden != true) {
		CCreature::Display();
	}
}

// npc.cpp:2822
void ntNpc_PolaShip::CheckTarget(float dt) {
	WVector next = m_spline.Evaluate(m_distance + 0.1f);
	m_matrix.pivot = m_spline.Evaluate(m_distance);

	if (WisEqual(next, m_nodes[m_nodes.size() - 1], 1e-5f)) {
		m_distance = 0;
	}

	next.y = m_matrix.pivot.y;

	m_matrix = MakeCameraMatrix(next, m_matrix.pivot);
}

// npc.cpp:2836
void ntNpc_PolaShip::SetArea(const std::vector<WVector> &points, float radius, unsigned long flags) {
	CCreature::SetArea(points, radius, flags);

	WVector *pPoints = (WVector *)_alloca(points.size() * sizeof(WVector));
	float *pTimes = (float *)_alloca(points.size() * sizeof(float));

	pPoints[0] = points[0];
	pTimes[0] = 0;

	for (unsigned int i = 1; i < points.size(); i++) {
		pPoints[i] = points[i];
		pTimes[i] = pTimes[i - 1] + (points[i] - points[i - 1]).Magnitude();
	}

	m_spline.Init(pPoints, pTimes, points.size());
}

// npc.cpp:2858
ntNpc_PolaPenguin1::ntNpc_PolaPenguin1(const char *name, float speed) {
	m_unk74 = speed;
	Init(name, 0);
}

// npc.cpp:2865
ntNpc_PolaPenguin1::~ntNpc_PolaPenguin1(void) {
}

// npc.cpp:2869
void ntNpc_PolaPenguin1::Process(float dt) {
	if (m_pPet && m_active) {
		float time = dt + m_motionTime;
		m_motionTime = time;
		if (!(time < 0.0f)) {
			ScriptProcess(m_pMotion, time - dt, time);
			if (m_motionTime > m_pMotion->e) {
				if (m_flags & 1) {
					m_active = false;
					m_motionTime = m_pMotion->s;
				} else {
					m_motionTime -= m_pMotion->e - m_pMotion->s;
				}
			}
			CheckTarget(dt);
		}
	}
}

// npc.cpp:2897
void ntNpc_PolaPenguin1::CheckTarget(float dt) {
	if ((m_matrix.pivot - m_pos).Magnitude() < 1.0f) {
		FindTarget();
	}

	float angle = CalcDeltaAngle(m_pos - m_matrix.pivot, m_matrix.za * -1.0f);

	if (Abs(angle) > data_9f0fc0) {
		m_matrix = RotMat(Between(-(dt * m_turnRate), angle, dt * m_turnRate), 1) * m_matrix;
	}

	m_matrix.pivot += m_matrix.za * -1.0f * m_unk74 * dt;
	m_matrix.pivot = GetPVS().GetGroundPoint(m_matrix.pivot, true, 0, false);
}

// npc.cpp:2912
void ntNpc_PolaPenguin1::FindTarget(void) {
	for (int i = 0; i < 3; i++) {
		float t = Random(0.0f, 1.0f, -1);
		m_pos.p[i] = m_area.min.p[i] * t + m_area.max.p[i] * (1.0f - t);
	}
	m_pos = GetPVS().GetGroundPoint(m_pos, false, 0, false);
}

// npc.cpp:2931
ntNpc_PolaPenguin2::ntNpc_PolaPenguin2(const char *name) {
	m_unk74 = 2.0f;
	Init(name, 0);
	m_unk120 = true;
	SetMotion("NPC_Act0", 0);
}

// npc.cpp:2941
ntNpc_PolaPenguin2::~ntNpc_PolaPenguin2(void) {
}

// npc.cpp:2946
void ntNpc_PolaPenguin2::Process(float dt) {
	if (m_pPet && m_active) {
		float time = dt + m_motionTime;
		m_motionTime = time;
		if (!(time < 0.0f)) {
			ScriptProcess(m_pMotion, time - dt, time);
			if (m_motionTime > m_pMotion->e) {
				if (m_flags & 1) {
					m_active = false;
					m_motionTime = m_pMotion->s;
				} else {
					m_motionTime -= m_pMotion->e - m_pMotion->s;
					if (m_unk120 == true && IsSlidable() == true) {
						SetMotion("NPC_Default", 0);
						m_unk120 = false;
					} else {
						SetMotion("NPC_Act0", 0);
						m_unk120 = true;
					}
				}
			}
			CheckTarget(dt);
		}
	}
}

// npc.cpp:2985
void ntNpc_PolaPenguin2::Display(void) {
	if (!WSingleton<COption>::Instance()->vIsNpcEnabled() && GOLFDOC()->m_currentHole != 0x13) {
		return;
	}

	if (m_pPet && m_active && !(m_motionTime < 0.0f)) {
		if (m_flags & 1) {
			m_pPet->SetAlpha(m_alpha);
		}

		WVector delta = m_pPet->GetDeltaVec(m_pMotion->name, m_motionTime);
		delta = RotVec(delta, m_matrix);

		m_matrix.pivot -= delta;

		m_pPet->ApplyBones(m_pPet->GetBones(0, m_motionTime, 0, 0, false), m_matrix);
		m_pPet->Transform(g_view, 1.0f, 0);
		m_pPet->UpdateBSphere();

		m_matrix.pivot += delta;

		if (g_view->InFrustum(m_pPet->GetBoundSphere())) {
			m_pPet->UpdateLightSource(&g_lightset, false, 0);
			m_pPet->Render(g_view, true, 1.0f, g_view->InFrustumSafe(m_pPet->GetBoundBox()), false, false);
		}
	}
}

// npc.cpp:3030
void ntNpc_PolaPenguin2::CheckTarget(float dt) {
	if ((m_matrix.pivot - m_pos).Magnitude() < 1.0f) {
		FindTarget();
	}

	if (m_unk120) {
		float angle = CalcDeltaAngle(m_pos - m_matrix.pivot, m_matrix.za * -1.0f);

		if (Abs(angle) > data_9f0fc0) {
			m_matrix = RotMat(Between(-(dt * m_turnRate), angle, dt * m_turnRate), 1) * m_matrix;
		}

		m_matrix.pivot += m_matrix.za * -1.0f * m_unk74 * dt;
		m_matrix.pivot = GetPVS().GetGroundPoint(m_matrix.pivot, true, 0, false);
	} else {
		WVector delta;
		delta = m_pPet->GetDeltaVec(m_pMotion->name, m_motionTime - dt, m_motionTime);
		delta = RotVec(delta, m_matrix);

		m_matrix.pivot += delta;
		m_matrix.pivot = GetPVS().GetGroundPoint(m_matrix.pivot, true, 0, false);
	}
}

// npc.cpp:3056
void ntNpc_PolaPenguin2::FindTarget(void) {
	for (int i = 0; i < 3; i++) {
		float r = Random(0, 1.0f, -1);
		(&m_pos.x)[i] = (1.0f - r) * (&m_area.max.x)[i] + r * (&m_area.min.x)[i];
	}

	while (m_area.IsInclude(m_pos)) {
		m_pos += (m_pos - m_matrix.pivot).Normalize() * 3.2f;
	}

	m_pos = GetPVS().GetGroundPoint(m_pos, true, 0, false);
}

// npc.cpp:3072
bool ntNpc_PolaPenguin2::IsSlidable(void) {
	float angle = CalcDeltaAngle(m_pos - m_matrix.pivot, m_matrix.za * -1.0f);

	if (Abs(angle) > data_9f0fc0)
		return false;

	w_motion_data *motion = m_pPet->FindMotionData("NPC_Default");

	if (!motion)
		return false;

	WVector delta = m_pos - m_matrix.pivot;
	WVector move;
	move = m_pPet->GetDeltaVec("NPC_Default", motion->e);

	if (delta.Magnitude() < move.Magnitude())
		return false;

	return true;
}

// npc.cpp:3104
CNPC_BIRD::CNPC_BIRD(const char *name, float speed) {
	m_unk74 = speed;
	Init(name, 0);
}

// npc.cpp:3109
CNPC_BIRD::~CNPC_BIRD(void) {
}

// npc.cpp:3114
void CNPC_BIRD::Process(float dt) {
	if (m_pPet && m_active) {
		float time = dt + m_motionTime;
		m_motionTime = time;
		if (!(time < 0.0f)) {
			ScriptProcess(m_pMotion, time - dt, time);
			if (m_motionTime > m_pMotion->e) {
				if (m_flags & 1) {
					m_active = false;
					m_motionTime = m_pMotion->s;
				} else {
					m_motionTime -= m_pMotion->e - m_pMotion->s;
				}
			}
			CheckTarget(dt);
		}
	}
}

// npc.cpp:3141
void CNPC_BIRD::CheckTarget(float dt) {
	if ((m_matrix.pivot - m_pos).Magnitude() < 1.0f) {
		FindTarget();
	}

	float angle = CalcDeltaAngle(m_pos - m_matrix.pivot, m_matrix.za * -1.0f);

	if (Abs(angle) > data_9f0fc0) {
		m_matrix = RotMat(Between(-(dt * m_turnRate), angle, dt * m_turnRate), 1) * m_matrix;
	}

	m_matrix.pivot += m_matrix.za * -1.0f * m_unk74 * dt;
	m_matrix.pivot = GetPVS().GetGroundPoint(m_matrix.pivot, true, 0, false);
}

// npc.cpp:3156
void CNPC_BIRD::FindTarget(void) {
	for (int i = 0; i < 3; i++) {
		float t = Random(0.0f, 1.0f, -1);
		m_pos.p[i] = m_area.min.p[i] * t + m_area.max.p[i] * (1.0f - t);
	}
	m_pos = GetPVS().GetGroundPoint(m_pos, false, 0, false);
}

// npc.cpp:3170
CNPC_PARROT::CNPC_PARROT(const char *name) {
	m_unk74 = 10.0f;
	Init(name, 0);
	m_unk7C = 0.7853982f;
	m_unk84 = 48.0f;
}

// npc.cpp:3178
CNPC_PARROT::~CNPC_PARROT(void) {
}

// npc.cpp:3182
CNPC_TORTOISE::CNPC_TORTOISE(const char *name, float speed) {
	Init(name, 0);
	m_nodeIndex = 1;
	m_unk74 = speed * 2.0f;
	m_unk7C = 0.7853982f;
	m_turnRate = 2.0943952f;
	m_climbRate = 0;
	m_rollRate = 1.5707964f;
	m_state = 2;
}

// npc.cpp:3195
CNPC_TORTOISE::~CNPC_TORTOISE(void) {
}

// npc.cpp:3200
void CNPC_TORTOISE::Process(float dt) {
	if (m_pPet && m_active) {
		float time = dt + m_motionTime;
		m_motionTime = time;
		if (!(time < 0.0f)) {
			ScriptProcess(m_pMotion, time - dt, time);
			if (m_motionTime > m_pMotion->e) {
				m_motionTime -= m_pMotion->e;
				SetMotion("NPC_Default", 0);
			}
			CheckTarget(dt);
		}
	}
}

// npc.cpp:3222
void CNPC_TORTOISE::CheckTarget(float dt) {
	WVector d = m_pos - m_matrix.pivot;
	d.y = 0.0f;

	if (d.Magnitude() < Max(dt, 0.1f) * m_unk74) {
		if (m_replayIndex >= m_recordedPath.size() || m_recorded) {
			if (m_nodeIndex == m_nodes.size() - 1)
				m_nodeIndex = 0;

			m_nodeIndex = (m_nodeIndex + 1) % m_nodes.size();
			m_pos = m_nodes[m_nodeIndex];
			m_pos += (m_unkAC * RotMat(Random(-1.5707964f, 1.5707964f, -1), 0).za) * RotMat(Random(-3.1415927f, 3.1415927f, -1), 1);

			d = m_pos - m_matrix.pivot;
			d.y = 0.0f;

			if (m_recorded)
				m_recordedPath.push_back(m_pos);
		} else {
			if (m_replayIndex < m_recordedPath.size()) {
				m_pos = m_recordedPath[m_replayIndex];
				m_replayIndex++;
			} else {
				m_replayIndex = 0;
			}
		}
	}

	float angle = CalcDeltaAngle(d, m_matrix.za * -1.0f);

	if (Abs(angle) > data_9f0fc0) {
		m_matrix = RotMat(Between(-(dt * m_turnRate), angle, dt * m_turnRate), 1) * m_matrix;
	}

	m_matrix.pivot += m_matrix.za * -1.0f * m_unk74 * dt;
	m_matrix.pivot.y += dt * m_climbRate;
	m_matrix.pivot = GetPVS().GetGroundPoint(m_matrix.pivot, true, 0, false);
}

// npc.cpp:3281
CNPC_TRSHIP::CNPC_TRSHIP(const char *name, float speed) {
	Init(name, 0);
	m_distance = 0;

	if (Between(-1.0f, speed, 1.0f) == speed)
		m_speed = 1.0f;
	else
		m_speed = speed;

	m_hidden = true;
}

// npc.cpp:3298
CNPC_TRSHIP::~CNPC_TRSHIP(void) {
}

// npc.cpp:3303
void CNPC_TRSHIP::Process(float dt) {
	if (m_active && m_pPet) {
		m_motionTime += dt;

		if (m_hidden) {
			if (!(m_motionTime > 0.0f)) {
				return;
			}

			m_motionTime = 0;
			m_hidden = false;
			SetMotion("NPC_default", -1.0f);
		}

		ScriptProcess(m_pMotion, m_motionTime - dt, m_motionTime);

		if (m_motionTime > m_pMotion->e) {
			m_motionTime -= m_pMotion->e - m_pMotion->s;
		}

		m_distance += dt * 10.0f;

		CheckTarget(dt);
	}
}

// npc.cpp:3334
void CNPC_TRSHIP::Display(void) {
	if (m_active && m_hidden != true) {
		CCreature::Display();
	}
}

// npc.cpp:3344
void CNPC_TRSHIP::CheckTarget(float dt) {
	WVector next = m_spline.Evaluate(m_distance + 0.1f);
	m_matrix.pivot = m_spline.Evaluate(m_distance);

	if (WisEqual(next, m_nodes[m_nodes.size() - 1], 1e-5f)) {
		m_distance = 0;
	}

	next.y = m_matrix.pivot.y;

	m_matrix = MakeCameraMatrix(next, m_matrix.pivot);
}

// npc.cpp:3358
void CNPC_TRSHIP::SetArea(const std::vector<WVector> &points, float radius, unsigned long flags) {
	CCreature::SetArea(points, radius, flags);

	WVector *pPoints = (WVector *)_alloca(points.size() * sizeof(WVector));
	float *pTimes = (float *)_alloca(points.size() * sizeof(float));

	pPoints[0] = points[0];
	pTimes[0] = 0;

	for (unsigned int i = 1; i < points.size(); i++) {
		pPoints[i] = points[i];

		float length = (points[i] - points[i - 1]).Magnitude();
		pTimes[i] = pTimes[i - 1] + (length != 0.0f ? length : 1.0f) / m_speed;
	}

	m_spline.Init(pPoints, pTimes, points.size());
}

// npc.cpp:3462
ntNpc_AreaWalker::ntNpc_AreaWalker(const char *name) {
	m_walking = true;
	m_unk74 = 2.0f;
	Init(name, 0);
}

// npc.cpp:3469
ntNpc_AreaWalker::~ntNpc_AreaWalker(void) {
}

// npc.cpp:3473
void ntNpc_AreaWalker::Process(float dt) {
	if (m_pPet && m_active) {
		float time = dt + m_motionTime;
		m_motionTime = time;
		if (!(time < 0.0f)) {
			ScriptProcess(m_pMotion, time - dt, time);
			if (m_motionTime > m_pMotion->e) {
				if (m_flags & 1) {
					m_active = false;
					m_motionTime = m_pMotion->s;
				} else {
					SelectNextMotion();
				}
			}
			CheckTarget(dt);
		}
	}
}

// npc.cpp:3501
void ntNpc_AreaWalker::CheckTarget(float dt) {
	if ((m_matrix.pivot - m_pos).Magnitude() < 1.0f) {
		FindTarget();
	}

	if (m_walking) {
		float angle = CalcDeltaAngle(m_pos - m_matrix.pivot, m_matrix.za * -1.0f);

		if (Abs(angle) > data_9f0fc0) {
			m_matrix = RotMat(Between(-(dt * m_turnRate), angle, dt * m_turnRate), 1) * m_matrix;
		}

		m_matrix.pivot += m_matrix.za * -1.0f * m_unk74 * dt;
		m_matrix.pivot = GetPVS().GetGroundPoint(m_matrix.pivot, true, 0, false);
	} else {
		WVector delta;
		delta = m_pPet->GetDeltaVec(m_pMotion->name, m_motionTime - dt, m_motionTime);
		delta = RotVec(delta, m_matrix);

		m_matrix.pivot += delta;
		m_matrix.pivot = GetPVS().GetGroundPoint(m_matrix.pivot, true, 0, false);
	}
}

// npc.cpp:3527
void ntNpc_AreaWalker::FindTarget(void) {
	for (int i = 0; i < 3; i++) {
		float r = Random(0, 1.0f, -1);
		(&m_pos.x)[i] = (1.0f - r) * (&m_area.max.x)[i] + r * (&m_area.min.x)[i];
	}

	while (m_area.IsInclude(m_pos)) {
		m_pos += (m_pos - m_matrix.pivot).Normalize() * 3.2f;
	}

	m_pos = GetPVS().GetGroundPoint(m_pos, true, 0, false);
}

// npc.cpp:3543
void ntNpc_AreaWalker::AddWalkingMotion(const char *name) {
	std::vector<std::string>::iterator it;
	it = std::find(m_walkingMotions.begin(), m_walkingMotions.end(), name);
	if (it == m_walkingMotions.end()) {
		m_walkingMotions.push_back(name);
	}
}

// npc.cpp:3550
void ntNpc_AreaWalker::SelectNextMotion(void) {
	std::map<std::string, std::list<MotionMap> >::iterator map = m_motionMaps.find(m_pMotion->name);

	if (map == m_motionMaps.end())
		return;

	int roll = rand() % 100;
	std::list<MotionMap> &motions = map->second;

	for (std::list<MotionMap>::iterator it = motions.begin(); it != motions.end(); ++it) {
		if (it->m_threshold > roll) {
			const char *name = it->m_motion.c_str();

			if (name) {
				SetMotion(name, 0);
				char *const &motionName = m_pMotion->name;
				std::vector<std::string>::iterator walking = m_walkingMotions.begin();
				std::vector<std::string>::iterator last = m_walkingMotions.end();
				for (; walking != last; ++walking) {
					if (*walking == motionName)
						break;
				}
				m_walking = walking != m_walkingMotions.end();
			}
			return;
		}
	}
}

// npc.cpp:3589
ntNpc_WizCityArea::ntNpc_WizCityArea(const char *name) : ntNpc_AreaWalker(name) {
	ConstructNextMotion();
	AddWalkingMotion("NPC_Default");
	int choice = rand() % 2;
	if (choice < 1) {
		SetMotion("NPC_Default", -1.0f);
	} else {
		m_walking = false;
		SetMotion("NPC_Ato", -1.0f);
	}
}

// npc.cpp:3604
void ntNpc_WizCityArea::Display(void) {
	WMatrix matrix = m_matrix; m_matrix *= m_scale;
	CCreature::Display();
	m_matrix = matrix;
}

// npc.cpp:3612
void ntNpc_WizCityArea::ConstructNextMotion(void) {
	MotionMap map;
	std::list<MotionMap> motions;
	map.m_motion = "NPC_Ato";

	map.m_threshold = 100;
	motions.push_back(map);
	m_motionMaps.insert(std::make_pair("NPC_Default", motions));

	map.m_motion = "NPC_Default";
	map.m_threshold = 100;
	motions.clear();
	motions.push_back(map);
	m_motionMaps.insert(std::make_pair("NPC_Ato", motions));
}

// npc.cpp:3632
ntNpc_NodeWalker::ntNpc_NodeWalker(const char *name, float speed) {
	m_unk74 = speed;
	m_splineLength = 0;
	m_distance = 0;
	Init(name, 0);
}

// npc.cpp:3641
ntNpc_NodeWalker::~ntNpc_NodeWalker(void) {
}

// npc.cpp:3645
void ntNpc_NodeWalker::Process(float dt) {
	m_distance += dt * m_unk74;
	CCreature::Process(dt);
}

// npc.cpp:3651
void ntNpc_NodeWalker::SetArea(const std::vector<WVector> &points, float radius, unsigned long flags) {
	CCreature::SetArea(points, radius, flags);

	m_nodes.push_back(points[0]);
	m_nodes.push_back(points[1]);

	MakeSpline(m_nodes, m_splineLength, m_spline);
}

// npc.cpp:3664
void ntNpc_NodeWalker::CheckTarget(float dt) {
	WVector next = m_spline.Evaluate(m_distance + 0.1f);
	m_matrix.pivot = m_spline.Evaluate(m_distance);

	if (m_distance > m_splineLength) {
		m_distance = 0;
	}

	next = GetPVS().GetGroundPoint(next, true, 0, false);
	m_matrix.pivot = GetPVS().GetGroundPoint(m_matrix.pivot, true, 0, false);

	if (next.y < m_matrix.pivot.y)
		next.y = m_matrix.pivot.y;

	m_matrix = MakeCameraMatrix(next, m_matrix.pivot);
}

// npc.cpp:3684
void ntNpc_NodeWalker::MakeSpline(const std::vector<WVector> &points, float &length, ntCatmullRom &spline) {
	int count = points.size();
	WVector *pPoints = (WVector *)_alloca(count * sizeof(WVector));
	float *pTimes = (float *)_alloca(count * sizeof(float));

	pPoints[0] = points[0];
	pTimes[0] = 0;

	for (int i = 1; i < count; i++) {
		pPoints[i] = points[i];
		pTimes[i] = pTimes[i - 1] + (points[i] - points[i - 1]).Magnitude();
	}

	// The original reads the second-to-last time, not the total.
	length = pTimes[count - 2];

	spline.Init(pPoints, pTimes, count);
}

// npc.cpp:3711
ntNpc_WizCityNode::ntNpc_WizCityNode(const char *name, float speed) : ntNpc_NodeWalker(name, speed) {
	m_scale = 1.0f;
}

// npc.cpp:3716
void ntNpc_WizCityNode::Display(void) {
	WMatrix matrix = m_matrix; m_matrix *= m_scale;
	CCreature::Display();
	m_matrix = matrix;
}

// npc.cpp:3729
CDisposable::CDisposable(const char *name) {
	unsigned long &flags = m_flags;
	flags |= 1;
	Init(name, 0);
}

// npc.cpp:3735
void CDisposable::PlayEvent(void) {
	CCreature::PlayEvent();

	m_matrix.za = m_nodes[0] - m_nodes[1];
	m_matrix.za.y = 0;
	m_matrix.za.Normalize();
	m_matrix.ya = WVector::UNIT_POS_Y;
	m_matrix.xa = WCrossProduct(m_matrix.ya, m_matrix.za);
	m_matrix.pivot = GetPVS().GetGroundPoint(m_nodes[0], true, 0, true);
}

// npc.cpp:3752
COnGameNet::COnGameNet(const char *name, unsigned char playerIndex) : m_hTexture(0) {
	Init(name, 0);
	m_unk74 = 0.5f;
	m_playerIndex = playerIndex;
	m_hTexture = g_resrcmng->LoadTexture("camera.jpg", 0, 0, MakeStr("camera_%d.jpg", playerIndex));
	w_texlist *texture = g_resrcmng->FindTexture("camera.jpg");

	if (texture) {
		m_pPet->ChangeTextureByHandle(m_hTexture, texture->texhandle, 0);
	}
}

// npc.cpp:3768
COnGameNet::~COnGameNet(void) {
	if (g_resrcmng && m_hTexture) { g_resrcmng->Release(m_hTexture); m_hTexture = 0; }
}

// npc.cpp:3773
void COnGameNet::Process(float dt) {
	if (GOLFDOC()->m_myPlayerIndex != m_playerIndex) {
		m_active = false;
		return;
	}

	m_active = true;
	CCreature::Process(dt);
}

// npc.cpp:3785
void COnGameNet::CheckTarget(float dt) {
	if (GolfBall().m_preDataIndex >= 0 && GOLFDOC()->GameMode() != 0x1000) return;

	{
		MsgObject msg(0, 0x124, m_playerIndex, (intptr_t)&m_playerMatrix, 0, 0, 0);
		(!CTaskManager::Instance() ? 0 : CTaskManager::Instance()->GetCurrentTask())->GetActor("Player") << msg;
	}

	WVector dir = m_playerMatrix.za;

	dir.y = 0.0f; dir.Normalize();

	if (GolfBall().m_groundType == 2)
		m_pos = m_playerMatrix.pivot - dir * (0.8f * 3.2f);
	else
		m_pos = m_playerMatrix.pivot - dir * (1.2f * 3.2f);
	m_pos.y = GolfBall().m_pos.y + 1.2f * 3.2f;

	dir = m_pos - m_matrix.pivot;
	float dist = dir.Magnitude();
	float t = MinOf(dist * 0.15625f, 1.0f);

	m_matrix.za = (1.0f - t) * (GolfBall().m_pos - m_matrix.pivot) + t * dir;

	m_matrix.za.y = 0.0f; m_matrix.za.Normalize();
	m_matrix.za *= -1.0f;
	m_matrix.ya = WVector::UNIT_POS_Y;
	m_matrix.xa = WCrossProduct(m_matrix.ya, m_matrix.za);

	WVector pivot = m_matrix.pivot;
	m_matrix.pivot = WVector::ZERO;

	m_matrix = RotMat(-0.5235988f, 1) * m_matrix;

	m_matrix.pivot = pivot;
	if (dist < 3.2f * 10.0) {
		if (dist > 0.5)
			m_matrix.pivot += dir * (dist * m_unk74) * dt;
	} else {
		m_matrix.pivot = m_pos;
	}
}

// npc.cpp:3836
ntCatmullRom::ntCatmullRom(void) : m_pPoints(0), m_pTimes(0), m_pLengths(0), m_totalLength(0), m_count(0) {
}

// npc.cpp:3840
ntCatmullRom::~ntCatmullRom(void) {
	Clean();
}

// npc.cpp:3845
bool ntCatmullRom::Init(const WVector *points, const float *times, int count) {
	if (m_count)
		return false;

	if (count < 4 || !points || !times)
		return false;

	m_pPoints = new WVector[count];
	m_pTimes = new float[count];
	m_count = count;
	unsigned int i;
	for (i = 0; i < count; i++) {
		m_pPoints[i] = points[i];
		m_pTimes[i] = times[i];
	}

	int segments = count - 1;
	m_pLengths = new float[segments];
	m_totalLength = 0;
	for (i = 0; i < segments; i++) {
		m_pLengths[i] = SegmentArcLength(i, 0.0f, 1.0f);
		m_totalLength += m_pLengths[i];
	}

	return true;
}

// npc.cpp:3883
void ntCatmullRom::Clean(void) {
	if (m_pPoints) { delete[] m_pPoints; m_pPoints = 0; }
	if (m_pTimes) { delete[] m_pTimes; m_pTimes = 0; }
	if (m_pLengths) { delete[] m_pLengths; m_pLengths = 0; }

	m_totalLength = 0;
	m_count = 0;
}

// npc.cpp:3894
WVector ntCatmullRom::Evaluate(float t) {
	if (m_count < 2)
		return WVector::ZERO;

	if (t <= m_pTimes[0])
		return m_pPoints[0];
	if (t >= m_pTimes[m_count - 1])
		return m_pPoints[m_count - 1];

	int i;
	for (i = 0; i < m_count - 1; i++) {
		if (t <= m_pTimes[i + 1])
			break;
	}

	float start = m_pTimes[i];
	float u = (t - start) / (m_pTimes[i + 1] - start);
	if (i == 0) {
		WVector a = m_pPoints[0] - m_pPoints[1] * 2.0f + m_pPoints[2];
		WVector b = m_pPoints[1] * 4.0f - m_pPoints[0] * 3.0f - m_pPoints[2];

		return (a * u + b) * (u * 0.5f) + m_pPoints[0];
	}

	if (i >= m_count - 2) {
		i = m_count - 2;
		WVector a = m_pPoints[i - 1] - m_pPoints[i] * 2.0f + m_pPoints[i + 1];
		WVector b = m_pPoints[i + 1] - m_pPoints[i - 1];

		return (a * u + b) * (u * 0.5f) + m_pPoints[i];
	}

	WVector a = m_pPoints[i] * 3.0f - m_pPoints[i - 1] - m_pPoints[i + 1] * 3.0f + m_pPoints[i + 2];
	WVector b = m_pPoints[i - 1] * 2.0f - m_pPoints[i] * 5.0f + m_pPoints[i + 1] * 4.0f - m_pPoints[i + 2];
	WVector c = m_pPoints[i + 1] - m_pPoints[i - 1];
	return ((a * u + b) * u + c) * (u * 0.5f) + m_pPoints[i];
}


// npc.cpp:3961
WVector ntCatmullRom::Velocity(float t) {
	if (m_count < 2)
		return WVector::ZERO;

	if (t <= m_pTimes[0])
		t = 0.0f;
	else if (t > m_pTimes[m_count - 1])
		t = m_pTimes[m_count - 1];

	int i;
	for (i = 0; i < m_count - 1; i++) {
		if (t <= m_pTimes[i + 1])
			break;
	}

	float start = m_pTimes[i];
	float u = (t - start) / (m_pTimes[i + 1] - start);
	if (i == 0) {
		WVector a = m_pPoints[0] - m_pPoints[1] * 2.0f + m_pPoints[2];
		WVector b = m_pPoints[1] * 4.0f - m_pPoints[0] * 3.0f - m_pPoints[2];

		return b * 0.5f + a * u;
	}

	if (i >= m_count - 2) {
		i = m_count - 2;
		WVector a = m_pPoints[i - 1] - m_pPoints[i] * 2.0f + m_pPoints[i + 1];
		WVector b = m_pPoints[i + 1] - m_pPoints[i - 1];

		return b * 0.5f + a * u;
	}

	WVector a = m_pPoints[i] * 3.0f - m_pPoints[i - 1] - m_pPoints[i + 1] * 3.0f + m_pPoints[i + 2];
	WVector b = m_pPoints[i - 1] * 2.0f - m_pPoints[i] * 5.0f + m_pPoints[i + 1] * 4.0f - m_pPoints[i + 2];
	WVector c = m_pPoints[i + 1] - m_pPoints[i - 1];
	return c * 0.5f + (a * (u * 1.5f) + b) * u;
}


// npc.cpp:4027
WVector ntCatmullRom::Acceleration(float t) {
	if (m_count < 2)
		return WVector::ZERO;

	if (t <= m_pTimes[0])
		t = 0.0f;
	else if (t > m_pTimes[m_count - 1])
		t = m_pTimes[m_count - 1];

	int i;
	for (i = 0; i < m_count - 1; i++) {
		if (t <= m_pTimes[i + 1])
			break;
	}

	float start = m_pTimes[i];
	float u = (t - start) / (m_pTimes[i + 1] - start);
	if (i == 0)
		return m_pPoints[0] - m_pPoints[1] * 2.0f + m_pPoints[2];

	if (i >= m_count - 2) {
		i = m_count - 2;
		return m_pPoints[i - 1] - m_pPoints[i] * 2.0f + m_pPoints[i + 1];
	}

	WVector a = m_pPoints[i] * 3.0f - m_pPoints[i - 1] - m_pPoints[i + 1] * 3.0f + m_pPoints[i + 2];
	WVector b = m_pPoints[i - 1] * 2.0f - m_pPoints[i] * 5.0f + m_pPoints[i + 1] * 4.0f - m_pPoints[i + 2];
	return a * (u * 3.0f) + b;
}

// npc.cpp:4088
float ntCatmullRom::FindParameterByDistance(float t, float distance) {
	if (ArcLength(t, m_pTimes[m_count - 1]) < distance)
		return m_pTimes[m_count - 1];

	float range = m_pTimes[m_count - 1] - m_pTimes[0];

	float u = t + range / m_totalLength * distance;

	for (int i = 0; i < 32; i++) {
		float diff = ArcLength(t, u) - distance;
		if (WAbsBySign(diff) < 0.001f)
			return u;

		u -= diff / Velocity(u).Magnitude();
	}

	return data_9f0fc4;
}

// npc.cpp:4118
float ntCatmullRom::ArcLength(float t0, float t1) {
	if (t1 <= t0)
		return 0.0f;

	if (t0 < m_pTimes[0])
		t0 = m_pTimes[0];
	if (t1 > m_pTimes[m_count - 1])
		t1 = m_pTimes[m_count - 1];

	int i0;
	for (i0 = 0; i0 < m_count - 1; i0++) {
		if (t0 <= m_pTimes[i0 + 1])
			break;
	}
	float u0 = (t0 - m_pTimes[i0]) / (m_pTimes[i0 + 1] - m_pTimes[i0]);

	int i1;
	for (i1 = 0; i1 < m_count - 1; i1++) {
		if (t1 <= m_pTimes[i1 + 1])
			break;
	}
	float u1 = (t1 - m_pTimes[i1]) / (m_pTimes[i1 + 1] - m_pTimes[i1]);

	if (i0 == i1)
		return SegmentArcLength(i0, u0, u1);

	float sum = SegmentArcLength(i0, u0, 1.0f);
	for (int i = i0 + 1; i < i1; i++)
		sum += m_pLengths[i];
	return SegmentArcLength(i1, 0.0f, u1) + sum;
}


// 5-point Gauss-Legendre rule (.rdata 0x9F0FD4 and 0x9F0FEC).
static const float s_gaussWeights[5] = { 0.5688889f, 0.47862867f, 0.47862867f, 0.23692688f, 0.23692688f };
static const float s_gaussNodes[5] = { 0.0f, 0.53846931f, -0.53846931f, 0.90617985f, -0.90617985f };

// npc.cpp:4176
float ntCatmullRom::SegmentArcLength(int segment, float t0, float t1) {
	if (t1 <= t0)
		return 0.0f;
	if (t0 < 0.0f)
		t0 = 0.0f;
	if (t1 > 1.0f)
		t1 = 1.0f;

	// Polynomial coefficients of the derivative (same pieces as Velocity).
	float sum = 0.0f;
	WVector a, b, c;
	if (segment == 0) {
		a = m_pPoints[0] - m_pPoints[1] * 2.0f + m_pPoints[2];
		b = m_pPoints[1] * 4.0f - m_pPoints[0] * 3.0f - m_pPoints[2];
	} else if (segment >= m_count - 2) {
		segment = m_count - 2;
		a = m_pPoints[segment - 1] - m_pPoints[segment] * 2.0f + m_pPoints[segment + 1];
		b = m_pPoints[segment + 1] - m_pPoints[segment - 1];
	} else {
		a = m_pPoints[segment] * 3.0f - m_pPoints[segment - 1] - m_pPoints[segment + 1] * 3.0f + m_pPoints[segment + 2];
		b = m_pPoints[segment - 1] * 2.0f - m_pPoints[segment] * 5.0f + m_pPoints[segment + 1] * 4.0f - m_pPoints[segment + 2];
		c = m_pPoints[segment + 1] - m_pPoints[segment - 1];
	}

	float range = t1 - t0;
	for (int k = 0; k < 5; k++) {
		float x = (range * s_gaussNodes[k] + t0 + t1) * 0.5f;
		WVector v;
		if (segment == 0 || segment >= m_count - 2)
			v = b * 0.5f + a * x;
		else
			v = c * 0.5f + (a * (x * 1.5f) + b) * x;
		sum += s_gaussWeights[k] * v.Magnitude();
	}
	return sum * range * 0.5f;
}

