// Original: d:\build\custom\temp\code\client\projectg\shotpreview.cpp
#include <atlbase.h>	// stand-in for the precompiled header: _AtlWinModule initializer
			// (keeps BAR_LENGTH dynamically initialized, docs/tough_problems/010)
#include "shotpreview.h"

#include <string.h>

#include "ObjectFactory.h"
#include "actor.h"
#include "scenemanager.h"
#include "3dfont.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "club.h"
#include "client/Wangreal/include/wresrcmng.h"
#include "client/Wangreal/include/wpuppet.h"
#include "client/Wangreal/include/wview.h"
#include "client/Fresh/include/frwnd.h"
#include "client/Fresh/include/frbutton.h"
#include "golfball.h"
#include "quadtree.h"
#include "fx.h"
#include "fxsequence.h"
#include "SoundManager.h"
#include "wind.h"
#include "TaskManager.h"
#include "commonutil.h"
#include "InputManager.h"
#include "shared/globalgamedefine.h"
#include "shared/Localize.h"
#include "cardsystem.h"

#include <algorithm>
#include <functional>

extern LightSet g_lightset;

// wmath.inl (not declared by wmath.h yet)
WMatrix operator*(const WMatrix& m, float f);

// A pang coin shown along the predicted flight: a "coin.pet" puppet
// registered in the scene manager under its own element name.
class ShotPreview::PangCoin {
	LAYOUT_CHECK_FRIEND;
public:
	PangCoin(const WVector& pos, float size, bool visible);
	~PangCoin(void);

	void SetVisible(bool visible);
	void Restore(void);
	void Update(float delta);
	void Render(void);

private:
	static int _PetReferenceCounter;   // numbers the scene element names

	WPuppet* m_pPuppet;       // 0x00, "coin.pet", loaded by Restore
	std::string m_name;       // 0x04, the scene element name
	WMatrix m_matrix;         // 0x20, the puppet's placement (turned by Render)
	float m_angle;            // 0x50, turn angle around Y
	float m_size;             // 0x54, scale of the puppet
	bool m_visible;           // 0x58
	float m_radius;           // 0x5C, pickup radius (size times the bounding sphere's)
	WVector m_pos;            // 0x60, where the ball picks it up
};

CHECK_SIZE(ShotPreview::PangCoin, 0x6C);
BEGIN_LAYOUT_CHECK(ShotPreview::PangCoin)
	CHECK_FIELD(ShotPreview::PangCoin, m_name, 0x04);
	CHECK_FIELD(ShotPreview::PangCoin, m_matrix, 0x20);
	CHECK_FIELD(ShotPreview::PangCoin, m_angle, 0x50);
	CHECK_FIELD(ShotPreview::PangCoin, m_size, 0x54);
	CHECK_FIELD(ShotPreview::PangCoin, m_visible, 0x58);
	CHECK_FIELD(ShotPreview::PangCoin, m_radius, 0x5C);
	CHECK_FIELD(ShotPreview::PangCoin, m_pos, 0x60);
END_LAYOUT_CHECK

// The shot parameters a prediction was made with (compared byte-wise to tell
// when the prediction must be redone).
class ShotPreview::UpdateSensor {
public:
	UpdateSensor(void);

	void Update(void);
	bool Equal(const UpdateSensor& other) const;
	bool operator==(const UpdateSensor& other) const;

private:
	int m_playerShot;               // 0x00, filled by the "Player" actor (message 0x110)
	int m_spin[2];                  // 0x04, filled by the "Spin" actor (message 0x161)
	unsigned char m_clubIndex;      // 0x0C, the current club's index (aligned padding to 0x10, no char array: a
	                                // char gap would give OnProcess's local sensor a /GS cookie)
	WVector m_wind;                 // 0x10, the global wind
	int m_powerShotType;            // 0x1C, the current club's power shot
	unsigned long m_shotItemTypeId; // 0x20, my player's shot item
	float m_impact;                 // 0x24, the shot bar's impact
};

CHECK_SIZE(ShotPreview::UpdateSensor, 0x28);

int ShotPreview::PangCoin::_PetReferenceCounter = 0;

// shotpreview.cpp:23
ShotPreview::PangCoin::PangCoin(const WVector& pos, float size, bool visible) {
	m_visible = true;
	m_size = size;
	m_angle = (rand() % 314) * 0.01f;
	m_pPuppet = g_resrcmng->GetPuppet("coin.pet", false, true, false);
	m_name = MakeStr("\xc6\xce\xc4\xda\xc0\xce_%d", ++_PetReferenceCounter);
	m_matrix.Reset();

	CSceneManager::Instance()->RegisterElement((eObjType)4, m_pPuppet, 0, m_name.c_str(), 8, 0);

	m_matrix.pivot = pos;
	m_pos = pos;
	m_pPuppet->CalcBound(0, true);
	m_radius = size * m_pPuppet->GetBoundSphere().radius;

	if (visible) {
		m_matrix.pivot.y -= m_radius;
	} else {
		m_pos.y += m_radius;
	}
}

