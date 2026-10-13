// Original: d:\build\custom\temp\code\shared\classdefine.h
#ifndef PG_SHARED_CLASSDEFINE_H
#define PG_SHARED_CLASSDEFINE_H

#include <string.h>

#include "defines.h"
#include "padding.h"

// The IFF table records. The sizes are the record strides of the
// CItemManager::Make*Map functions (the copy of each record into its map).
//
// The item records (sChar, sItem, sBall, sCaddie, sCourse, sQuest, sQuestDrop,
// sMascot, ...) begin with the common head CItemManager::FindCommonItem
// returns (IFF_ITEM_COMMON, uccmanager.h, 0x90 bytes): active flag 0x0, type
// id 0x4, name 0x8, level 0x30, icon name 0x31 (GetIconName), price 0x5C.
// Each record repeats the head members it uses under the IFF_ITEM_COMMON
// names; the original relation between the two types is not established.
// The special prize ability kinds (defined in avatar_avatar.h).
enum eSPAVILITYTYPE;

namespace IFF_STRUCT {

// A character record (MakeCharMap: 0x174 bytes).
struct sChar {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	char m_name[0x28];               // 0x8
	unsigned char m_level;           // 0x30
	char m_icon[0x28];               // 0x31, the icon name
	UNKNOWN(0x59, 0x68);
	unsigned char m_flags;           // 0x68, bit 0: a cash (cookie) item (CItemManager::IsCashItem)
	UNKNOWN(0x69, 0x90);
	char m_model[0x28];              // 0x90, the model name: <model>.bpet and <model>.apet (CPetFrame::Build)
	char m_hairTexture[0x28];        // 0xB8, the hair texture (CPetFrame::SetHairClr); length inferred
	char m_baseTexture[0x28];        // 0xE0, loaded and released by CPetFrame::SetPartPets; length inferred
	char m_faceTexture[0x28];        // 0x108, the face texture name (CPetFrame::ResetFace)
	short m_capacity[5];             // 0x130, the capacity of each eLvlType (GetCharCapacity); count inferred
	unsigned char m_numParts;        // 0x13A, the part slots of the character (CPartTidList::SetTids)
	unsigned char m_numExtraParts;   // 0x13B, the slots after them (counted in the total)
	UNKNOWN(0x13C, 0x140);
	float m_cameraScale;             // 0x140, scales the default camera's distance and height (CGolfDoc::MakeDefaultCamera)
	unsigned char m_baseLevels[5];   // 0x144, the base level of each eLvlType (GetCharLevel); count inferred
	char m_awardPose[0x2B];         // 0x149, the pose the award model plays ("1st_place", "2nd_place"; CRankExData::ShowLeader); length: to the record end
};
CHECK_SIZE(sChar, 0x174);
CHECK_FIELD(sChar, m_flags, 0x68);
CHECK_FIELD(sChar, m_awardPose, 0x149);
CHECK_FIELD(sChar, m_active, 0x0);
CHECK_FIELD(sChar, m_typeId, 0x4);
CHECK_FIELD(sChar, m_name, 0x8);
CHECK_FIELD(sChar, m_level, 0x30);
CHECK_FIELD(sChar, m_icon, 0x31);
CHECK_FIELD(sChar, m_model, 0x90);
CHECK_FIELD(sChar, m_hairTexture, 0xB8);
CHECK_FIELD(sChar, m_baseTexture, 0xE0);
CHECK_FIELD(sChar, m_faceTexture, 0x108);
CHECK_FIELD(sChar, m_capacity, 0x130);
CHECK_FIELD(sChar, m_numParts, 0x13A);
CHECK_FIELD(sChar, m_numExtraParts, 0x13B);
CHECK_FIELD(sChar, m_cameraScale, 0x140);
CHECK_FIELD(sChar, m_baseLevels, 0x144);

// An item record of the IFF tables (MakeItemMap: 0xC8 bytes).
struct sItem {
	bool IsRandomBox(void) const;    // classdefine.h:478

	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	char m_name[0x28];               // 0x8
	unsigned char m_level;           // 0x30
	char m_icon[0x28];               // 0x31, the icon name
	UNKNOWN(0x59, 0x68);
	unsigned char m_flags;           // 0x68, bit 0 tested by CItemManager (ItemManager s5 report)
	UNKNOWN(0x69, 0x6A);
	unsigned char m_timeType;        // 0x6A, nonzero: a time-limited item (IsTimeLimit)
	unsigned char m_rentalDays;      // 0x6B, the rental period in days (CItemManager::GetSetItemElem)
	UNKNOWN(0x6C, 0x90);
	unsigned char isRandomBox : 1;   // 0x90, bit 0
	UNKNOWN(0x91, 0x94);
	char m_model[0x28];              // 0x94, the model name (CExhibition::SetModel, item exhibits); length inferred (quantity follows)
	short quantity;                  // 0xBC, the count the fortune list scales the price by
	short m_successRate;             // 0xBE, the success rate in percent (CItemWindow::ProcessItem; 100 without a record)
	UNKNOWN(0xC0, 0xC6);
	short m_tikiPoint;               // 0xC6, the Tiki points one item is worth (CTikiMagicBoxDoc::GetOutput)
};
CHECK_SIZE(sItem, 0xC8);
CHECK_FIELD(sItem, m_rentalDays, 0x6B);
CHECK_FIELD(sItem, m_active, 0x0);
CHECK_FIELD(sItem, m_typeId, 0x4);
CHECK_FIELD(sItem, m_name, 0x8);
CHECK_FIELD(sItem, m_level, 0x30);
CHECK_FIELD(sItem, m_icon, 0x31);
CHECK_FIELD(sItem, m_flags, 0x68);
CHECK_FIELD(sItem, m_timeType, 0x6A);
CHECK_FIELD(sItem, quantity, 0xBC);
CHECK_FIELD(sItem, m_successRate, 0xBE);
CHECK_FIELD(sItem, m_tikiPoint, 0xC6);
CHECK_FIELD(sItem, m_model, 0x94);

// A part record of the IFF tables (clothes, hair, ...; MakePartMap: 0x204
// bytes).
struct sPart {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	char m_name[0x28];               // 0x8
	unsigned char m_level;           // 0x30, low 7 bits: the level the player needs
	char m_icon[0x28];               // 0x31, the icon name
	UNKNOWN(0x59, 0x5C);
	unsigned long m_price;           // 0x5C, the price (sDisplayUccClothes: 1000000)
	UNKNOWN(0x60, 0x68);
	unsigned char m_flags;           // 0x68, bits 1-4 (mask 0x1E) tested by the part users
	unsigned char m_flags1;          // 0x69, bits 0-1 cleared by sDisplayUccClothes
	UNKNOWN(0x6A, 0x6B);
	unsigned char m_rentalDays;      // 0x6B, the rental period in days (CItemManager::GetSetItemElem)
	UNKNOWN(0x6C, 0x90);
	char model[0x28];                // 0x90, the model name (IsSameClothes compares it)
	unsigned char slot;              // 0xB8, the part slot index (the mask bit is 1 << slot)
	UNKNOWN(0xB9, 0xBC);
	unsigned long hideMask;          // 0xBC, the part slots this part hides
	unsigned long meshHideMask;      // 0xC0, the part slots whose meshes this part hides on the bone model (CPetFrame::ApplyHideMask)
	char texture[3][0x28];           // 0xC4, the texture names ([0]: the makeup texture)
	char texture2[3][0x28];          // 0x13C, a second set of 3 texture names (IsSameClothes)
	short m_baseStat[5];             // 0x1B4, the stats of each eLvlType (GetItemLevel adds their low bytes; PrivateTradeDlg SetToolTip)
	short m_slotStat[5];             // 0x1BE, the slot stats (PrivateTradeDlg SetToolTip)
	unsigned char m_wearRestricted;  // 0x1C8, nonzero: CShopMain::SelectItemList explains a CanSetPart failure as cannot-be-worn
	UNKNOWN(0x1C9, 0x1F0);
	unsigned long m_subParts[2];     // 0x1F0, the type ids of the parts worn with this one, 0: none (CPartList::CheckSubParts)
	short m_charCardSlots;           // 0x1F8, the card slots for card kind 0 (typeId bits 22-25; GetCardSlotNum, IsPartAttachCard)
	short m_caddieCardSlots;         // 0x1FA, the card slots for card kind 1 (slots 5.., IsPartAttachCard)
	unsigned long m_rentalPrice;     // 0x1FC, the pang a rental extension costs (FrRentalExtensionDlg)
	unsigned char m_shopMark;        // 0x200, nonzero: the shop draws its mark overlay on the part's slot (CShopMain::DrawPartsSlot)
	UNKNOWN(0x201, 0x202);
	short m_tikiPoint;               // 0x202, the Tiki points one part is worth (CTikiMagicBoxDoc::GetOutput)
};
CHECK_SIZE(sPart, 0x204);
CHECK_FIELD(sPart, m_rentalDays, 0x6B);
CHECK_FIELD(sPart, m_shopMark, 0x200);
CHECK_FIELD(sPart, m_typeId, 0x4);
CHECK_FIELD(sPart, m_level, 0x30);
CHECK_FIELD(sPart, model, 0x90);
CHECK_FIELD(sPart, slot, 0xB8);
CHECK_FIELD(sPart, hideMask, 0xBC);
CHECK_FIELD(sPart, meshHideMask, 0xC0);
CHECK_FIELD(sPart, texture, 0xC4);
CHECK_FIELD(sPart, texture2, 0x13C);
CHECK_FIELD(sPart, m_flags, 0x68);
CHECK_FIELD(sPart, m_charCardSlots, 0x1F8);
CHECK_FIELD(sPart, m_caddieCardSlots, 0x1FA);
CHECK_FIELD(sPart, m_rentalPrice, 0x1FC);
CHECK_FIELD(sPart, m_baseStat, 0x1B4);
CHECK_FIELD(sPart, m_slotStat, 0x1BE);
CHECK_FIELD(sPart, m_tikiPoint, 0x202);
CHECK_FIELD(sPart, m_subParts, 0x1F0);
CHECK_FIELD(sPart, m_wearRestricted, 0x1C8);
CHECK_FIELD(sPart, m_price, 0x5C);
CHECK_FIELD(sPart, m_flags1, 0x69);

// A ball record (MakeBallMap: 0x2FC bytes).
struct sBall {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	char m_name[0x28];               // 0x8
	unsigned char m_level;           // 0x30
	char m_icon[0x28];               // 0x31, the icon name
	UNKNOWN(0x59, 0x5C);
	unsigned long m_price;           // 0x5C, the price (IsBasicAztecFamily: 0 for the basic balls)
	UNKNOWN(0x60, 0x68);
	unsigned char m_flags;           // 0x68, bit 0: a cash (cookie) item (CItemManager::IsCashItem)
	UNKNOWN(0x69, 0x6B);
	unsigned char m_rentalDays;      // 0x6B, the rental period in days (CItemManager::GetSetItemElem)
	UNKNOWN(0x6C, 0x90);
	char m_model[0x28];              // 0x90, the model name (CBall AddBall); length inferred
	UNKNOWN(0xB8, 0xC0);
	char m_fxName[7][0x28];          // 0xC0, the effect files ("xxx.spr"/"xxx.seq"; CBall::AttachFx index 0..6)
	char m_fxBone[7][0x28];          // 0x1D8, the bone each effect is attached to
	short quantity;                  // 0x2F0, the count the fortune list scales the price by
	UNKNOWN(0x2F2, 0x2FC);
};
CHECK_SIZE(sBall, 0x2FC);
CHECK_FIELD(sBall, m_flags, 0x68);
CHECK_FIELD(sBall, m_rentalDays, 0x6B);
CHECK_FIELD(sBall, m_typeId, 0x4);
CHECK_FIELD(sBall, m_icon, 0x31);
CHECK_FIELD(sBall, m_price, 0x5C);
CHECK_FIELD(sBall, quantity, 0x2F0);
CHECK_FIELD(sBall, m_model, 0x90);
CHECK_FIELD(sBall, m_fxName, 0xC0);
CHECK_FIELD(sBall, m_fxBone, 0x1D8);

// A caddie record (MakeCaddieMap: 0xC8 bytes).
struct sCaddie {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	char m_name[0x28];               // 0x8
	unsigned char m_level;           // 0x30, low 7 bits: the level the player needs (CaddieInfoDlg OnInit)
	char m_icon[0x28];               // 0x31, the icon name
	UNKNOWN(0x59, 0x68);
	unsigned char flags;             // 0x68, CaddieInfoDlg tests (flags & 0xE0) == 0xA0
	UNKNOWN(0x69, 0x90);
	unsigned long fee;               // 0x90, the contract fee ("fee", CaddieInfoDlg OnSignUpBtnUp)
	char m_model[0x28];              // 0x94, the model name (CGolfCaddie LoadMotionLevel/AddCaddie); length inferred (the levels follow)
	unsigned short m_levels[5];      // 0xBC, the stat bonus of each eLvlType; count inferred (0xBC..0xC4)
	UNKNOWN(0xC6, 0xC8);
};
CHECK_SIZE(sCaddie, 0xC8);
CHECK_FIELD(sCaddie, m_typeId, 0x4);
CHECK_FIELD(sCaddie, m_level, 0x30);
CHECK_FIELD(sCaddie, flags, 0x68);
CHECK_FIELD(sCaddie, m_icon, 0x31);
CHECK_FIELD(sCaddie, fee, 0x90);
CHECK_FIELD(sCaddie, m_levels, 0xBC);
CHECK_FIELD(sCaddie, m_model, 0x94);

// A course record (MakeCourseMap: 0x138 bytes).
struct sCourse {
	unsigned char m_active;          // 0x0, the record is in use (the course lists skip the others)
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	char m_name[0x28];               // 0x8
	unsigned char m_level;           // 0x30
	char m_icon[0x28];               // 0x31, the icon name
	UNKNOWN(0x59, 0x90);
	char m_mapName[0x28];            // 0x90, the course's file name stem ("30in_back_%s", "%s_fog.txt"); length inferred (two 0x28 strings fill 0x90-0xE0)
	char m_ambientSound[0x28];       // 0xB8, the ambient sound the course loops ("AmbientSnd", CAvatar_Main OnLoad)
	union {
		unsigned char courseFlags;   // 0xE0, the two nibbles below as one byte
		struct {
			unsigned char kind : 4;       // 0xE0 bits 0-3, the course kind
			unsigned char difficulty : 4; // 0xE0 bits 4-7, 0 normal, 1 hard (GetDiffFlag)
		};
	};
	char m_propertyFile[0x28];       // 0xE1, the course's property xml ("tuto_property.xml", MakeCourseMap); length inferred
	UNKNOWN(0x109, 0x10C);
	float m_rate;                    // 0x10C, 1.1 for the built-in TUTORIAL/TEST/RANDOM courses (MakeCourseMap)
	char m_skyEffectFile[0x28];      // 0x110, the sky effect (.spr/.seq) CSky::GenerateStars loads; length inferred
};
CHECK_SIZE(sCourse, 0x138);
CHECK_FIELD(sCourse, m_active, 0x0);
CHECK_FIELD(sCourse, m_typeId, 0x4);
CHECK_FIELD(sCourse, m_name, 0x8);
CHECK_FIELD(sCourse, m_icon, 0x31);
CHECK_FIELD(sCourse, courseFlags, 0xE0);
CHECK_FIELD(sCourse, m_mapName, 0x90);
CHECK_FIELD(sCourse, m_ambientSound, 0xB8);
CHECK_FIELD(sCourse, m_propertyFile, 0xE1);
CHECK_FIELD(sCourse, m_rate, 0x10C);

// A quest drop record (MakeQuestDropMap: 0xF4 bytes).
struct sQuestDrop {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	char m_name[0x28];               // 0x8
	unsigned char m_level;           // 0x30
	char m_icon[0x28];               // 0x31, the icon name
	UNKNOWN(0x59, 0x68);
	unsigned char m_flags;           // 0x68, bit 0: a cash (cookie) item (CItemManager::IsCashItem)
	UNKNOWN(0x69, 0xB0);
	char m_effect[0x28];             // 0xB0, the .spr or .seq effect CGolfRule::AddExtPrize opens at the cup when the drop is won
	UNKNOWN(0xD8, 0xF4);
};
CHECK_SIZE(sQuestDrop, 0xF4);
CHECK_FIELD(sQuestDrop, m_effect, 0xB0);
CHECK_FIELD(sQuestDrop, m_flags, 0x68);
CHECK_FIELD(sQuestDrop, m_typeId, 0x4);
CHECK_FIELD(sQuestDrop, m_name, 0x8);

// A quest (compound recipe) record (MakeQuestMap: 0xD8 bytes).
struct sQuest {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	char m_name[0x28];               // 0x8
	unsigned char m_level;           // 0x30
	char m_icon[0x28];               // 0x31, the icon name
	UNKNOWN(0x59, 0x90);
	union {
		unsigned long questDropId;   // 0x90, the type id of its sQuestDrop (FindQuestDrop)
		unsigned long m_materialIds[5]; // 0x90, the type ids of the five materials, 0: unused (CSharedDoc::CanCompound)
	};
	union {
		unsigned char questDropCount; // 0xA4, the count of its sQuestDrop the recipe needs (FrCompoundDlg::SetCompound message)
		unsigned char m_materialCounts[5]; // 0xA4, the count needed of each material
	};
	UNKNOWN(0xA9, 0xAA);
	unsigned short m_clearedA;       // 0xAA, cleared by MakeQuestMap
	unsigned short m_clearedB;       // 0xAC, cleared by MakeQuestMap
	UNKNOWN(0xAE, 0xB0);
	char *m_extraData;               // 0xB0, a heap array (delete[] by CItemManager::Reset)
	UNKNOWN(0xB4, 0xD8);
};
CHECK_SIZE(sQuest, 0xD8);
CHECK_FIELD(sQuest, m_typeId, 0x4);
CHECK_FIELD(sQuest, m_name, 0x8);
CHECK_FIELD(sQuest, questDropId, 0x90);
CHECK_FIELD(sQuest, questDropCount, 0xA4);
CHECK_FIELD(sQuest, m_materialIds, 0x90);
CHECK_FIELD(sQuest, m_materialCounts, 0xA4);
CHECK_FIELD(sQuest, m_clearedA, 0xAA);
CHECK_FIELD(sQuest, m_clearedB, 0xAC);
CHECK_FIELD(sQuest, m_extraData, 0xB0);

// A mascot record (MakeMascotMap: 0xFC bytes).
struct sMascot {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	char m_name[0x28];               // 0x8
	unsigned char m_level;           // 0x30
	char m_icon[0x28];               // 0x31, the icon name
	UNKNOWN(0x59, 0x68);
	unsigned char m_flags;           // 0x68, bit 0: priced in cookies (FrGiftDlg::RecalculateTotalCookie)
	UNKNOWN(0x69, 0x6B);
	unsigned char m_rentalDays;      // 0x6B, the rental period in days (CItemManager::GetSetItemElem)
	UNKNOWN(0x6C, 0x90);
	char m_model[0x28];              // 0x90, the model name (CMascot OnLoad); length inferred (the texture follows)
	char m_texture[0x28];            // 0xB8, the texture name (CMascot ChangeMascotMsgInTex); length inferred
	signed char m_periodPrices[5];   // 0xE0, the price of each rental period, 0: not sold for it (FrGiftDlg SendGift/RecalculateTotalCookie)
	UNKNOWN(0xE5, 0xEC);
	unsigned short m_itemDropRate;   // 0xEC, the item drop rate bonus in percent (FrMascotInfoDlg::OnInit)
	UNKNOWN(0xEE, 0xF0);
	unsigned short m_pangRate;       // 0xF0, the pang rate in percent, 100: no bonus (FrMascotInfoDlg::OnInit shows rate - 100)
	unsigned short m_expRate;        // 0xF2, the experience rate in percent, 100: no bonus (FrMascotInfoDlg::OnInit)
	unsigned char m_addItemSlots;    // 0xF4, the extra item slots the mascot grants (CSharedDoc::GetNumAddItemSlotByMascot)
	bool m_msgInTexture;             // 0xF5, the mascot shows its message in its texture (ChangeMascotMsgInTex)
	UNKNOWN(0xF6, 0xFC);
};
CHECK_SIZE(sMascot, 0xFC);
CHECK_FIELD(sMascot, m_rentalDays, 0x6B);
CHECK_FIELD(sMascot, m_itemDropRate, 0xEC);
CHECK_FIELD(sMascot, m_pangRate, 0xF0);
CHECK_FIELD(sMascot, m_expRate, 0xF2);
CHECK_FIELD(sMascot, m_model, 0x90);
CHECK_FIELD(sMascot, m_texture, 0xB8);
CHECK_FIELD(sMascot, m_addItemSlots, 0xF4);
CHECK_FIELD(sMascot, m_msgInTexture, 0xF5);
CHECK_FIELD(sMascot, m_typeId, 0x4);
CHECK_FIELD(sMascot, m_name, 0x8);
CHECK_FIELD(sMascot, m_icon, 0x31);
CHECK_FIELD(sMascot, m_flags, 0x68);
CHECK_FIELD(sMascot, m_periodPrices, 0xE0);

// A caddie item record (MakeCadItemMap: 0xEC bytes).
struct sCadItem {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	char m_name[0x28];               // 0x8
	unsigned char m_level;           // 0x30
	char m_icon[0x28];               // 0x31, the icon name
	UNKNOWN(0x59, 0x68);
	unsigned char m_flags;           // 0x68, bit 0: priced in cookies (FrGiftDlg::RecalculateTotalCookie)
	UNKNOWN(0x69, 0x6B);
	unsigned char m_rentalDays;      // 0x6B, the rental period in days (CItemManager::GetSetItemElem)
	UNKNOWN(0x6C, 0x90);
	char m_faceTexture[0x28];        // 0x90, the caddie's face texture with this item, "": unchanged (CGolfCaddie::AddCaddie)
	char m_bodyTexture[0x28];        // 0xB8, the caddie's body texture with this item, "": unchanged (CGolfCaddie::AddCaddie)
	short m_levels[5];               // 0xE0, the stat bonus of each eLvlType (FrBongdarishopRareItemDlg::SetRareItem); FrGiftDlg reads entries 0-3 as rental period prices
	UNKNOWN(0xEA, 0xEC);
};
CHECK_SIZE(sCadItem, 0xEC);
CHECK_FIELD(sCadItem, m_rentalDays, 0x6B);
CHECK_FIELD(sCadItem, m_levels, 0xE0);
CHECK_FIELD(sCadItem, m_typeId, 0x4);
CHECK_FIELD(sCadItem, m_icon, 0x31);
CHECK_FIELD(sCadItem, m_faceTexture, 0x90);
CHECK_FIELD(sCadItem, m_bodyTexture, 0xB8);
CHECK_FIELD(sCadItem, m_flags, 0x68);

// A description record (MakeDescMap: 0x204 bytes, keyed by its first dword).
struct sDesc {
	unsigned long m_typeId;          // 0x0, the typeid of the item it describes
	char m_text[0x200];              // 0x4, the description text
};
CHECK_SIZE(sDesc, 0x204);
CHECK_FIELD(sDesc, m_typeId, 0x0);
CHECK_FIELD(sDesc, m_text, 0x4);

// A club set record (MakeClubSetMap: 0xB4 bytes).
struct sClubSet {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	char m_name[0x28];               // 0x8
	unsigned char m_level;           // 0x30, low 7 bits: the level the player needs
	char m_icon[0x28];               // 0x31, the icon name
	UNKNOWN(0x59, 0x68);
	unsigned char m_flags;           // 0x68, bit 0: a cash (cookie) item (CItemManager::IsCashItem)
	UNKNOWN(0x69, 0x6B);
	unsigned char m_rentalDays;      // 0x6B, the rental period in days (CItemManager::GetSetItemElem)
	UNKNOWN(0x6C, 0x90);
	unsigned long m_clubTypeIds[4];  // 0x90, the type ids of the set's clubs (CClub::AddClubSet)
	short m_baseStat[5];             // 0xA0, the base stats (ItemInfoDlg SetLevelNumber/SetLevelBar)
	short m_maxStat[5];              // 0xAA, the stat limits (ItemInfoDlg SetEnchantNumber)
};
CHECK_SIZE(sClubSet, 0xB4);
CHECK_FIELD(sClubSet, m_flags, 0x68);
CHECK_FIELD(sClubSet, m_rentalDays, 0x6B);
CHECK_FIELD(sClubSet, m_clubTypeIds, 0x90);
CHECK_FIELD(sClubSet, m_baseStat, 0xA0);
CHECK_FIELD(sClubSet, m_maxStat, 0xAA);
CHECK_FIELD(sClubSet, m_typeId, 0x4);
CHECK_FIELD(sClubSet, m_level, 0x30);
CHECK_FIELD(sClubSet, m_icon, 0x31);

// A match (trophy) record (MakeMatchMap: 0x14C bytes); its icon name sits
// after the common head's (GetIconName).
struct sMatch {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	char m_name[0x28];               // 0x8, the match name (FrResultTeamDlg::OnMatchInit)
	UNKNOWN(0x30, 0x59);
	char m_trophyIcon[3][0x28];      // 0x59, the trophy model/icon names, indexed by rank - 1 (CExhibition::SetModel); entry 0 is the icon (GetIconName)
	UNKNOWN(0xD1, 0x14C);
};
CHECK_SIZE(sMatch, 0x14C);
CHECK_FIELD(sMatch, m_typeId, 0x4);
CHECK_FIELD(sMatch, m_name, 0x8);
CHECK_FIELD(sMatch, m_trophyIcon, 0x59);

// An enchant record (MakeEnchantMap: 0x10 bytes).
// No char array: MakeEnchantMap's record copy has no /GS cookie.
struct sEnchant {
	unsigned char m_active;          // 0x0
	unsigned long m_typeId;          // 0x4
	int m_value;                     // 0x8, the enchant value (TransferableItem reads it as an int)
	int m_param;                     // 0xC, a second enchant parameter; no reader transcribed yet
};
CHECK_SIZE(sEnchant, 0x10);
CHECK_FIELD(sEnchant, m_active, 0x0);
CHECK_FIELD(sEnchant, m_typeId, 0x4);
CHECK_FIELD(sEnchant, m_value, 0x8);
CHECK_FIELD(sEnchant, m_param, 0xC);

// A skin record (MakeSkinMap: 0xC4 bytes).
struct sSkin {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	char m_name[0x28];               // 0x8
	unsigned char m_level;           // 0x30
	char m_icon[0x28];               // 0x31, the icon name
	UNKNOWN(0x59, 0x5C);
	unsigned long m_price;           // 0x5C, the shop price (IFF_ITEM_COMMON::m_price; FrCheckoutDlg::OnCartOwnerDraw)
	UNKNOWN(0x60, 0x68);
	unsigned char m_flags;           // 0x68, bit 0: priced in cookies (FrGiftDlg::RecalculateTotalCookie)
	UNKNOWN(0x69, 0x6A);
	unsigned char m_timeType;        // 0x6A, 1 or 2: a time-limited skin (IsTimeLimit)
	unsigned char m_rentalDays;      // 0x6B, the rental period in days (CItemManager::GetSetItemElem)
	UNKNOWN(0x6C, 0x90);
	char m_cutinImage[0x28];         // 0x90, the cut-in image name (CShopMain/CRealMyRoomMain GetCutinImg); length inferred
	unsigned char m_sideSlideMode[2]; // 0xB8, the slide modes of the shop's two side panels (CShopMain::ChangeSkin)
	short m_periodPrices[5];         // 0xBA, the price of each rental period, 0: not sold for it (FrGiftDlg SendGift/RecalculateTotalCookie)
};
CHECK_SIZE(sSkin, 0xC4);
CHECK_FIELD(sSkin, m_rentalDays, 0x6B);
CHECK_FIELD(sSkin, m_typeId, 0x4);
CHECK_FIELD(sSkin, m_icon, 0x31);
CHECK_FIELD(sSkin, m_timeType, 0x6A);
CHECK_FIELD(sSkin, m_cutinImage, 0x90);
CHECK_FIELD(sSkin, m_flags, 0x68);
CHECK_FIELD(sSkin, m_periodPrices, 0xBA);
CHECK_FIELD(sSkin, m_sideSlideMode, 0xB8);
CHECK_FIELD(sSkin, m_price, 0x5C);

// A furniture record (MakeFurnitureMap: 0x1D0 bytes).
struct sFurniture {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	char m_name[0x28];               // 0x8
	unsigned char m_level;           // 0x30
	char m_icon[0x28];               // 0x31, the icon name
	UNKNOWN(0x59, 0x68);
	unsigned char m_flags;           // 0x68, bit 0: a cash (cookie) item (CItemManager::IsCashItem)
	UNKNOWN(0x69, 0x90);
	char m_model[0x28];              // 0x90, the model name (<model>.pet, RealMyRoomData)
	UNKNOWN(0xB8, 0xBA);
	short m_placeOnModel;            // 0xBA, 1: placed on another model of the room, found by name (CRealMyRoomData::AddModel)
	short m_modelType;               // 0xBC, the kind of model (RealMyRoomData)
	short m_rentalFlag;              // 0xBE, non-zero: the furniture offers the select menu (CRealMyRoomMain::UpdateRightMenu)
	UNKNOWN(0xC0, 0xC4);
	float m_pos[3];                  // 0xC4, its default position in the room (FrScratchDlg::UpdateListMyItem)
	float m_angle;                   // 0xD0, its default rotation (FrScratchDlg::UpdateListMyItem)
	char m_texture[3][0x28];         // 0xD4, the texture names (RealMyRoomData ChangePosterTexture/ChangeWallTexture)
	char m_texture2[3][0x28];        // 0x14C, the second texture set (RealMyRoomData ChangeFloorTexture)
	short m_dayPrices[5];            // 0x1C4, the price of each rental period (OfflineTradeDlg OnDaysBtnDown)
	UNKNOWN(0x1CE, 0x1D0);
};
CHECK_SIZE(sFurniture, 0x1D0);
CHECK_FIELD(sFurniture, m_flags, 0x68);
CHECK_FIELD(sFurniture, m_placeOnModel, 0xBA);
CHECK_FIELD(sFurniture, m_rentalFlag, 0xBE);
CHECK_FIELD(sFurniture, m_model, 0x90);
CHECK_FIELD(sFurniture, m_modelType, 0xBC);
CHECK_FIELD(sFurniture, m_texture, 0xD4);
CHECK_FIELD(sFurniture, m_texture2, 0x14C);
CHECK_FIELD(sFurniture, m_dayPrices, 0x1C4);
CHECK_FIELD(sFurniture, m_typeId, 0x4);
CHECK_FIELD(sFurniture, m_icon, 0x31);

// Bytes of unknown purpose where a char array would not do: a local copy
// of a record with a char array gets a /GS cookie the original lacks.
#define CLASSDEFINE_UNKNOWN_DWORDS(start, end) unsigned long CONCAT(unk_, __LINE__)[((end) - (start)) / 4]
// Three unknown bytes at an offset of the form 4n+1, as scalars (no array).
#define CLASSDEFINE_UNKNOWN_3BYTES(start) unsigned char CONCAT(unk_, __LINE__); unsigned short CONCAT(unkw_, __LINE__)

// A subscription item record (MakeSubscriptionItemMap: 0x2C bytes, keyed by
// the type id at 0x8). No char array: the record copy of
// MakeSubscriptionItemMap has no /GS cookie.
struct sSubscriptionItem {
	unsigned char m_active;          // 0x0
	CLASSDEFINE_UNKNOWN_3BYTES(0x1);
	unsigned char isCoupon : 1;      // 0x4, bit 0 (IsSubScriptionCoupon; IsNonVisibleItem tests it as a dword)
	CLASSDEFINE_UNKNOWN_3BYTES(0x5);
	unsigned long m_typeId;          // 0x8
	CLASSDEFINE_UNKNOWN_DWORDS(0xC, 0x2C);
};
CHECK_SIZE(sSubscriptionItem, 0x2C);
CHECK_FIELD(sSubscriptionItem, m_active, 0x0);
CHECK_FIELD(sSubscriptionItem, m_typeId, 0x8);

// A non-visible item record (MakeNonVisibleItemMap: 0x2C bytes, keyed by
// the type id at 0x8). No char array: the record copy of
// MakeNonVisibleItemMap has no /GS cookie.
struct sNonVisibleItem {
	unsigned char m_active;          // 0x0
	CLASSDEFINE_UNKNOWN_3BYTES(0x1);
	unsigned char isNonVisible : 1;  // 0x4, bit 0 (IsNonVisibleItem)
	CLASSDEFINE_UNKNOWN_3BYTES(0x5);
	unsigned long m_typeId;          // 0x8
	CLASSDEFINE_UNKNOWN_DWORDS(0xC, 0x2C);
};
CHECK_SIZE(sNonVisibleItem, 0x2C);
CHECK_FIELD(sNonVisibleItem, m_active, 0x0);
CHECK_FIELD(sNonVisibleItem, m_typeId, 0x8);

// A random box record (MakeRandomBoxMap: 0xE4 bytes): one item a box can
// give.
struct sRandomBox {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x24);
	unsigned long m_boxTypeId;       // 0x24, the box the item belongs to
	unsigned char m_kind;            // 0x28, 1 to 4
	UNKNOWN(0x29, 0x2C);
	unsigned long m_itemTypeId;      // 0x2C, the item given (stFindRandomBox)
	UNKNOWN(0x30, 0x38);
	unsigned long m_index;           // 0x38, matched by stFindRandomBox unless it searches index 0
	UNKNOWN(0x3C, 0xE4);
};
CHECK_SIZE(sRandomBox, 0xE4);
CHECK_FIELD(sRandomBox, m_active, 0x0);
CHECK_FIELD(sRandomBox, m_boxTypeId, 0x24);
CHECK_FIELD(sRandomBox, m_kind, 0x28);
CHECK_FIELD(sRandomBox, m_itemTypeId, 0x2C);
CHECK_FIELD(sRandomBox, m_index, 0x38);

