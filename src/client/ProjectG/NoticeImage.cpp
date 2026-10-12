// Original: d:\build\custom\temp\code\client\projectg\noticeimage.cpp
#include "NoticeImage.h"

#include <stdio.h>
#include <string.h>
#include <new>
#include <string>

#include "westpak.h"
#include "wresrcmng.h"

// noticeimage.cpp:16
// Reads a notice image file: "name (index x y w h) (index x y w h) ..." per
// line, one sNoticeImage per line, until count images are read.
void LoadNoticeImageFile(const char* fileName, std::vector<sNoticeImage>& images, int count, const char* section) {
	cFile* file = g_resrcmng->GetCFile(fileName, -1);

	if (file != 0) {
		int size = file->Length();
		char* buffer = new char[size + 1];

		file->Read(buffer, size);
		buffer[size] = 0;
		CloseCFile(file);

		char* p = buffer;

		images.clear();

		while (*p) {
			sNoticeImage image;
			std::string token;
			bool skipSpace = true;

			token.clear();

			while (*p && *p != '\n') {
				if (*p == '(') {
					skipSpace = false;

					if (image.name[0] == 0) {
						strcpy(image.name, token.c_str());
					}
					token.clear();
				}
				else if (*p == ')') {
					sNoticeImage::sBn bn;
					skipSpace = true;
					sscanf(token.c_str(), "%d %f %f %f %f", &bn.index, &bn.x, &bn.y, &bn.w, &bn.h);
					image.buttons.push_back(bn);
				}
				else if (*p != '\r') {
					if (*p != ' ' || !skipSpace) {
						token += *p;
					}
				}
				p++;
			}

			if (image.name[0] == 0) {
				strcpy(image.name, token.c_str());
			}
			int length = strlen(image.name);

			if (length != 0) {
				images.push_back(image);
			}

			if (images.size() == count) {
				break;
			}

			if (*p) {
				p++;
			}
		}

		delete[] buffer;
	}
}