// shotpreview.cpp:54
ShotPreview::PangCoin::~PangCoin(void) {
	--_PetReferenceCounter;

	if (m_pPuppet) {
		CSceneManager::Instance()->DeleteElement(m_name.c_str());
		if (g_resrcmng && m_pPuppet) { g_resrcmng->Release(m_pPuppet, false); m_pPuppet = 0; }
	}
}

// shotpreview.cpp:64
void ShotPreview::PangCoin::Update(float delta) {
	m_angle += delta * g_PI;

	if (m_pPuppet && m_visible) {
		if (GOLFDOC()->GameMode() & 0x80) {
			WVector d = GolfBall().m_pos - m_pos;
			if (d.Magnitude() <= m_radius) {
				CFx::Instance()->OpenSequence("coin_pang.seq", true)->Pos() = m_pos;
				CSceneManager::Instance()->DeleteElement(m_name.c_str());
				if (g_resrcmng && m_pPuppet) { g_resrcmng->Release(m_pPuppet, false); m_pPuppet = 0; }
				g_audio->PlaySfx(MakeStr("pang_coin_%d", rand() % 3 + 1), 0, 0, 0, 0, 0.5f, 200.0f);
			}
		}
	}
}

// shotpreview.cpp:82
void ShotPreview::PangCoin::SetVisible(bool visible) {
	m_visible = visible;
	CSceneManager::Instance()->SetVisible(m_name.c_str(), visible);
}

// shotpreview.cpp:88
void ShotPreview::PangCoin::Restore(void) {
	if (m_pPuppet == 0) {
		m_pPuppet = g_resrcmng->GetPuppet("coin.pet", false, true, false);
		CSceneManager::Instance()->RegisterElement((eObjType)4, m_pPuppet, 0, m_name.c_str(), 8, 0);
	}
}

// shotpreview.cpp:101
void ShotPreview::PangCoin::Render(void) {
	if (m_pPuppet && m_visible) {
		WVector pivot = m_matrix.pivot;
		m_matrix = RotMat(WVector(0.0f, 1.0f, 0.0f), m_angle);
		m_matrix.pivot = pivot;

		m_pPuppet->UpdateLightSource(&g_lightset, false, 0);
		m_pPuppet->ApplyBones(0, m_matrix * m_size);
		m_pPuppet->UpdateBSphere();

		m_pPuppet->Transform(0, 1.0f, 0);
	}
}

// shotpreview.cpp:138
ShotPreview::UpdateSensor::UpdateSensor(void) {
	memset(this, 0, sizeof(UpdateSensor));
}

// shotpreview.cpp:144
void ShotPreview::UpdateSensor::Update(void) {
	AfxGetTask()->GetActor("Player") << MsgObject(0, 0x110, (intptr_t)this, 0, 0, 0, 0);
	AfxGetTask()->GetActor("Spin") << MsgObject(0, 0x161, (intptr_t)&m_spin[1], (intptr_t)&m_spin[0], 0, 0, 0);

	m_clubIndex = GolfClub().GetIndex();
	m_wind = Wind().GetGlobalWind();
	m_powerShotType = GolfClub().PowerShotType();
	m_shotItemTypeId = PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId;
	m_impact = GOLFDOC()->GetBar().m_impact;
}

// shotpreview.cpp:156
bool ShotPreview::UpdateSensor::Equal(const UpdateSensor& other) const {
	int result = memcmp(this, &other, sizeof(UpdateSensor));
	return result == 0;
}

// shotpreview.cpp:161
bool ShotPreview::UpdateSensor::operator==(const UpdateSensor& other) const {
	int result = memcmp(this, &other, sizeof(UpdateSensor));
	return result == 0;
}

// shotpreview.cpp:177 (run-time type and factory registration of ShotPreview)
const WRTTI ShotPreview::m_RTTI("ShotPreview", &IActor::m_RTTI);

IObject* ShotPreviewMakeInstance(void) {
	return new ShotPreview;
}

struct __impShotPreview {
	__impShotPreview(void) {
		ObjectFactory().AddObjectFunctor(ShotPreviewMakeInstance, "ShotPreview");
		ActorFactory().AddActorClass("ShotPreview");
	}
};
static __impShotPreview s_registerShotPreview;

