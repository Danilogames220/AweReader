#TODO find way to include mupdf into executable
cmake_minimum_required(VERSION 3.23)
project(reader)

# resources
install(DIRECTORY "resources/" DESTINATION "resources/")

find_package(PkgConfig)
pkg_check_modules(GTKMM_VARS REQUIRED IMPORTED_TARGET gtkmm-4.0)


# executable
add_executable(${PROJECT_NAME})
add_subdirectory("scripts")


# libraries
target_link_libraries(${PROJECT_NAME}
	PRIVATE
		PkgConfig::GTKMM_VARS
		mupdf
)

