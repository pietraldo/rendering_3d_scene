add_executable (rendering_3d_scene main.cpp ../externals/stb_image/stb_image.h)


target_include_directories(rendering_3d_scene PRIVATE ${CMAKE_SOURCE_DIR}/rendering_3d_scene)
target_include_directories(rendering_3d_scene PRIVATE ${CMAKE_SOURCE_DIR}/externals/imgui/imgui/backends)
target_link_libraries(rendering_3d_scene glfw glad imgui glm)


