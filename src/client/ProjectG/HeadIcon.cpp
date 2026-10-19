// Original: d:\build\custom\temp\code\client\projectg\headicon.cpp
#include "headicon.h"
#include "wresrcmng.h"
#include "client/Wangreal/include/woverlay.h"
#include "client/Wangreal/include/wview.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "ProjectG.h"
#include "Packet.h"
#include "commonutil.h"
#include "shared/ItemManager.h"
#include "shared/classdefine.h"
#include "NetResourceManager.h"
#include "bitmap.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fremoticon.h"

// shareddoc.inl:503; not declared by SharedDoc.h yet (GolfRule emits the
// copy the original calls).
unsigned char MyIndex(void);

// headicon.cpp:26
CHeadIcon::CHeadIcon(void) {
	m_visible = true;

	for (int i = 0; i < 4; i++) {
		m_heads[i].m_iconId = 0xFFFF;
		m_heads[i].m_elapsed = 0.0f;
		m_heads[i].m_levelIcon = 0;
	}

	m_emoticonObject = g_resrcmng->GetOverlay("[emoti_object.png", 0);

	m_dstRect.w = 64.0f;
	m_dstRect.h = 64.0f;
	m_srcRect.w = 0.25f;
	m_srcRect.h = 0.25f;

	m_icons.clear();

	for (unsigned char i = 0; i < 44; i++) {
		if (i == 13)
			continue;

		IFF_STRUCT::sItem *item = ItemManager()->FindItem(0x18000000 | i);
		if (item == 0)
			continue;

		WOverlay *overlay = g_resrcmng->GetOverlay(MakeStr("%s.tga", item->m_icon), 0);
		if (overlay == 0)
			continue;

		unsigned long typeId = item->m_typeId;
		m_icons[typeId] = overlay;
	}
}

// headicon.cpp:64
CHeadIcon::~CHeadIcon(void) {
	if (g_resrcmng && m_emoticonObject) { g_resrcmng->Release(m_emoticonObject); m_emoticonObject = 0; }

	for (int i = 0; i < 4; i++) {
		if (g_resrcmng && m_heads[i].m_levelIcon) { g_resrcmng->Release(m_heads[i].m_levelIcon); m_heads[i].m_levelIcon = 0; }
	}

	for (std::map<unsigned long, WOverlay*>::iterator it = m_icons.begin(); it != m_icons.end(); ++it) {
		if (g_resrcmng && it->second) { g_resrcmng->Release(it->second); it->second = 0; }
	}
	m_icons.clear();
}

// headicon.cpp:77
void CHeadIcon::SetLevel(void) {
	int num = GOLFDOC()->GetPlayerNum();
	if (num > 4) num = 4;

	for (int i = 0; i < num; i++) {
		if (g_resrcmng && m_heads[i].m_levelIcon) { g_resrcmng->Release(m_heads[i].m_levelIcon); m_heads[i].m_levelIcon = 0; }

		if (WSingleton<CSharedDoc>::Instance()->m_currentChannel.flags & 0x80) {
			m_heads[i].m_levelIcon = g_resrcmng->GetOverlay(MakeStr("ladder_%03d.tga", WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_ladderPoint / 100), 0);
		} else if (WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_skinTypeId) {
			IFF_STRUCT::sSkin *skin = ItemManager()->FindSkin(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_skinTypeId);
			if (skin)
				m_heads[i].m_levelIcon = g_resrcmng->GetOverlay(MakeStr("%s.tga", skin->m_icon), 0);
			else
				m_heads[i].m_levelIcon = g_resrcmng->GetOverlay(MakeStr("level_%03d.tga", WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_level + 1), 0);
		} else {
			m_heads[i].m_levelIcon = g_resrcmng->GetOverlay(MakeStr("level_%03d.tga", WSingleton<CSharedDoc>::Instance()->m_roomPlayers[i].m_level + 1), 0);
		}
	}
}

