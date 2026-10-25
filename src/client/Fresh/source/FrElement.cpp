// Original: d:\build\custom\temp\code\client\fresh\source\frelement.cpp
#include <locale.h>
#include <stdio.h>
#include <string.h>

#include "frelement.h"
#include "bitmap.h"
#include "westpak.h"
#include "wresrcmng.h"
#include "client/ProjectG/commonutil.h"
#include "client/ProjectG/tinyxml.h"
#ifdef JP_PAKS
// JP 0xb4eb80 (src/jp/RFTextManager.h, not on the Fresh units'
// include path): the JP game's text for a Korean source text.
const char* Jp_LookupText(const char* key);
#endif


// <windows.h> maps GetForm to GetFormA; FrElementDoc::GetForm is declared before it.
#ifdef GetForm
#undef GetForm
#endif


void __cdecl CloseCFile(cFile* file);

// frelement.cpp:21
FrElementTID ElementType;
// frelement.cpp:22
FrGuiTID GuiType;

// frelement.cpp:25
FrElementTID::FrElementTID(void) {
	m_map[ED_NONE] = "NONE";
	m_map[ED_FRAME] = "FRAME";
	m_map[ED_LAYOUT] = "LAYOUT";
	m_map[ED_FORM] = "FORM";
	m_map[ED_BITMAP] = "BITMAP";
	m_map[ED_ICON] = "ICON";
	m_map[ED_INCLUDE] = "INCLUDE";
	m_map[ED_MACROITEM] = "MACROITEM";
}

// frelement.cpp:41
FrGuiTID::FrGuiTID(void) {
	m_map[GI_NONE] = "NONE";
	m_map[GI_FORM] = "FORM";
	m_map[GI_STATIC] = "STATIC";
	m_map[GI_TEXTBUTTON] = "TEXTBUTTON";
	m_map[GI_EDIT] = "EDIT";
	m_map[GI_COMBOBOX] = "COMBOBOX";
	m_map[GI_COMBOCTLEX] = "COMBOCTLEX";
	m_map[GI_BUTTON] = "BUTTON";
	m_map[GI_FRAME] = "FRAME";
	m_map[GI_RESOURCE] = "RESOURCE";
	m_map[GI_BITMAP] = "BITMAP";
	m_map[GI_AREA] = "AREA";
	m_map[GI_LISTBOX] = "LISTBOX";
	m_map[GI_GAUGEBAR] = "GAUGEBAR";
	m_map[GI_GAUGEBAREX] = "GAUGEBAREX";
	m_map[GI_GAUGEBARIMAGE] = "GAUGEBARIMAGE";
	m_map[GI_VIEWER] = "VIEWER";
	m_map[GI_CONTEXTMENU] = "CONTEXTMENU";
	m_map[GI_TABBUTTON] = "TABBUTTON";
	m_map[GI_GROUPBOX] = "GROUPBOX";
	m_map[GI_MACROITEM] = "MACROITEM";
}

// frelement.cpp:70
static void CreateGuiItem(const TiXmlNode* pSrc, std::list<FrGuiItem*>& guiList) {
	TiXmlNode* pNode = pSrc->FirstChild("item");
	while (pNode != 0) {
		FrGuiItem* pItem;
		if (pNode->FirstChild("item") != 0) {
			pItem = new FrGuiItemNested;
		} else {
			pItem = new FrGuiItem;
		}

		if (pItem != 0) {
			pItem->Init(pNode);
			guiList.push_back(pItem);
		}

		pNode = pNode->NextSibling("item");
	}
}

