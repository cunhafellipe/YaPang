// Original: d:\build\custom\temp\code\client\projectg\partsinfo.cpp
#include "partsinfo.h"
#include "parttidlist.h"
#include "../../shared/classdefine.h"
#include "../../shared/Localize.h"
#include "../../shared/ItemManager.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "TaskManager.h"
#include "../Wangreal/include/wpuppet.h"
#include "../Wangreal/include/wbone.h"
#include "../Wangreal/include/wresrcmng.h"
#include "../Wangreal/include/bitmap.h"
#include "UccLibrary.h"
#include "UccManager.h"
#include "../../shared/ExceptionReport.h"

#include <stdio.h>
#include <string.h>
#include <string>

// partsinfo.h:41
unsigned char CPartList::GetNumTotalParts(void) const {
	return m_numTotalParts;
}

// partsinfo.cpp:24
CPartList::CPartList(void)
	: m_tidChar(0), m_numDefaultParts(0), m_numTotalParts(0), m_hairClr(0), m_shirtsClr(0) {
}

// partsinfo.cpp:28
CPartList::~CPartList(void) {
}

// partsinfo.cpp:32
bool CPartList::Build(const CPartTidList *pTidList) {
	if (!pTidList->IsComboValid()) {
		ItemManager()->GetDefCombo(pTidList->m_tidChar, (unsigned long *)pTidList->m_partTids);
	}

	memset(m_parts, 0, sizeof(m_parts));

	m_tidChar = pTidList->m_tidChar;
	m_numDefaultParts = pTidList->m_unk04;
	m_numTotalParts = pTidList->m_numTotalParts;
	m_hairClr = pTidList->m_hairClr;
	m_shirtsClr = pTidList->m_shirtsClr;

	for (int i = 0; i < m_numTotalParts; i++) {
		if (pTidList->m_partTids[i] != 0) {
			IFF_STRUCT::sPart *pIff = ItemManager()->FindPart(pTidList->m_partTids[i]);
			m_parts[i].m_tid = pTidList->m_partTids[i];
			m_parts[i].m_pIff = pIff;
			if (IsLocalContent((localContentType_t)0x5F)) {
				m_parts[i].m_id = pTidList->m_partIds[i];
			}
		}
	}

	return true;
}

// partsinfo.cpp:73
unsigned long CPartList::SetPart(CPartTidList *pTidList, unsigned long tid, unsigned long id) {
	if (((tid >> 18) & 0xFF) != (m_tidChar & 0x3FFFFFF) || ((tid >> 18) & 0xFF) != (pTidList->m_tidChar & 0x3FFFFFF)) {
		return 0;
	}
	if (!IsIdentical(pTidList) || !pTidList->IsComboValid()) {
		return 0;
	}
	if (!SetPart(tid, id)) {
		return 0;
	}
	if (!CheckSubParts()) {
		return 0;
	}

	unsigned long mask = 0;
	for (int i = 0; i < m_numTotalParts; i++) {
		if (pTidList->m_partTids[i] != m_parts[i].m_tid
			|| (IsLocalContent((localContentType_t)0x57) && m_parts[i].m_tid == tid && IsUccClothes(m_parts[i].m_tid))) {
			mask |= 1 << i;
			pTidList->m_partTids[i] = m_parts[i].m_tid;
			if (IsLocalContent((localContentType_t)0x5F)) {
				pTidList->m_partIds[i] = m_parts[i].m_id;
			}
		}
	}

	return mask;
}

