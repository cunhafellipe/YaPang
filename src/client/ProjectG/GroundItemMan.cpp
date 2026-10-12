// Original: d:\build\custom\temp\code\client\projectg\grounditemman.cpp
// actor.h names IActor in MsgObject before declaring it.
class IActor;

#include "GroundItemMan.h"

#include <new>

#include "ObjectFactory.h"
#include "grounditem.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "wview.h"
#include "wresrcmng.h"
#include "quadtree.h"

unsigned int GroundItemMan::ms_initCnt;

// grounditemman.cpp:42 (run-time type and factory registration of GroundItemMan)
const WRTTI GroundItemMan::m_RTTI("GroundItemMan", &NetSyncActor::m_RTTI);

IObject* GroundItemManMakeInstance(void) {
	return new GroundItemMan;
}

struct __impGroundItemMan {
	__impGroundItemMan(void) {
		ObjectFactory().AddObjectFunctor(GroundItemManMakeInstance, "GroundItemMan");
		ActorFactory().AddActorClass("GroundItemMan");
	}
};
static __impGroundItemMan s_impGroundItemMan;

// grounditemman.cpp:47
GroundItemMan::GroundItemMan(void) {
	WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0xa3, m_gimmickContainer);
}

// grounditemman.cpp:53
GroundItemMan::~GroundItemMan(void) {
}

// grounditemman.cpp:58
void GroundItemMan::OnLoad(void) {
	if (!OnlinePlay()) {
		m_gimmickContainer->LoadFromScript("test_table");
	}

	m_playerStats = new PlayerStats[GOLFDOC()->GetPlayerNum()];
}

// grounditemman.cpp:68
void GroundItemMan::OnReload(void) {
}

// grounditemman.cpp:75
void GroundItemMan::OnInit(void) {
	NetSyncActor::OnInit();

	m_gimmickContainer->ResetGimmikFlag(GOLFDOC()->m_currentHole, GimmickDispositionInformation::FLAG_NONE);

	Ut::MakeObjectName(m_sceneElemName, "SceneElemName", *this, ms_initCnt++);

	CSceneManager::Instance()->RegisterElement((eObjType)5, NULL, this, m_sceneElemName.c_str(), 0, NULL);

	unsigned long seed = m_gimmickContainer->GetRandomSeed();

	NSA_SetInitialRandomSeed(seed);
	NSA_GetRandomGenerator().seed(seed);

	// The texture list's cursor is advanced in place (only a const accessor exists).
	WList<w_texlist*> &texList = const_cast<WList<w_texlist*>&>(g_resrcmng->GetTextureList());

	for (w_texlist *tex = texList.Start(); tex != NULL; tex = texList.Next()) {
		FieldItem::eDisposeTextureType textureType = (FieldItem::eDisposeTextureType)0;

		if (m_gimmickContainer->NeedToMakeTriPtArray(tex->texname, &textureType)) {
			const std::vector<WPolySoup::sTriangle*> *triangles = GetPVS().GetTriArray(g_resrcmng->FindTexture(tex->texname)->texhandle);

			DisposeGroundItem(textureType, triangles, m_gimmickContainer->GetGimmickPosition(GOLFDOC()->m_currentHole));
		}
	}

	std::list<GroundItem*>::iterator it = m_groundItems.begin();

	for (; it != m_groundItems.end(); ++it) {
		GroundItem *item = *it;
		item->Initialize();
	}
}

// grounditemman.cpp:130
void GroundItemMan::OnDestroy(void) {
	delete[] m_playerStats;

	m_gimmickContainer->Release();

	OnHoleOut();

	NetSyncActor::OnDestroy();
}

// grounditemman.cpp:145
void GroundItemMan::HandleMsg(const MsgObject &msg) {
	NetSyncActor::HandleMsg(msg);

	switch (msg.m_msg) {
	case 0x277:
		*(std::vector<FieldItem::AcquiredItemInfo>**)msg.m_param1 = &m_gimmickContainer->GetAcquiredItemInfoArray();
		break;
	case 0x27c:
		*(int*)msg.m_param1 = m_groundItems.size();
		break;
	case 0x27b:
		EnableGroundItemAll(msg.m_param1 == 1);
		break;
	}
}

// grounditemman.cpp:179
void GroundItemMan::NSC_OnStep(bool active, bool replay, float elapsed, float syncTime, int frame) {
	std::list<GroundItem*>::iterator it = m_groundItems.begin();
	for (; it != m_groundItems.end(); ++it) {
		(*it)->NSC_OnStep(active, replay, elapsed, syncTime, frame);
	}
}

// grounditemman.cpp:191
void GroundItemMan::NSC_OnFirstNewShotFrame(float startTime, int frame) {
	bool jackpot = false;

	unsigned long chance = NSA_GetRandomGenerator().randInt(999999);

	if ((CSharedDoc::Instance()->m_roomPlayers[GOLFDOC()->m_myPlayerIndex].m_ballId & 0x3FFFFFF) == 0) {
		if (chance < 20000) {
			jackpot = true;
		}
	} else {
		if (chance < 40000) {
			jackpot = true;
		}
	}

	GroundItem::EnableGreenItemJackpot(jackpot);

	PlayerStats &stats = m_playerStats[GOLFDOC()->m_myPlayerIndex];

	stats.m_shotCount[0] = 0;
	stats.m_shotCount[1] = 0;
	stats.m_shotCount[2] = 0;

	std::list<GroundItem*>::iterator it = m_groundItems.begin();

	for (; it != m_groundItems.end(); ++it) {
		GroundItem *item = *it;
		item->NSC_OnFirstNewShotFrame(startTime, frame);
	}
}