// frelement.cpp:121
void FrGuiItem::Init(const TiXmlNode* pSrc) {
	TiXmlElement* pElement = pSrc->ToElement();

	std::string aType = pElement->Attribute("type");
	std::string aRect = pElement->Attribute("rect");
	std::string aPos = pElement->Attribute("pos");

	memset(&m_rect, 0, sizeof(m_rect));
	if (aPos.length() == 0) {
		int v[4];
		sscanf(aRect.c_str(), "%d %d %d %d", &v[0], &v[1], &v[2], &v[3]);

		m_rect.left = v[0];
		m_rect.top = v[1];
		m_rect.right = v[2];
		m_rect.bottom = v[3];
	} else {
		int v[2];
		sscanf(aPos.c_str(), "%d %d", &v[0], &v[1]);

		m_rect.left = v[0];
		m_rect.top = v[1];
		m_rect.right = v[0] + 10;
		m_rect.bottom = v[1] + 10;
	}

	m_type = GuiType[aType];
	m_resource = pElement->Attribute("resource");
	m_name = pElement->Attribute("name");
	m_caption = pElement->Attribute("caption");
#ifdef JP_PAKS
	// JP 0xc1775d: a non-empty caption through the JP text table
	if (m_caption != "")
		m_caption = Jp_LookupText(m_caption.c_str());
#endif

	pElement->Attribute("flag", (int*)&m_flag);

	TiXmlNode* pParam = pSrc->FirstChild("param");
	while (pParam != 0) {
		TiXmlElement* pParamElement = pParam->ToElement();
#ifdef JP_PAKS
		// The JP data has stray text after a <param> (events.xml), a text
		// node this loop would take for an element (docs/JP_CLIENT.md).
		if (pParamElement == 0) {
			pParam = pParam->NextSibling();
			continue;
		}
#endif

		std::string aName = pParamElement->Attribute("name");
		std::string aVar = pParamElement->Attribute("var");
		if (aName.length() != 0 && aVar.length() != 0) {
			m_param[aName] = aVar;
		}
		pParam = pParam->NextSibling();
	}
}

// frelement.cpp:192
const std::list<FrGuiItem*>* FrGuiItem::GetChildList(void) {
	return 0;
}

// frelement.cpp:197
FrGuiItemNested::~FrGuiItemNested(void) {
	sequence_delete(m_childList.begin(), m_childList.end());
	m_childList.clear();
}

// frelement.cpp:203
void FrGuiItemNested::Init(const TiXmlNode* pSrc) {
	FrGuiItem::Init(pSrc);

	CreateGuiItem(pSrc, m_childList);
}

// frelement.cpp:210
const std::list<FrGuiItem*>* FrGuiItemNested::GetChildList(void) {
	return &m_childList;
}

// frelement.cpp:261
void FrElementFrame::Init(const TiXmlNode* pSrc) {
	std::string color = pSrc->ToElement()->Attribute("color");
	sscanf(color.c_str(), "%x", &m_color);

	TiXmlNode* pLayer = pSrc->FirstChild("layer");
	m_layers = 0;
	while (pLayer != 0 && m_layers < sizeof(m_aType) / sizeof(m_aType[0])) {
		m_aInvisible[m_layers] = 0;
		TiXmlElement* pElement = pLayer->ToElement();
		if (pElement != 0) {
			pElement->Attribute("type", &m_aType[m_layers]);
			pElement->Attribute("height", &m_aHeight[m_layers]);
			pElement->Attribute("pos", &m_aPos[m_layers]);
			pElement->Attribute("invisible", &m_aInvisible[m_layers]);

			int bottomPos;
			pElement->Attribute("bottompos", &bottomPos);
			if (bottomPos != 0) {
				m_aPos[m_layers] = -bottomPos;
			}
		}

		pLayer = pLayer->NextSibling("layer");
		m_layers++;
	}

	TiXmlNode* pNode = pSrc->FirstChild("bfrm");
	if (pNode != 0) {
		TiXmlElement* pElement = pNode->ToElement();
		if (pElement != 0) {
			m_bfrmName = pElement->Attribute("filename");
		}
	}

	pNode = pSrc->FirstChild("sfrm");
	if (pNode != 0) {
		TiXmlElement* pElement = pNode->ToElement();
		if (pElement != 0) {
			m_sfrmName = pElement->Attribute("filename");
		}
	}

	pNode = pSrc->FirstChild("cfrm");
	if (pNode != 0) {
		TiXmlElement* pElement = pNode->ToElement();
		if (pElement != 0) {
			m_cfrmName = pElement->Attribute("filename");
		}
	}
}