// headicon.cpp:117
void CHeadIcon::SetPlayerPos(int index, WVector &pos) {
	if (index < 0 || index >= sizeof(m_heads) / sizeof(m_heads[0]))
		return;

	WVector screen = g_view->Projection(pos);

	sPlayerHead &head = m_heads[index];
	WVector diff = pos - g_view->camera.pivot;
	head.m_screenPos.x = screen.x;
	head.m_screenPos.y = screen.y;
	head.m_screenPos.z = screen.z;
	head.m_distance = diff.Magnitude();
}

// headicon.cpp:145
bool CHeadIcon::SetIcon(unsigned long uid, unsigned short iconId) {
	unsigned char index = WSingleton<CSharedDoc>::Instance()->GetIndex(uid);
	if (index < 4) {
		sPlayerHead &head = m_heads[index];
		head.m_iconId = iconId;
		head.m_elapsed = 0.0f;
		return true;
	}

	return false;
}

// headicon.cpp:160
bool CHeadIcon::SendIcon(unsigned short iconId) {
	if (OnlinePlay()) {
		WSendPacket packet(GCT_EMOTICON);
		packet.Encode2(iconId);
		packet.Send(SEND_TO_DEFAULT);

		return SetIcon(WSingleton<CSharedDoc>::Instance()->GetPangyaUserInfo().guid, iconId);
	}

	return SetIcon(GOLFDOC()->m_myPlayerIndex, iconId);
}

// headicon.cpp:175
void CHeadIcon::Process(float elapsed) {
	int playerNum = GOLFDOC()->GetPlayerNum();
	int num = playerNum > 4 ? 4 : playerNum;

	for (int i = 0; i < num; i++) {
		if (PLAYER(i)->m_state == 3)
			continue;

		m_heads[i].m_elapsed += elapsed;
		if (m_heads[i].m_elapsed > 3.0f && m_heads[i].m_iconId != 0x1D) {
			m_heads[i].m_iconId = 0xFFFF;
			m_heads[i].m_elapsed = 0.0f;
		}
	}
}

// headicon.cpp:197
void CHeadIcon::DisplayOverPlayer(void) {
	if (!m_visible)
		return;

	if (WSingleton<CProjectG>::Instance()->HidePrivacy())
		return;

	if (GOLFDOC()->GameMode() & 0x20000) {
		for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
			unsigned char roomType = WSingleton<CSharedDoc>::Instance()->m_roomType;
			if ((roomType == 0 || roomType == 3 || roomType == 7) && PLAYER(i)->m_state == 3)
				continue;

			DisplayOverPlayer(i, true);
		}
	} else if (GOLFDOC()->GameMode() & 0x1000) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
			DisplayOverPlayer(MyIndex(), false);
			if (PLAYER(MyIndex())->m_partnerIndex != 0xFF)
				DisplayOverPlayer(PLAYER(MyIndex())->m_partnerIndex, false);
		} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 3) {
			DisplayOverPlayer(MyIndex(), false);
		} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 7) {
			DisplayOverPlayer(MyIndex(), false);
		} else {
			DisplayOverPlayer(GOLFDOC()->m_myPlayerIndex, false);
		}
	} else {
		DisplayOverPlayer(GOLFDOC()->m_myPlayerIndex, false);
	}
}

