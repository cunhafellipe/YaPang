// Original: d:\build\custom\temp\code\client\projectg\grounditem.cpp
#include "grounditem.h"
#include "GroundItemMan.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "golfball.h"
#include "IContentsDoc.h"
#include "scenemanager.h"
#include "wpuppet.h"
#include "wresrcmng.h"
#include "wscene.h"
#include "wview.h"
#include "fx.h"
#include "fxsequence.h"
#include "SoundManager.h"
#include "commonutil.h"

// ?g_lightset@@3ULightSet@@A; its header (wscene.h) does not declare it yet.
extern LightSet g_lightset;

// grounditem.cpp:23 (run-time type and factory registration of GroundItem)
const WRTTI GroundItem::m_RTTI("GroundItem", &IObject::m_RTTI);

IObject* GroundItemMakeInstance(void) {
	return new GroundItem;
}

struct __sGroundItem {
	__sGroundItem(void) {
		ObjectFactory().AddObjectFunctor(GroundItemMakeInstance, "GroundItem");
	}
};
static __sGroundItem s_registerGroundItem;

// grounditem.cpp:26
GroundItem::BallInfo GroundItem::ms_ballInfo;

unsigned int GroundItem::ms_initCnt;
bool GroundItem::ms_greenItemJackpot;

// grounditem.cpp:31
const ThSphere &GroundItem::GetBallBoundingVolume(void) {
	return ms_ballInfo.m_sphere;
}

// grounditem.cpp:37
void GroundItem::EnableGreenItemJackpot(bool enable) {
	ms_greenItemJackpot = enable;
}

// grounditem.cpp:42
// Works on ms_ballInfo whatever the object (the original addresses the
// static directly).
const ThSphere &GroundItem::BallInfo::MakeBallBoundingVolume(int frame) {
	if (frame == ms_ballInfo.m_frame) {
		return ms_ballInfo.m_sphere;
	}

	WVector prevPos = GolfBall().PreData(frame - 1)->m_pos;
	WVector curPos = GolfBall().PreData(frame)->m_pos;

	WVector sum = prevPos + curPos; WVector dir = curPos - prevPos;

	ms_ballInfo.m_frame = frame;

	// The ball positions are WVectors; the sphere is built in gaV3 terms.
	float ballRadius = GolfBall().GetRadius();
	ms_ballInfo.m_sphere.Set(reinterpret_cast<const gaV3 &>(sum) * 0.5f, reinterpret_cast<const gaV3 &>(dir).Length() + ballRadius);

	return ms_ballInfo.m_sphere;
}

// grounditem.cpp:73
GroundItem::GroundItem(void) {
}

// grounditem.cpp:88
GroundItem::GroundItem(GroundItemMan *owner, unsigned long id, GIMMICK_ITEM_TYPE itemType, FieldItem::eDisposeTextureType disposeTextureType, const char *puppetName, const Ut::Spatial &spatial)
	: m_owner(owner), m_id(id), m_itemType(itemType), m_state(GIS_NONE), m_enabled(true), m_disposeTextureType(disposeTextureType), m_puppetName(puppetName), m_puppet(0), m_spatial(spatial) {
	WSingleton<CContentsDoc>::Instance()->GetContainer((localContentType_t)0xa3, m_gimmickContainer);

	m_endFrame = m_startFrame = m_idleFrame = -1;
}

// grounditem.cpp:101
GroundItem::~GroundItem(void) {
}

// grounditem.cpp:106
void GroundItem::Initialize(void) {
	m_puppet = g_resrcmng->GetPuppet(m_puppetName.c_str(), false, true, false);

	Ut::MakeObjectName(m_sceneElemName, "SceneElemName", *this, ms_initCnt++);

	CSceneManager::Instance()->RegisterElement((eObjType)4, m_puppet, 0, m_sceneElemName.c_str(), 0x18, 0);

	m_state = GIS_IDLE; m_spatial.ToMatrix(m_worldMat);
	Transform();
}

