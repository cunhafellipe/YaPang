// Original: client\Refresh\src\RFControlFactories.h (JP 983 exe; FILE NAME
// AND GETTER NAMES OURS: the getters are not exported and no string names
// them). The getters of the control factories, each returning its
// function-local static factory; defined in RFControlFactories.cpp
// (0x433b90 + 0xa0 * i, in the controls' alphabetical order), called by
// RFManager::RFManager 0x417490, which registers the factories.
#ifndef PG_CLIENT_REFRESH_SRC_RFCONTROLFACTORIES_H
#define PG_CLIENT_REFRESH_SRC_RFCONTROLFACTORIES_H

class RFAnimButtonFactory;
class RFButtonFactory;
class RFCheckFactory;
class RFComboBoxFactory;
class RFEditFactory;
class RFLabelFactory;
class RFLabelButtonFactory;
class RFListSCFactory;
class RFPictureFactory;
class RFProgressFactory;
class RFScrollBarFactory;
class RFSliderFactory;
class RFStackPanelFactory;
class RFTextAreaSCFactory;
class RFWndExFactory;

RFAnimButtonFactory *GetAnimButtonFactory();    // 0x433b90
RFButtonFactory *GetButtonFactory();            // 0x433c30
RFCheckFactory *GetCheckFactory();              // 0x433cd0
RFComboBoxFactory *GetComboBoxFactory();        // 0x433d70
RFEditFactory *GetEditFactory();                // 0x433e10
RFLabelFactory *GetLabelFactory();              // 0x433eb0
RFLabelButtonFactory *GetLabelButtonFactory();  // 0x433f50
RFListSCFactory *GetListSCFactory();            // 0x433ff0
RFPictureFactory *GetPictureFactory();          // 0x434090
RFProgressFactory *GetProgressFactory();        // 0x434130
RFScrollBarFactory *GetScrollBarFactory();      // 0x4341d0
RFSliderFactory *GetSliderFactory();            // 0x434270
RFStackPanelFactory *GetStackPanelFactory();    // 0x434310
RFTextAreaSCFactory *GetTextAreaSCFactory();    // 0x4343b0
RFWndExFactory *GetWndExFactory();              // 0x434450

#endif
