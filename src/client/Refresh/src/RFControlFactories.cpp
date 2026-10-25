// Original: client\Refresh\src\RFControlFactories.cpp (JP 983 exe; FILE NAME OURS: no
// assert or __FILE__ string names this object). The object 0x432660-0x4344e9
// (jp/units.tsv): one alphabetical run, for each control AnimButton .. WndEx,
// of its factory's constructor (Create - 0x50, an LTCG copy whose this is the
// getter's static), Create, Destroy, scalar deleting destructor ??_G and an
// LTCG copy of the implicit destructor (this in esi); then the 15 getters
// 0x433b90 + 0xa0 * i and 0x434450 (function-local statics), right before
// RFAnimButton.cpp (0x4344f0). The factory vtables 0xd64ef4-0xd64fd4 are one
// alphabetical block too, apart from the controls' own vtables.
// The getters are free functions (names ours), declared in
// RFControlFactories.h; RFManager::RFManager registers the factories with them.
// lib: 0x432740, 0x432790, 0x4328a0, 0x4328f0, 0x432a20, 0x432a70, 0x432ba0,
// 0x432bf0, 0x432d00, 0x432d50, 0x432e60, 0x432eb0, 0x432fc0, 0x433010,
// 0x433110, 0x433160, 0x433260, 0x4332b0, 0x4333b0, 0x433400, 0x433510,
// 0x433560, 0x4336a0, 0x4336f0, 0x4337f0, 0x433840, 0x433990, 0x4339e0,
// 0x433b10, 0x433b60 (the factories' compiler-generated scalar deleting
// destructors and implicit destructors)
#include <new>

#include "RFAnimButton.h"
#include "RFButton.h"
#include "RFCheck.h"
#include "RFComboBox.h"
#include "RFControlFactories.h"
#include "RFEdit.h"
#include "RFLabel.h"
#include "RFLabelButton.h"
#include "RFListSC.h"
#include "RFPicture.h"
#include "RFProgress.h"
#include "RFScrollBar.h"
#include "RFSlider.h"
#include "RFStackPanel.h"
#include "RFTextAreaSC.h"
#include "RFWndEx.h"

// 0x432660
RFAnimButtonFactory::RFAnimButtonFactory() : RFWindowFactory(RFAnimButton::TypeName) {
}

// 0x4326b0
RFWnd *RFAnimButtonFactory::Create() {
	return new RFAnimButton;
}

// 0x432720
void RFAnimButtonFactory::Destroy(RFWnd *wnd) {
	delete wnd;
}

// 0x4327c0
RFButtonFactory::RFButtonFactory() : RFWindowFactory(RFButton::TypeName) {
}

// 0x432810
RFWnd *RFButtonFactory::Create() {
	return new RFButton;
}

// 0x432880
void RFButtonFactory::Destroy(RFWnd *wnd) {
	delete wnd;
}

// 0x432920
RFCheckFactory::RFCheckFactory() : RFWindowFactory(RFCheck::TypeName) {
}

// 0x432970
RFWnd *RFCheckFactory::Create() {
	return new RFCheck;
}

// 0x432a00
void RFCheckFactory::Destroy(RFWnd *wnd) {
	delete wnd;
}

// 0x432aa0
RFComboBoxFactory::RFComboBoxFactory() : RFWindowFactory(RFComboBox::TypeName) {
}

// 0x432af0
RFWnd *RFComboBoxFactory::Create() {
	return new RFComboBox;
}

// 0x432b80
void RFComboBoxFactory::Destroy(RFWnd *wnd) {
	delete wnd;
}

// 0x432c20
RFEditFactory::RFEditFactory() : RFWindowFactory(RFEdit::TypeName) {
}

// 0x432c70
RFWnd *RFEditFactory::Create() {
	return new RFEdit;
}

// 0x432ce0
void RFEditFactory::Destroy(RFWnd *wnd) {
	delete wnd;
}

// 0x432d80
RFLabelFactory::RFLabelFactory() : RFWindowFactory(RFLabel::TypeName) {
}

// 0x432dd0
RFWnd *RFLabelFactory::Create() {
	return new RFLabel;
}

// 0x432e40
void RFLabelFactory::Destroy(RFWnd *wnd) {
	delete wnd;
}

// 0x432ee0
RFLabelButtonFactory::RFLabelButtonFactory() : RFWindowFactory(RFLabelButton::TypeName) {
}

// 0x432f30
RFWnd *RFLabelButtonFactory::Create() {
	return new RFLabelButton;
}

// 0x432fa0
void RFLabelButtonFactory::Destroy(RFWnd *wnd) {
	delete wnd;
}

// 0x433040
RFListSCFactory::RFListSCFactory() : RFWindowFactory(RFListSC::TypeName) {
}