// grounditem.cpp:136
void GroundItem::Terminate(void) {
	CSceneManager::Instance()->DeleteElement(m_sceneElemName.c_str());
	if (g_resrcmng && m_puppet) { g_resrcmng->Release(m_puppet, false); m_puppet = 0; }
	m_puppet = 0;
}

// grounditem.cpp:143
void GroundItem::Transform(void) {
	m_puppet->UpdateLightSource(&g_lightset, false, 0);

	m_puppet->ApplyBones(0, m_worldMat);

	m_puppet->Transform(0, 1.0f, 0);

	m_puppet->UpdateBound(true, false);
}

// grounditem.cpp:157
void GroundItem::NSC_OnStep(bool active, bool replay, float elapsed, float syncTime, int frame) {
	if (frame == -1) {
		ms_ballInfo.Reset();
	}

	if (!NeedToUpdate()) {
		return;
	}

	eGoundItemUpdateType type = GIUT_NONE;

	switch (m_itemType) {
	case 0:
		type = m_disposeTextureType == 1 ? GIUT_DISPOSE_TEXTURE_1 : GIUT_ITEM_TYPE_0;
		break;

	case 1:
		type = m_disposeTextureType == 1 ? GIUT_DISPOSE_TEXTURE_1 : GIUT_ITEM_TYPE_1;
		break;
	}

	Update(type, active, replay, elapsed, syncTime, frame);
}

// grounditem.cpp:198
void GroundItem::NSC_OnFirstNewShotFrame(float startTime, int frame) {
	m_startFrame = m_endFrame = -1;

	if (m_state == GIS_ACTIVE) {
		ChangeState(GIS_IDLE, frame);
	}
}

// grounditem.cpp:211
void GroundItem::NSC_OnFirstReplayFrame(int frame) {
	if (m_state == GIS_ACTIVE) {
		if (frame <= m_startFrame) {
			ChangeState(GIS_IDLE, frame);
		}
	}

	if (m_state == GIS_REMOVED) {
		if (frame <= m_endFrame) {
			ChangeState(GIS_IDLE, frame);

			GroundItemMan::PlayerStats &stats = m_owner->m_playerStats[GOLFDOC()->m_myPlayerIndex];
			stats.m_shotCount[m_itemType]--;
		}
	}
}

// grounditem.cpp:241
void GroundItem::DrawBoundingVolume(void) {
	if (NeedToUpdate()) {
		WVector center;
		float radius;
		GetBoundingSphere(center, radius);
		g_view->DrawSphere(center, radius, 0xffff0000, 10, 0);

		g_view->DrawAABB(m_puppet->GetBoundBox(), 0xff00ff00, false, 0);
	}
}

// grounditem.cpp:261
void GroundItem::Enable(bool enable) {
	m_enabled = enable;

	CSceneManager::Instance()->SetVisible(m_sceneElemName.c_str(), NeedToUpdate());
}

// grounditem.cpp:274
void GroundItem::GetBoundingSphere(gaV3 &center, float &radius) const {
	const WSphere &sphere = m_puppet->GetBoundSphere();
	center = reinterpret_cast<const gaV3 &>(sphere.pos);
	radius = sphere.radius * 0.7f;
}

// grounditem.cpp:281
void GroundItem::GetBoundingSphere(WVector &center, float &radius) const {
	const WSphere &sphere = m_puppet->GetBoundSphere();
	center = sphere.pos;
	radius = sphere.radius * 0.7f;
}

// grounditem.cpp:288
GroundItem::eGroundItemState GroundItem::ChangeState(eGroundItemState state, int frame) {
	eGroundItemState prevState = m_state;
	m_state = state;
	OnChangeState(frame);
	return prevState;
}