// frelement.cpp:316
void FrElementLayout::Init(const TiXmlNode* pSrc) {
	TiXmlNode* pBase = pSrc->FirstChild("base");
	if (pBase != 0) {
		TiXmlElement* pElement = pBase->ToElement();
		pElement->Attribute("width", &m_bgSize.cx);
		pElement->Attribute("height", &m_bgSize.cy);

		char buff[256];
		sprintf(buff, "0x%s", pElement->Attribute("color").c_str());
		sscanf(buff, "%X", &m_bgColor);

		m_bgFilename = pElement->Attribute("background");

		CreateGuiItem(pSrc, m_guiList);

		TiXmlNode* pAniBg = pSrc->FirstChild("anibg");
		if (pAniBg != 0) {
			pElement = pAniBg->ToElement();

			sprintf(buff, "%s", pElement->Attribute("rect").c_str());
			int v[4];
			sscanf(buff, "%d %d %d %d", &v[0], &v[1], &v[2], &v[3]);

			m_aniBg.m_rect.left = v[0];
			m_aniBg.m_rect.top = v[1];
			m_aniBg.m_rect.right = v[2];
			m_aniBg.m_rect.bottom = v[3];

			m_aniBg.m_param["vel"] = pElement->Attribute("vel");
			m_aniBg.m_param["img"] = pElement->Attribute("img");
		}
	}
}

// frelement.cpp:367
void FrElementForm::Init(const TiXmlNode* pSrc) {
	m_caption = pSrc->ToElement()->Attribute("caption");
	m_desc = pSrc->ToElement()->Attribute("description");
#ifdef JP_PAKS
	// JP 0xc17d9c: a non-empty description through the JP text table
	if (m_desc != "")
		m_desc = Jp_LookupText(m_desc.c_str());
#endif

	std::string bgSize = pSrc->ToElement()->Attribute("size");
	sscanf(bgSize.c_str(), "%d %d", &m_bgSize.w, &m_bgSize.h);
	m_bgFrame.m_resource = pSrc->ToElement()->Attribute("resource");

	short w = m_bgSize.w;
	short h = m_bgSize.h;
	m_bgFrame.m_rect.left = 0;
	m_bgFrame.m_rect.top = 0;
	m_bgFrame.m_rect.right = w;
	m_bgFrame.m_rect.bottom = h;
	m_bgFrame.m_type = GI_FRAME;
	m_bgFrame.m_flag = 1;

	CreateGuiItem(pSrc, m_guiList);
}

// frelement.cpp:397
void FrElementBitmap::Init(const TiXmlNode* pSrc) {
	m_bitmapFilename = pSrc->ToElement()->Attribute("filename");
}

// frelement.cpp:403
void FrElementIcon::Init(const TiXmlNode* pSrc) {
}

// frelement.cpp:407
void FrElementMacroItem::Init(const TiXmlNode* pSrc) {
	CreateGuiItem(pSrc, m_guiList);
}

// frelement.cpp:465
const Bitmap* FrElementDoc::GetBitmap(const std::string& resID) {
	if (resID.length() == 0) {
		return 0;
	}

	if (m_mapBitmap.find(resID) == m_mapBitmap.end()) {
		LoadPic(resID.c_str());

		if (m_mapBitmap.find(resID) == m_mapBitmap.end()) {
			return 0;
		}
	}

	return m_mapBitmap[resID].bitmap;
}

// frelement.cpp:483
void FrElementDoc::ClearBitmaps(void) {
	for (std::map<std::string, resBitmap_t>::iterator it = m_mapBitmap.begin(); it != m_mapBitmap.end(); ++it) {
		delete it->second.bitmap;
	}
	m_mapBitmap.clear();
}

