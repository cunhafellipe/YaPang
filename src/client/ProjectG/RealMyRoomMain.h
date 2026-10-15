// Original: d:\build\custom\temp\code\client\projectg\realmyroommain.h
#ifndef PG_CLIENT_PROJECTG_REALMYROOMMAIN_H
#define PG_CLIENT_PROJECTG_REALMYROOMMAIN_H

#include <windows.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>

#include "defines.h"
#include "padding.h"
#include "RTTI.h"
#include "frcmdtarget.h"
#include "TaskMain.h"
#include "scenemanager.h"
#include "polysoup.h"
#include "AnotherHand.h"
#include "client/Wangreal/include/wmath.h"
#include "client/Wangreal/include/wtypes.h"
#include "client/Wangreal/include/wscene.h"
#include "shared/globalgamedefine.h"
#include <list>
#include <map>

class CFxSequence;
class CRealMyRoom_AuthorityDlg;
class CFurniture_AbilityDlg;
class FrFortuneOpenDlg;
class FrWnd;
class FrNoteDlg;
class FrFortuneDlg;
class FrEdit;
class FrGroupBox;
class FrArea;
class FrViewer;
class FrStatic;
class FrGaugeBar;
class WOverlay;
class FrForm;
class FrCardUseDlg;
class FrItemAttachCardDlg;
class FrOpenCardPackDlg;
class FrEnchantDlg;
class FrMascotInfoDlg;
class FrPangGiftDlg;
class FrUniteResultDlg;
class FrReplayModeDlg;
class FrUccCopyDlg;
class FrDowngradeDlg;
class CPost_ReciveDlg;
class CCardBook;
class FrItemStorageNotice;
class FrListBox;
class FrButton;
class FrTabButton;
class FrUccDrawDlg;
class FrGraphicInterface;
class FrItemStorageAccessPanel;
class CExhibition;
class Bitmap;
class CPost_SendDlg;
class WOverlay;
struct sItemInfo;
struct sCharacterInfo;
class WTitleFont;
struct sGiftInfo;
struct sCaddieInfo;
struct FrListItem;

// The real my-room task (registered as "CRealMyRoomMain"). Bases from the
// constructor's vtable stores: CTaskMain at 0x0, CRenderFuncPtr at 0x1e0,
// CHandOwner at 0x1e4. Only the members the transcribed functions touch are
// declared.
// The PC bang item ReinitializePC re-equips: 1 the caddie, 2 the mascot.
// The original header is not known.
enum ePCBangItem {
	PCBANG_ITEM_CADDIE = 1,
	PCBANG_ITEM_MASCOT = 2
};

class CRealMyRoomMain : public CTaskMain, public CRenderFuncPtr, public CHandOwner {
public:
	CRealMyRoomMain(void);
	virtual ~CRealMyRoomMain(void);

	static const WRTTI m_RTTI;

	virtual const WRTTI* GetRTTI(void) const;   // realmyroommain.h:45

	virtual void OnLoad(void);
	virtual void OnInit(void);
	virtual void OnPreLoadInit(void);
	virtual void OnProcess(float elapsed);
	virtual void DoReLoad(void);
	virtual void Display(void);
	virtual void OnDisplay(void);
	virtual void OnDestroy(void);
	virtual void HandleMsg(const MsgObject& msg);

	// Nested helper struct; members not recovered yet.
	struct miniBtn_t {
		miniBtn_t(void);
	};

	// An area of the item list where the right button does nothing
	// (OnItemListInit), one per item list tab.
	// (LoadMyRoomImages sets the rect and the two pictures of the button
	// drawn there, like CShopMain::miniBtn_t.)
	struct sItemListArea {
		WRect rect;                    // 0x0
		const Bitmap* normal;          // 0x10
		const Bitmap* over;            // 0x14
		UNKNOWN(0x18, 0x1C);
	};

	// A numbered select icon ("select_%d_n" / "select_%d_o", LoadMyRoomImages).
	struct sSelectIcon {
		const Bitmap* normal;          // 0x0
		const Bitmap* over;            // 0x4
		UNKNOWN(0x8, 0xC);
	};

	// The tabs of the item list; the values are not recovered yet.
	enum selTab_t {
	};

	// The main tabs; the values are not recovered yet.
	enum mainTab_t {
	};

	// The tabs of the stat window (OnStatPartUp selects 0, OnStatAllUp 1).
	enum statTab_t {
		STAT_TAB_PART = 0,
		STAT_TAB_ALL = 1
	};

	// The sub tabs of the item list; the values are not recovered yet.
	enum subTab_t {
	};

	// The main tabs of the furniture list; the values are not recovered yet.
	enum eFurniture_Tab {
	};

	bool IsItemStorageVisible(void) const;   // realmyroommain.h:1404
	void SetUIState(unsigned long state);
	void ChangeFlags(unsigned long flag);
	bool GetFlags(unsigned long flag);
	void OpenItemStorageAccessPanel(unsigned char type);
	void OpenItemStorageNotice(void);
	bool OnResultItemStorageNotice(int result, FrForm* pForm);
	bool OnResultItemStorageAccessPanel(int result, FrForm* pForm);
	void SetItemStorageVisible(bool visible);
	void OpenInformation(unsigned long* pTypeId, sGiftInfo* pGiftInfo, unsigned long* pSerial);
	void OpenInformation(FrListItem* pItem);

private:
	static sFRESH_ENTRY _MsgEntries[];

protected:
	static sFRESH_MSGMAP _MsgMap;
	virtual const sFRESH_MSGMAP* GetMessageMap(void) const;   // realmyroommainui.cpp:171

	virtual void OnAnotherHand(int nWork, void* pData);
	virtual void OnUnderBar_MyRoomUp(void);
	virtual void CloseAllDialogs(void);

	void OpenLayout(const char* layout);
	void OpenEventGiftBox(sItemInfo* pItemInfo);
	void OpenS4EventGiftBox(sItemInfo* pItemInfo);
	bool OnOpenEventGiftDlgResult(int result, FrForm* pForm);
	bool OnOpenS4EventGiftDlgResult(int result, FrForm* pForm);
	void OnUnderBar_GiftUp(void);
	void OpenCaddieInfo(const sCaddieInfo& info);
	void OnMiddleUIInit(int param);
	void OnTabTxt1Init(int param);
	void OnTabTxt2Init(int param);
	void OnTabTxt3Init(int param);
	void OnSelectedNameInit(int param);
	void OnPetInit(int param);
	void OnSkinPetInit(int param);
	void OnFurnitureArrangeInit(int param);
	void OnItemDescBgInit2(int param);
	void OnItemNameInit(int param);
	void OnGiftDescInit(int param);
	void OnUpgradeInit(int param);
	void OnDowngradeInit(int param);
	void SetItemDesc(void);
	void OnItemListMouseMove(void);
	void OnSelItemInit(int param);
	void OnInitItemStoragePang(int param);
	void OnInitItemStorageIndexMsg(int param);
	void OnInitItemStorageDragDrop(int param);
	void OnFurniturePupInit(int param);
	void OnFurniturePdownInit(int param);
	void OnFurnitureListMaxNumInit(int param);
	void OnFurniture_BtnListNumInit(int param);
	void OnFurnitureResetInit(int param);
	void OnFurnitureBackInit(int param);
	void OnFurnitureSaveInit(int param);
	void OnFurnitureOutInit(int param);
	void OnFurnitureBackGroundInit1(int param);
	void OnFurnitureBackGroundInit2(int param);
	void OnFurnitureBackGroundInit3(int param);
	void OnFurnitureBackGroundInit4(int param);
	void OnFurnitureBackGroundInit5(int param);
	void OnFurnitureBackGroundInit6(int param);
	void OnFurnitureBackGroundInit7(int param);
	void OnFurnitureBackGroundInit8(int param);
	void OnFurnitureScrollInit1(int param);
	void OnFurnitureScrollInit2(int param);
	void OnFurnitureScrollInit3(int param);
	void DrawStatusCard(FrGraphicInterface* pGI, FrListItem* pItem, short* const cardStatus, float x);
	void DrawStatus2(FrGraphicInterface* pGI, FrListItem* pItem, short* const status, short* const slotStatus, float x);
	void OnSkinPet2Init(int param);
	void OnItemDescBgInit(int param);
	void OnItemSlotInit(intptr_t param);
	void OnStatFrameInit(int param);
	void OnStatTextInit(int param);
	void OnSelFrameInit(int param);
	void OnStatBoxInit(int param);
	void OnSelItemIconInit(int param);
	void OnStatPartInit(int param);
	void OnStatAllInit(int param);
	void OnTabSubInit(int param);
	void OnPowerEditInit(int param);
	void OnControlEditInit(int param);
	void OnImpactEditInit(int param);
	void OnSpinEditInit(int param);
	void OnCurveEditInit(int param);
	void OnPowerBarInit(intptr_t param);
	void OnControlBarInit(intptr_t param);
	void OnImpactBarInit(intptr_t param);
	void OnSpinBarInit(intptr_t param);
	void OnCurveBarInit(intptr_t param);
	bool UnselectItemFromItemList(unsigned long typeId);
	void Furniture_ShowUI_Left(bool bShow);
	void OnDBClickItemStorageList(void);
	void OnStatAllOwnerDraw(int param);
	void DrawCutinFrame(void);
	WOverlay* GetCutinImg(unsigned long typeId);
	WOverlay* GetCardImg(unsigned long typeId);
	void DrawCard(void);
	void OpenEventXmasGiftBox(sItemInfo* pItemInfo);
	void OnBtnMenuInit(int param);
	void OnEffectViewInit(int param);
	void OnFurnitureListInit(int param);
	void SafeLoadPet(unsigned long* pTypeId, bool bForce);
	void OnEquipResetUp(void);
	void OnFurniturePupButtonUp(void);
	void OnFurniturePdownButtonUp(void);
	bool SelectItemList(FrListItem* pItem);
	bool SelectItemList(unsigned long typeId);
	void UnselectItemList(FrListItem* pItem);
	bool UnselectItemList(unsigned long typeId);
	void SetGiftDescString(const char* pSender, const char* pMessage, const char* pDate, const char* pTime);
	bool OnNoteDlgResult(int result, FrForm* pForm);