// A card record (MakeCardMap: 0x148 bytes).
struct sCard {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned int m_typeId;           // 0x4, unsigned int: the key type of the card image maps (RealMyRoomMain LoadCardImages)
	char m_name[0x28];               // 0x8
	unsigned char m_level;           // 0x30
	char m_icon[0x28];               // 0x31, the icon name
	UNKNOWN(0x59, 0x68);
	unsigned char m_flags;           // 0x68, bit 0: a cash (cookie) item (CItemManager::IsCashItem)
	UNKNOWN(0x69, 0x90);
	unsigned char m_grade;           // 0x90, the card grade (CCardManager::SetCaddieCardAvility keeps the best value per grade)
	char m_model[0x28];              // 0x91, the model name (CardBoxDlg LoadCardPackPet)
	UNKNOWN(0xB9, 0xBA);
	short m_stat[5];                 // 0xBA, the stats the card adds (CardSystem)
	unsigned short m_effectType;     // 0xC4, the special effect (CardDlg, CardSystem)
	unsigned short m_effectValue;    // 0xC6
	char m_image[0x28];              // 0xC8, the item image name (FrCardUseDlg::OnItemOwnerDraw, GetBitmap("ITEMS", ...))
	char m_texture[0x28];            // 0xF0, the texture name (CardDlg DrawBtnChar)
	char m_bitmap[0x28];             // 0x118, the bitmap name (FrSPCardBuffDlg::OnInit)
	UNKNOWN(0x140, 0x142);
	unsigned short m_type;           // 0x142, the card type (CCardPage::GetPageCount)
	UNKNOWN(0x144, 0x148);
};
CHECK_SIZE(sCard, 0x148);
CHECK_FIELD(sCard, m_flags, 0x68);
CHECK_FIELD(sCard, m_grade, 0x90);
CHECK_FIELD(sCard, m_image, 0xC8);
CHECK_FIELD(sCard, m_typeId, 0x4);
CHECK_FIELD(sCard, m_name, 0x8);
CHECK_FIELD(sCard, m_icon, 0x31);
CHECK_FIELD(sCard, m_model, 0x91);
CHECK_FIELD(sCard, m_stat, 0xBA);
CHECK_FIELD(sCard, m_effectType, 0xC4);
CHECK_FIELD(sCard, m_effectValue, 0xC6);
CHECK_FIELD(sCard, m_texture, 0xF0);
CHECK_FIELD(sCard, m_bitmap, 0x118);
CHECK_FIELD(sCard, m_type, 0x142);