// shotpreview.cpp:181
ShotPreview::ShotPreview(void) : m_predictType((PredictType::Enum)0), m_visible(1) {
	SetOptionFlag(10);
	m_updateElapsed = 0.0f;
	m_pBall = new CGolfBall;
	m_pSensor = new UpdateSensor;
	m_indicatorTexture = g_resrcmng->LoadTexture("{indicator.bmp", 0, 0, 0);

	m_priority = 2;
	CSceneManager::Instance()->RegisterElement((eObjType)6, 0, this, "ShotPreview", 0, 0);
	m_pFont = new C3dFont;
	m_pFont->LoadTexture("[font_bar.jpg", 16, 16, "1234567890ym%-.:", 6.0f, -25.0f);
}

// shotpreview.cpp:200
std::string ShotPreview::TwAddOption(const tweaker_param& param) {
	unsigned int flag = GetOptionFlag(); SetOptionFlag(flag | param[0].to_int());
	return std::string("TwAddOption Processed");
}

// shotpreview.cpp:207
std::string ShotPreview::TwSubOption(const tweaker_param& param) {
	unsigned int flag = GetOptionFlag(); SetOptionFlag(flag & ~param[0].to_int());
	return std::string("TwSubOption Processed");
}

// shotpreview.cpp:214
ShotPreview::~ShotPreview(void) {
	std::for_each(m_pangCoins.begin(), m_pangCoins.end(), Delete_Object());
	PG_VECTOR_FREE_STORAGE(m_pangCoins);

	if (m_pFont) { delete m_pFont; m_pFont = 0; }
	if (m_pBall) { delete m_pBall; m_pBall = 0; }
	if (m_pSensor) { delete m_pSensor; m_pSensor = 0; }
	if (g_resrcmng && m_indicatorTexture) { g_resrcmng->Release(m_indicatorTexture); m_indicatorTexture = 0; }
}

// shotpreview.cpp:225
bool ShotPreview::IsSkipState(void) {
	if (!(GOLFDOC()->GameMode() & 0x3F8))
		return true;

	if (GolfClub().GetType() == 3)
		return true;

	return m_visible == 0;
}

// shotpreview.cpp:238
void ShotPreview::OnInit(void) {
	PG_VECTOR_FREE_STORAGE(m_path);
}

// shotpreview.cpp:243
void ShotPreview::OnLoad(void) {
	PG_VECTOR_FREE_STORAGE(m_predictButtons);

	FrButton* buttons[4] = { 0, };
	SendMsg(this, "GolfRule", 0x24A, (intptr_t)"predict_normal", (intptr_t)&buttons[0], 0, 0);
	SendMsg(this, "GolfRule", 0x24A, (intptr_t)"predict_tomahwak", (intptr_t)&buttons[1], 0, 0);
	SendMsg(this, "GolfRule", 0x24A, (intptr_t)"predict_cobra", (intptr_t)&buttons[2], 0, 0);
	SendMsg(this, "GolfRule", 0x24A, (intptr_t)"predict_spike", (intptr_t)&buttons[3], 0, 0);

	for (int i = 0; i < 4; ++i) {
		if (buttons[i]) {
			FrButton* pButton = DYNAMIC_CAST(FrButton, buttons[i]);
			if (pButton) {
				m_predictButtons.push_back(pButton);
			}
		}
	}
}

// shotpreview.cpp:263
void ShotPreview::OnReload(void) {
	OnLoad();
}

// The original clamps low first, then high (its header is not known yet).
template <typename T>
inline T ClampOf(T value, T low, T high) {
	return value < low ? low : (value > high ? high : value);
}