	void DrawMouseCursor(void);
	void RefreshAllUccClothes(void);
	void LoadBg(const char* name, int hole);
	void ProcessCamera(float elapsed);
	void CheckUIPosition(void);
	void CalculationCameraMat(WMatrix& mat, WVector at, WVector dir, WVector pos, float targetFov, float curFov, float speed);
	void SetCameraDisplayType(int type);
	void ToggleTopViewMode(void);
	void InitMyRoomControls(void);
	void InitMyRoomDialogs(void);
	void InitMyRoomVariables(void);
	void ChangeSelTab(selTab_t tab);
	void TurnOnStat(void);
	void TurnOffStat(void);
	void OnSelItemOwnerDraw(int param);
	void OnSelItemUp(void);
	void OnCardPackListDown(void);
	void OnCardPackListRbUp(void);
	void BuildCardPackList(void);
	void OnCardPackListInit(int param);
	void OnTabCardPackInit(int param);
	void OnTabCardVolume1Init(int param);
	void OnTabCardVolume2Init(int param);
	void OnTabCardVolume3Init(int param);
	void OnTabCardVolume4Init(int param);
	void OnRightMenuInit(int param);
	void OnCardBookInit(int param);
	void OnCardBookIndexInit(int param);
	void OnCardUseInit(int param);
	void OnCardPropertyInit(int param);
	void OnCardBookOwnerDraw(int param);
	void OnCardBookIndexLBDown(void);
	void OnCardBookIndexMouseOver(void);
	void OnTabCardPackLBDown(void);
	void OnTabCardVolume1LBDown(void);
	void OnTabCardVolume2LBDown(void);
	void OnTabCardVolume3LBDown(void);
	void OnTabCardVolume4LBDown(void);
	bool OnCardBoxDlgResult(int result, FrForm* pForm);
	void CardPackBehavior(unsigned char slot, unsigned char button);
	void OnCardPackLBDown1(void);
	void OnCardPackLBDown2(void);
	void OnCardPackLBDown3(void);
	void OnCardPackLBDown4(void);
	void OnCardPackLBDown5(void);
	void OnCardPackLBDown6(void);
	void OnCardPackLBDown7(void);
	void OnCardPackLBDown8(void);
	void OnCardPackRBDown1(void);
	void OnCardPackRBDown2(void);
	void OnCardPackRBDown3(void);
	void OnCardPackRBDown4(void);
	void OnCardPackRBDown5(void);
	void OnCardPackRBDown6(void);
	void OnCardPackRBDown7(void);
	void OnCardPackRBDown8(void);
	void CardBehavior(unsigned char slot, unsigned char button);
	void OnCardLBDown1(void);
	void OnCardLBDown2(void);
	void OnCardLBDown3(void);
	void OnCardLBDown4(void);
	void OnCardLBDown5(void);
	void OnCardLBDown6(void);
	void OnCardLBDown7(void);
	void OnCardLBDown8(void);
	void OnCardLBDown9(void);
	void OnCardLBDown10(void);
	void OnCardLBDown11(void);
	void OnCardLBDown12(void);
	void OnCardRBDown1(void);
	void OnCardRBDown2(void);
	void OnCardRBDown3(void);
	void OnCardRBDown4(void);
	void OnCardRBDown5(void);
	void OnCardRBDown6(void);
	void OnCardRBDown7(void);
	void OnCardRBDown8(void);
	void OnCardRBDown9(void);
	void OnCardRBDown10(void);
	void OnCardRBDown11(void);
	void OnCardRBDown12(void);
	bool OnCardRemoveDlgResult(int result, FrForm* pForm);
	bool OnEnchantDlgResult(int result, FrForm* pForm);
	bool OnMascotInfoDlgResult(int result, FrForm* pForm);
	bool OnUpgradeDlgResult(int result, FrForm* pForm);
	bool OnFunritureSaveMsgdlgResult(int result, FrForm* pForm);
	void OOnBtnMenuButtonUP(void);
	bool OnResultDlg_Report(int result, FrForm* pForm);
	bool OnOpenEventXmasGiftDlgResult(int result, FrForm* pForm);
	void OnOwnerDrawItemStoragePang(int param);
	void OnItemSlotBtnDown(void);
	void OnBtn_MenuListRBtnDown(void);
	void OnBtn_MenuListMouseMove(void);
	void OnBtn_MenuListDClick(void);
	void SetLevelBar(void);
	void ChangeStatTab(statTab_t tab);
	void OnStatPartUp(void);
	void OnStatAllUp(void);
	void OnFurniture_BtnListNumBtnUp(void);
	bool OnFurnitureListEnterKey(int param);
	void OnOwnerDrawItemStorageBGI(int param);
	void OnMouseMoveItemStorageList(void);
	void OnOwnerDrawItemStorageDragDrop(int param);
	void SelectMenu(unsigned char menu);
	bool OnAuthDlgFaileResult(int result, FrForm* pForm);
	void ControlUiHideMode(int mode);
	void OnFurnitureArrangeButtonUP(void);
	void SetMiddleUIControl(bool bShow);
	void BuildDisplayStand(subTab_t tab);
	bool OnReplayDelOpenDlgResult(int result, FrForm* pForm);
	void SetOverAndUnderBarVisible(bool bVisible);
	void UIDisplayControl(bool bShow);
	void SetListMenu(void);
	bool OnPostDlgResult(int result, FrForm* pForm);
	void OnUnderBar_ItemStorageUp(void);
	void OnLBDownItemStoragePangMinus(void);
	void OnLBDownItemStoragePangPlus(void);
	void OnOwnerDrawItemStorageList(int param);
	void OnOwnerDrawItemStorageIndexMsg(int param);
	void OnLBDownItemStorageIndexMsg(void);
	void OnLBDownItemStoragePageNext(void);
	void OnLBDownItemStoragePagePrev(void);
	void TurnOnItemSelect(void);
	void TurnOffItemSelect(void);
	void BoundCardPackPage(FrButton* pButton, unsigned char slot);
	void OnCardPackInit1(int param);
	void OnCardPackInit2(int param);
	void OnCardPackInit3(int param);
	void OnCardPackInit4(int param);
	void OnCardPackInit5(int param);
	void OnCardPackInit6(int param);
	void OnCardPackInit7(int param);
	void OnCardPackInit8(int param);
	void SetCardBookVisible(bool bVisible);
	void BoundCardPage(FrButton* pButton, unsigned char slot);
	void OnCardInit1(int param);
	void OnCardInit2(int param);
	void OnCardInit3(int param);
	void OnCardInit4(int param);
	void OnCardInit5(int param);
	void OnCardInit6(int param);
	void OnCardInit7(int param);
	void OnCardInit8(int param);
	void OnCardInit9(int param);
	void OnCardInit10(int param);
	void OnCardInit11(int param);
	void OnCardInit12(int param);
	void OnFurnitureListNumBtnUp(void);
	bool OnUccCopyDlgResult(int result, FrForm* pForm);
	void ShowPet(unsigned long typeId);
	void CloseLayout(void);
	void ChangeSubTab(subTab_t tab, bool bForce);
	void OnTabSubButtonUp(void);
	void TurnOnItemStatus(void);
	void TurnOffItemStatus(void);
	void AutoScroll(void);
	void ChangeFMainTab(eFurniture_Tab tab, bool bForce);
	void BuildDisplayStandFurniture(void);
	void ReinitializePC(ePCBangItem item);
	bool ChangeCaddie(unsigned long id);
	void ChangeMascot(unsigned long id);
	void GetCaddieStatus(unsigned char* const pStatus, unsigned char* const pUnused);
	void OnUnderBar_BackUp(void);
	void OnFurnitureTabMainButtonUp(void);
	void OnRightMenuRBtnDown(void);
	bool OnUccDrawDlgResult(int result, FrForm* pForm);
	const Bitmap* GetItemIcon(unsigned long typeId);
	void DrawSelItem(FrGraphicInterface* pGI, unsigned long typeId);
	bool OnOpenNewYearMoneyDlgResult(int result, FrForm* pForm);
	void OpenNewYearMoney(sItemInfo* pItemInfo);
	void OnBtn_MenuListBtnDown(void);
	void HidePet(bool bWait);
	void SetPetAreaRect(void);
	void Reinitialize(bool bForce);
	void OnMyRoomFinish(void);
	void UnequipItem(unsigned long* pItemId);
	void OnItemSlotDClick(void);
	bool IsInItemList(unsigned long typeId);
	void OnEffectViewOnwnerDraw(int param);
	void OnMyRoomInit(int param);
	void Net_StartPeriodItem(unsigned long serial);
	bool OnOfflineShopEditMsgDlgResult(int result, FrForm* pForm);
	bool OnPostSendDlgResult(int result, FrForm* pForm);
	bool OnPostSendDlg2Result(int result, FrForm* pForm);
	void PostDlgCreate(void);
	void Net_EquipCharacter(void);
	void Net_EquipItem(void);
	bool OnFortuneDlgResult(int result, FrForm* pForm);
	void CharacterViewControl(bool bShow);
	void ChangeMainTab(mainTab_t tab, bool bForce);
	void SetMiddleUIVisible(bool bVisible);
	void OnGiftBgInit(int param);
	void OnSelItemPrevInit(int param);
	void OnSelItemNextInit(int param);
	void Net_EquipCaddie(void);
	void Net_EquipGear(void);
	void UpdateRightMenu(void);
	void PopupRightMenu(int x, int y);
	void OnFurnitureListNumInit(int param);
	void OnInitItemStoragePageNext(int param);
	void OnInitItemStoragePagePrev(int param);
	void OnLBDownItemStorageClose(void);
	void OnInitItemStoragePangMinus(int param);
	void OnInitItemStoragePangPlus(int param);
	void OnInitItemStorageClose(int param);
	void OnItemSlotBoxInit(int param);
	void OnEquipCancelInit(int param);
	void OnEquipResetInit(int param);

