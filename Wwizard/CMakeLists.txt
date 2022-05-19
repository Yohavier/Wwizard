cmake_minimum_required(VERSION "3.23.1")

project("Wwizard")

file(GLOB_RECURSE HEADER_QUERYMODULE
	Wwizard/QueryEditorModule.h)

file(GLOB_RECURSE SRC_PLUGIXML
	ThirdParty/pugixml-1.12/src/pugixml.cpp)

file(GLOB_RECURSE SRC_MODULES
	Wwizard/BaseModule.cpp
	Wwizard/ColorCodingModule.cpp
	Wwizard/NamingConventionModule.cpp
	Wwizard/ToolboxModule.cpp
	Wwizard/SortOriginalsModule.cpp
	Wwizard/QueryEditorModule.cpp)

file(GLOB_RECURSE SRC_IMGUI_IMNODES
ThirdParty/ImNodes/ImNodes.cpp
ThirdParty/ImNodes/ImNodesEz.cpp
)

file(GLOB_RECURSE SRC_IMGUI
	ThirdParty/backends/imgui_impl_win32.cpp
	ThirdParty/backends/imgui_impl_dx10.cpp
	ThirdParty/backends/imgui_impl_win32.h
	ThirdParty/backends/imgui_impl_dx10.h
	)

file(GLOB_RECURSE SRC_GUI
	Wwizard/ColorCodingLayout.cpp
	Wwizard/GUI.cpp
	Wwizard/HomeLayout.cpp
	Wwizard/NamingConventionLayout.cpp
	Wwizard/QueryEditorLayout.cpp
	Wwizard/SettingLayout.cpp
	Wwizard/SortOriginalsLayout.cpp
	Wwizard/ToolboxLayout.cpp)


source_group("Source Files/GUI" FILES ${SRC_GUI})
source_group("Source Files/ImGui/ImNodes" FILES ${SRC_IMGUI_IMNODES})
source_group("Source Files" FILES ${SRC})
source_group("Source Files/plugixml" FILES ${SRC_PLUGIXML})
source_group("Source Files/Modules" FILES ${SRC_MODULES})
source_group("Source Files/ImGui" FILES ${SRC_IMGUI})

file(GLOB_RECURSE SRC
Wwizard/Application.cpp
main.cpp
SettingHandler.cpp
WwizardWwiseClient.cpp
${SRC_MODULES}
${SRC_PLUGIXML} 
${SRC_IMGUI}
${SRC_GUI}
)


file(GLOB_RECURSE IMGUI
ThirdParty/imgui.cpp
ThirdParty/imgui.h
ThirdParty/imconfig.h
ThirdParty/imgui_internal.h
ThirdParty/imstb_rectpack.h
ThirdParty/imstb_textedit.h
ThirdParty/imstb_truetype.h
ThirdParty/imgui_demo.cpp
ThirdParty/imgui_draw.cpp
ThirdParty/imgui_stdlib.cpp
ThirdParty/imgui_stdlib.h
ThirdParty/imgui_tables.cpp
ThirdParty/imgui_widgets.cpp)

source_group("ImGui" FILES ${IMGUI})

file(GLOB_RECURSE HEADER_GUI
	Wwizard/ColorCodingLayout.h
	Wwizard/GUI.h
	Wwizard/HomeLayout.h
	Wwizard/NamingConventionLayout.h
	Wwizard/QueryEditorLayout.h
	Wwizard/SettingLayout.h
	Wwizard/SortOriginalsLayout.h
	Wwizard/ToolboxLayout.h
	Wwizard/BaseLayout.h)
source_group("Header Files/GUI" FILES ${HEADER_GUI})

file(GLOB_RECURSE HEADER_MODULES
	Wwizard/BaseModule.h
	Wwizard/SortOriginalsModule.h
	Wwizard/ToolboxModule.h
)
source_group("Header Files/Modules" FILES ${HEADER_MODULES})

file(GLOB_RECURSE HEADER_MODULES_COLORCODING
	Wwizard/ColorCodingModule.h
	Wwizard/ColorSetting.h
	Wwizard/ColorSettingMode.h)
	source_group("Header Files/Modules/ColorCodingModule" FILES ${HEADER_MODULES_COLORCODING})

file(GLOB_RECURSE HEADER_MODULES_HELPER
Wwizard/helper.h
Wwizard/ResultFile.h
Wwizard/WwiseColors.h)
source_group("Header Files/Modules/Helper" FILES ${HEADER_MODULES_HELPER})

file(GLOB_RECURSE HEADER_MODULES_NAMINGCONVENTION
	Wwizard/ContainerSetting.h
	Wwizard/NamingConventionModule.h
	Wwizard/NamingIssues.h
	Wwizard/WwuSetting.h)
	source_group("Header Files/Modules/NamingConventionModule" FILES ${HEADER_MODULES_NAMINGCONVENTION})

file(GLOB_RECURSE HEADER_MODULES_QUERYEDITOR
	Wwizard/BaseQueryStructure.h
	Wwizard/MyNode.h
	Wwizard/QueryEditorModule.h
	Wwizard/QueryType.h)
source_group("Header Files/Modules/QueryEditorModule" FILES ${HEADER_MODULES_QUERYEDITOR})

file(GLOB_RECURSE HEADER
	Wwizard/SettingHandler.h
	Wwizard/Application.h
	Wwizard/WwuLookUpData.h
	Wwizard/WwizardWwiseClient.h
)
source_group("Header Files" FILES ${HEADER})

add_executable("${PROJECT_NAME}" ${SRC} ${SRC_IMGUI_IMNODES} ${IMGUI} ${HEADER} ${HEADER_GUI} 
								 ${HEADER_MODULES} ${HEADER_MODULES_COLORCODING} ${HEADER_MODULES_HELPER} ${HEADER_MODULES_NAMINGCONVENTION} ${HEADER_MODULES_QUERYEDITOR})

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