// partsinfo.cpp:113
bool CPartList::SetPart(unsigned long tid, unsigned long id) {
	unsigned char slot = (unsigned char)((tid >> 13) & 0x1F);
	bool unequip = false;
	if (m_parts[slot].m_tid == tid && (id == 0 || m_parts[slot].m_id == 0 || m_parts[slot].m_id == id)) {
		if (slot < m_numDefaultParts) {
			tid = ((m_tidChar << 5 | slot) << 13) | 0x8000400;
			id = 0;
		} else {
			unequip = true;
		}
	}

	IFF_STRUCT::sPart *pIff = ItemManager()->FindPart(tid);
	if (pIff == 0) {
		return false;
	}

	unsigned long hideMask = pIff->hideMask;
	for (int i = 0; i < m_numTotalParts; i++) {
		if (m_parts[i].m_tid != 0) {
			unsigned long overlap = m_parts[i].m_pIff->hideMask & pIff->hideMask;
			if (overlap != 0) {
				hideMask &= ~overlap;
				if (overlap != m_parts[i].m_pIff->hideMask) {
					switch ((m_parts[i].m_tid >> 9) & 3) {
					case 0:
					case 2: {
						unsigned long subTid = m_parts[i].m_tid | 0x200;
						IFF_STRUCT::sPart *pSub = ItemManager()->FindPart(subTid);
						if (pSub != 0) {
							EmptyList(pSub->hideMask);
							for (int j = 0; j < m_numTotalParts; j++) {
								if (pSub->hideMask & (1 << j)) {
									m_parts[j].m_tid = subTid;
									m_parts[j].m_pIff = pSub;
									if (IsLocalContent((localContentType_t)0x5F)) {
										m_parts[j].m_id = 0;
									}
									break;
								}
							}
						} else {
							unsigned long freed = (pIff->hideMask ^ m_parts[i].m_pIff->hideMask) & m_parts[i].m_pIff->hideMask;
							for (int j = 0; j < m_numTotalParts; j++) {
								if ((freed & (0xFFFFFFFF << j)) == 0) {
									break;
								}
								if (freed & (1 << j)) {
									unsigned long defTid = ((m_tidChar << 5 | j) << 13) | 0x8000400;
									IFF_STRUCT::sPart *pDef = ItemManager()->FindPart(defTid);
									if (pDef != 0) {
										if (pIff->hideMask & pDef->hideMask) {
											return false;
										}
										EmptyList(pDef->hideMask);
										m_parts[j].m_tid = defTid;
									} else {
										m_parts[j].m_tid = 0;
									}
									m_parts[j].m_pIff = pDef;
									if (IsLocalContent((localContentType_t)0x5F)) {
										m_parts[j].m_id = 0;
									}
								}
							}
						}
						break;
					}
					}
				}
				EmptyList(overlap);
			}
			if (hideMask == 0) {
				break;
			}
		}
	}

	if (!unequip) {
		m_parts[slot].m_tid = tid;
		m_parts[slot].m_pIff = pIff;
		if (IsLocalContent((localContentType_t)0x5F)) {
			m_parts[slot].m_id = id;
		}
	}

	unsigned long usedMask = 0;
	for (int k = 0; k < m_numTotalParts; k++) {
		if (m_parts[k].m_pIff != 0) {
			usedMask |= m_parts[k].m_pIff->hideMask;
		}
	}

	if (!unequip) {
		unsigned long subTid = ((m_tidChar << 5 | slot) << 13) | 0x8000600;
		IFF_STRUCT::sPart *pSub = ItemManager()->FindPart(subTid);
		if (pSub != 0 && (usedMask & pSub->hideMask) == 0) {
			int j;
			for (j = 0; j < m_numDefaultParts; j++) {
				if (pSub->hideMask & (1 << j)) {
					break;
				}
			}
			if (m_parts[j].m_tid == 0) {
				m_parts[j].m_tid = subTid;
				m_parts[j].m_pIff = pSub;
				if (IsLocalContent((localContentType_t)0x5F)) {
					m_parts[j].m_id = 0;
				}
			}
		}
	}

	for (int k = 0; k < m_numDefaultParts; k++) {
		if ((usedMask & (1 << k)) == 0) {
			unsigned long defTid = ((m_tidChar << 5 | k) << 13) | 0x8000400;
			IFF_STRUCT::sPart *pDef = ItemManager()->FindPart(defTid);
			if (pDef != 0 && (pDef->hideMask & usedMask) == 0) {
				m_parts[k].m_tid = defTid;
				m_parts[k].m_pIff = pDef;
				if (IsLocalContent((localContentType_t)0x5F)) {
					m_parts[k].m_id = 0;
				}
			}
		}
	}

	return true;
}

// partsinfo.cpp:322
void CPartList::EmptyList(unsigned long mask) {
	for (int i = 0; i < m_numTotalParts; i++) {
		if ((mask & (0xFFFFFFFF << i)) == 0) {
			break;
		}
		if (mask & (1 << i)) {
			m_parts[i].m_tid = 0;
			m_parts[i].m_pIff = 0;
			if (IsLocalContent((localContentType_t)0x5F)) {
				m_parts[i].m_id = 0;
			}
		}
	}
}

// partsinfo.cpp:343
bool CPartList::CheckSubParts(void) {
	int i;

	// A sub part (tid bits 9-10 = 1) stays only while a worn part lists it.
	for (i = 0; i < m_numTotalParts; i++) {
		unsigned long tid = m_parts[i].m_tid;
		if (tid != 0 && (tid & 0x600) == 0x200) {
			int j;
			for (j = 0; j < m_numTotalParts; j++) {
				if (m_parts[j].m_pIff != 0 && i != j) {
					int k;
					for (k = 0; k < 2; k++) {
						if (m_parts[j].m_pIff->m_subParts[k] == tid)
							break;
					}
					if (k < 2)
						break;
				}
			}

			if (j >= m_numTotalParts) {
				if (!SetPart(tid, m_parts[i].m_id))
					return false;
			} else if (i >= m_numDefaultParts && m_parts[i].m_pIff != 0) {
				if (!IsEmpty(m_parts[i].m_pIff->hideMask, (unsigned char)i)) {
					m_parts[i].m_tid = 0;
					m_parts[i].m_pIff = 0;
					if (IsLocalContent((localContentType_t)0x5F)) {
						m_parts[i].m_id = 0;
					}
				}
			}
		}
	}

	// Wear the sub parts of every worn part.
	for (i = 0; i < m_numTotalParts; i++) {
		if (m_parts[i].m_pIff != 0) {
			for (int k = 0; k < 2; k++) {
				unsigned long subTid = m_parts[i].m_pIff->m_subParts[k];
				if (subTid != 0) {
					unsigned char slot = (unsigned char)((subTid >> 13) & 0x1F);
					if (slot < m_numDefaultParts) {
						if ((m_parts[slot].m_tid & 0x600) == 0x400) {
							if (!SetPart(subTid, m_parts[i].m_id))
								return false;
						}
					} else {
						IFF_STRUCT::sPart *pSub = ItemManager()->FindPart(subTid);
						if (pSub != 0 && IsEmpty(pSub->hideMask, 0xFF)) {
							if (!SetPart(m_parts[i].m_pIff->m_subParts[k], m_parts[i].m_id))
								return false;
						}
					}
				}
			}
		}
	}

	return true;
}

// partsinfo.cpp:426
bool CPartList::IsEmpty(unsigned long mask, unsigned char except) {
	for (int i = 0; i < m_numTotalParts; i++) {
		if (m_parts[i].m_pIff != 0 && except != i && (m_parts[i].m_pIff->hideMask & mask)) {
			return false;
		}
	}
	return true;
}

