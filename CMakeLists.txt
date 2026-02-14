#TODO find way to include mupdf into executable
cmake_minimum_required(VERSION 3.23)
project(reader)
set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

# project resources
install(DIRECTORY "resources/" DESTINATION "resources/")

# executable
qt_add_executable(${PROJECT_NAME})
add_subdirectory("scripts")

set_target_properties(${PROJECT_NAME} PROPERTIES
	WIN32_EXECUTABLE TRUE
	MACOSX_BUNDLE TRUE
)

# libraries
target_link_libraries(${PROJECT_NAME}
	PRIVATE
		mupdf
		Qt6::Core
    		Qt6::Gui
    		Qt6::Widgets
)

install(TARGETS ${PROJECT_NAME}
	BUNDLE  DESTINATION .
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
	TARGET ${PROJECT_NAME}
	OUTPUT_SCRIPT deploy_script
	NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

