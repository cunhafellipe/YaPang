// Original: client\Refresh\src\RFFactoryManager.cpp (JP 983 exe; the assert paths)
#include <assert.h>
#include "RFWindowFactoryManager.h"
#include "RFWindowFactory.h"

// lib: 0x417320 (implicit copy constructor), 0x417390 (implicit operator=)

// 0x417150
RFWindowFactoryManager::RFWindowFactoryManager() {
}

// 0x417280
RFWindowFactoryManager::~RFWindowFactoryManager() {
}

// 0x437650
void RFWindowFactoryManager::AddFactory(RFWindowFactory *factory) {
	if (factory) {
		if (_factories.find(factory->m_typeName) == _factories.end()) {
			_factories.insert(std::make_pair(factory->m_typeName, factory));
		}
	}
}

// 0x4377b0
void RFWindowFactoryManager::RemoveFactory(RFWindowFactory *factory) {
	std::map<std::wstring, RFWindowFactory *>::iterator it = _factories.find(factory->m_typeName);
	if (it != _factories.end()) {
		_factories.erase(it);
	}
}

// 0x437820
void RFWindowFactoryManager::RemoveFactory(std::wstring const& typeName) {
	std::map<std::wstring, RFWindowFactory *>::iterator it = _factories.find(typeName);
	if (it != _factories.end()) {
		_factories.erase(it);
	}
}

// 0x437890: an alias resolves to its type name first
RFWindowFactory *RFWindowFactoryManager::GetFactory(std::wstring const& name) const {
	std::wstring typeName;
	std::map<std::wstring, std::wstring>::const_iterator alias = _aliases.find(name);
	if (alias != _aliases.end()) {
		typeName = alias->second;
	} else {
		typeName = name;
	}
	std::map<std::wstring, RFWindowFactory *>::const_iterator it = _factories.find(typeName);
	if (it == _factories.end()) {
		return 0;
	}
	return it->second;
}

// 0x4379d0
void RFWindowFactoryManager::AddTypeAlias(std::wstring const& typeName, std::wstring const& alias) {
	assert(_aliases.find(alias) == _aliases.end());
	_aliases.insert(std::make_pair(alias, typeName));
}

// 0x437ba0
void RFWindowFactoryManager::RemoveTypeAlias(std::wstring const& alias) {
	std::map<std::wstring, std::wstring>::iterator it = _aliases.find(alias);
	if (it != _aliases.end()) {
		_aliases.erase(it);
	}
}