// partsinfo.cpp:442
bool CPartList::SetHairClr(CPartTidList *pTidList, unsigned char clr) {
	if (m_tidChar != pTidList->m_tidChar) {
		return false;
	}
	if (pTidList->m_hairClr == clr) {
		return false;
	}
	pTidList->m_hairClr = clr;
	m_hairClr = clr;
	return true;
}

// partsinfo.cpp:458
bool CPartList::SetShirtsClr(CPartTidList *pTidList, unsigned char clr) {
	if (m_tidChar != pTidList->m_tidChar) {
		return false;
	}
	if (pTidList->m_shirtsClr == clr) {
		return false;
	}
	pTidList->m_shirtsClr = clr;
	m_shirtsClr = clr;
	return true;
}

// partsinfo.cpp:474
bool CPartList::IsIdentical(CPartTidList *pTidList) {
	if (m_tidChar != pTidList->m_tidChar) {
		return false;
	}
	if (m_numDefaultParts != pTidList->m_unk04 || m_numTotalParts != pTidList->m_numTotalParts) {
		return false;
	}
	int i;
	for (i = 0; i < m_numTotalParts; i++) {
		if (m_parts[i].m_tid != pTidList->m_partTids[i]) {
			return false;
		}
	}
	if (IsLocalContent((localContentType_t)0x5F)) {
		for (i = 0; i < m_numTotalParts; i++) {
			if (m_parts[i].m_id != 0 && pTidList->m_partIds[i] != 0) {
				if (m_parts[i].m_id != pTidList->m_partIds[i]) {
					return false;
				}
			}
		}
	}
	return true;
}

// partsinfo.cpp:505
bool CPartList::FindPart(unsigned long tid) const {
	for (unsigned char i = 0; i < m_numTotalParts; i++) {
		if (GetPart(i)->m_tid == tid) {
			return true;
		}
	}
	return false;
}

// partsinfo.cpp:521
CPetFrame::CPetFrame(void)
	: m_tidChar(0), m_pBonePet(0), m_hairTex(0), m_shirtsTex(0), m_faceTex(0), m_pCurAniPet(0),
	  m_unk28(0), m_numPets(0), m_makeupIndex(1), m_unk1B4(true), m_hasMakeup(false) {
	memset(m_pets, 0, sizeof(m_pets));
	memset(m_petTex, 0, sizeof(m_petTex));
	memset(m_auxParts, 0, sizeof(m_auxParts));
}

// partsinfo.cpp:528
CPetFrame::~CPetFrame(void) {
	ReleasePets(7);
}

// partsinfo.cpp:533
void CPetFrame::ReleasePets(int mask) {
	if ((mask & 1) && g_resrcmng != 0 && m_pBonePet != 0) {
		g_resrcmng->Release(m_pBonePet, false);
		m_pBonePet = 0;
	}

	if (mask & 2) {
		for (std::vector<WPuppet *>::iterator it = m_aniPetList.begin(); it != m_aniPetList.end(); ++it) {
			if (*it != 0) {
				g_resrcmng->Release(*it, false);
			}
		}
		m_aniPetList.clear();
	}

	if (mask & 4) {
		for (int i = 0; i < m_numPets; i++) {
			if (g_resrcmng != 0 && m_pets[i] != 0) {
				g_resrcmng->Release(m_pets[i], false);
				m_pets[i] = 0;
			}
			for (int j = 0; j < 3; j++) {
				if (g_resrcmng != 0 && m_petTex[i][j] != 0) {
					g_resrcmng->Release(m_petTex[i][j]);
					m_petTex[i][j] = 0;
				}
			}
		}

		if (m_numPets != 0) {
			memset(m_pets, 0, sizeof(m_pets));
			memset(m_petTex, 0, sizeof(m_petTex));
			m_numPets = 0; m_unk28 = 0;
		}

		if (g_resrcmng != 0 && m_faceTex != 0) {
			g_resrcmng->Release(m_faceTex);
			m_faceTex = 0;
		}
		if (g_resrcmng != 0 && m_hairTex != 0) {
			g_resrcmng->Release(m_hairTex);
			m_hairTex = 0;
		}
		if (g_resrcmng != 0 && m_shirtsTex != 0) {
			g_resrcmng->Release(m_shirtsTex);
			m_shirtsTex = 0;
		}
	}
}

// partsinfo.cpp:594
bool CPetFrame::Build(const CPartTidList *pTidList, unsigned long *pAuxParts, bool bColors, bool bUpdateTnL, int faceIndex) {
	char bpetName[128];
	char apetName[128];
	char faceTexName[256];

	if (!m_partList.Build(pTidList)) {
		return false;
	}

	bool sameChar = pTidList->m_tidChar == m_tidChar;
	if (!sameChar) {
		ReleasePets(7);
	}
	m_tidChar = pTidList->m_tidChar;
	m_unk28 = pTidList->m_unk04;
	m_numPets = pTidList->m_numTotalParts;

	IFF_STRUCT::sChar *pChar = ItemManager()->FindChar(m_tidChar);
	if (pChar == 0) {
		return false;
	}

	sprintf(bpetName, "%s.bpet", pChar->m_model);

	if (!sameChar || m_pBonePet == 0) {
		m_pBonePet = g_resrcmng->GetPuppet(bpetName, false, false, false);
		if (m_pBonePet == 0) {
			g_exceptionReport.Crash();
			return false;
		}
	}

	sprintf(apetName, "%s.apet", pChar->m_model);
	AddAniPet(apetName);

	SetPartPets(0xFFFFFFFF);

	if (pAuxParts != 0) {
		memcpy(m_auxParts, pAuxParts, sizeof(m_auxParts));
	} else {
		memset(m_auxParts, 0, sizeof(m_auxParts));
	}

	strcpy(faceTexName, pChar->m_faceTexture);
	sprintf(strrchr(faceTexName, '.'), "_%d%s", faceIndex, strrchr(pChar->m_faceTexture, '.'));

	m_faceTex = g_resrcmng->LoadTexture(pChar->m_faceTexture, 0, 0, faceTexName);
	w_texlist *pTex = g_resrcmng->FindTexture(pChar->m_faceTexture);

	if (pTex != 0) {
		m_pBonePet->ChangeTextureByHandle(m_faceTex, pTex->texhandle, 0);
	}

	m_pBonePet->EndMeshBoneWork(); ApplyHideMask(true);

	if (bUpdateTnL) {
		m_pBonePet->xUpdateTnLBuffers(true);
	}

	if (!bColors) {
		ReleasePets(2);
	} else {
		SetHairClr(const_cast<CPartTidList *>(pTidList), pTidList->m_hairClr);
		SetShirtsClr(0, pTidList->m_shirtsClr);
		m_unk1B4 = true;
	}

	return true;
}

