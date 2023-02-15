include(FetchContent)

if (TARGET gtest)
    return()
endif()

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.13.0
    GIT_SHALLOW ON
)

if (NOT googletest_POPULATED)
    FetchContent_GetProperties(googletest)
    FetchContent_Populate(googletest)
endif()

option(BUILD_GMOCK "" OFF)
option(INSTALL_GTEST "" OFF)
add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

if (MINGW)
    # When cross-compiling with MinGW, make the Google Test-generated
    # executables statically-linked to allow them to run with no extra DLLs.
    target_link_libraries(gtest PUBLIC -static)
    target_link_libraries(gtest_main PUBLIC -static)
endif()