// grounditem.cpp:299
void GroundItem::OnChangeState(int frame) {
	bool visible = false;

	switch (m_state) {
	case GIS_IDLE:
		visible = true;
		break;

	case GIS_ACTIVE:
		m_startFrame = frame;
		visible = true;
		break;

	case GIS_REMOVED: {
		m_endFrame = frame;
		visible = false;

		CFxSequence *seq = CFx::Instance()->OpenSequence(m_gimmickContainer->m_crashSeq[m_itemType].c_str(), true);

		if (seq) {
			seq->Pos() = m_worldMat.pivot;

			switch (m_itemType) {
			case GIMMICK_ITEM_PANG_COIN:
				seq->Pos().y += 4.0f;
				break;

			case GIMMICK_ITEM_RANDOM_BOX:
				seq->Pos().y += 7.0f;
				break;
			}
		}

		g_audio->PlaySfx(MakeStr("pang_coin_%d", rand() % 3 + 1), 0, 0, 0, 0, 0.5f, 200.0f);
		break;
	}
	}

	CSceneManager::Instance()->SetVisible(m_sceneElemName.c_str(), visible);
}

// grounditem.cpp:383
bool GroundItem::TestIntersection(int frame) {
	const ThSphere &ball = ms_ballInfo.MakeBallBoundingVolume(frame);

	ThSphere item;
	GetBoundingSphere(item.center, item.radius);

	float dx = ball.center[0] - item.center[0];
	float dy = ball.center[1] - item.center[1];
	float dz = ball.center[2] - item.center[2];
	float sumRadius = item.radius + ball.radius;

	if (dx * dx + dy * dy + dz * dz <= sumRadius * sumRadius) {
		return true;
	}

	return false;
}

// grounditem.cpp:410
bool GroundItem::TestIntersection2(int frame, float ballRadius) {
	ThSphere ball = ms_ballInfo.MakeBallBoundingVolume(frame);

	ThSphere item;
	GetBoundingSphere(item.center, item.radius);

	float dx = ball.center.x - item.center.x;
	float dy = ball.center.y - item.center.y;
	float dz = ball.center.z - item.center.z;
	float sumRadius = item.radius + ballRadius;

	if (dx * dx + dy * dy + dz * dz <= sumRadius * sumRadius) {
		return true;
	}

	return false;
}

