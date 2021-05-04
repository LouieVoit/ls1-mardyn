# autopas library
option(ENABLE_AUTOPAS "Use autopas library" OFF)
if (ENABLE_AUTOPAS)
    message(STATUS "Using AutoPas.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMARDYN_AUTOPAS")

    # Enable ExternalProject CMake module
    include(FetchContent)

    # Select https (default) or ssh path.
    set(autopasRepoPath https://github.com/AutoPas/AutoPas.git)
    if (GIT_SUBMODULES_SSH)
        set(autopasRepoPath git@github.com:AutoPas/AutoPas.git)
    endif ()

    set(AUTOPAS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(AUTOPAS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(AUTOPAS_ENABLE_ADDRESS_SANITIZER ${ENABLE_ADDRESS_SANITIZER} CACHE BOOL "" FORCE)
    set(AUTOPAS_OPENMP ${OPENMP} CACHE BOOL "" FORCE)
    set(spdlog_ForceBundled ON CACHE BOOL "" FORCE)
    set(Eigen3_ForceBundled ON CACHE BOOL "" FORCE)

    FetchContent_Declare(
            autopasfetch
            GIT_REPOSITORY ${autopasRepoPath}
            # TODO currently this is the tag from PR599. Set this to the merged master.
            GIT_TAG ff092dbae2a8edc5bf5f34c707061dfbcab58965
    )

    # Get autopas source and binary directories from CMake project
    FetchContent_GetProperties(autopas)

    if (NOT autopasfetch_POPULATED)
        FetchContent_Populate(autopasfetch)

        add_subdirectory(${autopasfetch_SOURCE_DIR} ${autopasfetch_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif ()

    set(AUTOPAS_LIB "autopas")
else ()
    message(STATUS "Not using AutoPas.")
    set(AUTOPAS_LIB "")
endif ()
