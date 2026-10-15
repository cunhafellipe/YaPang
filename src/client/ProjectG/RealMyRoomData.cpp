// Original: d:\build\custom\temp\code\client\projectg\realmyroomdata.cpp
#include "NetworkSystem.h"
#include <stdio.h>
#include "RealMyRoomData.h"
#include "GolfDoc.h"
#include "SharedDoc.h"
#include "Packet.h"
#include "TaskManager.h"
#include "actor.h"
#include "shared/Localize.h"
#include "scenemanager.h"
#include "wpuppet.h"
#include "wresrcmng.h"
#include "wbone.h"
#include "fresh.h"
#include "frform.h"
#include "frwndmanager.h"
#include "ObjectFactory.h"
#include "quadtree.h"

class WPlane;

extern LightSet g_lightset;

// The texture numbers my_room_01_%02d.dds cycles through (2 to 6) when the
// room is edited offline.
static int s_floorTextureIndex = 2;
static int s_wallTextureIndex = 2;

// Defined out of line by avatar_avatar.cpp (its header is not identified yet).
float RayIntersectAABB(const Waabb &aabb, const WVector &pos, const WVector &moveDelta, WPlane *pReturnPlane, float radius);

// Wangreal (wmath), declared by no header transcribed yet.
int WisZero(const float &f, float eps);

// polysoup.h:191
WPolySoup::sModel::sModel() {
	m_pQuads = NULL;
	m_pTriangles = NULL;
	m_pPuppet = NULL;
	m_windFactor = 1.0f;
	m_reservedByte = 0;
	m_alpha = 0;
	m_hitCount = 0;
	m_name[0] = '\0';
	m_baseName[0] = '\0';
	m_objIndex = 0;
}

// realmyroomdata.cpp:39
CRealMyRoomData::CRealMyRoomData(void) : m_modelList(16, 16) {
	m_modelList.Reset();

	m_pSelectModel = NULL;
	m_pPickedModel = NULL;

	m_state = 0;

	m_targetPos = WVector(0.0f, -2.0f, 0.0f);

	m_dragPos = m_pickPos = m_targetPos;

	m_bDropped = false;
}

// realmyroomdata.cpp:68
CRealMyRoomData::~CRealMyRoomData(void) {
	std::list<sFurniture_List>::iterator it;

	for (it = m_furnitureList.begin(); it != m_furnitureList.end(); ++it) {
		sFurniture_List furniture = *it;

		// Wall furniture whose placement changed is reported to the server.
		if ((furniture.m_typeId & 0x3FF800) == 0x2800) {
			for (cModel *pModel = m_modelList.Start(); pModel; pModel = m_modelList.Next()) {
				if (pModel->m_pFurniture->m_id == furniture.m_id) {
					if ((pModel->m_pFurniture->m_flags ^ furniture.m_flags) & 1) {
						if (IsLocalContent((localContentType_t)0x66)) {
							WSendPacket packet(GCT_OFFLINESHOP_REQUEST_STALL_EQUIP);
							packet.Encode4(CSharedDoc::Instance()->GetStallItem()->ownerUID);
							packet.Encode1(furniture.m_flags & 1);
							packet.Encode1(0);
							packet.Send(SEND_TO_DEFAULT);

							CTaskManager* pTaskManager = CTaskManager::Instance();
							(!pTaskManager ? 0 : pTaskManager->GetCurrentTask())->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
						}
						break;
					}
				}
			}
		}
	}

	ResetList(true);
}

// realmyroomdata.cpp:134
void CRealMyRoomData::OnDestory(void) {
	for (cModel *pModel = m_modelList.Start(); pModel; pModel = m_modelList.Next()) delete pModel; m_modelList.Reset();
}

// realmyroomdata.cpp:139
void CRealMyRoomData::Process(float elapsed) {
	// The original body (lines 140-189) compiles to nothing.
}

// realmyroomdata.cpp:192
void CRealMyRoomData::Render(void) {
	// The selection blinks: its alpha swings between 128 and 253.
	static bool s_bFadeIn = true;
	static int s_alpha = 128;

	if (m_pSelectModel != NULL) {
		if (m_state == 1 || m_state == 2) {
			if ((m_pSelectModel->m_pFurniture->m_typeId & 0x3FF800) == 0x7000) {
				CSceneManager::Instance()->SetAlpha(m_pSelectModel->m_pModelData->m_name, (unsigned char)s_alpha);

				if (s_bFadeIn == true)
					s_alpha += 3;
				else
					s_alpha -= 3;

				if (s_alpha > 253)
					s_bFadeIn = false;

				if (s_alpha < 128)
					s_bFadeIn = true;
			}
		}
	}
}

// realmyroomdata.cpp:342
void CRealMyRoomData::OnLoad(void) {
	char modelName[256];

	if (!WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
		if (ItemManager()->m_furnitureMap.size() != 0) {
			for (std::map<unsigned int, IFF_STRUCT::sFurniture>::iterator it = ItemManager()->m_furnitureMap.begin(); it != ItemManager()->m_furnitureMap.end(); ++it) {
				sFurniture_List furniture;
				memset(&furniture, 0, sizeof(furniture));
				furniture.m_typeId = it->second.m_typeId;
				furniture.m_id = rand() % 100;
				furniture.m_pos.x = it->second.m_pos[0];
				furniture.m_pos.y = it->second.m_pos[1];
				furniture.m_pos.z = it->second.m_pos[2];
				furniture.m_flags &= ~1;
				furniture.m_angle = it->second.m_angle;
				furniture.m_number = it->second.m_dayPrices[0];

				if (furniture.m_typeId)
					AddModel(furniture);
			}
		}
	} else if (CSharedDoc::Instance()->m_furnitureList.size() != 0) {
		for (std::list<sFurniture_List>::iterator it = CSharedDoc::Instance()->m_furnitureList.begin(); it != CSharedDoc::Instance()->m_furnitureList.end(); ++it) {
			IFF_STRUCT::sFurniture *pFurniture = ItemManager()->FindFurniture(it->m_typeId);
			if (pFurniture) {
				sprintf(modelName, "%s.pet", pFurniture->m_model);
				WVector pos;
				pos.x = pFurniture->m_pos[0];
				pos.y = pFurniture->m_pos[1];
				pos.z = pFurniture->m_pos[2];

				if (GOLFDOC()->m_pPolySoup->FindModel(modelName) == 0) {
					cModel *pModel = CreateModel(modelName, pos, pFurniture->m_angle, false, pFurniture->m_modelType);
					if (pModel == 0)
						return;

					m_modelList += pModel;
					m_editModel = *pModel;
					GOLFDOC()->m_pPolySoup->CreateDataModel(pModel->m_pModelData);
					pModel->m_pFurniture = &*it;
					m_state = 1;
					return;
				}
			}
		}
	}
}

