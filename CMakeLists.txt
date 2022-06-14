cmake_minimum_required(VERSION "3.23.1")

project("Wwizard")


file(GLOB_RECURSE HEADER_QUERYMODULE
	Wwizard/Wwizard/QueryEditorModule.h)

file(GLOB_RECURSE SRC_PLUGIXML
	Wwizard/ThirdParty/pugixml-1.12/src/pugixml.cpp)

file(GLOB_RECURSE SRC_MODULES
	Wwizard/Wwizard/BaseModule.cpp
	Wwizard/Wwizard/ColorCodingModule.cpp
	Wwizard/Wwizard/NamingConventionModule.cpp
	Wwizard/Wwizard/ToolboxModule.cpp
	Wwizard/Wwizard/SortOriginalsModule.cpp
	Wwizard/Wwizard/QueryEditorModule.cpp)

file(GLOB_RECURSE SRC_IMGUI_IMNODES
Wwizard/ThirdParty/ImNodes/ImNodes.cpp
Wwizard/ThirdParty/ImNodes/ImNodesEz.cpp
)

file(GLOB_RECURSE SRC_IMGUI
	Wwizard/ThirdParty/backends/imgui_impl_win32.cpp
	Wwizard/ThirdParty/backends/imgui_impl_dx10.cpp
	Wwizard/ThirdParty/backends/imgui_impl_win32.h
	Wwizard/ThirdParty/backends/imgui_impl_dx10.h
	)

file(GLOB_RECURSE SRC_GUI
	Wwizard/Wwizard/ColorCodingLayout.cpp
	Wwizard/Wwizard/GUI.cpp
	Wwizard/Wwizard/HomeLayout.cpp
	Wwizard/Wwizard/NamingConventionLayout.cpp
	Wwizard/Wwizard/QueryEditorLayout.cpp
	Wwizard/Wwizard/SettingLayout.cpp
	Wwizard/Wwizard/SortOriginalsLayout.cpp
	Wwizard/Wwizard/ToolboxLayout.cpp)


source_group("Source Files/GUI" FILES ${SRC_GUI})
source_group("Source Files/ImGui/ImNodes" FILES ${SRC_IMGUI_IMNODES})
source_group("Source Files" FILES ${SRC})
source_group("Source Files/plugixml" FILES ${SRC_PLUGIXML})
source_group("Source Files/Modules" FILES ${SRC_MODULES})
source_group("Source Files/ImGui" FILES ${SRC_IMGUI})

file(GLOB_RECURSE SRC
Wwizard/Wwizard/Application.cpp
Wwizard/Wwizard/main.cpp
Wwizard/Wwizard/SettingHandler.cpp
Wwizard/Wwizard/WwizardWwiseClient.cpp
${SRC_MODULES}
${SRC_PLUGIXML} 
${SRC_IMGUI}
${SRC_GUI}
)


file(GLOB_RECURSE IMGUI
	Wwizard/ThirdParty/imgui.cpp
	Wwizard/ThirdParty/imgui.h
	Wwizard/ThirdParty/imconfig.h
	Wwizard/ThirdParty/imgui_internal.h
	Wwizard/ThirdParty/imstb_rectpack.h
	Wwizard/ThirdParty/imstb_textedit.h
	Wwizard/ThirdParty/imstb_truetype.h
	Wwizard/ThirdParty/imgui_demo.cpp
	Wwizard/ThirdParty/imgui_draw.cpp
	Wwizard/ThirdParty/imgui_stdlib.cpp
	Wwizard/ThirdParty/imgui_stdlib.h
	Wwizard/ThirdParty/imgui_tables.cpp
	Wwizard/ThirdParty/imgui_widgets.cpp)

source_group("ImGui" FILES ${IMGUI})

file(GLOB_RECURSE HEADER_GUI
	Wwizard/Wwizard/ColorCodingLayout.h
	Wwizard/Wwizard/GUI.h
	Wwizard/Wwizard/HomeLayout.h
	Wwizard/Wwizard/NamingConventionLayout.h
	Wwizard/Wwizard/QueryEditorLayout.h
	Wwizard/Wwizard/SettingLayout.h
	Wwizard/Wwizard/SortOriginalsLayout.h
	Wwizard/Wwizard/ToolboxLayout.h
	Wwizard/Wwizard/BaseLayout.h)
source_group("Header Files/GUI" FILES ${HEADER_GUI})

