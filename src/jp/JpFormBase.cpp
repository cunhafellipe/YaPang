// JP-only game code: FrFormBase, the JP notice form base (JpFormBase.h).
#include "JpFormBase.h"

#include "ClientSetting.h"
#include "GolfTask.h"
#include "ObjectFactory.h"
#include "RFTextManager.h"
#include "TaskManager.h"
#include "bitmap.h"
#include "fresh.h"
#include "frarea.h"
#include "frbutton.h"
#include "frcombobox.h"
#include "fredit.h"
#include "frframe.h"
#include "frgaugebar.h"
#include "frgraphicinterface.h"
#include "frgroupbox.h"
#include "frlistbox.h"
#include "frstatic.h"
#include "frtabbutton.h"
#include "frtextbutton.h"
#include "frviewer.h"
#include "frwnd.h"
#include "frwndmanager.h"
#include "wview.h"

// lib: 0x4fdcd0, 0x4fdce0 (this-adjusting deleting-destructor thunks),
// 0xccb6d0 (message-map initializer), 0x4f0bb0 (std::map clear),
// 0x4f0cd0 (std::map erase), 0x4fd810 (std::vector erase), 0x4fd880
// (std::vector destroy), 0x49a910 (std::map lower bound), 0x4fbf60
// (shared_ptr<sImage> from a pointer), 0x4fdcf0 (std::vector push_back),
// 0x4fbdd0 (shared_ptr<sImage> release)

// 0xf145f8: the JP global rect the layout's rects start from.
extern const WRect data_f145f8;

// lib: 0xccb690 (m_RTTI's dynamic initializer, compiler-generated from
// this definition: WRTTI::WRTTI 0xa4c140, base FrForm)
const WRTTI FrFormBase::m_RTTI("FrFormBase", &FrForm::m_RTTI);

// 0x4fefb0
IObject* FrFormBaseMakeInstance(void) {
	return new FrFormBase;
}

// 0x4ff020 (static init 0xccb6b0): registration with the form factory
struct __sFrFormBase {
	__sFrFormBase(void) {
		ObjectFactory().AddObjectFunctor(FrFormBaseMakeInstance, "FrFormBase");
	}
};
static __sFrFormBase s_registerFrFormBase;

// 0x4fb5f0
const sFRESH_MSGMAP* FrFormBase::GetMessageMap(void) const {
	return &FrFormBase::_MsgMap;
}

sFRESH_MSGMAP FrFormBase::_MsgMap = { &FrForm::_MsgMap, &FrFormBase::_MsgEntries[0] };

// 0xe08578 (the handlers are stored by the static init 0xccb6d0)
sFRESH_ENTRY FrFormBase::_MsgEntries[] = {
	{ "ok", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrFormBase::OnControlInit },
	{ "cancel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrFormBase::OnControlInit },
	{ "cancel2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrFormBase::OnControlInit },
	{ "message", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrFormBase::OnControlInit },
	{ "image", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrFormBase::OnControlInit },
	{ "caption", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrFormBase::OnControlInit },
	{ "cancel2", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrFormBase::OnFreshCancel },   // FrForm's, JP be5e00
	{ "image", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrFormBase::OnImageOwnerDraw },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x4fb600
bool FrFormBase::Open(FRESH_RESULT_CALLBACK pfnResult, DWORD flag) {
	IsReady();
	return FrForm::Open(pfnResult, flag);
}

// 0x4fb640
void FrFormBase::OnProc(const float dt) {
	if (m_pSubForm) {
		m_pSubForm->OnProc(dt);
	}
}

// 0x4fb670
FrFormBase* FrFormBase::SetSubForm(FrFormBase* pSubForm) {
	if (pSubForm) {
		m_pSubForm = pSubForm;
	}
	return pSubForm;
}

// 0x4fb690
void FrFormBase::SetMessage(const char* message, bool b) {
	FrForm::SetMessage(message, b);
}

// 0x4fb8b0
void FrFormBase::OnImageOwnerDraw(void) {
	DrawImages();
}

// 0x4fba30
void FrFormBase::UpdateScreenSize(void) {
	sOption option = *WSingleton<COption>::Instance()->GetOption();
	m_screenWidth = (float)option.m_screenWidth;
	m_screenHeight = (float)option.m_screenHeight;
	if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject*)__rtti_obj)->IsKindOf(&CGolfTask::m_RTTI) : false) {
		return;
	}
	// 0xd0562c / 0xd05628: 800 x 600 when there is no view.
	if ((g_view ? g_view->GetWidth() : 800.0f) < m_screenWidth) {
		m_screenWidth = g_view ? g_view->GetWidth() : 800.0f;
	}
	if ((g_view ? g_view->GetHeight() : 600.0f) < m_screenHeight) {
		m_screenHeight = g_view ? g_view->GetHeight() : 600.0f;
	}
}

