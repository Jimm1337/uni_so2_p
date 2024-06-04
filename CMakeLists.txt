cmake_minimum_required(VERSION 3.22)
project(uni_so2_p)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

include(CPM.cmake)

CPMAddPackage(
        NAME fmt
        GITHUB_REPOSITORY fmtlib/fmt
        GIT_TAG 10.2.1
)

CPMAddPackage(
        NAME gsl
        GITHUB_REPOSITORY microsoft/GSL
        GIT_TAG v4.0.0
)

CPMAddPackage(
        NAME raylib
        GITHUB_REPOSITORY raysan5/raylib
        GIT_TAG 5.0
)

find_package(raylib REQUIRED)
find_package(fmt REQUIRED)
find_package(gsl REQUIRED)

link_libraries(raylib fmt::fmt)
include_directories(${raylib_INCLUDE_DIRS})
include_directories(${fmt_INCLUDE_DIRS})
include_directories(${gsl_SOURCE_DIR}/include)

include_directories(include)

ADD_COMPILE_DEFINITIONS(RES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/")

add_subdirectory(src)
