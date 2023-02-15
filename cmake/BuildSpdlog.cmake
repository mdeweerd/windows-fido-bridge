include(BuildFmt)

include(FetchContent)

if (TARGET spdlog)
    return()
endif()

FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.11.0
    GIT_SHALLOW ON
)

if (NOT spdlog_POPULATED)
    FetchContent_GetProperties(spdlog)
    FetchContent_Populate(spdlog)
endif()

option(SPDLOG_FMT_EXTERNAL "" ON)
add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})

if (MINGW)
    target_link_libraries(spdlog PUBLIC fmt)
endif()
