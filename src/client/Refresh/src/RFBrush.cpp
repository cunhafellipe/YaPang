// Original: client\Refresh\src\RFBrush.cpp (JP 983 exe, unit bounds in
// jp/units.tsv; no assert path names the file, the name is the class's)
#include <new>

#include "RFBrush.h"
#include "RFInterface.h"
#include "RFManager.h"
#include "RFStringUtil.h"

#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#if !defined(_MSC_VER)
// The port's <malloc.h> has no _alloca yet.
#include <alloca.h>
#define _alloca alloca
#endif

// lib: 0x42a6d0 (RFBrush's scalar deleting destructor), 0x42a750
// (RFNullBrush's), 0x42a7c0 (RFPaintBrush's)

// The brush classes' header inlines (RFBrush, RFNullBrush, RFPaintBrush:
// 0x42a6d0-0x42a7c0, include/RFBrush.h) are the copies this object kept.
//
// 0x42a7f0-0x42a8a0 are five library helpers nothing in the exe calls; they
// lie inside this object's bounds (jp/units.tsv), so they are defined here.
// Their names are invented (none is exported).

// 0x42a7f0
void* AllocateMemory(size_t size) {
	return malloc(size);
}

// 0x42a800
void FreeMemory(void* memory) {
	free(memory);
}

// 0x42a810: formats into an alloca'd buffer and hands it to the
// application's RFInterface::DebugPrint, or to the debugger without one.
void DebugPrintf(wchar_t const* format, ...) {
	va_list args;
	va_start(args, format);
	int length = _vscwprintf(format, args) + 1;
	wchar_t* buffer = (wchar_t*)_alloca(length * sizeof(wchar_t));
	_vsnwprintf_s(buffer, length, length, format, args);
	if (RFManager::s_interface != 0) {
		RFManager::s_interface->DebugPrint(buffer);
	} else {
		OutputDebugStringW(buffer);
	}
}

// 0x42a890: returns the previous code page
unsigned int SetCodePage(unsigned int codePage) {
	unsigned int previous = g_codePage;
	g_codePage = codePage;
	return previous;
}

// 0x42a8a0
unsigned int GetCodePage() {
	return g_codePage;
}

// 0x42a8b0
RFBrush* CreateBrush(ImageSet* imageSet) {
	if (imageSet != 0) {
		switch (imageSet->GetType()) {
		case ImageSetTypes::None:
			return 0;
		case ImageSetTypes::Single:
			return new RFSingleBrush(*static_cast<SingleImageSet*>(imageSet));
		case ImageSetTypes::Frame:
			return new RFFullFrameBrush(*static_cast<FrameImageSet*>(imageSet));
		case ImageSetTypes::NinePatch:
			return new RFPatchBrush(*static_cast<NinePatchImageSet*>(imageSet));
		case ImageSetTypes::Paint:
			return new RFPaintBrush;
		}
	}
	return new RFNullBrush;
}
