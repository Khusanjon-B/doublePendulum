cmake_minimum_required(VERSION 3.16)

project(PhysicsSimulations CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

file(GLOB DEMO_SOURCES CONFIGURE_DEPENDS "*.cpp")

foreach(src ${DEMO_SOURCES})
    get_filename_component(name ${src} NAME_WE)
    add_executable(${name} ${src})
    target_link_libraries(${name} raylib)
endforeach()