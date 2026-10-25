// Original: client\Refresh\include\RFTypes.h (JP 983 exe; the enum structs the
// windows' accessors and the XML properties (RFProperty.h) share. The file
// name is assumed: no evidence names it.)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFTYPES_H
#define PG_CLIENT_REFRESH_INCLUDE_RFTYPES_H

// Where a window starts when it is shown (RFWnd::SetStartPosition 0x401e10,
// GetStartPosition 0x401e20; RFStartPositionProperty parses it, 0x4650a0).
struct StartPositionTypes {
	enum Type {
		CenterParent = 1,  // RFStartPositionProperty::Parse 0x4650a0 (0 otherwise)
		CenterScreen = 2
	};
};

// What the Esc key does in a window (RFWnd::SetEscKeyAction 0x401e30,
// GetEscKeyAction 0x401e40; RFEscKeyActionProperty parses it, 0x465110).
struct EscKeyActionTypes {
	enum Type {
		Close = 1  // RFEscKeyActionProperty::Parse 0x465110 (0 otherwise)
	};
};

// Vertical text alignment (RFLabel::SetVAlign 0x4146f0, GetVAlign 0x414710;
// RFVAlignProperty parses it, 0x4651a0).
struct VertAlignTypes {
	enum Type {
		Top = 0,     // the converter 0x41a240 compares "Top", "Center", "Bottom"
		Center = 1,
		Bottom = 2
	};
};

// Horizontal text alignment (RFLabel::SetHAlign 0x414700, GetHAlign 0x414720;
// RFHAlignProperty parses it, 0x465180).
struct HorzAlignTypes {
	enum Type {
		Left = 0,    // the converter 0x41a170 compares "Left", "Center", "Right"
		Center = 1,
		Right = 2
	};
};

// How text too long for its box is cut (RFLabel::SetTextTrimmingType
// 0x414740, GetTextTrimmingType 0x414750; RFTextTrimmingProperty parses it,
// 0x4651c0).
struct TextTrimmingTypes {
	enum Type {
		None = 0,              // the converter 0x41a310 compares "None", "CharacterEllipsis"
		CharacterEllipsis = 1
	};
};

// The direction a list lays its items out in (RFList::GetOrientation 0x423dd0,
// SetOrientation 0x423de0; RFOrientationProperty parses it, 0x4651e0).
struct OrientationTypes {
	enum Type {
		Vertical = 0,   // RFList::UpdateItemPosition 0x4231a0: VericalAlignment
		Horizontal = 1  // HorizontalAlignment
	};
};

#endif // PG_CLIENT_REFRESH_INCLUDE_RFTYPES_H