// 0x4fbd50
void FrFormBase::CenterOnScreen(float width, float height, bool flag) {
	UpdateScreenSize();
	SetRect(WRect(m_screenWidth * 0.5 - width * 0.5, m_screenHeight * 0.5 - height * 0.5, width, height));
	m_centred = flag;
}

// 0x4fc2b0
FrWnd* FrFormBase::FindControl(std::string name) {
	std::map<std::string, FrWnd*>::iterator it = m_controls.find(name.c_str());
	if (it == m_controls.end()) {
		return 0;
	}
	return it->second;
}

// 0x4fc410
FrEdit* FrFormBase::FindEdit(std::string name) {
	std::map<std::string, FrWnd*>::iterator it = m_controls.find(name.c_str());
	if (it == m_controls.end()) {
		return 0;
	}
	return (FrEdit*)it->second;
}

// 0x4fc7b0
bool FrFormBase::SetControlVisible(FrFormBase* form, std::string name, bool bVisible) {
	if (form) {
		FrWnd* pWnd = form->FindControl(name);
		if (pWnd) {
			pWnd->SetVisible(bVisible);
			return true;
		}
	}
	return false;
}

// 0x4fc870
bool FrFormBase::SetControlEnable(FrFormBase* form, std::string name, bool bEnable) {
	if (form) {
		FrWnd* pWnd = form->FindControl(name);
		if (pWnd) {
			pWnd->Enable(bEnable);
			return true;
		}
	}
	return false;
}

// 0x4fc930
bool FrFormBase::SetControlFadeOut(FrFormBase* form, std::string name, bool bFadeout) {
	if (form) {
		FrWnd* pWnd = form->FindControl(name);
		if (pWnd) {
			pWnd->SetElementFadeOut(bFadeout);
			return true;
		}
	}
	return false;
}

// 0x4fc9f0
bool FrFormBase::SetControlToolTip(std::string name, std::string text) {
	FrWnd* pWnd = FindControl(name.c_str());
	if (!pWnd) {
		return false;
	}
	pWnd->SetToolTipText(text.c_str());
	return true;
}

// 0x4fcb50
void FrFormBase::LayoutControls(void) {
	FrWnd* pCancel2 = FindControl("cancel2");
	if (pCancel2) {
		pCancel2->SetRect(WRect(GetRect().w + GetRect().x - 53.0, GetRect().y, pCancel2->GetRect().w, pCancel2->GetRect().h));
	}

	WRect captionRect = data_f145f8;
	WRect imageRect = data_f145f8;
	WRect messageRect = data_f145f8;
	FrWnd* pCaption = FindControl("caption");
	if (pCaption) {
		captionRect = pCaption->GetRect();
	}

	bool noImages = m_images.empty();
	FrWnd* pImage = FindControl("image");
	if (pImage) {
		pImage->SetRect(WRect(GetRect().x + 3.0, 10.0 + GetRect().y + captionRect.h, GetRect().w - 12.0,
			noImages ? 0.0f : m_imageHeight + 10.0));
		imageRect = pImage->GetRect();
	}

	FrEdit* pMessage = FindEdit("message");
	if (pMessage) {
		int lineNum = pMessage->GetLineNum();
		pMessage->SetRect(WRect(imageRect.x + 3.0, (noImages ? 40.0 : imageRect.h + 10.0) + imageRect.y,
			imageRect.w + 2.0, lineNum * 20.0 + 20.0));
		messageRect = pMessage->GetRect();
	}

	float height = imageRect.y - captionRect.y + captionRect.h;
	height = height + imageRect.h;
	height = height + (messageRect.x - imageRect.x);
	height = height + messageRect.h;
	height = height + 70.0;
	// The original keeps only a height of 280 or more.
	if (height < 250.0 || height < 280.0f) {
		height = (float)(imageRect.h > 0.0f ? 280 : 250);
	}
	CenterOnScreen(GetRect().w, height, m_centred);

	FrWnd* pOk = FindControl("ok");
	FrWnd* pCancel = FindControl("cancel");
	WRect okRect = pOk ? pOk->GetRect() : WRect(0.0f, 0.0f, 0.0f, 0.0f);
	float okWidth = okRect.w;
	float okHeight = okRect.h;
	WRect cancelRect = pCancel ? pCancel->GetRect() : WRect(0.0f, 0.0f, 0.0f, 0.0f);
	float cancelWidth = cancelRect.w;
	float cancelHeight = cancelRect.h;
	float x = GetRect().w * 0.5 - (cancelWidth + okWidth + GetRect().w * 0.1f) * 0.5 - 0.5;
	if (!pCancel) {
		x = x + 14.0;
	}
	if (pOk) {
		pOk->SetRect(WRect(x + GetRect().x, GetRect().h + GetRect().y - 70.0, okWidth, okHeight));
		if (pCancel) {
			pCancel->SetRect(WRect(okWidth + pOk->GetRect().x + GetRect().w * 0.1f, pOk->GetRect().y, cancelWidth, cancelHeight));
		}
	}
}