// grounditem.cpp:441
void GroundItem::Update(eGoundItemUpdateType type, bool active, bool replay, float elapsed, float syncTime, int frame) {
	Ut::Spatial spatial;

	// A per-item phase, so neighbouring items do not move in step.
	float phase = m_spatial.m_position.x + m_spatial.m_position.y + m_spatial.m_position.z;

	switch (type) {
	case GIUT_ITEM_TYPE_0: {
		float t = phase + syncTime;

		gaQ rot(m_spatial.m_rotation);

		spatial.m_scale = m_spatial.m_scale + (gaMath::Sin(t) + 1.0f) * 7.0f;
		float angle = t * 5.0f;
		gaQ spin(gaV3(0.0f, 1.0f, 0.0f), angle);
		spatial.m_rotation = rot * spin;

		spatial.m_position = m_spatial.m_position;
		spatial.ToMatrix(m_worldMat);
		Transform();
		break;
	}

	case GIUT_ITEM_TYPE_1: {
		float t = phase + syncTime;
		spatial.m_scale = m_spatial.m_scale;
		gaQ rot(m_spatial.m_rotation);
		float angle = t * 5.0f;
		gaQ spin(gaV3(0.0f, -1.0f, 0.0f), angle);
		spatial.m_rotation = rot * spin;

		float height = (gaMath::Sin(syncTime * 3.0f + phase) + 1.0f) * 10.0f;
		spatial.m_position.x = m_spatial.m_position.x;
		spatial.m_position.y = m_spatial.m_position.y + height;
		spatial.m_position.z = m_spatial.m_position.z;
		spatial.ToMatrix(m_worldMat);
		Transform();
		break;
	}

	case GIUT_DISPOSE_TEXTURE_1:
		spatial.m_scale = m_spatial.m_scale;

		if (m_state == GIS_ACTIVE && active) {
			int startFrame = m_startFrame;
			int spinFrames = GolfBall().m_preDataCount - startFrame;
			if (spinFrames > 70) {
				spinFrames = 70;
			}

			int passedFrames = frame - startFrame;
			if (passedFrames > spinFrames) {
				passedFrames = spinFrames;
			}

			int fastFrames = (int)(spinFrames * 0.2f);

			if (frame < startFrame + fastFrames) {
				spatial.m_rotation = m_spatial.m_rotation;

				// The original calls GolfBall() here and drops the result.
				GolfBall();

				gaQ rot(m_spatial.m_rotation);
				gaQ spin(gaV3(0.0f, -1.0f, 0.0f), syncTime * 20.0f);
				rot *= spin;
				spatial.m_rotation = rot;

				spatial.m_position.x = m_spatial.m_position.x;
				spatial.m_position.y = m_spatial.m_position.y;
				spatial.m_position.z = m_spatial.m_position.z;

				spatial.ToMatrix(m_worldMat);
				Transform();
			} else {
				gaQ rot(m_spatial.m_rotation);
				gaQ spin(gaV3(0.0f, -1.0f, 0.0f), syncTime * 30.0f);
				rot *= spin;
				spatial.m_rotation = rot;

				// Pulled toward the ball as the spin slows down.
				float ratio = gaMath::Pow(1.0f - (float)(passedFrames - fastFrames) / (spinFrames - fastFrames), 5.0f);
				float ballRatio = 1.0f - ratio;
				spatial.m_position = m_spatial.m_position * ratio + reinterpret_cast<const gaV3 &>(GolfBall().PreData(frame)->m_pos) * ballRatio;
				spatial.ToMatrix(m_worldMat);
				Transform();
			}
		} else {
			gaQ rot(m_spatial.m_rotation);
			float angle = syncTime * 5.0f;
			gaQ spin(gaV3(0.0f, 1.0f, 0.0f), angle);
			rot *= spin;
			spatial.m_rotation = rot;

			spatial.m_position.x = m_spatial.m_position.x;
			spatial.m_position.y = m_spatial.m_position.y;
			spatial.m_position.z = m_spatial.m_position.z;
			spatial.ToMatrix(m_worldMat);
			Transform();
		}
		break;
	}

	if (frame > 0 && active) {
		if (type == GIUT_DISPOSE_TEXTURE_1) {
			if (m_state != GIS_ACTIVE) {
				float radius = 55.0f;
				if (ms_greenItemJackpot) {
					radius = 140.0f;
				}

				if (TestIntersection2(frame, radius)) {
					ChangeState(GIS_ACTIVE, frame);
				}
			}
		}

		if (TestIntersection(frame)) {
			GroundItemMan::PlayerStats &stats = m_owner->m_playerStats[GOLFDOC()->m_myPlayerIndex];
			stats.m_shotCount[m_itemType]++;

			if (!replay) {
				stats.m_totalCount[m_itemType]++;

				FieldItem::AcquiredItemInfo info;
				info.itemType = m_itemType;
				info.id = m_id;
				info.count = 1;
				info.disposeTextureType = m_disposeTextureType;
				m_gimmickContainer->AddItemAcquired(info);
			}

			m_state = GIS_REMOVED; OnChangeState(frame);
		}
	}

	unsigned char alpha = m_puppet->GetAlpha(0);

	if (!active) {
		ThSphere ball;
		ball.radius = 20.0f;
		const WVector &ballPos = GolfBall().m_pos;
		ball.SetPos(ballPos.x, ballPos.y, ballPos.z);

		ThSphere item;
		GetBoundingSphere(item.center, item.radius);

		if (ball.TestIntersection(item)) {
			// Fades out while the ball is close.
			if (alpha > 100) {
				m_puppet->SetAlpha(alpha - 1);
			}
			return;
		}
	}

	if (alpha < 255) {
		m_puppet->SetAlpha(alpha + 1);
	}
}