// grounditemman.cpp:236
void GroundItemMan::NSC_OnFirstReplayFrame(int frame) {
	std::list<GroundItem*>::iterator it = m_groundItems.begin();

	for (; it != m_groundItems.end(); ++it) {
		GroundItem *item = *it;
		item->NSC_OnFirstReplayFrame(frame);
	}
}

// grounditemman.cpp:249
void GroundItemMan::OnHoleOut(void) {
	std::list<GroundItem*>::iterator it = m_groundItems.begin();

	for (; it != m_groundItems.end(); ++it) {
		GroundItem *item = *it;
		item->Terminate();
		delete item;
	}

	m_groundItems.clear();

	CSceneManager::Instance()->DeleteElement(m_sceneElemName.c_str());
}

// grounditemman.cpp:268
void GroundItemMan::Display(void) {
}

// grounditemman.cpp:273
void GroundItemMan::DebugDisplay(void) {
	std::list<GroundItem*>::iterator it = m_groundItems.begin();

	for (; it != m_groundItems.end(); ++it) {
		GroundItem *item = *it;
		item->DrawBoundingVolume();
	}

	const ThSphere &ball = GroundItem::GetBallBoundingVolume();
	g_view->DrawSphere((const WVector &)ball.center, ball.radius, 0xffff0000, 10, 0);
}

// grounditemman.cpp:290
int GroundItemMan::DisposeGroundItem(FieldItem::eDisposeTextureType textureType, const std::vector<WPolySoup::sTriangle*> *triangles, std::vector<GimmickDispositionInformation> &infos) {
	int count = 0;
	float minDist = m_gimmickContainer->GetMinDistBtwItems();
	std::vector<WVector> posList;
	Ut::BuildAppropriatePosList(triangles, minDist, posList);

	int numPos = posList.size();
	int numUnused = m_gimmickContainer->GetNumDisposeInfo(GOLFDOC()->m_currentHole, (GIMMICK_ITEM_TYPE)-1, GimmickDispositionInformation::FLAG_NONE);

	if (posList.empty()) {
		return 0;
	}

	int numItems = numPos;

	switch (textureType) {
	case 2:
		numItems = (int)(numPos * 0.7f);
		if (numItems > 50) {
			numItems = 50;
		}
		break;
	}

	if (numItems > numUnused) {
		numItems = numUnused;
	}

	int *order = new int[numPos];
	for (int i = 0; i < numPos; i++) {
		order[i] = i;
	}

	for (int i = 0; i < numPos * 3; i++) {
		int j = NSA_GetRandomGenerator().randInt(numPos - 1);
		std::swap(order[i % numPos], order[j]);
	}

	for (int i = 0; i < numItems; i++) {
		GimmickDispositionInformation *info = FindUnusedDisposeInfo(textureType, infos);

		if (info == NULL) {
			break;
		}

		Ut::Spatial spatial;
		spatial.m_position.x = posList[order[i]].x;
		spatial.m_position.y = posList[order[i]].y;
		spatial.m_position.z = posList[order[i]].z;

		GroundItem *item = CreateGroundItem(info->value4, info->itemType, textureType, spatial);

		m_groundItems.push_back(item);

		info->flag = (GimmickDispositionInformation::eFlag)1;
		count++;
	}

	// Allocated with new[], freed with the scalar delete (as the original does).
	if (order != NULL) {
		delete order;
	}

	return count;
}

// grounditemman.cpp:397
GimmickDispositionInformation *GroundItemMan::FindUnusedDisposeInfo(FieldItem::eDisposeTextureType textureType, std::vector<GimmickDispositionInformation> &infos) const {
	GimmickDispositionInformation *info = 0;
	switch (textureType) {
	case 1:
		info = m_gimmickContainer->FindDisposeInfo(GOLFDOC()->m_currentHole, (GIMMICK_ITEM_TYPE)0, GimmickDispositionInformation::FLAG_NONE);
		break;
	case 2:
		info = m_gimmickContainer->FindDisposeInfo(GOLFDOC()->m_currentHole, GIMMICK_ITEM_ANY, GimmickDispositionInformation::FLAG_NONE);
		break;
	}
	return info;
}

// grounditemman.cpp:424
GroundItem *GroundItemMan::CreateGroundItem(unsigned long id, GIMMICK_ITEM_TYPE itemType, FieldItem::eDisposeTextureType textureType, const Ut::Spatial &spatial) {
	GroundItem *item = 0;
	std::string puppetName;
	Ut::Spatial itemSpatial = spatial;

	switch (itemType) {
	case 0:
		puppetName = "coin.pet";

		itemSpatial.m_position.y += 0.1f;
		itemSpatial.m_scale = 10.0f;
		item = new GroundItem(this, id, itemType, textureType, puppetName.c_str(), itemSpatial);
		break;
	case 1:
		puppetName = "ct_event.pet";

		itemSpatial.m_position.y -= 3.0f;

		itemSpatial.m_scale = 14.0f;
		item = new GroundItem(this, id, itemType, textureType, puppetName.c_str(), itemSpatial);
		break;
	}

	return item;
}

// grounditemman.cpp:470
void GroundItemMan::EnableGroundItemAll(bool enable) {
	std::list<GroundItem*>::iterator it = m_groundItems.begin();

	for (; it != m_groundItems.end(); ++it) {
		(*it)->Enable(enable);
	}
}
