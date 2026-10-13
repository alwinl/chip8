
add_library(cxxopts INTERFACE)

target_include_directories(cxxopts INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/cxxopts/include
)

add_library(
	rlimgui

	rlImGui/rlImGui.cpp
)

target_include_directories(rlimgui INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/rlImGui
)


add_library(
	imgui

	ImGUI/imgui.cpp
	ImGUI/imgui_demo.cpp
	ImGUI/imgui_draw.cpp
	ImGUI/imgui_tables.cpp
	ImGUI/imgui_widgets.cpp
)

target_include_directories(imgui INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/ImGUI
)

target_link_libraries( rlimgui PRIVATE imgui )