// frelement.cpp:529
int FrElementDoc::LoadPic(const char* fileName) {
	enum { PIC_BMP, PIC_TGA };

	int picType = PIC_TGA;
	cFile* pFile = g_resrcmng->GetCFile(MakeStr("%s.tga", fileName), 0xFFFF);
	if (pFile == 0) {
		pFile = g_resrcmng->GetCFile(MakeStr("%s.bmp", fileName), 0xFFFF);
		if (pFile == 0) {
			return 0;
		}
		picType = PIC_BMP;
	}

	int fileSize = pFile->Length();
	char* pBuffer = new char[fileSize + 1];
	pFile->Read(pBuffer, fileSize);
	pBuffer[fileSize] = 0;
	CloseCFile(pFile);

	Bitmap* pBitmap;
	char* pBits;
	unsigned int height;
	int srcPitch;
	if (picType == PIC_BMP) {
		// BITMAPFILEHEADER + BITMAPINFOHEADER, bottom-up rows.
		pBits = pBuffer + 0x36;
		pBitmap = new Bitmap(*(int*)(pBuffer + 0x12), *(int*)(pBuffer + 0x16), *(short*)(pBuffer + 0x1c));

		height = *(int*)(pBuffer + 0x16);

		srcPitch = pBitmap->pitch;
	} else if (picType == PIC_TGA) {
		// TGA header, bottom-up rows.
		pBits = pBuffer + 0x12;
		pBitmap = new Bitmap(*(short*)(pBuffer + 0xc), *(short*)(pBuffer + 0xe), *(unsigned char*)(pBuffer + 0x10));

		height = *(short*)(pBuffer + 0xe);
		srcPitch = *(unsigned char*)(pBuffer + 0x10) * *(short*)(pBuffer + 0xc) / 8;
	} else {
		delete[] pBuffer;
		return 0;
	}

	for (unsigned int y = 0; y < height; y++) {
		memcpy(pBitmap->GetVram(y), pBits + (height - 1 - y) * srcPitch, srcPitch);
	}

	resBitmap_t res;
	res.bitmap = pBitmap;
	res.name = fileName;
	res.size = fileSize;

	m_mapBitmap[fileName] = res;

	delete[] pBuffer;

	return 1;
}

// frelement.cpp:618
template <class T>
struct sameElement {
	sameElement(enumElement type, const std::string& resID) : m_type(type), m_resID(resID) {}
	bool operator()(T pElement) {
		return pElement->m_type == m_type && pElement->m_name == m_resID;
	}

	const std::string& m_resID;                          // 0x0
	const enumElement m_type;                            // 0x4
};

// frelement.cpp:644
bool FrElementDoc::Load(const char* fileName) {
	std::list<FrElement*> elementList;

	ClearBitmaps();

	setlocale(LC_CTYPE, "Kor");

	m_xmlFileName = fileName;
	LoadXml(fileName, elementList);

	// Replace the previous document's elements.
	sequence_delete(m_elementList.begin(), m_elementList.end());
	m_elementList.clear();

	for (std::list<FrElement*>::iterator it = elementList.begin(); it != elementList.end(); ++it) {
		m_elementList.push_back(*it);
	}

	return true;
}

#ifdef JP_PAKS
// The JP data (2017) made the 2011 full-screen layouts forms of the same
// controls; the KR code still opens them as layouts (docs/JP_CLIENT.md).
static const char* JpLayoutForForm(const std::string& form) {
	static const char* const kMap[][2] = {
		{ "roomlist", "ROOMLIST" },
		{ "gameroom_main", "GAMEROOM" },
		{ "shopmain", "SHOPMAIN" },
		{ "tutorialmain", "TUTORIAL_MAIN" },
		{ "realmyroom", "REALMYROOM" },
	};
	for (unsigned int i = 0; i < sizeof(kMap) / sizeof(kMap[0]); i++) {
		if (stricmp(form.c_str(), kMap[i][0]) == 0) {
			return kMap[i][1];
		}
	}
	return 0;
}