// partsinfo.cpp:683
void CPetFrame::SetPartPets(unsigned long mask) {
	char faceTexName[256];
	char mpetName[32];
	int texA[3];
	int texB[3];
	int hBaseTex = 0;

	m_unk1B4 = false;
	if (mask == 0xFFFFFFFF) {
		m_hasMakeup = false;
		m_makeupIndex = 1;
	}

	IFF_STRUCT::sChar *pChar = ItemManager()->FindChar(m_tidChar);
	if (pChar->m_baseTexture[0] != 0) {
		hBaseTex = g_resrcmng->LoadTexture(pChar->m_baseTexture, 0, 0, 0);
	}

	for (int slot = 0; slot < m_numPets; slot++) {
		if ((mask & (0xFFFFFFFF << slot)) == 0) {
			break;
		}
		if ((mask & (1 << slot)) == 0) {
			continue;
		}

		texA[0] = 0; texA[1] = 0; texA[2] = 0;
		texB[0] = 0; texB[1] = 0; texB[2] = 0;
		IFF_STRUCT::sPart *pIff = m_partList.GetPart(slot)->m_pIff;

		if (pIff != 0) {
			if ((pIff->m_typeId & 0x1800) == 0x1000) {
				m_makeupIndex = slot;
				m_hasMakeup = true;
			} else if (pIff->hideMask & 2) {
				m_makeupIndex = 1;
				m_hasMakeup = false;
			}

			sprintf(mpetName, "%s.mpet", pIff->model);

			if (IsLocalContent((localContentType_t)0x57) && IsLocalContent((localContentType_t)0x5F) && IsUccClothes(pIff->m_typeId)) {
				char uccKey[9] = { 0 };
				sUccClothes *pClothes = UccManager()->FindClothes(m_partList.GetPart(slot)->m_id);
				if (pClothes != 0) {
					strncpy(uccKey, pClothes->m_uccKey, 9);
					if (UccManager()->IsDownloadComplete(pClothes)) {
						texA[0] = g_resrcmng->LoadTexture(GenerateTextureName_F(pIff->m_typeId, uccKey).c_str(), 0, 0, 0);
						texB[0] = g_resrcmng->LoadTexture(pIff->texture[0], 0, 0, 0);
						texA[1] = g_resrcmng->LoadTexture(GenerateTextureName_B(pIff->m_typeId, uccKey).c_str(), 0, 0, 0);
						texB[1] = g_resrcmng->LoadTexture(pIff->texture[1], 0, 0, 0);
						if (texA[0] == 0) {
							texA[0] = g_resrcmng->LoadTexture(pIff->texture[0], 0, 0, 0);
						}
						if (texA[1] == 0) {
							texA[1] = g_resrcmng->LoadTexture(pIff->texture[1], 0, 0, 0);
						}
					} else {
						for (int i = 0; i < 3; i++) {
							if (pIff->texture[i][0] != 0 && pIff->texture2[i][0] != 0) {
								texA[i] = g_resrcmng->LoadTexture(pIff->texture[i], 0, 0, 0);
								texB[i] = g_resrcmng->LoadTexture(pIff->texture2[i], 0, 0, 0);
							}
						}
					}
				} else {
					sItemInfo *pInfo = GetMyItemInfo(m_partList.GetPart(slot)->m_id);
					if (pInfo != 0) {
						strncpy(uccKey, pInfo->m_uccKey, 9);
					}

					bool found = false;
					std::list<sItemInfo> &items = CSharedDoc::Instance()->m_myItemList;
					for (std::list<sItemInfo>::iterator it = items.begin(); it != items.end(); ++it) {
						if (it->m_typeId == pIff->m_typeId && strlen(uccKey) != 0 && strcmp(it->m_uccKey, uccKey) == 0 && (it->m_uccStatus & 1) == 0) {
							texA[0] = g_resrcmng->LoadTexture(GenerateTextureName_F(pIff->m_typeId, uccKey).c_str(), 0, 0, 0);
							texB[0] = g_resrcmng->LoadTexture(pIff->texture[0], 0, 0, 0);
							texA[1] = g_resrcmng->LoadTexture(GenerateTextureName_B(pIff->m_typeId, uccKey).c_str(), 0, 0, 0);
							texB[1] = g_resrcmng->LoadTexture(pIff->texture[1], 0, 0, 0);
							found = true;
							break;
						}
					}
					if (!found) {
						for (int i = 0; i < 3; i++) {
							if (pIff->texture[i][0] != 0 && pIff->texture2[i][0] != 0) {
								texA[i] = g_resrcmng->LoadTexture(pIff->texture[i], 0, 0, 0);
								texB[i] = g_resrcmng->LoadTexture(pIff->texture2[i], 0, 0, 0);
							}
						}
					}
				}
			} else {
				for (int i = 0; i < 3; i++) {
					if (pIff->texture[i][0] != 0 && pIff->texture2[i][0] != 0) {
						texA[i] = g_resrcmng->LoadTexture(pIff->texture[i], 0, 0, 0);
						texB[i] = g_resrcmng->LoadTexture(pIff->texture2[i], 0, 0, 0);
					}
				}
			}

			if (m_pets[slot] != 0 && stricmp(m_pets[slot]->GetPuppetName(), mpetName) == 0) {
				for (int i = 0; i < 3; i++) {
					m_pBonePet->ChangeTextureByHandle(texA[i], texB[i], m_pets[slot]);
				}
			} else {
				WPuppet *pPet = g_resrcmng->GetPuppet(mpetName, false, true, false);
				if (pPet != 0) {
					m_pBonePet->ReplaceMeshBones(m_pets[slot], pPet, texA[0], texB[0]);
					for (int i = 1; i < 3; i++) {
						if (texA[i] != 0 && texB[i] != 0) {
							m_pBonePet->ChangeTextureByHandle(texA[i], texB[i], pPet);
						}
					}
					if (m_pets[slot] != 0) {
						g_resrcmng->Release(m_pets[slot], false);
					}
					m_pets[slot] = pPet;
				} else {
					CGolfDoc *pDoc = (CGolfDoc *)AfxGetDocument();
					if (pDoc != 0) {
						char *copy = new char[strlen(mpetName) + 1];
						strcpy(copy, mpetName);
						pDoc->m_ownedStrings.AddItem(copy, copy, false);
					}
				}
			}

			for (int i = 0; i < 3; i++) {
				if (texB[i] == hBaseTex && texA[i] == hBaseTex) {
					m_unk1B4 = true;
				}
				if (g_resrcmng != 0 && texB[i] != 0) {
					g_resrcmng->Release(texB[i]);
					texB[i] = 0;
				}
			}
		} else {
			m_pBonePet->ReplaceMeshBones(m_pets[slot], 0, 0, 0);
			if (g_resrcmng != 0 && m_pets[slot] != 0) {
				g_resrcmng->Release(m_pets[slot], false);
				m_pets[slot] = 0;
			}
		}

		for (int j = 0; j < 3; j++) {
			if (g_resrcmng != 0 && m_petTex[slot][j] != 0) {
				g_resrcmng->Release(m_petTex[slot][j]);
				m_petTex[slot][j] = 0;
			}
			m_petTex[slot][j] = texA[j];
		}
	}

	int hairSlot = -1;
	int faceSlot = -1;
	switch (pChar->m_typeId & 0x3FFFFFF) {
	case 2:
	case 7:
		hairSlot = 19;
		faceSlot = 1;
		break;
	case 4:
		hairSlot = 20;
		faceSlot = 1;
		break;
	}

	bool hasHairPart = false;
	bool hasFacePart = false;
	if (hairSlot >= 0) {
		if (m_partList.GetPart(hairSlot) != 0 && m_partList.GetPart(hairSlot)->m_tid != 0) {
			hasHairPart = true;
		}
	}
	if (faceSlot >= 0) {
		if (m_partList.GetPart(faceSlot) != 0 && (m_partList.GetPart(faceSlot)->m_tid & 0x1800) == 0x1000) {
			hasFacePart = true;
		}
	}

	if (hasHairPart) {
		if (faceSlot >= 0) {
			m_pBonePet->ReplaceMeshBones(m_pets[faceSlot], 0, 0, 0);
			if (g_resrcmng != 0 && m_pets[faceSlot] != 0) {
				g_resrcmng->Release(m_pets[faceSlot], false);
				m_pets[faceSlot] = 0;
			}
		}
	} else if (faceSlot >= 0 && m_partList.GetPart(faceSlot) != 0) {
		IFF_STRUCT::sPart *pIff = m_partList.GetPart(faceSlot)->m_pIff;
		if (pIff != 0) {
			char name[32] = "";
			sprintf(name, "%s.mpet", pIff->model);

			for (int i = 0; i < 3; i++) {
				texA[i] = 0;
				texB[i] = 0;
				if (pIff->texture[i][0] != 0 && pIff->texture2[i][0] != 0) {
					texA[i] = g_resrcmng->LoadTexture(pIff->texture[i], 0, 0, 0);
					texB[i] = g_resrcmng->LoadTexture(pIff->texture2[i], 0, 0, 0);
				}
			}

			WPuppet *pPet = g_resrcmng->GetPuppet(name, false, true, false);
			if (pPet != 0) {
				m_pBonePet->ReplaceMeshBones(m_pets[faceSlot], pPet, texA[0], texB[0]);
				for (int i = 1; i < 3; i++) {
					if (texA[i] != 0 && texB[i] != 0) {
						m_pBonePet->ChangeTextureByHandle(texA[i], texB[i], pPet);
					}
				}
				if (m_pets[faceSlot] != 0) {
					g_resrcmng->Release(m_pets[faceSlot], false);
				}
				m_pets[faceSlot] = pPet;
			}
		}
	}

	if (hasFacePart) {
		m_makeupIndex = faceSlot;
		m_hasMakeup = true;
		if (g_resrcmng != 0 && m_faceTex != 0) {
			g_resrcmng->Release(m_faceTex);
			m_faceTex = 0;
		}
		strcpy(faceTexName, pChar->m_faceTexture);
		sprintf(strrchr(faceTexName, '.'), "_%d%s", faceSlot, strrchr(pChar->m_faceTexture, '.'));
		m_faceTex = g_resrcmng->LoadTexture(pChar->m_faceTexture, 0, 0, faceTexName);
		w_texlist *pTex = g_resrcmng->FindTexture(pChar->m_faceTexture);
		if (pTex != 0) {
			m_pBonePet->ChangeTextureByHandle(m_faceTex, pTex->texhandle, 0);
		}
	}

	if (g_resrcmng != 0 && hBaseTex != 0) {
		g_resrcmng->Release(hBaseTex);
	}
}

