include(FetchContent)

if (TARGET fmt)
    return()
endif()

FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 9.1.0
    GIT_SHALLOW ON
)

if (NOT fmt_POPULATED)
    FetchContent_GetProperties(fmt)
    FetchContent_Populate(fmt)
endif()

add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