// realmyroomdata.cpp:457
// Places a furniture record in the room: a placed one (flag bit 0) gets its
// model, a floor (category 8) or wall (category 9) record changes the room's
// texture; the record is also added to the shared document's list.
bool CRealMyRoomData::AddModel(sFurniture_List furniture) {
	char modelName[256];

	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
		std::list<sFurniture_List>::iterator it;
		for (it = CSharedDoc::Instance()->m_furnitureList.begin(); it != CSharedDoc::Instance()->m_furnitureList.end(); ++it) {
			if (it->m_id == furniture.m_id)
				break;
		}

		if (it == CSharedDoc::Instance()->m_furnitureList.end())
			CSharedDoc::Instance()->AddFurniture(furniture);
	}

	IFF_STRUCT::sFurniture *pFurniture = ItemManager()->FindFurniture(furniture.m_typeId);
	if (furniture.m_typeId == 0 || pFurniture == NULL)
		return true;

	sprintf(modelName, "%s.pet", pFurniture->m_model);
	WVector pos;
	pos.x = furniture.m_pos.x;
	pos.y = furniture.m_pos.y;
	pos.z = furniture.m_pos.z;

	if (furniture.m_flags & 1) {
		for (cModel *pOther = m_modelList.Start(); pOther; pOther = m_modelList.Next()) {
			if (pOther && pOther->m_pFurniture->m_id) {
				IFF_STRUCT::sFurniture *pOtherFurniture = ItemManager()->FindFurniture(pOther->m_pFurniture->m_typeId);

				if (pFurniture->m_rentalFlag != 0 && pOtherFurniture->m_rentalFlag == pFurniture->m_rentalFlag && pOtherFurniture->m_placeOnModel == 0) {
					FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
					pForm->SetMessage("\xC1\xDF\xBA\xB9\xB5\xC8 \xB1\xE2\xB4\xC9\xC0\xC7 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xC3\xDF\xB0\xA1\xB7\xCE \xB9\xE8\xC4\xA1\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9. \xB5\xBF\xC0\xCF \xB1\xE2\xB4\xC9\xC0\xC7 \xB0\xA1\xB1\xB8\xB8\xA6 \xC1\xA6\xB0\xC5\xC7\xCF\xB0\xED \xB9\xE8\xC4\xA1\xC7\xD8 \xC1\xD6\xBC\xBC\xBF\xE4", false);
					pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
					return false;
				}
			}
		}

		if (GOLFDOC()->m_pPolySoup->FindModel(modelName) == 0 && ((furniture.m_typeId >> 11) & 0x7FF) != 8 && ((furniture.m_typeId >> 11) & 0x7FF) != 9) {
			cModel *pModel = CreateModel(modelName, pos, furniture.m_angle, false, pFurniture->m_modelType);

			if (pModel == NULL) {
				FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				pForm->SetMessage(MakeStr("\xB0\xA1\xB1\xB8 \xBB\xFD\xBC\xBA\xBF\xA1 \xBD\xC7\xC6\xD0\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9. error %d ", furniture.m_id), false);
				pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
				return false;
			}

			GOLFDOC()->m_pPolySoup->ChangeDataStructure(pModel->m_pModelData);

			std::list<sFurniture_List>::iterator it;
			for (it = CSharedDoc::Instance()->m_furnitureList.begin(); it != CSharedDoc::Instance()->m_furnitureList.end(); ++it) {
				if (it->m_id == furniture.m_id)
					break;
			}

			if (it != CSharedDoc::Instance()->m_furnitureList.end()) {
				pModel->m_pFurniture = &*it;
			} else {
				CSharedDoc::Instance()->AddFurniture(furniture);

				pModel->m_pFurniture->m_id = furniture.m_id;
				pModel->m_pFurniture->m_typeId = furniture.m_typeId;
				pModel->m_pFurniture->m_number = furniture.m_number;
				pModel->m_pFurniture->m_pos.x = furniture.m_pos.x;
				pModel->m_pFurniture->m_pos.y = furniture.m_pos.y;
				pModel->m_pFurniture->m_pos.z = furniture.m_pos.z;
				pModel->m_pFurniture->m_angle = furniture.m_angle;
				pModel->m_pFurniture->m_flags ^= (pModel->m_pFurniture->m_flags ^ furniture.m_flags) & 1;
			}

			m_modelList += pModel;
			m_state = 1;
			if ((pModel->m_pFurniture->m_typeId & 0x3FF800) == 0x7000) {
				ChangePosterTexture(pModel->m_pFurniture->m_typeId, *pModel->m_pModelData);

				m_pSelectModel = pModel;
				m_state = 2;
			}
		} else if (GOLFDOC()->m_pPolySoup->FindModel(modelName) != 0 && pFurniture->m_placeOnModel == 1) {
			cModel *pModel = CreateModel(modelName, pos, furniture.m_angle, true, pFurniture->m_modelType);

			if (pModel == NULL) {
				FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				pForm->SetMessage(MakeStr("\xB0\xA1\xB1\xB8 \xBB\xFD\xBC\xBA\xBF\xA1 \xBD\xC7\xC6\xD0\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9. error %d ", furniture.m_id), false);
				pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
				return false;
			}

			GOLFDOC()->m_pPolySoup->ChangeDataStructure(pModel->m_pModelData);

			std::list<sFurniture_List>::iterator it;
			for (it = CSharedDoc::Instance()->m_furnitureList.begin(); it != CSharedDoc::Instance()->m_furnitureList.end(); ++it) {
				if (it->m_id == furniture.m_id)
					break;
			}

			if (it != CSharedDoc::Instance()->m_furnitureList.end()) {
				pModel->m_pFurniture = &*it;
			} else {
				CSharedDoc::Instance()->AddFurniture(furniture);

				pModel->m_pFurniture->m_id = furniture.m_id;
				pModel->m_pFurniture->m_typeId = furniture.m_typeId;
				pModel->m_pFurniture->m_number = furniture.m_number;
				pModel->m_pFurniture->m_pos.x = furniture.m_pos.x;
				pModel->m_pFurniture->m_pos.y = furniture.m_pos.y;
				pModel->m_pFurniture->m_pos.z = furniture.m_pos.z;
				pModel->m_pFurniture->m_angle = furniture.m_angle;
				pModel->m_pFurniture->m_flags ^= (pModel->m_pFurniture->m_flags ^ furniture.m_flags) & 1;
			}

			m_modelList += pModel;
			if ((pModel->m_pFurniture->m_typeId & 0x3FF800) == 0x7000) {
				ChangePosterTexture(pModel->m_pFurniture->m_typeId, *pModel->m_pModelData);

				m_pSelectModel = pModel;
				m_state = 2;
			}
		} else if (((furniture.m_typeId >> 11) & 0x7FF) == 8) {
			cModel *pModel;
			bool bRemoved = false;
			for (pModel = m_modelList.Start(); pModel; pModel = m_modelList.Next()) {
				if ((pModel->m_pFurniture->m_typeId & 0x3FF800) == 0x4000) {
					if (furniture.m_id == pModel->m_pFurniture->m_id) {
						pModel->m_pFurniture->m_flags &= ~1;
						pModel->m_pFurniture = NULL;
						m_modelList -= pModel;
						delete pModel;
						bRemoved = true;
					} else {
						pModel->m_pFurniture->m_flags |= 1;
						m_modelList -= pModel;
					}

					break;
				}
			}

			if (bRemoved) {
				ChangeFloorTexture(&furniture, true);
				return true;
			}

			ChangeFloorTexture(&furniture, false);
			bool bFound = false;
			pModel = new cModel;
			furniture.m_flags |= 1;
			std::list<sFurniture_List>::iterator it;
			for (it = CSharedDoc::Instance()->m_furnitureList.begin(); it != CSharedDoc::Instance()->m_furnitureList.end(); ++it) {
				if (it->m_typeId == furniture.m_typeId && it->m_id == furniture.m_id) {
					pModel->m_pFurniture = &*it;
					bFound = true;
				}
			}

			if (!bFound) {
				ChangeFloorTexture(&furniture, true);
				FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				pForm->SetMessage(MakeStr("\xBA\xAE\xC1\xF6 \xB9\xD7 \xB9\xD9\xB4\xDA\xC1\xA6 \xB9\xD9\xB2\xD9\xB1\xE2 \xBD\xC7\xC6\xD0- \xB0\xA1\xC1\xF6\xB0\xED\xC0\xD6\xB4\xC2 \xB0\xA1\xB1\xB8\xBF\xA1\xBC\xAD \xC7\xF6\xC0\xE7 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xC3\xA3\xC1\xF6 \xB8\xF8\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", furniture.m_id), false);
				pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
			} else {
				m_modelList += pModel;
			}
		} else if (((furniture.m_typeId >> 11) & 0x7FF) == 9) {
			cModel *pModel;
			bool bRemoved = false;
			for (pModel = m_modelList.Start(); pModel; pModel = m_modelList.Next()) {
				if ((pModel->m_pFurniture->m_typeId & 0x3FF800) == 0x4800) {
					if (furniture.m_id == pModel->m_pFurniture->m_id) {
						pModel->m_pFurniture->m_flags &= ~1;
						pModel->m_pFurniture = NULL;
						m_modelList -= pModel;
						delete pModel;
						bRemoved = true;
					} else {
						pModel->m_pFurniture->m_flags &= ~1;
						m_modelList -= pModel;
					}

					break;
				}
			}

			if (bRemoved) {
				ChangeWallTexture(&furniture, true);
				return true;
			}

			ChangeWallTexture(&furniture, false);
			bool bFound = false;
			pModel = new cModel;
			furniture.m_flags |= 1;
			std::list<sFurniture_List>::iterator it;
			for (it = CSharedDoc::Instance()->m_furnitureList.begin(); it != CSharedDoc::Instance()->m_furnitureList.end(); ++it) {
				if (it->m_typeId == furniture.m_typeId && it->m_id == furniture.m_id) {
					pModel->m_pFurniture = &*it;
					bFound = true;
				}
			}

			if (!bFound) {
				ChangeWallTexture(&furniture, true);
				FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				pForm->SetMessage(MakeStr("\xBA\xAE\xC1\xF6 \xB9\xD7 \xB9\xD9\xB4\xDA\xC1\xA6 \xB9\xD9\xB2\xD9\xB1\xE2 \xBD\xC7\xC6\xD0- \xB0\xA1\xC1\xF6\xB0\xED\xC0\xD6\xB4\xC2 \xB0\xA1\xB1\xB8\xBF\xA1\xBC\xAD \xC7\xF6\xC0\xE7 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xC3\xA3\xC1\xF6 \xB8\xF8\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", furniture.m_id), false);
				pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
			} else {
				m_modelList += pModel;
			}
		}
	} else {
		std::list<sFurniture_List>::iterator it;
		for (it = CSharedDoc::Instance()->m_furnitureList.begin(); it != CSharedDoc::Instance()->m_furnitureList.end(); ++it) {
			if (it->m_id == furniture.m_id)
				break;
		}

		if (it == CSharedDoc::Instance()->m_furnitureList.end())
			CSharedDoc::Instance()->AddFurniture(furniture);
	}

	return true;
}