// partsinfo.cpp:1026
char *CPetFrame::GetMakeupTexture(void) {
	return m_partList.GetPart(m_makeupIndex)->m_pIff->texture[0];
}

// partsinfo.cpp:1032
WPuppet *CPetFrame::GetBonePet(void) {
	return m_pBonePet;
}

// partsinfo.cpp:1047
bool CPetFrame::Refresh(CPartTidList *pTidList) {
	if (!ResetFace(pTidList)) {
		return false;
	}
	if (!SetMakeup()) {
		return false;
	}
	return true;
}

// partsinfo.cpp:1074
bool CPetFrame::ResetFace(CPartTidList *pTidList) {
	IFF_STRUCT::sChar *pChar = ItemManager()->FindChar(m_tidChar);
	if (pChar == 0) {
		return false;
	}

	g_resrcmng->ChangeTexturePart(m_faceTex, pChar->m_faceTexture, 0, 0);

	SetHairClr(pTidList, pTidList->m_hairClr);

	return true;
}

// partsinfo.cpp:1094
void CPetFrame::AddAniPet(char *name) {
	for (std::vector<WPuppet *>::iterator it = m_aniPetList.begin(); it != m_aniPetList.end(); ++it) {
		WPuppet *pPet = *it;
		if (stricmp(pPet->GetPuppetName(), name) == 0) {
			return;
		}
	}

	WPuppet *pPet = g_resrcmng->GetPuppet(name, false, true, false);
	if (pPet == 0) {
		return;
	}

	m_aniPetList.push_back(pPet);
}