// shotpreview.cpp:268
void ShotPreview::OnProcess(float delta) {
	m_updateElapsed += delta;

	// Debug keys: add to the combo gauge.
	if (g_input->Get("LSHIFT", true) && g_input->GetDown("NUM_9", true)) {

		PLAYER(GOLFDOC()->m_myPlayerIndex)->m_comboGauge = ClampOf(PLAYER(GOLFDOC()->m_myPlayerIndex)->m_comboGauge + 33.0f, 0.0f, WSingleton<CSharedDoc>::Instance()->GetComboGaugeLimit(GOLFDOC()->m_myPlayerIndex));
	}

	if (g_input->Get("LSHIFT", true) && g_input->GetDown("REPLAY", true)) {

		switch (GetOptionFlag()) {
		case 10: SetOptionFlag(11); break;
		case 11: SetOptionFlag(0); break;
		case 0: SetOptionFlag(10); break;
		}

		std::for_each(m_pangCoins.begin(), m_pangCoins.end(), std::bind2nd(std::mem_fun(&PangCoin::SetVisible), ((GetOptionFlag() >> 4) & 1) != 0));
	}

	std::for_each(m_predictButtons.begin(), m_predictButtons.end(),
		std::bind2nd(std::mem_fun(&FrWnd::SetVisible), GetOptionFlag()));

	if (m_updateElapsed > 0.5f) {

		m_updateElapsed = 0.0f;

		if (!IsSkipState()) {

			UpdateSensor sensor;
			sensor.Update();
			if (!(*m_pSensor == sensor)) {

				m_path.clear();
				m_path2.clear();

				int count = -1;
				Predict(m_path, m_path2, &count);

				std::for_each(m_pangCoins.begin(), m_pangCoins.end(), Delete_Object());
				m_pangCoins.clear();

				if (count != -1 && !m_path.empty() && (GetOptionFlag() & 0x10)) {

					std::vector<WVector> coins;

					coins.assign(m_path.begin(), m_path.begin() + count);

					std::vector<WVector>::iterator it;
					while (coins.size() > 20) {

						int n = 0;
						for (it = coins.begin(); it != coins.end() - 1; ) {
							if (++n & 1) {
								it = coins.erase(it);
							} else {
								++it;
							}
						}
					}

					for (it = coins.begin(); it != coins.end() - 1; ++it)
						m_pangCoins.push_back(new PangCoin(*it, 3.0f, true));

					m_pangCoins.push_back(new PangCoin(*it, 8.0f, false));

					std::for_each(m_pangCoins.begin(), m_pangCoins.end(), std::bind2nd(std::mem_fun(&PangCoin::SetVisible), ((GetOptionFlag() >> 4) & 1) != 0));
				}

				m_pSensor->Update();
			}
		}
	}

	std::for_each(m_pangCoins.begin(), m_pangCoins.end(), std::bind2nd(std::mem_fun(&PangCoin::Update), delta));
}

namespace {

// shotpreview.cpp:368
unsigned long GetGuideLineColor(const WVector& pos) {
	WVector ballPos = pos;
	WVector cupPos = GOLFDOC()->GetHoleData().m_cupPos;

	WVector d;
	d.x = cupPos.x - ballPos.x;
	d.z = cupPos.z - ballPos.z;

	return sqrtf(d.x * d.x + d.z * d.z) < GetPVS().GetHoleCupRadius() ? 0xA80000FF : 0xA8FFFF00;
}

// shotpreview.cpp:380
WVector GetProjectionVector(const WVector& v) {
	WVector pos = v;
	WTVertex vtx;
	vtx.x = pos.x;
	vtx.y = pos.y;
	vtx.z = pos.z;

	g_view->Projection2(&vtx, pos);

	pos.x = vtx.x;
	pos.y = vtx.y;
	pos.z = vtx.z;

	return pos;
}

}

// shotpreview.cpp:403
void ShotPreview::DrawIndicator(const WVector& pos, unsigned long color, float size) {
	size *= 20.0f;

	WVector ground = GetPVS().GetGroundPoint(pos, false, 0, false);

	WTVertex* vl[5] = { 0, };
	WTVertex screen;
	WTVertex vtx[4];

	g_view->Projection2(&screen, ground);
	if (screen.z <= 0.0f || screen.z >= 1.0f) {
		return;
	}

	vl[0] = &vtx[0];
	vtx[0].x = screen.x - size * 0.5f;
	vtx[0].z = screen.z;
	vtx[0].y = screen.y;
	vtx[0].rhw = 1.0f;
	vtx[0].diffuse = color;
	vl[1] = &vtx[1];
	vtx[1].x = screen.x - size * 0.5f;
	vtx[1].z = screen.z;
	vtx[1].y = screen.y - size;
	vtx[1].rhw = 1.0f;
	vtx[1].diffuse = color;
	vl[2] = &vtx[2];
	vtx[2].x = size * 0.5f + screen.x;
	vtx[2].z = screen.z;
	vtx[2].y = screen.y - size;
	vtx[2].rhw = 1.0f;
	vtx[2].diffuse = color;
	vl[3] = &vtx[3];
	vtx[3].x = size * 0.5f + screen.x;
	vtx[3].z = screen.z;
	vtx[3].y = screen.y;
	vtx[3].rhw = 1.0f;
	vtx[3].diffuse = color;

	vtx[0].tu = 0.0f;
	vtx[0].tv = 1.0f;
	vtx[1].tu = 0.0f;
	vtx[1].tv = 0.0f;
	vtx[2].tu = 1.0f;
	vtx[2].tv = 0.0f;
	vtx[3].tu = 1.0f;
	vtx[3].tv = 1.0f;

	if (g_view->GetWidth() - 0.5f < vtx[0].x || vtx[3].x < 0.5f || vtx[0].y < 0.5f || g_view->GetHeight() - 0.5f < vtx[1].y) {
		return;
	}

	int flags = (m_indicatorTexture & 0x7FF) | 0x2400000;
	if (GOLFDOC()->GameMode() & 0x200) {
		flags |= 0x300000;
		m_pFont->SetProjectionStyle((C3dFont::ProjectionStyle::Enum)1);
	} else {
		m_pFont->SetProjectionStyle((C3dFont::ProjectionStyle::Enum)0);
	}

	g_view->DrawPolygonFan(vl, flags, 0, true);

	if (GetOptionFlag() & 8) {
		WVector ballPos = pos;
		WVector cupPos = GetHoleCupPos();

		WVector d(ballPos.x - cupPos.x, 0.0f, ballPos.z - cupPos.z);

		m_pFont->SetPos(ground);
		if (color == 0xA80000FF) {
			m_pFont->SetColor(0xFF80FFFF);
			m_pFont->Printf(g_view, -7, "0y");
		} else {
			m_pFont->SetColor(-1);
			float dist = d.Magnitude();
			m_pFont->Printf(g_view, -7, "%.1fy", dist * 0.3125f);
		}
	}
}