// 0x4fcfb0
bool FrFormBase::MoveControl(const std::string& name, float x, float y) {
	FrWnd* pWnd = FindControl(name);
	if (!pWnd) {
		return false;
	}
	const WRect& rect = pWnd->GetRect();
	pWnd->SetRect(WRect(x, y, rect.w, rect.h));
	return true;
}

// 0x4fd050
void FrFormBase::DrawImages(void) {
	if (m_images.size() == 0) {
		return;
	}
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	FrWnd* pImage = FindControl("image");
	if (!pGDI || !pImage) {
		return;
	}
	float offsetX = GetRect().w * 0.5 - m_imageWidth * 0.5;
	for (unsigned int i = 0; i < m_images.size(); ++i) {
		const sImage* image = m_images[i].get();
		const Bitmap* bitmap = image->bitmap;
		if (bitmap) {
			// The bitmap's size is read as unsigned.
			unsigned int width = (unsigned int)bitmap->bi->bmiHeader.biWidth;
			unsigned int height = (unsigned int)bitmap->bi->bmiHeader.biHeight;
			const WRect& imageRect = pImage->GetRect();
			float y = m_imageHeight - (float)height + imageRect.y + image->y;
			float x = imageRect.x + offsetX + image->x;
			pGDI->DrawTexture(bitmap, WRect(x, y, (float)width, (float)height), 0xffffffff, 0);
		}
	}
}

// 0x4fd380
bool FrFormBase::OnInit(void) {
	CenterOnScreen(350.0f, 250.0f, false);
	LayoutControls();
	if (m_pSubForm && m_pSubForm->IsReady()) {
		m_pSubForm->OnInit();
	}
	return FrForm::OnInit();
}

// 0x4fdb60 (0x4fea60 is the scalar deleting destructor)
FrFormBase::~FrFormBase(void) {
	ClearSubForm();
	m_controls.clear();
	m_images.erase(m_images.begin(), m_images.end());
	m_imageWidth = 0.0f;
	m_imageHeight = 0.0f;
	m_nextImageX = 0.0f;
}

// 0x4fdc60
void FrFormBase::OnDraw(void) {
	FrForm::OnDraw();
}

// 0x4fdc70
void FrFormBase::OnOK(void) {
	FrForm::OnOK();
}

// 0x4fdc80
void FrFormBase::OnCancel(void) {
	FrForm::OnCancel();
}

// 0x4fdc90
bool FrFormBase::Close(bool bDestroy) {
	return FrForm::Close(bDestroy);
}

// 0x4fdca0
bool FrFormBase::Close(eFormRet ret, bool bDestroy) {
	return FrForm::Close(ret, bDestroy);
}

// 0x4fdcb0
void FrFormBase::ClearSubForm(void) {
	m_pSubForm = 0;
}

// 0x4fdcc0
bool FrFormBase::IsReady(void) {
	return true;
}