// headicon.cpp:250
void CHeadIcon::DisplayOverPlayer(unsigned char index, bool team) {
	sPlayerHead &head = m_heads[index];

	if (head.m_screenPos.z < 0.0f)
		return;

	float distance = head.m_distance;
	if (head.m_screenPos.z <= 0.0f || 1.0f <= head.m_screenPos.z || distance > 640.0f)
		return;

	if (head.m_iconId < 0xFFFF) {
		int col = head.m_iconId % 4, row = head.m_iconId % 16 / 4;

		m_srcRect.x = col * 0.25f;
		m_srcRect.y = row * 0.25f;

		m_dstRect.x = (float)float2int(head.m_screenPos.x - 60.0f - 1.0f / distance * 200.0f);
		m_dstRect.y = (float)float2int(head.m_screenPos.y - 52.0f - 1.0f / distance * 800.0f);

		m_emoticonObject->Render(g_view, m_srcRect, m_dstRect, 0, 0xFFFFFFFF, 0.0f, 0);
	} else if (PLAYER(index)->m_shotItemTypeId != 0) {
		m_srcRect.y = 0.0f;
		m_dstRect.x = (float)float2int(head.m_screenPos.x - 60.0f - 1.0f / distance * 200.0f);
		m_dstRect.y = (float)float2int(head.m_screenPos.y - 52.0f - 1.0f / distance * 800.0f);

		if (PLAYER(index)->m_shotItemTypeId == 0x1800002D) {
			m_srcRect.x = 0.5f;
			m_emoticonObject->Render(g_view, m_srcRect, m_dstRect, 0, 0xFFFFFFFF, 0.0f, 0);
		} else if (PLAYER(index)->m_shotItemTypeId != 0x1800000E) {
			m_srcRect.x = 0.0f;
			m_emoticonObject->Render(g_view, m_srcRect, m_dstRect, 0, 0xFFFFFFFF, 0.0f, 0);

			m_icons[PLAYER(index)->m_shotItemTypeId]->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(m_dstRect.x + 28.0f, m_dstRect.y + 30.0f, 42.0f, 42.0f), 0, 0xFFFFFFFF, 0.0f, 0);
		}
	}

	if (GOLFDOC()->GameMode() == 0x800 && WSingleton<CSharedDoc>::Instance()->m_roomType == 10)
		return;

	if (GOLFDOC()->GameMode() == 0x200)
		return;

	FrGraphicInterface *gdi = g_pFresh->GetManager()->GetGDI();
	FrEmoticon *emoticon = g_pFresh->GetManager()->GetEmoticon();

	if (gdi == 0)
		return;

	gdi->SetAlpha(1.0f);

	const char *name = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_name[0] ? WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_name : "???";

	float textWidth = emoticon ? emoticon->GetTextWidth(name) : gdi->GetTextExtend(name);
	bool hasGuild = WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_guildUID != 0 && (WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_identity & 0x14) == 0;

	WRect levelRect;
	levelRect.w = head.m_levelIcon ? (float)head.m_levelIcon->GetWidth() : 0.0f;
	levelRect.h = head.m_levelIcon ? (float)head.m_levelIcon->GetHeight() : 0.0f;

	float width;
	if (team) {
		width = levelRect.w;
		if (hasGuild)
			width = levelRect.w + 27.0f;
	} else {
		width = levelRect.w + textWidth + 5.0f;
	}
	levelRect.x = floorf(head.m_screenPos.x - width * 0.5f);
	levelRect.y = floorf(head.m_screenPos.y - 30.0f - 1200.0f / distance);

	if (IsMassGame()) {
		if (WSingleton<CSharedDoc>::Instance()->m_roomType != 10) {
			const Bitmap *bmp = 0;
			if (WSingleton<CSharedDoc>::Instance()->m_roomType == 4) {
				bmp = g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_rank");
			} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
				bmp = g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_rank_02");
			} else if (WSingleton<CSharedDoc>::Instance()->m_roomType == 5) {
				if (WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_team == 0)
					bmp = g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_team_redteam_rank");
				else
					bmp = g_pFresh->GetManager()->GetBitmap("FRAMES4", "30in_team_blueteam_rank");
			}

			if (bmp) {
				if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6)
					gdi->DrawTexture(bmp, WRect(levelRect.x - 30.0f, levelRect.y - 1.0f, (float)bmp->Width(), (float)bmp->Height()), 0xFFFFFFFF, 0);
				else
					gdi->DrawTexture(bmp, WRect(levelRect.x - 19.0f, levelRect.y - 1.0f + 2.0f, (float)bmp->Width(), (float)bmp->Height()), 0xFFFFFFFF, 0);
			}
		}

		gdi->SetTextStyle(0);
		gdi->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);

		if (WSingleton<CSharedDoc>::Instance()->m_roomType == 6) {
			gdi->PrintText(WPoint(levelRect.x - 15.0f, levelRect.y + 2.0f), 1, MakeStr("%d \xC1\xB6", WSingleton<CSharedDoc>::Instance()->m_flag5020), 0);
		} else if (WSingleton<CSharedDoc>::Instance()->m_roomType != 10)
			gdi->PrintText(WPoint(levelRect.x - 10.0f, levelRect.y + 2.0f), 1, MakeStr("%d", WSingleton<CSharedDoc>::Instance()->m_gameResultList[MyIndex()].m_rank), 0);
	}

	if (head.m_levelIcon && !OfflinePlay())
		head.m_levelIcon->Render(g_view, WRect(0.0f, 0.0f, 1.0f, 1.0f), WRect(levelRect.x, levelRect.y + 8.0f - levelRect.h * 0.5f, levelRect.w, levelRect.h), 0, 0xFFFFFFFF, 0.0f, 0);

	float textX = levelRect.x + levelRect.w + 5.0f, textY = levelRect.y;
	if (hasGuild) {
		const Bitmap *emblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(WSingleton<CSharedDoc>::Instance()->m_roomPlayers[index].m_guildMark);

		if (emblem) {
			gdi->DrawTexture(emblem, WRect(textX, levelRect.y - 6.0f, (float)emblem->Width(), (float)emblem->Height()), 0xFFFFFFFF, 0);
			textX += 27.0f;
		}
	}

	if (team) {
		textX = head.m_screenPos.x - textWidth * 0.5f;
		textY = levelRect.y + 20.0f;
	}

	gdi->SetTextStyle(2);
	gdi->SetTextColor(0xFFFFFFFF, 0xFF000000);

	WPoint pos;
	pos.x = (float)float2int(textX);
	pos.y = (float)float2int(textY);

	if (emoticon)
		emoticon->PrintText(pos, 0, name, 0xFFFFFFFF);
	else
		gdi->PrintText(pos, 0, name, 0);

	gdi->SetAlpha(1.0f);
}