// shotpreview.cpp:496
void ShotPreview::Display(void) {
	if (IsSkipState() || m_path.size() == 0) {
		return;
	}

	std::for_each(m_pangCoins.begin(), m_pangCoins.end(), std::mem_fun(&PangCoin::Render));

	int count = m_path.size();

	unsigned long color = GetGuideLineColor(m_path.back());

	if (m_path2.size()) {
		if (m_path2.size() > 2) {
			m_path2.erase(m_path2.begin() + 1, m_path2.end() - 1);

			if (GetOptionFlag() & 4) {
				DrawIndicator(m_path2[1], color, 0.8f);
			}
			if (GetOptionFlag() & 2) {
				DrawIndicator(m_path2[0], color, 0.8f);
			}
		} else if (GetOptionFlag() & 6) {
			DrawIndicator(m_path2[0], color, 0.8f);
		}
	}

	if (!(GetOptionFlag() & 1)) {
		return;
	}

	if (GOLFDOC()->GameMode() & 0x200) {
		for (int i = 0; i < count - 1; ++i) {
			WVector a = GetPVS().GetGroundPoint(m_path[i], false, 0, false);
			WVector b = GetPVS().GetGroundPoint(m_path[i + 1], false, 0, false);

			g_view->DrawLine2D(a, b, 0xA8FFFF00, 1.0f);
		}
	} else {
		WTVertex vtx[4];
		WTVertex* vl[5] = { &vtx[0], &vtx[1], &vtx[2], &vtx[3], 0 };
		vtx[0].diffuse = color;
		vtx[1].diffuse = color;
		vtx[2].diffuse = color;
		vtx[3].diffuse = color;

		for (int i = 1; i < count; ++i) {
			WVector& prev = m_path[i - 1];
			WVector& cur = m_path[i];

			WVector side;
			side = WCrossProduct(cur - prev, WVector(0.0f, 1.0f, 0.0f));
			side.Normalize();
			side *= GolfBall().m_radius;

			WVector a;
			a = prev - side;
			WVector b;
			b = cur - side;
			WVector c;
			c = cur + side;
			WVector d;
			d = prev + side;

			vtx[0].x = a.x;
			vtx[0].y = a.y;
			vtx[0].z = a.z;
			vtx[1].x = b.x;
			vtx[1].y = b.y;
			vtx[1].z = b.z;
			vtx[2].x = c.x;
			vtx[2].y = c.y;
			vtx[2].z = c.z;
			vtx[3].x = d.x;
			vtx[3].y = d.y;
			vtx[3].z = d.z;

			g_view->DrawPolygonFan(vl, 0xC00000, 0x400, false);
		}
	}
}

