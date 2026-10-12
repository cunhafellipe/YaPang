// Original: d:\build\custom\temp\code\client\projectg\integritycheck.cpp
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <atlbase.h>
#include <map>
#include <string>
#include "IntegrityCheck.h"
#include "SharedDoc.h"
#include "commonutil.h"
#include "wbone.h"
#include "westpak.h"
#include "wpuppet.h"
#include "wresrcmng.h"

// integritycheck.cpp:21
CIntegrityCheck::CIntegrityCheck()
	: m_fp(NULL) {
}

// integritycheck.cpp:25
CIntegrityCheck::~CIntegrityCheck() {
}

// integritycheck.cpp:30
bool CIntegrityCheck::Report(const char* path) {
	m_fp = fopen(path, "wt");
	ReportCharParts();
	fclose(m_fp);
	return true;
}

// integritycheck.cpp:49
void CIntegrityCheck::Log(const char* format, ...) {
	char buffer[2048];
	va_list args;
	va_start(args, format);
	if (_vsnprintf(buffer, sizeof(buffer) - 1, format, args) == -1)
		buffer[sizeof(buffer) - 1] = '\0';
	va_end(args);
	fprintf(m_fp, buffer);
}

// integritycheck.cpp:65
bool CompareByName(const sResName& a, const sResName& b) {
	return strcmp(a.name, b.name) < 0 ? true : false;
}

// integritycheck.cpp:70
void CIntegrityCheck::PrintOkMissing(bool ok) {
	if (ok)
		Log("ok!!\n");
	else
		Log("missing!!\n");
}

// integritycheck.cpp:78
void CIntegrityCheck::PrintOkNg(bool ok) {
	if (ok)
		Log("ok!!\n");
	else
		Log("ng!!\n");
}

// integritycheck.cpp:330
bool CIntegrityCheck::ReportCharParts(void) {
	std::map<unsigned int, IFF_STRUCT::sPart>& parts = ItemManager()->m_partMap;
	std::map<unsigned int, IFF_STRUCT::sChar>& chars = ItemManager()->m_charMap;

	// The base puppets (.bpet) by character type id.
	std::map<unsigned long, WPuppet*> basePuppets;
	int maxNumVtx = 0;
	int maxNumIdx = 0;
	std::map<std::string, std::string> missingPets;
	std::map<std::string, std::string> loadFailedPets;
	std::map<std::string, std::string> matrixOkPets;
	std::map<std::string, std::string> matrixFailedPets;

	Log("* part check - total %d\n\n", parts.size());

	for (std::map<unsigned int, IFF_STRUCT::sPart>::iterator it = parts.begin(); it != parts.end(); ++it) {
		char meshPet[260];

		if (strlen(it->second.model) == 0)
			continue;

		sprintf(meshPet, "%s.mpet", it->second.model);

		Log("\t* part : \"%s\"\n", it->second.m_name);
		Log("\t\t* meshpet : %s - ", meshPet);

		cFile* file = g_resrcmng->GetCFile(meshPet, -1);

		PrintOkMissing(file != NULL);
		if (file == NULL) {
			missingPets[meshPet] = meshPet;
			continue;
		}

		CloseCFile(file);

		Log("\t\t* meshpet loading - ");

		WPuppet* puppet = g_resrcmng->GetPuppet(meshPet, false, true, false);

		PrintOkNg(puppet != NULL);
		if (puppet == NULL) {
			loadFailedPets[meshPet] = meshPet;
			continue;
		}

		for (WBone* bone = puppet->FindMeshBone(NULL); bone != NULL; bone = puppet->FindMeshBone(bone)) {
			int index = 0;
			for (w_mesh* mesh = bone->GetMesh(); mesh != NULL; mesh = mesh->next, index++) {
				Log("\t\t* mesh%d : numVtx(%d), numIdx(%d), drawFlag(0x%08x), numMesh, drawFlag2(0x%08x)\n",
					index, mesh->vtxNum, mesh->indexNum, mesh->drawFlag & 0xFFFC0000, mesh->xiDrawFlag2 & 0x3FFF);

				if (mesh->vtxNum > maxNumVtx)
					maxNumVtx = mesh->vtxNum;

				if (mesh->indexNum > maxNumIdx)
					maxNumIdx = mesh->indexNum;
			}
		}

		int charTypeId = ((it->second.m_typeId >> 18) & 0xFF) | 0x4000000;
		std::map<unsigned long, WPuppet*>::iterator found;
		found = basePuppets.find(charTypeId);
		WPuppet* basePuppet = NULL;

		if (found == basePuppets.end()) {
			std::map<unsigned int, IFF_STRUCT::sChar>::iterator character;
			character = chars.find(charTypeId);

			if (character != chars.end()) {
				basePuppet = g_resrcmng->GetPuppet(MakeStr("%s.bpet", character->second.m_model), false, true, false);

				if (basePuppet != NULL) {
					basePuppets.insert(std::map<unsigned long, WPuppet*>::value_type(charTypeId, basePuppet));
				}
			}
		} else {
			basePuppet = found->second;
		}

		Log("\t\t* compare original matrix - ");

		bool same = false;
		if (basePuppet != NULL)
			same = puppet->CompareBasicMatrix(basePuppet);

		if (same)
			matrixOkPets[meshPet] = meshPet;
		else
			matrixFailedPets[meshPet] = meshPet;

		PrintOkNg(same);

		g_resrcmng->Release(puppet, true);
	}

	Log("\n");

	Log("\t* missing mpets - total %d\n", missingPets.size());
	std::map<std::string, std::string>::iterator name;
	for (name = missingPets.begin(); name != missingPets.end(); ++name)
		Log("\t\t%s\n", name->second.c_str());
	Log("\n");

	Log("\t* loading failed mpets - total %d\n", loadFailedPets.size());
	for (name = loadFailedPets.begin(); name != loadFailedPets.end(); ++name)
		Log("\t\t%s\n", name->second.c_str());
	Log("\n");

	Log("\t* original matrix comparison succeeded mpets - %d\n", matrixOkPets.size());
	for (name = matrixOkPets.begin(); name != matrixOkPets.end(); ++name)
		Log("\t\t%s\n", name->second.c_str());
	Log("\n");

	Log("\t* original matrix comparison failed mpets - %d\n", matrixFailedPets.size());
	for (name = matrixFailedPets.begin(); name != matrixFailedPets.end(); ++name)
		Log("\t\t%s\n", name->second.c_str());
	Log("\n\n");

	Log("\t* maxNumVtx : %d\n", maxNumVtx);
	Log("\t* maxNumIdx : %d\n", maxNumIdx);

	for (std::map<unsigned long, WPuppet*>::iterator puppetIt = basePuppets.begin(); puppetIt != basePuppets.end(); ++puppetIt)
		g_resrcmng->Release(puppetIt->second, true);

	return true;
}