// A furniture ability record (MakeFurnitureAbilityList: 0x3C bytes). No code
// of the exe reads its members; the names follow the constructor's initial
// values (-1: none). No char array: the list's record copy has no /GS cookie.
struct sFurnitureAbility {
	// classdefine.h:825
	sFurnitureAbility() {
		m_active = 0;
		m_kind = 0xFF;
		m_furnitureTypeId = 0xFFFFFFFF;
		m_conditionKind = 0xFF;
		m_conditionValue = 0;
		m_effectActive = 0;
		m_effectTypeId = 0xFFFFFFFF;
		m_effectValue = 0;
		memset(m_params, 0, sizeof(m_params));
		m_rate = 0;
		m_targetTypeId = 0xFFFFFFFF;
		m_minValue = 0;
		m_maxValue = 0;
	}

	unsigned char m_active;          // 0x0
	unsigned char m_kind;            // 0x1
	unsigned long m_furnitureTypeId; // 0x4
	unsigned char m_conditionKind;   // 0x8
	unsigned long m_conditionValue;  // 0xC
	unsigned char m_effectActive;    // 0x10
	unsigned long m_effectTypeId;    // 0x14
	unsigned long m_effectValue;     // 0x18
	unsigned long m_params[4];       // 0x1C
	unsigned long m_rate;            // 0x2C
	unsigned long m_targetTypeId;    // 0x30
	unsigned long m_minValue;        // 0x34
	unsigned long m_maxValue;        // 0x38
};
CHECK_SIZE(sFurnitureAbility, 0x3C);
CHECK_FIELD(sFurnitureAbility, m_active, 0x0);
CHECK_FIELD(sFurnitureAbility, m_kind, 0x1);
CHECK_FIELD(sFurnitureAbility, m_furnitureTypeId, 0x4);
CHECK_FIELD(sFurnitureAbility, m_conditionKind, 0x8);
CHECK_FIELD(sFurnitureAbility, m_conditionValue, 0xC);
CHECK_FIELD(sFurnitureAbility, m_effectActive, 0x10);
CHECK_FIELD(sFurnitureAbility, m_effectTypeId, 0x14);
CHECK_FIELD(sFurnitureAbility, m_effectValue, 0x18);
CHECK_FIELD(sFurnitureAbility, m_params, 0x1C);
CHECK_FIELD(sFurnitureAbility, m_rate, 0x2C);
CHECK_FIELD(sFurnitureAbility, m_targetTypeId, 0x30);
CHECK_FIELD(sFurnitureAbility, m_minValue, 0x34);
CHECK_FIELD(sFurnitureAbility, m_maxValue, 0x38);