// 0x433090
RFWnd *RFListSCFactory::Create() {
	return new RFListSC;
}

// 0x4330f0
void RFListSCFactory::Destroy(RFWnd *wnd) {
	delete wnd;
}

// 0x433190
RFPictureFactory::RFPictureFactory() : RFWindowFactory(RFPicture::TypeName) {
}

// 0x4331e0
RFWnd *RFPictureFactory::Create() {
	return new RFPicture;
}

// 0x433240
void RFPictureFactory::Destroy(RFWnd *wnd) {
	delete wnd;
}

// 0x4332e0
RFProgressFactory::RFProgressFactory() : RFWindowFactory(RFProgress::TypeName) {
}

// 0x433330
RFWnd *RFProgressFactory::Create() {
	return new RFProgress;
}

// 0x433390
void RFProgressFactory::Destroy(RFWnd *wnd) {
	delete wnd;
}

// 0x433430
RFScrollBarFactory::RFScrollBarFactory() : RFWindowFactory(RFScrollBar::TypeName) {
}

// 0x433480
RFWnd *RFScrollBarFactory::Create() {
	return new RFScrollBar;
}

// 0x4334f0
void RFScrollBarFactory::Destroy(RFWnd *wnd) {
	delete wnd;
}

// 0x433590
RFSliderFactory::RFSliderFactory() : RFWindowFactory(RFSlider::TypeName) {
}

// 0x4335e0
RFWnd *RFSliderFactory::Create() {
	return new RFSlider;
}

// 0x433680
void RFSliderFactory::Destroy(RFWnd *wnd) {
	delete wnd;
}

// 0x433720
RFStackPanelFactory::RFStackPanelFactory() : RFWindowFactory(RFStackPanel::TypeName) {
}

// 0x433770
RFWnd *RFStackPanelFactory::Create() {
	return new RFStackPanel;
}

// 0x4337d0
void RFStackPanelFactory::Destroy(RFWnd *wnd) {
	delete wnd;
}

// 0x433870
RFTextAreaSCFactory::RFTextAreaSCFactory() : RFWindowFactory(RFTextAreaSC::TypeName) {
}

// 0x4338c0
RFWnd *RFTextAreaSCFactory::Create() {
	return new RFTextAreaSC;
}

// 0x433970
void RFTextAreaSCFactory::Destroy(RFWnd *wnd) {
	delete wnd;
}

// 0x433a10
RFWndExFactory::RFWndExFactory() : RFWindowFactory(RFWndEx::TypeName) {
}

// 0x433a60
RFWnd *RFWndExFactory::Create() {
	return new RFWndEx;
}

// 0x433af0
void RFWndExFactory::Destroy(RFWnd *wnd) {
	delete wnd;
}

// 0x433b90
RFAnimButtonFactory *GetAnimButtonFactory() {
	static RFAnimButtonFactory factory;
	return &factory;
}

// 0x433c30
RFButtonFactory *GetButtonFactory() {
	static RFButtonFactory factory;
	return &factory;
}

// 0x433cd0
RFCheckFactory *GetCheckFactory() {
	static RFCheckFactory factory;
	return &factory;
}

// 0x433d70
RFComboBoxFactory *GetComboBoxFactory() {
	static RFComboBoxFactory factory;
	return &factory;
}

// 0x433e10
RFEditFactory *GetEditFactory() {
	static RFEditFactory factory;
	return &factory;
}

// 0x433eb0
RFLabelFactory *GetLabelFactory() {
	static RFLabelFactory factory;
	return &factory;
}

// 0x433f50
RFLabelButtonFactory *GetLabelButtonFactory() {
	static RFLabelButtonFactory factory;
	return &factory;
}

// 0x433ff0
RFListSCFactory *GetListSCFactory() {
	static RFListSCFactory factory;
	return &factory;
}

// 0x434090
RFPictureFactory *GetPictureFactory() {
	static RFPictureFactory factory;
	return &factory;
}

// 0x434130
RFProgressFactory *GetProgressFactory() {
	static RFProgressFactory factory;
	return &factory;
}

// 0x4341d0
RFScrollBarFactory *GetScrollBarFactory() {
	static RFScrollBarFactory factory;
	return &factory;
}

// 0x434270
RFSliderFactory *GetSliderFactory() {
	static RFSliderFactory factory;
	return &factory;
}

// 0x434310
RFStackPanelFactory *GetStackPanelFactory() {
	static RFStackPanelFactory factory;
	return &factory;
}

// 0x4343b0
RFTextAreaSCFactory *GetTextAreaSCFactory() {
	static RFTextAreaSCFactory factory;
	return &factory;
}

// 0x434450
RFWndExFactory *GetWndExFactory() {
	static RFWndExFactory factory;
	return &factory;
}