// realmyroomdata.cpp:766
// Checks every placed model against the walls and the other furniture (the
// posters hanging on the same wall must not overlap), then with bSave copies
// the room's furniture list into the saved list.
bool CRealMyRoomData::SaveModel(bool bSave) {
	WList<cModel *> posterList(16, 16);
	bool bCollide = false;
	cModel *pModel;

	for (pModel = m_modelList.Start(); pModel; pModel = m_modelList.Next()) {
		IFF_STRUCT::sFurniture *pFurniture = ItemManager()->FindFurniture(pModel->m_pFurniture->m_typeId);

		unsigned long kind = (pModel->m_pFurniture->m_typeId >> 11) & 0x7FF;
		if (kind == 8 || kind == 9)
			continue;

		if (pModel->m_pModelData == NULL)
			continue;

		m_targetPos = pModel->m_pModelData->m_basePos;

		trace_s trace;
		WVector delta;
		if (pFurniture->m_modelType == 1 || pFurniture->m_modelType == 2 || pFurniture->m_modelType == 3) {
			GetPVS().TraceOBB_Avatar_genPosition(pModel->m_pModelData->m_basePos, m_targetPos, &trace, pModel->m_pModelData);

			if (trace.m_endPos.x == pModel->m_pModelData->m_basePos.x && trace.m_endPos.z == pModel->m_pModelData->m_basePos.z)
				m_targetPos = pModel->m_pModelData->m_basePos;
			else
				m_targetPos = trace.m_endPos;

			GetPVS().TraceOBB_WallSliding(pModel->m_pModelData->m_basePos, m_targetPos, &trace, pModel->m_pModelData, 0, false);
			delta = trace.m_endPos - pModel->m_pModelData->m_basePos;
			bCollide = GetPVS().TraceOBB_Object(pModel->m_pModelData->m_basePos, delta + pModel->m_pModelData->m_basePos, pModel->m_pModelData->m_aabb, &trace, pModel->m_pModelData, false);
		} else if (pFurniture->m_modelType == 5) {
			if ((pModel->m_pFurniture->m_typeId & 0x3FF800) == 0x7000) {
				posterList += pModel;
			} else {
				GetPVS().TraceOBB_WallSliding(pModel->m_pModelData->m_basePos, m_targetPos, &trace, pModel->m_pModelData, 1, false);
				delta = trace.m_endPos - pModel->m_pModelData->m_basePos;
				m_targetPos = delta + pModel->m_pModelData->m_basePos;
				bCollide = GetPVS().TraceOBB_Object(pModel->m_pModelData->m_basePos, m_targetPos, pModel->m_pModelData->m_aabb, &trace, pModel->m_pModelData, false);
			}
		}

		if (bCollide) {
			FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "rmr_notify", 0);
			pForm->SetMessage(MakeStr("[\xc0\xfa\xc0\xe5\xbd\xc7\xc6\xd0]\n\xc3\xe6\xb5\xb9\xc7\xcf\xb4\xc2 \xb0\xa1\xb1\xb8\xb0\xa1 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9."), false);
			pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
			return false;
		}
	}

	if (posterList.GetCount() > 0) {
		for (pModel = posterList.Start(); pModel; pModel = posterList.Next()) {
			Waabb other;
			WVector corner;

			switch (pModel->m_handle) {
			case 0: {
				Waabb box = pModel->m_pModelData->m_aabb;
				box.max.z = 0.0f;
				box.min.z = 0.0f;

				for (cModel *pOther = m_modelList.Start(); pOther; pOther = m_modelList.Next()) {
					if (pOther) {
						if ((pOther->m_pFurniture->m_typeId & 0x3FF800) == 0x7000 && pOther->m_pFurniture->m_id != pModel->m_pFurniture->m_id && pOther->m_handle == pModel->m_handle) {
							other = pOther->m_pModelData->m_aabb;
							other.max.z = 0.0f;
							other.min.z = 0.0f;

							corner = box.min;
							if (other.IsInclude(corner)) {
								bCollide = true;
								break;
							}

							corner = box.min;
							corner.x = box.max.x;
							if (other.IsInclude(corner)) {
								bCollide = true;
								break;
							}

							corner = box.max;
							if (other.IsInclude(corner)) {
								bCollide = true;
								break;
							}

							corner = box.max;
							corner.x = box.min.x;
							if (other.IsInclude(corner)) {
								bCollide = true;
								break;
							}
						}
					}
				}
				break;
			}
			case 1:
			case 2: {
				Waabb box = pModel->m_pModelData->m_aabb;
				box.max.x = 0.0f;
				box.min.x = 0.0f;

				for (cModel *pOther = m_modelList.Start(); pOther; pOther = m_modelList.Next()) {
					if (pOther) {
						if ((pOther->m_pFurniture->m_typeId & 0x3FF800) == 0x7000 && pOther->m_pFurniture->m_id != pModel->m_pFurniture->m_id && pOther->m_handle == pModel->m_handle) {
							other = pOther->m_pModelData->m_aabb;
							other.max.x = 0.0f;
							other.min.x = 0.0f;

							corner = box.min;
							if (other.IsInclude(corner)) {
								bCollide = true;
								break;
							}

							corner = box.min;
							corner.z = box.max.z;
							if (other.IsInclude(corner)) {
								bCollide = true;
								break;
							}

							corner = box.max;
							if (other.IsInclude(corner)) {
								bCollide = true;
								break;
							}

							corner = box.max;
							corner.z = box.min.z;
							if (other.IsInclude(corner)) {
								bCollide = true;
								break;
							}
						}
					}
				}
				break;
			}
			}
		}

		if (bCollide) {
			FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "rmr_notify", 0);
			pForm->SetMessage(MakeStr("[\xc0\xfa\xc0\xe5\xbd\xc7\xc6\xd0]\n\xc3\xe6\xb5\xb9\xc7\xcf\xb4\xc2 \xb0\xa1\xb1\xb8\xb0\xa1 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9."), false);
			pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
			return false;
		}
	}

	if (bSave)
		m_furnitureList.clear();

	if (IsLocalContent((localContentType_t)0x80)) {
		if (bSave) {
			for (std::list<sFurniture_List>::iterator it = CSharedDoc::Instance()->m_furnitureList.begin(); it != CSharedDoc::Instance()->m_furnitureList.end(); ++it) {
				cModel *pSaved = GetModelData(it->m_id, it->m_typeId);

				if (pSaved) {
					if (pSaved->m_pModelData) {
						it->m_pos.x = pSaved->m_pModelData->m_basePos.x;
						it->m_pos.y = pSaved->m_pModelData->m_basePos.y;
						it->m_pos.z = pSaved->m_pModelData->m_basePos.z;
					} else {
						it->m_pos.x = pSaved->m_pFurniture->m_pos.x;
						it->m_pos.y = pSaved->m_pFurniture->m_pos.y;
						it->m_pos.z = pSaved->m_pFurniture->m_pos.z;
					}
					it->m_angle = pSaved->m_angle;
				}

				sFurniture_List furniture = *it;

				m_furnitureList.push_back(furniture);
			}
		}
	} else {
		if (bSave) {
			for (std::list<sFurniture_List>::iterator it = CSharedDoc::Instance()->m_furnitureList.begin(); it != CSharedDoc::Instance()->m_furnitureList.end(); ++it) {
				sFurniture_List furniture = *it;

				m_furnitureList.push_back(furniture);
			}
		}
	}

	if (!IsLocalContent((localContentType_t)0x80) || bSave) {
		m_pSelectModel = NULL;
		m_state = 0;
	}

	return true;
}

// realmyroomdata.cpp:1065
cModel *CRealMyRoomData::CreateModel(const char *name) {
	m_pSelectModel = NULL;
	m_pPickedModel = NULL;

	if (!IsLocalContent((localContentType_t)0x56))
		return NULL;

	if (name == NULL)
		return NULL;

	WPuppet *pCenterPuppet = g_resrcmng->GetPuppet(name, false, true, false);

	pCenterPuppet->SetLightMode(0x4000000);
	w_bound_box *pBox = pCenterPuppet->m_bbList.Find("center");

	cModel *pModel = new cModel;
	pModel->m_pModelData = new WPolySoup::sModel;

	pModel->m_pModelData->m_subOrder = 1;
	pModel->m_pModelData->m_order = 0;

	WMatrix mat;
	mat.Reset();
	pModel->m_pModelData->m_matrix = mat;

	pModel->m_pModelData->m_scale = 1.0f;

	strncpy(pModel->m_pModelData->m_name, name, 0x1F);

	bool bSubOrder = pModel->m_pModelData->m_subOrder != 0;

	pModel->m_pModelData->m_pPuppet = g_resrcmng->GetPuppet(pModel->m_pModelData->m_name, true, bSubOrder, false);

	if (pBox != NULL) {
		pModel->m_pModelData->m_aabb.Reset();
		pModel->m_pModelData->m_aabb = pBox->info->aabb;

		pModel->m_pModelData->m_obb = pBox->bone->GetMatrix();
		pModel->m_pModelData->m_obb *= (pBox->info->aabb.max - pBox->info->aabb.min) * 0.5f;

		pModel->m_pModelData->m_obb.center = pBox->info->spherePivot * pBox->bone->GetMatrix();

		pModel->m_pModelData->m_basePos = (pModel->m_pModelData->m_aabb.max - pModel->m_pModelData->m_aabb.min) * 0.5f;

		pModel->m_pModelData->m_matrix.pivot.x = pModel->m_pModelData->m_basePos.x;
		pModel->m_pModelData->m_matrix.pivot.y = pModel->m_pModelData->m_basePos.y;
		pModel->m_pModelData->m_matrix.pivot.z = pModel->m_pModelData->m_basePos.z;

		pModel->m_pModelData->m_obb.center = pModel->m_pModelData->m_basePos;

		pModel->m_pModelData->m_aabb.min += pModel->m_pModelData->m_basePos;
		pModel->m_pModelData->m_aabb.max += pModel->m_pModelData->m_basePos;
	}

	pModel->m_pModelData->m_pPuppet->ApplyBones(NULL, pModel->m_pModelData->m_matrix);
	pModel->m_pModelData->m_pPuppet->UpdateBound(true, false);

	pModel->m_pModelData->m_pPuppet->UpdateLightSource(&g_lightset, false, 0);

	unsigned long flags = 0;
	if (pModel->m_pModelData->m_subOrder)
		flags = 4;

	if ((pModel->m_pModelData->m_order & 0x10) || pModel->m_pModelData->m_pPuppet->GetBoundSphere().radius > 128.0f)
		flags |= 2;

	if (pModel->m_pModelData->m_order & 8)
		flags |= 1;

	CSceneManager::Instance()->RegisterElement((eObjType)3, pModel->m_pModelData->m_pPuppet, NULL, pModel->m_pModelData->m_name, flags, &pModel->m_pModelData->m_sceneAabb);

	return pModel;
}

