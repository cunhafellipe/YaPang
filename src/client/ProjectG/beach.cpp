// Original: d:\build\custom\temp\code\client\projectg\beach.cpp
#include "beach.h"
#include "wresrcmng.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "wvideo.h"
#include "wview.h"
#include "quadtree.h"
#include "wscene.h"
#include <stdio.h>
#include <string.h>

extern LightSet g_lightset;

// beach.cpp:17
void CBeach::ReadAttribute(const char* attribute) {
	if (attribute) {
		int id;

		if (strchr(attribute, '_') && sscanf(strchr(attribute, '_') + 1, "%d", &id)) {
			m_texture = g_resrcmng->LoadTexture(GOLFDOC()->m_waveInfo.GetTexture(id), 0, 0, 0);
			m_range = GOLFDOC()->m_waveInfo.GetWidth(id);
			m_freq = GOLFDOC()->m_waveInfo.GetFreq(id);
		} else {
			m_texture = g_resrcmng->LoadTexture("[water.dds", 0, 0, 0);
			m_range = 76.8f;
			m_freq = 7.5f;
		}

		m_fadeInTime = m_freq * 0.26666668f;
		m_fadeOutTime = m_freq * 0.53333336f;
		m_restartDelay = m_freq * 0.2f;

		m_deceleration = m_range / (m_fadeInTime * 4.0f - m_fadeInTime * m_fadeInTime * 0.5f);
	}
}

// beach.cpp:49
void CBeach::Process(float elapsed) {
	for (unsigned char i = 0; i < m_layers.size(); i++) {
		m_layers[i].m_time += elapsed;

		switch (m_layers[i].m_state) {
		case 0:
			m_layers[i].m_alpha += elapsed * (255.0f / m_fadeInTime);
			m_layers[i].m_speed -= elapsed * m_deceleration;
			m_layers[i].m_offset += elapsed * m_layers[i].m_speed;

			if (m_layers[i].m_alpha > 255.0f) {
				m_layers[i].m_alpha = 255.0f;
				m_layers[i].m_state = 1;
				m_layers[(i + 1) % m_layers.size()].m_delay = m_restartDelay;
			}
			break;
		case 1:
			m_layers[i].m_alpha -= elapsed * (255.0f / m_fadeOutTime);
			m_layers[i].m_speed -= elapsed * m_deceleration;
			m_layers[i].m_offset += elapsed * m_layers[i].m_speed;

			if (m_layers[i].m_alpha < 0.0f) {
				m_layers[i].m_state = 2;
			}
			break;
		case 2:
			if (m_layers[i].m_delay > 0.0f) {
				m_layers[i].m_delay -= elapsed;
				if (m_layers[i].m_delay < 0.0f) {
					m_layers[i].m_state = 0;
					m_layers[i].m_alpha = 0;
					m_layers[i].m_time = 0;
					m_layers[i].m_offset = 0;
					m_layers[i].m_speed = m_deceleration * 4.0f;
				}
			}
			break;
		}
	}
}

// beach.cpp:97
void CBeach::Render(void) {
	float maxTime = 0;
	unsigned char oldest = 0;
	for (unsigned char i = 0; i < m_layers.size(); i++) {
		if (maxTime < m_layers[i].m_time) {
			oldest = i;
			maxTime = m_layers[i].m_time;
		}
	}

	unsigned char r = (unsigned char)(255 - (unsigned char)(((0xFF - (g_lightset.ambient2 >> 16)) & 0xFF) * 0.8f));
	unsigned char g = (unsigned char)(255 - (unsigned char)(((0xFF - (g_lightset.ambient2 >> 8)) & 0xFF) * 0.8f));
	unsigned char b = (unsigned char)(255 - (unsigned char)(((0xFF - g_lightset.ambient2) & 0xFF) * 0.8f));
	unsigned long color = (r << 16) | (g << 8) | b;

	for (unsigned char i = 0; i < m_layers.size(); i++) {
		if (m_layers[(oldest + i) % m_layers.size()].m_state != 2) {
			for (unsigned char j = 0; j < m_wavelets.size() - 1; j++) {
				for (unsigned char k = 0; k < 4; k++) {
					WVector pos;
					unsigned char index = ((k >> 1) & 1) + j;
					bool edge = index == 0 || index == m_wavelets.size() - 1;

					WVector dir = m_wavelets[index].dir * (((k & 1) ? 0.1f : (edge ? 0.5f : 1.0f)) * m_layers[(oldest + i) % m_layers.size()].m_offset);
					pos = GetPVS().GetGroundPoint(m_wavelets[index].pos[k & 1] - dir, true, NULL, true);
					pos.y += (k & 1) ? 1.5f : 1.0f;

					vtr[k].SetPosition(pos);

					vtr[k].tu = m_wavelets[index].length[k & 1];

					if (edge) {
						vtr[k].diffuse = color | 0xFF000000;
					} else {
						vtr[k].diffuse = ((int)m_layers[(oldest + i) % m_layers.size()].m_alpha << 24) | color;
					}
				}

				g_view->DrawPolygonFan(vtx, (m_texture & 0x7FF) | 0x28400000, 0x400, false);
			}
		}
	}
}