// partsinfo.cpp:1116
std::vector<WPuppet *> &CPetFrame::GetAniPetList(void) {
	return m_aniPetList;
}

// partsinfo.cpp:1121
w_motion_data *CPetFrame::FindMotionData(const char *name, bool setCur, WPuppet **ppPet) {
	for (std::vector<WPuppet *>::iterator it = m_aniPetList.begin(); it != m_aniPetList.end(); ++it) {
		w_motion_data *pData = (*it)->FindMotionData(name);
		if (pData != 0) {
			if (ppPet != 0) {
				*ppPet = *it;
			}
			if (setCur) {
				m_pCurAniPet = *it;
			}
			return pData;
		}
	}
	return 0;
}

// partsinfo.cpp:1145
int CPetFrame::GetMotionCountIncludeName(const char *motionName, const char *exceptName) {
	int count = 0;
	for (std::vector<WPuppet *>::iterator it = m_aniPetList.begin(); it != m_aniPetList.end(); ++it) {
		count = (*it)->GetMotionCountIncludeName(motionName, exceptName);
		if (count > 0) {
			return count;
		}
	}
	return count;
}

// partsinfo.cpp:1166
WPuppet *CPetFrame::GetCurAniPet(void) {
	return m_pCurAniPet;
}

// partsinfo.cpp:1171
void CPetFrame::ApplyBones(WBoneSet *pBoneSet, const WMatrix &mat) {
	m_pBonePet->ApplyBones(pBoneSet, mat);
}

// partsinfo.cpp:1176
void CPetFrame::Transform(WView *pView, float time, int flag) {
	m_pBonePet->Transform(pView, time, flag);
}

// partsinfo.cpp:1181
void CPetFrame::UpdateBound(bool b) {
	m_pBonePet->UpdateBound(b, false);
}

// partsinfo.cpp:1186
void CPetFrame::UpdateLightSource(LightSet *pLightSet, bool b, WScene *pScene) {
	m_pBonePet->UpdateLightSource(pLightSet, b, pScene);
}

// partsinfo.cpp:1191
void CPetFrame::SetAlpha(unsigned char alpha) {
	m_pBonePet->SetAlpha(alpha);
}

// partsinfo.cpp:1196
void CPetFrame::SetAlpha(char *name, unsigned char alpha) {
	m_pBonePet->SetAlpha(name, alpha);
}

// partsinfo.cpp:1201
void CPetFrame::Render(WView *pView, bool b, float f, bool b1, bool b2, bool b3) {
	m_pBonePet->Render(pView, b, f, b1, b2, b3);
}

// partsinfo.cpp:1206
bool CPetFrame::HasMPet(char *name) {
	for (int i = 0; i < m_numPets; i++) {
		if (m_pets[i] != 0 && stricmp(m_pets[i]->GetPuppetName(), name) == 0) {
			return true;
		}
	}
	return false;
}

// partsinfo.cpp:1219
void CPetFrame::SetInvisible(bool invisible) {
	if (m_pBonePet != 0) {
		m_pBonePet->GetRootBone()->SetInvisible(true, invisible);
		if (!invisible) {
			ApplyHideMask(true);
		}
	}
}

// partsinfo.cpp:1230
unsigned long CPetFrame::SetPart(CPartTidList *pTidList, unsigned long tid, unsigned long id) {
	ApplyHideMask(false);
	unsigned long mask = m_partList.SetPart(pTidList, tid, id);
	if (mask != 0) {
		SetPartPets(mask);
		SetHairClr(0, pTidList->m_hairClr);
		SetShirtsClr(0, pTidList->m_shirtsClr);
		m_unk1B4 = true; m_pBonePet->EndMeshBoneWork(); ApplyHideMask(true);
		return mask;
	}
	return 0;
}