// realmyroomdata.cpp:1165
// Creates a piece of furniture at a position and an angle; type 2/3 stand
// on the floor, 5/6 hang on a wall. With bRename the scene name gets the
// first free "%02d" prefix among the room's models.
cModel *CRealMyRoomData::CreateModel(const char *name, WVector pos, float angle, bool bRename, short type) {
	cModel *pModel = NULL;

	m_pSelectModel = NULL;
	m_pPickedModel = NULL;

	if (!IsLocalContent((localContentType_t)0x56))
		return NULL;

	if (name == NULL)
		return NULL;

	WPuppet *pCenterPuppet = g_resrcmng->GetPuppet(name, false, true, false);

	if (pCenterPuppet == NULL)
		return NULL;

	pCenterPuppet->SetLightMode(0x4000000);
	w_bound_box *pBox = pCenterPuppet->m_bbList.Find("center");

	pModel = new cModel;
	pModel->m_pModelData = new WPolySoup::sModel;

	pModel->m_pModelData->m_subOrder = 1;
	pModel->m_pModelData->m_order = 0;

	WMatrix mat;
	mat.Reset();
	pModel->m_pModelData->m_matrix = mat;

	pModel->m_pModelData->m_scale = 1.0f;

	strncpy(pModel->m_pModelData->m_name, name, 0x1F);

	bool bSubOrder = pModel->m_pModelData->m_subOrder != 0;

	pModel->m_pModelData->m_pPuppet = g_resrcmng->GetPuppet(pModel->m_pModelData->m_name, true, bSubOrder, false);

	if (pBox != NULL) {
		pModel->m_pModelData->m_aabb.Reset();
		pModel->m_pModelData->m_aabb = pBox->info->aabb;

		pModel->m_pModelData->m_obb = pBox->bone->GetMatrix();
		pModel->m_pModelData->m_obb *= (pBox->info->aabb.max - pBox->info->aabb.min) * 0.5f;
		pModel->m_pModelData->m_obb.center = pBox->info->spherePivot * pBox->bone->GetMatrix();

		pModel->m_pModelData->m_basePos = pos;

		if (type == 2 || type == 3)
			pModel->m_pModelData->m_basePos.y = 0.0f;

		pModel->m_pModelData->m_obb.center = pModel->m_pModelData->m_basePos;

		pModel->SetPosition(pModel->m_pModelData->m_basePos);

		pModel->m_angle = 0.0f;
		pModel->SetRotate(angle);

		if (type == 2 || type == 3) {
			pModel->m_pModelData->m_obb.center.y = pBox->info->aabb.max.y * 0.5f;
		} else if (type == 5 || type == 6) {
			pModel->m_pModelData->m_obb.center.y = pos.y;
		} else {
			pModel->m_pModelData->m_obb.center.y = pBox->info->aabb.max.y * 0.5f;
		}

		pModel->m_pModelData->m_subOrder = 1;

		pModel->m_pModelData->m_aabb.min += pModel->m_pModelData->m_basePos;
		pModel->m_pModelData->m_aabb.max += pModel->m_pModelData->m_basePos;
		pModel->OBBToAABB();
	}

	pModel->m_pModelData->m_pPuppet->ApplyBones(NULL, pModel->m_pModelData->m_matrix);
	pModel->m_pModelData->m_pPuppet->UpdateBound(true, false);

	pModel->m_pModelData->m_pPuppet->UpdateLightSource(&g_lightset, false, 0);

	if (bRename) {
		int index = 1;
		char sceneName[0x20];
		for (;;) {
			sprintf(sceneName, "%02d%s", index, pModel->m_pModelData->m_name);

			bool bUsed = false;
			for (cModel *pOther = m_modelList.Start(); pOther; pOther = m_modelList.Next()) {
				if (pOther && pOther->m_pModelData) {
					if (stricmp(sceneName, pOther->m_pModelData->m_name) == 0) {
						bUsed = true;
						index++;
						break;
					}
				}
			}
			if (!bUsed) {
				strncpy(pModel->m_pModelData->m_name, sceneName, 0x20);
				break;
			}
		}
	}

	WVector normal;
	pModel->m_handle = GetPVS().Get_WallSlidingAngle(pModel->m_pModelData->m_basePos, pModel->m_pModelData, normal);

	unsigned long flags = 0;
	if (pModel->m_pModelData->m_subOrder)
		flags = 4;

	if ((pModel->m_pModelData->m_order & 0x10) || pModel->m_pModelData->m_pPuppet->GetBoundSphere().radius > 128.0f)
		flags |= 2;

	if (pModel->m_pModelData->m_order & 8)
		flags |= 1;

	CSceneManager::Instance()->RegisterElement((eObjType)3, pModel->m_pModelData->m_pPuppet, NULL, pModel->m_pModelData->m_name, flags, &pModel->m_pModelData->m_sceneAabb);

	return pModel;
}

// realmyroomdata.cpp:1324
void CRealMyRoomData::ResotreList(void) {
	ResetList(false);

	for (std::list<sFurniture_List>::iterator it = m_furnitureList.begin(); it != m_furnitureList.end(); ++it) {
		sFurniture_List furniture = *it;

		if (furniture.m_flags & 1) {
			std::list<sFurniture_List> &docList = CSharedDoc::Instance()->m_furnitureList;
			for (std::list<sFurniture_List>::iterator docIt = docList.begin(); docIt != docList.end(); ++docIt) {
				if (docIt->m_typeId == it->m_typeId && docIt->m_id == it->m_id) {
					docIt->m_flags |= 1;
					break;
				}
			}

			AddModel(furniture);
		}
	}
}

