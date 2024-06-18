add_executable(uni_so2_p main.cpp Object.cpp
        Sprite.cpp
        Projectile.cpp
        Text.cpp
        Grid.cpp
        Canvas.cpp
        Game.cpp)

# Compile options
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(uni_so2_p INTERFACE -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic)
    if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
        target_compile_options(uni_so2_p INTERFACE -g -O0)
    elseif (CMAKE_BUILD_TYPE EQUAL "RELEASE")
        target_compile_options(uni_so2_p INTERFACE -O1)
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(uni_so2_p INTERFACE /W4 /w14640 /permissive-)
    if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
        target_compile_options(uni_so2_p INTERFACE /Zi)
    elseif (CMAKE_BUILD_TYPE EQUAL "RELEASE")
        target_compile_options(uni_so2_p INTERFACE /O1)
    endif ()
endif()