// 0x4fdd60 (instances, OnControlInit's cases: 0x4fdd60 FrForm, 0x4fde10
// FrFrame, 0x4fdec0 FrButton, 0x4fdf70 FrStatic, 0x4fe020 FrTextButton,
// 0x4fe0d0 FrArea, 0x4fe180 FrListBox, 0x4fe230 FrEdit, 0x4fe2e0
// FrComboBox, 0x4fe390 FrGaugeBar, 0x4fe440 FrGaugeBarEx, 0x4fe4f0
// FrGaugeBarImage, 0x4fe5a0 FrViewer, 0x4fe650 FrWnd, 0x4fe700
// FrTabButton, 0x4fe7b0 FrGroupBox, 0x4fe860 FrComboCtlEx)
template <class T>
T* FrFormBase::AddControl(const std::string& name, IObject* pObject) {
	if (m_controls.find(name) == m_controls.end()) {
		T* pControl = DYNAMIC_CAST(T, pObject);
		if (pControl) {
			m_controls[name] = pControl;
			return pControl;
		}
	}
	return 0;
}

// 0x4fe910
FrFormBase::FrFormBase(void) {
	m_pSubForm = 0;
	m_controls.clear();
	m_images.erase(m_images.begin(), m_images.end());
	m_imageWidth = 0.0f;
	m_imageHeight = 0.0f;
	m_centred = false;
	m_nextImageX = 0.0f;
	// 0xd05e64: the KR text "알림" (notice), CP949.
	SetCaption(Jp_LookupText("\xbe\xcb\xb8\xb2"));
	UpdateScreenSize();
}

// 0x4fea90
RF_SHARED_PTR<FrFormBase::sImage> FrFormBase::AddImage(const Bitmap* bitmap) {
	if (!bitmap) {
		return RF_SHARED_PTR<sImage>();
	}
	// The bitmap's size is read as unsigned.
	m_imageWidth = (unsigned int)bitmap->bi->bmiHeader.biWidth + 10.0 + m_imageWidth;
	if (m_imageHeight < (float)(unsigned int)bitmap->bi->bmiHeader.biHeight) {
		m_imageHeight = (float)(unsigned int)bitmap->bi->bmiHeader.biHeight;
	}
	RF_SHARED_PTR<sImage> image(new sImage);
	image->bitmap = bitmap;
	image->x = m_nextImageX;
	image->y = 0.0f;
	m_nextImageX = (unsigned int)bitmap->bi->bmiHeader.biWidth + 10.0 + m_nextImageX;
	m_images.push_back(image);
	return image;
}

// 0x4fedd0 (jump table 0x4fef58 on the GUI type - 1; cases in its block order)
void FrFormBase::OnControlInit(PG_MSGARG param) {
	FrWnd* pWnd = DYNAMIC_CAST(FrWnd, param);
	if (!pWnd) {
		return;
	}
	const std::string& name = pWnd->GetWndName();
	switch (pWnd->GetGuiType()) {
	case 1: AddControl<FrForm>(name, pWnd); break;
	case 8: AddControl<FrFrame>(name, pWnd); break;
	case 7: AddControl<FrButton>(name, pWnd); break;
	case 2: AddControl<FrStatic>(name, pWnd); break;
	case 3: AddControl<FrTextButton>(name, pWnd); break;
	case 11: AddControl<FrArea>(name, pWnd); break;
	case 12: AddControl<FrListBox>(name, pWnd); break;
	case 4: AddControl<FrEdit>(name, pWnd); break;
	case 5: AddControl<FrComboBox>(name, pWnd); break;
	case 13: AddControl<FrGaugeBar>(name, pWnd); break;
	case 14: AddControl<FrGaugeBarEx>(name, pWnd); break;
	case 15: AddControl<FrGaugeBarImage>(name, pWnd); break;
	case 16: AddControl<FrViewer>(name, pWnd); break;
	case 17: AddControl<FrWnd>(name, pWnd); break;   // 0x4fe650 tests FrWnd's RTTI 0xf1aacc
	case 18: AddControl<FrTabButton>(name, pWnd); break;
	case 19: AddControl<FrGroupBox>(name, pWnd); break;
	case 6: AddControl<FrComboCtlEx>(name, pWnd); break;
	}
}

// 0x5476b0
FrFormBase* CreateFormBase(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent) {
	return CreateForm<FrFormBase>(manager, owner, name, parent);
}