// realmyroomdata.cpp:1360
void CRealMyRoomData::DeleteList(void) {
	if (m_pSelectModel != NULL) {
		if (m_pPickedModel)
			m_pPickedModel = NULL;

		if (!GOLFDOC()->m_pPolySoup->DeleteDataStructure(m_pSelectModel->m_pModelData)) {
			FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			pForm->SetMessage("\xBB\xE8\xC1\xA6\xC7\xD2 \xB0\xA1\xB1\xB8\xB8\xA6 \xC3\xA3\xC1\xF6 \xB8\xF8\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
			return;
		}

		CSceneManager::Instance()->DeleteElement(m_pSelectModel->m_pModelData->m_name);

		m_pSelectModel->m_pModelData->m_pPuppet = NULL;
		m_modelList -= m_pSelectModel;
		delete m_pSelectModel->m_pModelData;
		if (m_pSelectModel) { delete m_pSelectModel; m_pSelectModel = NULL; }
	}
}

// realmyroomdata.cpp:1426
void CRealMyRoomData::ResetList(bool all) {
	cModel *pModel;

	for (pModel = m_modelList.Start(); pModel; pModel = m_modelList.Next()) {
		if (pModel && pModel->m_pModelData) {
			CSceneManager::Instance()->DeleteElement(pModel->m_pModelData->m_name);

			if (!GOLFDOC()->m_pPolySoup->DeleteDataStructure(pModel->m_pModelData)) {
				FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				pForm->SetMessage("\xBB\xE8\xC1\xA6\xC7\xD2 \xB0\xA1\xB1\xB8\xB8\xA6 \xC3\xA3\xC1\xF6 \xB8\xF8\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.", false);
				pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
				return;
			}
		}
	}

	ChangeWallTexture(NULL, true);
	ChangeFloorTexture(NULL, true);

	if (all) {
		for (pModel = m_modelList.Start(); pModel; pModel = m_modelList.Next()) {
			m_modelList -= pModel;

			if (pModel->m_pModelData) {
				pModel->m_pModelData->m_pPuppet = NULL;
				if (pModel->m_pModelData) { delete[] pModel->m_pModelData; pModel->m_pModelData = NULL; }
			} else if (pModel->m_pFurniture) {
				pModel->m_pFurniture = NULL;
			}

			delete pModel;
		}
	} else {
		for (pModel = m_modelList.Start(); pModel; pModel = m_modelList.Next()) {
			m_modelList -= pModel;

			if (pModel->m_pModelData) {
				pModel->m_pModelData->m_pPuppet = NULL;
				if (pModel->m_pModelData) { delete[] pModel->m_pModelData; pModel->m_pModelData = NULL; }
			}

			delete pModel;
		}
	}

	for (pModel = m_modelList.Start(); pModel; pModel = m_modelList.Next()) { if (pModel) delete pModel; } m_modelList.Reset();

	m_modelList.Reset();
	m_pSelectModel = NULL;
	m_pPickedModel = NULL;
}

// realmyroomdata.cpp:1511
cModel *CRealMyRoomData::ModelByRayIntersect(WVector const &origin, WVector const &dir) {
	for (cModel *pModel = m_modelList.Start(); pModel; pModel = m_modelList.Next()) {
		if (pModel && pModel->m_pModelData) {
			if (RayIntersectAABB(pModel->m_pModelData->m_aabb, origin, dir, NULL, 0) < 1.0f)
				return pModel;
		}
	}

	return NULL;
}

// realmyroomdata.cpp:1528
cModel *CRealMyRoomData::ModelByRayIntersectOBB(WVector const &pos) {
	if (m_state != 1)
		return NULL;

	for (cModel *pModel = m_modelList.Start(); pModel; pModel = m_modelList.Next()) {
		if (pModel && pModel->m_pModelData) {
			if (pModel->m_pModelData->m_obb.IsInclude(pos))
				return pModel;
		}
	}

	return NULL;
}

// realmyroomdata.cpp:1547
void CRealMyRoomData::ChangeFloorTexture(sFurniture_List *pList, bool bReset) {
	char newTex[256];
	char oldTex[256];

	if (!WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
		if (s_floorTextureIndex > 6)
			s_floorTextureIndex = 2;

		sprintf(newTex, "my_room_01_%02d.dds", s_floorTextureIndex);
		sprintf(oldTex, "floor_a.dds");

		GOLFDOC()->m_pPolySoup->ChangeFloorTexture(oldTex, newTex);
	}
	else {
		IFF_STRUCT::sFurniture *pFurniture;
		if (bReset)
			pFurniture = ItemManager()->FindFurniture(0x48004008);
		else
			pFurniture = ItemManager()->FindFurniture(pList->m_typeId);

		if (pFurniture) {
			for (int i = 0; i < 3; i++) {
				if (stricmp(pFurniture->m_texture[i], "") == 0 || stricmp(pFurniture->m_texture[i], "") == 0)
					break;

				sprintf(newTex, "%s.dds", pFurniture->m_texture[i]);
				sprintf(oldTex, "%s.dds", pFurniture->m_texture2[i]);

				GOLFDOC()->m_pPolySoup->ChangeFloorTexture(oldTex, newTex);
			}
		}
	}

	if (!WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT))
		s_floorTextureIndex++;
}

// realmyroomdata.cpp:1609
void CRealMyRoomData::ChangeWallTexture(sFurniture_List *pList, bool bReset) {
	char newTex[256];
	char oldTex[256];

	if (!WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
		if (s_wallTextureIndex > 6)
			s_wallTextureIndex = 2;

		sprintf(newTex, "my_room_01_%02d.dds", s_wallTextureIndex);
		sprintf(oldTex, "+wall_a.dds");
	}
	else {
		IFF_STRUCT::sFurniture *pFurniture;
		if (bReset)
			pFurniture = ItemManager()->FindFurniture(0x4800480E);
		else
			pFurniture = ItemManager()->FindFurniture(pList->m_typeId);

		if (pFurniture) {
			for (int i = 0; i < 3; i++) {
				if (stricmp(pFurniture->m_texture[i], "") == 0 || stricmp(pFurniture->m_texture[i], "") == 0)
					break;

				sprintf(newTex, "%s.dds", pFurniture->m_texture[i]);
				sprintf(oldTex, "%s.dds", pFurniture->m_texture2[i]);

				GOLFDOC()->m_pPolySoup->ChangeObjectTexture(oldTex, newTex, -1);
			}
		}
	}

	if (!WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT))
		s_wallTextureIndex++;
}

// realmyroomdata.cpp:1659
bool CRealMyRoomData::ChangePosterTexture(unsigned long typeId, WPolySoup::sModel &model) {
	char newTex[256];
	char oldTex[256];

	IFF_STRUCT::sFurniture *pFurniture = ItemManager()->FindFurniture(typeId);
	if (pFurniture) {
		for (int i = 0; i < 3; i++) {
			if (stricmp(pFurniture->m_texture[i], "") == 0 || stricmp(pFurniture->m_texture[i], "") == 0)
				break;

			if ((typeId & 0x3FF800) == 0x7000) {
				sprintf(newTex, "%s.jpg", pFurniture->m_texture[i]);
				sprintf(oldTex, "%s.jpg", pFurniture->m_texture2[i]);
			}
			else {
				sprintf(newTex, "%s.dds", pFurniture->m_texture[i]);
				sprintf(oldTex, "%s.dds", pFurniture->m_texture2[i]);
			}

			GOLFDOC()->m_pPolySoup->ChangeObjectTexture(oldTex, newTex, model.m_objIndex);
		}
	}

	return true;
}

// realmyroomdata.cpp:1695
void CRealMyRoomData::SetRotate(float angle) {
	if (m_state != 3 && m_state != 7)
		return;

	if (angle == 0.0f)
		return;

	if (m_pSelectModel == NULL)
		return;

	for (cModel *pModel = m_modelList.Start(); pModel; pModel = m_modelList.Next()) {
		if (pModel->m_pModelData == NULL || stricmp(pModel->m_pModelData->m_name, m_pSelectModel->m_pModelData->m_name) != 0)
			continue;

		Wobb obb; obb = pModel->m_pModelData->m_obb;
		WMatrix mat; mat = pModel->m_pModelData->m_matrix;

		pModel->SetRotate(angle * 0.03f);
		pModel->OBBToAABB();

		WVector pos; pos = ChangeModelPosition(7, pModel, pModel->m_pModelData);
		WPolySoup::sModel *pData = pModel->m_pModelData;
		if (!WisEqual(pos, pData->m_basePos, 1e-5f)) {
			pData->m_obb = obb;
			pModel->m_pModelData->m_matrix = mat;
			pModel->OBBToAABB();
			return;
		}

		pModel->Apply_ChangeModelData();
		m_pSelectModel = pModel;
		return;
	}
}

// realmyroomdata.cpp:1741
void CRealMyRoomData::SetTargetPosition(WVector const &pos, unsigned char mode) {
	if (m_state == 2) {
		switch (mode) {
		case 1:
			m_pickPos = m_pSelectModel->m_pModelData->m_basePos;
			m_targetPos = pos;
			break;
		case 2:
			m_targetPos = pos;
			m_dragPos = pos;
			MoveModelPosition(mode);
			break;
		case 3:
			m_bDropped = false; m_targetPos = pos;
			MoveModelPosition(mode);
			break;
		}
	}
}

// realmyroomdata.cpp:1777
// Removes from a velocity its part along a plane's normal (scaled by
// overbounce), the Quake way of sliding along a wall.
static void ClipVelocity(const WVector &in, const WVector &normal, WVector &out, float overbounce) {
	float backoff = WDotProduct(in, normal) * overbounce;
	out = in - normal * backoff;
}