// A child item record (MakeChildItemMap: 0x48 bytes), one of the items an
// item contains (FindChildItem by type id and index).
struct sChildItem {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4, the owning item's type id (FindChildItem)
	char m_name[0x28];               // 0x8, the item name (CheckInvalidItemName)
	UNKNOWN(0x30, 0x31);
	unsigned char m_enabled : 1;     // 0x31, bit 0: the entry is offered (FrGiftDlg::SendGift)
	unsigned char isCash : 1;        // 0x31, bit 1 (IsCashChildItem)
	UNKNOWN(0x32, 0x34);
	unsigned int m_price;            // 0x34, the price when m_hasPrice (GetChildItemPrice)
	bool m_hasPrice;                 // 0x38
	UNKNOWN(0x39, 0x3A);
	unsigned short m_quantity;       // 0x3A, the count sold for m_price (FrGiftDlg::SendGift "%d" + count unit)
	unsigned int m_altPrice;         // 0x3C, the price GetChildItemPrice falls back to when m_altPeriodUnit
	unsigned char m_altPeriodUnit;   // 0x40, 0: none, 3: hours, 5: months, else days (FrGiftDlg::SendGift)
	UNKNOWN(0x41, 0x42);
	unsigned short m_altPeriod;      // 0x42, the rental length in m_altPeriodUnit units
	UNKNOWN(0x44, 0x48);
};
CHECK_SIZE(sChildItem, 0x48);
CHECK_FIELD(sChildItem, m_active, 0x0);
CHECK_FIELD(sChildItem, m_typeId, 0x4);
CHECK_FIELD(sChildItem, m_name, 0x8);
CHECK_FIELD(sChildItem, m_price, 0x34);
CHECK_FIELD(sChildItem, m_hasPrice, 0x38);
CHECK_FIELD(sChildItem, m_quantity, 0x3A);
CHECK_FIELD(sChildItem, m_altPrice, 0x3C);
CHECK_FIELD(sChildItem, m_altPeriodUnit, 0x40);
CHECK_FIELD(sChildItem, m_altPeriod, 0x42);

