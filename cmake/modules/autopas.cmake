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

    if (AUTOPAS_MANUAL_COMMIT)
        set(AUTOPAS_TAG ${AUTOPAS_MANUAL_COMMIT})
    else()
        set(AUTOPAS_TAG 510e7bb43fb0255cc5d64966f7d68754890e8940)
    endif()

    FetchContent_Declare(
            autopasfetch
            GIT_REPOSITORY ${autopasRepoPath}
            GIT_TAG ${AUTOPAS_TAG}
    )

    # Get autopas source and binary directories from CMake project
    FetchContent_GetProperties(autopasfetch)

    if (NOT autopasfetch_POPULATED)
        FetchContent_Populate(autopasfetch)

        add_subdirectory(${autopasfetch_SOURCE_DIR} ${autopasfetch_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif ()

    set(AUTOPAS_LIB "autopas")
else ()
    message(STATUS "Not using AutoPas.")
    set(AUTOPAS_LIB "")
endif ()