// The JP lobby, the chat room and the forms above no longer place the
// under/over bars (the 2017 client draws them itself); the KR layouts put
// these three macros at 0,0. The JP data still has the macros. Added when the layout lacks them
// (the KR code keeps their controls; CLobbyMain hides the over bar where the
// JP form draws its own title row).
// gameroom_main holds a group per room kind: group_stroke (the KR GAMEROOM's
// controls), group_30s, group_gp and group_result (30-player, Grand Prix and
// the results panel), which the JP exe shows one at a time. The KR code knows
// one room: the other groups covered the start button, and their "user" lists
// took the room's user list handler.
static void JpDropGameRoomGroups(std::list<FrGuiItem*>& guiList) {
	for (std::list<FrGuiItem*>::iterator it = guiList.begin(); it != guiList.end();) {
		if ((*it)->m_name == "group_30s" || (*it)->m_name == "group_gp" || (*it)->m_name == "group_result") {
			delete *it;
			it = guiList.erase(it);
		} else {
			++it;
		}
	}
}

// Where the JP data moved or dropped a control the KR code binds, the
// elements are reshaped on load into what the KR message maps address. Rows
// name the element: a JP form registered as a KR layout (by the layout's
// name) or a macro. Check with the port's JP control check
// (docs/JP_CLIENT.md, "KR controls the JP data lacks").
//
// Renamed groups: the KR code reaches the controls inside through the KR
// group name (SHOPMAIN.st.epower). The JP stat box is the macro stat_default
// (one group, stat_group), shared by the shop and My Room.
static const char* const kJpRenames[][3] = {
	// element, JP name, KR name
	{ "stat_default", "stat_group", "st" },
};

// Items moved into a group: the KR My Room has its stat box and the
// upgrade/downgrade buttons inside middle_ui (REALMYROOM.middle_ui.st...,
// .middle_ui.upgrade); the JP form places their macros at the top. The JP
// group around the buttons (updown_group) is then dropped by FrCmdTarget's
// group reroute.
static const char* const kJpMoves[][3] = {
	// element, item, group
	{ "REALMYROOM", "st_com", "middle_ui" },
	{ "REALMYROOM", "up_com", "middle_ui" },
};