// headicon.cpp:447
void CHeadIcon::DisplayBesideBoard(float left, float right) {
	if (!(GOLFDOC()->GameMode() & 0xFFFFF77E))
		return;

	right -= 64.0f;

	for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++) {
		if (PLAYER(i)->m_state == 3)
			continue;

		sPlayerHead &head = m_heads[i];
		if (PLAYER(i)->m_chatPenalty) {
			m_srcRect.x = 0.75f;
			m_srcRect.y = 0.0f;

			if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
				m_dstRect.x = PLAYER(i)->m_attribute == 0 ? left : right;
				if (PLAYER(i)->m_partnerIndex == 0xFF)
					m_dstRect.y = 10.0f;
				else
					m_dstRect.y = i < PLAYER(i)->m_partnerIndex ? 10.0f : 60.0f;

				m_emoticonObject->Render(g_view, m_srcRect, m_dstRect, 0, 0xCCFFFFFF, 0.0f, PLAYER(i)->m_attribute == 0);
			} else {
				m_dstRect.x = (i & 2) ? right : left;
				m_dstRect.y = (i & 1) ? 81.0f : 10.0f;

				m_emoticonObject->Render(g_view, m_srcRect, m_dstRect, 0, 0xCCFFFFFF, 0.0f, !(i & 2));
			}
		} else if (i != GOLFDOC()->m_myPlayerIndex && head.m_iconId < 0xFFFF) {
			int col = head.m_iconId % 4, row = head.m_iconId % 16 / 4;

			m_srcRect.x = col * 0.25f;
			m_srcRect.y = row * 0.25f;

			if (WSingleton<CSharedDoc>::Instance()->m_roomType == 1) {
				m_dstRect.x = PLAYER(i)->m_attribute == 0 ? left : right;
				if (PLAYER(i)->m_partnerIndex == 0xFF)
					m_dstRect.y = 10.0f;
				else
					m_dstRect.y = i < PLAYER(i)->m_partnerIndex ? 10.0f : 60.0f;

				m_emoticonObject->Render(g_view, m_srcRect, m_dstRect, 0, 0xCCFFFFFF, 0.0f, PLAYER(i)->m_attribute == 0);
			} else {
				m_dstRect.x = (i & 2) ? right : left;
				m_dstRect.y = (i & 1) ? 81.0f : 10.0f;

				m_emoticonObject->Render(g_view, m_srcRect, m_dstRect, 0, 0xCCFFFFFF, 0.0f, !(i & 2));
			}
		}
	}
}