// An auxiliary part record (rings and the like; MakeAuxPartMap: 0xB0 bytes).
struct sAuxPart {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	char m_name[0x28];               // 0x8
	unsigned char m_level;           // 0x30
	char m_icon[0x28];               // 0x31, the icon name
	UNKNOWN(0x59, 0x68);
	unsigned char m_flags;           // 0x68, bit 0: a cash (cookie) item (CItemManager::IsCashItem)
	UNKNOWN(0x69, 0x90);
	short quantity;                  // 0x90, nonzero: the item count is displayed (IsDisplayItemNumber)
	UNKNOWN(0x92, 0x9A);
	unsigned char m_levels[5];       // 0x9A, the level of each eLvlType, 99 skipped (GetItemLevel); count inferred
	unsigned char m_capacity[5];     // 0x9F, the capacity of each eLvlType, 99 skipped (GetCharCapacity); count inferred
	char m_propertyAdd;              // 0xA4, added to property 0 (CSharedDoc::GetAuxPartProperty)
	UNKNOWN(0xA5, 0xA6);
	unsigned short m_propertyRate[3]; // 0xA6, properties 1-3 are scaled by rate / 100 when nonzero (GetAuxPartProperty)
	UNKNOWN(0xAC, 0xB0);
};
CHECK_SIZE(sAuxPart, 0xB0);
CHECK_FIELD(sAuxPart, m_flags, 0x68);
CHECK_FIELD(sAuxPart, m_active, 0x0);
CHECK_FIELD(sAuxPart, m_typeId, 0x4);
CHECK_FIELD(sAuxPart, m_capacity, 0x9F);
CHECK_FIELD(sAuxPart, m_name, 0x8);
CHECK_FIELD(sAuxPart, m_icon, 0x31);
CHECK_FIELD(sAuxPart, m_propertyAdd, 0xA4);
CHECK_FIELD(sAuxPart, m_propertyRate, 0xA6);
CHECK_FIELD(sAuxPart, quantity, 0x90);
CHECK_FIELD(sAuxPart, m_levels, 0x9A);