// Controls the KR code binds in an FRCMD_INIT handler and then uses without a
// NULL check, which the JP data dropped (its stat box shows the values only:
// no gauges, no selected-item box). Each gets an empty control of the KR
// type, zero-sized so it never draws or takes a click.
static const char* const kJpPlaceholders[][3] = {
	// element, path (group.name), KR type
	{ "stat_default", "st.selected_name", "STATIC" },
	{ "stat_default", "st.selframe", "AREA" },
	{ "stat_default", "st.selitemicon", "AREA" },
	{ "stat_default", "st.selitem", "BUTTON" },
	{ "stat_default", "st.selitem_prev", "BUTTON" },
	{ "stat_default", "st.selitem_next", "BUTTON" },
	{ "stat_default", "st.stat_frame", "AREA" },
	{ "stat_default", "st.stat_text", "AREA" },
	{ "stat_default", "st.stat_part", "VIEWER" },
	{ "stat_default", "st.stat_all", "VIEWER" },
	{ "stat_default", "st.powerbar", "GAUGEBAR" },
	{ "stat_default", "st.controlbar", "GAUGEBAR" },
	{ "stat_default", "st.impactbar", "GAUGEBAR" },
	{ "stat_default", "st.spinbar", "GAUGEBAR" },
	{ "stat_default", "st.curvebar", "GAUGEBAR" },
	// The bars' buttons the JP client dropped (item storage, scratch, the
	// messenger and gift alarms).
	{ "under_bar_m", "ubar.itemstorage", "BUTTON" },
	{ "under_bar_black_m", "ubar.itemstorage", "BUTTON" },
	{ "under_bar_black_m", "ubar.tabbtn_b", "BUTTON" },
	{ "under_tab_m", "utab.scratch", "BUTTON" },
	{ "under_tab_black_m", "utab.scratch", "BUTTON" },
	{ "over_bar_m", "obar.messenger_alarm", "BUTTON" },
	{ "over_bar_m", "obar.gift_alarm", "BUTTON" },
	// My Room: the KR furniture mode and right-click menu (the JP form has
	// its own furniture controls, ftab_main and fitem_list).
	{ "REALMYROOM", "middle_ui.furniture_arrange", "BUTTON" },
	{ "REALMYROOM", "middle_ui.chatbg", "AREA" },
	{ "REALMYROOM", "rmenu", "CONTEXTMENU" },
	{ "REALMYROOM", "farrorup", "BUTTON" },
	{ "REALMYROOM", "farrordown", "BUTTON" },
	{ "REALMYROOM", "flist_num", "EDIT" },
	{ "REALMYROOM", "flist_maxnum", "EDIT" },
	{ "REALMYROOM", "Btn_flistnum", "BUTTON" },
	{ "REALMYROOM", "freset", "BUTTON" },
	{ "REALMYROOM", "fback", "BUTTON" },
	{ "REALMYROOM", "fsave", "BUTTON" },
	{ "REALMYROOM", "fout", "BUTTON" },
	{ "REALMYROOM", "ftab_background01", "AREA" },
	{ "REALMYROOM", "ftab_background02", "AREA" },
	{ "REALMYROOM", "ftab_background03", "AREA" },
	{ "REALMYROOM", "ftab_background04", "AREA" },
	{ "REALMYROOM", "ftab_background05", "AREA" },
	{ "REALMYROOM", "ftab_background06", "AREA" },
	{ "REALMYROOM", "ftab_background07", "AREA" },
	{ "REALMYROOM", "ftab_background08", "AREA" },
	{ "REALMYROOM", "ftab_scrollimg01", "AREA" },
	{ "REALMYROOM", "ftab_scrollimg02", "AREA" },
	{ "REALMYROOM", "ftab_scrollimg03", "AREA" },
	{ "REALMYROOM", "Btn_Menu", "BUTTON" },
	{ "REALMYROOM", "Btn_MenuList", "LISTBOX" },
};

static FrGuiItem* JpFindItem(std::list<FrGuiItem*>& guiList, const std::string& name) {
	for (std::list<FrGuiItem*>::iterator it = guiList.begin(); it != guiList.end(); ++it) {
		if ((*it)->m_name == name) {
			return *it;
		}
	}
	return 0;
}

// The child list of the group at a dotted path ("" = guiList itself).
static std::list<FrGuiItem*>* JpGroupList(std::list<FrGuiItem*>& guiList, const std::string& path) {
	std::list<FrGuiItem*>* pList = &guiList;
	std::string::size_type start = 0;
	while (pList != 0 && start < path.length()) {
		std::string::size_type dot = path.find('.', start);
		if (dot == std::string::npos) {
			dot = path.length();
		}
		FrGuiItem* pGroup = JpFindItem(*pList, path.substr(start, dot - start));
		pList = pGroup != 0 ? const_cast<std::list<FrGuiItem*>*>(pGroup->GetChildList()) : 0;
		start = dot + 1;
	}
	return pList;
}

