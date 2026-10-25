// Original: d:\build\custom\temp\code\shared\luasystem\luatablehelper.h
#ifndef PG_SHARED_LUASYSTEM_LUATABLEHELPER_H
#define PG_SHARED_LUASYSTEM_LUATABLEHELPER_H

#include "lua_tinker/lua_tinker.h"
#include "defines.h"

namespace lua_system {

// A Lua table handle with the state it lives in.
class lua_table_helper {
	LAYOUT_CHECK_FRIEND;
public:
	// luatablehelper.h:25
	lua_table_helper(lua_tinker::table table) : m_table(table), m_L(table.m_obj->m_L) {}

	// luatablehelper.h:28
	lua_table_helper(const lua_table_helper &other) : m_table(other.m_table), m_L(other.m_L) {}
	// luatablehelper.h:29
	~lua_table_helper() {}

	// luatablehelper.h:50
	template<typename T>
	T GetValueByName(const char *name) {
		return m_table.get<T>(name);
	}
#ifdef JP_PAKS
	template<typename T> T GetValueByIndex(int index); // JP 0x480010
	int GetArraySize(void); // JP: lua_tinker::call<int>(m_L, "get_array_size", m_table) 0x480180
#endif
private:
	lua_tinker::table m_table; // 0x00
	lua_State *m_L;            // 0x04
};
}
CHECK_SIZE(lua_system::lua_table_helper, 0x08);
BEGIN_LAYOUT_CHECK(lua_system::lua_table_helper)
	CHECK_FIELD(lua_system::lua_table_helper, m_table, 0x00);
	CHECK_FIELD(lua_system::lua_table_helper, m_L, 0x04);
END_LAYOUT_CHECK

#ifdef JP_PAKS
// JP 0x480010: the table's value at an integer key (nil without a table or
// when the key is unset).
template<typename T>
inline T lua_system::lua_table_helper::GetValueByIndex(int index) {
	if (m_table.m_obj) {
		lua_pushnumber(m_L, index);
		lua_gettable(m_L, m_table.m_obj->m_index);
		if (!lua_isnil(m_L, -1)) {
			lua_remove(m_L, -1);
			lua_rawgeti(m_L, m_table.m_obj->m_index, index);
			return lua_tinker::pop<T>(m_L);
		}
		lua_remove(m_L, -1);
	}
	lua_pushnil(m_L);
	return lua_tinker::pop<T>(m_L);
}

// The script's get_array_size(table): the JP lua_tinker::call<int, table>
// (0x480180), which the KR lua_tinker lacks, spelled with the Lua C API.
inline int lua_system::lua_table_helper::GetArraySize(void) {
	lua_pushcclosure(m_L, lua_tinker::on_error, 0);
	int errfunc = lua_gettop(m_L);
	lua_pushstring(m_L, "get_array_size");
	lua_gettable(m_L, LUA_GLOBALSINDEX);
	if (lua_isfunction(m_L, -1)) {
		lua_tinker::push(m_L, m_table);
		if (lua_pcall(m_L, 1, 1, errfunc) != 0) {
			lua_pop(m_L, 1);
		}
	} else {
		lua_tinker::print_error(m_L, "lua_tinker::call() attempt to call global `%s' (not a function)", "get_array_size");
	}
	lua_remove(m_L, -2);
	return lua_tinker::pop<int>(m_L);
}
#endif

#endif