// A club record (MakeClubMap: 0xC4 bytes, keyed by the type id).
struct sClub {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	char m_name[0x28];               // 0x8, the item name (CheckInvalidItemName)
	UNKNOWN(0x30, 0x68);
	unsigned char m_flags;           // 0x68, bit 0: a cash (cookie) item (CItemManager::IsCashItem)
	UNKNOWN(0x69, 0x90);
	char m_model[0x28];              // 0x90, the model name: <model>.pet (CClub::AddClubSet); length inferred
	UNKNOWN(0xB8, 0xC4);
};
CHECK_SIZE(sClub, 0xC4);
CHECK_FIELD(sClub, m_model, 0x90);
CHECK_FIELD(sClub, m_flags, 0x68);
CHECK_FIELD(sClub, m_active, 0x0);
CHECK_FIELD(sClub, m_typeId, 0x4);
CHECK_FIELD(sClub, m_name, 0x8);

// A set item record (MakeSetItemMap: 0xDC bytes, keyed by the type id).
struct sSetItem {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	char m_name[0x28];               // 0x8, the item name (CheckInvalidItemName)
	unsigned char m_level;           // 0x30
	char m_icon[0x28];               // 0x31, the icon name (FrSetItemDlg::SetSetItem)
	UNKNOWN(0x59, 0x68);
	unsigned char m_flags0;          // 0x68, bits 5-7: the sale kind (FrSetItemDlg::SetSetItem)
	UNKNOWN(0x69, 0x90);
	unsigned char m_itemCount;       // 0x90, the components in m_itemTypeIds (GetSetItemElem)
	UNKNOWN(0x91, 0x94);
	unsigned long m_itemTypeIds[10]; // 0x94, the type ids of the components (GetSetItemElem); count inferred
	UNKNOWN(0xBC, 0xDC);
};
CHECK_SIZE(sSetItem, 0xDC);
CHECK_FIELD(sSetItem, m_level, 0x30);
CHECK_FIELD(sSetItem, m_icon, 0x31);
CHECK_FIELD(sSetItem, m_flags0, 0x68);
CHECK_FIELD(sSetItem, m_itemCount, 0x90);
CHECK_FIELD(sSetItem, m_itemTypeIds, 0x94);
CHECK_FIELD(sSetItem, m_active, 0x0);
CHECK_FIELD(sSetItem, m_typeId, 0x4);
CHECK_FIELD(sSetItem, m_name, 0x8);