	// Declared from the PDB publics, in source-line order (the original
	// declaration order is not known).
	void LoadMyRoomImages(void);
	void LoadItemImages(void);
	void ChangeCharacter(unsigned long typeId);
	int ChangeClubset(unsigned long serial);
	void ChangeAztec(unsigned long typeId);
	void DrawCharacterList(FrGraphicInterface* pGI, unsigned long typeId);
	void DrawCaddieList(FrGraphicInterface* pGI, unsigned long typeId);
	void DrawMascotList(FrGraphicInterface* pGI, unsigned long typeId);
	void DrawAztecList(FrGraphicInterface* pGI, unsigned long typeId);
	void DrawClubList(FrGraphicInterface* pGI, unsigned long typeId);
	void DrawSkinFrame(void);
	void DrawSkinSlot(void);
	void OnPetOwnerDraw(int param);
	void OnItemDescBgOwnerDraw2(int param);
	void OnItemDescBgOwnerDraw(int param);
	void OnItemDescInit2(int param);
	void OnItemDescInit(int param);
	void OnEquipCancelUp(void);
	void OnItemSlotOwnerDraw(int param);
	void OnSelItemIconOwnerDraw(int param);
	void OnSelItemPrevUp(void);
	void OnSelItemNextUp(void);
	void OnStatPartOwnerDraw(int param);
	void OnTabMainInit(int param);
	void OnTabMainButtonUp(void);
	void OnGiftPageOwnerDraw(int param);
	void OnGiftPageLBDown(void);
	void OnCardPackListDraw(int param);
	void OnCardUseLBDown(void);
	void OnItemListInit(intptr_t param);
	void DrawStatus(FrGraphicInterface* pGI, FrListItem* pItem, short* const status, short* const slotStatus);
	void OnItemListOwnerDraw(intptr_t param);
	void MoveGift(sGiftInfo* pGiftInfo);
	void OpenTikiReport(sItemInfo* pItemInfo);
	void OnItemListBtnDown(void);
	void OnItemListRBtnDown(void);
	void OnItemListDClick(void);
	void OnUpgradeUp(void);
	void OnDowngradeUp(void);
	void OnTooltipHoverOn(int param);
	void OnTooltipHoverOff(int param);
	void GetTotalStatus(unsigned char* const status, unsigned char* const slotStatus, unsigned char* const cardStatus);
	void GetCharacterStatus(unsigned char* const status, unsigned char* const slotStatus);
	void GetClubSetStatus(unsigned char* const status, unsigned char* const slotStatus);
	void ShowMainModel(void);
	void DrawSelectItemList(FrListItem* pItem);
	int GetItemNumInMyPocket(unsigned long typeId);
	void Net_EquipDresserParts(void);
	void Net_EquipSkin(void);
	void Net_EquipMascot(void);
	bool OnPangGiftDlgResult(int result, FrForm* pForm);
	bool OnReturnGiftDlgResult(int result, FrForm* pForm);
	bool OnMoveGiftDlgResult(int result, FrForm* pForm);
	bool OnFurnitrueListDeleteDlgResult(int result, FrForm* pForm);
	bool OnCaddieInfoDlgResult(int result, FrForm* pForm);
	bool OnOpenTikiReport(int result, FrForm* pForm);
	void EquipItem(sItemInfo* pItemInfo);
	bool OnFortuneOpenDlgResult(int result, FrForm* pForm);
	bool OnReplayModeOpenDlgResult(int result, FrForm* pForm);
	void OnFurnitureResetButtonUp(void);
	void OnFurnitureBackButtonUp(void);
	void OnFurnitureSaveButtonUp(void);
	void OnFurnitureOutButtonUp(void);
	void OnFurnitureTabMainInit(int param);
	void OnFurnitureListOwnerDraw(int param);
	void OnFurnitureListBtnDown(void);
	void OnBtn_MenuListInit(int param);
	void OnBtn_MenuListOwnerDraw(int param);
	void FurnitureDisplayControl(bool bShow);
	void Furniture_ShowUI(bool bShow);
	void OnRightMenuLBtnDown(void);
	void FurnitureUse(void);
	void FurnitureRun(void);
	void FurnitureFunctionChange(void);
	bool OnFurniture_AbilityDlgResult(int result, FrForm* pForm);
	bool OnAuthDlgResult(int result, FrForm* pForm);
	void LoadCardImages(void);
	void DeleteCardImages(void);
	void DrawPartsSlot(FrGraphicInterface* pGI, FrListItem* pItem);
	bool OnCardDlgResult(int result, FrForm* pForm);
	bool OnCardPackDlgResult(int result, FrForm* pForm);
	bool OnItemAttachCardDlgResult(int result, FrForm* pForm);
	void OpenNewRandomBox(sItemInfo* pItemInfo);
	bool OnOpenNewRandomBoxDlgResult(int result, FrForm* pForm);
	void OnInitItemStorageBGI(int param);
	void OnInitItemStorageList(int param);
	void OnLBDownItemStorageList(void);
	void OnRBDownItemStorageList(void);
	void DeleteCutinImages(void);
	bool OnRentalExtensionDlgResult(int result, FrForm* pForm);

public:
	bool GetArrangement(unsigned long typeId, unsigned long serial);
	void RMR_UpdateDB(void);

private:
	void ExecuteMiniButtonDown(sItemInfo* pItemInfo);
	friend class CRealMyRoom_Avatar;   // RealMyRoom_Avatar Render reads m_shadowTexture

private:
	LightSet m_lightSet;               // 0x1e8, the room's light (OnLoad)
	WTL::CString m_bgName;             // 0x204, LoadBg's map name
	bool m_entryFlag;                  // 0x208, set by message 0x16 (MakeRealMyRoom sends 1), read back by message 0x17
	bool m_decalShown;                 // 0x209, Display draws the decal under the avatar
	UNKNOWN(0x20A, 0x20C);
	int m_decalTexture;                // 0x20c, the texture of that decal
	FrWnd* m_pRightMenuWnd;            // 0x210, hidden by OnRightMenuRBtnDown
	WVector m_lookAtOffset;            // 0x214, its y is clamped to [-25, 23] and added to the free camera's look-at height (ProcessCamera)
	WVector m_avatarViewPos;           // 0x220, the local avatar's position raised 4.6 and moved back 46 (OnInit)
	float m_fovDeg;                    // 0x22c, the free camera's field of view in degrees, eased toward m_fovGoalDeg (ProcessCamera)
	float m_fovGoalDeg;                // 0x230
	float m_fovBaseDeg;                // 0x234, 28.8 like m_fovDeg after construction
	float m_cameraBlend;               // 0x238, 1.0 after construction
	float m_cameraHeight;              // 0x23c, 4.6 after construction
	float m_cameraHeightGoal;          // 0x240, 4.6, or 1.9 while the avatar is in state 2 or 3 (OnProcess)
	float m_cameraIdleTime;            // 0x244, accumulates the elapsed time (OnProcess), reset by ProcessCamera
	int m_cameraFlag;                  // 0x248, cleared by the constructor, use not recovered
	WVector m_headingLookAtOffset;     // 0x24c, m_lookAtOffset copied after each camera turn (OnProcess)
	bool m_topViewMode;                // 0x258, toggled by ToggleTopViewMode
	UNKNOWN(0x259, 0x25C);
	float m_cameraFov;                 // 0x25c, 70.0 when the top view / type 3 is set
	UNKNOWN(0x260, 0x264);
	WVector m_cameraOffset;            // 0x264, WVector::ZERO for the top view
	float m_decalPulse;                // 0x270, the decal's pulse phase (scaled by cos(phase * 10))
	float m_lookAtYMinDeg;             // 0x274, the custom clamp of m_lookAtOffset.y in degrees (ProcessCamera)
	float m_lookAtYMaxDeg;             // 0x278
	float m_lookAtXMinDeg;             // 0x27c, the custom clamp of m_lookAtOffset.x in degrees
	float m_lookAtXMaxDeg;             // 0x280
	int m_customLookAtLimits;          // 0x284, nonzero: ProcessCamera clamps with the four limits above
	WTitleFont* m_pTitleFont;          // 0x288, released by the destructor
	UNKNOWN(0x28C, 0x2B8);
	int m_pickIndex;                   // 0x2b8, -1 after construction, use not recovered
	int m_shadowTexture;               // 0x2bc, the shadow texture handle OnLoad loads
public:
	CFxSequence* m_pRoomFx;            // 0x2c0, the room's effect sequence OnLoad opens; avatars pause it when hidden
private:
	bool m_dialogsReady;               // 0x2c4, set by InitMyRoomDialogs
	UNKNOWN(0x2C5, 0x2C8);
	WVector m_cameraDropOrigin;        // 0x2c8, in front of its z the free camera drops by the distance (ProcessCamera)
	WVector m_eyePos;                  // 0x2d4, the eye position SetCameraDisplayType picks
	float m_eyeFov;                    // 0x2e0, the field of view going with it
	FrWnd* m_pMiddleUIWnd;             // 0x2e4, faded by SetMiddleUIVisible
	FrStatic* m_pSelectedName;         // 0x2e8
	FrWnd* m_pItemStatusWnd;           // 0x2ec, enabled by TurnOnItemStatus
	FrViewer* m_pStatAllViewer;        // 0x2f0, the viewer of the all-stat tab
	FrTabButton* m_pMainTab;           // 0x2f4, selects m_mainTab (ChangeMainTab)
	FrTabButton* m_pSubTab;            // 0x2f8, read by OnTabSubButtonUp
	FrWnd* m_pSubTabWndA;              // 0x2fc, use not recovered; cleared by InitMyRoomControls
	FrWnd* m_pSubTabWndB;              // 0x300, use not recovered; cleared by InitMyRoomControls
	FrListBox* m_pUccCopyHideWnd;      // 0x304, shown again when the UCC copy dialog closes; IsInItemList searches its items
	FrArea* m_pTabTxt[3];              // 0x308, the text areas of the three tabs
	FrWnd* m_pPetArea;                 // 0x314, one of m_pPetAreaWnd (SetPetAreaRect)
	FrWnd* m_pSelFrameWnd;             // 0x318, use not recovered; cleared by InitMyRoomControls
	FrArea* m_pSelFrame;               // 0x31c
	FrWnd* m_pSelItemWnd;              // 0x320, its rect frames DrawSelItem's icon
	FrButton* m_pItemSelectWnd;        // 0x324, blinks (OnSelItemInit)
	FrWnd* m_pStatWnd;                 // 0x328
	FrGaugeBar* m_pPowerBar;           // 0x32c
	FrGaugeBar* m_pControlBar;         // 0x330
	FrGaugeBar* m_pImpactBar;          // 0x334
	FrGaugeBar* m_pSpinBar;            // 0x338
	FrGaugeBar* m_pCurveBar;           // 0x33c
	FrStatic* m_pPowerEdit;            // 0x340
	FrStatic* m_pControlEdit;          // 0x344
	FrStatic* m_pImpactEdit;           // 0x348
	FrStatic* m_pSpinEdit;             // 0x34c
	FrStatic* m_pCurveEdit;            // 0x350
	FrButton* m_pSelItemPrevButton;    // 0x354, push delay 0.1 (OnSelItemPrevInit)
	FrButton* m_pSelItemNextButton;    // 0x358, push delay 0.1 (OnSelItemNextInit)
	FrArea* m_pItemDescBg;             // 0x35c
	FrEdit* m_pItemDescEdit;           // 0x360, cleared by SetItemDesc
	FrViewer* m_pItemDescBg2;          // 0x364, hidden at init (OnItemDescBgInit2)
	FrEdit* m_pItemDescEdit2;          // 0x368, hidden at init (OnItemDescInit2)
	FrEdit* m_pItemNameEdit;           // 0x36c, hidden at init (OnItemNameInit)
	FrWnd* m_pPetAreaWnd[3];           // 0x370, the pet areas SetPetAreaRect chooses from
	FrListBox* m_pItemSlotList;        // 0x37c, its items carry the equipped item ids (OnItemSlotDClick)
	FrButton* m_pFurnitureArrangeButton;   // 0x380, hidden at init (OnFurnitureArrangeInit)
	FrGroupBox* m_pItemSlotBox;        // 0x384, hidden at init, an item storage background UI
	FrButton* m_pEquipCancelButton;    // 0x388, hover enabled at init, an item storage background UI
	FrButton* m_pEquipResetButton;     // 0x38c, hover enabled at init, an item storage background UI
	FrButton* m_pUpgradeButton;        // 0x390, hidden at init, hover enabled
	FrButton* m_pDowngradeButton;      // 0x394, hidden at init, hover enabled
	FrWnd* m_pCaddieInfoWnd;           // 0x398, re-enabled when the caddie info dialog closes with 2, disabled again when the caddie's level is above the player's
	FrViewer* m_pGiftBgViewer;         // 0x39c, hidden and without mouse events at init (OnGiftBgInit)
	FrEdit* m_pGiftDescEdit;           // 0x3a0, the description of the selected gift (SetGiftDescString)
	FrArea* m_pStatFrame;              // 0x3a4
	FrArea* m_pStatText;               // 0x3a8
	FrTabButton* m_pFurnitureMainTab;  // 0x3ac, read by OnFurnitureTabMainButtonUp
	FrListBox* m_pFurnitureList;       // 0x3b0, multi-select except on furniture main tabs 1-3 (ChangeFMainTab)
	FrListBox* m_pMenuList;            // 0x3b4, its items carry the menu number + 1
	FrButton* m_pFurniturePageUpButton;     // 0x3b8, hidden at init
	FrButton* m_pFurniturePageDownButton;   // 0x3bc, hidden at init
	FrEdit* m_pFurnitureListNumWnd;    // 0x3c0, the current furniture page
	FrEdit* m_pFurniturePageCountWnd;  // 0x3c4, the furniture page count
	FrButton* m_pFurnitureListNumButton;    // 0x3c8, hidden at init
	FrButton* m_pFurnitureResetButton;      // 0x3cc, hidden at init
	FrButton* m_pFurnitureBackButton;       // 0x3d0, hidden at init
	FrButton* m_pFurnitureSaveButton;       // 0x3d4, hidden at init
	FrButton* m_pFurnitureOutButton;        // 0x3d8, hidden at init
	FrArea* m_pFurnitureBackGround[8];      // 0x3dc, hidden at init
	FrArea* m_pFurnitureScroll[3];          // 0x3fc, hidden at init
	FrWnd* m_pFurnitureCtrl[6];        // 0x408, use not recovered; cleared one by one by InitMyRoomControls
	FrButton* m_pBtnMenu;              // 0x420
	FrForm* m_pMenuDlg;                // 0x424, cleared by InitMyRoomDialogs
	FrEnchantDlg* m_pEnchantDlg;       // 0x428
	FrDowngradeDlg* m_pUpgradeDlg;     // 0x42c, the "downgrade" form OnUpgradeUp opens
	FrDowngradeDlg* m_pDowngradeDlg;   // 0x430, cleared by InitMyRoomDialogs
	FrPangGiftDlg* m_pPangGiftDlg;     // 0x434, cleared by OnPangGiftDlgResult
	FrMascotInfoDlg* m_pMascotInfoDlg; // 0x438
	FrNoteDlg* m_pNoteDlg;             // 0x43c
	FrFortuneDlg* m_pFortuneDlg;       // 0x440, cleared by OnFortuneDlgResult
	FrFortuneOpenDlg* m_pGiftOpenDlg;  // 0x444, the open-the-gift dialog OnFortuneOpenDlgResult closes; cleared by InitMyRoomDialogs
	FrReplayModeDlg* m_pReplayDelDlg;  // 0x448, the "replaymode" / "replaytape_del" form (OnItemListBtnDown)
	FrCardUseDlg* m_pCardUseDlg;                 // 0x44c, "usecard", cleared by OnCardDlgResult and InitMyRoomDialogs
	FrItemAttachCardDlg* m_pItemAttachCardDlg;   // 0x450, "itemattachcarddlg", created by OnCardDlgResult
	FrOpenCardPackDlg* m_pOpenCardPackDlg;       // 0x454, "cardpack_open", cleared by OnCardPackDlgResult
	UNKNOWN(0x458, 0x45C);
	FrUccDrawDlg* m_pUccDrawDlg;       // 0x45c
	FrUccCopyDlg* m_pUccCopyDlg;       // 0x460, the "ucc_copy" form (OnItemListBtnDown)
	FrUniteResultDlg* m_pReportDlg;    // 0x464, the "uniteresult" tiki report (HandleMsg 0x205), cleared by OnResultDlg_Report
	FrForm* m_pCardRemoveDlg;          // 0x468
	UNKNOWN(0x46C, 0x474);
	CPost_ReciveDlg* m_pPostDlg;       // 0x474, the "postbox" form
	CPost_SendDlg* m_pPostSendDlg;     // 0x478, the "write_post" form OnPostSendDlg2Result opens
	CRealMyRoom_AuthorityDlg* m_pAuthorityDlg;   // 0x47c, OnAuthDlgResult reads the setting back; cleared by InitMyRoomDialogs
	FrForm* m_pFurnitureAbilityDlg;    // 0x480, the "myitem_choice" CFurniture_AbilityDlg (FurnitureUse); cleared by InitMyRoomDialogs when local content 0x76 is on
	// OnCardPackListDraw's item frame: normal, under the cursor, selected, empty slot.
	const Bitmap* m_cardPackItemBmp[4]; // 0x484
	UNKNOWN(0x494, 0x4A0);
	// DrawStatus's stat points per stat: [0] the item's own point, [1] a slot's point.
	const Bitmap* m_pStatPointBitmap[5][2]; // 0x4a0
	// The item select list's background (DrawCharacterList draws [0]; the Aztec and club lists draw all three).
	const Bitmap* m_pListBgBitmap[3]; // 0x4c8
	const Bitmap* m_pListEmptyBitmap; // 0x4d4, the Aztec and club lists' empty cell filling their last row; cleared by InitMyRoomControls
	const Bitmap* m_pListSelBitmap;    // 0x4d8, the item select list's frame around the equipped entry
	const Bitmap* m_pListOverBitmap;   // 0x4dc, the item select list's frame around the hovered entry
	const Bitmap* m_pStatPartBitmap[3][3]; // 0x4e0, OnStatPartOwnerDraw's tab image per m_statSource: normal, hovered, selected
	const Bitmap* m_pStatAllBitmap[3]; // 0x504, OnStatAllOwnerDraw's tab image: normal, hovered, selected
	sSelectIcon m_selectIcons[5];      // 0x510, "select_1_n" .. "select_5_o"
	const Bitmap* m_pSelectDivide;     // 0x54c, "select_divide"
	const Bitmap* m_pSkinLevelBitmap[2]; // 0x550, DrawSkinSlot's level frame, indexed by the user's event flag bit 1
	const Bitmap* m_pSkinNameBitmap;   // 0x558, DrawSkinSlot's frame under the nickname; cleared by InitMyRoomControls
	const Bitmap* m_pSkinSlotBitmap;   // 0x55c, DrawSkinSlot's slot background; cleared by InitMyRoomControls
	const Bitmap* m_pItemSlotBitmap[3];    // 0x560, OnItemSlotOwnerDraw's slot frame: normal, hovered, selected
	const Bitmap* m_pItemSlotEmpty;    // 0x56c, "item_slot_empty"
	Bitmap* m_pItemDescBottom[2];     // 0x570, drawn under the item description at (+6, +320) and (+262, +320) (OnItemDescBgOwnerDraw)
	UNKNOWN(0x578, 0x588);
	// OnItemDescBgOwnerDraw2's frame around the second item description: the top row (left, middle, right),
	// the middle row and the bottom row, three each.
	const Bitmap* m_pItemDescFrame[9]; // 0x588
	// OnGiftPageOwnerDraw's page bar: the previous-group arrow, the bar's left end, a page button (the
	// bar's middle), the bar's right end and the next-group arrow.
	const Bitmap* m_pGiftPageBitmap[5]; // 0x5ac
	const Bitmap* m_pUccStateBitmap[3]; // 0x5c0, "ucc_state_original", "ucc_state_copied", "ucc_state_temp_saved"
	const Bitmap* m_pMenuIcon[13];     // 0x5cc, the menu list's icons ("rmr_f_wardrobe", "rmr_f_post", ...), by menu number - 1
	const Bitmap* m_pMenuText[13];     // 0x600, the menu list's captions ("rmr_f_wardrobe_text", ...), drawn when hovered or selected
	const Bitmap* m_pMenuOver;         // 0x634, "rmr_menu_o", the hovered menu frame
	const Bitmap* m_pMenuDown;         // 0x638, "rmr_menu_d", the selected menu frame
	UNKNOWN(0x63C, 0x640);
	const Bitmap* m_pFurnitureItemBg;  // 0x640, OnFurnitureListOwnerDraw's frame of a filled slot
	const Bitmap* m_pFurnitureItemDownBg; // 0x644, "rmr_arg_item_base_d"
	const Bitmap* m_pFurnitureSelFrame; // 0x648, drawn over a selected furniture slot
	const Bitmap* m_pFurnitureEmptyBg; // 0x64c, the frame of an empty furniture slot
	const Bitmap* m_pShopMarkBitmap;   // 0x650, DrawPartsSlot's shop-mark overlay (at +5, +65); cleared by InitMyRoomVariables
	FrWnd* m_pFurnitureWndA;           // 0x654, use not recovered; cleared by InitMyRoomControls
	FrWnd* m_pFurnitureWndB;           // 0x658, use not recovered; cleared by InitMyRoomControls
	UNKNOWN(0x65C, 0x660);
	FrWnd* m_pFurnitureWndC;           // 0x660, use not recovered; cleared by InitMyRoomControls
	FrWnd* m_pFurnitureWndD;           // 0x664, use not recovered; cleared by InitMyRoomControls
	FrWnd* m_pFurnitureWndE;           // 0x668, use not recovered; cleared by InitMyRoomControls
	FrWnd* m_pFurnitureWndF;           // 0x66c, use not recovered; cleared by InitMyRoomControls
	FrWnd* m_pFurnitureLeftWnd[5];     // 0x670, enabled together by Furniture_ShowUI_Left
	FrWnd* m_pEffectViewWnd;           // 0x684, framed by OnEffectViewOnwnerDraw
	FrWnd* m_pEffectViewWnd2;          // 0x688, use not recovered; cleared by InitMyRoomControls
	UNKNOWN(0x68C, 0x694);
	FrWnd* m_pLayoutWnd;               // 0x694, closed by CloseLayout
	WPoint m_infoDlgPos;               // 0x698, the clicked item's position minus (300, 450): where OpenInformation(FrListItem*) puts the dialog
	WPoint m_itemDescEdit2InitPos;     // 0x6a0, m_itemDescEdit2Pos as OnItemDescInit2 found it
	std::map<unsigned long, const Bitmap*> m_itemIconMap;   // 0x6a8, GetItemIcon's cache of registered icons by type id
	std::map<unsigned int, WOverlay*> m_cardImgMap;    // 0x6b4, GetCardImg's cache of card images by type id
	std::map<unsigned int, WOverlay*> m_cutinImgMap;   // 0x6c0, GetCutinImg's cache of skin cut-in images by type id
	UNKNOWN(0x6CC, 0x6D8);
	bool m_selItemOn;                  // 0x6d8
	UNKNOWN(0x6D9, 0x6DC);
	const char* m_pCharLevels;        // 0x6dc, the shown character's stat levels (GetCharacterStatus)
	const unsigned long* m_pCharParts; // 0x6e0, the shown character's equipped parts (GetCharacterStatus)
	unsigned long m_localContentData[24];   // 0x6e4, use not recovered; zeroed by InitMyRoomVariables when local content 0x57 is on
	unsigned long* m_pCharCards;       // 0x744, the shown character's equipped cards (GetCharacterStatus)
	unsigned long m_charTypeId;        // 0x748, the shown character's type id (GetCharacterStatus)
	unsigned long m_charSerial;        // 0x74c, the equipped character's id (DrawCharacterList frames it); cleared by InitMyRoomVariables
	unsigned long m_caddieTypeId;      // 0x750, the caddie whose stat bonus GetCaddieStatus adds
	unsigned long m_caddieValueA;      // 0x754, use not recovered; cleared by InitMyRoomVariables
	unsigned long m_caddieValueB;      // 0x758, use not recovered; cleared by InitMyRoomVariables
	unsigned long m_mascotTypeId;      // 0x75c, the shown mascot's type id (ChangeMascot)
	unsigned long m_mascotSerial;      // 0x760, the shown mascot's item id (ChangeMascot)
	unsigned int m_clubSerial;         // 0x764, the shown club set's item serial (GetClubSetStatus looks up its upgrade levels)
	unsigned long m_clubTypeId;        // 0x768, the shown club set's type id
	const short* m_pClubLevels;        // 0x76c, the shown club set's upgrade levels (GetTotalStatus)
	unsigned long m_aztecTypeId;       // 0x770, the shown aztec (ball) type id (ChangeAztec)
	statTab_t m_statTab;               // 0x774
	mainTab_t m_mainTab;               // 0x778, reselected by SetMiddleUIVisible
	int m_roomMode;                    // 0x77c, 13-15 and 16 select their own pet area (SetPetAreaRect)
	selTab_t m_selTab;                 // 0x780
	bool m_controlFlag;                // 0x784, use not recovered; cleared by InitMyRoomControls
	UNKNOWN(0x785, 0x788);
	WPoint m_itemDescPos;              // 0x788, set per room mode by SetItemDesc
	WPoint m_itemDescEditPos;          // 0x790, the bottom-left corner of m_pItemDescEdit (OnItemDescInit)
	WPoint m_itemDescEdit2Pos;         // 0x798, the bottom-left corner of m_pItemDescEdit2 (OnItemDescInit2)
	eFurniture_Tab m_furnitureMainTab; // 0x7a0, ChangeFMainTab's tab
	int m_furniturePage;               // 0x7a4, reset to 0 when the furniture main tab changes
	int m_furniturePageCount;          // 0x7a8, the furniture page is clamped below it
	sGiftInfo* m_pMoveGift;            // 0x7ac, the gift MoveGift asks about; OnMoveGiftDlgResult sends its serial and type id
	unsigned long m_newYearMoneySerial;   // 0x7b0, the serial OpenNewYearMoney opens
	unsigned long m_eventGiftBoxSerial;   // 0x7b4, the serial OpenEventGiftBox opens
	unsigned long m_xmasGiftBoxSerial;   // 0x7b8, the serial OpenEventXmasGiftBox opens
	unsigned long m_s4EventGiftBoxTypeId; // 0x7bc, the type OpenS4EventGiftBox opens
	UNKNOWN(0x7C0, 0x7D0);
	unsigned long m_tikiReportSerial;  // 0x7d0, the serial of the tiki report OpenTikiReport opens
	unsigned long m_tikiReportTypeId;  // 0x7d4, its type id
	int m_tikiReportRecord;            // 0x7d8, its record words packed (word 0xE << 15) + word 0x10
	CExhibition* m_pPetExhibition;     // 0x7dc, deleted by HidePet
	CAnotherHand m_petLoader;          // 0x7e0, HidePet waits for its work
	std::map<unsigned int, sCharacterInfo> m_characterMap;   // 0x968, a copy of CSharedDoc's owned characters (OnInit)
	unsigned long m_caddieId;         // 0x974, the caddie Net_EquipCaddie equips when it is not the current one
	unsigned long m_characterId;       // 0x978, the character Net_EquipCharacter equips when it is not the current one
	unsigned long m_clubId;           // 0x97c, the club set Net_EquipGear sends
	unsigned long m_ballId;           // 0x980, the ball Net_EquipGear sends
	unsigned long m_equipItem[10];     // 0x984, the edited equipped items, sent by Net_EquipItem when they differ from CSharedDoc's
	UNKNOWN(0x9AC, 0x9C4);
	unsigned long m_equipSkin[6];      // 0x9c4, the edited equipped skins, sent by Net_EquipSkin when they differ from CSharedDoc's
	unsigned long m_mascotId;          // 0x9dc, the mascot Net_EquipMascot equips when it is not the current one
	sItemListArea m_itemListArea[14]; // 0x9e0, the item list's no-right-button areas (OnItemListInit)
	std::list<sSpecialTrophy> m_initList;  // 0xb68, emptied by OnMyRoomInit
	int m_statSource;                  // 0xb74, what SetLevelBar showed last: 0 the character, 1 the club set, 2 the caddie
	unsigned long m_displaySkin[6];    // 0xb78, copy of CSharedDoc::m_equipSkin made by Reinitialize
	float m_skinSlot[2];               // 0xb90, eased by OnProcess per m_skinParam, cleared by Reinitialize
	int m_skinParam[2];                // 0xb98, the skin's bytes 0xB8 and 0xB9
	unsigned long m_cardTypeId;        // 0xba0, the card DrawCard shows
	unsigned int m_giftPageCount;      // 0xba4, the gift list's page count (OnGiftPageOwnerDraw)
	unsigned int m_giftPageShownCount; // 0xba8, how many page buttons the page bar shows (at most 5)
	unsigned int m_giftPageCurrent;    // 0xbac, the selected page button, 1-based within the group
	unsigned int m_giftPageGroup;      // 0xbb0, the shown group of 5 pages (first page = group * 5 + 1)
	WRect m_areaRects[7];              // 0xbb4, the page bar's clickable rects: the previous arrow, the page buttons, the next arrow
	bool m_areaFlagA;                  // 0xc24, use not recovered; false after InitMyRoomVariables
	bool m_areaFlagB;                  // 0xc25, use not recovered; true after InitMyRoomVariables
	bool m_areaFlagC;                  // 0xc26, use not recovered; false after InitMyRoomVariables
	PADDING(0xC27, 0xC28);
	WRect m_uccDrawRects[2];           // 0xc28, sized after "ucc_draw_n" at (103, 66) and (144, 66) (LoadMyRoomImages)
	WPolySoup::sCamera* m_pNamedCameras[2]; // 0xc48, two more cameras OnLoad finds by name
	// The named cameras of the room; SetCameraDisplayType cycles 2..5.
	WPolySoup::sCamera* m_pCameras[7]; // 0xc50
	WPolySoup::sPoint* m_pRoomPoint;   // 0xc6c, the point OnLoad finds by name
	float m_savedClipNear;             // 0xc70, g_view's near clip at construction, restored by ProcessCamera
	float m_savedClipFar;              // 0xc74, its far clip
	// Two debug segments OnDisplay draws in white.
	WVector m_debugLineFrom;           // 0xc78
	WVector m_debugLineTo;             // 0xc84
	WVector m_debugLineMid;            // 0xc90, halfway up the first segment (OnLoad)
	WVector m_debugLine2From;          // 0xc9c
	WVector m_debugLine2To;            // 0xca8
	WVector m_debugLine2Mid;           // 0xcb4, halfway up the second segment (OnLoad)
	WVector m_debugLine3From;          // 0xcc0, a third segment in the middle of the room's box (OnLoad)
	WVector m_debugLine3To;            // 0xccc
	WVector m_lookAt;                  // 0xcd8, the point the free camera (type 0) looks at, following the avatar
	bool m_lookAtMoving;               // 0xce4, the look-at catches up with the avatar
	UNKNOWN(0xCE5, 0xCE8);
	int m_cameraDisplayType;           // 0xce8, the type SetCameraDisplayType stores
	WPolySoup::sCamera m_savedCamera;  // 0xcec, the camera SetCameraDisplayType(0) restores
	bool m_uiFlagA;                    // 0xd44, use not recovered; false after InitMyRoomControls
	bool m_bHideMiddleUI;              // 0xd45, set by ControlUiHideMode
	bool m_uiFlagB;                    // 0xd46, use not recovered; false after InitMyRoomControls
	UNKNOWN(0xD47, 0xD4C);
	unsigned long m_uiState;           // 0xd4c, the UI state bits SetUIState toggles
	unsigned long m_prevUiState;       // 0xd50, the m_uiState ControlUiHideMode replaced
	// The UI flags of ChangeFlags/GetFlags: flag 1, 2, 4 and 8 select
	// elements 0-3.
	bool m_flags[5];                   // 0xd54
	UNKNOWN(0xD59, 0xD5C);
	int m_offlineShopEditState;        // 0xd5c, 1 once the offline shop edit message is confirmed
	WOverlay* m_pGenderIcons[6];       // 0xd60, the male/female icons OnLoad loads ("i_male.tga", ...)
	WOverlay* m_pMannerIcons[2];       // 0xd78, "i_male_manner.tga", "i_female_manner.tga"
	WOverlay* m_pAngelIcons[2];        // 0xd80, "i_male_angel.tga", ...
	bool m_cardBookVisible;            // 0xd88, the state SetCardBookVisible gave the card book
	UNKNOWN(0xD89, 0xD8C);
	CCardBook* m_pCardBook;            // 0xd8c
	FrItemStorageNotice* m_pItemStorageNotice;   // 0xd90
	FrItemStorageAccessPanel* m_pItemStorageAccessPanel;   // 0xd94, cleared by OnResultItemStorageAccessPanel
	bool m_itemStorageVisible;         // 0xd98
	UNKNOWN(0xD99, 0xD9C);
	WPoint m_itemStorageDlgPos;        // 0xd9c, beside the item storage dialog; InitMyRoomDialogs sets (0, 0), no other member reads it
	FrForm* m_pItemStorageDlg;         // 0xda4, cleared by InitMyRoomDialogs
	FrArea* m_pItemStoragePang;        // 0xda8, a front UI of the item storage
	FrButton* m_pItemStoragePangMinus; // 0xdac, a front UI of the item storage
	FrButton* m_pItemStoragePangPlus; // 0xdb0, a front UI of the item storage
	FrListBox* m_pItemStorageList;   // 0xdb4
	FrListBox* m_pItemStorageIndexMsg; // 0xdb8, a front UI of the item storage
	FrButton* m_pItemStoragePageNext; // 0xdbc, push delay 0.1, hidden at init
	FrButton* m_pItemStoragePagePrev; // 0xdc0, push delay 0.1, hidden at init
	FrButton* m_pItemStorageClose;    // 0xdc4, a front UI of the item storage
	FrArea* m_pItemStorageDragDrop;    // 0xdc8, a front UI of the item storage
	FrListBox* m_pCardPackListBox;     // 0xdcc, shown only on the card pack tab of the card book