// realmyroomdata.cpp:1783
// Moves the selected model toward the target position: floor furniture
// (types 1-3) slides along the floor, wall furniture (type 5) follows the
// wall it is dragged onto, turning to face the room. A drop onto another
// piece puts the model back where the saved list has it.
void CRealMyRoomData::MoveModelPosition(unsigned char mode) {
	IFF_STRUCT::sFurniture *pFurniture;
	WVector move(0.0f, 0.0f, 0.0f);
	trace_s trace;

	if (m_pSelectModel)
		pFurniture = ItemManager()->FindFurniture(m_pSelectModel->m_pFurniture->m_typeId);

	WVector delta(0.0f, 0.0f, 0.0f);

	if (pFurniture->m_modelType == 1 || pFurniture->m_modelType == 2 || pFurniture->m_modelType == 3) {
		GetPVS().TraceOBB_Avatar_genPosition(m_pSelectModel->m_pModelData->m_basePos, m_targetPos, &trace, m_pSelectModel->m_pModelData);

		if (trace.m_endPos.x == m_pSelectModel->m_pModelData->m_basePos.x && trace.m_endPos.z == m_pSelectModel->m_pModelData->m_basePos.z)
			m_targetPos = m_pSelectModel->m_pModelData->m_basePos;
		else
			m_targetPos = trace.m_endPos;

		GetPVS().TraceOBB_WallSliding(m_pSelectModel->m_pModelData->m_basePos, m_targetPos, &trace, m_pSelectModel->m_pModelData, 0, false);

		move = trace.m_endPos - m_pSelectModel->m_pModelData->m_basePos;
		delta = move;

		m_bDropped = GetPVS().TraceOBB_Object(m_pSelectModel->m_pModelData->m_basePos, delta + m_pSelectModel->m_pModelData->m_basePos, m_pSelectModel->m_pModelData->m_aabb, &trace, m_pSelectModel->m_pModelData, false);

		WVector slide = trace.m_endPos - m_pSelectModel->m_pModelData->m_basePos;

		m_targetPos = delta + m_pSelectModel->m_pModelData->m_basePos;

		if (mode == 2) {
			if (delta.x == 0.0f && delta.z == 0.0f)
				delta = move;
			else
				delta = slide;
		} else if (mode == 3) {
			delta = move;
		} else {
			delta = move;
		}

		if (pFurniture->m_modelType == 2 || pFurniture->m_modelType == 3)
			delta.y = 0.0f;

		m_pSelectModel->m_pModelData->m_basePos += delta;
		m_pSelectModel->m_pModelData->m_obb.center += delta;

		m_pSelectModel->m_pModelData->m_matrix.pivot.x = m_pSelectModel->m_pModelData->m_basePos.x;
		m_pSelectModel->m_pModelData->m_matrix.pivot.y = m_pSelectModel->m_pModelData->m_basePos.y;
		m_pSelectModel->m_pModelData->m_matrix.pivot.z = m_pSelectModel->m_pModelData->m_basePos.z;
		m_pSelectModel->m_pModelData->m_aabb.min += delta;
		m_pSelectModel->m_pModelData->m_aabb.max += delta;

		m_pSelectModel->m_pModelData->m_pPuppet->ApplyBones(NULL, m_pSelectModel->m_pModelData->m_matrix);
		m_pSelectModel->m_pModelData->m_pPuppet->UpdateBound(true, false);

		m_pSelectModel->Apply_ChangeModelData();
	} else if (pFurniture->m_modelType == 5) {
		if (GetPVS().CheckPickRoom(m_targetPos)) {
			WVector normal;
			int wall = GetPVS().Get_WallSlidingAngle(m_targetPos, m_pSelectModel->m_pModelData, normal);

			delta = m_targetPos - m_pSelectModel->m_pModelData->m_basePos;

			if (wall >= 0) {
				if (wall != m_pSelectModel->m_handle && m_pSelectModel->m_handle >= 0) {
					m_pSelectModel->m_handle = wall;

					move = WVector(0.0f, 0.0f, 0.0f);
					switch (wall) {
					case 0:
						if (m_pSelectModel->m_angle != 0.0f) {
							if (m_pSelectModel->m_angle == 360.0f)
								m_pSelectModel->SetRotate(-360.0f);
							else
								m_pSelectModel->SetRotate(360.0f - m_pSelectModel->m_angle);
						}
						break;
					case 1:
						if (m_pSelectModel->m_angle == 0.0f || m_pSelectModel->m_angle == 360.0f)
							m_pSelectModel->SetRotate(270.0f);
						break;
					case 2:
						if (m_pSelectModel->m_angle == 0.0f || m_pSelectModel->m_angle == 360.0f)
							m_pSelectModel->SetRotate(90.0f);
						break;
					}

					move = m_targetPos - m_pSelectModel->m_pModelData->m_basePos;

					m_pSelectModel->m_pModelData->m_basePos += move;
					m_pSelectModel->m_pModelData->m_obb.center += move;

					m_pSelectModel->SetPosition(m_pSelectModel->m_pModelData->m_basePos);

					m_pSelectModel->OBBToAABB();

					m_pSelectModel->m_pModelData->m_pPuppet->ApplyBones(NULL, m_pSelectModel->m_pModelData->m_matrix);
					m_pSelectModel->m_pModelData->m_pPuppet->UpdateBound(true, false);

					m_pSelectModel->Apply_ChangeModelData();
				}

				GetPVS().TraceOBB_WallSliding(m_pSelectModel->m_pModelData->m_basePos, m_targetPos, &trace, m_pSelectModel->m_pModelData, m_pSelectModel->m_handle + 1, false);

				delta = trace.m_endPos - m_pSelectModel->m_pModelData->m_basePos;

				m_pSelectModel->m_pModelData->m_basePos += delta;
				m_pSelectModel->m_pModelData->m_obb.center += delta;

				m_pSelectModel->SetPosition(m_pSelectModel->m_pModelData->m_basePos);

				m_pSelectModel->OBBToAABB();

				m_pSelectModel->m_pModelData->m_pPuppet->ApplyBones(NULL, m_pSelectModel->m_pModelData->m_matrix);
				m_pSelectModel->m_pModelData->m_pPuppet->UpdateBound(true, false);

				m_pSelectModel->Apply_ChangeModelData();

				if (((m_pSelectModel->m_pFurniture->m_typeId >> 11) & 0x7FF) == 0xE && mode == 2) {
					Waabb box;
					Waabb other;

					switch (m_pSelectModel->m_handle) {
					case 0:
						box = m_pSelectModel->m_pModelData->m_aabb;
						box.max.z = 0.0f;
						box.min.z = 0.0f;

						for (cModel *pOther = m_modelList.Start(); pOther; pOther = m_modelList.Next()) {
							if ((pOther->m_pFurniture->m_typeId & 0x3FF800) == 0x7000 && stricmp(pOther->m_pModelData->m_name, m_pSelectModel->m_pModelData->m_name) != 0) {
								other = pOther->m_pModelData->m_aabb;
								other.max.z = 0.0f;
								other.min.z = 0.0f;

								delta = box.min;
								if (other.IsInclude(delta)) {
									m_bDropped = true;
									break;
								}

								delta = box.min;
								delta.x = box.max.x;
								if (other.IsInclude(delta)) {
									m_bDropped = true;
									break;
								}

								delta = box.max;
								if (other.IsInclude(delta)) {
									m_bDropped = true;
									break;
								}

								delta = box.max;
								delta.x = box.min.x;
								if (other.IsInclude(delta)) {
									m_bDropped = true;
									break;
								}
							}
						}
						break;
					case 1:
					case 2:
						box = m_pSelectModel->m_pModelData->m_aabb;
						box.max.x = 0.0f;
						box.min.x = 0.0f;

						for (cModel *pOther = m_modelList.Start(); pOther; pOther = m_modelList.Next()) {
							if ((pOther->m_pFurniture->m_typeId & 0x3FF800) == 0x7000 && stricmp(pOther->m_pModelData->m_name, m_pSelectModel->m_pModelData->m_name) != 0) {
								other = pOther->m_pModelData->m_aabb;
								other.max.x = 0.0f;
								other.min.x = 0.0f;

								delta = box.min;
								if (other.IsInclude(delta)) {
									m_bDropped = true;
									break;
								}

								delta = box.min;
								delta.z = box.max.z;
								if (other.IsInclude(delta)) {
									m_bDropped = true;
									break;
								}

								delta = box.max;
								if (other.IsInclude(delta)) {
									m_bDropped = true;
									break;
								}

								delta = box.max;
								delta.z = box.min.z;
								if (other.IsInclude(delta)) {
									m_bDropped = true;
									break;
								}
							}
						}
						break;
					}
				} else if (((m_pSelectModel->m_pFurniture->m_typeId >> 11) & 0x7FF) != 0xE) {
					GetPVS().TraceOBB_Object(m_pSelectModel->m_pModelData->m_basePos, m_targetPos, m_pSelectModel->m_pModelData->m_aabb, &trace, m_pSelectModel->m_pModelData, false);

					move = trace.m_endPos - m_pSelectModel->m_pModelData->m_basePos;

					if (move.x == 0.0f && move.y == 0.0f && (m_targetPos.x != m_pSelectModel->m_pModelData->m_basePos.x || m_targetPos.y != m_pSelectModel->m_pModelData->m_basePos.y))
						m_bDropped = true;
					else
						m_bDropped = false;

					WVector step;
					if (mode == 2) {
						if (delta.x == 0.0f && delta.y == 0.0f)
							step = delta;
						else
							step = move;
					} else if (mode == 3) {
						step = delta;
					} else {
						step = delta;
					}

					step.z = 0.0f;
					m_pSelectModel->m_pModelData->m_basePos += step;
					m_pSelectModel->m_pModelData->m_obb.center += step;

					m_pSelectModel->SetPosition(m_pSelectModel->m_pModelData->m_basePos);

					m_pSelectModel->m_pModelData->m_aabb.min += step;
					m_pSelectModel->m_pModelData->m_aabb.max += step;

					m_pSelectModel->m_pModelData->m_pPuppet->ApplyBones(NULL, m_pSelectModel->m_pModelData->m_matrix);
					m_pSelectModel->m_pModelData->m_pPuppet->UpdateBound(true, false);

					m_pSelectModel->Apply_ChangeModelData();
				}
			}
		}
	}

	if (m_bDropped && mode == 2) {
		FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		pForm->SetMessage("\xb0\xa1\xb1\xb8\xb0\xa1 \xc3\xe6\xb5\xb9\xb5\xc7\xb4\xc2 \xc0\xa7\xc4\xa1\xbf\xa1 \xb3\xf5\xbf\xa9\xbc\xad \xb4\xd9\xbd\xc3 \xb9\xe8\xc4\xa1\xc7\xcf\xbc\xc5\xbe\xdf \xc7\xd5\xb4\xcf\xb4\xd9.", false);
		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);

		delta = WVector(0.0f, 0.0f, 0.0f);
		for (std::list<sFurniture_List>::iterator it = m_furnitureList.begin(); it != m_furnitureList.end(); ++it) {
			sFurniture_List furniture = *it;
			if (m_pSelectModel->m_pFurniture->m_id == furniture.m_id) {
				delta = furniture.m_pos;

				if (pFurniture->m_modelType == 2)
					delta.y = 0.0f;

				m_pSelectModel->SetRotate(furniture.m_angle - m_pSelectModel->m_angle);

				delta = delta - m_pSelectModel->m_pModelData->m_basePos;

				m_pSelectModel->m_pModelData->m_basePos += delta;
				m_pSelectModel->m_pModelData->m_obb.center += delta;

				m_pSelectModel->m_pModelData->m_matrix.pivot.x = m_pSelectModel->m_pModelData->m_basePos.x;
				m_pSelectModel->m_pModelData->m_matrix.pivot.y = m_pSelectModel->m_pModelData->m_basePos.y;
				m_pSelectModel->m_pModelData->m_matrix.pivot.z = m_pSelectModel->m_pModelData->m_basePos.z;

				m_pSelectModel->OBBToAABB();

				m_pSelectModel->m_pModelData->m_pPuppet->ApplyBones(NULL, m_pSelectModel->m_pModelData->m_matrix);
				m_pSelectModel->m_pModelData->m_pPuppet->UpdateBound(true, false);

				m_pSelectModel->Apply_ChangeModelData();

				m_pSelectModel->m_handle = GetPVS().Get_WallSlidingAngle(m_pSelectModel->m_pModelData->m_basePos, m_pSelectModel->m_pModelData, delta);
				break;
			}
		}
	}

	if (mode == 2) {
		if (m_pSelectModel) {
			if ((m_pSelectModel->m_pFurniture->m_typeId & 0x3FF800) == 0x7000)
				CSceneManager::Instance()->SetAlpha(m_pSelectModel->m_pModelData->m_name, 0xFF);
		}
	}
}