// partsinfo.cpp:1251
bool CPetFrame::SetAuxPart(unsigned char index, unsigned long tid) {
	if (index >= 5) {
		return false;
	}
	m_auxParts[index] = tid;
	return true;
}

// partsinfo.cpp:1262
bool CPetFrame::FindAuxPart(unsigned long tid) {
	for (int i = 0; i < 5; i++) {
		if (m_auxParts[i] == tid) {
			return true;
		}
	}
	return false;
}

namespace FIXHAIR {

// A hair style a character keeps whatever hair part it wears.
struct sFixItem {
	int m_charType;          // 0x0
	unsigned long m_tid;     // 0x4, the hair part's type id
	std::string m_texture;   // 0x8
};

// partsinfo.cpp:1315
sFixItem exTable[] = {
	{ 0, 0x8006031, "m_hair_02-01.jpg" },
	{ 0, 0x8006032, "m_hair_02-01.jpg" },
	{ 5, 0x8168009, "2#e_hair_07-07.jpg" },
};

} // namespace FIXHAIR

// partsinfo.cpp:1323
int IsFixedHairStyle(unsigned long tid) {
	for (int i = 0; i < 3; i++) {
		if (FIXHAIR::exTable[i].m_tid == tid) {
			return i;
		}
	}
	return -1;
}

// The wig colour slot of each character type (the colour field, bits 13-17,
// of the part type id that carries the hair colour).
int g_iIndexWigColor[] = {
	16, 16, 16, 16, 16, 20, 14, 16, 16, 16
};

// partsinfo.cpp:1341
bool CPetFrame::SetHairClr(CPartTidList *pTidList, unsigned char clr) {
	char texName[0x28] = { 0 };
	bool bFixed = false;
	char fixName[0x32] = { 0 };

	if (pTidList != 0) {
		for (int i = 0; i < pTidList->m_numTotalParts; i++) {
			int fix = IsFixedHairStyle(pTidList->m_partTids[i]);
			if (fix != -1) {
				bFixed = true;
				strcpy(fixName, FIXHAIR::exTable[fix].m_texture.c_str());
			}
			if (((pTidList->m_partTids[i] >> 13) & 0x1F) == g_iIndexWigColor[pTidList->m_tidChar & 0x3FFFFFF]) {
				IFF_STRUCT::sPart *pPart = ItemManager()->FindPart(pTidList->m_partTids[i]);
				if (pPart == 0) {
					break;
				}
				int hOldTex = g_resrcmng->LoadTexture(pPart->texture2[0], 0, 0, 0);
				if (bFixed && fixName[0] != 0) {
					strncpy(texName, fixName, 0x27);
				} else {
					strcpy(texName, pPart->texture2[0]);
				}
				char *p = strrchr(texName, '.');
				if (p == 0) {
					return false;
				}
				p -= 2;
				int n = atoi(p);
				if (bFixed) {
					sprintf(p, "%02d.jpg", n);
				} else {
					sprintf(p, "%02d.jpg", clr + n);
				}
				int hNewTex = g_resrcmng->LoadTexture(texName, 0, 0, 0);
				m_pBonePet->ChangeTextureByHandle(hNewTex, hOldTex, 0);
				break;
			}
		}
		if (m_tidChar != pTidList->m_tidChar) {
			return false;
		}
	}

	IFF_STRUCT::sChar *pChar = ItemManager()->FindChar(m_tidChar);
	int hOldTex;
	if (bFixed == true && fixName[0] != 0) {
		hOldTex = g_resrcmng->LoadTexture(fixName, 0, 0, 0);
	} else {
		hOldTex = g_resrcmng->LoadTexture(pChar->m_hairTexture, 0, 0, 0);
	}
	strcpy(texName, pChar->m_hairTexture);
	if (bFixed == true && fixName[0] != 0) {
		strncpy(texName, fixName, 0x27);
	}
	char *p = strrchr(texName, '.');
	if (p == 0) {
		return false;
	}
	p -= 2;
	int n = atoi(p);
	if (bFixed == true) {
		sprintf(p, "%02d.jpg", n);
	} else {
		sprintf(p, "%02d.jpg", clr + n);
	}
	int hNewTex = g_resrcmng->LoadTexture(texName, 0, 0, 0);
	m_pBonePet->ChangeTextureByHandle(hNewTex, hOldTex, 0);
	if (g_resrcmng != 0 && hOldTex != 0) {
		g_resrcmng->Release(hOldTex);
	}
	if (m_hairTex != 0) {
		g_resrcmng->Release(m_hairTex);
	}
	m_hairTex = hNewTex;

	if ((pChar->m_typeId & 0x3FFFFFF) > 2) {
		char baseName[0x28];
		char hairName[0x28];
		char *faceName = pChar->m_faceTexture;
		strcpy(baseName, faceName);
		char *pExt = strrchr(baseName, '.');
		*pExt = 0;
		sprintf(hairName, "%s_hr%02d.tga", baseName, clr + 1);

		Bitmap *pFaceBmp = g_resrcmng->LoadBitmap(faceName, 0, false);
		Bitmap *pHairBmp = g_resrcmng->LoadBitmap(hairName, 0, false);
		if (pFaceBmp != 0 && pHairBmp != 0) {
			Bitmap *pBmp = g_resrcmng->Combine(pFaceBmp, pHairBmp);
			tagRECT rc;
			rc.left = 0;
			rc.top = 0;
			rc.right = pBmp->Width();
			rc.bottom = pBmp->Height();
			g_resrcmng->ChangeTexturePart(m_faceTex, pBmp, rc);
			delete pFaceBmp;
			delete pHairBmp;
			delete pBmp;
		} else {
			if (pFaceBmp != 0) {
				delete pFaceBmp;
			}
			if (pHairBmp != 0) {
				delete pHairBmp;
			}
		}

		w_texlist *pTex = g_resrcmng->FindTexture(faceName);
		if (pTex != 0) {
			m_pBonePet->ChangeTextureByHandle(m_faceTex, pTex->texhandle, 0);
		}
	}
	return true;
}