// shotpreview.cpp:631
void ShotPreview::HandleMsg(const MsgObject& msg) {
	if (msg.m_msg == -1) {
		std::string name((const char*)msg.m_param1);
		if (name == "GET_PREDICTED_FIRSTBOUND_POS") {
			if (m_path2.size()) {
				*(int*)msg.m_param2 = 1;
				*(WVector*)msg.m_param3 = m_path2[0];
			} else {
				*(int*)msg.m_param2 = 0;
			}
		}

		if (name == "GET_PREDICT_POSITIONS") {
			std::vector<WVector>* positions = (std::vector<WVector>*)msg.m_param2;
			positions->assign(m_path.begin(), m_path.end());
		} else if (name == "CHANGE_PREDICT_TYPE") {
			PredictType::Enum type = (PredictType::Enum)msg.m_param2;
			if (type != m_predictType) {
				m_predictType = type;
				m_path.clear();
				m_path2.clear();
				Predict(m_path, m_path2, 0);
				m_pSensor->Update();
				m_updateElapsed = 0.0f;
			}
		} else if (name == "ENABLE_DISPLAY") {
			m_visible = msg.m_param2;
		} else if (name == "PM_REPLAY") {
			std::for_each(m_pangCoins.begin(), m_pangCoins.end(), std::mem_fun(&PangCoin::Restore));
		}
	}
}

// shotpreview.cpp:694
unsigned short ShotPreview::GetBallInitSpecialValueFromPredictType(PredictType::Enum type) {
	if (type == 1) return 0x10;
	if (type == 2) return 0x20;
	return type == 3 ? 0x40 : 0;
}

// The original's max compiles as `a < b ? b : a` (its header is not known yet).
template <typename T>
inline T MaxOf(T a, T b) {
	return a < b ? b : a;
}