static void JpReshape(const std::string& element, std::list<FrGuiItem*>& guiList) {
	unsigned int i;
	for (i = 0; i < sizeof(kJpRenames) / sizeof(kJpRenames[0]); i++) {
		if (stricmp(element.c_str(), kJpRenames[i][0]) == 0) {
			FrGuiItem* pItem = JpFindItem(guiList, kJpRenames[i][1]);
			if (pItem != 0) {
				pItem->m_name = kJpRenames[i][2];
			}
		}
	}
	for (i = 0; i < sizeof(kJpMoves) / sizeof(kJpMoves[0]); i++) {
		if (stricmp(element.c_str(), kJpMoves[i][0]) == 0) {
			FrGuiItem* pItem = JpFindItem(guiList, kJpMoves[i][1]);
			std::list<FrGuiItem*>* pGroup = JpGroupList(guiList, kJpMoves[i][2]);
			if (pItem != 0 && pGroup != 0 && pGroup != &guiList) {
				guiList.remove(pItem);
				pGroup->push_back(pItem);
			}
		}
	}
	for (i = 0; i < sizeof(kJpPlaceholders) / sizeof(kJpPlaceholders[0]); i++) {
		if (stricmp(element.c_str(), kJpPlaceholders[i][0]) != 0) {
			continue;
		}
		std::string path = kJpPlaceholders[i][1];
		std::string::size_type dot = path.rfind('.');
		std::string name = dot == std::string::npos ? path : path.substr(dot + 1);
		std::list<FrGuiItem*>* pList = JpGroupList(guiList, dot == std::string::npos ? std::string() : path.substr(0, dot));
		if (pList == 0 || JpFindItem(*pList, name) != 0) {
			continue;
		}
		FrGuiItem* pItem = new FrGuiItem;
		pItem->m_type = GuiType[kJpPlaceholders[i][2]];
		pItem->m_name = name;
		if (pItem->m_type == GI_CONTEXTMENU) {
			pItem->m_resource = "CONTEXTFRM"; // FrContextMenuCtrl::Init needs a frame
		}
		pItem->m_rect.left = 0;
		pItem->m_rect.top = 0;
		pItem->m_rect.right = 0;
		pItem->m_rect.bottom = 0;
		pList->push_back(pItem);
	}
}

static void JpAddBars(std::list<FrGuiItem*>& guiList, bool black) {
	static const char* const kBars[][3] = {
		{ "utab_mcr", "under_tab_m", "under_tab_black_m" },
		{ "ubar_mcr", "under_bar_m", "under_bar_black_m" },
		{ "obar_mcr", "over_bar_m", "over_bar_m" },
	};
	for (unsigned int i = 0; i < sizeof(kBars) / sizeof(kBars[0]); i++) {
		bool found = false;
		for (std::list<FrGuiItem*>::iterator it = guiList.begin(); it != guiList.end(); ++it) {
			if ((*it)->m_name == kBars[i][0]) {
				found = true;
			}
		}
		if (!found) {
			FrGuiItem* pItem = new FrGuiItem;
			pItem->m_type = GI_MACROITEM;
			pItem->m_name = kBars[i][0];
			pItem->m_resource = kBars[i][black ? 2 : 1];
			pItem->m_rect.left = 0;
			pItem->m_rect.top = 0;
			pItem->m_rect.right = 10;
			pItem->m_rect.bottom = 10;
			guiList.push_back(pItem);
		}
	}
}
#endif