// realmyroomdata.cpp:2235
// The step the edited model takes toward the target position for an edit
// mode: along the floor (2), along the wall it hangs on (3), sliding along
// the walls (5), or pushed out of the other furniture (7).
WVector CRealMyRoomData::ChangeModelPosition(int mode, cModel *pModel, WPolySoup::sModel *pData) {
	trace_s trace;
	WVector move;
	WVector dir;
	WVector newPos;
	WVector result = WVector::ZERO;

	WVector roomMax = GetPVS().m_pBaseModel->m_aabb.max;

	switch (mode) {
	case 2: {
		move = m_targetPos - pModel->m_pModelData->m_basePos;

		move.z = move.y; move.y = 0.0f;

		dir = move;

		move.Normalize();

		move = move * 17.0f * 0.04f;

		if (fabs(dir.x) < fabs(move.x))
			move.x = dir.x;
		if (fabs(dir.z) < fabs(move.z))
			move.z = dir.z;

		newPos = move + pModel->m_pModelData->m_basePos;

		if (!GetPVS().TraceAABB_BackGround(pModel->m_pModelData->m_basePos, newPos, pModel->m_pModelData->m_aabb, &trace, pData))
			break;

		GetPVS().TraceOBB_Object(pModel->m_pModelData->m_basePos, newPos, pModel->m_pModelData->m_aabb, &trace, pData, false);

		move = trace.m_endPos - pModel->m_pModelData->m_basePos;
		move.y = 0.0f;

		result = move;
		break;
	}
	case 3: {
		move = m_targetPos - pModel->m_pModelData->m_basePos;

		move.y = 0.0f;
		move.z = (roomMax.z - 0.5f)
			- fabs(pModel->m_pModelData->m_aabb.max.z - pModel->m_pModelData->m_aabb.min.z);

		dir = move;

		move.Normalize();

		move = move * 17.0f * 0.04f;

		if (fabs(dir.x) < fabs(move.x))
			move.x = dir.x;
		if (fabs(dir.y) < fabs(move.y))
			move.y = dir.y;

		newPos = move + pModel->m_pModelData->m_basePos;

		if (!GetPVS().TraceAABB_BackGround(pModel->m_pModelData->m_basePos, newPos, pModel->m_pModelData->m_aabb, &trace, pData))
			break;

		GetPVS().TraceOBB_Object(pModel->m_pModelData->m_basePos, newPos, pModel->m_pModelData->m_aabb, &trace, pData, false);

		move = trace.m_endPos - pModel->m_pModelData->m_basePos;
		move.y = 0.0f;

		move.z = 0.0f;
		result = move;
		break;
	}
	case 5:
		result = SlideWallPosition(pModel, pData);
		break;
	case 7: {
		GetPVS().TraceOBB_Object(pModel->m_pModelData->m_basePos, pModel->m_pModelData->m_basePos, pModel->m_pModelData->m_aabb, &trace, pData, true);

		move = trace.m_endPos - pModel->m_pModelData->m_basePos;

		if (WAbsBySign(move.x) < 1e-5f && WisZero(move.z, 1e-5f)) {
			result = WVector::ZERO;
			break;
		}

		result = pModel->m_pModelData->m_basePos;
		break;
	}
	}

	return result;
}

// realmyroomdata.cpp:2411
// Moves the model a step toward the target position along the wall it
// faces, stopping at the walls and the other furniture.
WVector CRealMyRoomData::SlideWallPosition(cModel *pModel, WPolySoup::sModel *pData) {
	WVector zero = WVector::ZERO;

	WVector roomMax = GetPVS().m_pBaseModel->m_aabb.max;

	WMatrix rot;
	pModel->m_pModelData->m_matrix.GetRotMatrix(&rot);

	float angle = acos(rot.xx) * 57.29578f;

	WVector dir;
	WVector move;
	WVector newPos;

	dir = m_targetPos - pModel->m_pModelData->m_basePos;
	move = m_targetPos.Normalize() - pModel->m_pModelData->m_basePos;

	if (angle == 90.0f) {
		move.x = (roomMax.x - 0.5f) - fabs(pModel->m_pModelData->m_aabb.max.x - pModel->m_pModelData->m_aabb.min.x);
	} else if (angle == 0.0f) {
		move.z = (roomMax.z - 0.5f) - fabs(pModel->m_pModelData->m_aabb.max.z - pModel->m_pModelData->m_aabb.min.z);
	} else if (angle == 270.0f || angle == -90.0f) {
		move.x = (roomMax.x - 0.5f)
			- fabs(pModel->m_pModelData->m_aabb.max.x - pModel->m_pModelData->m_aabb.min.x);
	}

	move.Normalize();

	move = move * 17.0f * 0.04f;

	if (fabs(dir.x) < fabs(move.x))
		move.x = dir.x;
	if (fabs(dir.y) < fabs(move.y))
		move.y = dir.y;

	newPos = move + pModel->m_pModelData->m_basePos;

	trace_s trace;
	if (GetPVS().TraceAABB_BackGround(pModel->m_pModelData->m_basePos, newPos, pModel->m_pModelData->m_aabb, &trace, pData)) {
		GetPVS().TraceOBB_Object(pModel->m_pModelData->m_basePos, newPos, pModel->m_pModelData->m_aabb, &trace, pData, false);

		dir = trace.m_endPos - pModel->m_pModelData->m_basePos;
		if (angle == 90.0f) {
			dir.x = 0.0f;
		} else if (angle == 0.0f) {
			dir.z = 0.0f;
		} else if (angle == 270.0f || angle == -90.0f) {
			dir.x = 0.0f;
		}

		zero = dir;
	}

	return zero;
}

// realmyroomdata.cpp:2511
// Moves the selected model a step toward the target position against the
// room's back wall, sliding along what it hits.
void CRealMyRoomData::SetModelPosition(void) {
	WVector delta(0.0f, 0.0f, 0.0f);
	WPolySoup::sModel *pData = new WPolySoup::sModel;

	WVector roomMax = GetPVS().m_pBaseModel->m_aabb.max;

	for (cModel *pModel = m_modelList.Start(); pModel; pModel = m_modelList.Next()) {
		if (m_pSelectModel && strcmp(pModel->m_pModelData->m_name, m_pSelectModel->m_pModelData->m_name) == 0) {
			memcpy(pData, pModel->m_pModelData, sizeof(WPolySoup::sModel));

			IFF_STRUCT::sFurniture *pFurniture = ItemManager()->FindFurniture(pModel->m_pFurniture->m_typeId);
			WVector move;
			WVector newPos;

			move = m_targetPos - pModel->m_pModelData->m_basePos;
			move.z = (roomMax.z - 0.5f)
				- fabs(pModel->m_pModelData->m_aabb.max.z - pModel->m_pModelData->m_aabb.min.z);
			WVector dir = move;

			move.Normalize();

			move = move * 17.0f * 0.04f;
			if (fabs(dir.x) < fabs(move.x))
				move.x = dir.x;
			if (fabs(dir.y) < fabs(move.y))
				move.y = dir.y;

			newPos = move + pModel->m_pModelData->m_basePos;
			WVector slide(0.0f, 0.0f, 0.0f);

			trace_s trace;
			if (GetPVS().TraceAABB_BackGround(pModel->m_pModelData->m_basePos, newPos, pModel->m_pModelData->m_aabb, &trace, pModel->m_pModelData)) {
				GetPVS().TraceOBB_Object(pModel->m_pModelData->m_basePos, newPos, pModel->m_pModelData->m_aabb, &trace, pData, false);

				delta = trace.m_endPos - pModel->m_pModelData->m_basePos;

				if (pFurniture->m_modelType == 5) {
					delta.z = 0.0f;
					if (delta.x == 0.0f && delta.y == 0.0f) {
						if (WDotProduct(trace.m_plane.normal, move) < 0.99f) {
							ClipVelocity(move, trace.m_plane.normal, slide, 1.001f);

							newPos = pModel->m_pModelData->m_basePos;

							if (slide.x != 0.0f)
								newPos.x += slide.x;
							else
								newPos.y += slide.y;
						}
					}

					pModel->m_pModelData->m_basePos.x += delta.x;
					pModel->m_pModelData->m_basePos.y += delta.y;
				}
				else {
					delta.y = 0.0f;
					if (delta.x == 0.0f && delta.z == 0.0f) {
						if (WDotProduct(trace.m_plane.normal, move) < 0.99f) {
							ClipVelocity(move, trace.m_plane.normal, slide, 1.001f);

							newPos = pModel->m_pModelData->m_basePos;

							if (slide.x != 0.0f)
								newPos.x += slide.x;
							else
								newPos.z += slide.z;
						}
					}

					pModel->m_pModelData->m_basePos.x += delta.x;
					pModel->m_pModelData->m_basePos.z += delta.z;
				}
			}

			pModel->m_pModelData->m_basePos += delta;

			pModel->m_pModelData->m_aabb.min += delta;
			pModel->m_pModelData->m_aabb.max += delta;
			pModel->m_pModelData->m_obb.center += delta;

			pModel->m_pModelData->m_matrix.pivot.x = pModel->m_pModelData->m_basePos.x;
			pModel->m_pModelData->m_matrix.pivot.y = pModel->m_pModelData->m_basePos.y;
			pModel->m_pModelData->m_matrix.pivot.z = pModel->m_pModelData->m_basePos.z;

			pModel->Apply_ChangeModelData();
			return;
		}
	}
}