file(GLOB_RECURSE HEADER_MODULES
	Wwizard/Wwizard/BaseModule.h
	Wwizard/Wwizard/SortOriginalsModule.h
	Wwizard/Wwizard/ToolboxModule.h
)
source_group("Header Files/Modules" FILES ${HEADER_MODULES})

file(GLOB_RECURSE HEADER_MODULES_COLORCODING
	Wwizard/Wwizard/ColorCodingModule.h
	Wwizard/Wwizard/ColorSetting.h
	Wwizard/Wwizard/ColorSettingMode.h)
	source_group("Header Files/Modules/ColorCodingModule" FILES ${HEADER_MODULES_COLORCODING})

file(GLOB_RECURSE HEADER_MODULES_HELPER
	Wwizard/Wwizard/GuiHelper.h
	Wwizard/Wwizard/helper.h
	Wwizard/Wwizard/ResultFile.h
	Wwizard/Wwizard/WwiseColors.h)
source_group("Header Files/Modules/Helper" FILES ${HEADER_MODULES_HELPER})

file(GLOB_RECURSE HEADER_MODULES_NAMINGCONVENTION
	Wwizard/Wwizard/NamingConventionModule.h
	Wwizard/Wwizard/NamingIssues.h
	Wwizard/Wwizard/NamingSetting.h)
	source_group("Header Files/Modules/NamingConventionModule" FILES ${HEADER_MODULES_NAMINGCONVENTION})

file(GLOB_RECURSE HEADER_MODULES_QUERYEDITOR
	Wwizard/Wwizard/BaseQueryStructure.h
	Wwizard/Wwizard/MyNode.h
	Wwizard/Wwizard/QueryEditorModule.h
	Wwizard/Wwizard/QueryType.h)
source_group("Header Files/Modules/QueryEditorModule" FILES ${HEADER_MODULES_QUERYEDITOR})

file(GLOB_RECURSE HEADER
	Wwizard/Wwizard/SettingHandler.h
	Wwizard/Wwizard/Application.h
	Wwizard/Wwizard/WwuLookUpData.h
	Wwizard/Wwizard/WwizardWwiseClient.h
)
source_group("Header Files" FILES ${HEADER})



add_executable("${PROJECT_NAME}" ${SRC} ${SRC_IMGUI_IMNODES} ${IMGUI} ${HEADER} ${HEADER_GUI} 
								 ${HEADER_MODULES} ${HEADER_MODULES_COLORCODING} ${HEADER_MODULES_HELPER} ${HEADER_MODULES_NAMINGCONVENTION} ${HEADER_MODULES_QUERYEDITOR})

target_link_libraries("${PROJECT_NAME}" d3d10.lib)
target_link_libraries("${PROJECT_NAME}" d3dcompiler.lib)
target_link_libraries("${PROJECT_NAME}" dxgi.lib)

target_include_directories("${PROJECT_NAME}" PRIVATE  ${PROJECT_SOURCE_DIR}/Wwizard/ThirdParty)
target_include_directories("${PROJECT_NAME}" PRIVATE  ${PROJECT_SOURCE_DIR}/Wwizard/ThirdParty/WwiseSDK/external/rapidjson)
target_include_directories("${PROJECT_NAME}" PRIVATE  ${PROJECT_SOURCE_DIR}/Wwizard/ThirdParty/WwiseSDK/include)
target_include_directories("${PROJECT_NAME}" PRIVATE  ${PROJECT_SOURCE_DIR}/Wwizard/ThirdParty/ImNodes)
target_include_directories("${PROJECT_NAME}" PRIVATE  ${PROJECT_SOURCE_DIR}/Wwizard/ThirdParty/backends)

target_include_directories("${PROJECT_NAME}" PRIVATE ${PROJECT_SOURCE_DIR}/Wwizard/Wwizard)

add_compile_definitions(SOLUTION_DIR="${PROJECT_SOURCE_DIR}")
add_compile_definitions(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_link_libraries("${PROJECT_NAME}" debug ${CMAKE_CURRENT_SOURCE_DIR}/Wwizard/ThirdParty/WwiseSDK/x64_vc160/Debug/lib/AkAutobahn.lib optimized ${CMAKE_CURRENT_SOURCE_DIR}/Wwizard/ThirdParty/WwiseSDK/x64_vc160/Release/lib/AkAutobahn.lib)