// A title record (MakeTitleMap: 0x58 bytes, keyed by the type id).
struct sTitle {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	char m_name[0x28];               // 0x8, the title name (CheckInvalidItemName)
	UNKNOWN(0x30, 0x58);
};
CHECK_SIZE(sTitle, 0x58);
CHECK_FIELD(sTitle, m_active, 0x0);
CHECK_FIELD(sTitle, m_typeId, 0x4);
CHECK_FIELD(sTitle, m_name, 0x8);

// A hair style record (MakeHairStyleMap: 0x94 bytes, keyed by the type id).
struct sHairStyle {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	char m_name[0x28];               // 0x8, the item name (CheckInvalidItemName)
	unsigned char m_level;           // 0x30
	char m_icon[0x28];               // 0x31, the icon name (CShopMain::LoadItemImages, "HAIRS")
	UNKNOWN(0x59, 0x68);
	unsigned char m_flags;           // 0x68, bit 0: a cash (cookie) item (CItemManager::IsCashItem)
	UNKNOWN(0x69, 0x90);
	unsigned char m_color;           // 0x90, the hair color (FindHairStyle's second parameter)
	unsigned char m_style;           // 0x91, the hair style (FindHairStyle's first parameter)
	UNKNOWN(0x92, 0x94);
};
CHECK_SIZE(sHairStyle, 0x94);
CHECK_FIELD(sHairStyle, m_flags, 0x68);
CHECK_FIELD(sHairStyle, m_active, 0x0);
CHECK_FIELD(sHairStyle, m_typeId, 0x4);
CHECK_FIELD(sHairStyle, m_name, 0x8);
CHECK_FIELD(sHairStyle, m_color, 0x90);
CHECK_FIELD(sHairStyle, m_style, 0x91);
CHECK_FIELD(sHairStyle, m_level, 0x30);
CHECK_FIELD(sHairStyle, m_icon, 0x31);

// A special prize item record (MakeSpecialPrizeItemMap: 0xC bytes, keyed by
// the type id, one map per eSPAVILITYTYPE).
struct sSpecialPrizeItem {
	unsigned long m_typeId;          // 0x0
	eSPAVILITYTYPE m_type;           // 0x4, picks the map of CItemManager the record goes into
	unsigned long m_value;           // 0x8, the ability value; type inferred
};
CHECK_SIZE(sSpecialPrizeItem, 0xC);
CHECK_FIELD(sSpecialPrizeItem, m_typeId, 0x0);
CHECK_FIELD(sSpecialPrizeItem, m_type, 0x4);
CHECK_FIELD(sSpecialPrizeItem, m_value, 0x8);

// A random recycle record (MakeRandomRecycleMap: 0x10 bytes, keyed by its
// first dword; the same key also goes into a std::set<unsigned long>).
struct sRandomRecycle {
	unsigned long m_recycleId;       // 0x0, the key of the record
	unsigned long m_typeId;          // 0x4, the item type id (RecycleDlg CheckRandomRecycleItem)
	UNKNOWN(0x8, 0x10);
};
CHECK_SIZE(sRandomRecycle, 0x10);
CHECK_FIELD(sRandomRecycle, m_recycleId, 0x0);
CHECK_FIELD(sRandomRecycle, m_typeId, 0x4);

// An offline shop record (MakeOfflineMap: 0xC8 bytes; CItemManager's map
// node puts its color byte at 0xD8). Fields not transcribed yet.
struct sOfflineShop {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned int m_typeId;           // 0x4, the map key
	char m_name[0x28];               // 0x8, the item name (CheckInvalidItemName)
	UNKNOWN(0x30, 0x68);
	unsigned char m_flags;           // 0x68, bit 0: a cash (cookie) item (CItemManager::IsCashItem)
	UNKNOWN(0x69, 0x70);
	unsigned short m_saleStartYear;  // 0x70, wYear of the sale start _SYSTEMTIME
	UNKNOWN(0x72, 0x80);
	unsigned short m_saleEndYear;    // 0x80, wYear of the sale end _SYSTEMTIME
	UNKNOWN(0x82, 0xC8);
};
CHECK_SIZE(sOfflineShop, 0xC8);
CHECK_FIELD(sOfflineShop, m_flags, 0x68);
CHECK_FIELD(sOfflineShop, m_active, 0x0);
CHECK_FIELD(sOfflineShop, m_typeId, 0x4);
CHECK_FIELD(sOfflineShop, m_name, 0x8);
CHECK_FIELD(sOfflineShop, m_saleStartYear, 0x70);
CHECK_FIELD(sOfflineShop, m_saleEndYear, 0x80);

