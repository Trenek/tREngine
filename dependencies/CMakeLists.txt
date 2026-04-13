find_package(Vulkan REQUIRED)

add_subdirectory(glfw EXCLUDE_FROM_ALL)
add_subdirectory(cglm EXCLUDE_FROM_ALL)
add_subdirectory(freetype EXCLUDE_FROM_ALL)
add_subdirectory(fast_obj EXCLUDE_FROM_ALL)

add_library(cgltf "${CMAKE_CURRENT_SOURCE_DIR}/cgltf.c")
target_include_directories(cgltf PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/cgltf")

add_library(stb "${CMAKE_CURRENT_SOURCE_DIR}/stb_image.c")
target_include_directories(stb PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/stb")

set(INCLUDE_DIRECTORIES
    "${Vulkan_INCLUDE_DIRS}"
)

set(LINK_LIBRARIES
    "${Vulkan_LIBRARIES}"
    glfw
    cglm
    freetype
    fast_obj_lib
    cgltf
    stb
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