// realmyroomdata.cpp:2715
unsigned long CRealMyRoomData::GetState(void) {
	return m_state;
}

// realmyroomdata.cpp:2719
void CRealMyRoomData::SetState(unsigned long state) {
	switch (state) {
	case 6:
		DeleteList();
		break;
	}

	m_state = state;
}

// realmyroomdata.cpp:2748
void CRealMyRoomData::ListFurnitureSelect(sFurniture_List *pList) {
	for (cModel *pModel = m_modelList.Start(); pModel; pModel = m_modelList.Next()) {
		if (pModel && pModel->m_pModelData) {
			if (pModel->m_pFurniture->m_id == pList->m_id)
				m_pSelectModel = pModel;
		}
	}
}

// realmyroomdata.cpp:2771
bool CRealMyRoomData::IsArragementFurniture(unsigned long id) {
	bool bArranged = false;
	std::list<sFurniture_List>::iterator it;

	for (it = m_furnitureList.begin(); it != m_furnitureList.end(); ++it) {
		if (id == it->m_id) {
			if (it->m_flags & 1)
				bArranged = true;
			break;
		}
	}

	return bArranged;
}

// realmyroomdata.cpp:2795
cModel *CRealMyRoomData::GetModelData(unsigned long id, unsigned long typeId) {
	for (cModel *pModel = m_modelList.Start(); pModel; pModel = m_modelList.Next()) {
		if (pModel) {
			if (pModel->m_pFurniture->m_typeId == typeId && pModel->m_pFurniture->m_id == id)
				return pModel;
		}
	}

	return NULL;
}

// realmyroomdata.cpp:2821
bool CRealMyRoomData::IsModelData_SaveList(unsigned long id, unsigned long typeId) {
	bool bFound = false;
	std::list<sFurniture_List>::iterator it;

	for (it = m_furnitureList.begin(); it != m_furnitureList.end(); ++it) {
		if (id == it->m_id && typeId == it->m_typeId && (it->m_flags & 1)) {
			bFound = true;
			break;
		}
	}

	return bFound;
}

// realmyroomdata.cpp:2846
int CRealMyRoomData::IsFunritureValueChange(cModel &model) {
	int result = -1;
	for (std::list<sFurniture_List>::iterator it = m_furnitureList.begin(); it != m_furnitureList.end(); ++it) {
		if (model.m_pFurniture->m_id == it->m_id) {
			result = 0;

			if (model.m_pModelData) {
				if (it->m_pos.x != model.m_pModelData->m_basePos.x)
					result = 1;

				if (it->m_pos.y != model.m_pModelData->m_basePos.y)
					result = 1;

				if (it->m_pos.z != model.m_pModelData->m_basePos.z)
					result = 1;

				if (it->m_angle != model.m_angle)
					result = 1;

				if ((model.m_pFurniture->m_flags ^ it->m_flags) & 1)
					return 1;
			} else if ((model.m_pFurniture->m_flags ^ it->m_flags) & 1) {
				return 2;
			}

			break;
		}
	}

	return result;
}

// realmyroomdata.cpp:2921
cModel::cModel() {
	m_state = 1; m_pModelData = NULL;
	m_pFurniture = NULL;

	m_angle = 0.0f;
	m_handle = -1;
}

// realmyroomdata.cpp:2931
cModel::~cModel() {
}

// realmyroomdata.cpp:2935
void cModel::SetPosition(WVector const &pos) {
	m_pModelData->m_matrix.pivot.x = pos.x;
	m_pModelData->m_matrix.pivot.y = pos.y;
	m_pModelData->m_matrix.pivot.z = pos.z;
}

// realmyroomdata.cpp:2943
void cModel::SetRotate(float angle) {
	if (angle + m_angle < 0.0f)
		m_angle = 360.0f;

	if (angle + m_angle > 360.0f)
		m_angle = 0.0f;

	m_angle += angle;

	WMatrix mat;
	mat.Reset();

	float rad = angle * 0.017453292f;
	m_pModelData->m_matrix.Rotate(rad, 1);

	SetPosition(m_pModelData->m_basePos);

	mat.xa = m_pModelData->m_obb.extend[0];
	mat.ya = m_pModelData->m_obb.extend[1];
	mat.za = m_pModelData->m_obb.extend[2];

	mat.Rotate(rad, 1);

	m_pModelData->m_obb.center += mat.pivot;
	m_pModelData->m_obb.extend[0] = mat.xa;
	m_pModelData->m_obb.extend[1] = mat.ya;
	m_pModelData->m_obb.extend[2] = mat.za;

	if (m_pFurniture)
		m_pFurniture->m_angle = angle;
}

// realmyroomdata.cpp:2980
void cModel::Apply_ChangeModelData() {
	m_pModelData->m_pPuppet->ApplyBones(NULL, m_pModelData->m_matrix);
	m_pModelData->m_pPuppet->Transform(NULL, 1.0f, 0);
	m_pModelData->m_pPuppet->UpdateBSphere();

	CSceneManager::Instance()->SetPuppet(m_pModelData->m_name, m_pModelData->m_pPuppet);

	GOLFDOC()->m_pPolySoup->ChangeDataStructure(m_pModelData);
}

// realmyroomdata.cpp:3003
// Rebuilds the model's world box from the eight corners of its oriented box,
// starting from the room's box turned inside out (the height spans 0 to 30).
void cModel::OBBToAABB() {
	WPolySoup::sModel *pRoom = GOLFDOC()->m_pPolySoup->FindModel(NULL);

	// Starts inverted (min at the room's max, max at its min) so the corners
	// pull it to their bounds; the bottom is kept between 0 and 30.
	Waabb room = pRoom->m_aabb; Waabb box(room.max, room.min); box.min.y = 30.0f;

	WVector center; center = m_pModelData->m_obb.center;
	WVector ext0; ext0 = m_pModelData->m_obb.extend[0];
	WVector ext1; ext1 = m_pModelData->m_obb.extend[1];
	WVector ext2; ext2 = m_pModelData->m_obb.extend[2];

	box.max.y = 0.0f;
	// Each corner goes through a temporary copied into the array, and the
	// sums are grouped per corner as the original adds them (the ext1/ext2
	// pair is shared between corners).
	WVector n0 = ext0 * -1.0f;
	WVector n1 = ext1 * -1.0f;
	WVector n2 = ext2 * -1.0f;
	WVector corner[8];
	corner[0] = center + (n0 + (ext1 + n2));
	corner[1] = center + (n0 + (n1 + n2));
	corner[2] = center + (n1 + n2) + ext0;
	corner[3] = center + (ext1 + n2) + ext0;
	corner[4] = center + n0 + (ext1 + ext2);
	corner[5] = center + n0 + (n1 + ext2);
	corner[6] = center + (n1 + ext2) + ext0;
	corner[7] = center + (ext1 + ext2) + ext0;

	WVector *pCorner = corner;
	for (int count = 8; count != 0; count--, pCorner++) {
		if (!(box.max.x > pCorner->x)) box.max.x = pCorner->x;
		if (!(box.max.y > pCorner->y)) box.max.y = pCorner->y;
		if (!(box.max.z > pCorner->z)) box.max.z = pCorner->z;
		if (!(box.min.x < pCorner->x)) box.min.x = pCorner->x;
		if (!(box.min.y < pCorner->y)) box.min.y = pCorner->y;
		if (!(box.min.z < pCorner->z)) box.min.z = pCorner->z;
	}
	m_pModelData->m_aabb.max = box.max;
	m_pModelData->m_aabb.min = box.min;
}