// A caddie magic box record (MakeCadieMagicBoxMap: 0x68 bytes, copied whole
// into CItemManager's list). Fields not transcribed yet.
struct sCadieMagicBox {
	// classdefine.h:740
	sCadieMagicBox() {
		m_number = 0;
		m_active = 0;
		m_category = 0;
		m_character = 0;
		m_level = 0;
		m_output = 0;
		m_outputCount = 0;
		memset(m_element, 0, sizeof(m_element));
		memset(m_elementCount, 0, sizeof(m_elementCount));
		m_randSeq = 0;
		memset(m_randName, 0, sizeof(m_randName));
	}

	unsigned long m_number;          // 0x0, the 1-based recipe number
	unsigned char m_active;          // 0x4, cleared by the constructor; no reader known
	unsigned long m_category;        // 0x8, 0..4: low, mid, high, special, event
	unsigned long m_character;       // 0xC
	unsigned long m_level;           // 0x10
	unsigned long m_output;          // 0x14, the result item's type id
	unsigned long m_outputCount;     // 0x18
	unsigned long m_element[4];      // 0x1C, the ingredients' type ids
	unsigned long m_elementCount[4]; // 0x2C, the ingredients' counts
	unsigned long m_randSeq;         // 0x3C
	char m_randName[0x28];           // 0x40
};
CHECK_SIZE(sCadieMagicBox, 0x68);
CHECK_FIELD(sCadieMagicBox, m_active, 0x4);
CHECK_FIELD(sCadieMagicBox, m_character, 0xC);
CHECK_FIELD(sCadieMagicBox, m_level, 0x10);
CHECK_FIELD(sCadieMagicBox, m_outputCount, 0x18);
CHECK_FIELD(sCadieMagicBox, m_randSeq, 0x3C);
CHECK_FIELD(sCadieMagicBox, m_randName, 0x40);

// A Tiki magic box output record (MakeTikiRecipeMap: 0x34 bytes, given to
// CTikiMagicBoxDoc::InsertOutput).
struct sTikiOutputTable {
	// classdefine.h:860
	sTikiOutputTable() {
		memset(this, 0, sizeof(*this));
	}

	unsigned long m_typeId;          // 0x0, the key of the record
	UNKNOWN(0x4, 0x5);
	char m_name[0x23];               // 0x5, the output name (the map key of CTikiMagicBoxDoc); ends by 0x28
	unsigned long m_itemTypeId;      // 0x28, the item the output gives (CTikiMagicBoxDoc::GetOutput)
	unsigned long m_itemCount;       // 0x2C, how many of it
	UNKNOWN(0x30, 0x34);
};
CHECK_SIZE(sTikiOutputTable, 0x34);
CHECK_FIELD(sTikiOutputTable, m_typeId, 0x0);
CHECK_FIELD(sTikiOutputTable, m_name, 0x5);
CHECK_FIELD(sTikiOutputTable, m_itemTypeId, 0x28);
CHECK_FIELD(sTikiOutputTable, m_itemCount, 0x2C);

// A Tiki magic box point record (MakeTikiPointTableMap: 0x30 bytes, given to
// CTikiMagicBoxDoc::InsertPointTable).
struct sTikiPointTable {
	// classdefine.h:874
	sTikiPointTable() {
		memset(this, 0, sizeof(*this));
	}

	unsigned long m_typeId;          // 0x0, the key of the record
	UNKNOWN(0x4, 0x5);
	char m_name[0x23];               // 0x5, the output group name (a key of CTikiMagicBoxDoc's output groups); ends by 0x28
	unsigned long m_minPoint;        // 0x28, the point range the table covers (CTikiMagicBoxDoc::GetOutput)
	unsigned long m_maxPoint;        // 0x2C
};
CHECK_SIZE(sTikiPointTable, 0x30);
CHECK_FIELD(sTikiPointTable, m_typeId, 0x0);
CHECK_FIELD(sTikiPointTable, m_name, 0x5);
CHECK_FIELD(sTikiPointTable, m_minPoint, 0x28);
CHECK_FIELD(sTikiPointTable, m_maxPoint, 0x2C);

// A Tiki magic box special recipe record (MakeTikiSpecialTableMap: 0x3C
// bytes, given to CTikiMagicBoxDoc::InsertSpecialRecipe).
struct sTikiSpecialRecipe {
	// classdefine.h:886
	sTikiSpecialRecipe() {
		memset(this, 0, sizeof(*this));
	}

	unsigned long m_typeId;          // 0x0, the key of the record
	UNKNOWN(0x4, 0x5);
	char m_name[0x23];               // 0x5, the recipe name; ends by 0x28
	int m_count;                     // 0x28, the entries of m_typeIds in use
	unsigned long m_typeIds[4];      // 0x2C, the item type ids of the recipe; count from the record size
};
CHECK_SIZE(sTikiSpecialRecipe, 0x3C);
CHECK_FIELD(sTikiSpecialRecipe, m_typeId, 0x0);
CHECK_FIELD(sTikiSpecialRecipe, m_name, 0x5);
CHECK_FIELD(sTikiSpecialRecipe, m_count, 0x28);
CHECK_FIELD(sTikiSpecialRecipe, m_typeIds, 0x2C);

namespace S5 {

// A cut-in record (MakeCutinInforMationTableMap: 0xD0 bytes, keyed by the
// type id). S5::CCutin loads one WOverlay from each image name (OnProcess,
// TwAnimateTest).
struct sCutinInformation {
	unsigned char m_active;          // 0x0
	UNKNOWN(0x1, 0x4);
	unsigned long m_typeId;          // 0x4
	UNKNOWN(0x8, 0x10);
	unsigned long m_condition;       // 0x10, the cut-in condition (CGolfTask passes it to S5::CCutin as message 0x24C's param3, CCutinSystem::SetCondition)
	UNKNOWN(0x14, 0x18);
	int m_charSerial;                // 0x18, the character (type id & 0x3FFFFFF) the cut-in belongs to, 100 or more: every character (CShopMain::Build_Cutin)
	char m_images[4][0x2C];          // 0x1C, the overlay image names
	UNKNOWN(0xCC, 0xD0);
};
CHECK_SIZE(sCutinInformation, 0xD0);
CHECK_FIELD(sCutinInformation, m_charSerial, 0x18);
CHECK_FIELD(sCutinInformation, m_active, 0x0);
CHECK_FIELD(sCutinInformation, m_typeId, 0x4);
CHECK_FIELD(sCutinInformation, m_condition, 0x10);
CHECK_FIELD(sCutinInformation, m_images, 0x1C);

} // namespace S5

} // namespace IFF_STRUCT

#endif // PG_SHARED_CLASSDEFINE_H
