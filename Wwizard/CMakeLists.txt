cmake_minimum_required(VERSION "3.23.1")

project("Wwizard")

set(SOURCES
	Wwizard/main.cpp
	Wwizard/Application.cpp
Wwizard/BaseModule.cpp
Wwizard/ColorCodingLayout.cpp
Wwizard/ColorCodingModule.cpp
Wwizard/GUI.cpp
Wwizard/HomeLayout.cpp
Wwizard/NamingConventionLayout.cpp
Wwizard/NamingConventionModule.cpp
Wwizard/QueryEditorLayout.cpp
Wwizard/QueryEditorModule.cpp
Wwizard/SettingHandler.cpp
Wwizard/SettingLayout.cpp
Wwizard/SortOriginalsLayout.cpp
Wwizard/SortOriginalsModule.cpp
Wwizard/ToolboxLayout.cpp
Wwizard/ToolboxModule.cpp
Wwizard/WwizardWwiseClient.cpp
ThirdParty/imgui.cpp
ThirdParty/imgui_demo.cpp
ThirdParty/imgui_draw.cpp
ThirdParty/imgui_stdlib.cpp
ThirdParty/imgui_tables.cpp
ThirdParty/imgui_widgets.cpp
ThirdParty/ImNodes/ImNodes.cpp
ThirdParty/ImNodes/ImNodesEz.cpp
ThirdParty/backends/imgui_impl_win32.cpp
ThirdParty/backends/imgui_impl_dx10.cpp
ThirdParty/pugixml-1.12/src/pugixml.cpp
)

add_executable("${PROJECT_NAME}" ${SOURCES})

target_link_libraries("${PROJECT_NAME}" ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/WwiseSDK/x64_vc160/Debug/lib/AkAutobahn.lib)

target_link_libraries("${PROJECT_NAME}" d3d10.lib)
target_link_libraries("${PROJECT_NAME}" d3dcompiler.lib)
target_link_libraries("${PROJECT_NAME}" dxgi.lib)

target_include_directories("${PROJECT_NAME}" PRIVATE  ${PROJECT_SOURCE_DIR}/ThirdParty)
target_include_directories("${PROJECT_NAME}" PRIVATE  ${PROJECT_SOURCE_DIR}/ThirdParty/WwiseSDK/external/rapidjson)
target_include_directories("${PROJECT_NAME}" PRIVATE  ${PROJECT_SOURCE_DIR}/ThirdParty/WwiseSDK/include)
target_include_directories("${PROJECT_NAME}" PRIVATE  ${PROJECT_SOURCE_DIR}/ThirdParty/ImNodes)
target_include_directories("${PROJECT_NAME}" PRIVATE  ${PROJECT_SOURCE_DIR}/ThirdParty/backends)

target_include_directories("${PROJECT_NAME}" PRIVATE ${PROJECT_SOURCE_DIR}/Wwizard)

add_compile_definitions(SOLUTION_DIR="${PROJECT_SOURCE_DIR}")
add_compile_definitions(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)