// shotpreview.cpp:702
// Runs the real ball through the physics (backed up in m_pBall and restored
// at the end) and collects its flight points and its bounce points.
bool ShotPreview::Predict(std::vector<WVector>& path, std::vector<WVector>& bounces, int* count) {
	float rate = (GOLFDOC()->GetBar().m_impact - BAR_START) / BAR_LENGTH;

	if (WAbsBySign(rate) < g_EPSILON)
		rate = 1.0f;

	float rateSqrt = sqrtf(rate);
	float power = GolfClub().GetPower() * rateSqrt;

	// The original calls GolfClub() here and discards the result.
	CClub& club = GolfClub();

	m_pBall->Reset(WVector());
	*m_pBall = GolfBall();

	CGolfBall& ball = GolfBall();
	ball.Reset(ball.m_pos);

	float spot;
	SendMsg(this, "Spot", 0xFA, (intptr_t)&spot, 0, 0, 0);
	float angle;
	SendMsg(this, "Player", 0x110, (intptr_t)&angle, 0, 0, 0);
	angle += spot;

	CGolfBall::sExpected* first = ball.PreDataToWrite(0);
	first->m_pos = ball.m_pos;
	first->m_groundType = 0;
	first->m_contactType = 1;

	power *= sqrtf(GetPVS().GetMaxPowerOnGround());

	// shotpreview.cpp:745
	ball.m_powerFlags = GetBallInitSpecialValueFromPredictType(m_predictType);
	// shotpreview.cpp:747
	if (ball.m_powerFlags == 0x40 || ball.m_powerFlags == 0x10)
		power *= 1.3f;

	SendMsg(this, "Spin", 0x161, (intptr_t)&ball.m_lieSlope, (intptr_t)&ball.m_lieAngle, 0, 0);

	float spinAngle = -(ball.m_lieSlope * 0.34906584f);

	if (PLAYER(GOLFDOC()->m_myPlayerIndex)->m_shotItemTypeId != 0x18000028) {
		// The base's up axis (CBase::HandleMsg 0x1C1 writes a whole WVector).
		WVector screen;
		SendMsg(this, "Screen", 0x1C1, (intptr_t)&screen, 0, 0, 0);

		if (ball.m_powerFlags & 0x40)
			ball.m_lieSlope -= screen.x * 0.15f;
		else
			ball.m_lieSlope -= screen.x * 0.5f;
	}

	WVector velocity;
	velocity = ((Abs(ball.m_lieSlope) * 0.1f + 1.0f) * power) * RotMat(GolfClub().GetLie(0), 0).za * RotMat(spinAngle + angle, 1);
	first->m_velocity = velocity;

	float frameOffset;
	SendMsg(this, "Player", 0x126, (intptr_t)&frameOffset, 0, 0, 0);
	unsigned long frame = GetPVS().GetFrame();
	GetPVS().SetFrame(frame - (int)(frameOffset * -50.0f) + 50);

	ball.m_velocity = first->m_velocity;
	ball.m_state = first->m_contactType;
	ball.m_flags = ball.m_powerFlags;
	ball.m_spin = 0.0f;
	ball.m_eventPreDataIndex[10] = -1;
	GetPVS().ResetVariable();

	unsigned char groundType = ball.m_groundType;

	if (GolfClub().GetApproachMode() == 4) {
		float aux = WSingleton<CSharedDoc>::Instance()->GetAuxPartProperty(GOLFDOC()->m_myPlayerIndex, 0) * 0.5f;
		float range;
		if (IsLocalContent((localContentType_t)0x70)) {
			range = (float)WSingleton<CSharedDoc>::Instance()->GetBuffValue(GOLFDOC()->m_myPlayerIndex, 0x10);
			if (GolfClub().PowerShotType())
				range += (float)WSingleton<CSharedDoc>::Instance()->GetBuffValue(GOLFDOC()->m_myPlayerIndex, 0x11);
		} else {
			// shotpreview.cpp:800
			WSingleton<CCardManager>::Instance()->SetPlayerIndex(GOLFDOC()->m_myPlayerIndex);
			range = WSingleton<CCardManager>::Instance()->GetCardRangeUp();
			if (GolfClub().PowerShotType())
				range += WSingleton<CCardManager>::Instance()->GetCardPowerRangeUp();
		}

		// shotpreview.cpp:810
		aux += range;
		float bonus = ((int)PLAYER(GOLFDOC()->m_myPlayerIndex)->m_power + aux) * (1.0f / 170.0f);
		bonus = MaxOf(0.0f, bonus);
		ball.m_spin = (bonus + 1.5f) * rate * rate;
	}

	// shotpreview.cpp:817
	ball.m_curve = rate * ball.m_lieSlope;
	while (ball.m_state) {
		CGolfBall::sExpected* pre = ball.PreDataToWrite(++ball.m_preDataCount);

		pre->m_sound[0] = 0;
		pre->m_groundType = 0;
		pre->m_groundSubType = 0;
		GetPVS().PreProcess(0.02f, true);
		GetPVS().BallProcess(0.02f);

		if (GolfClub().GetApproachMode() == 4) {
			if (ball.m_flags & 0x20) {
				if (ball.m_eventPreDataIndex[9] < 0) {
					ball.m_pos.y = MaxOf(ball.m_pos.y, GetPVS().GetBasePosY(ball.m_pos, ball.m_radius * 2.0f));
				} else {
					ball.m_spin -= 0.02f;
				}
			} else if (ball.m_flags & 0x40) {
				if (ball.m_eventPreDataIndex[12] < 0) {
					ball.m_pos.y = MaxOf(ball.m_pos.y, GetPVS().GetBasePosY(ball.m_pos, ball.m_radius * 2.0f));
				} else {
					ball.m_spin -= 0.02f;
				}
			} else {
				if (ball.m_eventPreDataIndex[0] < 0 && GetPVS().GetObjColNum() == 0) {
					ball.m_spin -= (0.5f - ball.m_lieAngle * 0.1f) * 0.02f;
				} else {
					ball.m_spin -= 0.002f;
				}
			}
		}

		// shotpreview.cpp:857
		if (ball.m_preDataCount >= 3000
			|| (ball.m_preDataCount >= 1500
				&& (WVectorLen(ball.m_velocity) < 0.1f
					|| GetPVS().GetObjColNum() >= 10
					|| GetPVS().GetMeshIndex(ball.m_pos, false) == -1))) {
			ball.m_velocity.Reset();
			ball.m_pos = GetPVS().GetGroundPoint(ball.m_pos, false, 0, 0);
			ball.m_pos.y += ball.m_radius;
			ball.m_state = 0;
		}

		// shotpreview.cpp:865
		pre->m_pos = ball.m_pos;
		pre->m_velocity = ball.m_velocity;
		pre->m_rotation.x = ball.m_rotation.x;
		pre->m_rotation.y = ball.m_rotation.y;
		pre->m_contactType = ball.m_state;

		// shotpreview.cpp:870
		if (ball.m_state == 0)
			ball.m_flags &= ~0x70;

		if (ball.m_state == 1 && ball.m_velocity.y < 0.0f && ball.m_eventPreDataIndex[1] < 0)
			ball.m_eventPreDataIndex[1] = ball.m_preDataCount;

		if (ball.m_eventPreDataIndex[10] < 0 && (ball.m_powerFlags & 0x10) && !(ball.m_flags & 0x10))
			ball.m_eventPreDataIndex[10] = ball.m_preDataCount;

		// shotpreview.cpp:879
		if (ball.m_flags & 0x20) {
			if (ball.m_preDataCount == 1) {
				ball.m_velocity.y = 0.0f;
				ball.m_velocity = power * ball.m_velocity.Normalize();
			}

			// shotpreview.cpp:887
			const CGolfBall::sExpected* start = ball.PreData(0);
			float dx = ball.m_pos.x - start->m_pos.x;
			float dz = ball.m_pos.z - start->m_pos.z;
			if (ball.m_eventPreDataIndex[9] < 0) {
				float dist = sqrtf(dz * dz + dx * dx);
				if (dist > (GolfClub().GetRange(0) * rateSqrt - 100.0f) * 3.2f) {
					ball.m_eventPreDataIndex[9] = ball.m_preDataCount;
					ball.m_velocity.Normalize();

					switch (GolfClub().GetID()) {
					case 0:
						ball.m_velocity *= 74.0f;
						break;
					case 1:
						ball.m_velocity *= 76.0f;
						break;
					case 2:
						ball.m_velocity *= 80.0f;
						break;
					}

					// shotpreview.cpp:909
					ball.m_velocity *= rateSqrt;
					ball.m_spin = 2.5f;
				}
			}
		}

		// shotpreview.cpp:914
		if (ball.m_eventPreDataIndex[14] < 0 && (ball.m_powerFlags & 0x40) && !(ball.m_flags & 0x40) && ball.m_eventPreDataIndex[13] > 0)
			ball.m_eventPreDataIndex[14] = ball.m_preDataCount;

		// shotpreview.cpp:917
		if (ball.m_flags & 0x40) {
			if (ball.m_preDataCount == 1) {
				ball.m_velocity.y = 0.0f;
				ball.m_velocity = power * ball.m_velocity.Normalize();
			}

			// shotpreview.cpp:925
			const CGolfBall::sExpected* start = ball.PreData(0);
			float dx = ball.m_pos.x - start->m_pos.x;
			float dz = ball.m_pos.z - start->m_pos.z;
			if (ball.m_eventPreDataIndex[12] < 0) {
				if (sqrtf(dz * dz + dx * dx) > 0.0f) {
					ball.m_eventPreDataIndex[12] = ball.m_preDataCount;
					ball.m_velocity.Normalize();
					ball.m_velocity *= 72.5f;
					ball.m_velocity *= rateSqrt * 2.0f;
					ball.m_spin = 3.1f;
				}
			}

			// shotpreview.cpp:959
			if (ball.m_eventPreDataIndex[1] >= 0 && ball.m_preDataCount > ball.m_eventPreDataIndex[1] + 60 && ball.m_eventPreDataIndex[13] < 0) {
				ball.m_eventPreDataIndex[13] = ball.m_preDataCount;

				// shotpreview.cpp:964
				switch (GolfClub().GetID()) {
				case 0:
					ball.m_velocity.y = -8.0f - (344.0f - (GolfClub().GetPower() * rateSqrt < 344.0f ? GolfClub().GetPower() * rateSqrt : 344.0f)) * 0.19196428f;
					break;
				case 1:
					ball.m_velocity.y = -10.3f - (306.0f - (GolfClub().GetPower() * rateSqrt < 306.0f ? GolfClub().GetPower() * rateSqrt : 306.0f)) * 0.1952381f;
					break;
				case 2:
					ball.m_velocity.y = -10.8f - (273.0f - (GolfClub().GetPower() * rateSqrt < 273.0f ? GolfClub().GetPower() * rateSqrt : 273.0f)) * 0.202f;
					break;
				}

				// shotpreview.cpp:988
				ball.m_velocity *= rateSqrt * 7.0f;
				ball.m_spin = ball.m_lieAngle;
			}
		}
	}

	// shotpreview.cpp:994
	ball.m_groundType = groundType;
	ball.m_flags = ball.m_powerFlags;

	// shotpreview.cpp:1002
	bool firstBounce = true;
	for (int i = 0; i < ball.m_preDataCount; ++i) {
		path.push_back(ball.PreData(i)->m_pos);
		if (ball.PreData(i)->m_groundType > 0) {
			bounces.push_back(ball.PreData(i)->m_pos);
			if (firstBounce) {
				firstBounce = false;
				if (count)
					*count = i;
			}
		}
	}

	// shotpreview.cpp:1017
	path.push_back(ball.PreData(ball.m_preDataCount)->m_pos);
	bounces.push_back(ball.PreData(ball.m_preDataCount)->m_pos);

	if (count && firstBounce)
		*count = path.size() - 1;

	// shotpreview.cpp:1025
	ball.Reset(WVector());

	GolfBall() = *m_pBall;
	GetPVS().SetFrame(frame - 1);
	GetPVS().PreProcess(0.02f, true);

	return true;
}