	LAYOUT_CHECK_FRIEND;
};
BEGIN_LAYOUT_CHECK(CRealMyRoomMain)
	CHECK_FIELD(CRealMyRoomMain, m_bgName, 0x204);
	CHECK_FIELD(CRealMyRoomMain, m_entryFlag, 0x208);
	CHECK_FIELD(CRealMyRoomMain, m_lookAtOffset, 0x214);
	CHECK_FIELD(CRealMyRoomMain, m_fovDeg, 0x22C);
	CHECK_FIELD(CRealMyRoomMain, m_fovGoalDeg, 0x230);
	CHECK_FIELD(CRealMyRoomMain, m_cameraDropOrigin, 0x2C8);
	CHECK_FIELD(CRealMyRoomMain, m_savedClipNear, 0xC70);
	CHECK_FIELD(CRealMyRoomMain, m_savedClipFar, 0xC74);
	CHECK_FIELD(CRealMyRoomMain, m_lookAt, 0xCD8);
	CHECK_FIELD(CRealMyRoomMain, m_lookAtMoving, 0xCE4);
	CHECK_FIELD(CRealMyRoomMain, m_pRightMenuWnd, 0x210);
	CHECK_FIELD(CRealMyRoomMain, m_pMainTab, 0x2F4);
	CHECK_FIELD(CRealMyRoomMain, m_pShopMarkBitmap, 0x650);
	CHECK_FIELD(CRealMyRoomMain, m_localContentData, 0x6E4);
	CHECK_FIELD(CRealMyRoomMain, m_charSerial, 0x74C);
	CHECK_FIELD(CRealMyRoomMain, m_caddieValueA, 0x754);
	CHECK_FIELD(CRealMyRoomMain, m_mascotTypeId, 0x75C);
	CHECK_FIELD(CRealMyRoomMain, m_mascotSerial, 0x760);
	CHECK_FIELD(CRealMyRoomMain, m_giftPageCount, 0xBA4);
	CHECK_FIELD(CRealMyRoomMain, m_giftPageShownCount, 0xBA8);
	CHECK_FIELD(CRealMyRoomMain, m_giftPageCurrent, 0xBAC);
	CHECK_FIELD(CRealMyRoomMain, m_giftPageGroup, 0xBB0);
	CHECK_FIELD(CRealMyRoomMain, m_areaRects, 0xBB4);
	CHECK_FIELD(CRealMyRoomMain, m_areaFlagA, 0xC24);
	CHECK_FIELD(CRealMyRoomMain, m_areaFlagC, 0xC26);
	CHECK_FIELD(CRealMyRoomMain, m_pSubTabWndA, 0x2FC);
	CHECK_FIELD(CRealMyRoomMain, m_pSubTabWndB, 0x300);
	CHECK_FIELD(CRealMyRoomMain, m_pSelFrameWnd, 0x318);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureCtrl, 0x408);
	CHECK_FIELD(CRealMyRoomMain, m_pListBgBitmap, 0x4C8);
	CHECK_FIELD(CRealMyRoomMain, m_pListEmptyBitmap, 0x4D4);
	CHECK_FIELD(CRealMyRoomMain, m_pListSelBitmap, 0x4D8);
	CHECK_FIELD(CRealMyRoomMain, m_pItemDescFrame, 0x588);
	CHECK_FIELD(CRealMyRoomMain, m_pGiftPageBitmap, 0x5AC);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureItemBg, 0x640);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureSelFrame, 0x648);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureEmptyBg, 0x64C);
	CHECK_FIELD(CRealMyRoomMain, m_pListOverBitmap, 0x4DC);
	CHECK_FIELD(CRealMyRoomMain, m_selectIcons, 0x510);
	CHECK_SIZE(CRealMyRoomMain::sSelectIcon, 0xC);
	CHECK_SIZE(CRealMyRoomMain::sItemListArea, 0x1C);
	CHECK_FIELD(CRealMyRoomMain, m_pSelectDivide, 0x54C);
	CHECK_FIELD(CRealMyRoomMain, m_pItemSlotEmpty, 0x56C);
	CHECK_FIELD(CRealMyRoomMain, m_pUccStateBitmap, 0x5C0);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureItemDownBg, 0x644);
	CHECK_FIELD(CRealMyRoomMain, m_uccDrawRects, 0xC28);
	CHECK_FIELD(CRealMyRoomMain, m_pSkinLevelBitmap, 0x550);
	CHECK_FIELD(CRealMyRoomMain, m_pSkinNameBitmap, 0x558);
	CHECK_FIELD(CRealMyRoomMain, m_pSkinSlotBitmap, 0x55C);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureWndA, 0x654);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureWndC, 0x660);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureWndF, 0x66C);
	CHECK_FIELD(CRealMyRoomMain, m_pEffectViewWnd2, 0x688);
	CHECK_FIELD(CRealMyRoomMain, m_controlFlag, 0x784);
	CHECK_FIELD(CRealMyRoomMain, m_uiFlagA, 0xD44);
	CHECK_FIELD(CRealMyRoomMain, m_uiFlagB, 0xD46);
	CHECK_FIELD(CRealMyRoomMain, m_avatarViewPos, 0x220);
	CHECK_FIELD(CRealMyRoomMain, m_pTitleFont, 0x288);
	CHECK_FIELD(CRealMyRoomMain, m_topViewMode, 0x258);
	CHECK_FIELD(CRealMyRoomMain, m_cameraFov, 0x25C);
	CHECK_FIELD(CRealMyRoomMain, m_cameraOffset, 0x264);
	CHECK_FIELD(CRealMyRoomMain, m_pRoomFx, 0x2C0);
	CHECK_FIELD(CRealMyRoomMain, m_pMiddleUIWnd, 0x2E4);
	CHECK_FIELD(CRealMyRoomMain, m_pItemStatusWnd, 0x2EC);
	CHECK_FIELD(CRealMyRoomMain, m_pSubTab, 0x2F8);
	CHECK_FIELD(CRealMyRoomMain, m_pUccCopyHideWnd, 0x304);
	CHECK_FIELD(CRealMyRoomMain, m_pPetArea, 0x314);
	CHECK_FIELD(CRealMyRoomMain, m_pSelItemWnd, 0x320);
	CHECK_FIELD(CRealMyRoomMain, m_pItemSelectWnd, 0x324);
	CHECK_FIELD(CRealMyRoomMain, m_pStatWnd, 0x328);
	CHECK_FIELD(CRealMyRoomMain, m_pSelItemPrevButton, 0x354);
	CHECK_FIELD(CRealMyRoomMain, m_pSelItemNextButton, 0x358);
	CHECK_FIELD(CRealMyRoomMain, m_pPetAreaWnd, 0x370);
	CHECK_FIELD(CRealMyRoomMain, m_pItemSlotList, 0x37C);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureListNumWnd, 0x3C0);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureMainTab, 0x3AC);
	CHECK_FIELD(CRealMyRoomMain, m_pMenuList, 0x3B4);
	CHECK_FIELD(CRealMyRoomMain, m_pEnchantDlg, 0x428);
	CHECK_FIELD(CRealMyRoomMain, m_pUpgradeDlg, 0x42C);
	CHECK_FIELD(CRealMyRoomMain, m_pPangGiftDlg, 0x434);
	CHECK_FIELD(CRealMyRoomMain, m_pMascotInfoDlg, 0x438);
	CHECK_FIELD(CRealMyRoomMain, m_pReplayDelDlg, 0x448);
	CHECK_FIELD(CRealMyRoomMain, m_pUccDrawDlg, 0x45C);
	CHECK_FIELD(CRealMyRoomMain, m_pUccCopyDlg, 0x460);
	CHECK_FIELD(CRealMyRoomMain, m_pReportDlg, 0x464);
	CHECK_FIELD(CRealMyRoomMain, m_pCardRemoveDlg, 0x468);
	CHECK_FIELD(CRealMyRoomMain, m_pPostDlg, 0x474);
	CHECK_FIELD(CRealMyRoomMain, m_pPostSendDlg, 0x478);
	CHECK_FIELD(CRealMyRoomMain, m_pEffectViewWnd, 0x684);
	CHECK_FIELD(CRealMyRoomMain, m_pLayoutWnd, 0x694);
	CHECK_FIELD(CRealMyRoomMain, m_selItemOn, 0x6D8);
	CHECK_FIELD(CRealMyRoomMain, m_pCharLevels, 0x6DC);
	CHECK_FIELD(CRealMyRoomMain, m_pCharParts, 0x6E0);
	CHECK_FIELD(CRealMyRoomMain, m_pCharCards, 0x744);
	CHECK_FIELD(CRealMyRoomMain, m_charTypeId, 0x748);
	CHECK_FIELD(CRealMyRoomMain, m_infoDlgPos, 0x698);
	CHECK_FIELD(CRealMyRoomMain, m_cardImgMap, 0x6B4);
	CHECK_FIELD(CRealMyRoomMain, m_cutinImgMap, 0x6C0);
	CHECK_FIELD(CRealMyRoomMain, m_pCaddieInfoWnd, 0x398);
	CHECK_FIELD(CRealMyRoomMain, m_statTab, 0x774);
	CHECK_FIELD(CRealMyRoomMain, m_mainTab, 0x778);
	CHECK_FIELD(CRealMyRoomMain, m_cardPackItemBmp, 0x484);
	CHECK_FIELD(CRealMyRoomMain, m_roomMode, 0x77C);
	CHECK_FIELD(CRealMyRoomMain, m_selTab, 0x780);
	CHECK_FIELD(CRealMyRoomMain, m_furnitureMainTab, 0x7A0);
	CHECK_FIELD(CRealMyRoomMain, m_furniturePage, 0x7A4);
	CHECK_FIELD(CRealMyRoomMain, m_caddieTypeId, 0x750);
	CHECK_FIELD(CRealMyRoomMain, m_clubSerial, 0x764);
	CHECK_FIELD(CRealMyRoomMain, m_clubTypeId, 0x768);
	CHECK_FIELD(CRealMyRoomMain, m_pClubLevels, 0x76c);
	CHECK_FIELD(CRealMyRoomMain, m_aztecTypeId, 0x770);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureList, 0x3B0);
	CHECK_FIELD(CRealMyRoomMain, m_newYearMoneySerial, 0x7B0);
	CHECK_FIELD(CRealMyRoomMain, m_eventGiftBoxSerial, 0x7B4);
	CHECK_FIELD(CRealMyRoomMain, m_s4EventGiftBoxTypeId, 0x7BC);
	CHECK_FIELD(CRealMyRoomMain, m_tikiReportSerial, 0x7D0);
	CHECK_FIELD(CRealMyRoomMain, m_tikiReportTypeId, 0x7D4);
	CHECK_FIELD(CRealMyRoomMain, m_tikiReportRecord, 0x7D8);
	CHECK_FIELD(CRealMyRoomMain, m_pPetExhibition, 0x7DC);
	CHECK_FIELD(CRealMyRoomMain, m_petLoader, 0x7E0);
	CHECK_FIELD(CRealMyRoomMain, m_characterId, 0x978);
	CHECK_FIELD(CRealMyRoomMain, m_characterMap, 0x968);
	CHECK_FIELD(CRealMyRoomMain, m_caddieId, 0x974);
	CHECK_FIELD(CRealMyRoomMain, m_clubId, 0x97C);
	CHECK_FIELD(CRealMyRoomMain, m_ballId, 0x980);
	CHECK_FIELD(CRealMyRoomMain, m_equipSkin, 0x9C4);
	CHECK_FIELD(CRealMyRoomMain, m_mascotId, 0x9DC);
	CHECK_FIELD(CRealMyRoomMain, m_pGiftBgViewer, 0x39C);
	CHECK_FIELD(CRealMyRoomMain, m_itemListArea, 0x9E0);
	CHECK_FIELD(CRealMyRoomMain, m_initList, 0xB68);
	CHECK_FIELD(CRealMyRoomMain, m_statSource, 0xB74);
	CHECK_FIELD(CRealMyRoomMain, m_bHideMiddleUI, 0xD45);
	CHECK_FIELD(CRealMyRoomMain, m_prevUiState, 0xD50);
	CHECK_FIELD(CRealMyRoomMain, m_displaySkin, 0xB78);
	CHECK_FIELD(CRealMyRoomMain, m_skinSlot, 0xB90);
	CHECK_FIELD(CRealMyRoomMain, m_skinParam, 0xB98);
	CHECK_FIELD(CRealMyRoomMain, m_debugLineFrom, 0xC78);
	CHECK_FIELD(CRealMyRoomMain, m_debugLineTo, 0xC84);
	CHECK_FIELD(CRealMyRoomMain, m_debugLineMid, 0xC90);
	CHECK_FIELD(CRealMyRoomMain, m_debugLine2From, 0xC9C);
	CHECK_FIELD(CRealMyRoomMain, m_debugLine2To, 0xCA8);
	CHECK_FIELD(CRealMyRoomMain, m_debugLine2Mid, 0xCB4);
	CHECK_FIELD(CRealMyRoomMain, m_debugLine3From, 0xCC0);
	CHECK_FIELD(CRealMyRoomMain, m_debugLine3To, 0xCCC);
	CHECK_FIELD(CRealMyRoomMain, m_cameraDisplayType, 0xCE8);
	CHECK_FIELD(CRealMyRoomMain, m_decalShown, 0x209);
	CHECK_FIELD(CRealMyRoomMain, m_decalTexture, 0x20C);
	CHECK_FIELD(CRealMyRoomMain, m_decalPulse, 0x270);
	CHECK_FIELD(CRealMyRoomMain, m_eyePos, 0x2D4);
	CHECK_FIELD(CRealMyRoomMain, m_eyeFov, 0x2E0);
	CHECK_FIELD(CRealMyRoomMain, m_pCameras, 0xC50);
	CHECK_FIELD(CRealMyRoomMain, m_lightSet, 0x1E8);
	CHECK_FIELD(CRealMyRoomMain, m_fovBaseDeg, 0x234);
	CHECK_FIELD(CRealMyRoomMain, m_cameraFlag, 0x248);
	CHECK_FIELD(CRealMyRoomMain, m_lookAtYMinDeg, 0x274);
	CHECK_FIELD(CRealMyRoomMain, m_customLookAtLimits, 0x284);
	CHECK_FIELD(CRealMyRoomMain, m_pickIndex, 0x2B8);
	CHECK_FIELD(CRealMyRoomMain, m_pNamedCameras, 0xC48);
	CHECK_FIELD(CRealMyRoomMain, m_pRoomPoint, 0xC6C);
	CHECK_FIELD(CRealMyRoomMain, m_headingLookAtOffset, 0x24C);
	CHECK_FIELD(CRealMyRoomMain, m_savedCamera, 0xCEC);
	CHECK_FIELD(CRealMyRoomMain, m_uiState, 0xD4C);
	CHECK_FIELD(CRealMyRoomMain, m_flags, 0xD54);
	CHECK_FIELD(CRealMyRoomMain, m_offlineShopEditState, 0xD5C);
	CHECK_FIELD(CRealMyRoomMain, m_shadowTexture, 0x2BC);
	CHECK_FIELD(CRealMyRoomMain, m_pGenderIcons, 0xD60);
	CHECK_FIELD(CRealMyRoomMain, m_pMannerIcons, 0xD78);
	CHECK_FIELD(CRealMyRoomMain, m_pAngelIcons, 0xD80);
	CHECK_FIELD(CRealMyRoomMain, m_cardBookVisible, 0xD88);
	CHECK_FIELD(CRealMyRoomMain, m_pCardBook, 0xD8C);
	CHECK_FIELD(CRealMyRoomMain, m_pItemStorageNotice, 0xD90);
	CHECK_FIELD(CRealMyRoomMain, m_pItemStorageAccessPanel, 0xD94);
	CHECK_FIELD(CRealMyRoomMain, m_itemStorageVisible, 0xD98);
	CHECK_FIELD(CRealMyRoomMain, m_itemStorageDlgPos, 0xD9C);
	CHECK_FIELD(CRealMyRoomMain, m_pItemStorageDlg, 0xDA4);
	CHECK_FIELD(CRealMyRoomMain, m_pItemStorageList, 0xDB4);
	CHECK_FIELD(CRealMyRoomMain, m_pItemStoragePangMinus, 0xDAC);
	CHECK_FIELD(CRealMyRoomMain, m_pItemStoragePangPlus, 0xDB0);
	CHECK_FIELD(CRealMyRoomMain, m_pItemStorageClose, 0xDC4);
	CHECK_FIELD(CRealMyRoomMain, m_pItemSlotBox, 0x384);
	CHECK_FIELD(CRealMyRoomMain, m_pEquipCancelButton, 0x388);
	CHECK_FIELD(CRealMyRoomMain, m_pEquipResetButton, 0x38C);
	CHECK_FIELD(CRealMyRoomMain, m_pItemStoragePageNext, 0xDBC);
	CHECK_FIELD(CRealMyRoomMain, m_pItemStoragePagePrev, 0xDC0);
	CHECK_FIELD(CRealMyRoomMain, m_pCardPackListBox, 0xDCC);
	CHECK_FIELD(CRealMyRoomMain, m_pSelectedName, 0x2E8);
	CHECK_FIELD(CRealMyRoomMain, m_pStatAllViewer, 0x2F0);
	CHECK_FIELD(CRealMyRoomMain, m_pTabTxt, 0x308);
	CHECK_FIELD(CRealMyRoomMain, m_pSelFrame, 0x31C);
	CHECK_FIELD(CRealMyRoomMain, m_pPowerBar, 0x32C);
	CHECK_FIELD(CRealMyRoomMain, m_pCurveBar, 0x33C);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureLeftWnd, 0x670);
	CHECK_FIELD(CRealMyRoomMain, m_pStatPartBitmap, 0x4E0);
	CHECK_FIELD(CRealMyRoomMain, m_pStatAllBitmap, 0x504);
	CHECK_FIELD(CRealMyRoomMain, m_pMoveGift, 0x7AC);
	CHECK_FIELD(CRealMyRoomMain, m_cardTypeId, 0xBA0);
	CHECK_FIELD(CRealMyRoomMain, m_pPowerEdit, 0x340);
	CHECK_FIELD(CRealMyRoomMain, m_pControlEdit, 0x344);
	CHECK_FIELD(CRealMyRoomMain, m_pImpactEdit, 0x348);
	CHECK_FIELD(CRealMyRoomMain, m_pSpinEdit, 0x34C);
	CHECK_FIELD(CRealMyRoomMain, m_pCurveEdit, 0x350);
	CHECK_FIELD(CRealMyRoomMain, m_pItemDescBg, 0x35C);
	CHECK_FIELD(CRealMyRoomMain, m_pStatFrame, 0x3A4);
	CHECK_FIELD(CRealMyRoomMain, m_pStatText, 0x3A8);
	CHECK_FIELD(CRealMyRoomMain, m_pBtnMenu, 0x420);
	CHECK_FIELD(CRealMyRoomMain, m_pMenuDlg, 0x424);
	CHECK_FIELD(CRealMyRoomMain, m_pDowngradeDlg, 0x430);
	CHECK_FIELD(CRealMyRoomMain, m_pGiftOpenDlg, 0x444);
	CHECK_FIELD(CRealMyRoomMain, m_pCardUseDlg, 0x44C);
	CHECK_FIELD(CRealMyRoomMain, m_pItemAttachCardDlg, 0x450);
	CHECK_FIELD(CRealMyRoomMain, m_pOpenCardPackDlg, 0x454);
	CHECK_FIELD(CRealMyRoomMain, m_pMenuIcon, 0x5CC);
	CHECK_FIELD(CRealMyRoomMain, m_pMenuText, 0x600);
	CHECK_FIELD(CRealMyRoomMain, m_pMenuOver, 0x634);
	CHECK_FIELD(CRealMyRoomMain, m_pMenuDown, 0x638);
	CHECK_FIELD(CRealMyRoomMain, m_pAuthorityDlg, 0x47C);
	CHECK_FIELD(CRealMyRoomMain, m_pStatPointBitmap, 0x4A0);
	CHECK_FIELD(CRealMyRoomMain, m_pItemSlotBitmap, 0x560);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureAbilityDlg, 0x480);
	CHECK_FIELD(CRealMyRoomMain, m_dialogsReady, 0x2C4);
	CHECK_FIELD(CRealMyRoomMain, m_itemIconMap, 0x6A8);
	CHECK_FIELD(CRealMyRoomMain, m_pFurniturePageCountWnd, 0x3C4);
	CHECK_FIELD(CRealMyRoomMain, m_furniturePageCount, 0x7A8);
	CHECK_FIELD(CRealMyRoomMain, m_pGiftDescEdit, 0x3A0);
	CHECK_FIELD(CRealMyRoomMain, m_pItemDescBg2, 0x364);
	CHECK_FIELD(CRealMyRoomMain, m_pItemDescEdit, 0x360);
	CHECK_FIELD(CRealMyRoomMain, m_pItemDescBottom, 0x570);
	CHECK_FIELD(CRealMyRoomMain, m_itemDescPos, 0x788);
	CHECK_FIELD(CRealMyRoomMain, m_equipItem, 0x984);
	CHECK_FIELD(CRealMyRoomMain, m_pFortuneDlg, 0x440);
	CHECK_FIELD(CRealMyRoomMain, m_pItemStoragePang, 0xDA8);
	CHECK_FIELD(CRealMyRoomMain, m_pItemStorageIndexMsg, 0xDB8);
	CHECK_FIELD(CRealMyRoomMain, m_pItemStorageDragDrop, 0xDC8);
	CHECK_FIELD(CRealMyRoomMain, m_pFurniturePageUpButton, 0x3B8);
	CHECK_FIELD(CRealMyRoomMain, m_pFurniturePageDownButton, 0x3BC);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureListNumButton, 0x3C8);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureResetButton, 0x3CC);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureBackButton, 0x3D0);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureSaveButton, 0x3D4);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureOutButton, 0x3D8);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureBackGround, 0x3DC);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureScroll, 0x3FC);
	CHECK_FIELD(CRealMyRoomMain, m_pItemNameEdit, 0x36C);
	CHECK_FIELD(CRealMyRoomMain, m_pItemDescEdit2, 0x368);
	CHECK_FIELD(CRealMyRoomMain, m_itemDescEdit2InitPos, 0x6A0);
	CHECK_FIELD(CRealMyRoomMain, m_itemDescEditPos, 0x790);
	CHECK_FIELD(CRealMyRoomMain, m_itemDescEdit2Pos, 0x798);
	CHECK_FIELD(CRealMyRoomMain, m_pFurnitureArrangeButton, 0x380);
	CHECK_FIELD(CRealMyRoomMain, m_pUpgradeButton, 0x390);
	CHECK_FIELD(CRealMyRoomMain, m_pDowngradeButton, 0x394);
	CHECK_FIELD(CRealMyRoomMain, m_pNoteDlg, 0x43C);
END_LAYOUT_CHECK
CHECK_SIZE(CRealMyRoomMain, 0xDD0);

#endif // PG_CLIENT_PROJECTG_REALMYROOMMAIN_H