// frelement.cpp:672
bool FrElementDoc::LoadXml(const char* pszXmlFilename, std::list<FrElement*>& tempList) {
	TiXmlDocument xml;
	if (!xml.LoadFileEx(pszXmlFilename)) {
		return false;
	}

	TiXmlNode* pNode = xml.FirstChild("resource");
	if (pNode == 0) {
		return false;
	}

	int count;
	pNode->ToElement()->Attribute("count", &count);
	pNode = pNode->FirstChild("element");
	while (pNode != 0) {
		TiXmlElement* pXmlElement = pNode->ToElement();
		if (pXmlElement == 0) {
			pNode = pNode->NextSibling();
			continue;
		}

		std::string name = pXmlElement->Attribute("name");
		std::string type = pXmlElement->Attribute("type");
		enumElement elementType = ElementType[type];

		std::list<FrElement*>::iterator it = m_elementList.end();

		switch (elementType) {
		case ED_INCLUDE:
			if (!LoadXml(name.c_str(), tempList)) {
				return false;
			}
			break;
		case ED_FRAME:
		case ED_LAYOUT:
		case ED_FORM:
		case ED_MACROITEM:
			break;
		default:
			it = std::find_if(m_elementList.begin(), m_elementList.end(), sameElement<FrElement*>(elementType, name));
			break;
		}

		if (it != m_elementList.end()) {
			// Already loaded: keep the existing element.
			tempList.push_back(*it);
			m_elementList.erase(it);
		} else {
			FrElement* pElement = 0;
			switch (elementType) {
			case ED_FRAME:		pElement = new FrElementFrame;		break;
			case ED_LAYOUT:		pElement = new FrElementLayout;		break;
			case ED_BITMAP:		pElement = new FrElementBitmap;		break;
			case ED_FORM:		pElement = new FrElementForm;		break;
			case ED_MACROITEM:	pElement = new FrElementMacroItem;	break;
			}

			if (pElement != 0) {
				pElement->m_name = name;
				pElement->Init(pNode);
				tempList.push_back(pElement);
			}
#ifdef JP_PAKS
			// Also register the form's controls as the layout the KR code opens.
			const char* layoutName = elementType == ED_FORM ? JpLayoutForForm(name) : 0;
			if (layoutName != 0) {
				FrElementLayout* pLayout = new FrElementLayout;
				pLayout->m_name = layoutName;
				std::string size = pXmlElement->Attribute("size");
				sscanf(size.c_str(), "%d %d", &pLayout->m_bgSize.cx, &pLayout->m_bgSize.cy);
				pLayout->m_bgColor = 0xFFFFFF;
				CreateGuiItem(pNode, pLayout->m_guiList);
				if (stricmp(layoutName, "GAMEROOM") == 0) {
					JpDropGameRoomGroups(pLayout->m_guiList);
				}
				JpReshape(layoutName, pLayout->m_guiList);
				JpAddBars(pLayout->m_guiList, stricmp(layoutName, "REALMYROOM") == 0);
				tempList.push_back(pLayout);
			}
			if (elementType == ED_MACROITEM && pElement != 0) {
				JpReshape(name, static_cast<FrElementMacroItem*>(pElement)->m_guiList);
			}
			if (elementType == ED_LAYOUT && (stricmp(name.c_str(), "TOPPAGE") == 0 || stricmp(name.c_str(), "AVATARCHAT_MAIN") == 0)) {
				JpAddBars(static_cast<FrElementLayout*>(pElement)->m_guiList, false);
			}
#endif
		}

		pNode = pNode->NextSibling();
	}

	return true;
}

// frelement.cpp:768
FrElementDoc::~FrElementDoc(void) {
	sequence_delete(m_elementList.begin(), m_elementList.end());
	m_elementList.clear();

	ClearBitmaps();
}

// frelement.cpp:776
FrElementLayout* FrElementDoc::GetLayout(const std::string& resID) {
	std::list<FrElement*>::iterator it;
	it = std::find_if(m_elementList.begin(), m_elementList.end(), sameElement<FrElement*>(ED_LAYOUT, resID));
	if (it != m_elementList.end()) {
		return static_cast<FrElementLayout*>(*it);
	}

	return 0;
}

// frelement.cpp:788
FrElementFrame* FrElementDoc::GetFrame(const std::string& resID) {
	return FindElement<std::list<FrElement*>, FrElementFrame>(m_elementList, ED_FRAME, resID);
}

// frelement.cpp:795
FrElementForm* FrElementDoc::GetForm(const std::string& resID) {
	return FindElement<std::list<FrElement*>, FrElementForm>(m_elementList, ED_FORM, resID);
}

// frelement.cpp:802
FrElementMacroItem* FrElementDoc::GetMacroItem(const std::string& resID) {
	return FindElement<std::list<FrElement*>, FrElementMacroItem>(m_elementList, ED_MACROITEM, resID);
}
