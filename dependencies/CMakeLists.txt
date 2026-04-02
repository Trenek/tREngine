find_package(Vulkan REQUIRED)

set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_MINIZIP ON CACHE BOOL "" FORCE)

add_subdirectory(glfw EXCLUDE_FROM_ALL)
add_subdirectory(cglm EXCLUDE_FROM_ALL)
add_subdirectory(freetype EXCLUDE_FROM_ALL)
add_subdirectory(miniaudio EXCLUDE_FROM_ALL)
add_subdirectory(fast_obj EXCLUDE_FROM_ALL)

set(INCLUDE_DIRECTORIES
    "${Vulkan_INCLUDE_DIRS}"
    "${CMAKE_CURRENT_SOURCE_DIR}/stb"
)

set(LINK_LIBRARIES
    "${Vulkan_LIBRARIES}"
    glfw
    cglm
    freetype
    miniaudio
    fast_obj_lib
)

if (WIN32)
    set(LINK_LIBRARIES
        ${LINK_LIBRARIES}
        -mcrtdll=ucrt
        msvcrt
    )
else ()
    set(LINK_LIBRARIES
        ${LINK_LIBRARIES}
        -lm
    )
endif()

set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} PARENT_SCOPE)
set(LINK_LIBRARIES ${LINK_LIBRARIES} PARENT_SCOPE)
