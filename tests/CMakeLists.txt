# Included from the top-level CMakeLists.txt (add_subdirectory(tests)); it is a
# plain subdirectory and deliberately declares no project() of its own, so CLion
# binds every run/test configuration to the root Yapang project.

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# --- Standalone host unit tests -------------------------------------------
# Tests for dependency-free src units compile the needed .cpp files directly
# instead of linking a shared Yapang lib: the src headers target PE32/Windows
# (they include <windows.h> and pull the Wangreal view stack), so the host
# target resolves those heavy includes to the minimal stand-ins in tests/shim
# and links inert stand-ins for the Fresh/Wangreal units. See README.md.

set(YAPANG_SRC_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/shim
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/client/Fresh/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/client/ProjectG
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/client/Wangreal/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/shared)

# Every .cpp the unit under test needs, compiled on the host. FrArea.cpp is
# the real decompiled unit; the Fresh/Wangreal units it calls are supplied
# as inert stand-ins by the test executable (Fresh/FrArea_standins.cpp), so
# they are deliberately NOT listed here.
add_library(YapangHostUnit STATIC
        ../src/client/Fresh/source/FrArea.cpp
)
target_include_directories(YapangHostUnit PRIVATE ${YAPANG_SRC_INCLUDES})

add_executable(YapangHostUnitTest
        catch.hpp
        catch_main.cpp
        Fresh/test_FrArea.cpp
        Fresh/FrArea_standins.cpp
)
target_include_directories(YapangHostUnitTest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${YAPANG_SRC_INCLUDES})
target_link_libraries(YapangHostUnitTest PRIVATE YapangHostUnit)

if(MSVC)
    # Native __declspec and strcmpi; nothing extra to do.
else()
    # The decompiled sources use MSVC extensions (__declspec(novtable/thread))
    # and the Windows-only strcmpi. clang/gcc accept the former with
    # -fms-extensions; map the latter onto strcasecmp.
    target_compile_options(YapangHostUnit PRIVATE
            -fms-extensions -Wno-ignored-attributes -Wno-format)
    target_compile_definitions(YapangHostUnit PRIVATE strcmpi=strcasecmp)

    target_compile_options(YapangHostUnitTest PRIVATE
            -fms-extensions -Wno-ignored-attributes -Wno-format)
    target_compile_definitions(YapangHostUnitTest PRIVATE strcmpi=strcasecmp)
endif()
