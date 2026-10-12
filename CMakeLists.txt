cmake_minimum_required(VERSION 3.10)
project(Yapang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The unit-test environment. When the full client build is wired in here, gate
# it the same way so the host unit tests stay host-only. See tests/README.md.
add_subdirectory(tests)