// partsinfo.cpp:1534
bool CPetFrame::SetShirtsClr(CPartTidList *pTidList, unsigned char clr) {
	if (!m_unk1B4) {
		return true;
	}
	if (pTidList != 0) {
		if (m_tidChar != pTidList->m_tidChar) {
			return false;
		}
		if (!m_partList.SetShirtsClr(pTidList, clr)) {
			return false;
		}
	}

	IFF_STRUCT::sChar *pChar = ItemManager()->FindChar(m_tidChar);
	char *texName = pChar->m_baseTexture;
	if (texName[0] == 0) {
		return false;
	}
	int hOldTex = g_resrcmng->LoadTexture(texName, 0, 0, 0);
	char *p = strrchr(texName, '.');
	if (p == 0) {
		return false;
	}
	p--;
	*p = clr + '1';
	int hNewTex = g_resrcmng->LoadTexture(texName, 0, 0, 0);
	m_pBonePet->ChangeTextureByHandle(hNewTex, hOldTex, 0);
	*p = '1';
	if (g_resrcmng != 0 && hOldTex != 0) {
		g_resrcmng->Release(hOldTex);
	}
	if (m_shirtsTex != 0) {
		g_resrcmng->Release(m_shirtsTex);
	}
	m_shirtsTex = hNewTex;
	return true;
}

// partsinfo.cpp:1602
bool CPetFrame::SetMakeup(void) {
	if (m_hasMakeup) {
		if (!m_pBonePet->ChangeTexturePart(m_faceTex, GetMakeupTexture(), 0, 0)) {
			return false;
		}
	}
	return true;
}

// partsinfo.cpp:1615
void CPetFrame::CalcBVolume(void) {
	if (m_pBonePet == 0) {
		return;
	}
	std::vector<WPuppet *>::iterator it = m_aniPetList.begin();
	if (it == m_aniPetList.end()) {
		m_pBonePet->CalcBound(0, true);
		return;
	}
	m_pBonePet->CalcBound(true, *it, true, 0, 0);
	for (++it; it != m_aniPetList.end(); ++it) {
		m_pBonePet->CalcBound(false, *it, true, 0, 0);
	}
}

// partsinfo.cpp:1636
void CPetFrame::EndSetPart(void) {
	m_pBonePet->EndMeshBoneWork();
	ApplyHideMask(true);
}

// partsinfo.cpp:1643
void CPetFrame::ApplyHideMask(bool b) {
	char szBoneName[64]; // unused; cl drops it but keeps the original's /GS cookie
	for (int i = 0; i < m_numPets; i++) {
		if (m_partList.m_parts[i].m_tid == 0 || m_partList.m_parts[i].m_pIff == 0) {
			continue;
		}
		IFF_STRUCT::sPart *pIff = m_partList.m_parts[i].m_pIff;
		if (pIff->meshHideMask == 0) {
			continue;
		}
		for (int j = 0; j < m_numPets; j++) {
			if ((pIff->meshHideMask & (1 << j)) == 0 || i == j) {
				continue;
			}
			if (m_partList.m_parts[j].m_tid == 0 || m_partList.m_parts[j].m_pIff == 0 || m_pets[j] == 0) {
				continue;
			}
			WBone *pMesh = m_pets[j]->FindMeshBone(0);
			while (pMesh != 0) {
				WBone *pBone = m_pBonePet->FindBone(pMesh->GetBoneName());
				if (pBone != 0) {
					pBone->SetInvisible(false, b);
				}
				pMesh = m_pets[j]->FindMeshBone(pMesh);
			}
		}
	}
}

// partsinfo.cpp:1703
bool CPetFrame::ChangeTexturePart(char *animName, char *texName, Bitmap **ppBmp, tagRECT *pRc) {
	w_face_animation *pAnim = FindFaceAnim(animName);
	if (pAnim == 0) {
		return false;
	}
	return g_resrcmng->ChangeTexturePart(pAnim->filename, texName, ppBmp, pRc);
}

// partsinfo.cpp:1729
void CPetFrame::ChangeTexturePart(char *animName, Bitmap *pBmp, const tagRECT &rc) {
	w_face_animation *pAnim = FindFaceAnim(animName);
	if (pAnim != 0) {
		g_resrcmng->ChangeTexturePart(pAnim->filename, pBmp, rc);
	}
}

// partsinfo.cpp:1743
void CPetFrame::ChangeTexture(char *srcName, char *dstName) {
	int hSrc = g_resrcmng->LoadTexture(srcName, 0, 0, 0);
	int hDst = g_resrcmng->LoadTexture(dstName, 0, 0, 0);
	if (hSrc != 0 && hDst != 0) {
		m_pBonePet->ChangeTextureByHandle(hDst, hSrc, 0);
	}
}

// partsinfo.cpp:1766
w_face_animation *CPetFrame::FindFaceAnim(char *animName) {
	w_face_animation *pAnim = 0;
	for (int i = 0; i < m_numPets; i++) {
		if (m_pets[i] != 0) {
			pAnim = m_pets[i]->FindFaceAnim(animName);
			if (pAnim != 0) {
				break;
			}
		}
	}
	return pAnim;
}
